#include "swplayercontrol.h"
#include "aurcamera.h"
#include "serverexoapp.h"
#include <float.h>
#include <math.h>
#include "appmanager.h"
#include "clientexoapp.h"
#include "nwcarea.h"
#include "nwcmodule.h"
#include "swguiabilitieschargen.h"
#include "swminigame.h"
#include "nwccreature.h"
#include "nwrules.h"

const int CSWPlayerControl::SWDRIVE_FORWARD = 1;
const int CSWPlayerControl::SWDRIVE_BACKWARD = 2;
const int CSWPlayerControl::SWDRIVE_STRAFELEFT = 16;
const int CSWPlayerControl::SWDRIVE_STRAFERIGHT = 32;
const int CSWPlayerControl::SWDRIVE_TURN = 256;
const int CSWPlayerControl::SWDRIVE_UP = 4096;
const int CSWPlayerControl::SWDRIVE_DOWN = 8192;

float fCombatTurnSpeed = 900.0f;

int g_bShowDrivingSpeed = 0;

int turbo = 0;
float turbospeed = 15.0f;
// PC 0x006792D0  Xbox 0x000938A0
int CSWPlayerControl::Initialize()
{
    m_oidPlayer = OBJECT_INVALID;
    m_pCamera = 0;
    m_bEnabled = 0;
    return 0;
}
// PC 0x006792E0  Xbox 0x000938B0
void CSWPlayerControl::SetEnabled(int bEnabled)
{
    m_bEnabled = bEnabled;
    if (m_oidPlayer != OBJECT_INVALID)
    {
        CSWCCreature* pCreature =
            g_pAppManager->m_pClientExoApp->GetCreatureByGameObjectID(
                m_oidPlayer);
        if (pCreature != 0)
        {
            if (m_bEnabled != 0)
            {
                pCreature->SwitchMode(CSWCCreature::MODE_SWDRIVE);
            }
            else
            {
                pCreature->SwitchMode(CSWCCreature::MODE_DEFAULT);
            }
        }
    }
}
// PC 0x00679340  Xbox 0x00093910
void CSWPlayerControl::SetPlayer(unsigned long oidPlayer)
{
    CClientExoApp* pClientExoApp;
    if (m_oidPlayer != OBJECT_INVALID)
    {
        pClientExoApp = g_pAppManager->GetClientExoApp();
        CSWCCreature* pCreature =
            pClientExoApp->GetCreatureByGameObjectID(m_oidPlayer);
        if (pCreature != 0)
        {
            pCreature->SwitchMode(CSWCCreature::MODE_DEFAULT);
        }
    }
    m_oidPlayer = oidPlayer;
    if (oidPlayer != OBJECT_INVALID && m_bEnabled != 0)
    {
        pClientExoApp = g_pAppManager->GetClientExoApp();
        CSWCCreature* pCreature =
            pClientExoApp->GetCreatureByGameObjectID(oidPlayer);
        if (pCreature != 0)
        {
            pCreature->SwitchMode(CSWCCreature::MODE_SWDRIVE);
        }
    }
}
// PC 0x006793B0  Xbox 0x00093980
int CSWPlayerControl::SetCamera(CAurCamera* pCamera)
{
    m_pCamera = pCamera;
    return (int)pCamera;
}

#define SWPLAYERCONTROL_DEFAULT_ACCELERATION 20.0f

