#!/usr/bin/env bash

# Usage:
# ./scripts/build.sh BUILD_TYPE -j NUM_JOBS
# Where BUILD_TYPE is one of: Debug, Release, ReleaseWithDebInfo

set -e

BUILD_TYPE=${1:-Debug}
NUM_JOBS=${2:-}
mkdir -p ./build && cd ./build && rm -rf ./*
cmake .. -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
cmake --build . --config $BUILD_TYPE -j $NUM_JOBS

cd ..