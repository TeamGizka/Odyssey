#ifndef __SWGUIINGAME_H__
#define __SWGUIINGAME_H__

#include "swguicontrol.h"
#include "swgui.h"
#include "serverexoappinternal.h"
#include "guiingame.h"
class CSWGuiControl;
class CSWGuiManager;
class Vector;
class CSWGuiSkillInfoBox;
class CSWGuiMessageBox;
class CGuiInGameMessage;
class CGuiInGameDialogMessage;
class CSWGuiLabel_vtbl;
#include "math3d.h"
#include "swguicustompanel.h"
#include "swguimaininterfacepc2.h"

class CSWGuiControllerLossBox : public CSWGuiMessageBox
{
public:
    CSWGuiControllerLossBox(CSWGuiManager* pManager);
    void OnAccept(CSWGuiControl* pControl);
#ifdef _XBOX

#define STRREF_CONTROLLER_LOST 42130
#else
#define STRREF_CONTROLLER_LOST 48398
#endif
    virtual void OnPanelAdded();
};

class CSWGuiInGameMenu : public CSWGuiPanel
{
public:
    CSWGuiLabelHilight m_LblhEqu;
    CSWGuiLabelHilight m_LblhInv;
    CSWGuiLabelHilight m_LblhCha;
    CSWGuiLabelHilight m_LblhMap;
    CSWGuiLabelHilight m_LblhAbi;
    CSWGuiLabelHilight m_LblhJou;
    CSWGuiLabelHilight m_LblhOpt;
    CSWGuiLabelHilight m_LblhMsg;
#ifndef _XBOX
    CSWGuiButton m_btnEqu; CSWGuiButton m_btnInv; CSWGuiButton m_btnChar;
    CSWGuiButton m_btnMap; CSWGuiButton m_btnAbi; CSWGuiButton m_btnJou;
    CSWGuiButton m_btnOpt; CSWGuiButton m_btnMsg;
#else
    CSWGuiLabel m_lblLeftArrow;
    CSWGuiLabel m_lblRightArrow;
#endif
    CSWGuiLabel m_lblTopClass1;
    CSWGuiLabel m_lblTopClass2;
    CSWGuiLabel m_lblTopClass1Level;
    CSWGuiLabel m_lblTopClass2Level;
    CSWGuiLabel m_lblCharName;
    CSWGuiMainInterfaceChar m_cChar2350;
    CSWGuiLabel m_lblChar2;
    CSWGuiLabel m_lblLevelUp2;
    CSWGuiLabel m_lblBack2;
    CSWGuiButton m_btnChange2;
    CSWGuiLabel m_lblChar3;
    CSWGuiLabel m_lblLevelUp3;
    CSWGuiLabel m_lblBack3;
    CSWGuiButton m_btnChange3;
    CSWGuiLabel m_lblSecTitle;
    int m_n3DCC;
    int m_nTopMenuOverrideCharacter;
    virtual void HandleInputEvent(int, int);
    void SetActiveControlID(int nControl, int bActive);
    void OnShoulderLeft(CSWGuiControl* pControl);
    void OnShoulderRight(CSWGuiControl* pControl);
    virtual void Update(float);
    void OnEquipButtonPressed(CSWGuiControl* pControl);
    void OnInvButtonPressed(CSWGuiControl* pControl);
    void OnCharButtonPressed(CSWGuiControl* pControl);
    void OnMapButtonPressed(CSWGuiControl* pControl);
    void OnAbiButtonPressed(CSWGuiControl* pControl);
    void OnJouButtonPressed(CSWGuiControl* pControl);
    void OnOptButtonPressed(CSWGuiControl* pControl);
    void OnMsgButtonPressed(CSWGuiControl* pControl);
    CSWGuiInGameMenu(CSWGuiManager*);
};

class CSWGuiInGameMessages : public CSWGuiPanel
{
public:

