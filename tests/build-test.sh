#!/bin/bash

# NOTE: To run this you first need to have done:
#   <path to emsdk>/emsdk activate latest
#   source <path to emsdk>/emsdk_env.sh

root_dir=$( pwd )

if [[ -z "$1" ]]
then
    echo -e "\e[31mBuild type required!\e[0m"
else
    src_dir=$root_dir
    build_dir=$root_dir/web/build
    build_type=$1

    echo "Using source dir: $src_dir"

    cd $build_dir

    echo "starting $build_type build to $build_dir"

    if [[ "$build_type" == "web" ]]
    then
        emcmake cmake -S ${src_dir} -B . -DBUILD_TYPE=$build_type
        cmake --build .
    elif [[ "$build_type" == "linux" ]]
    then
        echo "Linux build not yet implemented!"
    else
        echo -e "\e[31mUnsupported build type: $build_type\e[0m"
        echo "Currently supported build types:"
        echo "  web"
        echo "  linux"
    fi
fi

