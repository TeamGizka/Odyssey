#ifndef __SWGUIUPGRADE_H__
#define __SWGUIUPGRADE_H__

#include "swguidrawable.h"
#include "swguicontrol.h"
#include "swgui.h"
#ifdef _XBOX
#include "swguiingameequip.h"
#endif
class C2DA;
class CSWGuiControl;
class CSWGuiExtent;
class CSWGuiManager;
class CSWGuiText;
class CSWSCreature;
class CSWSItem;
class CSWGuiText_vtbl;
class CSWGuiUpgradeItemSelect;
class CSWGuiUpgradeSelection;
class CSWUpgradeItemEntry;
class CSWGuiCreateItem;
class CSWGuiStoreItemEntry;
class CSWSStore;

#define UPGRADE_TABLE_TYPE_POWER_CRYSTAL 0
#define UPGRADE_TABLE_TYPE_LS_LENS 1
#define UPGRADE_TABLE_TYPE_TARGETING 4
#define UPGRADE_TABLE_TYPE_MELEE_GRIP 7
#define UPGRADE_TABLE_TYPE_ARMOR_OVERLAY 10

class CSWGuiUpgrade : public CSWGuiPanel
{
public:

    static const unsigned long STRREF_NO_DESCRIPTION = 32172;

    static const unsigned long STRREF_UPGRADE_INCOMPATIBLE = 116713;

    static const unsigned long STRREF_UPGRADES = 42026;

    static const unsigned long STRREF_EQUIP = 31387;

    static const unsigned long STRREF_UNEQUIP = 36840;

    static const unsigned long STRREF_ASSEMBLE = 42021;

    static const unsigned long STRREF_UPGRADE_PREVENTS_REEQUIP = 42489;

    enum UpgradeSlot
    {
        UPGRADE_SLOT_POWER_CRYSTAL_1 = 0,
        UPGRADE_SLOT_COLOR_CRYSTAL = 1,
        UPGRADE_SLOT_POWER_CRYSTAL_2 = 2,
        UPGRADE_SLOT_LS_LENS = 3
    };

    enum ItemListMode
    {
        ITEMLIST_LIGHTSABER = 1,
        ITEMLIST_BLASTER = 2,
        ITEMLIST_MELEE = 3,
        ITEMLIST_ARMOR = 4
    };

    enum
    {
        NUM_ITEMLIST_MODES = 4,
        NUM_UPGRADE_SLOTS = 6
    };

    enum
    {
        ITEM_TYPE_ARMOR_HEAVY = 36,
        ITEM_TYPE_CLOTHING = 37,
        ITEM_TYPE_ROBE = 38
    };

    CSWGuiButton m_aUpgradeButtons4[4];
    CSWGuiButton m_aUpgradeButtons3Ls[6];
    CSWGuiButton m_aUpgradeButtons3[3];
    CSWGuiLabel m_aUpgradeLabels3Ls[6];
    CSWGuiLabel m_aUpgradeLabels3[3];
    CSWGuiListBox m_lbItems;
    CSWGuiListBox m_lbDesc;
    CSWGuiListBox m_lbDescLs;
    CSWGuiLabel m_cTitle;
    CSWGuiLabel m_lblTitle2;
    CSWGuiLabel m_cDescription;
    CSWGuiLabel m_cLightsaberDescription;
#ifndef _XBOX
    CSWGuiLabel m_cDescriptionBackground;
    CSWGuiLabel m_cLightsaberDescriptionBackground;
#endif
    CSWGuiLabel m_cProperty;
    CSWGuiLabel m_cSlotName;
#ifndef _XBOX
    CSWGuiButton m_cAssemble;
#endif
    CSWGuiButton m_btnOk;
    CSWGui3DSceneView m_3dModel;
    CSWGui3DSceneView m_3dModelLs;
#ifndef _XBOX
    CSWGuiButton m_btnBack;
#else
    CSWGuiLabel m_lblA;
    CSWGuiLabel m_lblB;
    CSWGuiLabel m_lblAAction;
    CSWGuiLabel m_lblBack;
    CSWGuiLabel m_lblRanalog;
    CSWGuiLabel m_alblBar[3];
    CSWGuiLabel m_lblXAssemble;
    CSWGuiLabel m_lblX;
#endif
    unsigned long m_bItemListShown : 1;
    unsigned long m_nFlagsUnused : 31;
    unsigned char m_nItemListMode;
    int m_nPreviousItemListMode;

    CSWSItem* m_pSelectedItem;

