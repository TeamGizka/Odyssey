#define MATH3D_NO_INLINE
#if defined(_WIN32) && !defined(_XBOX)
#include <windows.h>
#else
#include "platform.h"
#endif
#include "exobase.h"
#include "swmain.h"

const unsigned char CExoSound::CMAX_VOLUME = 127;
const int CExoSound::CEXOSOUND_MODE_GENERIC = 0;
const int CExoSound::CEXOSOUND_MODE_MUTE = 1;
const int CExoSound::CEXOSOUND_MODE_PAUSE = 2;
const int CExoSound::CEXOSOUND_MODE_PAUSE_ALL = 3;
const int CExoSound::CEXOSOUND_MODE_PAUSE_ALL_BUT_MUSIC = 4;
const int CExoSound::CEXOSOUND_MODE_LOSTFOCUS = 5;
const int CExoSound::CEXOSOUND_MODE_GAINFOCUS = 6;

#include "exosoundinternal.h"

#define EXOSOUND_DEBUG 0
// PC 0x006214C0 GOG 0x00621490 Xbox 0x001656C0
CExoSoundSource::CExoSoundSource()
{
    if (g_bDisableSound != 0)
    {
        m_pInternal = NULL;
    }
    else
    {
        m_pInternal = new CExoSoundSourceInternal(g_pExoSound->m_pInternal);
    }
}
// PC 0x00621550 GOG 0x00621520 Xbox 0x00165710
void CExoSoundSource::SetPriorityGroup(unsigned char nGroup)
{
    CExoSoundSourceInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->SetPriorityGroup(nGroup);
    }
}
// PC 0x00621560 GOG 0x00621530 Xbox 0x00165720
void CExoSoundSource::Set3D(int b3D)
{
    CExoSoundSourceInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->Set3D(b3D);
    }
}
// PC 0x00621570 GOG 0x00621540 Xbox 0x00165730
int CExoSoundSource::IsHardwarePlayingSoundPleaseDontUseThis()
{
    CExoSoundSourceInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        return pInternal->IsPlaying();
    }
    else
    {
        return 0;
    }
}
// PC 0x00621580 GOG 0x00621550 Xbox 0x00165740
int CExoSoundSource::Play()
{
    CExoSoundSourceInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        return pInternal->Play(0, 0);
    }
    else
    {
        return 0;
    }
}
// PC 0x006215A0 GOG 0x00621570 Xbox 0x00165760
void CExoSoundSource::SetVolume(unsigned char nVolume)
{
    CExoSoundSourceInternal* pInternal;

    if (nVolume > CExoSound::CMAX_VOLUME)
    {
        nVolume = CExoSound::CMAX_VOLUME;
    }
    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->SetVolume(nVolume, 0, 1.0f);
    }
}
// PC 0x006215D0 GOG 0x006215A0 Xbox 0x00165790
void CExoSoundSource::SetPitchVariance(float fVariance)
{
    CExoSoundSourceInternal* pInternal;

    if (fVariance > 1.0f)
    {
        fVariance = 1.0f;
    }
    else if (fVariance < 0.0f)
    {
        fVariance = 0.0f;
    }
    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->SetPitchVariance(fVariance);
    }
}
// PC 0x00621620 GOG 0x006215F0 Xbox 0x001657C0
void CExoSoundSource::SetLooping(int bLooping)
{
    CExoSoundSourceInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->SetLooping(bLooping);
    }
}
// PC 0x00621630 GOG 0x00621600 Xbox 0x001657D0
void CExoSoundSource::SetPosition(const Vector& vPosition, float fRadius)
{
    CExoSoundSourceInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {

        Vector vSoundPosition = vPosition;
        vSoundPosition.m_fZ += fRadius;
        pInternal->SetPosition(vSoundPosition);
    }
}
// PC 0x00621670 GOG 0x00621640 Xbox 0x00165820
void CExoSoundSource::Stop()
{
    CExoSoundSourceInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->Stop();
    }
}
// PC 0x00621680 GOG 0x00621650 Xbox 0x00165830
int CExoSoundSource::SetFixedVariance(float fVariance)
{
    CExoSoundSourceInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        return pInternal->SetFixedVariance(fVariance);
    }
    else
    {
        return 0;
    }
}
// PC 0x006216A0 GOG 0x00621670 Xbox 0x00165850
CExoStreamingSoundSource::CExoStreamingSoundSource(int bReservedStream)
{
    if (g_bDisableSound != 0)
    {
        m_pInternal = NULL;
    }
    else
    {
        m_pInternal = new CExoStreamingSoundSourceInternal(
            g_pExoSound->m_pInternal, bReservedStream);
    }
}
// PC 0x00621730 GOG 0x00621700 Xbox 0x001658A0
int CExoStreamingSoundSource::IsPlaying()
{
    CExoStreamingSoundSourceInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        return pInternal->IsPlaying();
    }
    else
    {
        return 0;
    }
}
// PC 0x00621740 GOG 0x00621710 Xbox 0x001658B0
int CExoStreamingSoundSource::IsDone()
{
    CExoStreamingSoundSourceInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        return pInternal->IsDone();
    }
    else
    {
        return 0;
    }
}
// PC 0x00621750 GOG 0x00621720
int CExoStreamingSoundSource::GetPlayFailed()
{
    CExoStreamingSoundSourceInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        return pInternal->GetPlayFailed();
    }
    else
    {
        return 0;
    }
}
// PC 0x00621760 GOG 0x00621730 Xbox 0x001658C0
int CExoStreamingSoundSource::Play(CResRef const& pResRef,
                                   unsigned short nResourceType, int bLooping,
                                   int b3D, int bReverb)
{
    CExoStreamingSoundSourceInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        return pInternal->Play(pResRef, nResourceType, bLooping, b3D, bReverb);
    }
    else
    {
        return 0;
    }
}
// PC 0x00621770 GOG 0x00621740 Xbox 0x001658D0
void CExoStreamingSoundSource::Pause()
{
    CExoStreamingSoundSourceInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->Pause();
    }
}
// PC 0x00621780 GOG 0x00621750 Xbox 0x001658E0
void CExoStreamingSoundSource::Resume()
{
    CExoStreamingSoundSourceInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->Resume();
    }
}
// PC 0x00621790 GOG 0x00621760 Xbox 0x001658F0
void CExoStreamingSoundSource::SetLooping(int bLooping)
{
    CExoStreamingSoundSourceInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->SetLooping(bLooping);
    }
}
// PC 0x006217A0 GOG 0x00621770 Xbox 0x00165900
void CExoStreamingSoundSource::SetVolume(unsigned char nVolume)
{
    CExoStreamingSoundSourceInternal* pInternal;

    if (nVolume > CExoSound::CMAX_VOLUME)
    {
        nVolume = CExoSound::CMAX_VOLUME;
    }
    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->SetVolume(nVolume, 0, 1.0f);
    }
}

