#include "nwcanimbase.h"
#include "math3d.h"

static inline void NwcanimbaseprojectileTrace2();
static inline void NwcanimbaseprojectileTrace1();
#define NWCANIMBASEPROJECTILE_TRACE() NwcanimbaseprojectileTrace2()
// PC 0x006EA6A0  Xbox 0x001244A0
int CSWCAnimBaseProjectile::SetOrientation(const Vector& vFacing)
{
    NWCANIMBASEPROJECTILE_TRACE();
    if (m_pModel == 0)
    {
        return 0;
    }
    if (vFacing.m_fX * vFacing.m_fX + vFacing.m_fY * vFacing.m_fY +
            vFacing.m_fZ * vFacing.m_fZ <=
        0.00001f)
    {
        return 0;
    }

    Vector vNormalized = normalize(vFacing);
    m_pModel->SetOrientation(
        YawPitchRoll(Yaw(vNormalized), Pitch(vNormalized), 0.0f));
    return 1;
}
// PC 0x006EA470  Xbox 0x00124250
CSWCAnimBaseProjectile::CSWCAnimBaseProjectile()
{
    m_nAnimationType = 7;
}
// PC 0x006EA490  Xbox 0x00124270
CSWCAnimBaseProjectile::~CSWCAnimBaseProjectile()
{
    UnloadModel();
}
// PC 0x006EA4E0  Xbox 0x001242C0
int CSWCAnimBaseProjectile::LoadModel(const CResRef& sResRef,
                                      unsigned char nType)
{
    char sModel[20];

    sResRef.CopyToString(sModel);
    m_pModel = static_cast<Gob*>(NewCAurObject(sModel, "projectile", 0, 0));
    if (m_pModel == 0)
    {
        return 0;
    }

    m_resLoadedModel = sResRef;
    m_bPaletteReplaced = 0;
    return 1;
}
// PC 0x006EA550  Xbox 0x00124340
int CSWCAnimBaseProjectile::SetAnimation(unsigned short nAnimation,
                                         float fSpeed, int nFlags, float fBlend)
{
    if (fSpeed == 0.0f)
    {
        fSpeed = 1.0f;
    }

    switch (nAnimation)
    {
    case 0:
        m_pModel->PlayAnimation("default", fSpeed, nFlags, fBlend);
        return 1;
    case 1:
        m_pModel->PlayAnimation("travel01", fSpeed, nFlags, fBlend);
        return 1;
    case 2:
        m_pModel->PlayAnimation("impact01", fSpeed, nFlags | 1, fBlend);
        return 1;
    case 3:
        m_pModel->PlayAnimation("throwout", fSpeed, nFlags, fBlend);
        return 1;
    case 4:
        m_pModel->PlayAnimation("throwback", fSpeed, nFlags, fBlend);
        return 1;
    default:
        m_pModel->PlayAnimation("", 1.0f, 0, fBlend);
        return 1;
    }
}

static inline void NwcanimbaseprojectileTrace2()
{
    NwcanimbaseprojectileTrace1();
}

static inline void NwcanimbaseprojectileTrace1()
{
}
