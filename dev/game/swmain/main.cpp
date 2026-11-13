#define _WIN32_WINNT 0x0400
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>
#include <GL/gl.h>
#include "exobase.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "serverexoapp.h"
#include "netlayer.h"
#include "swgui.h"
#include "exobase.h"
#include "aurora.h"
#include "aurconsole.h"
#include <aurlist.h>
#include "aurinternal.h"
#include "aurrendergl.h"
#include "clientoptions.h"
#include "clientexoappinternal.h"
#include "exoinput.h"
#include "worldtimer.h"
#include "exoimeui.h"
#include "nwreentrantserver.h"
#include "nwcanimbase.h"
#include "swmain.h"
#include "nwrules.h"
#include "nwtlktable.h"
#include "nwscript.h"
#include "nwccreature.h"
#ifdef _XBOX
#include "LaunchData.h"
#include "partytables.h"
#include "nwmessage.h"
#include "guiingame.h"
#endif

struct CConsoleState
{
    int m_bActive;
    int m_nCursorPos;
    int m_bSubmitted;

    char m_szLine[512];
};

extern UINT RestoreMouseTrailsState();
extern UINT RestoreScreenSaverState();
extern double g_fFpsLimitMs;
extern unsigned __int64 g_nServerFrameTimerStart;
extern unsigned __int64 g_nServerFrameTimerEnd;
extern List<char*> g_listHistory;
extern int g_nHistoryPosition;
extern int totalvertexpool;
extern int totalvertexpool4k;
extern int totalindexpool;
extern int totalindexpool4;
extern float g_SndUpdateTime;
extern float g_AIUpdateTime;
extern float g_GuiUpdateTime;
extern float g_AIModuleTime;
extern float g_AIObjectTime;
extern int usedtexturememory;
extern int usedcomptexmemory;
extern int usedlightmapmemory;
extern int usedcubemapmemory;
extern int usedbumpmapmemory;
extern int countpart;
extern int RyanStartTextureSize;
extern int RyanEndTextureSize;
extern int RyanLevelTextureSize;
extern int RyanLevelVertices;
extern int RyanLevelVertSize;
extern int RyanStartParts;
extern int RyanEndParts;
extern int RyanLevelTotalParts;
extern char* g_szProcessModuleMemName;
extern const char* spawn(char* sCommand);
extern int AurTextureMakeCurrent();

HWND g_hWnd = NULL;
HWND g_hRenderWnd = NULL;
HINSTANCE g_hInst;
char g_nClientServerMode = 0;
CExoBase* g_pExoBase = NULL;
CExoInput* g_pExoInput = NULL;
CExoResMan* g_pExoResMan = NULL;
CExoSound* g_pExoSound = NULL;
CAuroraInterface* g_pAuroraInterface = NULL;
CSWGuiManager* g_pGuiMan = NULL;
CAurInternalGL* g_pAurora = NULL;
CAppManager* g_pAppManager = NULL;
CVirtualMachine* g_pVirtualMachine = NULL;
CScriptCompiler* g_pScriptCompiler = NULL;
CTlkTable* g_pTlkTable = NULL;
CSWTlkTable* g_pSWTlkTable = NULL;
int g_bDemoMode = 0;

static union { int s_nInitCounterStep; };
CExoString g_sBuildNumber("427");
CExoString g_sInstallBuildNumber;
CExoString g_sVersionNumber;
CExoString g_sGameName;
CExoString g_sCopyrightInfo;
int g_bActivationDueToFullScreenMovie = 0;
int g_bActivationDueToAnyMovie = 0;
static int g_bLowDiskSpace = 0;
unsigned char g_nLiveContentCorrupt = 0;
__int64 g_nCurrentSessionStartFILETIME = 0;
CSWRules* g_pRules = NULL;
int g_nReInitializing = 0;
int g_bAllowWindowedMode = 0;
static int g_bAllowWindowedModeFunctionality = 0;
int g_bRenderWindowActive = 0;
int g_bKillFocusWorked = 1;
int g_bInReInitAurora = 0;
CConsoleState g_cConsole = {0};
DWORD g_nWindowDeactivationTickCount = 0;
int g_bHeadLightEnabled = 0;
int g_fullScreenDesired = 0;
#ifdef _XBOX
int g_nScreenWidth = 640;
int g_nScreenHeight = 480;
#else
int g_nScreenWidth = 800;
int g_nScreenHeight = 600;
#endif
int g_nDesiredFullScreenRefreshRate = 60;
unsigned char g_nBitsPerPixel = 32;

static int g_bNiceClient = 0;
int memoryused = 0;
static int maxmemoryused = 0;
float displayFPS = 0.0f;
static unsigned __int64 g_nFpsFrameTimer = 0;
static unsigned __int64 g_nFpsCheckTimer = 0;
static float g_frametotaltime = 0.0f;
float g_ClientUpdateTime = 0.0f;
float g_ServerUpdateTime = 0.0f;

float g_ServerAIUpdateTime = 0.0f;

int verts = 0;
int anims = 0;
int texts = 0;
int parts = 0;
int verts2 = 0;
int g_bSwappingBuffers = 0;
int g_bDo3DGUIs = 1;
DWORD g_nSleeptime = 1;
int g_nTType = 1;
int g_nPriorityClass = 0;
int g_nResetPriority = 0;
int g_nMouseTrails = 0;
int g_nScreenSaverEnabled = 0;
int nFrameSlow = 0;
int g_bConsoleKeyboardCaptured = 0;
int g_nMovieResolutionDepth = 0;
int g_bMovieResolutionChanged = 0;
unsigned long g_nLastResolutionWidth = 640;
unsigned long g_nLastResolutionHeight = 480;
unsigned char g_nLastResolutionBitsPerPixel = 32;
int g_bMovieResolutionLocked = 0;
int g_bVideoModeSettingsDirty = 1;
int g_bExitProgram;
static unsigned __int64 g_nStartTimer;
static unsigned __int64 g_nEndTimer;

int g_mdlMem;
int g_wavMem;

const int BYTES_PER_KB = 1024;

const int XBOX_TOTAL_MEMORY = 0x1b00000;
const int XBOX_LEVEL_MEMORY = 0xe80000;
const int XBOX_LEVEL_MEMORY_CAP = 0xd00000;

#define XBOX_TEXTURE_COMPRESSION 0.23f

#ifndef _XBOX
const char* Buffers(char* sBuffer)
{
    sprintf(sBuffer, "VB[%d][%d] IB[%d][%d]",
            totalvertexpool / BYTES_PER_KB, totalvertexpool4k / BYTES_PER_KB,
            totalindexpool / BYTES_PER_KB, totalindexpool4 / BYTES_PER_KB);
    return sBuffer;
}

const char* C_Stats(char* sBuffer)
{
    sprintf(sBuffer, "tot[%06.1f] snd[%06.1f] AI:[%06.1f] gui:[%06.1f]",
            g_ClientUpdateTime, g_SndUpdateTime, g_AIUpdateTime, g_GuiUpdateTime);
    return sBuffer;
}

const char* S_Stats(char* sBuffer)
{
    sprintf(sBuffer, "tot[%07.2f]", g_ServerUpdateTime);
    return sBuffer;
}

const char* AI_Stats(char* sBuffer)
{
    sprintf(sBuffer, "Mod[%05.2f] Obj[%05.2f]", g_AIModuleTime, g_AIObjectTime);
    return sBuffer;
}

const char* TexMem(char* sBuffer)
{
    int nUncompressed = usedtexturememory - usedcomptexmemory - usedlightmapmemory -
                        usedcubemapmemory - usedbumpmapmemory;

    sprintf(sBuffer, "(kB) Uncmp %d Cmp %d LM %d CM %d BM %d",
            nUncompressed / BYTES_PER_KB, usedcomptexmemory / BYTES_PER_KB,
            usedlightmapmemory / BYTES_PER_KB, usedcubemapmemory / BYTES_PER_KB,
            usedbumpmapmemory / BYTES_PER_KB);
    return sBuffer;
}

const char* GfxMemOld(char* sBuffer)
{
    sprintf(sBuffer, "(kB) Textures %d Verts %d Anims %d",
            usedtexturememory / BYTES_PER_KB, totalvertexpool / BYTES_PER_KB,
            animationfootprint / BYTES_PER_KB);
    return sBuffer;
}

const char* XbxMem(char* sBuffer)
{
    int nTotalMem = XBOX_TOTAL_MEMORY;
    if (usedcomptexmemory == 0)
    {
        int nTextures = usedtexturememory - usedlightmapmemory - usedcubemapmemory -
                        usedbumpmapmemory;
        nTotalMem -= (int)(nTextures * XBOX_TEXTURE_COMPRESSION);
        nTotalMem -= animationfootprint + totalvertexpool + usedlightmapmemory +
                     usedcubemapmemory + usedbumpmapmemory;
    }
    else
    {
        nTotalMem -= animationfootprint + totalvertexpool + usedtexturememory;
    }

    int nLevelMem = XBOX_LEVEL_MEMORY - totalvertexpool - usedtexturememory;
    if (nLevelMem > XBOX_LEVEL_MEMORY_CAP)
    {
        nLevelMem = XBOX_LEVEL_MEMORY_CAP;
    }

    sprintf(sBuffer, "Parts %d Total Mem %d Level Mem %d", countpart,
            nTotalMem / BYTES_PER_KB, nLevelMem / BYTES_PER_KB);
    return sBuffer;
}

const char* GuiMem(char* sBuffer)
{
    sprintf(sBuffer, "Tex %d Part %d", RyanStartTextureSize / BYTES_PER_KB,
            RyanStartParts);
    return sBuffer;
}

const char* LevMem(char* sBuffer)
{
    sprintf(sBuffer, "Tot %d Tex %d Part %d Vert %d-%d",
            (RyanLevelTextureSize + RyanLevelVertSize) / BYTES_PER_KB,
            RyanLevelTextureSize / BYTES_PER_KB, RyanLevelTotalParts,
            RyanLevelVertices, RyanLevelVertSize / BYTES_PER_KB);
    return sBuffer;
}

const char* DynMem(char* sBuffer)
{
    int nTextures = usedtexturememory - RyanEndTextureSize;
    int nVertSize = RyanTotalLevelVertSize - RyanLevelVertSize;

    sprintf(sBuffer, "Tot %d Tex %d Part %d Vert %d-%d",
            (nTextures + nVertSize) / BYTES_PER_KB, nTextures / BYTES_PER_KB,
            countpart - RyanEndParts, RyanTotalLevelVertices - RyanLevelVertices,
            nVertSize / BYTES_PER_KB);
    return sBuffer;
}

const char* GfxMem(char* sBuffer)
{
    int nResources;
    int nKeys;
    int nServicedSize;
    int nImageSize;
    int nModelDataSize;
    int nTextureSize;

    g_pExoResMan->GetResourcesInMemory(&nResources, &nKeys, &nServicedSize,
                                       &nImageSize, &g_mdlMem, &nModelDataSize,
                                       &nTextureSize, &g_wavMem);
    sprintf(sBuffer, "tot: %d md:%d mx:%d tx:%d",
            (nTextureSize + g_mdlMem + nModelDataSize) / BYTES_PER_KB,
            g_mdlMem / BYTES_PER_KB, nModelDataSize / BYTES_PER_KB,
            nTextureSize / BYTES_PER_KB);
    return sBuffer;
}

const char* ResMem(char* sBuffer)
{
    int nResources;
    int nKeys;
    int nServicedSize;
    int nImageSize;
    int nModelDataSize;
    int nTextureSize;

    int nTotal = g_pExoResMan->GetResourcesInMemory(
        &nResources, &nKeys, &nServicedSize, &nImageSize, &g_mdlMem,
        &nModelDataSize, &nTextureSize, &g_wavMem);
    sprintf(sBuffer, "%d %d %d/%d %d", nTotal / BYTES_PER_KB,
            nServicedSize / BYTES_PER_KB, nResources, nKeys,
            nImageSize / BYTES_PER_KB);
    return sBuffer;
}

const char* ClearLogCounters(char* sBuffer)
{
    return "NOT ENABLED";
}
// PC 0x00401430
const char* DumpLogCounters(char* sBuffer)
{
    return "NOT ENABLED";
}

void LogLevelStats()
{
    FILE* pFile = fopen("D:\\Logs\\Levelstats.txt", "a");
    if (pFile == NULL)
    {
        return;
    }

    fprintf(pFile, "%s", g_szProcessModuleMemName);
    fprintf(pFile, "\t%d", RyanLevelTotalParts);
    fprintf(pFile, "\t%d", RyanLevelVertices);
    fprintf(pFile, "\t%d", RyanLevelVertSize / BYTES_PER_KB);
    fprintf(pFile, "\t%d", -1);
    fprintf(pFile, "\t%d", RyanLevelTextureSize / BYTES_PER_KB);
    fprintf(pFile, "\t%d", -1);
    fprintf(pFile, "\t%d", countpart - RyanEndParts);
    fprintf(pFile, "\t%d", (usedtexturememory - RyanEndTextureSize) / BYTES_PER_KB);
    fprintf(pFile, "\t%d", RyanTotalLevelVertices - RyanLevelVertices);
    fprintf(pFile, "\t%d", (RyanTotalLevelVertSize - RyanLevelVertSize) / BYTES_PER_KB);
    fprintf(pFile, "\t%d", maxmemoryused);
    fprintf(pFile, "\t%d", usedtexturememory);
    fprintf(pFile, "\n");
    fclose(pFile);
}

