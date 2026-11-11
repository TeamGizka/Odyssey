#include "swcroom.h"
#include "aurscene.h"
// PC 0x00646080  Xbox 0x000B7230
CSWCRoom::CSWCRoom()
{
    m_pAuroraRoom = NULL;
    m_pRoomGob = NULL;
    m_nRoomIndex = -1;
}
// PC 0x006460B0  Xbox 0x000B7260
CSWCRoom::~CSWCRoom()
{
}
// PC 0x006460C0  Xbox 0x000B7270
Gob* CSWCRoom::SpawnRoom(CAurScene* pScene)
{
    if (m_bNoWalkMesh != 0)
    {
        return 0;
    }

    CExoString sRoomName(m_cResRef.GetResRefStr());
    m_pRoomGob = pScene->SpawnRoom((char*)sRoomName.CStr(), m_vPosition,
                                 m_qOrientation);
    if (m_pRoomGob != 0)
    {
        m_pAuroraRoom = pScene->FindRoom((char*)sRoomName.CStr());
    }
    return m_pRoomGob;
}
// PC 0x00646180  Xbox 0x000B7330
void CSWCRoom::PlayAnimation(unsigned short nAnimation)
{
    if (m_pRoomGob != 0)
    {
        CExoString sAnimation;

        if (nAnimation < 10)
        {
            sAnimation.Format("scriptloop0%d", nAnimation);
        }
        else
        {
            sAnimation.Format("scriptloop%d", nAnimation);
        }
        m_pRoomGob->PlayAnimation(sAnimation.CStr(), 1.0f, 8, 0.0f);
    }
}
