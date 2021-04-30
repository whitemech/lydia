#!/usr/bin/env bash

PLATFORM="linux"
ARCH="amd64"
VERSION="0.1.0"
RELEASE_NAME="lydia-${VERSION}_${PLATFORM}-${ARCH}"
prefix="/usr/local"
includedir="${prefix}/include/"

OUTPUT_DIR="${RELEASE_NAME}"
OUTPUT_TAR="${RELEASE_NAME}.tar.gz"

mkdir build && cd build && cmake .. && make -j4 && cd ..

rm -rf "${OUTPUT_DIR}"
mkdir "${OUTPUT_DIR}"
cp README.md "${OUTPUT_DIR}"
cp LICENSE  "${OUTPUT_DIR}"

mkdir ${OUTPUT_DIR}/bin
cp build/bin/lydia ${OUTPUT_DIR}/bin

tar -c "${OUTPUT_DIR}" -f "${OUTPUT_TAR}"
rm -r "${OUTPUT_DIR}"

echo "Output in ${OUTPUT_TAR}"