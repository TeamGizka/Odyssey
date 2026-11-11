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
    CSWGuiLabel m_cDecoration;
    CSWGuiLabel m_cRightAnalog;
    CSWGuiLabel m_cBackground;
    CSWGuiListBox m_cDescriptionList;
#ifndef _XBOX
    CSWGuiButton m_cBackButton;
#endif
    virtual void HandleInputEvent(int nEvent, int nState);
    void HandleCancelButton();
    void SetDescription(unsigned long nStrRef);
    void OnSelectQuickCharacterButton(CSWGuiControl* pControl);
    void OnSelectCustomCharacterButton(CSWGuiControl* pControl);
    void OnEnterQuickCharacterButton(  // PC 0x00761C50
        CSWGuiControl* pControl)
    {
        SetDescription(0xF1);
    }
    void OnEnterCustomCharacterButton(  // PC 0x00761C60
        CSWGuiControl* pControl)
    {
        SetDescription(0xF2);
    }
};

#endif
