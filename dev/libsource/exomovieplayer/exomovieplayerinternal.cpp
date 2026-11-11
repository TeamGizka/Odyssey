#include <stdio.h>
#include <windows.h>
#include <GL/gl.h>
#include "bink.h"
#include "mss.h"

HWND g_hMovieWnd;

#include "appmanager.h"
#include "aurinternal.h"
#include "clientexoapp.h"
#include "clientexoappinternal.h"
#include "clientoptions.h"
#include "swguifade.h"
#include "exobase.h"
#include "exoresman.h"

#include "exomovieplayerinternal.h"
#include "swmain.h"

template <class T> inline void ExoMovieListTrace(CExoArrayList<T>*)
{
}

template <class T> inline void ExoArrayListReleased(CExoArrayList<T>* pList)
{
    ExoMovieListTrace(pList);
}

void MovieSetUpProjection()
{
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
}
// PC 0x00404BD0
void AurOrtho(double dLeft, double dRight, double dBottom, double dTop,
              double dNear, double dFar)
{
    glOrtho(dLeft, dRight, dBottom, dTop, dNear, dFar);
}

void MovieSetUpAlphaTest()
{
    glAlphaFunc(GL_GREATER, 0.0f);
}

static CExoMoviePlayerInternal* g_pMoviePlayer;
// PC 0x00404F70
LRESULT CALLBACK MovieMessageHandler(HWND hWnd, UINT nMsg, WPARAM wParam,
                                    LPARAM lParam)
{
    switch (nMsg)
    {
    case WM_ACTIVATEAPP:
        if (wParam == 0 && g_pMoviePlayer != NULL &&
            g_pMoviePlayer->m_bShuttingDown == 0)
        {
            g_pMoviePlayer->OnDeactivation(0);
            g_pMoviePlayer->CancelMovie(1, 1);
        }
        break;
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE && g_pMoviePlayer != NULL)
        {
            g_pMoviePlayer->CancelMovie(0, 0);
        }
        break;
    case WM_SYSKEYDOWN:
        if (wParam == VK_F4 && (lParam & (KF_ALTDOWN << 16)) != 0 &&
            g_hWnd != NULL)
        {
            PostMessageA(g_hWnd, WM_CLOSE, 0, 0);
        }
        break;
    case WM_SYSKEYUP:
        if (wParam == VK_RETURN && (lParam & (KF_ALTDOWN << 16)) != 0 &&
            g_pMoviePlayer != NULL)
        {
            g_pMoviePlayer->OnDeactivation(1);
            g_pMoviePlayer->CancelMovie(1, 1);
            return 0;
        }
        break;
    case WM_LBUTTONDOWN:
        return 0;
    case WM_LBUTTONDBLCLK:
        return 0;
    case WM_RBUTTONDOWN:
        return 0;
    case WM_RBUTTONDBLCLK:
        return 0;
    case WM_LBUTTONUP:
        if (g_pMoviePlayer != NULL &&
            g_pAppManager->GetClientExoApp()->GetClientOptions()->m_bReverseMouseButtons == 0)
            g_pMoviePlayer->CancelMovie(0, 0);
        return 0;
    case WM_RBUTTONUP:
        if (g_pMoviePlayer != NULL &&
            g_pAppManager->GetClientExoApp()->GetClientOptions()->m_bReverseMouseButtons != 0)
            g_pMoviePlayer->CancelMovie(0, 0);
        return 0;
    case WM_MOVIE_WINDOWPOS_CHANGING:
        g_pMoviePlayer->OnWindowPosChanging((WINDOWPOS*)lParam);
        delete (WINDOWPOS*)lParam;
        return 0;
    case WM_MOVIE_WINDOWPOS_CHANGED:
        g_pMoviePlayer->OnWindowPosChanged((WINDOWPOS*)lParam);
        delete (WINDOWPOS*)lParam;
        return 0;
    }
    return DefWindowProcA(hWnd, nMsg, wParam, lParam);
}
// PC 0x00405E30
CExoMoviePlayerInternal::CExoMoviePlayerInternal()
    : m_lstMovies(0),
      m_lstSkippable(0)
{
    m_sCurrentMovie = "";
    m_bMoviePlaying = 0;
    m_bExitRequested = 0;
    m_hWakeEvent = CreateEventA(NULL, FALSE, FALSE, NULL);
    m_hWorkerThread = CreateThread(
        NULL, 0x8000,
        (LPTHREAD_START_ROUTINE)CExoMoviePlayerInternal::PlayMoviesAsync, this,
        0, (LPDWORD)&m_dwWorkerThreadId);
    m_nUnknown34 = 0;
    m_bCancelMovie = 0;
    m_nCancelResult = 0;
    m_nUnknown1C = 0;
    m_nUnknown20 = 0;
    m_bShuttingDown = 0;
    m_pFadePanel = NULL;
    m_hBinkBuffer = NULL;
    m_hBink = NULL;
    m_nUnknown70 = 0;
    m_hMovieWnd = NULL;
}
// PC 0x00405B10
CExoMoviePlayerInternal::~CExoMoviePlayerInternal()
{
    DWORD dwExitCode;

    m_bExitRequested = 1;
    SetEvent(m_hWakeEvent);
    do
    {
        GetExitCodeThread(m_hWorkerThread, &dwExitCode);
    } while (dwExitCode == STILL_ACTIVE);
    CloseHandle(m_hWorkerThread);
    CloseHandle(m_hWakeEvent);

    if (m_pFadePanel != NULL)
    {
        delete m_pFadePanel;
        m_pFadePanel = NULL;
    }

    ShutDown();
}
// PC 0x00404C20
int CExoMoviePlayerInternal::ShutDown()
{
    int nResult;
    HBINKBUFFER hBinkBuffer;
    BINKSUMMARY sum;

    nResult = (int)m_hBink;
    m_bShuttingDown = 1;
    if (nResult != 0)
    {
        BinkGetSummary(m_hBink, &sum);
        BinkPause(m_hBink, 1);
        BinkClose(m_hBink);
        hBinkBuffer = m_hBinkBuffer;
        m_hBink = NULL;
        if (hBinkBuffer != NULL)
        {
            BinkBufferClose(hBinkBuffer);
            m_hBinkBuffer = NULL;
        }
        nResult = DestroyWindow(m_hMovieWnd);
        m_hMovieWnd = NULL;
        g_hMovieWnd = NULL;
    }
    m_bShuttingDown = 0;
    return nResult;
}
// PC 0x00404C90
void CExoMoviePlayerInternal::OnDeactivation(int bToggleFullScreen)
{
    m_nUnknown1C = 1;
    if (m_nUnknown20 == 0 || bToggleFullScreen == 1)
        m_nUnknown20 = bToggleFullScreen;
}
// PC 0x00404CB0
int CExoMoviePlayerInternal::CancelMovie(int nA, int nB)
{
    if (nB == 0)
    {
        if (m_bMoviePlaying == 1)
        {
            if (m_bSkippable == 0)
            {
                m_nCancelResult = nA;
                return nA;
            }
        }
        else
        {
            m_nCancelResult = nA;
            return 1;
        }
    }

    m_bCancelMovie = 1;
    m_nCancelResult = nA;
    return nA;
}
// PC 0x004056E0
int CExoMoviePlayerInternal::RunMovieLoop(int nWait)
{
    MSG msg;
    BINKSUMMARY summary;

    if (m_hBink != NULL)
    {
        while (m_hBink->Frames != m_hBink->FrameNum)
        {
            BinkDoFrame(m_hBink);
            if (BinkBufferLock(m_hBinkBuffer))
            {
                BinkCopyToBuffer(m_hBink, m_hBinkBuffer->Buffer,
                                 m_hBinkBuffer->BufferPitch,
                                 m_hBinkBuffer->Height, 0, 0,
                                 m_hBinkBuffer->SurfaceType);
                BinkBufferUnlock(m_hBinkBuffer);
            }
            BinkBufferBlit(m_hBinkBuffer, m_hBink->FrameRects,
                           BinkGetRects(m_hBink, m_hBinkBuffer->SurfaceType));
            BinkNextFrame(m_hBink);
            while (BinkWait(m_hBink) == 1)
            {
                Sleep(1);
            }
            if (m_bCancelMovie == 1)
            {
                break;
            }
            while (PeekMessageA(&msg, NULL, 0, 0, PM_NOREMOVE))
            {
                if (!GetMessageA(&msg, NULL, 0, 0))
                {
                    break;
                }
                TranslateMessage(&msg);
                DispatchMessageA(&msg);
            }
        }
        BinkGetSummary(m_hBink, &summary);
    }
    return 0;
}
// PC 0x00404CF0
int CExoMoviePlayerInternal::IsMoviePlaying()
{
    return m_bMoviePlaying;
}
// PC 0x00404D00
void CExoMoviePlayerInternal::FinishMovie()
{
    ShutDown();
    m_bMoviePlaying = 0;
}
// PC 0x00405840
void CExoMoviePlayerInternal::ShowFadePanel()
{
    m_critSect.EnterCriticalSection();
    if (m_pFadePanel == NULL)
    {
        m_pFadePanel = new CSWGuiFade(g_pAppManager->GetClientExoApp()->GetSWGuiManager());
        m_pFadePanel->m_vColor = Vector(0.0f, 0.0f, 0.0f);
    }
    g_pAppManager->GetClientExoApp()->GetSWGuiManager()->AddPanel(m_pFadePanel, CSWGuiManager::PANEL_MODE_MODAL, 1);
    m_pFadePanel->SetAlpha(1.0f);
    UpdateScreen(1.0f / 30.0f, 0, 1);
    m_critSect.LeaveCriticalSection();
}
// PC 0x00405940
void CExoMoviePlayerInternal::HideFadePanel()
{
    m_critSect.EnterCriticalSection();
    if (m_pFadePanel != NULL)
    {
        m_pFadePanel->SetAlpha(0.0f);
        g_pAppManager->GetClientExoApp()->GetSWGuiManager()->RemovePanel(m_pFadePanel);
    }
    m_critSect.LeaveCriticalSection();
}
// PC 0x00404D20
void CExoMoviePlayerInternal::OnWindowPosChanging(WINDOWPOS* pWindowPos)
{
    if (m_hBinkBuffer != NULL)
    {
        S32 nX = pWindowPos->x;
        S32 nY = pWindowPos->y;
        BinkBufferCheckWinPos(m_hBinkBuffer, &nX, &nY);
        pWindowPos->x = nX;
        pWindowPos->y = nY;
    }
}
// PC 0x00404D60
void CExoMoviePlayerInternal::OnWindowPosChanged(WINDOWPOS*)
{
    if (m_hBinkBuffer != NULL)
        BinkBufferSetOffset(m_hBinkBuffer, m_nVideoOffsetX, m_nVideoOffsetY);

    WINDOWINFO wi;
    wi.cbSize = sizeof(wi);
    if (GetWindowInfo(g_hRenderWnd, &wi))
    {
        if (!MoveWindow(m_hMovieWnd, wi.rcClient.left, wi.rcClient.top,
                        wi.rcClient.right - wi.rcClient.left,
                        wi.rcClient.bottom - wi.rcClient.top, TRUE))
            GetLastError();
        HDC hDC = GetDC(g_hRenderWnd);
        PatBlt(hDC, 0, 0, g_nScreenWidth, g_nScreenHeight, BLACKNESS);
        ReleaseDC(g_hRenderWnd, hDC);
        ShowWindow(m_hMovieWnd, SW_SHOWNA);
    }
}
// PC 0x00404E60
int CExoMoviePlayerInternal::GetNumberOfMoviesToPlay()
{
    return m_lstMovies.num;
}
// PC 0x00404E70
int CExoMoviePlayerInternal::RestartMoviesAsync()
{
    SetThreadPriority(m_hWorkerThread, THREAD_PRIORITY_NORMAL);
    SetThreadPriority(m_hWorkerThread, THREAD_PRIORITY_NORMAL);
    m_bMoviePlaying = 1;
    return SetEvent(m_hWakeEvent);
}

