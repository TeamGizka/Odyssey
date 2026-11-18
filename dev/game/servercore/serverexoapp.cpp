#include "exobase.h"
#include "exoresman.h"
#include "appmanager.h"
#include "serverexoapp.h"
#include "nwsobject.h"
#include "nwsmodule.h"
#include "nwsarea.h"
#include "nwscombatround.h"
#include "partytables.h"
#include "nwsvartable.h"
#include "serverexoappinternal.h"
// PC 0x004DBE40 GOG 0x004DC550 Xbox 0x00171AE0
CServerInfo* CServerExoApp::GetServerInfo()
{
    return m_pcExoAppInternal->m_pServerInfo;
}
// PC 0x004DBE50 GOG 0x004DBF80 Xbox 0x00171540
CSWSModule* CServerExoApp::GetModule()
{
    return m_pcExoAppInternal->GetModule();
}
// PC 0x004DBE60 GOG 0x004DBF90 Xbox 0x00171550
CExoString CServerExoApp::GetModuleName()
{
    return m_pcExoAppInternal->GetModuleName();
}
// PC 0x004DBE80 GOG 0x004DBFB0 Xbox 0x00171570
int CServerExoApp::SaveGame(unsigned long nSlot, const CExoString& sName,
                            CSWSPlayer* pPlayer, int bAutoSave)
{
    return m_pcExoAppInternal->SaveGame(nSlot, sName, pPlayer, bAutoSave);
}

// Dead code: not in the PC executable
int CServerExoApp::SendCharacterQuery(CSWSPlayer* pPlayer)
{
    return m_pcExoAppInternal->SendCharacterQuery(pPlayer);
}
// PC 0x004DBE90 GOG 0x004DBFC0 Xbox 0x00171580
int CServerExoApp::LoadGame(unsigned long nSlot, const CExoString& sName,
                            const CExoString& sModule, CSWSPlayer* pPlayer)
{
    return m_pcExoAppInternal->LoadGame(nSlot, sName, sModule, pPlayer);
}

// Dead code: not in the PC executable
int CServerExoApp::StartNewModule(const CExoString& sModule)
{
    return m_pcExoAppInternal->StartNewModule(sModule);
}
// PC 0x004DBEA0 GOG 0x004DC580 Xbox 0x00171B10
void CServerExoApp::SetMoveToModulePending(int bPending)
{
    m_pcExoAppInternal->m_bMoveToModulePending = bPending;
}
// PC 0x004DBEB0 GOG 0x004DC590 Xbox 0x00171B20
int CServerExoApp::GetMoveToModulePending()
{
    return m_pcExoAppInternal->m_bMoveToModulePending;
}
// PC 0x004DBEC0 GOG 0x004DC5A0 Xbox 0x00171B30
void CServerExoApp::SetMoveToModuleString(const CExoString& sModule)
{
    m_pcExoAppInternal->m_sMoveToModule = sModule;
}

// Dead code: not in the PC executable
CExoString CServerExoApp::GetMoveToModuleString()
{
    return m_pcExoAppInternal->m_sMoveToModule;
}

// Dead code: not in the PC executable
int CServerExoApp::GetExportAllCharactersPending()
{
    return m_pcExoAppInternal->m_bExportAllCharactersPending;
}
// PC 0x004DBED0 GOG 0x004DC5B0 Xbox 0x00171B40
void CServerExoApp::SetExportAllCharactersPending(int bPending)
{
    m_pcExoAppInternal->m_bExportAllCharactersPending = bPending;
}

// Dead code: not in the PC executable
int CServerExoApp::ExportAllPlayers()
{
    return m_pcExoAppInternal->ExportAllPlayers();
}
// PC 0x004DBEE0 GOG 0x004DC5C0 Xbox 0x00171B50
int CServerExoApp::GetLoadingModule()
{
    return m_pcExoAppInternal->m_bLoadingModule;
}
// PC 0x004DBEF0 GOG 0x004DC5D0 Xbox 0x00171B60
void CServerExoApp::SetLoadingModule(int bLoading)
{
    m_pcExoAppInternal->m_bLoadingModule = bLoading;
}
// PC 0x004DBF00 GOG 0x004DC5E0 Xbox 0x00171B70
void CServerExoApp::SetAutoSavePending(int bPending)
{
    m_pcExoAppInternal->m_bAutoSavePending = bPending;
}
// PC 0x004DBF10 GOG 0x004DC5F0 Xbox 0x00171B80
int CServerExoApp::GetAutoSavePending()
{
    return m_pcExoAppInternal->m_bAutoSavePending;
}
// PC 0x004DBF20 GOG 0x004DC600 Xbox 0x00171B90
void CServerExoApp::SetMoveToModuleStartWaypoint(const CExoString& sWaypoint)
{
    m_pcExoAppInternal->m_sMoveToModuleStartWaypoint = sWaypoint;
}
// PC 0x004DBF30 GOG 0x004DC610
CExoString CServerExoApp::GetMoveToModuleStartWaypoint()
{
    return m_pcExoAppInternal->m_sMoveToModuleStartWaypoint;
}
// PC 0x004DBF60 GOG 0x004DBFD0 Xbox 0x00171590
int CServerExoApp::LoadCharacterStart(unsigned char nType, CSWSPlayer* pPlayer,
                                      CResRef cResRef, void* pData,
                                      unsigned long nSize)
{
    return m_pcExoAppInternal->LoadCharacterStart(nType, pPlayer, cResRef,
                                                  pData, nSize);
}
// PC 0x004DBFA0 GOG 0x004DC010 Xbox 0x001715D0
int CServerExoApp::LoadCharacterFinish(CSWSPlayer* pPlayer, int bSuccess)
{
    return m_pcExoAppInternal->LoadCharacterFinish(pPlayer, bSuccess);
}

