#include <windows.h>
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
// PC 0x005D5870  Xbox 0x00152790
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
// PC 0x005D5900  Xbox 0x00152830
void CExoSoundSource::SetPriorityGroup(unsigned char nGroup)
{
    CExoSoundSourceInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->SetPriorityGroup(nGroup);
    }
}
// PC 0x005D5910  Xbox 0x00152840
void CExoSoundSource::Set3D(int b3D)
{
    CExoSoundSourceInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->Set3D(b3D);
    }
}
// PC 0x005D5920  Xbox 0x00152850
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
// PC 0x005D5930  Xbox 0x00152860
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
// PC 0x005D5950  Xbox 0x00152880
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
// PC 0x005D5980  Xbox 0x001528B0
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
// PC 0x005D59D0  Xbox 0x001528E0
void CExoSoundSource::SetLooping(int bLooping)
{
    CExoSoundSourceInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->SetLooping(bLooping);
    }
}
// PC 0x005D59E0  Xbox 0x001528F0
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
// PC 0x005D5A20  Xbox 0x00152940
void CExoSoundSource::Stop()
{
    CExoSoundSourceInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->Stop();
    }
}
// PC 0x005D5A30  Xbox 0x00152950
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
// PC 0x005D5A50  Xbox 0x00152970
CExoStreamingSoundSource::CExoStreamingSoundSource()
{
    if (g_bDisableSound != 0)
    {
        m_pInternal = NULL;
    }
    else
    {
        m_pInternal =
            new CExoStreamingSoundSourceInternal(g_pExoSound->m_pInternal);
    }
}
// PC 0x005D5AF0  Xbox 0x00152A00
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
// PC 0x005D5B00  Xbox 0x00152A10
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
// PC 0x005D5B10  Xbox 0x00152A20
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
// PC 0x005D5B20  Xbox 0x00152A30
void CExoStreamingSoundSource::Pause()
{
    CExoStreamingSoundSourceInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->Pause();
    }
}
// PC 0x005D5B30  Xbox 0x00152A40
void CExoStreamingSoundSource::Resume()
{
    CExoStreamingSoundSourceInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->Resume();
    }
}
// PC 0x005D5B40  Xbox 0x00152A50
void CExoStreamingSoundSource::SetLooping(int bLooping)
{
    CExoStreamingSoundSourceInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->SetLooping(bLooping);
    }
}
// PC 0x005D5B50  Xbox 0x00152A60
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
// PC 0x005D5B80  Xbox 0x00152A90
void CExoStreamingSoundSource::Stop()
{
    CExoStreamingSoundSourceInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->Stop();
    }
}
// PC 0x005D5B90  Xbox 0x00152AA0
void CExoStreamingSoundSource::FadeAndStop(unsigned long nFadeTime)
{
    CExoStreamingSoundSourceInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->FadeAndStop(nFadeTime);
    }
}
// PC 0x005D5BA0  Xbox 0x00152AB0
void CExoStreamingSoundSource::SetPriorityGroup(unsigned char nGroup)
{
    CExoStreamingSoundSourceInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->SetPriorityGroup(nGroup);
    }
}

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
// PC 0x005D5BB0  Xbox 0x00152AF0
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
// Xbox 0x00152B20
void CExoStreamingSoundSource::SetPitchVariance(float fVariance)
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
// Xbox 0x00152B50
void CExoStreamingSoundSource::SetFixedVariance(float fVariance)
{
    CExoStreamingSoundSourceInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->SetFixedVariance(fVariance);
    }
}
// Xbox 0x00152B60
CExoSound::CExoSound(unsigned char n2DVoices, unsigned char n3DVoices, const CExoString& s3DProvider)
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
CExoSound::CExoSound(unsigned char n2DVoices, unsigned char n3DVoices,  // PC 0x005D5BC0
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
// PC 0x005D5C50  Xbox 0x00152BF0
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
// PC 0x005D5CD0  Xbox 0x00152C40
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
// PC 0x005D5D50  Xbox 0x00152C90
void CExoSound::SetDialogVolume(float fVolume)
{
    CExoSoundInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->SetDialogVolume(max(0.0f, min(fVolume, 1.0f)), 0);
        m_pInternal->ApplyNewVolume(0);
    }
}

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
// Xbox 0x00152CE0
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
void CExoSound::Render()  // PC 0x005D5DD0  Xbox 0x00152CF0
{
    CExoSoundInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->Render();
    }
}
// PC 0x005D5DE0  Xbox 0x00152D00
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
// PC 0x005D5DF0  Xbox 0x00152D10
void CExoSound::SetListenerPosition(const Vector& vPosition)
{
    CExoSoundInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->SetListenerPosition(vPosition);
    }
}
// PC 0x005D5E00  Xbox 0x00152D20
int CExoSound::PlayOneShotSound(const CResRef& cResRef, unsigned char nType,
                                unsigned long nA, unsigned char nB, float fA,
                                float fB)
{
    CExoSoundInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        return pInternal->PlayOneShotSound(cResRef, nType, nA, nB, fA, fB);
    }
    else
    {
        return 0;
    }
}
// PC 0x005D5E10  Xbox 0x00152D30
int CExoSound::Play3DOneShotSound(const CResRef& cResRef, Vector vPosition,
                                  float fRadius, unsigned char nGroup,
                                  unsigned long nA, unsigned char nB, float fA,
                                  float fB)
{
    CExoSoundInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        vPosition.m_fZ += fRadius;
        return pInternal->Play3DOneShotSound(cResRef, vPosition, nGroup, nA, nB,
                                             fA, fB);
    }
    else
    {
        return 0;
    }
}
// PC 0x005D5E70
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
// Xbox 0x00152D90
void CExoSound::SetSpeakerType(char nSpeakerType)
{
    CExoSoundInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->SetSpeakerType(nSpeakerType);
    }
}
// Xbox 0x00152DA0
void CExoSound::EnableEnvironmentEffects(int bEnabled)
{
    CExoSoundInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->EnableEnvironmentEffects(bEnabled);
    }
}
// Xbox 0x00152DB0
void CExoSound::SetEnvironmentEffectLevel(float fLevel)
{
    CExoSoundInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->SetEnvironmentEffectLevel(fLevel);
    }
}

