#!/usr/bin/env bash

set -e

mkdir -p ./build && cd ./build && rm -rf ./*
cmake ..
make -j "$@"
