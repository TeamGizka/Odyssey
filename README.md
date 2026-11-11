# Odyssey
A reconstruction of the Odyssey game engine

# Star Wars: Knights of the Old Republic

Source tree and build of the PC game executable, `swkotor.exe` version 1.03. Built with the original toolchain, the
result is byte-identical to the released executable.

```
dev/          the game: engine libraries (libsource/), game layers and the executable (game/), utilities
3rdparty/     where the Miles Sound System, Bink and NvTriStrip SDKs go (not distributed, see below)
cmake/        toolchain files, platform settings and build helpers
scripts/      extract_res.py: copies the cursors and icon out of your swkotor.exe
```

## Prerequisites

- Windows, CMake 3.25 or later, Python 3 (used by small build steps and `scripts/extract_res.py`).
- Visual Studio .NET 2002 (Visual C++ 7.0, `cl` 13.00.9466) with its Platform SDK. Set the environment variable
  `VS2002_ROOT` to the directory that contains `Program Files/Microsoft Visual Studio .NET` of that install; the
  presets take the compiler, `nmake`, headers and libraries from there.
- The third-party SDKs, copied into `3rdparty/` as listed in each `MANIFEST.txt` (see `3rdparty/README.md`).
- The cursors and icon of the game, extracted once from your own copy of `swkotor.exe` 1.03:

  ```
  python scripts/extract_res.py "C:\path\to\swkotor.exe"
  ```

  This writes `dev/game/swmain/res/` (93 files) and checks each file against `scripts/extract_res.sha256`.

None of these files are part of the repository. With them in place the build reads nothing else outside the
repository and the toolchain.

## Build

```
set VS2002_ROOT=C:\path\to\Microsoft Visual Studio .NET 2002
cmake --preset tier-b
cmake --build --preset tier-b
```

The executable is written to `build/tier-b/release/swkotor.exe`
(SHA-256 `0290889e0cea6ebc00021c1bdb945704fc4c45afaf7fcebabb0e82ba674437e6`).

| Preset | |
|---|---|
| `tier-b` | recommended: byte-identical `swkotor.exe` (`KOTOR_KEEPER_FILL=ON`) |
| `tier-a` | the same build without the generated filler block (`KOTOR_KEEPER_FILL=OFF`), see `cmake/keeperfill/README.md` |
| `vc70` | the plain toolchain preset the two above inherit; default options |
| `xbox` | Xbox configuration, see below |

Targets: `swmain` links `dev/game/swmain/swkotor.exe` in the build directory; `swkotor_release` (part of the default
build) writes `release/swkotor.exe` from it, setting the link time stamp and checksum fields of the shipped file
(`cmake/release/release_header.py`). `-DKOTOR_BUILD_SWSERVER=ON` adds an experimental dedicated server, `swserver`.

## Addresses

A comment on a function definition gives the address of that function in the released images:

```
// PC 0x005A1230  Xbox 0x0012F3A0
```

`PC` is the virtual address in `swkotor.exe` 1.03, `Xbox` the virtual address in the Xbox game's `default.xbe`. A part
is left out where the function has no single address in that image (inlined everywhere, a template with several
instances, or not located).

## Xbox

`dev/` also holds the Xbox sources. The `xbox` preset (environment variables `XDK_ROOT` for the Xbox XDK and
`VS2002_ROOT` for `nmake`) configures the Xbox libraries, but the Xbox build is not yet driven by CMake: several
units do not compile for that platform and no XBE is packaged.

# AI Disclaimer
LLMs were used heavily during the development of this source tree reconstruction. Despite this it has been handled with care and follows strict exactness rules where plausible