    CSWGuiListBox m_lbMessages;
    CSWGuiListBox m_lbDialog;
    CSWGuiListBox m_lbCombat;
    CSWGuiListBox m_lbEffectsGood;
    CSWGuiListBox m_lbEffectsBad;
    CSWGuiLabel m_lblTitle;
#ifdef _XBOX
    CSWGuiLabel m_lblStart; CSWGuiLabel m_lblExit;
    CSWGuiLabel m_lblX; CSWGuiLabel m_lblXDesc;
#endif
    CSWGuiLabelHilight* m_pMessageLabels;
    CSWGuiLabelHilight* m_pDialogLabels;
    CSWGuiLabelHilight* m_pCombatLabels;
    CSWGuiLabel m_lblEffectsGood;
    CSWGuiLabel m_lblEffectsBad;
#ifndef _XBOX
    CSWGuiButton m_btnBack;
#endif
    CSWGuiLabel m_alblBar[6];
    CSWGuiButton m_btnDialog;
    CSWGuiButton m_btnFeedback;
    CSWGuiButton m_btnCombat;
    CSWGuiButton m_btnEffects;
    CSWGuiLabel m_lblFilter;
    void ToggleMessages();
    void ShowDialogMessages();

    static const unsigned long STRREF_SHOW_FEEDBACK = 42142;
    static const unsigned long STRREF_DIALOG = 371;
    static const unsigned long STRREF_SHOW_DIALOG = 42143;
    static const unsigned long STRREF_FEEDBACK = 42167;
    static const unsigned long STRREF_MESSAGES = 1563;
    void ShowFeedbackMessages();
    CSWGuiInGameMessages(CSWGuiManager*);
    ~CSWGuiInGameMessages();

    static const int INITIAL_MESSAGE_HEIGHT = 10;
    void AddMessages(CGuiInGameMessage*, unsigned long);
    void AddDialogMessages(CGuiInGameDialogMessage*, unsigned long);
    virtual void OnPanelAdded();
    virtual void HandleInputEvent(int, int);
};

class CSWGuiStatusSummary : public CSWGuiPanel
{
public:

    static const unsigned long STRREF_CREDITS_RECEIVED = 0xa5c5;
    static const unsigned long STRREF_CREDITS_LOST = 0xa683;
    static const unsigned long STRREF_NET_CHANGE = 0xbb3d;
    static const unsigned long STRREF_XP_RECEIVED = 0xa5c6;
    static const unsigned long STRREF_STEALTH_XP_RECEIVED = 0xa5c7;
    static const unsigned long STRREF_NET_LIGHT_SHIFT = 0xbb3f;
    static const unsigned long STRREF_NET_DARK_SHIFT = 0xbb3e;
    CStatusSummary m_cStatusSummary;
    unsigned int m_bWasPaused : 1;
    unsigned int m_nStatusFlagsUnused : 31;
    int m_n94;
#ifdef _XBOX
    CSWGuiLabel m_lblA; CSWGuiLabel m_lblOk;
#else

    CSWGuiLabel m_lblUnbound1;
    CSWGuiLabel m_lblUnbound2;
#endif

    CSWGuiLabel m_aStatusLabels[GUI_STATUS_COUNT];
    CSWGuiLabel m_aStatusDescriptions[GUI_STATUS_COUNT];
#ifndef _XBOX
    CSWGuiButton m_btnOk;
#endif
    virtual void OnPanelRemoved();
    void AddAlignmentShift(char);
    void AddCredits(int nCredits);
    void AddExperience(int nExperience);
    void AddStealthExperience(int nExperience);
    virtual void HandleInputEvent(int, int);
    void PlayPendingSounds()
    {
        if (m_cStatusSummary.m_bLevelUpSound)
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_LEVELUP);
        if (m_cStatusSummary.m_bNewQuestSound)
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_NEWQUEST);
        if (m_cStatusSummary.m_bCompleteSound)
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_QUESTCOMPLETE);
    }
    void DisplayStatus();
    virtual void OnPanelAdded();
    CSWGuiStatusSummary(CSWGuiManager*);
};

