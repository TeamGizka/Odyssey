#include "exobase.h"
// PC 0x005EC250  Xbox 0x00139E20
CExoCriticalSectionInternal::CExoCriticalSectionInternal()
{
    ::InitializeCriticalSection(&m_CriticalSection);
}
// PC 0x005EC260
CExoCriticalSectionInternal::~CExoCriticalSectionInternal()
{
    DeleteCriticalSection(&m_CriticalSection);
}
// PC 0x005EC270  Xbox 0x00139E30
void CExoCriticalSectionInternal::EnterCriticalSection()
{
    ::EnterCriticalSection(&m_CriticalSection);
}
// PC 0x005EC280  Xbox 0x00139E40
void CExoCriticalSectionInternal::LeaveCriticalSection()
{
    ::LeaveCriticalSection(&m_CriticalSection);
}