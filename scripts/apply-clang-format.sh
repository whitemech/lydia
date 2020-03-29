#!/usr/bin/env bash

find app lib \( -iname "*.hpp" -o -iname "*.cpp" \) -exec clang-format -i {} \;
