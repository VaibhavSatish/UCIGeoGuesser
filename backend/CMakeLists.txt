cmake_minimum_required(VERSION 3.10)
project(Backend)

set(CMAKE_CXX_STANDARD 17)


# Find Asio (This often provides a proper target, so we keep it)
find_package(asio CONFIG REQUIRED)

# Handle Threads (Crow usually requires Pthreads on Linux)
find_package(Threads REQUIRED)

# Add server_connection.cpp as the main source file for the backend executable
add_executable(server_connection server_connection.cpp)

# Rename the binary to "backend" for clarity
set_target_properties(server_connection PROPERTIES OUTPUT_NAME "backend")

# Add the include directory for Crow
target_include_directories(server_connection PRIVATE ${CROW_INCLUDE_DIR})

# 5. Link only actual libraries (Asio and Threads)
# Remove 'crow::crow' from here!
target_link_libraries(server_connection PRIVATE 
    asio::asio 
    Threads::Threads
)