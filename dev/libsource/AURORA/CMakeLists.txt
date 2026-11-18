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
    aurtexman.cpp
    aurdxcore.cpp
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
    aurtexcontrol.cpp
    aurpixelshader.cpp
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
endif()
kotor_library(AURORA ${sources})

target_include_directories(AURORA BEFORE PRIVATE "${KOTOR_DEV}/libsource/math3d")

if(KOTOR_PLATFORM STREQUAL "xbox")
  # The pixel shader table, in table order: each psh/<Name>.psh is assembled by the XDK shader assembler and its
  # output turned into one element of the D3DPIXELSHADERDEF array aurpixelshader.cpp includes.
  set(pixel_shaders
    Blend TextureModulate NormalMapModulate BumpDiffuse BumpSpecularPower BumpSpecular BumpSelfShadow NormalMapLight
    BumpEnvironment ThreeTextureAdd FourTextureAdd Texture FourTextureWeighted DepthSprite DesatBumpmap
    DesatBumpyShiny Desat DesatEmlm DesatEm FglowBumpyShiny DesatNoiseSub DesatNoiseAdd DesatNoiseAddEm
    DesatNoiseAddEmlm DesatNoiseAddLm DesatNoiseSubEm DesatNoiseSubEmlm DesatNoiseSubLm DesatNoiseMod DesatNoiseModEm
    DesatNoiseModEmlm DesatNoiseModLm FglowBumpmap DependentLookup DistortionDepthTest Environment EnvironmentLightMap
    FglowEm FglowEmlm NoiseEmlmAdd Fglow NoiseEmlmSubtract Add Average Fog Distortion NoiseLmModulate
    DistortionDepthTestBias Holo HoloDesat Illum IllumDesatNoise IllumDesatNoiseAdd IllumDesatNoiseMod IllumDesatEmlm
    IllumDesatNoiseSub IllumEnv IllumNoiseAdd IllumNoiseLmModulate IllumNoiseEmModulate IllumNoiseEmAdd
    IllumNoiseEmlmSubtract IllumNoiseLmSubtract IllumDesatNoiseSubEm IllumDesatNoiseAddEm IllumDesatNoiseAddLm
    IllumDesatNoiseSubLm IllumDesatNoiseModEm IllumDesatNoiseModLm LightMap LightMapOnly TextureColorFog LightMapAdd
    NoiseSubtract NoiseLmSubtract NoiseEmSubtract EnvironmentSubtract NoiseEmModulate NoiseModulate
    TextureModulateAlpha NoiseAdd NoiseLmAdd NoiseEmAdd ColorLookupAlpha ColorLookup BumpLightMap ColorMatrix White)
  set(pixel_shader_dir "${CMAKE_CURRENT_BINARY_DIR}/psh")
  set(pixel_shader_script "${CMAKE_SOURCE_DIR}/cmake/PixelShaderInitializer.cmake")
  file(MAKE_DIRECTORY "${pixel_shader_dir}")
  set(pixel_shader_initializers "")
  foreach(shader IN LISTS pixel_shaders)
    set(source "${CMAKE_CURRENT_SOURCE_DIR}/psh/${shader}.psh")
    add_custom_command(
      OUTPUT "${pixel_shader_dir}/${shader}.inl"
      COMMAND "${XDK_BIN}/xsasm.exe" /nologo /h "${source}" "${pixel_shader_dir}/${shader}.h"
      COMMAND "${CMAKE_COMMAND}" "-DINPUT=${pixel_shader_dir}/${shader}.h" "-DOUTPUT=${pixel_shader_dir}/${shader}.inl"
              -P "${pixel_shader_script}"
      DEPENDS "${source}" "${pixel_shader_script}"
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/psh"
      VERBATIM)
    list(APPEND pixel_shader_initializers "${pixel_shader_dir}/${shader}.inl")
  endforeach()
  target_sources(AURORA PRIVATE ${pixel_shader_initializers})
  set_source_files_properties(aurpixelshader.cpp PROPERTIES OBJECT_DEPENDS "${pixel_shader_initializers}")
  target_include_directories(AURORA PRIVATE "${pixel_shader_dir}")
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
