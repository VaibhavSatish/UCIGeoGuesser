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
    cors.origin("*").methods("POST"_method);

    CROW_ROUTE(app, "/images").methods("POST"_method) ([](const crow::request& req){        
        
        crow::json::wvalue res;
        auto data = crow::json::load(req.body); 
        if (!data.has("totalRounds") || data["totalRounds"].t() != crow::json::type::Number)  {
            res["error"] = "Invalid totalRounds value";
            return res;
        }
        
        int rounds  = data["totalRounds"].i();
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
            --rounds;

            if (id == 6 || rounds == 0) {
                break;
            }
        }
        return res;
    }); 


    char* port_env = std::getenv("PORT");
    int port = (port_env != nullptr) ? std::stoi(port_env) : 18080;
    app.port(port).multithreaded().run();
}