const char* tc(char* sBuffer)
{
    int nLevelVertices = RyanTotalLevelVertices;
    int nAnimations = animationfootprint;
    int nVertices = totalvertexpool;
    int nTextures = usedtexturememory;
    int nParts = countpart;

    spawn(sBuffer);
    AurTextureMakeCurrent();

#ifndef _XBOX
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    SwapBuffers(GLRender::m_hDC);
#endif

    verts = (totalvertexpool - nVertices) / BYTES_PER_KB;
    anims = (animationfootprint - nAnimations) / BYTES_PER_KB;
    texts = (usedtexturememory - nTextures) / BYTES_PER_KB;
    parts = countpart - nParts;
    verts2 = RyanTotalLevelVertices - nLevelVertices;

    sprintf(sBuffer, "Verts %d-%d Anims %d Text %d Parts %d", verts2, verts,
            anims, texts, parts);
    return sBuffer;
}
#endif
#ifdef _XBOX

void ExoXboxRestart()
{
    XLaunchNewImage("d:\\default.xbe", NULL);
}
#endif

void SaveCreature(char* sName)
{
    g_pAppManager->m_pClientExoApp->GetPlayerCreature()->SaveCharGenCreature();
}

#ifndef _XBOX
void SetWindowTitle()  // PC 0x00401440
{
    CExoString sTitle = g_sCopyrightInfo + " " + g_sGameName;
    CExoString sGameName = g_sGameName;
    SetWindowText(g_hWnd, sGameName.CStr());
}
#endif

#ifndef _XBOX
void messagepump()  // PC 0x00401510
{
    MSG msg;

    while (PeekMessageA(&msg, NULL, 0, 0, PM_NOREMOVE))
    {
        if (GetMessageA(&msg, NULL, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
        else
        {
            g_bExitProgram = 1;
        }
    }
}
#endif
// PC 0x00401590
LRESULT CALLBACK CinematicBackdropWndProc(HWND hWnd, UINT nMsg, WPARAM wParam,
                                          LPARAM lParam)
{
    return 0;
}
#ifndef _XBOX
void HandleLoadError(unsigned long nError)  // PC 0x004015A0
{
}
#endif

void ReportFatalError(const char* szError)
{
    char szMessage[512];

    sprintf(szMessage, "Fatal error: %s\n", szError);
    OutputDebugString(szMessage);
}

#ifndef _XBOX
int StoreMouseTrailsState()  // PC 0x004015B0
{
    int nMouseTrails = 0;
    int bResult =
        SystemParametersInfoA(SPI_GETMOUSETRAILS, 0, &nMouseTrails, 0);

    if (bResult != 0)
    {
        bResult = nMouseTrails;
        if (nMouseTrails >= 1)
        {
            g_nMouseTrails = nMouseTrails;
            return SystemParametersInfoA(SPI_SETMOUSETRAILS, 0, NULL, 0);
        }
    }
    return bResult;
}
#endif

#ifndef _XBOX
UINT RestoreMouseTrailsState()  // PC 0x004015F0
{
    UINT nMouseTrails = g_nMouseTrails;

    if (g_nMouseTrails >= 1)
    {
        return SystemParametersInfoA(SPI_SETMOUSETRAILS, g_nMouseTrails, NULL,
                                     0);
    }
    return nMouseTrails;
}
#endif

#ifndef _XBOX
int StoreScreenSaverState()  // PC 0x00401610
{
    int bScreenSaverActive = 0;
    int bResult = SystemParametersInfoA(SPI_GETSCREENSAVEACTIVE, 0,
                                        &bScreenSaverActive, 0);

    if (bResult != 0)
    {
        bResult = bScreenSaverActive;
        if (bScreenSaverActive >= 1)
        {
            g_nScreenSaverEnabled = bScreenSaverActive;
            return SystemParametersInfoA(SPI_SETSCREENSAVEACTIVE, 0, NULL, 0);
        }
    }
    return bResult;
}
#endif

#ifndef _XBOX
UINT RestoreScreenSaverState()  // PC 0x00401650
{
    UINT bScreenSaverEnabled = g_nScreenSaverEnabled;

    if (g_nScreenSaverEnabled >= 1)
    {
        return SystemParametersInfoA(SPI_SETSCREENSAVEACTIVE,
                                     g_nScreenSaverEnabled, NULL, 0);
    }
    return bScreenSaverEnabled;
}
#endif
// PC 0x00401670
void SetBlendMode(int nSource, int nDestination)
{
    GLRender_SetBlendFunc(nSource, nDestination);
}
// PC 0x004016B0
void SetClearColor(char* sColor)
{
    GLclampf fRed = 0.0f;
    GLclampf fGreen = 0.0f;
    GLclampf fBlue = 0.0f;
    GLclampf fAlpha = 0.0f;

    sscanf(sColor, "%f %f %f %f", &fRed, &fGreen, &fBlue, &fAlpha);
    GLRender_SetClearColor(fRed, fGreen, fBlue, fAlpha);
}
// PC 0x00401720
void ProcessCommandLine(int argc, char** argv)
{

    const int DEFAULT_CAPTURE_EVENT_PLAYBACK_RATE = 30;

    for (int i = 1; i < argc; ++i)
    {
        if (strncmp(argv[i], "-ecf", 4) == 0)
        {
            ++i;
            CExoString sCaptureFile("capture");
            if (i < argc)
                sCaptureFile = argv[i];
            CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
            pClientExoApp->SetCaptureEvents(sCaptureFile);
        }
        else if (strncmp(argv[i], "-epf", 4) == 0)
        {
            ++i;
            CExoString sPlayBackFile("capture");
            if (i < argc)
                sPlayBackFile = argv[i];
            CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
            pClientExoApp->SetCaptureEventPlayBack(sPlayBackFile);
        }
        else if (strncmp(argv[i], "-epr", 4) == 0)
        {
            ++i;
            int nRate = DEFAULT_CAPTURE_EVENT_PLAYBACK_RATE;
            if (i < argc)
                nRate = atoi(argv[i]);
            CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
            pClientExoApp->SetCaptureEventPlayBack(nRate);
        }
        else if (strncmp(argv[i], "-epm", 4) == 0)
        {
        }
    }
}

#ifndef _XBOX
void DoConsoleHist(unsigned short nKey)  // PC 0x00401890
{
    if (nKey == VK_LEFT)
    {
        if (g_cConsole.m_bActive != 0)
        {
            g_cConsole.m_nCursorPos = (g_cConsole.m_nCursorPos - 1) &
                                  ((g_cConsole.m_nCursorPos - 1 <= 0) - 1);
        }
    }
    else if (nKey == VK_RIGHT && g_cConsole.m_bActive != 0)
    {
        ++g_cConsole.m_nCursorPos;
        if (g_cConsole.m_nCursorPos >= (unsigned int)strlen(g_cConsole.m_szLine))
        {
            g_cConsole.m_nCursorPos = strlen(g_cConsole.m_szLine);
        }
    }

    if (g_listHistory.m_nCount != 0)
    {
        if (nKey == VK_UP)
        {
            if (g_cConsole.m_bActive != 0)
            {
                g_nHistoryPosition = (g_nHistoryPosition - 1 < 0)
                                             ? 0
                                             : (g_nHistoryPosition - 1);
                strncpy(g_cConsole.m_szLine,
                        g_listHistory.m_pList[g_nHistoryPosition], 0x50);
                g_cConsole.m_nCursorPos = strlen(g_cConsole.m_szLine);
            }
        }
        else if (nKey == VK_RIGHT)
        {
            if (g_cConsole.m_bActive != 0)
            {
                if (strlen(g_cConsole.m_szLine) == 0)
                {
                    if (g_nHistoryPosition == g_listHistory.m_nCount)
                        --g_nHistoryPosition;
                    strncpy(g_cConsole.m_szLine,
                            g_listHistory.m_pList[g_nHistoryPosition], 0x50);
                    g_cConsole.m_nCursorPos = strlen(g_cConsole.m_szLine);
                }
            }
        }
        else if (nKey == VK_DOWN && g_cConsole.m_bActive != 0)
        {
            int nNext = g_nHistoryPosition + 1;

            if (g_listHistory.m_nCount < nNext)
                g_nHistoryPosition = g_listHistory.m_nCount;
            else
                g_nHistoryPosition = nNext;
            if (g_nHistoryPosition == g_listHistory.m_nCount)
            {
                g_cConsole.m_szLine[0] = 0;
                g_cConsole.m_nCursorPos = 0;
                return;
            }
            strncpy(g_cConsole.m_szLine,
                    g_listHistory.m_pList[g_nHistoryPosition], 0x50);
            g_cConsole.m_nCursorPos = strlen(g_cConsole.m_szLine);
        }
    }
}
#endif

inline DWORD ProcessorMask(unsigned int nProcessor)
{
    DWORD dwMask = 1;
    while (nProcessor--)
        dwMask *= 2;
    return dwMask;
}

#ifndef _XBOX
void SetProcessorAffinity(unsigned int nProcessor)
{
    if (nProcessor < 32)
    {
        OSVERSIONINFO osvi;
        osvi.dwOSVersionInfoSize = sizeof(osvi);
        if (GetVersionEx(&osvi) &&
            osvi.dwPlatformId == VER_PLATFORM_WIN32_NT &&
            osvi.dwMajorVersion >= 4)
        {
            DWORD dwProcessMask;
            DWORD dwSystemMask;
            if (GetProcessAffinityMask(GetCurrentProcess(), &dwProcessMask,
                                       &dwSystemMask))
            {
                DWORD dwMask = ProcessorMask(nProcessor);
                if (dwProcessMask & dwMask)
                    SetProcessAffinityMask(GetCurrentProcess(), dwMask);
            }
        }
    }
}
#endif

#ifndef _XBOX
DWORD WINAPI StartAddress(LPVOID pArg)  // PC 0x00401A50
{
    static int bClassRegistered;

    if (!bClassRegistered)
    {
        WNDCLASSA wndClass;

        wndClass.style = 0;
        wndClass.lpfnWndProc = CinematicBackdropWndProc;
        wndClass.cbClsExtra = 0;
        wndClass.cbWndExtra = 0;
        wndClass.hInstance = g_hInst;
        wndClass.hIcon = LoadIconA(g_hInst, MAKEINTRESOURCE(101));
        wndClass.hCursor = LoadCursorA(NULL, IDC_ARROW);
        wndClass.hbrBackground = NULL;
        wndClass.lpszMenuName = NULL;
        wndClass.lpszClassName = "KotorCin";
        if (!RegisterClassA(&wndClass))
        {
            FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                               FORMAT_MESSAGE_FROM_SYSTEM |
                               FORMAT_MESSAGE_IGNORE_INSERTS,
                           NULL, GetLastError(),
                           MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                           (LPSTR)&pArg, 0, NULL);
            MessageBoxA(NULL, (LPCSTR)pArg, "Error", MB_ICONINFORMATION);
            LocalFree(pArg);
            return 0;
        }
        bClassRegistered = 1;
    }

    WINDOWINFO windowInfo;

    windowInfo.cbSize = sizeof(WINDOWINFO);
    if (GetWindowInfo(g_hRenderWnd, &windowInfo))
    {
        HWND hWnd = CreateWindowExA(WS_EX_TOPMOST, "Knights Cinematic Backdrop",
                                    "Knights Cinematic Backdrop",
                                    WS_POPUP | WS_VISIBLE, 0, 0, 1600, 1200,
                                    NULL, NULL, g_hInst, NULL);

        SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 1600, 1200,
                     SWP_NOSIZE | SWP_FRAMECHANGED | SWP_NOCOPYBITS);

        HDC hDC = GetDC(hWnd);

        PatBlt(hDC, 0, 0, 1600, 1200, BLACKNESS);
        ReleaseDC(hWnd, hDC);
        ShowWindow(hWnd, SW_MAXIMIZE);

        HANDLE hEvent = (HANDLE)pArg;

        while (WaitForSingleObject(hEvent, 1) == WAIT_TIMEOUT)
        {
        }
        DestroyWindow(hWnd);
    }
    return 0;
}
#endif
// PC 0x00401BD0
void SetupOpenGL()
{
    GLRender_SetClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    GLRender_Enable(ENABLE_CULL_FACE);
    GLRender_Enable(ENABLE_DEPTH_TEST);
    GLRender_Enable(ENABLE_LIGHTING);
    GLRender_Enable(ENABLE_TEXTURE_2D);
    GLRender_Enable(ENABLE_BLEND);
    GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
    GLRender_Enable(ENABLE_ALPHA_TEST);
}

#ifdef _XBOX
extern void AurSetFrameBufferMode(int nMode);

void messagepump(int nUnused)
{
    UINT nResult;
    for (int i = 0; i < g_lstVisibilityTests.m_nCount; i++)
    {
        while (g_pD3DDevice->GetVisibilityTestResult(
                   g_lstVisibilityTests.m_pList[i], &nResult, NULL) ==
               D3DERR_TESTINCOMPLETE)
            ;
    }
    g_lstVisibilityTests.m_nCount = 0;
    g_nStreamBufferIndex = -1;
    g_pD3DDevice->SetStreamSource(0, NULL, 0);
    if ((float)(unsigned __int64)(AurGetHighResolutionTime() -
                                  g_nRenderStartTime) *
            0.001f <
        16.0f)
        g_pD3DDevice->BlockUntilVerticalBlank();
    g_pD3DDevice->Swap(0);
    g_nRenderStartTime = AurGetHighResolutionTime();
}

