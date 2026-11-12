#include "clientexoapp.h"
#include "appmanager.h"
#include "aurcamera.h"
#include "clientexoappinternal.h"
#include "nwcmodule.h"
#include "serverexoapp.h"

extern const int CLIENTEXOAPP_UNREFERENCED_128 = 128;

#ifndef _XBOX
void CClientExoApp::BeginIntro()  // PC 0x00638540
{
    m_pcExoAppInternal->BeginIntro();
}
#endif
// PC 0x0062B870
void CClientExoApp::EndLoadBar()
{
    m_pcExoAppInternal->EndLoadBar();
}
// PC 0x00638550
CSWCJournal* CClientExoApp::GetQuestJournal()
{
    return m_pcExoAppInternal->m_pQuestJournal;
}
// PC 0x00638560
CSWCWorldJournal* CClientExoApp::GetWorldJournal()
{
    return m_pcExoAppInternal->m_pWorldJournal;
}
// PC 0x00638570
void CClientExoApp::JournalUpdated(int nType, int nIndex,
                                   const CExoString& sName)
{
    m_pcExoAppInternal->JournalUpdated(nType, nIndex, sName);
}
// PC 0x00638580
int CClientExoApp::GetObjectName(unsigned long oidObject, CExoString& sName)
{
    return m_pcExoAppInternal->GetObjectName(oidObject, sName);
}
// PC 0x00638590
void CClientExoApp::AddFeedbackMessage(unsigned short nType,
                                       CSWCCMessageData* pMessage,
                                       const CExoString& sString)
{
    if (sString == "")
    {
        CExoString sEmpty("");
        m_pcExoAppInternal->AddFeedbackMessage(nType, pMessage, sEmpty);
    }
    else
    {
        m_pcExoAppInternal->AddFeedbackMessage(nType, pMessage, sString);
    }
}
// PC 0x00638640
void CClientExoApp::SetCharacterFile(unsigned long nDataSize,
                                     unsigned char* pCharacterData,
                                     int nCharacterFileId)
{
    m_pcExoAppInternal->SetCharacterFile(nDataSize, pCharacterData,
                                       nCharacterFileId);
}
// PC 0x00638650
void CClientExoApp::DisplayMainMenu(int bUnknown)
{
    m_pcExoAppInternal->DisplayMainMenu(bUnknown);
}

#ifdef _XBOX
void CClientExoApp::DisplayStartScreen()
{
    m_pcExoAppInternal->DisplayStartScreen();
}
#endif
// PC 0x00638660
void CClientExoApp::UnloadModule()
{
    m_pcExoAppInternal->UnloadModule();
}
// PC 0x00638670
CSWCModule* CClientExoApp::CreateModule()
{
    return m_pcExoAppInternal->CreateModule();
}

#ifndef _XBOX
void CClientExoApp::ExitProgram()  // PC 0x00638680
{
    m_pcExoAppInternal->ExitProgram();
}
#endif
// PC 0x00638690
CExoFile* CClientExoApp::GetCaptureEventFile()
{
    return m_pcExoAppInternal->m_pCaptureEventFile;
}
// PC 0x006386A0
int CClientExoApp::GetCaptureEvents()
{
    return m_pcExoAppInternal->m_nCaptureEvents;
}
// PC 0x006386B0
int CClientExoApp::GetCaptureEventPlayBack()
{
    return m_pcExoAppInternal->m_nCaptureEventPlayBack;
}
// PC 0x006386C0
CExoString CClientExoApp::GetClientName()
{
    return m_pcExoAppInternal->m_sClientName;
}
// PC 0x006386F0
int CClientExoApp::GetClientLanguage()
{
    return m_pcExoAppInternal->m_nClientLanguage;
}
// PC 0x00638700
int CClientExoApp::IsClientImeUI()
{
    return m_pcExoAppInternal->m_bClientImeUI;
}
// PC 0x00638710
CExoImeUI* CClientExoApp::GetImeUI()
{
    return m_pcExoAppInternal->m_pImeUI;
}
// PC 0x00638720
CWorldTimer* CClientExoApp::GetWorldTimer()
{
    return m_pcExoAppInternal->m_pWorldTimer;
}
// PC 0x00638730
CWorldTimer* CClientExoApp::GetPauseTimer()
{
    return m_pcExoAppInternal->m_pPauseTimer;
}
// PC 0x00638740
CWorldTimer* CClientExoApp::GetAnimationTimer()
{
    return m_pcExoAppInternal->m_pAnimationTimer;
}
// PC 0x00638750
CConnectionLib* CClientExoApp::GetConnectionLib()
{
    return m_pcExoAppInternal->m_pConnectionLib;
}
// PC 0x00638760
CSWCModule* CClientExoApp::GetModule()
{
    return m_pcExoAppInternal->GetModule();
}
// PC 0x004065A0
CExoLocString* CClientExoApp::GetModuleDescription()
{
    return 0;
}
// PC 0x00638770
CSWCCreature* CClientExoApp::GetPlayerCreature()
{
    return m_pcExoAppInternal->GetPlayerCreature();
}
// PC 0x00638780
unsigned long CClientExoApp::GetPlayerCreatureId()
{
    return m_pcExoAppInternal->m_nPlayerCreatureId;
}
// PC 0x00638790
CAurCamera* CClientExoApp::GetModuleCamera()
{
    return m_pcExoAppInternal->GetModuleCamera();
}
// PC 0x006387A0
CGameObjectArray* CClientExoApp::GetGameObjectArray()
{
    return m_pcExoAppInternal->m_pGameObjectArray;
}
// PC 0x006387B0
CSWCObject* CClientExoApp::GetGameObject(unsigned long oidObject)
{
    return (CSWCObject*)m_pcExoAppInternal->GetGameObject(oidObject);
}
// PC 0x006387E0
CSWCPlayer* CClientExoApp::GetPlayerByGameObjectID(unsigned long oidObject)
{
    return m_pcExoAppInternal->GetPlayerByGameObjectID(oidObject);
}
// PC 0x006387F0
CSWCItem* CClientExoApp::GetItemByGameObjectID(unsigned long nId)
{
    return m_pcExoAppInternal->GetItemByGameObjectID(nId);
}
// PC 0x00638800
CSWCCreature* CClientExoApp::GetCreatureByGameObjectID(unsigned long nId)
{
    return m_pcExoAppInternal->GetCreatureByGameObjectID(nId);
}
// PC 0x00638810
CSWCArea* CClientExoApp::GetAreaByGameObjectID(unsigned long oidObject)
{
    return m_pcExoAppInternal->GetAreaByGameObjectID(oidObject);
}
// PC 0x00638820
CSWCTrigger* CClientExoApp::GetTriggerByGameObjectID(unsigned long oidObject)
{
    return m_pcExoAppInternal->GetTriggerByGameObjectID(oidObject);
}
// PC 0x00638830
CSWCProjectile* CClientExoApp::GetProjectileByGameObjectID(
    unsigned long oidObject)
{
    return m_pcExoAppInternal->GetProjectileByGameObjectID(oidObject);
}
// PC 0x00638840
CSWCPlaceable* CClientExoApp::GetPlaceableByGameObjectID(
    unsigned long oidObject)
{
    return m_pcExoAppInternal->GetPlaceableByGameObjectID(oidObject);
}
// PC 0x00638850
CSWCDoor* CClientExoApp::GetDoorByGameObjectID(unsigned long oidObject)
{
    return m_pcExoAppInternal->GetDoorByGameObjectID(oidObject);
}
// PC 0x00638860
CSWCAreaOfEffectObject* CClientExoApp::GetAreaOfEffectByGameObjectID(
    unsigned long oidObject)
{
    return m_pcExoAppInternal->GetAreaOfEffectByGameObjectID(oidObject);
}
// PC 0x00638870
CSWCSoundObject* CClientExoApp::GetSoundObjectByGameObjectID(
    unsigned long oidObject)
{
    return m_pcExoAppInternal->GetSoundObjectByGameObjectID(oidObject);
}