#endif
void CExoSound::SetSoundMode(int nMode)  // PC 0x005D5E80  Xbox 0x00152DC0
{
    CExoSoundInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->SetSoundMode(nMode);
    }
}

CExoSoundSource::~CExoSoundSource()
{
    if (m_pInternal != NULL)
    {
        delete m_pInternal;
        m_pInternal = NULL;
    }
}
// PC 0x005D60E0  Xbox 0x00152E10
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
// PC 0x005D6190  Xbox 0x00152EC0
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
// PC 0x005D61A0  Xbox 0x00152ED0
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
// PC 0x005D61C0  Xbox 0x00152EF0
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
// PC 0x005D6200  Xbox 0x00152F30
CExoStreamingSoundSource::~CExoStreamingSoundSource()
{
    if (m_pInternal != NULL)
    {
        delete m_pInternal;
        m_pInternal = NULL;
    }
}
// PC 0x005D6230  Xbox 0x00152F60
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
// PC 0x005D6280  Xbox 0x00152FB0
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
void CExoStreamingSoundSource::Set3D(int b3D)  // Xbox 0x00152AC0
{
    CExoStreamingSoundSourceInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->Set3D(b3D);
    }
}
#else
CExoStreamingSoundSourceInternal* CExoStreamingSoundSource::Set3D(int b3D)  // PC 0x005D6290
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
void CExoStreamingSoundSource::SetDistance(float fMinDistance, float fMaxDistance)  // Xbox 0x00152AD0
{
    CExoStreamingSoundSourceInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->SetDistance(fMinDistance, fMaxDistance);
    }
}
#else
CExoStreamingSoundSourceInternal* CExoStreamingSoundSource::SetDistance(  // PC 0x005D62A0
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
// PC 0x005D62C0  Xbox 0x00152AE0
void CExoStreamingSoundSource::SetPosition(const Vector& vPosition)
{
    CExoStreamingSoundSourceInternal* pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->SetPosition(vPosition);
    }
}
// PC 0x005D62F0  Xbox 0x00152FC0
CExoSound::~CExoSound()
{
    if (m_pInternal != NULL)
    {
        delete m_pInternal;
        m_pInternal = NULL;
    }
}
// PC 0x005D6320  Xbox 0x00152FF0
void CExoSound::Set2D3DBias(float fBias)
{
    CExoSoundInternal* pInternal;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pInternal->Set2D3DBias(max(0.1f, min(fBias, 1.9f)));
    }
}
// PC 0x005D6380
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
// PC 0x005D63A0
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
// PC 0x005D63B0
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
// PC 0x005D63C0  Xbox 0x00153030
void CExoSound::GetListenerPosition(Vector* pPosition)
{
    if (m_pInternal != NULL)
    {
        *pPosition = m_pInternal->GetListenerPosition();
    }
}

