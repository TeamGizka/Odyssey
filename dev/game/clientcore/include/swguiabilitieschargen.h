#ifndef __SWGUIABILITIESCHARGEN_H__
#define __SWGUIABILITIESCHARGEN_H__

class CCAppearanceInfo;
class CCombatInformation;
class CSWSObjectAction;
class CSWGuiLevelUpPanel;
class CExoLinkedListInternal;
class CExoSoundSource;
class CExoStreamingSoundSource;
class CGameEffect;
class CGameObject;
class CItemRepository;
class CPathfindInformation;
class CResRef;
class CSWAnimBase;
class CSWCAnimBase;
class CSWCArea;
class CSWCCMessageData;
class CSWCCombatAttackData;
class CSWCCreatureStats;
class CSWCItem;
class CSWCObject;
class CSWCSpellCastVisual;
class CSWCreatureStatsUpdate;
class CSWGui3DSceneView;
class CSWLevelStats;
class CSWPartyFollowDataArray;
class CSWPartyMemberData;
class CSWSArea;
class CSWSCombatRound;
class CSWSCreaturePartyFollowInfo;
class CSWSFaction;
class CSWSForcedAction;
class CSWSJournal;
class CSWSObjectActionNode;
class CSWSpell;
struct STR_RES;
class Vector;
#include "swguileveluppanel.h"
#include "swgui.h"
#include "swguicontrol.h"
#include "exotlktable.h"
#include "swguidrawable.h"
#include "exobase.h"
#include "exoresman.h"
#include "math3d.h"

class CSWGuiManager;
class CSWGuiMessageBox;
class CResGFF;
class CResStruct;

extern CTlkTable* g_pTlkTable;

class CSWGuiControl;
class CSWGuiPanel;

class CSWGuiControl;

#ifdef _XBOX
struct CSWGuiAbilitiesArrowHilight
{
    CSWGuiLabel* m_pLabel;
    float m_fTimeLeft;
};
#endif

class CSWGuiAbilitiesCharGen : public CSWGuiPanel
{
public:
    CSWGuiAbilitiesCharGen(CSWGuiManager* pManager, CSWCCreature* pCreature,
                           int bLevelUp);
    virtual ~CSWGuiAbilitiesCharGen();

    virtual void OnPanelAdded();
    virtual void HandleInputEvent(int nEvent, int nState);
    // PC 0x006D2FC0
    void OnMinusButtonPushed(CSWGuiControl* pControl)
    {
        HandleInputEvent(GUIEVENT_LEFT, 1);
    }
    // PC 0x006D2FD0
    void OnPlusButtonPushed(CSWGuiControl* pControl)
    {
        HandleInputEvent(GUIEVENT_RIGHT, 1);
    }

    int DetermineCostToDecreaseScore(int nScore);
    int DetermineCostToIncreaseScore(int nScore);
    void HandleCancelButton();
    void OnExitPointsButton(CSWGuiControl* pControl);
    void CompletePanel();
    void SetDescription(unsigned long nStrRef);
    void OnEnterPointsButton(CSWGuiControl* pControl);
    void OnAcceptButton();
    void OnRecommendButton();
    void OnMinusButton();
    void OnPlusButton();
#ifdef _XBOX
    virtual void Draw(float fElapsed);
    void HilightArrow(CSWGuiLabel* pLabel);
#endif

    CSWCCreature* m_pCreature;

    CSWGuiPanel* m_pParentPanel;

    CSWGuiListBox m_lbDescription;
    CSWGuiLabel m_lblTitle;
    CSWGuiLabel m_lblSubTitle;
    CSWGuiLabel m_lblRemainingBox;
    CSWGuiLabel m_lblRemainingSelections;
    CSWGuiLabel m_lblSelectionsRemaining;
    CSWGuiLabel m_lblDescriptionHeader;
    CSWGuiLabel m_lblCost;
    CSWGuiLabel m_lblCostPoints;
    CSWGuiLabel m_lblDescription;
#ifdef _XBOX
    CSWGuiLabel m_lblA;
    CSWGuiLabel m_lblB;
    CSWGuiLabel m_lblY;
    CSWGuiLabel m_lblAccept;
    CSWGuiLabel m_lblBack;
    CSWGuiLabel m_lblRecommended;
#endif
    CSWGuiLabel m_lblAbilityMod;
    CSWGuiLabel m_lblModifier;
#ifdef _XBOX
    CSWGuiLabel m_lblRanalog;
#endif
    CSWGuiLabel m_lblScore[6];
#ifdef _XBOX
    CSWGuiLabel m_lblMinus[6];
    CSWGuiLabel m_lblPlus[6];
#endif
    CSWGuiButton m_btnPoints[6];
#ifndef _XBOX
    CSWGuiButton m_btnAccept;
    CSWGuiButton m_btnBack;
    CSWGuiButton m_btnRecommended;
    CSWGuiButton m_btnPlus[6];
    CSWGuiButton m_btnMinus[6];
#endif
    unsigned long m_aDescriptionStrRef[6];
#ifdef _XBOX

    CExoArrayList<CSWGuiAbilitiesArrowHilight> m_lArrowHilights;
#endif
    int m_nPointsRemaining;
    int m_aScore[6];
    int m_aMinimumScore[6];
    int m_nSelected;
    int m_bLevelUp;
};

#endif
