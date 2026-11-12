#ifndef __SWGUIMAININTERFACEPC2_H__
#define __SWGUIMAININTERFACEPC2_H__

#include "swguicontrol.h"
#include "swgui.h"
#include "swguiingamecharacter.h"
class CResRef;
class CSWCCreature;
class CSWCObject;
class CSWGuiControl;
class CSWGuiExtent;
class CSWGuiManager;
class CSWGuiCharButton_vtbl;
class CSWGuiProgressBar_vtbl;
class CSWGuiMainInterfaceChar;
class CSWGuiMainInterface;
class CAurGUIImageInternal;

enum CSWGuiMainInterfaceStatusEffectFlags
{
    STATUS_EFFECT_ENABLED = 2,
    STATUS_EFFECT_DISPLAYED = 1
};

struct CSWGuiControlFlags
{
#ifdef _XBOX
    unsigned long m_bTargetChanged : 1;
#endif
    unsigned long m_bUnknown : 1;
    unsigned long m_bEnabled : 1;
#ifdef _XBOX
    unsigned long m_nUnused : 29;
#else
    unsigned long m_nUnused : 30;
#endif
};
class CSWSCombatRound;
class CSWSCombatRoundAction;
class CSWSObjectActionNode;
class Vector;

#ifndef _XBOX
class CSWGuiCharButton : public CSWGuiButton
{
public:
    CSWGuiMainInterfaceChar* m_pCharacter;
    CExoString m_sToolTipText;

    enum ToolTipStringRef
    {
        STRREF_FORCE = 1060,
        STRREF_VITALITY = 1061,
        STRREF_LEVEL = 32154,
        STRREF_XP_NEEDED = 48459
    };
    void UpdateToolTipText();
    SWGUI_PCVIRTUAL int DisplayToolTip();
    virtual void Draw(float);
};
#endif

#ifdef _XBOX
class CSWGuiCharBar : public CSWGuiProgressBar
{
public:
    CSWGuiExtent m_cStartExtent; virtual void SetExtent(const CSWGuiExtent& cExtent); virtual void Draw(float fElapsed);
};
#endif

#define SWGUI_FLOATY_TEXT_MAX_LINES 5

class CSWGuiFloatyText : public CSWGuiBlackenedLabel
{
public:
    float m_fTimeLeft;
    float m_fDuration;
    unsigned long m_oidObject;
    unsigned char m_bActive;
    short m_nScreenX;
    short m_nScreenY;
    void CreateEntry(unsigned long, CExoString const&, Vector const&, float);
    virtual void Update(float);

    static CSWGuiMainInterface* m_pMainInterface;
};

class CSWGuiMainInterfaceAction
{
public:

    enum ToolTipStringRef
    {
        STRREF_ACTION_MENU_UP = 48465,
        STRREF_ACTION_MENU_DOWN = 48466,
        STRREF_ACTIVATE_FRIENDLY_POWER = 48486,
        STRREF_ACTIVATE_MEDICAL_ITEM = 48291,
        STRREF_ACTIVATE_NON_MEDICAL_ITEM = 48299,
        STRREF_ACTIVATE_MINE = 48295,
        STRREF_ACTIVATE_LEFT_ACTION = 48303,
        STRREF_ACTIVATE_MIDDLE_ACTION = 48307,
        STRREF_ACTIVATE_RIGHT_ACTION = 48311
    };
#ifndef _XBOX
    CSWGuiButton m_CSWGuiButton_0;
    CSWGuiButton m_CSWGuiButton_1C4;
    CSWGuiButton m_CSWGuiButton_388;
    CSWGuiButton m_CSWGuiButton_54C;
    float m_fFadeTimer;
    char m_nFadeSteps;
    char gap_715[3];
    int m_bTarget;
    void Pulse();
    CSWGuiControl* HitCheckMouse(int, int);
    int GetActive();
#else
    CSWGuiExtent m_cArrowExtent;
    CSWGuiExtent m_cArrowBExtent;
    CSWGuiExtent m_cBorderExtent;
    CSWGuiExtent m_cBorderBExtent;
    CSWGuiExtent m_cIconExtent;
    CSWGuiExtent m_cIconBExtent;
    CSWGuiLabel m_lblIcon;
    CSWGuiLabel m_lblIconB;
    CSWGuiLabelHilight m_lbhArrow;
    CSWGuiLabelHilight m_lbhBorder;
    float m_fFadeTimer;
    float m_fSlideTimer;
    char m_nFadeSteps;
    void StopSlide();

