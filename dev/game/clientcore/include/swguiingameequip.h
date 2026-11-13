#ifndef __SWGUIINGAMEEQUIP_H__
#define __SWGUIINGAMEEQUIP_H__

#include "swguidrawable.h"
#include "swguicontrol.h"
#include "swgui.h"
#include "nwitem.h"
class CSWCCreature;
class CSWGuiControl;
class CSWGuiExtent;
class CSWGuiManager;
class CSWGuiText;
class CSWGuiText_vtbl;
class CSWGuiInGameItemEntry;
class CSWSItem;

class CSWGuiContainer : public CSWGuiPanel
{
public:
    enum
    {
        GIVE_MODE_ACTIVE = 0x01,
        GIVE_MODE_SETUP = 0x02,
        GIVE_MODE_CLEAR = 0x04
    };

    enum { CONTAINER_TAKE_ALL = 1 };

    enum { STRREF_ITEMS_TO_GIVE = 392, STRREF_GET_ITEMS = 38542, STRREF_GIVE_ITEM = 38543, STRREF_GIVE_BUTTON_PREFIX = 47884 };

    enum { STRREF_CONTAINER_INVENTORY = 393, STRREF_CONTAINER_EMPTY = 394,
           STRREF_TARGET_INVENTORY_EMPTY = 119906 };

    unsigned long m_oidContainer;
    unsigned char m_nContainerType;
    char gap_69[3];
    unsigned long m_nGiveModeFlags;
    CSWGuiLabel m_lblMessage;
    CSWGuiLabel m_lblButton;
    CSWGuiLabel m_lblOk;
    CSWGuiLabel m_lblB;
    CSWGuiLabel m_lblCancel;
    CSWGuiLabel m_lblX;
#ifdef _XBOX
    CSWGuiLabel m_lblGiveitems;
#endif
    CSWGuiListBox m_lbItems;
#ifndef _XBOX
    CSWGuiButton m_btnOk;
    CSWGuiButton m_btnCancel;
    CSWGuiButton m_btnGiveitems;
#endif
    virtual void OnPanelAdded();
    virtual void OnPanelRemoved();
    CSWGuiContainer(CSWGuiManager*);
    void GiveItem(CSWGuiControl*);
#ifdef _XBOX
    void SetupGiveMode();
#else
    void SetupGiveMode(int);
#endif
    void SetContainer(unsigned long, int);
#ifndef _XBOX
    virtual void Update(float);
#endif
    virtual void HandleInputEvent(int, int);
};
#ifndef _XBOX
class CSWGuiInGameEquip : public CSWGuiPanel
{
public:

    enum
    {
        STRREF_ENVIRO_SUIT_RESTRICTION = 42346,
        STRREF_ARMOUR_COMBAT_RESTRICTION = 1506,
        STRREF_TWO_HANDED_RESTRICTION = 42344,
        STRREF_NO_EQUIPPABLE_ITEMS = 42345,
        STRREF_SELECT_ITEM_TO_EQUIP = 38154,

        STRREF_NO_DESCRIPTION = 32172,
        STRREF_ITEM_PREREQUISITES = 38450,
        STRREF_INCOMPATIBLE_WEAPON = 42271,
        STRREF_CANCEL = 1581,
        STRREF_CLOSE = 1582,
        STRREF_SLOT_LOCKED = 125554,
        STRREF_LEFT_HAND_LOCKED = 125569
    };
    CSWCCreature* m_pCreature;

    enum { NUM_EQUIPMENT_SLOTS = 11 };

    enum { INVALID_SLOT_ITEM_INDEX = 12 };

    enum { RIGHTHAND_ITEM_INDEX = 1 };

    enum { LEFTHAND_ITEM_INDEX = 0 };

    enum { ARMOUR_ITEM_INDEX = 5 };

    enum { LEFTHAND2_ITEM_INDEX = 9, RIGHTHAND2_ITEM_INDEX = 10 };

    CSWGuiButton m_aSlotButtons[NUM_EQUIPMENT_SLOTS];
    CSWGuiLabel m_aSlotLabels[NUM_EQUIPMENT_SLOTS];
    int m_an2274[NUM_EQUIPMENT_SLOTS];
    CExoArrayList<CSWGuiInGameItemEntry*> m_lItemEntries;