// Dead code: not in the PC executable
unsigned char CExoStreamingSoundSource::GetVolume()
{
    CExoStreamingSoundSourceInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        return pInternal->GetVolume();
    }
    return 0;
}
// PC 0x006217D0 GOG 0x006217A0 Xbox 0x00165930
void CExoStreamingSoundSource::Stop(int bCloseStream)
{
    CExoStreamingSoundSourceInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->Stop(bCloseStream);
    }
}
// PC 0x006217E0 GOG 0x006217B0 Xbox 0x00165940
void CExoStreamingSoundSource::FadeAndStop(unsigned long nFadeTime)
{
    CExoStreamingSoundSourceInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->FadeAndStop(nFadeTime);
    }
}
// PC 0x006217F0 GOG 0x006217C0 Xbox 0x00165950
void CExoStreamingSoundSource::SetPriorityGroup(unsigned char nGroup)
{
    CExoStreamingSoundSourceInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->SetPriorityGroup(nGroup);
    }
}

// Dead code: not in the PC executable
unsigned char CExoStreamingSoundSource::GetPriorityGroup()
{
    CExoStreamingSoundSourceInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        return pInternal->GetPriorityGroup();
    }
    return 23;
}
// PC 0x00621800 GOG 0x006217D0 Xbox 0x00165990
void CExoStreamingSoundSource::SetReverbEnabled(int bEnabled)
{
    CExoStreamingSoundSourceInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->SetReverbEnabled(bEnabled);
    }
}

#ifdef _XBOX

