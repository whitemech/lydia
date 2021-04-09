#!/usr/bin/env bash

set -e

cd build/lib/test && ../../bin/lydiaTest "~[integration] ~[advanced_theorems]"

cd ../../../

echo "========== Code coverage =========="
python3 -m gcovr -r . --config "gcovr.cfg" --print-summary --html --html-details -o coverage.html
python3 -m gcovr -r . --config "gcovr.cfg" --xml -o coverage.xml