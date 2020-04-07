#!/usr/bin/env bash

FILES=$(find lib app \( -iname "*.hpp" -o -iname "*.cpp" \))
return_code=0

mkdir -p ./build && cd ./build && rm -rf ./*
cmake ..
cd ..
cp build/compile_commands.json ./

for file in $FILES; do
  (set -x; clang-tidy -warnings-as-errors="*" "$file" "$@")
  return_code=$return_code || "$?"
done

exit $return_code
