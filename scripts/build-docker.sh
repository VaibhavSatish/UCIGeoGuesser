#!/bin/bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BACKEND_DIR="$SCRIPT_DIR/../backend"
cd "$BACKEND_DIR"

GCS_KEY_FILE="${GCS_KEY_FILE:-$HOME/Downloads/key-file.json}"
if [ ! -f "$GCS_KEY_FILE" ]; then
  echo "ERROR: GCS key not found at $GCS_KEY_FILE" >&2
  echo "Put the JSON in ~/Downloads/key-file.json, or set GCS_KEY_FILE to its path." >&2
  exit 1
fi

echo "Building Docker image..."
docker build -t ucigeoguesser --target builder .

echo "Building Docker container..."
docker stop backend >/dev/null 2>&1 || true
docker rm backend >/dev/null 2>&1 || true

docker run -p 18080:18080 -d --name backend \
  --env-file .env \
  -e GOOGLE_APPLICATION_CREDENTIALS=/secrets/gcs-key.json \
  -v "$BACKEND_DIR":/backend \
  -v "$GCS_KEY_FILE":/secrets/gcs-key.json:ro \
  ucigeoguesser sleep infinity

echo "Accessing Docker container..."
docker exec -it backend bash
