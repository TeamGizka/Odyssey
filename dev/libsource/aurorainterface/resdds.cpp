#include "exoresman.h"
// PC 0x007846D0 GOG 0x007844A0
CResDDS::CResDDS()
{
    m_bServiced = 0;
    m_nReserved = 0;
    m_pData = 0;
    m_pHeader = 0;
    m_bAllocPrefix = 1;
}
// PC 0x00784700 GOG 0x007844D0
CResDDS::~CResDDS()
{
}
// PC 0x00404CF0 GOG 0x00404CF0
unsigned char* CResDDS::GetDDSDataPtr()
{
    return m_pData;
}
// PC 0x00784760 GOG 0x00784610
int CResDDS::OnResourceFreed()
{
    m_bServiced = 0;
    m_nReserved = 0;
    m_pData = 0;
    m_pHeader = 0;
    return 1;
}
// PC 0x00784710 GOG 0x007844E0
int CResDDS::GetDDSAttrib(unsigned long* pnWidth, unsigned long* pnHeight, unsigned char* pnBpp, unsigned long* pnSize, float* pfAlpha)
{
    if (m_pHeader == 0)
    {
        return 0;
    }
    *pnWidth = m_pHeader[0];
    *pnHeight = m_pHeader[1];
    *pnBpp = ((unsigned char*)m_pHeader)[8];
    *pnSize = m_pHeader[3];
    *pfAlpha = ((float*)m_pHeader)[4];
    return 1;
}
// PC 0x00784780 GOG 0x00784530
int CResDDS::OnResourceServiced()
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
    m_pData = pResource + 20;
    m_bServiced = 1;
    return 1;
}
