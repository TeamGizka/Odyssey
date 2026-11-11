#include <string.h>

#include "ResBWM.h"
// PC 0x005CEAB0  Xbox 0x00228C70
CResBWM::CResBWM()
{
    m_bServiced = 0;
}
// PC 0x005CEAD0  Xbox 0x00228C90
CResBWM::~CResBWM()
{
}
// PC 0x00404DD0
unsigned char* CResBWM::GetBWMDataPtr()
{
    return m_pBWMData;
}
// PC 0x00710E50
unsigned long CResBWM::GetBWMSize()
{
    return m_nBWMSize;
}
// PC 0x005CEAE0
int CResBWM::OnResourceServiced()
{
    int nResult = (int)m_pResource;
    if (nResult == 0)
    {
        return nResult;
    }
    if (m_bServiced != 0)
    {
        return 1;
    }
    m_nBWMSize = m_nSize;
    m_pBWMData = (unsigned char*)m_pResource;
    m_pBWMHeader = (_CSWWalkMeshHeader*)m_pBWMData;
    if (strncmp((const char*)m_pBWMData, "BWM V1.0", 8) != 0)
    {
        m_pBWMHeader = NULL;
    }
    m_bServiced = 1;
    return 1;
}
// PC 0x00711010  Xbox 0x00228CA0
int CResBWM::OnResourceFreed()
{
    m_bServiced = 0;
    m_nBWMSize = 0;
    m_pBWMData = 0;
    m_pBWMHeader = 0;
    return 1;
}
// PC 0x005CEB30  Xbox 0x00228D10
int CResBWM::IsBinary()
{
    return strncmp((const char*)m_pBWMData, "BWM V1.0", 8) == 0;
}
