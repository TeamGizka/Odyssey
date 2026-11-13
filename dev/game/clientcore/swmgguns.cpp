#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "swminigame.h"
#include "aurscene.h"

#include "swminigame.h"
// PC 0x006C8D50 GOG 0x006C8A50
float CSWMGGunBank::SetBulletSpeed(float fSpeed)
{
    if (fSpeed >= 0.0)
    {
        m_bullet.m_fSpeed = fSpeed;
    }
    return m_bullet.m_fSpeed;
}
// PC 0x006C8D70 GOG 0x006C8A70
float CSWMGGunBank::SetBulletLifeSpan(float fLifeSpan)
{
    if (fLifeSpan > 0.0)
    {
        m_bullet.m_fLifeSpan = fLifeSpan;
    }
    return m_bullet.m_fLifeSpan;
}
// PC 0x006C8D90 GOG 0x006C8A90
int CSWMGGunBank::SetBulletTargetType(int nTargetType)
{
    if ((nTargetType & 0x11111110) != 0)
    {
        char sBuffer[256];
        sprintf(sBuffer, "CSWMGGunBank: Bullet Target Type (%d) is invalid",
                nTargetType);
    }
    else
    {
        m_bullet.m_nTargetType = nTargetType;
    }
    return m_bullet.m_nTargetType;
}
// PC 0x006C8DE0 GOG 0x006C8AE0
void CSWMGGunBank::OnFire()
{
    CResRef resScript = m_pFollower->GetScriptName(7);
    m_pFollower->SetBullet(&m_bullet);
    if (resScript.IsValid())
    {
        CExoString sScript;

        resScript.CopyToString(sScript);
        unsigned long nObjectId = m_pFollower->m_nObjectId;
        g_pVirtualMachine->RunScript(&sScript, nObjectId, 1);
    }
}
// PC 0x006C8E80 GOG 0x006C8B80
float CSWMGTargettingGunBank::SetSensingRadius(float fRadius)
{
    if (fRadius >= 0.0)
    {
        m_params.m_fSensingRadius = fRadius;
    }
    return m_params.m_fSensingRadius;
}
// PC 0x006C8EA0 GOG 0x006C8BA0
float CSWMGTargettingGunBank::SetVerticalSpread(float fSpread)
{
    if (fSpread >= 0.0 || fSpread == -1.0)
    {
        m_params.m_fVerticalSpread = fSpread;
    }
    return m_params.m_fVerticalSpread;
}
// PC 0x006C8ED0 GOG 0x006C8BD0
float CSWMGTargettingGunBank::SetHorizontalSpread(float fSpread)
{
    if (fSpread >= 0.0 || fSpread == -1.0)
    {
        m_params.m_fHorizontalSpread = fSpread;
    }
    return m_params.m_fHorizontalSpread;
}
// PC 0x006C8F00 GOG 0x006C8C00
float CSWMGTargettingGunBank::SetInaccuracy(float fInaccuracy)
{
    if (fInaccuracy >= 0.0)
    {
        m_params.m_fInaccuracy = fInaccuracy;
    }
    return m_params.m_fInaccuracy;
}
// PC 0x006C8F20 GOG 0x006C8C20
CSWMGBullet::CSWMGBullet()
{
    m_nDamage = 0;
    m_nTargetType = 0;
    m_fSpeed = 0;
    m_fRateOfFire = 0;
    m_fLifeSpan = 0;
}
// PC 0x006C8F40 GOG 0x006C8C40
CSWMGTargettingParameters::CSWMGTargettingParameters()
{
    m_fHorizontalSpread = 0;
    m_fVerticalSpread = 0;
    m_fSensingRadius = 0;
    m_fInaccuracy = 0;
}
// PC 0x006C8F50 GOG 0x006C8C50
void FireGunCallback(CAurObject* pObject, char* sEvent, void* pData)
{
    CSWMGGunBank* pBank = static_cast<CSWMGGunBank*>(pData);
    Gob* pGun = static_cast<Gob*>(pObject);
    if (strnicmp(sEvent, "fire", 4) == 0)
    {
        Vector vPosition;
        Quaternion qOrientation;
        int nHook;
        char sHook[20];
        sscanf(sEvent, "fire%d", &nHook);
        sprintf(sHook, "bullethook%d", nHook);
        if (pGun->GetPartPosition(sHook, &vPosition, &qOrientation))
        {
            CResRef resBullet = pBank->m_resBank;
            char sModel[20];
            resBullet.CopyToString(sModel);
            Gob* pBullet = static_cast<Gob*>(NewCAurObject(sModel, "", 0, 0));
            if (pBullet == 0)
            {
                char sError[256];
                sprintf(sError, "FireGunCallback: could not create bullet %s", sModel);
                return;
            }
            if (pBank->IsTargetting())
            {
                CSWMGBehaviorTrackAndFire* pTrack =
                    static_cast<CSWMGBehaviorTrackAndFire*>(
                        pGun->GetBehavior(AUR_BEHAVIOR_TRACKANDFIRE));
                if (pTrack == 0)
                {
                    delete pBullet;
                    return;
                }
                float fSpread = pTrack->m_params.m_fInaccuracy;
                Quaternion qTarget = static_cast<Gob*>(
                    static_cast<CAurObject*>(pTrack->m_pModel))->GetOrientation();
                Vector vTarget = -qTarget *
                    (qTarget * static_cast<Gob*>(
                        static_cast<CAurObject*>(pTrack->m_pModel))->GetPosition() + pTrack->m_vTarget);
                if (fSpread > 0.0f)
                {
                    Vector vAim = vTarget;
                    Vector vRandom((float)(rand() - RAND_MAX / 2),
                                   (float)(rand() - RAND_MAX / 2),
                                   (float)(rand() - RAND_MAX / 2));
                    vRandom.Normalize();
                    vRandom *= pTrack->GetSpeed() * fSpread;
                    vTarget = normalize(vAim - vPosition);
                    vTarget = (vAim + vRandom) - vTarget * (vTarget * vRandom);
                }
                vTarget -= vPosition;
                qOrientation = pointto(vTarget) *
                    Quaternion(Vector(1.0f, 0.0f, 0.0f), HALF_PI);
            }
            pBullet->PlayAnimation("ready", 1.0f, 0, 0);
            CSWMiniPlayer* pPlayer =
                g_pAppManager->GetClientExoApp()->GetMiniGame()->m_pPlayer;
            if (pBank->GetFollower()->m_pTrack == pPlayer->m_pTrack)
            {
                pBank->SetBulletSpeed(pPlayer->GetTrackSpeed() +
                                      pBank->m_bullet.m_fSpeed);
                qOrientation = pPlayer->GetModel(0)->GetOrientation();
            }
            pBullet->AttachToScene(static_cast<Scene*>(pGun->GetScene()));
            pBullet->SetPosition(vPosition);
            pBullet->SetOrientation(qOrientation);
            pBullet->SetBehavior(new CSWMGBehaviorBullet(pBullet, pBank->GetFollower(),
                                                        pBank->m_bullet));
            pBank->GetFollower()->GetMiniGame()->AddBullet(pBullet);
            if (pBank->GetFollower()->m_pTrack == pPlayer->m_pTrack)
            {
                pBank->SetBulletSpeed(pBank->m_bullet.m_fSpeed -
                                      pPlayer->GetTrackSpeed());
            }
        }
        else
        {
            char sError[256];
            sprintf(sError, "FireGunCallback: no part called bullethook%d", nHook);
        }
    }
    else if (strnicmp(sEvent, "start", 5) == 0)
    {
        if (strnicmp(sEvent + 5, "fire", 4) == 0)
        {
            pBank->OnFire();
        }
    }
}
// PC 0x006C9890 GOG 0x006C9590
float CSWMGGunBank::SetRateOfFire(float fRateOfFire)
{
    if (fRateOfFire >= 0.0)
    {
        int nGuns = m_lstGuns.num;
        m_bullet.m_fRateOfFire = fRateOfFire;
        for (int i = 0; i < nGuns; i++)
        {
            Gob* pGun = static_cast<Gob*>(m_lstGuns[i]);
            CAurBehavior* pBehavior =
                reinterpret_cast<CAurBehavior*>(pGun->GetBehavior(-1));
            switch (pBehavior->GetType())
            {
            case AUR_BEHAVIOR_TRACKANDFIRE:
                static_cast<CSWMGBehaviorTrackAndFire*>(pBehavior)
                    ->SetRateOfFire(fRateOfFire);
                break;
            case AUR_BEHAVIOR_FIRE:
                static_cast<CSWMGBehaviorFire*>(pBehavior)->SetRateOfFire(
                    fRateOfFire);
                break;
            }
        }
    }
    return m_bullet.m_fRateOfFire;
}
// PC 0x006C9910 GOG 0x006C9610
void CSWMGGunBank::Hit()
{
    int nGuns = m_lstGuns.num;
    for (int i = 0; i < nGuns; i++)
    {
        Gob* pGun = static_cast<Gob*>(m_lstGuns[i]);
        pGun->PlayAnimation("damage", 1.0f, 1, 0);
        pGun->PlayAnimation("ready", 1.0f, 32, 0);
    }
}
// PC 0x006C9960 GOG 0x006C9660
CAurObject* CSWMGGunBank::RemoveGun(CAurObject* pGun)
{
    int nIndex = -1;
    int nGuns = m_lstGuns.num;
    for (int i = 0; i < nGuns; i++)
    {
        if (m_lstGuns[i] == pGun)
        {
            nIndex = i;
            break;
        }
    }
    if (nIndex == -1)
    {
        return 0;
    }
    static_cast<Gob*>(pGun)->AttachToObject(0, 0, Gob::ATTACH_NORMAL);
    int nNum = m_lstGuns.num - 1;
    int j = nIndex;
    for (m_lstGuns.num = nNum; j < m_lstGuns.num; j++)
    {
        m_lstGuns[j] = m_lstGuns[j + 1];
    }
    return pGun;
}
// PC 0x006C99D0 GOG 0x006C96D0
void CSWMGGunBank::Fire()
{
    int nGuns = m_lstGuns.num;
    for (int i = 0; i < nGuns; i++)
    {
        Gob* pGun = static_cast<Gob*>(m_lstGuns[i]);
        CSWMGBehaviorFire* pBehavior = reinterpret_cast<CSWMGBehaviorFire*>(
            pGun->GetBehavior(AUR_BEHAVIOR_FIRE));
        if (pBehavior != 0)
        {
            pBehavior->Fire();
        }
    }
}
// PC 0x006C9A10 GOG 0x006C9710
void CSWMGGunBank::SetGunModel(const CResRef& sGun)
{
    m_resGun = sGun;
    int nGuns = m_lstGuns.num;
    Gob* pNewGun;
    for (int i = 0; i < nGuns; i++)
    {
        CAurObject* pModel = 0;
        Gob* pOldGun = static_cast<Gob*>(m_lstGuns.element[0]);
        CAurBehaviorAttach* pOld =
            reinterpret_cast<CAurBehaviorAttach*>(pOldGun->GetBehavior(-1));
        if (pOld != 0)
        {
            switch (pOld->GetType())
            {
            case AUR_BEHAVIOR_TRACKANDFIRE:
                pModel =
                    static_cast<CSWMGBehaviorTrackAndFire*>(pOld)->m_pModel;
            case AUR_BEHAVIOR_FIRE:
                CAurObject* pParent = pOld->m_pParent;
                CAurPart* pPart = pOld->m_pPart;
                if (pParent != 0 && pPart != 0)
                {
                    char sModel[20];
                    m_resGun.CopyToString(sModel);
                    pNewGun =
                        static_cast<Gob*>(NewCAurObject(sModel, "", 0, 0));
                    switch (pOld->GetType())
                    {
                    case AUR_BEHAVIOR_FIRE:
                    {
                        CSWMGBehaviorFire* pOldFire =
                            static_cast<CSWMGBehaviorFire*>(pOld);
                        CSWMGBehaviorFire* pNew =
                            new CSWMGBehaviorFire(pNewGun, pParent, pPart);
                        pNew->SetRateOfFire(pOldFire->GetRateOfFire());
                        pNew->SetFireSound(pOldFire->m_resFireSound);
                        pNewGun->SetBehavior(
                            reinterpret_cast<CAurBehavior*>(pNew));
                        pNewGun->RegisterCallBack(FireGunCallback, "*", this,
                                                  9999.0f, 0);
                        break;
                    }
                    case AUR_BEHAVIOR_TRACKANDFIRE:
                    {
                        CSWMGBehaviorTrackAndFire* pOldFire =
                            static_cast<CSWMGBehaviorTrackAndFire*>(pOld);
                        CSWMGBehaviorTrackAndFire* pNew =
                            new CSWMGBehaviorTrackAndFire(pNewGun, pParent,
                                                          pPart, pModel);
                        pNew->m_params = pOldFire->m_params;
                        pNew->SetSpeed(pOldFire->m_fSpeed);
                        pNew->m_vTarget = pOldFire->m_vTarget;
                        pNew->SetRateOfFire(pOldFire->GetRateOfFire());
                        pNew->SetFireSound(pOldFire->m_resFireSound);
                        pNewGun->SetBehavior(
                            reinterpret_cast<CAurBehavior*>(pNew));
                        pNewGun->RegisterCallBack(FireGunCallback, "*", this,
                                                  9999.0f, 0);
                        break;
                    }
                    }
                    pNewGun->AttachToPart(pParent, pPart, 0);
                }
            }
        }
        pNewGun->PlayAnimation("ready", 1.0f, 0, 0);
        m_lstGuns.DelIndex(0);
        delete pOldGun;
        m_lstGuns.Add(pNewGun);
    }
}
// PC 0x006C9CF0 GOG 0x006C99F0
CAurObject* CSWMGGunBank::AddGun(CAurObject* pObject, CAurPart* pPart)
{
    if (!m_resGun.IsValid() || pObject == 0 || pPart == 0)
    {
        return 0;
    }
    char sModel[20];
    m_resGun.CopyToString(sModel);
    Gob* pGun = static_cast<Gob*>(NewCAurObject(sModel, "", 0, 0));
    if (pGun == 0)
    {
        return 0;
    }
    CSWMGBehaviorFire* pBehavior = new CSWMGBehaviorFire(pGun, pObject, pPart);
    pBehavior->SetRateOfFire(m_bullet.m_fRateOfFire);
    pBehavior->m_resFireSound = m_resFireSound;
    pGun->SetBehavior(reinterpret_cast<CAurBehavior*>(pBehavior));
    pGun->RegisterCallBack(FireGunCallback, "*", this, 9999.0f, 0);
    pGun->PlayAnimation("ready", 1.0f, 0, 0);
    m_lstGuns.Add(pGun);
    return pGun;
}
// PC 0x006C9E10 GOG 0x006C9B10
int CSWMGGunBank::ClearGuns()
{
    int nGuns = m_lstGuns.num;
    for (int i = nGuns - 1; i >= 0; i--)
    {
        Gob* pGun = static_cast<Gob*>(m_lstGuns.element[i]);
        pGun->AttachToObject(0, 0, Gob::ATTACH_NORMAL);
        delete m_lstGuns.element[i];
    }
    m_lstGuns.SetSize(0);
    return nGuns;
}
// PC 0x006C9E70 GOG 0x006C9B70
CAurObject* CSWMGTargettingGunBank::AddGun(CAurObject* pObject, CAurPart* pPart)
{
    if (!m_resGun.IsValid() || pObject == 0 || pPart == 0)
    {
        return 0;
    }
    char sModel[20];
    m_resGun.CopyToString(sModel);
    Gob* pGun = static_cast<Gob*>(NewCAurObject(sModel, "", 0, 0));
    if (pGun == 0)
    {
        return 0;
    }
    CSWMGBehaviorTrackAndFire* pBehavior = new CSWMGBehaviorTrackAndFire(
        pGun, pObject, pPart, m_pTarget->GetModel(0));
    pBehavior->SetRateOfFire(m_bullet.m_fRateOfFire);
    pBehavior->m_params = m_params;
    float fSpeed = m_pTarget->m_fSphereRadius;
    pBehavior->m_fSpeed = fSpeed;
    pBehavior->m_resFireSound = m_resFireSound;
    if (m_pTarget->AsPlayer() != 0)
    {
        pBehavior->m_vTarget = m_pTarget->AsPlayer()->m_vTarget;
    }
    pGun->SetBehavior(reinterpret_cast<CAurBehavior*>(pBehavior));
    pGun->RegisterCallBack(FireGunCallback, "*", this, 9999.0f, 0);
    pGun->PlayAnimation("ready", 1.0f, 0, 0);
    m_lstGuns.Add(pGun);
    return pGun;
}
// PC 0x006CA010 GOG 0x006C9D10
CSWMGGunBank::CSWMGGunBank(CSWTrackFollower* pFollower, const CResRef& sBank,
                           const CResRef& sGun, const CSWMGBullet& bullet)
{
    m_resBank = sBank;
    SetGunModel(sGun);
    m_bullet.m_nDamage = bullet.m_nDamage;
    SetBulletSpeed(bullet.m_fSpeed);
    SetRateOfFire(bullet.m_fRateOfFire);
    SetBulletLifeSpan(bullet.m_fLifeSpan);
    SetBulletTargetType(bullet.m_nTargetType);
    m_bullet.m_resCollisionSound = bullet.m_resCollisionSound;
    m_pFollower = pFollower;
}
// PC 0x006CA140 GOG 0x006C9E40
CSWMGGunBank::~CSWMGGunBank()
{
    ClearGuns();
}
// PC 0x006CA1A0 GOG 0x006C9EA0
CSWMGTargettingGunBank::CSWMGTargettingGunBank(
    CSWTrackFollower* pFollower, const CResRef& sBank, const CResRef& sGun,
    const CSWMGBullet& bullet, const CSWMGTargettingParameters& params,
    CSWTrackFollower* pTarget)
    : CSWMGGunBank(pFollower, sBank, sGun, bullet)
{
    SetSensingRadius(params.m_fSensingRadius);
    SetHorizontalSpread(params.m_fHorizontalSpread);
    SetVerticalSpread(params.m_fVerticalSpread);
    SetInaccuracy(params.m_fInaccuracy);
    m_pTarget = pTarget;
}
// PC 0x006CA270 GOG 0x006C9F70
void GunDie(CAurObject* pObject, char* sEvent, void* pData)
{
    CSWMGGunBank* pBank = static_cast<CSWMGGunBank*>(pData);
    if (pBank == 0)
    {
        delete pObject;
    }
    else
    {
        pBank->RemoveGun(pObject);
        delete pObject;
        if (pBank->m_lstGuns.num == 0)
        {
            if (pBank->m_pFollower != 0)
            {
                pBank->m_pFollower->RemoveGunBank(pBank);
            }
            delete pBank;
        }
    }
}
// PC 0x006CA2D0 GOG 0x006C9FD0
void CSWMGGunBank::Die()
{
    for (int i = m_lstGuns.num - 1; i >= 0; i--)
    {
        Gob* pGun = static_cast<Gob*>(m_lstGuns.element[i]);
        pGun->PlayAnimation(0, 1.0f, 0, 0);
        if (pGun->PlayAnimation("die", 1.0f, 1, 0) != 0)
        {
            pGun->RegisterCallBack(GunDie, "donedie", this, 9999.0f, 0);
        }
        else
        {
            RemoveGun(pGun);
            delete pGun;
        }
    }
}
// PC 0x006CA350 GOG 0x006CA050
void CSWMGTargettingGunBank::Die()
{
    int nGuns = m_lstGuns.num;
    for (int i = nGuns - 1; i >= 0; i--)
    {
        Gob* pGun = static_cast<Gob*>(m_lstGuns.element[i]);
        CAurBehaviorAttach* pFire = reinterpret_cast<CAurBehaviorAttach*>(
            pGun->GetBehavior(AUR_BEHAVIOR_TRACKANDFIRE));
        if (pFire == 0)
        {
            RemoveGun(pGun);
            delete pGun;
            continue;
        }
        if (pFire->m_pParent != 0 && pFire->m_pPart != 0)
        {
            CAurBehaviorAttach* pAttach =
                new CAurBehaviorAttach(pGun, pFire->m_pParent, pFire->m_pPart);
            pGun->SetBehavior(reinterpret_cast<CAurBehavior*>(pAttach));
            pGun->PlayAnimation(0, 1.0f, 0, 0);
            if (pGun->PlayAnimation("die", 1.0f, 1, 0) != 0)
            {
                pGun->RegisterCallBack(GunDie, "donedie", this, 9999.0f, 0);
            }
            else
            {
                RemoveGun(pGun);
                delete pGun;
            }
        }
        else
        {
            RemoveGun(pGun);
            delete pGun;
        }
    }
}
