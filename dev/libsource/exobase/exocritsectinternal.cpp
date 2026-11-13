#include "exobase.h"
// PC 0x006379F0 GOG 0x006378F0
CExoCriticalSectionInternal::CExoCriticalSectionInternal()
{
    ::InitializeCriticalSection(&m_CriticalSection);
}
// PC 0x00637A00 GOG 0x00637900
CExoCriticalSectionInternal::~CExoCriticalSectionInternal()
{
    DeleteCriticalSection(&m_CriticalSection);
}
// PC 0x00637A10 GOG 0x00637910
void CExoCriticalSectionInternal::EnterCriticalSection()
{
    ::EnterCriticalSection(&m_CriticalSection);
}
// PC 0x00637A20 GOG 0x00637920
void CExoCriticalSectionInternal::LeaveCriticalSection()
{
    ::LeaveCriticalSection(&m_CriticalSection);
}