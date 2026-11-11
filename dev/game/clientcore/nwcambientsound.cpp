#include "exobase.h"
#include "exoresman.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "clientoptions.h"
#include "nwcmodule.h"
#include "worldtimer.h"
#include "nwmessage.h"
#include "nwcambientsound.h"
#include "nwcanimbase.h"
#include "nwrules.h"
#include "twodimarrays.h"
#include "twodimarray.h"
#include <stdlib.h>
#ifdef _XBOX

#define RESTYPE_BMU 11
#define RESTYPE_MP3 11
#endif

const int CSWCAmbientSound::BATTLE_DELAY = 1000;
const int CSWCAmbientSound::MUSIC_FADE_DURATION = 800;
const unsigned long CSWCAmbientSound::BATTLE_FADE_DURATION = 800;
const int CSWCAmbientSound::AMBIENT_FADE_DURATION = 6000;
const int CSWCAmbientSound::TRUNCATED_FADE_DURATION = 1000;
const int CSWCAmbientSound::STREAM_DOESNT_EXIST_DELAY = 10000;
// PC 0x0068D3D0  Xbox 0x000B4600
inline CSWCAmbientSoundTrack::CSWCAmbientSoundTrack()
{
}
// PC 0x0068D430  Xbox 0x000B4660
inline CSWCAmbientSoundTrack::~CSWCAmbientSoundTrack()
{
}
// PC 0x0068D4A0  Xbox 0x000B46D0
void CSWCAmbientSound::StartSounds()
{
    m_bSoundsStarted = 1;
    m_bSoundEnabled =
        g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nAmbientSoundOptions &
        CClientOptions::AMBIENT_SOUND_OPTION_MASK;

    if (m_bStarted != 0)
    {
        if (m_bBattleMusic != 0)
        {
            PlayBattleMusic(1);
        }
        else
        {
            PlayMusic(m_bMusicEnabled);
        }
    }

    if (m_bAmbientEnabled != 0)
    {
        PlayAmbientSound(m_bAmbientPlaying);
    }
}
// PC 0x0068D510  Xbox 0x000B4740
void CSWCAmbientSound::SetMusicDelay(int nDelay)
{
    if (nDelay > 0)
    {
        m_MusicDelay = nDelay;
    }
}

