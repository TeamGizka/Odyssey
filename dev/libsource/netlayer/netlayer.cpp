#include <ctype.h>
#include <string.h>
#include "exobase.h"
#include "netlayer.h"

CNetLayer* g_pcTmpNetLayer;

const unsigned long CNetLayerPlayerInfo::MAX_BUFFER_LENGTH = 16;
const unsigned long CNetLayer::PLAYERID_ALL_CLIENTS = 0xffffffffUL;
const unsigned long CNetLayer::PLAYERID_INVALIDID = 0xfffffffeUL;
const unsigned long CNetLayer::PLAYERID_SERVER = 0xfffffffdUL;
const unsigned long CNetLayer::PLAYERID_ALL_PLAYERS = 0xfffffff7UL;
const unsigned long CNetLayer::PLAYERID_ALL_GAMEMASTERS = 0xfffffff6UL;
const unsigned long CNetLayer::PLAYERID_ALL_SERVERADMINS = 0xfffffff5UL;
const unsigned long CNetLayer::APPLICATION_CLIENT = (1UL << 22);
const unsigned long CNetLayer::APPLICATION_SERVER = (1UL << 23);
const unsigned char CNetLayer::CONNECTTYPE_PLAYER = 0x10;
const unsigned char CNetLayer::CONNECTTYPE_GAMEMASTER = 32;
const unsigned char CNetLayer::CONNECTTYPE_SERVERADMIN = 64;
const unsigned long CNetLayer::INSTANCE_CLIENT = 0;
const unsigned long CNetLayer::INSTANCE_SERVER = 1;
const unsigned long CNetLayer::CONNECTION_PENDING = 2;
// PC 0x006208D0
int CNetLayer::SendMessageToPlayer(unsigned long nPlayerId,
                                   unsigned char* pData, unsigned long nSize,
                                   unsigned long nFlags)
{
    unsigned long nNewWritePos;
    unsigned long nWritePos;
    CNetLayer* pPeer = m_pPeer;
    if (pPeer == 0)
    {
        return 0;
    }
    nWritePos = pPeer->m_nWritePos;
    if (nWritePos >= 0x10000)
    {
        nWritePos = 0;
    }
    nNewWritePos = nWritePos - (nSize & 3) + nSize + 8;
    if (nNewWritePos > pPeer->m_nReadPos + 0x10000)
    {
        CExoString sMessage;
        sMessage.Format(
            "CNetLayer::SendMessageToPlayer - Message Buffer Overflow! "
            "Message Header Follows:\nSize = %d! <%c %x, %c %x, %c %x, %c %x, "
            "%c %x, %c %x, %c %x, %c %x>\n",
            nSize, (nSize >= 1 && isgraph(pData[0])) ? pData[0] : '_',
            (nSize >= 1) ? pData[0] : 0,
            (nSize >= 2 && isgraph(pData[1])) ? pData[1] : '_',
            (nSize >= 2) ? pData[1] : 0,
            (nSize >= 3 && isgraph(pData[2])) ? pData[2] : '_',
            (nSize >= 3) ? pData[2] : 0,
            (nSize >= 4 && isgraph(pData[3])) ? pData[3] : '_',
            (nSize >= 4) ? pData[3] : 0,
            (nSize >= 5 && isgraph(pData[4])) ? pData[4] : '_',
            (nSize >= 5) ? pData[4] : 0,
            (nSize >= 6 && isgraph(pData[5])) ? pData[5] : '_',
            (nSize >= 6) ? pData[5] : 0,
            (nSize >= 7 && isgraph(pData[6])) ? pData[6] : '_',
            (nSize >= 7) ? pData[6] : 0,
            (nSize >= 8 && isgraph(pData[7])) ? pData[7] : '_',
            (nSize >= 8) ? pData[7] : 0);
        EXOXBOXASSERT(0);
    }
    memcpy(&m_pPeer->m_aBuffer[nWritePos], &nSize, 4);
    memcpy(&m_pPeer->m_aBuffer[nWritePos + 4], pData, nSize);
    m_pPeer->m_nWritePos = nNewWritePos;
    return 1;
}
// PC 0x00620BD0
int CNetLayer::StartServerMode(const CExoString& sServerName,
                               unsigned long nPort)
{
    if (m_bServerMode == 1)
    {
        return 0;
    }
    m_bServerMode = 1;
    m_sServerName = sServerName;
    m_sServerString1 = "";
    m_sServerString2 = "";
    m_sServerString3 = "";
    return 1;
}
// PC 0x00620C40
int CNetLayer::EndServerMode()
{
    if (m_bServerMode == 0)
    {
        return 0;
    }
    m_bServerMode = 0;
    return 1;
}
// PC 0x00620C60
int CNetLayer::StartConnectToSession(unsigned long nPlayerId,
                                     const CExoString& sSessionName,
                                     const CExoString& sPlayerName,
                                     unsigned long nPort,
                                     unsigned long nClientType)
{
    if (m_bConnecting == 1)
    {
        return 0;
    }
    m_bConnecting = 0;
    unsigned long nLength = sSessionName.GetLength() + 9;
    unsigned char* pMessage = new unsigned char[nLength];
    pMessage[0] = 'B';
    pMessage[1] = 'N';
    pMessage[2] = 'C';
    pMessage[3] = 'S';
    pMessage[4] = 'U';
    pMessage[5] = 0;
    pMessage[6] = 0;
    pMessage[7] = nClientType;
    pMessage[8] = sSessionName.GetLength();
    memcpy(pMessage + 9, sSessionName.CStr(), sSessionName.GetLength());
    SendMessageToPlayer(0, pMessage, nLength, 0);
    m_bConnecting = 1;
    m_sSessionName = sPlayerName;
    m_nClientType = nClientType;
    m_bConnected = 0;
    m_nConnectState = 0;
    m_nReserved = 0;
    delete[] pMessage;
    return 1;
}
// PC 0x006212A0
int CNetLayer::UpdateStatusLoop(unsigned long nMessage)
{
    if (nMessage == 0x400000 && m_bConnecting == 1 && m_bConnected == 1 &&
        m_nConnectState != 2)
    {
        UpdateStatus();
    }
    return 1;
}
// PC 0x00620D90
int CNetLayer::UpdateStatus()
{
    if (m_bConnecting == 0)
    {
        return 0;
    }
    m_bConnecting = 0;
    m_bConnected = 0;
    m_nConnectState = 0;
    m_nReserved = 0;
    return 1;
}
// PC 0x00620DC0
CNetLayerPlayerInfo* CNetLayer::GetPlayerInfo(unsigned long nPlayerId)
{
    return &m_cPlayerInfo;
}
// PC 0x00620DD0
int CNetLayer::HandleBNCSMessage(unsigned long nPlayerId, unsigned char* pData,
                                 unsigned long nSize)
{
    CExoString sPlayerName;
    CExoString sUnused;
    if (nSize < 9)
    {
        return 1;
    }
    int nNameLength = pData[8];
    int nClientType = pData[7];
    if (nSize != nNameLength + 9)
    {
        return 1;
    }
    CNetLayerBNCSMessage* pMessage = (CNetLayerBNCSMessage*)pData;
    sPlayerName = CExoString(pMessage->m_szPlayerName, nNameLength);
    unsigned char* pReply = new unsigned char[10];
    pReply[0] = 'B';
    pReply[1] = 'N';
    pReply[2] = 'C';
    pReply[3] = 'R';
    m_cPlayerInfo.m_sPlayerName = sPlayerName;
    m_cPlayerInfo.m_nPlayerId = 1;
    if (nClientType == CONNECTTYPE_SERVERADMIN)
    {
        if (m_sServerString3 == "")
        {
            m_cPlayerInfo.m_bGameMaster = 0;
            m_cPlayerInfo.m_bPlayer = 0;
            m_cPlayerInfo.m_bAuthorized = 1;
        }
    }
    else if (nClientType == CONNECTTYPE_GAMEMASTER)
    {
        if (m_sServerString2 == "")
        {
            m_cPlayerInfo.m_bAuthorized = 0;
            m_cPlayerInfo.m_bGameMaster = 1;
            m_cPlayerInfo.m_bPlayer = 0;
        }
    }
    else
    {
        if (m_sServerString1 == "")
        {
            m_cPlayerInfo.m_bGameMaster = 0;
            m_cPlayerInfo.m_bPlayer = 1;
            m_cPlayerInfo.m_bAuthorized = 1;
        }
    }
    pReply[4] = 'A';
    if ((nClientType == CONNECTTYPE_PLAYER && m_sServerString1 == "") ||
        (nClientType == CONNECTTYPE_GAMEMASTER && m_sServerString2 == "") ||
        (nClientType == CONNECTTYPE_SERVERADMIN && m_sServerString3 == ""))
    {
        pReply[5] = 'C';
        m_pExoApp->PlayerListChange(0, 1, 0);
    }
    else
    {
        pReply[5] = 'P';
    }
    pReply[6] = nClientType;
    pReply[7] = 'U';
    pReply[8] = 0;
    pReply[9] = 0;
    SendMessageToPlayer(0, pReply, 10, 0);
    delete[] pReply;
    return 1;
}
// PC 0x004015A0
void CNetLayer::SetUpPlayBackConnection()
{
}
// PC 0x006212E0
int CNetLayer::NonWindowMessages(unsigned long nPlayerId, unsigned char* pData,
                                 unsigned long nSize)
{
    if (pData[0] == 'B' && pData[1] == 'N')
    {
        if (m_bServerMode == 1 && pData[2] == 'C' && pData[3] == 'S' &&
            nSize >= 7)
        {
            HandleBNCSMessage(nPlayerId, pData, nSize);
        }
        if (m_bConnecting == 1 && pData[2] == 'C' && pData[3] == 'R')
        {
            HandleBNCRMessage(nPlayerId, pData, nSize);
        }
        return 0;
    }
    else
    {
        m_pExoApp->HandleMessage(nPlayerId, pData, nSize, 1);
        return 0;
    }
}
// PC 0x00621040
int CNetLayer::HandleBNCRMessage(unsigned long nPlayerId, unsigned char* pData,
                                 unsigned long nSize)
{
    if (nSize != 10)
    {
        return 0;
    }
    if (pData[4] != 'A')
    {
        m_nReserved = 0;
    }
    else if (pData[5] == 'C')
    {
        m_bConnected = 1;
        m_nConnectState = 1;
        m_nReserved = 1;
    }
    return 1;
}
// PC 0x00621090
CNetLayerPlayerInfo::CNetLayerPlayerInfo()
{
    m_nPlayerId = 0;
    m_sPlayerName = "";
    m_bPlayer = 0;
    m_bGameMaster = 0;
    m_bAuthorized = 0;
}
// PC 0x00621370
CNetLayer::~CNetLayer()
{
    if (m_bInitialized == 1)
    {
        ShutDown();
    }
}
// PC 0x006210F0
CNetLayer::CNetLayer()
{
    m_pPeer = 0;
    m_bInitialized = 0;
}
// PC 0x006211A0
int CNetLayer::Initialize(CBaseExoApp* pExoApp)
{
    if (g_pcTmpNetLayer == 0)
    {
        g_pcTmpNetLayer = this;
    }
    else
    {
        m_pPeer = g_pcTmpNetLayer;
        g_pcTmpNetLayer->m_pPeer = this;
    }
    m_nReadPos = 0;
    m_nWritePos = 0;
    if (m_bInitialized == 1)
    {
        return 0;
    }
    m_pExoApp = pExoApp;
    m_bInitialized = 1;
    m_bServerMode = 0;
    CNetLayerPlayerInfo* pPlayerInfo = &m_cPlayerInfo;
    pPlayerInfo->m_nPlayerId = 0;
    pPlayerInfo->m_sPlayerName = "";
    pPlayerInfo->m_bPlayer = 0;
    pPlayerInfo->m_bGameMaster = 0;
    pPlayerInfo->m_bAuthorized = 0;
    m_bConnecting = 0;
    m_nClientType = 0;
    m_bConnected = 0;
    m_nConnectState = 0;
    m_nReserved = 0;
    return 1;
}
// PC 0x00621250
int CNetLayer::ShutDown()
{
    if (m_bInitialized == 0)
    {
        return 0;
    }
    if (m_bServerMode == 1)
    {
        m_bServerMode = 0;
    }
    if (m_pPeer != 0)
    {
        m_pPeer->m_pPeer = 0;
    }
    if (g_pcTmpNetLayer == this)
    {
        g_pcTmpNetLayer = m_pPeer;
    }
    m_bInitialized = 0;
    return 1;
}
// PC 0x00621420
void CNetLayer::ProcessReceivedFrames(int bProcess)
{
    int nFramePos;
    unsigned long nSize;
    int nPos;
    int nNextPos;

    if (bProcess)
    {
        while (m_nReadPos != m_nWritePos)
        {
            if (m_nReadPos >= 0x10000)
            {
                m_nReadPos = 0;
            }
            nFramePos = m_nReadPos;
            memcpy(&nSize, &m_aBuffer[nFramePos], 4);
            nPos = nFramePos + 4;
            nNextPos = nPos - (nSize & 3) + nSize + 4;
            m_nReadPos = nPos;
            if (m_aBuffer[nPos] == 'B' && m_aBuffer[nPos + 1] == 'N')
            {
                NonWindowMessages(0, (unsigned char*)&m_aBuffer[nPos], nSize);
            }
            else
            {
                m_pExoApp->HandleMessage(0, (unsigned char*)&m_aBuffer[nPos],
                                         nSize, 0);
            }
            m_nReadPos = nNextPos;
        }
    }
}