    void Pulse()
    {
        m_nFadeSteps = 2;
        m_fFadeTimer = 0.1f;
    }
#endif
    void Draw(float);
    void Update(float);
#ifndef _XBOX
    void SetIcon(CResRef const&, CExoString const&, int, unsigned char);
#else
    void SetIcon(CResRef const& cIcon, int bEnabled, unsigned char nSlide);
#endif
    CSWGuiMainInterfaceAction();
#ifndef _XBOX
    void Show(int, int);
#else
    void Show(int bActive, int bEmpty, int bArrow);
#endif
    ~CSWGuiMainInterfaceAction();
#ifndef _XBOX
    void Initialize(CSWGuiMainInterface* pMainInterface, int nAction, int bTarget);
#else
    void Initialize(CSWGuiMainInterface* pMainInterface, int nAction);
#endif
};

#ifndef _XBOX
class CSWGuiTargetActionMenu
{
public:
    CExoArrayList<CSWGuiInterfaceAction> m_aActionLists[3];
    int m_aSelectedActions[4][3];
    CSWGuiMainInterfaceAction m_aActions[3];
    CSWGuiMainInterface* m_pMainInterface;
    CSWGuiExtent m_cExtent;

    CSWGuiExtent m_cBounds;

    enum { MENU_POINT_SPACING = 32 };
    CSWGuiLabel m_cNameLabel;
    CSWGuiLabel m_cNameBackground;
    CSWGuiLabel m_cHealthBackground;
    CSWGuiProgressBar m_cHealthBar;

    float m_fFeedbackTimer;
    float m_fMouseOverTimer;
    unsigned long m_nFeedbackStrRef;
    unsigned char m_nHealthBarSpacing;
    unsigned char m_nActionSpacing;
    char m_nTargetType;
    char m_nSelectedAction;
    unsigned int m_bVisible : 1;
    unsigned int m_bMouseOver : 1;
    unsigned int : 30;
    char IsTargetActionMenuControl(CSWGuiControl*);
    void ResetFont();
    void SetFriend(int, int);
    void SetNameLabel(CExoString const&);
    void UpdateNameLabel(CSWCObject*);
    void Draw(float);
    void Update(float fElapsed);
    CSWGuiControl* HitCheckMouse(int, int);
    void PositionMenu(CSWCCreature*, CSWCObject*);
    void SelectNextAction(int nMenu);
    void SelectPrevAction(int nMenu);
    void PopulateMenus(CSWCCreature*, int, CSWCObject*);
    void DoTargetAction(int, int);
    ~CSWGuiTargetActionMenu();
    CSWGuiTargetActionMenu();
    void Initialize(CSWGuiMainInterface* pMainInterface);
};
#endif

class CSWGuiMainInterfaceChar
{
public:

    enum { MAX_LARGE_STACK_COUNT = 9, MAX_SMALL_STACK_COUNT = 5 };

    enum LabelIndex
    {
        LABEL_BACK, LABEL_DEBILATATED, LABEL_DISABLED,
#ifdef _XBOX
        LABEL_LEVELUP, LABEL_LVLUPBG, LABEL_CHAR
#else
        LABEL_LEVELUP, LABEL_CHAR
#endif
    };
    enum BarIndex { BAR_VIT, BAR_FORCE };
#ifndef _XBOX
    int m_nCurHealth;
    int m_nMaxHealth;

