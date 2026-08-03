#ifndef UPLOAD_IMAGES_HPP
#define UPLOAD_IMAGES_HPP

#include <vector>
#include <string>
#include <unordered_map>
#include <chrono>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

// ──────────────────────────────────────────────
// Data structures
// ──────────────────────────────────────────────

// Represents a single image with its GCS URL and answer coordinates.
struct ImageEntry {
  string filename;     // e.g. "IMG_20241012_141927.jpg"
  string gcs_url;      // public GCS URL
  double latitude;
  double longitude;
};

// One round within a game session.
struct RoundData {
  int image_index;     // index into the global image_index vector
  string gcs_url;      // public GCS URL for the image
  double answer_lat;
  double answer_lng;
  bool submitted;      // true once the player has submitted a guess or skipped
  int score;           // 0 until submitted
};

// A full game session.
struct GameSession {
  string session_id;
  vector<RoundData> rounds;
  int current_round;   // 0-indexed
  int total_score;
  chrono::steady_clock::time_point created_at;
};

// ──────────────────────────────────────────────
// GCS client (talks directly to the GCS JSON REST API over libcurl,
// instead of linking the google-cloud-cpp SDK — which drags in gRPC,
// protobuf, and Abseil, and was responsible for most of the build's size
// and memory footprint). Auth is a standard OAuth2 service-account JWT
// exchange, signed with OpenSSL.
// ──────────────────────────────────────────────

class GcsClient {
public:
    // Loads the service account key from the file at
    // GOOGLE_APPLICATION_CREDENTIALS. Exits with an error if unset/unreadable.
    GcsClient();

    // Uploads a local file to the bucket. Returns the public object URL on
    // success, or an empty string on failure.
    string UploadFile(const string& bucket, const string& object_name,
                       const string& file_path, const string& content_type);

    // Deletes an object from the bucket. Returns true on success.
    bool DeleteObject(const string& bucket, const string& object_name);

private:
    string client_email_;
    string private_key_pem_;
    string token_uri_;

    string cached_token_;
    chrono::system_clock::time_point token_expiry_;

    // Returns a valid bearer token, refreshing it if it's missing or about
    // to expire.
    string GetAccessToken();
};

// ──────────────────────────────────────────────
// GCS helpers (existing)
// ──────────────────────────────────────────────

const char* initialize_bucket();
GcsClient initialize_gcs();

const string write_to_bucket(GcsClient& client, const string& bucket_name,
                             const string& path, const string& object_name);

vector<unordered_map<string, string>> load_images_to_bucket(
  GcsClient& client, const string& directory, const string& bucket_name,
  const string& metadata, int capacity
);

void delete_objects_from_bucket(GcsClient& client, const string& bucket_name,
                                const vector<string>& object_names);

// ──────────────────────────────────────────────
// Game logic (new)
// ──────────────────────────────────────────────

// Scan res/ directory, upload images to GCS, and build an index of all available images.
// Reads each .supplemental-metadata.json file for coordinates.
vector<ImageEntry> load_image_index(GcsClient& client,
                                    const string& bucket_name,
                                    const string& directory,
                                    const string& metadata_suffix);

// Port of the frontend score formula. Returns a score in [0, 5000].
int calculate_score(double user_lat, double user_lng,
                    double answer_lat, double answer_lng);

// Generate a random UUID-style session ID.
string generate_session_id();

#endif // UPLOAD_IMAGES_HPP