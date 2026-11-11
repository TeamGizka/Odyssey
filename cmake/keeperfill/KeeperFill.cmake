# kotor_keeper_fill(<target> <source>...)
#   Generates keeperfill_appmanager.inc and keeperfill_main.inc, which the sources of swmain include, in the build
#   tree: the filler block when KOTOR_KEEPER_FILL is ON, empty files otherwise. See README.md in this directory.
function(kotor_keeper_fill target)
  set(generator "${CMAKE_SOURCE_DIR}/cmake/keeperfill/generate.py")
  set(keys "${CMAKE_SOURCE_DIR}/cmake/keeperfill/keys.json")
  if(KOTOR_KEEPER_FILL)
    set(directory "${CMAKE_CURRENT_BINARY_DIR}/keeperfill/on")
    set(mode "")
  else()
    set(directory "${CMAKE_CURRENT_BINARY_DIR}/keeperfill/off")
    set(mode --empty)
  endif()
  set(outputs "${directory}/keeperfill_appmanager.inc" "${directory}/keeperfill_main.inc")
  add_custom_command(OUTPUT ${outputs}
    COMMAND "${Python3_EXECUTABLE}" "${generator}" ${mode} "${directory}"
    DEPENDS "${generator}" "${keys}"
    COMMENT "Generating the swmain filler block (KOTOR_KEEPER_FILL=${KOTOR_KEEPER_FILL})"
    VERBATIM)
  target_sources(${target} PRIVATE ${outputs})
  target_include_directories(${target} PRIVATE "${directory}")
  set_property(SOURCE ${ARGN} APPEND PROPERTY OBJECT_DEPENDS ${outputs})
endfunction()