    int m_nCurForce;
    int m_nMaxForce;
    int m_nLevel;
    int m_nXPNeeded;
    CExoString m_sName;
    CExoString m_sDescription;
#endif
#ifndef _XBOX
    CSWGuiLabel m_aLabels[5];
    CSWGuiProgressBarReveal m_aBars[2];
#else
    CSWGuiLabel m_aLabels[6];
    CSWGuiCharBar m_aBars[2];
#endif
    CSWGuiLabel m_cBuffLabel;
    CSWGuiLabel m_cDebuffLabel;
#ifndef _XBOX
    CSWGuiCharButton m_cCharButton;
#endif
    short m_nStartY;
    short m_nStartX;
    short m_nEndY;
    short m_nEndX;
    unsigned char m_nBuffCount;
    unsigned char m_nDebuffCount;
    char gap_EA2[2];
#ifndef _XBOX
    int m_bToolTipDirty;
#else
    void SetDestination(int nPosition, int nCount, CSWGuiManager* pManager);
#endif
    void Adjust(float);
    void DrawStack(float fElapsed, int bLarge, int bBuff);
    void DrawEffects(float fElapsed, int bLarge);

    enum PortraitState { PORTRAIT_DEAD = 1, PORTRAIT_DEBILITATED = 2,
                         PORTRAIT_LEVEL_UP = 4 };
    void SetPortrait(const CResRef& cPortrait)
    {
        m_aLabels[LABEL_CHAR].m_cBorder.m_cParams.SetFillImage(cPortrait, 0);
    }
    void SetForce(int nCurForce, int nMaxForce)
    {
        if (nMaxForce == 0)
        {
            m_aBars[BAR_FORCE].SetMaxValue(1);
            m_aBars[BAR_FORCE].SetCurValue(0);
        }
        else
        {
            m_aBars[BAR_FORCE].SetMaxValue(nMaxForce);
            m_aBars[BAR_FORCE].SetCurValue(nCurForce);
        }
#ifndef _XBOX
        m_nCurForce = nCurForce;
        m_nMaxForce = nMaxForce;
        m_bToolTipDirty = 1;
#endif
    }
#ifndef _XBOX
    void SetLevel(int nLevel)
    {
        m_nLevel = nLevel;
        m_bToolTipDirty = 1;
    }
    void SetXPNeeded(int nXPNeeded)
    {
        m_nXPNeeded = nXPNeeded;
        m_bToolTipDirty = 1;
    }
#endif
    void SetHealth(int nCurHealth, int nMaxHealth, int bAlive);
    void Show(int, unsigned char, unsigned char, unsigned char);
    void Initialize(CSWGuiMainInterface*, int);
    CSWGuiMainInterfaceChar();
    ~CSWGuiMainInterfaceChar();
};

class CSWGuiMainInterfaceStatus
{
public:
    CSWGuiLabel m_cLabel;
    float m_fDisplayTime;
#ifndef _XBOX
    int m_nDisplayFlags;
#else
    unsigned int m_bDisplayed : 1;
    unsigned int m_bWasEnabled : 1;
    unsigned int m_bPaused : 1;
#endif
    unsigned char m_nStatusEffect;
    void Initialize(CSWGuiMainInterface*, unsigned char);
    void ShiftControl(int nOffset);
    void Show(int bShow)
    {
        if (bShow)
        {
            if (m_cLabel.m_bEnabled == 0)
            {
                m_cLabel.SetEnabledFlag(1);
            }
            m_fDisplayTime = 4.0f;
#ifndef _XBOX
            m_nDisplayFlags |= STATUS_EFFECT_DISPLAYED;
#else
            m_bDisplayed = 1;
#endif
        }
        else
        {
            m_cLabel.SetEnabledFlag(0);
        }
    }
    void Update(float);
    CSWGuiMainInterfaceStatus();
};

