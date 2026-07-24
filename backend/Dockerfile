
 
 
# Stage 1: Build stage
FROM --platform=$BUILDPLATFORM ubuntu:22.04 AS builder
 
ENV VCPKG_ROOT=/vcpkg
ENV PATH="${VCPKG_ROOT}:${PATH}"
 
# Install essential packages for compiling C++ code
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    curl \
    zip \
    unzip \
    tar \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*
 
 
 
# Install vcpkg package manager (for crow and asio)
RUN git clone https://github.com/microsoft/vcpkg /vcpkg
RUN vcpkg/bootstrap-vcpkg.sh
 
# NOTE: the google-cloud-sdk / gcloud CLI install was removed here. The app
# talks to Cloud Storage via the google-cloud-cpp *library* (linked in via
# vcpkg + CMake), so the CLI tool was never actually used at build or run
# time — it was just adding ~1-2GB of dead weight to the builder stage.
 
# Set the working directory
WORKDIR /backend
 
# Copy the vcpkg manifest file to the container
COPY vcpkg.json .
 
 
# Copy the source code into the container
COPY . .
 
 
# Builds the Build Directory and Connects the CMakeLists.txt to the vcpkg toolchain file to ensure that the dependencies are correctly linked during the build process.
RUN cmake -B build -S . \
    -DCMAKE_TOOLCHAIN_FILE=/vcpkg/scripts/buildsystems/vcpkg.cmake \
    -DVCPKG_MANIFEST_MODE=ON \
    -DVCPKG_ROOT=/vcpkg \
    -DCMAKE_BUILD_TYPE=Release
    
 
# Compile the code
RUN cmake --build build
 
# Exposing the port that the server will run on
EXPOSE 18080
 
# STAGE 2: Creates tiny image with only the compiled binary
FROM ubuntu:22.04
 
WORKDIR /backend
# Copy from /backend/build (matching Stage 1 WORKDIR).
# NOTE: there is no separate "upload_images" binary — upload_images.cpp is
# #included directly into server_connection.cpp and compiled into the single
# "backend" executable, so the old second COPY line here referenced a file
# that never existed and would have failed the build.
COPY --from=builder /backend/build/backend .
 
EXPOSE 18080
 
# Run the app (binary lands flat in /backend, not /backend/build)
CMD ["./backend"]


