# Third-Party Software Notices

The Rhea SDK contains and optionally uses third-party software.

The Rhea SDK itself is licensed under the MIT License as described in
[LICENSE](LICENSE). Third-party components remain subject to their respective
licenses. Nothing in the Rhea SDK license changes or replaces those licenses.

## Bundled Third-Party Software

The following software is included in the Rhea source repository.

### stb_image

- Component: stb_image
- Version: 2.23
- Upstream: https://github.com/nothings/stb
- Local path: `Components/ThirdParty/stb_image`
- License: MIT License
- License text: `Components/ThirdParty/stb_image/LICENSE`
- Modifications: None

### stb_vorbis

- Component: stb_vorbis
- Version: 1.22
- Upstream: https://github.com/nothings/stb
- Local path: `RheaAudio/thirdparty/stb`
- License: MIT License
- License text: included at the end of `stb_vorbis.c`
- Modifications: None

### tinygltf

- Component: tinygltf
- Version: 2.8.10
- Upstream: https://github.com/syoyo/tinygltf
- Local path: `Rhea/ThirdParty/tiny_gltf_loader`
- License: MIT License
- License text: `Rhea/ThirdParty/tiny_gltf_loader/LICENSE`
- Modifications: None

### tinyobjloader

- Component: tinyobjloader
- Version: 2.0.0
- Upstream: https://github.com/tinyobjloader/tinyobjloader
- Local path: `Rhea/ThirdParty/tiny_obj_loader`
- License: MIT License
- License text: `Rhea/ThirdParty/tiny_obj_loader/LICENSE`
- Modifications: None

### glad

- Component: glad-generated OpenGL loader
- Generator version: 0.1.36
- Upstream: https://github.com/Dav1dde/glad
- Local path: `Hippios/GLRenderer/glad`
- Generated API: OpenGL 4.5 compatibility profile
- License: See `Hippios/GLRenderer/glad/include/glad/LICENSE` and `Hippios/GLRenderer/glad/include/KHR/LICENSE`
- Modifications: None

### miniaudio

- Component: miniaudio
- Version: 0.11.25
- Upstream: https://github.com/mackron/miniaudio
- Local path: `RheaAudio/thirdparty/miniaudio`
- License: MIT No Attribution License (MIT-0)
- License text: `RheaAudio/thirdparty/miniaudio/LICENSE`
- Modifications: None

### libogg

- Component: libogg
- Version: 1.3.5
- Upstream: https://github.com/xiph/ogg
- Local path: `RheaAudio/thirdparty/libogg`
- License: BSD-style license
- License text: `RheaAudio/thirdparty/libogg/COPYING`
- Authors: `RheaAudio/thirdparty/libogg/AUTHORS`
- Modifications: None

### libvorbis

- Component: libvorbis
- Version: 1.3.7
- Upstream: https://github.com/xiph/vorbis
- Local path: `RheaAudio/thirdparty/libvorbis`
- License: BSD-style license
- License text: `RheaAudio/thirdparty/libvorbis/COPYING`
- Authors: `RheaAudio/thirdparty/libvorbis/AUTHORS`
- Modifications: None

### Inter Font

- Component: Inter font
- Upstream: https://github.com/rsms/inter
- Local path: `Rhea/Assets/fonts`
- License: SIL Open Font License
- License text: `Rhea/Assets/fonts/INTER_LICENSE.txt`
- Modifications: Bitmap font assets were generated from the original font files.

## Optional and Build-Time Dependencies

The following dependencies are not necessarily stored in this repository.
They may be supplied by the host system, a package manager, or CMake
FetchContent, depending on the selected configuration:

| Dependency | Version or constraint | Purpose |
|---|---:|---|
| SDL | 3.4.12 | Window, input, and platform integration |
| Bullet Physics | master | Optional physics implementation |
| nlohmann/json | 3.12.0 | JSON parsing |
| GoogleTest | 1.17.0 | Tests only |
| OpenSSL | system-provided version | Optional TLS support |
| Vorbis | system-provided version | Alternative to bundled Vorbis libraries |

These components remain subject to their upstream licenses. Distributions
that include compiled versions of these dependencies must include all notices
and license texts required by the exact versions used in that distribution.

## Modifications

If a bundled third-party component is modified, its entry above must describe
the modification or link to a local patch record.

Do not describe an imported component as unmodified if local changes have
been applied.

## Updating This Document

When adding or updating third-party software:

1. Record the exact upstream project and source URL.
2. Record the exact version or commit.
3. Verify the license against that exact version.
4. Store the unmodified upstream license text with the bundled source.
5. Record whether the source was modified.
6. Update dependency hashes or immutable commit references.
7. Update this document.
8. Ensure binary release packages contain the applicable license notices.