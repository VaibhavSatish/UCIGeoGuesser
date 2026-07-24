set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)
set(VCPKG_CMAKE_SYSTEM_NAME Linux)
 
# Only build the release configuration of every dependency. By default
# vcpkg builds BOTH debug and release variants of each port, which roughly
# doubles peak memory and compile time for heavy libraries like gRPC,
# protobuf, and Abseil (pulled in transitively via google-cloud-cpp). Since
# the CMakeLists only ever links the release config, the debug build was
# pure waste — this is the main lever for fixing an OOM during the Docker
# build on memory-constrained hosts (e.g. Render).
set(VCPKG_BUILD_TYPE release)