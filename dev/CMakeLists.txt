# The game tree: engine libraries (libsource), game layers and executables (game), utilities.
set(KOTOR_DEV "${CMAKE_CURRENT_SOURCE_DIR}")

# SDK files the game is built against.
set(THIRDPARTY "${CMAKE_SOURCE_DIR}/3rdparty")
set(MILES_DIR "${THIRDPARTY}/Miles/6.5h")
set(MILES_61C_DIR "${THIRDPARTY}/Miles/6.1c")
set(BINK_DIR "${THIRDPARTY}/Bink/1.5v")

# Include directories shared by every project.
add_library(kotor_includes INTERFACE)
target_include_directories(kotor_includes INTERFACE
  "${KOTOR_DEV}/include"
  "${KOTOR_DEV}/game/clientcore/include"
  "${KOTOR_DEV}/game/servercore/include"
  "${KOTOR_DEV}/libsource/exobase/include"
  "${KOTOR_DEV}/libsource/exoinput/include"
  "${KOTOR_DEV}/libsource/exoresman/include"
  "${KOTOR_DEV}/libsource/exosound/include"
  "${KOTOR_DEV}/libsource/nwscript/include"
  "${KOTOR_DEV}/libsource/AURORA"
  "${KOTOR_DEV}/libsource/math3d"
  "${MILES_DIR}/include"
  "${BINK_DIR}/include")
if(KOTOR_PLATFORM STREQUAL "pc")
  target_include_directories(kotor_includes INTERFACE "${THIRDPARTY}/NvTriStrip/original/include")
endif()

add_subdirectory(libsource/exobase)
add_subdirectory(libsource/exoresman)
add_subdirectory(libsource/exoinput)
add_subdirectory(libsource/exosound)
add_subdirectory(libsource/exomovieplayer)
add_subdirectory(libsource/math3d)
add_subdirectory(libsource/namegen)
add_subdirectory(libsource/netlayer)
add_subdirectory(libsource/nwscript)
add_subdirectory(libsource/pathfindinfo)
add_subdirectory(libsource/worldtimer)
add_subdirectory(libsource/AURORA)
add_subdirectory(libsource/aurorainterface)
add_subdirectory(libsource/SWGui)
add_subdirectory(game/clientcore)
add_subdirectory(game/servercore)
add_subdirectory(game/swmain)
if(KOTOR_PLATFORM STREQUAL "pc")
  add_subdirectory(utilities/swstub)
endif()
if(KOTOR_BUILD_SWSERVER AND KOTOR_PLATFORM STREQUAL "pc")
  add_subdirectory(game/swserver)
endif()
