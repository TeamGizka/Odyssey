# pathfindinfo library.
kotor_library(pathfindinfo
  pathfindinfo.cpp)

target_compile_options(pathfindinfo PRIVATE /YX)

kotor_project_pch(pathfindinfo HEADER stdafx.h SOURCE stdafx.cpp)
