#include "aurfonttexture.h"

CAuroraFontTexture::CAuroraFontTexture(CResRef cResRef, int bRequest)
    : CResHelper<CResFNT, RESTYPE_FNT>()
{
    SetResRef(cResRef, bRequest);
}

CAuroraFontTexture::~CAuroraFontTexture()
{
}

unsigned long* CAuroraFontTexture::GetFontDataPtr()
{
    if (m_pRes != NULL && m_pRes->GetDemands() >= 1)
    {
        return m_pRes->GetFNTDataPtr();
    }
    return 0;
}

int CAuroraFontTexture::GetFontTextureAttrib(unsigned long* pnWidth,
                                         unsigned long* pnHeight,
                                         unsigned char* pnDepth)
{
    if (m_pRes != NULL && m_pRes->GetDemands() >= 1)
    {
        return m_pRes->GetTGAAttrib(pnWidth, pnHeight, pnDepth);
    }
    return 0;
}

unsigned char* CAuroraFontTexture::GetFontTextureDataPtr()
{
    if (m_pRes != NULL && m_pRes->GetDemands() >= 1)
    {
        return m_pRes->GetTGADataPtr();
    }
    return 0;
}

unsigned char* CAuroraFontTexture::GetPtrToResHelperInfo()
{
    if (m_pRes == NULL || m_pRes->GetDemands() < 1)
    {
        return 0;
    }
    unsigned char* pFontInfo = (unsigned char*)GetFontDataPtr();
    return pFontInfo + FNT_FONT_INFO_OFFSET +
           FNT_GLYPH_SIZE * *m_pRes->GetFNTDataPtr();
}

int CAuroraFontTexture::Load()
{
    unsigned long nWidth;
    unsigned long nHeight;
    unsigned char nDepth;
    SFontTextureDataTrailer* pTrailer;

    if (m_pRes->Demand() == NULL)
    {
        return 0;
    }
    m_pRes->GetTGAAttrib(&nWidth, &nHeight, &nDepth);
    if ((nDepth != 24 && nDepth != 32) || m_pRes->GetTGADataPtr() == NULL)
    {
        m_pRes->Release();
        return 0;
    }
    unsigned long nGlyphs = *m_pRes->GetFNTDataPtr();
    pTrailer = (SFontTextureDataTrailer*)((unsigned char*)m_pRes->GetFNTDataPtr() +
                                          FNT_FONT_INFO_OFFSET +
                                          FNT_GLYPH_SIZE * nGlyphs);
    pTrailer->pOwner = this;
    pTrailer->nType = RESTYPE_FNT;
    return 1;
}

void CAuroraFontTexture::Unload()
{
    if (m_pRes != NULL && m_pRes->GetDemands() >= 1)
    {
        m_pRes->Release();
    }
}
