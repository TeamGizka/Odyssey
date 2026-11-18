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
#ifdef _XBOX
    CSWGuiLabel m_lblLegalText;
    CSWGuiLabel m_lblGameLogo;
#endif
    float m_fIdleTime;
    unsigned long m_bResetIdleTime : 1;
};

#endif
