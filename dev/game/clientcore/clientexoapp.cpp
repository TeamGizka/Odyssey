#include "clientexoapp.h"
#include "appmanager.h"
#include "aurcamera.h"
#include "clientexoappinternal.h"
#include "nwcmodule.h"
#include "serverexoapp.h"

extern const int CLIENTEXOAPP_UNREFERENCED_128 = 128;

#ifndef _XBOX
void CClientExoApp::BeginIntro()  // PC 0x005ED300
{
    m_pcExoAppInternal->BeginIntro();
}
#endif
// PC 0x005ED310
void CClientExoApp::EndLoadBar()
{
    m_pcExoAppInternal->EndLoadBar();
}
// PC 0x005ED320  Xbox 0x0001B200
CSWCJournal* CClientExoApp::GetQuestJournal()
{
    return m_pcExoAppInternal->m_pQuestJournal;
}
// PC 0x005ED330  Xbox 0x0001B210
CSWCWorldJournal* CClientExoApp::GetWorldJournal()
{
    return m_pcExoAppInternal->m_pWorldJournal;
}
// PC 0x005ED340  Xbox 0x0001B220
void CClientExoApp::JournalUpdated(int nType, int nIndex,
                                   const CExoString& sName)
{
    m_pcExoAppInternal->JournalUpdated(nType, nIndex, sName);
}
// PC 0x005ED350  Xbox 0x0001B230
int CClientExoApp::GetObjectName(unsigned long oidObject, CExoString& sName)
{
    return m_pcExoAppInternal->GetObjectName(oidObject, sName);
}
// PC 0x005ED360  Xbox 0x0001B240
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
// PC 0x005ED410  Xbox 0x0001B2F0
void CClientExoApp::SetCharacterFile(unsigned long nDataSize,
                                     unsigned char* pCharacterData,
                                     int nCharacterFileId)
{
    m_pcExoAppInternal->SetCharacterFile(nDataSize, pCharacterData,
                                       nCharacterFileId);
}
// PC 0x005ED420  Xbox 0x0001B300
void CClientExoApp::DisplayMainMenu(int bUnknown)
{
    m_pcExoAppInternal->DisplayMainMenu(bUnknown);
}

#ifdef _XBOX
void CClientExoApp::DisplayStartScreen()  // Xbox 0x0001B310
{
    m_pcExoAppInternal->DisplayStartScreen();
}
#endif
// PC 0x005ED430  Xbox 0x0001B320
void CClientExoApp::UnloadModule()
{
    m_pcExoAppInternal->UnloadModule();
}
// PC 0x005ED440  Xbox 0x0001B330
CSWCModule* CClientExoApp::CreateModule()
{
    return m_pcExoAppInternal->CreateModule();
}

#ifndef _XBOX
void CClientExoApp::ExitProgram()  // PC 0x005ED450
{
    m_pcExoAppInternal->ExitProgram();
}
#endif
// PC 0x005ED460  Xbox 0x0001B340
CExoFile* CClientExoApp::GetCaptureEventFile()
{
    return m_pcExoAppInternal->m_pCaptureEventFile;
}
// PC 0x005ED470  Xbox 0x0001B350
int CClientExoApp::GetCaptureEvents()
{
    return m_pcExoAppInternal->m_nCaptureEvents;
}
// PC 0x005ED480  Xbox 0x0001B360
int CClientExoApp::GetCaptureEventPlayBack()
{
    return m_pcExoAppInternal->m_nCaptureEventPlayBack;
}
// PC 0x005ED490  Xbox 0x0001B370
CExoString CClientExoApp::GetClientName()
{
    return m_pcExoAppInternal->m_sClientName;
}
// PC 0x005ED4C0  Xbox 0x0001B3A0
int CClientExoApp::GetClientLanguage()
{
    return m_pcExoAppInternal->m_nClientLanguage;
}
// PC 0x005ED4D0
int CClientExoApp::IsClientImeUI()
{
    return m_pcExoAppInternal->m_bClientImeUI;
}
// PC 0x005ED4E0
CExoImeUI* CClientExoApp::GetImeUI()
{
    return m_pcExoAppInternal->m_pImeUI;
}
// PC 0x005ED4F0  Xbox 0x0001B3B0
CWorldTimer* CClientExoApp::GetWorldTimer()
{
    return m_pcExoAppInternal->m_pWorldTimer;
}
// PC 0x005ED500  Xbox 0x0001B3C0
CWorldTimer* CClientExoApp::GetPauseTimer()
{
    return m_pcExoAppInternal->m_pPauseTimer;
}
// PC 0x005ED510  Xbox 0x0001B3D0
CWorldTimer* CClientExoApp::GetAnimationTimer()
{
    return m_pcExoAppInternal->m_pAnimationTimer;
}
// PC 0x005ED520  Xbox 0x0001B3E0
CConnectionLib* CClientExoApp::GetConnectionLib()
{
    return m_pcExoAppInternal->m_pConnectionLib;
}
// PC 0x005ED530  Xbox 0x0001B3F0
CSWCModule* CClientExoApp::GetModule()
{
    return m_pcExoAppInternal->GetModule();
}
// PC 0x0063E7F0
CExoLocString* CClientExoApp::GetModuleDescription()
{
    return 0;
}
// PC 0x005ED540  Xbox 0x0001B400
CSWCCreature* CClientExoApp::GetPlayerCreature()
{
    return m_pcExoAppInternal->GetPlayerCreature();
}
// PC 0x005ED550  Xbox 0x0001B410
unsigned long CClientExoApp::GetPlayerCreatureId()
{
    return m_pcExoAppInternal->m_nPlayerCreatureId;
}
// PC 0x005ED560  Xbox 0x0001B420
CAurCamera* CClientExoApp::GetModuleCamera()
{
    return m_pcExoAppInternal->GetModuleCamera();
}
// PC 0x005ED570  Xbox 0x0001B430
CGameObjectArray* CClientExoApp::GetGameObjectArray()
{
    return m_pcExoAppInternal->m_pGameObjectArray;
}
// PC 0x005ED580  Xbox 0x0001B440
CSWCObject* CClientExoApp::GetGameObject(unsigned long oidObject)
{
    return (CSWCObject*)m_pcExoAppInternal->GetGameObject(oidObject);
}
// PC 0x005ED590  Xbox 0x0001B450
CSWCPlayer* CClientExoApp::GetPlayerByGameObjectID(unsigned long oidObject)
{
    return m_pcExoAppInternal->GetPlayerByGameObjectID(oidObject);
}
// PC 0x005ED5A0  Xbox 0x0001B460
CSWCItem* CClientExoApp::GetItemByGameObjectID(unsigned long nId)
{
    return m_pcExoAppInternal->GetItemByGameObjectID(nId);
}
// PC 0x005ED5B0  Xbox 0x0001B470
CSWCCreature* CClientExoApp::GetCreatureByGameObjectID(unsigned long nId)
{
    return m_pcExoAppInternal->GetCreatureByGameObjectID(nId);
}
// PC 0x005ED5C0  Xbox 0x0001B480
CSWCArea* CClientExoApp::GetAreaByGameObjectID(unsigned long oidObject)
{
    return m_pcExoAppInternal->GetAreaByGameObjectID(oidObject);
}
// PC 0x005ED5D0  Xbox 0x0001B490
CSWCTrigger* CClientExoApp::GetTriggerByGameObjectID(unsigned long oidObject)
{
    return m_pcExoAppInternal->GetTriggerByGameObjectID(oidObject);
}
// PC 0x005ED5E0  Xbox 0x0001B4A0
CSWCProjectile* CClientExoApp::GetProjectileByGameObjectID(
    unsigned long oidObject)
{
    return m_pcExoAppInternal->GetProjectileByGameObjectID(oidObject);
}
// PC 0x005ED5F0  Xbox 0x0001B4B0
CSWCPlaceable* CClientExoApp::GetPlaceableByGameObjectID(
    unsigned long oidObject)
{
    return m_pcExoAppInternal->GetPlaceableByGameObjectID(oidObject);
}
// PC 0x005ED600  Xbox 0x0001B4C0
CSWCDoor* CClientExoApp::GetDoorByGameObjectID(unsigned long oidObject)
{
    return m_pcExoAppInternal->GetDoorByGameObjectID(oidObject);
}
// PC 0x005ED610  Xbox 0x0001B4D0
CSWCAreaOfEffectObject* CClientExoApp::GetAreaOfEffectByGameObjectID(
    unsigned long oidObject)
{
    return m_pcExoAppInternal->GetAreaOfEffectByGameObjectID(oidObject);
}
// PC 0x005ED620  Xbox 0x0001B4E0
CSWCSoundObject* CClientExoApp::GetSoundObjectByGameObjectID(
    unsigned long oidObject)
{
    return m_pcExoAppInternal->GetSoundObjectByGameObjectID(oidObject);
}

