#ifndef __SWACTIONNODEINL_H__
#define __SWACTIONNODEINL_H__

#include "nwscreature.h"

inline void CSWActionNode::Clear()
{
    m_nActionId = 0xFFFF;
    m_nGroupActionId = 0xFFFF;
    m_nActionType = 0xFFFF;
    m_vLocation = Vector();
    m_oidTarget = OBJECT_INVALID;
    m_nParameter = -1;
}

#endif