void UpdateScreen(float fElapsed, int bRunWorld, int bSkipRender)
{
    AurMakeCurrent();

    if (bSkipRender == 0)
    {
        GLRender::Clear(3);
        if (g_bHeadLightEnabled != 0)
            AurEnableHeadLight();
        else
            AurDisableHeadLight();
    }

    CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
    pClientExoApp->GetSWGuiManager()->Draw(fElapsed);

    if (bSkipRender == 0)
    {
        if (g_cConsole.m_bActive != 0)
        {
            AurPostString(">", 0, 0, 5.0f);
            AurPostString(g_cConsole.m_szLine, 2, 0, 5.0f);
        }
        GLRender::LoadViewIdentity();
        AurRenderStrings();
        messagepump(0);
    }

    if (bRunWorld == 1 && g_pAppManager != NULL)
    {
        CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
        if (pServerExoApp != NULL)
        {
            pServerExoApp->MainLoop();
            if (g_pAppManager->GetClientExoApp() != NULL)
                g_pAppManager->GetClientExoApp()->GetNetLayer()
                    ->ProcessReceivedFrames(0);
        }
        if (g_pAppManager->GetClientExoApp() != NULL)
            g_pAppManager->GetClientExoApp()->GetNetLayer()
                ->ProcessReceivedFrames(0);
    }

    if (g_pExoSound != NULL)
        g_pExoSound->Render();

    if (g_pAppManager != NULL)
    {
        if (g_pAppManager->GetClientExoApp() != NULL &&
            g_pAppManager->GetClientExoApp()->GetNetLayer() != NULL)
            g_pAppManager->GetClientExoApp()->GetNetLayer()->UpdateStatusLoop(
                CNetLayer::APPLICATION_CLIENT);
    }
}
#else
void UpdateScreen(float fElapsed, int bRunWorld, int bSkipRender)  // PC 0x00401C30
{
    AurMakeCurrent();

    if (bSkipRender == 0 &&
        g_pAppManager->m_pClientExoApp->IsMoviePlaying() == 0)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
                GL_STENCIL_BUFFER_BIT);
        if (g_bHeadLightEnabled != 0)
            AurEnableHeadLight();
        else
            AurDisableHeadLight();
    }

    CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
    pClientExoApp->GetSWGuiManager()->Draw(fElapsed);

    if (bSkipRender == 0 &&
        g_pAppManager->m_pClientExoApp->IsMoviePlaying() == 0)
    {
        if (g_cConsole.m_bActive != 0)
        {
            AurPostString(">", 0, 0, 5.0f);
            AurPostString(g_cConsole.m_szLine, 2, 0, 5.0f);
        }
        AurRenderStrings();
        HDC hDC = GetDC(g_hRenderWnd);
        SwapBuffers(hDC);
        ReleaseDC(g_hRenderWnd, hDC);
    }

    messagepump();

    if (bRunWorld == 1 && g_pAppManager != NULL)
    {
        CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
        if (pServerExoApp != NULL)
        {
            pServerExoApp->MainLoop();
            messagepump();
            if (g_pAppManager->m_pClientExoApp != NULL)
                g_pAppManager->m_pClientExoApp->GetNetLayer()
                    ->ProcessReceivedFrames(0);
        }
        if (g_pAppManager->m_pClientExoApp != NULL)
            g_pAppManager->m_pClientExoApp->GetNetLayer()
                ->ProcessReceivedFrames(0);
    }

    if (g_pExoSound != NULL)
        g_pExoSound->Render();

    if (g_pAppManager != NULL)
    {
        if (g_pAppManager->m_pClientExoApp != NULL &&
            g_pAppManager->m_pClientExoApp->GetNetLayer() != NULL)
            g_pAppManager->m_pClientExoApp->GetNetLayer()->UpdateStatusLoop(
                CNetLayer::APPLICATION_CLIENT);
    }

    CClientExoApp* pClientExoApp2 = g_pAppManager->m_pClientExoApp;
    pClientExoApp2->GetSWGuiManager()->Draw(0.0f);
}
#endif

#ifndef _XBOX
void DeactivateRenderWindow(int bMovie)  // PC 0x00401DB0
{
    if (g_pExoSound != NULL)
        g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_LOSTFOCUS, bMovie);
    if (g_bRenderWindowActive != 0)
    {
        if (g_pAppManager->m_pServerExoApp != NULL)
        {
            int nPauseState = g_pAppManager->m_pServerExoApp->GetPauseState(2);
            g_pAppManager->m_bServerWasPaused = nPauseState;
            if (nPauseState == 0)
                g_pAppManager->m_pServerExoApp->SetPauseState(2, 1);
        }
        g_pAppManager->m_pClientExoApp->SetInputActive(0, 0);
        g_bRenderWindowActive = 0;
    }
}
// PC 0x00401E30
int ActivateRenderWindow()
{
    if (g_bRenderWindowActive != 0)
        return g_bRenderWindowActive;

    g_bRenderWindowActive = 1;
    if (g_pAppManager->m_pServerExoApp != NULL &&
        g_pAppManager->m_bServerWasPaused == 0)
    {
        g_pAppManager->m_pServerExoApp->SetPauseState(2, 0);
    }
    g_pAppManager->m_pClientExoApp->SetInputActive(1, 0);
    if (g_pExoSound != NULL)
        g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_GAINFOCUS);
    CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
    return g_pAurora->ResetGammaRamp(
        pClientExoApp->GetClientOptions()->GetGammaSetting(), 0);
}
// PC 0x00401EC0
void NotifyModeChangeActivate(int bActive, int bMinimized)
{
    g_pAppManager->m_pClientExoApp->SetInputActive(1, 0);

    if (g_bAllowWindowedMode != 0)
    {
        if (g_pAurora != NULL)
        {
            if (bActive == 1)
                g_nReInitializing = (bMinimized == 1) + 1;
            else
                g_nReInitializing = g_pAurora->FullScreen() != 0 ? 1 : 2;
        }
        return;
    }

    SendMessageA(g_hRenderWnd, WM_ACTIVATEAPP, 0, 0);
}
#endif

#ifdef _XBOX
#define VK_ESCAPE 0x1B
#endif
// PC 0x00401F30
void DoConsole(char cChar)
{
    static int nOffset;
    List<char*> lstCompletions(0);

    if (cChar == VK_ESCAPE)
    {
        if (g_bConsoleKeyboardCaptured != 0)
            g_pExoInput->KeyboardModeOff();
        g_cConsole.m_bActive = 0;
        g_cConsole.m_szLine[0] = 0;
        g_cConsole.m_nCursorPos = 0;
        return;
    }
    else if (cChar == '\n' || cChar == '\r')
    {
        if (g_bConsoleKeyboardCaptured != 0)
            g_pExoInput->KeyboardModeOff();
        g_cConsole.m_bActive = 0;
        AurPostString("* ", 0, 0, 5.0f);
        AurPostString(g_cConsole.m_szLine, 2, 0, 5.0f);
        g_cConsole.m_bSubmitted = 1;
        return;
    }
    else if (cChar == '\b')
    {
        if (strlen(g_cConsole.m_szLine) != 0 && g_cConsole.m_nCursorPos != 0)
        {
            if (g_cConsole.m_nCursorPos >= (unsigned int)strlen(g_cConsole.m_szLine))
                g_cConsole.m_nCursorPos = strlen(g_cConsole.m_szLine);

            int nLength = (strlen(g_cConsole.m_szLine) > 500)
                              ? 500
                              : strlen(g_cConsole.m_szLine);

            g_cConsole.m_nCursorPos--;
            for (int i = g_cConsole.m_nCursorPos; i < nLength; i++)
                g_cConsole.m_szLine[i] = g_cConsole.m_szLine[i + 1];
        }
        return;
    }
    else if (cChar == '\t')
    {
        strcpy(g_cConsole.m_szLine,
               CommandCompletion(g_cConsole.m_szLine, lstCompletions));
        char* sPreview = FuncPreInterp(g_cConsole.m_szLine);

        AurPostString(sPreview, 0, 1, 1.0f);
        if (lstCompletions.m_nCount > 1)
        {
            float fDuration = 4.0f;
#ifdef _XBOX
            fDuration = (lstCompletions.m_nCount > 15) ? 4.0f : 1.0f; if (lstCompletions.m_nCount < 15) nOffset = 0;
#else
            if (lstCompletions.m_nCount <= 15) { fDuration = 1.0f;
                if (lstCompletions.m_nCount < 15) nOffset = 0; }
#endif

            for (int i = 0; i < lstCompletions.m_nCount; i++)
            {
                if (i >= 15)
                    break;
                AurPostString(
                    lstCompletions.m_pList[(nOffset + i) %
                                         lstCompletions.m_nCount],
                    4, i + 6, fDuration);
            }
            nOffset += 5;
        }
        g_cConsole.m_nCursorPos = strlen(g_cConsole.m_szLine);
        return;
    }
    else
    {
        int nLength = (strlen(g_cConsole.m_szLine) > 500)
                          ? 500
                          : strlen(g_cConsole.m_szLine);

        if (g_cConsole.m_nCursorPos >= (unsigned int)strlen(g_cConsole.m_szLine))
            g_cConsole.m_nCursorPos = strlen(g_cConsole.m_szLine);
        for (int i = nLength; i > g_cConsole.m_nCursorPos; i--)
            g_cConsole.m_szLine[i] = g_cConsole.m_szLine[i - 1];
        g_cConsole.m_szLine[g_cConsole.m_nCursorPos] = cChar;
        g_cConsole.m_nCursorPos++;
        g_cConsole.m_szLine[nLength + 1] = 0;
    }
}
#ifndef _XBOX
int HandleWMCharMessage(unsigned short nChar)  // PC 0x00402270
{
    unsigned short nCheatChar;

    if (g_pAppManager->m_pClientExoApp->GetCaptureEvents() != 0 &&
        g_pAppManager->m_pClientExoApp->GetCaptureEventFile() != NULL)
    {
        CExoString sEvent;
        CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;

        sEvent.Format("%d %c %d\n",
                      (unsigned int)(pClientExoApp->GetWorldTimer()->GetSnapshotTime() /
                                     1000),
                      'C', nChar);
        pClientExoApp = g_pAppManager->m_pClientExoApp;
        pClientExoApp->GetCaptureEventFile()->Write(sEvent);
    }

    switch (g_pAppManager->m_pClientExoApp->GetClientLanguage())
    {
    case 2:
        nCheatChar = 176;
        break;
    case 5:
        nCheatChar = 3;
        break;
    default:
        nCheatChar = 96;
        break;
    }

    if (nChar == nCheatChar)
    {
        if (nCheatChar == 3)
            g_bMouseCameraRotateToggle = 0;

        CExoIni cIni;
        CExoString sValue;

        {
            CExoString sKey("EnableCheats");
            CExoString sSection("Game Options");
            CExoString sFileName(".\\swkotor2.ini");

            cIni.ReadIniEntry(sValue, sFileName, sSection, sKey);
        }

        if (sValue.AsINT() == 1)
        {
            if (g_cConsole.m_bActive != 0)
            {
                if (g_bConsoleKeyboardCaptured != 0)
                    g_pExoInput->KeyboardModeOff();
                g_cConsole.m_bActive = 0;
            }
            else
            {
                g_bConsoleKeyboardCaptured =
                    (g_pExoInput->GetKeyboardMode() == 0);
                g_pExoInput->KeyboardModeOn();
                g_pExoInput->ClearEvents();
                g_pAppManager->m_pClientExoApp->TurnOffKeyboardModal();
                g_cConsole.m_bActive = 1;
            }
        }
        return 1;
    }

    if (g_pExoInput->GetKeyboardMode() != 0)
    {
        if (g_cConsole.m_bActive != 0)
        {
            DoConsole((char)nChar);
            return 1;
        }
        if (g_pGuiMan != NULL && g_pGuiMan->HandleKeyPress(nChar) != 0)
            return 1;
    }
    return 0;
}
#endif
#ifndef _XBOX
int HandleWMKeyDownMessage(HWND hWnd, unsigned int nKey, long nKeyData)  // PC 0x00402530
{
    if (g_pAppManager->m_pClientExoApp->GetCaptureEventPlayBack() != 0)
        return 0;

    if (g_pAppManager->m_pClientExoApp->GetCaptureEvents() != 0 &&
        g_pAppManager->m_pClientExoApp->GetCaptureEventFile() != NULL)
    {
        CExoString sEvent;

        sEvent.Format(
            "%d %c %d\n",
            (unsigned int)(g_pAppManager->m_pClientExoApp->GetWorldTimer()
                               ->GetSnapshotTime() /
                           1000),
            'K', nKey);

        CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;

        pClientExoApp->GetCaptureEventFile()->Write(sEvent);
    }

    switch (nKey)
    {
    case VK_LEFT:
    case VK_UP:
    case VK_RIGHT:
    case VK_DOWN:
        if (g_pExoInput->GetKeyboardMode() != 0)
        {
            if (g_cConsole.m_bActive != 0)
            {
                DoConsoleHist(nKey);
                return 0;
            }
            switch (nKey)
            {
            case VK_UP:
                HandleWMCharMessage(0x1D);
                return 0;
            case VK_DOWN:
                HandleWMCharMessage(0x1F);
                return 0;
            case VK_LEFT:
                HandleWMCharMessage(0x1C);
                return 0;
            case VK_RIGHT:
                HandleWMCharMessage(0x1E);
                return 0;
            }
        }
        break;

    case VK_F10:
        return 1;

    case VK_PROCESSKEY:
        if (g_pAppManager->m_pClientExoApp->IsClientImeUI() != 0 &&
            g_pAppManager->m_pClientExoApp->GetImeUI()->m_nCompositionActive !=
                0)
        {
            CClientExoApp* pImeApp = g_pAppManager->m_pClientExoApp;

            pImeApp->GetImeUI()->HandleImeKeyDown(hWnd, nKey, nKeyData);
        }
        break;
    }
    return 0;
}
#endif

