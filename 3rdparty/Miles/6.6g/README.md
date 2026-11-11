# Miles Sound System import library (6.6g)

The import library swkotor2.exe links for mss32.dll. The hints of its 61 Miles imports are the export name table of
mss32.dll 6.5h with three more names: two after `_AIL_enumerate_3D_providers@12`, one after
`_AIL_set_sample_file@12` (a 367-export Miles release newer than 6.5h; the Miles providers the game ships are 6.6g,
2004-12-11). That SDK release is not at hand, so `lib/mss32.lib` is rebuilt by `scripts/make_implibs.py` the way
RAD produced it: as the import library LINK 7.10 (Visual Studio .NET 2003, the linker of mss32.dll) writes when it
links the DLL, from the export table of the shipped Mss32.dll 6.5h plus the three names, with each export declared
as in the 6.5h SDK library. The three names never reach the executable (no game code imports them); the script
uses names a later Miles release exports in those places. Rebuilt the same way from mss32.dll 6.5h, the method
reproduces every import record of the 6.5h SDK library except the one `_AIL_mem_free_lock` hint of that file. The
headers stay those of `Miles/6.5h/include`.