float CSWPlayerControlCamRelative::GetAcceleration()
{
    if (GetPlayer() != NULL)
    {
        return GetPlayer()->GetDriveAcceleration();
    }
    return SWPLAYERCONTROL_DEFAULT_ACCELERATION;
}
// PC 0x006793C0  Xbox 0x00093990
void CSWPlayerControlCamRelative::Init()
{
    m_fReserved34 = 480.0f;
    CSWCArea* pArea = g_pAppManager->m_pClientExoApp->GetModule()->m_pInfo;
    int nRow = pArea->m_nTurnRateRow;
    CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
    EXOXBOXASSERT(p2DArrays->m_pCameraStyle);
    C2DA* pTable = p2DArrays->m_pCameraStyle;
    {
        CExoString sColumn("MaxTurnRate");
        pTable->GetFLOATEntry(nRow, sColumn, &m_fMaxTurnRate);
    }
    {
        CExoString sColumn("MinTurnRate");
        pTable->GetFLOATEntry(nRow, sColumn, &m_fMinTurnRate);
    }
    m_vFacing = Vector();
    m_integrator.m_fMaximumVelocity =
        g_pAppManager->m_pClientExoApp->GetClientOptions()
            ->GetCameraKeyboardDPS();
    m_integrator.m_fMaximumAcceleration =
        g_pAppManager->m_pClientExoApp->GetClientOptions()
            ->GetCameraKeyboardAcceleration();
    float fDeceleration =
        g_pAppManager->m_pClientExoApp->GetClientOptions()
            ->GetCameraKeyboardDeceleration();
    m_integrator.m_fMaximumDeceleration = fDeceleration;
}
// PC 0x00679510  Xbox 0x00093AA0
float CSWPlayerControlCamRelative::GetMaxSpeed()
{
    if (turbo ||
        g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nCheatFlags)
    {
        return turbospeed;
    }

    if (GetPlayer() != 0)
    {
        g_pAppManager->m_pClientExoApp->GetSWParty();
        CSWCCreature* pCreature = GetPlayer();
        if (pCreature->m_bRunning)
        {
            pCreature = GetPlayer();
            return pCreature->m_pAppearance->m_fDriveAnimWalk;
        }
        pCreature = GetPlayer();
        return pCreature->m_pAppearance->m_fDriveMaxSpeed;
    }
    return 6.0f;
}
// PC 0x006795C0  Xbox 0x00093B30
void CSWPlayerControlCamRelative::ResetDriveAcceleration()
{
    m_stateB.m_vRate = Vector();
    m_stateB.m_vValue = Vector();
    m_stateA.m_vRate = Vector();
    m_stateA.m_vValue = Vector();
    m_vTarget = Vector();
    m_vFacing = Vector();
}

CSWPlayerControl::CSWPlayerControl()
{
    m_oidPlayer = OBJECT_INVALID;
    m_pCamera = 0;
    m_bEnabled = 0;
}

CSWPlayerControl::CSWPlayerControl(unsigned long oidPlayer, CAurCamera* pCamera)
{
    m_oidPlayer = OBJECT_INVALID;
    m_pCamera = 0;
    m_bEnabled = 0;
    SetPlayer(oidPlayer);
    m_pCamera = pCamera;
}

