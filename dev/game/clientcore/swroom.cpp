#include "swroom.h"
// PC 0x005C7D10  Xbox 0x000C1170
CSWRoom::CSWRoom()
{
    m_nEnvAudio = 0;
    SetNoWalkMesh(0);
    m_fEnvAudioVolume = 1.0f;
}
// PC 0x005C7D00  Xbox 0x000C11D0
CSWRoom::~CSWRoom()
{
}
// PC 0x005C7D70  Xbox 0x000C11E0
void CSWRoom::SetRoomInfo(const CResRef& cResRef, const Vector& vPosition)
{
    SetNoWalkMesh(cResRef == "****");
    m_cResRef = cResRef;
    m_vPosition = vPosition;
}
// PC 0x005C7DC0  Xbox 0x000C1230
void CSWRoom::SetRoomEnvAudioProps(int nCount, CExoString* pasEnvAudio,
                                  int* panEnvAudio, float* pafEnvAudioVolume)
{
    int i = 0;
    if (GetNoWalkMesh() == 0)
    {
        CExoString sRoomName;
        m_cResRef.CopyToString(sRoomName);
        for (i = 0; i < nCount; ++i)
        {
            if (sRoomName.CompareNoCase(pasEnvAudio[i].CStr()))
            {
                break;
            }
        }
        if (i == nCount)
        {
            return;
        }
        m_nEnvAudio = panEnvAudio[i];
        m_fEnvAudioVolume = pafEnvAudioVolume[i];
    }
}

inline void CSWRoom::SetNoWalkMesh(int bNoWalkMesh)
{
    m_bNoWalkMesh = bNoWalkMesh;
}

inline int CSWRoom::GetNoWalkMesh()
{
    return m_bNoWalkMesh;
}
