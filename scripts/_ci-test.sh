#!/bin/bash -e
set -e

cd build && make test
cd ..

echo "========== Code coverage =========="
gcovr -r . -e "third_party/*" --print-summary --html --html-details --xml -o coverage.html
