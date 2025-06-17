cmake_minimum_required(VERSION 3.20)
project(FinalStorm)

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

# Scene sources
set(SCENE_SOURCES
    src/Scene/Scene.cpp
    src/Scene/SceneNode.cpp
    src/Scene/SceneManager.cpp
    src/Scene/CameraController.cpp
)

# Service sources
set(SERVICE_SOURCES
    src/Services/ServiceEntity.cpp
    src/Services/ServiceFactory.cpp
    src/Services/ServiceMetricsUpdater.cpp
    src/Services/MetricsMonitor.cpp
    src/Services/ServiceLayoutUtils.cpp
)

# Service visualization sources
set(SERVICE_VIZ_SOURCES
    src/Services/Visualizations/APIGatewayViz.cpp
    src/Services/Visualizations/AIServiceViz.cpp
    src/Services/Visualizations/AudioServiceViz.cpp
    src/Services/Visualizations/WorldEngineViz.cpp
    src/Services/Visualizations/DatabaseViz.cpp
    src/Services/Visualizations/BlockchainViz.cpp
    src/Services/Visualizations/CommunityViz.cpp
)

# Service component sources
set(SERVICE_COMPONENT_SOURCES
    src/Services/Components/ParticleEmitter.cpp
    src/Services/Components/ConnectionBeam.cpp
)

# UI sources
set(UI_SOURCES
    src/UI/UI3DPanel.cpp
    src/UI/HolographicDisplay.cpp
    src/UI/InteractiveOrb.cpp
    src/UI/ServiceDiscoveryUI.cpp
    src/UI/MetricsDisplay.cpp
)

# Source files
set(SHARED_SOURCES
    src/Core/Math/Math.cpp
    src/Core/World/Entity.cpp
    src/Core/World/WorldManager.cpp
    src/Core/Networking/FinalverseClient.cpp
    src/Core/Networking/MessageProtocol.cpp
    src/Core/Audio/AudioEngine.cpp
    src/Core/Audio/SpatialAudioSystem.cpp
    src/Core/Input/InteractionManager.cpp
    src/Scene/SceneNode.cpp
    src/Scene/ServiceNode.cpp
    src/Scene/ServiceVisualizations.cpp
    src/Core/Services/WebServerViz.cpp
    src/Core/Services/DatabaseViz.cpp
    src/Core/Services/ServiceEntity.cpp
    src/Core/Environment/EnvironmentController.cpp
    src/Core/Visualization/DataVisualizer.cpp
    src/Core/World/SceneManager.cpp
    src/UI/Panel.cpp
    src/UI/HolographicDisplay.cpp
    src/UI/InteractiveOrb.cpp
)

set(METAL_RENDERER_SOURCES
    src/Rendering/Metal/MetalRenderer.mm
)

# Metal shaders
set(METAL_SHADERS
    src/Rendering/Metal/Shaders.metal
)

# Set the shader files as resources
set_source_files_properties(${METAL_SHADERS} PROPERTIES
    MACOSX_PACKAGE_LOCATION Resources
)

# Always create macOS target
add_executable(FinalStorm MACOSX_BUNDLE
    ${SHARED_SOURCES}
    ${METAL_RENDERER_SOURCES}
    src/Platform/macOS/main.mm
    src/Platform/macOS/AppDelegate.mm
    src/Platform/macOS/GameViewController.mm
    ${METAL_SHADERS}
)

set_target_properties(FinalStorm PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/macOS/Info.plist
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.finalverse.finalstorm"
    MACOSX_BUNDLE_BUNDLE_NAME "FinalStorm"
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.finalverse.finalstorm"
    XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC "YES"
    XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++"
)

target_include_directories(FinalStorm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(FinalStorm
    ${METAL_FRAMEWORK}
    ${METALKIT_FRAMEWORK}
    ${QUARTZCORE_FRAMEWORK}
    ${COREFOUNDATION_FRAMEWORK}
    ${FOUNDATION_FRAMEWORK}
    ${COCOA_FRAMEWORK}
    ${APPKIT_FRAMEWORK}
)

# Shader compilation for macOS
add_custom_command(
    TARGET FinalStorm POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_BUNDLE_CONTENT_DIR:FinalStorm>/Resources
    COMMAND xcrun -sdk macosx metal -c ${CMAKE_CURRENT_SOURCE_DIR}/src/Rendering/Metal/Shaders.metal -o ${CMAKE_CURRENT_BINARY_DIR}/Shaders.air
    COMMAND xcrun -sdk macosx metallib ${CMAKE_CURRENT_BINARY_DIR}/Shaders.air -o ${CMAKE_CURRENT_BINARY_DIR}/default.metallib
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/default.metallib $<TARGET_BUNDLE_CONTENT_DIR:FinalStorm>/Resources/
    COMMENT "Compiling Metal shaders for macOS"
)

# Always create iOS target (it will show up in Xcode even if not buildable on macOS)
add_executable(FinalStorm-iOS MACOSX_BUNDLE
    ${SHARED_SOURCES}
    ${METAL_RENDERER_SOURCES}
    src/Platform/iOS/main.mm
    src/Platform/iOS/AppDelegate.mm
    src/Platform/iOS/GameViewController.mm
    ${METAL_SHADERS}
)

# Add after the iOS target creation
if(TARGET FinalStorm-iOS)
    # Copy shader source as a fallback
    add_custom_command(
        TARGET FinalStorm-iOS POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/src/Rendering/Metal/Shaders.metal
            $<TARGET_BUNDLE_CONTENT_DIR:FinalStorm-iOS>/Resources/Shaders.metal
        COMMENT "Copying shader source for iOS"
    )
endif()

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

target_include_directories(FinalStorm-iOS PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Link iOS frameworks explicitly
target_link_libraries(FinalStorm-iOS
    "-framework Metal"
    "-framework MetalKit"
    "-framework QuartzCore"
    "-framework CoreFoundation"
    "-framework Foundation"
    "-framework CoreGraphics"
    "-framework UIKit"
)

# Shader compilation for iOS
add_custom_command(
    TARGET FinalStorm-iOS POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_BUNDLE_CONTENT_DIR:FinalStorm-iOS>/Resources
    COMMAND xcrun -sdk iphoneos metal -c ${CMAKE_CURRENT_SOURCE_DIR}/src/Rendering/Metal/Shaders.metal -o ${CMAKE_CURRENT_BINARY_DIR}/Shaders-iOS.air
    COMMAND xcrun -sdk iphoneos metallib ${CMAKE_CURRENT_BINARY_DIR}/Shaders-iOS.air -o ${CMAKE_CURRENT_BINARY_DIR}/default-iOS.metallib
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/default-iOS.metallib $<TARGET_BUNDLE_CONTENT_DIR:FinalStorm-iOS>/Resources/default.metallib
    COMMENT "Compiling Metal shaders for iOS"
)

