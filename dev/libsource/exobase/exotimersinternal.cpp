#include "exobase.h"
#if defined(_WIN32) && !defined(_XBOX)
#include <windows.h>
#else
#include "platform.h"
#endif
// PC 0x005EC160  Xbox 0x00138FD0
CExoTimersInternal::CExoTimersInternal()
{
    LARGE_INTEGER nFrequency;
    m_bHighResolution = QueryPerformanceFrequency(&nFrequency);
    if (m_bHighResolution == 1)
    {
        m_nFrequency = nFrequency.QuadPart;
        m_bLowFrequency = 0;
        if ((unsigned __int64)nFrequency.QuadPart / 1000000 < 200)
        {
            m_bLowFrequency = 1;
            m_dMicrosecondsPerTick =
                1.0 / ((double)nFrequency.QuadPart * 0.000001);
        }
        else
        {
            m_dMicrosecondsPerTick =
                1.0 / ((double)nFrequency.QuadPart * 0.000001);
            m_nFrequencyMHz = nFrequency.QuadPart / 1000000;
        }
    }
}
// PC 0x004015A0
CExoTimersInternal::~CExoTimersInternal()
{
}
// PC 0x005EC200
unsigned long CExoTimersInternal::GetLowResolutionTimer()
{
    return GetTickCount();
}
// PC 0x005EC210  Xbox 0x00139080
unsigned __int64 CExoTimersInternal::GetHighResolutionTimer()
{
    if (m_bHighResolution == 1)
    {
        LARGE_INTEGER nCounter;
        QueryPerformanceCounter(&nCounter);
        if (m_bLowFrequency == 1)
        {
            return (unsigned __int64)((double)nCounter.QuadPart *
                                      m_dMicrosecondsPerTick);
        }
        else
        {
            return (unsigned __int64)((double)nCounter.QuadPart *
                                      m_dMicrosecondsPerTick);
        }
    }
    return (unsigned __int64)GetTickCount() * 1000;
}
