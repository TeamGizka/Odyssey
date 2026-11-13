#include "aurora.h"
// PC 0x007838F0 GOG 0x007833D0
int CAuroraModelX::GetModelDataSize()
{
    if (m_pRes != NULL && m_pRes->GetDemands() >= 1)
    {
        return m_pRes->GetMDXSize();
    }
    return 0;
}
// PC 0x00783FE0 GOG 0x00783400
unsigned char* CAuroraModelX::GetModelDataPtr()
{
    if (m_pRes != NULL && m_pRes->GetDemands() >= 1)
    {
        return m_pRes->GetMDXDataPtr();
    }
    return 0;
}
// PC 0x00783600 GOG 0x00783430
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
// PC 0x00783830 GOG 0x00783660
CAuroraModelX::CAuroraModelX(CResRef cResRef, int bRequest)
{
    SetResRef(cResRef, bRequest);
}
// PC 0x007838C0 GOG 0x007836F0
CAuroraModelX::~CAuroraModelX()
{
}
// PC 0x00783630 GOG 0x00783460
void CAuroraModelX::Unload()
{
    if (m_pRes != NULL && m_pRes->GetDemands() >= 1)
    {
        m_pRes->Release();
    }
}
// PC 0x00783320 GOG 0x007826C0
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
