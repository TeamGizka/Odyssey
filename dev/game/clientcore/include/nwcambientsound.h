#ifndef __NWCAMBIENTSOUND_H__
#define __NWCAMBIENTSOUND_H__

#include "exobase.h"
#include "nwambientsound.h"

class CExoStreamingSoundSource;
class CSWCArea;
class CSWCMessage;

class CSWCAmbientSoundTrack
{
public:
    CSWCAmbientSoundTrack();
    ~CSWCAmbientSoundTrack();

    CExoString m_sSound;

    CExoString m_sStinger1;
    CExoString m_sStinger2;
    CExoString m_sStinger3;
};

enum CSWCAmbientSoundPriority
{

    AMBIENT_SOUND_PRIORITY_LOW = 0,
    AMBIENT_SOUND_PRIORITY_MUSIC = 1,
    AMBIENT_SOUND_PRIORITY_AMBIENT = 2,
    AMBIENT_SOUND_PRIORITY_BATTLE = 4
};

class CSWCAmbientSound : public CSWAmbientSound
{
public:
    static const int BATTLE_DELAY;
    static const int MUSIC_FADE_DURATION;
    static const int TRUNCATED_FADE_DURATION;
    static const CExoString THUNDER1;
    static const CExoString THUNDER2;
    static const CExoString THUNDER_ROLLING1;
    static const CExoString THUNDER_ROLLING2;
    CSWCAmbientSound(CSWCArea* pArea);
    ~CSWCAmbientSound();

    virtual void PlayMusic(int bPlay);
    virtual void SetMusicDelay(int nDelay);
    virtual void SetMusicDayTrack(int nTrack, int bStreamingMusicForMovie);
    virtual void SetMusicNightTrack(int nTrack, int bStreamingMusicForMovie);
    virtual void PlayBattleMusic(int bPlay);
    virtual void SetBattleMusicTrack(int nTrack);

    virtual void PlayAmbientSound(int bPlay);
    virtual void SetAmbientDayTrack(int nTrack);
    virtual void SetAmbientNightTrack(int nTrack);

    static const unsigned long BATTLE_FADE_DURATION;

    static const int STREAM_DOESNT_EXIST_DELAY;

    static const int AMBIENT_FADE_DURATION;

    static const unsigned char SOUND_PRIORITY_GROUP_AMBIENT = 4;

    void StartSounds();
    void SetMusicVolume(unsigned char nVolume);
    void SetAmbientDayVolume(int nVolume);
    void SetAmbientNightVolume(int nVolume);

    void Thunder(int bRolling);
    void UpdateTimeOfDay();

    void ReadInfo(int bMusic, int nRow, CSWCAmbientSoundTrack* pTrack);

    void PlayStinger(CSWCAmbientSoundTrack* pTrack);
    void ResetLastUpdate();
    int GetTimeDiff();

    void PlayTruncated(int bPlay, CSWCAmbientSoundPriority nPriority);
    void StopSounds(int bMusicOnly);
    void LoadFromMessage(CSWCMessage* pMessage);

    void Update();

    CSWCAmbientSoundTrack m_MusicDay;
    CSWCAmbientSoundTrack m_MusicNight;
    CSWCAmbientSoundTrack m_BattleMusic;
    CSWCAmbientSoundTrack m_AmbientDay;
    CSWCAmbientSoundTrack m_AmbientNight;
    unsigned long m_MusicDelay;
    unsigned long m_nTimeRemaining;
    unsigned long m_nLastUpdateLow;
    unsigned long m_nLastUpdateHigh;
    int m_aUnknownDC[1];
    CExoStreamingSoundSource* m_pMusicSource;
    CExoStreamingSoundSource* m_pAmbientSource;
    CSWCArea* m_pArea;
    int m_bStarted;
    int m_bAmbientEnabled;
    int m_bSoundsStarted;
    int m_bDaytime;
    int m_bSoundEnabled;

    enum MusicPlayingType
    {
        MUSIC_PLAYING_NONE = 0,
        MUSIC_PLAYING_MUSIC = 1,
        MUSIC_PLAYING_AMBIENT = 2,
        MUSIC_PLAYING_BATTLE = 4
    };
    int m_nMusicPlayingType;
    CExoStreamingSoundSource* m_pStingerSource;
    int m_bAmbientFallback;
};

#endif
