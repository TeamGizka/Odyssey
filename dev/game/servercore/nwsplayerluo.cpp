#include <string.h>
#include "exobase.h"
#include "exoresman.h"
#include "nwsobject.h"
#include "gameobjectarray.h"
#include "nwsplayerluo.h"
// PC 0x006151B0 GOG 0x00615190
void CSWSPlayerLUOInventory::ClearSlots()
{
    for (int i = 0; i < 20; i++)
    {
        m_oidSlot[i] = CGameObjectArray::INVALIDOBJID;
        m_nSlotIndex[i] = -1;
    }
}
// PC 0x006151E0 GOG 0x006151C0
void CSWSPlayerLastUpdateObject::ClearKnownSpellUsesLeft()
{
    for (int i = 0; i < 2; i++)
    {
        m_nKnownSpellUsesLeft[i] = 0;
    }
}
// PC 0x006151F0 GOG 0x006151D0
unsigned char CSWSPlayerLastUpdateObject::GetKnownSpellUsesLeft(
    unsigned char nClass, unsigned char nLevel)
{
    if (nClass >= 2 || nLevel >= 1)
    {
        return 0;
    }
    return m_nKnownSpellUsesLeft[nClass + nLevel];
}
// PC 0x00615220 GOG 0x00615200
void CSWSPlayerLastUpdateObject::SetKnownSpellUsesLeft(unsigned char nClass,
                                                       unsigned char nLevel,
                                                       unsigned char nUsesLeft)
{
    if (nClass < 2 && nLevel < 1)
    {
        m_nKnownSpellUsesLeft[nClass + nLevel] = nUsesLeft;
    }
}

inline void CSWActionNode::Clear()
{
    m_nActionId = 0xFFFF;
    m_nGroupActionId = 0xFFFF;
    m_nActionType = 0xFFFF;
    m_vLocation = Vector();
    m_oidTarget = OBJECT_INVALID;
    m_nParameter = -1;
}
// PC 0x00615250 GOG 0x00615230
void CSWSPlayerLastUpdateObject::ClearActionQueue()
{
    for (int i = 0; i < 10; ++i)
    {
        m_aActions[i].Clear();
    }
}
// PC 0x00615600 GOG 0x006155E0
void CSWSPlayerLUOInventory::ClearRepository()
{
    while (!m_lstRepository.IsEmpty())
    {
        delete m_lstRepository.RemoveHead();
    }
}
// PC 0x00615630 GOG 0x00615610
void CSWSPlayerLUOInventory::ClearContainer()
{
    while (!m_lstContainer.IsEmpty())
    {
        delete m_lstContainer.RemoveHead();
    }
}
// PC 0x00615660 GOG 0x00615640
CSWSPlayerLUOInventoryItem* CSWSPlayerLUOInventory::ItemListGetUpdateItem(
    CExoLinkedListNode* pNode, unsigned char nList)
{
    CSWSPlayerLUOInventoryItem* pItem = NULL;

    if (nList == 0)
    {
        pItem = m_lstContainer.GetAtPos(pNode);
    }
    else if (nList == 1)
    {
        pItem = m_lstRepository.GetAtPos(pNode);
    }
    else if (nList == 2)
    {
        pItem = m_lstOverflow.GetAtPos(pNode);
    }
    return pItem;
}
// PC 0x006156B0 GOG 0x00615690
void CSWSPlayerLUOInventory::ItemListSetEnd(CExoLinkedListNode*& pNode,
                                            unsigned char nList)
{
    if (nList == 0)
    {
        pNode = m_lstContainer.GetTailPos();
    }
    else if (nList == 1)
    {
        pNode = m_lstRepository.GetTailPos();
    }
    else if (nList == 2)
    {
        pNode = m_lstOverflow.GetTailPos();
    }
}
// PC 0x00615700 GOG 0x006156E0
void CSWSPlayerLUOInventory::ItemListGetPrev(CExoLinkedListNode*& pNode,
                                             unsigned char nList)
{
    if (nList == 0)
    {
        m_lstContainer.GetPrev(pNode);
    }
    else if (nList == 1)
    {
        m_lstRepository.GetPrev(pNode);
    }
    else if (nList == 2)
    {
        m_lstOverflow.GetPrev(pNode);
    }
}
// PC 0x00615750 GOG 0x00615730
void CSWSPlayerLUOInventory::ItemListRemove(CExoLinkedListNode* pNode,
                                            unsigned char nList)
{
    if (nList == 0)
    {
        delete m_lstContainer.Remove(pNode);
    }
    else if (nList == 1)
    {
        delete m_lstRepository.Remove(pNode);
    }
    else if (nList == 2)
    {
        delete m_lstOverflow.Remove(pNode);
    }
}
// PC 0x006157C0 GOG 0x006157A0
void CSWSPlayerLUOInventory::ItemListAddHead(unsigned long oidItem,
                                             unsigned char nList,
                                             unsigned long nStackSize,
                                             unsigned long nCost)
{
    CSWSPlayerLUOInventoryItem* pItem = new CSWSPlayerLUOInventoryItem;
    pItem->m_oidItem = oidItem;
    pItem->m_nStackSize = nStackSize;
    pItem->m_nCost = nCost;

    if (nList == 0)
    {
        m_lstContainer.AddHead(pItem);
    }
    else if (nList == 1)
    {
        m_lstRepository.AddHead(pItem);
    }
    else if (nList == 2)
    {
        m_lstOverflow.AddHead(pItem);
    }
    else
    {
        delete pItem;
    }
}
// PC 0x00615830 GOG 0x00615810
CLastUpdateObject::~CLastUpdateObject()
{
}
// PC 0x006158E0 GOG 0x006158C0
unsigned long CSWSPlayerLastUpdateObject::GetKnownSpell(unsigned char nClass,
                                                        unsigned char nLevel,
                                                        unsigned char nIndex)
{
    int nList = nLevel + nClass;

    if (nIndex >= m_aKnownSpells[nList].num)
    {
        return -1;
    }
    return m_aKnownSpells[nList][nIndex];
}
// PC 0x00615920 GOG 0x00615900
void CSWSPlayerLastUpdateObject::ClearVisibilityList()
{
    if (m_lstVisibility.num > 0)
    {
        for (int i = m_lstVisibility.num - 1; i >= 0; i--)
        {
            CSWVisibilityNode* pEntry = m_lstVisibility[i];

            m_lstVisibility.DelIndex(i);
            if (pEntry != NULL)
            {
                delete pEntry;
            }
        }
    }
}

