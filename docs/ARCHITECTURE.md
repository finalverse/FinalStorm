# Project Architecture

This document outlines the high level architecture of **FinalStorm**. The codebase is organised into several modules that work together to provide a portable client for the Finalverse network.

High Level Architecture
-----------------------

```
+------------------+
| macOS / iOS UI   |      (AppDelegate, GameViewController, ARMode)
+------------------+
          |
          v
+------------------+           +---------------------------+
| MetalRenderer    |<--------->| Renderer interface        |
| (platform code)  |           | src/Rendering/Renderer.h   |
+------------------+           +---------------------------+
          |
          v
+---------------------------------------------------------+
| Scene Graph & World Management                          |
|  - SceneNode hierarchy (src/Scene)                |
|  - WorldManager, SceneManager, Entity types             |
|  - ServiceEntity for representing backend services      |
|  - ServiceVisualizations for rich 3D visuals            |
+---------------------------------------------------------+
          |
          v
+---------------------------------------------------------+
| Core Systems                                            |
|  - Math utilities & Transform                           |
|  - Networking (FinalverseClient, MessageProtocol)       |
|  - AudioEngine + SpatialAudioSystem                     |
|  - EnvironmentController & DataVisualizer               |
|  - Input (InteractionManager)                           |
+---------------------------------------------------------+

## Core Modules

### Math
Located under `src/Core/Math`, this module offers vector and matrix math helpers along with a `Camera` class used by the renderer. Utility functions for transformations and SIMD types are provided in `Math.h` and `Transform.h`.

### World
`src/World` implements the entity system and world grid management. `WorldManager` keeps track of entities, updates them each frame and handles visibility. `SceneManager` creates service entities that are visualised in the scene.

### Networking
Networking code lives in `src/Core/Networking`. `FinalverseClient` manages the websocket connection to a Finalverse server and exchanges messages defined in `MessageProtocol`. Player position updates and world data are sent through this layer.

### Audio
The audio engine in `src/Core/Audio` provides loading and playback of sound clips with positional audio. `SpatialAudioSystem` ties service nodes to 3D audio sources so each running service can emit sound in the world.

### Rendering
A platform independent interface is defined in `src/Rendering`. The current implementation uses Metal and resides in `MetalRenderer`. It supports macOS and iOS with a `RendererFactory` prepared for future Vulkan or DirectX 12 backends.

### Scene Graph
Classes under `src/Scene` form a hierarchical scene graph. `SceneNode` is the base, while `ServiceNode` and `ServiceVisualization` specialise it for representing running services. Nodes can update each frame and issue draw calls through the renderer.
Individual visualization classes reside under `src/Services/Visual`.

### UI
`src/UI` contains components such as `HolographicDisplay`, `Panel` and `InteractiveOrb`. These provide simple 3D user interface elements used by service visualisations.

## Current Features

- **Networking:** WebSocket client with message serialization for connection and entity updates.
- **World system:** grid based `WorldManager` and `SceneManager` handling transforms and frustum culling.
- **Rendering:** Metal renderer with shaders, pipeline configuration and scene traversal.
- **Audio:** positional sources driven by `SpatialAudioSystem`.
- **UI & interaction:** 3D panels and interactive orbs with AR anchoring on iOS.
- **Environment & visualization:** service health metrics drive the environment state.

## Planned Extensions

- **Rich service visualizations** for web servers, databases, blockchain and AI services (defined in `ServiceVisualizations.h`).
- **AR placement** of services via `ARMode`.
- **Improved world updates** using `WorldManager::generateGridContent`.
- **Expanded networking messages** for terrain data and player actions.


## Proposed Repository Reorganization

The repository uses a `src/` layout:

```
FinalStorm/
  CMakeLists.txt
  src/
    Core/
    Scene/
    Rendering/
    UI/
    Platform/macOS/
    Platform/iOS/
    main.cpp         -> shared entry from macOS/main.mm and iOS/main.mm

A top level `FinalStormApp.cpp` would construct the renderer and drive the
platform layer. Common Metal shaders could live in `shaders/FinalStorm.metal`.

