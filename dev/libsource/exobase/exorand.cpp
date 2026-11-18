#include "exobase.h"
// PC 0x00635E10 GOG 0x00635D00 Xbox 0x0014A6A0
CExoRand::CExoRand()
{
    m_pInternal = new CExoRandInternal();
}
// PC 0x00635E80 GOG 0x00635D70 Xbox 0x0014A6D0
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
// PC 0x00635E90 GOG 0x00635D80 Xbox 0x0014A6E0
CExoRand::~CExoRand()
{
    if (m_pInternal != 0)
    {
        delete m_pInternal;
        m_pInternal = 0;
    }
}