#ifndef _XBOX
LRESULT CALLBACK WinMessageHandler(HWND hWnd, UINT nMsg, WPARAM wParam,  // PC 0x00402840
                                   LPARAM lParam)
{
    switch (nMsg)
    {
    case WM_SIZE:
        if (hWnd == g_hRenderWnd)
        {
            if (wParam == SIZE_MAXIMIZED)
            {
                g_nReInitializing = 2;
                return 0;
            }
            return DefWindowProcA(g_hRenderWnd, nMsg, wParam, lParam);
        }
        break;

    case WM_KILLFOCUS:
        if (g_pAppManager->GetClientExoApp()->IsMoviePlaying() == 0)
        {
            g_pAppManager->GetClientExoApp()->SetInputActive(0, 0);
            g_bKillFocusWorked = 1;
        }
        else
        {
            g_bKillFocusWorked = 0;
        }
        break;

    case WM_SETFOCUS:
        if (g_bKillFocusWorked == 1)
            g_pAppManager->GetClientExoApp()->SetInputActive(1, 0);
        break;

    case WM_WINDOWPOSCHANGING:
        if (g_pAppManager->GetClientExoApp()->IsMoviePlaying() == 1)
        {
            WINDOWPOS* pPos = (WINDOWPOS*)lParam;
            WINDOWPOS* pCopy = new WINDOWPOS;

            pCopy->hwnd = g_hMovieWnd;
            pCopy->hwndInsertAfter = 0;
            pCopy->cx = pPos->cx;
            pCopy->cy = pPos->cy;
            pCopy->x = pPos->x;
            pCopy->y = pPos->y;
            pCopy->flags = 0;
            PostMessageA(g_hMovieWnd, WM_MOVIE_WINDOWPOS_CHANGING, wParam, (LPARAM)pCopy);
        }
        break;

    case WM_ACTIVATEAPP:
        if (g_pExoInput != NULL)
        {
            if (wParam == 0)
            {
                g_bActivationDueToFullScreenMovie = 0;
                g_bActivationDueToAnyMovie = 0;
                int bMinimize = 1;

                if (g_pAppManager != NULL &&
                    g_pAppManager->GetClientExoApp() != NULL)
                {
                    if (g_pAurora != NULL)
                    {
                        if (g_pAppManager->GetClientExoApp()->IsMoviePlaying() == 1)
                        {
                            if (g_pAurora->FullScreen() != 0)
                                g_bActivationDueToFullScreenMovie = 1;
                            g_bActivationDueToAnyMovie = 1;
                        }
                        if (g_bActivationDueToFullScreenMovie != 0)
                            bMinimize = 0;
                    }
                    else
                    {
                        bMinimize = 0;
                    }
                }
                if (bMinimize && g_pAurora != NULL &&
                    g_pAurora->FullScreen() == 1)
                {
                    ShowWindow(g_hRenderWnd, SW_MINIMIZE);
                    g_pAurora->RestoreDesktopDisplayMode();
                }
                if (hWnd == g_hRenderWnd)
                {
                    DeactivateRenderWindow(0);
                    g_nWindowDeactivationTickCount = GetTickCount();
                }
                if (g_bActivationDueToAnyMovie == 0)
                {
                    RestoreMouseTrailsState();
                    RestoreScreenSaverState();
                }
            }
            else
            {
                if (g_bActivationDueToFullScreenMovie == 0 && g_pAurora != NULL)
                {
                    if (g_pAurora->FullScreen() != 0)
                    {
                        if (g_nReInitializing == 0)
                        {
                            if (hWnd == g_hRenderWnd)
                                g_pAurora->RestoreAuroraDisplayMode();
                            g_nReInitializing = 2;
                        }
                    }
                    else if (g_bActivationDueToAnyMovie == 0 &&
                             g_pAppManager->GetClientExoApp()->IsMoviePlaying() ==
                                 0 &&
                             GetTickCount() - g_nWindowDeactivationTickCount > 100)
                    {
                        g_pAppManager->GetClientExoApp()
                            ->RestartMoviesIfNecessary();
                    }
                }
                if (hWnd == g_hRenderWnd &&
                    (GetTickCount() - g_nWindowDeactivationTickCount < 1000 ||
                     g_pAurora->FullScreen() == 0))
                {
                    ActivateRenderWindow();
                }
                if (g_bActivationDueToAnyMovie == 0)
                {
                    StoreMouseTrailsState();
                    StoreScreenSaverState();
                }
            }
        }
        break;

    case WM_SETCURSOR:
        if ((HWND)wParam == g_hRenderWnd && g_pExoInput != NULL &&
            (unsigned short)lParam == HTCLIENT)
        {
            g_pExoInput->RefreshMouseCursor(0);
            return 1;
        }
        break;

    case WM_CANCELMODE:
        if (g_bMouseCameraRotateToggle != 0)
            g_bMouseCameraRotateToggle = 0;
        break;

    case WM_CLOSE:
        g_pAppManager->GetClientExoApp()->QueryExitProgram();
        return 0;

    case WM_WINDOWPOSCHANGED:
        if (g_pAppManager->GetClientExoApp()->IsMoviePlaying() == 1)
        {
            WINDOWPOS* pPos = (WINDOWPOS*)lParam;
            WINDOWPOS* pCopy = new WINDOWPOS;

            pCopy->hwnd = g_hMovieWnd;
            pCopy->hwndInsertAfter = 0;
            pCopy->cx = pPos->cx;
            pCopy->cy = pPos->cy;
            pCopy->x = pPos->x;
            pCopy->y = pPos->y;
            pCopy->flags = 0;
            PostMessageA(g_hMovieWnd, WM_MOVIE_WINDOWPOS_CHANGED, wParam, (LPARAM)pCopy);
        }
        break;

    case WM_MOVIE_RESTORE_RESOLUTION:
        RestoreResolutionFromMovie();
        return 0;

    case WM_MOVIE_MODE_ACTIVATE:
        NotifyModeChangeActivate(1, wParam != 0);
        return 0;

    case WM_SYSKEYDOWN:
        switch (wParam)
        {
        case VK_MENU:
            g_pAppManager->GetClientExoApp()->SetInputActive(0, 0);
            return 0;
        case VK_F4:
            if ((lParam & 0x20000000) != 0)
                g_pAppManager->GetClientExoApp()->SetInputActive(1, 0);
            SendMessageA(hWnd, WM_CLOSE, wParam, lParam);
            break;
        case VK_RETURN:
            if ((lParam & 0x20000000) != 0)
                return 0;
            break;
        case VK_F10:
            return 0;
        }
        break;

    case WM_SYSKEYUP:
        if (wParam != VK_RETURN)
        {
            if (wParam != VK_MENU)
            {
                if (wParam == VK_F10)
                    return 0;
                if ((lParam & 0x20000000) != 0)
                    g_pAppManager->GetClientExoApp()->SetInputActive(1, 0);
            }
            else
            {
                g_pAppManager->GetClientExoApp()->SetInputActive(1, 0);
                return 0;
            }
        }
        else if ((lParam & 0x20000000) != 0)
        {
            NotifyModeChangeActivate(0, 0);
        }
        break;

    case WM_SYSCHAR:
        if (wParam == VK_RETURN && (lParam & 0x20000000) != 0)
            return 0;
        break;

    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE && g_pAppManager != NULL &&
            g_pAppManager->GetClientExoApp() != NULL &&
            g_pAppManager->GetClientExoApp()->IsMoviePlaying() == 1)
        {
            g_pAppManager->GetClientExoApp()->CancelMovie();
        }
        if (g_pAppManager->GetClientExoApp()->GetCaptureEventPlayBack() == 0 &&
            HandleWMKeyDownMessage(hWnd, wParam, lParam) == 1)
        {
            return 0;
        }
        break;

    case WM_CHAR:
        if (g_pAppManager->GetClientExoApp()->GetCaptureEventPlayBack() == 0 &&
            HandleWMCharMessage(wParam) == 1)
        {
            return 0;
        }
        break;

    case WM_INPUTLANGCHANGE:
    case WM_IME_SETCONTEXT:
    case WM_IME_CONTROL:
    case WM_IME_COMPOSITIONFULL:
    case WM_IME_KEYDOWN:
        if (g_pAppManager->GetClientExoApp()->IsClientImeUI() != 0)
            return 0;
        break;

    case WM_IME_STARTCOMPOSITION:
        if (g_pAppManager->GetClientExoApp()->IsClientImeUI() != 0)
        {
            g_pAppManager->GetClientExoApp()->GetImeUI()->m_nFlags |=
                CExoImeUI::COMPOSITION_IN_PROGRESS;
            return 0;
        }
        break;

    case WM_IME_ENDCOMPOSITION:
        if (g_pAppManager->GetClientExoApp()->IsClientImeUI() != 0)
        {
            g_pAppManager->GetClientExoApp()->GetImeUI()->m_nFlags &=
                ~CExoImeUI::COMPOSITION_IN_PROGRESS;
            return 0;
        }
        break;

    case WM_IME_COMPOSITION:
        if (g_pAppManager->GetClientExoApp()->IsClientImeUI() != 0)
        {
            CClientExoApp* pClientExoApp = g_pAppManager->GetClientExoApp();

            pClientExoApp->GetImeUI()->HandleImeKeyDown(hWnd, wParam, lParam);
            return 0;
        }
        break;

    case WM_MOUSEMOVE:
        if (g_pExoInput != NULL && g_pExoInput->GetActive() != 0)
        {
            MSG cEvent;

            cEvent.message = nMsg;
            cEvent.wParam = wParam;
            cEvent.lParam =
                (unsigned short)lParam |
                ((unsigned short)(g_nScreenHeight -
                                  (unsigned short)((unsigned long)lParam >> 16) -
                                  1)
                 << 16);
            g_pExoInput->BufferEvent(&cEvent);
        }
        break;

    case WM_LBUTTONDOWN:
        if (g_pExoInput == NULL || g_pExoInput->GetActive() == 0)
            return 0;
        {
            MSG cEvent;

            cEvent.message = nMsg;
            if (g_pAppManager->GetClientExoApp()->GetClientOptions() != NULL &&
                g_pAppManager->GetClientExoApp()->GetClientOptions()
                        ->m_bReverseMouseButtons != 0)
            {
                nMsg = WM_RBUTTONDOWN;
                cEvent.message = nMsg;
            }
            cEvent.wParam = wParam;
            cEvent.lParam =
                (unsigned short)lParam |
                ((unsigned short)(g_nScreenHeight -
                                  (unsigned short)((unsigned long)lParam >> 16) -
                                  1)
                 << 16);
            g_pExoInput->BufferEvent(&cEvent);
            SetCapture(hWnd);
        }
        break;

    case WM_LBUTTONDBLCLK:
        if (g_pExoInput == NULL || g_pExoInput->GetActive() == 0)
            return 0;
        {
            MSG cEvent;

            cEvent.message = nMsg;
            if (g_pAppManager->GetClientExoApp()->GetClientOptions() != NULL &&
                g_pAppManager->GetClientExoApp()->GetClientOptions()
                        ->m_bReverseMouseButtons != 0)
            {
                nMsg = WM_RBUTTONDBLCLK;
                cEvent.message = nMsg;
            }
            cEvent.wParam = wParam;
            cEvent.lParam =
                (unsigned short)lParam |
                ((unsigned short)(g_nScreenHeight -
                                  (unsigned short)((unsigned long)lParam >> 16) -
                                  1)
                 << 16);
            g_pExoInput->BufferEvent(&cEvent);
        }
        break;

    case WM_LBUTTONUP:
        if (g_pExoInput == NULL || g_pExoInput->GetActive() == 0)
            return 0;
        {
            MSG cEvent;

            cEvent.message = nMsg;
            if (g_pAppManager->GetClientExoApp()->GetClientOptions() != NULL &&
                g_pAppManager->GetClientExoApp()->GetClientOptions()
                        ->m_bReverseMouseButtons != 0)
            {
                nMsg = WM_RBUTTONUP;
                cEvent.message = nMsg;
            }
            cEvent.wParam = wParam;
            cEvent.lParam =
                (unsigned short)lParam |
                ((unsigned short)(g_nScreenHeight -
                                  (unsigned short)((unsigned long)lParam >> 16) -
                                  1)
                 << 16);
            g_pExoInput->BufferEvent(&cEvent);
        }
        break;

    case WM_RBUTTONDBLCLK:
        if (g_pExoInput == NULL || g_pExoInput->GetActive() == 0)
            return 0;
        {
            MSG cEvent;

            cEvent.message = nMsg;
            if (g_pAppManager->GetClientExoApp()->GetClientOptions() != NULL &&
                g_pAppManager->GetClientExoApp()->GetClientOptions()
                        ->m_bReverseMouseButtons != 0)
            {
                nMsg = WM_LBUTTONDBLCLK;
                cEvent.message = nMsg;
            }
            cEvent.wParam = wParam;
            cEvent.lParam =
                (unsigned short)lParam |
                ((unsigned short)(g_nScreenHeight -
                                  (unsigned short)((unsigned long)lParam >> 16) -
                                  1)
                 << 16);
            g_pExoInput->BufferEvent(&cEvent);
            SetCapture(hWnd);
        }
        break;

    case WM_RBUTTONDOWN:
        if (g_pExoInput == NULL || g_pExoInput->GetActive() == 0)
            break;
        {
            MSG cEvent;

            cEvent.message = nMsg;
            if (g_pAppManager->GetClientExoApp()->GetClientOptions() != NULL &&
                g_pAppManager->GetClientExoApp()->GetClientOptions()
                        ->m_bReverseMouseButtons != 0)
            {
                nMsg = WM_LBUTTONDOWN;
                cEvent.message = nMsg;
            }
            cEvent.wParam = wParam;
            cEvent.lParam =
                (unsigned short)lParam |
                ((unsigned short)(g_nScreenHeight -
                                  (unsigned short)((unsigned long)lParam >> 16) -
                                  1)
                 << 16);
            g_pExoInput->BufferEvent(&cEvent);
            SetCapture(hWnd);
        }
        break;

    case WM_RBUTTONUP:
        if (g_pExoInput == NULL || g_pExoInput->GetActive() == 0)
            break;
        {
            MSG cEvent;

            cEvent.message = nMsg;
            if (g_pAppManager->GetClientExoApp()->GetClientOptions() != NULL &&
                g_pAppManager->GetClientExoApp()->GetClientOptions()
                        ->m_bReverseMouseButtons != 0)
            {
                nMsg = WM_LBUTTONUP;
                cEvent.message = nMsg;
            }
            cEvent.wParam = wParam;
            cEvent.lParam =
                (unsigned short)lParam |
                ((unsigned short)(g_nScreenHeight -
                                  (unsigned short)((unsigned long)lParam >> 16) -
                                  1)
                 << 16);
            g_pExoInput->BufferEvent(&cEvent);
        }
        break;

    case WM_SYSCOMMAND:
        if ((wParam & 0xFFFFFFF0) == SC_SIZE)
            return 0;
        switch ((unsigned short)wParam)
        {
        case SC_SCREENSAVE:
        case SC_MONITORPOWER:
            return 0;
        }
        break;

    case WM_MBUTTONDOWN:
        if (g_pExoInput != NULL && g_pExoInput->GetActive() != 0)
        {
            MSG cEvent;

            cEvent.lParam =
                (unsigned short)lParam |
                ((unsigned short)(g_nScreenHeight -
                                  (unsigned short)((unsigned long)lParam >> 16) -
                                  1)
                 << 16);
            cEvent.message = WM_MBUTTONDOWN;
            cEvent.wParam = wParam;
            g_pExoInput->BufferEvent(&cEvent);
            SetCapture(hWnd);
        }
        break;

    case WM_MOUSEWHEEL:
        if (g_pExoInput != NULL && g_pExoInput->GetActive() != 0)
        {
            POINT ptScreen;
            MSG cEvent;

            ptScreen.x = (unsigned short)lParam;
            cEvent.message = nMsg;
            cEvent.wParam = wParam;
            ptScreen.y = (unsigned long)lParam >> 16;
            ScreenToClient(hWnd, &ptScreen);
            cEvent.lParam =
                (unsigned short)ptScreen.x |
                ((unsigned short)(g_nScreenHeight - ptScreen.y - 1) << 16);
            g_pExoInput->BufferEvent(&cEvent);
        }
        break;

    case WM_MBUTTONUP:
        if (g_pExoInput != NULL && g_pExoInput->GetActive() != 0)
        {
            MSG cEvent;

            cEvent.message = nMsg;
            cEvent.wParam = wParam;
            cEvent.lParam =
                (unsigned short)lParam |
                ((unsigned short)(g_nScreenHeight -
                                  (unsigned short)((unsigned long)lParam >> 16) -
                                  1)
                 << 16);
            g_pExoInput->BufferEvent(&cEvent);
        }
        break;

    case WM_IME_NOTIFY:
        if (g_pAppManager->GetClientExoApp()->IsClientImeUI() != 0)
        {
            CClientExoApp* pClientExoApp = g_pAppManager->GetClientExoApp();

            pClientExoApp->GetImeUI()->HandleImeNotify(hWnd, wParam, lParam);
            return 0;
        }
        break;
    }
    return DefWindowProcA(hWnd, nMsg, wParam, lParam);
}
#endif
void InitOpenGLWindow(HINSTANCE hInstance, int nCmdShow);

