#include "exobase.h"
#include "exoresman.h"
#include "resgff.h"
#include "appmanager.h"
#include "serverexoapp.h"
#include "serveraimaster.h"
#include "nwsarea.h"
#include "nwsitem.h"
#include "nwsmodule.h"
#include "nwsobject.h"
#include "nwsscriptevent.h"
#include "nwscript.h"
#include "gameobjectarray.h"
#include "nwsstore.h"

static inline void NwsstoreTrace2();
static inline void NwsstoreTrace1();
#define NWSSTORE_TRACE() NwsstoreTrace2()
// PC 0x005C6AB0  Xbox 0x00213DB0
CSWSStore::CSWSStore(unsigned long oidStore)
    : CSWSObject(CGameObject::OBJECT_TYPE_STORE, oidStore, 0)
{
    m_nInventoryIndex = 0;
    m_pItemRepository = new CItemRepository(m_nId, 0);
    m_nBonusMarkDown = 0;
    m_nBonusMarkUp = 0;
    m_nBuySellFlag = 3;
    g_pAppManager->m_pServerExoApp->GetServerAIMaster()->AddObject(this, 0);
}
// PC 0x004015A0
void CSWSStore::AIUpdate()
{
}

unsigned long CSWSStore::CalculateItemBuyPrice(CSWSItem* pItem,
                                               unsigned long oidCreature)
{
    if (pItem->m_bPlotObject != 0)
    {
        return 0;
    }
    int nCost = pItem->GetCost();
    if (nCost == 0)
    {
        return 0;
    }
    if (nCost <= 1)
    {
        return 1;
    }
    return nCost;
}

