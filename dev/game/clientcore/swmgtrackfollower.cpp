#include "swminigame.h"

#include "swminigame.h"
#include "nwcanimbase.h"
#include "exobase.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "aurmodel.h"
#include "auremitter.h"
#include "aurscene.h"
#include "aurlightmanager.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static inline void SwmgtrackfollowerTrace4();
static inline void SwmgtrackfollowerTrace3();
static inline void SwmgtrackfollowerTrace2();
static inline void SwmgtrackfollowerTrace1();

float fInertiaScale = 1.0f;

extern float fInertiaScale;

void CSWTrackFollower::AttachGuns()
{
    int nModels = m_lstModels.num;
    for (int i = 0; i < nModels; i++)
    {
        AttachGuns(m_lstModels.element[i].m_pModel);
    }
}
// PC 0x006C1820
void SetInitialHook(CAurObject* pObject, char* sEvent, void* pUserData)
{
    Vector vPosition;
    Quaternion qOrientation;
    static_cast<Gob*>(pObject)
        ->GetPart("modelhook")
        ->GetWorld(vPosition, qOrientation);
    static_cast<CSWMiniPlayer*>(pUserData)->m_vInitialHook = vPosition;
}
// PC 0x006C18A0
int EnumGunAttachPoints(CAurPart* pPart)
{
    return strnicmp(pPart->Name(), "gunbank", 7) == 0;
}
// PC 0x006C18C0
void CSWTrackFollower::LoadScripts(CResGFF* pGFF, CResStruct* pStruct,
                                   CResStruct& structScripts)
{
    int bSuccess;

    CSWMiniGameObject::LoadScripts(pGFF, pStruct, structScripts);
    {
        CResRef resDefault("");
        SetScriptName(5, pGFF->ReadFieldCResRef(&structScripts, "OnDamage",
                                                bSuccess, resDefault));
    }
    {
        CResRef resDefault("");
        SetScriptName(6, pGFF->ReadFieldCResRef(&structScripts, "OnDeath",
                                                bSuccess, resDefault));
    }
    {
        CResRef resDefault("");
        SetScriptName(7, pGFF->ReadFieldCResRef(&structScripts, "OnFire",
                                                bSuccess, resDefault));
    }
    {
        CResRef resDefault("");
        SetScriptName(8, pGFF->ReadFieldCResRef(&structScripts, "OnHitObstacle",
                                                bSuccess, resDefault));
    }
    {
        CResRef resDefault("");
        SetScriptName(9, pGFF->ReadFieldCResRef(&structScripts, "OnTrackLoop",
                                                bSuccess, resDefault));
    }
}
// PC 0x006C1A90
float CSWTrackFollower::SetSpeed(float fSpeed)
{
    m_fTrackSpeed = fSpeed;
    if (m_pTrack != 0 && m_bTrackPlaying)
    {
        int nAnimationFlags;
        if (m_nNumLoops == 1)
        {
            nAnimationFlags = 17;
        }
        else
        {
            nAnimationFlags = 20;
        }
        m_pTrack->PlayAnimation("track", m_fTrackSpeed / 100.0f, nAnimationFlags, 0.0f);
    }
    return m_fTrackSpeed;
}
// PC 0x006C1AF0
float CSWTrackFollower::SetSphereRadius(float fRadius)
{
    if (fRadius >= 0.0f)
    {
        m_fSphereRadius = fRadius;
    }
    return m_fSphereRadius;
}
// PC 0x006C1B20
int CSWTrackFollower::SetHitPoints(int nHitPoints)
{
    if (nHitPoints > 0)
    {
        m_nHitPoints = nHitPoints;
    }
    return m_nHitPoints;
}
// PC 0x006C1B40
float CSWTrackFollower::SetInvincibility(float fTime)
{
    if (fTime >= 0.0f)
    {
        m_fInvincePeriod = fTime;
    }
    return m_fInvincePeriod;
}
// PC 0x006C1B70
int CSWTrackFollower::SetMaxHitPoints(int nHitPoints)
{
    if (nHitPoints > 0)
    {
        m_nMaxHPs = nHitPoints;
    }
    return m_nMaxHPs;
}
// PC 0x006C1B90
CResRef CSWTrackFollower::GetScriptName(int nScript)
{
    if (nScript < 0 || nScript >= NUM_SCRIPTS)
    {
        return CResRef();
    }
    return m_aScripts[nScript];
}
// PC 0x006C1BE0
void CSWTrackFollower::SetScriptName(int nScript, const CResRef& resRef)
{
    if (nScript < 0 || nScript >= NUM_SCRIPTS)
    {
        return;
    }
    m_aScripts[nScript] = resRef;
}
// PC 0x006C1C10
int CSWTrackFollower::GetSoundVolume(int nSound)
{
    if (nSound < 0 || nSound >= 2)
    {
        return 0;
    }
    return m_aSounds[nSound].m_nFrequency;
}
// PC 0x006C1C30
void CSWTrackFollower::SetSoundFrequency(int nSound, int nFrequency)
{
    if (nSound >= 0 && nSound < 2)
    {
        m_aSounds[nSound].m_nFrequency = nFrequency;
    }
}
// PC 0x006C1C50
int CSWTrackFollower::GetSoundFrequencyIsRandom(int nSound)
{
    if (nSound < 0 || nSound >= 2)
    {
        return 0;
    }
    return m_aSounds[nSound].m_bFrequencyIsRandom;
}
// PC 0x006C1C70
void CSWTrackFollower::SetSoundFrequencyIsRandom(int nSound, int bRandom)
{
    if (nSound >= 0 && nSound < 2)
    {
        m_aSounds[nSound].m_bFrequencyIsRandom = bRandom;
    }
}
// PC 0x006C1C90
int CSWTrackFollower::GetSoundFrequency(int nSound)
{
    if (nSound < 0 || nSound >= 2)
    {
        return 0;
    }
    return m_aSounds[nSound].m_nVolume;
}
// PC 0x006C1CB0
void CSWTrackFollower::SetSoundVolume(int nSound, int nVolume)
{
    if (nSound >= 0 && nSound < 2)
    {
        m_aSounds[nSound].m_nVolume = nVolume;
        if (nSound < 1)
        {
            m_apSoundSources[nSound]->SetVolume(nVolume);
        }
    }
}
// PC 0x006C1CF0
CExoString CSWTrackFollower::GetName()
{
    if (m_pTrack != 0)
    {
        return CExoString(m_pTrack->GetModelName());
    }
    return CExoString("");
}
// PC 0x006C1D30
int CSWTrackFollower::GetLastObstacleHit()
{
    CClientExoApp* pApp = g_pAppManager->m_pClientExoApp;
    int nResult;

    nResult = (int)pApp->GetMGOArray()->GetMiniGameObject(m_nLastObstacleHit, SWMG_OBJECT_OBSTACLE);
    if (nResult == 0)
    {
        m_nLastObstacleHit = 255;
    }
    return nResult;
}
// PC 0x006C1D60
void CSWMiniPlayer::KeepInTunnel(Vector& vPosition)
{
    if (m_vTunnelWrap.m_fX != 0.0f)
    {
        if (m_pMiniGame->m_nAttachType == CSWMiniGame::ATTACH_CAMERA_HOOK)
        {
            if (vPosition.m_fX < 0.0f)
            {
                vPosition.m_fX += 360.0f;
            }
            else if (vPosition.m_fX > 359.0f)
            {
                vPosition.m_fX -= 360.0f;
            }
        }
    }
    else if (m_vTunnelMinimum.m_fX + m_vOrigin.m_fX < vPosition.m_fX)
    {
        vPosition.m_fX = m_vTunnelMinimum.m_fX + m_vOrigin.m_fX;
    }
    else if (m_vTunnelMaximum.m_fX + m_vOrigin.m_fX > vPosition.m_fX)
    {
        vPosition.m_fX = m_vTunnelMaximum.m_fX + m_vOrigin.m_fX;
    }

    if (m_vTunnelWrap.m_fY != 0.0f)
    {
        if (m_pMiniGame->m_nAttachType == CSWMiniGame::ATTACH_CAMERA_HOOK)
        {
            if (vPosition.m_fY < 0.0f)
            {
                vPosition.m_fY += 360.0f;
            }
            else if (vPosition.m_fY > 359.0f)
            {
                vPosition.m_fY -= 360.0f;
            }
        }
    }
    else if (m_vTunnelMinimum.m_fY + m_vOrigin.m_fY < vPosition.m_fY)
    {
        vPosition.m_fY = m_vTunnelMinimum.m_fY + m_vOrigin.m_fY;
    }
    else if (m_vTunnelMaximum.m_fY + m_vOrigin.m_fY > vPosition.m_fY)
    {
        vPosition.m_fY = m_vTunnelMaximum.m_fY + m_vOrigin.m_fY;
    }

    if (m_vTunnelWrap.m_fZ != 0.0f)
    {
        if (m_pMiniGame->m_nAttachType == CSWMiniGame::ATTACH_CAMERA_HOOK)
        {
            if (vPosition.m_fZ < 0.0f)
            {
                vPosition.m_fZ += 360.0f;
            }
            else if (vPosition.m_fZ > 359.0f)
            {
                vPosition.m_fZ -= 360.0f;
            }
        }
    }
    else if (m_vTunnelMinimum.m_fZ + m_vOrigin.m_fZ < vPosition.m_fZ)
    {
        vPosition.m_fZ = m_vTunnelMinimum.m_fZ + m_vOrigin.m_fZ;
    }
    else if (m_vTunnelMaximum.m_fZ + m_vOrigin.m_fZ > vPosition.m_fZ)
    {
        vPosition.m_fZ = m_vTunnelMaximum.m_fZ + m_vOrigin.m_fZ;
    }
}
// PC 0x006C1F10
Vector CSWMiniPlayer::SetOffset(const Vector& vOffset)
{
    m_vOffset = vOffset;
    KeepInTunnel(m_vOffset);
    return m_vOffset;
}

