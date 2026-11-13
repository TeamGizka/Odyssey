#include <aurlist.h>
#include "swbehaviorcamera.h"

#include "aurcamera.h"
#include "nwcarea.h"
#include "nwcobject.h"
#include "nwcmodule.h"
#include "nwrules.h"
#include "twodimarrays.h"
#include "gameobjectarray.h"
#include "clientexoappinternal.h"
#include "guiingame.h"
#include "serverexoapp.h"

#include <float.h>
#include <math.h>
#include "twodimarray.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "clientoptions.h"

static inline void SwbehaviorcameraTrace5();
static inline void SwbehaviorcameraTrace4();
static inline void SwbehaviorcameraTrace3();
static inline void SwbehaviorcameraTrace2();
static inline void SwbehaviorcameraTrace1();

#define SW_CAMERA_QA_REPORT

float CameraPersonalSpace = 0.35f;
float CameraLookUp = 30.0f;
float CameraLookDown = 30.0f;
float CameraRotate = 90.0f;
int CameraStyle = -1;
int CameraFreeStyle = 0;
float CameraInterpAmt1 = 2.5f;

float CameraInterpAmt2 = 7.5f;
float CameraInterpAmt3 = 1.0f;
float CameraInterpDistAmt = 0.5f;
float DesiredOrientation = -1.0f;
float ScreenFramePercentage = 0.001f;

int RenderCameraLOS = 0;
int CameraOnAStickGroundHug = 0;
float fDelayAmount = 0.0f;
int OldCamera = 0;
int bCameraSlideBack = 0;
int CameraFreeStyleMode = CAMERA_FREE_STYLE_TRANSLATE;

float fBorderScaleExponent = 2.0f;
float fMaxTurnRate = 50.0f;
// PC 0x0068DB40
void CSWCameraOnAStick::Message(char* sMessage)
{
    if (stricmp("m_fDist", firstword(sMessage)) == 0)
    {
        Parse(sMessage + 7, m_fDistance);
    }
    if (stricmp("m_fSpeed", firstword(sMessage)) == 0)
    {
        Parse(sMessage + 8, m_fSpeed);
    }
    if (stricmp("m_fPresetPitch", firstword(sMessage)) == 0)
    {
#ifdef _XBOX
        Parse(sMessage + 8, m_fPresetPitch);
#else
        Parse(sMessage + 14, m_fPresetPitch);
#endif
    }
    if (stricmp("m_fHeight", firstword(sMessage)) == 0)
    {
        Parse(sMessage + 9, m_fHeight);
    }
}
// PC 0x0068DC00
Vector CSWCameraOnAStick::Control_ComputeDesiredPosition(Vector vPosition)
{
    Vector vOffset = m_fDistance * Vector(0.0f, 1.0f, 0.0f);
    vOffset = m_qLastOrientation * vOffset;
    Vector vDesiredPosition = vPosition - vOffset;
    return vDesiredPosition;
}
// PC 0x0068DD80
float CSWCameraOnAStick::Control_AccelerateInterp(float fStart, float fMinimum,
                                                  float fMaximum,
                                                  Vector vCurrent,
                                                  Vector vDesired,
                                                  float fAcceleration)
{
    float fResult = fStart;
    float fDistance =
        (vDesired.m_fZ - vCurrent.m_fZ) * (vDesired.m_fZ - vCurrent.m_fZ) +
        (vDesired.m_fY - vCurrent.m_fY) * (vDesired.m_fY - vCurrent.m_fY) +
        (vDesired.m_fX - vCurrent.m_fX) * (vDesired.m_fX - vCurrent.m_fX);

    if (fDistance > 1.0f)
    {
        fResult = fAcceleration * 3.0f + fStart;
    }
    if (fDistance < 1.0f)
    {
        fResult -= fAcceleration * 5.0f;
    }
    if (!(fResult > fMinimum))
    {
        fResult = fMinimum;
    }
    if (!(fResult < fMaximum))
    {
        return fMaximum;
    }
    return fResult;
}
// PC 0x0068DE20
void CSWCameraOnAStick::ResetLastPosition(Vector vPosition,
                                          Quaternion qOrientation)
{
    m_vDesiredPosition = vPosition;
    m_vLastPosition = vPosition;
    m_qLastOrientation = qOrientation;
    static_cast<Gob*>(m_pObject)->SetPosition(vPosition);
    static_cast<Gob*>(m_pObject)->SetOrientation(qOrientation);
}
// PC 0x0068DEC0
int CSWCameraOnAStick::CameraFreeStyleControl(float fTimeStep)
{

    if (CameraFreeStyleMode == CAMERA_FREE_STYLE_TRANSLATE)
    {
        if (m_fTurn != 0.0f)
        {
            static_cast<Gob*>(m_pObject)->SetPosition(
                fTimeStep * Vector(1.0f, 0.0f, 0.0f) * m_fTurn +
                static_cast<Gob*>(m_pObject)->GetPosition());
        }
        if (m_fTilt < 0.0f)
        {
            static_cast<Gob*>(m_pObject)->SetPosition(
                fTimeStep * Vector(0.0f, -1.0f, 0.0f) +
                static_cast<Gob*>(m_pObject)->GetPosition());
        }
        if (m_fTilt > 0.0f)
        {
            static_cast<Gob*>(m_pObject)->SetPosition(
                fTimeStep * Vector(0.0f, 1.0f, 0.0f) +
                static_cast<Gob*>(m_pObject)->GetPosition());
        }
    }
    if (CameraFreeStyleMode == CAMERA_FREE_STYLE_ROTATE)
    {
        static float TPitch = 0.0f;
        static float TYaw = 0.0f;
        if (m_fTurn != 0.0f)
        {
            TYaw += m_fTurn;
        }
        if (m_fTilt < 0.0f)
        {
            TPitch += 0.1f;
        }
        if (m_fTilt > 0.0f)
        {
            TPitch -= 0.1f;
        }
        static_cast<Gob*>(m_pObject)->SetOrientation(YawPitchRoll(TYaw, TPitch, 0.0f));
    }
    if (CameraFreeStyleMode == CAMERA_FREE_STYLE_RAISE)
    {
        if (m_fTilt < 0.0f)
        {
            static_cast<Gob*>(m_pObject)->SetPosition(
                fTimeStep * Vector(0.0f, 0.0f, 1.0f) +
                static_cast<Gob*>(m_pObject)->GetPosition());
        }
        if (m_fTilt > 0.0f)
        {
            static_cast<Gob*>(m_pObject)->SetPosition(
                fTimeStep * Vector(0.0f, 0.0f, -1.0f) +
                static_cast<Gob*>(m_pObject)->GetPosition());
        }
    }
    m_fTurn = 0.0f;
    m_fTilt = 0.0f;
    return 0;
}
// PC 0x0068E270
void CSWCameraOnAStick::SetCameraStyle(int nStyle)
{
    if (m_nStyle != nStyle)
    {
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        EXOXBOXASSERT(p2DArrays->m_pCameraStyle);
        C2DA* p2DA = p2DArrays->m_pCameraStyle;
        if (p2DA == NULL)
        {
            m_nStyle = -1;
            m_fDistance = 6.25f;
            m_fSpeed = 20.0f;
            m_fPresetPitch = 80.0f;
            m_fHeight = 3.5f;
        }
        else if (nStyle <= p2DA->m_nNumRows)
        {
            m_nStyle = nStyle;
            p2DA->GetFLOATEntry(m_nStyle, "DISTANCE", &m_fDistance);
            p2DA->GetFLOATEntry(m_nStyle, "SPEED", &m_fSpeed);
            p2DA->GetFLOATEntry(m_nStyle, "PITCH", &m_fPresetPitch);
            p2DA->GetFLOATEntry(m_nStyle, "TILTUP", &CameraLookUp);
            p2DA->GetFLOATEntry(m_nStyle, "TILTDOWN", &CameraLookDown);
            p2DA->GetFLOATEntry(m_nStyle, "HEIGHT", &m_fHeight);
            p2DA->GetFLOATEntry(m_nStyle, "TILTSPEED", &m_fTiltSpeed);
            p2DA->GetFLOATEntry(m_nStyle, "ROTATION", &CameraRotate);
            float fViewAngle;
            p2DA->GetFLOATEntry(m_nStyle, "VIEWANGLE", &fViewAngle);
#ifndef _XBOX
            m_integrator.m_fMaximumVelocity =
                g_pAppManager->m_pClientExoApp->GetClientOptions()
                    ->GetCameraKeyboardDPS();
            m_integrator.m_fMaximumAcceleration =
                g_pAppManager->m_pClientExoApp->GetClientOptions()
                    ->GetCameraKeyboardAcceleration();
            m_integrator.m_fMaximumDeceleration =
                g_pAppManager->m_pClientExoApp->GetClientOptions()
                    ->GetCameraKeyboardDeceleration();
#endif
            if (fViewAngle == 0.0f)
            {
                fViewAngle = 55.0f;
            }
            m_pCamera->AsCAurCamera()->SetViewAngle(fViewAngle);
            m_fPreviousDistance = m_fDistance;
            m_fViewAngle = fViewAngle;
        }
    }
}

