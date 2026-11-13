#include "exobase.h"
#include <windows.h>
#include "resgff.h"
#include "nwerffile.h"
#include "appmanager.h"
#include "serverexoapp.h"
#include "clientexoapp.h"
#include "guiingame.h"
#include "partytables.h"
#include "serveraimaster.h"
#include "nwcarea.h"
#include "nwsobject.h"
#include "nwsitem.h"
#include "nwsdoor.h"
#include "nwstrigger.h"
#include "nwsencounter.h"
#include "nwswaypoint.h"
#include "nwssoundobject.h"
#include "nwsstore.h"
#include "nwsvartable.h"
#include "nwsplaceable.h"
#include "nwsspellimpact.h"
#include "nwmessage.h"
#include "nwsplayer.h"
#include "nwsmodule.h"
#include "swsareamap.h"
#include "swguicustompanel.h"
#include "exoresman.h"
#include "swsroom.h"
#include "nwsarea.h"
#include "nwsambientsound.h"
#include "gameobjectarray.h"
#include "nwsscriptevent.h"
#include "nwseffect.h"
#include "nwrules.h"
#include "twodimarrays.h"
#include "twodimarray.h"
#include "nwccreature.h"
#include "nwscreature.h"
#include "pathfindinfo.h"
#include "aurtexcontrol.h"
#include <float.h>
#include "worldtimer.h"

static inline void NwsareaSync4();
static inline void NwsareaSync3();
static inline void NwsareaSync2();
static inline void NwsareaSync1();

static inline void NwsareaTrace1();
#define NWSAREA_TRACE() NwsareaTrace1()

static CResRef* AllocateResRefArray(int nCount)
{
    return new CResRef[nCount];
}

const int CSWSArea::MAX_X_TILES = 32;
const int CSWSArea::MAX_Y_TILES = 32;
const float CSWSArea::MAX_OBJECT_VISIBLE_DISTANCE = 250.0f;
const float CSWSArea::MAX_OBJECT_CHATTALK_DISTANCE = 1000.0f;
const float CSWSArea::MAX_OBJECT_CHATWHISPER_DISTANCE = 3.0f;
const float CSWSArea::MAX_OBJECT_CONVERSATION_DISTANCE = 1000.0f;
const float CSWSArea::MAX_OBJECT_START_CONVERSATION_DISTANCE = 10.0f;
const float CSWSArea::MAX_OBJECT_CHATTALKSILENT_DISTANCE = 35.0f;
const float CSWSArea::MAX_OBJECT_CHATSHOUTSILENT_DISTANCE = 1000.0f;