class CSWGuiMainInterface : public CSWGuiPanel
{
public:

    enum { ACTION_QUEUE_ICON_COUNT = 4 };

    static const int STICKY_RETICLE_MAX_SIZE = 64;
    static const int STICKY_RETICLE_CREATURE_MIN_SIZE = 16;
    static const int STICKY_RETICLE_OBJECT_MIN_SIZE = 32;
    static const int STICKY_RETICLE_MARGIN = 32;
    static const int STICKY_RETICLE_NEAR_RANGE = 5;
    static const int STICKY_RETICLE_FAR_RANGE = 30;

    enum { ACTION_QUEUE_FIRST_ICON = 4, ACTION_ICON_COUNT = 7 };

    static const int FOG_POINT_SPAN = 4;

    static const unsigned long STRREF_COMBAT_MODE_ENGAGED = 111303;

    static const unsigned long STRREF_DEBILITATED = 47915;

    static const unsigned long STRREF_FORCE_DEPLETED = 38613;
    static const unsigned long STRREF_RESTRICTED_BY_ARMOR = 38614;
    static const unsigned long STRREF_MISSING_ITEM = 38615;
    static const unsigned long STRREF_TARGET_TOO_CLOSE = 42422;
    static const unsigned long STRREF_FULL_HEALTH = 42498;
    static const unsigned long STRREF_PC_DEAD = 47936;

    static const unsigned long STRREF_HOTKEY_HELP = 48458;
    static const unsigned long STRREF_MENU_EQUIPMENT = 48219;
    static const unsigned long STRREF_MENU_INVENTORY = 48220;
    static const unsigned long STRREF_MENU_CHARACTER = 48225;
    static const unsigned long STRREF_MENU_MAP = 48221;
    static const unsigned long STRREF_MENU_ABILITIES = 48224;
    static const unsigned long STRREF_MENU_JOURNAL = 48218;
    static const unsigned long STRREF_MENU_OPTIONS = 48222;
    static const unsigned long STRREF_MENU_MESSAGES = 48223;
    static const unsigned long STRREF_PAUSE = 48019;
    static const unsigned long STRREF_SOLO = 48035;
    static const unsigned long STRREF_STEALTH = 247;
    static const unsigned long STRREF_CLEAR_ONE = 48456;
    static const unsigned long STRREF_CLEAR_ALL = 48518;
#ifndef _XBOX
    unsigned long m_nTargetId;

    unsigned long m_oidPlayerCharacter;
    float m_fActionDescriptionTimer;

    unsigned long m_nActionDescription;
    CExoArrayList<CSWGuiInterfaceAction> m_aPersonalActionLists[7];
    CSWGuiTargetActionMenu m_cTargetActionMenu;
#endif
#ifndef _XBOX
    char IsTargetActionMenuControl(CSWGuiControl* pControl)
    {
        return m_cTargetActionMenu.IsTargetActionMenuControl(pControl);
    }
#endif
#ifndef _XBOX
    int m_aSelectedPersonalActions[7];
    int m_nDefaultAction;
    float m_f1C90;
    float m_f1C94;

    CSWGuiLabel m_aMoulding[2];

    CSWGuiMainInterfaceChar m_aCharacters[3];
    CSWGuiMainInterfaceChar m_cTransitionCharacter;
    float m_fPortraitTransitionTime;

    CSWGuiLabel m_lblReticle;

    CSWGuiLabel m_lblStickyReticle;
    unsigned long m_oidStickyTarget;
    float m_fTargetUpdateDelay;
    CExoArrayList<CSWGuiFloatyText*> m_lFloatyText;

    CSWGuiLabel m_lblMapborder;

    CSWGuiLabel m_lblMap;
    CSWGuiLabel m_lblArrow;

    CSWGuiExtent m_cMapViewport;
    int m_nMapWidth;
    int m_nMapHeight;
    CSWGuiButton m_btnMinimap;

