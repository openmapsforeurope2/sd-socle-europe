PROJECT_NAME=sd-socle-europe
DOCKER_TAG="latest"

BUILD_MODE=${1:-full}

DOCKER_BUILDKIT=1 docker build \
    --no-cache \
    --build-arg BUILD_MODE=$BUILD_MODE \
    -t $PROJECT_NAME:$DOCKER_TAG \
    -f Dockerfile .
