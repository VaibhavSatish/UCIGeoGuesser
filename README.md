# UCI GeoGuesser

A GeoGuessr-style guessing game set entirely on the University of California, Irvine campus. Players are shown a photo taken somewhere at UCI and have to drop a pin on the map as close as possible to where it was taken — the closer the guess, the higher the score.

🔗 **Live app:** [ucigeoguesser.vercel.app](https://ucigeoguesser.vercel.app/)

## Development
#### Getting Started
To get started, please read the frontend, backend, and db `README.md` and setup the environment accordingly.

#### Contributions

UCIGeoguesser is open for anyone to contribute, however they need to follow the following rules:
```markdown
1. Do not push anything to main (Always create Pull Requests!)
2. If you encounter any bugs, please create an issue (do not spam the owners!)
3. Follow the standard PR template (Listed below)
```

Your pull request should follow this template:

```markdown
## Description
<!-- Provide a brief summary of the changes introduced by this PR. Include motivation and context. -->

## Associated Issue
<!-- Link the issue this PR resolves using keywords (e.g., Closes #123, Fixes #456). -->
Closes #

## Type of Change
<!-- Please check the options that apply to this PR. -->
- [ ] `feat`: A new feature
- [ ] `fix`: A bug fix
- [ ] `docs`: Documentation changes
- [ ] `refactor`: Code change that neither fixes a bug nor adds a feature
- [ ] `perf`: A code change that improves performance
- [ ] `test`: Adding missing tests or correcting existing tests
- [ ] `chore`: Changes to the build process or auxiliary tools/libraries
- [ ] `infra`: Changes to the infrastructure (i.e. docker compose)

## How Has This Been Tested?
<!-- Describe the tests that you ran to verify your changes. Provide instructions so we can reproduce. -->
- [ ] **Unit Tests:** `npm test` / `google test` (Passes)
- [ ] **Manual Testing:** Describe steps taken to verify.

## Checklist
- [ ] My code follows the style guidelines of this project.
- [ ] I have performed a self-review of my own code.
- [ ] I have commented my code, particularly in hard-to-understand areas.
- [ ] I have made corresponding changes to the documentation.
- [ ] My changes generate no new warnings or console errors.

## Screenshots / Screen Recordings (if applicable)
<!-- Add visual proof for UI changes to help reviewers. -->
```


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
.
├── backend
│   ├── api_documentation.md
│   ├── CMakeLists.txt
│   ├── db
│   ├── Dockerfile
│   ├── README.md
│   ├── server_connection.cpp
│   ├── triplets
│   ├── upload_images.cpp
│   ├── upload_images.hpp
│   ├── vcpkg-configuration.json
│   └── vcpkg.json
├── compose.yaml
├── frontend
│   ├── app
│   │   ├── challenge
|   |   |     └── page.tsx
│   │   ├── ChallengeResults.tsx
│   │   ├── favicon.ico
│   │   ├── game
|   |   |     └── page.tsx
│   │   ├── GameOver.tsx
│   │   ├── GameTimer.jsx
│   │   ├── globals.css
│   │   ├── guess.tsx
│   │   ├── layout.tsx
│   │   ├── lib
│   │   ├── page.tsx
│   │   ├── results.tsx
│   │   └── TitleScreen.tsx
│   ├── package-lock.json
│   ├── package.json
│   ├── public
│   ├── README.md
│   └── tsconfig.json
├── README.md
└── scripts
    ├── build-docker.sh
    ├── clean-docker.sh
    ├── deploy-backend.sh
    └── run-docker.sh
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

### Backend and Database

The backend requires Google Cloud Storage credentials to run:

The database we are using is postgres (look at `$REPO_ROOT/backend/db/schema.sql` for schema).

Both services are bundled together through Docker Compose.

To run the services:
```bash
docker compose up --build 
```
Please see `$REPO_ROOT/backend/README.md` and `$REPO_ROOT/backend/db/README.md` for more information.

> Tip: the [Dev Containers](https://code.visualstudio.com/docs/devcontainers/containers) VS Code extension makes it easy to attach to and work inside the running backend container.

Helper scripts for building, running, cleaning, and deploying the backend live in [`scripts/`](scripts/).

## Deployment

- **Frontend:** deployed on Vercel.
- **Backend:** built into a Docker image and deployed to an Oracle Cloud VM. Pushes to `main` trigger [`deploy.yaml`](.github/workflows/deploy.yaml), which SSHes into the VM and runs `scripts/deploy-backend.sh`.

## Credits

Thanks to [OpenStreetMap](https://www.openstreetmap.org) for the map API that made this project possible.
