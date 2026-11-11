# Xbox: default.exe (Direct3D 8, DirectSound, XInput, XMV). Toolchain: cmake/toolchain-xdk.cmake.
# The link produces a relocatable Xbox PE with a PDB; packaging it into an XBE is not part of this build.
set(KOTOR_COMPILE_OPTIONS /nologo /W3 /MT /O2 /EHsc /Gy /arch:SSE)
set(KOTOR_COMPILE_DEFINITIONS NDEBUG _XBOX)
set(KOTOR_LINK_OPTIONS /nologo /RELEASE /OPT:REF /SUBSYSTEM:XBOX /MACHINE:X86 /FIXED:NO /DEBUG)
set(KOTOR_EXECUTABLE_KIND "")
