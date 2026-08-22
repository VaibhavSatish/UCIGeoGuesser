cmake_minimum_required(VERSION 3.10)
set(VCPKG_MANIFEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(VCPKG_MANIFEST_MODE ON)
project(backend)

set(CMAKE_CXX_STANDARD 17)

# Strip debug symbols from Release binaries. Because this project statically
# links vcpkg's static triplet (gRPC + protobuf + Abseil + OpenSSL via
# google-cloud-cpp), unstripped binaries can balloon into multiple GB each.
# "-s" tells the linker to omit the symbol table, shrinking the executables
# dramatically without touching how they run.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s")
endif()



# Find Asio (This often provides a proper target, so we keep it)
find_package(asio CONFIG REQUIRED)

# Find libpqxx (Required for PostgreSQL database connection support)
find_package(libpqxx CONFIG REQUIRED)

# Handle Threads (Crow usually requires Pthreads on Linux)
find_package(Threads REQUIRED)

# Used for direct GCS REST API calls (see upload_images.cpp) instead of the
# google-cloud-cpp SDK, which pulled in gRPC + protobuf + Abseil and was the
# main driver of build memory/time and final binary size.
find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)

find_package(nlohmann_json CONFIG REQUIRED)

add_executable(server_connection server_connection.cpp)

# Rename the binary to "backend" for clarity
set_target_properties(server_connection PROPERTIES OUTPUT_NAME "backend")

# Find Crow
target_include_directories(server_connection PRIVATE 
    ${CROW_INCLUDE_DIR}
)

# Link actual libraries
target_link_libraries(server_connection PRIVATE 
    asio::asio 
    Threads::Threads
    CURL::libcurl
    OpenSSL::SSL
    OpenSSL::Crypto
    nlohmann_json::nlohmann_json
    libpqxx::pqxx
)