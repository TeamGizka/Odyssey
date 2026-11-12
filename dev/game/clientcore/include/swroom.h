#ifndef __SWROOM_H__
#define __SWROOM_H__

class CResRef;
class Vector;
#include "exoresman.h"
#include "nwcanimbase.h"

class CSWRoom
{
public:
    CSWRoom();
    virtual ~CSWRoom();

    void SetRoomInfo(const CResRef& cResRef, const Vector& vPosition);
    void SetRoomEnvAudioProps(int nCount, CExoString* pasEnvAudio,
                             int* panEnvAudio, float* pafEnvAudioVolume);
    void SetNoWalkMesh(int bNoWalkMesh);
    int GetNoWalkMesh();
    CResRef GetResRef() { return m_cResRef; }

    Vector m_vPosition;
    Quaternion m_qOrientation;
    CResRef m_cResRef;
    int m_nEnvAudio;
    float m_fEnvAudioVolume;
    int m_bNoWalkMesh;
};

#endif
