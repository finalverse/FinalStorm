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
option(BUILD_IOS_ONLY "Build only the iOS target" OFF)
option(BUILD_MAC_ONLY "Build only the macOS target" OFF)

if(BUILD_IOS_ONLY AND BUILD_MAC_ONLY)
    message(FATAL_ERROR "Only one of BUILD_IOS_ONLY or BUILD_MAC_ONLY can be ON")
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
set(MACOS_SOURCES
    src/Platform/macOS/main.mm
    src/Platform/macOS/AppDelegate.mm
    src/Platform/macOS/GameViewController.mm
)

set(IOS_SOURCES
    src/Platform/iOS/main.mm
    src/Platform/iOS/AppDelegate.mm
    src/Platform/iOS/GameViewController.mm
)

# Shaders
file(GLOB SHADER_FILES ${CMAKE_SOURCE_DIR}/shaders/*.metal)

add_custom_target(Shaders
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/compile_shaders.sh --all
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Compiling Metal shaders"
)

########################################################################
# Targets
########################################################################

if(NOT BUILD_IOS_ONLY)
    add_executable(FinalStorm-macOS MACOSX_BUNDLE
        ${CORE_SOURCES}
        ${METAL_SOURCES}
        ${MACOS_SOURCES}
    )
endif()

if(NOT BUILD_MAC_ONLY)
    add_executable(FinalStorm-iOS MACOSX_BUNDLE
        ${CORE_SOURCES}
        ${METAL_SOURCES}
        ${IOS_SOURCES}
    )
endif()

########################################################################
# Target configuration
########################################################################

set(COMMON_FRAMEWORKS
    "-framework Foundation"
    "-framework Metal"
    "-framework MetalKit"
    "-framework QuartzCore"
    "-framework CoreGraphics"
    Threads::Threads
)

if(NOT BUILD_IOS_ONLY)
    set_target_properties(FinalStorm-macOS PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.finalverse.finalstorm"
        MACOSX_BUNDLE_BUNDLE_NAME "FinalStorm"
        MACOSX_BUNDLE_BUNDLE_VERSION "0.1.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "0.1"
        MACOSX_DEPLOYMENT_TARGET "11.0"
    )
    target_include_directories(FinalStorm-macOS PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
    target_link_libraries(FinalStorm-macOS PRIVATE
        ${COMMON_FRAMEWORKS}
        "-framework Cocoa"
        "-framework AppKit"
    )
    target_compile_definitions(FinalStorm-macOS PRIVATE USE_SIMD)
    add_dependencies(FinalStorm-macOS Shaders)
    add_custom_command(TARGET FinalStorm-macOS POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets
    )
    add_custom_command(TARGET FinalStorm-macOS POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders
    )
endif()

if(NOT BUILD_MAC_ONLY)
    set_target_properties(FinalStorm-iOS PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.finalverse.finalstorm"
        MACOSX_BUNDLE_BUNDLE_NAME "FinalStorm"
        MACOSX_BUNDLE_BUNDLE_VERSION "0.1.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "0.1"
        MACOSX_DEPLOYMENT_TARGET "14.0"
        OSX_ARCHITECTURES "arm64"
        XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
    )
    target_include_directories(FinalStorm-iOS PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
    target_link_libraries(FinalStorm-iOS PRIVATE
        ${COMMON_FRAMEWORKS}
        "-framework UIKit"
        "-framework ARKit"
    )
    target_compile_definitions(FinalStorm-iOS PRIVATE USE_SIMD)
    add_dependencies(FinalStorm-iOS Shaders)
    add_custom_command(TARGET FinalStorm-iOS POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets
    )
    add_custom_command(TARGET FinalStorm-iOS POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders
    )
endif()

########################################################################
# Installation
########################################################################

if(NOT BUILD_IOS_ONLY)
    install(TARGETS FinalStorm-macOS
        BUNDLE DESTINATION .
        RUNTIME DESTINATION bin
    )
endif()

if(NOT BUILD_MAC_ONLY)
    install(TARGETS FinalStorm-iOS
        BUNDLE DESTINATION .
        RUNTIME DESTINATION bin
    )
endif()

install(DIRECTORY shaders DESTINATION bin)
install(DIRECTORY assets DESTINATION bin)
