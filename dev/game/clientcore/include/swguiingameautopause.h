#ifndef __SWGUIINGAMEAUTOPAUSE_H__
#define __SWGUIINGAMEAUTOPAUSE_H__

class CResGFF;
class CResStruct;
class CSWGuiControl;
class CSWGuiExtent;
class CSWGuiManager;
#include "exobase.h"
#include "swgui.h"
#include "swguicontrol.h"
#include "swguioptions.h"

class CSWGuiInGameAutoPause : public CSWGuiPanel
{
public:
    CSWGuiInGameAutoPause(CSWGuiManager* pManager);

    virtual void HandleInputEvent(int nEvent, int nState);

    void SetupOptions();
    void ClosePanel();
    void SaveOptions();
    void OnDefault(CSWGuiControl* pControl);
    void SetDescription(unsigned long nStrRef);
    void OnEnterSlot(CSWGuiControl* pControl);

    enum
    {
        STRREF_DESC_END_ROUND = 42445,
        STRREF_DESC_ENEMY_SIGHTED = 42446,
        STRREF_DESC_PARTY_KILLED = 42447,
        STRREF_DESC_MINE_SIGHTED = 48704,
        STRREF_DESC_ACTION_MENU = 48705,
        STRREF_DESC_TARGET_CHANGE = 48706
    };

#ifdef _XBOX
    enum
    {
        STRREF_DESC_HORIZ_MENU = 42448,
        STRREF_DESC_VERT_MENU = 42449,
        STRREF_DESC_TRIGGERS = 42480
    };
    CSWGuiOptionsCheckbox m_aCheckboxes[7];
    CSWGuiLabel m_lblDescription; CSWGuiLabel m_lblTitle;
    CSWGuiLabel m_lblA; CSWGuiLabel m_lblB; CSWGuiLabel m_lblSelect; CSWGuiLabel m_lblBack; CSWGuiLabel m_lblRAnalog;
    CSWGuiLabel m_alblBar[4]; CSWGuiListBox m_lbxDetails;
#else
    CSWGuiOptionsCheckbox m_aCheckboxes[6];
    CSWGuiLabel m_lblDescription; CSWGuiLabel m_lblTitle; CSWGuiButton m_btnBack; CSWGuiButton m_btnDefault; CSWGuiLabel m_alblBar[4]; CSWGuiListBox m_lbxDetails;
#endif
};

#endif
