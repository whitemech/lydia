#!/bin/bash -e
set -e
# TOFIX exit script if a command fails

if [ $# -ne 1 ]; then
echo -e "\033[0;31m[!] Usage: ./scripts/build-cmake-dev.sh <SRC_ROOT> \033[0m"
exit 1
fi

SRC_ROOT="$1"
BUILD_DIR=build
cd "${SRC_ROOT}" && mkdir -p ${BUILD_DIR} && cd ${BUILD_DIR} && rm -rf ./*

# Configure compiler build with coverage flas
cmake .. -DENABLE_COVERAGE=ON
# compiler will generate *.gcno files for each compiled object
# running tests will generate *.gcda for each compiled object
#  excuted partially or entirely by tests
make -j
make test

# generate coverage information using lcov (that uses gcov)
gcovr -r . -e "third_party/*" --print-summary --html --html-details --xml -o coverage.html