#include "exobase.h"
// PC 0x005EA420  Xbox 0x00136200
CExoRand::CExoRand()
{
    m_pInternal = new CExoRandInternal();
}
// PC 0x005EA490  Xbox 0x00136270
unsigned long CExoRand::Rand()
{
    return m_pInternal->Rand();
}
// PC 0x005EA4A0  Xbox 0x00136280
CExoRand::~CExoRand()
{
    if (m_pInternal != 0)
    {
        delete m_pInternal;
        m_pInternal = 0;
    }
}
