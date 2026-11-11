#ifndef __SWACCELERATIONCONTROLERS_H__
#define __SWACCELERATIONCONTROLERS_H__

#include "math3d.h"

class CSWPhysicsState
{
public:
    CSWPhysicsState(const Vector& vValue = Vector(),
                    const Vector& vRate = Vector())
    {
        m_vValue = vValue;
        m_vRate = vRate;
    }

    Vector m_vValue;
    Vector m_vRate;

};

CSWPhysicsState operator+(const CSWPhysicsState& stateA,
                          const CSWPhysicsState& stateB);
CSWPhysicsState operator/(const CSWPhysicsState& state, float fDivisor);
CSWPhysicsState operator*(const CSWPhysicsState& state, float fScale);

class CSWRK4Acceleration
{
public:
    virtual Vector GetAccelerationVector(CSWPhysicsState& state,
                                         const Vector& vTarget);
    CSWRK4Acceleration();
    void Accelerate(const Vector& vTarget, float fTime);
    void SetVelocity(float fVelocity);
    CSWPhysicsState Update(Vector vTarget, float fTime, int bResetRate,
                           int bResetValue, int bResetOldState,
                           int bResetNewState);

    Vector m_vPreviousTarget;
    CSWPhysicsState m_OldState;
    CSWPhysicsState m_NewState;
    float m_fMaximumVelocity;
    float m_fMaximumAcceleration;
};

class CSWRK4SplitAcceleration : public CSWRK4Acceleration
{
public:
    virtual Vector GetAccelerationVector(CSWPhysicsState& state,
                                         const Vector& vTarget);
    CSWRK4SplitAcceleration();

    float m_fMaximumDeceleration;
};

#endif