int CClientExoApp::GetInStartNewModule()
{
    return m_pcExoAppInternal->m_bInStartNewModule;
}
// PC 0x005ED630  Xbox 0x0001B4F0
void CClientExoApp::SetInStartNewModule(int bInStartNewModule)
{
    m_pcExoAppInternal->m_bInStartNewModule = bInStartNewModule;
}
// PC 0x005ED640  Xbox 0x0001B500
int CClientExoApp::GetPauseState(unsigned char nPauseType)
{
    return m_pcExoAppInternal->GetPauseState(nPauseType);
}
// PC 0x005ED650  Xbox 0x0001B510
void CClientExoApp::SetPauseState(unsigned char nPauseState, int bPause,
                                 CExoLinkedList<unsigned long>* pExclusionList)
{
    m_pcExoAppInternal->SetPauseState(nPauseState, bPause,
                                             pExclusionList);
}
// PC 0x005ED660  Xbox 0x0001B520
CWorldTimer* CClientExoApp::GetActiveTimer(unsigned long nTimer)
{
    return m_pcExoAppInternal->GetActiveTimer(nTimer);
}

#ifdef _XBOX
int CClientExoApp::SendSaveGameRequest(unsigned long nPlayerId,  // Xbox 0x0001B530
                                       const CExoString& sSaveName)
{
    return m_pcExoAppInternal->SendSaveGameRequest(nPlayerId, sSaveName);
}
#else
int CClientExoApp::SendSaveGameRequest(unsigned long nPlayerId,  // PC 0x005ED670
                                       const CExoString& sSaveName,
                                       const CExoString& sSaveDir)
{
    return m_pcExoAppInternal->SendSaveGameRequest(nPlayerId, sSaveName,
                                                   sSaveDir);
}
#endif
// PC 0x005ED680  Xbox 0x0001B540
int CClientExoApp::SendLoadGameRequest(unsigned long nPlayerId,
                                       const CExoString& sSaveName,
                                       const CExoString& sSaveDir)
{
    return m_pcExoAppInternal->SendLoadGameRequest(nPlayerId, sSaveName,
                                                   sSaveDir);
}
// PC 0x005ED690  Xbox 0x0001B550
CGuiInGame* CClientExoApp::GetInGameGui()
{
    return m_pcExoAppInternal->m_pGuiInGame;
}
// PC 0x005ED6A0  Xbox 0x0001B560
int CClientExoApp::GetPlayModulePanel()
{
    return m_pcExoAppInternal->m_nPlayModulePanel;
}
// PC 0x005ED6B0  Xbox 0x0001B570
CClientAIMaster* CClientExoApp::GetClientAIMaster()
{
    return m_pcExoAppInternal->m_pClientAIMaster;
}
// PC 0x005ED6C0  Xbox 0x0001B580
CExoLinkedList<CSWCPlayer>* CClientExoApp::GetPlayerList()
{
    return &m_pcExoAppInternal->m_lstPlayerList;
}
// PC 0x005ED6D0  Xbox 0x0001B590
CSWCPlayer* CClientExoApp::GetPlayerByPlayerID(unsigned long nPlayerId)
{
    return m_pcExoAppInternal->GetPlayerByPlayerID(nPlayerId);
}
// PC 0x005ED6E0  Xbox 0x0001B5A0
CNetLayer* CClientExoApp::GetNetLayer()
{
    return m_pcExoAppInternal->m_pUnknown10;
}
// PC 0x005ED6F0  Xbox 0x0001B5B0
CSWCMessage* CClientExoApp::GetSWCMessage()
{
    return m_pcExoAppInternal->m_pSWCMessage;
}
// PC 0x005ED700  Xbox 0x0001B5C0
CClientOptions* CClientExoApp::GetClientOptions()
{
    return m_pcExoAppInternal->m_pClientOptions;
}
// PC 0x005ED710  Xbox 0x0001B5D0
CServerInfo* CClientExoApp::GetServerInfo()
{
    return m_pcExoAppInternal->m_pServerInfo;
}
// PC 0x005ED720  Xbox 0x0001B5E0
void CClientExoApp::AttachPlayerGameObjectToArea()
{
    m_pcExoAppInternal->AttachPlayerGameObjectToArea();
}
// PC 0x005ED730  Xbox 0x0001B5F0
int CClientExoApp::InitializeInGameGUI()
{
    return m_pcExoAppInternal->InitializeInGameGUI();
}
// PC 0x005ED740  Xbox 0x0001B600
void CClientExoApp::ServerModeChange(unsigned char nServerMode)
{
    m_pcExoAppInternal->ServerModeChange(nServerMode);
}
// PC 0x005ED750  Xbox 0x0001B610
void CClientExoApp::AddObjectToHitcheckIgnoreList(unsigned long oidObject)
{
    m_pcExoAppInternal->AddObjectToHitcheckIgnoreList(oidObject);
}
// PC 0x005ED760  Xbox 0x0001B620
void CClientExoApp::RemoveObjectFromHitcheckIgnoreList(unsigned long oidObject)
{
    m_pcExoAppInternal->RemoveObjectFromHitcheckIgnoreList(oidObject);
}
// PC 0x005ED770  Xbox 0x0001B630
void CClientExoApp::EmptyHitcheckIgnoreList()
{
    m_pcExoAppInternal->EmptyHitcheckIgnoreList();
}
// PC 0x005ED780  Xbox 0x0001B640
int CClientExoApp::SetPlayerCreature(unsigned long oidObject)
{
    return m_pcExoAppInternal->SetPlayerCreature(oidObject);
}
// PC 0x005ED790  Xbox 0x0001B650
int CClientExoApp::HandleMessage(unsigned long nPlayerId,
                                 unsigned char* pMessage, unsigned long nSize,
                                 int bGuaranteed)
{
    return m_pcExoAppInternal->HandleMessage(nPlayerId, pMessage, nSize,
                                             bGuaranteed);
}
// PC 0x005ED7A0  Xbox 0x0001B660
int CClientExoApp::ContinueMessageProcessing()
{
    return m_pcExoAppInternal->ContinueMessageProcessing();
}
// PC 0x005ED7B0  Xbox 0x0001B670
int CClientExoApp::Initialize()
{
    return m_pcExoAppInternal->Initialize();
}
// PC 0x005ED7C0
int CClientExoApp::IsClientInitialized()
{
    return m_pcExoAppInternal->m_bClientInitialized;
}
// PC 0x005ED7D0  Xbox 0x0001B680
int CClientExoApp::MainLoop()
{
    return m_pcExoAppInternal->MainLoop();
}
// PC 0x005ED7E0  Xbox 0x0001B690
int CClientExoApp::CreatureAcquireItem(unsigned long oidObject,
                                       const CResRef& cResRef)
{
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    oidObject = pServer->ClientToServerObjectId(oidObject);
    pServer = g_pAppManager->m_pServerExoApp;
    return pServer->CreatureAcquireItem(oidObject, cResRef);
}
// PC 0x005ED810  Xbox 0x0001B6C0
int CClientExoApp::Uninitialize()
{
    return m_pcExoAppInternal->Uninitialize();
}
// PC 0x005ED820  Xbox 0x0001B6D0
void CClientExoApp::SetCaptureEvents(const CExoString& sFileName)
{
    m_pcExoAppInternal->SetCaptureEvents(sFileName);
}
// PC 0x005ED830  Xbox 0x0001B6E0
void CClientExoApp::SetCaptureEventPlayBack(const CExoString& sFileName)
{
    m_pcExoAppInternal->SetCaptureEventPlayBack(sFileName);
}
// PC 0x005ED840  Xbox 0x0001B6F0
void CClientExoApp::SetCaptureEventPlayBack(int nRate)
{
    m_pcExoAppInternal->SetCaptureEventPlayBackRate(nRate);
}
// PC 0x005ED850  Xbox 0x0001B700
int CClientExoApp::SetDebugMode(int nDebugMode)
{
    return m_pcExoAppInternal->SetDebugMode(nDebugMode);
}
// PC 0x005ED860  Xbox 0x0001B710
int CClientExoApp::StartServices(int nServices, const CExoString& sArg2,
                                 const CExoString& sArg3,
                                 const CExoString& sArg4, unsigned long nArg5)
{
    return m_pcExoAppInternal->StartServices(nServices, sArg2, sArg3, sArg4,
                                             nArg5);
}
// PC 0x005ED870
void CClientExoApp::SetEventDescriptions()
{
    m_pcExoAppInternal->SetEventDescriptions();
}
// PC 0x005ED880  Xbox 0x0001B720
void CClientExoApp::StopServices()
{
    m_pcExoAppInternal->StopServices();
}