    CSWSItem* m_pSelectedItemCopy;

    CExoArrayList<CSWSItem*> m_aItemsTaken;
    CExoArrayList<CSWSItem*> m_aItemsAdded;

    CSWSItem* m_apItemEntries[NUM_UPGRADE_SLOTS];
    CExoString m_asUpgradeTemplates[NUM_UPGRADE_SLOTS];
    int m_anUpgradeProperties[NUM_UPGRADE_SLOTS];
    C2DA* m_pUpgradeTable;
    C2DA* m_pCrystalTable;
    CSWGuiControl* m_pItemListControl;
    CSWGuiUpgradeItemSelect* m_pItemSelect;
#ifdef _XBOX
    int m_nReserved;
#endif
    virtual void OnPanelRemoved();
    void ShowItems(int);
    void SetDescription(CExoString const&);
    virtual void Draw(float);
    void GetArmorModelAndTextureResRef(int, int, int, CExoString&, CExoString&);
    void Update3DView();
    void UpdateItems();
    void OnEnterSlot(CSWGuiControl*);
    ~CSWGuiUpgrade();
#ifndef _XBOX
    unsigned char GetNumUpgrades();
#endif
    void CloseItems();
    virtual void OnPanelAdded();
    void OnControlEntered(CSWGuiControl*);
    void OnUpgradeSelected(CSWGuiControl*);
    CSWUpgradeItemEntry* CreateItemEntry(unsigned long, int, int bDisabled);
    void InsertUpgrade(CSWSItem*, int);
    void VerifyUpgrade(CSWGuiControl*);
    void OnAssemble(CSWGuiControl*);
    void OnDisabledSelected(CSWGuiControl* pControl);
    void CancelPanel();
    void OnSlotSelected(CSWGuiControl*);
    virtual void HandleInputEvent(int, int);
    CSWGuiUpgrade(CSWGuiManager*);

    struct SlotInfo
    {
        int nProperty;
        char* pszIcon;
        unsigned long nStrRef;
    };
    static const SlotInfo SLOT_INFO[NUM_ITEMLIST_MODES][NUM_UPGRADE_SLOTS];
};

class CSWGuiSwoopUpgrade : public CSWGuiPanel
{
public:

    enum
    {
        NUM_SWOOP_SLOTS = 6,
        NUM_SWOOP_UPGRADE_SLOTS = 3
    };

    CSWGuiButton m_aUpgradeButtons4[4];
    CSWGuiButton m_aUpgradeButtons3[NUM_SWOOP_SLOTS];
    CSWGuiLabel m_aUpgradeLabels4[4];
    CSWGuiLabel m_aUpgradeLabels3[NUM_SWOOP_SLOTS];
    CSWGuiListBox m_lbItems;
    CSWGuiListBox m_lbDesc;
    CSWGuiListBox m_lbDescLs;
    CSWGuiLabel m_cTitle;
    CSWGuiLabel m_cDescription;
    CSWGuiLabel m_cLightsaberDescription;
    CSWGuiLabel m_cDescriptionBackground;
    CSWGuiLabel m_cLightsaberDescriptionBackground;
    CSWGuiLabel m_cProperty;
    CSWGuiLabel m_cSlotName;
    CSWGuiLabel m_cLightsaberSlotName;
    CSWGuiLabel m_cUpgrades;
    CSWGuiLabel m_cUpgradeCount;
    CSWGuiLabel m_lblA;
    CSWGuiLabel m_lblB;
    CSWGuiLabel m_lblAAction;
    CSWGuiLabel m_lblBack;
    CSWGuiLabel m_lblRanalog;
    CSWGuiLabel m_lblRanalogLs;
    CSWGuiButton m_cAssemble;
#ifndef _XBOX
    CSWGuiButton m_btnOk;
    CSWGuiButton m_btnBack;
#endif
    CSWGui3DSceneView m_3dModel;
    CSWGui3DSceneView m_3dModelLs;
    unsigned long m_bItemListShown : 1;
    unsigned long m_nFlagsUnused : 31;
    unsigned char m_nItemListMode;
    int m_nPreviousItemListMode;
    CSWSItem* m_pSelectedItem;
    CSWSItem* m_pSelectedItemCopy;
    CExoArrayList<CSWSItem*> m_aItemsTaken;
    CExoArrayList<CSWSItem*> m_aItemsAdded;
    CSWSItem* m_apItemEntries[NUM_SWOOP_SLOTS];
    CExoString m_asUpgradeTemplates[NUM_SWOOP_SLOTS];
    int m_anUpgradeProperties[NUM_SWOOP_SLOTS];
    int m_anUpgradeRows[NUM_SWOOP_SLOTS];
    C2DA* m_pUpgradeTableEx;
    C2DA* m_pCrystalTable;
    C2DA* m_pUpgradeTable;
    CSWGuiControl* m_pItemListControl;
    int m_nItemListSelection;

