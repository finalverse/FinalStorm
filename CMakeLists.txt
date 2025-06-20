# CMakeLists.txt
# FinalStorm build configuration
# Configures the build for macOS/iOS with Metal rendering

cmake_minimum_required(VERSION 3.20)
project(FinalStorm VERSION 0.1.0 LANGUAGES CXX OBJCXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Platform detection
if(NOT APPLE)
    message(FATAL_ERROR "FinalStorm currently only supports macOS and iOS")
endif()

# Options
option(BUILD_FOR_IOS "Build for iOS instead of macOS" OFF)

# Set deployment target
if(BUILD_FOR_IOS)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "14.0")
    set(CMAKE_OSX_ARCHITECTURES "arm64")
else()
    set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0")
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Find required packages
find_package(Threads REQUIRED)

# Source files
set(CORE_SOURCES
    src/FinalStormApp.cpp
    src/Core/Math/Math.cpp
    src/Core/Math/Transform.cpp
    src/Core/Math/Camera.cpp
    src/Scene/SceneNode.cpp
    src/Scene/Scene.cpp
    src/Scene/SceneManager.cpp
    src/World/Entity.cpp
    src/World/WorldManager.cpp
    src/Services/ServiceEntity.cpp
    src/Services/ServiceFactory.cpp
    src/Environment/EnvironmentController.cpp
    src/UI/UI3DPanel.cpp
    src/UI/HolographicDisplay.cpp
    src/UI/InteractiveOrb.cpp
    src/UI/ServiceDiscoveryUI.cpp
    src/Core/Input/InteractionManager.cpp
    src/Network/FinalverseClient.cpp
    src/Network/MessageProtocol.cpp
)

set(METAL_SOURCES
    src/Rendering/Metal/MetalRenderer.mm
    src/Rendering/Metal/MetalRenderContext.mm
    src/Rendering/Metal/MetalMesh.mm
    src/Rendering/Metal/MetalTexture.mm
)

# Platform-specific sources
if(BUILD_FOR_IOS)
    set(PLATFORM_SOURCES
        src/Platform/iOS/main.mm
        src/Platform/iOS/AppDelegate.mm
        src/Platform/iOS/GameViewController.mm
    )
else()
    set(PLATFORM_SOURCES
        src/Platform/macOS/main.mm
    )
endif()

# Create executable
add_executable(FinalStorm MACOSX_BUNDLE
    ${CORE_SOURCES}
    ${METAL_SOURCES}
    ${PLATFORM_SOURCES}
)

# Set bundle properties
set_target_properties(FinalStorm PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.finalverse.finalstorm"
    MACOSX_BUNDLE_BUNDLE_NAME "FinalStorm"
    MACOSX_BUNDLE_BUNDLE_VERSION "0.1.0"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "0.1"
)

# Include directories
target_include_directories(FinalStorm PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link frameworks
target_link_libraries(FinalStorm PRIVATE
    "-framework Foundation"
    "-framework Metal"
    "-framework MetalKit"
    "-framework QuartzCore"
    "-framework CoreGraphics"
    Threads::Threads
)

if(BUILD_FOR_IOS)
    target_link_libraries(FinalStorm PRIVATE
        "-framework UIKit"
        "-framework ARKit"
    )
else()
    target_link_libraries(FinalStorm PRIVATE
        "-framework Cocoa"
        "-framework AppKit"
    )
endif()

# Compile definitions
target_compile_definitions(FinalStorm PRIVATE
    USE_SIMD  # Use Apple's SIMD types
)

# Copy shaders to build directory
add_custom_command(TARGET FinalStorm POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/shaders
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders
)

# Copy assets to build directory
add_custom_command(TARGET FinalStorm POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets
)

# Installation
install(TARGETS FinalStorm
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)

# Install shaders and assets
install(DIRECTORY shaders DESTINATION bin)
install(DIRECTORY assets DESTINATION bin)        