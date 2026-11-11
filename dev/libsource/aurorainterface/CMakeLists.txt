# aurorainterface library.
set(sources
  aurorainterface.cpp
  aurlayeredtexture.cpp
  aurorainterfaceinternal.cpp
  aurtexture.cpp
  aurvisibility.cpp
  aurprocessedtexture.cpp
  aurtxi.cpp
  _aurmodel.cpp
  aurmodelx.cpp
  aurcompressedtexture.cpp
  aur16bittexture.cpp
  resplt.cpp
  resdds.cpp
  res4pc.cpp
  resmdx.cpp
  restxi.cpp
  resvis.cpp
  aurfonttexture.cpp
  resfnt.cpp)
if(KOTOR_PLATFORM STREQUAL "xbox")
  # Not built for Xbox yet
  list(REMOVE_ITEM sources
    aurorainterface.cpp)
endif()
kotor_library(aurorainterface ${sources})

target_compile_options(aurorainterface PRIVATE /YX)

kotor_project_pch(aurorainterface HEADER stdafx.h SOURCE stdafx.cpp)
