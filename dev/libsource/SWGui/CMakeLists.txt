# SWGui library.
kotor_library(SWGui
  swgui.cpp
  swguidrawable.cpp
  swguicontrol.cpp
  swguidynamic.cpp)

target_compile_options(SWGui PRIVATE /Ob1)
set_source_files_properties(swguicontrol.cpp PROPERTIES COMPILE_OPTIONS "/YX")

kotor_source_pch(swguicontrol.cpp appmanager.h)
kotor_project_pch(SWGui HEADER stdafx.h SOURCE stdafx.cpp)