#include "swguidrawable.h"
#include "swguicontrol.h"
#include "swgui.h"

class CSWCCreature;
class CSWSkill;
#include "swguiskillflowchart.h"

struct CSWGuiSkillStruct
{
    CSWSkill* m_pSkill;
    int m_nRank;
    int m_nBonus;
};

class CSWGuiInGameAbilities : public CSWGuiPanel
{
public:

    enum
    {
        CHARACTER_PLAYER = -1,
        CHARACTER_UNSET = -2
    };

    CSWGuiInGameAbilities(CSWGuiManager* pManager);
    virtual ~CSWGuiInGameAbilities();

    virtual void HandleInputEvent(int nEvent, int nState);
    virtual void OnPanelAdded();
    virtual void OnPanelRemoved();

    void CreatePowerList();

    void UpdateDescriptionListBox(int bShow);
    void SetDescription(unsigned long nStrRef);
    void SetDescription(const CExoString& sText);
    int DisplayPowers();
#ifndef _XBOX
    void SetButtonColor(CSWGuiControl* pControl);
#endif
    void OnEnterPower(unsigned long nPower);

    enum { STRREF_LIGHT_SIDE = 41891, STRREF_DARK_SIDE = 41890,
           STRREF_ADJUSTMENT = 47888 };
    void OnEnterSkill(CSWGuiControl* pControl);
    void OnEnterFeat(unsigned short nFeat);
#ifndef _XBOX
    void OnAbilitySelectionChanged(CSWGuiControl* pControl);
#endif
    void UpdateView();
    void CreateFeatList();
    void CreateSkillList();
#ifndef _XBOX
    void OnFeatsButtonPressed(CSWGuiControl* pControl);
    void OnPowersButtonPressed(CSWGuiControl* pControl);
    void OnSkillsButtonPressed(CSWGuiControl* pControl);
#endif
    void SetCharacter(CSWCCreature* pCreature);

    CExoArrayList<CSWGuiSkillStruct> m_lstSkills;
    CSWGuiInGameSkillEntry m_aSkillEntries[10];
#ifdef _XBOX
    CSWGuiLabel m_lblPortrait;
#endif
    CSWGuiLabel m_lblDescription;
    CSWGuiLabel m_lblSkillRank;
    CSWGuiLabel m_lblRankValue;
    CSWGuiLabel m_lblBonus;
    CSWGuiLabel m_lblBonusValue;
    CSWGuiLabel m_lblTotal;
    CSWGuiLabel m_lblTotalValue;
    CSWGuiLabel m_lblName;
#ifdef _XBOX
    CSWGuiLabel m_lblX;
    CSWGuiLabel m_lblY;
    CSWGuiLabel m_lblWhite;
    CSWGuiLabel m_lblStart;
    CSWGuiLabel m_lblBlack;
    CSWGuiLabel m_lblSkills;
    CSWGuiLabel m_lblPowers;
    CSWGuiLabel m_lblFeats;
    CSWGuiLabel m_lblNextPage;
    CSWGuiLabel m_lblExit;
    CSWGuiLabel m_lblRAnalog;
    CSWGuiLabel m_lblInfoBackground;
#else
    CSWGuiLabel m_lblInfoBackground;
    CSWGuiButton m_btnFeats;
    CSWGuiButton m_btnPowers;
    CSWGuiButton m_btnSkills;
    CSWGuiLabel m_alblBar[7];
    CSWGuiLabel m_lblFilter;
    CSWGuiLabel m_lblAbilities;
    CSWGuiListBox m_lbDescFeats;
    CSWGuiLabel m_lbl3F74;
#endif
    CSWGuiListBox m_lbAbility;
    CSWGuiListBox m_lbDescription;
#ifndef _XBOX
    CSWGuiButton m_btnExit;
#endif
    unsigned char m_nSelectedSkill;
    signed char m_nCharacterIndex;
    unsigned char m_aUnmapped3F72[2];
    CSWCCreature* m_pCreature;
    CSWGuiSkillFlowChart m_cPowerChart;
    CSWGuiSkillFlowChart m_cFeatChart;
};

