#ifndef __SWBEHAVIORCAMERA_H__
#define __SWBEHAVIORCAMERA_H__

#include "nwcanimbase.h"
#include <aurpointer.h>
#include "SWAccelerationControlers.h"
class CSWCObject;

char* firstword(char* sMessage);
int Parse(char* sValue, float& fValue);
void Parse(char* sValue, char* sDest);

const int AUR_BEHAVIOR_CAMERA = 4200;
const int AUR_BEHAVIOR_CAMERA_NAVIGATE = 4209;
const int AUR_BEHAVIOR_CAMERA_ONASTICK = 4202;
const int AUR_BEHAVIOR_CAMERA_DIALOG = 4205;
const int AUR_BEHAVIOR_CAMERA_FREELOOK = 4206;
const int AUR_BEHAVIOR_CAMERA_DEATH = 4207;
const int AUR_BEHAVIOR_CAMERA_COMBAT = 4208;

extern int CameraStyle;
extern float CameraLookUp;
extern float CameraLookDown;
extern float CameraRotate;
extern float CameraInterpDistAmt;
extern float CameraInterpAmt2;
extern float CameraInterpAmt1;
extern int CameraFreeStyle;
extern float DesiredOrientation;
extern int CameraHorzAxisFlip;
extern int CameraVertAxisFlip;
extern float ScreenFramePercentage;
extern int bCameraSlideBack;
extern float fBorderScaleExponent;

#define CAMERA_YAW_HALF_TURN_TOLERANCE 0.0001f

#define CAMERA_TARGET_HEIGHT_OFFSET 1.5f

#define CAMERA_COLLISION_CLEARANCE 0.25f

#define CAMERA_COLLISION_LIFT_CLEARANCE 0.15f

#define CAMERA_LOOK_AT_YAW_OFFSET 15.0f
#define CAMERA_LOOK_AT_DOT_CUTOFF 0.866f

#define CAMERA_NAVIGATE_HS_SPEED_SCALE 5.0f
#define CAMERA_NAVIGATE_HS_STRAFE_SCALE 3.0f
#define CAMERA_NAVIGATE_HS_TILT_SCALE 3.33f
#define CAMERA_NAVIGATE_HS_TURN_SCALE 2.67f
#define CAMERA_NAVIGATE_ROLL_HISTORY_WEIGHT 0.967f
#define CAMERA_NAVIGATE_ROLL_INPUT_WEIGHT 0.033f

#define CAMERA_NAVIGATE_HS_ENABLED 1

extern int CameraFreeStyleMode;

enum CameraFreeStyleModes
{
    CAMERA_FREE_STYLE_TRANSLATE = 0,
    CAMERA_FREE_STYLE_ROTATE = 1,
    CAMERA_FREE_STYLE_RAISE = 2
};

class CCameraSettings
{
public:
    static const int CAMERA_ON_STICK;
    static const int CAMERA_FREE_LOOK;

    enum SavedCameraType { SAVED_ON_A_STICK = 0, SAVED_FREE_LOOK = 1 };

    int m_bValid;
    int m_nCameraType;
    Gob* m_pCamera;
    SafePointer<CAurObject> m_pTarget;
    int m_nStyle;
    int m_nHeightMode;
    float m_fPitch;
    int m_nReserved1C;
#ifndef _XBOX
    float m_fDesiredPitch;
#endif
};

class CSWCameraNavigate : public CAurBehavior
{
public:
    CSWCameraNavigate(CAurObject* pObject);
    virtual ~CSWCameraNavigate()  // PC 0x0068E8D0
    {
    }
    virtual int Control(float fTimeStep);
    virtual void Message(char* sMessage);
    virtual int GetType()  // PC 0x0068E8A0
    {
        return AUR_BEHAVIOR_CAMERA_NAVIGATE;
    }

    float m_fTurn;
    float m_fTilt;
    float m_fSpeed;
    float m_fRaise;
    float m_fStrafe;
    float m_fRoll;

    float m_fLastRoll;
};

class CSWBehaviorCamera : public CAurBehavior
{
public:
    CSWBehaviorCamera(CAurObject* pObject) : CAurBehavior(pObject)
    {
    }
    virtual int Control(float fTimeStep)
    {
        return CAurBehavior::Control(fTimeStep);
    }
    virtual int GetType()
    {
        return AUR_BEHAVIOR_CAMERA;
    }
};

class CSWCameraOnAStick : public CSWBehaviorCamera
{
public:

    enum { CAMERA_STYLE_UNINITIALIZED = -1 };
    CSWCameraOnAStick(CAurObject* pObject, CAurObject* pTarget);
    CSWCameraOnAStick(CCameraSettings& settings);
    virtual ~CSWCameraOnAStick();
    virtual int Control(float fTimeStep);
    virtual void Message(char* sMessage);
    virtual int GetType()  // PC 0x00690FC0
    {
        return AUR_BEHAVIOR_CAMERA_ONASTICK;
    }

