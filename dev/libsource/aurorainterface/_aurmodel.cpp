#include "aurora.h"
// PC 0x007838F0
int CAuroraModel::GetModelDataSize()
{
    if (m_pRes != NULL && m_pRes->GetDemands() >= 1)
    {
        return m_pRes->GetMDLSize();
    }
    return 0;
}
// PC 0x00783FE0
unsigned char* CAuroraModel::GetModelDataPtr()
{
    if (m_pRes != NULL && m_pRes->GetDemands() >= 1)
    {
        return m_pRes->GetMDLDataPtr();
    }
    return 0;
}
// PC 0x00783920
int CAuroraModel::Load()
{
    unsigned char* pMDLData;
    SMDLDataTrailer* pTrailer;

    if (m_pRes->Demand() == NULL)
    {
        return 0;
    }
    pMDLData = m_pRes->GetMDLDataPtr();
    pTrailer = (SMDLDataTrailer*)(pMDLData - 6);
    pTrailer->pOwner = this;
    pTrailer->nType = RESTYPE_MDL;
    return 1;
}
// PC 0x00783320
unsigned char* CAuroraModel::GetPtrToResHelperInfo()
{
    unsigned char* nMDLDataPtr;

    if (m_pRes == NULL || m_pRes->GetDemands() < 1)
    {
        return 0;
    }
    nMDLDataPtr = GetModelDataPtr();
    return nMDLDataPtr - sizeof(SMDLDataTrailer);
}
// PC 0x00783B30
CAuroraModel::CAuroraModel(CResRef cResRef, int bRequest)
    : CResHelper<CResMDL, RESTYPE_MDL>()
{
    SetResRef(cResRef, bRequest);
}
// PC 0x00783BC0
CAuroraModel::~CAuroraModel()
{
}
// PC 0x00783630
void CAuroraModel::Unload()
{
    if (m_pRes != NULL && m_pRes->GetDemands() >= 1)
    {
        m_pRes->Release();
    }
}
