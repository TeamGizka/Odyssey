# Bink 1.5t for Xbox

The Bink for Xbox SDK the Xbox build compiles and links against: Bink 1.5t for Xbox (2003-02-06). Source of the files:
TeamGizka BinkSDK, branch `xbox-1.5t` (`Include/` -> `include/`, `Lib/binkxbox.lib` -> `lib/binkxbox.lib`).

`include/` holds the public headers (bink.h, rad3d.h, radbase.h, radmem.h); the Xbox movie player
(`dev/libsource/exomovieplayer`) compiles against them (CMake variable `KOTOR_XBOX_BINK_DIR`). `lib/binkxbox.lib` is
the SDK's library (link-time code generation objects, so the Xbox link uses /LTCG; CMake variable
`KOTOR_XBOX_BINK_LIB`).

Why 1.5t and not 1.5s (branch `xbox-1.5s`, 2003-01-14): the retail K2 Xbox executable links a Bink for Xbox later than
both (its sound track records are 0x17C bytes; 1.5t has 0x178) built with link-time code generation, which 1.5t's
library uses and 1.5s's does not. A test executable linked against each library and compared function by function
with the retail BINK sections finds 51,490 of 103,259 bytes identical with 1.5t (40 of 131 C functions, 79 of 130
blitters) and 40,175 of 105,758 with 1.5s (33 of 137 C functions, 133 of 207 blitters). The API and the HBINK fields
the game reads are the same in both.
