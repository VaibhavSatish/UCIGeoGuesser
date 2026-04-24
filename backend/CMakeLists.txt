cmake_minimum_required(VERSION 3.10)
project(Backend)

set(CMAKE_CXX_STANDARD 17)

# Find the packages
find_path(CROW_INCLUDE_DIR NAMES crow.h)
if(NOT CROW_INCLUDE_DIR)
    message(FATAL_ERROR "Crow header not found!")
endif() # Since Crow is header-only, we only need to find the include directory

# 2. Find Asio (This often provides a proper target, so we keep it)
find_package(asio CONFIG REQUIRED)

# 3. Handle Threads (Crow usually requires Pthreads on Linux)
find_package(Threads REQUIRED)

add_executable(server_connection server_connection.cpp)

# 4. Add the include directory for Crow
target_include_directories(server_connection PRIVATE ${CROW_INCLUDE_DIR})

# 5. Link only actual libraries (Asio and Threads)
# Remove 'crow::crow' from here!
target_link_libraries(server_connection PRIVATE 
    asio::asio 
    Threads::Threads
)