#include "exobase.h"
#include "resgff.h"
#include "appmanager.h"
#include "serverexoapp.h"
#include "gameobjectarray.h"
#include "nwarea.h"
#include "nwrules.h"
#include "nwportrait.h"
#include "twodimarray.h"
#include "twodimarrays.h"
#include "nwsmodule.h"
#include "nwsarea.h"
#include "serveraimaster.h"
#include "nwscreaturestats.h"
#include "nwseffectlist.h"
#include "partytables.h"
#include "nwstrigger.h"
#include "nwswaypoint.h"
#include "worldtimer.h"
#include "nwscript.h"
#include "nwsitem.h"
#include "nwitem.h"
#include "gameobjectarray.h"

#include <math.h>

#include "nwsscriptevent.h"
#include "clientexoapp.h"
#include "guiingame.h"
#include "swparty.h"
#include "nwsplayer.h"
#include "serverexoappinternal.h"
#include "clientoptions.h"
#include "nwctrigger.h"
#include "nwccreature.h"
#include "nwfeat.h"

static inline void NwstriggerTrace2();
static inline void NwstriggerTrace1();
#define NWSTRIGGER_TRACE() NwstriggerTrace1()

static inline CServerExoApp* GetServerExoApp();

int RenderQATriggers = 0;
float CSWSTrigger::m_fTrapRadiusRange = 1.0f;
// PC 0x005845C0 GOG 0x00584640
void CSWSTrigger::CreateNewGeometry(float fRadius, const Vector& vPosition,
                                    CSWSArea* pArea)
{
    m_nPolygonPointCount = 4;
    m_nGeometryPointCount = 4;
    if (m_pGeometry != NULL)
    {
        delete[] m_pGeometry;
        m_pGeometry = NULL;
    }
    if (m_pPolygonPointOrder != NULL)
    {
        delete[] m_pPolygonPointOrder;
        m_pPolygonPointOrder = NULL;
    }

    if (pArea != NULL)
    {
        m_pGeometry = new Vector[4];
        m_pPolygonPointOrder = new unsigned long[4];

        m_pGeometry[0].m_fX = m_vPosition.m_fX - fRadius;
        m_pGeometry[0].m_fY = m_vPosition.m_fY + fRadius;
        m_pGeometry[0].m_fZ = pArea->ComputeHeight(m_pGeometry[0], 1, NULL);
        if (m_pGeometry[0].m_fZ == 0.0f)
        {
            m_pGeometry[0].m_fZ = m_vPosition.m_fZ;
        }
        m_pPolygonPointOrder[0] = 0;

        m_pGeometry[1].m_fX = m_vPosition.m_fX - fRadius;
        m_pGeometry[1].m_fY = m_vPosition.m_fY - fRadius;
        m_pGeometry[1].m_fZ = pArea->ComputeHeight(m_pGeometry[1], 1, NULL);
        if (m_pGeometry[1].m_fZ == 0.0f)
        {
            m_pGeometry[1].m_fZ = m_vPosition.m_fZ;
        }
        m_pPolygonPointOrder[1] = 1;

        m_pGeometry[2].m_fX = m_vPosition.m_fX + fRadius;
        m_pGeometry[2].m_fY = m_vPosition.m_fY - fRadius;
        m_pGeometry[2].m_fZ = pArea->ComputeHeight(m_pGeometry[2], 1, NULL);
        if (m_pGeometry[2].m_fZ == 0.0f)
        {
            m_pGeometry[2].m_fZ = m_vPosition.m_fZ;
        }
        m_pPolygonPointOrder[2] = 2;

        m_pGeometry[3].m_fX = m_vPosition.m_fX + fRadius;
        m_pGeometry[3].m_fY = m_vPosition.m_fY + fRadius;
        m_pGeometry[3].m_fZ = pArea->ComputeHeight(m_pGeometry[3], 1, NULL);
        if (m_pGeometry[3].m_fZ == 0.0f)
        {
            m_pGeometry[3].m_fZ = m_vPosition.m_fZ;
        }
        m_pPolygonPointOrder[3] = 3;
    }
}
// PC 0x00584860 GOG 0x005848E0
int CSWSTrigger::GetCanFireMineOnObject(unsigned long oidObject, int bForce)
{
    int bCanFire = 1;
    CSWSCreature* pCreator =
        g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(m_oidCreator);
    CServerExoApp* pServerApp = GetServerExoApp();
    CSWSCreature* pCreature = pServerApp->GetCreatureByGameObjectID(oidObject);
    if (pCreature != NULL)
    {
        if (!bForce)
        {
            if (pCreator != NULL)
            {
                if (pCreator->GetFaction() != NULL)
                {
                    if (pCreature->GetReputation(pCreator->GetId(), 0) >
                            MAX_HOSTILE_REPUTATION ||
                        m_nFaction == pCreature->GetFactionId())
                    {
                        bCanFire = 0;
                    }
                }
            }
            else if (pCreature->GetReputation(GetId(), 0) >
                         MAX_HOSTILE_REPUTATION ||
                     pCreature->GetFactionId() == m_nFaction)
            {
                bCanFire = 0;
            }
        }
        if (pCreature->m_pStats->GetEffectImmunity(
                CSWSEffectListHandler::IMMUNITY_TYPE_MINE, NULL))
        {
            bCanFire = 0;
        }
        if (pCreature->m_pStats->HasFeat(CSWFeat::FEAT_MINE_IMMUNITY))
        {
            bCanFire = 0;
        }
        if (pCreature->m_bInParty != 0)
        {
            CSWCCreature* pLeader =
                g_pAppManager->GetClientExoApp()->GetSWParty()->GetCharacter(0);
            if (pLeader != NULL)
            {
                CSWSCreature* pServerLeader = pLeader->GetServerCreature();
                if (pServerLeader != NULL &&
                    pServerLeader->GetStats()->HasFeat(CSWFeat::FEAT_PARTY_MINE_IMMUNITY))
                {
                    return 0;
                }
            }
        }
    }
    return bCanFire;
}
// PC 0x00584990 GOG 0x00584A10
void CSWSTrigger::OnEnterMine(int bForce)
{
    CSWSCreature* pCreature =
        g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(m_oidLastEntered);
    if (pCreature != NULL)
    {
        if (GetCanFireMineOnObject(pCreature->m_nId, bForce))
        {
            unsigned long oidKey =
                pCreature->GetItemRepository(1)->FindItemWithTag(m_sKeyName);
            if (oidKey == OBJECT_INVALID)
            {
                for (int i = 0; i < CSWInventory::TOTAL_SLOT_COUNT; ++i)
                {
                    CSWInventory* pInventory = pCreature->m_pInventory;
                    CSWSItem* pItem = pInventory->GetItemInSlot(1 << i);
                    if (pItem != NULL && pItem->m_sTag == m_sKeyName)
                    {
                        oidKey = pItem->m_nId;
                    }
                }
            }

            CSWSItem* pKey =
                g_pAppManager->m_pServerExoApp->GetItemByGameObjectID(oidKey);
            if (pKey != NULL)
            {
                if (m_bAutoRemoveKey)
                {
                    pCreature->GetItemRepository(1)->RemoveItem(pKey);
                    pCreature->m_pInventory->RemoveItem(pKey);
                    delete pKey;
                }
                m_oidLastDisarmedBy = m_oidLastEntered;
                g_pVirtualMachine->RunScript(&m_asScripts[SCRIPT_ONDISARM], m_nId, 1);
                CServerExoApp* pServerApp = g_pAppManager->m_pServerExoApp;
                pServerApp->GetServerAIMaster()->AddEventDeltaTime(
                    0, 0, m_nId, m_nId, CServerAIMaster::EVENT_DESTROY_OBJECT, NULL);

            }
            else
            {
                pCreature->SendFeedbackMessage(FEEDBACK_TRAP_TRIGGERED, NULL);
                g_pVirtualMachine->RunScript(&m_asScripts[SCRIPT_ONTRAPTRIGGERED], m_nId, 1);
                g_pVirtualMachine->RunScript(&m_asScripts[SCRIPT_ONENTER], m_nId, 1);
                if (m_bTrapOneShot)
                {
                    CServerExoApp* pServerApp = g_pAppManager->m_pServerExoApp;
                    pServerApp->GetServerAIMaster()->AddEventDeltaTime(
                        0, 0, m_nId, m_nId, CServerAIMaster::EVENT_DESTROY_OBJECT, NULL);
                }
            }
        }
    }
}
// PC 0x00584B20 GOG 0x00584BA0
CSWCTrigger* CSWSTrigger::GetClientTrigger()
{
    if (GetClientObject() != NULL)
        return GetClientObject()->AsSWCTrigger();

    return NULL;
}
// PC 0x00584B40 GOG 0x00584BC0
void CSWSTrigger::SetCreator(unsigned long oidCreator)
{
    m_oidCreator = oidCreator;
    if (oidCreator != OBJECT_INVALID &&
        (oidCreator == GetServerExoApp()->GetPlayerCreatureId() ||
         GetServerExoApp()->GetPartyTable()->GetIsMember(
             oidCreator) != 0))
    {
        m_bSetByPlayerParty = 1;
    }
    else
    {
        m_bSetByPlayerParty = 0;
    }
}
// PC 0x00583B40 GOG 0x00583BC0
int CSWSTrigger::GetTargetArea()
{
    if (m_nTriggerType != 0)
    {
        CGameObject* pObject;
        CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
        unsigned long oidWaypoint = pServerExoApp->GetModule()->GetWaypoint(m_sLinkedTo);
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        if (pServerExoApp->GetObjectArray()->GetGameObject(
                oidWaypoint, &pObject) == CGameObjectArray::SUCCESS)
        {
            return pObject->AsSWSWaypoint()->GetArea()->GetId();
        }
    }
    return OBJECT_INVALID;
}
// PC 0x00583BB0 GOG 0x00583C30
int CSWSTrigger::CalculateNearestPoint(const Vector& vPoint,
                                     Vector* pvNearestPoint)
{
    Vector vNearest;
    if (m_bMineTrigger != 0)
    {
        Vector vDirection;
        float fPrimaryRange;
        bool bRangeMissing;
        {
            CExoString sPrimaryRange("PrimaryRange");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            EXOXBOXASSERT(p2DArrays->m_pRanges);
            C2DA* pRanges = p2DArrays->m_pRanges;
            bRangeMissing = !pRanges->GetFLOATEntry(
                17, sPrimaryRange, &fPrimaryRange);
        }
        if (bRangeMissing)
        {
            fPrimaryRange = 1.0f;
        }

        vDirection = vPoint - m_vPosition;
        if (vDirection * vDirection > 0.0001f)
        {
            vDirection = normalize(vDirection);
            vNearest = m_vPosition + vDirection * fPrimaryRange;
        }
        else
        {
            vNearest = vPoint;
        }
    }
    else
    {
        float fNearestDistance = 100000000.0f;
        int i = 0;
        int nPointCount = m_nPolygonPointCount;
        if (nPointCount > 0)
        {
            do
            {
                int nPoint = m_pPolygonPointOrder[i];
                int nNextPoint = i != nPointCount - 1 ? i + 1 : 0;
                nNextPoint = m_pPolygonPointOrder[nNextPoint];
                Vector* pStart = &m_pGeometry[nPoint];
                Vector* pEnd = &m_pGeometry[nNextPoint];
                Vector vOffset = vPoint - *pStart;
                Vector vDirection = *pEnd - *pStart;
                float fProjection = (vOffset * vDirection) /
                                    (vDirection * vDirection);
                Vector vPointOnEdge;
                if (fProjection > 1.0)
                {
                    vPointOnEdge = *pStart + vDirection;
                }
                else if (fProjection < 0.0)
                {
                    vPointOnEdge = *pStart;
                }
                else
                {
                    vPointOnEdge = *pStart + vDirection * fProjection;
                }
                Vector vDistance = vPoint - vPointOnEdge;
                float fDistance = vDistance * vDistance;
                if (fDistance < fNearestDistance)
                {
                    fNearestDistance = fDistance;
                    vNearest = vPointOnEdge;
                }
                i++;
            } while (i < nPointCount);
        }
    }
    *pvNearestPoint = vNearest;
    return 1;
}
// PC 0x00583F70 GOG 0x00583FF0
void CSWSTrigger::ComputeBoundingBox(float* pfMinimumX, float* pfMinimumY,
                                     float* pfMaximumX, float* pfMaximumY)
{
    int i;
    Vector* pPoint;

    if (m_fBoundingMinX == m_fBoundingMaxX &&
        m_fBoundingMinY == m_fBoundingMaxY)
    {
        pPoint = m_pGeometry;
        m_fBoundingMinX = pPoint->m_fX;
        m_fBoundingMinY = pPoint->m_fY;
        m_fBoundingMaxX = pPoint->m_fX;
        m_fBoundingMaxY = pPoint->m_fY;
        i = 1;
        for (; i < m_nGeometryPointCount; i++)
        {
            pPoint = &m_pGeometry[i];
            if (pPoint->m_fX < m_fBoundingMinX)
            {
                m_fBoundingMinX = pPoint->m_fX;
            }
            if (pPoint->m_fY < m_fBoundingMinY)
            {
                m_fBoundingMinY = pPoint->m_fY;
            }
            if (pPoint->m_fX > m_fBoundingMaxX)
            {
                m_fBoundingMaxX = pPoint->m_fX;
            }
            if (pPoint->m_fY > m_fBoundingMaxY)
            {
                m_fBoundingMaxY = pPoint->m_fY;
            }
        }
    }

    *pfMinimumX = m_fBoundingMinX;
    *pfMinimumY = m_fBoundingMinY;
    *pfMaximumX = m_fBoundingMaxX;
    *pfMaximumY = m_fBoundingMaxY;
}
// PC 0x00584090 GOG 0x00584110
int CSWSTrigger::GetDetectDC()
{
    int nDC = m_nTrapDetectDC;
    int nModifier = 0;
    switch (g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nDifficulty)
    {
        case 0:
            nModifier = -5;
            break;
        case 2:
            nModifier = 5;
            break;
    }
    nDC += nModifier;
    return nDC < 1 ? 1 : nDC;
}
// PC 0x005840E0 GOG 0x00584160
int CSWSTrigger::GetDisarmDC()
{
    int nDC = m_nTrapDisarmDC;
    int nModifier = 0;
    switch (g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nDifficulty)
    {
        case 0:
            nModifier = -5;
            break;
        case 2:
            nModifier = 5;
            break;
    }
    nDC += nModifier;
    return nDC < 1 ? 1 : nDC;
}
// PC 0x00584130 GOG 0x005841B0
int CSWSTrigger::LineSegmentIntersectTrigger(const Vector& vStart,
                                             const Vector& vEnd)
{
    CSWArea* pArea;
    int i;
    int nPoint;
    int nNextPoint;
    float fIntersectionX;
    float fIntersectionY;

    pArea = (CSWArea*)GetArea();
    if (pArea != NULL)
    {
        for (i = 0; i < m_nPolygonPointCount; i++)
        {
            nPoint = m_pPolygonPointOrder[i];
            nNextPoint = i != m_nPolygonPointCount - 1 ? i + 1 : 0;
            nNextPoint = m_pPolygonPointOrder[nNextPoint];
            if (pArea->IntersectLineSegments(
                    vStart.m_fX, vStart.m_fY, vEnd.m_fX, vEnd.m_fY,
                    m_pGeometry[nPoint].m_fX, m_pGeometry[nPoint].m_fY,
                    m_pGeometry[nNextPoint].m_fX, m_pGeometry[nNextPoint].m_fY,
                    &fIntersectionY, &fIntersectionX) == 1)
            {
                return 1;
            }
        }
    }
    return 0;
}
// PC 0x005862D0 GOG 0x00586350
CSWSTrigger::CSWSTrigger(unsigned long oidId)
    : CSWSObject(CGameObject::OBJECT_TYPE_TRIGGER, oidId, 0)
{
    g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddObject(this, 0);
    m_nAnimation = ANIMATION_COMBAT_READY;

    bool bRangeMissing;
    {
        CExoString sColumn("PrimaryRange");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pRanges = p2DArrays->m_pRanges;
        bRangeMissing = !pRanges->GetFLOATEntry(
            17, sColumn, &m_fTrapRadiusRange);
    }
    if (bRangeMissing)
    {
        m_fTrapRadiusRange = 1.0f;
    }

    m_sLinkedTo = "";
    m_nGeometryPointCount = 0;
    m_pGeometry = NULL;
    m_nGeometryInitialValue1 = 0;
    m_nGeometryInitialValue2 = 0;
    m_nPolygonPointCount = 0;
    m_pPolygonPointOrder = NULL;
    m_nCustomScriptEventId = 0;
    m_oidLastEntered = OBJECT_INVALID;
    m_oidLastLeft = OBJECT_INVALID;
    m_oidLastDisarmedBy = OBJECT_INVALID;
    m_nTriggerType = 0;
    m_bMineTrigger = 0;
    m_bTrapDetectable = 1;
    m_bTrapDisarmable = 1;
    m_bTrapFlagged = 0;
    m_sKeyName = "";
    m_bAutoRemoveKey = 0;
    m_bTrapOneShot = 1;
    m_nTrapType = 0xFF;
    m_nFaction = 0;
    m_nTrapDetectDC = 0;
    m_nTrapDisarmDC = 0;
    m_nOwnerDemolitionsSkill = 0;

    for (int i = 0; i < 7; i++)
    {
        m_asScripts[i] = "default";
    }

    m_fBoundingMinX = 0.0f;
    m_fBoundingMaxX = 0.0f;
    m_fBoundingMinY = 0.0f;
    m_fBoundingMaxY = 0.0f;
    m_nCursor = 0;
    m_nLastHeartbeatTime = 0;
    m_nLastHeartbeatDay = 0;
    m_oidCreator = OBJECT_INVALID;
    m_fHighlightHeight = 0.1f;
    m_nLoadScreenId = 0;
    m_nObjectIterator = 0;
    m_bSetByPlayerParty = 0;
    SetAnimation(TRAP_ANIMATION_ACTIVE);
}
// PC 0x00586560 GOG 0x005865E0
int CSWSTrigger::LoadFromTemplate(const CResRef& cResRef)
{
    CResGFF* pRes = new CResGFF(2032, "UTT ", cResRef);
    CResStruct stTop;
    int bSuccess;

    if (pRes->m_bLoaded == 0)
    {
        delete pRes;
        return 0;
    }

    pRes->GetTopLevelStruct(&stTop);
    bSuccess = LoadTrigger(pRes, &stTop);
    delete pRes;
    return bSuccess;
}
// PC 0x005866E0 GOG 0x00586760
CSWSTrigger::~CSWSTrigger()
{
    delete[] m_pGeometry;
    delete[] m_pPolygonPointOrder;

    if (GetArea() != NULL)
    {
        RemoveFromArea();
    }

    g_pAppManager->m_pServerExoApp->GetServerAIMaster()->RemoveObject(this);

    CSWSModule* pModule = g_pAppManager->m_pServerExoApp->GetModule();
    if (pModule != NULL)
    {
        unsigned long oidObject = m_nId;
        pModule->RemoveObjectFromLookupTable(m_sTag, oidObject);
    }
}
// PC 0x00586820 GOG 0x005868A0
void CSWSTrigger::AddToArea(CSWSArea* pArea, float fX, float fY, float fZ,
                            int bLoadingGame)
{
    RemoveFromArea();
    if (pArea != NULL)
    {
        CExoString sTag;
        float fMinimumX;
        float fMinimumY;
        float fMaximumX;
        float fMaximumY;

        SetArea(pArea);
        SetPosition(Vector(fX, fY, fZ), 0, 1, 0);
        GetArea()->AddObjectToArea(m_nId, bLoadingGame);
        CSWSArea* pTriggerArea = GetArea();
        pTriggerArea->m_lstTriggerIds.Add(m_nId);
        ComputeBoundingBox(&fMinimumX, &fMinimumY, &fMaximumX, &fMaximumY);
    }
}
// PC 0x00586930 GOG 0x005869B0
void CSWSTrigger::EventHandler(unsigned long nEventId, unsigned long oidCaller,
                              void* pEventData, unsigned long nDay,
                              unsigned long nTime)
{
    if (nEventId == CServerAIMaster::EVENT_TIMED_EVENT)
    {
        if (pEventData != NULL)
            g_pVirtualMachine->RunScriptSituation(pEventData, m_nId, 1);
    }
    else if (nEventId == CServerAIMaster::EVENT_DESTROY_OBJECT)
    {
        if (m_nEC)
        {
            RemoveFromArea();
            delete this;
        }
    }
    else if (nEventId == CServerAIMaster::EVENT_SIGNAL_EVENT)
    {
        CScriptEvent* pEvent = (CScriptEvent*)pEventData;
        if (pEvent->m_nType == CSWSSCRIPTEVENT_EVENTTYPE_ON_OBJECT_ENTER)
        {
            int bForce = pEvent->GetInteger(0);
            m_oidLastEntered = pEvent->GetObjectID(0);
            CSWSCreature* pCreature = g_pAppManager->GetServerExoApp()
                ->GetCreatureByGameObjectID(pEvent->GetObjectID(0));
            if (m_nTriggerType)
            {
                CSWSArea* pArea = GetArea();
                if (pCreature != NULL &&
                    g_pAppManager->GetClientExoApp()->GetInGameGui()
                        ->m_nGlobalDialogState == 0)
                {
                    if (pArea != NULL && m_sLinkedToModule.GetLength() > 0)
                    {
                        if (pArea->m_bTransitionPending)
                        {
                            unsigned char nTransitionId = pArea->m_nCurrentTransitionPendingId;
                            if (pEvent->GetInteger(1) == nTransitionId)
                            {
                                if (!pCreature->GetDead() && !pCreature->GetDeadTemp())
                                {
                                    CServerExoApp* pServer = g_pAppManager->GetServerExoApp();
                                    pServer->SetMoveToModulePending(1);
                                    pServer->SetMoveToModuleString(m_sLinkedToModule);
                                    pServer->SetMoveToModuleStartWaypoint(m_sLinkedTo);
                                }
                                else
                                {
                                    g_pAppManager->GetClientExoApp()->GetInGameGui()->StopGlobalFade();
                                }
                                pArea->SetTransitionPending(0, 0);
                            }
                        }
                        else if (pCreature->m_nAreaWaitState ||
                                 g_pAppManager->GetServerExoApp()->GetPartyTable()
                                     ->GetIsLeader(pCreature->GetId()))
                        {
                            if (g_pAppManager->GetClientExoApp()->GetSWParty()->IsPartyTogether())
                            {
                                CScriptEvent* pTransitionEvent = new CScriptEvent;
                                unsigned char nTransitionId = pArea->GetNextTransitionPendingID();
                                pTransitionEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_OBJECT_ENTER;
                                pTransitionEvent->SetObjectID(0, pEvent->GetObjectID(0));
                                pTransitionEvent->SetInteger(0, pEvent->GetInteger(0));
                                pTransitionEvent->SetInteger(1, nTransitionId);
                                g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                                    0, TRANSITION_FADE_DELAY, m_nId, m_nId,
                                    CServerAIMaster::EVENT_SIGNAL_EVENT, pTransitionEvent);
                                Vector vBlack(0.0f, 0.0f, 0.0f);
                                g_pAppManager->GetClientExoApp()->GetInGameGui()->StartGlobalFade(
                                    0, 0.0f, 0.5f, vBlack);
                                pArea->SetTransitionPending(1, nTransitionId);
                                g_pAppManager->GetServerExoApp()->SetPauseState(
                                    CServerExoAppInternal::PAUSE_TYPE_COMBAT_MASK, 0);
                            }
                            else
                            {
                                {
                                    CExoString sScript("k_trg_transfail");
                                    g_pVirtualMachine->RunScript(&sScript, pCreature->GetId(), 1);
                                }
                            }
                        }
                    }
                }
                else if (pArea != NULL)
                {
                    pArea->SetTransitionPending(0, 0);
                }
            }
            else if (m_bMineTrigger)
            {
                OnEnterMine(bForce);
            }
            else
            {
                g_pVirtualMachine->RunScript(GetScriptName(SCRIPT_ONENTER), m_nId, 1);
            }
            SetFadeOutTime(1);
        }
        else if (pEvent->m_nType == CSWSSCRIPTEVENT_EVENTTYPE_ON_OBJECT_EXIT)
        {
            m_oidLastLeft = pEvent->GetObjectID(0);
            g_pVirtualMachine->RunScript(GetScriptName(SCRIPT_ONEXIT), m_nId, 1);
        }
        else if (pEvent->m_nType == CSWSSCRIPTEVENT_EVENTTYPE_USER_DEFINED)
        {
            m_nCustomScriptEventId = pEvent->GetInteger(0);
            g_pVirtualMachine->RunScript(GetScriptName(SCRIPT_USERDEFINE), m_nId, 1);
        }
        else if (pEvent->m_nType == CSWSSCRIPTEVENT_EVENTTYPE_ON_DISARM)
        {
            m_oidLastDisarmedBy = oidCaller;
            g_pVirtualMachine->RunScript(GetScriptName(SCRIPT_ONDISARM), m_nId, 1);
        }
        else if (pEvent->m_nType == CSWSSCRIPTEVENT_EVENTTYPE_ON_CLICK)
        {
            m_oidLastEntered = pEvent->GetObjectID(0);
            int bNoClickScript = m_asScripts[SCRIPT_ONCLICK].IsEmpty();
            if (!bNoClickScript)
                bNoClickScript = m_asScripts[SCRIPT_ONCLICK] == "default";
            if (bNoClickScript && m_nTriggerType == TRIGGER_TYPE_AREA_TRANSITION)
                m_asScripts[SCRIPT_ONCLICK] = "NW_G0_Transition";
            CSWSPlayer* pPlayer = g_pAppManager->GetServerExoApp()
                ->GetClientObjectByObjectId(pEvent->GetObjectID(0));
            if (pPlayer != NULL && m_nLoadScreenId != 0)
            {
                CExoString sBMP("");
                unsigned short nLoadScreenId = m_nLoadScreenId;
                pPlayer->SetAreaTransitionBMP(nLoadScreenId, sBMP);
            }
            g_pVirtualMachine->RunScript(GetScriptName(SCRIPT_ONCLICK), m_nId, 1);
        }
        delete pEvent;
    }
}
// PC 0x00584BA0 GOG 0x00584C20
void CSWSTrigger::AIUpdate()
{
    unsigned __int64 nTimer;
    unsigned long nNowDay;
    unsigned long nNowTime;
    unsigned long nDiffDay;
    unsigned long nDiffTime;

    nTimer = g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
    {
        CServerExoApp* pServerApp = g_pAppManager->m_pServerExoApp;
        pServerApp->GetWorldTimer()->GetWorldTime(&nNowDay, &nNowTime);
    }
    {
        CServerExoApp* pServerApp = g_pAppManager->m_pServerExoApp;
        pServerApp->GetWorldTimer()->SubtractWorldTimes(
            nNowDay, nNowTime, m_nLastHeartbeatDay, m_nLastHeartbeatTime,
            &nDiffDay, &nDiffTime);
    }
    if (nDiffTime >= HEARTBEAT_INTERVAL)
    {
        g_pVirtualMachine->RunScript(GetScriptName(SCRIPT_HEARTBEAT), m_nId, 1);
        m_nLastHeartbeatDay = nNowDay;
        m_nLastHeartbeatTime = nNowTime;
    }

    if (m_bMineTrigger != 0)
    {
        CServerExoApp* pServerApp = g_pAppManager->m_pServerExoApp;
        CSWSCreature* pPlayer = pServerApp->GetPlayerCreature();
        if (m_bTrapFlagged == 0 &&
            (pPlayer == NULL ||
             (m_nFaction != pPlayer->m_pStats->m_nFactionId &&
              GetReputation(pPlayer->GetId(), 0) < MIN_FRIENDLY_REPUTATION &&
              m_lstTrapTargets.Contains(pPlayer->GetId()) == 0)))
        {
            if (m_nAnimation != TRAP_ANIMATION_ACTIVE)
            {
                SetAnimation(TRAP_ANIMATION_ACTIVE);
            }
        }
        else
        {
            if (m_nAnimation != TRAP_ANIMATION_INACTIVE)
            {
                SetAnimation(TRAP_ANIMATION_INACTIVE);
            }
        }
    }
    RunActions(nNowDay, nNowTime, nTimer);
}
// PC 0x00584D00 GOG 0x00584D80
int CSWSTrigger::GetClosestPointWithinTrigger(const Vector& vPosition,
                                              Vector* pvClosestPoint)
{
    float fMinimumX;
    float fMinimumY;
    float fMaximumX;
    float fMaximumY;
    float fClosestDistance = 1.0e11f;
    Vector vClosestPoint;
    Vector vPoint;
    Vector vOffset;

    ComputeBoundingBox(&fMinimumX, &fMinimumY, &fMaximumX, &fMaximumY);
    float fStepX = (fMaximumX - fMinimumX) * 0.2f;
    float fStepY = (fMaximumY - fMinimumY) * 0.2f;
    for (float fX = fMinimumX; fX <= fMaximumX; fX += fStepX)
    {
        for (float fY = fMinimumY; fY <= fMaximumY; fY += fStepY)
        {
            vPoint = Vector(fX, fY, 0.0f);
            vOffset = vPosition - vPoint;
            float fDistance = vOffset * vOffset;
            if (fDistance < fClosestDistance && InTrigger(vPoint) == 1)
            {
                vClosestPoint = vPoint;
                fClosestDistance = fDistance;
            }
        }
    }
    if (fClosestDistance < 1.0e10f)
    {
        *pvClosestPoint = vClosestPoint;
        return 1;
    }
    return 0;
}
// PC 0x00584EC0 GOG 0x00584F40
int CSWSTrigger::LoadTrigger(CResGFF* pRes, CResStruct* pStruct)
{
    int bSuccess;

    unsigned short nPortraitId =
        pRes->ReadFieldWORD(pStruct, "PortraitId", bSuccess,
                           PORTRAIT_ID_RESERVED_FFFF);
    if (nPortraitId >= PORTRAIT_ID_FIRST_RESERVED)
    {
        CResRef cPortrait("");
        SetPortrait(
            pRes->ReadFieldCResRef(pStruct, "Portrait", bSuccess, cPortrait));
    }
    else
    {
        SetPortraitId(nPortraitId);
    }

    CExoString sScript;
    SetCreator(pRes->ReadFieldDWORD(pStruct, "CreatorId", bSuccess,
                                    OBJECT_INVALID));
    {
        CResRef cScript(*GetScriptName(SCRIPT_HEARTBEAT));
        pRes->ReadFieldCResRef(pStruct, "ScriptHeartbeat", bSuccess, cScript)
            .CopyToString(sScript);
        m_asScripts[SCRIPT_HEARTBEAT] = sScript;
    }
    {
        CResRef cScript(*GetScriptName(SCRIPT_ONENTER));
        pRes->ReadFieldCResRef(pStruct, "ScriptOnEnter", bSuccess, cScript)
            .CopyToString(sScript);
        m_asScripts[SCRIPT_ONENTER] = sScript;
    }
    {
        CResRef cScript(*GetScriptName(SCRIPT_ONEXIT));
        pRes->ReadFieldCResRef(pStruct, "ScriptOnExit", bSuccess, cScript)
            .CopyToString(sScript);
        m_asScripts[SCRIPT_ONEXIT] = sScript;
    }
    {
        CResRef cScript(*GetScriptName(SCRIPT_USERDEFINE));
        pRes->ReadFieldCResRef(pStruct, "ScriptUserDefine", bSuccess, cScript)
            .CopyToString(sScript);
        m_asScripts[SCRIPT_USERDEFINE] = sScript;
    }
    {
        CResRef cScript(*GetScriptName(SCRIPT_ONTRAPTRIGGERED));
        pRes->ReadFieldCResRef(pStruct, "OnTrapTriggered", bSuccess, cScript)
            .CopyToString(sScript);
        m_asScripts[SCRIPT_ONTRAPTRIGGERED] = sScript;
    }
    {
        CResRef cScript(*GetScriptName(SCRIPT_ONDISARM));
        pRes->ReadFieldCResRef(pStruct, "OnDisarm", bSuccess, cScript)
            .CopyToString(sScript);
        m_asScripts[SCRIPT_ONDISARM] = sScript;
    }

    {
        CResRef cScript(*GetScriptName(SCRIPT_ONENTER));
        pRes->ReadFieldCResRef(pStruct, "OnClick", bSuccess, cScript)
            .CopyToString(sScript);
        m_asScripts[SCRIPT_ONCLICK] = sScript;
    }

    m_nTrapType = pRes->ReadFieldBYTE(pStruct, "TrapType", bSuccess,
                                      GetTrapType());
    CExoString* pTrapScript = GetScriptName(SCRIPT_ONTRAPTRIGGERED);
    if (pTrapScript == NULL || pTrapScript->IsEmpty() ||
        *pTrapScript == "default")
    {
        CExoString sTrapScript;
        {
            CExoString sColumn("TrapScript");
            g_pRules->Get2DArrays()->m_pTraps->GetCExoStringEntry(
                GetTrapType(), sColumn, &sTrapScript);
        }
        m_asScripts[SCRIPT_ONTRAPTRIGGERED] = sTrapScript;
    }
    int bTrapOneShot = m_bTrapOneShot;
    m_bTrapOneShot = pRes->ReadFieldBYTE(pStruct, "TrapOneShot", bSuccess,
                                         bTrapOneShot);

    CExoString sLinkedTo;
    {
        CExoString sDefault("");
        sLinkedTo =
            pRes->ReadFieldCExoString(pStruct, "LinkedTo", bSuccess, sDefault);
    }
    if (bSuccess == 1)
    {
        m_sLinkedTo = sLinkedTo;
    }
    m_nLinkedToFlags = pRes->ReadFieldBYTE(pStruct, "LinkedToFlags", bSuccess,
                                           0);
    CResRef cLinkedToModule;
    {
        CResRef cDefault("");
        cLinkedToModule = pRes->ReadFieldCResRef(pStruct, "LinkedToModule",
                                                 bSuccess, cDefault);
    }
    if (bSuccess == 1)
    {
        m_sLinkedToModule = cLinkedToModule.GetResRefStr();
    }
    int bAutoRemoveKey = pRes->ReadFieldBYTE(pStruct, "AutoRemoveKey",
                                             bSuccess, 0);
    if (bSuccess == 1)
    {
        m_bAutoRemoveKey = bAutoRemoveKey;
    }

    CExoString sTag;
    {
        CExoString sDefault("");
        sTag = pRes->ReadFieldCExoString(pStruct, "Tag", bSuccess, sDefault);
    }
    if (bSuccess == 1)
    {
        SetTag(sTag);
    }
    m_lsName = pRes->ReadFieldCExoLocString(pStruct, "LocalizedName", bSuccess,
                                            CExoLocString());
    int nFaction = pRes->ReadFieldDWORD(pStruct, "Faction", bSuccess, 0);
    if (bSuccess != 0)
    {
        m_nFaction = nFaction;
    }
    {
        CExoString sDefault("");
        m_sKeyName =
            pRes->ReadFieldCExoString(pStruct, "KeyName", bSuccess, sDefault);
    }
    int bTrapDisarmable = pRes->ReadFieldBYTE(pStruct, "TrapDisarmable",
                                              bSuccess, 0);
    m_bTrapDisarmable = bTrapDisarmable;
    int bTrapDetectable = pRes->ReadFieldBYTE(pStruct, "TrapDetectable",
                                              bSuccess, 0);
    m_bTrapDetectable = bTrapDetectable;
    m_nOwnerDemolitionsSkill = pRes->ReadFieldINT(pStruct, "OwnerDemolitionsSkill", bSuccess, 0);

    int nDCMod;
    if (g_pRules->Get2DArrays()->m_pTraps->GetINTEntry(
            GetTrapType(), "DisarmDCMod", &nDCMod))
    {
        m_nTrapDisarmDC = nDCMod;
    }
    if (g_pRules->Get2DArrays()->m_pTraps->GetINTEntry(
            GetTrapType(), "DetectDCMod", &nDCMod))
    {
        m_nTrapDetectDC = nDCMod;
    }

    m_nCursor = pRes->ReadFieldBYTE(pStruct, "Cursor", bSuccess, 0);
    {
        CResRef cPortrait("");
        SetPortrait(
            pRes->ReadFieldCResRef(pStruct, "Portrait", bSuccess, cPortrait));
    }
    m_lsTransitionDestination = pRes->ReadFieldCExoLocString(
        pStruct, "TransitionDestination", bSuccess, m_lsTransitionDestination);

    int nType = pRes->ReadFieldINT(pStruct, "Type", bSuccess, 0);
    if (bSuccess == 1)
    {
        if (nType == 1)
        {
            m_nTriggerType = TRIGGER_TYPE_AREA_TRANSITION;
            m_nCursor = 1;
        }
        else if (nType == 2)
        {
            m_bMineTrigger = 1;
            m_nCursor = 0;
        }
    }

    float fHeight = pRes->ReadFieldFLOAT(pStruct, "HighlightHeight", bSuccess,
                                         0.1f);
    if (fHeight > 0.0f)
    {
        m_fHighlightHeight = fHeight;
    }

    Vector vPosition;
    int bPositionSuccess = 0;
    vPosition.m_fX = pRes->ReadFieldFLOAT(pStruct, "XPosition",
                                          bPositionSuccess, 0.0f);
    vPosition.m_fY = pRes->ReadFieldFLOAT(pStruct, "YPosition",
                                          bPositionSuccess, 0.0f);
    vPosition.m_fZ = pRes->ReadFieldFLOAT(pStruct, "ZPosition",
                                          bPositionSuccess, 0.0f);
    Vector vOrientation;
    int bOrientationSuccess = 0;
    vOrientation.m_fX = pRes->ReadFieldFLOAT(pStruct, "XOrientation",
                                             bOrientationSuccess, 0.0f);
    vOrientation.m_fY = pRes->ReadFieldFLOAT(pStruct, "YOrientation",
                                             bOrientationSuccess, 0.0f);
    vOrientation.m_fZ = pRes->ReadFieldFLOAT(pStruct, "ZOrientation",
                                             bOrientationSuccess, 0.0f);
    if (vOrientation.m_fX * vOrientation.m_fX +
            vOrientation.m_fY * vOrientation.m_fY +
            vOrientation.m_fZ * vOrientation.m_fZ !=
        1.0f)
    {
        vOrientation.Normalize();
    }

    if (bOrientationSuccess == 1)
    {
        Quaternion qOldOrientation =
            YawPitchRoll(Yaw(m_vOrientation), 0.0f, 0.0f);
        Quaternion qNewOrientation =
            YawPitchRoll(Yaw(vOrientation), 0.0f, 0.0f);
        if (m_nGeometryPointCount > 0)
        {
            for (int i = 0; i < m_nGeometryPointCount; i++)
            {
                m_pGeometry[i] =
                    vPosition + qNewOrientation * (-qOldOrientation *
                                                   (m_pGeometry[i] - m_vPosition));
            }
        }
    }

    if (bPositionSuccess != 0)
    {
        SetPosition(vPosition, 1, 1, 0);
    }
    if (bOrientationSuccess != 0)
    {
        SetOrientation(vOrientation);
    }

    CResList cGeometry;
    if (pRes->GetList(&cGeometry, pStruct, "Geometry"))
    {
        LoadTriggerGeometry(pRes, cGeometry);
    }
    m_nLoadScreenId = pRes->ReadFieldWORD(pStruct, "LoadScreenID", bSuccess, 0);
    int bSetByPlayerParty = m_bSetByPlayerParty;
    m_bSetByPlayerParty = pRes->ReadFieldBYTE(pStruct, "SetByPlayerParty",
                                              bSuccess, bSetByPlayerParty);

    g_pAppManager->GetServerExoApp()->GetModule()->AddObjectToLookupTable(
        m_sTag, GetId());
    return 1;
}
// PC 0x00585E40 GOG 0x00585EC0
int CSWSTrigger::SaveTrigger(CResGFF* pRes, CResStruct* pStruct)
{
    CResList cList;
    CResStruct stPoint;
    int nType;
    int i;
    float fHeight;
    Vector vVector;
    Vector vPosition;
    Vector vPoint;

    {
        CResRef cScript(*GetScriptName(SCRIPT_HEARTBEAT));
        pRes->WriteFieldCResRef(pStruct, cScript, "ScriptHeartbeat");
    }
    {
        CResRef cScript(*GetScriptName(SCRIPT_ONENTER));
        pRes->WriteFieldCResRef(pStruct, cScript, "ScriptOnEnter");
    }
    {
        CResRef cScript(*GetScriptName(SCRIPT_ONEXIT));
        pRes->WriteFieldCResRef(pStruct, cScript, "ScriptOnExit");
    }
    {
        CResRef cScript(*GetScriptName(SCRIPT_USERDEFINE));
        pRes->WriteFieldCResRef(pStruct, cScript, "ScriptUserDefine");
    }
    {
        CResRef cScript(*GetScriptName(SCRIPT_ONTRAPTRIGGERED));
        pRes->WriteFieldCResRef(pStruct, cScript, "OnTrapTriggered");
    }
    {
        CResRef cScript(*GetScriptName(SCRIPT_ONDISARM));
        pRes->WriteFieldCResRef(pStruct, cScript, "OnDisarm");
    }
    {
        CResRef cScript(*GetScriptName(SCRIPT_ONCLICK));
        pRes->WriteFieldCResRef(pStruct, cScript, "OnClick");
    }
    pRes->WriteFieldBYTE(pStruct, m_nTrapType, "TrapType");
    pRes->WriteFieldBYTE(pStruct, m_bTrapOneShot, "TrapOneShot");
    pRes->WriteFieldDWORD(pStruct, m_oidCreator, "CreatorId");
    pRes->WriteFieldCExoString(pStruct, m_sLinkedTo, "LinkedTo");
    pRes->WriteFieldBYTE(pStruct, m_nLinkedToFlags, "LinkedToFlags");
    CResRef cModule((char*)m_sLinkedToModule.CStr());
    pRes->WriteFieldCResRef(pStruct, cModule, "LinkedToModule");
    pRes->WriteFieldBYTE(pStruct, m_bAutoRemoveKey, "AutoRemoveKey");
    pRes->WriteFieldCExoString(pStruct, m_sTag, "Tag");
    pRes->WriteFieldCExoLocString(pStruct, m_lsName, "LocalizedName");
    pRes->WriteFieldDWORD(pStruct, m_nFaction, "Faction");
    pRes->WriteFieldBYTE(pStruct, m_nCursor, "Cursor");
    pRes->WriteFieldCExoString(pStruct, m_sKeyName, "KeyName");
    pRes->WriteFieldBYTE(pStruct, m_bTrapDisarmable, "TrapDisarmable");
    pRes->WriteFieldBYTE(pStruct, m_bTrapDetectable, "TrapDetectable");
    pRes->WriteFieldINT(pStruct, m_nOwnerDemolitionsSkill, "OwnerDemolitionsSkill");
    if ((unsigned short)GetPortraitId() == 0xFFFF)
    {
        pRes->WriteFieldCResRef(pStruct, GetPortrait(), "Portrait");
    }
    else
    {
        pRes->WriteFieldWORD(pStruct, GetPortraitId(), "PortraitId");
    }

    nType = 0;
    if (m_nTriggerType != 0)
    {
        nType = 1;
    }
    else if (m_bMineTrigger != 0)
    {
        nType = 2;
    }
    pRes->WriteFieldINT(pStruct, nType, "Type");
    fHeight = m_fHighlightHeight;
    pRes->WriteFieldFLOAT(pStruct, fHeight, "HighlightHeight");
    vVector = m_vPosition;
    pRes->WriteFieldFLOAT(pStruct, vVector.m_fX, "XPosition");
    pRes->WriteFieldFLOAT(pStruct, vVector.m_fY, "YPosition");
    pRes->WriteFieldFLOAT(pStruct, vVector.m_fZ, "ZPosition");
    vVector = m_vOrientation;
    pRes->WriteFieldFLOAT(pStruct, vVector.m_fX, "XOrientation");
    pRes->WriteFieldFLOAT(pStruct, vVector.m_fY, "YOrientation");
    pRes->WriteFieldFLOAT(pStruct, vVector.m_fZ, "ZOrientation");
    vPosition = m_vPosition;
    pRes->AddList(&cList, pStruct, "Geometry");
    for (i = 0; i < m_nGeometryPointCount; i++)
    {
        pRes->AddListElement(&stPoint, &cList, 3);
        vPoint = m_pGeometry[i] - vPosition;
        pRes->WriteFieldFLOAT(&stPoint, vPoint.m_fX, "PointX");
        pRes->WriteFieldFLOAT(&stPoint, vPoint.m_fY, "PointY");
        pRes->WriteFieldFLOAT(&stPoint, vPoint.m_fZ, "PointZ");
    }
    pRes->WriteFieldWORD(pStruct, m_nLoadScreenId, "LoadScreenID");
    pRes->WriteFieldCExoLocString(pStruct, m_lsTransitionDestination,
                                  "TransitionDestination");
    pRes->WriteFieldBYTE(pStruct, m_bSetByPlayerParty, "SetByPlayerParty");
    return 1;
}
// PC 0x00586620 GOG 0x005866A0
void CSWSTrigger::RemoveFromArea()
{
    NwstriggerTrace2();
    if (GetArea() != NULL)
    {
        GetArea()->RemoveObjectFromArea(m_nId);
        GetArea()->RemoveSubArea(this);
        if (GetArea()->m_lstTrapObjectIds.Contains(m_nId) == 1)
        {
            unsigned long oidObject = m_nId;
            CSWSArea* pTrapArea = GetArea();
            pTrapArea->m_lstTrapObjectIds.Remove(oidObject);
        }
    }
    SetArea(NULL);
    SetPosition(Vector(0.0f, 0.0f, 0.0f), 1, 1, 0);
}