void CExoStreamingSoundSource::SetPitchVariance(float fVariance)  // Xbox 0x001659C0
{
    CExoStreamingSoundSourceInternal* pInternal;

    if (fVariance > 1.0f)
    {
        fVariance = 1.0f;
    }
    else if (fVariance < 0.0f)
    {
        fVariance = 0.0f;
    }
    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->SetPitchVariance(fVariance);
    }
}

void CExoStreamingSoundSource::SetFixedVariance(float fVariance)  // Xbox 0x001659F0
{
    CExoStreamingSoundSourceInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->SetFixedVariance(fVariance);
    }
}

CExoSound::CExoSound(unsigned char n2DVoices, unsigned char n3DVoices, const CExoString& s3DProvider)  // Xbox 0x00165A00
{
    m_pInternal = NULL;
    if (g_bDisableSound == 0)
    {
        m_pInternal = new CExoSoundInternal;
        if (m_pInternal != NULL)
        {
            m_pInternal->Initialize(s3DProvider, n2DVoices, n3DVoices);
        }
    }
}

#endif
#ifndef _XBOX
CExoSound::CExoSound(unsigned char n2DVoices, unsigned char n3DVoices,  // PC 0x00621810 GOG 0x006217E0
                     int nEAXMode, int bA3D)
{
    m_pInternal = NULL;
    if (g_bDisableSound == 0)
    {
        m_pInternal = new CExoSoundInternal;
        if (m_pInternal != NULL)
        {
            m_pInternal->Initialize(n2DVoices, n3DVoices, nEAXMode, bA3D);
        }
    }
}
#endif
// PC 0x006218A0 GOG 0x00621870 Xbox 0x00165A50
void CExoSound::SetMusicVolume(float fVolume, int bFade)
{
    CExoSoundInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->SetMusicVolume(max(0.0f, min(fVolume, 1.0f)), 0, bFade);
        m_pInternal->ApplyNewVolume(0);
    }
}
// PC 0x00621920 GOG 0x006218F0 Xbox 0x00165AA0
void CExoSound::SetSoundEffectVolume(float fVolume)
{
    CExoSoundInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->SetSoundEffectVolume(max(0.0f, min(fVolume, 1.0f)), 0);
        m_pInternal->ApplyNewVolume(0);
    }
}
// PC 0x006219A0 GOG 0x00621970 Xbox 0x00165AF0
void CExoSound::SetDialogVolume(float fVolume)
{
    CExoSoundInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
#ifdef _XBOX
        pInternal->SetDialogVolume(max(0.0f, min(fVolume, 1.0f)), 0);
#else
        pInternal->SetDialogVolume(max(0.01f, min(fVolume, 1.0f)), 0);
#endif
        m_pInternal->ApplyNewVolume(0);
    }
}

// Dead code: not in the PC executable
float CExoSound::GetMusicVolume()
{
    CExoSoundInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        return pInternal->GetMusicVolume(0);
    }
    return 1.0f;
}

// Dead code: not in the PC executable
float CExoSound::GetSoundEffectVolume()
{
    CExoSoundInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        return pInternal->GetSoundEffectVolume(0);
    }
    return 1.0f;
}

// Dead code: not in the PC executable
float CExoSound::GetDialogVolume()
{
    CExoSoundInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        return pInternal->GetDialogVolume(0);
    }
    return 1.0f;
}

#ifdef _XBOX

void CExoSound::SetRollOffFactor(float fFactor)
{
    CExoSoundInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->SetRollOffFactor(fFactor);
    }
}

