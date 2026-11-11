#include "exobase.h"
// PC 0x00634BE0
CExoTimers::CExoTimers()
{
    m_pInternal = new CExoTimersInternal;
}
// PC 0x00634C50
unsigned long CExoTimers::GetLowResolutionTimer()
{
    return m_pInternal->GetLowResolutionTimer();
}
// PC 0x00634C60
unsigned __int64 CExoTimers::GetHighResolutionTimer()
{
    return m_pInternal->GetHighResolutionTimer();
}
// PC 0x00634C70
CExoTimers::~CExoTimers()
{
    delete m_pInternal;
}
