#include "nwcmappin.h"
#include "nwsobject.h"
#include "nwcanimbase.h"
// PC 0x006EBFA0
CSWCMapPin::CSWCMapPin()
    : CSWCObject(CGameObject::OBJECT_TYPE_WAYPOINT)
{
    m_nCurrentAnimation = CREATURE_ANIMATION_PAUSE_READY;
    m_bEnabled = 0;
    m_nType = 0;
}
// PC 0x006EC010
CSWCMapPin::~CSWCMapPin()
{
}
