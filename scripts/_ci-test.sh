#!/usr/bin/env bash

set -e

cd build && make test
cd ..

echo "========== Code coverage =========="
gcovr -r . -e "third_party/*" --print-summary --html --html-details -o coverage.html
gcovr -r . -e "third_party/*" --xml -o coverage.xml

