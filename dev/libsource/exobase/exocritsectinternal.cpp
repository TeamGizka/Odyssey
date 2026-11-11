#include "exobase.h"
// PC 0x006379F0
CExoCriticalSectionInternal::CExoCriticalSectionInternal()
{
    ::InitializeCriticalSection(&m_CriticalSection);
}
// PC 0x00637A00
CExoCriticalSectionInternal::~CExoCriticalSectionInternal()
{
    DeleteCriticalSection(&m_CriticalSection);
}
// PC 0x00637A10
void CExoCriticalSectionInternal::EnterCriticalSection()
{
    ::EnterCriticalSection(&m_CriticalSection);
}
// PC 0x00637A20
void CExoCriticalSectionInternal::LeaveCriticalSection()
{
    ::LeaveCriticalSection(&m_CriticalSection);
}