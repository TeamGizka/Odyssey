#include "exoresman.h"
// PC 0x00710E10  Xbox 0x002FB060
CResMDX::CResMDX()
{
    m_bServiced = 0;
    m_nMDXSize = 0;
    m_pMDXData = 0;
    m_bAllocPrefix = 1;
}
// PC 0x00710E40
CResMDX::~CResMDX()
{
}
// PC 0x00404DD0
unsigned char* CResMDX::GetMDXDataPtr()
{
    return m_pMDXData;
}
// PC 0x00710E50
unsigned long CResMDX::GetMDXSize()
{
    return m_nMDXSize;
}
// PC 0x00710E60
int CResMDX::OnResourceFreed()
{
    m_bServiced = 0;
    m_nMDXSize = 0;
    m_pMDXData = 0;
    return 1;
}
