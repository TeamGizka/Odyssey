#include "exobase.h"
// PC 0x00634C90 GOG 0x00634B80 Xbox 0x0014D8C0
CExoCriticalSection::CExoCriticalSection()
{
    m_pInternal = new CExoCriticalSectionInternal;
}
// PC 0x00634D00 GOG 0x00634BF0 Xbox 0x0014D8F0
void CExoCriticalSection::EnterCriticalSection()
{
    m_pInternal->EnterCriticalSection();
}
// PC 0x00634D10 GOG 0x00634C00 Xbox 0x0014D900
void CExoCriticalSection::LeaveCriticalSection()
{
    m_pInternal->LeaveCriticalSection();
}
// PC 0x00634D20 GOG 0x00634C10 Xbox 0x0014D910
CExoCriticalSection::~CExoCriticalSection()
{
    delete m_pInternal;
    m_pInternal = 0;
}
