#include "nwsitem.h"
#include <stdio.h>
#include "nwcobject.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "gameobjectarray.h"
#include "nwitem.h"
#include "nwrules.h"
#include "nwsarea.h"
#include "nwsmodule.h"
#include "nwsplaceable.h"
#include "nwsplayer.h"
#include "nwmessage.h"
#include "nwsscriptevent.h"
#include "resgff.h"
#include "serveraimaster.h"
#include "serverexoapp.h"
#include "gameobjectarray.h"
#include "twodimarray.h"
#include "twodimarrays.h"
#include "worldtimer.h"
#include "guiingame.h"
#include "nwsstore.h"
#include "nwfeat.h"
#include "clientoptions.h"
#include "nwcitem.h"

static inline void NwsitemTrace1();
#define NWSITEM_TRACE() NwsitemTrace1()

enum UpgradeItemType
{
    UPGRADE_ITEM_LIGHTSABER = 0,
    UPGRADE_ITEM_MELEE_1 = 1,
    UPGRADE_ITEM_MELEE_2 = 2,
    UPGRADE_ITEM_MELEE_3 = 3,
    UPGRADE_ITEM_BLASTER_1 = 4,
    UPGRADE_ITEM_BLASTER_2 = 5,
    UPGRADE_ITEM_BLASTER_3 = 6,
    UPGRADE_ITEM_BLASTER_4 = 7,
    UPGRADE_ITEM_ARMOR_1 = 8,
    UPGRADE_ITEM_ARMOR_2 = 9
};

inline void CSWSItem::SetStolen(int bStolen) { m_bStolen = bStolen; }
inline void CSWSItem::SetPickpocketable(int bPickpocketable) { m_bPickpocketable = bPickpocketable; }
inline void CSWSItem::SetNonEquippable(int bNonEquippable) { m_bNonEquippable = bNonEquippable; }
inline void CSWSItem::SetNewItem(int bNewItem) { m_bNewItem = bNewItem; }
inline int CSWSItem::GetUpgrades() { return m_nUpgrades; }
inline char CSWSItem::GetModelVariation() { return m_nModelVariation; }

void CSWSItem::SetNumPassiveProperties(int nNumProperties)
{
    m_nPassiveProperties = nNumProperties;
    if (m_pPassiveProperties != NULL)
        delete[] m_pPassiveProperties;
    if (nNumProperties != 0)
        m_pPassiveProperties = new CSWItemProperty[m_nPassiveProperties];
    else
        m_pPassiveProperties = NULL;
}
// PC 0x005530A0  Xbox 0x001D32E0
CSWSItem::CSWSItem(unsigned long oidItem)
    : CSWSObject(CGameObject::OBJECT_TYPE_ITEM, oidItem, 0)
{
    m_pRepository = NULL;
    m_bInventoryOpen = 0;
    g_pAppManager->m_pServerExoApp->GetServerAIMaster()->AddObject(
        this, CServerAIMaster::AI_LEVEL_ITEM);

    m_nAnimation = ANIMATION_COMBAT_READY;
    m_oidPossessor = CGameObjectArray::INVALIDOBJID;
    m_nActiveProperties = 0;
    m_nPassiveProperties = 0;
    m_pActiveProperties = NULL;
    m_pPassiveProperties = NULL;
    m_nAddCost = 0;
    m_nUsableActivePropertyMask = 0;
    m_nCharges = DEFAULT_CHARGES;
    for (unsigned char i = 0; i < 8; i++)
    {
        m_aActivePropertyUsesLeft[i] = 0;
    }
    m_nBaseAC = 0;
    m_nStackSize = 1;
    m_nModelVariation = 1;
    m_bIdentified = 0;
    m_bInfinite = 0;
    m_bDropable = 1;
    m_bPickpocketable = 1;
    m_bStolen = 0;
    m_bNonEquippable = 0;
    m_bNewItem = 0;
    m_bDeleting = 0;
    m_nUpgrades = 0;
    m_nBodyVariation = 0;
    m_nTextureVar = 0;
}
// PC 0x00555720  Xbox 0x001D5990
int CSWSItem::AddToArea(CSWSArea* pArea, float fX, float fY, float fZ,
                        int bLoadingGame)
{
    Vector vPosition(fX, fY, fZ);
    int result;

    RemoveFromArea();
    result = (int)pArea;
    if (pArea != NULL)
    {
        SetArea(pArea);
        SetPosition(vPosition, 0, 1, 0);
        result = GetArea()->AddObjectToArea(m_nId, bLoadingGame);
    }

    return result;
}
// PC 0x00555790  Xbox 0x001D5A10
int CSWSItem::SaveItemProperties(CResGFF* pRes, CResStruct* pStruct)
{
    CResList list;
    CResStruct propertyStruct;
    CSWItemProperty* pItemProperty;
    int nProperty;

    pRes->AddList(&list, pStruct, "PropertiesList");
    for (nProperty = 0; nProperty < m_nActiveProperties; nProperty++)
    {
        pRes->AddListElement(&propertyStruct, &list, nProperty);
        pItemProperty = GetActiveProperty(nProperty);
        pRes->WriteFieldWORD(&propertyStruct, pItemProperty->m_nPropertyType,
                             "PropertyName");
        pRes->WriteFieldWORD(&propertyStruct, pItemProperty->m_nSubtype,
                             "Subtype");
        pRes->WriteFieldBYTE(&propertyStruct, pItemProperty->m_nCostTable,
                             "CostTable");
        pRes->WriteFieldWORD(&propertyStruct, pItemProperty->m_nCostValue,
                             "CostValue");
        pRes->WriteFieldBYTE(&propertyStruct, pItemProperty->m_nParam1,
                             "Param1");
        pRes->WriteFieldBYTE(&propertyStruct, pItemProperty->m_nParam1Value,
                             "Param1Value");
        pRes->WriteFieldBYTE(&propertyStruct, pItemProperty->m_nChanceAppear,
                             "ChanceAppear");
        pRes->WriteFieldBYTE(&propertyStruct, pItemProperty->m_nUsesPerDay,
                             "UsesPerDay");
        pRes->WriteFieldBYTE(&propertyStruct,
                             (unsigned char)pItemProperty->m_bUseable,
                             "Useable");
        pRes->WriteFieldBYTE(&propertyStruct, pItemProperty->m_nUpgradeType,
                             "UpgradeType");
    }

    for (nProperty = 0; nProperty < m_nPassiveProperties; nProperty++)
    {
        pRes->AddListElement(&propertyStruct, &list,
                             nProperty + m_nActiveProperties);
        pItemProperty = GetPassiveProperty(nProperty);
        pRes->WriteFieldWORD(&propertyStruct, pItemProperty->m_nPropertyType,
                             "PropertyName");
        pRes->WriteFieldWORD(&propertyStruct, pItemProperty->m_nSubtype,
                             "Subtype");
        pRes->WriteFieldBYTE(&propertyStruct, pItemProperty->m_nCostTable,
                             "CostTable");
        pRes->WriteFieldWORD(&propertyStruct, pItemProperty->m_nCostValue,
                             "CostValue");
        pRes->WriteFieldBYTE(&propertyStruct, pItemProperty->m_nParam1,
                             "Param1");
        pRes->WriteFieldBYTE(&propertyStruct, pItemProperty->m_nParam1Value,
                             "Param1Value");
        pRes->WriteFieldBYTE(&propertyStruct, pItemProperty->m_nChanceAppear,
                             "ChanceAppear");
        pRes->WriteFieldBYTE(&propertyStruct, pItemProperty->m_nUsesPerDay,
                             "UsesPerDay");
        pRes->WriteFieldBYTE(&propertyStruct,
                             (unsigned char)pItemProperty->m_bUseable,
                             "Useable");
        pRes->WriteFieldBYTE(&propertyStruct, pItemProperty->m_nUpgradeType,
                             "UpgradeType");
    }

    return 1;
}
// PC 0x00555A60  Xbox 0x001D5CE0
unsigned short CSWSItem::GetUsedActivePropertyUsesLeft(unsigned char nPropertyIndex)
{
    unsigned short nUsesLeft = 0;
    CSWItemProperty* pItemProperty = GetActiveProperty(nPropertyIndex);
    if (pItemProperty != NULL &&
        pItemProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_ACTIVATE_ITEM)
    {
        switch (pItemProperty->m_nCostValue)
        {
        case CSWItemProperty::ACTIVATE_ITEM_SINGLE_USE:
            if ((unsigned char)pItemProperty->m_bUseable)
                nUsesLeft = (unsigned char)m_nStackSize;
            break;
        case CSWItemProperty::ACTIVATE_ITEM_5_CHARGES:
            if ((unsigned char)pItemProperty->m_bUseable && m_nCharges > 0)
                nUsesLeft = m_nCharges / 5;
            break;
        case CSWItemProperty::ACTIVATE_ITEM_4_CHARGES:
            if ((unsigned char)pItemProperty->m_bUseable && m_nCharges > 0)
                nUsesLeft = m_nCharges / 4;
            break;
        case CSWItemProperty::ACTIVATE_ITEM_3_CHARGES:
            if ((unsigned char)pItemProperty->m_bUseable && m_nCharges > 0)
                nUsesLeft = m_nCharges / 3;
            break;
        case CSWItemProperty::ACTIVATE_ITEM_2_CHARGES:
            if ((unsigned char)pItemProperty->m_bUseable && m_nCharges > 0)
                nUsesLeft = m_nCharges / 2;
            break;
        case CSWItemProperty::ACTIVATE_ITEM_1_CHARGE:
            if ((unsigned char)pItemProperty->m_bUseable)
            {
                int nCharges = m_nCharges;
                if (nCharges > 0)
                    nUsesLeft = nCharges;
            }
            break;
        case CSWItemProperty::ACTIVATE_ITEM_1_USE_PER_DAY:
        case CSWItemProperty::ACTIVATE_ITEM_2_USES_PER_DAY:
        case CSWItemProperty::ACTIVATE_ITEM_3_USES_PER_DAY:
        case CSWItemProperty::ACTIVATE_ITEM_4_USES_PER_DAY:
        case CSWItemProperty::ACTIVATE_ITEM_5_USES_PER_DAY:
            if ((unsigned char)pItemProperty->m_bUseable && pItemProperty->m_nUsesPerDay > 0)
                nUsesLeft = pItemProperty->m_nUsesPerDay;
            break;
        case CSWItemProperty::ACTIVATE_ITEM_RECHARGE_1_MINUTE:
        case CSWItemProperty::ACTIVATE_ITEM_RECHARGE_2_MINUTES:
        case CSWItemProperty::ACTIVATE_ITEM_RECHARGE_3_MINUTES:
        case CSWItemProperty::ACTIVATE_ITEM_RECHARGE_4_MINUTES:
        case CSWItemProperty::ACTIVATE_ITEM_RECHARGE_5_MINUTES:
            if ((unsigned char)pItemProperty->m_bUseable)
                nUsesLeft = 1;
            break;
        }
    }
    return nUsesLeft;
}
// PC 0x00555BF0  Xbox 0x001D5E60
int CSWSItem::RestoreUsedActiveProperties(int bRestoreAll)
{
    int nActiveProperties;
    int nProperty;
    CSWItemProperty* pItemProperty;

    nActiveProperties = m_nActiveProperties;
    for (nProperty = 0; nProperty < nActiveProperties; nProperty++)
    {
        pItemProperty = GetActiveProperty(nProperty);
        if (pItemProperty != NULL && pItemProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_ACTIVATE_ITEM &&
            (bRestoreAll == 0 || pItemProperty->m_nUsesPerDay == 0xFF))
        {
            switch (pItemProperty->m_nCostValue)
            {
            case 8:
                pItemProperty->m_nUsesPerDay = 1;
                break;
            case 9:
                pItemProperty->m_nUsesPerDay = 2;
                break;
            case 10:
                pItemProperty->m_nUsesPerDay = 3;
                break;
            case 11:
                pItemProperty->m_nUsesPerDay = 4;
                break;
            case 12:
                pItemProperty->m_nUsesPerDay = 5;
                break;
            }
            pItemProperty->m_bUseable = 1;
        }
        nActiveProperties = m_nActiveProperties;
    }

    return nActiveProperties;
}
// PC 0x00555C90  Xbox 0x001D5F00
int CSWSItem::GetPropertyByTypeExists(unsigned short nPropertyType,
                                      unsigned short nSubtype)
{
    int nActiveProperties;
    int nProperty;
    CSWItemProperty* pItemProperty;
    int nPassiveProperties;

    nActiveProperties = m_nActiveProperties;
    for (nProperty = 0; nProperty < nActiveProperties; nProperty++)
    {
        pItemProperty = GetActiveProperty(nProperty);
        if ((pItemProperty->m_nUpgradeType == 0xFF ||
             ((1 << pItemProperty->m_nUpgradeType) & m_nUpgrades) != 0) &&
            GetActiveProperty(nProperty)->m_nPropertyType == nPropertyType &&
            (nSubtype == 0 ||
             GetActiveProperty(nProperty)->m_nSubtype == nSubtype))
        {
            return 1;
        }
        nActiveProperties = m_nActiveProperties;
    }

    nPassiveProperties = m_nPassiveProperties;
    for (nProperty = 0; nProperty < nPassiveProperties; nProperty++)
    {
        pItemProperty = GetPassiveProperty(nProperty);
        if ((pItemProperty->m_nUpgradeType == 0xFF ||
             ((1 << pItemProperty->m_nUpgradeType) & m_nUpgrades) != 0) &&
            GetPassiveProperty(nProperty)->m_nPropertyType == nPropertyType &&
            (nSubtype == 0 ||
             GetPassiveProperty(nProperty)->m_nSubtype == nSubtype))
        {
            return 1;
        }
        nPassiveProperties = m_nPassiveProperties;
    }

    return 0;
}
// PC 0x00555DE0  Xbox 0x001D6050
CItemRepository::~CItemRepository()
{
}
// PC 0x00555E00  Xbox 0x001D6070
int CItemRepository::FindItemWithBaseItemType(unsigned char nItemType, int nIndex)
{
    int nItem;
    int nMatches = 0;
    unsigned long oidItem;
    CSWSItem* pItem;
    CSWBaseItem* pBaseItem;
    CItemRepository* pRepository;
    CServerExoApp* pServerExoApp;
    int oidFound;

    for (nItem = 0; nItem < m_lstItems.num; nItem++)
    {
        oidItem = m_lstItems[nItem];
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        pItem = pServerExoApp->GetItemByGameObjectID(oidItem);
        if (pItem != NULL)
        {
            pRepository = pItem->m_pRepository;
            if (pRepository != NULL)
            {
                oidFound = pRepository->FindItemWithBaseItemType(nItemType, nIndex);
                if (oidFound != OBJECT_INVALID)
                {
                    return oidFound;
                }
            }

            pBaseItem = pItem->GetBaseItem();
            if (pBaseItem->m_nItemType == nItemType && nMatches == nIndex)
            {
                return oidItem;
            }
            pBaseItem = pItem->GetBaseItem();
            if (pBaseItem->m_nItemType == nItemType && nMatches != nIndex)
            {
                nMatches++;
            }
        }
    }

    return OBJECT_INVALID;
}
// PC 0x00555ED0  Xbox 0x001D6140
int CItemRepository::FindItemWithTag(const CExoString& sTag)
{
    int nItem;
    CSWSItem* pItem;
    CItemRepository* pRepository;
    CServerExoApp* pServerExoApp;
    int oidItem;

    for (nItem = 0; nItem < m_lstItems.num; nItem++)
    {
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        pItem = pServerExoApp->GetItemByGameObjectID(m_lstItems[nItem]);
        if (pItem != NULL)
        {
            if (pItem->m_sTag == sTag)
            {
                return pItem->m_nId;
            }

            pRepository = pItem->m_pRepository;
            if (pRepository != NULL)
            {
                oidItem = pRepository->FindItemWithTag(sTag);
                if (oidItem != OBJECT_INVALID)
                {
                    return oidItem;
                }
            }
        }
    }

    return OBJECT_INVALID;
}
// PC 0x00555F40  Xbox 0x001D61B0
int CItemRepository::GetItemInRepository(CSWSItem* pItem, int bFlag)
{
    CServerExoApp* pServerExoApp;
    CSWSItem* pRepositoryItem;
    CSWBaseItem* pBaseItem;
    CItemRepository* pSubRepository;

    for (int i = 0; i < m_lstItems.num; i++)
    {
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        pRepositoryItem =
            pServerExoApp->GetItemByGameObjectID(m_lstItems[i]);
        if (pItem == pRepositoryItem)
        {
            return 1;
        }

        if (bFlag != 0 && pRepositoryItem != NULL)
        {
            int nBaseItem = pRepositoryItem->m_nBaseItem;
            pBaseItem = g_pRules->m_pBaseItems->GetBaseItem(nBaseItem);
            if (pBaseItem != NULL && pBaseItem->m_bIsContainer != 0)
            {
                pSubRepository = pRepositoryItem->m_pRepository;
                if (pSubRepository != NULL &&
                    pSubRepository->GetItemInRepository(pItem, bFlag) != 0)
                {
                    return 1;
                }
            }
        }
    }

    return 0;
}
// PC 0x00555FD0  Xbox 0x001D6240
int CItemRepository::RemoveItem(CSWSItem* pItem)
{
    if (pItem != NULL)
    {
        for (int nItem = 0; nItem < m_lstItems.num; nItem++)
        {
            if (pItem->m_nId == m_lstItems.element[nItem])
            {
                m_lstItems.DelIndex(nItem);
                if (pItem->m_bNewItem)
                {
                    m_nNewItems--;
                    pItem->m_bNewItem = 0;
                }

                return 1;
            }
        }
    }

    return 0;
}
// PC 0x00556050  Xbox 0x001D62C0
void CItemRepository::ClearNewItems(CExoArrayList<unsigned long>* pNewItems)
{
    CServerExoApp* pServerExoApp;
    CSWSItem* pItem;

    for (int i = pNewItems->num - 1; i >= 0; i--)
    {
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        pItem = pServerExoApp->GetItemByGameObjectID(pNewItems->element[i]);
        if (pItem != NULL)
        {
            pItem->m_bNewItem = 0;
            m_nNewItems--;
            pNewItems->DelIndex(i);
        }
    }
}
// PC 0x005560B0  Xbox 0x001D6320
CSWSItem* CItemRepository::ItemListGetItem(int nIndex)
{
    CServerExoApp* pServerExoApp;
    CSWSItem* pItem;

    if (nIndex < 0 || nIndex >= m_lstItems.num)
    {
        return NULL;
    }

    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pItem = pServerExoApp->GetItemByGameObjectID(m_lstItems[nIndex]);
    return pItem;
}
// PC 0x005560E0  Xbox 0x001D6350
unsigned long CItemRepository::ItemListGetItemObjectID(int nIndex)
{
    if (nIndex < 0 || nIndex >= m_lstItems.num)
    {
        return OBJECT_INVALID;
    }

    return m_lstItems[nIndex];
}
// PC 0x00556100  Xbox 0x001D6370
int CItemRepository::CalculateContentsWeight()
{
    int nItems = m_lstItems.num;
    int nWeight = 0;

    for (int i = 0; i < nItems; i++)
    {
        CSWSItem* pItem = ItemListGetItem(i);
        if (pItem != NULL)
        {
            nWeight += pItem->GetWeight();
        }

        nItems = m_lstItems.num;
    }

    return nWeight;
}
// PC 0x00556150  Xbox 0x001D63C0
int CItemRepository::SetParentId(unsigned long oidParent)
{
    int nItems;
    int i;
    CServerExoApp* pServerExoApp;
    CSWSItem* pItem;

    m_oidOwner = oidParent;
    nItems = m_lstItems.num;
    for (i = 0; i < nItems; i++)
    {
        pServerExoApp = g_pAppManager->m_pServerExoApp;
        pItem = pServerExoApp->GetItemByGameObjectID(m_lstItems.element[i]);
        if (pItem != NULL)
        {
            pItem->SetPossessor(m_oidOwner, 0, 0, OBJECT_INVALID);
        }

        nItems = m_lstItems.num;
    }

    return nItems;
}
// PC 0x00553210  Xbox 0x001D3450
void CSWSItem::SetPossessor(unsigned long oidPossessor, int bRunScripts,
                          int bDisplayFeedback, unsigned long oidOldPossessor)
{
    unsigned long oidNewCreature = CGameObjectArray::INVALIDOBJID;
    unsigned long oidOldCreature = CGameObjectArray::INVALIDOBJID;
    unsigned long oidNewPlaceable = CGameObjectArray::INVALIDOBJID;
    unsigned long oidOldPlaceable = CGameObjectArray::INVALIDOBJID;
    unsigned long oidModule = g_pAppManager->GetServerExoApp()->GetModule()->GetId();

    if (m_oidPossessor == oidPossessor)
    {
        CScriptEvent* pEvent = new CScriptEvent;
        pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_ACQUIRE_ITEM;
        pEvent->SetObjectID(0, GetId());
        pEvent->SetObjectID(1, oidPossessor);
        g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
            0, 0, GetId(), oidModule, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
        return;
    }

    int bDifferentOldPossessor = 0;
    if (oidOldPossessor != CGameObjectArray::INVALIDOBJID)
    {
        if (m_oidPossessor != oidOldPossessor)
            bDifferentOldPossessor = 1;
        m_oidPossessor = oidOldPossessor;
    }

    if (m_oidPossessor != CGameObjectArray::INVALIDOBJID)
    {
        CGameObject* pObject;
        if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(
                m_oidPossessor, &pObject) == CGameObjectArray::SUCCESS && pObject != NULL)
        {
            if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
                oidOldCreature = m_oidPossessor;
            else if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_ITEM)
            {
                CSWSItem* pItem = pObject->AsSWSItem();
                unsigned long oidContainerPossessor = pItem->m_oidPossessor;
                if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(
                        oidContainerPossessor, &pObject) == CGameObjectArray::SUCCESS &&
                    pObject != NULL && pObject->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
                    oidOldCreature = pItem->m_oidPossessor;
            }
            else if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
                oidOldPlaceable = m_oidPossessor;
        }
    }
    if (oidPossessor != CGameObjectArray::INVALIDOBJID)
    {
        CGameObject* pObject;
        if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(
                oidPossessor, &pObject) == CGameObjectArray::SUCCESS && pObject != NULL)
        {
            if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
                oidNewCreature = oidPossessor;
            else if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_ITEM)
            {
                CSWSItem* pItem = pObject->AsSWSItem();
                unsigned long oidContainerPossessor = pItem->m_oidPossessor;
                if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(
                        oidContainerPossessor, &pObject) == CGameObjectArray::SUCCESS &&
                    pObject != NULL && pObject->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
                    oidNewCreature = pItem->m_oidPossessor;
            }
            else if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
                oidNewPlaceable = oidPossessor;
        }
    }

    if (!bDifferentOldPossessor && oidOldCreature != oidNewCreature)
    {
        if (oidOldCreature != CGameObjectArray::INVALIDOBJID)
        {
            if (bRunScripts == 1)
            {
                CScriptEvent* pEvent = new CScriptEvent;
                pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_LOSE_ITEM;
                pEvent->SetObjectID(0, GetId());
                g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                    0, 0, GetId(), oidModule, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
            }
            CSWSCreature* pCreature =
                g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidOldCreature);
            if (pCreature != NULL && bDisplayFeedback == 1)
            {
                CSWCCMessageData* pData = new CSWCCMessageData;
                pData->SetObjectID(0, GetId());
                pData->SetInteger(0, 1);
                pCreature->SendFeedbackMessage(CSWSCreature::FEEDBACK_ITEM_LOST, pData);
            }
        }
        if (oidNewCreature != CGameObjectArray::INVALIDOBJID)
        {
            CGameObject* pNewObject;
            if (g_pAppManager->GetServerExoApp()->GetObjectArray()->GetGameObject(
                    oidNewCreature, &pNewObject) == CGameObjectArray::SUCCESS &&
                pNewObject != NULL && pNewObject->AsSWSCreature()->m_pStats->GetIsPC())
            {
                SetDropable(1);
                m_bPickpocketable = 1;
            }
            if (bDisplayFeedback == 1)
            {
                CSWSCreature* pCreature =
                    g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidNewCreature);
                if (pCreature != NULL)
                {
                    CSWCCMessageData* pData = new CSWCCMessageData;
                    pData->SetObjectID(0, GetId());
                    pData->SetInteger(0, 1);
                    pCreature->SendFeedbackMessage(CSWSCreature::FEEDBACK_ITEM_RECEIVED, pData);
                }
            }
        }
    }
    if (oidNewCreature != CGameObjectArray::INVALIDOBJID && bRunScripts == 1)
    {
        CScriptEvent* pEvent = new CScriptEvent;
        pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_ACQUIRE_ITEM;
        pEvent->SetObjectID(0, GetId());
        pEvent->SetObjectID(1, oidNewCreature);
        g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
            0, 0, GetId(), oidModule, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
    }
    if (oidOldPlaceable != CGameObjectArray::INVALIDOBJID)
    {
        CSWSPlaceable* pPlaceable =
            g_pAppManager->GetServerExoApp()->GetPlaceableByGameObjectID(oidOldPlaceable);
        if (pPlaceable != NULL && pPlaceable->m_bDieWhenEmpty == 0)
        {
            unsigned long oidDisturber = CGameObjectArray::INVALIDOBJID;
            if (oidNewCreature != CGameObjectArray::INVALIDOBJID)
                oidDisturber = oidNewCreature;
            else if (oidNewPlaceable != CGameObjectArray::INVALIDOBJID)
                oidDisturber = oidNewPlaceable;
            if (oidDisturber != CGameObjectArray::INVALIDOBJID && bRunScripts == 1)
            {
                CScriptEvent* pEvent = new CScriptEvent;
                pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_INVENTORY_DISTURBED;
                pEvent->SetInteger(0, 1);
                pEvent->SetObjectID(0, GetId());
                g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                    0, 0, oidDisturber, oidOldPlaceable, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
            }
        }
    }
    if (oidNewPlaceable != CGameObjectArray::INVALIDOBJID)
    {
        CSWSPlaceable* pPlaceable =
            g_pAppManager->GetServerExoApp()->GetPlaceableByGameObjectID(oidNewPlaceable);
        if (pPlaceable != NULL && pPlaceable->m_bDieWhenEmpty == 0)
        {
            unsigned long oidDisturber = CGameObjectArray::INVALIDOBJID;
            if (oidOldCreature != CGameObjectArray::INVALIDOBJID)
                oidDisturber = oidOldCreature;
            else if (oidOldPlaceable != CGameObjectArray::INVALIDOBJID)
                oidDisturber = oidOldPlaceable;
            if (oidDisturber != CGameObjectArray::INVALIDOBJID && bRunScripts == 1)
            {
                CScriptEvent* pEvent = new CScriptEvent;
                pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_INVENTORY_DISTURBED;
                pEvent->SetInteger(0, 0);
                pEvent->SetObjectID(0, GetId());
                g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                    0, 0, oidDisturber, oidNewPlaceable, CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
            }
        }
    }
    m_oidPossessor = oidPossessor;
}
// PC 0x005538A0  Xbox 0x001D3AD0
void CSWSItem::RemoveFromArea()
{
    if (GetArea() != NULL)
    {
        GetArea()->RemoveObjectFromArea(m_nId);
    }

    SetArea(NULL);
    SetPosition(Vector(), 1, 1, 0);
}

