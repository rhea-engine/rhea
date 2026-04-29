# Rhea

Modern C++ game framework built around modular systems, rendering, and tooling.

Rhea is part of the broader Meworla ecosystem and is designed for building games and interactive applications with a flexible, component-oriented architecture.

---

## Features

- Modern C++20 architecture
- Modular framework structure
- Cross-platform oriented design
- Integrated rendering through Poseidon
- Virtual filesystem support through Orion
- Optional subsystems such as audio and client functionality
- Designed for both standalone and ecosystem-based usage

---

## Status

> ⚠️ Rhea is currently experimental and under active development.
>
> APIs may change frequently.
> Documentation and examples are still evolving.

---

## Architecture

Rhea is built as a collection of modular libraries.

Core ecosystem projects currently include:

- Poseidon — rendering framework
- Orion — virtual/global filesystem
- Rhea Common — shared foundational utilities and libraries

Optional modules may be bundled inside the repository while remaining independent CMake targets.

---

## Building

Clone the repository:

```bash
git clone https://github.com/rhea-engine/rhea.git
cd rhea
````

Configure and build:

```bash
cmake -S . -B build
cmake --build build
```

Build samples:

```bash
cmake -S . -B build -DRHEA_BUILD_SAMPLES=ON
cmake --build build
```

---

## Using with FetchContent

```cmake
include(FetchContent)

FetchContent_Declare(
    Rhea
    GIT_REPOSITORY https://github.com/rhea-engine/rhea.git
    GIT_TAG v0.9.0
)

FetchContent_MakeAvailable(Rhea)

target_link_libraries(MyGame PRIVATE Meworla::Rhea)
```

---

## Optional Modules

Some modules are optional and may be excluded from default builds.

Examples include:

* RheaClient
* RheaAudio

These modules are typically added as independent CMake targets.

---

## Samples

Sample applications are available in the `Samples/` directory.

Build samples with:

```bash
cmake -S . -B build -DRHEA_BUILD_SAMPLES=ON
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