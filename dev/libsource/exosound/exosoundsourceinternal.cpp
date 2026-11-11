#include "exosoundinternal.h"
#include "exobase.h"
#ifndef _XBOX
#include "mss.h"
#endif
#include <math.h>
#include <stdlib.h>

static inline void ExosoundsourceinternalTrace2();
static inline void ExosoundsourceinternalTrace1();
#define EXOSOUNDSOURCEINTERNAL_TRACE() ExosoundsourceinternalTrace2()

static inline void ExoSoundSourceTrace();
#define EXOSOUNDSOURCE_TRACE() ExoSoundSourceTrace()

#ifndef _XBOX
int CExoSoundSourceInternal::IsPaused()  // PC 0x0063E7F0  Xbox 0x00154870
{
    return 0;
}
#endif
// PC 0x005DAE50  Xbox 0x00154220
int CExoSoundSourceInternal::Demand()
{
    if (m_pRes == 0)
    {
        return 0;
    }
    if (m_bDemanded != 0 && m_pRes->GetDemands() != 0)
    {
        return 1;
    }
    int bResult = m_pRes->Demand() != 0;
    m_bDemanded = bResult;
    return bResult;
}

#ifdef _XBOX
// PC 0x005DAE90  Xbox 0x00154430
int CExoSoundSourceInternal::ReleaseDemand()
{
    m_nDuration = 0;
    m_pData = 0;
    if (m_bDemanded != 0)
    {
        m_pSound->DetachReleasingVoices(this);
        m_bDemanded = 0;
        return m_pRes->Release() != 0;
    }
    return 0;
}
#else
int CExoSoundSourceInternal::ReleaseDemand()  // PC 0x005DAE90  Xbox 0x00154430
{
    if (m_bDemanded != 0)
    {
        m_bDemanded = 0;
        return m_pRes->Release() != 0;
    }
    return 0;
}
#endif

#ifdef _XBOX
void CExoSoundSourceInternal::DoObstructionWork()  // PC 0x005DAEB0  Xbox 0x001549D0
{
    if (m_p3DVoice != 0)
    {
        DSI3DL2BUFFER cSource;

        ZeroMemory(&cSource, sizeof(cSource));
        cSource.Obstruction.lHFLevel = (int)(m_fObstruction * -2000.0f);
        cSource.Obstruction.flLFRatio = m_fObstruction * 0.25f;
        m_p3DVoice->m_pBuffer->SetI3DL2Source(&cSource, DS3D_IMMEDIATE);
    }
}
#else
void CExoSoundSourceInternal::DoObstructionWork()  // PC 0x005DAEB0  Xbox 0x001549D0
{
    if (m_p3DVoice != 0)
    {
        AIL_set_3D_sample_obstruction(m_p3DVoice->m_hSample, m_fObstruction);
    }
}
#endif

#ifdef _XBOX
CExoSoundSourceInternal::CExoSoundSourceInternal(CExoSoundInternal* pSound)  // PC 0x005DAF50  Xbox 0x001542E0
{
    m_pSound = pSound;
    m_fMaxVolumeDist = 10.0f;
    m_pListNode = 0;
    m_b3D = 0;
    m_bLooping = 0;
    m_bPlayWhenFree = 0;
    m_p3DVoice = 0;
    m_p2DVoice = 0;
    m_nRestartPosition = 0;
    m_nListType = LIST_NONE;
    m_nVolume = 127;
    m_fMinVolumeDist = 20.0f;
    m_nPriorityGroup = CExoSound::PRIORITY_GROUP_DEFAULT;
    m_nGroupVolume = pSound->m_pPriorityGroups[CExoSound::PRIORITY_GROUP_DEFAULT].m_nVolume;
    m_bDemanded = 0;
    m_nOneShotDelay = 0;
    m_nDelayStart = 0;
    m_nDuration = 0;
    m_fPitchVariance = 0.0f;
    m_fFixedVariance = 0.0f;
    int nSourceId = pSound->m_nNextSourceId++;
    m_nSourceId = nSourceId;
    m_nUnknown20 = 0;
    m_nDataSize = 0;
    m_pData = 0;
    ZeroMemory(&m_wfxFormat, sizeof(m_wfxFormat));
    m_vObstructionListenerPosition = Vector();
    m_nObstructionTime = 0;
    m_fObstruction = 0.0f;
}
#else
CExoSoundSourceInternal::CExoSoundSourceInternal(CExoSoundInternal* pSound)  // PC 0x005DAF50  Xbox 0x001542E0
{
    m_pSound = pSound;
    m_pListNode = 0;
    m_b3D = 0;
    m_bLooping = 0;
    m_nVolume = CExoSound::CMAX_VOLUME;
    m_bPlayWhenFree = 0;
    m_p3DVoice = 0;
    m_p2DVoice = 0;
    m_fMaxVolumeDist = 10.0f;
    m_fMinVolumeDist = 20.0f;
    m_nRestartPosition = 0;
    m_nListType = LIST_NONE;
    m_nPriorityGroup = CExoSound::PRIORITY_GROUP_DEFAULT;
    m_nGroupVolume = pSound->m_pPriorityGroups[CExoSound::PRIORITY_GROUP_DEFAULT].m_nVolume;
    m_fPitchVariance = 0.0f;
    m_fFixedVariance = 0.0f;
    m_bDemanded = 0;
    m_nOneShotDelay = 0;
    m_nDelayStart = 0;
    m_nDuration = 0;
    int nSourceId = pSound->m_nNextSourceId++;
    m_nSourceId = nSourceId;
    m_vObstructionListenerPosition = Vector();
    m_nObstructionTime = 0;
    m_fObstruction = 0.0f;
}
#endif

