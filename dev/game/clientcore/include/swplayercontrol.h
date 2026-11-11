#ifndef __SWPLAYERCONTROL_H__
#define __SWPLAYERCONTROL_H__

#define SWPLAYERCONTROL_TURN_EPSILON 0.1f
#define SWPLAYERCONTROL_STOP_SPEED 0.25
#define SWPLAYERCONTROL_FAST_SPEED 5000.0f
#define SWPLAYERCONTROL_FAST_DISPLACEMENT_SQUARED 0.001089f

#include "gameobjectarray.h"
#include <math.h>
#include "twodimarrays.h"
class CSWFeat;
class CSWRules;
class CTwoDimArrays;
class C2DA;
class CSWClass;
class CSWBaseItemArray;

class CSW2DAArrays
{
public:
    char m_aReserved[0x74];
    C2DA* m_pAreaTurnRates;
    C2DA* m_pGenericDoorTypes;
};

extern CSWRules* g_pRules;

#include "twodimarray.h"
#include "exobase.h"
#include "math3d.h"
#include "SWAccelerationControlers.h"
#include "appmanager.h"
#include "clientexoapp.h"

class CAurCamera;

class CSWMGPhysicsState
{
public:
    CSWMGPhysicsState(const Vector& vValue = Vector(),  // PC 0x00670750  Xbox 0x00093510
                      const Vector& vRate = Vector())
    {
        m_vValue = vValue;
        m_vRate = vRate;
    }

    Vector m_vValue;
    Vector m_vRate;
};

#include "clientoptions.h"

class CSWPlayerControl
{
public:
    static const int SWDRIVE_FORWARD;
    static const int SWDRIVE_BACKWARD;
    static const int SWDRIVE_STRAFELEFT;
    static const int SWDRIVE_STRAFERIGHT;
    static const int SWDRIVE_TURN;
    static const int SWDRIVE_UP;
    static const int SWDRIVE_DOWN;
    CSWPlayerControl();
    CSWPlayerControl(unsigned long oidPlayer, CAurCamera* pCamera);
    // PC 0x0060E760
    virtual void UpDown(float fValue)
    {
    }
    // PC 0x006796F0
    virtual float GetUpDown()
    {
        return 0.0f;
    }
    // PC 0x0060E760
    virtual void LeftRight(float fValue)
    {
    }
    // PC 0x006796F0
    virtual float GetLeftRight()
    {
        return 0.0f;
    }

#ifndef _XBOX
    virtual void SetPlayerWalking(int bWalking)  // PC 0x0060E760
    {
    }
    // PC 0x0063E7F0
    virtual int GetPlayerWalking()
    {
        return 0;
    }
#endif
    // PC 0x004015A0
    virtual void RunKey()
    {
    }
    // PC 0x006796F0
    virtual float GetCurSpeed()
    {
        return 0.0f;
    }
    // PC 0x0060E760
    virtual void MouseLeftRight(float fValue)
    {
    }
    // PC 0x0060E760
    virtual void MouseUpDown(float fValue)
    {
    }
    // PC 0x0063E7C0
    virtual int Control(float fTimeStep)
    {
        return 0;
    }
    // PC 0x004015A0
    virtual void ResetDriveAcceleration()
    {
    }

    CSWCCreature* GetPlayer()
    {
        return g_pAppManager->GetClientExoApp()->GetCreatureByGameObjectID(
            m_oidPlayer);
    }

    int Initialize();
    void SetEnabled(int bEnabled);
    void SetPlayer(unsigned long oidPlayer);
    int SetCamera(CAurCamera* pCamera);

    unsigned long m_oidPlayer;
    CAurCamera* m_pCamera;
    int m_bEnabled;
};

class CSWPlayerControlCamRelative : public CSWPlayerControl
{
public:
    CSWPlayerControlCamRelative();
    CSWPlayerControlCamRelative(unsigned long oidPlayer, CAurCamera* pCamera);
    // PC 0x00679700
    virtual void UpDown(float fValue)
    {
        m_fUpDown = fValue;
    }
    virtual float GetUpDown()  // PC 0x00679710  Xbox 0x00093C40
    {
        return m_fUpDown;
    }
    virtual void LeftRight(float fValue)  // PC 0x00679720  Xbox 0x00093C50
    {
        m_fLeftRight = fValue;
    }
    virtual float GetLeftRight()  // PC 0x00679730  Xbox 0x00093C60
    {
        return m_fLeftRight;
    }
#ifndef _XBOX
    virtual void SetPlayerWalking(int bWalking)  // PC 0x00679740
    {
        m_bPlayerWalking = bWalking;
    }
    virtual int GetPlayerWalking()  // PC 0x00644760
    {
        return m_bPlayerWalking;
    }
#endif
    virtual float GetCurSpeed()  // PC 0x00679750  Xbox 0x00093C70
    {
        float fForward = (float)fabs(m_stateB.m_vValue.m_fX);
        float fSideways = (float)fabs(m_stateB.m_vValue.m_fY);
        if (fForward > fSideways)
        {
            return fForward;
        }
        return fSideways;
    }
    virtual int Control(float fTimeStep);
    virtual void ResetDriveAcceleration();
    float GetAcceleration();

    void Init();
    float GetMaxSpeed();
    Vector GetAcceleration(CSWMGPhysicsState& state, const Vector& vTarget);

    float m_fUpDown;
    float m_fLeftRight;
#ifndef _XBOX
    int m_bPlayerWalking;
#endif
    Vector m_vFacing;
    Vector m_vReserved28;
    float m_fReserved34;
    float m_fMinTurnRate;
    float m_fMaxTurnRate;
    float m_fTurnDirection;
    CSWMGPhysicsState m_stateA;
    CSWMGPhysicsState m_stateB;
    Vector m_vTarget;
    CSWRK4SplitAcceleration m_integrator;
};

#endif
