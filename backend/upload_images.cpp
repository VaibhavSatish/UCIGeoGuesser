#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <unordered_map>
#include "laserpants/dotenv/dotenv.h"
#include "google/cloud/storage/client.h"
#include "google/cloud/common_options.h"
#include <vector>


using namespace std;
namespace gcs = ::google::cloud::storage;

const char* initialize_bucket() {
    // Load environment variables from .env file
    dotenv::init();

    // Get the bucket name from environment variable
    const char* bucket_env = getenv("BUCKET_NAME");
   
    return bucket_env;
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
    string object_link = object_metadata->media_link();
    cout << "Object link: " << object_link << endl;
    return object_link;
}
vector<unordered_map<string, string>> load_images_to_bucket(gcs::Client client, const string& directory, const string& bucket_name, int capacity) {
    vector<unordered_map<string, string>> images;
    if (capacity <= 0) {
        cerr << "Capacity must be greater than 0." << endl;
        return images;
    }
    for (const auto& entry : filesystem::directory_iterator(directory)) {
        cout << "Processing file: " << entry.path() << endl;
        if (entry.is_regular_file() && (entry.path().extension() == ".jpg" || entry.path().extension() == ".jpeg")) {
            const string& object_link = write_to_bucket(client, bucket_name, entry.path().string(), entry.path().filename().string());
            unordered_map<string, string> image_info;
            image_info["image"] = object_link;
            --capacity;
            
        }
        
        if (capacity == 0) {
            break;
        }
    }

    return images;
}



int main() {
    const string& directory = "./res";
    cout << "Loading images from directory: " << directory << endl;
    const int capacity = 10; // Set the desired capacity for loading images
    const char* bucket_env = initialize_bucket();
    if (bucket_env == nullptr) {
        std::cerr << "Error: BUCKET_NAME environment variable not set." << std::endl;
        return 1;
    }
    cout << "Using bucket: " << bucket_env << endl;
    auto options = google::cloud::Options{};
    auto const* ca_bundle = std::getenv("CURL_CA_BUNDLE");
    if (ca_bundle != nullptr) {
        options.set<google::cloud::CARootsFilePathOption>(ca_bundle);
    }
    auto client = gcs::Client(options);
    load_images_to_bucket(client, directory, bucket_env, capacity);
    
}
