#include "exoresman.h"
// PC 0x00620880 GOG 0x00620850 Xbox 0x0016E3C0
CResNCS::CResNCS()
{
    m_bServiced = 0;
    m_nNCSSize = 0;
    m_pNCSData = NULL;
}
// PC 0x006208A0 GOG 0x00620870
CResNCS::~CResNCS()
{
}
// PC 0x00404CF0 GOG 0x00404CF0 Xbox 0x00161340
char* CResNCS::GetNCSDataPtr()
{
    return (char*)m_pNCSData;
}
// PC 0x007846A0 GOG 0x00784450 Xbox 0x0015B870
unsigned long CResNCS::GetNCSSize()
{
    return m_nNCSSize;
}

// Dead code: not in the PC executable
int CResNCS::IsLoaded()
{
    return m_bServiced;
}
// PC 0x007845C0 GOG 0x00784460 Xbox 0x00331990
int CResNCS::OnResourceFreed()
{
    m_bServiced = 0;
    m_nNCSSize = 0;
    m_pNCSData = 0;
    return 1;
}
// PC 0x0077FE60 GOG 0x0062B5A0 Xbox 0x00161470
int CResNCS::OnResourceServiced()
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

    m_nNCSSize = m_nSize;
    m_pNCSData = pData;
    m_bServiced = 1;
    return 1;
}
// PC 0x0077FE60 GOG 0x0062B5A0 Xbox 0x00161470
int CResMDX::OnResourceServiced()
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

    m_nMDXSize = m_nSize;
    m_pMDXData = pData;
    m_bServiced = 1;
    return 1;
}
