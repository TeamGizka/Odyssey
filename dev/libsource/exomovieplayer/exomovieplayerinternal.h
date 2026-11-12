#ifndef _EXOMOVIEPLAYERINTERNAL_H_
#define _EXOMOVIEPLAYERINTERNAL_H_

#ifdef _XBOX
#include <xtl.h>
#include <xmv.h>
#else
#include <windows.h>
#include "bink.h"
#endif
#include "exobase.h"

class CSWGuiFade;

#ifdef _XBOX

class CExoMoviePlayerInternal
{
public:
    CExoMoviePlayerInternal();
    ~CExoMoviePlayerInternal();

    int Initialize(void* pParam);
    void CancelMovie();
    int IsMoviePlaying();
    int StartMovie(const CExoString& sMovie);
    int PlayMoviesAsync(const CExoArrayList<CExoString>& lstMovies, int nMode);
    void ShowFadePanel();
    void HideFadePanel(int bWait);
    void SetFadeMsg(unsigned long nStrRef);

private:
    static DWORD WINAPI PlayMoviesAsync(LPVOID pParam);
    void RunMovieLoop(int nWait);
    void FinishMovie();

public:

    enum { FADE_PANEL_HOLD_TIME = 4000 };

    int m_bExitRequested;
    HANDLE m_hWakeEvent;
    CExoArrayList<CExoString> m_lstMovies;
    int m_bCancelMovie;
    int m_nPlaybackMode;
    XMVDecoder* m_pDecoder;
    XMVVIDEO_DESC m_VideoDesc;
    CExoString m_sCurrentMovie;
    int m_bMoviePlaying;
    int m_nUnknown3C;
    HANDLE m_hWorkerThread;
    unsigned long m_dwWorkerThreadId;
    CExoCriticalSection m_critSect;
    CSWGuiFade* m_pFadePanel;
    unsigned long m_nFinishTime;
};
#else
class CExoMoviePlayerInternal
{
public:
    CExoMoviePlayerInternal();
    ~CExoMoviePlayerInternal();

    int ShutDown();
    void OnDeactivation(int bToggleFullScreen);
    int CancelMovie(int nA, int nB);
    int IsMoviePlaying();
    int StartMovie(const CExoString& sMovie);
    int PlayMoviesAsync(const CExoArrayList<CExoString>& lstMovies, int nA,
                    const CExoArrayList<int>* lstSkippable);
    void ShowFadePanel();
    void HideFadePanel();

    void OnWindowPosChanging(WINDOWPOS* pWindowPos);
    void OnWindowPosChanged(WINDOWPOS* pWindowPos);
    int GetNumberOfMoviesToPlay();
    int RestartMoviesAsync();
    void PauseMovie();
    void ResumeMovie();

    void AddMovie(const CExoString& sMovie) { m_lstMovies.Add(sMovie); }

private:
    static DWORD WINAPI PlayMoviesAsync(LPVOID pParam);

    int RunMovieLoop(int nWait);
    void FinishMovie();

public:
    int m_bExitRequested;
    HANDLE m_hWakeEvent;
    CExoArrayList<CExoString> m_lstMovies;
    int m_bCancelMovie;
    int m_nCancelResult;
    int m_nUnknown1C;
    int m_nUnknown20;
    int m_bShuttingDown;
    CExoString m_sCurrentMovie;
    int m_bMoviePlaying;
    int m_nUnknown34;
    HANDLE m_hWorkerThread;
    unsigned long m_dwWorkerThreadId;
    CExoCriticalSection m_critSect;
    CSWGuiFade* m_pFadePanel;
    HBINK m_hBink;
    HBINKBUFFER m_hBinkBuffer;
    HWND m_hMovieWnd;
    char m_aUnmapped54[12];
    int m_nUnknown60;
    int m_bPaused;
    CExoString m_sMovieFileName;
    int m_nUnknown70;
    char m_aUnmapped74[16];
    int m_nVideoOffsetX;
    int m_nVideoOffsetY;
    int m_nPlaybackMode;
    CExoArrayList<int> m_lstSkippable;
    int m_bSkippable;
};
#endif

#endif
