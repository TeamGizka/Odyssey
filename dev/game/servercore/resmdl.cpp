#include "exoresman.h"
// PC 0x00616050 GOG 0x00616030 Xbox 0x00265940
CResMDL::CResMDL()
{
    m_bServiced = 0;
    m_nMDLSize = 0;
    m_pMDLData = 0;
    m_bAllocPrefix = 1;
}
// PC 0x00616080 GOG 0x00616060
CResMDL::~CResMDL()
{
}
// PC 0x007845C0 GOG 0x00784460 Xbox 0x00331990
int CResMDL::OnResourceFreed()
{
    m_bServiced = 0;
    m_nMDLSize = 0;
    m_pMDLData = 0;
    return 1;
}
// PC 0x00404CF0 GOG 0x00404CF0 Xbox 0x00161340
unsigned char* CResMDL::GetMDLDataPtr()
{
    return m_pMDLData;
}
// PC 0x007846A0 GOG 0x00784450 Xbox 0x0015B870
unsigned long CResMDL::GetMDLSize()
{
    return m_nMDLSize;
}

// Dead code: not in the PC executable
int CResMDL::IsLoaded()
{
    return m_bServiced;
}
// PC 0x0077FE60 GOG 0x0062B5A0 Xbox 0x00161470
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