#ifndef _XBOX
HWND InitGameApp(HINSTANCE hInstance, int nCmdShow)  // PC 0x00403F70
{

    static char szClassName[] = "KotOR2";
    HWND hWnd;

    GetGameVersionInfo();
    CExoString sTitle = g_sCopyrightInfo + " " + g_sGameName;
    CExoString sGameName = g_sGameName + "    v" + g_sInstallBuildNumber + "." +
                           g_sBuildNumber;

    WNDCLASSA wc;

    wc.style = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc = WinMessageHandler;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIconA(hInstance, MAKEINTRESOURCE(0x65));
    wc.hCursor = NULL;
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = szClassName;
    RegisterClassA(&wc);

    hWnd = CreateWindowExA(
        WS_EX_TOOLWINDOW,
        szClassName,
        sGameName.CStr(),
        WS_CLIPCHILDREN | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL,
        hInstance, NULL);
    if (hWnd == NULL)
        return NULL;

    HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONULL);
    if (hMonitor != NULL)
    {
        MONITORINFO mi;

        memset(&mi, 0, sizeof(mi));
        mi.cbSize = sizeof(mi);
        if (!GetMonitorInfoA(hMonitor, &mi) ||
            mi.dwFlags != MONITORINFOF_PRIMARY)
            return NULL;
    }

    InitOpenGLWindow(hInstance, nCmdShow);
    g_hWnd = hWnd;
    g_hInst = hInstance;
    return hWnd;
}
#endif

#ifndef _XBOX
void InitOpenGLWindow(HINSTANCE hInstance, int nCmdShow)  // PC 0x00403730
{
    char szClassName[16];
    WNDCLASSA wc;

    strcpy(szClassName, "Render Window");
    CExoString sTitle = g_sGameName;

    wc.style = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc = WinMessageHandler;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIconA(hInstance, MAKEINTRESOURCE(0x65));
    wc.hCursor = NULL;
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = szClassName;
    RegisterClassA(&wc);

    HWND hWnd = CreateWindowExA(WS_EX_APPWINDOW, szClassName, sTitle.CStr(),
                                WS_CLIPCHILDREN | WS_CAPTION | WS_SYSMENU |
                                    WS_MINIMIZEBOX,
                                CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
    if (hWnd == NULL)
        return;
    g_hRenderWnd = hWnd;
}
#endif

#ifndef _XBOX
void ReInitAurora(unsigned long nWidth, unsigned long nHeight,  // PC 0x00403840
                  unsigned char nBitsPerPixel, int nFlags, int bRestartMovies)
{
    unsigned char nMouseMode = 0xFF;

    if (g_pAurora != NULL)
    {
        g_pAppManager->m_pClientExoApp->HideMouse(64);

        int bWasFullScreen = 0;
        HANDLE hEvent;
        HANDLE hThread;
        HWND hTrayWnd;
        DWORD dwThreadId;

        if (g_pAurora->FullScreen() == 1 && nFlags == 1)
        {
            bWasFullScreen = 1;
            hEvent = CreateEventA(NULL, FALSE, FALSE, NULL);
            hThread = CreateThread(NULL, 0x8000, StartAddress, hEvent, 0,
                                   &dwThreadId);
            SetThreadPriority(hThread, THREAD_PRIORITY_TIME_CRITICAL);
            GetTickCount();
            hTrayWnd = FindWindowA("Shell_TrayWnd", NULL);
            if (hTrayWnd != NULL)
            {
                EnableWindow(hTrayWnd, FALSE);
                ShowWindow(hTrayWnd, SW_HIDE);
            }
        }

        int bAlreadyReiniting = 0;
        if (g_bInReInitAurora == 1)
            bAlreadyReiniting = 1;
        else
            g_bInReInitAurora = 1;

        GLfloat fClearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
        glGetFloatv(GL_COLOR_CLEAR_VALUE, fClearColor);

        HWND hOldRenderWnd = g_hRenderWnd;
        g_pAurora->ShutDown(0, bWasFullScreen == 0);
        g_hRenderWnd = NULL;

        CExoIni cIni;
        CExoString sValue;
        int bHaveEntry;

        {
            CExoString sKey("Disable Vertex Buffer Objects");
            CExoString sSection("Graphics Options");
            CExoString sFileName(".\\swkotor2.ini");

            bHaveEntry =
                cIni.ReadIniEntry(sValue, sFileName, sSection, sKey);
        }
        if (bHaveEntry != 0 && sValue.AsINT() == 1)
            AurDisableVertexBufferObjects();

        GetTickCount();
        if (g_hRenderWnd != NULL)
        {
            g_pAppManager->m_pClientExoApp->SetInputActive(0, 0);
            delete g_pExoInput;
            g_pExoInput = NULL;
        }

        InitOpenGLWindow(g_hInst, 1);

        unsigned long nRefreshRate = g_nDesiredFullScreenRefreshRate;
        if (nFlags != 0)
            g_pAurora->GetValidMode(&nWidth, &nHeight, &nBitsPerPixel,
                                    &nRefreshRate, true);
        g_pAurora->Initialize(g_hWnd, g_hRenderWnd, nWidth, nHeight,
                              nBitsPerPixel, nRefreshRate, nFlags,
                              nBitsPerPixel, nBitsPerPixel);

        {
            CExoString sKey("Disable Write-Only VBO");
            CExoString sSection("Graphics Options");
            CExoString sFileName(".\\swkotor2.ini");

            bHaveEntry =
                cIni.ReadIniEntry(sValue, sFileName, sSection, sKey);
        }
        if (bHaveEntry != 0)
        {
            if (sValue.AsINT() == 1)
                AurDisableVertexBufferObjectsWriteOnly();
            else
                AurEnableVertexBufferObjectsWriteOnly();
        }

        SetCapture(g_hRenderWnd);
        if (hOldRenderWnd != NULL)
            DestroyWindow(hOldRenderWnd);
        GetTickCount();
        SetupOpenGL();
        if (g_nReInitializing != 0)
            g_nReInitializing = 0;
        glClearColor(fClearColor[0], fClearColor[1], fClearColor[2],
                     fClearColor[3]);

        if (g_pExoInput != NULL)
        {
            nMouseMode = g_pExoInput->GetMouseMode();
            delete g_pExoInput;
        }
        g_pExoInput = new CExoInput();
        if (nMouseMode != 0xFF)
            g_pExoInput->SetMouseMode(nMouseMode);

        DeactivateRenderWindow(0);
        if (g_pAppManager->m_pClientExoApp->IsClientInitialized() != 0)
            g_pAppManager->m_pClientExoApp->SetEventDescriptions();
        ActivateRenderWindow();
        GetTickCount();

        if (g_pAppManager != NULL && g_pAppManager->m_pClientExoApp != NULL)
        {
            if (bAlreadyReiniting == 0)
            {
                if (bRestartMovies == 1)
                    g_pAppManager->m_pClientExoApp->RestartMoviesIfNecessary();
                g_bInReInitAurora = 0;
            }
        }
        else if (bAlreadyReiniting == 0)
        {
            g_bInReInitAurora = 0;
        }

        if (bWasFullScreen == 1)
        {
            hTrayWnd = FindWindowA("Shell_TrayWnd", NULL);
            if (hTrayWnd != NULL)
            {
                EnableWindow(hTrayWnd, TRUE);
                ShowWindow(hTrayWnd, SW_SHOWNOACTIVATE);
            }
            SetEvent(hEvent);
            GetTickCount();
            DWORD dwExitCode;

            do
            {
                GetExitCodeThread(hThread, &dwExitCode);
            } while (dwExitCode == STILL_ACTIVE);
            GetTickCount();
            CloseHandle(hThread);
            CloseHandle(hEvent);
            GetTickCount();
        }

        g_pAppManager->m_pClientExoApp->ShowMouse(64);
    }
}
#endif

#ifndef _XBOX
void ChangeResolutionForMovie()  // PC 0x00403D40
{
    g_nMovieResolutionDepth++;
    g_nLastResolutionWidth = g_nScreenWidth;
    g_nLastResolutionHeight = g_nScreenHeight;
    g_nLastResolutionBitsPerPixel = (unsigned char)g_nBitsPerPixel;
    if (g_pAurora->FullScreen())
    {
        g_bMovieResolutionChanged = 1;

        CClientExoAppInternal* pInternal =
            g_pAppManager->m_pClientExoApp->m_pcExoAppInternal;

        if (!g_fullScreenDesired || pInternal->m_bMovieDisabled ||
            pInternal->m_bSkipIntro)
        {
            if (g_nScreenWidth != 640 || g_nScreenHeight != 480 ||
                (unsigned char)g_nBitsPerPixel != 32)
            {
                AurSetAntiAliasingMode(0);
                ReInitAurora(640, 480, 32, 1, 0);
            }
        }
        g_fullScreenDesired = 0;
        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_LOWEST);
    }
    else
    {
        g_bMovieResolutionChanged = 0;
        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_LOWEST);
    }
}

