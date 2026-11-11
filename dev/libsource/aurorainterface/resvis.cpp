#include "exoresman.h"
// PC 0x00710D50  Xbox 0x002FAF80
CResVIS::CResVIS()
{
    m_bServiced = 0;
    m_nVISSize = 0;
    m_pVISData = 0;
    m_bAllocPrefix = 1;
}
// PC 0x00710D80  Xbox 0x002FAFB0
CResVIS::~CResVIS()
{
}
// PC 0x00404DD0
unsigned char* CResVIS::GetVISDataPtr()
{
    return m_pVISData;
}
// PC 0x00710E50
unsigned long CResVIS::GetVISSize()
{
    return m_nVISSize;
}
// PC 0x00710E60  Xbox 0x002FB020
int CResVIS::OnResourceFreed()
{
    m_bServiced = 0;
    m_nVISSize = 0;
    m_pVISData = 0;
    return 1;
}
// PC 0x005D4C60  Xbox 0x002FB0A0
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
