#!/bin/bash -e
set -e

cd build && make test
cd ..
gcovr -r . -e "third_party/*" --print-summary --html --html-details --xml -o coverage.html
