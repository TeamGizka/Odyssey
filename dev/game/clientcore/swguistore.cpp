#include "exobase.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "serverexoapp.h"
#include "swguistore.h"

#include "guiingame.h"
#include "nwitem.h"
#include "nwtlktable.h"
#include "nwsitem.h"
#include "nwsobject.h"
#include "nwsstore.h"
#include "swguicustompanel.h"
#include "swguiingameequip.h"

static inline void SwguistoreTrace2();
static inline void SwguistoreTrace1();
#define SWGUISTORE_TRACE() SwguistoreTrace1()
// PC 0x00720E90
void CSWGuiStore::PopulateStore()
{
    CServerExoApp* pServer;
    CSWSCreature* pCreature;
    int nGold;

    pServer = g_pAppManager->m_pServerExoApp;
    pCreature = pServer->GetCreatureByGameObjectID(m_nCreatureId);
    if (pCreature != 0)
    {
        nGold = pCreature->GetGold();
        m_nGold = nGold;
        m_lblCreditsValue.m_cText.m_cParams.SetText(CExoString(nGold));
    }
}
// PC 0x00721300
void CSWGuiStore::SetDescription(CExoString const& sDescription)
{
    CSWGuiListBox* pListBox;
    int nHeight;
    CSWGuiControl* pLabel;

    m_lblDescription.m_cText.m_cParams.SetText(sDescription);
    pListBox = &m_CSWGuiListBox_1A40;
    pListBox->ClearItems();
    CSWGuiExtent cExtent = m_lblDescription.m_cExtent;
    if (sDescription.GetLength() > 0)
    {
        nHeight = m_lblDescription.m_cText.GetIdealHeight();
    }
    else
    {
        nHeight = m_lblDescription.m_cText.GetFontHeight();
    }
    cExtent.m_nHeight = nHeight;
    m_lblDescription.SetExtent(cExtent);
    pLabel = &m_lblDescription;
    pListBox->AddControls(&pLabel, 1, 0, 0, 0);
    pListBox->SetSelectedControl(&m_lblDescription, 0);
    m_lblUnknownF80.m_bEnabled =
        pListBox->m_nContentHeight + pListBox->m_nPadding >
            pListBox->m_cItemExtent.m_nHeight &&
        pListBox->m_lItems.num > 0 && !pListBox->m_bHideScrollIcon;
}
// PC 0x00721B80
void CSWGuiStore::OnControlEntered(CSWGuiControl* pControl)
{
    CServerExoApp* pServer;
    unsigned long oidItem;
    CSWSItem* pItem;

    if (pControl->m_bSelected != 0)
    {
        oidItem = static_cast<CSWGuiStoreItemEntry*>(pControl)->m_oidItem;
        pServer = g_pAppManager->m_pServerExoApp;
        pItem = pServer->GetItemByGameObjectID(pServer->ClientToServerObjectId(oidItem));
        if (pItem != 0)
        {
            if (m_lbxStoreItems.m_bEnabled)
                m_lblItemValue.m_cText.m_cParams.SetText(CExoString(GetItemBuyValue(pItem)));
            else
                m_lblItemValue.m_cText.m_cParams.SetText(CExoString(GetItemSellValue(pItem)));
            if (pItem->m_bInfinite)
            {
                m_lblUnknownD00.m_cText.m_cParams.SetStrRef(STRREF_INFINITE);
            }
            else
            {
                int nStackSize = pItem->m_nStackSize;
                m_lblUnknownD00.m_cText.m_cParams.SetText(CExoString(nStackSize));
            }
            SetDescription(pItem->GetPropertyDescription());
        }
    }
}
// PC 0x00721250
void CSWGuiStore::SellItemCallback(CSWGuiControl* pControl)
{
    CGuiInGame* pInGame = g_pAppManager->m_pClientExoApp->GetInGameGui();

    if (pInGame->m_pMessageBox->m_bAccepted)
    {
        unsigned long oidItem =
            static_cast<CSWGuiStoreItemEntry*>(pControl)->m_oidItem;
        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
        CSWSItem* pItem = pServer->GetItemByGameObjectID(
            pServer->ClientToServerObjectId(oidItem));
        if (pItem != 0)
            SellItem(pItem);
    }
}
// PC 0x00721670
void CSWGuiStore::OnControlInvAButton(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        if (pControl->m_nEvent == CSWGuiControl::EVENT_CONFIRM)
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
#ifndef _XBOX
        if (pControl == &m_btnAccept)
        {
            pControl = m_lbxInventoryItems.GetSelectedControl();
            if (pControl == 0)
                return;
        }
#endif
        unsigned long oidItem =
            static_cast<CSWGuiStoreItemEntry*>(pControl)->m_oidItem;
        CServerExoApp* pServer = g_pAppManager->GetServerExoApp();
        CSWSItem* pItem = pServer->GetItemByGameObjectID(
            pServer->ClientToServerObjectId(oidItem));
        if (pItem != 0)
        {
            CServerExoApp* pCreatureServer = g_pAppManager->GetServerExoApp();
            CSWSCreature* pCreature = pCreatureServer->GetPlayerCreature();
            unsigned int nConfirmationValue;
            if (pCreature != 0 &&
                pCreature->m_pStats->GetLevel(0) * SELL_CONFIRM_VALUE_PER_LEVEL <=
                    SELL_CONFIRM_VALUE_MAX)
            {
                CSWSCreatureStats* pStats = pCreature->GetStats();
                nConfirmationValue =
                    pStats->GetLevel(0) * SELL_CONFIRM_VALUE_PER_LEVEL;
            }
            else
                nConfirmationValue = SELL_CONFIRM_VALUE_MAX;
            if ((unsigned int)GetItemSellValue(pItem) > nConfirmationValue)
            {
                CSWGuiMessageBox* pMessageBox =
                    g_pAppManager->GetClientExoApp()->GetInGameGui()->m_pMessageBox;
                CExoString sMessage;
                g_pTlkTable->SetCustomToken(SELL_CONFIRM_VALUE_TOKEN,
                                            CExoString(nConfirmationValue));
                sMessage = g_pSWTlkTable->Fetch(STRREF_CONFIRM_SELL, 0, 0, 0);
                pMessageBox->SetAllowCancel(1);
                pMessageBox->SetMessage(sMessage);
                pMessageBox->SetCallback(this,
                    (CSWGuiHandler)&CSWGuiStore::SellItemCallback, pControl);
                g_pAppManager->GetClientExoApp()->GetSWGuiManager()->AddPanel(
                    pMessageBox, 1, 1);
            }
            else
                SellItem(pItem);
        }
    }
}
// PC 0x00722090
void CSWGuiStore::OnControlStoreAButton(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        if (pControl->m_nEvent != 0)
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
#ifndef _XBOX
        if (pControl == &m_btnAccept)
        {
            pControl = m_lbxStoreItems.GetSelectedControl();
            if (pControl == 0)
                return;
        }
#endif
        unsigned long oidItem =
            static_cast<CSWGuiStoreItemEntry*>(pControl)->m_oidItem;
        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
        CSWSItem* pItem = pServer->GetItemByGameObjectID(
            pServer->ClientToServerObjectId(oidItem));
        if (pItem != 0)
        {
            if ((unsigned int)m_nGold >= (unsigned int)GetItemBuyValue(pItem))
            {
                CServerExoApp* pCreatureServer = g_pAppManager->m_pServerExoApp;
                CSWSCreature* pCreature = pCreatureServer->GetPlayerCreature();
                unsigned int nConfirmationValue;
                if (pCreature != 0)
                {
                    CSWSCreatureStats* pStats = pCreature->m_pStats;
                    nConfirmationValue =
                        pStats->GetLevel(0) * BUY_CONFIRM_VALUE_PER_LEVEL;
                }
                else
                    nConfirmationValue = BUY_CONFIRM_VALUE_DEFAULT;
                if ((unsigned int)GetItemBuyValue(pItem) > nConfirmationValue)
                {
                    CSWGuiMessageBox* pMessageBox =
                        g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
                    CExoString sMessage;
                    g_pTlkTable->SetCustomToken(BUY_CONFIRM_VALUE_TOKEN,
                                                CExoString(nConfirmationValue));
                    sMessage = g_pSWTlkTable->Fetch(STRREF_CONFIRM_BUY, 0, 0, 0);
                    pMessageBox->SetAllowCancel(1);
                    pMessageBox->SetMessage(sMessage);
                    pMessageBox->SetCallback(this,
                        (CSWGuiHandler)&CSWGuiStore::BuyItemCallback, pControl);
                    g_pAppManager->m_pClientExoApp->GetSWGuiManager()->AddPanel(
                        pMessageBox, 1, 1);
                }
                else
                    BuyItem(pItem);
            }
            else
                g_pAppManager->m_pClientExoApp->GetInGameGui()->ShowExamineBox(
                    STRREF_INSUFFICIENT_CREDITS, 1);
        }
    }
}
// PC 0x00721F10
void CSWGuiStore::BuyItem(CSWSItem* pItem)
{
    SWGUISTORE_TRACE();
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    CSWSCreature* pCreature =
        pServer->GetPlayerCreature();
    CServerExoApp* pStoreServer = g_pAppManager->m_pServerExoApp;
    CSWSStore* pStore = pStoreServer->GetStoreByGameObjectID(m_nStoreId);

    if (pCreature != 0 && pStore != 0 &&
        pCreature->GetItemRepository(1) != 0 && pItem != 0)
    {
        m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_BUY_ITEM);
        m_nGold -= GetItemBuyValue(pItem);
        pCreature->SetGold(m_nGold);
        pStore->SellItem(pItem, pCreature);
        int nSelectedItem = m_lbxStoreItems.m_nSelectedItem;
#ifndef _XBOX
        int nTopVisibleItem = m_lbxStoreItems.m_nTopVisibleItem;
#endif
        PopulateStoreItemListBox();
        if (nSelectedItem >= m_lbxStoreItems.m_lItems.num)
            nSelectedItem = m_lbxStoreItems.m_lItems.num - 1;
#ifndef _XBOX
        m_lbxStoreItems.SetTopVisible(nTopVisibleItem);
#endif
        m_lbxStoreItems.SetSelectedControl(nSelectedItem, 0);
        m_lblCreditsValue.m_cText.m_cParams.SetText(CExoString(m_nGold));
    }
}
// PC 0x00720F10
int CSWGuiStore::GetItemBuyValue(CSWSItem* pItem)
{
    CSWSStore* pStore;
    CServerExoApp* pServer;
    int nMarkup;

    pServer = g_pAppManager->m_pServerExoApp;
    pStore = pServer->GetStoreByGameObjectID(m_nStoreId);
    if (pStore == 0 || pItem == 0)
        return 0;
    nMarkup = pStore->m_nMarkUp + pStore->m_nBonusMarkUp;
    return nMarkup * pItem->GetCost() / 100u;
}
// PC 0x00720F70
int CSWGuiStore::GetItemSellValue(CSWSItem* pItem)
{
    CSWSStore* pStore;
    CServerExoApp* pServer;
    int nMarkup;

    pServer = g_pAppManager->m_pServerExoApp;
    pStore = pServer->GetStoreByGameObjectID(m_nStoreId);
    if (pStore == 0 || pItem == 0)
        return 0;
    nMarkup = pStore->m_nMarkDown + pStore->m_nBonusMarkDown;
    return nMarkup * pItem->GetCost() / 100u;
}
// PC 0x00722030
void CSWGuiStore::BuyItemCallback(CSWGuiControl* pControl)
{
    CGuiInGame* pInGame = g_pAppManager->m_pClientExoApp->GetInGameGui();

    if (pInGame->m_pMessageBox->m_bAccepted)
    {
        unsigned long oidItem =
            static_cast<CSWGuiStoreItemEntry*>(pControl)->m_oidItem;
        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
        CSWSItem* pItem = pServer->GetItemByGameObjectID(
            pServer->ClientToServerObjectId(oidItem));
        if (pItem != 0)
            BuyItem(pItem);
    }
}
// PC 0x007210E0
void CSWGuiStore::SellItem(CSWSItem* pItem)
{
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    CSWSCreature* pCreature =
        pServer->GetPlayerCreature();
    CServerExoApp* pStoreServer = g_pAppManager->m_pServerExoApp;
    CSWSStore* pStore = pStoreServer->GetStoreByGameObjectID(m_nStoreId);

    if (pItem != 0 && pCreature != 0 && pStore != 0)
    {
        CItemRepository* pInventory = pCreature->GetItemRepository(1);
        if (pInventory != 0)
        {
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELL_ITEM);
            m_nGold += GetItemSellValue(pItem);
            pCreature->SetGold(m_nGold);
            if (pItem->m_nStackSize > 1)
                pStore->AcquireItem(pItem->SplitItem(1));
            else
            {
                pInventory->RemoveItem(pItem);
                pStore->AcquireItem(pItem);
            }
            int nSelectedItem = m_lbxInventoryItems.m_nSelectedItem;
#ifndef _XBOX
            int nTopVisibleItem = m_lbxInventoryItems.m_nTopVisibleItem;
#endif
            PopulateInventoryItemListBox();
            if (nSelectedItem >= m_lbxInventoryItems.m_lItems.num)
                nSelectedItem = m_lbxInventoryItems.m_lItems.num - 1;
#ifndef _XBOX
            m_lbxInventoryItems.SetTopVisible(nTopVisibleItem);
#endif
            m_lbxInventoryItems.SetSelectedControl(nSelectedItem, 0);
            m_lblCreditsValue.m_cText.m_cParams.SetText(CExoString(m_nGold));
        }
    }
}
// PC 0x007228E0
void CSWGuiStore::PopulateInventoryItemListBox()
{
    CServerExoApp* pServer;
    CSWSCreature* pCreature;
    CItemRepository* pRepository;
    int nEntryIndex;
    int nItemClass;
    int bCreateItem;
    unsigned long nItemCount;
    unsigned long nEntries;
    int nNextItemClass;
    unsigned long i;
    unsigned long j;

    pServer = g_pAppManager->GetServerExoApp();
    pCreature = pServer->GetCreatureByGameObjectID(m_nCreatureId);
    if (pCreature != 0)
        pRepository = pCreature->GetItemRepository(1);

    CExoArrayList<CSWGuiControl*> lControls;
    nEntryIndex = 0;
    bCreateItem = 0;
    nItemCount = 0;
    for (i = 0; i < pRepository->GetNumItems(); i++)
    {
        CSWSItem* pItem = pRepository->ItemListGetItem(i);
        if (pItem != 0 && pItem->GetPlotObject() == 0)
            nItemCount++;
    }

    nEntries = 0;
    nItemClass = 0;
    nNextItemClass = 0;
    while (nEntries < nItemCount)
    {
        for (j = 0; j < pRepository->GetNumItems(); j++)
        {
            CSWSItem* pItem;
            int nCurrentItemClass;

            if (bCreateItem != 0)
                break;
            pItem = pRepository->ItemListGetItem(j);
            if (pItem != 0 && pItem->GetPlotObject() == 0)
            {
                nCurrentItemClass =
                    pItem->GetBaseItem()->GetStorePanelSort();
                if (nCurrentItemClass == nItemClass)
                {
                    bCreateItem = CreateItemEntry(nEntryIndex, lControls,
                                                  pItem->GetId(), 1);
                    nEntries++;
                }
                else if (nCurrentItemClass > nItemClass &&
                         (nNextItemClass == nItemClass ||
                          nCurrentItemClass < nNextItemClass))
                {
                    nNextItemClass = nCurrentItemClass;
                }
            }
        }
        nItemClass = nNextItemClass;
    }
    m_lbxInventoryItems.AddControls(lControls, 0, 1, 0);
    if (m_lbxInventoryItems.m_bEnabled != 0)
    {
        if (lControls.num > 0)
        {
            m_lbxInventoryItems.SetSelectedControl(0, 0);
        }
        else
        {
            m_lbxInventoryItems.SetSelectedControl(-1, 0);
            m_lblItemValue.m_cText.m_cParams.SetText("");
            m_lblUnknownD00.m_cText.m_cParams.SetText("");
            m_lblDescription.m_cText.m_cParams.SetText("");
        }
    }
    m_nUnknown2274 = 0;
}
// PC 0x00722370
void CSWGuiStore::DeleteItemEntries()
{
    SwguistoreTrace2();
    CExoArrayList<CSWGuiStoreItemEntry*>& lStoreEntries = m_lStoreEntries;
    CExoArrayList<CSWGuiStoreItemEntry*>& lInventoryEntries = m_lInventoryEntries;
    m_lbxInventoryItems.ClearItems();
    m_lbxStoreItems.ClearItems();
    for (int i = 0; i < lStoreEntries.num; i++)
    {
        delete lStoreEntries[i];
        lStoreEntries[i] = 0;
    }
    lStoreEntries.num = 0;
    for (int i = 0; i < lInventoryEntries.num; i++)
    {
        delete lInventoryEntries[i];
        lInventoryEntries[i] = 0;
    }
    lInventoryEntries.num = 0;
}
// PC 0x00722C40
void CSWGuiStore::ShowSellGUI()
{
    CSWGuiListBox* pActiveList;

    pActiveList = &m_lbxInventoryItems;
    pActiveList->m_bEnabled = 1;
    m_lbxStoreItems.m_bEnabled = 0;
    m_lblUnknown1C0.m_cText.m_cParams.SetStrRef(125553);
#ifndef _XBOX
    m_btnAccept.m_cText.m_cParams.SetStrRef(125553);
#endif
#ifndef _XBOX
    m_btnAccept.AddEvent(
        39, this, (CSWGuiHandler)&CSWGuiStore::OnControlInvAButton);
#endif
    m_lblBuySell.m_cText.m_cParams.SetStrRef(32130);
    m_lblUnknown6C0.m_cText.m_cParams.SetStrRef(41937);
#ifndef _XBOX
    m_btnExamine.m_cText.m_cParams.SetStrRef(41937);
#endif
    m_lblCost.m_cText.m_cParams.SetStrRef(41945);
    SetActiveControl(pActiveList, 0);
    PopulateInventoryItemListBox();
}
// PC 0x00722EA0
CSWGuiStore::CSWGuiStore(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    m_nUnknown226C = 0;

    StartLoadFromLayout("store");
    InitControl(m_lblBuySell, "LBL_BUYSELL", 1);
    InitControl(m_lbxStoreItems, "LB_SHOPITEMS", 1);
    InitControl(m_lbxInventoryItems, "LB_INVITEMS", 1);
    InitControl(m_lblCredits, "LBL_CREDITS", 1);
    InitControl(m_lblCreditsValue, "LBL_CREDITS_VALUE", 1);
#ifdef _XBOX
    InitControl(m_lblUnknown80, "LBL_A", 1);
    InitControl(m_lblUnknown1C0, "LBL_Accept", 1);
    InitControl(m_lblUnknown300, "LBL_B", 1);
    InitControl(m_lblUnknown440, "LBL_Cancel", 1);
    InitControl(m_lblUnknown800, "LBL_X", 1);
    InitControl(m_lblUnknown6C0, "LBL_Examine", 1);
#endif
    InitControl(m_CSWGuiListBox_1A40, "LB_DESCRIPTION", 1);
    m_CSWGuiListBox_1A40.m_nSelectSound = CSWGuiManager::GUISOUND_SELECTION;
    InitControl(m_lblCost, "LBL_COST", 1);
    InitControl(m_lblItemValue, "LBL_COST_VALUE", 1);
    InitControl(m_lblStock, "LBL_STOCK", 1);
    InitControl(m_lblUnknownD00, "LBL_STOCK_VALUE", 1);
#ifdef _XBOX
    InitControl(m_lblUnknownF80, "LBL_RANALOG", 1);
#else
    InitControl(m_btnCancel, "BTN_Cancel", 1);
    InitControl(m_btnExamine, "BTN_Examine", 1);
    InitControl(m_btnAccept, "BTN_Accept", 1);
#endif
    StopLoadFromLayout();

    CSWGuiLabel* pProtoLabel = (CSWGuiLabel*)m_CSWGuiListBox_1A40.m_pProtoItem;
    m_lblDescription.Initialize(pProtoLabel->m_cExtent, pProtoLabel, 1.0f);

    CSWGuiListBox* pStoreItems = &m_lbxStoreItems;
    pStoreItems->m_bShowScrollBar = 1;
    CSWGuiListBox* pInventoryItems = &m_lbxInventoryItems;
    pInventoryItems->m_bShowScrollBar = 1;
    SetActiveControl(pStoreItems, 0);
    pInventoryItems->m_bEnabled = 0;

    m_nUnknown64 = 0;
    m_nUnknown2274 = 0;
    m_nGold = 0;

#ifndef _XBOX
    CSWGuiButton* pCancel = &m_btnCancel;
    pCancel->m_bFocusable = 0;
    pCancel->AddEvent(CSWGuiControl::EVENT_SELECT, this,
                      (CSWGuiHandler)&CSWGuiPanel::OnBButtonPressed);
    CSWGuiButton* pExamine = &m_btnExamine;
    pExamine->m_bFocusable = 0;
    pExamine->AddEvent(CSWGuiControl::EVENT_SELECT, this,
                       (CSWGuiHandler)&CSWGuiStore::OnControlXButton);
    m_btnAccept.m_bFocusable = 0;

    pInventoryItems->SetSelectOnLeftClick(1);
    pStoreItems->SetSelectOnLeftClick(1);
#endif
#ifndef _XBOX
    m_nBackground = 1;
#endif
}
// PC 0x00723780
void CSWGuiStore::OnPanelAdded()
{
    InitializeStoreType();
    PopulateStore();
#ifndef _XBOX
    CSWGuiPanel::OnPanelAdded();
#endif
    ShowBuyGUI();
}
// PC 0x007237C0
void CSWGuiStore::HandleInputEvent(int nEvent, int nState)
{
    CSWGuiPanel::HandleInputEvent(nEvent, nState);

    if (nState != 0)
    {
        switch (nEvent)
        {
        case 40:
        case 46:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            g_pAppManager->m_pClientExoApp->GetInGameGui()->HideStoreGui();
            break;
        case 58:
            m_CSWGuiListBox_1A40.HandleInputEvent(GUIEVENT_DOWN, 1);
            break;
        case 57:
            m_CSWGuiListBox_1A40.HandleInputEvent(GUIEVENT_UP, 1);
            break;
        case 41:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
#ifdef _XBOX
            OnControlXButton();
#else
            OnControlXButton(0);
#endif
            break;
        }
    }
}
// PC 0x007224B0
CSWGuiStore::~CSWGuiStore()
{
    DeleteItemEntries();
}
// PC 0x00722700
void CSWGuiStore::OnPanelRemoved()
{
    DeleteItemEntries();
#ifndef _XBOX
    CSWGuiPanel::OnPanelRemoved();
#endif
}
// PC 0x00721410
void CSWGuiStore::PopulateStoreItemListBox()
{
    CItemRepository* pRepository;

    pRepository =
        g_pAppManager->GetServerExoApp()->GetStoreByGameObjectID(m_nStoreId)
            ->m_pItemRepository;
    CExoArrayList<CSWGuiControl*> lControls;
    {
        int nEntryIndex;
        int nItemClass;
        int bCreateItem;
        unsigned long nEntries;
        int nNextItemClass;
        unsigned long i;
        nItemClass = 0;
        nEntryIndex = 0;
        bCreateItem = 0;
        nEntries = 0;
        nNextItemClass = 0;

        while (nEntries < pRepository->GetNumItems())
        {
            for (i = 0; i < pRepository->GetNumItems(); i++)
            {
                CSWSItem* pItem;
                int nCurrentItemClass;

                if (bCreateItem != 0)
                    break;
                pItem = pRepository->ItemListGetItem(i);
                if (pItem != 0 && pItem->m_bPlotObject == 0)
                {
                    nCurrentItemClass = pItem->GetBaseItem()->GetStorePanelSort();
                    if (nCurrentItemClass == nItemClass)
                    {
                        bCreateItem =
                            CreateItemEntry(nEntryIndex, lControls, pItem->GetId(), 0);
                        nEntries++;
                    }
                    else if (nCurrentItemClass > nItemClass &&
                             (nNextItemClass == nItemClass ||
                              nCurrentItemClass < nNextItemClass))
                    {
                        nNextItemClass = nCurrentItemClass;
                    }
                }
            }
            nItemClass = nNextItemClass;
        }

    }

    m_lbxStoreItems.AddControls(lControls, 0, 1, 0);
    if (m_lbxStoreItems.m_bEnabled != 0)
    {
        if (lControls.num > 0)
        {
            m_lbxStoreItems.SetSelectedControl(0, 0);
        }
        else
        {
            m_lbxStoreItems.SetSelectedControl(-1, 0);
            m_lblItemValue.m_cText.m_cParams.SetText("");
            m_lblUnknownD00.m_cText.m_cParams.SetText("");
            m_lblDescription.m_cText.m_cParams.SetText("");
        }
    }
    m_nUnknown2274 = 0;
}
// PC 0x00722710
int CSWGuiStore::CreateItemEntry(int& nEntries,
                                 CExoArrayList<CSWGuiControl*>& lControls,
                                 unsigned long oidItem, int bInventory)
{
    CExoArrayList<CSWGuiStoreItemEntry*>* pEntries =
        bInventory ? &m_lInventoryEntries : &m_lStoreEntries;
    CSWGuiStoreItemEntry* pEntry;
    if (nEntries < pEntries->num)
    {
        pEntry = (*pEntries)[nEntries];
    }
    else
    {
        CSWGuiStoreItemEntry* pNewEntry = new CSWGuiStoreItemEntry;
        pEntry = pNewEntry;
        pEntries->Add(pNewEntry);
        pNewEntry->m_nClickSound = CSWGuiControl::GUISOUND_NONE;
        if (bInventory)
        {
            CSWGuiButton* pProto = m_lbxInventoryItems.m_pProtoItem->AsButton();
            pNewEntry->Initialize(pProto->m_cText, pProto->m_cBorder,
                               pProto->m_cHilite,
                               m_lbxInventoryItems.m_cItemExtent.m_nWidth -
                                   2 * m_lbxInventoryItems.m_nPadding);
            pNewEntry->AddEvent(GUIEVENT_ACTIVATE, this,
                            (CSWGuiHandler)&CSWGuiStore::OnControlInvAButton);
            pNewEntry->AddEvent(GUIEVENT_ACCEPT, this,
                            (CSWGuiHandler)&CSWGuiStore::OnControlInvAButton);
        }
        else
        {
            CSWGuiButton* pProto = m_lbxStoreItems.m_pProtoItem->AsButton();
            pNewEntry->Initialize(pProto->m_cText, pProto->m_cBorder,
                               pProto->m_cHilite,
                               m_lbxStoreItems.m_cItemExtent.m_nWidth -
                                   2 * m_lbxStoreItems.m_nPadding);
            pNewEntry->AddEvent(GUIEVENT_ACTIVATE, this,
                            (CSWGuiHandler)&CSWGuiStore::OnControlStoreAButton);
            pNewEntry->AddEvent(GUIEVENT_ACCEPT, this,
                            (CSWGuiHandler)&CSWGuiStore::OnControlStoreAButton);
        }
        pNewEntry->AddEvent(GUIEVENT_MOUSE_ENTER, this,
                        (CSWGuiHandler)&CSWGuiStore::OnControlEntered);
    }
    pEntry->SetItem(oidItem, 0);
    pEntry->SetActive(0);
    lControls.Add(pEntry);
    nEntries++;
    return 0;
}
// PC 0x00722400
void CSWGuiStore::ShowBuyGUI()
{
    CSWGuiListBox* pActiveList;

#ifdef _XBOX
    m_lbxInventoryItems.m_bEnabled = 0;
    pActiveList = &m_lbxStoreItems;
    pActiveList->SetEnabledFlag(1);
#else
    pActiveList = &m_lbxStoreItems;
    m_lbxInventoryItems.m_bEnabled = 0;
    pActiveList->m_bEnabled = 1;
#endif
    m_lblUnknown1C0.m_cText.m_cParams.SetStrRef(125552);
#ifndef _XBOX
    m_btnAccept.m_cText.m_cParams.SetStrRef(125552);
#endif
#ifndef _XBOX
    m_btnAccept.AddEvent(
        39, this, (CSWGuiHandler)&CSWGuiStore::OnControlStoreAButton);
#endif
    m_lblBuySell.m_cText.m_cParams.SetStrRef(32132);
    m_lblUnknown6C0.m_cText.m_cParams.SetStrRef(41938);
#ifndef _XBOX
    m_btnExamine.m_cText.m_cParams.SetStrRef(41938);
#endif
    m_lblCost.m_cText.m_cParams.SetStrRef(41943);
    SetActiveControl(pActiveList, 0);
    PopulateStoreItemListBox();
}

