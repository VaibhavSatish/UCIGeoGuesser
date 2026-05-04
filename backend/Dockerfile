# Stage 1: Build stage
FROM ubuntu:latest AS build

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

# Install asio and crow packages
RUN /vcpkg/vcpkg install asio crow

# Set the working directory
WORKDIR /backend

# Copy the source code into the container
COPY . .

# Builds the Build Directory and Connects the CMakeLists.txt to the vcpkg toolchain file to ensure that the dependencies are correctly linked during the build process.
RUN cmake -B build -S . \
    -DCMAKE_TOOLCHAIN_FILE=/vcpkg/scripts/buildsystems/vcpkg.cmake \
    -DVCPKG_TARGET_TRIPLET=x64-linux \
    -DCMAKE_BUILD_TYPE=Release

# Compile the Code
RUN cmake --build build

# Exposing the port that the server will run on
EXPOSE 18080




# Default to bash to run commands
CMD ["./build/backend", "bash"]