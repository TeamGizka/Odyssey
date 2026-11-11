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
// PC 0x004DBE50
CSWSModule* CServerExoApp::GetModule()
{
    return m_pcExoAppInternal->GetModule();
}
// PC 0x004DBE60
CExoString CServerExoApp::GetModuleName()
{
    return m_pcExoAppInternal->GetModuleName();
}
// PC 0x004DBE80
int CServerExoApp::SaveGame(unsigned long nSlot, const CExoString& sName,
                            CSWSPlayer* pPlayer, int bAutoSave)
{
    return m_pcExoAppInternal->SaveGame(nSlot, sName, pPlayer, bAutoSave);
}

int CServerExoApp::SendCharacterQuery(CSWSPlayer* pPlayer)
{
    return m_pcExoAppInternal->SendCharacterQuery(pPlayer);
}
// PC 0x004DBE90
int CServerExoApp::LoadGame(unsigned long nSlot, const CExoString& sName,
                            const CExoString& sModule, CSWSPlayer* pPlayer)
{
    return m_pcExoAppInternal->LoadGame(nSlot, sName, sModule, pPlayer);
}

int CServerExoApp::StartNewModule(const CExoString& sModule)
{
    return m_pcExoAppInternal->StartNewModule(sModule);
}
// PC 0x004DBF60
int CServerExoApp::LoadCharacterStart(unsigned char nType, CSWSPlayer* pPlayer,
                                      CResRef cResRef, void* pData,
                                      unsigned long nSize)
{
    return m_pcExoAppInternal->LoadCharacterStart(nType, pPlayer, cResRef,
                                                  pData, nSize);
}
// PC 0x004DBFA0
int CServerExoApp::LoadCharacterFinish(CSWSPlayer* pPlayer, int bSuccess)
{
    return m_pcExoAppInternal->LoadCharacterFinish(pPlayer, bSuccess);
}