#ifdef _XBOX
CExoSoundSourceInternal::~CExoSoundSourceInternal()  // PC 0x005DB480  Xbox 0x00154A50
{
    ShutDownSource();
    ReleaseDemand();
}
#else
CExoSoundSourceInternal::~CExoSoundSourceInternal()  // PC 0x005DB480  Xbox 0x00154A50
{
    ShutDownSource();
}
#endif
// PC 0x005DB050  Xbox 0x00154470
int CExoSoundSourceInternal::GetWithinRangeOfListener()
{
    if (m_b3D == 0)
    {
        return 1;
    }

    Vector vPosition = m_vPosition;
    Vector* pListenerPosition = &m_pSound->m_vListenerPosition;
    Vector vListenerPosition = *pListenerPosition;
    return magnitude(vListenerPosition - vPosition) <= m_fMinVolumeDist;
}

#ifdef _XBOX
int CExoSoundSourceInternal::IsPlaying()  // PC 0x005DB0E0  Xbox 0x00154510
{
    DWORD nStatus = 0;

    EXOSOUNDSOURCE_TRACE();
    if (m_pSound != 0 && m_pSound->m_bInitialized != 0)
    {
        if (m_p3DVoice != 0 && m_p3DVoice->m_pSource != 0)
        {
            if (m_p3DVoice->m_pBuffer != 0)
            {
                m_p3DVoice->m_pBuffer->GetStatus(&nStatus);
                return (nStatus & DSBSTATUS_PLAYING) == DSBSTATUS_PLAYING;
            }
        }
        else if (m_p2DVoice != 0 && m_p2DVoice->m_pSource != 0)
        {
            if (m_p2DVoice->m_pBuffer != 0)
            {
                m_p2DVoice->m_pBuffer->GetStatus(&nStatus);
                return (nStatus & DSBSTATUS_PLAYING) == DSBSTATUS_PLAYING;
            }
        }
    }
    return 0;
}
#else
int CExoSoundSourceInternal::IsPlaying()  // PC 0x005DB0E0  Xbox 0x00154510
{
    EXOSOUNDSOURCE_TRACE();
    if (m_pSound->m_bInitialized != 0)
    {
        if (m_p3DVoice != 0 && m_p3DVoice->m_hSample != 0)
        {
            return AIL_3D_sample_status(m_p3DVoice->m_hSample) == 4;
        }
        if (m_p2DVoice != 0 && m_p2DVoice->m_hSample != 0)
        {
            return AIL_sample_status(m_p2DVoice->m_hSample) == 4;
        }
    }
    return 0;
}
#endif
// PC 0x005DB130  Xbox 0x00154570
void CExoSoundSourceInternal::SetLooping(int bLooping)
{
    EXOSOUNDSOURCE_TRACE();
    if (m_nListType == LIST_NONE)
    {
        m_bLooping = bLooping;
    }
}

#ifdef _XBOX
void CExoSoundSourceInternal::Pause()  // PC 0x005DB150  Xbox 0x00154590
{
    if (m_pSound->m_bInitialized != 0)
    {
        if (IsPlaying() != 0)
        {
            if (m_p3DVoice != 0 && m_p3DVoice->m_pSource != 0)
            {
                m_p3DVoice->m_pBuffer->GetCurrentPosition(&m_nRestartPosition, NULL);
                m_p3DVoice->m_pBuffer->Stop();
            }
            else if (m_p2DVoice != 0 && m_p2DVoice->m_pSource != 0)
            {
                m_p2DVoice->m_pBuffer->GetCurrentPosition(&m_nRestartPosition, NULL);
                m_p2DVoice->m_pBuffer->Stop();
            }
        }
    }
}
#else
void CExoSoundSourceInternal::Pause()  // PC 0x005DB150  Xbox 0x00154590
{
    if (m_pSound->m_bInitialized != 0)
    {
        if (IsPlaying() != 0)
        {
            if (m_p3DVoice != 0 && m_p3DVoice->m_hSample != 0)
            {
                m_nRestartPosition =
                    AIL_3D_sample_offset(m_p3DVoice->m_hSample);
                AIL_stop_3D_sample(m_p3DVoice->m_hSample);
            }
            else if (m_p2DVoice != 0 && m_p2DVoice->m_hSample != 0)
            {
                m_nRestartPosition = AIL_sample_position(m_p2DVoice->m_hSample);
                AIL_stop_sample(m_p2DVoice->m_hSample);
            }
        }
    }
}
#endif

