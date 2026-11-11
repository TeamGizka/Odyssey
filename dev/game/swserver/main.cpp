#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <process.h>
#include "exobase.h"
#include "exoresman.h"
#include "appmanager.h"
#include "serverexoapp.h"
#include "serverinfo.h"
#include "netlayer.h"
#include "nwsclient.h"
#include "nwsplayer.h"
#include "nwmessage.h"
#include "swgui.h"
#include "aurora.h"
#include "aurinternal.h"
#include "aurrendergl.h"
#include "exoinput.h"
#include "swmain.h"
#include "nwrules.h"
#include "nwtlktable.h"
#include "nwscript.h"
#include "twodimarrays.h"
#include "swserver.h"

int g_bDo3DGUIs = 1;
char g_nClientServerMode = 0;

HWND g_hWnd;
int g_nScreenWidth = 800;
int g_nScreenHeight = 600;
int g_nDesiredFullScreenRefreshRate = 60;
int g_nReInitializing = 0;
float displayFPS = 0.0f;
CAppManager* g_pAppManager;
CSWRules* g_pRules;
CSWTlkTable* g_pSWTlkTable;
CScriptCompiler* g_pScriptCompiler;
CVirtualMachine* g_pVirtualMachine;
CAurInternalGL* g_pAurora;
CSWGuiManager* g_pGuiMan;
CAuroraInterface* g_pAuroraInterface;
CExoSound* g_pExoSound;
CExoBase* g_pExoBase;
CExoString g_sBuildNumber;
CExoString g_sVersionNumber;
CExoString g_sGameName;
CExoString g_sCopyrightInfo;
CExoInput* g_pExoInput;
CExoResMan* g_pExoResMan = NULL;
CTlkTable* g_pTlkTable = NULL;
__int64 g_nCurrentSessionStartFILETIME = 0;

HINSTANCE g_hInst;
int g_bKillFocusWorked = 1;
int g_bActivationDueToFullScreenMovie = 0;
int g_bActivationDueToAnyMovie = 0;
DWORD g_nWindowDeactivationTickCount = 0;
int g_bInReInitAurora = 0;
int g_fullScreenDesired = 0;
int g_bAllowWindowedMode = 0;
int g_nPriorityClass = 0;
int g_nResetPriority = 0;
DWORD g_nSleeptime = 1;
int g_bSwappingBuffers = 0;
int memoryused = 0;
unsigned char g_nBitsPerPixel = 32;
float g_ClientUpdateTime = 0.0f;
float g_ServerUpdateTime = 0.0f;

int g_bExitProgram;
int g_nMouseTrails;
int g_nScreenSaverEnabled;

HWND g_hRenderWnd;
int g_bRenderWindowActive;
int g_bHeadLightEnabled;

int g_nMovieResolutionDepth;
int g_bMovieResolutionChanged;
unsigned long g_nLastResolutionWidth;
unsigned long g_nLastResolutionHeight;
unsigned char g_nLastResolutionBitsPerPixel;
int g_bMovieResolutionLocked;
int g_bVideoModeSettingsDirty;

void HandleLoadError(unsigned long nError)
{
}

const char* DumpLogCounters(char* sBuffer)
{
    return "NOT ENABLED";
}

void SetWindowTitle()
{
}

void SetClearColor(char* sColor)
{
}

void UpdateScreen(float fElapsed, int bRunWorld, int bSkipRender)
{
}

void messagepump()
{
}

static CExoCriticalSection* g_pInputCriticalSection = NULL;
static char g_szInputLine[SWSERVER_INPUT_LINE_SIZE];
static int g_bInputLineReady = FALSE;
static int g_bInteractive = TRUE;
static int g_nInstanceNumber = 0;
static HANDLE g_hInputThread = NULL;

unsigned int __stdcall Input(void* pData)
{
    char szLine[SWSERVER_INPUT_LINE_SIZE];

    for (;;)
    {
        fflush(stdout);
        if (fgets(szLine, sizeof(szLine), stdin) == NULL)
        {
            break;
        }

        char* pEnd = strchr(szLine, '\n');
        if (pEnd != NULL)
        {
            *pEnd = 0;
        }

        g_pInputCriticalSection->EnterCriticalSection();
        strcpy(g_szInputLine, szLine);
        g_bInputLineReady = TRUE;
        g_pInputCriticalSection->LeaveCriticalSection();

        while (g_bInputLineReady)
        {
            Sleep(1);
        }
    }

    return 0;
}

