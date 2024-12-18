#!/bin/bash

# Get script's dir
root_dir=$( dirname "$0" )/pesukarhu
cd $root_dir

# build glfw
echo "Building GLFW..."
cd glfw/
cmake -S . -B build -D BUILD_SHARED_LIBS=ON
cmake --build ./build/

echo "GLFW build success!"
cd $root_dir
