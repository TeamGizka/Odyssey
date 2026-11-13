#include "nwsspellimpact.h"

#include "appmanager.h"
#include "gameobjectarray.h"
#include "math3d.h"
#include "nwsarea.h"
#include "nwscript.h"
#include "nwscreaturestats.h"
#include "nwsobject.h"
#include "nwsmodule.h"
#include "nwrules.h"
#include "nwsscriptevent.h"
#include "nwspell.h"
#include "resgff.h"
#include "serveraimaster.h"
#include "serverexoapp.h"
#include "twodimarray.h"
#include "twodimarrays.h"
#include "worldtimer.h"

#include <math.h>

static inline void NwsspellimpactTrace1();
#define NWSSPELLIMPACT_TRACE() NwsspellimpactTrace1()
// PC 0x005718F0 GOG 0x00571A20
CSWSAreaOfEffectObject::CSWSAreaOfEffectObject(unsigned long oidObject)
    : CSWSObject(SW_OBJECT_TYPE_AREA_OF_EFFECT, oidObject, 0)
{
    unsigned long nCalendarDay;
    unsigned long nTimeOfDay;

    m_oidTarget = CGameObjectArray::INVALIDOBJID;
    m_oidLastEntered = CGameObjectArray::INVALIDOBJID;
    m_oidLastLeft = CGameObjectArray::INVALIDOBJID;
    m_oidCreator = CGameObjectArray::INVALIDOBJID;
    SetPosition(Vector(), 1, 1, 0);
    m_fRadius = 0.0f;
    m_nLastHeartbeatTime = 0;
    m_nLastHeartbeatDay = 0;
    m_nShape = SHAPE_CIRCLE;
    m_pShapeVertices = NULL;
    m_EffectSpellId = -1;

    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    pServer->GetWorldTimer()->GetWorldTime(&nCalendarDay, &nTimeOfDay);
    m_nLastUpdateDay = nCalendarDay;
    m_nLastUpdateTime = nTimeOfDay;
    m_nDuration = 0;
    m_nDurationType = DURATION_TYPE_PERMANENT;
    g_pAppManager->m_pServerExoApp->GetServerAIMaster()->AddObject(
        this, CServerAIMaster::AI_LEVEL_AREA_OF_EFFECT);
    m_nSpellLevel = 0;
    m_nSpellSaveDC = DEFAULT_SPELL_SAVE_DC;
}

