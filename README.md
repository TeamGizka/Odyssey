# Odyssey
A reconstruction of the Odyssey game engine

# Star Wars: Knights of the Old Republic II: The Sith Lords

Source tree and build of the PC game executable, `swkotor2.exe` (Steam release, file version 2.2.0.0, linked
2005-03-03). The tree started from the sources of the first game (branch `k1`). The `tier-a` build reproduces
`swkotor2.exe` byte for byte (see Build).

```
dev/          the game: engine libraries (libsource/) and game layers and the executable (game/)
3rdparty/     where the Miles Sound System, Bink and NvTriStrip SDKs go (not distributed, see below)
cmake/        toolchain files, platform settings and build helpers
scripts/      extract_res.py: copies the cursors and icon out of your swkotor2.exe; keeperfill/: the tier-a filler block
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

  This writes `dev/game/swmain/res/` (`icon101.ico`, `cursors.res`) and checks each file against
  `scripts/extract_res.sha256`.
- The Bink 1.6b and Miles 6.6g import libraries, copied to `3rdparty/Bink/1.6b/lib/binkw32.lib` and
  `3rdparty/Miles/6.6g/lib/mss32.lib` (see their README.md and MANIFEST.txt).

None of these files are part of the repository. With them in place the build reads nothing else outside the
repository and the toolchain.

## Build

```
set VS2002_ROOT=C:\path\to\Microsoft Visual Studio .NET 2002
cmake --preset tier-a
cmake --build --preset tier-a
```

The executable is written to `build/<preset>/release/swkotor2.exe`. The three PC presets compile the same sources
with the same options; they differ only in how the linker lays out the functions:

| Preset | |
|---|---|
| `tier-a` | the default: byte-identical `swkotor2.exe` (SHA-256 `3144437a7e0009bb6b59165783d7ab233f66606ace83a68089153a13fd0a2c13`). No order file; a generated block of unreferenced functions is compiled into `appmanager.cpp` (`KOTOR_KEEPER_FILL=ON`, `scripts/keeperfill/`). |
| `tier-b` | no filler block. The linker places the functions in the order of the shipped file, listed in `cmake/order/swkotor2.txt` (`/ORDER`, `KOTOR_LINK_ORDER=ON`). Every function is identical to the shipped one; the file as a whole is not (see below). |
| `tier-c` | the source tree alone: no order file, no filler block. Every function is identical to the shipped one, in the linker's default order. |
| `vc70` | the toolchain preset the three above inherit; its default options build the same as `tier-a` |
| `xbox` | Xbox configuration, see below |

The linker folds identical functions into one copy and keeps one of them. Which copy it keeps depends on every
function in the link, including unreferenced ones that it discards afterwards; those of the original build are not
known. In `tier-c` some kept copies therefore sit elsewhere than in the shipped file. The `tier-a` filler block is
never referenced and leaves no byte in the image: its only effect is that every folded group keeps the same copy
as the original, which puts every function and every exception-handling funclet at its original address. It is not
game code and exists only for that purpose. `tier-b` places the kept copies with the order file instead, but an order
file can only name functions with external linkage: the static functions follow the listed ones, the C runtime
behind them, and the exception-handling funclets keep the layout of `tier-c`. After a change to the sources, write
the order file again from the map of a `tier-a` build:

```
python cmake/order/order_file.py from-map build/tier-a/dev/game/swmain/swkotor2.map cmake/order/swkotor2.txt
```

Targets: `swmain` links `dev/game/swmain/swkotor2.exe` in the build directory; `swkotor2_release` (part of the
default build) writes `release/swkotor2.exe` from it: the cursors are added with the Win32 resource-update API, which
rewrites the resource section as the shipped file's (`cmake/release/release_resources.py`), then the link time stamp
and checksum fields of the shipped file are set (`cmake/release/release_header.py`). The `xbox` preset configures the
game for Xbox from the tree and the XDK alone (environment variable `XDK_ROOT`; `VS2002_ROOT` for nmake): the sources
reach the XDK's `<xtl.h>` through `dev/include/platform.h`, and the Bink for Xbox SDK is `3rdparty/Bink/xbox-1.6b/`.
There is no Xbox executable yet.

## Addresses

A comment on a function definition gives the address of that function in the released executable:

```
// PC 0x005A1230
```

`PC` is the virtual address in `swkotor2.exe`. It is left out where the function has no single address in that
image (inlined everywhere, a template with several instances, or not located). `Xbox` is the virtual address in the
Xbox executable (`default.xbe`); a function compiled only for Xbox carries `// Xbox 0x...` alone.

A function that is compiled but has no copy in the executable (the linker removed it, nothing calls it) carries
`// Dead code: not in the PC executable` on the line above its definition instead of an address.

# AI Disclaimer
LLMs were used heavily during the development of this source tree reconstruction. Despite this it has been handled with care and follows strict exactness rules where plausible