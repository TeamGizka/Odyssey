#include "exoresman.h"
// PC 0x005CEA50  Xbox 0x00228BF0
CResMDL::CResMDL()
{
    m_bServiced = 0;
    m_nMDLSize = 0;
    m_pMDLData = 0;
    m_bAllocPrefix = 1;
}
// PC 0x005CEA80  Xbox 0x00228C20
CResMDL::~CResMDL()
{
}
// PC 0x00710E60  Xbox 0x00168C50
int CResMDL::OnResourceFreed()
{
    m_bServiced = 0;
    m_nMDLSize = 0;
    m_pMDLData = 0;
    return 1;
}
// PC 0x00404DD0
unsigned char* CResMDL::GetMDLDataPtr()
{
    return m_pMDLData;
}
// PC 0x00710E50
unsigned long CResMDL::GetMDLSize()
{
    return m_nMDLSize;
}
// PC 0x005D4C60
int CResMDL::OnResourceServiced()
{
    unsigned char* pData;

    pData = (unsigned char*)m_pResource;
    if (pData == NULL)
    {
        return 0;
    }

    if (m_bServiced != 0)
    {
        return 1;
    }

    m_nMDLSize = m_nSize;
    m_pMDLData = pData;
    m_bServiced = 1;
    return 1;
}
