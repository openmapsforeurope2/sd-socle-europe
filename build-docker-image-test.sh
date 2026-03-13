PROJECT_NAME=sd-socle-europe-test
DOCKER_TAG="latest"

git clone --branch mborne --recurse-submodules http://gitlab.forge-idi.ign.fr/socle/sd-socle

# DOCKER_BUILDKIT=1 docker build --no-cache -t $PROJECT_NAME:$DOCKER_TAG -f Dockerfile.test .
DOCKER_BUILDKIT=1 docker build -t $PROJECT_NAME:$DOCKER_TAG -f Dockerfile.test .