    void ShowItems(int);
    virtual void OnPanelRemoved();
    void UpdateItems();
    void OnEnterSlot(CSWGuiControl*);
    void OnEnterAssemble(CSWGuiControl*);
    virtual void HandleInputEvent(int, int);
    void OnAssemble(CSWGuiControl*);
    ~CSWGuiSwoopUpgrade();
    virtual void OnPanelAdded();
    void SetDescription(CExoString const&);
    virtual void Draw(float);
    void CancelPanel();
    void OnControlEntered(CSWGuiControl*);
    void OnUpgradeSelected(CSWGuiControl*);
    void CloseItems(CSWGuiControl*);
    CSWUpgradeItemEntry* CreateItemEntry(unsigned long, int);
    void OnSlotSelected(CSWGuiControl*);
    CSWGuiSwoopUpgrade(CSWGuiManager*);

    static const CSWGuiUpgrade::SlotInfo SWOOP_SLOT_INFO[NUM_SWOOP_SLOTS];
};

class CSWGuiUpgradeItemSelect : public CSWGuiPanel
{
public:

    CSWGuiListBox m_lbItems;
    CSWGuiListBox m_lbDescription;

    CSWGuiLabel m_lblTitle;

    CSWGuiLabel m_lblDescription;
#ifdef _XBOX
    CSWGuiLabel m_lblA;
    CSWGuiLabel m_lblB;
    CSWGuiLabel m_lblUpgradeitem;
    CSWGuiLabel m_lblBack;
    CSWGuiLabel m_lblRanalog;
    CSWGuiLabel m_lblX;
    CSWGuiLabel m_lblCreateItem;
#else

    CSWGuiButton m_btnUpgradeitem;

    CSWGuiButton m_btnBack;
#endif

    unsigned char m_nUpgradeType;

    unsigned long m_bDualWield : 1;
    unsigned long m_bLeftHandItem : 1;
    unsigned long m_bSecondWeaponSet : 1;
    unsigned long m_oidOtherWeapon;

    int m_bExitAfterUpgrade;
    unsigned long m_nPresetItemCount;

    CSWSItem* m_pPresetItem;
    CSWUpgradeItemEntry* m_pUpgradeItem;

    CSWGuiUpgrade* m_pUpgradePanel;

    CSWGuiUpgradeSelection* m_pSelectionPanel;
    void SetDescription(CExoString const&);
    virtual void HandleInputEvent(int, int);
    void OnUpgradableSelected(CSWGuiControl*);
    inline unsigned long GetUpgradeItemId();
    inline unsigned long GetUpgradeItemProperty();
    int GetUpgradeItemEquipped();
    ~CSWGuiUpgradeItemSelect();
    void OnControlEntered(CSWGuiControl*);
    void BuildItemList(CSWSCreature*, CExoArrayList<CSWGuiControl*>&, C2DA*);
    void BuildUpgradeList();
    void FinishUpgrading();
    virtual void OnPanelAdded();
    CSWGuiUpgradeItemSelect(CSWGuiManager*);
};

