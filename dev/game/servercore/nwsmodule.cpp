#include <string.h>
#include <math.h>
#if defined(_WIN32) && !defined(_XBOX)
#include <windows.h>
#else
#include "platform.h"
#endif

#include "appmanager.h"
#include "factionmanager.h"
#include "gameobjectarray.h"
#include "nwerffile.h"
#include "nwsarea.h"
#include "nwseffect.h"
#include "nwseffectlist.h"
#include "nwmessage.h"
#include "nwsmodule.h"
#include "gameobjectarray.h"
#include "nwsdoor.h"
#include "nwsobject.h"
#include "nwsplayer.h"
#include "nwsscriptevent.h"
#include "serveraimaster.h"
#include "nwreentrantserver.h"
#include "nwstrigger.h"
#include "pathfindinfo.h"
#include "nwscript.h"
#include "serverexoapp.h"
#include "serverexoappinternal.h"
#include "worldtimer.h"
#include "clientexoapp.h"
#include "exoresman.h"
#include "exotlktable.h"
#include "partytables.h"
#include "serverinfo.h"
#include "swparty.h"
#include "nwccreature.h"
#include "swmain.h"

static inline void NwsmoduleTrace3();
static inline void NwsmoduleTrace2();
static inline void NwsmoduleTrace1();

void UpdateScreen(float fElapsed, int bRunWorld, int bSkipRender);
// PC 0x00509340
CSWSModule::CSWSModule(const CExoString& sModuleName, int bRequest, int bUnknown)
    : CGameObject(CGameObject::OBJECT_TYPE_MODULE, OBJECT_INVALID)
{
    CExoString sResourceName;
    m_pModuleInfo = new CSWSModuleInfo;
    m_pEntryInfo = new CSWSModuleEntryInfo;
    m_pAreaMap = new CSWSAreaMap;
    m_nHeartbeatDay = 0;
    m_nHeartbeatTime = 0;
    sResourceName = CExoString("CURRENTGAME:") + sModuleName;
    m_sModuleName = sResourceName;
    m_bAIUpdate = 1;
    m_nUserDefinedEvent = 0;
    m_oidClientEnter = CGameObjectArray::INVALIDOBJID;
    m_oidClientLeave = CGameObjectArray::INVALIDOBJID;
    m_oidAcquiredItem = CGameObjectArray::INVALIDOBJID;
    m_oidItemAcquirer = CGameObjectArray::INVALIDOBJID;
    m_oidDeadPlayer = CGameObjectArray::INVALIDOBJID;
    m_oidUnacquiredItem = CGameObjectArray::INVALIDOBJID;
    m_oidItemUnacquirer = CGameObjectArray::INVALIDOBJID;
    m_oidRestingPlayer = OBJECT_INVALID;
    m_bRestCancelled = 0;
    m_oidActivatedItem = OBJECT_INVALID;
    m_oidItemActivator = OBJECT_INVALID;
    m_oidActivatedTarget = OBJECT_INVALID;
    m_oidActivatedTarget2 = OBJECT_INVALID;
    m_oidLevelingPlayer = OBJECT_INVALID;
    m_n198 = OBJECT_INVALID;
    m_oidEquippedItem = OBJECT_INVALID;
    m_oidDyingPlayer = CGameObjectArray::INVALIDOBJID;
    m_oidRespawningPlayer = CGameObjectArray::INVALIDOBJID;
    m_n214 = 0;
    m_bIsNWMFile = 0;
    m_sSaveName = "";
    m_bIsSaveGame = 0;
    AddModuleResources(m_sModuleName);
    m_n210 = 2;
    m_nMinutesPerHour = 5;
    m_nDawnHour = 0;
    m_nDuskHour = 0;
    m_nStartMonth = 1;
    m_nStartDay = 1;
    m_nStartHour = 0;
    m_nXPScale = 10;
    m_nYear = CWorldTimer::DEFAULT_YEAR;
    m_nMonth = 1;
    m_nDay = 1;
    m_nTime = 0;
    m_nTime2 = 0;
    m_nTimeDawnDusk = 0;
    m_n1D8 = 1;
    m_pERFFile = NULL;
    m_pERFStruct = NULL;
    m_pModuleGFF = NULL;
    m_plstPlayers = NULL;
    SetResRef("", 0);
    SetResRef("MODULE", bRequest);
    g_pAppManager->GetServerExoApp()->GetObjectArray()->AddInternalObject(
        m_nId, this, 0);
    g_pAppManager->GetServerExoApp()->GetWorldTimer()->GetWorldTime(
        &m_nLastUpdateDay, &m_nLastUpdateTime);
    m_sTag = "";
    m_nResourceCount = -1;
    m_pAreaCached = NULL;
    m_oidAreaCached = OBJECT_INVALID;
}
// PC 0x00506390
void CSWSModule::EventHandler(unsigned long nEventId, unsigned long oidCaller,
                              void* pEventData, unsigned long nCalendarDay,
                              unsigned long nTimeOfDay)
{
    if (nEventId == CServerAIMaster::EVENT_TIMED_EVENT)
    {
        if (pEventData != NULL)
        {
            g_pVirtualMachine->RunScriptSituation(pEventData, m_nId, 1);
        }
    }
    else if (nEventId == CServerAIMaster::EVENT_SIGNAL_EVENT)
    {
        CScriptEvent* pData = static_cast<CScriptEvent*>(pEventData);
        switch (pData->m_nType)
        {
        case 0:
            m_nHeartbeatDay = nCalendarDay;
            m_nHeartbeatTime = nTimeOfDay;
            g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_HEARTBEAT], m_nId, 1);
            break;
        case 11:
            m_nUserDefinedEvent = pData->GetInteger(0);
            g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_USER_DEFINED], m_nId, 1);
            break;
        case 14:
            m_oidClientEnter = pData->GetObjectID(0);
            g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_CLIENT_ENTER], m_nId, 1);
            break;
        case 15:
            m_oidClientLeave = pData->GetObjectID(0);
            g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_CLIENT_LEAVE], m_nId, 1);
            break;
        case 16:
            g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_MODULE_START], m_nId, 1);
            break;
        case 17:
            g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_MODULE_LOAD], m_nId, 1);
            break;
        case 19:
            m_oidItemAcquirer = pData->GetObjectID(1);
            m_oidAcquiredItem = pData->GetObjectID(0);
            g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_ACQUIRE_ITEM], m_nId, 1);
            break;
        case 20:
            m_oidUnacquiredItem = pData->GetObjectID(0);
            m_oidItemUnacquirer = oidCaller;
            g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_UNACQUIRE_ITEM], m_nId, 1);
            break;
        case 18:
            m_oidActivatedItem = pData->GetObjectID(0);
            m_oidItemActivator = pData->GetObjectID(1);
            m_oidActivatedTarget = pData->GetObjectID(2);
            m_oidActivatedTarget2 = pData->GetObjectID(3);
            m_vActivatedLocation = Vector(
                pData->GetFloat(0), pData->GetFloat(1), pData->GetFloat(2));
            g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_ACTIVATE_ITEM], m_nId, 1);
            break;
        case 10:
            m_oidDeadPlayer = oidCaller;
            g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_PLAYER_DEATH], m_nId, 1);
            break;
        case 32:
            m_oidDyingPlayer = oidCaller;
            g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_PLAYER_DYING], m_nId, 1);
            break;
        case 33:
            m_oidRespawningPlayer = oidCaller;
            g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_SPAWN_BUTTON_DOWN], m_nId, 1);
            break;
        case 35:
            m_oidRestingPlayer = oidCaller;
            m_bRestCancelled = pData->GetInteger(0);
            g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_PLAYER_REST], m_nId, 1);
            break;
        case 37:
            m_oidLevelingPlayer = oidCaller;
            g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_PLAYER_LEVEL_UP], m_nId, 1);
            break;
        case 36:
        {
            CGameObject* pObject = NULL;
            CServerExoApp* pExoApp = g_pAppManager->m_pServerExoApp;
            pExoApp->GetObjectArray()->GetGameObject(oidCaller, &pObject);
            if (pObject != NULL && pObject->AsSWSCreature() != NULL)
            {
                g_pAppManager->m_pServerExoApp->GetObjectArray()->Delete(
                    oidCaller);
            }
            break;
        }
        case 38:
            m_oidEquippedItem = pData->GetObjectID(0);
            g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_EQUIP_ITEM], m_nId, 1);
            break;
        }
        delete pData;
    }
}
// PC 0x00507A30
CSWSModule::~CSWSModule()
{
    if (m_n1D8 == 0)
        m_pRes->Release();

    while (m_lstPlayers.GetHeadPos() != NULL)
    {
        NWPlayerListItem_st* pPlayer = m_lstPlayers.RemoveHead();
        delete pPlayer;
    }

    CSWSArea* pArea = g_pAppManager->GetServerExoApp()->GetAreaByGameObjectID(
        m_oidArea);
    if (pArea != NULL)
    {
        pArea->ClearObjects();
        delete pArea;
    }

    while (m_lstModuleCutScene.GetHeadPos() != NULL)
        delete m_lstModuleCutScene.RemoveHead();
    while (m_lstModuleExpansion.GetHeadPos() != NULL)
        delete m_lstModuleExpansion.RemoveHead();

    RemoveModuleResources(m_sModuleName);
    m_sModuleName = "";
    if (m_pEntryInfo != NULL)
        delete m_pEntryInfo;
    if (m_pModuleInfo != NULL)
        delete m_pModuleInfo;
    if (m_pAreaMap != NULL)
    {
        delete m_pAreaMap;
        m_pAreaMap = NULL;
    }

    g_pAppManager->GetServerExoApp()->GetObjectArray()->Delete(m_nId);
    g_pAppManager->GetServerExoApp()->ClearLastModuleFound();
    m_pAreaCached = NULL;
    m_oidAreaCached = OBJECT_INVALID;
}