#ifdef _XBOX
void CClientExoApp::ShutDownLegalScreen()  // Xbox 0x0001B730
{
    m_pcExoAppInternal->ShutDownLegalScreen();
}
#endif
// PC 0x005ED890  Xbox 0x0001B740
void CClientExoApp::ShutDownToMainMenu()
{
    m_pcExoAppInternal->ShutDownToMainMenu();
}
// PC 0x005ED8A0  Xbox 0x0001B750
void CClientExoApp::SetApplicationIdsMatch(int bMatch)
{
    m_pcExoAppInternal->m_bApplicationIdsMatch = bMatch;
}
// PC 0x005ED8B0  Xbox 0x0001B760
CSWParty* CClientExoApp::GetSWParty()
{
    return m_pcExoAppInternal->m_pSWParty;
}
// PC 0x005ED8C0  Xbox 0x0001B770
int CClientExoApp::GetHitCheckMaterialMask()
{
    return m_pcExoAppInternal->m_nHitCheckMaterialMask;
}

#ifndef _XBOX
int CClientExoApp::SetVideoMode(unsigned long nVideoMode)  // PC 0x005ED8D0
{
    return m_pcExoAppInternal->SetVideoMode(nVideoMode, 0);
}
#endif
// PC 0x005ED8F0
int CClientExoApp::SetTexturePack(unsigned char nTexturePack)
{
    return m_pcExoAppInternal->SetTexturePack(nTexturePack, 1);
}
// PC 0x005ED910  Xbox 0x0001B780
CSoundSetList* CClientExoApp::GetSoundSetList()
{
    return m_pcExoAppInternal->m_pSoundSetList;
}
// PC 0x005ED920  Xbox 0x0001B790
CResRef CClientExoApp::GetItemDropSound(unsigned char nType)
{
    CClientExoAppInternal* pInternal = m_pcExoAppInternal;
    if (pInternal->m_pItemDropSounds != NULL)
    {
        return pInternal->m_pItemDropSounds[nType];
    }
    return CResRef("");
}
// PC 0x005ED970  Xbox 0x0001B7E0
int CClientExoApp::GetLoadBarState()
{
    return m_pcExoAppInternal->m_nLoadBarState;
}

