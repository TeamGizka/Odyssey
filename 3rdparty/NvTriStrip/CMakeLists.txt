# NVIDIA NvTriStrip, linked statically into the game.
# VertexCache.cpp is not compiled: VertexCache.h defines the same functions inline.
add_library(NvTriStrip STATIC original/src/NvTriStrip.cpp original/src/NvTriStripObjects.cpp)
target_include_directories(NvTriStrip PUBLIC original/include PRIVATE original/src)
target_compile_options(NvTriStrip PRIVATE ${KOTOR_COMPILE_OPTIONS})
target_compile_definitions(NvTriStrip PRIVATE ${KOTOR_COMPILE_DEFINITIONS})
# The library sources include <windows.h>; on Xbox a private directory maps it to <xtl.h>.
if(KOTOR_PLATFORM STREQUAL "xbox")
  target_include_directories(NvTriStrip BEFORE PRIVATE xbox)
endif()
