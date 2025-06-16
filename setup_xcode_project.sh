#!/bin/bash

# Clean previous builds
rm -rf build
mkdir build
cd build

# Generate Xcode project with proper settings
cmake -G Xcode \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0 \
    ..

# Open the project
open FinalStorm.xcodeproj

echo "FinalStorm Xcode project is ready!"
echo "Build and run in Xcode to start the game."