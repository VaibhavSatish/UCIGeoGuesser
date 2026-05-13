#!bin/bash
echo "Cleaning up existing Docker container..."
docker stop backend
docker rm backend
echo "Deleting existing Docker image..."
docker rmi ucigeoguesser
echo "Docker cleanup complete."