// Dead code: not in the PC executable
int CServerExoApp::LoadPrimaryPlayer(CSWSPlayer* pPlayer)
{
    return m_pcExoAppInternal->LoadPrimaryPlayer(pPlayer);
}
// PC 0x004DBFB0 GOG 0x004DC640 Xbox 0x00171BA0
CGameObjectArray* CServerExoApp::GetObjectArray()
{
    return m_pcExoAppInternal->m_pObjectArray;
}
// PC 0x004DBFC0 GOG 0x004DC650 Xbox 0x00171BB0
CGameObject* CServerExoApp::GetGameObject(unsigned long oidObject)
{
    return m_pcExoAppInternal->GetGameObject(oidObject);
}
// PC 0x004DBFF0 GOG 0x004DC020 Xbox 0x001715E0
CSWSItem* CServerExoApp::GetItemByGameObjectID(unsigned long oidObject)
{
    return m_pcExoAppInternal->GetItemByGameObjectID(oidObject);
}
// PC 0x004DC000 GOG 0x004DC030 Xbox 0x001715F0
CSWSCreature* CServerExoApp::GetCreatureByGameObjectID(unsigned long oidObject)
{
    return m_pcExoAppInternal->GetCreatureByGameObjectID(oidObject);
}

// Dead code: not in the PC executable
CSWSModule* CServerExoApp::GetModuleByGameObjectID(unsigned long oidObject)
{
    return m_pcExoAppInternal->GetModuleByGameObjectID(oidObject);
}
// PC 0x004DC010 GOG 0x004DC040 Xbox 0x00171600
CSWSArea* CServerExoApp::GetAreaByGameObjectID(unsigned long oidObject)
{
    return m_pcExoAppInternal->GetAreaByGameObjectID(oidObject);
}
// PC 0x004DC020 GOG 0x004DC050 Xbox 0x00171610
CSWSTrigger* CServerExoApp::GetTriggerByGameObjectID(unsigned long oidObject)
{
    return m_pcExoAppInternal->GetTriggerByGameObjectID(oidObject);
}
// PC 0x004DC030 GOG 0x004DC060 Xbox 0x00171620
CSWSPlaceable* CServerExoApp::GetPlaceableByGameObjectID(
    unsigned long oidObject)
{
    return m_pcExoAppInternal->GetPlaceableByGameObjectID(oidObject);
}
// PC 0x004DC040 GOG 0x004DC070 Xbox 0x00171630
CSWSStore* CServerExoApp::GetStoreByGameObjectID(unsigned long oidObject)
{
    return m_pcExoAppInternal->GetStoreByGameObjectID(oidObject);
}
// PC 0x004DC050 GOG 0x004DC080 Xbox 0x00171640
CSWSDoor* CServerExoApp::GetDoorByGameObjectID(unsigned long oidObject)
{
    return m_pcExoAppInternal->GetDoorByGameObjectID(oidObject);
}
// PC 0x004DC060 GOG 0x004DC090 Xbox 0x00171650
CSWSAreaOfEffectObject* CServerExoApp::GetAreaOfEffectByGameObjectID(
    unsigned long oidObject)
{
    return m_pcExoAppInternal->GetAreaOfEffectByGameObjectID(oidObject);
}
// PC 0x004DC070 GOG 0x004DC0A0 Xbox 0x00171660
CSWSWaypoint* CServerExoApp::GetWaypointByGameObjectID(unsigned long oidObject)
{
    return m_pcExoAppInternal->GetWaypointByGameObjectID(oidObject);
}
// PC 0x004DC080 GOG 0x004DC0B0 Xbox 0x00171670
CSWSEncounter* CServerExoApp::GetEncounterByGameObjectID(
    unsigned long oidObject)
{
    return m_pcExoAppInternal->GetEncounterByGameObjectID(oidObject);
}
// PC 0x004DC090 GOG 0x004DC0C0 Xbox 0x00171680
CSWSSoundObject* CServerExoApp::GetSoundObjectByGameObjectID(
    unsigned long oidObject)
{
    return m_pcExoAppInternal->GetSoundObjectByGameObjectID(oidObject);
}

