#include "aurora.h"
// PC 0x00783FA0 GOG 0x00783DA0
int CAurora16BitTexture::GetTextureAttrib(unsigned long* pnA,
                                          unsigned long* pnB, float* pfC,
                                          int* pnD)
{
    if (m_pRes != 0 && m_pRes->GetDemands() >= 1)
    {
        return m_pRes->Get4PCAttrib(pnA, pnB, pfC, pnD);
    }
    return 0;
}
// PC 0x00783FE0 GOG 0x00783400
unsigned char* CAurora16BitTexture::GetTextureDataPtr()
{
    if (m_pRes != 0 && m_pRes->GetDemands() >= 1)
    {
        return m_pRes->Get4PCDataPtr();
    }
    return 0;
}
// PC 0x00784010 GOG 0x00783DE0
int CAurora16BitTexture::Load()
{
    S16BitTextureDataTrailer* pTrailer;
    int nResult = (int)m_pRes->Demand();
    if (nResult == 0)
    {
        return nResult;
    }
    pTrailer = (S16BitTextureDataTrailer*)(m_pRes->Get4PCDataPtr() - 22);
    pTrailer->pOwner = this;
    pTrailer->nType = 2059;
    return 1;
}
// PC 0x00783630 GOG 0x00783460
void CAurora16BitTexture::Unload()
{
    if (m_pRes != NULL && m_pRes->GetDemands() >= 1)
    {
        m_pRes->Release();
    }
}
// PC 0x00784040 GOG 0x00783E10
int CAurora16BitTexture::ReadTextureHeader(int* pnA, int* pnB, float* pfC,
                                           int* pnD)
{
    S16BitTextureHeader textureHeader;
    if (m_pRes == 0)
    {
        return 0;
    }
    if (m_pRes->ReadRaw(16, (char*)&textureHeader) == 0)
    {
        return 0;
    }
    double fValue = textureHeader.fAlpha;
    *pnA = textureHeader.nWidth;
    *pnB = textureHeader.nHeight;
    *pfC = fValue;
    *pnD = textureHeader.nFormat;
    return 1;
}
// PC 0x007840A0 GOG 0x00783E70
unsigned char* CAurora16BitTexture::GetPtrToResHelperInfo()
{
    if (m_pRes == 0 || m_pRes->GetDemands() < 1)
    {
        return 0;
    }
    return GetTextureDataPtr() - 22;
}
// PC 0x007842C0 GOG 0x00784090
CAurora16BitTexture::CAurora16BitTexture(CResRef cResRef, int bRequest)
    : CResHelper<CRes4PC, RESTYPE_4PC>()
{
    SetResRef(cResRef, bRequest);
}
// PC 0x00784350 GOG 0x00784120
CAurora16BitTexture::~CAurora16BitTexture()
{
}