#endif

#ifndef _XBOX
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,  // PC 0x00404250
                   LPSTR lpCmdLine, int nCmdShow)
{
    SECURITY_ATTRIBUTES cMutexAttr;

    memset(&cMutexAttr, 0, sizeof(cMutexAttr));
    cMutexAttr.nLength = sizeof(cMutexAttr);

    HANDLE hMutex = CreateMutexA(&cMutexAttr, FALSE, "swkotor2");
    if (hMutex == NULL)
        return (WPARAM)-1;
    if (WaitForSingleObject(hMutex, 0) != WAIT_OBJECT_0)
    {
        CloseHandle(hMutex);
        return (WPARAM)-1;
    }

    CoInitialize(NULL);

    MSG msg;
    PeekMessageA(&msg, NULL, 0, 0, PM_NOREMOVE);

    dofile("config.txt");

    g_pExoBase = new CExoBase();
    {
        CExoString sAliasFile("swKotor2.ini");

        g_pExoBase->LoadAliases(sAliasFile);
    }

    g_pAppManager = new CAppManager();

    if (InitGameApp(hInstance, nCmdShow) == 0)
        return 0;

    StoreMouseTrailsState();
    StoreScreenSaverState();

    CExoIni cIni;
    CExoString sDisableSound;
    int bHaveEntry;

    {
        CExoString sKey("Disable Sound");
        CExoString sSection("Sound Options");
        CExoString sFileName(".\\swkotor2.ini");

        bHaveEntry =
            cIni.ReadIniEntry(sDisableSound, sFileName, sSection, sKey);
    }

    if (bHaveEntry != 0)
    {
        g_bDisableSound = (sDisableSound.AsINT() != 0);
    }
    else
    {
        sDisableSound.Format("%d", 0);
        CExoString sKey2("Disable Sound");
        CExoString sSection2("Sound Options");
        CExoString sFileName2(".\\swkotor2.ini");

        cIni.WriteIniEntry(sDisableSound, sFileName2, sSection2, sKey2);
    }

    ProcessCommandLine(__argc, __argv);
    AurRenderCallback = messagepump;

    int bStartFailed;
    {
        CExoString sArg1("");
        CExoString sArg2("");
        CExoString sArg3("");

        CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;

        bStartFailed = pClientExoApp->StartServices(0, sArg3, sArg2, sArg1, 0);
    }
    if (bStartFailed != 0)
    {
        CloseHandle(hMutex);
        return 0;
    }

    g_pAppManager->m_pClientExoApp->Initialize();
    g_bExitProgram = 0;
    g_pAppManager->m_pClientExoApp->SetInputActive(1, 0);
    dofile("startup.txt");
    AurSetPauseOverlayColor(0.0f, 0.0f, 0.0f, 0.0f);
    g_pAppManager->m_pClientExoApp->SetLoadStep(0, 10);
    g_pAppManager->m_pClientExoApp->SetLoadStep(1, 20);
    g_pAppManager->m_pClientExoApp->SetLoadStep(2, 23);
    g_pAppManager->m_pClientExoApp->SetLoadStep(3, 23);
    g_pAppManager->m_pClientExoApp->SetLoadStep(4, 23);
    g_pAppManager->m_pClientExoApp->BeginIntro();
    g_pAppManager->m_pClientExoApp->DisplayMainMenu(1);

    if (g_bExitProgram == 0)
    {
        do
        {
            memoryused = (int)(262144.0f - memoryfree);

            if (PeekMessageA(&msg, NULL, 0, 0, PM_NOREMOVE))
            {
                if (!GetMessageA(&msg, NULL, 0, 0))
                    break;
                TranslateMessage(&msg);
                DispatchMessageA(&msg);
            }
            else
            {
                if (g_nResetPriority != 0)
                {
                    g_nResetPriority = 0;
                    if (g_nPriorityClass == 2)
                        SetPriorityClass(GetCurrentProcess(),
                                         REALTIME_PRIORITY_CLASS);
                    if (g_nPriorityClass == 1)
                        SetPriorityClass(GetCurrentProcess(),
                                         HIGH_PRIORITY_CLASS);
                    else
                        SetPriorityClass(GetCurrentProcess(),
                                         NORMAL_PRIORITY_CLASS);
                }

                if (g_nReInitializing != 0)
                {
                    if (g_pAppManager->m_pClientExoApp->m_pcExoAppInternal
                            ->m_bEvilEndingHackInProgress)
                        ReInitAurora(640, 480, 32, g_nReInitializing == 2, 1);
                    else
                        ReInitAurora(g_nScreenWidth, g_nScreenHeight,
                                     g_nBitsPerPixel, g_nReInitializing == 2,
                                     1);
                }

                if (g_pAppManager->m_pClientExoApp->IsMoviePlaying() == 0)
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
                            GL_STENCIL_BUFFER_BIT);
                if (g_bHeadLightEnabled != 0)
                    AurEnableHeadLight();
                else
                    AurDisableHeadLight();

                g_nStartTimer =
                    g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
                g_bExitProgram |= g_pAppManager->m_pClientExoApp->MainLoop();
                g_nEndTimer =
                    g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
                g_ClientUpdateTime =
                    (float)(g_nEndTimer - g_nStartTimer) * 0.001f;

                g_nStartTimer =
                    g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
                for (int i = 0; i < 1; ++i)
                {
                    if (g_pAppManager->m_pServerExoApp != NULL)
                    {
                        g_bExitProgram =
                            g_pAppManager->m_pServerExoApp->MainLoop() |
                            g_bExitProgram;
                        if (g_pAppManager->m_pReentrantServerStats
                                ->m_bOperationActive == 1)
                            break;
                    }
                }
                g_nEndTimer =
                    g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
                g_ServerUpdateTime =
                    (float)(g_nEndTimer - g_nStartTimer) * 0.001f;

                if (g_cConsole.m_bActive != 0)
                {
                    AurPostString(">", 0, 0, 5.0f);
                    AurPostString(g_cConsole.m_szLine, 2, 0, 5.0f);
                }
                if (g_cConsole.m_bSubmitted != 0)
                {
                    char* sResult = FuncInterp(g_cConsole.m_szLine);

                    AurPostString(sResult, 0, 1, 5.0f);
                    g_cConsole.m_szLine[0] = 0;
                    g_cConsole.m_nCursorPos = 0;
                    g_cConsole.m_bSubmitted = 0;
                }
                if (g_pAppManager->m_pClientExoApp->IsMoviePlaying() == 0)
                    AurRenderStrings();
                if (g_cConsole.m_bActive != 0)
                {
                    glColor4f(0.7f, 0.7f, 0.3f, 1.0f);
                    AurPrintString("_", g_cConsole.m_nCursorPos + 2, 0);
                }
                if (g_pAppManager->m_pClientExoApp->IsMoviePlaying() == 0 &&
                    g_nReInitializing == 0)
                {
                    g_bSwappingBuffers = 1;
                    HDC hDC = GetDC(g_hRenderWnd);
                    SwapBuffers(hDC);
                    ReleaseDC(g_hRenderWnd, hDC);
                    g_bSwappingBuffers = 0;
                }
                g_pAppManager->m_pClientExoApp
                    ->EnableMouseAtEndOfLoadModuleMovie();
                if (g_bNiceClient != 0)
                    Sleep(0);

                if (displayFPS > 0.0f)
                {
                    float fTargetMs = 1000.0f / displayFPS;

                    g_nFpsCheckTimer =
                        g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
                    if ((float)(g_nFpsCheckTimer - g_nFpsFrameTimer) * 0.001f <
                        fTargetMs)
                    {
                        do
                        {
                            g_nFpsCheckTimer = g_pExoBase->m_pExoTimers
                                                   ->GetHighResolutionTimer();
                        } while ((float)(g_nFpsCheckTimer - g_nFpsFrameTimer) *
                                     0.001f <
                                 fTargetMs);
                    }
                    g_nFpsFrameTimer =
                        g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
                }
            }
        } while (g_bExitProgram == 0);
    }

    g_pAppManager->DestroyServer();
    g_pAppManager->m_pClientExoApp->Uninitialize();
    g_pAppManager->m_pClientExoApp->StopServices();

    if ((int)g_nMouseTrails >= 1)
        SystemParametersInfoA(SPI_SETMOUSETRAILS, g_nMouseTrails, NULL, 0);
    if ((int)g_nScreenSaverEnabled >= 1)
        SystemParametersInfoA(SPI_SETSCREENSAVEACTIVE, g_nScreenSaverEnabled,
                              NULL, 0);

    delete g_pAppManager;
    g_pAppManager = NULL;
    delete g_pExoBase;
    g_pExoBase = NULL;

    CoUninitialize();
    CloseHandle(hMutex);
    return msg.wParam;
}
#endif

#ifndef _XBOX
void RestoreResolutionFromMovie()  // PC 0x00403E30
{
    unsigned long nFlags;
    CClientExoAppInternal* pInternal =
        g_pAppManager->m_pClientExoApp->m_pcExoAppInternal;

    if (pInternal != NULL && pInternal->m_bEvilEndingHackInProgress)
    {
        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
        pInternal->HideMovieFadePanel();
        if (g_bMovieResolutionChanged == 1 && !g_bMovieResolutionLocked)
        {
            ActivateRenderWindow();
        }
        return;
    }

    g_nMovieResolutionDepth--;
    if (g_nMovieResolutionDepth < 0)
    {
        g_nMovieResolutionDepth = 0;
        return;
    }

    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
    if (pInternal != NULL)
    {
        pInternal->HideMovieFadePanel();
    }
    if (g_bVideoModeSettingsDirty)
    {
        g_bVideoModeSettingsDirty = 0;
        if (pInternal != NULL)
        {
            pInternal->ReadVideoModeSettings(&g_nLastResolutionWidth,
                                             &g_nLastResolutionHeight,
                                             &g_nLastResolutionBitsPerPixel,
                                             &nFlags);
        }
    }
    if (g_bMovieResolutionChanged == 1 && !g_bMovieResolutionLocked)
    {
        if (g_nLastResolutionWidth == 640 && g_nLastResolutionHeight == 480 &&
            g_nLastResolutionBitsPerPixel == 32)
        {
            ActivateRenderWindow();
            return;
        }
        AurSetAntiAliasingMode(
            g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nAntiAlias);
        ReInitAurora(g_nLastResolutionWidth, g_nLastResolutionHeight,
                     g_nLastResolutionBitsPerPixel, g_bMovieResolutionChanged, 0);
    }
}
#endif

#ifdef _XBOX

int HandleWMCharMessage(unsigned short nChar)
{
    if (g_pAppManager->m_pClientExoApp->GetCaptureEvents() != 0 &&
        g_pAppManager->m_pClientExoApp->GetCaptureEventFile() != NULL)
    {
        CExoString sEvent;
        CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;

        sEvent.Format("%d %c %d\n",
                      (unsigned int)(pClientExoApp->GetWorldTimer()->GetSnapshotTime() /
                                     1000),
                      'C', nChar);
        pClientExoApp = g_pAppManager->m_pClientExoApp;
        pClientExoApp->GetCaptureEventFile()->Write(sEvent);
    }

    unsigned short nCheatChar;
    switch (g_pAppManager->m_pClientExoApp->GetClientLanguage())
    {
    case 2:
        nCheatChar = 176;
        break;
    default:
        nCheatChar = 96;
        break;
    }

    if (nChar == nCheatChar)
    {
        if (g_cConsole.m_bActive != 0)
        {
            if (g_bConsoleKeyboardCaptured != 0)
                g_pExoInput->KeyboardModeOff();
            g_cConsole.m_bActive = 0;
        }
        else
        {
            g_bConsoleKeyboardCaptured =
                (g_pExoInput->GetKeyboardMode() == 0);
            g_pExoInput->KeyboardModeOn();
            g_pAppManager->m_pClientExoApp->TurnOffKeyboardModal();
            g_cConsole.m_bActive = 1;
        }
        return 1;
    }

    if (g_pExoInput->GetKeyboardMode() != 0)
    {
        if (g_cConsole.m_bActive != 0)
        {
            DoConsole((char)nChar);
            return 1;
        }
        switch (nChar)
        {
        case '\n':
        case '\r':
            g_cConsole.m_szLine[0] = 0;
            g_cConsole.m_nCursorPos = 0;
            g_cConsole.m_bActive = 1;
            return 1;
        }
    }
    return 0;
}
#endif

#ifdef _XBOX

