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
#ifndef _XBOX
    enum
    {
        STRREF_DESC_LOAD_GAME = 42300,
        STRREF_DESC_SAVE_GAME = 42301,
        STRREF_DESC_GAMEPLAY = 42273,
        STRREF_DESC_FEEDBACK = 136314,
        STRREF_DESC_AUTOPAUSE = 42275,
        STRREF_DESC_GRAPHICS = 48687,
        STRREF_DESC_SOUND = 48688,
        STRREF_DESC_QUIT = 42302
    };
#endif
    CSWGuiInGameOptions(CSWGuiManager*);
};

#endif