static Vector* AllocateVectorArray(int nCount)
{
    return new Vector[nCount];
}
// PC 0x00571A60 GOG 0x00571B90
void CSWSAreaOfEffectObject::SetShape(unsigned char nShape, float fWidth,
                                      float fLength)
{
    float fHalfWidth;
    float fHalfLength;

    m_nShape = nShape;

    switch (nShape)
    {
    case 0:
        m_fRadius = fWidth;
        break;

    case 1:
        m_fWidth = fWidth;
        m_fLength = fLength;
        fHalfWidth = fWidth * 0.5f;
        fHalfLength = fLength * 0.5f;
        m_fRadius =
            (float)sqrt(fHalfLength * fHalfLength + fHalfWidth * fHalfWidth);
        m_pShapeVertices = new Vector[4];
        m_oidTarget = OBJECT_INVALID;
        break;
    }
}
// PC 0x00571C20 GOG 0x00571D50
void CSWSAreaOfEffectObject::LoadAreaEffect(int nAreaEffectId)
{
    NWSSPELLIMPACT_TRACE();
    m_nAreaEffectId = nAreaEffectId;
    CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
    C2DA* pAreaEffects = p2DArrays->m_pVisualEffectsPersistent;

    CExoString sShape;
    pAreaEffects->GetCExoStringEntry(m_nAreaEffectId, "SHAPE", &sShape);
    if (sShape == "C")
    {
        float fRadius;
        pAreaEffects->GetFLOATEntry(m_nAreaEffectId, "RADIUS", &fRadius);
        SetShape(SHAPE_CIRCLE, fRadius, 0.0f);
    }
    else if (sShape == "R")
    {
        float fWidth;
        float fLength;
        pAreaEffects->GetFLOATEntry(m_nAreaEffectId, "WIDTH", &fWidth);
        pAreaEffects->GetFLOATEntry(m_nAreaEffectId, "LENGTH", &fLength);
        SetShape(SHAPE_RECTANGLE, fWidth, fLength);
    }

    CExoString sOnEnter;
    pAreaEffects->GetCExoStringEntry(m_nAreaEffectId, "ONENTER", &sOnEnter);
    m_asScripts[SCRIPT_ON_ENTER] = sOnEnter;

    CExoString sHeartbeat;
    pAreaEffects->GetCExoStringEntry(m_nAreaEffectId, "HEARTBEAT", &sHeartbeat);
    m_asScripts[SCRIPT_HEARTBEAT] = sHeartbeat;

    CExoString sOnExit;
    pAreaEffects->GetCExoStringEntry(m_nAreaEffectId, "ONEXIT", &sOnExit);
    m_asScripts[SCRIPT_ON_EXIT] = sOnExit;

    CExoString sLabel;
    pAreaEffects->GetCExoStringEntry(m_nAreaEffectId, "LABEL", &sLabel);
    SetTag(sLabel);
    g_pAppManager->m_pServerExoApp->GetModule()->AddObjectToLookupTable(
        m_sTag, m_nId);
}
// PC 0x00571F70 GOG 0x005720A0
int CSWSAreaOfEffectObject::LoadEffect(CResGFF* pRes, CResStruct* pStruct)
{
    NWSSPELLIMPACT_TRACE();
    int bSuccess;
    unsigned char nShape;
    float fRadius;
    float fWidth;
    float fLength;

    SetTag(pRes->ReadFieldCExoString(pStruct, "Tag", bSuccess, ""));

    m_nAreaEffectId = pRes->ReadFieldINT(pStruct, "AreaEffectId", bSuccess, 0);
    m_EffectSpellId = pRes->ReadFieldDWORD(pStruct, "SpellId", bSuccess, 0);
    nShape = pRes->ReadFieldBYTE(pStruct, "Shape", bSuccess, 0);
    m_c1D5 = pRes->ReadFieldBYTE(pStruct, "MetaMagicType", bSuccess, 0);
    m_nSpellSaveDC = pRes->ReadFieldINT(pStruct, "SpellSaveDC", bSuccess, 0);
    m_nSpellLevel = pRes->ReadFieldINT(pStruct, "SpellLevel", bSuccess, 0);

    if (nShape == 0)
    {
        fRadius = pRes->ReadFieldFLOAT(pStruct, "Radius", bSuccess, 0.0f);
        SetShape(SHAPE_CIRCLE, fRadius, 0.0f);
    }
    else if (nShape == 1)
    {
        fLength = pRes->ReadFieldFLOAT(pStruct, "Length", bSuccess, 0.0f);
        fWidth = pRes->ReadFieldFLOAT(pStruct, "Width", bSuccess, 0.0f);
        SetShape(SHAPE_RECTANGLE, fWidth, fLength);
    }

    m_oidCreator = pRes->ReadFieldDWORD(pStruct, "CreatorId", bSuccess, 0);
    m_oidTarget = pRes->ReadFieldDWORD(pStruct, "LinkedToObject", bSuccess, 0);
    m_oidLastEntered =
        pRes->ReadFieldDWORD(pStruct, "LastEntered", bSuccess, 0);
    m_oidLastLeft = pRes->ReadFieldDWORD(pStruct, "LastLeft", bSuccess, 0);
    m_nDuration = pRes->ReadFieldDWORD(pStruct, "Duration", bSuccess, 0);
    m_nDurationType = pRes->ReadFieldBYTE(pStruct, "DurationType", bSuccess, 0);
    m_nLastHeartbeatDay =
        pRes->ReadFieldDWORD(pStruct, "LastHrtbtDay", bSuccess, 0);
    m_nLastHeartbeatTime =
        pRes->ReadFieldDWORD(pStruct, "LastHrtbtTime", bSuccess, 0);

    return 1;
}
// PC 0x005721F0 GOG 0x00572320
int CSWSAreaOfEffectObject::SaveEffect(CResGFF* pRes, CResStruct* pStruct)
{
    Vector vPosition;
    Vector vOrientation;

    pRes->WriteFieldCExoString(pStruct, m_sTag, "Tag");
    pRes->WriteFieldINT(pStruct, m_nAreaEffectId, "AreaEffectId");
    pRes->WriteFieldDWORD(pStruct, GetEffectSpellId(), "SpellId");
    pRes->WriteFieldBYTE(pStruct, m_nShape, "Shape");
    pRes->WriteFieldBYTE(pStruct, m_c1D5, "MetaMagicType");
    pRes->WriteFieldINT(pStruct, m_nSpellSaveDC, "SpellSaveDC");
    pRes->WriteFieldINT(pStruct, m_nSpellLevel, "SpellLevel");

    if (m_nShape == SHAPE_CIRCLE)
    {
        pRes->WriteFieldFLOAT(pStruct, GetRadius(), "Radius");
    }
    else if (m_nShape == SHAPE_RECTANGLE)
    {
        pRes->WriteFieldFLOAT(pStruct, m_fLength, "Length");
        pRes->WriteFieldFLOAT(pStruct, m_fWidth, "Width");
    }

    pRes->WriteFieldDWORD(pStruct, m_oidCreator, "CreatorId");
    pRes->WriteFieldDWORD(pStruct, m_oidTarget, "LinkedToObject");
    pRes->WriteFieldDWORD(pStruct, m_oidLastEntered, "LastEntered");
    pRes->WriteFieldDWORD(pStruct, m_oidLastLeft, "LastLeft");
    pRes->WriteFieldDWORD(pStruct, m_nDuration, "Duration");
    pRes->WriteFieldBYTE(pStruct, m_nDurationType, "DurationType");
    pRes->WriteFieldDWORD(pStruct, m_nLastHeartbeatDay, "LastHrtbtDay");
    pRes->WriteFieldDWORD(pStruct, m_nLastHeartbeatTime, "LastHrtbtTime");
    pRes->WriteFieldCExoString(pStruct, m_asScripts[0], "OnHeartbeat");
    pRes->WriteFieldCExoString(pStruct, m_asScripts[1], "OnUserDefined");
    pRes->WriteFieldCExoString(pStruct, m_asScripts[2], "OnObjEnter");
    pRes->WriteFieldCExoString(pStruct, m_asScripts[3], "OnObjExit");

    vPosition = GetPosition();
    pRes->WriteFieldFLOAT(pStruct, vPosition.m_fX, "PositionX");
    pRes->WriteFieldFLOAT(pStruct, vPosition.m_fY, "PositionY");
    pRes->WriteFieldFLOAT(pStruct, vPosition.m_fZ, "PositionZ");

    vOrientation = m_vOrientation;
    pRes->WriteFieldFLOAT(pStruct, vOrientation.m_fX, "OrientationX");
    pRes->WriteFieldFLOAT(pStruct, vOrientation.m_fY, "OrientationY");
    pRes->WriteFieldFLOAT(pStruct, vOrientation.m_fZ, "OrientationZ");

    return 1;
}
// PC 0x00572460 GOG 0x00572590
void CSWSAreaOfEffectObject::SetCreator(unsigned long oidCreator)
{
    NWSSPELLIMPACT_TRACE();
    CSWSCreature* pCreator;
    CSWSpell* pSpell;

    m_oidCreator = oidCreator;

    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    pCreator = pServer->GetCreatureByGameObjectID(oidCreator);
    if (pCreator != NULL && pCreator->GetEffectSpellId() != -1)
    {
        m_c1D5 = pCreator->m_c1D5;
        m_nSpellSaveDC =
            pCreator->CalculateSpellSaveDC(pCreator->GetEffectSpellId());

        if (pCreator->m_c1D4 != CASTING_CLASS_INNATE_FF &&
            pCreator->m_c1D4 != CASTING_CLASS_INNATE_FE)
        {
            unsigned char nClass = pCreator->m_c1D4;
            m_nSpellLevel = pCreator->m_pStats->GetClassLevel(nClass, 1);
        }
        else
        {
            pSpell = g_pRules->m_pSpellArray->GetSpell(
                pCreator->GetEffectSpellId());
            if (pSpell != NULL)
            {
                m_nSpellLevel = pSpell->m_nInnateLevel * 2 - 1;
            }
        }
    }
}
// PC 0x00572530 GOG 0x00572660
int CSWSAreaOfEffectObject::LineSegmentIntersectAreaOfEffect(
    Vector vStart, Vector vEnd)
{
    if (m_nShape == SHAPE_CIRCLE)
    {
        if (vEnd.m_fX == vStart.m_fX)
        {
            float fOffset = GetPosition().m_fX - vEnd.m_fX;
            if (-m_fRadius > fOffset || fOffset > m_fRadius)
                return 0;
            float fMaximum = vStart.m_fY > vEnd.m_fY ?
                vStart.m_fY : vEnd.m_fY;
            float fMinimum = vStart.m_fY < vEnd.m_fY ?
                vStart.m_fY : vEnd.m_fY;
            float fHalfHeight = (float)sqrt(m_fRadius * m_fRadius -
                                           fOffset * fOffset);
            if (GetPosition().m_fY - fHalfHeight <= fMinimum &&
                GetPosition().m_fY - fHalfHeight >= fMaximum)
                return 1;
            if (fHalfHeight + GetPosition().m_fY <= fMinimum &&
                fHalfHeight + GetPosition().m_fY >= fMaximum)
                return 1;
            return 0;
        }

        float fSlope = (vEnd.m_fY - vStart.m_fY) /
                       (vEnd.m_fX - vStart.m_fX);
        float fIntercept = vEnd.m_fY - fSlope * vEnd.m_fX;
        float fA = fSlope * fSlope + 1.0f;
        float fB = 2.0f * ((fIntercept - GetPosition().m_fY) * fSlope -
                          GetPosition().m_fX);
        float fC = (GetPosition().m_fX * GetPosition().m_fX) -
                   (fIntercept - GetPosition().m_fY) *
                       (fIntercept - GetPosition().m_fY) -
                   m_fRadius * m_fRadius;
        float fDiscriminant = fB * fB - fC * fA * 4.0f;
        if (fDiscriminant < 0.0f)
            return 0;
        float fRoot = sqrtf(fDiscriminant);
        float fFirstX = (float)((-fB - fRoot) / (fA + fA));
        float fSecondX = (float)((fRoot - fB) / (fA + fA));
        float fMaximum = vStart.m_fX > vEnd.m_fX ? vStart.m_fX : vEnd.m_fX;
        float fMinimum = vStart.m_fX < vEnd.m_fX ? vStart.m_fX : vEnd.m_fX;
        if (fMinimum >= fFirstX && fMaximum <= fFirstX)
            return 1;
        if (fMinimum >= fSecondX && fMaximum <= fSecondX)
            return 1;
        return 0;
    }

    CSWSArea* pArea = GetArea();
    if (pArea == NULL)
        return 0;
    for (int nCorner = 0; nCorner < 4; nCorner++)
    {
        int nNext = nCorner != 3 ? nCorner + 1 : 0;
        float fHitX, fHitY;
        if (pArea->IntersectLineSegments(
                vStart.m_fX, vStart.m_fY, vEnd.m_fX, vEnd.m_fY,
                m_pShapeVertices[nCorner].m_fX,
                m_pShapeVertices[nCorner].m_fY,
                m_pShapeVertices[nNext].m_fX,
                m_pShapeVertices[nNext].m_fY, &fHitX, &fHitY) == 1)
            return 1;
    }
    return 0;
}
// PC 0x00572990 GOG 0x00572AC0
inline void CSWSCreature::AddToAreaOfEffectObjects(unsigned long oidObject)
{
    m_lstAreaOfEffectObjects.Add(oidObject);
}
// PC 0x005729D0 GOG 0x00572B00
void CSWSAreaOfEffectObject::SetTargetObjID(unsigned long oidTarget)
{
    CGameObject* pObject;

    if (m_oidTarget != CGameObjectArray::INVALIDOBJID)
    {
        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
        if (pServer->GetObjectArray()->GetGameObject(
                m_oidTarget, &pObject) == CGameObjectArray::SUCCESS &&
            pObject != NULL)
        {
            unsigned long oidSelf = m_nId;
            CSWSCreature* pCreature = pObject->AsSWSCreature();
            pCreature->m_lstAreaOfEffectObjects.Remove(oidSelf);
        }
    }

    m_oidTarget = oidTarget;
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    if (pServer->GetObjectArray()->GetGameObject(
            oidTarget, &pObject) == CGameObjectArray::SUCCESS &&
        pObject != NULL)
    {
        unsigned long oidSelf = m_nId;
        pObject->AsSWSCreature()->AddToAreaOfEffectObjects(oidSelf);
    }
}
// PC 0x00573140 GOG 0x00573270
void CSWSAreaOfEffectObject::AIUpdate()
{
    unsigned __int64 nTimer;
    unsigned long nNowDay;
    unsigned long nNowTime;
    unsigned long nElapsedDay;
    unsigned long nElapsedTime;
    unsigned long nHeartbeatDay;
    unsigned long nHeartbeatTime;

    nTimer = g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
    {
        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
        pServer->GetWorldTimer()->GetWorldTime(
            &nNowDay, &nNowTime);
    }
    {
        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
        pServer->GetWorldTimer()->SubtractWorldTimes(
            nNowDay, nNowTime, m_nLastUpdateDay, m_nLastUpdateTime,
            &nElapsedDay, &nElapsedTime);
    }
    {
        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
        pServer->GetWorldTimer()->SubtractWorldTimes(
            nNowDay, nNowTime, m_nLastHeartbeatDay, m_nLastHeartbeatTime,
            &nHeartbeatDay, &nHeartbeatTime);
    }

    m_nLastUpdateDay = nNowDay;
    m_nLastUpdateTime = nNowTime;
    m_nElapsedTime = nElapsedTime;

    if (nHeartbeatTime >= HEARTBEAT_INTERVAL)
    {
        if (m_nLastHeartbeatTime != 0)
        {
            g_pVirtualMachine->RunScript(&m_asScripts[SCRIPT_HEARTBEAT], m_nId, 1);
        }
        m_nLastHeartbeatTime = nNowTime;
        m_nLastHeartbeatDay = nNowDay;
    }

    if (m_oidTarget != CGameObjectArray::INVALIDOBJID &&
        m_nShape == SHAPE_CIRCLE)
    {
        CGameObject* pObject;
        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
        if (pServer->GetObjectArray()->GetGameObject(
                m_oidTarget, &pObject) == CGameObjectArray::SUCCESS &&
            pObject != NULL)
        {
            Vector vPosition = ((CSWSObject*)pObject)->m_vPosition;
            if (GetArea() != NULL)
            {
                UpdateSubAreas(&vPosition);
            }
            SetPosition(vPosition, 1, 1, 0);
        }
        else
        {
            unsigned long oidSelf = m_nId;
            CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
            pServer->GetServerAIMaster()->
                AddEventDeltaTime(0, 0, oidSelf, oidSelf,
                                  CServerAIMaster::EVENT_DESTROY_OBJECT, NULL);
        }
    }

    if (m_nDurationType == DURATION_TYPE_TEMPORARY)
    {
        if (m_nDuration <= nElapsedTime)
        {
            CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
            pServer->GetServerAIMaster()->
                AddEventDeltaTime(0, 0, m_nId, m_nId,
                                  CServerAIMaster::EVENT_DESTROY_OBJECT, NULL);
            return;
        }
        m_nDuration -= nElapsedTime;
    }

    RunActions(nNowDay, nNowTime, nTimer);
}
// PC 0x00573360 GOG 0x00573490
void CSWSAreaOfEffectObject::AddToArea(CSWSArea* pArea, float fX, float fY,
                                      float fZ, int bLoadingGame)
{
    if (pArea != NULL)
    {
        SetArea(pArea);
        SetPosition(Vector(fX, fY, fZ), 0, 1, 0);

        if (m_nShape == SHAPE_RECTANGLE && m_pShapeVertices != NULL)
        {
            Vector vForward = normalize(m_vOrientation);
            Vector vSide(-vForward.m_fY, vForward.m_fX, vForward.m_fZ);

            float fWidth = m_fWidth;
            m_pShapeVertices[0] = vForward * m_fLength + vSide * fWidth;
            m_pShapeVertices[1] = vForward * m_fLength - vSide * m_fWidth;
            m_pShapeVertices[2] = -vForward * m_fLength - vSide * m_fWidth;
            m_pShapeVertices[3] = -vForward * m_fLength + vSide * m_fWidth;

            for (int nCorner = 0; nCorner < 4; nCorner++)
            {
                m_pShapeVertices[nCorner] =
                    m_pShapeVertices[nCorner] / 2.0f + GetPosition();
            }
        }

        GetArea()->AddObjectToArea(m_nId, bLoadingGame);
        GetArea()->AddSubArea(this);
        SetAnimation(1);
        UpdateSubAreas(NULL);
    }
}
// PC 0x00572A70 GOG 0x00572BA0
void CSWSAreaOfEffectObject::UpdateSubAreas(Vector* pvLastPosition)
{
    if (GetArea() == NULL)
    {
        return;
    }

    CSWSArea* pArea = GetArea();
    CGameObjectArray* pObjectArray =
        g_pAppManager->m_pServerExoApp->GetObjectArray();
    int nStartIndex = 0;
    float fMinX = GetPosition().m_fX;
    float fMaxX = fMinX;

    if (pvLastPosition != NULL)
    {
        fMinX = min(GetPosition().m_fX, pvLastPosition->m_fX);
        fMaxX = max(GetPosition().m_fX, pvLastPosition->m_fX);
    }

    GetArea()->GetFirstObjectIndiceByX(&nStartIndex, fMinX - m_fRadius);

    for (int nObject = nStartIndex; nObject < pArea->m_lstObjectIds.num;
         nObject++)
    {
        CGameObject* pObject;
        if (pObjectArray->GetGameObject(pArea->m_lstObjectIds[nObject],
                                        &pObject) == CGameObjectArray::SUCCESS &&
            pObject->AsSWSObject() != NULL)
        {
            if (pObject->m_nObjectType == SW_OBJECT_TYPE_CREATURE)
            {
                CSWSCreature* pCreature = pObject->AsSWSCreature();
                unsigned long oidSelf = m_nId;
                int nInSubArea = pCreature->m_lstSubAreas.Contains(oidSelf);

                int bInside;
                if (m_nShape == SHAPE_CIRCLE)
                {
                    if (pvLastPosition != NULL)
                    {

                        Vector vClosest;
                        vClosest = segmentproject(
                            GetPosition(), *pvLastPosition,
                            pCreature->m_vPosition + GetPosition());
                        Vector vOffset = pCreature->m_vPosition - vClosest;
                        float fDistance = vOffset * vOffset;
                        float fRadius = m_fRadius;
                        if (fDistance <= fRadius * fRadius)
                        {
                            Vector vLast =
                                pCreature->m_vPosition - *pvLastPosition;
                            if (vLast * vLast > m_fRadius * m_fRadius &&
                                nInSubArea == 0)
                            {

                                pCreature->m_lstSubAreas.AddUnique(m_nId);
                                CScriptEvent* pEvent = new CScriptEvent;
                                pEvent->m_nType =
                                    CSWSSCRIPTEVENT_EVENTTYPE_ON_OBJECT_ENTER;
                                pEvent->SetObjectID(0, pCreature->GetId());
                                unsigned long oidCreature = pCreature->m_nId;
                                CAppManager* pAppManager = g_pAppManager;
                                pAppManager->m_pServerExoApp->GetServerAIMaster()->
                                    AddEventDeltaTime(0, 0, oidCreature, m_nId,
                                                      CServerAIMaster::EVENT_SIGNAL_EVENT,
                                                      pEvent);
                                nInSubArea = 1;
                                bInside = FALSE;
                            }
                            else
                            {
                                bInside = TRUE;
                            }
                        }
                        else
                        {
                            bInside = FALSE;
                        }
                    }
                    else
                    {
                        Vector vOffset =
                            pCreature->m_vPosition - GetPosition();
                        float fDistance = vOffset * vOffset;
                        if (!(fDistance <= m_fRadius * m_fRadius))
                        {
                            bInside = FALSE;
                        }
                        else
                        {
                            bInside = TRUE;
                        }
                    }
                }
                else if (InAreaOfEffect(pCreature->m_vPosition) == 1)
                {
                    bInside = TRUE;
                }
                else
                {
                    bInside = FALSE;
                }

                if (bInside && nInSubArea == 0)
                {
                    pCreature->m_lstSubAreas.AddUnique(GetId());
                    CScriptEvent* pEvent = new CScriptEvent;
                    pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_OBJECT_ENTER;
                    pEvent->SetObjectID(0, pCreature->m_nId);
                    unsigned long oidCreature = pCreature->GetId();
                    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
                    pServer->GetServerAIMaster()->
                        AddEventDeltaTime(0, 0, oidCreature, GetId(),
                                          CServerAIMaster::EVENT_SIGNAL_EVENT,
                                          pEvent);
                }

                if (!bInside && nInSubArea != 0)
                {
                    pCreature->m_lstSubAreas.Remove(m_nId);
                    CScriptEvent* pEvent = new CScriptEvent;
                    pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_OBJECT_EXIT;
                    pEvent->SetObjectID(0, pCreature->m_nId);
                    unsigned long oidCreature = pCreature->m_nId;
                    CAppManager* pAppManager = g_pAppManager;
                    pAppManager->GetServerExoApp()->GetServerAIMaster()->
                        AddEventDeltaTime(0, 0, oidCreature, m_nId,
                                          CServerAIMaster::EVENT_SIGNAL_EVENT,
                                          pEvent);
                }

                if (bInside)
                {
                    continue;
                }
            }

            if (pObject->AsSWSObject()->m_vPosition.m_fX - fMaxX > m_fRadius)
            {
                break;
            }
        }
    }
}
// PC 0x00573700 GOG 0x00573830
void CSWSAreaOfEffectObject::RemoveFromArea()
{
    if (GetArea() != NULL)
    {
        RemoveFromSubAreas(1);
        GetArea()->RemoveObjectFromArea(m_nId);
        GetArea()->RemoveSubArea(this);
    }

    SetArea(NULL);
    SetPosition(Vector(), 1, 1, 0);
    SetAnimation(0);
}
// PC 0x00573790 GOG 0x005738C0
void CSWSAreaOfEffectObject::JumpToPoint(CSWSArea* pArea,
                                        const Vector& vPosition)
{
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    CSWSCreature* pCreature =
        pServer->GetCreatureByGameObjectID(m_oidTarget);

    if (pCreature != NULL)
    {
        if (GetArea() == pArea)
        {
            pCreature->m_lstSubAreas.Remove(m_nId);
            RemoveFromSubAreas(0);
            SetPosition(vPosition, 1, 1, 0);
            pCreature->m_lstSubAreas.AddUnique(m_nId);
            UpdateSubAreas(NULL);
        }
    }
}
// PC 0x00573800 GOG 0x00573930
CSWSAreaOfEffectObject::~CSWSAreaOfEffectObject()
{
    CGameObject* pObject;

    if (m_oidTarget != CGameObjectArray::INVALIDOBJID)
    {
        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
        if (pServer->GetObjectArray()->GetGameObject(
                m_oidTarget, &pObject) == CGameObjectArray::SUCCESS)
        {
            unsigned long oidSelf = m_nId;
            CSWSCreature* pCreature = pObject->AsSWSCreature();
            pCreature->m_lstAreaOfEffectObjects.Remove(oidSelf);
        }
    }

    RemoveFromArea();
    if (m_pShapeVertices != NULL)
    {
        delete [] m_pShapeVertices;
    }

    g_pAppManager->m_pServerExoApp->GetServerAIMaster()->RemoveObject(this);
    CSWSModule* pModule = g_pAppManager->m_pServerExoApp->GetModule();
    if (pModule != NULL)
    {
        unsigned long oidSelf = m_nId;
        pModule->RemoveObjectFromLookupTable(m_sTag, oidSelf);
    }
}
// PC 0x00573910 GOG 0x00573A40
void CSWSAreaOfEffectObject::EventHandler(unsigned long nEventId,
                                          unsigned long oidCaller,
                                          void* pEventData,
                                          unsigned long nCalendarDay,
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
        CScriptEvent* pEvent = (CScriptEvent*)pEventData;
        if (pEvent->m_nType == CSWSSCRIPTEVENT_EVENTTYPE_ON_OBJECT_ENTER)
        {
            m_oidLastEntered = pEvent->GetObjectID(0);
            g_pVirtualMachine->RunScript(&m_asScripts[2], m_nId, 1);
        }
        else if (pEvent->m_nType == CSWSSCRIPTEVENT_EVENTTYPE_ON_OBJECT_EXIT)
        {
            m_oidLastLeft = pEvent->GetObjectID(0);
            g_pVirtualMachine->RunScript(&m_asScripts[3], m_nId, 1);
        }
        delete pEvent;
    }
    else if (nEventId == CServerAIMaster::EVENT_DESTROY_OBJECT)
    {
        if (m_nEC != 0)
        {
            RemoveFromArea();
            delete this;
        }
    }
}
// PC 0x00572F90 GOG 0x005730C0
void CSWSAreaOfEffectObject::RemoveFromSubAreas(int bNotify)
{
    CSWSArea* pArea = GetArea();
    CGameObjectArray* pObjectArray =
        g_pAppManager->m_pServerExoApp->GetObjectArray();
    CGameObject* pObject;

    for (int nObject = 0; nObject < pArea->m_lstObjectIds.num; nObject++)
    {
        if (pObjectArray->GetGameObject(pArea->m_lstObjectIds[nObject],
                                        &pObject) == CGameObjectArray::SUCCESS &&
            pObject->m_nObjectType == SW_OBJECT_TYPE_CREATURE)
        {
            CSWSCreature* pCreature = pObject->AsSWSCreature();
            if (InAreaOfEffect(pCreature->m_vPosition))
            {
                unsigned long oidSelf = m_nId;
                if (pCreature->m_lstSubAreas.Contains(oidSelf))
                {
                    pCreature->m_lstSubAreas.Remove(oidSelf);
                    CScriptEvent* pEvent = new CScriptEvent;
                    pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_OBJECT_EXIT;
                    pEvent->SetObjectID(0, pCreature->m_nId);
                    if (!bNotify)
                    {
                        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
                        unsigned long oidCreature = pCreature->m_nId;
                        pServer->GetServerAIMaster()->
                            AddEventDeltaTime(0, 0, oidCreature,
                                              m_nId,
                                              CServerAIMaster::EVENT_SIGNAL_EVENT,
                                              pEvent);
                    }
                    else
                    {
                        EventHandler(CServerAIMaster::EVENT_SIGNAL_EVENT,
                                     pCreature->m_nId, pEvent, 0, 0);
                    }
                }
            }
        }
    }
}
// PC 0x005728C0 GOG 0x005729F0
int CSWSAreaOfEffectObject::InAreaOfEffect(Vector vPosition)
{
    Vector vOffset = vPosition - GetPosition();
    int nCorner;
    int nNext;

    if (vOffset * vOffset <= m_fRadius * m_fRadius)
    {
        if (m_nShape == SHAPE_CIRCLE)
        {
            return 1;
        }

        for (nCorner = 0; nCorner < 4; nCorner++)
        {
            nNext = nCorner == 3 ? 0 : nCorner + 1;
            Vector vToPoint = vPosition - m_pShapeVertices[nCorner];
            Vector vEdge = m_pShapeVertices[nNext] - m_pShapeVertices[nCorner];
            if (cross(vEdge, vToPoint).m_fZ > 0.0f)
            {
                return 0;
            }
        }

        return 1;
    }

    return 0;
}
// PC 0x00571B50 GOG 0x00571C80
Vector CSWSAreaOfEffectObject::GetPosition()
{
    CGameObject* pObject;

    if (m_oidTarget != CGameObjectArray::INVALIDOBJID && m_nShape == SHAPE_CIRCLE)
    {
        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
        if (pServer->GetObjectArray()->GetGameObject(m_oidTarget, &pObject) ==
                CGameObjectArray::SUCCESS && pObject != NULL)
        {
            return pObject->AsSWSCreature()->m_vPosition;
        }
    }

    return GetPosition();
}
// PC 0x00571BE0 GOG 0x00571D10
void CSWSAreaOfEffectObject::SetDuration(unsigned char nDurationType,
                                         float fDuration)
{
    m_nDurationType = nDurationType;

    if (fDuration > 0.0f)
    {
        m_nDuration = (unsigned long)(fDuration * 1000.0f);
    }
}

static inline void NwsspellimpactTrace1()
{
}
