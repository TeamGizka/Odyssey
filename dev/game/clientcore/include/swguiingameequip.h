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

    enum
    {
        STRREF_ITEMS_TO_GIVE = 392,
        STRREF_GET_ITEMS = 38542,
        STRREF_GIVE_ITEM = 38543,
        STRREF_GIVE_BUTTON_PREFIX = 47884
    };

    enum
    {
        STRREF_CONTAINER_INVENTORY = 393,
        STRREF_CONTAINER_EMPTY = 394
    };

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
    CSWGuiListBox m_lbItems;
    CSWGuiButton m_btnOk;
    CSWGuiButton m_btnCancel;
    CSWGuiButton m_btnGiveitems;
    virtual void OnPanelAdded();
    virtual void OnPanelRemoved();
    CSWGuiContainer(CSWGuiManager*);
    void GiveItem(CSWGuiControl*);
    void SetupGiveMode(int);
    void SetContainer(unsigned long, int);
    virtual void Update(float);
    virtual void HandleInputEvent(int, int);
};

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
    CSWCCreature* m_pCreature;

    enum { NUM_EQUIPMENT_SLOTS = 9 };

    enum { INVALID_SLOT_ITEM_INDEX = 10 };

    enum { RIGHTHAND_ITEM_INDEX = 1 };

    enum { LEFTHAND_ITEM_INDEX = 0 };

    enum { ARMOUR_ITEM_INDEX = 5 };
    CSWGuiButton m_aSlotButtons[NUM_EQUIPMENT_SLOTS];
    CSWGuiLabel m_aSlotLabels[NUM_EQUIPMENT_SLOTS];
    CExoArrayList<CSWGuiInGameItemEntry*> m_lItemEntries;

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
    CSWGuiLabel m_lblTohit;
    CSWGuiLabel m_lblDamage;
    CSWGuiLabel m_lblSlotname;
    CSWGuiLabel m_lblTitle;
    CSWGuiLabel m_lblSelecttitle;
    CSWGuiListBox m_lbItems;
    CSWGuiListBox m_lbDesc;
    CSWGuiButton m_btnEquip;
    CSWGuiButton m_btnBack;
    CSWGuiButton m_btnChange1;
    CSWGuiButton m_btnChange2;
    CSWGuiButton m_btnCharleft;
    CSWGuiButton m_btnCharright;
    CSWGuiLabel m_lblCantequip;
    unsigned int m_bDescriptionShown : 1;
    unsigned int : 31;
    char m_nCharacter;
    char gap_4275[3];

    int m_nSelectedSlot;

    unsigned long m_aoidEquippedItems[NUM_EQUIPMENT_SLOTS];
    CSWGuiControl* m_pDescribedControl;
    int m_nPendingSlot;
    unsigned long m_oidPendingItem;
    unsigned long m_oidDescribed;
    CSWSItem* m_pDescribedCopy;
    unsigned long m_oidCompared;
    CSWSItem* m_pComparedCopy;
    CExoString GetIcon(int);
    void SetDescription(CExoString const&);
    int EquipItem(unsigned long, int, int);
    int EquipItem(CSWSItem*, int, int);
    void UnequipItem(unsigned long, int);
    void ShowCantEquipMessage(int bShow, unsigned long nStrRef);

    enum { CHARACTER_UNSET = -2, CHARACTER_PLAYER = -1 };

    void UpdatePortraits(int bShow);
    void OnSwitchRight(CSWGuiControl*);
    void OnSwitchLeft(CSWGuiControl*);
    CSWSItem* CopyItem(unsigned long);
    int ItemExists(unsigned long);

    int GetSlotItemIdIndex(int nSlot);
    void SetCharacter(CSWCCreature*);
    void ShowDescription(int bShow);
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
    void OnChangeCharacter(CSWGuiControl*);
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

enum SWGuiItemEntryLayout
{
    SWGUI_ITEM_ICON_SIZE = 56, SWGUI_ITEM_COUNT_HEIGHT = 19,
    SWGUI_ITEM_COUNT_SHORT_DIGITS = 2,
    SWGUI_ITEM_COUNT_SHORT_WIDTH = 21, SWGUI_ITEM_COUNT_LONG_WIDTH = 42
};

class CSWGuiInGameItemEntry : public CSWGuiRadioButton
{
public:

    enum
    {
        CAN_USE = 0,
        CAN_USE_EQUIPPED = 1,
        CANNOT_USE = 2,
        CANNOT_USE_WITH_OTHER_HAND = 3
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
    // PC 0x005B5E90
    virtual void Initialize(const CSWGuiExtent&, CSWGuiButton*)
    {
    }
    virtual void Initialize(const CSWGuiExtent&,  // PC 0x0063CFC0
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
    CSWGuiInGameItemEntry();
};

class CSWGuiStoreItemEntry : public CSWGuiButton
{
public:
    virtual void SetExtent(const CSWGuiExtent& cExtent);
    virtual void Draw(float fElapsed);
    virtual void Initialize(const CSWGuiExtent&, CSWGuiButton*)  // PC 0x005B5E90  Xbox 0x000EB380
    {
    }
    virtual void Initialize(const CSWGuiExtent&,  // PC 0x0063CFC0
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
    virtual void Initialize(CSWGuiText&, CSWGuiBorder&, CSWGuiBorder&, int);
    CSWGuiStoreItemEntry();
    void SetItem(unsigned long, int);
};

#endif
