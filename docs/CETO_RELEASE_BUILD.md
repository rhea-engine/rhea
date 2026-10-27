# Standalone Ceto release build on Windows

Use an x64 Visual Studio developer PowerShell with Git, CMake 3.24 or newer,
Ninja, and Python 3 available. Build from a committed engine checkout, not the
parent development workspace.

## Prepare vcpkg

vcpkg is a separate build tool and does not need to be copied into every clone.
Reuse an existing installation:

```powershell
$env:VCPKG_ROOT = 'path\to\vcpkg'
```

Alternatively, install it once at a location of your choice:

```powershell
git clone https://github.com/microsoft/vcpkg.git C:\Tools\vcpkg
& C:\Tools\vcpkg\bootstrap-vcpkg.bat
$env:VCPKG_ROOT = 'C:\Tools\vcpkg'
```

## Configure and build

In the engine checkout (the directory containing this repository's top-level
`CMakeLists.txt`):

```powershell
cmake --preset x64-release-vcpkg
cmake --build out/build/x64-release-vcpkg --target Ceto
```

The preset uses a separate build directory, so a previous non-vcpkg configure
does not contaminate this build. Manifest mode installs Bullet, SDL3, OpenSSL,
and nlohmann/json automatically. The registry baseline matches the development
workspace baseline. The `x64-windows-static-md` triplet uses static dependency
libraries and the dynamic MSVC runtime, matching the workspace's linkage policy.

Missing dependencies fail configuration rather than silently falling back to
FetchContent. In particular, this route does not download the Bullet revision
pinned in the engine's FetchContent fallback. Updating that fallback is a
separate dependency change.

## Try the output

Locate `Ceto.exe` under `out/build/x64-release-vcpkg` and run it with its adjacent
`Data/ceto` and `Data/rhea` directories intact. Open the documented example scene
with the file browser. This is a local release-build check, not yet a complete
redistributable package: verify runtime DLL requirements and test on a clean
Windows system before publishing.
