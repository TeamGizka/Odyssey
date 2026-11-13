#include <windows.h>
#include "swmain.h"
#include "exobase.h"
#include "netlayer.h"
#include "worldtimer.h"
#include "gameobjectarray.h"
#include "nwssysadmin.h"
#include "clientexoapp.h"
#include "swsareamap.h"
#include "nwseffectlist.h"

template CExoLinkedList<unsigned long>::CExoLinkedList();
#include "nwsscriptevent.h"
#include "swmain.h"
#include "exoinput.h"
#include "exoinput.h"
#include "nwmessage.h"
#include "nwsarea.h"
#include "nwsitem.h"
#include "nwsmodule.h"
#include "nwsobject.h"
#include "nwsplayer.h"
#include "appmanager.h"
#include "exoresman.h"
#include "serverinfo.h"
#include "nwreentrantserver.h"
#include "resgff.h"
#include "serverexoappinternal.h"
#ifdef _XBOX
#include "LaunchData.h"
void ExoXboxLaunch(KOTOR_LAUNCH_DATA* pParams, const char* pszModuleName);
void AurPersistDisplay();
void AurReleaseFrameTexture();
#endif

static inline void ServerExoAppTrace(CServerExoApp* = 0);
// PC 0x004E9EF0 GOG 0x004EAC50
inline int CStatusSummary::SaveToGFF(CResGFF* pRes, CResStruct& stStruct)
{
    ServerExoAppTrace();
    pRes->WriteFieldBYTE(&stStruct, m_bDisplaySPending, "DISPLAYSPENDING");
    pRes->WriteFieldBYTE(&stStruct, m_bItemReceived, "ITEMRECEIVED");
    pRes->WriteFieldBYTE(&stStruct, m_bItemLost, "ITEMLOST");
    pRes->WriteFieldBYTE(&stStruct, m_bJournal, "JOURNAL");
    pRes->WriteFieldBYTE(&stStruct, m_nLightShift, "LIGHTSHIFT");
    pRes->WriteFieldBYTE(&stStruct, m_nDarkShift, "DARKSHIFT");
    pRes->WriteFieldINT(&stStruct, m_nCredits, "CREDITS");
    pRes->WriteFieldBYTE(&stStruct, m_bCreditsNet, "CREDITSNET");
    pRes->WriteFieldINT(&stStruct, m_nExperience, "XP");
    pRes->WriteFieldINT(&stStruct, m_nStealthExperience, "STEALTHXP");
    pRes->WriteFieldBYTE(&stStruct, m_bSoundPending, "SOUNDPENDING");
    pRes->WriteFieldBYTE(&stStruct, m_bLevelUpSound, "LEVELUPSOUND");
    pRes->WriteFieldBYTE(&stStruct, m_bNewQuestSound, "NEWQUESTSOUND");
    pRes->WriteFieldBYTE(&stStruct, m_bCompleteSound, "COMPLETESOUND");
    pRes->WriteFieldBYTE(&stStruct, m_bDarkAlignSound, "DARKALIGNSOUND");
    pRes->WriteFieldBYTE(&stStruct, m_bLightAlignSound, "LIGHTALIGNSOUND");
    pRes->WriteFieldINT(&stStruct, m_nSuppressed, "SUPPRESSED");
    pRes->WriteFieldINT(&stStruct, m_nInfluenceGain, "INFGAIN");
    pRes->WriteFieldINT(&stStruct, m_nInfluenceLost, "INFLOST");
    pRes->WriteFieldINT(&stStruct, m_nMaxForcePointsGain, "MAXFPGAIN");
    return pRes->WriteFieldINT(&stStruct, m_nMaxForcePointsLost, "MAXFPLOST");
}

#include "serverexoapp.h"
#include "swguisaveload.h"
// PC 0x004EC420 GOG 0x004EAF10
inline int KOTOR_AUTOSAVE_PARAMS::SaveToGFF(CResGFF* pRes, CResStruct& stStruct)
{
    CResStruct stStatusSummary;

    pRes->WriteFieldCExoString(&stStruct, m_sLoadMusic, "LOADMUSIC");
    pRes->WriteFieldCExoString(&stStruct, m_sStartWaypoint, "STARTWAYPOINT");
    pRes->WriteFieldCExoString(&stStruct, m_sMovie1, "MOVIE1");
    pRes->WriteFieldCExoString(&stStruct, m_sMovie2, "MOVIE2");
    pRes->WriteFieldCExoString(&stStruct, m_sMovie3, "MOVIE3");
    pRes->WriteFieldCExoString(&stStruct, m_sMovie4, "MOVIE4");
    pRes->WriteFieldCExoString(&stStruct, m_sMovie5, "MOVIE5");
    pRes->WriteFieldCExoString(&stStruct, m_sMovie6, "MOVIE6");
    pRes->WriteFieldDWORD(&stStruct, m_nTimeYear, "TIME_YEAR");
    pRes->WriteFieldBYTE(&stStruct, m_nTimeMonth, "TIME_MONTH");
    pRes->WriteFieldBYTE(&stStruct, m_nTimeDay, "TIME_DAY");
    pRes->WriteFieldBYTE(&stStruct, m_nTimeHour, "TIME_HOUR");
    pRes->WriteFieldWORD(&stStruct, m_nTimeMinute, "TIME_MINUTE");
    pRes->WriteFieldWORD(&stStruct, m_nTimeSecond, "TIME_SECOND");
    pRes->WriteFieldWORD(&stStruct, m_nTimeMillisecond, "TIME_MILLISECOND");
    pRes->WriteFieldDWORD(&stStruct, m_nTimePauseDay, "TIME_PAUSEDAY");
    pRes->WriteFieldDWORD(&stStruct, m_nTimePauseTime, "TIME_PAUSETIME");
    pRes->AddStructToStruct(&stStatusSummary, &stStruct, "STATUSSUMMARY", 0);
    return m_cStatusSummary.SaveToGFF(pRes, stStatusSummary);
}

#include "nwrules.h"
#include "nwscript.h"
#include "twodimarray.h"
#include "clientoptions.h"
#include "swparty.h"
#include "nwccreature.h"
#include "swsroom.h"
#include "nwsdoor.h"
#include "nwsencounter.h"
#include "nwsplaceable.h"
#include "nwstrigger.h"
#include "nwscreaturestats.h"
#include "appmanager.h"
#include "pathfindinfo.h"
#include "nwswaypoint.h"
#include "guiingame.h"
#include "nwcmodule.h"
#include "aurcamera.h"
#include "swbehaviorcamera.h"
#include "swgui.h"
#include "nwcarea.h"
#include "nwseffect.h"
#include "serveraimaster.h"
#include "factionmanager.h"
#include "nwvmachinecommands.h"
#include "exotlktable.h"
#include "twodimarrays.h"
#include "clientexoappinternal.h"
#include "nwerffile.h"
#include "nwerffile.h"

static inline void ServerExoAppInternalTrace2();
static inline void ServerExoAppTrace(CServerExoApp*);
#define SERVEREXOAPPINTERNAL_HOOK() ServerExoAppInternalTrace2()
#define STRREF_AUTO_SAVE_NAME 1593

inline void ServerExoAppInternalReserve(CExoArrayList<unsigned long>* pIds, int nSize, CExoString* pStrings,
    CExoLinkedList<CSWSClient>* pClients, CExoLinkedList<unsigned long>* pExclusions)
{
    pIds->Allocate(nSize);
    delete[] pStrings;
    delete pClients;
    delete pExclusions;
}

static inline CServerExoApp* GetServerExoApp()
{
    return g_pAppManager->m_pServerExoApp;
}

static inline CClientExoApp* GetClientExoApp()
{
    return g_pAppManager->m_pClientExoApp;
}

static inline CSWReentrantServerStats* GetReentrantServerStats()
{
    return g_pAppManager->m_pReentrantServerStats;
}

static inline unsigned __int64 GetHighResolutionTimer()
{
    return g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
}

static inline void ServerExoAppInternalTrace2()
{
    ServerExoAppTrace();
}

static inline void ServerExoAppTrace(CServerExoApp*)
{
}

CServerExoAppInternal* CServerExoAppInternal::s_pServerExoAppInternal;

int g_bSavePlayerBIC = 0;

int RenderValidatePath = 0;
int OverrideRenderValidatePath = 0;
int EnablePathValidation = 0;

static char* CMessageServerAdmin_Echo = "Echo";
static char* CMessageServerAdmin_ServerStatusCategory = "ServerStatus";
static char* CMessageServerAdmin_ServerStatus_GetStatus = "GetStatus";
static char* CMessageServerAdmin_ServerStatus_ModuleLoaded = "ModuleLoaded";
static char* CMessageServerAdmin_ServerStatus_Idle = "Idle";
static char* CMessageServerAdmin_ServerStatus_ModuleRunning = "ModuleRunning";
static char* CMessageServerAdmin_ServerStatus_GetModuleList = "GetModuleList";
static char* CMessageServerAdmin_ServerStatus_ModuleList = "ModuleList";
static char* CMessageServerAdmin_ServerStatus_GetCharactervaultList =
    "GetCharactervaultList";
static char* CMessageServerAdmin_ServerStatus_CharactervaultList =
    "CharactervaultList";
static char* CMessageServerAdmin_ServerStatus_GetPlayerList = "GetPlayerList";
static char* CMessageServerAdmin_ServerStatus_PlayerList = "PlayerList";
static char* CMessageServerAdmin_ServerStatus_ShutDown = "ShutDown";
static char* CMessageServerAdmin_ServerStatus_Reset = "Reset";
static char* CMessageServerAdmin_ServerStatus_ServerError = "ServerError";
static char* CMessageServerAdmin_Module = "Module";
static char* CMessageServerAdmin_Module_Load = "Load";
static char* CMessageServerAdmin_Module_Run = "Run";
static char* CMessageServerAdmin_Module_Save = "Save";
static char* CMessageServerAdmin_Module_GetSavegameList = "GetSavegameList";
static char* CMessageServerAdmin_Module_SavegameList = "SavegameList";
static char* CMessageServerAdmin_Module_LoadSavegame = "LoadSavegame";
static char* CMessageServerAdmin_JoiningRestrictions = "JoiningRestrictions";
static char* CMessageServerAdmin_JoiningRestrictions_AllowLocal = "AllowLocal";
static char* CMessageServerAdmin_JoiningRestrictions_AllowServer =
    "AllowServer";
static char* CMessageServerAdmin_JoiningRestrictions_AllowVault = "AllowVault";
static char* CMessageServerAdmin_JoiningRestrictions_FixedParties =
    "FixedParties";
static char* CMessageServerAdmin_JoiningRestrictions_AllowFighter =
    "AllowFighter";
static char* CMessageServerAdmin_JoiningRestrictions_AllowBarbarian =
    "AllowBarbarian";
static char* CMessageServerAdmin_JoiningRestrictions_AllowRanger =
    "AllowRanger";
static char* CMessageServerAdmin_JoiningRestrictions_AllowPaladin =
    "AllowPaladin";
static char* CMessageServerAdmin_JoiningRestrictions_AllowWizard =
    "AllowWizard";
static char* CMessageServerAdmin_JoiningRestrictions_AllowSorcerer =
    "AllowSorcerer";
static char* CMessageServerAdmin_JoiningRestrictions_AllowCleric =
    "AllowCleric";
static char* CMessageServerAdmin_JoiningRestrictions_AllowDruid = "AllowDruid";
static char* CMessageServerAdmin_JoiningRestrictions_AllowRogue = "AllowRogue";
static char* CMessageServerAdmin_JoiningRestrictions_AllowBard = "AllowBard";
static char* CMessageServerAdmin_JoiningRestrictions_AllowMonk = "AllowMonk";
static char* CMessageServerAdmin_JoiningRestrictions_AllowHuman = "AllowHuman";
static char* CMessageServerAdmin_JoiningRestrictions_AllowElf = "AllowElf";
static char* CMessageServerAdmin_JoiningRestrictions_AllowDwarf = "AllowDwarf";
static char* CMessageServerAdmin_JoiningRestrictions_AllowGnome = "AllowGnome";
static char* CMessageServerAdmin_JoiningRestrictions_AllowHalfling =
    "AllowHalfling";
static char* CMessageServerAdmin_JoiningRestrictions_AllowHalfOrc =
    "AllowHalfOrc";
static char* CMessageServerAdmin_JoiningRestrictions_AllowHalfElf =
    "AllowHalfElf";
static char* CMessageServerAdmin_JoiningRestrictions_MaxPlayers = "MaxPlayers";
static char* CMessageServerAdmin_JoiningRestrictions_MaxParties = "MaxParties";
static char* CMessageServerAdmin_JoiningRestrictions_MaxItemPoints =
    "MaxItemPoints";
static char* CMessageServerAdmin_JoiningRestrictions_MaxStatTotal =
    "MaxStatTotal";
static char* CMessageServerAdmin_JoiningRestrictions_MinLevel = "MinLevel";
static char* CMessageServerAdmin_JoiningRestrictions_MaxLevel = "MaxLevel";
static char* CMessageServerAdmin_PlayOptions = "PlayOptions";
static char* CMessageServerAdmin_PlayOptions_AllKillable = "AllKillable";
static char* CMessageServerAdmin_PlayOptions_NonPartyKillable =
    "NonPartyKillable";
static char* CMessageServerAdmin_PlayOptions_RequireResurrection =
    "RequireResurrection";
static char* CMessageServerAdmin_PlayOptions_LoseStolenItems =
    "LoseStolenItems";
static char* CMessageServerAdmin_PlayOptions_LoseItems = "LoseItems";
static char* CMessageServerAdmin_PlayOptions_LoseExp = "LoseExp";
static char* CMessageServerAdmin_PlayOptions_LoseGold = "LoseGold";
static char* CMessageServerAdmin_PlayOptions_LoseGoldNum = "LoseGoldNum";
static char* CMessageServerAdmin_PlayOptions_LoseExpNum = "LoseExpNum";
static char* CMessageServerAdmin_PlayOptions_LoseItemsNum = "LoseItemsNum";
static char* CMessageServerAdmin_PlayOptions_GetPVP = "GetPVP";
static char* CMessageServerAdmin_PlayOptions_SetPVP = "SetPVP";
static char* CMessageServerAdmin_PlayOptions_PVP_Full = "full";
static char* CMessageServerAdmin_PlayOptions_PVP_Party = "party";
static char* CMessageServerAdmin_PlayOptions_PVP_None = "none";
static char* CMessageServerAdmin_PlayOptions_GetPauseAndPlay = "PauseAndPlay";
static char* CMessageServerAdmin_ResetOption = "ResetOption";
static char* CMessageServerAdmin_ResetOption_OnVacant = "OnVacant";
static char* CMessageServerAdmin_ResetOption_OnVictory = "OnVictory";
static char* CMessageServerAdmin_ResetOption_OnFailure = "OnFailure";
static char* CMessageServerAdmin_ResetOption_OnDeath = "OnDeath";
static char* CMessageServerAdmin_ResetOption_OnPartyDeath = "OnPartyDeath";
static char* CMessageServerAdmin_AutoSave = "AutoSave";
static char* CMessageServerAdmin_AutoSave_AtIntervals = "AtIntervals";
static char* CMessageServerAdmin_AutoSave_AtSavePoints = "AtSavePoints";
static char* CMessageServerAdmin_AutoSave_OnPlayerChange = "OnPlayerChange";
static char* CMessageServerAdmin_AutoSave_Interval = "Interval";
static char* CMessageServerAdmin_Difficulty = "Difficulty";
static char* CMessageServerAdmin_Control = "Control";
static char* CMessageServerAdmin_Control_Boot = "Boot";
static char* CMessageServerAdmin_Control_Ban = "Ban";
static char* CMessageServerAdmin_Control_ChangeSAPassword = "SAPass";
static char* CMessageServerAdmin_Control_ChangeGMPassword = "GMPass";
static char* CMessageServerAdmin_Control_Say = "Say";

const unsigned __int64 CServerExoAppInternal::CCLIENTGAMEOBJECTUPDATETIME = 200000;
const int CServerExoAppInternal::STRREF_DISCONNECT_SERVERSIDE = 0;
const int CServerExoAppInternal::STRREF_DISCONNECT_CLIENTSIDE = 0;
const unsigned __int64 CServerExoAppInternal::SHUTDOWN_TIMER_CLIENTCHARS = 180000000;
const unsigned __int64 CServerExoAppInternal::SHUTDOWN_TIMER_SERVERCHARS = 1000000;
const unsigned __int64 CServerExoAppInternal::SHUTDOWN_TIMER_WARNING1 = 60000000;
const unsigned __int64 CServerExoAppInternal::SHUTDOWN_TIMER_WARNING2 = 30000000;
const int CServerExoAppInternal::STRREF_LOGIN_FAIL_NOTALLOWLOCALVAULT = 0;
const int CServerExoAppInternal::STRREF_LOGIN_FAIL_NOTALLOWSERVERVAULT = 0;
const int CServerExoAppInternal::STRREF_LOGIN_FAIL_NOTALLOWEDTOUSECHAR = 0;

float fFloorHeight = -9999.0f;
float fRoofHeight = 9999.0f;
int UpdateLoadScreen(int nCurrent, int nTotal, int nBase, int nRange);

static inline CServerExoApp* GetServerExoApp();
static inline CClientExoApp* GetClientExoApp();
static inline CSWReentrantServerStats* GetReentrantServerStats();
// PC 0x00405980 GOG 0x00405980
template <> inline CExoArrayList<CExoString>::CExoArrayList(int nSize)
{
    num = 0;
    array_size = 0;
    element = NULL;
    if (nSize != 0)
    {
        Allocate(nSize);
    }
}

inline CVirtualMachineCmdImplementer::CVirtualMachineCmdImplementer()
{
}
// PC 0x004EA1C0 GOG 0x004E9B20
CSWSModule* CServerExoAppInternal::GetModule()
{
    unsigned long oidModule;
    CGameObjectArray* pObjectArray;
    CGameObject* pObject;
    CSWSModule* pModule;
    CGameObject* pFoundObject;

    oidModule = m_oidModule;
    if (m_oidLastModuleFound != oidModule)
    {
#ifdef _XBOX
        pFoundObject = NULL;
        EXOXBOXASSERT(m_pObjectArray);
        m_pObjectArray->GetGameObject(m_oidModule, &pFoundObject);
#else
        pObjectArray = m_pObjectArray;
        pFoundObject = NULL;
        pObjectArray->GetGameObject(oidModule, &pFoundObject);
#endif
        pObject = pFoundObject;
        if (pFoundObject != NULL)
        {
            m_oidLastModuleFound = m_oidModule;
            pModule = pObject->AsSWSModule();
            m_pLastModuleFound = pModule;
            return pModule;
        }
        m_oidLastModuleFound = OBJECT_INVALID;
        m_pLastModuleFound = NULL;
    }
    return m_pLastModuleFound;
}
// PC 0x004EA230 GOG 0x004E9B90
void CServerExoAppInternal::ClearLastModuleFound()
{
    m_oidLastModuleFound = OBJECT_INVALID;
    m_pLastModuleFound = NULL;
}
// PC 0x004EA250 GOG 0x004E9BB0
CExoString CServerExoAppInternal::GetModuleName()
{
    if (GetModule() != NULL)
    {
        CExoString sModuleName;
        sModuleName = m_pServerInfo->m_sModuleName;
        return sModuleName;
    }
    return CExoString();
}
// PC 0x004EA300 GOG 0x004EB6B0
int CServerExoAppInternal::SendCharacterQuery(CSWSPlayer* pPlayer)
{
    unsigned char nClassCount;
    int nClasses[2];
    unsigned char nClassLevels[2];
    unsigned long nExperience;
    CSWSMessage* pMessage;

    pMessage = GetServerExoApp()->GetSWSMessage();
    pPlayer->GetCharacterInfoFromIFO(nClassCount, nClasses, nClassLevels,
                                    nExperience);
    pMessage->SendServerToPlayerLogin_CharacterQuery(
        pPlayer, nClassCount, nClasses, nClassLevels, nExperience);
    return 1;
}
// PC 0x004EA360 GOG 0x004E9C60
int CServerExoAppInternal::HandleMessage(unsigned long nPlayerId,
                                         unsigned char* pMessage,
                                         unsigned long nSize, int bGuaranteed)
{
    if (bGuaranteed == 0)
    {
        if (*pMessage == 's' &&
            m_pNetLayer->GetPlayerInfo(nPlayerId)->m_bAuthorized == 1)
        {
            m_pSWSMessage->HandleServerAdminToServerMessage(nPlayerId, pMessage,
                                                            nSize);
        }
        if (*pMessage == 'p' &&
            m_pNetLayer->GetPlayerInfo(nPlayerId)->m_bPlayer == 1)
        {
            m_pSWSMessage->HandlePlayerToServerMessage(nPlayerId, pMessage,
                                                       nSize);
        }
        return 1;
    }
    return 0;
}
// PC 0x004EA3E0 GOG 0x004E9CE0
int CServerExoAppInternal::Initialize()
{
    CResRef cResRef("globalcat");

    m_globalVariableTable.ReadCatalogue(cResRef);
    return 1;
}
// PC 0x004EA410 GOG 0x004E9D10
int CServerExoAppInternal::CopyModuleToCurrentGame(
    const CExoString& sSource, const CExoString& sDestination,
    unsigned short nFlags)
{
    int nResult = CopyFile(sSource.CStr(), sDestination.CStr(), FALSE);

    if (nResult == 0)
    {
        GetLastError();
        return 0;
    }

    if ((GetFileAttributes(sDestination.CStr()) &
         FILE_ATTRIBUTE_READONLY) != 0)
    {
        SetFileAttributes(sDestination.CStr(), FILE_ATTRIBUTE_NORMAL);
    }
    return 1;
}
// PC 0x004EA470 GOG 0x004EB710
int UpdateLoadScreen(int nCurrent, int nTotal, int nBase, int nRange)
{
#ifdef _XBOX
    GetClientExoApp()->SetLoadBarProgress(
        (int)((float)nCurrent / (float)nTotal * (float)nRange) + nBase, 0);
#else
    GetClientExoApp()->SetLoadBarProgress(
        nBase + (unsigned __int64)((double)nCurrent / (double)nTotal *
                                   (double)nRange),
        0);
#endif
    UpdateScreen(0.033333335f, 0, 0);
    return 0;
}
void CServerExoAppInternal::InitiateModuleForPlayer(void* pData)  // PC 0x004EA4B0 GOG 0x004EC2B0
{
    CSWSPlayer* pPlayer = (CSWSPlayer*)pData;
    CSWSCreature* pCreature;
    CSWSArea* pArea;
    if (pPlayer->GetGameObject() == NULL)
    {
        pCreature = new CSWSCreature(OBJECT_INVALID, 1);
        unsigned long oidCreature = pCreature->m_nId;
        pCreature->m_pPathfindInformation->m_oidPathfindCreature = oidCreature;
        pPlayer->SetGameObject((CSWSObject*)pCreature);
    }
    else
    {
        pCreature = pPlayer->GetGameObject()->AsSWSCreature();
    }
    int bModuleCharacterLoaded = pPlayer->m_bModuleCharacterLoaded;
    int bPlayerIsNewToModule = (bModuleCharacterLoaded == 0);
    if (bPlayerIsNewToModule)
    {
        Vector vOrientation;
        pArea = GetModule()->GetArea();
        pCreature->m_oidDisappearTarget = pArea->GetId();
        pCreature->m_vDisappearPosition =
            Vector(GetModule()->m_pEntryInfo->m_fEntryX,
                   GetModule()->m_pEntryInfo->m_fEntryY,
                   GetModule()->m_pEntryInfo->m_fEntryZ);
        pCreature->m_bWorkRemainingCompleteSent = 0;
        vOrientation.m_fX = GetModule()->m_pEntryInfo->m_fEntryDirX;
        vOrientation.m_fY = GetModule()->m_pEntryInfo->m_fEntryDirY;
        vOrientation.m_fZ = 0.0f;
        pCreature->CSWSObject::SetOrientation(vOrientation);
    }
    else
    {
        pArea = pCreature->GetArea();
        if (pArea != NULL)
        {
            pCreature->m_oidDisappearTarget = pArea->GetId();
            pCreature->m_vDisappearPosition = pCreature->m_vPosition;
            pCreature->m_bWorkRemainingCompleteSent = 0;
        }
        else
        {
#ifdef _XBOX
            unsigned long oidArea = pCreature->m_oidDisappearTarget;
            pArea = g_pAppManager->m_pServerExoApp->GetAreaByGameObjectID(oidArea);
#else
            pArea = g_pAppManager->m_pServerExoApp->GetAreaByGameObjectID(
                pCreature->m_oidDisappearTarget);
#endif
        }
    }

    if (pArea == NULL)
    {
        return;
    }
    pCreature->SetArea(NULL);

    CSWSMessage* pMessage = g_pAppManager->m_pServerExoApp->GetSWSMessage();
    if (pMessage == NULL)
    {
        return;
    }
    pPlayer->SetAreaTransitionBMP(pArea->GetLoadScreenID(), "");
    pMessage->SendServerToPlayerArea_ClientArea(
        pPlayer, pArea, pCreature->m_vDisappearPosition.m_fX,
        pCreature->m_vDisappearPosition.m_fY,
        pCreature->m_vDisappearPosition.m_fZ, pCreature->m_vOrientation,
        bPlayerIsNewToModule);
    pMessage->SendServerToPlayerSetCustomTokenList(pPlayer->m_nPlayerId);
}
// PC 0x004EA730 GOG 0x004EC530
void CServerExoAppInternal::MovePlayerToArea(void* pData)
{
    Vector vSafePosition;
    Vector vPosition;
    CSWSPlayer* pPlayer = (CSWSPlayer*)pData;

    if (pPlayer->GetGameObject() != NULL)
    {
        CSWSCreature* pCreature = pPlayer->GetGameObject()->AsSWSCreature();
        if (m_sMoveToModuleStartWaypoint[0])
        {
            unsigned long oidWaypoint = GetModule()->GetWaypoint(m_sMoveToModuleStartWaypoint);
            if (oidWaypoint != OBJECT_INVALID)
            {
                CSWSWaypoint* pWaypoint = g_pAppManager->GetServerExoApp()->GetWaypointByGameObjectID(oidWaypoint);
                if (pWaypoint != NULL)
                {
                    pCreature->m_vDisappearPosition = pWaypoint->m_vPosition;
                    pCreature->SetOrientation(pWaypoint->m_vOrientation);
                }
            }
            m_sMoveToModuleStartWaypoint = "";
        }
        vPosition = pCreature->m_vDisappearPosition;
        CSWSArea* pArea = GetModule()->GetArea();
        pCreature->m_pPathfindInformation->m_oidPathfindCreature = pCreature->m_nId;
        if (pArea != NULL)
        {
            CPathfindInformation* pPathfind = pCreature->m_pPathfindInformation;
            if (!pArea->ComputeSafeLocation(vPosition, 20.0f,
                                            pPathfind, 1,
                                            &vSafePosition))
            {
                vSafePosition = vPosition;
            }
            pCreature->UpdateSubareasOnJumpPosition(vSafePosition, pArea->m_nId,
                                                   !m_bLoadFromSaveGame);
            pCreature->AddToArea(pArea, vSafePosition.m_fX, vSafePosition.m_fY,
                                vSafePosition.m_fZ, 0, 0);
        }
        pCreature->UpdateVisibleList(0);
        m_partyTable.PositionParty();
        m_bLoadFromSaveGame = 0;
    }
}

void CServerExoAppInternal::OnCDChange()
{
}

void CServerExoAppInternal::OnExit()
{
}

void CServerExoAppInternal::OnGainFocus()
{
}

void CServerExoAppInternal::OnLostFocus()
{
}

