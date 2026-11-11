# AURORA library.
if(KOTOR_PLATFORM STREQUAL "pc")
  set(sources
    aurtexman.cpp
    aurglcore.cpp
    aurpostprocess.cpp
    aurmodel.cpp
    aurconsole.cpp
    aurfont.cpp
    aurinternal.cpp
    aurora.cpp
    aurscene.cpp
    aurgui.cpp
    aurcamera.cpp
    aurimage.cpp
    aurglext.cpp
    aurtexcontrol.cpp
    aurmodelparse.cpp
    aurscenemanager.cpp
    aurrenderinterfacecore.cpp
    aurmodelrender.cpp
    aurprimitive.cpp
    auraticore.cpp
    aurmaterial.cpp
    aurtristripper.cpp
    auranim.cpp
    aurskin.cpp
    auremitter.cpp
    aurtrigger.cpp
    aurbehavior.cpp
    aurio.cpp
    aurStringFont.cpp
    aurvertexprog.cpp
    aurlightmanager.cpp
    aurwindmanager.cpp
    aurgrass.cpp
    aurvertexcache.cpp
    aurlite.cpp
    aurString.cpp)
else()
  set(sources
    aurmodel.cpp
    aurconsole.cpp
    aurmodelparse.cpp
    aurtexcontrol.cpp
    aurscenemanager.cpp
    auranim.cpp
    aurio.cpp
    aurdxcore.cpp
    aurimage.cpp
    aurlite.cpp
    aurrenderinterfacecore.cpp
    aurStringFont.cpp
    aurtristripper.cpp
    aurvertexcache.cpp
    aurwindmanager.cpp
    aurString.cpp)
endif()
kotor_library(AURORA ${sources})

target_include_directories(AURORA BEFORE PRIVATE "${KOTOR_DEV}/libsource/math3d")
if(KOTOR_PLATFORM STREQUAL "xbox")
  target_compile_options(AURORA PRIVATE /Ob1)
endif()
set_source_files_properties(
  aurpostprocess.cpp aurcamera.cpp aurtexcontrol.cpp aurmodelparse.cpp aurtristripper.cpp aurskin.cpp aurio.cpp
  aurlightmanager.cpp aurwindmanager.cpp aurgrass.cpp
  PROPERTIES COMPILE_OPTIONS "/YX")

kotor_source_pch(aurtexman.cpp aurconsole.h)
kotor_source_pch(aurglcore.cpp aurglext.h)
kotor_source_pch(aurmodel.cpp math.h)
kotor_source_pch(aurscene.cpp aurrendergl.h)
kotor_source_pch(aurscenemanager.cpp aurconsole.h)
kotor_source_pch(aurmodelrender.cpp aurmodel.h)
kotor_source_pch(auremitter.cpp exoresfile.h)
kotor_project_pch(AURORA HEADER stdafx.h SOURCE stdafx.cpp
  EXCLUDE
    aurpostprocess.cpp aurconsole.cpp aurfont.cpp aurora.cpp aurgui.cpp aurcamera.cpp aurglext.cpp aurtexcontrol.cpp
    aurmodelparse.cpp aurtristripper.cpp aurskin.cpp aurio.cpp aurlightmanager.cpp aurwindmanager.cpp aurgrass.cpp)
kotor_automatic_pch_order(AURORA)
