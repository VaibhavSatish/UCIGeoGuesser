# To Run the Backend

To run the backend, you will need to have docker installed: 

`Docker Desktop: https://www.docker.com/products/docker-desktop/`

Now that you have docker installed, let's create a .env file with the following properties:
```python
COMPOSE_FILE = "../compose.yaml"
BUCKET_NAME = "your-bucket-name"
```

Now that you created the .env file, you can run the backend and db services through docker compose:
```bash
docker compose up --build # -d flag if you want to detach
# use `docker compose down` when you are done running
```


