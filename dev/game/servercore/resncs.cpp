#include "exoresman.h"
// PC 0x005D4C30  Xbox 0x0015BA10
CResNCS::CResNCS()
{
    m_bServiced = 0;
    m_nNCSSize = 0;
    m_pNCSData = NULL;
}
// PC 0x005D4C50  Xbox 0x0015BA30
CResNCS::~CResNCS()
{
}
// PC 0x00404DD0
char* CResNCS::GetNCSDataPtr()
{
    return (char*)m_pNCSData;
}
// PC 0x00710E50
unsigned long CResNCS::GetNCSSize()
{
    return m_nNCSSize;
}
// PC 0x00710E60
int CResNCS::OnResourceFreed()
{
    m_bServiced = 0;
    m_nNCSSize = 0;
    m_pNCSData = 0;
    return 1;
}
// PC 0x005D4C60
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
// PC 0x005D4C60
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