class CSWGuiUpgradeSelection : public CSWGuiPanel
{
public:
    CSWGuiLabel m_lblTitle;
    CSWGuiLabel m_lblTitle2;
#ifdef _XBOX
    CSWGuiLabel m_lblA;
    CSWGuiLabel m_lblUpgradeItems;
    CSWGuiLabel m_lblB;
    CSWGuiLabel m_lblBack;
    CSWGuiLabel m_lblY;
    CSWGuiLabel m_lblX;
    CSWGuiLabel m_lblCreateItem;
    CSWGuiLabel m_lblRanalog;
    CSWGuiLabel m_alblBar[3];
#else
    CSWGuiButton m_btnUpgradeitems;
    CSWGuiButton m_btnBack;
    CSWGuiButton m_btnCreateitems;
#endif
    CSWGuiCreateItem* m_pCreateItems;
    CSWGuiButton m_aTypeButtons[5];
    CSWGuiListBox m_lbUpgradelist;
    CSWGuiListBox m_lbDescription;
    CSWGuiLabel m_lblDescription;
    unsigned long m_oidItem;
    unsigned long m_oidCharacter;
    int m_anUpgradable[4];
    int m_nFilter;
    CSWGuiUpgradeItemSelect* m_pItemSelect;
#ifndef _XBOX
    CSWGuiControl* m_pSelectedType;
#endif
    int m_bDisableItemCreation;
    int m_bDisableUpgrade;
    CExoString m_sOverride2DA;
    void SetItem(unsigned long oidItem) { m_oidItem = oidItem; }
    void SetCharacter(unsigned long oidCharacter) { m_oidCharacter = oidCharacter; }
    void SetDisableItemCreation(int bDisableItemCreation) { m_bDisableItemCreation = bDisableItemCreation; }
    void SetDisableUpgrade(int bDisableUpgrade) { m_bDisableUpgrade = bDisableUpgrade; }
    // PC 0x0067CB20 GOG 0x0067C8F0 Xbox 0x00055140
    void SetOverride2DA(CExoString sOverride2DA)
    {
        m_sOverride2DA = sOverride2DA;
    }
    void AutoUpgradeDone();
    virtual void HandleInputEvent(int, int);
#ifdef _XBOX
    void OnCreateItem();
    void NextFilter();
#else
    void OnCreateItem(CSWGuiControl*);
#endif
    void UpdateFilterButtons();
    void SetDescription(CExoString);
    void OnItemSelected(CSWGuiControl*);
    ~CSWGuiUpgradeSelection();
    void OnItemEntered(CSWGuiControl*);
    void GetPlayerItems(CSWSCreature*, CExoArrayList<CSWGuiControl*>&, C2DA*);
    void PopulateItemList();
    virtual void OnPanelAdded();
    void OnFilterPicked(CSWGuiControl*);
    CSWGuiUpgradeSelection(CSWGuiManager*, unsigned long oidCharacter);
};

#ifdef _XBOX
#define CREATEITEM_NUM_ITEM_SLOTS 8
#define CREATEITEM_SLOT_TEXT_ALIGNMENT 0x11

struct CItemSlotData
{
    unsigned long m_oidItem;
    CSWGuiHandler m_pfnHandler;
};
#endif

class CSWGuiCreateItem : public CSWGuiPanel
{
public:
    enum
    {
        STRREF_INFINITE = 41951,
        STRREF_SKILL_REQUIRED_TO_CREATE = 115429,
        STRREF_SKILL_TOO_LOW = 116292,
        STRREF_NOT_ENOUGH_COMPONENTS = 116297,
        STRREF_CONFIRM_CREATE_COST = 124113,
        STRREF_CONFIRM_BREAKDOWN_VALUE = 124115,
        STRREF_REMOVE_UPGRADES_FIRST = 124119,
        CONFIRM_VALUE_PER_LEVEL = 50,
        MAX_BREAKDOWN_CONFIRM_VALUE = 250
    };

    static const unsigned long COMPONENT_VALUE_DIVISOR = 20;

    int m_nPendingAction;
    CExoArrayList<CSWGuiStoreItemEntry*> m_lStoreEntries;
    CExoArrayList<CSWGuiStoreItemEntry*> m_lInventoryEntries;
    CSWGuiLabel m_lblA;
    CSWGuiLabel m_lblAccept;
    CSWGuiLabel m_lblB;
    CSWGuiLabel m_lblCancel;
#ifdef _XBOX
    CSWGuiLabel m_lblUnused;
#else
    CSWGuiLabel m_lblY;
#endif
    CSWGuiLabel m_lblExamine;
    CSWGuiLabel m_lblX;
    CSWGuiLabel m_lblDescription;
    CSWGuiLabel m_lblCost;
    CSWGuiLabel m_lblCostValue;
    CSWGuiLabel m_lblStock;
    CSWGuiLabel m_lblStockValue;
#ifdef _XBOX
    CSWGuiLabel m_alblBars[6];
    CSWGuiLabel m_alblCreateFilters[5];
    CSWGuiLabel m_alblBreakdownFilters[5];
    CSWGuiLabel m_lblY;
#else
    CSWGuiButton m_btnAccept;
    CSWGuiButton m_btnCancel;
    CSWGuiButton m_btnExamine;
    CSWGuiButton m_abtnCreateFilters[5];
    CSWGuiButton m_abtnBreakdownFilters[5];
    CSWGuiLabel m_alblBars[3];
#endif
    CSWGuiLabel m_lblTitle;
    CSWGuiLabel m_lblTitle2;
    CSWGuiLabel m_lblRelatedSkill;
    CSWGuiListBox m_lbxDescriptionInv;
    CSWGuiLabel m_lblRanalog;
    CSWGuiLabel m_lblCredits;
    CSWGuiLabel m_lblCreditsValue;
    CSWGuiListBox m_lbxStoreItems;
#ifndef _XBOX
    CSWGuiListBox m_lbxInventoryItems;
#endif
    CSWGuiListBox m_lbxDescription;
    int m_nItemCount;
    int m_nPreviousComponents;
    int m_nComponents;
    int m_nSelectedEntry;
    unsigned long m_nStoreId;
    unsigned long m_nCreatureId;
    float m_fRepairFactor;
    int m_bAlternateList;
    CSWSStore* m_pStore;
    int m_nCreateFilter;
    int m_bBuyMode;
    int m_nBreakdownFilter;
    CSWGuiUpgradeSelection* m_pUpgradeSelection;
    CExoString m_sCreateTable;
#ifdef _XBOX
    CExoArrayList<CItemSlotData> m_lItemSlots;
    CSWGuiStoreItemEntry m_aItemButtons[CREATEITEM_NUM_ITEM_SLOTS];
    int m_nTopItem;
    int m_nSelectedItem;
    int m_nPreviousSelectedItem;
#endif
    void SetCreateTable(CExoString sTable);