void CSWSItem::SetNumActiveProperties(int nNumProperties)
{
    m_nActiveProperties = nNumProperties;
    if (m_pActiveProperties != NULL)
        delete[] m_pActiveProperties;
    if (nNumProperties != 0)
        m_pActiveProperties = new CSWItemProperty[m_nActiveProperties];
    else
        m_pActiveProperties = NULL;
}
// PC 0x00553900  Xbox 0x001D3B30
int CSWSItem::ActiveProperty(unsigned short nPropertyType)
{
    switch ((unsigned short)nPropertyType)
    {
    case 10:
    case 37:
    case 46:
    case 53:
        return 1;
    }

    return 0;
}
// PC 0x005561A0  Xbox 0x001D6410
void CSWSItem::OpenInventory(unsigned long oidOpener)
{
    NwsitemTrace1();
    if (m_bInventoryOpen != 0)
        return;

    int bIsContainer = g_pRules->m_pBaseItems->GetBaseItem(GetBaseItemType())->m_bIsContainer;
    if (bIsContainer != 0)
    {
        CSWSPlayer* pPlayer =
            g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(oidOpener);
        if (pPlayer != NULL)
        {
            unsigned int nItemCount = m_pRepository->m_lstItems.num;
            int bHasItems = nItemCount > 0;
            unsigned long oidObject = GetId();
            g_pAppManager->GetServerExoApp()->GetSWSMessage()
                ->SendServerToPlayerGuiContainerObject_Open(
                    pPlayer, oidObject, bHasItems);
        }

        CScriptEvent* pEvent = new CScriptEvent;
        pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_OPEN;
        g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
            0, 0, oidOpener, GetId(), CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
        m_bInventoryOpen = 1;
    }
}
// PC 0x005562A0  Xbox 0x001D6510
int CSWSItem::GetWeight()
{
    CItemRepository* pRepository = m_pRepository;
    int nWeight;
    int nStackSize;

    if (pRepository != NULL)
    {
        return m_nWeight + pRepository->CalculateContentsWeight();
    }

    nStackSize = m_nStackSize;
    nWeight = m_nWeight;
    if (nStackSize > 1)
    {
        nWeight *= nStackSize;
    }

    return nWeight;
}
// PC 0x005562D0  Xbox 0x001D6540
void CSWSItem::GetSortedPropertyStrings(CExoArrayList<CExoString>& aStrings,
                                        CExoString& sString)
{
    if (aStrings.num != 0)
    {
        CExoString sColon(":\n");
        CClientExoApp* pClientExoApp;
        pClientExoApp = g_pAppManager->m_pClientExoApp;
        sString = pClientExoApp->GetGUIString(
            CClientExoApp::GUI_STRREF_ITEM_PROPERTY_HEADING) + sColon;
    }

    while (aStrings.num != 0)
    {
        int nLowest = 0;
        for (int i = 1; i < aStrings.num; i++)
        {
            if (aStrings.element[i] < aStrings.element[nLowest])
            {
                nLowest = i;
            }
        }

        sString = sString + aStrings.element[nLowest] + "\n";
        aStrings.DelIndex(nLowest);
    }
}

inline int CSWBaseItem::GetRequiredFeatCount()
{
    return m_nRequiredFeatCount;
}
// PC 0x00556490  Xbox 0x001D6700
void CSWSItem::AddFeatRequirements(CExoString& sString)
{
    int bHeadingAdded = 0;
    CExoString sFeat;
    CExoString sFeatType;
    CExoString sFeatName;
    int nArmourFeat = CSWFeat::FEAT_ARMOUR_PROF_LIGHT;
    int nFeat;
    int nName;
    int nColon;
    int nProperty;
    CSWItemProperty* pProperty;

    for (int i = 0; i < GetBaseItem()->GetRequiredFeatCount(); ++i)
    {
        if (GetBaseItem()->GetRequiredFeat(i) == CSWFeat::FEAT_ARMOUR_PROF_MEDIUM &&
            nArmourFeat == CSWFeat::FEAT_ARMOUR_PROF_LIGHT)
            nArmourFeat = CSWFeat::FEAT_ARMOUR_PROF_MEDIUM;
        if (GetBaseItem()->GetRequiredFeat(i) == CSWFeat::FEAT_ARMOUR_PROF_HEAVY &&
            (nArmourFeat == CSWFeat::FEAT_ARMOUR_PROF_LIGHT ||
             nArmourFeat == CSWFeat::FEAT_ARMOUR_PROF_MEDIUM))
            nArmourFeat = CSWFeat::FEAT_ARMOUR_PROF_HEAVY;
    }

    for (i = 0; i < GetBaseItem()->GetRequiredFeatCount(); ++i)
    {
        nFeat = (unsigned short)GetBaseItem()->GetRequiredFeat(i);
        if (nFeat > 0 &&
            ((nFeat != CSWFeat::FEAT_ARMOUR_PROF_LIGHT &&
              nFeat != CSWFeat::FEAT_ARMOUR_PROF_MEDIUM &&
              nFeat != CSWFeat::FEAT_ARMOUR_PROF_HEAVY) || nFeat == nArmourFeat))
        {
            if (!bHeadingAdded)
            {
                sString = sString + g_pAppManager->GetClientExoApp()->GetGUIString(
                    CClientExoApp::GUI_STRREF_FEATS_REQUIRED) + ":\n";
                bHeadingAdded = 1;
            }
            {
                CExoString sColumn("NAME");
#ifdef _XBOX
                CTwoDimArrays* pArraysForFeat = g_pRules->Get2DArrays();
                EXOXBOXASSERT(pArraysForFeat->m_pFeat);
                C2DA* pFeatTable = pArraysForFeat->m_pFeat;
#else
                C2DA* pFeatTable = g_pRules->Get2DArrays()->m_pFeat;
#endif
                pFeatTable->GetINTEntry(nFeat, sColumn, &nName);
            }
            sFeat = g_pAppManager->GetClientExoApp()->GetGUIString(nName);
            nColon = sFeat.Find(':', 0);
            if (nColon != -1)
            {
                sFeatType = sFeat.Left(nColon);
                sFeatName = sFeat.Right(sFeat.GetLength() - nColon - 1);
                sFeat = CExoString(sFeatType) + " - " + CExoString(sFeatName);
            }
            sString = sString + sFeat + "\n";
        }
    }

    if (GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_USE_LIMITATION_FEAT, 0))
    {
        for (nProperty = 0; nProperty < m_nPassiveProperties; ++nProperty)
        {
            pProperty = GetPassiveProperty(nProperty);
            if ((pProperty->m_nUpgradeType == CSWItemProperty::UPGRADE_TYPE_NONE ||
                 ((CSWSItem::UPGRADE_PROPERTY_MASK << pProperty->m_nUpgradeType) & m_nUpgrades) != 0) &&
                pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_USE_LIMITATION_FEAT)
            {
                nFeat = pProperty->m_nSubtype;
                if (nFeat > 0)
                {
                    if (!bHeadingAdded)
                    {
                        sString = sString + g_pAppManager->GetClientExoApp()->GetGUIString(
                            CClientExoApp::GUI_STRREF_FEATS_REQUIRED) + ":\n";
                        bHeadingAdded = 1;
                    }
                    {
                        CExoString sColumn("NAME");
#ifdef _XBOX
                        CTwoDimArrays* pArraysForFeat = g_pRules->Get2DArrays();
                        EXOXBOXASSERT(pArraysForFeat->m_pFeat);
                        C2DA* pFeatTable = pArraysForFeat->m_pFeat;
#else
                        C2DA* pFeatTable = g_pRules->Get2DArrays()->m_pFeat;
#endif
                        pFeatTable->GetINTEntry(nFeat, sColumn, &nName);
                    }
                    sFeat = g_pAppManager->GetClientExoApp()->GetGUIString(nName);
                    nColon = sFeat.Find(':', 0);
                    if (nColon != -1)
                    {
                        sFeatType = sFeat.Left(nColon);
                        sFeatName = sFeat.Right(sFeat.GetLength() - nColon - 1);
                        sFeat = CExoString(sFeatType) + " - " + CExoString(sFeatName);
                    }
                    sString = sString + sFeat + "\n";
                }
            }
        }
    }
    if (bHeadingAdded)
        sString = sString + "\n";
}