#endif
void CExoSound::Render()  // PC 0x00621A20 GOG 0x006219F0 Xbox 0x00165B40
{
    CExoSoundInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->Render();
    }
}
// PC 0x00621A30 GOG 0x00621A00 Xbox 0x00165B50
void CExoSound::SetListenerOrientation(const Vector& vOrientation,
                                       const Vector& vUp)
{
    CExoSoundInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->SetListenerOrientation(vOrientation, vUp);
    }
}
// PC 0x00621A40 GOG 0x00621A10 Xbox 0x00165B60
void CExoSound::SetListenerPosition(const Vector& vPosition)
{
    CExoSoundInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->SetListenerPosition(vPosition);
    }
}
// PC 0x00621A50 GOG 0x00621A20 Xbox 0x00165B70
int CExoSound::PlayOneShotSound(const CResRef& cResRef, unsigned char nType,
                                unsigned long nOneShotDelay, unsigned char nVolume, float fFixedVariance,
                                float fPitchVariance)
{
    CExoSoundInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        return pInternal->PlayOneShotSound(cResRef, nType, nOneShotDelay, nVolume, fFixedVariance, fPitchVariance);
    }
    else
    {
        return 0;
    }
}
// PC 0x00621A60 GOG 0x00621A30 Xbox 0x00165B80
int CExoSound::Play3DOneShotSound(const CResRef& cResRef, Vector vPosition,
                                  float fRadius, unsigned char nGroup,
                                  unsigned long nOneShotDelay, unsigned char nVolume, float fFixedVariance,
                                  float fPitchVariance)
{
    CExoSoundInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        vPosition.m_fZ += fRadius;
        return pInternal->Play3DOneShotSound(cResRef, vPosition, nGroup, nOneShotDelay, nVolume,
                                             fFixedVariance, fPitchVariance);
    }
    else
    {
        return 0;
    }
}
// PC 0x00621AC0 GOG 0x00621A90
void CExoSound::StopAllOneShots()
{
    CExoSoundInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->StopAllOneShots();
    }
}

#ifdef _XBOX

void CExoSound::SetSpeakerType(char nSpeakerType)  // Xbox 0x00165BE0
{
    CExoSoundInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->SetSpeakerType(nSpeakerType);
    }
}

void CExoSound::EnableEnvironmentEffects(int bEnabled)  // Xbox 0x00165BF0
{
    CExoSoundInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->EnableEnvironmentEffects(bEnabled);
    }
}

void CExoSound::SetEnvironmentEffectLevel(float fLevel)  // Xbox 0x00165C00
{
    CExoSoundInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->SetEnvironmentEffectLevel(fLevel);
    }
}

#endif
// PC 0x00621AD0 GOG 0x00621AA0
void CExoSound::ResumeAllStreamingMusicForMovie()
{
    CExoSoundInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->ResumeAllStreamingMusicForMovie();
    }
}
void CExoSound::SetSoundMode(int nMode, int bMovie)  // PC 0x00621AE0 GOG 0x00621AB0 Xbox 0x00165C10
{
    CExoSoundInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->SetSoundMode(nMode, bMovie);
    }
}

// Dead code: not in the PC executable
CExoSoundSource::~CExoSoundSource()
{
    if (m_pInternal != NULL)
    {
        delete m_pInternal;
        m_pInternal = NULL;
    }
}
// PC 0x00621D30 GOG 0x00621D00 Xbox 0x00165C60
CExoSoundSource::CExoSoundSource(const CResRef& cResRef)
{
    if (g_bDisableSound != 0)
    {
        m_pInternal = NULL;
    }
    else
    {
        m_pInternal = new CExoSoundSourceInternal(g_pExoSound->m_pInternal);
        if (m_pInternal != NULL)
        {
            m_pInternal->SetResRef(cResRef, 0);
        }
    }
}
// PC 0x00621DE0 GOG 0x00621DB0 Xbox 0x00165CC0
int CExoSoundSource::GetLooping()
{
    CExoSoundSourceInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        return pInternal->GetLooping();
    }
    else
    {
        return 0;
    }
}
// PC 0x00621DF0 GOG 0x00621DC0 Xbox 0x00165CD0
CExoSoundSourceInternal* CExoSoundSource::SetDistance(float fMaxDistance,
                                                      float fMinDistance)
{
    CExoSoundSourceInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->SetDistance(fMaxDistance, fMinDistance);
    }
    return pInternal;
}
// PC 0x00621E10 GOG 0x00621DE0 Xbox 0x00165CF0
void CExoSoundSource::SetResRef(const CResRef& cResRef, int bRequest)
{
    CExoSoundSourceInternal* pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->Stop();
        m_pInternal->m_nDuration = 0;
        m_pInternal->m_pSound->Render();
        m_pInternal->CResHelper<CResWave, RESTYPE_WAV>::SetResRef(cResRef, bRequest);
    }
}
// PC 0x00621E50 GOG 0x00621E20 Xbox 0x00165D30
CExoStreamingSoundSource::~CExoStreamingSoundSource()
{
    if (m_pInternal != NULL)
    {
        delete m_pInternal;
        m_pInternal = NULL;
    }
}
// PC 0x00621E80 GOG 0x00621E50 Xbox 0x00165D60
CResRef CExoStreamingSoundSource::GetFilePlaying()
{
    if (m_pInternal != NULL)
    {
        return m_pInternal->GetFilePlaying();
    }
    else
    {
        return CResRef("");
    }
}
// PC 0x00621ED0 GOG 0x00621EA0 Xbox 0x00165DB0
int CExoStreamingSoundSource::GetLooping()
{
    if (m_pInternal != NULL)
    {
        return m_pInternal->GetLooping();
    }
    else
    {
        return 0;
    }
}