#ifndef _XBOX
void CExoSound::ReInitialize(int nEAXMode, int bA3D)  // PC 0x005D63F0
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
CExoSoundInternal* CExoSound::EnableStreamingEnvironmentEffects(int bEnabled)  // PC 0x005D6430
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
CExoSoundInternal* CExoSound::EnableNonStreamingEnvironmentEffects(int bEnabled)  // PC 0x005D6440
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
// PC 0x005D6450  Xbox 0x00153060
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
int CExoSound::GetBestEAXAvailable()  // PC 0x005D6460
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
// PC 0x005D6470  Xbox 0x00153070
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
// PC 0x005D6480  Xbox 0x00153080
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
// PC 0x005D6490
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
// PC 0x005D64B0  Xbox 0x001530A0
void CExoSound::GetPriorityGroupDistance(int nGroup, float* pfMinDistance,
                                         float* pfMaxDistance)
{
    CExoSoundInternal* pInternal;
    CExoSoundPriorityGroup* pPriorityGroup;

    pInternal = m_pInternal;
    if (pInternal != NULL)
    {
        pPriorityGroup = pInternal->GetPriorityGroup((unsigned char)nGroup);
        *pfMinDistance = pPriorityGroup->m_fMaxVolumeDist;
        *pfMaxDistance = pPriorityGroup->m_fMinVolumeDist;
    }
}

#ifndef _XBOX
int CExoSound::GetLostFocusPaused()  // PC 0x005D64E0
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
int CExoStreamingSoundSource::GetHasStartedStreaming()  // PC 0x0063E7A0  Xbox 0x00152B00
{
    if (m_pInternal != NULL)
    {
        return m_pInternal->GetHasStartedStreaming();
    }
    return 1;
}
#else
int CExoStreamingSoundSource::GetHasStartedStreaming()  // PC 0x0063E7A0  Xbox 0x00152B00
{
    return 1;
}
#endif
// PC 0x005A4C50
inline int CExoSoundSourceInternal::GetLooping()
{
    return m_bLooping;
}

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
// PC 0x00675DD0
inline int CExoStreamingSoundSourceInternal::GetLooping()
{
    return m_bLooping;
}

#ifndef _XBOX
inline void CExoStreamingSoundSourceInternal::Set3D(int b3D)  // Xbox 0x001560A0
{
    m_b3D = b3D;
}
#endif

#ifndef _XBOX
inline void CExoStreamingSoundSourceInternal::SetDistance(float fMinDistance,  // Xbox 0x00156170
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
// PC 0x005D5FE0
inline unsigned char CExoStreamingSoundSourceInternal::GetVolume()
{
    return m_nVolume;
}
// PC 0x005D5FF0
inline unsigned char CExoStreamingSoundSourceInternal::GetPriorityGroup()
{
    return m_nPriorityGroup;
}
// PC 0x005D6000
inline int CExoStreamingSoundSourceInternal::Get3D()
{
    return m_b3D;
}
// PC 0x005D6010
inline float CExoSoundInternal::GetMusicVolume(int bTemporary)
{
    return bTemporary ? m_fTempMusicVolume : m_fMusicVolume;
}
// PC 0x005D6030
inline float CExoSoundInternal::GetSoundEffectVolume(int bTemporary)
{
    return bTemporary ? m_fTempSoundEffectVolume : m_fSoundEffectVolume;
}
// PC 0x005D6050
inline float CExoSoundInternal::GetDialogVolume(int bTemporary)
{
    return bTemporary ? m_fTempDialogVolume : m_fDialogVolume;
}

inline void CExoSoundInternal::Set2D3DBias(float fBias)
{
    m_f2D3DBias = fBias;
}

inline float CExoSoundInternal::Get2D3DBias()
{
    return m_f2D3DBias;
}
// PC 0x005D6070
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

inline void CExoSoundInternal::SetDesiredEnvironment(int nEnvironment)
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
// PC 0x005D6080
inline CExoSoundPriorityGroup* CExoSoundInternal::GetPriorityGroup(unsigned char nGroup)
{
    return m_pPriorityGroups + nGroup;
}

#ifndef _XBOX
inline int CExoSoundInternal::GetLostFocusPaused()
{
    return m_bPaused;
}
#endif