void CSWCameraOnAStick::UpdateCameraStyle()
{
    SetCameraStyle(g_pAppManager->m_pClientExoApp->GetModule()
                       ->m_pInfo->m_nTurnRateRow);
}
// PC 0x0068E5B0
void CSWCameraOnAStick::ClearLookAtTarget()
{
    m_oidLookAt = OBJECT_INVALID;
}
// PC 0x0068E5C0
void CSWCameraFreeLook::UpdateCamraStyle()
{
    CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
    EXOXBOXASSERT(p2DArrays->m_pCameraStyle);
    C2DA* p2DA = p2DArrays->m_pCameraStyle;
    if (p2DA == NULL)
    {
        m_fRotateSpeed = 60.0f;
        m_fTiltSpeed = 60.0f;
    }
    CSWCArea* pArea = g_pAppManager->m_pClientExoApp->GetModule()->m_pInfo;
    int nRow = pArea->m_nTurnRateRow;
#ifdef _XBOX
    p2DA->GetFLOATEntry(nRow, "FL_TiltSpeed", &m_fRotateSpeed);
    p2DA->GetFLOATEntry(nRow, "FL_RotateSpeed", &m_fTiltSpeed);
#else
    p2DA->GetFLOATEntry(nRow, "FL_PC_TiltSpeed", &m_fTiltSpeed);
    p2DA->GetFLOATEntry(nRow, "FL_PC_RotateSpeed", &m_fRotateSpeed);
#endif
    p2DA->GetFLOATEntry(nRow, "FL_LOOKUP", &m_fLookUp);
    p2DA->GetFLOATEntry(nRow, "FL_LOOKDOWN", &m_fLookDown);
#ifndef _XBOX
    if (m_fLookUp > m_fLookDown)
    {
        m_integrator.m_fMaximumVelocity = m_fLookUp;
    }
    else
    {
        m_integrator.m_fMaximumVelocity = m_fLookDown;
    }
#endif
}
// PC 0x005EE1C0
void CSWCameraFreeLook::Message(char* sMessage)
{
}
// PC 0x0068E740
void CSWCameraDeath::Message(char* sMessage)
{
    if (stricmp("m_yaw", firstword(sMessage)) == 0)
    {
        Parse(sMessage + 5, m_fYaw);
    }
    if (stricmp("m_speedYaw", firstword(sMessage)) == 0)
    {
        Parse(sMessage + 10, m_fSpeedYaw);
    }
    if (stricmp("m_desiredPitch", firstword(sMessage)) == 0)
    {
        Parse(sMessage + 14, m_fDesiredPitch);
    }
    if (stricmp("m_desiredDistance", firstword(sMessage)) == 0)
    {
        Parse(sMessage + 17, m_fDesiredDistance);
    }
    if (stricmp("m_logYaw", firstword(sMessage)) == 0)
    {
        Parse(sMessage + 8, m_fLogYaw);
    }
    if (stricmp("m_logPitch", firstword(sMessage)) == 0)
    {
        Parse(sMessage + 10, m_fLogPitch);
    }
    if (stricmp("m_logDistance", firstword(sMessage)) == 0)
    {
        Parse(sMessage + 13, m_fLogDistance);
    }
}
// PC 0x0068E870
CSWCameraNavigate::CSWCameraNavigate(CAurObject* pObject)
    : CAurBehavior(pObject)
{
    m_fTurn = 0.0f;
    m_fTilt = 0.0f;
    m_fSpeed = 0.0f;
    m_fRaise = 0.0f;
    m_fStrafe = 0.0f;
    m_fRoll = 0.0f;
    m_fLastRoll = 0.0f;
}
// PC 0x0068E8E0
void CSWCameraNavigate::Message(char* sMessage)
{
    if (stricmp("turn", firstword(sMessage)) == 0)
    {
        Parse(sMessage + 4, m_fTurn);
    }
    if (stricmp("tilt", firstword(sMessage)) == 0)
    {
        Parse(sMessage + 4, m_fTilt);
    }
    if (stricmp("speed", firstword(sMessage)) == 0)
    {
        Parse(sMessage + 5, m_fSpeed);
    }
    if (stricmp("raise", firstword(sMessage)) == 0)
    {
        Parse(sMessage + 5, m_fRaise);
    }
    if (stricmp("strafe", firstword(sMessage)) == 0)
    {
        Parse(sMessage + 6, m_fStrafe);
    }
    if (stricmp("roll", firstword(sMessage)) == 0)
    {
        Parse(sMessage + 4, m_fRoll);
    }
}
// PC 0x0068E9E0
int CSWCameraNavigate::Control(float fTimeStep)
{
    int nResult = CAurBehavior::Control(fTimeStep);
    if (!nResult)
    {
        return 0;
    }

    if (CAMERA_NAVIGATE_HS_ENABLED && g_nHSMode)
    {
        m_fSpeed *= CAMERA_NAVIGATE_HS_SPEED_SCALE;
        m_fStrafe *= CAMERA_NAVIGATE_HS_STRAFE_SCALE;
        m_fTilt *= CAMERA_NAVIGATE_HS_TILT_SCALE;
        m_fTurn *= CAMERA_NAVIGATE_HS_TURN_SCALE;

        Quaternion qOrientation = static_cast<Gob*>(m_pObject)->GetOrientation();
        qOrientation = YawPitchRoll(fTimeStep * m_fTurn, 0.0f, 0.0f) * qOrientation;
        m_fTurn = 0.0f;
        qOrientation = qOrientation * YawPitchRoll(0.0f, -90.0f, 0.0f);
        m_fLastRoll = m_fRoll =
            (CAMERA_NAVIGATE_ROLL_HISTORY_WEIGHT - 2.0f * fTimeStep) * m_fLastRoll +
            (2.0f * fTimeStep + CAMERA_NAVIGATE_ROLL_INPUT_WEIGHT) * m_fRoll;
        qOrientation = qOrientation *
            YawPitchRoll(0.0f, 0.0f, m_fRoll - Roll(qOrientation));
        float fTilt = fTimeStep * m_fTilt;
        float fPitch = Pitch(qOrientation) - fTilt;
        if (fPitch > -90.0f && fPitch < 90.0f)
        {
            qOrientation = qOrientation * YawPitchRoll(0.0f, -fTilt, 0.0f);
        }
        m_fTilt = 0.0f;
        Quaternion qMovement = qOrientation;
        qOrientation = qOrientation * YawPitchRoll(0.0f, 90.0f, 0.0f);
        qOrientation = normalize(qOrientation);
        static_cast<Gob*>(m_pObject)->SetOrientation(qOrientation);

        Vector vMovement;
        vMovement.m_fX = m_fStrafe;
        vMovement.m_fY = -m_fSpeed;
        vMovement.m_fZ = 0.0f;
        m_fStrafe = 0.0f;
        m_fSpeed = 0.0f;
        if (vMovement.m_fX != 0.0f || vMovement.m_fY != 0.0f)
        {
            vMovement = (qMovement * vMovement) * fTimeStep;
            static_cast<Gob*>(m_pObject)->SetPosition(
                vMovement + static_cast<Gob*>(m_pObject)->GetPosition());
        }
        if (RenderQA && m_pObject)
        {
            SW_CAMERA_QA_REPORT("Camera position (%f, %f, %f)",
                static_cast<Gob*>(m_pObject)->GetPosition().m_fX,
                static_cast<Gob*>(m_pObject)->GetPosition().m_fY,
                static_cast<Gob*>(m_pObject)->GetPosition().m_fZ);
        }
        return 1;
    }

    float fYaw = Yaw(static_cast<Gob*>(m_pObject)->GetOrientation());
    float fPitch = Pitch(static_cast<Gob*>(m_pObject)->GetOrientation());
    float fDirection = 1.0f;
    float fDot = static_cast<Gob*>(m_pObject)->GetOrientation().zdir() *
        Vector(0.0f, 0.0f, 1.0f);
    if (fDot >= 0.0f)
    {
        fDirection = -1.0f;
    }
    fYaw += fTimeStep * m_fTurn;
    m_fTurn = 0.0f;
    fPitch = max(0.0f, fPitch + fDirection * m_fTilt * fTimeStep);
    m_fTilt = 0.0f;
    Quaternion qMovement = YawPitchRoll(
        fYaw, fPitch - 90.0f, Roll(static_cast<Gob*>(m_pObject)->GetOrientation()));
    Vector vMovement;
    vMovement.m_fX = m_fStrafe;
    vMovement.m_fY = fDirection * m_fSpeed;
    vMovement.m_fZ = 0.0f;
    if (vMovement.m_fX != 0.0f || vMovement.m_fY != 0.0f)
    {
        vMovement = (qMovement * vMovement) * fTimeStep;
        static_cast<Gob*>(m_pObject)->SetPosition(
            vMovement + static_cast<Gob*>(m_pObject)->GetPosition());
    }
    static_cast<Gob*>(m_pObject)->SetOrientation(YawPitchRoll(
        fYaw, fPitch, Roll(static_cast<Gob*>(m_pObject)->GetOrientation())));
    if (RenderQA && m_pObject)
    {
        SW_CAMERA_QA_REPORT("Camera position (%f, %f, %f)",
            static_cast<Gob*>(m_pObject)->GetPosition().m_fX,
            static_cast<Gob*>(m_pObject)->GetPosition().m_fY,
            static_cast<Gob*>(m_pObject)->GetPosition().m_fZ);
    }
    return 1;
}
// PC 0x0068F530
void CSWCameraOnAStick::SetCameraForScreenShot()
{
    Gob* pTarget = static_cast<Gob*>(m_pTarget.m_pObject);
    Vector vDirection =
        pTarget->GetPosition() - static_cast<Gob*>(m_pObject)->GetPosition();
    Pitch(vDirection);
    vDirection.m_fZ = 0.0f;
    if (vDirection.m_fY * vDirection.m_fY + vDirection.m_fX * vDirection.m_fX >
        0.0f)
    {
        static_cast<Gob*>(m_pObject)->SetOrientation(
            YawPitchRoll(Yaw(normalize(vDirection)), m_fPresetPitch, 0.0f));
    }
}
// PC 0x0068F610
void CSWCameraOnAStick::Control_CheckAndUpdateCameraStyle()
{
#ifndef _XBOX
    m_integrator.m_fMaximumVelocity =
        g_pAppManager->m_pClientExoApp->GetClientOptions()
            ->GetCameraKeyboardDPS();
    m_integrator.m_fMaximumAcceleration =
        g_pAppManager->m_pClientExoApp->GetClientOptions()
            ->GetCameraKeyboardAcceleration();
    m_integrator.m_fMaximumDeceleration =
        g_pAppManager->m_pClientExoApp->GetClientOptions()
            ->GetCameraKeyboardDeceleration();
#endif
    if (CameraStyle != -1)
    {
        SetCameraStyle(CameraStyle);
        CameraStyle = -1;
    }
}
// PC 0x0068F690
void CSWCameraOnAStick::Control_SetCameraInintalState()
{
    Quaternion qTarget =
        static_cast<Gob*>(m_pTarget.m_pObject)->GetOrientation();
    m_qLastOrientation = YawPitchRoll(
        Yaw(normalize(qTarget.zdir())) + m_fShakeYaw,
        m_fPresetPitch + m_fShakeYaw, 0.0f);
    static_cast<Gob*>(m_pObject)->SetOrientation(m_qLastOrientation);

    Vector vPosition =
        static_cast<Gob*>(m_pTarget.m_pObject)->GetPosition();
    vPosition.m_fZ += 1.5f;
    vPosition -= qTarget.ydir();
    static_cast<Gob*>(m_pObject)->SetPosition(vPosition);
    m_vDesiredPosition = m_vInitialPosition = m_vLastPosition = vPosition;
}

