#include "swbehaviorcameracombat.h"

#include "appmanager.h"
#include "clientexoapp.h"
#include "clientexoappinternal.h"
#include "clientoptions.h"
#include "swparty.h"
#include "nwccreature.h"
#include "nwcobject.h"
#include "nwcanimbase.h"
#include "nwccreatureappearance.h"
#include "twodimarray.h"
#include "aurscene.h"
#include "aurcamera.h"
#include "nwcanimbase.h"
#include "swminigame.h"
#include "math3d.h"
#include "exobase.h"
#include <math.h>
#include "nwrules.h"
// PC 0x0073F790 GOG 0x0073F610
CSWBehaviorCameraCombat::CSWBehaviorCameraCombat(CAurObject* pObject)
    : CAurBehavior(pObject)
{
    m_fPitchOffset = 0;
    m_fYawOffset = 0;
#ifndef _XBOX
    m_fTurn = 0;
    m_fTurnAcceleration = 0;
#endif

    CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
    EXOXBOXASSERT(p2DArrays->m_pCameraStyle);
    C2DA* p2DA = p2DArrays->m_pCameraStyle;
    {
        CExoString sColumn("DISTANCE");
        p2DA->GetFLOATEntry(CTwoDimArrays::ROW2DA_CAMERASTYLE_COMBAT, sColumn,
                           &m_fDistance);
    }
    {
        CExoString sColumn("HEIGHT");
        p2DA->GetFLOATEntry(CTwoDimArrays::ROW2DA_CAMERASTYLE_COMBAT, sColumn,
                           &m_fHeight);
    }
    {
        CExoString sColumn("PITCH");
        p2DA->GetFLOATEntry(CTwoDimArrays::ROW2DA_CAMERASTYLE_COMBAT, sColumn,
                           &m_fPitch);
    }
    {
        CExoString sColumn("ROTATION");
        p2DA->GetFLOATEntry(CTwoDimArrays::ROW2DA_CAMERASTYLE_COMBAT, sColumn,
                           &m_fRotation);
    }
    {
        CExoString sColumn("SPEED");
        p2DA->GetFLOATEntry(CTwoDimArrays::ROW2DA_CAMERASTYLE_COMBAT, sColumn,
                           &m_fSpeed);
    }
    float fViewAngle;
    {
        CExoString sColumn("VIEWANGLE");
        p2DA->GetFLOATEntry(CTwoDimArrays::ROW2DA_CAMERASTYLE_COMBAT, sColumn,
                           &fViewAngle);
    }

    static_cast<Gob*>(m_pObject)->AsCAurCamera()->SetViewAngle(fViewAngle);

#ifndef _XBOX
    m_Acceleration.m_fMaximumVelocity =
        g_pAppManager->m_pClientExoApp->GetClientOptions()
            ->GetCameraKeyboardDPS();
    m_Acceleration.m_fMaximumAcceleration =
        g_pAppManager->m_pClientExoApp->GetClientOptions()
            ->GetCameraKeyboardAcceleration();
    float fDeceleration =
        g_pAppManager->m_pClientExoApp->GetClientOptions()
            ->GetCameraKeyboardDeceleration();
    m_Acceleration.m_fMaximumDeceleration = fDeceleration;
    m_bMouseCameraRotateToggle = g_bMouseCameraRotateToggle;
#endif
}