int g_nBSPWidth = 40;
int g_nBSPHeight = 40;
// PC 0x004E2040 GOG 0x004E2160
int CSWSArea::GetFirstObjectIndiceByX(int* pnIndex, float fX)
{
    CGameObjectArray* pObjectArray =
        g_pAppManager->m_pServerExoApp->GetObjectArray();
    CGameObject* pGameObject;
    int nCount = m_lstObjectIds.num;
    int nHigh = nCount - 1;
    int nLow = 0;
    int nMid;

    for (;;)
    {
        if (nLow >= nHigh && nLow != nCount - 1)
        {
            if (nLow == nHigh)
            {
                *pnIndex = nLow;
                return 1;
            }
            return 0;
        }
        nMid = (nLow + nHigh) >> 1;
        if (pObjectArray->GetGameObject(m_lstObjectIds[nMid],
                                        &pGameObject) !=
            CGameObjectArray::SUCCESS)
        {
            return 0;
        }
        if (((CSWSObject*)pGameObject)->m_vPosition.m_fX >= fX)
        {
            if (nMid == nHigh)
            {
                break;
            }
            nHigh = nMid;
        }
        else
        {
            nLow = nMid + 1;
        }
    }
    *pnIndex = nMid;
    return 1;
}
// PC 0x004E2100 GOG 0x004E2220
int CSWSArea::UpdatePositionInObjectsArray(CGameObject* pObject)
{
    int nIndex = m_lstObjectIds.IndexOf(pObject->GetId());
    int nObjects = m_lstObjectIds.num;
    CGameObjectArray* pObjectArray =
        g_pAppManager->m_pServerExoApp->GetObjectArray();
    float fX = ((CSWSObject*)pObject)->m_vPosition.m_fX;
    CGameObject* pGameObject = pObject;

    int i = nIndex;

    if (nIndex + 1 != nObjects &&
        (pObjectArray->GetGameObject(m_lstObjectIds[nIndex + 1],
                                     &pGameObject) !=
             CGameObjectArray::SUCCESS ||
         !(((CSWSObject*)pGameObject)->m_vPosition.m_fX >= fX)))
    {
        while (i + 1 != nObjects &&
               pObjectArray->GetGameObject(m_lstObjectIds[i + 1],
                                           &pGameObject) ==
                   CGameObjectArray::SUCCESS &&
               ((CSWSObject*)pGameObject)->m_vPosition.m_fX < fX)
        {
            unsigned long oidSwap = m_lstObjectIds[i + 1];
            m_lstObjectIds[i + 1] = m_lstObjectIds[i];
            m_lstObjectIds[i] = oidSwap;
            i++;
        }
    }
    else if (nIndex != 0 &&
             (pObjectArray->GetGameObject(m_lstObjectIds[nIndex - 1],
                                          &pGameObject) !=
                  CGameObjectArray::SUCCESS ||
              !(((CSWSObject*)pGameObject)->m_vPosition.m_fX <= fX)))
    {
        for (; i != 0; i--)
        {
            if (pObjectArray->GetGameObject(m_lstObjectIds[i - 1],
                                            &pGameObject) !=
                    CGameObjectArray::SUCCESS ||
                !(((CSWSObject*)pGameObject)->m_vPosition.m_fX > fX))
            {
                break;
            }
            unsigned long oidSwap = m_lstObjectIds[i - 1];
            m_lstObjectIds[i - 1] = m_lstObjectIds[i];
            m_lstObjectIds[i] = oidSwap;
        }
    }
    return 1;
}
// PC 0x004E22D0 GOG 0x004E23F0
int CSWSArea::GetFirstObjectInArea(unsigned long& oidObject)
{
    int nObjectCount;

    nObjectCount = m_lstObjectIds.num;
    m_nObjectIterator = 0;
    if (nObjectCount == 0)
    {
        return 0;
    }
    oidObject = m_lstObjectIds[0];
    return 1;
}
// PC 0x004E2300 GOG 0x004E2420
int CSWSArea::GetNextObjectInArea(unsigned long& oidObject)
{
    int nObjectCount;
    int nObjectIndex;

    nObjectCount = m_lstObjectIds.num;
    nObjectIndex = m_nObjectIterator + 1;
    m_nObjectIterator = nObjectIndex;
    if (nObjectIndex < nObjectCount)
    {
        oidObject = m_lstObjectIds[nObjectIndex];
        return 1;
    }
    return 0;
}
// PC 0x004E3D00 GOG 0x004E3DD0
int CSWSArea::AIUpdate()
{
    unsigned long nCurrentDay;
    unsigned long nCurrentTime;
    unsigned long nElapsedDays;
    unsigned long nElapsedTime;

    g_pAppManager->GetServerExoApp()->GetWorldTimer()->GetWorldTime(
        &nCurrentDay, &nCurrentTime);
    g_pAppManager->GetServerExoApp()->GetWorldTimer()->SubtractWorldTimes(
        nCurrentDay, nCurrentTime, m_nLastAIUpdateDay, m_nLastAIUpdateTime,
        &nElapsedDays, &nElapsedTime);
    m_nLastAIUpdateDay = nCurrentDay;
    m_nLastAIUpdateTime = nCurrentTime;

    if (m_nWeatherOverride == WEATHER_AUTOMATIC && !(m_nFlags & AREA_FLAG_SUPPRESS_WEATHER))
    {
        if (m_nWeatherCheckTimer == 0)
        {
            int nRoll = rand() % 100;
            int bWeatherChanged = 0;

            if (m_nChanceRain > 0)
            {
                if (nRoll < m_nChanceRain)
                {
                    if (m_bWeatherStarted == 0)
                    {
                        m_nCurrentWeather = WEATHER_RAIN;
                        m_bWeatherStarted = 1;
                        bWeatherChanged = 1;
                    }
                }
                else if (m_bWeatherStarted != 0)
                {
                    m_nCurrentWeather = WEATHER_RAIN;
                    m_bWeatherStarted = 0;
                    bWeatherChanged = 1;
                }
            }
            else if (m_nChanceSnow > 0)
            {
                if (nRoll < m_nChanceSnow)
                {
                    if (m_bWeatherStarted == 0)
                    {
                        m_nCurrentWeather = WEATHER_SNOW;
                        m_bWeatherStarted = 1;
                        bWeatherChanged = 1;
                    }
                }
                else if (m_bWeatherStarted != 0)
                {
                    m_nCurrentWeather = WEATHER_SNOW;
                    m_bWeatherStarted = 0;
                    bWeatherChanged = 1;
                }
            }
            else if (m_nChanceLightning > 0)
            {
                if (nRoll < m_nChanceLightning)
                {
                    if (m_bWeatherStarted == 0)
                    {
                        m_nCurrentWeather = WEATHER_RAIN;
                        m_bWeatherStarted = 1;
                        bWeatherChanged = 1;
                    }
                }
                else if (m_bWeatherStarted != 0)
                {
                    m_nCurrentWeather = WEATHER_RAIN;
                    m_bWeatherStarted = 0;
                    bWeatherChanged = 1;
                }
            }
            if (bWeatherChanged != 0)
                SetCurrentWeather(m_nCurrentWeather, m_bWeatherStarted);
            m_nWeatherCheckTimer = WEATHER_CHECK_INTERVAL;
        }
        else if (m_nWeatherCheckTimer <= nElapsedTime)
        {
            m_nWeatherCheckTimer = 0;
        }
        else
        {
            m_nWeatherCheckTimer -= nElapsedTime;
        }
    }

    g_pAppManager->GetServerExoApp()->GetWorldTimer()->SubtractWorldTimes(
        nCurrentDay, nCurrentTime, m_nLastHeartbeatDay, m_nLastHeartbeatTime,
        &nElapsedDays, &nElapsedTime);
    if (nElapsedTime >= HEARTBEAT_INTERVAL)
    {
        if (m_nLastHeartbeatTime != 0)
            g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_ON_HEARTBEAT], m_nId, 1);
        m_nLastHeartbeatDay = nCurrentDay;
        m_nLastHeartbeatTime = nCurrentTime;
        return 1;
    }
    return 0;
}
// PC 0x004E3EF0 GOG 0x004E3FC0
void CSWSArea::IncrementPlayersInArea()
{
    if (m_nPlayersInArea == 0)
    {
        IncreaseAILevelPriority();
    }
    ++m_nPlayersInArea;
}
// PC 0x004E2340 GOG 0x004E2460
void CSWSArea::IncreaseAILevelPriority()
{
    CGameObjectArray* pObjectArray =
        g_pAppManager->m_pServerExoApp->GetObjectArray();
    CServerAIMaster* pAIMaster =
        g_pAppManager->m_pServerExoApp->GetServerAIMaster();
    CGameObject* pGameObject;
    int i = 0;

    if (m_lstObjectIds.num == 0)
    {
        return;
    }

    for (; i < m_lstObjectIds.num; i++)
    {
        unsigned long oidObject = m_lstObjectIds[i];

        if (pObjectArray->GetGameObject(oidObject, &pGameObject) ==
                CGameObjectArray::SUCCESS &&
            pGameObject->m_nObjectType > CGameObject::OBJECT_TYPE_AREA)
        {
            CSWSObject* pObject = (CSWSObject*)pGameObject;
            if (pObject->m_nAILevel == CServerAIMaster::AI_LEVEL_EMPTY_AREA_CREATURE)
            {
                if (pGameObject->AsSWSCreature() != NULL ||
                    pGameObject->AsSWSAreaOfEffectObject() != NULL ||
                    pGameObject->AsSWSTrigger() != NULL)
                {
                    pAIMaster->SetAILevel(pObject, 1);
                }
                else if (pGameObject->AsSWSDoor() != NULL)
                {
                    CSWSDoor* pDoor = pGameObject->AsSWSDoor();
                    if (pDoor->m_asScripts[CSWSDoor::SCRIPT_ON_HEARTBEAT]
                                .GetLength() > 0 &&
                        !pDoor->m_bStatic)
                    {
                        pAIMaster->SetAILevel(pObject, 1);
                    }
                }
                else if (pGameObject->AsSWSPlaceable() != NULL)
                {
                    CSWSPlaceable* pPlaceable = pGameObject->AsSWSPlaceable();
                    if (!pPlaceable->m_bStatic)
                    {
                        pAIMaster->SetAILevel(pObject, 1);
                    }
                }
            }
        }
    }
}
float __stdcall GetObstructionFactor(Vector vStart, Vector vEnd)  // PC 0x004E8290 GOG 0x004E8360
{
    unsigned long nStartTime = (unsigned long)
        (g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000);
    float fObstruction = 0.0f;
    Vector vZero;
    if (equal(vEnd, vZero))
        return 0.0f;
    CServerExoApp* pServer = g_pAppManager->GetServerExoApp();
    if (pServer == NULL)
        return 0.0f;
    CSWSArea* pArea = NULL;
    CSWSModule* pModule = pServer->GetModule();
    if (pModule != NULL)
        pArea = pModule->GetArea();
    if (pArea == NULL)
        return 0.0f;

    unsigned long oidHitObject = OBJECT_INVALID;
    Vector vHitPosition;
    unsigned long nElapsedTime = 0;
    unsigned long oidExclude = OBJECT_INVALID;
    while (!pArea->ClearLineOfSight(vStart, vEnd, &vHitPosition,
               &oidHitObject, oidExclude, OBJECT_INVALID, 0, 0) &&
           nElapsedTime < AREA_SOUND_OBSTRUCTION_TIME_LIMIT)
    {
        if (oidHitObject == OBJECT_INVALID)
            fObstruction += AREA_SOUND_STATIC_OBSTRUCTION;
        else
        {
            CGameObject* pObject =
                g_pAppManager->GetServerExoApp()->GetGameObject(oidHitObject);
            if (pObject != NULL)
            {
                if (pObject->AsSWSPlaceable() != NULL)
                    fObstruction += AREA_SOUND_STATIC_OBSTRUCTION;
                if (pObject->AsSWSDoor() != NULL)
                    fObstruction += AREA_SOUND_DOOR_OBSTRUCTION;
            }
        }
        if (fObstruction > 1.0f)
            return 1.0f;
        vStart = vHitPosition;
        vStart = vStart + normalize(vEnd - vStart) * AREA_SOUND_OBSTRUCTION_STEP;
        oidExclude = oidHitObject;
        oidHitObject = OBJECT_INVALID;
        nElapsedTime = (unsigned long)
            (g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000) - nStartTime;
    }
    return fObstruction;
}
// PC 0x004E8520 GOG 0x004E85F0
CSWSArea::CSWSArea(const CResRef& cResRef, int nAreaId, unsigned long oidArea)
    : CGameObject(CGameObject::OBJECT_TYPE_AREA, oidArea)
{
    m_pVersionInfo = new CSWSAreaVersionInfo;
    m_nFlags = 0;
    m_nChanceFog = 0;
    m_nHeight = 0;
    m_nWidth = 0;
    m_nGridMaskBytes = 0;
    m_nObjectByNameIndex = 0;
    m_nPlayersInArea = 0;
    m_nModSpotCheck = 0;
    m_nModListenCheck = 0;
    m_nLastHeartbeatDay = 0;
    m_nLastHeartbeatTime = 0;
    m_pGridTransTable = NULL;
    m_pPathPointTransTable = NULL;
    SetResRef(cResRef, nAreaId);
    m_pAreaPathfindInformation = new CPathfindInformation;
    InitializeStaticObjects();
    if (oidArea == OBJECT_INVALID)
        g_pAppManager->GetServerExoApp()->GetObjectArray()->AddInternalObject(
            m_nId, this, 0);
    else
        g_pAppManager->GetServerExoApp()->GetObjectArray()->AddObjectAtPos(
            m_nId, this);
    m_bTransitionCounted = 0;
    m_pAmbientSound = new CSWSAmbientSound(m_nId);
    unsigned long nDay;
    unsigned long nTime;
    g_pAppManager->GetServerExoApp()->GetWorldTimer()->GetWorldTime(&nDay, &nTime);
    m_nLastAIUpdateDay = nDay;
    m_nLastAIUpdateTime = nTime;
    m_nCurrentWeather = WEATHER_CLEAR;
    m_bWeatherStarted = 0;
    m_nWeatherCheckTimer = 0;
    m_nWeatherOverride = WEATHER_AUTOMATIC;
    m_nActiveWeather = WEATHER_CLEAR;
    m_bHeaderLoaded = 0;
    m_bUnescapable = 0;
    m_bDisableTransit = 0;
    m_nPlayerRestrictMode = 0;
    m_nPathPointCount = 0;
    m_pPathPoints = NULL;
    m_nPathConnectionCount = 0;
    m_pPathConnections = NULL;
    m_pPathfindInformation = NULL;
    m_nLoadScreenID = 0;
    m_nRoomCount = 0;
    m_pRooms = NULL;
    m_psRoomAudio = NULL;
    m_pnRoomAudio = NULL;
    m_pfRoomAudio = NULL;
    m_nRoomAudioCount = 0;
    m_nRoomAudioEntryCount = 0;
    m_pnRoomForceRating = NULL;
    m_pbRoomDisableWeather = NULL;
    m_pnRoomAudioIndex = NULL;
    m_nStealthXPMax = 0;
    m_nStealthXPCurrent = 0;
    m_nStealthXPLoss = 0;
    m_bStealthXPEnabled = 0;
    m_bTransitionPending = 0;
    m_nNextTransitionPendingId = 0;
    m_nCurrentTransitionPendingId = 0;
    m_pClientArea = NULL;
}
// PC 0x004E8920 GOG 0x004E89F0
CSWSArea::~CSWSArea()
{
    UnloadArea();
    g_pAppManager->GetServerExoApp()->GetObjectArray()->Delete(m_nId);
    if (m_pAreaPathfindInformation != NULL)
    {
        delete m_pAreaPathfindInformation;
        m_pAreaPathfindInformation = NULL;
    }
    if (m_pAmbientSound != NULL)
    {
        delete m_pAmbientSound;
        m_pAmbientSound = NULL;
    }
    if (m_pPathPoints != NULL)
    {
        delete[] m_pPathPoints;
        m_pPathPoints = NULL;
        m_nPathPointCount = 0;
    }
    if (m_pPathConnections != NULL)
    {
        delete[] m_pPathConnections;
        m_pPathConnections = NULL;
        m_nPathConnectionCount = 0;
    }
    if (m_psRoomAudio != NULL)
    {
        delete[] m_psRoomAudio;
        m_psRoomAudio = NULL;
    }
    if (m_pnRoomAudio != NULL)
    {
        delete[] m_pnRoomAudio;
        m_pnRoomAudio = NULL;
    }
    if (m_pfRoomAudio != NULL)
    {
        delete[] m_pfRoomAudio;
        m_pfRoomAudio = NULL;
    }
    if (m_pnRoomForceRating != NULL)
    {
        delete[] m_pnRoomForceRating;
        m_pnRoomForceRating = NULL;
    }
    if (m_pbRoomDisableWeather != NULL)
    {
        delete[] m_pbRoomDisableWeather;
        m_pbRoomDisableWeather = NULL;
    }
    if (m_pnRoomAudioIndex != NULL)
    {
        delete[] m_pnRoomAudioIndex;
        m_pnRoomAudioIndex = NULL;
    }
    if (m_pClientArea != NULL)
    {
        m_pClientArea->DetachFromServerArea();
        m_pClientArea = NULL;
    }
    g_pAppManager->GetServerExoApp()->GetModule()->ClearAreaCache();
    g_pAppManager->GetServerExoApp()->ClearAreaCache();
}
// PC 0x004E8CB0 GOG 0x004E8D80
void CSWSArea::EventHandler(unsigned long nEventId, unsigned long oidCaller,
                           void* pEventData, unsigned long nCalendarDay,
                           unsigned long nTimeOfDay)
{
    if (nEventId == CServerAIMaster::EVENT_TIMED_EVENT)
    {
        if (pEventData != NULL)
            g_pVirtualMachine->RunScriptSituation(pEventData, GetId(), 1);
    }
    else if (nEventId == CServerAIMaster::EVENT_SPAWN_BODY_BAG)
    {
        if (pEventData != NULL)
        {
            CServerAIBodyBagInfo* pInfo = (CServerAIBodyBagInfo*)pEventData;
            CGameObjectArray* pObjects =
                g_pAppManager->GetServerExoApp()->GetObjectArray();
            if (pObjects != NULL)
            {
                CGameObject* pObject;
                if (pObjects->GetGameObject(pInfo->m_nBodyBagId, &pObject) ==
                    CGameObjectArray::SUCCESS)
                {
                    CSWSPlaceable* pPlaceable = pObject->AsSWSPlaceable();
                    if (pPlaceable != NULL)
                    {
                        const Vector& vPosition = pInfo->m_vPosition;
                        pPlaceable->AddToArea(this, vPosition, 0);
                        if (pPlaceable->m_bIsCorpse)
                        {
                            Vector vMinimum;
                            Vector vMaximum;
                            pPlaceable->GetMeshBoundingBox(vMinimum, vMaximum);
                            BudgeCreatures(vPosition, vMinimum, vMaximum,
                                           OBJECT_INVALID, 0);
                        }
                    }
                }
            }
        }
    }
    else if (nEventId == CServerAIMaster::EVENT_APPLY_EFFECT)
    {
        CGameEffect* pEffect = (CGameEffect*)pEventData;
        if (pEffect != NULL &&
            pEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_VISUALEFFECT)
        {
            Vector vPosition(pEffect->GetFloat(0), pEffect->GetFloat(1),
                             pEffect->GetFloat(2));
            Vector vOrientation;
            ApplyEffect(pEffect, vPosition, vOrientation);
        }
    }
    else if (nEventId == CServerAIMaster::EVENT_SIGNAL_EVENT)
    {
        CScriptEvent* pEvent = (CScriptEvent*)pEventData;
        if (pEvent->m_nType == CSWSSCRIPTEVENT_EVENTTYPE_ON_HEARTBEAT)
        {
            m_nLastHeartbeatDay = nCalendarDay;
            m_nLastHeartbeatTime = nTimeOfDay;
            g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_ON_HEARTBEAT], GetId(), 1);
        }
        else if (pEvent->m_nType == CSWSSCRIPTEVENT_EVENTTYPE_ON_USER_DEFINED_EVENT)
        {
            m_nCustomScriptEventId = pEvent->GetInteger(0);
            g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_ON_USER_DEFINED], GetId(), 1);
        }
        else if (pEvent->m_nType == CSWSSCRIPTEVENT_EVENTTYPE_ON_OBJECT_ENTER)
        {
            m_oidEnteringObject = oidCaller;
            int bLoadFromSaveGame =
                g_pAppManager->GetServerExoApp()->GetLoadFromSaveGame();
            g_pAppManager->GetServerExoApp()->SetLoadFromSaveGame(
                pEvent->GetInteger(0));
            g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_ON_ENTER], GetId(), 1);
            g_pAppManager->GetServerExoApp()->SetLoadFromSaveGame(
                bLoadFromSaveGame);
        }
        else if (pEvent->m_nType == CSWSSCRIPTEVENT_EVENTTYPE_ON_OBJECT_EXIT)
        {
            m_oidExitingObject = oidCaller;
            g_pVirtualMachine->RunScript(&m_sScripts[SCRIPT_ON_EXIT], GetId(), 1);
        }
        delete pEvent;
    }
    else if (nEventId == CServerAIMaster::EVENT_AREA_TRANSITION)
    {
        CScriptEvent* pEvent = (CScriptEvent*)pEventData;
        if (m_bTransitionPending)
        {
            unsigned char nCurrentId = m_nCurrentTransitionPendingId;
            if (pEvent->GetInteger(CScriptEvent::TRANSITION_INTEGER_ID) ==
                nCurrentId)
            {
                g_pAppManager->GetServerExoApp()->SetMoveToModulePending(1);
                g_pAppManager->GetServerExoApp()->SetMoveToModuleString(
                    pEvent->GetString(CScriptEvent::TRANSITION_STRING_MODULE));
                g_pAppManager->GetServerExoApp()->SetMoveToModuleStartWaypoint(
                    pEvent->GetString(CScriptEvent::TRANSITION_STRING_WAYPOINT));
                int nMovie = CScriptEvent::TRANSITION_STRING_FIRST_MOVIE;
                for (int nRemaining = CScriptEvent::NUM_TRANSITION_STRINGS -
                         CScriptEvent::TRANSITION_STRING_FIRST_MOVIE;
                     nRemaining != 0; nRemaining--)
                {
                    g_pAppManager->GetClientExoApp()->AddMoveToModuleMovie(
                        pEvent->GetString(nMovie));
                    nMovie++;
                }
                SetTransitionPending(0, 0);
            }
        }
        else
        {
            CScriptEvent* pNextEvent = new CScriptEvent;
            unsigned char nTransitionId = GetNextTransitionPendingID();
            pNextEvent->SetInteger(CScriptEvent::TRANSITION_INTEGER_ID, nTransitionId);
            int nString = 0;
            for (int nRemaining = CScriptEvent::NUM_TRANSITION_STRINGS;
                 nRemaining != 0; nRemaining--)
            {
                pNextEvent->SetString(nString, pEvent->GetString(nString));
                nString++;
            }
            g_pAppManager->GetServerExoApp()->GetServerAIMaster()->
                AddEventDeltaTime(0, TRANSITION_FADE_DELAY, GetId(), GetId(),
                                 CServerAIMaster::EVENT_AREA_TRANSITION,
                                 pNextEvent);
            {
                Vector vBlack;
                g_pAppManager->GetClientExoApp()->GetInGameGui()->StartGlobalFade(
                    0, 0.0f, 0.5f, vBlack);
            }
            SetTransitionPending(1, nTransitionId);
        }
        delete pEvent;
    }
}
// PC 0x004E9230 GOG 0x004E9300
int CSWSArea::RemoveObjectFromArea(unsigned long oidObject)
{
    if (m_lstObjectIds.Contains(oidObject))
    {
        m_lstObjectIds.Remove(oidObject);
    }

    CGameObjectArray* pObjectArray =
        g_pAppManager->GetServerExoApp()->GetObjectArray();
    CGameObject* pGameObject;
    if (pObjectArray->GetGameObject(oidObject, &pGameObject) == CGameObjectArray::SUCCESS &&
        pGameObject != NULL)
    {
        if (pGameObject->m_nObjectType > CGameObject::OBJECT_TYPE_AREA &&
            ((CSWSObject*)pGameObject)->GetDead())
        {
            return 1;
        }
        if (pGameObject->AsSWSCreature() != NULL)
        {
            CSWSPlayer* pPlayer = g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(oidObject);
            if (pPlayer != NULL)
            {
                DecrementPlayersInArea();
            }
            CScriptEvent* pEvent = new CScriptEvent;
            pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_OBJECT_EXIT;
            CServerExoApp* pServer = g_pAppManager->GetServerExoApp();
            unsigned long oidArea = m_nId;
            pServer->GetServerAIMaster()->AddEventDeltaTime(
                0, 0, oidObject, oidArea, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
        }
    }
    return 1;
}
// PC 0x004E3F10 GOG 0x004E3FE0
void CSWSArea::DecrementPlayersInArea()
{
    if (--m_nPlayersInArea == 0)
    {
        DecreaseAILevelPriority();
    }
}
// PC 0x004E2460 GOG 0x004E2580
void CSWSArea::DecreaseAILevelPriority()
{
    CGameObjectArray* pObjectArray =
        g_pAppManager->m_pServerExoApp->GetObjectArray();
    CServerAIMaster* pAIMaster =
        g_pAppManager->m_pServerExoApp->GetServerAIMaster();
    CSWSObject* pObject;

    for (int i = 0; i < m_lstObjectIds.num; i++)
    {
        unsigned long oidObject = m_lstObjectIds[i];

        if (pObjectArray->GetGameObject(oidObject, (CGameObject**)&pObject) ==
                CGameObjectArray::SUCCESS &&
            pObject->m_nObjectType > CGameObject::OBJECT_TYPE_AREA &&
            pObject->AsSWSCreature() != NULL && pObject->m_nAILevel == CServerAIMaster::AI_LEVEL_OCCUPIED_AREA_CREATURE)
        {
            pAIMaster->SetAILevel(pObject, CServerAIMaster::AI_LEVEL_EMPTY_AREA_CREATURE);
        }
    }
}
// PC 0x004E3F20 GOG 0x004E3FF0
int CSWSArea::LoadAreaHeader(CResStruct* pStruct)
{
    int bFieldRead;
    m_pVersionInfo->m_nID = m_pRes->ReadFieldINT(pStruct, "ID", bFieldRead, 0);
    m_pVersionInfo->m_nCreatorID = m_pRes->ReadFieldINT(pStruct, "Creator_ID", bFieldRead, 0);
    m_pVersionInfo->m_nVersion = m_pRes->ReadFieldDWORD(pStruct, "Version", bFieldRead, 0);
    m_sComments = m_pRes->ReadFieldCExoString(pStruct, "Comments", bFieldRead, "");

    int i;
    CResList cList;
    CResStruct cElement;
    if (m_pRes->GetList(&cList, pStruct, "Expansion_List") == 1)
    {
        int nExpansions = m_pRes->GetListCount(&cList);
        for (i = 0; i < nExpansions; i++)
        {
            m_pRes->GetListElement(&cElement, &cList, i);
            NWAreaExpansion_st* pExpansion = new NWAreaExpansion_st;
            if (pExpansion == NULL)
            {
                m_pRes->Release();
                return 0;
            }
            pExpansion->m_sExpansionName = m_pRes->ReadFieldCExoLocString(
                &cElement, "Expansion_Name", bFieldRead, CExoLocString());
            pExpansion->m_nExpansionID = m_pRes->ReadFieldINT(&cElement, "Expansion_ID", bFieldRead, 0);
            m_lstExpansions.AddTail(pExpansion);
        }
    }

    CExoString sScript;
    m_pRes->ReadFieldCResRef(pStruct, "OnHeartbeat", bFieldRead, "").CopyToString(sScript);
    m_sScripts[SCRIPT_ON_HEARTBEAT] = sScript;
    m_pRes->ReadFieldCResRef(pStruct, "OnUserDefined", bFieldRead, "").CopyToString(sScript);
    m_sScripts[SCRIPT_ON_USER_DEFINED] = sScript;
    m_pRes->ReadFieldCResRef(pStruct, "OnEnter", bFieldRead, "").CopyToString(sScript);
    m_sScripts[SCRIPT_ON_ENTER] = sScript;
    m_pRes->ReadFieldCResRef(pStruct, "OnExit", bFieldRead, "").CopyToString(sScript);
    m_sScripts[SCRIPT_ON_EXIT] = sScript;
    m_lsName = m_pRes->ReadFieldCExoLocString(pStruct, "Name", bFieldRead, CExoLocString());
    SetTag(m_pRes->ReadFieldCExoString(pStruct, "Tag", bFieldRead, ""));
    m_nFlags = m_pRes->ReadFieldDWORD(pStruct, "Flags", bFieldRead, 0);
    m_nTurnRateRow = m_pRes->ReadFieldINT(pStruct, "CameraStyle", bFieldRead, 0);
    m_cDefaultEnvMap = m_pRes->ReadFieldCResRef(pStruct, "DefaultEnvMap", bFieldRead, "");
    m_bHeaderLoaded = 1;
    if (g_pAppManager->GetServerExoApp()->GetPartyTable()->m_nFollowState == 1)
        g_pAppManager->GetServerExoApp()->GetPartyTable()->SetPartyFollowState(0);
    int nUnescapableDefault = m_bUnescapable;
    m_bUnescapable = m_pRes->ReadFieldBYTE(pStruct, "Unescapable", bFieldRead, nUnescapableDefault);
    m_bDisableTransit = m_pRes->ReadFieldBYTE(pStruct, "DisableTransit", bFieldRead, 0);
    int nRestrictDefault = m_nPlayerRestrictMode;
    SetPlayerRestrictMode(m_pRes->ReadFieldBYTE(pStruct, "RestrictMode", bFieldRead, nRestrictDefault));
    m_nChanceRain = m_pRes->ReadFieldINT(pStruct, "ChanceRain", bFieldRead, 0);
    m_nChanceSnow = m_pRes->ReadFieldINT(pStruct, "ChanceSnow", bFieldRead, 0);
    m_nChanceFog = m_pRes->ReadFieldINT(pStruct, "ChanceFog", bFieldRead, 0);
    m_nChanceLightning = m_pRes->ReadFieldINT(pStruct, "ChanceLightning", bFieldRead, 0);
    m_nWind = m_pRes->ReadFieldINT(pStruct, "WindPower", bFieldRead, 0);
    if (m_nFlags & AREA_FLAG_SUPPRESS_WEATHER)
    {
        m_nChanceRain = 0;
        m_nChanceSnow = 0;
        m_nChanceLightning = 0;
        m_nWind = 0;
        m_nChanceFog = 0;
    }
    m_nMoonAmbientColor = m_pRes->ReadFieldDWORD(pStruct, "MoonAmbientColor", bFieldRead, 0);
    m_nMoonDiffuseColor = m_pRes->ReadFieldDWORD(pStruct, "MoonDiffuseColor", bFieldRead, 0);
    m_nMoonFogColor = m_pRes->ReadFieldDWORD(pStruct, "MoonFogColor", bFieldRead, 0);
    m_fMoonFogNear = m_pRes->ReadFieldFLOAT(pStruct, "MoonFogNear", bFieldRead, AREA_DEFAULT_FOG_DISTANCE);
    if (m_fMoonFogNear < 0.0f)
        m_fMoonFogNear = 0.0f;
    m_fMoonFogFar = m_pRes->ReadFieldFLOAT(pStruct, "MoonFogFar", bFieldRead, AREA_DEFAULT_FOG_DISTANCE);
    if (m_fMoonFogFar < 0.0f)
        m_fMoonFogFar = 0.0f;
    m_bMoonFogOn = m_pRes->ReadFieldBYTE(pStruct, "MoonFogOn", bFieldRead, 0);
    m_bMoonShadows = m_pRes->ReadFieldBYTE(pStruct, "MoonShadows", bFieldRead, 0);
    m_nSunAmbientColor = m_pRes->ReadFieldDWORD(pStruct, "SunAmbientColor", bFieldRead, 0);
    m_nSunDiffuseColor = m_pRes->ReadFieldDWORD(pStruct, "SunDiffuseColor", bFieldRead, 0);
    m_nSunFogColor = m_pRes->ReadFieldDWORD(pStruct, "SunFogColor", bFieldRead, 0);
    m_fSunFogNear = m_pRes->ReadFieldFLOAT(pStruct, "SunFogNear", bFieldRead, AREA_DEFAULT_FOG_DISTANCE);
    if (m_fSunFogNear < 0.0f)
        m_fSunFogNear = 0.0f;
    m_fSunFogFar = m_pRes->ReadFieldFLOAT(pStruct, "SunFogFar", bFieldRead, AREA_DEFAULT_FOG_DISTANCE);
    if (m_fSunFogFar < 0.0f)
        m_fSunFogFar = 0.0f;
    m_bSunFogOn = m_pRes->ReadFieldBYTE(pStruct, "SunFogOn", bFieldRead, 0);
    m_bSunShadows = m_pRes->ReadFieldBYTE(pStruct, "SunShadows", bFieldRead, 0);
    m_bDayNightCycle = m_pRes->ReadFieldBYTE(pStruct, "DayNightCycle", bFieldRead, 0);
    m_bIsNight = m_pRes->ReadFieldBYTE(pStruct, "IsNight", bFieldRead, 0);
    m_nDynAmbientColor = m_pRes->ReadFieldDWORD(pStruct, "DynAmbientColor", bFieldRead, 0);
    int nNoRestDefault = m_bNoRest;
    m_bNoRest = m_pRes->ReadFieldBYTE(pStruct, "NoRest", bFieldRead, nNoRestDefault);
    unsigned char nShadowOpacityDefault = m_nShadowOpacity;
    m_nShadowOpacity = m_pRes->ReadFieldBYTE(pStruct, "ShadowOpacity", bFieldRead, nShadowOpacityDefault);
    m_nLightingScheme = m_pRes->ReadFieldBYTE(pStruct, "LightingScheme", bFieldRead, 0);
    m_nModSpotCheck = m_pRes->ReadFieldINT(pStruct, "ModSpotCheck", bFieldRead, 0);
    m_nModListenCheck = m_pRes->ReadFieldINT(pStruct, "ModListenCheck", bFieldRead, 0);
    m_bMiniGame = m_pRes->GetStructFromStruct(&cElement, pStruct, "MiniGame");
    m_nLoadScreenID = m_pRes->ReadFieldWORD(pStruct, "LoadScreenID", bFieldRead, 0);
    m_nGrassDiffuse = m_pRes->ReadFieldDWORD(pStruct, "Grass_Diffuse", bFieldRead, 0);
    m_nGrassAmbient = m_pRes->ReadFieldDWORD(pStruct, "Grass_Ambient", bFieldRead, 0);
    m_nGrassEmissive = m_pRes->ReadFieldDWORD(pStruct, "Grass_Emissive", bFieldRead, 0);
    m_fGrassDensity = m_pRes->ReadFieldFLOAT(pStruct, "Grass_Density", bFieldRead, 0.0f);
    m_fGrassQuadSize = m_pRes->ReadFieldFLOAT(pStruct, "Grass_QuadSize", bFieldRead, 0.0f);
    m_cGrassTexName = m_pRes->ReadFieldCResRef(pStruct, "Grass_TexName", bFieldRead, "");
    if (!m_cGrassTexName.IsValid())
        m_cGrassTexName = "grass";
    m_fGrassProbability[GRASS_PROB_LL] = m_pRes->ReadFieldFLOAT(pStruct, "Grass_Prob_LL", bFieldRead, 0.0f);
    m_fGrassProbability[GRASS_PROB_LR] = m_pRes->ReadFieldFLOAT(pStruct, "Grass_Prob_LR", bFieldRead, 0.0f);
    m_fGrassProbability[GRASS_PROB_UL] = m_pRes->ReadFieldFLOAT(pStruct, "Grass_Prob_UL", bFieldRead, 0.0f);
    m_fGrassProbability[GRASS_PROB_UR] = m_pRes->ReadFieldFLOAT(pStruct, "Grass_Prob_UR", bFieldRead, 0.0f);
    m_fAlphaTest = m_pRes->ReadFieldFLOAT(pStruct, "AlphaTest", bFieldRead, AREA_DEFAULT_ALPHA_TEST);

    m_pRes->GetList(&cList, pStruct, "Rooms");
    m_nRoomAudioCount = m_pRes->GetListCount(&cList);
    InitializeRoomAudioParams(m_nRoomAudioCount);
    for (i = 0; i < m_nRoomAudioCount; i++)
    {
        m_pRes->GetListElement(&cElement, &cList, i);
        m_psRoomAudio[i] = m_pRes->ReadFieldCExoString(&cElement, "RoomName", bFieldRead, "");
        m_pnRoomAudio[i] = m_pRes->ReadFieldINT(&cElement, "EnvAudio", bFieldRead, 0);
        m_pfRoomAudio[i] = m_pRes->ReadFieldFLOAT(&cElement, "AmbientScale", bFieldRead, 0.0f);
        m_pnRoomForceRating[i] = m_pRes->ReadFieldINT(&cElement, "ForceRating", bFieldRead, 0);
        m_pbRoomDisableWeather[i] = m_pRes->ReadFieldBYTE(&cElement, "DisableWeather", bFieldRead, 0);
        CResList cPartSounds;
        CResStruct cPart;
        if (m_pRes->GetList(&cPartSounds, &cElement, "PartSounds"))
        {
            int nPartSounds = m_pRes->GetListCount(&cPartSounds);
            m_nRoomAudioEntryCount += nPartSounds;
            for (int nPart = 0; nPart < nPartSounds; nPart++)
            {
                m_pRes->GetListElement(&cPart, &cPartSounds, nPart);
                m_lstRoomAudioNames.Add(m_psRoomAudio[i]);
                m_lstRoomAudioLooping.Add(m_pRes->ReadFieldBYTE(&cPart, "Looping", bFieldRead, 0));
                m_lstRoomAudioModelParts.Add(m_pRes->ReadFieldCExoString(&cPart, "ModelPart", bFieldRead, ""));
                m_lstRoomAudioOmenEvents.Add(m_pRes->ReadFieldCExoString(&cPart, "OmenEvent", bFieldRead, ""));
                m_lstRoomAudioSounds.Add(m_pRes->ReadFieldCResRef(&cPart, "Sound", bFieldRead, ""));
            }
        }
    }

    CResRef cArea = g_pAppManager->GetServerExoApp()->GetModule()->m_cAreaResRef;
    CExoString sArea;
    CExoString sMap;
    cArea.CopyToString(sArea);
    sMap.Format("lbl_map%s", sArea.CStr());
    if (g_pExoResMan->Exists(sMap, RESTYPE_TGA, (unsigned long*)NULL) ||
        g_pExoResMan->Exists(sMap, RESTYPE_TPC, (unsigned long*)NULL))
    {
        if (m_pRes->GetStructFromStruct(&cElement, pStruct, "Map"))
        {
            int nMapResX = m_pRes->ReadFieldINT(&cElement, "MapResX", bFieldRead, 0);
            if (nMapResX == 0)
                g_pAppManager->GetServerExoApp()->GetModule()->GetAreaMap()->Initialize(0, 88, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0, 1);
            else
            {
                unsigned long nNorthAxis = m_pRes->ReadFieldINT(&cElement, "NorthAxis", bFieldRead, 0);
                int nMapZoom = m_pRes->ReadFieldINT(&cElement, "MapZoom", bFieldRead, 1);
                int nMapPt1X, nMapPt1Y, nMapPt2X, nMapPt2Y;
                if (m_pRes->GetFieldType(&cElement, "MapPt1X", CResGFF::FIELD_INDEX_INVALID) == CResGFF::FIELD_FLOAT)
                {
                    nMapPt1X = (int)floor(m_pRes->ReadFieldFLOAT(&cElement, "MapPt1X", bFieldRead, 0.0f) * CSWSAreaMap::MAP_PIXEL_WIDTH + 0.5f);
                    nMapPt1Y = (int)floor(m_pRes->ReadFieldFLOAT(&cElement, "MapPt1Y", bFieldRead, 0.0f) * CSWSAreaMap::MAP_PIXEL_HEIGHT + 0.5f);
                    nMapPt2X = (int)floor(m_pRes->ReadFieldFLOAT(&cElement, "MapPt2X", bFieldRead, 0.0f) * CSWSAreaMap::MAP_PIXEL_WIDTH + 0.5f);
                    nMapPt2Y = (int)floor(m_pRes->ReadFieldFLOAT(&cElement, "MapPt2Y", bFieldRead, 0.0f) * CSWSAreaMap::MAP_PIXEL_HEIGHT + 0.5f);
                }
                else
                {
                    nMapPt1X = m_pRes->ReadFieldINT(&cElement, "MapPt1X", bFieldRead, 0);
                    nMapPt1Y = m_pRes->ReadFieldINT(&cElement, "MapPt1Y", bFieldRead, 0);
                    nMapPt2X = m_pRes->ReadFieldINT(&cElement, "MapPt2X", bFieldRead, 0);
                    nMapPt2Y = m_pRes->ReadFieldINT(&cElement, "MapPt2Y", bFieldRead, 0);
                }
                float fWorldPt1X = m_pRes->ReadFieldFLOAT(&cElement, "WorldPt1X", bFieldRead, 0.0f);
                float fWorldPt1Y = m_pRes->ReadFieldFLOAT(&cElement, "WorldPt1Y", bFieldRead, 0.0f);
                float fWorldPt2X = m_pRes->ReadFieldFLOAT(&cElement, "WorldPt2X", bFieldRead, 0.0f);
                float fWorldPt2Y = m_pRes->ReadFieldFLOAT(&cElement, "WorldPt2Y", bFieldRead, 0.0f);
                g_pAppManager->GetServerExoApp()->GetModule()->GetAreaMap()->Initialize(
                    1, nMapResX, nNorthAxis, fWorldPt1X, fWorldPt1Y, fWorldPt2X,
                    fWorldPt2Y, nMapPt1X, nMapPt1Y, nMapPt2X, nMapPt2Y, nMapZoom);
            }
        }
        else
            g_pAppManager->GetServerExoApp()->GetModule()->GetAreaMap()->Initialize(0, 88, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0, 1);
    }
    else
        g_pAppManager->GetServerExoApp()->GetModule()->GetAreaMap()->Initialize(0, 88, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0, 1);

    unsigned long nMaxStealthXPDefault = m_nStealthXPMax;
    SetMaxStealthXP(m_pRes->ReadFieldDWORD(pStruct, "StealthXPMax", bFieldRead, nMaxStealthXPDefault));
    SetCurrentStealthXP(m_pRes->ReadFieldDWORD(pStruct, "StealthXPCurrent", bFieldRead, m_nStealthXPMax));
    unsigned long nStealthXPLossDefault = m_nStealthXPLoss;
    m_nStealthXPLoss = m_pRes->ReadFieldDWORD(pStruct, "StealthXPLoss", bFieldRead, nStealthXPLossDefault);
    int nStealthXPEnabledDefault = m_bStealthXPEnabled;
    m_bStealthXPEnabled = m_pRes->ReadFieldBYTE(pStruct, "StealthXPEnabled", bFieldRead, nStealthXPEnabledDefault);
    m_nDirtyARGB[0] = m_pRes->ReadFieldINT(pStruct, "DirtyARGBOne", bFieldRead, 0);
    m_nDirtySize[0] = m_pRes->ReadFieldINT(pStruct, "DirtySizeOne", bFieldRead, 16);
    int nDirtyFormulaOne = m_pRes->ReadFieldINT(pStruct, "DirtyFormulaOne", bFieldRead, 1);
    if (nDirtyFormulaOne == 1)
    {
        nDirtyFormulaOne = 0;
    }
    else if (nDirtyFormulaOne == 0)
    {
        nDirtyFormulaOne = 1;
    }
    m_nDirtyFormula[0] = nDirtyFormulaOne;
    m_nDirtyFunc[0] = m_pRes->ReadFieldINT(pStruct, "DirtyFuncOne", bFieldRead, 0);
    m_nDirtyARGB[1] = m_pRes->ReadFieldINT(pStruct, "DirtyARGBTwo", bFieldRead, 0);
    m_nDirtySize[1] = m_pRes->ReadFieldINT(pStruct, "DirtySizeTwo", bFieldRead, 16);
    int nDirtyFormulaTwo = m_pRes->ReadFieldINT(pStruct, "DirtyFormulaTwo", bFieldRead, 1);
    if (nDirtyFormulaTwo == 1)
    {
        nDirtyFormulaTwo = 0;
    }
    else if (nDirtyFormulaTwo == 0)
    {
        nDirtyFormulaTwo = 1;
    }
    m_nDirtyFormula[1] = nDirtyFormulaTwo;
    m_nDirtyFunc[1] = m_pRes->ReadFieldINT(pStruct, "DirtyFuncTwo", bFieldRead, 0);
    m_nDirtyARGB[2] = m_pRes->ReadFieldINT(pStruct, "DirtyARGBThree", bFieldRead, 0);
    m_nDirtySize[2] = m_pRes->ReadFieldINT(pStruct, "DirtySizeThree", bFieldRead, 16);
    int nDirtyFormulaThree = m_pRes->ReadFieldINT(pStruct, "DirtyFormulaThree", bFieldRead, 1);
    if (nDirtyFormulaThree == 1)
    {
        nDirtyFormulaThree = 0;
    }
    else if (nDirtyFormulaThree == 0)
    {
        nDirtyFormulaThree = 1;
    }
    m_nDirtyFormula[2] = nDirtyFormulaThree;
    m_nDirtyFunc[2] = m_pRes->ReadFieldINT(pStruct, "DirtyFuncThree", bFieldRead, 0);
    m_bTransitionPending = m_pRes->ReadFieldBYTE(pStruct, "TransPending", bFieldRead, m_bTransitionPending);
    m_nNextTransitionPendingId = m_pRes->ReadFieldBYTE(pStruct, "TransPendNextID", bFieldRead, m_nNextTransitionPendingId);
    m_nCurrentTransitionPendingId = m_pRes->ReadFieldBYTE(pStruct, "TransPendCurrID", bFieldRead, m_nCurrentTransitionPendingId);
    return 1;
}
// PC 0x004E24F0 GOG 0x004E2610
int CSWSArea::LoadRoomInfo(CResStruct* pStruct)
{
    m_nWidth = g_nBSPWidth;
    m_nHeight = g_nBSPHeight;

    CLYT* pLayout = new CLYT();
    int bSuccess = pLayout->LoadLayout(m_cResRef);

    if (bSuccess)
    {
        m_nRoomCount = pLayout->GetRoomCount();
        m_pRooms = LoadRooms(pLayout);
    }

    pLayout->UnloadLayout();
    delete pLayout;

    return bSuccess;
}
// PC 0x004DF890 GOG 0x004DF9B0
CSWSRoom* CSWSArea::LoadRooms(CLYT* pLayout)
{
    int nRoomCount = pLayout->GetRoomCount();

    if (nRoomCount == 0)
    {
        return NULL;
    }

    CSWSRoom* pRooms = new CSWSRoom[nRoomCount];
    int i, j;

    for (i = 0; i < nRoomCount; i++)
    {
        pRooms[i].SetRoomInfo(pLayout->GetRoomName(i),
                              pLayout->GetRoomPosition(i));
        pRooms[i].LoadWalkMesh(0);
        pRooms[i].TransformToWorld();
    }

    for (i = 0; i < nRoomCount; i++)
    {
        for (j = i + 1; j < nRoomCount; j++)
        {
            pRooms[i].WeldRoom(pRooms[j], 0.01f);
        }
    }

    for (i = 0; i < nRoomCount; i++)
    {
        for (j = i + 1; j < nRoomCount; j++)
        {
            pRooms[i].SetRoomAdjacency(pRooms[j], i, j);
        }
    }

    for (i = 0; i < nRoomCount; i++)
    {
        pRooms[i].ComputeMeshNormals();
    }

    return pRooms;
}
// PC 0x004DFA90 GOG 0x004DFBB0
int CSWSArea::LoadCreatures(CResGFF* pRes, CResStruct* pStruct,
                            int bLoadingGame, int bUseTemplates)
{
    CResList cList;
    CResStruct cStruct;
    int bSuccess;

    if (pRes->GetList(&cList, pStruct, "Creature List") != 0)
    {
        for (unsigned long i = 0; i < pRes->GetListCount(&cList); i++)
        {
            if (pRes->GetListElement(&cStruct, &cList, i) == 0 ||
                pRes->GetElementType(&cStruct) != GFF_STRUCT_CREATURE)
                continue;

            CSWSCreature* pCreature;
            if (bUseTemplates != 0)
            {
                pCreature = new CSWSCreature(OBJECT_INVALID, 0);
                if (pCreature == NULL)
                    continue;

                CResRef cDefault("");
                CResRef cTemplate = pRes->ReadFieldCResRef(
                    &cStruct, "TemplateResRef", bSuccess, cDefault);
                if (bSuccess == 0 ||
                    pCreature->LoadFromTemplate(cTemplate, 1) == 0)
                {
                    delete pCreature;
                    continue;
                }
            }
            else
            {
                unsigned long oidCreature = pRes->ReadFieldDWORD(
                    &cStruct, "ObjectId", bSuccess, OBJECT_INVALID);
                pCreature = new CSWSCreature(oidCreature, 0);
                if (pCreature == NULL)
                    continue;

                if (pCreature->LoadCreature(pRes, &cStruct, bLoadingGame) == 0)
                {
                    delete pCreature;
                    continue;
                }
            }

            Vector vPosition;
            Vector vSafePosition;
            vPosition.m_fX = pRes->ReadFieldFLOAT(
                &cStruct, "XPosition", bSuccess, 0.0f);
            vPosition.m_fY = pRes->ReadFieldFLOAT(
                &cStruct, "YPosition", bSuccess, 0.0f);
            vPosition.m_fZ = pRes->ReadFieldFLOAT(
                &cStruct, "ZPosition", bSuccess, 0.0f);
            if (ComputeSafeLocation(vPosition,
                    AREA_CREATURE_LOAD_SAFE_LOCATION_RADIUS,
                    pCreature->GetPathfindInformation(), 1, &vSafePosition))
                vPosition = vSafePosition;

            pCreature->AddToArea(this, vPosition.m_fX, vPosition.m_fY,
                                vPosition.m_fZ, bLoadingGame, 0);
            pCreature->SetOrientation(Vector(
                pRes->ReadFieldFLOAT(&cStruct, "XOrientation", bSuccess, 0.0f),
                pRes->ReadFieldFLOAT(&cStruct, "YOrientation", bSuccess, 0.0f),
                pRes->ReadFieldFLOAT(&cStruct, "ZOrientation", bSuccess, 0.0f)));
        }
    }
    return 1;
}
// PC 0x004DFE00 GOG 0x004DFF20
int CSWSArea::LoadItems(CResGFF* pRes, CResStruct* pStruct,
                        int bLoadingGame, int bUseTemplates)
{
    CResList cList;
    CResStruct cStruct;
    int bObjectIdRead;
    int bSuccess;

    if (pRes->GetList(&cList, pStruct, "List") != 0)
    {
        for (unsigned long i = 0; i < pRes->GetListCount(&cList); i++)
        {
            if (pRes->GetListElement(&cStruct, &cList, i) != 0 &&
                pRes->GetElementType(&cStruct) == GFF_STRUCT_ITEM)
            {
                unsigned long oidItem = pRes->ReadFieldDWORD(
                    &cStruct, "ObjectId", bObjectIdRead, OBJECT_INVALID);
                CSWSItem* pItem = new CSWSItem(oidItem);

                if (pItem == NULL)
                    continue;

                if (bUseTemplates != 0)
                {
                    CResRef cDefault("");
                    CResRef cTemplate = pRes->ReadFieldCResRef(
                        &cStruct, "TemplateResRef", bSuccess, cDefault);

                    if (bSuccess == 0 || pItem->LoadFromTemplate(cTemplate) == 0)
                    {
                        delete pItem;
                        continue;
                    }
                }
                else if (pItem->LoadItem(pRes, &cStruct) == 0)
                {
                    delete pItem;
                    continue;
                }
                {
                    Vector vOrientation;

                    if (bLoadingGame != 0)
                        pItem->LoadObjectState(pRes, &cStruct);

                    pItem->AddToArea(
                        this,
                        pRes->ReadFieldFLOAT(&cStruct, "XPosition", bSuccess, 0.0f),
                        pRes->ReadFieldFLOAT(&cStruct, "YPosition", bSuccess, 0.0f),
                        pRes->ReadFieldFLOAT(&cStruct, "ZPosition", bSuccess, 0.0f),
                        bLoadingGame == 0);
                    vOrientation.m_fX = pRes->ReadFieldFLOAT(&cStruct, "XOrientation", bSuccess, 0.0f);
                    vOrientation.m_fY = pRes->ReadFieldFLOAT(&cStruct, "YOrientation", bSuccess, 0.0f);
                    vOrientation.m_fZ = pRes->ReadFieldFLOAT(&cStruct, "ZOrientation", bSuccess, 0.0f);
                    pItem->SetOrientation(vOrientation);
                }
            }
        }
    }
    return 1;
}
// PC 0x004E0080 GOG 0x004E01A0
int CSWSArea::LoadEncounters(CResGFF* pRes, CResStruct* pStruct,
                             int bLoadingGame, int bUseTemplates)
{
    CResList cList;
    CResList cSubList;
    CResStruct cStruct;
    int bSuccess;

    if (pRes->GetList(&cList, pStruct, "Encounter List") != 0)
    {
        for (unsigned long i = 0; i < pRes->GetListCount(&cList); i++)
        {
            if (pRes->GetListElement(&cStruct, &cList, i) != 0 &&
                pRes->GetElementType(&cStruct) == 7)
            {
                unsigned long oidEncounter = pRes->ReadFieldDWORD(
                    &cStruct, "ObjectId", bSuccess, OBJECT_INVALID);
                CSWSEncounter* pEncounter = new CSWSEncounter(oidEncounter);

                if (pEncounter != NULL)
                {
                    if (bUseTemplates != 0)
                    {
                        CResRef cDefault("");
                        CResRef cResRef = pRes->ReadFieldCResRef(
                            &cStruct, "TemplateResRef", bSuccess, cDefault);

                        if (bSuccess == 0 ||
                            pEncounter->LoadFromTemplate(cResRef, 0, NULL) == 0)
                        {
                            delete pEncounter;
                            continue;
                        }

                        float fX = pRes->ReadFieldFLOAT(&cStruct, "XPosition",
                                                        bSuccess, 0.0f);
                        float fY = pRes->ReadFieldFLOAT(&cStruct, "YPosition",
                                                        bSuccess, 0.0f);
                        Vector vPosition(fX, fY,
                                         pRes->ReadFieldFLOAT(&cStruct,
                                                              "ZPosition",
                                                              bSuccess, 0.0f));

                        pEncounter->SetPosition(vPosition, 1, 1, 0);
                        if (pRes->GetList(&cSubList, &cStruct, "Geometry") !=
                                0 &&
                            pEncounter->LoadEncounterGeometry(pRes, cSubList) ==
                                0)
                        {
                            delete pEncounter;
                            pEncounter = NULL;
                        }
                        if (pRes->GetList(&cSubList, &cStruct,
                                          "SpawnPointList") != 0)
                        {
                            pEncounter->LoadEncounterSpawnPoints(pRes,
                                                                 cSubList);
                        }
                    }
                    else if (pEncounter->LoadEncounter(pRes, &cStruct) == 0)
                    {
                        delete pEncounter;
                        continue;
                    }
                    if (pEncounter != NULL)
                    {
                        if (bLoadingGame != 0)
                        {
                            pEncounter->LoadObjectState(pRes, &cStruct);
                        }
                        pEncounter->AddToArea(
                            this, pEncounter->m_vPosition.m_fX,
                            pEncounter->m_vPosition.m_fY,
                            pEncounter->m_vPosition.m_fZ, bLoadingGame == 0);
                    }
                }
            }
        }
    }
    return 1;
}
// PC 0x004E0380 GOG 0x004E04A0
int CSWSArea::LoadWaypoints(CResGFF* pRes, CResStruct* pStruct,
                            int bLoadingGame, int bUseTemplates)
{
    CResList cList;
    CResStruct cStruct;
    int bSuccess;

    if (pRes->GetList(&cList, pStruct, "WaypointList") != 0)
    {
        for (unsigned long i = 0; i < pRes->GetListCount(&cList); i++)
        {
            if (pRes->GetListElement(&cStruct, &cList, i) != 0 &&
                pRes->GetElementType(&cStruct) == 5)
            {
                unsigned long oidWaypoint = pRes->ReadFieldDWORD(
                    &cStruct, "ObjectId", bSuccess, OBJECT_INVALID);
                CSWSWaypoint* pWaypoint = new CSWSWaypoint(oidWaypoint);

                if (pWaypoint != NULL)
                {
                    if (pWaypoint->LoadWaypoint(pRes, &cStruct) == 0)
                    {
                        delete pWaypoint;
                        continue;
                    }
                    if (bLoadingGame != 0)
                    {
                        pWaypoint->LoadObjectState(pRes, &cStruct);
                    }
                    pWaypoint->AddToArea(
                        this,
                        pRes->ReadFieldFLOAT(&cStruct, "XPosition", bSuccess,
                                             0.0f),
                        pRes->ReadFieldFLOAT(&cStruct, "YPosition", bSuccess,
                                             0.0f),
                        pRes->ReadFieldFLOAT(&cStruct, "ZPosition", bSuccess,
                                             0.0f),
                        bLoadingGame == 0);

                    Vector vPosition = pWaypoint->m_vPosition;

                    vPosition.m_fZ = ComputeHeight(vPosition, 1, NULL);
                    pWaypoint->SetPosition(vPosition, 1, 1, 0);
                }
            }
        }
    }
    return 1;
}
// PC 0x004E0580 GOG 0x004E06A0
int CSWSArea::LoadSounds(CResGFF* pRes, CResStruct* pStruct,
                         int bLoadingGame, int bUseTemplates)
{
    CResList cList;
    CResStruct cStruct;
    int bSuccess;

    if (pRes->GetList(&cList, pStruct, "SoundList") != 0)
    {
        unsigned long nSounds = pRes->GetListCount(&cList);
        for (unsigned long i = 0; i < nSounds; i++)
        {
            if (pRes->GetListElement(&cStruct, &cList, i) != 0 &&
                pRes->GetElementType(&cStruct) == GFF_STRUCT_SOUND)
            {
                unsigned long oidSound = pRes->ReadFieldDWORD(
                    &cStruct, "ObjectId", bSuccess, OBJECT_INVALID);
                CSWSSoundObject* pSound = new CSWSSoundObject(oidSound);

                if (bUseTemplates != 0)
                {
                    CResRef cDefault("");
                    CResRef cTemplate = pRes->ReadFieldCResRef(
                        &cStruct, "TemplateResRef", bSuccess, cDefault);

                    if (bSuccess != 0 && pSound->LoadFromTemplate(cTemplate) != 0)
                    {
                        pSound->m_nGeneratedType = pRes->ReadFieldDWORD(
                            &cStruct, "GeneratedType", bSuccess, 0);
                        float fZ = pRes->ReadFieldFLOAT(&cStruct, "ZPosition", bSuccess, 0.0f);
                        float fY = pRes->ReadFieldFLOAT(&cStruct, "YPosition", bSuccess, 0.0f);
                        Vector vPosition(
                            pRes->ReadFieldFLOAT(&cStruct, "XPosition", bSuccess, 0.0f), fY, fZ);

                        pSound->SetPosition(vPosition, 1, 1, 0);
                        pSound->AddToArea(this, bLoadingGame == 0);
                    }
                    else
                        delete pSound;
                }
                else
                {
                    if (pSound->Load(pRes, &cStruct) != 0)
                        pSound->AddToArea(this, bLoadingGame == 0);
                    else
                        delete pSound;
                }
            }
        }
    }
    return 1;
}
// PC 0x004E07C0 GOG 0x004E08E0
int CSWSArea::LoadStores(CResGFF* pRes, CResStruct* pStruct, int bLoadingGame,
                         int bUseTemplates)
{
    CResList cList;
    CResStruct cStruct;
    int bSuccess;

    if (pRes->GetList(&cList, pStruct, "StoreList") != 0)
    {
        for (unsigned long i = 0; i < pRes->GetListCount(&cList); i++)
        {
            if (pRes->GetListElement(&cStruct, &cList, i) == 0 ||
                pRes->GetElementType(&cStruct) != 11)
            {
                continue;
            }

            unsigned long oidStore = pRes->ReadFieldDWORD(&cStruct, "ObjectId",
                                                          bSuccess, OBJECT_INVALID);
            CSWSStore* pStore = new CSWSStore(oidStore);

            if (pStore == NULL)
            {
                continue;
            }
            if (bUseTemplates != 0)
            {
                CResRef cDefault("");
                CResRef cResRef = pRes->ReadFieldCResRef(&cStruct, "ResRef",
                                                         bSuccess, cDefault);

                if (bSuccess == 0 || pStore->LoadFromTemplate(cResRef, 1) == 0)
                {
                    delete pStore;
                    continue;
                }
            }
            else if (pStore->LoadStore(pRes, &cStruct, 0) == 0)
            {
                delete pStore;
                continue;
            }

            float fZ =
                pRes->ReadFieldFLOAT(&cStruct, "ZOrientation", bSuccess, 0.0f);
            float fY =
                pRes->ReadFieldFLOAT(&cStruct, "YOrientation", bSuccess, 0.0f);
            Vector vOrientation(
                pRes->ReadFieldFLOAT(&cStruct, "XOrientation", bSuccess, 0.0f),
                fY, fZ);

            if (vOrientation.m_fX * vOrientation.m_fX + fY * fY + fZ * fZ >
                0.00001f)
            {
                vOrientation = normalize(vOrientation);
            }
            else
            {
                vOrientation = Vector(0.0f, 1.0f, 0.0f);
            }
            pStore->SetOrientation(vOrientation);
            if (bLoadingGame != 0)
            {
                pStore->LoadObjectState(pRes, &cStruct);
            }
            pStore->AddToArea(
                this,
                pRes->ReadFieldFLOAT(&cStruct, "XPosition", bSuccess, 0.0f),
                pRes->ReadFieldFLOAT(&cStruct, "YPosition", bSuccess, 0.0f),
                pRes->ReadFieldFLOAT(&cStruct, "ZPosition", bSuccess, 0.0f),
                bLoadingGame == 0);
        }
    }
    return 1;
}
// PC 0x004E0B10 GOG 0x004E0C30
int CSWSArea::LoadAreaEffects(CResGFF* pRes, CResStruct* pStruct,
                              int bLoadingGame, int bUseTemplates)
{
    CResList cList;
    CResStruct cStruct;
    int bSuccess;

    if (pRes->GetList(&cList, pStruct, "AreaEffectList") != 0)
    {
        for (unsigned long i = 0; i < pRes->GetListCount(&cList); i++)
        {
            if (pRes->GetListElement(&cStruct, &cList, i) != 0 &&
                pRes->GetElementType(&cStruct) == 13)
            {
                unsigned long oidEffect = pRes->ReadFieldDWORD(
                    &cStruct, "ObjectId", bSuccess, OBJECT_INVALID);
                CSWSAreaOfEffectObject* pEffect =
                    new CSWSAreaOfEffectObject(oidEffect);

                if (pEffect != NULL)
                {
                    if (pEffect->LoadEffect(pRes, &cStruct) == 0)
                    {
                        delete pEffect;
                        continue;
                    }

                    float fZ = pRes->ReadFieldFLOAT(&cStruct, "OrientationZ",
                                                    bSuccess, 0.0f);
                    float fY = pRes->ReadFieldFLOAT(&cStruct, "OrientationY",
                                                    bSuccess, 0.0f);
                    Vector vOrientation(pRes->ReadFieldFLOAT(&cStruct,
                                                             "OrientationX",
                                                             bSuccess, 0.0f),
                                        fY, fZ);

                    if (vOrientation.m_fX * vOrientation.m_fX + fY * fY +
                            fZ * fZ >
                        0.0001f)
                    {
                        vOrientation = normalize(vOrientation);
                    }
                    else
                    {
                        vOrientation = Vector(0.0f, 1.0f, 0.0f);
                    }
                    ((CSWSObject*)pEffect)->SetOrientation(vOrientation);
                    if (bLoadingGame != 0)
                    {
                        ((CSWSObject*)pEffect)->LoadObjectState(pRes, &cStruct);
                    }
                    pEffect->AddToArea(
                        this,
                        pRes->ReadFieldFLOAT(&cStruct, "PositionX", bSuccess,
                                             0.0f),
                        pRes->ReadFieldFLOAT(&cStruct, "PositionY", bSuccess,
                                             0.0f),
                        pRes->ReadFieldFLOAT(&cStruct, "PositionZ", bSuccess,
                                             0.0f),
                        bLoadingGame == 0);
                }
            }
        }
    }
    return 1;
}
// PC 0x004E0DC0 GOG 0x004E0EE0
int CSWSArea::LoadMaps(CResGFF* pRes, CResStruct* pStruct)
{
    CResStruct cStruct;
    int bFieldRead;
    int bSuccess = 1;

    if (pRes->GetStructFromStruct(&cStruct, pStruct, "AreaMap") != 0)
    {
        int nWidth = pRes->ReadFieldINT(&cStruct, "AreaMapResX", bFieldRead, 0);
        int nHeight = pRes->ReadFieldINT(&cStruct, "AreaMapResY", bFieldRead, 0);
        unsigned long nDataSize = pRes->ReadFieldDWORD(
            &cStruct, "AreaMapDataSize", bFieldRead, 0);
        unsigned long* pData = new unsigned long[nDataSize];

        pRes->ReadFieldVOID(&cStruct, pData, nDataSize, "AreaMapData",
                            bFieldRead, NULL);
        if (bFieldRead != 0)
        {
            CSWSModule* pModule =
                g_pAppManager->m_pServerExoApp->GetModule();
            if (pModule->m_pAreaMap != NULL)
                pModule->m_pAreaMap->LoadSavedAreaMapData(pData, nDataSize / sizeof(*pData),
                                              nWidth, nHeight);
        }
        else
            bSuccess = 0;

        if (pData != NULL)
            delete[] pData;
    }
    else
        bSuccess = 0;
    return bSuccess;
}
// PC 0x004E0ED0 GOG 0x004E0FF0
int CSWSArea::LoadPlaceableCameras(CResGFF* pRes, CResStruct* pStruct)
{
    CResList cList;
    CResStruct cStruct;
    int bSuccess = 1;
    Vector vDefault;
    Quaternion qDefault;

    pRes->GetList(&cList, pStruct, "CameraList");
    int nCameras = pRes->GetListCount(&cList);

    if (nCameras > CGuiInGame::MAX_PLACEABLE_CAMERAS)
        return 0;

    g_pAppManager->m_pClientExoApp->GetInGameGui()->InitializePlaceableCameras(nCameras);
    for (int i = 0; i < nCameras; i++)
    {
        pRes->GetListElement(&cStruct, &cList, i);
        int nCameraId = pRes->ReadFieldINT(&cStruct, "CameraID", bSuccess, -1);
        Vector vPosition = pRes->ReadFieldVector(&cStruct, "Position", bSuccess, vDefault);
        Quaternion qOrientation = pRes->ReadFieldQuaternion(&cStruct, "Orientation", bSuccess, qDefault);
        float fPitch = pRes->ReadFieldFLOAT(&cStruct, "Pitch", bSuccess, 0.0f);
        float fHeight = pRes->ReadFieldFLOAT(&cStruct, "Height", bSuccess, 0.0f);
        float fFieldOfView = pRes->ReadFieldFLOAT(
            &cStruct, "FieldOfView", bSuccess, GFF_CAMERA_DEFAULT_FIELD_OF_VIEW);
        float fMicRange = pRes->ReadFieldFLOAT(&cStruct, "MicRange", bSuccess, 0.0f);

        CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
        pClient->GetInGameGui()->SetPlaceableCamera(
            i, nCameraId, vPosition, qOrientation, fPitch, fHeight, fFieldOfView, fMicRange);
    }
    return 1;
}
// PC 0x004E10B0 GOG 0x004E11D0
int CSWSArea::SaveProperties(CResGFF* pRes, CResStruct* pStruct)
{
    pRes->AddStructToStruct((CResStruct*)&pStruct, pStruct, "AreaProperties",
                            0x64);
    m_pAmbientSound->Save(pRes, (CResStruct*)&pStruct);
    pRes->WriteFieldBYTE((CResStruct*)&pStruct, m_bUnescapable, "Unescapable");
    pRes->WriteFieldBYTE((CResStruct*)&pStruct, m_bDisableTransit,
                         "DisableTransit");
    pRes->WriteFieldBYTE((CResStruct*)&pStruct, m_nPlayerRestrictMode,
                         "RestrictMode");
    pRes->WriteFieldDWORD((CResStruct*)&pStruct, m_nStealthXPMax,
                          "StealthXPMax");
    pRes->WriteFieldDWORD((CResStruct*)&pStruct, m_nStealthXPCurrent,
                          "StealthXPCurrent");
    pRes->WriteFieldDWORD((CResStruct*)&pStruct, m_nStealthXPLoss,
                          "StealthXPLoss");
    pRes->WriteFieldBYTE((CResStruct*)&pStruct, m_bStealthXPEnabled,
                         "StealthXPEnabled");
    pRes->WriteFieldBYTE((CResStruct*)&pStruct, m_bTransitionPending,
                         "TransPending");
    pRes->WriteFieldBYTE((CResStruct*)&pStruct, m_nNextTransitionPendingId,
                         "TransPendNextID");
    pRes->WriteFieldBYTE((CResStruct*)&pStruct, m_nCurrentTransitionPendingId,
                         "TransPendCurrID");
    return pRes->WriteFieldDWORD((CResStruct*)&pStruct, m_nSunFogColor,
                                 "SunFogColor");
}
// PC 0x004E1200 GOG 0x004E1320
void CSWSArea::SaveMaps(CResGFF* pRes, CResStruct* pStruct)
{
    CSWSModule* pModule = g_pAppManager->m_pServerExoApp->GetModule();

    if (pModule != NULL && pModule->m_pAreaMap != NULL &&
        pModule->m_pAreaMap->m_nExploredSize > 0)
    {
        CSWSAreaMap* pAreaMap = pModule->m_pAreaMap;
        unsigned long nSize = 0;
        unsigned long* pData;
        CResStruct cStruct;

        {
            int nWidth;
            int nHeight = 0;

            pData = pAreaMap->GetMapData(nSize, nWidth, nHeight);
            pRes->AddStructToStruct(&cStruct, pStruct, "AreaMap", 0x65);
            pRes->WriteFieldINT(&cStruct, nWidth, "AreaMapResX");
            pRes->WriteFieldINT(&cStruct, nHeight, "AreaMapResY");
        }
        unsigned long nDataSize = 4 * nSize;
        pRes->WriteFieldDWORD(&cStruct, nDataSize, "AreaMapDataSize");
        pRes->WriteFieldVOID(&cStruct, pData, nDataSize, "AreaMapData");
    }
}
// PC 0x004E55E0 GOG 0x004E56B0
int CSWSArea::LoadDoors(CResGFF* pRes, CResStruct* pStruct, int bLoadingGame,
                        int bUseTemplates)
{
    CResList cList;
    CResStruct cStruct;
    int bSuccess;

    if (pRes->GetList(&cList, pStruct, "Door List") != 0)
    {
        for (unsigned long i = 0; i < pRes->GetListCount(&cList); i++)
        {
            if (pRes->GetListElement(&cStruct, &cList, i) == 0 ||
                pRes->GetElementType(&cStruct) != GFF_STRUCT_DOOR)
            {
                continue;
            }

            unsigned long oidDoor = pRes->ReadFieldDWORD(
                &cStruct, "ObjectId", bSuccess, OBJECT_INVALID);
            CSWSDoor* pDoor = new CSWSDoor(oidDoor);
            if (pDoor == NULL)
            {
                continue;
            }
            if (pDoor->LoadDoorExternal(pRes, &cStruct, bUseTemplates) == 0)
            {
                delete pDoor;
                continue;
            }

            float fBearing =
                pRes->ReadFieldFLOAT(&cStruct, "Bearing", bSuccess, 0.0f);
            {
                Quaternion qOrientation(Vector(0.0f, 0.0f, 1.0f), fBearing);
                pDoor->m_fBearing = fBearing;
                pDoor->SetOrientation(qOrientation);
            }

            if (bLoadingGame != 0)
            {
                pDoor->LoadObjectState(pRes, &cStruct);
            }
            pDoor->AddToArea(
                this, pRes->ReadFieldFLOAT(&cStruct, "X", bSuccess, 0.0f),
                pRes->ReadFieldFLOAT(&cStruct, "Y", bSuccess, 0.0f),
                pRes->ReadFieldFLOAT(&cStruct, "Z", bSuccess, 0.0f),
                bLoadingGame == 0);

            if (pDoor->m_bTrapFlag != 0)
            {
                m_lstTrapObjectIds.Add(pDoor->GetId());
            }
        }
    }
    return 1;
}
// PC 0x004E9370 GOG 0x004E9440
int CSWSArea::LoadGIT(int bLoadingGame)
{
    CResStruct cTopLevelStruct;
    int bFieldRead;

    if (g_pExoResMan->Exists(m_cResRef, RESTYPE_GIT, (unsigned long*)NULL) == 0)
        return 0;

    CResGFF* pRes = new CResGFF(RESTYPE_GIT, "GIT ", m_cResRef);
    if (pRes->m_bLoaded == 0)
    {
        delete pRes;
        m_pRes->Release();
        return 0;
    }

    pRes->GetTopLevelStruct(&cTopLevelStruct);
    if (bLoadingGame != 0)
    {
        m_lScriptVars.LoadVarTable(pRes, &cTopLevelStruct);
        m_lVarTable.LoadVarTable(pRes, &cTopLevelStruct);
        m_nCurrentWeather = pRes->ReadFieldBYTE(
            &cTopLevelStruct, "CurrentWeather", bFieldRead, 0);
        m_bWeatherStarted = pRes->ReadFieldBYTE(
            &cTopLevelStruct, "WeatherStarted", bFieldRead, 0);

        if (m_nFlags & AREA_FLAG_SUPPRESS_WEATHER)
        {
            m_nCurrentWeather = WEATHER_AUTOMATIC;
            m_bWeatherStarted = 0;
        }
    }

    int bUseTemplates = pRes->ReadFieldBYTE(
        &cTopLevelStruct, "UseTemplates", bFieldRead, 0);
    LoadCreatures(pRes, &cTopLevelStruct, bLoadingGame, bUseTemplates);
    LoadItems(pRes, &cTopLevelStruct, bLoadingGame, bUseTemplates);
    LoadDoors(pRes, &cTopLevelStruct, bLoadingGame, bUseTemplates);
    LoadTriggers(pRes, &cTopLevelStruct, bLoadingGame, bUseTemplates);
    LoadEncounters(pRes, &cTopLevelStruct, bLoadingGame, bUseTemplates);
    LoadWaypoints(pRes, &cTopLevelStruct, bLoadingGame, bUseTemplates);
    LoadSounds(pRes, &cTopLevelStruct, bLoadingGame, bUseTemplates);
    LoadPlaceables(pRes, &cTopLevelStruct, bLoadingGame, bUseTemplates);
    LoadStores(pRes, &cTopLevelStruct, bLoadingGame, bUseTemplates);
    LoadAreaEffects(pRes, &cTopLevelStruct, bLoadingGame, bUseTemplates);
    LoadProperties(pRes, &cTopLevelStruct);
    LoadMaps(pRes, &cTopLevelStruct);
    LoadPlaceableCameras(pRes, &cTopLevelStruct);
    delete pRes;
    return 1;
}
// PC 0x004E5850 GOG 0x004E5920
int CSWSArea::LoadTriggers(CResGFF* pRes, CResStruct* pStruct,
                          int bLoadingGame, int bUseTemplates)
{
    CResList cList;
    CResStruct cStruct;
    int bObjectIdSuccess;

    if (pRes->GetList(&cList, pStruct, "TriggerList") != 0)
    {
        for (unsigned long i = 0; i < pRes->GetListCount(&cList); i++)
        {
            if (pRes->GetListElement(&cStruct, &cList, i) == 0 ||
                pRes->GetElementType(&cStruct) != GFF_STRUCT_TRIGGER)
                continue;

            unsigned long oidTrigger = pRes->ReadFieldDWORD(
                &cStruct, "ObjectId", bObjectIdSuccess, OBJECT_INVALID);
            CSWSTrigger* pTrigger = new CSWSTrigger(oidTrigger);
            if (pTrigger == NULL)
                continue;

            int bSuccess;
            float fX, fY, fZ;
            if (bUseTemplates != 0)
            {
                CResRef cDefault("");
                CResRef cTemplate = pRes->ReadFieldCResRef(
                    &cStruct, "TemplateResRef", bSuccess, cDefault);
                if (bSuccess == 0 || pTrigger->LoadFromTemplate(cTemplate) == 0)
                {
                    delete pTrigger;
                    continue;
                }

                CExoString sLinkedToModule;
                CResRef cLinkedDefault("");
                CResRef cLinkedToModule = pRes->ReadFieldCResRef(
                    &cStruct, "LinkedToModule", bSuccess, cLinkedDefault);
                cLinkedToModule.CopyToString(sLinkedToModule);
                pTrigger->m_lsTransitionDestination = pRes->ReadFieldCExoLocString(
                    &cStruct, "TransitionDestination", bSuccess,
                    pTrigger->m_lsTransitionDestination);
                {
                    CExoString sDefault("");
                    pTrigger->m_sLinkedTo = pRes->ReadFieldCExoString(
                        &cStruct, "LinkedTo", bSuccess, sDefault);
                }
                pTrigger->m_sLinkedToModule = sLinkedToModule;
                pTrigger->m_nLinkedToFlags = pRes->ReadFieldBYTE(
                    &cStruct, "LinkedToFlags", bSuccess, 0);

                fX = pRes->ReadFieldFLOAT(&cStruct, "XPosition", bSuccess, 0.0f);
                fY = pRes->ReadFieldFLOAT(&cStruct, "YPosition", bSuccess, 0.0f);
                fZ = pRes->ReadFieldFLOAT(&cStruct, "ZPosition", bSuccess, 0.0f);
                pTrigger->SetPosition(Vector(fX, fY, fZ), 1, 1, 0);
                CResList cGeometry;
                if (pRes->GetList(&cGeometry, &cStruct, "Geometry") != 0)
                    pTrigger->LoadTriggerGeometry(pRes, cGeometry);
            }
            else
            {
                if (pTrigger->LoadTrigger(pRes, &cStruct) == 0)
                {
                    delete pTrigger;
                    continue;
                }
                fX = pRes->ReadFieldFLOAT(&cStruct, "XPosition", bSuccess, 0.0f);
                fY = pRes->ReadFieldFLOAT(&cStruct, "YPosition", bSuccess, 0.0f);
                fZ = pRes->ReadFieldFLOAT(&cStruct, "ZPosition", bSuccess, 0.0f);
            }

            if (bLoadingGame != 0)
                pTrigger->LoadObjectState(pRes, &cStruct);
            pTrigger->AddToArea(this, fX, fY, fZ, bLoadingGame == 0);
            if (pTrigger->m_bMineTrigger != 0)
                m_lstTrapObjectIds.Add(pTrigger->GetId());
        }
    }
    return 1;
}
// PC 0x004E5CB0 GOG 0x004E5D80
int CSWSArea::LoadPlaceables(CResGFF* pRes, CResStruct* pStruct,
                             int bLoadingGame, int bUseTemplates)
{
    CResList cList;
    CResStruct cStruct;
    int bSuccess;

    if (pRes->GetList(&cList, pStruct, "Placeable List") != 0)
    {
        for (unsigned long i = 0; i < pRes->GetListCount(&cList); i++)
        {
            if (pRes->GetListElement(&cStruct, &cList, i) == 0 ||
                pRes->GetElementType(&cStruct) != GFF_STRUCT_PLACEABLE)
            {
                continue;
            }

            unsigned long oidPlaceable = pRes->ReadFieldDWORD(
                &cStruct, "ObjectId", bSuccess, OBJECT_INVALID);
            CSWSPlaceable* pPlaceable = new CSWSPlaceable(oidPlaceable);
            if (pPlaceable == NULL)
                continue;

            if (bUseTemplates != 0)
            {
                CResRef cDefault("");
                CResRef cTemplate = pRes->ReadFieldCResRef(
                    &cStruct, "TemplateResRef", bSuccess, cDefault);
                if (bSuccess == 0 || pPlaceable->LoadFromTemplate(cTemplate, 1) == 0)
                {
                    delete pPlaceable;
                    continue;
                }
                pPlaceable->m_bUseTweakColor = pRes->ReadFieldBYTE(
                    &cStruct, "UseTweakColor", bSuccess, 0);
                pPlaceable->m_nTweakColor = pRes->ReadFieldDWORD(
                    &cStruct, "TweakColor", bSuccess, 0);
            }
            else if (pPlaceable->LoadPlaceable(pRes, &cStruct, 0) == 0)
            {
                delete pPlaceable;
                continue;
            }

            float fBearing = pRes->ReadFieldFLOAT(
                &cStruct, "Bearing", bSuccess, 0.0f);
            pPlaceable->m_fBearing = fBearing;
            Quaternion qOrientation(Vector(0.0f, 0.0f, 1.0f), fBearing);
            pPlaceable->SetOrientation(qOrientation);

            if (bLoadingGame != 0)
                pPlaceable->LoadObjectState(pRes, &cStruct);

            pPlaceable->AddToArea(
                this,
                Vector(pRes->ReadFieldFLOAT(&cStruct, "X", bSuccess, 0.0f),
                       pRes->ReadFieldFLOAT(&cStruct, "Y", bSuccess, 0.0f),
                       pRes->ReadFieldFLOAT(&cStruct, "Z", bSuccess, 0.0f)),
                bLoadingGame);

            if (pPlaceable->m_nTrapFlag != 0)
                m_lstTrapObjectIds.Add(pPlaceable->GetId());
        }
    }
    return 1;
}
// PC 0x004E5FF0 GOG 0x004E60C0
int CSWSArea::PackAreaIntoMessage(int nPlayerId, int nAreaId, int nFlags,
                                 CSWSPlayer* pPlayer)
{
    CSWSMessage* pMessage = g_pAppManager->GetServerExoApp()->GetSWSMessage();
    CExoString sMessage;
    CGameObjectArray* pObjectArray =
        g_pAppManager->GetServerExoApp()->GetObjectArray();
    CExoArrayList<unsigned long> lstWaypoints;
    CExoArrayList<unsigned long> lstObjects;
    CExoString sVariable;
    CExoString sMapNote;
    CExoArrayList<int> lstMapPins;
    CExoArrayList<unsigned long> lstSounds;
    CGameObject* pObject;
    int i;

    if (pMessage == NULL)
        return 0;

    pMessage->WriteOBJECTIDServer(m_nId);
    pMessage->WriteCResRef(m_cResRef, 16);
    pMessage->WriteCExoLocStringServer(m_lsName, 0);
    pMessage->WriteINT(nPlayerId, 32);
    pMessage->WriteINT(nAreaId, 32);
    pMessage->WriteINT(nFlags, 32);
    pMessage->WriteDWORD(m_nFlags, 32);
    pMessage->WriteINT(m_nTurnRateRow, 32);
    pMessage->WriteCResRef(m_cDefaultEnvMap, 16);
    pMessage->WriteBYTE(m_bMiniGame, 8);
    pMessage->WriteDWORD(m_nMoonAmbientColor, 32);
    pMessage->WriteDWORD(m_nMoonDiffuseColor, 32);
    pMessage->WriteBYTE(m_bMoonFogOn, 8);
    pMessage->WriteDWORD(m_nMoonFogColor, 32);
    pMessage->WriteFLOAT(m_fMoonFogNear, 1.0f, 32);
    pMessage->WriteFLOAT(m_fMoonFogFar, 1.0f, 32);
    pMessage->WriteBOOL(m_bMoonShadows);
    pMessage->WriteDWORD(m_nSunAmbientColor, 32);
    pMessage->WriteDWORD(m_nSunDiffuseColor, 32);
    pMessage->WriteBYTE(m_bSunFogOn, 8);
    pMessage->WriteDWORD(m_nSunFogColor, 32);
    pMessage->WriteFLOAT(m_fSunFogNear, 1.0f, 32);
    pMessage->WriteFLOAT(m_fSunFogFar, 1.0f, 32);
    pMessage->WriteBOOL(m_bSunShadows);
    pMessage->WriteBYTE(m_bDayNightCycle, 8);
    pMessage->WriteBYTE(m_bIsNight, 8);
    pMessage->WriteDWORD(m_nDynAmbientColor, 32);
    pMessage->WriteBYTE(m_bNoRest, 8);
    pMessage->WriteBYTE(m_nShadowOpacity, 8);
    m_pAmbientSound->PackIntoMessage(pMessage);

    unsigned long nYear, nMonth, nDay, nHour, nTime;
    unsigned char nDawnDusk;
    g_pAppManager->GetServerExoApp()->GetModule()->GetTime(
        &nYear, &nMonth, &nDay, &nHour, &nDawnDusk, &nTime);
    unsigned char nDawnHour =
        g_pAppManager->GetServerExoApp()->GetModule()->m_nDawnHour;
    unsigned char nDuskHour =
        g_pAppManager->GetServerExoApp()->GetModule()->m_nDuskHour;
    pMessage->WriteBYTE(nDawnDusk, 8);
    pMessage->WriteDWORD(nTime, 32);
    pMessage->WriteBYTE(nHour, 8);
    pMessage->WriteBYTE(nDay, 8);
    pMessage->WriteBYTE(nMonth, 8);
    pMessage->WriteDWORD(nYear, 32);
    pMessage->WriteBYTE(nDawnHour, 8);
    pMessage->WriteBYTE(nDuskHour, 8);
    pMessage->WriteCResRef(m_cGrassTexName, 16);
    pMessage->WriteFLOAT(m_fGrassDensity, 1.0f, 32);
    pMessage->WriteFLOAT(m_fGrassQuadSize, 1.0f, 32);
    pMessage->WriteDWORD(m_nGrassDiffuse, 32);
    pMessage->WriteDWORD(m_nGrassAmbient, 32);
    pMessage->WriteDWORD(m_nGrassEmissive, 32);
    for (i = 0; i < 4; i++)
        pMessage->WriteFLOAT(m_fGrassProbability[i], 1.0f, 32);
    pMessage->WriteBYTE(m_nChanceLightning, 8);
    pMessage->WriteBYTE(m_nWind, 8);
    if (m_nWeatherOverride == WEATHER_AUTOMATIC)
    {
        pMessage->WriteBYTE(m_nCurrentWeather, 8);
        pMessage->WriteBOOL(m_bWeatherStarted);
    }
    else
    {
        pMessage->WriteBYTE(m_nWeatherOverride, 8);
        pMessage->WriteBOOL(1);
    }
    pMessage->WriteBYTE(m_nChanceRain, 8);
    pMessage->WriteFLOAT(m_fAlphaTest, 1.0f, 32);
    pMessage->WriteINT(m_nRoomAudioCount, 32);
    for (i = 0; i < m_nRoomAudioCount; i++)
    {
        pMessage->WriteCExoString(m_psRoomAudio[i], 32);
        pMessage->WriteINT(m_pnRoomAudio[i], 32);
        pMessage->WriteFLOAT(m_pfRoomAudio[i], 1.0f, 32);
    }
    pMessage->WriteINT(m_nRoomAudioEntryCount, 32);
    for (i = 0; i < m_nRoomAudioEntryCount; i++)
    {
        pMessage->WriteCExoString(m_lstRoomAudioNames[i], 32);
        pMessage->WriteBOOL(m_lstRoomAudioLooping[i]);
        pMessage->WriteCExoString(m_lstRoomAudioModelParts[i], 32);
        pMessage->WriteCExoString(m_lstRoomAudioOmenEvents[i], 32);
        pMessage->WriteCResRef(m_lstRoomAudioSounds[i], 16);
    }
    pMessage->WriteDWORD(g_pAppManager->GetServerExoApp()->GetModule()->
                             GetAreaMap()->GetOrientation(), 32);
    pMessage->WriteINT(m_nWidth, 32);
    pMessage->WriteINT(m_nHeight, 32);

    Vector vPosition;
    int nWaypoints;
    i = 0;
    nWaypoints = 0;
    for (; i < m_lstObjectIds.num; i++)
    {
        if (pObjectArray->GetGameObject(m_lstObjectIds[i], &pObject) ==
            CGameObjectArray::SUCCESS)
        {
            CSWSWaypoint* pWaypoint = pObject->AsSWSWaypoint();
            if (pWaypoint != NULL)
            {
                if (pWaypoint->m_bHasMapNote)
                {
                    lstWaypoints.Add(m_lstObjectIds[i]);
                    nWaypoints++;
                }
            }
            else if (pObject->AsSWSSoundObject() != NULL)
                lstSounds.Add(m_lstObjectIds[i]);
        }
    }
    pMessage->WriteINT(nWaypoints, 32);
#ifdef _XBOX
    int nWaypointsWritten = 0;
#endif
    for (i = 0; i < nWaypoints; i++)
    {
        if (pObjectArray->GetGameObject(lstWaypoints[i], &pObject) ==
            CGameObjectArray::SUCCESS)
        {
            CSWSWaypoint* pWaypoint = pObject->AsSWSWaypoint();
            if (pWaypoint != NULL)
            {
                pMessage->WriteOBJECTIDServer(pWaypoint->GetId());
                vPosition = pWaypoint->m_vPosition;
                pMessage->WriteFLOAT(vPosition.m_fX, 1.0f, 32);
                pMessage->WriteFLOAT(vPosition.m_fY, 1.0f, 32);
                pMessage->WriteFLOAT(vPosition.m_fZ, 1.0f, 32);
                pMessage->WriteBOOL(pWaypoint->GetMapNoteEnabled());
                pMessage->WriteCExoLocStringServer(pWaypoint->m_lsMapNote, 0);
#ifdef _XBOX
                nWaypointsWritten++;
#endif
            }
        }
    }
    EXOXBOXASSERT(nWaypointsWritten == nWaypoints);

    CSWSScriptVarTable* pVars;
    CSWSCreature* pCreature;
    pObject = pPlayer->GetGameObject();
    if (pObject != NULL && (pCreature = pObject->AsSWSCreature()) != NULL)
    {
        pVars = &pCreature->m_lScriptVars;
        if (pVars != NULL)
        {
            CExoString sTotalMapPins("NW_TOTAL_MAP_PINS");
            unsigned long nMapPins = pVars->GetInt(sTotalMapPins);
            if (nMapPins > 0)
            {
                for (unsigned long nPin = 0; nPin <= nMapPins; nPin++)
                {
                    sVariable.Format("NW_MAP_PIN_NTRY_%i", nPin);
                    sMapNote = pVars->GetString(sVariable);
                    if (sMapNote != "")
                    {
                        sVariable.Format("NW_MAP_PIN_AREA_%i", nPin);
                        if (pVars->GetObject(sVariable) == m_nId)
                            lstMapPins.Add(nPin);
                    }
                }
            }
        }
    }
    pMessage->WriteINT(lstMapPins.num, 32);
    if (lstMapPins.num > 0)
    {
        for (i = 0; i < lstMapPins.num; i++)
        {
            pMessage->WriteINT(lstMapPins[i], 32);
            sVariable.Format("NW_MAP_PIN_NTRY_%i", lstMapPins[i]);
            sMapNote = pVars->GetString(sVariable);
            pMessage->WriteCExoString(sMapNote, 32);
            sVariable.Format("NW_MAP_PIN_XPOS_%i", lstMapPins[i]);
            Vector vMapPinPosition;
            vMapPinPosition.m_fX = pVars->GetFloat(sVariable);
            pMessage->WriteFLOAT(vMapPinPosition.m_fX, 1.0f, 32);
            sVariable.Format("NW_MAP_PIN_YPOS_%i", lstMapPins[i]);
            vMapPinPosition.m_fY = pVars->GetFloat(sVariable);
            pMessage->WriteFLOAT(vMapPinPosition.m_fY, 1.0f, 32);
            sVariable.Format("NW_MAP_PIN_ZPOS_%i", lstMapPins[i]);
            vMapPinPosition.m_fZ = pVars->GetFloat(sVariable);
            pMessage->WriteFLOAT(vMapPinPosition.m_fZ, 1.0f, 32);
        }
    }

    unsigned short nSounds = lstSounds.num;
    pMessage->WriteWORD(nSounds, 16);
    for (unsigned short nSound = 0; nSound < nSounds; nSound++)
    {
        if (pObjectArray->GetGameObject(lstSounds[nSound], &pObject) ==
            CGameObjectArray::SUCCESS)
        {
            CSWSSoundObject* pSound = pObject->AsSWSSoundObject();
            if (pSound != NULL)
                pSound->PackIntoMessage(pMessage);
        }
    }
    unsigned short nLights = 0;
    unsigned short nStaticPlaceables = 0;
    for (i = 0; i < m_lstObjectIds.num; i++)
    {
        CSWSPlaceable* pPlaceable = g_pAppManager->GetServerExoApp()->
            GetPlaceableByGameObjectID(m_lstObjectIds[i]);
        if (pPlaceable != NULL)
        {
            if (pPlaceable->GetLightIsOn())
                nLights++;
            if (pPlaceable->m_bStatic == 1)
                nStaticPlaceables++;
        }
    }
    pMessage->WriteWORD(nLights, 16);
    for (i = 0; i < m_lstObjectIds.num; i++)
    {
        CSWSPlaceable* pPlaceable = g_pAppManager->GetServerExoApp()->
            GetPlaceableByGameObjectID(m_lstObjectIds[i]);
        if (pPlaceable != NULL && pPlaceable->GetLightIsOn())
        {
            vPosition = pPlaceable->m_vPosition;
            pMessage->WriteOBJECTIDServer(pPlaceable->GetId());
            unsigned long nAppearance = pPlaceable->m_nAppearance;
            pMessage->WriteDWORD(nAppearance, 32);
            pMessage->WriteFLOAT(vPosition.m_fX, 1.0f, 32);
            pMessage->WriteFLOAT(vPosition.m_fY, 1.0f, 32);
            pMessage->WriteFLOAT(vPosition.m_fZ, 1.0f, 32);
        }
    }
    pMessage->WriteWORD(nStaticPlaceables, 16);
    for (i = 0; i < m_lstObjectIds.num; i++)
    {
        CSWSPlaceable* pPlaceable = g_pAppManager->GetServerExoApp()->
            GetPlaceableByGameObjectID(m_lstObjectIds[i]);
        if (pPlaceable != NULL && pPlaceable->m_bStatic == 1)
        {
            vPosition = pPlaceable->m_vPosition;
            Vector vOrientation = pPlaceable->m_vOrientation;
            pMessage->WriteOBJECTIDServer(pPlaceable->GetId());
            unsigned long nAppearance = pPlaceable->m_nAppearance;
            pMessage->WriteDWORD(nAppearance, 32);
            pMessage->WriteFLOAT(vPosition.m_fX, 1.0f, 32);
            pMessage->WriteFLOAT(vPosition.m_fY, 1.0f, 32);
            pMessage->WriteFLOAT(vPosition.m_fZ, 1.0f, 32);
            pMessage->WriteFLOAT(vOrientation.m_fX, 1.0f, 32);
            pMessage->WriteFLOAT(vOrientation.m_fY, 1.0f, 32);
            pMessage->WriteFLOAT(vOrientation.m_fZ, 1.0f, 32);
        }
    }
    return 1;
}
// PC 0x004E6F70 GOG 0x004E7040
void CSWSArea::SaveGIT(CERFFile* pERFFile, const CExoString& sName,
                       CExoArrayList<unsigned long>& lstPlayers)
{

    CExoString sFileName;
    int i = 0;
    CGameObjectArray* pObjectArray =
        g_pAppManager->m_pServerExoApp->GetObjectArray();
    CExoArrayList<unsigned long> lstCreatures;
    CExoArrayList<unsigned long> lstItems;
    CExoArrayList<unsigned long> lstDoors;
    CExoArrayList<unsigned long> lstTriggers;
    CExoArrayList<unsigned long> lstEncounters;
    CExoArrayList<unsigned long> lstWaypoints;
    CExoArrayList<unsigned long> lstSounds;
    CExoArrayList<unsigned long> lstPlaceables;
    CExoArrayList<unsigned long> lstStores;
    CExoArrayList<unsigned long> lstAreaEffects;
    CGameObject* pGameObject;

    CResGFF* pRes = new CResGFF;
#ifdef _XBOX
    CResStruct* pStruct = new CResStruct();
#else
    CResStruct* pStruct = new CResStruct;
#endif
    pRes->CreateGFFFile(pStruct, "GIT ", "V2.0");

    while (i < m_lstObjectIds.num)
    {
        if (pObjectArray->GetGameObject(m_lstObjectIds[i],
                                        &pGameObject) ==
            CGameObjectArray::SUCCESS)
        {
            if (pGameObject->AsSWSCreature() != NULL)
            {
                if (pGameObject->AsSWSCreature()->m_pStats->GetIsPC())
                    lstPlayers.Add(m_lstObjectIds[i]);
                else
                    lstCreatures.Add(m_lstObjectIds[i]);
            }
            else if (pGameObject->AsSWSItem() != NULL)
                lstItems.Add(m_lstObjectIds[i]);
            else if (pGameObject->AsSWSDoor() != NULL)
                lstDoors.Add(m_lstObjectIds[i]);
            else if (pGameObject->AsSWSTrigger() != NULL)
                lstTriggers.Add(m_lstObjectIds[i]);
            else if (pGameObject->AsSWSEncounter() != NULL)
                lstEncounters.Add(m_lstObjectIds[i]);
            else if (pGameObject->AsSWSWaypoint() != NULL)
                lstWaypoints.Add(m_lstObjectIds[i]);
            else if (pGameObject->AsSWSSoundObject() != NULL)
                lstSounds.Add(m_lstObjectIds[i]);
            else if (pGameObject->AsSWSPlaceable() != NULL)
                lstPlaceables.Add(m_lstObjectIds[i]);
            else if (pGameObject->AsSWSStore() != NULL)
                lstStores.Add(m_lstObjectIds[i]);
            else if (pGameObject->AsSWSAreaOfEffectObject() != NULL)
                lstAreaEffects.Add(m_lstObjectIds[i]);
        }
        i++;
    }

    m_lScriptVars.SaveVarTable(pRes, pStruct);
    m_lVarTable.SaveVarTable(pRes, pStruct);
    pRes->WriteFieldBYTE(pStruct, m_nCurrentWeather, "CurrentWeather");
    pRes->WriteFieldBYTE(pStruct, m_bWeatherStarted, "WeatherStarted");
    pRes->WriteFieldBYTE(pStruct, m_bTransitionPending, "TransPending");
    pRes->WriteFieldBYTE(pStruct, m_nNextTransitionPendingId,
                         "TransPendNextID");
    pRes->WriteFieldBYTE(pStruct, m_nCurrentTransitionPendingId,
                         "TransPendCurrID");
    SaveCreatures(pRes, pStruct, lstCreatures);
    SaveItems(pRes, pStruct, lstItems);
    SaveDoors(pRes, pStruct, lstDoors);
    SaveTriggers(pRes, pStruct, lstTriggers);
    SaveEncounters(pRes, pStruct, lstEncounters);
    SaveWaypoints(pRes, pStruct, lstWaypoints);
    SaveSounds(pRes, pStruct, lstSounds);
    SavePlaceables(pRes, pStruct, lstPlaceables);
    SaveStores(pRes, pStruct, lstStores);
    SaveAreaEffects(pRes, pStruct, lstAreaEffects);
    SaveProperties(pRes, pStruct);
    SaveMaps(pRes, pStruct);
    SavePlaceableCameras(pRes, pStruct);

    CExoString sResRef;
    m_cResRef.CopyToString(sResRef);
    pERFFile->WriteResource(sResRef.CStr(), RESTYPE_GIT, pRes, 1, (void*)-1);
    delete pRes;
    delete pStruct;
}
// PC 0x004E12D0 GOG 0x004E13F0
void CSWSArea::SavePlaceableCameras(CResGFF* pRes, CResStruct* pStruct)
{
    CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
    CGuiInGame* pGui = pClient->GetInGameGui();
    CResList cList;
    CResStruct cStruct;

    pRes->AddList(&cList, pStruct, "CameraList");
    for (int i = 0; i < pGui->m_nPlaceableCameras; i++)
    {
        CPlaceableCamera* pCamera = pGui->GetPlaceableCamera(i);
        if (pCamera != NULL)
        {
            pRes->AddListElement(&cStruct, &cList, GFF_STRUCT_PLACEABLE_CAMERA);
            pRes->WriteFieldINT(&cStruct, pCamera->m_nPlaceableID, "CameraID");
            pRes->WriteFieldVector(&cStruct, pCamera->m_vPosition, "Position");
            pRes->WriteFieldQuaternion(&cStruct, pCamera->m_qOrientation, "Orientation");
            pRes->WriteFieldFLOAT(&cStruct, pCamera->m_fPitch, "Pitch");
            pRes->WriteFieldFLOAT(&cStruct, pCamera->m_fHeight, "Height");
            pRes->WriteFieldFLOAT(&cStruct, pCamera->m_fFieldOfView, "FieldOfView");
            pRes->WriteFieldFLOAT(&cStruct, pCamera->m_fMicRange, "MicRange");
        }
    }
}
// PC 0x004E7650 GOG 0x004E7720
void CSWSArea::UnloadArea()
{
    delete[] m_p34;
    m_p34 = NULL;
    delete[] m_p40;
    m_p40 = NULL;
    delete[] m_p1C;
    m_p1C = NULL;
    delete[] m_p20;
    m_p20 = NULL;
    delete[] m_p24;
    m_p24 = NULL;
    delete[] m_p28;
    m_p28 = NULL;

    while (!m_lstExpansions.IsEmpty())
    {
        NWAreaExpansion_st* pExpansion = m_lstExpansions.RemoveHead();
        if (pExpansion != NULL)
        {
            delete pExpansion;
        }
    }

    delete m_pVersionInfo;
    m_pVersionInfo = NULL;

    CSWSModule* pModule = g_pAppManager->m_pServerExoApp->GetModule();
    if (pModule != NULL)
    {
        pModule->RemoveObjectFromLookupTable(m_sTag, m_nId);
    }

    delete[] m_pRooms;
    m_pRooms = NULL;
}
// PC 0x004E13F0 GOG 0x004E1510
int CSWSArea::GetSurfaceMaterial(Vector vPosition)
{
    CSWSRoom* pRoom = GetRoom(vPosition, NULL);

    if (pRoom != NULL)
    {
        CSWRoomSurfaceMesh* pMesh = pRoom->m_pWalkMesh;

        return pMesh->GetSurfaceMaterial(vPosition);
    }
    return 0;
}
// PC 0x004E1430 GOG 0x004E1550
int CSWSArea::PositionWalkable(Vector vPosition)
{
    CSWSRoom* pRoom = GetRoom(vPosition, NULL);

    if (pRoom != NULL)
    {
        CSWRoomSurfaceMesh* pMesh = pRoom->m_pWalkMesh;
        int nSurfaceMaterial =
            pMesh->GetSurfaceMaterialWalkCheckOnly(vPosition);
        int nWalk;

        {
            CExoString sColumn("Walk");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pTable = p2DArrays->m_pSurfaceMaterial;
            pTable->GetINTEntry(nSurfaceMaterial, sColumn, &nWalk);
        }
        return nWalk != 0;
    }
    return 0;
}
#ifndef _XBOX

int CSWSArea::TestPathPoint(float fPersonalSpace, int bVerbose)
{
    int bFailed = FALSE;

    for (int nPoint = 0; nPoint < m_nPathPointCount; nPoint++)
    {
        CSWSAreaPathPoint* pPoint = &m_pPathPoints[nPoint];
        if (pPoint->m_nConnectionCount == 0)
            bFailed = TRUE;

        for (int nConnection = pPoint->m_nFirstConnection;
             nConnection < pPoint->m_nFirstConnection + pPoint->m_nConnectionCount;
             nConnection++)
        {
            int nTarget = m_pPathConnections[nConnection];
            Vector vStart(pPoint->m_fX, pPoint->m_fY, 0.0f);
            Vector vEnd(m_pPathPoints[nTarget].m_fX,
                        m_pPathPoints[nTarget].m_fY, 0.0f);
            vStart.m_fZ = ComputeHeight(vStart, 1, NULL);
            vEnd.m_fZ = ComputeHeight(vEnd, 1, NULL);
            if (nPoint > nTarget)
            {
                vStart.m_fZ += AREA_PATH_POINT_TEST_HEIGHT;
                vEnd.m_fZ += AREA_PATH_POINT_TEST_HEIGHT;
            }

            int nResult = TestDirectLine(vStart, vEnd, fPersonalSpace, 0.0f,
                                         1, NULL);
            if (nResult != DIRECT_LINE_BLOCKED_CREATURE &&
                nResult != DIRECT_LINE_CLEAR)
            {
                bFailed = TRUE;
            }
        }
    }
    return bFailed;
}
#endif
int CSWSArea::GetDoorCrossed(const Vector& vStart, const Vector& vEnd)  // PC 0x004E1500 GOG 0x004E1620
{
    Vector vRaisedStart = vStart;
    Vector vRaisedEnd = vEnd;

    vRaisedStart.m_fZ += AREA_DOOR_CROSSING_HEIGHT;
    vRaisedEnd.m_fZ += AREA_DOOR_CROSSING_HEIGHT;

    for (int i = 0; i < m_nBoundingBoxCount; i++)
    {
        if (BoxIntersect(vRaisedStart, vRaisedEnd,
                         m_pBoundingBoxes[i * 2],
                         m_pBoundingBoxes[i * 2 + 1]))
        {
            unsigned long oidDoor = m_pBoundingBoxIds[i];
            CSWSDoor* pDoor =
                g_pAppManager->m_pServerExoApp->GetDoorByGameObjectID(oidDoor);

            if (pDoor != NULL)
            {
                Vector vDoorPosition = pDoor->m_vPosition;
                int bStartSide =
                    (vRaisedStart - vDoorPosition) * pDoor->m_vOrientation > 0.0f;
                int bEndSide =
                    (vRaisedEnd - vDoorPosition) * pDoor->m_vOrientation > 0.0f;

                if (bStartSide != bEndSide)
                    return oidDoor;
            }
        }
    }
    return OBJECT_INVALID;
}
// PC 0x004E25B0 GOG 0x004E26D0
int CSWSArea::LoadProperties(CResGFF* pRes, CResStruct* pStruct)
{
    CResStruct cProperties;
    int bSuccess;

    if (pRes->GetStructFromStruct(&cProperties, pStruct, "AreaProperties"))
    {
        int bUnescapable = m_bUnescapable;
        m_bUnescapable = pRes->ReadFieldBYTE(
            &cProperties, "Unescapable", bSuccess, bUnescapable);
        int nRestrictMode = m_nPlayerRestrictMode;
        SetPlayerRestrictMode(m_pRes->ReadFieldBYTE(
            pStruct, "RestrictMode", bSuccess, nRestrictMode));
        unsigned long nMaxXP = m_nStealthXPMax;
        nMaxXP = pRes->ReadFieldDWORD(
            &cProperties, "StealthXPMax", bSuccess, nMaxXP);
        if (m_nStealthXPCurrent > nMaxXP)
            m_nStealthXPCurrent = nMaxXP;
        m_nStealthXPMax = nMaxXP;
        unsigned long nCurrentXP = m_nStealthXPMax;
        nCurrentXP = pRes->ReadFieldDWORD(
            &cProperties, "StealthXPCurrent", bSuccess, nCurrentXP);
        if (nCurrentXP > m_nStealthXPMax)
            nCurrentXP = m_nStealthXPMax;
        m_nStealthXPCurrent = nCurrentXP;
        unsigned long nLossXP = m_nStealthXPLoss;
        m_nStealthXPLoss = pRes->ReadFieldDWORD(
            &cProperties, "StealthXPLoss", bSuccess, nLossXP);
        int bStealthXPEnabled = m_bStealthXPEnabled;
        m_bStealthXPEnabled = pRes->ReadFieldBYTE(
            &cProperties, "StealthXPEnabled", bSuccess, bStealthXPEnabled);
        m_bTransitionPending = pRes->ReadFieldBYTE(
            &cProperties, "TransPending", bSuccess, m_bTransitionPending);
        m_nNextTransitionPendingId = pRes->ReadFieldBYTE(
            &cProperties, "TransPendNextID", bSuccess, m_nNextTransitionPendingId);
        m_nCurrentTransitionPendingId = pRes->ReadFieldBYTE(
            &cProperties, "TransPendCurrID", bSuccess, m_nCurrentTransitionPendingId);
        m_nSunFogColor = pRes->ReadFieldDWORD(
            &cProperties, "SunFogColor", bSuccess, m_nSunFogColor);
        CSWSAmbientSound* pAmbientSound = m_pAmbientSound;
        return pAmbientSound->Load(pRes, &cProperties);
    }
    return 0;
}
// PC 0x004E27A0 GOG 0x004E28C0
void CSWSArea::SaveCreatures(CResGFF* pRes, CResStruct* pStruct,
                             CExoArrayList<unsigned long>& lstObjectIds)
{
    CGameObjectArray* pObjectArray =
        g_pAppManager->m_pServerExoApp->GetObjectArray();
    CSWPartyTable* pPartyTable =
        g_pAppManager->m_pServerExoApp->GetPartyTable();
    CResList cList;
    CResStruct cStruct;
    CGameObject* pGameObject;
    int i;

    pRes->AddList(&cList, pStruct, "Creature List");
    for (i = 0; i < lstObjectIds.num; i++)
    {
        if (pObjectArray->GetGameObject(lstObjectIds[i],
                                        &pGameObject) ==
            CGameObjectArray::SUCCESS)
        {
            CSWSCreature* pCreature =
                ((CSWSObject*)pGameObject)->AsSWSCreature();

            EXOXBOXASSERT(pCreature);
            if (pCreature->m_bInParty == 0 && pCreature->m_n11FC == 0)
            {
                pRes->AddListElement(&cStruct, &cList, 4);
                pRes->WriteFieldDWORD(&cStruct, pCreature->m_nId,
                                      "ObjectId");
                pCreature->SaveCreature(pRes, &cStruct);
            }
        }
    }
}
// PC 0x004E2880 GOG 0x004E29A0
void CSWSArea::SaveItems(CResGFF* pRes, CResStruct* pStruct,
                         CExoArrayList<unsigned long>& lstObjectIds)
{
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
    CGameObjectArray* pObjectArray = pServerExoApp->GetObjectArray();
    CResList cList;
    CResStruct cStruct;
    CGameObject* pGameObject;
    int i;

    pRes->AddList(&cList, pStruct, "List");
    for (i = 0; i < lstObjectIds.num; i++)
    {
        if (pObjectArray->GetGameObject(lstObjectIds[i],
                                        &pGameObject) ==
            CGameObjectArray::SUCCESS)
        {
            CSWSItem* pItem = pGameObject->AsSWSItem();

            EXOXBOXASSERT(pItem);
            pRes->AddListElement(&cStruct, &cList, GFF_STRUCT_ITEM);
            pRes->WriteFieldDWORD(&cStruct, pItem->m_nId, "ObjectId");
            pItem->SaveItem(pRes, &cStruct);
            pItem->SaveObjectState(pRes, &cStruct);
        }
    }
}
// PC 0x004E2940 GOG 0x004E2A60
void CSWSArea::SaveDoors(CResGFF* pRes, CResStruct* pStruct,
                         CExoArrayList<unsigned long>& lstObjectIds)
{
    CGameObjectArray* pObjectArray =
        g_pAppManager->m_pServerExoApp->GetObjectArray();
    CResList cList;
    CResStruct cStruct;
    CGameObject* pGameObject;
    int i;

    pRes->AddList(&cList, pStruct, "Door List");
    for (i = 0; i < lstObjectIds.num; i++)
    {
        if (pObjectArray->GetGameObject(lstObjectIds[i],
                                        &pGameObject) ==
            CGameObjectArray::SUCCESS)
        {
            CSWSDoor* pDoor = ((CSWSObject*)pGameObject)->AsSWSDoor();
            EXOXBOXASSERT(pDoor);

            pRes->AddListElement(&cStruct, &cList, 8);
            pRes->WriteFieldDWORD(&cStruct, pDoor->m_nId, "ObjectId");
            pDoor->SaveDoor(pRes, &cStruct);
            pDoor->SaveObjectState(pRes, &cStruct);
        }
    }
}
// PC 0x004E2A00 GOG 0x004E2B20
void CSWSArea::SaveTriggers(CResGFF* pRes, CResStruct* pStruct,
                            CExoArrayList<unsigned long>& lstObjectIds)
{
    CGameObjectArray* pObjectArray =
        g_pAppManager->m_pServerExoApp->GetObjectArray();
    CResList cList;
    CResStruct cStruct;
    CGameObject* pGameObject;
    int i;

    pRes->AddList(&cList, pStruct, "TriggerList");
    for (i = 0; i < lstObjectIds.num; i++)
    {
        if (pObjectArray->GetGameObject(lstObjectIds[i],
                                        &pGameObject) ==
            CGameObjectArray::SUCCESS)
        {
            CSWSTrigger* pTrigger = ((CSWSObject*)pGameObject)->AsSWSTrigger();
            EXOXBOXASSERT(pTrigger);

            pRes->AddListElement(&cStruct, &cList, 1);
            pRes->WriteFieldDWORD(&cStruct, pTrigger->m_nId, "ObjectId");
            pTrigger->SaveTrigger(pRes, &cStruct);
            pTrigger->SaveObjectState(pRes, &cStruct);
        }
    }
}
// PC 0x004E2AC0 GOG 0x004E2BE0
void CSWSArea::SaveEncounters(CResGFF* pRes, CResStruct* pStruct,
                              CExoArrayList<unsigned long>& lstObjectIds)
{
    CGameObjectArray* pObjectArray =
        g_pAppManager->m_pServerExoApp->GetObjectArray();
    CResList cList;
    CResStruct cStruct;
    CGameObject* pGameObject;
    int i;

    pRes->AddList(&cList, pStruct, "Encounter List");
    for (i = 0; i < lstObjectIds.num; i++)
    {
        if (pObjectArray->GetGameObject(lstObjectIds[i],
                                        &pGameObject) ==
            CGameObjectArray::SUCCESS)
        {
            CSWSEncounter* pEncounter =
                ((CSWSObject*)pGameObject)->AsSWSEncounter();

            EXOXBOXASSERT(pEncounter);
            pRes->AddListElement(&cStruct, &cList, CSWSModule::GFF_STRUCT_TOKEN);
            pRes->WriteFieldDWORD(&cStruct, pEncounter->m_nId, "ObjectId");
            pEncounter->SaveEncounter(pRes, &cStruct);
            pEncounter->SaveObjectState(pRes, &cStruct);
        }
    }
}
// PC 0x004E2B80 GOG 0x004E2CA0
void CSWSArea::SaveWaypoints(CResGFF* pRes, CResStruct* pStruct,
                             CExoArrayList<unsigned long>& lstObjectIds)
{
    CGameObjectArray* pObjectArray =
        g_pAppManager->m_pServerExoApp->GetObjectArray();
    CResList cList;
    CResStruct cStruct;
    CGameObject* pGameObject;
    int i;

    pRes->AddList(&cList, pStruct, "WaypointList");
    for (i = 0; i < lstObjectIds.num; i++)
    {
        if (pObjectArray->GetGameObject(lstObjectIds[i],
                                        &pGameObject) ==
            CGameObjectArray::SUCCESS)
        {
            CSWSWaypoint* pWaypoint =
                ((CSWSObject*)pGameObject)->AsSWSWaypoint();

            EXOXBOXASSERT(pWaypoint);
            pRes->AddListElement(&cStruct, &cList, 5);
            pRes->WriteFieldDWORD(&cStruct, pWaypoint->m_nId, "ObjectId");
            pWaypoint->SaveWaypoint(pRes, &cStruct);
            pWaypoint->SaveObjectState(pRes, &cStruct);
        }
    }
}
// PC 0x004E2C40 GOG 0x004E2D60
void CSWSArea::SaveSounds(CResGFF* pRes, CResStruct* pStruct,
                          CExoArrayList<unsigned long>& lstObjectIds)
{
    CGameObjectArray* pObjectArray =
        g_pAppManager->m_pServerExoApp->GetObjectArray();
    CResList cList;
    CResStruct cStruct;
    CGameObject* pGameObject;
    int i;

    pRes->AddList(&cList, pStruct, "SoundList");
    for (i = 0; i < lstObjectIds.num; i++)
    {
        if (pObjectArray->GetGameObject(lstObjectIds[i],
                                        &pGameObject) ==
            CGameObjectArray::SUCCESS)
        {
            CSWSSoundObject* pSound = ((CSWSObject*)pGameObject)->AsSWSSoundObject();
            EXOXBOXASSERT(pSound);

            pRes->AddListElement(&cStruct, &cList, 6);
            pRes->WriteFieldDWORD(&cStruct, pSound->m_nId, "ObjectId");
            pSound->Save(pRes, &cStruct);
            pSound->SaveObjectState(pRes, &cStruct);
        }
    }
}
// PC 0x004E2D00 GOG 0x004E2E20
void CSWSArea::SavePlaceables(CResGFF* pRes, CResStruct* pStruct,
                              CExoArrayList<unsigned long>& lstObjectIds)
{
    CGameObjectArray* pObjectArray =
        g_pAppManager->m_pServerExoApp->GetObjectArray();
    CResList cList;
    CResStruct cStruct;
    CGameObject* pGameObject;
    int i;

    pRes->AddList(&cList, pStruct, "Placeable List");
    for (i = 0; i < lstObjectIds.num; i++)
    {
        if (pObjectArray->GetGameObject(lstObjectIds[i],
                                        &pGameObject) ==
            CGameObjectArray::SUCCESS)
        {
            CSWSPlaceable* pPlaceable =
                ((CSWSObject*)pGameObject)->AsSWSPlaceable();

            EXOXBOXASSERT(pPlaceable);
            if (pPlaceable->m_bIsCorpse == 0)
            {
                pRes->AddListElement(&cStruct, &cList, GFF_STRUCT_PLACEABLE);
                pRes->WriteFieldDWORD(&cStruct, pPlaceable->m_nId,
                                      "ObjectId");
                pPlaceable->SavePlaceable(pRes, &cStruct);
                pPlaceable->SaveObjectState(pRes, &cStruct);
            }
        }
    }
}
// PC 0x004E2DD0 GOG 0x004E2EF0
void CSWSArea::SaveStores(CResGFF* pRes, CResStruct* pStruct,
                          CExoArrayList<unsigned long>& lstObjectIds)
{
    CGameObjectArray* pObjectArray =
        g_pAppManager->m_pServerExoApp->GetObjectArray();
    CResList cList;
    CResStruct cStruct;
    CGameObject* pGameObject;
    int i;

    pRes->AddList(&cList, pStruct, "StoreList");
    for (i = 0; i < lstObjectIds.num; i++)
    {
        if (pObjectArray->GetGameObject(lstObjectIds[i],
                                        &pGameObject) ==
            CGameObjectArray::SUCCESS)
        {
            CSWSStore* pStore = ((CSWSObject*)pGameObject)->AsSWSStore();
            EXOXBOXASSERT(pStore);

            pRes->AddListElement(&cStruct, &cList, 11);
            pRes->WriteFieldDWORD(&cStruct, pStore->m_nId, "ObjectId");
            pStore->SaveStore(pRes, &cStruct);
            pStore->SaveObjectState(pRes, &cStruct);
        }
    }
}
// PC 0x004E2E90 GOG 0x004E2FB0
void CSWSArea::SaveAreaEffects(CResGFF* pRes, CResStruct* pStruct,
                               CExoArrayList<unsigned long>& lstObjectIds)
{
    CGameObjectArray* pObjectArray =
        g_pAppManager->m_pServerExoApp->GetObjectArray();
    CResList cList;
    CResStruct cStruct;
    CGameObject* pGameObject;
    int i;

    pRes->AddList(&cList, pStruct, "AreaEffectList");
    for (i = 0; i < lstObjectIds.num; i++)
    {
        if (pObjectArray->GetGameObject(lstObjectIds[i],
                                        &pGameObject) ==
            CGameObjectArray::SUCCESS)
        {
            CSWSAreaOfEffectObject* pEffect =
                ((CSWSObject*)pGameObject)->AsSWSAreaOfEffectObject();

            EXOXBOXASSERT(pEffect);
            pRes->AddListElement(&cStruct, &cList, 13);
            pRes->WriteFieldDWORD(&cStruct, pEffect->m_nId, "ObjectId");
            pEffect->SaveEffect(pRes, &cStruct);
            pEffect->SaveObjectState(pRes, &cStruct);
        }
    }
}
// PC 0x004E2F50 GOG 0x004E3070
void CSWSArea::PlayVisualEffect(unsigned short nEffect, const Vector& vPosition)
{
    CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
    C2DA* pVisualEffects = p2DArrays->m_pVisualEffect;
    float fRadius;
    pVisualEffects->GetFLOATEntry(nEffect, "RADIUS", &fRadius);
    fRadius += MAX_OBJECT_VISIBLE_DISTANCE;

    float fMinX = vPosition.m_fX - fRadius;
    float fMaxX = fRadius + vPosition.m_fX;
    CGameObjectArray* pObjectArray =
        g_pAppManager->GetServerExoApp()->GetObjectArray();
    int nIndex;
    if (GetFirstObjectIndiceByX(&nIndex, fMinX) == 1)
    {
        for (; nIndex < m_lstObjectIds.num; nIndex++)
        {
            CGameObject* pGameObject;
            unsigned char nResult = pObjectArray->GetGameObject(
                m_lstObjectIds.element[nIndex], &pGameObject);
            if (nResult == CGameObjectArray::SUCCESS &&
                pGameObject != NULL &&
                pGameObject->m_nObjectType > CGameObject::OBJECT_TYPE_AREA)
            {
                Vector vObjectPosition =
                    ((CSWSObject*)pGameObject)->m_vPosition;
                if (vObjectPosition.m_fX > fMaxX)
                {
                    break;
                }

                CSWSPlayer* pPlayer =
                    g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(
                        pGameObject->GetId());
                if (pPlayer != NULL)
                {
                    Vector vDelta = vObjectPosition - vPosition;
                    if (vDelta * vDelta <= fRadius * fRadius)
                    {
                        g_pAppManager->GetServerExoApp()->GetSWSMessage()
                            ->SendServerToPlayerArea_VisualEffect(
                                pPlayer, nEffect, vPosition);
                    }
                }
            }
        }
    }
}
// PC 0x004E3140 GOG 0x004E3260
int CSWSArea::GetIsIDInExcludeList(unsigned long oidObject,
                                   CExoArrayList<CGameObject*>* pExcludeList)
{
    if (pExcludeList != NULL && pExcludeList->num > 0)
    {
        for (int i = 0; i < pExcludeList->num; i++)
        {
            if ((*pExcludeList)[i]->m_nId == oidObject)
            {
                return 1;
            }
        }
    }
    return 0;
}
// PC 0x004E3180 GOG 0x004E32A0
void CSWSArea::SetCurrentWeather(unsigned char nWeather, int bEnabled)
{
    if (nWeather == WEATHER_AUTOMATIC)
    {
        if (m_nActiveWeather == m_nCurrentWeather)
        {
            return;
        }
        if (m_nFlags & AREA_FLAG_SUPPRESS_WEATHER)
        {
            m_nCurrentWeather = WEATHER_AUTOMATIC;
            if (m_nActiveWeather == WEATHER_RAIN)
            {
                nWeather = WEATHER_RAIN;
                bEnabled = 0;
            }
            else if (m_nActiveWeather == WEATHER_SNOW)
            {
                nWeather = WEATHER_SNOW;
                bEnabled = 0;
            }
        }
        else if (m_nActiveWeather == WEATHER_RAIN)
        {
            if (m_nCurrentWeather == WEATHER_SNOW)
            {
                nWeather = WEATHER_SNOW;
                bEnabled = 1;
            }
            else
            {
                nWeather = WEATHER_RAIN;
                bEnabled = 0;
            }
        }
        else if (m_nActiveWeather == WEATHER_SNOW)
        {
            if (m_nCurrentWeather == WEATHER_RAIN)
            {
                nWeather = WEATHER_RAIN;
                bEnabled = 1;
            }
            else
            {
                nWeather = WEATHER_SNOW;
                bEnabled = 0;
            }
        }
    }

    CExoLinkedList<CSWSClient>* pPlayerList =
        g_pAppManager->GetServerExoApp()->GetPlayerList();
    if (pPlayerList != NULL)
    {
        CExoLinkedListNode* pNode = pPlayerList->GetHeadPos();
        CSWSPlayer* pPlayer = (CSWSPlayer*)pPlayerList->GetHead();
        while (pNode != NULL)
        {
            if (pPlayer != NULL)
            {
                CSWSObject* pObject = pPlayer->GetGameObject();
                if (pObject != NULL && pObject->GetArea() != NULL &&
                    pObject->GetArea()->GetId() == GetId())
                {
                    g_pAppManager->GetServerExoApp()->GetSWSMessage()
                        ->SendServerToPlayerArea_Weather(pPlayer, nWeather,
                                                        bEnabled, m_nChanceRain);
                }
            }
            pPlayer = (CSWSPlayer*)pPlayerList->GetNext(pNode);
        }
    }
    if (bEnabled)
    {
        m_nActiveWeather = nWeather;
    }
    else
    {
        m_nActiveWeather = WEATHER_CLEAR;
    }
}
// PC 0x004E32B0 GOG 0x004E33D0
void CSWSArea::BudgeCreatures(const Vector& vCentre, const Vector& vMin,
                             const Vector& vMax, unsigned long oidObject,
                             int bUseActionPoint)
{
    CExoArrayList<unsigned long>* pObjects = &m_lstObjectIds;
    int nIndex = 0;
    GetFirstObjectIndiceByX(&nIndex,
        vCentre.m_fX - vMin.m_fX - AREA_BUDGE_SEARCH_MARGIN);
    Vector vDestination;
    Vector vPosition;

    if (pObjects != NULL && pObjects->num > 0)
    {
        while (nIndex < pObjects->num)
        {
            CSWSCreature* pCreature = g_pAppManager->GetServerExoApp()
                ->GetCreatureByGameObjectID((*pObjects)[nIndex]);
            nIndex++;
            if (pCreature != NULL)
            {
                vPosition = pCreature->m_vPosition;
                if (vPosition.m_fX >= vMax.m_fX + AREA_BUDGE_SEARCH_MARGIN)
                    break;
                if (vMin.m_fY - AREA_BUDGE_SEARCH_MARGIN >= vPosition.m_fY ||
                    vMax.m_fY + AREA_BUDGE_SEARCH_MARGIN <= vPosition.m_fY)
                    continue;
                if (!TestSafeLocationPoint(vPosition,
                        pCreature->GetPathfindInformation()))
                {
                    if (bUseActionPoint)
                    {
                        CSWSDoor* pDoor = g_pAppManager->GetServerExoApp()
                            ->GetDoorByGameObjectID(oidObject);
                        if (pDoor != NULL)
                        {
                            vDestination = pDoor->GetNearestActionPoint(vPosition, 0);
                        }
                        else
                        {
                            CSWSPlaceable* pPlaceable =
                                g_pAppManager->GetServerExoApp()
                                    ->GetPlaceableByGameObjectID(oidObject);
                            if (pPlaceable != NULL)
                                vDestination = pPlaceable->GetNearestActionPoint(vPosition);
                        }
                    }
                    else
                    {
                        vDestination = vPosition + normalize(vPosition - vCentre);
                    }
                    if (ComputeSafeLocation(vDestination,
                            AREA_BUDGE_SAFE_LOCATION_RADIUS,
                            pCreature->GetPathfindInformation(), 0, &vDestination))
                    {
                        pCreature->SetPosition(vDestination, 1, 1, 0);
                    }
                }
            }
        }
    }
}
// PC 0x004E3530 GOG 0x004E3650
int CSWSArea::LoadPathPoints(CResStruct* pStruct)
{
    NwsareaTrace1();
    int bSuccess = 1;
    int bFieldRead;
    CResStruct cRoot;
    CResStruct cStruct;
    CResList cList;

    if (g_pExoResMan->Exists(m_cResRef, RESTYPE_PTH, (unsigned long*)NULL))
    {
        CResGFF* pRes = new CResGFF(RESTYPE_PTH, "PTH ", m_cResRef);

        if (!pRes->m_bLoaded)
        {
            delete pRes;
            return 0;
        }
        pRes->GetTopLevelStruct(&cRoot);

        if (pRes->GetList(&cList, &cRoot, "Path_Points"))
        {
            m_nPathPointCount = pRes->GetListCount(&cList);
            if (m_nPathPointCount == 0)
            {
                delete pRes;
                return 0;
            }
            m_pPathPoints = new CSWSAreaPathPoint[m_nPathPointCount];
            for (unsigned long i = 0; i < m_nPathPointCount; i++)
            {
                if (pRes->GetListElement(&cStruct, &cList, i) &&
                    pRes->GetElementType(&cStruct) == GFF_STRUCT_PATH_POINT)
                {
                    m_pPathPoints[i].m_fX = pRes->ReadFieldFLOAT(
                        &cStruct, "X", bFieldRead, 0.0f);
                    bSuccess &= bFieldRead;
                    m_pPathPoints[i].m_fY = pRes->ReadFieldFLOAT(
                        &cStruct, "Y", bFieldRead, 0.0f);
                    bSuccess &= bFieldRead;
                    m_pPathPoints[i].m_nConnectionCount = pRes->ReadFieldDWORD(
                        &cStruct, "Conections", bFieldRead, 0);
                    bSuccess &= bFieldRead;
                    m_pPathPoints[i].m_nFirstConnection = pRes->ReadFieldDWORD(
                        &cStruct, "First_Conection", bFieldRead, 0);
                    bSuccess &= bFieldRead;
                }
            }
        }
        if (pRes->GetList(&cList, &cRoot, "Path_Conections"))
        {
            m_nPathConnectionCount = pRes->GetListCount(&cList);
            EXOXBOXASSERT(m_nPathConnectionCount);
            m_pPathConnections = new int[m_nPathConnectionCount];
            for (unsigned long i = 0; i < m_nPathConnectionCount; i++)
            {
                if (pRes->GetListElement(&cStruct, &cList, i) &&
                    pRes->GetElementType(&cStruct) == GFF_STRUCT_PATH_CONNECTION)
                {
                    m_pPathConnections[i] = pRes->ReadFieldDWORD(
                        &cStruct, "Destination", bFieldRead, 0);
                    bSuccess &= bFieldRead;
                }
            }
        }
        delete pRes;
    }
    return bSuccess;
}
// PC 0x004E3800 GOG 0x004E3920
int CSWSArea::NoNonWalkPolys(const Vector& vStart, const Vector& vEnd,
                             float fDistance, float fWidth, float fHeight,
                             unsigned long oidObject,
                             CWalkHitInfo* pWalkHitInfo, int bCheckDoors)
{
    return NoNonWalkPolysDetailed(vStart, vEnd, fDistance, fWidth, fHeight,
                                  oidObject, (unsigned long&)bCheckDoors, 0, 0, 0, pWalkHitInfo,
                                  bCheckDoors);
}
// PC 0x004E1680 GOG 0x004E17A0
int CSWSArea::NoNonWalkPolysDetailed(
    const Vector& vStart, const Vector& vEnd, float fMinHeight, float fMaxHeight,
    float fRadius, unsigned long oidObject, unsigned long& oidHitObject,
    int bFlag, CExoArrayList<CGameObject*>* pExcludeList,
    int bCheckTransparentDoors, CWalkHitInfo* pWalkHitInfo, int bCheckDoors)
{
    int bClear = 1;
    if (m_nBoundingBoxCount > 0)
    {
        Vector vMinLine, vMaxLine;
        if (vStart.m_fX < vEnd.m_fX)
        {
            vMinLine.m_fX = vStart.m_fX;
            vMaxLine.m_fX = vEnd.m_fX;
        }
        else
        {
            vMinLine.m_fX = vEnd.m_fX;
            vMaxLine.m_fX = vStart.m_fX;
        }
        if (vStart.m_fY < vEnd.m_fY)
        {
            vMinLine.m_fY = vStart.m_fY;
            vMaxLine.m_fY = vEnd.m_fY;
        }
        else
        {
            vMinLine.m_fY = vEnd.m_fY;
            vMaxLine.m_fY = vStart.m_fY;
        }
        vMinLine.m_fX -= fRadius;
        vMinLine.m_fY -= fRadius;
        vMaxLine.m_fX += fRadius;
        vMaxLine.m_fY += fRadius;

        for (int i = 0; i < m_nBoundingBoxCount; i++)
        {
            if (bCheckDoors)
            {
                CGameObject* pObject = g_pAppManager->GetServerExoApp()->GetGameObject(
                    m_pBoundingBoxIds[i]);
                if (pObject != NULL)
                {
                    CSWSDoor* pDoor = pObject->AsSWSDoor();
                    if (pDoor == NULL || pDoor->m_nOpenState != CSWSDoor::OPEN_STATE_CLOSED)
                        continue;
                }
            }

            Vector vMax = m_pBoundingBoxes[i * 2 + 1];
            Vector vMin = m_pBoundingBoxes[i * 2];
            if ((vMinLine.m_fX < vMin.m_fX && vMaxLine.m_fX < vMin.m_fX) ||
                (vMaxLine.m_fX > vMax.m_fX && vMinLine.m_fX > vMax.m_fX) ||
                (vMinLine.m_fY < vMin.m_fY && vMaxLine.m_fY < vMin.m_fY) ||
                (vMaxLine.m_fY > vMax.m_fY && vMinLine.m_fY > vMax.m_fY) ||
                (fMinHeight < vMin.m_fZ && fMaxHeight < vMin.m_fZ) ||
                (fMaxHeight > vMax.m_fZ && fMinHeight > vMax.m_fZ))
                continue;
            if (!GetIsIDInExcludeList(m_pBoundingBoxIds[i], pExcludeList) &&
                !NoNoneWalkPolysInStaticObject(m_pBoundingBoxIds[i], vStart, vEnd,
                    fMinHeight, fMaxHeight, fRadius, oidObject, bFlag, pWalkHitInfo))
            {
                oidHitObject = m_pBoundingBoxIds[i];
                if (pWalkHitInfo != NULL)
                    pWalkHitInfo->m_oidBlocker = oidHitObject;
                if (bCheckTransparentDoors == 1)
                {
                    bClear &= HandleTransparentDoors(vStart, vEnd, fMinHeight,
                        fMaxHeight, fRadius, oidObject, oidHitObject, bFlag,
                        pExcludeList, 1);
                }
                else
                    bClear = 0;
            }
        }
    }
    return bClear;
}
// PC 0x004E1930 GOG 0x004E1A50
void CSWSArea::InitializeRoomAudioParams(int nRoomCount)
{
    if (m_psRoomAudio != NULL)
    {
        delete[] m_psRoomAudio;
        m_psRoomAudio = NULL;
    }

    if (m_pnRoomAudio != NULL)
    {
        delete[] m_pnRoomAudio;
        m_pnRoomAudio = NULL;
    }

    if (m_pfRoomAudio != NULL)
    {
        delete[] m_pfRoomAudio;
        m_pfRoomAudio = NULL;
    }

    if (m_pnRoomForceRating != NULL)
    {
        delete[] m_pnRoomForceRating;
        m_pnRoomForceRating = NULL;
    }

    if (m_pbRoomDisableWeather != NULL)
    {
        delete[] m_pbRoomDisableWeather;
        m_pbRoomDisableWeather = NULL;
    }

    if (nRoomCount != 0)
    {
        m_psRoomAudio = new CExoString[nRoomCount];
        m_pnRoomAudio = new int[nRoomCount];
        m_pfRoomAudio = new float[nRoomCount];
        m_pnRoomForceRating = new int[nRoomCount];
        m_pbRoomDisableWeather = new unsigned char[nRoomCount];
        m_pnRoomAudioIndex = new unsigned char[nRoomCount];
    }
}
// PC 0x004E1A90 GOG 0x004E1BB0
CSWCArea* CSWSArea::GetClientArea()
{
    if (m_pClientArea == NULL)
    {
        CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
        unsigned long oidServerArea = m_nId;

        m_pClientArea = pClientExoApp->GetAreaByGameObjectID(
            pClientExoApp->ServerToClientObjectId(oidServerArea));
        if (m_pClientArea != NULL)
        {
            m_pClientArea->AttachmentFromServerArea(this);
        }
    }
    return m_pClientArea;
}
// PC 0x004E1AE0 GOG 0x004E1C00
int CSWSArea::AttachmentFromClientArea(CSWCArea* pArea)
{
    m_pClientArea = pArea;
    return (int)pArea;
}
// PC 0x004E1AF0 GOG 0x004E1C10
void CSWSArea::DetachFromClientArea()
{
    m_pClientArea = 0;
}
// PC 0x004E1B00 GOG 0x004E1C20
int CSWSArea::SetMaxStealthXP(unsigned long nExperience)
{
    if (m_nStealthXPCurrent > nExperience)
        m_nStealthXPCurrent = nExperience;
    m_nStealthXPMax = nExperience;
    return nExperience;
}
// PC 0x004E1B20 GOG 0x004E1C40
int CSWSArea::SetCurrentStealthXP(unsigned long nExperience)
{
    unsigned long nMaximum = m_nStealthXPMax;
    if (nExperience > nMaximum)
        nExperience = nMaximum;
    m_nStealthXPCurrent = nExperience;
    return nMaximum;
}
// PC 0x004E1B40 GOG 0x004E1C60
unsigned char CSWSArea::GetNextTransitionPendingID()
{
    if (++m_nNextTransitionPendingId == 0)
    {
        m_nNextTransitionPendingId = 1;
    }
    return m_nNextTransitionPendingId;
}

#ifdef _XBOX

#define RGB(r, g, b) ((DWORD)(((BYTE)(r) | ((WORD)((BYTE)(g)) << 8)) | (((DWORD)(BYTE)(b)) << 16)))
#endif
// PC 0x004E1B60 GOG 0x004E1C80
void CSWSArea::SetFogColor(const Vector& vColor)
{
    m_nSunFogColor = RGB((unsigned char)(vColor.m_fX * 255.0f),
                         (unsigned char)(vColor.m_fY * 255.0f),
                         (unsigned char)(vColor.m_fZ * 255.0f));
}
// PC 0x004E1BB0 GOG 0x004E1CD0
void CSWSArea::SetTransitionPending(int bPending, unsigned char nId)
{
    m_bTransitionPending = bPending;
    m_nCurrentTransitionPendingId = nId;
    if (bPending != 0)
    {
        g_pAppManager->m_pClientExoApp->SetInputActive(0, 1);
        return;
    }
    else
    {
        g_pAppManager->m_pClientExoApp->SetInputActive(1, 0);
        return;
    }
}
// PC 0x004E1C10 GOG 0x004E1D30
void CSWSArea::SetPlayerRestrictMode(int nRestrictMode)
{
    if (nRestrictMode != 0 && m_nPlayerRestrictMode != nRestrictMode)
    {
        g_pAppManager->m_pServerExoApp->GetPartyTable()->UnstealthParty(1);
    }
    m_nPlayerRestrictMode = nRestrictMode;
}
// PC 0x004E1C50 GOG 0x004E1D70
void DirtyTextureController::Init()
{
    if (g_pAppManager->m_pServerExoApp != NULL &&
        g_pAppManager->m_pServerExoApp->GetModule() != NULL)
    {
        int nSize = g_pAppManager->m_pServerExoApp->GetModule()->GetArea()->GetDirtySize(m_nDirtyIndex);
        int nARGB = g_pAppManager->m_pServerExoApp->GetModule()->GetArea()->GetDirtyARGB(m_nDirtyIndex);
        int nFormula = g_pAppManager->m_pServerExoApp->GetModule()->GetArea()->GetDirtyFormula(m_nDirtyIndex);
        int nFunc = g_pAppManager->m_pServerExoApp->GetModule()->GetArea()->GetDirtyFunc(m_nDirtyIndex);
        if (nSize != 0)
        {
            Init(nARGB, nSize, nFormula, nFunc);
            return;
        }
    }
    Init(0, 16, 0, 0);
}
// PC 0x004E3840 GOG 0x004E3960
void CSWSArea::ClearObjects()
{
    CGameObjectArray* pObjectArray =
        g_pAppManager->m_pServerExoApp->GetObjectArray();
    CGameObject* pGameObject;

    for (int i = m_lstObjectIds.num - 1; i >= 0; i--)
    {
        pObjectArray->GetGameObject(m_lstObjectIds[i], &pGameObject);
        if (pGameObject != NULL)
        {
            delete pGameObject;
        }
    }
    m_lstObjectIds.num = 0;
}
// PC 0x004E38A0 GOG 0x004E39C0
void CSWSArea::AwardStealthXP(unsigned long oidObject)
{
    NWSAREA_TRACE();
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    CSWSCreature* pCreature = pServer->GetCreatureByGameObjectID(oidObject);

    if (pCreature != NULL && m_bStealthXPEnabled != 0 &&
        m_nStealthXPCurrent > 0)
    {
        if (g_pAppManager->m_pServerExoApp->GetPartyTable() != NULL)
        {
            int nExperience = m_nStealthXPCurrent;
            g_pAppManager->m_pServerExoApp->GetPartyTable()
                ->DistributeExperience(nExperience, 1);
        }
        if (g_pAppManager->m_pClientExoApp->GetInGameGui() != NULL)
        {
            g_pAppManager->m_pClientExoApp->GetInGameGui()->UpdateStatus(
                GUI_STATUS_STEALTH_XP, m_nStealthXPCurrent);
        }
        m_nStealthXPCurrent = 0;
        m_bStealthXPEnabled = 0;
    }
}
// PC 0x004E3950 GOG 0x004E3A70
int CSWSArea::EnforceNoPCHeadCloning(int nHead)
{
    int nObjects;
    for (int i = 0; i < (nObjects = m_lstObjectIds.num); i++)
    {
        unsigned long oidObject = m_lstObjectIds[i];
        CSWSCreature* pCreature = g_pAppManager->m_pServerExoApp
            ->GetCreatureByGameObjectID(oidObject);
        if (pCreature != NULL)
        {
            if (&pCreature->m_cAppearanceInfo != NULL)
            {
                int nNormalHead;
                if (pCreature->m_bPMIsDisguised)
                {
                    CExoString sColumn("NORMALHEAD");
                    unsigned short nAppearance = pCreature->m_nPMAppearance;
                    g_pRules->Get2DArrays()->GetAppearance()->GetINTEntry(
                        nAppearance, sColumn, &nNormalHead);
                }
                else
                {
                    CExoString sColumn("NORMALHEAD");
                    g_pRules->Get2DArrays()->GetAppearance()->GetINTEntry(
                        pCreature->m_cAppearanceInfo.m_nAppearance, sColumn, &nNormalHead);
                }
                if (nNormalHead == nHead)
                    pCreature->m_cAppearanceInfo.m_bUseBackupHead = 1;
            }
        }
    }
    return nObjects;
}
// PC 0x004E3A90 GOG 0x004E3BB0
unsigned char CSWSArea::GetNumMinesSetByPlayerParty()
{
    unsigned char nMines = 0;

    for (int i = 0; i < m_lstTrapObjectIds.num; i++)
    {
        CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
        CSWSTrigger* pTrigger = pServerExoApp->GetTriggerByGameObjectID(
            m_lstTrapObjectIds[i]);

        if (pTrigger != NULL && pTrigger->m_bMineTrigger != 0 &&
            pTrigger->m_bSetByPlayerParty != 0)
        {
            nMines++;
        }
    }
    return nMines;
}
// PC 0x004E3B00 GOG 0x004E3C20
int CSWSArea::GetPlayerCanSetMines()
{
    return (unsigned char)GetNumMinesSetByPlayerParty() < 15;
}
// PC 0x004E7750 GOG 0x004E7820
int CSWSArea::HandleTransparentDoors(
    const Vector& vStart, const Vector& vEnd, float fDistance, float fWidth,
    float fHeight, unsigned long oidObject, unsigned long& oidHitObject,
    int bFlag, CExoArrayList<CGameObject*>* pExcludeList, int bCheckObjects)
{
    if (oidHitObject != OBJECT_INVALID)
    {

        CSWSDoor* pDoor =
            g_pAppManager->GetServerExoApp()->GetDoorByGameObjectID(oidHitObject);
        if (pDoor != NULL)
        {

            int nBlockSight;
            if (pDoor->m_nAppearanceType == 0)
            {
                CExoString sColumn("BlockSight");
                unsigned char nType = (unsigned char)pDoor->m_nGenericType;
                C2DA* pTable = g_pRules->m_p2DArrays->m_pGenericDoors;
                pTable->GetINTEntry(nType, sColumn, &nBlockSight);
            }
            else
            {
                CExoString sColumn("BlockSight");
                unsigned char nType = (unsigned char)pDoor->m_nAppearanceType;
                CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
                C2DA* pTable = p2DArrays->m_pDoorTypes;
                pTable->GetINTEntry(nType, sColumn, &nBlockSight);
            }

            if (nBlockSight == 0 && pExcludeList != NULL)
            {
                pExcludeList->Add(pDoor->AsSWSObject());
                return NoNonWalkPolysDetailed(
                    vStart, vEnd, fDistance, fWidth, fHeight, oidObject, oidHitObject,
                    bFlag, pExcludeList, 0, NULL, 0);
            }
        }
    }
    return 0;
}
// PC 0x004E78A0 GOG 0x004E7970
int CSWSArea::ClearLineOfSight(
    const Vector& vStart, const Vector& vEnd, Vector* pvHit,
    unsigned long* poidHit, unsigned long oidObject, unsigned long oidTarget,
    int bCheckTransparentDoors, int bCheckAllRooms)
{
    Vector vRoomHit;
    unsigned long nFace = OBJECT_INVALID;
    float fNearestDistance = FLT_MAX;
    int bClear = 1;
    float fMinHeight, fMaxHeight;
    if (vStart.m_fZ < vEnd.m_fZ)
    {
        fMinHeight = vStart.m_fZ;
        fMaxHeight = vEnd.m_fZ;
    }
    else
    {
        fMinHeight = vEnd.m_fZ;
        fMaxHeight = vStart.m_fZ;
    }

    CExoArrayList<CGameObject*> lstExclude;
    CGameObject* pObject =
        g_pAppManager->GetServerExoApp()->GetGameObject(oidObject);
    if (pObject != NULL && pObject->AsSWSObject() != NULL)
    {
        lstExclude.Add(pObject);
    }
    pObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidTarget);
    if (pObject != NULL && pObject->AsSWSObject() != NULL)
    {
        lstExclude.Add(pObject);
    }

    CWalkHitInfo cHitInfo;
    int bStaticClear = NoNonWalkPolysDetailed(vStart, vEnd, fMinHeight, fMaxHeight,
        AREA_LINE_OF_SIGHT_RADIUS, OBJECT_INVALID, *poidHit, 1, &lstExclude,
        bCheckTransparentDoors, &cHitInfo, 0);
    if (bStaticClear == 0)
    {
        *pvHit = cHitInfo.m_vHitPosition;
        bClear = 0;
    }

    if (bStaticClear || bCheckAllRooms)
    {
        for (int i = 0; i < m_nRoomCount; i++)
        {
            if (!m_pRooms[i].ClearLineOfSight(vStart, vEnd, &vRoomHit, &nFace))
            {
                Vector vDelta = vRoomHit - vStart;
                float fDistance = vDelta * vDelta;
                if (fDistance < fNearestDistance)
                {
                    fNearestDistance = fDistance;
                    *pvHit = vRoomHit;
                    bClear = 0;
                }
            }
        }
    }
    return bClear;
}

