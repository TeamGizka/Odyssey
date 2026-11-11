#include "exoresman.h"
// PC 0x00710DB0
CResTXI::CResTXI()
{
    m_bServiced = 0;
    m_nTXISize = 0;
    m_pTXIData = 0;
    m_bAllocPrefix = 1;
}
// PC 0x00710DE0
CResTXI::~CResTXI()
{
}
// PC 0x00404DD0
void* CResTXI::GetTXIDataPtr()
{
    return m_pTXIData;
}
// PC 0x00710E50
unsigned long CResTXI::GetTXISize()
{
    return m_nTXISize;
}
// PC 0x00710E60
int CResTXI::OnResourceFreed()
{
    m_bServiced = 0;
    m_nTXISize = 0;
    m_pTXIData = 0;
    return 1;
}
// PC 0x005D4C60
int CResTXI::OnResourceServiced()
{
    if (m_pResource == 0)
    {
        return 0;
    }
    if (m_bServiced != 0)
    {
        return 1;
    }
    m_nTXISize = m_nSize;
    m_pTXIData = (unsigned char*)m_pResource;
    m_bServiced = 1;
    return 1;
}
