#include "exoresman.h"
// PC 0x0062B6A0 GOG 0x0062B570
CResLYT::CResLYT()
{
    m_bServiced = 0;
    m_nLYTSize = 0;
    m_pLYTData = 0;
}
// PC 0x0062B6C0 GOG 0x0062B590
CResLYT::~CResLYT()
{
}
// PC 0x00404CF0 GOG 0x00404CF0
unsigned char* CResLYT::GetLYTDataPtr()
{
    return m_pLYTData;
}
// PC 0x007846A0 GOG 0x00784450
int CResLYT::GetLYTSize()
{
    return m_nLYTSize;
}
// PC 0x007845C0 GOG 0x00784460
int CResLYT::OnResourceFreed()
{
    m_bServiced = 0;
    m_nLYTSize = 0;
    m_pLYTData = 0;
    return 1;
}
// PC 0x0077FE60 GOG 0x0062B5A0
int CResLYT::OnResourceServiced()
{
    if (m_pResource == 0)
    {
        return 0;
    }

    if (m_bServiced != 0)
    {
        return 1;
    }

    m_nLYTSize = m_nSize;
    m_pLYTData = (unsigned char*)m_pResource;
    m_bServiced = 1;
    return 1;
}
