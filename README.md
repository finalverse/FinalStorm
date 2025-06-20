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
./scripts/build_setup.sh --clean --open
```

This script creates a `build` directory and produces `FinalStorm.xcodeproj` with
both macOS and iOS targets. The `--open` flag launches Xcode once generation
completes. Use `--ios-only` or `--mac-only` if you want a single platform.


The project also includes a **Shaders** target which compiles all Metal shaders
to verify they are valid for both platforms.

See [docs/BUILD.md](docs/BUILD.md) for additional build information.

## Verifying the repository

A helper script `check_structure.sh` checks that all expected files are
present. Run it from the repository root:

```bash
./scripts/check_structure.sh
```

## License

This project is provided for experimentation and is not yet production ready.


