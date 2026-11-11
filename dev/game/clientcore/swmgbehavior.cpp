#include <math.h>
#include "swminigame.h"
#include "nwcanimbase.h"
#include "swminigame.h"
#include "exobase.h"
// PC 0x006DADF0  Xbox 0x001260E0
CSWMGBehaviorFire::CSWMGBehaviorFire(CAurObject* pObject,
                                     CAurObject* pParent,
                                     CAurPart* pPart)
    : CAurBehaviorAttach(pObject, pParent, pPart),
      m_resFireSound(),
      m_fTimeToFire(0.0f)
{
}
// PC 0x006DAE70  Xbox 0x00126160
int CSWMGBehaviorFire::Control(float fTimeStep)
{
    int nResult = CAurBehaviorAttach::Control(fTimeStep);
    if (nResult == 0)
    {
        return nResult;
    }
    if (m_fTimeToFire > 0.0)
    {
        float fTime = m_fTimeToFire - fTimeStep;
        if (0.0f > fTime)
        {
            fTime = 0.0f;
        }
        m_fTimeToFire = fTime;
    }
    return 1;
}
// PC 0x006DAEC0  Xbox 0x001261B0
int CSWMGBehaviorFire::Fire()
{
    if (m_fTimeToFire == 0.0)
    {
        m_fTimeToFire = m_fRateOfFire;
        ((Gob*)m_pObject)->PlayAnimation("fire", 1.0f, 1, 0.0f);
        ((Gob*)m_pObject)->PlayAnimation("ready", 1.0f, 32, 0.0f);
        if (m_resFireSound.IsValid())
        {
            CResRef resSound(
                reinterpret_cast<char*>(m_resFireSound.GetResRef()));
            g_pExoSound->PlayOneShotSound(resSound, CExoSound::PRIORITY_GROUP_DEFAULT, 0, 0, 0.0f, 0.0f);
        }
        return 1;
    }
    return 0;
}
// PC 0x006DAF60  Xbox 0x00126250
void CSWMGBehaviorFire::SetRateOfFire(float fRateOfFire)
{
    if (m_fTimeToFire < m_fRateOfFire && m_fTimeToFire != 0.0f)
    {
        m_fTimeToFire = m_fTimeToFire - (m_fRateOfFire - fRateOfFire);
    }
    m_fRateOfFire = fRateOfFire;
}
// PC 0x006DAFA0  Xbox 0x001262A0
void CSWMGBehaviorTrackAndFire::SetRateOfFire(float fRateOfFire)
{
    if (m_fTimeToFire < m_fRateOfFire && m_fTimeToFire != 0.0f)
    {
        m_fTimeToFire = m_fTimeToFire - (m_fRateOfFire - fRateOfFire);
    }
    m_fRateOfFire = fRateOfFire;
}
// PC 0x006DAFE0  Xbox 0x001262F0
CSWMGBehaviorBullet::CSWMGBehaviorBullet(CAurObject* pObject,
                                        CSWTrackFollower* pFollower,
                                        const CSWMGBullet& bullet)
    : CAurBehavior(pObject)
{
    m_bullet = bullet;
    m_pFollower = pFollower;
}
// PC 0x006DB090  Xbox 0x001263A0
int CSWMGBehaviorBullet::Control(float fTimeStep)
{
    if (CAurBehavior::Control(fTimeStep) == 0)
    {
        return 0;
    }
    m_bullet.m_fLifeSpan -= fTimeStep;
    if (m_bullet.m_fLifeSpan <= 0.0f)
    {
        delete m_pObject;
        return 0;
    }
    ((Gob*)m_pObject)
        ->SetPosition(((Gob*)m_pObject)->GetPosition() +
                      ((Gob*)m_pObject)->GetOrientation().ydir() *
                          (fTimeStep * m_bullet.m_fSpeed));
    return 1;
}
// PC 0x006DB180  Xbox 0x00126510
int CSWMGBehaviorTrack::Control(float fTimeStep)
{
    m_bTargetting = false;
    if (CAurBehaviorAttach::Control(fTimeStep) == 0)
    {
        return 0;
    }
    if (m_pModel.m_pObject == NULL)
    {
        ((Gob*)m_pObject)->SetBehavior(NULL);
        return 0;
    }
    Quaternion qOrientation = ((Gob*)m_pObject)->GetOrientation();
    Vector vOffset =
        -qOrientation *
        (qOrientation * ((Gob*)m_pObject)->GetPosition() + m_vTarget);
    Vector vLocal =
        -qOrientation * (((Gob*)m_pModel.m_pObject)->GetPosition() - vOffset);
    if (!(m_params.m_fSensingRadius * m_params.m_fSensingRadius <
          vLocal * vLocal))
    {
        float fPitch = Pitch(vLocal);
        float fYaw = Yaw(vLocal);
        m_bTargetting = true;
        if (m_params.m_fHorizontalSpread < fabs(fYaw))
        {
            m_bTargetting = false;
            if (fYaw < 0.0f)
            {
                fYaw = -m_params.m_fHorizontalSpread;
            }
            else
            {
                fYaw = m_params.m_fHorizontalSpread;
            }
        }
        if (m_params.m_fVerticalSpread < fabs(fPitch))
        {
            m_bTargetting = false;
            if (fPitch < 0.0f)
            {
                fPitch = -m_params.m_fVerticalSpread;
            }
            else
            {
                fPitch = m_params.m_fVerticalSpread;
            }
        }
        ((Gob*)m_pObject)
            ->SetOrientation(YawPitchRoll(fYaw, -fPitch, Roll(qOrientation)) *
                             qOrientation);
    }
    return 1;
}
// PC 0x006DB3D0  Xbox 0x00126790
int CSWMGBehaviorTrackAndFire::Control(float fTimeStep)
{
    int nResult = CSWMGBehaviorTrack::Control(fTimeStep);
    if (nResult == 0)
    {
        return nResult;
    }
    if (m_fTimeToFire > 0.0)
    {
        float fTime = m_fTimeToFire - fTimeStep;
        if (0.0f > fTime)
        {
            fTime = 0.0f;
        }
        m_fTimeToFire = fTime;
    }
    if (m_bTargetting && m_fTimeToFire == 0.0)
    {
        m_fTimeToFire = m_fRateOfFire;
        ((Gob*)m_pObject)->PlayAnimation("fire", 1.0f, 1, 0.0f);
        ((Gob*)m_pObject)->PlayAnimation("ready", 1.0f, 32, 0.0f);
        if (m_resFireSound.IsValid())
        {
            CResRef resSound(
                reinterpret_cast<char*>(m_resFireSound.GetResRef()));
            g_pExoSound->Play3DOneShotSound(resSound,
                                            ((Gob*)m_pObject)->GetPosition(),
                                            0.0f, 24, 0, 0, 0.0f, 0.0f);
        }
    }
    return 1;
}
// PC 0x006DB4F0  Xbox 0x001268D0
CSWMGBehaviorTrack::CSWMGBehaviorTrack(CAurObject* pObject,
                                     CAurObject* pParent,
                                     CAurPart* pPart,
                                     CAurObject* pModel)
    : CAurBehaviorAttach(pObject, pParent, pPart)
{
    m_bTargetting = false;
    m_pModel = pModel;
    AddDependency(pModel);
}
// PC 0x006DB590  Xbox 0x00126980
CSWMGBehaviorTrackAndFire::CSWMGBehaviorTrackAndFire(CAurObject* pObject,
                                                      CAurObject* pParent,
                                                      CAurPart* pPart,
                                                      CAurObject* pModel)
    : CSWMGBehaviorTrack(pObject, pParent, pPart, pModel),
      m_resFireSound(),
      m_fTimeToFire(0.0f)
{
}

