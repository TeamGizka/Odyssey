# exosound library.
set(sources
  exosound.cpp
  exosoundinternal.cpp
  exosoundsourceinternal.cpp
  exostreamingsoundsourceinternal.cpp
  exoxboxstream.cpp
  exostreamingsoundsourceinternalxbox.cpp)
if(KOTOR_PLATFORM STREQUAL "pc")
  # Xbox only
  list(REMOVE_ITEM sources
    exosound.cpp exoxboxstream.cpp exostreamingsoundsourceinternalxbox.cpp)
elseif(KOTOR_PLATFORM STREQUAL "xbox")
  # PC only
  list(REMOVE_ITEM sources
    exostreamingsoundsourceinternal.cpp)
endif()
kotor_library(exosound ${sources})

target_compile_options(exosound PRIVATE /YX)

kotor_project_pch(exosound HEADER exosoundinternal.h SOURCE exosound.cpp
  EXCLUDE
    exosoundinternal.cpp exosoundsourceinternal.cpp exostreamingsoundsourceinternal.cpp)
kotor_automatic_pch_order(exosound)