void CSWSModule::ClearAreaVisitedFlags()
{
    CServerExoApp* pExoApp = g_pAppManager->m_pServerExoApp;
    CSWSArea* pArea = pExoApp->GetAreaByGameObjectID(m_oidArea);
    if (pArea != NULL)
    {
        pArea->m_bTransitionCounted = 0;
    }
}
// PC 0x00507D60
int CSWSModule::ComputeInterAreaPath(CPathfindInformation* pPathfind)
{
    int nDepth;

    pPathfind->m_nField_A8 = 10;
    pPathfind->m_pAreaPath = new int[10];
    pPathfind->m_pAreaPathCost = new int[9];
    pPathfind->m_nField_A0 = 0;
    nDepth = 1;
    do
    {
        if (pPathfind->m_nField_A0 != 0)
        {
            break;
        }
        pPathfind->m_pAreaPath[0] = pPathfind->m_nEndArea;
        ClearAreaVisitedFlags();
        if (InterAreaDFS(0, nDepth, pPathfind) == 1)
        {
            pPathfind->m_nField_A0 = 1;
        }
        nDepth++;
    } while (nDepth < 10);
    if (pPathfind->m_nField_A0 == 0)
    {
        return 0;
    }
    SetIntraAreaGoal(pPathfind);
    return 1;
}
// PC 0x00506710
int CSWSModule::GenerateInterAreaDFSSuccessors(
    int nArea, CPathfindInformation* pPathfind, unsigned long** ppSuccessors)
{
    CSWSArea* pArea = GetArea();
    if (pArea == NULL)
    {
        return 0;
    }
    int nCount = pArea->CountAreaTransitionTriggers(
        NULL, pPathfind->GetAreaListCount(), pPathfind->GetAreaList());
    if (nCount == 0)
    {
        return 0;
    }
    *ppSuccessors = new unsigned long[nCount];
    return pArea->CountAreaTransitionTriggers(
        *ppSuccessors, pPathfind->GetAreaListCount(), pPathfind->GetAreaList());
}
// PC 0x00504120
inline CResIFO::CResIFO()
{
    strncpy(m_szFileType, "IFO ", 4);
}
// PC 0x00504150
CSWSArea* CSWSModule::GetArea()
{
    if (m_oidAreaCached != m_oidArea)
    {
        CServerExoApp* pExoApp = g_pAppManager->m_pServerExoApp;

        m_pAreaCached = pExoApp->GetAreaByGameObjectID(m_oidArea);
    }

    if (m_pAreaCached != NULL)
    {
        m_oidAreaCached = m_oidArea;
    }
    else
    {
        m_pAreaCached = NULL;
        m_oidAreaCached = OBJECT_INVALID;
    }
    return m_pAreaCached;
}
// PC 0x005041B0
void CSWSModule::ClearAreaCache()
{
    m_pAreaCached = NULL;
    m_oidAreaCached = OBJECT_INVALID;
}
// PC 0x005041D0
void CSWSModule::PackModuleIntoMessage(unsigned long nPlayerId)
{
    CSWSMessage* pMessage = g_pAppManager->m_pServerExoApp->GetSWSMessage();

    {
        CExoString sModuleName(strchr(m_sModuleName.CStr(), ':') + 1);

        pMessage->WriteCExoString(sModuleName, 32);
    }
    pMessage->WriteCExoLocStringServer(m_lsName, 0);
    pMessage->WriteBYTE(m_nMinutesPerHour, 8);
    pMessage->WriteCResRef(m_cHak, 16);
    pMessage->WriteCResRef(m_cStartMovie, 16);
}
// PC 0x005042C0
int CSWSModule::PlotPathInArea(CPathfindInformation* pPathfind,
                              unsigned long nFlags)
{
    float fStartX = pPathfind->m_fEndX;
    float fStartY = pPathfind->m_fEndY;
    float fEndX = pPathfind->m_fStartX;
    float fEndY = pPathfind->m_fStartY;
    float fDistance = pPathfind->m_fStepScale;
    float fWidth = pPathfind->m_fHalfStep;
    CSWSArea* pArea = GetArea();
    int nDirectLine;
    int nResult;

    if (pArea == NULL || pArea->GetId() != pPathfind->m_nEndArea)
    {
        return CSWSObject::ACTION_FAILED;
    }
    ++pPathfind->m_nField_3C;
    pArea->m_pPathfindInformation = pPathfind;
    if (pPathfind->m_fStartDistance > NWSMODULE_PATH_GOAL_MARGIN &&
        pPathfind->m_nField_240 == 0)
    {
        Vector vGoal(fEndX, fEndY, 0.0f);
        Vector vNewGoal = vGoal;
        Vector vDistance = vGoal - Vector(fStartX, fStartY, 0.0f);
        float fLengthSquared = vDistance * vDistance;
        float fGoalDistance = pPathfind->m_fStartDistance;

        float fGoalDistanceSquared = fGoalDistance * fGoalDistance;
        if (fLengthSquared >= NWSMODULE_PATH_LENGTH_MIN * NWSMODULE_PATH_LENGTH_MIN)
        {
            if (fLengthSquared > fGoalDistanceSquared)
            {
                vNewGoal = vGoal + (fGoalDistance - NWSMODULE_PATH_GOAL_MARGIN) *
                    normalize(Vector(fStartX - fEndX, fStartY - fEndY, 0.0f));
            }
            else
            {
                vNewGoal = vGoal + (float)sqrt(fLengthSquared) *
                    normalize(Vector(fStartX - fEndX, fStartY - fEndY, 0.0f));
            }
        }
        fEndX = vNewGoal.m_fX;
        fEndY = vNewGoal.m_fY;
    }
    if (pPathfind->m_nField_3C == 1)
    {
        nDirectLine = pArea->TestDirectLine(Vector(fStartX, fStartY, 0.0f),
            Vector(fEndX, fEndY, 0.0f), fDistance, fWidth, 1, NULL);
        pPathfind->m_nDirectLineResult = nDirectLine;
    }
    else
    {
        nDirectLine = pPathfind->m_nDirectLineResult;
    }
    if (pPathfind->m_nField_198 == 1)
    {
        if (nDirectLine != CSWSArea::DIRECT_LINE_CLEAR &&
            pPathfind->m_nField_84 == 0)
        {
            float fFraction = 0.5f;
            float fStep = 0.25f;
            int bFound = 0;
            float fDeltaX = fEndX - fStartX;
            float fDeltaY = fEndY - fStartY;
            float fBestX, fBestY;
            do
            {
                float fTestX = fDeltaX * fFraction + fStartX;
                float fTestY = fDeltaY * fFraction + fStartY;
                if (pArea->TestDirectLine(Vector(fStartX, fStartY, 0.0f),
                    Vector(fTestX, fTestY, 0.0f), fDistance, fWidth, 1, NULL) ==
                    CSWSArea::DIRECT_LINE_CLEAR)
                {
                    fFraction += fStep;
                    fBestX = fTestX;
                    fBestY = fTestY;
                    bFound = 1;
                }
                else
                {

                    if (bFound)
                        fFraction -= fStep;
                    else
                        fFraction -= fStep;
                }
                fStep *= 0.5f;
            } while (fStep > NWSMODULE_PARTIAL_PATH_SEARCH_MIN);
            if (bFound == 1)
            {
                fEndX = fBestX;
                fEndY = fBestY;
                nDirectLine = CSWSArea::DIRECT_LINE_CLEAR;
            }
            else
            {
                nDirectLine = PATH_PARTIAL_SEARCH_FAILED;
            }
        }
        pPathfind->m_nField_194 = 0;
        pPathfind->m_nField_198 = 0;
    }
    if (nDirectLine == CSWSArea::DIRECT_LINE_CLEAR)
    {
        pPathfind->m_nWayPointCount = 4;
        float* pPoints = new float[4];
        pPoints[0] = fStartX;
        pPoints[1] = fStartY;
        pPoints[2] = fEndX;
        pPoints[3] = fEndY;
        pPathfind->m_pWayPoints = pPoints;
        nResult = CSWSObject::ACTION_COMPLETE;
    }
    else
    {
        CSWSCreature* pCreature = g_pAppManager->m_pServerExoApp->
            GetCreatureByGameObjectID(pPathfind->m_oidPathfindCreature);
        if (pCreature != NULL && pCreature->m_nAILevel == CServerAIMaster::AI_LEVEL_EMPTY_AREA_CREATURE)
        {
            nResult = CSWSObject::ACTION_FAILED;
        }
        else
        {

            int bRenderAi;
            if (pPathfind->m_nField_194 != 0)
            {
                nResult = pArea->PlotGridPath(pPathfind, nFlags);
                if (RenderAi)
                    g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(
                        pPathfind->m_oidPathfindCreature);
                bRenderAi = RenderAi;
            }
            else if (nDirectLine == CSWSArea::DIRECT_LINE_BLOCKED_OBJECT ||
                     nDirectLine == CSWSArea::DIRECT_LINE_BLOCKED_ROOM ||
                     nDirectLine == CSWSArea::DIRECT_LINE_BLOCKED_CREATURE ||
                     nDirectLine == CSWSArea::DIRECT_LINE_NO_ROOM)
            {
                if ((unsigned long)pArea->m_nPathPointCount > 0)
                {
                    if (RenderAi)
                        g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(
                            pPathfind->m_oidPathfindCreature);
                    nResult = pArea->PlotPathPointPath(pPathfind, nFlags);
                }
                else
                {
                    if (RenderAi)
                        g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(
                            pPathfind->m_oidPathfindCreature);
                    nResult = pArea->PlotGridPath(pPathfind, nFlags);
                }
                bRenderAi = RenderAi;
            }
            else
            {
                nResult = CSWSObject::ACTION_FAILED;
                bRenderAi = RenderAi;
            }
            if (bRenderAi)
            {

                unsigned long oidCreature = pPathfind->m_oidPathfindCreature;
                if (pArea == NULL && oidCreature != 0)
                    return CSWSObject::ACTION_FAILED;
                g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(
                    oidCreature);
            }
            if (pPathfind->m_nField_3C > PATH_MAX_ATTEMPTS &&
                nResult == CSWSObject::ACTION_IN_PROGRESS)
                nResult = CSWSObject::ACTION_FAILED;
        }
    }
    if (nResult == CSWSObject::ACTION_FAILED)
    {
        if (pPathfind->m_nPathFlags == 0)
        {
            fStartX = pPathfind->m_fEndX;
            fStartY = pPathfind->m_fEndY;
            Vector vEnd(pPathfind->m_fStartX, pPathfind->m_fStartY, 0.0f);
            Vector vStart(fStartX, fStartY, 0.0f);
            pPathfind->m_nField_84 = 0;

            int bFound = 0;
            Vector vBest;
            Vector vDelta = vEnd - vStart;

            EXOLOG("PlotPathInArea: partial path search, delta %p", &vDelta);
            float fFraction = 0.5f;
            float fStep = 0.25f;
            if (nDirectLine != CSWSArea::DIRECT_LINE_BLOCKED_ROOM)
            {
                do
                {
                    vEnd = vStart + fFraction * vDelta;
                    if (pArea->TestDirectLine(Vector(fStartX, fStartY, 0.0f), vEnd,
                        fDistance, fWidth, 0, NULL) == CSWSArea::DIRECT_LINE_CLEAR)
                    {
                        vBest = vEnd;
                        fFraction += fStep;
                        bFound = 1;
                    }
                    else
                    {

                        if (pArea > NULL)
                            fFraction -= fStep;
                        else
                            fFraction -= fStep;
                    }
                    fStep *= 0.5f;
                } while (fStep > NWSMODULE_PARTIAL_PATH_SEARCH_MIN);
                if (bFound)
                {
                    pPathfind->m_nWayPointCount = 4;
                    float* pPoints = new float[4];
                    pPoints[0] = fStartX;
                    pPoints[1] = fStartY;
                    pPoints[2] = vBest.m_fX;
                    pPoints[3] = vBest.m_fY;
                    pPathfind->m_pWayPoints = pPoints;
                    nResult = CSWSObject::ACTION_COMPLETE;
                }
            }
        }
    }
    if (nResult == CSWSObject::ACTION_COMPLETE && pPathfind->m_nField_84 == 1)
    {
        int nCount = pPathfind->m_nWayPointCount;
        float* pOldPoints = pPathfind->m_pWayPoints;
        float* pNewPoints = new float[nCount];
        for (int i = 0; i < nCount; i += 2)
        {
            int nSource = nCount - i - 2;
            pNewPoints[i] = pOldPoints[nSource];
            pNewPoints[i + 1] = pOldPoints[nSource + 1];
        }
        pPathfind->m_pWayPoints = pNewPoints;
        if (pOldPoints != NULL)
            delete[] pOldPoints;
        pPathfind->m_nField_84 = 0;
        pPathfind->FlipStartEndPoints();
    }
    if (nResult == CSWSObject::ACTION_FAILED ||
        nResult == CSWSObject::ACTION_COMPLETE)
        pPathfind->m_nField_88 = 1;
    return nResult;
}
// PC 0x005049D0
int CSWSModule::SaveModuleFAC()
{
    CExoString sFileName;
    CServerExoApp* pExoApp = g_pAppManager->m_pServerExoApp;
    CServerExoAppInternal* pInternal = pExoApp->m_pcExoAppInternal;
    CFactionManager* pFactionManager = pInternal->m_pFactionManager;
    CResGFF* pGFF = new CResGFF;
#ifdef _XBOX
    CResStruct* pStruct = new CResStruct();
#else
    CResStruct* pStruct = new CResStruct;
#endif
    CResList cList;
    int bSuccess = pGFF->CreateGFFFile(pStruct, "FAC ", "V2.0");

    if (bSuccess == 1)
    {
        pGFF->AddList(&cList, pStruct, "FactionList");
        pFactionManager->SaveFactions(pGFF, &cList);
        pGFF->AddList(&cList, pStruct, "RepList");
        pFactionManager->SaveReputations(pGFF, &cList);
        sFileName = "GAMEINPROGRESS:REPUTE";
        pGFF->WriteGFFFile(sFileName.CStr(), 2038);
        delete pGFF;
    }
    else
    {
        delete pGFF;
    }
    delete pStruct;
    return bSuccess;
}
// PC 0x00504B80
int CSWSModule::SaveModuleInProgress()
{
    CExoString sName;
    CExoString sAreaName;
    CExoString sGitName;

    if (m_pERFFile == NULL)
    {
        return 0;
    }
    sAreaName = "tmparea";
    sGitName = "tmpgit";
    CServerExoApp* pExoApp = g_pAppManager->m_pServerExoApp;
    CSWSArea* pArea = pExoApp->GetAreaByGameObjectID(m_oidArea);
    sName = sGitName;
    pArea->SaveGIT(m_pERFFile, sName, *m_plstPlayers);
    CSWReentrantServerStats* pStats = g_pAppManager->m_pReentrantServerStats;
    pStats->m_nProgress = 1;
    CSWReentrantServerStats* pStatsNow = g_pAppManager->m_pReentrantServerStats;
    unsigned long nProgress = pStatsNow->m_nProgressTotal;
    pExoApp = g_pAppManager->m_pServerExoApp;
    pExoApp->GetSWSMessage()->SendServerToPlayerLoadBar_UpdateStallEvent(
        1, nProgress);
    return 1;
}
// PC 0x00504CE0
void CSWSModule::SavePrimaryPlayerInfo(CSWSCreature* pCreature,
                                      const CExoString& sPath)
{
    CSWSPlayer* pPlayer = g_pAppManager->m_pServerExoApp->
        GetClientObjectByObjectId(pCreature->m_nId);
    CResGFF* pGFF = new CResGFF;
#ifdef _XBOX
    CResStruct* pStruct = new CResStruct();
#else
    CResStruct* pStruct = new CResStruct;
#endif
    int bSuccess = pGFF->CreateGFFFile(pStruct, "BIC ", "V2.0");

    if (bSuccess)
    {
        CExoString sFileName;

        pGFF->WriteFieldCExoString(pStruct, pPlayer->GetPlayerName(),
                                  "Mod_CommntyName");
        int bIsPrimaryPlayer = pPlayer->m_bIsAllowedToSave;
        pGFF->WriteFieldBYTE(pStruct, bIsPrimaryPlayer,
                             "Mod_IsPrimaryPlr");
        unsigned long oidPlayer = pCreature->m_nId;
        pGFF->WriteFieldDWORD(pStruct, oidPlayer, "ObjectId");
        pCreature->SaveCreature(pGFF, pStruct);
        sFileName = sPath + "\\" + "Player";
        pGFF->WriteGFFFile(sFileName, RESTYPE_BIC);
    }
    delete pGFF;
}
// PC 0x00504F10
void CSWSModule::SetIntraAreaGoal(CPathfindInformation* pPathfind)
{
    if (pPathfind->m_nField_A8 > 1)
    {
        pPathfind->m_fTempStartX = pPathfind->m_fStartX;
        pPathfind->m_fTempStartY = pPathfind->m_fStartY;
        pPathfind->m_fTempStartZ = pPathfind->m_fStartZ;
        pPathfind->m_nTempArea = pPathfind->m_nStartArea;
        pPathfind->m_fTempDistance = pPathfind->m_fStartDistance;

        CGameObject* pObject;
        int* pTransitions = pPathfind->m_pAreaPathCost;
        CServerExoApp* pExoApp = g_pAppManager->m_pServerExoApp;
        if (pExoApp->GetObjectArray()->GetGameObject(
                pTransitions[0], &pObject) ==
            CGameObjectArray::SUCCESS)
        {
            if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_TRIGGER)
            {
                CSWSTrigger* pTrigger = pObject->AsSWSTrigger();
                Vector vEnd(pPathfind->m_fEndX, pPathfind->m_fEndY,
                            pPathfind->m_fEndZ);
                Vector vGoal;
                if (pTrigger->GetClosestPointWithinTrigger(
                        vEnd, &vGoal) == 1)
                {
                    pPathfind->m_fStartX = vGoal.m_fX;
                    pPathfind->m_fStartY = vGoal.m_fY;
                    pPathfind->m_fStartZ = vGoal.m_fZ;
                    pPathfind->m_fStartDistance = 0.0f;
                    pPathfind->ComputeStepTolerance();
                    pPathfind->m_nStartArea = pPathfind->m_nEndArea;
                    pPathfind->m_oidPathfindTargetCreature = pTrigger->GetId();
                }
            }
            else if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR)
            {
                CSWSDoor* pDoor = pObject->AsSWSDoor();
                Vector vGoal = pDoor->GetNearestActionPoint(
                    Vector(pPathfind->m_fEndX, pPathfind->m_fEndY,
                           pPathfind->m_fEndZ), 1);
                pPathfind->m_fStartX = vGoal.m_fX;
                pPathfind->m_fStartY = vGoal.m_fY;
                pPathfind->m_fStartZ = vGoal.m_fZ;
                pPathfind->m_fStartDistance = pPathfind->m_fStepScale +
                    NWSMODULE_DOOR_GOAL_DISTANCE_MARGIN;
                pPathfind->ComputeStepTolerance();
                pPathfind->m_nStartArea = pPathfind->m_nEndArea;
                pPathfind->m_oidPathfindTargetCreature = pDoor->GetId();
            }
        }
    }
}
// PC 0x00506780
int CSWSModule::InterAreaDFS(int nArea, int nDepth,
                            CPathfindInformation* pPathfind)
{
    if (pPathfind->m_pAreaPath[nArea] == pPathfind->m_nStartArea)
    {
        pPathfind->m_nField_A8 = nArea + 1;
        return 1;
    }
    if (nArea >= nDepth)
    {
        return 0;
    }
    for (int nLevel = 0; nLevel < nArea; nLevel++)
    {
        if (pPathfind->m_pAreaPath[nArea] == pPathfind->m_nStartArea)
        {
            return 0;
        }
    }
    unsigned long* pSuccessors = NULL;
    int nCount = GenerateInterAreaDFSSuccessors(nArea, pPathfind, &pSuccessors);
    CGameObjectArray* pArray = g_pAppManager->m_pServerExoApp->GetObjectArray();
    for (int i = 0; i < nCount; i++)
    {
        pPathfind->m_pAreaPathCost[nArea] = pSuccessors[i];
        CGameObject* pObject;
        if (pArray->GetGameObject(pSuccessors[i], &pObject) ==
            CGameObjectArray::SUCCESS)
        {
            if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_TRIGGER)
            {
                CSWSTrigger* pTrigger = pObject->AsSWSTrigger();
                if (pTrigger->m_nTriggerType != 0)
                {
                    if (pArray->GetGameObject(pTrigger->GetTargetArea(), &pObject) ==
                        CGameObjectArray::SUCCESS)
                    {
                        CSWSArea* pArea = pObject->AsSWSArea();
                        if (pArea != NULL)
                        {
                            pPathfind->m_pAreaPath[nArea + 1] = pArea->GetId();
                            if (InterAreaDFS(nArea + 1, nDepth, pPathfind) == 1)
                            {
                                if (pSuccessors != NULL)
                                    delete[] pSuccessors;
                                return 1;
                            }
                        }
                    }
                }
            }
            else if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR)
            {
                CSWSDoor* pDoor = pObject->AsSWSDoor();
                if (pDoor != NULL && pDoor->GetIsLinked())
                {
                    CSWSObject* pLinkedObject = pDoor->GetLinkedObject();
                    if (pLinkedObject != NULL)
                    {
                        CSWSArea* pArea = pLinkedObject->GetArea();
                        if (pArea != NULL)
                        {
                            pPathfind->m_pAreaPath[nArea + 1] = pArea->GetId();
                            if (InterAreaDFS(nArea + 1, nDepth, pPathfind) == 1)
                            {
                                if (pSuccessors != NULL)
                                    delete[] pSuccessors;
                                return 1;
                            }
                        }
                    }
                }
            }
        }
    }
    if (pSuccessors != NULL)
        delete[] pSuccessors;
    return 0;
}
// PC 0x00506990
int CSWSModule::LoadModuleInProgress(int nAreasLoaded, int nAreasToLoad)
{
    CSWSArea* pArea;
    unsigned long oidArea;

    if (m_bIsSaveGame)
    {
        oidArea = m_oidArea;
    }
    if (m_bIsSaveGame)
    {
        pArea = new CSWSArea(m_cAreaResRef, 0, oidArea);
    }
    else
    {
        pArea = new CSWSArea(m_cAreaResRef, 0, OBJECT_INVALID);
    }
    if (pArea == NULL)
    {
        m_pRes->Release();
        RemoveModuleResources(m_sModuleName);
        return 3;
    }
    if (!pArea->LoadArea(m_bIsSaveGame))
    {
        m_pRes->Release();
        RemoveModuleResources(m_sModuleName);
        delete pArea;
        return 4;
    }
    if (!m_bIsSaveGame)
    {
        m_oidArea = pArea->m_nId;
    }
    g_pAppManager->m_pReentrantServerStats->m_nProgress = nAreasLoaded + 1;
    g_pAppManager->m_pServerExoApp->GetSWSMessage()
        ->SendServerToPlayerLoadBar_UpdateStallEvent(nAreasLoaded + 1, -1);
    return 0;
}
// PC 0x00506AF0
int CSWSModule::LoadModuleFinish()
{
    CResStruct cTopLevelStruct;

    m_pRes->GetTopLevelStruct(&cTopLevelStruct);
    CSWReentrantServerStats* pStats = g_pAppManager->m_pReentrantServerStats;
    int nProgress = pStats->m_nProgressTotal;
    pStats->m_nProgress = nProgress;
    CServerExoApp* pExoApp = g_pAppManager->m_pServerExoApp;
    pExoApp->GetSWSMessage()->SendServerToPlayerLoadBar_UpdateStallEvent(
        nProgress, -1);
    PostProcess();

    CScriptEvent* pEvent = new CScriptEvent;

    pEvent->m_nType = 17;
    pExoApp = g_pAppManager->m_pServerExoApp;
    pExoApp->GetServerAIMaster()->AddEventDeltaTime(
        0, 0, m_nId, m_nId, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
    m_pRes->Release();
    m_n1D8 = 1;
    if (g_nSetMovieFrameRate != 0)
    {
        SetFramesPerSecond("30");
        displayFPS = (float)g_nSetMovieFrameRate;
        g_bEnableAurPostString = 0;
    }
    return 0;
}

void CSWSModule::PostProcess()
{
}
// PC 0x00506C00
int CSWSModule::SaveStatic(CERFFile* pERFFile, const CExoString& sFileType,
                           unsigned short nResType, int bIsGFF)
{
    int bCreated = 0;
    int bSuccess = 1;
    CRes* pModule = g_pExoResMan->GetResObject("Module", RESTYPE_IFO);
    CExoStringList* pResources = g_pExoResMan->GetResOfType(nResType, pModule);

    if (pResources != NULL)
    {
        for (int nResource = 0; nResource < pResources->GetCount(); nResource++)
        {
            CExoString* pName = pResources->GetString(nResource);
            CRes* pRes = g_pExoResMan->GetResObject(
                (char*)pName->CStr(), nResType);

            if (pRes == NULL)
            {
                if (bIsGFF)
                {
                    CResGFF* pGFF = new CResGFF;
                    pRes = pGFF;
                    const char* szFileType = sFileType.CStr();
                    strncpy(pGFF->m_szFileType, szFileType, 4);
                }
                else
                {
                    pRes = new CRes;
                }
                g_pExoResMan->SetResObject((char*)pName->CStr(), nResType, pRes);
                bCreated = 1;
            }

            if (pRes != NULL)
            {
                if (pRes->Demand() != NULL)
                {
                    pERFFile->WriteResource((char*)pName->CStr(), nResType,
                                            pRes, bIsGFF, (void*)-1);
                    pRes->Release();
                }
                else
                {
                    bSuccess = 0;
                }
                g_pExoResMan->ReleaseResObject(pRes);
            }
            else
            {
                bSuccess = 0;
            }

            if (bCreated)
            {
                bCreated = 0;
                delete pRes;
            }
        }
    }
    g_pExoResMan->ReleaseResObject(pModule);
    delete pResources;
    return bSuccess;
}
// PC 0x00507E20
int CSWSModule::AIUpdate()
{
    unsigned long nDiffDay;
    unsigned long nDiffTime;
    unsigned long nTime;
    unsigned long nDay;
    CServerExoApp* pExoApp;

    pExoApp = g_pAppManager->m_pServerExoApp;
    pExoApp->GetWorldTimer()->GetWorldTime(&nDay, &nTime);
    pExoApp = g_pAppManager->m_pServerExoApp;
    pExoApp->GetWorldTimer()->SubtractWorldTimes(
        nDay, nTime, m_nHeartbeatDay, m_nHeartbeatTime, &nDiffDay, &nDiffTime);
    if (nDiffTime >= 6000)
    {
        if (m_nHeartbeatTime != 0)
        {
            g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_HEARTBEAT], m_nId, 1);
        }
        m_nHeartbeatDay = nDay;
        m_nHeartbeatTime = nTime;
        return 1;
    }
    pExoApp = g_pAppManager->m_pServerExoApp;
    pExoApp->GetWorldTimer()->SubtractWorldTimes(nDay, nTime, m_nLastUpdateDay,
                                                 m_nLastUpdateTime, &nDiffDay,
                                                 &nDiffTime);
    UpdateTime(nDay, nTime, nDiffTime);
    m_nLastUpdateDay = nDay;
    m_nLastUpdateTime = nTime;
    if (g_pAppManager->m_pServerExoApp->GetActivePauseState() == 1)
    {
        TimeStopSanityCheck();
    }
    return 0;
}
// PC 0x00507F50
int CSWSModule::DoUpdate()
{
    CServerExoApp* pExoApp;
    CSWSArea* pArea;

    if (m_bAIUpdate != 0)
    {
        m_bAIUpdate = 0;
        return AIUpdate();
    }

    pExoApp = g_pAppManager->m_pServerExoApp;
    pArea = pExoApp->GetAreaByGameObjectID(m_oidArea);
    if (pArea != NULL)
    {
        pArea->AIUpdate();
    }
    m_bAIUpdate = 1;
}
// PC 0x00507F90
unsigned long CSWSModule::GetWaypoint(const CExoString& sTag)
{
    return FindObjectByTagTypeOrdinal(sTag, CGameObject::OBJECT_TYPE_WAYPOINT,
                                      0);
}
// PC 0x00507FB0
int CSWSModule::PlotInterAreaPath(CPathfindInformation* pPathfind,
                                  unsigned long nFlags)
{
    int nResult;
    int nCount;
    unsigned long* pNewList;
    int* pStart;
    int i;

    if (pPathfind->m_nStartArea == pPathfind->m_nEndArea)
    {
        if (pPathfind->m_pAreaPath == NULL)
        {
            pPathfind->m_nField_A0 = 1;
            pPathfind->m_nField_A8 = 1;
            pStart = new int[1];
            pStart[0] = pPathfind->m_nStartArea;
            pPathfind->m_pAreaPath = pStart;
            pPathfind->m_pAreaPathCost = NULL;
        }
    }
    else
    {
        if (pPathfind->m_pAreaPath == NULL)
        {
            if (ComputeInterAreaPath(pPathfind) == 0)
            {
                return CSWSObject::ACTION_FAILED;
            }
        }
        else
        {
            pPathfind->FlipTempEndPoints();
        }
    }
    nResult = PlotPathInArea(pPathfind, nFlags);
    if (nResult == CSWSObject::ACTION_FAILED)
    {
        if (pPathfind->m_nField_A8 > 1)
        {
            nCount = pPathfind->m_nField_B4;
            pNewList = new unsigned long[nCount + 1];
            for (i = 0; i < nCount; i++)
            {
                pNewList[i] = pPathfind->m_pAreaList[i];
            }
            pNewList[nCount] = pPathfind->m_pAreaPathCost[0];
            pPathfind->m_nField_B4 = nCount + 1;
            if (pPathfind->m_pAreaList != NULL)
            {
                delete[] pPathfind->m_pAreaList;
            }
            pPathfind->m_pAreaList = pNewList;
            pPathfind->ResetInterAreaPathSearchData();
            pPathfind->FlipTempEndPoints();
            if (ComputeInterAreaPath(pPathfind) == 0)
            {
                return CSWSObject::ACTION_FAILED;
            }
            nResult = CSWSObject::ACTION_IN_PROGRESS;
            pPathfind->FlipTempEndPoints();
            return nResult;
        }
    }
    else
    {
        if (pPathfind->m_nField_A8 > 1)
        {
            pPathfind->FlipTempEndPoints();
        }
    }
    return nResult;
}
// PC 0x00508100
int CSWSModule::PlotPath(CPathfindInformation* pPathfind, unsigned long nFlags)
{
    int nResult;
    int nSavedField;
    unsigned long oidCreature;
    CSWSCreature* pCreature;

    if (pPathfind == NULL)
    {
        return CSWSObject::ACTION_FAILED;
    }
    if (pPathfind->m_nField_58 == 0 && pPathfind->m_nField_88 == 1)
    {
        return CSWSObject::ACTION_COMPLETE;
    }
    if (pPathfind->m_nField_58 == 1)
    {
        if (pPathfind->m_nField_A4 == 1)
        {
            pPathfind->ResetInterTileSearchData();
            pPathfind->ResetGridSearchData();
            pPathfind->m_nField_A4 = 0;
        }
        else
        {
            nSavedField = pPathfind->m_nField_84;
            pPathfind->ResetInterAreaSearchData();
            pPathfind->m_nField_84 = nSavedField;
        }
        pPathfind->m_nField_3C = 0;
        pPathfind->m_nField_88 = 0;
        pPathfind->m_nField_58 = 0;
    }
    g_pAppManager->m_pServerExoApp->GetServerAIMaster()->m_nLastEvent[0] = 1;
    nResult = PlotInterAreaPath(pPathfind, nFlags);
    if (RenderAi)
    {
        oidCreature = pPathfind->m_oidPathfindCreature;
        pCreature = g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(
            oidCreature);
        if (nResult == CSWSObject::ACTION_COMPLETE)
        {
            pCreature->ComputeTotalWalkDistance();
            pCreature->ComputeTotalWalkDistance();
        }
    }
    return nResult;
}
// PC 0x00509800
void CSWSModule::SaveModuleStart(CExoString& sPath, CExoString& sName)
{
    m_bIsSaveGame = 1;
    CExoString sFileName = g_pExoBase->m_pExoAliasList->ResolveFileName(
        sName, RESTYPE_RSV);
    DeleteFileA(sFileName.CStr());

    m_pERFFile = new CERFFile;
    EXOXBOXASSERT(m_pERFFile);
    m_plstPlayers = new CExoArrayList<unsigned long>(0);
    m_pERFFile->Create(sName);
    m_pERFFile->SetVersion("MOD V1.0");
    m_pERFFile->WriteHeader();
    m_pERFFile->WriteStringTable();
    m_nResourceCount = 3;
    m_pERFFile->SetNumEntries(3);
    m_pModuleGFF = new CResGFF;
#ifdef _XBOX
    m_pERFStruct = new CResStruct();
#else
    m_pERFStruct = new CResStruct;
#endif
    if (m_pModuleGFF->CreateGFFFile(m_pERFStruct, "IFO ", "V2.0"))
    {
        SaveModuleIFOStart(m_pModuleGFF, m_pERFStruct);
        SaveModuleFAC();
    }
    else
    {
        delete m_pERFFile;
        m_pERFFile = NULL;
        delete m_pModuleGFF;
        m_pModuleGFF = NULL;
        delete m_pERFStruct;
        m_pERFStruct = NULL;
    }
}
// PC 0x005081E0
int CSWSModule::SaveModuleIFOStart(CResGFF* pRes, CResStruct* pStruct)
{
    CExoLocString lsDefault;
    CResList cList;
    CResStruct cElement;
    CExoLinkedListNode* pPos;
    unsigned long nCalendarDay, nTimeOfDay;
    unsigned long nHour, nMinute, nSecond, nMillisecond;

    CGameObjectArray* pObjectArray =
        g_pAppManager->GetServerExoApp()->GetObjectArray();
    pRes->WriteFieldVOID(pStruct, m_pModuleInfo->m_aModuleID, 32, "Mod_ID");
    pRes->WriteFieldINT(pStruct, m_pModuleInfo->m_nCreatorID, "Mod_Creator_ID");
    pRes->WriteFieldDWORD(pStruct, m_pModuleInfo->m_nVersion, "Mod_Version");
    pRes->WriteFieldCExoLocString(pStruct, m_lsName, "Mod_Name");
    pRes->WriteFieldCExoLocString(pStruct, m_sDescription, "Mod_Description");
    pRes->WriteFieldBYTE(pStruct, m_bIsSaveGame, "Mod_IsSaveGame");
    pRes->WriteFieldBYTE(pStruct, m_bIsNWMFile, "Mod_IsNWMFile");
    if (m_bIsNWMFile)
        pRes->WriteFieldCExoString(pStruct, m_sSaveName, "Mod_NWMResName");
    pRes->WriteFieldCExoString(pStruct, m_cHak.GetResRefStr(), "Mod_Hak");
    pRes->WriteFieldDWORD64(pStruct, CGameEffect::m_nNextID, "Mod_Effect_NxtId");
    unsigned long oidNext0 = pObjectArray->m_nPrevObjectId[0];
    pRes->WriteFieldDWORD(pStruct, oidNext0, "Mod_NextCharId0");
    unsigned long oidNext1 = pObjectArray->m_nPrevObjectId[1];
    pRes->WriteFieldDWORD(pStruct, oidNext1, "Mod_NextCharId1");
    unsigned long oidNext2 = pObjectArray->m_nNextObjectId[0];
    pRes->WriteFieldDWORD(pStruct, oidNext2, "Mod_NextObjId0");
    unsigned long oidNext3 = pObjectArray->m_nNextObjectId[1];
    pRes->WriteFieldDWORD(pStruct, oidNext3, "Mod_NextObjId1");
    pRes->WriteFieldCExoString(pStruct, m_sTag, "Mod_Tag");
    pRes->WriteFieldCResRef(pStruct, m_pEntryInfo->m_cEntryArea, "Mod_Entry_Area");
    pRes->WriteFieldFLOAT(pStruct, m_pEntryInfo->m_fEntryX, "Mod_Entry_X");
    pRes->WriteFieldFLOAT(pStruct, m_pEntryInfo->m_fEntryY, "Mod_Entry_Y");
    pRes->WriteFieldFLOAT(pStruct, m_pEntryInfo->m_fEntryZ, "Mod_Entry_Z");
    pRes->WriteFieldFLOAT(pStruct, m_pEntryInfo->m_fEntryDirX, "Mod_Entry_Dir_X");
    pRes->WriteFieldFLOAT(pStruct, m_pEntryInfo->m_fEntryDirY, "Mod_Entry_Dir_Y");
    pRes->WriteFieldBYTE(pStruct, m_nMinutesPerHour, "Mod_MinPerHour");
    pRes->WriteFieldBYTE(pStruct, m_nDawnHour, "Mod_DawnHour");
    pRes->WriteFieldBYTE(pStruct, m_nDuskHour, "Mod_DuskHour");
    CServerExoApp* pExoApp = g_pAppManager->GetServerExoApp();
    pExoApp->GetWorldTimer()->GetWorldTime(&nCalendarDay, &nTimeOfDay);
    pExoApp = g_pAppManager->GetServerExoApp();
    pExoApp->GetWorldTimer()->ConvertFromTimeOfDay(
        nTimeOfDay, &nHour, &nMinute, &nSecond, &nMillisecond);
    pExoApp = g_pAppManager->GetServerExoApp();
    unsigned long nPauseDay =
        pExoApp->GetWorldTimer()->m_nPausedCalendarDay;
    pExoApp = g_pAppManager->GetServerExoApp();
    unsigned long nPauseTime =
        pExoApp->GetWorldTimer()->m_nPausedTimeOfDay;
    pRes->WriteFieldDWORD(pStruct, m_nYear, "Mod_StartYear");
    pRes->WriteFieldBYTE(pStruct, m_nMonth, "Mod_StartMonth");
    pRes->WriteFieldBYTE(pStruct, m_nDay, "Mod_StartDay");
    pRes->WriteFieldBYTE(pStruct, m_nTime, "Mod_StartHour");
    pRes->WriteFieldDWORD(pStruct, m_nTime2, "Mod_Transition");
    pRes->WriteFieldWORD(pStruct, nMinute, "Mod_StartMinute");
    pRes->WriteFieldWORD(pStruct, nSecond, "Mod_StartSecond");
    pRes->WriteFieldWORD(pStruct, nMillisecond, "Mod_StartMiliSec");
    pRes->WriteFieldDWORD(pStruct, nPauseDay, "Mod_PauseDay");
    pRes->WriteFieldDWORD(pStruct, nPauseTime, "Mod_PauseTime");
    pRes->WriteFieldBYTE(pStruct, m_nXPScale, "Mod_XPScale");
    SaveLimboCreatures(pRes, pStruct);

    pRes->AddList(&cList, pStruct, "Mod_Expan_List");
    pPos = m_lstModuleExpansion.GetHeadPos();
    while (pPos != NULL)
    {
        NWModuleExpansion_st* pExpansion = m_lstModuleExpansion.GetAtPos(pPos);
        pRes->AddListElement(&cElement, &cList, GFF_STRUCT_EXPANSION);
        pRes->WriteFieldCExoLocString(&cElement, pExpansion->m_lsExpansionName, "Expansion_Name");
        pRes->WriteFieldINT(&cElement, pExpansion->m_nExpansionID, "Expansion_ID");
        m_lstModuleExpansion.GetNext(pPos);
    }
    pRes->AddList(&cList, pStruct, "Mod_CutSceneList");
    pPos = m_lstModuleCutScene.GetHeadPos();
    while (pPos != NULL)
    {
        NWModuleCutScene_st* pCutScene = m_lstModuleCutScene.GetAtPos(pPos);
        pRes->AddListElement(&cElement, &cList, GFF_STRUCT_CUTSCENE);
        pRes->WriteFieldCResRef(&cElement, pCutScene->m_cCutSceneName, "CutScene_Name");
        pRes->WriteFieldDWORD(&cElement, pCutScene->m_nCutSceneID, "CutScene_ID");
        m_lstModuleCutScene.GetNext(pPos);
    }
    {
        CExoString* psScript = &m_sScripts[SCRIPT_HEARTBEAT];
        CResRef cScript(*psScript);
        pRes->WriteFieldCResRef(pStruct, cScript, "Mod_OnHeartbeat");
    }
    {
        CExoString* psScript = &m_sScripts[SCRIPT_USER_DEFINED];
        CResRef cScript(*psScript);
        pRes->WriteFieldCResRef(pStruct, cScript, "Mod_OnUsrDefined");
    }
    {
        CExoString* psScript = &m_sScripts[SCRIPT_CLIENT_ENTER];
        CResRef cScript(*psScript);
        pRes->WriteFieldCResRef(pStruct, cScript, "Mod_OnClientEntr");
    }
    {
        CExoString* psScript = &m_sScripts[SCRIPT_CLIENT_LEAVE];
        CResRef cScript(*psScript);
        pRes->WriteFieldCResRef(pStruct, cScript, "Mod_OnClientLeav");
    }
    {
        CExoString* psScript = &m_sScripts[SCRIPT_ACTIVATE_ITEM];
        CResRef cScript(*psScript);
        pRes->WriteFieldCResRef(pStruct, cScript, "Mod_OnActvtItem");
    }
    {
        CExoString* psScript = &m_sScripts[SCRIPT_ACQUIRE_ITEM];
        CResRef cScript(*psScript);
        pRes->WriteFieldCResRef(pStruct, cScript, "Mod_OnAcquirItem");
    }
    {
        CExoString* psScript = &m_sScripts[SCRIPT_UNACQUIRE_ITEM];
        CResRef cScript(*psScript);
        pRes->WriteFieldCResRef(pStruct, cScript, "Mod_OnUnAqreItem");
    }
    {
        CExoString* psScript = &m_sScripts[SCRIPT_MODULE_LOAD];
        CResRef cScript(*psScript);
        pRes->WriteFieldCResRef(pStruct, cScript, "Mod_OnModLoad");
    }
    {
        CExoString* psScript = &m_sScripts[SCRIPT_MODULE_START];
        CResRef cScript(*psScript);
        pRes->WriteFieldCResRef(pStruct, cScript, "Mod_OnModStart");
    }
    {
        CExoString* psScript = &m_sScripts[SCRIPT_PLAYER_DEATH];
        CResRef cScript(*psScript);
        pRes->WriteFieldCResRef(pStruct, cScript, "Mod_OnPlrDeath");
    }
    {
        CExoString* psScript = &m_sScripts[SCRIPT_PLAYER_DYING];
        CResRef cScript(*psScript);
        pRes->WriteFieldCResRef(pStruct, cScript, "Mod_OnPlrDying");
    }
    {
        CExoString* psScript = &m_sScripts[SCRIPT_SPAWN_BUTTON_DOWN];
        CResRef cScript(*psScript);
        pRes->WriteFieldCResRef(pStruct, cScript, "Mod_OnSpawnBtnDn");
    }
    {
        CExoString* psScript = &m_sScripts[SCRIPT_PLAYER_REST];
        CResRef cScript(*psScript);
        pRes->WriteFieldCResRef(pStruct, cScript, "Mod_OnPlrRest");
    }
    {
        CExoString* psScript = &m_sScripts[SCRIPT_PLAYER_LEVEL_UP];
        CResRef cScript(*psScript);
        pRes->WriteFieldCResRef(pStruct, cScript, "Mod_OnPlrLvlUp");
    }
    pRes->AddList(&cList, pStruct, "Mod_Area_list");
    CResRef cArea = g_pAppManager->GetServerExoApp()->GetAreaByGameObjectID(
        m_oidArea)->GetResRef();
    pRes->AddListElement(&cElement, &cList, GFF_STRUCT_AREA);
    pRes->WriteFieldCResRef(&cElement, cArea, "Area_Name");
    pRes->WriteFieldDWORD(&cElement, m_oidArea, "ObjectId");
    CSWReentrantServerStats* pProgressStats = g_pAppManager->m_pReentrantServerStats;
    pProgressStats->m_nProgressTotal = 1;
    pRes->AddList(&cList, pStruct, "Mod_Tokens");
    for (unsigned long i = 0; i < g_pTlkTable->m_nCustomTokenCount; i++)
    {
        if (g_pTlkTable->m_pCustomTokens[i].m_nToken >
            CTlkTable::CUSTOMTOKEN_RESERVED_END)
        {
            pRes->AddListElement(&cElement, &cList, GFF_STRUCT_TOKEN);
            unsigned long nToken = g_pTlkTable->m_pCustomTokens[i].m_nToken;
            pRes->WriteFieldDWORD(&cElement, nToken,
                                 "Mod_TokensNumber");
            CTlkTable* pTlkTable = g_pTlkTable;
            pRes->WriteFieldCExoString(&cElement, pTlkTable->m_pCustomTokens[i].m_sValue,
                                      "Mod_TokensValue");
        }
    }
    m_ScriptVarTable.SaveVarTable(pRes, pStruct);
    m_lVarTable.SaveVarTable(pRes, pStruct);
    g_pAppManager->GetServerExoApp()->GetServerAIMaster()->SaveEventQueue(pRes, pStruct);
    return 1;
}
// PC 0x00506E30
void CSWSModule::SaveLimboCreatures(CResGFF* pRes, CResStruct* pStruct)
{
    CResList cList;
    CResStruct cCreatureStruct;
    CGameObject* pObject;
    CGameObjectArray* pArray = g_pAppManager->m_pServerExoApp->GetObjectArray();

    pRes->AddList(&cList, pStruct, "Creature List");
    for (int i = 0; i < m_lLimbo.num; i++)
    {
        if (pArray->GetGameObject(m_lLimbo[i], &pObject) ==
            CGameObjectArray::SUCCESS)
        {
            CSWSCreature* pCreature = pObject->AsSWSCreature();
            EXOXBOXASSERT(pCreature);

            pRes->AddListElement(&cCreatureStruct, &cList, 4);
            pRes->WriteFieldDWORD(&cCreatureStruct, pCreature->m_nId,
                                  "ObjectId");
            pCreature->SaveCreature(pRes, &cCreatureStruct);
        }
    }
}
// PC 0x00509A30
int CSWSModule::SaveModuleIFOFinish(CResGFF* pRes, CResStruct* pStruct,
                                    CERFFile* pERFFile, CExoString& sPath,
                                    CExoArrayList<unsigned long>& lstPlayers)
{
    CExoString sFileName;
    CExoString sDirectory;
    CExoString sResRef;

    SavePlayers(pRes, pStruct, sPath, lstPlayers);
    pERFFile->WriteResource("Module", RESTYPE_IFO, pRes, 1, (void*)-1);
    delete pRes;
    delete pStruct;
    return 1;
}
// PC 0x00508A00
int CSWSModule::SavePlayers(CResGFF* pRes, CResStruct* pStruct,
                            const CExoString& sPath,
                            CExoArrayList<unsigned long>& lstPlayers)
{
    CGameObjectArray* pObjectArray =
        g_pAppManager->GetServerExoApp()->GetObjectArray();
    CExoArrayList<unsigned long> lstSavedPlayers;
    CServerExoApp* pServer = g_pAppManager->GetServerExoApp();
    CResList cPlayerList;
    CResStruct cPlayerStruct;
    CGameObject* pObject;
    int nPlayer;

    pRes->AddList(&cPlayerList, pStruct, "Mod_PlayerList");
    for (int i = 0; i < lstPlayers.num; i++)
    {
        if (pObjectArray->GetGameObject(lstPlayers[i], &pObject) ==
            CGameObjectArray::SUCCESS)
        {
            CSWSCreature* pCreature = pObject->AsSWSCreature();
            EXOXBOXASSERT(pCreature);
            CSWSPlayer* pPlayer = pServer->GetClientObjectByObjectId(
                pCreature->GetId());
            if (pPlayer == NULL)
            {
                pPlayer = pServer->GetClientObjectByObjectId(
                    pServer->ClientToServerObjectId(g_pAppManager->
                        GetClientExoApp()->GetSWParty()->GetCharacter(0)->GetId()));
            }
            if (pPlayer != NULL)
            {
                pPlayer->SaveServerCharacter();
                nPlayer = GetPlayerIndexInPlayerList(pPlayer);
                if (nPlayer != -1)
                    lstSavedPlayers.Add(nPlayer);
                pRes->AddListElement(&cPlayerStruct, &cPlayerList,
                                     GFF_STRUCT_PLAYER);
                pRes->WriteFieldCExoString(&cPlayerStruct,
                    pPlayer->GetPlayerName(), "Mod_CommntyName");
                int bIsPrimaryPlayer = pPlayer->m_bIsAllowedToSave;
                pRes->WriteFieldBYTE(&cPlayerStruct,
                    bIsPrimaryPlayer, "Mod_IsPrimaryPlr");
                pRes->WriteFieldCExoLocString(&cPlayerStruct,
                    pCreature->GetStats()->m_lsFirstName, "Mod_FirstName");
                pRes->WriteFieldCExoLocString(&cPlayerStruct,
                    pCreature->GetStats()->m_lsLastName, "Mod_LastName");
                pRes->WriteFieldDWORD(&cPlayerStruct,
                    pCreature->GetId(), "ObjectId");
                pCreature->SaveCreature(pRes, &cPlayerStruct);
            }
            else
            {
                pCreature->DumpToLog();
            }
        }
    }

    CResGFF* pModule;
    pModule = (CResGFF*)g_pExoResMan->GetResObject("Module", RESTYPE_IFO);
    CExoString sPlayerName;
    CResStruct cModuleStruct;
    if (pModule != NULL)
    {
        pModule->Demand();
        pModule->GetTopLevelStruct(&cModuleStruct);
        pModule->Release();
        g_pExoResMan->ReleaseResObject(pModule);
    }
    return 0;
}
// PC 0x00506EF0
int CSWSModule::GetPlayerIndexInPlayerList(CSWSPlayer* pPlayer)
{
    int nIndex = 0;
    CExoLinkedListNode* pNode = m_lstPlayers.GetHeadPos();

    while (pNode != NULL)
    {
        NWPlayerListItem_st* pListItem =
            (NWPlayerListItem_st*)m_lstPlayers.GetAtPos(pNode);

        if (pListItem->m_sPlayerName == pPlayer->GetPlayerName())
        {
            CSWSCreature* pCreature =
                g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(
                    pPlayer->m_oidPCObject);

            CSWSCreatureStats* pStats = pCreature->m_pStats;
            if (pListItem->m_lsFirstName == pStats->m_lsFirstName)
            {
                pStats = pCreature->m_pStats;
                if (pListItem->m_lsLastName == pStats->m_lsLastName)
                    return nIndex;
            }
        }
        m_lstPlayers.GetNext(pNode);
        nIndex++;
    }
    return -1;
}
// PC 0x00507010
int CSWSModule::GetPrimaryPlayerIndex()
{
    int nPrimaryPlayerIndex = -1;
    int nIndex = 0;
    CExoLinkedListNode* pNode = m_lstPlayers.GetHeadPos();

    if (pNode != NULL)
    {
        do
        {
            NWPlayerListItem_st* pPlayer =
                (NWPlayerListItem_st*)m_lstPlayers.GetAtPos(pNode);

            if (pPlayer->m_nPlayerType == 1)
            {
                nPrimaryPlayerIndex = nIndex;
                break;
            }
            m_lstPlayers.GetNext(pNode);
            nIndex++;
        } while (pNode != NULL);
    }
    return nPrimaryPlayerIndex;
}
// PC 0x00507070
inline NWPlayerCharacterList_st::~NWPlayerCharacterList_st()
{
}
// PC 0x00508CB0
int CSWSModule::AddObjectToLookupTable(const CExoString& sTag,
                                       unsigned long oidObject)
{
    CExoString sTagCopy = sTag;
    int nIndex = FindTagPositionInTable((char*)sTagCopy.CStr());
    CSWSTagNode cNode;

    cNode.m_oidObject = oidObject;
    strcpy(cNode.m_sTag, sTagCopy.CStr());
    for (; nIndex < m_lstTagTable.num; nIndex++)
    {
        if (!(sTag == m_lstTagTable[nIndex].m_sTag))
        {
            break;
        }
    }
    m_lstTagTable.Insert(cNode, nIndex);
    return 1;
}
// PC 0x005070E0
int CSWSModule::FindTagPositionInTable(char* sTag)
{
    int nLow = 0;
    int nHigh = m_lstTagTable.num - 1;

    while (nLow < nHigh || nLow == m_lstTagTable.num - 1)
    {
        int nMiddle = (nHigh + nLow) >> 1;

        if (strcmp(m_lstTagTable[nMiddle].m_sTag, sTag) >= 0)
        {
            if (nMiddle == nHigh)
                return nMiddle;
            nHigh = nMiddle;
        }
        else
        {
            nLow = nMiddle + 1;
        }
    }
    return nLow;
}
// PC 0x00507170
int CSWSModule::RemoveObjectFromLookupTable(const CExoString& sTag,
                                            unsigned long oidObject)
{
    CExoString sTagCopy = sTag;
    int nIndex = FindTagPositionInTable((char*)sTagCopy.CStr());

    while (nIndex < m_lstTagTable.num &&
           m_lstTagTable[nIndex].m_oidObject != oidObject)
    {
        nIndex++;
    }
    if (nIndex < m_lstTagTable.num &&
        sTagCopy == m_lstTagTable[nIndex].m_sTag)
    {
        m_lstTagTable.DelIndex(nIndex);
        return 1;
    }
    return 0;
}
// PC 0x00507260
unsigned long CSWSModule::FindObjectByTagOrdinal(const CExoString& sTag,
                                                 unsigned long nNth)
{
    CExoString sTagCopy = sTag;
    int nIndex = nNth + FindTagPositionInTable((char*)sTagCopy.CStr());

    if (nIndex < m_lstTagTable.num &&
        sTagCopy == m_lstTagTable[nIndex].m_sTag)
    {
        return m_lstTagTable[nIndex].m_oidObject;
    }
    return OBJECT_INVALID;
}