void CServerExoAppInternal::OnVideoChange()
{
}
// PC 0x004EC590 GOG 0x004EB080
CSWSPlayer* CServerExoAppInternal::GetClientObjectByObjectId(
    unsigned long oidObject)
{
    CExoLinkedListNode* pNode;
    CSWSPlayer* pPlayer;

    if (oidObject == OBJECT_INVALID)
    {
        return NULL;
    }
    pNode = m_pPlayerList->GetHeadPos();
    while (pNode != NULL)
    {
        pPlayer = (CSWSPlayer*)m_pPlayerList->GetAtPos(pNode);
        if (pPlayer != NULL && pPlayer->GetGameObject() != NULL &&
            pPlayer->GetGameObject()->m_nId == oidObject)
        {
            return pPlayer;
        }
        m_pPlayerList->GetNext(pNode);
    }
    return NULL;
}
// PC 0x004EC620 GOG 0x004EB110
CSWSPlayer* CServerExoAppInternal::GetClientObjectByPlayerId(
    unsigned long nPlayerId, unsigned char nGroup)
{
    CSWSPlayer* pPlayer;

    if (nGroup == 1 || nGroup == 0)
    {
        for (CExoLinkedListNode* pNode = m_pPlayerList->GetHeadPos(); pNode != NULL;
             m_pPlayerList->GetNext(pNode))
        {
            pPlayer = (CSWSPlayer*)m_pPlayerList->GetAtPos(pNode);
            if (pPlayer->GetPlayerId() == nPlayerId)
            {
                return pPlayer;
            }
        }
    }
    if (nGroup == 2 || nGroup == 0)
    {
        for (CExoLinkedListNode* pNode = m_pSysAdminList->GetHeadPos(); pNode != NULL;
             m_pSysAdminList->GetNext(pNode))
        {
            pPlayer = (CSWSPlayer*)m_pSysAdminList->GetAtPos(pNode);
            if (pPlayer->GetPlayerId() == nPlayerId)
            {
                return pPlayer;
            }
        }
    }
    return NULL;
}
#ifndef _XBOX
int CServerExoAppInternal::SaveGame(unsigned long nSlot,  // PC 0x004EC6E0 GOG 0x004EE1B0
                                    const CExoString& sName,
                                    CSWSPlayer* pPlayer, int bAutoSave)
{
    CExoString sDirectory;
    CExoString sSaveFile;
    unsigned __int64 nFreeBytes;

    g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_PAUSE_ALL);
    g_pExoResMan->CreateDirectory2("SAVES:");
    sDirectory.Format("SAVES:%06d - %s", nSlot, sName.CStr());

    int nResult = g_pExoResMan->GetFreeDiskSpace("SAVES:", &nFreeBytes);
    if (nResult == 1)
    {
        unsigned long nSaveGameSize = m_nEstimatedSaveSize;
        if (nSaveGameSize >
            nFreeBytes + (unsigned long)g_pExoBase->GetDirectorySize(sDirectory))
        {
            CSWSMessage* pMessage = m_pSWSMessage;
            pMessage->SendServerToPlayerSaveLoad_Status(
                pPlayer, CSWSMessage::SAVELOAD_STATUS_SAVE_FAILED,
                CSWSMessage::SAVELOAD_FAILURE_DISK_SPACE);
            return 0;
        }
    }

    if (g_pExoResMan->CreateDirectory2(sDirectory) == 0 &&
        g_pExoResMan->CleanDirectory(sDirectory, 0, 0) == 0)
    {
        return 0;
    }

    sSaveFile = sDirectory + "\\" + "SAVEGAME";

    g_pAppManager->m_pReentrantServerStats->StartStallEvent(
        CSWReentrantServerStats::OPERATION_SAVE_GAME);
    CSWReentrantServerStats* pDirectoryStats = g_pAppManager->m_pReentrantServerStats;
    pDirectoryStats->m_sUnknown24 = sDirectory;
    CSWReentrantServerStats* pStats = g_pAppManager->m_pReentrantServerStats;
    pStats->m_sUnknown32 = sSaveFile;
    CSWReentrantServerStats* pCompletedStats = g_pAppManager->m_pReentrantServerStats;
    pCompletedStats->m_nProgress = 0;
    CSWReentrantServerStats* pProgressStats = g_pAppManager->m_pReentrantServerStats;
    pProgressStats->m_nProgressTotal = 0;
    SendStartStallEvent(CSWSMessage::LOADBAR_STALL_EVENT_SAVE_GAME);

    sDirectory = g_pExoBase->m_pExoAliasList->GetAliasPath("SAVES", 0);
    if (sDirectory != "")
    {
        CAurCamera* pModuleCamera =
            g_pAppManager->GetClientExoApp()->GetModule()->GetCamera();
        CSWCameraOnAStick* pCamera = (CSWCameraOnAStick*)
            pModuleCamera->GetBehavior(AUR_BEHAVIOR_CAMERA_ONASTICK);
        if (pCamera != NULL)
        {
            pCamera->SetCameraForScreenShot();
        }
        sSaveFile.Format("%s%06d - %s%sScreen", sDirectory.CStr(),
                         nSlot, sName.CStr(), "\\");
        g_pAppManager->DoSaveGameScreenShot(sSaveFile);
    }

    if (nSlot != 0)
    {
        g_pAppManager->GetClientExoApp()->GetSWGuiManager()->Draw(1.0f / 30.0f);
        UpdateScreen(1.0f / 30.0f, 0, 0);
    }
    m_nAutoSaveElapsedTime = 0;
    m_bAutoSaveTimerExpired = 0;
    return 1;
}
// PC 0x004ECF10 GOG 0x004EE570
int CServerExoAppInternal::LoadCharacterFinish(CSWSPlayer* pPlayer, int bSuccess)
{
    CSWSModule* pModule = g_pAppManager->m_pServerExoApp->GetModule();
    CScriptEvent* pEnterEvent = new CScriptEvent;
    CScriptEvent* pDeathEvent = NULL;
    int nHead;

    char nAreaLoadState = pPlayer->m_nAreaLoadState;
    if (nAreaLoadState != 1)
    {
        pPlayer->m_nAreaLoadState = CSWSPlayer::AREA_LOAD_PENDING;
        m_pSWSMessage->SendServerToPlayerLogin_Confirm(pPlayer->GetPlayerId());
        m_pSWSMessage->SendServerToPlayerModule_Info(pPlayer->GetPlayerId());
    }
    if (bSuccess != 0 && pPlayer->m_nModulePlayerIndex != -1)
    {
        CSWSObject* pObject = pPlayer->GetGameObject();
        if (pObject != NULL)
            delete pObject;
        pPlayer->LoadCharacterFromIFO(pPlayer->m_nModulePlayerIndex, 1, 1);
    }

    pEnterEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_CLIENT_ENTER;
    if (pPlayer->GetGameObject() != NULL)
    {
        if (pPlayer->GetGameObject()->AsSWSCreature()->GetDead() == 1 ||
            pPlayer->GetGameObject()->AsSWSCreature()->GetDeadTemp() == 1)
        {
            pDeathEvent = new CScriptEvent;
            pDeathEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_DEATH;
        }
        pEnterEvent->SetObjectID(0, pPlayer->GetGameObject()->GetId());

        CSWSMessage* pMessage =
            g_pAppManager->m_pServerExoApp->GetSWSMessage();
        CExoLinkedList<CSWSClient>* pPlayers =
            g_pAppManager->m_pServerExoApp->GetPlayerList();
        CSWSCreature* pCreature =
            pPlayer->GetGameObject()->AsSWSCreature();
        CExoLinkedListNode* pNode = pPlayers->GetHeadPos();
        while (pNode != NULL)
        {
            CSWSPlayer* pOtherPlayer = (CSWSPlayer*)pPlayers->GetAtPos(pNode);
            if (pOtherPlayer != NULL &&
                pOtherPlayer->m_nPlayModuleCharacterListState != 0 &&
                pMessage != NULL)
                pMessage->SendServerToPlayerPlayModuleCharacterListResponse(
                    pOtherPlayer->GetPlayerId(), pCreature->GetId(), 1);
            pPlayers->GetNext(pNode);
        }
    }
    else
    {
        pEnterEvent->SetObjectID(0, OBJECT_INVALID);
    }

    if (pModule != NULL)
    {
        g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
            0, 0, pPlayer->GetPlayerId(), pModule->GetId(),
            CServerAIMaster::EVENT_SIGNAL_EVENT, pEnterEvent);
        if (pDeathEvent != NULL)
            g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                0, 0, pPlayer->GetGameObject()->GetId(), pModule->GetId(),
                CServerAIMaster::EVENT_SIGNAL_EVENT, pDeathEvent);

        unsigned long oidPC = pPlayer->m_oidPCObject;
        CSWSCreature* pCreature =
            g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(oidPC);
        if (pCreature != NULL)
        {
            if (&pCreature->m_cAppearanceInfo != NULL)
            {
                g_pRules->Get2DArrays()->GetAppearance()->GetINTEntry(
                    pCreature->m_cAppearanceInfo.m_nAppearance, "NORMALHEAD", &nHead);
                pModule->GetArea()->EnforceNoPCHeadCloning(nHead);
            }
        }
    }
    return 1;
}
#endif
// PC 0x004ECAA0 GOG 0x004EB1D0
int CServerExoAppInternal::StorePlayerCharacters()
{
    CExoLinkedList<CSWSClient>* pPlayerList = m_pPlayerList;
    CResGFF cGFF;
    CResStruct stPlayer;
    CResStruct stTop;
    CResList lstPlayers;
    CExoLinkedListNode* pNode;
    CSWSPlayer* pPlayer;
    CSWSCreature* pCreature;
    int nStored = 0;
    bool bFailed;

    {
        CExoString sVersion("V2.0");
        CExoString sFileType("IFO ");

        bFailed = cGFF.CreateGFFFile(&stTop, sFileType, sVersion) == 0;
    }
    if (bFailed)
    {
        return 0;
    }
    cGFF.AddList(&lstPlayers, &stTop, "Mod_PlayerList");
    pNode = pPlayerList->GetHeadPos();
    if (pNode != NULL)
    {
        do
        {
            pPlayer = pPlayerList->GetAtPos(pNode)->AsSWSPlayer();
            if (pPlayer != NULL)
            {
                pPlayer->m_nPlayerIndex = -1;
                pCreature =
                    g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(
                        pPlayer->m_oidPCObject);
                if (pCreature != NULL)
                {
                    cGFF.AddListElement(&stPlayer, &lstPlayers, CSWSModule::GFF_STRUCT_PLAYER);
                    ((CSWSObject*)pCreature)->ClearAllActions(1);
                    pCreature->SaveCreature(&cGFF, &stPlayer);
                    pPlayer->m_nPlayerIndex = nStored++;
                }
            }
            pPlayerList->GetNext(pNode);
        } while (pNode != NULL);
        if (nStored > 0)
        {
            CExoString sFileName;
            CExoString sPath;

            sFileName = "pifo";
            sPath = "TEMP:";
            sPath = sPath + sFileName;
            {
                CExoString sFile(sPath.CStr());
                cGFF.WriteGFFFile(sFile, 0x7DE);
            }
        }
    }
    m_partyTable.UpdateMembers(1);
    m_partyTable.UpdateInventory(1);
    return nStored > 0;
}

int CServerExoAppInternal::ValidateCreateServerCharacter(CSWSPlayer* pPlayer, void* pData,
                                                          unsigned long nSize)
{
    CExoString sTempDirectory;
    CExoString sVaultDirectory;

    CExoFile* pFile = new CExoFile("TEMP:temp_char", RESTYPE_BIC, "wb");
    if (!pFile->FileOpened())
    {
        delete pFile;
        return 0;
    }
    pFile->Write(pData, 1, nSize);
    delete pFile;

    sTempDirectory = "TEMP:";
    g_pExoResMan->AddResourceDirectory(sTempDirectory);
    CResGFF* pGFF = new CResGFF(RESTYPE_BIC, "BIC ", CResRef("temp_char"));
    CResStruct stTop;
    CResList lstClasses;
    CResStruct stClass;
    int bSuccess;
    int bValid = 0;

    pGFF->GetTopLevelStruct(&stTop);
    if (pGFF->ReadFieldDWORD(&stTop, "Experience", bSuccess, 0) == 0 &&
        pGFF->ReadFieldBYTE(&stTop, "Invulnerable", bSuccess, 0) == 0 &&
        pGFF->GetList(&lstClasses, &stTop, "ClassList") &&
        pGFF->GetListCount(&lstClasses) == 1 &&
        pGFF->GetListElement(&stClass, &lstClasses, 0) &&
        pGFF->ReadFieldSHORT(&stClass, "ClassLevel", bSuccess, 0) == 1)
    {
        bValid = 1;
    }
    delete pGFF;
    if (bValid)
    {
        pPlayer->LoadLocalCharacter();
    }
    g_pExoResMan->RemoveResourceDirectory(sTempDirectory);
    g_pExoResMan->CleanDirectory(sTempDirectory, 0, 0);
    if (!bValid)
    {
        return 0;
    }

    sVaultDirectory.Format("SERVERVAULT:%s", pPlayer->GetPlayerName().CStr());
    g_pExoResMan->CreateDirectory2(sVaultDirectory);
    return pPlayer->SaveServerCharacter();
}
// PC 0x004ECD70 GOG 0x004EB4A0
int CServerExoAppInternal::StoreCurrentModule()
{
    CSWSModule* pModule;
    int nColon;
    int bInclude;

    pModule = GetModule();
    if (pModule != NULL)
    {
        bInclude = IncludeModuleInSave(pModule->GetModuleResourceName());
        if (bInclude != 0)
        {
            CExoString sPath("GAMEINPROGRESS:");
            CExoString sModuleName(pModule->m_sModuleName);

            nColon = sModuleName.Find(':', 0);
            if (nColon == -1)
            {
                sModuleName = sPath + sModuleName;
            }
            else
            {
                sModuleName = sPath + sModuleName.Right(
                                          sModuleName.GetLength() - nColon - 1);
            }
            pModule->SaveModuleStart(sPath, sModuleName);
            pModule->SaveModuleInProgress();
            pModule->SaveModuleFinish(sPath, sModuleName);
        }
    }
    return 1;
}
int CServerExoAppInternal::LoadPrimaryPlayer(CSWSPlayer* pPlayer)  // PC 0x004ED220 GOG 0x004EE880
{
    CSWSModule* pModule;
    int nPrimaryPlayerIndex;

    pModule = g_pAppManager->m_pServerExoApp->GetModule();
    nPrimaryPlayerIndex = pModule->GetPrimaryPlayerIndex();
    if (nPrimaryPlayerIndex == -1)
    {
        return 0;
    }
    pPlayer->m_nModulePlayerIndex = nPrimaryPlayerIndex;
    LoadCharacterFinish(pPlayer, 1);
    return 1;
}
// PC 0x004ED260 GOG 0x004EB640
int CServerExoAppInternal::ExportAllPlayers()
{
    CExoLinkedListNode* pNode;
    CSWSPlayer* pPlayer;
    char nState;

    pNode = m_pPlayerList->GetHeadPos();
    while (pNode != NULL)
    {
        pPlayer = (CSWSPlayer*)m_pPlayerList->GetAtPos(pNode);
        nState = pPlayer->m_nCharacterType;
        if (nState != 1)
        {
            if ((unsigned char)nState > 2 && (unsigned char)nState <= 4)
            {
                pPlayer->SaveServerCharacter();
            }
        }
        else
        {
            m_pSWSMessage->SendServerToPlayerModule_ExportReply(pPlayer);
        }
        m_pPlayerList->GetNext(pNode);
    }
    return 1;
}
#ifndef _XBOX
void CServerExoAppInternal::StallEventSaveGame()  // PC 0x004ED2D0 GOG 0x004EB750
{

    CExoString sModuleName;
    CExoString sAreaName;
    CExoString sSaveInfoFile;
    CExoString sPortraitLabel;
    CSWParty* pParty;

    sModuleName = GetModule()->GetModuleName();
    int nAliasSeparator = sModuleName.Find(':', 0);
    if (nAliasSeparator != -1)
    {
        sModuleName = sModuleName.Right(sModuleName.GetLength() - nAliasSeparator - 1);
    }

    g_pAppManager->GetClientExoApp()->SetLoadScreenByModuleName(sModuleName);
    g_pAppManager->GetClientExoApp()->SetLoadScreenHint(
        g_pAppManager->GetClientExoApp()->GetNextLoadScreenHintSTRREF(sModuleName, 0));
    g_pAppManager->GetClientExoApp()->ShowLoadScreen(0, 1, 1);
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(0, 0);
    g_pAppManager->GetClientExoApp()->GetSWGuiManager()->Draw(1.0f / 30.0f);
    UpdateScreen(1.0f / 30.0f, 0, 0);
    unsigned __int64 nStartTime = g_pExoBase->m_pExoTimers->GetLowResolutionTimer();

    CSWReentrantServerStats* pStats = g_pAppManager->m_pReentrantServerStats;
    CExoString sDirectory;
    CExoString sSaveFile;
    sDirectory = pStats->m_sUnknown24;
    sSaveFile = pStats->m_sUnknown32;

    g_pAppManager->GetServerExoApp()->GetSWSMessage()
        ->SendServerToPlayerModule_SaveGameStatus(CSWSMessage::SAVEGAME_STATUS_STARTED);
    StoreCurrentModule();
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(5, 0);
    UpdateScreen(1.0f / 30.0f, 0, 0);
    m_partyTable.Save(sDirectory, 1);
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(10, 0);
    UpdateScreen(1.0f / 30.0f, 0, 0);
    m_globalVariableTable.Save(sDirectory);
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(15, 0);
    UpdateScreen(1.0f / 30.0f, 0, 0);

    CSWSCreature* pCreature = g_pAppManager->GetServerExoApp()->GetPlayerCreature();
    CSWSPlayer* pPlayer = GetClientObjectByObjectId(pCreature->GetId());
    if (g_bSavePlayerBIC)
    {
        GetModule()->SavePrimaryPlayerInfo(pCreature, sDirectory);
    }
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(20, 0);
    UpdateScreen(1.0f / 30.0f, 0, 0);

    sSaveInfoFile = sDirectory + "\\" + "savenfo";
    CResGFF* pGFF = new CResGFF;
    CResStruct stSaveInfo;
    pGFF->CreateGFFFile(&stSaveInfo, "NFO ", "V2.0");
    int nLanguage = pPlayer->m_nLanguage;
    CSWSArea* pArea = pCreature->GetArea();
    pArea->m_lsName.GetString(nLanguage, &sAreaName, 0);
    pGFF->WriteFieldCExoString(&stSaveInfo, sAreaName, "AREANAME");
    pGFF->WriteFieldCExoString(&stSaveInfo, sModuleName, "LASTMODULE");
    pGFF->WriteFieldDWORD(&stSaveInfo, m_partyTable.GetTotalPlayedSeconds(), "TIMEPLAYED");
#ifndef _XBOX
    int bCheatUsed = m_partyTable.m_bCheatUsed;
    pGFF->WriteFieldBYTE(&stSaveInfo, bCheatUsed, "CHEATUSED");
    pGFF->WriteFieldCExoString(&stSaveInfo, m_sSaveGameName, "SAVEGAMENAME");
#endif
    {
        SYSTEMTIME stLocalTime;
        FILETIME ftTimeStamp;
        GetLocalTime(&stLocalTime);
        SystemTimeToFileTime(&stLocalTime, &ftTimeStamp);
        unsigned __int64 nTimeStamp = ftTimeStamp.dwHighDateTime;
        nTimeStamp <<= 32;
        nTimeStamp |= ftTimeStamp.dwLowDateTime;
        pGFF->WriteFieldDWORD64(&stSaveInfo, nTimeStamp, "TIMESTAMP");
    }
    pGFF->WriteFieldCExoString(&stSaveInfo, m_partyTable.GetPCName(), "PCNAME");
    int nSaveNumber = m_partyTable.m_n248;
    pGFF->WriteFieldDWORD(&stSaveInfo, nSaveNumber, "SAVENUMBER");
    pGFF->WriteFieldBYTE(&stSaveInfo,
        g_pAppManager->GetClientExoApp()->GetGamePlayHint(), "GAMEPLAYHINT");
    pGFF->WriteFieldBYTE(&stSaveInfo,
        g_pAppManager->GetClientExoApp()->GetStoryHint(0), "STORYHINT0");
    pGFF->WriteFieldBYTE(&stSaveInfo,
        g_pAppManager->GetClientExoApp()->GetStoryHint(1), "STORYHINT1");
    pGFF->WriteFieldBYTE(&stSaveInfo,
        g_pAppManager->GetClientExoApp()->GetStoryHint(2), "STORYHINT2");
    pGFF->WriteFieldBYTE(&stSaveInfo,
        g_pAppManager->GetClientExoApp()->GetStoryHint(3), "STORYHINT3");
    pGFF->WriteFieldBYTE(&stSaveInfo,
        g_pAppManager->GetClientExoApp()->GetStoryHint(4), "STORYHINT4");
    pGFF->WriteFieldBYTE(&stSaveInfo,
        g_pAppManager->GetClientExoApp()->GetStoryHint(5), "STORYHINT5");
    pGFF->WriteFieldBYTE(&stSaveInfo,
        g_pAppManager->GetClientExoApp()->GetStoryHint(6), "STORYHINT6");
    pGFF->WriteFieldBYTE(&stSaveInfo,
        g_pAppManager->GetClientExoApp()->GetStoryHint(7), "STORYHINT7");
    pGFF->WriteFieldBYTE(&stSaveInfo,
        g_pAppManager->GetClientExoApp()->GetStoryHint(8), "STORYHINT8");
    pGFF->WriteFieldBYTE(&stSaveInfo,
        g_pAppManager->GetClientExoApp()->GetStoryHint(9), "STORYHINT9");

    pParty = g_pAppManager->GetClientExoApp()->GetSWParty();
    unsigned char nLiveContent = 0;
    CExoString sLiveContentLabel;
    for (int nContent = 1; nContent < CTlkTable::NUM_CONTENT_PKGS; ++nContent)
    {
        sLiveContentLabel.Format("LIVE%d", nContent);
        pGFF->WriteFieldCExoString(&stSaveInfo,
            g_pAppManager->GetClientExoApp()->GetLiveContentName(nContent),
            sLiveContentLabel.CStr());
        if (g_pExoBase->m_pExoAliasList->GetAliasPath(sLiveContentLabel, 0).GetLength() > 0)
        {
            nLiveContent |= 1 << (nContent - 1);
        }
    }
    pGFF->WriteFieldBYTE(&stSaveInfo, nLiveContent, "LIVECONTENT");
    for (int nMember = 0; nMember < pParty->m_nCharacters; ++nMember)
    {
        CSWSCreature* pServerCreature = pParty->GetCharacter(nMember)->GetServerCreature();
        if (pServerCreature != NULL)
        {
            sPortraitLabel.Format("%s%d", "PORTRAIT", nMember);
            CSWCCreature* pClientCreature = pServerCreature->GetClientCreature();
            if (pClientCreature != NULL)
            {
                pGFF->WriteFieldCResRef(&stSaveInfo, pClientCreature->GetPortrait(1),
                    sPortraitLabel.CStr());
            }
            else
            {
                pGFF->WriteFieldCResRef(&stSaveInfo, pServerCreature->GetPortrait(),
                    sPortraitLabel.CStr());
            }
        }
    }
    pGFF->WriteGFFFile(sSaveInfoFile.CStr(), 0);
    delete pGFF;
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(25, 0);
    UpdateScreen(1.0f / 30.0f, 0, 0);

    CERFFile* pERF = new CERFFile;
    pERF->Create(sSaveFile);
    pERF->SetVersion("MOD V1.0");
    pERF->WriteHeader();
    pERF->WriteStringTable();
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(30, 0);
    UpdateScreen(1.0f / 30.0f, 0, 0);
    pERF->ImportFiles("GAMEINPROGRESS:", (void*)-1, UpdateLoadScreen, 30, 60);
    pERF->Finish();
    pERF->Read(sSaveFile);
    pERF->ReadHeaderVariance(sSaveFile);
    delete pERF;
    unsigned __int64 nEndTime = g_pExoBase->m_pExoTimers->GetLowResolutionTimer();
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(100, 0);
    UpdateScreen(1.0f / 30.0f, 0, 0);
    g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_GENERIC);
#ifdef _XBOX
    g_pAppManager->GetClientExoApp()->HideLoadScreen(0);
#else
    g_pAppManager->GetClientExoApp()->HideLoadScreen(0, 1, 0);
#endif
    if (g_pAppManager->GetClientExoApp()->GetInGameGui()->m_pPartySelection == 0)
    {
        if (g_pAppManager->GetClientExoApp()->GetInGameGui()->m_nGlobalDialogState != 0)
        {
            g_pAppManager->GetClientExoApp()->GetInGameGui()->m_bHoldWorldFadeInForDialog = 1;
        }
        else
        {
            g_pAppManager->GetClientExoApp()->GetInGameGui()->StartGlobalFade(
                1, 0.5f, 1.0f, Vector(0.0f, 0.0f, 0.0f));
        }
    }
    g_pAppManager->GetClientExoApp()->SetCanSendDialog(1);
    g_pExoInput->EndAttemptedBind();
    g_pAppManager->GetClientExoApp()->GetInGameGui()->SetGlobalUnpauseableState(0);
    g_pAppManager->GetClientExoApp()->GetModule()->m_pInfo->StartSounds();
    if (pStats->m_bOperationEndPending != 1)
    {
        pStats->m_bOperationEndPending = 1;
        if (g_nClientServerMode == CLIENT_SERVER_MODE_SERVER)
        {
            pStats->m_bOperationEndPending = 0;
            pStats->m_bOperationActive = 0;
            pStats->m_nOperation = 0;
        }
        g_pAppManager->GetServerExoApp()->GetSWSMessage()
            ->SendServerToPlayerModule_SaveGameStatus(CSWSMessage::SAVEGAME_STATUS_COMPLETE);
        g_pAppManager->GetServerExoApp()->GetSWSMessage()
            ->SendServerToPlayerLoadBar_EndStallEvent(CSWSMessage::LOADBAR_STALL_EVENT_SAVE_GAME, 0);
    }
}
#endif
// PC 0x004EDE30 GOG 0x004EEBB0
int CServerExoAppInternal::RunModule()
{
    CExoString sModule;

    if (m_nServerMode == SERVER_MODE_LOADING)
    {
        m_nServerMode = SERVER_MODE_RUNNING;
        m_pSWSMessage->SendServerToPlayerServerStatus_Status(
            CNetLayer::PLAYERID_ALL_PLAYERS);
        m_pSWSMessage->SendServerToServerAdminServerStatus(
            CNetLayer::PLAYERID_ALL_SERVERADMINS);

        CExoLinkedListNode* pPosition = m_pPlayerList->GetHeadPos();
        while (pPosition != NULL)
        {
            CSWSPlayer* pPlayer =
                (CSWSPlayer*)m_pPlayerList->GetAtPos(pPosition);
            if (pPlayer->m_nAreaLoadState)
            {
                MovePlayerToArea(pPlayer);
            }
            m_pPlayerList->GetNext(pPosition);
        }

        if (GetModule()->m_bIsSaveGame && m_bDoingStartNewModule != 1)
        {
            pPosition = m_pPlayerList->GetHeadPos();
            LoadPrimaryPlayer(
                (CSWSPlayer*)m_pPlayerList->GetAtPos(pPosition));
            m_partyTable.CreateParty();
        }

        if (m_bDoingStartNewModule == 1)
        {
            SendExitingStartNewModuleMessage(1);
        }
        return 1;
    }
    return 0;
}
// PC 0x004F2220 GOG 0x004F1D60
int CServerExoAppInternal::UnloadModule()
{
    CGameObjectArray* pObjectArray = m_pObjectArray;
    CSWSModule* pModule = GetModule();
    CSWSMessage* pMessage = g_pAppManager->GetServerExoApp()->GetSWSMessage();

#ifndef _XBOX
    if (g_pAppManager->GetClientExoApp() != NULL &&
        g_pAppManager->GetClientExoApp()->GetSoundPausedByCombat())
    {
        g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_GENERIC);
    }