CExoString* CSWSTrigger::GetScriptName(int nScript)
{
    if (nScript < 0 || nScript > SCRIPT_ONCLICK)
    {
        return NULL;
    }
    return &m_asScripts[nScript];
}
// PC 0x005841F0 GOG 0x00584270
int CSWSTrigger::InTrigger(const Vector& vPosition)
{
    if (m_bMineTrigger != 0)
    {
        Vector vOffset = vPosition - m_vPosition;
        if (vOffset * vOffset <= m_fTrapRadiusRange * m_fTrapRadiusRange)
        {
            return 1;
        }
        return 0;
    }

    int nIntersections = 0;
    int nPoints = m_nPolygonPointCount;
    for (int i = 0; i < nPoints; i++)
    {
        int nPoint = m_pPolygonPointOrder[i];
        int nNextPoint = i != nPoints - 1 ? i + 1 : 0;
        nNextPoint = m_pPolygonPointOrder[nNextPoint];
        Vector* pNextPoint = &m_pGeometry[nNextPoint];
        Vector* pPoint = &m_pGeometry[nPoint];

        if (pNextPoint->m_fX != pPoint->m_fX)
        {
            float fPointY = pPoint->m_fY;
            float fX = vPosition.m_fX;
            float fDeltaY = pNextPoint->m_fY - fPointY;
            float fDeltaX = pNextPoint->m_fX - pPoint->m_fX;
            float fY = fPointY -
                (pPoint->m_fX - vPosition.m_fX) * (fDeltaY / fDeltaX);
            if (fY > vPosition.m_fY)
            {
                if (fabs(fDeltaY) > fabs(fDeltaX))
                {
                    if (pNextPoint->m_fY > pPoint->m_fY)
                    {
                        if (fY >= pPoint->m_fY && fY < pNextPoint->m_fY)
                        {
                            nIntersections++;
                        }
                    }
                    else if (pPoint->m_fY > pNextPoint->m_fY &&
                             fY > pNextPoint->m_fY && fY <= pPoint->m_fY)
                    {
                        nIntersections++;
                    }
                }
                else
                {
                    if (pNextPoint->m_fX > pPoint->m_fX)
                    {
                        if (fX >= pPoint->m_fX && fX < pNextPoint->m_fX)
                        {
                            nIntersections++;
                        }
                    }
                    else if (pPoint->m_fX > pNextPoint->m_fX &&
                             fX > pNextPoint->m_fX && fX <= pPoint->m_fX)
                    {
                        nIntersections++;
                    }
                }
            }
        }
        if (pNextPoint->m_fX == vPosition.m_fX &&
            (pNextPoint->m_fY > vPosition.m_fY ||
             pPoint->m_fY >= vPosition.m_fY))
        {
            nIntersections++;
        }
    }
    return nIntersections % 2 == 1;
}
// PC 0x00584410 GOG 0x00584490
void CSWSTrigger::LoadTriggerGeometry(CResGFF* pRes, CResList& list)
{
    CResStruct stPoint;
    int bSuccess;
    int i;

    m_nGeometryPointCount = pRes->GetListCount(&list);
    m_nPolygonPointCount = m_nGeometryPointCount;
    if (m_pGeometry != NULL)
    {
        delete[] m_pGeometry;
        m_pGeometry = NULL;
    }
    if (m_pPolygonPointOrder != NULL)
    {
        delete[] m_pPolygonPointOrder;
        m_pPolygonPointOrder = NULL;
    }

    m_pGeometry = new Vector[m_nGeometryPointCount];
    m_pPolygonPointOrder = new unsigned long[m_nGeometryPointCount];
    for (i = 0; i < m_nGeometryPointCount; i++)
    {
        if (pRes->GetListElement(&stPoint, &list, i) != 0 &&
            pRes->GetElementType(&stPoint) == 3)
        {
            Vector vPoint;

            vPoint.m_fX =
                pRes->ReadFieldFLOAT(&stPoint, "PointX", bSuccess, 0.0f);
            vPoint.m_fY =
                pRes->ReadFieldFLOAT(&stPoint, "PointY", bSuccess, 0.0f);
            vPoint.m_fZ =
                pRes->ReadFieldFLOAT(&stPoint, "PointZ", bSuccess, 0.0f);
            m_pGeometry[i] = vPoint + m_vPosition;
            m_pPolygonPointOrder[i] = i;
        }
    }
}

static inline CServerExoApp* GetServerExoApp()
{
    return g_pAppManager->m_pServerExoApp;
}

static inline void NwstriggerTrace2()
{
    NwstriggerTrace1();
}

static inline void NwstriggerTrace1()
{
}