int GetInstanceNumber()
{
    CExoArrayList<int> lstProcessIds;
    int nProcessId;
    int nInstance;
    int i;
    FILE* pFile = fopen(".swpid", "r");

    if (pFile != NULL)
    {
        while (fscanf(pFile, "%d", &nProcessId) == 1)
        {
            lstProcessIds.Add(nProcessId);
        }
        fclose(pFile);
    }

    for (nInstance = 0; nInstance < lstProcessIds.num; nInstance++)
    {
        HANDLE hProcess = OpenProcess(SYNCHRONIZE, FALSE, lstProcessIds[nInstance]);
        if (hProcess == NULL)
        {
            break;
        }
        CloseHandle(hProcess);
    }

    if (nInstance < lstProcessIds.num)
    {
        lstProcessIds[nInstance] = _getpid();
    }
    else
    {
        lstProcessIds.Add(_getpid());
    }

    pFile = fopen(".swpid", "w");
    if (pFile != NULL)
    {
        for (i = 0; i < lstProcessIds.num; i++)
        {
            fprintf(pFile, "%d\n", lstProcessIds[i]);
        }
        fclose(pFile);
    }

    return nInstance;
}

void ReleaseInstanceNumber()
{
    CExoArrayList<int> lstProcessIds;
    int nProcessId;
    int i;
    FILE* pFile = fopen(".swpid", "r");

    if (pFile == NULL)
    {
        return;
    }
    while (fscanf(pFile, "%d", &nProcessId) == 1)
    {
        lstProcessIds.Add(nProcessId);
    }
    fclose(pFile);

    for (i = 0; i < lstProcessIds.num; i++)
    {
        if (lstProcessIds[i] == _getpid())
        {
            lstProcessIds[i] = 0;
        }
    }

    pFile = fopen(".swpid", "w");
    if (pFile != NULL)
    {
        for (i = 0; i < lstProcessIds.num; i++)
        {
            fprintf(pFile, "%d\n", lstProcessIds[i]);
        }
        fclose(pFile);
    }
}

static int GetOptionParameter(int argc, int& i, const char* szOption)
{
    if (i + 1 >= argc)
    {
        printf("%s: %s %s\n", "Server", szOption, "requires one parameter");
        return FALSE;
    }

    ++i;
    return TRUE;
}

