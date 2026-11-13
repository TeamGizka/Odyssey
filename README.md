# Odyssey
A reconstruction of the Odyssey game engine

# Star Wars: Knights of the Old Republic II: The Sith Lords

Source tree and build of the PC game executable, `swkotor2.exe` (Steam release, file version 2.2.0.0, linked
2005-03-03). Work in progress: the tree starts from the sources of the first game (branch `k1`) and does not yet
reproduce `swkotor2.exe`.

```
dev/          the game: engine libraries (libsource/) and game layers and the executable (game/)
3rdparty/     where the Miles Sound System, Bink and NvTriStrip SDKs go (not distributed, see below)
cmake/        toolchain files, platform settings and build helpers
scripts/      extract_res.py: copies the cursors and icon out of your swkotor2.exe; make_implibs.py: writes the
              Bink and Miles import libraries from the DLLs the game ships
```

## Prerequisites

- Windows, CMake 3.25 or later, Python 3 (used by small build steps and `scripts/extract_res.py`).
- Visual Studio .NET 2002 (Visual C++ 7.0, `cl` 13.00.9466) with its Platform SDK. Set the environment variable
  `VS2002_ROOT` to the directory that contains `Program Files/Microsoft Visual Studio .NET` of that install; the
  presets take the compiler, `nmake`, headers and libraries from there.
- The third-party SDKs, copied into `3rdparty/` as listed in each `MANIFEST.txt` (see `3rdparty/README.md`).
- The cursors and icon of the game, extracted once from your own copy of `swkotor2.exe`:

  ```
  python scripts/extract_res.py "C:\path\to\swkotor2.exe"
  ```

  This writes `dev/game/swmain/res/` (`icon101.ico`, `cursor1.cur` ... `cursor92.cur`) and checks each file against
  `scripts/extract_res.sha256`.
- The Bink 1.6b and Miles import libraries, written once from the `binkw32.dll` and `Mss32.dll` of your game folder
  with Visual Studio .NET 2003 (`VS2003_ROOT` = the folder that holds its `Vc7`):

  ```
  python scripts/make_implibs.py "C:\path\to\Knights of the Old Republic II"
  ```

  This writes `3rdparty/Bink/1.6b/lib/binkw32.lib` and `3rdparty/Miles/6.6g/lib/mss32.lib` (see their README.md).

None of these files are part of the repository. With them in place the build reads nothing else outside the
repository and the toolchain.

## Build

```
set VS2002_ROOT=C:\path\to\Microsoft Visual Studio .NET 2002
cmake --preset vc70
cmake --build --preset vc70
```

The executable is written to `build/vc70/release/swkotor2.exe`. Targets: `swmain` links
`dev/game/swmain/swkotor2.exe` in the build directory; `swkotor2_release` (part of the default build) writes
`release/swkotor2.exe` from it, with the link time stamp and checksum fields of the shipped file
(`cmake/release/release_header.py`). The `xbox` preset configures the libraries
for Xbox (environment variables `XDK_ROOT` and `VS2002_ROOT`); there is no Xbox executable yet.

## Addresses

A comment on a function definition gives the address of that function in the released executable:

```
// PC 0x005A1230
```

`PC` is the virtual address in `swkotor2.exe`. It is left out where the function has no single address in that
image (inlined everywhere, a template with several instances, or not located).

# AI Disclaimer
LLMs were used heavily during the development of this source tree reconstruction. Despite this it has been handled with care and follows strict exactness rules where plausible