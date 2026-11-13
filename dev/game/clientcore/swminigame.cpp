#include <aurlist.h>
#include <math.h>
#include <string.h>
#include "nwccreature.h"
#include "swminigame.h"

#include "swplayercontrol.h"
#include "nwcarea.h"
#include "nwcmodule.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "serverexoapp.h"
#include "partytables.h"
#include "twodimarray.h"
#include "aurcamera.h"
#include "nwcanimbase.h"
#include "exobase.h"
#include "math3d.h"
#include "aurmodel.h"
#include "swcroom.h"
#include "aurscene.h"
#include "nwcplaceable.h"
#include "nwsobject.h"
#include "nwsmodule.h"
#include "nwsarea.h"
#include "nwsscriptevent.h"
#include "serveraimaster.h"

int SceneHitCheck(CAurScene* pScene, Vector vStart, Vector vEnd, Vector* pHit, CHitInfo* pHitInfo);

static inline void SwminigameTrace1();

inline void SwminigameContainerAllocates(CExoArrayList<unsigned long>* pIds,
                                         CExoArrayList<SWMGTrackInfo*>* pTracks,
                                         CExoArrayList<SafePointer<CAurObject>*>* pBullets,
                                         CExoArrayList<SWMGExplodeBullet*>* pEffects,
                                         CExoArrayList<CResRef>* pModels,
                                         List<SafePointer<CAurObject>*>* pPointers)
{
    pIds->Allocate(1);
    pTracks->Allocate(1);
    pBullets->Allocate(1);
    pEffects->Allocate(1);
    pModels->Allocate(1);
    pPointers->allocate(1);
}