#endif
    SetPauseState(PAUSE_STATE_COMBAT, 0);
    SetPauseState(PAUSE_STATE_PAUSED, 0);

    delete m_pTimestopExclusionList;
    m_pTimestopExclusionList = new CExoLinkedList<unsigned long>;
    delete m_pPauseExclusionList;
    m_pPauseExclusionList = new CExoLinkedList<unsigned long>;

    CExoLinkedListNode* pPosition = m_pPlayerList->GetHeadPos();
    while (pPosition != NULL)
    {
        CSWSPlayer* pPlayer =
            m_pPlayerList->GetAtPos(pPosition)->AsSWSPlayer();
        if (pPlayer != NULL)
        {
            g_pAppManager->GetObjectTableManager(CAppManager::OBJECT_TABLE_SERVER)
                ->RemovePlayer(pPlayer->GetPlayerId());
            pMessage->SendServerToPlayerModule_DumpPlayer(pPlayer);
            CSWSObject* pObject = pPlayer->GetGameObject();
            if (pObject != NULL)
            {
                CSWSCreature* pCreature = pObject->AsSWSCreature();
                if (pCreature != NULL)
                {
                    if (pCreature->GetArea() != NULL)
                    {
                        pCreature->RemoveFromArea(0);
                    }
                    delete pCreature;
                    pPlayer->SetGameObject((CSWSObject*)NULL);
                }
            }
            pPlayer->ClearPlayerOnDestroyGame();
            pPlayer->m_nAreaLoadState = 0;
            pPlayer->m_n7C = 0;
            pPlayer->m_oidLastControlledObject = OBJECT_INVALID;
            m_pSWSMessage->SendServerToPlayerGameObjUpdate_ObjControl(
                pPlayer->GetPlayerId(), OBJECT_INVALID);
        }
        m_pPlayerList->GetNext(pPosition);
    }

    if (g_pAppManager->GetClientExoApp() != NULL &&
        g_pAppManager->GetClientExoApp()->GetSWParty() != NULL)
    {
        g_pAppManager->GetClientExoApp()->GetSWParty()->ClearParty();
    }
    if (&m_partyTable != NULL)
    {
        m_partyTable.ClearInventory();
    }
    g_pTlkTable->ClearCustomTokens();
    delete pModule;
    m_oidModule = OBJECT_INVALID;
    delete m_pFactionManager;
    m_pFactionManager = NULL;
    m_pFactionManager = new CFactionManager;
    delete pObjectArray;
    m_pObjectArray = new CGameObjectArray(1);
    m_pServerAIMaster->ClearEventQueue();
    g_pExoResMan->NukeDirectory("CURRENTGAME:", 1, 1);
    return 1;
}
// PC 0x004EDFA0 GOG 0x004EED20
void CServerExoAppInternal::StartServices(int nServices)
{
    CExoString sTimestamp;

    g_pExoBase->m_pExoDebug->GetCurrentTimestamp(sTimestamp);

    EXOLOG("[%s]Server Starting Up\n", sTimestamp.CStr());

    m_pFactionManager = new CFactionManager;
    m_pWorldTimer = new CWorldTimer;
    m_pTimestopTimer = new CWorldTimer;
    m_pPauseTimer = new CWorldTimer;
    m_pObjectArray = new CGameObjectArray(1);
    if (g_nClientServerMode == CLIENT_SERVER_MODE_SERVER)
    {
        g_pExoResMan = new CExoResMan;
    }
    m_pNetLayer = new CNetLayer;
    if (m_pServerInfo == NULL)
    {
        m_pServerInfo = new CServerInfo;
    }
    m_pSWSMessage = new CSWSMessage;
    m_pServerAIMaster = new CServerAIMaster;
    m_pServerAIMaster->SetExoAppInternal(this);

    if (g_nClientServerMode == CLIENT_SERVER_MODE_SERVER)
    {
        g_pExoResMan->AddResourceDirectory("OVERRIDE:");
        g_pExoResMan->AddResourceDirectory("ERRORTEX:");
        g_pExoResMan->AddFixedKeyTableFile("HD0:chitin");
        g_pExoResMan->AddResourceDirectory("SERVERVAULT:");
#ifdef _XBOX
        CExoIni cIni;
        CExoString sPatchFile;
        CExoString sKey;
        CExoString sPatchPath;
        int nPatchFile;

        sKey.Format("%s%03i", "PatchFile", 0);
        nPatchFile = 1;
        while (cIni.ReadIniEntry(sPatchFile, "d:\\nwnpatch.ini", "Patch", sKey))
        {
            sPatchPath = "PATCH:";
            sPatchPath = sPatchPath + sPatchFile;
            g_pExoResMan->AddEncapsulatedResourceFile(sPatchPath, 2);
            sKey.Format("%s%03i", "PatchFile", nPatchFile++);
        }
#endif
    }
#ifndef _XBOX
    g_pExoResMan->RemoveFile("HD0:TEMP", 86);
#endif
    g_pExoResMan->CreateDirectory2("TEMP:");
    g_pExoResMan->CreateDirectory2("GAMEINPROGRESS:");
    if (g_pTlkTable == NULL)
    {
        g_pTlkTable = new CTlkTable;
        g_pTlkTable->OpenFile("HD0:dialog");
    }
    g_pVirtualMachine = new CVirtualMachine;
    m_pVirtualMachineCommands = new CSWVirtualMachineCommands;
    g_pVirtualMachine->SetCommandImplementer(m_pVirtualMachineCommands);
    if (g_nClientServerMode != CLIENT_SERVER_MODE_SERVER)
    {
        g_pExoBase->m_pExoDebug->OpenLogFiles("swserver");
    }

    m_nShutdownMessageDelay = SHUTDOWN_REPLY_TIMEOUT;
    InitializeNetLayer();
    GetServerInfoFromIniFile();
    if (g_nClientServerMode == CLIENT_SERVER_MODE_SERVER)
    {
        g_pRules = new CSWSRules;
    }
    else
    {
        DumpServerOptionsToLog();
    }

    m_nLastMainLoopTime = 0;
    m_nMainLoopTimeBudget = 0;
    m_nLastTempDeathUpdate = g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000;
    m_nTempDeathCheckElapsed = 0;
    m_nTempDeathRecoveryElapsed = 0;
    m_nAutoSaveElapsedTime = 0;
    m_bAutoSaveTimerExpired = 0;
}
// PC 0x004EA900 GOG 0x004EC6E0
void CServerExoAppInternal::UpdateClientGameObjectsForPlayer(
    CSWSPlayer* pPlayer, int bFlag, unsigned __int64 nTime)
{
    unsigned __int64 nUpdateTime = nTime;
    CSWSCreature* pCreature;
    if (bFlag == 1 && nTime == 0)
    {
        nUpdateTime = GetHighResolutionTimer();
    }

    if (pPlayer->m_nAreaLoadState == CSWSPlayer::AREA_LOAD_COMPLETE ||
        (pPlayer->m_nAreaLoadState == CSWSPlayer::AREA_LOAD_PENDING &&
         pPlayer->m_n7C == 1 && pPlayer->GetGameObject() != NULL &&
         (pCreature = pPlayer->GetGameObject()->AsSWSCreature()) != NULL &&
         pCreature->m_oidDisappearTarget != OBJECT_INVALID))
    {
        if (bFlag == 1 || nTime - *(unsigned __int64*)pPlayer->m_anLastGameObjectUpdateTime >=
                          CSWSPlayer::GAME_OBJECT_UPDATE_INTERVAL)
        {
            m_pSWSMessage->SendServerToPlayerGameObjUpdate(pPlayer);
            if (pPlayer->GetGameObject() != NULL)
            {
                unsigned long oidObject = pPlayer->GetGameObject()->m_nId;
                if (pPlayer->m_oidLastControlledObject != oidObject &&
                    pPlayer->GetGameObject()->GetArea() != NULL)
                {
                    m_pSWSMessage->SendServerToPlayerGameObjUpdate_ObjControl(
                        pPlayer->m_nPlayerId, oidObject);
                    pPlayer->m_oidLastControlledObject = oidObject;
                    *(unsigned __int64*)pPlayer->m_anLastGameObjectUpdateTime = nUpdateTime;
                }
            }
            *(unsigned __int64*)pPlayer->m_anLastGameObjectUpdateTime = nUpdateTime;
        }
        if (pPlayer->GetGameObject() != NULL &&
            pPlayer->GetGameObject()->AsSWSCreature()->GetArea() == NULL &&
            pPlayer->m_oidLastControlledObject != OBJECT_INVALID)
        {
            pPlayer->m_oidLastControlledObject = OBJECT_INVALID;
            m_pSWSMessage->SendServerToPlayerGameObjUpdate_ObjControl(
                pPlayer->m_nPlayerId, OBJECT_INVALID);
            *(unsigned __int64*)pPlayer->m_anLastGameObjectUpdateTime = nUpdateTime;
        }
    }
}
// PC 0x004EAA70 GOG 0x004EC850
int CServerExoAppInternal::ContinueMessageProcessing()
{
    return GetReentrantServerStats()->m_bOperationActive == 1;
}

