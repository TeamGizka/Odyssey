#include <aurlist.h>
#include "swbehaviorcameradialog.h"

#include "aurcamera.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "nwcobject.h"
#include "nwccreature.h"
#include "nwcanimbase.h"
#include "aurtexman.h"
#include "swbehaviorcamera.h"
#include "nwcplaceable.h"
#include "nwcdoor.h"

static inline void SwbehaviorcameradialogTrace5();
static inline void SwbehaviorcameradialogTrace4();
static inline void SwbehaviorcameradialogTrace3();
static inline void SwbehaviorcameradialogTrace2();
static inline void SwbehaviorcameradialogTrace1();
// PC 0x005EE1C0 GOG 0x005EE1A0
void CSWBehaviorCameraDialog::Message(char* sMessage)
{
}
// PC 0x0070FE50 GOG 0x0070FBA0
void CSWBehaviorCameraDialog::SetShot(int nShot, Vector vPosition,
                                      Quaternion qOrientation, float fCameraYaw)
{
    EXOXBOXASSERT(nShot == FIXED_CAMERA_SHOT);
    if (nShot == FIXED_CAMERA_SHOT)
    {
        m_nActiveShot = 0;
        if (((Gob*)m_pObject)->AsCAurCamera() != 0)
        {
            ((Gob*)m_pObject)->AsCAurCamera()->SetViewAngle(fCameraYaw);
        }
        m_nShot = FIXED_CAMERA_SHOT;
        m_vCameraPosition = vPosition;
        m_qCameraOrientation = qOrientation;
    }
}
// PC 0x0070FED0 GOG 0x0070FC20
void CSWBehaviorCameraDialog::ApplyCameraShot()
{
    ((Gob*)m_pObject)->SetPosition(m_vCameraPosition);
    ((Gob*)m_pObject)->SetOrientation(m_qCameraOrientation);
}
// PC 0x0070FF30 GOG 0x0070FC80
int CSWBehaviorCameraDialog::GetLineOfActionSide(unsigned long oidFirst,
                                                 unsigned long oidSecond)
{
    for (int i = 0; i < 4; i++)
    {
        if (m_aLineOfAction[i].m_nSide == 0)
        {
            return 0;
        }
        if (m_aLineOfAction[i].m_oidSecond == oidFirst &&
            m_aLineOfAction[i].m_oidFirst == oidSecond)
        {
            return m_aLineOfAction[i].m_nSide;
        }
        if (m_aLineOfAction[i].m_oidSecond == oidSecond &&
            m_aLineOfAction[i].m_oidFirst == oidFirst)
        {
            return m_aLineOfAction[i].m_nSide == 2 ? 1 : 2;
        }
    }
    return 0;
}
// PC 0x0070FF90 GOG 0x0070FCE0
CSWBehaviorCameraDialog::CSWBehaviorCameraDialog(CAurObject* pObject,
                                                 int nDialogType)
    : CAurBehavior(pObject)
{
    m_fFirstHeight = 0.0f;
    m_fSecondHeight = 0.0f;
    m_fFirstOffset = 0.0f;
    m_fSecondOffset = 0.0f;
    m_fCameraYaw = 0.0f;
    m_pFirstObject = 0;
    m_pSecondObject = 0;
#ifdef _XBOX
    m_nActiveShot = 0;
    m_nDialogType = nDialogType;
    m_fYawOffset = 0.0f;
    m_fPitchOffset = 0.0f;
#else
    m_nDialogType = nDialogType;
    m_fYawOffset = 0.0f;
    m_fPitchOffset = 0.0f;
    m_nActiveShot = 0;
#endif

    for (int i = 0; i < 4; i++)
    {
        m_aLineOfAction[i].m_nSide = 0;
        m_aLineOfAction[i].m_oidFirst = CSWCameraLineOfAction::EMPTY_OBJECT_ID;
        m_aLineOfAction[i].m_oidSecond = CSWCameraLineOfAction::EMPTY_OBJECT_ID;
    }

    m_nCameraState = INITIAL_CAMERA_STATE;
}
// PC 0x00710070 GOG 0x0070FDC0
CSWBehaviorCameraDialog::~CSWBehaviorCameraDialog() throw()
{
}
// PC 0x007100D0 GOG 0x0070FE20
void CSWBehaviorCameraDialog::GetCameraParameters(unsigned long nShot,
                                                unsigned long nSide,
                                                Vector vFirst, Vector vSecond,
                                                Vector& vPosition,
                                                Quaternion& qOrientation)
{
    SwbehaviorcameradialogTrace5();
    Vector vDirection;
    Vector vLookAt;
    float fFraction;
    float fDistance;
    float fYaw;
    float fTargetHeight;
    float fFirstHeight;
    float fSecondHeight;
    float fCameraHeight;

    if (equal(vFirst, vSecond))
    {
        m_nActiveShot = 1;
        vSecond = vFirst + Vector(1.5f, 1.5f, 0.1f);
    }
    switch (nShot)
    {
    case OVER_SHOULDER_CAMERA_SHOT:
        fFraction = 0.3f;
        fDistance = 0.8f;
        fFirstHeight = -0.1f;
        fSecondHeight = -0.05f;
        fYaw = 30.0f;
        fTargetHeight = 0.0f;
        fCameraHeight = 0.0f;
        break;
    case TWO_PERSON_CAMERA_SHOT:
        fFraction = 0.5f;
        fDistance = 1.5f;
        fFirstHeight = 0.0f;
        fSecondHeight = 0.0f;
        fYaw = 90.0f;
        fTargetHeight = -0.2f;
        fCameraHeight = 0.3f;
        break;
    default:
        fFraction = nShot == CLOSE_UP_CAMERA_SHOT ? 1.0f : 1.0f;
        fDistance = 0.5f;
        fFirstHeight = 0.0f;
        fSecondHeight = 0.0f;
        fYaw = 30.0f;
        fTargetHeight = 0.0f;
        fCameraHeight = 0.0f;
        break;
    }
    if (nSide == 2)
    {
        fYaw = -fYaw;
    }
    if (m_nDialogType)
    {
        float fFirstHeadHeight = m_fFirstHeight;
        float fSecondHeadHeight = m_fSecondHeight;
        if (nShot != CLOSE_UP_CAMERA_SHOT)
        {
            float fLength = magnitude(vSecond - vFirst);
            fFirstHeight *= fLength;
            fSecondHeight *= fLength;
            float fTargetOffset = (fLength * fTargetHeight) + m_fSecondOffset;
            float fCameraOffset = (fLength * fCameraHeight) + m_fFirstOffset;
            vSecond.m_fZ = (vSecond.m_fZ + fSecondHeadHeight) + fSecondHeight;
            vFirst.m_fZ = (vFirst.m_fZ + fFirstHeadHeight) + fFirstHeight;
            vLookAt = fFraction * (vFirst - vSecond) + vSecond;
            vLookAt.m_fZ += fTargetOffset;
            vDirection = YawPitchRoll(fYaw, 0.0f, 0.0f) * (vFirst - vSecond);
            if (nShot == OVER_SHOULDER_CAMERA_SHOT)
            {
                vPosition = vLookAt - (fDistance * vDirection +
                                      0.15f * normalize(vDirection));
            }
            else if (nShot == TWO_PERSON_CAMERA_SHOT)
            {
                vPosition = vLookAt - fDistance * vDirection;
            }
            vPosition.m_fZ += fCameraOffset;
        }
        else
        {
            fYaw = nSide == 2 ? -30.0f : 30.0f;
            vFirst.m_fZ += fFirstHeadHeight;
            vSecond.m_fZ += fFirstHeadHeight;
            vLookAt = vFirst - 0.2f * normalize(vFirst - vSecond);
            vLookAt.m_fZ = vLookAt.m_fZ - 0.04f + m_fSecondOffset;
            vDirection = YawPitchRoll(fYaw, 0.0f, 0.0f) * (vFirst - vSecond);
            vDirection = normalize(vDirection);
            vPosition = vLookAt - 0.5f * normalize(vDirection);
            vPosition.m_fZ += m_fFirstOffset;
        }
        if (m_nActiveShot)
        {
            Vector vHit;
            CHitInfo hitInfo(0, ~CHitInfo::HIT_TRIGGER, 0);
            hitInfo.m_nType = 0;
            hitInfo.m_lstHits.Add(m_pFirstObject);
            hitInfo.m_lstHits.Add(m_pSecondObject);
            CSWCObject* pObject = g_pAppManager->GetClientExoApp()->GetGameObject(
                m_oidFirst);
            if (pObject && pObject->AsSWCObject() &&
                pObject->AsSWCObject()->GetAnimationBase())
            {
                CAurObject* pHead = pObject->AsSWCObject()->GetAnimationBase()
                    ->GetModel(CSWCAnimBase::MODEL_SLOT_HEAD);
                if (pHead)
                {
                    hitInfo.m_lstHits.Add(pHead);
                }
            }
            pObject = g_pAppManager->GetClientExoApp()->GetGameObject(m_oidSecond);
            if (pObject && pObject->AsSWCObject() &&
                pObject->AsSWCObject()->GetAnimationBase())
            {
                CAurObject* pHead = pObject->AsSWCObject()->GetAnimationBase()
                    ->GetModel(CSWCAnimBase::MODEL_SLOT_HEAD);
                if (pHead)
                {
                    hitInfo.m_lstHits.Add(pHead);
                }
            }
            if (((Gob*)m_pObject)->GetScene()->HitCheck(
                    vLookAt, vPosition, &vHit, &hitInfo))
            {
                vPosition = vHit - 0.1f * normalize(vHit - vLookAt);
            }
        }
    }
    else
    {
        if (nShot != CLOSE_UP_CAMERA_SHOT && m_nActiveShot != 1)
        {
            float fLength = magnitude(vSecond - vFirst);
            float fCameraOffset = (fCameraHeight * fLength) + m_fFirstOffset;
            vSecond.m_fZ = (fSecondHeight * fLength) + vSecond.m_fZ;
            vFirst.m_fZ = (fFirstHeight * fLength) + vFirst.m_fZ;
            vLookAt = fFraction * (vFirst - vSecond) + vSecond;
            vLookAt.m_fZ = ((fLength * fTargetHeight) + m_fSecondOffset) + vLookAt.m_fZ;
            vDirection = YawPitchRoll(fYaw, 0.0f, 0.0f) * (vFirst - vSecond);
            if (nShot == OVER_SHOULDER_CAMERA_SHOT)
            {
                vPosition = vLookAt - (fDistance * vDirection +
                                      0.15f * normalize(vDirection));
            }
            if (nShot == TWO_PERSON_CAMERA_SHOT)
            {
                vPosition = vLookAt - fDistance * vDirection;
            }
            vPosition.m_fZ += fCameraOffset;
        }
        else
        {
            fYaw = nSide == 2 ? -30.0f : 30.0f;
            vSecond.m_fZ = vFirst.m_fZ;
            vLookAt = vFirst - 0.2f * normalize(vFirst - vSecond);
            vLookAt.m_fZ = (vLookAt.m_fZ - 0.04f) + GetSecondOffset();

            vDirection = YawPitchRoll(fYaw, 0.0f, 0.0f) * (vFirst - vSecond);
            vDirection = normalize(vDirection);
            float fYawOffset = 0.0f;
            if (m_nActiveShot == 0)
            {
                fYawOffset = m_fCameraYaw;
            }
            float fYawHeight = 0.2f * fYawOffset;
            vLookAt.m_fZ -= fYawHeight;
            float fCloseDistance = (fYawOffset + 0.5f);
            vPosition = vLookAt - fCloseDistance * normalize(vDirection);
            vPosition.m_fZ = (fYawHeight + m_fFirstOffset) + vPosition.m_fZ;
        }
    }
    qOrientation = YawPitchRoll(Yaw(normalize(vDirection)),
                                Pitch(normalize(vLookAt - vPosition)) + 90.0f,
                                0.0f);
}
// PC 0x00711390 GOG 0x007110E0
int CSWBehaviorCameraDialog::GetSideUnobstructed()
{
    Vector vHookPosition;
    Quaternion qOrientation;
    Vector vFirst = ((Gob*)(CAurObject*)m_pFirstObject)->GetPosition();
    Vector vSecond = ((Gob*)(CAurObject*)m_pSecondObject)->GetPosition();
    vFirst.m_fZ += 0.1f;
    vSecond.m_fZ += 0.1f;
    Vector vMiddle = 0.5f * (vSecond - vFirst) + vFirst;
    vMiddle.m_fZ = (m_fFirstHeight - m_fSecondHeight) * 0.5f +
                  m_fFirstHeight + vMiddle.m_fZ;
    if (!m_nDialogType)
    {
        vFirst.m_fZ += m_fFirstHeight;
        vSecond.m_fZ += m_fSecondHeight;
        if (((Gob*)(CAurObject*)m_pFirstObject)->GetPartPosition(
                "CAMERAHOOK", &vHookPosition, &qOrientation))
        {
            vFirst = vHookPosition;
        }
        if (((Gob*)(CAurObject*)m_pSecondObject)->GetPartPosition(
                "CAMERAHOOK", &vHookPosition, &qOrientation))
        {
            vSecond = vHookPosition;
        }
    }
    Vector vPosition;
    Vector vHit;
    CHitInfo hitInfo(0, ~CHitInfo::HIT_TRIGGER, 0);
    hitInfo.m_nType = 0;
    hitInfo.m_lstHits.Add(m_pFirstObject);
    hitInfo.m_lstHits.Add(m_pSecondObject);
    CSWCObject* pObject = g_pAppManager->GetClientExoApp()->GetGameObject(
        m_oidFirst);
    if (pObject && pObject->AsSWCObject() &&
        pObject->AsSWCObject()->GetAnimationBase())
    {
        CAurObject* pHead = pObject->AsSWCObject()->GetAnimationBase()->GetModel(
            CSWCAnimBase::MODEL_SLOT_HEAD);
        if (pHead)
        {
            hitInfo.m_lstHits.Add(pHead);
        }
    }
    pObject = g_pAppManager->GetClientExoApp()->GetGameObject(m_oidSecond);
    if (pObject && pObject->AsSWCObject() &&
        pObject->AsSWCObject()->GetAnimationBase())
    {
        CAurObject* pHead = pObject->AsSWCObject()->GetAnimationBase()->GetModel(
            CSWCAnimBase::MODEL_SLOT_HEAD);
        if (pHead)
        {
            hitInfo.m_lstHits.Add(pHead);
        }
    }

    GetCameraParameters(TWO_PERSON_CAMERA_SHOT, 2, vFirst, vSecond,
                        vPosition, qOrientation);
    int nSecondSideHits = ((Gob*)m_pObject)->GetScene()->HitCheck(
        vMiddle, vPosition, &vHit, &hitInfo);
    hitInfo.ClearOutput();
    if (!m_nDialogType)
    {
        nSecondSideHits += ((Gob*)m_pObject)->GetScene()->HitCheck(
            vFirst, vPosition, &vHit, &hitInfo);
        hitInfo.ClearOutput();
        nSecondSideHits += ((Gob*)m_pObject)->GetScene()->HitCheck(
            vSecond, vPosition, &vHit, &hitInfo);
        hitInfo.ClearOutput();
    }
    GetCameraParameters(OVER_SHOULDER_CAMERA_SHOT, 2, vFirst, vSecond,
                        vPosition, qOrientation);
    nSecondSideHits += ((Gob*)m_pObject)->GetScene()->HitCheck(
        vFirst, vPosition, &vHit, &hitInfo);
    hitInfo.ClearOutput();
    GetCameraParameters(OVER_SHOULDER_CAMERA_SHOT, 1, vSecond, vFirst,
                        vPosition, qOrientation);
    nSecondSideHits += ((Gob*)m_pObject)->GetScene()->HitCheck(
        vSecond, vPosition, &vHit, &hitInfo);
    hitInfo.ClearOutput();

    GetCameraParameters(TWO_PERSON_CAMERA_SHOT, 1, vFirst, vSecond,
                        vPosition, qOrientation);
    int nFirstSideHits = ((Gob*)m_pObject)->GetScene()->HitCheck(
        vMiddle, vPosition, &vHit, &hitInfo);
    hitInfo.ClearOutput();
    if (!m_nDialogType)
    {
        nFirstSideHits += ((Gob*)m_pObject)->GetScene()->HitCheck(
            vFirst, vPosition, &vHit, &hitInfo);
        hitInfo.ClearOutput();
        nFirstSideHits += ((Gob*)m_pObject)->GetScene()->HitCheck(
            vSecond, vPosition, &vHit, &hitInfo);
        hitInfo.ClearOutput();
    }
    GetCameraParameters(OVER_SHOULDER_CAMERA_SHOT, 1, vFirst, vSecond,
                        vPosition, qOrientation);
    nFirstSideHits += ((Gob*)m_pObject)->GetScene()->HitCheck(
        vFirst, vPosition, &vHit, &hitInfo);
    hitInfo.ClearOutput();
    GetCameraParameters(OVER_SHOULDER_CAMERA_SHOT, 2, vSecond, vFirst,
                        vPosition, qOrientation);
    nFirstSideHits += ((Gob*)m_pObject)->GetScene()->HitCheck(
        vSecond, vPosition, &vHit, &hitInfo);
    hitInfo.ClearOutput();
    if (nSecondSideHits > nFirstSideHits)
    {
        return 1;
    }
    else
    {
        return 2;
    }
}
// PC 0x00711C50 GOG 0x007119A0
int CSWBehaviorCameraDialog::AddCalculatedLineOfAction(unsigned long oidFirst,
                                                       unsigned long oidSecond)
{
    int i;
    int nSide;

    for (i = 0; i < 4; i++)
    {
        if (m_aLineOfAction[i].m_nSide == 0)
        {
            break;
        }
        if ((m_aLineOfAction[i].m_oidSecond == oidFirst &&
             m_aLineOfAction[i].m_oidFirst == oidSecond) ||
            (m_aLineOfAction[i].m_oidSecond == oidSecond &&
             m_aLineOfAction[i].m_oidFirst == oidFirst))
        {
            if (m_nDialogType == 0)
            {
                return GetLineOfActionSide(oidFirst, oidSecond);
            }
            return m_aLineOfAction[i].m_nSide;
        }
    }
    if (i == 4)
    {
        i = 0;
    }
    nSide = GetSideUnobstructed();
    m_aLineOfAction[i].m_nSide = nSide;
    m_aLineOfAction[i].m_oidFirst = oidSecond;
    m_aLineOfAction[i].m_oidSecond = oidFirst;
    return nSide;
}
// PC 0x00711CE0 GOG 0x00711A30
int CSWBehaviorCameraDialog::Control(float fTimeStep)
{
    int nSide;

    if (m_nCameraState == INITIAL_CAMERA_STATE &&
        !CAurBehavior::Control(fTimeStep))
    {
        return 1;
    }

    if (m_nShot != FIXED_CAMERA_SHOT)
    {
        if (!m_pFirstObject || !m_pSecondObject)
        {
            return 1;
        }

        nSide = GetLineOfActionSide(m_oidFirst, m_oidSecond);
        if (nSide == 0)
        {
            return 1;
        }
    }

    Vector vPosition;
    Quaternion qOrientation;

    {
        Vector vShotPosition = m_vCameraPosition;
        qOrientation = m_qCameraOrientation;

        if (m_nDialogType)
        {
            if (m_nShot != FIXED_CAMERA_SHOT)
            {
                GetCameraParameters(
                    m_nShot, nSide,
                    ((Gob*)(CAurObject*)m_pFirstObject)->GetPosition(),
                    ((Gob*)(CAurObject*)m_pSecondObject)->GetPosition(),
                    vShotPosition, qOrientation);
            }
        }
        else if (m_nShot != FIXED_CAMERA_SHOT)
        {
            Vector vHookPosition;
            Quaternion qHookOrientation;

            Vector vFirst = ((Gob*)(CAurObject*)m_pFirstObject)->GetPosition();
            Vector vSecond = ((Gob*)(CAurObject*)m_pSecondObject)->GetPosition();
            vFirst.m_fZ += 0.1f;
            vFirst.m_fZ += m_fFirstHeight;
            vSecond.m_fZ += 0.1f;
            vSecond.m_fZ += m_fSecondHeight;

            if (((Gob*)(CAurObject*)m_pFirstObject)
                    ->GetPartPosition("CAMERAHOOK", &vHookPosition,
                                      &qHookOrientation))
            {
                vFirst = vHookPosition;
            }
            if (((Gob*)(CAurObject*)m_pSecondObject)
                    ->GetPartPosition("CAMERAHOOK", &vHookPosition,
                                      &qHookOrientation))
            {
                vSecond = vHookPosition;
            }

            GetCameraParameters(m_nShot, nSide, vFirst, vSecond, vShotPosition,
                                qOrientation);
        }

        vPosition = vShotPosition;
    }

    ((Gob*)m_pObject)->SetPosition(vPosition);
    qOrientation = YawPitchRoll(Yaw(qOrientation) + m_fYawOffset,
                                Pitch(qOrientation) + m_fPitchOffset,
                                Roll(qOrientation));
    ((Gob*)m_pObject)->SetOrientation(qOrientation);

    return 1;
}
// PC 0x00711FF0 GOG 0x00711D40
void CSWBehaviorCameraDialog::SetShot(unsigned long oidFirst,
                                     unsigned long oidSecond,
                                     int const& nShot,
                                     float const& fFirstHeight,
                                     float const& fSecondHeight,
                                     float fCameraYaw)
{
    Vector vHookPosition;
    Quaternion qHookOrientation;

    if (m_nShot == TWO_PERSON_CAMERA_SHOT && nShot == TWO_PERSON_CAMERA_SHOT)
    {
        return;
    }
    if (nShot == FIXED_CAMERA_SHOT)
    {
        EXOXBOXASSERT(0);
        return;
    }

    m_nActiveShot = 0;
    if (((Gob*)m_pObject)->AsCAurCamera())
    {
        ((Gob*)m_pObject)->AsCAurCamera()->SetViewAngle(DEFAULT_DIALOG_VIEW_ANGLE);
    }

    CSWCObject* pObject = g_pAppManager->GetClientExoApp()->GetGameObject(oidFirst);
    if (!pObject)
    {
        EXOXBOXASSERT(0);
        return;
    }
    CSWCCreature* pCreature = pObject->AsSWCCreature();
    CSWCPlaceable* pPlaceable = pObject->AsSWCPlaceable();
    CSWCDoor* pDoor = pObject->AsSWCDoor();
    if (pCreature)
    {
        m_pFirstObject = pCreature->GetCameraFollowPart();
        m_fFirstHeight = pCreature->GetHeadHeightOffset();
    }
    else if (pPlaceable)
    {
        m_pFirstObject = pPlaceable->GetCameraFollowPart();
        m_fFirstHeight = pPlaceable->GetHeadHeightOffset();
    }
    else if (pDoor)
    {
        m_pFirstObject = pDoor->GetCameraFollowPart();
        m_fFirstHeight = pDoor->GetHeadHeightOffset();
    }
    else
    {
        EXOXBOXASSERT(0);
        return;
    }
    AddDependency(m_pFirstObject);

    pObject = g_pAppManager->GetClientExoApp()->GetGameObject(oidSecond);
    if (!pObject)
    {
        EXOXBOXASSERT(0);
        return;
    }
    pCreature = pObject->AsSWCCreature();
    pPlaceable = pObject->AsSWCPlaceable();
    pDoor = pObject->AsSWCDoor();
    if (pCreature)
    {
        m_pSecondObject = pCreature->GetCameraFollowPart();
        m_fSecondHeight = pCreature->GetHeadHeightOffset();
    }
    else if (pPlaceable)
    {
        m_pSecondObject = pPlaceable->GetCameraFollowPart();
        m_fSecondHeight = pPlaceable->GetHeadHeightOffset();
    }
    else if (pDoor)
    {
        m_pSecondObject = pDoor->GetCameraFollowPart();
        m_fSecondHeight = pDoor->GetHeadHeightOffset();
    }
    else
    {
        EXOXBOXASSERT(0);
        return;
    }
    AddDependency(m_pSecondObject);

    m_oidFirst = oidFirst;
    m_oidSecond = oidSecond;
    m_fFirstOffset = fFirstHeight;
    m_fSecondOffset = fSecondHeight;
    if (fCameraYaw < 0.001f && fCameraYaw > -0.001f)
    {
        fCameraYaw = 0.0f;
    }
    m_fCameraYaw = fCameraYaw;
    int nSide = AddCalculatedLineOfAction(oidFirst, oidSecond);

    Vector vPosition;
    Vector vHit;
    CHitInfo hitInfo(0, ~CHitInfo::HIT_TRIGGER, 0);
    hitInfo.m_nType = 0;
    hitInfo.m_lstHits.Add(m_pFirstObject);
    hitInfo.m_lstHits.Add(m_pSecondObject);

    pObject = g_pAppManager->GetClientExoApp()->GetGameObject(m_oidFirst);
    if (pObject && pObject->AsSWCObject() &&
        pObject->AsSWCObject()->GetAnimationBase())
    {
        CAurObject* pHead = pObject->AsSWCObject()->GetAnimationBase()->GetModel(
            CSWCAnimBase::MODEL_SLOT_HEAD);
        if (pHead)
        {
            hitInfo.m_lstHits.Add(pHead);
        }
    }
    pObject = g_pAppManager->GetClientExoApp()->GetGameObject(m_oidSecond);
    if (pObject && pObject->AsSWCObject() &&
        pObject->AsSWCObject()->GetAnimationBase())
    {
        CAurObject* pHead = pObject->AsSWCObject()->GetAnimationBase()->GetModel(
            CSWCAnimBase::MODEL_SLOT_HEAD);
        if (pHead)
        {
            hitInfo.m_lstHits.Add(pHead);
        }
    }

    Vector vFirst = ((Gob*)(CAurObject*)m_pFirstObject)->GetPosition();
    Vector vSecond = ((Gob*)(CAurObject*)m_pSecondObject)->GetPosition();
    vFirst.m_fZ += 0.1f;
    vSecond.m_fZ += 0.1f;
    Vector vMiddle = 0.5f * (vSecond - vFirst) + vFirst;
    vMiddle.m_fZ = (m_fFirstHeight - m_fSecondHeight) * 0.5f +
                  vMiddle.m_fZ + m_fFirstHeight;
    Quaternion qOrientation;
    if (m_nDialogType)
    {
        GetCameraParameters(nShot, nSide,
                            ((Gob*)(CAurObject*)m_pFirstObject)->GetPosition(),
                            ((Gob*)(CAurObject*)m_pSecondObject)->GetPosition(),
                            vPosition, qOrientation);
    }
    else
    {
        vFirst.m_fZ += m_fFirstHeight;
        vSecond.m_fZ += m_fSecondHeight;
        if (((Gob*)(CAurObject*)m_pFirstObject)->GetPartPosition(
                "CAMERAHOOK", &vHookPosition, &qHookOrientation))
        {
            vFirst = vHookPosition;
        }
        if (((Gob*)(CAurObject*)m_pSecondObject)->GetPartPosition(
                "CAMERAHOOK", &vHookPosition, &qHookOrientation))
        {
            vSecond = vHookPosition;
        }
        GetCameraParameters(nShot, nSide, vFirst, vSecond, vPosition, qOrientation);
    }

    int nHits;
    if (nShot == TWO_PERSON_CAMERA_SHOT)
    {
        nHits = ((Gob*)m_pObject)->GetScene()->HitCheck(vMiddle, vPosition,
                                                      &vHit, &hitInfo);
        hitInfo.ClearOutput();
        if (!m_nDialogType)
        {
            nHits += ((Gob*)m_pObject)->GetScene()->HitCheck(vFirst, vPosition,
                                                           &vHit, &hitInfo);
            hitInfo.ClearOutput();
            nHits += ((Gob*)m_pObject)->GetScene()->HitCheck(vSecond, vPosition,
                                                           &vHit, &hitInfo);
            hitInfo.ClearOutput();
        }
    }
    else
    {
        nHits = ((Gob*)m_pObject)->GetScene()->HitCheck(vFirst, vPosition,
                                                      &vHit, &hitInfo);
        hitInfo.ClearOutput();
    }
    if (nHits)
    {
        m_nActiveShot = 1;
    }
    m_nShot = nShot;
    m_nCameraState = 0;
    Control(0.0f);
    m_nCameraState = INITIAL_CAMERA_STATE;
}

static inline void SwbehaviorcameradialogTrace5()
{
    SwbehaviorcameradialogTrace4();
}

static inline void SwbehaviorcameradialogTrace4()
{
    SwbehaviorcameradialogTrace3();
}

static inline void SwbehaviorcameradialogTrace3()
{
    SwbehaviorcameradialogTrace2();
}

static inline void SwbehaviorcameradialogTrace2()
{
    SwbehaviorcameradialogTrace1();
}

static inline void SwbehaviorcameradialogTrace1()
{
}