#ifndef _XBOX
void CClientExoApp::RestartMoviesIfNecessary()  // PC 0x005ED980
{
    m_pcExoAppInternal->RestartMoviesIfNecessary();
}
#endif
// PC 0x005ED990  Xbox 0x0001B7F0
int CClientExoApp::GetApplicationId()
{
    return g_pAppManager->m_nAppId;
}
// PC 0x005ED9A0  Xbox 0x0001B800
void CClientExoApp::AddObjectModelDirectlyToIgnoreList(
    unsigned long oidObject, List<CAurObject*>* pIgnoreList)
{
    m_pcExoAppInternal->AddObjectModelDirectlyToIgnoreList(oidObject,
                                                                  pIgnoreList);
}
// PC 0x005ED9B0  Xbox 0x0001B810
void CClientExoApp::StartMenuMusic(int bCultTheme, int nVolume, int nPriorityGroup)
{
    m_pcExoAppInternal->StartMenuMusic(bCultTheme, nVolume, nPriorityGroup);
}
// PC 0x005ED9C0  Xbox 0x0001B820
void CClientExoApp::StopMenuMusic()
{
    m_pcExoAppInternal->StopMenuMusic();
}
// PC 0x005ED9D0  Xbox 0x0001B830
void CClientExoApp::StopLoadMusic()
{
    m_pcExoAppInternal->StopLoadMusic();
}
// PC 0x005ED9E0  Xbox 0x0001B840
void CClientExoApp::SetStartMenuMusicOnMovieEnd(int bValue)
{
    m_pcExoAppInternal->m_bStartMenuMusicOnMovieEnd = bValue;
}
// PC 0x005ED9F0  Xbox 0x0001B850
unsigned char CClientExoApp::GetActivePauseState()
{
    return m_pcExoAppInternal->GetActivePauseState();
}

#ifndef _XBOX
void CClientExoApp::QueryExitProgram()  // PC 0x005EDA00
{
    m_pcExoAppInternal->QueryExitProgram();
}
#endif
// PC 0x005EDA10  Xbox 0x0001B860
void CClientExoApp::SetInputActive(int nA, int nB)
{
    m_pcExoAppInternal->SetInputActive(nA, nB);
}
// PC 0x005EDA20  Xbox 0x0001B870
void CClientExoApp::TurnOffKeyboardModal()
{
    m_pcExoAppInternal->TurnOffKeyboardModal();
}
// PC 0x005EDA30  Xbox 0x0001B880
int CClientExoApp::IsPlayerCreature(CSWCCreature* pCreature)
{
    return m_pcExoAppInternal->IsPlayerCreature(pCreature);
}
// PC 0x005EDA40  Xbox 0x0001B890
CSWGuiManager* CClientExoApp::GetSWGuiManager()
{
    return m_pcExoAppInternal->m_pSWGuiManager;
}
// PC 0x005EDA50  Xbox 0x0001B8A0
unsigned long CClientExoApp::ServerToClientObjectId(unsigned long oidServer)
{
    return m_pcExoAppInternal->ServerToClientObjectId(oidServer);
}

#ifdef _XBOX
void CClientExoApp::SetInputClass(int nClass)  // Xbox 0x0001B8B0
{
    m_pcExoAppInternal->SetInputClass(nClass);
}
#else
void CClientExoApp::SetInputClass(int nClass, int bHideInterface)  // PC 0x005EDA60
{
    m_pcExoAppInternal->SetInputClass(nClass, bHideInterface);
}
#endif
// PC 0x005EDA70  Xbox 0x0001B8C0
int CClientExoApp::GetInputClass()
{
    return m_pcExoAppInternal->m_nInputClass;
}
// PC 0x005EDA80  Xbox 0x0001B8D0
void CClientExoApp::ShowLoadScreen(int bFade, int bPlayMusic, int bSaving)
{
    m_pcExoAppInternal->ShowLoadScreen(bFade, bPlayMusic, bSaving);
}