void CSWCAmbientSound::SetMusicVolume(unsigned char nVolume)
{
    if (nVolume >= CExoSound::CMAX_VOLUME)
    {
        nVolume = CExoSound::CMAX_VOLUME;
    }
    m_pMusicSource->SetVolume(nVolume);
}
// PC 0x0068D530  Xbox 0x000B4780
void CSWCAmbientSound::SetAmbientDayVolume(int nVolume)
{
    CExoString sPlayingFile;

    m_pAmbientSource->GetFilePlaying().CopyToString(sPlayingFile);

    if (m_nAmbientDayVolume != nVolume)
    {
        if (m_pAmbientSource->IsPlaying() != 0 &&
            sPlayingFile.CompareNoCase(m_AmbientDay.m_sSound) != 0)
        {
            m_pAmbientSource->SetVolume(nVolume);
        }
        m_nAmbientDayVolume = nVolume;
    }
}
// PC 0x0068D5E0  Xbox 0x000B4830
void CSWCAmbientSound::SetAmbientNightVolume(int nVolume)
{
    CExoString sPlayingFile;

    m_pAmbientSource->GetFilePlaying().CopyToString(sPlayingFile);

    if (m_nAmbientNightVolume != nVolume)
    {
        if (m_pAmbientSource->IsPlaying() != 0 &&
            sPlayingFile.CompareNoCase(m_AmbientNight.m_sSound) != 0)
        {
            m_pAmbientSource->SetVolume(nVolume);
        }
        m_nAmbientNightVolume = nVolume;
    }
}
// PC 0x0068D690  Xbox 0x000B48E0
void CSWCAmbientSound::UpdateTimeOfDay()
{
    CExoString sMusicFile;
    CExoString sAmbientFile;

    m_pMusicSource->GetFilePlaying().CopyToString(sMusicFile);
    m_pAmbientSource->GetFilePlaying().CopyToString(sAmbientFile);

    switch (g_pAppManager->m_pClientExoApp->GetModule()->m_nTimeOfDayState)
    {
    case CSWCModule::TIME_DEFAULT:
    case CSWCModule::TIME_DAY:
    case CSWCModule::TIME_DAWN:
        if (m_bDaytime != 1)
        {
            if (sMusicFile.CompareNoCase(m_MusicNight.m_sSound) != 0 ||
                (m_bSoundEnabled != 0 &&
                 sMusicFile.CompareNoCase(m_AmbientNight.m_sSound) != 0))
            {
                PlayMusic(0);
                PlayMusic(1);
            }
            if (sAmbientFile.CompareNoCase(m_AmbientNight.m_sSound) != 0)
            {
                PlayAmbientSound(0);
                PlayAmbientSound(1);
            }
        }
        m_bDaytime = 1;
        break;

    case CSWCModule::TIME_NIGHT:
    case CSWCModule::TIME_DUSK:
        if (m_bDaytime != 0)
        {
            if (sMusicFile.CompareNoCase(m_MusicDay.m_sSound) != 0 ||
                (m_bSoundEnabled != 0 &&
                 sMusicFile.CompareNoCase(m_AmbientDay.m_sSound) != 0))
            {
                PlayMusic(0);
                PlayMusic(1);
            }
            if (sAmbientFile.CompareNoCase(m_AmbientDay.m_sSound) != 0)
            {
                PlayAmbientSound(0);
                PlayAmbientSound(1);
            }
        }
        m_bDaytime = 0;
        break;
    }
}
// PC 0x0068E1F0  Xbox 0x000B5500
void CSWCAmbientSound::PlayMusic(int bPlay)
{
    if (m_bSoundsStarted != 0)
    {
        if (m_bSoundEnabled != 0)
        {
            PlayTruncated(bPlay, AMBIENT_SOUND_PRIORITY_MUSIC);
        }
        else if (bPlay == 0)
        {
            m_pMusicSource->FadeAndStop(MUSIC_FADE_DURATION);
            m_bMusicEnabled = 0;
        }
        else if (m_bStarted != 0)
        {
            if (m_pMusicSource->IsPlaying() == 0)
            {
                m_nTimeRemaining = 0;
                m_pMusicSource->SetPriorityGroup(
                    CExoStreamingSoundSource::PRIORITY_GROUP_MUSIC);
                int bPlaying;
                if (m_bDaytime != 0)
                {
                    bPlaying = m_pMusicSource->Play(m_MusicDay.m_sSound,
                                                    RESTYPE_BMU, 0, 1, 0);
                }
                else
                {
                    bPlaying = m_pMusicSource->Play(m_MusicNight.m_sSound,
                                                    RESTYPE_BMU, 0, 1, 0);
                }
                if (!bPlaying)
                {
                    m_nTimeRemaining = STREAM_DOESNT_EXIST_DELAY;
                }
            }
            m_bMusicEnabled = 1;
        }
    }
}
// PC 0x0068E490  Xbox 0x000B5780
void CSWCAmbientSound::SetMusicDayTrack(int nTrack)
{
    CExoString sPlayingFile;

    m_pMusicSource->GetFilePlaying().CopyToString(sPlayingFile);
    if (m_pMusicSource->IsPlaying() != 0 &&
        sPlayingFile.CompareNoCase(m_MusicDay.m_sSound) != 0)
    {
        PlayMusic(0);
    }
    ReadInfo(1, nTrack, &m_MusicDay);
    m_nMusicDayTrack = nTrack;
    PlayMusic(1);
    m_nTimeRemaining = 1;
}
// PC 0x0068E550  Xbox 0x000B5840
void CSWCAmbientSound::SetMusicNightTrack(int nTrack)
{
    CExoString sPlayingFile;

    m_pMusicSource->GetFilePlaying().CopyToString(sPlayingFile);
    if (m_pMusicSource->IsPlaying() != 0 &&
        sPlayingFile.CompareNoCase(m_MusicNight.m_sSound) != 0)
    {
        PlayMusic(0);
    }
    ReadInfo(1, nTrack, &m_MusicNight);
    m_nMusicNightTrack = nTrack;
    PlayMusic(1);
    m_nTimeRemaining = 1;
}
// PC 0x0068E610  Xbox 0x000B5900
void CSWCAmbientSound::PlayBattleMusic(int bPlay)
{
    CResRef cResRef;
    CExoString sPlayingFile;

    if (m_bSoundEnabled != 0)
    {
        PlayTruncated(bPlay, AMBIENT_SOUND_PRIORITY_BATTLE);
    }
    else if (bPlay == 0)
    {
        if (m_bBattleMusic != 0)
        {
            m_pMusicSource->FadeAndStop(BATTLE_FADE_DURATION);
            if (m_bStarted != 0)
            {
                PlayStinger(&m_BattleMusic);
                PlayMusic(1);
            }
            m_bBattleMusic = 0;
        }
    }
    else if (m_bStarted != 0 && m_bSoundsStarted != 0 &&
             m_BattleMusic.m_sSound != "")
    {
        m_bBattleMusic = 1;
        if (m_pMusicSource->IsPlaying() != 0)
        {
            m_pMusicSource->GetFilePlaying().CopyToString(sPlayingFile);
            if (sPlayingFile.CompareNoCase(m_BattleMusic.m_sSound) == 0)
            {
                m_pMusicSource->FadeAndStop(BATTLE_FADE_DURATION);
                m_bMusicEnabled = 0;
                m_nTimeRemaining = 10;
                ResetLastUpdate();
            }
        }
        else
        {
            m_pMusicSource->SetPriorityGroup(2);
            CResRef cBattleResRef(m_BattleMusic.m_sSound);
            if (m_pMusicSource->Play(cBattleResRef, RESTYPE_BMU, 0, 1, 0) == 0)
            {
                m_nTimeRemaining = STREAM_DOESNT_EXIST_DELAY;
            }
            else
            {
                m_nTimeRemaining = 0;
            }
        }
    }
}
// PC 0x0068E940  Xbox 0x000B5C40
void CSWCAmbientSound::SetBattleMusicTrack(int nTrack)
{
    CExoString sPlayingFile;
    int bPlaying;

    m_pMusicSource->GetFilePlaying().CopyToString(sPlayingFile);
    bPlaying = m_pMusicSource->IsPlaying() != 0 &&
        sPlayingFile.CompareNoCase(m_BattleMusic.m_sSound) != 0;
    if (bPlaying != 0)
    {
        PlayBattleMusic(0);
    }
    ReadInfo(1, nTrack, &m_BattleMusic);
    m_nBattleMusicTrack = nTrack;
    if (bPlaying != 0)
    {
        PlayBattleMusic(1);
    }
}
// PC 0x0068EA10  Xbox 0x000B5D10
void CSWCAmbientSound::PlayAmbientSound(int bPlay)
{
    if (m_bSoundsStarted)
    {
        if (m_bSoundEnabled)
        {
            PlayTruncated(bPlay, AMBIENT_SOUND_PRIORITY_AMBIENT);
        }
        else if (!bPlay)
        {
            m_pAmbientSource->FadeAndStop(AMBIENT_FADE_DURATION);
            m_bAmbientPlaying = 0;
            m_bAmbientFallback = 0;
        }
        else if (m_bAmbientEnabled)
        {
            if (!m_pAmbientSource->IsPlaying())
            {
                if (m_bDaytime)
                {
                    m_pAmbientSource->SetVolume(m_nAmbientDayVolume);
                    m_pAmbientSource->SetPriorityGroup(SOUND_PRIORITY_GROUP_AMBIENT);
                    {
                        CResRef cResRef(m_AmbientDay.m_sSound);
                        m_bAmbientFallback =
                            !m_pAmbientSource->Play(cResRef, RESTYPE_MP3, 0, 1, 0);
                    }
                    if (m_bAmbientFallback)
                    {
                        CResRef cResRef(m_AmbientDay.m_sSound);
                        m_bAmbientFallback =
                            !m_pAmbientSource->Play(cResRef, RESTYPE_WAV, 0, 1, 0);
                    }
                }
                else
                {
                    m_pAmbientSource->SetVolume(m_nAmbientNightVolume);
                    m_pAmbientSource->SetPriorityGroup(SOUND_PRIORITY_GROUP_AMBIENT);
                    {
                        CResRef cResRef(m_AmbientNight.m_sSound);
                        m_bAmbientFallback =
                            !m_pAmbientSource->Play(cResRef, RESTYPE_MP3, 0, 1, 0);
                    }
                    if (m_bAmbientFallback)
                    {
                        CResRef cResRef(m_AmbientNight.m_sSound);
                        m_bAmbientFallback =
                            !m_pAmbientSource->Play(cResRef, RESTYPE_WAV, 0, 1, 0);
                    }
                }
            }
            m_bAmbientPlaying = 1;
        }
    }
}
// PC 0x0068ED00  Xbox 0x000B6000
void CSWCAmbientSound::SetAmbientDayTrack(int nTrack)
{
    CExoString sPlayingFile;

    m_pAmbientSource->GetFilePlaying().CopyToString(sPlayingFile);
    if (m_pAmbientSource->IsPlaying() != 0 &&
        sPlayingFile.CompareNoCase(m_AmbientDay.m_sSound) != 0)
    {
        PlayAmbientSound(0);
    }
    ReadInfo(0, nTrack, &m_AmbientDay);
    m_nAmbientDayTrack = nTrack;
    PlayAmbientSound(1);
}
// PC 0x0068EDC0  Xbox 0x000B60C0
void CSWCAmbientSound::SetAmbientNightTrack(int nTrack)
{
    CExoString sPlayingFile;

    m_pAmbientSource->GetFilePlaying().CopyToString(sPlayingFile);
    if (m_pAmbientSource->IsPlaying() != 0 &&
        sPlayingFile.CompareNoCase(m_AmbientNight.m_sSound) != 0)
    {
        PlayAmbientSound(0);
    }
    ReadInfo(0, nTrack, &m_AmbientNight);
    m_nAmbientNightTrack = nTrack;
    PlayAmbientSound(1);
}
// PC 0x0068EE80  Xbox 0x000B6180
void CSWCAmbientSound::LoadFromMessage(CSWCMessage* pMessage)
{
    m_bMusicEnabled = pMessage->ReadBOOL();
    m_bBattleMusic = pMessage->ReadBOOL();
    m_bAmbientPlaying = pMessage->ReadBOOL();
    m_MusicDelay = pMessage->ReadINT(32);
    m_MusicDelay = m_MusicDelay > 1 ? m_MusicDelay : 1;
    m_nMusicDayTrack = pMessage->ReadINT(32);
    m_nMusicNightTrack = pMessage->ReadINT(32);
    m_nBattleMusicTrack = pMessage->ReadINT(32);
    m_nAmbientDayTrack = pMessage->ReadINT(32);
    m_nAmbientNightTrack = pMessage->ReadINT(32);
    m_nAmbientDayVolume = pMessage->ReadBYTE(8);
    m_nAmbientNightVolume = pMessage->ReadBYTE(8);

    ReadInfo(1, m_nMusicDayTrack, &m_MusicDay);
    ReadInfo(1, m_nMusicNightTrack, &m_MusicNight);
    ReadInfo(1, m_nBattleMusicTrack, &m_BattleMusic);
    ReadInfo(0, m_nAmbientDayTrack, &m_AmbientDay);
    ReadInfo(0, m_nAmbientNightTrack, &m_AmbientNight);
}
// PC 0x0068D860  Xbox 0x000B4AB0
void CSWCAmbientSound::ReadInfo(int bMusic, int nRow,
                              CSWCAmbientSoundTrack* pTrack)
{
    CExoString sResource;

    if (bMusic != 0)
    {
        CTwoDimArrays* pArrays = g_pRules->m_p2DArrays;
        EXOXBOXASSERT(pArrays->m_pAmbientMusic);
        C2DA* pMusic = pArrays->m_pAmbientMusic;

        if (pMusic->GetCExoStringEntry(nRow, "Resource", &sResource))
        {
            pTrack->m_sSound = sResource.UpperCase();
        }
        else
        {
            pTrack->m_sSound = "";
        }

        if (pMusic->GetCExoStringEntry(nRow, "Stinger1", &sResource))
        {
            pTrack->m_sStinger1 = sResource.UpperCase();
        }
        else
        {
            pTrack->m_sStinger1 = "";
        }

        if (pMusic->GetCExoStringEntry(nRow, "Stinger2", &sResource))
        {
            pTrack->m_sStinger2 = sResource.UpperCase();
        }
        else
        {
            pTrack->m_sStinger2 = "";
        }

        if (pMusic->GetCExoStringEntry(nRow, "Stinger3", &sResource))
        {
            pTrack->m_sStinger3 = sResource.UpperCase();
        }
        else
        {
            pTrack->m_sStinger3 = "";
        }
    }
    else
    {
        CTwoDimArrays* pSoundArrays = g_pRules->m_p2DArrays;
        EXOXBOXASSERT(pSoundArrays->m_pAmbientSound);
        C2DA* pSound = pSoundArrays->m_pAmbientSound;

        if (pSound->GetCExoStringEntry(nRow, "Resource", &sResource))
        {
            pTrack->m_sSound = sResource.UpperCase();
            m_bAmbientFallback = 0;
        }
        else
        {
            pTrack->m_sSound = "";
        }
    }
}

