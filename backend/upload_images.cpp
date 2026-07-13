#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <unordered_map>
#include "laserpants/dotenv/dotenv.h"
#include "google/cloud/storage/client.h"
#include "google/cloud/common_options.h"
#include "upload_images.hpp"
#include <nlohmann/json.hpp>
#include <vector>






const char* initialize_bucket() {
    // Load environment variables from .env file
    dotenv::init();

    // Get the bucket name from environment variable
    const char* bucket_env = getenv("BUCKET_NAME");
    if (bucket_env == nullptr) {
      std::cerr << "Error: BUCKET_NAME environment variable not set." << std::endl;
      exit(EXIT_FAILURE);
    }
   
    return bucket_env;
}

gcs::Client initialize_gcs() {
    // Initialize the Google Cloud Storage client and load images to the bucket
    auto options = google::cloud::Options{};
    auto const* ca_bundle = std::getenv("CURL_CA_BUNDLE");
    if (ca_bundle != nullptr) {
        options.set<google::cloud::CARootsFilePathOption>(ca_bundle);
    }
    auto client = gcs::Client(options);
    
    return client;
}


const string write_to_bucket(gcs::Client client, const string& bucket_name, const string& path, const string& object_name) {
    // Implement the logic to write data to the bucket using the provided bucket name and key
    cout << "Uploading file: " << path << " to bucket: " << bucket_name << " with object name: " << object_name << endl;
    auto object_metadata = client.UploadFile(path, bucket_name, object_name, gcs::IfGenerationMatch(0), gcs::ContentType("image/jpeg"), gcs::WithObjectMetadata(gcs::ObjectMetadata().set_content_disposition("inline")));
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




