#include "exobase.h"
#include "netlayer.h"

static unsigned long s_nUDPReceivePort = CNetLayer::DEFAULT_SERVER_PORT;
static unsigned long s_nSessionMaxPlayers = 16;

unsigned long CNetLayer::GetUDPRecievePort()
{
    return s_nUDPReceivePort;
}

int CNetLayer::StartProtocol(unsigned long nProtocol, unsigned long nAdapter,
                             unsigned long nPort, int bServer)
{
    if (nPort == 0)
    {
        return 0;
    }

    s_nUDPReceivePort = nPort;
    return 1;
}

int CNetLayer::EndProtocol(unsigned long nProtocol)
{
    s_nUDPReceivePort = 0;
    return 1;
}

void CNetLayer::SetPlayerPassword(CExoString sPassword)
{
    m_sServerString1 = sPassword;
}

void CNetLayer::SetGameMasterPassword(CExoString sPassword)
{
    m_sServerString2 = sPassword;
}

void CNetLayer::SetServerAdminPassword(CExoString sPassword)
{
    m_sServerString3 = sPassword;
}

void CNetLayer::SetSessionMaxPlayers(unsigned long nMaxPlayers)
{
    s_nSessionMaxPlayers = nMaxPlayers;
}

unsigned long CNetLayer::GetSessionMaxPlayers()
{
    return s_nSessionMaxPlayers;
}

void CNetLayer::SetSessionName(CExoString sSessionName)
{
    m_sServerName = sSessionName;
}

CExoString CNetLayer::GetSessionName()
{
    return m_sServerName;
}

int CNetLayer::DisconnectPlayer(unsigned long nPlayerId, unsigned long nStrRef,
                                int bSendMessage)
{
    if (m_cPlayerInfo.m_nPlayerId != (int)nPlayerId || m_pExoApp == NULL)
    {
        return 0;
    }

    m_pExoApp->PlayerListChange(nPlayerId, 0, 0);
    m_cPlayerInfo.m_sPlayerName = "";
    return 1;
}

CExoString CNetLayer::GetPlayerAddress(unsigned long nPlayerId)
{
    return CExoString("127.0.0.1");
}