// Dead code: not in the PC executable
unsigned long CServerExoApp::GetPlayerIDByGameObjectID(unsigned long oidObject)
{
    return m_pcExoAppInternal->GetPlayerIDByGameObjectID(oidObject);
}
// PC 0x004DC0A0 GOG 0x004DC0D0 Xbox 0x00171690
int CServerExoApp::GetPlayerLanguage(unsigned long nPlayerId)
{
    return m_pcExoAppInternal->GetPlayerLanguage(nPlayerId);
}
// PC 0x004DC0B0 GOG 0x004DC0E0 Xbox 0x001716A0
int CServerExoApp::GetModuleLanguage()
{
    return m_pcExoAppInternal->GetModuleLanguage();
}
// PC 0x004DC0C0 GOG 0x004DC680 Xbox 0x00171BE0
CServerAIMaster* CServerExoApp::GetServerAIMaster()
{
    return m_pcExoAppInternal->m_pServerAIMaster;
}
// PC 0x004DC0D0 GOG 0x004DC690 Xbox 0x00171BF0
CExoLinkedList<CSWSClient>* CServerExoApp::GetPlayerList()
{
    return m_pcExoAppInternal->m_pPlayerList;
}
// PC 0x004DC0E0 GOG 0x004DC6A0 Xbox 0x00171C00
CNetLayer* CServerExoApp::GetNetLayer()
{
    return m_pcExoAppInternal->m_pNetLayer;
}
// PC 0x004DC0F0 GOG 0x004DC6B0 Xbox 0x00171C10
CSWSMessage* CServerExoApp::GetSWSMessage()
{
    return m_pcExoAppInternal->m_pSWSMessage;
}
// PC 0x004DC100 GOG 0x004DC6C0 Xbox 0x00171C20
CExoLinkedList<CSWSClient>* CServerExoApp::GetSysAdminList()
{
    return m_pcExoAppInternal->m_pSysAdminList;
}
// PC 0x004DC110 GOG 0x004DC6D0 Xbox 0x00171C30
short CServerExoApp::GetServerMode()
{
    return m_pcExoAppInternal->m_nServerMode;
}
// PC 0x004DC120 GOG 0x004DC0F0 Xbox 0x001716B0
CWorldTimer* CServerExoApp::GetActiveTimer(unsigned long nTimer)
{
    return m_pcExoAppInternal->GetActiveTimer(nTimer);
}
// PC 0x004DC130 GOG 0x004DC6E0 Xbox 0x00171C40
CWorldTimer* CServerExoApp::GetWorldTimer()
{
    return m_pcExoAppInternal->m_pWorldTimer;
}
// PC 0x004DC140 GOG 0x004DC6F0 Xbox 0x00171C50
CWorldTimer* CServerExoApp::GetTimestopTimer()
{
    return m_pcExoAppInternal->m_pTimestopTimer;
}
// PC 0x004DC150 GOG 0x004DC700 Xbox 0x00171C60
CWorldTimer* CServerExoApp::GetPauseTimer()
{
    return m_pcExoAppInternal->m_pPauseTimer;
}
// PC 0x004DC160 GOG 0x004DC710 Xbox 0x00171C70
void CServerExoApp::SetDebugMode(int nDebugMode)
{
    m_pcExoAppInternal->m_nDebugMode = nDebugMode;
}
// PC 0x004DC170 GOG 0x004DC720 Xbox 0x00171C80
int CServerExoApp::GetDebugMode()
{
    return m_pcExoAppInternal->m_nDebugMode;
}
// PC 0x004DC180 GOG 0x004DC100 Xbox 0x001716C0
int CServerExoApp::HandleMessage(unsigned long nPlayerId,
                                 unsigned char* pMessage, unsigned long nSize,
                                 int bGuaranteed)
{
    return m_pcExoAppInternal->HandleMessage(nPlayerId, pMessage, nSize,
                                             bGuaranteed);
}
// PC 0x004DC190 GOG 0x004DC110 Xbox 0x001716D0
void CServerExoApp::Initialize()
{
    m_pcExoAppInternal->Initialize();
}
// PC 0x004DC1A0 GOG 0x004DC120 Xbox 0x001716E0
int CServerExoApp::MainLoop()
{
    return m_pcExoAppInternal->MainLoop();
}
// PC 0x004DC1B0 GOG 0x004DC130
void CServerExoApp::PlayerListChange(unsigned long nPlayerId, int bAdded,
                                     int bGuaranteed)
{
    m_pcExoAppInternal->PlayerListChange(nPlayerId, bAdded, bGuaranteed);
}

// Dead code: not in the PC executable
void CServerExoApp::DumpServerOptionsToLog()
{
    m_pcExoAppInternal->DumpServerOptionsToLog();
}
// PC 0x004DC1C0 GOG 0x004DC140 Xbox 0x00171700
int CServerExoApp::ContinueMessageProcessing()
{
    return m_pcExoAppInternal->ContinueMessageProcessing();
}
// PC 0x004DC1D0 GOG 0x004DC150 Xbox 0x00171710
void CServerExoApp::Shutdown(int bExit, int bSave)
{
    m_pcExoAppInternal->Shutdown(bExit, bSave);
}
// PC 0x004DC1E0 GOG 0x004DC160
int CServerExoApp::UnloadModule()
{
    return m_pcExoAppInternal->UnloadModule();
}
// PC 0x004DC1F0 GOG 0x004DC170 Xbox 0x00171720
int CServerExoApp::LoadModule(const CExoString& sModule, CSWSPlayer* pPlayer)
{
    return m_pcExoAppInternal->LoadModule(sModule, pPlayer);
}
// PC 0x004DC200 GOG 0x004DC180 Xbox 0x00171730
int CServerExoApp::RunModule()
{
    return m_pcExoAppInternal->RunModule();
}
// PC 0x004DC210 GOG 0x004DC190 Xbox 0x00171740
CExoString CServerExoApp::GetPlayerListString()
{
    return m_pcExoAppInternal->GetPlayerListString();
}

// Dead code: not in the PC executable
void CServerExoApp::OnCDChange()
{
    m_pcExoAppInternal->OnCDChange();
}

