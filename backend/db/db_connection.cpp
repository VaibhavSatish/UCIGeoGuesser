#include <iostream>
#include <pqxx/pqxx>
#include <string>

bool connect_to_database(std::string dbname, std::string user, std::string password, std::string hostaddr, pqxx::connection& conn) {
    try {
        conn = pqxx::connection("dbname=" + dbname + " user=" + user + " password=" + password + " hostaddr=" + hostaddr);
        return true;
    } catch (const std::exception &e) {
        std::cerr << "Error connecting to database: " << e.what() << std::endl;
        return false;
    }
}

bool add_image_entry(pqxx::connection& conn, const std::string& gcs_url, double latitude, double longitude) {
    try {
        pqxx::work txn(conn);
        txn.exec_params("INSERT INTO images (gcs_url, latitude, longitude) VALUES ($1, $2, $3)",
                        gcs_url, latitude, longitude);
        txn.commit();
        return true;
    } catch (const std::exception &e) {
        std::cerr << "Error adding image entry: " << e.what() << std::endl;
        return false;
    }
}