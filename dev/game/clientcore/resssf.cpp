#include "exoresman.h"
// PC 0x00749960 GOG 0x007497B0
CResSSF::CResSSF()
{
    m_bServiced = 0;
}
// PC 0x00749980 GOG 0x007497D0
CResSSF::~CResSSF()
{
}
// PC 0x00749990 GOG 0x007497E0
int CResSSF::OnResourceFreed()
{
    m_bServiced = 0;
    return 1;
}
// PC 0x007499A0 GOG 0x007497F0
int CResSSF::OnResourceServiced()
{
    if (m_pResource == 0)
        return 0;
    if (m_bServiced != 0)
        return 1;
    m_bServiced = 1;
    return 1;
}