#ifdef _XBOX
void CSWGuiStore::OnControlXButton()
{
    CServerExoApp* pServer;
    CSWSStore* pStore;

    pServer = g_pAppManager->m_pServerExoApp;
    pStore = pServer->GetStoreByGameObjectID(m_nStoreId);
    if (pStore != 0 && pStore->m_nBuySellFlag == CSWSStore::BuySellFlagBoth)
    {
        if (m_lbxStoreItems.m_bEnabled)
            ShowSellGUI();
        else
            ShowBuyGUI();
    }
}
#else
void CSWGuiStore::OnControlXButton(CSWGuiControl* pControl)  // PC 0x00722D80
{
    CServerExoApp* pServer;
    CSWSStore* pStore;

    if (pControl == 0 || pControl->m_bSelected != 0)
    {
        pServer = g_pAppManager->m_pServerExoApp;
        pStore = pServer->GetStoreByGameObjectID(m_nStoreId);
        if (pStore != 0 && pStore->m_nBuySellFlag == CSWSStore::BuySellFlagBoth)
        {
            if (m_lbxStoreItems.m_bEnabled)
                ShowSellGUI();
            else
                ShowBuyGUI();
        }
    }
}
#endif
// PC 0x00722DD0
void CSWGuiStore::InitializeStoreType()
{
    CServerExoApp* pServer;
    CSWSStore* pStore;

    pServer = g_pAppManager->m_pServerExoApp;
    pStore = pServer->GetStoreByGameObjectID(m_nStoreId);
    if (pStore != 0)
    {
        switch (pStore->m_nBuySellFlag)
        {
        case CSWSStore::BuySellFlagBuy:
            m_lblUnknown800.SetEnabledFlag(0);
            m_lblUnknown6C0.SetEnabledFlag(0);
#ifndef _XBOX
            m_btnExamine.SetEnabledFlag(0);
#endif
            ShowBuyGUI();
            return;
        case CSWSStore::BuySellFlagSell:
            m_lblUnknown800.SetEnabledFlag(0);
            m_lblUnknown6C0.SetEnabledFlag(0);
#ifndef _XBOX
            m_btnExamine.SetEnabledFlag(0);
#endif
            ShowSellGUI();
            return;
        case CSWSStore::BuySellFlagBoth:
            m_lblUnknown800.SetEnabledFlag(1);
            m_lblUnknown6C0.SetEnabledFlag(1);
#ifndef _XBOX
            m_btnExamine.SetEnabledFlag(1);
#endif
            ShowBuyGUI();
            return;
        }
    }
}

static inline void SwguistoreTrace2()
{
    SwguistoreTrace1();
}

static inline void SwguistoreTrace1()
{
}