#ifdef _XBOX
void CExoSoundSourceInternal::Resume()  // PC 0x005DB1C0  Xbox 0x00154600
{
    EXOSOUNDSOURCE_TRACE();
    if (m_pSound->m_bInitialized != 0)
    {
        if (m_p3DVoice != 0 && m_p3DVoice->m_pSource != 0)
        {
            m_p3DVoice->m_pBuffer->SetCurrentPosition(m_nRestartPosition);
            if (m_bLooping != 0)
            {
                m_p3DVoice->m_pBuffer->Play(0, 0, DSBPLAY_LOOPING);
            }
            else
            {
                m_p3DVoice->m_pBuffer->Play(0, 0, 0);
            }
            m_nRestartPosition = 0;
        }
        else
        {
            if (m_p2DVoice != 0 && m_p2DVoice->m_pSource != 0)
            {
                m_p2DVoice->m_pBuffer->SetCurrentPosition(m_nRestartPosition);
                if (m_bLooping != 0)
                {
                    m_p2DVoice->m_pBuffer->Play(0, 0, DSBPLAY_LOOPING);
                }
                else
                {
                    m_p2DVoice->m_pBuffer->Play(0, 0, 0);
                }
            }
            m_nRestartPosition = 0;
        }
    }
}
#else
void CExoSoundSourceInternal::Resume()  // PC 0x005DB1C0  Xbox 0x00154600
{
    EXOSOUNDSOURCE_TRACE();
    if (m_pSound->m_bInitialized != 0)
    {
        if (m_p3DVoice != 0 && m_p3DVoice->m_hSample != 0)
        {
            m_pSound->PlaySourceOn3DVoice(this, m_p3DVoice);
            m_nRestartPosition = 0;
        }
        else
        {
            if (m_p2DVoice != 0 && m_p2DVoice->m_hSample != 0)
            {
                m_pSound->PlaySourceOn2DVoice(this, m_p2DVoice);
            }
            m_nRestartPosition = 0;
        }
    }
}
#endif

#ifdef _XBOX
// PC 0x005DB210  Xbox 0x001546C0
void CExoSoundSourceInternal::ShutDownSource()
{
    int bReleaseDemand = 1;

    EXOSOUNDSOURCE_TRACE();
    if (m_nListType == 1)
    {
        m_pSound->RemoveFromPlayingList(this);
    }
    else if (m_nListType == LIST_NONPLAYING_LOOPING)
    {
        m_pSound->RemoveFromNonPlayingLoopingList(this);
    }
    else if (m_nListType == 3)
    {
        m_pSound->RemoveFromDelayedOneShotList(this);
    }

    if (m_pSound->m_bInitialized != 0 &&
        ((m_p3DVoice != 0 && m_p3DVoice->m_pSource != 0) || (m_p2DVoice != 0 && m_p2DVoice->m_pSource != 0)))
    {
        if (m_p3DVoice != 0)
        {
            if (m_p3DVoice->m_pBuffer != 0)
            {
                if (m_pSound->IsReleasing3DVoice(m_p3DVoice) == 0)
                {
                    m_p3DVoice->m_pBuffer->Stop();
                    if (m_p3DVoice->m_pSource->IsPlaying())
                    {
                        m_pSound->AddReleasing3DVoice(m_p3DVoice);
                        bReleaseDemand = 0;
                    }
                    else
                    {
                        m_p3DVoice->m_pBuffer->Release();
                        m_p3DVoice->m_pSource = 0;
                    }
                }
                m_p3DVoice = 0;
            }
        }
        else if (m_p2DVoice->m_pBuffer != 0)
        {
            if (m_pSound->IsReleasing2DVoice(m_p2DVoice) == 0)
            {
                m_p2DVoice->m_pBuffer->Stop();
                if (m_p2DVoice->m_pSource->IsPlaying())
                {
                    m_pSound->AddReleasing2DVoice(m_p2DVoice);
                    bReleaseDemand = 0;
                }
                else
                {
                    m_p2DVoice->m_pBuffer->Release();
                    m_p2DVoice->m_pSource = 0;
                }
            }
            m_p2DVoice = 0;
        }
        m_nListType = LIST_NONE;
        m_pListNode = 0;
        if (bReleaseDemand != 0)
        {
            ReleaseDemand();
        }
    }
}
#else
void CExoSoundSourceInternal::ShutDownSource()  // PC 0x005DB210  Xbox 0x001546C0
{
    EXOSOUNDSOURCE_TRACE();
    if (m_nListType == 1)
    {
        m_pSound->RemoveFromPlayingList(this);
    }
    else if (m_nListType == LIST_NONPLAYING_LOOPING)
    {
        m_pSound->RemoveFromNonPlayingLoopingList(this);
    }
    else if (m_nListType == 3)
    {
        m_pSound->RemoveFromDelayedOneShotList(this);
    }

    if (m_pSound->m_bInitialized != 0)
    {
        if (m_p3DVoice != 0 && m_p3DVoice->m_hSample != 0)
        {
            AIL_end_3D_sample(m_p3DVoice->m_hSample);
            m_p3DVoice->m_pSource = 0;
            m_p3DVoice = 0;
        }
        else if (m_p2DVoice != 0 && m_p2DVoice->m_hSample != 0)
        {
            AIL_end_sample(m_p2DVoice->m_hSample);
            m_p2DVoice->m_pSource = 0;
            m_p2DVoice = 0;
        }
    }
    if (m_bDemanded != 0)
    {
        m_bDemanded = 0;
        m_pRes->Release();
    }
    m_nListType = LIST_NONE;
    m_pListNode = 0;
}
#endif
// PC 0x005DB2B0  Xbox 0x001547F0
void CExoSoundSourceInternal::Stop()
{
    ShutDownSource();
}

