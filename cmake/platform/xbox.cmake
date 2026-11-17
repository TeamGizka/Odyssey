# Xbox: swmain.exe packaged as default.xbe (Direct3D 8, DirectSound, XInput, Bink). Toolchain: cmake/toolchain-xdk.cmake
# with the XDK 5849 the XBE's library-version table names (its vc71 compiler is cl 13.10.3077).
# The link produces a relocatable Xbox PE with a PDB; packaging it into an XBE is not part of this build.
# No /EHsc: the retail bodies carry no C++ exception-handling state (functions with local objects that have destructors
# have no EH frame), unlike the PC build.
set(KOTOR_COMPILE_OPTIONS /nologo /W3 /MT /O2 /Gy /arch:SSE)
set(KOTOR_COMPILE_DEFINITIONS NDEBUG _XBOX)
set(KOTOR_LINK_OPTIONS /nologo /RELEASE /OPT:REF /SUBSYSTEM:XBOX /MACHINE:X86 /FIXED:NO /DEBUG)
set(KOTOR_EXECUTABLE_KIND "")
