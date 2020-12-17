#!/usr/bin/env bash

set -e

echo "Installing CUDD..."
wget https://github.com/whitemech/cudd/releases/download/v3.0.0/cudd_3.0.0_linux-amd64.tar.gz
tar -xf cudd_3.0.0_linux-amd64.tar.gz
cd cudd_3.0.0_linux-amd64
sudo cp -P lib/* /usr/local/lib/
sudo cp -Pr include/* /usr/local/include
# TODO: fix Syft to add the prefix "<cudd/...> for CUDD headers.
sudo cp -Pr include/cudd/ /usr/local/include
cd ..
echo "CUDD installed."

echo "Installing MONA..."
wget https://github.com/whitemech/MONA/releases/download/v1.4-18.dev0/mona_1.4-18.dev0_linux-amd64.tar.gz
tar -xf mona_1.4-18.dev0_linux-amd64.tar.gz
cd mona_1.4-18.dev0_linux-amd64
sudo cp -P lib/* /usr/local/lib/
sudo cp -Pr include/* /usr/local/include
cd ..
echo "MONA installed."

echo "Installing MiniSAT..."
wget https://github.com/whitemech/minisat/releases/download/v2.1.0/minisat_2.1.0_linux-amd64.tar.gz
tar -xf minisat_2.1.0_linux-amd64.tar.gz
cd minisat_2.1.0_linux-amd64
sudo cp -P lib/* /usr/local/lib/
sudo cp -Pr include/* /usr/local/include
cd ..
echo "MiniSAT installed."

echo "Installing Syft..."
cd third_party/Syft
/bin/rm -rf build
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j
sudo make install
echo "Syft installed."