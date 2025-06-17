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
| (platform code)  |           | Shared/Renderer/Renderer.h|
+------------------+           +---------------------------+
          |
          v
+---------------------------------------------------------+
| Scene Graph & World Management                          |
|  - SceneNode hierarchy (Shared/SceneGraph)              |
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
          |
          v
+------------------+             +------------------+
| Resources        |             | Build scripts    |
| (Assets, shaders)|             | CMake, setup*.sh |
+------------------+             +------------------+
```

## Core Modules

### Math
Located under `Shared/Core/Math`, this module offers vector and matrix math helpers along with a `Camera` class used by the renderer. Utility functions for transformations and SIMD types are provided in `Math.h` and `Transform.h`.

### World
`Shared/Core/World` implements the entity system and world grid management. `WorldManager` keeps track of entities, updates them each frame and handles visibility. `SceneManager` creates service entities that are visualised in the scene.

### Networking
Networking code lives in `Shared/Core/Networking`. `FinalverseClient` manages the websocket connection to a Finalverse server and exchanges messages defined in `MessageProtocol`. Player position updates and world data are sent through this layer.

### Audio
The audio engine in `Shared/Core/Audio` provides loading and playback of sound clips with positional audio. `SpatialAudioSystem` ties service nodes to 3D audio sources so each running service can emit sound in the world.

### Rendering
A platform independent interface is defined in `Shared/Renderer`. The current implementation uses Metal and resides in `MetalRenderer`. It supports macOS and iOS with a `RendererFactory` prepared for future Vulkan or DirectX 12 backends.

### Scene Graph
Classes under `Shared/SceneGraph` form a hierarchical scene graph. `SceneNode` is the base, while `ServiceNode` and `ServiceVisualization` specialise it for representing running services. Nodes can update each frame and issue draw calls through the renderer.

### UI
`Shared/UI3D` contains components such as `HolographicDisplay`, `Panel` and `InteractiveOrb`. These provide simple 3D user interface elements used by service visualisations.

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

To simplify cross platform builds the repository may be reorganized under a `src/`
layout:

```
FinalStorm/
  CMakeLists.txt
  src/
    Core/            -> from Shared/Core/
    Scene/           -> from Shared/SceneGraph/
    Rendering/       -> from Shared/Renderer/ plus MetalRenderer/*
    UI/              -> from Shared/UI3D/
    Platform/macOS/  -> from macOS/
    Platform/iOS/    -> from iOS/
    main.cpp         -> shared entry from macOS/main.mm and iOS/main.mm
  resources/         -> from Resources/
  scripts/           -> generate_xcodeproj.sh, setup_xcode_project.sh, verify_structure.sh
```

A top level `FinalStormApp.cpp` would construct the renderer and drive the
platform layer. Common Metal shaders could live in `shaders/FinalStorm.metal`.
