#include "exoresman.h"
// PC 0x0077FE30 GOG 0x0077FC30 Xbox 0x00161450
CResLIP::CResLIP()
{
    m_bServiced = 0;
    m_nLIPSize = 0;
    m_pLIPData = 0;
}
// PC 0x0077FE50 GOG 0x0077FC50
CResLIP::~CResLIP()
{
}
// PC 0x00404CF0 GOG 0x00404CF0 Xbox 0x00161340
char* CResLIP::GetLIPDataPtr()
{
    return m_pLIPData;
}

// Dead code: not in the PC executable
unsigned long CResLIP::GetLIPSize()
{
    return m_nLIPSize;
}

// Dead code: not in the PC executable
int CResLIP::IsLoaded()
{
    return m_bServiced;
}
// PC 0x007845C0 GOG 0x00784460 Xbox 0x00331990
int CResLIP::OnResourceFreed()
{
    m_bServiced = 0;
    m_nLIPSize = 0;
    m_pLIPData = 0;
    return 1;
}
// PC 0x0077FE60 GOG 0x0062B5A0 Xbox 0x00161470
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
