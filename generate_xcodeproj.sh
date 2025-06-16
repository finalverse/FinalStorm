#!/bin/bash

# Create Xcode project using CMake
mkdir -p build
cd build
cmake -G Xcode ..
cd ..

echo "Xcode project generated in build/FinalStorm.xcodeproj"