#ifndef _XBOX
Vector CSWBehaviorCameraCombat::CalculateDesiredLocation(  // PC 0x0073F9C0 GOG 0x0073F840
    CSWCCreature* pCreature, CSWCObject* pTarget, float fTimeStep,
    float& fYaw)
{
    CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
    CAurCamera* pCamera = pClientExoApp->GetModuleCamera();
    Vector vResult;
    Vector vDirection;
    Vector vPosition = pCreature->m_vPosition;

    float fTurn;
    float fPitch = 0.0f;
    float fRoll = 0.0f;
    if (m_fTurn == 0.0f)
    {
        Vector vAcceleration(0.0f, m_fTurnAcceleration, 0.0f);
        fTurn = m_Acceleration.Update(vAcceleration, fTimeStep, 1, 0, 0, 0)
                    .m_vRate.m_fY;
    }
    else
    {
        fTurn = m_fTurn;
        m_Acceleration.SetVelocity(m_fTurn);
        m_fTurnAcceleration = 0;
    }

    Quaternion qOrientation =
        YawPitchRoll(fTurn, fPitch, fRoll) * pCamera->GetOrientation();
    fYaw = Yaw(qOrientation);

    vDirection = Vector(2.0f * (qOrientation.m_fX * qOrientation.m_fY -
                                qOrientation.m_fW * qOrientation.m_fZ),
                        1.0f - 2.0f * (qOrientation.m_fX * qOrientation.m_fX +
                                       qOrientation.m_fZ * qOrientation.m_fZ),
                        2.0f * (qOrientation.m_fW * qOrientation.m_fX +
                                qOrientation.m_fY * qOrientation.m_fZ));
    vDirection.m_fZ = 0.0f;
    vDirection.Normalize();

    vPosition.m_fZ = 0.0f;
    vResult = vPosition + -m_fDistance * vDirection;
    vResult.m_fZ = pCreature->m_pAppearance->GetHeadHeightOffset() +
                   pCreature->m_vPosition.m_fZ + m_fHeight;
    return vResult;
}
// PC 0x0073FC40 GOG 0x0073FAC0
float CSWBehaviorCameraCombat::GetNextYaw(const Vector& vDirection, float fYaw,
                                          float fTimeStep, float& fTurn)
{
    Quaternion qOrientation = ((Gob*)m_pObject)->GetOrientation();
    if (Yaw(qOrientation) != fYaw)
    {
        CClientOptions* pOptions =
            g_pAppManager->GetClientExoApp()->GetClientOptions();
        pOptions->GetMouseSenSetting();
    }
    CClientOptions* pOptions =
        g_pAppManager->GetClientExoApp()->GetClientOptions();
    pOptions->GetMouseSenSetting();
    fTurn = 0.0f;
    return fYaw;
}
#endif
// PC 0x0073FCB0 GOG 0x0073FB30
Vector CSWBehaviorCameraCombat::GetNextPosition(const Vector& vTarget,
                                                float fTimeStep, float fSpeed)
{
    Vector vDelta = vTarget - ((Gob*)m_pObject)->GetPosition();
    Vector vResult = vTarget;
    if (fTimeStep > 0.001f)
    {
        float fDistanceSquared = vDelta * vDelta;
        if (fDistanceSquared > 0.0099999998f)
        {
            float fScale = sqrt(fDistanceSquared) / fTimeStep * fSpeed;
            vResult = normalize(vDelta) * fScale +
                      ((Gob*)m_pObject)->GetPosition();
        }
    }
    return vResult;
}
// PC 0x0073FDC0 GOG 0x0073FC40
int CSWBehaviorCameraCombat::SetTurn(float fTurn)
{
    int bToggle = g_bMouseCameraRotateToggle;
    CClientOptions* pOptions =
        g_pAppManager->GetClientExoApp()->GetClientOptions();
    if (pOptions->m_bMouseLook)
    {
        bToggle = (bToggle == 0);
    }
    if (bToggle != 0)
    {
        m_fTurn = fTurn;
    }
    else
    {
        m_fTurn = 0;
    }
    return (int)pOptions;
}