Vector CSWMiniPlayer::DoInertia(const Vector& vPrevious,
                               const Vector& vCurrent,
                               const Quaternion& qOrientation, float fTime)
{
    Vector vPlane = m_pMiniGame->BumpPlane(qOrientation);
    Vector vVelocity = (vCurrent - vPrevious) / fTime;
    Vector vChange = m_vInertia - vVelocity;
    vChange -= (vChange * vPlane) * vPlane;
    m_vInertia = vVelocity;
    float fScale = fInertiaScale;
    return fScale * (-qOrientation * vChange);
}
// PC 0x006C1F50
void CSWMiniPlayer::AdjustPosition(int nPosition, int bRelative)
{
    if (bRelative)
    {
        m_nBankPosition += nPosition;
    }
    else
    {
        if (m_nBankPosition == nPosition)
        {
            return;
        }
        m_nBankPosition = nPosition;
    }

    if (m_nBankPosition < -10)
    {
        m_nBankPosition = -10;
    }
    else if (m_nBankPosition > 10)
    {
        m_nBankPosition = 10;
    }

    char szAnimation[12];
    if (m_nBankPosition != 0)
    {
        sprintf(szAnimation, "Bank%c_%02d", m_nBankPosition < 0 ? 'L' : 'R',
                abs(m_nBankPosition));
    }
    else
    {
        sprintf(szAnimation, "Ready_01");
    }
    PlayAnimation(szAnimation, 1.0f, 16);
}
// PC 0x006C2030
int CSWMiniPlayer::SetLooping(int nLooping)
{
    int nResult = nLooping;
    if (nLooping >= -1 && nLooping != 0)
    {
        m_nNumLoops = nLooping;
    }
    return nResult;
}
// PC 0x006C2050
float CSWMiniPlayer::SetMinSpeed(float fSpeed)
{
    if (fSpeed >= 0.0f)
    {
        m_fMinimumSpeed = fSpeed;
    }
    return m_fMinimumSpeed;
}
// PC 0x006C2070
float CSWMiniPlayer::SetMaxSpeed(float fSpeed)
{
    if (fSpeed >= 0.0f)
    {
        m_fMaximumSpeed = fSpeed;
    }
    return m_fMaximumSpeed;
}
// PC 0x006C20A0
float CSWMiniPlayer::SetAPS(float fAPS)
{
    if (fAPS >= 0.0f)
    {
        m_fAPS = fAPS;
    }
    return m_fAPS;
}
// PC 0x006C20C0
Vector CSWMiniPlayer::SetOrigin(const Vector& vOrigin)
{
    m_vOrigin = vOrigin;
    if (!m_bTrackPlaying)
    {
        m_vOffset = vOrigin;
    }
    return m_vOrigin;
}
// PC 0x006C2120
void CSWMiniPlayer::SetCameraHook(CAurObject* pCameraHook,
                                  unsigned char bAttached)
{
    int nAttachType;
    if (bAttached != 0)
    {
        nAttachType = m_pMiniGame->GetAttachType();
    }
    else
    {
        nAttachType = 0;
    }

    if (m_pCameraHook != 0)
    {
        delete m_pCameraHook;
    }
    m_pCameraHook = static_cast<Gob*>(pCameraHook);
    m_bCameraHookAttached = bAttached;

    if (m_pCameraHook != 0 && m_pTrack != 0)
    {
        m_pCameraHook->AttachToObject(m_pTrack, "modelhook", nAttachType);
    }
}
// PC 0x006C2180
int CSWMiniEnemy::SetLooping(int nLooping)
{
    int nResult = abs(nLooping);
    if (nResult == 1)
    {
        m_nNumLoops = nLooping;
    }
    return nResult;
}
// PC 0x006C21A0
int CSWMGObstacle::Load(CResGFF* pGFF, CResStruct* pStruct)
{
    CResStruct scripts;
    if (pGFF->GetStructFromStruct(&scripts, pStruct, "Scripts"))
    {
        LoadScripts(pGFF, pStruct, scripts);
    }
    OnCreate();
    return 1;
}
// PC 0x006C21F0
CResRef CSWMGObstacle::GetScriptName(int nScript)
{
    if (nScript < 0 || nScript >= 5)
    {
        return CResRef();
    }
    return m_aScripts[nScript];
}
// PC 0x006C2240
void CSWMGObstacle::SetScriptName(int nScript, const CResRef& resRef)
{
    if (nScript < 0 || nScript >= 5)
    {
        return;
    }
    m_aScripts[nScript] = resRef;
}
// PC 0x006C2270
void CSWMGObstacle::LoadScripts(CResGFF* pGFF, CResStruct* pStruct,
                                CResStruct& structScripts)
{
    CSWMiniGameObject::LoadScripts(pGFF, pStruct, structScripts);
}

int bDoAcceleration = 1;
float fSpeed = 100.0f;
float fLastSpeedSet = 100.0f;
// PC 0x006C23C0
CSWMGTargettingGunBank* CSWTrackFollower::GetGunBank(int nGunBank)
{
    if (nGunBank < 0 || nGunBank >= m_lstGunBanks.num)
    {
        return 0;
    }
    return (CSWMGTargettingGunBank*)m_lstGunBanks[nGunBank];
}
// PC 0x006C23E0
Gob* CSWTrackFollower::GetModel(int nIndex)
{
    if (nIndex >= m_lstModels.num)
    {
        return 0;
    }
    return m_lstModels[nIndex].m_pModel;
}

void OnReportKey(CAurObject* pObject, char* sEvent, void* pUserData)
{
    CSWMiniGame* pMiniGame = g_pAppManager->m_pClientExoApp->GetMiniGame();
    if (pMiniGame->m_bUseInertia)
    {
        SWMGReportKeyData* pData = (SWMGReportKeyData*)pUserData;
        CSWMiniPlayer* pPlayer = pMiniGame->m_pPlayer;
        SWMGKeyInfo key;
        key.m_fTime = pData->m_fTime;
        key.m_bPosition = pData->m_pPosition != 0;
        key.m_bOrientation = pData->m_pOrientation != 0;
        if (pData->m_pOrientation != 0)
        {
            key.m_qOrientation = *pData->m_pOrientation;
        }
        if (pData->m_pPosition != 0)
        {
            key.m_vPosition = *pData->m_pPosition;
        }
        if (pData->m_bPositionKey)
        {
            pPlayer->m_lstPositionKeys.Add(key);
        }
        else
        {
            pPlayer->m_lstOrientationKeys.Add(key);
        }
    }
}