static void DeleteStreamingSource(CExoStreamingSoundSource* pSource)
{
    delete pSource;
}
// PC 0x0068DB30  Xbox 0x000B4DA0
void CSWCAmbientSound::PlayStinger(CSWCAmbientSoundTrack* pTrack)
{
    CResRef cStinger;

    if (m_bSoundsStarted != 0)
    {
        if (pTrack->m_sStinger1 != "")
        {
            int nStingers = 1;
            if (pTrack->m_sStinger2 != "")
            {
                nStingers = 2;
                if (pTrack->m_sStinger3 != "")
                {
                    nStingers = 3;
                }
            }

            switch (rand() % nStingers)
            {
            case 0:
                cStinger = pTrack->m_sStinger1;
                break;
            case 1:
                cStinger = pTrack->m_sStinger2;
                break;
            case 2:
                cStinger = pTrack->m_sStinger3;
                break;
            }

            delete m_pStingerSource;
            m_pStingerSource = new CExoStreamingSoundSource;
            m_pStingerSource->SetPriorityGroup(
                CExoStreamingSoundSource::PRIORITY_GROUP_STINGER);
            if (!m_pStingerSource->Play(cStinger, RESTYPE_WAV, 0, 0, 0))
            {
                delete m_pStingerSource;
                m_pStingerSource = NULL;
            }
        }
    }
}
// PC 0x0068DC90  Xbox 0x000B4F20
void CSWCAmbientSound::ResetLastUpdate()
{
    CClientExoApp* pClientExoApp;

    pClientExoApp = g_pAppManager->m_pClientExoApp;
    pClientExoApp->GetPauseTimer()->GetWorldTime(
        &m_nLastUpdateLow, &m_nLastUpdateHigh);
}
// PC 0x0068EF80  Xbox 0x000B6280
void CSWCAmbientSound::Update()
{
    unsigned long nTimeDiff;

#ifdef _XBOX
    if (!m_pMusicSource->IsPlaying())
#else
    if (!m_pMusicSource->IsPlaying() &&
        g_pExoSound->GetSoundMode() != CExoSound::CEXOSOUND_MODE_PAUSE_ALL &&
        !g_pExoSound->GetLostFocusPaused())
#endif
    {
        if (m_nTimeRemaining > 0)
        {
            nTimeDiff = GetTimeDiff();
            if (nTimeDiff >= m_nTimeRemaining)
            {
                m_nTimeRemaining = 0;
                if (!m_bSoundEnabled)
                {
                    if (m_bBattleMusic)
                    {
                        PlayBattleMusic(1);
                    }
                    else
                    {
                        PlayMusic(1);
                    }
                }
                else
                {
                    PlayTruncated(1, AMBIENT_SOUND_PRIORITY_LOW);
                }
            }
            else
            {
                m_nTimeRemaining -= nTimeDiff;
                ResetLastUpdate();
            }
        }
        else if (m_bMusicEnabled || m_bBattleMusic)
        {
            if (!m_bSoundEnabled)
            {
                if (m_bBattleMusic)
                {
                    m_nTimeRemaining = BATTLE_DELAY;
                }
                else
                {
                    m_nTimeRemaining = m_MusicDelay > 1 ? m_MusicDelay : 1;
                }
            }
            else
            {
                m_nTimeRemaining = 10;
            }
            ResetLastUpdate();
        }
    }

#ifdef _XBOX
    if (!m_bSoundEnabled &&
        !m_pAmbientSource->IsPlaying() &&
        m_bAmbientPlaying &&
        !m_bAmbientFallback)
#else
    if (!m_bSoundEnabled &&
        !m_pAmbientSource->IsPlaying() &&
        g_pExoSound->GetSoundMode() != CExoSound::CEXOSOUND_MODE_PAUSE_ALL &&
        g_pExoSound->GetSoundMode() !=
            CExoSound::CEXOSOUND_MODE_PAUSE_ALL_BUT_MUSIC &&
        m_bAmbientPlaying &&
        !m_bAmbientFallback &&
        !g_pExoSound->GetLostFocusPaused())
#endif
    {
        PlayAmbientSound(1);
    }
}
// PC 0x0068DCC0  Xbox 0x000B4F50
int CSWCAmbientSound::GetTimeDiff()
{
    CClientExoApp* pClientExoApp;
    unsigned long nCurrentDay;
    unsigned long nCurrentTime;
    unsigned long nDifferenceDay;
    unsigned long nDifferenceTime;

    pClientExoApp = g_pAppManager->m_pClientExoApp;
    pClientExoApp->GetPauseTimer()->GetWorldTime(&nCurrentDay, &nCurrentTime);
    pClientExoApp = g_pAppManager->m_pClientExoApp;
    pClientExoApp->GetPauseTimer()->SubtractWorldTimes(
        nCurrentDay, nCurrentTime, m_nLastUpdateLow, m_nLastUpdateHigh,
        &nDifferenceDay, &nDifferenceTime);
    return nDifferenceTime;
}
// PC 0x0068DD30  Xbox 0x000B4FC0
void CSWCAmbientSound::PlayTruncated(int bPlay, CSWCAmbientSoundPriority nPriority)
{
    switch (nPriority)
    {
    case AMBIENT_SOUND_PRIORITY_AMBIENT:
        m_bAmbientPlaying = bPlay;
        break;
    case AMBIENT_SOUND_PRIORITY_BATTLE:
        m_bBattleMusic = bPlay;
        break;
    case AMBIENT_SOUND_PRIORITY_MUSIC:
        m_bMusicEnabled = bPlay;
        break;
    }

    m_nTimeRemaining = 10;
    if (m_pMusicSource->IsPlaying())
    {
        if (nPriority > m_nMusicPlayingType)
        {
            if (bPlay)
            {
                m_pMusicSource->FadeAndStop(TRUNCATED_FADE_DURATION);
            }
        }
        else if (nPriority == m_nMusicPlayingType && !bPlay)
        {
            m_pMusicSource->FadeAndStop(TRUNCATED_FADE_DURATION);
            if (nPriority == AMBIENT_SOUND_PRIORITY_BATTLE)
            {
                PlayStinger(&m_BattleMusic);
            }
        }
    }
    else if (bPlay)
    {
        if (m_bBattleMusic)
        {
            if (!m_pMusicSource->Play(m_BattleMusic.m_sSound, RESTYPE_BMU, 0, 1, 0))
            {
                m_nTimeRemaining = STREAM_DOESNT_EXIST_DELAY;
            }
            m_nMusicPlayingType = MUSIC_PLAYING_BATTLE;
        }
        else if (m_bAmbientPlaying)
        {
            if (m_bDaytime)
            {
                if (!m_pMusicSource->Play(m_AmbientDay.m_sSound, RESTYPE_WAV, 0, 1, 0))
                {
                    m_nTimeRemaining = STREAM_DOESNT_EXIST_DELAY;
                }
                m_nMusicPlayingType = MUSIC_PLAYING_AMBIENT;
            }
            else
            {
                if (!m_pMusicSource->Play(m_AmbientNight.m_sSound, RESTYPE_WAV, 0, 1, 0))
                {
                    m_nTimeRemaining = STREAM_DOESNT_EXIST_DELAY;
                }
                m_nMusicPlayingType = MUSIC_PLAYING_AMBIENT;
            }
        }
        else if (m_bMusicEnabled)
        {
            if (m_bDaytime)
            {
                if (!m_pMusicSource->Play(m_MusicDay.m_sSound, RESTYPE_BMU, 0, 1, 0))
                {
                    m_nTimeRemaining = STREAM_DOESNT_EXIST_DELAY;
                }
                m_nMusicPlayingType = MUSIC_PLAYING_MUSIC;
            }
            else
            {
                if (!m_pMusicSource->Play(m_MusicNight.m_sSound, RESTYPE_BMU, 0, 1, 0))
                {
                    m_nTimeRemaining = STREAM_DOESNT_EXIST_DELAY;
                }
                m_nMusicPlayingType = MUSIC_PLAYING_MUSIC;
            }
        }
        else
        {
            m_nTimeRemaining = STREAM_DOESNT_EXIST_DELAY;
            m_nMusicPlayingType = MUSIC_PLAYING_NONE;
        }
    }
}
// PC 0x0068DF40  Xbox 0x000B5250
CSWCAmbientSound::CSWCAmbientSound(CSWCArea* pArea)
{
    m_nTimeRemaining = 0;
    m_MusicDelay = 30000;
    m_pMusicSource = new CExoStreamingSoundSource;
    m_pAmbientSource = new CExoStreamingSoundSource;
    m_pMusicSource->SetLooping(0);
    m_pAmbientSource->SetLooping(1);
    m_pAmbientSource->SetPriorityGroup(SOUND_PRIORITY_GROUP_AMBIENT);
    m_bStarted = 1;
    m_bAmbientEnabled = 1;
    m_bSoundsStarted = 0;
    m_pArea = pArea;
    m_bDaytime = 1;
    m_bSoundEnabled = 0;
    m_nMusicPlayingType = MUSIC_PLAYING_NONE;
    m_pStingerSource = NULL;
    m_bAmbientFallback = 0;
}
// PC 0x0068E0D0  Xbox 0x000B53E0
CSWCAmbientSound::~CSWCAmbientSound()
{
    if (m_pMusicSource != NULL)
    {
        delete m_pMusicSource;
        m_pMusicSource = NULL;
    }
    if (m_pAmbientSource != NULL)
    {
        delete m_pAmbientSource;
        m_pAmbientSource = NULL;
    }
}
// PC 0x0068E1B0  Xbox 0x000B54C0
void CSWCAmbientSound::StopSounds(int bMusicOnly)
{
    PlayMusic(0);
    m_bBattleMusic = 0;
    m_nTimeRemaining = 0;
    if (bMusicOnly != 0)
    {
        m_pAmbientSource->Stop();
        m_bAmbientPlaying = 0;
        m_bAmbientFallback = 0;
    }
    else
    {
        PlayAmbientSound(0);
    }
}
