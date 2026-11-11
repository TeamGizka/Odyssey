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
        POWER_UNAVAILABLE = 3
    };
    enum
    {
        STRREF_POWER_RESTRICTED = 42470,
        STRREF_NO_POWERS_REMAINING = 42529,
        STRREF_POWER_GRANTED = 42185,
        STRREF_POWER_UNAVAILABLE = 42186
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

    CSWCCreature* m_pCreature;

    CSWGuiLevelUpPanel* m_pParentPanel;

    CSWGuiLabel m_lblTitle;
    CSWGuiLabel m_lblSubTitle;
    CSWGuiLabel m_lblRemainingBox;
    CSWGuiLabel m_lblRemainingSelections;
    CSWGuiLabel m_lblSelectionsRemaining;
#ifdef _XBOX
    CSWGuiLabel m_lblRecommended;
    CSWGuiLabel m_lblAccept;
#endif
    CSWGuiLabel m_lblUnmapped7EC;
    CSWGuiLabel m_lblUnmapped92C;
    CSWGuiLabel m_lblDescription;
#ifdef _XBOX
    CSWGuiLabel m_lblBack;
    CSWGuiLabel m_lblSelect;
    CSWGuiLabel m_lblA;
    CSWGuiLabel m_lblB;
    CSWGuiLabel m_lblX;
    CSWGuiLabel m_lblY;
    CSWGuiLabel m_lblRanalog;
#endif
    CSWGuiLabel m_lblPower;
    CSWGuiListBox m_lbPowers;
    CSWGuiListBox m_lbDescription;
#ifndef _XBOX
    CSWGuiButton m_btnRecommended;
    CSWGuiButton m_btnSelect;
    CSWGuiButton m_btnAccept;
    CSWGuiButton m_btnBack;
    CSWGuiLabel m_alblBar[2];
#endif
    unsigned char m_nPowersRemaining;
    unsigned char m_nSpellLevel;
    unsigned char m_aUnmapped19BE[2];
    unsigned long m_bIsPC : 1;
    int m_nSelectedPower;
    CExoString m_sSelectText;
    CExoString m_sDeselectText;
    CExoArrayList<unsigned long> m_lGranted;
    CExoArrayList<unsigned long> m_lAvailable;
    CExoArrayList<unsigned long> m_lChosen;
    CSWGuiSkillFlowChart m_cChart;
#ifndef _XBOX
    unsigned char m_nPowerState;
#endif
};

#endif
