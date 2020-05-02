#!/usr/bin/env bash

find app lib \( -iname "*.cc"\
    -o -iname "*hh"\
    -o -iname "*.c"\
    -o -iname "*.h"\
    -o -iname "*.hpp"\
    -o -iname "*.cpp" \) -exec clang-format -i {} \;
