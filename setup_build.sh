#!/bin/bash
# setup_build.sh
# Build setup script for FinalStorm
# Creates build directory and runs CMake

echo "Setting up FinalStorm build..."

# Create build directory
mkdir -p build
cd build

# Determine platform
if [[ "$1" == "ios" ]]; then
    echo "Configuring for iOS..."
    cmake -G Xcode -DBUILD_FOR_IOS=ON ..
else
    echo "Configuring for macOS..."
    cmake -G Xcode ..
fi

if [ $? -eq 0 ]; then
    echo ""
    echo "Build setup complete!"
    echo ""
    echo "To build FinalStorm:"
    echo "  1. Open build/FinalStorm.xcodeproj in Xcode"
    echo "  2. Select FinalStorm target"
    echo "  3. Select your device/simulator"
    echo "  4. Build and run (Cmd+R)"
    echo ""
    echo "Or build from command line:"
    echo "  cd build"
    echo "  xcodebuild -scheme FinalStorm -configuration Debug"
else
    echo "Build setup failed!"
    exit 1
fi