    void PopulateStore();
    int CheckBreakdownFilter(CSWSItem*, CSWSCreature*, int);
    void CloseCreateItem(CSWGuiControl*);
    void UpdateBreakdownFilters();
    void UpdateCreateFilters();
    unsigned long GetItemBuyValue(CSWSItem*);
    virtual void HandleInputEvent(int, int);
    void BuyItem(CSWSItem*);
    void BuyItemCallback(CSWGuiControl*);
    void SetDescription(const CExoString&);
    void PopulateInventoryItemListBox();
    void OnBreakdownFilterPicked(CSWGuiControl*);
    void OnControlStoreAButton(CSWGuiControl*);
    void OnControlEntered(CSWGuiControl*);
    void SellItem(CSWSItem*);
    void SellItemCallback(CSWGuiControl*);
    void ShowSellGUI();
    void OnControlInvAButton(CSWGuiControl*);
    void DeleteItemEntries();
    void OnAcceptPicked(CSWGuiControl*);
    ~CSWGuiCreateItem();
    virtual void OnPanelRemoved();
    int CreateItemEntry(int&, CExoArrayList<CSWGuiControl*>&, unsigned long,
                        int, CExoString);
    void PopulateStoreItemListBox();
    void ShowBuyGUI();
    void SetItemList();
    void OnCreateFilterPicked(CSWGuiControl*);
    void OnControlXButton(CSWGuiControl*);
    void InitializeStoreType();
#ifdef _XBOX
    void UpdateItemSlots(int nTopItem, int nSelectedItem);
#endif
    CSWGuiCreateItem(CSWGuiManager*, unsigned long);
    virtual void OnPanelAdded();
};

class CSWGuiCreateMedicalItem : public CSWGuiPanel
{
public:
    enum
    {
        STRREF_INFINITE = 41951,
        STRREF_SKILL_REQUIRED_TO_CREATE = 115429,
        STRREF_SKILL_TOO_LOW = 116292,
        STRREF_NOT_ENOUGH_CHEMICALS = 116298,
        STRREF_CONFIRM_CREATE_COST = 124114,
        STRREF_CONFIRM_BREAKDOWN_VALUE = 124116,
        CONFIRM_VALUE_PER_LEVEL = 50,
        MAX_BREAKDOWN_CONFIRM_VALUE = 250
    };

    static const unsigned long CHEMICAL_VALUE_DIVISOR = 2;

