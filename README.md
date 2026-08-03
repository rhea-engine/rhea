# Rhea SDK

Rhea is an experimental C++17 SDK for building games and interactive applications.

The repository contains the Rhea runtime framework together with its foundational libraries, rendering infrastructure, filesystem abstraction, optional runtime modules, and development tools. These components are maintained and versioned together but remain separate CMake targets.

> Rhea is under active development. APIs, file formats, and module boundaries may change between versions.

## Components

| Component | Purpose |
|---|---|
| Rhea | Application framework, scenes, components, services, and Fabrics |
| Components | Shared math, parsing, and utility libraries |
| Psio | Platform, networking, and system abstractions |
| Orion | Virtual and mounted filesystem support |
| Poseidon | Rendering framework |
| Hippios | Platform-specific rendering backends |
| RheaClient | Client and graphics integration |
| RheaAudio | Optional audio support |
| RheaPhysics | Physics abstraction |
| RheaPhysicsBullet | Optional Bullet Physics implementation |

More detailed documentation is available in the component directories:

- [Rhea](Rhea/README.md)
- [Components](Components/README.md)
- [Orion](Orion/README.md)
- [Poseidon](Poseidon/README.md)

## Requirements

- CMake 3.24 or newer
- A C++17 compiler
- Python 3 for generated registries
- Platform-specific graphics and system development libraries

Optional dependencies include SDL3, Bullet Physics, and OpenSSL. Depending on the selected configuration, dependencies can be supplied by the host system, vcpkg, or Rhea's FetchContent fallbacks.

## Building the SDK

Configure and build with CMake:

```sh
cmake -S . -B out/build -DRHEA_BUILD_TESTS=OFF
cmake --build out/build
```

On Windows, the included presets can also be used:

```sh
cmake --preset x64-debug
cmake --build out/build/x64-debug
```

## Using Rhea with add_subdirectory

Place Rhea in your project, for example as a Git submodule, filesystem link, or local checkout:

```text
MyGame/
├─ external/
│  └─ rhea/
├─ src/
└─ CMakeLists.txt
```

Then add it from your game's `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.24)
project(MyGame LANGUAGES C CXX)

set(RHEA_SDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/rhea")

include("${RHEA_SDK_DIR}/Rhea/cmake/rhea_project.cmake")

add_subdirectory("${RHEA_SDK_DIR}")

add_executable(MyGame
    src/main.cpp
)

target_link_libraries(MyGame PRIVATE
    Rhea::Core
    Rhea::Client
    RheaFabrics
)

rhea_project_end()
```

Additional packages that register Fabrics must be added before `rhea_project_end()`.

## Selecting Fabrics

A project may provide an `enabled.json` file and select it before including Rhea:

```cmake
set(
    RHEA_ENABLED_PROFILE_JSON
    "${CMAKE_CURRENT_SOURCE_DIR}/enabled.json"
)
```

If no enabled profile is specified, all registered Fabrics are enabled.

## Useful CMake options

| Option | Description |
|---|---|
| `RHEA_BUILD_TESTS` | Build SDK tests |
| `RHEA_BUILD_SAMPLES` | Build Rhea samples |
| `POSEIDON_BUILD_SAMPLES` | Build Poseidon samples |
| `RHEA_BUILD_AUDIO` | Build audio support |
| `RHEA_BUILD_CLIENT` | Build client support |
| `RHEA_BUILD_PHYSICS` | Build the physics abstraction |
| `RHEA_BUILD_BULLET` | Build the Bullet Physics integration |
| `RHEA_FETCH_MISSING_DEPENDENCIES` | Download supported dependencies when they cannot be found locally |
| `RHEA_SDL_STATIC` | Prefer static SDL3 linkage |

## Repository model

Rhea is distributed as one SDK repository. Its subsystems are separate CMake targets and can be enabled or disabled according to the needs of an application.

Games and application-specific code are expected to live in separate repositories. Official SDK tools, such as the scene editor, may be maintained in this repository while remaining optional build targets.

The `mwpkg` tool can provide additional package-management workflows, but it is not required to use the SDK.

## Status

Rhea is currently experimental and intended primarily for evaluation and active development.

Before a stable release, the project still expects changes in areas such as:

- public API stability
- editor and asset workflows
- installation and package configuration
- platform coverage
- documentation and examples

## License

See [LICENSE](LICENSE).
