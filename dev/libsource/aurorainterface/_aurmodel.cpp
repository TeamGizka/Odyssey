#include "aurora.h"
// PC 0x007838F0 GOG 0x007833D0 Xbox 0x0032FEC0
int CAuroraModel::GetModelDataSize()
{
    if (m_pRes != NULL && m_pRes->GetDemands() >= 1)
    {
        return m_pRes->GetMDLSize();
    }
    return 0;
}
// PC 0x00783FE0 GOG 0x00783400 Xbox 0x00331360
unsigned char* CAuroraModel::GetModelDataPtr()
{
    if (m_pRes != NULL && m_pRes->GetDemands() >= 1)
    {
        return m_pRes->GetMDLDataPtr();
    }
    return 0;
}
// PC 0x00783920 GOG 0x00783720 Xbox 0x00330D60
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
// PC 0x00783320 GOG 0x007826C0 Xbox 0x0032FF20
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
// PC 0x00783B30 GOG 0x00783930 Xbox 0x00330F30
CAuroraModel::CAuroraModel(CResRef cResRef, int bRequest)
    : CResHelper<CResMDL, RESTYPE_MDL>()
{
    SetResRef(cResRef, bRequest);
}
// PC 0x00783BC0 GOG 0x007839C0
CAuroraModel::~CAuroraModel()
{
}
// PC 0x00783630 GOG 0x00783460 Xbox 0x0032ED20
void CAuroraModel::Unload()
{
    if (m_pRes != NULL && m_pRes->GetDemands() >= 1)
    {
        m_pRes->Release();
    }
}
