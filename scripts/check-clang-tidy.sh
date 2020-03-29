#!/usr/bin/env bash

FILES=$(find lib app \( -iname "*.hpp" -o -iname "*.cpp" \))
return_code=0

for file in $FILES; do
  (set -x; clang-tidy -warnings-as-errors="*" "$file" "$@")
  return_code=$return_code || "$?"
done

exit $return_code