    CSWGuiLabel m_aActionIcons[ACTION_QUEUE_ICON_COUNT];
    CSWGuiLabel m_aCombatbg[2];
    CSWGuiLabelHilight m_CSWGuiLabelHilight_6B1C;
    CSWGuiButton m_btnClearone;
    CSWGuiButton m_btnClearone2;
    CSWGuiButton m_btnClearall;

    CSWGuiLabel m_lblActionQueueBackground;
    CSWGuiLabel m_lblCmbtmodemsg;
    CSWGuiLabel m_cToolTipBackground;
    CSWGuiLabel m_cToolTip;
    int m_bToolTipShown;

    float m_fCombatMessageDuration;
    float m_fCombatMessageTime;
    unsigned long m_nCombatMessage;

    CSWGuiMainInterfaceAction m_aPersonalActions[7];
    CSWGuiLabel m_cActionText;
    CSWGuiLabel m_cActionTextBack;
    int m_nActionTextBottom;
    unsigned char m_nInterfaceOffset;
    char gap_A459[3];
    CSWGuiControlFlags m_cActionDescriptionState;

    CSWGuiMainInterfaceStatus m_aEffects[13];
    CSWGuiButton m_btnEqu;
    CSWGuiButton m_btnInv;
    CSWGuiButton m_btnChar;
    CSWGuiButton m_btnAbi;
    CSWGuiButton m_btnMsg;
    CSWGuiButton m_btnJou;
    CSWGuiButton m_btnMap;
    CSWGuiButton m_btnOpt;
    CSWGuiLabel m_lblMenubg;
    CSWGuiLabel m_lblArrowMargin;
    CSWGuiButtonToggle m_tbPause;
    CSWGuiButtonToggle m_tbSolo;
    CSWGuiButtonToggle m_tbStealth;
    CSWGuiButton m_btnSwapWeapons;

