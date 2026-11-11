#include "exosoundinternal.h"
#include "exobase.h"
#include <math.h>
#include <stdlib.h>

static const int MAX_VOLUME = 127;

CExoXboxStreamCommand::~CExoXboxStreamCommand()
{
}

int CExoXboxStreamManager::QueueCommand(CExoXboxStreamCommand cCommand)
{
    DWORD nNext = m_nCommandHead;
    DWORD nTail = m_nCommandTail;
    int nCommand = nNext;

    while (nCommand != nTail)
    {
        if (cCommand.m_pStream == m_aCommands[nCommand].m_pStream)
        {
            if (cCommand.m_nCommand != m_aCommands[nCommand].m_nCommand)
            {
                break;
            }
            if (cCommand.m_fParam1 == m_aCommands[nCommand].m_fParam1 &&
                cCommand.m_fParam2 == m_aCommands[nCommand].m_fParam2 &&
                cCommand.m_fParam3 == m_aCommands[nCommand].m_fParam3 &&
                cCommand.m_fParam4 == m_aCommands[nCommand].m_fParam4 &&
                cCommand.m_sFileName == m_aCommands[nCommand].m_sFileName)
            {
                return 1;
            }
        }
        if (--nCommand < 0)
        {
            nCommand = QUEUE_SIZE - 1;
        }
    }

    nNext++;
    if (nNext >= QUEUE_SIZE)
    {
        nNext = 0;
    }
    if (nNext == nTail)
    {
        return 0;
    }
    m_aCommands[nNext].m_nCommand = cCommand.m_nCommand;
    m_aCommands[nNext].m_fParam1 = cCommand.m_fParam1;
    m_aCommands[nNext].m_fParam2 = cCommand.m_fParam2;
    m_aCommands[nNext].m_fParam3 = cCommand.m_fParam3;
    m_aCommands[nNext].m_fParam4 = cCommand.m_fParam4;
    m_aCommands[nNext].m_sFileName = cCommand.m_sFileName;
    m_aCommands[nNext].m_pStream = cCommand.m_pStream;
    m_nCommandHead = nNext;
    return 1;
}

int CExoXboxStreamManager::GetNextCommand(CExoXboxStreamCommand& cCommand)
{
    DWORD nNext = m_nCommandTail;
    if (m_nCommandHead == nNext)
    {
        cCommand.m_nCommand = CExoXboxStreamCommand::COMMAND_NONE;
        cCommand.m_pStream = NULL;
        return 0;
    }

    nNext++;
    if (nNext >= QUEUE_SIZE)
    {
        nNext = 0;
    }
    cCommand.m_nCommand = m_aCommands[nNext].m_nCommand;
    cCommand.m_fParam1 = m_aCommands[nNext].m_fParam1;
    cCommand.m_fParam2 = m_aCommands[nNext].m_fParam2;
    cCommand.m_fParam3 = m_aCommands[nNext].m_fParam3;
    cCommand.m_fParam4 = m_aCommands[nNext].m_fParam4;
    cCommand.m_sFileName = m_aCommands[nNext].m_sFileName;
    cCommand.m_pStream = m_aCommands[nNext].m_pStream;
    m_nCommandTail = nNext;
    return 1;
}

int CExoXboxStreamManager::QueueDeletedStream(CExoXboxStream* pStream)
{
    DWORD nNext = m_nDeletedHead;
    DWORD nTail = m_nDeletedTail;

    nNext++;
    if (nNext >= QUEUE_SIZE)
    {
        nNext = 0;
    }
    if (nNext == nTail)
    {
        return 0;
    }
    m_apDeletedStreams[nNext] = pStream;
    m_nDeletedHead = nNext;
    return 1;
}

int CExoXboxStreamManager::GetNextDeletedStream(CExoXboxStream** ppStream)
{
    if (m_nDeletedHead == m_nDeletedTail)
    {
        *ppStream = NULL;
        return 0;
    }

    DWORD nNext = m_nDeletedTail + 1;
    if (nNext >= QUEUE_SIZE)
    {
        nNext = 0;
    }
    *ppStream = m_apDeletedStreams[nNext];
    m_apDeletedStreams[nNext] = NULL;
    m_nDeletedTail = nNext;
    return 1;
}

