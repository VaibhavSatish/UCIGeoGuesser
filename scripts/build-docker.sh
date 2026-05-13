#!/bin/bash
echo "Building Docker image..."
docker build -t ucigeoguesser --target builder .
echo "Building Docker container..."
docker run -p 18080:18080 -d --name backend -v .:/backend ucigeoguesser sleep infinity
echo "Accessing Docker container..."
docker exec -it backend bash