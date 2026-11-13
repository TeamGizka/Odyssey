#include "exoresman.h"
// PC 0x00620880 GOG 0x00620850
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
// PC 0x00404CF0 GOG 0x00404CF0
char* CResNCS::GetNCSDataPtr()
{
    return (char*)m_pNCSData;
}
// PC 0x007846A0 GOG 0x00784450
unsigned long CResNCS::GetNCSSize()
{
    return m_nNCSSize;
}
// PC 0x007845C0 GOG 0x00784460
int CResNCS::OnResourceFreed()
{
    m_bServiced = 0;
    m_nNCSSize = 0;
    m_pNCSData = 0;
    return 1;
}
// PC 0x0077FE60 GOG 0x0062B5A0
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
// PC 0x0077FE60 GOG 0x0062B5A0
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
