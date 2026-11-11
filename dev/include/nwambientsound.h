#ifndef __NWAMBIENTSOUND_H__
#define __NWAMBIENTSOUND_H__

class CSWAmbientSound
{
public:
    CSWAmbientSound()
    {
        m_bMusicEnabled = 1;
        m_nMusicDelay = 5000;
        m_nMusicDayTrack = 2;
        m_nMusicNightTrack = 3;
        m_bBattleMusic = 0;
        m_nBattleMusicTrack = 1;
        m_bAmbientPlaying = 1;
        m_nAmbientDayTrack = 1;
        m_nAmbientNightTrack = 2;
        m_nAmbientDayVolume = 0;
        m_nAmbientNightVolume = 0;
    }
    // PC 0x00504860
    ~CSWAmbientSound() {}

    virtual void PlayMusic(int bPlay) = 0;
    virtual void SetMusicDelay(int nDelay) = 0;
    virtual void SetMusicDayTrack(int nTrack) = 0;
    virtual void SetMusicNightTrack(int nTrack) = 0;
    virtual void PlayBattleMusic(int bPlay) = 0;
    virtual void SetBattleMusicTrack(int nTrack) = 0;
    virtual void PlayAmbientSound(int bPlay) = 0;
    virtual void SetAmbientDayTrack(int nTrack) = 0;
    virtual void SetAmbientNightTrack(int nTrack) = 0;

    int m_bMusicEnabled;
    int m_nMusicDelay;
    int m_nMusicDayTrack;
    int m_nMusicNightTrack;
    int m_bBattleMusic;
    int m_nBattleMusicTrack;
    int m_bAmbientPlaying;
    int m_nAmbientDayTrack;
    int m_nAmbientNightTrack;
    unsigned char m_nAmbientDayVolume;
    unsigned char m_nAmbientNightVolume;

};

#endif
