#include "exoresman.h"
// PC 0x00784600 GOG 0x007843B0 Xbox 0x00331900
CResTXI::CResTXI()
{
    m_bServiced = 0;
    m_nTXISize = 0;
    m_pTXIData = 0;
    m_bAllocPrefix = 1;
}
// PC 0x00784630 GOG 0x007843E0
CResTXI::~CResTXI()
{
}
// PC 0x00404CF0 GOG 0x00404CF0 Xbox 0x00161340
void* CResTXI::GetTXIDataPtr()
{
    return m_pTXIData;
}
// PC 0x007846A0 GOG 0x00784450 Xbox 0x0015B870
unsigned long CResTXI::GetTXISize()
{
    return m_nTXISize;
}

// Dead code: not in the PC executable
int CResTXI::IsLoaded()
{
    return m_bServiced;
}
// PC 0x007845C0 GOG 0x00784460 Xbox 0x00331990
int CResTXI::OnResourceFreed()
{
    m_bServiced = 0;
    m_nTXISize = 0;
    m_pTXIData = 0;
    return 1;
}
// PC 0x0077FE60 GOG 0x0062B5A0 Xbox 0x00161470
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
