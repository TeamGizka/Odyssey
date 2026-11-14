#ifndef __NWSDIALOGINL_H__
#define __NWSDIALOGINL_H__

#include "nwsdialog.h"

// PC 0x004F60A0 GOG 0x00505890
inline unsigned long CSWSDialog::GetCurrentPlayerId()
{
    if (m_lstPlayers.GetHeadPos() != NULL)
    {
        return m_oidCurrentPlayer;
    }
    return OBJECT_INVALID;
}

#endif
