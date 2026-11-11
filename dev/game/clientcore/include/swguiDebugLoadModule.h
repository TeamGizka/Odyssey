#ifndef __SWGUIDEBUGLOADMODULE_H__
#define __SWGUIDEBUGLOADMODULE_H__

#include "swguicontrol.h"
#include "swgui.h"
class CSWGuiControl;
class CSWGuiManager;

class CSWGuiLoadModuleDebugMenu : public CSWGuiPanel
{
public:
    CSWGuiListBox m_CSWGuiListBox_64;
    CSWGuiLabel m_lblBuild;
    int m_nSelection;
    int m_nNumOptions;
    CSWGuiControl* CreateOption(const CExoString& sText, PFNGUIEVENT pHandler,
                               CSWGuiButton* pProto);
    void Back();
    void OnModulePicked(CSWGuiControl* pControl);
    CSWGuiLoadModuleDebugMenu(CSWGuiManager*);
    virtual void HandleInputEvent(int, int);
};

#endif
