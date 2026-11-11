# exoresman library.
set(sources
  exoresman.cpp
  exoresfile.cpp
  resgff.cpp
  twodimarray.cpp
  exotlktable.cpp
  nwerffile.cpp
  lyt.cpp
  lip.cpp
  restga.cpp
  restpc.cpp
  exoresgffread.cpp
  res2da.cpp
  reslip.cpp
  reslyt.cpp
  reswave.cpp
  exoresfileinfo.cpp)
if(KOTOR_PLATFORM STREQUAL "xbox")
  # Not built for Xbox yet
  list(REMOVE_ITEM sources
    exoresman.cpp reswave.cpp)
endif()
kotor_library(exoresman ${sources})

target_compile_options(exoresman PRIVATE /YX)

kotor_automatic_pch_order(exoresman)
