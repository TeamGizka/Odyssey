# servercore library.
set(sources
  serverexoapp.cpp
  serverexoappinternal.cpp
  nwsareapathfind.cpp
  nwsmodule.cpp
  nwsobject.cpp
  nwscombatround.cpp
  nwsscriptevent.cpp
  nwseffectlist.cpp
  nwsitemproperty.cpp
  nwscreature.cpp
  nwsforcedaction.cpp
  nwseffect.cpp
  nwsarea.cpp
  nwscreatureai.cpp
  nwshandleplayermessage.cpp
  nwshandleserveradminmessage.cpp
  globalvartables.cpp
  factionmanager.cpp
  nwvmachinecommands.cpp
  serveraimaster.cpp
  nwscreaturestats.cpp
  nwsspellimpact.cpp
  nwsdoor.cpp
  nwsplayer.cpp
  partytables.cpp
  nwssendplayermessage.cpp
  nwsmessage.cpp
  swsareamap.cpp
  swsroom.cpp
  nwssendserveradminmessage.cpp
  nwsobjectai.cpp
  swroomsurfacemesh.cpp
  swcollisionmesh.cpp
  nwsitem.cpp
  nwsplaceable.cpp
  nwstrigger.cpp
  nwswaypoint.cpp
  nwsencounter.cpp
  nwsvartable.cpp
  swvartable.cpp
  nwsexpression.cpp
  nwsdialog.cpp
  nwinventory.cpp
  nwscreatureaicombat.cpp
  nwsparty.cpp
  nwsjournal.cpp
  nwsstore.cpp
  nwssoundobject.cpp
  nwsambientsound.cpp
  nwsplayerluo.cpp
  swmgvmachinecommands.cpp
  nwsclient.cpp
  nwsrules.cpp
  nwssysadmin.cpp
  nwupdatestats.cpp
  ResBWM.cpp
  resmdl.cpp
  resncs.cpp)
if(KOTOR_PLATFORM STREQUAL "xbox")
  # Not built for Xbox yet
  list(REMOVE_ITEM sources
    serverexoappinternal.cpp nwsarea.cpp nwsencounter.cpp)
endif()
kotor_library(servercore ${sources})

kotor_source_pch(nwsobject.cpp math3d.h)
kotor_source_pch(nwscreature.cpp nwsobject.h)
kotor_source_pch(nwssendplayermessage.cpp nwcanimbase.h)
kotor_source_pch(swroomsurfacemesh.cpp exobase.h)
kotor_project_pch(servercore HEADER stdafx.h SOURCE stdafx.cpp
  EXCLUDE
    serverexoappinternal.cpp nwsareapathfind.cpp nwsmodule.cpp nwseffectlist.cpp nwsarea.cpp nwscreatureai.cpp
    nwshandleplayermessage.cpp nwvmachinecommands.cpp nwscreaturestats.cpp nwsspellimpact.cpp nwsplayer.cpp
    swcollisionmesh.cpp nwsitem.cpp nwsplaceable.cpp nwstrigger.cpp nwsencounter.cpp nwsdialog.cpp
    nwscreatureaicombat.cpp nwsplayerluo.cpp)