int CClientExoApp::GetInStartNewModule()
{
    return m_pcExoAppInternal->m_bInStartNewModule;
}
// PC 0x00638880
void CClientExoApp::SetInStartNewModule(int bInStartNewModule)
{
    m_pcExoAppInternal->m_bInStartNewModule = bInStartNewModule;
}
// PC 0x00638890
int CClientExoApp::GetPauseState(unsigned char nPauseType)
{
    return m_pcExoAppInternal->GetPauseState(nPauseType);
}
// PC 0x006388A0
void CClientExoApp::SetPauseState(unsigned char nPauseState, int bPause,
                                 CExoLinkedList<unsigned long>* pExclusionList)
{
    m_pcExoAppInternal->SetPauseState(nPauseState, bPause,
                                             pExclusionList);
}
// PC 0x006388B0
CWorldTimer* CClientExoApp::GetActiveTimer(unsigned long nTimer)
{
    return m_pcExoAppInternal->GetActiveTimer(nTimer);
}

#ifdef _XBOX
int CClientExoApp::SendSaveGameRequest(unsigned long nPlayerId,
                                       const CExoString& sSaveName)
{
    return m_pcExoAppInternal->SendSaveGameRequest(nPlayerId, sSaveName);
}
#else
int CClientExoApp::SendSaveGameRequest(unsigned long nPlayerId,  // PC 0x006388C0
                                       const CExoString& sSaveName,
                                       const CExoString& sSaveDir)
{
    return m_pcExoAppInternal->SendSaveGameRequest(nPlayerId, sSaveName,
                                                   sSaveDir);
}
#endif
// PC 0x006388D0
int CClientExoApp::SendLoadGameRequest(unsigned long nPlayerId,
                                       const CExoString& sSaveName,
                                       const CExoString& sSaveDir)
{
    return m_pcExoAppInternal->SendLoadGameRequest(nPlayerId, sSaveName,
                                                   sSaveDir);
}
// PC 0x006388E0
CGuiInGame* CClientExoApp::GetInGameGui()
{
    return m_pcExoAppInternal->m_pGuiInGame;
}
// PC 0x006388F0
int CClientExoApp::GetPlayModulePanel()
{
    return m_pcExoAppInternal->m_nPlayModulePanel;
}
// PC 0x00638900
CClientAIMaster* CClientExoApp::GetClientAIMaster()
{
    return m_pcExoAppInternal->m_pClientAIMaster;
}
// PC 0x00638910
CExoLinkedList<CSWCPlayer>* CClientExoApp::GetPlayerList()
{
    return &m_pcExoAppInternal->m_lstPlayerList;
}
// PC 0x00638920
CSWCPlayer* CClientExoApp::GetPlayerByPlayerID(unsigned long nPlayerId)
{
    return m_pcExoAppInternal->GetPlayerByPlayerID(nPlayerId);
}
// PC 0x00638930
CNetLayer* CClientExoApp::GetNetLayer()
{
    return m_pcExoAppInternal->m_pUnknown10;
}
// PC 0x00638940
CSWCMessage* CClientExoApp::GetSWCMessage()
{
    return m_pcExoAppInternal->m_pSWCMessage;
}
// PC 0x00638950
CClientOptions* CClientExoApp::GetClientOptions()
{
    return m_pcExoAppInternal->m_pClientOptions;
}
// PC 0x00638960
CServerInfo* CClientExoApp::GetServerInfo()
{
    return m_pcExoAppInternal->m_pServerInfo;
}
// PC 0x00638970
void CClientExoApp::AttachPlayerGameObjectToArea()
{
    m_pcExoAppInternal->AttachPlayerGameObjectToArea();
}
// PC 0x00638980
int CClientExoApp::InitializeInGameGUI()
{
    return m_pcExoAppInternal->InitializeInGameGUI();
}
// PC 0x00638990
void CClientExoApp::ServerModeChange(unsigned char nServerMode)
{
    m_pcExoAppInternal->ServerModeChange(nServerMode);
}
// PC 0x006389A0
void CClientExoApp::AddObjectToHitcheckIgnoreList(unsigned long oidObject)
{
    m_pcExoAppInternal->AddObjectToHitcheckIgnoreList(oidObject);
}
// PC 0x006389B0
void CClientExoApp::RemoveObjectFromHitcheckIgnoreList(unsigned long oidObject)
{
    m_pcExoAppInternal->RemoveObjectFromHitcheckIgnoreList(oidObject);
}
// PC 0x006389C0
void CClientExoApp::EmptyHitcheckIgnoreList()
{
    m_pcExoAppInternal->EmptyHitcheckIgnoreList();
}
// PC 0x006389D0
int CClientExoApp::SetPlayerCreature(unsigned long oidObject)
{
    return m_pcExoAppInternal->SetPlayerCreature(oidObject);
}
// PC 0x006389E0
int CClientExoApp::HandleMessage(unsigned long nPlayerId,
                                 unsigned char* pMessage, unsigned long nSize,
                                 int bGuaranteed)
{
    return m_pcExoAppInternal->HandleMessage(nPlayerId, pMessage, nSize,
                                             bGuaranteed);
}
// PC 0x006389F0
int CClientExoApp::ContinueMessageProcessing()
{
    return m_pcExoAppInternal->ContinueMessageProcessing();
}
// PC 0x00638A00
int CClientExoApp::Initialize()
{
    return m_pcExoAppInternal->Initialize();
}
// PC 0x00638A10
int CClientExoApp::IsClientInitialized()
{
    return m_pcExoAppInternal->m_bClientInitialized;
}
// PC 0x00638A20
int CClientExoApp::MainLoop()
{
    return m_pcExoAppInternal->MainLoop();
}
// PC 0x00638A30
int CClientExoApp::CreatureAcquireItem(unsigned long oidObject,
                                       const CResRef& cResRef)
{
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    oidObject = pServer->ClientToServerObjectId(oidObject);
    pServer = g_pAppManager->m_pServerExoApp;
    return pServer->CreatureAcquireItem(oidObject, cResRef);
}
// PC 0x00638A60
int CClientExoApp::Uninitialize()
{
    return m_pcExoAppInternal->Uninitialize();
}
// PC 0x00638A70
void CClientExoApp::SetCaptureEvents(const CExoString& sFileName)
{
    m_pcExoAppInternal->SetCaptureEvents(sFileName);
}
// PC 0x00638A80
void CClientExoApp::SetCaptureEventPlayBack(const CExoString& sFileName)
{
    m_pcExoAppInternal->SetCaptureEventPlayBack(sFileName);
}
// PC 0x00638A90
void CClientExoApp::SetCaptureEventPlayBack(int nRate)
{
    m_pcExoAppInternal->SetCaptureEventPlayBackRate(nRate);
}
// PC 0x00638AA0
int CClientExoApp::SetDebugMode(int nDebugMode)
{
    return m_pcExoAppInternal->SetDebugMode(nDebugMode);
}
#ifndef _XBOX
int CClientExoApp::StartServices(int nServices, const CExoString& sArg2,  // PC 0x00638AB0
                                 const CExoString& sArg3,
                                 const CExoString& sArg4, unsigned long nArg5)
{
    return m_pcExoAppInternal->StartServices(nServices, sArg2, sArg3, sArg4,
                                             nArg5);
}
#endif
void CClientExoApp::SetEventDescriptions()  // PC 0x00638AC0
{
    m_pcExoAppInternal->SetEventDescriptions();
}
// PC 0x00638AD0
void CClientExoApp::StopServices()
{
    m_pcExoAppInternal->StopServices();
}

