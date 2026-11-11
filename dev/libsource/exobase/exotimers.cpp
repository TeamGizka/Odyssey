#include "exobase.h"
// PC 0x005E91E0  Xbox 0x00134CC0
CExoTimers::CExoTimers()
{
    m_pInternal = new CExoTimersInternal;
}
// PC 0x005E9250  Xbox 0x00134D30
unsigned long CExoTimers::GetLowResolutionTimer()
{
    return m_pInternal->GetLowResolutionTimer();
}
// PC 0x005E9260  Xbox 0x00134D40
unsigned __int64 CExoTimers::GetHighResolutionTimer()
{
    return m_pInternal->GetHighResolutionTimer();
}
// PC 0x005E9270  Xbox 0x00134D50
CExoTimers::~CExoTimers()
{
    delete m_pInternal;
}
