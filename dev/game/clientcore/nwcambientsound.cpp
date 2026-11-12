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
const CExoString CSWCAmbientSound::THUNDER1("as_wt_thundercl1");
const CExoString CSWCAmbientSound::THUNDER2("as_wt_thundercl2");
const CExoString CSWCAmbientSound::THUNDER_ROLLING1("as_wt_thunderds1");
const CExoString CSWCAmbientSound::THUNDER_ROLLING2("as_wt_thunderds2");
// PC 0x006E6780
inline CSWCAmbientSoundTrack::CSWCAmbientSoundTrack()
{
}
// PC 0x006E67E0
inline CSWCAmbientSoundTrack::~CSWCAmbientSoundTrack()
{
}
// PC 0x006E6850
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
// PC 0x006E68C0
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
// PC 0x006E68E0
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
// PC 0x006E6990
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
// PC 0x006E6A40
void CSWCAmbientSound::Thunder(int bRolling)
{
    if (!m_bAmbientPlaying)
    {
        return;
    }
    if (bRolling)
    {
        if (rand() % 2 == 0)
        {
            CResRef cSound(THUNDER_ROLLING1);
            g_pExoSound->PlayOneShotSound(
                cSound, CExoSound::PRIORITY_GROUP_POWERED_ITEM, 0, 0, 0.0f, 0.0f);
            return;
        }
        CResRef cSound(THUNDER_ROLLING2);
        g_pExoSound->PlayOneShotSound(
            cSound, CExoSound::PRIORITY_GROUP_POWERED_ITEM, 0, 0, 0.0f, 0.0f);
    }
    else
    {
        if (rand() % 2)
        {
            CResRef cSound(THUNDER2);
            g_pExoSound->PlayOneShotSound(
                cSound, CExoSound::PRIORITY_GROUP_POWERED_ITEM, 0, 0, 0.0f, 0.0f);
            return;
        }
        CResRef cSound(THUNDER1);
        g_pExoSound->PlayOneShotSound(
            cSound, CExoSound::PRIORITY_GROUP_POWERED_ITEM, 0, 0, 0.0f, 0.0f);
    }
}
// PC 0x006E6B10
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
// PC 0x006E7690
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
#ifndef _XBOX
                CExoString& sMusic = m_bDaytime ? m_MusicDay.m_sSound : m_MusicNight.m_sSound;
                CExoStreamingSoundSource* pSource =
                    g_pAppManager->GetClientExoApp()->AquireLoadMusic(sMusic);
                if (pSource != 0)
                {
                    delete m_pMusicSource;
                    m_pMusicSource = pSource;
                    m_pMusicSource->SetLooping(0);
                    m_pMusicSource->SetPriorityGroup(
                        CExoStreamingSoundSource::PRIORITY_GROUP_MUSIC);
                }
                else if (!m_pMusicSource->Play(sMusic, RESTYPE_BMU, 0, 1, 0))
                {
                    m_nTimeRemaining = STREAM_DOESNT_EXIST_DELAY;
                }
#else
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
#endif
            }
            m_bMusicEnabled = 1;
        }
    }
}
// PC 0x006E7980
void CSWCAmbientSound::SetMusicDayTrack(int nTrack,
                                        int bStreamingMusicForMovie)
{
    CExoString sPlayingFile;

    m_pMusicSource->GetFilePlaying().CopyToString(sPlayingFile);
    m_pMusicSource->SetStreamingMusicForMovie(bStreamingMusicForMovie);
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
// PC 0x006E7A50
void CSWCAmbientSound::SetMusicNightTrack(int nTrack,
                                        int bStreamingMusicForMovie)
{
    CExoString sPlayingFile;

    m_pMusicSource->GetFilePlaying().CopyToString(sPlayingFile);
    m_pMusicSource->SetStreamingMusicForMovie(bStreamingMusicForMovie);
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
// PC 0x006E7B20
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
            m_pMusicSource->SetLooping(0);
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
// PC 0x006E7E60
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
// PC 0x006E7F30
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
// PC 0x006E8220
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
// PC 0x006E82E0
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
// PC 0x006E83A0
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
// PC 0x006E6CE0
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
// PC 0x006E6FB0
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
            m_pStingerSource = new CExoStreamingSoundSource(1);
            m_pStingerSource->SetPriorityGroup(
                CExoStreamingSoundSource::PRIORITY_GROUP_STINGER);
            if (!m_pStingerSource->Play(cStinger, RESTYPE_BMU, 0, 1, 0))
            {
                if (!m_pStingerSource->Play(cStinger, RESTYPE_WAV, 0, 0, 0))
                {
                    delete m_pStingerSource;
                    m_pStingerSource = NULL;
                }
            }
        }
    }
}
// PC 0x006E7130
void CSWCAmbientSound::ResetLastUpdate()
{
    CClientExoApp* pClientExoApp;

    pClientExoApp = g_pAppManager->m_pClientExoApp;
    pClientExoApp->GetPauseTimer()->GetWorldTime(
        &m_nLastUpdateLow, &m_nLastUpdateHigh);
}
// PC 0x006E84A0
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
// PC 0x006E7160
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
// PC 0x006E71D0
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
// PC 0x006E73E0
CSWCAmbientSound::CSWCAmbientSound(CSWCArea* pArea)
{
    m_nTimeRemaining = 0;
    m_MusicDelay = 30000;
    m_pMusicSource = new CExoStreamingSoundSource(1);
    m_pAmbientSource = new CExoStreamingSoundSource(1);
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
    m_bBattleMusic = 0;
}
// PC 0x006E7570
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
// PC 0x006E7650
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

#ifdef _XBOX

#include "aurconsole.h"
#include "nwcarea.h"
char* SetMusicVolume(char* szArgs)
{
    if (g_pAppManager->m_pClientExoApp != NULL &&
        g_pAppManager->m_pClientExoApp->GetModule() != NULL &&
        g_pAppManager->m_pClientExoApp->GetModule()->GetArea() != NULL)
    {
        CSWCAmbientSound* pAmbientSound =
            g_pAppManager->m_pClientExoApp->GetModule()->GetArea()->GetAmbientSound();
        if (pAmbientSound != NULL)
        {
            int nVolume = atoi(szArgs);
            if (nVolume >= 0 && nVolume <= CExoSound::CMAX_VOLUME)
            {
                pAmbientSound->SetMusicVolume((unsigned char)nVolume);
                return "Success";
            }
        }
    }
    return "Failed";
}
ConsoleFunc ConFSetMusicVolume("SetMusicVolume", SetMusicVolume);
#endif
