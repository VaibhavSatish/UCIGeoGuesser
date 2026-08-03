#include "crow.h"
#include "crow/middlewares/cors.h"
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <mutex>
#include <random>
#include <algorithm>
#include <chrono>
#include "upload_images.cpp"

// ──────────────────────────────────────────────
// Global state
// ──────────────────────────────────────────────

// Pre-loaded image index (populated once at startup)
static vector<ImageEntry> g_image_index;

// Active game sessions (protected by mutex for thread safety)
static std::unordered_map<string, GameSession> g_sessions;
static std::mutex g_sessions_mutex;

// Session timeout: 10 minutes
static constexpr auto SESSION_TIMEOUT = std::chrono::minutes(10);

// ──────────────────────────────────────────────
// Helpers
// ──────────────────────────────────────────────

// Remove sessions older than SESSION_TIMEOUT. Call under lock.
static void cleanup_expired_sessions() {
    auto now = std::chrono::steady_clock::now();
    for (auto it = g_sessions.begin(); it != g_sessions.end(); ) {
        if (now - it->second.created_at > SESSION_TIMEOUT) {
            cout << "Cleaning up expired session: " << it->first << endl;
            it = g_sessions.erase(it);
        } else {
            ++it;
        }
    }
}

// Pick `count` unique random indices from [0, pool_size)
static vector<int> pick_random_indices(int pool_size, int count) {
    // Build a list of all indices and shuffle
    vector<int> all(pool_size);
    std::iota(all.begin(), all.end(), 0);

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::shuffle(all.begin(), all.end(), gen);

    // Take the first `count` indices (or all if pool is smaller)
    int take = std::min(count, pool_size);
    return vector<int>(all.begin(), all.begin() + take);
}