int CServerExoApp::LoadPrimaryPlayer(CSWSPlayer* pPlayer)
{
    return m_pcExoAppInternal->LoadPrimaryPlayer(pPlayer);
}
// PC 0x004DBFC0
CGameObject* CServerExoApp::GetGameObject(unsigned long oidObject)
{
    return m_pcExoAppInternal->GetGameObject(oidObject);
}
// PC 0x004DBFF0
CSWSItem* CServerExoApp::GetItemByGameObjectID(unsigned long oidObject)
{
    return m_pcExoAppInternal->GetItemByGameObjectID(oidObject);
}
// PC 0x004DC000
CSWSCreature* CServerExoApp::GetCreatureByGameObjectID(unsigned long oidObject)
{
    return m_pcExoAppInternal->GetCreatureByGameObjectID(oidObject);
}
// PC 0x004DC010
CSWSArea* CServerExoApp::GetAreaByGameObjectID(unsigned long oidObject)
{
    return m_pcExoAppInternal->GetAreaByGameObjectID(oidObject);
}
// PC 0x004DC020
CSWSTrigger* CServerExoApp::GetTriggerByGameObjectID(unsigned long oidObject)
{
    return m_pcExoAppInternal->GetTriggerByGameObjectID(oidObject);
}
// PC 0x004DC030
CSWSPlaceable* CServerExoApp::GetPlaceableByGameObjectID(
    unsigned long oidObject)
{
    return m_pcExoAppInternal->GetPlaceableByGameObjectID(oidObject);
}
// PC 0x004DC040
CSWSStore* CServerExoApp::GetStoreByGameObjectID(unsigned long oidObject)
{
    return m_pcExoAppInternal->GetStoreByGameObjectID(oidObject);
}
// PC 0x004DC050
CSWSDoor* CServerExoApp::GetDoorByGameObjectID(unsigned long oidObject)
{
    return m_pcExoAppInternal->GetDoorByGameObjectID(oidObject);
}
// PC 0x004DC060
CSWSAreaOfEffectObject* CServerExoApp::GetAreaOfEffectByGameObjectID(
    unsigned long oidObject)
{
    return m_pcExoAppInternal->GetAreaOfEffectByGameObjectID(oidObject);
}
// PC 0x004DC070
CSWSWaypoint* CServerExoApp::GetWaypointByGameObjectID(unsigned long oidObject)
{
    return m_pcExoAppInternal->GetWaypointByGameObjectID(oidObject);
}
// PC 0x004DC080
CSWSEncounter* CServerExoApp::GetEncounterByGameObjectID(
    unsigned long oidObject)
{
    return m_pcExoAppInternal->GetEncounterByGameObjectID(oidObject);
}
// PC 0x004DC090
CSWSSoundObject* CServerExoApp::GetSoundObjectByGameObjectID(
    unsigned long oidObject)
{
    return m_pcExoAppInternal->GetSoundObjectByGameObjectID(oidObject);
}
// PC 0x004DC0A0
int CServerExoApp::GetPlayerLanguage(unsigned long nPlayerId)
{
    return m_pcExoAppInternal->GetPlayerLanguage(nPlayerId);
}
// PC 0x004DC0B0
int CServerExoApp::GetModuleLanguage()
{
    return m_pcExoAppInternal->GetModuleLanguage();
}
// PC 0x004DC120
CWorldTimer* CServerExoApp::GetActiveTimer(unsigned long nTimer)
{
    return m_pcExoAppInternal->GetActiveTimer(nTimer);
}
// PC 0x004DC180
int CServerExoApp::HandleMessage(unsigned long nPlayerId,
                                 unsigned char* pMessage, unsigned long nSize,
                                 int bGuaranteed)
{
    return m_pcExoAppInternal->HandleMessage(nPlayerId, pMessage, nSize,
                                             bGuaranteed);
}
// PC 0x004DC190
void CServerExoApp::Initialize()
{
    m_pcExoAppInternal->Initialize();
}
// PC 0x004DC1A0
int CServerExoApp::MainLoop()
{
    return m_pcExoAppInternal->MainLoop();
}
// PC 0x004DC1B0
void CServerExoApp::PlayerListChange(unsigned long nPlayerId, int bAdded,
                                     int bGuaranteed)
{
    m_pcExoAppInternal->PlayerListChange(nPlayerId, bAdded, bGuaranteed);
}
// PC 0x004DC1C0
int CServerExoApp::ContinueMessageProcessing()
{
    return m_pcExoAppInternal->ContinueMessageProcessing();
}
// PC 0x004DC1D0
void CServerExoApp::Shutdown(int bExit, int bSave)
{
    m_pcExoAppInternal->Shutdown(bExit, bSave);
}
// PC 0x004DC1E0
int CServerExoApp::UnloadModule()
{
    return m_pcExoAppInternal->UnloadModule();
}
// PC 0x004DC1F0
int CServerExoApp::LoadModule(const CExoString& sModule, CSWSPlayer* pPlayer)
{
    return m_pcExoAppInternal->LoadModule(sModule, pPlayer);
}
// PC 0x004DC200
int CServerExoApp::RunModule()
{
    return m_pcExoAppInternal->RunModule();
}
// PC 0x004DC210
CExoString CServerExoApp::GetPlayerListString()
{
    return m_pcExoAppInternal->GetPlayerListString();
}

void CServerExoApp::OnCDChange()
{
    m_pcExoAppInternal->OnCDChange();
}

void CServerExoApp::OnExit()
{
    m_pcExoAppInternal->OnExit();
}

void CServerExoApp::OnGainFocus()
{
    m_pcExoAppInternal->OnGainFocus();
}

void CServerExoApp::OnLostFocus()
{
    m_pcExoAppInternal->OnLostFocus();
}

