#!/usr/bin/bash -e

##########################################
# Preliminaries
##########################################
# To make this script work, you need 'mona', 'ltlf2fol' in the environment PATH.
# Moreover, you need 'mona2sym' already built.
# To install 'ltlf2fol':
# - download and build the Syft project: https://github.com/saffiepig/Syft
# - put 'ltlf2fol' in a directory in the PATH variable.
# To install mona:
# - Download the project: https://www.brics.dk/mona/download.html
# - inside the project folder: "./configure && make && sudo make install-strip"
# To build 'mona2sym', please follow the instruction in the README.
##########################################
# Usage
##########################################
# - save a file 'formula' with an LDLf formula:
#
#      echo "F(a)" > formula.ltlf
#
# - call the script:
#
#      translate.sh formula
#
# - You'll get the following output
#
#      formula.dfa
#      formula.dot
#      formula.mona
#      formula.svg
#      formula/
#             - 0
#             - 0.svg
#             - 1
#             - 1.svg
#

ltlf2fol NNF "$1.ltlf" > "$1.mona"
# print BDD-based MONA DFA
mona -u -xw "$1.mona" > "$1.dfa"

mona -gw "$1.mona" > "$1.dot"
dot -Tsvg "$1.dot" > "$1.svg"

# TODO: print shared multi-terminal BDD (MONA DFA)
# mona ?

rm -rf "${1:?}/"
./build/bin/mona2sym -i "$1.dfa" -o "$1"

eog "$1.svg" "$1"/*.svg &