    CSWGuiLabel m_lblAtkl;
    CSWGuiLabel m_lblTohitl;
    CSWGuiLabel m_lblAtkr;
    CSWGuiLabel m_lblTohitr;
    CSWGuiLabel m_cDescriptionText;

    CSWGuiLabel m_lblAttackInfo;
    CSWGuiLabel m_lblDefInfo;
    CSWGuiLabel m_lblDefText;
    CSWGuiLabel m_lblTohit;
    CSWGuiLabel m_lblDamage;
    CSWGuiLabel m_lblSlotname;
    CSWGuiLabel m_lblTitle;
    CSWGuiLabel m_lblAtkl2;
    CSWGuiLabel m_lblTohitl2;
    CSWGuiLabel m_lblAtkr2;
    CSWGuiLabel m_lblTohitr2;
    CSWGuiListBox m_lbItems;
    CSWGuiListBox m_lbDesc;
    CSWGuiButton m_btnEquip;
    CSWGuiButton m_btnBack;
    CSWGuiLabel m_lblCantequip;
    CSWGuiLabel m_alblBar[5];
    CSWGuiLabel m_lblBack1;
    CSWGuiLabel m_lblAttackMod;
    CSWGuiLabel m_lblDamText;
    CSWGuiLabel m_lblDef;
    CSWGuiLabel m_lblDefBack;
    CSWGuiButton m_btnSwapWeapons;
    unsigned int m_bDescriptionShown : 1;
    unsigned int : 31;

    int m_nSelectedSlot;

    unsigned long m_aoidEquippedItems[NUM_EQUIPMENT_SLOTS];
    CSWGuiControl* m_pDescribedControl;
    int m_nPendingSlot;
    unsigned long m_oidPendingItem;
    unsigned long m_oidDescribed;
    CSWSItem* m_pDescribedCopy;
    unsigned long m_oidCompared;
    CSWSItem* m_pComparedCopy;
    float m_f50E4;
    int m_n50E8;
    int m_nCharacter;
    CSWGuiButton m_btnPrevNpc;
    CSWGuiButton m_btnNextNpc;
    CExoString GetIcon(int);
    void SetDescription(CExoString const&);
    int EquipItem(unsigned long, int, int);
    int EquipItem(CSWSItem*, int, int);
    void UnequipItem(unsigned long, int);
    void ShowCantEquipMessage(int bShow, unsigned long nStrRef);
    void UpdatePortraits(int bShow);

    enum { CHARACTER_UNSET = -2, CHARACTER_PLAYER = -1 };

    enum
    {
        NPC_ATTON = 0,
        NPC_BAO_DUR = 1,
        NPC_CANDEROUS = 2,
        NPC_G0T0 = 3,
        NPC_HANDMAIDEN = 4,
        NPC_HK_47 = 5,
        NPC_KREIA = 6,
        NPC_MIRA = 7,
        NPC_T3_M4 = 8,
        NPC_VISAS = 9,
        NPC_HANHARR = 10,
        NPC_DISCIPLE = 11
    };

    CSWSItem* CopyItem(unsigned long);
    int ItemExists(unsigned long);

    int GetSlotItemIdIndex(int nSlot);
    void SetCharacter(CSWCCreature*);
    void ShutdownCleanup();
    void UpdateSlotLocks();
    int IsSlotLocked(int nSlot);
    void OnPrevNPCClicked(CSWGuiControl*);
    void OnNextNPCClicked(CSWGuiControl*);
    void ShowDescription(int bShow);
    void OnItemExited(CSWGuiControl*);
    void OnItemSelected(CSWGuiControl*);
    void CloseDescription();
    ~CSWGuiInGameEquip();
    virtual void OnPanelRemoved();
    void OnSelectSlot(CSWGuiControl*);
    void OnOKPressed(CSWGuiControl*);
    CSWGuiInGameItemEntry* AddItemEntryToList(int);
    void OnEnterSlot(CSWGuiControl*);

    void UpdateInventory();
    virtual void HandleInputEvent(int, int);
    CSWGuiInGameEquip(CSWGuiManager*);
    virtual void Draw(float);
    virtual void OnPanelAdded();

    struct SlotInfo
    {
        int nSlot;
        const char* sButton;
        const char* sLabel;
        unsigned long nNameStrRef;
        unsigned long nDescriptionStrRef;
    };
    static const SlotInfo SLOT_INFO[NUM_EQUIPMENT_SLOTS];
};
#endif

