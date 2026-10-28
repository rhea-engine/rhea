# Rhea

A modular C++ virtual world and game engine framework designed for flexibility, composability, and explicit engine boundaries.

Rhea may be used as a standalone engine or as a collection of libraries integrated into other projects. 
Using the CMake build system, Rhea can be built and linked as a single library or as a set of independent modules.

---

## Features

- Modern C++17 architecture
- CMake-based build system
- Modular framework structure
- Cross-platform oriented design
- Subsystems for rendering, audio, physics can be included or excluded and replaced as needed
- Designed for both standalone and library usage

---

## Status

See the [v0.2.0 release notes](docs/releases/v0.2.0.md) for Ceto packaging,
engine changes, migration guidance, and validation limits.

> ⚠️ Rhea is currently experimental and under active development.
>
> APIs may change frequently.
> Documentation and examples are still evolving.
>
> An editor is planned but is not included in the initial public release.

---

## Who Rhea is for

Rhea targets C++ developers who want to build their own 3D applications and compose or extend the engine's subsystems. The first public release is intended for experimentation and feedback. API compatibility is not yet guaranteed.

## Start here

1. **Build and modify BasicApp:** follow the [Getting Started guide](docs/GETTING_STARTED.md) to explore a small application with a scene, camera, light, and textured cube.
2. **Explore scenes with Ceto:** use the scene viewer as the next demonstration of the engine. A public scene package and a verified walkthrough are still being prepared.
3. **Study a larger application:** Novara is an external, more complex example. It is planned to become public alongside the SDK; it is not required for Getting Started.

## Architecture

The SDK has two main parts:

- **Rhea Engine** provides the application model, scenes, components, services, resources, and runtime composition.
- **Poseidon** provides rendering abstractions and the rendering pipeline.

Rhea and Poseidon are separate libraries. Poseidon can be used without the Rhea Engine, and Rhea can be used without Poseidon in 
applications that do not require its renderer. `RheaClient` connects the two for the standard graphical client configuration.

Several lower-level concerns are implemented behind separate abstractions. The repository includes integrations and backends for third-party projects, including:

- Bullet Physics through `RheaPhysicsBullet`
- miniaudio through `RheaAudio`
- SDL through the Psio and Hippios platform backends

These integrations are not intended to define the only possible implementations.
Applications may select the provided implementation, omit the subsystem, 
or provide another implementation of the corresponding Rhea, Poseidon, Psio, or Hippios interface.

### Fabrics

The Rhea Engine is composed from modules called **Fabrics**.

A Fabric groups the code needed to add a particular capability to a Rhea application. 
Depending on the subsystem, this may include services, components, resource types, loaders, controllers, or application lifecycle setup.

Examples in this repository include animation, character handling, controllers, geometry, GUI, terrain, text, media, and asset-format support.

Fabrics are registered during the CMake build and assembled into the application’s Fabric registry. 
A project can select which registered Fabrics are enabled through its `enabled.json` configuration. 
Applications and external modules may also register their own Fabrics.

This makes Fabric selection a build-time composition mechanism rather than a fixed set of features built into `Rhea::Core`. A provided Fabric can be:

- enabled when its capability is needed
- left out of an application
- replaced by another Fabric
- supplemented by application-specific Fabrics

Replacing a Fabric may require providing compatible services or interfaces when other enabled Fabrics depend on it.

### Default Client Composition

A typical graphical Rhea application uses the following composition:

```text
Rhea Engine
    |
    +-- selected Fabrics
    |
RheaClient
    |
Poseidon renderer
    |
Hippios graphics backend
    |
platform integration, such as SDL, OpenGL, or D3D11
```

This is the configuration provided by the SDK, not a requirement imposed on every Rhea application. 
Headless applications can use the engine without the client and rendering layers, while renderer-only applications can use Poseidon independently of Rhea.

---

## Supported platforms

| Platform | Status | Tested |
|---|---|---|
| Windows | Supported | Visual Studio 2026, Win 11, D3D11, OpenGL |
| Linux | Supported | WSL Ubuntu 22.04, OpenGL |
| macOS | Planned | not tested |

Specific compiler versions and graphics backend combinations will be documented as part of release validation.

