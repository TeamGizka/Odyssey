#ifndef __SWGUILEVELUPPANEL_H__
#define __SWGUILEVELUPPANEL_H__

#include "swguicontrol.h"
class CSWGuiControl;
class CSWGuiManager;
#include "swguiingamecharacter.h"
#include "swguifeatschargen.h"
#include "exobase.h"
#include "swgui.h"
#include "swguiskillflowchart.h"
#include "nwccreaturestats.h"

class CSWGuiLevelUpPanel;

class CSWGuiLevelUpPanel : public CSWGuiPanel
{
public:

    enum LevelUpState
    {
        STATE_ABILITIES,
        STATE_SKILLS,
        STATE_FEATS,
        STATE_POWERS,
        STATE_ACCEPT,
        STATE_COUNT
    };

    void CancelledByChild();
    void NewAbilityScores(int nStr, int nDex, int nCon, int nWis, int nInt,
                          int nCha, int nRemaining);
    void AbilitySelectionCompleted();

    CSWCCreature* m_pCreature;
    CSWGuiLevelUpCharGen* m_pParent;
    CSWGuiLabel m_lblBackground;
    CSWGuiLabelHilight m_aHilights[STATE_COUNT];
    CSWGuiLabel m_aNumbers[STATE_COUNT];
    CSWGuiButton m_aSteps[STATE_COUNT];
#ifndef _XBOX
    CSWGuiButton m_btnBack;
    CSWGuiButton m_btnCancel;
#endif
    int m_nState;
    int m_nFirstState;
    int m_anStateUsed[STATE_COUNT];
    int m_bCanCancel;
    CSWGuiLevelUpPanel(CSWGuiManager* pManager, unsigned long nCreature);
    virtual void HandleInputEvent(int nEvent, int nState);
    virtual void OnPanelAdded();
    void ChangeState(int bBack);
    void OnSelectPowersButton(CSWGuiControl* pControl);
    void OnSelectFeatsButton(CSWGuiControl* pControl);
    void OnSelectSkillsButton(CSWGuiControl* pControl);
    void OnSelectAbilitiesButton(CSWGuiControl* pControl);
    void FeatSelectionCompleted();
    void SkillSelectionCompleted();
    void PowersSelectionCompleted();
#ifndef _XBOX
    void OnCancelPressed(CSWGuiControl* pControl);
    void SetCanCancel(int bCanCancel);
#else
    void SetCanCancel(int bCanCancel) { m_bCanCancel = bCanCancel; }  // PC 0x006EE640
#endif
    void PlayGame();
    void OnSelectAcceptButton(CSWGuiControl* pControl);
};

#endif
