#if defined(_WIN32) && !defined(_XBOX)
#include <windows.h>
#else
#include "platform.h"
#endif
#include "exobase.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "clientexoappinternal.h"
#include "serverexoapp.h"
#include "nwreentrantserver.h"
#include "appmanager.h"
#include "swmain.h"

#include "aurtexman.h"
#include "aurinternal.h"
#include <stdio.h>
void SetWindowTitle();
extern int g_nReInitializing;

#include <time.h>

const int LARGE_TEMP_BUFFER_SIZE = 0x40000;

void FreeAppObjectTables(CAppManager* pManager)
{
    unsigned long nTable;
    CObjectTableManager* pTables;

    pTables = pManager->m_pObjectTableManager;
    for (nTable = 0; nTable < pTables->m_nTables; nTable++)
    {
        CObjectTable* pTable = pTables->m_apTables[nTable];
        if (pTable)
        {
            delete [] pTable->m_pTable;
            delete pTable;
        }
    }
    pTables = pManager->m_pServerObjectTableManager;
    for (nTable = 0; nTable < pTables->m_nTables; nTable++)
    {
        CObjectTable* pTable = pTables->m_apTables[nTable];
        if (pTable)
        {
            delete [] pTable->m_pTable;
            delete pTable;
        }
    }
    delete pManager->m_pLargeTempBuffer;
    pManager->m_pLargeTempBuffer = NULL;
}
// PC 0x00401000  Xbox 0x00011000
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
// PC 0x00401060  Xbox 0x00011060
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

    EXOXBOXASSERT(0);
    return NULL;
}
#ifndef _XBOX
void CAppManager::DoSaveGameScreenShot(const CExoString& sName)  // PC 0x00401080  Xbox 0x00011090
{
    AurSaveGameSnapshot((char*)sName.CStr(), 256, true, true);
}

void CAppManager::TakeScreenShot(const CExoString& sName)
{
    AurSaveGameSnapshot((char*)sName.CStr(), (short)m_pClientExoApp->m_pcExoAppInternal->m_nScreenShot, false, true);
}
#endif
void CAppManager::AllocLargeTempBuffer()  // PC 0x004010A0  Xbox 0x00011250
{
    if (m_pLargeTempBuffer == NULL)
    {
        m_pLargeTempBuffer = new char[LARGE_TEMP_BUFFER_SIZE];
    }
}

struct CAppManagerFileEntry
{
    unsigned long nTime;
    CExoString sName;
};

void FreeFileNames(CExoString* pNames)
{
    delete [] pNames;
}

static unsigned long s_nFilesWritten;
static unsigned long s_nBytesWritten;
static unsigned long s_nLastWriteTime;

int WriteFileToStream(FILE* pStream, CAppManagerFileEntry& entry)
{
    FILE* pFile = fopen(entry.sName.CStr(), "rb");
    if (pFile == NULL)
    {
        return 0;
    }

    if (g_pAppManager->m_pLargeTempBuffer == NULL)
    {
        fclose(pFile);
        return 0;
    }

    char szName[32];
    strncpy(szName, entry.sName.CStr(), sizeof(szName));
    szName[sizeof(szName) - 1] = 0;
    fwrite(szName, sizeof(szName), 1, pStream);

    unsigned long nSize = fread(g_pAppManager->m_pLargeTempBuffer, 1, LARGE_TEMP_BUFFER_SIZE, pFile);
    while (nSize != 0)
    {
        fwrite(&nSize, sizeof(nSize), 1, pStream);
        fwrite(g_pAppManager->m_pLargeTempBuffer, 1, nSize, pStream);
        s_nBytesWritten += nSize;
        nSize = fread(g_pAppManager->m_pLargeTempBuffer, 1, LARGE_TEMP_BUFFER_SIZE, pFile);
    }
    fwrite(&nSize, sizeof(nSize), 1, pStream);
    fclose(pFile);

    entry.nTime = (unsigned long)time(NULL);
    s_nLastWriteTime = entry.nTime;
    s_nFilesWritten++;
    fwrite(&entry.nTime, sizeof(entry.nTime), 1, pStream);
    return 1;
}
// PC 0x004010C0  Xbox 0x00011270
CSWReentrantServerStats::CSWReentrantServerStats()
{
    m_bOperationActive = 0;
    m_nOperation = 0;
    m_nProgressTotal = 0;
    m_nProgress = 0;
    m_bOperationStartPending = 0;
    m_bOperationEndPending = 0;
}

static void DestroyAppObjects(CAppManager* pManager)
{
    delete pManager->m_pClientExoApp;
    pManager->m_pClientExoApp = NULL;
    delete pManager->m_pObjectTableManager;
    pManager->m_pObjectTableManager = NULL;
    delete pManager->m_pServerExoApp;
    pManager->m_pServerExoApp = NULL;
    delete pManager->m_pReentrantServerStats;
    pManager->m_pReentrantServerStats = NULL;
}

