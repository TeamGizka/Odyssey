#include "exobase.h"
// PC 0x005E9290  Xbox 0x00139D60
CExoCriticalSection::CExoCriticalSection()
{
    m_pInternal = new CExoCriticalSectionInternal;
}
// PC 0x005E9300  Xbox 0x00139DD0
void CExoCriticalSection::EnterCriticalSection()
{
    m_pInternal->EnterCriticalSection();
}
// PC 0x005E9310  Xbox 0x00139DE0
void CExoCriticalSection::LeaveCriticalSection()
{
    m_pInternal->LeaveCriticalSection();
}
// PC 0x005E9320  Xbox 0x00139DF0
CExoCriticalSection::~CExoCriticalSection()
{
    delete m_pInternal;
    m_pInternal = 0;
}
