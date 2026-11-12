#include "exobase.h"

#include "exoresman.h"
#include "math3d.h"
#include <math.h>
#include "resgff.h"
#include "appmanager.h"
#include "serverexoapp.h"
#include "serveraimaster.h"
#include "worldtimer.h"
#include "nwscript.h"
#include "SWCollisionMesh.h"
#include "nwsmodule.h"
#include "nwsarea.h"
#include "nwarea.h"
#include "nwdoorsurfacemesh.h"
#include "nwseffect.h"
#include "nwseffectlist.h"
#include "twodimarray.h"
#include "swplayercontrol.h"
#include "nwsobject.h"
#include "nwsdoor.h"
#include "nwcanimbase.h"
#include "gameobjectarray.h"
#include "nwmessage.h"
#include "nwscombatround.h"
#include "nwitem.h"
#include "nwsitem.h"
#include "nwsscriptevent.h"
#include "guiingame.h"
#include "partytables.h"
#include "swparty.h"
#include "nwsplayer.h"
#include "nwportrait.h"
#include "clientexoapp.h"
#include "clientoptions.h"
#include "nwstrigger.h"
#include "exotlktable.h"
#include "nwtlktable.h"

static inline void NwsdoorTrace2();
static inline void NwsdoorTrace1();

extern int nRenderDoorBBox;
// PC 0x0057F290
int CSWSDoor::GetMeshOutline(Vector vPosition, Vector vUnused, Vector& vFirst,
                             Vector& vSecond, Vector& vThird, Vector& vFourth)
{
    m_apCollisionMesh[CSWDoorSurfaceMesh::DOOR_STATE_CLOSED]->m_vWorldPosition =
        vPosition;
    Quaternion qOrientation = m_qOrientation;

    m_apCollisionMesh[CSWDoorSurfaceMesh::DOOR_STATE_CLOSED]->m_qOrientation =
        qOrientation;
    return m_apCollisionMesh[CSWDoorSurfaceMesh::DOOR_STATE_CLOSED]
        ->GetMeshOutline(vFirst, vSecond, vThird, vFourth);
}
// PC 0x0057F310
void CSWSDoor::AIUpdate()
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
    CServerExoApp* pServerApp = g_pAppManager->m_pServerExoApp;
    pServerApp->GetWorldTimer()->SubtractWorldTimes(
        nNowDay, nNowTime, m_nLastUpdateSeconds, m_nLastUpdateMilliseconds,
        &nDiffDay, &nDiffTime);
    if (nDiffTime >= 6000)
    {
        if (m_nLastUpdateMilliseconds != 0 && GetDead() == 0)
        {
            if (m_c251 == 0)
            {
                char nPCLevel = 0;
                g_pAppManager->GetServerExoApp()->GetGlobalVariableTable()->GetValueNumber(
                    "G_PC_LEVEL", nPCLevel);
                m_c251 = nPCLevel;
            }
            g_pVirtualMachine->RunScript(&m_asScripts[5], m_nId, 1);
        }
        m_nLastUpdateSeconds = nNowDay;
        m_nLastUpdateMilliseconds = nNowTime;
    }
    UpdateEffectList(nNowDay, nNowTime);
    RunActions(nNowDay, nNowTime, nTimer);
}
// PC 0x0057F480
int CSWSDoor::GetDead()
{
    if (m_bPlotObject != 0)
    {
        return 0;
    }
    else
    {
        return CSWSObject::GetDead();
    }
}
// PC 0x0057F4A0
unsigned char CSWSDoor::GetOpenLockDC()
{
    unsigned char nDC;
    unsigned char nPercent;
    char nModifier = 0;
    if (m_nOpenLockDiff == 4)
    {
        char nFixedModifier;
        switch (g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nDifficulty)
        {
            case 0:
                nFixedModifier = -5;
                break;
            case 1:
                nFixedModifier = 0;
                break;
            case 2:
                nFixedModifier = 5;
                break;
            default:
                nFixedModifier = 0;
                break;
        }
        nDC = m_nOpenLockDC;
        if (nDC + nFixedModifier > 0)
        {
            return nDC + nFixedModifier;
        }
        return 0;
    }
    switch (m_nOpenLockDiff)
    {
        case 0:
            nPercent = 0;
            break;
        case 1:
            nPercent = 30;
            break;
        case 2:
            nPercent = 70;
            break;
        case 3:
            nPercent = 100;
            break;
        default:
            nPercent = 30;
            break;
    }
    switch (g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nDifficulty)
    {
        case 0:
            nModifier = -5;
            break;
        case 1:
            nModifier = 0;
            break;
        case 2:
            nModifier = 5;
            break;
    }
    nDC = (m_c251 + 7) * (nPercent / 100) + m_nOpenLockDiffMod + nModifier + 21;
    return nDC;
}
// PC 0x0057F590
unsigned char CSWSDoor::GetDetectDC()
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
    if (nDC < 1)
        nDC = 1;
    return nDC;
}
// PC 0x0057F5E0
unsigned char CSWSDoor::GetDisarmDC()
{
    unsigned char nDC = m_nDisarmDC;
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
    if (nDC < 1)
    {
        return 1;
    }
    return nDC;
}
// PC 0x005802B0
int CSWSDoor::SaveDoor(CResGFF* pRes, CResStruct* pStruct)
{
    pRes->WriteFieldDWORD(pStruct, (unsigned char)m_nAppearanceType, "Appearance");
    pRes->WriteFieldBYTE(pStruct, m_nGenericType, "GenericType");
    pRes->WriteFieldBYTE(pStruct, m_nOpenState, "OpenState");
    pRes->WriteFieldBYTE(pStruct, m_bAutoRemoveKey, "AutoRemoveKey");
    float fBearing = m_fBearing;
    pRes->WriteFieldFLOAT(pStruct, fBearing, "Bearing");
    pRes->WriteFieldFLOAT(pStruct, m_vPosition.m_fX, "X");
    pRes->WriteFieldFLOAT(pStruct, m_vPosition.m_fY, "Y");
    pRes->WriteFieldFLOAT(pStruct, m_vPosition.m_fZ, "Z");
    pRes->WriteFieldDWORD(pStruct, m_nFactionId, "Faction");
    pRes->WriteFieldBYTE(pStruct, m_nFortSave, "Fort");
    pRes->WriteFieldBYTE(pStruct, m_nWillSave, "Will");
    pRes->WriteFieldBYTE(pStruct, m_nReflexSave, "Ref");
    pRes->WriteFieldSHORT(pStruct, m_nMaxHitPoints, "HP");
    pRes->WriteFieldSHORT(pStruct, GetCurrentHitPoints(0), "CurrentHP");
    pRes->WriteFieldBYTE(pStruct, m_bPlotObject, "Plot");
    pRes->WriteFieldBYTE(pStruct, m_bNotBlastable, "NotBlastable");
    pRes->WriteFieldBYTE(pStruct, m_bMin1HP, "Min1HP");
    pRes->WriteFieldCExoString(pStruct, m_sKeyName, "KeyName");
    pRes->WriteFieldBYTE(pStruct, m_bKeyRequired, "KeyRequired");
    pRes->WriteFieldBYTE(pStruct, m_nOpenLockDC, "OpenLockDC");
    pRes->WriteFieldBYTE(pStruct, m_nOpenLockDiff, "OpenLockDiff");
    pRes->WriteFieldCHAR(pStruct, m_nOpenLockDiffMod, "OpenLockDiffMod");
    pRes->WriteFieldBYTE(pStruct, m_nCloseLockDC, "CloseLockDC");
    pRes->WriteFieldBYTE(pStruct, m_nSecretDoorDC, "SecretDoorDC");
    pRes->WriteFieldCExoString(pStruct, m_sTag, "Tag");
    pRes->WriteFieldCResRef(pStruct, GetDialogResref(), "Conversation");
    if (GetPortraitId() == -1)
    {
        pRes->WriteFieldCResRef(pStruct, GetPortrait(), "Portrait");
    }
    else
    {
        pRes->WriteFieldWORD(pStruct, GetPortraitId(), "PortraitId");
    }
    pRes->WriteFieldBYTE(pStruct, m_nHardness, "Hardness");
    {
        CResRef cScript(m_asScripts[SCRIPT_ON_CLOSED]);
        pRes->WriteFieldCResRef(pStruct, cScript, "OnClosed");
    }
    {
        CResRef cScript(m_asScripts[SCRIPT_ON_DAMAGED]);
        pRes->WriteFieldCResRef(pStruct, cScript, "OnDamaged");
    }
    {
        CResRef cScript(m_asScripts[SCRIPT_ON_DEATH]);
        pRes->WriteFieldCResRef(pStruct, cScript, "OnDeath");
    }
    {
        CResRef cScript(m_asScripts[SCRIPT_ON_DISARM]);
        pRes->WriteFieldCResRef(pStruct, cScript, "OnDisarm");
    }
    {
        CResRef cScript(m_asScripts[SCRIPT_ON_HEARTBEAT]);
        pRes->WriteFieldCResRef(pStruct, cScript, "OnHeartbeat");
    }
    {
        CResRef cScript(m_asScripts[SCRIPT_ON_LOCK]);
        pRes->WriteFieldCResRef(pStruct, cScript, "OnLock");
    }
    {
        CResRef cScript(m_asScripts[SCRIPT_ON_MELEE_ATTACKED]);
        pRes->WriteFieldCResRef(pStruct, cScript, "OnMeleeAttacked");
    }
    {
        CResRef cScript(m_asScripts[SCRIPT_ON_OPEN]);
        pRes->WriteFieldCResRef(pStruct, cScript, "OnOpen");
    }
    {
        CResRef cScript(m_asScripts[SCRIPT_ON_SPELL_CAST_AT]);
        pRes->WriteFieldCResRef(pStruct, cScript, "OnSpellCastAt");
    }
    {
        CResRef cScript(m_asScripts[SCRIPT_ON_TRAP_TRIGGERED]);
        pRes->WriteFieldCResRef(pStruct, cScript, "OnTrapTriggered");
    }
    {
        CResRef cScript(m_asScripts[SCRIPT_ON_UNLOCK]);
        pRes->WriteFieldCResRef(pStruct, cScript, "OnUnlock");
    }
    {
        CResRef cScript(m_asScripts[SCRIPT_ON_USER_DEFINED]);
        pRes->WriteFieldCResRef(pStruct, cScript, "OnUserDefined");
    }
    {
        CResRef cScript(m_asScripts[SCRIPT_ON_CLICK]);
        pRes->WriteFieldCResRef(pStruct, cScript, "OnClick");
    }
    {
        CResRef cScript(m_asScripts[SCRIPT_ON_FAIL_TO_OPEN]);
        pRes->WriteFieldCResRef(pStruct, cScript, "OnFailToOpen");
    }
    {
        CResRef cScript(m_asScripts[SCRIPT_ON_DIALOG]);
        pRes->WriteFieldCResRef(pStruct, cScript, "OnDialog");
    }
    pRes->WriteFieldBYTE(pStruct, m_nTrapType, "TrapType");
    pRes->WriteFieldBYTE(pStruct, m_bTrapDisarmable, "TrapDisarmable");
    pRes->WriteFieldBYTE(pStruct, m_bTrapDetectable, "TrapDetectable");
    pRes->WriteFieldBYTE(pStruct, GetDisarmDC(), "DisarmDC");
    pRes->WriteFieldBYTE(pStruct, GetDetectDC(), "TrapDetectDC");
    pRes->WriteFieldINT(pStruct, m_nOwnerDemolitionsSkill, "OwnerDemolitionsSkill");
    pRes->WriteFieldBYTE(pStruct, m_bTrapFlag, "TrapFlag");
    pRes->WriteFieldBYTE(pStruct, m_bTrapOneShot, "TrapOneShot");
    pRes->WriteFieldBYTE(pStruct, m_bLocked, "Locked");
    pRes->WriteFieldBYTE(pStruct, m_bLockable, "Lockable");
    pRes->WriteFieldBYTE(pStruct, m_IsLinked, "LinkedToFlags");
    pRes->WriteFieldCExoString(pStruct, m_sLinkedTo, "LinkedTo");
    CResRef cModule((char*)m_sLinkedToModule.CStr());
    pRes->WriteFieldCResRef(pStruct, cModule, "LinkedToModule");
    pRes->WriteFieldWORD(pStruct, m_nLoadScreenId, "LoadScreenID");
    pRes->WriteFieldBYTE(pStruct, m_bUseTweakColor, "UseTweakColor");
    pRes->WriteFieldDWORD(pStruct, m_nTweakColor, "TweakColor");
    pRes->WriteFieldBYTE(pStruct, m_c251, "PCLevelAtSpawn");
    pRes->WriteFieldCExoLocString(pStruct, *GetFirstName(), "LocName");
    pRes->WriteFieldCExoLocString(pStruct, m_locDescription, "Description");
    pRes->WriteFieldBYTE(pStruct, m_bStatic, "Static");
    pRes->WriteFieldCExoLocString(pStruct, m_locTransitionDestination, "TransitionDestination");
    return 1;
}
// PC 0x0057F630
int CSWSDoor::GetIsLinked()
{
    char nIsLinked;
    nIsLinked = m_IsLinked;
    return (nIsLinked == DOOR_LINK_DOOR) ||
           (nIsLinked == DOOR_LINK_WAYPOINT);
}
// PC 0x0057F650
int CSWSDoor::NoNonWalkPolysInDoor(const Vector& vStart, const Vector& vEnd,
                                   const float& fA, const float& fB,
                                   const float& fC, CWalkHitInfo* pHitInfo)
{
    CSWCollisionMesh* pMesh = m_apCollisionMesh[(unsigned char)m_nOpenState];
    if (pMesh == 0)
    {
        return 0;
    }
    m_apCollisionMesh[(unsigned char)m_nOpenState]->m_vWorldPosition =
        m_vPosition;
    Quaternion qOrientation = m_qOrientation;
    m_apCollisionMesh[(unsigned char)m_nOpenState]->m_qOrientation =
        qOrientation;
    return m_apCollisionMesh[(unsigned char)m_nOpenState]->NoNonWalkPolys(
        vStart, vEnd, fA, fB, fC, pHitInfo);
}
// PC 0x0057F6F0
void CSWSDoor::DoDamage(int nDamage)
{
    if (m_bPlotObject == 0)
    {
        CSWSObject::DoDamage(nDamage);
    }
}
#ifdef _XBOX  // PC 0x0057F710
__forceinline
#endif
Vector CSWSDoor::GetActionPoint(int nAnimation, unsigned char nPoint)
{
    const float fInvalidActionPoint = -9999999.0f;

    switch (nAnimation)
    {
    case DOOR_ANIMATION_CLOSED:
        return m_apCollisionMesh[CSWDoorSurfaceMesh::DOOR_STATE_CLOSED]
            ->LocalToWorld(m_apCollisionMesh[CSWDoorSurfaceMesh::DOOR_STATE_CLOSED]
                              ->m_avUsePoints[nPoint]);
    case DOOR_ANIMATION_OPEN1:
        return m_apCollisionMesh[CSWDoorSurfaceMesh::DOOR_STATE_OPEN1]
            ->LocalToWorld(m_apCollisionMesh[CSWDoorSurfaceMesh::DOOR_STATE_OPEN1]
                              ->m_avUsePoints[nPoint]);
    case DOOR_ANIMATION_OPEN2:
        return m_apCollisionMesh[CSWDoorSurfaceMesh::DOOR_STATE_OPEN2]
            ->LocalToWorld(m_apCollisionMesh[CSWDoorSurfaceMesh::DOOR_STATE_OPEN2]
                              ->m_avUsePoints[nPoint]);
    default:
        EXOXBOXASSERT(0);
        return Vector(fInvalidActionPoint, fInvalidActionPoint, fInvalidActionPoint);
    }
}
// PC 0x0057F7A0
Vector CSWSDoor::GetNearestActionPoint(const Vector& vPosition, int bClosed)
{
    Vector vNearestPoint;
    float fNearestDistance = DOOR_ACTION_POINT_INITIAL_DISTANCE_SQUARED;

    int nAnimation = m_nAnimation;
    CSWSArea* pArea = GetArea();

    if (bClosed)
    {
        nAnimation = DOOR_ANIMATION_CLOSED;
    }

    Vector vPoint1 = GetActionPoint(nAnimation, 0);
    Vector vPoint2 = GetActionPoint(nAnimation, 1);
    float fDistance1 = (vPosition - vPoint1) * (vPosition - vPoint1);
    float fDistance2 = (vPosition - vPoint2) * (vPosition - vPoint2);

    if (fDistance1 < fDistance2 && pArea && pArea->PositionWalkable(vPoint1))
    {
        fNearestDistance = fDistance1;
        vNearestPoint = vPoint1;
    }
    else if (pArea && pArea->PositionWalkable(vPoint2))
    {
        fNearestDistance = fDistance2;
        vNearestPoint = vPoint2;
    }

    if (nAnimation != DOOR_ANIMATION_CLOSED)
    {
        Vector vClosedPoint1 = GetActionPoint(DOOR_ANIMATION_CLOSED, 0);
        Vector vClosedPoint2 = GetActionPoint(DOOR_ANIMATION_CLOSED, 1);
        float fClosedDistance1 =
            (vPosition - vClosedPoint1) * (vPosition - vClosedPoint1);
        float fClosedDistance2 =
            (vPosition - vClosedPoint2) * (vPosition - vClosedPoint2);

        if (fClosedDistance1 < fNearestDistance && pArea &&
            pArea->PositionWalkable(vClosedPoint1))
        {
            vNearestPoint = vClosedPoint1;
        }
        else if (fClosedDistance2 < fNearestDistance && pArea &&
                 pArea->PositionWalkable(vClosedPoint2))
        {
            vNearestPoint = vClosedPoint2;
        }
    }
    return vNearestPoint;
}
// PC 0x0057FAE0
CSWSObject* CSWSDoor::GetLinkedObject()
{
    CSWSObject* pLinkedObject = 0;
    if (m_IsLinked == DOOR_LINK_DOOR || m_IsLinked == DOOR_LINK_WAYPOINT)
    {
        CServerExoApp* pServerApp = g_pAppManager->m_pServerExoApp;
        pLinkedObject = pServerApp->GetDoorByGameObjectID(m_oidLinkedObject);
        if (pLinkedObject == 0)
        {
            pServerApp = g_pAppManager->m_pServerExoApp;
            m_oidLinkedObject = pServerApp->GetModule()
                ->FindObjectByTagOrdinal(m_sLinkedTo, 0UL);
            pServerApp = g_pAppManager->m_pServerExoApp;
            CGameObject* pGameObject = pServerApp->GetGameObject(m_oidLinkedObject);
            if (pGameObject != 0)
            {
                pLinkedObject = pGameObject->AsSWSObject();
            }
        }
    }
    return pLinkedObject;
}