#ifdef _XBOX
void CExoSoundSourceInternal::SetPosition(const Vector& vPosition)  // PC 0x005DB2C0  Xbox 0x00154800
{
    m_vPosition = vPosition;
    if (m_b3D != 0)
    {
        if (IsPlaying() != 0)
        {
            m_p3DVoice->m_pBuffer->SetPosition(vPosition.m_fX, vPosition.m_fZ, vPosition.m_fY, DS3D_DEFERRED);
        }
    }
}
#else
void CExoSoundSourceInternal::SetPosition(const Vector& vPosition)  // PC 0x005DB2C0  Xbox 0x00154800
{
    m_vPosition = vPosition;
    if (m_b3D != 0)
    {
        if (IsPlaying() != 0)
        {
            AIL_set_3D_position(m_p3DVoice->m_hSample, vPosition.m_fX,
                                vPosition.m_fY, vPosition.m_fZ * -1.0f);
        }
    }
}
#endif
// PC 0x005DB320  Xbox 0x00154850
void CExoSoundSourceInternal::Set3D(int b3D)
{
    EXOSOUNDSOURCE_TRACE();
    if (m_nListType != 1 && m_p3DVoice == 0 && m_p2DVoice == 0)
    {
        m_b3D = b3D;
    }
}

#ifdef _XBOX
// PC 0x0063E7F0  Xbox 0x00154870
int CExoSoundSourceInternal::IsPaused()
{
    DWORD nPosition = 0;
    LPDIRECTSOUNDBUFFER pBuffer;

    if (m_pSound->m_bInitialized == 0)
    {
        return 0;
    }
    if (m_p3DVoice != 0 && m_p3DVoice->m_pSource != 0)
    {
        pBuffer = m_p3DVoice->m_pBuffer;
    }
    else if (m_p2DVoice != 0 && m_p2DVoice->m_pSource != 0)
    {
        pBuffer = m_p2DVoice->m_pBuffer;
    }
    else
    {
        return 0;
    }
    if (m_nRestartPosition != 0)
    {
        return 1;
    }
    pBuffer->GetCurrentPosition(&nPosition, NULL);
    if (!IsPlaying() && (nPosition == 0 || nPosition == m_nDataSize))
    {
        return 1;
    }
    return 0;
}

#endif
void CExoSoundSourceInternal::SetOneShotDelay(unsigned long nDelay)  // PC 0x005DB340  Xbox 0x001548E0
{
    EXOSOUNDSOURCE_TRACE();
    if (m_pSound->m_bInitialized != 0)
    {
        m_nOneShotDelay = nDelay;
        m_nDelayStart = g_pExoBase->m_pExoTimers->GetLowResolutionTimer();
        m_pSound->AddToDelayedOneShotList(this);
    }
}

#ifndef _XBOX
void CExoSoundSourceInternal::StoreRestartSoundPosition()  // PC 0x005DB380
{
    EXOSOUNDSOURCE_TRACE();
    if (m_pSound->m_bInitialized != 0)
    {
        if (m_b3D != 0)
        {
            m_nRestartPosition = AIL_3D_sample_offset(m_p3DVoice->m_hSample);
        }
        else
        {
            m_nRestartPosition = AIL_sample_position(m_p2DVoice->m_hSample);
        }
    }
    else
    {
        m_nRestartPosition = 0;
    }
}
#endif