Current project structure:
```
FinalStorm/
├── CMakeLists.txt
├── README.md
├── LICENSE
├── .gitignore
├── setup_xcode_project.sh
├── generate_xcodeproj.sh
├── verify_structure.sh
├── docs/                           # Documentation
│   ├── ARCHITECTURE.md
│   ├── API.md
│   ├── BUILD.md
│   └── CONTRIBUTING.md
├── include/FinalStorm/             # Public headers (for library usage)
│   ├── Core/
│   │   ├── Math.h                 # Public math utilities
│   │   └── Transform.h            # Public transform interface
│   ├── Networking/
│   │   └── FinalverseClient.h     # Public client interface
│   └── FinalStorm.h               # Main public header
├── src/                            # Implementation files
│   ├── main.cpp                   # Main entry point (creates FinalStormApp)
│   ├── FinalStormApp.cpp          # Application class implementation
│   ├── FinalStormApp.h            # Application class header
│   │
│   ├── Core/                      # Core systems
│   │   ├── Audio/
│   │   │   ├── AudioEngine.cpp
│   │   │   ├── AudioEngine.h
│   │   │   ├── SpatialAudioSystem.cpp
│   │   │   └── SpatialAudioSystem.h
│   │   │
│   │   ├── Input/
│   │   │   ├── InteractionManager.cpp
│   │   │   ├── InteractionManager.h
│   │   │   ├── InputTypes.h
│   │   │   └── GestureRecognizer.h
│   │   │
│   │   ├── Math/
│   │   │   ├── Math.cpp
│   │   │   ├── Math.h            # Internal math header
│   │   │   ├── Transform.cpp
│   │   │   └── Transform.h       # Internal transform header
│   │   │
│   │   ├── Networking/
│   │   │   ├── FinalverseClient.cpp
│   │   │   ├── FinalverseClient.h  # Internal client header
│   │   │   ├── MessageProtocol.cpp
│   │   │   ├── MessageProtocol.h
│   │   │   ├── ServiceDiscovery.cpp
│   │   │   └── ServiceDiscovery.h
│   │   │
│   │   └── World/
│   │       ├── Entity.cpp
│   │       ├── Entity.h
│   │       ├── WorldManager.cpp
│   │       └── WorldManager.h
│   │
│   ├── Scene/                     # Scene management
│   │   ├── SceneManager.cpp       # Main scene manager
│   │   ├── SceneManager.h
│   │   ├── SceneNode.cpp          # Base scene node
│   │   ├── SceneNode.h
│   │   ├── Scene.cpp              # Scene implementation
│   │   ├── Scene.h
│   │   ├── CameraController.cpp
│   │   └── CameraController.h
│   │
│   ├── Services/                  # Service visualizations
│   │   ├── ServiceEntity.cpp      # Base service entity
│   │   ├── ServiceEntity.h
│   │   ├── ServiceFactory.cpp     # Factory for creating services
│   │   ├── ServiceFactory.h
│   │   ├── ServiceMetrics.h       # Metrics structures
│   │   │
│   │   ├── Visual/                # Specific service visualizations
│   │   │   ├── APIGatewayViz.cpp
│   │   │   ├── APIGatewayViz.h
│   │   │   ├── AIServiceViz.cpp
│   │   │   ├── AIServiceViz.h
│   │   │   ├── AudioServiceViz.cpp
│   │   │   ├── AudioServiceViz.h
│   │   │   ├── WorldEngineViz.cpp
│   │   │   ├── WorldEngineViz.h
│   │   │   ├── DatabaseViz.cpp
│   │   │   ├── DatabaseViz.h
│   │   │   ├── CommunityViz.cpp
│   │   │   └── CommunityViz.h
│   │   │
│   │   └── Components/            # Service components
│   │       ├── ParticleEmitter.cpp
│   │       ├── ParticleEmitter.h
│   │       ├── ConnectionBeam.cpp
│   │       └── ConnectionBeam.h
│   │
│   ├── Environment/               # Environment systems
│   │   ├── EnvironmentController.cpp
│   │   ├── EnvironmentController.h
│   │   ├── Skybox.cpp
│   │   ├── Skybox.h
│   │   ├── GridMesh.cpp
│   │   └── GridMesh.h
│   │
│   ├── UI/                        # User interface
│   │   ├── UI3DPanel.cpp          # Base 3D panel
│   │   ├── UI3DPanel.h
│   │   ├── HolographicDisplay.cpp
│   │   ├── HolographicDisplay.h
│   │   ├── InteractiveOrb.cpp
│   │   ├── InteractiveOrb.h
│   │   ├── ServiceDiscoveryUI.cpp
│   │   ├── ServiceDiscoveryUI.h
│   │   ├── MetricsDisplay.cpp
│   │   └── MetricsDisplay.h
│   │
│   ├── Rendering/                 # Rendering system
│   │   ├── Renderer.h             # Abstract renderer interface
│   │   ├── RenderContext.h
│   │   ├── Material.h
│   │   ├── Mesh.h
│   │   │
│   │   └── Metal/                 # Metal implementation
│   │       ├── MetalRenderer.h
│   │       ├── MetalRenderer.mm
│   │       ├── MetalRenderContext.h
│   │       ├── MetalRenderContext.mm
│   │       ├── MetalMesh.h
│   │       ├── MetalMesh.mm
│   │       ├── MetalTexture.h
│   │       ├── MetalTexture.mm
│   │       └── MetalShaderTypes.h
│   │
│   ├── Visualization/             # Data visualization
│   │   ├── DataVisualizer.cpp
│   │   ├── DataVisualizer.h
│   │   ├── GraphRenderer.cpp
│   │   ├── GraphRenderer.h
│   │   ├── WaveformVisualizer.cpp
│   │   └── WaveformVisualizer.h
│   │
│   └── Platform/                  # Platform-specific code
│       ├── iOS/
│       │   ├── AppDelegate.h
│       │   ├── AppDelegate.mm
│       │   ├── GameViewController.h
│       │   ├── GameViewController.mm
│       │   ├── ARMode.h
│       │   ├── ARMode.mm
│       │   ├── Info.plist
│       │   └── main.mm
│       │
│       └── macOS/
│           ├── AppDelegate.h
│           ├── AppDelegate.mm
│           ├── GameViewController.h
│           ├── GameViewController.mm
│           ├── Info.plist
│           ├── FinalStorm.entitlements
│           └── main.mm
├── shaders/                       # Shader files
│   ├── Common.metal              # Common shader functions
│   ├── Service.metal             # Service visualization shaders
│   ├── Environment.metal         # Environment shaders
│   ├── UI.metal                  # UI shaders
│   ├── Particles.metal           # Particle shaders
│   └── PostProcess.metal         # Post-processing shaders
├── assets/                        # Asset files
│   ├── textures/
│   │   ├── skybox/
│   │   ├── particles/
│   │   └── ui/
│   ├── models/
│   └── audio/
├── tests/                         # Unit tests
│   ├── Core/
│   ├── Scene/
│   └── Services/
└── external/                      # Third-party dependencies
    └── glm/                      # If not using system GLM
```
