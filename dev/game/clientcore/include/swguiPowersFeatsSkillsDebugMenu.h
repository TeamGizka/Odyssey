#ifndef __SWGUIPOWERSFEATSSKILLSDEBUGMENU_H__
#define __SWGUIPOWERSFEATSSKILLSDEBUGMENU_H__

#include "exobase.h"
#include "swgui.h"
#include "swguicontrol.h"

class CSWSCreature;

class CSWGuiPowersFeatsSkillsDebugMenu : public CSWGuiPanel
{
public:

    enum MenuType
    {
        MENU_MAIN = 0,
        MENU_POWERS = 1,
        MENU_SKILLS = 2,
        MENU_FEATS = 3
    };

    CSWGuiPowersFeatsSkillsDebugMenu(CSWGuiManager* pManager);

    virtual void HandleInputEvent(int nEvent, int nState);

    void AddPrerequisiteSpells(unsigned long nSpell, CSWSCreature* pCreature);
    void AddPrerequisiteFeats(unsigned short nFeat, CSWSCreature* pCreature);
    void HideMenu();
    void OnSkillPicked(CSWGuiControl* pControl);
    void CreateSkillsSubMenu();
    void OnPowerPicked(CSWGuiControl* pControl);
    void OnFeatPicked(CSWGuiControl* pControl);
    void OnAddSkillPicked(CSWGuiControl* pControl);
    void CreatePowersSubMenu();
    void CreateFeatsSubMenu();
    void OnAddPowerPicked(CSWGuiControl* pControl);
    void OnAddFeatPicked(CSWGuiControl* pControl);
    void CreateMainPowersFeatsSkillsMenu();
    void Back();

    CSWGuiControl* CreateOption(const CExoString& sText, PFNGUIEVENT pHandler,
                                CSWGuiButton* pProto);

    CSWGuiListBox m_lbOptions;
    CSWGuiLabel m_lblBuild;
    int m_nSelected;
    int m_nNumOptions;
    int m_nMenu;
};

#endif
