#!/usr/bin/env bash

set -e

mkdir -p ./build && cd ./build && rm -rf ./*
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release -j "$@"

cd ..