# Building FinalStorm

The project uses CMake to generate platform specific build files. On
macOS the provided scripts will create an Xcode project configured for
both macOS and iOS targets.

```bash
./setup_xcode_project.sh
```

This script generates a `build/` directory and opens `FinalStorm.xcodeproj`.
For manual CMake invocation run:

```bash
./generate_xcodeproj.sh
```

After configuring you can build either from Xcode or via `cmake --build`.
