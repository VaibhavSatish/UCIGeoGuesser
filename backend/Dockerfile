# Stage 1: Build stage
# NOTE: previously pinned to --platform=$BUILDPLATFORM. That pattern only
# makes sense if you also set up a real cross-compilation toolchain (a
# separate C/C++ cross-compiler + cross vcpkg triplet). Since this project
# doesn't do that, keeping $BUILDPLATFORM here while the final stage follows
# whatever target platform was requested would silently link a binary built
# for the host arch into a base image of a different arch. Removed so the
# builder just compiles natively for whatever platform is requested
# (correct for both amd64 and arm64 builds, as long as you're not trying to
# cross-build one from the other).
FROM ubuntu:22.04 AS builder

ARG TARGETARCH

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
# VCPKG_OVERLAY_TRIPLETS + VCPKG_TARGET_TRIPLET point at a custom triplet
# that skips building the unused debug configuration of every dependency
# (see triplets/*.cmake), which roughly halves peak memory during the
# vcpkg install step. TARGETARCH ("amd64" or "arm64") picks the right one.
RUN VCPKG_TRIPLET=$( [ "$TARGETARCH" = "arm64" ] && echo "arm64-linux-release" || echo "x64-linux-release" ) && \
    cmake -B build -S . \
    -DCMAKE_TOOLCHAIN_FILE=/vcpkg/scripts/buildsystems/vcpkg.cmake \
    -DVCPKG_MANIFEST_MODE=ON \
    -DVCPKG_ROOT=/vcpkg \
    -DVCPKG_OVERLAY_TRIPLETS=/backend/triplets \
    -DVCPKG_TARGET_TRIPLET=${VCPKG_TRIPLET} \
    -DCMAKE_BUILD_TYPE=Release
    

# Compile the code. "-j2" caps parallel compile jobs — gRPC/protobuf
# translation units can each need 1-2GB+ of RAM, so letting CMake spawn one
# job per core is what actually triggers the OOM on memory-constrained
# build hosts. Lower to -j1 if this still runs out of memory.
RUN cmake --build build -- -j2

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