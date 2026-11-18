# math3d library.
kotor_library(math3d
  math3d.cpp
  bsp.cpp)

if(NOT KOTOR_PLATFORM STREQUAL "xbox")
  # Xbox: no /Ob1. The retail Xbox math3d inlines Vector::Normalize into RotationArc and DistanceBetweenLines.
  target_compile_options(math3d PRIVATE /Ob1)
endif()
set_source_files_properties(bsp.cpp PROPERTIES COMPILE_OPTIONS /YX)

kotor_automatic_pch_order(math3d)
