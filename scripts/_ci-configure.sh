#!/usr/bin/env bash

# exit script if a command fails
set -e

BUILD_DIR=build
BUILD_TYPE=${1:-Debug}
mkdir -p ${BUILD_DIR} && cd ${BUILD_DIR} && rm -rf ./*

# Configure compiler build with coverage flags
cmake .. -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" -DENABLE_COVERAGE=ON