// Dead code: not in the PC executable
void CServerExoApp::OnExit()
{
    m_pcExoAppInternal->OnExit();
}

// Dead code: not in the PC executable
void CServerExoApp::OnGainFocus()
{
    m_pcExoAppInternal->OnGainFocus();
}

// Dead code: not in the PC executable
void CServerExoApp::OnLostFocus()
{
    m_pcExoAppInternal->OnLostFocus();
}

// Dead code: not in the PC executable
void CServerExoApp::OnVideoChange()
{
    m_pcExoAppInternal->OnVideoChange();
}

// Dead code: not in the PC executable
void CServerExoApp::SetForceUpdate()
{
    m_pcExoAppInternal->m_bForceUpdate = 1;
}
// PC 0x004DC230 GOG 0x004DC730 Xbox 0x00171C90
CConnectionLib* CServerExoApp::GetConnectionLib()
{
    return m_pcExoAppInternal->m_pConnectionLib;
}
// PC 0x004DC240 GOG 0x004DC1B0 Xbox 0x00171770
void CServerExoApp::StartServices(int nServices)
{
    m_pcExoAppInternal->StartServices(nServices);
}
// PC 0x004DC250 GOG 0x004DC1C0 Xbox 0x00171780
void CServerExoApp::StopServices()
{
    m_pcExoAppInternal->StopServices();
}
// PC 0x004DC260 GOG 0x004DC1D0 Xbox 0x00171790
CSWSPlayer* CServerExoApp::GetClientObjectByObjectId(unsigned long oidObject)
{
    return m_pcExoAppInternal->GetClientObjectByObjectId(oidObject);
}
// PC 0x004DC270 GOG 0x004DC1E0 Xbox 0x001717A0
CSWSPlayer* CServerExoApp::GetClientObjectByPlayerId(unsigned long nPlayerId,
                                                     unsigned char nGroup)
{
    return m_pcExoAppInternal->GetClientObjectByPlayerId(nPlayerId, nGroup);
}

// Dead code: not in the PC executable
int CServerExoApp::ValidatePlayerLogin(void* pPlayerData)
{
    return m_pcExoAppInternal->ValidatePlayerLogin(pPlayerData);
}
// PC 0x004DC280 GOG 0x004DC1F0 Xbox 0x001717B0
void CServerExoApp::InitiateModuleForPlayer(void* pData)
{
    m_pcExoAppInternal->InitiateModuleForPlayer(pData);
}
// PC 0x004DC290 GOG 0x004DC200 Xbox 0x001717C0
void CServerExoApp::MovePlayerToArea(void* pData)
{
    m_pcExoAppInternal->MovePlayerToArea(pData);
}

// Dead code: not in the PC executable
int CServerExoApp::GetIsControlledByPlayer(unsigned long oidObject)
{
    return m_pcExoAppInternal->GetClientObjectByObjectId(oidObject) != NULL;
}
// PC 0x004DC2A0 GOG 0x004DC210 Xbox 0x001717D0
int CServerExoApp::GetFactionOfObject(unsigned long oidObject, int* pnFaction)
{
    return m_pcExoAppInternal->GetFactionOfObject(oidObject, pnFaction);
}
// PC 0x004DC2B0 GOG 0x004DC220 Xbox 0x001717E0
void CServerExoApp::TogglePauseState(unsigned char nPauseType)
{
    m_pcExoAppInternal->TogglePauseState(nPauseType);
}
// PC 0x004DC2C0 GOG 0x004DC230 Xbox 0x001717F0
void CServerExoApp::SkipPastDialog(unsigned long oidObject)
{
    m_pcExoAppInternal->SkipPastDialog(oidObject);
}
// PC 0x004DC2D0 GOG 0x004DC240 Xbox 0x00171800
int CServerExoApp::GetPauseState(unsigned char nPauseType)
{
    return m_pcExoAppInternal->GetPauseState(nPauseType);
}
// PC 0x004DC2E0 GOG 0x004DC250 Xbox 0x00171810
unsigned char CServerExoApp::GetActivePauseState()
{
    return m_pcExoAppInternal->GetActivePauseState();
}
// PC 0x004DC2F0 GOG 0x004DC260 Xbox 0x00171820
void CServerExoApp::SetPauseState(unsigned char nPauseType, int bPause)
{
    m_pcExoAppInternal->SetPauseState(nPauseType, bPause);
}
// PC 0x004DC300 GOG 0x004DC270 Xbox 0x00171830
void CServerExoApp::AddToExclusionList(unsigned long oidObject,
                                      unsigned char nType)
{
    m_pcExoAppInternal->AddToExclusionList(oidObject, nType);
}
// PC 0x004DC310 GOG 0x004DC280 Xbox 0x00171840
void CServerExoApp::RemoveFromExclusionList(unsigned long oidObject,
                                           unsigned char nType)
{
    m_pcExoAppInternal->RemoveFromExclusionList(oidObject, nType);
}
// PC 0x004DC320 GOG 0x004DC290 Xbox 0x00171850
CExoLinkedList<unsigned long>* CServerExoApp::GetActiveExclusionList()
{
    return m_pcExoAppInternal->GetActiveExclusionList();
}

