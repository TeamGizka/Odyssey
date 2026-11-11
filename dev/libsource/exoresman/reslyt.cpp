#include "exoresman.h"
// PC 0x005DF3A0  Xbox 0x0014ED60
CResLYT::CResLYT()
{
    m_bServiced = 0;
    m_nLYTSize = 0;
    m_pLYTData = 0;
}
// PC 0x005DF3C0  Xbox 0x0014ED80
CResLYT::~CResLYT()
{
}
// PC 0x00404DD0
unsigned char* CResLYT::GetLYTDataPtr()
{
    return m_pLYTData;
}
// PC 0x00710E50
int CResLYT::GetLYTSize()
{
    return m_nLYTSize;
}
// PC 0x00710E60
int CResLYT::OnResourceFreed()
{
    m_bServiced = 0;
    m_nLYTSize = 0;
    m_pLYTData = 0;
    return 1;
}
// PC 0x005D4C60
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
