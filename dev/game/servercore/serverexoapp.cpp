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
// PC 0x004AE6B0  Xbox 0x0015E4F0
CSWSModule* CServerExoApp::GetModule()
{
    return m_pcExoAppInternal->GetModule();
}
// PC 0x004AE6C0  Xbox 0x0015E500
CExoString CServerExoApp::GetModuleName()
{
    return m_pcExoAppInternal->GetModuleName();
}
// PC 0x004AE6E0  Xbox 0x0015E520
int CServerExoApp::SaveGame(unsigned long nSlot, const CExoString& sName,
                            CSWSPlayer* pPlayer, int bAutoSave)
{
    return m_pcExoAppInternal->SaveGame(nSlot, sName, pPlayer, bAutoSave);
}

int CServerExoApp::SendCharacterQuery(CSWSPlayer* pPlayer)
{
    return m_pcExoAppInternal->SendCharacterQuery(pPlayer);
}
// PC 0x004AE6F0  Xbox 0x0015E530
int CServerExoApp::LoadGame(unsigned long nSlot, const CExoString& sName,
                            const CExoString& sModule, CSWSPlayer* pPlayer)
{
    return m_pcExoAppInternal->LoadGame(nSlot, sName, sModule, pPlayer);
}

int CServerExoApp::StartNewModule(const CExoString& sModule)
{
    return m_pcExoAppInternal->StartNewModule(sModule);
}
// PC 0x004AE700  Xbox 0x0015E540
int CServerExoApp::LoadCharacterStart(unsigned char nType, CSWSPlayer* pPlayer,
                                      CResRef cResRef, void* pData,
                                      unsigned long nSize)
{
    return m_pcExoAppInternal->LoadCharacterStart(nType, pPlayer, cResRef,
                                                  pData, nSize);
}
// PC 0x004AE740  Xbox 0x0015E580
int CServerExoApp::LoadCharacterFinish(CSWSPlayer* pPlayer, int bSuccess)
{
    return m_pcExoAppInternal->LoadCharacterFinish(pPlayer, bSuccess);
}

