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
int CAuroraLayeredTexture::GetTextureAttrib(unsigned long* pnA,
                                            unsigned long* pnB,
                                            unsigned long* pnC)
{
    if (m_pRes != NULL && m_pRes->GetDemands() >= 1)
    {
        return m_pRes->GetPLTAttrib(pnA, pnB, pnC);
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
    unsigned long nA;
    unsigned long nB;
    unsigned long nC;

    if (m_pRes->Demand() == NULL)
    {
        return 0;
    }
    m_pRes->GetPLTAttrib(&nA, &nB, &nC);
    pPLTData = m_pRes->GetPLTDataPtr();
    if (pPLTData == NULL)
    {
        m_pRes->Release();
        return 0;
    }
    pTrailer = (SPLTLayeredTrailer*)(pPLTData + 2 * (nB * nA) + 4);
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
    unsigned long nA;
    unsigned long nB;
    unsigned long nC;

    if (!(m_pRes == NULL || m_pRes->GetDemands() < 1))
    {
        nPLTDataPtr = GetTextureDataPtr();
        GetTextureAttrib(&nA, &nB, &nC);
        return (nPLTDataPtr + 2 * (nB * nA)) + 4;
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