// PC 0x004EAA90 GOG 0x004E9D90
CSWSItem* CServerExoAppInternal::GetItemByGameObjectID(unsigned long oidObject)
{
    CGameObject* pObject = NULL;

    if (m_pObjectArray->GetGameObject(oidObject, &pObject) ==
            CGameObjectArray::SUCCESS &&
        pObject != NULL &&
        pObject->m_nObjectType == CGameObject::OBJECT_TYPE_ITEM)
    {
        return ((CSWSObject*)pObject)->AsSWSItem();
    }
    return NULL;
}
// PC 0x004EAAE0 GOG 0x004E9DE0
CSWSCreature* CServerExoAppInternal::GetCreatureByGameObjectID(
    unsigned long oidObject)
{
    CGameObject* pObject = NULL;

    if (m_pObjectArray->GetGameObject(oidObject, &pObject) ==
            CGameObjectArray::SUCCESS &&
        pObject != NULL &&
        pObject->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
    {
        return ((CSWSObject*)pObject)->AsSWSCreature();
    }
    return NULL;
}
// PC 0x004EAB30 GOG 0x004E9E30
CSWSArea* CServerExoAppInternal::GetAreaByGameObjectID(unsigned long oidArea)
{
    unsigned long oidCachedArea = oidArea;
    CGameObject* pObject;

    if (m_oidAreaCache != oidArea)
    {
        m_pAreaCache = NULL;
        m_oidAreaCache = OBJECT_INVALID;
        pObject = NULL;
        if (m_pObjectArray->GetGameObject(oidArea, &pObject) ==
            CGameObjectArray::SUCCESS)
        {
            if (pObject != NULL &&
                pObject->m_nObjectType == CGameObject::OBJECT_TYPE_AREA)
            {
                m_oidAreaCache = oidCachedArea;
                m_pAreaCache = pObject->AsSWSArea();
            }
        }
    }
    return m_pAreaCache;
}
// PC 0x004EABB0 GOG 0x004E9EB0
void CServerExoAppInternal::ClearAreaCache()
{
    m_pAreaCache = NULL;
    m_oidAreaCache = OBJECT_INVALID;
}
// PC 0x004EABD0 GOG 0x004E9ED0
CSWSTrigger* CServerExoAppInternal::GetTriggerByGameObjectID(
    unsigned long oidObject)
{
    CGameObject* pObject = NULL;

    if (m_pObjectArray->GetGameObject(oidObject, &pObject) ==
            CGameObjectArray::SUCCESS &&
        pObject != NULL &&
        pObject->m_nObjectType == CGameObject::OBJECT_TYPE_TRIGGER)
    {
        return ((CSWSObject*)pObject)->AsSWSTrigger();
    }
    return NULL;
}
// PC 0x004EAC20 GOG 0x004E9F20
CSWSPlaceable* CServerExoAppInternal::GetPlaceableByGameObjectID(
    unsigned long oidObject)
{
    CGameObject* pObject = NULL;

    if (m_pObjectArray->GetGameObject(oidObject, &pObject) ==
            CGameObjectArray::SUCCESS &&
        pObject != NULL &&
        pObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
    {
        return ((CSWSObject*)pObject)->AsSWSPlaceable();
    }
    return NULL;
}
// PC 0x004EAC70 GOG 0x004E9F70
CSWSDoor* CServerExoAppInternal::GetDoorByGameObjectID(unsigned long oidObject)
{
    CGameObject* pObject = NULL;

    if (m_pObjectArray->GetGameObject(oidObject, &pObject) ==
            CGameObjectArray::SUCCESS &&
        pObject != NULL &&
        pObject->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR)
    {
        return ((CSWSObject*)pObject)->AsSWSDoor();
    }
    return NULL;
}
// PC 0x004EACC0 GOG 0x004E9FC0
CSWSAreaOfEffectObject* CServerExoAppInternal::GetAreaOfEffectByGameObjectID(
    unsigned long oidObject)
{
    CGameObject* pObject = NULL;

    if (m_pObjectArray->GetGameObject(oidObject, &pObject) ==
            CGameObjectArray::SUCCESS &&
        pObject != NULL &&
        pObject->m_nObjectType == CGameObject::OBJECT_TYPE_AREAOFEFFECTOBJECT)
    {
        return ((CSWSObject*)pObject)->AsSWSAreaOfEffectObject();
    }
    return NULL;
}
// PC 0x004EAD10 GOG 0x004EA010
CSWSWaypoint* CServerExoAppInternal::GetWaypointByGameObjectID(
    unsigned long oidObject)
{
    CGameObject* pObject = NULL;

    if (m_pObjectArray->GetGameObject(oidObject, &pObject) ==
            CGameObjectArray::SUCCESS &&
        pObject != NULL &&
        pObject->m_nObjectType == CGameObject::OBJECT_TYPE_WAYPOINT)
    {
        return ((CSWSObject*)pObject)->AsSWSWaypoint();
    }
    return NULL;
}
// PC 0x004EAD60 GOG 0x004EA060
CSWSEncounter* CServerExoAppInternal::GetEncounterByGameObjectID(
    unsigned long oidObject)
{
    CGameObject* pObject = NULL;

    if (m_pObjectArray->GetGameObject(oidObject, &pObject) ==
            CGameObjectArray::SUCCESS &&
        pObject != NULL &&
        pObject->m_nObjectType == CGameObject::OBJECT_TYPE_ENCOUNTER)
    {
        return ((CSWSObject*)pObject)->AsSWSEncounter();
    }
    return NULL;
}
// PC 0x004EADB0 GOG 0x004EA0B0
CSWSSoundObject* CServerExoAppInternal::GetSoundObjectByGameObjectID(
    unsigned long oidObject)
{
    CGameObject* pObject = NULL;

    if (m_pObjectArray->GetGameObject(oidObject, &pObject) ==
            CGameObjectArray::SUCCESS &&
        pObject != NULL &&
        pObject->m_nObjectType == CGameObject::OBJECT_TYPE_SOUND)
    {
        return ((CSWSObject*)pObject)->AsSWSSoundObject();
    }
    return NULL;
}
// PC 0x004EAE00 GOG 0x004EA100
CSWSStore* CServerExoAppInternal::GetStoreByGameObjectID(
    unsigned long oidObject)
{
    CGameObject* pObject = NULL;

    if (m_pObjectArray->GetGameObject(oidObject, &pObject) ==
            CGameObjectArray::SUCCESS &&
        pObject != NULL &&
        pObject->m_nObjectType == CGameObject::OBJECT_TYPE_STORE)
    {
        return ((CSWSObject*)pObject)->AsSWSStore();
    }
    return NULL;
}
// PC 0x004EAE50 GOG 0x004EA150
void CServerExoAppInternal::GetServerInfoFromIniFile()
{
#ifdef _XBOX

    CExoIni cIni;
    CExoString sSection;
    CExoString sValue;
    CExoString sEntry;
    int nMaxPlayers;
    int nDifficulty;
    int bFound;

    sSection.Format("Server Options");
    bFound = cIni.ReadIniEntry(sValue, "d:\\swplayer.ini", sSection, "Max Players");
    if (bFound == 1)
    {
        nMaxPlayers = sValue.AsINT();
        if (nMaxPlayers < 1)
        {
            nMaxPlayers = CServerInfo::DEFAULT_MAX_PLAYERS;
        }
        else if (nMaxPlayers > CServerInfo::DEFAULT_MAX_PLAYERS)
        {
            nMaxPlayers = CServerInfo::DEFAULT_MAX_PLAYERS;
        }
    }
    else
    {
        nMaxPlayers = CServerInfo::DEFAULT_MAX_PLAYERS;
        sEntry.Format("%i", nMaxPlayers);
        cIni.WriteIniEntry(sEntry, "d:\\swplayer.ini", sSection, "Max Players");
    }
    bFound = cIni.ReadIniEntry(sValue, "d:\\swplayer.ini", sSection, "Server Name");
    if (bFound == 1)
    {
        m_pNetLayer->StartServerMode(sValue, nMaxPlayers);
    }
    else
    {
        m_pNetLayer->StartServerMode("Server", nMaxPlayers);
        cIni.WriteIniEntry("Server", "d:\\swplayer.ini", sSection, "Server Name");
    }
    m_pServerInfo->m_nMaxPlayers = nMaxPlayers;
    bFound = cIni.ReadIniEntry(sValue, "d:\\swplayer.ini", sSection, "Game Type");
    if (bFound == 1)
    {
        m_pServerInfo->m_nXboxUnmapped114 = sValue.AsINT();
    }
    else
    {
        sEntry.Format("%d", m_pServerInfo->m_nXboxUnmapped114);
        cIni.WriteIniEntry(sEntry, "d:\\swplayer.ini", sSection, "Game Type");
    }

    CExoFile cServerDesc("HD0:ServerDesc", RESTYPE_TXT, "rt");
    if (cServerDesc.Read(sValue, cServerDesc.GetSize()) > 0)
    {
        m_pServerInfo->m_sModuleName = sValue;
    }

    bFound = cIni.ReadIniEntry(sValue, "d:\\swplayer.ini", "Game Options", "Difficulty Level");
    if (bFound)
    {
        nDifficulty = sValue.AsINT();
        if (nDifficulty < 0 || nDifficulty > 5)
        {
            nDifficulty = CServerInfo::GAME_DIFFICULTY_NORMAL;
        }
    }
    else
    {
        nDifficulty = CServerInfo::GAME_DIFFICULTY_NORMAL;
        sValue.Format("%d", nDifficulty);
        cIni.WriteIniEntry(sValue, "d:\\swplayer.ini", "Game Options", "Difficulty Level");
    }
    m_pServerInfo->SetDifficultyLevel(nDifficulty);
    m_pServerInfo->SetDifficultyLevel(nDifficulty);
    m_pServerInfo->m_bPauseAndPlay = 1;
    m_pServerInfo->m_bAutoSaveAtIntervals = 0;
    m_pServerInfo->m_nAutoSaveInterval = 0;
    m_nAutoSaveElapsedTime = 0;
    m_pServerInfo->m_nPVP = 1;

    m_pServerInfo->m_nXboxUnmapped0D4 = 1;
    bFound = cIni.ReadIniEntry(sValue, "d:\\swplayer.ini", "Game Options", "Single Player Enforce Legal Characters");
    if (bFound)
    {
        m_pServerInfo->m_nXboxUnmapped0D4 = sValue.AsINT();
    }
    else
    {
        sEntry.Format("%d", m_pServerInfo->m_nXboxUnmapped0D4);
        cIni.WriteIniEntry(sEntry, "d:\\swplayer.ini", "Game Options", "Single Player Enforce Legal Characters");
    }
    m_pServerInfo->m_bItemLevelRestrictions = 1;
    bFound = cIni.ReadIniEntry(sValue, "d:\\swplayer.ini", "Game Options", "Single Player ItemLevelRestrictions");
    if (bFound)
    {
        m_pServerInfo->m_bItemLevelRestrictions = sValue.AsINT();
    }
    else
    {
        sEntry.Format("%d", m_pServerInfo->m_bItemLevelRestrictions);
        cIni.WriteIniEntry(sEntry, "d:\\swplayer.ini", "Game Options", "Single Player ItemLevelRestrictions");
    }
#else

    CExoIni cIni;
    CExoString sValue;
    CExoString sSection;
    CExoString sEntry;

    m_pNetLayer->StartServerMode("Server", CNetLayer::DEFAULT_SERVER_PORT);
    m_pServerInfo->m_nMaxPlayers = CServerInfo::DEFAULT_MAX_PLAYERS;
    m_pServerInfo->SetDifficultyLevel(CServerInfo::GAME_DIFFICULTY_NORMAL);
    m_pServerInfo->m_bPauseAndPlay = 1;
    m_pServerInfo->m_bAutoSaveAtIntervals = 0;
    m_pServerInfo->m_nAutoSaveInterval = 0;
    m_nAutoSaveElapsedTime = 0;
    m_pServerInfo->m_nPVP = 1;
    m_pServerInfo->m_bItemLevelRestrictions = 1;
#endif
}
// PC 0x004EAF90 GOG 0x004EA290
void CServerExoAppInternal::DumpServerOptionsToLog()
{
    CExoString sTimestamp;

    g_pExoBase->m_pExoDebug->GetCurrentTimestamp(sTimestamp);
    sTimestamp.CStr();
}
// PC 0x004EB000 GOG 0x004EC8E0
int CServerExoAppInternal::GetFactionOfObject(unsigned long oidObject,
                                             int* pnFaction)
{
    CGameObject* pObject = GetGameObject(oidObject);

    if (pObject != NULL)
    {
        if (pObject->AsSWSCreature() != NULL)
        {
            *pnFaction = pObject->AsSWSCreature()->m_pStats->m_nFactionId;
            return 1;
        }
        if (pObject->AsSWSDoor() != NULL)
        {
            *pnFaction = pObject->AsSWSDoor()->m_nFactionId;
            return 1;
        }
        if (pObject->AsSWSEncounter() != NULL)
        {
            *pnFaction = pObject->AsSWSEncounter()->m_nFaction;
            return 1;
        }
        if (pObject->AsSWSPlaceable() != NULL)
        {
            *pnFaction = pObject->AsSWSPlaceable()->m_nFaction;
            return 1;
        }
        if (pObject->AsSWSTrigger() != NULL)
        {
            *pnFaction = pObject->AsSWSTrigger()->m_nFaction;
            return 1;
        }
    }
    return 0;
}
// PC 0x004EB0F0 GOG 0x004EA300
void CServerExoAppInternal::UpDatePlayerPosition(
    unsigned long oidObject, const Vector& vPosition,
    const Vector& vOrientation)
{
    CGameObject* pObject = NULL;
    CSWSObject* pCreature;

    if (m_pObjectArray->GetGameObject(oidObject, &pObject) ==
            CGameObjectArray::SUCCESS &&
        pObject != NULL &&
        pObject->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
    {
        pCreature = (CSWSObject*)pObject->AsSWSCreature();
        if (pCreature != NULL)
        {
            pCreature->SetPosition(vPosition, 1, 1, 0);
            pCreature->SetOrientation(vOrientation);
        }
    }
}
// PC 0x004EB160 GOG 0x004EA370
int CServerExoAppInternal::IsInjured(unsigned long oidObject)
{
    CSWSCreature* pCreature = GetCreatureByGameObjectID(oidObject);

    if (pCreature != NULL)
    {
        return pCreature->IsInjured();
    }
    return 0;
}
// PC 0x004EB1C0 GOG 0x004EA3D0
int CServerExoAppInternal::GetPauseState(unsigned char nPauseType)
{
    return (unsigned char)(m_PauseState.m_nPauseState & nPauseType) > 0;
}
// PC 0x004EB1E0 GOG 0x004EA3F0
unsigned char CServerExoAppInternal::GetActivePauseState()
{
    if (GetPauseState(PAUSE_TYPE_COMBAT_MASK))
    {
        return PAUSE_STATE_COMBAT;
    }
    else if (GetPauseState(PAUSE_TYPE_PAUSED_MASK))
    {
        return PAUSE_STATE_PAUSED;
    }
    return 0;
}
// PC 0x004EB200 GOG 0x004EA410
CExoLinkedList<unsigned long>* CServerExoAppInternal::GetActiveExclusionList()
{
    if (GetActivePauseState() == PAUSE_STATE_COMBAT)
    {
        return m_pTimestopExclusionList;
    }
    else if (GetActivePauseState() == PAUSE_STATE_PAUSED)
    {
        return m_pPauseExclusionList;
    }
    return NULL;
}
// PC 0x004EB230 GOG 0x004EA440
int CServerExoAppInternal::GetDifficultyOption(int nOption)
{
    int nDifficulty = m_pServerInfo->m_nDifficultyLevel;

    return g_pRules->GetDifficultyOption(nOption, nDifficulty);
}
// PC 0x004EB250 GOG 0x004EA460
int CServerExoAppInternal::UpdateMainLoopTimeBudget(unsigned __int64 nTime)
{
    if (m_nLastMainLoopTime == 0)
    {
        m_nLastMainLoopTime = nTime;
        m_nMainLoopTimeBudget = 10000000;
        return 0;
    }

    if (nTime - m_nLastMainLoopTime >= m_nMainLoopTimeBudget)
    {
        m_nMainLoopTimeBudget = 10000000;
        m_nLastMainLoopTime = nTime;
        return 1;
    }

    m_nMainLoopTimeBudget = m_nLastMainLoopTime + m_nMainLoopTimeBudget - nTime;
    m_nLastMainLoopTime = nTime;
    return 0;
}
// PC 0x004EB320 GOG 0x004EA530
int CServerExoAppInternal::UpdateAutoSaveTimer()
{
    int nCurrentTime;
    int nPreviousTime;

    m_pPauseTimer->GetSnapshotTime();
    nCurrentTime = g_pExoBase->m_pExoTimers->GetLowResolutionTimer();
    if (m_bAutoSaveTimerExpired != 0)
    {
        m_nAutoSaveLastTime = nCurrentTime;
        return 1;
    }
    if (m_nAutoSaveLastTime == 0)
    {
        m_nAutoSaveLastTime = nCurrentTime;
    }
    nPreviousTime = m_nAutoSaveLastTime;
    m_nAutoSaveLastTime = nCurrentTime;
    m_nAutoSaveElapsedTime += nCurrentTime - nPreviousTime;
    if ((unsigned int)m_nAutoSaveElapsedTime > 900000)
    {
        m_nAutoSaveElapsedTime = 0;
        m_bAutoSaveTimerExpired = 1;
        return 1;
    }
    return 0;
}
// PC 0x004EB3B0 GOG 0x004EA5C0
unsigned long CServerExoAppInternal::ClientToServerObjectId(
    unsigned long oidClient)
{
    if (oidClient != OBJECT_INVALID)
    {
        return oidClient & ~CGameObjectArray::LISTTYPE_MASK;
    }
    return OBJECT_INVALID;
}
// PC 0x004EB3D0 GOG 0x004ED490
int CServerExoAppInternal::GetPlayerCreatureId()
{
    CSWSPlayer* pPlayer;

    pPlayer = GetServerExoApp()->GetClientObjectByPlayerId(0, 1);
    if (pPlayer != NULL && pPlayer->GetGameObject() != NULL)
    {
        return pPlayer->GetGameObject()->m_nId;
    }
    return OBJECT_INVALID;
}
// PC 0x004EB410 GOG 0x004EA5E0
void CServerExoAppInternal::SkipPastDialog(unsigned long oidObject)
{
    CGameObject* pObject = GetGameObject(oidObject);

    if (pObject != NULL && pObject->AsSWSObject() != NULL)
    {
        pObject->AsSWSObject()->m_nDialogReplyState = 1;
    }
}
// PC 0x004EB460 GOG 0x004ED550
int CServerExoAppInternal::CreatureAcquireItem(unsigned long oidObject,
                                               const CResRef& cResRef)
{
    CGameObject* pObject = NULL;
    CSWSItem* pItem;

    if (m_pObjectArray->GetGameObject(oidObject, &pObject) !=
            CGameObjectArray::SUCCESS ||
        pObject == NULL || ((CSWSObject*)pObject)->AsSWSCreature() == 0)
    {
        return 0;
    }
    pItem = new CSWSItem(OBJECT_INVALID);
    pItem->LoadFromTemplate(cResRef);
    pItem->m_bInfinite = 0;
    return ((CSWSObject*)pObject)
        ->AsSWSCreature()
        ->AcquireItem(&pItem, OBJECT_INVALID, OBJECT_INVALID, 1);
}
#ifndef _XBOX
int CServerExoAppInternal::TestSafeLocation(  // PC 0x004EB560 GOG 0x004ED650
    unsigned long oidObject, const Vector& vPosition)
{
    CGameObjectArray* pObjectArray = m_pObjectArray;
    CGameObject* pObject = NULL;
    CSWSCreature* pCreature;

    if (pObjectArray->GetGameObject(oidObject, &pObject) !=
        CGameObjectArray::SUCCESS)
    {
        return 0;
    }
    if (pObject == NULL)
    {
        return 0;
    }
    if (pObject->m_nObjectType != CGameObject::OBJECT_TYPE_CREATURE)
    {
        return 0;
    }
    pCreature = pObject->AsSWSCreature();
    if (pCreature == NULL)
    {
        return 0;
    }
    CPathfindInformation* pPathfind = pCreature->m_pPathfindInformation;
    unsigned long oidArea = pCreature->m_oidArea;
    return GetAreaByGameObjectID(oidArea)
        ->TestSafeLocationPoint(vPosition, pPathfind);
}
// PC 0x004EB5E0 GOG 0x004ED6D0
int CServerExoAppInternal::ComputeSafeLocation(
    const Vector& vPosition, float fRadius, unsigned long oidObject,
    int nFlags, Vector* pvSafe)
{
    CGameObjectArray* pObjectArray = m_pObjectArray;
    CGameObject* pObject = NULL;
    CSWSCreature* pCreature = NULL;

    if (pObjectArray->GetGameObject(oidObject, &pObject) ==
            CGameObjectArray::SUCCESS &&
        pObject != NULL &&
        pObject->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
    {
        pCreature = ((CSWSObject*)pObject)->AsSWSCreature();
    }
    if (pCreature == NULL)
    {
        return 0;
    }
    CPathfindInformation* pPathfind = pCreature->m_pPathfindInformation;
    return GetAreaByGameObjectID(pCreature->m_oidArea)->ComputeSafeLocation(
        vPosition, fRadius, pPathfind, nFlags, pvSafe);
}
#endif
void CServerExoAppInternal::UpdateMapData()  // PC 0x004EB670 GOG 0x004ED760
{
    CSWParty* pParty;
    CSWSModule* pModule;
    CSWSAreaMap* pAreaMap;
    CSWCCreature* pCreature;
    int nMember;

    pParty = g_pAppManager->m_pClientExoApp->GetSWParty();
    pModule = GetModule();
    if (pParty != NULL && pModule != NULL)
    {
        pAreaMap = pModule->m_pAreaMap;
        if (pAreaMap != NULL)
        {
            for (nMember = 0; nMember < pParty->m_nCharacters; nMember++)
            {
                pCreature = pParty->GetCharacter(nMember);
                if (pCreature != NULL)
                {
                    pAreaMap->SetWorldPointExplored(pCreature->m_vPosition, 1);
                    pAreaMap->SetPartyMemberWorldLocation(
                        nMember, pCreature->m_vPosition);
                    pAreaMap->SetPartyMemberWorldOrientation(
                        nMember, pCreature->m_vOrientation);
                }
            }
        }
    }
}
#ifndef _XBOX
Vector CServerExoAppInternal::GetNormalOfPoint(const Vector& vPosition)  // PC 0x004EB740 GOG 0x004ED8D0
{
    CSWSModule* pModule;
    CSWSRoom* pRoom;
    int nFace;

    pModule = GetServerExoApp()->GetModule();
    pRoom = pModule->GetArea()->GetRoom(vPosition, &nFace);
    if (pRoom != NULL)
    {
        return pRoom->GetNormalOfPoint(nFace);
    }
    return Vector(0.0f, 0.0f, 1.0f);
}
#endif
// PC 0x004EB7B0 GOG 0x004EA630
int CServerExoAppInternal::IncludeModuleInSave(const CExoString& sModule)
{
    C2DA* p2DA = new C2DA("modulesave", 0);
    int nIncludeInSave;

    if (p2DA == NULL)
    {
        return 1;
    }
    if (p2DA->Load2DArray() == 0)
    {
        delete p2DA;
        return 1;
    }
    if (p2DA->GetINTEntry(sModule, "IncludeInSave", &nIncludeInSave) == 0)
    {
        delete p2DA;
        return 1;
    }
    delete p2DA;
    return nIncludeInSave != 0;
}
int CServerExoAppInternal::AutoSaveOnEnteringModule(const CExoString& sModule,  // PC 0x004EB8D0 GOG 0x004ED940
                                                   C2DA* p2DA)
{
    CExoString sAutoSaveOnEnter;
    int bAutoSave;

    if (!p2DA->GetCExoStringEntry(sModule, "AutoSaveOnEnter", &sAutoSaveOnEnter))
    {
        m_nAutoSaveElapsedTime = 0;
        m_bAutoSaveTimerExpired = 0;
        return 1;
    }

    if (sAutoSaveOnEnter.CompareNoCase("force"))
        bAutoSave = 1;
    else if (sAutoSaveOnEnter.CompareNoCase("no"))
        bAutoSave = 0;
    else
    {
        if (m_bAutoSaveTimerExpired == 0)
            return 0;

        if (sAutoSaveOnEnter.CompareNoCase("yes"))
            bAutoSave = GetClientExoApp()->GetClientOptions()->m_bAutoSave;
        else
            bAutoSave = 1;
    }

    if (bAutoSave != 0)
    {
        m_nAutoSaveElapsedTime = 0;
        m_bAutoSaveTimerExpired = 0;
    }
    return bAutoSave;
}
// PC 0x004EBA20 GOG 0x004EA750
int CServerExoAppInternal::ShrinkLineByCreatures(const Vector& vStart,
                                                 const Vector& vEnd,
                                                 Vector& vResult)
{
    EXOXBOXASSERT(GetModule());
    EXOXBOXASSERT(GetModule()->GetArea());
    return GetModule()->GetArea()->ShrinkLineByCreatures(vStart, vEnd, vResult);
}
unsigned __int64 CServerExoAppInternal::GetSystemTimeAsDWORD64()
{
    SYSTEMTIME stLocalTime;
    FILETIME ftTime;
    GetLocalTime(&stLocalTime);
    SystemTimeToFileTime(&stLocalTime, &ftTime);
    return ((unsigned __int64)ftTime.dwHighDateTime << 32) | ftTime.dwLowDateTime;
}

#ifndef _XBOX
// PC 0x004F1110 GOG 0x004F0C50
void CServerExoAppInternal::DoPCAutosave(const CExoString& sModule, C2DA* p2DA)

{
    g_pAppManager->GetClientExoApp()->SetLoadScreenByModuleName(sModule);
    g_pAppManager->GetClientExoApp()->SetLoadScreenHint(
        g_pAppManager->GetClientExoApp()->GetNextLoadScreenHintSTRREF(sModule, 0));
    g_pAppManager->GetClientExoApp()->ShowLoadScreen(0, 1, 1);
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(0, 0);
    CClientExoAppInternal* pClient =
        g_pAppManager->GetClientExoApp()->m_pcExoAppInternal;
    pClient->UpdateRumble(0.0f);
    g_pExoInput->PauseRumble();
    g_pAppManager->GetClientExoApp()->GetSWGuiManager()->Draw(1.0f / 30.0f);
    UpdateScreen(1.0f / 30.0f, 0, 0);

    g_pExoResMan->CreateDirectory2("SAVES:");
    CExoString sDirectory;
    sDirectory.Format("SAVES:%06d - %s", CSWGuiSaveLoadEntry::SAVE_SLOT_AUTO, "AUTOSAVE");
    if (g_pExoResMan->CreateDirectory2(sDirectory) == 0)
    {
        g_pExoResMan->CleanDirectory(sDirectory, 0, 0);
    }
    int nLoadStep = g_pAppManager->GetClientExoApp()->GetLoadStep(0);
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(5 * nLoadStep / 100, 0);
    g_pAppManager->GetClientExoApp()->GetSWGuiManager()->Draw(1.0f / 30.0f);
    UpdateScreen(1.0f / 30.0f, 0, 0);
    m_partyTable.SaveTableInfo(sDirectory);
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(10 * nLoadStep / 100, 0);
    g_pAppManager->GetClientExoApp()->GetSWGuiManager()->Draw(1.0f / 30.0f);
    UpdateScreen(1.0f / 30.0f, 0, 0);
    m_globalVariableTable.Save(sDirectory);
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(15 * nLoadStep / 100, 0);
    g_pAppManager->GetClientExoApp()->GetSWGuiManager()->Draw(1.0f / 30.0f);
    UpdateScreen(1.0f / 30.0f, 0, 0);

    sDirectory = sDirectory + "\\";
    CExoString sSaveInfo = sDirectory + "savenfo";
    CResGFF* pGFF = new CResGFF;
    CResStruct cRoot;
    pGFF->CreateGFFFile(&cRoot, "NFO ", "V2.0");
    CExoString sAreaName;
    int nAreaName;
    if (p2DA->GetINTEntry(sModule, "AreaName", &nAreaName))
    {
        sAreaName = g_pTlkTable->GetSimpleString(nAreaName);
    }
    CExoString sBMPResRef;
    bool bMissingImage;
    {
        CExoString sColumn("BMPResRef");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pLoadscreens = p2DArrays->m_pAreaTransition;
        bMissingImage = pLoadscreens->GetCExoStringEntry(
            sModule, sColumn, &sBMPResRef) == 0;
    }
    if (bMissingImage)
    {
        sBMPResRef.Format("load_%s", sModule);
        int nResourceExists;
        {
            CResRef cImage(sBMPResRef);
            nResourceExists = g_pExoResMan->Exists(cImage, RESTYPE_TGA, (unsigned long*)NULL);
        }
        if (!nResourceExists)
        {
            CResRef cImage(sBMPResRef);
            nResourceExists = g_pExoResMan->Exists(cImage, RESTYPE_TPC, (unsigned long*)NULL);
        }
        if (!nResourceExists)
        {
            CExoString sColumn("BMPResRef");
            CExoString sDefault("DEFAULT");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pLoadscreens = p2DArrays->m_pAreaTransition;
            pLoadscreens->GetCExoStringEntry(sDefault, sColumn, &sBMPResRef);
        }
    }
    pGFF->WriteFieldCExoString(&cRoot, sAreaName, "AREANAME");
    pGFF->WriteFieldCExoString(&cRoot, sModule, "LASTMODULE");
    pGFF->WriteFieldDWORD(&cRoot, m_partyTable.GetTotalPlayedSeconds(), "TIMEPLAYED");
#ifndef _XBOX
    int bCheatUsed = m_partyTable.m_bCheatUsed;
    pGFF->WriteFieldBYTE(&cRoot, bCheatUsed, "CHEATUSED");
#endif
    pGFF->WriteFieldBYTE(&cRoot, 1, "PCAUTOSAVE");
    pGFF->WriteFieldCExoString(&cRoot, sBMPResRef, "SCREENSHOT");
    {
        SYSTEMTIME stLocalTime;
        FILETIME ftTimeStamp;
        GetLocalTime(&stLocalTime);
        SystemTimeToFileTime(&stLocalTime, &ftTimeStamp);
        unsigned __int64 nTimeStamp = ftTimeStamp.dwHighDateTime;
        nTimeStamp <<= 32;
        nTimeStamp |= ftTimeStamp.dwLowDateTime;
        pGFF->WriteFieldDWORD64(&cRoot, nTimeStamp, "TIMESTAMP");
    }
    pGFF->WriteFieldCExoString(&cRoot, m_partyTable.GetPCName(), "PCNAME");
    int nSaveNumber = m_partyTable.m_n248;
    pGFF->WriteFieldDWORD(&cRoot, nSaveNumber, "SAVENUMBER");
    pGFF->WriteFieldBYTE(&cRoot, g_pAppManager->GetClientExoApp()->GetGamePlayHint(),
                         "GAMEPLAYHINT");
    pGFF->WriteFieldBYTE(&cRoot, g_pAppManager->GetClientExoApp()->GetStoryHint(0),
                         "STORYHINT0");
    pGFF->WriteFieldBYTE(&cRoot, g_pAppManager->GetClientExoApp()->GetStoryHint(1),
                         "STORYHINT1");
    pGFF->WriteFieldBYTE(&cRoot, g_pAppManager->GetClientExoApp()->GetStoryHint(2),
                         "STORYHINT2");
    pGFF->WriteFieldBYTE(&cRoot, g_pAppManager->GetClientExoApp()->GetStoryHint(3),
                         "STORYHINT3");
    pGFF->WriteFieldBYTE(&cRoot, g_pAppManager->GetClientExoApp()->GetStoryHint(4),
                         "STORYHINT4");
    pGFF->WriteFieldBYTE(&cRoot, g_pAppManager->GetClientExoApp()->GetStoryHint(5),
                         "STORYHINT5");
    pGFF->WriteFieldBYTE(&cRoot, g_pAppManager->GetClientExoApp()->GetStoryHint(6),
                         "STORYHINT6");
    pGFF->WriteFieldBYTE(&cRoot, g_pAppManager->GetClientExoApp()->GetStoryHint(7),
                         "STORYHINT7");
    pGFF->WriteFieldBYTE(&cRoot, g_pAppManager->GetClientExoApp()->GetStoryHint(8),
                         "STORYHINT8");
    pGFF->WriteFieldBYTE(&cRoot, g_pAppManager->GetClientExoApp()->GetStoryHint(9),
                         "STORYHINT9");

    KOTOR_AUTOSAVE_PARAMS cParams;
    cParams.m_sLoadMusic =
        g_pAppManager->GetClientExoApp()->GetLoadMusicByModuleName(sModule);
    CExoArrayList<CExoString> lstMovies(
        *g_pAppManager->GetClientExoApp()->GetMoveToModuleMovies());
    cParams.m_sMovie1 = lstMovies.num > 0 ? lstMovies[0] : CExoString("");
    cParams.m_sMovie2 = lstMovies.num > 1 ? lstMovies[1] : CExoString("");
    cParams.m_sMovie3 = lstMovies.num > 2 ? lstMovies[2] : CExoString("");
    cParams.m_sMovie4 = lstMovies.num > 3 ? lstMovies[3] : CExoString("");
    cParams.m_sMovie5 = lstMovies.num > 4 ? lstMovies[4] : CExoString("");
    cParams.m_sMovie6 = lstMovies.num > 5 ? lstMovies[5] : CExoString("");
    unsigned long nCalendarDay, nTimeOfDay;
    unsigned long nHour, nMinute, nSecond, nMillisecond;
    unsigned long nYear, nMonth, nDay, nTimeOfDay2;
    unsigned char nDawnDusk;
    g_pAppManager->GetServerExoApp()->GetWorldTimer()->GetWorldTime(
        &nCalendarDay, &nTimeOfDay);
    g_pAppManager->GetServerExoApp()->GetWorldTimer()->ConvertFromTimeOfDay(
        nTimeOfDay, &nHour, &nMinute, &nSecond, &nMillisecond);
    GetModule()->GetTime(&nYear, &nMonth, &nDay, &nHour, &nDawnDusk, &nTimeOfDay2);
    cParams.m_nTimeYear = nYear;
    cParams.m_nTimeMonth = nMonth;
    cParams.m_nTimeDay = nDay;
    cParams.m_nTimeHour = nHour;
    cParams.m_nTimeMinute = nMinute;
    cParams.m_nTimeSecond = nSecond;
    cParams.m_nTimeMillisecond = nMillisecond;
    cParams.m_nTimePauseDay = nCalendarDay;
    cParams.m_nTimePauseTime = nTimeOfDay;
    cParams.m_sStartWaypoint =
        g_pAppManager->GetServerExoApp()->GetMoveToModuleStartWaypoint();
    CStatusSummary cSummary;
    g_pAppManager->GetClientExoApp()->GetInGameGui()->GetStatusSummary(cSummary);
    cParams.m_cStatusSummary = cSummary;
    CResStruct cAutoSaveParams;
    if (pGFF->AddStructToStruct(&cAutoSaveParams, &cRoot, "AUTOSAVEPARAMS", 0))
    {
        cParams.SaveToGFF(pGFF, cAutoSaveParams);
    }
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(20 * nLoadStep / 100, 0);
    g_pAppManager->GetClientExoApp()->GetSWGuiManager()->Draw(1.0f / 30.0f);
    UpdateScreen(1.0f / 30.0f, 0, 0);

    CSWParty* pParty = g_pAppManager->GetClientExoApp()->GetSWParty();
    CExoString sPortrait;
    for (int nCharacter = 0; nCharacter < pParty->GetNumCharacters(); ++nCharacter)
    {
        CSWSCreature* pCreature = pParty->GetCharacter(nCharacter)->GetServerCreature();
        if (pCreature != NULL)
        {
            sPortrait.Format("%s%d", "PORTRAIT", nCharacter);
            CSWCCreature* pClientCreature = pCreature->GetClientCreature();
            if (pClientCreature != NULL)
            {
                pGFF->WriteFieldCResRef(&cRoot, pClientCreature->GetPortrait(1),
                                       sPortrait.CStr());
            }
            else
            {
                pGFF->WriteFieldCResRef(&cRoot, pCreature->GetPortrait(),
                                       sPortrait.CStr());
            }
        }
    }
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(25 * nLoadStep / 100, 0);
    g_pAppManager->GetClientExoApp()->GetSWGuiManager()->Draw(1.0f / 30.0f);
    UpdateScreen(1.0f / 30.0f, 0, 0);
    pGFF->WriteGFFFile(sSaveInfo, 0);
    delete pGFF;

    CERFFile* pERF = new CERFFile;
    CExoString sSaveGame = sDirectory + "SAVEGAME";
    pERF->Create(sSaveGame);
    pERF->SetVersion("MOD V1.0");
    pERF->WriteHeader();
    pERF->WriteStringTable();
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(30 * nLoadStep / 100, 0);
    UpdateScreen(1.0f / 30.0f, 0, 0);
    pERF->ImportFiles("GAMEINPROGRESS:", (void*)-1, UpdateLoadScreen,
                      30 * nLoadStep / 100, 60 * nLoadStep / 100);
    pERF->Finish();
    delete pERF;

    CExoString sPlayerInfo = sDirectory + "pifo";
    g_pExoResMan->FileCopy("TEMP:pifo", sPlayerInfo, RESTYPE_IFO, RESTYPE_INVALID);
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(100 * nLoadStep / 100, 0);
    g_pAppManager->GetClientExoApp()->GetSWGuiManager()->Draw(1.0f / 30.0f);
    UpdateScreen(1.0f / 30.0f, 0, 0);
}
#endif
void CServerExoAppInternal::SetMoveToWorldTime(  // PC 0x004EBB30 GOG 0x004EA860
    unsigned long nCalendarDay, unsigned char nHour, unsigned char nMinute,
    unsigned char nSecond, unsigned short nMillisecond, unsigned short nYear,
    unsigned short nMonth)
{
    m_nMoveToWorldCalendarDay = nCalendarDay;
    m_nMoveToWorldHour = nHour;
    m_nMoveToWorldMinute = nMinute;
    m_nMoveToWorldSecond = nSecond;
    m_nMoveToWorldMillisecond = nMillisecond;
    m_nMoveToWorldYear = nYear;
    m_nMoveToWorldMonth = nMonth;
}
// PC 0x004EBB80 GOG 0x004EA8B0
void CServerExoAppInternal::GetMoveToWorldTime(
    unsigned long& nCalendarDay, unsigned char& nHour, unsigned char& nMinute,
    unsigned char& nSecond, unsigned short& nMillisecond, unsigned short& nYear,
    unsigned short& nMonth)
{
    nCalendarDay = m_nMoveToWorldCalendarDay;
    nHour = m_nMoveToWorldHour;
    nMinute = m_nMoveToWorldMinute;
    nSecond = m_nMoveToWorldSecond;
    nMillisecond = m_nMoveToWorldMillisecond;
    nYear = m_nMoveToWorldYear;
    nMonth = m_nMoveToWorldMonth;
}
// PC 0x004EBBE0 GOG 0x004EA910
void CServerExoAppInternal::DoModuleEnterSaveCleanup(const CExoString& sModule,
                                                     C2DA* p2DA)
{
    int nSaveGroup;
    int nDeleteGroup;
    int nRow;
    int bFound;

    bFound =
        p2DA->GetINTEntry(sModule, "DeleteSaveGroupOnEnter", &nDeleteGroup);
    if (bFound != 0)
    {
        CExoString sPath("GAMEINPROGRESS:");
        CExoString sFileName;

        for (nRow = 0; nRow < p2DA->m_nNumRows; nRow++)
        {
            bFound = p2DA->GetINTEntry(nRow, "SaveGroup", &nSaveGroup);
            if (bFound != 0 && nSaveGroup == nDeleteGroup)
            {
                bFound =
                    p2DA->GetCExoStringEntry(nRow, "ModuleName", &sFileName);
                if (bFound != 0)
                {
                    g_pExoResMan->RemoveFile(sPath + sFileName, 0x809);
                }
            }
        }
    }
}
// PC 0x004EBD80 GOG 0x004EDA90
int CServerExoAppInternal::GetModulePreviouslyVisited(const CExoString& sModule)
{
    if (sModule.GetLength() < 1)
    {
        return 1;
    }

    CExoString sModuleFile;
    CExoString sModulePath;
    unsigned short nModuleType = RESTYPE_INVALID;
    int bPreviouslyVisited = 0;

    if (IncludeModuleInSave(sModule))
    {
        g_pAppManager->GetServerExoApp()->GetPartyTable()->AddGameInProgress();
        if (g_pExoResMan->Exists(sModule, RESTYPE_RSV, (unsigned long*)NULL))
        {
            nModuleType = RESTYPE_RSV;
            bPreviouslyVisited = 1;
        }
        else if (g_pExoResMan->Exists(sModule, RESTYPE_SAV, (unsigned long*)NULL))
        {
            nModuleType = RESTYPE_SAV;
            bPreviouslyVisited = 1;
        }
        g_pAppManager->GetServerExoApp()->GetPartyTable()->RemoveGameInProgress();
    }

    if (nModuleType == RESTYPE_INVALID)
    {
        g_pExoResMan->AddResourceDirectory("NWMFILES:");
        if (g_pExoResMan->Exists(sModule, RESTYPE_NWM, (unsigned long*)NULL))
        {
            nModuleType = RESTYPE_NWM;
            bPreviouslyVisited = 0;
        }
        g_pExoResMan->RemoveResourceDirectory("NWMFILES:");
    }

    if (nModuleType == RESTYPE_INVALID)
    {
        g_pExoResMan->AddResourceDirectory("MODULES:");
        if (g_pExoResMan->Exists(sModule, RESTYPE_MOD, (unsigned long*)NULL))
        {
            nModuleType = RESTYPE_MOD;
            bPreviouslyVisited = 0;
        }
        else if (g_pExoResMan->Exists(sModule, RESTYPE_RIM, (unsigned long*)NULL))
        {
            nModuleType = RESTYPE_RIM;
            bPreviouslyVisited = 0;
        }
        g_pExoResMan->RemoveResourceDirectory("MODULES:");
    }

    if (nModuleType == RESTYPE_INVALID)
    {
        CExoString sDirectory;
        for (int nContentPackage = 1; nContentPackage < CTlkTable::NUM_CONTENT_PKGS;
             ++nContentPackage)
        {
            sDirectory.Format("LIVE%d", nContentPackage);
            if (g_pExoBase->m_pExoAliasList->GetAliasPath(sDirectory, 0).GetLength())
            {
                sDirectory = sDirectory + ":MODULES";
                g_pExoResMan->AddResourceDirectory(sDirectory);
                if (g_pExoResMan->Exists(sModule, RESTYPE_MOD, (unsigned long*)NULL))
                {
                    nModuleType = RESTYPE_MOD;
                    bPreviouslyVisited = 0;
                }
                else if (nModuleType == RESTYPE_INVALID
                         && g_pExoResMan->Exists(sModule, RESTYPE_RIM,
                                               (unsigned long*)NULL))
                {
                    nModuleType = RESTYPE_RIM;
                    bPreviouslyVisited = 0;
                }
                g_pExoResMan->RemoveResourceDirectory(sDirectory);
                if (nModuleType != RESTYPE_INVALID)
                {
                    break;
                }
            }
        }
    }
    g_pExoInput->UnpauseRumble();
    if (nModuleType == RESTYPE_INVALID)
    {
        return 0;
    }
    return bPreviouslyVisited;
}
// PC 0x004EC1E0 GOG 0x004EAAB0
int CServerExoAppInternal::HasEnoughDiskSpaceForSaveGame()
{
    unsigned __int64 nFreeBytes = 0;

    g_pExoResMan->GetFreeDiskSpace("SAVES:", &nFreeBytes);
    __int64 nFreeUnits = nFreeBytes >> 14;

    return nFreeUnits > 1600;
}
// PC 0x004065A0 GOG 0x00406570
int CServerExoAppInternal::GetModuleLanguage()
{
    return 0;
}
// PC 0x004015A0 GOG 0x004015A0
void CServerExoAppInternal::Uninitialize()
{
}

void CServerExoAppInternal::InitializeNetLayer()
{
    m_pNetLayer->Initialize(g_pAppManager->GetServerExoApp());
}
#ifndef _XBOX

int CServerExoAppInternal::DebugValidatePath(unsigned long oidCreature,
                                             int bTempWayPoints)
{
    if (EnablePathValidation == 0)
    {
        return 1;
    }

    CSWSCreature* pCreature =
        g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(oidCreature);
    if (pCreature == NULL)
    {
        return 0;
    }
    CSWSArea* pArea = pCreature->GetArea();
    if (pArea == NULL)
    {
        return 0;
    }
    CPathfindInformation* pPathfind = pCreature->m_pPathfindInformation;
    if (pPathfind == NULL)
    {
        return 0;
    }

    int nCount;
    float* pfPoints;
    if (bTempWayPoints == 0)
    {
        nCount = pPathfind->m_nWayPointCount;
        pfPoints = pPathfind->m_pWayPoints;
    }
    else
    {
        nCount = pPathfind->m_nField_94;
        pfPoints = pPathfind->m_pWayPointsTemp;
    }
    if (nCount == 0 || pfPoints == NULL)
    {
        return 1;
    }

    Vector vPoint(pfPoints[0], pfPoints[1], 0.0f);
    vPoint.m_fZ = pArea->ComputeHeight(vPoint, 1, NULL);
    pArea->m_pPathfindInformation = pPathfind;
    int bValid = DebugValidatePath_Point(pArea, vPoint);
    Vector vLast = vPoint;

    for (int nPoint = 2; nPoint < nCount; nPoint += 2)
    {
        vPoint = Vector(pfPoints[nPoint], pfPoints[nPoint + 1], 0.0f);
        vPoint.m_fZ = pArea->ComputeHeight(vPoint, 1, NULL);
        int bPointValid = DebugValidatePath_Point(pArea, vPoint);
        int bSegmentValid =
            DebugValidatePath_Segment(pArea, pPathfind, vLast, vPoint);
        bValid = bValid & bSegmentValid & bPointValid;
        vLast = vPoint;
    }
    pArea->m_pPathfindInformation = pPathfind;
    return bValid;
}

int CServerExoAppInternal::DebugValidatePath_Point(CSWSArea* pArea,
                                                   const Vector& vPoint)
{
    int bInRoom = pArea->GetRoom(vPoint, NULL) != NULL;
    int bWalkable = pArea->PositionWalkable(vPoint) != 0;
    return bInRoom & bWalkable;
}

int CServerExoAppInternal::DebugValidatePath_Segment(
    CSWSArea* pArea, CPathfindInformation* pPathfind, const Vector& vStart,
    const Vector& vEnd)
{
    CWalkHitInfo cHitInfo;
    int nResult = pArea->TestDirectLine(vStart, vEnd, pPathfind->m_fStepScale,
                                        pPathfind->m_fHalfStep, 1, &cHitInfo);
    if (nResult > 0 || nResult == CSWSArea::DIRECT_LINE_BLOCKED_CREATURE)
    {
        return 1;
    }
    if (nResult != CSWSArea::DIRECT_LINE_BLOCKED_OBJECT)
    {
        return 0;
    }

    CGameObject* pObject = NULL;
    if (m_pObjectArray->GetGameObject(cHitInfo.m_oidBlocker, &pObject) !=
            CGameObjectArray::SUCCESS ||
        pObject == NULL)
    {
        return 1;
    }
    int bValid = 0;
    if (pObject->AsSWSDoor() != NULL)
    {
        if (pObject->AsSWSDoor()->m_bStatic == 0)
        {
            nResult = CSWSArea::DIRECT_LINE_CLEAR;
            bValid = 1;
        }
    }
    if (pObject->AsSWSPlaceable() != NULL &&
        pObject->AsSWSPlaceable()->m_nId ==
            pPathfind->m_oidPathfindTargetCreature)
    {
        return 1;
    }
    if (nResult == CSWSArea::DIRECT_LINE_BLOCKED_OBJECT)
    {
        bValid = 1;
    }
    return bValid;
}
#endif

void CServerExoAppInternal::RunDeathSequence()
{
    g_pAppManager->GetClientExoApp()->RunDeathSequence();
}

#ifdef _XBOX

void KOTOR_LAUNCH_OPTIONS_DATA::SaveToGFF(CResGFF* pRes, CResStruct& stStruct)
{
    CResList lstMoviesShown;
    CResStruct stMovieShown;

    pRes->WriteFieldBYTE(&stStruct, m_bAutoLevelNPCs, "AUTOLEVELNPCS");
    pRes->WriteFieldBYTE(&stStruct, m_bAutoCentre, "AUTOCENTRE");
    pRes->WriteFieldBYTE(&stStruct, m_bAutoSave, "AUTOSAVE");
    pRes->WriteFieldBYTE(&stStruct, m_bReverse, "REVERSE");
    pRes->WriteFieldBYTE(&stStruct, m_bCombatMove, "COMBATMOVE");
    pRes->WriteFieldBYTE(&stStruct, m_bHideUnequippable, "HIDEUNEQUIPPABLE");
    pRes->WriteFieldBYTE(&stStruct, m_bShowTutorial, "SHOWTUTORIAL");
    pRes->WriteFieldBYTE(&stStruct, m_bShowSubtitles, "SHOWSUBTITLES");
    pRes->WriteFieldBYTE(&stStruct, m_bShowMiniMap, "SHOWMINIMAP");
    pRes->WriteFieldBYTE(&stStruct, m_bRumble, "RUMBLE");
    pRes->WriteFieldBYTE(&stStruct, m_bShowFloaty, "SHOWFLOATY");
    pRes->WriteFieldBYTE(&stStruct, m_bEndRound, "ENDROUND");
    pRes->WriteFieldBYTE(&stStruct, m_bEnemySighted, "ENEMYSIGHTED");
    pRes->WriteFieldBYTE(&stStruct, m_bPartyKilled, "PARTYKILLED");
    pRes->WriteFieldBYTE(&stStruct, m_bUseHorzMenu, "USEHORZMENU");
    pRes->WriteFieldBYTE(&stStruct, m_bUseVertMenu, "USEVERTMENU");
    pRes->WriteFieldBYTE(&stStruct, m_bTrigger, "TRIGGER");
    pRes->WriteFieldBYTE(&stStruct, m_bShowSummary, "SHOWSUMMARY");
    pRes->WriteFieldBYTE(&stStruct, m_nDifficultyOption, "DIFFICULTYOPTION");
    pRes->WriteFieldBYTE(&stStruct, m_nGamma, "GAMMA");
    pRes->WriteFieldFLOAT(&stStruct, m_fGammaMin, "GAMMAMIN");
    pRes->WriteFieldFLOAT(&stStruct, m_fGammaMax, "GAMMAMAX");
    pRes->WriteFieldBYTE(&stStruct, m_nVolumeMusic, "VOLUMEMUSIC");
    pRes->WriteFieldBYTE(&stStruct, m_nVolumeVO, "VOLUMEVO");
    pRes->WriteFieldBYTE(&stStruct, m_nVolumeFX, "VOLUMEFX");
    pRes->WriteFieldBYTE(&stStruct, m_nVolumeMovie, "VOLUMEMOVIE");
    pRes->WriteFieldBYTE(&stStruct, m_nNumMovies, "NUMMOVIES");
    if (pRes->AddList(&lstMoviesShown, &stStruct, "MOVIESSHOWN"))
    {
        for (int nMovie = 0; nMovie < 11; nMovie++)
        {
            if (pRes->AddListElement(&stMovieShown, &lstMoviesShown, 0))
            {
                pRes->WriteFieldBYTE(&stMovieShown, m_aMoviesShown[nMovie], "MOVIESHOWN");
            }
        }
    }
}

int KOTOR_LAUNCH_DATA::SaveToGFF(CResGFF* pRes, CResStruct& stStruct)
{
    CResStruct stSub;

    pRes->WriteFieldDWORD(&stStruct, GetModuleTrans(), "MODULETRANS");
    pRes->WriteFieldBYTE(&stStruct, GetRenderQA(), "RENDERQA");
    pRes->WriteFieldBYTE(&stStruct, m_bLowDiskMode, "LOWDISKMODE");
    pRes->WriteFieldDWORD(&stStruct, GetAutoCalInfo(), "AUTOCALINFO");
    pRes->WriteFieldDWORD(&stStruct, GetPortBinding(), "PORTBINDING");
    pRes->WriteFieldINT(&stStruct, GetSaveSlot(), "SAVESLOT");
    pRes->WriteFieldCExoString(&stStruct, GetXboxSavePath(), "XBOXSAVEPATH");
    pRes->WriteFieldCExoString(&stStruct, GetModuleName(), "MODULENAME");
    pRes->WriteFieldCExoString(&stStruct, GetLoadScreen(), "LOADSCREEN");
    pRes->WriteFieldCExoString(&stStruct, GetLoadMusic(), "LOADMUSIC");
    pRes->WriteFieldCExoString(&stStruct, GetStartWaypoint(), "STARTWAYPOINT");
    pRes->WriteFieldCExoString(&stStruct, GetMovie1(), "MOVIE1");
    pRes->WriteFieldCExoString(&stStruct, GetMovie2(), "MOVIE2");
    pRes->WriteFieldCExoString(&stStruct, GetMovie3(), "MOVIE3");
    pRes->WriteFieldCExoString(&stStruct, GetMovie4(), "MOVIE4");
    pRes->WriteFieldCExoString(&stStruct, GetMovie5(), "MOVIE5");
    pRes->WriteFieldCExoString(&stStruct, GetMovie6(), "MOVIE6");
    pRes->WriteFieldBYTE(&stStruct, m_bMinGlobalRim, "MINGLOBALRIM");
    pRes->WriteFieldBYTE(&stStruct, m_bLoadingSave, "LOADINGSAVE");
    pRes->WriteFieldBYTE(&stStruct, m_bLoadingSave, "LOAD_AUTOSAVE");
    pRes->WriteFieldDWORD(&stStruct, m_nTimeYear, "TIME_YEAR");
    pRes->WriteFieldBYTE(&stStruct, m_nTimeMonth, "TIME_MONTH");
    pRes->WriteFieldBYTE(&stStruct, m_nTimeDay, "TIME_DAY");
    pRes->WriteFieldBYTE(&stStruct, m_nTimeHour, "TIME_HOUR");
    pRes->WriteFieldWORD(&stStruct, m_nTimeMinute, "TIME_MINUTE");
    pRes->WriteFieldWORD(&stStruct, m_nTimeSecond, "TIME_SECOND");
    pRes->WriteFieldWORD(&stStruct, m_nTimeMillisecond, "TIME_MILLISECOND");
    pRes->WriteFieldDWORD(&stStruct, m_nTimePauseDay, "TIME_PAUSEDAY");
    pRes->WriteFieldDWORD(&stStruct, m_nTimePauseTime, "TIME_PAUSETIME");
    pRes->AddStructToStruct(&stSub, &stStruct, "OPTIONSDATA", 0);
    m_cOptionsData.SaveToGFF(pRes, stSub);
    pRes->AddStructToStruct(&stSub, &stStruct, "STATUSSUMMARY", 0);
    m_cStatusSummary.SaveToGFF(pRes, stSub);
    pRes->WriteFieldBYTE(&stStruct, m_nGamePlayHint, "GAMEPLAYHINT");
    return pRes->WriteFieldBYTE(&stStruct, m_nStoryHint, "STORYHINT");
}

int CServerExoAppInternal::IsMoveToModuleMinGlobalRim()
{
    CExoString sModuleResRef;
    CExoString sModule;

    sModule = m_sMoveToModule;
    C2DA* pMinGlobalRim = g_pRules->Get2DArrays()->m_pMinGlobalRim;
    if (pMinGlobalRim != NULL)
    {
        for (int nRow = 0; nRow < pMinGlobalRim->m_nNumRows; nRow++)
        {
            if (pMinGlobalRim->GetCExoStringEntry(nRow, "ModuleResRef", &sModuleResRef) == 1)
            {
                if (sModuleResRef.CompareNoCase(sModule) == 1)
                {
                    return 1;
                }
            }
        }
    }
    return 0;
}

void CServerExoAppInternal::DoPCAutosave(const CExoString& sModule, KOTOR_LAUNCH_DATA& cLaunchData, C2DA* p2DA)
{
    AurReleaseFrameTexture();
    g_pAppManager->GetClientExoApp()->SetLoadScreenByModuleName(sModule);
    g_pAppManager->GetClientExoApp()->SetLoadScreenHint(41926);
    g_pAppManager->GetClientExoApp()->ShowLoadScreen(0, 1, 1);
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(0, 0);
    CClientExoAppInternal* pClient =
        g_pAppManager->GetClientExoApp()->m_pcExoAppInternal;
    pClient->UpdateRumble(0.0f);
    g_pExoInput->PauseRumble();
    g_pAppManager->GetClientExoApp()->GetSWGuiManager()->Draw(1.0f / 30.0f);
    UpdateScreen(1.0f / 30.0f, 0, 0);
    unsigned long nStartTime = g_pExoBase->m_pExoTimers->GetLowResolutionTimer();

    CExoString sSaveRoot("U:\\");
    WCHAR wszSaveName[256];
    char szSavePath[256];
    mbstowcs(wszSaveName, g_pTlkTable->GetSimpleString(STRREF_AUTO_SAVE_NAME).CStr(), 255);
    XCreateSaveGame(sSaveRoot.CStr(), wszSaveName, OPEN_ALWAYS, 1, szSavePath, 254);
    {
        CExoString sAlias("SAVES");
        g_pExoBase->m_pExoAliasList->ChangePath(sAlias, szSavePath);
    }
    CExoString sDirectory;
    sDirectory.Format("SAVES:%06d - %s", CSWGuiSaveLoadEntry::SAVE_SLOT_QUICK, "AUTOSAVE");
    if (g_pExoResMan->CreateDirectory2(sDirectory) == 0)
    {
        g_pExoResMan->CleanDirectory(sDirectory, 0, 0);
    }
    sDirectory = sDirectory + "\\";
    g_pExoResMan->FileCopy("HD0:AutoSaveImage", "SAVES:SaveImage", 3011, RESTYPE_INVALID);
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(5, 0);
    g_pAppManager->GetClientExoApp()->GetSWGuiManager()->Draw(1.0f / 30.0f);
    UpdateScreen(1.0f / 30.0f, 0, 0);

    CExoString sSaveInfo = sDirectory + "savenfo";
    CResGFF* pGFF = new CResGFF;
    CResStruct cRoot;
    pGFF->CreateGFFFile(&cRoot, "NFO ", "V2.0");
    CExoString sAreaName;
    int nAreaName;
    if (p2DA->GetINTEntry(sModule, "AreaName", &nAreaName))
    {
        sAreaName = g_pTlkTable->GetSimpleString(nAreaName);
    }
    CExoString sBMPResRef;
    bool bMissingImage;
    {
        CExoString sColumn("BMPResRef");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pLoadscreens = p2DArrays->m_pAreaTransition;
        bMissingImage = pLoadscreens->GetCExoStringEntry(
            sModule, sColumn, &sBMPResRef) == 0;
    }
    if (bMissingImage)
    {
        sBMPResRef.Format("load_%s", sModule);
        int nResourceExists;
        {
            CResRef cImage(sBMPResRef);
            nResourceExists = g_pExoResMan->Exists(cImage, RESTYPE_TGA, (unsigned long*)NULL);
        }
        if (!nResourceExists)
        {
            CResRef cImage(sBMPResRef);
            nResourceExists = g_pExoResMan->Exists(cImage, RESTYPE_TPC, (unsigned long*)NULL);
        }
        if (!nResourceExists)
        {
            CExoString sColumn("BMPResRef");
            CExoString sDefault("DEFAULT");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pLoadscreens = p2DArrays->m_pAreaTransition;
            pLoadscreens->GetCExoStringEntry(sDefault, sColumn, &sBMPResRef);
        }
    }
    pGFF->WriteFieldCExoString(&cRoot, sAreaName, "AREANAME");
    pGFF->WriteFieldCExoString(&cRoot, sModule, "LASTMODULE");
    pGFF->WriteFieldDWORD(&cRoot, m_partyTable.GetTotalPlayedSeconds(), "TIMEPLAYED");
    pGFF->WriteFieldBYTE(&cRoot, 1, "REBOOTAUTOSAVE");
    pGFF->WriteFieldCExoString(&cRoot, sBMPResRef, "SCREENSHOT");
    pGFF->WriteFieldBYTE(&cRoot, g_pAppManager->GetClientExoApp()->GetGamePlayHint(),
                         "GAMEPLAYHINT");
    pGFF->WriteFieldBYTE(&cRoot, g_pAppManager->GetClientExoApp()->GetStoryHint(),
                         "STORYHINT");
    unsigned char nLiveContent = 0;
    CExoString sAlias;
    for (int nContent = 1; nContent < CTlkTable::NUM_CONTENT_PKGS; nContent++)
    {
        sAlias.Format("LIVE%d", nContent);
        pGFF->WriteFieldCExoString(&cRoot, g_pAppManager->GetClientExoApp()->GetLiveContentName(nContent),
                                   sAlias.CStr());
        if (g_pExoBase->m_pExoAliasList->GetAliasPath(sAlias, 0).GetLength() > 0)
        {
            nLiveContent |= 1 << (nContent - 1);
        }
    }
    pGFF->WriteFieldBYTE(&cRoot, nLiveContent, "LIVECONTENT");
    CResStruct cLaunchParams;
    if (pGFF->AddStructToStruct(&cLaunchParams, &cRoot, "LAUNCHPARAMS", 0))
    {
        cLaunchData.SaveToGFF(pGFF, cLaunchParams);
    }
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(10, 0);
    g_pAppManager->GetClientExoApp()->GetSWGuiManager()->Draw(1.0f / 30.0f);
    UpdateScreen(1.0f / 30.0f, 0, 0);

    CSWParty* pParty = g_pAppManager->GetClientExoApp()->GetSWParty();
    CExoString sPortrait;
    for (int nCharacter = 0; nCharacter < pParty->GetNumCharacters(); ++nCharacter)
    {
        CSWSCreature* pCreature = pParty->GetCharacter(nCharacter)->GetServerCreature();
        if (pCreature != NULL)
        {
            sPortrait.Format("%s%d", "PORTRAIT", nCharacter);
            CSWCCreature* pClientCreature = pCreature->GetClientCreature();
            if (pClientCreature != NULL)
            {
                pGFF->WriteFieldCResRef(&cRoot, pClientCreature->GetPortrait(1),
                                       sPortrait.CStr());
            }
            else
            {
                pGFF->WriteFieldCResRef(&cRoot, pCreature->GetPortrait(),
                                       sPortrait.CStr());
            }
        }
    }
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(15, 0);
    g_pAppManager->GetClientExoApp()->GetSWGuiManager()->Draw(1.0f / 30.0f);
    UpdateScreen(1.0f / 30.0f, 0, 0);
    pGFF->WriteGFFFile(sSaveInfo, 0);
    delete pGFF;
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(20, 0);
    UpdateScreen(1.0f / 30.0f, 0, 0);
    unsigned char aInfoSignature[XCALCSIG_SIGNATURE_SIZE];
    g_pExoResMan->CreateSignatureFile(sSaveInfo, 0, 0, aInfoSignature, 0);
    g_pExoResMan->WriteSignatureFile(sDirectory + "SAVE_INFO", aInfoSignature);

    XCALCSIG_SIGNATURE cDataSignature;
    unsigned char aHeaderSignature[XCALCSIG_SIGNATURE_SIZE];
    unsigned char aHeaderVarSignature[XCALCSIG_SIGNATURE_SIZE];
    HANDLE hSignature;
    CERFFile* pERF = new CERFFile;
    CExoString sERFName = sDirectory + "SAVEGAME";
    pERF->Create(sERFName);
    pERF->SetVersion("MOD V1.0");
    pERF->WriteHeader();
    pERF->WriteStringTable();
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(25, 0);
    UpdateScreen(1.0f / 30.0f, 0, 0);
    hSignature = XCalculateSignatureBegin(0);
    pERF->ImportFiles("GAMEINPROGRESS:", hSignature, UpdateLoadScreen, 25, 30);
    if (XCalculateSignatureEnd(hSignature, &cDataSignature) == ERROR_SUCCESS)
    {
        g_pExoResMan->WriteSignatureFile(sDirectory + "SAVE_DATA", (unsigned char*)&cDataSignature);
    }
    pERF->Finish();
    pERF->Read(sERFName, aHeaderSignature);
    pERF->ReadHeaderVariance(sERFName, aHeaderVarSignature);
    g_pExoResMan->WriteSignatureFile(sDirectory + "SAVE_HEADER", aHeaderSignature);
    g_pExoResMan->WriteSignatureFile(sDirectory + "SAVE_HEADERVAR", aHeaderVarSignature);
    delete pERF;

    pERF = new CERFFile;
    sERFName = sDirectory + "REBOOT";
    pERF->Create(sERFName);
    pERF->SetVersion("MOD V1.0");
    pERF->WriteHeader();
    pERF->WriteStringTable();
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(60, 0);
    UpdateScreen(1.0f / 30.0f, 0, 0);
    hSignature = XCalculateSignatureBegin(0);
    pERF->ImportFiles("REBOOTDATA:", hSignature, UpdateLoadScreen, 60, 20);
    if (XCalculateSignatureEnd(hSignature, &cDataSignature) == ERROR_SUCCESS)
    {
        g_pExoResMan->WriteSignatureFile(sDirectory + "REBOOT_DATA", (unsigned char*)&cDataSignature);
    }
    pERF->Finish();
    pERF->Read(sERFName, aHeaderSignature);
    pERF->ReadHeaderVariance(sERFName, aHeaderVarSignature);
    g_pExoResMan->WriteSignatureFile(sDirectory + "REBOOT_HEADER", aHeaderSignature);
    g_pExoResMan->WriteSignatureFile(sDirectory + "REBOOT_HEADERVAR", aHeaderVarSignature);
    delete pERF;

    CExoString sPlayerInfo = sDirectory + "pifo";
    g_pExoResMan->FileCopy("TEMP:pifo", sPlayerInfo, RESTYPE_IFO, RESTYPE_INVALID);
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(85, 0);
    g_pAppManager->GetClientExoApp()->GetSWGuiManager()->Draw(1.0f / 30.0f);
    UpdateScreen(1.0f / 30.0f, 0, 0);
    unsigned char aPlayerSignature[XCALCSIG_SIGNATURE_SIZE];
    g_pExoResMan->CreateSignatureFile(sPlayerInfo, RESTYPE_IFO, 0, aPlayerSignature, 0);
    g_pExoResMan->WriteSignatureFile(sDirectory + "PLAYER_INFO", aPlayerSignature);
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(90, 0);
    g_pAppManager->GetClientExoApp()->GetSWGuiManager()->Draw(1.0f / 30.0f);
    UpdateScreen(1.0f / 30.0f, 0, 0);

    unsigned long nEndTime = g_pExoBase->m_pExoTimers->GetLowResolutionTimer();
    CExoString sSavePath = g_pExoBase->m_pExoAliasList->GetAliasPath("SAVES", 0);
    {
        int nFillerSize = (1600 - XGetDisplayBlocks(sSavePath.CStr())) << 14;
        CExoFile cFiller(sDirectory + "Filler", 2057, "wb+");
        cFiller.Seek(nFillerSize - 1, 0);
        char cZero = 0;
        cFiller.Write(&cZero, 1, 1);
        cFiller.Flush();
    }
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(100, 0);
    UpdateScreen(1.0f / 30.0f, 0, 0);
    if (nEndTime - nStartTime < 3000)
    {
        Sleep(3000 - (nEndTime - nStartTime));
    }
}

void CServerExoAppInternal::StallEventSaveGame()
{
    CExoString sSaveDirectory;
    CExoString sModuleName;
    CExoString sAreaName;
    CExoString sSaveInfoFile;
    CExoString sPortraitLabel;
    CSWParty* pParty;

    sModuleName = GetModule()->GetModuleName();
    int nAliasSeparator = sModuleName.Find(':', 0);
    if (nAliasSeparator != -1)
    {
        sModuleName = sModuleName.Right(sModuleName.GetLength() - nAliasSeparator - 1);
    }

    g_pAppManager->GetClientExoApp()->SetLoadScreenByModuleName(sModuleName);
    g_pAppManager->GetClientExoApp()->SetLoadScreenHint(41926);
    g_pAppManager->GetClientExoApp()->ShowLoadScreen(0, 1, 1);
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(0, 0);
    g_pAppManager->GetClientExoApp()->GetSWGuiManager()->Draw(1.0f / 30.0f);
    UpdateScreen(1.0f / 30.0f, 0, 0);
    unsigned long nStartTime = g_pExoBase->m_pExoTimers->GetLowResolutionTimer();

    CSWReentrantServerStats* pStats = g_pAppManager->m_pReentrantServerStats;
    CExoString sDirectory;
    CExoString sSaveFile;
    sDirectory = pStats->m_sUnknown24;
    sSaveFile = pStats->m_sUnknown32;
    sSaveDirectory = sDirectory + "\\";

    g_pAppManager->GetServerExoApp()->GetSWSMessage()
        ->SendServerToPlayerModule_SaveGameStatus(CSWSMessage::SAVEGAME_STATUS_STARTED);
    StoreCurrentModule();
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(5, 0);
    UpdateScreen(1.0f / 30.0f, 0, 0);
    m_partyTable.Save(sDirectory, 1);
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(10, 0);
    UpdateScreen(1.0f / 30.0f, 0, 0);
    unsigned char aPartySignature[XCALCSIG_SIGNATURE_SIZE];
    g_pExoResMan->CreateSignatureFile(sDirectory + "\\" + "PARTYTABLE", 0, 0, aPartySignature, 0);
    g_pExoResMan->WriteSignatureFile(sDirectory + "\\" + "SAVE_PARTY", aPartySignature);
    m_globalVariableTable.Save(sDirectory);
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(15, 0);
    UpdateScreen(1.0f / 30.0f, 0, 0);
    unsigned char aVarsSignature[XCALCSIG_SIGNATURE_SIZE];
    g_pExoResMan->CreateSignatureFile(sDirectory + "\\" + "GLOBALVARS", 0, 0, aVarsSignature, 0);
    g_pExoResMan->WriteSignatureFile(sDirectory + "\\" + "SAVE_VARS", aVarsSignature);

    CSWSCreature* pCreature = g_pAppManager->GetServerExoApp()->GetPlayerCreature();
    CSWSPlayer* pPlayer = GetClientObjectByObjectId(pCreature->GetId());
    if (g_bSavePlayerBIC)
    {
        GetModule()->SavePrimaryPlayerInfo(pCreature, sDirectory);
    }
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(20, 0);
    UpdateScreen(1.0f / 30.0f, 0, 0);

    sSaveInfoFile = sDirectory + "\\" + "savenfo";
    CResGFF* pGFF = new CResGFF;
    CResStruct stSaveInfo;
    pGFF->CreateGFFFile(&stSaveInfo, "NFO ", "V2.0");
    int nLanguage = pPlayer->m_nLanguage;
    CSWSArea* pArea = pCreature->GetArea();
    pArea->m_lsName.GetString(nLanguage, &sAreaName, 0);
    pGFF->WriteFieldCExoString(&stSaveInfo, sAreaName, "AREANAME");
    pGFF->WriteFieldCExoString(&stSaveInfo, sModuleName, "LASTMODULE");
    pGFF->WriteFieldDWORD(&stSaveInfo, m_partyTable.GetTotalPlayedSeconds(), "TIMEPLAYED");
    pGFF->WriteFieldBYTE(&stSaveInfo,
        g_pAppManager->GetClientExoApp()->GetGamePlayHint(), "GAMEPLAYHINT");
    pGFF->WriteFieldBYTE(&stSaveInfo,
        g_pAppManager->GetClientExoApp()->GetStoryHint(), "STORYHINT");

    pParty = g_pAppManager->GetClientExoApp()->GetSWParty();
    unsigned char nLiveContent = 0;
    CExoString sLiveContentLabel;
    for (int nContent = 1; nContent < CTlkTable::NUM_CONTENT_PKGS; ++nContent)
    {
        sLiveContentLabel.Format("LIVE%d", nContent);
        pGFF->WriteFieldCExoString(&stSaveInfo,
            g_pAppManager->GetClientExoApp()->GetLiveContentName(nContent),
            sLiveContentLabel.CStr());
        if (g_pExoBase->m_pExoAliasList->GetAliasPath(sLiveContentLabel, 0).GetLength() > 0)
        {
            nLiveContent |= 1 << (nContent - 1);
        }
    }
    pGFF->WriteFieldBYTE(&stSaveInfo, nLiveContent, "LIVECONTENT");
    for (int nMember = 0; nMember < pParty->m_nCharacters; ++nMember)
    {
        CSWSCreature* pServerCreature = pParty->GetCharacter(nMember)->GetServerCreature();
        if (pServerCreature != NULL)
        {
            sPortraitLabel.Format("%s%d", "PORTRAIT", nMember);
            CSWCCreature* pClientCreature = pServerCreature->GetClientCreature();
            if (pClientCreature != NULL)
            {
                pGFF->WriteFieldCResRef(&stSaveInfo, pClientCreature->GetPortrait(1),
                    sPortraitLabel.CStr());
            }
            else
            {
                pGFF->WriteFieldCResRef(&stSaveInfo, pServerCreature->GetPortrait(),
                    sPortraitLabel.CStr());
            }
        }
    }
    pGFF->WriteGFFFile(sSaveInfoFile.CStr(), 0);
    delete pGFF;
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(25, 0);
    UpdateScreen(1.0f / 30.0f, 0, 0);
    unsigned char aInfoSignature[XCALCSIG_SIGNATURE_SIZE];
    g_pExoResMan->CreateSignatureFile(sSaveInfoFile, 0, 0, aInfoSignature, 0);
    g_pExoResMan->WriteSignatureFile(sSaveDirectory + "SAVE_INFO", aInfoSignature);

    CERFFile* pERF = new CERFFile;
    pERF->Create(sSaveFile);
    pERF->SetVersion("MOD V1.0");
    pERF->WriteHeader();
    pERF->WriteStringTable();
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(30, 0);
    UpdateScreen(1.0f / 30.0f, 0, 0);
    HANDLE hSignature = XCalculateSignatureBegin(0);
    pERF->ImportFiles("GAMEINPROGRESS:", hSignature, UpdateLoadScreen, 30, 60);
    XCALCSIG_SIGNATURE cDataSignature;
    if (XCalculateSignatureEnd(hSignature, &cDataSignature) == ERROR_SUCCESS)
    {
        g_pExoResMan->WriteSignatureFile(sSaveDirectory + "SAVE_DATA", (unsigned char*)&cDataSignature);
    }
    pERF->Finish();
    unsigned char aHeaderSignature[XCALCSIG_SIGNATURE_SIZE];
    unsigned char aHeaderVarSignature[XCALCSIG_SIGNATURE_SIZE];
    pERF->Read(sSaveFile, aHeaderSignature);
    pERF->ReadHeaderVariance(sSaveFile, aHeaderVarSignature);
    g_pExoResMan->WriteSignatureFile(sSaveDirectory + "SAVE_HEADER", aHeaderSignature);
    g_pExoResMan->WriteSignatureFile(sSaveDirectory + "SAVE_HEADERVAR", aHeaderVarSignature);
    delete pERF;
    unsigned long nEndTime = g_pExoBase->m_pExoTimers->GetLowResolutionTimer();
    CExoString sSavePath = g_pExoBase->m_pExoAliasList->GetAliasPath("SAVES", 0);
    {
        int nFillerSize = (1600 - XGetDisplayBlocks(sSavePath.CStr())) << 14;
        CExoFile cFiller(sDirectory + "\\" + "Filler", 2057, "wb+");
        cFiller.Seek(nFillerSize - 1, 0);
        char cZero = 0;
        cFiller.Write(&cZero, 1, 1);
        cFiller.Flush();
    }
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(100, 0);
    UpdateScreen(1.0f / 30.0f, 0, 0);
    if (nEndTime - nStartTime < 3000)
    {
        Sleep(3000 - (nEndTime - nStartTime));
    }
    g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_GENERIC);
    g_pAppManager->GetClientExoApp()->HideLoadScreen(0);
    if (g_pAppManager->GetClientExoApp()->GetInGameGui()->m_pPartySelection == 0)
    {
        if (g_pAppManager->GetClientExoApp()->GetInGameGui()->m_nGlobalDialogState != 0)
        {
            g_pAppManager->GetClientExoApp()->GetInGameGui()->m_bHoldWorldFadeInForDialog = 1;
        }
        else
        {
            g_pAppManager->GetClientExoApp()->GetInGameGui()->StartGlobalFade(
                1, 0.5f, 1.0f, Vector(0.0f, 0.0f, 0.0f));
        }
    }
    g_pAppManager->GetClientExoApp()->SetCanSendDialog(1);
    g_pExoInput->EndAttemptedBind();
    g_pAppManager->GetClientExoApp()->GetInGameGui()->SetGlobalUnpauseableState(0);
    g_pAppManager->GetClientExoApp()->GetModule()->m_pInfo->StartSounds();
    if (pStats->m_bOperationEndPending != 1)
    {
        pStats->m_bOperationEndPending = 1;
        if (g_nClientServerMode == CLIENT_SERVER_MODE_SERVER)
        {
            pStats->m_bOperationEndPending = 0;
            pStats->m_bOperationActive = 0;
            pStats->m_nOperation = 0;
        }
        g_pAppManager->GetServerExoApp()->GetSWSMessage()
            ->SendServerToPlayerModule_SaveGameStatus(CSWSMessage::SAVEGAME_STATUS_COMPLETE);
        g_pAppManager->GetServerExoApp()->GetSWSMessage()
            ->SendServerToPlayerLoadBar_EndStallEvent(CSWSMessage::LOADBAR_STALL_EVENT_SAVE_GAME, 0);
    }
}

int CServerExoAppInternal::SaveGame(unsigned long nSlot,
                                    const CExoString& sName,
                                    CSWSPlayer* pPlayer, int bAutoSave)
{
    CExoString sDirectory;
    CExoString sSaveFile;
    unsigned __int64 nFreeBytes;

    g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_PAUSE_ALL);
    g_pExoResMan->CreateDirectory2("SAVES:");
    sDirectory.Format("SAVES:%06d - %s", nSlot, sName.CStr());
    if (g_pExoResMan->CreateDirectory2(sDirectory) == 0 &&
        g_pExoResMan->CleanDirectory(sDirectory, 0, 0) == 0)
    {
        return 0;
    }

    int nResult = g_pExoResMan->GetFreeDiskSpace(sDirectory, &nFreeBytes);
    if (nResult == 1)
    {
        if (m_nEstimatedSaveSize > nFreeBytes)
        {
            CSWSMessage* pMessage = m_pSWSMessage;
            pMessage->SendServerToPlayerSaveLoad_Status(
                pPlayer, CSWSMessage::SAVELOAD_STATUS_SAVE_FAILED,
                CSWSMessage::SAVELOAD_FAILURE_DISK_SPACE);
            return 0;
        }
    }

    sSaveFile = sDirectory + "\\" + "SAVEGAME";

    g_pAppManager->m_pReentrantServerStats->StartStallEvent(
        CSWReentrantServerStats::OPERATION_SAVE_GAME);
    CSWReentrantServerStats* pDirectoryStats = g_pAppManager->m_pReentrantServerStats;
    pDirectoryStats->m_sUnknown24 = sDirectory;
    CSWReentrantServerStats* pStats = g_pAppManager->m_pReentrantServerStats;
    pStats->m_sUnknown32 = sSaveFile;
    CSWReentrantServerStats* pCompletedStats = g_pAppManager->m_pReentrantServerStats;
    pCompletedStats->m_nProgress = 0;
    CSWReentrantServerStats* pProgressStats = g_pAppManager->m_pReentrantServerStats;
    pProgressStats->m_nProgressTotal = 0;
    SendStartStallEvent(CSWSMessage::LOADBAR_STALL_EVENT_SAVE_GAME);

    sDirectory = g_pExoBase->m_pExoAliasList->GetAliasPath("SAVES", 0);
    if (sDirectory != "")
    {
        CAurCamera* pModuleCamera =
            g_pAppManager->GetClientExoApp()->GetModule()->GetCamera();
        CSWCameraOnAStick* pCamera = (CSWCameraOnAStick*)
            pModuleCamera->GetBehavior(AUR_BEHAVIOR_CAMERA_ONASTICK);
        if (pCamera != NULL)
        {
            pCamera->SetCameraForScreenShot();
        }
        sSaveFile.Format("%s%06d - %s%sScreen", sDirectory.CStr(),
                         nSlot, sName.CStr(), "\\");
        g_pAppManager->DoSaveGameScreenShot(sSaveFile);
        sSaveFile.Format("SAVES:%06d - %s%sScreen", nSlot, sName.CStr(), "\\");
        unsigned char aSignature[XCALCSIG_SIGNATURE_SIZE];
        g_pExoResMan->CreateSignatureFile(sSaveFile, RESTYPE_TGA, 0, aSignature, 0);
        g_pExoResMan->WriteSignatureFile(sSaveFile, aSignature);
    }

    g_pAppManager->GetClientExoApp()->GetSWGuiManager()->Draw(1.0f / 30.0f);
    UpdateScreen(1.0f / 30.0f, 0, 0);
    m_nAutoSaveElapsedTime = 0;
    m_bAutoSaveTimerExpired = 0;
    return 1;
}

#include "nwsparty.h"

int CServerExoAppInternal::LoadCharacterFinish(CSWSPlayer* pPlayer, int bSuccess)
{
    CSWSModule* pModule = g_pAppManager->m_pServerExoApp->GetModule();
    CScriptEvent* pEnterEvent = new CScriptEvent;
    CScriptEvent* pDeathEvent = NULL;
    int nHead;

    char nAreaLoadState = pPlayer->m_nAreaLoadState;
    if (nAreaLoadState != 1)
    {
        pPlayer->m_nAreaLoadState = CSWSPlayer::AREA_LOAD_PENDING;
        m_pSWSMessage->SendServerToPlayerLogin_Confirm(pPlayer->GetPlayerId());
        m_pSWSMessage->SendServerToPlayerModule_Info(pPlayer->GetPlayerId());
    }
    if (bSuccess != 0 && pPlayer->m_nModulePlayerIndex != -1)
    {
        if (g_pAppManager->GetServerExoApp()->GetServerInfo()->m_nXboxUnmapped0D0 == 1 &&
            pPlayer->GetGameObject() != NULL)
        {
            CSWSCreature* pOldCreature = pPlayer->GetGameObject()->AsSWSCreature();
            if (pOldCreature != NULL && pOldCreature->GetFaction() != NULL)
            {
                pOldCreature->GetFaction()->RemoveMember(pOldCreature->GetId());
            }
        }
        CSWSObject* pObject = pPlayer->GetGameObject();
        if (pObject != NULL)
            delete pObject;
        pPlayer->LoadCharacterFromIFO(pPlayer->m_nModulePlayerIndex, 1, 1);
    }

    pEnterEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_CLIENT_ENTER;
    if (pPlayer->GetGameObject() != NULL)
    {
        if (pPlayer->GetGameObject()->AsSWSCreature()->GetDead() == 1 ||
            pPlayer->GetGameObject()->AsSWSCreature()->GetDeadTemp() == 1)
        {
            pDeathEvent = new CScriptEvent;
            pDeathEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_DEATH;
        }
        pEnterEvent->SetObjectID(0, pPlayer->GetGameObject()->GetId());

        CSWSMessage* pMessage =
            g_pAppManager->m_pServerExoApp->GetSWSMessage();
        CExoLinkedList<CSWSClient>* pPlayers =
            g_pAppManager->m_pServerExoApp->GetPlayerList();
        CSWSCreature* pCreature =
            pPlayer->GetGameObject()->AsSWSCreature();
        CExoLinkedListNode* pNode = pPlayers->GetHeadPos();
        while (pNode != NULL)
        {
            CSWSPlayer* pOtherPlayer = (CSWSPlayer*)pPlayers->GetAtPos(pNode);
            if (pOtherPlayer != NULL &&
                pOtherPlayer->m_nPlayModuleCharacterListState != 0 &&
                pMessage != NULL)
                pMessage->SendServerToPlayerPlayModuleCharacterListResponse(
                    pOtherPlayer->GetPlayerId(), pCreature->GetId(), 1);
            pPlayers->GetNext(pNode);
        }
    }
    else
    {
        pEnterEvent->SetObjectID(0, OBJECT_INVALID);
    }

    if (pModule != NULL)
    {
        g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
            0, 0, pPlayer->GetPlayerId(), pModule->GetId(),
            CServerAIMaster::EVENT_SIGNAL_EVENT, pEnterEvent);
        if (pDeathEvent != NULL)
            g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                0, 0, pPlayer->GetGameObject()->GetId(), pModule->GetId(),
                CServerAIMaster::EVENT_SIGNAL_EVENT, pDeathEvent);

        unsigned long oidPC = pPlayer->m_oidPCObject;
        CSWSCreature* pCreature =
            g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(oidPC);
        if (pCreature != NULL)
        {
            if (&pCreature->m_cAppearanceInfo != NULL)
            {
                g_pRules->Get2DArrays()->GetAppearance()->GetINTEntry(
                    pCreature->m_cAppearanceInfo.m_nAppearance, "NORMALHEAD", &nHead);
                pModule->GetArea()->EnforceNoPCHeadCloning(nHead);
            }
        }
    }
    return 1;
}

