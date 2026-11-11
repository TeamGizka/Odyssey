#include "exoresman.h"
// PC 0x0070C690  Xbox 0x0014EDB0
CResLIP::CResLIP()
{
    m_bServiced = 0;
    m_nLIPSize = 0;
    m_pLIPData = 0;
}
// PC 0x0070C6B0  Xbox 0x0014EDD0
CResLIP::~CResLIP()
{
}
// PC 0x00404DD0
char* CResLIP::GetLIPDataPtr()
{
    return m_pLIPData;
}
// PC 0x00710E60
int CResLIP::OnResourceFreed()
{
    m_bServiced = 0;
    m_nLIPSize = 0;
    m_pLIPData = 0;
    return 1;
}
// PC 0x005D4C60
int CResLIP::OnResourceServiced()
{
    char* pData;

    pData = (char*)m_pResource;
    if (pData == NULL)
    {
        return 0;
    }

    if (m_bServiced != 0)
    {
        return 1;
    }

    m_nLIPSize = m_nSize;
    m_pLIPData = pData;
    m_bServiced = 1;
    return 1;
}
