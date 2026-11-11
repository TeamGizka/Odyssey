#include <windows.h>
#include "exobase.h"
#include "appmanager.h"
#include "serverexoapp.h"
#include "nwreentrantserver.h"
#include "swmain.h"

const int LARGE_TEMP_BUFFER_SIZE = 0x40000;

CSWReentrantServerStats::~CSWReentrantServerStats()
{
}

void CAppManager::FreeLargeTempBuffer()
{
    if (m_pLargeTempBuffer)
    {
        delete m_pLargeTempBuffer;
        m_pLargeTempBuffer = NULL;
    }
}

CObjectTableManager* CAppManager::GetObjectTableManager(unsigned long nTable)
{
    if (nTable == 0)
    {
        return m_pObjectTableManager;
    }

    if (nTable == 1)
    {
        return m_pServerObjectTableManager;
    }

    return NULL;
}

void CAppManager::DoSaveGameScreenShot(const CExoString& sName)
{
}

void CAppManager::TakeScreenShot(const CExoString& sName)
{
}

void CAppManager::AllocLargeTempBuffer()
{
    if (m_pLargeTempBuffer == NULL)
    {
        m_pLargeTempBuffer = new char[LARGE_TEMP_BUFFER_SIZE];
    }
}

CSWReentrantServerStats::CSWReentrantServerStats()
{
    m_bOperationActive = 0;
    m_nOperation = 0;
    m_nProgressTotal = 0;
    m_nProgress = 0;
    m_bOperationStartPending = 0;
    m_bOperationEndPending = 0;
}

CAppManager::CAppManager()
{
    m_pClientExoApp = NULL;
    m_pServerExoApp = NULL;
    m_nAppId = GetTickCount();
    m_pReentrantServerStats = new CSWReentrantServerStats;
    m_pObjectTableManager = new CObjectTableManager;
    m_pServerObjectTableManager = new CObjectTableManager;
    m_pLargeTempBuffer = NULL;
    AllocLargeTempBuffer();
}

void CAppManager::DestroyServer()
{
    if (m_pServerExoApp)
    {
        m_pServerExoApp->Uninitialize();
        m_pServerExoApp->StopServices();
        delete m_pServerExoApp;
        m_pServerExoApp = NULL;
    }
}

CAppManager::~CAppManager()
{
    if (m_pServerExoApp)
    {
        m_pServerExoApp->Uninitialize();
        m_pServerExoApp->StopServices();
        delete m_pServerExoApp;
        m_pServerExoApp = NULL;
    }

    if (m_pObjectTableManager)
    {
        delete m_pObjectTableManager;
        m_pObjectTableManager = NULL;
    }

    if (m_pServerObjectTableManager)
    {
        delete m_pServerObjectTableManager;
        m_pServerObjectTableManager = NULL;
    }

    if (m_pReentrantServerStats)
    {
        delete m_pReentrantServerStats;
        m_pReentrantServerStats = NULL;
    }

    if (m_pLargeTempBuffer)
    {
        delete m_pLargeTempBuffer;
        m_pLargeTempBuffer = NULL;
    }
}

void CAppManager::CreateServer(int nServices)
{
    if (m_pServerExoApp)
    {
        DestroyServer();
    }

    m_pServerExoApp = new CServerExoApp;
    m_pServerExoApp->StartServices(nServices);
    m_pServerExoApp->Initialize();
}
