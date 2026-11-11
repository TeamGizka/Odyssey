#include "nwsclient.h"
// PC 0x006173B0
CSWSClient::CSWSClient(unsigned long nPlayerId)
{
    m_nPlayerId = nPlayerId;
    m_nLanguage = 0;
}
// PC 0x006173D0
CSWSClient::~CSWSClient()
{
}

