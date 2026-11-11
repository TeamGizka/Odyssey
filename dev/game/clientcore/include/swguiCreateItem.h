#ifndef __SWGUICREATEITEM_H__
#define __SWGUICREATEITEM_H__

#include "swgui.h"
class CSWGuiControl;
class CSWGuiManager;

class CSWGuiCreateItemMenu : public CSWGuiPanel
{
public:
    CSWGuiCreateItemMenu(CSWGuiManager* pManager);
    virtual void HandleInputEvent(int nEvent, int nState);
    CSWGuiButton* CreateOption(const CExoString& sText,
                               CSWGuiHandler pHandler,
                               CSWGuiButton* pPrototype);
    void Back(CSWGuiControl* pControl);
    void ItemSubTypeMenu(CSWGuiControl* pControl);

    CSWGuiListBox m_lbOptions;
    CSWGuiLabel m_lblBuild;
    int m_nSelectedSubType;
};

#endif
