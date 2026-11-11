#include <math.h>
#include "nwcmodule.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "clientoptions.h"
#include "exoresman.h"
#include "nwcarea.h"
#include "nwcambientsound.h"
#include "clientexoappinternal.h"
#include "guiingame.h"
#include "nwmessage.h"
#include "swgui.h"
#include "aurcamera.h"
#include "gameobjectarray.h"
#if defined(_WIN32) && !defined(_XBOX)
#include <windows.h>
#else
#include "platform.h"
#endif
#include "worldtimer.h"
#include "swbehaviorcamera.h"
#include "swbehaviorcameracombat.h"
#include "swbehaviorcameradialog.h"
#include "swminigame.h"
#include "swparty.h"

#include "nwccreature.h"
#include "nwcanimbase.h"
#include "swmain.h"

static inline void NwcmoduleTrace5();
static inline void NwcmoduleTrace4();
static inline void NwcmoduleTrace3();
static inline void NwcmoduleTrace2();
static inline void NwcmoduleTrace1();

const int CCameraSettings::CAMERA_ON_STICK = 0;
const int CCameraSettings::CAMERA_FREE_LOOK = 1;

int lockcamera(char* szValue);

char modulereturnvalue[260];
int useclearlineofsight = 0;
float g_fMinDist = 5.0f;
float g_fMaxDist = 20.0f;
float g_fMinPitch = 1.0f;
float g_fMaxPitch = 50.0f;
float camerafollowangle = 30.0f;
float camerafollowspeedfactor = 1.0f;
int enablecameracollision = 1;
float fShakePeriod = 30.0f;
float fShakeMag = 20.0f;
// PC 0x00643F40  Xbox 0x00067170
CSWCModule::CSWCModule()
    : CGameObject(CGameObject::OBJECT_TYPE_MODULE, OBJECT_INVALID)
{
    m_vPlayerPosition = Vector(0.0f, 0.0f, 0.0f);
    m_vCameraPosition = Vector(0.0f, 0.0f, 0.0f);
    m_pModuleCamera = NULL;
    m_nReserved44 = 0;
    m_pInfo = NULL;
    m_nCameraType = CAMERA_UNINITIALIZED;
    m_nReserved80 = 0;
    m_nReserved84 = 0;
    m_nReserved88 = 0;
    m_vCameraOffset = Vector(0.0f, 0.0f, 1.0f);
    m_oidCameraTarget = CGameObjectArray::INVALIDOBJID;
    m_fCameraDistance = 0.0f;
    m_fCameraYaw = 0.0f;
    m_fCameraPitch = 0.0f;
    m_nReserved90 = 0;
    m_bCameraCollisionChanged = 0;
    m_fCameraTilt = 0.0f;
    m_fCameraTurn = 0.0f;
    m_fCameraZoom = 0.0f;
    m_sModuleTitle = "";
    m_nTimeA4 = CWorldTimer::DEFAULT_YEAR;
    m_nTimeA8 = 1;
    m_nTimeAC = 1;
    m_nTimeOfDayHour = 5;
    m_nTimeB0 = 0;
    m_nTimeOfDayState = TIME_DEFAULT;
    m_fTimeOfDay = 0.0f;
    m_fTimeOfDayHours = 0.0f;
    m_nReservedD4 = 0;
    m_nReservedC8 = 0;
    m_nReservedCC = 0;
    m_nReservedD0 = 0;
    m_bCameraModeChanged = 0;
    m_nCameraShakeTime = 0;
    m_nCameraShakeAmount = 0;
    m_nCameraShakeDelay = 0;
    m_bCameraShakeChanged = 1;
    m_nCameraBumpTime = 0;
    m_nCameraBumpDelay = 0;
    m_bCameraBumpChanged = 1;
    m_nReservedFC = 1;
    m_nReserved100 = 0;
    m_nReserved104 = 0;
    m_nReserved108 = 0;
    m_cameraSettings.m_bValid = 0;
    m_pAnimatedCamera = NULL;
    m_nLastCameraMode = CAMERA_UNINITIALIZED;
}
// PC 0x0063F5E0  Xbox 0x00062040
inline int CSWCObject::IsModelLoaded()
{
    if (m_pAnimBase != NULL &&
        m_pAnimBase->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN) != NULL)
    {
        return 1;
    }
    return 0;
}
// PC 0x0063F600  Xbox 0x00062060
inline Vector CSWCameraOnAStick::GetPosition()
{
    if (m_pCamera != NULL)
    {
        return m_pCamera->GetPosition();
    }
    return Vector();
}
// PC 0x0063F630  Xbox 0x000620A0
void CSWCameraOnAStick::SetPosition(Vector vPosition)
{
    if (m_pCamera != NULL)
    {
        m_pCamera->SetPosition(vPosition);
    }
}
// PC 0x004015A0
void CSWCModule::UnloadModule()
{
}
// PC 0x0063F660  Xbox 0x00062690
int CSWCModule::LoadModule()
{
    CSWCMessage* pMessage = g_pAppManager->GetClientExoApp()->GetSWCMessage();

    m_pInfo = new CSWCArea;
    if (m_pInfo != NULL)
    {
        if (m_pModuleCamera == NULL)
        {
            m_pModuleCamera = NewCAurCamera("camera");
        }
        if (m_pModuleCamera == NULL)
        {
            delete m_pInfo;
            m_pInfo = NULL;
            return 0;
        }

        m_pModuleCamera->SetClipDist(0.1f, 10000.0f);
        g_pAppManager->GetClientExoApp()->GetClientOptions()->SetCameraMode(
            CClientOptions::CAMERA_MODE_ON_A_STICK);

        m_sModuleName = pMessage->ReadCExoString(32);
        m_sModuleTitle = pMessage->ReadCExoLocStringClient(CResRef(), NULL);
        m_nTimeOfDayHour = pMessage->ReadBYTE(8);
        m_resHak = pMessage->ReadCResRef(16);
        m_resReserved28 = pMessage->ReadCResRef(16);
        g_pAppManager->GetClientExoApp()->GetWorldTimer()->SetMinutesPerHour(
            m_nTimeOfDayHour);

        if (m_resHak != "")
        {
            CExoString sHakFile;
            sHakFile = "HAK:";
            sHakFile = sHakFile + CExoString((char*)m_resHak.GetResRef(), 16);
            if (!g_pExoResMan->AddEncapsulatedResourceFile(
                    sHakFile, CExoResMan::PRIORITY_MODULE))
            {
                delete m_pModuleCamera;
                m_pModuleCamera = NULL;
                delete m_pInfo;
                m_pInfo = NULL;
                return 0;
            }
        }

        if (!pMessage->MessageReadOverflow(1) &&
            !pMessage->MessageReadUnderflow(1))
        {
            g_pAppManager->GetClientExoApp()->SetLoadScreenByModuleName(m_sModuleName);
            g_pAppManager->GetClientExoApp()->ShowLoadScreen(1, 1, 0);
            g_pAppManager->GetClientExoApp()->SetLoadBarProgress(
                g_pAppManager->GetClientExoApp()->GetLoadStep(1) +
                g_pAppManager->GetClientExoApp()->GetLoadStep(0), 1);
            UpdateScreen(0.033333335f, 0, 0);
            return 1;
        }

        delete m_pModuleCamera;
        m_pModuleCamera = NULL;
        delete m_pInfo;
        m_pInfo = NULL;
    }
    return 0;
}
// PC 0x0063F9D0  Xbox 0x000629E0
void CSWCModule::SetCameraFollow(unsigned long oidTarget, int bSnap)
{
    CSWCCreature* pCreature;
    CSWCameraOnAStick* pOnAStick;
    CSWCObject* pObject;
    CSWCObject* pFollowed;
    Gob* pModel;
    CAurBehaviorCameraFollow* pFollow;
    CAurBehavior* pBehavior;
    unsigned long oidPrevious;

    if (g_pAppManager->m_pClientExoApp->GetCombatMode() == 0)
    {
        CClientExoApp* pApp = g_pAppManager->m_pClientExoApp;

        if (m_nCameraType == 3)
        {
            pCreature = pApp->GetCreatureByGameObjectID(oidTarget);
            if (pCreature != NULL)
            {
                pOnAStick = reinterpret_cast<CSWCameraOnAStick*>(
                    m_pModuleCamera->GetBehavior(AUR_BEHAVIOR_CAMERA_ONASTICK));
                if (pOnAStick == NULL)
                {
                    pOnAStick =
                        new CSWCameraOnAStick(m_pModuleCamera->AsCAurObject(),
                                              pCreature->GetCameraFollowPart());
                    m_pModuleCamera->SetBehavior(pOnAStick);
                }
                Vector vPosition = pOnAStick->GetPosition();
                pOnAStick->m_nHeightMode = pCreature->GetFollowingBody();
                pOnAStick->m_fPitch =
                    pCreature->GetHeadHeightOffset() + pCreature->GetCameraOffset();
                pOnAStick->ChangeTarget(pCreature->GetCameraFollowPart(),
                                        bSnap);
                m_oidCameraTarget = oidTarget;
                if (bSnap == 0)
                {
                    pOnAStick->SetPosition(vPosition);
                    Vector vLast = vPosition;
                    pOnAStick->m_vLastPosition = vLast;
                }
            }
        }
        else
        {
            pObject = pApp->GetGameObject(oidTarget);
            if (pObject != NULL)
            {
                pFollowed = pObject->AsSWCObject();
                if (pFollowed != NULL)
                {
                    pModel = pFollowed->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1);
                    if (pModel != NULL)
                    {
                        oidPrevious = m_oidCameraTarget;
                        m_oidCameraTarget = oidTarget;
                        m_bCameraCollisionChanged = bSnap;
                        if (m_pModuleCamera != NULL)
                        {
                            m_pModuleCamera->SetFollow(pModel);
                            m_vCameraPosition = pModel->GetPosition();
                            pBehavior = m_pModuleCamera->GetBehavior(-1);
                            if (pBehavior != 0)
                            {
                                pFollow =
                                    pBehavior->AsBehaviorCameraFollow();
                                if (pFollow != NULL)
                                {
                                    pFollow->m_fPitch = g_fMaxPitch;
                                    pFollow->m_fSpring = 750.0f;
                                    if (lockcamera(NULL) != 0)
                                    {
                                        pFollow->m_fClampPitch = 1.0f;
                                        pFollow->m_fClampDistance = 1.0f;
                                    }
                                    pFollow->m_fMinPitch = g_fMinPitch;
                                    pFollow->m_fMaxPitch = g_fMaxPitch;
                                    pFollow->m_fMinDistance =
                                        g_fMinDist;
                                    pFollow->m_fMaxDistance =
                                        g_fMaxDist;
                                    if (m_fCameraDistance != 0.0f)
                                    {
                                        pFollow->m_fDistance =
                                            m_fCameraDistance;
                                        pFollow->m_fYaw = m_fCameraYaw;
                                        pFollow->m_fPitch = m_fCameraPitch;
                                    }
                                    pFollow->m_vOffset = m_vCameraOffset;
                                }
                            }
                            pApp = g_pAppManager->m_pClientExoApp;
                            pCreature =
                                pApp->GetCreatureByGameObjectID(oidPrevious);
                            if (pCreature != NULL)
                            {
                                pCreature->UpdateShadowBlob();
                            }
                            if (pFollowed->AsSWCCreature() != NULL)
                            {
                                pFollowed->AsSWCCreature()->UpdateShadowBlob();
                            }
                        }
                    }
                }
            }
        }
    }
}
// PC 0x0063FCC0  Xbox 0x00062D00
void CSWCModule::TiltCamera(float fTilt)
{
    CAurBehavior* pResult;
    CAurBehavior* pBehavior;
    CAurBehaviorCameraFollow* pFollow;

    if (m_nCameraType == 3)
    {
        pResult = m_pModuleCamera->GetBehavior(AUR_BEHAVIOR_CAMERA_ONASTICK);
        if (pResult != 0)
        {
            reinterpret_cast<CSWCameraOnAStick*>(pResult)->m_fTilt = fTilt;
        }
    }
    else if (m_nCameraType == 5)
    {
        pResult = m_pModuleCamera->GetBehavior(AUR_BEHAVIOR_CAMERA_FREELOOK);
        if (pResult != 0)
        {
            reinterpret_cast<CSWCameraFreeLook*>(pResult)->m_fTiltInput = fTilt;
        }
    }
    else if (m_nCameraType == 7)
    {
        pResult = m_pModuleCamera->GetBehavior(AUR_BEHAVIOR_CAMERA_NAVIGATE);
        if (pResult != 0)
        {
            reinterpret_cast<CSWCameraNavigate*>(pResult)->m_fTilt =
                fTilt * -60.0f;
        }
    }
    else
    {
        if (m_pModuleCamera != NULL)
        {
            pBehavior = m_pModuleCamera->GetBehavior(-1);
            if (pBehavior != 0)
            {
                pFollow = pBehavior->AsBehaviorCameraFollow();
                if (pFollow != NULL)
                {
                    pFollow->m_fTilt += fTilt;
                    m_fCameraTilt = pFollow->m_fTilt;
                }
            }
        }
        int bChanged;
        if (fTilt != 0.0f)
        {
            bChanged = 1;
        }
        else
        {
            bChanged = 0;
        }
        m_nReserved80 |= bChanged;
    }
}