unsigned long CSWSModule::FindObjectByTagOrdinal(const CExoString& sTag,
                                                 int nOrdinal)
{
    CExoString sTagCopy = sTag;
    int nIndex = nOrdinal + FindTagPositionInTable((char*)sTagCopy.CStr());

    if (nIndex < m_lstTagTable.num &&
        sTagCopy == m_lstTagTable[nIndex].m_sTag)
    {
        return m_lstTagTable[nIndex].m_oidObject;
    }
    return OBJECT_INVALID;
}
// PC 0x00507330
unsigned long CSWSModule::FindObjectByTagTypeOrdinal(const CExoString& sTag,
                                                     int nObjectType,
                                                     unsigned long nNth)
{
    CGameObject* pObject;
    CExoString sTagLower = sTag.LowerCase();
    CGameObjectArray* pArray = NULL;
    int nIndex = FindTagPositionInTable((char*)sTagLower.CStr());
    unsigned long nSkip = nNth;

    for (; nIndex < m_lstTagTable.num &&
           sTagLower == m_lstTagTable[nIndex].m_sTag;
         nIndex++)
    {
        if (pArray == NULL)
        {
            pArray = g_pAppManager->m_pServerExoApp->GetObjectArray();
        }
        if (pArray->GetGameObject(m_lstTagTable[nIndex].m_oidObject,
                                  &pObject) == CGameObjectArray::SUCCESS &&
            nObjectType == pObject->m_nObjectType)
        {
            if (nSkip == 0)
            {
                return m_lstTagTable[nIndex].m_oidObject;
            }
            nSkip--;
        }
    }
    return OBJECT_INVALID;
}
// PC 0x00507450
int CSWSModule::GetWorldJournalIndexUnique()
{
    int nEntryCount = m_lstWorldJournal.num;
    int nEntry = 0;
    int nIndex = 1;

    if (nEntryCount > 0)
    {
        do
        {
            if (m_lstWorldJournal[nEntry].m_bStringRef == 0 &&
                m_lstWorldJournal[nEntry].m_nIndex == nIndex)
            {
                ++nIndex;
                nEntry = -1;
            }
            ++nEntry;
        } while (nEntry < nEntryCount);
    }
    return nIndex;
}
// PC 0x00507490
void CSWSModule::UpdateTime(unsigned long nDay, unsigned long nTime,
                           unsigned long nDeltaTime)
{
    unsigned long nYear;
    unsigned long nMonth;
    unsigned long nDayOfMonth;
    unsigned long nHour;
    unsigned long nMinute;
    unsigned long nSecond;
    unsigned long nMillisecond;
    unsigned char nPeriod;
    unsigned char nUpdateMask;
    CServerExoApp* pExoApp;

    pExoApp = g_pAppManager->m_pServerExoApp;
    pExoApp->GetWorldTimer()->ConvertFromCalendarDay(
        nDay, &nYear, &nMonth, &nDayOfMonth);
    pExoApp = g_pAppManager->m_pServerExoApp;
    pExoApp->GetWorldTimer()->ConvertFromTimeOfDay(
        nTime, &nHour, &nMinute, &nSecond, &nMillisecond);

    if (m_nDawnHour == m_nDuskHour)
        nPeriod = TIME_DAY;
    else if (nHour == m_nDawnHour)
        nPeriod = TIME_DAWN;
    else if (nHour == m_nDuskHour)
        nPeriod = TIME_DUSK;
    else if (m_nDawnHour < m_nDuskHour)
    {
        if (nHour < m_nDawnHour || nHour > m_nDuskHour)
            nPeriod = TIME_NIGHT;
        else
            nPeriod = TIME_DAY;
    }
    else
    {
        if (nHour < m_nDuskHour || nHour > m_nDawnHour)
            nPeriod = TIME_DAY;
        else
            nPeriod = TIME_NIGHT;
    }

    if (m_nTimeDawnDusk == TIME_DAWN || m_nTimeDawnDusk == TIME_DUSK)
        m_nTime2 += nDeltaTime;
    else
        m_nTime2 = 0;

    nUpdateMask = 0;
    if (m_nTimeDawnDusk != nPeriod)
    {
        nUpdateMask |= TIME_UPDATE_PERIOD;
        m_nTimeDawnDusk = nPeriod;
    }
    if (m_nTime != nHour)
    {
        nUpdateMask |= TIME_UPDATE_HOUR;
        m_nTime = nHour;
    }
    if (m_nDay != nDayOfMonth)
    {
        nUpdateMask |= TIME_UPDATE_DAY;
        m_nDay = nDayOfMonth;
    }
    if (m_nMonth != nMonth)
    {
        nUpdateMask |= TIME_UPDATE_MONTH;
        m_nMonth = nMonth;
    }
    if (m_nYear != nYear)
    {
        nUpdateMask |= TIME_UPDATE_YEAR;
        m_nYear = nYear;
    }

    if (nUpdateMask != 0)
    {
        CExoLinkedList<CSWSClient>* pPlayerList =
            g_pAppManager->m_pServerExoApp->GetPlayerList();
        CExoLinkedListNode* pNode = pPlayerList->GetHeadPos();
        while (pNode != NULL)
        {
            CSWSPlayer* pPlayer = (CSWSPlayer*)pPlayerList->GetAtPos(pNode);
            if (pPlayer != NULL)
            {
                pExoApp = g_pAppManager->m_pServerExoApp;
                pExoApp->GetSWSMessage()->
                    SendServerToPlayerModuleUpdate_Time(pPlayer, nUpdateMask,
                        nYear, nMonth, nDayOfMonth, nHour, m_nTimeDawnDusk,
                        m_nTime2);
            }
            pPlayerList->GetNext(pNode);
        }
    }
}
// PC 0x00507670
void CSWSModule::RemoveObjectFromLimbo(unsigned long oidObject)
{
    for (int nIndex = m_lLimbo.num - 1; nIndex >= 0; nIndex--)
    {
        if (oidObject == m_lLimbo[nIndex])
        {
            m_lLimbo.DelIndex(nIndex);
            return;
        }
    }
}
// PC 0x005076D0
int CSWSModule::IsObjectInLimbo(unsigned long oidObject)
{
    for (int nIndex = m_lLimbo.num - 1; nIndex >= 0; nIndex--)
    {
        if (oidObject == m_lLimbo[nIndex])
        {
            return 1;
        }
    }
    return 0;
}
// PC 0x00508DA0
void CSWSModule::AddWorldJournalEntry(int nIndex, const CExoString& sText,
                                     const CExoString& sTag,
                                     unsigned long nCalendarDay,
                                     unsigned long nTimeOfDay)
{
    CSWSMessage* pMessage = g_pAppManager->m_pServerExoApp->GetSWSMessage();

    CExoLinkedListNode* pNext;
    CWorldJournalEntry entry;
    entry.m_nIndex = nIndex;
    entry.m_sText = sText;
    entry.m_sTag = sTag;
    entry.m_nCalendarDay = nCalendarDay;
    entry.m_nTimeOfDay = nTimeOfDay;
    entry.m_bStringRef = 0;
    m_lstWorldJournal.Add(entry);

    if (pMessage != NULL)
    {
        CExoLinkedList<CSWSClient>* pPlayerList =
            g_pAppManager->m_pServerExoApp->GetPlayerList();

        if (pPlayerList != NULL)
        {
            CExoLinkedListNode* pNode = pPlayerList->GetHeadPos();
            pNext = pNode;

            if (pNode != NULL)
            {
                do
                {
                    CSWSPlayer* pPlayer =
                        (CSWSPlayer*)pPlayerList->GetAtPos(pNext);

                    if (pPlayer != NULL && pPlayer->GetGameObject() != NULL)
                    {
                        pMessage->SendServerToPlayerJournalAddWorld(
                            pPlayer, nIndex, sText, sTag, nCalendarDay,
                            nTimeOfDay);
                    }
                    pPlayerList->GetNext(pNext);
                } while (pNext != NULL);
            }
        }
    }
}
// PC 0x00508F00
void CSWSModule::AddWorldJournalEntryStrref(unsigned long nIndex,
                                            unsigned long nStrRef,
                                            unsigned long nCalendarDay,
                                            unsigned long nTimeOfDay)
{
    CSWSMessage* pMessage = g_pAppManager->m_pServerExoApp->GetSWSMessage();

    CExoLinkedListNode* pNext;
    CWorldJournalEntry entry;
    entry.m_nIndex = nIndex;
    entry.m_nStringRef = nStrRef;
    entry.m_nCalendarDay = nCalendarDay;
    entry.m_nTimeOfDay = nTimeOfDay;
    entry.m_bStringRef = 1;
    m_lstWorldJournal.Add(entry);

    if (pMessage != NULL)
    {
        CExoLinkedList<CSWSClient>* pPlayerList =
            g_pAppManager->m_pServerExoApp->GetPlayerList();

        if (pPlayerList != NULL)
        {
            CExoLinkedListNode* pNode = pPlayerList->GetHeadPos();
            pNext = pNode;

            if (pNode != NULL)
            {
                do
                {
                    CSWSPlayer* pPlayer =
                        (CSWSPlayer*)pPlayerList->GetAtPos(pNext);

                    if (pPlayer != NULL && pPlayer->GetGameObject() != NULL)
                    {
                        pMessage->SendServerToPlayerJournalAddWorldStrref(
                            pPlayer, nIndex, nStrRef, nCalendarDay,
                            nTimeOfDay);
                    }
                    pPlayerList->GetNext(pNext);
                } while (pNext != NULL);
            }
        }
    }
}
// PC 0x00509040
void CSWSModule::DeleteWorldJournalEntry(int nIndex)
{
    CSWSMessage* pMessage = g_pAppManager->m_pServerExoApp->GetSWSMessage();

    for (int nEntry = m_lstWorldJournal.num; nEntry >= 0; nEntry--)
    {
        if (m_lstWorldJournal[nEntry].m_bStringRef == 0 &&
            m_lstWorldJournal[nEntry].m_nIndex == nIndex)
        {
            m_lstWorldJournal.DelIndex(nEntry);
            break;
        }
    }
    if (pMessage != NULL)
    {
        CExoLinkedList<CSWSClient>* pPlayerList =
            g_pAppManager->m_pServerExoApp->GetPlayerList();

        if (pPlayerList != NULL)
        {
            CExoLinkedListNode* pNode = pPlayerList->GetHeadPos();
            CExoLinkedListNode* pNext = pNode;

            if (pNode != NULL)
            {
                do
                {
                    CSWSPlayer* pPlayer =
                        (CSWSPlayer*)pPlayerList->GetAtPos(pNext);

                    if (pPlayer != NULL && pPlayer->GetGameObject() != NULL)
                    {
                        pMessage->SendServerToPlayerJournalDeleteWorld(pPlayer,
                                                                       nIndex);
                    }
                    pPlayerList->GetNext(pNext);
                } while (pNext != NULL);
            }
        }
    }
}
// PC 0x00509100
void CSWSModule::DeleteWorldJournalEntryStrref(unsigned long nStrRef)
{
    CSWSMessage* pMessage = g_pAppManager->m_pServerExoApp->GetSWSMessage();

    for (int nEntry = m_lstWorldJournal.num; nEntry >= 0; nEntry--)
    {
        if (m_lstWorldJournal[nEntry].m_bStringRef != 0 &&
            m_lstWorldJournal[nEntry].m_nIndex == nStrRef)
        {
            m_lstWorldJournal.DelIndex(nEntry);
            break;
        }
    }

    CExoLinkedList<CSWSClient>* pPlayerList =
        g_pAppManager->m_pServerExoApp->GetPlayerList();

    if (pPlayerList != NULL)
    {
        CExoLinkedListNode* pNode = pPlayerList->GetHeadPos();
        CExoLinkedListNode* pNext = pNode;

        if (pNode != NULL)
        {
            do
            {
                CSWSPlayer* pPlayer = (CSWSPlayer*)pPlayerList->GetAtPos(pNext);

                if (pPlayer != NULL && pPlayer->GetGameObject() != NULL &&
                    pMessage != NULL)
                {
                    pMessage->SendServerToPlayerJournalDeleteWorldStrref(
                        pPlayer, nStrRef);
                }
                pPlayerList->GetNext(pNext);
            } while (pNext != NULL);
        }
    }
}
// PC 0x005091C0
void CSWSModule::DeleteWorldJournalAllEntries()
{
    CSWSMessage* pMessage = g_pAppManager->m_pServerExoApp->GetSWSMessage();

    m_lstWorldJournal.SetSize(0);

    CExoLinkedList<CSWSClient>* pPlayerList =
        g_pAppManager->m_pServerExoApp->GetPlayerList();

    if (pPlayerList != NULL)
    {
        CExoLinkedListNode* pNode = pPlayerList->GetHeadPos();
        CExoLinkedListNode* pNext = pNode;

        if (pNode != NULL)
        {
            do
            {
                CSWSPlayer* pPlayer = (CSWSPlayer*)pPlayerList->GetAtPos(pNext);

                if (pPlayer != NULL && pPlayer->GetGameObject() != NULL &&
                    pMessage != NULL)
                {
                    pMessage->SendServerToPlayerJournalDeleteWorldAll(pPlayer);
                }
                pPlayerList->GetNext(pNext);
            } while (pNext != NULL);
        }
    }
}
// PC 0x00509B10
int CSWSModule::LoadLimboCreatures(CResGFF* pRes, CResStruct* pStruct,
                                   int bIsSaveGame)
{
    CResList cList;
    CResStruct cCreatureStruct;
    int bSuccess;

    if (pRes->GetList(&cList, pStruct, "Creature List"))
    {
        for (unsigned long i = 0; i < pRes->GetListCount(&cList); i++)
        {
            if (pRes->GetListElement(&cCreatureStruct, &cList, i) &&
                pRes->GetElementType(&cCreatureStruct) ==
                    GFF_STRUCT_LIMBO_CREATURE)
            {
                unsigned long oidCreature = pRes->ReadFieldDWORD(
                    &cCreatureStruct, "ObjectId", bSuccess, OBJECT_INVALID);
                CSWSCreature* pCreature = new CSWSCreature(oidCreature, 0);

                if (pCreature != NULL)
                {
                    if (!pCreature->LoadCreature(pRes, &cCreatureStruct,
                                                 bIsSaveGame))
                    {
                        delete pCreature;
                    }
                    else
                    {
                        if (bIsSaveGame)
                        {
                            pCreature->m_bCreationScriptFired = 1;
                        }

                        pRes->ReadFieldFLOAT(&cCreatureStruct, "XPosition",
                                             bSuccess, 0.0f);
                        pRes->ReadFieldFLOAT(&cCreatureStruct, "YPosition",
                                             bSuccess, 0.0f);
                        pRes->ReadFieldFLOAT(&cCreatureStruct, "ZPosition",
                                             bSuccess, 0.0f);
                        Vector vOrientation(
                            pRes->ReadFieldFLOAT(&cCreatureStruct,
                                                 "XOrientation", bSuccess,
                                                 0.0f),
                            pRes->ReadFieldFLOAT(&cCreatureStruct,
                                                 "YOrientation", bSuccess,
                                                 0.0f),
                            pRes->ReadFieldFLOAT(&cCreatureStruct,
                                                 "ZOrientation", bSuccess,
                                                 0.0f));
                        pCreature->CSWSObject::SetOrientation(vOrientation);
                        AddObjectToLimbo(pCreature->m_nId);
                    }
                }
            }
        }
    }
    return 1;
}
// PC 0x00509280
void CSWSModule::AddObjectToLimbo(unsigned long oidObject)
{
    for (int i = 0; i < m_lLimbo.num; i++)
    {
        if (oidObject == m_lLimbo[i])
        {
            return;
        }
    }
    m_lLimbo.Add(oidObject);
}
// PC 0x00509D30
void CSWSModule::PackPlayerCharacterListIntoMessage(CSWSPlayer* pPlayer,
    CExoArrayList<NWPlayerCharacterList_st*>& lstCharacters)
{
    NwsmoduleTrace3();
    int nIndex = 0;
    CResRef cPortrait;
    CExoString sPortrait;
    CExoLinkedListNode* pNode = m_lstPlayers.GetHeadPos();

    while (pNode != NULL)
    {
        NWPlayerListItem_st* pEntry = m_lstPlayers.GetAtPos(pNode);
        NWPlayerCharacterList_st* pCharacter = new NWPlayerCharacterList_st;

        pCharacter->m_lsFirstName = pEntry->m_lsFirstName;
        pCharacter->m_lsLastName = pEntry->m_lsLastName;
        pCharacter->m_bCharacterExists =
            NWPlayerCharacterList_st::CHARACTER_LIST_MODULE_PLAYER;
        sPortrait.Format("%i", (unsigned short)nIndex);
        cPortrait = sPortrait;
        pCharacter->m_cCharacter = cPortrait;

        if (!lstCharacters.DerefContains(pCharacter))
            lstCharacters.Add(pCharacter);
        else
            delete pCharacter;

        m_lstPlayers.GetNext(pNode);
        nIndex++;
    }
}
// PC 0x00509EF0
int CSWSModule::LoadModuleStart(const CExoString& sModule, int bLoadModuleTime)
{
    CExoString sModuleName;
    CExoString sTokenValue;
    int bSuccess;
    CResStruct cModuleStruct;
    CResStruct cElement;
    CResList cList;
    unsigned short nMinute, nSecond, nMillisecond;
    unsigned long nPauseTime, nPauseDay;

#ifndef _XBOX
    g_pExoResMan->FreeChunk();
#endif
    CGameObjectArray* pObjectArray =
        g_pAppManager->GetServerExoApp()->GetObjectArray();
    sModuleName = CExoString("CURRENTGAME:") + sModule;
    if (m_sModuleName != sModuleName)
    {
        if (m_sModuleName.GetLength() > 0)
            RemoveModuleResources(m_sModuleName);
        m_sModuleName = sModuleName;
        AddModuleResources(sModuleName);
    }
    if (m_pRes->Demand() == NULL)
        return LOAD_MODULE_RESOURCE_FAILED;

    m_pRes->GetTopLevelStruct(&cModuleStruct);
    m_bIsSaveGame = m_pRes->ReadFieldBYTE(&cModuleStruct, "Mod_IsSaveGame",
        bSuccess, (unsigned char)m_bIsSaveGame);
    m_bIsNWMFile = m_pRes->ReadFieldBYTE(&cModuleStruct, "Mod_IsNWMFile",
        bSuccess, (unsigned char)m_bIsNWMFile);
    if (m_bIsNWMFile == 1)
        m_sSaveName = m_pRes->ReadFieldCExoString(&cModuleStruct,
            "Mod_NWMResName", bSuccess, m_sSaveName);
    m_nResourceCount = g_pExoResMan->GetTableCount(m_pRes, m_bIsNWMFile == 0);
    m_pRes->ReadFieldVOID(&cModuleStruct, m_pModuleInfo->m_aModuleID,
        sizeof(m_pModuleInfo->m_aModuleID), "Mod_ID", bSuccess, NULL);
    m_pModuleInfo->m_nCreatorID = m_pRes->ReadFieldINT(&cModuleStruct,
        "Mod_Creator_ID", bSuccess, 0);
    m_pModuleInfo->m_nVersion = m_pRes->ReadFieldDWORD(&cModuleStruct,
        "Mod_Version", bSuccess, 0);
    g_pAppManager->GetServerExoApp()->GetServerInfo()->m_sModuleList = sModule;
    m_lsName = m_pRes->ReadFieldCExoLocString(&cModuleStruct,
        "Mod_Name", bSuccess, CExoLocString());
    m_sDescription = m_pRes->ReadFieldCExoLocString(&cModuleStruct,
        "Mod_Description", bSuccess, CExoLocString());
    m_cStartMovie = m_pRes->ReadFieldCResRef(&cModuleStruct,
        "Mod_StartMovie", bSuccess, "");
    SetTag(m_pRes->ReadFieldCExoString(&cModuleStruct,
        "Mod_Tag", bSuccess, ""));
    m_pEntryInfo->m_cEntryArea = m_pRes->ReadFieldCResRef(&cModuleStruct,
        "Mod_Entry_Area", bSuccess, "");
    m_pEntryInfo->m_fEntryX = m_pRes->ReadFieldFLOAT(&cModuleStruct,
        "Mod_Entry_X", bSuccess, 0.0f);
    m_pEntryInfo->m_fEntryY = m_pRes->ReadFieldFLOAT(&cModuleStruct,
        "Mod_Entry_Y", bSuccess, 0.0f);
    m_pEntryInfo->m_fEntryZ = m_pRes->ReadFieldFLOAT(&cModuleStruct,
        "Mod_Entry_Z", bSuccess, 0.0f);
    m_pEntryInfo->m_fEntryDirX = m_pRes->ReadFieldFLOAT(&cModuleStruct,
        "Mod_Entry_Dir_X", bSuccess, 0.0f);
    m_pEntryInfo->m_fEntryDirY = m_pRes->ReadFieldFLOAT(&cModuleStruct,
        "Mod_Entry_Dir_Y", bSuccess, 0.0f);
    if (!bSuccess)
    {
        m_pEntryInfo->m_fEntryDirX = 1.0f;
        m_pEntryInfo->m_fEntryDirY = 0.0f;
    }
    m_nMinutesPerHour = m_pRes->ReadFieldBYTE(&cModuleStruct,
        "Mod_MinPerHour", bSuccess, 0);
    m_nDawnHour = m_pRes->ReadFieldBYTE(&cModuleStruct,
        "Mod_DawnHour", bSuccess, 0);
    m_nDuskHour = m_pRes->ReadFieldBYTE(&cModuleStruct,
        "Mod_DuskHour", bSuccess, 0);
    if (bLoadModuleTime)
    {
        m_nYear = m_pRes->ReadFieldDWORD(&cModuleStruct,
            "Mod_StartYear", bSuccess, CWorldTimer::DEFAULT_YEAR);
        m_nStartMonth = m_pRes->ReadFieldBYTE(&cModuleStruct,
            "Mod_StartMonth", bSuccess, CWorldTimer::DEFAULT_MONTH);
        m_nStartDay = m_pRes->ReadFieldBYTE(&cModuleStruct,
            "Mod_StartDay", bSuccess, CWorldTimer::DEFAULT_DAY);
        m_nStartHour = m_pRes->ReadFieldBYTE(&cModuleStruct,
            "Mod_StartHour", bSuccess, CWorldTimer::DEFAULT_HOUR);
        m_nTime2 = m_pRes->ReadFieldDWORD(&cModuleStruct,
            "Mod_Transition", bSuccess, 0);
        nMinute = m_pRes->ReadFieldWORD(&cModuleStruct,
            "Mod_StartMinute", bSuccess, 0);
        nSecond = m_pRes->ReadFieldWORD(&cModuleStruct,
            "Mod_StartSecond", bSuccess, 0);
        nMillisecond = m_pRes->ReadFieldWORD(&cModuleStruct,
            "Mod_StartMiliSec", bSuccess, 0);
        nPauseTime = m_pRes->ReadFieldDWORD(&cModuleStruct,
            "Mod_PauseTime", bSuccess, 0);
        nPauseDay = m_pRes->ReadFieldDWORD(&cModuleStruct,
            "Mod_PauseDay", bSuccess, 0);
    }
    else
    {
        g_pAppManager->GetServerExoApp()->GetMoveToWorldTime(m_nYear,
            m_nStartMonth, m_nStartDay, m_nStartHour,
            nMinute, nSecond, nMillisecond);
        nPauseTime = g_pAppManager->GetServerExoApp()->GetMoveToPausedTimeOfDay();
        nPauseDay = g_pAppManager->GetServerExoApp()->GetMoveToPausedCalendarDay();
    }
    m_nMonth = m_nStartMonth;
    m_nDay = m_nStartDay;
    m_nTime = m_nStartHour;
    g_pAppManager->GetServerExoApp()->GetWorldTimer()->SetMinutesPerHour(
        m_nMinutesPerHour);
    g_pAppManager->GetServerExoApp()->GetWorldTimer()->SetWorldTime(
        m_nYear, m_nStartMonth, m_nStartDay, m_nStartHour,
        nMinute, nSecond, nMillisecond, 1);
    g_pAppManager->GetServerExoApp()->GetWorldTimer()->m_nPausedCalendarDay = nPauseDay;
    g_pAppManager->GetServerExoApp()->GetWorldTimer()->m_nPausedTimeOfDay = nPauseTime;
    g_pAppManager->GetServerExoApp()->GetTimestopTimer()->ResetTimer(
        g_pAppManager->GetServerExoApp()->GetWorldTimer());
    g_pAppManager->GetServerExoApp()->GetPauseTimer()->ResetTimer(
        g_pAppManager->GetServerExoApp()->GetWorldTimer());
    g_pAppManager->GetServerExoApp()->GetWorldTimer()->GetWorldTime(
        &m_nLastUpdateDay, &m_nLastUpdateTime);
    if (m_nDawnHour == m_nDuskHour)
        m_nTimeDawnDusk = TIME_DAY;
    else if (m_nTime == m_nDawnHour)
        m_nTimeDawnDusk = TIME_DAWN;
    else if (m_nTime == m_nDuskHour)
        m_nTimeDawnDusk = TIME_DUSK;
    else if (m_nDawnHour < m_nDuskHour)
    {
        if (m_nTime < m_nDawnHour || m_nTime > m_nDuskHour)
            m_nTimeDawnDusk = TIME_NIGHT;
        else
            m_nTimeDawnDusk = TIME_DAY;
    }
    else
    {
        if (m_nTime >= m_nDuskHour && m_nTime <= m_nDawnHour)
            m_nTimeDawnDusk = TIME_NIGHT;
        else
            m_nTimeDawnDusk = TIME_DAY;
    }
    m_nXPScale = m_pRes->ReadFieldBYTE(&cModuleStruct,
        "Mod_XPScale", bSuccess, 10);
    m_pRes->GetList(&cList, &cModuleStruct, "Mod_Expan_List");
    unsigned long nListCount = m_pRes->GetListCount(&cList);
    for (unsigned long i = 0; i < nListCount; ++i)
    {
        m_pRes->GetListElement(&cElement, &cList, i);
        NWModuleExpansion_st* pExpansion = new NWModuleExpansion_st;
        if (pExpansion == NULL)
        {
            m_pRes->Release();
            return LOAD_MODULE_OUT_OF_MEMORY;
        }
        pExpansion->m_lsExpansionName = m_pRes->ReadFieldCExoLocString(
            &cElement, "Expansion_Name", bSuccess, CExoLocString());
        pExpansion->m_nExpansionID = m_pRes->ReadFieldINT(
            &cElement, "Expansion_ID", bSuccess, 0);
        m_lstModuleExpansion.AddTail(pExpansion);
    }
    m_pRes->GetList(&cList, &cModuleStruct, "Mod_CutSceneList");
    nListCount = m_pRes->GetListCount(&cList);
    for (unsigned long j = 0; j < nListCount; ++j)
    {
        m_pRes->GetListElement(&cElement, &cList, j);
        NWModuleCutScene_st* pCutScene = new NWModuleCutScene_st;
        if (pCutScene == NULL)
        {
            m_pRes->Release();
            return LOAD_MODULE_OUT_OF_MEMORY;
        }
        pCutScene->m_cCutSceneName = m_pRes->ReadFieldCResRef(
            &cElement, "CutScene_Name", bSuccess, "");
        pCutScene->m_nCutSceneID = m_pRes->ReadFieldDWORD(
            &cElement, "CutScene_ID", bSuccess, 0);
        m_lstModuleCutScene.AddTail(pCutScene);
    }
    CExoString sScript;
    m_pRes->ReadFieldCResRef(&cModuleStruct, "Mod_OnHeartbeat", bSuccess,
        "").CopyToString(sScript);
    m_sScripts[SCRIPT_HEARTBEAT] = sScript;
    m_pRes->ReadFieldCResRef(&cModuleStruct, "Mod_OnUsrDefined", bSuccess,
        "").CopyToString(sScript);
    m_sScripts[SCRIPT_USER_DEFINED] = sScript;
    m_pRes->ReadFieldCResRef(&cModuleStruct, "Mod_OnClientEntr", bSuccess,
        "").CopyToString(sScript);
    m_sScripts[SCRIPT_CLIENT_ENTER] = sScript;
    m_pRes->ReadFieldCResRef(&cModuleStruct, "Mod_OnClientLeav", bSuccess,
        "").CopyToString(sScript);
    m_sScripts[SCRIPT_CLIENT_LEAVE] = sScript;
    m_pRes->ReadFieldCResRef(&cModuleStruct, "Mod_OnActvtItem", bSuccess,
        "").CopyToString(sScript);
    m_sScripts[SCRIPT_ACTIVATE_ITEM] = sScript;
    m_pRes->ReadFieldCResRef(&cModuleStruct, "Mod_OnAcquirItem", bSuccess,
        "").CopyToString(sScript);
    m_sScripts[SCRIPT_ACQUIRE_ITEM] = sScript;
    m_pRes->ReadFieldCResRef(&cModuleStruct, "Mod_OnUnAqreItem", bSuccess,
        "").CopyToString(sScript);
    m_sScripts[SCRIPT_UNACQUIRE_ITEM] = sScript;
    m_pRes->ReadFieldCResRef(&cModuleStruct, "Mod_OnModLoad", bSuccess,
        "").CopyToString(sScript);
    m_sScripts[SCRIPT_MODULE_LOAD] = sScript;
    m_pRes->ReadFieldCResRef(&cModuleStruct, "Mod_OnModStart", bSuccess,
        "").CopyToString(sScript);
    m_sScripts[SCRIPT_MODULE_START] = sScript;
    m_pRes->ReadFieldCResRef(&cModuleStruct, "Mod_OnPlrDeath", bSuccess,
        "").CopyToString(sScript);
    m_sScripts[SCRIPT_PLAYER_DEATH] = sScript;
    m_pRes->ReadFieldCResRef(&cModuleStruct, "Mod_OnPlrDying", bSuccess,
        "").CopyToString(sScript);
    m_sScripts[SCRIPT_PLAYER_DYING] = sScript;
    m_pRes->ReadFieldCResRef(&cModuleStruct, "Mod_OnSpawnBtnDn", bSuccess,
        "").CopyToString(sScript);
    m_sScripts[SCRIPT_SPAWN_BUTTON_DOWN] = sScript;
    m_pRes->ReadFieldCResRef(&cModuleStruct, "Mod_OnPlrRest", bSuccess,
        "").CopyToString(sScript);
    m_sScripts[SCRIPT_PLAYER_REST] = sScript;
    m_pRes->ReadFieldCResRef(&cModuleStruct, "Mod_OnPlrLvlUp", bSuccess,
        "").CopyToString(sScript);
    m_sScripts[SCRIPT_PLAYER_LEVEL_UP] = sScript;
    m_pRes->ReadFieldCResRef(&cModuleStruct, "Mod_OnEquipItem", bSuccess,
        "").CopyToString(sScript);
    m_sScripts[SCRIPT_EQUIP_ITEM] = sScript;
    m_pRes->GetList(&cList, &cModuleStruct, "Mod_Area_list");
    nListCount = m_pRes->GetListCount(&cList);
    EXOXBOXASSERT(nListCount == 1);
    m_pRes->GetListElement(&cElement, &cList, 0);
    m_cAreaResRef = m_pRes->ReadFieldCResRef(&cElement, "Area_Name",
        bSuccess, "");
    if (m_bIsSaveGame)
    {
        m_oidArea = m_pRes->ReadFieldDWORD(&cElement, "ObjectId",
            bSuccess, OBJECT_INVALID);
        if (m_bIsSaveGame)
        {
            CGameEffect::m_nNextID = m_pRes->ReadFieldDWORD64(&cModuleStruct,
                "Mod_Effect_NxtId", bSuccess, 0);
            pObjectArray->m_nPrevObjectId[0] = m_pRes->ReadFieldDWORD(
                &cModuleStruct, "Mod_NextCharId0", bSuccess, 0);
            pObjectArray->m_nPrevObjectId[1] = m_pRes->ReadFieldDWORD(
                &cModuleStruct, "Mod_NextCharId1", bSuccess, 0);
            pObjectArray->m_nNextObjectId[0] = m_pRes->ReadFieldDWORD(
                &cModuleStruct, "Mod_NextObjId0", bSuccess, 0);
            pObjectArray->m_nNextObjectId[1] = m_pRes->ReadFieldDWORD(
                &cModuleStruct, "Mod_NextObjId1", bSuccess, 0);
            m_pRes->GetList(&cList, &cModuleStruct, "Mod_PlayerList");
            for (unsigned long k = 0; k < m_pRes->GetListCount(&cList); ++k)
            {
                NWPlayerListItem_st* pPlayer = new NWPlayerListItem_st;
                if (pPlayer == NULL)
                {
                    m_pRes->Release();
                    return LOAD_MODULE_OUT_OF_MEMORY;
                }
                m_pRes->GetListElement(&cElement, &cList, k);
                pPlayer->m_sPlayerName = m_pRes->ReadFieldCExoString(
                    &cElement, "Mod_CommntyName", bSuccess, "");
                pPlayer->m_lsFirstName = m_pRes->ReadFieldCExoLocString(
                    &cElement, "Mod_FirstName", bSuccess, CExoLocString());
                pPlayer->m_lsLastName = m_pRes->ReadFieldCExoLocString(
                    &cElement, "Mod_LastName", bSuccess, CExoLocString());
                pPlayer->m_nPlayerType = m_pRes->ReadFieldBYTE(
                    &cElement, "Mod_IsPrimaryPlr", bSuccess, 0);
                m_lstPlayers.AddTail(pPlayer);
            }
            m_pRes->GetList(&cList, &cModuleStruct, "Mod_Tokens");
            for (unsigned long n = 0; n < m_pRes->GetListCount(&cList); ++n)
            {
                m_pRes->GetListElement(&cElement, &cList, n);
                unsigned long nToken = m_pRes->ReadFieldDWORD(
                    &cElement, "Mod_TokensNumber", bSuccess, 0);
                sTokenValue = m_pRes->ReadFieldCExoString(
                    &cElement, "Mod_TokensValue", bSuccess, "");
                g_pTlkTable->SetCustomToken(nToken, sTokenValue);
            }
            m_ScriptVarTable.LoadVarTable(m_pRes, &cModuleStruct);
            m_lVarTable.LoadVarTable(m_pRes, &cModuleStruct);
            g_pAppManager->GetServerExoApp()->GetServerAIMaster()->LoadEventQueue(
                m_pRes, &cModuleStruct);
        }
    }
    CServerExoAppInternal* pExoAppInternal =
        g_pAppManager->GetServerExoApp()->m_pcExoAppInternal;
    if (pExoAppInternal != NULL)
    {
        CFactionManager* pFactions = pExoAppInternal->m_pFactionManager;
        EXOXBOXASSERT(pFactions);
        int bLoadFactions = 1;
        g_pAppManager->GetServerExoApp()->GetPartyTable()->AddGameInProgress();
        if (g_pExoResMan->Exists("REPUTE", RESTYPE_FAC,
                                CExoResMan::TABLE_DIRECTORY))
        {
            CResGFF* pFactionsGFF = new CResGFF(RESTYPE_FAC, "FAC ", "REPUTE");
            if (pFactionsGFF != NULL)
            {
                if (pFactionsGFF->m_bLoaded)
                {
                    CResStruct cFactions;
                    CResList cFactionList;
                    CResList cReputations;
                    pFactionsGFF->GetTopLevelStruct(&cFactions);
                    if (pFactionsGFF->GetList(&cFactionList, &cFactions, "FactionList") &&
                        pFactions->LoadFactionsFromSaveGame(pFactionsGFF, &cFactionList))
                    {
                        if (pFactionsGFF->GetList(&cReputations, &cFactions, "RepList"))
                            pFactions->LoadReputationsFromSaveGame(pFactionsGFF, &cReputations);
                        bLoadFactions = 0;
                    }
                }
                delete pFactionsGFF;
            }
        }
        g_pAppManager->GetServerExoApp()->GetPartyTable()->RemoveGameInProgress();
        if (bLoadFactions)
        {
            if (pFactions->LoadFactions(0))
                pFactions->LoadReputations();
            else
                pFactions->CreateDefaultFactions();
        }
    }
    LoadLimboCreatures(m_pRes, &cModuleStruct, 1);
    unsigned long nAreasToLoad = nListCount;
    CSWReentrantServerStats* pStats = g_pAppManager->m_pReentrantServerStats;
    pStats->m_nProgress = 0;
    CSWReentrantServerStats* pLoadStats = g_pAppManager->m_pReentrantServerStats;
    pLoadStats->m_nProgressTotal = nAreasToLoad;
    g_pAppManager->GetServerExoApp()->GetSWSMessage()->
        SendServerToPlayerLoadBar_UpdateStallEvent(0, nAreasToLoad);
    m_n1D8 = 0;
    return LOAD_MODULE_SUCCESS;
}
// PC 0x0050B530
int CSWSModule::SaveModuleFinish(CExoString& sPath, CExoString& sName)
{
    if (m_pERFFile == NULL)
    {
        return 0;
    }
    if (m_bIsNWMFile == 0)
    {
        CExoString sExtension = "ARE ";
        SaveStatic(m_pERFFile, sExtension, 2012, 1);
    }
    SaveModuleIFOFinish(m_pModuleGFF, m_pERFStruct, m_pERFFile, sPath,
                        *m_plstPlayers);
    m_pERFFile->Finish();
    delete m_pERFFile;
    m_pERFFile = NULL;
    delete m_plstPlayers;
    m_plstPlayers = NULL;
    return 1;
}
// PC 0x00507700
void CSWSModule::TimeStopSanityCheck()
{
    int bTimeStop = 0;
    CExoLinkedList<unsigned long>* pExclusionList =
        g_pAppManager->m_pServerExoApp->GetActiveExclusionList();
    CExoLinkedListNode* pPos = pExclusionList->GetHeadPos();

    while (pPos != NULL && bTimeStop == 0)
    {
        CSWSCreature* pCreature =
            g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(
                (unsigned long)pExclusionList->GetAtPos(pPos));
        if (pCreature != NULL)
        {
            for (int i = 0; i < pCreature->m_lstEffects.num; i++)
            {
                if (pCreature->m_lstEffects.element[i]->m_nType ==
                    CSWSEffectListHandler::EFFECT_TIMESTOP)
                {
                    bTimeStop = 1;
                }
            }
        }
        pExclusionList->GetNext(pPos);
    }
    if (bTimeStop == 0)
    {
        g_pAppManager->m_pServerExoApp->TogglePauseState(1);
    }
}
// PC 0x00505170
int CSWSModule::GetTime(unsigned long* pnYear, unsigned long* pnMonth,
                        unsigned long* pnDay, unsigned long* pnTimeOfDay,
                        unsigned char* pnDawnDusk, unsigned long* pnTimeOfDay2)
{
    int nTimeOfDay2;

    *pnYear = m_nYear;
    *pnMonth = m_nMonth;
    *pnDay = m_nDay;
    *pnTimeOfDay = m_nTime;
    *pnDawnDusk = m_nTimeDawnDusk;
    nTimeOfDay2 = m_nTime2;
    *pnTimeOfDay2 = nTimeOfDay2;
    return nTimeOfDay2;
}

