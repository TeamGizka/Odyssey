#include "aurora.h"
// PC 0x00783630
void CAuroraCompressedTexture::Unload()
{
    if (m_pRes != NULL && m_pRes->GetDemands() >= 1)
    {
        m_pRes->Release();
    }
}
// PC 0x00783FE0
unsigned char* CAuroraCompressedTexture::GetCompressedTextureDataPtr()
{
    if (m_pRes != 0 && m_pRes->GetDemands() >= 1)
    {
        return m_pRes->GetDDSDataPtr();
    }
    return 0;
}
// PC 0x00783BF0
int CAuroraCompressedTexture::GetCompressedTextureAttrib(
    unsigned long* pnWidth, unsigned long* pnHeight, unsigned char* pnBpp,
    unsigned long* pnSize, float* pfAlpha)
{
    if (m_pRes != 0 && m_pRes->GetDemands() >= 1)
    {
        return m_pRes->GetDDSAttrib(pnWidth, pnHeight, pnBpp, pnSize, pfAlpha);
    }
    else
    {
        return 0;
    }
}
// PC 0x00783C30
int CAuroraCompressedTexture::Load()
{
    SCompressedTextureDataTrailer* pInfo;

    if (m_pRes->Demand() == 0)
    {
        return 0;
    }
    pInfo = (SCompressedTextureDataTrailer*)(m_pRes->GetDDSDataPtr() - 26);
    pInfo->pOwner = this;
    pInfo->nType = 2033;
    return 1;
}
// PC 0x00783C60
int CAuroraCompressedTexture::ReadTextureHeader(int* pnWidth, int* pnHeight,
                                                int* pnBpp, int* pnSize,
                                                float* pfAlpha)
{
    struct
    {
        long nWidth;
        long nHeight;
        long nBpp;
        long nSize;
        float fAlpha;
    } header;

    if (m_pRes == 0)
    {
        return 0;
    }
    if (m_pRes->ReadRaw(sizeof(header), (char*)&header) == 0)
    {
        return 0;
    }
    *pnWidth = header.nWidth;
    *pnHeight = header.nHeight;
    *pnBpp = header.nBpp;
    *pnSize = header.nSize;
    *pfAlpha = header.fAlpha;
    return 1;
}
// PC 0x00783CC0
unsigned char* CAuroraCompressedTexture::GetPtrToResHelperInfo()
{
    if (m_pRes == 0 || m_pRes->GetDemands() < 1)
    {
        return 0;
    }
    return (GetCompressedTextureDataPtr() - 26);
}
// PC 0x00783EE0
CAuroraCompressedTexture::CAuroraCompressedTexture(CResRef cResRef, int bRequest)
{
    SetResRef(cResRef, bRequest);
}
// PC 0x00783F70
CAuroraCompressedTexture::~CAuroraCompressedTexture()
{
}
