#ifndef __SWGUIINGAMEAREATRANSITION_H__
#define __SWGUIINGAMEAREATRANSITION_H__

#include "swgui.h"
class CSWGuiManager;
class Vector;

class CSWGuiInGameAreaTransition : public CSWGuiPanel
{
public:
    CSWGuiLabel m_lblTextBackground;
    CSWGuiLabel m_lblIcon;
    CSWGuiLabel m_lblDescription;
    Vector m_vTransitionPosition;
    unsigned long m_oidTransitionObject;
    int m_nDefaultTop;

    CSWGuiInGameAreaTransition(CSWGuiManager* pManager);
    void SetTransitionObject(unsigned long oidObject, const Vector& vPosition);
    void Draw(float fElapsed);
};

#endif
