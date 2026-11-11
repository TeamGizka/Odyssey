#include "SWAccelerationControlers.h"
// PC 0x006707A0
CSWPhysicsState operator+(const CSWPhysicsState& stateA,
                          const CSWPhysicsState& stateB)
{
    Vector vRate = stateA.m_vRate + stateB.m_vRate;
    Vector vValue = stateA.m_vValue + stateB.m_vValue;
    return CSWPhysicsState(vValue, vRate);
}
// PC 0x00670830
CSWPhysicsState operator*(const CSWPhysicsState& state, float fScale)
{
    Vector vRate(state.m_vRate.m_fX * fScale, state.m_vRate.m_fY * fScale,
                 state.m_vRate.m_fZ * fScale);
    Vector vValue(state.m_vValue.m_fX * fScale, state.m_vValue.m_fY * fScale,
                  state.m_vValue.m_fZ * fScale);
    return CSWPhysicsState(vValue, vRate);
}
// PC 0x006D0A40
CSWPhysicsState operator/(const CSWPhysicsState& state, float fDivisor)
{
    return fDivisor != 0.0f ? CSWPhysicsState(state.m_vValue / fDivisor,
                                              state.m_vRate / fDivisor)
                            : CSWPhysicsState();
}
// PC 0x006D0B20
CSWRK4Acceleration::CSWRK4Acceleration()
    : m_fMaximumVelocity(100.0f),
      m_fMaximumAcceleration(100.0f)
{
}
// PC 0x006D0BD0
void CSWRK4Acceleration::Accelerate(const Vector& vTarget, float fTime)
{
    fTime = fTime < 0.2f ? fTime : 0.2f;

    CSWPhysicsState k1, k2, k3, k4;
    CSWPhysicsState state;
    Vector vMidTarget = 0.5f * (vTarget + m_vPreviousTarget);

    k1 = CSWPhysicsState(GetAccelerationVector(m_OldState, m_vPreviousTarget),
                          m_OldState.m_vValue) * fTime;

    state = m_OldState + k1 * 0.5f;
    k2 = CSWPhysicsState(GetAccelerationVector(state, vMidTarget),
                          state.m_vValue) * fTime;

    state = m_OldState + k2 * 0.5f;
    k3 = CSWPhysicsState(GetAccelerationVector(state, vMidTarget),
                          state.m_vValue) * fTime;

    state = m_OldState + k3;
    k4 = CSWPhysicsState(GetAccelerationVector(state, vTarget),
                          state.m_vValue) * fTime;

    m_NewState = m_OldState + k1 / 6.0f + k2 / 3.0f + k3 / 3.0f + k4 / 6.0f;
    m_vPreviousTarget = vTarget;
}
// PC 0x006D0FC0
Vector CSWRK4Acceleration::GetAccelerationVector(CSWPhysicsState& state,
                                                 const Vector& vTarget)
{
    float fRatio = m_fMaximumAcceleration / m_fMaximumVelocity;
    return m_fMaximumAcceleration * vTarget - fRatio * state.m_vValue;
}
// PC 0x006D1030
void CSWRK4Acceleration::SetVelocity(float fVelocity)
{
    m_OldState = m_NewState;
    m_NewState.m_vRate = Vector();
    m_NewState.m_vValue = Vector(0.0f, fVelocity, 0.0f);
}
// PC 0x006D10B0
CSWRK4SplitAcceleration::CSWRK4SplitAcceleration()
    : CSWRK4Acceleration(),
      m_fMaximumDeceleration(100.0f)
{
}
// PC 0x006D10D0
Vector CSWRK4SplitAcceleration::GetAccelerationVector(CSWPhysicsState& state,
                                                      const Vector& vTarget)
{
    if (vTarget.m_fX == 0.0f && vTarget.m_fY == 0.0f && vTarget.m_fZ == 0.0f)
    {
        float fRatio = m_fMaximumDeceleration / m_fMaximumVelocity;

        return m_fMaximumDeceleration * vTarget - fRatio * state.m_vValue;
    }
    return CSWRK4Acceleration::GetAccelerationVector(state, vTarget);
}
// PC 0x006D1190
CSWPhysicsState CSWRK4Acceleration::Update(Vector vTarget, float fTime,
                                           int bResetRate, int bResetValue,
                                           int bResetOldState,
                                           int bResetNewState)
{
    if (bResetOldState)
    {
        m_NewState.m_vRate = Vector();
    }
    if (bResetNewState)
    {
        m_NewState.m_vValue = Vector();
    }

    m_OldState = m_NewState;

    if (bResetRate)
    {
        m_OldState.m_vRate = Vector();
    }
    if (bResetValue)
    {
        m_OldState.m_vValue = Vector();
    }

    Accelerate(vTarget, fTime);
    return m_NewState;
}
