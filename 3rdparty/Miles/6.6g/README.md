# Miles Sound System import library (6.6g)

The import library swkotor2.exe links for mss32.dll: Miles 6.6g (December 2004), the release of the Miles providers
the game ships. Its export list is that of mss32.dll 6.5h, which the game also ships, with three more names:
`_AIL_find_3D_provider@8` and `_AIL_find_filter@8` (added in 6.6g) and `_AIL_set_sample_info@8` (added in 6.6f).
`lib/mss32.lib` is rebuilt by `scripts/make_implibs.py` the way RAD produced it: as the import library LINK 7.10
(Visual Studio .NET 2003, the linker of mss32.dll) writes when it links the DLL, from the export table of the shipped
Mss32.dll 6.5h plus the three names, with each export declared as in the 6.5h SDK library. No game code imports the
three names. The headers stay those of `Miles/6.5h/include`.
