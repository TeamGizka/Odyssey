#ifndef __SWGUISKILLSCHARGEN_H__
#define __SWGUISKILLSCHARGEN_H__

#include "exobase.h"
#include "swguiabilitieschargen.h"

class CSWGuiSkillsCharGen : public CSWGuiPanel
{
public:

    enum
    {
        STRREF_LEVEL_UP = 1071,
        STRREF_NOT_ENOUGH_SKILL_POINTS = 42464,
        STRREF_PERSUADE_MAIN_CHARACTER_ONLY = 42469,
        STRREF_DROID_SKILL_UNAVAILABLE = 42471,
        STRREF_CLASS_SKILL_MAX_RANK = 42178,
        STRREF_CROSS_CLASS_SKILL_MAX_RANK = 42179
    };
    CSWGuiSkillsCharGen(CSWGuiManager* pManager, CSWCCreature* pCreature,
                        int bLevelUp);
    virtual ~CSWGuiSkillsCharGen();

    virtual void HandleInputEvent(int nEvent, int nState);
    virtual void OnPanelAdded();

    void HandleCancelButton();
    void OnExitPointsButton(CSWGuiControl* pControl);
    void SetDescription(const CExoString& sDescription);
    int IsClassSkill(unsigned short nSkill);
    void OnEnterPointsButton(CSWGuiControl* pControl);
    void ConfirmAccept(CSWGuiControl* pControl);
    void OnRecommendButton();
    void OnMinusButtonPushed(CSWGuiControl*)  // PC 0x006D2FC0
    {
        HandleInputEvent(GUIEVENT_LEFT, 1);
    }
    void OnPlusButtonPushed(CSWGuiControl*)  // PC 0x006D2FD0
    {
        HandleInputEvent(GUIEVENT_RIGHT, 1);
    }
    void OnMinusButton();
    void OnPlusButton();
#ifdef _XBOX
    virtual void Draw(float fElapsed);
    void HilightArrow(CSWGuiLabel* pLabel);
#endif
    void CompletePanel();
    void OnAcceptButton();
    CSWCCreature* GetCreature() { return m_pCreature; }

    CSWCCreature* m_pCreature;

    union
    {
        CSWGuiPanel* m_pParentPanel;
        CSWGuiLevelUpPanel* m_pParent;
    };

    CSWGuiListBox m_lbDescription;
    CSWGuiLabel m_lblTitle;
    CSWGuiLabel m_lblSubTitle;
    CSWGuiLabel m_lblRemainingBox;
    CSWGuiLabel m_lblRemainingSelections;
    CSWGuiLabel m_lblSelectionsRemaining;
    CSWGuiLabel m_lblDescriptionHeader;
    CSWGuiLabel m_lblCost;
    CSWGuiLabel m_lblCostPoints;
    CSWGuiLabel m_lblClassSkill;
    CSWGuiLabel m_lblDescription;
#ifdef _XBOX
    CSWGuiLabel m_lblA;
    CSWGuiLabel m_lblB;
    CSWGuiLabel m_lblY;
    CSWGuiLabel m_lblAccept;
    CSWGuiLabel m_lblBack;
    CSWGuiLabel m_lblRecommended;
    CSWGuiLabel m_lblRanalog;
#endif
    CSWGuiLabel m_lblSkill[8];
#ifdef _XBOX
    CSWGuiLabel m_lblMinus[8];
    CSWGuiLabel m_lblPlus[8];
#endif
    CSWGuiButton m_btnPoints[8];
#ifndef _XBOX
    CSWGuiButton m_btnAccept;
    CSWGuiButton m_btnBack;
    CSWGuiButton m_btnRecommended;
    CSWGuiButton m_btnPlus[8];
    CSWGuiButton m_btnMinus[8];
#endif
    CExoString m_sDescription[8];
#ifdef _XBOX
    CExoArrayList<CSWGuiAbilitiesArrowHilight> m_lArrowHilights;
#endif
    int m_nPointsRemaining;
    int m_nSelected;
    unsigned char m_aStartRank[8];
    unsigned long m_bLevelUp : 1;
    unsigned long m_bAllSkills : 1;
    unsigned long m_bDroid : 1;
    int m_nStartPoints;
};

#endif
