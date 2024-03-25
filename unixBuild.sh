#!/bin/bash (si Linux ou Macos)
mkdir -p build
cd build
cmake -S ../ -B .
make && make Shaders && ./LveEngine
cd ..