enum SWGuiItemEntryLayout
{
    SWGUI_ITEM_ICON_SIZE = 49, SWGUI_ITEM_COUNT_HEIGHT = 19,
    SWGUI_ITEM_COUNT_SHORT_DIGITS = 2,
    SWGUI_ITEM_COUNT_SHORT_WIDTH = 21, SWGUI_ITEM_COUNT_LONG_WIDTH = 55,
    SWGUI_ITEM_COUNT_LONG_ALIGNMENT = 0x24
};

class CSWGuiInGameItemEntry : public CSWGuiRadioButton
{
public:

    enum
    {
        CAN_USE = 0,
        CAN_USE_EQUIPPED = 1,
        CANNOT_USE = 2,
        CANNOT_USE_WITH_OTHER_HAND = 3,
        CANNOT_USE_LEFT_HAND_LOCKED = 4
    };
    unsigned long m_oidItem;
    CSWGuiBorder m_cIconBorder;
    CSWGuiBorder m_cIconHilite;
    CSWGuiBorder m_cCountBorder;
    CSWGuiText m_cCountText;
    unsigned int m_bPulsing : 1;

    unsigned int m_bEquipped : 1;
    unsigned int m_bUnavailable : 1;
    unsigned int : 29;
    unsigned char m_nCanUse;
    char gap_399[3];
    virtual void SetExtent(const CSWGuiExtent& cExtent);
    virtual void Draw(float);
    // PC 0x006049D0 GOG 0x006049B0
    virtual void Initialize(const CSWGuiExtent&, CSWGuiButton*)
    {
    }
    virtual void Initialize(const CSWGuiExtent&,  // PC 0x00693D50 GOG 0x00693AC0
                                          const CSWGuiTextParams&,
                                          const CSWGuiBorderParams&,
                                          const CSWGuiBorderParams&)
    {
    }
    virtual void Initialize(CSWGuiText&, CSWGuiBorder&, CSWGuiBorder&, int,
                            int);
    int GetNumItems();

    unsigned long GetItem() { return m_oidItem; }
    bool IsEquipped() { return m_bEquipped != 0; }
    unsigned char GetCanUse() { return m_nCanUse; }
    void SetCanUse(unsigned char);
    enum { STRREF_EQUIPPED = 32346 };
    void SetItem(unsigned long, int, int);
    void UpdateItem();
    CSWGuiInGameItemEntry();
#ifdef _XBOX
    void RefreshItem();
#endif
};
class CSWGuiStoreItemEntry : public CSWGuiButton
{
public:
    virtual void SetExtent(const CSWGuiExtent& cExtent);
    virtual void Draw(float fElapsed);
    virtual void Initialize(const CSWGuiExtent&, CSWGuiButton*)  // PC 0x006049D0 GOG 0x006049B0
    {
    }
    virtual void Initialize(const CSWGuiExtent&,  // PC 0x00693D50 GOG 0x00693AC0
                                         const CSWGuiTextParams&,
                                         const CSWGuiBorderParams&,
                                         const CSWGuiBorderParams&)
    {
    }

    unsigned long m_oidItem;
    CSWGuiBorder m_cIconBorder;
    CSWGuiBorder m_cIconHilite;
    CSWGuiBorder m_cCountBorder;
    CSWGuiText m_cCountText;
    int m_nCharacter;
    int m_n3B0;
    int m_n3B4;
    virtual void Initialize(CSWGuiText&, CSWGuiBorder&, CSWGuiBorder&, int);
    enum { CHARACTER_NONE = -1 };
    CSWGuiStoreItemEntry();
    void SetItem(unsigned long, int, int nCharacter = CHARACTER_NONE);
    void SetColors(Vector, Vector, Vector, Vector);
};

#ifdef _XBOX

