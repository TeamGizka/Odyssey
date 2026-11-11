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
        NUM_UPGRADE_SLOTS = 4
    };

    CSWGuiButton m_aUpgradeButtons4[4];
    CSWGuiButton m_aUpgradeButtons3[3];
    CSWGuiLabel m_aUpgradeLabels4[4];
    CSWGuiLabel m_aUpgradeLabels3[3];
    CSWGuiListBox m_lbItems;
    CSWGuiListBox m_lbDesc;
    CSWGuiListBox m_lbDescLs;
    CSWGuiLabel m_cTitle;
    CSWGuiLabel m_CSWGuiLabel_1F60;
    CSWGuiLabel m_cLabel20A0;
    CSWGuiLabel m_cDescriptionBackground;
    CSWGuiLabel m_cLightsaberDescriptionBackground;
    CSWGuiLabel m_cProperty;
    CSWGuiLabel m_cSlotName;
    CSWGuiLabel m_cLightsaberSlotName;
    CSWGuiLabel m_cUpgrades;
    CSWGuiLabel m_cUpgradeCount;
#ifdef _XBOX
    CSWGuiLabel m_lblA;
    CSWGuiLabel m_lblB;
    CSWGuiLabel m_lblAAction;
    CSWGuiLabel m_lblBack;
    CSWGuiLabel m_lblRanalog;
    CSWGuiLabel m_lblRanalogLs;
#endif
    CSWGuiButton m_cAssemble;
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
    unsigned char m_anUpgradeProperties[NUM_UPGRADE_SLOTS];
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
#ifdef _XBOX
    CSWGuiLabel m_lblA;
    CSWGuiLabel m_lblUpgradeitems;
    CSWGuiLabel m_lblB;
    CSWGuiLabel m_lblBack;
#else

    CSWGuiButton m_btnUpgradeitems;
    CSWGuiButton m_btnBack;
#endif

#ifdef _XBOX
    CSWGuiButton m_aTypeButtons[4];
#else
    CSWGuiRadioButton m_aTypeButtons[4];
#endif
    CSWGuiLabel m_aTypeLabels[4];

    unsigned long m_oidItem;
    int m_anUpgradable[4];
    CSWGuiUpgradeItemSelect* m_pItemSelect;
#ifndef _XBOX
    CSWGuiControl* m_pSelectedType;
#endif
    void AutoUpgradeDone();
    void GetUpgradableItems(CSWSCreature*, int*, C2DA*);
    virtual void HandleInputEvent(int, int);
    void OnTypeSelected(CSWGuiControl*);
    void OnEnter(CSWGuiControl*);
    ~CSWGuiUpgradeSelection();
    virtual void OnPanelAdded();
    CSWGuiUpgradeSelection(CSWGuiManager*);
};

class CSWUpgradeItemEntry : public CSWGuiButton
{
public:

    static const int ENTRY_HEIGHT = 56;

    static const unsigned long STRREF_EQUIPPED = 32346;
    unsigned long m_oidItem;
    unsigned long m_nItemId;
    unsigned long m_nItemProperty;
    unsigned int m_bShowItemSuffix : 1;
    char m_nUpgradeIndex;
    char gap_1D5[3];
    CSWGuiBorder m_CSWGuiBorder_1D8;
    CSWGuiBorder m_CSWGuiBorder_24C;
    CSWGuiBorder m_CSWGuiBorder_2C0;
    CSWGuiText m_CSWGuiText_334;
    virtual void Initialize(CSWGuiText&, CSWGuiBorder&, CSWGuiBorder&, int);
    virtual void Initialize(const CSWGuiExtent&, CSWGuiButton*)  // PC 0x005B5E90  Xbox 0x000FBD20
    {
    }
    virtual void Initialize(const CSWGuiExtent&, const CSWGuiTextParams&,  // PC 0x0063CFC0
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
