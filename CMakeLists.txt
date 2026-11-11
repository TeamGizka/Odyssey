cmake_minimum_required(VERSION 3.25)

# The game uses the static multithreaded C runtime (/MT); it is passed explicitly with the other compiler options.
set(CMAKE_MSVC_RUNTIME_LIBRARY "")
project(KotOR LANGUAGES C CXX)

# Every compiler and linker option is spelled out by the platform settings; CMake's defaults are cleared.
foreach(lang C CXX)
  set(CMAKE_${lang}_FLAGS "")
  set(CMAKE_${lang}_FLAGS_RELEASE "")
  set(CMAKE_${lang}_STANDARD_LIBRARIES "")
endforeach()
set(CMAKE_EXE_LINKER_FLAGS "")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "")

find_package(Python3 REQUIRED COMPONENTS Interpreter)

# Target platform: "pc" (swkotor.exe, the default) or "xbox" (selected by cmake/toolchain-xdk.cmake).
set(KOTOR_PLATFORMS pc xbox)
if(NOT DEFINED KOTOR_PLATFORM)
  set(KOTOR_PLATFORM "pc")
endif()
if(NOT KOTOR_PLATFORM IN_LIST KOTOR_PLATFORMS)
  message(FATAL_ERROR "KOTOR_PLATFORM '${KOTOR_PLATFORM}' is not supported (supported: ${KOTOR_PLATFORMS})")
endif()
include(cmake/platform/${KOTOR_PLATFORM}.cmake)

# ON: swkotor.exe is byte-identical to the 1.03 release (adds generated, unreferenced filler functions to swmain).
# OFF: swkotor.exe is built from the source tree alone. See cmake/keeperfill/README.md.
option(KOTOR_KEEPER_FILL "Generate the linker filler block for swmain" ON)
option(KOTOR_BUILD_SWSERVER "Build the experimental dedicated server (dev/game/swserver)" OFF)

include(cmake/KotorTargets.cmake)
include(cmake/PrecompiledHeaders.cmake)
include(cmake/keeperfill/KeeperFill.cmake)

add_subdirectory(3rdparty/NvTriStrip)
add_subdirectory(dev)

if(KOTOR_PLATFORM STREQUAL "pc")
  include(cmake/release/Release.cmake)
endif()