#ifndef _XBOX
Quaternion CSWCameraOnAStick::Control_UpdateCameraDesiredOrientation(  // PC 0x0068F850
    Quaternion qOrientation, float fTimeStep)
{
    Vector vCameraPos;
    Vector vSide;
    float fMouseSenSetting;
    CSWCObject* pTarget;

    this->m_fOrientationInterpAmount = 0.0f;
    this->m_bPreserveLastYaw = 1;
    vCameraPos = static_cast<Gob*>(m_pTarget.m_pObject)->GetPosition();
    fMouseSenSetting = g_pAppManager->m_pClientExoApp->GetClientOptions()->GetMouseSenSetting();
    Quaternion qResult = qOrientation;
    if (this->m_fTurn == 0.0f && this->m_fTurnAcceleration == 0.0f)
    {
      if (this->m_oidLookAt != OBJECT_INVALID)
      {
        pTarget = g_pAppManager->m_pClientExoApp->GetGameObject(this->m_oidLookAt);
        if (pTarget != NULL)
        {
            Vector vDirection = pTarget->m_vPosition -
                static_cast<Gob*>(m_pTarget.m_pObject)->GetPosition();
            vSide = cross(zaxis, qResult.ydir());
            float fYawOffset = CAMERA_LOOK_AT_YAW_OFFSET;
            float fDot = vSide * vDirection;
            if (fDot < 0.0)
            {
                fYawOffset = -CAMERA_LOOK_AT_YAW_OFFSET;
            }
            if (fabs(fDot) < CAMERA_LOOK_AT_DOT_CUTOFF)
            {
                m_oidLookAt = OBJECT_INVALID;
            }
            qResult = YawPitchRoll(Yaw(vDirection) - fYawOffset, 0.0f, 0.0f);
            m_fOrientationInterpAmount = CameraInterpAmt2;
            m_bPreserveLastYaw = 0;
            m_nReservedAC = 1;
        }
      }
    }
    else
    {
        m_oidLookAt = OBJECT_INVALID;
    }

    g_pAppManager->m_pClientExoApp->GetSWGuiManager();
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
    int bTurnNegative = bToggle;
    int bTurnPositive = bToggle;
    if (bToggle == 0 ||
        g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pPartySelection != 0)
    {
        g_pAppManager->m_pClientExoApp->ShowMouse(MOUSE_HIDE_CAMERA_ROTATE);
    }
    else
    {
        g_pAppManager->m_pClientExoApp->HideMouse(MOUSE_HIDE_CAMERA_ROTATE);
    }

    float fTurn = pow(1.0f, fBorderScaleExponent) * m_fTurn;
    if (0.0f == m_fTurn && bToggle == 0)
    {
        Vector vTurn(0.0f, m_fTurnAcceleration, 0.0f);
        fTurn = m_integrator.Update(vTurn, fTimeStep, 1, 0, 0, 0).m_vRate.m_fY;
        bTurnPositive = fTurn > 0.0f;
        bTurnNegative = fTurn < 0.0f;
    }
    else
    {
        Vector vTurn(0.0f, fTurn / fMouseSenSetting, 0.0f);
        m_integrator.Update(vTurn, fTimeStep, 1, 0, 0, 0);
        m_fTurnAcceleration = 0.0f;
    }
    if (bTurnNegative)
    {
        if (bCameraSlideBack && bToggle == 0)
        {
            qResult = YawPitchRoll(-fTurn, 0.0f, 0.0f) * qResult;
        }
        else if (CameraHorzAxisFlip)
        {
            qResult = YawPitchRoll(-max(0.0f, fTurn), 0.0f, 0.0f) * qResult;
        }
        else
        {
            qResult = YawPitchRoll(-min(0.0f, fTurn), 0.0f, 0.0f) * qResult;
        }
    }
    if (bTurnPositive)
    {
        if (bCameraSlideBack && bToggle == 0)
        {
            qResult = YawPitchRoll(-fTurn, 0.0f, 0.0f) * qResult;
        }
        else if (CameraHorzAxisFlip)
        {
            qResult = YawPitchRoll(-min(0.0f, fTurn), 0.0f, 0.0f) * qResult;
        }
        else
        {
            qResult = YawPitchRoll(-max(0.0f, fTurn), 0.0f, 0.0f) * qResult;
        }
    }
    m_fTurn = 0.0f;
    m_vInitialPosition = static_cast<Gob*>(m_pTarget.m_pObject)->GetPosition();
    if (DesiredOrientation >= 0.0f)
    {
        qResult = YawPitchRoll(DesiredOrientation, 0.0f, 0.0f);
        DesiredOrientation = -1.0f;
    }
    return qResult;
}
#endif
// PC 0x00690220
Quaternion CSWCameraOnAStick::Control_SetCameraOrientation(float fTimeStep)
{
    Gob* pTarget = static_cast<Gob*>(m_pTarget.m_pObject);
    Vector vDirection =
        pTarget->GetPosition() - static_cast<Gob*>(m_pObject)->GetPosition();
    vDirection.m_fZ = 0.0f;
    float fPitch = Pitch(static_cast<Gob*>(m_pObject)->GetOrientation());
    m_fLookUp = CameraLookUp;
    m_fLookDown = CameraLookDown;
    m_fTiltRate = 100.0f;
    int bToggle = g_bMouseCameraRotateToggle;
    if (g_pAppManager->m_pClientExoApp->GetClientOptions()->m_bMouseLook)
    {
        bToggle = (bToggle == 0);
    }
    if (m_fTilt != 0.0f && bToggle)
    {
        m_fDesiredPitch += m_fTilt * (fTimeStep * m_fTiltRate);
        m_integratorTilt.m_NewState.m_vRate = Vector();
        m_integratorTilt.m_NewState.m_vValue = Vector();
        m_integratorTilt.m_OldState.m_vRate = Vector();
        m_integratorTilt.m_OldState.m_vValue = Vector();
    }
    else
    {
        m_fDesiredPitch += m_integratorTilt
                               .Update(Vector(0.0f, m_fTiltAcceleration, 0.0f),
                                       fTimeStep, 1, 0, 0, 0)
                               .m_vRate.m_fY;
        m_fTiltAcceleration = 0;
    }
    m_fDesiredPitch =
        (m_fDesiredPitch < -m_fLookUp) ? -m_fLookUp : m_fDesiredPitch;
    m_fDesiredPitch =
        (m_fDesiredPitch > m_fLookDown) ? m_fLookDown : m_fDesiredPitch;
    return YawPitchRoll(Yaw(normalize(vDirection)) + m_fShakeYaw,
                        m_fDesiredPitch + m_fShakePitch + 90.0f, 0.0f);
}
// PC 0x00690480
Vector CSWCameraOnAStick::Control_ComputeNewCameraPosition(
    Vector vDesiredPosition, Vector vPosition, Vector vTargetPosition,
    float fTimeStep)
{
    Vector vOffset(vPosition.m_fX - vTargetPosition.m_fX,
                   vPosition.m_fY - vTargetPosition.m_fY, 0.0f);
    Quaternion qCurrentOrientation(
        zaxis, Yaw(Vector(-vOffset.m_fX, -vOffset.m_fY, 0.0f)) * DEG_TO_RAD);
    float fCurrentYaw = Yaw(qCurrentOrientation);
    if (fCurrentYaw < 0.0f)
    {
        fCurrentYaw += 360.0f;
    }
    float fLastYaw = Yaw(m_qLastOrientation);
    if (fLastYaw < 0.0f)
    {
        fLastYaw += 360.0f;
    }

    float fDeltaYaw;
    if (fCurrentYaw >= 270.0f && fLastYaw <= 90.0f)
    {
        fDeltaYaw = 360.0f - fCurrentYaw + fLastYaw;
    }
    else if (fCurrentYaw <= 90.0f && fLastYaw >= 270.0f)
    {
        fDeltaYaw = -(360.0f - fLastYaw + fCurrentYaw);
    }
    else
    {
        fDeltaYaw = fLastYaw - fCurrentYaw;
    }

    if ((fDeltaYaw > 180.0f - CAMERA_YAW_HALF_TURN_TOLERANCE &&
         fDeltaYaw < 180.0f + CAMERA_YAW_HALF_TURN_TOLERANCE) ||
        (fDeltaYaw < -180.0f + CAMERA_YAW_HALF_TURN_TOLERANCE &&
         fDeltaYaw > -180.0f - CAMERA_YAW_HALF_TURN_TOLERANCE))
    {
        fDeltaYaw = 180.0f;
    }
    else if (fDeltaYaw > 180.0f)
    {
        fDeltaYaw -= 360.0f;
    }
    else if (fDeltaYaw < -180.0f)
    {
        fDeltaYaw += 360.0f;
    }
    fDeltaYaw = fDeltaYaw * m_fPositionInterpSpeed * fTimeStep;

    if (m_bResetToTargetOrientation)
    {
        m_bResetToTargetOrientation = 0;
        m_qLastOrientation =
            static_cast<Gob*>(m_pTarget.m_pObject)->GetOrientation();
        fDeltaYaw = Yaw(m_qLastOrientation);
    }
    if (fDeltaYaw > 360.0f)
    {
        fDeltaYaw -= 360.0f;
    }
    if (fDeltaYaw < 0.0f)
    {
        fDeltaYaw += 360.0f;
    }
#ifdef _XBOX
    fDeltaYaw += fCurrentYaw;
#else
    if (m_bPreserveLastYaw)
    {
        m_bPreserveLastYaw = 0;
        fDeltaYaw = fLastYaw;
    }
    else
    {
        fDeltaYaw += fCurrentYaw;
    }
#endif

    Quaternion qNewOrientation(zaxis, fDeltaYaw * DEG_TO_RAD);
    float fDistance = magnitude(vOffset) - m_fDistance;
    fDistance = fDistance - CameraInterpDistAmt * fDistance + m_fDistance;
    vDesiredPosition = fDistance * Vector(0.0f, 1.0f, 0.0f);
    vDesiredPosition = qNewOrientation * vDesiredPosition;
    vPosition = vTargetPosition - vDesiredPosition;
    vDesiredPosition = vPosition;
    vDesiredPosition.m_fZ = vTargetPosition.m_fZ + m_fHeight;
    return vDesiredPosition;
}
// PC 0x006908B0
void CSWCameraOnAStick::SetLookAtTarget(CSWCObject* pTarget)
{
    if (m_pTarget != NULL && pTarget != NULL)
    {
        m_oidLookAt = pTarget->m_nId;
        m_vLookAtOffset =
            normalize(static_cast<Gob*>((CAurObject*)m_pTarget)->GetPosition() -
                      pTarget->m_vPosition) *
            m_fDistance;
        m_vLookAtOffset +=
            static_cast<Gob*>((CAurObject*)m_pTarget)->GetPosition();
        m_vLookAtOffset.m_fZ = m_fHeight + m_fPitch + m_vLookAtOffset.m_fZ;
    }
}
// PC 0x00690990
int CSWCameraFreeLook::Control(float fTimeStep)
{
    if (CAurBehavior::Control(fTimeStep))
    {
#ifndef _XBOX
        g_pAppManager->m_pClientExoApp->HideMouse(MOUSE_HIDE_CAMERA_ROTATE);
#endif
        Vector vPosition;
        Quaternion qOrientation;
        if (!static_cast<Gob*>(m_pTarget.m_pObject)
                 ->GetPartPosition("FreeLookHook", &vPosition, &qOrientation) &&
            !static_cast<Gob*>(m_pTarget.m_pObject)
                 ->GetPartPosition("CameraHook", &vPosition, &qOrientation))
        {
            static_cast<Gob*>(m_pTarget.m_pObject)
                ->GetPartPosition("root", &vPosition, &qOrientation);
            vPosition.m_fZ += 2.0f;
        }
        static_cast<Gob*>(m_pObject)->SetPosition(vPosition);
#ifdef _XBOX
        if (m_fTiltInput != 0.0f)
        {
            m_fPitch += m_fTiltSpeed * fTimeStep * m_fTiltInput;
        }
#else
        if (m_fTiltInput != 0.0f)
        {
            m_fPitch += m_fTiltInput * (fTimeStep * m_fTiltSpeed);
            m_integrator.m_NewState.m_vRate = Vector();
            m_integrator.m_NewState.m_vValue = Vector();
            m_integrator.m_OldState.m_vRate = Vector();
            m_integrator.m_OldState.m_vValue = Vector();
        }
        else
        {
            m_fPitch += m_integrator
                            .Update(Vector(0.0f, m_fTiltAcceleration, 0.0f),
                                    fTimeStep, 1, 0, 0, 0)
                            .m_vRate.m_fY;
            m_fTiltAcceleration = 0;
        }
#endif
        m_fPitch += (m_fPitch < -180.0f) ? 360 : 0;
        m_fPitch -= (m_fPitch > 180.0f) ? 360 : 0;
        m_fPitch = (m_fPitch < -m_fLookDown) ? -m_fLookDown : m_fPitch;
        m_fPitch = (m_fPitch > m_fLookUp) ? m_fLookUp : m_fPitch;
        static_cast<Gob*>(m_pObject)->SetOrientation(
            YawPitchRoll(Yaw(qOrientation) + m_fYawOffset,
                         m_fPitch + 90.0f + m_fPitchOffset, 0.0f));
    }
    return 1;
}
// PC 0x00690C50
CSWCameraOnAStick::CSWCameraOnAStick(CAurObject* pObject, CAurObject* pTarget)
    : CSWBehaviorCamera(pObject)
{
    m_pCamera = static_cast<Gob*>(pObject);
    AddDependency(pTarget);
    m_pTarget = pTarget;
    m_pCombatTarget = NULL;
    m_nStyle = CAMERA_STYLE_UNINITIALIZED;
    SetCameraStyle(g_pAppManager->m_pClientExoApp->GetModule()
                       ->m_pInfo->m_nTurnRateRow);
    m_vPlayerOrientation = Vector(0.0f, 0.0f, 0.0f);
    m_nOrientationState = 0;
    m_fMaximumDistance = 100.0f;
    m_fTilt = 0.0f;
    m_fDesiredPitch = m_fPresetPitch;
    m_fTurn = 0.0f;
#ifndef _XBOX
    m_fTurnAcceleration = 0.0f;
    m_fTiltAcceleration = 0;
#endif

    Quaternion qTarget =
        static_cast<Gob*>(m_pTarget.m_pObject)->GetOrientation();
    m_qLastOrientation = YawPitchRoll(
        Yaw(normalize(qTarget.zdir())), m_fPresetPitch, 0.0f);
    static_cast<Gob*>(m_pObject)->SetOrientation(m_qLastOrientation);

    Vector vPosition = static_cast<Gob*>(m_pTarget.m_pObject)->GetPosition();
    vPosition.m_fZ += CAMERA_TARGET_HEIGHT_OFFSET;
    vPosition -= qTarget.ydir();
    static_cast<Gob*>(m_pObject)->SetPosition(vPosition);
    m_vDesiredPosition = m_vInitialPosition = m_vLastPosition = vPosition;

    m_bResetToTargetOrientation = 1;
#ifndef _XBOX
    m_bPreserveLastYaw = 0;
#endif
    m_vInitialPosition.m_fX = FLT_MAX;
    m_bKeepFramingOnViewAngle = 0;
    m_fShakeYaw = 0.0f;
    m_fShakePitch = 0.0f;
    m_oidLookAt = OBJECT_INVALID;
    m_nReservedA4 = 0;
    m_fReserved98 = -1.0f;
    m_nReservedAC = 0;
#ifndef _XBOX
    m_bMouseCameraRotateToggle = g_bMouseCameraRotateToggle;
#endif
}
// PC 0x00690FD0
CSWCameraOnAStick::~CSWCameraOnAStick()
{
}
// PC 0x00691010
void CSWCameraOnAStick::ClearCombatCamera()
{
    m_pCombatTarget = NULL;
}
// PC 0x00691030
void CSWCameraOnAStick::ChangeTarget(CAurObject* pTarget, int bSnap)
{
    m_fTurn = 0.0f;
    if (pTarget != m_pTarget.m_pObject)
    {
        RemoveDependency(m_pTarget.m_pObject);
        AddDependency(pTarget);
    }
    m_pTarget = pTarget;

    Quaternion qTarget;
    Quaternion qCamera;
    Vector vDirection;

    qTarget = static_cast<Gob*>(m_pTarget.m_pObject)->GetOrientation();
    qCamera = static_cast<Gob*>(m_pObject)->GetOrientation();
    static_cast<Gob*>(m_pObject)->SetOrientation(
        YawPitchRoll(Yaw(qTarget), Pitch(qCamera), Roll(qCamera)));

    vDirection = -qTarget.ydir();
    vDirection.m_fZ = 0.0f;
    vDirection.Normalize();

    Vector vPosition =
        vDirection * m_fDistance +
        static_cast<Gob*>(m_pTarget.m_pObject)->GetPosition();
    vPosition.m_fZ = m_fHeight;
    m_vLastPosition = m_vInitialPosition = vPosition;
}
// PC 0x00691A20
CSWCameraOnAStick::CSWCameraOnAStick(CCameraSettings& settings)
    : CSWBehaviorCamera(settings.m_pCamera)
{
    m_pTarget = settings.m_pTarget;
    m_pCamera = settings.m_pCamera;
    m_nHeightMode = settings.m_nHeightMode;
    m_fPitch = settings.m_fPitch;
    m_nStyle = CAMERA_STYLE_UNINITIALIZED;
    SetCameraStyle(settings.m_nStyle);
#ifndef _XBOX
    m_integratorTilt.m_fMaximumVelocity = 200.0f;
    m_integratorTilt.m_fMaximumAcceleration = 500.0f;
    m_fTiltAcceleration = 0;
#endif
    AddDependency(m_pTarget.m_pObject);

    m_nOrientationState = 0;
    m_vPlayerOrientation = Vector();
#ifdef _XBOX
    m_fDesiredPitch = m_fPresetPitch;
#endif
    m_fMaximumDistance = 100.0f;
    m_fTilt = 0.0f;
#ifndef _XBOX
    m_fDesiredPitch = settings.m_fDesiredPitch;
#endif
    m_fTurn = 0.0f;
#ifndef _XBOX
    m_fTurnAcceleration = 0.0f;
#endif

    Quaternion qTarget =
        static_cast<Gob*>(m_pTarget.m_pObject)->GetOrientation();
    m_qLastOrientation =
        YawPitchRoll(Yaw(normalize(qTarget.zdir())), m_fPresetPitch, 0.0f);
    static_cast<Gob*>(m_pObject)->SetOrientation(qTarget);

    Vector vPosition =
        static_cast<Gob*>(m_pTarget.m_pObject)->GetPosition();
    vPosition.m_fZ += CAMERA_TARGET_HEIGHT_OFFSET;
    vPosition -= qTarget.ydir();
    m_vDesiredPosition = m_vInitialPosition = m_vLastPosition = vPosition;

    m_bResetToTargetOrientation = 1;
#ifndef _XBOX
    m_bPreserveLastYaw = 1;
#endif
    m_fShakeYaw = 0.0f;
    m_fShakePitch = 0.0f;
    m_nReservedA4 = 0;
    m_nReservedAC = 0;
    m_oidLookAt = OBJECT_INVALID;
    m_fReserved98 = -1.0f;
#ifndef _XBOX
    m_bMouseCameraRotateToggle = g_bMouseCameraRotateToggle;
#endif
}
// PC 0x00692770
void CSWCameraOnAStick::SaveCameraSettings(CCameraSettings& settings)
{
    settings.m_bValid = 1;
    settings.m_nCameraType = CCameraSettings::CAMERA_ON_STICK;
    settings.m_pCamera = m_pCamera;
    settings.m_pTarget = m_pTarget;
    settings.m_nHeightMode = m_nHeightMode;
    settings.m_nStyle = m_nStyle;
    settings.m_fPitch = m_fPitch;
    settings.m_nReserved1C = -1;
#ifndef _XBOX
    settings.m_fDesiredPitch = m_fDesiredPitch;
#endif
}
// PC 0x00691280
CSWCameraFreeLook::CSWCameraFreeLook(CAurObject* pObject, CAurObject* pTarget,
                                     int nMode)
    : CSWBehaviorCamera(pObject)
{
    m_pCamera = pObject;
    AddDependency(pTarget);
    m_pTarget = pTarget;
    m_fTiltInput = 0.0f;
#ifndef _XBOX
    m_fTiltAcceleration = 0.0f;
#endif
    m_fPitch = 0.0f;
    m_fYawOffset = 0.0f;
    m_fPitchOffset = 0.0f;
    UpdateCamraStyle();
    m_nMode = nMode;
    g_pAppManager->m_pClientExoApp->EnableVideoEffect(nMode);
#ifndef _XBOX
    m_integrator.m_fMaximumVelocity = 200.0f;
    m_integrator.m_fMaximumAcceleration = 500.0f;
#endif
}
// PC 0x006927E0
CSWCameraFreeLook::CSWCameraFreeLook(CCameraSettings& settings)
    : CSWBehaviorCamera(settings.m_pCamera)
{
    m_pTarget = settings.m_pTarget;
    m_pCamera = settings.m_pCamera;
    m_fPitch = 0.0f;
    m_fTiltInput = 0.0f;
#ifndef _XBOX
    m_fTiltAcceleration = 0.0f;
#endif
    UpdateCamraStyle();
    m_nMode = settings.m_nReserved1C;
    g_pAppManager->m_pClientExoApp->EnableVideoEffect(m_nMode);
#ifndef _XBOX
    m_integrator.m_fMaximumVelocity = 200.0f;
    m_integrator.m_fMaximumAcceleration = 500.0f;
#endif
}
// PC 0x006928C0
CSWCameraDeath::CSWCameraDeath(CAurObject* pObject, CAurObject* pTarget)
    : CSWBehaviorCamera(pObject)
{
    m_fYaw = 0.0f;
    m_fLogYaw = 0.5f;
    m_fSpeedYaw = 30.0f;
    m_fLogPitch = 0.01f;
    m_fDesiredPitch = 90.0f;
    m_fLogDistance = 0.5f;
    m_fDesiredDistance = 3.0f;
    m_fReserved34 = 0.75f;
    m_fReserved38 = 0.0f;
    m_fReserved3C = 0.01f;
    m_pTarget = NULL;
    SetTarget(pTarget);
}
// PC 0x00691370
CSWCameraFreeLook::~CSWCameraFreeLook()
{
    g_pAppManager->m_pClientExoApp->DisableVideoEffect();
}
// PC 0x006913E0
CSWCameraDeath::~CSWCameraDeath()
{
}
// PC 0x00691420
void CSWCameraDeath::SetTarget(CAurObject* pTarget)
{
    if (m_pTarget != NULL)
    {
        RemoveDependency(m_pTarget);
    }
    if (pTarget != NULL)
    {
        AddDependency(pTarget);
        Vector vDirection =
            normalize(static_cast<Gob*>(pTarget)->GetPosition() -
                      static_cast<Gob*>(m_pObject)->GetPosition());
        m_fYaw = Yaw(vDirection);
        m_fReserved34 = 0.75f;
    }
    m_pTarget = pTarget;
}
// PC 0x006914C0
int CSWCameraDeath::Control(float fTimeStep)
{
    SafePointer<CAurObject> pObject(m_pObject);
    if (!CAurBehavior::Control(fTimeStep))
    {
        return 0;
    }

    EXOXBOXASSERT((CAurObject*)pObject);
    if (static_cast<Gob*>((CAurObject*)pObject)->GetBehavior(-1) != this)
    {
        return 1;
    }

    if (m_pTarget != NULL)
    {
        m_fReserved34 = (m_fReserved38 - m_fReserved34) * m_fReserved3C
                        + m_fReserved34;
        m_fYaw += fTimeStep * m_fSpeedYaw;
        if (m_fYaw < 0.0f)
        {
            m_fYaw += 360.0f;
        }
        else if (m_fYaw >= 360.0f)
        {
            m_fYaw -= 360.0f;
        }

        float fYaw = Yaw(static_cast<Gob*>(m_pObject)->GetOrientation());
        while (fYaw + 360.0f < m_fYaw)
        {
            fYaw += 360.0f;
        }
        float fDesiredPitch = 90.0f - m_fDesiredPitch;
        if (fDesiredPitch < 0.0f)
        {
            fDesiredPitch += 360.0f;
        }
        float fPitch = Pitch(static_cast<Gob*>(m_pObject)->GetOrientation());
        while (fPitch < fDesiredPitch)
        {
            fPitch += 360.0f;
        }
        float fNewYaw = (m_fYaw - fYaw) * m_fLogYaw + fYaw;
        float fNewPitch = (fDesiredPitch - fPitch) * m_fLogPitch + fPitch;
        Quaternion qOrientation = YawPitchRoll(fNewYaw, fNewPitch, 0.0f);

        float fDistance = magnitude(
            static_cast<Gob*>((CAurObject*)m_pTarget)->GetPosition() -
            static_cast<Gob*>(m_pObject)->GetPosition());
        fDistance = (m_fDesiredDistance - fDistance) * m_fLogDistance
                    + fDistance;
        Vector vTarget = static_cast<Gob*>((CAurObject*)m_pTarget)->GetPosition()
                         + Vector(0.0f, 0.0f, m_fReserved34);
        Vector vPosition = fDistance * qOrientation.zdir() + vTarget;
        Vector vHit;
        CHitInfo hitInfo(0, CHitInfo::HIT_ALL_NON_TRIGGER, 0);
        hitInfo.m_bResult0 = 1;
        hitInfo.m_lstHits.Add(m_pTarget);
        hitInfo.ClearOutput();
        Vector vOffset = normalize(vPosition - vTarget) * CAMERA_COLLISION_CLEARANCE;
        hitInfo.m_bResult2 = 1;
        int bHit = static_cast<Gob*>(m_pObject)->GetScene()->HitCheck(
            vTarget, vPosition + vOffset, &vHit, &hitInfo) != 0;
        if (bHit)
        {
            vPosition = CAMERA_COLLISION_CLEARANCE * hitInfo.m_vOutputNormal + vHit;
        }
        static_cast<Gob*>(m_pObject)->SetPosition(vPosition);
        static_cast<Gob*>(m_pObject)->SetOrientation(qOrientation);
    }
    return 1;
}