#ifdef _XBOX
#define XB7_RESTYPE_ERF 9997
#define XB7_RESTYPE_MOD 2011
#endif
void CSWSModule::AddModuleResources(const CExoString& sModuleName)  // PC 0x005051C0
{
    CExoString sResourceName;
    CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
    int nWaitCount = 0;
    int nProgress = pClientExoApp->GetLoadStep(0);
    int nProgressEnd = nProgress +
        g_pAppManager->m_pClientExoApp->GetLoadStep(1);

    g_pExoResMan->FindModuleDownloadedPkg(sModuleName);
    int nProgressIncrement = (nProgressEnd - nProgress) / 15;
    nProgress += nProgressIncrement;
    g_pAppManager->m_pClientExoApp->SetLoadBarProgress(nProgress, 1);
    UpdateScreen(0.033333335f, 0, 0);

    int nColon = sModuleName.Find(':', 0);
    if (nColon == -1)
    {
        sResourceName = sResourceName + sModuleName;
    }
    else
    {
        sResourceName = sResourceName +
            sModuleName.Right(sModuleName.GetLength() - nColon - 1);
    }

    g_pExoResMan->AddResourceDirectory("CURRENTGAME:");
    {
        CResRef cModuleResRef(sResourceName);
#ifdef _XBOX
        if (g_pExoResMan->Exists(cModuleResRef, RESTYPE_SAV, (unsigned long*)NULL))
#else
        if (g_pExoResMan->Exists(cModuleResRef, RESTYPE_SAV, (unsigned long*)NULL))
#endif
        {
            m_bIsSaveGame = 1;
        }
    }
    g_pExoResMan->RemoveResourceDirectory("CURRENTGAME:");

    CExoString sLipsName;
    sLipsName.Format("LIPS:%s_loc", sResourceName.CStr());
#ifdef _XBOX
    g_pExoResMan->AddEncapsulatedResourceFile(
        g_pExoResMan->CacheResource(sLipsName, XB7_RESTYPE_MOD), CExoResMan::PRIORITY_MODULE);
    {
        CExoString sLocalization("LIPS:localization");
        g_pExoResMan->AddEncapsulatedResourceFile(
            g_pExoResMan->CacheResource(sLocalization, XB7_RESTYPE_MOD), CExoResMan::PRIORITY_MODULE);
    }
#else
    g_pExoResMan->AddEncapsulatedResourceFile(
        sLipsName, CExoResMan::PRIORITY_MODULE);
    g_pExoResMan->AddEncapsulatedResourceFile(
        "LIPS:localization", CExoResMan::PRIORITY_MODULE);
#endif
    int bIsSaveGame = m_bIsSaveGame;
#ifdef _XBOX
    g_pExoResMan->AddResourceImageFilesAsync(sResourceName, 1, bIsSaveGame);
#else
    g_pExoResMan->AddResourceImageFilesAsync(sResourceName, 1, bIsSaveGame);
#endif

    while (g_pExoResMan->m_nAsyncState == CExoResMan::ASYNC_LOAD_REQUESTED)
    {
        Sleep(10);
        if (nProgress < nProgressEnd)
        {
            ++nWaitCount;
            if (nWaitCount > 100)
            {
                nProgress = min(nProgress + nProgressIncrement, nProgressEnd);
                pClientExoApp = g_pAppManager->m_pClientExoApp;
                pClientExoApp->SetLoadBarProgress(nProgress, 1);
                UpdateScreen(0.033333335f, 0, 0);
                nWaitCount = 0;
            }
        }
    }
    unsigned char nLoadedFlags = g_pExoResMan->m_nAsyncLoadedFlags;
    pClientExoApp = g_pAppManager->m_pClientExoApp;
    pClientExoApp->SetModuleResourceLoaded(nLoadedFlags);
    g_pAppManager->m_pClientExoApp->SetLoadBarProgress(nProgressEnd, 1);
    UpdateScreen(0.033333335f, 0, 0);
}
// PC 0x00505500
void CSWSModule::RemoveModuleResources(const CExoString& sModuleName)
{
    g_pExoResMan->RemoveEncapsulatedResourceFile(sModuleName);

    CExoString sResourceName;
    int nColon = sModuleName.Find(':', 0);
    if (nColon == -1)
    {
        sResourceName = sModuleName;
    }
    else
    {
        sResourceName =
            sModuleName.Right(sModuleName.GetLength() - nColon - 1);
    }

    int nDownloadedPackage =
        g_pExoResMan->FindModuleDownloadedPkg(sResourceName);
    g_pExoResMan->AddResourceDirectory("CURRENTGAME:");
    g_pExoResMan->RemoveResourceImageFile(
        CExoString("CURRENTGAME:") + sResourceName);
    g_pExoResMan->RemoveEncapsulatedResourceFile(
        CExoString("CURRENTGAME:") + sResourceName);
    g_pExoResMan->RemoveResourceDirectory("CURRENTGAME:");

    g_pExoResMan->AddResourceDirectory("MODULES:");
    if (g_pExoResMan->Exists(sResourceName, RESTYPE_MOD,
                            (unsigned long*)NULL))
    {
        g_pExoResMan->RemoveEncapsulatedResourceFile(
            CExoString("MODULES:") + sResourceName);
    }
    else
    {
        if (g_pExoResMan->Exists(sResourceName + CExoString("_dlg"),
                                RESTYPE_ERF, (unsigned long*)NULL))
        {
            if (nDownloadedPackage == 0)
            {
                g_pExoResMan->RemoveEncapsulatedResourceFile(
                    CExoString("MODULES:") + sResourceName + CExoString("_dlg"));
            }
            else
            {
                CExoString sDirectory;
                sDirectory.Format("LIVE%d:MODULES\\", nDownloadedPackage);
                g_pExoResMan->RemoveEncapsulatedResourceFile(
                    sDirectory + sResourceName + CExoString("_dlg"));
            }
        }
        if (g_pExoResMan->Exists(sResourceName + CExoString("_s"),
                                RESTYPE_RIM, (unsigned long*)NULL))
        {
            if (nDownloadedPackage == 0)
            {
                g_pExoResMan->RemoveResourceImageFile(
                    CExoString("MODULES:") + sResourceName + CExoString("_s"));
            }
            else
            {
                CExoString sDirectory;
                sDirectory.Format("LIVE%d:MODULES\\", nDownloadedPackage);
                g_pExoResMan->RemoveResourceImageFile(
                    sDirectory + sResourceName + CExoString("_s"));
            }
        }
    }
    g_pExoResMan->RemoveResourceDirectory("MODULES:");

#ifdef _XBOX

    if (g_pExoResMan->Exists(sResourceName + CExoString("_a"),
                            RESTYPE_RIM, (unsigned long*)NULL))
    {
        if (nDownloadedPackage == 0)
        {
            g_pExoResMan->RemoveResourceImageFile(
                CExoString("RIMSXBOX:") + sResourceName + CExoString("_a"));
        }
        else
        {
            CExoString sDirectory;
            sDirectory.Format("LIVE%d:MODULES\\", nDownloadedPackage);
            g_pExoResMan->RemoveResourceImageFile(sDirectory + sResourceName);
        }
    }
    if (g_pExoResMan->Exists(sResourceName + CExoString("_adx"),
                            RESTYPE_RIM, (unsigned long*)NULL))
    {
        if (nDownloadedPackage == 0)
        {
            g_pExoResMan->RemoveResourceImageFile(
                CExoString("RIMSXBOX:") + sResourceName + CExoString("_adx"));
        }
        else
        {
            CExoString sDirectory;
            sDirectory.Format("LIVE%d:MODULES\\", nDownloadedPackage);
            g_pExoResMan->RemoveResourceImageFile(sDirectory + sResourceName);
        }
    }

#else
    CExoString sLipsName;
    sLipsName.Format("LIPS:%s_loc", sResourceName.CStr());
    g_pExoResMan->RemoveEncapsulatedResourceFile(sLipsName);
    g_pExoResMan->RemoveEncapsulatedResourceFile("LIPS:localization");

    if (g_pExoResMan->Exists(sResourceName + CExoString("_a"),
                            RESTYPE_RIM, (unsigned long*)NULL))
    {
        g_pExoResMan->RemoveResourceImageFile(
            CExoString("RIMS:") + sResourceName + CExoString("_a"));
    }
    if (g_pExoResMan->Exists(sResourceName + CExoString("_adx"),
                            RESTYPE_RIM, (unsigned long*)NULL))
    {
        g_pExoResMan->RemoveResourceImageFile(
            CExoString("RIMS:") + sResourceName + CExoString("_adx"));
    }

    g_pExoResMan->RemoveEncapsulatedResourceFile("HD0:players");
#endif
    g_pAppManager->m_pClientExoApp->ResetModuleResourcesLoaded();
}
// PC 0x00505DB0
CExoString CSWSModule::GetModuleResourceName()
{
    int nColon;
    CExoString sResourceName;

    sResourceName = m_sModuleName;

    for (nColon = sResourceName.Find(':', 0); nColon != -1;
         nColon = sResourceName.Find(':', 0))
    {
        sResourceName =
            sResourceName.Right(sResourceName.GetLength() - nColon - 1);
    }
    return sResourceName;
}

static inline void NwsmoduleTrace3()
{
    NwsmoduleTrace2();
}

static inline void NwsmoduleTrace2()
{
    NwsmoduleTrace1();
}

static inline void NwsmoduleTrace1()
{
}

#ifdef _XBOX

void CSWSModule::TestPathPoints(char* szArgs)
{
    float fPersonalSpace = (float)atof(szArgs);
    CSWSArea* pArea = GetArea();
    if (pArea != NULL)
    {
        pArea->TestPathPoint(fPersonalSpace, 1);
    }
}
#endif
