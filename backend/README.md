# To run the backend, download Docker Desktop
`https://www.docker.com/products/docker-desktop/`

After downloading docker desktop create a Docker image using the command below:

`docker build -t ucigeoguesser .`

Run the docker image to create a container

`docker run -p 18080:18080 -d --name backend -v LOCAL_PATH_TO_BACKEND:/backend ucigeoguesser sleep infinity`

Execute the container 

`docker exec -it backend bash `

# To Run CMake in Docker

Initialize Build Directory:

`cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=/vcpkg/scripts/buildsystems/vcpkg.cmake`

Compile the build

`cmake --build build `