inline int CSWSItem::GetPassiveProperties() { return m_nPassiveProperties; }
// PC 0x00556DE0  Xbox 0x001D7070
void CSWSItem::AddDamageProperties(CExoString& sString)
{
    CExoString sDamage;
    int nMinDamage, nMaxDamage;
    int i;
    CSWItemProperty* pProperty;
    sString = sString + g_pAppManager->GetClientExoApp()->GetGUIString(
        CClientExoApp::GUI_STRREF_DAMAGE) + ": ";
    if (GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_NO_DAMAGE, 0))
        sString = sString + "0\n";
    else
    {
        int nNumDice = g_pRules->m_pBaseItems->GetBaseItem(GetBaseItemType())->m_nNumDice;
        int nDieToRoll = g_pRules->m_pBaseItems->GetBaseItem(GetBaseItemType())->m_nDieToRoll;
        int nMinimum = nNumDice;
        int nMaximum = nNumDice * nDieToRoll;
        CExoString sDamageType;
        unsigned long nDamageFlags = GetDamageFlags();
        GetDamageTypeString(nDamageFlags, sDamageType);
        if (sDamageType != "")
            sString = sString + sDamageType + ", ";
        if (GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_DECREASED_DAMAGE, 0) ||
            GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_ENHANCEMENT_BONUS, 0) ||
            GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_DAMAGE_BONUS, 0))
        {
            for (i = 0; i < m_nPassiveProperties; i++)
            {
                pProperty = GetPassiveProperty(i);
                if (!pProperty ||
                    (pProperty->m_nUpgradeType != CSWItemProperty::UPGRADE_TYPE_NONE &&
                     ((UPGRADE_PROPERTY_MASK << pProperty->m_nUpgradeType) & m_nUpgrades) == 0))
                    continue;
                if (pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_ENHANCEMENT_BONUS)
                {
                    nMinimum += pProperty->GetCostValue();
                    nMaximum += pProperty->GetCostValue();
                }
                if (pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_DECREASED_DAMAGE)
                {
                    nMinimum -= pProperty->GetCostValue();
                    nMaximum -= pProperty->GetCostValue();
                }
                if (pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_DAMAGE_BONUS)
                {
                    nDamageFlags = GetDamageFlags();
                    unsigned short nSubtype = pProperty->m_nSubtype;
                    unsigned char nTypeBits = (unsigned char)(nDamageFlags >> nSubtype);
                    if (nTypeBits & 1)
                    {
                        GetPropertyDamageRange(pProperty->GetCostValue(), nMinDamage, nMaxDamage);
                        nMinimum += nMinDamage;
                        nMaximum += nMaxDamage;
                    }
                }
            }
        }
        sDamage.Format("%d-%d\n\n", nMinimum, nMaximum);
        sString = sString + sDamage.CStr();
    }

    CExoString sBonus;
    int bFound = 0;
    int nMinimum = 0;
    int nMaximum = 0;
    int nLastProperty = -1;
    CExoString sName;
    CExoString sSubtype;
    CExoString sCost;
    CExoString sParam1;
    CExoString sProperty;
    char sType[32];
    int nBonus;
    if (GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_DAMAGE_BONUS_VS_ALIGNMENT_GROUP, 0))
    {
        for (i = 0; i < GetPassiveProperties(); i++)
        {
            pProperty = GetPassiveProperty(i);
            if (pProperty &&
                (pProperty->m_nUpgradeType == CSWItemProperty::UPGRADE_TYPE_NONE ||
                 ((UPGRADE_PROPERTY_MASK << pProperty->m_nUpgradeType) & m_nUpgrades) != 0))
            {
                if (pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_DAMAGE_BONUS_VS_ALIGNMENT_GROUP)
                {
                    nLastProperty = i;
                    int nCostValue = pProperty->m_nCostValue;
                    GetPropertyDamageRange(nCostValue, nMinDamage, nMaxDamage);
                    nMinimum += nMinDamage;
                    nMaximum += nMaxDamage;
                }
                bFound = 1;
            }
        }
        if (bFound)
        {
            pProperty = GetPassiveProperty(nLastProperty);
            unsigned char nParam1Value = pProperty->m_nParam1Value;
            unsigned short nSubtype = pProperty->m_nSubtype;
            unsigned short nType = pProperty->m_nPropertyType;
            GetPropertyStrings(nType, nSubtype,
                CSWItemProperty::COST_VALUE_NONE, nParam1Value,
                sName, sSubtype, sCost, sParam1);
            if (nMinimum == nMaximum) sCost.Format("+%d", nMinimum);
            else sCost.Format("+%d-%d", nMinimum, nMaximum);
            sString = sString + sParam1 + ": " + sCost + " " +
                g_pAppManager->GetClientExoApp()->GetGUIString(GUI_STRREF_VERSUS) +
                " " + sSubtype + "\n\n";
        }
    }
    {
        bFound = 0;
        nMaximum = 0;
        int nMinimum = 0;
        if (GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_DAMAGE_BONUS_VS_RACIAL_GROUP, 0))
        {
            for (i = 0; i < m_nPassiveProperties; i++)
            {
                pProperty = GetPassiveProperty(i);
                if (pProperty &&
                    (pProperty->m_nUpgradeType == CSWItemProperty::UPGRADE_TYPE_NONE ||
                     ((UPGRADE_PROPERTY_MASK << pProperty->m_nUpgradeType) & m_nUpgrades) != 0))
                {
                    if (pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_DAMAGE_BONUS_VS_RACIAL_GROUP)
                    {
                        nLastProperty = i;
                        int nCostValue = pProperty->m_nCostValue;
                        GetPropertyDamageRange(nCostValue, nMinDamage, nMaxDamage);
                        nMinimum += nMinDamage;
                        nMaximum += nMaxDamage;
                    }
                    bFound = 1;
                }
            }
            if (bFound)
            {
                pProperty = GetPassiveProperty(nLastProperty);
                unsigned short nSubtype = pProperty->m_nSubtype;
                unsigned char nParam1Value = pProperty->m_nParam1Value;
                unsigned short nType = pProperty->m_nPropertyType;
                GetPropertyStrings(nType, nSubtype,
                    CSWItemProperty::COST_VALUE_NONE, nParam1Value,
                    sName, sSubtype, sCost, sParam1);
                if (nMinimum == nMaximum) sCost.Format("+%d", nMinimum);
                else sCost.Format("+%d-%d", nMinimum, nMaximum);
                sString = sString + sParam1 + ": " + sCost + " " +
                    g_pAppManager->GetClientExoApp()->GetGUIString(GUI_STRREF_VERSUS) +
                    " " + sSubtype + "\n\n";
            }
        }
    }
    if (GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_DAMAGE_REDUCTION, 0))
    {
        for (i = 0; i < m_nPassiveProperties; i++)
        {
            pProperty = GetPassiveProperty(i);
            if (pProperty &&
                (pProperty->m_nUpgradeType == CSWItemProperty::UPGRADE_TYPE_NONE ||
                 ((UPGRADE_PROPERTY_MASK << pProperty->m_nUpgradeType) & m_nUpgrades) != 0))
            {
                CExoString sName, sSubtype, sCost, sParam1, sProperty;
                switch (pProperty->m_nPropertyType)
                {
                case CSWItemProperty::ITEM_PROPERTY_DAMAGE_REDUCTION:
                    if (!GetExistsHigherValuedProperty(pProperty->m_nPropertyType,
                            pProperty->m_nSubtype, pProperty->m_nCostValue))
                    {
                        GetPropertyStrings(pProperty->m_nPropertyType, pProperty->m_nSubtype,
                            pProperty->m_nCostValue, CSWItemProperty::PARAM1_VALUE_NONE,
                            sName, sSubtype, sCost, sParam1);
                        sscanf(sSubtype.CStr(), "%s +%d", sType, &nBonus);
                        sBonus.Format("%d", nBonus);
                        sString = sString + sName + ": +" + sBonus;
                        sBonus.Format("%s", sCost);
                        sString = sString + " (" + sCost + ")\n\n";
                    }
                    break;
                }
            }
        }
    }
    int bHeadingAdded = 0;
    nMinimum = 0;
    nMaximum = 0;
    if (GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_DAMAGE_BONUS, 0))
    {
        for (i = 0; i < m_nPassiveProperties; i++)
        {
            pProperty = GetPassiveProperty(i);
            if (!pProperty ||
                (pProperty->m_nUpgradeType != CSWItemProperty::UPGRADE_TYPE_NONE &&
                 ((UPGRADE_PROPERTY_MASK << pProperty->m_nUpgradeType) & m_nUpgrades) == 0))
                continue;
            CExoString sName, sSubtype, sCost, sParam1, sProperty;
            if (pProperty->m_nPropertyType != CSWItemProperty::ITEM_PROPERTY_DAMAGE_BONUS)
                continue;
            unsigned long nDamageFlags = GetDamageFlags();
            if ((nDamageFlags >> pProperty->m_nSubtype) & 1) continue;
            unsigned short nCostValue = pProperty->m_nCostValue;
            unsigned short nSubtype = pProperty->m_nSubtype;
            unsigned short nType = pProperty->m_nPropertyType;
            GetPropertyStrings(nType, nSubtype,
                nCostValue, CSWItemProperty::PARAM1_VALUE_NONE,
                sName, sSubtype, sCost, sParam1);
            if (!bHeadingAdded) sString = sString + sName + ": ";
            for (int j = 0; j < m_nPassiveProperties; j++)
            {
                CSWItemProperty* pOther = GetPassiveProperty(j);
                int nPropertySubtype = pProperty->m_nSubtype;
                int nOtherSubtype = pOther->m_nSubtype;
                if ((pOther->m_nUpgradeType == CSWItemProperty::UPGRADE_TYPE_NONE ||
                     ((UPGRADE_PROPERTY_MASK << pOther->m_nUpgradeType) & m_nUpgrades) != 0) &&
                    pOther->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_DAMAGE_BONUS &&
                    (pProperty->m_nSubtype == pOther->m_nSubtype ||
                     ((nPropertySubtype == CSWItemProperty::DAMAGE_BONUS_SUBTYPE_BLUDGEONING ||
                       nPropertySubtype == CSWItemProperty::DAMAGE_BONUS_SUBTYPE_PIERCING ||
                       nPropertySubtype == CSWItemProperty::DAMAGE_BONUS_SUBTYPE_SLASHING) &&
                      (nOtherSubtype == CSWItemProperty::DAMAGE_BONUS_SUBTYPE_BLUDGEONING ||
                       nOtherSubtype == CSWItemProperty::DAMAGE_BONUS_SUBTYPE_PIERCING ||
                       nOtherSubtype == CSWItemProperty::DAMAGE_BONUS_SUBTYPE_SLASHING))))
                {
                    if (j < i) goto NextDamageProperty;
                    unsigned short nOtherCost = pOther->m_nCostValue;
                    GetPropertyDamageRange(nOtherCost, nMinDamage, nMaxDamage);
                    if (nMinDamage == nMaxDamage)
                    {
                        nMinimum += nMinDamage;
                        nMaximum += nMinDamage;
                    }
                    else
                    {
                        nMinimum += nMinDamage;
                        nMaximum += nMaxDamage;
                    }
                }
            }
            if (nMinimum == nMaximum) sBonus.Format("+%d", nMinimum);
            else sBonus.Format("+%d-%d", nMinimum, nMaximum);
            if (pProperty->m_nSubtype == CSWItemProperty::DAMAGE_BONUS_SUBTYPE_BLUDGEONING ||
                pProperty->m_nSubtype == CSWItemProperty::DAMAGE_BONUS_SUBTYPE_PIERCING ||
                pProperty->m_nSubtype == CSWItemProperty::DAMAGE_BONUS_SUBTYPE_SLASHING)
                sSubtype = g_pAppManager->GetClientExoApp()->GetGUIString(GUI_STRREF_DAMAGE_PHYSICAL);
            if (!bHeadingAdded)
            {
                sString = sString + sBonus + " " + sSubtype;
                bHeadingAdded = 1;
            }
            else sString = sString + ", " + sBonus + " " + sSubtype;
            nMinimum = 0;
            nMaximum = 0;
NextDamageProperty:
            ;
        }
        if (bHeadingAdded) sString = sString + "\n\n";
    }
}
// PC 0x0055D290  Xbox 0x001DD3C0
CItemRepository::CItemRepository(unsigned long oidOwner, int bFlag)
    : m_lstItems(0)
{
    m_oidOwner = oidOwner;
    m_nNewItems = 0;
    m_bUnknown08 = bFlag;
}
// PC 0x0055D2C0  Xbox 0x001DD3F0
void CItemRepository::InsertItem(CSWSItem* pItem, int nIndex)
{
    if (m_bUnknown08)
    {
        return;
    }

    if (nIndex < 0 || nIndex >= m_lstItems.num)
    {
        m_lstItems.Add(pItem->m_nId);
    }
    else
    {
        m_lstItems.Insert(pItem->m_nId, nIndex);
    }
}
// PC 0x0055D330  Xbox 0x001DD460
int CItemRepository::AddItem(CSWSItem** ppItem, int bMergeItem,
                             int bUpdatePossessor, int bNewItem)
{
    CGameObject* pOwner;
    CSWSItem* pContainer;
    unsigned long oidPossessor;
    CSWSCreature* pCreature;
    CSWSItem* pItem;
    CSWSPlayer* pPlayer;
    int bFoundPosition;
    int nPosition;
    int nLanguage;
    int nItem;

    pOwner = g_pAppManager->GetServerExoApp()->GetGameObject(m_oidOwner);
    if (pOwner != NULL)
    {
        if (pOwner->AsSWSCreature() != NULL)
        {
            if ((*ppItem)->GetBaseItem()->m_nItemType == CSWBaseItem::ITEM_TYPE_CREDITS)
            {
                int nGold = (*ppItem)->m_nStackSize;
                pOwner->AsSWSCreature()->AddGold(nGold, 1);
                delete *ppItem;
                *ppItem = NULL;
                return 1;
            }
            else if ((*ppItem)->GetBaseItem()->m_nItemType == CSWBaseItem::ITEM_TYPE_PAZAAK_CARD)
            {
                pOwner->AsSWSCreature()->AddPazaakCard(*ppItem);
                delete *ppItem;
                *ppItem = NULL;
                return 1;
            }
        }
        else if (pOwner->AsSWSItem() != NULL)
        {
            pContainer = pOwner->AsSWSItem();
            if (g_pRules->GetBaseItemArray()->GetBaseItem(pContainer->GetBaseItemType())
                    ->m_bIsContainer == 1)
            {
                oidPossessor = pContainer->m_oidPossessor;
                if (oidPossessor != OBJECT_INVALID)
                {
                    pCreature = g_pAppManager->GetServerExoApp()
                                    ->GetCreatureByGameObjectID(oidPossessor);
                    if (pCreature != NULL)
                    {
                        if ((*ppItem)->GetBaseItem()->m_nItemType == CSWBaseItem::ITEM_TYPE_CREDITS)
                        {
                            pCreature->AddGold((*ppItem)->m_nStackSize, 1);
                            delete *ppItem;
                            *ppItem = NULL;
                            return 1;
                        }
                        else if ((*ppItem)->GetBaseItem()->m_nItemType == CSWBaseItem::ITEM_TYPE_PAZAAK_CARD)
                        {
                            pCreature->AddPazaakCard(*ppItem);
                            delete *ppItem;
                            *ppItem = NULL;
                            return 1;
                        }
                    }
                }
            }
        }
    }

    nPosition = 0;
    bFoundPosition = 0;
    nLanguage = g_pAppManager->GetClientExoApp()->GetClientLanguage();
    CExoString sName;
    CExoString sItemName;
    if (m_bUnknown08)
    {
        (*ppItem)->m_sLocalizedName.GetString(nLanguage, &sName, 0);
    }

    if (bMergeItem == 1)
    {
        for (nItem = 0; nItem < m_lstItems.num; nItem++)
        {
            unsigned long oidItem = m_lstItems[nItem];
            pItem = g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(oidItem);
            if (pItem != NULL)
            {
                if (m_bUnknown08 && !bFoundPosition)
                {
                    pItem->m_sLocalizedName.GetString(nLanguage, &sItemName, 0);
                    if ((unsigned long)(*ppItem)->m_nBaseItem <
                            (unsigned long)pItem->m_nBaseItem ||
                        ((*ppItem)->m_nBaseItem == pItem->m_nBaseItem &&
                         sName < sItemName))
                    {
                        nPosition = nItem;
                        bFoundPosition = 1;
                    }
                }

#ifdef _XBOX

                if (pItem->GetBaseItemType() == (*ppItem)->GetBaseItemType() &&
                    pItem->GetModelVariation() == (*ppItem)->GetModelVariation() &&
                    (pItem->m_nId != (*ppItem)->m_nId || (ExoXboxRestart(), 1)) &&
                    pItem->MergeItem(*ppItem) == 1)
#else
                if (pItem->GetBaseItemType() == (*ppItem)->GetBaseItemType() &&
                    pItem->GetModelVariation() == (*ppItem)->GetModelVariation() &&
                    pItem->MergeItem(*ppItem) == 1)
#endif
                {
                    if (pOwner != NULL)
                    {
                        pPlayer = NULL;
                        if (pOwner->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
                        {
                            pPlayer = g_pAppManager->GetServerExoApp()
                                          ->GetClientObjectByObjectId(m_oidOwner);
                        }
                        else if (pOwner->m_nObjectType == CGameObject::OBJECT_TYPE_ITEM)
                        {
                            pContainer = pOwner->AsSWSItem();
                            pPlayer = g_pAppManager->GetServerExoApp()
                                          ->GetClientObjectByObjectId(pContainer->GetPossessor());
                        }
                        if (pPlayer != NULL)
                        {
                            g_pAppManager->GetServerExoApp()->GetSWSMessage()
                                ->SendServerPlayerItemUpdate_DestroyItem(
                                    pPlayer, (*ppItem)->GetId());
                        }
                    }

                    delete *ppItem;
                    *ppItem = pItem;
                    oidPossessor = pItem->m_oidPossessor;
                    pItem->SetPossessor(oidPossessor, bUpdatePossessor,
                                        bUpdatePossessor, OBJECT_INVALID);
                    if (bNewItem)
                    {
                        (*ppItem)->m_bNewItem = 1;
                        m_nNewItems++;
                    }
                    return 1;
                }
            }
        }
    }
    else if (m_bUnknown08)
    {
        for (nItem = 0; nItem < m_lstItems.num; nItem++)
        {
            unsigned long oidItem = m_lstItems[nItem];
            pItem = g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(oidItem);
            if (pItem != NULL && m_bUnknown08)
            {
                pItem->m_sLocalizedName.GetString(nLanguage, &sItemName, 0);
                if (sName < sItemName)
                {
                    nPosition = nItem;
                    bFoundPosition = 1;
                    break;
                }
            }
        }
    }

    CExoArrayList<CSWSItem*> lstAddedItems;
    lstAddedItems.Add(*ppItem);
    if (m_bUnknown08)
    {
        pPlayer = g_pAppManager->GetServerExoApp()->GetClientObjectByPlayerId(0, 1);
        if (pPlayer != NULL)
        {
            oidPossessor = pPlayer->m_oidPCObject;
            (*ppItem)->SetPossessor(oidPossessor, bUpdatePossessor,
                                    bUpdatePossessor, OBJECT_INVALID);
        }
        if (bFoundPosition)
        {
            m_lstItems.Insert((*ppItem)->GetId(), nPosition);
        }
        else
        {
            m_lstItems.Add((*ppItem)->GetId());
        }
    }
    else
    {
        m_lstItems.Insert((*ppItem)->GetId(), 0);
    }

    if (bNewItem)
    {
        (*ppItem)->m_bNewItem = 1;
    }
    if ((*ppItem)->m_bNewItem)
    {
        m_nNewItems++;
    }
    return 1;
}
// PC 0x0055D800  Xbox 0x001DD930
void CSWSItem::CloseInventory(unsigned long oidCloser, int bUpdatePlayer)
{
    if (m_bInventoryOpen)
    {
        if (bUpdatePlayer)
        {
            CSWSPlayer* pPlayer = g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(oidCloser);
            if (pPlayer != NULL)
            {
                CSWSCreature* pCreature = pPlayer->GetGameObject()->AsSWSCreature();
                while (m_pRepository->m_lstItems.num > 0u)
                {
                    unsigned int nItemCount = m_pRepository->m_lstItems.num;
                    CSWSItem* pItem = m_pRepository->ItemListGetItem(nItemCount - 1);
                    m_pRepository->RemoveItem(pItem);
                    pCreature->GetItemRepository(1)->AddItem(&pItem, 1, 0, 1);
                }
            }
        }

        CScriptEvent* pEvent = new CScriptEvent;
        pEvent->m_nType = CSWSSCRIPTEVENT_EVENTTYPE_ON_CLOSE;
        g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
            0, 0, oidCloser, GetId(), CServerAIMaster::EVENT_SIGNAL_EVENT, pEvent);
        m_bInventoryOpen = 0;
    }
}
// PC 0x0055D950  Xbox 0x001DDA80
int CSWSItem::CopyItem(CSWSItem* pItem)
{
    if (pItem == NULL)
    {
        return 0;
    }
    if (pItem->m_pRepository != NULL &&
        (unsigned int)pItem->m_pRepository->GetNumItems() > 0)
    {
        return 0;
    }

    m_sDescIdentified = pItem->m_sDescIdentified;
    m_sDescription = pItem->m_sDescription;
    m_sLocalizedName = pItem->m_sLocalizedName;
    m_sTag = pItem->m_sTag;
    m_oidPossessor = pItem->GetPossessor();
    if (pItem->m_pRepository != NULL)
    {
        m_pRepository = new CItemRepository(GetId(), 0);
    }
    int nProperty;
    m_nModelVariation = pItem->m_nModelVariation;
    m_nActiveProperties = pItem->m_nActiveProperties;
    if (m_nActiveProperties > 0)
    {
        m_pActiveProperties = new CSWItemProperty[m_nActiveProperties];
        for (nProperty = 0; nProperty < m_nActiveProperties; nProperty++)
        {
            m_pActiveProperties[nProperty] = pItem->m_pActiveProperties[nProperty];
        }
    }
    m_nPassiveProperties = pItem->m_nPassiveProperties;
    if (m_nPassiveProperties > 0)
    {
        m_pPassiveProperties = new CSWItemProperty[m_nPassiveProperties];
        for (nProperty = 0; nProperty < m_nPassiveProperties; nProperty++)
        {
            m_pPassiveProperties[nProperty] = pItem->m_pPassiveProperties[nProperty];
        }
    }
    m_nAddCost = pItem->m_nAddCost;
    m_nCharges = pItem->m_nCharges;
    m_nMaxCharges = pItem->m_nMaxCharges;
    m_nBaseAC = pItem->m_nBaseAC;
    m_bInfinite = pItem->m_bInfinite;
    m_nStackSize = pItem->m_nStackSize;
    m_nWeight = pItem->m_nWeight;
    m_bIdentified = pItem->m_bIdentified;
    m_nUpgrades = pItem->m_nUpgrades;
    m_nBodyVariation = pItem->m_nBodyVariation;
    m_nTextureVar = pItem->m_nTextureVar;
    m_bNonEquippable = pItem->m_bNonEquippable;
    m_nBaseItem = pItem->GetBaseItemType();
    for (int i = 0; i < 6; i++)
    {
        m_aUnknown04[i] = pItem->m_aUnknown04[i];
    }
    g_pAppManager->GetServerExoApp()->GetModule()->AddObjectToLookupTable(
        m_sTag, GetId());
    return 1;
}
// PC 0x0055DC10  Xbox 0x001DDD50
void CSWSItem::CloseItemForAllPlayers()
{
    CSWBaseItem* pBaseItem;
    CExoLinkedList<CSWSClient>* pPlayerList;
    CExoLinkedListNode* pPosition;
    CSWSPlayer* pPlayer;
    CSWSObject* pPlayerObject;
    int nBaseItem;

    nBaseItem = m_nBaseItem;
    pBaseItem = g_pRules->m_pBaseItems->GetBaseItem(nBaseItem);
    if (pBaseItem != NULL && pBaseItem->m_bIsContainer != 0)
    {
        pPlayerList = g_pAppManager->m_pServerExoApp->GetPlayerList();
        for (pPosition = pPlayerList->GetHeadPos(); pPosition != NULL;
             pPlayerList->GetNext(pPosition))
        {
            pPlayer =
                (CSWSPlayer*)pPlayerList->GetAtPos(pPosition);
            if (pPlayer != NULL)
            {
                pPlayerObject = pPlayer->GetGameObject();
                if (pPlayerObject != NULL &&
                    pPlayer->m_pContainerGUI->m_oidContainer == m_nId)
                {
                    CloseInventory(pPlayerObject->m_nId, 1);
                }
            }
        }
    }
}
// PC 0x0055DCA0  Xbox 0x001DDDE0
int CSWSItem::AcquireItem(CSWSItem** ppItem, unsigned long oidPossessor,
                          int bDisplayFeedback)
{
    CGameObjectArray* pObjectArray =
        g_pAppManager->m_pServerExoApp->GetObjectArray();
    CSWSItem* pContainer = NULL;
    CItemRepository* pRepository;
    CGameObject* pPossessor;
    int bResult;

    if ((*ppItem)->m_oidPossessor != oidPossessor)
    {
        pContainer = g_pAppManager->m_pServerExoApp->GetItemByGameObjectID(
            (*ppItem)->m_oidPossessor);
        if (pContainer == NULL || pContainer->m_oidPossessor != oidPossessor)
        {
            return 0;
        }
        oidPossessor = (*ppItem)->m_oidPossessor;
    }

    pRepository = m_pRepository;
    pPossessor = g_pAppManager->m_pServerExoApp->GetGameObject(oidPossessor);
    if (pPossessor != NULL)
    {
        if (pPossessor->m_nObjectType == SW_OBJECT_TYPE_CREATURE)
        {
            pPossessor->AsSWSCreature()->RemoveItem(*ppItem, 1,
                                                    bDisplayFeedback, NULL);
        }
        else if (pPossessor->m_nObjectType == SW_OBJECT_TYPE_PLACEABLE)
        {
            pPossessor->AsSWSPlaceable()->RemoveItem(*ppItem);
        }
        else if (pPossessor->m_nObjectType == SW_OBJECT_TYPE_ITEM &&
                 pPossessor->AsSWSItem()->m_pRepository != NULL)
        {
            CSWSItem* pItem = pPossessor->AsSWSItem();
            pItem->m_pRepository->RemoveItem(*ppItem);
        }
    }
    else if (pContainer != NULL &&
             g_pRules->m_pBaseItems->GetBaseItem((*ppItem)->GetBaseItemType())
                 ->m_bIsContainer &&
             g_pRules->m_pBaseItems->GetBaseItem(pContainer->GetBaseItemType())
                 ->m_bIsContainer)
    {
        return 0;
    }

    bResult = pRepository->AddItem(ppItem, 1, 0, 1);
    if (bResult == 1)
    {
        if (pPossessor == NULL && *ppItem != NULL)
        {
            (*ppItem)->RemoveFromArea();
        }
        if (*ppItem != NULL)
        {
            (*ppItem)->SetPossessor(m_nId, 1, bDisplayFeedback, OBJECT_INVALID);
        }
    }
    return bResult;
}
// PC 0x0055DE20  Xbox 0x001DDF60
void CSWSItem::DecrementUses(int nPropertyIndex, unsigned long oidUser)
{

    if (g_pAppManager->GetClientExoApp()->GetClientOptions()->m_nDebugFlags != 0)
    {
        return;
    }

    CSWItemProperty* pProperty = GetActiveProperty(nPropertyIndex);
    int bChargeBased = 0;
    int bDestroy = 0;
    int bRecharges = 0;
    int nCharges;
    int nProperty;
    unsigned long nCalendarDay;
    unsigned long nTimeOfDay;

    switch (pProperty->m_nCostValue)
    {
    case CSWItemProperty::ACTIVATE_ITEM_SINGLE_USE:
        if (m_nStackSize > 1)
        {
            m_nStackSize--;
        }
        else
        {
            pProperty->m_bUseable = 0;
            bDestroy = 1;
        }
        break;
    case CSWItemProperty::ACTIVATE_ITEM_5_CHARGES:
        bChargeBased = 1;
        nCharges = 5;
        break;
    case CSWItemProperty::ACTIVATE_ITEM_4_CHARGES:
        bChargeBased = 1;
        nCharges = 4;
        break;
    case CSWItemProperty::ACTIVATE_ITEM_3_CHARGES:
        bChargeBased = 1;
        nCharges = 3;
        break;
    case CSWItemProperty::ACTIVATE_ITEM_2_CHARGES:
        bChargeBased = 1;
        nCharges = 2;
        break;
    case CSWItemProperty::ACTIVATE_ITEM_1_CHARGE:
        bChargeBased = 1;
        nCharges = 1;
        break;
    case CSWItemProperty::ACTIVATE_ITEM_1_USE_PER_DAY:
    case CSWItemProperty::ACTIVATE_ITEM_2_USES_PER_DAY:
    case CSWItemProperty::ACTIVATE_ITEM_3_USES_PER_DAY:
    case CSWItemProperty::ACTIVATE_ITEM_4_USES_PER_DAY:
    case CSWItemProperty::ACTIVATE_ITEM_5_USES_PER_DAY:
        bRecharges = 1;
        if (pProperty->m_nUsesPerDay > 0)
        {
            pProperty->m_nUsesPerDay--;
        }
        if (pProperty->m_nUsesPerDay < 1)
        {
            pProperty->m_bUseable = 0;
        }
        break;
    case CSWItemProperty::ACTIVATE_ITEM_RECHARGE_1_MINUTE:
    case CSWItemProperty::ACTIVATE_ITEM_RECHARGE_2_MINUTES:
    case CSWItemProperty::ACTIVATE_ITEM_RECHARGE_3_MINUTES:
    case CSWItemProperty::ACTIVATE_ITEM_RECHARGE_4_MINUTES:
    case CSWItemProperty::ACTIVATE_ITEM_RECHARGE_5_MINUTES:
        bRecharges = 1;
        g_pAppManager->GetServerExoApp()->GetWorldTimer()->GetWorldTime(
            &nCalendarDay, &nTimeOfDay);
        pProperty->m_nLastUseDay = nCalendarDay;
        pProperty->m_nLastUseTime = nTimeOfDay;
        pProperty->m_bUseable = 0;
        break;
    }

    if (bChargeBased)
    {
        if (m_nCharges >= nCharges)
        {
            m_nCharges -= nCharges;
        }
        for (nProperty = 0; nProperty < m_nActiveProperties; nProperty++)
        {
            pProperty = GetActiveProperty(nProperty);
            if (pProperty != NULL &&
                pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_ACTIVATE_ITEM)
            {
                int nCostValue = pProperty->m_nCostValue;
                if ((nCostValue == CSWItemProperty::ACTIVATE_ITEM_5_CHARGES &&
                     m_nCharges < 5) ||
                    (nCostValue == CSWItemProperty::ACTIVATE_ITEM_4_CHARGES &&
                     m_nCharges < 4) ||
                    (nCostValue == CSWItemProperty::ACTIVATE_ITEM_3_CHARGES &&
                     m_nCharges < 3) ||
                    (nCostValue == CSWItemProperty::ACTIVATE_ITEM_2_CHARGES &&
                     m_nCharges < 2) ||
                    (nCostValue == CSWItemProperty::ACTIVATE_ITEM_1_CHARGE &&
                     m_nCharges < 1))
                {
                    pProperty->m_bUseable = 0;
                }
            }
        }
    }

    if ((unsigned char)pProperty->m_bUseable == 0)
    {
        if (GetBaseItem()->m_nItemType == CSWBaseItem::ITEM_TYPE_STIMULANT ||
            GetBaseItem()->m_nItemType == CSWBaseItem::ITEM_TYPE_REPAIR_KIT ||
            GetBaseItem()->m_nItemType == CSWBaseItem::ITEM_TYPE_SQUAD_MEDPAC)
        {
            bDestroy = 1;
        }
        else if (bChargeBased)
        {
            int nProperties = m_nActiveProperties;
            for (nProperty = 0; nProperty < m_nActiveProperties; nProperty++)
            {
                CSWItemProperty* pChargeProperty = GetActiveProperty(nProperty);
                if (pChargeProperty != NULL &&
                    pChargeProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_ACTIVATE_ITEM)
                {
                    int nCostValue = pChargeProperty->m_nCostValue;
                    if ((nCostValue == CSWItemProperty::ACTIVATE_ITEM_5_CHARGES ||
                         nCostValue == CSWItemProperty::ACTIVATE_ITEM_4_CHARGES ||
                         nCostValue == CSWItemProperty::ACTIVATE_ITEM_3_CHARGES ||
                         nCostValue == CSWItemProperty::ACTIVATE_ITEM_2_CHARGES ||
                         nCostValue == CSWItemProperty::ACTIVATE_ITEM_1_CHARGE) &&
                        (unsigned char)pChargeProperty->m_bUseable != 0)
                    {
                        break;
                    }
                }
            }
            if (nProperty == nProperties)
            {
                bDestroy = 1;
            }
        }
    }

    if (bDestroy && !bRecharges)
    {
        CSWSPlayer* pPlayer =
            g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(oidUser);
        if (pPlayer != NULL)
        {
            g_pAppManager->GetServerExoApp()->GetSWSMessage()
                ->SendServerPlayerItemUpdate_DestroyItem(pPlayer, GetId());
        }
        m_bDeleting = 1;
        g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
            0, 0, oidUser, GetId(), CServerAIMaster::EVENT_DESTROY_OBJECT, NULL);
    }
    else
    {
        UpdateUsedActiveProperties();
    }
}
// PC 0x0055E180  Xbox 0x001DE290
void CSWSItem::AddProperty(unsigned short nPropertyType, unsigned short nSubtype,
                          unsigned short nCostValue, unsigned char nParam1Value,
                          CExoArrayList<CExoString>& aStrings)
{
    CExoString sProperty;
    CExoString sSubTypeResRef;
    int nCostTableResRef = 0;
    int nParam1ResRef = -1;
    int bHasSubtype;
    int bHasParam1;
    {
        int nName = -1;

        bool bFailed;
        {
            CExoString sColumn("Name");
            C2DA* pTable = g_pRules->Get2DArrays()->m_pItemPropDef;
            bFailed = pTable->GetINTEntry(nPropertyType, sColumn, &nName) == 0;
        }
        if (bFailed)
            return;

        {
            CExoString sColumn("SubTypeResRef");
            C2DA* pTable = g_pRules->Get2DArrays()->m_pItemPropDef;
            bHasSubtype = pTable->GetCExoStringEntry(
                nPropertyType, sColumn, &sSubTypeResRef);
        }
        {
            CExoString sColumn("CostTableResRef");
            C2DA* pTable = g_pRules->Get2DArrays()->m_pItemPropDef;
            pTable->GetINTEntry(nPropertyType, sColumn, &nCostTableResRef);
        }
        {
            CExoString sColumn("Param1ResRef");
            C2DA* pTable = g_pRules->Get2DArrays()->m_pItemPropDef;
            bHasParam1 = pTable->GetINTEntry(nPropertyType, sColumn, &nParam1ResRef);
        }

        sProperty = g_pAppManager->GetClientExoApp()->GetGUIString(nName);
    }
    if (bHasSubtype || bHasParam1 || nCostTableResRef)
    {
        sProperty = sProperty + ":";
        if (bHasSubtype)
        {
            C2DA* pSubtypeTable = new C2DA(sSubTypeResRef, 0);
            if (pSubtypeTable->Load2DArray() == 0)
                return;

            int nTableRef;
            int nSubtypeName = -1;
            pSubtypeTable->GetINTEntry(nSubtype, "Name", &nSubtypeName);
            bool bHasCost;
            {
                CExoString sColumn("CostTableResRef");
                bHasCost = pSubtypeTable->GetINTEntry(nSubtype, sColumn, &nTableRef)
                    && nTableRef != 0;
            }
            if (bHasCost)
                nCostTableResRef = nTableRef;
            if (pSubtypeTable->GetINTEntry(nSubtype, "Param1ResRef", &nTableRef))
            {
                bHasParam1 = 1;
                nParam1ResRef = nTableRef;
            }
            delete pSubtypeTable;

            if (nPropertyType == CSWItemProperty::ITEM_PROPERTY_DAMAGE_BONUS &&
                (nSubtype == CSWItemProperty::DAMAGE_SUBTYPE_BLUDGEONING ||
                 nSubtype == CSWItemProperty::DAMAGE_SUBTYPE_PIERCING ||
                 nSubtype == CSWItemProperty::DAMAGE_SUBTYPE_SLASHING))
            {
                sProperty = sProperty + " " +
                    g_pAppManager->GetClientExoApp()->GetGUIString(
                        GUI_STRREF_DAMAGE_PHYSICAL);
            }
            else
            {
                sProperty = sProperty + " " +
                    g_pAppManager->GetClientExoApp()->GetGUIString(nSubtypeName);
            }

            if (bHasParam1 || nCostTableResRef)
                sProperty = sProperty + ",";
        }

        if (nCostTableResRef)
        {
            C2DA* pCostTable = g_pRules->Get2DArrays()->GetIPRPCostTable(nCostTableResRef);
            int nCostName = -1;
            pCostTable->GetINTEntry(nCostValue, "Name", &nCostName);
            sProperty = sProperty + " " +
                g_pAppManager->GetClientExoApp()->GetGUIString(nCostName);
            if (bHasParam1)
                sProperty = sProperty + ",";
        }
        if (bHasParam1)
        {
            C2DA* pParamTable = g_pRules->Get2DArrays()->GetIPRPParamTable(nParam1ResRef);
            int nParamName = -1;
            pParamTable->GetINTEntry(nParam1Value, "Name", &nParamName);
            sProperty = sProperty + " " +
                g_pAppManager->GetClientExoApp()->GetGUIString(nParamName);
        }
    }
    aStrings.Add(sProperty);
}
// PC 0x0055E930  Xbox 0x001DE990
void CSWSItem::AddAttackModifierProperties(CExoString& sString)
{
    int nAttackModifier = 0;
    CExoString sAttackModifier;
    if (GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_ATTACK_BONUS, 0) ||
        GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_ATTACK_PENALTY, 0) ||
        GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_DECREASED_ATTACK_MODIFIER, 0) ||
        GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_ENHANCEMENT_BONUS, 0))
    {
        for (int nProperty = 0; nProperty < m_nPassiveProperties; nProperty++)
        {
            CSWItemProperty* pProperty = GetPassiveProperty(nProperty);
            if (pProperty != NULL &&
                (pProperty->m_nUpgradeType == CSWItemProperty::UPGRADE_TYPE_NONE ||
                 ((1 << pProperty->m_nUpgradeType) & m_nUpgrades) != 0))
            {
                if (pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_ENHANCEMENT_BONUS)
                    nAttackModifier += pProperty->m_nCostValue;
                else if (pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_ATTACK_BONUS)
                    nAttackModifier += pProperty->m_nCostValue;
                if (pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_ATTACK_PENALTY ||
                    pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_DECREASED_ATTACK_MODIFIER)
                    nAttackModifier -= pProperty->m_nCostValue;
            }
        }
        if (nAttackModifier < 0)
        {
            sAttackModifier.Format("%s: -%d\n", g_pAppManager->GetClientExoApp()->GetGUIString(
                GUI_STRREF_ITEM_ATTACK_MODIFIER).CStr(), nAttackModifier);
            sString = sString + sAttackModifier;
        }
        else if (nAttackModifier > 0)
        {
            sAttackModifier.Format("%s: +%d\n", g_pAppManager->GetClientExoApp()->GetGUIString(
                GUI_STRREF_ITEM_ATTACK_MODIFIER).CStr(), nAttackModifier);
            sString = sString + sAttackModifier;
        }
    }
    if (GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_ENHANCEMENT_BONUS_VS_ALIGNMENT_GROUP, 0) ||
        GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_ENHANCEMENT_BONUS_VS_RACIAL_GROUP, 0) ||
        GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_ATTACK_BONUS_VS_ALIGNMENT_GROUP, 0) ||
        GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_ATTACK_BONUS_VS_RACIAL_GROUP, 0))
    {
        if (nAttackModifier == 0)
            sString = sString + g_pAppManager->GetClientExoApp()->GetGUIString(
                GUI_STRREF_ITEM_ATTACK_MODIFIER) + ": ";
        AddAttackBonusStrings(CSWItemProperty::ITEM_PROPERTY_ENHANCEMENT_BONUS_VS_ALIGNMENT_GROUP, sString);
        AddAttackBonusStrings(CSWItemProperty::ITEM_PROPERTY_ENHANCEMENT_BONUS_VS_RACIAL_GROUP, sString);
        AddAttackBonusStrings(CSWItemProperty::ITEM_PROPERTY_ATTACK_BONUS_VS_ALIGNMENT_GROUP, sString);
        AddAttackBonusStrings(CSWItemProperty::ITEM_PROPERTY_ATTACK_BONUS_VS_RACIAL_GROUP, sString);
    }
    sString = sString + "\n";
}

