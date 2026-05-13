#!/bin/bash
echo "Cleaning up existing Docker container..."
cd "$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")" || exit 1
cd ../backend 
docker stop backend
docker rm backend
echo "Deleting existing Docker image..."
docker rmi ucigeoguesser
echo "Docker cleanup complete."