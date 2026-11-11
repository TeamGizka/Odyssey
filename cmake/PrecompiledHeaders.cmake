# Precompiled headers, as the Visual C++ 7.0 project settings spell them. PC only: the functions do nothing elsewhere.
#
# kotor_source_pch(<source> <header>)
#   "Create precompiled header through <header>" for one file, which gets a .pch of its own.
#
# kotor_project_pch(<target> HEADER <header> SOURCE <source> [EXCLUDE <source>...])
#   "Use precompiled header through <header>" for the project: SOURCE is added to the target and creates
#   <target>.pch; every other source uses it with the header as a forced include. Sources in EXCLUDE and sources
#   given a .pch of their own by kotor_source_pch keep their settings.
#
# kotor_automatic_pch_order(<target>)
#   Sources compiled with "Automatically generate" (/YX) share one vc70.pch per project: the first one compiled
#   creates it. They are compiled in alphabetical order, the order of the IDE's file list.
#
# Call them after the per-source compile options of the directory are set, in the order above.

function(kotor_source_pch source header)
  if(NOT KOTOR_PLATFORM STREQUAL "pc")
    return()
  endif()
  get_filename_component(stem "${source}" NAME_WE)
  _kotor_pch_options("${source}" "/Yc${header}" "/Fp${CMAKE_CURRENT_BINARY_DIR}/${stem}.pch")
endfunction()

function(kotor_project_pch target)
  if(NOT KOTOR_PLATFORM STREQUAL "pc")
    return()
  endif()
  cmake_parse_arguments(ARG "" "HEADER;SOURCE" "EXCLUDE" ${ARGN})
  set(pch "${CMAKE_CURRENT_BINARY_DIR}/${target}.pch")
  get_target_property(sources ${target} SOURCES)
  foreach(source IN LISTS sources)
    get_source_file_property(options "${source}" COMPILE_OPTIONS)
    if(source IN_LIST ARG_EXCLUDE OR options MATCHES "/Yc")
      continue()
    endif()
    _kotor_pch_options("${source}" "/FI${ARG_HEADER}" "/Yu${ARG_HEADER}" "/Fp${pch}")
    set_property(SOURCE "${source}" APPEND PROPERTY OBJECT_DEPENDS "${pch}")
  endforeach()
  set(creator "${CMAKE_CURRENT_SOURCE_DIR}/${ARG_SOURCE}")
  target_sources(${target} PRIVATE "${creator}")
  set_source_files_properties("${creator}" PROPERTIES
    COMPILE_OPTIONS "/Yc${ARG_HEADER};/Fp${pch}"
    OBJECT_OUTPUTS "${pch}")
endfunction()

function(kotor_automatic_pch_order target)
  if(NOT KOTOR_PLATFORM STREQUAL "pc")
    return()
  endif()
  get_target_property(target_options ${target} COMPILE_OPTIONS)
  get_target_property(sources ${target} SOURCES)
  set(automatic "")
  foreach(source IN LISTS sources)
    get_source_file_property(options "${source}" COMPILE_OPTIONS)
    if(NOT options)
      set(options "")
    endif()
    if(options MATCHES "/Y[cu]" OR NOT source MATCHES "[.](cpp|c)$")
      continue()
    endif()
    if("/YX" IN_LIST options OR "/YX" IN_LIST target_options)
      list(APPEND automatic "${source}")
    endif()
  endforeach()
  list(SORT automatic CASE INSENSITIVE)
  set(previous "")
  foreach(source IN LISTS automatic)
    if(previous)
      set_property(SOURCE "${source}" APPEND PROPERTY OBJECT_DEPENDS
        "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${target}.dir/${previous}.obj")
    endif()
    set(previous "${source}")
  endforeach()
endfunction()

# Replace a source's automatic precompiled header setting, if any, with the given options.
function(_kotor_pch_options source)
  get_source_file_property(options "${source}" COMPILE_OPTIONS)
  if(NOT options)
    set(options "")
  endif()
  list(REMOVE_ITEM options "/YX")
  list(APPEND options ${ARGN})
  set_source_files_properties("${source}" PROPERTIES COMPILE_OPTIONS "${options}")
endfunction()