#ifndef _XBOX
int CSWSDoor::OpenDoor(unsigned long oidOpener)  // PC 0x005801E0
{
    CGameObject* pGameObject;
    CSWSObject* pOpener;
    CSWSObject* pLinkedObject;
    int nState;

    nState = 2;
    pGameObject = g_pAppManager->m_pServerExoApp->GetGameObject(oidOpener);
    if (pGameObject != 0)
    {
        pOpener = pGameObject->AsSWSObject();
        if ((pOpener->m_vPosition - m_vPosition) * m_vOrientation > 0.0f)
        {
            nState = 1;
        }
    }
    m_oidLastOpener = oidOpener;
    SetOpenState(nState, 1);
    pLinkedObject = GetLinkedObject();
    if (pLinkedObject != 0 && pLinkedObject->AsSWSDoor() != 0)
    {
        pLinkedObject->AsSWSDoor()->SetOpenState(nState, 1);
    }
    return g_pVirtualMachine->RunScript(&m_asScripts[0], m_nId, 1);
}
#endif
// PC 0x00580E50
int CSWSDoor::LoadDoor(CResGFF* pRes, CResStruct* pStruct, int bLoadTemplate)
{
    int bSuccess;
    m_nAppearanceType = pRes->ReadFieldDWORD(pStruct, "Appearance", bSuccess,
                                           (unsigned char)m_nAppearanceType);
    {
        unsigned char nDefault = m_nGenericType;
        m_nGenericType = pRes->ReadFieldBYTE(pStruct, "GenericType", bSuccess,
                                       nDefault);
    }
    {
    unsigned char nDefaultByte = m_nOpenState;
    unsigned char nOpenState = pRes->ReadFieldBYTE(pStruct, "OpenState", bSuccess,
                                                 nDefaultByte);
    if (!m_bAnimatable)
        nOpenState = OPEN_STATE_DESTROYED;
    m_nOpenState = m_nTargetOpenState = nOpenState;
    switch ((unsigned char)m_nOpenState)
    {
    case OPEN_STATE_CLOSED: SetAnimation(DOOR_ANIMATION_CLOSED); break;
    case OPEN_STATE_OPEN1: SetAnimation(DOOR_ANIMATION_OPEN1); break;
    case OPEN_STATE_OPEN2: SetAnimation(DOOR_ANIMATION_OPEN2); break;
    case OPEN_STATE_DESTROYED: SetAnimation(DOOR_ANIMATION_DESTROYED); break;
    }
    }
    {
        unsigned char nDefault = m_bAutoRemoveKey;
        m_bAutoRemoveKey = pRes->ReadFieldBYTE(pStruct, "AutoRemoveKey", bSuccess,
                                         nDefault);
    }
    {
        float fDefaultBearing = m_fBearing;
        m_fBearing = pRes->ReadFieldFLOAT(pStruct, "Bearing", bSuccess, fDefaultBearing);
    }
    {
        unsigned long nDefault = m_nFactionId;
        m_nFactionId = pRes->ReadFieldDWORD(pStruct, "Faction", bSuccess, nDefault);
    }
    {
        unsigned char nDefault = m_nFortSave;
        m_nFortSave = pRes->ReadFieldBYTE(pStruct, "Fort", bSuccess, nDefault);
    }
    {
        unsigned char nDefault = m_nWillSave;
        m_nWillSave = pRes->ReadFieldBYTE(pStruct, "Will", bSuccess, nDefault);
    }
    {
        unsigned char nDefault = m_nReflexSave;
        m_nReflexSave = pRes->ReadFieldBYTE(pStruct, "Ref", bSuccess, nDefault);
    }
    {
        short nDefault = m_nMaxHitPoints;
        m_nMaxHitPoints = pRes->ReadFieldSHORT(pStruct, "HP", bSuccess, nDefault);
    }
    int nHitPoints = pRes->ReadFieldSHORT(pStruct, "CurrentHP", bSuccess,
                                          GetCurrentHitPoints(0));
    if (bLoadTemplate && nHitPoints > CSWSObject::GetMaxHitPoints(0))
        SetCurrentHitPoints(CSWSObject::GetMaxHitPoints(0));
    else
        SetCurrentHitPoints(nHitPoints);
    {
        int nDefault = m_bPlotObject;
        int bPlot = pRes->ReadFieldBYTE(pStruct, "Invulnerable", bSuccess, nDefault);
        if (!bSuccess)
        {
            nDefault = m_bPlotObject;
            bPlot = pRes->ReadFieldBYTE(pStruct, "Plot", bSuccess, nDefault);
        }
        int bStatic = pRes->ReadFieldBYTE(pStruct, "Static", bSuccess, 0);
        m_bPlotObject = bStatic ? 1 : bPlot;
        m_bStatic = bStatic;
    }
    {
        int nDefault = m_bNotBlastable;
        m_bNotBlastable = pRes->ReadFieldBYTE(pStruct, "NotBlastable", bSuccess, nDefault);
    }
    {
        int nDefault = m_bMin1HP;
        m_bMin1HP = pRes->ReadFieldBYTE(pStruct, "Min1HP", bSuccess, nDefault);
    }
    m_sKeyName = pRes->ReadFieldCExoString(pStruct, "KeyName", bSuccess, m_sKeyName);
    {
        int nDefault = m_bKeyRequired;
        m_bKeyRequired = pRes->ReadFieldBYTE(pStruct, "KeyRequired", bSuccess, nDefault);
    }
    m_nOpenLockDC = pRes->ReadFieldBYTE(pStruct, "OpenLockDC", bSuccess, 0);
    {
        unsigned char nDefault = m_nOpenLockDiff;
        m_nOpenLockDiff = pRes->ReadFieldBYTE(pStruct, "OpenLockDiff", bSuccess, nDefault);
    }
    {
        char nDefault = m_nOpenLockDiffMod;
        m_nOpenLockDiffMod = pRes->ReadFieldCHAR(pStruct, "OpenLockDiffMod", bSuccess, nDefault);
    }
    {
        unsigned char nDefault = m_nCloseLockDC;
        m_nCloseLockDC = pRes->ReadFieldBYTE(pStruct, "CloseLockDC", bSuccess, nDefault);
    }
    {
        unsigned char nDefault = m_nSecretDoorDC;
        m_nSecretDoorDC = pRes->ReadFieldBYTE(pStruct, "SecretDoorDC", bSuccess, nDefault);
    }
    SetTag(pRes->ReadFieldCExoString(pStruct, "Tag", bSuccess, m_sTag));
    SetDialogResref(pRes->ReadFieldCResRef(pStruct, "Conversation", bSuccess, ""));
    unsigned short nPortraitId = pRes->ReadFieldWORD(pStruct, "PortraitId", bSuccess,
                                                    PORTRAIT_ID_RESERVED_FFFF);
    if (nPortraitId >= PORTRAIT_ID_FIRST_RESERVED)
    {
        CResRef cPortrait;
        cPortrait = pRes->ReadFieldCResRef(pStruct, "Portrait", bSuccess, "");
        if (*cPortrait.GetResRef())
            SetPortrait(cPortrait);
        else
            SetPortraitId(PORTRAIT_ID_DEFAULT_DOOR);
    }
    else
    {
        if (nPortraitId)
            SetPortraitId(nPortraitId);
        else
            SetPortraitId(PORTRAIT_ID_DEFAULT_DOOR);
    }
    m_nHardness = pRes->ReadFieldBYTE(pStruct, "Hardness", bSuccess, 0);
    m_locName = pRes->ReadFieldCExoLocString(pStruct, "LocName", bSuccess, CExoLocString());
    m_locDescription = pRes->ReadFieldCExoLocString(pStruct, "Description", bSuccess, CExoLocString());
    {
        CResRef cScript(m_asScripts[SCRIPT_ON_CLOSED]);
        CExoString sScript(pRes->ReadFieldCResRef(
            pStruct, "OnClosed", bSuccess, cScript).GetResRefStr());
        m_asScripts[SCRIPT_ON_CLOSED] = sScript;
    }

    {
        CResRef cScript(m_asScripts[SCRIPT_ON_CLOSED]);
        CExoString sScript(pRes->ReadFieldCResRef(
            pStruct, "OnDamaged", bSuccess, cScript).GetResRefStr());
        m_asScripts[SCRIPT_ON_DAMAGED] = sScript;
    }
    {
        CResRef cScript(m_asScripts[SCRIPT_ON_DEATH]);
        CExoString sScript(pRes->ReadFieldCResRef(
            pStruct, "OnDeath", bSuccess, cScript).GetResRefStr());
        m_asScripts[SCRIPT_ON_DEATH] = sScript;
    }
    {
        CResRef cScript(m_asScripts[SCRIPT_ON_DISARM]);
        CExoString sScript(pRes->ReadFieldCResRef(
            pStruct, "OnDisarm", bSuccess, cScript).GetResRefStr());
        m_asScripts[SCRIPT_ON_DISARM] = sScript;
    }
    {
        CResRef cScript(m_asScripts[SCRIPT_ON_HEARTBEAT]);
        CExoString sScript(pRes->ReadFieldCResRef(
            pStruct, "OnHeartbeat", bSuccess, cScript).GetResRefStr());
        m_asScripts[SCRIPT_ON_HEARTBEAT] = sScript;
    }
    {
        CResRef cScript(m_asScripts[SCRIPT_ON_LOCK]);
        CExoString sScript(pRes->ReadFieldCResRef(
            pStruct, "OnLock", bSuccess, cScript).GetResRefStr());
        m_asScripts[SCRIPT_ON_LOCK] = sScript;
    }
    {
        CResRef cScript(m_asScripts[SCRIPT_ON_MELEE_ATTACKED]);
        CExoString sScript(pRes->ReadFieldCResRef(
            pStruct, "OnMeleeAttacked", bSuccess, cScript).GetResRefStr());
        m_asScripts[SCRIPT_ON_MELEE_ATTACKED] = sScript;
    }
    {
        CResRef cScript(m_asScripts[SCRIPT_ON_OPEN]);
        CExoString sScript(pRes->ReadFieldCResRef(
            pStruct, "OnOpen", bSuccess, cScript).GetResRefStr());
        m_asScripts[SCRIPT_ON_OPEN] = sScript;
    }
    {
        CResRef cScript(m_asScripts[SCRIPT_ON_SPELL_CAST_AT]);
        CExoString sScript(pRes->ReadFieldCResRef(
            pStruct, "OnSpellCastAt", bSuccess, cScript).GetResRefStr());
        m_asScripts[SCRIPT_ON_SPELL_CAST_AT] = sScript;
    }
    {
        CResRef cScript(m_asScripts[SCRIPT_ON_TRAP_TRIGGERED]);
        CExoString sScript(pRes->ReadFieldCResRef(
            pStruct, "OnTrapTriggered", bSuccess, cScript).GetResRefStr());
        m_asScripts[SCRIPT_ON_TRAP_TRIGGERED] = sScript;
    }
    {
        CResRef cScript(m_asScripts[SCRIPT_ON_UNLOCK]);
        CExoString sScript(pRes->ReadFieldCResRef(
            pStruct, "OnUnlock", bSuccess, cScript).GetResRefStr());
        m_asScripts[SCRIPT_ON_UNLOCK] = sScript;
    }
    {
        CResRef cScript(m_asScripts[SCRIPT_ON_USER_DEFINED]);
        CExoString sScript(pRes->ReadFieldCResRef(
            pStruct, "OnUserDefined", bSuccess, cScript).GetResRefStr());
        m_asScripts[SCRIPT_ON_USER_DEFINED] = sScript;
    }
    {
        CResRef cScript(m_asScripts[SCRIPT_ON_CLICK]);
        CExoString sScript(pRes->ReadFieldCResRef(
            pStruct, "OnClick", bSuccess, cScript).GetResRefStr());
        m_asScripts[SCRIPT_ON_CLICK] = sScript;
    }
    {
        CResRef cScript(m_asScripts[SCRIPT_ON_FAIL_TO_OPEN]);
        CExoString sScript(pRes->ReadFieldCResRef(
            pStruct, "OnFailToOpen", bSuccess, cScript).GetResRefStr());
        m_asScripts[SCRIPT_ON_FAIL_TO_OPEN] = sScript;
    }
    {
        CResRef cScript(m_asScripts[SCRIPT_ON_DIALOG]);
        CExoString sScript(pRes->ReadFieldCResRef(
            pStruct, "OnDialog", bSuccess, cScript).GetResRefStr());
        m_asScripts[SCRIPT_ON_DIALOG] = sScript;
    }
    {
        unsigned char nDefault = m_nTrapType;
        m_nTrapType = pRes->ReadFieldBYTE(pStruct, "TrapType", bSuccess, nDefault);
    }
    CExoString* pTrapScript = &m_asScripts[SCRIPT_ON_TRAP_TRIGGERED];
    if (pTrapScript == NULL || pTrapScript->IsEmpty() || *pTrapScript == "default")
    {
        CExoString sMineScript;
        {
            CExoString sColumn("MineScript");
            unsigned char nTrapType = m_nTrapType;
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pTraps = p2DArrays->m_pTraps;
            pTraps->GetCExoStringEntry(nTrapType, sColumn, &sMineScript);
        }
        m_asScripts[SCRIPT_ON_TRAP_TRIGGERED] = sMineScript;
    }
    {
        int nDefault = m_bTrapDisarmable;
        m_bTrapDisarmable = pRes->ReadFieldBYTE(pStruct, "TrapDisarmable", bSuccess, nDefault);
    }
    {
        int nDefault = m_bTrapDetectable;
        m_bTrapDetectable = pRes->ReadFieldBYTE(pStruct, "TrapDetectable", bSuccess, nDefault);
    }
    m_nDisarmDC = pRes->ReadFieldBYTE(pStruct, "DisarmDC", bSuccess, GetDisarmDC());
    m_nTrapDetectDC = pRes->ReadFieldBYTE(pStruct, "TrapDetectDC", bSuccess, GetDetectDC());
    m_nOwnerDemolitionsSkill = pRes->ReadFieldINT(pStruct, "OwnerDemolitionsSkill", bSuccess, 0);
    {
        int nDefault = m_bTrapFlag;
        m_bTrapFlag = pRes->ReadFieldBYTE(pStruct, "TrapFlag", bSuccess, nDefault);
    }
    {
        int nDefault = m_bTrapOneShot;
        m_bTrapOneShot = pRes->ReadFieldBYTE(pStruct, "TrapOneShot", bSuccess, nDefault);
    }
    {
        int nDefault = m_bLocked;
        m_bLocked = pRes->ReadFieldBYTE(pStruct, "Locked", bSuccess, nDefault);
    }
    {
        int nDefault = m_bLockable;
        m_bLockable = pRes->ReadFieldBYTE(pStruct, "Lockable", bSuccess, nDefault);
    }
    m_IsLinked = pRes->ReadFieldBYTE(pStruct, "LinkedToFlags", bSuccess, 0);
    m_sLinkedTo = pRes->ReadFieldCExoString(pStruct, "LinkedTo", bSuccess, "");
    m_bUseTweakColor = pRes->ReadFieldBYTE(pStruct, "UseTweakColor", bSuccess, 0);
    m_nTweakColor = pRes->ReadFieldDWORD(pStruct, "TweakColor", bSuccess, 0);
    m_c251 = pRes->ReadFieldBYTE(pStruct, "PCLevelAtSpawn", bSuccess, 0);
    m_locTransitionDestination = pRes->ReadFieldCExoLocString(pStruct, "TransitionDestination", bSuccess, m_locTransitionDestination);
    CResRef cModule;
    cModule = pRes->ReadFieldCResRef(pStruct, "LinkedToModule", bSuccess, "");
    if (bSuccess == 1)
        m_sLinkedToModule = cModule.GetResRefStr();
    m_nLoadScreenId = pRes->ReadFieldWORD(pStruct, "LoadScreenID", bSuccess, 0);
    CExoString sModel;
    if (m_nAppearanceType)
    {
        {
            CExoString sColumn("Model");
            unsigned char nAppearance = m_nAppearanceType;
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pDoorTypes = p2DArrays->m_pDoorTypes;
            pDoorTypes->GetCExoStringEntry(nAppearance, sColumn, &sModel);
        }
        int bVisibleModel = 1;
        {
            CExoString sColumn("VisibleModel");
            unsigned char nAppearance = m_nAppearanceType;
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pDoorTypes = p2DArrays->m_pDoorTypes;
            pDoorTypes->GetINTEntry(nAppearance, sColumn, &bVisibleModel);
        }
        m_bAnimatable = bVisibleModel;
        m_bPreciseUse = 0;
    }
    else
    {
        {
            CExoString sColumn("ModelName");
            unsigned char nAppearance = m_nGenericType;
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pDoorTypes = p2DArrays->m_pGenericDoors;
            pDoorTypes->GetCExoStringEntry(nAppearance, sColumn, &sModel);
        }
        int bPreciseUse = 0;
        {
            CExoString sColumn("PreciseUse");
            unsigned char nAppearance = m_nGenericType;
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pDoorTypes = p2DArrays->m_pGenericDoors;
            pDoorTypes->GetINTEntry(nAppearance, sColumn, &bPreciseUse);
        }
        int bUsePrecisePosition = bPreciseUse != 0;
        m_bPreciseUse = bUsePrecisePosition;
    }
    for (int nMesh = CSWDoorSurfaceMesh::DOOR_STATE_CLOSED;
         nMesh <= CSWDoorSurfaceMesh::DOOR_STATE_OPEN2; nMesh++)
    {
        m_apCollisionMesh[nMesh] = new CSWDoorSurfaceMesh;
        m_apCollisionMesh[nMesh]->SetDoorType(nMesh);
        m_apCollisionMesh[nMesh]->m_cResRef = CResRef(sModel);
        m_apCollisionMesh[nMesh]->LoadMesh(0);
        m_apCollisionMesh[nMesh]->ComputeMeshNormals();
    }
    PostProcess();
    return 1;
}
// PC 0x00580980
void CSWSDoor::PostProcess()
{
    if (m_nHardness > 0)
    {
        m_n1EC = 1;
        CGameEffect* pEffect = new CGameEffect(1);
        pEffect->m_nType = CSWSEffectListHandler::EFFECT_DAMAGE_RESISTANCE;
        pEffect->m_nSubType = (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                              CGameEffect::DURATION_TYPE_PERMANENT;
        pEffect->m_bExposed = 0;
        pEffect->SetInteger(0, CSWSEffectListHandler::DAMAGE_FLAG_ALL);
        pEffect->SetInteger(1, m_nHardness);
        ApplyEffect(pEffect, 0, 0);
        m_n1EC = 0;
    }

    if (m_bAnimatable == 0)
    {
        SetOpenState(OPEN_STATE_DESTROYED, 1);
        m_bPlotObject = 1;
    }

    if (m_bStatic != 0)
    {
        CExoString sStaticAnim;
        bool bNotFound;
        {
            CExoString sColumn("StaticAnim");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pGenericDoors = p2DArrays->m_pGenericDoors;
            bNotFound = !pGenericDoors->GetCExoStringEntry(m_nGenericType, sColumn,
                                                         &sStaticAnim);
        }
        if (bNotFound)
        {
            sStaticAnim = "";
        }
        if (sStaticAnim.CompareNoCase("busted"))
        {
            SetAnimation(DOOR_ANIMATION_BUSTED);
        }
    }
}
// PC 0x00580B20
CSWSDoor::CSWSDoor(unsigned long oidSelf)
    : CSWSObject(CGameObject::OBJECT_TYPE_DOOR, oidSelf, 0)
{
    g_pAppManager->m_pServerExoApp->GetServerAIMaster()->AddObject(this, 0);
    m_nAnimation = DOOR_ANIMATION_CLOSED;
    m_oidLastOpener = OBJECT_INVALID;
    m_oidLastCloser = OBJECT_INVALID;
    m_oidLastLockedBy = OBJECT_INVALID;
    m_oidLastUnlockedBy = OBJECT_INVALID;
    m_oidLastDisarmedBy = OBJECT_INVALID;
    m_oidLastTriggeredBy = OBJECT_INVALID;
    for (int nScript = 0; nScript < 15; nScript++)
    {
        m_asScripts[nScript] = "default";
    }
    m_nAppearanceType = 0;
    m_nGenericType = 0;
    m_bAutoRemoveKey = 0;
    m_nDisarmDC = 0;
    m_nTrapDetectDC = 0;
    m_bTrapFlag = 0;
    m_bTrapFlagged = 0;
    m_nTrapType = TRAP_TYPE_NONE;
    m_bTrapDetectable = 1;
    m_bTrapDisarmable = 1;
    m_bTrapOneShot = 1;
    m_fBearing = 0.0f;
    m_nFactionId = 0;
    m_nFortSave = 0;
    m_nReflexSave = 0;
    m_nWillSave = 0;
    m_sKeyName = "";
    m_bKeyRequired = 0;
    m_nOpenLockDC = 0;
    m_nCloseLockDC = 0;
    m_nSecretDoorDC = 0;
    m_nHardness = 0;
    m_bLocked = 0;
    m_bLockable = 0;
    m_nOpenState = CSWSDoor::OPEN_STATE_CLOSED;
    m_nTargetOpenState = 0;
    m_bAnimatable = 1;
    m_nOpenLockDiffMod = 0;
    m_nOwnerDemolitionsSkill = 0;
    m_bUseTweakColor = 0;
    m_nTweakColor = 0;
    m_nLastUpdateSeconds = 0;
    m_nLastUpdateMilliseconds = 0;
    m_IsLinked = 0;
    m_oidLinkedObject = OBJECT_INVALID;
    m_nOpenLockDiff = 1;
    m_apCollisionMesh[CSWDoorSurfaceMesh::DOOR_STATE_CLOSED] = NULL;
    m_apCollisionMesh[CSWDoorSurfaceMesh::DOOR_STATE_OPEN1] = NULL;
    m_apCollisionMesh[CSWDoorSurfaceMesh::DOOR_STATE_OPEN2] = NULL;
    m_nOutlineVertices = 0;
    m_nLoadScreenId = 0;
    m_bStatic = 0;
    m_bPreciseUse = 0;
    m_pTrapTrigger = NULL;
}
// PC 0x0057FB60
void CSWSDoor::SetOpenState(unsigned char nOpenState, int bImmediate)
{
    unsigned char nState;

    if (m_bAnimatable == 0)
    {
        nState = 3;
    }
    else
    {
        nState = nOpenState;
    }
    m_nTargetOpenState = nState;
    if (bImmediate == 0)
    {
        m_nOpenState = nState;
    }
    switch (nState)
    {
    case 0:
        SetAnimation(DOOR_ANIMATION_CLOSED);
        break;
    case 1:
        SetAnimation(DOOR_ANIMATION_OPEN1);
        break;
    case 2:
        SetAnimation(DOOR_ANIMATION_OPEN2);
        break;
    case 3:
        SetAnimation(10072);
        break;
    default:
        break;
    }
}
// PC 0x0057FBE0
void CSWSDoor::MoveToNextOpenState()
{
    char nOldState;
    char nNewState;
    CSWSArea* pArea;

    nOldState = m_nOpenState;
    nNewState = m_nTargetOpenState;
    m_nOpenState = nNewState;
    if (nNewState != nOldState && nNewState != 3)
    {
        pArea = GetArea();
        if (pArea != 0)
        {
            pArea->BudgeCreatures(m_vPosition, m_vMoveToPosition,
                                  m_vMoveToNormal, m_nId,
                                  m_nOpenState == CSWSDoor::OPEN_STATE_CLOSED);
        }
    }
}
// PC 0x0057FC40
void CSWSDoor::SetOrientation(Quaternion qOrientation)
{
    Vector vFacing;
    float fCross;
    float fSquares;

    m_qOrientation = qOrientation;
    fCross = qOrientation.m_fX * qOrientation.m_fY -
             qOrientation.m_fW * qOrientation.m_fZ;
    vFacing.m_fX = fCross + fCross;
    fSquares = qOrientation.m_fZ * qOrientation.m_fZ +
               qOrientation.m_fX * qOrientation.m_fX;
    vFacing.m_fY = 1.0f - (fSquares + fSquares);
    vFacing.m_fZ = 0.0f;
    if (vFacing.m_fY * vFacing.m_fY + vFacing.m_fX * vFacing.m_fX > 0.0001f)
    {
        vFacing = normalize(vFacing);
    }
    else
    {
        vFacing = Vector(0.0f, 1.0f, 0.0f);
    }
    CSWSObject::SetOrientation(vFacing);
}

int nRenderDoorBBox = 0;

#ifndef _XBOX
int CSWSDoor::InDoor(Vector vPoint)  // PC 0x0057FD30
{
    int nCrossings;
    int nVertex;
    int nNext;
    float fDeltaY;
    float fDeltaX;
    float fIntersectY;

    nCrossings = 0;
    for (nVertex = 0; nVertex < m_nOutlineVertices; nVertex++)
    {
        nNext = nVertex != m_nOutlineVertices - 1 ? nVertex + 1 : 0;
        if (m_avOutline[nNext].m_fX != m_avOutline[nVertex].m_fX)
        {
            double dStartY = m_avOutline[nVertex].m_fY;
            fDeltaY = m_avOutline[nNext].m_fY - dStartY;
            fDeltaX = m_avOutline[nNext].m_fX - m_avOutline[nVertex].m_fX;
            double dSlope = fDeltaY / fDeltaX;
            fIntersectY = dStartY -
                          dSlope *
                              (m_avOutline[nVertex].m_fX - vPoint.m_fX);
            if (fIntersectY > vPoint.m_fY)
            {
                if (fabs(fDeltaY) > fabs(fDeltaX))
                {
                    if (m_avOutline[nNext].m_fY > m_avOutline[nVertex].m_fY)
                    {
                        if (fIntersectY >= m_avOutline[nVertex].m_fY &&
                            fIntersectY < m_avOutline[nNext].m_fY)
                        {
                            nCrossings++;
                        }
                    }
                    else if (m_avOutline[nVertex].m_fY > m_avOutline[nNext].m_fY &&
                             fIntersectY > m_avOutline[nNext].m_fY &&
                             fIntersectY <= m_avOutline[nVertex].m_fY)
                    {
                        nCrossings++;
                    }
                }
                else
                {
                    if (m_avOutline[nNext].m_fX > m_avOutline[nVertex].m_fX)
                    {
                        if (vPoint.m_fX >= m_avOutline[nVertex].m_fX &&
                            vPoint.m_fX < m_avOutline[nNext].m_fX)
                        {
                            nCrossings++;
                        }
                    }
                    else if (m_avOutline[nVertex].m_fX > m_avOutline[nNext].m_fX &&
                             vPoint.m_fX > m_avOutline[nNext].m_fX &&
                             vPoint.m_fX <= m_avOutline[nVertex].m_fX)
                    {
                        nCrossings++;
                    }
                }
            }
        }
        if (m_avOutline[nNext].m_fX == vPoint.m_fX &&
            (m_avOutline[nNext].m_fY > vPoint.m_fY ||
             m_avOutline[nVertex].m_fY >= vPoint.m_fY))
        {
            nCrossings++;
        }
    }
    return nCrossings % 2 == 1;
}
#endif
int CSWSDoor::GetMeshBoundingBox(Vector vPosition, Vector vUnused,  // PC 0x0057FEF0
                                 Vector& vMinimum, Vector& vMaximum)
{
    int nMesh;
    int bSuccess;
    Vector vBoundsMin;
    Vector vBoundsMax;

    nMesh = CSWDoorSurfaceMesh::DOOR_STATE_CLOSED;
    vBoundsMin.m_fX = 0.0f;
    vBoundsMin.m_fY = 0.0f;
    vBoundsMin.m_fZ = 0.0f;
    vBoundsMax.m_fX = 0.0f;
    vBoundsMax.m_fY = 0.0f;
    vBoundsMax.m_fZ = 0.0f;
    bSuccess = 1;
    for (; nMesh <= CSWDoorSurfaceMesh::DOOR_STATE_OPEN2; ++nMesh)
    {
        m_apCollisionMesh[nMesh]->m_vWorldPosition = vPosition;
        Quaternion qOrientation = m_qOrientation;
        m_apCollisionMesh[nMesh]->m_qOrientation = qOrientation;
        bSuccess &= m_apCollisionMesh[nMesh]->GetMeshBoundingBox(vBoundsMin,
                                                                 vBoundsMax);

        if (nRenderDoorBBox != 0)
        {
            if (nMesh == 0)
            {
                Vector vClosedFirst =
                    m_apCollisionMesh[CSWDoorSurfaceMesh::DOOR_STATE_CLOSED]
                        ->LocalToWorld(
                            m_apCollisionMesh
                                [CSWDoorSurfaceMesh::DOOR_STATE_CLOSED]
                                    ->m_avUsePoints[0]);
                Vector vClosedSecond =
                    m_apCollisionMesh[CSWDoorSurfaceMesh::DOOR_STATE_CLOSED]
                        ->LocalToWorld(
                            m_apCollisionMesh
                                [CSWDoorSurfaceMesh::DOOR_STATE_CLOSED]
                                    ->m_avUsePoints[1]);
                nRenderDoorBBox = 2;
                m_apCollisionMesh[0]->RenderMesh(0, 0xFFFF0000, 0xFF00FF00);
            }
            else if (nMesh == 1)
            {
                Vector vOpenFirst =
                    m_apCollisionMesh[CSWDoorSurfaceMesh::DOOR_STATE_OPEN1]
                        ->LocalToWorld(
                            m_apCollisionMesh
                                [CSWDoorSurfaceMesh::DOOR_STATE_OPEN1]
                                    ->m_avUsePoints[0]);
                vOpenFirst.m_fZ += 0.2f;
                Vector vOpenSecond =
                    m_apCollisionMesh[CSWDoorSurfaceMesh::DOOR_STATE_OPEN1]
                        ->LocalToWorld(
                            m_apCollisionMesh
                                [CSWDoorSurfaceMesh::DOOR_STATE_OPEN1]
                                    ->m_avUsePoints[1]);
                nRenderDoorBBox = 3;
                vOpenSecond.m_fZ += 0.2f;
                m_apCollisionMesh[1]->RenderMesh(0, 0xFFFF0000, 0xFF00FF00);
            }
            else if (nMesh == 2)
            {
                Vector vOtherFirst =
                    m_apCollisionMesh[CSWDoorSurfaceMesh::DOOR_STATE_OPEN2]
                        ->LocalToWorld(
                            m_apCollisionMesh
                                [CSWDoorSurfaceMesh::DOOR_STATE_OPEN2]
                                    ->m_avUsePoints[0]);
                vOtherFirst.m_fZ += 0.4f;
                Vector vOtherSecond =
                    m_apCollisionMesh[CSWDoorSurfaceMesh::DOOR_STATE_OPEN2]
                        ->LocalToWorld(
                            m_apCollisionMesh
                                [CSWDoorSurfaceMesh::DOOR_STATE_OPEN2]
                                    ->m_avUsePoints[1]);
                nRenderDoorBBox = 4;
                vOtherSecond.m_fZ += 0.4f;
                m_apCollisionMesh[2]->RenderMesh(0, 0xFFFF0000, 0xFF00FF00);
            }
        }

        if (nMesh == CSWDoorSurfaceMesh::DOOR_STATE_CLOSED)
        {
            vMinimum = vBoundsMin;
            vMaximum = vBoundsMax;
        }
        else
        {
            if (vBoundsMin.m_fX < vMinimum.m_fX)
                vMinimum.m_fX = vBoundsMin.m_fX;
            if (vBoundsMin.m_fY < vMinimum.m_fY)
                vMinimum.m_fY = vBoundsMin.m_fY;
            if (vBoundsMin.m_fZ < vMinimum.m_fZ)
                vMinimum.m_fZ = vBoundsMin.m_fZ;
            if (vBoundsMax.m_fX > vMaximum.m_fX)
                vMaximum.m_fX = vBoundsMax.m_fX;
            if (vBoundsMax.m_fY > vMaximum.m_fY)
                vMaximum.m_fY = vBoundsMax.m_fY;
            if (vBoundsMax.m_fZ > vMaximum.m_fZ)
                vMaximum.m_fZ = vBoundsMax.m_fZ;
        }
    }
    return bSuccess;
}
// PC 0x00582130
int CSWSDoor::LoadFromTemplate(CResRef const& cResRef)
{
    int result;
    CResStruct topStruct;
    CResGFF* pRes = new CResGFF(0x7FA, "UTD ", cResRef);

    if (pRes->m_bLoaded == 0)
    {
        delete pRes;
        return 0;
    }
    pRes->GetTopLevelStruct(&topStruct);
    result = LoadDoor(pRes, &topStruct, 1);
    delete pRes;
    return result;
}
// PC 0x005821F0
void CSWSDoor::RemoveFromArea()
{
    NwsdoorTrace2();
    if (GetArea() != 0)
    {
        GetArea()->RemoveObjectFromArea(m_nId);
        if (GetArea()->m_lstTrapObjectIds.Contains(m_nId) == 1)
        {
            unsigned long oidObject = m_nId;
            CSWSArea* pArea = GetArea();
            pArea->m_lstTrapObjectIds.Remove(oidObject);
        }
        GetArea()->RemoveStaticBoundingBox(m_nId);
    }
    SetArea(0);
    SetPosition(Vector(0.0f, 0.0f, 0.0f), 1, 1, 0);
}
// PC 0x005822B0
CSWSDoor::~CSWSDoor()
{
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
    for (int nMesh = CSWDoorSurfaceMesh::DOOR_STATE_CLOSED;
         nMesh <= CSWDoorSurfaceMesh::DOOR_STATE_OPEN2; nMesh++)
    {
        if (m_apCollisionMesh[nMesh] != NULL)
        {
            delete m_apCollisionMesh[nMesh];
            m_apCollisionMesh[nMesh] = NULL;
        }
    }
    m_nOutlineVertices = 0;
}
// PC 0x00582430
void CSWSDoor::AddToArea(CSWSArea* pArea, float fX, float fY, float fZ,
                         int bLoadingGame)
{
    RemoveFromArea();
    if (pArea != NULL)
    {
        SetArea(pArea);
        Vector vPosition(fX, fY, fZ);
        SetPosition(vPosition, 0, 1, 0);
        pArea->AddObjectToArea(m_nId, bLoadingGame);
        pArea->m_lstTriggerIds.Add(m_nId);
        if (m_apCollisionMesh != NULL)
        {
            if (GetMeshBoundingBox(m_vPosition, m_vOrientation,
                                   m_vMoveToPosition, m_vMoveToNormal))
            {
                pArea->AddStaticBoundingBox(m_nId, m_vMoveToPosition,
                                             m_vMoveToNormal);
                GetMeshOutline(m_vPosition, m_vOrientation, m_avOutline[0],
                                 m_avOutline[1], m_avOutline[2], m_avOutline[3]);
                m_nOutlineVertices = 4;
            }
        }
    }
}
// PC 0x005825B0
void CSWSDoor::EventHandler(unsigned long nEventId, unsigned long oidCaller,
                           void* pEventData, unsigned long nCalendarDay,
                           unsigned long nTimeOfDay)
{
    if (nEventId == CServerAIMaster::EVENT_FEEDBACK_MESSAGE)
    {
        CSWCCMessageData* pData = (CSWCCMessageData*)pEventData;
        if (pData != NULL)
        {
            CSWSCreature* pCreature =
                g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidCaller);
            if (pData->m_nType == CSWCCMessageData::DATA_TYPE_SAVING_THROW)
            {
                if (pCreature != NULL)
                    pCreature->BroadcastSavingThrowData(pData);
                delete pData;
            }
            else if (pData->m_nType == CSWCCMessageData::DATA_TYPE_FEEDBACK)
            {
                if (pCreature != NULL)
                    pCreature->SendFeedbackMessage(pData->GetInteger(0), pData);
                else
                    delete pData;
            }
            else if (pCreature != NULL)
                pCreature->SendFeedbackMessage(pData->GetInteger(0), pData);
            else
                delete pData;
        }
    }
    else if (nEventId == CServerAIMaster::EVENT_ON_MELEE_ATTACKED)
    {
        CSWSCombatAttackData* pAttack = (CSWSCombatAttackData*)pEventData;
        CSWSItem* pWeapon = NULL;
        CSWSCreature* pAttacker;
        if (pAttack != NULL)
        {
            CSWCCMessageData* pData = new CSWCCMessageData;
            pData->SetObjectID(0, oidCaller);
            pData->SetObjectID(1, GetId());
            if (g_bEnableCombatDebugging)
                pData->SetString(0, pAttack->m_sAttackDebugText);
            pAttacker = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidCaller);
            if (pAttacker != NULL)
            {
                pAttacker->BroadcastAttackDataToParty(pData, pAttack);
                pAttacker->RemoveCombatInvisibilityEffects();
            }
            delete pData;
            m_nAttackType = pAttack->GetAttackType();
            m_nAttackMode = pAttack->GetAttackMode();
            if (pAttack->m_nWeaponAttackType == CSWSCombatRound::WEAPON_ATTACK_RIGHTHAND ||
                pAttack->m_nWeaponAttackType == CSWSCombatRound::WEAPON_ATTACK_EXTRA_RIGHTHAND)
                pWeapon = pAttacker->m_pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND);
            else if (pAttack->m_nWeaponAttackType == CSWSCombatRound::WEAPON_ATTACK_LEFTHAND)
                pWeapon = pAttacker->m_pInventory->GetItemInSlot(INVENTORY_SLOT_LEFTHAND);
            else if (pAttack->m_nWeaponAttackType == CSWSCombatRound::WEAPON_ATTACK_UNARMED ||
                     pAttack->m_nWeaponAttackType == CSWSCombatRound::WEAPON_ATTACK_EXTRA_UNARMED)
                pWeapon = pAttacker->m_pInventory->GetItemInSlot(INVENTORY_SLOT_HANDS);
            else if (pAttack->m_nWeaponAttackType == CSWSCombatRound::WEAPON_ATTACK_CREATURE1)
                pWeapon = pAttacker->m_pInventory->GetItemInSlot(INVENTORY_SLOT_CREATUREWEAPON1);
            else if (pAttack->m_nWeaponAttackType == CSWSCombatRound::WEAPON_ATTACK_CREATURE2)
                pWeapon = pAttacker->m_pInventory->GetItemInSlot(INVENTORY_SLOT_CREATUREWEAPON2);
            else if (pAttack->m_nWeaponAttackType == CSWSCombatRound::WEAPON_ATTACK_CREATURE3)
                pWeapon = pAttacker->m_pInventory->GetItemInSlot(INVENTORY_SLOT_CREATUREWEAPON3);
        }
        else

            pAttacker = (CSWSCreature*)oidCaller;
        m_oidLastAttacker = oidCaller;
        SetLastHostileActor(oidCaller, 0);
        if (pAttacker != NULL)
        {
            pAttacker->BroadcastCombatStateToParty(OBJECT_INVALID);
            pAttacker->RemoveCombatInvisibilityEffects();
            if (pWeapon != NULL)
                pAttacker->m_oidLastWeaponUsed = pWeapon->GetId();
            else
                pAttacker->m_oidLastWeaponUsed = OBJECT_INVALID;
        }
        g_pVirtualMachine->RunScript(&m_asScripts[SCRIPT_ON_MELEE_ATTACKED], m_nId, 1);
        if (m_bTrapFlag == 1 && pAttacker != NULL)
        {
            Vector vDistance = pAttacker->m_vPosition - m_vPosition;
            if (vDistance * vDistance <= 16.0f)
            {
                CScriptEvent* pEvent = new CScriptEvent;
                pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_MINE_TRIGGERED;
                g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                    0, 0, oidCaller, GetId(), CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
            }
        }
    }
    else if (nEventId == CServerAIMaster::EVENT_SIGNAL_EVENT)
    {
        CScriptEvent* pEvent = (CScriptEvent*)pEventData;
        switch (pEvent->m_nType)
        {
        case CSWSSCRIPTEVENT_EVENTTYPE_ON_FAIL_TO_OPEN:
            {
                m_oidLastTriggeredBy = oidCaller;
                g_pVirtualMachine->RunScript(&m_asScripts[SCRIPT_ON_FAIL_TO_OPEN], m_nId, 1);
                if (m_bLocked)
                {
                    int bFound = 0;
                    CExoLinkedListNode* pPosition = m_lstActions.GetHeadPos();
                    while (pPosition != NULL)
                    {
                        CSWSObjectActionNode* pAction = m_lstActions.GetAtPos(pPosition);
                        if (pAction != NULL && pAction->m_nActionType == ACTION_OPEN_DOOR)
                        {
                            unsigned long oidTarget = (unsigned long)pAction->m_apParam[0];
                            if (oidTarget == m_nId)
                            {
                                bFound = 1;
                                break;
                            }
                        }
                        m_lstActions.GetNext(pPosition);
                    }
                    if (!bFound && !pEvent->GetInteger(0))
                    {
                        CSWSCreature* pCreature = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidCaller);
                        if (pCreature != NULL)
                        {
                            CSWCCMessageData* pData = new CSWCCMessageData;
                            pData->SetObjectID(0, m_nId);
                            pCreature->SendFeedbackMessage(CSWSCreature::FEEDBACK_OBJECT_LOCKED, pData);
                        }
                    }
                }
                break;
            }
        case CSWSSCRIPTEVENT_EVENTTYPE_ON_DAMAGED:
            m_oidLastDamager = oidCaller;
            SetLastHostileActor(oidCaller, 0);
            g_pVirtualMachine->RunScript(&m_asScripts[SCRIPT_ON_DAMAGED], m_nId, 1);
            break;
        case CSWSSCRIPTEVENT_EVENTTYPE_ON_DEATH:
            m_n154 = oidCaller;
            g_pVirtualMachine->RunScript(&m_asScripts[SCRIPT_ON_DEATH], m_nId, 1);
            break;
        case CSWSSCRIPTEVENT_EVENTTYPE_ON_DISARM:
            if (m_bTrapFlag)
            {
                m_bTrapFlag = 0;
                m_oidLastDisarmedBy = oidCaller;
                g_pVirtualMachine->RunScript(&m_asScripts[SCRIPT_ON_DISARM], m_nId, 1);
            }
            if (m_lstTrapDetectedBy.num > 0)
                m_lstTrapDetectedBy.SetSize(0);
            break;
        case CSWSSCRIPTEVENT_EVENTTYPE_ON_HEARTBEAT:
            m_nLastUpdateSeconds = nCalendarDay;
            m_nLastUpdateMilliseconds = nTimeOfDay;
            g_pVirtualMachine->RunScript(&m_asScripts[SCRIPT_ON_HEARTBEAT], m_nId, 1);
            break;
        case CSWSSCRIPTEVENT_EVENTTYPE_ON_MINE_TRIGGERED:
            {
                int bForce = pEvent->GetInteger(0);
                int bTriggered = pEvent->GetInteger(1);
                int bBlastAttempt = pEvent->GetInteger(2);
                if (pEvent->GetInteger(4) != 0)
                {
                    m_bLocked = 0;
                    OpenDoor(m_nId);
                }
                else if (bBlastAttempt == 0 || bTriggered != 0)
                {
                    CSWSCreature* pCreature = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidCaller);
                    if ((pCreature == NULL || !pCreature->m_pStats->GetEffectImmunity(
                             CSWSEffectListHandler::IMMUNITY_TYPE_MINE, NULL)) &&
                        (bForce || (GetReputation(oidCaller, 0) < MIN_FRIENDLY_REPUTATION &&
                                    m_nFactionId != pCreature->m_pStats->m_nFactionId)) && m_bTrapFlag)
                    {
                        if (pCreature != NULL)
                            pCreature->SendFeedbackMessage(FEEDBACK_TRAP_TRIGGERED, NULL);
                        m_oidLastTriggeredBy = oidCaller;
                        g_pVirtualMachine->RunScript(&m_asScripts[SCRIPT_ON_TRAP_TRIGGERED], m_nId, 1);
                        if (m_bTrapOneShot)
                        {
                            m_bTrapFlag = 0;
                            if (GetArea() != NULL)
                            {
                                unsigned long oidDoor = m_nId;
                                CSWSArea* pArea = GetArea();
                                if (pArea->m_lstTrapObjectIds.Contains(oidDoor))
                                {
                                    unsigned long oidObject = m_nId;
                                    CSWSArea* pTrapArea = GetArea();
                                    pTrapArea->m_lstTrapObjectIds.Remove(oidObject);
                                }
                            }
                        }
                        if (m_lstTrapDetectedBy.num > 0)
                            m_lstTrapDetectedBy.SetSize(0);
                        CSWSTrigger* pTrapTrigger = m_pTrapTrigger;
                        if (pTrapTrigger != NULL)
                        {
                            pTrapTrigger->SetFadeOutTime(1);
                            pTrapTrigger->RemoveFromArea();
                            delete pTrapTrigger;
                        }
                    }
                }
                else if (m_bTrapFlag)
                {
                    CSWSCreature* pCreature = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidCaller);
                    int nRoll = pCreature->m_pStats->GetSkillRank(CSWRules::SKILL_DEMOLITIONS, NULL, 0) / 2 + TRAP_RECOVER_ROLL_BASE;
                    nRoll += pEvent->GetInteger(3);
                    int bSuccess = 0;
                    if (nRoll >= GetOpenLockDC())
                    {
                        bSuccess = 1;
                        CScriptEvent* pOpenEvent = new CScriptEvent;
                        pOpenEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_MINE_TRIGGERED;
                        pOpenEvent->SetInteger(4, 1);
                        g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                            0, 1000, oidCaller, m_nId, CServerAIMaster::EVENT_SIGNAL_EVENT, pOpenEvent);
                        CScriptEvent* pUnlockEvent = new CScriptEvent;
                        pUnlockEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_UNLOCKED;
                        g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                            0, 0, pCreature->m_nId, m_nId, CServerAIMaster::EVENT_SIGNAL_EVENT, pUnlockEvent);
                    }
                    CExoString sResult;
                    if (bSuccess)
                        sResult = g_pAppManager->GetClientExoApp()->GetGUIString(STRREF_BLAST_SUCCESS);
                    else
                        sResult = g_pAppManager->GetClientExoApp()->GetGUIString(STRREF_BLAST_FAILURE);
                    g_pTlkTable->SetCustomToken(1, sResult);
                    g_pTlkTable->SetCustomToken(2, CExoString(nRoll));
                    g_pTlkTable->SetCustomToken(3, CExoString("20"));
                    g_pTlkTable->SetCustomToken(4, CExoString(
                        pCreature->m_pStats->GetSkillRank(CSWRules::SKILL_DEMOLITIONS, NULL, 0)));
                    g_pTlkTable->SetCustomToken(5, CExoString(pEvent->GetInteger(3)));
                    g_pTlkTable->SetCustomToken(6, CExoString(GetOpenLockDC()));
                    CExoString sMessage = g_pSWTlkTable->Fetch(STRREF_BLAST_FEEDBACK, NULL, 0, 0);
                    g_pAppManager->GetClientExoApp()->GetInGameGui()->AppendToMsgBuffer(sMessage, 0x80, 0);
                    m_oidLastTriggeredBy = oidCaller;
                    m_bTrapFlag = 0;
                    if (GetArea() != NULL)
                    {
                        unsigned long oidDoor = m_nId;
                        CSWSArea* pArea = GetArea();
                        if (pArea->m_lstTrapObjectIds.Contains(oidDoor))
                        {
                            unsigned long oidObject = m_nId;
                            CSWSArea* pTrapArea = GetArea();
                            pTrapArea->m_lstTrapObjectIds.Remove(oidObject);
                        }
                    }
                    if (m_lstTrapDetectedBy.num > 0)
                        m_lstTrapDetectedBy.SetSize(0);
                    CSWSTrigger* pTrapTrigger = m_pTrapTrigger;
                    if (pTrapTrigger != NULL)
                    {
                        pTrapTrigger->SetFadeOutTime(1);
                        pTrapTrigger->RemoveFromArea();
                        delete pTrapTrigger;
                    }
                }
                break;
            }
        case CSWSSCRIPTEVENT_EVENTTYPE_ON_DIALOGUE:
            {
                m_oidLastSpeaker = oidCaller;
                m_nMatchedStringsNum = pEvent->GetInteger(1);
                m_sLastListenText = pEvent->GetString(0);
                g_pAppManager->GetServerExoApp()->SetLastConversation(pEvent->GetString(0));
                m_nConversationType = pEvent->GetInteger(2);
                g_pAppManager->GetServerExoApp()->SetActionConversation(1);
                g_pAppManager->GetServerExoApp()->SetIgnoreConversationStartRange(pEvent->GetInteger(3));
                ClearMatchedExpressionStrings();
                for (int i = 1; i < pEvent->GetInteger(0); ++i)
                    AddMatchedExpressionString(pEvent->GetString(i));
                CExoString* pScript = &m_asScripts[SCRIPT_ON_DIALOG];
                if (pScript == NULL || *pScript == "default")
                    m_asScripts[SCRIPT_ON_DIALOG] = "k_hen_dialogue01";
                g_pVirtualMachine->RunScript(&m_asScripts[SCRIPT_ON_DIALOG], m_nId, 1);
                break;
            }
        case CSWSSCRIPTEVENT_EVENTTYPE_ON_LOCKED:
            m_oidLastLockedBy = oidCaller;
            g_pVirtualMachine->RunScript(&m_asScripts[SCRIPT_ON_LOCK], m_nId, 1);
            break;
        case CSWSSCRIPTEVENT_EVENTTYPE_ON_UNLOCKED:
            m_oidLastUnlockedBy = oidCaller;
            g_pVirtualMachine->RunScript(&m_asScripts[SCRIPT_ON_UNLOCK], m_nId, 1);
            break;
        case CSWSSCRIPTEVENT_EVENTTYPE_ON_SPELLCASTAT:
            {
                m_n1D8 = pEvent->GetObjectID(0);
                m_n1DC = pEvent->GetInteger(0);
                m_n1E0 = pEvent->GetInteger(1);
                if (m_n1E0)
                {
                    SetLastHostileActor(m_n1D8, 0);
                    unsigned long oidCaster = m_n1D8;
                    CSWSCreature* pCreature = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidCaster);
                    if (pCreature != NULL)
                    {
                        pCreature->BroadcastCombatStateToParty(OBJECT_INVALID);
                        pCreature->SetExcitedState(2);
                        pCreature->RemoveCombatInvisibilityEffects();
                    }
                }
                g_pVirtualMachine->RunScript(&m_asScripts[SCRIPT_ON_SPELL_CAST_AT], m_nId, 1);
                break;
            }
        case CSWSSCRIPTEVENT_EVENTTYPE_ON_USER_DEFINED_EVENT:
            m_nCustomScriptEventId = pEvent->GetInteger(0);
            g_pVirtualMachine->RunScript(&m_asScripts[SCRIPT_ON_USER_DEFINED], m_nId, 1);
            break;
        case CSWSSCRIPTEVENT_EVENTTYPE_ON_CLICKED:
            {
                m_oidLastTriggeredBy = pEvent->GetObjectID(0);
                CSWSCreature* pCreature = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(pEvent->GetObjectID(0));
                if (GetIsLinked())
                {
                    CSWSArea* pArea = GetArea();
                    if (pCreature == NULL || g_pAppManager->GetClientExoApp()->GetInGameGui()->m_nGlobalDialogState)
                    {
                        if (pArea != NULL)
                            pArea->SetTransitionPending(0, 0);
                    }
                    else if (pArea != NULL && m_sLinkedToModule.GetLength() > 0)
                    {
                        if (pArea->m_bTransitionPending)
                        {
                            unsigned char nId = pArea->m_nCurrentTransitionPendingId;
                            if (pEvent->GetInteger(0) == nId)
                            {
                                if (pCreature->GetDead() || pCreature->GetDeadTemp())
                                {
                                    g_pAppManager->GetClientExoApp()->GetInGameGui()->StopGlobalFade();
                                    pArea->SetTransitionPending(0, 0);
                                }
                                else
                                {
                                    CServerExoApp* pServer = g_pAppManager->GetServerExoApp();
                                    pServer->SetMoveToModulePending(1);
                                    pServer->SetMoveToModuleString(m_sLinkedToModule);
                                    pServer->SetMoveToModuleStartWaypoint(m_sLinkedTo);
                                    pArea->SetTransitionPending(0, 0);
                                }
                            }
                        }
                        else if (pCreature->GetIsPC() ||
                            g_pAppManager->GetServerExoApp()->GetPartyTable()->GetIsLeader(pCreature->GetId()))
                        {
                            if (g_pAppManager->GetClientExoApp()->GetSWParty()->IsPartyTogether())
                            {
                                CScriptEvent* pNextEvent = new CScriptEvent;
                                unsigned char nId = pArea->GetNextTransitionPendingID();
                                pNextEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_CLICKED;
                                pNextEvent->SetObjectID(0, pEvent->GetObjectID(0));
                                pNextEvent->SetInteger(0, nId);
                                g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                                    0, 500, m_nId, m_nId, CServerAIMaster::EVENT_SIGNAL_EVENT, pNextEvent);
                                g_pAppManager->GetClientExoApp()->GetInGameGui()->StartGlobalFade(0, 0.0f, 0.5f, Vector());
                                pArea->SetTransitionPending(1, nId);
                                g_pAppManager->GetServerExoApp()->SetPauseState(2, 0);
                            }
                            else
                            {
                                CExoString sScript("k_trg_transfail");
                                g_pVirtualMachine->RunScript(&sScript, pCreature->GetId(), 1);
                            }
                        }
                    }
                }
                else
                {
                    if ((m_asScripts[SCRIPT_ON_CLICK].IsEmpty() || m_asScripts[SCRIPT_ON_CLICK] == "default") && GetIsLinked() == 1)
                        m_asScripts[SCRIPT_ON_CLICK] = "NW_G0_Transition";
                    CSWSPlayer* pPlayer = g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(pEvent->GetObjectID(0));
                    if (pPlayer != NULL && m_nLoadScreenId != 0)
                    {
                        CExoString sBMP("");
                        unsigned short nLoadScreenId = m_nLoadScreenId;
                        pPlayer->SetAreaTransitionBMP(nLoadScreenId, sBMP);
                    }
                    g_pVirtualMachine->RunScript(&m_asScripts[SCRIPT_ON_CLICK], m_nId, 1);
                }
                break;
            }
        }
        delete pEvent;
    }
    else if (nEventId == CServerAIMaster::EVENT_TIMED_EVENT)
    {
        if (pEventData != NULL)
            g_pVirtualMachine->RunScriptSituation(pEventData, m_nId, 1);
    }
    else if (nEventId == CServerAIMaster::EVENT_OPEN_OBJECT)
        OpenDoor(oidCaller);
    else if (nEventId == CServerAIMaster::EVENT_CLOSE_OBJECT)
    {
        m_oidLastCloser = oidCaller;
        SetOpenState(OPEN_STATE_CLOSED, 1);
        CSWSObject* pLinked = GetLinkedObject();
        if (pLinked != NULL && pLinked->AsSWSDoor() != NULL)
            pLinked->AsSWSDoor()->SetOpenState(OPEN_STATE_CLOSED, 1);
        g_pVirtualMachine->RunScript(&m_asScripts[SCRIPT_ON_CLOSED], m_nId, 1);
    }
    else if (nEventId == CServerAIMaster::EVENT_APPLY_EFFECT)
    {
        if (pEventData != NULL)
            ApplyEffect((CGameEffect*)pEventData, 0, 0);
    }
    else if (nEventId == CServerAIMaster::EVENT_DESTROY_OBJECT)
    {
        if (m_nEC)
        {
            if (!GetIsLinked())
            {
                RemoveFromArea();
                delete this;
            }
            else
            {
                if (GetArea() != NULL)
                    GetArea()->RemoveStaticBoundingBox(m_nId);
                SetOpenState(OPEN_STATE_DESTROYED, 1);
            }
        }
    }
    else if (nEventId == CServerAIMaster::EVENT_PLAY_ANIMATION)
    {
        if (pEventData != NULL && !GetDead())
            SetAnimation((int)pEventData);
    }
}
// PC 0x00583850
int CSWSDoor::LoadDoorExternal(CResGFF* pRes, CResStruct* pStruct,
                              int bLoadTemplate)
{
    int bSuccess;
    int bResult = 1;

    if (bLoadTemplate != 0)
    {
        CResRef cTemplate = pRes->ReadFieldCResRef(
            pStruct, "TemplateResRef", bSuccess, "");
        if (bSuccess == 0 || LoadFromTemplate(cTemplate) == 0)
        {
            return 0;
        }
        m_locTransitionDestination = pRes->ReadFieldCExoLocString(
            pStruct, "TransitionDestination", bSuccess,
            m_locTransitionDestination);
        m_sLinkedTo = pRes->ReadFieldCExoString(
            pStruct, "LinkedTo", bSuccess, "");
        m_IsLinked = pRes->ReadFieldBYTE(
            pStruct, "LinkedToFlags", bSuccess, 0);
        CExoString sString;
        CResRef cModule = pRes->ReadFieldCResRef(
            pStruct, "LinkedToModule", bSuccess, "");
        cModule.CopyToString(sString);
        m_sLinkedToModule = sString;
        m_bUseTweakColor = pRes->ReadFieldBYTE(pStruct, "UseTweakColor", bSuccess, 0);
        m_nTweakColor = pRes->ReadFieldDWORD(pStruct, "TweakColor", bSuccess, 0);
        sString = pRes->ReadFieldCExoString(pStruct, "Tag", bSuccess, "");
        if (bSuccess != 0)
            SetTag(sString);
    }
    else
    {
        bResult = LoadDoor(pRes, pStruct, 0);
    }
    if (bResult != 0)
    {
        CServerExoApp* pServerApp = g_pAppManager->m_pServerExoApp;
        pServerApp->GetModule()->AddObjectToLookupTable(m_sTag, m_nId);
    }
    return bResult;
}