inline void CSWSObject::AddEffectTarget(unsigned long oidTarget)
{
    m_lstEffectTargets.AddUnique(oidTarget);
}
// PC 0x004E7C20 GOG 0x004E7CF0
void CSWSArea::ApplyEffect(CGameEffect* pEffect, const Vector& vPosition,
                          const Vector& vOrientation)
{
    if (pEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_AREA_OF_EFFECT)
    {
        CSWSAreaOfEffectObject* pAreaEffect =
            new CSWSAreaOfEffectObject(OBJECT_INVALID);
        pAreaEffect->SetCreator(pEffect->GetCreator());
        if ((pEffect->m_nSubType & CGameEffect::SUBTYPE_MASK) ==
                SUBTYPE_MAGICAL &&
            pEffect->GetCreator() != OBJECT_INVALID)
        {
            CGameObject* pCreator = g_pAppManager->GetServerExoApp()->
                GetGameObject(pEffect->GetCreator());
            if (pCreator != NULL && pCreator->AsSWSObject() != NULL)
                pCreator->AsSWSObject()->AddEffectTarget(pAreaEffect->GetId());
        }
        pAreaEffect->LoadAreaEffect(pEffect->GetInteger(0));
        pAreaEffect->SetDuration(pEffect->m_nSubType & CGameEffect::DURATION_MASK,
                                pEffect->GetDuration());
        pAreaEffect->SetOrientation(vOrientation);
        pAreaEffect->SetEffectSpellId(pEffect->GetSpellId());
        if (pEffect->GetString(0) != "")
            pAreaEffect->m_asScripts[CSWSAreaOfEffectObject::SCRIPT_ON_ENTER] =
                pEffect->GetString(0);
        if (pEffect->GetString(1) != "")
            pAreaEffect->m_asScripts[CSWSAreaOfEffectObject::SCRIPT_HEARTBEAT] =
                pEffect->GetString(1);
        if (pEffect->GetString(2) != "")
            pAreaEffect->m_asScripts[CSWSAreaOfEffectObject::SCRIPT_ON_EXIT] =
                pEffect->GetString(2);
        pAreaEffect->AddToArea(this, vPosition.m_fX, vPosition.m_fY, vPosition.m_fZ, 1);
    }
    else if (pEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_SUMMON_CREATURE)
    {
        CSWSCreature* pCreator = g_pAppManager->GetServerExoApp()->
            GetCreatureByGameObjectID(pEffect->GetCreator());
        if (pCreator != NULL)
        {
            pEffect->SetObjectID(0, GetId());
            pEffect->SetFloat(0, vPosition.m_fX);
            pEffect->SetFloat(1, vPosition.m_fY);
            pEffect->SetFloat(2, vPosition.m_fZ);
            pCreator->ApplyEffect(pEffect, 0, 0);
            return;
        }
    }
    else if (pEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_VISUALEFFECT)
    {
        PlayVisualEffect(pEffect->GetInteger(0), vPosition);
    }
    else if (pEffect->GetEffectType() == CSWSEffectListHandler::EFFECT_LINK)
    {
        pEffect->UpdateLinked();
        if (pEffect->m_pLinkLeft != NULL)
            ApplyEffect(pEffect->m_pLinkLeft, vPosition, vOrientation);
        if (pEffect->m_pLinkRight != NULL)
            ApplyEffect(pEffect->m_pLinkRight, vPosition, vOrientation);
        pEffect->SetLinked(NULL, NULL);
    }
    delete pEffect;
}
// PC 0x004E7F80 GOG 0x004E8050
int CSWSArea::NoNonWalkPolysInDoors(
    const Vector& vStart, const Vector& vEnd, float fMinHeight, float fMaxHeight,
    float fRadius, unsigned long oidObject, unsigned long& oidHitObject,
    CExoArrayList<CGameObject*>* pExcludeList, int bFlag,
    int bCheckTransparentDoors)
{
    if (m_nBoundingBoxCount > 0)
    {
        Vector vMinLine, vMaxLine;
        if (vStart.m_fX < vEnd.m_fX)
        {
            vMinLine.m_fX = vStart.m_fX;
            vMaxLine.m_fX = vEnd.m_fX;
        }
        else
        {
            vMinLine.m_fX = vEnd.m_fX;
            vMaxLine.m_fX = vStart.m_fX;
        }
        if (vStart.m_fY < vEnd.m_fY)
        {
            vMinLine.m_fY = vStart.m_fY;
            vMaxLine.m_fY = vEnd.m_fY;
        }
        else
        {
            vMinLine.m_fY = vEnd.m_fY;
            vMaxLine.m_fY = vStart.m_fY;
        }
        vMinLine.m_fX -= fRadius;
        vMinLine.m_fY -= fRadius;
        vMaxLine.m_fX += fRadius;
        vMaxLine.m_fY += fRadius;
        int bAllocatedExcludeList = 0;

        for (int i = 0; i < m_nBoundingBoxCount; i++)
        {
            Vector vMax = m_pBoundingBoxes[i * 2 + 1];
            Vector vMin = m_pBoundingBoxes[i * 2];
            if ((vMinLine.m_fX < vMin.m_fX && vMaxLine.m_fX < vMin.m_fX) ||
                (vMaxLine.m_fX > vMax.m_fX && vMinLine.m_fX > vMax.m_fX) ||
                (vMinLine.m_fY < vMin.m_fY && vMaxLine.m_fY < vMin.m_fY) ||
                (vMaxLine.m_fY > vMax.m_fY && vMinLine.m_fY > vMax.m_fY) ||
                (fMinHeight < vMin.m_fZ && fMaxHeight < vMin.m_fZ) ||
                (fMaxHeight > vMax.m_fZ && fMinHeight > vMax.m_fZ))
                continue;

            unsigned long oidDoor = m_pBoundingBoxIds[i];
            CGameObject* pObject =
                g_pAppManager->GetServerExoApp()->GetGameObject(oidDoor);
            if (pObject != NULL && pObject->AsSWSDoor() != NULL &&
                !GetIsIDInExcludeList(oidDoor, pExcludeList) &&
                !NoNoneWalkPolysInStaticObject(m_pBoundingBoxIds[i], vStart,
                    vEnd, fMinHeight, fMaxHeight, fRadius, oidObject, bFlag, NULL))
            {
                oidHitObject = m_pBoundingBoxIds[i];
                if (bCheckTransparentDoors == 1)
                {
                    if (pExcludeList == NULL)
                    {
                        CExoArrayList<CGameObject*>* pNewExcludeList =
                            new CExoArrayList<CGameObject*>(0);
                        pExcludeList = pNewExcludeList;
                        bAllocatedExcludeList = 1;
                    }
                    int bClear = HandleTransparentDoors(vStart, vEnd,
                        fMinHeight, fMaxHeight, fRadius, oidObject, oidHitObject,
                        bFlag, pExcludeList, 1);
                    if (pExcludeList != NULL && bAllocatedExcludeList)
                        delete pExcludeList;
                    return bClear;
                }
                return 0;
            }
        }
    }
    return 1;
}
// PC 0x004E95C0 GOG 0x004E9690
int CSWSArea::AddObjectToArea(unsigned long oidObject, int bLoadingGame)
{
    NwsareaSync4();
    CGameObjectArray* pObjectArray =
        g_pAppManager->m_pServerExoApp->GetObjectArray();
    CGameObject* pGameObject;
    if (pObjectArray->GetGameObject(oidObject, &pGameObject) !=
        CGameObjectArray::SUCCESS)
    {
        return 0;
    }

    if (m_lstObjectIds.Contains(oidObject))
    {
        RemoveObjectFromArea(oidObject);
    }

    float fX = ((CSWSObject*)pGameObject)->m_vPosition.m_fX;
    int nIndex;
    if (GetFirstObjectIndiceByX(&nIndex, fX))
    {
        m_lstObjectIds.Insert(oidObject, nIndex);
    }
    else
    {
        m_lstObjectIds.Add(oidObject);
    }

    CGameObject* pCreatureObject;
    if (pObjectArray->GetGameObject(oidObject, &pCreatureObject) ==
            CGameObjectArray::SUCCESS &&
        pCreatureObject != NULL &&
        pCreatureObject->AsSWSCreature() != NULL)
    {
        if (g_pAppManager->m_pServerExoApp->GetClientObjectByObjectId(oidObject) != NULL)
        {
            IncrementPlayersInArea();
        }
        if (bLoadingGame == 1)
        {
            CScriptEvent* pEvent = new CScriptEvent;
            pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_OBJECT_ENTER;
            pEvent->SetInteger(0,
                g_pAppManager->m_pServerExoApp->GetLoadFromSaveGame());
            CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
            unsigned long oidArea = m_nId;
            pServer->GetServerAIMaster()->AddEventDeltaTime(
                0, 0, oidObject, oidArea, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
        }
    }
    return 1;
}
// PC 0x004E9780 GOG 0x004E9850
int CSWSArea::LoadArea(int bLoadingGame)
{
    NwsareaSync4();
    if (m_pRes->Demand() == NULL)
    {
        return 0;
    }

    CResStruct cTopLevelStruct;

    m_pRes->GetTopLevelStruct(&cTopLevelStruct);
    LoadAreaHeader(&cTopLevelStruct);
    LoadRoomInfo(&cTopLevelStruct);
    for (int nRoom = 0; nRoom < m_nRoomCount; nRoom++)
    {
        for (int nAudio = 0; nAudio < m_nRoomAudioCount; nAudio++)
        {
            if (m_psRoomAudio[nAudio].CompareNoCase(
                    m_pRooms[nRoom].GetResRef().GetResRefStr()))
            {
                m_pnRoomAudioIndex[nRoom] = (unsigned char)nAudio;
                break;
            }
        }
    }
    LoadGIT(bLoadingGame);
    LoadPathPoints(&cTopLevelStruct);
    m_pRes->Release();

    unsigned long oidArea = m_nId;
    CServerExoApp* pServerApp = g_pAppManager->m_pServerExoApp;
    pServerApp->GetModule()->AddObjectToLookupTable(m_sTag, oidArea);

    int nHeight = m_nHeight;
    int nWidth = m_nWidth;
    m_nGridMaskBytes = (unsigned char)ceil(nWidth * nHeight / 8.0f);

    return 1;
}

static inline void NwsareaTrace1()
{
}

static inline void NwsareaSync4()
{
    NwsareaSync3();
}

static inline void NwsareaSync3()
{
    NwsareaSync2();
}

static inline void NwsareaSync2()
{
    NwsareaSync1();
}

static inline void NwsareaSync1()
{
}

#ifdef _XBOX

int CSWSArea::TestPathPoint(float fPersonalSpace, int bVerbose)
{
    int bFailed = FALSE;

    for (unsigned long nPoint = 0; nPoint < (unsigned long)m_nPathPointCount; nPoint++)
    {
        Vector vPoint(m_pPathPoints[nPoint].m_fX, m_pPathPoints[nPoint].m_fY, 0.0f);
        ComputeHeight(vPoint, 1, NULL);
        if (m_pPathPoints[nPoint].m_nFirstConnection + m_pPathPoints[nPoint].m_nConnectionCount == 0)
            bFailed = TRUE;

        for (unsigned long nConnection = m_pPathPoints[nPoint].m_nFirstConnection;
             nConnection < (unsigned long)(m_pPathPoints[nPoint].m_nFirstConnection + m_pPathPoints[nPoint].m_nConnectionCount);
             nConnection++)
        {
            Vector vStart(m_pPathPoints[nPoint].m_fX, m_pPathPoints[nPoint].m_fY, 0.0f);
            Vector vEnd(m_pPathPoints[m_pPathConnections[nConnection]].m_fX,
                        m_pPathPoints[m_pPathConnections[nConnection]].m_fY, 0.0f);
            vStart.m_fZ = ComputeHeight(vStart, 1, NULL);
            vEnd.m_fZ = ComputeHeight(vEnd, 1, NULL);
            if (nPoint > (unsigned long)m_pPathConnections[nConnection])
            {
                vStart.m_fZ += AREA_PATH_POINT_TEST_HEIGHT;
                vEnd.m_fZ += AREA_PATH_POINT_TEST_HEIGHT;
            }

            int nResult = TestDirectLine(vStart, vEnd, fPersonalSpace, 0.0f, 1, NULL);
            Vector vPerpendicular = normalize(cross(Vector(0.0f, 0.0f, 1.0f), vEnd - vStart));
            Vector vDirection = normalize(vEnd - vStart);
            if (nResult != DIRECT_LINE_CLEAR && nResult != DIRECT_LINE_BLOCKED_CREATURE)
            {
                g_pExoBase->m_pExoDebug->WriteToScriptLogFile(g_pExoBase->m_pExoDebug->m_sLogDirectory);
                g_pExoBase->m_pExoDebug->FlushScriptLogFile();
                AurPostString("Invalid Path Point conections found, details in error log.", 0, 13, 100.0f);
                bFailed = TRUE;
            }
        }
    }
    return bFailed;
}

#include "aurconsole.h"
void AurRemoveAllDrawableSquares();

static char* TestPathPoints(char* szArgs)
{
    int nLength = strlen(szArgs);
    if (nLength == 0)
    {
        return "USAGE: <Personal Space>";
    }
    g_pAppManager->m_pServerExoApp->GetModule()->TestPathPoints(szArgs);
    return "SUSSESS";
}

static char* ClearPathPoints(char* szArgs)
{
    AurRemoveAllDrawableSquares();
    return "SUSSESS";
}

ConsoleFunc ConVg_nBSPWidth("g_nBSPWidth", &g_nBSPWidth);
ConsoleFunc ConVg_nBSPHeight("g_nBSPHeight", &g_nBSPHeight);
ConsoleFunc ConFTestPathPoints("TestPathPoints", TestPathPoints);
ConsoleFunc ConFClearPathPoints("ClearPathPoints", ClearPathPoints);
#endif
