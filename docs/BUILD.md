# Building FinalStorm

The project uses CMake to generate platform specific build files. On
macOS the provided scripts will create an Xcode project configured for
both macOS and iOS targets.

```bash
./scripts/build_setup.sh --open
```

This script generates a `build/` directory and optionally opens
`FinalStorm.xcodeproj` when the `--open` flag is supplied. By default the
project contains both macOS and iOS targets. You can restrict generation
using `--ios-only` or `--mac-only`. After configuring you can build either
from Xcode or via `cmake --build`.

