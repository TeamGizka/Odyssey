# Bink 1.6b for Xbox

The Bink for Xbox SDK the Xbox build compiles and links against: Bink 1.6b for Xbox, built from source. Source of the
files: TeamGizka BinkSDK, branch `1.6b`, preset `xbox` (`cmake --preset xbox`, `cmake --build --preset xbox`; see the
branch README). Copy its `build/xbox/sdk/Include/` to `include/` and `build/xbox/sdk/Lib/binkxbox.lib` to
`lib/binkxbox.lib`.

`include/` holds the public headers (bink.h, rad3d.h, radbase.h: the same files as `../1.6b/include/`, and radmem.h);
the Xbox movie player (`dev/libsource/exomovieplayer`) compiles against them (CMake variable `KOTOR_XBOX_BINK_DIR`).
`lib/binkxbox.lib` is the library the game links (CMake variable `KOTOR_XBOX_BINK_LIB`). It is native code (no
link-time code generation), so the Xbox link needs no /LTCG for it. MANIFEST.txt lists the headers only: the library
records the time of its build and the assembler's source paths, so it has no fixed SHA-256; its code is the same from
every build.

Why 1.6b: the retail K2 Xbox executable links Bink for Xbox 1.6b, built natively with the XDK 5849 compiler. Linked
into a test executable with the game's link options, this library gives all 287 Bink functions of the retail
executable byte for byte (101,986 bytes) and its BINKDATA section unchanged; the 1.5t library (BinkSDK branch
`xbox-1.5t`, used before) gives 119 of them. The API and the HBINK fields the game reads are the same.