#include "swguicontrol.h"
#include "swgui.h"
class CSWGuiControl;
class CSWGuiManager;
#include "exobase.h"
#include "nwsitem.h"

class CSWCCreature;
class CSWSCreature;
class CSWSItem;

#ifndef _XBOX
class CSWGuiInGameInventory : public CSWGuiPanel
{
public:

    enum { STRREF_ITEM_USED_THIS_ROUND = 42409 };

    CSWGuiInGameInventory(CSWGuiManager* pManager);
    virtual ~CSWGuiInGameInventory();
    virtual void Draw(float fElapsed);
    virtual void HandleInputEvent(int nEvent, int nState);
    virtual void OnPanelAdded();
    virtual void OnPanelRemoved();

    int CheckFilter(CSWSItem* pItem, CSWSCreature* pCreature, int nSlot);
    void PopulateItemListBox();
    void SetNextFilter();
    void OnControlSelected(CSWGuiControl* pControl);
    void OnControlEntered(CSWGuiControl* pControl);
    void CreateItemEntry(CSWCCreature* pCreature, int& nItem,
                        CExoArrayList<CSWGuiControl*>& lControls,
                        CSWSItem* pItem, int bSquadItem, int bEquipped);
    void CantEquip(CSWGuiControl* pControl);
    void NotUseable(CSWGuiControl* pControl);
    void NotUseableOnHawk(CSWGuiControl* pControl);
    void FullHealth(CSWGuiControl* pControl);
    void SquadFullHealth(CSWGuiControl* pControl);

    static const int m_nFilterName[6];

    CSWGuiLabel m_lblUnknown64;
    CSWGuiLabel m_lblInv;
    CSWGuiLabel m_lblCredits;
    CSWGuiLabel m_lblCreditsValue;
    CSWGuiListBox m_lbxItems;
    CSWGuiListBox m_lbxDescription;
    CSWGuiButton m_btnExit;
    CSWGuiButton m_btnUseItem;
    CSWGuiLabel m_alblBar[6];
    CSWGuiLabel m_lblFilter;
    CSWGuiButton m_btnAll;
    CSWGuiButton m_btnDatapads;
    CSWGuiButton m_btnWeapons;
    CSWGuiButton m_btnArmor;
    CSWGuiButton m_btnUseable;
    CSWGuiButton m_btnMisc;
    CSWGuiButton m_btnQuests;
    CExoArrayList<CSWGuiControl*> m_aItemEntries;
    CExoArrayList<unsigned long> m_aNewItems;
    unsigned long m_oidSelectedItem;
    short m_nSavedTopItem;
    short m_nSavedSelected;
    unsigned long m_bUpdateItems : 1;

    unsigned long m_bRestoreSelection : 1;
    unsigned long m_bUpdateUseItem : 1;
    unsigned long m_nInventoryFlagsUnused : 29;
};
#endif

class CSWGuiControl;
class CSWGuiManager;
#include "exobase.h"
#include "swgui.h"
#include "swguicontrol.h"
#include "nwcjournal.h"

class CSWGuiQuestItem;

class CSWGuiJournalItemEntry : public CSWGuiButton
{
public:
    CSWGuiJournalItemEntry()
    {
        m_nEntryIndex = 0;
        m_bEncouraged = 0;
    }

    void SetEncouraged(int bEncouraged);

    int m_nEntryIndex;
    unsigned long m_bEncouraged : 1;
    unsigned long m_nUnknown1C8Rest : 31;
};

class CSWGuiInGameJournal : public CSWGuiPanel
{
public:
    CSWGuiInGameJournal(CSWGuiManager* pManager);
    virtual ~CSWGuiInGameJournal();
    virtual void HandleInputEvent(int nEvent, int nState);
    virtual void OnPanelAdded();
    virtual void OnPanelRemoved();
    virtual CSWJournalEntry* GetQuestEntryAt(int nIndex);
    virtual void Draw(float fElapsed);

