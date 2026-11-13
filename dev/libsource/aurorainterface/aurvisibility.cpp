#include "aurora.h"
// PC 0x007838F0 GOG 0x007833D0
int CAuroraVisibility::GetVisibilityDataSize()
{
    if (m_pRes != NULL && m_pRes->GetDemands() >= 1)
    {
        return m_pRes->GetVISSize();
    }
    return 0;
}
// PC 0x00783FE0 GOG 0x00783400
unsigned char* CAuroraVisibility::GetVisibilityDataPtr()
{
    if (m_pRes != NULL && m_pRes->GetDemands() >= 1)
    {
        return m_pRes->GetVISDataPtr();
    }
    return 0;
}
// PC 0x00782890 GOG 0x00782690
int CAuroraVisibility::Load()
{
    unsigned char* pVISData;
    SVISDataTrailer* pTrailer;

    if (m_pRes->Demand() == NULL)
    {
        return 0;
    }
    pVISData = m_pRes->GetVISDataPtr();
    pTrailer = (SVISDataTrailer*)(pVISData - 6);
    pTrailer->pOwner = this;
    pTrailer->nType = RESTYPE_VIS;
    return 1;
}
// PC 0x00783630 GOG 0x00783460
void CAuroraVisibility::Unload()
{
    if (m_pRes != NULL && m_pRes->GetDemands() >= 1)
    {
        m_pRes->Release();
    }
}
// PC 0x00783320 GOG 0x007826C0
unsigned char* CAuroraVisibility::GetPtrToResHelperInfo()
{
    unsigned char* nVISDataPtr;

    if (m_pRes == NULL || m_pRes->GetDemands() < 1)
    {
        return 0;
    }
    nVISDataPtr = GetVisibilityDataPtr();
    return nVISDataPtr - sizeof(SVISDataTrailer);
}
// PC 0x00782AA0 GOG 0x007828E0
CAuroraVisibility::CAuroraVisibility(CResRef cResRef, int bRequest)
    : CResHelper<CResVIS, RESTYPE_VIS>()
{
    SetResRef(cResRef, bRequest);
}
// PC 0x00782B30 GOG 0x00782970
CAuroraVisibility::~CAuroraVisibility()
{
}
