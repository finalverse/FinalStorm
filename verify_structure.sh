#!/bin/bash
# verify_structure.sh

echo "Verifying FinalStorm project structure..."

# Define all required files
files=(
    "Shared/Core/Math/Math.h"
    "Shared/Core/Math/Math.cpp"
    "Shared/Core/Math/Transform.h"
    "Shared/Core/World/Entity.h"
    "Shared/Core/World/Entity.cpp"
    "Shared/Core/World/WorldManager.h"
    "Shared/Core/World/WorldManager.cpp"
    "Shared/Core/Networking/FinalverseClient.h"
    "Shared/Core/Networking/FinalverseClient.cpp"
    "Shared/Core/Networking/MessageProtocol.h"
    "Shared/Core/Networking/MessageProtocol.cpp"
    "Shared/Core/Audio/AudioEngine.h"
    "Shared/Core/Audio/AudioEngine.cpp"
    "Shared/Renderer/Renderer.h"
    "Shared/Renderer/ShaderTypes.h"
    "macOS/main.m"
    "macOS/GameViewController.h"
    "macOS/GameViewController.mm"
    "macOS/Renderer/MetalRenderer.h"
    "macOS/Renderer/MetalRenderer.mm"
    "macOS/Renderer/Shaders.metal"
    "macOS/Renderer/MetalShaderTypes.h"
    "macOS/Info.plist"
    "iOS/main.m"
    "iOS/AppDelegate.h"
    "iOS/AppDelegate.m"
    "iOS/GameViewController.h"
    "iOS/GameViewController.mm"
    "iOS/Info.plist"
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