// PC 0x006929A0
int CSWCameraOnAStick::Control(float fTimeStep)
{
    if (m_bKeepFramingOnViewAngle)
    {
        float fFraming = tan(m_fViewAngle * 0.0087266462f) * m_fPreviousDistance;
        m_fDistance = fFraming /
            (float)tan(m_pCamera->AsCAurCamera()->GetViewAngle() * 0.0087266462f);
    }
    Control_CheckAndUpdateCameraStyle();
    if (CAurBehavior::Control(fTimeStep))
    {
        if (CameraFreeStyle)
        {
            return CameraFreeStyleControl(fTimeStep);
        }
        if (m_pTarget.m_pObject == NULL)
        {
            static_cast<Gob*>(m_pObject)->SetBehavior(NULL);
            return 1;
        }
        if (m_vInitialPosition.m_fX >= FLT_MAX)
        {
            Control_SetCameraInintalState();
        }
        m_qLastOrientation = Control_UpdateCameraDesiredOrientation(
            m_qLastOrientation, fTimeStep);
        Vector vPosition = static_cast<Gob*>(m_pObject)->GetPosition();
        Vector vTargetPosition = GetTargetHeadPosition();
        Vector vDesiredPosition = Control_ComputeDesiredPosition(vTargetPosition);
        m_fPositionInterpSpeed = Control_AccelerateInterp(
            m_fPositionInterpSpeed, CameraInterpAmt1, m_fOrientationInterpAmount,
            static_cast<Gob*>(m_pObject)->GetPosition(), vDesiredPosition, fTimeStep);
        vDesiredPosition = Control_HitCheckCamera(Control_ComputeNewCameraPosition(
            vDesiredPosition, vPosition, vTargetPosition, fTimeStep));
#ifndef _XBOX
        if (RenderQA && m_pObject)
        {
            SW_CAMERA_QA_REPORT("Camera position (%f, %f, %f), target distance %f",
                static_cast<Gob*>(m_pObject)->GetPosition().m_fX,
                static_cast<Gob*>(m_pObject)->GetPosition().m_fY,
                static_cast<Gob*>(m_pObject)->GetPosition().m_fZ,
                magnitude(static_cast<Gob*>(m_pObject)->GetPosition() - vTargetPosition));
        }
#endif
        static_cast<Gob*>(m_pObject)->SetPosition(vDesiredPosition);
        static_cast<Gob*>(m_pObject)->SetOrientation(Control_SetCameraOrientation(fTimeStep));
        if (m_bResetToTargetOrientation)
        {
            m_bResetToTargetOrientation = 0;
        }
    }
    return 1;
}

