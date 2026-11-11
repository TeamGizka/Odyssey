#include "exobase.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "worldtimer.h"
#include "exobase.h"
#include "nwmessage.h"
#include "nwcarea.h"
#include "gameobjectarray.h"
#include "clientaimaster.h"
#include "nwcmodule.h"
#include "nwsobject.h"
#include "nwcsoundobject.h"

#include <stdlib.h>

static inline void NwcsoundobjectTrace3();
static inline void NwcsoundobjectTrace2();
static inline void NwcsoundobjectTrace1();

const int CSWCSoundObject::MAX_VOLUME = 127;
// PC 0x0069E3C0
CSWCSoundObject::CSWCSoundObject()
    : CSWCObject(CGameObject::OBJECT_TYPE_SOUND)
{
    m_bStreaming = 0;
    m_pSoundSource = NULL;
    m_pStreamingSource = NULL;
    m_nIntervalTimer = 0;
    m_bPlaying = 1;
    m_nVolume = 127;
    m_nVolumeVariation = 0;
    m_nTimeOfDay = TIME_OF_DAY_ALWAYS;
    m_fPitchVariation = 0.0f;
    m_fFixedVariance = 1.0f;
    m_nHourMask = 0;
    m_bActiveNow = 1;
    m_nGeneratedType = 0;
    m_nInterval = 0;
    m_nIntervalVariation = 0;
    m_bContinuous = 0;
    m_bRandomOrder = 0;
    m_pSounds = new CExoArrayList<CResRef>(0);
    m_nCurrentSound = 0;
    m_bFirstSound = 0;
    m_bRandomPosition = 0;
    m_fRandomRangeX = 0.0f;
    m_fRandomRangeY = 0.0f;
    m_bRestart = 0;
}
// PC 0x0069DE70
void CSWCSoundObject::ChangeVolume(int nVolume)
{
    if (m_bStreaming == 0)
    {
        m_pSoundSource->SetVolume(nVolume);
    }
    else
    {
        m_pStreamingSource->SetVolume(nVolume);
    }
    m_nVolume = nVolume;
}
// PC 0x0069E530
CSWCSoundObject::~CSWCSoundObject()
{
    if (m_pSoundSource != NULL)
    {
        delete m_pSoundSource;
        m_pSoundSource = NULL;
    }
    if (m_pStreamingSource != NULL)
    {
        delete m_pStreamingSource;
        m_pStreamingSource = NULL;
    }
    if (m_pSounds != NULL)
    {
        delete m_pSounds;
        m_pSounds = NULL;
    }
}
// PC 0x0069E5E0
void CSWCSoundObject::LoadFromMessage(CSWCMessage* pMessage)
{
    Vector vPosition;

    SetId(pMessage->ReadOBJECTIDClient());
    int bActive = pMessage->ReadBOOL();
    int bPositional = pMessage->ReadBOOL();
    int bLooping = pMessage->ReadBOOL();

    if (bLooping != 0)
    {
        m_pStreamingSource = new CExoStreamingSoundSource;
        m_bStreaming = 1;
    }
    else
    {
        m_pSoundSource = new CExoSoundSource;
    }
    SetIsActive(bActive);
    SetPositional(bPositional);
    SetLooping(bLooping);

    if (bLooping != 0)
    {
        if (bPositional != 0)
        {
            m_pStreamingSource->SetLooping(1);
            m_pStreamingSource->Set3D(1);
            m_pStreamingSource->SetPriorityGroup(
                EXOSOUND_PRIORITY_STREAMING_SOUND_OBJECT_3D);
        }
        else
        {
            m_pStreamingSource->SetLooping(1);
            m_pStreamingSource->Set3D(0);
            m_pStreamingSource->SetPriorityGroup(
                EXOSOUND_PRIORITY_STREAMING_SOUND_OBJECT_2D);
        }
    }
    else
    {
        if (bPositional != 0)
        {
            m_pSoundSource->SetPriorityGroup(
                EXOSOUND_PRIORITY_SOUND_OBJECT_3D);
        }
        else
        {
            m_pSoundSource->SetPriorityGroup(
                EXOSOUND_PRIORITY_SOUND_OBJECT_2D);
        }
    }

    float fValue;
    m_nVolume = pMessage->ReadBYTE(8);
    m_nVolumeVariation = pMessage->ReadBYTE(8);
    m_nTimeOfDay = pMessage->ReadBYTE(8);
    fValue = pMessage->ReadFLOAT(1.0f, 32);
    m_fPitchVariation = fValue;
    fValue = pMessage->ReadFLOAT(1.0f, 32);
    m_fFixedVariance = fValue;
    m_nHourMask = pMessage->ReadDWORD(32);
    m_nGeneratedType = pMessage->ReadBYTE(8);
    m_nInterval = pMessage->ReadDWORD(32);
    m_nIntervalVariation = pMessage->ReadDWORD(32);
    float fMinDistance = pMessage->ReadFLOAT(1.0f, 32);
    float fMaxDistance = pMessage->ReadFLOAT(1.0f, 32);
    SetDistance(fMinDistance, fMaxDistance);
    m_bContinuous = pMessage->ReadBOOL();
    m_bRandomOrder = pMessage->ReadBOOL();
    m_bRandomPosition = pMessage->ReadBOOL();
    fValue = pMessage->ReadFLOAT(1.0f, 32);
    m_fRandomRangeX = fValue;
    fValue = pMessage->ReadFLOAT(1.0f, 32);
    m_fRandomRangeY = fValue;
    vPosition.m_fX = pMessage->ReadFLOAT(1.0f, 32);
    vPosition.m_fY = pMessage->ReadFLOAT(1.0f, 32);
    vPosition.m_fZ = pMessage->ReadFLOAT(1.0f, 32);
    m_fHeight = vPosition.m_fZ;
    SetPosition(vPosition);

    unsigned short nSounds = pMessage->ReadWORD(16);
    for (unsigned short nSound = 0; nSound < nSounds; nSound++)
    {
        m_pSounds->Add(pMessage->ReadCResRef(16));
    }
    m_bFirstSound = 1;
    m_nSoundTimer = 0;
    if (m_bContinuous != 0 && nSounds == 1 &&
        m_bRandomPosition == 0 && m_nInterval == 0)
    {
        SetLooping(1);
    }
    UpdateIntervalVariation();
}
// PC 0x0069DEB0
void CSWCSoundObject::ChangePitchVariation(float fVariation)
{
    if (m_bStreaming == 0)
    {
        m_pSoundSource->SetPitchVariance(fVariation);
        m_fPitchVariation = fVariation;
    }
    else
    {
        m_fPitchVariation = fVariation;
    }
}
// PC 0x0069DEF0
void CSWCSoundObject::ChangeFixedVariance(float fVariance)
{
    if (m_bStreaming == 0)
    {
        m_pSoundSource->SetFixedVariance(fVariance);
        m_fFixedVariance = fVariance;
    }
    else
    {
        m_fFixedVariance = fVariance;
    }
}
// PC 0x0069DF30
void CSWCSoundObject::ChangePosition(const Vector& vPosition)
{
    Vector vSource = vPosition;

    vSource.m_fZ = vSource.m_fZ + m_fHeight;
    if (m_bStreaming == 0)
    {
        m_pSoundSource->SetPosition(vSource, 0.0f);
    }
    else
    {
        m_pStreamingSource->SetPosition(vSource);
    }
    SetPosition(vSource);
}

