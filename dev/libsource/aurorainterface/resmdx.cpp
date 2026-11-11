#include "exoresman.h"
// PC 0x00784660
CResMDX::CResMDX()
{
    m_bServiced = 0;
    m_nMDXSize = 0;
    m_pMDXData = 0;
    m_bAllocPrefix = 1;
}
// PC 0x00784690
CResMDX::~CResMDX()
{
}
// PC 0x00404CF0
unsigned char* CResMDX::GetMDXDataPtr()
{
    return m_pMDXData;
}
// PC 0x007846A0
unsigned long CResMDX::GetMDXSize()
{
    return m_nMDXSize;
}
// PC 0x007845C0
int CResMDX::OnResourceFreed()
{
    m_bServiced = 0;
    m_nMDXSize = 0;
    m_pMDXData = 0;
    return 1;
}
