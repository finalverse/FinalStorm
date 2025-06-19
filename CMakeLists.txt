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

# Source files
set(SHARED_SOURCES
    src/main.cpp
    src/FinalStormApp.cpp
    src/Scene/Scenes/FirstScene.cpp
    src/Scene/SceneLoader.cpp
    
    # Core
    src/Core/Math/Math.cpp
    src/World/Entity.cpp
    src/World/WorldManager.cpp
    src/Network/FinalverseClient.cpp
    src/Network/MessageProtocol.cpp
    src/Network/NetworkClient.cpp
    src/Core/Audio/AudioEngine.cpp
    src/Core/Audio/SpatialAudioSystem.cpp
    src/Core/Input/InteractionManager.cpp
    src/Visual/DataVisualizer.cpp
    
    # Scene
    src/Scene/SceneNode.cpp
    src/Scene/SceneManager.cpp
    src/Scene/CameraController.cpp
    src/Scene/Scene.cpp
    
    # Services
    src/Services/ServiceEntity.cpp
    src/Services/ServiceFactory.cpp
    src/Services/ServiceNode.cpp
    src/Services/ServiceVisualizations.cpp
    src/Services/Visual/DatabaseViz.cpp
    src/Services/Visual/WebServerViz.cpp
    src/Services/Visual/AIServiceViz.cpp
    src/Services/Visual/AudioServiceViz.cpp
    src/Services/Visual/WorldEngineViz.cpp
    src/Services/Visual/CommunityViz.cpp
    src/Services/Visual/APIGatewayViz.cpp
    src/Services/Components/ParticleEmitter.cpp
    
    # UI
    src/UI/Panel.cpp
    src/UI/HolographicDisplay.cpp
    src/UI/InteractiveOrb.cpp
    src/UI/UI3DPanel.cpp
    
    # Environment
    src/Environment/EnvironmentController.cpp
)

set(METAL_RENDERER_SOURCES
    src/Rendering/Metal/MetalRenderer.mm
)

# Metal shaders
set(METAL_SHADERS
    shaders/Common.metal
    shaders/Core.metal
    shaders/Environment.metal
    shaders/FirstScene.metal
    shaders/Particles.metal
    shaders/PostProcess.metal
    shaders/Service.metal
    shaders/UI.metal
)

# Platform specific sources
set(MACOS_SOURCES
    src/Platform/macOS/main.mm
    src/Platform/macOS/AppDelegate.mm
    src/Platform/macOS/GameViewController.mm
)

set(IOS_SOURCES
    src/Platform/iOS/main.mm
    src/Platform/iOS/AppDelegate.mm
    src/Platform/iOS/GameViewController.mm
    src/Platform/iOS/ARMode.mm
)

# Placeholder sets for additional platforms
set(LINUX_SOURCES)
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    list(APPEND LINUX_SOURCES
        # src/Platform/Linux/main.cpp
    )
endif()

set(ANDROID_SOURCES)
if(ANDROID)
    list(APPEND ANDROID_SOURCES
        # src/Platform/Android/main.cpp
    )
endif()

set(WINDOWS_SOURCES)
if(WIN32 AND NOT APPLE)
    list(APPEND WINDOWS_SOURCES
        # src/Platform/Windows/main.cpp
    )
endif()

# Set the shader files as resources
set_source_files_properties(${METAL_SHADERS} PROPERTIES
    MACOSX_PACKAGE_LOCATION Resources/Shaders
)

# macOS target
add_executable(FinalStorm MACOSX_BUNDLE
    ${SHARED_SOURCES}
    ${METAL_RENDERER_SOURCES}
    ${MACOS_SOURCES}
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
    XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++17"
)

