#!/bin/bash

# Metal Shader Compilation Script for FinalStorm
# This script compiles all Metal shaders to verify they're syntactically correct

SHADER_DIR="./shaders"
OUTPUT_DIR="./shaders/compiled"
PLATFORM="macosx"  # or "ios" for iOS
IOS_PLATFORM="iphoneos"

# Create output directory if it doesn't exist
mkdir -p "$OUTPUT_DIR"
mkdir -p "$OUTPUT_DIR/macos"
mkdir -p "$OUTPUT_DIR/ios"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo "🔧 Compiling Metal shaders for FinalStorm..."
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

# Function to compile a shader
compile_shader() {
    local shader_file=$1
    local platform=$2
    local output_subdir=$3
    local shader_name=$(basename "$shader_file" .metal)
    
    echo -n "Compiling $shader_name.metal for $platform... "
    
    # Compile to AIR (Apple Intermediate Representation)
    xcrun -sdk $platform metal -c "$shader_file" -o "$OUTPUT_DIR/$output_subdir/$shader_name.air" 2> "$OUTPUT_DIR/$output_subdir/$shader_name.log"
    
    if [ $? -eq 0 ]; then
        # If AIR compilation successful, compile to metallib
        xcrun -sdk $platform metallib "$OUTPUT_DIR/$output_subdir/$shader_name.air" -o "$OUTPUT_DIR/$output_subdir/$shader_name.metallib" 2>> "$OUTPUT_DIR/$output_subdir/$shader_name.log"
        
        if [ $? -eq 0 ]; then
            echo -e "${GREEN}✓ Success${NC}"
            rm "$OUTPUT_DIR/$output_subdir/$shader_name.log"  # Remove log if successful
            return 0
        else
            echo -e "${RED}✗ Failed (metallib stage)${NC}"
            echo "  Error log: $OUTPUT_DIR/$output_subdir/$shader_name.log"
            return 1
        fi
    else
        echo -e "${RED}✗ Failed (compilation stage)${NC}"
        echo "  Error log: $OUTPUT_DIR/$output_subdir/$shader_name.log"
        return 1
    fi
}

# Compile all shaders for macOS
echo "Compiling for macOS..."
success_count=0
fail_count=0

for shader in "$SHADER_DIR"/*.metal; do
    if [ -f "$shader" ]; then
        compile_shader "$shader" "$PLATFORM" "macos"
        if [ $? -eq 0 ]; then
            ((success_count++))
        else
            ((fail_count++))
        fi
    fi
done

echo "macOS: ✓ $success_count, ✗ $fail_count"

# Compile all shaders for iOS (optional)
if [ "$1" == "--ios" ] || [ "$1" == "--all" ]; then
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo "Compiling for iOS..."
    ios_success_count=0
    ios_fail_count=0
    
    for shader in "$SHADER_DIR"/*.metal; do
        if [ -f "$shader" ]; then
            compile_shader "$shader" "$IOS_PLATFORM" "ios"
            if [ $? -eq 0 ]; then
                ((ios_success_count++))
            else
                ((ios_fail_count++))
            fi
        fi
    done
    
    echo "iOS: ✓ $ios_success_count, ✗ $ios_fail_count"
fi

echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "Compilation complete!"

if [ $fail_count -eq 0 ]; then
    echo -e "${GREEN}All shaders compiled successfully for macOS!${NC}"
    
    # Create unified metallib using AIR files instead of metallib files
    echo -n "Creating unified shader library... "
    
    # For macOS
    xcrun -sdk $PLATFORM metallib "$OUTPUT_DIR/macos"/*.air -o "$OUTPUT_DIR/macos/FinalStorm.metallib" 2> "$OUTPUT_DIR/macos/unified.log"
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ Success${NC}"
        echo "Unified library: $OUTPUT_DIR/macos/FinalStorm.metallib"
        rm "$OUTPUT_DIR/macos/unified.log"
    else
        echo -e "${YELLOW}⚠ Warning: Could not create unified library${NC}"
        echo "Check log: $OUTPUT_DIR/macos/unified.log"
    fi
    
    # For iOS if compiled
    if [ "$1" == "--ios" ] || [ "$1" == "--all" ]; then
        if [ $ios_fail_count -eq 0 ]; then
            xcrun -sdk $IOS_PLATFORM metallib "$OUTPUT_DIR/ios"/*.air -o "$OUTPUT_DIR/ios/FinalStorm.metallib" 2> "$OUTPUT_DIR/ios/unified.log"
            if [ $? -eq 0 ]; then
                echo "iOS unified library: $OUTPUT_DIR/ios/FinalStorm.metallib"
                rm "$OUTPUT_DIR/ios/unified.log"
            fi
        fi
    fi
else
    echo -e "${RED}Some shaders failed to compile${NC}"
    exit 1
fi

# Clean up intermediate files
# rm -f "$OUTPUT_DIR/macos"/*.air
# rm -f "$OUTPUT_DIR/ios"/*.air

exit 0
