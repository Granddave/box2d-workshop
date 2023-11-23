#!/usr/bin/env bash

# Use this to build box2d on any system with a bash shell
rm -rf build
mkdir build
cd build
cmake -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DBOX2D_BUILD_DOCS=OFF ..
cmake --build .
