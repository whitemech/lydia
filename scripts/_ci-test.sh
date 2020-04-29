#!/usr/bin/env bash

set -e

cd build && make test
cd ..

echo "========== Code coverage =========="
gcovr -r . -e "third_party/*" -e "build/lib/CMakeFiles/lydia.dir/parser/parser.yy" \
           -e "build/lib/CMakeFiles/lydia.dir/parser/lexer.l" -e "lib/test/*" -e "lib/benchmark/*" \
           --print-summary --html --html-details -o coverage.html
gcovr -r . -e "third_party/*" -e "build/lib/CMakeFiles/lydia.dir/parser/parser.yy" \
           -e "build/lib/CMakeFiles/lydia.dir/parser/lexer.l" -e "lib/test/*" -e "lib/benchmark/*" \
           --xml -o coverage.xml