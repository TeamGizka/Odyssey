# PC: swkotor2.exe (Win32, OpenGL, Miles Sound System, Bink, DirectInput). Toolchain: cmake/toolchain-vc70.cmake.
set(KOTOR_COMPILE_OPTIONS /nologo /W3 /MT /O2 /EHsc /Gy)
set(KOTOR_COMPILE_DEFINITIONS WIN32 NDEBUG _WINDOWS)
set(KOTOR_LINK_OPTIONS /nologo /RELEASE /OPT:REF /SUBSYSTEM:WINDOWS /MACHINE:X86)
set(KOTOR_EXECUTABLE_KIND WIN32)

# Each source is compiled in its own directory under its file name alone, as the project build does: __FILE__ is
# "file.cpp", not the absolute path.
foreach(lang C CXX)
  set(CMAKE_${lang}_COMPILER_LAUNCHER "${CMAKE_COMMAND}" -P "${CMAKE_SOURCE_DIR}/cmake/SourceDirectoryCompile.cmake" --)
endforeach()
