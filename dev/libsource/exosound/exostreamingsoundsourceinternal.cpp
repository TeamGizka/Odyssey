#include <io.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "appmanager.h"
#include "clientexoapp.h"
#include "exobase.h"
#include "exosoundinternal.h"
#include "math3d.h"
#include "mss.h"

static inline void ExostreamingsoundsourceinternalTrace1();
// PC 0x004015A0  Xbox 0x001567B0
void CExoStreamingSoundSourceInternal::DoObstructionWork()
{
}
// PC 0x0063E7F0  Xbox 0x00155D80
int CExoStreamingSoundSourceInternal::GetPlayFailed()
{
    return 0;
}
// PC 0x005DBB90
int CExoStreamingSoundSourceInternal::SetReverbEnabled(int bEnabled)
{
    int nResult;

    nResult = bEnabled;
    if (m_bReverbEnabled != bEnabled)
    {
        m_bReverbEnabled = bEnabled;
        m_bReverbChanged = 1;
    }
    return nResult;
}
// PC 0x005DBBB0
inline Vector CExoSoundInternal::GetListenerOrientation()
{
    return m_vListenerOrientation;
}
// PC 0x005DBBE0  Xbox 0x00155DA0
CExoStreamingSoundSourceInternal::CExoStreamingSoundSourceInternal(
    CExoSoundInternal* pSoundInternal)
{
    int nSourceId;

    m_pSoundInternal = pSoundInternal;
    m_bLooping = 0;
    m_hStream = NULL;
    m_nPausePosition = 0;
    m_nFadeDuration = 0;
    m_nFadeStartTime = 0;
    m_bPlaying = 0;
    m_pFile = NULL;
    m_nVolume = CExoSound::CMAX_VOLUME;
    nSourceId = pSoundInternal->m_nNextSourceId;
    pSoundInternal->m_nNextSourceId = nSourceId + 1;
    m_bReverbEnabled = 1;
    m_bReverbChanged = 1;
    m_nSourceId = nSourceId;
    m_b3D = 0;
    m_fMinDistance = 0.0f;
    m_fMaxDistance = 0.0f;
    m_fVolume = -1.0f;
    m_nPriorityGroup = CExoSoundPriorityGroup::MUSIC_GROUP;
    m_nPriorityVolume =
        m_pSoundInternal->m_pPriorityGroups[m_nPriorityGroup].m_nVolume;
    m_vObstructionListenerPosition = Vector();
    m_nObstructionTime = 0;
    m_fOcclusion = 0.0f;
}
// PC 0x005DBCA0  Xbox 0x00155E90
CExoStreamingSoundSourceInternal::~CExoStreamingSoundSourceInternal()
{
    ExostreamingsoundsourceinternalTrace1();
    int bPlaying;
    CExoSoundInternal* pSound;

    bPlaying = m_bPlaying;
    pSound = m_pSoundInternal;
    if (bPlaying != 0)
    {
        pSound->RemoveFromStreamingList(this);
    }
    else
    {
        pSound->RemoveFromNonPlayingStreamingLoopingList(this);
    }
    if (m_pSoundInternal->m_bInitialized != 0 && m_hStream != NULL)
    {
        AIL_close_stream(m_hStream);
        if (m_pFile != NULL)
        {
            fclose(m_pFile);
        }
        m_hStream = NULL;
    }
}
// PC 0x005DBD30
void CExoStreamingSoundSourceInternal::InitializeSource()
{
    ExostreamingsoundsourceinternalTrace1();
    char szHeader[5];
    if (g_bModifiedSound != 0)
    {
        CExoSoundInternal* pSound = m_pSoundInternal;
        int bInitialized = pSound->m_bInitialized;
        HDIGDRIVER hDig;
        if (bInitialized == 0)
        {
            return;
        }
        if (m_pFile != NULL)
        {
            fclose(m_pFile);
        }
        m_pFile = NULL;
        if (m_nResourceType == RESTYPE_MP3)
        {
            CExoString sWaveFile = m_sFileName.Left(m_sFileName.GetLength() - 3) + "wav";
            int nSize;
            int bEmbeddedMP3 = 0;

            m_pFile = fopen(sWaveFile.CStr(), "rb");
            if (m_pFile != NULL)
            {
                fread(szHeader, 1, 4, m_pFile);
                if (strncmp(szHeader, "RIFF", 4) != 0)
                {
                    fclose(m_pFile);
                    m_pFile = NULL;
                }
                else
                {
                    int nRiffSize;
                    fread(&nRiffSize, 4, 1, m_pFile);
                    int nHeaderSize = nRiffSize + 8;
                    fseek(m_pFile, 0, SEEK_END);
                    nSize = ftell(m_pFile);
                    if (nHeaderSize != nSize)
                    {
                        fseek(m_pFile, nHeaderSize, SEEK_SET);
                        nSize -= nHeaderSize;
                        bEmbeddedMP3 = 1;
                    }
                    else
                    {
                        fclose(m_pFile);
                        m_pFile = NULL;
                    }
                }
            }
            if (bEmbeddedMP3 == 0)
            {
                m_pFile = fopen(m_sFileName.CStr(), "rb");
                if (m_pFile == NULL)
                {
                    return;
                }
                fseek(m_pFile, 0, SEEK_END);
                nSize = ftell(m_pFile);
                fseek(m_pFile, 0, SEEK_SET);
            }
            int nFile = m_pFile->_file;
            intptr_t nHandle = _get_osfhandle(nFile);
            CExoString sStream;
            AIL_quick_handles(&hDig, NULL, NULL);
            sStream.Format("\\\\\\\\%d,%d", nHandle, nSize);
            if (m_nResourceType == RESTYPE_MP3)
            {
                sStream = sStream + ".mp3";
            }
            m_hStream = AIL_open_stream(hDig, sStream.CStr(), 0);
        }
        else
        {
            int nSize;
            m_pFile = fopen(m_sFileName.CStr(), "rb");
            if (m_pFile == NULL)
            {
                return;
            }
            fread(szHeader, 1, 4, m_pFile);
            if (strncmp(szHeader, "RIFF", 4) != 0)
            {
                fseek(m_pFile, LEGACY_WAVE_HEADER_SIZE, SEEK_SET);
                fread(szHeader, 1, 4, m_pFile);
                if (strncmp(szHeader, "RIFF", 4) != 0)
                {
                    fclose(m_pFile);
                    m_pFile = NULL;
                    return;
                }
                fseek(m_pFile, 0, SEEK_END);
                nSize = ftell(m_pFile) - LEGACY_WAVE_HEADER_SIZE;
                fseek(m_pFile, LEGACY_WAVE_HEADER_SIZE, SEEK_SET);
            }
            else
            {
                fseek(m_pFile, 0, SEEK_END);
                nSize = ftell(m_pFile);
                fseek(m_pFile, 0, SEEK_SET);
            }
            int nFile = m_pFile->_file;
            intptr_t nHandle = _get_osfhandle(nFile);
            CExoString sStream;
            AIL_quick_handles(&hDig, NULL, NULL);
            sStream.Format("\\\\\\\\%d,%d", nHandle, nSize);
            if (m_nResourceType == RESTYPE_WAV)
            {
                sStream = sStream + ".wav";
            }
            m_hStream = AIL_open_stream(hDig, sStream.CStr(), 0);
        }
    }
    else
    {
        CExoSoundInternal* pSound = m_pSoundInternal;
        int bInitialized = pSound->m_bInitialized;
        if (bInitialized != 0)
        {
        m_pFile = NULL;
        HDIGDRIVER hDig;
        if (m_nResourceType == RESTYPE_MP3)
        {
            m_pFile = fopen(m_sFileName.CStr(), "rb");
            if (m_pFile == NULL)
            {
                return;
            }
            fseek(m_pFile, 0, SEEK_END);
            int nSize = ftell(m_pFile);
            fseek(m_pFile, 0, SEEK_SET);
            int nFile = m_pFile->_file;
            intptr_t nHandle = _get_osfhandle(nFile);
            CExoString sStream;
            AIL_quick_handles(&hDig, NULL, NULL);
            sStream.Format("\\\\\\\\%d,%d", nHandle, nSize);
            if (m_nResourceType == RESTYPE_MP3)
            {
                sStream = sStream + ".mp3";
            }
            m_hStream = AIL_open_stream(hDig, sStream.CStr(), 0);
        }
        else
        {
            AIL_quick_handles(&hDig, NULL, NULL);
            m_hStream = AIL_open_stream(hDig, m_sFileName.CStr(), 0);
        }
    }
    }
}
// PC 0x005DC300  Xbox 0x00155D70
int CExoStreamingSoundSourceInternal::IsPlaying()
{
    ExostreamingsoundsourceinternalTrace1();
    if (m_pSoundInternal->m_bInitialized != 0 && m_hStream != NULL)
    {
        return AIL_stream_status(m_hStream) - SMP_PLAYING == 0;
    }
    return 0;
}
// PC 0x005DC330  Xbox 0x00155FA0
void CExoStreamingSoundSourceInternal::SetLooping(int bLooping)
{
    int nLoopCount;

    m_bLooping = bLooping;
    if (IsPlaying())
    {
        if (m_bLooping == 0)
        {
            nLoopCount = 1;
        }
        else
        {
            nLoopCount = 0;
        }
        AIL_set_stream_loop_count(m_hStream, nLoopCount);
    }
}

