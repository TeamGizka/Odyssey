#include <stdio.h>

#include "nwcanimbase.h"
#include "twodimarray.h"
#include "nwrules.h"
#include "twodimarrays.h"
#include "nwcanimbase.h"

static inline void NwcanimbaseplaceableTrace1();
#define NWCANIMBASEPLACEABLE_TRACE() NwcanimbaseplaceableTrace1()
// PC 0x00755B60
CSWCAnimBasePlaceable::CSWCAnimBasePlaceable()
{
    m_nAnimationType = 10;
}
// PC 0x00755B80
CSWCAnimBasePlaceable::~CSWCAnimBasePlaceable()
{
    UnloadModel();
}
// PC 0x00755BD0
int CSWCAnimBasePlaceable::LoadModel(const CResRef& sResRef,
                                     unsigned char nType)
{
    char szModel[20];

    sResRef.CopyToString(szModel);
    m_pModel = static_cast<Gob*>(NewCAurObject(szModel, "placeable", 0, 0));
    if (m_pModel == 0)
    {
        return 0;
    }

    m_resLoadedModel = sResRef;
    m_bPaletteReplaced = 0;
    return 1;
}
// PC 0x00755EA0
int CSWCAnimBasePlaceable::SetAnimation(unsigned short nAnimation, float fSpeed,
                                        int nFlags, float fBlend)
{
    if (m_pModel == 0)
    {
        return 0;
    }

    CExoString sAnimation = GetAnimationName(nAnimation);
    if (fSpeed == 0.0f)
    {
        fSpeed = 1.0f;
    }

    switch (nAnimation)
    {
    case 10000:
    case 10072:
    case 10073:
    case 10074:
    case 10075:
    case 10076:
        m_pModel->PlayAnimation(sAnimation.CStr(), fSpeed, 0, fBlend);
        return 0;
    case 306:
    case 312:
    case 313:
    case 314:
    case 315:
    case 10014:
    case 10077:
    case 10213:
    case 10214:
    case 10215:
    case 10216:
        m_pModel->PlayAnimation(sAnimation.CStr(), fSpeed, 1, fBlend);
        break;
    case 10106:
    case 10107:
    case 10108:
    case 10110:
    case 10111:
    case 10112:
    case 10113:
    case 10114:
    case 10115:
    case 10116:
        m_pModel->PlayAnimation(sAnimation.CStr(), fSpeed, 0, fBlend);
        break;
    default:
        if (nAnimation >= 1000 && nAnimation <= 1327)
        {
            m_pModel->PlayAnimation(sAnimation.CStr(), fSpeed, 1, fBlend);
        }
        else if (nAnimation >= 1400 && nAnimation <= 1727)
        {
            m_pModel->PlayAnimation(sAnimation.CStr(), fSpeed, 0, fBlend);
        }
        else
        {
            m_pModel->PlayAnimation("", 1.0f, 0, fBlend);
        }
        break;
    }
    return 1;
}
// PC 0x00755C40
CExoString CSWCAnimBasePlaceable::GetAnimationName(unsigned short nAnimation)
{
    NWCANIMBASEPLACEABLE_TRACE();
    unsigned short nClientAnimation = ServerToClientAnimation(nAnimation);
    CExoString sName;
    CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
    C2DA* pAnimations = p2DArrays->m_pAnimations;

    if (pAnimations->GetCExoStringEntry(nClientAnimation,
                                        CTwoDimArrays::COL2DA_ANIMATIONS_NAME,
                                        &sName) == 0)
    {
        sName = "";
    }

    if (sName.GetLength() > 0)
    {
        return sName;
    }

    if (nAnimation >= 1000 && nAnimation <= 1727)
    {
        int nIndex = -1;
        CExoString sSuffix;

        if (nAnimation <= 1127)
        {
            nIndex = nAnimation - 1000;
        }
        else if (nAnimation >= 1200 && nAnimation <= 1327)
        {
            sSuffix = "w";
            nIndex = nAnimation - 1200;
        }
        else if (nAnimation >= 1400 && nAnimation <= 1527)
        {
            sSuffix = "l";
            nIndex = nAnimation - 1400;
        }
        else if (nAnimation >= 1600)
        {
            sSuffix = "wl";
            nIndex = nAnimation - 1600;
        }

        nIndex++;
        if (nIndex != 0)
        {
            char szNumber[4];
            sprintf(szNumber, "%03d", nIndex);
            CExoString sNumber(szNumber);
            sName = CExoString("cut") + sNumber + sSuffix;
        }
    }
    else
    {
        sName = "default";
    }

    return sName;
}
// PC 0x007560F0
unsigned short CSWCAnimBasePlaceable::ServerToClientAnimation(unsigned short nAnimation)
{
    NWCANIMBASEPLACEABLE_TRACE();
    switch (nAnimation)
    {
    case PLACEABLE_ANIMATION_BASE:
        nAnimation = 304;
        break;
    case 10072:
        nAnimation = 307;
        break;
    case 10073:
        nAnimation = 308;
        break;
    case 10074:
        nAnimation = 309;
        break;
    case 10014:
    case 10077:
    case 10213:
    case 10214:
    case 10215:
    case 10216:
        nAnimation = 305;
        break;
    case 10075:
        nAnimation = 310;
        break;
    case 10076:
        nAnimation = 311;
        break;
    case 10106:
        nAnimation = 316;
        break;
    case 10107:
        nAnimation = 317;
        break;
    case 10108:
        nAnimation = 318;
        break;
    case 10110:
        nAnimation = 319;
        break;
    case 10111:
        nAnimation = 320;
        break;
    case 10112:
        nAnimation = 321;
        break;
    case 10113:
        nAnimation = 322;
        break;
    case 10114:
        nAnimation = 323;
        break;
    case 10115:
        nAnimation = 324;
        break;
    case 10116:
        nAnimation = 325;
        break;
    }

    if (nAnimation > SERVER_ANIMATION_FIRST - 1)
    {
        char szError[256];
        sprintf(szError, "CSWCAnimBasePlaceable::ServerToClientAnimation(): Failed to map server anim %i to client anim.", nAnimation);
    }
    return nAnimation;
}

static inline void NwcanimbaseplaceableTrace1()
{
}
