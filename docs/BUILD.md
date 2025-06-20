# Building FinalStorm

The project uses CMake to generate platform specific build files. On
macOS the provided scripts will create an Xcode project configured for
both macOS and iOS targets.

```bash
./scripts/build_setup.sh --open
```

This script generates a `build/` directory and optionally opens
`FinalStorm.xcodeproj` when the `--open` flag is supplied. Pass `--ios`
to configure for the iOS SDK. After configuring you can build either
from Xcode or via `cmake --build`.