CSWMGBehaviorBullet::~CSWMGBehaviorBullet()
{
}
// PC 0x0073FE10 GOG 0x0073FCB0
Vector CSWBehaviorCameraCombat::DetermineSafePosition(CSWCCreature* pCreature,
                                                      const Vector& vDesired,
                                                      const Vector& vCurrent)
{
    Vector vHit;
    CHitInfo hitInfo(0, ~CHitInfo::HIT_TRIGGER, 0);
    hitInfo.m_bResult0 = 1;
    Vector vSafe = vDesired;
    hitInfo.ClearOutput();

    Vector vOffset =
        normalize(cross(zaxis, vCurrent - vSafe)) * CameraPersonalSpace;
    int bHit = 0;
    float fDistance = 0.0f;

    Vector vProbe = vSafe + vOffset;
    hitInfo.ClearOutput();
    hitInfo.m_bResult2 = 1;
    if (((Gob*)m_pObject)->GetScene()->HitCheck(vCurrent, vProbe, &vHit,
                                                &hitInfo))
    {
        bHit = 1;
        fDistance = max(fDistance, magnitude(vHit - vProbe));
    }

    vProbe = vSafe - vOffset;
    hitInfo.ClearOutput();
    hitInfo.m_bResult2 = 1;
    if (((Gob*)m_pObject)->GetScene()->HitCheck(vCurrent, vProbe, &vHit,
                                                &hitInfo))
    {
        bHit = 1;
        fDistance = max(fDistance, magnitude(vHit - vProbe));
    }

    vOffset = Vector(0.0f, 0.0f, 1.0f) * CameraPersonalSpace;
    vProbe = vSafe + vOffset;
    hitInfo.ClearOutput();
    hitInfo.m_bResult2 = 1;
    if (((Gob*)m_pObject)->GetScene()->HitCheck(vCurrent, vProbe, &vHit,
                                                &hitInfo))
    {
        bHit = 1;
        fDistance = max(fDistance, magnitude(vHit - vProbe));
    }

    vProbe = vSafe - vOffset;
    hitInfo.ClearOutput();
    hitInfo.m_bResult2 = 1;
    if (((Gob*)m_pObject)->GetScene()->HitCheck(vCurrent, vProbe, &vHit,
                                                &hitInfo))
    {
        bHit = 1;
        fDistance = max(fDistance, magnitude(vHit - vProbe));
    }

    if (bHit)
    {
        Vector vDelta = vDesired - vCurrent;
        float fTargetDistance = magnitude(vDelta);
        vSafe -= fDistance * normalize(vDelta);

        vSafe += normalize(pCreature->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1)
                             ->GetOrientation()
                             .zdir()) *
                 ((CameraPersonalSpace + 0.15f) *
                  (fDistance / fTargetDistance));
    }

    return vSafe;
}
// PC 0x00740540 GOG 0x007403E0
int CSWBehaviorCameraCombat::Control(float fTimeStep)
{
    int nResult = CAurBehavior::Control(fTimeStep);
    if (nResult == 0)
    {
        return nResult;
    }

#ifndef _XBOX
    m_Acceleration.m_fMaximumVelocity =
        g_pAppManager->m_pClientExoApp->GetClientOptions()
            ->GetCameraKeyboardDPS();
    m_Acceleration.m_fMaximumAcceleration =
        g_pAppManager->m_pClientExoApp->GetClientOptions()
            ->GetCameraKeyboardAcceleration();
    m_Acceleration.m_fMaximumDeceleration =
        g_pAppManager->m_pClientExoApp->GetClientOptions()
            ->GetCameraKeyboardDeceleration();
#endif
    CSWParty* pSWParty = g_pAppManager->m_pClientExoApp->GetSWParty();
    CSWCCreature* pPlayerCharacter = pSWParty->GetPlayerCharacter();
    CClientExoApp* pApp = g_pAppManager->m_pClientExoApp;
    unsigned long nLastTarget = pApp->GetLastTarget();
    CSWCObject* pGameObject = pApp->GetGameObject(nLastTarget);
#ifndef _XBOX
    if (RenderQA != 0)
    {
        if (m_pObject != 0 && pPlayerCharacter != 0)
        {
            Vector vA = ((Gob*)m_pObject)->GetPosition();
            Vector vB = ((Gob*)m_pObject)->GetPosition();
            Vector vC = ((Gob*)m_pObject)->GetPosition();
            magnitude(((Gob*)m_pObject)->GetPosition() -
                      pPlayerCharacter->m_vPosition);
        }
    }

    int bToggle = g_bMouseCameraRotateToggle;
    if (g_pAppManager->m_pClientExoApp->GetClientOptions()->m_bMouseLook)
    {
        bToggle = (bToggle == 0);
    }
    if (bToggle != 0)
    {
        if (m_bMouseCameraRotateToggle == 0)
        {
#ifndef _XBOX
            g_pGuiMan->StoreCurrentMousePosition();
#endif
            m_bMouseCameraRotateToggle = bToggle;
            g_pAppManager->m_pClientExoApp->HideMouse(MOUSE_HIDE_CAMERA_ROTATE);
        }
    }
    else if (m_bMouseCameraRotateToggle != 0)
    {
#ifndef _XBOX
        g_pGuiMan->RestoreOldMousePosition();
#endif
        m_bMouseCameraRotateToggle = 0;
        g_pAppManager->m_pClientExoApp->ShowMouse(MOUSE_HIDE_CAMERA_ROTATE);
    }
    if (g_pAppManager->m_pClientExoApp->GetInputClass() == INPUT_CLASS_GUI)
    {
        m_bMouseCameraRotateToggle = 0;
        g_pAppManager->m_pClientExoApp->ShowMouse(MOUSE_HIDE_CAMERA_ROTATE);
    }
#endif

    if (pGameObject == 0)
    {
        pGameObject = pPlayerCharacter;
    }
    if (pPlayerCharacter != 0 && pGameObject != 0 &&
        pGameObject->AsSWCObject() != 0)
    {
        Vector vPlayerPosition = pPlayerCharacter->m_vPosition;
        vPlayerPosition.m_fZ =
            pPlayerCharacter->m_pAppearance->GetHeight() + vPlayerPosition.m_fZ;
        float fHeight;
        Vector vDesired = CalculateDesiredLocation(
            pPlayerCharacter, pGameObject->AsSWCObject(), fTimeStep, fHeight);
        Vector vSafe =
            DetermineSafePosition(pPlayerCharacter, vDesired, vPlayerPosition);
        float fTurn = 0.0f;
        float fNextYaw = GetNextYaw(vSafe, fHeight, fTimeStep, fTurn);
        Vector vNextPosition = GetNextPosition(vSafe, fTurn, fTimeStep);
        Quaternion qOrientation = YawPitchRoll(
            fNextYaw + m_fYawOffset, m_fPitch + m_fPitchOffset, 0.0f);
        ((Gob*)m_pObject)->SetOrientation(qOrientation);
        ((Gob*)m_pObject)->SetPosition(vNextPosition);
    }
    return 1;
}
// PC 0x00740840 GOG 0x007406E0
void CSWBehaviorCameraCombat::PutAtDesiredLocation()
{
    CSWParty* pSWParty = g_pAppManager->m_pClientExoApp->GetSWParty();
    CSWCCreature* pPlayerCharacter = pSWParty->GetPlayerCharacter();
    CClientExoApp* pApp = g_pAppManager->m_pClientExoApp;
    unsigned long nLastTarget = pApp->GetLastTarget();
    CSWCObject* pGameObject = pApp->GetGameObject(nLastTarget);
    g_pAppManager->m_pClientExoApp->GetModuleCamera();
    if (pPlayerCharacter != 0 && pGameObject != 0)
    {
        if (pGameObject->AsSWCObject() != 0)
        {
            Vector vPlayerPosition = pPlayerCharacter->m_vPosition;
            CSWCObject* pSelf = pGameObject->AsSWCObject();
            Vector vCameraPosition = pSelf->m_vPosition;
            Vector vDelta(vCameraPosition.m_fX - vPlayerPosition.m_fX,
                          vCameraPosition.m_fY - vPlayerPosition.m_fY,
                          vCameraPosition.m_fZ - vPlayerPosition.m_fZ);
            float fYaw = Yaw(vDelta);
            vDelta.Normalize();
            float fScale = -m_fDistance;
            Vector vNewPosition = fScale * vDelta + vPlayerPosition;
            Vector vSafe = DetermineSafePosition(pPlayerCharacter, vNewPosition,
                                                 vPlayerPosition);
            Quaternion qOrientation = YawPitchRoll(fYaw, m_fPitch, 0.0f);
            ((Gob*)m_pObject)->SetOrientation(qOrientation);
            ((Gob*)m_pObject)->SetPosition(vSafe);
        }
    }
}

