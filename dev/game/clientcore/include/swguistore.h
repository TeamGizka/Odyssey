#ifndef __SWGUISTORE_H__
#define __SWGUISTORE_H__

#include "swguicontrol.h"
#include "swgui.h"
#ifdef _XBOX
#include "swguiingameequip.h"
#endif
class CSWGuiControl;
class CSWGuiStoreItemEntry;
class CSWGuiManager;
class CSWSItem;

class CSWGuiStore : public CSWGuiPanel
{
public:

    enum { STRREF_INFINITE = 41951 };
    enum
    {
        STORE_FILTER_ALL = 0,
        STORE_FILTER_WEAPONS = 1,
        STORE_FILTER_ARMOR = 2,
        STORE_FILTER_MISC = 3,
        STORE_FILTER_COUNT = 4
    };
    enum { NUM_INVENTORY_SLOTS = 20 };

    enum
    {
        SELL_CONFIRM_VALUE_PER_LEVEL = 50,
        SELL_CONFIRM_VALUE_MAX = 250,
        SELL_CONFIRM_VALUE_TOKEN = 0,
        STRREF_CONFIRM_SELL = 41985,
        STRREF_CONFIRM_SELL_EQUIPPED = 105896,
        STRREF_CANNOT_SELL_EQUIPMENT = 125628
    };

    enum
    {
        BUY_CONFIRM_VALUE_PER_LEVEL = 50,
        BUY_CONFIRM_VALUE_DEFAULT = 50,
        BUY_CONFIRM_VALUE_TOKEN = 0,
        STRREF_CONFIRM_BUY = 42020,
        STRREF_INSUFFICIENT_CREDITS = 41950
    };
    int m_nPendingAction;

    CExoArrayList<CSWGuiStoreItemEntry*> m_lStoreEntries;
    CExoArrayList<CSWGuiStoreItemEntry*> m_lInventoryEntries;
    CSWGuiLabel m_lblA;
    CSWGuiLabel m_lblAccept;
    CSWGuiLabel m_lblB;
    CSWGuiLabel m_lblCancel;
    CSWGuiLabel m_lblY;
    CSWGuiLabel m_lblExamine;
    CSWGuiLabel m_lblX;
    CSWGuiLabel m_lblDescription;
    CSWGuiLabel m_lblCost;
    CSWGuiLabel m_lblItemValue;
    CSWGuiLabel m_lblStock;
    CSWGuiLabel m_lblStockValue;
#ifdef _XBOX
    CSWGuiLabel m_lblOnHand;
    CSWGuiLabel m_lblOnHandValue;
    CSWGuiLabel m_alblBar[5];
    CSWGuiLabel m_lblFilter;
    CSWGuiLabel m_alblFilter[STORE_FILTER_COUNT];
    CSWGuiLabel m_lblFilterY;
#endif
    CSWGuiLabel m_lblRanalog;
    CSWGuiLabel m_lblCredits;
    CSWGuiLabel m_lblCreditsValue;
    CSWGuiLabel m_lblBuySell;
#ifndef _XBOX
    CSWGuiListBox m_lbxStoreItems;
    CSWGuiListBox m_lbxInventoryItems;
#endif
    CSWGuiListBox m_lbxDescription;
#ifndef _XBOX
    CSWGuiButton m_btnCancel;
    CSWGuiButton m_btnExamine;
    CSWGuiButton m_btnAccept;
    CSWGuiLabel m_alblBar[5];
    CSWGuiLabel m_lblFilter;
    CSWGuiLabel m_lblOnHand;
    CSWGuiLabel m_lblOnHandValue;
    CSWGuiButton m_abtnFilter[STORE_FILTER_COUNT];
#endif
    int m_nPreviousGold;
    int m_nGold;
    int m_nSelectedEntry;
    unsigned long m_nStoreId;
    unsigned long m_nCreatureId;
    unsigned char m_nFilter;
    unsigned char m_bInventoryDirty;
#ifdef _XBOX
    enum { NUM_ITEM_ROWS = 8 };
    struct ItemListEntry
    {
        unsigned long m_oidItem;
        CSWGuiHandler m_pHandler;
        char m_nCharacter;
    };
    int m_bBuying;
    CExoArrayList<ItemListEntry> m_lInventoryItems;
    CExoArrayList<ItemListEntry> m_lStoreItems;
    CSWGuiStoreItemEntry m_aItemRows[NUM_ITEM_ROWS];
    int m_nTopItem;
    int m_nSelectedItem;
    int m_nPreviousSelectedItem;
    int GetNumItemEntries()
    {
        return m_bBuying ? m_lStoreItems.num : m_lInventoryItems.num;
    }
    ItemListEntry* GetItemEntries()
    {
        return m_bBuying ? m_lStoreItems.element : m_lInventoryItems.element;
    }
    void SetItemRows(int nTopItem, int nSelectedItem);
    void SelectNextItem();
    void SelectPreviousItem();
    void OnNextFilter();
#endif
    int CheckFilter(CSWSItem* pItem);
    void UpdateFilters();
    void OnFilterPressed(CSWGuiControl* pControl);
    virtual void Draw(float fElapsed);
    void PopulateStore();
    void SetDescription(CExoString const&);
    int GetItemBuyValue(CSWSItem*);
    int GetItemSellValue(CSWSItem*);
    void PopulateInventoryItemListBox();
    void OnControlEntered(CSWGuiControl*);
    void PopulateStoreItemListBox();
    void BuyItem(CSWSItem*);
    void BuyItemCallback(CSWGuiControl*);
    void SellItem(CSWSItem*);
    void SellItemCallback(CSWGuiControl*);
    int CreateItemEntry(int&, CExoArrayList<CSWGuiControl*>&, unsigned long,
                        int, int nCharacter);
    void OnControlInvAButton(CSWGuiControl*);
    void OnControlStoreAButton(CSWGuiControl*);
    void DeleteItemEntries();
    ~CSWGuiStore();
    virtual void OnPanelRemoved();
    void ShowBuyGUI();
    void ShowSellGUI();
#ifdef _XBOX
    void OnControlXButton();
#else
    void OnControlXButton(CSWGuiControl*);
#endif
    void InitializeStoreType();
    CSWGuiStore(CSWGuiManager*);
    virtual void OnPanelAdded();
    virtual void HandleInputEvent(int, int);
};

#endif