inline unsigned short CSWItemProperty::GetPropertyType()
{
    return m_nPropertyType;
}

inline unsigned short CSWItemProperty::GetSubtype()
{
    return m_nSubtype;
}
// PC 0x005584C0  Xbox 0x001D8740
void CSWSItem::AddAttackBonusStrings(int nPropertyType, CExoString& sString)
{
    int nAttackBonus = 0;
    CExoString sName;
    CExoString sSubtype;
    CExoString sCost;
    CExoString sParam1;
    CExoString sProperty;

    if (GetPropertyByTypeExists(nPropertyType, 0))
    {
        for (int nProperty = 0; nProperty < m_nPassiveProperties; nProperty++)
        {
            CSWItemProperty* pProperty = GetPassiveProperty(nProperty);
            if (pProperty != NULL &&
                (pProperty->m_nUpgradeType == CSWItemProperty::UPGRADE_TYPE_NONE ||
                 ((UPGRADE_PROPERTY_MASK << pProperty->m_nUpgradeType) & m_nUpgrades) != 0))
            {
                unsigned short nType = pProperty->GetPropertyType();
                if (nType == nPropertyType)
                {
                    for (int nOtherProperty = 0; nOtherProperty < m_nPassiveProperties;
                         nOtherProperty++)
                    {
                        CSWItemProperty* pOtherProperty = GetPassiveProperty(nOtherProperty);
                        if (pOtherProperty != NULL &&
                            (pOtherProperty->m_nUpgradeType == CSWItemProperty::UPGRADE_TYPE_NONE ||
                             ((UPGRADE_PROPERTY_MASK << pOtherProperty->m_nUpgradeType) & m_nUpgrades) != 0) &&
                            pOtherProperty->GetPropertyType() == nPropertyType &&
                            pProperty->GetSubtype() == pOtherProperty->GetSubtype())
                        {
                            if (nOtherProperty < nProperty)
                                goto NextProperty;
                            nAttackBonus += pOtherProperty->GetCostValue();
                        }
                    }

                    unsigned short nCostValue = pProperty->GetCostValue();
                    unsigned short nSubtype = pProperty->GetSubtype();
                    GetPropertyStrings(nType, nSubtype,
                        nCostValue, CSWItemProperty::PARAM1_VALUE_NONE,
                        sName, sSubtype, sCost, sParam1);
                    sString = sString + "+" + CExoString(nAttackBonus) + " " +
                        g_pAppManager->GetClientExoApp()->GetGUIString(
                            GUI_STRREF_VERSUS) + " " + sSubtype + "\n";
                }
            }
NextProperty:
            ;
        }
    }
}

inline unsigned char CSWBaseItem::GetCritThreat()
{
    return m_nCritThreat;
}

