echo "Building Docker image..."
cd "$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")" || exit 1
cd ../backend 
sudo docker build -t ucigeoguesser .
echo "Building Docker container..."
sudo docker stop backend
sudo docker rm backend
sudo docker run -d \
  --name backend \
  --restart unless-stopped \
  -p 18080:18080 \
  -v ~/ucigeoguesser/backend/key-file.json:/backend/key-file.json:ro \
  -e GOOGLE_APPLICATION_CREDENTIALS="/backend/key-file.json" \
  --env-file ~/ucigeoguesser/backend/.env \
  ucigeoguesser
echo "Successfully deployed backend Docker container."