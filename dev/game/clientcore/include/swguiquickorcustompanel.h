#ifndef __SWGUIQUICKORCUSTOMPANEL_H__
#define __SWGUIQUICKORCUSTOMPANEL_H__

class CSWGuiControl;
class CSWGuiManager;
#include "exobase.h"
#include "swgui.h"
#include "swguicontrol.h"

class CSWCCreature;
class CSWGuiMainCharGen;

class CSWGuiQuickOrCustomPanel : public CSWGuiPanel
{
public:
    CSWGuiQuickOrCustomPanel(CSWGuiManager* pManager, CSWCCreature* pCreature);

    CSWCCreature* m_pCreature;
    CSWGuiMainCharGen* m_pMainCharGen;
    CSWGuiButton m_cQuickButton;
    CSWGuiButton m_cCustomButton;
    CSWGuiLabel m_cDescription;
    CSWGuiLabel m_cRightAnalog;
    CSWGuiListBox m_cDescriptionList;
#ifndef _XBOX
    CSWGuiButton m_cBackButton;
#endif
    virtual void HandleInputEvent(int nEvent, int nState);
    void HandleCancelButton();
    void SetDescription(unsigned long nStrRef);
    void OnSelectQuickCharacterButton(CSWGuiControl* pControl);
    void OnSelectCustomCharacterButton(CSWGuiControl* pControl);
    void OnEnterQuickCharacterButton(  // PC 0x00761C50 GOG 0x007619A0
        CSWGuiControl* pControl)
    {
        SetDescription(0xF1);
    }
    void OnEnterCustomCharacterButton(  // PC 0x00761C60 GOG 0x007619B0
        CSWGuiControl* pControl)
    {
        SetDescription(0xF2);
    }
};

#endif