#ifdef _XBOX
void CClientExoApp::ShutDownLegalScreen()
{
    m_pcExoAppInternal->ShutDownLegalScreen();
}
#endif
// PC 0x00638AE0
void CClientExoApp::ShutDownToMainMenu()
{
    m_pcExoAppInternal->ShutDownToMainMenu();
}
// PC 0x00638AF0
void CClientExoApp::SetApplicationIdsMatch(int bMatch)
{
    m_pcExoAppInternal->m_bApplicationIdsMatch = bMatch;
}
// PC 0x00638B00
CSWParty* CClientExoApp::GetSWParty()
{
    return m_pcExoAppInternal->m_pSWParty;
}
// PC 0x00638B10
int CClientExoApp::GetHitCheckMaterialMask()
{
    return m_pcExoAppInternal->m_nHitCheckMaterialMask;
}

#ifndef _XBOX
int CClientExoApp::SetVideoMode(unsigned long nVideoMode)  // PC 0x00638B20
{
    return m_pcExoAppInternal->SetVideoMode(nVideoMode, 0);
}
#endif
// PC 0x00638B40
int CClientExoApp::SetTexturePack(unsigned char nTexturePack)
{
    return m_pcExoAppInternal->SetTexturePack(nTexturePack, 1);
}
// PC 0x00638B60
CSoundSetList* CClientExoApp::GetSoundSetList()
{
    return m_pcExoAppInternal->m_pSoundSetList;
}
// PC 0x00638B70
CResRef CClientExoApp::GetItemDropSound(unsigned char nType)
{
    CClientExoAppInternal* pInternal = m_pcExoAppInternal;
    if (pInternal->m_pItemDropSounds != NULL)
    {
        return pInternal->m_pItemDropSounds[nType];
    }
    return CResRef("");
}
// PC 0x00638BC0
int CClientExoApp::GetLoadBarState()
{
    return m_pcExoAppInternal->m_nLoadBarState;
}

