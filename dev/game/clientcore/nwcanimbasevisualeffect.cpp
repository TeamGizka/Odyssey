#include "nwcanimbase.h"
// PC 0x00756A10
CSWCAnimBaseVisualEffect::CSWCAnimBaseVisualEffect()
{
    m_nAnimationType = 8;
}
// PC 0x00756A30
CSWCAnimBaseVisualEffect::~CSWCAnimBaseVisualEffect()
{
    UnloadModel();
}
// PC 0x00756A80
int CSWCAnimBaseVisualEffect::LoadModel(const CResRef& sResRef,
                                        unsigned char nType)
{
    char szModel[20];

    sResRef.CopyToString(szModel);
    m_pModel = static_cast<Gob*>(NewCAurObject(szModel, "visualeffect", 0, 0));
    if (m_pModel == 0)
    {
        return 0;
    }

    m_resLoadedModel = sResRef;
    m_bPaletteReplaced = 0;
    m_pModel->DisableFog();
    return 1;
}
// PC 0x00756AF0
CExoString CSWCAnimBaseVisualEffect::GetAnimationName(unsigned short nAnimation)
{
    switch (nAnimation)
    {
    case 0:
        return CExoString("default");
    case 1:
        return CExoString("impact");
    case 2:
        return CExoString("duration");
    case 3:
        return CExoString("cessation");
    default:
        return CExoString("");
    }
}
// PC 0x00756B90
int CSWCAnimBaseVisualEffect::SetAnimationInternal(CAurObject* pObject,
                                                   unsigned short nAnimation,
                                                   float fSpeed, int nFlags,
                                                   float fBlend)
{
    CExoString sAnimation;

    if (pObject == 0)
    {
        return 0;
    }

    sAnimation = GetAnimationName(nAnimation);
    if (static_cast<Gob*>(pObject)->PlayAnimation(sAnimation.CStr(), fSpeed,
                                                  nFlags, fBlend) == 0)
    {
        return 0;
    }

    return 1;
}
