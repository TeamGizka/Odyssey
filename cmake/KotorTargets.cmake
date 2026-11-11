# Target helpers for the project directories under dev/.

# kotor_library(<name> <source>...)
#   An engine or game library, compiled with the platform settings.
#   On PC it is an object library: swmain packs the objects into the static libraries it links (kotor_archive),
#   because a few of those libraries hold objects compiled in another project. Elsewhere it is a static library.
function(kotor_library name)
  if(KOTOR_PLATFORM STREQUAL "pc")
    add_library(${name} OBJECT ${ARGN})
  else()
    add_library(${name} STATIC ${ARGN})
  endif()
  kotor_target_settings(${name})
endfunction()

# kotor_executable(<name> <source>...)
function(kotor_executable name)
  add_executable(${name} ${KOTOR_EXECUTABLE_KIND} ${ARGN})
  target_link_options(${name} PRIVATE ${KOTOR_LINK_OPTIONS})
  kotor_target_settings(${name})
endfunction()

function(kotor_target_settings name)
  target_compile_options(${name} PRIVATE ${KOTOR_COMPILE_OPTIONS})
  target_compile_definitions(${name} PRIVATE ${KOTOR_COMPILE_DEFINITIONS})
  target_link_libraries(${name} PUBLIC kotor_includes)
endfunction()

# kotor_archive(<name> OBJECTS <library>/<source>... [IMPORT_LIBRARIES <lib>...])
#   The static library <name>.lib (target lib_<name>) holding the listed objects, each named by the kotor_library
#   that compiles it and its source file. The members are stored in the listed order: LIB stores the objects of one
#   command line in reverse, so they are passed reversed. IMPORT_LIBRARIES are merged into the library.
function(kotor_archive name)
  cmake_parse_arguments(ARG "" "" "OBJECTS;IMPORT_LIBRARIES" ${ARGN})
  set(objects "")
  set(libraries "")
  foreach(unit IN LISTS ARG_OBJECTS)
    get_filename_component(library "${unit}" DIRECTORY)
    get_filename_component(file "${unit}" NAME)
    string(REPLACE "." "[.]" file_regex "${file}")
    list(APPEND objects "$<FILTER:$<TARGET_OBJECTS:${library}>,INCLUDE,/${file_regex}[.]obj$>")
    list(APPEND libraries ${library})
  endforeach()
  list(REVERSE objects)
  list(REMOVE_DUPLICATES libraries)
  add_library(lib_${name} STATIC ${objects})
  set_target_properties(lib_${name} PROPERTIES LINKER_LANGUAGE CXX OUTPUT_NAME "${name}")
  add_dependencies(lib_${name} ${libraries})
  if(ARG_IMPORT_LIBRARIES)
    set_target_properties(lib_${name} PROPERTIES STATIC_LIBRARY_OPTIONS "${ARG_IMPORT_LIBRARIES}")
  endif()
endfunction()
