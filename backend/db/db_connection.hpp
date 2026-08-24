#ifndef DB_CONNECTION_HPP
#define DB_CONNECTION_HPP



#include <pqxx/pqxx>


pqxx::connection connect_to_db();

bool add_image_entry(pqxx::connection& conn, const std::string& gcs_url, double latitude, double longitude);


#endif // DB_CONNECTION_HPP