void CServerExoApp::OnVideoChange()
{
    m_pcExoAppInternal->OnVideoChange();
}
// PC 0x004DC240
void CServerExoApp::StartServices(int nServices)
{
    m_pcExoAppInternal->StartServices(nServices);
}
// PC 0x004DC250
void CServerExoApp::StopServices()
{
    m_pcExoAppInternal->StopServices();
}
// PC 0x004DC260
CSWSPlayer* CServerExoApp::GetClientObjectByObjectId(unsigned long oidObject)
{
    return m_pcExoAppInternal->GetClientObjectByObjectId(oidObject);
}
// PC 0x004DC270
CSWSPlayer* CServerExoApp::GetClientObjectByPlayerId(unsigned long nPlayerId,
                                                     unsigned char nGroup)
{
    return m_pcExoAppInternal->GetClientObjectByPlayerId(nPlayerId, nGroup);
}
// PC 0x004DC280
void CServerExoApp::InitiateModuleForPlayer(void* pData)
{
    m_pcExoAppInternal->InitiateModuleForPlayer(pData);
}
// PC 0x004DC290
void CServerExoApp::MovePlayerToArea(void* pData)
{
    m_pcExoAppInternal->MovePlayerToArea(pData);
}
// PC 0x004DC2A0
int CServerExoApp::GetFactionOfObject(unsigned long oidObject, int* pnFaction)
{
    return m_pcExoAppInternal->GetFactionOfObject(oidObject, pnFaction);
}
// PC 0x004DC2B0
void CServerExoApp::TogglePauseState(unsigned char nPauseType)
{
    m_pcExoAppInternal->TogglePauseState(nPauseType);
}
// PC 0x004DC2C0
void CServerExoApp::SkipPastDialog(unsigned long oidObject)
{
    m_pcExoAppInternal->SkipPastDialog(oidObject);
}
// PC 0x004DC2D0
int CServerExoApp::GetPauseState(unsigned char nPauseType)
{
    return m_pcExoAppInternal->GetPauseState(nPauseType);
}
// PC 0x004DC2E0
unsigned char CServerExoApp::GetActivePauseState()
{
    return m_pcExoAppInternal->GetActivePauseState();
}
// PC 0x004DC2F0
void CServerExoApp::SetPauseState(unsigned char nPauseType, int bPause)
{
    m_pcExoAppInternal->SetPauseState(nPauseType, bPause);
}
// PC 0x004DC300
void CServerExoApp::AddToExclusionList(unsigned long oidObject,
                                      unsigned char nType)
{
    m_pcExoAppInternal->AddToExclusionList(oidObject, nType);
}
// PC 0x004DC310
void CServerExoApp::RemoveFromExclusionList(unsigned long oidObject,
                                           unsigned char nType)
{
    m_pcExoAppInternal->RemoveFromExclusionList(oidObject, nType);
}
// PC 0x004DC320
CExoLinkedList<unsigned long>* CServerExoApp::GetActiveExclusionList()
{
    return m_pcExoAppInternal->GetActiveExclusionList();
}

int CServerExoApp::IsOnActiveExclusionList(unsigned long oidObject)
{
    return m_pcExoAppInternal->IsOnActiveExclusionList(oidObject);
}
// PC 0x004DC330
int CServerExoApp::ResolvePlayerByFirstName(const CExoString& sFirstName)
{
    return m_pcExoAppInternal->ResolvePlayerByFirstName(sFirstName);
}
// PC 0x004DC340
int CServerExoApp::GetDifficultyOption(int nOption)
{
    return m_pcExoAppInternal->GetDifficultyOption(nOption);
}
// PC 0x004DC350
void CServerExoApp::AddCharListRequest(unsigned long nPlayerId)
{
    m_pcExoAppInternal->AddCharListRequest(nPlayerId);
}
// PC 0x004DC360
int CServerExoApp::GetFirstPCObject()
{
    return m_pcExoAppInternal->GetFirstPCObject();
}

