#include <xtl.h>
#include <xmv.h>
#include "bink.h"

#include "appmanager.h"
#include "clientexoapp.h"
#include "clientoptions.h"
#include "swguifade.h"
#include "exobase.h"

#include "exomovieplayerinternal.h"
#include "swmain.h"

extern int g_bMovieNotification;

#define MOVIE_BINK_VOLUME_FULL 32768
#define MOVIE_SOUND_TRACKS 4

inline void CExoMoviePlayerInternal::ShutDown()
{
    if (m_hBink != NULL)
    {
        BinkSetVolume(m_hBink, 0, 0);
        BinkSetVolume(m_hBink, 1, 0);
        BinkSetSoundOnOff(m_hBink, 0);
        BinkClose(m_hBink);
        m_hBink = NULL;
    }
}

inline void CExoMoviePlayerInternal::FinishMovie()
{
    m_critSect.EnterCriticalSection();
    BinkSetVolume(m_hBink, 0, 0);
    BinkClose(m_hBink);
    m_hBink = NULL;
    m_nFinishTime = g_pExoBase->m_pExoTimers->GetLowResolutionTimer();
    m_bMoviePlaying = 0;
    m_critSect.LeaveCriticalSection();
}
// Xbox 0x0033A710
int CExoMoviePlayerInternal::IsMoviePlaying()
{
    return m_bMoviePlaying;
}
// Xbox 0x0033A720
void CExoMoviePlayerInternal::CancelMovie()
{
    m_bCancelMovie = 1;
}
// Xbox 0x0033A730
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
// Xbox 0x0033A770
int CExoMoviePlayerInternal::StartMovie(const CExoString& sMovie)
{
    if (m_hBink != NULL)
    {
        BinkClose(m_hBink);
        m_hBink = NULL;
    }
    if (BinkSoundUseDirectSound(g_pExoSound->GetEAX()))
    {
        U32 aTrackIDs[MOVIE_SOUND_TRACKS];
        U32 aMixBins[2];

        aTrackIDs[0] = 0;
        aTrackIDs[1] = 1;
        aTrackIDs[2] = 2;
        aTrackIDs[3] = 3;
        BinkSetSoundTrack(MOVIE_SOUND_TRACKS, aTrackIDs);
        m_hBink = BinkOpen(sMovie.CStr(), BINKNOTHREADEDIO);
        aMixBins[0] = DSMIXBIN_FRONT_LEFT;
        aMixBins[1] = DSMIXBIN_FRONT_RIGHT;
        BinkSetMixBins(m_hBink, 0, aMixBins, 2);
        aMixBins[0] = DSMIXBIN_FRONT_CENTER;
        BinkSetMixBins(m_hBink, 1, aMixBins, 1);
        aMixBins[0] = DSMIXBIN_LOW_FREQUENCY;
        BinkSetMixBins(m_hBink, 2, aMixBins, 1);
        aMixBins[0] = DSMIXBIN_BACK_LEFT;
        aMixBins[1] = DSMIXBIN_BACK_RIGHT;
        BinkSetMixBins(m_hBink, 3, aMixBins, 2);
    }
    if (m_hBink == NULL)
    {
        CExoString sMessage;

        sMessage = "CExoMoviePlayerInternal::CExoMoviePlayerInternal()  Can't open movie: ";
        sMessage = sMessage + sMovie;
        ShutDown();
        m_bMoviePlaying = 0;
        return 0;
    }

    m_bCancelMovie = 0;
    D3DDevice::CreateTexture(m_hBink->Width, m_hBink->Height, 1, 0, D3DFMT_YUY2, D3DPOOL_DEFAULT,
                             &m_aFrameBuffers[0].m_pTexture);
    D3DDevice::CreateTexture(m_hBink->Width, m_hBink->Height, 1, 0, D3DFMT_YUY2, D3DPOOL_DEFAULT,
                             &m_aFrameBuffers[1].m_pTexture);
    if (m_aFrameBuffers[0].m_pTexture != NULL && m_aFrameBuffers[1].m_pTexture != NULL)
    {
        D3DDevice::EnableOverlay(TRUE);
        m_aFrameBuffers[0].m_pTexture->GetSurfaceLevel(0, &m_aFrameBuffers[0].m_pSurface);
        m_aFrameBuffers[1].m_pTexture->GetSurfaceLevel(0, &m_aFrameBuffers[1].m_pSurface);
    }
    BinkSetVolume(m_hBink, 0,
                  (g_pAppManager->GetClientExoApp()->GetClientOptions()->m_nMovieVolume *
                   MOVIE_BINK_VOLUME_FULL) / 100);
    return 1;
}
// Xbox 0x0033A990
void CExoMoviePlayerInternal::RenderFrame(int nWait)
{
    D3DSurface* pBackBuffer;
    D3DSURFACE_DESC desc;
    D3DLOCKED_RECT lockedRect;
    RECT rectSource;
    RECT rectDest;
    unsigned long nCenterX;
    unsigned long nCenterY;
    int nBuffer;

    if (m_hBink == NULL)
    {
        return;
    }
    nBuffer = 0;
    D3DDevice::GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
    pBackBuffer->GetDesc(&desc);
    nCenterX = desc.Width / 2;
    nCenterY = desc.Height / 2;
    while (m_hBink->Frames != m_hBink->FrameNum && m_bMoviePlaying)
    {
        BinkDoFrame(m_hBink);
        nBuffer ^= 1;
        m_aFrameBuffers[nBuffer].m_pTexture->LockRect(0, &lockedRect, NULL, 0);
        BinkCopyToBuffer(m_hBink, lockedRect.pBits, lockedRect.Pitch, m_hBink->Height, 0, 0,
                         BINKSURFACEYUY2 | BINKCOPYALL);
        rectSource.left = 0;
        rectSource.top = 0;
        rectSource.right = m_hBink->Width;
        rectSource.bottom = m_hBink->Height;
        RECT rectFrame = {nCenterX - m_hBink->Width / 2, nCenterY - m_hBink->Height / 2,
                          m_hBink->Width / 2 + nCenterX, m_hBink->Height / 2 + nCenterY};
        rectDest = rectFrame;
        D3DDevice::UpdateOverlay(m_aFrameBuffers[nBuffer].m_pSurface, &rectSource, &rectDest, FALSE, 0);
        BinkNextFrame(m_hBink);
        while (BinkWait(m_hBink) == 1)
        {
            Sleep(1);
        }
        if (m_bCancelMovie == 1)
        {
            break;
        }
    }
    D3DDevice::BlockUntilVerticalBlank();
    D3DDevice::EnableOverlay(FALSE);
    m_aFrameBuffers[0].m_pSurface->Release();
    m_aFrameBuffers[1].m_pSurface->Release();
    if (m_aFrameBuffers[0].m_pTexture != NULL)
    {
        m_aFrameBuffers[0].m_pTexture->Release();
        m_aFrameBuffers[0].m_pTexture = NULL;
    }
    if (m_aFrameBuffers[1].m_pTexture != NULL)
    {
        m_aFrameBuffers[1].m_pTexture->Release();
        m_aFrameBuffers[1].m_pTexture = NULL;
    }
}
// Xbox 0x0033AB20
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
// Xbox 0x0033ABE0
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
// Xbox 0x0033AC50
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
                self->RenderFrame(1);
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
// Xbox 0x0033AD20
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
    m_nCurrentMovie = 0;
    m_bCancelMovie = 0;
    m_hBink = NULL;
    m_pFadePanel = NULL;
    m_nFinishTime = -1;
}
// Xbox 0x0033ADA0
CExoMoviePlayerInternal::~CExoMoviePlayerInternal()
{
    DWORD dwExitCode;

    ShutDown();
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
// Xbox 0x0033AE50
int CExoMoviePlayerInternal::PlayMoviesAsync(const CExoArrayList<CExoString>& lstMovies,
                                             int nMode)
{
    m_lstMovies = lstMovies;
    m_nPlaybackMode = nMode;
    m_bMoviePlaying = 1;
    return SetEvent(m_hWakeEvent);
}
// Xbox 0x001500F0
int CExoMoviePlayerInternal::Initialize(void* pParam)
{
    return 1;
}