#include <stdlib.h>
#include "aurconsole.h"

extern float g_fInjuredLevel;
ConsoleFunc ConVg_bSavePlayerBIC("g_bSavePlayerBIC", &g_bSavePlayerBIC);
ConsoleFunc ConVg_fInjuredLevel("g_fInjuredLevel", &g_fInjuredLevel);
ConsoleFunc ConVRenderValidatePath("RenderValidatePath", &RenderValidatePath);
ConsoleFunc ConVOverrideRenderValidatePath("OverrideRenderValidatePath", &OverrideRenderValidatePath);
ConsoleFunc ConVfFloorHeight("fFloorHeight", &fFloorHeight);
ConsoleFunc ConVfRoofHeight("fRoofHeight", &fRoofHeight);
ConsoleFunc ConVEnablePathValidation("EnablePathValidation", &EnablePathValidation);

static char* warpforward(char* szValue)
{
    float fDistance = (float)atof(szValue);
    if (fDistance <= 0.0f)
    {
        fDistance = 5.0f;
    }
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    if (pServer != NULL)
    {
        CSWSPlayer* pPlayer = pServer->GetClientObjectByObjectId(pServer->GetPlayerCreatureId());
        if (pPlayer != NULL)
        {
            CSWSObject* pObject = pPlayer->GetGameObject();
            if (pObject != NULL && pObject->GetArea() != NULL)
            {
                Vector vPosition = pObject->m_vPosition;
                Vector vOrientation = pObject->m_vOrientation;
                vPosition += vOrientation * fDistance;
                unsigned long oidArea = pObject->GetArea()->GetId();
                int bRun = 1;
                float fZero = 0.0f;
                pObject->AddActionToFront(CSWSObject::ACTION_JUMP_TO_POINT,
                    CSWSObject::ACTION_GROUP_NEW,
                    CSWSObject::ACTION_PARAM_FLOAT, &vPosition.m_fX,
                    CSWSObject::ACTION_PARAM_FLOAT, &vPosition.m_fY,
                    CSWSObject::ACTION_PARAM_FLOAT, &vPosition.m_fZ,
                    CSWSObject::ACTION_PARAM_OBJECT, &oidArea,
                    CSWSObject::ACTION_PARAM_INTEGER, &bRun,
                    CSWSObject::ACTION_PARAM_FLOAT, &fZero,
                    CSWSObject::ACTION_PARAM_FLOAT, &vOrientation.m_fX,
                    CSWSObject::ACTION_PARAM_FLOAT, &vOrientation.m_fY,
                    0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL);
                return "Success!";
            }
        }
    }
    return "Failure!";
}
ConsoleFunc ConFwarpforward("warpforward", warpforward);

