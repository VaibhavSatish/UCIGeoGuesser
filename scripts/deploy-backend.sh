#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BACKEND_DIR="$SCRIPT_DIR/../backend"
cd "$BACKEND_DIR" || exit 1
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
  -v "$BACKEND_DIR/key-file.json:/backend/key-file.json:ro" \
  -e GOOGLE_APPLICATION_CREDENTIALS="/backend/key-file.json" \
  --env-file "$BACKEND_DIR/.env" \
  ucigeoguesser
echo "Successfully deployed backend Docker container."