void ProcessCommandLine(int argc, char** argv, int& bRunModule)
{
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    CServerInfo* pServerInfo = pServer->GetServerInfo();
    char* szModule = NULL;
    int nLoadSlot = -1;
    int i;

    for (i = 1; i < argc; i++)
    {
        char* szOption = argv[i];

        if (strcmp(szOption, "-quiet") == 0)
        {
            g_bInteractive = FALSE;
            continue;
        }
        if (strcmp(szOption, "-interactive") == 0)
        {
            g_bInteractive = TRUE;
            continue;
        }
        if (szOption[0] != '-')
        {
            printf("%s: %s %s\n", "Server", szOption, "not understood");
            continue;
        }
        if (!GetOptionParameter(argc, i, szOption))
        {
            continue;
        }

        char* szValue = argv[i];

        if (strcmp(szOption, "-maxclients") == 0)
        {
            unsigned long nMaxPlayers = strtoul(szValue, NULL, 10);
            pServer->GetNetLayer()->SetSessionMaxPlayers(nMaxPlayers);
            pServerInfo->m_nMaxPlayers = nMaxPlayers;
        }
        else if (strcmp(szOption, "-port") == 0)
        {
            AdminSetPort(strtoul(szValue, NULL, 10));
        }
        else if (strcmp(szOption, "-servervault") == 0)
        {
            pServerInfo->m_PersistantWorldOptions.bVaultCharsOnly = strtol(szValue, NULL, 10);
        }
        else if (strcmp(szOption, "-module") == 0)
        {
            szModule = szValue;
        }
        else if (strcmp(szOption, "-load") == 0)
        {
            nLoadSlot = strtol(szValue, NULL, 10);
        }
        else if (strcmp(szOption, "-pvp") == 0)
        {
            pServerInfo->m_nPVP = strtoul(szValue, NULL, 10);
        }
        else if (strcmp(szOption, "-minlevel") == 0)
        {
            pServerInfo->m_nMinLevel = strtoul(szValue, NULL, 10);
        }
        else if (strcmp(szOption, "-maxlevel") == 0)
        {
            pServerInfo->m_nMaxLevel = strtoul(szValue, NULL, 10);
        }
        else if (strcmp(szOption, "-reloadwhenempty") == 0)
        {
            pServer->SetReloadModuleWhenEmpty(strtol(szValue, NULL, 10));
        }
        else if (strcmp(szOption, "-pauseandplay") == 0)
        {
            pServerInfo->m_bPauseAndPlay = strtol(szValue, NULL, 10);
        }
        else if (strcmp(szOption, "-servername") == 0)
        {
            pServer->GetNetLayer()->SetSessionName(CExoString(szValue));
        }
        else if (strcmp(szOption, "-autosaveinterval") == 0)
        {
            pServerInfo->m_nAutoSaveInterval = strtoul(szValue, NULL, 10);
            pServerInfo->m_bAutoSaveAtIntervals = pServerInfo->m_nAutoSaveInterval != 0;
        }
        else if (strcmp(szOption, "-playerpassword") == 0)
        {
            pServer->GetNetLayer()->SetPlayerPassword(CExoString(szValue));
        }
        else if (strcmp(szOption, "-dmpassword") == 0)
        {
            pServer->GetNetLayer()->SetGameMasterPassword(CExoString(szValue));
        }
        else if (strcmp(szOption, "-adminpassword") == 0)
        {
            pServer->GetNetLayer()->SetServerAdminPassword(CExoString(szValue));
        }
        else if (strcmp(szOption, "-ilr") == 0)
        {
            pServerInfo->m_bItemLevelRestrictions = strtol(szValue, NULL, 10);
        }
        else if (strcmp(szOption, "-oneparty") == 0)
        {
            pServerInfo->m_bFixedParties = strtol(szValue, NULL, 10);
        }
        else if (strcmp(szOption, "-difficulty") == 0)
        {
            unsigned long nDifficulty = strtoul(szValue, NULL, 10);
            if (nDifficulty >= 1 && nDifficulty <= 4)
            {
                pServerInfo->SetDifficultyLevel(nDifficulty);
            }
            else
            {
                printf("%s: %s %s\n", "Server", "Invalid argument to", szOption);
            }
        }
        else
        {

            printf("%s: %s %s\n", "Server", szOption, "not understood");
        }
    }

    if (szModule != NULL)
    {
        bRunModule = AdminLoadModule(szModule);
    }
    else if (nLoadSlot != -1)
    {
        bRunModule = AdminLoadGame(nLoadSlot);
    }
}

