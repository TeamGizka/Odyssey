#include "exoresman.h"
// PC 0x007847E0
CRes4PC::CRes4PC()
{
    m_bServiced = 0;
    m_nReserved = 0;
    m_pData = 0;
    m_pHeader = 0;
    m_bAllocPrefix = 1;
}
// PC 0x00784810
CRes4PC::~CRes4PC()
{
}
// PC 0x00404CF0
unsigned char* CRes4PC::Get4PCDataPtr()
{
    return m_pData;
}
// PC 0x00784820
int CRes4PC::Get4PCAttrib(unsigned long* pnA, unsigned long* pnB, float* pfC, int* pnD)
{
    if (m_pHeader == 0)
    {
        return 0;
    }
    *pnA = m_pHeader[0];
    *pnB = m_pHeader[1];
    *pfC = ((float*)m_pHeader)[2];
    *pnD = m_pHeader[3];
    return 1;
}
// PC 0x00784760
int CRes4PC::OnResourceFreed()
{
    m_bServiced = 0;
    m_nReserved = 0;
    m_pData = 0;
    m_pHeader = 0;
    return 1;
}
// PC 0x00784860
int CRes4PC::OnResourceServiced()
{
    unsigned char* pResource = (unsigned char*)m_pResource;
    if (pResource == 0)
    {
        return 0;
    }
    if (m_bServiced != 0)
    {
        return 1;
    }
    if (m_pHeader != 0)
    {
        return 0;
    }
    if (m_pData != 0)
    {
        return 0;
    }
    m_pHeader = (unsigned long*)pResource;
    m_pData = pResource + 16;
    m_bServiced = 1;
    return 1;
}
