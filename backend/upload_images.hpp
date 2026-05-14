#ifndef UPLOAD_IMAGES_HPP
#define UPLOAD_IMAGES_HPP

#include <vector>
#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>

using namespace std;
namespace gcs = ::google::cloud::storage;
using json = nlohmann::json;

const char* initialize_bucket();

gcs::Client initialize_gcs();

const string write_to_bucket(gcs::Client client, const string& bucket_name, const string& path, const string& object_name);

vector<unordered_map<string, string>> load_images_to_bucket(
  gcs::Client client, const string& directory, const string& bucket_name,
  const string& metadata, int capacity
);

void delete_objects_from_bucket(gcs::Client client, const string& bucket_name, const vector<string>& object_names);





#endif // UPLOAD_IMAGES_HPP