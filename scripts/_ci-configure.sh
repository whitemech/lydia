#!/bin/bash -e
set -e
# TOFIX exit script if a command fails

if [ $# -ne 1 ]; then
echo -e "\033[0;31m[!] Usage: ./scripts/build-cmake-dev.sh <SRC_ROOT> \033[0m"
exit 1
fi

SRC_ROOT=$(readlink -e $1)
BUILD_DIR=build
cd "${SRC_ROOT}" && mkdir -p ${BUILD_DIR} && cd ${BUILD_DIR} && rm -rf ./*

# Configure compiler build with coverage flas
cmake "${SRC_ROOT}" -DENABLE_COVERAGE=ON
