#ifndef __SWGUISTARTSCREEN_H__
#define __SWGUISTARTSCREEN_H__

#include "swgui.h"

class CSWGuiStartScreen : public CSWGuiPanel
{
public:
    CSWGuiStartScreen(CSWGuiManager* pManager);
    virtual void Draw(float fElapsed);
    virtual void HandleInputEvent(int nEvent, int nState);

    CSWGuiLabel m_lblStartText;
    float m_fIdleTime;
    unsigned long m_bResetIdleTime : 1;
};

#endif