#ifdef _XBOX
unsigned int CExoSoundSourceInternal::Play(int bNoInterrupt, int bResume)  // PC 0x005DB4D0  Xbox 0x00154D20
{
    m_bRetryPlay = 0;
    m_nOneShotDelay = 0;
    m_nDelayStart = 0;
    if (m_pSound->m_bInitialized == 0)
    {
        if (m_bPlayWhenFree != 0)
        {
            m_bRetryPlay = 1;
        }
        m_nDuration = 0;
        return 0;
    }
    SetVolume(m_nVolume, 0, 1.0f);
    if (GetWithinRangeOfListener() == 0)
    {
        if (m_bLooping != 0)
        {
            if (m_nListType == LIST_NONE)
            {
                m_pSound->AddToNonPlayingLoopingList(this);
                return 1;
            }
            m_pData = 0;
        }
        else if (m_bPlayWhenFree != 0)
        {
            m_nDuration = 0;
            m_bRetryPlay = 1;
            return 0;
        }
        return 1;
    }
    if (bResume == 0)
    {
        m_nRestartPosition = 0;
    }
    if (m_p3DVoice != 0 && m_p3DVoice->m_pSource != 0)
    {
        m_p3DVoice->m_pBuffer->SetCurrentPosition(m_nRestartPosition);
        if (m_bLooping != 0)
        {
            m_p3DVoice->m_pBuffer->Play(0, 0, DSBPLAY_LOOPING);
        }
        else
        {
            m_p3DVoice->m_pBuffer->Play(0, 0, 0);
        }
        return m_nDuration;
    }
    if (m_p2DVoice != 0 && m_p2DVoice->m_pBuffer != 0)
    {
        m_p2DVoice->m_pBuffer->SetCurrentPosition(m_nRestartPosition);
        if (m_bLooping != 0)
        {
            m_p2DVoice->m_pBuffer->Play(0, 0, DSBPLAY_LOOPING);
        }
        else
        {
            m_p2DVoice->m_pBuffer->Play(0, 0, 0);
        }
        return m_nDuration;
    }
    if (m_nDuration == 0 && m_bDemanded == 0)
    {
        if (Demand() == 0)
        {
            if (m_bPlayWhenFree != 0)
            {
                m_bRetryPlay = 1;
            }
            m_nDuration = 0;
            m_nDataSize = 0;
            return 0;
        }
        if (m_pData == 0)
        {
            m_pData = 0;
            m_nDataSize = m_pSound->ParseWaveData(m_pRes->GetData(), &m_wfxFormat, &m_pData);
        }
        if (m_nDataSize == 0)
        {
            m_nDuration = 0;
            return 0;
        }
        unsigned long nRate = m_wfxFormat.wfx.nSamplesPerSec;
        unsigned short nChannels = m_wfxFormat.wfx.nChannels;
        m_nDuration = (int)((double)m_nDataSize * 1000.0 /
                            ((double)m_wfxFormat.wfx.wBitsPerSample * (double)nChannels * (double)nRate * 0.125));
        if (m_b3D != 0 && nChannels != 1)
        {
            ReleaseDemand();
            if (m_bPlayWhenFree != 0)
            {
                m_bRetryPlay = 1;
                m_nDuration = 0;
            }
            return m_nDuration;
        }
        m_nSampleRate = nRate;
        m_nPitchRate = nRate;
        if (m_fPitchVariance != 0.0f)
        {
            CalculatePitchVarianceFrequency();
        }
        if (m_fFixedVariance != 0.0f)
        {
            CalculateFixedVarianceFrequency();
        }
    }

    unsigned int nDuration = m_nDuration;
    CExoSoundPriorityGroup* pGroup = m_pSound->m_pPriorityGroups + m_nPriorityGroup;
    if (pGroup->m_nPlaying >= pGroup->m_nMaxPlaying)
    {
        if (bNoInterrupt != 0 || pGroup->m_bInterrupt == 0)
        {
            ReleaseDemand();
            m_pData = 0;
            if (m_bLooping != 0)
            {
                if (m_nListType == LIST_NONE)
                {
                    m_pSound->AddToNonPlayingLoopingList(this);
                }
            }
            else if (m_bPlayWhenFree != 0)
            {
                nDuration = 0;
                m_bRetryPlay = 1;
            }
            return nDuration;
        }
        m_pSound->InterruptPlayingSound(m_nPriorityGroup);
    }
    if (m_b3D != 0)
    {
        C3DVoice* p3DVoice = m_pSound->GetFree3DVoice(m_nPriorityGroup);
        if (p3DVoice == 0)
        {
            ReleaseDemand();
            m_pData = 0;
            if (m_bLooping != 0)
            {
                if (m_nListType == LIST_NONE)
                {
                    m_pSound->AddToNonPlayingLoopingList(this);
                }
            }
            else if (m_bPlayWhenFree != 0)
            {
                nDuration = 0;
                m_bRetryPlay = 1;
            }
            return nDuration;
        }
        if (m_bDemanded == 0 && Demand() == 0)
        {
            if (m_bPlayWhenFree != 0)
            {
                nDuration = 0;
                m_nDuration = 0;
                m_bRetryPlay = 1;
            }
            return nDuration;
        }
        m_pSound->PlaySourceOn3DVoice(this, p3DVoice);
    }
    else
    {
        C2DVoice* p2DVoice = m_pSound->GetFree2DVoice(m_nPriorityGroup);
        if (p2DVoice == 0)
        {
            ReleaseDemand();
            m_pData = 0;
            if (m_bLooping != 0)
            {
                if (m_nListType == LIST_NONE)
                {
                    m_pSound->AddToNonPlayingLoopingList(this);
                }
            }
            else if (m_bPlayWhenFree != 0)
            {
                nDuration = 0;
                m_bRetryPlay = 1;
            }
            return nDuration;
        }
        if (m_bDemanded == 0 && Demand() == 0)
        {
            if (m_bPlayWhenFree != 0)
            {
                nDuration = 0;
                m_nDuration = 0;
                m_bRetryPlay = 1;
            }
            return nDuration;
        }
        m_pSound->PlaySourceOn2DVoice(this, p2DVoice);
    }
    if (m_nListType == LIST_NONPLAYING_LOOPING)
    {
        m_pSound->RemoveFromNonPlayingLoopingList(this);
    }
    m_pSound->AddToPlayingList(this);
    if (g_bSoundPaused != 0 && m_pSound->GetPauseable(this) != 0)
    {
        Pause();
    }
    return nDuration;
}
#else
unsigned int CExoSoundSourceInternal::Play(int bNoInterrupt, int bResume)  // PC 0x005DB4D0  Xbox 0x00154D20
{
    CExoString sName;
    AILSOUNDINFO info;

    m_bRetryPlay = 0;
    m_nOneShotDelay = 0;
    m_nDelayStart = 0;
    if (m_pSound->m_bInitialized == 0)
    {
        if (m_bPlayWhenFree != 0)
        {
            m_bRetryPlay = 1;
        }
        return m_nDuration;
    }
    int bLooping = m_bLooping;
    int nLoopCount = bLooping == 0;
    if (bResume == 0)
    {
        m_nRestartPosition = 0;
    }
    if (m_p3DVoice != 0 && m_p3DVoice->m_hSample != 0)
    {
        AIL_stop_3D_sample(m_p3DVoice->m_hSample);
        AIL_set_3D_sample_loop_count(m_p3DVoice->m_hSample, nLoopCount);
        AIL_set_3D_sample_offset(m_p3DVoice->m_hSample, 0);
        AIL_resume_3D_sample(m_p3DVoice->m_hSample);
        return m_nDuration;
    }
    if (m_p2DVoice != 0 && m_p2DVoice->m_hSample != 0)
    {
        AIL_stop_sample(m_p2DVoice->m_hSample);
        AIL_set_sample_loop_count(m_p2DVoice->m_hSample, nLoopCount);
        AIL_set_sample_position(m_p2DVoice->m_hSample, 0);
        AIL_resume_sample(m_p2DVoice->m_hSample);
        return m_nDuration;
    }
    if (m_nDuration == 0 && m_bDemanded == 0)
    {
        if (Demand() == 0)
        {
            if (m_bPlayWhenFree != 0)
            {
                m_bRetryPlay = 1;
            }
            return m_nDuration;
        }

        if (m_pRes != 0)
        {
            sName.CStr();
            m_pRes->GetDemands();
        }
        AIL_WAV_info(m_pRes->GetData(), &info);
        unsigned long nRate = info.rate;
        unsigned long nChannels = info.channels;
        double dDuration = 8.0 / (double)(nRate * info.bits);
        dDuration *= (double)(info.data_len / nChannels);
        dDuration *= 1000.0;
        m_nDuration = (unsigned int)dDuration;
        if (m_b3D != 0 && nChannels != 1)
        {
            ReleaseDemand();
            if (m_bPlayWhenFree != 0)
            {
                m_bRetryPlay = 1;
            }
            return m_nDuration;
        }
        m_nSampleRate = nRate;
        m_nPitchRate = nRate;
        if (m_fPitchVariance != 0.0f)
        {
            CalculatePitchVarianceFrequency();
        }
        if (m_fFixedVariance != 0.0f)
        {
            CalculateFixedVarianceFrequency();
        }
    }
    if (GetWithinRangeOfListener() == 0)
    {
        ReleaseDemand();
        if (m_bLooping != 0)
        {
            if (m_nListType == LIST_NONE)
            {
                m_pSound->AddToNonPlayingLoopingList(this);
            }
            return m_nDuration;
        }
        if (m_bPlayWhenFree != 0)
        {
            m_bRetryPlay = 1;
        }
        return m_nDuration;
    }
    CExoSoundPriorityGroup* pGroup = m_pSound->m_pPriorityGroups + m_nPriorityGroup;
    if (pGroup->m_nPlaying >= pGroup->m_nMaxPlaying)
    {
        if (bNoInterrupt != 0 || pGroup->m_bInterrupt == 0)
        {
            ReleaseDemand();
            if (m_bLooping != 0)
            {
                if (m_nListType == LIST_NONE)
                {
                    m_pSound->AddToNonPlayingLoopingList(this);
                }
            }
            else if (m_bPlayWhenFree != 0)
            {
                m_bRetryPlay = 1;
            }
            return m_nDuration;
        }
        unsigned char nPriorityGroup = m_nPriorityGroup;
        m_pSound->InterruptPlayingSound(nPriorityGroup);
    }
    int b3D = m_b3D;
    CExoSoundInternal* pSound = m_pSound;
    if (b3D != 0)
    {
        unsigned char nPriorityGroup = m_nPriorityGroup;
        C3DVoice* p3DVoice =
            pSound->GetFree3DVoice(nPriorityGroup);
        if (p3DVoice == 0)
        {
            ReleaseDemand();
            if (m_bLooping != 0)
            {
                if (m_nListType == LIST_NONE)
                {
                    m_pSound->AddToNonPlayingLoopingList(this);
                }
            }
            else if (m_bPlayWhenFree != 0)
            {
                m_bRetryPlay = 1;
            }
            return m_nDuration;
        }
        if (m_bDemanded == 0 && Demand() == 0)
        {
            if (m_bPlayWhenFree != 0)
            {
                m_bRetryPlay = 1;
            }
            return m_nDuration;
        }
        m_pSound->PlaySourceOn3DVoice(this, p3DVoice);
    }
    else
    {
        unsigned char nPriorityGroup = m_nPriorityGroup;
        C2DVoice* p2DVoice =
            pSound->GetFree2DVoice(nPriorityGroup);
        if (p2DVoice == 0)
        {
            ReleaseDemand();
            if (m_bLooping != 0)
            {
                if (m_nListType == LIST_NONE)
                {
                    m_pSound->AddToNonPlayingLoopingList(this);
                }
            }
            else if (m_bPlayWhenFree != 0)
            {
                m_bRetryPlay = 1;
            }
            return m_nDuration;
        }
        if (m_bDemanded == 0 && Demand() == 0)
        {
            if (m_bPlayWhenFree != 0)
            {
                m_bRetryPlay = 1;
            }
            return m_nDuration;
        }
        m_pSound->PlaySourceOn2DVoice(this, p2DVoice);
    }
    if (m_nListType == LIST_NONPLAYING_LOOPING)
    {
        m_pSound->RemoveFromNonPlayingLoopingList(this);
    }
    m_pSound->AddToPlayingList(this);
    if (m_pSound->m_bPaused != 0 ||
        (g_bSoundPaused != 0 && m_pSound->GetPauseable(this) != 0))
    {
        Pause();
    }
    return m_nDuration;
}
#endif