void CSWSPlayerLUOInventory::ClearStore()
{
    while (!m_lstOverflow.IsEmpty())
    {
        delete m_lstOverflow.RemoveHead();
    }
}
// PC 0x00615990 GOG 0x00615970
CSWSPlayerLUOInventory::CSWSPlayerLUOInventory()
{
    ClearSlots();
    ClearRepository();
    ClearContainer();
    ClearStore();
    m_oidRepository = CGameObjectArray::INVALIDOBJID;
}
// PC 0x00615AA0 GOG 0x00615A80
CSWSPlayerLUOInventory::~CSWSPlayerLUOInventory()
{
    ClearSlots();
    ClearRepository();
    ClearContainer();
    ClearStore();
    m_oidRepository = CGameObjectArray::INVALIDOBJID;
}
// PC 0x00615BF0 GOG 0x00615BD0
CLastUpdateObject::CLastUpdateObject()
{
    m_nUnknown2A = 0;
    m_fUnknown50 = 1.0f;
    m_nAnimation = CSWSObject::ANIMATION_COMBAT_READY;
    memset(m_nClass, UNSET_SHORT_CACHE, sizeof(m_nClass));
    memset(m_nClassLevel, UNSET_SHORT_CACHE, sizeof(m_nClassLevel));
}
// PC 0x00615E30 GOG 0x00615E10
CSWSPlayerLastUpdateObject::CSWSPlayerLastUpdateObject()
{
    m_bCanLevelUp = 0;
    m_pExtraSpells = NULL;
}
// PC 0x00615F20 GOG 0x00615F00
CSWSPlayerLastUpdateObject::~CSWSPlayerLastUpdateObject()
{
    ClearKnownSpells();
    ClearVisibilityList();
    delete m_pExtraSpells;
    m_pExtraSpells = NULL;
}
// PC 0x00615CD0 GOG 0x00615CB0
void CSWSPlayerLastUpdateObject::ClearKnownSpells()
{
    for (int i = 0; i < 2; i++)
    {
        m_aKnownSpells[i].SetSize(0);
    }
}
// PC 0x00615D10 GOG 0x00615CF0
void CSWSPlayerLastUpdateObject::ClearFeats()
{
    m_lstFeats.SetSize(0);
    m_lstBonusFeats.SetSize(0);

    for (int i = 0; i < m_lstFeatUses.num; i++)
    {
        if (m_lstFeatUses.element[i] != NULL)
        {
            delete m_lstFeatUses.element[i];
            m_lstFeatUses.element[i] = NULL;
        }
    }
    m_lstFeatUses.SetSize(0);
}
// PC 0x00615D90 GOG 0x00615D70
void CSWSPlayerLastUpdateObject::AddKnownSpell(unsigned char nClass,
                                               unsigned char nLevel,
                                               unsigned long nSpell)
{
    m_aKnownSpells[nClass + nLevel].Add(nSpell);
}
// PC 0x00615DE0 GOG 0x00615DC0
void CSWSPlayerLastUpdateObject::ClearSpellAddDeleteLists()
{
    for (int i = 0; i < 2; i++)
    {
        m_aSpellsRemoved[i].SetSize(0);
        m_aSpellsAdded[i].SetSize(0);
    }
}