void ProcessKeyboardInput()
{
    char szLine[SWSERVER_INPUT_LINE_SIZE];
    char szCommand[SWSERVER_INPUT_LINE_SIZE];

    g_pInputCriticalSection->EnterCriticalSection();
    if (!g_bInputLineReady)
    {
        g_pInputCriticalSection->LeaveCriticalSection();
        return;
    }
    strcpy(szLine, g_szInputLine);
    g_bInputLineReady = FALSE;
    g_pInputCriticalSection->LeaveCriticalSection();

    szCommand[0] = 0;
    sscanf(szLine, "%s", szCommand);
    if (szCommand[0] == 0)
    {
        return;
    }

    char* szParameter = strchr(szLine, ' ');
    if (szParameter != NULL)
    {
        while (*szParameter == ' ')
        {
            szParameter++;
        }
        if (*szParameter == 0)
        {
            szParameter = NULL;
        }
    }

    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    CServerInfo* pServerInfo = pServer->GetServerInfo();
    CExoString sCommand(szCommand);
    CExoString sParameter(szParameter != NULL ? szParameter : "");
    int bNeedsParameter = FALSE;

    if (sCommand == "status")
    {
        AdminStatus();
    }
    else if (sCommand == "kick")
    {
        if (szParameter != NULL)
            AdminKick(sParameter);
        else
            bNeedsParameter = TRUE;
    }
    else if (sCommand == "banip" || sCommand == "bankey" || sCommand == "banname" ||
             sCommand == "unbanip" || sCommand == "unbankey" || sCommand == "unbanname")
    {
        if (szParameter != NULL)
        {
            if (sCommand == "banip")
                pServer->AddIPToBannedList(sParameter);
            else if (sCommand == "bankey")
                pServer->AddCDKeyToBannedList(sParameter);
            else if (sCommand == "banname")
                pServer->AddPlayerNameToBannedList(sParameter);
            else if (sCommand == "unbanip")
                pServer->RemoveIPFromBannedList(sParameter);
            else if (sCommand == "unbankey")
                pServer->RemoveCDKeyFromBannedList(sParameter);
            else
                pServer->RemovePlayerNameFromBannedList(sParameter);

            CExoString sBanList = pServer->GetBannedListString();
            printf("%s\n%s", "Ban List:", sBanList.CStr());
        }
        else
        {
            bNeedsParameter = TRUE;
        }
    }
    else if (sCommand == "listbans")
    {
        CExoString sBanList = pServer->GetBannedListString();
        printf("%s\n%s", "Ban List:", sBanList.CStr());
    }
    else if (sCommand == "save" || sCommand == "forcesave" || sCommand == "saveandexit")
    {
        char* szName = szParameter != NULL ? strchr(szParameter, ' ') : NULL;
        if (szName != NULL)
        {
            int nSlot = strtoul(szParameter, NULL, 10);
            AdminSaveGame(nSlot, CExoString(szName + 1), sCommand != "save");
            if (sCommand == "saveandexit")
            {
                g_bExitProgram = TRUE;
            }
        }
        else
        {
            printf("%s: %s %s\n", "Server", szCommand, "requires two parameters");
        }
    }
    else if (sCommand == "exit" || sCommand == "quit")
    {
        g_bExitProgram = TRUE;
    }
    else if (sCommand == "clientinfo")
    {
        if (szParameter != NULL)
            AdminClientInfo(sParameter);
        else
            bNeedsParameter = TRUE;
    }
    else if (sCommand == "module")
    {
        if (szParameter != NULL)
        {
            if (AdminLoadModule(szParameter))
            {
                pServer->RunModule();
            }
        }
        else
        {
            bNeedsParameter = TRUE;
        }
    }
    else if (sCommand == "load")
    {
        if (szParameter != NULL)
        {
            if (AdminLoadGame(strtol(szParameter, NULL, 10)))
            {
                pServer->RunModule();
            }
        }
        else
        {
            bNeedsParameter = TRUE;
        }
    }
    else if (sCommand == "say")
    {
        if (szParameter != NULL)
            AdminSay(sParameter);
        else
            bNeedsParameter = TRUE;
    }
    else if (sCommand == "playerpassword")
    {
        pServer->GetNetLayer()->SetPlayerPassword(sParameter);
    }
    else if (sCommand == "dmpassword")
    {
        pServer->GetNetLayer()->SetGameMasterPassword(sParameter);
    }
    else if (sCommand == "adminpassword")
    {
        pServer->GetNetLayer()->SetServerAdminPassword(sParameter);
    }
    else if (sCommand == "maxclients")
    {
        if (szParameter != NULL)
        {
            unsigned long nMaxPlayers = strtoul(szParameter, NULL, 10);
            if (nMaxPlayers != 0)
            {
                pServer->GetNetLayer()->SetSessionMaxPlayers(nMaxPlayers);
                pServerInfo->m_nMaxPlayers = nMaxPlayers;
            }
            else
            {
                printf("%s: %s %s\n", "Server", "Invalid argument to", szCommand);
            }
        }
        else
        {
            bNeedsParameter = TRUE;
        }
    }
    else if (sCommand == "minlevel")
    {
        if (szParameter != NULL)
            pServerInfo->m_nMinLevel = strtoul(szParameter, NULL, 10);
        else
            bNeedsParameter = TRUE;
    }
    else if (sCommand == "maxlevel")
    {
        if (szParameter != NULL)
            pServerInfo->m_nMaxLevel = strtoul(szParameter, NULL, 10);
        else
            bNeedsParameter = TRUE;
    }
    else if (sCommand == "pauseandplay")
    {
        if (szParameter != NULL)
            pServerInfo->m_bPauseAndPlay = strtol(szParameter, NULL, 10);
        else
            bNeedsParameter = TRUE;
    }
    else if (sCommand == "servername")
    {
        if (szParameter != NULL)
            pServer->GetNetLayer()->SetSessionName(sParameter);
        else
            bNeedsParameter = TRUE;
    }
    else if (sCommand == "autosaveinterval")
    {
        if (szParameter != NULL)
        {
            pServerInfo->m_nAutoSaveInterval = strtoul(szParameter, NULL, 10);
            pServerInfo->m_bAutoSaveAtIntervals = pServerInfo->m_nAutoSaveInterval != 0;
        }
        else
        {
            bNeedsParameter = TRUE;
        }
    }
    else if (sCommand == "ilr")
    {
        if (szParameter != NULL)
            pServerInfo->m_bItemLevelRestrictions = strtol(szParameter, NULL, 10);
        else
            bNeedsParameter = TRUE;
    }
    else if (sCommand == "oneparty")
    {
        if (szParameter != NULL)
            pServerInfo->m_bFixedParties = strtol(szParameter, NULL, 10);
        else
            bNeedsParameter = TRUE;
    }
    else if (sCommand == "export")
    {

        pServer->SetExportAllCharactersPending(TRUE);
    }
    else if (sCommand == "difficulty")
    {
        if (szParameter != NULL)
        {
            unsigned long nDifficulty = strtoul(szParameter, NULL, 10);
            if (nDifficulty >= 1 && nDifficulty <= 4)
                pServerInfo->SetDifficultyLevel(nDifficulty);
            else
                printf("%s: %s %s\n", "Server", "Invalid argument to", szCommand);
        }
        else
        {
            bNeedsParameter = TRUE;
        }
    }
    else if (sCommand == "help")
    {
        printf("status clientinfo kick listbans banip bankey banname unbanip unbankey unbanname\n"
               "save forcesave saveandexit module load say export exit quit\n"
               "maxclients minlevel maxlevel pauseandplay ilr oneparty difficulty autosaveinterval\n"
               "playerpassword dmpassword adminpassword servername help\n");
    }
    else
    {
        printf("%s: %s %s\n", "Server", szCommand, "not understood");
    }

    if (bNeedsParameter)
    {
        printf("%s: %s %s\n", "Server", szCommand, "requires one parameter");
    }
}

