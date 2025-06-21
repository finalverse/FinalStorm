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
    src/Scene/CameraController.cpp
    src/World/Entity.cpp
    src/World/WorldManager.cpp
    src/Services/ServiceEntity.cpp
    src/Services/ServiceFactory.cpp
    src/Services/ServiceNode.cpp
    src/Services/ServiceVisualizations.cpp
    src/Environment/EnvironmentController.cpp
    src/UI/UI3DPanel.cpp
    src/UI/Panel.cpp
    src/UI/HolographicDisplay.cpp
    src/UI/InteractiveOrb.cpp
    src/UI/ServiceDiscoveryUI.cpp
    src/Core/Input/InteractionManager.cpp
    src/Network/FinalverseClient.cpp
    src/Network/NetworkClient.cpp
    src/Network/MessageProtocol.cpp
    src/Core/Audio/AudioEngine.cpp
    src/Core/Audio/SpatialAudioSystem.cpp
    src/Visual/DataVisualizer.cpp
    src/Services/Components/ParticleEmitter.cpp
    src/Services/Components/ConnectionBeam.cpp
    src/Services/Components/ConnectionBeamRendering.cpp
    src/Services/Components/BeamMesh.cpp
    src/Services/Components/EnergyRing.cpp
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

# Create both macOS and iOS targets by default
add_executable(FinalStorm-macOS MACOSX_BUNDLE
    ${CORE_SOURCES}
    ${METAL_SOURCES}
    ${MACOS_SOURCES}
)

add_executable(FinalStorm-iOS MACOSX_BUNDLE
    ${CORE_SOURCES}
    ${METAL_SOURCES}
    ${IOS_SOURCES}
)

# Common include directories for both targets
set(COMMON_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Common frameworks
set(COMMON_FRAMEWORKS
    "-framework Foundation"
    "-framework Metal"
    "-framework MetalKit"
    "-framework QuartzCore"
    "-framework CoreGraphics"
    Threads::Threads
)

# Common compile definitions
set(COMMON_COMPILE_DEFS
    USE_SIMD
)

# Configure macOS target
set_target_properties(FinalStorm-macOS PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.finalverse.finalstorm"
    MACOSX_BUNDLE_BUNDLE_NAME "FinalStorm"
    MACOSX_BUNDLE_BUNDLE_VERSION "0.1.0"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "0.1"
    MACOSX_DEPLOYMENT_TARGET "11.0"
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.finalverse.finalstorm.macos"
)

target_include_directories(FinalStorm-macOS PRIVATE ${COMMON_INCLUDE_DIRS})
target_link_libraries(FinalStorm-macOS PRIVATE
    ${COMMON_FRAMEWORKS}
    "-framework Cocoa"
    "-framework AppKit"
)
target_compile_definitions(FinalStorm-macOS PRIVATE ${COMMON_COMPILE_DEFS})

# Configure iOS target
set_target_properties(FinalStorm-iOS PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.finalverse.finalstorm"
    MACOSX_BUNDLE_BUNDLE_NAME "FinalStorm"
    MACOSX_BUNDLE_BUNDLE_VERSION "0.1.0"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "0.1"
    XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "14.0"
    XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.finalverse.finalstorm.ios"
    XCODE_ATTRIBUTE_DEVELOPMENT_TEAM ""
)

target_include_directories(FinalStorm-iOS PRIVATE ${COMMON_INCLUDE_DIRS})
target_link_libraries(FinalStorm-iOS PRIVATE
    ${COMMON_FRAMEWORKS}
    "-framework UIKit"
    "-framework ARKit"
)
target_compile_definitions(FinalStorm-iOS PRIVATE ${COMMON_COMPILE_DEFS})

# Copy resources for both targets
foreach(target FinalStorm-macOS FinalStorm-iOS)
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:${target}>/../Resources/assets
    )
    
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        $<TARGET_FILE_DIR:${target}>/../Resources/shaders
    )
endforeach()

# Installation
install(TARGETS FinalStorm-macOS FinalStorm-iOS
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)

install(DIRECTORY shaders DESTINATION bin)
install(DIRECTORY assets DESTINATION bin)