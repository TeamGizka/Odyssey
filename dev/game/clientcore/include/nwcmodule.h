#ifndef __NWCMODULE_H__
#define __NWCMODULE_H__

class CAurCamera;
class CResRef;
class CSWCArea;
class CSWMiniGame;
class Gob;
#include "exobase.h"
#include "exoresman.h"
#include "math3d.h"
#include "nwsobject.h"
#include "swbehaviorcamera.h"

#define CAMERA_COLLISION_RAY_EPSILON 0.001f
#define CAMERA_COLLISION_DISTANCE_TOLERANCE 0.0001f

class CSWCModule : public CGameObject
{
public:

    enum CameraType
    {
        CAMERA_UNINITIALIZED = -1,
        CAMERA_ON_A_STICK = 3, CAMERA_FREE_LOOK = 5, CAMERA_NAVIGATE = 7
    };

    enum TimeOfDayState
    {
        TIME_DEFAULT = 0, TIME_DAY = 1, TIME_NIGHT = 2,
        TIME_DAWN = 3, TIME_DUSK = 4
    };

    enum CameraTurnPreference
    {
        CAMERA_TURN_PREFER_POSITIVE = 1, CAMERA_TURN_PREFER_NEGATIVE = 2
    };

    CSWCModule();
    virtual ~CSWCModule();
    virtual CSWCModule* AsSWCModule()  // PC 0x00698BD0
    {
        return this;
    }

    void UnloadModule();
    int LoadModule();
    CExoString GetModuleTitle();
    void SetCameraFollow(unsigned long oidTarget, int bSnap);
    void TiltCamera(float fTilt);
    void AcclTiltCamera(float fTilt);
    void TurnCamera(float fTurn, float fTimeStep);
    void AcclTurnCamera(float fTurn, float fTimeStep);
    void ZoomCamera(float fZoom);
    void ScrollCamera(Vector vScroll);
    int GetNextCameraPosition(float fTimeStep, Vector* pPosition);
    void SetTimeOfDayState(unsigned char nState, unsigned long nTime);
    void UpdateCameraModeChaseCam();
    void UpdateCameraModeBG();
    void UpdateCameraModeEQ(float fTimeStep);
    void LoadAnimatedCamera(const CResRef& cAnimatedCamera);
    float SetAnimatedCamera(unsigned short nAnimation, float fFieldOfView,
                            unsigned long oidTarget);
    int IsCameraAnimated();
    void SetDialogCamera(int nDialogType);
    int SetFreeLookCamera(unsigned long oidTarget);
    int SetCombatCamera();
    int SetNavigateCamera();
    int ShakeCamera(unsigned long nDelay, unsigned long nDuration);
    int BumpCamera(unsigned long nDelay);
    void UpdateCameraBumpAndShake(float fTimeDelta);
    CSWMiniGame* GetMiniGame();
    void UpdateCameraFollow(float fTimeDelta);
    int RestoreCamera();

    CAurCamera* GetCamera() { return m_pModuleCamera; }
    CSWCArea* GetArea() { return m_pInfo; }
    unsigned char GetDawnHour() { return m_nDawnHour; }
    unsigned char GetDuskHour() { return m_nDuskHour; }
    float UpdateNormalCameraCollision(float fTimeDelta, float fUnknown);
    float UpdatePushCameraCollision(float fTimeDelta, float fUnknown);
    float UpdateCameraCollision(float fTimeDelta, float fUnknown);
    int FindClearView(float fTimeDelta, float* pfYaw, float* pfPitch);
    void Update(float fTimeDelta);

    int m_nCameraType;
    CExoString m_sModuleTitle;
    CResRef m_resHak;
    CResRef m_resReserved28;
    CExoString m_sModuleName;
    CAurCamera* m_pModuleCamera;
    int m_nReserved44;
    CSWCArea* m_pInfo;
    Vector m_vPlayerPosition;
    Vector m_vCameraPosition;
    Vector m_vCameraOffset;
    unsigned long m_oidCameraTarget;
    float m_fCameraDistance;
    float m_fCameraYaw;
    float m_fCameraPitch;
    int m_nReserved80;
    int m_nReserved84;
    int m_nReserved88;
    int m_bCameraCollisionChanged;
    char m_nReserved90;
    float m_fCameraTilt;
    float m_fCameraTurn;
    float m_fCameraZoom;
    unsigned char m_nTimeOfDayHour;
    int m_nTimeA4;
    int m_nTimeA8;
    int m_nTimeAC;
    int m_nTimeB0;
    unsigned char m_nTimeOfDayState;
    float m_fTimeOfDay;
    float m_fTimeOfDayHours;

    unsigned char m_nDawnHour;
    unsigned char m_nDuskHour;
    unsigned char m_aUnusedC2[2];
    int m_nReservedC4;
    int m_nReservedC8;
    int m_nReservedCC;
    int m_nReservedD0;
    char m_nReservedD4;
    unsigned char m_nLastCameraMode;
    int m_bCameraModeChanged;
    int m_nCameraShakeTime;
    int m_nCameraShakeAmount;
    int m_nCameraShakeDelay;
    int m_nCameraShakeLength;
    int m_bCameraShakeChanged;
    unsigned long m_nCameraBumpTime;
    unsigned long m_nCameraBumpDelay;
    int m_bCameraBumpChanged;
    int m_nReservedFC;
    int m_nReserved100;
    int m_nReserved104;
    int m_nReserved108;
    CCameraSettings m_cameraSettings;
    int m_n12C;
    Gob* m_pAnimatedCamera;
    CExoString m_sAnimatedCamera;
#ifdef _XBOX

    void SetCameraOffset(Vector vOffset) { m_vCameraOffset = vOffset; }
#endif
};

#endif
