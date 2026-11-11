# exoinput library.
set(sources
  exoinput.cpp
  exoinputinternal.cpp
  exoimeui.cpp
  exorawinputwin32.cpp
  exorawinputxbox.cpp)
if(KOTOR_PLATFORM STREQUAL "pc")
  # Xbox only
  list(REMOVE_ITEM sources
    exorawinputxbox.cpp)
elseif(KOTOR_PLATFORM STREQUAL "xbox")
  # PC only
  list(REMOVE_ITEM sources
    exorawinputwin32.cpp)
endif()
kotor_library(exoinput ${sources})

target_compile_options(exoinput PRIVATE /YX)

kotor_automatic_pch_order(exoinput)