#define SWMINIGAME_TRACE() SwminigameTrace1()
// PC 0x006C55D0 GOG 0x006C52E0
inline void CSWMiniGame::AxisVelocity(const Vector& vAxis, float fScale)
{
    Vector vVelocity = m_fAxisScale * vAxis;
    m_NextState.m_vRate = m_CurrentState.m_vRate + vVelocity * fScale;
}
// PC 0x006C5650 GOG 0x006C5360
inline Vector CSWMiniGame::GetAcceleration(CSWMGPhysicsState& state,
                                           const Vector& vTarget)
{
    return m_fLateralAccel * vTarget -
           (m_fLateralAccel / m_fAxisScale) * state.m_vValue;
}
// PC 0x006C56D0 GOG 0x006C53E0
inline int CSWMiniEnemy::TargetType()
{
    return TARGET_ENEMY;
}
// PC 0x006C56E0 GOG 0x006C53F0
void BulletDie(CAurObject* pBullet, char*, void*)
{
    delete pBullet;
}
// PC 0x006C56F0 GOG 0x006C5400
void CSWMiniGame::GetClipDist(float* pfNear, float* pfFar)
{
    if (pfNear != NULL)
    {
        *pfNear = m_fNearClip;
    }
    if (pfFar != NULL)
    {
        *pfFar = m_fFarClip;
    }
}
// PC 0x006C5710 GOG 0x006C5420
void CSWMiniGame::SetClipDist(float fNear, float fFar)
{
    if (fNear > fFar)
    {
#ifdef _XBOX
        m_fNearClip = fFar;
        m_fFarClip = fNear;
#else
        m_fFarClip = fNear;
        m_fNearClip = fFar;
#endif
    }
    else if (fNear < fFar)
    {
        m_fNearClip = fNear;
        m_fFarClip = fFar;
    }
    else
    {
        m_fNearClip = fNear;
        m_fFarClip = fFar;
    }

    if (m_pCam != NULL)
    {
        m_pCam->SetClipDist(m_fNearClip, m_fFarClip);
    }
}
// PC 0x006C5770 GOG 0x006C5480
Vector CSWMiniGame::BumpPlane(const Quaternion& qOrientation)
{
    switch (m_nBumpPlane)
    {
    case 1:
        return qOrientation.xdir();
    case 2:
        return qOrientation.ydir();
    case 3:
        return qOrientation.zdir();
    default:
        return Vector();
    }
}
// PC 0x006C57D0 GOG 0x006C54E0
int CSWMiniGame::GetAttachType()
{
    switch (m_nAttachType)
    {
    case 1:
        return 1;
    case 2:
        return 2;
    default:
        return 0;
    }
}
// PC 0x006C57F0 GOG 0x006C5500
void CSWMiniGame::AxisAcceleration(const Vector& vAxis, float fScale)
{

    CSWMGPhysicsState state;
    CSWMGPhysicsState k1;
    CSWMGPhysicsState k2;
    CSWMGPhysicsState k3;
    CSWMGPhysicsState k4;

    Vector vMidAxis = 0.5f * (vAxis + m_vPosition);
    k1 = GetDerivative(m_CurrentState.m_vValue,
                       GetAcceleration(m_CurrentState, m_vPosition)) * fScale;
    state = m_CurrentState + k1 * 0.5f;
    k2 = GetDerivative(state.m_vValue,
                       GetAcceleration(state, vMidAxis)) * fScale;
    state = m_CurrentState + k2 * 0.5f;
    k3 = GetDerivative(state.m_vValue,
                       GetAcceleration(state, vMidAxis)) * fScale;
    state = m_CurrentState + k3;
    k4 = GetDerivative(state.m_vValue,
                       GetAcceleration(state, vAxis)) * fScale;
    m_NextState = m_CurrentState + k1 / 6.0f + k2 / 3.0f +
                  k3 / 3.0f + k4 / 6.0f;
    if (m_nAttachType == ATTACH_PLAYER_MODEL)
    {
        m_NextState.m_vValue.m_fZ = m_CurrentState.m_vValue.m_fZ - fScale * 9.8f;
        if (m_NextState.m_vRate.m_fZ <= 0.0f)
        {
            m_NextState.m_vValue.m_fZ = 0.0f;
        }
    }
    m_vPosition = vAxis;
}
// PC 0x006C5BB0 GOG 0x006C58C0
void CSWMiniGame::UpdateAxis(Vector& vAxis, float fScale)
{
    switch (m_nAttachType)
    {
    case 1:
        vAxis.m_fY = vAxis.m_fY * -1.0f;
        vAxis.m_fZ = vAxis.m_fZ * 0.0f;
        break;
    case 2:
    {
        float fX = -vAxis.m_fX;
        vAxis.m_fX = -vAxis.m_fZ;
        vAxis.m_fZ = fX;
        vAxis.m_fY = vAxis.m_fY * -1.0f;
    }
    break;
    default:
        return;
    }
    switch (m_nPhysicsMode)
    {
    case 0:
        AxisVelocity(vAxis, fScale);
        break;
    case 1:
        AxisAcceleration(vAxis, fScale);
        break;
    }
}
// PC 0x006C5C20 GOG 0x006C5930
void CSWMiniGame::UpdateMouse(float fDeltaX, float fDeltaY, float fTimeStep)
{
    int nAxisX = m_nMouseAxisX;
    float fScale = fTimeStep * m_fAxisScale;
    fDeltaX = fDeltaX * fScale;
    float fDeltaYScaled = fScale * fDeltaY;
    if (nAxisX < 0)
    {
        fDeltaX = -fDeltaX;
    }
    switch (abs(nAxisX))
    {
    case 1:
        m_NextState.m_vRate.m_fX = m_NextState.m_vRate.m_fX - fDeltaX;
        break;
    case 2:
        m_NextState.m_vRate.m_fY = m_NextState.m_vRate.m_fY - fDeltaX;
        break;
    case 3:
        m_NextState.m_vRate.m_fZ = m_NextState.m_vRate.m_fZ - fDeltaX;
        break;
    }

    int nAxisY = m_nMouseAxisY;
    if (nAxisY < 0)
    {
        fDeltaYScaled = -fDeltaYScaled;
    }
    switch (abs(nAxisY))
    {
    case 1:
        m_NextState.m_vRate.m_fX = m_NextState.m_vRate.m_fX - fDeltaYScaled;
        break;
    case 2:
        m_NextState.m_vRate.m_fY = m_NextState.m_vRate.m_fY - fDeltaYScaled;
        break;
    case 3:
        m_NextState.m_vRate.m_fZ = m_NextState.m_vRate.m_fZ - fDeltaYScaled;
        break;
    }
}
// PC 0x006C5CE0 GOG 0x006C59F0
void CSWMiniGame::Control(float fTimeStep)
{
    if (m_pPlayer != NULL)
    {
        m_pPlayer->Control(fTimeStep, m_NextState, m_CurrentState);
        m_CurrentState = m_NextState;
    }

    int nDirection;
    if (m_fSteerValue < 0.0f)
    {
        nDirection = -1;
    }
    else
    {
        nDirection = 1;
    }

    if (m_nAttachType == ATTACH_PLAYER_MODEL)
    {
        float fMagnitude = (float)fabs(m_fSteerValue);
        float fMove = fTimeStep * m_vPosition.m_fX;
        if (m_vPosition.m_fX != 0.0f)
        {
            m_bSteerCentering = 0;
            if (!((m_vPosition.m_fX < 0.0f && m_fSteerValue < 0.0f) ||
                  (m_vPosition.m_fX > 0.0f && m_fSteerValue > 0.0f)))
            {
                m_fSteerValue = fMove;
            }
            else if (fMagnitude < 0.25f)
            {
                float fNewValue = fMove + m_fSteerValue;
                m_fSteerValue = fNewValue;
                if (fNewValue >= 0.25f)
                {
                    m_fSteerValue = 0.255f;
                }
                else if (fNewValue <= -0.25f)
                {
                    m_fSteerValue = -0.255f;
                }
            }
        }
        else if (m_fSteerValue != 0.0f)
        {
            if (m_bSteerCentering == 0)
            {
                if (fMagnitude >= 0.23f && fMagnitude < 0.235f)
                {
                    m_fSteerValue = nDirection * 0.235f;
                }
                else if (fMagnitude >= 0.17f && fMagnitude < 0.175f)
                {
                    m_fSteerValue = nDirection * 0.175f;
                }
                else if (fMagnitude >= 0.1f && fMagnitude < 0.105f)
                {
                    m_fSteerValue = nDirection * 0.105f;
                }
                else if (fMagnitude >= 0.041999999f &&
                         fMagnitude < 0.046999998f)
                {
                    m_fSteerValue = nDirection * 0.041999999f;
                }
                m_bSteerCentering = 1;
            }
            float fStep = nDirection * fTimeStep;
            if (fabs(fStep) > fabs(m_fSteerValue))
            {
                m_fSteerValue = 0.0f;
            }
            else
            {
                m_fSteerValue = m_fSteerValue - fStep;
            }
        }

        fMagnitude = (float)fabs(m_fSteerValue);
        if (fMagnitude >= 0.25f)
        {
            nDirection *= 10;
        }
        else if (fMagnitude >= 0.23999999f)
        {
            nDirection *= 9;
        }
        else if (fMagnitude >= 0.23f)
        {
            nDirection *= 8;
        }
        else if (fMagnitude >= 0.2f)
        {
            nDirection *= 7;
        }
        else if (fMagnitude >= 0.17f)
        {
            nDirection *= 6;
        }
        else if (fMagnitude >= 0.13500001f)
        {
            nDirection *= 5;
        }
        else if (fMagnitude >= 0.1f)
        {
            nDirection *= 4;
        }
        else if (fMagnitude >= 0.071000002f)
        {
            nDirection *= 3;
        }
        else if (fMagnitude >= 0.041999999f)
        {
            nDirection *= 2;
        }
        else if (!(fMagnitude >= 0.021f))
        {
            nDirection = 0;
        }

        if (m_pPlayer != NULL)
        {
            m_pPlayer->AdjustPosition(nDirection, 0);
        }
    }
}
// PC 0x006C60E0 GOG 0x006C5DF0
void CSWMiniGame::Go()
{
    SWMINIGAME_TRACE();
    CSWMiniGameObjectArray* pArray =
        g_pAppManager->GetClientExoApp()->GetMGOArray();
    if (m_pPlayer != NULL)
    {
        int nSwoop1 = g_pAppManager->GetServerExoApp()->GetPartyTable()->GetSwoopUpgrade(0);
        int nSwoop2 = g_pAppManager->GetServerExoApp()->GetPartyTable()->GetSwoopUpgrade(1);
        int nSwoop3 = g_pAppManager->GetServerExoApp()->GetPartyTable()->GetSwoopUpgrade(2);
        m_pPlayer->Go();
    }
    for (int i = m_lstEnemyIds.num - 1; i >= 0; --i)
    {
        CSWTrackFollower* pEnemy = (CSWTrackFollower*)pArray->GetMiniGameObject(
            m_lstEnemyIds.element[i], 2);
        if (pEnemy != NULL)
        {
            pEnemy->Go();
        }
        else
        {
            int nNewCount = m_lstEnemyIds.num - 1;
            int j = i;
            for (m_lstEnemyIds.num = nNewCount; j < m_lstEnemyIds.num; ++j)
            {
                m_lstEnemyIds.element[j] = m_lstEnemyIds.element[j + 1];
            }
        }
    }
    if (m_pImpactSound != NULL)
    {
#ifdef _XBOX
        m_pImpactSound->Play(m_cAmbientSoundResRef, 11, 0, 1, 0);
#else
        m_pImpactSound->Play(m_cAmbientSoundResRef, 8, 0, 1, 0);
#endif
    }
}
// PC 0x006C61A0 GOG 0x006C5EB0
int CSWMiniGame::GetEnemy(int nIndex)
{
    SWMINIGAME_TRACE();
    if (nIndex >= m_lstEnemyIds.num)
    {
        return 0;
    }
    return (int)g_pAppManager->GetClientExoApp()->GetMGOArray()->GetMiniGameObject(
        m_lstEnemyIds.element[nIndex], 2);
}
// PC 0x006C61E0 GOG 0x006C5EF0
CSWMiniEnemy* CSWMiniGame::RemoveEnemy(CSWMiniEnemy* pEnemy)
{
    unsigned long nObjectId = pEnemy->m_nObjectId;
    int nIndex = m_lstEnemyIds.IndexOf(nObjectId);
    if (nIndex == -1)
    {
        return NULL;
    }
    m_lstEnemyIds.DelIndex(nIndex);
    return pEnemy;
}
// PC 0x006C6240 GOG 0x006C5F50
CSWMGObstacle* CSWMiniGame::RemoveObstacle(CSWMGObstacle* pObstacle)
{
    unsigned long nObjectId = pObstacle->m_nObjectId;
    int nIndex = m_lstObstacleIds.IndexOf(nObjectId);
    if (nIndex == -1)
    {
        return NULL;
    }
    m_lstObstacleIds.DelIndex(nIndex);
    pObstacle->m_pModel->AttachToScene(NULL);
    return pObstacle;
}
// PC 0x006C62B0 GOG 0x006C5FC0
SWMGTrackInfo* CSWMiniGame::GetProtoTrack(int nIndex)
{
    SWMINIGAME_TRACE();
    if (nIndex >= m_lstTracks.num)
    {
        return 0;
    }
    return m_lstTracks.element[nIndex];
}