void CSWTrackFollower::UpdateGunBanks()
{
    if (m_bDying)
    {
        int nGunBanks = m_lstGunBanks.num;
        for (int i = 0; i < nGunBanks; i++)
        {
            m_lstGunBanks.element[i]->m_pFollower = 0;
        }
    }
}
// PC 0x006C2B00
void TrackFollowerPartDie(CAurObject* pObject, char* sEvent, void* pUserData)
{
    CSWTrackFollower* pFollower = (CSWTrackFollower*)pUserData;
    if (pFollower == 0)
    {
        if (pObject != 0)
        {
            delete pObject;
        }
        return;
    }
    pFollower->RemoveModel(pObject);
    if (pObject != 0)
    {
        delete pObject;
    }
    if (pFollower->m_lstModels.num == 0)
    {
        CSWMiniGame* pMiniGame = pFollower->m_pMiniGame;
        pFollower->UpdateGunBanks();
        if (pMiniGame->m_pPlayer == pFollower)
        {
            pMiniGame->SetPlayer(0);
        }
        else
        {
            pMiniGame->RemoveEnemy((CSWMiniEnemy*)pFollower);
            delete pFollower;
        }
    }
}
// PC 0x006C2400
CAurObject* CSWTrackFollower::RemoveModel(CAurObject* pModel)
{
    for (int i = 0; i < m_lstModels.num; i++)
    {
        if (m_lstModels.element[i].m_pModel == pModel)
        {
            m_lstModels.DelIndex(i);
            static_cast<Gob*>(pModel)->AttachToObject(0, 0, Gob::ATTACH_NORMAL);
            return pModel;
        }
    }
    return 0;
}
// PC 0x006C2B90
CSWTrackFollower::CSWTrackFollower(CSWMiniGame* pMiniGame, unsigned long nType)
    : CSWMiniGameObject(nType)
{
    m_pMiniGame = pMiniGame;
    m_pTrack = 0;
    m_bTrackPlaying = 0;
    m_bDying = 0;
    m_fSphereRadius = 0.0f;
    m_nBumpDamage = 0;
    m_fInvincePeriod = 0.0f;
    m_fInvincibilityRemaining = 0.0f;
    m_nDamage = 0;
    m_fTrackSpeed = 100.0f;
    m_nNumLoops = 1;
    m_nHitPoints = 100;
    m_nLastObstacleHit = 255;
    for (int i = 0; i < NUM_SOUND_SOURCES; i++)
    {
        m_apSoundSources[i] = 0;
    }
}
// PC 0x006C2CB0
CSWTrackFollower::~CSWTrackFollower()
{
    int nModels = m_lstModels.num;
    for (int i = 0; i < nModels; i++)
    {
        delete m_lstModels.element[i].m_pModel;
    }
    m_lstModels.SetSize(0);

    if (!m_bDying)
    {
        int nGunBanks = m_lstGunBanks.num;
        for (int i = nGunBanks - 1; i >= 0; i--)
        {
            delete m_lstGunBanks.element[i];
        }
    }

    if (m_apSoundSources[0] != 0)
    {
        m_apSoundSources[0]->Stop();
        delete m_apSoundSources[0];
    }

    delete m_pTrack;
}
// PC 0x006C2DC0
void CSWTrackFollower::RemoveGunBank(CSWMGGunBank* pGunBank)
{
    SwmgtrackfollowerTrace2();
    if (m_bDying)
    {
        m_lstGunBanks.Remove(pGunBank);
    }
}
// PC 0x006C2DE0
void CSWTrackFollower::Update(float fTime)
{
    SwmgtrackfollowerTrace2();
    CSWMiniGameObject::Update(fTime);
    if (m_fInvincibilityRemaining > 0.0f)
    {
        m_fInvincibilityRemaining =
            m_fInvincibilityRemaining - fTime > 0.0f
                ? m_fInvincibilityRemaining - fTime : 0.0f;
    }
    if (m_apSoundSources[0] != 0)
    {
        Vector vPosition;
        if (m_lstModels.num != 0)
        {
            vPosition = m_lstModels[0].m_pModel->GetPosition();
        }
        Vector vSoundPosition = vPosition;
        m_apSoundSources[0]->SetPosition(vSoundPosition, 0.0f);
    }
}
// PC 0x006C2EA0
int CSWTrackFollower::AddModel(CAurObject* pModel, unsigned char bAttach)
{
    if (pModel == 0)
    {
        return 0;
    }

    int nModels = m_lstModels.num;
    for (int i = 0; i < nModels; i++)
    {
        if (m_lstModels.element[i].m_pModel == pModel)
        {
            return (int)pModel;
        }
    }

    SWMGFollowerModel model;
    model.m_pModel = static_cast<Gob*>(pModel);
    model.m_bAttached = bAttach;
    m_lstModels.Add(model);
    if (m_pTrack != 0)
    {
        model.m_pModel->AttachToObject(m_pTrack, "modelhook", Gob::ATTACH_NORMAL);
    }
    return (int)pModel;
}
// PC 0x006C2F30
int CSWTrackFollower::SetTrack(CAurObject* pTrack, int bPlayAnimation)
{
    if (pTrack != m_pTrack)
    {
        return ForceSetTrack(pTrack, bPlayAnimation);
    }
    return (int)m_pTrack;
}
// PC 0x006C2470
int CSWTrackFollower::ForceSetTrack(CAurObject* pTrack, int bPlayAnimation)
{
    if (m_pTrack != 0 && m_pTrack != pTrack)
    {
        delete m_pTrack;
    }
    m_pTrack = static_cast<Gob*>(pTrack);
    m_bTrackPlaying = 0;

    if (pTrack != 0)
    {
        Part* pHook = m_pTrack->GetPart("modelhook");
        Vector vPosition;
        Quaternion qOrientation;
        pHook->GetWorld(vPosition, qOrientation);

        if (AsPlayer() != 0)
        {
            m_pTrack->RegisterCallBack(SetInitialHook, "starttrack", this,
                                       AUR_CALLBACK_LONG_DELAY, 0);
        }

        int nModels = m_lstModels.num;
        for (int i = 0; i < nModels; i++)
        {
            SWMGFollowerModel model = m_lstModels[i];
            Gob* pModel = model.m_pModel;
            if (model.m_bAttached)
            {
                pModel->AttachToObject(m_pTrack, "modelhook", bPlayAnimation);
            }
            else
            {
                pModel->AttachToObject(m_pTrack, "modelhook", Gob::ATTACH_NORMAL);
            }
            pModel->SetPosition(vPosition);
        }

        if (AsPlayer() != 0)
        {
            if (static_cast<CSWMiniPlayer*>(this)->m_pCameraHook != 0)
            {
                static_cast<CSWMiniPlayer*>(this)->m_pCameraHook->AttachToObject(
                    m_pTrack, "modelhook", bPlayAnimation);
            }
        }
    }
    return (int)m_pTrack;
}
// PC 0x006C25C0
void CSWTrackFollower::Stop()
{
    SwmgtrackfollowerTrace1();
    Gob* pTrack = m_pTrack;
    if (pTrack != 0 && m_bTrackPlaying)
    {
        int nAnimationFlags;
        if (m_nNumLoops == 1)
        {
            nAnimationFlags = 17;
        }
        else
        {
            nAnimationFlags = 20;
        }
        pTrack->PlayAnimation("track", 0.0f, nAnimationFlags, 0.0f);
        m_bTrackPlaying = 0;
        if (m_apSoundSources[0] != 0)
        {
            m_apSoundSources[0]->Stop();
        }
    }
}
// PC 0x006C2610
void CSWTrackFollower::Hit()
{
    PlayAnimation("damage", 1.0f, 1);
    PlayAnimation("Ready_01", 1.0f, 32);
    int nGunBanks = m_lstGunBanks.num;
    for (int i = 0; i < nGunBanks; i++)
    {
        m_lstGunBanks[i]->Hit();
    }
}
// PC 0x006C2660
int CSWTrackFollower::DoBulletHitCheck(CAurObject* pBullet)
{
    CSWMGBehaviorBullet* pBehavior =
        (CSWMGBehaviorBullet*)((Gob*)pBullet)->GetBehavior(AUR_BEHAVIOR_BULLET);
    if (pBehavior != 0 && pBehavior->m_pFollower == this)
    {
        return 0;
    }

    int nModels = m_lstModels.num;
    if (nModels <= 0)
    {
        return 0;
    }
    for (int i = 0; i < nModels; i++)
    {
        if (m_pMiniGame->HitCheckBullet(pBullet, m_lstModels[i].m_pModel,
                                        this))
        {
            return 1;
        }
    }
    return 0;
}
// PC 0x006C26D0
void CSWTrackFollower::PlayAnimation(char* szAnimation, float fSpeed, int nFlags)
{
    int nModels = m_lstModels.num;
    for (int i = 0; i < nModels; i++)
    {
        m_lstModels[i].m_pModel->PlayAnimation(szAnimation, fSpeed,
                                                       nFlags, 0.0f);
    }

    if (AsPlayer() != 0)
    {
        if (AsPlayer()->GetCameraHook() != 0)
        {
            AsPlayer()->GetCameraHook()->PlayAnimation(
                szAnimation, fSpeed, nFlags, 0.0f);
        }
    }
}
// PC 0x006C2F50
void CSWTrackFollower::OnDeath(int nReserved)
{
    if (nReserved == 0)
    {
        CResRef resScript = GetScriptName(SCRIPT_DEATH);
        if (resScript.IsValid())
        {
            CExoString sScript;
            resScript.CopyToString(sScript);
            g_pVirtualMachine->RunScript(&sScript, m_nObjectId, 1);
            return;
        }
    }

    Stop();
    int nSound = SOUND_DEATH;
    CResRef resSound = m_aOtherSoundNames[nSound - SOUND_DEATH];
    if (resSound.IsValid())
    {
        unsigned char nPriority = AsPlayer() ? SOUND_PRIORITY_PLAYER : SOUND_PRIORITY_ENEMY;
        if (resSound == "mgs_accelpad")
        {
            g_pExoSound->PlayOneShotSound(resSound, nPriority, 0,
                                         CExoSound::CMAX_VOLUME, 0.0f, 0.0f);
        }
        else
        {
            g_pExoSound->PlayOneShotSound(resSound, nPriority, 0,
                                         DEATH_SOUND_VOLUME, 0.0f, 0.0f);
        }
    }

    for (int i = m_lstModels.num - 1; i >= 0; i--)
    {
        Gob* pModel = m_lstModels.element[i].m_pModel;
        pModel->PlayAnimation(0, 1.0f, 0, 0.0f);
        if (pModel->PlayAnimation("die", 1.0f, 1, 0.0f))
        {
            pModel->RegisterCallBack(TrackFollowerPartDie, "donedie", this,
                                     9999.0f, 0);
        }
        else
        {
            RemoveModel(pModel);
            delete pModel;
        }
    }

    int nGunBanks = m_lstGunBanks.num;
    for (int j = 0; j < nGunBanks; j++)
    {
        m_lstGunBanks.element[j]->Die();
    }
    if (m_lstModels.num == 0)
    {
        if (GetMiniGame()->GetPlayer() == this)
        {
            GetMiniGame()->SetPlayer(0);
        }
        else
        {
            m_bCreated = 1;
        }
    }
    GetMiniGame()->RemoveEnemy((CSWMiniEnemy*)this);
}
// PC 0x006C3140
int CSWTrackFollower::AttachGuns(CAurObject* pObject)
{
    SwmgtrackfollowerTrace2();
    List<CAurPart*> parts;

    static_cast<Gob*>(pObject)->EnumParts(EnumGunAttachPoints, &parts);
    int nAttached = 0;
    for (int i = 0; i < parts.m_nCount; i++)
    {
        int nBank;
        if (sscanf(parts[i]->Name(), "gunbank%d-", &nBank) != 1)
        {
            char szError[128];
            sprintf(szError,
                    "CSWTrackFollower: Could not find bank ID in attach part name %s.",
                    parts[i]->Name());
        }
        else if (nBank < 0)
        {
            char szError[128];
            sprintf(szError, "CSWTrackFollower: Bank ID %d < 0.", nBank);
        }
        else if (nBank < m_lstGunBanks.num)
        {
            m_lstGunBanks.element[nBank]->AddGun(pObject, parts[i]);
            nAttached++;
        }
        else
        {
            char szError[128];
            sprintf(szError,
                    "CSWTrackFollower: Trying to attach gun to a non-existent bank ID: %d",
                    nBank);
        }
    }
    return nAttached;
}
// PC 0x006C3270
void CSWTrackFollower::OnHitObstacle(int nReserved)
{
    int nModels;
    int i;
    if (nReserved == 0)
    {
        CResRef resScript = GetScriptName(8);
        if (resScript.IsValid())
        {
            CExoString sScript;
            resScript.CopyToString(sScript);
            g_pVirtualMachine->RunScript(&sScript, m_nObjectId, 1);
            return;
        }
    }
    nModels = m_lstModels.num;
    for (i = 0; i < nModels; i++)
    {
        m_lstModels.element[i].m_pModel->SetBehavior(0);
    }
    if (!m_bDying)
    {
        m_bDying = 1;
        OnDeath(0);
        return;
    }
}
// PC 0x006C3370
int CSWTrackFollower::DoObstacleHitCheck(CSWMGObstacle* pObstacle)
{
    Gob* pObstacleModel = (Gob*)(CAurObject*)pObstacle->m_pModel;
    if (pObstacleModel == 0)
    {
        return 0;
    }

    Part* pHitBump = pObstacleModel->GetPart("hitbump");
    if (pHitBump == 0)
    {
        return 0;
    }

    if (m_lstModels.num == 0)
    {
        return 0;
    }

    Gob* pModel = m_lstModels.element[0].m_pModel;
    Vector vStart = pModel->GetPreviousPosition();
    Vector vEnd = pModel->GetPosition();
    if (pObstacleModel->HitCheckPart((CAurPart*)pHitBump, vStart, vEnd, 0,
                                     &DefaultHitInfo))
    {
        m_nLastObstacleHit = pObstacle->m_nObjectId;
        OnHitObstacle(0);
        pObstacle->m_nLastFollowerHit = m_nObjectId;
        pObstacle->OnHitFollower(0);
        return 1;
    }
    return 0;
}
// PC 0x006C3480
void CSWTrackFollower::OnDamage(int nReserved)
{
    if (nReserved == 0)
    {
        CResRef resScript = GetScriptName(5);
        if (resScript.IsValid())
        {
            CExoString sScript;
            resScript.CopyToString(sScript);
            g_pVirtualMachine->RunScript(&sScript, m_nObjectId, 1);
            return;
        }
    }
    {
        int nDamage = m_nDamage;
        m_nHitPoints += nDamage;
        if (nDamage < 0)
        {
            if (m_nHitPoints <= 0)
            {
                if (!m_bDying)
                {
                    m_bDying = 1;
                    OnDeath(0);
                    return;
                }
            }
            else
            {
                PlayAnimation("damage", 1.0f, 1);
                PlayAnimation("Ready_01", 1.0f, 32);
                return;
            }
        }
        else
        {
            m_nHitPoints =
                m_nHitPoints < m_nMaxHPs ? m_nHitPoints : m_nMaxHPs;
        }
    }
}
// PC 0x006C35C0
void CSWTrackFollower::AdjustHitPoints(int nHitPoints)
{
    m_nDamage = nHitPoints;
    return OnDamage(0);
}
// PC 0x006C3C80
void CSWTrackFollower::LoadSounds(CResGFF* pGFF, CResStruct* pStruct,
                                  CResStruct& structSounds)
{
    int bSuccess;

    {
        CResRef resDefault("");
        CExoSoundSource* pSource =
            SetSoundName(SOUND_ENGINE, pGFF->ReadFieldCResRef(&structSounds, "Engine",
                                                   bSuccess, resDefault));
        if (pSource != 0)
        {
            pSource->SetLooping(1);
        }
    }
    {
        CResRef resDefault("");
        SetSoundName(SOUND_DEATH, pGFF->ReadFieldCResRef(&structSounds, "Death", bSuccess,
                                               resDefault));
    }
}
// PC 0x006C35E0
CExoSoundSource* CSWTrackFollower::SetSoundName(int nSound,
                                                const CResRef& resRef)
{
    if (nSound < 0)
    {
        return 0;
    }
    if (nSound < SOUND_DEATH)
    {
        if (m_apSoundSources[nSound] != 0)
        {
            int bPlaying = m_apSoundSources[nSound]->IsHardwarePlayingSoundPleaseDontUseThis();
            if (bPlaying)
            {
                m_apSoundSources[nSound]->Stop();
            }
            if (resRef.IsValid())
            {
                m_apSoundSources[nSound]->SetResRef(resRef, 1);
                m_apSoundSources[nSound]->Set3D(1);
                m_apSoundSources[nSound]->SetPosition(GetPosition(), 0.0f);
                m_apSoundSources[nSound]->SetVolume(m_aSounds[nSound].m_nVolume);
                if (AsPlayer())
                {
                    m_apSoundSources[nSound]->SetPriorityGroup(SOUND_PRIORITY_PLAYER);
                }
                else
                {
                    m_apSoundSources[nSound]->SetPriorityGroup(SOUND_PRIORITY_ENEMY);
                }
                if (bPlaying)
                {
                    m_apSoundSources[nSound]->Play();
                }
            }
            else
            {
                delete m_apSoundSources[nSound];
                m_apSoundSources[nSound] = 0;
            }
        }
        else if (resRef.IsValid())
        {
            m_apSoundSources[nSound] = new CExoSoundSource(resRef);
            m_apSoundSources[nSound]->Set3D(1);
            m_apSoundSources[nSound]->SetPosition(GetPosition(), 0.0f);
            m_apSoundSources[nSound]->SetVolume(m_aSounds[nSound].m_nVolume);
            if (AsPlayer())
            {
                m_apSoundSources[nSound]->SetPriorityGroup(SOUND_PRIORITY_PLAYER);
            }
            else
            {
                m_apSoundSources[nSound]->SetPriorityGroup(SOUND_PRIORITY_ENEMY);
            }
        }
        return m_apSoundSources[nSound];
    }
    if (nSound < SOUND_COUNT)
    {
        m_aOtherSoundNames[nSound - SOUND_DEATH] = resRef;
    }
    return 0;
}
// PC 0x006C2750
Vector CSWTrackFollower::GetPosition()
{
    Vector vPosition;
    if (m_lstModels.num != 0)
    {
        vPosition = m_lstModels[0].m_pModel->GetPosition();
    }
    return vPosition;
}
// PC 0x006C27C0
void CSWMiniPlayer::Control(float fTime, CSWMGPhysicsState& stateCurrent,
                           CSWMGPhysicsState& statePrevious)
{
    Vector vOffset = stateCurrent.m_vRate - statePrevious.m_vRate;

    if (!m_bSpeedOverride && bDoAcceleration &&
        m_fTrackSpeed != m_fMaximumSpeed)
    {
        if (m_fTrackSpeed < m_fMaximumSpeed)
            m_fTrackSpeed = min(m_fTrackSpeed + fTime * m_fAPS, m_fMaximumSpeed);
        else
            m_fTrackSpeed = max(m_fTrackSpeed - fTime * m_fAPS, m_fMaximumSpeed);
        SetSpeed(m_fTrackSpeed);
    }

    m_bSpeedOverride = 0;
    if (fSpeed != fLastSpeedSet)
    {
        SetSpeed(fSpeed);
        fLastSpeedSet = fSpeed;
    }

    if (m_pMiniGame->m_bUseInertia)
    {
        Vector vPrevious = m_vInitialHook;
        int nPositionKeys = m_lstPositionKeys.num;
        int nOrientationKeys = m_lstOrientationKeys.num;

        if (m_bTrackPlaying)
        {
            if (nPositionKeys || nOrientationKeys)
            {
                float fPositionBase = 0.0f;
                float fOrientationBase = 0.0f;
                float fPreviousTime = 0.0f;
                int nPosition = 0;
                int nOrientation = 0;
                int nMergedKeys = 0;
                Vector vTrackPosition = m_pTrack->GetPosition();
                Quaternion qTrackOrientation = m_pTrack->GetOrientation();
                --nPositionKeys;
                --nOrientationKeys;

                for (; nPosition < nPositionKeys &&
                       nOrientation < nOrientationKeys;)
                {
                    SWMGKeyInfo* pPosition = &m_lstPositionKeys[nPosition];
                    float fPositionTime = fPositionBase + pPosition->m_fTime;
                    SWMGKeyInfo* pOrientation = &m_lstOrientationKeys[nOrientation];
                    float fOrientationTime = fOrientationBase + pOrientation->m_fTime;

                    if (fPositionTime <= fOrientationTime)
                    {
                        if (fPositionTime == fPreviousTime && nMergedKeys)
                        {
                            m_lstKeyAddresses[nMergedKeys - 1] = pPosition;
                            fPositionBase = fPositionTime;
                        }
                        else
                        {
                            pPosition->m_fTime = fPositionTime - fPreviousTime;
                            m_lstKeyAddresses[nMergedKeys] = pPosition;
                            nMergedKeys++;
                            fPreviousTime = fPositionTime;
                        }
                        ++nPosition;
                    }
                    else
                    {
                        if (fOrientationTime == fPreviousTime && nMergedKeys)
                        {
                            m_lstKeyAddresses[nMergedKeys - 1] = pOrientation;
                            fOrientationBase = fOrientationTime;
                        }
                        else
                        {
                            pOrientation->m_fTime = fOrientationTime - fPreviousTime;
                            m_lstKeyAddresses[nMergedKeys] = pOrientation;
                            nMergedKeys++;
                            fPreviousTime = fOrientationTime;
                        }
                        ++nOrientation;
                    }
                }

                for (; nPosition < nPositionKeys; ++nPosition)
                {
                    SWMGKeyInfo* pKey = &m_lstPositionKeys[nPosition];
                    float fKeyTime = fPositionBase + pKey->m_fTime;
                    if (fKeyTime == fPreviousTime && nMergedKeys)
                    {
                        fPositionBase = fKeyTime;
                        m_lstKeyAddresses[nMergedKeys - 1] = pKey;
                    }
                    else
                    {
                        pKey->m_fTime = fKeyTime - fPreviousTime;
                        m_lstKeyAddresses[nMergedKeys] = pKey;
                        nMergedKeys++;
                        fPreviousTime = fKeyTime;
                    }
                }

                for (; nOrientation < nOrientationKeys; ++nOrientation)
                {
                    SWMGKeyInfo* pKey = &m_lstOrientationKeys[nOrientation];
                    float fKeyTime = fOrientationBase + pKey->m_fTime;
                    if (fKeyTime == fPreviousTime && nMergedKeys)
                    {
                        fOrientationBase = fKeyTime;
                        m_lstKeyAddresses[nMergedKeys - 1] = pKey;
                    }
                    else
                    {
                        pKey->m_fTime = fKeyTime - fPreviousTime;
                        m_lstKeyAddresses[nMergedKeys] = pKey;
                        nMergedKeys++;
                        fPreviousTime = fKeyTime;
                    }
                }

                for (int i = 0; i < nMergedKeys; ++i)
                {
                    SWMGKeyInfo* pKey = m_lstKeyAddresses[i];
                    if (pKey->m_bOrientation)
                        m_qInitialHook = pKey->m_qOrientation * qTrackOrientation;
                    if (pKey->m_bPosition)
                        m_vInitialHook = vTrackPosition + qTrackOrientation * pKey->m_vPosition;
                    if (pKey->m_fTime != 0.0f)
                        stateCurrent.m_vValue += DoInertia(vPrevious, m_vInitialHook,
                                                          m_qInitialHook, pKey->m_fTime);
                    vPrevious = m_vInitialHook;
                }
            }

            m_pTrack->GetPart("modelhook")->GetWorld(m_vInitialHook, m_qInitialHook);
            if (nPositionKeys || nOrientationKeys)
                fTime = min(m_lstPositionKeys[nPositionKeys].m_fTime,
                            m_lstOrientationKeys[nOrientationKeys].m_fTime);
            stateCurrent.m_vValue += DoInertia(vPrevious, m_vInitialHook,
                                              m_qInitialHook, fTime);
        }
        m_lstPositionKeys.num = 0;
        m_lstOrientationKeys.num = 0;
    }

    m_vOffset += vOffset;
    KeepInTunnel(m_vOffset);
    int nModels = m_lstModels.num;
    for (int i = 0; i < nModels; ++i)
    {
        SWMGFollowerModel model = m_lstModels[i];
        if (model.m_bAttached)
        {
            CAurBehavior* pBehavior = (CAurBehavior*)model.m_pModel->GetBehavior(-1);
            if (pBehavior)
            {
                switch (pBehavior->GetType())
                {
                case CAurBehavior::TYPE_ATTACH_TRANSLATE:
                    ((CAurBehaviorAttachTranslate*)pBehavior)->m_vTranslation = m_vOffset;
                    break;
                case CAurBehavior::TYPE_ATTACH_ROTATE:
                    ((CAurBehaviorAttachRotate*)pBehavior)->m_vRotation = m_vOffset;
                    break;
                }
            }
        }
    }

    if (m_pCameraHook)
    {
        CAurBehavior* pBehavior = (CAurBehavior*)m_pCameraHook->GetBehavior(-1);
        if (pBehavior && pBehavior->GetType() == CAurBehavior::TYPE_ATTACH_ROTATE)
            ((CAurBehaviorAttachRotate*)pBehavior)->m_vRotation = m_vOffset;
    }
}
// PC 0x006C29E0
void CSWMiniPlayer::Fire()
{
    int nGunBanks = m_lstGunBanks.num;
    for (int i = 0; i < nGunBanks; i++)
    {
        m_lstGunBanks[i]->Fire();
    }
}
// PC 0x006C2A10
CExoString CSWMGObstacle::GetName()
{
    if (m_pModel != 0)
    {
        return CExoString(((Gob*)(CAurObject*)m_pModel)->GetModelName());
    }
    return CExoString("");
}