#ifdef _XBOX

Vector CSWBehaviorCameraCombat::CalculateDesiredLocation(
    CSWCCreature* pCreature, CSWCObject* pTarget, float fTimeStep,
    float& fYaw)
{
    CAurCamera* pCamera = g_pAppManager->m_pClientExoApp->GetModuleCamera();
    Vector vResult;
    Vector vDirection;
    Vector vPosition = pCreature->m_vPosition;

    float fTurn = m_fRotation * m_fTurn * fTimeStep;
    Quaternion qOrientation =
        YawPitchRoll(fTurn, 0.0f, 0.0f) * pCamera->GetOrientation();
    float fCameraYaw = Yaw(qOrientation);
    float fTargetYaw = Yaw(pTarget->m_vPosition - pCamera->GetPosition());

    float fDelta = fCameraYaw - fTargetYaw;
    float fAngle = (float)fabs(fDelta);
    while (fAngle < 0.0f)
        fAngle += 360.0f;
    while (fAngle >= 360.0f)
        fAngle -= 360.0f;
    if (fAngle > 180.0f)
        fAngle = 360.0f - fAngle;
    if (fAngle > 25.0f)
    {
        float fOffset = 25.0f;
        if (fCameraYaw > fTargetYaw)
        {
            if (fDelta > 180.0f)
                fOffset = -25.0f;
        }
        else if (fTargetYaw - fCameraYaw < 180.0f)
        {
            fOffset = -25.0f;
        }
        fYaw = fTargetYaw + fOffset;
        qOrientation = YawPitchRoll(fYaw, 0.0f, 0.0f);
    }
    else
    {
        fYaw = fCameraYaw;
    }

    vDirection = Vector(2.0f * (qOrientation.m_fX * qOrientation.m_fY -
                                qOrientation.m_fW * qOrientation.m_fZ),
                        1.0f - 2.0f * (qOrientation.m_fX * qOrientation.m_fX +
                                       qOrientation.m_fZ * qOrientation.m_fZ),
                        2.0f * (qOrientation.m_fW * qOrientation.m_fX +
                                qOrientation.m_fY * qOrientation.m_fZ));
    vDirection.m_fZ = 0.0f;
    vDirection.Normalize();

    vPosition.m_fZ = 0.0f;
    vResult = vPosition + -m_fDistance * vDirection;
    vResult.m_fZ = pCreature->m_pAppearance->GetHeadHeightOffset() +
                   pCreature->m_vPosition.m_fZ + m_fHeight;
    return vResult;
}