CExoXboxStreamCommand::CExoXboxStreamCommand()
{
    m_nCommand = COMMAND_NONE;
    m_pStream = NULL;
    m_fParam1 = 0.0f;
    m_fParam2 = 0.0f;
    m_fParam3 = 0.0f;
    m_fParam4 = 0.0f;
    m_sFileName = "";
}

void CExoXboxStreamManager::ShutDown()
{
    CExoXboxStream* pStream = m_lstStreams.GetHead();

    while (m_lstStreams.GetHeadPos() != NULL)
    {
        pStream->Stop(0);
        delete pStream;
        pStream = m_lstStreams.GetHead();
    }
}

void CExoXboxStreamManager::RemoveStream(CExoXboxStream* pStream)
{
    CExoLinkedListNode* pPosition = m_lstStreams.GetHeadPos();
    CExoXboxStream* pListStream = m_lstStreams.GetHead();

    while (pPosition != NULL)
    {
        if (pStream == pListStream)
        {
            m_lstStreams.Remove(pPosition);
            return;
        }
        pListStream = m_lstStreams.GetNext(pPosition);
    }
}

void CExoXboxStreamManager::ProcessCommand(CExoXboxStreamCommand cCommand)
{
    int nCommand = cCommand.m_nCommand;
    float fParam1 = cCommand.m_fParam1;
    float fParam2 = cCommand.m_fParam2;
    float fParam3 = cCommand.m_fParam3;
    float fParam4 = cCommand.m_fParam4;
    CExoString sFileName = cCommand.m_sFileName;
    CExoXboxStream* pStream = cCommand.m_pStream;

    switch (nCommand)
    {
    case CExoXboxStreamCommand::COMMAND_PLAY:
    {
        int bListed = m_lstStreams.Contains(pStream);
        if (bListed == 0)
        {

            unsigned int nResult = pStream->Play(sFileName, (unsigned short)fParam1, (int)fParam2, (int)fParam3,
                                                 (int)fParam4, bListed);
            if (nResult > 0)
            {
                m_lstStreams.AddHead(pStream);
            }
            else
            {
                pStream->Stop(0);
                pStream->m_bPlayFailed = 1;
            }
        }
        break;
    }
    case CExoXboxStreamCommand::COMMAND_DELETE:
    {
        if (m_lstStreams.Contains(pStream) != 0)
        {
            RemoveStream(pStream);
            pStream->m_bPlaying = 0;
            pStream->m_bPlayFailed = 1;
            pStream->Stop(0);
        }
        pStream->m_nMagic = CExoXboxStream::STREAM_MAGIC;
        if (pStream->m_bDeleted == 0)
        {
            pStream->m_bDeleted = 1;
            pStream->Stop(0);
            QueueDeletedStream(pStream);
        }
        break;
    }
    case CExoXboxStreamCommand::COMMAND_SET_LOOPING:
        if (m_lstStreams.Contains(pStream) != 0)
        {
            if (fParam1 == 1.0f)
            {
                pStream->SetLooping(1);
            }
            else
            {
                pStream->SetLooping(0);
            }
        }
        break;
    case CExoXboxStreamCommand::COMMAND_SET_3D:
        if (m_lstStreams.Contains(pStream) != 0)
        {
            if (fParam1 == 1.0f)
            {
                pStream->Set3D(1);
            }
            else
            {
                pStream->Set3D(0);
            }
        }
        break;
    case CExoXboxStreamCommand::COMMAND_SET_VOLUME:
        if (m_lstStreams.Contains(pStream) != 0)
        {
            unsigned char nVolume = (unsigned char)fParam1;
            int nSliderMode = 0;
            if (fParam2 == 1.0f)
            {
                nSliderMode = 1;
            }
            pStream->SetVolume(nVolume, nSliderMode, fParam3, 0);
        }
        break;
    case CExoXboxStreamCommand::COMMAND_STOP:
        if (m_lstStreams.Contains(pStream) != 0 && pStream->IsPlaying())
        {
            RemoveStream(pStream);
            pStream->Stop(0);
        }
        break;
    case CExoXboxStreamCommand::COMMAND_PAUSE:
        if (m_lstStreams.Contains(pStream) != 0)
        {
            pStream->Pause();
        }
        break;
    case CExoXboxStreamCommand::COMMAND_RESUME:
        if (m_lstStreams.Contains(pStream) != 0)
        {
            pStream->Resume();
        }
        break;
    case CExoXboxStreamCommand::COMMAND_SET_POSITION:
        if (m_lstStreams.Contains(pStream) != 0)
        {
            Vector vPosition;
            vPosition.m_fX = fParam1;
            vPosition.m_fY = fParam2;
            vPosition.m_fZ = fParam3;
            pStream->SetPosition(vPosition);
        }
        break;
    case CExoXboxStreamCommand::COMMAND_FADE_AND_STOP:
        if (m_lstStreams.Contains(pStream) != 0)
        {
            pStream->FadeAndStop((unsigned long)fParam1);
        }
        break;
    case CExoXboxStreamCommand::COMMAND_SET_OBSTRUCTION:
        if (m_lstStreams.Contains(pStream) != 0)
        {
            pStream->SetObstruction(fParam1);
        }
        break;
    case CExoXboxStreamCommand::COMMAND_SET_PRIORITY_GROUP:
        if (m_lstStreams.Contains(pStream) != 0)
        {
            pStream->SetPriorityGroup((unsigned char)fParam1);
        }
        break;
    case CExoXboxStreamCommand::COMMAND_SET_PITCH_VARIANCE:
        if (m_lstStreams.Contains(pStream) != 0)
        {
            pStream->SetPitchVariance(fParam1);
        }
        break;
    case CExoXboxStreamCommand::COMMAND_SET_FIXED_VARIANCE:
        if (m_lstStreams.Contains(pStream) != 0)
        {
            pStream->SetFixedVariance(fParam1);
        }
        break;
    case CExoXboxStreamCommand::COMMAND_STOP_ALL:
    {
        CExoXboxStream* pPlaying = m_lstStreams.GetHead();
        while (m_lstStreams.GetHeadPos() != NULL)
        {
            RemoveStream(pPlaying);
            pPlaying->Stop(0);
            pPlaying->m_bDeleted = 1;
            QueueDeletedStream(pPlaying);
        }
        break;
    }
    default:
        break;
    }
}