#ifndef _XBOX
template <> void CExoArrayList<CSWMGGunBank*>::Remove(CSWMGGunBank* value)  // PC 0x004E3B70
{
    int nIndex;
    for (nIndex = 0; nIndex < num; nIndex++)
    {
        if ((*this)[nIndex] == value)
            break;
    }
    DelIndex(nIndex);
}
#endif
// PC 0x006C3D10
CSWMiniPlayer::CSWMiniPlayer(CSWMiniGame* pMiniGame)
    : CSWTrackFollower(pMiniGame, SWMG_OBJECT_PLAYER)
{
    m_nBankPosition = 0;
    m_fMinimumSpeed = 100.0f;
    m_fMaximumSpeed = 100.0f;
    m_fAPS = 0.0f;
    m_bSpeedOverride = 0;
    m_pCameraHook = 0;

    m_lstPositionKeys.SetSize(8);
    m_lstOrientationKeys.SetSize(8);
    m_lstKeyAddresses.SetSize(16);
    m_lstPositionKeys.num = 0;
    m_lstOrientationKeys.num = 0;
}
// PC 0x006C3830
CSWMiniPlayer::~CSWMiniPlayer()
{
    if (m_pCameraHook != 0)
    {
        delete m_pCameraHook;
    }
}

int CSWMiniPlayer::DoFollowerHitCheck(CExoArrayList<unsigned long>& lstEnemyIds,
                                     int bRunScripts)
{
    int nModels = m_lstModels.num;
    int nHits = 0;
    if (nModels != 0)
    {
        int nEnemies = lstEnemyIds.num;
        int nDamage = 0;
        Gob* pModel = m_lstModels.element[0].m_pModel;
        CSWMiniGameObjectArray* pObjects = g_pAppManager->GetClientExoApp()->GetMGOArray();
        Vector vPosition = pModel->GetPosition();
        Vector vPrevious = pModel->GetPreviousPosition();
        Vector vPlane = m_pMiniGame->BumpPlane(pModel->GetOrientation());
        Vector vPush;
        Vector vEnemyPosition;
        Vector vEnemyPrevious;
        Vector vRelativePosition;
        Vector vRelativePrevious;
        float fPlaneDistance = 0.0f - vPlane * vPosition;
        Vector vSphereCenter;

        for (int i = nEnemies - 1; i >= 0; i--)
        {
            CSWMiniEnemy* pEnemy = static_cast<CSWMiniEnemy*>(
                pObjects->GetMiniGameObject(lstEnemyIds.element[i], SWMG_OBJECT_ENEMY));
            if (pEnemy != 0)
            {
                if (pEnemy->m_bTrigger || m_pMiniGame->m_bDoBumping)
                {
                    if (pEnemy->m_lstModels.num <= 0 || pEnemy->GetModel(0) == 0)
                    {
                        break;
                    }
                    vEnemyPosition = pEnemy->GetModel(0)->GetPosition();
                    vEnemyPrevious = pEnemy->GetModel(0)->GetPreviousPosition();
                    float fEnemyRadius = pEnemy->m_fSphereRadius;
                    vRelativePosition = vPosition - vEnemyPosition;
                    vRelativePrevious = vPrevious - vEnemyPrevious;
                    if (SphereIntersect(vRelativePrevious, vRelativePosition, vSphereCenter,
                                        fEnemyRadius + m_fSphereRadius))
                    {
                        if (pEnemy->m_bTrigger)
                        {
                            if (bRunScripts)
                            {
                                m_nLastFollowerHit = pEnemy->m_nObjectId;
                                OnHitFollower(0);
                                pEnemy->m_nLastFollowerHit = m_nObjectId;
                                pEnemy->OnHitFollower(0);
                            }
                        }
                        else
                        {
                            vEnemyPosition = planeproject(vPlane, fPlaneDistance,
                                                          vEnemyPosition);
                            vPush += normalize(vPosition - vEnemyPosition)
                                     * (fEnemyRadius + m_fSphereRadius + 0.1f)
                                     + vEnemyPosition - vPosition;
                            if (bRunScripts)
                            {
                                if (!pEnemy->GetIsInvulnerable())
                                {
                                    pEnemy->Hit();
                                    pEnemy->AdjustHitPoints(-m_nBumpDamage);
                                    pEnemy->m_fInvincibilityRemaining = pEnemy->m_fInvincePeriod;
                                }
                                int nBumpDamage = pEnemy->m_nBumpDamage;
                                if (nDamage > nBumpDamage)
                                {
                                    nBumpDamage = nDamage;
                                }
                                nDamage = nBumpDamage;
                                m_nLastFollowerHit = pEnemy->m_nObjectId;
                                OnHitFollower(0);
                                pEnemy->m_nLastFollowerHit = m_nObjectId;
                                pEnemy->OnHitFollower(0);
                            }
                            nHits++;
                        }
                    }
                }
            }
            else
            {
                lstEnemyIds.DelIndex(i);
            }
        }

        if (nHits)
        {
            vPush /= (float)nHits;
            Vector vOffset;
            vOffset = vPush + m_vOffset;
            KeepInTunnel(vOffset);
            vPush = vOffset + vPosition - m_vOffset;
            m_vOffset = vOffset;
            for (int j = 0; j < nModels; j++)
            {
                m_lstModels.element[j].m_pModel->SetPosition(vPush);
            }
            if (m_fInvincibilityRemaining == 0.0f)
            {
                SetSpeed(m_fMinimumSpeed);
                m_fInvincibilityRemaining = m_fInvincePeriod;
                if (bRunScripts)
                {
                    Hit();
                    m_nDamage = -nDamage;
                    OnDamage(0);
                    m_bSpeedOverride = 1;
                }
            }
        }
    }
    return nHits;
}
// PC 0x006C3ED0
int CSWMiniPlayer::LoadGun(CResGFF* pGFF, CResStruct* pStruct)
{
    int bSuccess;
    CResStruct structBullet;
    CResRef resBullet;
    CResRef resGun;
    CResRef resFireSound;
    CSWMGBullet bullet;

    int nBank = pGFF->ReadFieldDWORD(pStruct, "BankID", bSuccess,
                                    INVALID_GUN_BANK);
    if (nBank == INVALID_GUN_BANK)
    {
        return 0;
    }

    resGun = pGFF->ReadFieldCResRef(pStruct, "Gun_Model", bSuccess, "");
    if (!resGun.IsValid())
    {
        return 0;
    }

    bSuccess = pGFF->GetStructFromStruct(&structBullet, pStruct, "Bullet");
    if (!bSuccess)
    {
        return 0;
    }

    {
        CResRef resDefault("");
        resBullet = pGFF->ReadFieldCResRef(&structBullet, "Bullet_Model", bSuccess,
                                         resDefault);
    }
    if (!resBullet.IsValid())
    {
        return 0;
    }

    bullet.m_nDamage = pGFF->ReadFieldDWORD(&structBullet, "Damage", bSuccess, 0);
    if (!bSuccess)
    {
        return 0;
    }
    bullet.m_fLifeSpan = pGFF->ReadFieldFLOAT(&structBullet, "Lifespan", bSuccess,
                                            0.0f);
    if (!bSuccess)
    {
        return 0;
    }
    bullet.m_fRateOfFire = pGFF->ReadFieldFLOAT(&structBullet, "Rate_Of_Fire",
                                              bSuccess, 0.0f);
    if (!bSuccess)
    {
        return 0;
    }
    bullet.m_fSpeed = pGFF->ReadFieldFLOAT(&structBullet, "Speed", bSuccess, 0.0f);
    if (!bSuccess)
    {
        return 0;
    }
    bullet.m_nTargetType = pGFF->ReadFieldDWORD(&structBullet, "Target_Type",
                                              bSuccess, 0);
    if (!bSuccess)
    {
        return 0;
    }

    {
        CResRef resDefault("");
        bullet.m_resCollisionSound = pGFF->ReadFieldCResRef(&structBullet,
            "Collision_Sound", bSuccess, resDefault);
    }
    {
        CResRef resDefault("");
        resFireSound = pGFF->ReadFieldCResRef(pStruct, "Fire_Sound", bSuccess,
                                            resDefault);
    }
    CreateGunBank(nBank, resBullet, resGun, bullet, resFireSound);
    return 1;
}
// PC 0x006C38E0
CSWMGGunBank* CSWMiniPlayer::CreateGunBank(int nBank, const CResRef& sBank,
                                           const CResRef& sGun,
                                           const CSWMGBullet& bullet,
                                           const CResRef& sFireSound)
{
    CSWMGGunBank* pGunBank = NULL;

    if (nBank < 0)
    {
        return NULL;
    }

    if (nBank >= m_lstGunBanks.num)
    {
        for (int i = m_lstGunBanks.num; i < nBank; i++)
        {
            m_lstGunBanks.Add(NULL);
        }

        pGunBank = new CSWMGGunBank(this, sBank, sGun, bullet);
        pGunBank->m_resFireSound = sFireSound;
        m_lstGunBanks.Add(pGunBank);
    }
    else if (m_lstGunBanks[nBank] == NULL)
    {
        pGunBank = new CSWMGGunBank(this, sBank, sGun, bullet);
        pGunBank->m_resFireSound = sFireSound;
        m_lstGunBanks.Insert(pGunBank, nBank);
        m_lstGunBanks.DelIndex(nBank + 1);
    }
    else
    {
        char sBuffer[64];
        sprintf(sBuffer, "CSWMiniPlayer: Recreating gun bank %d", nBank);
        return NULL;
    }

    return pGunBank;
}
// PC 0x006C4160
int CSWMiniEnemy::LoadGun(CResGFF* pGFF, CResStruct* pStruct)
{
    int bSuccess;
    CResStruct structBullet;
    CResRef resBullet;
    CResRef resGun;
    CResRef resFireSound;
    CSWMGBullet bullet;
    CSWMGTargettingParameters params;

    int nBank = pGFF->ReadFieldDWORD(pStruct, "BankID", bSuccess,
                                    INVALID_GUN_BANK);
    if (nBank == INVALID_GUN_BANK)
    {
        return 0;
    }

    resGun = pGFF->ReadFieldCResRef(pStruct, "Gun_Model", bSuccess, "");
    if (!resGun.IsValid())
    {
        return 0;
    }

    params.m_fSensingRadius = pGFF->ReadFieldFLOAT(pStruct, "Sensing_Radius",
                                                 bSuccess, 0.0f);
    if (!bSuccess)
    {
        return 0;
    }
    params.m_fHorizontalSpread = pGFF->ReadFieldFLOAT(pStruct, "Horiz_Spread",
                                                    bSuccess, 0.0f);
    if (!bSuccess)
    {
        return 0;
    }
    params.m_fVerticalSpread = pGFF->ReadFieldFLOAT(pStruct, "Vert_Spread",
                                                  bSuccess, 0.0f);
    if (!bSuccess)
    {
        return 0;
    }
    params.m_fInaccuracy = pGFF->ReadFieldFLOAT(pStruct, "Inaccuracy", bSuccess,
                                              0.0f);

    bSuccess = pGFF->GetStructFromStruct(&structBullet, pStruct, "Bullet");
    if (!bSuccess)
    {
        return 0;
    }

    {
        CResRef resDefault("");
        resBullet = pGFF->ReadFieldCResRef(&structBullet, "Bullet_Model", bSuccess,
                                         resDefault);
    }
    if (!resBullet.IsValid())
    {
        return 0;
    }

    bullet.m_nDamage = pGFF->ReadFieldDWORD(&structBullet, "Damage", bSuccess, 0);
    if (!bSuccess)
    {
        return 0;
    }
    bullet.m_fLifeSpan = pGFF->ReadFieldFLOAT(&structBullet, "Lifespan", bSuccess,
                                            0.0f);
    if (!bSuccess)
    {
        return 0;
    }
    bullet.m_fRateOfFire = pGFF->ReadFieldFLOAT(&structBullet, "Rate_Of_Fire",
                                              bSuccess, 0.0f);
    if (!bSuccess)
    {
        return 0;
    }
    bullet.m_fSpeed = pGFF->ReadFieldFLOAT(&structBullet, "Speed", bSuccess, 0.0f);
    if (!bSuccess)
    {
        return 0;
    }
    bullet.m_nTargetType = pGFF->ReadFieldDWORD(&structBullet, "Target_Type",
                                              bSuccess, 0);
    if (!bSuccess)
    {
        return 0;
    }

    {
        CResRef resDefault("");
        bullet.m_resCollisionSound = pGFF->ReadFieldCResRef(&structBullet,
            "Collision_Sound", bSuccess, resDefault);
    }
    {
        CResRef resDefault("");
        resFireSound = pGFF->ReadFieldCResRef(pStruct, "Fire_Sound", bSuccess,
                                            resDefault);
    }
    CreateGunBank(nBank, resBullet, resGun, bullet, params, resFireSound);
    return 1;
}
// PC 0x006C3A90
CSWMGGunBank* CSWMiniEnemy::CreateGunBank(int nBank, const CResRef& sBank,
                                      const CResRef& sGun,
                                      const CSWMGBullet& bullet,
                                      const CSWMGTargettingParameters& params,
                                      const CResRef& sFireSound)
{
    CSWMGGunBank* pGunBank = NULL;

    if (nBank < 0)
    {
        return NULL;
    }

    if (nBank >= m_lstGunBanks.num)
    {
        for (int i = m_lstGunBanks.num; i < nBank; i++)
        {
            m_lstGunBanks.Add(NULL);
        }

        pGunBank = new CSWMGTargettingGunBank(this, sBank, sGun, bullet,
                                             params, GetMiniGame()->GetPlayer());
        pGunBank->m_resFireSound = sFireSound;
        m_lstGunBanks.Add(pGunBank);
    }
    else if (m_lstGunBanks[nBank] == NULL)
    {
        pGunBank = new CSWMGTargettingGunBank(this, sBank, sGun, bullet,
                                             params, GetMiniGame()->GetPlayer());
        m_lstGunBanks.Insert(pGunBank, nBank);
        m_lstGunBanks.DelIndex(nBank + 1);
    }
    else
    {
        char sBuffer[64];
        sprintf(sBuffer, "CSWMiniEnemy: Recreating gun bank %d", nBank);
        return NULL;
    }

    return pGunBank;
}
// PC 0x006C4490
void OnAnimKeyEvent(CAurObject* pObject, char* sEvent, void* pUserData)
{
    CSWMiniGameObject* pMiniGameObject = (CSWMiniGameObject*)pUserData;
    if (strnicmp(sEvent, "custom_", 7) == 0)
    {
        sEvent += 7;
        CResRef resScript = pMiniGameObject->GetScriptName(
            CSWMiniGameObject::SCRIPT_ANIM_EVENT);
        CExoString sCustomEvent(sEvent);
        pMiniGameObject->m_pModel = pObject;
        pMiniGameObject->m_sName = sCustomEvent;
        if (resScript.IsValid())
        {
            CExoString sScript;
            resScript.CopyToString(sScript);
            unsigned long nObjectId = pMiniGameObject->m_nObjectId;
            g_pVirtualMachine->RunScript(&sScript, nObjectId, 1);
        }
    }
    else if (strnicmp(sEvent, "loop", 4) == 0)
    {
        CSWTrackFollower* pFollower = pMiniGameObject->AsFollower();
        if (pFollower != 0)
        {
            if (pFollower->m_nNumLoops != CSWTrackFollower::LOOP_FOREVER)
            {
                pFollower->SetLooping(pFollower->m_nNumLoops - 1);
                float fTrackSpeed = pFollower->m_fTrackSpeed;
                pFollower->SetSpeed(fTrackSpeed);
            }
            CResRef resScript = pFollower->GetScriptName(
                CSWTrackFollower::SCRIPT_TRACK_LOOP);
            if (resScript.IsValid())
            {
                CExoString sScript;
                resScript.CopyToString(sScript);
                unsigned long nObjectId = pFollower->m_nObjectId;
                g_pVirtualMachine->RunScript(&sScript, nObjectId, 1);
            }
        }
    }
}
// PC 0x006C4630
int CSWTrackFollower::Load(CResGFF* pGFF, CResStruct* pStruct)
{
    SwmgtrackfollowerTrace4();
    int bSuccess;
    CResList listGunBanks;
    CResStruct structChild;

    SetHitPoints(pGFF->ReadFieldDWORD(pStruct, "Hit_Points", bSuccess, 0));
    SetMaxHitPoints(pGFF->ReadFieldDWORD(pStruct, "Max_HPs", bSuccess, 0));
    SetSphereRadius(pGFF->ReadFieldFLOAT(pStruct, "Sphere_Radius", bSuccess, -1.0f));
    SetLooping(pGFF->ReadFieldINT(pStruct, "Num_Loops", bSuccess, LOOP_UNSPECIFIED));
    m_nBumpDamage = pGFF->ReadFieldINT(pStruct, "Bump_Damage", bSuccess, 0);
    SetInvincibility(pGFF->ReadFieldFLOAT(pStruct, "Invince_Period", bSuccess, 0.0f));

    bSuccess = pGFF->GetList(&listGunBanks, pStruct, "Gun_Banks");
    if (bSuccess)
    {
        int nGunBanks = pGFF->GetListCount(&listGunBanks);
        for (int i = 0; i < nGunBanks; i++)
        {
            bSuccess = pGFF->GetListElement(&structChild, &listGunBanks, i);
            if (bSuccess)
            {
                LoadGun(pGFF, &structChild);
            }
        }
        if (nGunBanks)
        {
            AttachGuns();
        }
    }
    if (pGFF->GetStructFromStruct(&structChild, pStruct, "Scripts"))
    {
        LoadScripts(pGFF, pStruct, structChild);
    }
    if (pGFF->GetStructFromStruct(&structChild, pStruct, "Sounds"))
    {
        LoadSounds(pGFF, pStruct, structChild);
    }
    OnCreate();
    return 1;
}
// PC 0x006C4800
void CSWTrackFollower::Go()
{
    if (m_pTrack != 0 && !m_bTrackPlaying)
    {
        int nModels = m_lstModels.num;
        int nAnimationFlags = Gob::ANIMATION_FLAG_TRACK_PLAYBACK;
        if (m_nNumLoops == 1)
        {
            nAnimationFlags |= Gob::ANIMATION_FLAG_NO_LOOP;
        }
        for (int i = 0; i < nModels; i++)
        {
            Gob* pModel = m_lstModels.element[i].m_pModel;
            pModel->RegisterCallBack(OnAnimKeyEvent, "*", this,
                                    AUR_CALLBACK_LONG_DELAY, false);
            pModel->PlayAnimation("Ready_01", 1.0f, 0, 0.0f);
        }
        if (AsPlayer() != 0)
        {
            m_pTrack->RegisterCallBack(OnReportKey, "ReportKey", 0,
                                      AUR_CALLBACK_LONG_DELAY, false);
            nAnimationFlags |= Gob::ANIMATION_FLAG_REPORT_KEYS;
        }
        m_pTrack->RegisterCallBack(OnAnimKeyEvent, "*", this,
                                  AUR_CALLBACK_LONG_DELAY, false);
        m_pTrack->PlayAnimation("track", m_fTrackSpeed / 100.0f,
                                nAnimationFlags, 0.0f);
        m_bTrackPlaying = 1;
        for (int i = 0; i < NUM_SOUND_SOURCES; i++)
        {
            CExoSoundSource* pSound = m_apSoundSources[i];
            if (pSound != 0 && !pSound->IsHardwarePlayingSoundPleaseDontUseThis())
            {
                pSound->SetPosition(GetPosition(), 0.0f);
                pSound->Play();
            }
        }
    }
}
// PC 0x006C4930
int CSWMiniPlayer::Load(CResGFF* pGFF, CResStruct* pStruct)
{
    int bSuccess;

    if (!CSWTrackFollower::Load(pGFF, pStruct))
    {
        return 0;
    }

    float fMinimumSpeed = pGFF->ReadFieldFLOAT(pStruct, "Minimum_Speed",
                                               bSuccess, -1.0f);
    if (fMinimumSpeed >= 0.0f)
    {
        m_fMinimumSpeed = fMinimumSpeed;
    }
    float fMaximumSpeed = pGFF->ReadFieldFLOAT(pStruct, "Maximum_Speed",
                                               bSuccess, 100.0f);
    if (fMaximumSpeed >= 0.0f)
    {
        m_fMaximumSpeed = fMaximumSpeed;
    }
    float fAccelSecs = pGFF->ReadFieldFLOAT(pStruct, "Accel_Secs", bSuccess,
                                            -1.0f);
    if (fAccelSecs == 0.0f)
    {
        float fAPS = fMaximumSpeed - fMinimumSpeed;
        if (fAPS >= 0.0f)
        {
            m_fAPS = fAPS;
        }
    }
    else if (fAccelSecs > 0.0f)
    {
        float fAPS = (fMaximumSpeed - fMinimumSpeed) / fAccelSecs;
        if (fAPS >= 0.0f)
        {
            m_fAPS = fAPS;
        }
    }

    m_vTunnelMinimum = Vector(
        pGFF->ReadFieldFLOAT(pStruct, "TunnelXPos", bSuccess, 0.0f),
        pGFF->ReadFieldFLOAT(pStruct, "TunnelYPos", bSuccess, 0.0f),
        pGFF->ReadFieldFLOAT(pStruct, "TunnelZPos", bSuccess, 0.0f));
    m_vTunnelMaximum = Vector(
        pGFF->ReadFieldFLOAT(pStruct, "TunnelXNeg", bSuccess, 0.0f),
        pGFF->ReadFieldFLOAT(pStruct, "TunnelYNeg", bSuccess, 0.0f),
        pGFF->ReadFieldFLOAT(pStruct, "TunnelZNeg", bSuccess, 0.0f));
    m_vTunnelWrap = pGFF->ReadFieldVector(pStruct, "TunnelInfinite", bSuccess,
                                          Vector(0.0f, 0.0f, 0.0f));
    SetOrigin(Vector(
        pGFF->ReadFieldFLOAT(pStruct, "Start_Offset_X", bSuccess, 0.0f),
        pGFF->ReadFieldFLOAT(pStruct, "Start_Offset_Y", bSuccess, 0.0f),
        pGFF->ReadFieldFLOAT(pStruct, "Start_Offset_Z", bSuccess, 0.0f)));
    m_vTarget = Vector(
        pGFF->ReadFieldFLOAT(pStruct, "Target_Offset_X", bSuccess, 0.0f),
        pGFF->ReadFieldFLOAT(pStruct, "Target_Offset_Y", bSuccess, 0.0f),
        pGFF->ReadFieldFLOAT(pStruct, "Target_Offset_Z", bSuccess, 0.0f));
    return 1;
}
// PC 0x006C4C20
int CSWMiniEnemy::Load(CResGFF* pGFF, CResStruct* pStruct)
{
    int bSuccess;

    if (!CSWTrackFollower::Load(pGFF, pStruct))
    {
        return 0;
    }

    m_bTrigger = pGFF->ReadFieldBYTE(pStruct, "Trigger", bSuccess, 0);
    return 1;
}
// PC 0x006C4C70
CSWMGObstacle::CSWMGObstacle(CSWMiniGame* pMiniGame, CAurObject* pModel)
    : CSWMiniGameObject(3)
{
    m_pModel = pModel;
}
// PC 0x006C4D10
CSWMGObstacle::~CSWMGObstacle()
{
    if (m_pModel != 0)
    {
        delete m_pModel;
    }
}

static inline void SwmgtrackfollowerTrace4()
{
    SwmgtrackfollowerTrace3();
}

static inline void SwmgtrackfollowerTrace3()
{
    SwmgtrackfollowerTrace2();
}

static inline void SwmgtrackfollowerTrace2()
{
    SwmgtrackfollowerTrace1();
}

static inline void SwmgtrackfollowerTrace1()
{
}
