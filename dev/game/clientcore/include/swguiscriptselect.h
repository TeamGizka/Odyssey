#ifndef __SWGUISCRIPTSELECT_H__
#define __SWGUISCRIPTSELECT_H__

#include "exobase.h"
#include "swgui.h"
#include "swguicontrol.h"

class CSWGuiButtonToggle;
class CSWGuiControl;
class CSWGuiManager;
class CSWGuiObject;

class CSWGuiScriptSelect : public CSWGuiPanel
{
public:

    struct SCRIPT_INFO
    {
        unsigned long m_nDescriptionStrRef;
        int m_nAIState;
    };

    CSWGuiScriptSelect(CSWGuiManager* pManager);
    virtual ~CSWGuiScriptSelect();

    void SetCreatureID(unsigned long oidCreature);
    CExoArrayList<SCRIPT_INFO> m_aScripts;
    CSWGuiListBox m_lbxScripts;
    CSWGuiListBox m_lbxDescription;
    CSWGuiLabel m_lblDescription;
#ifdef _XBOX

    CSWGuiLabel m_lblAImg;
    CSWGuiLabel m_lblAccept;
    CSWGuiLabel m_lblBImg;
    CSWGuiLabel m_lblBack;
    CSWGuiLabel m_lblTitle;
    CSWGuiLabel m_lblRAnalog;
#else
    CSWGuiLabel m_lblTitle;
    CSWGuiButton m_btnBack;
    CSWGuiButton m_btnAccept;
#endif
    int m_nUnknownC84;
    unsigned long m_oidCreature;
    virtual void HandleInputEvent(int nEvent, int nState);
    virtual void OnPanelAdded();
    void SetDescription(unsigned long nStrRef);
    void OnScriptSelected(CSWGuiControl* pControl);
    CSWGuiControl* CreateOption(unsigned long nStrRef, PFNGUIEVENT pHandler,
                                CSWGuiButtonToggle* pProto);
    void OnEnter(CSWGuiControl* pControl);
};

#endif