#ifdef _XBOX
void CClientExoApp::HideLoadScreen(int bFade)  // Xbox 0x0001B8E0
{
    m_pcExoAppInternal->HideLoadScreen(bFade);
}
#else
void CClientExoApp::HideLoadScreen(int bFade, int bUpdateScreen)  // PC 0x005EDA90
{
    m_pcExoAppInternal->HideLoadScreen(bFade, bUpdateScreen);
}
#endif
// PC 0x005EDAA0  Xbox 0x0001B8F0
CExoString& CClientExoApp::SetPlayerCharacterName(const CExoString& sName)
{
    return m_pcExoAppInternal->SetPlayerCharacterName(sName);
}
// PC 0x005EDAB0  Xbox 0x0001B900
CExoString& CClientExoApp::GetPlayerCharacterName()
{
    return m_pcExoAppInternal->GetPlayerCharacterName();
}
// PC 0x005EDAC0  Xbox 0x0001B910
int CClientExoApp::GetPlayerCharacterSet()
{
    return m_pcExoAppInternal->m_bPlayerCharacterNameSet;
}
// PC 0x005EDAD0  Xbox 0x0001B920
int CClientExoApp::GetAreaNotReadyToDisplay()
{
    return m_pcExoAppInternal->m_nAreaNotReadyToDisplay;
}
// PC 0x005EDAE0  Xbox 0x0001B930
void CClientExoApp::SetAreaNotReadyToDisplay(int bNotReady)
{
    m_pcExoAppInternal->m_nAreaNotReadyToDisplay = bNotReady;
}
// PC 0x005EDAF0  Xbox 0x0001B940
CSWMiniGameObjectArray* CClientExoApp::GetMGOArray()
{
    return m_pcExoAppInternal->m_pMiniGameObjectArray;
}
// PC 0x005EDB00  Xbox 0x0001B950
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
// PC 0x005EDB20
int CClientExoApp::PlayLoadModuleMovies()
{
    return m_pcExoAppInternal->PlayLoadModuleMovies();
}
// PC 0x005EDB30  Xbox 0x0001B970
int CClientExoApp::PlayMovie(const CExoString& sMovie, int& nA, int nB, int nC)
{
    return m_pcExoAppInternal->PlayMovie(sMovie, nA, nB, nC);
}
// PC 0x005EDB40  Xbox 0x0001B980
int CClientExoApp::IsMoviePlaying()
{
    return m_pcExoAppInternal->IsMoviePlaying();
}
// PC 0x005EDB50  Xbox 0x0001B990
int CClientExoApp::AddMoveToModuleMovie(const CExoString& sMovie)
{
    return m_pcExoAppInternal->AddMoveToModuleMovie(sMovie);
}
// PC 0x005EDB60  Xbox 0x0001B9A0
CExoArrayList<CExoString>* CClientExoApp::GetMoveToModuleMovies()
{
    return &m_pcExoAppInternal->m_aMoveToModuleMovies;
}
// PC 0x005EDB70  Xbox 0x0001B9B0
void CClientExoApp::IndicateObject(CSWCObject* pObject, int nFlags)
{
    m_pcExoAppInternal->IndicateObject(pObject, nFlags);
}
// PC 0x005EDB80  Xbox 0x0001B9C0
int CClientExoApp::ReattachCreatureToScene(unsigned long oidObject)
{
    return oidObject != m_pcExoAppInternal->m_oidReattachedCreature;
}
// PC 0x005EDBA0  Xbox 0x0001B9E0
void CClientExoApp::ResetDriveAcceleration()
{
    m_pcExoAppInternal->ResetDriveAcceleration();
}
// PC 0x005EDBB0  Xbox 0x0001B9F0
void CClientExoApp::BarkString(unsigned long oidObject, const CExoString& sText,
                              const CResRef& cSound, int nFlags)
{
    m_pcExoAppInternal->BarkString(oidObject, sText, cSound, nFlags);
}
// PC 0x005EDBC0  Xbox 0x0001BA00
int CClientExoApp::IsSoundPlayingInDialog()
{
    return m_pcExoAppInternal->IsSoundPlayingInDialog();
}
// PC 0x005EDBD0  Xbox 0x0001BA10
void CClientExoApp::StopSoundPlayingInDialog()
{
    m_pcExoAppInternal->StopSoundPlayingInDialog();
}
// PC 0x005EDBE0  Xbox 0x0001BA20
void CClientExoApp::DisableInput()
{
    m_pcExoAppInternal->DisableInput();
}
// PC 0x005EDBF0  Xbox 0x0001BA30
void CClientExoApp::EnableInput()
{
    m_pcExoAppInternal->EnableInput();
}
// PC 0x005EDC00  Xbox 0x0001BA40
int CClientExoApp::DismissInGameGUI()
{
    return m_pcExoAppInternal->DismissInGameGUI();
}
// PC 0x005EDC10  Xbox 0x0001BA50
int CClientExoApp::GetPausedByCombat()
{
    return m_pcExoAppInternal->GetPausedByCombat();
}
// PC 0x005EDC20  Xbox 0x0001BA60
void CClientExoApp::SetPausedByCombat(int nA, unsigned char nB, int nC)
{
    m_pcExoAppInternal->SetPausedByCombat(nA, nB, nC);
}
// PC 0x005EDC30  Xbox 0x0001BA70
void CClientExoApp::ChangeCharacter(int nCharacter, int bPlaySound)
{
    m_pcExoAppInternal->ChangeCharacter(nCharacter, bPlaySound);
}
// PC 0x005EDC40  Xbox 0x0001BA80
void CClientExoApp::RunDeathSequence()
{
    m_pcExoAppInternal->RunDeathSequence();
}
// PC 0x005EDC50
int CClientExoApp::SetGameOver(int bGameOver)
{
    return m_pcExoAppInternal->SetGameOver(bGameOver);
}
// PC 0x005EDC60  Xbox 0x0001BA90
int CClientExoApp::GetGameOver()
{
    return m_pcExoAppInternal->m_nGameOver;
}
// PC 0x005EDC70  Xbox 0x0001BAA0
void CClientExoApp::SetLastPartyMemberTempKilled(unsigned long oidObject)
{
    m_pcExoAppInternal->SetLastPartyMemberTempKilled(oidObject);
}
// PC 0x005EDC80  Xbox 0x0001BAB0
void CClientExoApp::SetCanSendDialog(int bCanSend)
{
    m_pcExoAppInternal->SetCanSendDialog(bCanSend);
}
// PC 0x005EDC90  Xbox 0x0001BAC0
int CClientExoApp::GetCanSendDialog(unsigned long nA, unsigned long nB,
                                    CSWSDialog* pDialog)
{
    return m_pcExoAppInternal->GetCanSendDialog(nA, nB, pDialog);
}
// PC 0x005EDCA0  Xbox 0x0001BAD0
void CClientExoApp::SetDialogSent(int bSent)
{
    m_pcExoAppInternal->m_bDialogSent = bSent;
}

int CClientExoApp::GetDialogSent()
{
    return m_pcExoAppInternal->m_bDialogSent;
}
// PC 0x005EDCB0  Xbox 0x0001BAE0
void CClientExoApp::SetDialogModelsSent(int bSent)
{
    m_pcExoAppInternal->m_bDialogModelsSent = bSent;
}