#ifdef _XBOX
void CExoStreamingSoundSource::Set3D(int b3D)  // Xbox 0x00165960
{
    CExoStreamingSoundSourceInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->Set3D(b3D);
    }
}
#else
CExoStreamingSoundSourceInternal* CExoStreamingSoundSource::Set3D(int b3D)  // PC 0x00621EE0 GOG 0x00621EB0
{
    CExoStreamingSoundSourceInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->Set3D(b3D);
    }
    return pInternal;
}
#endif

// Dead code: not in the PC executable
int CExoStreamingSoundSource::Get3D()
{
    CExoStreamingSoundSourceInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        return pInternal->Get3D();
    }
    return 0;
}

#ifdef _XBOX
void CExoStreamingSoundSource::SetDistance(float fMinDistance, float fMaxDistance)  // Xbox 0x00165970
{
    CExoStreamingSoundSourceInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->SetDistance(fMinDistance, fMaxDistance);
    }
}
#else
CExoStreamingSoundSourceInternal* CExoStreamingSoundSource::SetDistance(  // PC 0x00621EF0 GOG 0x00621EC0
    float fMinDistance, float fMaxDistance)
{
    CExoStreamingSoundSourceInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->SetDistance(fMinDistance, fMaxDistance);
    }
    return pInternal;
}
#endif
// PC 0x00621F10 GOG 0x00621EE0 Xbox 0x00165980
void CExoStreamingSoundSource::SetPosition(const Vector& vPosition)
{
    CExoStreamingSoundSourceInternal* pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->SetPosition(vPosition);
    }
}
// PC 0x00621F40 GOG 0x00621F10 Xbox 0x00165DC0
void CExoStreamingSoundSource::SetStreamingMusicForMovie(
    int bStreamingMusicForMovie)
{
    if (EXOSOUND_DEBUG)
    {
        EXOLOG("CExoStreamingSoundSource::SetStreamingMusicForMovie() volume %d\n", GetVolume());
    }
    CExoStreamingSoundSourceInternal* pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->m_bStreamingMusicForMovie = bStreamingMusicForMovie;
    }
}
// PC 0x00621F60 GOG 0x00621F30 Xbox 0x00165DE0
CExoSound::~CExoSound()
{
    if (m_pInternal != NULL)
    {
        delete m_pInternal;
        m_pInternal = NULL;
    }
}
// PC 0x00621F90 GOG 0x00621F60 Xbox 0x00165E10
void CExoSound::Set2D3DBias(float fBias)
{
    CExoSoundInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->Set2D3DBias(max(0.1f, min(fBias, 1.9f)));
    }
}
// PC 0x00621FF0 GOG 0x00621FC0
float CExoSound::Get2D3DBias()
{
    if (m_pInternal != NULL)
    {
        return m_pInternal->Get2D3DBias();
    }
    else
    {
        return 0.5f;
    }
}
// PC 0x00622010 GOG 0x00621FE0
unsigned char CExoSound::GetNumber2DVoices()
{
    if (m_pInternal != NULL)
    {
        return m_pInternal->GetNumber2DVoices();
    }
    else
    {
        return 24;
    }
}
// PC 0x00622020 GOG 0x00621FF0
unsigned char CExoSound::GetNumber3DVoices()
{
    if (m_pInternal != NULL)
    {
        return m_pInternal->GetNumber3DVoices();
    }
    else
    {
        return 16;
    }
}
// PC 0x00622030 GOG 0x00622000 Xbox 0x00165E50
void CExoSound::GetListenerPosition(Vector* pPosition)
{
    if (m_pInternal != NULL)
    {
        *pPosition = m_pInternal->GetListenerPosition();
    }
}

