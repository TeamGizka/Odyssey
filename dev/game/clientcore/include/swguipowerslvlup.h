#ifndef __SWGUIPOWERSLVLUP_H__
#define __SWGUIPOWERSLVLUP_H__

#include "exobase.h"
#include "swgui.h"
#include "swguiabilitieschargen.h"
#include "swguiskillflowchart.h"

class CSWCCreature;
class CSWGuiLevelUpPanel;

class CSWGuiPowersLevelUp : public CSWGuiPanel
{
public:

    enum PowerState
    {
        POWER_AVAILABLE = 0,
        POWER_CHOSEN = 1,
        POWER_GRANTED = 2,
        POWER_UNAVAILABLE = 3,
        POWER_LOCKED = 4
    };
    enum
    {
        STRREF_POWER_RESTRICTED = 42470,
        STRREF_NO_POWERS_REMAINING = 42529,
        STRREF_POWER_GRANTED = 42185,
        STRREF_POWER_UNAVAILABLE = 42186,
        STRREF_POWER_LOCKED = 111634
    };

    CSWGuiPowersLevelUp(CSWGuiManager* pManager, unsigned long nCreature);
    virtual ~CSWGuiPowersLevelUp();

#ifndef _XBOX
    virtual void Draw(float fElapsed);
#endif
    virtual void HandleInputEvent(int nEvent, int nState);
    virtual void OnPanelAdded();

    void HandleCancelButton();
    void OnAcceptButton();
    void DeterminePower(unsigned long nPower, unsigned char& nState);
    void SetDescription(unsigned long nStrRef);
    void OnEnterPower(unsigned long nPower);
    void BuildButtons();
    void BuildAvailableList();
#ifndef _XBOX
    void OnPowerSelectionChanged(CSWGuiControl* pControl);
#endif
    void InitiatePowers();
    void AddChosenPower(unsigned long nPower);
    void RemoveChosenPower(unsigned long nPower);
    void OnRecommendButton();
    void OnPowerPicked(unsigned long nPower);
#ifndef _XBOX
    void OnDoubleClick(CSWGuiControl* pControl);
#endif
    void ShowGranted(CSWGuiControl* pControl);

    CSWCCreature* m_pCreature;

    CSWGuiLevelUpPanel* m_pParentPanel;

    CSWGuiLabel m_lblTitle;
    CSWGuiLabel m_lblSubTitle;
    CSWGuiLabel m_lblRemainingBox;
    CSWGuiLabel m_lblRemainingSelections;
    CSWGuiLabel m_lblSelectionsRemaining;
    CSWGuiLabel m_lblSelectionsRemainingCount;
    CSWGuiLabel m_lblPowerTitle;
    CSWGuiLabel m_lblDescription;
    CSWGuiLabel m_lblPower;
    CSWGuiListBox m_lbPowers;
    CSWGuiListBox m_lbDescription;
#ifdef _XBOX
    CSWGuiLabel m_lblRecommended;
    CSWGuiLabel m_lblAccept;
    CSWGuiLabel m_lblBack;
    CSWGuiLabel m_lblSelect;
    CSWGuiLabel m_lblA;
    CSWGuiLabel m_lblB;
    CSWGuiLabel m_lblX;
    CSWGuiLabel m_lblY;
    CSWGuiLabel m_lblRanalog;
#endif
#ifndef _XBOX
    CSWGuiButton m_btnRecommended;
    CSWGuiButton m_btnSelect;
    CSWGuiButton m_btnAccept;
    CSWGuiButton m_btnBack;
    CSWGuiLabel m_alblBar[2];
#endif
    unsigned char m_nPowersRemaining;
    unsigned char m_nSpellLevel;
    unsigned long m_bIsPC : 1;
    int m_nSelectedPower;
    unsigned long m_bShowGrantedTutorial : 1;
    CExoString m_sSelectText;
    CExoString m_sDeselectText;
    CExoArrayList<unsigned long> m_lGranted;
    CExoArrayList<unsigned long> m_lAvailable;
    CExoArrayList<unsigned long> m_lChosen;
    CExoArrayList<unsigned long> m_lLocked;
    CSWGuiSkillFlowChart m_cChart;
    unsigned char m_nPowerState;
};

#endif