int CClientExoApp::GetDialogModelsSent()
{
    return m_pcExoAppInternal->m_bDialogModelsSent;
}
// PC 0x005EDCC0  Xbox 0x0001BAF0
int CClientExoApp::GetClientRangedAnimation(unsigned long nA, int nB, int nC,
                                            int nD)
{
    return m_pcExoAppInternal->GetClientRangedAnimation(nA, nB, 0, 0);
}
// PC 0x005EDCE0  Xbox 0x0001BB10
int CClientExoApp::GetClientMeleeAnimation(unsigned long nA, unsigned long nB,
                                           unsigned short nC, int nD, int nE,
                                           int nF)
{
    return m_pcExoAppInternal->GetClientMeleeAnimation(nA, nB, nC, nD, nE, nF);
}
// PC 0x005EDCF0  Xbox 0x0001BB20
void CClientExoApp::SetLoadScreenByModuleName(const CExoString& sModule)
{
    m_pcExoAppInternal->SetLoadScreenByModuleName(sModule);
}
// PC 0x005EDD00  Xbox 0x0001BB30
CExoString CClientExoApp::GetLoadMusicByModuleName(const CExoString& sModule)
{
    return m_pcExoAppInternal->GetLoadMusicByModuleName(sModule);
}
// PC 0x005EDD30  Xbox 0x0001BB60
void CClientExoApp::SetLoadMusic(const CExoString& sMusic)
{
    m_pcExoAppInternal->m_sLoadMusic = sMusic;
}
// PC 0x005EDD40  Xbox 0x0001BB70
void CClientExoApp::SetLoadBarProgress(int nProgress, int bUpdateHint)
{
    m_pcExoAppInternal->SetLoadBarProgress(nProgress, bUpdateHint);
}
// PC 0x005EDD50  Xbox 0x0001BB80
void CClientExoApp::SetLoadFromIFO(int bValue)
{
    m_pcExoAppInternal->m_nLoadFromIFO = bValue;
}
// PC 0x005EDD60  Xbox 0x0001BB90
int CClientExoApp::GetLoadFromIFO()
{
    return m_pcExoAppInternal->m_nLoadFromIFO;
}
// PC 0x005EDD70  Xbox 0x0001BBA0
void CClientExoApp::SetLastTarget(unsigned long oidObject, int nB)
{
    m_pcExoAppInternal->SetLastTarget(oidObject, nB);
}
// PC 0x005EDD80  Xbox 0x0001BBB0
int CClientExoApp::GetLastTarget()
{
    return m_pcExoAppInternal->m_nLastTarget;
}
// PC 0x005EDD90  Xbox 0x0001BBC0
void CClientExoApp::SetRunScript(const CExoString& sScript)
{
    CClientExoAppInternal* pInternal = m_pcExoAppInternal;
    pInternal->m_bRunScript = 1;
    pInternal->m_sRunScript = sScript;
}
// PC 0x005EDDB0  Xbox 0x0001BBE0
void CClientExoApp::UpdateRoom(const Vector& vPosition)
{
    m_pcExoAppInternal->UpdateRoom(vPosition);
}
// PC 0x005EDDC0  Xbox 0x0001BBF0
void CClientExoApp::StartPazaakGame(int nSideDeck, const CExoString& sEndScript, int nWager, int bTutorial,
                                   unsigned long oidOpponent)
{
    m_pcExoAppInternal->StartPazaakGame(nSideDeck, sEndScript, nWager, bTutorial, oidOpponent);
}
// PC 0x005EDDD0  Xbox 0x0001BC00
void CClientExoApp::EndPazaakGame(int nResult, int nWager, int bPlayAgain,
                                 unsigned long oidOpponent)
{
    m_pcExoAppInternal->EndPazaakGame(nResult, nWager, bPlayAgain,
                                             oidOpponent);
}
// PC 0x005EDDE0  Xbox 0x0001BC10
int CClientExoApp::GetPazaakResults(int* pnResult, int* pnUnused)
{
    CClientExoAppInternal* pInternal = m_pcExoAppInternal;
    int nUnused;
    *pnResult = pInternal->m_PazaakState.m_nResult;
    nUnused = pInternal->m_PazaakState.m_nResultUnused;
    *pnUnused = nUnused;
    return nUnused;
}
// PC 0x005EDE00  Xbox 0x0001BC30
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
// PC 0x005EDE10  Xbox 0x0001BC40
void CClientExoApp::SetEndGamePending(int bPending, float fDelay, int nC)
{
    CClientExoAppInternal* pInternal = m_pcExoAppInternal;
    pInternal->m_bEndGamePending = bPending;
    pInternal->m_fEndGamePendingDelay = fDelay;
    pInternal->m_nEndGamePendingUnknown = nC;
}
// PC 0x005EDE40  Xbox 0x0001BC70
int CClientExoApp::PlayRumblePattern(int nPattern)
{
    return m_pcExoAppInternal->PlayRumblePattern(nPattern);
}
// PC 0x005EDE50  Xbox 0x0001BC80
int CClientExoApp::StopRumblePattern(int nPattern)
{
    return m_pcExoAppInternal->StopRumblePattern(nPattern);
}
// PC 0x005EDE60  Xbox 0x0001BC90
void CClientExoApp::TogglePartyFollow()
{
    m_pcExoAppInternal->TogglePartyFollow();
}
// PC 0x005EDE70  Xbox 0x0001BCA0
int CClientExoApp::GetCombatMode()
{
    return m_pcExoAppInternal->m_nCombatMode;
}
// PC 0x005EDE80  Xbox 0x0001BCB0
void CClientExoApp::SetCombatMode(int nMode)
{
    m_pcExoAppInternal->SetCombatMode(nMode);
}
// PC 0x005EDE90  Xbox 0x0001BCC0
void CClientExoApp::PlayerFlourishWeapons()
{
    m_pcExoAppInternal->PlayerFlourishWeapons();
}
// PC 0x005EDEA0  Xbox 0x0001BCD0
void CClientExoApp::AddFloatyText(unsigned long oidObject, unsigned char nType,
                               int nValue)
{
    m_pcExoAppInternal->AddFloatyText(oidObject, nType, nValue);
}
// PC 0x005EDEB0  Xbox 0x0001BCE0
CExoString CClientExoApp::GetGUIString(unsigned long nStrRef)
{
    return m_pcExoAppInternal->GetGUIString(nStrRef);
}
// PC 0x005EDEE0  Xbox 0x0001BD10
void CClientExoApp::SetAutoPaused(int bAuto, unsigned char nB)
{
    m_pcExoAppInternal->SetAutoPaused(bAuto, nB);
}
// PC 0x005EDEF0  Xbox 0x0001BD20
int CClientExoApp::GetAutoPaused()
{
#ifdef _XBOX
    return m_pcExoAppInternal->m_PauseFlags.m_Bits.m_bAutoPaused;
#else
    return m_pcExoAppInternal->m_AutoPauseFlags.m_Bits.m_bAutoPaused;
#endif
}
// PC 0x005EDF00  Xbox 0x0001BD30
void CClientExoApp::SetLoadScreenHint(unsigned long nHint)
{
    m_pcExoAppInternal->SetLoadScreenHint(nHint);
}
// Xbox 0x0001BD40
unsigned long CClientExoApp::GetLoadScreenHint()
{
    return m_pcExoAppInternal->GetLoadScreenHint();
}
// PC 0x005EDF10  Xbox 0x0001BD50
void CClientExoApp::EnableVideoEffect(int nEffect)
{
    m_pcExoAppInternal->EnableVideoEffect(nEffect);
}
// PC 0x005EDF20  Xbox 0x0001BD60
void CClientExoApp::DisableVideoEffect()
{
    m_pcExoAppInternal->DisableVideoEffect();
}
// PC 0x005EDF30  Xbox 0x0001BD70
void CClientExoApp::DelayNextEnemyAutoPause()
{
    m_pcExoAppInternal->DelayNextEnemyAutoPause();
}
// PC 0x005EDF40  Xbox 0x0001BD80
int CClientExoApp::ShowTutorialWindow(unsigned char nTutorial, unsigned long nX,
                                      unsigned long nY, unsigned long nActionParam)
{
    return m_pcExoAppInternal->ShowTutorialWindow(nTutorial, nX, nY, nActionParam);
}
// PC 0x005EDF50  Xbox 0x0001BD90
int CClientExoApp::ClearCachedCreatures(CSWCCreature* pCreature)
{
    return m_pcExoAppInternal->ClearCachedCreatures(pCreature);
}
// PC 0x005EDF60  Xbox 0x0001BDA0
void CClientExoApp::LookUpAndPerformRumbleWithCutOff(int nEntry,
                                                     unsigned char nTable,
                                                     const Vector& vPosition)
{
    m_pcExoAppInternal->LookUpAndPerformRumbleWithCutOff(nEntry, nTable,
                                                         vPosition);
}
// PC 0x005EDF70  Xbox 0x0001BE00
int CClientExoApp::SaveOptions()
{
    return m_pcExoAppInternal->SaveOptions();
}

