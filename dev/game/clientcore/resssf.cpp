#include "exoresman.h"
// PC 0x006DB650  Xbox 0x0010CAB0
CResSSF::CResSSF()
{
    m_bServiced = 0;
}
// PC 0x006DB670  Xbox 0x0010CAD0
CResSSF::~CResSSF()
{
}
// PC 0x006DB680  Xbox 0x0010CAE0
int CResSSF::OnResourceFreed()
{
    m_bServiced = 0;
    return 1;
}
// PC 0x006DB690  Xbox 0x0010CAF0
int CResSSF::OnResourceServiced()
{
    if (m_pResource == 0)
        return 0;
    if (m_bServiced != 0)
        return 1;
    m_bServiced = 1;
    return 1;
}
