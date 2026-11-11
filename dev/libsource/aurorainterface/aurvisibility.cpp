#include "aurora.h"
// PC 0x007100A0  Xbox 0x002F9240
int CAuroraVisibility::GetVisibilityDataSize()
{
    if (m_pRes != NULL && m_pRes->GetDemands() >= 1)
    {
        return m_pRes->GetVISSize();
    }
    return 0;
}
// PC 0x007107B0  Xbox 0x002F9270
unsigned char* CAuroraVisibility::GetVisibilityDataPtr()
{
    if (m_pRes != NULL && m_pRes->GetDemands() >= 1)
    {
        return m_pRes->GetVISDataPtr();
    }
    return 0;
}
// PC 0x0070F060  Xbox 0x002F92A0
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
// PC 0x00710100  Xbox 0x002F92D0
void CAuroraVisibility::Unload()
{
    if (m_pRes != NULL && m_pRes->GetDemands() >= 1)
    {
        m_pRes->Release();
    }
}
// PC 0x0070FAF0
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
// PC 0x0070F270  Xbox 0x002F94D0
CAuroraVisibility::CAuroraVisibility(CResRef cResRef, int bRequest)
    : CResHelper<CResVIS, RESTYPE_VIS>()
{
    SetResRef(cResRef, bRequest);
}
// PC 0x0070F300  Xbox 0x002F9560
CAuroraVisibility::~CAuroraVisibility()
{
}
