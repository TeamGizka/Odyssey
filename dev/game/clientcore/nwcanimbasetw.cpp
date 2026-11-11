#include "nwcanimbase.h"
#include "swminigame.h"

#include <string.h>
// PC 0x006F64A0
Gob* CSWCAnimBaseTW::GetModel(unsigned char nSlot)
{
    switch (nSlot)
    {
    case 252:
        return m_pWings;
    case 253:
        return m_pTail;
    default:
        return CSWCAnimBase::GetModel(nSlot);
    }
}
// PC 0x006F64D0
int CSWCAnimBaseTW::LoadModel(const CResRef& sResRef, unsigned char nType)
{
    char szModel[20];

    switch (nType)
    {
    case MODEL_SLOT_MAIN:
        if (CSWCAnimBase::LoadModel(sResRef, MODEL_SLOT_MAIN) == 0)
        {
            return 0;
        }
        if (m_pTail != 0)
        {
            m_pTail->AttachToObject(m_pModel, "tail", Gob::ATTACH_NORMAL);
        }
        if (m_pWings != 0)
        {
            m_pWings->AttachToObject(m_pModel, "wings", Gob::ATTACH_NORMAL);
        }
        break;
    case MODEL_TAIL:
        sResRef.CopyToString(szModel);
        m_pTail = (Gob*)NewCAurObject(szModel, "tail", 0, 0);
        if (m_pTail == 0)
        {
            return 0;
        }
        m_sTailTexture = sResRef;
        m_bTailTextureChanged = 0;
        m_pTail->AttachToObject(m_pModel, "tail", Gob::ATTACH_NORMAL);
        break;
    case MODEL_WINGS:
        sResRef.CopyToString(szModel);
        m_pWings = (Gob*)NewCAurObject(szModel, "wings", 0, 0);
        if (m_pWings == 0)
        {
            return 0;
        }
        m_sWingsTexture = sResRef;
        m_bWingsTextureChanged = 0;
        m_pWings->AttachToObject(m_pModel, "wings", Gob::ATTACH_NORMAL);
        break;
    default:
        return 0;
    }
    return 1;
}
// PC 0x006F6610
int CSWCAnimBaseTW::ReplaceTexture(const CResRef& sOld, const CResRef& sNew,
                                   int bStore)
{
    char szNew[20];
    char szOld[20];

    if (m_pTail != 0)
    {
        sOld.CopyToString(szOld);
        sNew.CopyToString(szNew);
        strlwr(szNew);
        strlwr(szOld);
        m_pTail->ReplaceTexture(szOld, szNew);
        if (bStore != 0)
        {
            m_sTailTexture = sNew;
            m_bTailTextureChanged = 0;
        }
    }
    if (m_pWings != 0)
    {
        sOld.CopyToString(szOld);
        sNew.CopyToString(szNew);
        strlwr(szNew);
        strlwr(szOld);
        m_pWings->ReplaceTexture(szOld, szNew);
        if (bStore != 0)
        {
            m_sWingsTexture = sNew;
            m_bWingsTextureChanged = 0;
        }
    }
    return CSWCAnimBase::ReplaceTexture(sOld, sNew, bStore);
}
// PC 0x006F6720
int CSWCAnimBaseTW::ReplaceTexture(unsigned char nModel, const CResRef& sOld,
                                   const CResRef& sNew, int bStore)
{
    char szNew[20];
    char szOld[20];

    if (nModel == MODEL_TAIL)
    {
        if (m_pTail == 0)
        {
            return 0;
        }
        sOld.CopyToString(szOld);
        sNew.CopyToString(szNew);
        strlwr(szOld);
        strlwr(szNew);
        m_pTail->ReplaceTexture(szOld, szNew);
        if (bStore == 0)
        {
            return 1;
        }
        m_sTailTexture = sNew;
        m_bTailTextureChanged = 0;
        return 1;
    }
    if (nModel == MODEL_WINGS)
    {
        if (m_pWings == 0)
        {
            return 0;
        }
        sOld.CopyToString(szOld);
        sNew.CopyToString(szNew);
        strlwr(szOld);
        strlwr(szNew);
        m_pWings->ReplaceTexture(szOld, szNew);
        if (bStore != 0)
        {
            m_sWingsTexture = sNew;
            m_bWingsTextureChanged = 0;
        }
        return 1;
    }
    return CSWCAnimBase::ReplaceTexture(MODEL_SLOT_MAIN, sOld, sNew, bStore);
}
// PC 0x006F6880
int CSWCAnimBaseTW::ReplaceTexture(unsigned char nModel, const CResRef& sOld,
                                   const CResRef& sNew, unsigned long nColor,
                                   unsigned short* pnPalette, int bStore,
                                   unsigned long nPaletteStart)
{
    char szNew[20];
    char szOld[20];

    if (nModel == MODEL_TAIL)
    {
        if (m_pTail == 0)
        {
            return 0;
        }
        sOld.CopyToString(szOld);
        sNew.CopyToString(szNew);
        strlwr(szOld);
        strlwr(szNew);
        m_pTail->ReplaceTexture(szOld, szNew, nColor, pnPalette, nPaletteStart);
        if (bStore != 0)
        {
            m_sTailTexture = sNew;
            m_bTailTextureChanged = 1;
            m_nTailColor = (unsigned char)nColor;
            m_nTailPaletteStart = (unsigned char)nPaletteStart;
            memcpy(m_aTailPalette, &pnPalette[nPaletteStart], 2 * nColor);
        }
        return 1;
    }
    if (nModel == MODEL_WINGS)
    {
        if (m_pWings == 0)
        {
            return 0;
        }
        sOld.CopyToString(szOld);
        sNew.CopyToString(szNew);
        strlwr(szOld);
        strlwr(szNew);
        m_pWings->ReplaceTexture(szOld, szNew, nColor, pnPalette,
                                 nPaletteStart);
        if (bStore != 0)
        {
            m_sWingsTexture = sNew;
            m_bWingsTextureChanged = 1;
            m_nWingsColor = (unsigned char)nColor;
            m_nWingsPaletteStart = (unsigned char)nPaletteStart;
            memcpy(m_aWingsPalette, &pnPalette[nPaletteStart], 2 * nColor);
        }
        return 1;
    }
    return CSWCAnimBase::ReplaceTexture(MODEL_SLOT_MAIN, sOld, sNew, nColor, pnPalette,
                                        bStore, nPaletteStart);
}
// PC 0x006F6A50
int CSWCAnimBaseTW::ReplaceTexture(const CResRef& sOld, const CResRef& sNew,
                                   unsigned long nColor,
                                   unsigned short* pnPalette, int bStore,
                                   unsigned long nPaletteStart)
{
    unsigned short* pnCurrentPalette;
    const CResRef* pCurrentTexture;
    char szNew[20];
    char szOld[20];

    if (m_pTail != 0)
    {
        sOld.CopyToString(szOld);
        pCurrentTexture = &sNew;
        sNew.CopyToString(szNew);
        strlwr(szOld);
        strlwr(szNew);
        pnCurrentPalette = pnPalette;
        m_pTail->ReplaceTexture(szOld, szNew, nColor, pnPalette, nPaletteStart);
        if (bStore != 0)
        {
            m_sTailTexture = sNew;
            m_bTailTextureChanged = 1;
            m_nTailColor = (unsigned char)nColor;
            m_nTailPaletteStart = (unsigned char)nPaletteStart;
            memcpy(m_aTailPalette, &pnPalette[nPaletteStart], 2 * nColor);
        }
    }
    else
    {
        pnCurrentPalette = pnPalette;
        pCurrentTexture = &sNew;
    }
    if (m_pWings != 0)
    {
        sOld.CopyToString(szOld);
        pCurrentTexture->CopyToString(szNew);
        strlwr(szOld);
        strlwr(szNew);
        m_pWings->ReplaceTexture(szOld, szNew, nColor, pnPalette,
                                 nPaletteStart);
        if (bStore != 0)
        {
            m_sWingsTexture = *pCurrentTexture;
            m_bWingsTextureChanged = 1;
            m_nWingsColor = (unsigned char)nColor;
            m_nWingsPaletteStart = (unsigned char)nPaletteStart;
            memcpy(m_aWingsPalette, &pnCurrentPalette[nPaletteStart],
                   2 * nColor);
            pnCurrentPalette = pnPalette;
            pCurrentTexture = &sNew;
        }
    }
    return CSWCAnimBase::ReplaceTexture(sOld, *pCurrentTexture, nColor,
                                        pnCurrentPalette, bStore,
                                        nPaletteStart);
}
// PC 0x006F6BF0
int CSWCAnimBaseTW::RestoreTexture()
{

    if (m_pTail != 0 && m_bTailTextureChanged != 0)
    {
        CResRef sTexture("");
        ReplaceTexture(253, sTexture, m_sTailTexture, m_nTailColor,
                       m_aTailPalette, 0, m_nTailPaletteStart);
    }
    if (m_pWings != 0 && m_bWingsTextureChanged != 0)
    {
        CResRef sTexture("");
        ReplaceTexture(252, sTexture, m_sWingsTexture, m_nWingsColor,
                       m_aWingsPalette, 0, m_nWingsPaletteStart);
    }
    return CSWCAnimBase::RestoreTexture();
}
// PC 0x006F6CB0
int CSWCAnimBaseTW::RestoreTexture(unsigned char nModel)
{
    Gob* pModel;

    if (nModel == MODEL_TAIL)
    {
        pModel = m_pTail;
        if (pModel != 0)
        {
            if (m_bTailTextureChanged != 0)
            {
                CResRef sTexture("");
                ReplaceTexture(MODEL_TAIL, sTexture, m_sTailTexture, m_nTailColor,
                               m_aTailPalette, 0, m_nTailPaletteStart);
                return 1;
            }
            pModel->RestoreTexture();
            return 1;
        }
        return 0;
    }
    if (nModel == MODEL_WINGS)
    {
        pModel = m_pWings;
        if (pModel != 0)
        {
            if (m_bWingsTextureChanged != 0)
            {
                CResRef sTexture("");
                ReplaceTexture(MODEL_WINGS, sTexture, m_sWingsTexture, m_nWingsColor,
                               m_aWingsPalette, 0, m_nWingsPaletteStart);
                return 1;
            }
            pModel->RestoreTexture();
            return 1;
        }
        return 0;
    }
    return CSWCAnimBase::RestoreTexture(nModel);
}
// PC 0x006F6DC0
void CSWCAnimBaseTW::UnloadModel()
{
    if (m_pTail != 0)
    {
        m_pTail->AttachToObject(0, 0, Gob::ATTACH_NORMAL);
        delete m_pTail;
        m_pTail = 0;
    }
    if (m_pWings != 0)
    {
        m_pWings->AttachToObject(0, 0, Gob::ATTACH_NORMAL);
        delete m_pWings;
        m_pWings = 0;
    }
    CSWCAnimBase::UnloadModel();
}
// PC 0x006F6E30
void CSWCAnimBaseTW::UnloadModel(unsigned char nModel)
{
    if (nModel == MODEL_TAIL)
    {
        if (m_pTail != 0)
        {
            m_pTail->AttachToObject(0, 0, Gob::ATTACH_NORMAL);
            delete m_pTail;
            m_pTail = 0;
            return;
        }
    }
    else if (nModel == MODEL_WINGS)
    {
        if (m_pWings != 0)
        {
            m_pWings->AttachToObject(0, 0, Gob::ATTACH_NORMAL);
            delete m_pWings;
            m_pWings = 0;
            return;
        }
    }
    CSWCAnimBase::UnloadModel(nModel);
}
// PC 0x006F6EC0
int CSWCAnimBaseTW::SetEnvMap(int nType, int bStore, const CResRef& sMap)
{
    char szEnvMap[20];

    if (nType != 0)
    {
        sMap.CopyToString(szEnvMap);
        if (m_pTail != 0)
        {
            m_pTail->SetEnvironmentMap(szEnvMap);
        }
        if (m_pWings != 0)
        {
            m_pWings->SetEnvironmentMap(szEnvMap);
        }
    }
    else
    {
        if (m_pTail != 0)
        {
            m_pTail->SetEnvironmentMap("");
        }
        if (m_pWings != 0)
        {
            m_pWings->SetEnvironmentMap("");
        }
    }
    if (bStore != 0)
    {
        m_sTailEnvMap = sMap;
        m_bTailEnvMap = nType;
        m_sWingsEnvMap = sMap;
        m_bWingsEnvMap = nType;
    }
    return CSWCAnimBase::SetEnvMap(nType, bStore, sMap);
}
// PC 0x006F6F80
int CSWCAnimBaseTW::SetEnvMap(unsigned char nModel, int nType, int bStore,
                              const CResRef& sMap)
{
    char szEnvMap[20];
    Gob* pModel;

    if (nModel == MODEL_TAIL)
    {
        pModel = m_pTail;
        if (pModel != 0)
        {
            if (nType != 0)
            {
                sMap.CopyToString(szEnvMap);
                m_pTail->SetEnvironmentMap(szEnvMap);
            }
            else
            {
                pModel->SetEnvironmentMap("");
            }
            if (bStore != 0)
            {
                m_sTailEnvMap = sMap;
                m_bTailEnvMap = nType;
                return 1;
            }
        }
        return 1;
    }
    if (nModel == MODEL_WINGS)
    {
        pModel = m_pWings;
        if (pModel != 0)
        {
            if (nType != 0)
            {
                sMap.CopyToString(szEnvMap);
                m_pWings->SetEnvironmentMap(szEnvMap);
            }
            else
            {
                pModel->SetEnvironmentMap("");
            }
            if (bStore != 0)
            {
                m_sWingsEnvMap = sMap;
                m_bWingsEnvMap = nType;
            }
        }
        return 1;
    }
    return CSWCAnimBase::SetEnvMap(nModel, nType, bStore, sMap);
}
// PC 0x006F7090
int CSWCAnimBaseTW::RestoreEnvMap()
{
    char szEnvMap[20];

    if (m_pTail != 0)
    {
        if (m_bTailEnvMap != 0)
        {
            m_sTailEnvMap.CopyToString(szEnvMap);
            m_pTail->SetEnvironmentMap(szEnvMap);
        }
        else
        {
            m_pTail->SetEnvironmentMap("");
        }
    }
    if (m_pWings != 0)
    {
        if (m_bWingsEnvMap != 0)
        {
            m_sWingsEnvMap.CopyToString(szEnvMap);
            m_pWings->SetEnvironmentMap(szEnvMap);
            return CSWCAnimBase::RestoreEnvMap();
        }
        m_pWings->SetEnvironmentMap("");
    }
    return CSWCAnimBase::RestoreEnvMap();
}
// PC 0x006F7130
int CSWCAnimBaseTW::SetAnimationInternal(CAurObject* pObject,
                                         unsigned short nAnimation,
                                         float fSpeed, int nFlags, float fBlend)
{
    if (fSpeed == 0.0f)
    {
        fSpeed = 1.0f;
    }
    if (m_pTail != 0 || m_pWings != 0)
    {
        CExoString sAnimation = GetAnimationName(nAnimation);
        if (m_pTail != 0)
        {
            if (m_pTail->PlayAnimation(sAnimation.CStr(), fSpeed, nFlags,
                                       fBlend) == 0)
            {
                m_pTail->PlayAnimation("default", fSpeed, nFlags, fBlend);
            }
        }
        if (m_pWings != 0)
        {
            if (m_pWings->PlayAnimation(sAnimation.CStr(), fSpeed, nFlags,
                                        fBlend) == 0)
            {
                m_pWings->PlayAnimation("default", fSpeed, nFlags, fBlend);
            }
        }
    }
    return CSWCAnimBase::SetAnimationInternal(pObject, nAnimation, fSpeed,
                                              nFlags, fBlend);
}
// PC 0x006F7260
int CSWCAnimBaseTW::SetAnimationOnSubObjectInternal(CAurObject* pObject,
                                                    unsigned char nModel,
                                                    unsigned short nAnimation,
                                                    float fSpeed, int nFlags,
                                                    float fBlend)
{
    CExoString sAnimation = GetAnimationName(nAnimation);

    if (nModel == MODEL_SLOT_MAIN)
    {
        return CSWCAnimBase::SetAnimationInternal(pObject, nAnimation, fSpeed,
                                                  nFlags, fBlend);
    }
    else if (nModel == MODEL_TAIL)
    {
        if (m_pTail == 0)
        {
            return 0;
        }
        int nResult = m_pTail->PlayAnimation(sAnimation.CStr(), fSpeed,
                                             nFlags, fBlend);
        if (nResult == 0)
        {
            m_pTail->PlayAnimation("default", fSpeed, nFlags, fBlend);
        }
        return nResult;
    }
    else if (nModel == MODEL_WINGS)
    {
        if (m_pWings != 0)
        {
            int nResult = m_pWings->PlayAnimation(sAnimation.CStr(), fSpeed,
                                                  nFlags, fBlend);
            if (nResult == 0)
            {
                m_pWings->PlayAnimation("default", fSpeed, nFlags, fBlend);
            }
            return nResult;
        }
    }
    return 0;
}
// PC 0x006F73C0
CSWCAnimBaseTW::CSWCAnimBaseTW()
{
    m_nAnimationType = ANIMATION_TYPE_TAIL_WINGS;
    m_pTail = 0;
    m_pWings = 0;
    m_sTailTexture = "";
    m_bTailTextureChanged = 0;
    m_nTailColor = 0;
    m_nTailPaletteStart = 0;
    memset(m_aTailPalette, 0, sizeof(m_aTailPalette));
    m_sWingsTexture = "";
    m_bWingsTextureChanged = 0;
    m_nWingsColor = 0;
    m_nWingsPaletteStart = 0;
    memset(m_aWingsPalette, 0, sizeof(m_aWingsPalette));
    m_sTailEnvMap = "";
    m_bTailEnvMap = 0;
    m_sWingsEnvMap = "";
    m_bWingsEnvMap = 0;
}
// PC 0x006F7500
CSWCAnimBaseTW::~CSWCAnimBaseTW()
{
    UnloadModel();
}