#ifndef _XBOX
void CClientExoApp::RestartMoviesIfNecessary()  // PC 0x00638BD0
{
    m_pcExoAppInternal->RestartMoviesIfNecessary();
}
#endif
// PC 0x00638BE0
int CClientExoApp::GetApplicationId()
{
    return g_pAppManager->m_nAppId;
}
// PC 0x00638BF0
void CClientExoApp::AddObjectModelDirectlyToIgnoreList(
    unsigned long oidObject, List<CAurObject*>* pIgnoreList)
{
    m_pcExoAppInternal->AddObjectModelDirectlyToIgnoreList(oidObject,
                                                                  pIgnoreList);
}
// PC 0x00638C00
void CClientExoApp::StartMenuMusic(int bCultTheme, int nVolume, int nPriorityGroup)
{
    m_pcExoAppInternal->StartMenuMusic(bCultTheme, nVolume, nPriorityGroup);
}
// PC 0x00638C10
void CClientExoApp::StopMenuMusic()
{
    m_pcExoAppInternal->StopMenuMusic();
}
// PC 0x00638C20
void CClientExoApp::StopLoadMusic()
{
    m_pcExoAppInternal->StopLoadMusic();
}
// PC 0x00638C40
void CClientExoApp::SetStartMenuMusicOnMovieEnd(int bValue)
{
    m_pcExoAppInternal->m_bStartMenuMusicOnMovieEnd = bValue;
}
// PC 0x00638C50
unsigned char CClientExoApp::GetActivePauseState()
{
    return m_pcExoAppInternal->GetActivePauseState();
}

#ifndef _XBOX
void CClientExoApp::QueryExitProgram()  // PC 0x00638C60
{
    m_pcExoAppInternal->QueryExitProgram();
}
#endif
// PC 0x00638C70
void CClientExoApp::SetInputActive(int nA, int nB)
{
    m_pcExoAppInternal->SetInputActive(nA, nB);
}
// PC 0x00638C80
void CClientExoApp::TurnOffKeyboardModal()
{
    m_pcExoAppInternal->TurnOffKeyboardModal();
}
// PC 0x00638C90
int CClientExoApp::IsPlayerCreature(CSWCCreature* pCreature)
{
    return m_pcExoAppInternal->IsPlayerCreature(pCreature);
}
// PC 0x00638CA0
CSWGuiManager* CClientExoApp::GetSWGuiManager()
{
    return m_pcExoAppInternal->m_pSWGuiManager;
}
// PC 0x00638CB0
unsigned long CClientExoApp::ServerToClientObjectId(unsigned long oidServer)
{
    return m_pcExoAppInternal->ServerToClientObjectId(oidServer);
}

#ifdef _XBOX
void CClientExoApp::SetInputClass(int nClass)
{
    m_pcExoAppInternal->SetInputClass(nClass);
}
#else
void CClientExoApp::SetInputClass(int nClass, int bHideInterface)  // PC 0x00638CC0
{
    m_pcExoAppInternal->SetInputClass(nClass, bHideInterface);
}
#endif
// PC 0x00638CD0
int CClientExoApp::GetInputClass()
{
    return m_pcExoAppInternal->m_nInputClass;
}
// PC 0x00638CE0
void CClientExoApp::ShowLoadScreen(int bFade, int bPlayMusic, int bSaving)
{
    m_pcExoAppInternal->ShowLoadScreen(bFade, bPlayMusic, bSaving);
}

#ifdef _XBOX
void CClientExoApp::HideLoadScreen(int bFade)
{
    m_pcExoAppInternal->HideLoadScreen(bFade);
}
#else
void CClientExoApp::HideLoadScreen(int bFade, int bUpdateScreen)  // PC 0x00638CF0
{
    m_pcExoAppInternal->HideLoadScreen(bFade, bUpdateScreen);
}
#endif
// PC 0x00638D00
CExoString& CClientExoApp::SetPlayerCharacterName(const CExoString& sName)
{
    return m_pcExoAppInternal->SetPlayerCharacterName(sName);
}
// PC 0x00638D10
CExoString& CClientExoApp::GetPlayerCharacterName()
{
    return m_pcExoAppInternal->GetPlayerCharacterName();
}
// PC 0x00638D20
int CClientExoApp::GetPlayerCharacterSet()
{
    return m_pcExoAppInternal->m_bPlayerCharacterNameSet;
}
// PC 0x00638D30
int CClientExoApp::GetAreaNotReadyToDisplay()
{
    return m_pcExoAppInternal->m_nAreaNotReadyToDisplay;
}
// PC 0x00638D40
void CClientExoApp::SetAreaNotReadyToDisplay(int bNotReady)
{
    m_pcExoAppInternal->m_nAreaNotReadyToDisplay = bNotReady;
}
// PC 0x00638D50
CSWMiniGameObjectArray* CClientExoApp::GetMGOArray()
{
    return m_pcExoAppInternal->m_pMiniGameObjectArray;
}
// PC 0x00638D60
CSWMiniGame* CClientExoApp::GetMiniGame()
{
    CSWCModule* pModule = m_pcExoAppInternal->GetModule();
    if (pModule != 0)
    {
        return pModule->GetMiniGame();
    }
    else
    {
        return 0;
    }
}
// PC 0x00638D80
int CClientExoApp::PlayLoadModuleMovies()
{
    return m_pcExoAppInternal->PlayLoadModuleMovies();
}
// PC 0x00638D90
int CClientExoApp::PlayMovie(const CExoString& sMovie, int& nA, int nB, int nC)
{
    return m_pcExoAppInternal->PlayMovie(sMovie, nA, nB, nC);
}
// PC 0x00638DA0
int CClientExoApp::IsMoviePlaying()
{
    return m_pcExoAppInternal->IsMoviePlaying();
}
// PC 0x00638DB0
int CClientExoApp::AddMoveToModuleMovie(const CExoString& sMovie)
{
    return m_pcExoAppInternal->AddMoveToModuleMovie(sMovie);
}
// PC 0x00638DC0
CExoArrayList<CExoString>* CClientExoApp::GetMoveToModuleMovies()
{
    return &m_pcExoAppInternal->m_aMoveToModuleMovies;
}
// PC 0x00638DD0
void CClientExoApp::IndicateObject(CSWCObject* pObject, int nFlags)
{
    m_pcExoAppInternal->IndicateObject(pObject, nFlags);
}
// PC 0x00638DE0
int CClientExoApp::ReattachCreatureToScene(unsigned long oidObject)
{
    return oidObject != m_pcExoAppInternal->m_oidReattachedCreature;
}
// PC 0x00638E00
void CClientExoApp::ResetDriveAcceleration()
{
    m_pcExoAppInternal->ResetDriveAcceleration();
}
// PC 0x00638E10
void CClientExoApp::BarkString(unsigned long oidObject, const CExoString& sText,
                              const CResRef& cSound, int nFlags)
{
    m_pcExoAppInternal->BarkString(oidObject, sText, cSound, nFlags);
}
// PC 0x00638E20
int CClientExoApp::IsSoundPlayingInDialog()
{
    return m_pcExoAppInternal->IsSoundPlayingInDialog();
}
// PC 0x00638E30
void CClientExoApp::StopSoundPlayingInDialog()
{
    m_pcExoAppInternal->StopSoundPlayingInDialog();
}
// PC 0x00638E40
void CClientExoApp::DisableInput()
{
    m_pcExoAppInternal->DisableInput();
}
// PC 0x00638E50
void CClientExoApp::EnableInput()
{
    m_pcExoAppInternal->EnableInput();
}
// PC 0x00638E60
int CClientExoApp::DismissInGameGUI()
{
    return m_pcExoAppInternal->DismissInGameGUI();
}
// PC 0x00638E70
int CClientExoApp::GetPausedByCombat()
{
    return m_pcExoAppInternal->GetPausedByCombat();
}
// PC 0x00638E80
void CClientExoApp::SetPausedByCombat(int nA, unsigned char nB, int nC)
{
    m_pcExoAppInternal->SetPausedByCombat(nA, nB, nC);
}
// PC 0x00638E90
void CClientExoApp::ChangeCharacter(int nCharacter, int bPlaySound)
{
    m_pcExoAppInternal->ChangeCharacter(nCharacter, bPlaySound);
}
// PC 0x00638EA0
void CClientExoApp::RunDeathSequence()
{
    m_pcExoAppInternal->RunDeathSequence();
}
// PC 0x00638EB0
int CClientExoApp::SetGameOver(int bGameOver)
{
    return m_pcExoAppInternal->SetGameOver(bGameOver);
}
// PC 0x00638EC0
int CClientExoApp::GetGameOver()
{
    return m_pcExoAppInternal->m_nGameOver;
}
// PC 0x00638ED0
void CClientExoApp::SetLastPartyMemberTempKilled(unsigned long oidObject)
{
    m_pcExoAppInternal->SetLastPartyMemberTempKilled(oidObject);
}
// PC 0x00638EE0
void CClientExoApp::SetCanSendDialog(int bCanSend)
{
    m_pcExoAppInternal->SetCanSendDialog(bCanSend);
}
// PC 0x00638EF0
int CClientExoApp::GetCanSendDialog(unsigned long nA, unsigned long nB,
                                    CSWSDialog* pDialog)
{
    return m_pcExoAppInternal->GetCanSendDialog(nA, nB, pDialog);
}
// PC 0x00638F00
void CClientExoApp::SetDialogSent(int bSent)
{
    m_pcExoAppInternal->m_bDialogSent = bSent;
}

