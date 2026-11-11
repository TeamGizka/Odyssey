#include <string.h>
#include "exobase.h"
#include "exoresman.h"
#include "nwsobject.h"
#include "gameobjectarray.h"
#include "nwsplayerluo.h"
// PC 0x005CA030  Xbox 0x00244720
void CSWSPlayerLUOInventory::ClearSlots()
{
    for (int i = 0; i < 18; i++)
    {
        m_oidSlot[i] = CGameObjectArray::INVALIDOBJID;
        m_nSlotIndex[i] = -1;
    }
}
// PC 0x005CA060  Xbox 0x00244750
void CSWSPlayerLastUpdateObject::ClearKnownSpellUsesLeft()
{
    for (int i = 0; i < 2; i++)
    {
        m_nKnownSpellUsesLeft[i] = 0;
    }
}
// PC 0x005CA070  Xbox 0x00244760
unsigned char CSWSPlayerLastUpdateObject::GetKnownSpellUsesLeft(
    unsigned char nClass, unsigned char nLevel)
{
    if (nClass >= 2 || nLevel >= 1)
    {
        return 0;
    }
    return m_nKnownSpellUsesLeft[nClass + nLevel];
}
// PC 0x005CA0A0  Xbox 0x00244790
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
// PC 0x005CA0D0  Xbox 0x002447C0
void CSWSPlayerLastUpdateObject::ClearActionQueue()
{
    for (int i = 0; i < 10; ++i)
    {
        m_aActions[i].Clear();
    }
}
// PC 0x005CA470  Xbox 0x00244B30
void CSWSPlayerLUOInventory::ClearRepository()
{
    while (!m_lstRepository.IsEmpty())
    {
        delete m_lstRepository.RemoveHead();
    }
}
// PC 0x005CA4A0  Xbox 0x00244B60
void CSWSPlayerLUOInventory::ClearContainer()
{
    while (!m_lstContainer.IsEmpty())
    {
        delete m_lstContainer.RemoveHead();
    }
}
// PC 0x005CA4D0  Xbox 0x00244B90
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
// PC 0x005CA520  Xbox 0x00244BE0
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
// PC 0x005CA570  Xbox 0x00244C30
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
// PC 0x005CA5C0  Xbox 0x00244C80
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
// PC 0x005CA630  Xbox 0x00244CF0
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
// PC 0x005CA6A0  Xbox 0x00244D60
CLastUpdateObject::~CLastUpdateObject()
{
}
// PC 0x005CA740  Xbox 0x00244E00
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
// PC 0x005CA780  Xbox 0x00244E40
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
// PC 0x005CA7F0  Xbox 0x00244EB0
CSWSPlayerLUOInventory::CSWSPlayerLUOInventory()
{
    ClearSlots();
    ClearRepository();
    ClearContainer();
    ClearStore();
    m_oidRepository = CGameObjectArray::INVALIDOBJID;
}
// PC 0x005CA900  Xbox 0x00244FD0
CSWSPlayerLUOInventory::~CSWSPlayerLUOInventory()
{
    ClearSlots();
    ClearRepository();
    ClearContainer();
    ClearStore();
    m_oidRepository = CGameObjectArray::INVALIDOBJID;
}
// PC 0x005CAA50  Xbox 0x00245120
CLastUpdateObject::CLastUpdateObject()
{
    m_nUnknown2A = 0;
    m_fUnknown50 = 1.0f;
    m_nAnimation = CSWSObject::ANIMATION_COMBAT_READY;
    memset(m_nClass, UNSET_SHORT_CACHE, sizeof(m_nClass));
    memset(m_nClassLevel, UNSET_SHORT_CACHE, sizeof(m_nClassLevel));
}
// PC 0x005CAC80  Xbox 0x00245370
CSWSPlayerLastUpdateObject::CSWSPlayerLastUpdateObject()
{
    m_bCanLevelUp = 0;
    m_pExtraSpells = NULL;
}
// PC 0x005CAD70  Xbox 0x00245460
CSWSPlayerLastUpdateObject::~CSWSPlayerLastUpdateObject()
{
    ClearKnownSpells();
    ClearVisibilityList();
    delete m_pExtraSpells;
    m_pExtraSpells = NULL;
}
// PC 0x005CAB20  Xbox 0x00245210
void CSWSPlayerLastUpdateObject::ClearKnownSpells()
{
    for (int i = 0; i < 2; i++)
    {
        m_aKnownSpells[i].SetSize(0);
    }
}
// PC 0x005CAB60  Xbox 0x00245250
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
// PC 0x005CABE0  Xbox 0x002452D0
void CSWSPlayerLastUpdateObject::AddKnownSpell(unsigned char nClass,
                                               unsigned char nLevel,
                                               unsigned long nSpell)
{
    m_aKnownSpells[nClass + nLevel].Add(nSpell);
}
// PC 0x005CAC30  Xbox 0x00245320
void CSWSPlayerLastUpdateObject::ClearSpellAddDeleteLists()
{
    for (int i = 0; i < 2; i++)
    {
        m_aSpellsRemoved[i].SetSize(0);
        m_aSpellsAdded[i].SetSize(0);
    }
}
