cmake_minimum_required(VERSION 3.20)
project(FinalStorm)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum OS X deployment version")

# Options
option(BUILD_IOS "Build for iOS" OFF)

# Set bundle identifier
set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.finalverse.finalstorm")
set(MACOSX_BUNDLE_BUNDLE_NAME "FinalStorm")

# Find frameworks
find_library(METAL_FRAMEWORK Metal REQUIRED)
find_library(METALKIT_FRAMEWORK MetalKit REQUIRED)
find_library(QUARTZCORE_FRAMEWORK QuartzCore REQUIRED)
find_library(COREFOUNDATION_FRAMEWORK CoreFoundation REQUIRED)
find_library(FOUNDATION_FRAMEWORK Foundation REQUIRED)

if(BUILD_IOS)
    find_library(UIKIT_FRAMEWORK UIKit REQUIRED)
    set(PLATFORM_FRAMEWORKS ${UIKIT_FRAMEWORK})
else()
    find_library(COCOA_FRAMEWORK Cocoa REQUIRED)
    find_library(APPKIT_FRAMEWORK AppKit REQUIRED)
    set(PLATFORM_FRAMEWORKS ${COCOA_FRAMEWORK} ${APPKIT_FRAMEWORK})
endif()

# Source files
set(SHARED_SOURCES
    Shared/Core/Math/Math.cpp
    Shared/Core/World/Entity.cpp
    Shared/Core/World/WorldManager.cpp
    Shared/Core/Networking/FinalverseClient.cpp
    Shared/Core/Networking/MessageProtocol.cpp
    Shared/Core/Audio/AudioEngine.cpp
)

set(SHARED_HEADERS
    Shared/Core/Math/Math.h
    Shared/Core/Math/Transform.h
    Shared/Core/World/Entity.h
    Shared/Core/World/WorldManager.h
    Shared/Core/Networking/FinalverseClient.h
    Shared/Core/Networking/MessageProtocol.h
    Shared/Core/Audio/AudioEngine.h
    Shared/Renderer/Renderer.h
    Shared/Renderer/ShaderTypes.h
)

if(BUILD_IOS)
    set(PLATFORM_SOURCES
        iOS/main.m
        iOS/AppDelegate.m
        iOS/GameViewController.mm
        MetalRenderer/MetalRenderer.mm
    )
    set(INFO_PLIST_PATH ${CMAKE_CURRENT_SOURCE_DIR}/iOS/Info.plist)
else()
    set(PLATFORM_SOURCES
        macOS/main.m
        macOS/GameViewController.mm
        MetalRenderer/MetalRenderer.mm
    )
    set(INFO_PLIST_PATH ${CMAKE_CURRENT_SOURCE_DIR}/macOS/Info.plist)
endif()

# Metal shaders
set(METAL_SHADERS
    MetalRenderer/Shaders.metal
)

# Create executable - THIS MUST COME BEFORE target_link_libraries
add_executable(FinalStorm MACOSX_BUNDLE
    ${SHARED_SOURCES}
    ${SHARED_HEADERS}
    ${PLATFORM_SOURCES}
    ${METAL_SHADERS}
)

# Set bundle properties
set_target_properties(FinalStorm PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_INFO_PLIST ${INFO_PLIST_PATH}
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.finalverse.finalstorm"
    MACOSX_BUNDLE_BUNDLE_NAME "FinalStorm"
    MACOSX_BUNDLE_BUNDLE_VERSION "1.0"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.finalverse.finalstorm"
    XCODE_ATTRIBUTE_CURRENT_PROJECT_VERSION "1"
    XCODE_ATTRIBUTE_MARKETING_VERSION "1.0"
)

# Include directories
target_include_directories(FinalStorm PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link libraries - THIS MUST COME AFTER add_executable
target_link_libraries(FinalStorm
    ${METAL_FRAMEWORK}
    ${METALKIT_FRAMEWORK}
    ${QUARTZCORE_FRAMEWORK}
    ${COREFOUNDATION_FRAMEWORK}
    ${FOUNDATION_FRAMEWORK}
    ${PLATFORM_FRAMEWORKS}
)

# Compile Metal shaders
if(BUILD_IOS)
    set(METAL_SDK iphonesimulator)
else()
    set(METAL_SDK macosx)
endif()

add_custom_command(
    TARGET FinalStorm POST_BUILD
    COMMAND xcrun -sdk ${METAL_SDK} metal -c ${CMAKE_CURRENT_SOURCE_DIR}/MetalRenderer/Shaders.metal -o ${CMAKE_CURRENT_BINARY_DIR}/Shaders.air
    COMMAND xcrun -sdk ${METAL_SDK} metallib ${CMAKE_CURRENT_BINARY_DIR}/Shaders.air -o ${CMAKE_CURRENT_BINARY_DIR}/default.metallib
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/default.metallib $<TARGET_BUNDLE_CONTENT_DIR:FinalStorm>/Resources/
    COMMENT "Compiling Metal shaders"
)

# Copy Info.plist
configure_file(${INFO_PLIST_PATH} ${CMAKE_CURRENT_BINARY_DIR}/Info.plist COPYONLY)

# Set deployment target for iOS
if(BUILD_IOS)
    set_target_properties(FinalStorm PROPERTIES
        XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "14.0"
        XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
    )
endif()

# Enable ARC
set_target_properties(FinalStorm PROPERTIES
    XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC "YES"
)

# Set C++ standard library
set_target_properties(FinalStorm PROPERTIES
    XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++"
)