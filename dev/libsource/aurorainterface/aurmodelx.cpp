#include "aurora.h"
// PC 0x007100A0
int CAuroraModelX::GetModelDataSize()
{
    if (m_pRes != NULL && m_pRes->GetDemands() >= 1)
    {
        return m_pRes->GetMDXSize();
    }
    return 0;
}
// PC 0x007107B0
unsigned char* CAuroraModelX::GetModelDataPtr()
{
    if (m_pRes != NULL && m_pRes->GetDemands() >= 1)
    {
        return m_pRes->GetMDXDataPtr();
    }
    return 0;
}
// PC 0x0070FDD0  Xbox 0x002FA030
int CAuroraModelX::Load()
{
    SMDXResPrefix* pPrefix;

    if (m_pRes->Demand() == NULL)
    {
        return 0;
    }
    pPrefix = (SMDXResPrefix*)(m_pRes->GetMDXDataPtr() - sizeof(SMDXResPrefix));
    pPrefix->nType = RESTYPE_MDX;
    pPrefix->pOwner = this;
    return 1;
}
// PC 0x0070FFE0  Xbox 0x002FA240
CAuroraModelX::CAuroraModelX(CResRef cResRef, int bRequest)
{
    SetResRef(cResRef, bRequest);
}
// PC 0x00710070  Xbox 0x002FA2D0
CAuroraModelX::~CAuroraModelX()
{
}
// PC 0x00710100
void CAuroraModelX::Unload()
{
    if (m_pRes != NULL && m_pRes->GetDemands() >= 1)
    {
        m_pRes->Release();
    }
}
// PC 0x0070FAF0
unsigned char* CAuroraModelX::GetPtrToResHelperInfo()
{
    unsigned char* nMDXDataPtr;

    if (m_pRes == NULL || m_pRes->GetDemands() < 1)
    {
        return 0;
    }
    nMDXDataPtr = GetModelDataPtr();
    return nMDXDataPtr - sizeof(SMDXResPrefix);
}
