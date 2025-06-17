cmake_minimum_required(VERSION 3.20)
project(FinalStorm VERSION 1.0.0 LANGUAGES CXX OBJCXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum OS X deployment version")

# Find frameworks
find_library(METAL_FRAMEWORK Metal REQUIRED)
find_library(METALKIT_FRAMEWORK MetalKit REQUIRED)
find_library(QUARTZCORE_FRAMEWORK QuartzCore REQUIRED)
find_library(COREFOUNDATION_FRAMEWORK CoreFoundation REQUIRED)
find_library(FOUNDATION_FRAMEWORK Foundation REQUIRED)
find_library(COCOA_FRAMEWORK Cocoa)
find_library(APPKIT_FRAMEWORK AppKit)
find_library(UIKIT_FRAMEWORK UIKit)
find_library(COREGRAPHICS_FRAMEWORK CoreGraphics)

# Core sources
set(CORE_SOURCES
    src/main.cpp
    src/FinalStormApp.cpp
    src/Core/Math/Math.cpp
    src/Core/World/Entity.cpp
    src/Core/World/WorldManager.cpp
    src/Core/Networking/FinalverseClient.cpp
    src/Core/Networking/MessageProtocol.cpp
    src/Core/Audio/AudioEngine.cpp
    src/Core/Audio/SpatialAudioSystem.cpp
    src/Core/Input/InteractionManager.cpp
)

# Scene sources
set(SCENE_SOURCES
    src/Scene/SceneManager.cpp
    src/Scene/SceneNode.cpp
    src/Scene/CameraController.cpp
)

# Service sources
set(SERVICE_SOURCES
    src/Services/ServiceEntity.cpp
    src/Services/ServiceFactory.cpp
    src/Services/ServiceNode.cpp
    src/Services/ServiceVisualizations.cpp
    src/Services/Visualizations/DatabaseViz.cpp
    src/Services/Visualizations/WebServerViz.cpp
)

# Environment sources
set(ENVIRONMENT_SOURCES
    src/Core/Environment/EnvironmentController.cpp
)

# UI sources
set(UI_SOURCES
    src/UI/Panel.cpp
    src/UI/HolographicDisplay.cpp
    src/UI/InteractiveOrb.cpp
)

# Visualization sources
set(VISUALIZATION_SOURCES
    src/Core/Visualization/DataVisualizer.cpp
)

# Rendering sources
set(RENDERING_SOURCES
    src/Renderer/Metal/MetalRenderer.mm
)

# Shader files
set(SHADER_FILES
    shaders/Common.metal
    shaders/Core.metal
    shaders/Service.metal
    shaders/Environment.metal
    shaders/UI.metal
    shaders/Particles.metal
    shaders/PostProcess.metal
)

# Combine all sources
set(ALL_SOURCES
    ${CORE_SOURCES}
    ${SCENE_SOURCES}
    ${SERVICE_SOURCES}
    ${ENVIRONMENT_SOURCES}
    ${UI_SOURCES}
    ${VISUALIZATION_SOURCES}
    ${RENDERING_SOURCES}
)

# macOS target
add_executable(FinalStorm MACOSX_BUNDLE
    ${ALL_SOURCES}
    src/Platform/macOS/main.mm
    src/Platform/macOS/AppDelegate.mm
    src/Platform/macOS/GameViewController.mm
)

# iOS target
add_executable(FinalStorm-iOS MACOSX_BUNDLE
    ${ALL_SOURCES}
    src/Platform/iOS/main.mm
    src/Platform/iOS/AppDelegate.mm
    src/Platform/iOS/GameViewController.mm
    src/Platform/iOS/ARMode.mm
)

# Include directories
target_include_directories(FinalStorm
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Core
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Scene
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Services
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/Environment
        ${CMAKE_CURRENT_SOURCE_DIR}/src/UI
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Renderer
)

target_include_directories(FinalStorm-iOS
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Core
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Scene
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Services
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/Environment
        ${CMAKE_CURRENT_SOURCE_DIR}/src/UI
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Renderer
)

# Link frameworks
target_link_libraries(FinalStorm
    ${METAL_FRAMEWORK}
    ${METALKIT_FRAMEWORK}
    ${QUARTZCORE_FRAMEWORK}
    ${COREFOUNDATION_FRAMEWORK}
    ${FOUNDATION_FRAMEWORK}
    ${COCOA_FRAMEWORK}
    ${APPKIT_FRAMEWORK}
)

target_link_libraries(FinalStorm-iOS
    "-framework Metal"
    "-framework MetalKit"
    "-framework QuartzCore"
    "-framework CoreFoundation"
    "-framework Foundation"
    "-framework CoreGraphics"
    "-framework UIKit"
)

set_source_files_properties(${SHADER_FILES} PROPERTIES
    MACOSX_PACKAGE_LOCATION Resources
)

# Shader compilation for macOS
add_custom_command(
    TARGET FinalStorm POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_BUNDLE_CONTENT_DIR:FinalStorm>/Resources
    COMMAND xcrun -sdk macosx metal -c ${SHADER_FILES} -o ${CMAKE_CURRENT_BINARY_DIR}/Shaders.air
    COMMAND xcrun -sdk macosx metallib ${CMAKE_CURRENT_BINARY_DIR}/Shaders.air -o ${CMAKE_CURRENT_BINARY_DIR}/default.metallib
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/default.metallib $<TARGET_BUNDLE_CONTENT_DIR:FinalStorm>/Resources/
    COMMENT "Compiling Metal shaders for macOS"
)

# Shader compilation for iOS
add_custom_command(
    TARGET FinalStorm-iOS POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_BUNDLE_CONTENT_DIR:FinalStorm-iOS>/Resources
    COMMAND xcrun -sdk iphoneos metal -c ${SHADER_FILES} -o ${CMAKE_CURRENT_BINARY_DIR}/Shaders-iOS.air
    COMMAND xcrun -sdk iphoneos metallib ${CMAKE_CURRENT_BINARY_DIR}/Shaders-iOS.air -o ${CMAKE_CURRENT_BINARY_DIR}/default-iOS.metallib
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/default-iOS.metallib $<TARGET_BUNDLE_CONTENT_DIR:FinalStorm-iOS>/Resources/default.metallib
    COMMENT "Compiling Metal shaders for iOS"
)

target_compile_definitions(FinalStorm PRIVATE SHADER_PATH="${CMAKE_CURRENT_SOURCE_DIR}/shaders")
target_compile_definitions(FinalStorm-iOS PRIVATE SHADER_PATH="${CMAKE_CURRENT_SOURCE_DIR}/shaders")

set_target_properties(FinalStorm PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/macOS/Info.plist
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.finalverse.finalstorm"
    MACOSX_BUNDLE_BUNDLE_NAME "FinalStorm"
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.finalverse.finalstorm"
    XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC "YES"
    XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++"
)

set_target_properties(FinalStorm-iOS PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/iOS/Info.plist
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.finalverse.finalstorm"
    MACOSX_BUNDLE_BUNDLE_NAME "FinalStorm"
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.finalverse.finalstorm"
    XCODE_ATTRIBUTE_PRODUCT_NAME "FinalStorm"
    XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "14.0"
    XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
    XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC "YES"
    XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++"
    XCODE_ATTRIBUTE_SDKROOT "iphoneos"
    XCODE_ATTRIBUTE_SUPPORTED_PLATFORMS "iphonesimulator iphoneos"
    XCODE_ATTRIBUTE_FRAMEWORK_SEARCH_PATHS "$(inherited)"
)