int CServerExoApp::GetNextPCObject()
{
    return m_pcExoAppInternal->GetNextPCObject();
}
// PC 0x004DC380
unsigned long CServerExoApp::ClientToServerObjectId(unsigned long oidClient)
{
    return m_pcExoAppInternal->ClientToServerObjectId(oidClient);
}
// PC 0x004DC3A0
int CServerExoApp::GetPlayerCreatureId()
{
    return m_pcExoAppInternal->GetPlayerCreatureId();
}
// PC 0x004DC4A0
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
// PC 0x004DC4F0
void CServerExoApp::UpDatePlayerPosition(unsigned long oidObject,
                                         const Vector& vPosition,
                                         const Vector& vOrientation)
{
    unsigned long oidServer =
        m_pcExoAppInternal->ClientToServerObjectId(oidObject);
    m_pcExoAppInternal->UpDatePlayerPosition(oidServer, vPosition,
                                             vOrientation);
}
// PC 0x004DC520
void CServerExoApp::UpdateSubareasOnMoveTo(unsigned long oidObject,
                                           const Vector& vFrom,
                                           const Vector& vTo)
{
    m_pcExoAppInternal->UpdateSubareasOnMoveTo(oidObject, vFrom, vTo);
}
// PC 0x004DC5B0
int CServerExoApp::IsActionsPending(unsigned long oidObject)
{
    return m_pcExoAppInternal->IsActionsPending(oidObject);
}
// PC 0x004DC5C0
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
// PC 0x004DC610
int CServerExoApp::CreatureAcquireItem(unsigned long oidObject,
                                       const CResRef& cResRef)
{
    return m_pcExoAppInternal->CreatureAcquireItem(oidObject, cResRef);
}
#ifndef _XBOX
int CServerExoApp::TestSafeLocation(unsigned long oidObject,  // PC 0x004DC6A0
                                    const Vector& vPosition)
{
    return m_pcExoAppInternal->TestSafeLocation(oidObject, vPosition);
}
// PC 0x004DC6B0
int CServerExoApp::ComputeSafeLocation(const Vector& vPosition, float fRadius,
                                       unsigned long oidObject, int nFlags,
                                       Vector* pvSafe)
{
    return m_pcExoAppInternal->ComputeSafeLocation(vPosition, fRadius,
                                                   oidObject, nFlags, pvSafe);
}
// PC 0x004DC710
Vector CServerExoApp::GetNormalOfPoint(const Vector& vPosition)
{
    return m_pcExoAppInternal->GetNormalOfPoint(vPosition);
}
#endif
void CServerExoApp::ClearLastModuleFound()  // PC 0x004DC730
{
    m_pcExoAppInternal->ClearLastModuleFound();
}
// PC 0x004DC740
void CServerExoApp::ClearAreaCache()
{
    m_pcExoAppInternal->ClearAreaCache();
}
// PC 0x004DC750
int CServerExoApp::ShrinkLineByCreatures(const Vector& vStart,
                                         const Vector& vEnd, Vector& vResult)
{
    return m_pcExoAppInternal->ShrinkLineByCreatures(vStart, vEnd, vResult);
}
// PC 0x004DC7A0
void CServerExoApp::SetMoveToWorldTime(
    unsigned long nCalendarDay, unsigned char nHour, unsigned char nMinute,
    unsigned char nSecond, unsigned short nMillisecond, unsigned short nYear,
    unsigned short nMonth)
{
    m_pcExoAppInternal->SetMoveToWorldTime(
        nCalendarDay, nHour, nMinute, nSecond, nMillisecond, nYear, nMonth);
}
// PC 0x004DC7B0
void CServerExoApp::GetMoveToWorldTime(
    unsigned long& nCalendarDay, unsigned char& nHour, unsigned char& nMinute,
    unsigned char& nSecond, unsigned short& nMillisecond, unsigned short& nYear,
    unsigned short& nMonth)
{
    m_pcExoAppInternal->GetMoveToWorldTime(
        nCalendarDay, nHour, nMinute, nSecond, nMillisecond, nYear, nMonth);
}
// PC 0x004DC7C0
int CServerExoApp::GetModulePreviouslyVisited(const CExoString& sModule)
{
    return m_pcExoAppInternal->GetModulePreviouslyVisited(sModule);
}
// PC 0x004DC7D0
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
// PC 0x004015A0
inline void CServerExoApp::ShutDownToMainMenu()
{
}
// PC 0x004DC800
CServerExoApp::CServerExoApp()
{
    m_pcExoAppInternal = new CServerExoAppInternal;
}
// PC 0x004DC870
CServerExoApp::~CServerExoApp()
{
    if (m_pcExoAppInternal != NULL)
    {
        delete m_pcExoAppInternal;
    }
}
// PC 0x004DBE40
CServerInfo* CServerExoApp::GetServerInfo()
{
    return m_pcExoAppInternal->m_pServerInfo;
}
// PC 0x004DC890
CExoLocString* CServerExoApp::GetModuleDescription()
{
    if (m_pcExoAppInternal->GetModule() != NULL)
    {
        return &m_pcExoAppInternal->GetModule()->m_sDescription;
    }

    return NULL;
}
// PC 0x004DBEA0
void CServerExoApp::SetMoveToModulePending(int bPending)
{
    m_pcExoAppInternal->m_bMoveToModulePending = bPending;
}

