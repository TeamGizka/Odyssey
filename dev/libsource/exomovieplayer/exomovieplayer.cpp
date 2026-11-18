#include "exobase.h"

#include "exomovieplayerinternal.h"
// PC 0x0070C170  Xbox 0x00303DB0
CExoMoviePlayer::CExoMoviePlayer()
{
    m_pInternal = new CExoMoviePlayerInternal();
}

#ifdef _XBOX
int CExoMoviePlayer::Initialize(void* pParam)  // PC 0x0063E7C0  Xbox 0x00303E20
{
    return m_pInternal->Initialize(pParam);
}
// Xbox 0x00303E30
void CExoMoviePlayer::CancelMovie()
{
    m_pInternal->CancelMovie();
}
#else
int CExoMoviePlayer::Initialize(void* pParam)  // PC 0x0063E7C0  Xbox 0x00303E20
{
    return 0;
}
// PC 0x0070C1E0
int CExoMoviePlayer::CancelMovie(int nResult, int bForce)
{
    return m_pInternal->CancelMovie(nResult, bForce);
}
#endif
// PC 0x0070C1F0  Xbox 0x00303E40
int CExoMoviePlayer::IsMoviePlaying()
{
    return m_pInternal->IsMoviePlaying();
}

#ifdef _XBOX
int CExoMoviePlayer::PlayMoviesAsync(const CExoArrayList<CExoString>& lstMovies,  // Xbox 0x00303E50
                                     int nMode)
{
    return m_pInternal->PlayMoviesAsync(lstMovies, nMode);
}
// PC 0x0070C210  Xbox 0x00303E60
void CExoMoviePlayer::ShowFadePanel()
{
    m_pInternal->ShowFadePanel();
}
// Xbox 0x00303E70
void CExoMoviePlayer::HideFadePanel(int bWait)
{
    m_pInternal->HideFadePanel(bWait);
}
// Xbox 0x00303E80
void CExoMoviePlayer::SetFadeMsg(unsigned long nStrRef)
{
    m_pInternal->SetFadeMsg(nStrRef);
}
#else
int CExoMoviePlayer::PlayMoviesAsync(const CExoArrayList<CExoString>& lstMovies,  // PC 0x0070C200
                                     int nMode,
                                     const CExoArrayList<int>* lstSkippable)
{
    return m_pInternal->PlayMoviesAsync(lstMovies, nMode, lstSkippable);
}
// PC 0x0070C210  Xbox 0x00303E60
void CExoMoviePlayer::ShowFadePanel()
{
    m_pInternal->ShowFadePanel();
}
// PC 0x0070C220
void CExoMoviePlayer::HideFadePanel()
{
    m_pInternal->HideFadePanel();
}
// PC 0x0070C230
int CExoMoviePlayer::GetNumberOfMoviesToPlay()
{
    if (m_pInternal != 0)
    {
        return m_pInternal->GetNumberOfMoviesToPlay();
    }
    return 0;
}
// PC 0x0070C240
int CExoMoviePlayer::RestartMoviesAsync()
{
    return m_pInternal->RestartMoviesAsync();
}
#endif
// PC 0x0070C250  Xbox 0x00303E90
CExoMoviePlayer::~CExoMoviePlayer()
{
    if (m_pInternal != 0)
    {
        delete m_pInternal;
        m_pInternal = 0;
    }
}
