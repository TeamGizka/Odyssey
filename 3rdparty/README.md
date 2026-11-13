# 3rdparty/

Third-party SDKs the game is built against, in a versioned `<SDK>/<version>/` layout beside the game tree (`dev/`).
The SDK files are not distributed with this repository: copy them from your own SDK installs into the paths listed
in each directory's `MANIFEST.txt`, which also gives the SHA-256 every file must have. Only the README, manifest
and build files are tracked.

| Directory | SDK | Use |
|---|---|---|
| `Miles/6.6g/` | Miles Sound System import library | the import library the game links (release library of the Miles 6.6g reconstruction) |
| `Miles/6.5h/` | Miles Sound System 6.5h | headers |
| `Miles/6.5c/` | Miles Sound System 6.5c | older SDK; not used by the build |
| `Miles/6.1c/` | Miles Sound System 6.1c | older import library (swkotor.exe); not used by the build |
| `Bink/1.6b/` | Bink 1.6b | headers and `binkw32.lib` the game links (release library of the Bink 1.6b reconstruction) |
| `Bink/1.5v/` | Bink 1.5v | older SDK; not used by the build |
| `DirectX/8.1/` | DirectX 8.1 SDK | `dinput8.lib` |
| `NvTriStrip/original/` | NVIDIA NvTriStrip | built from source as `NvTriStrip.lib` |
