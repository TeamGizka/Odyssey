# Bink 1.6b

The import library swkotor2.exe links for binkw32.dll: the game ships binkw32.dll 1.6b (2003-12-10), and the hints
of its 20 Bink imports are that DLL's export name table. The Bink 1.6b SDK release is not at hand, so
`lib/binkw32.lib` is rebuilt by `scripts/make_implibs.py` from the shipped DLL the way RAD produced it: as the
import library LINK 7.10 (Visual Studio .NET 2003, the linker of binkw32.dll 1.6b) writes when it links the DLL.
Rebuilt the same way from binkw32.dll 1.5v, the method reproduces every import record of the 1.5v SDK library.
The headers stay those of `Bink/1.5v/include`.
