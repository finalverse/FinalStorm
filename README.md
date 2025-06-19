# FinalStorm

FinalStorm is an experimental client for connecting to the Finalverse network and
other compatible virtual world platforms. It is built with C++17 and Metal to
run on macOS and iOS.

For an overview of the code layout see [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md).

## Features

- Basic scene and entity management
- Networking layer for Finalverse servers
- Audio playback through a simple engine
- Metal based renderer with configurable shaders

## Requirements

- macOS 11 or later / Xcode with command line tools
- CMake 3.20 or later

## Building

To generate the Xcode project run:

```bash
./setup_xcode_project.sh
```

This script creates a `build` directory and produces `FinalStorm.xcodeproj`.
Open the project in Xcode to build and run the client.


When opening the generated project, ensure the **FinalStorm-iOS** target
uses the iOS SDK. In Xcode's Build Settings, the **Base SDK** should be
either *iPhoneOS* or *iPhoneSimulator*. If you see build errors such as
`CVOpenGLESTexture.h` not found, the macOS SDK is selected by mistake.
Adjust the SDK and verify the framework search paths do not reference
`MacOSX.platform`.

For a bare CMake invocation you can also run:

```bash
./generate_xcodeproj.sh
```

See [docs/BUILD.md](docs/BUILD.md) for additional build information.

## Verifying the repository

A helper script `verify_structure.sh` checks that all expected files are
present. Run it from the repository root:

```bash
./verify_structure.sh
```

## License

This project is provided for experimentation and is not yet production ready.