int main() {
    using namespace std;

    /* Initialize the Crow app with CORS support */
    crow::App<crow::CORSHandler> app;
    auto& cors = app.get_middleware<crow::CORSHandler>().global();
    cors.origin("*")
        .methods("GET"_method, "POST"_method, "OPTIONS"_method)
        .headers("Content-Type", "ngrok-skip-browser-warning");

    /* Constants */
    const string IMAGE_DIRECTORY = "./res";
    const string METADATA_SUFFIX = "supplemental-metadata.json";
    const int MAX_ROUNDS = 25;

    /* ───── Startup: initialize GCS & load/upload image index ───── */
    auto client = initialize_gcs();
    const char* bucket_name = initialize_bucket();

    g_image_index = load_image_index(client, bucket_name, IMAGE_DIRECTORY, METADATA_SUFFIX);
    if (g_image_index.empty()) {
        cerr << "FATAL: No images found in " << IMAGE_DIRECTORY << endl;
        return 1;
    }

    cout << "Server starting with " << g_image_index.size() << " indexed & uploaded images." << endl;

    /* ───────────────────────────────────────
       GET /api/health_check
       Response: { "status" }
       ─────────────────────────────────────── */
    CROW_ROUTE(app, "/api/health_check").methods("GET"_method)
    ([]() {

        crow::json::wvalue res;
        res["status"] = "ok";
        return crow::response(200, res);
        
    });


    /* ───────────────────────────────────────
       POST /api/start_game
       Request:  { "totalRounds": 5 }
       Response: { "sessionId", "totalRounds", "round", "imageUrl" }
       ─────────────────────────────────────── */
    CROW_ROUTE(app, "/api/start_game").methods("POST"_method)
    ([MAX_ROUNDS](const crow::request& req) {
        crow::json::wvalue res;
        auto data = crow::json::load(req.body);

        if (!data) {
            res["error"] = "Malformed JSON payload";
            return crow::response(400, res);
        }

        if (!data.has("totalRounds") || data["totalRounds"].t() != crow::json::type::Number) {
            res["error"] = "Invalid totalRounds value";
            return crow::response(400, res);
        }

        int requested_rounds = data["totalRounds"].i();
        if (requested_rounds <= 0) requested_rounds = 1;
        if (requested_rounds > MAX_ROUNDS) requested_rounds = MAX_ROUNDS;
        // Don't request more rounds than we have images
        if (requested_rounds > static_cast<int>(g_image_index.size())) {
            requested_rounds = static_cast<int>(g_image_index.size());
        }

        // Pick random images
        auto indices = pick_random_indices(g_image_index.size(), requested_rounds);

        // Build session
        GameSession session;
        session.session_id = generate_session_id();
        session.current_round = 0;
        session.total_score = 0;
        session.created_at = std::chrono::steady_clock::now();

        for (int idx : indices) {
            const auto& img = g_image_index[idx];
            RoundData rd;
            rd.image_index = idx;
            rd.gcs_url = img.gcs_url;
            rd.answer_lat = img.latitude;
            rd.answer_lng = img.longitude;
            rd.submitted = false;
            rd.score = 0;
            session.rounds.push_back(rd);
        }


        string sid = session.session_id;

        // Store session
        {
            std::lock_guard<std::mutex> lock(g_sessions_mutex);
            cleanup_expired_sessions();
            g_sessions[sid] = std::move(session);
        }

        // Respond with first round (no answer coords!)
        const auto& first_round = g_sessions[sid].rounds[0];
        res["sessionId"] = sid;
        res["totalRounds"] = requested_rounds;
        res["round"] = 1;
        res["imageUrl"] = first_round.gcs_url;

        cout << "Game started: session=" << sid
             << " rounds=" << requested_rounds << endl;

        return crow::response(200, res);
    });

    /* ───────────────────────────────────────
       POST /api/get_round
       Request:  { "sessionId": "..." }
       Response: { "round", "totalRounds", "imageUrl" }
       ─────────────────────────────────────── */
    CROW_ROUTE(app, "/api/get_round").methods("POST"_method)
    ([](const crow::request& req) {
        crow::json::wvalue res;
        auto data = crow::json::load(req.body);

        if (!data || !data.has("sessionId")) {
            res["error"] = "Missing sessionId";
            return crow::response(400, res);
        }

        string sid = data["sessionId"].s();

        std::lock_guard<std::mutex> lock(g_sessions_mutex);
        auto it = g_sessions.find(sid);
        if (it == g_sessions.end()) {
            res["error"] = "Session not found or expired";
            return crow::response(404, res);
        }

        auto& session = it->second;
        int round_idx = session.current_round;

        if (round_idx >= static_cast<int>(session.rounds.size())) {
            res["error"] = "Game is already over";
            res["gameOver"] = true;
            res["totalScore"] = session.total_score;
            return crow::response(200, res);
        }

        const auto& round = session.rounds[round_idx];
        res["round"] = round_idx + 1;  // 1-indexed for the client
        res["totalRounds"] = static_cast<int>(session.rounds.size());
        res["imageUrl"] = round.gcs_url;

        return crow::response(200, res);
    });

    /* ───────────────────────────────────────
       POST /api/submit_guess
       Request:  { "sessionId": "...", "lat": N, "lng": N }
       Response: { "score", "answerLat", "answerLng",
                   "totalScore", "round", "gameOver" }
       ─────────────────────────────────────── */
    CROW_ROUTE(app, "/api/submit_guess").methods("POST"_method)
    ([](const crow::request& req) {
        crow::json::wvalue res;
        auto data = crow::json::load(req.body);

        if (!data || !data.has("sessionId") ||
            !data.has("lat") || !data.has("lng")) {
            res["error"] = "Missing sessionId, lat, or lng";
            return crow::response(400, res);
        }

        string sid = data["sessionId"].s();
        double user_lat = data["lat"].d();
        double user_lng = data["lng"].d();

        std::lock_guard<std::mutex> lock(g_sessions_mutex);
        auto it = g_sessions.find(sid);
        if (it == g_sessions.end()) {
            res["error"] = "Session not found or expired";
            return crow::response(404, res);
        }

        auto& session = it->second;
        int round_idx = session.current_round;

        if (round_idx >= static_cast<int>(session.rounds.size())) {
            res["error"] = "Game is already over";
            res["gameOver"] = true;
            res["totalScore"] = session.total_score;
            return crow::response(200, res);
        }

        auto& round = session.rounds[round_idx];
        if (round.submitted) {
            res["error"] = "This round was already submitted";
            return crow::response(400, res);
        }

        // Calculate score
        int score = calculate_score(user_lat, user_lng,
                                    round.answer_lat, round.answer_lng);
        round.score = score;
        round.submitted = true;
        session.total_score += score;
        session.current_round++;

        bool game_over = (session.current_round >=
                          static_cast<int>(session.rounds.size()));

        // NOW reveal the answer coordinates
        res["score"] = score;
        res["answerLat"] = round.answer_lat;
        res["answerLng"] = round.answer_lng;
        res["totalScore"] = session.total_score;
        res["round"] = round_idx + 1;
        res["gameOver"] = game_over;

        cout << "Guess submitted: session=" << sid
             << " round=" << (round_idx + 1)
             << " score=" << score
             << " total=" << session.total_score
             << " gameOver=" << game_over << endl;

        // Clean up completed session
        if (game_over) {
            // Keep it briefly so the client can read the final response,
            // but it will be cleaned up by timeout eventually.
        }

        return crow::response(200, res);
    });

    /* ───────────────────────────────────────
       POST /api/skip_round
       Request:  { "sessionId": "..." }
       Response: { "score": 0, "answerLat", "answerLng",
                   "totalScore", "round", "gameOver" }
       ─────────────────────────────────────── */
    CROW_ROUTE(app, "/api/skip_round").methods("POST"_method)
    ([](const crow::request& req) {
        crow::json::wvalue res;
        auto data = crow::json::load(req.body);

        if (!data || !data.has("sessionId")) {
            res["error"] = "Missing sessionId";
            return crow::response(400, res);
        }

        string sid = data["sessionId"].s();

        std::lock_guard<std::mutex> lock(g_sessions_mutex);
        auto it = g_sessions.find(sid);
        if (it == g_sessions.end()) {
            res["error"] = "Session not found or expired";
            return crow::response(404, res);
        }

        auto& session = it->second;
        int round_idx = session.current_round;

        if (round_idx >= static_cast<int>(session.rounds.size())) {
            res["error"] = "Game is already over";
            res["gameOver"] = true;
            res["totalScore"] = session.total_score;
            return crow::response(200, res);
        }

        auto& round = session.rounds[round_idx];
        if (round.submitted) {
            res["error"] = "This round was already submitted";
            return crow::response(400, res);
        }

        // Score is 0 for a skip
        round.score = 0;
        round.submitted = true;
        session.current_round++;

        bool game_over = (session.current_round >=
                          static_cast<int>(session.rounds.size()));

        res["score"] = 0;
        res["answerLat"] = round.answer_lat;
        res["answerLng"] = round.answer_lng;
        res["totalScore"] = session.total_score;
        res["round"] = round_idx + 1;
        res["gameOver"] = game_over;

        cout << "Round skipped: session=" << sid
             << " round=" << (round_idx + 1)
             << " gameOver=" << game_over << endl;

        return crow::response(200, res);
    });

    char* port_env = std::getenv("PORT");
    int port = (port_env != nullptr) ? std::stoi(port_env) : 18080;
    app.port(port).multithreaded().run();
}