int CServerExoAppInternal::TestSafeLocation(
    unsigned long oidObject, Vector vPosition)
{
    CGameObjectArray* pObjectArray = m_pObjectArray;
    CGameObject* pObject = NULL;
    CSWSCreature* pCreature;

    if (pObjectArray->GetGameObject(oidObject, &pObject) !=
        CGameObjectArray::SUCCESS)
    {
        return 0;
    }
    if (pObject == NULL)
    {
        return 0;
    }
    if (pObject->m_nObjectType != CGameObject::OBJECT_TYPE_CREATURE)
    {
        return 0;
    }
    pCreature = pObject->AsSWSCreature();
    if (pCreature == NULL)
    {
        return 0;
    }
    CPathfindInformation* pPathfind = pCreature->m_pPathfindInformation;
    unsigned long oidArea = pCreature->m_oidArea;
    return GetAreaByGameObjectID(oidArea)
        ->TestSafeLocationPoint(vPosition, pPathfind);
}

int CServerExoAppInternal::ComputeSafeLocation(
    Vector vPosition, float fRadius, unsigned long oidObject,
    int nFlags, Vector* pvSafe)
{
    CGameObjectArray* pObjectArray = m_pObjectArray;
    CGameObject* pObject = NULL;
    CSWSCreature* pCreature = NULL;

    if (pObjectArray->GetGameObject(oidObject, &pObject) ==
            CGameObjectArray::SUCCESS &&
        pObject != NULL &&
        pObject->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
    {
        pCreature = ((CSWSObject*)pObject)->AsSWSCreature();
    }
    if (pCreature == NULL)
    {
        return 0;
    }
    CPathfindInformation* pPathfind = pCreature->m_pPathfindInformation;
    return GetAreaByGameObjectID(pCreature->m_oidArea)->ComputeSafeLocation(
        vPosition, fRadius, pPathfind, nFlags, pvSafe);
}

Vector CServerExoAppInternal::GetNormalOfPoint(Vector vPosition)
{
    CSWSModule* pModule;
    CSWSRoom* pRoom;
    int nFace;

    pModule = GetServerExoApp()->GetModule();
    pRoom = pModule->GetArea()->GetRoom(vPosition, &nFace);
    if (pRoom != NULL)
    {
        return pRoom->GetNormalOfPoint(nFace);
    }
    return Vector(0.0f, 0.0f, 1.0f);
}

int CServerExoAppInternal::DebugValidatePath(unsigned long oidCreature,
                                             int bTempWayPoints)
{
    int bValid = 1;

    if (EnablePathValidation != 0)
    {
        CSWSCreature* pCreature =
            GetServerExoApp()->GetCreatureByGameObjectID(oidCreature);
        if (pCreature == NULL)
        {
            return 0;
        }
        CSWSArea* pArea = pCreature->GetArea();
        if (pArea == NULL)
        {
            return 0;
        }
        CPathfindInformation* pPathfind = pCreature->m_pPathfindInformation;
        if (pPathfind == NULL)
        {
            return 0;
        }

        int nCount;
        float* pfPoints;
        if (bTempWayPoints != 0)
        {
            nCount = pPathfind->m_nField_94;
            pfPoints = pPathfind->m_pWayPointsTemp;
        }
        else
        {
            nCount = pPathfind->m_nWayPointCount;
            pfPoints = pPathfind->m_pWayPoints;
        }
        if (nCount == 0 || pfPoints == NULL)
        {
            return 1;
        }

        Vector vPoint(pfPoints[0], pfPoints[1], 0.0f);
        vPoint.m_fZ = pArea->ComputeHeight(vPoint, 1, NULL);
        pArea->m_pPathfindInformation = pPathfind;
        bValid &= DebugValidatePath_Point(pArea, vPoint);
        Vector vLast = vPoint;

        for (int nPoint = 2; nPoint < nCount; nPoint += 2)
        {
            vPoint = Vector(pfPoints[nPoint], pfPoints[nPoint + 1], 0.0f);
            vPoint.m_fZ = pArea->ComputeHeight(vPoint, 1, NULL);
            bValid &= DebugValidatePath_Point(pArea, vPoint);
            bValid &= DebugValidatePath_Segment(pArea, pPathfind, vLast, vPoint);
            vLast = vPoint;
        }
        pArea->m_pPathfindInformation = pPathfind;
    }
    return bValid;
}

int CServerExoAppInternal::DebugValidatePath_Point(CSWSArea* pArea,
                                                   Vector vPoint)
{
    int bValid = 1;

    if (pArea->GetRoom(vPoint, NULL) == NULL)
    {
        bValid = 0;
    }
    if (pArea->PositionWalkable(vPoint) == 0)
    {
        bValid = 0;
    }
    return bValid;
}

int CServerExoAppInternal::DebugValidatePath_Segment(
    CSWSArea* pArea, CPathfindInformation* pPathfind, Vector vStart,
    Vector vEnd)
{
    int bValid = 0;
    CWalkHitInfo cHitInfo;
    float fStepScale = pPathfind->m_fStepScale;
    float fHalfStep = pPathfind->m_fHalfStep;
    int nResult = pArea->TestDirectLine(vStart, vEnd, fStepScale,
                                        fHalfStep, 1, &cHitInfo);
    if (nResult > 0 || nResult == CSWSArea::DIRECT_LINE_BLOCKED_CREATURE)
    {
        return 1;
    }
    if (nResult != CSWSArea::DIRECT_LINE_BLOCKED_OBJECT)
    {
        return 0;
    }

    CGameObject* pObject = GetGameObject(cHitInfo.m_oidBlocker);
    if (pObject == NULL)
    {
        return 1;
    }
    if (pObject->AsSWSDoor() != NULL)
    {
        if (pObject->AsSWSDoor()->m_bStatic == 0)
        {
            nResult = CSWSArea::DIRECT_LINE_CLEAR;
            bValid = 1;
        }
    }
    if (pObject->AsSWSPlaceable() != NULL &&
        pObject->AsSWSPlaceable()->m_nId ==
            pPathfind->m_oidPathfindTargetCreature)
    {
        return 1;
    }
    if (nResult == CSWSArea::DIRECT_LINE_BLOCKED_OBJECT)
    {
        bValid = 1;
    }
    return bValid;
}
#endif
// PC 0x004EE510 GOG 0x004EF290
void CServerExoAppInternal::UpdateClientGameObjects(int bFlag)
{
    unsigned __int64 nTime;
    CExoLinkedListNode* pNode;
    CSWSPlayer* pPlayer;

    nTime = g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
    pNode = m_pPlayerList->GetHeadPos();
    while (pNode != NULL)
    {
        pPlayer = (CSWSPlayer*)m_pPlayerList->GetAtPos(pNode);
        UpdateClientGameObjectsForPlayer(pPlayer, bFlag, nTime);
        m_pPlayerList->GetNext(pNode);
    }
}
// PC 0x004EE580 GOG 0x004EC870
int CServerExoAppInternal::GetPlayerLanguage(unsigned long nPlayerId)
{
    CExoLinkedListNode* pNode;
    CSWSPlayer* pPlayer;

    pNode = m_pPlayerList->GetHeadPos();
    while (pNode != NULL)
    {
        pPlayer = (CSWSPlayer*)m_pPlayerList->GetAtPos(pNode);
        if (pPlayer != NULL && pPlayer->m_nPlayerId == nPlayerId)
        {
            return pPlayer->m_nLanguage;
        }
        m_pPlayerList->GetNext(pNode);
    }
    return 0;
}
// PC 0x004EE5F0 GOG 0x004EC9D0
CExoString CServerExoAppInternal::GetPlayerListString()
{
    CExoString sList;
    CExoString sPlayerName;
    CExoLinkedListNode* pNode;
    CSWSPlayer* pPlayer;
    int nCount;
    CNetLayerPlayerInfo* pPlayerInfo;
    unsigned long nPlayerId;
    CServerExoApp* pServer;

    nCount = m_pSysAdminList->Count();
    sList = sList + "ServerAdmins: " + nCount;
    pNode = m_pSysAdminList->GetHeadPos();
    while (pNode != NULL)
    {
        pPlayer = (CSWSPlayer*)m_pSysAdminList->GetAtPos(pNode);
        pServer = g_pAppManager->m_pServerExoApp;
        nPlayerId = pPlayer->m_nPlayerId;
        pPlayerInfo = pServer->GetNetLayer()
            ->GetPlayerInfo(nPlayerId);
        sPlayerName = pPlayerInfo->m_sPlayerName.CStr();
        sList = sList + "\n" + sPlayerName;
        sList = sList + " [ServerAdmin]";
        m_pSysAdminList->GetNext(pNode);
    }
    sList = sList + "\n";
    nCount = m_pPlayerList->Count();
    sList = sList + "Players: " + nCount;
    pNode = m_pPlayerList->GetHeadPos();
    while (pNode != NULL)
    {
        pPlayer = (CSWSPlayer*)m_pPlayerList->GetAtPos(pNode);
        pServer = g_pAppManager->m_pServerExoApp;
        nPlayerId = pPlayer->m_nPlayerId;
        pPlayerInfo = pServer->GetNetLayer()
            ->GetPlayerInfo(nPlayerId);
        sPlayerName = pPlayerInfo->m_sPlayerName.CStr();
        sList = sList + "\n" + sPlayerName;
        sList = sList + " [Player]";
        m_pPlayerList->GetNext(pNode);
    }
    sList = sList + "\n";
    int nAdminCount = m_pSysAdminList->Count();
    int nPlayerCount = m_pPlayerList->Count();
    sList = sList + "Total: " + (nPlayerCount + nAdminCount);
    return sList;
}
// PC 0x004EEB60 GOG 0x004ECF40
void CServerExoAppInternal::AddToExclusionList(unsigned long oidObject,
                                              unsigned char nType)
{
    CExoLinkedListNode* pNode;

    if (nType == 0)
    {
        return;
    }
    if ((nType & PAUSE_TYPE_COMBAT_MASK) != 0)
    {
        pNode = m_pTimestopExclusionList->GetHeadPos();
        while (pNode != NULL)
        {
            if ((unsigned long)m_pTimestopExclusionList->GetAtPos(pNode) ==
                oidObject)
            {
                return;
            }
            m_pTimestopExclusionList->GetNext(pNode);
        }
        m_pTimestopExclusionList->AddTail((unsigned long*)oidObject);
    }
    else if ((nType & PAUSE_TYPE_PAUSED_MASK) != 0)
    {
        pNode = m_pPauseExclusionList->GetHeadPos();
        while (pNode != NULL)
        {
            if ((unsigned long)m_pPauseExclusionList->GetAtPos(pNode) == oidObject)
            {
                return;
            }
            m_pPauseExclusionList->GetNext(pNode);
        }
        m_pPauseExclusionList->AddTail((unsigned long*)oidObject);
    }
}
// PC 0x004EEC30 GOG 0x004ED010
void CServerExoAppInternal::RemoveFromExclusionList(unsigned long oidObject,
                                                   unsigned char nType)
{
    CExoLinkedListNode* pNode;

    if (nType == 0)
    {
        return;
    }
    if ((nType & PAUSE_TYPE_PAUSED_MASK) != 0)
    {
        pNode = m_pPauseExclusionList->GetHeadPos();
        while (pNode != NULL)
        {
            if ((unsigned long)m_pPauseExclusionList->GetAtPos(pNode) == oidObject)
            {
                m_pPauseExclusionList->Remove(pNode);
                return;
            }
            m_pPauseExclusionList->GetNext(pNode);
        }
    }
    else if ((nType & PAUSE_TYPE_COMBAT_MASK) != 0)
    {
        pNode = m_pTimestopExclusionList->GetHeadPos();
        while (pNode != NULL)
        {
            if ((unsigned long)m_pTimestopExclusionList->GetAtPos(pNode) ==
                oidObject)
            {
                m_pTimestopExclusionList->Remove(pNode);
                return;
            }
            m_pTimestopExclusionList->GetNext(pNode);
        }
    }
}
// PC 0x004EED10 GOG 0x004ED0F0
int CServerExoAppInternal::IsOnActiveExclusionList(unsigned long oidObject)
{
    CExoLinkedList<unsigned long>* pList = GetActiveExclusionList();
    CExoLinkedListNode* pNode;

    if (pList == NULL)
    {
        return 1;
    }
    pNode = pList->GetHeadPos();
    while (pNode != NULL)
    {
        if ((unsigned long)pList->GetAtPos(pNode) == oidObject)
        {
            return 1;
        }
        pList->GetNext(pNode);
    }
    return 0;
}
// PC 0x004EED90 GOG 0x004ED170
int CServerExoAppInternal::ResolvePlayerByFirstName(const CExoString& sFirstName)
{
    CExoString sPlayerName;
    CExoLinkedListNode* pNode;
    CSWSPlayer* pPlayer;
    CSWSObject* pObject;
    CSWSCreature* pCreature;
    unsigned long nPlayerId;

    pNode = m_pPlayerList->GetHeadPos();
    while (pNode != NULL)
    {
        pPlayer = (CSWSPlayer*)m_pPlayerList->GetAtPos(pNode);
        if (pPlayer != NULL)
        {
            pObject = pPlayer->GetGameObject();
            if (pObject != NULL)
            {
                pCreature = pObject->AsSWSCreature();
                if (pCreature != NULL)
                {
                    CSWSMessage::GetLocStringServer(
                        nPlayerId = pPlayer->m_nPlayerId, *pCreature->GetFirstName(),
                        CExoLocString(), sPlayerName, 0);
                    if (sPlayerName == sFirstName)
                    {
                        return pPlayer->m_nPlayerId;
                    }
                }
            }
        }
        m_pPlayerList->GetNext(pNode);
    }
    return CGameObjectArray::INVALIDOBJID;
}
// PC 0x004EEEE0 GOG 0x004ED2C0
int CServerExoAppInternal::UpdateShutdownTimer(unsigned __int64 nTime)
{
    if (m_nShutdownTimeout == 0)
    {
        return 0;
    }

    if (nTime - m_nShutdownStartTime > m_nShutdownTimeout)
    {
        m_nShutdownTimeout = 0;
        return 1;
    }

    if ((m_nShutdownTimeout > SHUTDOWN_FIRST_WARNING_TIME &&
         m_nShutdownTimeout - (nTime - m_nShutdownStartTime) < SHUTDOWN_FIRST_WARNING_TIME) ||
        (m_nShutdownTimeout > SHUTDOWN_SECOND_WARNING_TIME &&
         m_nShutdownTimeout - (nTime - m_nShutdownStartTime) < SHUTDOWN_SECOND_WARNING_TIME))
    {
        for (int nPlayer = 0; nPlayer < m_lstPendingPlayers.num; nPlayer++)
        {
            m_pSWSMessage->SendServerToPlayerShutDownServer(
                m_lstPendingPlayers[nPlayer],
                (unsigned long)((m_nShutdownTimeout - (nTime - m_nShutdownStartTime)) /
                                SHUTDOWN_MICROSECONDS_PER_SECOND));
        }
    }

    m_nShutdownTimeout -= nTime - m_nShutdownStartTime;
    m_nShutdownStartTime = nTime;
    return 0;
}
// PC 0x004EF080 GOG 0x004ED460
int CServerExoAppInternal::GetFirstPCObject()
{
    CSWSPlayer* pPlayer;

    m_pPCListPosition = m_pPlayerList->GetHeadPos();
    if (m_pPCListPosition != NULL)
    {
        pPlayer = (CSWSPlayer*)m_pPlayerList->GetAtPos(m_pPCListPosition);
        if (pPlayer != NULL)
        {
            return pPlayer->m_oidPCObject;
        }
    }
    return CGameObjectArray::INVALIDOBJID;
}

int CServerExoAppInternal::GetNextPCObject()
{
    CSWSPlayer* pPlayer;

    if (m_pPCListPosition != NULL)
    {
        pPlayer = (CSWSPlayer*)m_pPlayerList->GetNext(m_pPCListPosition);
        if (pPlayer != NULL)
        {
            return pPlayer->m_oidPCObject;
        }
    }
    return CGameObjectArray::INVALIDOBJID;
}
// PC 0x004EF0B0 GOG 0x004ED4D0
int CServerExoAppInternal::IsActionsPending(unsigned long oidObject)
{
    unsigned long oidNormalized;

    if (oidObject != OBJECT_INVALID)
    {
        oidNormalized = oidObject & ~CGameObjectArray::LISTTYPE_MASK;
    }
    else
    {
        oidNormalized = OBJECT_INVALID;
    }

    CSWSCreature* pCreature = GetCreatureByGameObjectID(oidNormalized);

    if (pCreature != NULL)
    {
        return pCreature->m_lstActions.GetHeadPos() != NULL;
    }
    return 0;
}
// PC 0x004EF130 GOG 0x004EF6E0
void CServerExoAppInternal::TempDeathUpdate()
{
    SERVEREXOAPPINTERNAL_HOOK();
    if (g_pAppManager->GetClientExoApp()->GetGameOver() ||
        g_pAppManager->GetClientExoApp()->GetAreaNotReadyToDisplay())
    {
        return;
    }
    int nModalPanels = g_pAppManager->GetClientExoApp()->GetSWGuiManager()->m_lModalPanels.num;
    if (nModalPanels > 0)
    {
        return;
    }

    unsigned long nTime = (unsigned long)(
        g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000);
    unsigned long nElapsed = nTime - m_nLastTempDeathUpdate;
    m_nLastTempDeathUpdate = nTime;
    int bAllDead = 1;
    int bAnyDead = 0;
    int bSafe = 1;
    int nCharacters = g_pAppManager->GetClientExoApp()->GetSWParty()->m_nCharacters;
    CSWSCreature* pCreature;
    for (int nMember = 0; nMember < nCharacters; ++nMember)
    {
        CSWCCreature* pCharacter =
            g_pAppManager->GetClientExoApp()->GetSWParty()->GetCharacter(nMember);
        if (pCharacter != NULL)
        {
            pCreature = pCharacter->GetServerCreature();
            if (pCreature != NULL && pCreature->GetDeadTemp())
            {
                bAnyDead = 1;
                bAllDead &= 1;
            }
            else
            {
                bAllDead = 0;
            }
        }
    }
    if (bAllDead && nCharacters != 0)
    {
        RunDeathSequence();
    }
    if (bAnyDead)
    {
        m_nTempDeathCheckElapsed += nElapsed;
        if (m_nTempDeathCheckElapsed > TEMP_DEATH_CHECK_INTERVAL)
        {
            m_nTempDeathCheckElapsed = 0;
            CSWSArea* pArea = pCreature->GetArea();
            CGameObjectArray* pObjectArray =
                g_pAppManager->GetServerExoApp()->GetObjectArray();
            if (pArea != NULL && pObjectArray != NULL)
            {
                unsigned long oidObject;
                CGameObject* pObject;
                pArea->GetFirstObjectInArea(oidObject);
                do
                {
                    pObjectArray->GetGameObject(oidObject, &pObject);
                    if (pObject != NULL && pObject->AsSWSCreature() != NULL &&
                        pObject->AsSWSCreature()->m_bInParty == 0)
                    {
                        CSWSCreature* pOther = pObject->AsSWSCreature();
                        if (pOther->GetReputation(pCreature->GetId(), 0) <=
                                CSWSObject::MAX_HOSTILE_REPUTATION &&
                            pOther->IsPartyVisible())
                        {
                            bSafe = 0;
                        }
                    }
                } while (pArea->GetNextObjectInArea(oidObject) && bSafe);
            }
        }
    }
    if (bAnyDead && bSafe)
    {
        m_nTempDeathRecoveryElapsed += nElapsed;
    }
    else
    {
        m_nTempDeathRecoveryElapsed = 0;
    }

    if (m_nTempDeathRecoveryElapsed > TEMP_DEATH_RECOVERY_DELAY)
    {
        Vector vLeader = g_pAppManager->GetClientExoApp()->GetSWParty()
            ->GetCharacter(0)->GetServerCreature()->m_vPosition;
        for (int nMember = 0; nMember < nCharacters; ++nMember)
        {
            CSWSCreature* pMember = g_pAppManager->GetClientExoApp()->GetSWParty()
                ->GetCharacter(nMember)->GetServerCreature();
            if (pMember != NULL)
            {
                if (pMember->GetDeadTemp())
                {
                    Vector vPosition = pMember->m_vPosition;
                    g_pAppManager->GetServerExoApp()->ComputeSafeLocation(
                        vPosition, SERVER_TEMP_DEATH_SAFE_RADIUS,
                        pMember->GetId(), 0, &vPosition);
                    pMember->SetPosition(vPosition, 1, 1, 0);
                    CGameEffect* pEffect = new CGameEffect(1);
                    pEffect->m_nType = CSWSEffectListHandler::EFFECT_RESURRECTION;
                    pEffect->m_nSubType = CGameEffect::DURATION_TYPE_INSTANTANEOUS |
                        (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK);
                    pMember->ApplyEffect(pEffect, 0, 0);
                    pMember->m_nF0 = 1;
                }
                if (g_pAppManager->GetServerExoApp()->GetPartyTable()->m_bSoloMode == 0)
                {
                    Vector vDistance = pMember->m_vPosition - vLeader;
                    if (vDistance * vDistance > SERVER_TEMP_DEATH_MAX_DISTANCE_SQUARED)
                    {
#ifdef _XBOX
                        CSWParty* pMemberParty = g_pAppManager->GetClientExoApp()->GetSWParty();
                        EXOXBOXASSERT(nMember >= 0 && nMember < 3);
                        Vector vPosition = pMemberParty
                            ->m_aPartyMemberData[nMember].m_vFormationPosition;
#else
                        Vector vPosition = g_pAppManager->GetClientExoApp()->GetSWParty()
                            ->m_aPartyMemberData[nMember].m_vFormationPosition;
#endif
                        g_pAppManager->GetServerExoApp()->ComputeSafeLocation(
                            vPosition, SERVER_TEMP_DEATH_SAFE_RADIUS,
                            pMember->GetId(), 0, &vPosition);
                        pMember->SetPosition(vPosition, 1, 1, 0);
                    }
                }
            }
        }
    }
}
// PC 0x0040FAD0 GOG 0x0040FA80
template <> inline CExoArrayList<CExoString>::CExoArrayList(const CExoArrayList<CExoString>& list)
{
    num = 0;
    array_size = 0;
    element = NULL;
    for (int i = 0; i < list.num; i++)
    {
        Add(list[i]);
    }
}

#ifndef _XBOX
template <> inline void CExoArrayList<CExoString>::Add(CExoString value)  // PC 0x00405A30 GOG 0x00405A30
{
    if (num == array_size)
    {
        Allocate(array_size != 0 ? array_size * 2 : EXOARRAYLIST_INITIAL_ALLOCATION);
    }
    element[num++] = value;
}
#endif
// PC 0x004EF5C0 GOG 0x004ED830
void CServerExoAppInternal::ForceRunAreaOnExitScripts()
{
    CExoLinkedListNode* pNode;
    CSWSPlayer* pPlayer;
    CSWSObject* pObject;
    CSWSCreature* pCreature;
    CSWSArea* pArea;

    pNode = m_pPlayerList->GetHeadPos();
    while (pNode != NULL)
    {
        pPlayer = m_pPlayerList->GetAtPos(pNode)->AsSWSPlayer();
        if (pPlayer != NULL)
        {
            pObject = pPlayer->GetGameObject();
            if (pObject != NULL)
            {
                pCreature = pObject->AsSWSCreature();
                if (pCreature != NULL)
                {
                    pArea = ((CSWSObject*)pCreature)->GetArea();
                    if (pArea != NULL)
                    {
                        pArea->m_oidExitingObject =
                            ((CSWSObject*)pCreature)->m_nId;
                        g_pVirtualMachine->RunScript(&pArea->m_sScripts[CSWSArea::SCRIPT_ON_EXIT],
                                                     pArea->m_nId, 1);
                    }
                }
            }
        }
        m_pPlayerList->GetNext(pNode);
    }
}
// PC 0x004EF6A0 GOG 0x004EDEF0
CServerExoAppInternal::CServerExoAppInternal()
{
    unsigned long nOption;

    m_nShutdownTimeout = 0;
    m_nDebugMode = 0;
    m_pServerInfo = NULL;
    m_nServerMode = 0;
    m_oidModule = CGameObjectArray::INVALIDOBJID;
    m_pPlayerList = new CExoLinkedList<CSWSClient>;
    m_pSysAdminList = new CExoLinkedList<CSWSClient>;
    m_bForceUpdate = 0;
    m_pSWSMessage = NULL;
    EXOXBOXASSERT(s_pServerExoAppInternal == NULL);
    s_pServerExoAppInternal = this;

    m_pServerOptions = new unsigned long[16];
    for (nOption = 0; nOption < 16; nOption++)
    {
        m_pServerOptions[nOption] = CNetLayer::PLAYERID_INVALIDID;
    }

    m_PauseState.m_nPauseState = 0;
    m_bDoingStartNewModule = 0;
    m_bMoveToModulePending = 0;
    m_sMoveToModule = "";
    m_bLoadingModule = 0;
    m_bAutoSavePending = 0;
    m_bExportAllCharactersPending = 0;
    m_bNeedCharacterPending = 0;
    m_pTimestopExclusionList = new CExoLinkedList<unsigned long>;
    m_pPauseExclusionList = new CExoLinkedList<unsigned long>;
    m_nEstimatedSaveSize = 0;
    m_pPCListPosition = NULL;
    m_sLastConversation = "";
    m_nActionConversation = 0;
    m_bIgnoreConversationStartRange = 0;
    m_bLoadFromSaveGame = 0;
    m_bButtonMashCheck = 0;
    m_oidLastModuleFound = OBJECT_INVALID;
    m_pLastModuleFound = NULL;
    m_oidAreaCache = OBJECT_INVALID;
    m_pAreaCache = NULL;
    m_nMoveToPausedCalendarDay = 0;
    m_nMoveToPausedTimeOfDay = 0;
    m_nAutoSaveElapsedTime = 0;
    m_nAutoSaveCalendarDay = 0;
    m_nAutoSaveTimeOfDay = 0;
    m_bAutoSaveTimerExpired = 0;
    m_nAutoSaveLastTime = 0;
#ifndef _XBOX
    m_sSaveGameName = "";
#endif
}

void CServerExoAppInternal::SendStartStallEvent(unsigned long nEvent)
{
    CSWSMessage* pMessage = GetServerExoApp()->GetSWSMessage();
    pMessage->SendServerToPlayerLoadBar_StartStallEvent(nEvent);
}
// PC 0x004EF960 GOG 0x004EFDB0
int CServerExoAppInternal::LoadCharacterStart(
    unsigned char nType, CSWSPlayer* pPlayer, CResRef cResRef,
    void* pData, unsigned long nSize)
{
    SERVEREXOAPPINTERNAL_HOOK();
    CSWSModule* pModule = g_pAppManager->GetServerExoApp()->GetModule();
    CExoString sPlayerName;
    CExoArrayList<CExoString> lstDirectories;
    CExoString sDirectory;

    if (nType == CSWSMessage::LOGIN_SERVER_CHARACTER)
    {
        if (pPlayer->GetGameObject() == NULL)
        {
            pPlayer->LoadServerCharacter(cResRef);
        }
    }
    else if (nType == CSWSMessage::LOGIN_MODULE_CHARACTER)
    {
        if (pPlayer->GetGameObject() == NULL)
        {
            CExoString sPlayerIndex;
            cResRef.CopyToString(sPlayerIndex);
            pPlayer->LoadCharacterFromIFO(sPlayerIndex.AsINT(), 0, 1);
        }
    }
    else if (nType == CSWSMessage::LOGIN_SERVER_PLAYER_CHARACTER)
    {
        if (pPlayer->GetGameObject() == NULL)
        {
            sPlayerName = pPlayer->GetPlayerName();
            g_pExoBase->GetDirectoryList(&lstDirectories, "SERVERVAULT:",
                                         RESTYPE_INVALID, 1, 0);
            if (lstDirectories.Contains(sPlayerName))
            {
                sDirectory.Format("SERVERVAULT:%s", sPlayerName.CStr());
                g_pExoResMan->AddResourceDirectory(sDirectory);
                pPlayer->LoadServerCharacter(cResRef);
                pPlayer->m_nCharacterType = CHARACTER_TYPE_SERVER_PLAYER;
                g_pExoResMan->RemoveResourceDirectory(sDirectory);
            }
        }
    }
    else if (nType == CSWSMessage::LOGIN_DEFAULT_MODULE_CHARACTER)
    {
        pPlayer->LoadCharacterFromIFO(-1, 1, 1);
        m_partyTable.CreateParty();

    }
    else
    {
        if (pPlayer->GetGameObject() == NULL)
        {
            CExoFile* pFile = new CExoFile("TEMP:temp_char", RESTYPE_BIC, "wb");
            if (pFile == NULL)
            {
                return 0;
            }
            if (!pFile->FileOpened())
            {
                delete pFile;
                return 0;
            }
            pFile->Write(pData, 1, nSize);
            delete pFile;

            CExoString sTempDirectory = "TEMP:";
            g_pExoResMan->AddResourceDirectory(sTempDirectory);
            if (nType == CSWSMessage::LOGIN_LOCAL_CHARACTER ||
                nType == CSWSMessage::LOGIN_LOCAL_CHARACTER_DATA)
            {
                pPlayer->LoadLocalCharacter();
            }
            g_pExoResMan->RemoveResourceDirectory(sTempDirectory);
            g_pExoResMan->CleanDirectory(sTempDirectory, 0, 0);
            m_partyTable.CreateParty();
        }
    }

    if (nType != CSWSMessage::LOGIN_DEFAULT_MODULE_CHARACTER &&
        pModule->m_bIsSaveGame && nType != CSWSMessage::LOGIN_MODULE_CHARACTER)
    {
        int nPlayerIndex = pModule->GetPlayerIndexInPlayerList(pPlayer);
        if (nPlayerIndex != -1)
        {
            pPlayer->m_nModulePlayerIndex = nPlayerIndex;
            SendCharacterQuery(pPlayer);
        }
        else
        {
            LoadCharacterFinish(pPlayer, 0);
        }
    }
    else
    {
        LoadCharacterFinish(pPlayer, 0);
    }
    return 1;
}
// PC 0x004EFDF0 GOG 0x004EE8C0
inline void CServerExoAppInternal::SetEstimatedSaveSize(
    const CExoString& sModuleName, unsigned short nModuleType)
{
#ifdef _XBOX
    CExoString sModule;
    sModule = sModuleName;
    CExoFile cFile(sModule, nModuleType, "rb");
    if (!cFile.FileOpened())
    {
        m_nEstimatedSaveSize = 50000000;
    }
    else if (nModuleType == RESTYPE_SAV || nModuleType == RESTYPE_RSV || nModuleType == RESTYPE_RIM)
    {
        m_nEstimatedSaveSize = cFile.GetSize() * 3 / 2;
    }
    else if (nModuleType == RESTYPE_MOD || nModuleType == RESTYPE_NWM)
    {
        m_nEstimatedSaveSize = cFile.GetSize() * 2;
    }
#else
    CExoArrayList<CExoString> lstFiles;
    CExoString sModule;
    unsigned long nSaveSize = 0;

    g_pExoBase->GetDirectoryList(&lstFiles, "GAMEINPROGRESS:",
                                RESTYPE_INVALID, 0, 0);
    int nFiles = lstFiles.num;
    if (nFiles == 0)
    {
        m_nEstimatedSaveSize = DEFAULT_SAVE_SIZE;
    }
    else
    {
        CExoString sFileName;
        CExoString sPath;
        CExoString sExtension;
        for (int i = 0; i < nFiles; i++)
        {
            sFileName = lstFiles[i];
            sPath = CExoString("GAMEINPROGRESS:") +
                    sFileName.Left(sFileName.GetLength() - 4);
            sExtension = sFileName.Right(3);
            CExoFile file(sPath, g_pExoBase->GetResTypeFromExtension(sExtension),
                          "rb");
            if (!file.FileOpened())
            {
                nSaveSize += DEFAULT_MODULE_SAVE_SIZE;
            }
            else
            {
                nSaveSize += file.GetSize();
            }
        }

        if (nModuleType != RESTYPE_RSV && nModuleType != RESTYPE_SAV)
        {
            nSaveSize += DEFAULT_MODULE_SAVE_SIZE;
        }
        m_nEstimatedSaveSize = nSaveSize * 10 / 9;
    }
#endif
}

inline void CServerExoAppInternal::SendExitingStartNewModuleMessage(
    int bSuccess)
{
    CSWSMessage* pMessage = GetServerExoApp()->GetSWSMessage();
    pMessage->SendServerToPlayerModule_EndStartNewModule(bSuccess);
    m_bDoingStartNewModule = 0;
}
// PC 0x004F00E0 GOG 0x004EF300
void CServerExoAppInternal::Shutdown(int bExit, int bSave)
{
    SERVEREXOAPPINTERNAL_HOOK();
    int nPVP = m_pServerInfo->m_nPVP;
    m_lstPendingPlayers.SetSize(0);
    CExoLinkedListNode* pPosition = m_pPlayerList->GetHeadPos();

    if (nPVP && bExit)
    {
        while (pPosition != NULL)
        {
            CSWSPlayer* pPlayer = (CSWSPlayer*)m_pPlayerList->GetAtPos(pPosition);
            if (pPlayer != NULL)
            {
                m_pSWSMessage->SendServerToPlayerShutDownServer(
                    pPlayer->m_nPlayerId,
                    m_nShutdownMessageDelay / SHUTDOWN_MESSAGE_TIME_UNIT);
                m_lstPendingPlayers.Add(pPlayer->m_nPlayerId);
            }
            m_pPlayerList->GetNext(pPosition);
        }
    }
    else
    {
        int nAttempts = SHUTDOWN_CLIENT_LOOP_LIMIT;
        while (pPosition != NULL && --nAttempts > 0)
        {
            CSWSPlayer* pPlayer = (CSWSPlayer*)m_pPlayerList->GetAtPos(pPosition);
            if (pPlayer != NULL)
            {
                if (bSave)
                {
                    m_lstPendingPlayers.Add(pPlayer->m_nPlayerId);
                    m_pPlayerList->GetNext(pPosition);
                }
                else
                {
                    pPosition = m_pPlayerList->GetHeadPos();
                }
            }
            else
            {
                m_pPlayerList->GetNext(pPosition);
            }
        }
    }

    pPosition = m_pSysAdminList->GetHeadPos();
    int nAttempts = SHUTDOWN_CLIENT_LOOP_LIMIT;
    while (pPosition != NULL && --nAttempts > 0)
    {
        CSWSClient* pClient = m_pSysAdminList->GetAtPos(pPosition);
        if (pClient != NULL)
        {
            unsigned long nPlayerId = pClient->m_nPlayerId;
            if (!m_lstPendingPlayers.Contains(nPlayerId))
            {
                pPosition = m_pSysAdminList->GetHeadPos();
            }
            else
            {
                m_pSysAdminList->GetNext(pPosition);
            }
        }
        else
        {
            m_pSysAdminList->GetNext(pPosition);
        }
    }

    if (nPVP && bExit && m_lstPendingPlayers.num != 0)
    {
        m_nShutdownStartTime = g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
        m_nShutdownTimeout = SHUTDOWN_REPLY_TIMEOUT;
    }
    else
    {
        m_nShutdownStartTime = g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
        m_nShutdownTimeout = SHUTDOWN_IMMEDIATE_TIMEOUT;
    }
}
// PC 0x004F0360 GOG 0x004EF580
CWorldTimer* CServerExoAppInternal::GetActiveTimer(unsigned long oidObject)
{
    if (oidObject != OBJECT_INVALID && GetActivePauseState() > 0)
    {
        if (GetActivePauseState() == PAUSE_STATE_COMBAT)
        {
            if (IsOnActiveExclusionList(oidObject) == 1)
            {
                return m_pPauseTimer;
            }
        }
        else if (GetActivePauseState() == PAUSE_STATE_PAUSED)
        {
            if (IsOnActiveExclusionList(oidObject) == 1)
            {
                return m_pTimestopTimer;
            }
        }
        else
        {
            return NULL;
        }
    }
    return m_pWorldTimer;
}
// PC 0x004F03D0 GOG 0x004EF5F0
void CServerExoAppInternal::AddCharListRequest(unsigned long nPlayerId)
{
    m_lstCharListRequests.AddUnique(nPlayerId);
}
// PC 0x004F03E0 GOG 0x004EF600
void CServerExoAppInternal::UpdateSubareasOnMoveTo(unsigned long oidObject,
                                                   const Vector& vFrom,
                                                   const Vector& vTo)
{
    SERVEREXOAPPINTERNAL_HOOK();
    unsigned long oidNormalized;
    CGameObject* pObject;
    CSWSCreature* pCreature;
    CSWSArea* pArea;

    pObject = NULL;
    if (oidObject != OBJECT_INVALID)
    {
        oidNormalized = oidObject & ~CGameObjectArray::LISTTYPE_MASK;
    }
    else
    {
        oidNormalized = OBJECT_INVALID;
    }
    m_pObjectArray->GetGameObject(oidNormalized, &pObject);
    if (pObject != NULL)
    {
        pCreature = pObject->AsSWSObject()->AsSWSCreature();
        if (pCreature != NULL)
        {
            pArea = pCreature->GetArea();
            if (pArea != NULL)
            {
                CExoArrayList<unsigned long> lstIntersectList;

                pArea->IntersectingLineSegment(vFrom, vTo, &lstIntersectList);
                pCreature->UpdateSubareasOnMoveTo(vFrom, vTo, 1,
                                                  &lstIntersectList, 0);
            }
        }
    }
}
// PC 0x004F04C0 GOG 0x004EFB70
CServerExoAppInternal::~CServerExoAppInternal()
{
    if (m_pPlayerList != NULL)
    {
        while (!m_pPlayerList->IsEmpty())
        {

            delete m_pPlayerList->RemoveHead();
        }
        delete m_pPlayerList;
    }
    if (m_pSysAdminList != NULL)
    {
        while (!m_pSysAdminList->IsEmpty())
        {
            delete m_pSysAdminList->RemoveHead();
        }
        delete m_pSysAdminList;
    }
    if (m_pServerOptions != NULL)
        delete[] m_pServerOptions;
    delete m_pTimestopExclusionList;
    m_pTimestopExclusionList = NULL;
    delete m_pPauseExclusionList;
    m_pPauseExclusionList = NULL;
    EXOXBOXASSERT(s_pServerExoAppInternal == this);
    s_pServerExoAppInternal = NULL;
    g_pAppManager->GetObjectTableManager(CAppManager::OBJECT_TABLE_SERVER)
        ->ClearAll();
}
// PC 0x004F0700 GOG 0x004F0240
void CServerExoAppInternal::PlayerListChange(unsigned long nPlayerId,
                                           int bAdded, int bGuaranteed)
{
    CExoString sTimestamp;

    g_pExoBase->m_pExoDebug->GetCurrentTimestamp(sTimestamp);
    if (bAdded == 1)
    {
        CNetLayerPlayerInfo* pPlayerInfo = m_pNetLayer->GetPlayerInfo(nPlayerId);
        CExoString sPlayerName(pPlayerInfo->m_sPlayerName);
        EXOLOG("ADDING PLAYER : %d - %s\n", nPlayerId, sPlayerName.CStr());
        pPlayerInfo = m_pNetLayer->GetPlayerInfo(nPlayerId);
        if (pPlayerInfo->m_bAuthorized == 1)
        {
            CSWSSysAdmin* pAdmin = new CSWSSysAdmin(nPlayerId);
            pAdmin->m_nLanguage = 0;
            m_pSysAdminList->AddHead(pAdmin);
            EXOLOG("%s Joined as Server Admin [%s]\n", sPlayerName.CStr(),
                   sTimestamp.CStr());
        }
        pPlayerInfo = m_pNetLayer->GetPlayerInfo(nPlayerId);
        if (pPlayerInfo->m_bPlayer == 1)
        {
            CSWSPlayer* pPlayer = new CSWSPlayer(nPlayerId);
            pPlayer->m_nLanguage = 0;
            m_pPlayerList->AddHead(pPlayer);
            EXOLOG("%s Joined as Player [%s]\n", sPlayerName.CStr(),
                   sTimestamp.CStr());
        }
        m_pSWSMessage->SendServerToServerAdminPlayerList(
            CNetLayer::PLAYERID_ALL_SERVERADMINS);
    }
    else if (bAdded == 0)
    {
        CNetLayerPlayerInfo* pPlayerInfo = m_pNetLayer->GetPlayerInfo(nPlayerId);
        CExoString sPlayerName(pPlayerInfo->m_sPlayerName);
        int bShutdown = 0;
        CExoLinkedListNode* pNode;

        pPlayerInfo = m_pNetLayer->GetPlayerInfo(nPlayerId);
        if (pPlayerInfo->m_bAuthorized == 1)
        {
            pNode = m_pSysAdminList->GetHeadPos();
            while (pNode != NULL)
            {
                CSWSSysAdmin* pAdmin = (CSWSSysAdmin*)m_pSysAdminList->GetAtPos(pNode);
                if (pAdmin->m_nPlayerId == nPlayerId)
                {
                    m_pSysAdminList->Remove(pNode);
                    delete pAdmin;
                    break;
                }
                m_pSysAdminList->GetNext(pNode);
            }
            EXOLOG("%s Left as a Server Admin [%s]\n", sPlayerName.CStr(),
                   sTimestamp.CStr());
        }
        pPlayerInfo = m_pNetLayer->GetPlayerInfo(nPlayerId);
        if (pPlayerInfo->m_bPlayer == 1)
        {
            pNode = m_pPlayerList->GetHeadPos();
            while (pNode != NULL)
            {
                CSWSPlayer* pPlayer = (CSWSPlayer*)m_pPlayerList->GetAtPos(pNode);
                if (pPlayer->m_nPlayerId == nPlayerId)
                {
                    if (m_pPCListPosition == pNode)
                    {
                        m_pPlayerList->GetNext(m_pPCListPosition);
                    }
                    m_pPlayerList->Remove(pNode);
                    m_pSWSMessage->SendServerToPlayerPlayerList_Delete(
                        CNetLayer::PLAYERID_ALL_PLAYERS, pPlayer);
                    bShutdown = pPlayer->m_bIsAllowedToSave;
                    delete pPlayer;
                    break;
                }
                m_pPlayerList->GetNext(pNode);
            }
            g_pAppManager->GetObjectTableManager(CAppManager::OBJECT_TABLE_SERVER)
                ->RemovePlayer(nPlayerId);
            EXOLOG("%s Left as a Player [%s]\n", sPlayerName.CStr(),
                   sTimestamp.CStr());
            if (bShutdown)
            {
                Shutdown(1, 0);
            }
        }
        if (m_lstPendingPlayers.Contains(nPlayerId))
        {
            m_lstPendingPlayers.Remove(nPlayerId);
            if (m_lstPendingPlayers.num == 0)
            {
                m_nShutdownTimeout = SHUTDOWN_COMPLETE_TIMEOUT;
            }
        }
    }
}
// PC 0x004F0A70 GOG 0x004F05B0
void CServerExoAppInternal::StopServices()
{
    CExoString sTimestamp;

    g_pExoBase->m_pExoDebug->GetCurrentTimestamp(sTimestamp);

    EXOLOG("[%s] Server Shutting Down\n", sTimestamp.CStr());
    Shutdown(0, 0);

    if (m_pObjectArray != NULL)
    {
        delete m_pObjectArray;
        m_pObjectArray = NULL;
    }
    if (m_pServerAIMaster != NULL)
    {
        delete m_pServerAIMaster;
        m_pServerAIMaster = NULL;
    }
    if (m_pVirtualMachineCommands != NULL)
    {
        delete m_pVirtualMachineCommands;
        m_pVirtualMachineCommands = NULL;
    }
    if (m_pSWSMessage != NULL)
    {
        delete m_pSWSMessage;
        m_pSWSMessage = NULL;
    }
    if (m_pServerInfo != NULL)
    {
        delete m_pServerInfo;
        m_pServerInfo = NULL;
    }
    if (m_pNetLayer != NULL)
    {
        m_pNetLayer->EndServerMode();
        m_pNetLayer->ShutDown();
        delete m_pNetLayer;
        m_pNetLayer = NULL;
    }
    if (g_pVirtualMachine != NULL)
    {
        delete g_pVirtualMachine;
        g_pVirtualMachine = NULL;
    }
    if (g_pScriptCompiler != NULL)
    {
        delete g_pScriptCompiler;
        g_pScriptCompiler = NULL;
    }

    g_pExoResMan->NukeDirectory("TEMP:", 1, 1);
    g_pExoResMan->NukeDirectory("CURRENTGAME:", 1, 1);
    g_pExoResMan->NukeDirectory("GAMEINPROGRESS:", 1, 1);

    if (g_nClientServerMode == CLIENT_SERVER_MODE_SERVER)
    {
        if (g_pRules != NULL)
        {
            delete g_pRules;
            g_pRules = NULL;
        }
        g_pExoResMan->RemoveFixedKeyTableFile("HD0:chitin");
        g_pExoResMan->RemoveResourceDirectory("OVERRIDE:");
        g_pExoResMan->RemoveResourceDirectory("ERRORTEX:");
        g_pExoResMan->RemoveResourceDirectory("SERVERVAULT:");
        if (g_pTlkTable != NULL)
        {
            delete g_pTlkTable;
            g_pTlkTable = NULL;
        }
        if (g_pExoResMan != NULL)
        {
            delete g_pExoResMan;
            g_pExoResMan = NULL;
        }
    }

    if (m_pWorldTimer != NULL)
    {
        delete m_pWorldTimer;
        m_pWorldTimer = NULL;
    }
    if (m_pTimestopTimer != NULL)
    {
        delete m_pTimestopTimer;
        m_pTimestopTimer = NULL;
    }
    if (m_pPauseTimer != NULL)
    {
        delete m_pPauseTimer;
        m_pPauseTimer = NULL;
    }
    if (m_pFactionManager != NULL)
    {
        delete m_pFactionManager;
        m_pFactionManager = NULL;
    }
}
// PC 0x004F0E30 GOG 0x004F0970
void CServerExoAppInternal::ReprocessExclusionListActions(unsigned char nPauseType)
{
    CExoLinkedList<unsigned long>* pList;
    CExoLinkedListNode* pNode;
    unsigned long nCalendarDay;
    unsigned long nTimeOfDay;
    CGameObject* pObject;

    if (nPauseType == PAUSE_TYPE_COMBAT_MASK)
    {
        pList = m_pTimestopExclusionList;
    }
    else
    {
        pList = m_pPauseExclusionList;
    }

    pNode = pList->GetHeadPos();
    while (pNode != NULL)
    {
        unsigned long oidObject = (unsigned long)pList->GetAtPos(pNode);
        GetActiveTimer(oidObject)->GetWorldTime(&nCalendarDay, &nTimeOfDay);
        m_pObjectArray->GetGameObject(oidObject, &pObject);
        if (pObject != NULL)
        {
            pObject->ResetUpdateTimes(nCalendarDay, nTimeOfDay);
        }
        pList->GetNext(pNode);
    }
}
// PC 0x004F2590 GOG 0x004F20D0
#ifdef _XBOX
int CServerExoAppInternal::LoadModule(CExoString sModule, CSWSPlayer* pPlayer)
#else
int CServerExoAppInternal::LoadModule(const CExoString& sModule,
                                     CSWSPlayer* pPlayer)
#endif
{
    unsigned __int64 nFreeBytes;
    int nDiskResult;
    CSWSModule* pModule = GetModule();

    CExoString sSourcePath;
    CExoString sDestinationPath;
    {
        CExoString sModuleName;
        CExoString sResourceName;
        CExoString sUnused;
        unsigned short nModuleType = RESTYPE_INVALID;
        CExoString sTimestamp;

        g_pExoBase->m_pExoDebug->GetCurrentTimestamp(sTimestamp);

#ifdef _XBOX
        EXOLOG("[%s] Loading Module: %s\n", sTimestamp.CStr(), sModule.CStr());
#else
        EXOLOG("Loading Module: %s [%s]\n", sModule.CStr(), sTimestamp.CStr());
#endif

        if (m_bLoadingModule != 1)
        {
            m_bLoadingModule = 1;
        }
        if (pModule != NULL)
        {
            UnloadModule();
        }
        if (g_pExoResMan->CreateDirectory2("CURRENTGAME:") == 0)
        {
            g_pExoResMan->CleanDirectory("CURRENTGAME:", 1, 1);
        }

        sDestinationPath = "CURRENTGAME:";
        int nSeparator = sModule.Find("\\", 0);
        if (nSeparator == -1)
        {
            sDestinationPath = sDestinationPath + sModule;
            sModuleName = sModule;
        }
        else
        {
            sModuleName = sModule.Right(sModule.GetLength() - nSeparator - 1);
            sDestinationPath = sDestinationPath + sModuleName;
        }

        if (IncludeModuleInSave(sModule))
        {
            g_pAppManager->GetServerExoApp()->GetPartyTable()->AddGameInProgress();
            if (g_pExoResMan->Exists(sModule, RESTYPE_RSV, (unsigned long*)NULL))
            {
                sSourcePath = "GAMEINPROGRESS:";
                sSourcePath = sSourcePath + sModule;
                sResourceName = sSourcePath;
                sSourcePath = g_pExoBase->m_pExoAliasList->ResolveFileName(
                    sSourcePath, RESTYPE_RSV);
                sDestinationPath = g_pExoBase->m_pExoAliasList->ResolveFileName(
                    sDestinationPath, RESTYPE_RSV);
                nModuleType = RESTYPE_RSV;
                SetEstimatedSaveSize(sResourceName, RESTYPE_RSV);
            }
            else if (g_pExoResMan->Exists(sModule, RESTYPE_SAV, (unsigned long*)NULL))
            {
                sSourcePath = "GAMEINPROGRESS:";
                sSourcePath = sSourcePath + sModule;
                sResourceName = sSourcePath;
                sSourcePath = g_pExoBase->m_pExoAliasList->ResolveFileName(
                    sSourcePath, RESTYPE_SAV);
                sDestinationPath = g_pExoBase->m_pExoAliasList->ResolveFileName(
                    sDestinationPath, RESTYPE_SAV);
                nModuleType = RESTYPE_SAV;
                SetEstimatedSaveSize(sResourceName, RESTYPE_SAV);
            }
            g_pAppManager->GetServerExoApp()->GetPartyTable()->RemoveGameInProgress();
        }

        if (nModuleType == RESTYPE_INVALID)
        {
            g_pExoResMan->AddResourceDirectory("NWMFILES:");
            if (g_pExoResMan->Exists(sModule, RESTYPE_NWM, (unsigned long*)NULL))
            {
                sSourcePath = "NWMFILES:";
                sSourcePath = sSourcePath + sModule;
                sResourceName = sSourcePath;
                sSourcePath = g_pExoBase->m_pExoAliasList->ResolveFileName(
                    sSourcePath, RESTYPE_NWM);
                sDestinationPath = g_pExoBase->m_pExoAliasList->ResolveFileName(
                    sDestinationPath, RESTYPE_NWM);
                nModuleType = RESTYPE_NWM;
                SetEstimatedSaveSize(sResourceName, RESTYPE_NWM);
            }
            g_pExoResMan->RemoveResourceDirectory("NWMFILES:");
        }

        if (nModuleType == RESTYPE_INVALID)
        {
            CExoString sDirectory;
            for (int nContentPackage = 1; nContentPackage < CTlkTable::NUM_CONTENT_PKGS;
                 ++nContentPackage)
            {
                sDirectory.Format("LIVE%d", nContentPackage);
                if (g_pExoBase->m_pExoAliasList->GetAliasPath(sDirectory, 0).GetLength())
                {
                    sDirectory = sDirectory + ":MODULES";
                    g_pExoResMan->AddResourceDirectory(sDirectory);
                    if (g_pExoResMan->Exists(sModule, RESTYPE_MOD, (unsigned long*)NULL))
                    {
                        sSourcePath = sDirectory + "\\";
                        sSourcePath = sSourcePath + sModule;
                        sResourceName = sSourcePath;
                        sSourcePath = g_pExoBase->m_pExoAliasList->ResolveFileName(
                            sSourcePath, RESTYPE_MOD);
                        sDestinationPath = g_pExoBase->m_pExoAliasList->ResolveFileName(
                            sDestinationPath, RESTYPE_MOD);
                        nModuleType = RESTYPE_MOD;
                        SetEstimatedSaveSize(sResourceName, RESTYPE_MOD);
                    }
                    if (nModuleType == RESTYPE_INVALID &&
                        g_pExoResMan->Exists(sModule, RESTYPE_RIM, (unsigned long*)NULL))
                    {
                        sSourcePath = sDirectory + "\\";
                        sSourcePath = sSourcePath + sModule;
                        sResourceName = sSourcePath;
                        sSourcePath = g_pExoBase->m_pExoAliasList->ResolveFileName(
                            sSourcePath, RESTYPE_RIM);
                        sDestinationPath = g_pExoBase->m_pExoAliasList->ResolveFileName(
                            sDestinationPath, RESTYPE_RIM);
                        nModuleType = RESTYPE_RIM;
                        SetEstimatedSaveSize(sResourceName, RESTYPE_RIM);
                    }
                    g_pExoResMan->RemoveResourceDirectory(sDirectory);
                    if (nModuleType != RESTYPE_INVALID)
                    {
                        break;
                    }
                }
            }
        }

        if (nModuleType == RESTYPE_INVALID)
        {
            g_pExoResMan->AddResourceDirectory("MODULES:");
            if (g_pExoResMan->Exists(sModule, RESTYPE_MOD, (unsigned long*)NULL))
            {
                sSourcePath = "MODULES:";
                sSourcePath = sSourcePath + sModule;
                sResourceName = sSourcePath;
                sSourcePath = g_pExoBase->m_pExoAliasList->ResolveFileName(
                    sSourcePath, RESTYPE_MOD);
                sDestinationPath = g_pExoBase->m_pExoAliasList->ResolveFileName(
                    sDestinationPath, RESTYPE_MOD);
                nModuleType = RESTYPE_MOD;
                SetEstimatedSaveSize(sResourceName, RESTYPE_MOD);
            }
            else if (g_pExoResMan->Exists(sModule, RESTYPE_RIM, (unsigned long*)NULL))
            {
                sSourcePath = "MODULES:";
                sSourcePath = sSourcePath + sModule;
                sResourceName = sSourcePath;
                sSourcePath = g_pExoBase->m_pExoAliasList->ResolveFileName(
                    sSourcePath, RESTYPE_RIM);
                sDestinationPath = g_pExoBase->m_pExoAliasList->ResolveFileName(
                    sDestinationPath, RESTYPE_RIM);
                nModuleType = RESTYPE_RIM;
                SetEstimatedSaveSize(sResourceName, RESTYPE_RIM);
            }
            g_pExoResMan->RemoveResourceDirectory("MODULES:");
        }

        if (nModuleType == RESTYPE_INVALID)
        {
            CSWSMessage* pMessage = m_pSWSMessage;
            pMessage->SendServerToPlayerSaveLoad_Status(
                pPlayer, CSWSMessage::SAVELOAD_STATUS_LOAD_FAILED,
                CSWSMessage::SAVELOAD_FAILURE_MODULE_NOT_FOUND);
            goto LoadModuleFailed;
        }

        nDiskResult = g_pExoResMan->GetFreeDiskSpace("CURRENTGAME:", &nFreeBytes);
        if (nDiskResult == 1)
        {
            CExoFile* pFile = new CExoFile(sResourceName, nModuleType, "rb");
            unsigned __int64 nFileSize = pFile->GetSize();
            delete pFile;
            if (nFileSize > nFreeBytes)
            {
                CSWSMessage* pMessage = m_pSWSMessage;
#ifdef _XBOX
                EXOXBOXASSERT(pMessage->SendServerToPlayerSaveLoad_Status(
                    pPlayer, CSWSMessage::SAVELOAD_STATUS_LOAD_FAILED,
                    CSWSMessage::SAVELOAD_FAILURE_DISK_SPACE));
#else
                pMessage->SendServerToPlayerSaveLoad_Status(
                    pPlayer, CSWSMessage::SAVELOAD_STATUS_LOAD_FAILED,
                    CSWSMessage::SAVELOAD_FAILURE_DISK_SPACE);
#endif
                goto LoadModuleFailed;
            }
        }

        CSWReentrantServerStats* pModuleNameStats = g_pAppManager->m_pReentrantServerStats;
        pModuleNameStats->m_sUnknown24 = sModuleName;
        CSWReentrantServerStats* pSourceStats = g_pAppManager->m_pReentrantServerStats;
        pSourceStats->m_sUnknown32 = sSourcePath;
        CSWReentrantServerStats* pDestinationStats = g_pAppManager->m_pReentrantServerStats;
        pDestinationStats->m_sUnknown40 = sDestinationPath;
        CSWReentrantServerStats* pTypeStats = g_pAppManager->m_pReentrantServerStats;
        pTypeStats->m_nResourceType = nModuleType;
        if (nModuleType == RESTYPE_SAV || nModuleType == RESTYPE_RSV)
        {
            g_pAppManager->m_pReentrantServerStats->StartStallEvent(
                CSWReentrantServerStats::OPERATION_LOAD_GAME);
            SendStartStallEvent(CSWSMessage::LOADBAR_STALL_EVENT_LOAD_GAME);
        }
        else
        {
            g_pAppManager->m_pReentrantServerStats->StartStallEvent(
                CSWReentrantServerStats::OPERATION_LOAD_MODULE);
            SendStartStallEvent(CSWSMessage::LOADBAR_STALL_EVENT_LOAD_MODULE);
        }
        CSWReentrantServerStats* pCompletedStats = g_pAppManager->m_pReentrantServerStats;
        pCompletedStats->m_nProgress = 0;
        CSWReentrantServerStats* pProgressStats = g_pAppManager->m_pReentrantServerStats;
        pProgressStats->m_nProgressTotal = 0;
        CSWReentrantServerStats* pFinalNameStats = g_pAppManager->m_pReentrantServerStats;
        pFinalNameStats->m_sUnknown24 = sModuleName;
    }
    return 1;

LoadModuleFailed:
    return 0;
}
// PC 0x004F35B0 GOG 0x004F30F0
void CServerExoAppInternal::DealWithLoadGameError(unsigned long nValue)
{
    CSWReentrantServerStats* pStats = g_pAppManager->m_pReentrantServerStats;

    UnloadModule();
    pStats->m_nUnknown56 = nValue;
    pStats->m_bOperationEndPending = 1;
    if (g_nClientServerMode == 2)
    {
        pStats->m_bOperationEndPending = 0;
        pStats->m_bOperationActive = 0;
        pStats->m_nOperation = 0;
    }
    m_nServerMode = 0;
    m_bLoadingModule = 0;
    g_pAppManager->m_pServerExoApp->GetSWSMessage()
        ->SendServerToPlayerLoadBar_EndStallEvent(CSWSMessage::LOADBAR_STALL_EVENT_LOAD_MODULE, nValue);
}
// PC 0x004F3610 GOG 0x004F3150
void CServerExoAppInternal::TogglePauseState(unsigned char nPauseType)
{
    SetPauseState(nPauseType, !GetPauseState(nPauseType));
}
// PC 0x004F0F20 GOG 0x004F0A60
void CServerExoAppInternal::SetPauseState(unsigned char nPauseType, int bPause)
{
    CSWSMessage* pMessage = m_pSWSMessage;
    CWorldTimer* pTimer;

    if (GetPauseState(nPauseType) != bPause)
    {
        if (bPause == 1)
        {
            m_PauseState.m_nPauseState |= nPauseType;
            m_pWorldTimer->PauseWorldTimer();
            if (GetActivePauseState() == PAUSE_STATE_COMBAT)
            {
                m_pTimestopTimer->PauseWorldTimer();
            }
            pMessage->SendServerToPlayerModule_SetPauseState(nPauseType, 1);
        }
        else
        {
            m_PauseState.m_nPauseState &= ~nPauseType;
            if (nPauseType == PAUSE_TYPE_COMBAT_MASK)
            {
                m_pPauseTimer->PauseWorldTimer();
                ReprocessExclusionListActions(PAUSE_TYPE_COMBAT_MASK);
                m_pPauseTimer->ResetTimer(m_pTimestopTimer);
                m_pPauseTimer->SetSnapshotTime();
                m_pTimestopTimer->SetSnapshotTime();
                pTimer = m_pPauseTimer;
                pTimer->m_nPreviousSnapshotTime = pTimer->GetSnapshotTime();
                pTimer = m_pTimestopTimer;
                pTimer->m_nPreviousSnapshotTime = pTimer->GetSnapshotTime();

                if (GetActivePauseState() != PAUSE_STATE_PAUSED)
                {
                    m_pWorldTimer->SetSnapshotTime();
                    pTimer = m_pWorldTimer;
                    pTimer->m_nPreviousSnapshotTime = pTimer->GetSnapshotTime();
                    m_pWorldTimer->UnpauseWorldTimer();
                }
                m_pTimestopTimer->UnpauseWorldTimer();
                m_pPauseTimer->UnpauseWorldTimer();
            }
            else if (nPauseType == PAUSE_TYPE_PAUSED_MASK)
            {
                m_pTimestopTimer->PauseWorldTimer();
                m_pPauseTimer->PauseWorldTimer();
                ReprocessExclusionListActions(PAUSE_TYPE_PAUSED_MASK);
                m_pTimestopTimer->ResetTimer(m_pWorldTimer);
                m_pPauseTimer->ResetTimer(m_pWorldTimer);
                m_pWorldTimer->SetSnapshotTime();
                m_pTimestopTimer->SetSnapshotTime();
                m_pPauseTimer->SetSnapshotTime();
                pTimer = m_pPauseTimer;
                pTimer->m_nPreviousSnapshotTime = pTimer->GetSnapshotTime();
                pTimer = m_pTimestopTimer;
                pTimer->m_nPreviousSnapshotTime = pTimer->GetSnapshotTime();
                pTimer = m_pWorldTimer;
                pTimer->m_nPreviousSnapshotTime = pTimer->GetSnapshotTime();
                m_pWorldTimer->UnpauseWorldTimer();
                m_pTimestopTimer->UnpauseWorldTimer();
                m_pPauseTimer->UnpauseWorldTimer();
            }
            pMessage->SendServerToPlayerModule_SetPauseState(nPauseType, bPause);
        }
    }
}
// PC 0x004F3640 GOG 0x004F3180
int CServerExoAppInternal::LoadGame(unsigned long nSlot,
                                    const CExoString& sName,
                                    const CExoString& sModule,
                                    CSWSPlayer* pPlayer)
{
    CExoString sDirectory;
    CExoString sGameName;

    CExoString sUnused1;
    CExoString sUnused2;

#ifdef _XBOX
    g_pAppManager->m_pClientExoApp->SetLoadBarProgress(50, 1);
    g_pAppManager->m_pClientExoApp->SetLoadStep(0, 50);
    g_pAppManager->m_pClientExoApp->SetLoadStep(1, 10);
    g_pAppManager->m_pClientExoApp->SetLoadStep(2, 13);
    g_pAppManager->m_pClientExoApp->SetLoadStep(3, 13);
    g_pAppManager->m_pClientExoApp->SetLoadStep(4, 13);
#else
    g_pAppManager->m_pClientExoApp->SetLoadBarProgress(10, 1);
    g_pAppManager->m_pClientExoApp->SetLoadStep(0, 10);
    g_pAppManager->m_pClientExoApp->SetLoadStep(1, 20);
    g_pAppManager->m_pClientExoApp->SetLoadStep(2, 23);
    g_pAppManager->m_pClientExoApp->SetLoadStep(3, 23);
    g_pAppManager->m_pClientExoApp->SetLoadStep(4, 23);
#endif

    if (nSlot != -1)
    {
        sGameName.Format("%06d - %s", nSlot, sName.CStr());
    }
    else
    {
        sGameName = sName;
    }

    sDirectory = "SAVES:";
    sDirectory = sDirectory + sGameName;
    g_pExoResMan->AddResourceDirectory(sDirectory);
    m_partyTable.LoadTableInfo();
    m_globalVariableTable.Load();
    g_pExoResMan->RemoveResourceDirectory(sDirectory);

    m_partyTable.m_bSkipPartyPositioning = 1;
    m_nAutoSaveElapsedTime = 0;
    m_bAutoSaveTimerExpired = 0;

    return LoadModule(sModule, pPlayer);
}

int CServerExoAppInternal::SendEnteringStartNewModuleMessage()
{
    GetServerExoApp()->GetSWSMessage()->SendServerToPlayerModule_StartStartNewModule();
    return 1;
}
// PC 0x004F3820 GOG 0x004F3360
int CServerExoAppInternal::ExecuteLoadModule(const CExoString& sModule)
{
    if (GetModule() != NULL && !GetModule()->GetModuleName().IsEmpty())
    {
        DumpLogCounters((char*)GetModule()->GetModuleName().CStr());
    }
    else
    {
        DumpLogCounters("");
    }

    SendEnteringStartNewModuleMessage();
    g_pExoInput->ClearEvents();
    g_bMouseCameraRotateToggle = 0;
    g_bLeftMouseDown = 0;
    g_bShiftActionState = 0;
    g_pAppManager->m_pClientExoApp->ResetDriveAcceleration();

    g_pAppManager->m_pClientExoApp->GetWorldTimer()->PauseWorldTimer();
    g_pAppManager->m_pServerExoApp->GetWorldTimer()->PauseWorldTimer();
    g_pAppManager->m_pClientExoApp->GetWorldTimer()->SetSnapshotTime();
    g_pAppManager->m_pServerExoApp->GetWorldTimer()->SetSnapshotTime();
    g_pAppManager->m_pClientExoApp->DisableInput();

    return LoadModule(sModule, NULL);
}
// PC 0x004F3920 GOG 0x004F3460
int CServerExoAppInternal::StartNewModule(const CExoString& sModule)
{
    int bLoadingModule = m_bLoadingModule;
    if (bLoadingModule == 1)
    {
        return 0;
    }

    g_pAppManager->GetClientExoApp()->SetLoadScreenHint(
        g_pAppManager->GetClientExoApp()->GetNextLoadScreenHintSTRREF(sModule, 0));
    g_pAppManager->GetClientExoApp()->SetLoadScreenHintUpdateTimer(0);
    g_pAppManager->GetClientExoApp()->SetCanSendDialog(0);
    g_pAppManager->GetClientExoApp()->GetInGameGui()->SetGlobalDialogState(0);
    g_pAppManager->GetClientExoApp()->GetInGameGui()->CloseDialog();
    g_pExoSound->StopAllOneShots();
    g_pAppManager->GetClientExoApp()->PlayLoadModuleMovies();

    g_pExoResMan->AddResourceDirectory("MODULES:");

    if (!g_pExoResMan->Exists(sModule, RESTYPE_MOD,
                             (unsigned long*)NULL)
        && !g_pExoResMan->Exists(sModule, RESTYPE_RIM,
                                (unsigned long*)NULL))
    {
        g_pExoResMan->RemoveResourceDirectory("MODULES:");
        return 0;
    }
    g_pExoResMan->RemoveResourceDirectory("MODULES:");

    m_bLoadingModule = 1;
    m_bDoingStartNewModule = 1;
    ForceRunAreaOnExitScripts();
    if (!StorePlayerCharacters())
    {
        m_bDoingStartNewModule = 0;
        return 0;
    }
    if (!StoreCurrentModule())
    {
        m_bDoingStartNewModule = 0;
        return 0;
    }

    C2DA* pModuleSave = new C2DA("modulesave", 0);
    if (pModuleSave != NULL && !pModuleSave->Load2DArray())
    {
        delete pModuleSave;
        pModuleSave = NULL;
    }
    DoModuleEnterSaveCleanup(sModule, pModuleSave);
    if (GetModule() != NULL
        && (pModuleSave == NULL || AutoSaveOnEnteringModule(sModule, pModuleSave)))
    {
#ifndef _XBOX
        DoPCAutosave(sModule, pModuleSave);
#endif
    }
    if (pModuleSave != NULL)
    {
        delete pModuleSave;
    }
    return ExecuteLoadModule(sModule);
}
// PC 0x004F3BB0 GOG 0x004F36F0
int CServerExoAppInternal::MainLoop()
{

    UpdateAutoSaveTimer();
    CSWReentrantServerStats* pStats = g_pAppManager->m_pReentrantServerStats;
    int bReentrantOperation = pStats->m_bOperationActive;
    if (bReentrantOperation == 0)
    {
        if (GetActivePauseState() == 0)
            m_pWorldTimer->SetSnapshotTime();
        if (GetActivePauseState() != PAUSE_STATE_COMBAT)
            m_pTimestopTimer->SetSnapshotTime();
        m_pPauseTimer->SetSnapshotTime();
        m_pNetLayer->ProcessReceivedFrames(1);
        m_pNetLayer->UpdateStatusLoop(CNetLayer::APPLICATION_SERVER);
        TempDeathUpdate();
        UpdateMapData();
        if (g_pAppManager->m_pReentrantServerStats->m_bOperationActive == 0 &&
            m_nServerMode == SERVER_MODE_RUNNING)
        {
            m_pServerAIMaster->UpdateState();
            int bForceUpdate = m_bForceUpdate;
            UpdateClientGameObjects(bForceUpdate);
            m_bForceUpdate = 0;
        }
#ifdef _XBOX
        int bTransitionReady = 1;
        if (m_bMoveToModulePending == 1)
        {
            if (m_nAutoSaveCalendarDay == 0)
            {
                m_nAutoSaveCalendarDay = 1;
                bTransitionReady = 0;
                g_pExoInput->SetActive(0);
                g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_PAUSE_ALL);
                m_nAutoSaveTimeOfDay = g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000;
            }
            else
            {
                bTransitionReady =
                    (unsigned long)(g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000) -
                        m_nAutoSaveTimeOfDay > 500;
                g_pExoInput->UpdateJoysticks();
                g_pAppManager->GetClientExoApp()->GetInGameGui()->UpdateControllerPauseState();
            }
        }
        if (m_bMoveToModulePending == 1 && bTransitionReady == 1 &&
            g_pAppManager->GetClientExoApp()->GetInGameGui()
                ->m_bControllerPauseActive == 0)
        {
            ForceRunAreaOnExitScripts();
            if (!StorePlayerCharacters())
            {
                m_bDoingStartNewModule = 0;
                return 0;
            }
            if (!StoreCurrentModule())
            {
                m_bDoingStartNewModule = 0;
                return 0;
            }
            if (g_pExoResMan->CreateDirectory2("REBOOTDATA:") == 0)
            {
                g_pExoResMan->CleanDirectory("REBOOTDATA:", 0, 0);
            }
            CExoString sModule(m_sMoveToModule);
            m_partyTable.Save("REBOOTDATA:", 0);
            m_globalVariableTable.Save("REBOOTDATA:");
            KOTOR_LAUNCH_DATA cParams;
            cParams.m_nModuleTrans = KOTOR_LAUNCH_DATA::MODULETRANS_MODULE;
            CExoString sStartWaypoint(m_sMoveToModuleStartWaypoint);
            CExoString sMovie1("");
            CExoString sMovie2("");
            CExoString sMovie3("");
            CExoString sMovie4("");
            CExoString sMovie5("");
            CExoString sMovie6("");
            CExoArrayList<CExoString> lMovies(
                *g_pAppManager->GetClientExoApp()->GetMoveToModuleMovies());
            CExoString sLoadScreen("");
            CExoString sLoadMusic("");
            int bMinGlobalRim = 0;
            if (g_pRules->m_p2DArrays->m_pAreaTransition->GetCExoStringEntry(
                    sModule, "BMPResRef", &sLoadScreen) == 0)
            {
                sLoadScreen.Format("load_%s", sModule);
                if (!g_pExoResMan->Exists(sLoadScreen, RESTYPE_TGA, (unsigned long*)NULL) &&
                    !g_pExoResMan->Exists(sLoadScreen, RESTYPE_TPC, (unsigned long*)NULL))
                {
                    g_pRules->m_p2DArrays->m_pAreaTransition->GetCExoStringEntry(
                        "DEFAULT", "BMPResRef", &sLoadScreen);
                }
            }
            sLoadMusic = g_pAppManager->GetClientExoApp()->GetLoadMusicByModuleName(sModule);
            if (lMovies.num > 0)
                sMovie1 = lMovies.element[0];
            if (lMovies.num > 1)
                sMovie2 = lMovies.element[1];
            if (lMovies.num > 2)
                sMovie3 = lMovies.element[2];
            if (lMovies.num > 3)
                sMovie4 = lMovies.element[3];
            if (lMovies.num > 4)
                sMovie5 = lMovies.element[4];
            if (lMovies.num > 5)
                sMovie6 = lMovies.element[5];
            if (IsMoveToModuleMinGlobalRim() == 1)
            {
                bMinGlobalRim = 1;
            }
            cParams.m_nAutoCalInfo = g_pExoInput->GetAutoCalInfo();
            cParams.m_nPortBinding = g_pExoInput->GetPortBinding();
            if (sStartWaypoint == "")
            {
                sStartWaypoint = "*";
            }
            cParams.m_bRenderQA = RenderQA;
            cParams.SetModuleName(sModule);
            cParams.SetLoadScreen(sLoadScreen);
            cParams.SetLoadMusic(sLoadMusic);
            cParams.SetStartWaypoint(sStartWaypoint);
            cParams.SetMovie1(sMovie1);
            cParams.SetMovie2(sMovie2);
            cParams.SetMovie3(sMovie3);
            cParams.SetMovie4(sMovie4);
            cParams.SetMovie5(sMovie5);
            cParams.SetMovie6(sMovie6);
            cParams.m_bMinGlobalRim = bMinGlobalRim;
            unsigned long nCalendarDay, nTimeOfDay;
            unsigned long nHour, nMinute, nSecond, nMillisecond;
            unsigned long nYear, nMonth, nDay, nModuleTime;
            unsigned char nDawnDusk;
            g_pAppManager->GetServerExoApp()->GetWorldTimer()
                ->GetWorldTime(&nCalendarDay, &nTimeOfDay);
            g_pAppManager->GetServerExoApp()->GetWorldTimer()
                ->ConvertFromTimeOfDay(nTimeOfDay, &nHour, &nMinute,
                                      &nSecond, &nMillisecond);
            GetModule()->GetTime(&nYear, &nMonth, &nDay, &nHour,
                                 &nDawnDusk, &nModuleTime);
            cParams.SetTime(nYear, nMonth, nDay, nHour, nMinute, nSecond, nMillisecond);
            cParams.m_nTimePauseTime = nTimeOfDay;
            cParams.m_nTimePauseDay = nCalendarDay;
            KOTOR_LAUNCH_OPTIONS_DATA cOptionsData;
            g_pAppManager->GetClientExoApp()->GetClientOptions()->GetOptionsData(&cOptionsData);
            cParams.SetOptionsData(cOptionsData);
            CStatusSummary cStatusSummary;
            g_pAppManager->GetClientExoApp()->GetInGameGui()->GetStatusSummary(cStatusSummary);
            cParams.SetStatusSummary(cStatusSummary);
            C2DA* pModuleSave = new C2DA(CResRef("modulesave"), 0);
            if (pModuleSave != NULL && !pModuleSave->Load2DArray())
            {
                delete pModuleSave;
                pModuleSave = NULL;
            }
            DoModuleEnterSaveCleanup(sModule, pModuleSave);
            if (HasEnoughDiskSpaceForSaveGame() && GetModule() != NULL &&
                (pModuleSave == NULL || AutoSaveOnEnteringModule(sModule, pModuleSave)))
            {
                DoPCAutosave(sModule, cParams, pModuleSave);
            }
            cParams.m_nHintTimer1 = m_nAutoSaveElapsedTime;
            cParams.m_nHintTimer2 = m_bAutoSaveTimerExpired;
            if (sMovie1.GetLength() <= 0 && sMovie2.GetLength() <= 0 &&
                sMovie3.GetLength() <= 0 && sMovie4.GetLength() <= 0 &&
                sMovie5.GetLength() <= 0 && sMovie6.GetLength() <= 0)
            {
                cParams.m_nLoadScreenHint =
                    g_pAppManager->GetClientExoApp()->GetNextLoadScreenHintSTRREF(sModule, 0);
            }
            else
            {
                cParams.m_nLoadScreenHint = 0;
            }
            cParams.m_nGamePlayHint = g_pAppManager->GetClientExoApp()->GetGamePlayHint();
            cParams.m_nStoryHint = g_pAppManager->GetClientExoApp()->GetStoryHint();
            cParams.m_nLoadScreenHintUpdateTimer = 0;
            if (pModuleSave != NULL)
            {
                delete pModuleSave;
            }
            AurPersistDisplay();
            ExoXboxLaunch(&cParams, g_pAppManager->GetServerExoApp()->GetModuleName().CStr());
        }
#else
        int bMovePending = m_bMoveToModulePending;
        if (bMovePending == 1 &&
            g_pAppManager->GetClientExoApp()->GetInGameGui()
                ->m_bControllerPauseActive == 0)
        {
            g_pAppManager->GetServerExoApp()->GetGlobalVariableTable()->SetValueNumber(
                "000_Human_Conf_Active", 0);
            g_pAppManager->GetServerExoApp()->GetGlobalVariableTable()->SetValueNumber(
                "000_Beast_Conf_Active", 0);
            g_pAppManager->GetServerExoApp()->GetGlobalVariableTable()->SetValueNumber(
                "000_Droid_Conf_Active", 0);
            unsigned long nCalendarDay, nTimeOfDay;
            unsigned long nHour, nMinute, nSecond, nMillisecond;
            unsigned long nYear, nMonth, nDay, nModuleTime;
            unsigned char nDawnDusk;
            g_pAppManager->GetServerExoApp()->GetWorldTimer()
                ->GetWorldTime(&nCalendarDay, &nTimeOfDay);
            g_pAppManager->GetServerExoApp()->GetWorldTimer()
                ->ConvertFromTimeOfDay(nTimeOfDay, &nHour, &nMinute,
                                      &nSecond, &nMillisecond);
            GetModule()->GetTime(&nYear, &nMonth, &nDay, &nHour,
                                 &nDawnDusk, &nModuleTime);
            g_pAppManager->GetServerExoApp()->SetMoveToWorldTime(
                nYear, nMonth, nDay, nHour, nMinute, nSecond, nMillisecond);
            g_pAppManager->GetServerExoApp()->SetMoveToPausedTimeOfDay(nTimeOfDay);
            g_pAppManager->GetServerExoApp()->SetMoveToPausedCalendarDay(nCalendarDay);
            m_partyTable.UpdateMembers(0);
            StartNewModule(m_sMoveToModule);
            m_bMoveToModulePending = 0;
        }
#endif
        pStats = g_pAppManager->m_pReentrantServerStats;
        if (pStats->m_bOperationActive == 0 &&
            g_pAppManager->GetServerExoApp()->GetPlayerCreature() != NULL &&
            g_pAppManager->GetServerExoApp()->GetPlayerCreature()->GetArea() != NULL &&
            !g_pAppManager->GetClientExoApp()->GetAreaNotReadyToDisplay())
        {
            int bPartyAlive = 0;
            if (m_bAutoSavePending == 1)
            {
                for (int nMember = 0; nMember < CSWParty::MAX_MEMBERS; nMember++)
                {
                    CSWCCreature* pCharacter =
                        g_pAppManager->GetClientExoApp()->GetSWParty()->GetCharacter(nMember);
                    if (pCharacter != NULL &&
                        pCharacter->GetServerCreature()->GetCurrentHitPoints(0) > 1)
                    {
                        bPartyAlive = 1;
                        break;
                    }
                }
            }
            if (bPartyAlive)
            {
                if (HasEnoughDiskSpaceForSaveGame() &&
                    m_nServerMode == SERVER_MODE_RUNNING)
                {
                    CExoString sName;
                    sName = "AUTOSAVE";
#ifdef _XBOX
                    AurReleaseFrameTexture();
                    CExoString sSaveRoot("U:\\");
                    WCHAR wszSaveName[256];
                    char szSavePath[256];
                    mbstowcs(wszSaveName, g_pTlkTable->GetSimpleString(STRREF_AUTO_SAVE_NAME).CStr(), 255);
                    XCreateSaveGame(sSaveRoot.CStr(), wszSaveName, OPEN_ALWAYS, 1, szSavePath, 254);
                    {
                        CExoString sAlias("SAVES");
                        g_pExoBase->m_pExoAliasList->ChangePath(sAlias, szSavePath);
                    }
                    g_pExoResMan->FileCopy("HD0:AutoSaveImage", "SAVES:SaveImage", 3011, RESTYPE_INVALID);
                    SaveGame(0, sName, NULL, 1);
#else
                    SaveGame(1, sName, NULL, 1);
#endif
                }
                else
                {
                    if (g_pAppManager->GetClientExoApp()->GetInGameGui()
                            ->m_nGlobalDialogState != 0)
                        g_pAppManager->GetClientExoApp()->GetInGameGui()
                            ->m_bHoldWorldFadeInForDialog = 1;
                    else
                    {
                        Vector vBlack(0.0f, 0.0f, 0.0f);
                        g_pAppManager->GetClientExoApp()->GetInGameGui()
                            ->StartGlobalFade(1, 0.5f, 1.0f, vBlack);
                    }
                    g_pAppManager->GetClientExoApp()->SetCanSendDialog(1);
                    g_pExoInput->EndAttemptedBind();
                    g_pAppManager->GetClientExoApp()->GetInGameGui()
                        ->SetGlobalUnpauseableState(0);
                    g_pAppManager->GetClientExoApp()->GetModule()->m_pInfo->StartSounds();
                }
            }
            m_bAutoSavePending = 0;
        }
        if (m_bExportAllCharactersPending == 1)
        {
            ExportAllPlayers();
            m_bExportAllCharactersPending = 0;
        }
        if (m_bNeedCharacterPending == 1)
        {
            m_pSWSMessage->SendServerToPlayerLogin_NeedCharacter(
                ((CSWSPlayer*)m_pPlayerList->GetHead())->m_nPlayerId);
            m_bNeedCharacterPending = 0;
        }
    }
    else
    {
        m_pNetLayer->ProcessReceivedFrames(0);
        m_pNetLayer->UpdateStatusLoop(CNetLayer::APPLICATION_SERVER);
        CSWReentrantServerStats* pStats = g_pAppManager->m_pReentrantServerStats;
        if (pStats->m_nUnknown56 == 0)
        {
            if (pStats->m_nOperation == CSWReentrantServerStats::OPERATION_LOAD_MODULE ||
                pStats->m_nOperation == CSWReentrantServerStats::OPERATION_LOAD_GAME)
            {
                CExoString sModule(pStats->m_sUnknown24);
                CSWSModule* pModule = GetModule();
                int bLoadingSaveGame = 0;
                if (m_bLoadFromSaveGame)
                    bLoadingSaveGame = 1;
                if (pStats->m_nProgressTotal == 0)
                {
                    CExoString sModuleName(pStats->m_sUnknown24);
                    CExoString sSource(pStats->m_sUnknown32);
                    CExoString sDestination(pStats->m_sUnknown40);
                    unsigned long nType = pStats->m_nResourceType;
                    if (!CopyModuleToCurrentGame(sSource, sDestination, nType))
                        DealWithLoadGameError(LOAD_GAME_ERROR_COPY_MODULE);
                    else
                    {
                        g_pAppManager->GetClientExoApp()->GetInGameGui()->m_n1024 = 0;
                        pModule = new CSWSModule(sModuleName, 0,
                            (unsigned short)nType == RESTYPE_SAV ||
                            (unsigned short)nType == RESTYPE_RSV);
                        m_oidModule = pModule->GetId();
                        if ((unsigned short)nType == RESTYPE_RSV)
                        {
                            pModule->m_bIsNWMFile = 1;
                            pModule->m_sSaveName = sModuleName;
                        }
                        unsigned long nError = pModule->LoadModuleStart(
                            sModuleName, bLoadingSaveGame);
                        if (nError != 0)
                            DealWithLoadGameError(nError);
                    }
                }
                else if (pStats->m_nProgressTotal != pStats->m_nProgress)
                {
                    m_partyTable.ClearMembers();
                    m_partyTable.ClearPuppets();
                    int nLoadProgress = pStats->m_nProgressTotal;
                    int nLoadStep = pStats->m_nProgress;
                    unsigned long nError = pModule->LoadModuleInProgress(
                        nLoadStep, nLoadProgress);
                    if (nError != 0)
                        DealWithLoadGameError(nError);
                }
                else if (pStats->m_bOperationEndPending != 1)
                {
                    unsigned long nError = pModule->LoadModuleFinish();
                    if (nError != 0)
                        DealWithLoadGameError(nError);
                    pStats->m_bOperationEndPending = 1;
                    if (g_nClientServerMode == CLIENT_SERVER_MODE_SERVER)
                    {
                        pStats->m_bOperationEndPending = 0;
                        pStats->m_bOperationActive = 0;
                        pStats->m_nOperation = 0;
                    }
                    m_nServerMode = SERVER_MODE_LOADING;
                    m_bLoadingModule = 0;
                    g_pAppManager->GetServerExoApp()->GetSWSMessage()
                        ->SendServerToPlayerLoadBar_EndStallEvent(CSWSMessage::LOADBAR_STALL_EVENT_LOAD_MODULE, 0);
                    m_pSWSMessage->SendServerToPlayerServerStatus_Status(
                        CNetLayer::PLAYERID_ALL_PLAYERS);
                    m_pSWSMessage->SendServerToServerAdminServerStatus(
                        CNetLayer::PLAYERID_ALL_SERVERADMINS);
                    if (m_lstCharListRequests.num != 0)
                    {
                        for (int nRequest = 0; nRequest < m_lstCharListRequests.num; ++nRequest)
                        {
                            CSWSPlayer* pPlayer = GetClientObjectByPlayerId(
                                m_lstCharListRequests[nRequest], 0);
                            if (pPlayer != NULL)
                                m_pSWSMessage->SendServerToPlayerCharList(pPlayer);
                        }
                        m_lstCharListRequests.SetSize(0);
                    }
                }
            }
            else if (pStats->m_nOperation == CSWReentrantServerStats::OPERATION_SAVE_GAME)
            {
                StallEventSaveGame();
                DumpLogCounters("2");
            }
        }
    }
    if (UpdateShutdownTimer(g_pExoBase->m_pExoTimers->GetHighResolutionTimer()))
    {
        if (m_lstPendingPlayers.num != 0)
            Shutdown(0, 0);
        else
            g_pAppManager->DestroyServer();
    }
    UpdateMainLoopTimeBudget(g_pExoBase->m_pExoTimers->GetHighResolutionTimer());
    return 0;
}
