#!/bin/bash
# check_structure.sh

echo "Verifying FinalStorm project structure..."

# Define all required files
files=(
    "src/Core/Math/Math.h"
    "src/Core/Math/Math.cpp"
    "src/Core/Math/Transform.h"
    "src/World/Entity.h"
    "src/World/Entity.cpp"
    "src/World/WorldManager.h"
    "src/World/WorldManager.cpp"
    "src/Network/FinalverseClient.h"
    "src/Network/FinalverseClient.cpp"
    "src/Network/MessageProtocol.h"
    "src/Network/MessageProtocol.cpp"
    "src/Core/Audio/AudioEngine.h"
    "src/Core/Audio/AudioEngine.cpp"
    "src/Rendering/Renderer.h"
    "src/Rendering/ShaderTypes.h"
    "src/Platform/macOS/main.mm"
    "src/Platform/macOS/GameViewController.h"
    "src/Platform/macOS/GameViewController.mm"
    "src/Rendering/Metal/MetalRenderer.hpp"
    "src/Rendering/Metal/MetalRenderer.mm"
    "src/Rendering/Metal/MetalShaderTypes.h"
    "shaders/Core.metal"
    "src/Platform/macOS/Info.plist"
    "src/Platform/iOS/main.mm"
    "src/Platform/iOS/AppDelegate.h"
    "src/Platform/iOS/AppDelegate.mm"
    "src/Platform/iOS/GameViewController.h"
    "src/Platform/iOS/GameViewController.mm"
    "src/Platform/iOS/Info.plist"
    "CMakeLists.txt"
)

missing=0
for file in "${files[@]}"; do
    if [ ! -f "$file" ]; then
        echo "❌ Missing: $file"
        missing=$((missing + 1))
    else
        echo "✅ Found: $file"
    fi
done

if [ $missing -eq 0 ]; then
    echo "✅ All files present! Project structure is complete."
else
    echo "❌ Missing $missing files. Please create them."
fi

