cmake_minimum_required(VERSION 3.10)
set(VCPKG_MANIFEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(VCPKG_MANIFEST_MODE ON)
project(backend)

set(CMAKE_CXX_STANDARD 17)

# Include dotenv-cpp using FetchContent
include(FetchContent)
FetchContent_Declare(
    laserpants_dotenv
    GIT_REPOSITORY https://github.com/laserpants/dotenv-cpp
    GIT_TAG master
)
FetchContent_MakeAvailable(laserpants_dotenv)


# Find Asio (This often provides a proper target, so we keep it)
find_package(asio CONFIG REQUIRED)



# Handle Threads (Crow usually requires Pthreads on Linux)
find_package(Threads REQUIRED)

find_package(google_cloud_cpp_storage REQUIRED)

find_package(nlohmann_json CONFIG REQUIRED)

add_executable(server_connection server_connection.cpp)

# Rename the binary to "backend" for clarity
set_target_properties(server_connection PROPERTIES OUTPUT_NAME "backend")

# Find Crow
target_include_directories(server_connection PRIVATE 
    ${CROW_INCLUDE_DIR}
    ${laserpants_dotenv_SOURCE_DIR}/include
)

# 5. Link only actual libraries (Asio and Threads)
target_link_libraries(server_connection PRIVATE 
    asio::asio 
    Threads::Threads
    google-cloud-cpp::storage
    nlohmann_json::nlohmann_json
)