void CExoXboxStreamManager::Update()
{
    float fVolumeScale = m_fVolumeScale;
    float fLastVolumeScale = m_fLastVolumeScale;
    m_fLastVolumeScale = fVolumeScale;

    CExoXboxStreamCommand cCommand;
    while (GetNextCommand(cCommand) != 0)
    {
        ProcessCommand(cCommand);
    }

    g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
    DirectSoundDoWork();

    CExoLinkedListNode* pPosition = m_lstStreams.GetHeadPos();
    CExoXboxStream* pStream = m_lstStreams.GetHead();
    while (pPosition != NULL)
    {
        unsigned long nFadeDuration = pStream->m_nFadeDuration;
        int bActive = pStream->IsActive();

        pStream->Process();
        if (fVolumeScale != fLastVolumeScale)
        {
            unsigned char nVolume = (unsigned char)pStream->m_nVolume;
            pStream->SetVolume(nVolume, 0, m_fVolumeScale, 0);
        }
        if (pStream->m_bLooping != 1 && bActive != 1)
        {
            CExoXboxStream* pFinished = pStream;
            pStream = m_lstStreams.GetNext(pPosition);
            RemoveStream(pFinished);
            pFinished->Stop(0);
        }
        else if (nFadeDuration != 0 && bActive == 1 &&
                 nFadeDuration < (unsigned long)(g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000) -
                                     pStream->m_nFadeStartTime)
        {
            CExoXboxStream* pFinished = pStream;
            pStream = m_lstStreams.GetNext(pPosition);
            RemoveStream(pFinished);
            pFinished->Stop(0);
        }
        else
        {
            pStream = m_lstStreams.GetNext(pPosition);
        }
    }
}