#include "nwportrait.h"

static inline void NwsdoorTrace2()
{
    NwsdoorTrace1();
}

static inline void NwsdoorTrace1()
{
}

#ifdef _XBOX

int CSWSDoor::OpenDoor(unsigned long oidOpener)
{
    CGameObject* pGameObject;
    CSWSObject* pOpener;
    CSWSObject* pLinkedObject;
    int nState;

    nState = 2;
    pGameObject = g_pAppManager->m_pServerExoApp->GetGameObject(oidOpener);
    if (pGameObject != 0)
    {
        pOpener = pGameObject->AsSWSObject();
        Vector vDelta;
        vDelta = pOpener->m_vPosition - m_vPosition;
        if (vDelta * m_vOrientation > 0.0f)
        {
            nState = 1;
        }
    }
    m_oidLastOpener = oidOpener;
    SetOpenState(nState, 1);
    pLinkedObject = GetLinkedObject();
    if (pLinkedObject != 0 && pLinkedObject->AsSWSDoor() != 0)
    {
        pLinkedObject->AsSWSDoor()->SetOpenState(nState, 1);
    }
    return g_pVirtualMachine->RunScript(&m_asScripts[0], m_nId, 1);
}

int CSWSDoor::InDoor(Vector vPoint)
{
    int nCrossings;
    int nVertex;
    int nNext;
    float fIntersectY;

    nCrossings = 0;
    for (nVertex = 0; nVertex < m_nOutlineVertices; nVertex++)
    {
        nNext = nVertex != m_nOutlineVertices - 1 ? nVertex + 1 : 0;
        if (m_avOutline[nNext].m_fX != m_avOutline[nVertex].m_fX)
        {
            fIntersectY = m_avOutline[nVertex].m_fY;
            fIntersectY = fIntersectY - (m_avOutline[nVertex].m_fX - vPoint.m_fX) *
                ((m_avOutline[nNext].m_fY - fIntersectY) /
                 (m_avOutline[nNext].m_fX - m_avOutline[nVertex].m_fX));
            if (fIntersectY > vPoint.m_fY)
            {
                if (fabs(m_avOutline[nNext].m_fY - m_avOutline[nVertex].m_fY) >
                    fabs(m_avOutline[nNext].m_fX - m_avOutline[nVertex].m_fX))
                {
                    if (m_avOutline[nNext].m_fY > m_avOutline[nVertex].m_fY)
                    {
                        if (fIntersectY >= m_avOutline[nVertex].m_fY &&
                            fIntersectY < m_avOutline[nNext].m_fY)
                        {
                            nCrossings++;
                        }
                    }
                    else if (m_avOutline[nVertex].m_fY > m_avOutline[nNext].m_fY &&
                             fIntersectY > m_avOutline[nNext].m_fY &&
                             fIntersectY <= m_avOutline[nVertex].m_fY)
                    {
                        nCrossings++;
                    }
                }
                else
                {
                    if (m_avOutline[nNext].m_fX > m_avOutline[nVertex].m_fX)
                    {
                        if (vPoint.m_fX >= m_avOutline[nVertex].m_fX &&
                            vPoint.m_fX < m_avOutline[nNext].m_fX)
                        {
                            nCrossings++;
                        }
                    }
                    else if (m_avOutline[nVertex].m_fX > m_avOutline[nNext].m_fX &&
                             vPoint.m_fX > m_avOutline[nNext].m_fX &&
                             vPoint.m_fX <= m_avOutline[nVertex].m_fX)
                    {
                        nCrossings++;
                    }
                }
            }
        }
        if (m_avOutline[nNext].m_fX == vPoint.m_fX &&
            (m_avOutline[nNext].m_fY > vPoint.m_fY ||
             m_avOutline[nVertex].m_fY >= vPoint.m_fY))
        {
            nCrossings++;
        }
    }
    return nCrossings % 2 == 1;
}
#endif