#ifdef _XBOX
int CClientExoApp::LoadOptions()  // Xbox 0x0001BE10
{
    return m_pcExoAppInternal->LoadOptions();
}
#endif
// PC 0x005EDF80
int CClientExoApp::ChangeCharacterToNextLivingPartyMember(int nA, int nB,
                                                          unsigned char nC)
{
    return m_pcExoAppInternal->ChangeCharacterToNextLivingPartyMember(nA, nB,
                                                                      nC);
}
// PC 0x005ED310
void CClientExoApp::StopMovie()
{
    m_pcExoAppInternal->StopMovie();
}
// PC 0x005EDF90  Xbox 0x0001BE30
void CClientExoApp::SetAttackMashTutorialTimer(unsigned long nTimer)
{
    m_pcExoAppInternal->m_nAttackMashTutorialTimer = nTimer;
}
// PC 0x005EDFA0  Xbox 0x0001BE40
unsigned long CClientExoApp::GetAttackMashTutorialTimer()
{
    return m_pcExoAppInternal->m_nAttackMashTutorialTimer;
}

#ifdef _XBOX
void CClientExoApp::SetMinGlobalRim(int bMinGlobalRim)  // Xbox 0x0001BE50
{
    m_pcExoAppInternal->m_bMinGlobalRim = bMinGlobalRim;
}
#endif
// PC 0x005EDFB0  Xbox 0x0001BE60
int CClientExoApp::GetLoadingSaveGame()
{
    return m_pcExoAppInternal->m_nLoadingSaveGame;
}
// PC 0x005EDFC0  Xbox 0x0001BE70
void CClientExoApp::SetLoadingSaveGame(int bLoading)
{
    m_pcExoAppInternal->m_nLoadingSaveGame = bLoading;
}
// PC 0x005EDFD0  Xbox 0x0001BE80
void CClientExoApp::SetLoadStep(int nStep, unsigned char cValue)
{
    m_pcExoAppInternal->m_aLoadSteps[nStep] = cValue;
}
// PC 0x005EDFF0  Xbox 0x0001BEA0
unsigned char CClientExoApp::GetLoadStep(int nStep)
{
    return m_pcExoAppInternal->m_aLoadSteps[nStep];
}
// PC 0x005EE010  Xbox 0x0001BEC0
void CClientExoApp::ClearEnemySighted()
{
    m_pcExoAppInternal->m_bEnemySighted = 0;
}
// PC 0x005EE020  Xbox 0x0001BED0
CExoString& CClientExoApp::GetLiveContentName(int nIndex)
{
    return m_pcExoAppInternal->m_aLiveContentNames[nIndex].m_sName;
}
// Xbox 0x0001BEF0
int CClientExoApp::GetLiveContentExists(int nIndex)
{
    return m_pcExoAppInternal->m_aLiveContentNames[nIndex].m_bExists;
}
// Xbox 0x0001BF10
int CClientExoApp::GetLiveContentCorrupt(int nIndex)
{
    return m_pcExoAppInternal->m_aLiveContentNames[nIndex].m_bCorrupt;
}
// Xbox 0x0001BF30
unsigned long CClientExoApp::GetLoadScreenHintUpdateTimer()
{
    return m_pcExoAppInternal->m_nLoadScreenHintUpdateTimer;
}
// PC 0x005EE040  Xbox 0x0001BF40
void CClientExoApp::SetLoadScreenHintUpdateTimer(unsigned long nTimer)
{
    m_pcExoAppInternal->m_nLoadScreenHintUpdateTimer = nTimer;
}
// PC 0x005EE050  Xbox 0x0001BF50
unsigned char CClientExoApp::GetGamePlayHint()
{
    return m_pcExoAppInternal->m_nGamePlayHint;
}
// PC 0x005EE060  Xbox 0x0001BF60
void CClientExoApp::SetGamePlayHint(unsigned char nHint)
{
    m_pcExoAppInternal->m_nGamePlayHint = nHint;
}
// PC 0x005EE070  Xbox 0x0001BF70
unsigned char CClientExoApp::GetStoryHint()
{
    return m_pcExoAppInternal->m_nStoryHint;
}
// PC 0x005EE080  Xbox 0x0001BF80
void CClientExoApp::SetStoryHint(unsigned char nHint)
{
    m_pcExoAppInternal->m_nStoryHint = nHint;
}
// PC 0x005EE090  Xbox 0x0001BF90
int CClientExoApp::GetNextLoadScreenHintSTRREF(const CExoString& sModule,
                                               unsigned char nB)
{
    return m_pcExoAppInternal->GetNextLoadScreenHintSTRREF(sModule, nB);
}
// PC 0x005EE0A0  Xbox 0x0001BFA0
void CClientExoApp::SetModuleResourceLoaded(unsigned char nMask)
{
    m_pcExoAppInternal->m_nModuleResourcesLoaded |= nMask;
}
// PC 0x005EE0C0  Xbox 0x0001BFC0
void CClientExoApp::ResetModuleResourcesLoaded()
{
    m_pcExoAppInternal->m_nModuleResourcesLoaded = 0;
}
// PC 0x005EE0D0  Xbox 0x0001BFD0
int CClientExoApp::GetModuleResourceLoaded(unsigned char nMask)
{
    unsigned char nLoaded =
        m_pcExoAppInternal->m_nModuleResourcesLoaded & nMask;
    return nLoaded > 0;
}
// PC 0x005EE0F0  Xbox 0x0001BFF0
int CClientExoApp::GetTargetChanging()
{
    return m_pcExoAppInternal->m_fTargetChanging > 0.0f;
}
// PC 0x005EE110  Xbox 0x0001C010
int CClientExoApp::GetTogglePauseAtEndOfFrame()
{
    return m_pcExoAppInternal->m_PauseFlags.m_Bits.m_bTogglePauseAtEndOfFrame !=
           0;
}
// PC 0x005EE120  Xbox 0x0001C020
void CClientExoApp::BusyWaitMenuMusic()
{
    m_pcExoAppInternal->BusyWaitMenuMusic();
}
// PC 0x005EE130  Xbox 0x0001C030
void CClientExoApp::BusyWaitMuteSound()
{
    m_pcExoAppInternal->BusyWaitMuteSound();
}
// PC 0x005EE140  Xbox 0x0001C050
int CClientExoApp::GetPazaakIsRunning()
{
    return m_pcExoAppInternal->m_PazaakState.m_bIsRunning;
}
// PC 0x005EE150
void CClientExoApp::SetMouseMode(unsigned char nMode)
{
    m_pcExoAppInternal->SetMouseMode(nMode);
}
// PC 0x005EE160
void CClientExoApp::CancelMovie()
{
    m_pcExoAppInternal->CancelMovie();
}
// PC 0x005EE170
void CClientExoApp::HideMouse(int nFlags)
{
    m_pcExoAppInternal->HideMouse(nFlags);
}
// PC 0x005EE180
void CClientExoApp::ShowMouse(int nFlags)
{
    m_pcExoAppInternal->ShowMouse(nFlags);
}
// PC 0x005EE190
int CClientExoApp::GetCharacterChangeInProgress()
{
    return m_pcExoAppInternal->m_bCharacterChangeInProgress;
}
// PC 0x005EE1A0
void CClientExoApp::SetCharacterChangeInProgress(int bInProgress)
{
    m_pcExoAppInternal->m_bCharacterChangeInProgress = bInProgress;
}
// PC 0x005EE1B0
void CClientExoApp::FadeAndStopStreamingSoundSource(
    CExoStreamingSoundSource* pSource, unsigned long nB)
{
    m_pcExoAppInternal->FadeAndStopStreamingSoundSource(pSource, nB);
}
// PC 0x005EE1C0
void CClientExoApp::StartCreditSequence(const Vector& vColor,
                                      const CExoString& sMusic)
{
    m_pcExoAppInternal->StartCreditSequence(vColor, sMusic);
}
// PC 0x005EE1D0
void CClientExoApp::StopCreditSequence()
{
    m_pcExoAppInternal->StopCreditSequence();
}
// PC 0x005EE1E0
int CClientExoApp::GetCreditSequenceInProgress()
{
    return m_pcExoAppInternal->m_bCreditSequenceInProgress;
}

