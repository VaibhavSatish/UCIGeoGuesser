#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <unordered_map>
#include <cmath>
#include <random>
#include <sstream>
#include <iomanip>
#include <fstream>
#include "laserpants/dotenv/dotenv.h"
#include "google/cloud/storage/client.h"
#include "google/cloud/common_options.h"
#include "upload_images.hpp"
#include <nlohmann/json.hpp>
#include <vector>

// ──────────────────────────────────────────────
// GCS helpers (existing, unchanged)
// ──────────────────────────────────────────────

const char* initialize_bucket() {
    dotenv::init();
    const char* bucket_env = getenv("BUCKET_NAME");
    if (bucket_env == nullptr) {
      std::cerr << "Error: BUCKET_NAME environment variable not set." << std::endl;
      exit(EXIT_FAILURE);
    }
    return bucket_env;
}

gcs::Client initialize_gcs() {
    auto options = google::cloud::Options{};
    auto const* ca_bundle = std::getenv("CURL_CA_BUNDLE");
    if (ca_bundle != nullptr) {
        options.set<google::cloud::CARootsFilePathOption>(ca_bundle);
    }
    auto client = gcs::Client(options);
    return client;
}

const string write_to_bucket(gcs::Client client, const string& bucket_name, const string& path, const string& object_name) {
    cout << "Uploading file: " << path << " to bucket: " << bucket_name << " with object name: " << object_name << endl;
    auto object_metadata = client.UploadFile(path, bucket_name, object_name, gcs::ContentType("image/jpeg"), gcs::WithObjectMetadata(gcs::ObjectMetadata().set_content_disposition("inline")));
    cout << "Upload response received for file: " << path << endl;
    if (!object_metadata) {
        cerr << "Error uploading file: " << object_metadata.status() << endl;
    } else {
        cout << "File uploaded successfully: " << object_metadata->name() << endl;
    }
    string object_link = "https://storage.googleapis.com/" + bucket_name + "/" + object_name;
    cout << "Object link: " << object_link << endl;
    return object_link;
}

vector<unordered_map<string, string>> load_images_to_bucket(gcs::Client client, const string& directory, const string& bucket_name, const string& metadata, int capacity) {
    vector<unordered_map<string, string>> images;
    if (capacity <= 0) {
        cerr << "Capacity must be greater than 0." << endl;
        return images;
    }
    for (const auto& entry : filesystem::directory_iterator(directory)) {
        cout << "Processing file: " << entry.path() << endl;
        if (entry.is_regular_file() && (entry.path().extension() == ".jpg" || entry.path().extension() == ".jpeg")) {
            const string& object_name = entry.path().filename().string();
            const string& object_link = write_to_bucket(client, bucket_name, entry.path().string(), object_name);
            unordered_map<string, string> image_info;
            image_info["image"] = object_link;
            ifstream metadata_file(entry.path().string() + "." + metadata);
            json metadata_json = json::parse(metadata_file);
            image_info["metadata"] = metadata_json["geoData"].dump();
            images.push_back(image_info);
            --capacity;
        }
        if (capacity == 0) {
            break;
        }
    }
    cout << "Finished processing images. Total images loaded: " << images.size() << endl;
    return images;
}

void delete_objects_from_bucket(gcs::Client client, const string& bucket_name, const vector<string>& object_names) {
    for (const auto& object_name : object_names) {
        cout << "Deleting object: " << object_name << " from bucket: " << bucket_name << endl;
        auto status = client.DeleteObject(bucket_name, object_name);
        if (!status.ok()) {
            cerr << "Error deleting object: " << status << endl;
        } else {
            cout << "Object deleted successfully: " << object_name << endl;
        }
    }
}

// ──────────────────────────────────────────────
// Game logic (new)
// ──────────────────────────────────────────────

vector<ImageEntry> load_image_index(gcs::Client client,
                                    const string& bucket_name,
                                    const string& directory,
                                    const string& metadata_suffix) {
    vector<ImageEntry> index;

    for (const auto& entry : filesystem::directory_iterator(directory)) {
        if (!entry.is_regular_file()) continue;
        auto ext = entry.path().extension().string();
        if (ext != ".jpg" && ext != ".jpeg") continue;

        // Look for the matching metadata file
        string meta_path = entry.path().string() + "." + metadata_suffix;
        if (!filesystem::exists(meta_path)) {
            cerr << "Warning: no metadata file for " << entry.path().filename()
                 << ", skipping." << endl;
            continue;
        }

        try {
            ifstream meta_file(meta_path);
            json meta_json = json::parse(meta_file);

            const string& object_name = entry.path().filename().string();
            string object_link = write_to_bucket(client, bucket_name, entry.path().string(), object_name);

            ImageEntry img;
            img.filename = object_name;
            img.gcs_url = object_link;
            img.latitude  = meta_json["geoData"]["latitude"].get<double>();
            img.longitude = meta_json["geoData"]["longitude"].get<double>();
            index.push_back(img);
        } catch (const std::exception& e) {
            cerr << "Error reading metadata/uploading " << entry.path().filename()
                 << ": " << e.what() << endl;
        }
    }

    cout << "Image index loaded & uploaded: " << index.size() << " images." << endl;
    return index;
}


int calculate_score(double user_lat, double user_lng,
                    double answer_lat, double answer_lng) {
    // Direct port of frontend score.tsx formula.
    double lat_diff = std::abs(user_lat - answer_lat);
    double lng_diff = std::abs(user_lng - answer_lng);
    double distance = std::sqrt(lat_diff * lat_diff + lng_diff * lng_diff);

    const int MAX_SCORE = 5000;
    int score;

    if (distance <= 0.00398543) {
        score = static_cast<int>(std::round(
            -MAX_SCORE * std::exp((distance - 0.005) * 650) + 5200));
    } else {
        score = static_cast<int>(std::round(
            MAX_SCORE * std::exp(-658 * (distance - 0.003))));
    }

    if (score >= 4950) {
        score = 5000;
    }
    if (score < 0) {
        score = 0;
    }

    return score;
}

string generate_session_id() {
    // Generate a random UUID-like string: xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<int> dist(0, 15);

    const char* hex = "0123456789abcdef";
    std::ostringstream ss;

    // 8-4-4-4-12 pattern
    const int lengths[] = {8, 4, 4, 4, 12};
    for (int g = 0; g < 5; ++g) {
        if (g > 0) ss << '-';
        for (int i = 0; i < lengths[g]; ++i) {
            ss << hex[dist(gen)];
        }
    }

    return ss.str();
}
