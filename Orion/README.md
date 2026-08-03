# Orion

Virtual and global filesystem framework for modern C++ applications and games.

Orion is part of the Rhea ecosystem and provides a flexible abstraction layer for working with filesystems, mounted resources, archives, and virtualized file access.

---

## Features

- Modern C++17 architecture
- Virtual filesystem abstraction
- Mountable filesystem structure
- Cross-platform oriented design
- Stream-based file access
- Integration-focused API design
- Designed for games and tooling workflows

---

## Status

> ⚠️ Orion is currently experimental and under active development.
>
> APIs may change frequently.
> Documentation and examples are still evolving.

---

## Architecture

Orion is designed as a modular filesystem layer that can unify access to:

- Native filesystem paths
- Mounted directories
- Archives and packaged assets
- Virtual resource paths
- Engine and application resource systems

Orion is intended to integrate closely with the wider Rhea ecosystem while remaining independently usable.

---

## Building

Clone the repository:

```bash
git clone https://github.com/rhea-engine/rhea.git
cd rhea
```

Configure and build:

```bash
cmake -S . -B build
cmake --build build
```

Build samples:

```bash
cmake -S . -B build -DORION_BUILD_SAMPLES=ON
cmake --build build
```

---

## Using with FetchContent

```cmake
include(FetchContent)

FetchContent_Declare(
    Rhea
    GIT_REPOSITORY https://github.com/rhea-engine/rhea.git
    GIT_TAG v0.1.0
)

FetchContent_MakeAvailable(Rhea)

target_link_libraries(MyGame PRIVATE Meworla::Orion)
```

---

## Example

```cpp
#include <Orion/Orion.h>

int main()
{
    return 0;
}
```

---

## Philosophy

Orion focuses on:

* Unified resource access
* Clear abstraction boundaries
* Flexible integration into applications and engines
* Modern C++ development practices
* Standalone usability without requiring custom tooling

The framework is designed to work with plain CMake while also supporting additional tooling such as `mwpkg`.

---

## Roadmap

Planned improvements include:

* Archive support
* Package mounting
* Asset registry integration
* Streaming improvements
* Async filesystem operations
* Extended tooling support

---

## License

MIT License
