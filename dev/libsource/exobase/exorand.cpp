#include "exobase.h"
// PC 0x00635E10
CExoRand::CExoRand()
{
    m_pInternal = new CExoRandInternal();
}
// PC 0x00635E80
unsigned long CExoRand::Rand()
{
    return m_pInternal->Rand();
}
// PC 0x00635E90
CExoRand::~CExoRand()
{
    if (m_pInternal != 0)
    {
        delete m_pInternal;
        m_pInternal = 0;
    }
}