CSWPlayerControlCamRelative::CSWPlayerControlCamRelative()
{
    Init();
}
// PC 0x00679780  Xbox 0x00093CB0
CSWPlayerControlCamRelative::CSWPlayerControlCamRelative(
    unsigned long oidPlayer, CAurCamera* pCamera)
    : CSWPlayerControl(oidPlayer, pCamera)
{
    Init();
}
// PC 0x00679870  Xbox 0x00093E00
inline Vector CSWPlayerControlCamRelative::GetAcceleration(
    CSWMGPhysicsState& state, const Vector& vTarget)
{
    float fAcceleration;
    if (GetPlayer() != 0)
    {
        CSWCCreature* pCreature = GetPlayer();
        fAcceleration = pCreature->GetDriveAcceleration();
    }
    else
    {
        fAcceleration = 25.0f;
    }
    float fMaxSpeed = GetMaxSpeed();
    if (fMaxSpeed <= 1.8f)
    {
        fAcceleration = fAcceleration * fMaxSpeed * 0.55555558f;
    }
    float fDrag = fAcceleration / fMaxSpeed;
    return fAcceleration * vTarget - fDrag * state.m_vValue;
}
// PC 0x00679940  Xbox 0x00093EF0
int CSWPlayerControlCamRelative::Control(float fTimeStep)
{
    float fTime = 1.0f < fTimeStep ? 1.0f : fTimeStep;
    if (m_oidPlayer != OBJECT_INVALID && m_pCamera != NULL &&
        g_pAppManager->GetServerExoApp() != NULL)
    {
        CSWCCreature* pPlayer = GetPlayer();
        CSWSCreature* pServerPlayer = pPlayer->GetServerCreature();
        if (pServerPlayer != NULL && pPlayer != NULL &&
            !pServerPlayer->GetDead() && !pServerPlayer->GetIsPCDying() &&
            !pServerPlayer->GetDeadTemp())
        {
#ifndef _XBOX
            if (g_pAppManager->GetClientExoApp()->GetClientOptions()->m_nCameraMode ==
                CClientOptions::CAMERA_MODE_WORLD_CLICK_DISABLED)
            {
                float fTurn = m_integrator.Update(
                    Vector(0.0f, pPlayer->m_fFreeLookTurn, 0.0f),
                    fTime, 1, 0, 0, 0).m_vRate.m_fY;
                pPlayer->m_fFreeLookTurn = 0.0f;
                Quaternion qOrientation = pointto(pPlayer->m_vOrientation);
                qOrientation = YawPitchRoll(Yaw(qOrientation) + fTurn,
                                           Pitch(qOrientation), Roll(qOrientation));
                pPlayer->SetOrientation(qOrientation.zdir());
                pServerPlayer->SetOrientation(qOrientation.zdir());
            }
#endif

            Vector vInput(-m_fLeftRight, m_fUpDown, 0.0f);
            if (vInput.m_fX != 0.0f && vInput.m_fY != 0.0f)
                vInput.Normalize();
#ifndef _XBOX
            if (m_bPlayerWalking)
                vInput *= 0.5f;
#endif

            Vector vCameraDirection = m_pCamera->GetOrientation().zdir();
            vCameraDirection.m_fZ = 0.0f;
            if (vCameraDirection.m_fX != 0.0f && vCameraDirection.m_fY != 0.0f)
                vCameraDirection.Normalize();
            Quaternion qCamera = YawPitchRoll(Yaw(vCameraDirection), 0.0f, 0.0f);
            unsigned char bStopped = vInput.IsZero();
            pPlayer->m_fDriveTurnAngle = 0.0f;
            if (m_bEnabled && !bStopped)
            {
                Vector vDirection = qCamera * vInput;
                pointto(vDirection);
                float fYaw = Yaw(vDirection);
                float fCurrentYaw = Yaw(pPlayer->m_vOrientation);
                if (fCurrentYaw < 0.0f)
                    fCurrentYaw += 360.0f;
                float fDesiredYaw = (fYaw < 0.0f) ? (fYaw + 360.0f) : fYaw;

                float fDifference;
                if (fDesiredYaw > fCurrentYaw)
                {
                    fDifference = fDesiredYaw - fCurrentYaw;
                    if (fDifference <= 180.0f)
                        m_fTurnDirection = 1.0f;
                    else
                        m_fTurnDirection = -1.0f;
                }
                else
                {
                    fDifference = fCurrentYaw - fDesiredYaw;
                    if (fDifference <= 180.0f)
                        m_fTurnDirection = -1.0f;
                    else
                        m_fTurnDirection = 1.0f;
                }
                if (fDifference > 180.0f)
                    fDifference = 360.0f - fDifference;
                pPlayer->m_fDriveTurnAngle = fDifference * m_fTurnDirection;
                if (fabs(fDifference) > SWPLAYERCONTROL_TURN_EPSILON &&
                    fabs(pPlayer->GetSWAnimationSpeed()) > SWPLAYERCONTROL_TURN_EPSILON)
                {
                    float fSpeed = magnitude(m_stateB.m_vValue);
                    float fScale = 1.0f - fSpeed / GetMaxSpeed();
                    float fTurn = (m_fMinTurnRate +
                        (m_fMaxTurnRate - m_fMinTurnRate) * fScale) *
                        fTime * m_fTurnDirection;
                    if (fTurn > fDifference)
                        fTurn = fDifference;
                    else if (fTurn < -fDifference)
                        fTurn = -fDifference;
                    Quaternion qTurn = YawPitchRoll(fTurn, 0.0f, 0.0f);
                    vDirection = normalize(qTurn * pPlayer->m_vOrientation);
                    if (pServerPlayer->m_nAIStateFlags & CSWSCreature::AI_STATE_CAN_MOVE)
                    {
                        pPlayer->SetOrientation(vDirection);
                        pPlayer->SetMoveOrientation(vDirection);
                    }
                    m_vFacing = vDirection;
                }
            }

            float fAnimationSpeed = 0.0f;
            if (bStopped &&
                (m_stateB.m_vValue.m_fX != 0.0f || m_stateB.m_vValue.m_fY != 0.0f) &&
                fabs(m_stateB.m_vValue.m_fX) < SWPLAYERCONTROL_STOP_SPEED &&
                fabs(m_stateB.m_vValue.m_fY) < SWPLAYERCONTROL_STOP_SPEED)
            {
                m_stateB.m_vValue = Vector();
                m_stateA = m_stateB;
            }
            else
            {
                if (vInput.m_fX == 0.0f && vInput.m_fY != 0.0f)
                    m_stateB.m_vValue.m_fX = 0.0f;
                if (vInput.m_fY == 0.0f && vInput.m_fX != 0.0f)
                    m_stateB.m_vValue.m_fY = 0.0f;

                CSWMGPhysicsState state, k1, k2, k3, k4;
                Vector vMidTarget = (vInput + m_vTarget) * 0.5f;
                k1 = GetDerivative(m_stateB.m_vValue,
                                   GetAcceleration(m_stateB, m_vTarget)) * fTime;
                state = m_stateB + k1 * 0.5f;
                k2 = GetDerivative(state.m_vValue,
                                   GetAcceleration(state, vMidTarget)) * fTime;
                state = m_stateB + k2 * 0.5f;
                k3 = GetDerivative(state.m_vValue,
                                   GetAcceleration(state, vMidTarget)) * fTime;
                state = m_stateB + k3;
                k4 = GetDerivative(state.m_vValue,
                                   GetAcceleration(state, vInput)) * fTime;
                m_stateA = m_stateB + k1 / 6.0f + k2 / 3.0f +
                           k3 / 3.0f + k4 / 6.0f;
                m_vTarget = vInput;
                Vector vDisplacement = m_stateA.m_vRate - m_stateB.m_vRate;
                float fMinimumDisplacement = SWPLAYERCONTROL_FAST_DISPLACEMENT_SQUARED;
                if (GetMaxSpeed() < SWPLAYERCONTROL_FAST_SPEED)
                    fMinimumDisplacement = 0.0f;
                if (vDisplacement.m_fX * vDisplacement.m_fX +
                    vDisplacement.m_fY * vDisplacement.m_fY +
                    vDisplacement.m_fZ * vDisplacement.m_fZ > fMinimumDisplacement)
                {
                    Vector vOrientation(pPlayer->m_vOrientation.m_fX,
                                        pPlayer->m_vOrientation.m_fY, 0.0f);
                    pPlayer->SetOrientation(normalize(vOrientation));
                    vDisplacement = pPlayer->m_vOrientation * magnitude(vDisplacement);
                    Vector vPosition = pPlayer->m_vPosition + vDisplacement;
                    float fDistance = magnitude(pPlayer->m_vPosition - vPosition);
                    fAnimationSpeed = magnitude(m_stateA.m_vValue);
                    if (_finite(fAnimationSpeed) && fAnimationSpeed > 1.0f)
                    {
                        if (!pPlayer->SetPrecalculatedPosition(vPosition))
                            m_vFacing = Vector();
                        pPlayer->ResetCameraToFollow();
                    }
                    else
                        fAnimationSpeed = 0.0f;
                }
                else if (g_pAppManager->GetClientExoApp()->GetClientOptions()->m_nCameraMode ==
                         CClientOptions::CAMERA_MODE_COMBAT_FACING)
                {
                    unsigned long oidTarget = g_pAppManager->GetServerExoApp()->
                        ClientToServerObjectId(g_pAppManager->GetClientExoApp()->GetLastTarget());
                    CSWSCreature* pTarget = g_pAppManager->GetServerExoApp()->
                        GetCreatureByGameObjectID(oidTarget);
                    if (pTarget != NULL &&
                        pServerPlayer->GetReputation(pTarget->GetId(), 0) <=
                            CSWSObject::MAX_HOSTILE_REPUTATION &&
                        !pPlayer->IsDriveAnimationBlocked())
                    {
                        Vector vDirection = pTarget->m_vPosition - pServerPlayer->m_vPosition;
                        Vector vOrientation = pServerPlayer->m_vOrientation;
                        float fDifference = Yaw(vDirection) - Yaw(vOrientation);
                        if (fDifference > 180.0f)
                            fDifference -= 360.0f;
                        if (fDifference < -180.0f)
                            fDifference += 360.0f;
                        if (fDifference < 0.0f)
                        {
                            float fTurn = -fCombatTurnSpeed * fTime;
                            if (fDifference < fTurn)
                                fDifference = fTurn;
                        }
                        else
                        {
                            float fTurn = fCombatTurnSpeed * fTime;
                            if (fDifference > fTurn)
                                fDifference = fTurn;
                        }
                        Vector vFacing = YawPitchRoll(fDifference, 0.0f, 0.0f) * vOrientation;
                        pServerPlayer->SetOrientation(vFacing);
                        pPlayer->SetOrientation(vFacing);
                    }
                }
                m_stateB = m_stateA;
            }
            pPlayer->SetSWAnimationSpeed(fAnimationSpeed, 0.0f);
            if (fAnimationSpeed > 1.0f)
                pPlayer->GetAnimationBase()->SetLean(0.0f);
            else
                pPlayer->GetAnimationBase()->SetLean(0.0f);
            return 1;
        }
    }
    return 0;
}
