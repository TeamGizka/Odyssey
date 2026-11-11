# NvTriStrip

NVIDIA's triangle-stripping library, statically linked into the game (`NvStripifier`, `NvStripInfo`, `VertexCache`,
wrapped by the game's `AurStripify`). Built by `CMakeLists.txt` in this directory as `NvTriStrip.lib`.

`original/` holds the NVIDIA library release (2002-2003), from github.com/drhelius/NvTriStrip-Library, commit 99a1486
"loading original sources". `VertexCache.cpp` duplicates the inline bodies in `VertexCache.h` and is not compiled.

Xbox: the NVIDIA sources include `<windows.h>`; the Xbox configuration gives this target alone `xbox/windows.h`,
which includes `<xtl.h>`, so `original/` stays unmodified.