## Requirements

The common build requirements are:

- CMake 3.24 or newer
- a C++17 compiler
- platform-specific development libraries required by the selected backends

Python 3 is required when building the Rhea Engine. It is used at build time to generate:

- the Fabric registry that assembles the selected Fabrics
- the type registry used by development tools and editors

Python is not linked into Rhea applications and is not required at runtime.

CMake normally discovers Python automatically. A specific interpreter can be selected when configuring:

```sh
cmake -S . -B out/build \
    -DPython3_EXECUTABLE=/path/to/python
```

The type registry can be disabled independently:

```sh
cmake -S . -B out/build \
    -DRHEA_GENERATE_TYPE_REGISTRY=OFF
```

Disabling the type registry does not disable Fabric registry generation. Python is therefore still required for an Engine build.

A renderer-only configuration (`RHEA_BUILD_ENGINE=OFF`) does not initialize the Fabric registry and does not require Python.

nlohmann/json is required by the top-level build. SDL3 and Bullet Physics are required when their integrations are enabled, as they are by default. CMake first looks for installed packages and downloads these libraries if missing, unless `RHEA_FETCH_MISSING_DEPENDENCIES=OFF`. OpenSSL is optional, is never automatically downloaded, and enables TLS only when found. Vorbis is bundled by default for audio.

See [Build dependencies](docs/DEPENDENCIES.md) for the dependency table, feature switches, and complete vcpkg configuration examples.

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

Tests are disabled by default (`RHEA_BUILD_TESTS=OFF`). The Windows (D3D11/OpenGL) and WSL Linux (OpenGL) validation reported above covered the engine and BasicApp with tests disabled; it does not establish that the automated test suite passes.

To explicitly build and run tests in a separate build directory:

```bash
cmake -S . -B build-tests -DRHEA_BUILD_TESTS=ON -DRHEA_BUILD_APPS=OFF
cmake --build build-tests --config Debug
ctest --test-dir build-tests -C Debug --output-on-failure --timeout 60
```

Tests require GoogleTest and GoogleMock. CMake uses an installed GoogleTest package or downloads it when `RHEA_FETCH_MISSING_DEPENDENCIES=ON`. TLS socket tests are included only when OpenSSL support is available. Some network tests require external services.

For an existing build directory, explicitly pass `-DRHEA_BUILD_TESTS=OFF` to disable a previously cached `ON` setting.

Build engine samples:

```bash
cmake -S . -B build -DRHEA_BUILD_SAMPLES=ON
cmake --build build
```

Build renderer samples:

```bash
cmake -S . -B build -DPOSEIDON_BUILD_SAMPLES=ON
cmake --build build
```

---

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

## Using with FetchContent

The example below must be pinned to a published tag or commit before use. The first release tag is still to be confirmed.

```cmake
include(FetchContent)

FetchContent_Declare(
    Rhea
    GIT_REPOSITORY https://github.com/rhea-engine/rhea.git
    GIT_TAG <published-tag-or-commit>
)

FetchContent_MakeAvailable(Rhea)

target_link_libraries(MyGame PRIVATE Rhea::Core)
```

---

## Samples

Sample engine applications are available in the `Rhea/Samples/` directory.

Sample renderer applications are available in the `Poseidon/Samples/` directory.

See the [Poseidon sample guide](Poseidon/Samples/README.md) for a renderer-only build, backend selection, assets, and controls.

Build samples with:

```bash
cmake -S . -B build -DRHEA_BUILD_SAMPLES=ON
cmake --build build
```

```bash
cmake -S . -B build -DPOSEIDON_BUILD_SAMPLES=ON
cmake --build build
```

## Philosophy

Rhea focuses on:

* Clear modular boundaries
* Modern C++ development practices
* Flexible integration into existing projects
* Standalone usability without requiring custom tooling
* Incremental ecosystem growth

The ecosystem is designed to work with plain CMake while also supporting additional tooling such as `mwpkg`.

---

## Roadmap

Planned improvements include:

* Additional subsystems
* Improved tooling
* Asset pipeline support
* Better documentation
* Expanded sample applications
* Editor and workflow tooling

---

## License

MIT License