extern float g_fSoundRenderTime;
extern float g_fSoundStreamingTime;
extern float g_fSoundPlayingTime;
extern float g_fSoundNonPlayingTime;
extern float g_ServerAIUpdateTime;
extern int animationfootprint;
extern int RyanTotalLevelVertSize;
extern int RyanTotalLevelVertices;

int g_mdxMem;
int g_txbMem;

char* Sound(char* sBuffer)
{
    static char sResult[128];
    sprintf(sResult, " Tot[%4.2f] Strm[%4.2f] Play[%4.2f] NonPlayg[%4.2f]",
            g_fSoundRenderTime, g_fSoundStreamingTime, g_fSoundPlayingTime,
            g_fSoundNonPlayingTime);
    return sResult;
}
ConsoleFunc ConFSound("Sound", Sound);

char* Buffers(char* sBuffer)
{
    static char sResult[128];
    sprintf(sResult, "VB[%d][%d] IB[%d][%d]",
            totalvertexpool / BYTES_PER_KB, totalvertexpool4k / BYTES_PER_KB,
            totalindexpool / BYTES_PER_KB, totalindexpool4 / BYTES_PER_KB);
    return sResult;
}
ConsoleFunc ConFBuffers("Buffers", Buffers);

char* C_Stats(char* sBuffer)
{
    static char sResult[128];
    sprintf(sResult, "tot[%04.1f] snd[%04.1f] AI:[%04.1f] gui:[%04.1f]",
            g_ClientUpdateTime, g_SndUpdateTime, g_AIUpdateTime, g_GuiUpdateTime);
    return sResult;
}
ConsoleFunc ConFC_Stats("C_Stats", C_Stats);

char* S_Stats(char* sBuffer)
{
    static char sResult[128];
    sprintf(sResult, "tot[%05.2f] AI[%05.2f]", g_ServerUpdateTime,
            g_ServerAIUpdateTime);
    return sResult;
}
ConsoleFunc ConFS_Stats("S_Stats", S_Stats);

char* AI_Stats(char* sBuffer)
{
    static char sResult[128];
    sprintf(sResult, "Mod[%05.2f] Obj[%05.2f]", g_AIModuleTime, g_AIObjectTime);
    return sResult;
}
ConsoleFunc ConFAI_Stats("AI_Stats", AI_Stats);

char* TexMem(char* sBuffer)
{
    static char sResult[128];
    int nUncompressed = usedtexturememory - usedlightmapmemory - usedcomptexmemory -
                        usedcubemapmemory - usedbumpmapmemory;

    sprintf(sResult, "(kB) Uncmp %d Cmp %d LM %d CM %d BM %d",
            nUncompressed / BYTES_PER_KB, usedcomptexmemory / BYTES_PER_KB,
            usedlightmapmemory / BYTES_PER_KB, usedcubemapmemory / BYTES_PER_KB,
            usedbumpmapmemory / BYTES_PER_KB);
    return sResult;
}
ConsoleFunc ConFTexMem("TexMem", TexMem);

char* GfxMemOld(char* sBuffer)
{
    static char sResult[128];
    sprintf(sResult, "(kB) Textures %d Verts %d Anims %d",
            usedtexturememory / BYTES_PER_KB, totalvertexpool / BYTES_PER_KB,
            animationfootprint / BYTES_PER_KB);
    return sResult;
}
ConsoleFunc ConFGfxMemOld("GfxMemOld", GfxMemOld);

char* XbxMem(char* sBuffer)
{
    static char sResult[128];
    int nTotalMem;
    if (usedcomptexmemory == 0)
    {
        int nTextures = usedtexturememory - usedlightmapmemory - usedcubemapmemory -
                        usedbumpmapmemory;
        nTotalMem = XBOX_TOTAL_MEMORY - (int)(nTextures * XBOX_TEXTURE_COMPRESSION) -
                    totalvertexpool - animationfootprint - usedbumpmapmemory -
                    usedcubemapmemory - usedlightmapmemory;
    }
    else
    {
        nTotalMem = XBOX_TOTAL_MEMORY - totalvertexpool - animationfootprint -
                    usedtexturememory;
    }

    int nLevelMem = XBOX_LEVEL_MEMORY - totalvertexpool - usedtexturememory;
    if (nLevelMem > XBOX_LEVEL_MEMORY_CAP)
    {
        nLevelMem = XBOX_LEVEL_MEMORY_CAP;
    }

    sprintf(sResult, "Parts %d Total Mem %d Level Mem %d", countpart,
            nTotalMem / BYTES_PER_KB, nLevelMem / BYTES_PER_KB);
    return sResult;
}
ConsoleFunc ConFXbxMem("XbxMem", XbxMem);

char* GuiMem(char* sBuffer)
{
    static char sResult[128];
    sprintf(sResult, "Tex %d Part %d", RyanStartTextureSize / BYTES_PER_KB,
            RyanStartParts);
    return sResult;
}
ConsoleFunc ConFGuiMem("GuiMem", GuiMem);

char* LevMem(char* sBuffer)
{
    static char sResult[128];
    sprintf(sResult, "Tot %d Tex %d Part %d Vert %d-%d",
            (RyanLevelTextureSize + RyanLevelVertSize) / BYTES_PER_KB,
            RyanLevelTextureSize / BYTES_PER_KB, RyanLevelTotalParts,
            RyanLevelVertices, RyanLevelVertSize / BYTES_PER_KB);
    return sResult;
}
ConsoleFunc ConFLevMem("LevMem", LevMem);

char* DynMem(char* sBuffer)
{
    static char sResult[128];
    sprintf(sResult, "Tot %d Tex %d Part %d Vert %d-%d",
            (usedtexturememory - RyanEndTextureSize - RyanLevelVertSize +
             RyanTotalLevelVertSize) / BYTES_PER_KB,
            (usedtexturememory - RyanEndTextureSize) / BYTES_PER_KB,
            countpart - RyanEndParts, RyanTotalLevelVertices - RyanLevelVertices,
            (RyanTotalLevelVertSize - RyanLevelVertSize) / BYTES_PER_KB);
    return sResult;
}
ConsoleFunc ConFDynMem("DynMem", DynMem);

char* GfxMem(char* sBuffer)
{
    static char sResult[128];
    int nResources;
    int nKeys;
    int nServicedSize;
    int nImageSize;

    g_pExoResMan->GetResourcesInMemory(&nResources, &nKeys, &nServicedSize,
                                       &nImageSize, &g_mdlMem, &g_mdxMem,
                                       &g_txbMem, &g_wavMem);
    sprintf(sResult, "tot: %d md:%d mx:%d tx:%d",
            (g_mdlMem + g_mdxMem + g_txbMem) / BYTES_PER_KB,
            g_mdlMem / BYTES_PER_KB, g_mdxMem / BYTES_PER_KB,
            g_txbMem / BYTES_PER_KB);
    return sResult;
}
ConsoleFunc ConFGfxMem("GfxMem", GfxMem);

char* ResMem(char* sBuffer)
{
    static char sResult[128];
    int nResources;
    int nKeys;
    int nServicedSize;
    int nImageSize;

    int nTotal = g_pExoResMan->GetResourcesInMemory(
        &nResources, &nKeys, &nServicedSize, &nImageSize, &g_mdlMem,
        &g_mdxMem, &g_txbMem, &g_wavMem);
    sprintf(sResult, "%d %d %d/%d %d", nTotal / BYTES_PER_KB,
            nServicedSize / BYTES_PER_KB, nResources, nKeys,
            nImageSize / BYTES_PER_KB);
    return sResult;
}
ConsoleFunc ConFResMem("ResMem", ResMem);

char* ResMem2(char* sBuffer)
{
    static char sResult[128];
    sprintf(sResult, "tot: %d md:%d mx:%d tx:%d wv:%d",
            (g_mdlMem + g_mdxMem + g_txbMem + g_wavMem) / BYTES_PER_KB,
            g_mdlMem / BYTES_PER_KB, g_mdxMem / BYTES_PER_KB,
            g_txbMem / BYTES_PER_KB, g_wavMem / BYTES_PER_KB);
    return sResult;
}
ConsoleFunc ConFResMem2("ResMem2", ResMem2);

char* ClearLogCounters(char* sBuffer)
{
    return "NOT ENABLED";
}
ConsoleFunc ConFClearLogCounters("ClearLogCounters", ClearLogCounters);

char* DumpLogCounters(char* sBuffer)
{
    return "NOT ENABLED";
}
ConsoleFunc ConFDumpLogCounters("DumpLogCounters", DumpLogCounters);

ConsoleFunc ConVverts("verts", &verts);
ConsoleFunc ConVanims("anims", &anims);
ConsoleFunc ConVtexts("texts", &texts);
ConsoleFunc ConVparts("parts", &parts);
ConsoleFunc ConVverts2("verts2", &verts2);

char* tc(char* sBuffer)
{
    static char sResult[128];
    int nLevelVertices = RyanTotalLevelVertices;
    int nVertices = totalvertexpool;
    int nTextures = usedtexturememory;
    int nAnimations = animationfootprint;
    int nParts = countpart;

    spawn(sBuffer);
    AurTextureMakeCurrent();

    verts = (totalvertexpool - nVertices) / BYTES_PER_KB;
    anims = (animationfootprint - nAnimations) / BYTES_PER_KB;
    texts = (usedtexturememory - nTextures) / BYTES_PER_KB;
    parts = countpart - nParts;
    verts2 = RyanTotalLevelVertices - nLevelVertices;

    sprintf(sResult, "Verts %d-%d Anims %d Text %d Parts %d", verts2, verts,
            anims, texts, parts);
    return sResult;
}
ConsoleFunc ConFtc("tc", tc);

ConsoleFunc ConVg_bDo3DGUIs("g_bDo3DGUIs", &g_bDo3DGUIs);
ConsoleFunc ConVg_nSleeptime("g_nSleeptime", (int*)&g_nSleeptime);
ConsoleFunc ConVg_nPriorityClass("g_nPriorityClass", &g_nPriorityClass);
ConsoleFunc ConVg_nResetPriority("g_nResetPriority", &g_nResetPriority);

int antialiasmode = 0;
ConsoleFunc ConVantialiasmode("antialiasmode", &antialiasmode);
int g_nOldFrameBufferMode = antialiasmode;

int g_bLowDiskMode = 0;
unsigned char g_nBootFlags = 0;

void ExoXboxLaunch(KOTOR_LAUNCH_DATA* pParams, const char* pszModuleName)
{
    XLaunchNewImage("d:\\default.xbe", (PLAUNCH_DATA)pParams);
}

void SetWindowTitle()
{
}

int HandleWMKeyDownMessage(HWND hWnd, unsigned int nKey, long nKeyData)
{
    return 0;
}

void ReInitAurora(unsigned long nWidth, unsigned long nHeight,
                  unsigned char nBitsPerPixel, int nFlags)
{
    if (g_pAurora != NULL)
    {
        float fClearColor[4];
        fClearColor[0] = (float)(g_nClearColor & 0xFF) * (1.0f / 255.0f);
        fClearColor[1] = (float)((unsigned char*)&g_nClearColor)[1] * (1.0f / 255.0f);
        fClearColor[2] = (float)((unsigned char*)&g_nClearColor)[2] * (1.0f / 255.0f);
        fClearColor[3] = (float)((unsigned char*)&g_nClearColor)[3] * (1.0f / 255.0f);

        g_pAurora->ShutDown(0);
        g_pAurora->Initialize(g_hWnd, g_hWnd, nWidth, nHeight, nBitsPerPixel,
                              g_nDesiredFullScreenRefreshRate, 0,
                              nBitsPerPixel, nBitsPerPixel);
        SetupOpenGL();
        AurTextureRefreshAll();
        if (g_nReInitializing != 0)
            g_nReInitializing = 0;
        GLRender::SetClearColor(fClearColor[0], fClearColor[1], fClearColor[2],
                                fClearColor[3]);
    }
}

