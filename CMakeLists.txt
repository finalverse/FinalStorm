#
#  CMakeLists.txt
#  FinalStorm
#
#  Created by Wenyan Qin on 2025-06-15.
#

cmake_minimum_required(VERSION 3.20)
project(FinalStorm)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum OS X deployment version")

# Options
option(BUILD_IOS "Build for iOS" OFF)

# Find frameworks
find_library(METAL_FRAMEWORK Metal REQUIRED)
find_library(METALKIT_FRAMEWORK MetalKit REQUIRED)
find_library(QUARTZCORE_FRAMEWORK QuartzCore REQUIRED)
find_library(COREFOUNDATION_FRAMEWORK CoreFoundation REQUIRED)

if(BUILD_IOS)
    find_library(UIKIT_FRAMEWORK UIKit REQUIRED)
    set(PLATFORM_FRAMEWORKS ${UIKIT_FRAMEWORK})
else()
    find_library(COCOA_FRAMEWORK Cocoa REQUIRED)
    set(PLATFORM_FRAMEWORKS ${COCOA_FRAMEWORK})
endif()

# Source files
set(SHARED_SOURCES
    Shared/Core/Math/Math.cpp
    Shared/Core/World/Entity.cpp
    Shared/Core/World/WorldManager.cpp
    Shared/Core/Networking/FinalverseClient.cpp
)

set(SHARED_HEADERS
    Shared/Core/Math/Math.h
    Shared/Core/Math/Transform.h
    Shared/Core/World/Entity.h
    Shared/Core/World/WorldManager.h
    Shared/Core/Networking/FinalverseClient.h
    Shared/Core/Networking/MessageProtocol.h
    Shared/Renderer/Renderer.h
    Shared/Renderer/ShaderTypes.h
)

if(BUILD_IOS)
    set(PLATFORM_SOURCES
        iOS/main.m
        iOS/AppDelegate.m
        iOS/GameViewController.mm
        macOS/Renderer/MetalRenderer.mm
    )
else()
    set(PLATFORM_SOURCES
        macOS/main.m
        macOS/GameViewController.mm
        macOS/Renderer/MetalRenderer.mm
    )
endif()

# Metal shaders
set(METAL_SHADERS
    macOS/Renderer/Shaders.metal
)

# Create shader library
add_custom_target(ShaderLibrary ALL)

foreach(SHADER ${METAL_SHADERS})
    get_filename_component(SHADER_NAME ${SHADER} NAME_WE)
    set(SHADER_LIB ${CMAKE_CURRENT_BINARY_DIR}/${SHADER_NAME}.metallib)
    
    add_custom_command(
        OUTPUT ${SHADER_LIB}
        COMMAND xcrun -sdk ${SDK} metal -c ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER} -o ${CMAKE_CURRENT_BINARY_DIR}/${SHADER_NAME}.air
        COMMAND xcrun -sdk ${SDK} metallib ${CMAKE_CURRENT_BINARY_DIR}/${SHADER_NAME}.air -o ${SHADER_LIB}
        DEPENDS ${SHADER}
        COMMENT "Compiling Metal shader ${SHADER_NAME}"
    )
    
    add_custom_target(${SHADER_NAME} DEPENDS ${SHADER_LIB})
    add_dependencies(ShaderLibrary ${SHADER_NAME})
endforeach()

# Create executable
add_executable(FinalStorm MACOSX_BUNDLE
    ${SHARED_SOURCES}
    ${SHARED_HEADERS}
    ${PLATFORM_SOURCES}
)

# Include directories
target_include_directories(FinalStorm PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link libraries
target_link_libraries(FinalStorm
    ${METAL_FRAMEWORK}
    ${METALKIT_FRAMEWORK}
    ${QUARTZCORE_FRAMEWORK}
    ${COREFOUNDATION_FRAMEWORK}
    ${PLATFORM_FRAMEWORKS}
)

# Bundle configuration
if(BUILD_IOS)
    set_target_properties(FinalStorm PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/iOS/Info.plist
        XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
        XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "14.0"
    )
else()
    set_target_properties(FinalStorm PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/macOS/Info.plist
    )
endif()

# Copy shader library to bundle
add_custom_command(TARGET FinalStorm POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/*.metallib $<TARGET_BUNDLE_CONTENT_DIR:FinalStorm>/Resources/
)

# Copy resources
file(GLOB_RECURSE RESOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Resources/*")
foreach(RESOURCE ${RESOURCES})
    add_custom_command(TARGET FinalStorm POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${RESOURCE} $<TARGET_BUNDLE_CONTENT_DIR:FinalStorm>/Resources/
    )
endforeach()
