# Poseidon renderer samples

These examples use Psio, Hippios, and Poseidon directly, without the Rhea runtime or an editor.

| Target | Demonstrates |
| --- | --- |
| `Poseidon_Sample_01_Triangle` | Vertex colors, indexed mesh, camera, renderable handle, and viewport context |
| `Poseidon_Sample_02_Cube` | A rotating cube with colored faces and keyboard camera movement |
| `Poseidon_Sample_03_TexturedCube` | UV coordinates and a generated RGBA checkerboard texture |

## Build

From the SDK repository root:

```sh
cmake -S . -B build-poseidon -DRHEA_BUILD_ENGINE=OFF -DRHEA_BUILD_CLIENT=OFF -DRHEA_BUILD_APPS=OFF -DRHEA_BUILD_SAMPLES=OFF -DPOSEIDON_BUILD_SAMPLES=ON -DRHEA_BUILD_TESTS=OFF
cmake --build build-poseidon --config Release --target Poseidon_Sample_01_Triangle Poseidon_Sample_02_Cube Poseidon_Sample_03_TexturedCube
```

The SDK's compiler, CMake, and platform dependency requirements apply. This renderer-only configuration does not require Python. No Rhea scene files or external image files are needed.

## Run

Executables are under `build-poseidon/Poseidon/Samples`, with a `Release` subdirectory for multi-configuration generators. The build copies shader files to `Data/PoseidonSamples` beside each executable. Keep this directory with the program when copying it elsewhere.

```powershell
.\Poseidon_Sample_01_Triangle.exe --backend d3d11
.\Poseidon_Sample_02_Cube.exe --backend opengl
.\Poseidon_Sample_03_TexturedCube.exe --backend opengl
```

On Linux, omit `.exe` and use `./` to launch. Direct3D 11 is available only on Windows; the selected backend must be enabled in the build. `auto`, the default, selects Direct3D 11 on Windows and OpenGL on other platforms.

Press Escape or close the window to exit. In the colored cube sample, arrow keys move the camera horizontally and forward/backward; Home and End move it vertically. The camera remains pointed at the cube. Resizing the window updates the viewport and camera aspect ratio.

Use `--frames 10` to render a bounded number of frames and exit for a smoke check. A zero value runs until closed. `--assets <directory>` overrides the shader directory; otherwise it is located relative to the executable, independently of the working directory. Shader, initialization, and frame errors return a nonzero exit code.

## Code structure

`PoseidonApp` owns the platform engine, renderer, window, input loop, and backend-specific shader loading. `SampleScene` demonstrates ownership of a camera handle, renderable handle, mesh, material, and optional texture. The viewport owns its render context; shader objects belong to `Core`. Scene handles and GPU resources are released before the window and renderer.

`SampleVertex` supplies position, color, and UV attributes with an explicit input layout. The HLSL and GLSL shaders match the frame and object buffers populated by `FrameRenderer`. The cube geometry uses 32-bit indices on every platform.

## Validation

All three samples were built in Release configuration with MSVC on Windows and completed five-frame smoke runs with both Direct3D 11 and OpenGL using the Win32 platform backend. These checks cover initialization, shader compilation, frame execution, and normal shutdown; they do not replace visual and interactive validation. Linux and macOS have not been validated for these updated renderer samples.
