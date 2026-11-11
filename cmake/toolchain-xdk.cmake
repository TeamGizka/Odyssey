# Toolchain: Xbox XDK with its Visual C++ 7.1 compiler (xbox/bin/vc71: cl 13.10, link 7.10). nmake comes from
# Visual Studio .NET 2002.
#
# The environment variable XDK_ROOT names the XDK install (the directory containing xbox/bin, xbox/include and
# xbox/lib); VS2002_ROOT is described in cmake/toolchain-vc70.cmake.

foreach(var XDK_ROOT VS2002_ROOT)
  if("$ENV{${var}}" STREQUAL "")
    message(FATAL_ERROR "Set the environment variable ${var} (see cmake/toolchain-xdk.cmake).")
  endif()
endforeach()
file(TO_CMAKE_PATH "$ENV{XDK_ROOT}" XDK_ROOT)

set(XDK_BIN "${XDK_ROOT}/xbox/bin")
set(XDK_VC_BIN "${XDK_BIN}/vc71")
if(NOT EXISTS "${XDK_VC_BIN}/cl.exe")
  message(FATAL_ERROR "XDK_ROOT is '${XDK_ROOT}', but ${XDK_VC_BIN}/cl.exe does not exist.")
endif()

set(KOTOR_PLATFORM "xbox")

set(CMAKE_SYSTEM_NAME Windows)  # MSVC command-line conventions; the target is the Xbox
set(CMAKE_SYSTEM_PROCESSOR x86)

set(CMAKE_C_COMPILER   "${XDK_VC_BIN}/cl.exe")
set(CMAKE_CXX_COMPILER "${XDK_VC_BIN}/cl.exe")
set(CMAKE_LINKER       "${XDK_VC_BIN}/link.exe")
set(CMAKE_AR           "${XDK_VC_BIN}/lib.exe")
set(CMAKE_MT           "")

# Headers: the XDK's CRT, STL and Xbox API headers only. Libraries are found through LIB (CMakePresets.json).
set(XDK_INCLUDES "${XDK_ROOT}/xbox/include")
set(CMAKE_C_STANDARD_INCLUDE_DIRECTORIES   ${XDK_INCLUDES})
set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${XDK_INCLUDES})

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