unsigned long CSWSStore::CalculateItemSellPrice(CSWSItem* pItem,
                                                unsigned long oidCreature)
{
    if (pItem->m_bPlotObject != 0)
    {
        return 0;
    }
    int nCost = pItem->GetCost();
    if (nCost <= 1)
    {
        return 1;
    }
    return nCost;
}
// PC 0x005C6BA0  Xbox 0x00213EA0
int CSWSStore::RemoveItem(CSWSItem* pItem)
{
    return m_pItemRepository->RemoveItem(pItem);
}
// PC 0x005C6BB0  Xbox 0x00213EB0
int CSWSStore::GetItemInInventory(unsigned long oidItem)
{
    CServerExoApp* pServerExoApp;
    CSWSItem* pItem;

    pServerExoApp = g_pAppManager->m_pServerExoApp;
    pItem = pServerExoApp->GetItemByGameObjectID(oidItem);

    return pItem != NULL &&
           m_pItemRepository->GetItemInRepository(pItem, 0) != 0;
}
// PC 0x005C6BF0  Xbox 0x00213EF0
CSWSStore::~CSWSStore()
{
    delete m_pItemRepository;
    m_n240 = OBJECT_INVALID;
    GetArea();
    g_pAppManager->m_pServerExoApp->GetServerAIMaster()->RemoveObject(this);

    CSWSModule* pModule = g_pAppManager->m_pServerExoApp->GetModule();
    if (pModule != NULL)
    {
        unsigned long oidObject = m_nId;
        pModule->RemoveObjectFromLookupTable(m_sTag, oidObject);
    }
}
// PC 0x005C6CD0  Xbox 0x00213FD0
int CSWSStore::SaveStore(CResGFF* pRes, CResStruct* pStruct)
{
    NwsstoreTrace1();
    pRes->WriteFieldCExoString(pStruct, m_sTag, "Tag");
    pRes->WriteFieldCExoLocString(pStruct, m_lsName, "LocName");
    pRes->WriteFieldINT(pStruct, m_nMarkDown, "MarkDown");
    pRes->WriteFieldINT(pStruct, m_nMarkUp, "MarkUp");

    CResRef cScript(m_sOnOpenStore[0]);
    pRes->WriteFieldCResRef(pStruct, cScript, "OnOpenStore");
    pRes->WriteFieldBYTE(pStruct, m_nBuySellFlag, "BuySellFlag");

    CResList cItemList;
    pRes->AddList(&cItemList, pStruct, "ItemList");
    CItemRepository* pRepository = m_pItemRepository;
    unsigned long nIndex = 0;
    if (nIndex < (unsigned long)pRepository->m_lstItems.num)
    {
        do
        {
            CSWSItem* pItem = pRepository->ItemListGetItem(nIndex);
            CResStruct cItemStruct;
            pRes->AddListElement(&cItemStruct, &cItemList, nIndex);
            pRes->WriteFieldDWORD(&cItemStruct, pItem->m_nId, "ObjectId");
            pRes->WriteFieldBYTE(&cItemStruct, pItem->m_bInfinite != 0,
                                 "Infinite");
            pItem->SaveItem(pRes, &cItemStruct);
            pRepository = m_pItemRepository;
            ++nIndex;
        } while (nIndex < (unsigned long)pRepository->m_lstItems.num);
    }

    pRes->WriteFieldFLOAT(pStruct, m_vPosition.m_fX, "XPosition");
    pRes->WriteFieldFLOAT(pStruct, m_vPosition.m_fY, "YPosition");
    pRes->WriteFieldFLOAT(pStruct, m_vPosition.m_fZ, "ZPosition");
    pRes->WriteFieldFLOAT(pStruct, m_vOrientation.m_fX, "XOrientation");
    pRes->WriteFieldFLOAT(pStruct, m_vOrientation.m_fY, "YOrientation");
    pRes->WriteFieldFLOAT(pStruct, m_vOrientation.m_fZ, "ZOrientation");
    return 1;
}
// PC 0x005C6E80  Xbox 0x00214180
int CSWSStore::AddToArea(CSWSArea* pArea, float fX, float fY, float fZ,
                         int bLoadingGame)
{
    NwsstoreTrace1();
    int result = (int)pArea;

    if (pArea != NULL)
    {
        SetArea(pArea);
        Vector vPosition(fX, fY, fZ);
        SetPosition(vPosition, 0, 1, 0);
        result = GetArea()->AddObjectToArea(m_nId, bLoadingGame);
    }

    return result;
}
// PC 0x005C6EE0  Xbox 0x002141F0
void CSWSStore::EventHandler(unsigned long nEventId, unsigned long oidCaller,
                             void* pEventData, unsigned long nCalendarDay,
                             unsigned long nTimeOfDay)
{
    if (nEventId == CServerAIMaster::EVENT_SIGNAL_EVENT)
    {
        CScriptEvent* pEvent = (CScriptEvent*)pEventData;
        if (pEvent->m_nType == CSWSSCRIPTEVENT_EVENTTYPE_ON_OPEN)
        {
            m_oidEventCaller = oidCaller;
            g_pVirtualMachine->RunScript(&m_sOnOpenStore[0], m_nId, 1);
        }
        delete pEvent;
    }
    else if (nEventId == CServerAIMaster::EVENT_TIMED_EVENT)
    {
        if (pEventData != NULL)
        {
            g_pVirtualMachine->RunScriptSituation(pEventData, m_nId, 1);
        }
    }
    else if (nEventId == CServerAIMaster::EVENT_DESTROY_OBJECT)
    {
        delete this;
    }
}
// PC 0x005C6F70  Xbox 0x00214280
int CSWSStore::SellItem(CSWSItem* pItem, CSWSCreature* pCreature)
{
    NwsstoreTrace1();
    if (pItem->m_bInfinite)
    {
        CSWSItem* pNewItem = new CSWSItem(OBJECT_INVALID);
        pNewItem->CopyItem(pItem);
        pNewItem->m_bInfinite = 0;
        pNewItem->m_nStackSize = 1;
        unsigned long oidCaller = m_nId;
        return pCreature->AcquireItem(&pNewItem, oidCaller, OBJECT_INVALID, 1);
    }
    else if (pItem->m_nStackSize > 1)
    {
        CSWSItem* pSplitItem = pItem->SplitItem(1);
        unsigned long oidCaller = m_nId;
        return pCreature->AcquireItem(&pSplitItem, oidCaller, OBJECT_INVALID, 1);
    }
    else if (m_pItemRepository->RemoveItem(pItem))
    {
        unsigned long oidCaller = m_nId;
        return pCreature->AcquireItem(&pItem, oidCaller, OBJECT_INVALID, 1);
    }
    return 0;
}
// PC 0x005C7180  Xbox 0x002144F0
int CSWSStore::LoadStore(CResGFF* pRes, CResStruct* pStruct, int bFromTemplate)
{
    CResStruct cItemStruct;
    int bSuccess;
    SetTag(pRes->ReadFieldCExoString(pStruct, "Tag", bSuccess, ""));
    m_lsName = pRes->ReadFieldCExoLocString(pStruct, "LocName", bSuccess,
                                          CExoLocString());
    m_nMarkDown = pRes->ReadFieldINT(pStruct, "MarkDown", bSuccess, 0);
    m_nMarkUp = pRes->ReadFieldINT(pStruct, "MarkUp", bSuccess, 0);

    CExoString sScript;
    pRes->ReadFieldCResRef(pStruct, "OnOpenStore", bSuccess,
                          "").CopyToString(sScript);
    m_sOnOpenStore[0] = sScript;
    unsigned char nBuySellFlag = m_nBuySellFlag;
    m_nBuySellFlag = pRes->ReadFieldBYTE(pStruct, "BuySellFlag", bSuccess,
                                        nBuySellFlag);

    CExoArrayList<CSWSItem*> lstItems;
    CResList cItemList;
    CSWSItem* pItem;
    int bHasList = pRes->GetList(&cItemList, pStruct, "ItemList");
    if (bHasList)
    {
        for (unsigned long nIndex = 0; nIndex < pRes->GetListCount(&cItemList);
             nIndex++)
        {
            pItem = NULL;
            pRes->GetListElement(&cItemStruct, &cItemList, nIndex);
            unsigned long oidItem = pRes->ReadFieldDWORD(
                &cItemStruct, "ObjectId", bSuccess, OBJECT_INVALID);
            if (g_pAppManager->GetServerExoApp()->GetGameObject(oidItem) == NULL)
            {
                pItem = new CSWSItem(oidItem);
                if (pItem != NULL)
                {
                    if (bFromTemplate)
                    {
                        CResRef cInventoryRes = pRes->ReadFieldCResRef(
                            &cItemStruct, "InventoryRes", bSuccess, "");
                        if (!bSuccess || !pItem->LoadFromTemplate(cInventoryRes))
                        {
                            delete pItem;
                            pItem = NULL;
                        }
                    }
                    else if (!pItem->LoadItem(pRes, &cItemStruct))
                    {
                        delete pItem;
                        pItem = NULL;
                    }
                    if (pItem != NULL)
                    {
                        int bInfinite = pRes->ReadFieldBYTE(
                            &cItemStruct, "Infinite", bSuccess, 0);
                        pItem->m_bInfinite = bInfinite;
                        unsigned long nCost = pItem->GetCost();
                        int nInsert = -1;
                        for (int i = 0; i < lstItems.num && nInsert == -1; i++)
                        {
                            if (lstItems[i]->GetCost() < nCost)
                                nInsert = i;
                        }
                        if (nInsert != -1)
                            lstItems.Insert(pItem, nInsert);
                        else
                            lstItems.Add(pItem);
                    }
                }
            }
        }
    }
    for (int i = lstItems.num - 1; i >= 0; i--)
    {
        if (AddItemToInventory(&lstItems[i]) && pItem != NULL)
        {
            unsigned long oidStore = m_nId;
            lstItems[i]->SetPossessor(oidStore, 1, 1, OBJECT_INVALID);
        }
    }
    unsigned long oidStore = m_nId;
    g_pAppManager->GetServerExoApp()->GetModule()->AddObjectToLookupTable(
        m_sTag, oidStore);
    return 1;
}
// PC 0x005C7610  Xbox 0x00214980
int CSWSStore::AcquireItem(CSWSItem* pItem)
{
    NWSSTORE_TRACE();
    if (pItem == NULL || pItem->m_bPlotObject != 0)
        return 0;

    pItem->SetIdentified(1);
    unsigned long oidPossessor = pItem->m_oidPossessor;
    CGameObject* pPossessor = g_pAppManager->m_pServerExoApp->GetGameObject(
        oidPossessor);
    if (pPossessor != NULL)
    {
        if (pPossessor->AsSWSCreature() != NULL)
        {
            pPossessor->AsSWSCreature()->RemoveItem(pItem, 1, 1, NULL);
        }
        else if (pPossessor->AsSWSItem() != NULL)
        {
            CSWSCreature* pCreature =
                g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(
                    pPossessor->AsSWSItem()->m_oidPossessor);
            if (pCreature != NULL)
                pCreature->RemoveItem(pItem, 1, 1, NULL);
        }
    }

    CSWSItem* pInventoryItem =
        g_pAppManager->m_pServerExoApp->GetItemByGameObjectID(
            m_pItemRepository->FindItemWithTag(pItem->m_sTag));
    if (pInventoryItem != NULL && pInventoryItem->CompareItem(pItem))
    {
        if (pInventoryItem->m_bInfinite)
        {
            delete pItem;
            return 1;
        }
        else
        {
            ++pInventoryItem->m_nStackSize;
            delete pItem;
            return 1;
        }
    }
    else
    {
        pItem->SetPossessor(m_nId, 1, 1, OBJECT_INVALID);
        return AddItemToInventory(&pItem);
    }
}
// PC 0x005C70C0  Xbox 0x002143D0
int CSWSStore::AddItemToInventory(CSWSItem** ppItem)
{
    NwsstoreTrace1();
    unsigned long nCost = (*ppItem)->GetCost();
    CSWSItem* pItem = m_pItemRepository->ItemListGetItem(0);

    if (pItem == NULL)
    {
        m_pItemRepository->AddItem(ppItem, 1, 0, 1);
        return 1;
    }

    if (pItem->GetCost() <= nCost)
    {
        m_pItemRepository->AddItem(ppItem, 1, 0, 1);
        return 1;
    }
    else
    {
        int nIndex = m_pItemRepository->m_lstItems.num;
        while (1)
        {
            pItem = m_pItemRepository->ItemListGetItem(--nIndex);
            if (pItem != NULL)
            {
                if (pItem->m_nId == (*ppItem)->m_nId)
                    break;
                if (pItem->GetCost() > nCost)
                {
                    m_pItemRepository->InsertItem(*ppItem, nIndex);
                    return 1;
                }
            }
        }
    }
    return 0;
}
// PC 0x005C7760  Xbox 0x00214AD0
int CSWSStore::LoadFromTemplate(const CResRef& cResRef, int bFromTemplate)
{
    CResStruct stTop;
    CResGFF* pRes = new CResGFF(RESTYPE_UTM, "UTM ", cResRef);

    if (pRes->m_bLoaded == 0)
    {
        delete pRes;
        CExoString sMessage;
        CExoString sResRef;
        cResRef.CopyToString(sResRef);
        sMessage.Format("Store template %s doesn't exist.\n", sResRef.CStr());
        return 0;
    }
    else
    {
        pRes->GetTopLevelStruct(&stTop);
        LoadStore(pRes, &stTop, bFromTemplate);
        delete pRes;
        return 1;
    }
}

static inline void NwsstoreTrace2()
{
    NwsstoreTrace1();
}

static inline void NwsstoreTrace1()
{
}