double MovieGetFrameRate()
{
    if (g_pMoviePlayer == NULL || g_pMoviePlayer->m_hBink == NULL)
    {
        return -1.0;
    }
    return (double)(S32)g_pMoviePlayer->m_hBink->FrameRate /
           (double)(S32)g_pMoviePlayer->m_hBink->FrameRateDiv;
}
// PC 0x004051E0
int CExoMoviePlayerInternal::StartMovie(const CExoString& sMovie)
{
    static int bClassRegistered;
    WINDOWINFO wi;
    WNDCLASSA WndClass;
    HDIGDRIVER hDig;
    const char* pszMovie;
    int nMovieVolume;
    int nScaledWidth, nScaledHeight;
    HDC hDC;

    m_hMovieWnd = NULL;
    g_hMovieWnd = NULL;

    if (!bClassRegistered)
    {
        WndClass.style = 0;
        WndClass.lpfnWndProc = MovieMessageHandler;
        WndClass.cbClsExtra = 0;
        WndClass.cbWndExtra = 0;
        WndClass.hInstance = g_hInst;
        WndClass.hIcon = LoadIconA(g_hInst, (LPCSTR)0x65);
        WndClass.hCursor = LoadCursorA(NULL, (LPCSTR)0x7F00);
        WndClass.hbrBackground = NULL;
        WndClass.lpszMenuName = NULL;
        WndClass.lpszClassName = "SWMovieWindow";
        if (RegisterClassA(&WndClass) == 0)
        {
            LPSTR pszError;

            FormatMessageA(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL, GetLastError(), 0x400, (LPSTR)&pszError, 0, NULL);
            MessageBoxA(NULL, pszError, "Error", MB_OK | MB_ICONINFORMATION);
            LocalFree(pszError);
            return 0;
        }
        bClassRegistered = 1;
    }

    m_bMoviePlaying = 1;
    wi.cbSize = sizeof(wi);
    if (!GetWindowInfo(g_hRenderWnd, &wi))
    {
        return 0;
    }

    RECT rcClient = wi.rcClient;

    m_hMovieWnd = CreateWindowExA(
        0, "SWMovieWindow", "SW Movie Player Window", WS_POPUP | WS_VISIBLE,
        rcClient.left, rcClient.top, rcClient.right - rcClient.left,
        rcClient.bottom - rcClient.top, g_hRenderWnd, NULL, g_hInst,
        NULL);
    g_hMovieWnd = m_hMovieWnd;
    SetCapture(g_hMovieWnd);
    g_pAurora->FullScreen();
    SetWindowLongA(m_hMovieWnd, GWL_STYLE, WS_POPUP | WS_CLIPCHILDREN);
    SetWindowPos(m_hMovieWnd, HWND_TOPMOST, rcClient.left, rcClient.top, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
                 SWP_NOSIZE | SWP_FRAMECHANGED | SWP_NOCOPYBITS);

    m_hBink = NULL;
    m_nUnknown60 = 0;
    m_nUnknown64 = 0;

    pszMovie = strchr(sMovie.CStr(), '.');
    if (pszMovie == NULL)
    {
        m_sMovieFileName = sMovie;
        if (strchr(m_sMovieFileName.CStr(), ':') == NULL)
        {
            m_sMovieFileName.Format("MOVIES:%s", m_sMovieFileName.CStr());
        }
        m_sMovieFileName = g_pExoBase->m_pExoAliasList->ResolveFileName(
            m_sMovieFileName, RESTYPE_BIK);
    }
    else
    {
        m_sMovieFileName = sMovie;
    }

    AIL_quick_handles(&hDig, NULL, NULL);
    BinkSoundUseMiles(hDig);

    m_hBink = BinkOpen(m_sMovieFileName.CStr(), 0x8000000);
    if (m_hBink == NULL)
    {
        CExoString sMessage;

        sMessage = "CExoMoviePlayerInternal::CExoMoviePlayerInternal()  Can't "
                   "open movie: ";
        sMessage = sMessage + m_sMovieFileName;
        g_pExoBase->m_pExoDebug->Warning(1083, "exomovieplayerinternal.cpp",
                                         sMessage.CStr());
        ShutDown();
        m_bMoviePlaying = 0;
        return 0;
    }

    m_hBinkBuffer = NULL;
    m_hBinkBuffer = BinkBufferOpen(m_hMovieWnd, m_hBink->Width,
                                   m_hBink->Height, 0x5D000000);
    if (m_hBinkBuffer == NULL)
    {
        BinkClose(m_hBink);
        m_bMoviePlaying = 0;
        return 0;
    }

    nMovieVolume = g_pAppManager->GetClientExoApp()
                       ->GetClientOptions()
                       ->m_nMovieVolume;
    if (g_pAppManager->GetClientExoApp()->m_pcExoAppInternal
            ->m_bEvilEndingHackInProgress)
    {
        nMovieVolume = 0;
    }
    BinkSetVolume(m_hBink, 0, (nMovieVolume << 15) / 100);

    if ((rcClient.right - rcClient.left) != m_hBink->Width)
    {
        float fScale = (float)(rcClient.right - rcClient.left) / (float)m_hBink->Width;

        nScaledWidth = (int)(m_hBink->Width * fScale);
        nScaledHeight = (int)(m_hBink->Height * fScale);
        BinkBufferSetScale(m_hBinkBuffer, nScaledWidth, nScaledHeight);
        m_nVideoOffsetX = (rcClient.right - nScaledWidth - rcClient.left) / 2;
        m_nVideoOffsetY = (rcClient.bottom - nScaledHeight - rcClient.top) / 2;
    }
    else
    {
        m_nVideoOffsetX = (rcClient.right - m_hBink->Width - rcClient.left) / 2;
        m_nVideoOffsetY = (rcClient.bottom - m_hBink->Height - rcClient.top) / 2;
    }

    BinkBufferSetOffset(m_hBinkBuffer, m_nVideoOffsetX,
                        m_nVideoOffsetY);
    m_bMoviePlaying = 1;
    while (g_bSwappingBuffers == 1)
    {
        Sleep(1);
    }
    hDC = GetDC(g_hRenderWnd);
    PatBlt(hDC, 0, 0, g_nScreenWidth, g_nScreenHeight, BLACKNESS);
    ReleaseDC(g_hRenderWnd, hDC);
    ShowWindow(m_hMovieWnd, SW_SHOWNA);
    return 1;
}
// PC 0x00405BF0
DWORD WINAPI CExoMoviePlayerInternal::PlayMoviesAsync(LPVOID pParam)
{
    CExoMoviePlayerInternal* self;
    int i;

    self = (CExoMoviePlayerInternal*)pParam;
    g_pMoviePlayer = self;
    if (self == NULL)
    {
        ExitThread(0);
    }

    WaitForSingleObject(self->m_hWakeEvent, INFINITE);
    while (self->m_bExitRequested == 0)
    {
        for (int nMovie = 0; nMovie < self->m_lstMovies.num; nMovie++)
        {
            self->m_lstMovies[nMovie] != "";
        }
        self->m_bSkippable = 1;
        for (i = 0; i < self->m_lstMovies.num; i++)
        {
            CExoString* pMovie = &self->m_lstMovies[i];
            if (*pMovie != "")
            {
                if (self->m_lstSkippable.num > 0)
                {
                    if (i < self->m_lstSkippable.num)
                        self->m_bSkippable = self->m_lstSkippable[i];
                    else
                        self->m_bSkippable = 1;
                }
                if (self->StartMovie(*pMovie) == 1)
                {
                    self->RunMovieLoop(1);
                    Sleep(5);
                    if (self->m_nUnknown1C == 0)
                        self->m_lstMovies[i] = "";
                    self->FinishMovie();
                }
                if (self->m_bSkippable != 0)
                {
                    if (self->m_bCancelMovie == 1)
                    {
                        if (self->m_nCancelResult != 0)
                            break;
                        self->m_bCancelMovie = 0;
                    }
                }
                else
                    self->m_bCancelMovie = 0;
            }
        }
        if (self->m_nUnknown1C == 0)
            self->m_lstMovies.SetSize(0);
        self->m_bCancelMovie = 0;
        self->m_nCancelResult = 0;
        self->m_bMoviePlaying = 0;
        if (self->m_nUnknown1C == 1)
        {
            g_bMovieResolutionLocked = 1;
            if (self->m_nUnknown20 == 1)
            {
                if (g_pAurora->FullScreen() == 1)
                    PostMessageA(g_hRenderWnd, WM_MOVIE_MODE_ACTIVATE, 0, 0);
                else
                    PostMessageA(g_hRenderWnd, WM_MOVIE_MODE_ACTIVATE, 1, 0);
            }
            else
                PostMessageA(g_hRenderWnd, WM_ACTIVATEAPP, 0, 0);
            self->m_nUnknown20 = 0;
            self->m_nUnknown1C = 0;
        }
        else
        {
            g_bMovieResolutionLocked = 0;
            PostMessageA(g_hRenderWnd, WM_MOVIE_RESTORE_RESOLUTION, 0, 0);
        }
        WaitForSingleObject(self->m_hWakeEvent, INFINITE);
    }
    ExitThread(0);
}
// PC 0x00405F10
int CExoMoviePlayerInternal::PlayMoviesAsync(const CExoArrayList<CExoString>& lstMovies,
                                        int nMode,
                                        const CExoArrayList<int>* pSkippable)
{
    SetThreadPriority(m_hWorkerThread, THREAD_PRIORITY_NORMAL);
    m_lstMovies = lstMovies;
    m_nPlaybackMode = nMode;
    if (pSkippable != NULL)
    {
        m_lstSkippable = *pSkippable;
        if (m_lstMovies.num > m_lstSkippable.num)
        {
            g_pExoBase->m_pExoDebug->Warning(1619, "exomovieplayerinternal.cpp",
                "CExoMoviePlayerInternal::PlayMoviesAsync(): More movies specified than skippable flags.");
        }
    }
    else
    {
        m_lstSkippable.RemoveAll(0);
    }
    m_bMoviePlaying = 1;
    return SetEvent(m_hWakeEvent);
}

void MovieDumpQueue(const char* szFileName)
{
    if (g_pMoviePlayer == NULL)
    {
        return;
    }
    CExoArrayList<CExoString> lstMovies;
    g_pMoviePlayer->m_critSect.EnterCriticalSection();
    lstMovies = g_pMoviePlayer->m_lstMovies;
    g_pMoviePlayer->m_critSect.LeaveCriticalSection();
    FILE* pFile = fopen(szFileName, "w");
    if (pFile == NULL)
    {
        return;
    }
    for (int i = 0; i < lstMovies.num; i++)
    {
        fprintf(pFile, "%s", lstMovies[i].CStr());
        fprintf(pFile, "\n");
    }
    fclose(pFile);
}
