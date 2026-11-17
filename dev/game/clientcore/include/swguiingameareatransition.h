#ifndef __SWGUIINGAMEAREATRANSITION_H__
#define __SWGUIINGAMEAREATRANSITION_H__

#include "swgui.h"
class CSWGuiManager;
class Vector;

class CSWGuiInGameAreaTransition : public CSWGuiPanel
{
public:
    enum { AREA_TRANSITION_WIDTH = 400, AREA_TRANSITION_HEIGHT = 82 };
    CSWGuiLabel m_lblTextBackground;
    CSWGuiLabel m_lblIcon;
    CSWGuiLabel m_lblDescription;
    Vector m_vTransitionPosition;
    unsigned long m_oidTransitionObject;
    int m_nDefaultTop;
    Vector m_vTextBackgroundBorderColor;
    Vector m_vIconBorderColor;
    Vector m_vDescriptionTextColor;

    CSWGuiInGameAreaTransition(CSWGuiManager* pManager);
    void SetTransitionObject(unsigned long oidObject, const Vector& vPosition);
    void Draw(float fElapsed);
};

#endif