#ifdef _XBOX
void CExoSoundSourceInternal::SetVolume(unsigned char nVolume, int nSliderMode,  // PC 0x005DB800  Xbox 0x00154AD0
                                        float fScale)
{
    EXOSOUNDSOURCEINTERNAL_TRACE();
    if (nVolume > 127)
    {
        nVolume = 127;
    }
    int nMode = nSliderMode;
    if (nSliderMode == 0)
    {
        m_nVolume = nVolume;
    }
    int nSliderType = m_pSound->GetSliderType(this);
    if (m_pSound->GetMutable(this) != 0 && nSliderMode == 0)
    {
        float fFirst = m_pSound->GetSliderVolume(nSliderType, 1);
        nMode = m_pSound->GetSliderVolume(nSliderType, 0) != fFirst;
    }
    float fSlider = m_pSound->GetSliderVolume(nSliderType, nMode);
    float fVolumeScale = !IsScalable() ? 1.0f : fScale;
    float fVolume = (float)m_nGroupVolume * (float)nVolume * fSlider * fVolumeScale * (1.0f / 127.0f);
    long nMillibels = ExoSoundVolumeToMillibels((int)fVolume, EXOSOUND_MIN_VOLUME);
    if (m_pSound->m_bInitialized != 0)
    {
        if (m_b3D != 0)
        {
            if (m_p3DVoice != 0 && m_p3DVoice->m_pBuffer != 0)
            {
                m_p3DVoice->m_pBuffer->SetVolume(nMillibels);
            }
        }
        else if (m_p2DVoice != 0 && m_p2DVoice->m_pBuffer != 0)
        {
            m_p2DVoice->m_pBuffer->SetVolume(nMillibels);
        }
    }
}
#else
void CExoSoundSourceInternal::SetVolume(unsigned char nVolume, int nSliderMode,  // PC 0x005DB800  Xbox 0x00154AD0
                                        float fScale)
{
    EXOSOUNDSOURCEINTERNAL_TRACE();
    int nMode = nSliderMode;
    int nSliderType = m_pSound->GetSliderType(this);
    if (nSliderMode == 0)
    {
        m_nVolume = nVolume;
    }
    if (m_pSound->GetMutable(this) == 0 && nSliderMode == 0)
    {
        float fFirst = m_pSound->GetSliderVolume(nSliderType, 1);
        nMode = m_pSound->GetSliderVolume(nSliderType, 0) != fFirst;
    }
    float fSlider = m_pSound->GetSliderVolume(nSliderType, nMode);
    float fVolumeScale = !IsScalable() ? 1.0f : fScale;
    int nBase =
        (int)((float)m_nGroupVolume * nVolume * fVolumeScale * (1.0f / 127.0f));
    CExoSoundInternal* pSound = m_pSound;
    if (pSound->m_bInitialized != 0)
    {
        if (m_b3D != 0)
        {
            if (m_p3DVoice != 0 && m_p3DVoice->m_hSample != 0)
            {
                if (pSound->m_f2D3DBias < 1.0f)
                {
                    fSlider = fSlider * pSound->m_f2D3DBias;
                }
                float fLevel =
                    pow((float)nBase * fSlider * (1.0f / 127.0f), 0.6f);
                AIL_set_3D_sample_volume(m_p3DVoice->m_hSample, fLevel);
            }
        }
        else if (m_p2DVoice != 0 && m_p2DVoice->m_hSample != 0)
        {
            if (pSound->m_f2D3DBias > 1.0f)
            {
                fSlider = (2.0f - pSound->m_f2D3DBias) * fSlider;
            }
            float fLevel = (float)nBase * fSlider * (1.0f / 127.0f);
            AIL_set_sample_volume_levels(m_p2DVoice->m_hSample, fLevel, fLevel);
        }
    }
}
#endif

