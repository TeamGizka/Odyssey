#ifndef __SWGUIUPGRADE_H__
#define __SWGUIUPGRADE_H__

#include "swguidrawable.h"
#include "swguicontrol.h"
#include "swgui.h"
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

class CSWGuiUpgrade : public CSWGuiPanel
{
public:

    static const unsigned long STRREF_NO_DESCRIPTION = 32172;

    static const unsigned long STRREF_UPGRADES = 42026;

    static const unsigned long STRREF_UPGRADE_PREVENTS_REEQUIP = 42489;

    enum UpgradeSlot
    {
        UPGRADE_SLOT_POWER_CRYSTAL_1 = 0,
        UPGRADE_SLOT_COLOR_CRYSTAL = 1,
        UPGRADE_SLOT_POWER_CRYSTAL_2 = 2
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

    CSWGuiButton m_aUpgradeButtons4[4];
#ifndef _XBOX
    CSWGuiButton m_aUpgradeButtons3Ls[6];
#endif
    CSWGuiButton m_aUpgradeButtons3[3];
#ifdef _XBOX
    CSWGuiLabel m_aUpgradeLabels4[4];
#else
    CSWGuiLabel m_aUpgradeLabels3Ls[6];
#endif
    CSWGuiLabel m_aUpgradeLabels3[3];
    CSWGuiListBox m_lbItems;
    CSWGuiListBox m_lbDesc;
    CSWGuiListBox m_lbDescLs;
    CSWGuiLabel m_cTitle;
#ifndef _XBOX
    CSWGuiLabel m_lblTitle2;
#endif
    CSWGuiLabel m_CSWGuiLabel_1F60;
    CSWGuiLabel m_cLabel20A0;
    CSWGuiLabel m_cDescriptionBackground;
    CSWGuiLabel m_cLightsaberDescriptionBackground;
    CSWGuiLabel m_cProperty;
    CSWGuiLabel m_cSlotName;
#ifdef _XBOX
    CSWGuiLabel m_cLightsaberSlotName;
    CSWGuiLabel m_cUpgrades;
    CSWGuiLabel m_cUpgradeCount;
    CSWGuiLabel m_lblA;
    CSWGuiLabel m_lblB;
    CSWGuiLabel m_lblAAction;
    CSWGuiLabel m_lblBack;
    CSWGuiLabel m_lblRanalog;
    CSWGuiLabel m_lblRanalogLs;
#endif
    CSWGuiButton m_cAssemble;
#ifndef _XBOX
    CSWGuiButton m_btnOk;
#endif
    CSWGui3DSceneView m_3dModel;
    CSWGui3DSceneView m_3dModelLs;
#ifndef _XBOX
    CSWGuiButton m_btnBack;
#endif
    unsigned long m_bItemListShown : 1;
    unsigned long m_nPanelFlagsUnused : 31;
    unsigned char m_nItemListMode;
    char gap_2F4D[7];

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
    virtual void OnPanelRemoved();
    void ShowItems(int);
    void SetDescription(CExoString const&);
    virtual void Draw(float);
    void GetArmorModelAndTextureResRef(int, int, int, CExoString&, CExoString&);
    void Update3DView();
    void UpdateItems();
    void OnEnterSlot(CSWGuiControl*);
    ~CSWGuiUpgrade();
    void CloseItems();
    virtual void OnPanelAdded();
    void OnControlEntered(CSWGuiControl*);
    void OnUpgradeSelected(CSWGuiControl*);
    CSWUpgradeItemEntry* CreateItemEntry(unsigned long, int);
    void InsertUpgrade(CSWSItem*, int);
    void VerifyUpgrade(CSWGuiControl*);
    void OnAssemble(CSWGuiControl*);
#ifdef _XBOX
    void OnEnterAssemble(CSWGuiControl*);
#endif
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
#else

    CSWGuiButton m_btnUpgradeitem;

    CSWGuiButton m_btnBack;
#endif

    unsigned char m_nUpgradeType;
    char gap_C2D[3];

    unsigned long m_bDualWield : 1;
    unsigned long m_bLeftHandItem : 1;
    unsigned long m_oidOtherWeapon;

    int m_nC84;
    unsigned long m_nC88;

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
    CSWGuiButton m_btnUpgradeitems;
    CSWGuiButton m_btnBack;
    CSWGuiButton m_btnCreateitems;
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
    CSWGuiControl* m_pSelectedType;
    int m_bDisableItemCreation;
    int m_bDisableUpgrade;
    CExoString m_sOverride2DA;
    // PC 0x0067CB20
    void SetOverride2DA(CExoString sOverride2DA)
    {
        m_sOverride2DA = sOverride2DA;
    }
    void AutoUpgradeDone();
    virtual void HandleInputEvent(int, int);
    void OnCreateItems(CSWGuiControl*);
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

class CSWGuiCreateItem : public CSWGuiPanel
{
public:
    enum { STRREF_INFINITE = 41951 };

