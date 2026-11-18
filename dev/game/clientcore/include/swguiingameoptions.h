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
    CSWGuiLabel m_lblA;
    CSWGuiLabel m_lblSelect;
    CSWGuiLabel m_lblStart;
    CSWGuiLabel m_lblExit;
    CSWGuiLabel m_lblScrollIcon;
    CSWGuiListBox m_lbDescription;
#ifndef _XBOX
    CSWGuiButton m_btnResume;
    CSWGuiButton m_btnExit;
    CSWGuiLabel m_alblBar[5];
#else
    CSWGuiLabel m_alblBar[6];
    Vector m_vBorderColor;
    Vector m_vHiliteColor;
#endif
#ifndef _XBOX
    virtual void OnPanelRemoved();
#endif
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
#ifndef _XBOX
    virtual void OnPanelAdded();
#endif
    void OnQuit(CSWGuiControl*);
    void OnEnterSlot(CSWGuiControl*);
#ifndef _XBOX
    void SetActiveButton(int);
#endif
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
