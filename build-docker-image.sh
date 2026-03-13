PROJECT_NAME=sd-socle-europe
DOCKER_TAG="latest"

DOCKER_BUILDKIT=1 docker build --no-cache -t $PROJECT_NAME:$DOCKER_TAG -f Dockerfile .
