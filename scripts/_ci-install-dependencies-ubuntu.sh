#!/usr/bin/env bash

set -e

echo "Installing CUDD..."
wget https://github.com/whitemech/cudd/releases/download/v3.0.0/cudd_3.0.0_linux-amd64.tar.gz
tar -xf cudd_3.0.0_linux-amd64.tar.gz
cd cudd_3.0.0_linux-amd64
sudo cp -P lib/* /usr/local/lib/
sudo cp -Pr include/* /usr/local/include

echo "Installing MONA..."
wget https://github.com/whitemech/MONA/releases/download/v1.4-18.dev0/mona_1.4-18.dev0_linux-amd64.tar.gz
tar -xf mona_1.4-18.dev0_linux-amd64.tar.gz
cd mona_1.4-18.dev0_linux-amd64
sudo cp -P lib/* /usr/local/lib/
sudo cp -Pr include/* /usr/local/include

echo "Installing MiniSAT..."
wget https://github.com/whitemech/minisat/releases/download/v2.1.0/minisat_2.1.0_linux-amd64.tar.gz
tar -xf minisat_2.1.0_linux-amd64.tar.gz
cd minisat_2.1.0_linux-amd64
sudo cp -P lib/* /usr/local/lib/
sudo cp -Pr include/* /usr/local/include

cd ..