    int m_n68;
    CExoArrayList<CSWGuiStoreItemEntry*> m_lStoreEntries;
    CExoArrayList<CSWGuiStoreItemEntry*> m_lInventoryEntries;
    CSWGuiLabel m_lbl84;
    CSWGuiLabel m_lbl1CC;
    CSWGuiLabel m_lbl314;
    CSWGuiLabel m_lbl45C;
    CSWGuiLabel m_lbl5A4;
    CSWGuiLabel m_lbl6EC;
    CSWGuiLabel m_lbl834;
    CSWGuiLabel m_lblDescription;
    CSWGuiLabel m_lblCost;
    CSWGuiLabel m_lblCostValue;
    CSWGuiLabel m_lblStock;
    CSWGuiLabel m_lblStockValue;
    CSWGuiButton m_btnAccept;
    CSWGuiButton m_btnCancel;
    CSWGuiButton m_btnExamine;
    CSWGuiButton m_abtnCreateFilters[5];
    CSWGuiButton m_abtnBreakdownFilters[5];
    CSWGuiLabel m_alblBars[3];
    CSWGuiLabel m_lblTitle;
    CSWGuiLabel m_lblTitle2;
    CSWGuiLabel m_lblRelatedSkill;
    CSWGuiListBox m_lbxDescriptionInv;
    CSWGuiLabel m_lblRanalog;
    CSWGuiLabel m_lblCredits;
    CSWGuiLabel m_lblCreditsValue;
    CSWGuiListBox m_lbxStoreItems;
    CSWGuiListBox m_lbxInventoryItems;
    CSWGuiListBox m_lbxDescription;
    int m_n3EBC;
    int m_n3EC0;
    int m_nComponents;
    int m_n3EC8;
    unsigned long m_nStoreId;
    unsigned long m_nCreatureId;
    float m_fRepairFactor;
    int m_n3ED8;
    CSWSStore* m_pStore;
    int m_nCreateFilter;
    int m_bBuyMode;
    int m_nBreakdownFilter;
    CSWGuiUpgradeSelection* m_pUpgradeSelection;
    CExoString m_s3EF0;
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
    CSWGuiCreateItem(CSWGuiManager*, unsigned long);
    virtual void OnPanelAdded();
};

class CSWGuiCreateMedicalItem : public CSWGuiPanel
{
public:
    enum { STRREF_INFINITE = 41951 };

    int m_n68;
    CExoArrayList<CSWGuiStoreItemEntry*> m_lStoreEntries;
    CExoArrayList<CSWGuiStoreItemEntry*> m_lInventoryEntries;
    CSWGuiLabel m_lbl84;
    CSWGuiLabel m_lbl1CC;
    CSWGuiLabel m_lbl314;
    CSWGuiLabel m_lbl45C;
    CSWGuiLabel m_lbl5A4;
    CSWGuiLabel m_lbl6EC;
    CSWGuiLabel m_lbl834;
    CSWGuiLabel m_lblDescription;
    CSWGuiLabel m_lblTitle;
    CSWGuiLabel m_lblTitle2;
    CSWGuiLabel m_lblRelatedSkill;
    CSWGuiListBox m_lbxDescriptionInv;
    CSWGuiButton m_btnAccept;
    CSWGuiButton m_btnCancel;
    CSWGuiButton m_btnExamine;
    CSWGuiLabel m_alblBars[3];
    CSWGuiButton m_abtnCreateFilters[4];
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
    int m_n33DC;
    int m_n33E0;
    int m_nChemicals;
    int m_n33E8;
    unsigned long m_nStoreId;
    unsigned long m_nCreatureId;
    float m_fRepairFactor;
    int m_n33F8;
    unsigned long m_oidCreature;
    CSWSStore* m_pStore;
    int m_nCreateFilter;
    int m_bBuyMode;
    int m_n340C;

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
    int m_nUpgradeIndex;
    CSWGuiBorder m_CSWGuiBorder_1D8;
    CSWGuiBorder m_CSWGuiBorder_24C;
    CSWGuiBorder m_CSWGuiBorder_2C0;
    CSWGuiText m_CSWGuiText_334;
    virtual void Initialize(CSWGuiText&, CSWGuiBorder&, CSWGuiBorder&, int, int);
    virtual void Initialize(const CSWGuiExtent&, CSWGuiButton*)  // PC 0x006049D0
    {
    }
    virtual void Initialize(const CSWGuiExtent&, const CSWGuiTextParams&,  // PC 0x00693D50
                            const CSWGuiBorderParams&, const CSWGuiBorderParams&)
    {
    }
    virtual void SetExtent(CSWGuiExtent const&);
    virtual void Draw(float);
    CSWUpgradeItemEntry();
    void SetItem(unsigned long, int, unsigned long, unsigned long);
};

inline int CSWGuiUpgradeItemSelect::GetUpgradeItemEquipped()
{
    if (m_pUpgradeItem != 0)
    {
        return m_pUpgradeItem->m_bShowItemSuffix;
    }
    return 0;
}

#endif
