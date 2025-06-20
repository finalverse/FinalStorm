#!/bin/bash
# build_setup.sh - unified project generation script

set -e

IOS_ONLY=false
MAC_ONLY=false
CLEAN=false
OPEN=false

usage() {
    echo "Usage: $0 [--ios-only] [--mac-only] [--clean] [--open]"
    echo "  --ios-only Generate project only for iOS"
    echo "  --mac-only Generate project only for macOS"
    echo "  --clean    Remove existing build directory before generating"
    echo "  --open     Open the generated Xcode project"
}

for arg in "$@"; do
    case $arg in
        --ios-only)
            IOS_ONLY=true
            ;;
        --mac-only)
            MAC_ONLY=true
            ;;
        --clean)
            CLEAN=true
            ;;
        --open)
            OPEN=true
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            echo "Unknown option: $arg"
            usage
            exit 1
            ;;
    esac
done

echo "Setting up FinalStorm project..."

if $CLEAN; then
    rm -rf build
fi

mkdir -p build
cd build

cmake_args="-G Xcode"

if $IOS_ONLY; then
    echo "Configuring for iOS only..."
    cmake $cmake_args -DBUILD_IOS_ONLY=ON ..
elif $MAC_ONLY; then
    echo "Configuring for macOS only..."
    cmake $cmake_args -DBUILD_MAC_ONLY=ON ..
else
    echo "Configuring for macOS and iOS..."
    cmake $cmake_args ..
fi

if [ $? -eq 0 ]; then
    echo "✅ Xcode project generated in build/FinalStorm.xcodeproj"
    if $OPEN; then
        open FinalStorm.xcodeproj
    fi
else
    echo "❌ Build setup failed!"
    exit 1
fi



