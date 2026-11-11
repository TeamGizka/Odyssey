#include <xtl.h>
#include <xmv.h>

#include "appmanager.h"
#include "clientexoapp.h"
#include "clientoptions.h"
#include "swguifade.h"
#include "exobase.h"

#include "exomovieplayerinternal.h"
#include "swmain.h"

extern int g_bMovieNotification;

unsigned char ExoSoundMillibelsToVolume(long nMillibels, long nMinVolume);
long ExoSoundVolumeToMillibels(unsigned char nVolume, long nMinVolume);

#define MOVIE_SCREEN_WIDTH 640
#define MOVIE_SCREEN_HEIGHT 480
#define MOVIE_VOLUME_MIN -6400
#define MOVIE_VOLUME_SURROUND -1204
#define MOVIE_MIXBIN_VOLUME(nDecibels)     (m_nPlaybackMode ? MOVIE_VOLUME_MIN :      ExoSoundVolumeToMillibels((unsigned char)(ExoSoundMillibelsToVolume(nDecibels, MOVIE_VOLUME_MIN) * fVolume),                               MOVIE_VOLUME_MIN))

int CExoMoviePlayerInternal::StartMovie(const CExoString& sMovie)
{
    if (!sMovie.IsEmpty())
    {
        m_sCurrentMovie = sMovie;
        if (FAILED(XMVDecoder_CreateDecoderForFile(0, m_sCurrentMovie.CStr(), &m_pDecoder)))
        {
            m_sCurrentMovie = "";
            return 0;
        }
        m_bMoviePlaying = 1;
        return 1;
    }
    return 0;
}

void CExoMoviePlayerInternal::SetFadeMsg(unsigned long nStrRef)
{
    if (m_pFadePanel != NULL && g_bMovieNotification != 0)
    {
        m_pFadePanel->SetMsg(nStrRef);
        if (nStrRef != 0)
        {
            UpdateScreen(1.0f / 30.0f, 0, 0);
        }
    }
}

void CExoMoviePlayerInternal::FinishMovie()
{
    m_critSect.EnterCriticalSection();
    if (m_pDecoder != NULL)
    {
        delete m_pDecoder;
        m_pDecoder = NULL;
    }
    m_nFinishTime = g_pExoBase->m_pExoTimers->GetLowResolutionTimer();
    m_bMoviePlaying = 0;
    m_critSect.LeaveCriticalSection();
}

void CExoMoviePlayerInternal::RunMovieLoop(int nWait)
{
    RECT rect;
    DSMIXBINS mixBins;
    DSMIXBINVOLUMEPAIR aPairs[6];

    if (m_pDecoder == NULL)
    {
        return;
    }
    XMVDecoder_GetVideoDescriptor(m_pDecoder, &m_VideoDesc);
    rect.left = (MOVIE_SCREEN_WIDTH - m_VideoDesc.Width) / 2;
    rect.right = MOVIE_SCREEN_WIDTH - rect.left;
    rect.top = (MOVIE_SCREEN_HEIGHT - m_VideoDesc.Height) / 2;
    rect.bottom = MOVIE_SCREEN_HEIGHT - rect.top;
    if (m_VideoDesc.AudioStreamCount != 0)
    {
        float fVolume = g_pAppManager->GetClientExoApp()->GetClientOptions()->m_nMovieVolume * 0.01f;

        long nVolume;

        mixBins.dwMixBinCount = 6;
        mixBins.lpMixBinVolumePairs = aPairs;
        aPairs[0].dwMixBin = DSMIXBIN_FRONT_LEFT;
        nVolume = MOVIE_MIXBIN_VOLUME(0);
        aPairs[0].lVolume = nVolume;
        aPairs[1].lVolume = nVolume;
        aPairs[1].dwMixBin = DSMIXBIN_FRONT_RIGHT;
        aPairs[3].dwMixBin = DSMIXBIN_LOW_FREQUENCY;
        nVolume = MOVIE_MIXBIN_VOLUME(0);
        aPairs[3].lVolume = nVolume;
        aPairs[4].dwMixBin = DSMIXBIN_BACK_LEFT;
        nVolume = MOVIE_MIXBIN_VOLUME(MOVIE_VOLUME_SURROUND);
        aPairs[4].lVolume = nVolume;
        aPairs[5].lVolume = nVolume;
        aPairs[2].lVolume = nVolume;
        aPairs[5].dwMixBin = DSMIXBIN_BACK_RIGHT;
        aPairs[2].dwMixBin = DSMIXBIN_FRONT_CENTER;
        XMVDecoder_EnableAudioStream(m_pDecoder, 0, 0, &mixBins, NULL);
    }
    XMVDecoder_Play(m_pDecoder, 0, &rect);
    XMVDecoder_CloseDecoder(m_pDecoder);
    m_pDecoder = NULL;
    m_sCurrentMovie = "";
}

