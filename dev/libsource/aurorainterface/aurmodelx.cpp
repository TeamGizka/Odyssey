#include "aurora.h"
// PC 0x007838F0
int CAuroraModelX::GetModelDataSize()
{
    if (m_pRes != NULL && m_pRes->GetDemands() >= 1)
    {
        return m_pRes->GetMDXSize();
    }
    return 0;
}
// PC 0x00783FE0
unsigned char* CAuroraModelX::GetModelDataPtr()
{
    if (m_pRes != NULL && m_pRes->GetDemands() >= 1)
    {
        return m_pRes->GetMDXDataPtr();
    }
    return 0;
}
// PC 0x00783600
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
// PC 0x00783830
CAuroraModelX::CAuroraModelX(CResRef cResRef, int bRequest)
{
    SetResRef(cResRef, bRequest);
}
// PC 0x007838C0
CAuroraModelX::~CAuroraModelX()
{
}
// PC 0x00783630
void CAuroraModelX::Unload()
{
    if (m_pRes != NULL && m_pRes->GetDemands() >= 1)
    {
        m_pRes->Release();
    }
}
// PC 0x00783320
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
