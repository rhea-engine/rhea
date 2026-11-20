Ceto scene viewer
=================

Extract the complete Windows x64 ZIP before starting Ceto.exe. Keep the
Data directory, runtime DLLs, and realm-resolver.conf next to the executable.
No command-line arguments are needed.

In the Open browser, select Samples, open Courtyard, and open world.scene.
See Data/samples/Courtyard/README.md for controls and suggested scene changes.

Open a local Rhea scene (.scene) or a glTF, GLB, or OBJ asset through the
Open browser. You can also enter a direct URL or a configured realm address.
Keep external buffers, materials, and textures in their expected locations.
Use Open or press O to return to the browser.

In an asset preview:

  Left mouse drag   Orbit around the asset
  Mouse wheel       Zoom
  Focus or F        Fit the asset in the view
  Reset or R        Restore the initial view
  Play / Pause      Control a skeletal animation in the Animations panel

The address field shows the opened content. Enter another address and press
Enter to open it. Rhea worlds can supply their own camera and controls.

Ceto previews supported glTF skeletal clips. glTF node, morph target, and
material animations are outside the supported playback scope.

Licenses: LICENSE, THIRD_PARTY_NOTICES.md, and the Licenses directory.
The engine font license is included under Data/rhea/fonts.
