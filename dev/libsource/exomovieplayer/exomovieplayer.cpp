#include "exobase.h"

#include "exomovieplayerinternal.h"
// PC 0x0077F8F0
CExoMoviePlayer::CExoMoviePlayer()
{
    m_pInternal = new CExoMoviePlayerInternal();
}

#ifdef _XBOX
int CExoMoviePlayer::Initialize(void* pParam)
{
    return m_pInternal->Initialize(pParam);
}

void CExoMoviePlayer::CancelMovie()
{
    m_pInternal->CancelMovie();
}
#else
int CExoMoviePlayer::Initialize(void* pParam)  // PC 0x006955A0
{
    return 0;
}
// PC 0x0077F960
int CExoMoviePlayer::CancelMovie(int nA, int nB)
{
    return m_pInternal->CancelMovie(nA, nB);
}
#endif
// PC 0x0077F970
int CExoMoviePlayer::IsMoviePlaying()
{
    return m_pInternal->IsMoviePlaying();
}

#ifdef _XBOX
int CExoMoviePlayer::PlayMoviesAsync(const CExoArrayList<CExoString>& lstMovies,
                                     int nA)
{
    return m_pInternal->PlayMoviesAsync(lstMovies, nA);
}

void CExoMoviePlayer::ShowFadePanel()
{
    m_pInternal->ShowFadePanel();
}

void CExoMoviePlayer::HideFadePanel(int bWait)
{
    m_pInternal->HideFadePanel(bWait);
}

void CExoMoviePlayer::SetFadeMsg(unsigned long nStrRef)
{
    m_pInternal->SetFadeMsg(nStrRef);
}
#else
int CExoMoviePlayer::PlayMoviesAsync(const CExoArrayList<CExoString>& lstMovies,  // PC 0x0077F980
                                     int nA,
                                     const CExoArrayList<int>* lstSkippable)
{
    return m_pInternal->PlayMoviesAsync(lstMovies, nA, lstSkippable);
}
// PC 0x0077F990
void CExoMoviePlayer::ShowFadePanel()
{
    m_pInternal->ShowFadePanel();
}
// PC 0x0077F9A0
void CExoMoviePlayer::HideFadePanel()
{
    m_pInternal->HideFadePanel();
}
// PC 0x0077F9B0
int CExoMoviePlayer::GetNumberOfMoviesToPlay()
{
    if (m_pInternal != 0)
    {
        return m_pInternal->GetNumberOfMoviesToPlay();
    }
    return 0;
}
// PC 0x0077F9C0
int CExoMoviePlayer::RestartMoviesAsync()
{
    return m_pInternal->RestartMoviesAsync();
}
#endif
// PC 0x0077F9D0
CExoMoviePlayer::~CExoMoviePlayer()
{
    if (m_pInternal != 0)
    {
        delete m_pInternal;
        m_pInternal = 0;
    }
}
