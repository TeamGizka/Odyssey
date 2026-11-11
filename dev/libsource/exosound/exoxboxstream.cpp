#include "exosoundinternal.h"
#include "exobase.h"
#include <math.h>
#include <stdlib.h>

enum { MAX_VOLUME = 127 };

static DWORD s_nFreePackets;
// Xbox 0x001530D0
CExoXboxStream::~CExoXboxStream()
{
    if (m_pFileObject != NULL)
    {
        m_pFileObject->Release();
    }
    if (m_pStream != NULL)
    {
        m_pStream->Release();
    }
    if (m_pBuffer != NULL)
    {
        XPhysicalFree(m_pBuffer);
    }
    if (m_hFile != NULL)
    {
        CloseHandle(m_hFile);
    }
}
// Xbox 0x00153160
int CExoXboxStream::OpenWaveFile(XBOXADPCMWAVEFORMAT* pFormat, int bCached)
{
    WAVEFORMATEX wfxFile;
    LPCWAVEFORMATEX pFileFormat = &wfxFile;

    if (m_pFileObject == NULL)
    {
        if (FAILED(XWaveFileCreateMediaObjectEx(m_sFileName.CStr(), m_hFile,
                                                (XWaveFileMediaObject**)&m_pFileObject)))
        {
            return 0;
        }
    }
    ((XWaveFileMediaObject*)m_pFileObject)->GetFormat(&pFileFormat);
    pFormat->wfx.cbSize = pFileFormat->cbSize;
    pFormat->wfx.nAvgBytesPerSec = pFileFormat->nAvgBytesPerSec;
    pFormat->wfx.nBlockAlign = pFileFormat->nBlockAlign;
    pFormat->wfx.nChannels = pFileFormat->nChannels;
    pFormat->wfx.nSamplesPerSec = pFileFormat->nSamplesPerSec;
    pFormat->wfx.wBitsPerSample = pFileFormat->wBitsPerSample;
    pFormat->wfx.wFormatTag = pFileFormat->wFormatTag;
    pFormat->wSamplesPerBlock = pFormat->wfx.wFormatTag == WAVE_FORMAT_XBOX_ADPCM ? 64 : 0;
    m_pFileObject->GetLength(&m_nFileLength);
    m_nResourceType = RESTYPE_WAV;
    return (int)((double)m_nFileLength * 1000.0 /
                 ((double)pFileFormat->wBitsPerSample * (double)pFileFormat->nSamplesPerSec *
                  (double)pFileFormat->nChannels * 0.125));
}
// Xbox 0x00153270
int CExoXboxStream::OpenWMAFile(XBOXADPCMWAVEFORMAT* pFormat)
{
    WMAXMOFileHeader cHeader;

    if (m_pFileObject != NULL)
    {
        m_pFileObject->Release();
        m_pFileObject = NULL;
    }
    if (FAILED(WmaCreateDecoderEx(m_sFileName.CStr(), NULL, FALSE, WMA_LOOKAHEAD_SIZE, NUM_PACKETS, 1,
                                  &pFormat->wfx, (XWmaFileMediaObject**)&m_pFileObject)))
    {
        return 0;
    }
    ((XWmaFileMediaObject*)m_pFileObject)->GetFileHeader(&cHeader);
    m_nResourceType = EXOSOUND_RESTYPE_WMA;
    return cHeader.dwDuration;
}
// Xbox 0x001532F0
DWORD CExoXboxStream::GetPacketSize(XBOXADPCMWAVEFORMAT* pFormat, int bWMA)
{
    DWORD nBytes;
    DWORD nAlignment = max(pFormat->wfx.nBlockAlign * 512, 2048);
    if (bWMA != 0)
    {
        nBytes = pFormat->wfx.nAvgBytesPerSec * 333 / 1000;
    }
    else
    {
        nBytes = pFormat->wfx.nAvgBytesPerSec * 666 / 1000;
    }
    DWORD nSize = max(nAlignment, nBytes);
    m_nBlockAlign = pFormat->wfx.nBlockAlign;
    return (nSize + nAlignment - 1) / nAlignment * nAlignment;
}
// Xbox 0x00153350
int CExoXboxStream::GetPendingPackets(DWORD* pnPending)
{
    DWORD nPending = 0;
    int bFree = 0;
    CExoXboxStreamPacket* pPacket = m_aPackets;

    for (int nPacket = NUM_PACKETS; nPacket != 0; nPacket--)
    {
        if (pPacket->m_nStatus == XMEDIAPACKET_STATUS_PENDING)
        {
            nPending++;
        }
        else
        {
            bFree = 1;
        }
        pPacket++;
    }
    *pnPending = nPending;
    return bFree;
}
// Xbox 0x00153380
HRESULT CExoXboxStream::ReadPacket(DWORD nPacket)
{
    XMEDIAPACKET cPacket;
    DWORD nCompleted;
    HRESULT hr;

    ZeroMemory(&cPacket, sizeof(cPacket));
    cPacket.pvBuffer = (unsigned char*)m_pBuffer + m_nPacketSize * nPacket;
    cPacket.dwMaxSize = m_nPacketSize;
    cPacket.pdwCompletedSize = &nCompleted;
    cPacket.pdwStatus = &m_aPackets[nPacket].m_nStatus;
    m_aPackets[nPacket].m_nState = PACKET_READ;
    hr = m_pFileObject->Process(NULL, &cPacket);
    m_nPacketsRead++;
    if (nCompleted < m_nPacketSize)
    {
        cPacket.dwMaxSize = nCompleted;
        m_aPackets[nPacket].m_nCompletedSize = nCompleted;
        if (m_bLooping != 0)
        {
            if (m_nResourceType == EXOSOUND_RESTYPE_WMA)
            {
                m_pFileObject->Flush();
            }
            else
            {
                m_pFileObject->Seek(0, FILE_BEGIN, NULL);
            }
        }
        else
        {
            m_aPackets[nPacket].m_nLastPacket = m_nPacketsRead;
            m_bEndOfFile = 1;
        }
    }
    if (FAILED(hr) && hr != E_PENDING)
    {
        m_aPackets[nPacket].m_nState = PACKET_FREE;
        m_aPackets[nPacket].m_nStatus = 0;
        m_aPackets[nPacket].m_nCompletedSize = m_nPacketSize;
    }
    return S_OK;
}
// Xbox 0x001534A0
HRESULT CExoXboxStream::SubmitPacket(DWORD nPacket)
{
    XMEDIAPACKET cPacket;
    HRESULT hr = S_OK;

    ZeroMemory(&cPacket, sizeof(cPacket));
    cPacket.pvBuffer = (unsigned char*)m_pBuffer + nPacket * m_nPacketSize;
    cPacket.dwMaxSize = m_aPackets[nPacket].m_nCompletedSize;
    cPacket.pdwStatus = &m_aPackets[nPacket].m_nStatus;
    m_nPacketsSubmitted++;
    if (m_aPackets[nPacket].m_nLastPacket > 0)
    {
        if (m_nPacketsSubmitted == m_aPackets[nPacket].m_nLastPacket)
        {
            m_aPackets[nPacket].m_nState = PACKET_FREE;
            if (cPacket.dwMaxSize > 0)
            {
                m_pStream->Process(&cPacket, NULL);
            }
            m_pStream->Discontinuity();
            m_aPackets[nPacket].m_nCompletedSize = m_nPacketSize;
            m_bFinished = 1;
            return S_OK;
        }
    }
    else
    {
        m_aPackets[nPacket].m_nState = PACKET_FREE;
        if (cPacket.dwMaxSize > 0)
        {
            hr = m_pStream->Process(&cPacket, NULL);
        }
        m_aPackets[nPacket].m_nCompletedSize = m_nPacketSize;
        if (FAILED(hr))
        {
            return hr;
        }
    }
    return S_OK;
}
// Xbox 0x001535A0
void CExoXboxStream::SetLooping(int bLooping)
{
    m_bLooping = bLooping;
}
// Xbox 0x001535B0
void CExoXboxStream::Set3D(int b3D)
{
    m_b3D = b3D;
}
// Xbox 0x001535C0
CExoXboxStream::CExoXboxStream(CExoSoundInternal* pSoundInternal)
{
    m_bLooping = 0;
    m_nUnknown5C = 0;
    m_nFadeDuration = 0;
    m_nFadeStartTime = 0;
    m_b3D = 0;
    m_pSoundInternal = pSoundInternal;
    m_nPriorityGroup = CExoSoundPriorityGroup::MUSIC_GROUP;
    unsigned char nPriorityVolume =
        pSoundInternal->m_pPriorityGroups[CExoSoundPriorityGroup::MUSIC_GROUP].m_nVolume;
    m_pFileObject = NULL;
    m_pStream = NULL;
    m_pBuffer = NULL;
    m_nFileLength = 0;
    m_nUnknownB0 = 0;
    m_bPaused = 0;
    m_hFile = NULL;
    m_nPriorityVolume = nPriorityVolume;
    m_bFinished = 1;
    int nSourceId = pSoundInternal->m_nNextSourceId++;
    m_fMaxVolumeDist = 1.0f;
    m_bPlaying = 0;
    m_bPlayFailed = 0;
    m_bStarting = 0;
    m_bDeleted = 0;
    m_nUnknown14 = 0;
    m_bHasStartedStreaming = 0;
    m_nSourceId = nSourceId;
    m_bStopped = 1;
    m_nMagic = STREAM_MAGIC;
    m_nVolume = MAX_VOLUME;
    m_fMinVolumeDist = 10.0f;
    m_bReverbEnabled = 1;
    m_fFixedVariance = 0.0f;
    m_fPitchVariance = 0.0f;
}
// Xbox 0x001536C0
int CExoXboxStream::CreateStream(XBOXADPCMWAVEFORMAT* pFormat, int bWMA)
{
    DSMIXBINVOLUMEPAIR aMixBinVolumePairs[6];
    DSMIXBINS cMixBins;
    DSSTREAMDESC cStreamDesc;

    ZeroMemory(&cStreamDesc, sizeof(cStreamDesc));
    cStreamDesc.dwMaxAttachedPackets = NUM_PACKETS;
    cStreamDesc.lpwfxFormat = &pFormat->wfx;
    if (m_b3D != 0)
    {
        cStreamDesc.dwFlags |= DSSTREAMCAPS_CTRL3D;
    }
    else if (m_nPriorityGroup == 8 || m_nPriorityGroup == 9 || m_nPriorityGroup == 1 ||
             m_nPriorityGroup == CExoSoundPriorityGroup::MUSIC_GROUP || m_nPriorityGroup == 4)
    {

        cStreamDesc.lpMixBins = &cMixBins;
        cMixBins.dwMixBinCount = 6;
        cMixBins.lpMixBinVolumePairs = aMixBinVolumePairs;
        aMixBinVolumePairs[0].dwMixBin = DSMIXBIN_FRONT_LEFT;
        aMixBinVolumePairs[0].lVolume = DSBVOLUME_MAX;
        aMixBinVolumePairs[1].dwMixBin = DSMIXBIN_FRONT_RIGHT;
        aMixBinVolumePairs[1].lVolume = DSBVOLUME_MAX;
        aMixBinVolumePairs[2].dwMixBin = DSMIXBIN_FRONT_CENTER;
        if (m_nPriorityGroup == 8 || m_nPriorityGroup == 9)
        {
            aMixBinVolumePairs[2].lVolume = DSBVOLUME_MAX;
        }
        else
        {
            aMixBinVolumePairs[2].lVolume = EXOSOUND_MIN_VOLUME;
        }
        aMixBinVolumePairs[3].dwMixBin = DSMIXBIN_LOW_FREQUENCY;
        aMixBinVolumePairs[3].lVolume = DSBVOLUME_MAX;
        aMixBinVolumePairs[4].dwMixBin = DSMIXBIN_BACK_LEFT;
        aMixBinVolumePairs[4].lVolume = -1204;
        aMixBinVolumePairs[5].dwMixBin = DSMIXBIN_BACK_RIGHT;
        aMixBinVolumePairs[5].lVolume = -1204;
    }
    if (m_pStream == NULL)
    {
        if (FAILED(DirectSoundCreateStream(&cStreamDesc, &m_pStream)))
        {
            return 0;
        }
    }
    if (m_b3D != 0 && m_pStream != NULL && m_bReverbEnabled == 0)
    {
        DSI3DL2BUFFER cSource;

        ZeroMemory(&cSource, sizeof(cSource));
        cSource.lRoomHF = DSBVOLUME_MIN;
        cSource.lRoom = DSBVOLUME_MIN;
        m_pStream->SetI3DL2Source(&cSource, DS3D_DEFERRED);
    }
    m_nPacketSize = GetPacketSize(pFormat, bWMA);
    return 1;
}
// Xbox 0x00153840
HRESULT CExoXboxStream::Process()
{
    DWORD nPacket = 0;
    DWORD nProcessed = 0;
    DWORD nPending = 0;
    DWORD nStatus;
    HRESULT hr;

    if (m_pFileObject == NULL)
    {
        return S_OK;
    }
    DirectSoundDoWork();
    if (m_bFinished != 0 || m_bPaused != 0)
    {
        return S_OK;
    }
    if (FAILED(m_pFileObject->GetStatus(&nStatus)))
    {
        return E_FAIL;
    }
    if ((nStatus & XMO_STATUSF_ACCEPT_OUTPUT_DATA) == 0)
    {
        return S_OK;
    }
    GetPendingPackets(&nPending);
    s_nFreePackets = NUM_PACKETS - nPending;
    if (nPending != NUM_PACKETS)
    {
        for (; nPacket < NUM_PACKETS; nPacket++)
        {
            if (m_aPackets[nPacket].m_nStatus != XMEDIAPACKET_STATUS_PENDING)
            {
                nProcessed++;
                if (m_aPackets[nPacket].m_nState == PACKET_FREE)
                {
                    if (m_bEndOfFile != 0)
                    {
                        m_aPackets[nPacket].m_nStatus = XMEDIAPACKET_STATUS_PENDING;
                    }
                    else
                    {
                        hr = ReadPacket(nPacket);
                        if (FAILED(hr))
                        {
                            return hr;
                        }
                        if (m_bFinished != 0)
                        {
                            return S_OK;
                        }
                    }
                }
                if (m_aPackets[nPacket].m_nState == PACKET_READ)
                {
                    hr = SubmitPacket(nPacket);
                    if (FAILED(hr))
                    {
                        return hr;
                    }
                }
                else if (m_aPackets[nPacket].m_nState != PACKET_FREE)
                {
                    m_aPackets[nPacket].m_nState = PACKET_FREE;
                }
                if (nProcessed >= s_nFreePackets)
                {
                    break;
                }
            }
        }
    }
    m_bStarting = 0;
    return S_OK;
}
// Xbox 0x00153950
int CExoXboxStream::IsPlaying()
{
    DWORD nStatus = 0;

    if (m_bPaused != 0)
    {
        return 1;
    }
    if (m_pSoundInternal != NULL && m_pSoundInternal->m_bInitialized != 0 && m_pStream != NULL)
    {
        m_pStream->GetStatus(&nStatus);
        if ((nStatus & DSSTREAMSTATUS_PLAYING) != 0)
        {
            return 1;
        }
        if ((nStatus & DSSTREAMSTATUS_STARVED) != 0)
        {
            return 1;
        }
    }
    return 0;
}
// Xbox 0x001539A0
int CExoXboxStream::IsActive()
{
    return IsPlaying() || m_bStarting != 0;
}
// Xbox 0x001539C0
void CExoXboxStream::Stop(int bKeepPlaying)
{
    if (m_pStream != NULL)
    {
        m_pStream->FlushEx(0, DSSTREAMFLUSHEX_ASYNC);
    }
    if (m_pBuffer != NULL)
    {
        XPhysicalFree(m_pBuffer);
        m_pBuffer = NULL;
    }
    m_nResourceType = RESTYPE_INVALID;
    m_nFadeDuration = 0;
    m_nFadeStartTime = 0;
    m_nUnknown5C = 0;
    m_bFinished = 1;
    m_bEndOfFile = 1;
    if (bKeepPlaying == 0)
    {
        m_bPlaying = 0;
    }
    m_bStarting = 0;
    m_nUnknown14 = 0;
    m_bHasStartedStreaming = 0;
    m_bStopped = 1;
}
// Xbox 0x00153A30
void CExoXboxStream::FadeAndStop(unsigned long nFadeTime)
{
    if (nFadeTime <= EXOSOUND_MIN_FADE_TIME)
    {
        Stop(0);
        return;
    }
    if (m_pStream != NULL)
    {
        m_nFadeStartTime = g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000;
        m_nFadeDuration = nFadeTime - EXOSOUND_MIN_FADE_TIME;

        DSENVELOPEDESC cEnvelope = { 0 };
        cEnvelope.dwEG = DSEG_AMPLITUDE;
        cEnvelope.dwMode = DSEG_MODE_ATTACK;
        cEnvelope.dwAttack = 0;

        float fSeconds = (float)nFadeTime * 0.001f;
        cEnvelope.dwRelease = (DWORD)(fSeconds * 93.75f);
        cEnvelope.dwSustain = 255;
        m_pStream->SetEG(&cEnvelope);
        m_pStream->Discontinuity();
        m_pStream->FlushEx(0, DSSTREAMFLUSHEX_ASYNC | DSSTREAMFLUSHEX_ENVELOPE);
    }
}
// Xbox 0x00153B30
void CExoXboxStream::Pause()
{
    if (m_pSoundInternal->m_bInitialized != 0)
    {
        if (m_pStream != NULL && m_bPaused == 0)
        {
            m_pStream->Pause(DSSTREAMPAUSE_PAUSE);
            m_bPaused = 1;
        }
    }
}
// Xbox 0x00153B60
void CExoXboxStream::Resume()
{
    if (m_bPaused != 0)
    {
        if (m_pStream != NULL)
        {
            m_pStream->Pause(DSSTREAMPAUSE_RESUME);
            m_bPaused = 0;
        }
    }
}
// Xbox 0x00153B90
void CExoXboxStream::SetPosition(const Vector& vPosition)
{
    m_vPosition = vPosition;
    if (m_b3D != 0)
    {
        if (m_pStream != NULL)
        {
            m_pStream->SetPosition(vPosition.m_fX, vPosition.m_fZ, vPosition.m_fY, DS3D_DEFERRED);
        }
    }
}
// Xbox 0x00153BE0
void CExoXboxStream::SetObstruction(float fObstruction)
{
    if (m_pStream != NULL)
    {
        DSI3DL2BUFFER cSource;

        ZeroMemory(&cSource, sizeof(cSource));
        cSource.Obstruction.lHFLevel = (int)(fObstruction * -2000.0f);
        cSource.Obstruction.flLFRatio = fObstruction * 0.25f;
        if (m_bReverbEnabled == 0)
        {
            cSource.lRoom = DSBVOLUME_MIN;
            cSource.lRoomHF = DSBVOLUME_MIN;
        }
        m_pStream->SetI3DL2Source(&cSource, DS3D_IMMEDIATE);
    }
}
// Xbox 0x00153C60
void CExoXboxStream::CalculatePitchVarianceFrequency()
{
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
        unsigned long nRange = (unsigned long)(fVariance * m_nSampleRate * 0.5f);
        if (nRange != 0)
        {
            m_nPitchRate -= rand() % nRange;
        }
    }
}
// Xbox 0x00153D00
void CExoXboxStream::SetVolume(unsigned char nVolume, int nSliderMode, float fScale, int bDirect)
{
    if (nVolume > MAX_VOLUME)
    {
        nVolume = MAX_VOLUME;
    }
    int nMode = nSliderMode;
    if (nSliderMode == 0)
    {
        m_nVolume = nVolume;
    }
    int nSliderType = m_pSoundInternal->GetSliderType(this);
    if (m_pSoundInternal->GetMutable(this) != 0 && nSliderMode == 0)
    {
        float fFirst = m_pSoundInternal->GetSliderVolume(nSliderType, 1);
        nMode = m_pSoundInternal->GetSliderVolume(nSliderType, 0) != fFirst;
    }
    float fSlider = m_pSoundInternal->GetSliderVolume(nSliderType, nMode);
    float fVolumeScale = (m_nPriorityGroup == 4 || m_nPriorityGroup == 21) ? fScale : 1.0f;
    int nLevel = nVolume;
    if (bDirect == 0)
    {
        nLevel = (int)((float)m_nPriorityVolume * (float)nVolume * fVolumeScale * fSlider * (1.0f / 127.0f));
    }
    long nMillibels = ExoSoundVolumeToMillibels((unsigned char)nLevel, EXOSOUND_MIN_VOLUME);
    if (m_pStream != NULL)
    {
        m_pStream->SetVolume(nMillibels);
    }
}
// Xbox 0x00153E00
void CExoXboxStream::SetPriorityGroup(unsigned char nPriorityGroup)
{
    CExoSoundPriorityGroup* pGroup = &m_pSoundInternal->m_pPriorityGroups[nPriorityGroup];
    if (pGroup != NULL)
    {
        m_nPriorityGroup = nPriorityGroup;
    }
    else
    {
        pGroup = &m_pSoundInternal->m_pPriorityGroups[m_nPriorityGroup];
    }
    m_nPriorityVolume = pGroup->m_nVolume;
    SetVolume(m_nVolume, 0, 1.0f, 0);
    float fMinVolumeDist = pGroup->m_fMinVolumeDist;
    m_fMaxVolumeDist = pGroup->m_fMaxVolumeDist;
    m_fMinVolumeDist = fMinVolumeDist;
}
// Xbox 0x00153E70
int CExoXboxStream::SetFixedVariance(float fVariance)
{
    if (fVariance <= 2.0f && fVariance >= 0.5f)
    {
        m_fFixedVariance = fVariance;
        m_fPitchVariance = 0.0f;
        unsigned long nFixedRate = (unsigned long)((float)m_nSampleRate * fVariance);
        m_nFixedRate = nFixedRate;
        if (m_pStream != NULL && (IsPlaying() || m_bHasStartedStreaming != 0))
        {
            m_pStream->SetFrequency(nFixedRate);
        }
        return 1;
    }
    return 0;
}
// Xbox 0x00153F10
void CExoXboxStream::SetPitchVariance(float fVariance)
{
    float fPitchVariance = min(1.0f, max(0.0f, fVariance));

    m_fPitchVariance = fPitchVariance;
    m_fFixedVariance = 0.0f;
    CalculatePitchVarianceFrequency();
    unsigned long nPitchRate = m_nPitchRate;
    if (m_pStream != NULL && (IsPlaying() || m_bHasStartedStreaming != 0))
    {
        m_pStream->SetFrequency(nPitchRate);
    }
}
// Xbox 0x00153F90
int CExoXboxStream::Play(const CExoString& sFileName, unsigned short nResourceType, int bVoice, int bMusic,
                         int bCached, int bPaused)
{
    XBOXADPCMWAVEFORMAT wfxFormat;
    int bDirect = bVoice != 0 && bMusic != 0 && bCached != 0;

    if (bDirect)
    {
        bCached = 0;
    }
    if (sFileName == "")
    {
        return 0;
    }
    m_resRef = sFileName;
    m_sFileName = sFileName;
    if (m_pStream != NULL)
    {
        m_pStream->FlushEx(0, DSSTREAMFLUSHEX_ASYNC);
    }
    if (m_pBuffer != NULL)
    {
        XPhysicalFree(m_pBuffer);
        m_pBuffer = NULL;
    }
    m_nResourceType = RESTYPE_INVALID;
    m_nFadeDuration = 0;
    m_nFadeStartTime = 0;
    m_nUnknown5C = 0;
    m_bFinished = 1;
    m_bEndOfFile = 1;
    m_bStarting = 0;
    m_nUnknown14 = 0;
    m_bStopped = 1;
    m_bHasStartedStreaming = 0;
    if (m_hFile != NULL)
    {
        CloseHandle(m_hFile);
    }

    int nDuration;
    if (nResourceType == RESTYPE_WAV)
    {
        nDuration = OpenWaveFile(&wfxFormat, bCached);
    }
    else
    {
        nDuration = OpenWMAFile(&wfxFormat);
    }
    if (nDuration == 0)
    {
        return 0;
    }
    if (CreateStream(&wfxFormat, nResourceType == EXOSOUND_RESTYPE_WMA) == 0)
    {
        return 0;
    }
    m_bStopped = 0;
    SetPosition(m_vPosition);
    if (m_pBuffer == NULL)
    {
        m_pBuffer = XPhysicalAlloc(NUM_PACKETS * m_nPacketSize, MAXULONG_PTR, 0, PAGE_READWRITE | PAGE_NOCACHE);
        if (m_pBuffer == NULL)
        {
            return 0;
        }
    }
    CExoXboxStreamPacket* pPacket = m_aPackets;
    for (int nPacket = NUM_PACKETS; nPacket != 0; nPacket--)
    {
        pPacket->m_nCompletedSize = m_nPacketSize;
        pPacket->m_nStatus = 0;
        pPacket->m_nState = PACKET_FREE;
        pPacket->m_nLastPacket = 0;
        pPacket++;
    }
    m_bEndOfFile = 0;
    m_nPacketsRead = 0;
    m_nPacketsSubmitted = 0;
    SetVolume(m_nVolume, 0, m_pSoundInternal->m_fVolumeScale, bDirect);
    if (m_b3D != 0)
    {
        m_pStream->SetMinDistance(m_fMaxVolumeDist, DS3D_DEFERRED);
        m_pStream->SetMaxDistance(m_fMinVolumeDist, DS3D_DEFERRED);
    }
    m_bFinished = 0;
    m_bStarting = 1;
    m_bHasStartedStreaming = 1;
    m_bPlaying = 1;
    if (bPaused != 0)
    {
        Pause();
    }
    m_nSampleRate = wfxFormat.wfx.nSamplesPerSec;
    m_nPitchRate = wfxFormat.wfx.nSamplesPerSec;
    if (m_fPitchVariance != 0.0f)
    {
        CalculatePitchVarianceFrequency();
    }
    if (m_fFixedVariance != 0.0f)
    {
        m_nFixedRate = (unsigned long)((float)m_nSampleRate * m_fFixedVariance);
    }
    return nDuration;
}
