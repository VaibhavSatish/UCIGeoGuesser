#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BACKEND_DIR="$SCRIPT_DIR/../backend"
cd "$BACKEND_DIR" || exit 1

GCS_KEY_FILE="${GCS_KEY_FILE:-$HOME/Downloads/key-file.json}"
if [ ! -f "$GCS_KEY_FILE" ]; then
  echo "ERROR: GCS key not found at $GCS_KEY_FILE" >&2
  echo "Put the JSON in ~/Downloads/key-file.json, or set GCS_KEY_FILE to its path." >&2
  exit 1
fi

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
  -v "$GCS_KEY_FILE:/secrets/gcs-key.json:ro" \
  -e GOOGLE_APPLICATION_CREDENTIALS="/secrets/gcs-key.json" \
  --env-file "$BACKEND_DIR/.env" \
  ucigeoguesser
echo "Successfully deployed backend Docker container."