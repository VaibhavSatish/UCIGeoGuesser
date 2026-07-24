
set(VCPKG_TARGET_ARCHITECTURE arm64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)
set(VCPKG_CMAKE_SYSTEM_NAME Linux)
 
# Only build the release configuration of every dependency — see the
# comment in x64-linux-release.cmake for why. Same rationale here, just
# targeting arm64 (e.g. Render's ARM build hosts, Apple Silicon, AWS
# Graviton).
set(VCPKG_BUILD_TYPE release)