inline unsigned char CSWBaseItem::GetCritHitMult()
{
    return m_nCritHitMult;
}
// PC 0x00558950  Xbox 0x001D8BD0
void CSWSItem::AddCriticalThreatProperties(CExoString& sString)
{
    CExoString sRange;
    sString = sString + g_pAppManager->GetClientExoApp()->GetGUIString(
        CClientExoApp::GUI_STRREF_ITEM_CRITICAL_THREAT) + ":\n";
    int nThreat = GetBaseItem()->GetCritThreat();
    int nMultiplier = GetBaseItem()->GetCritHitMult();
    if (GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_KEEN, 0))
        nThreat *= 2;
    int nMinimum = 0;
    int nMaximum = 0;
    if (GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_MASSIVE_CRITICALS, 0))
    {
        for (int i = 0; i < m_nPassiveProperties; i++)
        {
            CSWItemProperty* pProperty = GetPassiveProperty(i);
            if (pProperty &&
                (pProperty->m_nUpgradeType == CSWItemProperty::UPGRADE_TYPE_NONE ||
                 ((UPGRADE_PROPERTY_MASK << pProperty->m_nUpgradeType) & m_nUpgrades) != 0))
            {
                switch (pProperty->GetPropertyType())
                {
                case CSWItemProperty::ITEM_PROPERTY_MASSIVE_CRITICALS:
                    if (!GetExistsHigherValuedProperty(pProperty->GetPropertyType(),
                                                      pProperty->GetSubtype(), pProperty->GetCostValue()))
                    {
                        int nMinDamage;
                        int nMaxDamage;
                        GetPropertyDamageRange(pProperty->GetCostValue(), nMinDamage, nMaxDamage);
                        nMinimum += nMinDamage;
                        nMaximum += nMaxDamage;
                    }
                    break;
                }
            }
        }
    }
    if (nMinimum != 0 || nMaximum != 0)
        sRange.Format("%d-20,x%d +%d-%d\n", 21 - nThreat, nMultiplier, nMinimum, nMaximum);
    else
        sRange.Format("%d-20,x%d\n", 21 - nThreat, nMultiplier);
    sString = sString + sRange + "\n";
}
// PC 0x00558C10  Xbox 0x001D8E90
void CSWSItem::AddOnHitProperties(CExoString& sString)
{
    if (GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_ON_HIT_PROPERTIES, 0))
    {
        CExoString sProperty;
        CExoString sSubtype;
        CExoString sCost;
        CExoString sParam1;
        CExoString sDescription;
        CExoString sDetails;
        int nEffectChance;
        int nDurationRounds;
        int nDcSave;

        for (int nProperty = 0; nProperty < m_nPassiveProperties; nProperty++)
        {
            CSWItemProperty* pProperty = GetPassiveProperty(nProperty);
            if (pProperty != NULL &&
                (pProperty->m_nUpgradeType == CSWItemProperty::UPGRADE_TYPE_NONE ||
                 ((UPGRADE_PROPERTY_MASK << pProperty->m_nUpgradeType) & m_nUpgrades) != 0))
            {
                unsigned short nPropertyType = pProperty->m_nPropertyType;
                if (pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_ON_HIT_PROPERTIES)
                {
                    unsigned short nSubtype = pProperty->m_nSubtype;
                    GetPropertyStrings(nPropertyType, nSubtype,
                        CSWItemProperty::COST_VALUE_NONE, CSWItemProperty::PARAM1_VALUE_NONE,
                        sProperty, sSubtype, sCost, sParam1);
                    switch (pProperty->m_nSubtype)
                    {
                    case CSWItemProperty::ON_HIT_SLEEP:
                    case CSWItemProperty::ON_HIT_STUN:
                    case CSWItemProperty::ON_HIT_HOLD:
                    case CSWItemProperty::ON_HIT_CONFUSION:
                    case CSWItemProperty::ON_HIT_FEAR:
                    case CSWItemProperty::ON_HIT_SLOW:
                    {
                        sString = sString + g_pAppManager->GetClientExoApp()->GetGUIString(GUI_STRREF_ITEM_ON_HIT) + ": " + sSubtype + " ";
                        unsigned char nParam1Value = pProperty->m_nParam1Value;
                        unsigned short nCostValue = pProperty->m_nCostValue;
                        GetPropertyChanceDurationDC(nCostValue,
                            nParam1Value, nEffectChance, nDurationRounds, nDcSave);
                        nDurationRounds *= 3;
                        sDetails.Format("%d%% %s, %d %s, %s %d\n", nEffectChance,
                            g_pAppManager->GetClientExoApp()->GetGUIString(GUI_STRREF_CHANCE).CStr(),
                            nDurationRounds,
                            g_pAppManager->GetClientExoApp()->GetGUIString(GUI_STRREF_SECONDS).CStr(),
                            g_pAppManager->GetClientExoApp()->GetGUIString(GUI_STRREF_DC).CStr(), nDcSave);
                        sString = sString + sDetails + "\n";
                        break;
                    }
                    case CSWItemProperty::ON_HIT_ABILITY_DAMAGE:
                    case CSWItemProperty::ON_HIT_SLAY_RACIAL_GROUP:
                    case CSWItemProperty::ON_HIT_SLAY_ALIGNMENT_GROUP:
                    {
                        sString = sString + g_pAppManager->GetClientExoApp()->GetGUIString(GUI_STRREF_ITEM_ON_HIT) + ": " + sSubtype + " ";
                        unsigned char nParam1Value = pProperty->m_nParam1Value;
                        unsigned short nCostValue = pProperty->m_nCostValue;
                        unsigned short nSubtype = pProperty->m_nSubtype;
                        unsigned short nPropertyType = pProperty->m_nPropertyType;
                        GetPropertyStrings(nPropertyType, nSubtype,
                            nCostValue, nParam1Value,
                            sProperty, sSubtype, sCost, sParam1);
                        sString = sString + sParam1 + " " + sCost + "\n\n";
                        break;
                    }
                    case CSWItemProperty::ON_HIT_POISON:
                    {
                        sString = sString + g_pAppManager->GetClientExoApp()->GetGUIString(GUI_STRREF_ITEM_ON_HIT) + ": ";
                        unsigned char nParam1Value = pProperty->m_nParam1Value;
                        unsigned short nCostValue = pProperty->m_nCostValue;
                        unsigned short nSubtype = pProperty->m_nSubtype;
                        unsigned short nPropertyType = pProperty->m_nPropertyType;
                        GetPropertyStrings(nPropertyType, nSubtype,
                            nCostValue, nParam1Value,
                            sProperty, sSubtype, sCost, sParam1);
                        {
                            CExoString sColumn("DC_SAVE");
                            unsigned char nPoison = pProperty->m_nParam1Value;
                            C2DA* pPoisonTable = g_pRules->Get2DArrays()->m_pPoison;
                            pPoisonTable->GetINTEntry(nPoison, sColumn, &nDcSave);
                        }
                        sString = sString + sParam1 + " " +
                            g_pAppManager->GetClientExoApp()->GetGUIString(GUI_STRREF_DC) + " " +
                            CExoString(nDcSave) + "\n\n";
                        break;
                    }
                    case CSWItemProperty::ON_HIT_INSTANT_DEATH:
                    {
                        sString = sString + g_pAppManager->GetClientExoApp()->GetGUIString(GUI_STRREF_ITEM_ON_HIT) + ": " + sSubtype + " ";
                        unsigned char nParam1Value = pProperty->m_nParam1Value;
                        unsigned short nCostValue = pProperty->m_nCostValue;
                        unsigned short nSubtype = pProperty->m_nSubtype;
                        unsigned short nPropertyType = pProperty->m_nPropertyType;
                        GetPropertyStrings(nPropertyType, nSubtype,
                            nCostValue, nParam1Value,
                            sProperty, sSubtype, sCost, sParam1);
                        sString = sString + ", " + sCost + "\n\n";
                        break;
                    }
                    }
                }
            }
        }
    }
}
// PC 0x005599D0  Xbox 0x001D9B20
void CSWSItem::AddDefenceProperties(CExoString& sString)
{
    CExoString sBonus;
    int nDefenceBonus = GetBaseItem()->m_nBaseAC;
    CSWItemProperty* pProperty;
    int nProperty;

    if (GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_AC_BONUS, 0))
    {
        for (nProperty = 0; nProperty < m_nPassiveProperties; nProperty++)
        {
            pProperty = GetPassiveProperty(nProperty);
            if (pProperty != NULL &&
                (pProperty->m_nUpgradeType == CSWItemProperty::UPGRADE_TYPE_NONE ||
                 ((1 << pProperty->m_nUpgradeType) & m_nUpgrades) != 0) &&
                pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_AC_BONUS &&
                (GetBaseItem()->m_nACEnchantType == 0 ||
                 !GetExistsHigherValuedProperty(pProperty->m_nPropertyType,
                    pProperty->m_nSubtype, pProperty->m_nCostValue)))
            {
                nDefenceBonus += pProperty->m_nCostValue;
            }
        }
    }

    if (GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_DECREASED_AC, 0))
    {
        GetPropertyByType(&pProperty, CSWItemProperty::ITEM_PROPERTY_DECREASED_AC, 0);
        if (pProperty != NULL &&
            (pProperty->m_nUpgradeType == CSWItemProperty::UPGRADE_TYPE_NONE ||
             ((1 << pProperty->m_nUpgradeType) & m_nUpgrades) != 0))
        {
            unsigned short nCostValue = pProperty->m_nCostValue;
            unsigned short nSubtype = pProperty->m_nSubtype;
            unsigned short nPropertyType = pProperty->m_nPropertyType;
            if (!GetExistsHigherValuedProperty(nPropertyType, nSubtype, nCostValue))
                nDefenceBonus -= nCostValue;
        }
    }

    if (nDefenceBonus != 0)
    {
        sString = sString + g_pAppManager->GetClientExoApp()->GetGUIString(
            GUI_STRREF_ITEM_DEFENCE_BONUS) + ": ";
        sBonus.Format("%d\n", nDefenceBonus);
        sString = sString + sBonus + "\n";
    }

    unsigned char nItemType = GetBaseItem()->m_nItemType;
    if (nItemType != CSWBaseItem::ITEM_TYPE_NO_DEX_LIMIT_DISPLAY)
    {
        unsigned char nDexBonus = GetBaseItem()->m_nDexBonus;
        if (nDexBonus != CSWBaseItem::DEX_BONUS_UNLIMITED)
        {
            CExoString sNewLine("\n\n");
            sString = sString + g_pAppManager->GetClientExoApp()->GetGUIString(
                GUI_STRREF_ITEM_MAX_DEXTERITY_BONUS) + ": +" +
                CExoString(nDexBonus) + sNewLine;
        }
    }

    CExoString sProperty;
    CExoString sSubtype;
    CExoString sCost;
    CExoString sParam1;
    CExoString sDefence;
    if (GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_AC_BONUS_VS_ALIGNMENT_GROUP, 0) ||
        GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_AC_BONUS_VS_DAMAGE_TYPE, 0) ||
        GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_AC_BONUS_VS_RACIAL_GROUP, 0) ||
        GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_IMMUNITY_DAMAGE_TYPE, 0) ||
        GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_DAMAGE_RESISTANCE, 0))
    {
        for (nProperty = 0; nProperty < m_nPassiveProperties; nProperty++)
        {
            pProperty = GetPassiveProperty(nProperty);
            if (pProperty != NULL &&
                (pProperty->m_nUpgradeType == CSWItemProperty::UPGRADE_TYPE_NONE ||
                 ((1 << pProperty->m_nUpgradeType) & m_nUpgrades) != 0))
            {
                if ((pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_AC_BONUS_VS_ALIGNMENT_GROUP ||
                     pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_AC_BONUS_VS_DAMAGE_TYPE ||
                     pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_AC_BONUS_VS_RACIAL_GROUP) &&
                    !GetExistsHigherValuedProperty(pProperty->m_nPropertyType,
                        pProperty->m_nSubtype, pProperty->m_nCostValue))
                {
                    GetPropertyStrings(pProperty->m_nPropertyType,
                        pProperty->m_nSubtype, pProperty->m_nCostValue,
                        CSWItemProperty::PARAM1_VALUE_NONE,
                        sProperty, sSubtype, sCost, sParam1);
                    sString = sString + "+" + sCost + " " +
                        g_pAppManager->GetClientExoApp()->GetGUIString(GUI_STRREF_VERSUS) +
                        " " + sSubtype + "\n\n";
                }
                if ((pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_IMMUNITY_DAMAGE_TYPE ||
                     pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_DAMAGE_RESISTANCE) &&
                    !GetExistsHigherValuedProperty(pProperty->m_nPropertyType,
                        pProperty->m_nSubtype, pProperty->m_nCostValue))
                {
                    GetPropertyStrings(pProperty->m_nPropertyType,
                        pProperty->m_nSubtype, pProperty->m_nCostValue,
                        CSWItemProperty::PARAM1_VALUE_NONE,
                        sProperty, sSubtype, sCost, sParam1);
                    sString = sString + sProperty + ": " + sCost + " " +
                        g_pAppManager->GetClientExoApp()->GetGUIString(GUI_STRREF_VERSUS) +
                        " " + sSubtype + "\n\n";
                }
            }
        }
    }
}
// PC 0x0055A510  Xbox 0x001DA660
void CSWSItem::AddMiscellaneousProperties(CExoString& sString)
{
    CExoString sProperty;
    CExoString sSubtype;
    CExoString sCost;
    CExoString sParam1;
    CExoString sDescription;
    CExoString sDetails;
    char szCost[32];
    int nCost;
    int nMaxUses;
    int nProperty;
    unsigned short nSubtype;
    unsigned short nCostValue;

    if (GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_ACTIVATE_ITEM, 0))
    {
        for (nProperty = 0; nProperty < m_nActiveProperties; nProperty++)
        {
            CSWItemProperty* pProperty = GetActiveProperty(nProperty);
            if (pProperty != NULL &&
                (pProperty->m_nUpgradeType == CSWItemProperty::UPGRADE_TYPE_NONE ||
                 ((UPGRADE_PROPERTY_MASK << pProperty->m_nUpgradeType) & m_nUpgrades) != 0) &&
                pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_ACTIVATE_ITEM)
            {
                int nCostValue = pProperty->m_nCostValue;
                if (nCostValue == CSWItemProperty::ACTIVATE_ITEM_SINGLE_USE)
                {
                    sString = sString + g_pAppManager->GetClientExoApp()->GetGUIString(GUI_STRREF_ITEM_SINGLE_USE) + "\n\n";
                }
                else
                {
                    switch (nCostValue)
                    {
                    case CSWItemProperty::ACTIVATE_ITEM_UNLIMITED:
                        nMaxUses = 0;
                        break;
                    case CSWItemProperty::ACTIVATE_ITEM_UNLIMITED_ALT:
                        nMaxUses = 0;
                        break;
                    case CSWItemProperty::ACTIVATE_ITEM_SINGLE_USE:
                        nMaxUses = 1;
                        break;
                    case CSWItemProperty::ACTIVATE_ITEM_1_USE_PER_DAY:
                        nMaxUses = 1;
                        break;
                    case CSWItemProperty::ACTIVATE_ITEM_RECHARGE_1_MINUTE:
                        nMaxUses = 1;
                        break;
                    case CSWItemProperty::ACTIVATE_ITEM_RECHARGE_2_MINUTES:
                        nMaxUses = 1;
                        break;
                    case CSWItemProperty::ACTIVATE_ITEM_RECHARGE_3_MINUTES:
                        nMaxUses = 1;
                        break;
                    case CSWItemProperty::ACTIVATE_ITEM_RECHARGE_4_MINUTES:
                        nMaxUses = 1;
                        break;
                    case CSWItemProperty::ACTIVATE_ITEM_RECHARGE_5_MINUTES:
                        nMaxUses = 1;
                        break;
                    case CSWItemProperty::ACTIVATE_ITEM_2_USES_PER_DAY:
                        nMaxUses = 2;
                        break;
                    case CSWItemProperty::ACTIVATE_ITEM_3_USES_PER_DAY:
                        nMaxUses = 3;
                        break;
                    case CSWItemProperty::ACTIVATE_ITEM_4_USES_PER_DAY:
                        nMaxUses = 4;
                        break;
                    case CSWItemProperty::ACTIVATE_ITEM_5_USES_PER_DAY:
                        nMaxUses = 5;
                        break;
                    case CSWItemProperty::ACTIVATE_ITEM_5_CHARGES:
                        nMaxUses = m_nMaxCharges / 5;
                        break;
                    case CSWItemProperty::ACTIVATE_ITEM_4_CHARGES:
                        nMaxUses = m_nMaxCharges / 4;
                        break;
                    case CSWItemProperty::ACTIVATE_ITEM_3_CHARGES:
                        nMaxUses = m_nMaxCharges / 3;
                        break;
                    case CSWItemProperty::ACTIVATE_ITEM_2_CHARGES:
                        nMaxUses = m_nMaxCharges;
                        break;
                    case CSWItemProperty::ACTIVATE_ITEM_1_CHARGE:
                        nMaxUses = m_nMaxCharges;
                        break;
                    }
                    sString = sString + g_pAppManager->GetClientExoApp()->GetGUIString(GUI_STRREF_ITEM_USES) + ": ";
                    if (nMaxUses == 0)
                        sString = sString + g_pAppManager->GetClientExoApp()->GetGUIString(GUI_STRREF_UNLIMITED) + "\n\n";
                    else
                    {
                        int nUsesLeft = GetUsedActivePropertyUsesLeft(nProperty);
                        sDetails.Format("%d/%d\n", nUsesLeft, nMaxUses);
                        sString = sString + sDetails + "\n";
                    }
                }
            }
        }
    }

    int nCount = 0;
    if (GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_IMMUNITY, 0))
    {
        for (nProperty = 0; nProperty < m_nPassiveProperties; nProperty++)
        {
            CSWItemProperty* pProperty = GetPassiveProperty(nProperty);
            if (pProperty != NULL &&
                (pProperty->m_nUpgradeType == CSWItemProperty::UPGRADE_TYPE_NONE ||
                 ((UPGRADE_PROPERTY_MASK << pProperty->m_nUpgradeType) & m_nUpgrades) != 0))
            {
                unsigned short nPropertyType = pProperty->m_nPropertyType;
                if (pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_IMMUNITY)
                {
                    nSubtype = pProperty->m_nSubtype;
                    GetPropertyStrings(nPropertyType, nSubtype,
                        CSWItemProperty::COST_VALUE_NONE, CSWItemProperty::PARAM1_VALUE_NONE,
                        sProperty, sSubtype, sCost, sParam1);
                    if (nCount == 0)
                        sString = sString + sProperty + ": ";
                    else if (nCount > 0)
                        sString = sString + ", ";
                    sString = sString + sSubtype;
                    nCount++;
                }
            }
        }
        sString = sString + "\n\n";
    }

    nCount = 0;
    if (GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_IMPROVED_SAVING_THROW, 0) ||
        GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_DECREASED_SAVING_THROWS, 0) ||
        GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_IMPROVED_SAVING_THROW_SPECIFIC, 0) ||
        GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_DECREASED_SAVING_THROWS_SPECIFIC, 0))
    {
        sString = sString + g_pAppManager->GetClientExoApp()->GetGUIString(GUI_STRREF_ITEM_SAVES) + ": ";
        for (nProperty = 0; nProperty < m_nPassiveProperties; nProperty++)
        {
            CSWItemProperty* pProperty = GetPassiveProperty(nProperty);
            if (pProperty != NULL &&
                (pProperty->m_nUpgradeType == CSWItemProperty::UPGRADE_TYPE_NONE ||
                 ((UPGRADE_PROPERTY_MASK << pProperty->m_nUpgradeType) & m_nUpgrades) != 0) &&
                !GetExistsHigherValuedProperty(pProperty->m_nPropertyType,
                    pProperty->m_nSubtype, pProperty->m_nCostValue))
            {
                GetPropertyStrings(pProperty->m_nPropertyType, pProperty->m_nSubtype,
                    pProperty->m_nCostValue, CSWItemProperty::PARAM1_VALUE_NONE,
                    sProperty, sSubtype, sCost, sParam1);
                switch (pProperty->m_nPropertyType)
                {
                case CSWItemProperty::ITEM_PROPERTY_IMPROVED_SAVING_THROW:
                case CSWItemProperty::ITEM_PROPERTY_IMPROVED_SAVING_THROW_SPECIFIC:
                    if (nCount > 0)
                        sString = sString + ", ";
                    sString = sString + sSubtype + " +" + sCost;
                    nCount++;
                    break;
                case CSWItemProperty::ITEM_PROPERTY_DECREASED_SAVING_THROWS:
                case CSWItemProperty::ITEM_PROPERTY_DECREASED_SAVING_THROWS_SPECIFIC:
                    if (nCount > 0)
                        sString = sString + ", ";
                    sscanf(sCost.CStr(), "%s -%d", szCost, &nCost);
                    sDetails.Format("%d", nCost);
                    sString = sString + sSubtype + " -" + sDetails;
                    nCount++;
                    break;
                }
            }
        }
        sString = sString + "\n\n";
    }

    nCount = 0;
    if (GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_SKILL_BONUS, 0) ||
        GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_DECREASED_SKILL_MODIFIER, 0))
    {
        sString = sString + g_pAppManager->GetClientExoApp()->GetGUIString(GUI_STRREF_ITEM_SKILLS) + ": ";
        for (nProperty = 0; nProperty < m_nPassiveProperties; nProperty++)
        {
            CSWItemProperty* pProperty = GetPassiveProperty(nProperty);
            if (pProperty != NULL &&
                (pProperty->m_nUpgradeType == CSWItemProperty::UPGRADE_TYPE_NONE ||
                 ((UPGRADE_PROPERTY_MASK << pProperty->m_nUpgradeType) & m_nUpgrades) != 0) &&
                !GetExistsHigherValuedProperty(pProperty->m_nPropertyType,
                    pProperty->m_nSubtype, pProperty->m_nCostValue))
            {
                GetPropertyStrings(pProperty->m_nPropertyType, pProperty->m_nSubtype,
                    pProperty->m_nCostValue, CSWItemProperty::PARAM1_VALUE_NONE,
                    sProperty, sSubtype, sCost, sParam1);
                switch (pProperty->m_nPropertyType)
                {
                case CSWItemProperty::ITEM_PROPERTY_SKILL_BONUS:
                    nCostValue = pProperty->m_nCostValue;
                    nSubtype = pProperty->m_nSubtype;
                    if (!GetExistsHigherValuedProperty(pProperty->m_nPropertyType,
                        nSubtype, nCostValue))
                    {
                        if (nCount > 0)
                            sString = sString + ", ";
                        nCostValue = pProperty->m_nCostValue;
                        nSubtype = pProperty->m_nSubtype;
                        unsigned short nPropertyType = pProperty->m_nPropertyType;
                        GetPropertyStrings(nPropertyType, nSubtype,
                            nCostValue, CSWItemProperty::PARAM1_VALUE_NONE,
                            sProperty, sSubtype, sCost, sParam1);
                        sString = sString + sSubtype + " " + sCost;
                        nCount++;
                    }
                    break;
                case CSWItemProperty::ITEM_PROPERTY_DECREASED_SKILL_MODIFIER:
                    nCostValue = pProperty->m_nCostValue;
                    nSubtype = pProperty->m_nSubtype;
                    if (!GetExistsHigherValuedProperty(pProperty->m_nPropertyType,
                        nSubtype, nCostValue))
                    {
                        if (nCount > 0)
                            sString = sString + ", ";
                        nCostValue = pProperty->m_nCostValue;
                        nSubtype = pProperty->m_nSubtype;
                        unsigned short nPropertyType = pProperty->m_nPropertyType;
                        GetPropertyStrings(nPropertyType, nSubtype,
                            nCostValue, CSWItemProperty::PARAM1_VALUE_NONE,
                            sProperty, sSubtype, sCost, sParam1);
                        sscanf(sCost.CStr(), "%s -%d", szCost, &nCost);
                        sDetails.Format("-%d", nCost);
                        sString = sString + sSubtype + " " + sDetails;
                        nCount++;
                    }
                    break;
                }
            }
        }
        sString = sString + "\n\n";
    }

    for (nProperty = 0; nProperty < m_nPassiveProperties; nProperty++)
    {
        CSWItemProperty* pProperty = GetPassiveProperty(nProperty);
        if (pProperty != NULL &&
            (pProperty->m_nUpgradeType == CSWItemProperty::UPGRADE_TYPE_NONE ||
             ((UPGRADE_PROPERTY_MASK << pProperty->m_nUpgradeType) & m_nUpgrades) != 0))
        {
            unsigned short nPropertyType = pProperty->m_nPropertyType;
            switch (pProperty->m_nPropertyType)
            {
            case CSWItemProperty::ITEM_PROPERTY_BONUS_FEAT:
                nSubtype = pProperty->m_nSubtype;
                GetPropertyStrings(nPropertyType, nSubtype,
                    CSWItemProperty::COST_VALUE_NONE, CSWItemProperty::PARAM1_VALUE_NONE,
                    sProperty, sSubtype, sCost, sParam1);
                sString = sString + sProperty + ": " + sSubtype + "\n\n";
                break;
            case CSWItemProperty::ITEM_PROPERTY_ABILITY_BONUS:
                {
                    nCostValue = pProperty->m_nCostValue;
                    nSubtype = pProperty->m_nSubtype;
                    if (!GetExistsHigherValuedProperty(nPropertyType, nSubtype, nCostValue))
                    {
                        GetPropertyStrings(nPropertyType, nSubtype, nCostValue,
                            CSWItemProperty::PARAM1_VALUE_NONE, sProperty, sSubtype, sCost, sParam1);
                        sString = sString + sSubtype + ": " + sCost + "\n\n";
                    }
                }
                break;
            case CSWItemProperty::ITEM_PROPERTY_REGENERATION:
            case CSWItemProperty::ITEM_PROPERTY_REGENERATION_FORCE_POINTS:
                {
                    nCostValue = pProperty->m_nCostValue;
                    nSubtype = pProperty->m_nSubtype;
                    if (!GetExistsHigherValuedProperty(nPropertyType, nSubtype, nCostValue))
                    {
                        GetPropertyStrings(nPropertyType, nSubtype, nCostValue,
                            CSWItemProperty::PARAM1_VALUE_NONE, sProperty, sSubtype, sCost, sParam1);
                        sString = sString + sProperty + ": " + sCost + "\n\n";
                    }
                }
                break;
            case CSWItemProperty::ITEM_PROPERTY_IMPROVED_FORCE_RESISTANCE:
                {
                    nCostValue = pProperty->m_nCostValue;
                    nSubtype = pProperty->m_nSubtype;
                    if (!GetExistsHigherValuedProperty(nPropertyType, nSubtype, nCostValue))
                    {
                        GetPropertyStrings(nPropertyType, nSubtype, nCostValue,
                            CSWItemProperty::PARAM1_VALUE_NONE, sProperty, sSubtype, sCost, sParam1);
                        sscanf(sCost.CStr(), "%s %d", szCost, &nCost);
                        sDetails.Format("%d", nCost);
                        sString = sString + g_pAppManager->GetClientExoApp()->GetGUIString(GUI_STRREF_ITEM_FORCE_RESIST) + ": +" + sDetails + "\n\n";
                    }
                }
                break;
            case CSWItemProperty::ITEM_PROPERTY_DECREASED_ABILITY_SCORE:
                {
                    nCostValue = pProperty->m_nCostValue;
                    nSubtype = pProperty->m_nSubtype;
                    if (!GetExistsHigherValuedProperty(nPropertyType, nSubtype, nCostValue))
                    {
                        GetPropertyStrings(nPropertyType, nSubtype, nCostValue,
                            CSWItemProperty::PARAM1_VALUE_NONE, sProperty, sSubtype, sCost, sParam1);
                        sscanf(sCost.CStr(), "%s -%d", szCost, &nCost);
                        sDetails.Format("-%d", nCost);
                        sString = sString + sSubtype + ": " + sDetails + "\n\n";
                    }
                }
                break;
            case CSWItemProperty::ITEM_PROPERTY_LIGHT:
            case CSWItemProperty::ITEM_PROPERTY_TRUE_SEEING:
            case CSWItemProperty::ITEM_PROPERTY_FREEDOM_OF_MOVEMENT:
                GetPropertyStrings(nPropertyType, CSWItemProperty::SUBTYPE_NONE,
                    CSWItemProperty::COST_VALUE_NONE, CSWItemProperty::PARAM1_VALUE_NONE,
                    sProperty, sSubtype, sCost, sParam1);
                sString = sString + sProperty + "\n\n";
                break;
            case CSWItemProperty::ITEM_PROPERTY_BLASTER_BOLT_DEFLECT_INCREASE:
                nCostValue = pProperty->m_nCostValue;
                nSubtype = pProperty->m_nSubtype;
                if (!GetExistsHigherValuedProperty(nPropertyType, nSubtype, nCostValue))
                {
                    GetPropertyStrings(nPropertyType, CSWItemProperty::SUBTYPE_NONE,
                        pProperty->m_nCostValue, CSWItemProperty::PARAM1_VALUE_NONE,
                        sProperty, sSubtype, sCost, sParam1);
                    sDetails.Format("%d", pProperty->m_nCostValue);
                    CExoString sLeft;
                    CExoString sRight;
                    int nColon = sProperty.Find(':', 0);
                    if (nColon != -1)
                    {
                        sLeft = sProperty.Left(nColon);
                        sRight = sProperty.Right(sProperty.GetLength() - nColon - 1);
                        sProperty = CExoString(sLeft) + ": ";
                    }
                    sString = sString + sProperty + "+" + sDetails + "\n\n";
                }
                break;
            case CSWItemProperty::ITEM_PROPERTY_BLASTER_BOLT_DEFLECT_DECREASE:
                {
                    nCostValue = pProperty->m_nCostValue;
                    nSubtype = pProperty->m_nSubtype;
                    if (!GetExistsHigherValuedProperty(nPropertyType, nSubtype, nCostValue))
                    {
                        GetPropertyStrings(nPropertyType, CSWItemProperty::SUBTYPE_NONE,
                            nCostValue, CSWItemProperty::PARAM1_VALUE_NONE,
                            sProperty, sSubtype, sCost, sParam1);
                        CExoString sLeft;
                        CExoString sRight;
                        int nColon = sProperty.Find(':', 0);
                        if (nColon != -1)
                        {
                            sLeft = sProperty.Left(nColon);
                            sRight = sProperty.Right(sProperty.GetLength() - nColon - 1);
                            sProperty = CExoString(sLeft) + ": ";
                        }
                        sscanf(sCost.CStr(), "%s -%d", szCost, &nCost);
                        sDetails.Format("%d", nCost);
                        sString = sString + sProperty + " -" + sDetails + "\n\n";
                    }
                }
                break;
            case CSWItemProperty::ITEM_PROPERTY_DAMAGE_VULNERABILITY:
                nCostValue = pProperty->m_nCostValue;
                nSubtype = pProperty->m_nSubtype;
                if (!GetExistsHigherValuedProperty(nPropertyType, nSubtype, nCostValue))
                {
                    GetPropertyStrings(nPropertyType, pProperty->m_nSubtype,
                        CSWItemProperty::COST_VALUE_NONE, CSWItemProperty::PARAM1_VALUE_NONE,
                        sProperty, sSubtype, sCost, sParam1);
                    int nEffectChance;
                    int nDurationRounds;
                    int nDC;
                    unsigned char nParam1Value = pProperty->m_nParam1Value;
                    nCostValue = pProperty->m_nCostValue;
                    GetPropertyChanceDurationDC(nCostValue, nParam1Value,
                        nEffectChance, nDurationRounds, nDC);
                    nDurationRounds *= 3;
                    sDetails.Format("%d%% %s, %d%s ", nEffectChance,
                        g_pAppManager->GetClientExoApp()->GetGUIString(GUI_STRREF_CHANCE).CStr(),
                        nDurationRounds,
                        g_pAppManager->GetClientExoApp()->GetGUIString(GUI_STRREF_SECONDS).CStr());
                    sString = sString + g_pAppManager->GetClientExoApp()->GetGUIString(GUI_STRREF_ITEM_VULNERABILITY) + ": " +
                        sDetails + g_pAppManager->GetClientExoApp()->GetGUIString(GUI_STRREF_VERSUS) + " " + sSubtype + "\n\n";
                }
                break;
            case CSWItemProperty::ITEM_PROPERTY_USE_LIMITATION_ALIGNMENT_GROUP:
            case CSWItemProperty::ITEM_PROPERTY_USE_LIMITATION_CLASS:
                nSubtype = pProperty->m_nSubtype;
                GetPropertyStrings(nPropertyType, nSubtype,
                    CSWItemProperty::COST_VALUE_NONE, CSWItemProperty::PARAM1_VALUE_NONE,
                    sProperty, sSubtype, sCost, sParam1);
                sString = sString + g_pAppManager->GetClientExoApp()->GetGUIString(GUI_STRREF_ITEM_RESTRICTED_TO) + ": " + sSubtype + "\n\n";
                break;
            }
        }
    }
}
// PC 0x0055CB60  Xbox 0x001DCC90
void CSWSItem::AIUpdate()
{
    if (GetPropertyByTypeExists(CSWItemProperty::ITEM_PROPERTY_ACTIVATE_ITEM, 0))
    {
        unsigned long nCalendarDay;
        unsigned long nTimeOfDay;
        unsigned long nElapsedDay;
        unsigned long nElapsedTime;

        {
            CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
            pServer->GetWorldTimer()->GetWorldTime(&nCalendarDay, &nTimeOfDay);
        }
        for (int nProperty = 0; nProperty < m_nActiveProperties; nProperty++)
        {
            CSWItemProperty* pItemProperty = GetActiveProperty(nProperty);
            if (pItemProperty != NULL &&
                pItemProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_ACTIVATE_ITEM)
            {
                int nCostValue = pItemProperty->m_nCostValue;
                if ((nCostValue == CSWItemProperty::ACTIVATE_ITEM_RECHARGE_1_MINUTE ||
                     nCostValue == CSWItemProperty::ACTIVATE_ITEM_RECHARGE_2_MINUTES ||
                     nCostValue == CSWItemProperty::ACTIVATE_ITEM_RECHARGE_3_MINUTES ||
                     nCostValue == CSWItemProperty::ACTIVATE_ITEM_RECHARGE_4_MINUTES ||
                     nCostValue == CSWItemProperty::ACTIVATE_ITEM_RECHARGE_5_MINUTES) &&
                    (unsigned char)pItemProperty->m_bUseable == 0)
                {
                    unsigned long nLastUseTime = pItemProperty->m_nLastUseTime;
                    unsigned long nLastUseDay = pItemProperty->m_nLastUseDay;
                    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
                    pServer->GetWorldTimer()->SubtractWorldTimes(
                        nCalendarDay, nTimeOfDay, nLastUseDay,
                        nLastUseTime, &nElapsedDay, &nElapsedTime);
                    if ((nCostValue == CSWItemProperty::ACTIVATE_ITEM_RECHARGE_1_MINUTE &&
                         nElapsedTime >= ACTIVATE_ITEM_RECHARGE_INTERVAL) ||
                        (nCostValue == CSWItemProperty::ACTIVATE_ITEM_RECHARGE_2_MINUTES &&
                         nElapsedTime >= 2 * ACTIVATE_ITEM_RECHARGE_INTERVAL) ||
                        (nCostValue == CSWItemProperty::ACTIVATE_ITEM_RECHARGE_3_MINUTES &&
                         nElapsedTime >= 3 * ACTIVATE_ITEM_RECHARGE_INTERVAL) ||
                        (nCostValue == CSWItemProperty::ACTIVATE_ITEM_RECHARGE_4_MINUTES &&
                         nElapsedTime >= 4 * ACTIVATE_ITEM_RECHARGE_INTERVAL) ||
                        (nCostValue == CSWItemProperty::ACTIVATE_ITEM_RECHARGE_5_MINUTES &&
                         nElapsedTime >= 5 * ACTIVATE_ITEM_RECHARGE_INTERVAL))
                    {
                        pItemProperty->m_bUseable = 1;
                    }
                }
            }
        }
    }
}
// PC 0x0055CCD0  Xbox 0x001DCE00
int CSWSItem::SaveItem(CResGFF* pRes, CResStruct* pStruct)
{
    pRes->WriteFieldINT(pStruct, GetBaseItemType(), "BaseItem");
    pRes->WriteFieldCExoString(pStruct, m_sTag, "Tag");
    pRes->WriteFieldBYTE(pStruct, 1, "Identified");
    pRes->WriteFieldCExoLocString(pStruct, m_sDescription, "Description");
    pRes->WriteFieldCExoLocString(pStruct, m_sDescIdentified, "DescIdentified");
    pRes->WriteFieldCExoLocString(pStruct, m_sLocalizedName, "LocalizedName");
    pRes->WriteFieldWORD(pStruct, m_nStackSize, "StackSize");
    pRes->WriteFieldBYTE(pStruct, m_bStolen, "Stolen");
    pRes->WriteFieldDWORD(pStruct, m_nUpgrades, "Upgrades");
    pRes->WriteFieldBYTE(pStruct, m_bDropable, "Dropable");
    pRes->WriteFieldBYTE(pStruct, m_bPickpocketable, "Pickpocketable");
    pRes->WriteFieldBYTE(pStruct, m_nModelVariation, "ModelVariation");
    switch (g_pRules->GetBaseItemArray()->GetBaseItem(GetBaseItemType())->m_bUnknown18)
    {
    case BASEITEM_MODEL_TYPE_BODY_VARIATION:
        pRes->WriteFieldBYTE(pStruct, m_nBodyVariation, "BodyVariation");
        pRes->WriteFieldBYTE(pStruct, m_nTextureVar, "TextureVar");
        break;
    }
    pRes->WriteFieldBYTE(pStruct, m_nCharges, "Charges");
    pRes->WriteFieldBYTE(pStruct, m_nMaxCharges, "MaxCharges");
    pRes->WriteFieldDWORD(pStruct, GetCost(), "Cost");
    pRes->WriteFieldDWORD(pStruct, m_nAddCost, "AddCost");
    pRes->WriteFieldBYTE(pStruct, m_bPlotObject, "Plot");
    if (g_pRules->m_pBaseItems->GetBaseItem(GetBaseItemType())->m_bIsContainer != 0)
    {
        SaveContainerItems(pRes, pStruct);
    }
    SaveItemProperties(pRes, pStruct);
    pRes->WriteFieldFLOAT(pStruct, m_vPosition.m_fX, "XPosition");
    pRes->WriteFieldFLOAT(pStruct, m_vPosition.m_fY, "YPosition");
    pRes->WriteFieldFLOAT(pStruct, m_vPosition.m_fZ, "ZPosition");
    pRes->WriteFieldFLOAT(pStruct, m_vOrientation.m_fX, "XOrientation");
    pRes->WriteFieldFLOAT(pStruct, m_vOrientation.m_fY, "YOrientation");
    pRes->WriteFieldFLOAT(pStruct, m_vOrientation.m_fZ, "ZOrientation");
    pRes->WriteFieldBYTE(pStruct, m_bNonEquippable, "NonEquippable");
    pRes->WriteFieldBYTE(pStruct, m_bNewItem, "NewItem");
    pRes->WriteFieldBYTE(pStruct, m_bDeleting, "DELETING");
    return 1;
}
// PC 0x0055CFA0  Xbox 0x001DD0D0
int CSWSItem::SaveContainerItems(CResGFF* pRes, CResStruct* pStruct)
{
    CResList list;
    CResStruct itemStruct;

    pRes->AddList(&list, pStruct, "ItemList");
    for (int i = 0; (unsigned int)i < m_pRepository->m_lstItems.num; i++)
    {
        pRes->AddListElement(&itemStruct, &list, i);
        m_pRepository->ItemListGetItem(i)->SaveItem(pRes, &itemStruct);
    }

    return 1;
}
// PC 0x0055D040  Xbox 0x001DD170
void CSWSItem::UpdateUsedActiveProperties()
{
    unsigned char aUsesLeft[MAX_TRACKED_ACTIVE_PROPERTIES];
    int nProperty;
    unsigned char nUsableMask = 0;
    for (nProperty = 0; nProperty < MAX_TRACKED_ACTIVE_PROPERTIES; nProperty++)
        aUsesLeft[nProperty] = 0;

    for (nProperty = 0; nProperty < m_nActiveProperties; nProperty++)
    {
        CSWItemProperty* pItemProperty = GetActiveProperty(nProperty);
        if (pItemProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_ACTIVATE_ITEM)
        {
            switch (pItemProperty->m_nCostValue)
            {
            case CSWItemProperty::ACTIVATE_ITEM_SINGLE_USE:
                if ((unsigned char)pItemProperty->m_bUseable != 0)
                {
                    if (nProperty < MAX_TRACKED_ACTIVE_PROPERTIES)
                        aUsesLeft[nProperty] = GetUsedActivePropertyUsesLeft(nProperty);
                    nUsableMask |= ACTIVE_PROPERTY_MASK << nProperty;
                }
                break;
            case CSWItemProperty::ACTIVATE_ITEM_5_CHARGES:
            case CSWItemProperty::ACTIVATE_ITEM_4_CHARGES:
            case CSWItemProperty::ACTIVATE_ITEM_3_CHARGES:
            case CSWItemProperty::ACTIVATE_ITEM_2_CHARGES:
            case CSWItemProperty::ACTIVATE_ITEM_1_CHARGE:
                if (m_nCharges > 0)
                {
                    if (nProperty < MAX_TRACKED_ACTIVE_PROPERTIES)
                        aUsesLeft[nProperty] = GetUsedActivePropertyUsesLeft(nProperty);
                    nUsableMask |= ACTIVE_PROPERTY_MASK << nProperty;
                }
                break;
            case CSWItemProperty::ACTIVATE_ITEM_1_USE_PER_DAY:
            case CSWItemProperty::ACTIVATE_ITEM_2_USES_PER_DAY:
            case CSWItemProperty::ACTIVATE_ITEM_3_USES_PER_DAY:
            case CSWItemProperty::ACTIVATE_ITEM_4_USES_PER_DAY:
            case CSWItemProperty::ACTIVATE_ITEM_5_USES_PER_DAY:
                if (pItemProperty->m_nUsesPerDay > 0)
                {
                    if (nProperty < MAX_TRACKED_ACTIVE_PROPERTIES)
                        aUsesLeft[nProperty] = GetUsedActivePropertyUsesLeft(nProperty);
                    nUsableMask |= ACTIVE_PROPERTY_MASK << nProperty;
                }
                break;
            case CSWItemProperty::ACTIVATE_ITEM_UNLIMITED:
            case CSWItemProperty::ACTIVATE_ITEM_UNLIMITED_ALT:
            case CSWItemProperty::ACTIVATE_ITEM_RECHARGE_1_MINUTE:
            case CSWItemProperty::ACTIVATE_ITEM_RECHARGE_2_MINUTES:
            case CSWItemProperty::ACTIVATE_ITEM_RECHARGE_3_MINUTES:
            case CSWItemProperty::ACTIVATE_ITEM_RECHARGE_4_MINUTES:
            case CSWItemProperty::ACTIVATE_ITEM_RECHARGE_5_MINUTES:
                nUsableMask |= ACTIVE_PROPERTY_MASK << nProperty;
                break;
            }
        }
    }

    unsigned char nChangedMask = 0;
    for (nProperty = 0; nProperty < MAX_TRACKED_ACTIVE_PROPERTIES; nProperty++)
    {
        if (aUsesLeft[nProperty] != m_aActivePropertyUsesLeft[nProperty])
            nChangedMask |= ACTIVE_PROPERTY_MASK << nProperty;
    }

    if (nUsableMask != m_nUsableActivePropertyMask || nChangedMask != 0)
    {
        m_nUsableActivePropertyMask = nUsableMask;
        for (nProperty = 0; nProperty < MAX_TRACKED_ACTIVE_PROPERTIES; nProperty++)
            m_aActivePropertyUsesLeft[nProperty] = aUsesLeft[nProperty];

        CSWSMessage* pMessage = g_pAppManager->GetServerExoApp()->GetSWSMessage();
        CSWSPlayer* pPlayer = g_pAppManager->m_pServerExoApp->GetClientObjectByObjectId(m_oidPossessor);
        if (pPlayer != NULL)
            pMessage->SendServerToPlayerUpdateActiveItemProperties(
                pPlayer, GetId(), m_nUsableActivePropertyMask,
                nChangedMask, m_aActivePropertyUsesLeft);
    }
}
// PC 0x00553960  Xbox 0x001D3B90
CSWItemProperty* CSWSItem::GetActiveProperty(int nIndex)
{
    if (nIndex < 0 || nIndex >= m_nActiveProperties)
    {
        return NULL;
    }

    return &m_pActiveProperties[nIndex];
}
// PC 0x00553990  Xbox 0x001D3BC0
CSWItemProperty* CSWSItem::GetPassiveProperty(int nIndex)
{
    if (nIndex < 0 || nIndex >= m_nPassiveProperties)
    {
        return NULL;
    }

    return &m_pPassiveProperties[nIndex];
}
// PC 0x005539C0  Xbox 0x001D3BF0
int CSWSItem::GetPropertyByType(CSWItemProperty** ppProperty,
                                unsigned short nPropertyType,
                                unsigned short nSubtype)
{
    int nActiveProperties;
    int nProperty;
    CSWItemProperty* pItemProperty;
    int nPassiveProperties;

    nActiveProperties = m_nActiveProperties;
    for (nProperty = 0; nProperty < nActiveProperties; nProperty++)
    {
        pItemProperty = GetActiveProperty(nProperty);
        if ((pItemProperty->m_nUpgradeType == 0xFF ||
             ((1 << pItemProperty->m_nUpgradeType) & m_nUpgrades) != 0) &&
            GetActiveProperty(nProperty)->m_nPropertyType == nPropertyType)
        {
            if (nSubtype != 0)
            {
                if (GetActiveProperty(nProperty)->m_nSubtype == nSubtype)
                {
                    *ppProperty = GetActiveProperty(nProperty);
                    return 1;
                }
            }
            else
            {
                *ppProperty = GetActiveProperty(nProperty);
                return 1;
            }
        }
        nActiveProperties = m_nActiveProperties;
    }

    nPassiveProperties = m_nPassiveProperties;
    for (nProperty = 0; nProperty < nPassiveProperties; nProperty++)
    {
        pItemProperty = GetPassiveProperty(nProperty);
        if ((pItemProperty->m_nUpgradeType == 0xFF ||
             ((1 << pItemProperty->m_nUpgradeType) & m_nUpgrades) != 0) &&
            GetPassiveProperty(nProperty)->m_nPropertyType == nPropertyType)
        {
            if (nSubtype != 0)
            {
                if (GetPassiveProperty(nProperty)->m_nSubtype == nSubtype)
                {
                    *ppProperty = GetPassiveProperty(nProperty);
                    return 1;
                }
            }
            else
            {
                *ppProperty = GetPassiveProperty(nProperty);
                return 1;
            }
        }
        nPassiveProperties = m_nPassiveProperties;
    }

    return 0;
}
// PC 0x00553BB0  Xbox 0x001D3DE0
void CSWSItem::ApplyItemProperties(CSWSCreature* pCreature,
                                   unsigned long nSlot, int bLoading)
{
    CServerAIMaster* pServerAIMaster;
    int nPassiveProperties;
    int nProperty;
    CSWItemProperty* pItemProperty;
    unsigned char nUpgradeType;

    pServerAIMaster = g_pAppManager->GetServerExoApp()->GetServerAIMaster();
    if (pServerAIMaster != NULL)
    {
        nPassiveProperties = m_nPassiveProperties;
        for (nProperty = 0; nProperty < nPassiveProperties; nProperty++)
        {
            pItemProperty = GetPassiveProperty(nProperty);
            nUpgradeType = pItemProperty->m_nUpgradeType;
            if (nUpgradeType == CSWItemProperty::UPGRADE_TYPE_NONE ||
                ((1 << nUpgradeType) & m_nUpgrades) != 0)
            {
                pServerAIMaster->OnItemPropertyApplied(
                    this, pItemProperty, pCreature, nSlot, bLoading);
            }
            nPassiveProperties = m_nPassiveProperties;
        }
    }
}
// PC 0x00553C30  Xbox 0x001D3E60
void CSWSItem::RemoveItemProperties(CSWSCreature* pCreature,
                                    unsigned long nSlot)
{
    CServerAIMaster* pServerAIMaster;
    int nPassiveProperties;
    int nProperty;
    CSWItemProperty* pItemProperty;
    unsigned char nUpgradeType;

    pServerAIMaster = g_pAppManager->m_pServerExoApp->GetServerAIMaster();
    if (pServerAIMaster != NULL)
    {
        nPassiveProperties = m_nPassiveProperties;
        for (nProperty = 0; nProperty < nPassiveProperties; nProperty++)
        {
            pItemProperty = GetPassiveProperty(nProperty);
            nUpgradeType = pItemProperty->m_nUpgradeType;
            if (nUpgradeType == CSWItemProperty::UPGRADE_TYPE_NONE ||
                ((1 << nUpgradeType) & m_nUpgrades) != 0)
            {
                pServerAIMaster->OnItemPropertyRemoved(
                    this, pItemProperty, pCreature, nSlot);
            }
            nPassiveProperties = m_nPassiveProperties;
        }
    }
    pCreature->RemoveEffectByCreator(m_nId);
}
// PC 0x00553CC0  Xbox 0x001D3EF0
int CSWSItem::ComputeArmorClass()
{
    if (GetBaseItem()->m_bUnknown18 != 1)
    {
        return 0;
    }

    return GetBaseItem()->m_nBaseAC;
}
// PC 0x00553CF0  Xbox 0x001D3F20
int CSWSItem::CompareItem(CSWSItem* pItem)
{
    int nActiveProperty;
    int nPassiveProperty;
    int nSecondPassiveProperty;
    CSWItemProperty* pThisProperty;
    CSWItemProperty* pOtherProperty;

    if (m_nUpgrades != pItem->m_nUpgrades ||
        m_nActiveProperties != pItem->m_nActiveProperties ||
        m_nPassiveProperties != pItem->m_nPassiveProperties ||
        m_bPlotObject != pItem->m_bPlotObject ||
        m_nCharges != pItem->m_nCharges || m_nBaseItem != pItem->m_nBaseItem ||
        m_bStolen != pItem->m_bStolen ||
        m_nModelVariation != pItem->m_nModelVariation ||
        m_nBodyVariation != pItem->m_nBodyVariation ||
        m_nTextureVar != pItem->m_nTextureVar || m_sTag != pItem->m_sTag ||
        m_sLocalizedName != pItem->m_sLocalizedName)
    {
        return 0;
    }

    for (nActiveProperty = 0; nActiveProperty < m_nActiveProperties;
         nActiveProperty++)
    {
        pThisProperty = GetActiveProperty(nActiveProperty);
        pOtherProperty = pItem->GetActiveProperty(nActiveProperty);
        if (pThisProperty->m_nPropertyType != pOtherProperty->m_nPropertyType ||
            pThisProperty->m_nSubtype != pOtherProperty->m_nSubtype ||
            pThisProperty->m_nCostTable != pOtherProperty->m_nCostTable ||
            pThisProperty->m_nCostValue != pOtherProperty->m_nCostValue ||
            pThisProperty->m_nParam1 != pOtherProperty->m_nParam1 ||
            pThisProperty->m_nParam1Value != pOtherProperty->m_nParam1Value ||
            pThisProperty->m_nUsesPerDay != pOtherProperty->m_nUsesPerDay ||
            pThisProperty->m_nUpgradeType != pOtherProperty->m_nUpgradeType)
        {
            return 0;
        }
    }

    for (nPassiveProperty = 0; nPassiveProperty < m_nPassiveProperties;
         nPassiveProperty++)
    {
        pThisProperty = GetPassiveProperty(nPassiveProperty);
        pOtherProperty = pItem->GetPassiveProperty(nPassiveProperty);
        if (pThisProperty->m_nPropertyType != pOtherProperty->m_nPropertyType ||
            pThisProperty->m_nSubtype != pOtherProperty->m_nSubtype ||
            pThisProperty->m_nCostTable != pOtherProperty->m_nCostTable ||
            pThisProperty->m_nCostValue != pOtherProperty->m_nCostValue ||
            pThisProperty->m_nParam1 != pOtherProperty->m_nParam1 ||
            pThisProperty->m_nParam1Value != pOtherProperty->m_nParam1Value ||
            pThisProperty->m_nUsesPerDay != pOtherProperty->m_nUsesPerDay ||
            pThisProperty->m_nUpgradeType != pOtherProperty->m_nUpgradeType)
        {
            return 0;
        }
    }

    for (nSecondPassiveProperty = 0;
         nSecondPassiveProperty < m_nPassiveProperties;
         nSecondPassiveProperty++)
    {
        pThisProperty = GetPassiveProperty(nSecondPassiveProperty);
        pOtherProperty = pItem->GetPassiveProperty(nSecondPassiveProperty);
        if (pThisProperty->m_nPropertyType != pOtherProperty->m_nPropertyType ||
            pThisProperty->m_nSubtype != pOtherProperty->m_nSubtype ||
            pThisProperty->m_nCostTable != pOtherProperty->m_nCostTable ||
            pThisProperty->m_nCostValue != pOtherProperty->m_nCostValue ||
            pThisProperty->m_nParam1 != pOtherProperty->m_nParam1 ||
            pThisProperty->m_nParam1Value != pOtherProperty->m_nParam1Value ||
            pThisProperty->m_nUsesPerDay != pOtherProperty->m_nUsesPerDay ||
            pThisProperty->m_nUpgradeType != pOtherProperty->m_nUpgradeType)
        {
            return 0;
        }
    }

    return 1;
}
// PC 0x00553F90  Xbox 0x001D41C0
int CSWSItem::MergeItem(CSWSItem* pItem)
{
    if (CompareItem(pItem))
    {
        CSWBaseItem* pBaseItem =
            g_pRules->m_pBaseItems->GetBaseItem(GetBaseItemType());
        if (m_nStackSize + pItem->m_nStackSize > pBaseItem->GetMaxStackSize())
        {
            int nOldStackSize = m_nStackSize;
            m_nStackSize = pBaseItem->GetMaxStackSize();
            pItem->m_nStackSize += nOldStackSize - pBaseItem->GetMaxStackSize();
            return 0;
        }

        m_nStackSize += pItem->m_nStackSize;
        return 1;
    }

    return 0;
}
// PC 0x00554000  Xbox 0x001D4240
int CSWSItem::GetCost()
{
    if (m_bPlotObject == 1)
    {
        return 0;
    }

    if ((unsigned int)(GetBaseItem()->m_fCostMultiplier *
                       (double)m_nAddCost) > 1)
    {
        return (int)(GetBaseItem()->m_fCostMultiplier *
                     (double)m_nAddCost);
    }

    return 1;
}
// PC 0x00554050  Xbox 0x001D4290
unsigned long CSWSItem::GetDamageFlags()
{
    unsigned long nDamageFlags;
    int nBaseItem;
    CSWBaseItem* pBaseItem;

    nBaseItem = m_nBaseItem;
    pBaseItem = g_pRules->m_pBaseItems->GetBaseItem(nBaseItem);
    if (pBaseItem == NULL)
    {
        return 8;
    }

    nDamageFlags = pBaseItem->m_nDamageFlags;
    if (nDamageFlags == 0)
    {
        return 8;
    }

    return nDamageFlags;
}
// PC 0x00554080  Xbox 0x001D42C0
unsigned char CSWSItem::GetMinEquipLevel()
{
    CTwoDimArrays* pTables = g_pRules->m_p2DArrays;
    EXOXBOXASSERT(pTables->m_pItemValues);
    C2DA* pItemValues = pTables->m_pItemValues;
    int nLevel;
    int nMaxSingleItemValue;

    for (nLevel = 0; nLevel < pItemValues->m_nNumRows; nLevel++)
    {
        pItemValues->GetINTEntry(nLevel, "MAXSINGLEITEMVALUE",
                                &nMaxSingleItemValue);
        if ((unsigned long)GetCost() <= (unsigned long)nMaxSingleItemValue)
        {
            break;
        }
    }

    unsigned char nRequiredLevel = nLevel;
    nRequiredLevel++;
    return nRequiredLevel;
}
// PC 0x00554170  Xbox 0x001D43C0
void CSWSItem::SetIdentified(int bIdentified)
{
    m_bIdentified = bIdentified;
}
// PC 0x00554190  Xbox 0x001D43E0
CSWCItem* CSWSItem::GetClientItem()
{
    if (GetClientObject() != NULL)
    {
        return GetClientObject()->AsSWCItem();
    }

    return NULL;
}
// PC 0x005541C0  Xbox 0x001D4410
unsigned char CSWSItem::GetUpgradableType(C2DA* pTable)
{
    unsigned char nType;
    int nUpgradeType;
    int nProperty;

    EXOXBOXASSERT(pTable);
    nType = UPGRADABLE_TYPE_NONE;

    for (nProperty = 0; nProperty < m_nActiveProperties; nProperty++)
    {
        if (GetActiveProperty(nProperty)->m_nUpgradeType != CSWItemProperty::UPGRADE_TYPE_NONE)
        {
            {
                CExoString sColumn("UpgradeType");
                unsigned char nUpgradeRow = GetActiveProperty(nProperty)->m_nUpgradeType;
                pTable->GetINTEntry(nUpgradeRow, sColumn, &nUpgradeType);
            }
            switch (nUpgradeType)
            {
            case UPGRADE_ITEM_LIGHTSABER:
                nType = UPGRADABLE_TYPE_LIGHTSABER;
                break;
            case UPGRADE_ITEM_BLASTER_1:
            case UPGRADE_ITEM_BLASTER_2:
            case UPGRADE_ITEM_BLASTER_3:
            case UPGRADE_ITEM_BLASTER_4:
                nType = UPGRADABLE_TYPE_BLASTER;
                break;
            case UPGRADE_ITEM_MELEE_1:
            case UPGRADE_ITEM_MELEE_2:
            case UPGRADE_ITEM_MELEE_3:
                nType = UPGRADABLE_TYPE_MELEE;
                break;
            case UPGRADE_ITEM_ARMOR_1:
            case UPGRADE_ITEM_ARMOR_2:
                nType = UPGRADABLE_TYPE_ARMOR;
                break;
            }
        }
    }

    for (nProperty = 0; nProperty < m_nPassiveProperties; nProperty++)
    {
        if (GetPassiveProperty(nProperty)->m_nUpgradeType != CSWItemProperty::UPGRADE_TYPE_NONE)
        {
            {
                CExoString sColumn("UpgradeType");
                unsigned char nUpgradeRow = GetPassiveProperty(nProperty)->m_nUpgradeType;
                pTable->GetINTEntry(nUpgradeRow, sColumn, &nUpgradeType);
            }
            switch (nUpgradeType)
            {
            case UPGRADE_ITEM_LIGHTSABER:
                nType = UPGRADABLE_TYPE_LIGHTSABER;
                break;
            case UPGRADE_ITEM_BLASTER_1:
            case UPGRADE_ITEM_BLASTER_2:
            case UPGRADE_ITEM_BLASTER_3:
            case UPGRADE_ITEM_BLASTER_4:
                nType = UPGRADABLE_TYPE_BLASTER;
                break;
            case UPGRADE_ITEM_MELEE_1:
            case UPGRADE_ITEM_MELEE_2:
            case UPGRADE_ITEM_MELEE_3:
                nType = UPGRADABLE_TYPE_MELEE;
                break;
            case UPGRADE_ITEM_ARMOR_1:
            case UPGRADE_ITEM_ARMOR_2:
                nType = UPGRADABLE_TYPE_ARMOR;
                break;
            }
        }
    }

    return nType;
}
// PC 0x005543B0  Xbox 0x001D4610
void CSWSItem::AddRangeProperties(CExoString& sString)
{
    CExoString sRange;
    int bRanged = GetBaseItem()->m_nRangedWeapon > 0;
    if (bRanged > 0)
    {
        int nRange = (int)GetBaseItem()->m_fMaxAttackRange;
        sRange.Format("%s: %dm\n", g_pAppManager->GetClientExoApp()->GetGUIString(
            GUI_STRREF_ITEM_RANGE).CStr(), nRange);
        sString = sString + sRange + "\n";
    }
}
// PC 0x005544F0  Xbox 0x001D4750
void CSWSItem::AddWeaponSizeProperties(CExoString& sString)
{
    char nWeaponSize = GetBaseItem()->m_nWeaponSize;
    if (nWeaponSize == 2)
    {
        CExoString sNewLine("\n\n");
        CClientExoApp* pClientExoApp;
        pClientExoApp = g_pAppManager->m_pClientExoApp;
        sString = sString + pClientExoApp->GetGUIString(0xA4A1) + sNewLine;
    }
}
// PC 0x005545D0  Xbox 0x001D4830
void CSWSItem::GetDamageTypeString(unsigned long nDamageFlags,
                                   CExoString& sString)
{
    CClientExoApp* pClientExoApp;
    int nDamageTypes;

    nDamageTypes = 0;
    if ((nDamageFlags & 7) != 0)
    {
        pClientExoApp = g_pAppManager->m_pClientExoApp;
        sString = pClientExoApp->GetGUIString(0x9698);
        nDamageTypes = 1;
    }

    if ((nDamageFlags & 8) != 0)
    {
        if (nDamageTypes > 0)
        {
            sString = sString + ", ";
        }
        pClientExoApp = g_pAppManager->m_pClientExoApp;
        sString = sString + pClientExoApp->GetGUIString(0x9699);
        nDamageTypes++;
    }

    if ((nDamageFlags & 0x10) != 0)
    {
        if (nDamageTypes > 0)
        {
            sString = sString + ", ";
        }
        pClientExoApp = g_pAppManager->m_pClientExoApp;
        sString = sString + pClientExoApp->GetGUIString(0x969A);
        nDamageTypes++;
    }

    if ((nDamageFlags & 0x20) != 0)
    {
        if (nDamageTypes > 0)
        {
            sString = sString + ", ";
        }
        pClientExoApp = g_pAppManager->m_pClientExoApp;
        sString = sString + pClientExoApp->GetGUIString(0x969B);
        nDamageTypes++;
    }

    if ((nDamageFlags & 0x40) != 0)
    {
        if (nDamageTypes > 0)
        {
            sString = sString + ", ";
        }
        pClientExoApp = g_pAppManager->m_pClientExoApp;
        sString = sString + pClientExoApp->GetGUIString(0x969C);
        nDamageTypes++;
    }

    if ((nDamageFlags & 0x80) != 0)
    {
        if (nDamageTypes > 0)
        {
            sString = sString + ", ";
        }
        pClientExoApp = g_pAppManager->m_pClientExoApp;
        sString = sString + pClientExoApp->GetGUIString(0x969D);
        nDamageTypes++;
    }

    if ((nDamageFlags & 0x100) != 0)
    {
        if (nDamageTypes > 0)
        {
            sString = sString + ", ";
        }
        pClientExoApp = g_pAppManager->m_pClientExoApp;
        sString = sString + pClientExoApp->GetGUIString(0x969E);
        nDamageTypes++;
    }

    if ((nDamageFlags & 0x200) != 0)
    {
        if (nDamageTypes > 0)
        {
            sString = sString + ", ";
        }
        pClientExoApp = g_pAppManager->m_pClientExoApp;
        sString = sString + pClientExoApp->GetGUIString(0x969F);
        nDamageTypes++;
    }

    if ((nDamageFlags & 0x400) != 0)
    {
        if (nDamageTypes > 0)
        {
            sString = sString + ", ";
        }
        pClientExoApp = g_pAppManager->m_pClientExoApp;
        sString = sString + pClientExoApp->GetGUIString(0x96A0);
        nDamageTypes++;
    }

    if ((nDamageFlags & 0x800) != 0)
    {
        if (nDamageTypes > 0)
        {
            sString = sString + ", ";
        }
        pClientExoApp = g_pAppManager->m_pClientExoApp;
        sString = sString + pClientExoApp->GetGUIString(0x96A1);
        nDamageTypes++;
    }

    if ((nDamageFlags & 0x1000) != 0)
    {
        if (nDamageTypes > 0)
        {
            sString = sString + ", ";
        }
        pClientExoApp = g_pAppManager->m_pClientExoApp;
        sString = sString + pClientExoApp->GetGUIString(0x96A2);
        nDamageTypes++;
    }

    if ((nDamageFlags & 0x2000) != 0)
    {
        if (nDamageTypes > 0)
        {
            sString = sString + ", ";
        }
        pClientExoApp = g_pAppManager->m_pClientExoApp;
        sString = sString + pClientExoApp->GetGUIString(0xA3AF);
    }
}
// PC 0x00554E00  Xbox 0x001D5060
void CSWSItem::GetPropertyStrings(unsigned short nPropertyType,
                                 unsigned short nSubtype,
                                 unsigned short nCostValue,
                                 unsigned char nParam1Value,
                                 CExoString& sProperty, CExoString& sSubtype,
                                 CExoString& sCost, CExoString& sParam1)
{
    int nName = -1;
    bool bFailed;
    {
        CExoString sColumn("Name");
        C2DA* pTable = g_pRules->Get2DArrays()->m_pItemPropDef;
        bFailed = pTable->GetINTEntry(nPropertyType, sColumn, &nName) == 0;
    }
    if (bFailed)
        return;

    sProperty = g_pAppManager->GetClientExoApp()->GetGUIString(nName);

    CExoString sSubTypeResRef;
    CExoString sCostTableResRef;
    int nTable;

    bool bSuccess;
    {
        CExoString sColumn("SubTypeResRef");
        C2DA* pTable = g_pRules->Get2DArrays()->m_pItemPropDef;
        bSuccess = pTable->GetCExoStringEntry(
            nPropertyType, sColumn, &sSubTypeResRef) == 1;
    }
    if (bSuccess)
    {
        C2DA* pSubtypeTable = new C2DA(sSubTypeResRef, 0);
        if (pSubtypeTable->Load2DArray() == 0)
            return;
        pSubtypeTable->GetINTEntry(nSubtype, "Name", &nName);
        sSubtype = g_pAppManager->GetClientExoApp()->GetGUIString(nName);
        delete pSubtypeTable;
    }

    if (nCostValue != CSWItemProperty::COST_VALUE_NONE)
    {
        {
            CExoString sColumn("CostTableResRef");
            C2DA* pTable = g_pRules->Get2DArrays()->m_pItemPropDef;
            bSuccess = pTable->GetINTEntry(nPropertyType, sColumn, &nTable) == 1;
        }
        if (bSuccess)
        {
            C2DA* pCostTable = g_pRules->Get2DArrays()->GetIPRPCostTable(nTable);
            pCostTable->GetINTEntry(nCostValue, "Name", &nName);
            sCost = g_pAppManager->GetClientExoApp()->GetGUIString(nName);
        }
    }

    if (nParam1Value != CSWItemProperty::PARAM1_VALUE_NONE)
    {
        {
            CExoString sColumn("Param1ResRef");
            C2DA* pTable = g_pRules->Get2DArrays()->m_pItemPropDef;
            bSuccess = pTable->GetINTEntry(nPropertyType, sColumn, &nTable) == 1;
        }
        if (bSuccess)
        {
            {
                CExoString sColumn("Name");
                g_pRules->Get2DArrays()->GetIPRPParamTable(nTable)->GetINTEntry(
                    nParam1Value, sColumn, &nName);
            }
            sParam1 = g_pAppManager->GetClientExoApp()->GetGUIString(nName);
        }
        else
        {
            {
                CExoString sColumn("SubTypeResRef");
                C2DA* pTable = g_pRules->Get2DArrays()->m_pItemPropDef;
                bSuccess = pTable->GetCExoStringEntry(
                    nPropertyType, sColumn, &sSubTypeResRef) == 1;
            }
            if (!bSuccess)
                return;
            C2DA* pSubtypeTable = new C2DA(sSubTypeResRef, 0);
            if (pSubtypeTable->Load2DArray() == 0)
            {
                delete pSubtypeTable;
                return;
            }
            {
                if (pSubtypeTable->GetINTEntry(nSubtype, "Param1ResRef", &nTable) == 1)
                {
                    g_pRules->Get2DArrays()->GetIPRPParamTable(nTable)->GetINTEntry(
                        nParam1Value, "Name", &nName);
                    sParam1 = g_pAppManager->GetClientExoApp()->GetGUIString(nName);
                }
            }
            delete pSubtypeTable;
        }
    }
}
// PC 0x00555380  Xbox 0x001D55E0
void CSWSItem::GetPropertyDamageRange(unsigned short nCostValue,
                                     int& nMinDamage, int& nMaxDamage)
{
    int nNumDice = 0;
    int nDie = 0;
    bool bSuccess;
    CTwoDimArrays* pTables;
    C2DA* pTable;

    {
        CExoString sColumn("NumDice");
        pTables = g_pRules->m_p2DArrays;
        pTable = pTables->m_pIPRPDamage;
        bSuccess = pTable->GetINTEntry(nCostValue, sColumn, &nNumDice) == 1;
    }
    if (bSuccess)
    {
        if (nNumDice == 0)
        {
            nMinDamage = 1;
            nMaxDamage = nCostValue;
        }
        else
        {
            {
                CExoString sColumn("Die");
                pTables = g_pRules->m_p2DArrays;
                pTable = pTables->m_pIPRPDamage;
                pTable->GetINTEntry(nCostValue, sColumn, &nDie);
            }
            nMinDamage = nNumDice;
            nMaxDamage = nNumDice * nDie;
        }
    }
    else
    {
        nMinDamage = nCostValue;
        nMaxDamage = nCostValue;
    }
}
void CSWSItem::GetPropertyChanceDurationDC(unsigned short nCostValue,  // PC 0x005554D0  Xbox 0x001D5730
                                         unsigned char nParam1Value,
                                         int& nEffectChance,
                                         int& nDurationRounds, int& nDC)
{
    int nValue;
    CTwoDimArrays* pTables;
    C2DA* pTable;

    {
        CExoString sColumn("EffectChance");
        pTables = g_pRules->m_p2DArrays;
        EXOXBOXASSERT(pTables->m_pOnHitDuration);
        pTable = pTables->m_pOnHitDuration;
        pTable->GetINTEntry(nParam1Value, sColumn, &nValue);
    }
    nEffectChance = nValue;

    {
        CExoString sColumn("DurationRounds");
        pTables = g_pRules->m_p2DArrays;
        EXOXBOXASSERT(pTables->m_pOnHitDuration);
        pTable = pTables->m_pOnHitDuration;
        pTable->GetINTEntry(nParam1Value, sColumn, &nValue);
    }
    nDurationRounds = nValue;

    {
        CExoString sColumn("Value");
        pTables = g_pRules->m_p2DArrays;
        pTables->GetIPRPCostTable(CTwoDimArrays::IPRP_COST_TABLE_DC)->GetINTEntry(
            nCostValue, sColumn, &nValue);
    }
    nDC = nValue;
}
// PC 0x00555610  Xbox 0x001D5880
int CSWSItem::GetExistsHigherValuedProperty(unsigned short nPropertyType,
                                            unsigned short nSubtype,
                                            unsigned short nCostValue)
{
    int nPassiveProperties;
    int nProperty;
    CSWItemProperty* pItemProperty;

    nPassiveProperties = m_nPassiveProperties;
    for (nProperty = 0; nProperty < nPassiveProperties; nProperty++)
    {
        pItemProperty = GetPassiveProperty(nProperty);
        if (pItemProperty != NULL &&
            (pItemProperty->m_nUpgradeType == 0xFF ||
             ((1 << pItemProperty->m_nUpgradeType) & m_nUpgrades) != 0) &&
            pItemProperty->m_nPropertyType == nPropertyType &&
            pItemProperty->m_nSubtype == nSubtype &&
            pItemProperty->m_nCostValue > nCostValue)
        {
            return 1;
        }
    }

    return 0;
}

