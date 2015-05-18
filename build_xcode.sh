#!/bin/bash

cur_dir=`dirname "$0"`; cur_dir=`eval "cd \"$cur_dir\" && pwd"`
build_path=$cur_dir"/BuildX"
install_path=$cur_dir"/bin"

# build
mkdir $build_path
cd $build_path
cmake -G Xcode ../

echo ""
echo "done !"
read x