// Dead code: not in the PC executable
int CServerExoApp::IsOnActiveExclusionList(unsigned long oidObject)
{
    return m_pcExoAppInternal->IsOnActiveExclusionList(oidObject);
}
// PC 0x004DC330 GOG 0x004DC2A0 Xbox 0x00171860
int CServerExoApp::ResolvePlayerByFirstName(const CExoString& sFirstName)
{
    return m_pcExoAppInternal->ResolvePlayerByFirstName(sFirstName);
}
// PC 0x004DC340 GOG 0x004DC2B0 Xbox 0x00171870
int CServerExoApp::GetDifficultyOption(int nOption)
{
    return m_pcExoAppInternal->GetDifficultyOption(nOption);
}
// PC 0x004DC350 GOG 0x004DC2C0 Xbox 0x00171880
void CServerExoApp::AddCharListRequest(unsigned long nPlayerId)
{
    m_pcExoAppInternal->AddCharListRequest(nPlayerId);
}

// Dead code: not in the PC executable
int CServerExoApp::IsOnExclusionList(unsigned long oidObject)
{
    return m_pcExoAppInternal->IsOnExclusionList(oidObject);
}
// PC 0x004DC360 GOG 0x004DC2D0 Xbox 0x00171890
int CServerExoApp::GetFirstPCObject()
{
    return m_pcExoAppInternal->GetFirstPCObject();
}

// Dead code: not in the PC executable
int CServerExoApp::GetNextPCObject()
{
    return m_pcExoAppInternal->GetNextPCObject();
}
// PC 0x004DC370 GOG 0x004DC760 Xbox 0x00171CD0
CSWGlobalVariableTable* CServerExoApp::GetGlobalVariableTable()
{
    return &m_pcExoAppInternal->m_globalVariableTable;
}

// Dead code: not in the PC executable
int CServerExoApp::DebugValidatePath_Point(CSWSArea* pArea, const Vector& vPoint)
{
    return m_pcExoAppInternal->DebugValidatePath_Point(pArea, vPoint);
}

// Dead code: not in the PC executable
int CServerExoApp::DebugValidatePath_Segment(CSWSArea* pArea, CPathfindInformation* pPathfind,
                                             const Vector& vStart, const Vector& vEnd)
{
    return m_pcExoAppInternal->DebugValidatePath_Segment(pArea, pPathfind, vStart, vEnd);
}

