#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR/../backend"
echo "Stopping and removing existing backend Docker container..."
sudo docker stop backend
sudo docker rm backend
sudo docker rmi ucigeoguesser
echo "Building Docker image..."
sudo docker build -t ucigeoguesser . || exit 1
echo "Building Docker container..."
sudo docker run -d \
  --name backend \
  --restart unless-stopped \
  -p 18080:18080 \
  -v ~/ucigeoguesser/backend/key-file.json:/backend/key-file.json:ro \
  -e GOOGLE_APPLICATION_CREDENTIALS="/backend/key-file.json" \
  --env-file ~/ucigeoguesser/backend/.env \
  ucigeoguesser
echo "Successfully deployed backend Docker container."