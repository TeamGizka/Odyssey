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
    static const unsigned long STRREF_REMAINING_POINTS = 210;
    static const unsigned long STRREF_STRENGTH_DESC = 222;
    static const unsigned long STRREF_DEXTERITY_DESC = 223;
    static const unsigned long STRREF_CONSTITUTION_DESC = 224;
    static const unsigned long STRREF_WISDOM_DESC = 225;
    static const unsigned long STRREF_INTELLIGENCE_DESC = 226;
    static const unsigned long STRREF_CHARISMA_DESC = 227;
    static const unsigned long STRREF_LEVEL_UP = 1071;
    static const int STRREF_SPEND_ALL_POINTS_XBOX = 41816;
    static const int STRREF_SCORE_BELOW_MINIMUM = 42180;
    static const int STRREF_SCORE_ABOVE_MAXIMUM = 42181;
    static const int STRREF_SPEND_ALL_POINTS = 48729;

    CSWGuiAbilitiesCharGen(CSWGuiManager* pManager, CSWCCreature* pCreature,
                           int bLevelUp);
    virtual ~CSWGuiAbilitiesCharGen();

    virtual void OnPanelAdded();
    virtual void HandleInputEvent(int nEvent, int nState);
    // PC 0x006D2FC0 GOG 0x006D2D50
    void OnMinusButtonPushed(CSWGuiControl* pControl)
    {
        HandleInputEvent(GUIEVENT_LEFT, 1);
    }
    // PC 0x006D2FD0 GOG 0x006D2D60
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
    CSWGuiLabel m_alblBar[4];
    CSWGuiLabel m_alblBonus[6];
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
#ifndef _XBOX
    CSWGuiLabel m_lblPortrait;
    Vector m_v49CC;
    Vector m_v49D8;
    Vector m_v49E4;
    Vector m_v49F0;
    Vector m_v49FC;
    Vector m_v4A08;
#endif
};

#endif
