#ifndef __NETLAYER_H__
#define __NETLAYER_H__

#include "exobase.h"

class CNetLayer;
class CConnectionLib;

class CBaseExoApp
{
public:
    CBaseExoApp() {}

    virtual void PlayerListChange(unsigned long nPlayerId, int bAdded,
                                  int bGuaranteed) = 0;
    virtual int HandleMessage(unsigned long nPlayerId, unsigned char* pData,
                              unsigned long nSize, int bNonWindow) = 0;
    virtual float GetFPS() = 0;
    virtual int ContinueMessageProcessing() = 0;
    virtual CNetLayer* GetNetLayer() = 0;
    virtual void ShutDownToMainMenu() = 0;
    virtual CConnectionLib* GetConnectionLib() = 0;
    virtual CExoLocString* GetModuleDescription() = 0;
    virtual int GetApplicationId() = 0;
    virtual void SetApplicationIdsMatch(int bMatch) = 0;
};

class CNetLayerPlayerInfo
{
public:
    CNetLayerPlayerInfo();

    static const unsigned long MAX_BUFFER_LENGTH;

    int m_nPlayerId;
    CExoString m_sPlayerName;
    unsigned char m_aAddress[16];
    int m_bPlayer;
    int m_bGameMaster;
    int m_bAuthorized;
};

struct CNetLayerBNCSMessage
{
    unsigned char m_aMessageType[7];
    unsigned char m_nClientType;
    unsigned char m_nNameLength;
    char m_szPlayerName[1];
};

class CNetLayer
{
public:

    enum { LOCAL_SESSION_PORT = 10 };

    enum { DEFAULT_SERVER_PORT = 16 };

    static const unsigned long PLAYERID_ALL_CLIENTS;
    static const unsigned long PLAYERID_INVALIDID;
    static const unsigned long PLAYERID_SERVER;
    static const unsigned long PLAYERID_ALL_PLAYERS;
    static const unsigned long PLAYERID_ALL_GAMEMASTERS;
    static const unsigned long PLAYERID_ALL_SERVERADMINS;
    static const unsigned long APPLICATION_CLIENT;
    static const unsigned long APPLICATION_SERVER;
    static const unsigned char CONNECTTYPE_PLAYER;
    static const unsigned char CONNECTTYPE_GAMEMASTER;
    static const unsigned char CONNECTTYPE_SERVERADMIN;
    static const unsigned long INSTANCE_CLIENT;
    static const unsigned long INSTANCE_SERVER;
    static const unsigned long CONNECTION_PENDING;

    CNetLayer();
    ~CNetLayer();

    int SendMessageToPlayer(unsigned long nPlayerId, unsigned char* pData,
                            unsigned long nSize, unsigned long nFlags);
    int StartServerMode(const CExoString& sServerName, unsigned long nPort);
    int EndServerMode();
    int StartConnectToSession(unsigned long nPlayerId,
                              const CExoString& sSessionName,
                              const CExoString& sPlayerName,
                              unsigned long nPort, unsigned long nClientType);
    CNetLayerPlayerInfo* GetPlayerInfo(unsigned long nPlayerId);
    int HandleBNCSMessage(unsigned long nPlayerId, unsigned char* pData,
                          unsigned long nSize);
    int HandleBNCRMessage(unsigned long nPlayerId, unsigned char* pData,
                          unsigned long nSize);
    int Initialize(CBaseExoApp* pExoApp);
    int ShutDown();
    int UpdateStatusLoop(unsigned long nMessage);
    int NonWindowMessages(unsigned long nPlayerId, unsigned char* pData,
                          unsigned long nSize);
    void ProcessReceivedFrames(int bProcess);

    int UpdateStatus();
    void SetUpPlayBackConnection();

    unsigned long GetUDPRecievePort();
    int StartProtocol(unsigned long nProtocol, unsigned long nAdapter,
                      unsigned long nPort, int bServer);
    int EndProtocol(unsigned long nProtocol);
    void SetPlayerPassword(CExoString sPassword);
    void SetGameMasterPassword(CExoString sPassword);
    void SetServerAdminPassword(CExoString sPassword);
    void SetSessionMaxPlayers(unsigned long nMaxPlayers);
    unsigned long GetSessionMaxPlayers();
    void SetSessionName(CExoString sSessionName);
    CExoString GetSessionName();
    int DisconnectPlayer(unsigned long nPlayerId, unsigned long nStrRef,
                         int bSendMessage);
    CExoString GetPlayerAddress(unsigned long nPlayerId);

    char m_aBuffer[0x20000];
    unsigned long m_nReadPos;
    unsigned long m_nWritePos;
    CBaseExoApp* m_pExoApp;
    CNetLayer* m_pPeer;
    int m_bInitialized;
    CNetLayerPlayerInfo m_cPlayerInfo;
    int m_bServerMode;
    CExoString m_sServerName;
    CExoString m_sServerString1;
    CExoString m_sServerString2;
    CExoString m_sServerString3;
    int m_bConnecting;
    CExoString m_sSessionName;
    int m_nClientType;
    int m_bConnected;
    int m_nConnectState;
    int m_nReserved;
};

#endif
