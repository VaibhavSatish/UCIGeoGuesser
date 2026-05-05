

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



# Set the working directory
WORKDIR /backend



# Copy the source code into the container
COPY . .


# Create the manifest file automatically inside the container
RUN vcpkg new --application --name backend || true && \
    vcpkg add port crow asio 


# Builds the Build Directory and Connects the CMakeLists.txt to the vcpkg toolchain file to ensure that the dependencies are correctly linked during the build process.
RUN cmake -B build -S . \
    -DCMAKE_TOOLCHAIN_FILE=/vcpkg/scripts/buildsystems/vcpkg.cmake \
    -DVCPKG_MANIFEST_MODE=ON \
    -DVCPKG_ROOT=/vcpkg \
    -DCMAKE_BUILD_TYPE=Release
    

# Compile the Code
RUN cmake --build build

# Exposing the port that the server will run on
EXPOSE 18080

# STAGE 2: Run (The tiny image that goes to Cloud Run)
FROM ubuntu:22.04

WORKDIR /backend
# Copy from /backend/build (matching Stage 1 WORKDIR)
COPY --from=builder /backend/build/backend .
COPY --from=builder /backend/build/upload_images .
COPY .env . 

EXPOSE 18080

# Run the app
CMD ["./backend"]



