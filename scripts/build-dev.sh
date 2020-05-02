#!/usr/bin/env bash

# exit script if a command fails
set -e

BUILD_DIR=build
mkdir -p ${BUILD_DIR} && cd ${BUILD_DIR} && rm -rf ./*

# Configure compiler build with coverage flas
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON
# compiler will generate *.gcno files for each compiled object
# running tests will generate *.gcda for each compiled object
#  excuted partially or entirely by tests
make -j
make test

# generate coverage information using lcov (that uses gcov)
cd ..
echo "========== Code coverage =========="
gcovr -r . --config "gcovr.cfg" --print-summary --html --html-details -o coverage.html
gcovr -r . --config "gcovr.cfg" --xml -o coverage.xml