int CClientExoApp::GetDialogSent()
{
    return m_pcExoAppInternal->m_bDialogSent;
}
// PC 0x00638F10
void CClientExoApp::SetDialogModelsSent(int bSent)
{
    m_pcExoAppInternal->m_bDialogModelsSent = bSent;
}

int CClientExoApp::GetDialogModelsSent()
{
    return m_pcExoAppInternal->m_bDialogModelsSent;
}
// PC 0x00638F20
int CClientExoApp::GetClientRangedAnimation(unsigned long nA, int nB, int nC,
                                            int nD)
{
    return m_pcExoAppInternal->GetClientRangedAnimation(nA, nB, 0, 0);
}
// PC 0x00638F40
int CClientExoApp::GetClientMeleeAnimation(unsigned long nA, unsigned long nB,
                                           unsigned short nC, int nD, int nE,
                                           int nF)
{
    return m_pcExoAppInternal->GetClientMeleeAnimation(nA, nB, nC, nD, nE, nF);
}
// PC 0x00638F50
void CClientExoApp::SetLoadScreenByModuleName(const CExoString& sModule)
{
    m_pcExoAppInternal->SetLoadScreenByModuleName(sModule);
}
// PC 0x00638F60
CExoString CClientExoApp::GetLoadMusicByModuleName(const CExoString& sModule)
{
    return m_pcExoAppInternal->GetLoadMusicByModuleName(sModule);
}
// PC 0x00638F90
void CClientExoApp::SetLoadMusic(const CExoString& sMusic)
{
    m_pcExoAppInternal->m_sLoadMusic = sMusic;
}
// PC 0x00638FA0
void CClientExoApp::SetLoadBarProgress(int nProgress, int bUpdateHint)
{
    m_pcExoAppInternal->SetLoadBarProgress(nProgress, bUpdateHint);
}
// PC 0x00638FB0
void CClientExoApp::SetLoadFromIFO(int bValue)
{
    m_pcExoAppInternal->m_nLoadFromIFO = bValue;
}
// PC 0x00638FC0
int CClientExoApp::GetLoadFromIFO()
{
    return m_pcExoAppInternal->m_nLoadFromIFO;
}
// PC 0x00638FD0
void CClientExoApp::SetLastTarget(unsigned long oidObject, int nB)
{
    m_pcExoAppInternal->SetLastTarget(oidObject, nB);
}
// PC 0x00638FE0
int CClientExoApp::GetLastTarget()
{
    return m_pcExoAppInternal->m_nLastTarget;
}