    void OnControlEntered(CSWGuiControl* pControl);
    void PopulateItemListBox();
    void MarkNewQuestsObserved();

    static const unsigned long STRREF_SORT = 42566;
    static const unsigned long STRREF_COMPLETED_QUESTS = 32177;
    static const unsigned long STRREF_ACTIVE_QUESTS = 32178;
    static const unsigned long STRREF_SORT_BY_DATE = 32173;
    static const unsigned long STRREF_SORT_BY_NAME = 32174;
    static const unsigned long STRREF_SORT_BY_PRIORITY = 32175;
    static const unsigned long STRREF_SORT_BY_PLANET = 32176;

    static unsigned long s_pnSortLabels[4];

    CSWGuiLabel m_lblTitle;
    CSWGuiListBox m_lblItemDescription;
    CSWGuiLabel m_lblDescription;
#ifdef _XBOX
    CSWGuiLabel m_lblImage;
#endif
    CSWGuiListBox m_lbxItems;
#ifdef _XBOX
    CSWGuiLabel m_lblSortButton;
    CSWGuiLabel m_lblSort;
    CSWGuiLabel m_lblSwapText;
    CSWGuiLabel m_lblSwapButton;
    CSWGuiLabel m_lblStart;
    CSWGuiLabel m_lblExit;
    CSWGuiLabel m_lblXButton;
    CSWGuiLabel m_lblQuestItems;
    CSWGuiLabel m_lblRAnalog;
#else
    CSWGuiButton m_btnSwapText;
    CSWGuiButton m_btnExit;
    CSWGuiButton m_btnFilterTime;
    CSWGuiButton m_btnFilterName;
    CSWGuiButton m_btnFilterPlanet;
    CSWGuiButton m_btnMessages;
#endif
    CSWGuiLabel m_alblBar[5];
    CSWGuiQuestItem* m_pQuestItemPanel;
    CSWGuiControl* m_pUnknownFB8;
    CExoArrayList<int> m_aNewQuests;
};

extern int g_nJournalSortType;
extern int g_bOverrideJournalSortType;

class CSWGuiControl;
class CSWGuiManager;
class CSWGuiInGameItemEntry;
#include "exobase.h"
#include "swgui.h"
#include "swguicontrol.h"

class CSWGuiQuestItem : public CSWGuiPanel
{
public:
    CSWGuiQuestItem(CSWGuiManager* pManager);
    virtual ~CSWGuiQuestItem();

    virtual void Draw(float fElapsed);

    virtual void HandleInputEvent(int nEvent, int nState);
    virtual void OnPanelAdded();

    void PopulateItemListBox();
    void OnControlEntered(CSWGuiControl* pControl);

    CExoLinkedList<CSWGuiInGameItemEntry> m_lstItemEntries;
    CSWGuiListBox m_lbxDescription;
    CSWGuiLabel m_lblDescription;
#ifdef _XBOX
    CSWGuiLabel m_lblB;
    CSWGuiLabel m_lblBack;
#endif
    CSWGuiListBox m_lbxItems;
    CSWGuiLabel m_lblTitle;
#ifdef _XBOX
    CSWGuiLabel m_lblRAnalog;
#else
    CSWGuiButton m_btnBack;
#endif

    CSWGuiControl* m_pSelectedItemControl;
};

class CExoStreamingSoundSource;
class CSWGuiManager;
#include "exobase.h"
#include "swgui.h"

class CSWGuiInGameCredits : public CSWGuiPanel
{
public:
#ifdef _XBOX
    CSWGuiInGameCredits(CSWGuiManager* pManager);
#else
    CSWGuiInGameCredits(CSWGuiManager* pManager, const CExoString& sMusic);
#endif
    virtual ~CSWGuiInGameCredits();
    virtual void Draw(float fElapsed);
    virtual void HandleInputEvent(int nEvent, int nState);
    virtual void OnPanelAdded();

