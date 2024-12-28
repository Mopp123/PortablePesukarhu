#!/bin/bash

# Get script's dir
# looks fucked but necessary to get absolute path... or idk, I'm not shell wizard...
root_dir=$( dirname "$0" )
cd $root_dir
root_dir=$( pwd )/dependencies
cd $root_dir

# build glfw
echo "Building GLFW..."
cd glfw/
cmake -S . -B build -D BUILD_SHARED_LIBS=ON
cmake --build ./build/

cd $root_dir

# build GLEW
# NOTE: Remember the python/python3 issue!
cd glew/auto
echo "Making glew(auto)..."
make
cd ..
echo "Making glew..."
make
# I recall these installed some random unnecessary shit...
#make install
#make clean

# build Freetype and HarfBuzz
# initial Freetype..
echo "Building freetype(without harfbuzz)"
cd ../freetype
./autogen.sh
./configure --without-harfbuzz
make
# build HarfBuzz
echo "Building harfbuzz"
cd ../harfbuzz
meson build
meson compile -C build
# finalize building Freetype
cd ../freetype
make distclean
./configure
make
