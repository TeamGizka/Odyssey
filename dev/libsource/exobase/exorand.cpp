#include "exobase.h"
// PC 0x005EA420  Xbox 0x00136200
CExoRand::CExoRand()
{
    m_pInternal = new CExoRandInternal();
}
// PC 0x005EA490  Xbox 0x00136270
unsigned long CExoRand::Rand()
{
    return m_pInternal->Rand();
}
// Dead code: not in the PC executable
CExoString CExoRand::GetString(unsigned short nLength)
{
    return m_pInternal->GetString(nLength);
}
// Dead code: not in the PC executable
void CExoRand::SignalEvent()
{
    m_pInternal->SignalEvent();
}
// Dead code: not in the PC executable
void CExoRand::SignalEvent(unsigned long nEvent)
{
    m_pInternal->SignalEvent(nEvent);
}
// PC 0x005EA4A0  Xbox 0x00136280
CExoRand::~CExoRand()
{
    if (m_pInternal != 0)
    {
        delete m_pInternal;
        m_pInternal = 0;
    }
}
