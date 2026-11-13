#include "exoresman.h"
// PC 0x0041FF20 GOG 0x0041FF30
CRes2DA::CRes2DA()
{
    m_bServiced = 0;
    m_n2DASize = 0;
    m_p2DAData = 0;
    m_nUnknown34 = 0;
    m_bPacked = 0;
}
// PC 0x0041FF50 GOG 0x0041FF60
CRes2DA::~CRes2DA()
{
}
// PC 0x00404CF0 GOG 0x00404CF0
void* CRes2DA::Get2DADataPtr()
{
    return m_p2DAData;
}
// PC 0x007846A0 GOG 0x00784450
unsigned long CRes2DA::Get2DASize()
{
    return m_n2DASize;
}
// PC 0x0041FF60 GOG 0x0041FF70
int CRes2DA::OnResourceFreed()
{
    m_bServiced = 0;
    m_n2DASize = 0;
    m_p2DAData = 0;
    m_nUnknown34 = 0;
    m_bPacked = 0;
    return 1;
}
// PC 0x0041FF80 GOG 0x0041FF90
int CRes2DA::OnResourceServiced()
{
    unsigned long* pData = (unsigned long*)m_pResource;
    if (pData == 0)
    {
        return 0;
    }

    if (m_bServiced != 0)
    {
        return 1;
    }

    m_n2DASize = m_nSize - 8;
    m_nUnknown34 = (unsigned long)pData;
    if (pData[0] != 0x20414432)
    {
        OnResourceFreed();
        return 0;
    }

    if (pData[1] == 0x622E3256)
    {
        m_bPacked = 1;
    }
    else if (pData[1] != 0x302E3256)
    {
        OnResourceFreed();
        return 0;
    }

    m_p2DAData = pData + 2;
    m_bServiced = 1;
    return 1;
}
