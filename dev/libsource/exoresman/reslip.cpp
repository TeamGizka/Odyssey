#include "exoresman.h"
// PC 0x0077FE30
CResLIP::CResLIP()
{
    m_bServiced = 0;
    m_nLIPSize = 0;
    m_pLIPData = 0;
}
// PC 0x0077FE50
CResLIP::~CResLIP()
{
}
// PC 0x00404CF0
char* CResLIP::GetLIPDataPtr()
{
    return m_pLIPData;
}
// PC 0x007845C0
int CResLIP::OnResourceFreed()
{
    m_bServiced = 0;
    m_nLIPSize = 0;
    m_pLIPData = 0;
    return 1;
}
// PC 0x0077FE60
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