inline int CSWCSoundObject::GetLooping()
{
    if (m_bStreaming == 0 && m_pSoundSource != NULL)
    {
        return m_pSoundSource->GetLooping();
    }
    else if (m_pStreamingSource != NULL)
    {
        return m_pStreamingSource->GetLooping();
    }
    return 0;
}
// PC 0x0069DFA0
void CSWCSoundObject::SetIsActive(int bActive)
{
    if (m_bPlaying != bActive)
    {
        m_nIntervalTimer = 0;
        m_bPlaying = bActive;
        if (bActive != 0)
        {
            int bLooping;

            bLooping = GetLooping();
            if (bLooping != 0)
            {
                m_bRestart = 1;
            }
        }
    }
}
// PC 0x0069E000
void CSWCSoundObject::UpdateTimeOfDay()
{
    if (m_nTimeOfDay != TIME_OF_DAY_CUSTOM)
    {
        switch (
            g_pAppManager->GetClientExoApp()->GetModule()->m_nTimeOfDayState)
        {
        case 0:
        case 1:
        case 3:
            m_bActiveNow = (m_nTimeOfDay != TIME_OF_DAY_NIGHT);
            break;
        case 2:
        case 4:
            m_bActiveNow = (m_nTimeOfDay != TIME_OF_DAY_DAY);
            break;
        }
    }
    else
    {
        int nHourMask = m_nHourMask;

        m_bActiveNow =
            (nHourMask &
             (1 << g_pAppManager->GetClientExoApp()->GetModule()->m_nTimeB0)) !=
            0;
    }
}
// PC 0x0069E0A0
unsigned long CSWCSoundObject::GetTimeDiff()
{
    unsigned long nDay;
    unsigned long nTime;
    unsigned long nDiffDay;
    unsigned long nDiffTime;

    g_pAppManager->GetClientExoApp()->GetWorldTimer()->GetWorldTime(&nDay,
                                                                    &nTime);
    g_pAppManager->GetClientExoApp()->GetWorldTimer()->SubtractWorldTimes(
        nDay, nTime, m_nLastUpdateDay, m_nLastUpdateTime, &nDiffDay,
        &nDiffTime);
    return nDiffTime;
}
// PC 0x0069E110
void CSWCSoundObject::UpdateVolumeVariation()
{
    int nVariation = m_nVolumeVariation;

    if (nVariation > 0)
    {
        int nBase = m_nVolume;
        int nVolume = nBase + (rand() % (2 * nVariation)) - nVariation;
        int nFinal;

        if (nVolume > MAX_VOLUME)
        {
            nVolume = MAX_VOLUME;
        }
        nFinal = (nVolume < 0) ? 0 : nVolume;
        if (m_bStreaming == 0)
        {
            m_pSoundSource->SetVolume(nFinal);
        }
        else
        {
            m_pStreamingSource->SetVolume(nFinal);
        }
    }
}
// PC 0x0069E170
void CSWCSoundObject::UpdateIntervalVariation()
{
    if (m_nInterval != 0)
    {
        int nInterval = m_nInterval;
        unsigned long nVariation = m_nIntervalVariation;

        if (nVariation > 0)
        {
            unsigned long nRand = g_pExoBase->m_pExoRand->Rand();
            unsigned long nRemainder = nRand % (2 * nVariation);
            int nDelta = nRemainder - nVariation;

            nInterval += nDelta;
            nInterval = (nInterval < 0) ? 0 : nInterval;
        }
        m_nIntervalTimer = nInterval;
    }
}

