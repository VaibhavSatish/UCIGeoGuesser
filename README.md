# UCI GeoGuesser

A GeoGuessr-style guessing game set entirely on the University of California, Irvine campus. Players are shown a photo taken somewhere at UCI and have to drop a pin on the map as close as possible to where it was taken — the closer the guess, the higher the score.

🔗 **Live app:** [ucigeoguesser.vercel.app](https://ucigeoguesser.vercel.app/)

## How it works

1. Start a game and choose how many rounds to play.
2. Each round shows a photo taken somewhere on campus.
3. Drop a pin on the map where you think the photo was taken.
4. Submit your guess (or skip the round) to see the real location and your score for that round.
5. After the final round, see your total score and how you did.

## Tech stack

**Frontend**
- [Next.js](https://nextjs.org) (React, TypeScript)
- Tailwind CSS
- Interactive map for placing guesses ([OpenStreetMap](https://www.openstreetmap.org))
- Deployed on [Vercel](https://vercel.com)

**Backend**
- C++17 with the [Crow](https://crowcpp.org) web framework
- [libpqxx](https://github.com/jtv/libpqxx) for PostgreSQL
- Google Cloud Storage (via direct REST calls with libcurl) for hosting game images
- Dependency management via [vcpkg](https://vcpkg.io)
- Containerized with Docker and deployed to an Oracle Cloud VM (see `.github/workflows/deploy.yaml`)

## Project structure

```
UCIGeoGuesser/
├── backend/          # C++ (Crow) API server, Dockerfile, CMake build
│   ├── server_connection.cpp
│   ├── upload_images.cpp / .hpp
│   ├── api_documentation.md
│   └── CMakeLists.txt
├── frontend/         # Next.js game client
│   └── app/
│       ├── page.tsx          # Landing page
│       ├── TitleScreen.tsx   # Title/start screen
│       ├── game/page.tsx     # Core game loop
│       ├── guess.tsx         # Guess map component
│       ├── results.tsx       # Per-round results
│       ├── GameOver.tsx      # End-of-game summary
│       └── GameTimer.jsx     # Round timer
└── scripts/          # Docker build/run and deployment helper scripts
```

## Backend API

The backend exposes a small JSON API for running a game session:

| Method | Endpoint | Description |
| :--- | :--- | :--- |
| `GET` | `/api/health_check` | Health check for the API server |
| `POST` | `/api/start_game` | Starts a new game session and returns the first round |
| `POST` | `/api/get_round` | Gets the current round for an active session |
| `POST` | `/api/submit_guess` | Submits a lat/lng guess, returns score and the real location |
| `POST` | `/api/skip_round` | Skips the current round for 0 points |

See [`backend/api_documentation.md`](backend/api_documentation.md) for full request/response schemas.

## Getting started

### Frontend

```bash
cd frontend
npm install
npm run dev
```

Then open [http://localhost:3000](http://localhost:3000).

### Backend

The backend is built and run via Docker:

```bash
cd backend

# Build the image
docker build -t ucigeoguesser --target builder .

# Run the container
docker run -p 18080:18080 -d --name backend -v .:/backend ucigeoguesser sleep infinity

# Enter the container / run the server
docker exec -it backend bash
# or
docker exec -it backend ./build/backend
```

You'll also need to authenticate with Google Cloud (used for image storage):

```bash
gcloud auth application-default login
```

If you're not using Docker, you can build directly with CMake + vcpkg:

```bash
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build
```

> Tip: the [Dev Containers](https://code.visualstudio.com/docs/devcontainers/containers) VS Code extension makes it easy to attach to and work inside the running backend container.

Helper scripts for building, running, cleaning, and deploying the backend live in [`scripts/`](scripts/).

## Deployment

- **Frontend:** deployed on Vercel.
- **Backend:** built into a Docker image and deployed to an Oracle Cloud VM. Pushes to `main` trigger [`deploy.yaml`](.github/workflows/deploy.yaml), which SSHes into the VM and runs `scripts/deploy-backend.sh`.

## Credits

Thanks to [OpenStreetMap](https://www.openstreetmap.org) for the map API that made this project possible.