target_include_directories(FinalStorm 
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(FinalStorm
    ${METAL_FRAMEWORK}
    ${METALKIT_FRAMEWORK}
    ${QUARTZCORE_FRAMEWORK}
    ${COREFOUNDATION_FRAMEWORK}
    ${FOUNDATION_FRAMEWORK}
    ${COCOA_FRAMEWORK}
    ${APPKIT_FRAMEWORK}
)

# iOS target
add_executable(FinalStorm-iOS MACOSX_BUNDLE
    ${SHARED_SOURCES}
    ${METAL_RENDERER_SOURCES}
    ${IOS_SOURCES}
    ${METAL_SHADERS}
)

# Add Resources to iOS bundle
set_source_files_properties(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/iOS/Resources/Assets.xcassets
    PROPERTIES MACOSX_PACKAGE_LOCATION Resources
)

target_sources(FinalStorm-iOS PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/iOS/Resources/Assets.xcassets
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
    XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++17"
    XCODE_ATTRIBUTE_SDKROOT "iphoneos"
    XCODE_ATTRIBUTE_SUPPORTED_PLATFORMS "iphonesimulator iphoneos"
    XCODE_ATTRIBUTE_FRAMEWORK_SEARCH_PATHS "$(inherited)"
    XCODE_ATTRIBUTE_ENABLE_BITCODE "NO"
)

target_include_directories(FinalStorm-iOS 
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}
)

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

# Shader compilation target
add_custom_target(CompileShaders ALL
    COMMENT "Compiling Metal shaders..."
)

# for Metal shader compilation
#   - Usage in CMakeLists.txt:
#   - compile_metal_shaders(FinalStorm "${CMAKE_SOURCE_DIR}/shaders" "${CMAKE_BINARY_DIR}/shaders")
#
# Metal shader compilation function
function(compile_metal_shaders target_name shader_dir output_dir)
    if(APPLE)
        file(GLOB METAL_SHADERS "${shader_dir}/*.metal")
        
        set(COMPILED_SHADERS)
        
        foreach(SHADER ${METAL_SHADERS})
            get_filename_component(SHADER_NAME ${SHADER} NAME_WE)
            set(OUTPUT_AIR "${output_dir}/${SHADER_NAME}.air")
            
            # Determine SDK based on platform
            if(IOS)
                set(SDK_FLAG "-sdk" "iphoneos")
            else()
                set(SDK_FLAG "-sdk" "macosx")
            endif()
            
            # Compile to AIR
            add_custom_command(
                OUTPUT ${OUTPUT_AIR}
                COMMAND xcrun ${SDK_FLAG} metal -c ${SHADER} -o ${OUTPUT_AIR}
                DEPENDS ${SHADER}
                COMMENT "Compiling Metal shader ${SHADER_NAME}"
                VERBATIM
            )
            
            list(APPEND COMPILED_SHADERS ${OUTPUT_AIR})
        endforeach()
        
        # Create unified metallib
        set(METALLIB_OUTPUT "${output_dir}/FinalStorm.metallib")
        add_custom_command(
            OUTPUT ${METALLIB_OUTPUT}
            COMMAND xcrun ${SDK_FLAG} metallib ${COMPILED_SHADERS} -o ${METALLIB_OUTPUT}
            DEPENDS ${COMPILED_SHADERS}
            COMMENT "Creating unified Metal library"
            VERBATIM
        )
        
        # Add metallib to target
        add_custom_target(${target_name}_shaders ALL DEPENDS ${METALLIB_OUTPUT})
        add_dependencies(${target_name} ${target_name}_shaders)
        
        # Copy metallib to Resources
        if(IOS)
            set(RESOURCES_DIR "${CMAKE_CURRENT_BINARY_DIR}/${target_name}.app")
        else()
            set(RESOURCES_DIR "${CMAKE_CURRENT_BINARY_DIR}/${target_name}.app/Contents/Resources")
        endif()
        
        add_custom_command(
            TARGET ${target_name}_shaders POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory ${RESOURCES_DIR}/Shaders
            COMMAND ${CMAKE_COMMAND} -E copy ${METALLIB_OUTPUT} ${RESOURCES_DIR}/Shaders/
        )
    endif()
