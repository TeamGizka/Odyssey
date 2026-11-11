# clientcore library.
set(sources
  clientexoappinternal.cpp
  swguiingameabilities.cpp
  guiingame.cpp
  ObjectLookupTable.cpp
  nwmessage.cpp
  gameobjectarray.cpp
  swparty.cpp
  serveroptiontable.cpp
  nwrules.cpp
  swminigame.cpp
  swguiingamecharacter.cpp
  swguiabilitieschargen.cpp
  nwarea.cpp
  nwcarea.cpp
  nwspell.cpp
  nwccreaturestats.cpp
  nwitem.cpp
  nwclass.cpp
  twodimarrays.cpp
  nwcjournal.cpp
  nwrace.cpp
  swroom.cpp
  nwcreaturelevelstats.cpp
  nwplaceablesurfacemesh.cpp
  nwdoorsurfacemesh.cpp
  swguimovie.cpp
  clientexoapp.cpp
  clientaimaster.cpp
  console.cpp
  nwccreature.cpp
  nwcanimbase.cpp
  clientoptions.cpp
  clientinput.cpp
  swguifade.cpp
  swguiingame.cpp
  swbehaviorcamera.cpp
  nwcobject.cpp
  nwcmodule.cpp
  nwcareaofeffectobject.cpp
  swguiingamejournal.cpp
  swcroom.cpp
  nwcitem.cpp
  nwchandlemessage.cpp
  nwcsoundobject.cpp
  swmgobject.cpp
  swmgtrackfollower.cpp
  swmgguns.cpp
  nwtlktable.cpp
  clientmsgsysadmin.cpp
  nwcsendmessage.cpp
  soundset.cpp
  nwportrait.cpp
  swplayercontrol.cpp
  swguiloadscreen.cpp
  swguimainmenu.cpp
  swguipazaak.cpp
  nwcplaceable.cpp
  nwcdoor.cpp
  swguimaininterfacepc2.cpp
  nwcambientsound.cpp
  swguiingamecredits.cpp
  nwcvisualeffect.cpp
  swbehaviorattachsound.cpp
  nwctrigger.cpp
  nwcmessage.cpp
  swguiingamemap.cpp
  nwcspellvisual.cpp
  nwccreatureappearance.cpp
  nwcanimbasewield.cpp
  nwcanimbasehead.cpp
  nwcanimbasetw.cpp
  nwcanimbaseprojectile.cpp
  nwcvisualeffectonobject.cpp
  swguidialog.cpp
  swguibarkbubble.cpp
  swguiingametutorialbox.cpp
  swguiingameoptions.cpp
  swguiingameinventory.cpp
  swguiingameequip.cpp
  swbehaviorcameradialog.cpp
  swguiSubCreateItem.cpp
  swguipartyselection.cpp
  swguiingamepause.cpp
  swguistore.cpp
  swguiupgrade.cpp
  swguiingameareatransition.cpp
  swguisaveload.cpp
  SWGuiQuestItem.cpp
  swguiskillflowchart.cpp
  swguiPowersFeatsSkillsDebugMenu.cpp
  swguiDebugLoadModule.cpp
  swguiCreateItem.cpp
  SWAccelerationControlers.cpp
  swbehaviorcameracombat.cpp
  nwcprojectile.cpp
  swmgbehavior.cpp
  swguiclassselection.cpp
  swguioptions.cpp
  swpazaak.cpp
  nwcanimbasedoor.cpp
  nwcanimbasevisualeffect.cpp
  swguiingamegameplay.cpp
  swguiingameautopause.cpp
  swguilevelupchargen.cpp
  swguimainchargen.cpp
  swguiinOPTKeyMapping.cpp
  swguileveluppanel.cpp
  swguicustompanel.cpp
  swguiquickpanel.cpp
  swguiquickorcustompanel.cpp
  swguipowerslvlup.cpp
  swguifeatschargen.cpp
  swguiskillschargen.cpp
  swguiportraitchargen.cpp
  swguinamechargen.cpp
  nwanimbase.cpp
  nwcanimbaseheadwield.cpp
  nwcanimbaseplaceable.cpp
  nwcanimbasetrigger.cpp
  nwcmappin.cpp
  nwfeat.cpp
  nwskill.cpp
  resdwk.cpp
  resssf.cpp
  swguiexamine.cpp
  swguiingamesolomodequery.cpp
  swpattern.cpp
  swguistartscreen.cpp
  swguilegalscreen.cpp
  swguimusic.cpp)
if(KOTOR_PLATFORM STREQUAL "pc")
  # Xbox only
  list(REMOVE_ITEM sources
    swguistartscreen.cpp)
elseif(KOTOR_PLATFORM STREQUAL "xbox")
  # PC only
  list(REMOVE_ITEM sources
    swguiinOPTKeyMapping.cpp)
  # Not built for Xbox yet
  list(REMOVE_ITEM sources
    clientexoappinternal.cpp swbehaviorcamera.cpp swguioptions.cpp swguiingameautopause.cpp)
endif()
kotor_library(clientcore ${sources})

if(KOTOR_PLATFORM STREQUAL "xbox")
  # /GS for every source except these
  set(selected ${sources})
  list(REMOVE_ITEM selected
    nwdoorsurfacemesh.cpp nwcareaofeffectobject.cpp nwcvisualeffectonobject.cpp swmgbehavior.cpp)
  set_source_files_properties(${selected} PROPERTIES COMPILE_OPTIONS "/GS")
endif()

kotor_source_pch(swguiingame.cpp swguiingame.h)
kotor_source_pch(swguipazaak.cpp exobase.h)
kotor_source_pch(nwcanimbasewield.cpp nwcitem.h)
kotor_project_pch(clientcore HEADER stdafx.h SOURCE stdafx.cpp
  EXCLUDE
    clientexoappinternal.cpp guiingame.cpp swminigame.cpp console.cpp nwccreature.cpp swbehaviorcamera.cpp
    nwcobject.cpp nwcvisualeffect.cpp swbehaviorcameradialog.cpp swguiSubCreateItem.cpp swguiskillflowchart.cpp
    SWAccelerationControlers.cpp swguifeatschargen.cpp)