void CClientExoApp::SetRunScript(const CExoString& sScript)
{
    CClientExoAppInternal* pInternal = m_pcExoAppInternal;
    pInternal->m_bRunScript = 1;
    pInternal->m_sRunScript = sScript;
}
// PC 0x00639000
void CClientExoApp::UpdateRoom(const Vector& vPosition)
{
    m_pcExoAppInternal->UpdateRoom(vPosition);
}
// PC 0x00639010
void CClientExoApp::StartPazaakGame(int nSideDeck, const CExoString& sEndScript, int nWager, int bTutorial,
                                   unsigned long oidOpponent)
{
    m_pcExoAppInternal->StartPazaakGame(nSideDeck, sEndScript, nWager, bTutorial, oidOpponent);
}
// PC 0x00639020
void CClientExoApp::EndPazaakGame(int nResult, int nWager, int bPlayAgain,
                                 unsigned long oidOpponent)
{
    m_pcExoAppInternal->EndPazaakGame(nResult, nWager, bPlayAgain,
                                             oidOpponent);
}
// PC 0x00639030
int CClientExoApp::GetPazaakResults(int* pnResult, int* pnUnused)
{
    CClientExoAppInternal* pInternal = m_pcExoAppInternal;
    int nUnused;
    *pnResult = pInternal->m_PazaakState.m_nResult;
    nUnused = pInternal->m_PazaakState.m_nResultUnused;
    *pnUnused = nUnused;
    return nUnused;
}
// PC 0x00639050
void CClientExoApp::SetFutureLeader(unsigned long oidObject)
{
    m_pcExoAppInternal->m_oidFutureLeader = oidObject;
}

unsigned long CClientExoApp::GetFutureLeader()
{
    return m_pcExoAppInternal->m_oidFutureLeader;
}

int CClientExoApp::GetEndGamePending()
{
    return m_pcExoAppInternal->m_bEndGamePending;
}
// PC 0x00639060
void CClientExoApp::SetEndGamePending(int bPending, float fDelay, int nC)
{
    CClientExoAppInternal* pInternal = m_pcExoAppInternal;
    pInternal->m_bEndGamePending = bPending;
    pInternal->m_fEndGamePendingDelay = fDelay;
    pInternal->m_nEndGamePendingUnknown = nC;
}
// PC 0x00639090
int CClientExoApp::PlayRumblePattern(int nPattern)
{
    return m_pcExoAppInternal->PlayRumblePattern(nPattern);
}
// PC 0x006390A0
int CClientExoApp::StopRumblePattern(int nPattern)
{
    return m_pcExoAppInternal->StopRumblePattern(nPattern);
}
// PC 0x006390B0
void CClientExoApp::TogglePartyFollow()
{
    m_pcExoAppInternal->TogglePartyFollow();
}
// PC 0x006390C0
int CClientExoApp::GetCombatMode()
{
    return m_pcExoAppInternal->m_nCombatMode;
}
// PC 0x006390D0
void CClientExoApp::SetCombatMode(int nMode)
{
    m_pcExoAppInternal->SetCombatMode(nMode);
}
// PC 0x006390E0
void CClientExoApp::PlayerFlourishWeapons()
{
    m_pcExoAppInternal->PlayerFlourishWeapons();
}
// PC 0x006390F0
void CClientExoApp::AddFloatyText(unsigned long oidObject, unsigned char nType,
                               int nValue)
{
    m_pcExoAppInternal->AddFloatyText(oidObject, nType, nValue);
}
// PC 0x00639100
CExoString CClientExoApp::GetGUIString(unsigned long nStrRef)
{
    return m_pcExoAppInternal->GetGUIString(nStrRef);
}
// PC 0x00639130
void CClientExoApp::SetAutoPaused(int bAuto, unsigned char nB)
{
    m_pcExoAppInternal->SetAutoPaused(bAuto, nB);
}
// PC 0x00639140
int CClientExoApp::GetAutoPaused()
{
#ifdef _XBOX
    return m_pcExoAppInternal->m_PauseFlags.m_Bits.m_bAutoPaused;
#else
    return m_pcExoAppInternal->m_AutoPauseFlags.m_Bits.m_bAutoPaused;
#endif
}
// PC 0x00639150
void CClientExoApp::SetLoadScreenHint(unsigned long nHint)
{
    m_pcExoAppInternal->SetLoadScreenHint(nHint);
}

unsigned long CClientExoApp::GetLoadScreenHint()
{
    return m_pcExoAppInternal->GetLoadScreenHint();
}
// PC 0x00639160
void CClientExoApp::EnableVideoEffect(int nEffect)
{
    m_pcExoAppInternal->EnableVideoEffect(nEffect);
}
// PC 0x00639170
void CClientExoApp::DisableVideoEffect()
{
    m_pcExoAppInternal->DisableVideoEffect();
}
// PC 0x00639190
void CClientExoApp::DelayNextEnemyAutoPause()
{
    m_pcExoAppInternal->DelayNextEnemyAutoPause();
}
// PC 0x006391A0
int CClientExoApp::ShowTutorialWindow(unsigned char nTutorial, unsigned long nX,
                                      unsigned long nY, unsigned long nActionParam)
{
    return m_pcExoAppInternal->ShowTutorialWindow(nTutorial, nX, nY, nActionParam);
}
// PC 0x006391B0
int CClientExoApp::ClearCachedCreatures(CSWCCreature* pCreature)
{
    return m_pcExoAppInternal->ClearCachedCreatures(pCreature);
}
// PC 0x006391C0
void CClientExoApp::LookUpAndPerformRumbleWithCutOff(int nEntry,
                                                     unsigned char nTable,
                                                     const Vector& vPosition)
{
    m_pcExoAppInternal->LookUpAndPerformRumbleWithCutOff(nEntry, nTable,
                                                         vPosition);
}
#include "swplayercontrol.h"
// PC 0x006391D0
int CClientExoApp::GetPlayerMovementInput()
{
    CClientExoAppInternal* pInternal = m_pcExoAppInternal;
    if (pInternal->m_pPlayerControl != 0 &&
        (pInternal->m_pPlayerControl->GetLeftRight() != 0.0f ||
         pInternal->m_pPlayerControl->GetUpDown() != 0.0f))
    {
        return 1;
    }
    return 0;
}
// PC 0x00639220
int CClientExoApp::SaveOptions()
{
    return m_pcExoAppInternal->SaveOptions();
}

