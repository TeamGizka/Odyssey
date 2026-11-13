#include "exobase.h"
// PC 0x00634C90 GOG 0x00634B80
CExoCriticalSection::CExoCriticalSection()
{
    m_pInternal = new CExoCriticalSectionInternal;
}
// PC 0x00634D00 GOG 0x00634BF0
void CExoCriticalSection::EnterCriticalSection()
{
    m_pInternal->EnterCriticalSection();
}
// PC 0x00634D10 GOG 0x00634C00
void CExoCriticalSection::LeaveCriticalSection()
{
    m_pInternal->LeaveCriticalSection();
}
// PC 0x00634D20 GOG 0x00634C10
CExoCriticalSection::~CExoCriticalSection()
{
    delete m_pInternal;
    m_pInternal = 0;
}
