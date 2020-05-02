#!/usr/bin/env bash

set -e

cd build && make test
cd ..

echo "========== Code coverage =========="
python -m gcovr -r . --config "gcovr.cfg" --print-summary --html --html-details -o coverage.html
python -m gcovr -r . --config "gcovr.cfg" --xml -o coverage.xml