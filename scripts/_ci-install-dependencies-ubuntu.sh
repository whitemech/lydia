#!/usr/bin/env bash

set -e

echo "Installing CUDD..."
wget https://github.com/whitemech/cudd/releases/download/v3.0.0/cudd_3.0.0_linux-amd64.tar.gz
tar -xf cudd_3.0.0_linux-amd64.tar.gz
cd cudd_3.0.0_linux-amd64
sudo cp -P lib/* /usr/local/lib/
sudo cp -Pr include/* /usr/local/include
cd ..
echo "CUDD installed."

echo "Installing MONA..."
wget https://github.com/whitemech/MONA/releases/download/v1.4-19.dev0/mona_1.4-19.dev0_linux-amd64.tar.gz
tar -xf mona_1.4-19.dev0_linux-amd64.tar.gz
cd mona_1.4-19.dev0_linux-amd64
sudo cp -P lib/* /usr/local/lib/
sudo cp -Pr include/* /usr/local/include
cd ..
echo "MONA installed."

echo "Installing Syft..."
git clone https://github.com/whitemech/Syft.git
cd Syft
git checkout v0.1.1
/bin/rm -rf build
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j
sudo make install
echo "Syft installed."