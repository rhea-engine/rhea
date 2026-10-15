# Build dependencies

This guide describes configuration from the SDK repository root. 
The examples target BasicApp and keep tests and Ceto disabled.

## Tools you install yourself

- A C++17 compiler and platform SDK. On Windows, select Visual Studio's **Desktop development with C++** workload, including MSVC, a Windows SDK, and **C++ CMake tools for Windows**. Run commands in a Developer PowerShell or Developer Command Prompt.
- CMake 3.24 or newer.
- Python 3 for Fabric registry generation. Disabling `RHEA_GENERATE_TYPE_REGISTRY` does not remove this requirement. The current top-level SDK also initializes the Fabric registry for renderer-only configurations, so Python is required there too.
- Git to clone the repository and for Git-based dependency downloads.
- Ninja only if you select a Ninja generator or the supplied Ninja presets.

Rhea does not install these tools. Verify availability with `cmake --version`, `git --version`, and a Python version command such as `py -3 --version` on Windows. If Python is installed but CMake cannot locate it, pass `-DPython3_EXECUTABLE="C:/path/to/python.exe"` when configuring.

## Libraries and default behavior

| Dependency | Required when | How it is obtained | Disable the feature |
|---|---|---|---|
| nlohmann/json | Always in the current top-level SDK configuration | Installed CMake package first; otherwise downloads the 3.12.0 archive | No top-level switch |
| SDL3 | SDL backends are enabled; enabled by default | Installed CMake package first; otherwise downloads Git tag `release-3.4.12` | `RHEA_ENABLE_SDL=OFF` |
| Bullet | Engine physics and the Bullet implementation are enabled; both enabled by default | Installed package first (config, then CMake's FindBullet module); 
otherwise downloads Git commit `d3afe76132fc76b0f9b2359cbaef47a3766e7fbb` | `RHEA_BUILD_BULLET=OFF`; or disable all physics with `RHEA_BUILD_PHYSICS=OFF` |
| OpenSSL | TLS support is desired; lookup enabled by default | Installed package only; if missing, configuration continues with TLS disabled | `RHEA_ENABLE_OPENSSL=OFF` |
| GoogleTest | `RHEA_BUILD_TESTS=ON`; disabled by default | Installed package lookup, with a download fallback to `v1.17.0` | `RHEA_BUILD_TESTS=OFF` |
| miniaudio, stb, tinygltf, tinyobjloader, glad | Their respective modules are built | Sources already included in the repository | Select the relevant modules/backends |
| libogg/libvorbis | Audio and Vorbis decoding are enabled; enabled by default | Sources included in the repository by default; external `Vorbis` package if `RHEA_AUDIO_USE_BUNDLED_VORBIS=OFF` | `RHEA_AUDIO_ENABLE_VORBIS=OFF`, or `RHEA_BUILD_AUDIO=OFF` |

These are build-configuration requirements: even building only the BasicApp target can configure other enabled modules and resolve their dependencies. 
FetchContent downloads source at configure time and the dependency is subsequently built with the project. It does not install a system-wide package.

The listed versions describe download fallbacks, not version constraints on installed packages. 
See [Third-party notices](../THIRD_PARTY_NOTICES.md) for licenses.

Turning SDL off requires a suitable alternative window/input backend for a graphical application on Linux. 
It is not a general solution for missing platform libraries. 
Fetching SDL source also does not install a graphics driver or Linux platform development packages.

## Option A: let Rhea obtain missing libraries

From the SDK root:

```sh
cmake -S . -B out/basic-app -DRHEA_BUILD_APPS=OFF -DRHEA_BUILD_SAMPLES=ON -DPOSEIDON_BUILD_SAMPLES=OFF -DRHEA_BUILD_TESTS=OFF -DRHEA_FETCH_MISSING_DEPENDENCIES=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build out/basic-app --config Debug --target Rhea_Sample_01_BasicApp
```

`RHEA_FETCH_MISSING_DEPENDENCIES` defaults to `ON`. Existing packages discoverable by CMake are preferred. 
Missing nlohmann/json, SDL3, and Bullet are fetched; missing OpenSSL is not. 
Network access and Git are needed for the source downloads. Downloaded source normally lives under the build directory's `_deps` directory.

## Option B: provide installed packages and disable Rhea downloads

Set `-DRHEA_FETCH_MISSING_DEPENDENCIES=OFF`. Missing required packages then cause configuration to fail instead of being downloaded. This does not disable SDL, physics, or JSON support.

For installations outside CMake's normal search paths, supply a package-manager toolchain or `-DCMAKE_PREFIX_PATH="C:/path/to/install-prefix"`. The prefix must contain the package configuration files, headers, and libraries, not merely downloaded source.

### vcpkg: explicit installation (classic mode)

Use a standalone vcpkg checkout in a writable directory, for example `C:/dev/vcpkg`. If you already have one, skip the clone and bootstrap steps. The following commands are for Windows PowerShell:

```powershell
git clone https://github.com/microsoft/vcpkg.git C:/dev/vcpkg
& C:/dev/vcpkg/bootstrap-vcpkg.bat
& C:/dev/vcpkg/vcpkg.exe install bullet3 sdl3 nlohmann-json openssl --triplet x64-windows-static-md --classic
```

The port names are `bullet3`, `sdl3`, and `nlohmann-json`; their CMake package names are `Bullet`, `SDL3`, and `nlohmann_json`. OpenSSL is included here to retain TLS support. It can be omitted if TLS is not needed.

Configure from the SDK root using a new build directory:

```powershell
cmake -S . -B out/basic-app-vcpkg "-DCMAKE_TOOLCHAIN_FILE=C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake" -DVCPKG_TARGET_TRIPLET=x64-windows-static-md -DVCPKG_MANIFEST_MODE=OFF -DRHEA_FETCH_MISSING_DEPENDENCIES=OFF -DRHEA_BUILD_APPS=OFF -DRHEA_BUILD_SAMPLES=ON -DPOSEIDON_BUILD_SAMPLES=OFF -DRHEA_BUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Debug
cmake --build out/basic-app-vcpkg --config Debug --target Rhea_Sample_01_BasicApp
```

Use an x64 compiler environment when using this triplet. `x64-windows-static-md` selects static dependency libraries with the dynamic MSVC runtime, matching the usual MSVC runtime default. Other architectures and linkage choices need a matching triplet and compiler configuration. With shared libraries, required DLLs must also be available when running the sample.

`VCPKG_MANIFEST_MODE=OFF` explicitly selects the packages installed in classic mode. Set the toolchain and triplet on the first configure; use a new build directory when changing package source, toolchain, or triplet. `vcpkg integrate install` is not required for this explicit CMake toolchain workflow.

### vcpkg: project-owned dependencies (manifest mode)

For a project that uses Rhea, maintain a `vcpkg.json` at that project's root, for example:

```json
{
  "name": "my-rhea-app",
  "version-string": "0.1.0",
  "dependencies": ["bullet3", "sdl3", "nlohmann-json", "openssl"]
}
```

Configure that project with the vcpkg toolchain, a matching triplet, and `RHEA_FETCH_MISSING_DEPENDENCIES=OFF`. 
Leave manifest mode enabled. The toolchain installs manifest dependencies during configuration. 
Add a registry baseline to pin package resolution for reproducible releases, and commit the manifest and registry configuration.

**Disabling Rhea downloads does not disable vcpkg downloads.** In classic mode, install packages explicitly before configuring. 
In manifest mode, to prevent automatic installation during CMake configuration, set `VCPKG_MANIFEST_INSTALL=OFF` after preinstalling 
dependencies into the same manifest installation directory. A completely offline build also requires all necessary packages, tools, 
and caches to be available locally.

For details, see Microsoft's [CMake integration](https://learn.microsoft.com/en-us/vcpkg/users/buildsystems/cmake-integration) 
and [manifest tutorial](https://learn.microsoft.com/en-us/vcpkg/consume/manifest-mode).
