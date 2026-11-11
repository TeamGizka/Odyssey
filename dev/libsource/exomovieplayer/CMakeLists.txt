# exomovieplayer library.
set(sources
  exomovieplayer.cpp
  exomovieplayerinternal.cpp
  exomovieplayerinternalxbox.cpp)
if(KOTOR_PLATFORM STREQUAL "pc")
  # Xbox only
  list(REMOVE_ITEM sources
    exomovieplayerinternalxbox.cpp)
elseif(KOTOR_PLATFORM STREQUAL "xbox")
  # PC only
  list(REMOVE_ITEM sources
    exomovieplayerinternal.cpp)
endif()
kotor_library(exomovieplayer ${sources})

target_compile_options(exomovieplayer PRIVATE /Ob1 /YX)

kotor_automatic_pch_order(exomovieplayer)
