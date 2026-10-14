# Getting Started with Rhea

This guide introduces Rhea through BasicApp, a small C++ application that creates a scene, camera, directional light, and textured cube. Start here before moving on to the Ceto scene viewer or a larger external application such as Novara.

**Documentation draft:** the commands and application behavior below are based on the current source. They have not yet been verified with a clean standalone build. The public repository URL remains to be confirmed.

## Prerequisites

- A C++17 compiler and its platform development tools.
- CMake 3.24 or newer.
- Python 3 for build-time Fabric and type registry generation.
- Git to obtain the repository and fetch Git-based dependencies.
- Dependencies required by the selected graphics and platform backends.
- Network access if CMake needs to download missing dependencies.

On Windows with MSVC, run the commands from a developer terminal with the compiler available. Ninja is required if you use the repository's Ninja presets. The commands below let CMake use its default generator instead.

See [Build dependencies](DEPENDENCIES.md) for the required libraries, automatic downloads, and a vcpkg setup that disables Rhea's download fallbacks. The default BasicApp configuration enables SDL3, Bullet, and audio. nlohmann/json is always required by the top-level configuration; OpenSSL is optional and is not downloaded automatically.

Windows and Linux are supported. macOS support is planned but has not been tested; this walkthrough targets Windows and Linux. The repository also contains a macOS preset for future development. Specific compiler versions and graphics backend combinations remain to be documented during release validation.

## Configure and build BasicApp

Obtain a checkout of the engine repository and open a terminal in its root, the directory containing `Rhea/`, `Poseidon/`, and the top-level `CMakeLists.txt`. In the development workspace this directory is `SDK/Rhea`; do not configure from the workspace root for this walkthrough.

```sh
cmake -S . -B out/basic-app -DRHEA_BUILD_APPS=OFF -DRHEA_BUILD_SAMPLES=ON -DPOSEIDON_BUILD_SAMPLES=OFF -DRHEA_BUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Debug
cmake --build out/basic-app --config Debug --target Rhea_Sample_01_BasicApp
```

This configuration selects the engine sample and omits Ceto and the renderer-only samples. It retains the default engine subsystem selections. The full dependency requirements for this configuration still need clean-build validation.

If CMake cannot find Python, repeat configuration with `-DPython3_EXECUTABLE` pointing to your Python executable. For compiler or dependency errors, retain the configure output and check which tool or package it identifies before proceeding.

## Run the sample

Find the built executable named `Rhea_Sample_01_BasicApp` (`Rhea_Sample_01_BasicApp.exe` on Windows) inside `out/basic-app`. Its directory depends on the generator; multi-configuration generators may add a `Debug` directory.

The sample's build rules copy these assets next to the executable:

```text
Data/
  basic_app/
  rhea/
```

Change your terminal's working directory to the directory containing the executable and run it. Keep the `Data` directory beside it. On Windows PowerShell, use:

```powershell
.\Rhea_Sample_01_BasicApp.exe
```

On Linux, use:

```sh
./Rhea_Sample_01_BasicApp
```

The intended result is a window displaying a textured, lit cube. Press Escape to exit. This visual result still needs runtime verification before publication.

If assets are missing, check the post-build copy output and the adjacent `Data` directory. If the application opens but renders incorrectly, report the operating system, graphics backend, and application output.

## Make your first change

Open `Rhea/Samples/01_BasicApp/BasicApp.cpp`. In `createScene()`, find the camera position:

```cpp
camera->setLocalPosition(math::Vector3(0, 2, -7.5f));
```

Move it farther from the cube:

```cpp
camera->setLocalPosition(math::Vector3(0, 2, -8));
```

Rebuild the same target from the repository root, then run the executable again. With the camera still looking at the origin, the cube should appear smaller. This demonstrates the edit-build-run workflow without requiring an editor.

## Understand the example

- `setup()` loads the shader and texture before calling `createScene()`, which creates the scene, camera, lighting, cube mesh, and material.
- `onInput()` handles Escape to request an exit.
- `main()` initializes the application and enters its loop.
- The sample links `Rhea::Core`, `Rhea::Client`, and `RheaFabrics`.

For engine composition and integration into your own CMake project, continue with the [README](../README.md).

## Next: Ceto and larger applications

Ceto is the scene viewer intended to demonstrate scene loading and exploration. Its public walkthrough will specify a redistributable scene, launch configuration, and controls once these are validated.

Novara is a larger external application for studying how Rhea is used in practice. It is planned to become public alongside the SDK. Its repository link, build instructions, and compatible engine revision will be documented for publication. Neither Ceto nor Novara is needed to complete this introductory sample.



