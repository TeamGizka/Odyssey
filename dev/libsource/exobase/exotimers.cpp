#include "exobase.h"
// PC 0x00634BE0 GOG 0x00634AD0 Xbox 0x00149890
CExoTimers::CExoTimers()
{
    m_pInternal = new CExoTimersInternal;
}
// PC 0x00634C50 GOG 0x00634B40 Xbox 0x001498C0
unsigned long CExoTimers::GetLowResolutionTimer()
{
    return m_pInternal->GetLowResolutionTimer();
}
// PC 0x00634C60 GOG 0x00634B50 Xbox 0x001498D0
unsigned __int64 CExoTimers::GetHighResolutionTimer()
{
    return m_pInternal->GetHighResolutionTimer();
}
// PC 0x00634C70 GOG 0x00634B60 Xbox 0x001498E0
CExoTimers::~CExoTimers()
{
    delete m_pInternal;
}
