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
if(KOTOR_PLATFORM STREQUAL "xbox")
  # Xbox: the Bink for Xbox headers (3rdparty/Bink/xbox-1.6b, see its MANIFEST.txt) come before the PC Bink headers.
  # No /Ob1 here: the retail Xbox movie player inlines CExoArrayList's constructor into CExoMoviePlayerInternal().
  set(KOTOR_XBOX_BINK_DIR "${THIRDPARTY}/Bink/xbox-1.6b" CACHE PATH "Bink for Xbox SDK directory")
  target_include_directories(exomovieplayer BEFORE PRIVATE "${KOTOR_XBOX_BINK_DIR}/include")
  target_compile_options(exomovieplayer PRIVATE /YX)
else()
  target_compile_options(exomovieplayer PRIVATE /Ob1 /YX)
endif()

kotor_automatic_pch_order(exomovieplayer)
