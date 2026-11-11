# swmain filler block (`KOTOR_KEEPER_FILL`)

The linker folds identical functions and keeps one copy of each group. Which copy it keeps depends on every function
in the link, including unreferenced ones that it discards afterwards. The unreferenced functions of the original
build are not known, so the source tree alone reproduces swkotor.exe 1.03 except for the choice of the kept copy in
some of those groups.

`KOTOR_KEEPER_FILL=ON` (the default, preset `tier-b`) compiles a generated block of unreferenced functions into
`appmanager.cpp` (through `keeperfill_appmanager.inc`; `keeperfill_main.inc` is empty). The linker discards every
one of them; their only effect is that each group keeps the same copy as the original, which makes the image
byte-identical to it. The block is not game code and exists only for that purpose.

`KOTOR_KEEPER_FILL=OFF` (preset `tier-a`) generates empty include files: the image is built from the source tree
alone and differs from the original only in which of several identical copies is kept.

| File | |
|---|---|
| `generate.py` | writes the two include files into the build tree (Python standard library, deterministic) |
| `keys.json` | the sizes of the generated functions |
| `KeeperFill.cmake` | `kotor_keeper_fill()`: runs the generator before swmain compiles |