int CServerExoApp::GetMoveToModulePending()
{
    return m_pcExoAppInternal->m_bMoveToModulePending;
}
// PC 0x004DBEC0
void CServerExoApp::SetMoveToModuleString(const CExoString& sModule)
{
    m_pcExoAppInternal->m_sMoveToModule = sModule;
}

CExoString CServerExoApp::GetMoveToModuleString()
{
    return m_pcExoAppInternal->m_sMoveToModule;
}
// PC 0x004DBED0
void CServerExoApp::SetExportAllCharactersPending(int bPending)
{
    m_pcExoAppInternal->m_bExportAllCharactersPending = bPending;
}

int CServerExoApp::ExportAllPlayers()
{
    return m_pcExoAppInternal->ExportAllPlayers();
}
// PC 0x004DBEE0
int CServerExoApp::GetLoadingModule()
{
    return m_pcExoAppInternal->m_bLoadingModule;
}
// PC 0x004DBEF0
void CServerExoApp::SetLoadingModule(int bLoading)
{
    m_pcExoAppInternal->m_bLoadingModule = bLoading;
}
// PC 0x004DBF00
void CServerExoApp::SetAutoSavePending(int bPending)
{
    m_pcExoAppInternal->m_bAutoSavePending = bPending;
}
// PC 0x004DBF10
int CServerExoApp::GetAutoSavePending()
{
    return m_pcExoAppInternal->m_bAutoSavePending;
}
// PC 0x004DBF20
void CServerExoApp::SetMoveToModuleStartWaypoint(const CExoString& sWaypoint)
{
    m_pcExoAppInternal->m_sMoveToModuleStartWaypoint = sWaypoint;
}
// PC 0x004DBF30
CExoString CServerExoApp::GetMoveToModuleStartWaypoint()
{
    return m_pcExoAppInternal->m_sMoveToModuleStartWaypoint;
}
// PC 0x004DBFB0
CGameObjectArray* CServerExoApp::GetObjectArray()
{
    return m_pcExoAppInternal->m_pObjectArray;
}
// PC 0x004DC0C0
CServerAIMaster* CServerExoApp::GetServerAIMaster()
{
    return m_pcExoAppInternal->m_pServerAIMaster;
}
// PC 0x004DC0D0
CExoLinkedList<CSWSClient>* CServerExoApp::GetPlayerList()
{
    return m_pcExoAppInternal->m_pPlayerList;
}
// PC 0x004DC0E0
CNetLayer* CServerExoApp::GetNetLayer()
{
    return m_pcExoAppInternal->m_pNetLayer;
}
// PC 0x004DC0F0
CSWSMessage* CServerExoApp::GetSWSMessage()
{
    return m_pcExoAppInternal->m_pSWSMessage;
}
// PC 0x004DC100
CExoLinkedList<CSWSClient>* CServerExoApp::GetSysAdminList()
{
    return m_pcExoAppInternal->m_pSysAdminList;
}
// PC 0x004DC110
short CServerExoApp::GetServerMode()
{
    return m_pcExoAppInternal->m_nServerMode;
}
// PC 0x004DC130
CWorldTimer* CServerExoApp::GetWorldTimer()
{
    return m_pcExoAppInternal->m_pWorldTimer;
}
// PC 0x004DC140
CWorldTimer* CServerExoApp::GetTimestopTimer()
{
    return m_pcExoAppInternal->m_pTimestopTimer;
}
// PC 0x004DC150
CWorldTimer* CServerExoApp::GetPauseTimer()
{
    return m_pcExoAppInternal->m_pPauseTimer;
}
// PC 0x004DC160
void CServerExoApp::SetDebugMode(int nDebugMode)
{
    m_pcExoAppInternal->m_nDebugMode = nDebugMode;
}
// PC 0x004DC170
int CServerExoApp::GetDebugMode()
{
    return m_pcExoAppInternal->m_nDebugMode;
}
// PC 0x004DC230
CConnectionLib* CServerExoApp::GetConnectionLib()
{
    return m_pcExoAppInternal->m_pConnectionLib;
}
// PC 0x004DC8B0
int CServerExoApp::IsInjured(unsigned long oidObject)
{
    unsigned long oidServer =
        m_pcExoAppInternal->ClientToServerObjectId(oidObject);
    return m_pcExoAppInternal->IsInjured(oidServer);
}
// PC 0x004DC370
CSWGlobalVariableTable* CServerExoApp::GetGlobalVariableTable()
{
    return &m_pcExoAppInternal->m_globalVariableTable;
}

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
// PC 0x004DC390
CSWPartyTable* CServerExoApp::GetPartyTable()
{
    return &m_pcExoAppInternal->m_partyTable;
}
// PC 0x004DC3B0
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

    return 0;
}
// PC 0x004DC470
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
// PC 0x004DC530
void CServerExoApp::ClearAllActions(unsigned long oidObject)
{
    CSWSCreature* pCreature =
        m_pcExoAppInternal->GetCreatureByGameObjectID(oidObject);
    if (pCreature != NULL)
    {
        pCreature->ClearAllActions(1);
        pCreature->m_pcCombatRound->RemoveAllActions();
        pCreature->m_pcCombatRound->m_oidUnknown9CC = 2130706432;
        pCreature->m_oidAttemptedAttackTarget = 2130706432;
        pCreature->m_oidAttackTarget = 2130706432;
        pCreature->m_oidAttemptedSpellTarget = 2130706432;
        pCreature->m_oidSpellTarget = 2130706432;
    }
}
// PC 0x004DC590
void CServerExoApp::ClearAllQueuedCombatActions(unsigned long oidObject)
{
    CSWSCreature* pCreature =
        m_pcExoAppInternal->GetCreatureByGameObjectID(oidObject);
    if (pCreature != NULL)
    {
        pCreature->m_pcCombatRound->RemoveAllActions();
    }
}
// PC 0x004DC620
CFactionManager* CServerExoApp::GetFactionManager()
{
    return m_pcExoAppInternal->m_pFactionManager;
}
// PC 0x004DC630
void CServerExoApp::SetLastConversation(const CExoString& sConversation)
{
    m_pcExoAppInternal->m_sLastConversation = sConversation;
}
#ifndef _XBOX
CExoString CServerExoApp::GetLastConversation()  // PC 0x004DC640
{
    return m_pcExoAppInternal->m_sLastConversation;
}
#endif
void CServerExoApp::SetActionConversation(int bActionConversation)  // PC 0x004DC670
{
    m_pcExoAppInternal->m_nActionConversation = bActionConversation;
}
// PC 0x004DC680
int CServerExoApp::GetActionConversation()
{
    return m_pcExoAppInternal->m_nActionConversation;
}
// PC 0x004DC690
void CServerExoApp::SetIgnoreConversationStartRange(int bIgnore)
{
    m_pcExoAppInternal->m_bIgnoreConversationStartRange = bIgnore;
}

