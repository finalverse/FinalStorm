#!/bin/bash

# gen_xcode.sh - create Xcode project using CMake
echo "Setting up FinalStorm Xcode project..."

# Clean previous builds
rm -rf build
mkdir -p build
cd build
cmake -G Xcode ..
cd ..

echo "Xcode project generated in build/FinalStorm.xcodeproj"

