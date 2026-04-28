# Poseidon

C++ rendering framework used by the Rhea ecosystem.

---

## Features

- Modern C++20 renderer architecture
- Vulkan-based rendering backend
- Frame renderer system
- Materials, textures, cubemaps
- Window abstraction
- Designed for integration into the Rhea ecosystem
- Cross-platform oriented architecture

---

## Status

> ⚠️ Poseidon is currently experimental and under active development.
>
> APIs may change frequently.
> Documentation and examples are still evolving.

---

## Example

```cpp
#include <Poseidon/Core.h>

int main()
{
    return 0;
}
````

---

## Building

Clone the repository:

```bash
git clone https://github.com/rhea-engine/poseidon.git
cd poseidon
```

Configure and build:

```bash
cmake -S . -B build
cmake --build build
```

Build with samples enabled:

```bash
cmake -S . -B build -DPOSEIDON_BUILD_SAMPLES=ON
cmake --build build
```

---

## Using with FetchContent

```cmake
include(FetchContent)

FetchContent_Declare(
    Poseidon
    GIT_REPOSITORY https://github.com/rhea-engine/poseidon.git
    GIT_TAG v0.1.0
)

FetchContent_MakeAvailable(Poseidon)

target_link_libraries(MyGame PRIVATE Meworla::Poseidon)
```

---

## Architecture

Poseidon is part of the larger Rhea ecosystem.

Some lower-level modules are bundled internally and may later become standalone packages.

---

## Roadmap

Planned improvements include:

* Additional rendering backends
* Shader pipeline improvements
* Better documentation
* Sample applications

---

## License

MIT License
