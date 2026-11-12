#ifndef __SWGUISTORE_H__
#define __SWGUISTORE_H__

#include "swguicontrol.h"
#include "swgui.h"
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
    int m_nUnknown64;

    CExoArrayList<CSWGuiStoreItemEntry*> m_lStoreEntries;
    CExoArrayList<CSWGuiStoreItemEntry*> m_lInventoryEntries;
    CSWGuiLabel m_lblUnknown80;
    CSWGuiLabel m_lblUnknown1C0;
    CSWGuiLabel m_lblUnknown300;
    CSWGuiLabel m_lblUnknown440;
    CSWGuiLabel m_lblUnknown580;
    CSWGuiLabel m_lblUnknown6C0;
    CSWGuiLabel m_lblUnknown800;
    CSWGuiLabel m_lblDescription;
    CSWGuiLabel m_lblCost;
    CSWGuiLabel m_lblItemValue;
    CSWGuiLabel m_lblStock;
    CSWGuiLabel m_lblUnknownD00;
    CSWGuiLabel m_lblUnknownF80;
    CSWGuiLabel m_lblCredits;
    CSWGuiLabel m_lblCreditsValue;
    CSWGuiLabel m_lblBuySell;
    CSWGuiListBox m_lbxStoreItems;
    CSWGuiListBox m_lbxInventoryItems;
    CSWGuiListBox m_CSWGuiListBox_1A40;
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
    int m_nUnknown226C;
    int m_nGold;
    int m_nUnknown2274;
    unsigned long m_nStoreId;
    unsigned long m_nCreatureId;
#ifndef _XBOX
    unsigned char m_nFilter;
    unsigned char m_bInventoryDirty;
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