    void SetText(unsigned long nStrRef, int bFading);
    void UpdateScrolling(float fElapsed);
    void Initialize();
    void SetNextText();
    void UpdateFading(float fElapsed);

    CSWGuiListBox m_lbxCredits;
    CSWGuiLabel m_lblCredit;
#ifndef _XBOX
    CSWGuiLabel m_lblExit;
    CExoString m_sMusic;
    CExoStreamingSoundSource* m_pMusic;
#endif
    unsigned long* m_pText;
    float m_fScrollTime;
    float m_fFadeTime;
    int m_nPreviousInputClass;
    unsigned char m_nCurrentText;
    unsigned char m_aUnmapped5E1[3];
    int m_bFading;
    int m_bFadeIn;
    unsigned char m_nTextCount;
    unsigned char m_aUnmapped5ED[3];
    float m_fScrollInterval;
    unsigned char m_aUnmapped5F4[4];
};

#ifdef _XBOX

#include "swguiingameequip.h"

class CSWGuiInGameInventory : public CSWGuiPanel
{
public:

    enum { STRREF_ITEM_USED_THIS_ROUND = 42409 };

    enum { NUM_ITEM_ROWS = 5 };

    struct ItemListEntry
    {
        unsigned long m_oidItem;
        CSWGuiHandler m_pfnSelect;
        unsigned long m_bUsable : 1;
        unsigned long m_bEquipped : 1;
        unsigned long m_bNewItem : 1;
    };

    CSWGuiInGameInventory(CSWGuiManager* pManager);
    virtual ~CSWGuiInGameInventory();
    virtual void Draw(float fElapsed);
    virtual void HandleInputEvent(int nEvent, int nState);
    virtual void OnPanelAdded();
    virtual void OnPanelRemoved();

    int CheckFilter(CSWSItem* pItem, CSWSCreature* pCreature, int nSlot);
    void PopulateItemListBox();
    void SetNextFilter();
    void UpdatePlayerStats();
    void OnControlSelected(CSWGuiControl* pControl);
    void OnControlEntered(CSWGuiControl* pControl);
    void CreateItemEntry(CSWCCreature* pCreature, CSWSItem* pItem, ItemListEntry* pEntry,
                        int bSquadItem, int bEquipped);
    void CantEquip(CSWGuiControl* pControl);
    void NotUseable(CSWGuiControl* pControl);
    void FullHealth(CSWGuiControl* pControl);

    void SetItemRows(int nTopItem, int nSelectedItem);
    void SelectNextItem();
    void SelectPreviousItem();

    static const int m_nFilterName[6];

    CSWGuiLabel m_lblUnknown64;
    CSWGuiLabel m_lblA;
    CSWGuiLabel m_lblX;
    CSWGuiLabel m_lblStart;
    CSWGuiLabel m_lblExit;
    CSWGuiLabel m_lblBlack;
    CSWGuiLabel m_lblUseItem;
    CSWGuiLabel m_lblFilter;
    CSWGuiLabel m_lblInv;
    CSWGuiLabel m_lblCredits;
    CSWGuiLabel m_lblCreditsValue;
    CSWGuiLabel m_lblRanalog;
    CSWGuiListBox m_lbxDescription;
    CSWGuiLabel m_lblBgPort;
    CSWGuiLabel m_lblBgStats;
    CSWGuiLabel m_lblPort;
    CSWGuiLabel m_lblVit;
    CSWGuiLabel m_lblDef;
    CSWGuiLabel m_lblDownarrow;
    CSWGuiLabel m_lblUparrow;
    CSWGuiInGameItemEntry m_aItemRows[NUM_ITEM_ROWS];
    CExoArrayList<unsigned long> m_aNewItems;
    CExoArrayList<ItemListEntry> m_lItems;
    unsigned long m_oidSelectedItem;
    short m_nTopItem;
    short m_nSelectedItem;
    unsigned long m_bUpdateItems : 1;
};
#endif

#endif
