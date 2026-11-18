#include "exoresman.h"
// PC 0x00710F90  Xbox 0x002FB1E0
CRes4PC::CRes4PC()
{
    m_bServiced = 0;
    m_nReserved = 0;
    m_pData = 0;
    m_pHeader = 0;
    m_bAllocPrefix = 1;
}
// PC 0x00710FC0  Xbox 0x002FB210
CRes4PC::~CRes4PC()
{
}
// Dead code: not in the PC executable
int CRes4PC::Copy4PCData(unsigned char* p4PCData)
{
    if (m_bServiced == 0)
    {
        return 0;
    }
    memcpy(p4PCData, m_pData, m_nReserved);
    return 1;
}
// PC 0x00404DD0
unsigned char* CRes4PC::Get4PCDataPtr()
{
    return m_pData;
}
// PC 0x00710FD0  Xbox 0x002FB220
int CRes4PC::Get4PCAttrib(unsigned long* pnWidth, unsigned long* pnHeight, float* pfAlpha, int* pnFormat)
{
    if (m_pHeader == 0)
    {
        return 0;
    }
    *pnWidth = m_pHeader[0];
    *pnHeight = m_pHeader[1];
    *pfAlpha = ((float*)m_pHeader)[2];
    *pnFormat = m_pHeader[3];
    return 1;
}
// Dead code: not in the PC executable
unsigned long* CRes4PC::Get4PCHeaderPtr()
{
    return m_pHeader;
}
// Dead code: not in the PC executable
int CRes4PC::Get4PCSize()
{
    return m_nReserved;
}
// Dead code: not in the PC executable
int CRes4PC::IsLoaded()
{
    return m_bServiced;
}
// PC 0x00711010
int CRes4PC::OnResourceFreed()
{
    m_bServiced = 0;
    m_nReserved = 0;
    m_pData = 0;
    m_pHeader = 0;
    return 1;
}
// PC 0x00711030  Xbox 0x002FB270
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
