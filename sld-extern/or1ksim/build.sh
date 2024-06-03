#!/bin/sh
set -e
rm -rf build
mkdir build
cd build
../configure --prefix="$PWD/../../" --disable-shared
make
make install
cd ..
rm -r build