int AdminLoadModule(char* szModule)
{
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;

    if (pServer->GetLoadingModule())
    {
        printf("\n%s: %s\n", "Server", "Unable to load module");
        return FALSE;
    }

    printf("%s: %s \"%s\"", "Server", "Loading module", szModule);
    fflush(stdout);

    if (!pServer->LoadModule(CExoString(szModule), NULL))
    {
        printf("\n%s: %s\n", "Server", "Unable to load module");
        return FALSE;
    }

    do
    {
        printf(".");
        fflush(stdout);
        pServer->MainLoop();
    } while (pServer->GetLoadingModule());

    if (pServer->GetServerMode() == SWSERVER_MODE_IDLE)
    {
        printf("\n%s: %s\n", "Server", "Unable to load module");
        return FALSE;
    }

    printf("\n%s: %s\n", "Server", "Module loaded");
    return TRUE;
}

int AdminLoadGame(int nSlot)
{
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    CExoArrayList<CExoString> lstSaves;
    CExoString sSaveName;
    int i;

    if (pServer->GetLoadingModule())
    {
        printf("\n%s: %s\n", "Server", "Unable to load saved game");
        return FALSE;
    }

    printf("%s: %s %d", "Server", "Loading saved game", nSlot);
    fflush(stdout);

    g_pExoBase->GetDirectoryList(&lstSaves, CExoString("SAVES:"), 0xFFFF, TRUE, FALSE);
    for (i = 0; i < lstSaves.num; i++)
    {
        if (lstSaves[i].AsINT() == nSlot)
        {
            sSaveName = lstSaves[i];
            break;
        }
    }
    if (i == lstSaves.num)
    {
        printf("\n%s: %s %s\n", "Server", "Invalid argument to", "load");
        return FALSE;
    }

    if (!pServer->LoadGame(nSlot, sSaveName, CExoString(""), NULL))
    {
        printf("\n%s: %s\n", "Server", "Unable to load saved game");
        return FALSE;
    }

    do
    {
        printf(".");
        fflush(stdout);
        pServer->MainLoop();
    } while (pServer->GetLoadingModule());

    if (pServer->GetServerMode() == SWSERVER_MODE_IDLE)
    {
        printf("\n%s: %s\n", "Server", "Unable to load saved game");
        return FALSE;
    }

    printf("\n%s: %s\n", "Server", "Saved game loaded");
    return TRUE;
}