#ifndef _XBOX
void CExoSound::ReInitialize(int nEAXMode, int bA3D)  // PC 0x00622060 GOG 0x00622030
{
    CExoSoundInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->Initialize(
            pInternal->GetNumber2DVoices() < 24 ? 24 : pInternal->GetNumber2DVoices(),
            pInternal->GetNumber3DVoices() < 16 ? 16 : pInternal->GetNumber3DVoices(), nEAXMode,
            bA3D);
    }
}
#endif

#ifndef _XBOX
CExoSoundInternal* CExoSound::EnableStreamingEnvironmentEffects(int bEnabled)  // PC 0x006220A0 GOG 0x00622070
{
    CExoSoundInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->EnableStreamingEnvironmentEffects(bEnabled);
    }
    return pInternal;
}
#endif

#ifndef _XBOX
CExoSoundInternal* CExoSound::EnableNonStreamingEnvironmentEffects(int bEnabled)  // PC 0x006220B0 GOG 0x00622080
{
    CExoSoundInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->EnableNonStreamingEnvironmentEffects(bEnabled);
    }
    return pInternal;
}
#endif
// PC 0x006220C0 GOG 0x00622090 Xbox 0x00165E80
int CExoSound::GetEAX()
{
    if (m_pInternal != NULL)
    {
        return m_pInternal->GetEAX();
    }
    else
    {
        return 0;
    }
}

#ifndef _XBOX
int CExoSound::GetBestEAXAvailable()  // PC 0x006220D0 GOG 0x006220A0
{
    if (m_pInternal != NULL)
    {
        return m_pInternal->GetBestEAXAvailable();
    }
    else
    {
        return 0;
    }
}
#endif
// PC 0x006220E0 GOG 0x006220B0 Xbox 0x00165E90
CExoSoundInternal* CExoSound::SetDesiredEnvironment(int nEnvironment)
{
    CExoSoundInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->SetDesiredEnvironment(nEnvironment);
    }
    return pInternal;
}
// PC 0x006220F0 GOG 0x006220C0 Xbox 0x00165EA0
CExoSoundInternal* CExoSound::SetDesiredAmbientScale(float fScale)
{
    CExoSoundInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->SetDesiredAmbientScale(fScale);
    }
    return pInternal;
}
// PC 0x00622100 GOG 0x006220D0
int CExoSound::GetSoundMode()
{
    if (m_pInternal != NULL)
    {
        return m_pInternal->GetSoundMode();
    }
    else
    {
        return 0;
    }
}
// PC 0x00622120 GOG 0x006220F0 Xbox 0x00165EC0
void CExoSound::GetPriorityGroupDistance(int nGroup, float* pfMinDistance,
                                         float* pfMaxDistance)
{
    if (EXOSOUND_DEBUG)
    {
        EXOLOG("CExoSound::GetPriorityGroupDistance() music volume %f\n", GetMusicVolume());
    }
    CExoSoundInternal* pInternal;
    CPriorityGroup* pPriorityGroup;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pPriorityGroup = pInternal->GetPriorityGroup((unsigned char)nGroup);
        *pfMinDistance = pPriorityGroup->m_fMaxVolumeDist;
        *pfMaxDistance = pPriorityGroup->m_fMinVolumeDist;
    }
}

#ifndef _XBOX
int CExoSound::GetLostFocusPaused()  // PC 0x00622150 GOG 0x00622120
{
    if (m_pInternal != NULL)
    {
        return m_pInternal->GetLostFocusPaused();
    }
    else
    {
        return 0;
    }
}
#endif

#ifdef _XBOX
int CExoStreamingSoundSource::GetHasStartedStreaming()  // Xbox 0x001659A0
{
    if (m_pInternal != NULL)
    {
        return m_pInternal->GetHasStartedStreaming();
    }
    return 1;
}
#else
int CExoStreamingSoundSource::GetHasStartedStreaming()  // PC 0x0065D0F0 GOG 0x0065CF00
{
    return 1;
}
#endif

#ifndef _XBOX
inline int CExoSoundSourceInternal::GetLooping()  // PC 0x005ED200 GOG 0x005ED1E0
{
    return m_bLooping;
}
#endif
inline void CExoSoundSourceInternal::SetDistance(float fMaxDistance,
                                                 float fMinDistance)
{
    m_fMaxVolumeDist = fMaxDistance;
    m_fMinVolumeDist = fMinDistance;
}