// Dead code: not in the PC executable
void CServerExoApp::SetCreatureFollowing(unsigned long oidCreature,
                                         unsigned long oidTarget, int bRun,
                                         const Vector& vDestination)
{
    CSWSCreature* pCreature =
        m_pcExoAppInternal->GetCreatureByGameObjectID(oidCreature);
    if (pCreature != NULL)
    {
        pCreature->ClearAllActions(1);
        if (m_pcExoAppInternal->GetGameObject(oidTarget) != NULL)
        {
            int bStarted = 0;
            Vector vPosition = pCreature->m_vPosition;
            pCreature->AddAction(CSWSObject::ACTION_FORCE_FOLLOW_OBJECT,
                CSWSObject::ACTION_GROUP_NEW,
                CSWSObject::ACTION_PARAM_OBJECT, &oidTarget,
                CSWSObject::ACTION_PARAM_INTEGER, &bRun,
                CSWSObject::ACTION_PARAM_FLOAT, (void*)&vDestination.m_fX,
                CSWSObject::ACTION_PARAM_FLOAT, (void*)&vDestination.m_fY,
                CSWSObject::ACTION_PARAM_INTEGER, &bStarted,
                CSWSObject::ACTION_PARAM_FLOAT, &vPosition.m_fX,
                CSWSObject::ACTION_PARAM_FLOAT, &vPosition.m_fY,
                0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
        }
    }
}
// PC 0x004DC380 GOG 0x004DC2E0 Xbox 0x001718A0
unsigned long CServerExoApp::ClientToServerObjectId(unsigned long oidClient)
{
    return m_pcExoAppInternal->ClientToServerObjectId(oidClient);
}
// PC 0x004DC390 GOG 0x004DC770 Xbox 0x00171CE0
CSWPartyTable* CServerExoApp::GetPartyTable()
{
    return &m_pcExoAppInternal->m_partyTable;
}
// PC 0x004DC3A0 GOG 0x004DC2F0 Xbox 0x001718B0
int CServerExoApp::GetPlayerCreatureId()
{
    return m_pcExoAppInternal->GetPlayerCreatureId();
}
// PC 0x004DC3B0 GOG 0x004DC780 Xbox 0x00171CF0
int CServerExoApp::TestDirectLine(unsigned long oidObject,
                                  const CResRef& cAreaResRef,
                                  const Vector& vStart, const Vector& vEnd,
                                  const float& fDistance, float fWidth,
                                  int nFlags, CWalkHitInfo* pWalkHitInfo)
{
    CSWSArea* pArea;
    if (m_pcExoAppInternal->GetModule() != NULL)
    {
        pArea = m_pcExoAppInternal->GetModule()->GetArea();
    }

    unsigned long oidServer =
        m_pcExoAppInternal->ClientToServerObjectId(oidObject);
    CSWSCreature* pCreature =
        m_pcExoAppInternal->GetCreatureByGameObjectID(oidServer);
    if (pCreature != NULL && pArea != NULL)
    {
        pArea->m_pPathfindInformation = pCreature->m_pPathfindInformation;
        return pArea->TestDirectLine(vStart, vEnd, fDistance, fWidth, nFlags,
                                     pWalkHitInfo);
    }
    if (g_pAppManager->m_pClientExoApp->GetMiniGame() != NULL)
    {
        return pArea->TestDirectLine(vStart, vEnd, fDistance, fWidth, nFlags,
                                     pWalkHitInfo);
    }

    return 0;
}
// PC 0x004DC470 GOG 0x004DC840 Xbox 0x00171DB0
CPathfindInformation* CServerExoApp::GetCreaturePathfindInformation(
    unsigned long oidObject)
{
    unsigned long oidServer =
        m_pcExoAppInternal->ClientToServerObjectId(oidObject);
    CSWSCreature* pCreature =
        m_pcExoAppInternal->GetCreatureByGameObjectID(oidServer);
    if (pCreature != NULL)
    {
        return pCreature->m_pPathfindInformation;
    }

    return NULL;
}
// PC 0x004DC4A0 GOG 0x004DC300 Xbox 0x001718C0
float CServerExoApp::ComputeHeight(const CResRef& cAreaResRef,
                                   const Vector& vPosition, int nRoomIndex)
{
    if (m_pcExoAppInternal->GetModule() != NULL)
    {
        CSWSArea* pArea = m_pcExoAppInternal->GetModule()->GetArea();
        if (pArea != NULL)
        {
            return pArea->ComputeHeight(vPosition, nRoomIndex, NULL);
        }
    }

    return 0.0f;
}
// PC 0x004DC4F0 GOG 0x004DC350 Xbox 0x00171910
void CServerExoApp::UpDatePlayerPosition(unsigned long oidObject,
                                         const Vector& vPosition,
                                         const Vector& vOrientation)
{
    unsigned long oidServer =
        m_pcExoAppInternal->ClientToServerObjectId(oidObject);
    m_pcExoAppInternal->UpDatePlayerPosition(oidServer, vPosition,
                                             vOrientation);
}
// PC 0x004DC520 GOG 0x004DC380 Xbox 0x00171940
void CServerExoApp::UpdateSubareasOnMoveTo(unsigned long oidObject,
                                           const Vector& vFrom,
                                           const Vector& vTo)
{
    m_pcExoAppInternal->UpdateSubareasOnMoveTo(oidObject, vFrom, vTo);
}
// PC 0x004DC530 GOG 0x004DC870 Xbox 0x00171DE0
void CServerExoApp::ClearAllActions(unsigned long oidObject)
{
    CSWSCreature* pCreature =
        m_pcExoAppInternal->GetCreatureByGameObjectID(oidObject);
    if (pCreature != NULL)
    {
        pCreature->ClearAllActions(1);
        pCreature->m_pcCombatRound->RemoveAllActions();
        pCreature->m_pcCombatRound->m_oidRoundTarget = 2130706432;
        pCreature->SetAttemptedAttackTarget(2130706432);
        pCreature->m_oidAttackTarget = 2130706432;
        pCreature->m_oidAttemptedSpellTarget = 2130706432;
        pCreature->m_oidSpellTarget = 2130706432;
    }
}
// PC 0x004DC590 GOG 0x004DC8D0 Xbox 0x00171E40
void CServerExoApp::ClearAllQueuedCombatActions(unsigned long oidObject)
{
    CSWSCreature* pCreature =
        m_pcExoAppInternal->GetCreatureByGameObjectID(oidObject);
    if (pCreature != NULL)
    {
        pCreature->m_pcCombatRound->RemoveAllActions();
    }
}
// PC 0x004DC5B0 GOG 0x004DC390 Xbox 0x00171950
int CServerExoApp::IsActionsPending(unsigned long oidObject)
{
    return m_pcExoAppInternal->IsActionsPending(oidObject);
}

// Dead code: not in the PC executable
class CSWSRoom* CServerExoApp::GetRoom(const CResRef& cAreaResRef, const Vector& vPosition)
{
    return m_pcExoAppInternal->GetModule()->GetArea()->GetRoom(vPosition, NULL);
}
// PC 0x004DC5C0 GOG 0x004DC3A0 Xbox 0x00171960
int CServerExoApp::GetRoomIndex(const CResRef& cAreaResRef,
                                const Vector& vPosition, int nFlags)
{
    int nRoomIndex = -1;
    if (m_pcExoAppInternal->GetModule() != NULL)
    {
        CSWSArea* pArea = m_pcExoAppInternal->GetModule()->GetArea();
        if (pArea != NULL)
        {
            nRoomIndex = pArea->GetRoomIndex(vPosition, nFlags);
        }
    }

    return nRoomIndex;
}
// PC 0x004DC610 GOG 0x004DC3F0 Xbox 0x001719C0
int CServerExoApp::CreatureAcquireItem(unsigned long oidObject,
                                       const CResRef& cResRef)
{
    return m_pcExoAppInternal->CreatureAcquireItem(oidObject, cResRef);
}
// PC 0x004DC620 GOG 0x004DC8F0 Xbox 0x00171E60
CFactionManager* CServerExoApp::GetFactionManager()
{
    return m_pcExoAppInternal->m_pFactionManager;
}
// PC 0x004DC630 GOG 0x004DC900 Xbox 0x00171E70
void CServerExoApp::SetLastConversation(const CExoString& sConversation)
{
    m_pcExoAppInternal->m_sLastConversation = sConversation;
}
#ifndef _XBOX
CExoString CServerExoApp::GetLastConversation()  // PC 0x004DC640 GOG 0x004DC910
{
    return m_pcExoAppInternal->m_sLastConversation;
}
#endif
void CServerExoApp::SetActionConversation(int bActionConversation)  // PC 0x004DC670 GOG 0x004DC940 Xbox 0x00171EA0
{
    m_pcExoAppInternal->m_nActionConversation = bActionConversation;
}
// PC 0x004DC680 GOG 0x004DC950 Xbox 0x00171EB0
int CServerExoApp::GetActionConversation()
{
    return m_pcExoAppInternal->m_nActionConversation;
}
// PC 0x004DC690 GOG 0x004DC960 Xbox 0x00171EC0
void CServerExoApp::SetIgnoreConversationStartRange(int bIgnore)
{
    m_pcExoAppInternal->m_bIgnoreConversationStartRange = bIgnore;
}

// Dead code: not in the PC executable
int CServerExoApp::GetIgnoreConversationStartRange()
{
    return m_pcExoAppInternal->m_bIgnoreConversationStartRange;
}

// Dead code: not in the PC executable
void CServerExoApp::RunDeathSequence()
{
    m_pcExoAppInternal->RunDeathSequence();
}
int CServerExoApp::TestSafeLocation(unsigned long oidObject,  // PC 0x004DC6A0 GOG 0x004DC400 Xbox 0x001719C0
                                    const Vector& vPosition)
{
    return m_pcExoAppInternal->TestSafeLocation(oidObject, vPosition);
}
// PC 0x004DC6B0 GOG 0x004DC410 Xbox 0x001719D0
int CServerExoApp::ComputeSafeLocation(const Vector& vPosition, float fRadius,
                                       unsigned long oidObject, int nFlags,
                                       Vector* pvSafe)
{
    return m_pcExoAppInternal->ComputeSafeLocation(vPosition, fRadius,
                                                   oidObject, nFlags, pvSafe);
}
// PC 0x004DC6C0 GOG 0x004DC970 Xbox 0x00171ED0
int CServerExoApp::GetLoadFromSaveGame()
{
    return m_pcExoAppInternal->m_bLoadFromSaveGame;
}
// PC 0x004DC6D0 GOG 0x004DC980 Xbox 0x00171EE0
void CServerExoApp::SetLoadFromSaveGame(int bLoadFromSaveGame)
{
    m_pcExoAppInternal->m_bLoadFromSaveGame = bLoadFromSaveGame;
}
// PC 0x004DC6E0 GOG 0x004DC990 Xbox 0x00171EF0
int CServerExoApp::GetButtonMashCheck()
{
    return m_pcExoAppInternal->m_bButtonMashCheck;
}
// PC 0x004DC6F0 GOG 0x004DC9A0 Xbox 0x00171F00
void CServerExoApp::SetButtonMashCheck(int bButtonMashCheck)
{
    m_pcExoAppInternal->m_bButtonMashCheck = bButtonMashCheck;
}
// PC 0x004DC700 GOG 0x004DC9B0 Xbox 0x00171F10
void CServerExoApp::SetDoingStartNewModule(int bDoingStartNewModule)
{
    m_pcExoAppInternal->m_bDoingStartNewModule = bDoingStartNewModule;
}
// PC 0x004DC710 GOG 0x004DC420 Xbox 0x001719E0
Vector CServerExoApp::GetNormalOfPoint(const Vector& vPosition)
{
    return m_pcExoAppInternal->GetNormalOfPoint(vPosition);
}
void CServerExoApp::ClearLastModuleFound()  // PC 0x004DC730 GOG 0x004DC440 Xbox 0x00171A00
{
    m_pcExoAppInternal->ClearLastModuleFound();
}
// PC 0x004DC740 GOG 0x004DC450 Xbox 0x00171A10
void CServerExoApp::ClearAreaCache()
{
    m_pcExoAppInternal->ClearAreaCache();
}
// PC 0x004DC750 GOG 0x004DC460 Xbox 0x00171A20
int CServerExoApp::ShrinkLineByCreatures(const Vector& vStart,
                                         const Vector& vEnd, Vector& vResult)
{
    return m_pcExoAppInternal->ShrinkLineByCreatures(vStart, vEnd, vResult);
}
// PC 0x004DC760 GOG 0x004DC9C0 Xbox 0x00171F20
void CServerExoApp::SetMoveToPausedCalendarDay(unsigned long nCalendarDay)
{
    m_pcExoAppInternal->m_nMoveToPausedCalendarDay = nCalendarDay;
}
// PC 0x004DC770 GOG 0x004DC9D0 Xbox 0x00171F30
unsigned long CServerExoApp::GetMoveToPausedCalendarDay()
{
    return m_pcExoAppInternal->m_nMoveToPausedCalendarDay;
}
// PC 0x004DC780 GOG 0x004DC9E0 Xbox 0x00171F40
void CServerExoApp::SetMoveToPausedTimeOfDay(unsigned long nTimeOfDay)
{
    m_pcExoAppInternal->m_nMoveToPausedTimeOfDay = nTimeOfDay;
}
// PC 0x004DC790 GOG 0x004DC9F0 Xbox 0x00171F50
unsigned long CServerExoApp::GetMoveToPausedTimeOfDay()
{
    return m_pcExoAppInternal->m_nMoveToPausedTimeOfDay;
}
// PC 0x004DC7A0 GOG 0x004DC470 Xbox 0x00171A30
void CServerExoApp::SetMoveToWorldTime(
    unsigned long nCalendarDay, unsigned char nHour, unsigned char nMinute,
    unsigned char nSecond, unsigned short nMillisecond, unsigned short nYear,
    unsigned short nMonth)
{
    m_pcExoAppInternal->SetMoveToWorldTime(
        nCalendarDay, nHour, nMinute, nSecond, nMillisecond, nYear, nMonth);
}
// PC 0x004DC7B0 GOG 0x004DC480 Xbox 0x00171A40
void CServerExoApp::GetMoveToWorldTime(
    unsigned long& nCalendarDay, unsigned char& nHour, unsigned char& nMinute,
    unsigned char& nSecond, unsigned short& nMillisecond, unsigned short& nYear,
    unsigned short& nMonth)
{
    m_pcExoAppInternal->GetMoveToWorldTime(
        nCalendarDay, nHour, nMinute, nSecond, nMillisecond, nYear, nMonth);
}

// Dead code: not in the PC executable
unsigned long CServerExoApp::GetAutoSaveTimer()
{
    return m_pcExoAppInternal->m_nAutoSaveElapsedTime;
}

// Dead code: not in the PC executable
void CServerExoApp::SetAutoSaveTimer(unsigned long nTimer, int bReset)
{
    m_pcExoAppInternal->m_nAutoSaveElapsedTime = nTimer;
    m_pcExoAppInternal->m_bAutoSaveTimerExpired = bReset;
}
// PC 0x004DC7C0 GOG 0x004DC490 Xbox 0x00171A50
int CServerExoApp::GetModulePreviouslyVisited(const CExoString& sModule)
{
    return m_pcExoAppInternal->GetModulePreviouslyVisited(sModule);
}

// Dead code: not in the PC executable
int CServerExoApp::HasEnoughDiskSpaceForSaveGame()  // Xbox 0x00171A60
{
    return m_pcExoAppInternal->HasEnoughDiskSpaceForSaveGame();
}
// PC 0x004DC7D0 GOG 0x004DC4A0
int CServerExoApp::ExecuteLoadModule(const CExoString& sModule)
{
    return m_pcExoAppInternal->ExecuteLoadModule(sModule);
}

inline void CServerExoApp::DestroyExoAppInternal()
{
    if (m_pcExoAppInternal != NULL)
    {
        delete m_pcExoAppInternal;
        m_pcExoAppInternal = NULL;
    }
}
// PC 0x004015A0 GOG 0x004015A0 Xbox 0x001A2B40
inline void CServerExoApp::ShutDownToMainMenu()
{
}
// PC 0x004DC7E0 GOG 0x004DCA00
void CServerExoApp::SetSaveGameName(const CExoString& sName)
{
#ifndef _XBOX
    m_pcExoAppInternal->m_sSaveGameName = sName;
#endif
}
// PC 0x004DC7F0 GOG 0x004DC4B0
unsigned __int64 CServerExoApp::GetSystemTimeAsDWORD64()
{
    return m_pcExoAppInternal->GetSystemTimeAsDWORD64();
}
// PC 0x0062B870 GOG 0x00638C70 Xbox 0x00171760
void CServerExoApp::Uninitialize()
{
    m_pcExoAppInternal->Uninitialize();
}
// PC 0x00638BE0 GOG 0x00638AD0 Xbox 0x00171CC0
int CServerExoApp::GetApplicationId()
{
    return g_pAppManager->m_nAppId;
}
// PC 0x004DC800 GOG 0x004DC4C0 Xbox 0x00171A70
CServerExoApp::CServerExoApp()
{
    m_pcExoAppInternal = new CServerExoAppInternal;
}
// PC 0x004DC870 GOG 0x004DC530 Xbox 0x00171AC0
CServerExoApp::~CServerExoApp()
{
    if (m_pcExoAppInternal != NULL)
    {
        delete m_pcExoAppInternal;
    }
}
// PC 0x004DC890 GOG 0x004DC560 Xbox 0x00171AF0
CExoLocString* CServerExoApp::GetModuleDescription()
{
    if (m_pcExoAppInternal->GetModule() != NULL)
    {
        return &m_pcExoAppInternal->GetModule()->m_sDescription;
    }

    return NULL;
}
// PC 0x004DC8B0 GOG 0x004DC740 Xbox 0x00171CA0
int CServerExoApp::IsInjured(unsigned long oidObject)
{
    unsigned long oidServer =
        m_pcExoAppInternal->ClientToServerObjectId(oidObject);
    return m_pcExoAppInternal->IsInjured(oidServer);
}

#ifdef _XBOX

CExoString CServerExoAppInternal::GetLastConversation()
{
    return m_sLastConversation;
}

CExoString CServerExoApp::GetLastConversation()  // Xbox 0x00171E80
{
    return m_pcExoAppInternal->GetLastConversation();
}

int CServerExoApp::DebugValidatePath(unsigned long oidCreature, int bTempWayPoints)
{
    return m_pcExoAppInternal->DebugValidatePath(oidCreature, bTempWayPoints);
}

void CServerExoApp::SetHintTimers(unsigned long nHintTimer1, unsigned long nHintTimer2)  // Xbox 0x00171F60
{
    CServerExoAppInternal* pInternal = m_pcExoAppInternal;
    pInternal->m_nAutoSaveElapsedTime = nHintTimer1;
    pInternal->m_bAutoSaveTimerExpired = nHintTimer2;
}
#endif
