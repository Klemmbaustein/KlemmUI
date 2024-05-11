#!/bin/bash

declare -a dependencies=("Dependencies/SDL" "Dependencies/glew-cmake")

for i in "${dependencies[@]}"; do
	cd $i
	cmake -S . -B bld/
	cd bld/
	make
	cd ../../..
done

mkdir -p lib/Debug
mkdir -p lib/Release

cp Dependencies/glew-cmake/bld/lib/libglew.a lib/Debug/
cp Dependencies/glew-cmake/bld/lib/libglew.a lib/Release/

cd Markup/MarkupHeaderTool
cmake -S . -B Build/
cd Build
make