static DWORD WINAPI ExoXboxStreamThread(LPVOID pParameter)
{
    CExoXboxStreamManager* pManager = (CExoXboxStreamManager*)pParameter;

    do
    {
        pManager->Update();
    } while (WaitForSingleObject(pManager->m_hEvent, CExoXboxStreamManager::UPDATE_INTERVAL) == WAIT_TIMEOUT);
    return 0;
}

void CExoXboxStreamManager::Initialize()
{
    m_hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    m_hThread = CreateThread(NULL, THREAD_STACK_SIZE, ExoXboxStreamThread, this, 0, NULL);
    m_nStartTime = g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000;
    m_nCommandHead = 0;
    m_nCommandTail = 0;
    m_nDeletedHead = 0;
    m_nDeletedTail = 0;
    m_fVolumeScale = 1.0f;
    m_fLastVolumeScale = 1.0f;
    m_fUnknown1C = 1.0f;
}

CExoXboxStreamManager::CExoXboxStreamManager(CExoSoundInternal* pSoundInternal)
{
    m_pSoundInternal = pSoundInternal;
    Initialize();
}

int CExoStreamingSoundSourceInternal::IsPlaying()
{
    return m_pStream->m_bPlaying;
}

int CExoStreamingSoundSourceInternal::GetPlayFailed()
{
    return m_pStream->m_bPlayFailed;
}

int CExoStreamingSoundSourceInternal::GetHasStartedStreaming()
{
    if (m_pStream != NULL)
    {
        return m_pStream->m_bHasStartedStreaming;
    }
    return 0;
}

CExoStreamingSoundSourceInternal::CExoStreamingSoundSourceInternal(CExoSoundInternal* pSoundInternal)
{
    m_pSoundInternal = pSoundInternal;
    m_b3D = 0;
    m_bLooping = 0;
    m_nUnknown34 = 0;
    m_nVolume = MAX_VOLUME;
    m_nPriorityGroup = CExoSoundPriorityGroup::MUSIC_GROUP;
    m_pStream = NULL;
    m_pStream = new CExoXboxStream(pSoundInternal);
    m_nSourceId = m_pStream->m_nSourceId;
    m_vObstructionListenerPosition = Vector();
    m_nObstructionTime = 0;
    m_fOcclusion = 0.0f;
}

CExoStreamingSoundSourceInternal::~CExoStreamingSoundSourceInternal()
{
    int bPlaying = m_pStream->m_bPlaying;

    m_pSoundInternal->RemoveFromStreamingList(this);
    if (bPlaying == 0)
    {
        m_pStream->m_nMagic = bPlaying;
        if (m_pStream != NULL)
        {
            delete m_pStream;
        }
        m_pStream = NULL;
    }
    else
    {
        CExoXboxStreamCommand cCommand;
        cCommand.m_pStream = m_pStream;
        cCommand.m_nCommand = CExoXboxStreamCommand::COMMAND_DELETE;
        GetSoundInternal()->m_pStreamManager->QueueCommand(cCommand);
    }
}

void CExoStreamingSoundSourceInternal::SetLooping(int bLooping)
{
    int bPlaying = m_pStream->m_bPlaying;
    m_bLooping = bLooping;
    if (bPlaying == 0)
    {
        m_pStream->SetLooping(bLooping);
        return;
    }

    CExoXboxStreamCommand cCommand;
    cCommand.m_nCommand = CExoXboxStreamCommand::COMMAND_SET_LOOPING;
    cCommand.m_pStream = m_pStream;
    if (bLooping != 0)
    {
        cCommand.m_fParam1 = 1.0f;
    }
    GetSoundInternal()->m_pStreamManager->QueueCommand(cCommand);
}

