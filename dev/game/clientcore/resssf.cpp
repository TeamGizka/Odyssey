#include "exoresman.h"
// PC 0x00749960
CResSSF::CResSSF()
{
    m_bServiced = 0;
}
// PC 0x00749980
CResSSF::~CResSSF()
{
}
// PC 0x00749990
int CResSSF::OnResourceFreed()
{
    m_bServiced = 0;
    return 1;
}
// PC 0x007499A0
int CResSSF::OnResourceServiced()
{
    if (m_pResource == 0)
        return 0;
    if (m_bServiced != 0)
        return 1;
    m_bServiced = 1;
    return 1;
}
