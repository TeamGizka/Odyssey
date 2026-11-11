#include "aurora.h"
// PC 0x00782C40
int CAuroraTexture::GetTextureDataSize()
{
    if (m_pRes != NULL && m_pRes->GetDemands() >= 1)
    {
        return m_pRes->GetTGASize();
    }
    return 0;
}
// PC 0x00782C10
unsigned char* CAuroraTexture::GetTextureDataPtr()
{
    if (m_pRes != NULL && m_pRes->GetDemands() >= 1)
    {
        return m_pRes->GetTGADataPtr();
    }
    return 0;
}
// PC 0x007823E0
int CAuroraTexture::GetTextureAttrib(unsigned long* pnWidth,
                                     unsigned long* pnHeight,
                                     unsigned char* pnDepth)
{
    if (m_pRes != NULL && m_pRes->GetDemands() >= 1)
    {
        return m_pRes->GetTGAAttrib(pnWidth, pnHeight, pnDepth);
    }
    else
    {
        return 0;
    }
}
// PC 0x00782420
int CAuroraTexture::Load()
{
    unsigned char* nTGADataPtr;
    STGATextureTrailer* pTrailer;
    unsigned long nWidth;
    unsigned long nHeight;
    unsigned char nDepth;

    if (m_pRes->Demand() == NULL)
    {
        return 0;
    }
    m_pRes->GetTGAAttrib(&nWidth, &nHeight, &nDepth);
    if ((nDepth != 8 && nDepth != 24 && nDepth != 32) ||
        (nTGADataPtr = m_pRes->GetTGADataPtr()) == 0)
    {
        m_pRes->Release();
        return 0;
    }
    pTrailer = (STGATextureTrailer*)(nTGADataPtr +
                                     nHeight * nWidth * (nDepth >> 3) + 4);
    pTrailer->pOwner = this;
    pTrailer->nType = 3;
    return 1;
}
// PC 0x007824A0
void CAuroraTexture::Unload()
{
    int nBytesPerPixel;
    unsigned long nPixelCount;
    char* pPixel;
    char nBlue;
    unsigned char nDepth;
    unsigned long nWidth;
    unsigned long nHeight;

    if (m_pRes != NULL)
    {
        if (m_pRes->GetDemands() >= 1)
        {
            m_pRes->GetTGAAttrib(&nWidth, &nHeight, &nDepth);
            nBytesPerPixel = nDepth >> 3;
            nPixelCount = nHeight * nWidth;
            for (pPixel = (char*)m_pRes->GetTGADataPtr(); nPixelCount > 0;
                 --nPixelCount)
            {
                nBlue = *pPixel;
                *pPixel = pPixel[2];
                pPixel[2] = nBlue;
                pPixel += nBytesPerPixel;
            }
            m_pRes->Release();
            return;
        }
    }
}
// PC 0x00782510
int CAuroraTexture::ReadTextureHeader(int* pnWidth, int* pnHeight, int* pnDepth)
{
    TGAHeader tgaHeader;

    if (m_pRes == NULL)
    {
        return 0;
    }
    if (m_pRes->ReadRaw(sizeof(TGAHeader), (char*)&tgaHeader) == 0)
    {
        return 0;
    }
    *pnWidth = tgaHeader.nWidth;
    *pnHeight = tgaHeader.nHeight;
    *pnDepth = tgaHeader.nBitsPerPixel >> 3;
    return 1;
}
// PC 0x00782570
unsigned char* CAuroraTexture::GetPtrToResHelperInfo()
{
    unsigned char* nTGADataPtr;
    unsigned char nDepth;
    unsigned long nWidth;
    unsigned long nHeight;

    if (!(m_pRes == NULL || m_pRes->GetDemands() < 1))
    {
        nTGADataPtr = GetTextureDataPtr();
        GetTextureAttrib(&nWidth, &nHeight, &nDepth);
        return nHeight * nWidth * (nDepth >> 3) + nTGADataPtr + 4;
    }
    return 0;
}
// PC 0x007827D0
CAuroraTexture::CAuroraTexture(CResRef cResRef, int bRequest)
    : CResHelper<CResTGA, RESTYPE_TGA>()
{
    SetResRef(cResRef, bRequest);
}
// PC 0x00782860
CAuroraTexture::~CAuroraTexture()
{
}
