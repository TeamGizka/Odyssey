#include "aurora.h"
// PC 0x007838F0
int CAuroraTXI::GetTXIDataSize()
{
    if (m_pRes != NULL && m_pRes->GetDemands() >= 1)
    {
        return m_pRes->GetTXISize();
    }
    return 0;
}
// PC 0x00783FE0
unsigned char* CAuroraTXI::GetTXIDataPtr()
{
    if (m_pRes != NULL && m_pRes->GetDemands() >= 1)
    {
        return (unsigned char*)m_pRes->GetTXIDataPtr();
    }
    return 0;
}
// PC 0x007832F0
int CAuroraTXI::Load()
{
    STXIResourcePrefix* pPrefix;

    if (m_pRes->Demand() == NULL)
    {
        return 0;
    }
    pPrefix =
        (STXIResourcePrefix*)((unsigned char*)m_pRes->GetTXIDataPtr() - 6);
    pPrefix->pOwner = this;
    pPrefix->nType = RESTYPE_TXI;
    return 1;
}
// PC 0x00783630
void CAuroraTXI::Unload()
{
    if (m_pRes != NULL && m_pRes->GetDemands() >= 1)
    {
        m_pRes->Release();
    }
}
// PC 0x00783320
unsigned char* CAuroraTXI::GetPtrToResHelperInfo()
{
    unsigned char* nTXIDataPtr;

    if (m_pRes == NULL || m_pRes->GetDemands() < 1)
    {
        return 0;
    }
    nTXIDataPtr = GetTXIDataPtr();
    return nTXIDataPtr - sizeof(STXIResourcePrefix);
}
// PC 0x00783540
CAuroraTXI::CAuroraTXI(CResRef cResRef, int bRequest)
    : CResHelper<CResTXI, RESTYPE_TXI>()
{
    SetResRef(cResRef, bRequest);
}
// PC 0x007835D0
CAuroraTXI::~CAuroraTXI()
{
}
