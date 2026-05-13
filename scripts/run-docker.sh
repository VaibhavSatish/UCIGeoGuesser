#!bin/bash
echo "Cleaning up existing Docker container..."
docker stop backend
docker rm backend
echo "Starting new container..."
docker run -p 18080:18080 -d --name backend -v .:/backend ucigeoguesser sleep infinity
docker exec -it backend bash