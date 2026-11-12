#ifndef __NWSSTORE_H__
#define __NWSSTORE_H__

#include "nwsobject.h"

class CItemRepository;
class CResGFF;
class CResRef;
class CResStruct;
class CSWSArea;
class CSWSCreature;
class CSWSItem;

class CSWSStore : public CSWSObject
{
public:

    enum BuySellFlag
    {
        BuySellFlagBuy = 1,
        BuySellFlagSell = 2,
        BuySellFlagBoth = 3
    };

    enum { MIN_BONUS_PERCENT = -100, MAX_BONUS_PERCENT = 100 };

    CSWSStore(unsigned long oidStore);
    virtual ~CSWSStore();

    virtual void AIUpdate();
    unsigned long CalculateItemBuyPrice(CSWSItem* pItem,
                                        unsigned long oidCreature);
    unsigned long CalculateItemSellPrice(CSWSItem* pItem,
                                         unsigned long oidCreature);
    // PC 0x00698BD0
    virtual CSWSStore* AsSWSStore()
    {
        return this;
    }

    virtual void EventHandler(unsigned long nEventId, unsigned long oidCaller,
                              void* pEventData, unsigned long nCalendarDay,
                              unsigned long nTimeOfDay);

    int RemoveItem(CSWSItem* pItem);
    int GetItemInInventory(unsigned long oidItem);
    int SaveStore(CResGFF* pRes, CResStruct* pStruct);
    int AddToArea(CSWSArea* pArea, float fX, float fY, float fZ, int bLoading);
    int SellItem(CSWSItem* pItem, CSWSCreature* pCreature);
    int AddItemToInventory(CSWSItem** ppItem, int bAppend = 0);
    int LoadStore(CResGFF* pRes, CResStruct* pStruct, int bFromTemplate);
    int AcquireItem(CSWSItem* pItem, int bAppend = 0);
    int LoadFromTemplate(const CResRef& cResRef, int bFromTemplate);

    CExoString m_sOnOpenStore[1];
    CItemRepository* m_pItemRepository;
    CExoLocString m_lsName;
    unsigned long m_oidEventCaller;
    int m_n240;
    int m_nMarkDown;
    int m_nMarkUp;

    int m_nBonusMarkDown;
    int m_nBonusMarkUp;

    unsigned short m_nInventoryIndex;
    unsigned char m_nBuySellFlag;
    char m_aUnmapped257[1];

public:

    int GetMarkDown()
    {
        return m_nMarkDown;
    }
    void SetMarkDown(int nMarkDown)
    {
        m_nMarkDown = nMarkDown;
    }
    int GetMarkUp()
    {
        return m_nMarkUp;
    }
    void SetMarkUp(int nMarkUp)
    {
        m_nMarkUp = nMarkUp;
    }
    CExoLocString* GetName()
    {
        return &m_lsName;
    }
};

#endif