SWMGTrackInfo* CSWMiniGame::GetProtoTrack(const CResRef& cResRef)
{
    int nTracks = m_lstTracks.num;
    for (int i = 0; i < nTracks; ++i)
    {
        SWMGTrackInfo* pTrack = m_lstTracks.element[i];
        if (cResRef == pTrack->m_cResRef)
        {
            return pTrack;
        }
    }
    return NULL;
}
// PC 0x006C62D0 GOG 0x006C5FE0
CSWMGObstacle* CSWMiniGame::GetObstacle(int nIndex)
{
    SWMINIGAME_TRACE();
    if (nIndex >= m_lstObstacleIds.num)
    {
        return 0;
    }
    return (CSWMGObstacle*)g_pAppManager->GetClientExoApp()->GetMGOArray()->GetMiniGameObject(
        m_lstObstacleIds.element[nIndex], SWMG_OBJECT_OBSTACLE);
}
// PC 0x006C6310 GOG 0x006C6020
CAurCamera* CSWMiniGame::SetCamera(CAurCamera* pCamera)
{
    SWMINIGAME_TRACE();
    m_pCam = pCamera;
    if (m_pCam != NULL && m_pPlayer != NULL && m_pPlayer->m_pTrack != NULL)
    {
        switch (m_nAttachType)
        {
        case ATTACH_PLAYER_MODEL:
        {
            int nModels = m_pPlayer->m_lstModels.num;
            CAurPart* pHook = NULL;
            Gob* pModel;
            for (int i = 0; i < nModels && pHook == NULL; ++i)
            {
                pModel = m_pPlayer->GetModel(i);
                if (pModel != NULL)
                {

                    pHook = (CAurPart*)pModel->GetPart("camerahook");
                }
            }
            if (pHook != NULL)
            {
                m_pCam->SetBehavior(new CAurBehaviorAttach(
                    m_pCam->AsCAurObject(), pModel, pHook));
            }
            else
            {
                Gob* pTrack = m_pPlayer->m_pTrack;
                if (pTrack != NULL)
                {
                    CAurPart* pTrackHook = (CAurPart*)pTrack->GetPart("camerahook");
                    if (pTrackHook != NULL)
                    {
                        m_pCam->SetBehavior(new CAurBehaviorAttach(
                            m_pCam->AsCAurObject(), pTrack, pTrackHook));
                    }
                }
            }
        }
        break;
        case ATTACH_CAMERA_HOOK:
            m_pCam->AttachToObject(m_pPlayer->GetCameraHook(), "camerahook", Gob::ATTACH_NORMAL);
            break;
        }
        m_pCam->SetClipDist(m_fNearClip, m_fFarClip);
        m_pCam->SetViewAngle(m_fViewAngle);
    }
    return m_pCam;
}
// PC 0x006C6490 GOG 0x006C61A0
CSWMiniPlayer* CSWMiniGame::SetPlayer(CSWMiniPlayer* pPlayer)
{
    if (m_pPlayer != NULL)
    {
        delete m_pPlayer;
    }
    CAurCamera* pCamera = m_pCam;
    m_pPlayer = pPlayer;
    SetCamera(pCamera);
    return m_pPlayer;
}
// PC 0x006C64C0 GOG 0x006C61D0
CSWMiniPlayer* CSWMiniGame::SetPlayer(CExoArrayList<CResRef>& lstModels,
                                      CExoArrayList<unsigned char>& lstRotating,
                                      const CResRef& cTrack)
{
    char sModel[20];
    int nModels = lstModels.num;
    if (m_pPlayer != NULL)
    {
        delete m_pPlayer;
    }
    m_pPlayer = new CSWMiniPlayer(this);
    for (int i = 0; i < nModels; ++i)
    {
        lstModels.element[i].CopyToString(sModel);
        CAurObject* pModel = NewCAurObject(sModel, "", 0, 0);
        if (pModel != NULL)
        {
            m_pPlayer->AddModel(pModel, lstRotating.element[i]);
        }
    }

    cTrack.CopyToString(sModel);
    Gob* pTrack = static_cast<Gob*>(NewCAurObject(sModel, "", 0, 0));
    if (pTrack != NULL)
    {
        SWMGTrackInfo* pTrackInfo = GetProtoTrack(cTrack);
        if (pTrackInfo != NULL)
        {
            pTrack->SetPosition(pTrackInfo->m_vPosition);
        }
        pTrack->AttachToScene((Scene*)m_pArea->m_pScene);
        m_pPlayer->SetTrack(pTrack, GetAttachType());
    }
    SetCamera(m_pCam);
    return m_pPlayer;
}
// PC 0x006C6670 GOG 0x006C6380
CSWMiniPlayer* CSWMiniGame::SetPlayer(CExoArrayList<CResRef>& lstModels,
                                      CExoArrayList<unsigned char>& lstRotating,
                                      const CResRef& cTrack, const CResRef& cCamera,
                                      unsigned char bCameraRotate)
{
    char sModel[20];
    int nModels = lstModels.num;
    if (m_pPlayer != NULL)
    {
        delete m_pPlayer;
    }
    m_pPlayer = new CSWMiniPlayer(this);
    for (int i = 0; i < nModels; ++i)
    {
        lstModels.element[i].CopyToString(sModel);
        CAurObject* pModel = NewCAurObject(sModel, "", 0, 0);
        if (pModel != NULL)
        {
            m_pPlayer->AddModel(pModel, lstRotating.element[i]);
        }
    }

    cTrack.CopyToString(sModel);
    Gob* pTrack = static_cast<Gob*>(NewCAurObject(sModel, "", 0, 0));
    if (pTrack != NULL)
    {
        SWMGTrackInfo* pTrackInfo = GetProtoTrack(cTrack);
        if (pTrackInfo != NULL)
        {
            pTrack->SetPosition(pTrackInfo->m_vPosition);
        }
        pTrack->AttachToScene((Scene*)m_pArea->m_pScene);
        m_pPlayer->SetTrack(pTrack, GetAttachType());
    }
    cCamera.CopyToString(sModel);
    Gob* pCameraHook = static_cast<Gob*>(NewCAurObject(sModel, "", 0, 0));
    if (pCameraHook != NULL)
    {
        pCameraHook->AttachToScene((Scene*)m_pArea->m_pScene);
        m_pPlayer->SetCameraHook(pCameraHook, bCameraRotate);
    }
    SetCamera(m_pCam);
    return m_pPlayer;
}
// PC 0x006C6860 GOG 0x006C6570
int CSWMiniGame::SetMouseAxisX(int nAxis)
{
    SWMINIGAME_TRACE();
    if (nAxis >= 0 && nAxis <= 3)
    {
        m_nMouseAxisX = nAxis;
    }
    return m_nMouseAxisX;
}
// PC 0x006C6880 GOG 0x006C6590
int CSWMiniGame::SetMouseAxisY(int nAxis)
{
    SWMINIGAME_TRACE();
    if (nAxis >= 0 && nAxis <= 3)
    {
        m_nMouseAxisY = nAxis;
    }
    return m_nMouseAxisY;
}
// PC 0x006C68A0 GOG 0x006C65B0
void CSWMiniGame::SetCurPos(Vector vPosition)
{
    Vector vTrackPosition = m_pPlayer->m_pTrack->GetPosition();
    m_pPlayer->m_pTrack->GetPart("modelhook");
    m_pPlayer->m_pTrack->PlayAnimation(NULL, 0.0f, 2, 0.0f);
    m_pPlayer->m_pTrack->PlayAnimation("track", 0.0f, 2, 0.0f);
    m_CurrentState.m_vValue.m_fX = 0.0f;
    m_CurrentState.m_vRate.m_fX = 0.0f;
    m_NextState.m_vRate.m_fX = 0.0f;
    m_CurrentState.m_vRate.m_fY = 0.0f;
    m_NextState.m_vRate.m_fY = 0.0f;
    m_vPosition.m_fX = 0.0f;
    m_vPosition.m_fY = 0.0f;
    m_NextState.m_vValue.m_fX = 0.0f;
    m_CurrentState.m_vValue.m_fY = 0.0f;
    m_fLateralAccel = 0.0f;
    m_pPlayer->SetOffset(Vector(0.0f, 0.0f, 0.0f));
    SetCamera(m_pCam);
}
// PC 0x00410CD0 GOG 0x00410C80
template <> void CExoArrayList<SWMGExplodeBullet*>::Add(SWMGExplodeBullet* value)
{
    if (num == array_size)
    {
        Allocate(array_size != 0 ? array_size * 2 : EXOARRAYLIST_INITIAL_ALLOCATION);
    }
    element[num++] = value;
}
// PC 0x006C6960 GOG 0x006C6670
CSWMiniGame::CSWMiniGame(CSWCArea* pArea)
{
    m_pArea = pArea;
    m_pPlayer = NULL;
    m_pCam = NULL;
    m_fSteerValue = 0.0f;
    m_fNearClip = 0.1f;
    m_fViewAngle = 65.0f;
    m_fFarClip = 100.0f;
    m_nBumpPlane = 0;
    m_nDOF = 0;
    m_nAttachType = ATTACH_TRACK;
    m_nMouseAxisX = 0;
    m_nMouseAxisY = 0;
    m_nPhysicsMode = PHYSICS_ACCELERATION;
    m_pCurrentRoom = NULL;
    m_pImpactSound = new CExoStreamingSoundSource(1);
    m_pImpactSound->SetLooping(1);
    m_fF0 = 1.0f;
    m_fF4 = 1.0f;
    m_fF8 = 1.0f;
    SetResRef(pArea->m_cAreaResRef, 0);
    m_pFC = new C2DA("swoopupgrade", 0);
    m_pFC->Load2DArray();
    for (int i = 0; i < 6; i++)
    {
        int nValue = 0;
        int nLevel = g_pAppManager->GetServerExoApp()->GetPartyTable()->GetSwoopUpgrade(i);
        m_pFC->GetINTEntry(nLevel, "level", &nValue);
        m_anSwoopUpgrades[i] = nValue;
    }
    m_fJumpSpeed = 0.0f;
}
// PC 0x006C6C40 GOG 0x006C6940
CSWMiniEnemy* CSWMiniGame::AddEnemy(CExoArrayList<CResRef>& lstModels,
                                    CExoArrayList<unsigned char>& lstRotating,
                                    const CResRef& cTrack)
{
    char sModel[20];
    int nModels = lstModels.num;
    CSWMiniEnemy* pEnemy = new CSWMiniEnemy(this);
    for (int i = 0; i < nModels; ++i)
    {
        lstModels.element[i].CopyToString(sModel);
        CAurObject* pModel = NewCAurObject(sModel, "", 0, 0);
        if (pModel != NULL)
        {
            pEnemy->AddModel(pModel, lstRotating.element[i]);
        }
    }

    cTrack.CopyToString(sModel);
    Gob* pTrack = static_cast<Gob*>(NewCAurObject(sModel, "", 0, 0));
    if (pTrack != NULL)
    {
        SWMGTrackInfo* pTrackInfo = GetProtoTrack(cTrack);
        if (pTrackInfo != NULL)
        {
            pTrack->SetPosition(pTrackInfo->m_vPosition);
        }
        pTrack->AttachToScene((Scene*)m_pArea->m_pScene);
        pEnemy->SetTrack(pTrack, 0);
    }
    m_lstEnemyIds.Add(pEnemy->m_nObjectId);
    return pEnemy;
}
// PC 0x006C6E40 GOG 0x006C6B40
int CSWMiniGame::ClearEnemies()
{
    int nCount = m_lstEnemyIds.num;
    CSWMiniGameObjectArray* pArray =
        g_pAppManager->GetClientExoApp()->GetMGOArray();
    for (int i = 0; i < nCount; ++i)
    {
        CSWMiniEnemy* pEnemy = (CSWMiniEnemy*)pArray->GetMiniGameObject(
            m_lstEnemyIds[i], 2);
        if (pEnemy != NULL)
        {
            delete pEnemy;
        }
    }
    m_lstEnemyIds.SetSize(0);
    return nCount;
}
// PC 0x006C6EC0 GOG 0x006C6BC0
SWMGTrackInfo* CSWMiniGame::AddTrack(const CResRef& cResRef,
                                     const Vector& vPosition)
{
    int nCount = m_lstTracks.num;
    for (int i = 0; i < nCount; ++i)
    {
        SWMGTrackInfo* pTrack = m_lstTracks.element[i];
        if (pTrack->m_cResRef == cResRef)
        {
            return pTrack;
        }
    }
    SWMGTrackInfo* pNewTrack = new SWMGTrackInfo;
    pNewTrack->m_cResRef = cResRef;
    pNewTrack->m_vPosition = vPosition;
    m_lstTracks.Add(pNewTrack);
    return pNewTrack;
}
// PC 0x006C6FF0 GOG 0x006C6CF0
int CSWMiniGame::ClearTracks()
{
    int nOldCount = m_lstTracks.num;
    for (int i = nOldCount - 1; i >= 0; --i)
    {
        operator delete(m_lstTracks[i]);
    }
    m_lstTracks.SetSize(0);
    return nOldCount;
}
// PC 0x006C7040 GOG 0x006C6D40
CSWMGObstacle* CSWMiniGame::AddObstacle(const CResRef& cResRef,
                                        const Vector& vPosition)
{
    char sModel[20];
    CSWMGObstacle* pObstacle = NULL;
    cResRef.CopyToString(sModel);
    Gob* pModel = static_cast<Gob*>(NewCAurObject(sModel, "", 0, 0));
    if (pModel != NULL)
    {
        pModel->SetPosition(vPosition);
        CAurScene* pScene = m_pArea->m_pScene;
        pModel->AttachToScene((Scene*)pScene);
        pModel->PlayAnimation("ready", 1.0f, 0, 0.0f);
        pObstacle = new CSWMGObstacle(this, pModel);
        m_lstObstacleIds.Add(pObstacle->m_nObjectId);
    }
    return pObstacle;
}
// PC 0x006C71D0 GOG 0x006C6ED0
int CSWMiniGame::Load()
{
    CResRef cResRef;
    CExoArrayList<CResRef> lstModels;
    CExoArrayList<unsigned char> lstRotating;
    CExoString sName;
    CResStruct structTop;
    CResStruct structMiniGame;
    CResStruct structModel;
    CResList list;
    CResList listModels;
    int bSuccess;

    if (m_pRes->Demand() == NULL)
        return 0;
    m_pRes->GetTopLevelStruct(&structTop);
    m_pRes->GetStructFromStruct(&structMiniGame, &structTop, "MiniGame");
    int nType = m_pRes->ReadFieldDWORD(&structMiniGame, "Type", bSuccess, 0);
    if (nType == ATTACH_PLAYER_MODEL || nType == ATTACH_CAMERA_HOOK)
        m_nAttachType = nType;
    float fDefaultSpeed;
    if (m_nAttachType == ATTACH_PLAYER_MODEL)
        fDefaultSpeed = 6.0f;
    else if (m_nAttachType == ATTACH_CAMERA_HOOK)
        fDefaultSpeed = 90.0f;
    else
        fDefaultSpeed = 0.0f;
    m_fAxisScale = m_pRes->ReadFieldFLOAT(&structMiniGame, "MovementPerSec",
                                         bSuccess, fDefaultSpeed);
    m_fLateralAccel = m_pRes->ReadFieldFLOAT(&structMiniGame, "LateralAccel",
                                            bSuccess, 60.0f);
    int nBumpPlane = m_pRes->ReadFieldDWORD(&structMiniGame, "Bump_Plane", bSuccess, 0);
    if (nBumpPlane >= BUMP_NONE && nBumpPlane <= BUMP_Z)
        m_nBumpPlane = nBumpPlane;
    m_bDoBumping = m_pRes->ReadFieldBYTE(&structMiniGame, "DoBumping", bSuccess, 0);
    m_bUseInertia = m_pRes->ReadFieldBYTE(&structMiniGame, "UseInertia", bSuccess, 0);
    int nDOF = m_pRes->ReadFieldDWORD(&structMiniGame, "DOF", bSuccess, 0);
    if ((nDOF & ~DOF_VALID_MASK) == 0)
        m_nDOF = nDOF;
    {
        CResRef resDefault("");
        m_cAmbientSoundResRef = m_pRes->ReadFieldCResRef(&structMiniGame, "Music",
                                                       bSuccess, resDefault);
    }
    SetClipDist(m_pRes->ReadFieldFLOAT(&structMiniGame, "Near_Clip", bSuccess, 0.1f),
                m_pRes->ReadFieldFLOAT(&structMiniGame, "Far_Clip", bSuccess, 100.0f));
    float fViewAngle = m_pRes->ReadFieldFLOAT(&structMiniGame, "CameraViewAngle",
                                             bSuccess, 65.0f);
    m_fViewAngle = fViewAngle;
    if (m_pCam != NULL)
        m_pCam->SetViewAngle(fViewAngle);
    bSuccess = m_pRes->GetStructFromStruct(&structTop, &structMiniGame, "Player");
    if (!bSuccess)
        return 0;
    bSuccess = m_pRes->GetList(&list, &structTop, "Models");
    if (!bSuccess)
        return 0;
    int nCount = m_pRes->GetListCount(&list);
    for (int i = 0; i < nCount; ++i)
    {
        m_pRes->GetListElement(&structModel, &list, i);
        cResRef = m_pRes->ReadFieldCResRef(&structModel, "Model", bSuccess, "");
        if (cResRef.IsValid())
        {
            lstModels.Add(cResRef);
            lstRotating.Add(m_pRes->ReadFieldBYTE(&structModel, "RotatingModel", bSuccess, 1));
        }
    }
    if (lstModels.num == 0)
        return 0;
    cResRef = m_pRes->ReadFieldCResRef(&structTop, "Track", bSuccess, "");
    if (!cResRef.IsValid())
        return 0;
    CSWMiniPlayer* pPlayer;
    if (m_nAttachType == ATTACH_CAMERA_HOOK)
    {
        CResRef cCamera;
        cCamera = m_pRes->ReadFieldCResRef(&structTop, "Camera", bSuccess, "");
        if (!cCamera.IsValid())
            return 0;
        int bCameraRotate = m_pRes->ReadFieldBYTE(&structTop, "CameraRotate", bSuccess, 0);
        pPlayer = SetPlayer(lstModels, lstRotating, cResRef, cCamera, bCameraRotate);
    }
    else
        pPlayer = SetPlayer(lstModels, lstRotating, cResRef);
    lstModels.SetSize(0);
    lstRotating.SetSize(0);
    if (pPlayer == NULL)
        return 0;
    pPlayer->Load(m_pRes, &structTop);

    bSuccess = m_pRes->GetStructFromStruct(&structTop, &structMiniGame, "Mouse");
    if (bSuccess)
    {
        SetMouseAxisX(m_pRes->ReadFieldDWORD(&structTop, "AxisX", bSuccess, 0));
        SetMouseAxisY(m_pRes->ReadFieldDWORD(&structTop, "AxisY", bSuccess, 0));
        if (m_pRes->ReadFieldBYTE(&structTop, "FlipAxisX", bSuccess, 0))
            m_nMouseAxisX = -m_nMouseAxisX;
        if (m_pRes->ReadFieldBYTE(&structTop, "FlipAxisY", bSuccess, 0))
            m_nMouseAxisY = -m_nMouseAxisY;
    }
    bSuccess = m_pRes->GetList(&list, &structMiniGame, "Enemies");
    if (bSuccess)
    {
        nCount = m_pRes->GetListCount(&list);
        for (int nEnemy = 0; nEnemy < nCount; ++nEnemy)
        {
            bSuccess = m_pRes->GetListElement(&structTop, &list, nEnemy);
            if (bSuccess)
            {
                bSuccess = m_pRes->GetList(&listModels, &structTop, "Models");
                if (bSuccess)
                {
                    int nModels = m_pRes->GetListCount(&listModels);
                    for (int nModel = 0; nModel < nModels; ++nModel)
                    {
                        m_pRes->GetListElement(&structModel, &listModels, nModel);
                        cResRef = m_pRes->ReadFieldCResRef(&structModel, "Model", bSuccess, "");
                        if (cResRef.IsValid())
                        {
                            lstModels.Add(cResRef);
                            lstRotating.Add(m_pRes->ReadFieldBYTE(&structModel, "RotatingModel", bSuccess, 1));
                        }
                    }
                    if (lstModels.num != 0)
                    {
                        cResRef = m_pRes->ReadFieldCResRef(&structTop, "Track", bSuccess, "");
                        if (!cResRef.IsValid())
                        {
                            lstModels.SetSize(0);
                            lstRotating.SetSize(0);
                        }
                        else
                        {
                            CSWMiniEnemy* pEnemy = AddEnemy(lstModels, lstRotating, cResRef);
                            lstModels.SetSize(0);
                            lstRotating.SetSize(0);
                            if (pEnemy != NULL)
                                pEnemy->Load(m_pRes, &structTop);
                        }
                    }
                }
            }
        }
    }
    bSuccess = m_pRes->GetList(&list, &structMiniGame, "Obstacles");
    if (bSuccess)
    {
        int nObstacles = m_pRes->GetListCount(&list);
        for (int nObstacle = 0; nObstacle < nObstacles; ++nObstacle)
        {
            bSuccess = m_pRes->GetListElement(&structTop, &list, nObstacle);
            if (bSuccess)
            {
                cResRef = m_pRes->ReadFieldCResRef(&structTop, "Name", bSuccess, "");
                if (cResRef.IsValid())
                {
                    cResRef.CopyToString(sName);
                    CSWMGObstacle* pObstacle = (CSWMGObstacle*)g_pAppManager->GetClientExoApp()->GetMGOArray()->GetMiniGameObjectByName(sName);
                    if (pObstacle != NULL)
                        pObstacle->Load(m_pRes, &structTop);
                }
            }
        }
    }
    m_pRes->Release();
    return 1;
}
// PC 0x006C7CB0 GOG 0x006C79B0
CSWMiniGame::~CSWMiniGame()
{
    if (m_pPlayer != NULL)
    {
        delete m_pPlayer;
    }
    m_pPlayer = NULL;
    SetCamera(m_pCam);
    ClearEnemies();
    ClearTracks();
    ClearObstacles();
    delete m_pImpactSound;
}
// PC 0x006C7DA0 GOG 0x006C7AA0
void CSWMiniGame::AddBullet(CAurObject* pBullet)
{
    m_lstBullets.Add(new SafePointer<CAurObject>(pBullet));
}
// PC 0x006C7E50 GOG 0x006C7B50
SWMGExplodeBullet::SWMGExplodeBullet()
{
}
// PC 0x006C81F0 GOG 0x006C7EF0
void CSWMiniGame::DoHitCheck()
{
    int nBullets = m_lstBullets.num;
    CSWMiniGameObjectArray* pArray =
        g_pAppManager->GetClientExoApp()->GetMGOArray();
    for (int nBullet = nBullets - 1; nBullet >= 0; --nBullet)
    {
        int bHit = 0;
        CAurObject* pBullet = *m_lstBullets.element[nBullet];
        if (pBullet != NULL)
        {
            if (m_pPlayer != NULL && m_pPlayer->DoBulletHitCheck(pBullet))
            {
                delete m_lstBullets.element[nBullet];
                m_lstBullets.DelIndex(nBullet);
                bHit = 1;
            }
            else
            {
                int nEnemies = m_lstEnemyIds.num;
                for (int nEnemy = nEnemies - 1; nEnemy >= 0; --nEnemy)
                {
                    CSWTrackFollower* pEnemy =
                        (CSWTrackFollower*)pArray->GetMiniGameObject(
                            m_lstEnemyIds.element[nEnemy], SWMG_OBJECT_ENEMY);
                    if (pEnemy != NULL)
                    {
                        if (pEnemy->DoBulletHitCheck(pBullet))
                        {
                            delete m_lstBullets.element[nBullet];
                            m_lstBullets.DelIndex(nBullet);
                            bHit = 1;
                            break;
                        }
                    }
                    else
                    {
                        m_lstEnemyIds.DelIndex(nEnemy);
                    }
                }

                if (!bHit)
                {
                    int nObstacles = m_lstObstacleIds.num;
                    for (int nObstacle = nObstacles - 1; nObstacle >= 0 && !bHit;
                         --nObstacle)
                    {
                        CSWMGObstacle* pObstacle =
                            (CSWMGObstacle*)pArray->GetMiniGameObject(
                                m_lstObstacleIds.element[nObstacle],
                                SWMG_OBJECT_OBSTACLE);
                        if (pObstacle != NULL)
                        {
                            if (HitCheckBullet(pBullet, pObstacle->m_pModel,
                                               pObstacle))
                            {
                                pObstacle->OnHitBullet(0);
                                delete m_lstBullets.element[nBullet];
                                m_lstBullets.DelIndex(nBullet);
                                bHit = 1;
                                pObstacle->m_pModel->DisableAlwaysRender();
                                delete pObstacle;
                                m_lstObstacleIds.DelIndex(nObstacle);
                                break;
                            }
                        }
                        else
                        {
                            m_lstObstacleIds.DelIndex(nObstacle);
                        }
                    }
                }
            }

            int nCompare = g_pAppManager->GetClientExoApp()->GetModule()
                ->GetModuleTitle().CompareNoCase("107PER");
            if (nCompare && !bHit)
            {
                if (m_nAttachType == ATTACH_CAMERA_HOOK)
                {
                    Vector vPosition = pBullet->GetPosition();
                    Vector vPrevious = pBullet->GetPreviousPosition();
                    Vector vHit(0.0f, 0.0f, 0.0f);
                    CHitInfo cHitInfo(0, CHitInfo::HIT_ALL_NON_TRIGGER, 0);
                    if (vPrevious.m_fX != 0.0 || vPrevious.m_fY != 0.0 ||
                        vPrevious.m_fZ != 0.0)
                    {
                        cHitInfo.m_nType = 1;
                        CAurScene* pScene = g_pAppManager->GetClientExoApp()
                            ->GetModule()->GetArea()->m_pScene;
                        if (SceneHitCheck(pScene, vPosition, vPrevious, &vHit,
                                          &cHitInfo))
                        {
                            delete pBullet;
                            delete m_lstBullets.element[nBullet];
                            m_lstBullets.DelIndex(nBullet);
                            bHit = 1;
                        }
                    }
                }

                if (!bHit && m_nAttachType == ATTACH_CAMERA_HOOK)
                {
                    g_pAppManager->GetClientExoApp()->GetGameObjectArray();
                    CSWSArea* pArea =
                        g_pAppManager->GetServerExoApp()->GetModule()->GetArea();
                    unsigned long oidObject;
                    int bMore = pArea->GetFirstObjectInArea(oidObject);
                    while (bMore)
                    {
                        CSWMGBehaviorBullet* pBehavior =
                            (CSWMGBehaviorBullet*)pBullet->GetBehavior(
                                AUR_BEHAVIOR_BULLET);
                        if (pBehavior == NULL)
                            break;

                        unsigned long oidClient = g_pAppManager->GetClientExoApp()
                            ->ServerToClientObjectId(oidObject);
                        CSWCObject* pObject =
                            g_pAppManager->GetClientExoApp()->GetGameObject(
                                oidClient);
                        if (pObject != NULL &&
                            (pObject->AsSWCCreature() != NULL ||
                             pObject->AsSWCPlaceable() != NULL))
                        {
                            Gob* pModel = NULL;
                            if (pObject->AsSWCCreature() != NULL)
                            {
                                CSWSCreature* pServerCreature =
                                    pObject->AsSWCCreature()->GetServerCreature();
                                if (pServerCreature != NULL)
                                {
                                    if (!pServerCreature->GetInParty())
                                    {
                                        pModel = pObject->AsSWCCreature()->GetModel(0xff, 1);
                                    }
                                }
                                else
                                {
                                    pModel = pObject->AsSWCCreature()->GetModel(0xff, 1);
                                }
                            }
                            else
                            {
                                pModel = pObject->AsSWCPlaceable()->GetModel(0xff, 1);
                            }

                            if (pModel != NULL)
                            {
                                CHitInfo cHitInfo(0, CHitInfo::HIT_ALL_NON_TRIGGER, 0);
                                cHitInfo.m_nMask = 1;
                                if (pModel->HitCheck(pBullet->GetPreviousPosition(),
                                                     pBullet->GetPosition(), NULL,
                                                     &cHitInfo))
                                {
                                    CScriptEvent* pEvent = new CScriptEvent;
                                    pEvent->m_nType =
                                        CSWSSCRIPTEVENT_EVENTTYPE_ON_USER_DEFINED_EVENT;
                                    pEvent->SetInteger(0, pBehavior->m_bullet.m_nDamage);
                                    g_pAppManager->GetServerExoApp()
                                        ->GetServerAIMaster()->AddEventDeltaTime(
                                            0, 0, oidObject, oidObject,
                                            CServerAIMaster::EVENT_SIGNAL_EVENT,
                                            pEvent);
                                    delete pBullet;
                                    delete m_lstBullets.element[nBullet];
                                    m_lstBullets.DelIndex(nBullet);
                                    break;
                                }
                            }
                        }
                        bMore = pArea->GetNextObjectInArea(oidObject);
                    }
                }
            }
        }
        else
        {
            delete m_lstBullets.element[nBullet];
            m_lstBullets.DelIndex(nBullet);
        }
    }

    for (int nObstacle = m_lstObstacleIds.num - 1; nObstacle >= 0; --nObstacle)
    {
        if (m_pPlayer != NULL && !m_pPlayer->m_bDying)
        {
            CSWMGObstacle* pObstacle =
                (CSWMGObstacle*)pArray->GetMiniGameObject(
                    m_lstObstacleIds.element[nObstacle], SWMG_OBJECT_OBSTACLE);
            if (pObstacle != NULL)
            {
                m_pPlayer->DoObstacleHitCheck(pObstacle);
            }
            else
            {
                m_lstObstacleIds.DelIndex(nObstacle);
            }
        }
    }

    if (m_pPlayer != NULL && !m_pPlayer->m_bDying)
    {
        for (int nPass = 0; nPass < 3; ++nPass)
        {
            if (m_pPlayer != NULL &&
                !m_pPlayer->DoFollowerHitCheck(m_lstEnemyIds, nPass == 0))
            {
                break;
            }
        }
    }
}
// PC 0x006C7E70 GOG 0x006C7B70
int CSWMiniGame::HitCheckBullet(CAurObject* pBullet, CAurObject* pTarget,
                               CSWMiniGameObject* pHit)
{
    CSWMGBehaviorBullet* pBehavior =
        (CSWMGBehaviorBullet*)pBullet->GetBehavior(AUR_BEHAVIOR_BULLET);
    if (pBehavior == NULL)
        return 0;

    CAurPart* pPart = pTarget->GetPart("root");
    if (pPart == NULL)
        return 0;

    if (pTarget->HitCheckPart(pPart, pBullet->GetPreviousPosition(),
                              pBullet->GetPosition(), NULL, &DefaultHitInfo))
    {
        pHit->SetBullet(pBehavior->m_bullet);
        pHit->m_nLastBulletHitShooter = pBehavior->m_pFollower->m_nObjectId;
        pHit->m_sLastBulletHitPart = DefaultHitInfo.m_sOutputPartName;
        pHit->m_vSoundPosition = DefaultHitInfo.m_vOutputPosition;
        pHit->OnHitBullet(0);

        if (!g_pAppManager->GetClientExoApp()->GetModule()->GetModuleTitle().CompareNoCase("505OND") &&
            !g_pAppManager->GetClientExoApp()->GetModule()->GetModuleTitle().CompareNoCase("module000"))
        {
            if (pBullet->PlayAnimation("explode", 1.0f, 3, 0.0f))
            {
                SWMGExplodeBullet* pEffect = new SWMGExplodeBullet;
                pEffect->m_pObject = pBullet;
                pEffect->m_qOrientation = pointto(DefaultHitInfo.m_vOutputNormal);
                pEffect->m_qOrientation = Quaternion(Vector(1.0f, 0.0f, 0.0f),
                                                     1.5707963267948966f) *
                                          pEffect->m_qOrientation;
                pEffect->m_vPosition = DefaultHitInfo.m_vOutputPosition;
                pBullet->SetBehavior(NULL);
                pBullet->RegisterCallBack(BulletDie, "doneexplode", NULL,
                                          AUR_CALLBACK_LONG_DELAY, false);
                m_lstEffects.Add(pEffect);
            }
            else
            {
                delete pBullet;
            }
        }
        else
        {
            delete pBullet;
        }
        return 1;
    }
    return 0;
}
// PC 0x006C8AD0 GOG 0x006C87D0
void CSWMiniGame::Update(float fTimeStep)
{
    CSWMiniGameObjectArray* pArray =
        g_pAppManager->GetClientExoApp()->GetMGOArray();
    for (int i = 0; i < 255; ++i)
    {
        CSWMiniGameObject* pObject =
            (CSWMiniGameObject*)pArray->GetMiniGameObject(i, SWMG_OBJECT_ANY);
        if (pObject != NULL && pObject->m_bCreated)
        {
            delete pObject;
        }
    }

    if (m_pPlayer != NULL)
    {
        m_pPlayer->Update(fTimeStep);
    }
    for (int nEnemy = m_lstEnemyIds.num - 1; nEnemy >= 0; --nEnemy)
    {
        CSWMiniEnemy* pEnemy = (CSWMiniEnemy*)pArray->GetMiniGameObject(
            m_lstEnemyIds.element[nEnemy], SWMG_OBJECT_ENEMY);
        if (pEnemy != NULL)
        {
            pEnemy->Update(fTimeStep);
        }
        else
        {
            m_lstEnemyIds.DelIndex(nEnemy);
        }
    }
    for (int nObstacle = m_lstObstacleIds.num - 1; nObstacle >= 0; --nObstacle)
    {
        CSWMiniGameObject* pObstacle =
            (CSWMiniGameObject*)pArray->GetMiniGameObject(
                m_lstObstacleIds.element[nObstacle], SWMG_OBJECT_OBSTACLE);
        if (pObstacle != NULL)
        {
            pObstacle->Update(fTimeStep);
        }
        else
        {
            m_lstObstacleIds.DelIndex(nObstacle);
        }
    }
    for (int nEffect = m_lstEffects.num - 1; nEffect >= 0; --nEffect)
    {
        SWMGExplodeBullet* pEffect = m_lstEffects.element[nEffect];
        Gob* pBullet = static_cast<Gob*>((CAurObject*)pEffect->m_pObject);
        pBullet->SetPosition(pEffect->m_vPosition);
        pBullet->SetOrientation(pEffect->m_qOrientation);
        delete pEffect;
        m_lstEffects.DelIndex(nEffect);
    }

    if (m_pPlayer != NULL && m_pPlayer->m_bTrackPlaying)
    {
        DoHitCheck();
    }
    if (m_pPlayer != NULL && m_pPlayer->m_lstModels.num != 0)
    {
        CSWCRoom* pRoom = m_pArea->GetRoom(m_pPlayer->GetPosition());
        if (pRoom != NULL)
        {
            m_pCurrentRoom = pRoom;
        }
    }
    if (m_pArea != NULL && m_pArea->m_pScene != NULL &&
        m_pCurrentRoom != NULL)
    {
        m_pArea->m_pScene->SetCurrentRoom(m_pCurrentRoom->m_nRoomIndex);
    }
}
// PC 0x006C7150 GOG 0x006C6E50
int CSWMiniGame::ClearObstacles()
{
    int nOldCount = m_lstObstacleIds.num;
    int nCount = nOldCount;
    CSWMiniGameObjectArray* pArray =
        g_pAppManager->GetClientExoApp()->GetMGOArray();
    int i = 0;
    if (nCount > 0)
    {
        do
        {
            CSWMGObstacle* pObstacle =
                (CSWMGObstacle*)pArray->GetMiniGameObject(
                    m_lstObstacleIds.element[i], 3);
            if (pObstacle != NULL)
            {
                delete pObstacle;
            }
            ++i;
        } while (i < nCount);
        nOldCount = nCount;
    }
    m_lstObstacleIds.SetSize(0);
    return nOldCount;
}

static inline void SwminigameTrace1()
{
}

#ifdef _XBOX

void CSWMiniGame::UpdateAxis(float fX, float fZ, float fTimeStep)
{
    Vector vAxis(fX, 0.0f, fZ);
    UpdateAxis(vAxis, fTimeStep);
}
#endif