int CServerExoApp::LoadPrimaryPlayer(CSWSPlayer* pPlayer)
{
    return m_pcExoAppInternal->LoadPrimaryPlayer(pPlayer);
}
// PC 0x004AE750  Xbox 0x0015E590
CGameObject* CServerExoApp::GetGameObject(unsigned long oidObject)
{
    return m_pcExoAppInternal->GetGameObject(oidObject);
}
// PC 0x004AE760  Xbox 0x0015E5A0
CSWSItem* CServerExoApp::GetItemByGameObjectID(unsigned long oidObject)
{
    return m_pcExoAppInternal->GetItemByGameObjectID(oidObject);
}
// PC 0x004AE770  Xbox 0x0015E5B0
CSWSCreature* CServerExoApp::GetCreatureByGameObjectID(unsigned long oidObject)
{
    return m_pcExoAppInternal->GetCreatureByGameObjectID(oidObject);
}
// PC 0x004AE780  Xbox 0x0015E5C0
CSWSArea* CServerExoApp::GetAreaByGameObjectID(unsigned long oidObject)
{
    return m_pcExoAppInternal->GetAreaByGameObjectID(oidObject);
}
// PC 0x004AE790  Xbox 0x0015E5D0
CSWSTrigger* CServerExoApp::GetTriggerByGameObjectID(unsigned long oidObject)
{
    return m_pcExoAppInternal->GetTriggerByGameObjectID(oidObject);
}
// PC 0x004AE7A0  Xbox 0x0015E5E0
CSWSPlaceable* CServerExoApp::GetPlaceableByGameObjectID(
    unsigned long oidObject)
{
    return m_pcExoAppInternal->GetPlaceableByGameObjectID(oidObject);
}
// PC 0x004AE7B0  Xbox 0x0015E5F0
CSWSStore* CServerExoApp::GetStoreByGameObjectID(unsigned long oidObject)
{
    return m_pcExoAppInternal->GetStoreByGameObjectID(oidObject);
}
// PC 0x004AE7C0  Xbox 0x0015E600
CSWSDoor* CServerExoApp::GetDoorByGameObjectID(unsigned long oidObject)
{
    return m_pcExoAppInternal->GetDoorByGameObjectID(oidObject);
}
// PC 0x004AE7D0  Xbox 0x0015E610
CSWSAreaOfEffectObject* CServerExoApp::GetAreaOfEffectByGameObjectID(
    unsigned long oidObject)
{
    return m_pcExoAppInternal->GetAreaOfEffectByGameObjectID(oidObject);
}
// PC 0x004AE7E0  Xbox 0x0015E620
CSWSWaypoint* CServerExoApp::GetWaypointByGameObjectID(unsigned long oidObject)
{
    return m_pcExoAppInternal->GetWaypointByGameObjectID(oidObject);
}
// PC 0x004AE7F0  Xbox 0x0015E630
CSWSEncounter* CServerExoApp::GetEncounterByGameObjectID(
    unsigned long oidObject)
{
    return m_pcExoAppInternal->GetEncounterByGameObjectID(oidObject);
}
// PC 0x004AE800  Xbox 0x0015E640
CSWSSoundObject* CServerExoApp::GetSoundObjectByGameObjectID(
    unsigned long oidObject)
{
    return m_pcExoAppInternal->GetSoundObjectByGameObjectID(oidObject);
}
// PC 0x004AE810  Xbox 0x0015E650
int CServerExoApp::GetPlayerLanguage(unsigned long nPlayerId)
{
    return m_pcExoAppInternal->GetPlayerLanguage(nPlayerId);
}
// PC 0x004AE820  Xbox 0x0015E660
int CServerExoApp::GetModuleLanguage()
{
    return m_pcExoAppInternal->GetModuleLanguage();
}
// PC 0x004AE830  Xbox 0x0015E670
CWorldTimer* CServerExoApp::GetActiveTimer(unsigned long nTimer)
{
    return m_pcExoAppInternal->GetActiveTimer(nTimer);
}
// PC 0x004AE840  Xbox 0x0015E680
int CServerExoApp::HandleMessage(unsigned long nPlayerId,
                                 unsigned char* pMessage, unsigned long nSize,
                                 int bGuaranteed)
{
    return m_pcExoAppInternal->HandleMessage(nPlayerId, pMessage, nSize,
                                             bGuaranteed);
}
// PC 0x004AE850  Xbox 0x0015E690
void CServerExoApp::Initialize()
{
    m_pcExoAppInternal->Initialize();
}
// PC 0x004AE860  Xbox 0x0015E6A0
int CServerExoApp::MainLoop()
{
    return m_pcExoAppInternal->MainLoop();
}
// PC 0x004AE870  Xbox 0x0015E6B0
void CServerExoApp::PlayerListChange(unsigned long nPlayerId, int bAdded,
                                     int bGuaranteed)
{
    m_pcExoAppInternal->PlayerListChange(nPlayerId, bAdded, bGuaranteed);
}
// PC 0x004AE880  Xbox 0x0015E6C0
int CServerExoApp::ContinueMessageProcessing()
{
    return m_pcExoAppInternal->ContinueMessageProcessing();
}
// PC 0x004AE890  Xbox 0x0015E6D0
void CServerExoApp::Shutdown(int bExit, int bSave)
{
    m_pcExoAppInternal->Shutdown(bExit, bSave);
}
// PC 0x004AE8A0
int CServerExoApp::UnloadModule()
{
    return m_pcExoAppInternal->UnloadModule();
}
// PC 0x004AE8B0  Xbox 0x0015E6E0
int CServerExoApp::LoadModule(const CExoString& sModule, CSWSPlayer* pPlayer)
{
    return m_pcExoAppInternal->LoadModule(sModule, pPlayer);
}
// PC 0x004AE8C0  Xbox 0x0015E710
int CServerExoApp::RunModule()
{
    return m_pcExoAppInternal->RunModule();
}
// PC 0x004AE8D0  Xbox 0x0015E720
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
// PC 0x004AE8F0  Xbox 0x0015E750
void CServerExoApp::StartServices(int nServices)
{
    m_pcExoAppInternal->StartServices(nServices);
}
// PC 0x004AE900  Xbox 0x0015E760
void CServerExoApp::StopServices()
{
    m_pcExoAppInternal->StopServices();
}
// PC 0x004AE910  Xbox 0x0015E770
CSWSPlayer* CServerExoApp::GetClientObjectByObjectId(unsigned long oidObject)
{
    return m_pcExoAppInternal->GetClientObjectByObjectId(oidObject);
}
// PC 0x004AE920  Xbox 0x0015E780
CSWSPlayer* CServerExoApp::GetClientObjectByPlayerId(unsigned long nPlayerId,
                                                     unsigned char nGroup)
{
    return m_pcExoAppInternal->GetClientObjectByPlayerId(nPlayerId, nGroup);
}
// PC 0x004AE930  Xbox 0x0015E790
void CServerExoApp::InitiateModuleForPlayer(void* pData)
{
    m_pcExoAppInternal->InitiateModuleForPlayer(pData);
}
// PC 0x004AE940  Xbox 0x0015E7A0
void CServerExoApp::MovePlayerToArea(void* pData)
{
    m_pcExoAppInternal->MovePlayerToArea(pData);
}
// PC 0x004AE950  Xbox 0x0015E7B0
int CServerExoApp::GetFactionOfObject(unsigned long oidObject, int* pnFaction)
{
    return m_pcExoAppInternal->GetFactionOfObject(oidObject, pnFaction);
}
// PC 0x004AE960  Xbox 0x0015E7C0
void CServerExoApp::TogglePauseState(unsigned char nPauseType)
{
    m_pcExoAppInternal->TogglePauseState(nPauseType);
}
// PC 0x004AE970  Xbox 0x0015E7D0
void CServerExoApp::SkipPastDialog(unsigned long oidObject)
{
    m_pcExoAppInternal->SkipPastDialog(oidObject);
}
// PC 0x004AE980  Xbox 0x0015E7E0
int CServerExoApp::GetPauseState(unsigned char nPauseType)
{
    return m_pcExoAppInternal->GetPauseState(nPauseType);
}
// PC 0x004AE990  Xbox 0x0015E7F0
unsigned char CServerExoApp::GetActivePauseState()
{
    return m_pcExoAppInternal->GetActivePauseState();
}
// PC 0x004AE9A0  Xbox 0x0015E800
void CServerExoApp::SetPauseState(unsigned char nPauseType, int bPause)
{
    m_pcExoAppInternal->SetPauseState(nPauseType, bPause);
}
// PC 0x004AE9B0  Xbox 0x0015E810
void CServerExoApp::AddToExclusionList(unsigned long oidObject,
                                      unsigned char nType)
{
    m_pcExoAppInternal->AddToExclusionList(oidObject, nType);
}
// PC 0x004AE9C0  Xbox 0x0015E820
void CServerExoApp::RemoveFromExclusionList(unsigned long oidObject,
                                           unsigned char nType)
{
    m_pcExoAppInternal->RemoveFromExclusionList(oidObject, nType);
}
// PC 0x004AE9D0  Xbox 0x0015E830
CExoLinkedList<unsigned long>* CServerExoApp::GetActiveExclusionList()
{
    return m_pcExoAppInternal->GetActiveExclusionList();
}

