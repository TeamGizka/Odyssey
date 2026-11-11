#include "exobase.h"
// PC 0x00634C90
CExoCriticalSection::CExoCriticalSection()
{
    m_pInternal = new CExoCriticalSectionInternal;
}
// PC 0x00634D00
void CExoCriticalSection::EnterCriticalSection()
{
    m_pInternal->EnterCriticalSection();
}
// PC 0x00634D10
void CExoCriticalSection::LeaveCriticalSection()
{
    m_pInternal->LeaveCriticalSection();
}
// PC 0x00634D20
CExoCriticalSection::~CExoCriticalSection()
{
    delete m_pInternal;
    m_pInternal = 0;
}
