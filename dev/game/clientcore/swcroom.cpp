#include "swcroom.h"
#include "aurscene.h"
// PC 0x0069FB80 GOG 0x0069F960
CSWCRoom::CSWCRoom()
{
    m_pAuroraRoom = NULL;
    m_pRoomGob = NULL;
    m_nRoomIndex = -1;
}
// PC 0x0069FBB0 GOG 0x0069F990
CSWCRoom::~CSWCRoom()
{
}
// PC 0x0069FBC0 GOG 0x0069F9A0
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
// PC 0x0069FC80 GOG 0x0069FA60
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
        m_pRoomGob->PlayAnimation("null", 1.0f, 8, 0.0f);
        m_pRoomGob->PlayAnimation(sAnimation.CStr(), 1.0f, 8, 0.0f);
    }
}