    void ClearLookAtTarget();
    Vector GetTargetHeadPosition()  // PC 0x00640B20
    {
        Vector vPosition = static_cast<Gob*>((CAurObject*)m_pTarget)->GetPosition();
        vPosition.m_fZ += m_fPitch;
        return vPosition;
    }
    void SaveCameraSettings(CCameraSettings& settings);
    void ClearCombatCamera();
    Vector GetPosition();
    void SetPosition(Vector vPosition);
    void ChangeTarget(CAurObject* pTarget, int bSnap);
    void SetLookAtTarget(CSWCObject* pTarget);
    void ResetLastPosition(Vector vPosition, Quaternion qOrientation);
    Vector Control_ComputeDesiredPosition(Vector vPosition);

    float Control_AccelerateInterp(float fStart, float fMinimum, float fMaximum,
                                   Vector vCurrent, Vector vDesired,
                                   float fAcceleration);
    int CameraFreeStyleControl(float fTimeStep);
    void SetCameraStyle(int nStyle);
    void SetCameraForScreenShot();
    void UpdateCameraStyle();
    void Control_CheckAndUpdateCameraStyle();
    void Control_SetCameraInintalState();
    Quaternion Control_SetCameraOrientation(float fTimeStep);
    Quaternion Control_UpdateCameraDesiredOrientation(Quaternion qOrientation,
                                                       float fTimeStep);
    Vector Control_ComputeNewCameraPosition(Vector vDesiredPosition,
                                            Vector vPosition,
                                            Vector vTargetPosition,
                                            float fTimeStep);
    Vector Control_HitCheckCamera(Vector vPosition);
    SafePointer<CAurObject> m_pTarget;
    SafePointer<CAurObject> m_pCombatTarget;
    Gob* m_pCamera;
    char m_aReserved20[8];
    Quaternion m_qLastOrientation;
    Vector m_vDesiredPosition;

    Vector m_vInitialPosition;
    Vector m_vLastPosition;
    int m_nStyle;

    int m_bResetToTargetOrientation;
#ifndef _XBOX
    int m_bPreserveLastYaw;
#endif
    int m_nHeightMode;
    float m_fPitch;
    char m_aReserved70[4];
    Vector m_vLookAtOffset;
    unsigned long m_oidLookAt;

    int m_bKeepFramingOnViewAngle;
    float m_fPreviousDistance;
    float m_fViewAngle;
    float m_fShakeYaw;
    float m_fShakePitch;

    float m_fReserved98;

    float m_fPositionInterpSpeed;

    float m_fOrientationInterpAmount;
    int m_nReservedA4;
#ifdef _XBOX
    int m_bTargetTowardCamera;
#else
    char m_aReservedA8[4];
#endif
    int m_nReservedAC;
#ifndef _XBOX
    int m_bMouseCameraRotateToggle;
    CSWRK4SplitAcceleration m_integrator;
    float m_fTurn;
    float m_fTilt;
    float m_fDesiredPitch;
    float m_fTurnAcceleration;
    CSWRK4Acceleration m_integrator110;
    int m_n158;
    int m_n15C;
    float m_f160;
    float m_f164;
    float m_f168;
#endif
    float m_fDistance;
    float m_fSpeed;
    float m_fMaximumDistance;
    float m_fPresetPitch;
    float m_fHeight;

    int m_nOrientationState;
    Vector m_vPlayerOrientation;
    float m_fTiltSpeed;
    float m_fDefaultTiltSpeed;
#ifdef _XBOX

    float m_fTurn;
    float m_fTilt;
    float m_fDesiredPitch;

#endif
};

class CSWCameraFreeLook : public CSWBehaviorCamera
{
public:
    CSWCameraFreeLook(CAurObject* pObject, CAurObject* pTarget, int nMode);
    virtual ~CSWCameraFreeLook();
    CSWCameraFreeLook(CCameraSettings& settings);
    virtual int Control(float fTimeStep);
    virtual void Message(char* sMessage);
    virtual int GetType()  // PC 0x00691360
    {
        return AUR_BEHAVIOR_CAMERA_FREELOOK;
    }
    void UpdateCamraStyle();

    SafePointer<CAurObject> m_pTarget;
    CAurObject* m_pCamera;
    char m_aReserved1C[4];

    float m_fPitch;
#ifndef _XBOX
    float m_fTiltAcceleration;
#endif
    float m_fRotateSpeed;
    float m_fTiltSpeed;
    float m_fLookUp;
    float m_fLookDown;
    float m_fTiltInput;
    Quaternion m_qOrientation;
    Vector m_vReserved4C;
    Vector m_vReserved58;
    Vector m_vReserved64;
    float m_fYawOffset;
    float m_fPitchOffset;
    int m_nMode;
#ifndef _XBOX
    CSWRK4Acceleration m_integrator;
#endif
};

class CSWCameraDeath : public CSWBehaviorCamera
{
public:
    CSWCameraDeath(CAurObject* pObject, CAurObject* pTarget);
    virtual ~CSWCameraDeath();
    virtual int Control(float fTimeStep);
    virtual void Message(char* sMessage);
    virtual int GetType()  // PC 0x00691410
    {
        return AUR_BEHAVIOR_CAMERA_DEATH;
    }

    void SetTarget(CAurObject* pTarget);

    SafePointer<CAurObject> m_pTarget;
    float m_fYaw;
    float m_fSpeedYaw;
    float m_fDesiredPitch;
    float m_fDesiredDistance;
    float m_fLogYaw;
    float m_fLogPitch;
    float m_fLogDistance;
    float m_fReserved34;
    float m_fReserved38;
    float m_fReserved3C;
};

#endif