#ifndef _XBOX
void CSWCModule::AcclTiltCamera(float fTilt)  // PC 0x0063FDB0
{
    int nCameraType;
    CAurBehavior* pBehavior;

    nCameraType = m_nCameraType;
    if (nCameraType == 3)
    {
        m_pModuleCamera->GetBehavior(AUR_BEHAVIOR_CAMERA_ONASTICK);
    }
    else if (nCameraType == 5)
    {
        pBehavior = m_pModuleCamera->GetBehavior(AUR_BEHAVIOR_CAMERA_FREELOOK);
        if (pBehavior != 0)
        {
            reinterpret_cast<CSWCameraFreeLook*>(pBehavior)->m_fTiltAcceleration =
                -fTilt;
        }
    }
    else if (nCameraType == 7)
    {
        m_pModuleCamera->GetBehavior(AUR_BEHAVIOR_CAMERA_NAVIGATE);
    }
}

#endif
// PC 0x0063FE10  Xbox 0x00062DD0
void CSWCModule::TurnCamera(float fTurn, float fTimeStep)
{
    if (g_pAppManager->m_pClientExoApp->GetCombatMode() != 0)
    {
        CSWBehaviorCameraCombat* pCombat =
            reinterpret_cast<CSWBehaviorCameraCombat*>(
                m_pModuleCamera->GetBehavior(AUR_BEHAVIOR_CAMERA_COMBAT));
        if (pCombat != NULL)
        {
#ifdef _XBOX
            pCombat->m_fTurn = fTurn;
#else
            pCombat->m_fTurnAcceleration = 0.0f;
            pCombat->SetTurn(-fTurn);
#endif
        }
    }
    else if (m_nCameraType == CAMERA_ON_A_STICK)
    {
        CSWCameraOnAStick* pOnAStick = reinterpret_cast<CSWCameraOnAStick*>(
            m_pModuleCamera->GetBehavior(AUR_BEHAVIOR_CAMERA_ONASTICK));
        if (pOnAStick != NULL)
        {
#ifdef _XBOX
            pOnAStick->m_fTurn = fTurn;
#else
            pOnAStick->m_fTurn = fTurn;
            pOnAStick->m_fTurnAcceleration = 0.0f;
#endif
        }
    }
    else if (m_nCameraType == CAMERA_FREE_LOOK)
    {
        CSWCameraFreeLook* pFreeLook = reinterpret_cast<CSWCameraFreeLook*>(
            m_pModuleCamera->GetBehavior(AUR_BEHAVIOR_CAMERA_FREELOOK));
        if (pFreeLook != NULL)
        {
#ifdef _XBOX

            fTurn = pFreeLook->m_fRotateSpeed * fTimeStep * fTurn;
#else
            float fTurnRate = max(-30.0f, min(30.0f, fTurn * 0.5f));
            float fSpeed = pFreeLook->m_fRotateSpeed;
            fSpeed *= fTimeStep;
            fTurn = -(fSpeed * fTurnRate);
#endif
            CSWCCreature* pCreature =
                g_pAppManager->m_pClientExoApp->GetPlayerCreature();
            if (pCreature != NULL)
            {
#ifdef _XBOX
                Quaternion qOrientation = pointto(pCreature->m_vOrientation);
                qOrientation = YawPitchRoll(Yaw(qOrientation) + fTurn,
                                            Pitch(qOrientation),
                                            Roll(qOrientation));
#else
                pCreature->m_fFreeLookTurn = 0.0f;
                Quaternion qOrientation = pointto(pCreature->m_vOrientation);
                qOrientation = YawPitchRoll(Yaw(qOrientation) + fTurn,
                                            Pitch(qOrientation),
                                            Roll(qOrientation));
#endif
                pCreature->SetOrientation(qOrientation.zdir());
                if (pCreature->GetServerCreature() != NULL)
                {
                    pCreature->GetServerCreature()->SetOrientation(
                        qOrientation.zdir());
                }
            }
        }
    }
#ifndef _XBOX
    else if (m_nCameraType == CAMERA_NAVIGATE)
    {
        CSWCameraNavigate* pNavigate = reinterpret_cast<CSWCameraNavigate*>(
            m_pModuleCamera->GetBehavior(AUR_BEHAVIOR_CAMERA_NAVIGATE));
        if (pNavigate != NULL)
        {
            pNavigate->m_fTurn = fTurn * -60.0f;
        }
    }
#endif
    else
    {
        if (m_pModuleCamera != NULL)
        {
            CAurBehavior* pBehavior = m_pModuleCamera->GetBehavior(-1);
            if (pBehavior != 0)
            {
                CAurBehaviorCameraFollow* pFollow =
                    pBehavior->AsBehaviorCameraFollow();
                if (pFollow != NULL)
                {
                    pFollow->m_fTurn += fTurn;
                    m_fCameraTurn = pFollow->m_fTurn;
                }
            }
        }
        if (fTurn != 0.0f)
        {
            int bChanged = 1;
            m_nReserved100 = bChanged;
            m_nReserved80 |= bChanged;
        }
    }
}
// PC 0x00640090
void CSWCModule::AcclTurnCamera(float fTurn, float fTimeStep)
{
    CSWBehaviorCameraCombat* pCombat;
    CSWCCreature* pCreature;
    CAurBehaviorCameraFollow* pFollow;
    CAurBehavior* pBehavior;

    if (g_pAppManager->m_pClientExoApp->GetCombatMode() != 0)
    {
        pCombat = reinterpret_cast<CSWBehaviorCameraCombat*>(
            m_pModuleCamera->GetBehavior(AUR_BEHAVIOR_CAMERA_COMBAT));
        if (pCombat != NULL)
        {
#ifndef _XBOX
            pCombat->SetTurn(0.0f);
            pCombat->m_fTurnAcceleration = -fTurn;
#endif
        }
    }
    else if (m_nCameraType == 3)
    {
        CSWCameraOnAStick* pOnAStick = reinterpret_cast<CSWCameraOnAStick*>(
            m_pModuleCamera->GetBehavior(AUR_BEHAVIOR_CAMERA_ONASTICK));
        if (pOnAStick != NULL)
        {
#ifndef _XBOX
            pOnAStick->m_fTurn = 0;
            pOnAStick->m_fTurnAcceleration = fTurn;
#endif
        }
    }
    else if (m_nCameraType == 5)
    {
        CSWCameraFreeLook* pFreeLook = reinterpret_cast<CSWCameraFreeLook*>(
            m_pModuleCamera->GetBehavior(AUR_BEHAVIOR_CAMERA_FREELOOK));
        if (pFreeLook != NULL)
        {
            pCreature = g_pAppManager->m_pClientExoApp->GetPlayerCreature();
            if (pCreature != NULL)
            {
#ifndef _XBOX
                pCreature->m_fFreeLookTurn = -fTurn;
#endif
            }
        }
    }
    else if (m_nCameraType == 7)
    {
        CSWCameraNavigate* pNavigate = reinterpret_cast<CSWCameraNavigate*>(
            m_pModuleCamera->GetBehavior(AUR_BEHAVIOR_CAMERA_NAVIGATE));
        if (pNavigate != NULL)
        {
            pNavigate->m_fTurn = fTurn * -60.0f;
        }
    }
    else
    {
        if (m_pModuleCamera != NULL)
        {
            pBehavior = m_pModuleCamera->GetBehavior(-1);
            if (pBehavior != 0)
            {
                pFollow = pBehavior->AsBehaviorCameraFollow();
                if (pFollow != NULL)
                {
                    pFollow->m_fTurn += fTurn;
                    m_fCameraTurn = pFollow->m_fTurn;
                }
            }
        }
        if (fTurn != 0.0f)
        {
            int bChanged = 1;
            m_nReserved100 = bChanged;
            m_nReserved80 |= bChanged;
        }
    }
}
// PC 0x006401D0  Xbox 0x000630B0
void CSWCModule::ZoomCamera(float fZoom)
{
    CAurBehavior* pBehavior;
    CAurBehaviorCameraFollow* pFollow;

    if (m_pModuleCamera != NULL)
    {
        pBehavior = m_pModuleCamera->GetBehavior(-1);
        if (pBehavior != 0)
        {
            pFollow = pBehavior->AsBehaviorCameraFollow();
            if (pFollow != NULL)
            {
                pFollow->m_fSpeed += fZoom;
                m_fCameraZoom = pFollow->m_fSpeed;
            }
        }
    }
    if (fZoom != 0.0f)
    {
        int bChanged = 1;
        m_nReserved104 = bChanged;
        m_nReserved80 |= bChanged;
    }
}
// PC 0x00640240  Xbox 0x00063120
void CSWCModule::ScrollCamera(Vector vScroll)
{
    Vector vPosition;

    if (!(vScroll * vScroll <= 0.00001f))
    {
        vPosition = m_pModuleCamera->GetPosition();
        vPosition += vScroll;
        m_pModuleCamera->SetPosition(vPosition);
        m_nReserved80 = 1;
    }
}
// PC 0x006402F0  Xbox 0x000631E0
int CSWCModule::GetNextCameraPosition(float fTimeStep, Vector* pPosition)
{
    Quaternion qTarget;

    if (m_pModuleCamera == NULL)
    {
        return 0;
    }
    CSWCObject* pObject =
        g_pAppManager->GetClientExoApp()->GetGameObject(m_oidCameraTarget);
    if (pObject == NULL)
    {
        return 0;
    }
    CSWCObject* pFollowed = pObject->AsSWCObject();
    if (pFollowed == NULL)
    {
        return 0;
    }
    Gob* pModel = pFollowed->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1);
    if (pModel == NULL)
    {
        return 0;
    }

    float fYaw = m_pModuleCamera->GetYaw() + fTimeStep * m_fCameraTurn;
    float fPitch = m_pModuleCamera->GetPitch() + fTimeStep * m_fCameraTilt;
    float fDistance = m_pModuleCamera->GetDist() *
                      powf(2.0f, fTimeStep * m_fCameraZoom * -0.5f);
    fPitch = max(g_fMinPitch, min(fPitch, g_fMaxPitch));
    fDistance =
        max(g_fMinDist, min(fDistance, g_fMaxDist));

    float fSpring = 1.0f - powf(0.5f, fTimeStep * 750.0f);
    Vector vPosition;
    vPosition = (1.0f - fSpring) * m_vCameraPosition +
                fSpring * (m_vCameraOffset + pModel->GetPosition());
    qTarget = YawPitchRoll(fYaw, fPitch, 0.0f);
    Quaternion qOrientation;
    qOrientation = normalize(
        slerp(m_pModuleCamera->GetOrientation(), qTarget, fSpring));
    Vector vOffset = qOrientation.zdir() * fDistance;
    m_vCameraPosition = vPosition;
    *pPosition = vPosition + vOffset;
    return 1;
}
// PC 0x00640640  Xbox 0x000635D0
void CSWCModule::SetTimeOfDayState(unsigned char nState, unsigned long nTime)
{
    CSWCArea* pArea;
    CSWCArea* pCurrentArea;
    int nHours;

    pArea = m_pInfo;
    if (pArea != NULL)
    {
        switch (nState)
        {
        case 1:
            pArea->SetAsDay();
            break;
        case 2:
            pArea->SetAsNight();
            break;
        case 3:
            pArea->SetAsNight();
            m_pInfo->SetAsDawn();
            break;
        case 4:
            pArea->SetAsDay();
            m_pInfo->SetAsDusk();
            break;
        }
    }
    if (m_nTimeOfDayState != nState)
    {
        pCurrentArea = m_pInfo;
        m_nTimeOfDayState = nState;
        if (pCurrentArea != NULL)
        {
            pCurrentArea->UpdateSoundObjectsTimeOfDay();
            m_pInfo->m_pAmbientSound->UpdateTimeOfDay();
        }
    }
    nHours = m_nTimeOfDayHour;
    m_fTimeOfDay = nTime * 0.001f;
    m_fTimeOfDayHours = nHours * 60.0f;
}
// PC 0x00640700  Xbox 0x00063690
void CSWCModule::UpdateCameraModeChaseCam()
{
    CAurBehavior* pBehavior;
    CAurBehaviorCameraFollow* pFollow;
    CAurBehaviorCameraFollow* pFollowMode;
    CAurScene* pScene;
    CSWCCreature* pPlayer;
    float fNear;
    float fFar;
    float fCameraYaw;
    float fDelta;

    pBehavior = m_pModuleCamera->GetBehavior(-1);
    if (pBehavior != 0)
    {
        pFollow = pBehavior->AsBehaviorCameraFollow();
    }
    else
    {
        pFollow = NULL;
    }
    pScene = NULL;
    if (m_pInfo != NULL)
    {
        pScene = m_pInfo->m_pScene;
    }
    pPlayer = g_pAppManager->m_pClientExoApp->GetPlayerCreature();
    if (pScene != NULL && pPlayer != NULL)
    {
        if (m_bCameraModeChanged != 0)
        {
            pBehavior = m_pModuleCamera->GetBehavior(-1);
            if (pBehavior != 0)
            {
                pFollowMode = pBehavior->AsBehaviorCameraFollow();
                if (pFollowMode != NULL)
                {
                    pFollowMode->m_fMinDistance = 5.0f;
                    pFollowMode->m_fMaxDistance = 20.0f;
                    pFollowMode->m_fMinPitch = 1.0f;
                    pFollowMode->m_fMaxPitch = 50.0f;
                    g_fMinDist = 5.0f;
                    g_fMaxDist = 20.0f;
                    g_fMinPitch = 1.0f;
                    g_fMaxPitch = 50.0f;
                    m_vCameraOffset = Vector(0.0f, 0.0f, 1.0f);
                    pFollowMode->m_vOffset = m_vCameraOffset;
                    m_pModuleCamera->GetClipDist(fNear, fFar);
                    m_pModuleCamera->SetClipDist(fNear, 45.0f);
                    pFollowMode->m_fPitch = 50.0f;
                    pFollowMode->m_fDistance = 20.0f;
                    m_bCameraModeChanged = 0;
                }
            }
        }
        if (pPlayer->GetCurrentAnimation() == 10002 ||
            pPlayer->GetCurrentAnimation() == 10003 ||
            pPlayer->GetCurrentAnimation() == 10004 ||
            pPlayer->GetCurrentAnimation() == 10078 ||
            pPlayer->GetCurrentAnimation() == 10079 ||
            pPlayer->GetCurrentAnimation() == 10084 ||
            pPlayer->GetCurrentAnimation() == 10085 ||
            pPlayer->GetCurrentAnimation() == 10086 ||
            pPlayer->GetCurrentAnimation() == 10087)
        {
            if (m_nReservedFC != 0)
            {
                m_nReserved88 = 0;
                m_nReservedFC = 0;
            }
        }
        else
        {
            m_vPlayerPosition = pPlayer->m_vPosition;
            m_nReservedFC = 1;
        }
        if (m_nReserved80 != 0)
        {
            m_nReserved88 = 1;
            m_nReserved84 = 0;
        }
        if (m_nReserved88 == 0 && m_nReserved80 == 0)
        {
            fCameraYaw = Yaw(m_pModuleCamera->GetOrientation());
            fDelta = Yaw(pPlayer->m_vOrientation) - fCameraYaw;
            while (fDelta < -180.0f)
            {
                fDelta += 360.0f;
            }
            while (fDelta > 180.0f)
            {
                fDelta -= 360.0f;
            }
            if (pPlayer->m_nCombatState == 0)
            {
                if (fDelta > 90.0f && m_nReserved90 == CAMERA_TURN_PREFER_NEGATIVE)
                {
                    fDelta = -90.0f;
                }
                else if (fDelta < -90.0f && m_nReserved90 == CAMERA_TURN_PREFER_POSITIVE)
                {
                    fDelta = 90.0f;
                }
            }
            if (fabs(fDelta) > 1.0)
            {
                if (fabs(fDelta) > camerafollowangle &&
                        m_nReserved84 == 0 ||
                    m_nReserved84 != 0)
                {
                    if (fDelta > 90.0f)
                    {
                        fDelta = 90.0f;
                    }
                    else if (fDelta < -90.0f)
                    {
                        fDelta = -90.0f;
                    }
                    if (pFollow != NULL)
                    {
                        pFollow->m_fTurn = camerafollowspeedfactor * fDelta;
                    }
                    m_nReserved80 = 1;
                    m_nReserved84 = 1;
                }
            }
            else
            {
                m_nReserved84 = 0;
            }
        }
    }
}
// PC 0x00640A80  Xbox 0x00063A20
void CSWCModule::UpdateCameraModeBG()
{
    CAurBehavior* pBehavior;
    CAurBehaviorCameraFollow* pFollow;
    float fNear;
    float fFar;

    if (m_bCameraModeChanged != 0)
    {
        pBehavior = m_pModuleCamera->GetBehavior(-1);
        if (pBehavior != 0)
        {
            pFollow = pBehavior->AsBehaviorCameraFollow();
            if (pFollow != NULL)
            {
                pFollow->m_fMinDistance = 5.0f;
                pFollow->m_fMaxDistance = 20.0f;
                pFollow->m_fMinPitch = 1.0f;
                pFollow->m_fMaxPitch = 50.0f;
                g_fMinDist = 5.0f;
                g_fMaxDist = 20.0f;
                g_fMinPitch = 1.0f;
                g_fMaxPitch = 50.0f;
                m_vCameraOffset = Vector(0.0f, 0.0f, 1.0f);
                pFollow->m_vOffset = m_vCameraOffset;
                m_pModuleCamera->GetClipDist(fNear, fFar);
                m_pModuleCamera->SetClipDist(fNear, 45.0f);
                pFollow->m_fDistance = 20.0f;
                pFollow->m_fPitch = 50.0f;
                m_bCameraModeChanged = 0;
            }
        }
    }
}
// PC 0x00640B90  Xbox 0x00063B40
void CSWCModule::UpdateCameraModeEQ(float fTimeStep)
{
    CAurScene* pScene = NULL;
    if (m_pInfo != NULL)
    {
        pScene = m_pInfo->m_pScene;
    }
    CSWCCreature* pPlayer =
        g_pAppManager->GetClientExoApp()->GetPlayerCreature();
    if (pScene != NULL && pPlayer != NULL)
    {
        CAurBehavior* pBehavior = m_pModuleCamera->GetBehavior(-1);
        CAurBehaviorCameraFollow* pFollow;
        if (pBehavior != NULL)
        {
            pFollow = pBehavior->AsBehaviorCameraFollow();
        }
        else
        {
            pFollow = NULL;
        }
        if (m_bCameraModeChanged != 0 && pFollow != NULL)
        {
            pFollow->m_fMinDistance = 6.0f;
            pFollow->m_fMaxDistance = 6.0f;
            pFollow->m_fMinPitch = 1.0f;
            pFollow->m_fMaxPitch = 62.0f;
            g_fMinDist = 6.0f;
            g_fMaxDist = 6.0f;
            g_fMinPitch = 1.0f;
            g_fMaxPitch = 62.0f;
            m_vCameraOffset = Vector(0.0f, 0.0f, 2.0f);
            pFollow->m_vOffset = m_vCameraOffset;
            float fNear;
            float fFar;
            m_pModuleCamera->GetClipDist(fNear, fFar);
            m_pModuleCamera->SetClipDist(fNear, 45.0f);
            pFollow->m_fDistance = 6.0f;
            pFollow->m_fPitch = 62.0f;
            m_bCameraModeChanged = 0;
        }
        if (pPlayer->GetCurrentAnimation() == 10002 ||
            pPlayer->GetCurrentAnimation() == 10003 ||
            pPlayer->GetCurrentAnimation() == 10004 ||
            pPlayer->GetCurrentAnimation() == 10078 ||
            pPlayer->GetCurrentAnimation() == 10079 ||
            pPlayer->GetCurrentAnimation() == 10084 ||
            pPlayer->GetCurrentAnimation() == 10085 ||
            pPlayer->GetCurrentAnimation() == 10086 ||
            pPlayer->GetCurrentAnimation() == 10087)
        {
            if (m_nReservedFC != 0)
            {
                m_nReserved88 = 0;
                m_nReservedFC = 0;
            }
        }
        else
        {
            m_vPlayerPosition = pPlayer->m_vPosition;
            m_nReservedFC = 1;
        }
        if (m_nReserved80 != 0)
        {
            m_nReserved88 = 1;
            m_nReserved84 = 0;
        }
        if (m_nReserved88 == 0 && m_nReserved80 == 0)
        {
            float fCameraYaw = m_pModuleCamera->GetYaw();
            float fPlayerYaw = Yaw(pPlayer->m_vOrientation);
            float fDelta = fPlayerYaw - fCameraYaw;
            while (fDelta < -180.0f)
            {
                fDelta += 360.0f;
            }
            while (fDelta > 180.0f)
            {
                fDelta -= 360.0f;
            }
            if (pPlayer->m_nCombatState == 0)
            {
                if (fDelta > 90.0f &&
                    m_nReserved90 == CAMERA_TURN_PREFER_NEGATIVE)
                {
                    fDelta = -90.0f;
                }
                else if (fDelta < -90.0f &&
                         m_nReserved90 == CAMERA_TURN_PREFER_POSITIVE)
                {
                    fDelta = 90.0f;
                }
            }
            if (fabs(fDelta) > 1.0)
            {
                float fTurn = fDelta * 5.0f;
                if (fTurn > 180.0f)
                {
                    fTurn = 180.0f;
                }
                else if (fTurn < -180.0f)
                {
                    fTurn = -180.0f;
                }
                float fRemainingDelta =
                    fPlayerYaw - (fTimeStep * fTurn + fCameraYaw);
                while (fRemainingDelta < -180.0f)
                {
                    fRemainingDelta += 360.0f;
                }
                while (fRemainingDelta > 180.0f)
                {
                    fRemainingDelta -= 360.0f;
                }
                if (fTurn > 0.0001f && fRemainingDelta < 0.0001f ||
                    fTurn < 0.0001f && fRemainingDelta > 0.0001f)
                {
                    fTurn = (fPlayerYaw - fCameraYaw) / fTimeStep;
                }
                if (pFollow != NULL)
                {
                    pFollow->m_fTurn = camerafollowspeedfactor * fTurn;
                }
                m_nReserved80 = 1;
                m_nReserved84 = 1;
            }
            else
            {
                m_nReserved84 = 0;
            }
        }
    }
}
// PC 0x00640F70  Xbox 0x00063EE0
void CSWCModule::LoadAnimatedCamera(const CResRef& cAnimatedCamera)
{
    if (m_pAnimatedCamera == NULL)
    {
        if (cAnimatedCamera != "")
        {
            CExoString sModel;

            cAnimatedCamera.CopyToString(sModel);
            m_pAnimatedCamera = static_cast<Gob*>(
                NewCAurObject((char*)sModel.CStr(), "", 0, 0));
            EXOXBOXASSERT(m_pAnimatedCamera);
        }
    }
}
// PC 0x00641010  Xbox 0x00063F90
float CSWCModule::SetAnimatedCamera(unsigned short nAnimation,
                                    float fFieldOfView, unsigned long oidTarget)
{
    unsigned char nCameraMode;
    CAurCamera* pCamera;
    CAurScene* pScene;
    CClientExoApp* pApp;
    CSWCameraOnAStick* pOnAStick;
    CSWCObject* pObject;
    CSWCObject* pTarget;
    CGuiInGame* pGuiInGame;
    float fTime;
    float fLength;

    if (oidTarget != OBJECT_INVALID && m_pAnimatedCamera != NULL)
    {
        nCameraMode =
            g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nCameraMode;
        if (nCameraMode == 3)
        {
            pOnAStick = reinterpret_cast<CSWCameraOnAStick*>(
                m_pModuleCamera->GetBehavior(AUR_BEHAVIOR_CAMERA_ONASTICK));
            if (pOnAStick != NULL)
            {
                pOnAStick->SaveCameraSettings(m_cameraSettings);
            }
        }
        pCamera = m_pModuleCamera;
        pCamera->SetClipDist(0.1f, 10000.0f);
        pScene = m_pInfo->m_pScene;
        m_pAnimatedCamera->AttachToScene((Scene*)pScene);
        pObject = g_pAppManager->m_pClientExoApp->GetGameObject(oidTarget);
        if (pObject != NULL)
        {
            if (pObject->AsSWCObject() != NULL)
            {
                pTarget = pObject->AsSWCObject();
                Vector vPosition;
                Quaternion qOrientation;
                m_pAnimatedCamera->SetPosition(vPosition);
                m_pAnimatedCamera->SetOrientation(qOrientation);
                pCamera->AttachToObject(m_pAnimatedCamera, "camerahook", Gob::ATTACH_NORMAL);
                pGuiInGame = g_pAppManager->m_pClientExoApp->GetInGameGui();
                m_sAnimatedCamera =
                    pGuiInGame->GetCameraAnimationName(nAnimation);
                m_pAnimatedCamera->EnableAlwaysRender();
                m_pAnimatedCamera->EnableInCutscene();
                m_pAnimatedCamera->GetAnimation((char*)m_sAnimatedCamera.CStr(),
                                                &fTime, &fLength);
                if (fLength <= 0.0f)
                {
                    fLength = 0.0f;
                }
                if (pGuiInGame->IsLoopingDialogStuntAnimation(nAnimation) != 0)
                {
                    m_pAnimatedCamera->PlayAnimation(m_sAnimatedCamera.CStr(),
                                                     1.0f, 2, 0.0f);
                }
                else
                {
                    m_pAnimatedCamera->PlayAnimation(m_sAnimatedCamera.CStr(),
                                                     1.0f, 3, 0.0f);
                }
                if (fFieldOfView > 0.0f)
                {
                    pApp = g_pAppManager->m_pClientExoApp;
                    pApp->GetInGameGui()
                        ->SetDeferredFOVCallForAnimatedCutscenes(fFieldOfView);
                }
                return fLength;
            }
        }
    }
    return 0.0f;
}
// PC 0x006412B0  Xbox 0x00064230
int CSWCModule::IsCameraAnimated()
{
    if (m_pAnimatedCamera != NULL)
    {
        return m_pAnimatedCamera->GetAnimation((char*)m_sAnimatedCamera.CStr(),
                                               NULL, NULL) != 0;
    }
    return 0;
}
// PC 0x006412F0  Xbox 0x00064270
void CSWCModule::SetDialogCamera(int nDialogType)
{
    unsigned char nCameraMode;
    CSWCameraOnAStick* pOnAStick;
    CSWBehaviorCameraDialog* pDialog;

    nCameraMode =
        g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nCameraMode;
    if (nCameraMode == 3)
    {
        pOnAStick = reinterpret_cast<CSWCameraOnAStick*>(
            m_pModuleCamera->GetBehavior(AUR_BEHAVIOR_CAMERA_ONASTICK));
        if (pOnAStick != NULL)
        {
            pOnAStick->SaveCameraSettings(m_cameraSettings);
        }
    }
    pDialog = new CSWBehaviorCameraDialog(m_pModuleCamera->AsCAurObject(),
                                          nDialogType);
    g_pAppManager->m_pClientExoApp->GetClientOptions()->SetCameraMode(CClientOptions::CAMERA_MODE_DIALOG);
    m_pModuleCamera->SetBehavior(pDialog);
}
// PC 0x006413C0  Xbox 0x00064340
int CSWCModule::SetFreeLookCamera(unsigned long oidTarget)
{
    CSWCObject* pObject;
    CSWCCreature* pCreature;
    CSWCameraOnAStick* pOnAStick;
    CSWCameraFreeLook* pFreeLook;
    CClientExoApp* pClientExoApp;

    pClientExoApp = g_pAppManager->m_pClientExoApp;
    pObject = pClientExoApp->GetGameObject(oidTarget);
    if (pObject == NULL)
    {
        return 0;
    }
    pCreature = pObject->AsSWCCreature();
    if (pCreature == NULL)
    {
        return 0;
    }
    if (g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nCameraMode == CClientOptions::CAMERA_MODE_ON_A_STICK)
    {
        pOnAStick = reinterpret_cast<CSWCameraOnAStick*>(
            m_pModuleCamera->GetBehavior(AUR_BEHAVIOR_CAMERA_ONASTICK));
        if (pOnAStick != NULL)
        {
            pOnAStick->SaveCameraSettings(m_cameraSettings);
            Vector vDirection =
                pCreature->m_vPosition - pOnAStick->GetPosition();
            vDirection.m_fZ = 0.0f;
            pCreature->SetOrientation(
                normalize(vDirection));
        }
    }
    pFreeLook = new CSWCameraFreeLook(m_pModuleCamera->AsCAurObject(),
                                      pCreature->GetCameraFollowPart(),
                                      pCreature->GetFreeLookVideoEffect());
    g_pAppManager->m_pClientExoApp->GetClientOptions()->SetCameraMode(5);
    m_pModuleCamera->SetBehavior(pFreeLook);
    m_nCameraType = 5;
    return 1;
}
// PC 0x00641540  Xbox 0x000644C0
int CSWCModule::SetCombatCamera()
{
    unsigned char nCameraMode;
    CSWCameraOnAStick* pOnAStick;
    CSWBehaviorCameraCombat* pCombat;

    nCameraMode =
        g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nCameraMode;
    if (nCameraMode == 3)
    {
        pOnAStick = reinterpret_cast<CSWCameraOnAStick*>(
            m_pModuleCamera->GetBehavior(AUR_BEHAVIOR_CAMERA_ONASTICK));
        if (pOnAStick != NULL)
        {
            pOnAStick->SaveCameraSettings(m_cameraSettings);
        }
    }
    pCombat = new CSWBehaviorCameraCombat(m_pModuleCamera->AsCAurObject());
    g_pAppManager->m_pClientExoApp->GetClientOptions()->SetCameraMode(CClientOptions::CAMERA_MODE_COMBAT_FACING);
    m_pModuleCamera->SetBehavior(pCombat);
    return 1;
}
// PC 0x00641610  Xbox 0x00064580
int CSWCModule::SetNavigateCamera()
{
    unsigned char nCameraMode;
    CSWCameraOnAStick* pOnAStick;
    CSWCameraNavigate* pNavigate;

    nCameraMode =
        g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nCameraMode;
    if (nCameraMode == 3)
    {
        pOnAStick = reinterpret_cast<CSWCameraOnAStick*>(
            m_pModuleCamera->GetBehavior(AUR_BEHAVIOR_CAMERA_ONASTICK));
        if (pOnAStick != NULL)
        {
            pOnAStick->SaveCameraSettings(m_cameraSettings);
        }
        else
        {
            return 0;
        }
    }
    pNavigate = new CSWCameraNavigate(m_pModuleCamera->AsCAurObject());
    g_pAppManager->m_pClientExoApp->GetClientOptions()->SetCameraMode(CClientOptions::CAMERA_MODE_NAVIGATE);
    m_pModuleCamera->SetBehavior(pNavigate);
    return 1;
}
// PC 0x006416F0  Xbox 0x00064660
int CSWCModule::ShakeCamera(unsigned long nDelay, unsigned long nDuration)
{
    int nResult;

    nResult = nDuration;
    if (nDuration != 0 && (int)nDuration >= m_nCameraShakeTime)
    {
        m_nCameraShakeTime = nDuration;
        m_nCameraShakeLength = nDuration;
        m_nCameraShakeDelay = nDelay;
        m_nCameraShakeAmount = 70;
        m_bCameraShakeChanged = 1;
        return nDelay;
    }
    return nResult;
}
// PC 0x00641730  Xbox 0x000646A0
int CSWCModule::BumpCamera(unsigned long nDelay)
{
    int nResult;

    nResult = m_nCameraBumpTime;
    if (nResult == 0)
    {
        m_nCameraBumpTime = 50;
        m_nCameraBumpDelay = nDelay;
        m_bCameraBumpChanged = 1;
        return nDelay;
    }
    return nResult;
}
// PC 0x00641760  Xbox 0x000646D0
void CSWCModule::UpdateCameraBumpAndShake(float fTimeDelta)
{
    CSWCameraOnAStick* pOnAStick;
    CSWBehaviorCameraDialog* pDialog;
    CSWCameraFreeLook* pFreeLook;
    CSWBehaviorCameraCombat* pCombat;
    int nElapsed;
    int nRemaining;
    float fYaw;
    float fPitch;
    float fShake;

    pOnAStick = reinterpret_cast<CSWCameraOnAStick*>(
        m_pModuleCamera->GetBehavior(AUR_BEHAVIOR_CAMERA_ONASTICK));
    pDialog = reinterpret_cast<CSWBehaviorCameraDialog*>(
        m_pModuleCamera->GetBehavior(AUR_BEHAVIOR_CAMERA_DIALOG));
    pFreeLook = reinterpret_cast<CSWCameraFreeLook*>(
        m_pModuleCamera->GetBehavior(AUR_BEHAVIOR_CAMERA_FREELOOK));
    pCombat = reinterpret_cast<CSWBehaviorCameraCombat*>(
        m_pModuleCamera->GetBehavior(AUR_BEHAVIOR_CAMERA_COMBAT));
    if (pOnAStick != NULL || pDialog != NULL || pFreeLook != NULL ||
        pCombat != NULL)
    {
        if (g_pAppManager->m_pClientExoApp->GetClientOptions()
                ->m_bVideoOption == 0)
        {
            if (m_nCameraBumpTime > 0)
            {
                m_nCameraBumpTime = 0;
                m_nCameraBumpDelay = 0;
                m_bCameraBumpChanged = 1;
            }
            else if (m_nCameraShakeTime > 0)
            {
                m_nCameraShakeTime = 0;
                m_nCameraShakeDelay = 0;
                m_nCameraShakeAmount = 0;
                m_bCameraShakeChanged = 1;
            }
        }
        nElapsed = (int)(fTimeDelta * 1000.0f);
        nRemaining = m_nCameraShakeTime;
        if (nRemaining > 0)
        {
            if (m_nCameraShakeDelay > 0)
            {
                if (m_nCameraShakeDelay <= nElapsed)
                {
                    nElapsed -= m_nCameraShakeDelay;
                    m_nCameraShakeDelay = 0;
                }
                else
                {
                    m_nCameraShakeDelay -= nElapsed;
                }
            }
            if (m_nCameraShakeDelay == 0)
            {
                if (nRemaining <= nElapsed)
                {
                    fPitch = 0.0f;
                    m_nCameraShakeTime = 0;
                    fYaw = 0.0f;
                }
                else
                {
                    float fCosine = (float)cos(nRemaining * 6.2831852 /
                                               m_nCameraShakeLength) +
                                    1.0f;
                    float fSine = (float)sin(nRemaining * 6.2831852 /
                                             fShakePeriod);
                    fShake = fCosine * fSine * fShakeMag *
                             DEG_TO_RAD;
                    fYaw = ((rand() + 1) % 2 - 1) * fShake;
                    fPitch = ((rand() + 1) % 2 - 1) * fShake;
                }
                if (pOnAStick != NULL)
                {
                    pOnAStick->m_fShakePitch = fPitch;
                    pOnAStick->m_fShakeYaw = fYaw;
                }
                else if (pCombat != NULL)
                {
                    pCombat->m_fPitchOffset = fPitch;
                    pCombat->m_fYawOffset = fYaw;
                }
                else if (pDialog != NULL)
                {
                    pDialog->m_fPitchOffset = fPitch;
                    pDialog->m_fYawOffset = fYaw;
                }
                else if (pFreeLook != NULL)
                {
                    pFreeLook->m_fPitchOffset = fPitch;
                    pFreeLook->m_fYawOffset = fYaw;
                }
                m_nCameraShakeTime -= nElapsed;
            }
        }
    }
}
// PC 0x00641980  Xbox 0x000648F0
CSWMiniGame* CSWCModule::GetMiniGame()
{
    CSWCArea* pArea;

    pArea = m_pInfo;
    if (pArea != NULL)
    {
        return pArea->m_pMiniGame;
    }
    return 0;
}
// PC 0x006419A0  Xbox 0x00064910
void CSWCModule::UpdateCameraFollow(float fTimeDelta)
{
    int nCameraMode =
        g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nCameraMode;
    if (nCameraMode == CClientOptions::CAMERA_MODE_DIALOG ||
        m_bCameraCollisionChanged != 0 || m_pModuleCamera == NULL)
    {
        return;
    }

    if (nCameraMode != m_nCameraType)
    {
        CClientExoApp* pApp = g_pAppManager->m_pClientExoApp;
        CSWCObject* pObject = pApp->GetGameObject(m_oidCameraTarget);
        if (pObject == NULL)
        {
            return;
        }
        CSWCObject* pFollowed = pObject->AsSWCObject();
        if (pFollowed == NULL)
        {
            return;
        }
        Gob* pModel = pFollowed->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1);
        if (pModel == NULL)
        {
            return;
        }

        if (m_nCameraType == CAMERA_ON_A_STICK)
        {
            m_pModuleCamera->SetBehavior(NULL);
        }
        switch (nCameraMode)
        {
        case CAMERA_ON_A_STICK:
        {
            CSWCCreature* pCreature = pObject->AsSWCCreature();
            CSWCameraOnAStick* pOnAStick =
                new CSWCameraOnAStick(m_pModuleCamera->AsCAurObject(),
                                     pCreature->GetCameraFollowPart());
            pOnAStick->m_nHeightMode = pCreature->GetFollowingBody();
            pOnAStick->m_fPitch =
                pCreature->GetHeadHeightOffset() + pCreature->GetCameraOffset();
            m_pModuleCamera->SetBehavior(pOnAStick);
            break;
        }
        case CAMERA_FREE_LOOK:
        {
            CSWCCreature* pCreature = pObject->AsSWCCreature();
            CSWCameraFreeLook* pFreeLook =
                new CSWCameraFreeLook(m_pModuleCamera->AsCAurObject(),
                                     pCreature->GetCameraFollowPart(),
                                     pCreature->GetFreeLookVideoEffect());
            m_pModuleCamera->SetBehavior(pFreeLook);
            break;
        }
        case CAMERA_NAVIGATE:
        {
            CSWCameraNavigate* pNavigate =
                new CSWCameraNavigate(m_pModuleCamera->AsCAurObject());
            m_pModuleCamera->SetBehavior(pNavigate);
            break;
        }
        default:
            m_pModuleCamera->SetFollow(pModel);
            break;
        }
        m_nCameraType = nCameraMode;
    }

    if (m_nLastCameraMode !=
        g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nCameraMode)
    {
        m_bCameraModeChanged = 1;
    }
    switch (g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nCameraMode)
    {
    case CClientOptions::CAMERA_MODE_CHASE:
        UpdateCameraModeChaseCam();
        break;
    case CClientOptions::CAMERA_MODE_BG:
        UpdateCameraModeBG();
        break;
    case CClientOptions::CAMERA_MODE_EQ:
        UpdateCameraModeEQ(fTimeDelta);
        break;
    case CAMERA_ON_A_STICK:
    case CClientOptions::CAMERA_MODE_DIALOG:
        return;
    }

    if (g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nCameraMode ==
            CClientOptions::CAMERA_MODE_CHASE ||
        g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nCameraMode ==
            CClientOptions::CAMERA_MODE_BG)
    {
        CAurBehavior* pBehavior = reinterpret_cast<CAurBehavior*>(
            m_pModuleCamera->GetBehavior(-1));
        if (pBehavior != NULL)
        {
            pBehavior->AsBehaviorCameraFollow();
        }
    }
    m_fCameraDistance = m_pModuleCamera->GetDist();
    m_fCameraYaw = m_pModuleCamera->GetYaw();
    m_fCameraPitch = m_pModuleCamera->GetPitch();
    m_nLastCameraMode =
        g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nCameraMode;
}
// PC 0x00641CD0  Xbox 0x00064CA0
CSWCModule::~CSWCModule()
{
    m_sModuleTitle = "";
    if (m_pInfo != NULL)
    {
        m_pInfo->UnloadArea();
        delete m_pInfo;
        m_pInfo = NULL;
    }
    if (m_pModuleCamera != NULL)
    {
        delete m_pModuleCamera;
        m_pModuleCamera = NULL;
    }
    if (m_pAnimatedCamera != NULL)
    {
        delete m_pAnimatedCamera;
        m_pAnimatedCamera = NULL;
    }
}
// PC 0x00641DC0  Xbox 0x00064D80
int CSWCModule::RestoreCamera()
{
    if (g_pAppManager->GetClientExoApp()->GetCombatMode())
    {
        SetCombatCamera();
        CSWBehaviorCameraCombat* pCombat =
            reinterpret_cast<CSWBehaviorCameraCombat*>(
                m_pModuleCamera->GetBehavior(AUR_BEHAVIOR_CAMERA_COMBAT));
        if (pCombat != NULL)
        {
            pCombat->PutAtDesiredLocation();
        }
        return 1;
    }
    if (!m_cameraSettings.m_bValid)
    {
        g_pAppManager->GetClientExoApp()->GetClientOptions()->SetCameraMode(
            CAMERA_ON_A_STICK);
        return 0;
    }

    Vector vPosition;
    Quaternion qOrientation;
    if (m_pModuleCamera->GetBehavior(-1) != NULL)
    {
        vPosition = static_cast<Gob*>(
            m_pModuleCamera->GetBehavior(-1)->m_pObject)->GetPosition();
        qOrientation = static_cast<Gob*>(
            m_pModuleCamera->GetBehavior(-1)->m_pObject)->GetOrientation();
    }
    int bResetOrientation = 0;
    int bRestorePosition = 0;
    if (m_pModuleCamera->GetBehavior(-1) != NULL &&
        m_pModuleCamera->GetBehavior(-1)->GetType() == AUR_BEHAVIOR_CAMERA_DIALOG)
    {
        bResetOrientation = 1;
    }
    if (m_pModuleCamera->GetBehavior(-1) != NULL &&
        m_pModuleCamera->GetBehavior(-1)->GetType() == AUR_BEHAVIOR_CAMERA_FREELOOK)
    {
        bResetOrientation = 1;
    }
    if (m_pModuleCamera->GetBehavior(-1) != NULL &&
        m_pModuleCamera->GetBehavior(-1)->GetType() == AUR_BEHAVIOR_CAMERA_COMBAT)
    {
        bRestorePosition = 1;
    }

    if ((m_nCameraType == CAMERA_ON_A_STICK ||
         m_nCameraType == CAMERA_FREE_LOOK ||
         m_nCameraType == CAMERA_NAVIGATE) && m_cameraSettings.m_bValid)
    {
        if (m_cameraSettings.m_pTarget == NULL)
        {
            CSWCCreature* pCreature =
                g_pAppManager->GetClientExoApp()->GetSWParty()->GetCharacter(0);
            if (pCreature != NULL)
            {
                switch (m_cameraSettings.m_nCameraType)
                {
                case CCameraSettings::SAVED_ON_A_STICK:
                    {
                        CSWCameraOnAStick* pCamera = new CSWCameraOnAStick(
                            m_pModuleCamera->AsCAurObject(),
                            pCreature->GetCameraFollowPart());
                        pCamera->m_nHeightMode = pCreature->GetFollowingBody();
                        pCamera->m_fPitch = pCreature->GetCameraOffset() +
                            pCreature->GetHeadHeightOffset();
                        if (bRestorePosition)
                        {
                            pCamera->ResetLastPosition(vPosition, qOrientation);
                        }
                        pCamera->m_bResetToTargetOrientation = bResetOrientation;
                        m_pModuleCamera->SetBehavior(pCamera);
                    }
                    break;
                case CCameraSettings::SAVED_FREE_LOOK:
                    m_pModuleCamera->SetBehavior(new CSWCameraFreeLook(
                        m_pModuleCamera->AsCAurObject(),
                        pCreature->GetCameraFollowPart(),
                        pCreature->GetFreeLookVideoEffect()));
                    break;
                }
            }
        }
        else
        {
            switch (m_cameraSettings.m_nCameraType)
            {
            case CCameraSettings::SAVED_ON_A_STICK:
                {
                    CSWCCreature* pCreature =
                        g_pAppManager->GetClientExoApp()->GetSWParty()->GetCharacter(0);
                    if (pCreature != NULL)
                    {
                        m_cameraSettings.m_pTarget = pCreature->GetCameraFollowPart();
                        m_cameraSettings.m_nHeightMode = pCreature->GetFollowingBody();
                        m_cameraSettings.m_fPitch = pCreature->GetCameraOffset() +
                            pCreature->GetHeadHeightOffset();
                    }
                    CSWCameraOnAStick* pCamera = new CSWCameraOnAStick(m_cameraSettings);
                    if (bRestorePosition)
                    {
                        pCamera->ResetLastPosition(vPosition, qOrientation);
                    }
                    pCamera->m_bResetToTargetOrientation = bResetOrientation;
                    m_pModuleCamera->SetBehavior(pCamera);
                    m_nCameraType = CAMERA_ON_A_STICK;
                }
                break;
            case CCameraSettings::SAVED_FREE_LOOK:
                m_pModuleCamera->SetBehavior(new CSWCameraFreeLook(m_cameraSettings));
                m_nCameraType = CAMERA_FREE_LOOK;
                break;
            }
        }
        if (m_pAnimatedCamera != NULL)
        {
            delete m_pAnimatedCamera;
            m_pAnimatedCamera = NULL;
        }
        g_pAppManager->GetClientExoApp()->GetClientOptions()->SetCameraMode(
            m_nCameraType);
        return 1;
    }
    EXOXBOXASSERT(0);
    return 0;
}
// PC 0x006422E0  Xbox 0x000652B0
float CSWCModule::UpdateNormalCameraCollision(float fTimeDelta, float fZoom)
{
    CAurBehavior* pBehavior = m_pModuleCamera->GetBehavior(-1);
    CAurBehaviorCameraFollow* pFollow;
    if (pBehavior != NULL)
    {
        pFollow = pBehavior->AsBehaviorCameraFollow();
    }
    else
    {
        pFollow = NULL;
    }
    if (pFollow == NULL)
    {
        return 0.0f;
    }

    CAurScene* pScene = NULL;
    if (m_pInfo != NULL)
    {
        pScene = m_pInfo->m_pScene;
    }
    CSWCCreature* pCreature =
        g_pAppManager->GetClientExoApp()->GetPlayerCreature();
    if (enablecameracollision && m_pModuleCamera != NULL &&
        pScene != NULL && pCreature != NULL &&
        pCreature->IsModelLoaded())
    {
        Vector vInitialCamera;
        vInitialCamera = m_pModuleCamera->GetPosition();
        Vector vTarget;
        vTarget = pCreature->m_vPosition;
        vTarget = vTarget + m_vCameraOffset;
        Vector vDifference;
        vDifference = vTarget - vInitialCamera;
        if (vDifference * vDifference > 0.0f)
        {
            CHitInfo hitInfo(0, CHitInfo::HIT_ALL_NON_TRIGGER, 0);
            float fNewZoom = fZoom;
            Vector vHit;
            Vector vReverseHit;
            int bReverseMiss = 0;
            int bClampDistance = 1;
            float fCollisionDistance = 0.0f;
            int bReturning = 0;
            Vector vDirection;
            vDirection = normalize(vTarget - vInitialCamera);
            Vector vMaxPosition;
            vMaxPosition = vTarget - vDirection * g_fMaxDist;

            static float fOldDist = 0.0f;
            static Vector vLastPos;
            Quaternion qDesired;
            if (vLastPos.m_fX == 0.0f && vLastPos.m_fY == 0.0f &&
                vLastPos.m_fZ == 0.0f)
            {
                vLastPos = pCreature->m_vPosition;
            }
            float fYaw = m_pModuleCamera->GetYaw() + fTimeDelta * m_fCameraTurn;
            float fPitch = m_pModuleCamera->GetPitch() + fTimeDelta * m_fCameraTilt;
            float fDistance = m_pModuleCamera->GetDist() *
                              powf(2.0f, fTimeDelta * fZoom * -0.5f);
            fPitch = max(g_fMinPitch, min(fPitch, g_fMaxPitch));
            fDistance = max(g_fMinDist, min(fDistance, g_fMaxDist));
            float fSpring = 1.0f - powf(0.5f, fTimeDelta * 750.0f);
            Vector vCamera;
            vCamera = pCreature->m_vPosition + Vector(0.0f, 0.0f, 1.0f);
            vCamera = vLastPos * (1.0f - fSpring) + vCamera * fSpring;
            qDesired = YawPitchRoll(fYaw, fPitch, 0.0f);
            Quaternion qOrientation;
            qOrientation = normalize(slerp(m_pModuleCamera->GetOrientation(), qDesired, fSpring));
            Vector vNext;
            vNext = vCamera + qOrientation.zdir() * fDistance;
            vLastPos = vCamera;

            do
            {
                if (fZoom < 0.0f)
                {
                    hitInfo.Reset(0, CHitInfo::HIT_ALL_NON_TRIGGER, 0);
                    hitInfo.m_nType = CHitInfo::HIT_TYPE_VISUAL_EFFECT_GROUND;
                    int bHit;
                    if (useclearlineofsight)
                        bHit = !m_pInfo->ClearLineOfSightOneWay(vNext, vMaxPosition, &vHit);
                    else
                        bHit = pScene->HitCheck(vNext, vMaxPosition, &vHit, &hitInfo);
                    if (bHit)
                    {
                        vHit = vHit + vDirection * CAMERA_COLLISION_RAY_EPSILON;
                        hitInfo.Reset(0, CHitInfo::HIT_ALL_NON_TRIGGER, 0);
                        hitInfo.m_nType = CHitInfo::HIT_TYPE_VISUAL_EFFECT_GROUND;
                        if (useclearlineofsight)
                            bHit = !m_pInfo->ClearLineOfSightOneWay(vHit, vNext, &vReverseHit);
                        else
                            bHit = pScene->HitCheck(vHit, vNext, &vReverseHit, &hitInfo);
                    }
                    else
                    {
                        hitInfo.Reset(0, CHitInfo::HIT_ALL_NON_TRIGGER, 0);
                        hitInfo.m_nType = CHitInfo::HIT_TYPE_VISUAL_EFFECT_GROUND;
                        if (useclearlineofsight)
                            bHit = !m_pInfo->ClearLineOfSightOneWay(vMaxPosition, vNext, &vReverseHit);
                        else
                            bHit = pScene->HitCheck(vMaxPosition, vNext, &vReverseHit, &hitInfo);
                    }
                    if (bHit)
                    {
                        vReverseHit = vReverseHit - vDirection * CAMERA_COLLISION_RAY_EPSILON;
                        fCollisionDistance = magnitude(vReverseHit - vTarget);
                        fOldDist = 0.0f;
                        break;
                    }
                    bReverseMiss = 1;
                }
                int bCanReturn = 0;
                if (!(fZoom > 0.0f))
                    bCanReturn = 1;
                hitInfo.Reset(0, CHitInfo::HIT_ALL_NON_TRIGGER, 0);
                hitInfo.m_nType = CHitInfo::HIT_TYPE_VISUAL_EFFECT_GROUND;
                int bHit;
                if (useclearlineofsight)
                    bHit = !m_pInfo->ClearLineOfSightOneWay(vNext, vTarget, &vHit);
                else
                    bHit = pScene->HitCheck(vNext, vTarget, &vHit, &hitInfo);
                if (bHit)
                {
                    vHit = vHit - vDirection * CAMERA_COLLISION_RAY_EPSILON;
                    hitInfo.Reset(0, CHitInfo::HIT_ALL_NON_TRIGGER, 0);
                    hitInfo.m_nType = CHitInfo::HIT_TYPE_VISUAL_EFFECT_GROUND;
                    if (useclearlineofsight)
                        bHit = !m_pInfo->ClearLineOfSightOneWay(vHit, vNext, &vReverseHit);
                    else
                        bHit = pScene->HitCheck(vHit, vNext, &vReverseHit, &hitInfo);
                }
                else
                {
                    hitInfo.Reset(0, CHitInfo::HIT_ALL_NON_TRIGGER, 0);
                    hitInfo.m_nType = CHitInfo::HIT_TYPE_VISUAL_EFFECT_GROUND;
                    if (useclearlineofsight)
                        bHit = !m_pInfo->ClearLineOfSightOneWay(vTarget, vNext, &vReverseHit);
                    else
                        bHit = pScene->HitCheck(vTarget, vNext, &vReverseHit, &hitInfo);
                }
                if (bHit)
                {
                    if (fOldDist == 0.0f)
                        fOldDist = m_pModuleCamera->GetDist();
                    if (bReverseMiss)
                        fNewZoom = 0.0f;
                    vReverseHit = vReverseHit + vDirection * CAMERA_COLLISION_RAY_EPSILON;
                    fCollisionDistance = magnitude(vReverseHit - vTarget);
                }
                else if (fOldDist > 0.0f)
                {
                    Vector vLimit;
                    vLimit = vTarget - vDirection * fOldDist;
                    hitInfo.Reset(0, CHitInfo::HIT_ALL_NON_TRIGGER, 0);
                    hitInfo.m_nType = CHitInfo::HIT_TYPE_VISUAL_EFFECT_GROUND;
                    if (m_pModuleCamera->GetDist() < fOldDist)
                    {
                        if (useclearlineofsight)
                            bHit = !m_pInfo->ClearLineOfSightOneWay(vNext, vLimit, &vHit);
                        else
                            bHit = pScene->HitCheck(vNext, vLimit, &vHit, &hitInfo);
                    }
                    else
                    {
                        if (useclearlineofsight)
                            bHit = !m_pInfo->ClearLineOfSightOneWay(vLimit, vNext, &vHit);
                        else
                            bHit = pScene->HitCheck(vLimit, vNext, &vHit, &hitInfo);
                    }
                    if (bHit)
                    {
                        vHit = vHit + vDirection * CAMERA_COLLISION_RAY_EPSILON;
                        fCollisionDistance = m_pModuleCamera->GetDist() +
                            (fOldDist - m_pModuleCamera->GetDist()) * fTimeDelta * 3.5f;
                        Vector vHitDifference = vHit - vTarget;
                        float fLengthSquared = vHitDifference * vHitDifference;
                        if (fCollisionDistance * fCollisionDistance >= fLengthSquared)
                            fCollisionDistance = sqrtf(fLengthSquared);
                    }
                    else
                    {
                        fCollisionDistance = m_pModuleCamera->GetDist() +
                            (fOldDist - m_pModuleCamera->GetDist()) * fTimeDelta * 3.5f;
                        bReturning = 1;
                        if (fCollisionDistance - fOldDist >= -CAMERA_COLLISION_DISTANCE_TOLERANCE)
                        {
                            fCollisionDistance = fOldDist;
                            fOldDist = 0.0f;
                        }
                    }
                }
                if (!bCanReturn)
                    fOldDist = 0.0f;
                if ((fCollisionDistance == 0.0f && m_pModuleCamera->GetDist() < g_fMinDist) ||
                    (fCollisionDistance > 0.0f && g_fMinDist - fCollisionDistance >= CAMERA_COLLISION_DISTANCE_TOLERANCE))
                {
                    bClampDistance = 0;
                    if (fOldDist < g_fMinDist)
                        fOldDist = g_fMinDist;
                    fNewZoom = 0.0f;
                }
            } while (0);
            if (lockcamera(NULL))
            {
                if (bClampDistance)
                    pFollow->m_fClampDistance = 1.0f;
                else
                    pFollow->m_fClampDistance = 0.0f;
                if (fCollisionDistance > 0.0f)
                {
                    pFollow->m_fDistance = fCollisionDistance;
                    if (!bReturning)
                        fNewZoom = 0.0f;
                    m_nReserved80 = 1;
                }
                if (m_pModuleCamera->GetDist() <= g_fMinDist && fNewZoom > 0.0f)
                    fNewZoom = 0.0f;
                if (m_pModuleCamera->GetDist() >= g_fMaxDist && fNewZoom < 0.0f)
                    fNewZoom = 0.0f;
                fZoom = fNewZoom;
            }
        }
    }
    return fZoom;
}
// PC 0x00644170  Xbox 0x00067400
float CSWCModule::UpdateCameraCollision(float fTimeDelta, float fUnknown)
{
    if (m_bCameraCollisionChanged != 0)
    {
        return fUnknown;
    }
    if (g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nCameraMode == CClientOptions::CAMERA_MODE_EQ)
    {
        return UpdatePushCameraCollision(fTimeDelta, fUnknown);
    }
    return UpdateNormalCameraCollision(fTimeDelta, fUnknown);
}
// PC 0x006433B0  Xbox 0x00066500
float CSWCModule::UpdatePushCameraCollision(float fTimeDelta, float fZoom)
{
    CAurBehavior* pBehavior = m_pModuleCamera->GetBehavior(-1);
    CAurBehaviorCameraFollow* pFollow;
    if (pBehavior != NULL)
    {
        pFollow = pBehavior->AsBehaviorCameraFollow();
    }
    else
    {
        pFollow = NULL;
    }
    if (pFollow == NULL)
    {
        return 0.0f;
    }

    CAurScene* pScene = NULL;
    if (m_pInfo != NULL)
    {
        pScene = m_pInfo->m_pScene;
    }
    CSWCCreature* pCreature =
        g_pAppManager->GetClientExoApp()->GetPlayerCreature();
    if (enablecameracollision && m_pModuleCamera != NULL &&
        pScene != NULL && pCreature != NULL &&
        pCreature->IsModelLoaded())
    {
        Vector vInitialCamera;
        vInitialCamera = m_pModuleCamera->GetPosition();
        Vector vTarget;
        vTarget = pCreature->m_vPosition;
        vTarget = vTarget + m_vCameraOffset;
        Vector vDifference;
        vDifference = vTarget - vInitialCamera;
        if (vDifference * vDifference > 0.0f)
        {
            CHitInfo hitInfo(0, CHitInfo::HIT_ALL_NON_TRIGGER, 0);
            float fNewZoom = fZoom;
            Vector vHit;
            int bClampDistance = 1;
            float fCollisionDistance = 0.0f;
            int bReturning = 0;
            Vector vDirection;
            vDirection = normalize(vTarget - vInitialCamera);

            static float fOldDist = 0.0f;
            static Vector vLastPos;
            Quaternion qDesired;
            if (vLastPos.m_fX == 0.0f && vLastPos.m_fY == 0.0f &&
                vLastPos.m_fZ == 0.0f)
            {
                vLastPos = pCreature->m_vPosition;
            }
            float fYaw = m_pModuleCamera->GetYaw() + fTimeDelta * m_fCameraTurn;
            float fPitch = m_pModuleCamera->GetPitch() + fTimeDelta * m_fCameraTilt;
            float fDistance = m_pModuleCamera->GetDist() *
                              powf(2.0f, fTimeDelta * fZoom * -0.5f);
            fPitch = max(g_fMinPitch, min(fPitch, g_fMaxPitch));
            fDistance = max(g_fMinDist, min(fDistance, g_fMaxDist));
            float fSpring = 1.0f - powf(0.5f, fTimeDelta * 750.0f);
            Vector vCamera;
            vCamera = pCreature->m_vPosition + Vector(0.0f, 0.0f, 1.0f);
            vCamera = vLastPos * (1.0f - fSpring) + vCamera * fSpring;
            qDesired = YawPitchRoll(fYaw, fPitch, 0.0f);
            Quaternion qOrientation;
            qOrientation = normalize(slerp(m_pModuleCamera->GetOrientation(), qDesired, fSpring));
            Vector vNext;
            vNext = vCamera + qOrientation.zdir() * fDistance;
            vLastPos = vCamera;

            int bCanReturn = 0;
            if (!(fZoom > 0.0f))
                bCanReturn = 1;
            hitInfo.Reset(0, CHitInfo::HIT_ALL_NON_TRIGGER, 0);
            hitInfo.m_nType = CHitInfo::HIT_TYPE_VISUAL_EFFECT_GROUND;
            int bHit;
            if (useclearlineofsight)
                bHit = !m_pInfo->ClearLineOfSightOneWay(vTarget, vNext, &vHit);
            else
                bHit = pScene->HitCheck(vTarget, vNext, &vHit, &hitInfo);
            if (bHit)
            {
                vHit = vHit + vDirection * CAMERA_COLLISION_RAY_EPSILON;
                fCollisionDistance = magnitude(vHit - vTarget);
                if (fOldDist == 0.0f)
                    fOldDist = m_pModuleCamera->GetDist();
            }
            else if (fOldDist > 0.0f)
            {
                Vector vLimit;
                vLimit = vTarget - vDirection * fOldDist;
                hitInfo.Reset(0, CHitInfo::HIT_ALL_NON_TRIGGER, 0);
                hitInfo.m_nType = CHitInfo::HIT_TYPE_VISUAL_EFFECT_GROUND;
                if (m_pModuleCamera->GetDist() < fOldDist)
                {
                    if (useclearlineofsight)
                        bHit = !m_pInfo->ClearLineOfSightOneWay(vNext, vLimit, &vHit);
                    else
                        bHit = pScene->HitCheck(vNext, vLimit, &vHit, &hitInfo);
                }
                else
                {
                    if (useclearlineofsight)
                        bHit = !m_pInfo->ClearLineOfSightOneWay(vLimit, vNext, &vHit);
                    else
                        bHit = pScene->HitCheck(vLimit, vNext, &vHit, &hitInfo);
                }
                if (bHit)
                {
                    vHit = vHit + vDirection * CAMERA_COLLISION_RAY_EPSILON;
                    fCollisionDistance = m_pModuleCamera->GetDist() +
                        (fOldDist - m_pModuleCamera->GetDist()) * fTimeDelta * 3.5f;
                    Vector vHitDifference = vHit - vTarget;
                    float fLengthSquared = vHitDifference * vHitDifference;
                    if (fCollisionDistance * fCollisionDistance >= fLengthSquared)
                        fCollisionDistance = sqrtf(fLengthSquared);
                }
                else
                {
                    fCollisionDistance = m_pModuleCamera->GetDist() +
                        (fOldDist - m_pModuleCamera->GetDist()) * fTimeDelta * 3.5f;
                    bReturning = 1;
                    if (fCollisionDistance >= fOldDist)
                    {
                        fCollisionDistance = fOldDist;
                        fOldDist = 0.0f;
                    }
                }
            }
            if (!bCanReturn)
                fOldDist = 0.0f;
            if ((fCollisionDistance == 0.0f && m_pModuleCamera->GetDist() < g_fMinDist) ||
                (fCollisionDistance > 0.0f && g_fMinDist - fCollisionDistance >= CAMERA_COLLISION_DISTANCE_TOLERANCE))
            {
                bClampDistance = 0;
                if (fOldDist < g_fMinDist)
                    fOldDist = g_fMinDist;
                fNewZoom = 0.0f;
            }
            if (lockcamera(NULL))
            {
                if (bClampDistance)
                    pFollow->m_fClampDistance = 1.0f;
                else
                    pFollow->m_fClampDistance = 0.0f;
                if (fCollisionDistance > 0.0f)
                {
                    pFollow->m_fDistance = fCollisionDistance;
                    if (!bReturning)
                        fNewZoom = 0.0f;
                    m_nReserved80 = 1;
                }
                if (m_pModuleCamera->GetDist() <= g_fMinDist && fNewZoom > 0.0f)
                    fNewZoom = 0.0f;
                if (m_pModuleCamera->GetDist() >= g_fMaxDist && fNewZoom < 0.0f)
                    fNewZoom = 0.0f;
                fZoom = fNewZoom;
            }
        }
    }
    return fZoom;
}
// PC 0x006441D0  Xbox 0x00067460
int CSWCModule::FindClearView(float fTimeDelta, float* pfYaw, float* pfPitch)
{
    NwcmoduleTrace5();
    CHitInfo hitInfo(0, CHitInfo::HIT_ALL_NON_TRIGGER, 0);
    Vector vHit;
    Vector vPosition;
    Vector vTarget;
    Vector vDirection;
    CAurBehavior* pBehavior;
    CAurBehaviorCameraFollow* pFollow;
    CClientExoApp* pApp;
    CSWCObject* pObject;
    CSWCObject* pTarget;
    CAurScene* pScene;
    float fYaw;
    float fTemp;

    pBehavior = m_pModuleCamera->GetBehavior(-1);
    if (pBehavior != NULL)
    {
        pFollow = pBehavior->AsBehaviorCameraFollow();
    }
    else
    {
        pFollow = NULL;
    }
    m_nReserved88 = 1;
    m_nReserved84 = 0;
    if (m_pInfo == NULL)
    {
        return 0;
    }
    pApp = g_pAppManager->m_pClientExoApp;
    pObject = pApp->GetGameObject(m_oidCameraTarget);
    if (pObject == NULL)
    {
        return 0;
    }
    pTarget = pObject->AsSWCObject();
    if (pTarget == NULL)
    {
        return 0;
    }
    if (pTarget->AsSWCCreature() != NULL)
    {
        fYaw = Yaw(pTarget->AsSWCCreature()->m_vDesiredOrientation);
    }
    else
    {
        fYaw = Yaw(pTarget->m_vOrientation);
    }
    if (pFollow != NULL)
    {
        pFollow->m_fYaw = fYaw;
    }
    if (GetNextCameraPosition(fTimeDelta, &vPosition) == 0)
    {
        return 0;
    }
    pScene = m_pInfo->m_pScene;
    if (pScene == NULL)
    {
        return 0;
    }
    vTarget = pTarget->m_vPosition + m_vCameraOffset;
    hitInfo.m_nType = CHitInfo::HIT_TYPE_VISUAL_EFFECT_GROUND;
    if (pScene->HitCheck(vPosition, vTarget, &vHit, &hitInfo) == 0)
    {
        return 0;
    }
    vDirection = vPosition - vTarget;
    vDirection.m_fX = -vDirection.m_fX;
    vDirection.m_fY = -vDirection.m_fY;
    hitInfo.Reset(0, CHitInfo::HIT_ALL_NON_TRIGGER, 0);
    hitInfo.m_nType = CHitInfo::HIT_TYPE_VISUAL_EFFECT_GROUND;
    if (pScene->HitCheck(vTarget + vDirection, vTarget, &vHit,
                         &hitInfo) == 0)
    {
        *pfYaw = m_pModuleCamera->GetYaw() + 180.0f;
        *pfPitch = m_pModuleCamera->GetPitch();
        return 1;
    }
    fTemp = vDirection.m_fX;
    vDirection.m_fX = vDirection.m_fY;
    vDirection.m_fY = -fTemp;
    hitInfo.Reset(0, CHitInfo::HIT_ALL_NON_TRIGGER, 0);
    hitInfo.m_nType = CHitInfo::HIT_TYPE_VISUAL_EFFECT_GROUND;
    if (pScene->HitCheck(vTarget + vDirection, vTarget, &vHit,
                         &hitInfo) == 0)
    {
        *pfYaw = m_pModuleCamera->GetYaw() - 90.0f;
        *pfPitch = m_pModuleCamera->GetPitch();
        return 1;
    }
    hitInfo.Reset(0, CHitInfo::HIT_ALL_NON_TRIGGER, 0);
    hitInfo.m_nType = CHitInfo::HIT_TYPE_VISUAL_EFFECT_GROUND;
    if (pScene->HitCheck(vPosition, vTarget, &vHit, &hitInfo) == 0)
    {
        *pfYaw = m_pModuleCamera->GetYaw() + 90.0f;
        *pfPitch = m_pModuleCamera->GetPitch();
        return 1;
    }
    return 0;
}
// PC 0x00644650  Xbox 0x00067900
void CSWCModule::Update(float fTimeDelta)
{
    float fYaw;
    float fPitch;
    CAurBehavior* pBehavior;
    CAurBehaviorCameraFollow* pFollow;

    if (m_pInfo != NULL)
    {
        if (m_pModuleCamera != NULL)
        {
            if (m_bCameraCollisionChanged != 0)
            {
                if (FindClearView(fTimeDelta, &fYaw, &fPitch) != 0)
                {
                    pBehavior = m_pModuleCamera->GetBehavior(-1);
                    if (pBehavior != NULL)
                    {
                        pFollow = pBehavior->AsBehaviorCameraFollow();
                        if (pFollow != NULL)
                        {
                            pFollow->m_fYaw = fYaw;
                            pFollow->m_fPitch = fPitch;
                        }
                    }
                }
                m_bCameraCollisionChanged = 0;
            }
            if (!g_pAppManager->m_pClientExoApp->m_pcExoAppInternal->GetActivePauseState())
            {
                m_fTimeOfDay = fTimeDelta + m_fTimeOfDay;
            }
            m_pInfo->Update(fTimeDelta);
            if (g_pAppManager->m_pClientExoApp->GetSWGuiManager()->m_nBackgroundPanels <= 0)
            {
                m_pModuleCamera->RenderScene();
            }
        }
    }
}

