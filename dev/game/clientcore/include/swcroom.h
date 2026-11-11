#ifndef __SWCROOM_H__
#define __SWCROOM_H__

class CAurScene;
class CAurRoom;
class Gob;
#include "swroom.h"

class CSWCRoom : public CSWRoom
{
public:
    CSWCRoom();
    ~CSWCRoom();
    Gob* SpawnRoom(CAurScene* pScene);
    void PlayAnimation(unsigned short nAnimation);

    CAurRoom* m_pAuroraRoom;
    Gob* m_pRoomGob;
    int m_nRoomIndex;
};

#endif