void CClientExoApp::SetCreditSequenceInProgress(int bInProgress)
{
    m_pcExoAppInternal->m_bCreditSequenceInProgress = bInProgress;
}
// PC 0x005EE1F0
void CClientExoApp::EnableMouseAtEndOfLoadModuleMovie()
{
    m_pcExoAppInternal->EnableMouseAtEndOfLoadModuleMovie();
}
// PC 0x005EE200
void CClientExoApp::SetLastClickedOnTarget(unsigned long oidObject)
{
    m_pcExoAppInternal->m_oidLastClickedOnTarget = oidObject;
}

unsigned long CClientExoApp::GetLastClickedOnTarget()
{
    return m_pcExoAppInternal->m_oidLastClickedOnTarget;
}

#ifndef _XBOX
int CClientExoApp::AddMovieToMovieQueue(const CExoString& sMovie, int nFlags)  // PC 0x005EE210
{
    return m_pcExoAppInternal->AddMovieToMovieQueue(sMovie, nFlags);
}
// PC 0x005EE220
int CClientExoApp::PlayMovieQueue(int nFlags)
{
    return m_pcExoAppInternal->PlayMovieQueue(nFlags);
}
#endif

#ifdef _XBOX

#include "swplayercontrol.h"
int CClientExoApp::GetInFreeLook()  // PC 0x005EE230  Xbox 0x0001BDB0
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
int CClientExoApp::GetInFreeLook()  // PC 0x005EE230  Xbox 0x0001BDB0
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
// PC 0x005EE280
CExoString CClientExoApp::GetDisplayCharacterForKey(int nKey)
{
    return m_pcExoAppInternal->GetDisplayCharacterForKey(nKey);
}
// PC 0x005EE2B0
int CClientExoApp::IsValidResolution(unsigned long nWidth,
                                     unsigned long nHeight)
{
    return m_pcExoAppInternal->IsValidResolution(nWidth, nHeight);
}
// PC 0x005EE2C0
int CClientExoApp::GetAllowHighMonitorFrequency()
{
    return m_pcExoAppInternal->m_bAllowHighMonitorFrequency;
}
// PC 0x005C5630
void CClientExoApp::PlayerListChange(unsigned long, int, int)
{
}
// PC 0x005EE2D0
int CClientExoApp::GetSoundPausedByCombat()
{
    return m_pcExoAppInternal->m_nSoundPausedByCombat;
}

void CClientExoApp::SetSoundPausedByCombat(int nPaused)
{
    m_pcExoAppInternal->m_nSoundPausedByCombat = nPaused;
}
// PC 0x005EE2E0
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
// PC 0x005EE2F0  Xbox 0x0001C130
CClientExoApp::CClientExoApp()
{
    m_pcExoAppInternal = new CClientExoAppInternal();
}
// PC 0x005EE360  Xbox 0x0001C1B0
CClientExoApp::~CClientExoApp()
{
    if (m_pcExoAppInternal != NULL)
    {
        delete m_pcExoAppInternal;
    }
}
// PC 0x005EE380
void CClientExoApp::RemoveMoveToModuleMovies()
{
    m_pcExoAppInternal->m_aMoveToModuleMovies.RemoveAll(0);
}

#ifdef _XBOX
int CClientExoApp::ChangeCharacterToNextLivingPartyMember(int nA, int nB)  // Xbox 0x0001BE20
{
    return m_pcExoAppInternal->ChangeCharacterToNextLivingPartyMember(nA, nB);
}
#endif