#ifdef _XBOX
int CClientExoApp::LoadOptions()
{
    return m_pcExoAppInternal->LoadOptions();
}
#endif
// PC 0x00639230
int CClientExoApp::ChangeCharacterToNextLivingPartyMember(int nA, int nB,
                                                          unsigned char nC)
{
    return m_pcExoAppInternal->ChangeCharacterToNextLivingPartyMember(nA, nB,
                                                                      nC);
}
// PC 0x0062B870
void CClientExoApp::StopMovie()
{
    m_pcExoAppInternal->StopMovie();
}
// PC 0x00639250
void CClientExoApp::SetAttackMashTutorialTimer(unsigned long nTimer)
{
    m_pcExoAppInternal->m_nAttackMashTutorialTimer = nTimer;
}
// PC 0x00639260
unsigned long CClientExoApp::GetAttackMashTutorialTimer()
{
    return m_pcExoAppInternal->m_nAttackMashTutorialTimer;
}

#ifdef _XBOX
void CClientExoApp::SetMinGlobalRim(int bMinGlobalRim)
{
    m_pcExoAppInternal->m_bMinGlobalRim = bMinGlobalRim;
}
#endif

int CClientExoApp::GetLoadingSaveGame()
{
    return m_pcExoAppInternal->m_nLoadingSaveGame;
}
// PC 0x00639270
void CClientExoApp::SetLoadingSaveGame(int bLoading)
{
    m_pcExoAppInternal->m_nLoadingSaveGame = bLoading;
}
// PC 0x00639280
void CClientExoApp::SetLoadStep(int nStep, unsigned char cValue)
{
    m_pcExoAppInternal->m_aLoadSteps[nStep] = cValue;
}
// PC 0x006392A0
unsigned char CClientExoApp::GetLoadStep(int nStep)
{
    return m_pcExoAppInternal->m_aLoadSteps[nStep];
}
// PC 0x006392C0
void CClientExoApp::ClearEnemySighted()
{
    m_pcExoAppInternal->m_bEnemySighted = 0;
}
// PC 0x006392D0
CExoString& CClientExoApp::GetLiveContentName(int nIndex)
{
    return m_pcExoAppInternal->m_aLiveContentNames[nIndex].m_sName;
}

int CClientExoApp::GetLiveContentExists(int nIndex)
{
    return m_pcExoAppInternal->m_aLiveContentNames[nIndex].m_bExists;
}

int CClientExoApp::GetLiveContentCorrupt(int nIndex)
{
    return m_pcExoAppInternal->m_aLiveContentNames[nIndex].m_bCorrupt;
}

unsigned long CClientExoApp::GetLoadScreenHintUpdateTimer()
{
    return m_pcExoAppInternal->m_nLoadScreenHintUpdateTimer;
}
// PC 0x006392F0
void CClientExoApp::SetLoadScreenHintUpdateTimer(unsigned long nTimer)
{
    m_pcExoAppInternal->m_nLoadScreenHintUpdateTimer = nTimer;
}
// PC 0x00639300
unsigned char CClientExoApp::GetGamePlayHint()
{
    return m_pcExoAppInternal->m_nGamePlayHint;
}
// PC 0x00639310
void CClientExoApp::SetGamePlayHint(unsigned char nHint)
{
    m_pcExoAppInternal->m_nGamePlayHint = nHint;
}
// PC 0x00639320
unsigned char CClientExoApp::GetStoryHint()
{
    return m_pcExoAppInternal->m_nStoryHint;
}
// PC 0x00639340
void CClientExoApp::SetStoryHint(unsigned char nHint)
{
    m_pcExoAppInternal->m_nStoryHint = nHint;
}
// PC 0x00639360
int CClientExoApp::GetNextLoadScreenHintSTRREF(const CExoString& sModule,
                                               unsigned char nB)
{
    return m_pcExoAppInternal->GetNextLoadScreenHintSTRREF(sModule, nB);
}
// PC 0x00639370
void CClientExoApp::SetModuleResourceLoaded(unsigned char nMask)
{
    m_pcExoAppInternal->m_nModuleResourcesLoaded |= nMask;
}
// PC 0x00639390
void CClientExoApp::ResetModuleResourcesLoaded()
{
    m_pcExoAppInternal->m_nModuleResourcesLoaded = 0;
}
// PC 0x006393A0
int CClientExoApp::GetModuleResourceLoaded(unsigned char nMask)
{
    unsigned char nLoaded =
        m_pcExoAppInternal->m_nModuleResourcesLoaded & nMask;
    return nLoaded > 0;
}
// PC 0x006393C0
int CClientExoApp::GetTargetChanging()
{
    return m_pcExoAppInternal->m_fTargetChanging > 0.0f;
}
// PC 0x006393E0
int CClientExoApp::GetTogglePauseAtEndOfFrame()
{
    return m_pcExoAppInternal->m_PauseFlags.m_Bits.m_bTogglePauseAtEndOfFrame !=
           0;
}
// PC 0x006393F0
void CClientExoApp::BusyWaitMenuMusic()
{
    m_pcExoAppInternal->BusyWaitMenuMusic();
}
// PC 0x00639400
void CClientExoApp::BusyWaitMuteSound()
{
    m_pcExoAppInternal->BusyWaitMuteSound();
}
// PC 0x00639410
int CClientExoApp::GetPazaakIsRunning()
{
    return m_pcExoAppInternal->m_PazaakState.m_bIsRunning;
}
// PC 0x00639420
void CClientExoApp::SetMouseMode(unsigned char nMode)
{
    m_pcExoAppInternal->SetMouseMode(nMode);
}
// PC 0x00639430
void CClientExoApp::CancelMovie()
{
    m_pcExoAppInternal->CancelMovie();
}
// PC 0x00639440
void CClientExoApp::HideMouse(int nFlags)
{
    m_pcExoAppInternal->HideMouse(nFlags);
}
// PC 0x00639450
void CClientExoApp::ShowMouse(int nFlags)
{
    m_pcExoAppInternal->ShowMouse(nFlags);
}
#ifndef _XBOX
int CClientExoApp::GetCharacterChangeInProgress()  // PC 0x00639460
{
    return m_pcExoAppInternal->m_bCharacterChangeInProgress;
}
// PC 0x00639470
void CClientExoApp::SetCharacterChangeInProgress(int bInProgress)
{
    m_pcExoAppInternal->m_bCharacterChangeInProgress = bInProgress;
}
#endif
void CClientExoApp::FadeAndStopStreamingSoundSource(  // PC 0x00639480
    CExoStreamingSoundSource* pSource, unsigned long nB)
{
    m_pcExoAppInternal->FadeAndStopStreamingSoundSource(pSource, nB);
}
#ifndef _XBOX
void CClientExoApp::StartCreditSequence(const Vector& vColor,  // PC 0x00639490
                                      const CExoString& sMusic)
{
    m_pcExoAppInternal->StartCreditSequence(vColor, sMusic);
}
// PC 0x006394A0
void CClientExoApp::StopCreditSequence()
{
    m_pcExoAppInternal->StopCreditSequence();
}