int CServerExoApp::IsOnActiveExclusionList(unsigned long oidObject)
{
    return m_pcExoAppInternal->IsOnActiveExclusionList(oidObject);
}
// PC 0x004AE9E0  Xbox 0x0015E840
int CServerExoApp::ResolvePlayerByFirstName(const CExoString& sFirstName)
{
    return m_pcExoAppInternal->ResolvePlayerByFirstName(sFirstName);
}
// PC 0x004AE9F0  Xbox 0x0015E850
int CServerExoApp::GetDifficultyOption(int nOption)
{
    return m_pcExoAppInternal->GetDifficultyOption(nOption);
}
// PC 0x004AEA00  Xbox 0x0015E860
void CServerExoApp::AddCharListRequest(unsigned long nPlayerId)
{
    m_pcExoAppInternal->AddCharListRequest(nPlayerId);
}
// PC 0x004AEA10  Xbox 0x0015E870
int CServerExoApp::GetFirstPCObject()
{
    return m_pcExoAppInternal->GetFirstPCObject();
}
// PC 0x004AEA20  Xbox 0x0015E880
int CServerExoApp::GetNextPCObject()
{
    return m_pcExoAppInternal->GetNextPCObject();
}
// PC 0x004AEA30  Xbox 0x0015E890
unsigned long CServerExoApp::ClientToServerObjectId(unsigned long oidClient)
{
    return m_pcExoAppInternal->ClientToServerObjectId(oidClient);
}
// PC 0x004AEA40  Xbox 0x0015E8B0
int CServerExoApp::GetPlayerCreatureId()
{
    return m_pcExoAppInternal->GetPlayerCreatureId();
}
// PC 0x004AEA50  Xbox 0x0015E8C0
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
// PC 0x004AEAA0  Xbox 0x0015E910
void CServerExoApp::UpDatePlayerPosition(unsigned long oidObject,
                                         const Vector& vPosition,
                                         const Vector& vOrientation)
{
    unsigned long oidServer =
        m_pcExoAppInternal->ClientToServerObjectId(oidObject);
    m_pcExoAppInternal->UpDatePlayerPosition(oidServer, vPosition,
                                             vOrientation);
}
// PC 0x004AEAD0  Xbox 0x0015E940
void CServerExoApp::UpdateSubareasOnMoveTo(unsigned long oidObject,
                                           const Vector& vFrom,
                                           const Vector& vTo)
{
    m_pcExoAppInternal->UpdateSubareasOnMoveTo(oidObject, vFrom, vTo);
}
// PC 0x004AEAE0  Xbox 0x0015E950
int CServerExoApp::IsActionsPending(unsigned long oidObject)
{
    return m_pcExoAppInternal->IsActionsPending(oidObject);
}
// PC 0x004AEAF0  Xbox 0x0015E960
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
// PC 0x004AEB40  Xbox 0x0015E9B0
int CServerExoApp::CreatureAcquireItem(unsigned long oidObject,
                                       const CResRef& cResRef)
{
    return m_pcExoAppInternal->CreatureAcquireItem(oidObject, cResRef);
}
// PC 0x004AEB50  Xbox 0x0015E9C0
int CServerExoApp::TestSafeLocation(unsigned long oidObject,
                                    const Vector& vPosition)
{
    return m_pcExoAppInternal->TestSafeLocation(oidObject, vPosition);
}
// PC 0x004AEB60  Xbox 0x0015E9F0
int CServerExoApp::ComputeSafeLocation(const Vector& vPosition, float fRadius,
                                       unsigned long oidObject, int nFlags,
                                       Vector* pvSafe)
{
    return m_pcExoAppInternal->ComputeSafeLocation(vPosition, fRadius,
                                                   oidObject, nFlags, pvSafe);
}
// PC 0x004AEB70  Xbox 0x0015EA30
Vector CServerExoApp::GetNormalOfPoint(const Vector& vPosition)
{
    return m_pcExoAppInternal->GetNormalOfPoint(vPosition);
}
// PC 0x004AEB90  Xbox 0x0015EA60
void CServerExoApp::ClearLastModuleFound()
{
    m_pcExoAppInternal->ClearLastModuleFound();
}
// PC 0x004AEBA0  Xbox 0x0015EA70
void CServerExoApp::ClearAreaCache()
{
    m_pcExoAppInternal->ClearAreaCache();
}
// PC 0x004AEBB0  Xbox 0x0015EA80
int CServerExoApp::ShrinkLineByCreatures(const Vector& vStart,
                                         const Vector& vEnd, Vector& vResult)
{
    return m_pcExoAppInternal->ShrinkLineByCreatures(vStart, vEnd, vResult);
}
// PC 0x004AEBC0  Xbox 0x0015EA90
void CServerExoApp::SetMoveToWorldTime(
    unsigned long nCalendarDay, unsigned char nHour, unsigned char nMinute,
    unsigned char nSecond, unsigned short nMillisecond, unsigned short nYear,
    unsigned short nMonth)
{
    m_pcExoAppInternal->SetMoveToWorldTime(
        nCalendarDay, nHour, nMinute, nSecond, nMillisecond, nYear, nMonth);
}
// PC 0x004AEBD0  Xbox 0x0015EAA0
void CServerExoApp::GetMoveToWorldTime(
    unsigned long& nCalendarDay, unsigned char& nHour, unsigned char& nMinute,
    unsigned char& nSecond, unsigned short& nMillisecond, unsigned short& nYear,
    unsigned short& nMonth)
{
    m_pcExoAppInternal->GetMoveToWorldTime(
        nCalendarDay, nHour, nMinute, nSecond, nMillisecond, nYear, nMonth);
}
// PC 0x004AEBE0  Xbox 0x0015EAB0
int CServerExoApp::GetModulePreviouslyVisited(const CExoString& sModule)
{
    return m_pcExoAppInternal->GetModulePreviouslyVisited(sModule);
}
// PC 0x004AEBF0  Xbox 0x0015EAC0
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
// PC 0x004AEC00  Xbox 0x0015EAF0
CServerExoApp::CServerExoApp()
{
    m_pcExoAppInternal = new CServerExoAppInternal;
}
// PC 0x004AEC70  Xbox 0x0015EB70
CServerExoApp::~CServerExoApp()
{
    if (m_pcExoAppInternal != NULL)
    {
        delete m_pcExoAppInternal;
    }
}
// PC 0x004AEC90  Xbox 0x0015EB90
CServerInfo* CServerExoApp::GetServerInfo()
{
    return m_pcExoAppInternal->m_pServerInfo;
}
// PC 0x004AECA0  Xbox 0x0015EBA0
CExoLocString* CServerExoApp::GetModuleDescription()
{
    if (m_pcExoAppInternal->GetModule() != NULL)
    {
        return &m_pcExoAppInternal->GetModule()->m_sDescription;
    }

    return NULL;
}
// PC 0x004AECC0  Xbox 0x0015EBC0
void CServerExoApp::SetMoveToModulePending(int bPending)
{
    m_pcExoAppInternal->m_bMoveToModulePending = bPending;
}

