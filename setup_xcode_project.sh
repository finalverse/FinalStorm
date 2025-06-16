#!/bin/bash

echo "Setting up FinalStorm Xcode project..."

# Clean previous builds
rm -rf build
mkdir build
cd build

# Generate Xcode project with both macOS and iOS targets
echo "Generating Xcode project..."
cmake -G Xcode \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0 \
    ..

if [ $? -eq 0 ]; then
    echo "✅ Xcode project generated successfully!"
    echo ""
    echo "Available targets:"
    echo "  - FinalStorm (macOS)"
    echo "  - FinalStorm-iOS (iOS)"
    echo ""
    echo "Opening Xcode..."
    open FinalStorm.xcodeproj
else
    echo "❌ Failed to generate Xcode project"
    exit 1
fi