int CServerExoApp::GetIgnoreConversationStartRange()
{
    return m_pcExoAppInternal->m_bIgnoreConversationStartRange;
}
// PC 0x004DC6C0
int CServerExoApp::GetLoadFromSaveGame()
{
    return m_pcExoAppInternal->m_bLoadFromSaveGame;
}
// PC 0x004DC6D0
void CServerExoApp::SetLoadFromSaveGame(int bLoadFromSaveGame)
{
    m_pcExoAppInternal->m_bLoadFromSaveGame = bLoadFromSaveGame;
}
// PC 0x004DC6E0
int CServerExoApp::GetButtonMashCheck()
{
    return m_pcExoAppInternal->m_bButtonMashCheck;
}
// PC 0x004DC6F0
void CServerExoApp::SetButtonMashCheck(int bButtonMashCheck)
{
    m_pcExoAppInternal->m_bButtonMashCheck = bButtonMashCheck;
}
// PC 0x004DC700
void CServerExoApp::SetDoingStartNewModule(int bDoingStartNewModule)
{
    m_pcExoAppInternal->m_bDoingStartNewModule = bDoingStartNewModule;
}
// PC 0x004DC760
void CServerExoApp::SetMoveToPausedCalendarDay(unsigned long nCalendarDay)
{
    m_pcExoAppInternal->m_nMoveToPausedCalendarDay = nCalendarDay;
}
// PC 0x004DC770
unsigned long CServerExoApp::GetMoveToPausedCalendarDay()
{
    return m_pcExoAppInternal->m_nMoveToPausedCalendarDay;
}
// PC 0x004DC780
void CServerExoApp::SetMoveToPausedTimeOfDay(unsigned long nTimeOfDay)
{
    m_pcExoAppInternal->m_nMoveToPausedTimeOfDay = nTimeOfDay;
}
// PC 0x004DC790
unsigned long CServerExoApp::GetMoveToPausedTimeOfDay()
{
    return m_pcExoAppInternal->m_nMoveToPausedTimeOfDay;
}
// PC 0x004DC7E0
void CServerExoApp::SetSaveGameName(const CExoString& sName)
{
#ifndef _XBOX
    m_pcExoAppInternal->m_sSaveGameName = sName;
#endif
}
// PC 0x0062B870
void CServerExoApp::Uninitialize()
{
    m_pcExoAppInternal->Uninitialize();
}
// PC 0x00638BE0
int CServerExoApp::GetApplicationId()
{
    return g_pAppManager->m_nAppId;
}

