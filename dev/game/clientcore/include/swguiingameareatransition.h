#ifndef __SWGUIINGAMEAREATRANSITION_H__
#define __SWGUIINGAMEAREATRANSITION_H__

#include "swgui.h"
class CSWGuiManager;
class Vector;

class CSWGuiInGameAreaTransition : public CSWGuiPanel
{
public:
    enum { AREA_TRANSITION_WIDTH = 400, AREA_TRANSITION_HEIGHT = 105 };
    CSWGuiLabel m_lblTextBackground;
    CSWGuiLabel m_lblIcon;
    CSWGuiLabel m_lblDescription;
    Vector m_vTransitionPosition;
    unsigned long m_oidTransitionObject;
    int m_nDefaultTop;
    Vector m_v454;
    Vector m_v460;
    Vector m_v46C;

    CSWGuiInGameAreaTransition(CSWGuiManager* pManager);
    void SetTransitionObject(unsigned long oidObject, const Vector& vPosition);
    void Draw(float fElapsed);
};

#endif