int main()
{
    KOTOR_LAUNCH_DATA cParams;
    LAUNCH_DATA cLaunchData;
    DWORD nLaunchType;
    DWORD bNoLaunchData;

    XSetFileCacheSize(0x80000);

    bNoLaunchData = XGetLaunchInfo(&nLaunchType, &cLaunchData);
    if (bNoLaunchData == 0)
    {
        if (((KOTOR_LAUNCH_DATA*)&cLaunchData)->m_nMagic ==
            KOTOR_LAUNCH_DATA::MAGIC)
            memcpy(&cParams, &cLaunchData, sizeof(cParams));
        else
            bNoLaunchData = 1;
    }

    CSWPartyTable::ResetCurrentSessionStartTime();
    if (bNoLaunchData == 0)
    {
        g_bLowDiskMode = cParams.m_bLowDiskMode;
        g_nBootFlags = cParams.m_nBootFlags;
        g_nCurrentSessionStartFILETIME =
            cParams.m_nCurrentSessionStartFILETIME;
    }

    g_pExoBase = new CExoBase;
    g_pExoBase->LoadAliases("swdirsXbox.INI");
    g_pAppManager = new CAppManager;

    CExoIni cIni;
    CExoString sDisableSound;
    int bHaveEntry;

    {
        CExoString sKey("DisableSound");
        CExoString sSection("Sound Options");
        CExoString sFileName("d:\\swkotor2.ini");

        bHaveEntry =
            cIni.ReadIniEntry(sDisableSound, sFileName, sSection, sKey);
    }

    if (bHaveEntry != 0)
    {
        g_bDisableSound = (sDisableSound.AsINT() != 0);
    }
    else
    {
        sDisableSound.Format("%d", 0);
        CExoString sKey2("DisableSound");
        CExoString sSection2("Sound Options");
        CExoString sFileName2("d:\\swkotor2.ini");

        cIni.WriteIniEntry(sDisableSound, sFileName2, sSection2, sKey2);
    }

    if (bNoLaunchData == 0)
    {
        g_pAppManager->m_pClientExoApp->GetClientOptions()->SetDefaults();
        if (cParams.m_bOptionsDataValid == 1)
            g_pAppManager->m_pClientExoApp->GetClientOptions()
                ->SetOptionsData(&cParams.m_cOptionsData);

        g_pAppManager->m_pClientExoApp->AddMoveToModuleMovie(cParams.GetMovie1());
        g_pAppManager->m_pClientExoApp->AddMoveToModuleMovie(cParams.GetMovie2());
        g_pAppManager->m_pClientExoApp->AddMoveToModuleMovie(cParams.GetMovie3());
        g_pAppManager->m_pClientExoApp->AddMoveToModuleMovie(cParams.GetMovie4());
        g_pAppManager->m_pClientExoApp->AddMoveToModuleMovie(cParams.GetMovie5());
        g_pAppManager->m_pClientExoApp->AddMoveToModuleMovie(cParams.GetMovie6());
        g_pAppManager->m_pClientExoApp->SetMinGlobalRim(cParams.m_bMinGlobalRim);
        g_pAppManager->m_pClientExoApp->SetLoadScreenHintUpdateTimer(
            cParams.m_nLoadScreenHintUpdateTimer);
        g_pAppManager->m_pClientExoApp->SetGamePlayHint(cParams.m_nGamePlayHint);
        g_pAppManager->m_pClientExoApp->SetStoryHint(cParams.m_nStoryHint);
    }

    if (cParams.m_bLoadingSave)
        g_pAppManager->m_pClientExoApp->SetLoadingSaveGame(1);

    if (bNoLaunchData == 0 &&
        (cParams.m_nModuleTrans == KOTOR_LAUNCH_DATA::MODULETRANS_LOADGAME ||
         cParams.m_bLoadAutoSave))
    {
        g_pAppManager->m_pClientExoApp->SetLoadStep(0, 50);
        g_pAppManager->m_pClientExoApp->SetLoadStep(1, 10);
        g_pAppManager->m_pClientExoApp->SetLoadStep(2, 13);
        g_pAppManager->m_pClientExoApp->SetLoadStep(3, 13);
        g_pAppManager->m_pClientExoApp->SetLoadStep(4, 13);
    }
    else
    {
        g_pAppManager->m_pClientExoApp->SetLoadStep(0, 10);
        g_pAppManager->m_pClientExoApp->SetLoadStep(1, 30);
        g_pAppManager->m_pClientExoApp->SetLoadStep(2, 20);
        g_pAppManager->m_pClientExoApp->SetLoadStep(3, 20);
        g_pAppManager->m_pClientExoApp->SetLoadStep(4, 20);
    }

    g_pAppManager->m_pClientExoApp->StartServices(
        bNoLaunchData == 0, cParams.GetLoadScreen(), cParams.GetLoadMusic(),
        cParams.GetModuleName(), cParams.m_nLoadScreenHint);
    g_pAppManager->m_pClientExoApp->Initialize();
    g_pAppManager->m_pClientExoApp->GetClientOptions()->ApplySoundOptions();
    g_bExitProgram = 0;
    g_pAppManager->m_pClientExoApp->SetInputActive(1, 0);
    g_sBuildNumber = "385632";

    if (bNoLaunchData != 0)
    {
        g_pAppManager->m_pClientExoApp->DisplayStartScreen();
    }
    else
    {
        if (cParams.m_bOptionsDataValid == 0)
            g_pAppManager->m_pClientExoApp->LoadOptions(1);
        g_pAppManager->m_pClientExoApp->GetClientOptions()->ApplySoundOptions();
        g_pAurora->ResetGammaRamp(
            g_pAppManager->m_pClientExoApp->GetClientOptions()->GetGammaSetting(),
            0);
        g_pAppManager->m_pClientExoApp->GetInGameGui()->SetStatusSummary(
            cParams.m_cStatusSummary);
        g_pExoInput->SetAutoCalInfo(cParams.m_nAutoCalInfo);
        g_pExoInput->SetPortBinding(cParams.m_nPortBinding);

        if (cParams.m_nModuleTrans == KOTOR_LAUNCH_DATA::MODULETRANS_NONE)
        {
            g_pAppManager->m_pClientExoApp->HideLoadScreen(1, 0);
            g_pAppManager->m_pClientExoApp->DisplayMainMenu(0);
        }
        else if (cParams.m_nModuleTrans ==
                 KOTOR_LAUNCH_DATA::MODULETRANS_LOADGAME)
        {
            g_pExoBase->m_pExoAliasList->ChangePath("SAVES",
                                                cParams.GetXboxSavePath());

            CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
            pClientExoApp->SetLoadScreenByModuleName(cParams.GetModuleName());
            pClientExoApp->SetLoadScreenHint(cParams.m_nLoadScreenHint);
            pClientExoApp->ShowLoadScreen(1, 1, 0);
            g_pAppManager->CreateServer(0);
            g_pAppManager->m_pClientExoApp->GetWorldTimer()->PauseWorldTimer();
            g_pAppManager->m_pServerExoApp->GetWorldTimer()->PauseWorldTimer();
            pClientExoApp->GetNetLayer()->StartConnectToSession(
                0, pClientExoApp->GetClientName(), "", 10, 16);
            pClientExoApp->GetSWCMessage()
                ->SendPlayerToServerStatus_RequestStatus();
            pClientExoApp->SetPlayerCharacterName("test");
            pClientExoApp->SetInputClass(0, 1);
            pClientExoApp->DisableInput();
            g_pAppManager->m_pServerExoApp->SetLoadFromSaveGame(1);
            g_pAppManager->m_pClientExoApp->SetLoadingSaveGame(1);
            g_pAppManager->m_pClientExoApp->SendLoadGameRequest(
                cParams.m_nSaveSlot, cParams.GetStartWaypoint(),
                cParams.GetModuleName());
        }
        else if (cParams.m_nModuleTrans ==
                 KOTOR_LAUNCH_DATA::MODULETRANS_MODULE)
        {
            CExoString sMessage;
            CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;

            pClientExoApp->SetLoadScreenByModuleName(cParams.GetModuleName());
            if (pClientExoApp->GetMoveToModuleMovies()->num == 0)
                pClientExoApp->ShowLoadScreen(1, 1, 0);
            g_pAppManager->CreateServer(1);
            g_pAppManager->m_pClientExoApp->GetWorldTimer()->PauseWorldTimer();
            g_pAppManager->m_pServerExoApp->GetWorldTimer()->PauseWorldTimer();
            g_pAppManager->m_pServerExoApp->SetDoingStartNewModule(1);
            g_pExoResMan->AddResourceDirectory("REBOOTDATA:");
            if (g_pAppManager->m_pServerExoApp->GetPartyTable()->LoadTableInfo() &&
                g_pAppManager->m_pServerExoApp->GetGlobalVariableTable()->Load())
            {
                g_pExoResMan->RemoveResourceDirectory("REBOOTDATA:");
                g_pExoResMan->NukeDirectory("REBOOTDATA:", 0, 0);
                g_pAppManager->m_pClientExoApp->SetLoadFromIFO(1);
                pClientExoApp->GetNetLayer()->StartConnectToSession(
                    0, pClientExoApp->GetClientName(), "", 10, 16);
                pClientExoApp->GetSWCMessage()
                    ->SendPlayerToServerStatus_RequestStatus();
                sMessage.Format("%c%s.%s %s", 's', "Module", "Load",
                                cParams.GetModuleName().CStr());
                pClientExoApp->GetNetLayer()->SendMessageToPlayer(
                    0xFFFFFFFD, (unsigned char*)sMessage.CStr(),
                    sMessage.GetLength(), 0);
                if (cParams.GetStartWaypoint() != "*")
                    g_pAppManager->m_pServerExoApp->SetMoveToModuleStartWaypoint(
                        cParams.GetStartWaypoint());
                else
                    g_pAppManager->m_pServerExoApp->SetMoveToModuleStartWaypoint(
                        "");
                g_pAppManager->m_pServerExoApp->SetMoveToWorldTime(
                    cParams.m_nTimeYear, cParams.m_nTimeMonth,
                    cParams.m_nTimeDay, cParams.m_nTimeHour,
                    cParams.m_nTimeMinute, cParams.m_nTimeSecond,
                    cParams.m_nTimeMillisecond);
                g_pAppManager->m_pServerExoApp->SetMoveToPausedTimeOfDay(
                    cParams.m_nTimePauseTime);
                g_pAppManager->m_pServerExoApp->SetMoveToPausedCalendarDay(
                    cParams.m_nTimePauseDay);
                pClientExoApp->SetPlayerCharacterName("test");
                pClientExoApp->SetInputClass(0, 1);
            }
            else
            {
                g_pAppManager->m_pReentrantServerStats->StartStallEvent(1);
                g_pAppManager->m_pReentrantServerStats->m_nUnknown56 = 4;
            }
        }
    }

    while (g_bExitProgram == 0)
    {
        memoryused = (int)(65536.0f - memoryfree);
        if (maxmemoryused < memoryused)
            maxmemoryused = memoryused;

        if (g_nOldFrameBufferMode != antialiasmode)
        {
            g_nOldFrameBufferMode = antialiasmode;
            AurSetFrameBufferMode(antialiasmode);
        }

        if (g_nReInitializing != 0)
            ReInitAurora(g_nScreenWidth, g_nScreenHeight, g_nBitsPerPixel,
                         g_nReInitializing == 2);

        GLRender::Clear(3);
        if (g_bHeadLightEnabled != 0)
            AurEnableHeadLight();
        else
            AurDisableHeadLight();

        g_nStartTimer = g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
        g_bExitProgram |= g_pAppManager->m_pClientExoApp->MainLoop();
        g_nEndTimer = g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
        g_ClientUpdateTime = (float)(g_nEndTimer - g_nStartTimer) * 0.001f;

        g_nStartTimer = g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
        for (int i = 0; i < 1; ++i)
        {
            if (g_pAppManager->m_pServerExoApp != NULL)
            {
                g_bExitProgram |= g_pAppManager->m_pServerExoApp->MainLoop();
                if (g_pAppManager->m_pReentrantServerStats->m_bOperationActive ==
                    1)
                {
                    g_pAppManager->m_pClientExoApp->GetSWGuiManager()->Draw(
                        1.0f / 30.0f);
                    break;
                }
            }
        }
        g_nEndTimer = g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
        g_ServerUpdateTime = (float)(g_nEndTimer - g_nStartTimer) * 0.001f;

        if (g_cConsole.m_bActive != 0)
        {
            AurPostString(">", 0, 0, 5.0f);
            AurPostString(g_cConsole.m_szLine, 2, 0, 5.0f);
        }
        if (g_cConsole.m_bSubmitted != 0)
        {
            char* sResult = FuncInterp(g_cConsole.m_szLine);

            AurPostString(sResult, 0, 1, 5.0f);
            g_cConsole.m_szLine[0] = 0;
            g_cConsole.m_nCursorPos = 0;
            g_cConsole.m_bSubmitted = 0;
        }
        GLRender::LoadViewIdentity();
        AurRenderStrings();
        if (g_cConsole.m_bActive != 0)
        {
            GLRender::SetColor4f(0.7f, 0.7f, 0.3f, 1.0f);
            AurPrintString("_", g_cConsole.m_nCursorPos + 2, 0);
        }
        messagepump();
        if (g_bNiceClient != 0)
            Sleep(g_nSleeptime);

        if (displayFPS > 0.0f)
        {
            float fTargetMs = 1000.0f / displayFPS;

            g_nFpsCheckTimer =
                g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
            if ((float)(g_nFpsCheckTimer - g_nFpsFrameTimer) * 0.001f <
                fTargetMs)
            {
                do
                {
                    g_nFpsCheckTimer =
                        g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
                } while ((float)(g_nFpsCheckTimer - g_nFpsFrameTimer) * 0.001f <
                         fTargetMs);
            }
            g_nFpsFrameTimer =
                g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
        }
    }

    g_pAppManager->DestroyServer();
    g_pAppManager->m_pClientExoApp->Uninitialize();
    g_pAppManager->m_pClientExoApp->StopServices();

    delete g_pAppManager;
    g_pAppManager = NULL;
    delete g_pExoBase;
    g_pExoBase = NULL;
    return 0;
}
#endif

#ifdef _XBOX
#include "swguiingame.h"

void HandleLoadError(unsigned long nError)
{
    CSWGuiMessageBox* pBox = g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
    pBox->ShowLoadError(nError);
    g_pAppManager->m_pClientExoApp->GetSWGuiManager()->AddPanel(pBox, 1, 1);
    g_pExoInput->SetActive(1);
    g_pAppManager->m_pClientExoApp->EnableInput();
    g_pAppManager->m_pClientExoApp->SetInputClass(2);
}
#endif