class CSWGuiInGameEquip : public CSWGuiPanel
{
public:
    enum
    {
        STRREF_ENVIRO_SUIT_RESTRICTION = 42346,
        STRREF_ARMOUR_COMBAT_RESTRICTION = 1506,
        STRREF_TWO_HANDED_RESTRICTION = 42344,
        STRREF_NO_EQUIPPABLE_ITEMS = 42345,
        STRREF_SELECT_ITEM_TO_EQUIP = 38154,
        STRREF_NO_DESCRIPTION = 32172,
        STRREF_ITEM_PREREQUISITES = 38450,
        STRREF_INCOMPATIBLE_WEAPON = 42271
    };
    enum { NUM_EQUIPMENT_SLOTS = 9 };
    enum { INVALID_SLOT_ITEM_INDEX = 10 };
    enum { RIGHTHAND_ITEM_INDEX = 1 };
    enum { LEFTHAND_ITEM_INDEX = 0 };
    enum { ARMOUR_ITEM_INDEX = 5 };

    enum { NUM_ITEM_ROWS = 5, NUM_DESC_BORDERS = 4 };
    enum { CHARACTER_UNSET = -2, CHARACTER_PLAYER = -1 };

    struct ItemListEntry
    {
        unsigned long m_oidItem;
        unsigned char m_nCanUse;
        unsigned long m_bEquipped : 1;
    };
    CSWCCreature* m_pCreature;
    CSWGuiButton m_aSlotButtons[NUM_EQUIPMENT_SLOTS];
    CSWGuiLabel m_aSlotLabels[NUM_EQUIPMENT_SLOTS];
    CSWGuiLabel m_lblDownarrow;
    CSWGuiLabel m_lblUparrow;
    CExoArrayList<ItemListEntry> m_lItems;
    CSWGuiInGameItemEntry m_aItemRows[NUM_ITEM_ROWS];
    CSWGuiLabel m_lblAtkl;
    CSWGuiLabel m_lblTohitl;
    CSWGuiLabel m_lblAtkr;
    CSWGuiLabel m_lblTohitr;
    CSWGuiLabel m_lblDef;
    CSWGuiLabel m_lblVitality;
    CSWGuiLabel m_cDescriptionText;
    CSWGuiLabel m_lblPortrait;
    CSWGuiLabel m_lblAttackInfo;
    CSWGuiLabel m_lblDefInfo;
    CSWGuiLabel m_lblTxtbar;
    CSWGuiLabel m_lblPortBord;
    CSWGuiLabel m_lblEquip;
    CSWGuiLabel m_lblTohit;
    CSWGuiLabel m_lblDamage;
    CSWGuiLabel m_lblSlotname;
    CSWGuiLabel m_lblA;
    CSWGuiLabel m_lblB;
    CSWGuiLabel m_lblStart;
    CSWGuiLabel m_lblBack;
    CSWGuiLabel m_lblTitle;
    CSWGuiLabel m_lblSelecttitle;
    CSWGuiLabel m_lblBlack;
    CSWGuiLabel m_aDescBorders[NUM_DESC_BORDERS];
    CSWGuiLabel m_lblRanalog;
    CSWGuiListBox m_lbDesc;
    unsigned int m_bDescriptionShown : 1;

    unsigned int m_bResetItems : 1;
    unsigned int : 30;
    int m_nSelectedSlot;

    short m_nTopItem;
    short m_nSelectedItem;
    unsigned long m_aoidEquippedItems[NUM_EQUIPMENT_SLOTS];
    CSWGuiControl* m_pDescribedControl;
    CSWGuiInGameEquip(CSWGuiManager*);
    ~CSWGuiInGameEquip();
    virtual void Draw(float);
    virtual void HandleInputEvent(int, int);
    virtual void OnPanelAdded();
    virtual void OnPanelRemoved();
    CExoString GetIcon(int);
    void SetDescription(CExoString const&);
    void SetCharacter(CSWCCreature*);
    void ShowDescription(int bShow);
    void OnItemSelected(CSWGuiControl*);
    void OnOKPressed(CSWGuiControl*);
    void CloseDescription();
    void OnSelectSlot(CSWGuiControl*);
    void OnEnterSlot(CSWGuiControl*);
    void UpdateInventory();

    void SetItemRows(int nTopItem, int nSelectedItem);
    void SelectNextItem();
    void SelectPreviousItem();
    void ScrollItemsDown();
    void ScrollItemsUp();
    struct SlotInfo
    {
        int nSlot;
        const char* sButton;
        const char* sLabel;
        unsigned long nNameStrRef;
        unsigned long nDescriptionStrRef;
    };
    static const SlotInfo SLOT_INFO[NUM_EQUIPMENT_SLOTS];
};
#endif

#endif