static Vector* AllocateVectorArray(int nCount)
{
    return new Vector[nCount];
}
// PC 0x00401160  Xbox 0x00011310
CAppManager::CAppManager()
{
    m_pClientExoApp = NULL;
    m_pServerExoApp = NULL;
    m_nAppId = GetTickCount();
    m_pClientExoApp = new CClientExoApp;
    m_pReentrantServerStats = new CSWReentrantServerStats;
    m_pObjectTableManager = new CObjectTableManager;
    m_pServerObjectTableManager = new CObjectTableManager;
    m_pLargeTempBuffer = NULL;
    AllocLargeTempBuffer();
}
// PC 0x00401280  Xbox 0x00011430
void CAppManager::DestroyServer()
{
    if (m_pServerExoApp)
    {
        m_pServerExoApp->Uninitialize();
        m_pServerExoApp->StopServices();
        delete m_pServerExoApp;
        m_pServerExoApp = NULL;
    }

    SetWindowTitle();
}
// PC 0x004012C0  Xbox 0x00011470
CAppManager::~CAppManager()
{
    if (m_pClientExoApp)
    {
        delete m_pClientExoApp;
        m_pClientExoApp = NULL;
    }

    if (m_pServerExoApp)
    {
        m_pServerExoApp->Uninitialize();
        m_pServerExoApp->StopServices();
        delete m_pServerExoApp;
        m_pServerExoApp = NULL;
    }

    SetWindowTitle();

    if (m_pObjectTableManager)
    {
        delete m_pObjectTableManager;
#ifndef _XBOX
        m_pObjectTableManager = NULL;
#endif
    }

    if (m_pServerObjectTableManager)
    {
        delete m_pServerObjectTableManager;
#ifndef _XBOX
        m_pServerObjectTableManager = NULL;
#endif
    }

    if (m_pReentrantServerStats)
    {
        delete m_pReentrantServerStats;
#ifndef _XBOX
        m_pReentrantServerStats = NULL;
#endif
    }

    if (m_pLargeTempBuffer)
    {
        delete m_pLargeTempBuffer;
        m_pLargeTempBuffer = NULL;
    }
}
// PC 0x00401380  Xbox 0x00011530
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

#ifdef _XBOX
// Xbox 0x000115E0
const char* SetResolution(char* sCommand)
{
    static char sBuffer[80];
    int nWidth = 0;
    int nHeight = 0;

    sscanf(sCommand, "%d%d", &nWidth, &nHeight);
    if (nWidth == 0 || nHeight == 0)
    {
        sprintf(sBuffer,
                "Screen resolution is %d x %d.  Usage:  SetResolution nWidth nHeight",
                g_nScreenWidth, g_nScreenHeight);
        return sBuffer;
    }

    g_nScreenWidth = nWidth;
    g_nScreenHeight = nHeight;
    if (g_pAurora->FullScreen())
    {
        unsigned long nRefreshRate = g_nDesiredFullScreenRefreshRate;
        g_nReInitializing = 2;
        g_pAurora->GetValidMode((unsigned long*)&nWidth, (unsigned long*)&nHeight,
                                &g_nBitsPerPixel, &nRefreshRate, true);
        sprintf(sBuffer, "Setting %d x %d, %d bpp, %d refresh...", nWidth, nHeight,
                g_nBitsPerPixel, nRefreshRate);
        return sBuffer;
    }

    g_nReInitializing = 1;
    sprintf(sBuffer, "Setting %d x %d windowed...", nWidth, nHeight);
    return sBuffer;
}
#else
const char* SetResolution(char* sBuffer)  // Xbox 0x000115E0
{
    int nWidth = 0;
    int nHeight = 0;

    sscanf(sBuffer, "%d%d", &nWidth, &nHeight);
    if (nWidth == 0 || nHeight == 0)
    {
        sprintf(sBuffer,
                "Screen resolution is %d x %d.  Usage:  SetResolution nWidth nHeight",
                g_nScreenWidth, g_nScreenHeight);
        return sBuffer;
    }

    g_nScreenWidth = nWidth;
    g_nScreenHeight = nHeight;
    if (g_pAurora->FullScreen())
    {
        unsigned long nRefreshRate = g_nDesiredFullScreenRefreshRate;
        g_nReInitializing = 2;
        g_pAurora->GetValidMode((unsigned long*)&nWidth, (unsigned long*)&nHeight,
                                &g_nBitsPerPixel, &nRefreshRate, true);
        sprintf(sBuffer, "Setting %d x %d, %d bpp, %d refresh...", nWidth, nHeight,
                g_nBitsPerPixel, nRefreshRate);
        return sBuffer;
    }

    g_nReInitializing = 1;
    sprintf(sBuffer, "Setting %d x %d windowed...", nWidth, nHeight);
    return sBuffer;
}
#endif

#ifdef _XBOX
// Xbox 0x000110B0
unsigned char CAppManager::GetCodeWord()
{
    CExoIni cIni;
    CExoString sValue;
    unsigned char nCodeWord = 0;
    int bFound;

    {
        CExoString sKey("CODEWORD");
        CExoString sSection("Game Options");
        CExoString sFileName("d:\\swplayer.ini");

        bFound = cIni.ReadIniEntry(sValue, sFileName, sSection, sKey);
    }

    if (bFound)
    {
        if (sValue.CompareNoCase("rangersandhamsters"))
            nCodeWord = 1;
        else if (sValue.CompareNoCase("gnomelover"))
            nCodeWord = 2;
        else if (sValue.CompareNoCase("andiwantapony"))
            nCodeWord = 3;
        else if (sValue.CompareNoCase("chimpfactor5"))
            nCodeWord = 4;
        else if (sValue.CompareNoCase("notzesty"))
            nCodeWord = 5;
        else if (sValue.CompareNoCase("setinjello"))
            nCodeWord = 6;
        else
            nCodeWord = sValue.CompareNoCase("hacktastic") ? 7 : 0;
    }
    return nCodeWord;
}
#endif

#ifdef _XBOX
// PC 0x00401080  Xbox 0x00011090
void CAppManager::DoSaveGameScreenShot(const CExoString& sName)
{
    AurSaveGameSnapshot((char*)sName.CStr(), 256, false);
}

void CAppManager::TakeScreenShot(const CExoString& sName)
{
    AurSaveGameSnapshot((char*)sName.CStr(), (short)m_pClientExoApp->m_pcExoAppInternal->m_nScreenShot, false);
}
#endif
