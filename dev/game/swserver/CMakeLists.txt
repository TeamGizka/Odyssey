# swserver: experimental dedicated server (KOTOR_BUILD_SWSERVER). Not part of the shipped game.
kotor_executable(swserver
  main.cpp
  appmanager.cpp
  nullclient.cpp
  lineage/netlayer.cpp
  lineage/serverexoapp.cpp)

set_target_properties(swserver PROPERTIES WIN32_EXECUTABLE OFF)
target_link_options(swserver PRIVATE /SUBSYSTEM:CONSOLE "/MAP:${CMAKE_CURRENT_BINARY_DIR}/swserver.map")

# The server links whole engine and game libraries, in this order.
set(libraries exoresman math3d worldtimer servercore pathfindinfo nwscript netlayer exobase clientcore namegen)
foreach(library IN LISTS libraries)
  add_library(swserver_${library} STATIC $<TARGET_OBJECTS:${library}>)
  set_target_properties(swserver_${library} PROPERTIES LINKER_LANGUAGE CXX)
  target_link_libraries(swserver PRIVATE swserver_${library})
endforeach()
target_link_libraries(swserver PRIVATE kernel32.lib user32.lib ole32.lib version.lib)