int CServerExoApp::GetMoveToModulePending()
{
    return m_pcExoAppInternal->m_bMoveToModulePending;
}
// PC 0x004AECD0  Xbox 0x0015EBD0
void CServerExoApp::SetMoveToModuleString(const CExoString& sModule)
{
    m_pcExoAppInternal->m_sMoveToModule = sModule;
}

CExoString CServerExoApp::GetMoveToModuleString()
{
    return m_pcExoAppInternal->m_sMoveToModule;
}
// PC 0x004AECE0  Xbox 0x0015EBE0
void CServerExoApp::SetExportAllCharactersPending(int bPending)
{
    m_pcExoAppInternal->m_bExportAllCharactersPending = bPending;
}

int CServerExoApp::ExportAllPlayers()
{
    return m_pcExoAppInternal->ExportAllPlayers();
}
// PC 0x004AECF0  Xbox 0x0015EBF0
int CServerExoApp::GetLoadingModule()
{
    return m_pcExoAppInternal->m_bLoadingModule;
}
// PC 0x004AED00  Xbox 0x0015EC00
void CServerExoApp::SetLoadingModule(int bLoading)
{
    m_pcExoAppInternal->m_bLoadingModule = bLoading;
}
// PC 0x004AED10  Xbox 0x0015EC10
void CServerExoApp::SetAutoSavePending(int bPending)
{
    m_pcExoAppInternal->m_bAutoSavePending = bPending;
}
// PC 0x004AED20  Xbox 0x0015EC20
int CServerExoApp::GetAutoSavePending()
{
    return m_pcExoAppInternal->m_bAutoSavePending;
}
// PC 0x004AED30  Xbox 0x0015EC30
void CServerExoApp::SetMoveToModuleStartWaypoint(const CExoString& sWaypoint)
{
    m_pcExoAppInternal->m_sMoveToModuleStartWaypoint = sWaypoint;
}
// PC 0x004AED40
CExoString CServerExoApp::GetMoveToModuleStartWaypoint()
{
    return m_pcExoAppInternal->m_sMoveToModuleStartWaypoint;
}
// PC 0x004AED70  Xbox 0x0015EC40
CGameObjectArray* CServerExoApp::GetObjectArray()
{
    return m_pcExoAppInternal->m_pObjectArray;
}
// PC 0x004AED80  Xbox 0x0015EC50
CServerAIMaster* CServerExoApp::GetServerAIMaster()
{
    return m_pcExoAppInternal->m_pServerAIMaster;
}
// PC 0x004AED90  Xbox 0x0015EC60
CExoLinkedList<CSWSClient>* CServerExoApp::GetPlayerList()
{
    return m_pcExoAppInternal->m_pPlayerList;
}
// PC 0x004AEDA0  Xbox 0x0015EC70
CNetLayer* CServerExoApp::GetNetLayer()
{
    return m_pcExoAppInternal->m_pNetLayer;
}
// PC 0x004AEDB0  Xbox 0x0015EC80
CSWSMessage* CServerExoApp::GetSWSMessage()
{
    return m_pcExoAppInternal->m_pSWSMessage;
}
// PC 0x004AEDC0  Xbox 0x0015EC90
CExoLinkedList<CSWSClient>* CServerExoApp::GetSysAdminList()
{
    return m_pcExoAppInternal->m_pSysAdminList;
}
// PC 0x004AEDD0  Xbox 0x0015ECA0
short CServerExoApp::GetServerMode()
{
    return m_pcExoAppInternal->m_nServerMode;
}
// PC 0x004AEDE0  Xbox 0x0015ECB0
CWorldTimer* CServerExoApp::GetWorldTimer()
{
    return m_pcExoAppInternal->m_pWorldTimer;
}
// PC 0x004AEDF0  Xbox 0x0015ECC0
CWorldTimer* CServerExoApp::GetTimestopTimer()
{
    return m_pcExoAppInternal->m_pTimestopTimer;
}
// PC 0x004AEE00  Xbox 0x0015ECD0
CWorldTimer* CServerExoApp::GetPauseTimer()
{
    return m_pcExoAppInternal->m_pPauseTimer;
}
// PC 0x004AEE10  Xbox 0x0015ECE0
void CServerExoApp::SetDebugMode(int nDebugMode)
{
    m_pcExoAppInternal->m_nDebugMode = nDebugMode;
}
// PC 0x004AEE20  Xbox 0x0015ECF0
int CServerExoApp::GetDebugMode()
{
    return m_pcExoAppInternal->m_nDebugMode;
}
// PC 0x004AEE30  Xbox 0x0015ED00
CConnectionLib* CServerExoApp::GetConnectionLib()
{
    return m_pcExoAppInternal->m_pConnectionLib;
}
// PC 0x004AEE40  Xbox 0x0015ED10
int CServerExoApp::IsInjured(unsigned long oidObject)
{
    unsigned long oidServer =
        m_pcExoAppInternal->ClientToServerObjectId(oidObject);
    return m_pcExoAppInternal->IsInjured(oidServer);
}
// PC 0x004AEE60  Xbox 0x0015ED30
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
// PC 0x004AEE70  Xbox 0x0015ED40
CSWPartyTable* CServerExoApp::GetPartyTable()
{
    return &m_pcExoAppInternal->m_partyTable;
}
// PC 0x004AEE80  Xbox 0x0015ED50
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
// PC 0x004AEF10  Xbox 0x0015EDE0
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
// PC 0x004AEF40  Xbox 0x0015EE10
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
// PC 0x004AEFA0  Xbox 0x0015EE70
void CServerExoApp::ClearAllQueuedCombatActions(unsigned long oidObject)
{
    CSWSCreature* pCreature =
        m_pcExoAppInternal->GetCreatureByGameObjectID(oidObject);
    if (pCreature != NULL)
    {
        pCreature->m_pcCombatRound->RemoveAllActions();
    }
}
// PC 0x004AEFC0  Xbox 0x0015EE90
CFactionManager* CServerExoApp::GetFactionManager()
{
    return m_pcExoAppInternal->m_pFactionManager;
}
// PC 0x004AEFD0  Xbox 0x0015EEA0
void CServerExoApp::SetLastConversation(const CExoString& sConversation)
{
    m_pcExoAppInternal->m_sLastConversation = sConversation;
}
// PC 0x004AEFE0  Xbox 0x0015EEB0
CExoString CServerExoApp::GetLastConversation()
{
    return m_pcExoAppInternal->m_sLastConversation;
}
// PC 0x004AF010  Xbox 0x0015EED0
void CServerExoApp::SetActionConversation(int bActionConversation)
{
    m_pcExoAppInternal->m_nActionConversation = bActionConversation;
}
// PC 0x004AF020  Xbox 0x0015EEE0
int CServerExoApp::GetActionConversation()
{
    return m_pcExoAppInternal->m_nActionConversation;
}
// PC 0x004AF030  Xbox 0x0015EEF0
void CServerExoApp::SetIgnoreConversationStartRange(int bIgnore)
{
    m_pcExoAppInternal->m_bIgnoreConversationStartRange = bIgnore;
}
// PC 0x004AF040  Xbox 0x0015EF00
int CServerExoApp::GetIgnoreConversationStartRange()
{
    return m_pcExoAppInternal->m_bIgnoreConversationStartRange;
}
// PC 0x004AF050  Xbox 0x0015EF10
int CServerExoApp::GetLoadFromSaveGame()
{
    return m_pcExoAppInternal->m_bLoadFromSaveGame;
}
// PC 0x004AF060  Xbox 0x0015EF20
void CServerExoApp::SetLoadFromSaveGame(int bLoadFromSaveGame)
{
    m_pcExoAppInternal->m_bLoadFromSaveGame = bLoadFromSaveGame;
}
// PC 0x004AF070  Xbox 0x0015EF30
int CServerExoApp::GetButtonMashCheck()
{
    return m_pcExoAppInternal->m_bButtonMashCheck;
}
// PC 0x004AF080  Xbox 0x0015EF40
void CServerExoApp::SetButtonMashCheck(int bButtonMashCheck)
{
    m_pcExoAppInternal->m_bButtonMashCheck = bButtonMashCheck;
}
// PC 0x004AF090  Xbox 0x0015EF50
void CServerExoApp::SetDoingStartNewModule(int bDoingStartNewModule)
{
    m_pcExoAppInternal->m_bDoingStartNewModule = bDoingStartNewModule;
}
// PC 0x004AF0A0  Xbox 0x0015EF60
void CServerExoApp::SetMoveToPausedCalendarDay(unsigned long nCalendarDay)
{
    m_pcExoAppInternal->m_nMoveToPausedCalendarDay = nCalendarDay;
}
// PC 0x004AF0B0  Xbox 0x0015EF70
unsigned long CServerExoApp::GetMoveToPausedCalendarDay()
{
    return m_pcExoAppInternal->m_nMoveToPausedCalendarDay;
}
// PC 0x004AF0C0  Xbox 0x0015EF80
void CServerExoApp::SetMoveToPausedTimeOfDay(unsigned long nTimeOfDay)
{
    m_pcExoAppInternal->m_nMoveToPausedTimeOfDay = nTimeOfDay;
}
// PC 0x004AF0D0  Xbox 0x0015EF90
unsigned long CServerExoApp::GetMoveToPausedTimeOfDay()
{
    return m_pcExoAppInternal->m_nMoveToPausedTimeOfDay;
}
// PC 0x004AF0E0
void CServerExoApp::SetSaveGameName(const CExoString& sName)
{
#ifndef _XBOX
    m_pcExoAppInternal->m_sSaveGameName = sName;
#endif
}
// PC 0x005ED310  Xbox 0x0015E740
void CServerExoApp::Uninitialize()
{
    m_pcExoAppInternal->Uninitialize();
}
// PC 0x005ED990
int CServerExoApp::GetApplicationId()
{
    return g_pAppManager->m_nAppId;
}