static inline void NwcmoduleTrace5()
{
    NwcmoduleTrace4();
}

static inline void NwcmoduleTrace4()
{
    NwcmoduleTrace3();
}

static inline void NwcmoduleTrace3()
{
    NwcmoduleTrace2();
}

static inline void NwcmoduleTrace2()
{
    NwcmoduleTrace1();
}

static inline void NwcmoduleTrace1()
{
}
#ifdef _XBOX

#include "aurconsole.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char g_szViewResult[256];
// Xbox 0x000620E0
char* viewmindist(char* szArgs)
{
    char szCommand[256];
    if (strlen(szArgs) < 1)
    {
        sprintf(g_szViewResult, "%f", g_fMinDist);
        return g_szViewResult;
    }
    sprintf(szCommand, "mindist %s", szArgs);
    if (g_pAppManager != NULL && g_pAppManager->m_pClientExoApp != NULL &&
        g_pAppManager->m_pClientExoApp->GetModule() != NULL &&
        g_pAppManager->m_pClientExoApp->GetModule()->m_pModuleCamera != NULL)
    {
        g_pAppManager->m_pClientExoApp->GetModule()->GetCamera()->BehaviorMessage(szCommand);
        g_fMinDist = (float)atof(szArgs);
        return "Success";
    }
    return "Failure";
}
ConsoleFunc ConFviewmindist("viewmindist", viewmindist);
// Xbox 0x000621F0
char* viewmaxdist(char* szArgs)
{
    char szCommand[256];
    if (strlen(szArgs) < 1)
    {
        sprintf(g_szViewResult, "%f", g_fMaxDist);
        return g_szViewResult;
    }
    sprintf(szCommand, "maxdist %s", szArgs);
    if (g_pAppManager != NULL && g_pAppManager->m_pClientExoApp != NULL &&
        g_pAppManager->m_pClientExoApp->GetModule() != NULL &&
        g_pAppManager->m_pClientExoApp->GetModule()->m_pModuleCamera != NULL)
    {
        g_pAppManager->m_pClientExoApp->GetModule()->GetCamera()->BehaviorMessage(szCommand);
        g_fMaxDist = (float)atof(szArgs);
        return "Success";
    }
    return "Failure";
}
ConsoleFunc ConFviewmaxdist("viewmaxdist", viewmaxdist);
// Xbox 0x00062300
char* viewminpitch(char* szArgs)
{
    char szCommand[256];
    if (strlen(szArgs) < 1)
    {
        sprintf(g_szViewResult, "%f", g_fMinPitch);
        return g_szViewResult;
    }
    sprintf(szCommand, "minpitch %s", szArgs);
    if (g_pAppManager != NULL && g_pAppManager->m_pClientExoApp != NULL &&
        g_pAppManager->m_pClientExoApp->GetModule() != NULL &&
        g_pAppManager->m_pClientExoApp->GetModule()->m_pModuleCamera != NULL)
    {
        g_pAppManager->m_pClientExoApp->GetModule()->GetCamera()->BehaviorMessage(szCommand);
        g_fMinPitch = (float)atof(szArgs);
        return "Success";
    }
    return "Failure";
}
ConsoleFunc ConFviewminpitch("viewminpitch", viewminpitch);
// Xbox 0x00062410
char* viewmaxpitch(char* szArgs)
{
    char szCommand[256];
    if (strlen(szArgs) < 1)
    {
        sprintf(g_szViewResult, "%f", g_fMaxPitch);
        return g_szViewResult;
    }
    sprintf(szCommand, "maxpitch %s", szArgs);
    if (g_pAppManager != NULL && g_pAppManager->m_pClientExoApp != NULL &&
        g_pAppManager->m_pClientExoApp->GetModule() != NULL &&
        g_pAppManager->m_pClientExoApp->GetModule()->m_pModuleCamera != NULL)
    {
        g_pAppManager->m_pClientExoApp->GetModule()->GetCamera()->BehaviorMessage(szCommand);
        g_fMaxPitch = (float)atof(szArgs);
        return "Success";
    }
    return "Failure";
}
ConsoleFunc ConFviewmaxpitch("viewmaxpitch", viewmaxpitch);
// Xbox 0x00062520
char* viewfollowoffset(char* szArgs)
{
    Vector vOffset(0.0f, 0.0f, 0.0f);
    char szCommand[64];
    sscanf(szArgs, "%f %f %f", &vOffset.m_fX, &vOffset.m_fY, &vOffset.m_fZ);
    sprintf(szCommand, "offset %f %f %f", vOffset.m_fX, vOffset.m_fY, vOffset.m_fZ);
    if (g_pAppManager != NULL && g_pAppManager->m_pClientExoApp != NULL &&
        g_pAppManager->m_pClientExoApp->GetModule() != NULL &&
        g_pAppManager->m_pClientExoApp->GetModule()->m_pModuleCamera != NULL)
    {
        g_pAppManager->m_pClientExoApp->GetModule()->SetCameraOffset(vOffset);
        g_pAppManager->m_pClientExoApp->GetModule()->GetCamera()->BehaviorMessage(szCommand);
        return "Success";
    }
    return "Failure";
}
ConsoleFunc ConFviewfollowoffset("viewfollowoffset", viewfollowoffset);
// Xbox 0x00062630
char* setcameramode(char* szArgs)
{
    int nMode;
    sscanf(szArgs, "%i", &nMode);
    if (g_pAppManager != NULL && g_pAppManager->m_pClientExoApp != NULL &&
        g_pAppManager->m_pClientExoApp->GetClientOptions() != NULL)
    {
        g_pAppManager->GetClientExoApp()->GetClientOptions()->SetCameraMode((unsigned char)nMode);
        return "Success";
    }
    return "Failure";
}
ConsoleFunc ConFsetcameramode("setcameramode", setcameramode);
#endif
