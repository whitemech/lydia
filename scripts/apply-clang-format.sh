#!/usr/bin/env bash

find app lib -iname "*.hpp" -o -iname "*.cpp" | xargs clang-format -i
