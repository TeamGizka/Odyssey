#include <string.h>

#include "ResBWM.h"
// PC 0x006160B0
CResBWM::CResBWM()
{
    m_bServiced = 0;
}
// PC 0x006160D0
CResBWM::~CResBWM()
{
}
// PC 0x00404CF0
unsigned char* CResBWM::GetBWMDataPtr()
{
    return m_pBWMData;
}
// PC 0x007846A0
unsigned long CResBWM::GetBWMSize()
{
    return m_nBWMSize;
}
// PC 0x006160E0
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
// PC 0x00784760
int CResBWM::OnResourceFreed()
{
    m_bServiced = 0;
    m_nBWMSize = 0;
    m_pBWMData = 0;
    m_pBWMHeader = 0;
    return 1;
}
// PC 0x00616130
int CResBWM::IsBinary()
{
    return strncmp((const char*)m_pBWMData, "BWM V1.0", 8) == 0;
}
