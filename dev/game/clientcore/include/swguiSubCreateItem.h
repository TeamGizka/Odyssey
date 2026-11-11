#ifndef __SWGUISUBCREATEITEM_H__
#define __SWGUISUBCREATEITEM_H__

#include "exobase.h"
#include "swgui.h"
class CSWGuiControl;
class CSWGuiManager;

class CSWGuiCreateItemSubMenu : public CSWGuiPanel
{
public:
    CSWGuiListBox m_lstOptions;
    CSWGuiLabel m_lblBuild;
    int m_nSelection;
    CExoArrayList<CSWGuiControl*> m_lOptions;
    CExoStringList m_lstOptionNames;
    CExoStringList m_lstBlueprintNames;

    virtual void HandleInputEvent(int nEvent, int nState);
    CSWGuiButton* CreateOption(const CExoString& sText, CSWGuiHandler pHandler,
                               CSWGuiButton* pPrototype);
    void Back(CSWGuiControl* pControl);
    void CreateItem(CSWGuiControl* pControl);
    CSWGuiCreateItemSubMenu(CSWGuiManager*);
    void LoadItems(int);
    void ClearItems();
};

#endif