#undef SW_CAMERA_QA_REPORT

static inline void SwbehaviorcameraTrace5()
{
    SwbehaviorcameraTrace4();
}

static inline void SwbehaviorcameraTrace4()
{
    SwbehaviorcameraTrace3();
}

static inline void SwbehaviorcameraTrace3()
{
    SwbehaviorcameraTrace2();
}

static inline void SwbehaviorcameraTrace2()
{
    SwbehaviorcameraTrace1();
}

static inline void SwbehaviorcameraTrace1()
{
}
// PC 0x00691D30
Vector CSWCameraOnAStick::Control_HitCheckCamera(Vector vPosition)
{
    SwbehaviorcameraTrace5();
    Vector vSafe = vPosition;
    Vector vHitRight;
    Vector vHitLeft;
    Vector vHitUp;
    Vector vHitDown;
    CHitInfo hitInfo(0, CHitInfo::HIT_ALL_NON_TRIGGER, 0);
    int nChecks = 1;
    hitInfo.m_bResult0 = 1;
    int bContinue = 1;
    hitInfo.m_lstHits.Add(m_pTarget);
    while (bContinue && nChecks-- > 0)
    {
        hitInfo.ClearOutput();
        Vector vStart = GetTargetHeadPosition();
        Vector vShrunk;
        if (g_pAppManager->GetServerExoApp()->ShrinkLineByCreatures(
                vStart, vPosition, vShrunk))
        {
            vPosition = vShrunk;
            vSafe = vShrunk;
        }
        Vector vDelta = vPosition - vStart;
        Vector vDirection = normalize(cross(zaxis, vStart - vPosition));
        Vector vOffset = CameraPersonalSpace * vDirection;

        Vector vProbeRight = vPosition + vOffset;
        hitInfo.ClearOutput();
        hitInfo.m_bResult2 = 1;
        CAurScene* pSceneRight = static_cast<Gob*>(m_pObject)->GetScene();
        int bHitRight = pSceneRight->HitCheck(
            vStart, vProbeRight, &vHitRight, &hitInfo) != 0;

        Vector vProbeLeft = vPosition - vOffset;
        hitInfo.ClearOutput();
        hitInfo.m_bResult2 = 1;
        CAurScene* pSceneLeft = static_cast<Gob*>(m_pObject)->GetScene();
        int bHitLeft = pSceneLeft->HitCheck(
            vStart, vProbeLeft, &vHitLeft, &hitInfo) != 0;

        Vector vUp(0.0f, 0.0f, 1.0f);
        Vector vVertical = vUp * CameraPersonalSpace;
        Vector vProbeUp = vPosition + vVertical;
        hitInfo.ClearOutput();
        hitInfo.m_bResult2 = 1;
        CAurScene* pSceneUp = static_cast<Gob*>(m_pObject)->GetScene();
        int bHitUp = pSceneUp->HitCheck(
            vStart, vProbeUp, &vHitUp, &hitInfo) != 0;

        Vector vProbeDown = vPosition - vVertical;
        hitInfo.ClearOutput();
        hitInfo.m_bResult2 = 1;
        CAurScene* pSceneDown = static_cast<Gob*>(m_pObject)->GetScene();
        int bHitDown = pSceneDown->HitCheck(
            vStart, vProbeDown, &vHitDown, &hitInfo) != 0;

        bContinue = bHitRight || bHitLeft || bHitUp || bHitDown;
        if (bContinue)
        {
            float fTargetDistance = magnitude(vDelta);
            float fDistance = 0.0f;
            if (bHitRight)
            {
                fDistance = max(fDistance, magnitude(vHitRight - vProbeRight));
            }
            if (bHitLeft)
            {
                fDistance = max(fDistance, magnitude(vHitLeft - vProbeLeft));
            }
            if (bHitUp)
            {
                fDistance = max(fDistance, magnitude(vHitUp - vProbeUp));
            }
            if (bHitDown)
            {
                fDistance = max(fDistance, magnitude(vHitDown - vProbeDown));
            }
            Vector vCorrection = fDistance * normalize(vDelta);
            vSafe -= vCorrection;
            Gob* pTarget = static_cast<Gob*>(m_pTarget.m_pObject);
            vCorrection =
                normalize(pTarget->GetOrientation().zdir()) *
                ((fDistance / fTargetDistance) *
                 (CameraPersonalSpace + CAMERA_COLLISION_LIFT_CLEARANCE));
            vSafe += vCorrection;
        }
    }
    return vSafe;
}