void CSWSItem::PowerItem(int bPower, int bAnimate, int bSilent)
{
    CSWCItem* pClientItem = GetClientItem();
    if (pClientItem != NULL)
    {
        pClientItem->PowerItem(bPower, FALSE, TRUE, FALSE);
    }
}
// PC 0x00555690  Xbox 0x001D5900
int CSWSItem::IsLightsaber()
{
    int nBaseItem;
    int nResult;

    nBaseItem = m_nBaseItem;
    nResult = 0;
    if (nBaseItem == BASE_ITEM_LIGHTSABER ||
        nBaseItem == BASE_ITEM_DOUBLE_BLADED_LIGHTSABER ||
        nBaseItem == BASE_ITEM_SHORT_LIGHTSABER)
    {
        nResult = 1;
    }

    return nResult;
}
// PC 0x005556B0  Xbox 0x001D5920
CResRef CSWSItem::GetIcon()
{
    CSWBaseItem* pBaseItem = GetBaseItem();

    if (pBaseItem == NULL)
    {
        return CResRef("");
    }

    if (pBaseItem->m_bUnknown18 != 0)
    {
        return pBaseItem->GetIconResRef((unsigned char)m_nTextureVar, 'm');
    }

    return pBaseItem->GetIconResRef((unsigned char)m_nModelVariation, 'm');
}
// PC 0x0055EC70  Xbox 0x001DECD0
CSWSItem::~CSWSItem()
{
    unsigned long oidClient =
        g_pAppManager->GetClientExoApp()->ServerToClientObjectId(GetId());
    CSWCItem* pClientItem =
        g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(oidClient);
    if (pClientItem != NULL)
    {
        CGameObject* pObject;
        if (g_pAppManager->GetClientExoApp()->GetGameObjectArray()->Delete(
                oidClient, &pObject) == CGameObjectArray::SUCCESS &&
            pObject != NULL)
            delete pClientItem;
    }

    CSWSModule* pModule = g_pAppManager->GetServerExoApp()->GetModule();
    if (pModule != NULL)
        pModule->RemoveObjectFromLookupTable(m_sTag, GetId());

    if (m_pRepository != NULL)
    {
        delete m_pRepository;
        m_pRepository = NULL;
    }
    if (m_pActiveProperties != NULL)
    {
        delete[] m_pActiveProperties;
        m_pActiveProperties = NULL;
    }
    if (m_pPassiveProperties != NULL)
    {
        delete[] m_pPassiveProperties;
        m_pPassiveProperties = NULL;
    }
    if (GetArea() != NULL)
        RemoveFromArea();
    g_pAppManager->GetServerExoApp()->GetServerAIMaster()->RemoveObject(this);
}
// PC 0x0055EE10  Xbox 0x001DEE70
void CSWSItem::EventHandler(unsigned long nEventId, unsigned long oidCaller,
                           void* pEventData, unsigned long nCalendarDay,
                           unsigned long nTimeOfDay)
{
    if (nEventId == CServerAIMaster::EVENT_TIMED_EVENT)
    {
        if (pEventData != NULL)
            g_pVirtualMachine->RunScriptSituation(pEventData, GetId(), 1);
    }
    else if (nEventId == CServerAIMaster::EVENT_DECREMENT_STACKSIZE)
    {
        unsigned long oidPossessor = m_oidPossessor;
        CSWSCreature* pCreature =
            g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidPossessor);
        if (m_nStackSize > 1)
            --m_nStackSize;
        else
            g_pAppManager->GetServerExoApp()->GetServerAIMaster()->AddEventDeltaTime(
                0, 0, GetId(), GetId(), CServerAIMaster::EVENT_DESTROY_OBJECT, NULL);

        if (pCreature != NULL && pCreature->m_pInventory->GetItemInInventory(this))
            pCreature->m_nEquippedWeight -=
                g_pRules->m_pBaseItems->GetBaseItem(GetBaseItemType())->m_nTenthLBS;
    }
    else if (nEventId == CServerAIMaster::EVENT_DESTROY_OBJECT)
    {
        if (m_nEC != 0)
        {
            CloseItemForAllPlayers();
            unsigned long oidPossessor = m_oidPossessor;
            CGameObject* pPossessor =
                g_pAppManager->GetServerExoApp()->GetGameObject(oidPossessor);
            if (pPossessor != NULL)
            {
                if (pPossessor->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
                {
                    CSWSCreature* pCreature = pPossessor->AsSWSCreature();
                    pCreature->RemoveItem(this, 0, 1, NULL);
                    if (pCreature->m_bInParty && pEventData == (void*)1)
                        g_pAppManager->GetClientExoApp()->GetInGameGui()->UpdateStatus(
                            GUI_STATUS_ITEM_LOST, 0);
                }
                else if (pPossessor->m_nObjectType == CGameObject::OBJECT_TYPE_ITEM)
                {
                    CSWSItem* pItem = pPossessor->AsSWSItem();
                    if (pItem->m_pRepository->GetItemInRepository(this, 0) == 1)
                        pItem->m_pRepository->RemoveItem(this);
                    SetPossessor(OBJECT_INVALID, 1, 1, OBJECT_INVALID);
                }
                else if (pPossessor->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
                {
                    CSWSPlaceable* pPlaceable = pPossessor->AsSWSPlaceable();
                    if (pPlaceable->m_bHasInventory == 1 &&
                        pPlaceable->m_pcItemRepository->GetItemInRepository(this, 0) == 1)
                        pPlaceable->m_pcItemRepository->RemoveItem(this);
                    SetPossessor(OBJECT_INVALID, 1, 1, OBJECT_INVALID);
                }
                else if (pPossessor->m_nObjectType == CGameObject::OBJECT_TYPE_STORE)
                {
                    CSWSStore* pStore = pPossessor->AsSWSStore();
                    if (pStore->GetItemInInventory(GetId()) == 1)
                        pStore->RemoveItem(this);
                    SetPossessor(OBJECT_INVALID, 1, 1, OBJECT_INVALID);
                }
            }
            RemoveFromArea();
            delete this;
        }
    }
}
// PC 0x0055FCD0  Xbox 0x001DFD50
void CSWSItem::LoadDataFromGff(CResGFF* pRes, CResStruct* pStruct)
{
    int bSuccess;
    m_nBaseItem = pRes->ReadFieldINT(pStruct, "BaseItem", bSuccess, GetBaseItemType());
    InitRepository(GetId());
    SetTag(pRes->ReadFieldCExoString(pStruct, "Tag", bSuccess, m_sTag));
    SetIdentified(pRes->ReadFieldBYTE(pStruct, "Identified", bSuccess, 1));
    m_sDescription = pRes->ReadFieldCExoLocString(pStruct, "Description", bSuccess, m_sDescIdentified);
    m_sDescIdentified = pRes->ReadFieldCExoLocString(pStruct, "DescIdentified", bSuccess, m_sDescription);
    m_sLocalizedName = pRes->ReadFieldCExoLocString(pStruct, "LocalizedName", bSuccess, m_sLocalizedName);
    m_nStackSize = pRes->ReadFieldWORD(pStruct, "StackSize", bSuccess, GetStackSize());
    SetStolen(pRes->ReadFieldBYTE(pStruct, "Stolen", bSuccess, m_bStolen));
    m_nUpgrades = pRes->ReadFieldDWORD(pStruct, "Upgrades", bSuccess, GetUpgrades());
    SetDropable(pRes->ReadFieldBYTE(pStruct, "Dropable", bSuccess, 0));
    SetPickpocketable(pRes->ReadFieldBYTE(pStruct, "Pickpocketable", bSuccess, 0));
    SetNonEquippable(pRes->ReadFieldBYTE(pStruct, "NonEquippable", bSuccess, m_bNonEquippable));
    unsigned char nModelVariation = m_nModelVariation;
    nModelVariation = pRes->ReadFieldBYTE(pStruct, "ModelVariation", bSuccess, nModelVariation);
    if (!bSuccess)
    {
        nModelVariation = pRes->ReadFieldBYTE(pStruct, "ModelPart1", bSuccess, nModelVariation);
        if (nModelVariation == 0)
            nModelVariation = 1;
    }
    m_nModelVariation = nModelVariation;
    switch (g_pRules->GetBaseItemArray()->GetBaseItem(GetBaseItemType())->m_bUnknown18)
    {
    case BASEITEM_MODEL_TYPE_BODY_VARIATION:
        m_nBodyVariation = g_pRules->m_pBaseItems->GetBaseItem(GetBaseItemType())->m_nBodyVar;
        m_nTextureVar = pRes->ReadFieldBYTE(pStruct, "TextureVar", bSuccess, 1);
        break;
    }
    int nCharges = pRes->ReadFieldBYTE(pStruct, "Charges", bSuccess, DEFAULT_CHARGES);
    m_nCharges = nCharges;
    m_nMaxCharges = pRes->ReadFieldBYTE(pStruct, "MaxCharges", bSuccess, nCharges);
    SetNewItem(pRes->ReadFieldBYTE(pStruct, "NewItem", bSuccess, m_bNewItem));
    int bDeleting = pRes->ReadFieldBYTE(pStruct, "DELETING", bSuccess, m_bDeleting);
    m_bDeleting = bDeleting;
    m_nAddCost = pRes->ReadFieldDWORD(pStruct, "AddCost", bSuccess, m_nAddCost);
    if (g_pRules->m_pBaseItems->GetBaseItem(GetBaseItemType())->m_bIsContainer)
        ReadContainerItemsFromGff(pRes, pStruct);
    m_bPlotObject = pRes->ReadFieldBYTE(pStruct, "Plot", bSuccess, GetPlotObject());

    CResList lstProperties;
    CResStruct propertyStruct;
    if (pRes->GetList(&lstProperties, pStruct, "PropertiesList") == 1 &&
        pRes->GetListCount(&lstProperties) > 0)
    {
        int nNumProperties = pRes->GetListCount(&lstProperties);
        int nProperty;
        int nActiveProperties;
        for (nProperty = 0, nActiveProperties = 0; nProperty < nNumProperties; nProperty++)
        {
            pRes->GetListElement(&propertyStruct, &lstProperties, nProperty);
            if (pRes->GetFieldCount(&propertyStruct) != 0)
            {
                unsigned short nPropertyType = pRes->ReadFieldWORD(&propertyStruct, "PropertyName", bSuccess, 0);
                if (bSuccess && ActiveProperty(nPropertyType))
                    nActiveProperties++;
            }
        }
        SetNumActiveProperties(nActiveProperties);

        int nActiveProperty = 0;
        for (nProperty = 0; nProperty < nNumProperties; nProperty++)
        {
            pRes->GetListElement(&propertyStruct, &lstProperties, nProperty);
            if (pRes->GetFieldCount(&propertyStruct) != 0)
            {
                unsigned short nPropertyType = pRes->ReadFieldWORD(&propertyStruct, "PropertyName", bSuccess, 0);
                switch (nPropertyType)
                {
                case CSWItemProperty::ITEM_PROPERTY_ACTIVATE_ITEM:
                case CSWItemProperty::ITEM_PROPERTY_SECURITY_SPIKE:
                case CSWItemProperty::ITEM_PROPERTY_TRAP:
                case CSWItemProperty::ITEM_PROPERTY_COMPUTER_SPIKE:
                    if (bSuccess)
                        GetActiveProperty(nActiveProperty)->m_nPropertyType = nPropertyType;
                    unsigned short nSubtype = pRes->ReadFieldWORD(&propertyStruct, "Subtype", bSuccess, 0);
                    if (bSuccess)
                        GetActiveProperty(nActiveProperty)->m_nSubtype = nSubtype;
                    unsigned char nCostTable = pRes->ReadFieldBYTE(&propertyStruct, "CostTable", bSuccess, 0);
                    if (bSuccess)
                        GetActiveProperty(nActiveProperty)->m_nCostTable = nCostTable;
                    unsigned short nCostValue = pRes->ReadFieldWORD(&propertyStruct, "CostValue", bSuccess, 0);
                    if (bSuccess)
                        GetActiveProperty(nActiveProperty)->m_nCostValue = nCostValue;
                    unsigned char nParam1 = pRes->ReadFieldBYTE(&propertyStruct, "Param1", bSuccess, 0);
                    if (bSuccess)
                        GetActiveProperty(nActiveProperty)->m_nParam1 = nParam1;
                    unsigned char nParam1Value = pRes->ReadFieldBYTE(&propertyStruct, "Param1Value", bSuccess, 0);
                    if (bSuccess)
                        GetActiveProperty(nActiveProperty)->m_nParam1Value = nParam1Value;
                    unsigned char nChanceAppear = pRes->ReadFieldBYTE(&propertyStruct, "ChanceAppear", bSuccess, 0);
                    if (bSuccess)
                        GetActiveProperty(nActiveProperty)->m_nChanceAppear = nChanceAppear;
                    unsigned char bUseable = pRes->ReadFieldBYTE(&propertyStruct, "Useable", bSuccess, 1);
                    GetActiveProperty(nActiveProperty)->m_bUseable = bUseable;
                    unsigned char nUsesPerDay = pRes->ReadFieldBYTE(&propertyStruct, "UsesPerDay", bSuccess, CSWItemProperty::UPGRADE_TYPE_NONE);
                    GetActiveProperty(nActiveProperty)->m_nUsesPerDay = nUsesPerDay;
                    unsigned char nUpgradeType = pRes->ReadFieldBYTE(&propertyStruct, "UpgradeType", bSuccess, CSWItemProperty::UPGRADE_TYPE_NONE);
                    GetActiveProperty(nActiveProperty)->m_nUpgradeType = nUpgradeType;
                    nActiveProperty++;
                    break;
                }
            }
        }
        SetNumPassiveProperties(nNumProperties - m_nActiveProperties);

        int nPassiveProperty = 0;
        for (nProperty = 0; nProperty < nNumProperties; nProperty++)
        {
            pRes->GetListElement(&propertyStruct, &lstProperties, nProperty);
            if (pRes->GetFieldCount(&propertyStruct) != 0)
            {
                unsigned short nPropertyType = pRes->ReadFieldWORD(&propertyStruct, "PropertyName", bSuccess, 0);
                switch (nPropertyType)
                {
                case CSWItemProperty::ITEM_PROPERTY_ACTIVATE_ITEM:
                case CSWItemProperty::ITEM_PROPERTY_SECURITY_SPIKE:
                case CSWItemProperty::ITEM_PROPERTY_TRAP:
                case CSWItemProperty::ITEM_PROPERTY_COMPUTER_SPIKE:
                    break;
                default:
                    if (bSuccess)
                        GetPassiveProperty(nPassiveProperty)->m_nPropertyType = nPropertyType;
                    unsigned short nSubtype = pRes->ReadFieldWORD(&propertyStruct, "Subtype", bSuccess, 0);
                    if (bSuccess)
                        GetPassiveProperty(nPassiveProperty)->m_nSubtype = nSubtype;
                    unsigned char nCostTable = pRes->ReadFieldBYTE(&propertyStruct, "CostTable", bSuccess, 0);
                    if (bSuccess)
                        GetPassiveProperty(nPassiveProperty)->m_nCostTable = nCostTable;
                    unsigned short nCostValue = pRes->ReadFieldWORD(&propertyStruct, "CostValue", bSuccess, 0);
                    if (bSuccess)
                        GetPassiveProperty(nPassiveProperty)->m_nCostValue = nCostValue;
                    unsigned char nParam1 = pRes->ReadFieldBYTE(&propertyStruct, "Param1", bSuccess, 0);
                    if (bSuccess)
                        GetPassiveProperty(nPassiveProperty)->m_nParam1 = nParam1;
                    unsigned char nParam1Value = pRes->ReadFieldBYTE(&propertyStruct, "Param1Value", bSuccess, 0);
                    if (bSuccess)
                        GetPassiveProperty(nPassiveProperty)->m_nParam1Value = nParam1Value;
                    unsigned char nChanceAppear = pRes->ReadFieldBYTE(&propertyStruct, "ChanceAppear", bSuccess, 0);
                    if (bSuccess)
                        GetPassiveProperty(nPassiveProperty)->m_nChanceAppear = nChanceAppear;
                    unsigned char bUseable = pRes->ReadFieldBYTE(&propertyStruct, "Useable", bSuccess, 0);
                    GetPassiveProperty(nPassiveProperty)->m_bUseable = bUseable;
                    unsigned char nUsesPerDay = pRes->ReadFieldBYTE(&propertyStruct, "UsesPerDay", bSuccess, CSWItemProperty::UPGRADE_TYPE_NONE);
                    GetPassiveProperty(nPassiveProperty)->m_nUsesPerDay = nUsesPerDay;
                    unsigned char nUpgradeType = pRes->ReadFieldBYTE(&propertyStruct, "UpgradeType", bSuccess, CSWItemProperty::UPGRADE_TYPE_NONE);
                    GetPassiveProperty(nPassiveProperty)->m_nUpgradeType = nUpgradeType;
                    nPassiveProperty++;
                    break;
                }
            }
        }
    }
    RestoreUsedActiveProperties(1);
    UpdateUsedActiveProperties();
    if (m_nActiveProperties == 0 && m_nPassiveProperties == 0)
        m_bIdentified = 1;
    m_nBaseAC = ComputeArmorClass();
    g_pAppManager->GetServerExoApp()->GetModule()->AddObjectToLookupTable(m_sTag, GetId());
    float fWeight = (float)(GetBaseItem()->m_nTenthLBS * 1.0);
    m_nWeight = (int)fWeight;
}
// PC 0x0055F050  Xbox 0x001DF0B0
void CSWSItem::InitRepository(unsigned long oidOwner)
{
    if (m_pRepository != NULL)
    {
        delete m_pRepository;
        m_pRepository = NULL;
    }
    EXOXBOXASSERT(g_pRules->m_pBaseItems->GetBaseItem(GetBaseItemType()));
    if (g_pRules->m_pBaseItems->GetBaseItem(GetBaseItemType())->m_bIsContainer)
    {
        m_pRepository = new CItemRepository(oidOwner, 0);
    }
}
// PC 0x0055F0F0  Xbox 0x001DF170
void CSWSItem::ReadContainerItemsFromGff(CResGFF* pRes, CResStruct* pStruct)
{
    CResList list;
    CResStruct itemStruct;
    int bObjectIdSuccess;
    int bFieldSuccess;
    unsigned long oidItem;
    CSWSItem* pNewItem;
    unsigned long i;

    if (m_pRepository == NULL)
    {
        return;
    }

    if (pRes->GetList(&list, pStruct, "ItemList") != 1)
    {
        return;
    }

    for (i = 0; i < pRes->GetListCount(&list); i++)
    {
        pRes->GetListElement(&itemStruct, &list, i);
        oidItem = pRes->ReadFieldDWORD(&itemStruct, "ObjectId", bObjectIdSuccess,
                                       OBJECT_INVALID);
        pNewItem = new CSWSItem(oidItem);
        {
            CSWSItem* pItem = pNewItem;
            if (pNewItem == NULL)
            {
                continue;
            }

            if (!pNewItem->LoadItem(pRes, &itemStruct))
            {
                delete pNewItem;
                continue;
            }

            pRes->ReadFieldBYTE(&itemStruct, "Repos_PosX", bFieldSuccess, CSWItemProperty::UPGRADE_TYPE_NONE);
            pRes->ReadFieldBYTE(&itemStruct, "Repos_Posy", bFieldSuccess, CSWItemProperty::UPGRADE_TYPE_NONE);
            if (m_pRepository->AddItem(&pItem, 1, 0, 1) && pItem != NULL)
            {
                pItem->SetPossessor(m_nId, 1, 0, OBJECT_INVALID);
            }
        }
    }
}
// PC 0x0055F280  Xbox 0x001DF300
CSWSItem* CSWSItem::SplitItem(int nCount)
{
    CSWSItem* pNewItem;

    if (nCount <= 0 || nCount >= m_nStackSize)
    {
        return NULL;
    }

    pNewItem = new CSWSItem(OBJECT_INVALID);
    pNewItem->CopyItem(this);
    if (pNewItem != NULL)
    {
        m_nStackSize -= nCount;
        pNewItem->m_nStackSize = nCount;
    }

    return pNewItem;
}
// PC 0x0055F330  Xbox 0x001DF3B0
int CItemRepository::ClearItems()
{
    m_lstItems.RemoveAll(0);
    m_nNewItems = 0;
    return 0;
}
// PC 0x0055F340  Xbox 0x001DF3C0
CExoString CSWSItem::GetPropertyDescription()
{
    CExoArrayList<CExoString> aStrings;
    CExoString sString;

    if (GetBaseItem()->GetItemType() != CSWBaseItem::ITEM_TYPE_NO_PROPERTY_DESCRIPTION &&
        GetBaseItem()->GetItemType() != CSWBaseItem::ITEM_TYPE_HOSTILE_USABLE)
    {
        AddFeatRequirements(sString);
        if (GetBaseItem()->m_nWeaponType != 0)
        {
            AddDamageProperties(sString);
            AddRangeProperties(sString);
            AddCriticalThreatProperties(sString);
            AddOnHitProperties(sString);
            AddWeaponSizeProperties(sString);
        }
        AddAttackModifierProperties(sString);
        AddDefenceProperties(sString);
        AddMiscellaneousProperties(sString);
    }

    CExoString sDescription;
    CExoLocString* pDescription = &m_sDescIdentified;
    pDescription->GetString(g_pAppManager->GetClientExoApp()->GetClientLanguage(), &sDescription, 0);
    if (sDescription == "")
    {
        sDescription = g_pAppManager->GetClientExoApp()->GetGUIString(CClientExoApp::GUI_STRREF_NO_DESCRIPTION);
    }
    sDescription = sString + sDescription;
    return sDescription;
}
// PC 0x0055F510  Xbox 0x001DF590
CExoString CSWSItem::GetKeyedPropertyString(unsigned char nUpgradeIndex)
{
    CExoArrayList<CExoString> aStrings;
    int nProperty;
    CSWItemProperty* pProperty;
    int nActiveProperties;
    int nPassiveProperties;

    nActiveProperties = m_nActiveProperties;
    for (nProperty = 0; nProperty < nActiveProperties; nProperty++)
    {
        pProperty = GetActiveProperty(nProperty);
        if (pProperty != NULL && pProperty->m_nUpgradeType == nUpgradeIndex)
        {
            if (pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_ENHANCEMENT_BONUS)
            {
                CExoString sProperty;
                int nCostValue = pProperty->m_nCostValue;
                if (nCostValue > 0)
                {
                    sProperty = sProperty +
                        g_pAppManager->GetClientExoApp()->GetGUIString(CClientExoApp::GUI_STRREF_ATTACK) +
                        " +" + CExoString(pProperty->m_nCostValue) + ", " +
                        g_pAppManager->GetClientExoApp()->GetGUIString(CClientExoApp::GUI_STRREF_DAMAGE) +
                        " +" + CExoString(pProperty->m_nCostValue);
                }
                aStrings.Add(sProperty);
            }
            else
            {
                unsigned char nParam1Value = pProperty->m_nParam1Value;
                unsigned short nCostValue = pProperty->m_nCostValue;
                unsigned short nSubtype = pProperty->m_nSubtype;
                AddProperty(pProperty->m_nPropertyType, nSubtype,
                            nCostValue, nParam1Value, aStrings);
            }
        }
        nActiveProperties = m_nActiveProperties;
    }

    nPassiveProperties = m_nPassiveProperties;
    for (nProperty = 0; nProperty < nPassiveProperties; nProperty++)
    {
        pProperty = GetPassiveProperty(nProperty);
        if (pProperty != NULL && pProperty->m_nUpgradeType == nUpgradeIndex)
        {
            if (pProperty->m_nPropertyType == CSWItemProperty::ITEM_PROPERTY_ENHANCEMENT_BONUS)
            {
                CExoString sProperty;
                int nCostValue = pProperty->m_nCostValue;
                if (nCostValue > 0)
                {
                    sProperty = sProperty +
                        g_pAppManager->GetClientExoApp()->GetGUIString(CClientExoApp::GUI_STRREF_ATTACK) +
                        " +" + CExoString(pProperty->m_nCostValue) + ", " +
                        g_pAppManager->GetClientExoApp()->GetGUIString(CClientExoApp::GUI_STRREF_DAMAGE) +
                        " +" + CExoString(pProperty->m_nCostValue);
                }
                aStrings.Add(sProperty);
            }
            else
            {
                unsigned char nParam1Value = pProperty->m_nParam1Value;
                unsigned short nCostValue = pProperty->m_nCostValue;
                unsigned short nSubtype = pProperty->m_nSubtype;
                AddProperty(pProperty->m_nPropertyType, nSubtype,
                            nCostValue, nParam1Value, aStrings);
            }
        }
        nPassiveProperties = m_nPassiveProperties;
    }

    CExoString sString;
    GetSortedPropertyStrings(aStrings, sString);
    return sString;
}
// PC 0x005608B0  Xbox 0x001E0930
int CSWSItem::LoadFromTemplate(const CResRef& cResRef)
{
    CResGFF* pGFF;
    CResStruct resStruct;

    pGFF = new CResGFF(RESTYPE_UTI, "UTI ", cResRef);
    if (pGFF->m_bLoaded == 0)
    {
        delete pGFF;
        return 0;
    }

    pGFF->GetTopLevelStruct(&resStruct);
    LoadDataFromGff(pGFF, &resStruct);
    delete pGFF;
    return 1;
}
// PC 0x00560970  Xbox 0x001E09F0
int CSWSItem::LoadItem(CResGFF* pRes, CResStruct* pStruct)
{
    int bSuccess = 0;
    CResRef cEquippedRes = pRes->ReadFieldCResRef(pStruct, "EquippedRes", bSuccess, "");
    if (bSuccess)
    {
        bSuccess = LoadFromTemplate(cEquippedRes);
        int bFieldSuccess;
        int bDropable = pRes->ReadFieldBYTE(pStruct, "Dropable", bFieldSuccess, 0);
        if (bFieldSuccess)
            m_bDropable = bDropable;
        int bPickpocketable = pRes->ReadFieldBYTE(pStruct, "Pickpocketable", bFieldSuccess, 0);
        if (bFieldSuccess)
            m_bPickpocketable = bPickpocketable;
        return bSuccess;
    }
    else
    {
        bSuccess = 0;
        CResRef cInventoryRes = pRes->ReadFieldCResRef(pStruct, "InventoryRes", bSuccess, "");
        if (bSuccess)
        {
            bSuccess = LoadFromTemplate(cInventoryRes);
            int bFieldSuccess;
            int bDropable = pRes->ReadFieldBYTE(pStruct, "Dropable", bFieldSuccess, 0);
            if (bFieldSuccess)
                m_bDropable = bDropable;
            int bPickpocketable = pRes->ReadFieldBYTE(pStruct, "Pickpocketable", bFieldSuccess, 0);
            if (bFieldSuccess)
                m_bPickpocketable = bPickpocketable;
        }
        else
        {
            LoadDataFromGff(pRes, pStruct);
            return 1;
        }
    }
    return bSuccess;
}

static inline void NwsitemTrace1()
{
}
