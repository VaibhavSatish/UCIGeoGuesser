#!/bin/bash
echo "Building Docker image..."
cd "$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")" || exit 1
cd ../backend 
docker build -t ucigeoguesser --target builder .
echo "Building Docker container..."
docker stop backend
docker rm backend
docker run -p 18080:18080 -d --name backend -v .:/backend ucigeoguesser sleep infinity
echo "Accessing Docker container..."
docker exec -it backend bash

