#!/bin/bash

# args:
#   Positional:
#       Required: <source>($1), <build destination>($2)
#       Optional: <site media dir>($3)
#           * This copies built .js, .wasm, .data and .html if found
#           from <build destination>($2) into <site media dir>($3)
#           * This is to use a single command to build the required
#           wasm stuff into the "site media target"
#           (we dont want to copy the whole "original build dir" into the site
#           since it contains so much unnecessary stuff for the site itself)

# NOTE: To run this you first need to have done:
#   <path to emsdk>/emsdk activate latest
#   source <path to emsdk>/emsdk_env.sh

root_dir=$( pwd )
echo "Using root dir: $root_dir"

if [[ -z "$1" ]] && [[ -z "$2" ]]
then
    echo -e "\e[31mInvalid arguments. Source dir and build destination dir required\e[0m"
else
    src_dir=$root_dir/$1
    build_dir=$root_dir/$2

    echo "Using source dir: $src_dir"
    echo "Using build dir: $build_dir"

    cd $build_dir

    echo "starting building to $build_dir"

    emcmake cmake -S ${src_dir} -B . -DBUILD_TYPE="PesukarhuWeb"
    cmake --build .
fi

