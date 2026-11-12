#include "appmanager.h"
#include "serverexoapp.h"
#include "nwitem.h"
#include "gameobjectarray.h"
#include "nwsitem.h"
// PC 0x005ECEC0
CSWInventory::CSWInventory()
{
    for (int i = 0; i < TOTAL_SLOT_COUNT; ++i)
    {
        m_oidSlot[i] = 0;
    }
}

CSWInventory::~CSWInventory()
{
}
// PC 0x005ECFD0
int CSWInventory::GetItemInInventory(CSWSItem* pItem)
{
    unsigned long oidItem = OBJECT_INVALID;

    if (pItem != NULL)
    {
        oidItem = pItem->m_nId;
    }

    for (int i = 0; i < TOTAL_SLOT_COUNT; i++)
    {
        if (m_oidSlot[i] == oidItem)
        {
            return 1;
        }
    }

    return 0;
}
// PC 0x005ED000
unsigned long CSWInventory::GetSlotFromItem(CSWSItem* pItem)
{
    unsigned long oidItem = OBJECT_INVALID;

    if (pItem != NULL)
    {
        oidItem = pItem->m_nId;
    }

    for (int i = 0; i < TOTAL_SLOT_COUNT; i++)
    {
        if (m_oidSlot[i] == oidItem)
        {
            return (i < 11) ? (1 << i) : (1 << (i + 3));
        }
    }

    return 0;
}
// PC 0x005ED1D0
CSWSItem* CSWInventory::GetItemInSlot(unsigned long nSlot)
{
    unsigned long* pSlot = GetSlotPointer(nSlot);

    if (pSlot != NULL)
    {
        CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
        return pServerExoApp->GetItemByGameObjectID(*pSlot);
    }

    return NULL;
}
// PC 0x005ED040
unsigned long* CSWInventory::GetSlotPointer(unsigned long nSlot)
{
    switch (nSlot)
    {
    case INVENTORY_SLOT_HEAD: return &m_oidSlot[0];
    case INVENTORY_SLOT_ARMOUR: return &m_oidSlot[1];
    case INVENTORY_SLOT_BOOTS: return &m_oidSlot[2];
    case INVENTORY_SLOT_HANDS: return &m_oidSlot[3];
    case INVENTORY_SLOT_RIGHTHAND: return &m_oidSlot[4];
    case INVENTORY_SLOT_LEFTHAND: return &m_oidSlot[5];
    case INVENTORY_SLOT_LEFTRING: return &m_oidSlot[6];
    case INVENTORY_SLOT_RIGHTRING: return &m_oidSlot[7];
    case INVENTORY_SLOT_NECK: return &m_oidSlot[8];
    case INVENTORY_SLOT_BELT: return &m_oidSlot[9];
    case INVENTORY_SLOT_ARMS: return &m_oidSlot[10];
    case INVENTORY_SLOT_ARROWS: return NULL;
    case INVENTORY_SLOT_BULLETS: return NULL;
    case INVENTORY_SLOT_BOLTS: return NULL;
    case INVENTORY_SLOT_CREATUREWEAPON1: return &m_oidSlot[11];
    case INVENTORY_SLOT_CREATUREWEAPON2: return &m_oidSlot[12];
    case INVENTORY_SLOT_CREATUREWEAPON3: return &m_oidSlot[13];
    case INVENTORY_SLOT_CREATUREARMOUR: return &m_oidSlot[14];
    case INVENTORY_SLOT_RIGHTHAND2: return &m_oidSlot[15];
    case INVENTORY_SLOT_LEFTHAND2: return &m_oidSlot[16];
    default: return NULL;
    }
}
// PC 0x005ED150
int CSWInventory::RemoveItem(CSWSItem* pItem)
{
    unsigned long oidItem = OBJECT_INVALID;

    if (pItem != NULL)
    {
        oidItem = pItem->m_nId;
    }

    for (int i = 0; i < TOTAL_SLOT_COUNT; i++)
    {
        if (m_oidSlot[i] == oidItem)
        {
            m_oidSlot[i] = OBJECT_INVALID;
            return 1;
        }
    }

    return 0;
}
// PC 0x005ED190
void CSWInventory::PutItemInSlot(unsigned long nSlot, CSWSItem* pItem)
{
    unsigned long* pSlot = GetSlotPointer(nSlot);
    if (pSlot != NULL)
    {
        *pSlot = pItem->m_nId;
    }
}