inline CResRef CExoStreamingSoundSourceInternal::GetFilePlaying()
{
    return m_resRef;
}
#ifndef _XBOX
inline int CExoStreamingSoundSourceInternal::GetLooping()  // PC 0x006CB4A0 GOG 0x006CB1A0
{
    return m_bLooping;
}
#endif
#ifndef _XBOX
inline void CExoStreamingSoundSourceInternal::Set3D(int b3D)
{
    m_b3D = b3D;
}
#endif

#ifndef _XBOX
inline void CExoStreamingSoundSourceInternal::SetDistance(float fMinDistance,
                                                          float fMaxDistance)
{
    m_fMinDistance = fMinDistance;
    m_fMaxDistance = fMaxDistance;
}
#endif

#ifndef _XBOX
inline void CExoStreamingSoundSourceInternal::SetPosition(Vector vPosition)
{
    m_vPosition = vPosition;
}
#endif
// PC 0x00470D50 GOG 0x00470E90
inline unsigned char CExoStreamingSoundSourceInternal::GetVolume()
{
    return m_nVolume;
}
// PC 0x00621C50 GOG 0x00621C20
inline unsigned char CExoStreamingSoundSourceInternal::GetPriorityGroup()
{
    return m_nPriorityGroup;
}
#ifndef _XBOX
inline int CExoStreamingSoundSourceInternal::Get3D()  // PC 0x0046DD00 GOG 0x0046DE60
{
    return m_b3D;
}
// PC 0x00621C80 GOG 0x00621C50
inline float CExoSoundInternal::GetMusicVolume(int bTemporary)
{
    return bTemporary ? m_fTempMusicVolume : m_fMusicVolume;
}
// PC 0x00621CA0 GOG 0x00621C70
inline float CExoSoundInternal::GetSoundEffectVolume(int bTemporary)
{
    return bTemporary ? m_fTempSoundEffectVolume : m_fSoundEffectVolume;
}
// PC 0x00621CC0 GOG 0x00621C90
inline float CExoSoundInternal::GetDialogVolume(int bTemporary)
{
    return bTemporary ? m_fTempDialogVolume : m_fDialogVolume;
}
#endif
inline void CExoSoundInternal::Set2D3DBias(float fBias)
{
    m_f2D3DBias = fBias;
}

inline float CExoSoundInternal::Get2D3DBias()
{
    return m_f2D3DBias;
}
// PC 0x00621CE0 GOG 0x00621CB0
inline unsigned char CExoSoundInternal::GetNumber2DVoices()
{
    return m_n2DVoices;
}

inline unsigned char CExoSoundInternal::GetNumber3DVoices()
{
    return m_n3DVoices;
}

inline Vector CExoSoundInternal::GetListenerPosition()
{
    return m_vListenerPosition;
}

#ifndef _XBOX
inline void CExoSoundInternal::EnableStreamingEnvironmentEffects(int bEnabled)
{
    m_bStreamingEnvironmentEffects = bEnabled;
}
#endif

#ifndef _XBOX
inline void CExoSoundInternal::EnableNonStreamingEnvironmentEffects(int bEnabled)
{
    m_bNonStreamingEnvironmentEffects = bEnabled;
}
#endif

#ifdef _XBOX

inline int CExoSoundInternal::GetEAX()
{
    return (int)m_pDirectSound;
}
#else
inline int CExoSoundInternal::GetEAX()
{
    return m_nEAX;
}
#endif

#ifndef _XBOX
inline int CExoSoundInternal::GetBestEAXAvailable()
{
    return m_nBestEAXAvailable;
}
#endif

inline void CExoSoundInternal::SetDesiredEnvironment(int nEnvironment)  // Xbox 0x00166F00
{
    m_nDesiredEnvironment = nEnvironment;
}

inline void CExoSoundInternal::SetDesiredAmbientScale(float fScale)
{
    m_fDesiredAmbientScale = fScale;
}

inline int CExoSoundInternal::GetSoundMode()
{
    return m_anSoundMode[m_nSoundMode];
}
#ifndef _XBOX
inline int CExoSoundInternal::GetLostFocusPaused()
{
    return m_bPaused;
}
#endif
