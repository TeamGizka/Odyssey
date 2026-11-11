#ifndef __SWGUIINGAMEOPTIONS_H__
#define __SWGUIINGAMEOPTIONS_H__

#include "swguicontrol.h"
#include "swgui.h"
class CClientExoAppInternal;
class CSWGuiControl;
class CSWGuiManager;

class CSWGuiInGameOptions : public CSWGuiPanel
{
public:
#ifdef _XBOX
    CSWGuiButton m_aOptions[7];
#else
    CSWGuiButton m_aOptions[8];
#endif
    CSWGuiLabel m_lblTitle;
    CSWGuiLabel m_lblDescription;
    CSWGuiLabel m_lblUnknown12C8;
    CSWGuiLabel m_lblUnknown1408;
    CSWGuiLabel m_lblUnknown1548;
    CSWGuiLabel m_lblUnknown1688;
    CSWGuiLabel m_lblScrollIcon;
    CSWGuiListBox m_lbDescription;
#ifndef _XBOX
    CSWGuiButton m_btnUnknown;
    CSWGuiButton m_btnExit;
    CSWGuiLabel m_alblBar[5];
#endif
    virtual void OnPanelRemoved();
    void OnLoadGame(CSWGuiControl*);
    void OnSaveGame(CSWGuiControl*);
    void OnGameplay(CSWGuiControl*);
    void OnFeedback(CSWGuiControl*);
    void OnAutopause(CSWGuiControl*);
#ifndef _XBOX
    void OnGraphics(CSWGuiControl*);
#endif
    void OnSound(CSWGuiControl*);
    void ReallyQuit(CSWGuiControl*);
    virtual void HandleInputEvent(int, int);
    void SetDescription(unsigned long);
    virtual void OnPanelAdded();
    void OnQuit(CSWGuiControl*);
    void OnEnterSlot(CSWGuiControl*);
    CSWGuiInGameOptions(CSWGuiManager*);
};

#endif