    CAurGUIImageInternal* m_pBlackDot;
    int m_nD560;
    int m_nD564;
    unsigned long m_nD568;
    CSWGuiButton m_btnD56C;
    int m_nD73C;
    int m_nD740;
    int m_nD744;
#else
    unsigned long m_nTargetId;
    unsigned long m_oidPlayerCharacter;
    float m_fActionDescriptionTimer;
    unsigned long m_nActionDescription;
    CExoArrayList<CSWGuiInterfaceAction> m_aPersonalActionLists[6];
    int m_aSelectedPersonalActions[5][6];
    int m_nDefaultAction;
    CSWGuiMainInterfaceChar m_aCharacters[3];
    CSWGuiMainInterfaceChar m_cTransitionCharacter;
    float m_fPortraitTransitionTime;
    CSWGuiLabel m_lblName;
    CSWGuiLabel m_lblReticle;
    CSWGuiLabel m_lblIndicate;
    CSWGuiProgressBar m_pbHealth;
    float m_fTargetUpdateDelay;
    CExoArrayList<CSWGuiFloatyText*> m_lFloatyText;
    CSWGuiLabel m_lblMapborder;
    CSWGuiLabel m_lblMap;
    CSWGuiLabel m_lblArrow;
    CSWGuiExtent m_cMapViewport;
    int m_nMapWidth;
    int m_nMapHeight;
    CSWGuiLabel m_aActionIcons[ACTION_QUEUE_ICON_COUNT];
    CSWGuiLabel m_lblBg1;
    CSWGuiLabel m_lblBg2;
    CSWGuiLabelHilight m_CSWGuiLabelHilight_6B1C;
    CSWGuiLabel m_lblActionQueueBackground;
    CSWGuiLabel m_lblCmbtmodemsg;
    float m_fCombatMessageDuration;
    float m_fCombatMessageTime;
    unsigned long m_nCombatMessage;
    CSWGuiMainInterfaceAction m_aPersonalActions[6];
    CSWGuiLabel m_cActionText;
    CSWGuiLabel m_cActionTextBack;
    CSWGuiLabel m_lblActiondescbg2;
    CSWGuiLabel m_lblSolomode;
    unsigned char m_aSelectedMenu[5];
    unsigned char m_nSelectedAction;
    unsigned char m_nTargetType;
    unsigned char m_nActionTextMargin;
    unsigned char m_nInterfaceOffset;
    CSWGuiControlFlags m_cActionDescriptionState;
    int m_nXboxUnmapped5F18;
    CSWGuiExtent m_cActionTextBackExtent;
    CSWGuiMainInterfaceStatus m_aEffects[9];
#endif
    void SetActionDescription(CExoString const&);
    int SetTarget(unsigned long, int);
    void ShowActionIcon(int&, CResRef const&, CResRef const&, int, int);
    void OnDefaultActionFocus(CSWGuiControl*);
    void DisplayToolTipMessage();
    void RemoveToolTipMessage();
#ifndef _XBOX
    SWGUI_PCVIRTUAL void ResetFont();
    virtual CSWGuiControl* HitCheckMouse(int, int);
#endif
    virtual void Update(float);
#ifndef _XBOX
    void UpdateActionDescription();
#else
    void UpdateActionDescription(CSWGuiControl* pControl);
    void DrawActionDescription(float fElapsed);
    virtual void HandleInputEvent(int nEvent, int nState);
#endif
    void GetActionIcon(CSWSCombatRound*, CSWSObjectActionNode*,
                      CSWSCombatRoundAction*, int&, CResRef&, CResRef&);
    void ShiftControl(CSWGuiControl& cControl, int nOffset);
    void ShowActionQueue(int);
    void SetCombatMessage(unsigned long nMessage);
    void UpdatePortraits();
    void InitializeMap();
#ifdef _XBOX
    void StartRotation();
#else
    int StartRotation();
#endif
    void ShowFlashingStatus(unsigned char nEffect);
    void RemoveFloatyText(CSWGuiFloatyText*);
#ifdef _XBOX
    void OnCombatXButton(int nEvent); void OnNonCombatXButton();
#endif
    void OnCombatYButton();
    void UpdateAndDrawFogOfWar(CSWGuiExtent const&);
    void OnMenuButtonPressed(CSWGuiControl*);
    void OnTargetActionFocus(CSWGuiControl*);
    void OnTargetUpArrowPressed(CSWGuiControl*);
    void OnTargetDownArrowPressed(CSWGuiControl*);
    void OnPauseButtonPressed(CSWGuiControl*);
    void OnSoloButtonPressed(CSWGuiControl*);
    void OnStealthButtonPressed(CSWGuiControl*);
    void OnCharacterClicked(CSWGuiControl*);
    void OnCharacterRightClicked(CSWGuiControl*);
    void ClearOneAction();
    void ClearAllActions();
    void QueueActiveAction();
    void SelectNextPersonalAction(int);
    void SelectPrevPersonalAction(int);
    void UpdateStickyTarget();
    void SetupPauseGuiExtent(CSWGuiExtent*);
    void PopulateMenus();
    void UpdateActionQueue();
    void UpdateIndicator(float);
    void DrawMap(float);
    void DoTargetAction(int, int);
    void DoPersonalAction(int, int);
    void OnActionUpArrowPressed(CSWGuiControl*);
    void OnActionDownArrowPressed(CSWGuiControl*); void OnClearOneButtonPressed(CSWGuiControl*); void OnClearAllButtonPressed(CSWGuiControl*);
#ifndef _XBOX
    ~CSWGuiMainInterface();
#endif
    virtual void Draw(float);
    void AddFloatyText(unsigned long, CExoString const&, Vector const&, float);
    void OnDefaultActionLeft(CSWGuiControl*);
    CSWGuiMainInterface(CSWGuiManager*);
};

#endif