void CExoStreamingSoundSourceInternal::Set3D(int b3D)
{
    int bStopped = m_pStream->m_bStopped;
    int bPlaying = m_pStream->m_bPlaying;

    if (bStopped != 0)
    {
        m_b3D = b3D;
        if (bPlaying == 0)
        {
            m_pStream->Set3D(b3D);
            return;
        }

        CExoXboxStreamCommand cCommand;
        cCommand.m_nCommand = CExoXboxStreamCommand::COMMAND_SET_3D;
        cCommand.m_pStream = m_pStream;
        if (b3D != 0)
        {
            cCommand.m_fParam1 = 1.0f;
        }
        GetSoundInternal()->m_pStreamManager->QueueCommand(cCommand);
    }
}

void CExoStreamingSoundSourceInternal::SetDistance(float fMaxVolumeDist, float fMinVolumeDist)
{
    CExoXboxStream* pStream = m_pStream;
    int bPlaying = pStream->m_bPlaying;
    m_fMaxVolumeDist = fMaxVolumeDist;
    m_fMinVolumeDist = fMinVolumeDist;
    if (bPlaying == 0)
    {
        pStream->m_fMaxVolumeDist = fMaxVolumeDist;
        pStream->m_fMinVolumeDist = fMinVolumeDist;
    }
}

void CExoStreamingSoundSourceInternal::SetVolume(unsigned char nVolume, int nSliderMode, float fScale)
{
    int bPlaying = m_pStream->m_bPlaying;

    if (nSliderMode == 0)
    {
        m_nVolume = nVolume;
    }
    if (bPlaying == 0)
    {
        m_pStream->SetVolume(nVolume, nSliderMode, fScale, 0);
        return;
    }

    CExoXboxStreamCommand cCommand;
    cCommand.m_nCommand = CExoXboxStreamCommand::COMMAND_SET_VOLUME;
    cCommand.m_pStream = m_pStream;
    cCommand.m_fParam1 = (float)nVolume;
    if (nSliderMode != 0)
    {
        cCommand.m_fParam2 = 1.0f;
    }
    cCommand.m_fParam3 = fScale;
    GetSoundInternal()->m_pStreamManager->QueueCommand(cCommand);
}

void CExoStreamingSoundSourceInternal::Stop()
{
    int bPlaying = m_pStream->m_bPlaying;

    m_pSoundInternal->RemoveFromStreamingList(this);
    if (bPlaying == 0)
    {
        m_pStream->Stop(bPlaying);
        return;
    }

    CExoXboxStreamCommand cCommand;
    cCommand.m_pStream = m_pStream;
    cCommand.m_nCommand = CExoXboxStreamCommand::COMMAND_STOP;
    GetSoundInternal()->m_pStreamManager->QueueCommand(cCommand);
}

void CExoStreamingSoundSourceInternal::FadeAndStop(unsigned long nFadeTime)
{
    int bPlaying = m_pStream->m_bPlaying;
    if (bPlaying != 0)
    {
        CExoXboxStreamCommand cCommand;
        cCommand.m_nCommand = CExoXboxStreamCommand::COMMAND_FADE_AND_STOP;
        cCommand.m_fParam1 = (float)nFadeTime;
        cCommand.m_pStream = m_pStream;
        GetSoundInternal()->m_pStreamManager->QueueCommand(cCommand);
    }
    m_nFadeDuration = nFadeTime;
}

void CExoStreamingSoundSourceInternal::Pause()
{
    int bPlaying = m_pStream->m_bPlaying;
    if (bPlaying == 0)
    {
        m_pStream->Pause();
        return;
    }

    CExoXboxStreamCommand cCommand;
    cCommand.m_pStream = m_pStream;
    cCommand.m_nCommand = CExoXboxStreamCommand::COMMAND_PAUSE;
    GetSoundInternal()->m_pStreamManager->QueueCommand(cCommand);
}