float CSWBehaviorCameraCombat::GetNextYaw(const Vector& vDirection, float fYaw,
                                          float fTimeStep, float& fTurn)
{
    Quaternion qOrientation = ((Gob*)m_pObject)->GetOrientation();
    float fCurrent = Yaw(qOrientation);
    float fResult = fYaw;
    float fRemaining = 0.0f;
    if (fCurrent != fYaw)
    {
        float fStep = m_fRotation * fTimeStep;
        int bForward;
        if (fCurrent > fYaw)
        {
            fRemaining = fCurrent - fYaw;
            if (fRemaining > 180.0f)
            {
                fRemaining = 360.0f - fRemaining;
                bForward = 1;
            }
            else
            {
                bForward = 0;
            }
        }
        else
        {
            fRemaining = fYaw - fCurrent;
            if (fRemaining > 180.0f)
            {
                fRemaining = 360.0f - fRemaining;
                bForward = 0;
            }
            else
            {
                bForward = 1;
            }
        }
        if (fRemaining > fStep)
        {
            if (bForward)
                fStep *= -1.0f;
            fResult = fCurrent - fStep;
            if (fResult >= 360.0f)
                fResult -= 360.0f;
            else if (fResult < 0.0f)
                fResult += 360.0f;
        }
    }
    fTurn = fRemaining / m_fRotation;
    if (fTimeStep > fTurn)
    {
        fTurn = 0.0f;
        fResult = fYaw;
    }
    return fResult;
}
#endif