void CSWCSoundObject::ResetLastUpdate()
{
    g_pAppManager->GetClientExoApp()->GetWorldTimer()->GetWorldTime(
        &m_nLastUpdateDay, &m_nLastUpdateTime);
}
// PC 0x0069E210
void CSWCSoundObject::Play()
{
    CResRef cResRef;
    int bLooping;

    if (!((m_bStreaming != 0 ||
           m_pSoundSource->IsHardwarePlayingSoundPleaseDontUseThis() != 0) &&
          (m_bStreaming == 0 || m_pStreamingSource->IsPlaying() != 0)))
    {
        m_nSoundTimer = 0;
        m_nIntervalTimer = 0;
        m_bFirstSound = 1;
        ResetLastUpdate();
    }
    if (m_bPlaying == 1)
    {
        return;
    }
    m_nIntervalTimer = 0;
    m_bPlaying = 1;
    bLooping = GetLooping();
    if (bLooping != 0)
    {
        m_bRestart = 1;
    }
}
// PC 0x0069E2F0
void CSWCSoundObject::Stop()
{
    if (m_bPlaying != 0)
    {
        m_nIntervalTimer = 0;
        m_bPlaying = 0;
    }
    if (m_bStreaming == 0)
    {
        m_pSoundSource->Stop();
    }
    else
    {
        m_pStreamingSource->Stop();
    }
    m_nSoundTimer = 0;
    m_nIntervalTimer = 0;
}
// PC 0x0069E350
void CSWCSoundObject::FadeAndStop(unsigned long nFadeTime)
{
    if (m_bPlaying != 0)
    {
        m_nIntervalTimer = 0;
        m_bPlaying = 0;
    }
    if (m_bStreaming == 0)
    {
        m_pSoundSource->Stop();
    }
    else
    {
        m_pStreamingSource->FadeAndStop(nFadeTime);
    }
    m_nSoundTimer = 0;
    m_nIntervalTimer = 0;
}
// PC 0x0069EA60
int CSWCSoundObject::AddToArea(CSWCArea* pArea, CGameObjectArray* pObjectArray)
{
    CClientAIMaster* pAIMaster =
        g_pAppManager->GetClientExoApp()->GetClientAIMaster();
    int bLooping;

    SetArea(pArea);
    pArea->m_lstSoundObjectIds.AddUnique(m_nId);
    if (pObjectArray->AddExternalObject(m_nId, (CGameObject*)this, 0) !=
        CGameObjectArray::SUCCESS)
    {
        return 0;
    }
    if (pAIMaster->AddObject(this) == 0)
    {
        return 0;
    }
    ResetLastUpdate();
    bLooping = GetLooping();
    if (bLooping != 0 && m_bPlaying != 0)
    {
        m_bRestart = 1;
    }
    return 1;
}
// PC 0x0069EB40
void CSWCSoundObject::ShuffleList()
{
    CExoArrayList<CResRef>* pSounds = new CExoArrayList<CResRef>(0);
    int nSounds = m_pSounds->num;

    for (int i = 0; i < nSounds; i++)
    {
        int nSound = rand() % (nSounds - i);

        pSounds->Add(m_pSounds->element[nSound]);
        m_pSounds->DelIndex(nSound);
    }
    delete m_pSounds;
    m_pSounds = pSounds;
}
// PC 0x0069EC90
CResRef CSWCSoundObject::GetNextSound()
{
    NwcsoundobjectTrace3();
    CResRef cResRef;

    if (m_bFirstSound != 0)
    {
        m_bFirstSound = 0;
        m_nCurrentSound = 0;
        cResRef = m_pSounds->element[0];
    }
    else if (m_nCurrentSound < m_pSounds->num - 1)
    {
        m_nCurrentSound++;
        cResRef = m_pSounds->element[m_nCurrentSound];
    }
    else if (m_bContinuous != 0)
    {
        if (m_bRandomOrder != 0)
        {
            ShuffleList();
        }
        m_nCurrentSound = 0;
        cResRef = m_pSounds->element[0];
    }
    else
    {
        cResRef = "0";
    }
    return cResRef;
}
// PC 0x0069ED90
void CSWCSoundObject::AIUpdate()
{
    CResRef cResRef;

    if (m_bPlaying == 0)
    {
        if (m_bStreaming != 0)
        {
            if (m_pStreamingSource->IsPlaying() != 0)
            {
                m_pStreamingSource->Stop();
            }
        }
        else if (m_pSoundSource->IsHardwarePlayingSoundPleaseDontUseThis() != 0)
        {
            m_pSoundSource->Stop();
        }
        return;
    }

    if (m_nTimeOfDay == TIME_OF_DAY_CUSTOM)
    {
        UpdateTimeOfDay();
    }
    if (m_bActiveNow == 0)
    {
        if (m_bStreaming == 0)
        {
            if (m_pSoundSource->IsHardwarePlayingSoundPleaseDontUseThis() != 0)
            {
                m_pSoundSource->Stop();
            }
        }
        else
        {
            m_pStreamingSource->Stop();
        }
        return;
    }

    int bLooping = GetLooping();

    if (bLooping != 0)
    {
        if (m_bRestart != 0)
        {
            m_bRestart = 0;
            m_bFirstSound = 1;
            if (m_bStreaming == 0)
            {
                m_pSoundSource->SetResRef(GetNextSound(), 0);
                m_pSoundSource->SetVolume(m_nVolume);
                m_nSoundTimer = m_pSoundSource->Play();
            }
            else
            {
                m_pStreamingSource->SetVolume(m_nVolume);
                m_nSoundTimer = m_pStreamingSource->Play(GetNextSound(), 4, 0,
                                                        0, 0);
            }
            if (m_nSoundTimer == 0 && m_bPlaying != 0)
            {
                m_nIntervalTimer = 0;
                m_bPlaying = 0;
            }
        }
        return;
    }

    unsigned long nTimeDiff = GetTimeDiff();

    ResetLastUpdate();
    if (m_nSoundTimer != 0)
    {
        if (nTimeDiff >= m_nSoundTimer)
        {
            m_nSoundTimer = 0;
            nTimeDiff = 0;
        }
        else
        {
            m_nSoundTimer -= nTimeDiff;
            return;
        }
    }
    if (m_nIntervalTimer != 0)
    {
        if (nTimeDiff >= m_nIntervalTimer)
        {
            m_nIntervalTimer = 0;
        }
        else
        {
            m_nIntervalTimer -= nTimeDiff;
            return;
        }
    }

    if ((cResRef = GetNextSound()) == "0")
    {
        if (m_bPlaying != 0)
        {
            m_nIntervalTimer = 0;
            m_bPlaying = 0;
        }
        return;
    }

    if (m_bStreaming == 0)
    {
        m_pSoundSource->SetResRef(cResRef, 0);
        m_pSoundSource->SetVolume(m_nVolume);
        UpdateVolumeVariation();
        float fPitchVariation = m_fPitchVariation;
        m_pSoundSource->SetPitchVariance(fPitchVariation);
        float fFixedVariance = m_fFixedVariance;
        m_pSoundSource->SetFixedVariance(fFixedVariance);
        m_nSoundTimer = m_pSoundSource->Play();
    }
    else
    {
        m_pStreamingSource->SetVolume(m_nVolume);
        UpdateVolumeVariation();
        m_nSoundTimer = m_pStreamingSource->Play(cResRef, RESTYPE_WAV, 0, 0, 0);
    }
    if (m_nSoundTimer == 0)
    {
        if (m_pSounds->num > 1)
        {
            m_pSounds->DelIndex(m_nCurrentSound);
            if (m_nCurrentSound > 0)
            {
                m_nCurrentSound--;
            }
        }
        else
        {
            if (m_bPlaying != 0)
            {
                m_nIntervalTimer = 0;
                m_bPlaying = 0;
            }
            return;
        }
    }
    if (m_bContinuous != 0 || m_nCurrentSound < m_pSounds->num - 1)
    {
        UpdateIntervalVariation();
    }
}

static inline void NwcsoundobjectTrace3()
{
    NwcsoundobjectTrace2();
}

static inline void NwcsoundobjectTrace2()
{
    NwcsoundobjectTrace1();
}

static inline void NwcsoundobjectTrace1()
{
}