void CExoStreamingSoundSourceInternal::Resume()
{
    int bPlaying = m_pStream->m_bPlaying;
    if (bPlaying == 0)
    {
        m_pStream->Resume();
        return;
    }

    CExoXboxStreamCommand cCommand;
    cCommand.m_pStream = m_pStream;
    cCommand.m_nCommand = CExoXboxStreamCommand::COMMAND_RESUME;
    GetSoundInternal()->m_pStreamManager->QueueCommand(cCommand);
}

void CExoStreamingSoundSourceInternal::SetPosition(const Vector& vPosition)
{
    int bPlaying = m_pStream->m_bPlaying;
    m_vPosition = vPosition;
    if (bPlaying == 0)
    {
        m_pStream->SetPosition(vPosition);
        return;
    }

    CExoXboxStreamCommand cCommand;
    cCommand.m_fParam1 = vPosition.m_fX;
    cCommand.m_fParam2 = vPosition.m_fY;
    cCommand.m_pStream = m_pStream;
    cCommand.m_nCommand = CExoXboxStreamCommand::COMMAND_SET_POSITION;
    cCommand.m_fParam3 = vPosition.m_fZ;
    GetSoundInternal()->m_pStreamManager->QueueCommand(cCommand);
}

void CExoStreamingSoundSourceInternal::DoObstructionWork()
{
    int bPlaying = m_pStream->m_bPlaying;
    if (bPlaying != 0)
    {
        CExoXboxStreamCommand cCommand;
        cCommand.m_pStream = m_pStream;
        cCommand.m_nCommand = CExoXboxStreamCommand::COMMAND_SET_OBSTRUCTION;
        cCommand.m_fParam1 = m_fOcclusion;
        GetSoundInternal()->m_pStreamManager->QueueCommand(cCommand);
    }
}

void CExoStreamingSoundSourceInternal::SetPriorityGroup(unsigned char nPriorityGroup)
{
    CExoSoundInternal* pSoundInternal = m_pSoundInternal;
    CExoSoundPriorityGroup* pGroup = &pSoundInternal->m_pPriorityGroups[nPriorityGroup];
    CExoXboxStream* pStream = m_pStream;
    int bPlaying = pStream->m_bPlaying;

    if (pGroup != NULL)
    {
        m_nPriorityGroup = nPriorityGroup;
    }
    else
    {
        pGroup = &pSoundInternal->m_pPriorityGroups[m_nPriorityGroup];
    }
    if (bPlaying == 0)
    {
        pStream->SetPriorityGroup(m_nPriorityGroup);
    }
    else
    {
        CExoXboxStreamCommand cCommand;
        cCommand.m_pStream = m_pStream;
        cCommand.m_nCommand = CExoXboxStreamCommand::COMMAND_SET_PRIORITY_GROUP;
        cCommand.m_fParam1 = (float)m_nPriorityGroup;
        m_pSoundInternal->m_pStreamManager->QueueCommand(cCommand);
    }
    float fMinVolumeDist = pGroup->m_fMinVolumeDist;
    m_fMaxVolumeDist = pGroup->m_fMaxVolumeDist;
    m_fMinVolumeDist = fMinVolumeDist;
}

void CExoStreamingSoundSourceInternal::SetReverbEnabled(int bEnabled)
{
    int bPlaying = m_pStream->m_bPlaying;
    if (bPlaying == 0)
    {
        m_pStream->m_bReverbEnabled = bEnabled;
    }
}

int CExoStreamingSoundSourceInternal::GetWithinRangeOfListener()
{
    if (m_b3D == 0)
    {
        return 1;
    }
    Vector vPosition = m_vPosition;
    Vector* pListenerPosition = &m_pSoundInternal->m_vListenerPosition;
    Vector vListenerPosition = *pListenerPosition;
    return magnitude(vListenerPosition - vPosition) <= m_fMinVolumeDist;
}

void CExoStreamingSoundSourceInternal::SetPitchVariance(float fVariance)
{
    int bPlaying = m_pStream->m_bPlaying;
    if (bPlaying == 0)
    {
        m_pStream->SetPitchVariance(fVariance);
        return;
    }

    CExoXboxStreamCommand cCommand;
    cCommand.m_pStream = m_pStream;
    cCommand.m_nCommand = CExoXboxStreamCommand::COMMAND_SET_PITCH_VARIANCE;
    cCommand.m_fParam1 = fVariance;
    GetSoundInternal()->m_pStreamManager->QueueCommand(cCommand);
}

