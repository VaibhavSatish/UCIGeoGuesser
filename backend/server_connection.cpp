#include "crow.h"
#include "crow/middlewares/cors.h"
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>

int main() {
    using namespace std;

    crow::App<crow::CORSHandler> app;
    auto& cors = app.get_middleware<crow::CORSHandler>().global();
    cors.origin("*").methods("GET"_method);

    CROW_ROUTE(app, "/images").methods("GET"_method) ([](){        
        
        crow::json::wvalue res;
        
        int id = 1;

        for (const auto& file : filesystem::directory_iterator("./res")) {

            if(file.path().extension() != ".jpg" && file.path().extension() != ".jpeg"){
                continue;
            }

            ifstream in(file.path(), ios::binary);
            stringstream ss;
            ss << in.rdbuf();

            res["images"][to_string(id++)]["image"] =
                "data:image/jpeg;base64," +
                crow::utility::base64encode(ss.str(), ss.str().size());

            ifstream into(file.path().string() + ".supplemental-metadata.json");
            stringstream sst;
            sst << into.rdbuf();

            res["images"][to_string(id)]["metadata"] = crow::json::load(sst.str());

            if (id == 6) {
                break;
            }
        }
        return res;
    }); 


    char* port_env = std::getenv("PORT");
    int port = (port_env != nullptr) ? std::stoi(port_env) : 18080;
    app.port(port).multithreaded().run();
}
