# math3d library.
kotor_library(math3d
  math3d.cpp
  bsp.cpp)

target_compile_options(math3d PRIVATE /Ob1 /YX)

kotor_automatic_pch_order(math3d)
