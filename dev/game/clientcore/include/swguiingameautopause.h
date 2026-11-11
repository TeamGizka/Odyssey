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

    CSWGuiOptionsCheckbox m_aCheckboxes[6];
    CSWGuiLabel m_lblDescription;
    CSWGuiLabel m_lblTitle;
    CSWGuiButton m_btnBack;
    CSWGuiButton m_btnDefault;
    CSWGuiListBox m_lbxDetails;
};

#endif