    int m_nPendingAction;
    CExoArrayList<CSWGuiStoreItemEntry*> m_lStoreEntries;
    CExoArrayList<CSWGuiStoreItemEntry*> m_lInventoryEntries;
    CSWGuiLabel m_lblA;
    CSWGuiLabel m_lblAccept;
    CSWGuiLabel m_lblB;
    CSWGuiLabel m_lblCancel;
#ifdef _XBOX
    CSWGuiLabel m_lblUnused;
#else
    CSWGuiLabel m_lblY;
#endif
    CSWGuiLabel m_lblExamine;
    CSWGuiLabel m_lblX;
    CSWGuiLabel m_lblDescription;
    CSWGuiLabel m_lblTitle;
    CSWGuiLabel m_lblTitle2;
    CSWGuiLabel m_lblRelatedSkill;
    CSWGuiListBox m_lbxDescriptionInv;
#ifdef _XBOX
    CSWGuiLabel m_alblBars[6];
    CSWGuiLabel m_alblCreateFilters[4];
    CSWGuiLabel m_lblY;
#else
    CSWGuiButton m_btnAccept;
    CSWGuiButton m_btnCancel;
    CSWGuiButton m_btnExamine;
    CSWGuiLabel m_alblBars[3];
    CSWGuiButton m_abtnCreateFilters[4];
#endif
    CSWGuiLabel m_lblCost;
    CSWGuiLabel m_lblCostValue;
    CSWGuiLabel m_lblStock;
    CSWGuiLabel m_lblStockValue;
    CSWGuiLabel m_lblRanalog;
    CSWGuiLabel m_lblCredits;
    CSWGuiLabel m_lblCreditsValue;
    CSWGuiListBox m_lbxStoreItems;
    CSWGuiListBox m_lbxInventoryItems;
    CSWGuiListBox m_lbxDescription;
    int m_nItemCount;
    int m_nPreviousChemicals;
    int m_nChemicals;
    int m_nSelectedEntry;
    unsigned long m_nStoreId;
    unsigned long m_nCreatureId;
    float m_fRepairFactor;
    int m_bAlternateList;
    unsigned long m_oidCreature;
    CSWSStore* m_pStore;
    int m_nCreateFilter;
    int m_bBuyMode;
    int m_nBreakdownFilter;

    void PopulateStore();
    void CloseChemical(CSWGuiControl*);
    void UpdateCreateFilters();
    unsigned long GetItemBuyValue(CSWSItem*);
    virtual void HandleInputEvent(int, int);
    void BuyItem(CSWSItem*);
    void BuyItemCallback(CSWGuiControl*);
    void SetDescription(const CExoString&);
    void PopulateInventoryItemListBox();
    void OnControlStoreAButton(CSWGuiControl*);
    void OnControlEntered(CSWGuiControl*);
    void SellItem(CSWSItem*);
    void SellItemCallback(CSWGuiControl*);
    void ShowSellGUI();
    void OnControlInvAButton(CSWGuiControl*);
    void DeleteItemEntries();
    void OnAcceptPicked(CSWGuiControl*);
    ~CSWGuiCreateMedicalItem();
    virtual void OnPanelRemoved();
    int CreateItemEntry(int&, CExoArrayList<CSWGuiControl*>&, unsigned long,
                        int, CExoString);
    void PopulateStoreItemListBox();
    void ShowBuyGUI();
    void SetItemList();
    void OnCreateFilterPicked(CSWGuiControl*);
    void OnControlXButton(CSWGuiControl*);
    void InitializeStoreType();
    CSWGuiCreateMedicalItem(CSWGuiManager*, unsigned long);
    virtual void OnPanelAdded();
};

class CSWUpgradeItemEntry : public CSWGuiButton
{
public:

    static const int ENTRY_HEIGHT = 36;
    static const int COUNT_HEIGHT = 19;

    static const unsigned long STRREF_EQUIPPED = 32346;
    unsigned long m_oidItem;
    unsigned long m_nItemId;
    unsigned long m_nItemProperty;
    unsigned int m_bShowItemSuffix : 1;
    unsigned int m_bDisabled : 1;
    int m_nUpgradeIndex;
    CSWGuiBorder m_cIconBorder;
    CSWGuiBorder m_cIconHilite;
    CSWGuiBorder m_cIcon;
    CSWGuiText m_cCountText;
    virtual void Initialize(CSWGuiText&, CSWGuiBorder&, CSWGuiBorder&, int, int);
    virtual void Initialize(const CSWGuiExtent&, CSWGuiButton*)  // PC 0x006049D0 GOG 0x006049B0 Xbox 0x000FFC10
    {
    }
    virtual void Initialize(const CSWGuiExtent&, const CSWGuiTextParams&,  // PC 0x00693D50 GOG 0x00693AC0 Xbox 0x000FFC00
                            const CSWGuiBorderParams&, const CSWGuiBorderParams&)
    {
    }
    virtual void SetExtent(CSWGuiExtent const&);
    virtual void Draw(float);
    CSWUpgradeItemEntry();
    void SetItem(unsigned long, int, unsigned long, unsigned long);
};

#endif