int CExoStreamingSoundSourceInternal::IsScalable()
{
    return m_nPriorityGroup == 4 || m_nPriorityGroup == 21;
}
// PC 0x005DC380  Xbox 0x001562E0
void CExoStreamingSoundSourceInternal::Stop()
{
    ExostreamingsoundsourceinternalTrace1();
    CExoSoundInternal* pSound;

    if (m_hStream != NULL)
    {
        AIL_pause_stream(m_hStream, 1);
    }
    pSound = m_pSoundInternal;
    m_nFadeDuration = 0;
    m_nFadeStartTime = 0;
    m_nPausePosition = 0;
    if (m_bPlaying != 0)
    {
        pSound->RemoveFromStreamingList(this);
    }
    else
    {
        pSound->RemoveFromNonPlayingStreamingLoopingList(this);
    }
}
// PC 0x005DC3C0  Xbox 0x001564D0
void CExoStreamingSoundSourceInternal::Pause()
{
    CExoSoundInternal* pSound;
    int bPlaying;
    int bInitialized;
    CExoString sResRef;

    m_resRef.CopyToString(sResRef);
    if (sResRef.CompareNoCase("evil_ending") == 0 ||
        g_pAppManager->m_pClientExoApp->GetCreditSequenceInProgress() != 0)
    {
        pSound = m_pSoundInternal;
        bInitialized = pSound->m_bInitialized;
        if (bInitialized != 0 && m_hStream != NULL)
        {
            bPlaying = IsPlaying();
            if (bPlaying != 0)
            {
                AIL_pause_stream(m_hStream, 1);
                m_nPausePosition = AIL_stream_position(m_hStream);
            }
        }
    }
}
// PC 0x005DC480  Xbox 0x001563D0
void CExoStreamingSoundSourceInternal::FadeAndStop(unsigned long nDuration)
{
    if (m_pSoundInternal->m_bInitialized != 0 && IsPlaying())
    {
        m_nFadeStartTime = g_pExoBase->m_pExoTimers->GetLowResolutionTimer();
        m_nFadeDuration = nDuration;
    }
}
// PC 0x005DC4D0  Xbox 0x001569E0
int CExoStreamingSoundSourceInternal::GetWithinRangeOfListener()
{
    if (m_b3D == 0)
    {
        return 1;
    }
    Vector vPosition = m_vPosition;
    Vector* pListenerPosition = &m_pSoundInternal->m_vListenerPosition;
    Vector vListenerPosition = *pListenerPosition;
    return magnitude(vListenerPosition - vPosition) <= m_fMaxDistance;
}
// PC 0x005DC560
void CExoStreamingSoundSourceInternal::Update3DStream()
{
    float fDistance;
    float fVolume;
    float fPan;
    if (Vector(m_vPosition).IsZero())
    {
        return;
    }
    if (m_b3D == 0 || !(m_fVolume > 0.0f) || !(m_fMaxDistance > 0.0f) ||
        !(m_fMinDistance < m_fMaxDistance))
    {
        return;
    }
    if (g_pExoSound->GetSoundMode() == CExoSound::CEXOSOUND_MODE_PAUSE_ALL ||
        g_pExoSound->GetSoundMode() == CExoSound::CEXOSOUND_MODE_PAUSE_ALL_BUT_MUSIC)
    {
        AIL_set_stream_volume_levels(m_hStream, 0.0f, 0.0f);
        return;
    }
    fVolume = m_fVolume;
    Vector vListenerOrientation;
    Vector vDistance;
    Vector vListenerPosition = m_pSoundInternal->m_vListenerPosition;
    vListenerOrientation = m_pSoundInternal->GetListenerOrientation();
    fDistance = magnitude(Vector(m_vPosition) - vListenerPosition);
    if (fDistance > m_fMinDistance)
    {
        fVolume = (m_fMaxDistance - fDistance) /
                  (m_fMaxDistance - m_fMinDistance) * fVolume;
    }
    vDistance = Vector(m_vPosition) - vListenerPosition;
    vDistance.Normalize();
    vListenerOrientation.Normalize();
    float fCosine = vListenerOrientation.m_fX * vDistance.m_fX +
                    vListenerOrientation.m_fY * vDistance.m_fY;
    if (vListenerOrientation.m_fY > 0.0f ?
        (vDistance.m_fY > 0.0f ? !(vListenerOrientation.m_fX < vDistance.m_fX) :
                               !(vListenerOrientation.m_fX > -vDistance.m_fX)) :
        (vDistance.m_fY < 0.0f ? !(vListenerOrientation.m_fX > vDistance.m_fX) :
                               !(vListenerOrientation.m_fX < -vDistance.m_fX)))
    {
        fPan = (float)fabs(fCosine) * 0.5f;
    }
    else
    {
        fPan = 1.0f - (float)fabs(fCosine) * 0.5f;
    }
    fPan = !(0.05f > fPan) ? (!(0.95f < fPan) ? fPan : 0.95f) : 0.05f;
    if (fCosine < 0.0f)
    {
        fVolume = (fCosine * 0.125f + 1.0f) * fVolume;
    }
    fVolume = (float)pow((1.0f - m_fOcclusion) * fVolume, 0.6f);
    fVolume = !(0.0f > fVolume) ? (!(1.0f < fVolume) ? fVolume : 1.0f) : 0.0f;
    AIL_set_stream_volume_pan(m_hStream, fVolume, fPan);
    if (m_bReverbChanged != 0)
    {
        if (m_bReverbEnabled != 0 &&
            m_pSoundInternal->m_bStreamingEnvironmentEffects != 0)
        {
            float fReverbLevel = m_pSoundInternal->m_fReverbLevel;
            AIL_set_stream_reverb_levels(m_hStream, 1.0f,
                                         fReverbLevel);
        }
        else
        {
            AIL_set_stream_reverb_levels(m_hStream, 1.0f, 0.0f);
        }
        m_bReverbChanged = 0;
    }
}
// PC 0x005DC930  Xbox 0x001561B0
void CExoStreamingSoundSourceInternal::SetVolume(unsigned char nVolume,
                                                int bUnknown, float fScale)
{
    int nSliderVolumeIndex;
    int nSliderType;
    float fVolumeScale;
    float fVolume;
    float fNewVolume;
    int nScaledVolume;

    nSliderVolumeIndex = bUnknown;
    nSliderType = m_pSoundInternal->GetSliderType(this);
    if (!IsScalable())
    {
        fVolumeScale = 1.0;
    }
    else
    {
        fVolumeScale = fScale;
    }
    nScaledVolume = (int)(nVolume * fVolumeScale);
    if (bUnknown == 0)
    {
        m_nVolume = nVolume;
    }
    if (m_pSoundInternal->m_bInitialized != 0 && m_hStream != NULL)
    {
        if (m_pSoundInternal->GetMutable(this) == 0 && bUnknown == 0)
        {
            fVolume = m_pSoundInternal->GetSliderVolume(nSliderType, 1);
            nSliderVolumeIndex =
                m_pSoundInternal->GetSliderVolume(nSliderType, 0) != fVolume;
        }
        fVolume =
            m_pSoundInternal->GetSliderVolume(nSliderType, nSliderVolumeIndex);
        fNewVolume = (float)m_nPriorityVolume * fVolume / 127.0f;
        fNewVolume = fNewVolume * nScaledVolume / 127.0f;
        AIL_set_stream_volume_levels(m_hStream, fNewVolume, fNewVolume);
        m_fVolume = fNewVolume;
        Update3DStream();
    }
}
// PC 0x005DCA30  Xbox 0x00156C60
int CExoStreamingSoundSourceInternal::Play(const CResRef& resRef,
                                           unsigned short nResourceType,
                                           int bVoice, int bMusic, int bUnused)
{
    int nLoopCount;

    if (m_hStream == NULL || resRef != m_resRef)
    {
        CExoString sResRef;

        resRef.CopyToString(sResRef);
        const char* szResRef = sResRef.CStr();
        m_resRef = resRef;
        m_nResourceType = nResourceType;
        resRef.CopyToString(m_sFileName);
        if (bMusic != 0)
        {
            m_sFileName.Format("HD0:STREAMMUSIC\\%s", m_sFileName.CStr());
        }
        else if (bVoice != 0)
        {
            if ((m_sFileName[0] == 'n' || m_sFileName[0] == 'N') &&
                m_sFileName.GetLength() == 16 && m_sFileName[1] != '_')
            {
                int i;
                char szDirectory[8];
                char szSound[8];

                for (i = 0; i < 5; i++)
                {
                    szDirectory[i] = m_sFileName[i + 1];
                }
                szDirectory[i] = 0;
                for (i = 0; i < 6; i++)
                {
                    szSound[i] = m_sFileName[i + 6];
                }
                szSound[i] = 0;
                m_sFileName.Format("HD0:STREAMWAVES\\%s\\%s\\%s", szDirectory,
                                   szSound, m_sFileName.CStr());
            }
            else
            {
                m_sFileName.Format("HD0:STREAMWAVES\\%s", m_sFileName.CStr());
            }
        }
        else
        {
            m_sFileName.Format("HD0:STREAMSOUNDS\\%s", m_sFileName.CStr());
        }
        m_sFileName = g_pExoBase->m_pExoAliasList->ResolveFileName(
            m_sFileName, nResourceType);
        Stop();
        if (m_hStream != NULL)
        {
            AIL_close_stream(m_hStream);
            m_hStream = NULL;
        }
        InitializeSource();
    }
    else
    {
        Stop();
    }
    if (m_pSoundInternal->m_bInitialized != 0 && m_hStream != NULL)
    {
        S32 nDuration;
        float fVolumeScale = m_pSoundInternal->m_fVolumeScale;
        SetVolume(m_nVolume, 1, fVolumeScale);
        nLoopCount = m_bLooping == 0;
        Update3DStream();
        AIL_set_stream_loop_count(m_hStream, nLoopCount);
        m_nPausePosition = 0;
        AIL_stream_ms_position(m_hStream, &nDuration, NULL);
        CExoSoundPriorityGroup* pGroup =
            &m_pSoundInternal->m_pPriorityGroups[m_nPriorityGroup];
        if (pGroup->m_nPlaying >= pGroup->m_nMaxPlaying)
        {
            if (pGroup->m_bInterrupt == 0)
            {
                if (m_bLooping != 0)
                {
                    m_pSoundInternal->AddToNonPlayingStreamingLoopingList(this);
                    return nDuration;
                }
                return 0;
            }
            m_pSoundInternal->InterruptPlayingStream(m_nPriorityGroup);
        }
        if (m_bLooping != 0 && GetWithinRangeOfListener() == 0)
        {
            m_pSoundInternal->AddToNonPlayingStreamingLoopingList(this);
            return nDuration;
        }
        if (m_pSoundInternal->AddToStreamingList(this) == 0)
        {
            return 0;
        }
        AIL_start_stream(m_hStream);
        if (m_pSoundInternal->m_bPaused != 0 ||
            (g_bSoundPaused != 0 && m_pSoundInternal->GetPauseable(this) != 0))
        {
            Pause();
        }
        return nDuration;
    }
    return 0;
}
// PC 0x005DCD50  Xbox 0x00156890
void CExoStreamingSoundSourceInternal::SetPriorityGroup(unsigned char nGroup)
{
    float fMaxDistance;
    float fMinDistance;
    CExoSoundPriorityGroup* pGroup =
        &m_pSoundInternal->m_pPriorityGroups[nGroup];

    if (pGroup != NULL)
    {
        m_nPriorityGroup = nGroup;
    }
    else
    {
        pGroup = &m_pSoundInternal->m_pPriorityGroups[m_nPriorityGroup];
    }

    m_nPriorityVolume = pGroup->m_nVolume;
    SetVolume(m_nVolume, 0, 1.0f);
    fMinDistance = pGroup->m_fMinVolumeDist;
    fMaxDistance = pGroup->m_fMaxVolumeDist;
    m_fMinDistance = fMaxDistance;
    m_fMaxDistance = fMinDistance;
}
// PC 0x005DCDB0  Xbox 0x001565B0
void CExoStreamingSoundSourceInternal::Resume()
{
    int bPlaying;
    int nLoopCount;

    if (m_pSoundInternal->m_bInitialized != 0 && m_hStream != NULL)
    {
        bPlaying = IsPlaying();
        if (bPlaying == 0)
        {
            SetVolume(m_nVolume, 0, 1.0f);
            if (m_bLooping == 0)
            {
                nLoopCount = 1;
            }
            else
            {
                nLoopCount = 0;
            }
            AIL_set_stream_loop_count(m_hStream, nLoopCount);
            AIL_set_stream_position(m_hStream, m_nPausePosition);
            AIL_pause_stream(m_hStream, 0);
            m_nPausePosition = 0;
        }
    }
}

static inline void ExostreamingsoundsourceinternalTrace1()
{
}
