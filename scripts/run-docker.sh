#!/bin/bash
echo "Cleaning up existing Docker container..."
cd "$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")" || exit 1
cd ../backend 
docker stop backend
docker rm backend
echo "Starting new container..."
docker run -p 18080:18080 -d --name backend -v .:/backend ucigeoguesser sleep infinity
docker exec -it backend bash