int AdminSaveGame(int nSlot, CExoString sName, int bForce)
{
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    CExoArrayList<CExoString> lstSaves;
    int i;

    if (sName.GetLength() == 0)
    {
        printf("%s: %s %s\n", "Server", "Invalid argument to", "save");
        return FALSE;
    }
    if (pServer->GetServerMode() != SWSERVER_MODE_RUNNING)
    {
        printf("%s: %s\n", "Server", "Failed to save");
        return FALSE;
    }

    g_pExoBase->GetDirectoryList(&lstSaves, CExoString("SAVES:"), 0xFFFF, TRUE, FALSE);
    for (i = 0; i < lstSaves.num; i++)
    {
        if (lstSaves[i].AsINT() == nSlot)
        {
            if (!bForce)
            {
                printf("%s: %s\n", "Server", "Specified slot is in use");
                return FALSE;
            }

            CExoString sOldSave;
            sOldSave.Format("SAVES:%s", lstSaves[i].CStr());
            g_pExoResMan->CleanDirectory(sOldSave, TRUE, TRUE);
            break;
        }
    }

    CExoString sModule = pServer->GetModuleName();
    printf("%s: %s \"%s\" (%s)", "Server", "Saving", sName.CStr(), sModule.CStr());
    fflush(stdout);

    if (!pServer->SaveGame(nSlot, sName, NULL, FALSE))
    {
        printf("\n%s: %s\n", "Server", "Failed to save");
        return FALSE;
    }

    printf(".");
    fflush(stdout);
    pServer->MainLoop();
    printf("\n%s: %s\n", "Server", "Save complete");
    return TRUE;
}

void AdminStatus()
{
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    CNetLayer* pNetLayer = pServer->GetNetLayer();
    CServerInfo* pServerInfo = pServer->GetServerInfo();
    CExoString sValue;

    sValue = pNetLayer->GetSessionName();
    printf("%s: %s\n", "Server Name", sValue.CStr());
    printf("%s: %ld\n", "Maximum Clients", pNetLayer->GetSessionMaxPlayers());
    printf("%s: %lu\n", "Server Port", pNetLayer->GetUDPRecievePort());
    sValue = pServer->GetModuleName();
    printf("%s: %s\n", "Module Name", sValue.CStr());

    printf("%s: ", "Module Status");
    switch (pServer->GetServerMode())
    {
    case SWSERVER_MODE_LOADING:
        printf("%s\n", "Loaded");
        break;
    case SWSERVER_MODE_RUNNING:
        printf("%s\n", "Running");
        break;
    case SWSERVER_MODE_IDLE:
        printf("%s\n", "Idle");
        break;
    default:
        printf("%s\n", "Unknown");
        break;
    }

    printf("%s: ", "PVP");
    switch (pServerInfo->m_nPVP)
    {
    case 0:
        printf("%s\n", "None");
        break;
    case 1:
        printf("%s\n", "Party");
        break;
    case 2:
        printf("%s\n", "Full");
        break;
    default:
        printf("%s\n", "Unknown");
        break;
    }

    printf("%s: %d\n", "Difficulty", pServerInfo->m_nDifficultyLevel);
    printf("%s: %d to %d\n", "Levels", pServerInfo->m_nMinLevel, pServerInfo->m_nMaxLevel);
    printf("%s: %d\n", "ILR", pServerInfo->m_bItemLevelRestrictions);

}

