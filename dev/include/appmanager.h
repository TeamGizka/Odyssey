#ifndef __APPMANAGER_H__
#define __APPMANAGER_H__

#include "exobase.h"

class CClientExoApp;
class CServerExoApp;
class CObjectTableManager;
class CSWReentrantServerStats;

class CAppManager
{
public:

    enum { OBJECT_TABLE_SERVER = 1 };

    CAppManager();
    ~CAppManager();

    CObjectTableManager* GetObjectTableManager(unsigned long nTable);
    void DoSaveGameScreenShot(const CExoString& sName);
    void AllocLargeTempBuffer();
    void FreeLargeTempBuffer();
    void DestroyServer();
    void CreateServer(int nServices);

    CServerExoApp* GetServerExoApp()
    {
        return m_pServerExoApp;
    }

    CClientExoApp* GetClientExoApp()
    {
        return m_pClientExoApp;
    }

    void* m_pLargeTempBuffer;
    CClientExoApp* m_pClientExoApp;
    CServerExoApp* m_pServerExoApp;
    CObjectTableManager* m_pObjectTableManager;
    CObjectTableManager* m_pServerObjectTableManager;
    CSWReentrantServerStats* m_pReentrantServerStats;
    unsigned long m_nAppId;
    int m_bServerWasPaused;
#ifdef _XBOX
    unsigned char GetCodeWord();
#endif
};

extern "C" CAppManager* g_pAppManager;


#include "exobase.h"



class CObjectTable
{
public:
    char* m_pTable;
};

class CObjectTableManager
{
public:
    CObjectTableManager();
    ~CObjectTableManager();

    void ClearAll();
    BOOL RemovePlayer(unsigned long nPlayerId);

    CObjectTable* m_apTables[96];
    unsigned long m_nTables;
};


#endif