int CClientExoApp::GetCreditSequenceInProgress()
{
    return m_pcExoAppInternal->m_bCreditSequenceInProgress;
}

void CClientExoApp::SetCreditSequenceInProgress(int bInProgress)
{
    m_pcExoAppInternal->m_bCreditSequenceInProgress = bInProgress;
}
#endif
void CClientExoApp::EnableMouseAtEndOfLoadModuleMovie()  // PC 0x006394C0
{
    m_pcExoAppInternal->EnableMouseAtEndOfLoadModuleMovie();
}
#ifndef _XBOX
void CClientExoApp::SetLastClickedOnTarget(unsigned long oidObject)  // PC 0x006394D0
{
    m_pcExoAppInternal->m_oidLastClickedOnTarget = oidObject;
}

unsigned long CClientExoApp::GetLastClickedOnTarget()
{
    return m_pcExoAppInternal->m_oidLastClickedOnTarget;
}
#endif
#ifndef _XBOX
int CClientExoApp::AddMovieToMovieQueue(const CExoString& sMovie, int nFlags)  // PC 0x006394E0
{
    return m_pcExoAppInternal->AddMovieToMovieQueue(sMovie, nFlags);
}
// PC 0x006394F0
int CClientExoApp::PlayMovieQueue(int nFlags)
{
    return m_pcExoAppInternal->PlayMovieQueue(nFlags);
}
#endif

#ifdef _XBOX

#include "swplayercontrol.h"
int CClientExoApp::GetInFreeLook()
{
    CClientExoAppInternal* pInternal = m_pcExoAppInternal;
    if (pInternal->m_pPlayerControl != 0 &&
        (pInternal->m_pPlayerControl->GetLeftRight() != 0.0f ||
         pInternal->m_pPlayerControl->GetUpDown() != 0.0f))
    {
        return 1;
    }
    return 0;
}
#else
int CClientExoApp::GetInFreeLook()  // PC 0x00639520
{
    if (m_pcExoAppInternal->GetModule() == 0 ||
        m_pcExoAppInternal->GetModule()->m_pModuleCamera == 0)
    {
        return 0;
    }
    CSWCModule* pModule = m_pcExoAppInternal->GetModule();
    return pModule->m_pModuleCamera->GetBehavior(
               AUR_BEHAVIOR_CAMERA_FREELOOK) != 0;
}
#endif
// PC 0x00639570
CExoString CClientExoApp::GetDisplayCharacterForKey(int nKey)
{
    return m_pcExoAppInternal->GetDisplayCharacterForKey(nKey);
}

int CClientExoApp::IsValidResolution(unsigned long nWidth,
                                     unsigned long nHeight)
{
    return m_pcExoAppInternal->IsValidResolution(nWidth, nHeight);
}
#ifndef _XBOX
int CClientExoApp::GetAllowHighMonitorFrequency()  // PC 0x006394B0
{
    return m_pcExoAppInternal->m_bAllowHighMonitorFrequency;
}
#endif
void CClientExoApp::PlayerListChange(unsigned long, int, int)  // PC 0x006009E0
{
}
#ifndef _XBOX
int CClientExoApp::GetSoundPausedByCombat()  // PC 0x006395A0
{
    return m_pcExoAppInternal->m_nSoundPausedByCombat;
}

void CClientExoApp::SetSoundPausedByCombat(int nPaused)
{
    m_pcExoAppInternal->m_nSoundPausedByCombat = nPaused;
}
#endif
int CClientExoApp::DontUseThisSetPlayerCreature(unsigned long oidObject)
{
    return m_pcExoAppInternal->DontUseThisSetPlayerCreature(oidObject);
}

#ifndef _XBOX
void CClientExoApp::HandleExitGameButton()
{
    m_pcExoAppInternal->HandleExitGameButton(NULL);
}
#endif
// PC 0x006395F0
CClientExoApp::CClientExoApp()
{
    m_pcExoAppInternal = new CClientExoAppInternal();
}
// PC 0x00639660
CClientExoApp::~CClientExoApp()
{
    if (m_pcExoAppInternal != NULL)
    {
        delete m_pcExoAppInternal;
    }
}
// PC 0x00639680
void CClientExoApp::RemoveMoveToModuleMovies()
{
    m_pcExoAppInternal->m_aMoveToModuleMovies.RemoveAll(0);
}

#ifdef _XBOX
int CClientExoApp::ChangeCharacterToNextLivingPartyMember(int nA, int nB)
{
    return m_pcExoAppInternal->ChangeCharacterToNextLivingPartyMember(nA, nB);
}
#endif

#ifdef _XBOX

void CClientExoApp::SetMissingFilePending(int bPending)
{
    m_pcExoAppInternal->m_nXboxUnmapped468 = bPending;
}
#endif

#ifdef _XBOX

void CClientExoApp::StartServices(int nServices, const CExoString& sArg2,
                                  const CExoString& sArg3,
                                  const CExoString& sArg4, unsigned long nArg5)
{
    m_pcExoAppInternal->StartServices(nServices, sArg2, sArg3, sArg4, nArg5);
}
#endif
