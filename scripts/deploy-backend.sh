#!/bin/bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BACKEND_DIR="$SCRIPT_DIR/../backend"
cd "$BACKEND_DIR" || exit 1


echo "==> Building updated Docker images..."
docker compose build

echo "==> Deploying containers..."
docker compose up -d --remove-orphans

echo "==> Cleaning up unused build cache and dangling images..."
docker image prune -f

echo "==> Deployment complete!"
