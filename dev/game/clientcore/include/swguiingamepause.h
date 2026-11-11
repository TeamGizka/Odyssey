#ifndef __SWGUIINGAMEPAUSE_H__
#define __SWGUIINGAMEPAUSE_H__

#include "swgui.h"
class CSWGuiControl;
class CSWGuiManager;

class CSWGuiInGamePause : public CSWGuiPanel
{
public:
    CSWGuiLabel m_lblPauseReason;
    CSWGuiLabel m_lblPress;
#ifndef _XBOX
    CSWGuiButton m_btnUnpause;
#endif
    int m_nOriginalTop;
    int m_nOriginalLeft;

    virtual void Draw(float fElapsed);
    void SetPauseReason(unsigned char nReason);
    virtual void OnPanelAdded();
    void OnUnpauseButtonPressed(CSWGuiControl* pControl);
    CSWGuiInGamePause(CSWGuiManager*);

};

#endif