#ifdef _XBOX

#include "swplayercontrol.h"
Quaternion CSWCameraOnAStick::Control_UpdateCameraDesiredOrientation(
    Quaternion qOrientation, float fTimeStep)
{
    Vector vTargetMove =
        static_cast<Gob*>(m_pTarget.m_pObject)->GetPosition() - m_vInitialPosition;
    Quaternion qResult = qOrientation;
    if (fabs(m_fTurn) > 1.0f)
    {
        m_fTurn = m_fTurn * 0.013333334f;
    }
    if (m_fTurn != 0.0f)
    {
        float fRotate = CameraRotate;
        if (vTargetMove.m_fX != 0.0f || vTargetMove.m_fY != 0.0f ||
            vTargetMove.m_fZ != 0.0f)
        {
            fRotate *= 0.5f;
        }
        qResult = YawPitchRoll(fRotate * fTimeStep * m_fTurn, 0.0f, 0.0f) * qResult;
    }

    int bLookAt = 0;
    if (m_fTurn == 0.0f)
    {
        if (m_oidLookAt != OBJECT_INVALID)
        {
            CSWCObject* pTarget =
                g_pAppManager->m_pClientExoApp->GetGameObject(m_oidLookAt);
            if (pTarget != NULL)
            {
                Vector vDirection = pTarget->m_vPosition -
                    static_cast<Gob*>(m_pTarget.m_pObject)->GetPosition();
                Vector vSide = cross(zaxis, qResult.ydir());
                float fDot = vSide * vDirection;
                float fYawOffset = CAMERA_LOOK_AT_YAW_OFFSET;
                if (fDot < 0.0)
                {
                    fYawOffset = -CAMERA_LOOK_AT_YAW_OFFSET;
                }
                if (fabs(fDot) < CAMERA_LOOK_AT_DOT_CUTOFF)
                {
                    m_oidLookAt = OBJECT_INVALID;
                }
                qResult = YawPitchRoll(Yaw(vDirection) - fYawOffset, 0.0f, 0.0f);
                bLookAt = 1;
                m_nReservedAC = 1;
            }
        }
    }
    else
    {
        m_oidLookAt = OBJECT_INVALID;
    }

    Vector vCameraOffset =
        static_cast<Gob*>(m_pObject)->GetPosition() - m_vInitialPosition;
    static_cast<Gob*>(m_pTarget.m_pObject)->GetOrientation();
    int bTowardCamera;
    if ((vTargetMove.m_fX != 0.0f || vTargetMove.m_fY != 0.0f ||
         vTargetMove.m_fZ != 0.0f) &&
        vCameraOffset * vTargetMove >= 0.0f)
    {
        bTowardCamera = 1;
    }
    else
    {
        bTowardCamera = 0;
    }
    m_bTargetTowardCamera = bTowardCamera;

    m_fReserved98 -= fTimeStep;
    if (vTargetMove.m_fX == 0.0f && vTargetMove.m_fY == 0.0f &&
        vTargetMove.m_fZ == 0.0f)
    {
        m_fReserved98 = fDelayAmount;
    }
    if (m_fReserved98 < 0.0f)
    {
        m_fReserved98 = -1.0f;
    }

    float fUpDown = g_pAppManager->m_pClientExoApp->m_pcExoAppInternal
                        ->m_pPlayerControl->GetUpDown();
    float fLeftRight = g_pAppManager->m_pClientExoApp->m_pcExoAppInternal
                           ->m_pPlayerControl->GetLeftRight();
    int bStickCentered;
    if (fUpDown >= -0.01f && fUpDown <= 0.01f && fLeftRight >= -0.01f &&
        fLeftRight <= 0.01f)
    {
        bStickCentered = 1;
        if (m_fTurn != 0.0f)
        {
            m_nReservedAC = 1;
        }
    }
    else
    {
        bStickCentered = 0;
        m_nReservedAC = 0;
        if (OldCamera)
        {
            m_fPositionInterpSpeed = 0.0f;
        }
    }

    int bSnapToFacing = 0;
    if (!OldCamera)
    {
        if (m_fTurn == 0.0f && !bStickCentered && !bLookAt)
        {
            Vector vToTarget =
                static_cast<Gob*>(m_pTarget.m_pObject)->GetPosition() -
                static_cast<Gob*>(m_pObject)->GetPosition();
            vToTarget.m_fZ = 0.0f;
            qResult = YawPitchRoll(Yaw(vToTarget), 0.0f, 0.0f);
        }
        if (m_fTurn == 0.0f && bStickCentered && !bLookAt && m_nReservedAC == 0)
        {
            bSnapToFacing = 1;
        }
    }
    else if (m_fTurn == 0.0f && !bStickCentered && m_fReserved98 == -1.0f)
    {
        bSnapToFacing = 1;
    }
    if (bSnapToFacing &&
        g_pAppManager->m_pClientExoApp->GetClientOptions()->m_bMouseLook)
    {
        qResult = static_cast<Gob*>(m_pTarget.m_pObject)->GetOrientation();
    }

    m_vInitialPosition = static_cast<Gob*>(m_pTarget.m_pObject)->GetPosition();
    float fInterp;
    if (m_fTurn == 0.0f && !bStickCentered)
    {
        if (bLookAt)
        {
            fInterp = CameraInterpAmt2;
        }
        else
        {
            fInterp = CameraInterpAmt1;
            m_nReservedA4 = 1;
        }
    }
    else if (!bLookAt && m_fTurn == 0.0f && m_nReservedAC != 1)
    {
        fInterp = CameraInterpAmt3;
    }
    else
    {
        fInterp = CameraInterpAmt2;
    }
    m_fOrientationInterpAmount = fInterp;
    if (m_fTurn == 0.0f && m_nReservedA4 && OldCamera)
    {
        m_fOrientationInterpAmount = CameraInterpAmt3;
    }
    else
    {
        m_nReservedA4 = 0;
    }
    m_fTurn = 0.0f;
    if (DesiredOrientation >= 0.0f)
    {
        qResult = YawPitchRoll(DesiredOrientation, 0.0f, 0.0f);
        DesiredOrientation = -1.0f;
    }
    return qResult;
}
#endif
#ifdef _XBOX

#include "aurconsole.h"
#include "nwccreature.h"
char* DeathCamera(char* szArgs)
{
    if (g_pAppManager->m_pClientExoApp != NULL)
    {
        CSWCModule* pModule = g_pAppManager->m_pClientExoApp->GetModule();
        CSWCCreature* pPlayer = g_pAppManager->m_pClientExoApp->GetPlayerCreature();
        if (pModule != NULL && pPlayer != NULL)
        {
            CAurCamera* pCamera = pModule->m_pModuleCamera;
            if (pCamera != NULL)
            {
                pCamera->SetBehavior(new CSWCameraDeath(pCamera->AsCAurObject(),
                                                        pPlayer->GetCameraFollowPart()));
            }
        }
    }
    return "Success";
}
ConsoleFunc ConFDeathCamera("DeathCamera", DeathCamera);
#endif