#ifdef _XBOX

CExoString CServerExoAppInternal::GetLastConversation()
{
    return m_sLastConversation;
}

CExoString CServerExoApp::GetLastConversation()
{
    return m_pcExoAppInternal->GetLastConversation();
}

int CServerExoApp::TestSafeLocation(unsigned long oidObject, Vector vPosition)
{
    return m_pcExoAppInternal->TestSafeLocation(oidObject, vPosition);
}

int CServerExoApp::ComputeSafeLocation(Vector vPosition, float fRadius,
                                       unsigned long oidObject, int nFlags,
                                       Vector* pvSafe)
{
    return m_pcExoAppInternal->ComputeSafeLocation(vPosition, fRadius,
                                                   oidObject, nFlags, pvSafe);
}

Vector CServerExoApp::GetNormalOfPoint(Vector vPosition)
{
    return m_pcExoAppInternal->GetNormalOfPoint(vPosition);
}

int CServerExoApp::DebugValidatePath(unsigned long oidCreature, int bTempWayPoints)
{
    return m_pcExoAppInternal->DebugValidatePath(oidCreature, bTempWayPoints);
}

void CServerExoApp::SetHintTimers(unsigned long nHintTimer1, unsigned long nHintTimer2)
{
    CServerExoAppInternal* pInternal = m_pcExoAppInternal;
    pInternal->m_nAutoSaveElapsedTime = nHintTimer1;
    pInternal->m_bAutoSaveTimerExpired = nHintTimer2;
}
#endif