int CExoSoundSourceInternal::IsScalable()
{
    return m_nPriorityGroup == 4 || (m_nPriorityGroup == 21 && m_p2DVoice != 0);
}

#ifdef _XBOX
int CExoSoundSourceInternal::SetFixedVariance(float fVariance)  // PC 0x005DB980  Xbox 0x00154BE0
{
    if (fVariance <= 2.0f && fVariance >= 0.5f)
    {
        CExoSoundInternal* pSound = m_pSound;
        m_fFixedVariance = fVariance;
        m_fPitchVariance = 0.0f;
        if (pSound->m_bInitialized != 0)
        {
            Pause();
            if (m_p3DVoice != 0 && m_p3DVoice->m_pSource != 0)
            {
                CalculateFixedVarianceFrequency();
                m_p3DVoice->m_pBuffer->SetFrequency(m_nFixedRate);
                Resume();
                return 1;
            }
            if (m_p2DVoice != 0 && m_p2DVoice->m_pSource != 0)
            {
                CalculateFixedVarianceFrequency();
                m_p2DVoice->m_pBuffer->SetFrequency(m_nFixedRate);
            }
            Resume();
        }
        return 1;
    }
    return 0;
}
#else
int CExoSoundSourceInternal::SetFixedVariance(float fVariance)  // PC 0x005DB980  Xbox 0x00154BE0
{
    if (fVariance <= 2.0f && fVariance >= 0.5f)
    {
        CExoSoundInternal* pSound = m_pSound;
        m_fFixedVariance = fVariance;
        m_fPitchVariance = 0.0f;
        if (pSound->m_bInitialized != 0)
        {
            Pause();
            if (m_p3DVoice != 0 && m_p3DVoice->m_hSample != 0)
            {
                CalculateFixedVarianceFrequency();
                AIL_set_3D_sample_playback_rate(m_p3DVoice->m_hSample,
                                                m_nFixedRate);
                Resume();
                return 1;
            }
            if (m_p2DVoice != 0 && m_p2DVoice->m_hSample != 0)
            {
                CalculateFixedVarianceFrequency();
                AIL_set_sample_playback_rate(m_p2DVoice->m_hSample,
                                             m_nFixedRate);
            }
            Resume();
        }
        return 1;
    }
    return 0;
}
#endif