endfunction()

# macOS shader compilation
add_custom_command(
    TARGET CompileShaders
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/CompiledShaders
    COMMAND xcrun -sdk macosx metal -c ${CMAKE_CURRENT_SOURCE_DIR}/shaders/Common.metal -o ${CMAKE_CURRENT_BINARY_DIR}/CompiledShaders/Common.air
    COMMAND xcrun -sdk macosx metal -c ${CMAKE_CURRENT_SOURCE_DIR}/shaders/Core.metal -o ${CMAKE_CURRENT_BINARY_DIR}/CompiledShaders/Core.air
    COMMAND xcrun -sdk macosx metal -c ${CMAKE_CURRENT_SOURCE_DIR}/shaders/FirstScene.metal -o ${CMAKE_CURRENT_BINARY_DIR}/CompiledShaders/FirstScene.air
    COMMAND xcrun -sdk macosx metallib ${CMAKE_CURRENT_BINARY_DIR}/CompiledShaders/*.air -o ${CMAKE_CURRENT_BINARY_DIR}/default.metallib
    COMMENT "Compiling Metal shaders for macOS"
)

# iOS shader compilation
add_custom_command(
    TARGET CompileShaders
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/CompiledShaders-iOS
    COMMAND xcrun -sdk iphoneos metal -c ${CMAKE_CURRENT_SOURCE_DIR}/shaders/Common.metal -o ${CMAKE_CURRENT_BINARY_DIR}/CompiledShaders-iOS/Common.air || true
    COMMAND xcrun -sdk iphoneos metal -c ${CMAKE_CURRENT_SOURCE_DIR}/shaders/Core.metal -o ${CMAKE_CURRENT_BINARY_DIR}/CompiledShaders-iOS/Core.air || true
    COMMAND xcrun -sdk iphoneos metal -c ${CMAKE_CURRENT_SOURCE_DIR}/shaders/FirstScene.metal -o ${CMAKE_CURRENT_BINARY_DIR}/CompiledShaders-iOS/FirstScene.air || true
    COMMAND xcrun -sdk iphoneos metallib ${CMAKE_CURRENT_BINARY_DIR}/CompiledShaders-iOS/*.air -o ${CMAKE_CURRENT_BINARY_DIR}/default-iOS.metallib || true
    COMMENT "Compiling Metal shaders for iOS"
)

# Make sure apps depend on shader compilation
add_dependencies(FinalStorm CompileShaders)
add_dependencies(FinalStorm-iOS CompileShaders)

# Copy compiled shader library to app bundles
add_custom_command(
    TARGET FinalStorm POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_BINARY_DIR}/default.metallib
        $<TARGET_BUNDLE_CONTENT_DIR:FinalStorm>/Resources/default.metallib
    COMMENT "Copying shader library to macOS app bundle"
)

add_custom_command(
    TARGET FinalStorm-iOS POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_BINARY_DIR}/default-iOS.metallib
        $<TARGET_BUNDLE_CONTENT_DIR:FinalStorm-iOS>/Resources/default.metallib || true
    COMMENT "Copying shader library to iOS app bundle"
)

# Group source files in Xcode
source_group("Source Files" FILES ${SHARED_SOURCES})
source_group("Source Files\\Platform\\macOS" FILES ${MACOS_SOURCES})
source_group("Source Files\\Platform\\iOS" FILES ${IOS_SOURCES})
source_group("Source Files\\Platform\\Linux" FILES ${LINUX_SOURCES})
source_group("Source Files\\Platform\\Android" FILES ${ANDROID_SOURCES})
source_group("Source Files\\Platform\\Windows" FILES ${WINDOWS_SOURCES})
source_group("Source Files\\Rendering\\Metal" FILES ${METAL_RENDERER_SOURCES})
source_group("Shaders" FILES ${METAL_SHADERS})
source_group("Resources" FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/Platform/iOS/Resources/Assets.xcassets)
