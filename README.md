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
cmake --preset tier-a
cmake --build --preset tier-a
```

The executable is written to `build/<preset>/release/swkotor.exe`. The three PC presets compile the same sources
with the same options; they differ only in how the linker lays out the functions:

| Preset | |
|---|---|
| `tier-a` | the default: byte-identical `swkotor.exe` (SHA-256 `0290889e0cea6ebc00021c1bdb945704fc4c45afaf7fcebabb0e82ba674437e6`). No order file; a generated block of unreferenced functions is compiled into `swmain` (`KOTOR_KEEPER_FILL=ON`, `cmake/keeperfill/README.md`). |
| `tier-b` | no filler block. The linker places the functions in the order of the shipped file, listed in `cmake/order/swkotor.txt` (`/ORDER`, `KOTOR_LINK_ORDER=ON`). The file as a whole is not identical (see below). |
| `tier-c` | the source tree alone: no order file, no filler block, the linker's default order. |
| `vc70` | the toolchain preset the three above inherit; its default options build the same as `tier-a` |
| `xbox` | Xbox configuration, see below |

The linker folds identical functions into one copy and keeps one of them. Which copy it keeps depends on every
function in the link, including unreferenced ones that it discards afterwards; those of the original build are not
known. In `tier-c` some kept copies therefore sit elsewhere than in the shipped file. The `tier-a` filler block is
never referenced and leaves no byte in the image: its only effect is that every folded group keeps the same copy
as the original, which puts every function and every exception-handling funclet at its original address. It is not
game code and exists only for that purpose. `tier-b` places the kept copies with the order file instead, but an order
file can only name functions with external linkage: the static functions follow the listed ones, the import thunk of
`DirectInput8Create` (which sits among the game functions in the shipped file) follows them too, the C runtime comes
behind them, and the exception-handling funclets keep the layout of `tier-c`. After a change to the sources, write
the order file again from the map of a `tier-a` build:

```
python cmake/order/order_file.py from-map build/tier-a/dev/game/swmain/swkotor.map cmake/order/swkotor.txt
```

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