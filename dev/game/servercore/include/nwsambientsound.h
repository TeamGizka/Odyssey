#ifndef __NWSAMBIENTSOUND_H__
#define __NWSAMBIENTSOUND_H__

#include "nwambientsound.h"

template <class T> class CExoArrayList;
class CResGFF;
class CResStruct;
class CSWSMessage;

class CSWSAmbientSound : public CSWAmbientSound
{
public:
    static const int BATTLE_DELAY;

    virtual void PlayMusic(int bPlay);
    virtual void SetMusicDelay(int nDelay);
    virtual void SetMusicDayTrack(int nTrack, int bStreamingMusicForMovie);
    virtual void SetMusicNightTrack(int nTrack, int bStreamingMusicForMovie);
    virtual void PlayBattleMusic(int bPlay);
    virtual void SetBattleMusicTrack(int nTrack);
    virtual void PlayAmbientSound(int bPlay);
    virtual void SetAmbientDayTrack(int nTrack);
    virtual void SetAmbientNightTrack(int nTrack);

    CSWSAmbientSound(unsigned long oidArea);

    int Load(CResGFF* pRes, CResStruct* pStruct);
    void CacheSound(int nTrack, int bMusic);
    int Save(CResGFF* pRes, CResStruct* pStruct);
    void PackIntoMessage(CSWSMessage* pMessage);
    void SetAmbientDayVolume(int nVolume);
    void SetAmbientNightVolume(int nVolume);
    int GetPlayersInArea(CExoArrayList<unsigned long>* pPlayerList);

    unsigned long m_oidArea;
};

#endif
