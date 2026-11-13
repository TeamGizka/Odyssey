#include "exoresman.h"
// PC 0x00784580 GOG 0x00784350
CResVIS::CResVIS()
{
    m_bServiced = 0;
    m_nVISSize = 0;
    m_pVISData = 0;
    m_bAllocPrefix = 1;
}
// PC 0x007845B0 GOG 0x00784380
CResVIS::~CResVIS()
{
}
// PC 0x00404CF0 GOG 0x00404CF0
unsigned char* CResVIS::GetVISDataPtr()
{
    return m_pVISData;
}
// PC 0x007846A0 GOG 0x00784450
unsigned long CResVIS::GetVISSize()
{
    return m_nVISSize;
}
// PC 0x007845C0 GOG 0x00784460
int CResVIS::OnResourceFreed()
{
    m_bServiced = 0;
    m_nVISSize = 0;
    m_pVISData = 0;
    return 1;
}
// PC 0x0077FE60 GOG 0x0062B5A0
int CResVIS::OnResourceServiced()
{
    if (m_pResource == 0)
    {
        return 0;
    }
    if (m_bServiced != 0)
    {
        return 1;
    }
    m_nVISSize = m_nSize;
    m_pVISData = (unsigned char*)m_pResource;
    m_bServiced = 1;
    return 1;
}
