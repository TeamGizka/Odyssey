#include "nwsclient.h"
// PC 0x005CDA10  Xbox 0x00245590
CSWSClient::CSWSClient(unsigned long nPlayerId)
{
    m_nPlayerId = nPlayerId;
    m_nLanguage = 0;
}
// PC 0x005CDA30  Xbox 0x002455B0
CSWSClient::~CSWSClient()
{
}

