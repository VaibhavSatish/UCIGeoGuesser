#include "crow.h"
#include "crow/middlewares/cors.h"
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include "upload_images.cpp"

int main() {
    using namespace std;

    /* Initialize the Crow app with CORS support */
    crow::App<crow::CORSHandler> app;
    auto& cors = app.get_middleware<crow::CORSHandler>().global();
    cors.origin("*").methods("POST"_method);

    /* Constants for accessing images */
    const string IMAGE_DIRECTORY = "./res";
    const string METADATA_FILE = "supplemental-metadata.json";
    const int MAX_IMAGES = 5;
    const int MAX_REQUESTS = 25;

    CROW_ROUTE(app, "/post_images").methods("POST"_method) ([IMAGE_DIRECTORY, METADATA_FILE, MAX_IMAGES, MAX_REQUESTS](const crow::request& req){        
        
        crow::json::wvalue res;
        auto data = crow::json::load(req.body); 

        if (!data) {
          res["error"] = "Malformed JSON payload";
          return res;
        }

        if (!data.has("totalRounds") || data["totalRounds"].t() != crow::json::type::Number)  {
          res["error"] = "Invalid totalRounds value";
          return res;
        }
        
        int rounds  = data["totalRounds"].i();
       
        if (rounds > MAX_REQUESTS) {
          rounds = MAX_REQUESTS;
        }

        auto client = initialize_gcs();
        auto bucket_name = initialize_bucket();
        auto images = load_images_to_bucket(client, IMAGE_DIRECTORY, bucket_name, METADATA_FILE, rounds);
        
        // Prepare the response with image links and metadata
        std::vector<crow::json::wvalue> image_list;
        image_list.reserve(images.size()); // Optimize memory allocation

        
        for (const auto& img_map : images) {
            crow::json::wvalue img_obj;
            for (const auto& [key, val] : img_map) {
                img_obj[key] = val;
            }
            image_list.push_back(std::move(img_obj));
        }

        
        res["images"] = std::move(image_list);

        return res;
    }); 

    CROW_ROUTE(app, "/delete_images").methods("POST"_method) ([](const crow::request& req){        
        
        crow::json::wvalue res;
        auto data = crow::json::load(req.body); 
        if (!data.has("objects") || data["objects"].t() != crow::json::type::List)  {
            res["error"] = "Invalid objects value";
            return res;
        }
        
        vector<string> objects_to_delete;
        for (const auto& obj : data["objects"]) {
            if (obj.t() == crow::json::type::String) {
                objects_to_delete.push_back(obj.s());
            }
        }

        cout << "Received request to delete " << objects_to_delete.size() << " objects." << endl;
        auto client = initialize_gcs();
        delete_objects_from_bucket(client, initialize_bucket(), objects_to_delete);
        res["status"] = "Objects deletion initiated";

        return res;
        
    });


    char* port_env = std::getenv("PORT");
    int port = (port_env != nullptr) ? std::stoi(port_env) : 18080;
    app.port(port).multithreaded().run();
}
