# namegen library.
kotor_library(namegen
  nwnamegen.cpp
  resltr.cpp)

target_compile_options(namegen PRIVATE /YX)

kotor_automatic_pch_order(namegen)