static CSWSClient* FindClient(const CExoString& sPlayer)
{
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    CNetLayer* pNetLayer = pServer->GetNetLayer();
    CExoLinkedList<CSWSClient>* pPlayerList = pServer->GetPlayerList();
    unsigned long nPlayerId = sPlayer.AsINT();
    int bById = nPlayerId != 0 || sPlayer == "0";

    if (pPlayerList == NULL)
    {
        return NULL;
    }

    CExoLinkedListNode* pPosition = pPlayerList->GetHeadPos();
    while (pPosition != NULL)
    {
        CSWSClient* pClient = pPlayerList->GetAtPos(pPosition);
        if (pClient != NULL)
        {
            CNetLayerPlayerInfo* pPlayerInfo = pNetLayer->GetPlayerInfo(pClient->GetPlayerId());
            if ((bById && pClient->GetPlayerId() == nPlayerId) ||
                (pPlayerInfo != NULL && pPlayerInfo->m_sPlayerName.CompareNoCase(sPlayer)))
            {
                return pClient;
            }
        }
        pPlayerList->GetNext(pPosition);
    }

    return NULL;
}

void AdminKick(CExoString sPlayer)
{
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    CSWSClient* pClient = FindClient(sPlayer);

    if (pClient == NULL)
    {
        printf("%s: %s \"%s\"\n", "Server", "Unable to find player", sPlayer.CStr());
        return;
    }

    pServer->GetNetLayer()->DisconnectPlayer(pClient->GetPlayerId(), SWSERVER_STRREF_BOOTED, TRUE);
}

void AdminClientInfo(CExoString sPlayer)
{
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    CNetLayer* pNetLayer = pServer->GetNetLayer();
    CSWSClient* pClient = FindClient(sPlayer);

    if (pClient == NULL)
    {
        printf("%s: %s \"%s\"\n", "Server", "Unable to find player", sPlayer.CStr());
        return;
    }

    CNetLayerPlayerInfo* pPlayerInfo = pNetLayer->GetPlayerInfo(pClient->GetPlayerId());
    CExoString sAddress = pNetLayer->GetPlayerAddress(pClient->GetPlayerId());

    printf("%s: %s\n", "Player Name", pPlayerInfo->m_sPlayerName.CStr());
    printf("%s: %s\n", "IP Address", sAddress.CStr());
    printf("%s: %ld\n", "ID", pClient->GetPlayerId());
}

void AdminSay(CExoString sMessage)
{
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;

    if (pServer->GetPlayerList() == NULL)
    {
        return;
    }

    pServer->GetSWSMessage()->SendServerToPlayerChatMessage(
        SWSERVER_CHAT_SERVER_SHOUT, SWSERVER_INVALID_OBJECT_ID, sMessage,
        CNetLayer::PLAYERID_ALL_CLIENTS, CExoString(""));
    printf("%s: %s \"%s\"\n", "Server", "Sent message to all clients", sMessage.CStr());
}

void SignalHandler(int nSignal)
{
    signal(nSignal, SignalHandler);
    g_bExitProgram = TRUE;
}

void InitGameApp()
{
    g_sGameName = "Star Wars: Knights of the Old Republic Server";
    g_sBuildNumber = "0";
    g_sVersionNumber = "1.03";
    g_sCopyrightInfo = "Copyright BioWare Corp 1998-2003";
}

void PrintGameInfo()
{
    printf("%s\n", g_sGameName.CStr());
    printf("Build:%s\n", g_sBuildNumber.CStr());
    printf("%s\n", g_sCopyrightInfo.CStr());
}

void AdminSetPort(unsigned long nPort)
{
    CNetLayer* pNetLayer = g_pAppManager->m_pServerExoApp->GetNetLayer();
    unsigned long nOldPort = pNetLayer->GetUDPRecievePort();

    pNetLayer->EndProtocol(SWSERVER_PROTOCOL_UDP);
    if (!pNetLayer->StartProtocol(SWSERVER_PROTOCOL_UDP, 0, nPort, TRUE))
    {
        printf("%s: %s %ld\n", "Server", "Unable to use port", nPort);
        if (!pNetLayer->StartProtocol(SWSERVER_PROTOCOL_UDP, 0, nOldPort, TRUE))
        {
            printf("%s: %s %ld\n", "Server", "Unable to restore port", nOldPort);
        }
        else
        {
            printf("%s: %s %ld\n", "Server", "Restored port", nOldPort);
        }
    }
}

