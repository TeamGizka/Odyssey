#ifndef __SWGUIINGAMEPAUSE_H__
#define __SWGUIINGAMEPAUSE_H__

#include "swgui.h"
class CSWGuiControl;
class CSWGuiManager;

class CSWGuiInGamePause : public CSWGuiPanel
{
public:
    enum
    {
        STRREF_PAUSED = 1508,
        STRREF_ENEMY_SIGHTED = 48732,
        STRREF_ACTION_QUEUED = 48734,
        STRREF_MINE_SIGHTED = 48735,
        STRREF_PRESS_TO_CONTINUE = 136311,
        STRREF_PRESS_TO_FORFEIT = 136309,
        STRREF_PRESS_TO_UNPAUSE_MINIGAME = 136318
    };
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
