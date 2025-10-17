#!/bin/bash

IMAGE=$(docker build -q .)
docker run --rm              \
    -v ./:/workdir           \
    --user $(id -u):$(id -g) \
    $IMAGE                   \
    make "$@"

sed -i '' "s|/workdir/|$(pwd)/|g" build/compile_commands.json