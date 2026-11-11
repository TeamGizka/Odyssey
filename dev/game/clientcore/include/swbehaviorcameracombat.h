#ifndef __SWBEHAVIORCAMERACOMBAT_H__
#define __SWBEHAVIORCAMERACOMBAT_H__

#include "aurbehavior.h"
#include "swplayercontrol.h"

class CAurObject;
class CSWCCreature;
class CSWCObject;
class Vector;

class CSWBehaviorCameraCombat : public CAurBehavior
{
public:
    CSWBehaviorCameraCombat(CAurObject* pObject);
    virtual int Control(float fTimeStep);
    virtual int GetType()  // PC 0x0073F9B0
    {
        return 4208;
    }

    Vector CalculateDesiredLocation(CSWCCreature* pCreature,
                                    CSWCObject* pTarget, float fTimeStep,
                                    float& fHeight);
    float GetNextYaw(const Vector& vDirection, float fYaw, float fTimeStep,
                     float& fTurn);
    Vector GetNextPosition(const Vector& vTarget, float fTimeStep,
                           float fSpeed);
    int SetTurn(float fTurn);
    Vector DetermineSafePosition(CSWCCreature* pCreature,
                                 const Vector& vDesired,
                                 const Vector& vCurrent);
    void PutAtDesiredLocation();

    float m_fYawOffset;
    float m_fPitchOffset;

    Vector m_vOffset;
    float m_fTurn;
#ifndef _XBOX
    float m_fTurnAcceleration;
#endif
    float m_fPitch;
    float m_fRotation;
    float m_fDistance;
    float m_fSpeed;
    float m_fHeight;
#ifndef _XBOX
    CSWRK4SplitAcceleration m_Acceleration;
    int m_bMouseCameraRotateToggle;
#endif
};

#endif