void CExoStreamingSoundSourceInternal::SetFixedVariance(float fVariance)
{
    int bPlaying = m_pStream->m_bPlaying;
    if (bPlaying == 0)
    {
        m_pStream->SetFixedVariance(fVariance);
        return;
    }

    CExoXboxStreamCommand cCommand;
    cCommand.m_pStream = m_pStream;
    cCommand.m_nCommand = CExoXboxStreamCommand::COMMAND_SET_FIXED_VARIANCE;
    cCommand.m_fParam1 = fVariance;
    GetSoundInternal()->m_pStreamManager->QueueCommand(cCommand);
}

int CExoStreamingSoundSourceInternal::Play(CResRef const& cResRef, unsigned short nResourceType, int bVoice,
                                           int bMusic, int bCached)
{
    int bDirect;

    if (!cResRef.IsValid())
    {
        return 0;
    }

    int bPlaying = m_pStream->m_bPlaying;
    m_resRef = cResRef;
    m_nResourceType = nResourceType;
    cResRef.CopyToString(m_sFileName);
    if (bVoice != 0 && bMusic != 0 && bCached != 0)
    {
        bDirect = 1;
        bVoice = 0;
        bCached = 0;
    }
    else
    {
        bDirect = 0;
    }
    if (bCached != 0)
    {
        m_sFileName.Format("CACHE:STREAMMUSIC\\%s", m_sFileName.CStr());
    }
    else if (bMusic != 0)
    {
        m_sFileName.Format("HD0:STREAMMUSIC\\%s", m_sFileName.CStr());
    }
    else if (bVoice != 0)
    {

        if ((m_sFileName[0] == 'n' || m_sFileName[0] == 'N') && m_sFileName.GetLength() == 16 &&
            m_sFileName[1] != '_')
        {
            char szModule[8];
            char szDialog[8];
            int nChar;

            for (nChar = 0; nChar < 5; nChar++)
            {
                szModule[nChar] = m_sFileName[nChar + 1];
            }
            szModule[nChar] = '\0';
            for (nChar = 0; nChar < 6; nChar++)
            {
                szDialog[nChar] = m_sFileName[nChar + 6];
            }
            szDialog[nChar] = '\0';
            m_sFileName.Format("HD0:STREAMWAVES\\%s\\%s\\%s", szModule, szDialog, m_sFileName.CStr());
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
    m_sFileName = g_pExoBase->m_pExoAliasList->ResolveFileName(m_sFileName, nResourceType);
    if (bDirect != 0)
    {
        bVoice = 1;
        bCached = 1;
    }
    if (bPlaying == 0)
    {
        if ((m_b3D != 0 && GetWithinRangeOfListener()) || m_b3D == 0)
        {
            if (g_bSoundPaused != 0)
            {
                m_pSoundInternal->GetPauseable(this);
            }
            m_pStream->m_bPlaying = 1;
            m_pStream->m_bPlayFailed = 0;

            CExoXboxStreamCommand cCommand;
            cCommand.m_nCommand = CExoXboxStreamCommand::COMMAND_PLAY;
            cCommand.m_pStream = m_pStream;
            cCommand.m_fParam1 = (float)nResourceType;
            cCommand.m_fParam2 = (float)bVoice;
            cCommand.m_fParam3 = (float)bMusic;
            cCommand.m_fParam4 = (float)bCached;
            cCommand.m_sFileName = m_sFileName;
            m_pSoundInternal->m_pStreamManager->QueueCommand(cCommand);
            m_pSoundInternal->AddToStreamingList(this);
            return 1;
        }
        if (m_bLooping != 0)
        {
            m_pSoundInternal->AddToNonPlayingStreamingLoopingList(this);
        }
        return 1;
    }
    return 0;
}