void CExoMoviePlayerInternal::CancelMovie()
{
    if (m_pDecoder != NULL)
    {
        XMVDecoder_TerminatePlayback(m_pDecoder);
        m_bCancelMovie = 1;
    }
}

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
    UpdateScreen(1.0f / 30.0f, 0, 0);
    m_critSect.LeaveCriticalSection();
}

void CExoMoviePlayerInternal::HideFadePanel(int bWait)
{
    m_critSect.EnterCriticalSection();
    unsigned long nTime = g_pExoBase->m_pExoTimers->GetLowResolutionTimer();
    if (!bWait || (m_nFinishTime > -1 && nTime > m_nFinishTime + FADE_PANEL_HOLD_TIME))
    {
        if (m_pFadePanel != NULL)
        {
            m_pFadePanel->SetAlpha(0.0f);
            g_pAppManager->GetClientExoApp()->GetSWGuiManager()->RemovePanel(m_pFadePanel);
        }
        m_nFinishTime = -1;
    }
    m_critSect.LeaveCriticalSection();
}

DWORD WINAPI CExoMoviePlayerInternal::PlayMoviesAsync(LPVOID pParam)
{
    CExoMoviePlayerInternal* self;
    int i;

    self = (CExoMoviePlayerInternal*)pParam;
    if (self == NULL)
    {
        ExitThread(0);
    }

    WaitForSingleObject(self->m_hWakeEvent, INFINITE);
    while (self->m_bExitRequested == 0)
    {
        for (i = 0; i < self->m_lstMovies.num; i++)
        {
            CExoString* pMovie = &self->m_lstMovies[i];
            if (*pMovie != "" && self->StartMovie(*pMovie))
            {
                self->RunMovieLoop(1);
                Sleep(5);
                if (self->m_bCancelMovie)
                {
                    break;
                }
            }
        }
        self->FinishMovie();
        WaitForSingleObject(self->m_hWakeEvent, INFINITE);
    }
    ExitThread(0);
}

CExoMoviePlayerInternal::CExoMoviePlayerInternal()
    : m_lstMovies(0)
{
    m_pDecoder = NULL;
    m_sCurrentMovie = "";
    m_bMoviePlaying = 0;
    m_bExitRequested = 0;
    m_hWakeEvent = CreateEventA(NULL, FALSE, FALSE, NULL);
    m_hWorkerThread = CreateThread(
        NULL, 0x8000,
        (LPTHREAD_START_ROUTINE)CExoMoviePlayerInternal::PlayMoviesAsync, this,
        0, (LPDWORD)&m_dwWorkerThreadId);
    m_nUnknown3C = 0;
    m_bCancelMovie = 0;
    m_pFadePanel = NULL;
    m_nFinishTime = -1;
}

CExoMoviePlayerInternal::~CExoMoviePlayerInternal()
{
    DWORD dwExitCode;

    m_bExitRequested = 1;
    SetEvent(m_hWakeEvent);
    do
    {
        SwitchToThread();
        GetExitCodeThread(m_hWorkerThread, &dwExitCode);
    } while (dwExitCode == STILL_ACTIVE);
    CloseHandle(m_hWorkerThread);
    CloseHandle(m_hWakeEvent);

    if (m_pFadePanel != NULL)
    {
        delete m_pFadePanel;
        m_pFadePanel = NULL;
    }
}

int CExoMoviePlayerInternal::PlayMoviesAsync(const CExoArrayList<CExoString>& lstMovies,
                                             int nMode)
{
    m_lstMovies = lstMovies;
    m_nPlaybackMode = nMode;
    m_bMoviePlaying = 1;
    return SetEvent(m_hWakeEvent);
}

int CExoMoviePlayerInternal::IsMoviePlaying()
{
    return m_bMoviePlaying;
}

int CExoMoviePlayerInternal::Initialize(void* pParam)
{
    return 1;
}