#ifdef _XBOX
// PC 0x005DBB20  Xbox 0x001550F0
void CExoSoundSourceInternal::SetPriorityGroup(unsigned char nPriorityGroup)
{
    CExoSoundInternal* pSound = m_pSound;
    if (pSound->m_bInitialized != 0)
    {
        CExoSoundPriorityGroup* pGroup =
            &pSound->m_pPriorityGroups[nPriorityGroup];
        if (pGroup != NULL)
        {
            m_nPriorityGroup = nPriorityGroup;
        }
        else
        {
            pGroup = &pSound->m_pPriorityGroups[m_nPriorityGroup];
        }
        m_nGroupVolume = pGroup->m_nVolume;
        SetVolume(m_nVolume, 0, 1.0f);
        float fPitchVariance = pGroup->m_fPitchVariance;
        SetPitchVariance(fPitchVariance);
        float fMinVolumeDist = pGroup->m_fMinVolumeDist;
        float fMaxVolumeDist = pGroup->m_fMaxVolumeDist;
        m_fMaxVolumeDist = fMaxVolumeDist;
        m_fMinVolumeDist = fMinVolumeDist;
    }
}
#else
void CExoSoundSourceInternal::SetPriorityGroup(unsigned char nPriorityGroup)  // PC 0x005DBB20  Xbox 0x001550F0
{
    CExoSoundInternal* pSound = m_pSound;
    if (pSound->m_bInitialized != 0)
    {
        m_nPriorityGroup = nPriorityGroup;
        CExoSoundPriorityGroup* pGroup =
            &pSound->m_pPriorityGroups[nPriorityGroup];
        m_nGroupVolume = pGroup->m_nVolume;
        SetVolume(m_nVolume, 0, 1.0f);
        float fPitchVariance = pGroup->m_fPitchVariance;
        SetPitchVariance(fPitchVariance);
        float fMinVolumeDist = pGroup->m_fMinVolumeDist;
        float fMaxVolumeDist = pGroup->m_fMaxVolumeDist;
        m_fMaxVolumeDist = fMaxVolumeDist;
        m_fMinVolumeDist = fMinVolumeDist;
    }
}
#endif

#ifdef _XBOX
void CExoSoundSourceInternal::SetPitchVariance(float fVariance)  // PC 0x005DBA40  Xbox 0x00154C70
{
    float fPitchVariance = min(1.0f, max(0.0f, fVariance));

    CExoSoundInternal* pSound = m_pSound;
    m_fPitchVariance = fPitchVariance;
    m_fFixedVariance = 0.0f;
    if (pSound->m_bInitialized != 0)
    {
        Pause();
        if (m_p3DVoice != 0 && m_p3DVoice->m_pSource != 0)
        {
            CalculatePitchVarianceFrequency();
            m_p3DVoice->m_pBuffer->SetFrequency(m_nPitchRate);
            Resume();
            return;
        }
        if (m_p2DVoice != 0 && m_p2DVoice->m_pSource != 0)
        {
            CalculatePitchVarianceFrequency();
            m_p2DVoice->m_pBuffer->SetFrequency(m_nPitchRate);
        }
        Resume();
    }
}
#else
void CExoSoundSourceInternal::SetPitchVariance(float fVariance)  // PC 0x005DBA40  Xbox 0x00154C70
{
    float fPitchVariance = min(1.0f, max(0.0f, fVariance));

    CExoSoundInternal* pSound = m_pSound;
    m_fPitchVariance = fPitchVariance;
    m_fFixedVariance = 0.0f;
    if (pSound->m_bInitialized != 0)
    {
        Pause();
        if (m_p3DVoice != 0 && m_p3DVoice->m_hSample != 0)
        {
            CalculatePitchVarianceFrequency();
            AIL_set_3D_sample_playback_rate(m_p3DVoice->m_hSample,
                                            m_nPitchRate);
            Resume();
            return;
        }
        if (m_p2DVoice != 0 && m_p2DVoice->m_hSample != 0)
        {
            CalculatePitchVarianceFrequency();
            AIL_set_sample_playback_rate(m_p2DVoice->m_hSample,
                                         m_nPitchRate);
        }
        Resume();
    }
}
#endif
// PC 0x005DB3D0  Xbox 0x00154920
void CExoSoundSourceInternal::CalculatePitchVarianceFrequency()
{
    EXOSOUNDSOURCE_TRACE();
    if (rand() % 2 != 0)
    {
        float fVariance = m_fPitchVariance;
        unsigned long nRange = (unsigned long)(fVariance * m_nSampleRate);
        if (nRange != 0)
        {
            m_nPitchRate = rand() % nRange + m_nPitchRate;
        }
    }
    else
    {
        float fVariance = m_fPitchVariance;
        unsigned long nRange =
            (unsigned long)(fVariance * m_nSampleRate * 0.5f);
        if (nRange != 0)
        {
            m_nPitchRate -= rand() % nRange;
        }
    }
}

#ifdef _XBOX
void CExoSoundSourceInternal::CalculateFixedVarianceFrequency()  // PC 0x005DB450  Xbox 0x001549A0
{
    EXOSOUNDSOURCE_TRACE();
    m_nFixedRate = (unsigned long)((float)m_nSampleRate * m_fFixedVariance);
}
#else
void CExoSoundSourceInternal::CalculateFixedVarianceFrequency()  // PC 0x005DB450  Xbox 0x001549A0
{
    EXOSOUNDSOURCE_TRACE();
    double dVariance = m_fFixedVariance;
    m_nFixedRate = (unsigned long)(dVariance * (float)m_nSampleRate);
}
#endif

static inline void ExoSoundSourceTrace()
{
}

static inline void ExosoundsourceinternalTrace2()
{
    ExosoundsourceinternalTrace1();
}

static inline void ExosoundsourceinternalTrace1()
{
}
