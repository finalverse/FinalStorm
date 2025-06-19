# Public API Overview

This document lists the main header files intended for consumption by
applications embedding **FinalStorm**. The API is still experimental
and may change frequently.

## Headers

- `include/FinalStorm/FinalStorm.h` - aggregates the main components
  required to bootstrap the engine.
- `include/FinalStorm/Core/Math.h` and `include/FinalStorm/Core/Transform.h` -
  math helpers used by most subsystems.
- `include/FinalStorm/Networking/FinalverseClient.h` - simple client for
  connecting to Finalverse servers.

Additional headers under `include/FinalStorm` may become stable over time.
