# 3rdparty/

Third-party SDKs the game is built against, in a versioned `<SDK>/<version>/` layout beside the game tree (`dev/`).
The SDK files are not distributed with this repository: copy them from your own SDK installs into the paths listed
in each directory's `MANIFEST.txt`, which also gives the SHA-256 every file must have. Only the README, manifest
and build files are tracked.

| Directory | SDK | Use |
|---|---|---|
| `Miles/6.5h/` | Miles Sound System 6.5h | headers and the import library the game links |
| `Miles/6.5c/` | Miles Sound System 6.5c | older SDK; not used by the build |
| `Miles/6.1c/` | Miles Sound System 6.1c | older import library that resolves three imports of `reswave.cpp` |
| `Bink/1.5v/` | Bink 1.5v | headers and `binkw32.lib` |
| `NvTriStrip/original/` | NVIDIA NvTriStrip | built from source as `NvTriStrip.lib` |
