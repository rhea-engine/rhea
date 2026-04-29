# Rhea Common

Shared foundational libraries and utilities used throughout the Rhea ecosystem.

Rhea Common contains reusable low-level modules that support rendering, filesystem, engine, and tooling projects across the ecosystem.

Note: This repository is a filtered public export of the main Rhea development workspace.
Some shared infrastructure may live outside this repository.

---

## Features

- Modern C++20 utility libraries
- Shared platform abstractions
- Math utilities
- Parsing utilities
- Third-party integrations
- Cross-platform oriented architecture
- Designed for standalone and ecosystem usage

---

## Status

> ⚠️ Rhea Common is currently experimental and under active development.
>
> APIs may change frequently.
> Documentation and module structures are still evolving.

---

## Modules

The repository currently contains several reusable modules, including:

- Psio — platform and system utilities
- Math — shared math functionality
- SimpleParser — lightweight parsing utilities

Additional modules may be added over time.

---

## Architecture

Rhea Common acts as the foundational layer for the wider ecosystem.

Projects such as:

- Rhea
- Poseidon
- Orion

build upon components provided by this repository.

Modules are exposed as independent CMake targets while remaining bundled in a shared repository.

---

## Building

Clone the repository:

```bash
git clone https://github.com/rhea-engine/rhea-common.git
cd rhea-common
````

Configure and build:

```bash
cmake -S . -B build
cmake --build build
```

---

## Using with FetchContent

```cmake
include(FetchContent)

FetchContent_Declare(
    RheaCommon
    GIT_REPOSITORY https://github.com/rhea-engine/rhea-common.git
    GIT_TAG v0.9.0
)

FetchContent_MakeAvailable(RheaCommon)

target_link_libraries(MyProject PRIVATE Meworla::Psio)
```

---

## Example

```cpp
#include <Psio/Log.h>
#include <Math/Vector4.h>

int main()
{
    return 0;
}
```

---

## Philosophy

Rhea Common focuses on:

* Reusable low-level infrastructure
* Clear modular boundaries
* Minimal external dependencies
* Modern C++ development practices
* Flexible integration into standalone projects

The repository is designed to work with plain CMake while also supporting additional tooling such as `mwpkg`.

---

## Roadmap

Planned improvements include:

* Additional utility modules
* Improved platform abstractions
* Better documentation
* Expanded testing
* Additional parsing and serialization support

---

## License

MIT License