static void SetInstanceAlias(const char* szAlias, int nInstance)
{
    if (nInstance == 0)
    {
        return;
    }

    CExoString sAlias(szAlias);
    CExoString sPath = g_pExoBase->m_pExoAliasList->GetAliasPath(sAlias, FALSE);
    CExoString sInstancePath;

    if (sPath.GetLength() == 0)
    {
        return;
    }

    sInstancePath.Format("%s.%d/", sPath.Left(sPath.GetLength() - 1).CStr(), nInstance);

    g_pExoBase->m_pExoAliasList->Add(sAlias, sInstancePath);
    CreateDirectoryA(sInstancePath.CStr(), NULL);
}

int main(int argc, char** argv)
{
    int bRunModule = FALSE;

    if (argc > 1 && strcmp(argv[1], "-help") == 0)
    {
        printf("%s\n",
               "swserver [-module name | -load slot] [-maxclients n] [-port n] [-servername name]\n"
               "         [-playerpassword p] [-dmpassword p] [-adminpassword p] [-pvp n]\n"
               "         [-difficulty n] [-minlevel n] [-maxlevel n] [-pauseandplay n] [-ilr n]\n"
               "         [-oneparty n] [-servervault n] [-autosaveinterval n]\n"
               "         [-reloadwhenempty n] [-quiet | -interactive]");
        exit(0);
    }

    signal(SIGINT, SignalHandler);
    signal(SIGTERM, SignalHandler);

    InitGameApp();
    PrintGameInfo();
    printf("\n%s: %s\n", "Server", "Loading...");
    fflush(stdout);

    g_hInst = GetModuleHandleA(NULL);
    CoInitialize(NULL);

    g_nClientServerMode = CLIENT_SERVER_MODE_SERVER;

    g_pAppManager = new CAppManager;
    g_pExoBase = new CExoBase;
    {
        CExoString sAliasFile("swKotor.ini");

        g_pExoBase->LoadAliases(sAliasFile);
    }
    g_pInputCriticalSection = new CExoCriticalSection;

    g_pAppManager->CreateServer(0);

    g_nInstanceNumber = GetInstanceNumber();
    SetInstanceAlias("TEMP", g_nInstanceNumber);
    SetInstanceAlias("CURRENTGAME", g_nInstanceNumber);
    SetInstanceAlias("LOGS", g_nInstanceNumber);

    printf("%s: %s\n\n", "Server", "Running...");
    fflush(stdout);

    ProcessCommandLine(argc, argv, bRunModule);

    if (g_pAppManager->m_pServerExoApp->GetNetLayer()->GetUDPRecievePort() == 0)
    {
        CExoIni cIni;
        CExoString sPort;

        printf("%s: %s\n", "Server",
               "Could not establish network communication, port may be in use");
        cIni.ReadIniEntry(sPort, CExoString("Server Options"), CExoString("Game Port"),
                          CExoString("./swKotor.ini"));
        AdminSetPort(sPort.AsINT());
    }

    if (g_bInteractive)
    {
        unsigned int nThreadId;

        g_hInputThread = (HANDLE)_beginthreadex(NULL, 0, Input, NULL, 0, &nThreadId);
    }

    if (bRunModule && g_pAppManager->m_pServerExoApp->GetServerMode() != SWSERVER_MODE_IDLE)
    {
        if (!g_pAppManager->m_pServerExoApp->RunModule())
        {
            printf("%s: %s\n", "Server", "Unable to run module");
        }
    }

    g_bExitProgram = FALSE;
    while (!g_bExitProgram)
    {
        unsigned __int64 nFrameTimerStart = g_pExoBase->m_pExoTimers->GetHighResolutionTimer();

        if (g_pAppManager->m_pServerExoApp->MainLoop())
        {
            g_bExitProgram = TRUE;
        }

        unsigned __int64 nFrameTimerEnd = g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
        g_ServerUpdateTime = (float)(__int64)(nFrameTimerEnd - nFrameTimerStart) * 0.001f;

        Sleep(g_nSleeptime);
        ProcessKeyboardInput();
    }

    if (g_hInputThread != NULL)
    {
        TerminateThread(g_hInputThread, 0);
        CloseHandle(g_hInputThread);
    }

    printf("%s: %s\n", "Server", "Exiting...");
    fflush(stdout);

    g_pAppManager->DestroyServer();
    delete g_pAppManager;
    g_pAppManager = NULL;
    delete g_pInputCriticalSection;
    delete g_pExoBase;
    g_pExoBase = NULL;

    ReleaseInstanceNumber();
    CoUninitialize();
    return 0;
}
