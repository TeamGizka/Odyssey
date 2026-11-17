#include "aurora.h"
// PC 0x00782BB0 GOG 0x007810A0
unsigned char* CAuroraLayeredTexture::GetTextureDataPtr()
{
    if (m_pRes != NULL && m_pRes->GetDemands() >= 1)
    {
        return m_pRes->GetPLTDataPtr();
    }
    return 0;
}
// PC 0x00781290 GOG 0x00781060
int CAuroraLayeredTexture::GetTextureAttrib(unsigned long* pnWidth,
                                            unsigned long* pnHeight,
                                            unsigned long* pnReserved)
{
    if (m_pRes != NULL && m_pRes->GetDemands() >= 1)
    {
        return m_pRes->GetPLTAttrib(pnWidth, pnHeight, pnReserved);
    }
    else
    {
        return 0;
    }
}
// PC 0x007812D0 GOG 0x007810D0
int CAuroraLayeredTexture::Load()
{
    unsigned char* pPLTData;
    SPLTLayeredTrailer* pTrailer;
    unsigned long nWidth;
    unsigned long nHeight;
    unsigned long nReserved;

    if (m_pRes->Demand() == NULL)
    {
        return 0;
    }
    m_pRes->GetPLTAttrib(&nWidth, &nHeight, &nReserved);
    pPLTData = m_pRes->GetPLTDataPtr();
    if (pPLTData == NULL)
    {
        m_pRes->Release();
        return 0;
    }
    pTrailer = (SPLTLayeredTrailer*)(pPLTData + 2 * (nHeight * nWidth) + 4);
    pTrailer->pOwner = this;
    pTrailer->nType = 6;
    return 1;
}
// PC 0x00783630 GOG 0x00783460
void CAuroraLayeredTexture::Unload()
{
    if (m_pRes != NULL && m_pRes->GetDemands() >= 1)
    {
        m_pRes->Release();
    }
}
// PC 0x00781340 GOG 0x00781140
unsigned char* CAuroraLayeredTexture::GetPtrToResHelperInfo()
{
    unsigned char* nPLTDataPtr;
    unsigned long nWidth;
    unsigned long nHeight;
    unsigned long nReserved;

    if (!(m_pRes == NULL || m_pRes->GetDemands() < 1))
    {
        nPLTDataPtr = GetTextureDataPtr();
        GetTextureAttrib(&nWidth, &nHeight, &nReserved);
        return (nPLTDataPtr + 2 * (nHeight * nWidth)) + 4;
    }
    return 0;
}
// PC 0x00781590 GOG 0x00781390
CAuroraLayeredTexture::CAuroraLayeredTexture(CResRef cResRef, int bRequest)
    : CResHelper<CResPLT, RESTYPE_PLT>()
{
    SetResRef(cResRef, bRequest);
}
// PC 0x00781620 GOG 0x00781420
CAuroraLayeredTexture::~CAuroraLayeredTexture()
{
}
