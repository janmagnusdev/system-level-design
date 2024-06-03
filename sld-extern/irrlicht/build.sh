#!/bin/sh
set -e
make -C source/Irrlicht
mv lib/Linux/*.a ../lib
cp -r include ../include/irrlicht
make -C source/Irrlicht clean
