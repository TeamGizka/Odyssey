#include "exobase.h"
#include "aurcamera.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "serverexoapp.h"
#include "swguiupgrade.h"

#include "nwitem.h"
#include "nwsitem.h"
#include "nwsobject.h"
#include "nwsplayer.h"
#include "guiingame.h"
#include "gameobjectarray.h"
#include "twodimarray.h"
#include "partytables.h"
#include "nwrules.h"
#include "twodimarrays.h"
#include "exoresman.h"
#include "aurmodel.h"
#include "aurscene.h"
#include "nwscript.h"
#include "swmain.h"
#include "nwsstore.h"
#include "swguiingameequip.h"
#include "nwccreature.h"
#include "nwtlktable.h"
#include "nwskill.h"
#include "swparty.h"
// PC 0x007292B0
void CSWGuiUpgrade::OnUpgradeSelected(CSWGuiControl* pControl)
{
    if (!pControl->m_bSelected)
        return;
    if (pControl->GetEvent() == GUIEVENT_ACCEPT)
        m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
    CSWUpgradeItemEntry* pEntry;
    if (pControl == &m_btnOk)
        pEntry = (CSWUpgradeItemEntry*)m_lbItems.GetSelectedControl();
    else
        pEntry = (CSWUpgradeItemEntry*)pControl;
    if (pEntry == 0)
        return;
    if (pEntry->m_bDisabled)
        return;
    CServerExoApp* pServer = g_pAppManager->GetServerExoApp();
    CItemRepository* pRepository = pServer->GetPartyTable()->GetPartyItems();
    int nSlot = m_pItemListControl->m_nIndex;
    if (nSlot == UPGRADE_SLOT_COLOR_CRYSTAL && m_nItemListMode == ITEMLIST_LIGHTSABER)
    {
        int nUpgradeIndex = pEntry->m_nUpgradeIndex;
        if (nUpgradeIndex == -1)
            goto Exit;
        CExoString sTemplate;
        CExoString sColumn;
        switch (m_pSelectedItem->GetBaseItem()->m_nItemType)
        {
        case CSWBaseItem::ITEM_TYPE_POWERED_HAND_39:
            sColumn = "DoubleMdlVar";
            break;
        case CSWBaseItem::ITEM_TYPE_POWERED_HAND_40:
            sColumn = "ShortMdlVar";
            break;
        case CSWBaseItem::ITEM_TYPE_POWERED_HAND_41:
            sColumn = "LongMdlVar";
            break;
        }
        int nCrystalRow = pEntry->m_nUpgradeIndex;
        m_pCrystalTable->GetCExoStringEntry(nCrystalRow, sColumn, &sTemplate);
        CSWSItem* pItem = new CSWSItem(OBJECT_INVALID);
        {
            CResRef resTemplate(sTemplate);
            pItem->LoadFromTemplate(resTemplate);
        }
        pItem->SetPossessor(pServer->GetPlayerCreatureId(), 0, 0, OBJECT_INVALID);
        pItem->SetIdentified(1);
        int nUpgrades = m_pSelectedItem->m_nUpgrades;
        pItem->m_nUpgrades = nUpgrades;
        for (int i = 0; i < NUM_UPGRADE_SLOTS; i++)
        {
            if (i != UPGRADE_SLOT_COLOR_CRYSTAL)
            {
                pItem->AddUpgrade(m_apItemEntries[i], i);
                pItem->m_anUpgradeSlot[i] = m_pSelectedItem->m_anUpgradeSlot[i];
            }
        }
        unsigned long bStolen = m_pSelectedItem->m_bStolen;
        pItem->m_bStolen = bStolen;
        delete m_pSelectedItem;
        m_pSelectedItem = pItem;
        Update3DView();
    }
    else
    {
        int nUpgradeIndex = pEntry->m_nUpgradeIndex;
        if (nUpgradeIndex == -1 &&
            pEntry->m_oidItem != OBJECT_INVALID)
            goto Exit;
    }

    CSWSItem*& pOldItem = m_apItemEntries[nSlot];
    if (pOldItem != 0)
    {
        pOldItem->SetPossessor(pServer->GetPlayerCreatureId(),
                                            0, 0, OBJECT_INVALID);
        int nIndex = m_aItemsAdded.IndexOf(pOldItem);
        if (nIndex != -1)
        {
            m_aItemsAdded.DelIndex(nIndex);
            pRepository->AddItem(&pOldItem, 1, 0, 0);
        }
        else
        {
            pRepository->AddItem(&pOldItem, 1, 0, 0);
            m_aItemsTaken.Add(pOldItem);
        }
        if (nSlot != UPGRADE_SLOT_COLOR_CRYSTAL || m_nItemListMode != ITEMLIST_LIGHTSABER)
        {
            m_pSelectedItem->RemoveUpgrade(pOldItem, nSlot);
            m_pSelectedItem->m_anUpgradeSlot[nSlot] = -1;
            pOldItem->m_anUpgradeSlot[nSlot] = -1;
        }
        pOldItem = 0;
    }
    CSWSItem* pItem = pServer->GetItemByGameObjectID(pEntry->m_oidItem);
    if (pItem != 0)
    {
        int nIndex = m_aItemsTaken.IndexOf(pItem);
        if (nIndex != -1)
        {
            m_aItemsTaken.DelIndex(nIndex);
            if (pItem->m_nStackSize > 1)
                pItem = pItem->SplitItem(1);
            else
                pRepository->RemoveItem(pItem);
        }
        else
        {
            if (pItem->m_nStackSize > 1)
                pItem = pItem->SplitItem(1);
            else
                pRepository->RemoveItem(pItem);
            m_aItemsAdded.Add(pItem);
        }
        int nRows = m_pUpgradeTable->GetNumRows();
        int nUpgradeRow = 0;
        for (int nRow = 0; nRow < nRows; nRow++)
        {
            CExoString sTemplate;
            m_pUpgradeTable->GetCExoStringEntry(nRow, "Template", &sTemplate);
            if (_stricmp(pItem->m_sTag.CStr(), sTemplate.CStr()) == 0)
            {
                nUpgradeRow = nRow;
                break;
            }
        }
        m_apItemEntries[nSlot] = pItem;
        if (nSlot != UPGRADE_SLOT_COLOR_CRYSTAL || m_nItemListMode != ITEMLIST_LIGHTSABER)
        {
            m_anUpgradeProperties[nSlot] = pEntry->m_nUpgradeIndex;
            m_pSelectedItem->AddUpgrade(pItem, nSlot);
            m_pSelectedItem->m_anUpgradeSlot[nSlot] = nUpgradeRow;
        }
    }
Exit:
    CloseItems();
}
// PC 0x007297E0
CSWUpgradeItemEntry* CSWGuiUpgrade::CreateItemEntry(unsigned long oidItem, int nUpgradeIndex,
                                                    int bDisabled)
{
    int nWidth = m_lbItems.m_cItemExtent.m_nWidth -
                 2 * m_lbItems.m_nPadding;
    CSWGuiButton* pProto = (CSWGuiButton*)m_lbItems.m_pProtoItem;
    CSWUpgradeItemEntry* pEntry = new CSWUpgradeItemEntry;

    pEntry->Initialize(pProto->m_cText, pProto->m_cBorder, pProto->m_cHilite, nWidth, bDisabled);
    pEntry->AddEvent(CSWGuiControl::EVENT_ENTER, this, (CSWGuiHandler)&CSWGuiUpgrade::OnControlEntered);
    pEntry->m_bDisabled = bDisabled;
    if (!bDisabled)
    {
        pEntry->AddEvent(CSWGuiControl::EVENT_SELECT, this, (CSWGuiHandler)&CSWGuiUpgrade::OnUpgradeSelected);
        pEntry->AddEvent(CSWGuiControl::EVENT_CONFIRM, this, (CSWGuiHandler)&CSWGuiUpgrade::OnUpgradeSelected);
    }
    else
    {
        pEntry->m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_DARK_TAN;
        pEntry->m_cText.m_cParams.SetDefaultColor(CGuiInGame::COLOR_DARK_TAN);
        pEntry->AddEvent(CSWGuiControl::EVENT_SELECT, this, (CSWGuiHandler)&CSWGuiUpgrade::OnDisabledSelected);
        pEntry->AddEvent(CSWGuiControl::EVENT_CONFIRM, this, (CSWGuiHandler)&CSWGuiUpgrade::OnDisabledSelected);
    }
    pEntry->SetItem(oidItem, 0, OBJECT_INVALID, 0);
    pEntry->m_nUpgradeIndex = nUpgradeIndex;
    return pEntry;
}
// PC 0x00729930
void CSWGuiUpgrade::InsertUpgrade(CSWSItem* pItem, int nSlot)
{
    if (pItem != 0)
    {
        int nIndex = m_aItemsTaken.IndexOf(pItem);
        if (nIndex != -1)
        {
            m_aItemsTaken.DelIndex(nIndex);
            if (pItem->m_nStackSize > 1)
            {
                pItem = pItem->SplitItem(1);
            }
            else
            {
                g_pAppManager->m_pServerExoApp->GetPartyTable()
                    ->GetPartyItems()->RemoveItem(pItem);
            }
        }
        else
        {
            if (pItem->m_nStackSize > 1)
            {
                pItem = pItem->SplitItem(1);
            }
            else
            {
                g_pAppManager->m_pServerExoApp->GetPartyTable()
                    ->GetPartyItems()->RemoveItem(pItem);
            }
            m_aItemsAdded.Add(pItem);
        }
        m_apItemEntries[nSlot] = pItem;
    }
}

const CSWGuiUpgrade::SlotInfo
    CSWGuiUpgrade::SLOT_INFO[NUM_ITEMLIST_MODES][NUM_UPGRADE_SLOTS] =
{
    { { 0, "i_powerc", 48473 }, { -1, "i_powerc", 48472 },
      { 0, "i_powerc", 48473 }, { 1, "i_lens", 48464 },
      { 2, "i_emitter", 48465 }, { 3, "i_lsenergy", 48466 } },
    { { 4, "i_scope", 48471 }, { 5, "i_energy", 48470 },
      { 6, "i_imp_eng", 48469 }, { -1, "", 0 },
      { -1, "", 0 }, { -1, "", 0 } },
    { { 7, "i_grip", 48467 }, { 8, "i_edge", 48468 },
      { 9, "i_vcell", 48466 }, { -1, "", 0 },
      { -1, "", 0 }, { -1, "", 0 } },
    { { 10, "i_armorrein", 48475 }, { 11, "i_armorrein", 48474 },
      { -1, "", 0 }, { -1, "", 0 },
      { -1, "", 0 }, { -1, "", 0 } }
};

const CSWGuiUpgrade::SlotInfo
    CSWGuiSwoopUpgrade::SWOOP_SLOT_INFO[NUM_SWOOP_SLOTS] =
{
    { 0, "i_powerc", 48553 }, { 1, "i_colorc", 48554 },
    { 2, "i_powerc", 48555 }, { -1, "i_powerc", 0 },
    { -1, "i_powerc", 0 }, { -1, "i_powerc", 0 }
};
// PC 0x00724610
void CSWGuiUpgrade::OnPanelRemoved()
{
    CSWSItem* pItem;
    int i;
    m_nItemListMode = 0;
    m_pSelectedItem = 0;
    pItem = m_pSelectedItemCopy;
    if (pItem != 0)
    {
        delete pItem;
        m_pSelectedItemCopy = 0;
    }
    for (i = 0; i < NUM_UPGRADE_SLOTS; i++)
    {
        if (m_apItemEntries[i] != 0)
        {
            delete m_apItemEntries[i];
            m_apItemEntries[i] = 0;
        }
    }
    m_aItemsTaken.num = 0; m_aItemsAdded.num = 0;
#ifndef _XBOX
    CSWGuiPanel::OnPanelRemoved();
#endif
}
// PC 0x007254C0
void CSWUpgradeItemEntry::SetExtent(const CSWGuiExtent& cExtent)
{

    if (cExtent.m_nWidth > ENTRY_HEIGHT)
    {
        m_cExtent = cExtent;
        CSWGuiExtent cIconExtent(m_cExtent.m_nLeft, m_cExtent.m_nTop,
                                ENTRY_HEIGHT, ENTRY_HEIGHT);
        m_CSWGuiBorder_1D8.SetExtent(cIconExtent);
        m_CSWGuiBorder_24C.SetExtent(cIconExtent);
        m_CSWGuiBorder_2C0.SetExtent(cIconExtent);

        CSWGuiExtent cCountExtent = cIconExtent;
        if (m_CSWGuiText_334.m_cParams.m_sText.GetLength() > 2)
            cCountExtent.m_nWidth = 42;
        else
            cCountExtent.m_nWidth = 21;
        cCountExtent.m_nLeft += ENTRY_HEIGHT - cCountExtent.m_nWidth;
        cCountExtent.m_nTop += ENTRY_HEIGHT - COUNT_HEIGHT;
        cCountExtent.m_nHeight = COUNT_HEIGHT;
        m_CSWGuiText_334.SetExtent(cCountExtent);

        cIconExtent.m_nLeft += ENTRY_HEIGHT;
        cIconExtent.m_nWidth = m_cExtent.m_nWidth - ENTRY_HEIGHT;
        m_cBorder.SetExtent(cIconExtent);
        m_cHilite.SetExtent(cIconExtent);
        m_cText.SetExtent(CSWGuiExtent::Intersection(m_cBorder.GetInnerExtent(),
                                                   m_cHilite.GetInnerExtent()));
    }
}
// PC 0x00725640
void CSWUpgradeItemEntry::Draw(float fElapsed)
{
    if (m_bActive)
    {
        m_cHilite.Draw(fElapsed);
        m_CSWGuiBorder_24C.Draw(fElapsed);
    }
    else
    {
        m_cBorder.Draw(fElapsed);
        m_CSWGuiBorder_1D8.Draw(fElapsed);
    }
    m_CSWGuiBorder_2C0.Draw(fElapsed);
    m_cText.Draw(fElapsed);
    m_CSWGuiText_334.Draw(fElapsed);
}
// PC 0x007256B0
void CSWUpgradeItemEntry::Initialize(CSWGuiText& cText, CSWGuiBorder& cBorder,
                                     CSWGuiBorder& cHilite, int nWidth, int bSlotEntry)
{
    m_cText.m_cParams = cText.m_cParams;
    CSWGuiBorderParams* pParams = &m_cBorder.m_cParams;
    *pParams = cBorder.m_cParams;
    if (!bSlotEntry)
        pParams->m_vColor = CGuiInGame::COLOR_UPGRADE_BORDER;
    else
        pParams->m_vColor = CGuiInGame::COLOR_DARK_TAN;
    pParams = &m_cHilite.m_cParams;
    *pParams = cHilite.m_cParams;
    if (!bSlotEntry)
        pParams->m_vColor = CGuiInGame::COLOR_UPGRADE_TEXT;
    else
        pParams->m_vColor = CGuiInGame::COLOR_NEW_HIGHLIGHT;
    pParams->SetPulsingAlpha(1, 1, 0);
    pParams = &m_CSWGuiBorder_1D8.m_cParams;
    {
#ifndef _XBOX
        CResRef cFill("UIBIT_EQP_ITM1"); CResRef cEdge(""); CResRef cCorner(""); *pParams = CSWGuiBorderParams(cCorner, cEdge, cFill, 0, 0, 1.0f, Vector(1.0f, 1.0f, 1.0f), 0);
#else
        *pParams = CSWGuiBorderParams("", "", "UIBIT_EQP_ITM1", 0, 0, 1.0f, Vector(1.0f, 1.0f, 1.0f), 0);
#endif
        if (!bSlotEntry)
            pParams->m_vColor = CGuiInGame::COLOR_UPGRADE_BORDER;
        else
            pParams->m_vColor = CGuiInGame::COLOR_DARK_TAN;
    }
    pParams = &m_CSWGuiBorder_24C.m_cParams;
    {
#ifndef _XBOX
        CResRef cFill("UIBIT_EQP_ITM1"); CResRef cEdge(""); CResRef cCorner(""); *pParams = CSWGuiBorderParams(cCorner, cEdge, cFill, 0, 0, 1.0f, Vector(1.0f, 1.0f, 1.0f), 0);
#else
        *pParams = CSWGuiBorderParams("", "", "UIBIT_EQP_ITM1", 0, 0, 1.0f, Vector(1.0f, 1.0f, 1.0f), 0);
#endif
        if (!bSlotEntry)
            pParams->m_vColor = CGuiInGame::COLOR_UPGRADE_TEXT;
        else
            pParams->m_vColor = CGuiInGame::COLOR_NEW_HIGHLIGHT;
        pParams->SetPulsingAlpha(1, 0, 1);
    }
    SetExtent(CSWGuiExtent(0, 0, nWidth, ENTRY_HEIGHT));
}

#ifndef _XBOX
template <> int CExoArrayList<CSWSItem*>::IndexOf(CSWSItem* value) const  // PC 0x004DD4A0
{
    for (int i = 0; i < num; i++)
    {
        if (element[i] == value)
        {
            return i;
        }
    }
    return -1;
}
#endif
// PC 0x00723C90
unsigned long CSWGuiUpgradeItemSelect::GetUpgradeItemId()
{
    if (m_pUpgradeItem != 0)
    {
        return m_pUpgradeItem->m_nItemId;
    }
    return OBJECT_INVALID;
}
// PC 0x00723CB0
unsigned long CSWGuiUpgradeItemSelect::GetUpgradeItemProperty()
{
    if (m_pUpgradeItem != 0)
    {
        return m_pUpgradeItem->m_nItemProperty;
    }
    return 0;
}
#ifndef _XBOX
// PC 0x00723CD0
void CSWSItem::ClearUpgradeActive(unsigned char nUpgradeType)
{
    m_nUpgrades &= ~(UPGRADE_PROPERTY_MASK << nUpgradeType);
}
#endif
// PC 0x00727F30
CSWUpgradeItemEntry::CSWUpgradeItemEntry()
{
    m_bShowItemSuffix = 0;
    m_oidItem = OBJECT_INVALID;
    m_nUpgradeIndex = -1;
}
// PC 0x00733EB0
CSWGuiUpgradeSelection::CSWGuiUpgradeSelection(CSWGuiManager* pManager, unsigned long oidCharacter)
    : CSWGuiPanel(pManager)
{
    StartLoadFromLayout("upgradesel_p");
    InitControl(m_lblTitle, "LBL_TITLE", 1);
    InitControl(m_lblTitle2, "LBL_TITLE2", 1);
    InitControl(m_aTypeButtons[0], "BTN_ALL", 1);
    InitControl(m_aTypeButtons[1], "BTN_LIGHTSABER", 1);
    InitControl(m_aTypeButtons[2], "BTN_RANGED", 1);
    InitControl(m_aTypeButtons[3], "BTN_MELEE", 1);
    InitControl(m_aTypeButtons[4], "BTN_ARMOR", 1);
    InitControl(m_lbUpgradelist, "LB_UPGRADELIST", 1);
    InitControl(m_lbDescription, "LB_DESCRIPTION", 1);
    InitControl(m_btnBack, "BTN_BACK", 1);
    InitControl(m_btnUpgradeitems, "BTN_UPGRADEITEMS", 1);
    InitControl(m_btnCreateitems, "BTN_CREATEITEMS", 1);
    StopLoadFromLayout();
    m_lbUpgradelist.SetSelectOnLeftClick(1);
    m_lblDescription.Initialize(m_lbDescription.m_pProtoItem->m_cExtent,
                                (CSWGuiLabel*)m_lbDescription.m_pProtoItem, 1.0f);

    m_oidCharacter = oidCharacter;
    m_oidItem = OBJECT_INVALID;
    m_pItemSelect = new CSWGuiUpgradeItemSelect(pManager);
    m_pItemSelect->m_pSelectionPanel = this;
    m_pCreateItems = new CSWGuiCreateItem(pManager, oidCharacter);
    m_pCreateItems->m_pUpgradeSelection = this;
    for (int i = 0; i < 5; i++)
    {
        m_aTypeButtons[i].m_nIndex = i;
        m_aTypeButtons[i].AddEvent(GUIEVENT_ACTIVATE, this,
                                   (CSWGuiHandler)&CSWGuiUpgradeSelection::OnFilterPicked);
        m_aTypeButtons[i].SetFocusable(0);
    }
    m_btnBack.SetFocusable(0);
    m_btnBack.AddEvent(GUIEVENT_ACTIVATE, this,
                       (CSWGuiHandler)&CSWGuiPanel::OnBButtonPressed);
    m_btnUpgradeitems.SetFocusable(0);
    m_btnUpgradeitems.AddEvent(GUIEVENT_ACTIVATE, this,
                               (CSWGuiHandler)&CSWGuiUpgradeSelection::OnItemSelected);
    m_btnCreateitems.SetFocusable(0);
    m_btnCreateitems.AddEvent(GUIEVENT_ACTIVATE, this,
                              (CSWGuiHandler)&CSWGuiUpgradeSelection::OnCreateItems);
    m_pSelectedType = 0;
    m_nFilter = 0;
    UpdateFilterButtons();
    m_bDisableItemCreation = 0;
    m_bDisableUpgrade = 0;
}
// PC 0x00731CF0
CSWGuiUpgradeItemSelect::CSWGuiUpgradeItemSelect(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{

#ifdef _XBOX
    StartLoadFromLayout("upgradeitems");
#else
    {
        CResRef cLayout("upgradeitems_p");
        StartLoadFromLayout(cLayout);
    }
#endif
    InitControl(m_lblTitle, "LBL_TITLE", 1);
    InitControl(m_lbItems, "LB_ITEMS", 1);
    InitControl(m_lbDescription, "LB_DESCRIPTION", 1);
    m_lbDescription.m_nSelectSound = CSWGuiManager::GUISOUND_SELECTION;
#ifdef _XBOX
    InitControl(m_lblA, "LBL_A", 1);
    InitControl(m_lblUpgradeitem, "LBL_UPGRADEITEM", 1);
    InitControl(m_lblB, "LBL_B", 1);
    InitControl(m_lblBack, "LBL_BACK", 1);
    InitControl(m_lblRanalog, "LBL_RANALOG", 1);
#else
    InitControl(m_btnUpgradeitem, "BTN_UPGRADEITEM", 1);
    InitControl(m_btnBack, "BTN_BACK", 1);
#endif
    StopLoadFromLayout();
#ifndef _XBOX
    m_lblDescription.Initialize(m_lbDescription.m_pProtoItem->m_cExtent, (CSWGuiLabel*)m_lbDescription.m_pProtoItem, 1.0f);
#else
    m_lblDescription.Initialize(m_lbDescription.m_pProtoItem->m_cExtent, (CSWGuiLabel*)m_lbDescription.m_pProtoItem);
#endif
    m_pUpgradePanel = new CSWGuiUpgrade(pManager);
    m_pUpgradePanel->m_pItemSelect = this;
#ifndef _XBOX
    m_btnUpgradeitem.SetFocusable(0);
    m_btnUpgradeitem.AddEvent(GUIEVENT_ACTIVATE, this, (CSWGuiHandler)&CSWGuiUpgradeItemSelect::OnUpgradableSelected);
    m_btnBack.SetFocusable(0);
    m_btnBack.AddEvent(GUIEVENT_ACTIVATE, this, (CSWGuiHandler)&CSWGuiPanel::OnBButtonPressed);
    m_lbItems.SetSelectOnLeftClick(1);
#endif
    m_nUpgradeType = 0;
    m_pPresetItem = 0;
    m_nC84 = 0;
#ifndef _XBOX
    SetActiveControl(&m_lbItems, 0);
    m_bDualWield = 0;
    m_bSecondWeaponSet = 0;
    m_oidOtherWeapon = OBJECT_INVALID;
#endif
}
// PC 0x0072C990
void CSWGuiUpgradeItemSelect::BuildUpgradeList()
{

    CExoArrayList<CSWGuiControl*> aItems;
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    CSWPartyTable* pParty = pServer->GetPartyTable();
    CItemRepository* pRepository = pParty->GetPartyItems();
    CSWGuiListBox* pListBox = &m_lbItems;
    CSWGuiButton* pProto = (CSWGuiButton*)pListBox->m_pProtoItem;
    int nWidth = pListBox->m_cItemExtent.m_nWidth -
                 2 * pListBox->m_nPadding;
    C2DA* pTable = new C2DA("upgrade", 0);
    pTable->Load2DArray();
    unsigned long oidPC = pServer->GetPlayerList()->GetHead()->AsSWSPlayer()->m_oidPCObject;
    CSWSCreature* pCreature = pServer->GetCreatureByGameObjectID(oidPC);
    BuildItemList(pCreature, aItems, pTable);
    for (unsigned int i = 0; i < CSWPartyTable::MAX_NPCS; i++)
    {
        if (pParty->GetIsNPCAvailable(i) && pParty->GetNPCSelectability(i))
        {
            pCreature = pServer->GetCreatureByGameObjectID(
                pParty->GetNPCObject(i, 0, 1));
            if (pCreature != 0)
                BuildItemList(pCreature, aItems, pTable);
        }
    }
    for (unsigned int i = 0; i < (unsigned int)pRepository->m_lstItems.num; i++)
    {
        CSWSItem* pItem = pRepository->ItemListGetItem(i);
        if (pItem != 0 && pItem->GetUpgradableType(pTable) == m_nUpgradeType)
        {
            CSWUpgradeItemEntry* pEntry = new CSWUpgradeItemEntry;
            pEntry->Initialize(pProto->m_cText, pProto->m_cBorder,
                               pProto->m_cHilite, nWidth, 0);
            pEntry->AddEvent(CSWGuiControl::EVENT_ENTER, this,
                             (CSWGuiHandler)&CSWGuiUpgradeItemSelect::OnControlEntered);
            pEntry->AddEvent(CSWGuiControl::EVENT_SELECT, this,
                             (CSWGuiHandler)&CSWGuiUpgradeItemSelect::OnUpgradableSelected);
            pEntry->AddEvent(CSWGuiControl::EVENT_CONFIRM, this,
                             (CSWGuiHandler)&CSWGuiUpgradeItemSelect::OnUpgradableSelected);
            unsigned long oidItem = pItem->m_nId;
            pEntry->SetItem(oidItem, 0, OBJECT_INVALID, 0);
            aItems.Add(pEntry);
        }
    }
    delete pTable;
    pListBox->AddControls(aItems, 1, 0, 0);
}
// PC 0x0072CC90
void CSWGuiUpgradeItemSelect::FinishUpgrading()
{

    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    CSWSItem* pItem = m_pUpgradePanel->m_pSelectedItem;
    if (m_pUpgradeItem->m_bShowItemSuffix)
    {
        unsigned long nSlot = m_pUpgradeItem->m_nItemProperty;
        unsigned long oidCreature = m_pUpgradeItem->m_nItemId;
        CSWSCreature* pCreature = pServer->GetCreatureByGameObjectID(oidCreature);
        pCreature->SetCombatState(1, 2, 0);
        int bCanEquip = 1;
        if (!pCreature->CanEquipItem(pItem, &nSlot, 0, 0, 0))
        {
            pServer->GetPartyTable()->GetPartyItems()->AddItem(&pItem, 1, 0, 0);
            m_pUpgradeItem->SetItem(pItem->m_nId, 0, OBJECT_INVALID, 0);
            bCanEquip = 0;
        }
        if (m_bDualWield)
        {
            if (m_bLeftHandItem && !m_bSecondWeaponSet)
            {
                pCreature->EquipItem(INVENTORY_SLOT_RIGHTHAND,
                    pServer->GetItemByGameObjectID(m_oidOtherWeapon), 1, 0, 0);
                if (bCanEquip)
                {
                    pCreature->EquipItem(INVENTORY_SLOT_LEFTHAND, pItem, 1, 0, 0);
                }
            }
            else if (!m_bLeftHandItem && !m_bSecondWeaponSet)
            {
                if (bCanEquip)
                {
                    pCreature->EquipItem(INVENTORY_SLOT_RIGHTHAND, pItem, 1, 0, 0);
                    pCreature->EquipItem(INVENTORY_SLOT_LEFTHAND,
                        pServer->GetItemByGameObjectID(m_oidOtherWeapon), 1, 0, 0);
                }
                else
                {
                    pCreature->EquipItem(INVENTORY_SLOT_RIGHTHAND,
                        pServer->GetItemByGameObjectID(m_oidOtherWeapon), 1, 0, 0);
                }
            }
            else if (m_bLeftHandItem && m_bSecondWeaponSet)
            {
                pCreature->EquipItem(INVENTORY_SLOT_RIGHTHAND2,
                    pServer->GetItemByGameObjectID(m_oidOtherWeapon), 1, 0, 0);
                if (bCanEquip)
                {
                    pCreature->EquipItem(INVENTORY_SLOT_LEFTHAND2, pItem, 1, 0, 0);
                }
            }
            else if (!m_bLeftHandItem && m_bSecondWeaponSet)
            {
                if (bCanEquip)
                {
                    pCreature->EquipItem(INVENTORY_SLOT_RIGHTHAND2, pItem, 1, 0, 0);
                    pCreature->EquipItem(INVENTORY_SLOT_LEFTHAND2,
                        pServer->GetItemByGameObjectID(m_oidOtherWeapon), 1, 0, 0);
                }
                else
                {
                    pCreature->EquipItem(INVENTORY_SLOT_RIGHTHAND2,
                        pServer->GetItemByGameObjectID(m_oidOtherWeapon), 1, 0, 0);
                }
            }
        }
        else if (bCanEquip)
        {
            pCreature->EquipItem(nSlot, pItem, 1, 0, 0);
        }
    }
    else
    {
        pServer->GetPartyTable()->GetPartyItems()->AddItem(&pItem, 1, 0, 0);
    }
    if (m_pPresetItem != 0 && m_nC84 != 0)
    {
        SetModal(0);
        m_pManager->PopModalPanel();
        g_pAppManager->m_pClientExoApp->GetInGameGui()->HideUpgradeScreen();
        m_pPresetItem = 0;
        delete m_pUpgradeItem;
    }
    else if (m_pPresetItem != 0)
    {
        SetModal(0);
        m_pManager->PopModalPanel();
        m_pPresetItem = 0;
        m_pSelectionPanel->PopulateItemList();
        delete m_pUpgradeItem;
    }
    else
    {
        if (m_pUpgradeItem->m_oidItem != pItem->m_nId)
        {
            BuildUpgradeList();
        }

        for (int i = 0; m_lbItems.m_lItems.num; i++)
        {
            CSWUpgradeItemEntry* pEntry = (CSWUpgradeItemEntry*)m_lbItems.GetControl(i);
            if (pEntry->m_oidItem == pItem->m_nId)
            {
                m_pUpgradeItem = pEntry;
                break;
            }
        }
        m_lbItems.SetSelectedControl(-1, 0);
        m_lbItems.SetSelectedControl(m_pUpgradeItem, 0);
    }
}
// PC 0x00725940
void CSWUpgradeItemEntry::SetItem(unsigned long oidItem, int bShowItemSuffix,
                                  unsigned long nItemId, unsigned long nItemProperty)
{

    CResRef cIcon;
    m_oidItem = oidItem;
    m_bShowItemSuffix = bShowItemSuffix;
    m_nItemId = nItemId;
    m_nItemProperty = nItemProperty;
    if (oidItem != OBJECT_INVALID)
    {
        CServerExoApp* pServer = g_pAppManager->GetServerExoApp();
        CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
        CSWSItem* pItem = pServer->GetItemByGameObjectID(oidItem);
        if (pItem != 0)
        {
            CExoString sName;
            pItem->m_sLocalizedName.GetString(pClient->GetClientLanguage(), &sName, 0);
            if (m_bShowItemSuffix)
                sName = sName + " (" +
                        g_pAppManager->GetClientExoApp()->GetGUIString(STRREF_EQUIPPED) +
                        ")";
            int nTag = sName.Find('<', 0);
            int nToken = sName.Find('{', 0);
            if (nTag != -1 || nToken != -1)
            {
                CSWCCreature* pPlayerCharacter =
                    g_pAppManager->m_pClientExoApp->GetSWParty()->GetPlayerCharacter();
                g_pSWTlkTable->ParseStr(sName, (CGameObject*)pPlayerCharacter, 0, 1);
            }
            m_cText.m_cParams.SetText(sName);
            CExoString sCount;
            int nCount = pItem->m_nStackSize;
            if (nCount > 1)
            {
                if (nCount >= 100)
                {
                    m_CSWGuiBorder_1D8.m_cParams.SetFillImage("UIBIT_EQP_ITM3", 0);
                    m_CSWGuiBorder_24C.m_cParams.SetFillImage("UIBIT_EQP_ITM3", 0);
                }
                else
                {
                    m_CSWGuiBorder_1D8.m_cParams.SetFillImage("UIBIT_EQP_ITM2", 0);
                    m_CSWGuiBorder_24C.m_cParams.SetFillImage("UIBIT_EQP_ITM2", 0);
                }
                sCount.Format("%d", nCount);
            }
            else
            {
                m_CSWGuiBorder_1D8.m_cParams.SetFillImage("UIBIT_EQP_ITM1", 0);
                m_CSWGuiBorder_24C.m_cParams.SetFillImage("UIBIT_EQP_ITM1", 0);
            }
            m_CSWGuiText_334.m_cParams = CSWGuiTextParams(sCount, "fnt_d16x16",
                CGuiInGame::COLOR_UPGRADE_TEXT, CSWGuiTextParams::ALIGNMENT_ITEM_COUNT, 1.0f);
            m_CSWGuiText_334.m_cParams.SetText(sCount);
            cIcon = pItem->GetIcon();
        }
        else
        {
            m_CSWGuiBorder_1D8.m_cParams.SetFillImage("UIBIT_EQP_ITM1", 0);
            m_CSWGuiBorder_24C.m_cParams.SetFillImage("UIBIT_EQP_ITM1", 0);
            m_cText.m_cParams.SetText(g_pAppManager->GetClientExoApp()->GetGUIString(CClientExoApp::GUI_STRREF_NONE));
            m_CSWGuiText_334.m_cParams = CSWGuiTextParams(CExoString(), "fnt_d16x16",
                CGuiInGame::COLOR_UPGRADE_TEXT, CSWGuiTextParams::ALIGNMENT_ITEM_COUNT, 1.0f);
            m_CSWGuiText_334.m_cParams.SetText("");
            cIcon = "inone_blue";
        }
    }
    else
    {
        m_CSWGuiBorder_1D8.m_cParams.SetFillImage("UIBIT_EQP_ITM1", 0);
        m_CSWGuiBorder_24C.m_cParams.SetFillImage("UIBIT_EQP_ITM1", 0);
        m_cText.m_cParams.SetText(g_pAppManager->GetClientExoApp()->GetGUIString(CClientExoApp::GUI_STRREF_NONE));
        m_CSWGuiText_334.m_cParams = CSWGuiTextParams(CExoString(), "fnt_d16x16",
            CGuiInGame::COLOR_BLUE, CSWGuiTextParams::ALIGNMENT_ITEM_COUNT, 1.0f);
        cIcon = "inone_blue";
    }
    CSWGuiBorderParams* pParams = &m_CSWGuiBorder_2C0.m_cParams;
    pParams->m_nFillMode = CSWGuiBorderParams::FILL_MODE_BACKGROUND;
    pParams->SetFillImage(cIcon, 0);
}
// PC 0x00726EE0
CSWGuiUpgradeSelection::~CSWGuiUpgradeSelection()
{
#ifndef _XBOX
    m_lbDescription.ClearItems();
    m_lbUpgradelist.ClearItems();
#endif
    if (m_pItemSelect != 0)
    {
        delete m_pItemSelect;
        m_pItemSelect = 0;
    }
}
// PC 0x0072E0B0
void CSWGuiUpgradeSelection::OnPanelAdded()
{
    C2DA* pTable = new C2DA("upgrade", 0);
    pTable->Load2DArray();
    m_pCreateItems->SetCreateTable(m_sOverride2DA);
    if (m_bDisableUpgrade)
    {
        m_pManager->AddPanel(m_pCreateItems, CSWGuiManager::PANEL_MODE_MODAL, 1);
    }
    CSWSItem* pItem;
    if (m_oidItem != OBJECT_INVALID &&
        (pItem = g_pAppManager->m_pServerExoApp->GetItemByGameObjectID(m_oidItem)) != 0)
    {
        int nType = pItem->GetUpgradableType(pTable);
        m_pItemSelect->m_nUpgradeType = nType;
        m_pItemSelect->m_pPresetItem = pItem;
        m_pItemSelect->m_nC84 = 1;
        m_pManager->AddPanel(m_pItemSelect, CSWGuiManager::PANEL_MODE_MODAL, 1);
    }
    else
    {
        PopulateItemList();
    }
    delete pTable;
    if (m_bDisableItemCreation)
    {
        m_btnCreateitems.SetEnabledFlag(0);
    }
    CSWGuiPanel::OnPanelAdded();
}
// PC 0x0072E200
void CSWGuiUpgradeSelection::OnFilterPicked(CSWGuiControl* pControl)
{
    m_nFilter = pControl->AsButton()->m_nIndex;
    UpdateFilterButtons();
    PopulateItemList();
}
// PC 0x00728F70
void CSWGuiUpgradeSelection::GetPlayerItems(CSWSCreature* pCreature,
                                            CExoArrayList<CSWGuiControl*>& aItems,
                                            C2DA* pTable)
{
    int nWidth = m_lbUpgradelist.m_cItemExtent.m_nWidth -
                 2 * m_lbUpgradelist.m_nPadding;
    CSWGuiButton* pProto = (CSWGuiButton*)m_lbUpgradelist.m_pProtoItem;

    for (int i = 0; i < CSWInventory::TOTAL_SLOT_COUNT; i++)
    {
        unsigned long nSlot = 1 << i;
        if (nSlot == 0x4000 || nSlot == 0x8000 || nSlot == 0x10000 || nSlot == 0x20000)
        {
            continue;
        }
        CSWSItem* pItem = pCreature->m_pInventory->GetItemInSlot(nSlot);
        if (pItem != 0)
        {
            int nType = pItem->GetUpgradableType(pTable);
            if (nType > 0 && (m_nFilter == 0 || nType == m_nFilter))
            {
                CSWUpgradeItemEntry* pEntry = new CSWUpgradeItemEntry;
                pEntry->Initialize(pProto->m_cText, pProto->m_cBorder, pProto->m_cHilite, nWidth, 0);
                pEntry->AddEvent(CSWGuiControl::EVENT_ENTER, this,
                                 (CSWGuiHandler)&CSWGuiUpgradeSelection::OnItemEntered);
                pEntry->AddEvent(CSWGuiControl::EVENT_SELECT, this,
                                 (CSWGuiHandler)&CSWGuiUpgradeSelection::OnItemSelected);
                pEntry->AddEvent(CSWGuiControl::EVENT_CONFIRM, this,
                                 (CSWGuiHandler)&CSWGuiUpgradeSelection::OnItemSelected);
                pEntry->SetItem(pItem->m_nId, 1, pCreature->m_nId, nSlot);
                aItems.Add(pEntry);
            }
        }
    }
}
// PC 0x0072C580
void CSWGuiUpgradeSelection::PopulateItemList()
{
    CExoArrayList<CSWGuiControl*> aItems;
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    CSWPartyTable* pParty = pServer->GetPartyTable();
    CItemRepository* pRepository = pParty->GetPartyItems();
    CSWGuiButton* pProto = (CSWGuiButton*)m_lbUpgradelist.m_pProtoItem;
    int nWidth = m_lbUpgradelist.m_cItemExtent.m_nWidth -
                 2 * m_lbUpgradelist.m_nPadding;
    C2DA* pTable = new C2DA("upgrade", 0);
    pTable->Load2DArray();
    SetDescription("");
    unsigned long oidPC = pServer->GetPlayerList()->GetHead()->AsSWSPlayer()->m_oidPCObject;
    CSWSCreature* pCreature = pServer->GetCreatureByGameObjectID(oidPC);
    if (pCreature->m_bPlayerCreated != 0)
    {
        GetPlayerItems(pCreature, aItems, pTable);
        for (unsigned int i = 0; i < CSWPartyTable::MAX_NPCS; i++)
        {
            if (pParty->GetIsNPCAvailable(i))
            {
                pCreature = pServer->GetCreatureByGameObjectID(
                    pParty->GetNPCObject(i, 0, 0));
                if (pCreature != 0)
                    GetPlayerItems(pCreature, aItems, pTable);
            }
        }
    }
    else
    {
        for (unsigned int i = 0; i < 3; i++)
        {
            CSWCCreature* pMember = g_pAppManager->m_pClientExoApp->GetSWParty()->GetCharacter(i);
            if (pMember != 0)
            {
                CSWSCreature* pServerMember = pMember->GetServerCreature();
                if (pServerMember != 0)
                    GetPlayerItems(pServerMember, aItems, pTable);
            }
        }
    }
    for (unsigned int nItem = 0; nItem < (unsigned int)pRepository->m_lstItems.num; nItem++)
    {
        CSWSItem* pItem = pRepository->ItemListGetItem(nItem);
        if (pItem != 0)
        {
            int nType = pItem->GetUpgradableType(pTable);
            if (nType > 0 && (m_nFilter == 0 || nType == m_nFilter))
            {
                CSWUpgradeItemEntry* pEntry = new CSWUpgradeItemEntry;
                pEntry->Initialize(pProto->m_cText, pProto->m_cBorder,
                                   pProto->m_cHilite, nWidth, 0);
                pEntry->AddEvent(CSWGuiControl::EVENT_ENTER, this,
                                 (CSWGuiHandler)&CSWGuiUpgradeSelection::OnItemEntered);
                pEntry->AddEvent(CSWGuiControl::EVENT_SELECT, this,
                                 (CSWGuiHandler)&CSWGuiUpgradeSelection::OnItemSelected);
                pEntry->AddEvent(CSWGuiControl::EVENT_CONFIRM, this,
                                 (CSWGuiHandler)&CSWGuiUpgradeSelection::OnItemSelected);
                unsigned long oidItem = pItem->m_nId;
                pEntry->SetItem(oidItem, 0, OBJECT_INVALID, 0);
                aItems.Add(pEntry);
            }
        }
    }
    delete pTable;
    CSWGuiListBox* pListBox = &m_lbUpgradelist;
    pListBox->AddControls(aItems, 1, 1, 0);
    if (m_lbUpgradelist.m_lItems.num > 0)
    {
        pListBox->SetSelectedControl(pListBox->GetControl(0), 0);
        m_btnUpgradeitems.m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_UPGRADE_BORDER;
        CSWGuiTextParams* pParams = &m_btnUpgradeitems.m_cText.m_cParams;
        pParams->SetColor(CGuiInGame::COLOR_UPGRADE_TEXT);
        pParams->SetDefaultColor(CGuiInGame::COLOR_UPGRADE_TEXT);
        m_btnUpgradeitems.SetEnabled(1);
    }
    else
    {
        m_btnUpgradeitems.m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_DARK_TAN;
        CSWGuiTextParams* pParams = &m_btnUpgradeitems.m_cText.m_cParams;
        pParams->SetColor(CGuiInGame::COLOR_DARK_TAN);
        pParams->SetDefaultColor(CGuiInGame::COLOR_DARK_TAN);
        m_btnUpgradeitems.SetEnabled(0);
    }
}
// PC 0x00727010
void CSWGuiUpgradeSelection::OnItemEntered(CSWGuiControl* pControl)
{
    int nSelected = pControl->m_bSelected;
    if (nSelected != 0)
    {
        CSWUpgradeItemEntry* pEntry = (CSWUpgradeItemEntry*)pControl;
        if (pEntry->m_oidItem != OBJECT_INVALID)
        {
            CSWSItem* pItem = g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(pEntry->m_oidItem);
            CExoString sDescription;
            sDescription = pItem->GetPropertyDescription();
            if (sDescription != "")
            {
                SetDescription(sDescription);
            }
            else
            {
                SetDescription(g_pAppManager->GetClientExoApp()->GetGUIString(CSWGuiUpgrade::STRREF_NO_DESCRIPTION));
            }
        }
        else
        {
            SetDescription("");
        }
    }
}

void CSWGuiUpgradeSelection::AutoUpgradeDone()
{
    g_pAppManager->m_pClientExoApp->GetInGameGui()->HideUpgradeScreen();
}
// PC 0x00723C20
void CSWGuiCreateItem::SetCreateTable(CExoString sTable)
{
    m_s3EF0 = sTable;
}
// PC 0x00723CF0
void CSWGuiUpgradeSelection::HandleInputEvent(int nEvent, int nState)
{
    CGuiInGame* pInGame;

    if (nState != 0 && (nEvent == 40 || nEvent == 46))
    {
        if (m_bDisableUpgrade == 0)
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
        pInGame = (CGuiInGame*)g_pAppManager->m_pClientExoApp->GetInGameGui();
        pInGame->HideUpgradeScreen();
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
// PC 0x00723D50
void CSWGuiUpgradeSelection::OnCreateItems(CSWGuiControl* pControl)
{
    m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
    m_pManager->AddPanel(m_pCreateItems, CSWGuiManager::PANEL_MODE_MODAL, 1);
}
// PC 0x00723D80
void CSWGuiUpgradeSelection::UpdateFilterButtons()
{
    for (int i = 0; i < 5; i++)
    {
        if (i == m_nFilter)
        {
            m_aTypeButtons[i].m_cText.m_cParams.SetDefaultColor(CGuiInGame::COLOR_NEW_HIGHLIGHT);
        }
        else
        {
            m_aTypeButtons[i].m_cText.m_cParams.SetDefaultColor(CGuiInGame::COLOR_UPGRADE_TEXT);
        }
    }
}
// PC 0x00723DC0
void CSWGuiUpgradeSelection::SetDescription(CExoString sDescription)
{
    CSWGuiListBox* pListBox;
    int nHeight;
    CSWGuiControl* pControl;

    m_lblDescription.m_cText.m_cParams.SetText(sDescription);
    pListBox = &m_lbDescription;
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
    pControl = &m_lblDescription;
    pListBox->AddControls(&pControl, 1, 0, 1, 0);
    pListBox->SetSelectedControl(&m_lblDescription, 0);
}
// PC 0x00723EC0
void CSWGuiUpgradeSelection::OnItemSelected(CSWGuiControl* pControl)
{
    if (pControl == &m_btnUpgradeitems)
    {
        pControl = m_lbUpgradelist.GetSelectedControl();
    }
    if (pControl != 0)
    {
        C2DA* pTable = new C2DA("upgrade", 0);
        pTable->Load2DArray();
        CSWUpgradeItemEntry* pEntry = (CSWUpgradeItemEntry*)pControl;
        if (pEntry->m_oidItem != OBJECT_INVALID)
        {
            CSWSItem* pItem = g_pAppManager->m_pServerExoApp->GetItemByGameObjectID(pEntry->m_oidItem);
            if (pItem != 0)
            {
                int nType = pItem->GetUpgradableType(pTable);
                m_pItemSelect->m_nUpgradeType = nType;
                m_pItemSelect->m_pPresetItem = pItem;
                m_pItemSelect->m_nC84 = 0;
                m_pManager->AddPanel(m_pItemSelect, CSWGuiManager::PANEL_MODE_MODAL, 1);
            }
        }
    }
}
// PC 0x00727130
CSWGuiUpgradeItemSelect::~CSWGuiUpgradeItemSelect()
{
    m_lbItems.ClearItems();
    m_lbDescription.ClearItems();
    if (m_pUpgradePanel != 0)
    {
        delete m_pUpgradePanel;
        m_pUpgradePanel = 0;
    }
}
// PC 0x00727210
void CSWGuiUpgradeItemSelect::OnControlEntered(CSWGuiControl* pControl)
{
    int nSelected = pControl->m_bSelected;
    if (nSelected != 0)
    {
        CSWUpgradeItemEntry* pEntry = (CSWUpgradeItemEntry*)pControl;
        if (pEntry->m_oidItem != OBJECT_INVALID)
        {
            CSWSItem* pItem = g_pAppManager->GetServerExoApp()
                ->GetItemByGameObjectID(pEntry->m_oidItem);
            if (pItem != 0)
            {
                SetDescription(pItem->GetPropertyDescription());
            }
            else
            {
                SetDescription("");
            }
        }
        else
        {
            SetDescription("");
        }
    }
}

// PC 0x00723FD0
void CSWGuiUpgradeItemSelect::SetDescription(const CExoString& sDescription)
{
    CSWGuiListBox* pListBox;
    int nHeight;
    CSWGuiControl* pControl;

    m_lblDescription.m_cText.m_cParams.SetText(sDescription);
    pListBox = &m_lbDescription;
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
    pControl = &m_lblDescription;
    pListBox->AddControls(&pControl, 1, 0, 0, 0);
    pListBox->SetSelectedControl(&m_lblDescription, 0);
#ifdef _XBOX
    m_lblRanalog.SetEnabledFlag(pListBox->IsOversized());
#endif
}
// PC 0x00724090
void CSWGuiUpgradeItemSelect::HandleInputEvent(int nEvent, int nState)
{
    if (nState != 0)
    {
        switch (nEvent)
        {
        case GUIEVENT_CANCEL:
        case GUIEVENT_CANCEL_ALTERNATE:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            SetModal(0);
            m_pManager->PopModalPanel();
            break;

        case 58:
            m_lbDescription.HandleInputEvent(GUIEVENT_DOWN, 1);
            break;
        case 57:
            m_lbDescription.HandleInputEvent(GUIEVENT_UP, 1);
            break;
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
// PC 0x00724150
void CSWGuiUpgradeItemSelect::OnUpgradableSelected(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        if (pControl->m_nEvent == GUIEVENT_ACCEPT)
        {
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
        }
        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
#ifndef _XBOX
        if (pControl == &m_btnUpgradeitem)
        {
            m_pUpgradeItem = (CSWUpgradeItemEntry*)m_lbItems.GetSelectedControl();
            if (m_pUpgradeItem == 0)
            {
                return;
            }
        }
        else
#endif
        {
            m_pUpgradeItem = (CSWUpgradeItemEntry*)pControl;
        }
        unsigned long oidItem = m_pUpgradeItem->m_oidItem;
        CSWSItem* pItem = pServer->GetItemByGameObjectID(oidItem);
        m_bDualWield = 0;
        if (m_pUpgradeItem->m_bShowItemSuffix)
        {
            unsigned long oidCreature = m_pUpgradeItem->m_nItemId;
            CSWSCreature* pCreature = pServer->GetCreatureByGameObjectID(oidCreature);
            if (m_pUpgradeItem->m_nItemProperty == INVENTORY_SLOT_RIGHTHAND ||
                m_pUpgradeItem->m_nItemProperty == INVENTORY_SLOT_LEFTHAND)
            {
                CSWSItem* pRightHand = pCreature->m_pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND);
                CSWSItem* pLeftHand = pCreature->m_pInventory->GetItemInSlot(INVENTORY_SLOT_LEFTHAND);
                if (pLeftHand != 0)
                {
                    if (pRightHand == pItem)
                    {
                        m_bDualWield = 1;
                        m_bLeftHandItem = 0;
                        m_oidOtherWeapon = pLeftHand->m_nId;
                        pCreature->UnequipItem(pLeftHand);
                        pCreature->UnequipItem(pItem);
                    }
                    else if (pLeftHand == pItem)
                    {
                        m_bDualWield = 1;
                        m_bLeftHandItem = 1;
                        m_oidOtherWeapon = pRightHand->m_nId;
                        pCreature->UnequipItem(pItem);
                        pCreature->UnequipItem(pRightHand);
                    }
                }
                else
                {
                    pCreature->UnequipItem(pItem);
                }
            }
            else
            {
                pCreature->UnequipItem(pItem);
            }
        }
        else if (pItem->m_nStackSize > 1)
        {
            pItem = pItem->SplitItem(1);
        }
        else
        {
            pServer->GetPartyTable()->GetPartyItems()->RemoveItem(pItem);
        }
        m_pUpgradePanel->m_nItemListMode = m_nUpgradeType;
        m_pUpgradePanel->m_pSelectedItem = pItem;
        m_pManager->AddPanel(m_pUpgradePanel, CSWGuiManager::PANEL_MODE_MODAL, 1);
    }
}
// PC 0x00729120
void CSWGuiUpgradeItemSelect::BuildItemList(CSWSCreature* pCreature,
                                           CExoArrayList<CSWGuiControl*>& aItems,
                                           C2DA* pTable)
{
    int nWidth = m_lbItems.m_cItemExtent.m_nWidth -
                 2 * m_lbItems.m_nPadding;
    CSWGuiButton* pProto = (CSWGuiButton*)m_lbItems.m_pProtoItem;

    for (int i = 0; i < CSWInventory::TOTAL_SLOT_COUNT; i++)
    {
        unsigned long nSlot = 1 << i;
        CSWSItem* pItem = pCreature->m_pInventory->GetItemInSlot(nSlot);
        if (pItem != 0 && pItem->GetUpgradableType(pTable) == m_nUpgradeType)
        {
            CSWUpgradeItemEntry* pEntry = new CSWUpgradeItemEntry;
            pEntry->Initialize(pProto->m_cText, pProto->m_cBorder, pProto->m_cHilite, nWidth, 0);
            pEntry->AddEvent(CSWGuiControl::EVENT_ENTER, this, (CSWGuiHandler)&CSWGuiUpgradeItemSelect::OnControlEntered);
            pEntry->AddEvent(CSWGuiControl::EVENT_SELECT, this, (CSWGuiHandler)&CSWGuiUpgradeItemSelect::OnUpgradableSelected);
            pEntry->AddEvent(CSWGuiControl::EVENT_CONFIRM, this, (CSWGuiHandler)&CSWGuiUpgradeItemSelect::OnUpgradableSelected);
            pEntry->SetItem(pItem->m_nId, 1, pCreature->m_nId, nSlot);
            aItems.Add(pEntry);
        }
    }
}
#ifndef _XBOX
void CSWGuiUpgrade::ShowItems(int bShow)  // PC 0x007242E0
{

    if (bShow)
    {
        m_3dModel.SetShown(0);
        m_3dModelLs.SetShown(0);
        m_cSlotName.SetShown(0);
        m_cProperty.SetShown(0);
        m_cAssemble.SetShown(0);
        m_btnOk.SetShown(1);
        for (int i = 0; i < 3; i++)
        {
            m_aUpgradeButtons3[i].SetShown(0);
            m_aUpgradeLabels3[i].SetShown(0);
        }
        for (int i = 0; i < 6; i++)
        {
            m_aUpgradeButtons3Ls[i].SetShown(0);
            m_aUpgradeLabels3Ls[i].SetShown(0);
        }
        m_lbItems.SetShown(1);
        return;
    }

    if (m_nItemListMode == ITEMLIST_LIGHTSABER)
    {
        m_3dModel.SetShown(0);
        m_cSlotName.SetShown(1);
        m_cProperty.SetShown(1);
        m_cDescriptionBackground.SetShown(0);
        m_3dModelLs.SetShown(1);
        m_cAssemble.SetShown(1);
        m_btnOk.SetShown(0);
        m_cLightsaberDescriptionBackground.SetShown(1);
        for (int i = 0; i < 6; i++)
        {
            m_aUpgradeButtons3Ls[i].SetShown(1);
            m_aUpgradeLabels3Ls[i].SetShown(1);
        }
        for (int i = 0; i < 3; i++)
        {
            m_aUpgradeButtons3[i].SetShown(0);
            m_aUpgradeLabels3[i].SetShown(0);
        }
    }
    else
    {
        int bShowThreeSlots = 0;
        m_3dModel.SetShown(1);
        m_cSlotName.SetShown(1);
        m_cProperty.SetShown(1);
        m_cAssemble.SetShown(1);
        m_btnOk.SetShown(0);
        m_cDescriptionBackground.SetShown(1);
        m_lbDesc.SetShown(1);
        m_3dModelLs.SetShown(0);
        m_cLightsaberDescriptionBackground.SetShown(0);
        if (m_nItemListMode >= ITEMLIST_BLASTER && m_nItemListMode <= ITEMLIST_ARMOR)
            bShowThreeSlots = 1;
        for (int i = 0; i < 3; i++)
        {
            m_aUpgradeButtons3[i].SetShown(bShowThreeSlots);
            m_aUpgradeLabels3[i].SetShown(bShowThreeSlots);
        }
        for (int i = 0; i < 6; i++)
        {
            m_aUpgradeButtons3Ls[i].SetShown(0);
            m_aUpgradeLabels3Ls[i].SetShown(0);
        }
    }
    if (m_nItemListMode == ITEMLIST_ARMOR)
    {
        m_aUpgradeLabels3[2].SetShown(0);
        m_aUpgradeButtons3[2].SetShown(0);
        m_aUpgradeButtons3[0].SetMoveToControl(3, &m_aUpgradeButtons3[1]);
        m_aUpgradeButtons3[1].SetMoveToControl(1, &m_aUpgradeButtons3[0]);
    }
    else
    {
        m_aUpgradeButtons3[1].SetMoveToControl(1, &m_aUpgradeButtons3[2]);
        m_aUpgradeButtons3[0].SetMoveToControl(3, &m_aUpgradeButtons3[2]);
    }
    m_lbItems.SetShown(0);
}
#endif
CSWGuiUpgrade::CSWGuiUpgrade(CSWGuiManager* pManager)  // PC 0x00730C60
    : CSWGuiPanel(pManager)
{

    CExoString sControl;
    m_pUpgradeTable = new C2DA("upgrade", 0);
    m_pCrystalTable = new C2DA("upcrystals", 0);
    m_pUpgradeTable->Load2DArray();
    m_pCrystalTable->Load2DArray();
#ifdef _XBOX
    StartLoadFromLayout("upgrade");
#else
    {
        CResRef cLayout("upgrade_p");
        StartLoadFromLayout(cLayout);
    }
#endif
    InitControl(m_cTitle, "LBL_TITLE", 1);
#ifndef _XBOX
    InitControl(m_lblTitle2, "LBL_TITLE2", 1);
#endif
    InitControl(m_lbItems, "LB_ITEMS", 1);
    InitControl(m_lbDesc, "LB_DESC", 1);
    m_lbDesc.m_nSelectSound = CSWGuiManager::GUISOUND_SELECTION;
#ifdef _XBOX
    InitControl(m_lbDescLs, "LB_DESC_LS", 1);
#endif

    m_lbDesc.m_nSelectSound = CSWGuiManager::GUISOUND_SELECTION;
    InitControl(m_cDescriptionBackground, "LBL_DESCBG", 1);
    InitControl(m_cLightsaberDescriptionBackground, "LBL_DESCBG_LS", 1);
    InitControl(m_3dModel, "3D_MODEL", 1);
    InitControl(m_3dModelLs, "3D_MODEL_LS", 1);
    InitControl(m_cSlotName, "LBL_SLOTNAME", 1);
#ifdef _XBOX
    InitControl(m_cLightsaberSlotName, "LBL_LSSLOTNAME", 1);
    InitControl(m_cUpgrades, "LBL_UPGRADES", 1);
    InitControl(m_cUpgradeCount, "LBL_UPGRADE_COUNT", 1);
#endif
#ifdef _XBOX
    InitControl(m_lblA, "LBL_A", 1);
    InitControl(m_lblB, "LBL_B", 1);
    InitControl(m_lblAAction, "LBL_A_ACTION", 1);
    InitControl(m_lblBack, "LBL_BACK", 1);
#endif
    InitControl(m_cProperty, "LBL_PROPERTY", 1);
#ifdef _XBOX
    InitControl(m_lblRanalog, "LBL_RANALOG", 1);
    InitControl(m_lblRanalogLs, "LBL_RANALOG_LS", 1);
#endif
    InitControl(m_cAssemble, "BTN_ASSEMBLE", 1);
#ifndef _XBOX
    InitControl(m_btnOk, "BTN_OK", 1);
    InitControl(m_btnBack, "BTN_BACK", 1);
#endif
    m_cAssemble.m_nClickSound = CSWGuiManager::GUISOUND_ASSEMBLE;
    m_cAssemble.AddEvent(GUIEVENT_ACTIVATE, this, (CSWGuiHandler)&CSWGuiUpgrade::OnAssemble);
    m_cAssemble.AddEvent(GUIEVENT_ACCEPT, this, (CSWGuiHandler)&CSWGuiUpgrade::OnAssemble);
#ifdef _XBOX
    m_cAssemble.AddEvent(GUIEVENT_MOUSE_ENTER, this,
                        (CSWGuiHandler)&CSWGuiUpgrade::OnEnterAssemble);
    m_cAssemble.m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_BLUE;
    m_cAssemble.m_cHilite.m_cParams.m_vColor = CGuiInGame::COLOR_YELLOW;
    m_cAssemble.m_cHilite.m_cParams.SetPulsingAlpha(1, 1, 0);
#else
    m_cAssemble.SetFocusable(0);
    m_btnOk.SetFocusable(0);
    m_btnOk.AddEvent(GUIEVENT_ACTIVATE, this, (CSWGuiHandler)&CSWGuiUpgrade::OnUpgradeSelected);
#endif
#ifdef _XBOX
    for (int i = 0; i < 4; i++)
    {
        if (i < 3)
        {
            sControl.Format("LBL_UPGRADE3%d", i + 1);
            InitControl(m_aUpgradeLabels3[i], sControl, 1);
            sControl.Format("BTN_UPGRADE3%d", i + 1);
            InitControl(m_aUpgradeButtons3[i], sControl, 1);
            m_aUpgradeButtons3[i].AddEvent(GUIEVENT_ACTIVATE, this,
                                          (CSWGuiHandler)&CSWGuiUpgrade::OnSlotSelected);
            m_aUpgradeButtons3[i].AddEvent(GUIEVENT_ACCEPT, this,
                                          (CSWGuiHandler)&CSWGuiUpgrade::OnSlotSelected);
            m_aUpgradeButtons3[i].AddEvent(GUIEVENT_MOUSE_ENTER, this,
                                          (CSWGuiHandler)&CSWGuiUpgrade::OnEnterSlot);
            m_aUpgradeButtons3[i].m_nIndex = i;
            m_aUpgradeButtons3[i].m_cHilite.m_cParams.SetPulsingAlpha(1, 0, 1);
        }
        sControl.Format("LBL_UPGRADE4%d", i + 1);
        InitControl(m_aUpgradeLabels4[i], sControl, 1);
        sControl.Format("BTN_UPGRADE4%d", i + 1);
        InitControl(m_aUpgradeButtons4[i], sControl, 1);
        m_aUpgradeButtons4[i].AddEvent(GUIEVENT_ACTIVATE, this,
                                      (CSWGuiHandler)&CSWGuiUpgrade::OnSlotSelected);
        m_aUpgradeButtons4[i].AddEvent(GUIEVENT_ACCEPT, this,
                                      (CSWGuiHandler)&CSWGuiUpgrade::OnSlotSelected);
        m_aUpgradeButtons4[i].AddEvent(GUIEVENT_MOUSE_ENTER, this,
                                      (CSWGuiHandler)&CSWGuiUpgrade::OnEnterSlot);
        m_aUpgradeButtons4[i].m_nIndex = i;
        m_aUpgradeButtons4[i].m_cHilite.m_cParams.SetPulsingAlpha(1, 0, 1);
    }
#else
    int i;
    for (i = 0; i < 3; i++)
    {
        sControl.Format("LBL_UPGRADE3%d", i + 1);
        InitControl(m_aUpgradeLabels3[i], sControl, 1);
        sControl.Format("BTN_UPGRADE3%d", i + 1);
        InitControl(m_aUpgradeButtons3[i], sControl, 1);
        m_aUpgradeButtons3[i].AddEvent(GUIEVENT_ACTIVATE, this,
                                      (CSWGuiHandler)&CSWGuiUpgrade::OnSlotSelected);
        m_aUpgradeButtons3[i].AddEvent(GUIEVENT_ACCEPT, this,
                                      (CSWGuiHandler)&CSWGuiUpgrade::OnSlotSelected);
        m_aUpgradeButtons3[i].AddEvent(GUIEVENT_MOUSE_ENTER, this,
                                      (CSWGuiHandler)&CSWGuiUpgrade::OnEnterSlot);
        m_aUpgradeButtons3[i].m_nIndex = i;
        m_aUpgradeButtons3[i].m_cHilite.m_cParams.SetPulsingAlpha(1, 0, 1);
    }
    for (i = 0; i < 6; i++)
    {
        sControl.Format("LBL_UPGRADE3%d_LS", i + 1);
        InitControl(m_aUpgradeLabels3Ls[i], sControl, 1);
        sControl.Format("BTN_UPGRADE3%d_LS", i + 1);
        InitControl(m_aUpgradeButtons3Ls[i], sControl, 1);
        m_aUpgradeButtons3Ls[i].AddEvent(GUIEVENT_ACTIVATE, this,
                                        (CSWGuiHandler)&CSWGuiUpgrade::OnSlotSelected);
        m_aUpgradeButtons3Ls[i].AddEvent(GUIEVENT_ACCEPT, this,
                                        (CSWGuiHandler)&CSWGuiUpgrade::OnSlotSelected);
        m_aUpgradeButtons3Ls[i].AddEvent(GUIEVENT_MOUSE_ENTER, this,
                                        (CSWGuiHandler)&CSWGuiUpgrade::OnEnterSlot);
        m_aUpgradeButtons3Ls[i].m_nIndex = i;
        m_aUpgradeButtons3Ls[i].m_cHilite.m_cParams.SetPulsingAlpha(1, 0, 1);
    }
#endif
    StopLoadFromLayout();
#ifndef _XBOX
    m_lbItems.SetSelectOnLeftClick(1);
#endif
    m_cSlotName.m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_UPGRADE_BORDER;
#ifdef _XBOX
    m_cLightsaberSlotName.m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_BLUE;
    m_cUpgradeCount.m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_BLUE;
    m_cUpgrades.m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_BLUE;
#endif
    m_cProperty.m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_UPGRADE_BORDER;
    m_lbItems.m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_UPGRADE_BORDER;
#ifndef _XBOX
    m_CSWGuiLabel_1F60.Initialize(m_lbDesc.m_pProtoItem->m_cExtent, (CSWGuiLabel*)m_lbDesc.m_pProtoItem, 1.0f);
#else
    m_CSWGuiLabel_1F60.Initialize(m_lbDescLs.m_pProtoItem->m_cExtent, (CSWGuiLabel*)m_lbDescLs.m_pProtoItem);
#endif
    CSWGuiButton* pProto = (CSWGuiButton*)m_lbItems.m_pProtoItem;
    pProto->m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_BLUE;
    pProto->m_cHilite.m_cParams.m_vColor = CGuiInGame::COLOR_YELLOW;
    pProto->m_cHilite.m_cParams.SetPulsingAlpha(1, 1, 0);
    m_nItemListMode = 0;
    m_pSelectedItem = 0;
    m_pSelectedItemCopy = 0;
    m_bItemListShown = 0;
    for (int j = 0; j < NUM_UPGRADE_SLOTS; j++)
        m_apItemEntries[j] = 0;
    if (g_bDo3DGUIs != 0)
    {
        CSWGuiScene* pScene = &m_3dModel.m_cScene;
        pScene->m_pScene->SpawnRoom("gui3D_room", Vector(), Quaternion(0, 0, 0, 1));
        pScene->AddModel("upgitem_light", -1);
        pScene = &m_3dModelLs.m_cScene;
        pScene->m_pScene->SpawnRoom("gui3D_room", Vector(), Quaternion(0, 0, 0, 1));
        pScene->AddModel("upgitem_light", -1);
    }
#ifndef _XBOX
    m_btnBack.SetFocusable(0);
    m_btnBack.AddEvent(GUIEVENT_ACTIVATE, this, (CSWGuiHandler)&CSWGuiPanel::OnBButtonPressed);
#endif
}
// PC 0x007272F0
CSWGuiUpgrade::~CSWGuiUpgrade()
{
    m_lbItems.ClearItems();
    m_lbDesc.ClearItems();
    if (m_pUpgradeTable != 0)
    {
        delete m_pUpgradeTable;
        m_pUpgradeTable = 0;
    }
    if (m_pCrystalTable != 0)
    {
        delete m_pCrystalTable;
        m_pCrystalTable = 0;
    }
}
// PC 0x00727560
void CSWGuiUpgrade::CloseItems()
{
    CSWGuiControl* pControl;

    ShowItems(0);
    pControl = (CSWGuiControl*)m_pItemListControl;
    m_bItemListShown = 0;
    SetActiveControl(pControl, 0);
    UpdateItems();
    OnEnterSlot(m_pActiveControl);
}
// PC 0x007275A0
void CSWGuiUpgrade::OnPanelAdded()
{
#ifdef _XBOX
    if (m_pSelectedItem == 0)
    {
        SetModal(0);
        return;
    }

    m_pSelectedItemCopy = new CSWSItem(OBJECT_INVALID);
    m_pSelectedItemCopy->CopyItem(m_pSelectedItem);

    int nFirstSlot = 0;
    CSWGuiLabel* pLabels;
    int nUpgradeType;
    switch (m_nItemListMode)
    {
    case ITEMLIST_LIGHTSABER:
    {
        CExoString sColumn;
        CExoString sModel;
        CExoString sTemplate;
        switch (m_pSelectedItem->GetBaseItem()->m_nItemType)
        {
        case CSWBaseItem::ITEM_TYPE_POWERED_HAND_39:
            sColumn = "DoubleMdlVar";
            break;
        case CSWBaseItem::ITEM_TYPE_POWERED_HAND_40:
            sColumn = "ShortMdlVar";
            break;
        case CSWBaseItem::ITEM_TYPE_POWERED_HAND_41:
            sColumn = "LongMdlVar";
            break;
        default:
            SetModal(0);
            return;
        }
        for (int i = 0; i < m_pCrystalTable->GetNumRows(); i++)
        {
#ifdef _XBOX
            if (m_pCrystalTable->GetCExoStringEntry(i, sColumn, &sModel)
                && sModel.CompareNoCase(m_pSelectedItem->m_sTag))
            {
                int bTemplate = m_pCrystalTable->GetCExoStringEntry(i, "Template", &sTemplate);
                if (bTemplate)
                {
                    m_apItemEntries[UPGRADE_SLOT_COLOR_CRYSTAL] = new CSWSItem(OBJECT_INVALID);
                    m_apItemEntries[UPGRADE_SLOT_COLOR_CRYSTAL]->LoadFromTemplate(sTemplate);
                    break;
                }
            }
#else
            if (m_pCrystalTable->GetCExoStringEntry(i, sColumn, &sModel)
                && sModel.CompareNoCase(m_pSelectedItem->m_sTag)
                && m_pCrystalTable->GetCExoStringEntry(i, "Template", &sTemplate))
            {
                m_apItemEntries[UPGRADE_SLOT_COLOR_CRYSTAL] = new CSWSItem(OBJECT_INVALID);
                m_apItemEntries[UPGRADE_SLOT_COLOR_CRYSTAL]->LoadFromTemplate(sTemplate);
                break;
            }
#endif
        }
        if (m_apItemEntries[UPGRADE_SLOT_COLOR_CRYSTAL] == 0)
        {
            SetModal(0);
            return;
        }
#ifdef _XBOX
        m_lblAAction.m_cText.m_cParams.SetStrRef(36841);
#endif
        pLabels = m_aUpgradeLabels3;
        break;
    }
    case ITEMLIST_MELEE:
        pLabels = m_aUpgradeLabels3;
        break;
    case ITEMLIST_BLASTER:
        m_aUpgradeButtons4[1].SetMoveToControl(1, &m_aUpgradeButtons4[0]);
        m_aUpgradeButtons4[2].SetMoveToControl(3, &m_aUpgradeButtons4[3]);
        pLabels = m_aUpgradeLabels3Ls;
        break;
    case ITEMLIST_ARMOR:
        m_aUpgradeButtons4[1].SetMoveToControl(1, &m_aUpgradeButtons4[2]);
        m_aUpgradeButtons4[2].SetMoveToControl(3, &m_aUpgradeButtons4[1]);
        nFirstSlot = 1;
        pLabels = m_aUpgradeLabels3Ls;
        break;
    default:
        SetModal(0);
        return;
    }
    ShowItems(0);

    unsigned char nSlots = 0;
    switch (m_nItemListMode)
    {
    case ITEMLIST_LIGHTSABER:
    case ITEMLIST_MELEE:
        nSlots = 3;
        break;
    case ITEMLIST_BLASTER:
        nSlots = 4;
        break;
    case ITEMLIST_ARMOR:
        nSlots = 2;
        break;
    }
    int nEndSlot = nFirstSlot + nSlots;
    CExoString sTemplate;
    for (int i = 0; i < m_pUpgradeTable->GetNumRows(); i++)
    {
        m_pUpgradeTable->GetINTEntry(i, "UpgradeType", &nUpgradeType);
        m_pUpgradeTable->GetCExoStringEntry(i, "Template", &sTemplate);
        int nSlot;
        for (nSlot = nFirstSlot; nSlot < nEndSlot; nSlot++)
        {
            if (m_apItemEntries[nSlot] == 0
                && nUpgradeType == SLOT_INFO[m_nItemListMode - 1][nSlot].nProperty)
                break;
        }
        if (nSlot < nEndSlot)
        {
            if (m_nItemListMode != ITEMLIST_LIGHTSABER)
            {
                m_asUpgradeTemplates[nSlot] = sTemplate.LowerCase();
                pLabels[nSlot].m_cBorder.m_cParams.m_fHilightAlpha = 1.0f;
            }
            m_anUpgradeProperties[nSlot] = i;
            if (m_pSelectedItem->GetIsUpgradeActive(i))
            {
                m_apItemEntries[nSlot] = new CSWSItem(OBJECT_INVALID);
                m_apItemEntries[nSlot]->LoadFromTemplate(sTemplate);
            }
            else if (m_nItemListMode != ITEMLIST_LIGHTSABER)
            {
                if (g_pAppManager->GetServerExoApp()->GetPartyTable()->GetPartyItems()
                    ->FindItemWithTag(m_asUpgradeTemplates[nSlot]) == OBJECT_INVALID)
                    pLabels[nSlot].m_cBorder.m_cParams.m_fHilightAlpha = 0.25f;
            }
        }
    }
    Update3DView();
    UpdateItems();
#ifdef _XBOX
    SetActiveControl(&m_cAssemble, 0);
#else
    CSWGuiPanel::OnPanelAdded();
#endif
#else
    if (m_pSelectedItem == 0)
    {
        SetModal(0);
        return;
    }

    CExoString sName;
    CExoLocString& sLocName = m_pSelectedItem->m_sLocalizedName;
    sLocName.GetString(g_pAppManager->m_pClientExoApp->GetClientLanguage(), &sName, 0);
    int nTag = sName.Find('<', 0);
    int nToken = sName.Find('{', 0);
    if (nTag != -1 || nToken != -1)
    {
        g_pSWTlkTable->ParseStr(
            sName,
            (CGameObject*)g_pAppManager->m_pClientExoApp->GetSWParty()->GetPlayerCharacter(),
            0, 1);
    }
    m_cTitle.m_cText.m_cParams.SetText(sName);
    {
        CExoString sEmpty("");
        m_cSlotName.m_cText.m_cParams.SetText(sEmpty);
    }
    {
        CExoString sEmpty("");
        m_cProperty.m_cText.m_cParams.SetText(sEmpty);
    }

    m_pSelectedItemCopy = new CSWSItem(OBJECT_INVALID);
    m_pSelectedItemCopy->CopyItem(m_pSelectedItem);

    CSWGuiLabel* pLabels;
    int nUpgradeType;
    switch (m_nItemListMode)
    {
    case ITEMLIST_LIGHTSABER:
    {
        CExoString sColumn;
        CExoString sModel;
        CExoString sTemplate;
        switch (m_pSelectedItem->GetBaseItem()->m_nItemType)
        {
        case CSWBaseItem::ITEM_TYPE_POWERED_HAND_39:
            sColumn = "DoubleMdlVar";
            break;
        case CSWBaseItem::ITEM_TYPE_POWERED_HAND_40:
            sColumn = "ShortMdlVar";
            break;
        case CSWBaseItem::ITEM_TYPE_POWERED_HAND_41:
            sColumn = "LongMdlVar";
            break;
        default:
            SetModal(0);
            return;
        }
        for (int i = 0; i < m_pCrystalTable->GetNumRows(); i++)
        {
            if (m_pCrystalTable->GetCExoStringEntry(i, sColumn, &sModel)
                && sModel.CompareNoCase(m_pSelectedItem->m_sTag)
                && m_pCrystalTable->GetCExoStringEntry(i, "Template", &sTemplate))
            {
                m_apItemEntries[UPGRADE_SLOT_COLOR_CRYSTAL] = new CSWSItem(OBJECT_INVALID);
                CResRef cTemplate(sTemplate);
                m_apItemEntries[UPGRADE_SLOT_COLOR_CRYSTAL]->LoadFromTemplate(cTemplate);
                break;
            }
        }
        pLabels = m_aUpgradeLabels3Ls;
        break;
    }
    case ITEMLIST_BLASTER:
    case ITEMLIST_MELEE:
    case ITEMLIST_ARMOR:
        pLabels = m_aUpgradeLabels3;
        break;
    default:
        SetModal(0);
        return;
    }
    ShowItems(0);

    unsigned char nSlotCount = 0;
    switch (m_nItemListMode)
    {
    case ITEMLIST_LIGHTSABER:
        nSlotCount = NUM_UPGRADE_SLOTS;
        break;
    case ITEMLIST_BLASTER:
    case ITEMLIST_MELEE:
        nSlotCount = 3;
        break;
    case ITEMLIST_ARMOR:
        nSlotCount = 2;
        break;
    }
    int nSlots = nSlotCount;
    CExoString sTemplate;
    int abAvailable[NUM_UPGRADE_SLOTS];
    memset(abAvailable, 0, sizeof(abAvailable));
    int nSlot;
    for (nSlot = 0; nSlot < nSlots; nSlot++)
    {
        pLabels[nSlot].m_cBorder.m_cParams.m_fHilightAlpha = 0.25f;
    }
    for (int i = 0; i < m_pUpgradeTable->GetNumRows(); i++)
    {
        m_pUpgradeTable->GetINTEntry(i, "UpgradeType", &nUpgradeType);
        m_pUpgradeTable->GetCExoStringEntry(i, "Template", &sTemplate);
        for (nSlot = 0; nSlot < nSlots; nSlot++)
        {
            if (m_apItemEntries[nSlot] == 0
                && nUpgradeType == SLOT_INFO[m_nItemListMode - 1][nSlot].nProperty)
                break;
        }
        if (nSlot < nSlots)
        {
            m_asUpgradeTemplates[nSlot] = sTemplate.LowerCase();
            m_anUpgradeProperties[nSlot] = i;
            if (m_pSelectedItem->m_anUpgradeSlot[nSlot] == i)
            {
                m_apItemEntries[nSlot] = new CSWSItem(OBJECT_INVALID);
                CResRef cTemplate(sTemplate);
                m_apItemEntries[nSlot]->LoadFromTemplate(cTemplate);
                m_apItemEntries[nSlot]->m_anUpgradeSlot[nSlot] = i;
            }
            else if (m_nItemListMode == ITEMLIST_LIGHTSABER &&
                     nSlot == UPGRADE_SLOT_POWER_CRYSTAL_1 &&
                     m_pSelectedItem->m_anUpgradeSlot[UPGRADE_SLOT_POWER_CRYSTAL_2] == i)
            {
                m_apItemEntries[UPGRADE_SLOT_POWER_CRYSTAL_2] = new CSWSItem(OBJECT_INVALID);
                CResRef cTemplate(sTemplate);
                m_apItemEntries[UPGRADE_SLOT_POWER_CRYSTAL_2]->LoadFromTemplate(cTemplate);
                m_apItemEntries[UPGRADE_SLOT_POWER_CRYSTAL_2]->m_anUpgradeSlot[UPGRADE_SLOT_POWER_CRYSTAL_2] = i;
            }
            else
            {
                unsigned long oidItem = g_pAppManager->m_pServerExoApp->GetPartyTable()
                    ->GetPartyItems()->FindItemWithTag(m_asUpgradeTemplates[nSlot]);
                if (oidItem != OBJECT_INVALID)
                {
                    CGameObject* pObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidItem);
                    CSWSItem* pItem = pObject != 0 ? pObject->AsSWSItem() : 0;
                    if (pObject->AsSWSItem()->m_nUpgradeLevel <= m_pSelectedItem->m_nUpgradeLevel)
                    {
                        pLabels[nSlot].m_cBorder.m_cParams.m_fHilightAlpha = 1.0f;
                        abAvailable[nSlot] = 1;
                        if (m_nItemListMode == ITEMLIST_LIGHTSABER &&
                            nSlot == UPGRADE_SLOT_POWER_CRYSTAL_1)
                        {
                            pLabels[UPGRADE_SLOT_POWER_CRYSTAL_2].m_cBorder.m_cParams.m_fHilightAlpha =
                                1.0f;
                        }
                    }
                }
                else if (abAvailable[nSlot] == 0)
                {
                    pLabels[nSlot].m_cBorder.m_cParams.m_fHilightAlpha = 0.25f;
                }
            }
        }
    }
    for (nSlot = 0; nSlot < nSlots; nSlot++)
    {
        if (m_apItemEntries[nSlot] != 0)
        {
            pLabels[nSlot].m_cBorder.m_cParams.m_fHilightAlpha = 1.0f;
        }
    }
    if (m_nItemListMode == ITEMLIST_LIGHTSABER)
    {
        pLabels[UPGRADE_SLOT_COLOR_CRYSTAL].m_cBorder.m_cParams.m_fHilightAlpha = 1.0f;
    }
    Update3DView();
    UpdateItems();
    SetActiveControl(pLabels, 0);
    CSWGuiPanel::OnPanelAdded();
#endif
}
// PC 0x00727DA0
void CSWGuiUpgrade::OnControlEntered(CSWGuiControl* pControl)
{
    int nSelected = pControl->m_bSelected;
    if (nSelected != 0)
    {
        CSWUpgradeItemEntry* pEntry = (CSWUpgradeItemEntry*)pControl;
        if (pEntry->m_oidItem != OBJECT_INVALID)
        {
            CSWSItem* pItem = g_pAppManager->GetServerExoApp()
                ->GetItemByGameObjectID(pEntry->m_oidItem);
            CExoString sDescription;
            sDescription = pItem->GetPropertyDescription();
            int nTag = sDescription.Find('<', 0);
            int nToken = sDescription.Find('{', 0);
            if (nTag != -1 || nToken != -1)
            {
                CSWCCreature* pPlayerCharacter =
                    g_pAppManager->GetClientExoApp()->GetSWParty()->GetPlayerCharacter();
                g_pSWTlkTable->ParseStr(sDescription, pPlayerCharacter, 0, 1);
            }
            if (sDescription != "")
                SetDescription(sDescription);
            else
                SetDescription(g_pAppManager->GetClientExoApp()->GetGUIString(STRREF_NO_DESCRIPTION));
        }
        else
            SetDescription("");
    }
}
// PC 0x0072D060
void CSWGuiUpgrade::VerifyUpgrade(CSWGuiControl* pControl)
{
    int nSlot = m_pActiveControl->m_nIndex;

    if (g_pAppManager->m_pClientExoApp->GetInGameGui()
            ->m_pMessageBox->m_bAccepted != 0)
    {
        InsertUpgrade((CSWSItem*)pControl, nSlot);
        UpdateItems();
        OnEnterSlot(m_pActiveControl);
    }
    else
    {
        ((CSWSItem*)pControl)->m_nUpgrades &=
            ~(1 << (unsigned char)m_anUpgradeProperties[nSlot]);
    }
}
// PC 0x0072D0D0
void CSWGuiUpgrade::OnAssemble(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        if (pControl->m_nEvent == 45)
        {
            m_pManager->PlayGuiSound((char)pControl->m_nClickSound);
        }
        m_pItemSelect->FinishUpgrading();
        m_bDeferredRemoval = 0;
        if (m_nExitState != EXITSTATE_DELETE)
        {
            m_nExitState = EXITSTATE_REMOVE;
        }
        m_pManager->PopModalPanel();
    }
}
// PC 0x0072D130
void CSWGuiUpgrade::CancelPanel()
{

    CSWSItem* pItem;
    CItemRepository* pPartyItems;
    int i;

    delete m_pSelectedItem;
    m_pSelectedItem = m_pSelectedItemCopy;
    m_pSelectedItemCopy = 0;
    pPartyItems = g_pAppManager->m_pServerExoApp->GetPartyTable()->GetPartyItems();
    for (i = 0; i < m_aItemsAdded.num; i++)
    {
        pItem = m_aItemsAdded[i];
        for (int j = 0; j < NUM_UPGRADE_SLOTS; j++)
        {
            if (pItem == m_apItemEntries[j])
            {
                m_apItemEntries[j] = 0;
                break;
            }
        }
        pPartyItems->AddItem(&pItem, 1, 0, 0);
    }
    for (i = 0; i < m_aItemsTaken.num; i++)
    {
        pItem = m_aItemsTaken[i];
        if (pItem->m_nStackSize > 1)
        {
            pItem->m_nStackSize--;
        }
        else
        {
            pPartyItems->RemoveItem(pItem);
            delete pItem;
        }
    }
    m_pItemSelect->FinishUpgrading();
    m_bDeferredRemoval = 0;
    if (m_nExitState != EXITSTATE_DELETE)
    {
        m_nExitState = EXITSTATE_REMOVE;
    }
    m_pManager->PopModalPanel();
}
// PC 0x0072E230
void CSWGuiUpgradeItemSelect::OnPanelAdded()
{
    if (m_nUpgradeType > 0 && m_nUpgradeType <= CSWGuiUpgrade::ITEMLIST_ARMOR)
    {
        if (m_pPresetItem != 0)
        {
            CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
            unsigned long oidPossessor = m_pPresetItem->m_oidPossessor;
            CSWSCreature* pCreature = pServer->GetCreatureByGameObjectID(oidPossessor);
            int nSlot = pCreature->m_pInventory->GetSlotFromItem(m_pPresetItem);
            m_pUpgradeItem = new CSWUpgradeItemEntry;
            if (nSlot > 0)
            {
                m_pUpgradeItem->SetItem(m_pPresetItem->m_nId, 1, pCreature->m_nId, nSlot);
                unsigned long nEquipSlot = m_pUpgradeItem->m_nItemProperty;
                if (nEquipSlot != INVENTORY_SLOT_RIGHTHAND && nEquipSlot != INVENTORY_SLOT_LEFTHAND)
                {
                    if (nEquipSlot != INVENTORY_SLOT_RIGHTHAND2
                        && nEquipSlot != INVENTORY_SLOT_LEFTHAND2)
                    {
                        m_bDualWield = 0;
                        m_bLeftHandItem = 0;
                        m_bSecondWeaponSet = 0;
                        pCreature->UnequipItem(m_pPresetItem);
                    }
                    else
                    {
                        CSWSItem* pRight =
                            pCreature->m_pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND2);
                        CSWSItem* pLeft =
                            pCreature->m_pInventory->GetItemInSlot(INVENTORY_SLOT_LEFTHAND2);
                        if (pLeft == 0)
                        {
                            m_bDualWield = 0;
                            m_bLeftHandItem = 0;
                            m_bSecondWeaponSet = 1;
                            m_oidOtherWeapon = OBJECT_INVALID;
                            pCreature->UnequipItem(pRight);
                        }
                        else if (pRight == m_pPresetItem)
                        {
                            m_bDualWield = 1;
                            m_bLeftHandItem = 0;
                            m_bSecondWeaponSet = 1;
                            m_oidOtherWeapon = pLeft->m_nId;
                            pCreature->UnequipItem(pLeft);
                            pCreature->UnequipItem(pRight);
                        }
                        else if (pLeft == m_pPresetItem)
                        {
                            m_bDualWield = 1;
                            m_bLeftHandItem = 1;
                            m_bSecondWeaponSet = 1;
                            m_oidOtherWeapon = pRight->m_nId;
                            pCreature->UnequipItem(pLeft);
                            pCreature->UnequipItem(pRight);
                        }
                    }
                }
                else
                {
                    CSWSItem* pRight =
                        pCreature->m_pInventory->GetItemInSlot(INVENTORY_SLOT_RIGHTHAND);
                    CSWSItem* pLeft =
                        pCreature->m_pInventory->GetItemInSlot(INVENTORY_SLOT_LEFTHAND);
                    if (pLeft == 0)
                    {
                        m_bDualWield = 0;
                        m_bLeftHandItem = 0;
                        m_bSecondWeaponSet = 0;
                        m_oidOtherWeapon = OBJECT_INVALID;
                        pCreature->UnequipItem(pRight);
                    }
                    else if (pRight == m_pPresetItem)
                    {
                        m_bDualWield = 1;
                        m_bLeftHandItem = 0;
                        m_bSecondWeaponSet = 0;
                        m_oidOtherWeapon = pLeft->m_nId;
                        pCreature->UnequipItem(pLeft);
                        pCreature->UnequipItem(pRight);
                    }
                    else if (pLeft == m_pPresetItem)
                    {
                        m_bDualWield = 1;
                        m_bLeftHandItem = 1;
                        m_bSecondWeaponSet = 0;
                        m_oidOtherWeapon = pRight->m_nId;
                        pCreature->UnequipItem(pLeft);
                        pCreature->UnequipItem(pRight);
                    }
                }
            }
            else
            {
                if (m_pPresetItem->m_nStackSize > 1)
                {
                    m_pPresetItem = m_pPresetItem->SplitItem(1);
                }
                else
                {
                    pServer->GetPartyTable()->GetPartyItems()->RemoveItem(m_pPresetItem);
                }
            }
            m_pUpgradePanel->m_nItemListMode = m_nUpgradeType;
            m_pUpgradePanel->m_pSelectedItem = m_pPresetItem;
            m_pManager->AddPanel(m_pUpgradePanel, CSWGuiManager::PANEL_MODE_MODAL, 1);
        }
        else
        {
            BuildUpgradeList();
            m_lbItems.SetSelectedControl(0, 0);
            SetActiveControl(&m_lbItems, 0);
#ifndef _XBOX
            CSWGuiPanel::OnPanelAdded();
#endif
        }
    }
    else
    {
        SetModal(0);
    }
}
// PC 0x0072E550
void CSWGuiUpgrade::OnSlotSelected(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        if (pControl->GetEvent() == GUIEVENT_ACCEPT)
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);

        int nSlot = (int)pControl->GetUserData();
        CItemRepository* pRepository = g_pAppManager->GetServerExoApp()
            ->GetPartyTable()->GetPartyItems();
        if (m_nItemListMode == ITEMLIST_LIGHTSABER)
        {
            CExoArrayList<CSWGuiControl*> aControls;
            int nSlotUpgradeType;
            if (nSlot == UPGRADE_SLOT_POWER_CRYSTAL_1 || nSlot == UPGRADE_SLOT_POWER_CRYSTAL_2)
                nSlotUpgradeType = UPGRADE_TABLE_TYPE_POWER_CRYSTAL;
            else
                nSlotUpgradeType = nSlot - UPGRADE_SLOT_LS_LENS + UPGRADE_TABLE_TYPE_LS_LENS;
            if (nSlot == UPGRADE_SLOT_COLOR_CRYSTAL)
            {
                CExoString sTemplate;
                aControls.Add(CreateItemEntry(m_apItemEntries[UPGRADE_SLOT_COLOR_CRYSTAL]->GetId(), -1, 0));
                for (int i = 0; i < m_pCrystalTable->GetNumRows(); i++)
                {
                    m_pCrystalTable->GetCExoStringEntry(i, "Template", &sTemplate);
                    sTemplate = sTemplate.LowerCase();
                    if (sTemplate != m_apItemEntries[UPGRADE_SLOT_COLOR_CRYSTAL]->m_sTag)
                    {
                        unsigned long oidItem = pRepository->FindItemWithTag(sTemplate);
                        if (oidItem != OBJECT_INVALID)
                            aControls.Add(CreateItemEntry(oidItem, i, 0));
                    }
                }
            }
            else if (nSlot < UPGRADE_SLOT_LS_LENS)
            {
                CExoString sTemplate;
                int nOtherSlot = nSlot == UPGRADE_SLOT_POWER_CRYSTAL_1
                    ? UPGRADE_SLOT_POWER_CRYSTAL_2 : UPGRADE_SLOT_POWER_CRYSTAL_1;
                aControls.Add(CreateItemEntry(OBJECT_INVALID, -1, 0));
                if (m_apItemEntries[nSlot] != 0)
                    aControls.Add(CreateItemEntry(m_apItemEntries[nSlot]->GetId(), -1, 0));
                for (int i = 0; i < m_pUpgradeTable->GetNumRows(); i++)
                {
                    int nUpgradeType;
                    m_pUpgradeTable->GetINTEntry(i, "UpgradeType", &nUpgradeType);
                    if (nUpgradeType == UPGRADE_TABLE_TYPE_POWER_CRYSTAL)
                    {
                        m_pUpgradeTable->GetCExoStringEntry(i, "Template", &sTemplate);
                        unsigned long oidItem = pRepository->FindItemWithTag(sTemplate.LowerCase());
                        if (oidItem != OBJECT_INVALID &&
                            (m_apItemEntries[nOtherSlot] == 0 || i != m_anUpgradeProperties[nOtherSlot]) &&
                            (m_apItemEntries[nSlot] == 0 || i != m_anUpgradeProperties[nSlot]))
                            aControls.Add(CreateItemEntry(oidItem, i, 0));
                    }
                }
            }
            else
            {
                CExoString sTemplate;
                int nOtherSlot = nSlot == UPGRADE_SLOT_POWER_CRYSTAL_1
                    ? UPGRADE_SLOT_POWER_CRYSTAL_2 : UPGRADE_SLOT_POWER_CRYSTAL_1;
                aControls.Add(CreateItemEntry(OBJECT_INVALID, -1, 0));
                if (m_apItemEntries[nSlot] != 0)
                    aControls.Add(CreateItemEntry(m_apItemEntries[nSlot]->GetId(), -1, 0));
                for (int i = 0; i < m_pUpgradeTable->GetNumRows(); i++)
                {
                    int nUpgradeType;
                    m_pUpgradeTable->GetINTEntry(i, "UpgradeType", &nUpgradeType);
                    if (nUpgradeType == nSlotUpgradeType)
                    {
                        m_pUpgradeTable->GetCExoStringEntry(i, "Template", &sTemplate);
                        unsigned long oidItem = pRepository->FindItemWithTag(sTemplate.LowerCase());
                        if (oidItem != OBJECT_INVALID &&
                            (m_apItemEntries[nOtherSlot] == 0 || i != m_anUpgradeProperties[nOtherSlot]) &&
                            (m_apItemEntries[nSlot] == 0 || i != m_anUpgradeProperties[nSlot]))
                            aControls.Add(CreateItemEntry(oidItem, i, 0));
                    }
                }
            }
            m_lbItems.AddControls(aControls, 1, 0, 0);
            ShowItems(1);
            SetActiveControl(&m_lbItems, 0);
            m_bItemListShown = 1;
            m_pItemListControl = pControl;
        }
        else if (m_nItemListMode == ITEMLIST_BLASTER || m_nItemListMode == ITEMLIST_MELEE ||
                 m_nItemListMode == ITEMLIST_ARMOR)
        {
            CExoArrayList<CSWGuiControl*> aControls;
            int nSlotUpgradeType = 0;
            if (m_nItemListMode == ITEMLIST_BLASTER)
                nSlotUpgradeType = nSlot + UPGRADE_TABLE_TYPE_TARGETING;
            else if (m_nItemListMode == ITEMLIST_MELEE)
                nSlotUpgradeType = nSlot + UPGRADE_TABLE_TYPE_MELEE_GRIP;
            else if (m_nItemListMode == ITEMLIST_ARMOR)
                nSlotUpgradeType = nSlot + UPGRADE_TABLE_TYPE_ARMOR_OVERLAY;
            {
                CExoString sTemplate;
                int nOtherSlot = nSlot == UPGRADE_SLOT_POWER_CRYSTAL_1
                    ? UPGRADE_SLOT_POWER_CRYSTAL_2 : UPGRADE_SLOT_POWER_CRYSTAL_1;
                aControls.Add(CreateItemEntry(OBJECT_INVALID, -1, 0));
                if (m_apItemEntries[nSlot] != 0)
                    aControls.Add(CreateItemEntry(m_apItemEntries[nSlot]->GetId(), -1, 0));
                for (int i = 0; i < m_pUpgradeTable->GetNumRows(); i++)
                {
                    int nUpgradeType;
                    m_pUpgradeTable->GetINTEntry(i, "UpgradeType", &nUpgradeType);
                    if (nUpgradeType == nSlotUpgradeType)
                    {
                        m_pUpgradeTable->GetCExoStringEntry(i, "Template", &sTemplate);
                        unsigned long oidItem = pRepository->FindItemWithTag(sTemplate.LowerCase());
                        if (oidItem != OBJECT_INVALID &&
                            (m_apItemEntries[nOtherSlot] == 0 || i != m_anUpgradeProperties[nOtherSlot]) &&
                            (m_apItemEntries[nSlot] == 0 || i != m_anUpgradeProperties[nSlot]))
                        {
                            CSWSItem* pItem = g_pAppManager->m_pServerExoApp->GetItemByGameObjectID(oidItem);
                            if (pItem->m_nUpgradeLevel <= m_pSelectedItem->m_nUpgradeLevel)
                                aControls.Add(CreateItemEntry(oidItem, i, 0));
                            else
                                aControls.Add(CreateItemEntry(oidItem, i, 1));
                        }
                    }
                }
            }
            m_lbItems.AddControls(aControls, 1, 0, 0);
            ShowItems(1);
            SetActiveControl(&m_lbItems, 0);
            m_bItemListShown = 1;
            m_pItemListControl = pControl;
        }
        else
        {
            CSWSItem*& pOldItem = m_apItemEntries[nSlot];
            if (pOldItem != 0)
            {
#ifdef _XBOX
                m_pSelectedItem->m_nUpgrades &=
                    ~(CSWSItem::UPGRADE_PROPERTY_MASK << (unsigned char)m_anUpgradeProperties[nSlot]);
#else
                m_pSelectedItem->ClearUpgradeActive((unsigned char)m_anUpgradeProperties[nSlot]);
#endif
                int nIndex = m_aItemsAdded.IndexOf(pOldItem);
                if (nIndex != -1)
                {
                    m_aItemsAdded.DelIndex(nIndex);
                    pRepository->AddItem(&pOldItem, 1, 0, 0);
                }
                else
                {
                    pRepository->AddItem(&pOldItem, 1, 0, 0);
                    m_aItemsTaken.Add(pOldItem);
                }
                pOldItem = 0;
            }
            else
            {
                unsigned long oidItem = pRepository->FindItemWithTag(m_asUpgradeTemplates[nSlot]);
                CSWSItem* pItem = g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(oidItem);
                if (pItem == 0 ||
                    (unsigned long)pItem->m_nUpgradeLevel >= (unsigned long)m_pSelectedItem->m_nUpgradeLevel)
                    return;
                CSWSCreature* pCreature = 0;
                unsigned long nEquipSlot = 0;
                if (m_pItemSelect->GetUpgradeItemEquipped())
                {
                    nEquipSlot = m_pItemSelect->GetUpgradeItemProperty();
                    pCreature = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(
                        m_pItemSelect->GetUpgradeItemId());
                }
                m_pSelectedItem->AddUpgrade(pItem, 0);
                if (pCreature != 0 && !pCreature->CanEquipItem(m_pSelectedItem, &nEquipSlot, 0, 0, 0))
                {
                    CSWGuiMessageBox* pMessageBox = g_pAppManager->GetClientExoApp()
                        ->GetInGameGui()->m_pMessageBox;
                    pMessageBox->SetAllowCancel(1);
                    pMessageBox->SetCallback(this, (CSWGuiHandler)&CSWGuiUpgrade::VerifyUpgrade,
                                             (CSWGuiControl*)pItem);
                    pMessageBox->SetMessage(STRREF_UPGRADE_PREVENTS_REEQUIP);
                    m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
                    return;
                }
                InsertUpgrade(pItem, nSlot);
            }
            UpdateItems();
            OnEnterSlot(pControl);
        }
    }
}
// PC 0x0072EE80
void CSWGuiUpgrade::HandleInputEvent(int nEvent, int nState)
{
    if (nState != 0)
    {
        switch (nEvent)
        {
        case GUIEVENT_CANCEL:
        case GUIEVENT_CANCEL_ALTERNATE:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            if (m_bItemListShown)
            {
                CloseItems();
            }
            else
            {
                CancelPanel();
            }
            break;
        case GUIEVENT_SCROLL_DOWN:
            m_lbDesc.HandleInputEvent(GUIEVENT_DOWN, 1);
            break;
        case GUIEVENT_SCROLL_UP:
            m_lbDesc.HandleInputEvent(GUIEVENT_UP, 1);
            break;
        case GUIEVENT_TOGGLE_MODE:
            OnAssemble(0);
            break;
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
// PC 0x00724670
void CSWGuiUpgrade::SetDescription(const CExoString& sDescription)
{
    CSWGuiListBox* pListBox;
    int nHeight;
    CSWGuiControl* pControl;

    m_CSWGuiLabel_1F60.m_cText.m_cParams.SetText(sDescription);
    pListBox = &m_lbDesc;
#ifdef _XBOX
    CSWGuiLabel* pRanalog = &m_lblRanalog;
#endif
    pListBox->ClearItems();
    CSWGuiExtent cExtent = m_CSWGuiLabel_1F60.m_cExtent;
    if (sDescription.GetLength() > 0)
    {
        nHeight = m_CSWGuiLabel_1F60.m_cText.GetIdealHeight();
    }
    else
    {
        nHeight = m_CSWGuiLabel_1F60.m_cText.GetFontHeight();
    }
    cExtent.m_nHeight = nHeight;
    m_CSWGuiLabel_1F60.SetExtent(cExtent);
    pControl = &m_CSWGuiLabel_1F60;
    pListBox->AddControls(&pControl, 1, 0, 0, 0);
    pListBox->SetSelectedControl(&m_CSWGuiLabel_1F60, 0);
#ifdef _XBOX
    pRanalog->SetEnabledFlag(pListBox->IsOversized());
#endif
}
// PC 0x00724730
void CSWGuiUpgrade::Draw(float fElapsed)
{
    if (g_bDo3DGUIs != 0)
    {
        Gob* pModel;
        if (m_nItemListMode == ITEMLIST_LIGHTSABER)
        {
            pModel = (Gob*)m_3dModelLs.m_cScene.GetModel(1);
        }
        else
        {
            pModel = (Gob*)m_3dModel.m_cScene.GetModel(1);
        }
        if (pModel != 0 && pModel->GetBehavior(-1) == 0)
        {
            pModel->SetOrientation(
                pModel->GetOrientation() *
                YawPitchRoll(fElapsed * -70.0f, 0.0f, 0.0f));
        }
    }
    CSWGuiPanel::Draw(fElapsed);
}
// PC 0x00724870
void CSWGuiUpgrade::GetArmorModelAndTextureResRef(
    int nAppearance, int nBodyVariation, int nTextureVar,
    CExoString& sModel, CExoString& sTexture)
{
    char sModelColumn[] = "ModelA";
    char sTextureColumn[] = "TEXA";
    int nBody = max(1, min(CCAppearanceInfo::BODY_VARIATION_N, nBodyVariation));
    int nTexture = max(1, nTextureVar);
    sModelColumn[5] = 'A' + nBody - 1;
    sTextureColumn[3] = 'A' + nBody - 1;

    bool bMissing;
    {
        CExoString sColumn(sModelColumn);
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pAppearance2DA = p2DArrays->m_pAppearance;
        bMissing = !pAppearance2DA->GetCExoStringEntry(
            nAppearance, sColumn, &sModel);
    }
    if (bMissing)
    {
        sModel = "";
    }
    int bFound;
    {
        CExoString sColumn(sTextureColumn);
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pAppearance2DA = p2DArrays->m_pAppearance;
        bFound = pAppearance2DA->GetCExoStringEntry(
            nAppearance, sColumn, &sTexture);
    }
    if (bFound)
    {
        sTexture.Format("%s%02d", sTexture.CStr(), nTexture);
        if (!g_pExoResMan->Exists(sTexture, RESTYPE_TGA, (unsigned long*)0) &&
#ifdef _XBOX
            !g_pExoResMan->Exists(sTexture, 3006, (unsigned long*)0))
#else
            !g_pExoResMan->Exists(sTexture, RESTYPE_TPC, (unsigned long*)0))
#endif
        {
            sTexture.Format("%s%02d", sModel.CStr(), 1);
        }
    }
    else
    {
        sTexture = "";
    }
}
// PC 0x00724A40
void CSWGuiUpgrade::Update3DView()
{
    if (g_bDo3DGUIs != 0)
    {
        CSWGui3DSceneView* pView = &m_3dModel;
        if (m_nItemListMode == ITEMLIST_LIGHTSABER)
            pView = &m_3dModelLs;
        CSWGuiScene* pScene = &pView->m_cScene;
        pScene->RemoveModel(1, 1);

        CExoString sModel;
        CExoString sTexture;
        CSWBaseItem* pBaseItem = m_pSelectedItem->GetBaseItem();
        if (m_nItemListMode == ITEMLIST_ARMOR)
        {
            unsigned char nTextureVar = m_pSelectedItem->m_nTextureVar;
            unsigned char nBodyVar = pBaseItem->m_nBodyVar;
            GetArmorModelAndTextureResRef(DISGUISE_TYPE_P_MAL_A_MED_01,
                nBodyVar, nTextureVar,
                sModel, sTexture);
        }
        else
        {
            unsigned char nModelVariation = m_pSelectedItem->m_nModelVariation;
            pBaseItem->GetModelResRef(nModelVariation, 'm').CopyToString(sModel);
        }
        CExoString sCameraHook;
        CExoString sRotateHook;
        sCameraHook.Format("camerahook%d", pBaseItem->m_nItemType);
        sRotateHook.Format("rotatehook%d", pBaseItem->m_nItemType);
        switch (pBaseItem->m_nItemType)
        {
        case CSWBaseItem::ITEM_TYPE_BLASTER_RIFLE:
            if (m_pSelectedItem->m_nBaseItem == BASE_ITEM_BLASTER_CARBINE)
            {
                sCameraHook = sCameraHook + "a";
                sRotateHook = sRotateHook + "a";
            }
            break;
        case CSWBaseItem::ITEM_TYPE_STUN_BATON:
            if (m_pSelectedItem->m_nBaseItem == BASE_ITEM_STUN_BATON)
            {
                sCameraHook = sCameraHook + "a";
                sRotateHook = sRotateHook + "a";
            }
            break;
        case ITEM_TYPE_ARMOR_HEAVY:
        case ITEM_TYPE_CLOTHING:
        case ITEM_TYPE_ROBE:
            sCameraHook = "camerahook31";
            sRotateHook = "rotatehook31";
            break;
        }

        CAurCamera* pCamera = pView->m_cScene.m_pCamera;
        Gob* pRoom = (Gob*)pScene->GetModel(0);
        if (pRoom->GetPart((char*)sCameraHook.CStr()) == 0)
            sCameraHook = "camerahook";
        pCamera->AttachToObject(pScene->GetModel(0), sCameraHook.CStr(), Gob::ATTACH_NORMAL);
        pCamera->SetViewAngle(22.726f);
        Gob* pModel = (Gob*)pScene->AddModel(sModel, -1);
        if (pModel != 0)
        {
            if (!sTexture.IsEmpty())
                pModel->ReplaceTexture("", (char*)sTexture.CStr());
            if (pRoom->GetPart((char*)sRotateHook.CStr()) != 0)
            {
                pModel->AttachToObject(pRoom, (char*)sRotateHook.CStr(), Gob::ATTACH_NORMAL);
                pRoom->PlayAnimation("rotate", 1.0f, 0, 0);
            }
            if (m_pSelectedItem->GetBaseItem()->m_bPoweredItem)
                pModel->PlayAnimation("powered", 1.0f, 0, 0);
            if (m_nItemListMode == ITEMLIST_ARMOR)
                pModel->PlayAnimation("neutral", 1.0f, 0, 0);
        }
    }
}
// PC 0x00724F20
void CSWGuiUpgrade::UpdateItems()
{
    CResRef cIcon;
    CExoString sDescription;
    CSWGuiLabel* pLabels;
    switch (m_nItemListMode)
    {
    case ITEMLIST_LIGHTSABER:
        pLabels = m_aUpgradeLabels3Ls;
        break;
    case ITEMLIST_BLASTER:
        pLabels = m_aUpgradeLabels3;
        break;
    case ITEMLIST_MELEE:
        pLabels = m_aUpgradeLabels3;
        break;
    case ITEMLIST_ARMOR:
        pLabels = m_aUpgradeLabels3;
        break;
    }
    unsigned char nSlots = 0;
    switch (m_nItemListMode)
    {
    case ITEMLIST_LIGHTSABER:
        nSlots = 6;
        break;
    case ITEMLIST_BLASTER:
    case ITEMLIST_MELEE:
        nSlots = 3;
        break;
    case ITEMLIST_ARMOR:
        nSlots = 2;
        break;
    }
    int nEndSlot = nSlots;
    for (int i = 0; i < nEndSlot; i++)
    {
        if (m_apItemEntries[i] != 0)
        {
            CSWBaseItem* pBaseItem = g_pRules->m_pBaseItems->GetBaseItem(
                m_apItemEntries[i]->GetBaseItemType());
            cIcon = m_apItemEntries[i]->GetIcon();
            pLabels[i].m_cBorder.m_cParams.m_fHilightAlpha = 1.0f;
        }
        else
        {
            cIcon = SLOT_INFO[m_nItemListMode - 1][i].pszIcon;
        }
        pLabels[i].m_cBorder.m_cParams.SetFillImage(cIcon, 0);
    }
    SetDescription(m_pSelectedItem->GetPropertyDescription());
}
// PC 0x007250B0
void CSWGuiUpgrade::OnDisabledSelected(CSWGuiControl* pControl)
{
    if (pControl != 0 && pControl->m_bSelected)
    {
        CSWGuiMessageBox* pMessageBox =
            g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
        pMessageBox->SetAllowCancel(0);
        pMessageBox->SetMessage(STRREF_UPGRADE_INCOMPATIBLE);
        pMessageBox->SetCallback(0, 0, 0);
        pMessageBox->SetMessageColors(CGuiInGame::COLOR_UPGRADE_BORDER,
                                      CGuiInGame::COLOR_NEW_HIGHLIGHT);
        g_pAppManager->m_pClientExoApp->GetSWGuiManager()->AddPanel(pMessageBox, 1, 1);
    }
}
// PC 0x00725160
void CSWGuiUpgrade::OnEnterSlot(CSWGuiControl* pControl)
{
    int nSelected = pControl->m_bSelected;
    if (nSelected != 0)
    {
        int nSlot = pControl->m_nIndex;
        unsigned long nStrRef = SLOT_INFO[m_nItemListMode - 1][nSlot].nStrRef;
        if (m_nItemListMode == ITEMLIST_LIGHTSABER)
        {
            if (m_apItemEntries[nSlot] != 0)
            {
                CExoString sName;
                CExoLocString sItemName = m_apItemEntries[nSlot]->m_sLocalizedName;
                sItemName.GetString(g_pAppManager->GetClientExoApp()->GetClientLanguage(),
                                    &sName, 0);
                int nTag = sName.Find('<', 0);
                int nToken = sName.Find('{', 0);
                if (nTag != -1 || nToken != -1)
                {
                    CSWCCreature* pPlayerCharacter =
                        g_pAppManager->GetClientExoApp()->GetSWParty()->GetPlayerCharacter();
                    g_pSWTlkTable->ParseStr(sName, pPlayerCharacter, 1, 1);
                }
                m_cSlotName.m_cText.m_cParams.SetText(sName);
                m_cProperty.m_cText.m_cParams.SetText(
                    m_apItemEntries[nSlot]->GetPropertiesOnlyDescription());
            }
            else
            {
                m_cSlotName.m_cText.m_cParams.SetStrRef(nStrRef);
                m_cProperty.m_cText.m_cParams.SetText("");
            }
        }
        else
        {
            CServerExoApp* pServer = g_pAppManager->GetServerExoApp();
            CSWSItem* pItem = pServer->GetItemByGameObjectID(
                pServer->GetPartyTable()->GetPartyItems()->FindItemWithTag(
                    m_asUpgradeTemplates[nSlot]));
            if (m_apItemEntries[nSlot] != 0)
            {
                CExoString sName;
                CExoLocString sItemName = m_apItemEntries[nSlot]->m_sLocalizedName;
                sItemName.GetString(g_pAppManager->GetClientExoApp()->GetClientLanguage(),
                                    &sName, 0);
                int nTag = sName.Find('<', 0);
                int nToken = sName.Find('{', 0);
                if (nTag != -1 || nToken != -1)
                {
                    CSWCCreature* pPlayerCharacter =
                        g_pAppManager->GetClientExoApp()->GetSWParty()->GetPlayerCharacter();
                    g_pSWTlkTable->ParseStr(sName, pPlayerCharacter, 1, 1);
                }
                m_cSlotName.m_cText.m_cParams.SetText(sName);
                m_cProperty.m_cText.m_cParams.SetText(
                    m_apItemEntries[nSlot]->GetPropertiesOnlyDescription());
            }
            else
            {
                CSWSItem* pSelectedItem = m_pSelectedItem;
                m_cProperty.m_cText.m_cParams.SetText(
                    pSelectedItem->GetKeyedPropertyString(m_anUpgradeProperties[nSlot]));
                m_cSlotName.m_cText.m_cParams.SetStrRef(nStrRef);
            }
        }
    }
}

#ifdef _XBOX
void CSWGuiUpgrade::OnEnterAssemble(CSWGuiControl* pControl)
{
    m_lblAAction.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
    m_lblAAction.m_cText.m_cParams.SetStrRef(STRREF_ASSEMBLE);
    m_cSlotName.m_cText.m_cParams.SetText("");
    m_cLightsaberSlotName.m_cText.m_cParams.SetText("");
    m_cUpgradeCount.m_cText.m_cParams.SetText("");
    m_cProperty.m_cText.m_cParams.SetText("");
    m_cUpgrades.m_cText.m_cParams.SetText("");
}

void CSWGuiUpgrade::ShowItems(int bShow)
{
    if (bShow)
    {
        m_3dModel.SetShown(0);
        m_3dModelLs.SetShown(0);
        m_cSlotName.SetShown(0);
        m_cLightsaberSlotName.SetShown(0);
        m_cUpgradeCount.SetShown(0);
        m_cUpgrades.SetShown(0);
        m_cProperty.SetShown(0);
        m_cAssemble.SetShown(0);
        for (int i = 0; i < 4; i++)
        {
            m_aUpgradeButtons4[i].SetShown(0);
            m_aUpgradeLabels4[i].SetShown(0);
            if (i < 3)
            {
                m_aUpgradeButtons3[i].SetShown(0);
                m_aUpgradeLabels3[i].SetShown(0);
            }
        }
        m_lbItems.SetShown(1);
        return;
    }

    if (m_nItemListMode == ITEMLIST_LIGHTSABER)
    {
        m_3dModel.SetShown(0);
        m_cSlotName.SetShown(0);
        m_cUpgradeCount.SetShown(0);
        m_cUpgrades.SetShown(0);
        m_cProperty.SetShown(0);
        m_cDescriptionBackground.SetShown(0);
        m_lbDesc.SetShown(0);
        m_lblRanalog.SetShown(0);
        m_3dModelLs.SetShown(1);
        m_cLightsaberSlotName.SetShown(1);
        m_cAssemble.SetShown(1);
        m_cLightsaberDescriptionBackground.SetShown(1);
        m_lbDescLs.SetShown(1);
        m_lblRanalogLs.SetShown(1);
        m_cAssemble.SetMoveToControl(0, &m_aUpgradeButtons3[1]);
        m_cAssemble.SetMoveToControl(2, &m_aUpgradeButtons3[1]);
        for (int i = 0; i < 4; i++)
        {
            m_aUpgradeButtons4[i].SetShown(0);
            m_aUpgradeLabels4[i].SetShown(0);
            if (i < 3)
            {
                m_aUpgradeButtons3[i].SetShown(1);
                m_aUpgradeLabels3[i].SetShown(1);
            }
        }
    }
    else
    {
        int bShowThreeSlots = 0;
        m_3dModel.SetShown(1);
        m_cSlotName.SetShown(1);
        m_cUpgradeCount.SetShown(1);
        m_cUpgrades.SetShown(1);
        m_cProperty.SetShown(1);
        m_cAssemble.SetShown(1);
        m_cDescriptionBackground.SetShown(1);
        m_lbDesc.SetShown(1);
        m_lblRanalog.SetShown(1);
        m_3dModelLs.SetShown(0);
        m_cLightsaberSlotName.SetShown(0);
        m_cLightsaberDescriptionBackground.SetShown(0);
        m_lbDescLs.SetShown(0);
        m_lblRanalogLs.SetShown(0);
        switch (m_nItemListMode)
        {
        case ITEMLIST_BLASTER:
            for (int i = 0; i < 4; i++)
            {
                m_aUpgradeButtons4[i].SetShown(1);
                m_aUpgradeLabels4[i].SetShown(1);
            }
            m_cAssemble.SetMoveToControl(0, &m_aUpgradeButtons4[1]);
            m_cAssemble.SetMoveToControl(2, &m_aUpgradeButtons4[1]);
            break;
        case ITEMLIST_ARMOR:
            m_aUpgradeButtons4[0].SetShown(0);
            m_aUpgradeLabels4[0].SetShown(0);
            m_aUpgradeButtons4[1].SetShown(1);
            m_aUpgradeLabels4[1].SetShown(1);
            m_aUpgradeButtons4[2].SetShown(1);
            m_aUpgradeLabels4[2].SetShown(1);
            m_aUpgradeButtons4[3].SetShown(0);
            m_aUpgradeLabels4[3].SetShown(0);
            m_cAssemble.SetMoveToControl(0, &m_aUpgradeButtons4[1]);
            m_cAssemble.SetMoveToControl(2, &m_aUpgradeButtons4[1]);
            break;
        case ITEMLIST_MELEE:
            for (int i = 0; i < 4; i++)
            {
                m_aUpgradeButtons4[i].SetShown(0);
                m_aUpgradeLabels4[i].SetShown(0);
            }
            m_cAssemble.SetMoveToControl(0, &m_aUpgradeButtons3[1]);
            m_cAssemble.SetMoveToControl(2, &m_aUpgradeButtons3[1]);
            bShowThreeSlots = 1;
            break;
        }
        for (int i = 0; i < 3; i++)
        {
            m_aUpgradeButtons3[i].SetShown(bShowThreeSlots);
            m_aUpgradeLabels3[i].SetShown(bShowThreeSlots);
        }
    }
    m_lbItems.SetShown(0);
}
#endif
// PC 0x00725F90
void CSWGuiCreateItem::PopulateStore()
{
    CServerExoApp* pServer;
    CSWSCreature* pCreature;
    int nComponents;

    pServer = g_pAppManager->m_pServerExoApp;
    pCreature = pServer->GetCreatureByGameObjectID(m_nCreatureId);
    if (pCreature != 0)
    {
        nComponents = pCreature->GetComponent();
        m_nComponents = nComponents;
        m_lblCreditsValue.m_cText.m_cParams.SetText(CExoString(nComponents));
    }
}
// PC 0x00726180
void CSWGuiCreateItem::CloseCreateItem(CSWGuiControl* pControl)
{
    if (m_pUpgradeSelection != 0 && m_pUpgradeSelection->m_bDisableUpgrade != 0)
    {
        m_pUpgradeSelection->HandleInputEvent(GUIEVENT_CANCEL, 1);
    }
    else
    {
        m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
    }
    SetModal(0);
    m_pManager->PopModalPanel();
}
// PC 0x007261E0
void CSWGuiCreateItem::UpdateBreakdownFilters()
{
    for (int i = 0; i < 5; i++)
    {
        if (m_abtnBreakdownFilters[i].m_nIndex == m_nBreakdownFilter)
            m_abtnBreakdownFilters[i].m_cText.m_cParams.SetDefaultColor(
                CGuiInGame::COLOR_NEW_HIGHLIGHT);
        else
            m_abtnBreakdownFilters[i].m_cText.m_cParams.SetDefaultColor(
                CGuiInGame::COLOR_UPGRADE_TEXT);
    }
}
// PC 0x00726220
void CSWGuiCreateItem::UpdateCreateFilters()
{
    for (int i = 0; i < 5; i++)
    {
        if (m_abtnCreateFilters[i].m_nIndex == m_nCreateFilter)
            m_abtnCreateFilters[i].m_cText.m_cParams.SetDefaultColor(
                CGuiInGame::COLOR_NEW_HIGHLIGHT);
        else
            m_abtnCreateFilters[i].m_cText.m_cParams.SetDefaultColor(
                CGuiInGame::COLOR_UPGRADE_TEXT);
    }
}
// PC 0x00726260
unsigned long CSWGuiCreateItem::GetItemBuyValue(CSWSItem* pItem)
{
    if (pItem == 0)
        return 0;
    return max(1u, pItem->GetCost() / COMPONENT_VALUE_DIVISOR);
}
// PC 0x00728220
void CSWGuiCreateItem::BuyItemCallback(CSWGuiControl* pControl)
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
// PC 0x00729E20
void CSWGuiCreateItem::OnBreakdownFilterPicked(CSWGuiControl* pControl)
{
    if (m_bBuyMode == 0 && pControl != 0)
    {
        m_nBreakdownFilter = pControl->m_nIndex;
        UpdateBreakdownFilters();
        PopulateInventoryItemListBox();
    }
}
// PC 0x0072AC50
void CSWGuiCreateItem::SellItemCallback(CSWGuiControl* pControl)
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
// PC 0x0072D5C0
void CSWGuiCreateItem::DeleteItemEntries()
{
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
// PC 0x0072EF70
void CSWGuiCreateItem::OnAcceptPicked(CSWGuiControl* pControl)
{
    if (m_bBuyMode != 0)
    {
        CSWGuiControl* pSelected = m_lbxStoreItems.GetSelectedControl();
        if (pSelected != 0)
            OnControlStoreAButton(pSelected);
    }
    else
    {
        CSWGuiControl* pSelected = m_lbxInventoryItems.GetSelectedControl();
        if (pSelected != 0)
            OnControlInvAButton(pSelected);
    }
}
// PC 0x0072EFC0
CSWGuiCreateItem::~CSWGuiCreateItem()
{
    delete m_pStore;
    DeleteItemEntries();
}
// PC 0x0072F250
void CSWGuiCreateItem::OnPanelRemoved()
{
    DeleteItemEntries();
    CItemRepository* pRepository = m_pStore->m_pItemRepository;
    for (int i = 0; i < pRepository->GetNumItems(); i++)
    {
        CSWSItem* pItem = pRepository->ItemListGetItem(i);
        if (pItem != 0)
            delete pItem;
    }
    if (m_pUpgradeSelection != 0)
        m_pUpgradeSelection->PopulateItemList();
    CSWGuiPanel::OnPanelRemoved();
}
// PC 0x00732400
void CSWGuiCreateItem::OnCreateFilterPicked(CSWGuiControl* pControl)
{
    if (m_bBuyMode != 0 && pControl != 0)
    {
        m_nCreateFilter = pControl->m_nIndex;
        UpdateCreateFilters();
        SetItemList();
    }
}
// PC 0x00732430
void CSWGuiCreateItem::OnControlXButton(CSWGuiControl* pControl)
{
    m_n3ED8 = !m_n3ED8;
    m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
    if (m_lbxStoreItems.m_bEnabled)
        ShowSellGUI();
    else
        ShowBuyGUI();
}
// PC 0x00732470
void CSWGuiCreateItem::InitializeStoreType()
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
            m_lbl834.SetEnabledFlag(0);
            m_lbl6EC.SetEnabledFlag(0);
            ShowBuyGUI();
            return;
        case CSWSStore::BuySellFlagSell:
            m_lbl834.SetEnabledFlag(0);
            m_lbl6EC.SetEnabledFlag(0);
            ShowSellGUI();
            return;
        case CSWSStore::BuySellFlagBoth:
            m_lbl834.SetEnabledFlag(1);
            m_lbl6EC.SetEnabledFlag(1);
            ShowBuyGUI();
            return;
        }
    }
}
// PC 0x00732880
CSWGuiCreateItem::CSWGuiCreateItem(CSWGuiManager* pManager,
                                   unsigned long oidCreature)
    : CSWGuiPanel(pManager)
{
    m_n3EBC = 0;

    StartLoadFromLayout("component_p");
    InitControl(m_lbxStoreItems, "LB_SHOPITEMS", 1);
    InitControl(m_lbxInventoryItems, "LB_INVITEMS", 1);
    InitControl(m_lblCredits, "LBL_CREDITS", 1);
    InitControl(m_lblCreditsValue, "LBL_CREDITS_VALUE", 1);
    InitControl(m_btnAccept, "BTN_Accept", 1);
    InitControl(m_btnCancel, "BTN_Cancel", 1);
    InitControl(m_btnExamine, "BTN_Examine", 1);
    m_btnCancel.AddEvent(CSWGuiControl::EVENT_SELECT, this,
                         (CSWGuiHandler)&CSWGuiCreateItem::CloseCreateItem);
    m_btnExamine.AddEvent(CSWGuiControl::EVENT_SELECT, this,
                          (CSWGuiHandler)&CSWGuiCreateItem::OnControlXButton);
    InitControl(m_abtnCreateFilters[0], "BTN_CREATE_ARMOR", 1);
    InitControl(m_abtnCreateFilters[1], "BTN_CREATE_RANGED", 1);
    InitControl(m_abtnCreateFilters[2], "BTN_CREATE_MELEE", 1);
    InitControl(m_abtnCreateFilters[3], "BTN_CREATE_LIGHTSABER", 1);
    InitControl(m_abtnCreateFilters[4], "BTN_CREATE_MISC", 1);
    InitControl(m_abtnBreakdownFilters[0], "BTN_BREAK_ALL", 1);
    InitControl(m_abtnBreakdownFilters[1], "BTN_BREAK_WEAPONS", 1);
    InitControl(m_abtnBreakdownFilters[2], "BTN_BREAK_ARMOR", 1);
    InitControl(m_abtnBreakdownFilters[3], "BTN_BREAK_USEABLE", 1);
    InitControl(m_abtnBreakdownFilters[4], "BTN_BREAK_MISC", 1);
    InitControl(m_alblBars[0], "LBL_BAR1", 1);
    InitControl(m_alblBars[1], "LBL_BAR2", 1);
    InitControl(m_alblBars[2], "LBL_BAR3", 1);
    InitControl(m_lblTitle, "LBL_TITLE", 1);
    InitControl(m_lblTitle2, "LBL_TITLE2", 1);
    InitControl(m_lblRelatedSkill, "LBL_RELATEDSKILL", 1);
    InitControl(m_lbxDescriptionInv, "LB_DESCRIPTIONINV", 1);
    InitControl(m_lbxDescription, "LB_DESCRIPTION", 1);
    m_lbxDescription.m_nSelectSound = CSWGuiManager::GUISOUND_SELECTION;
    InitControl(m_lblCost, "LBL_COST", 1);
    InitControl(m_lblCostValue, "LBL_COST_VALUE", 1);
    InitControl(m_lblStock, "LBL_STOCK", 1);
    InitControl(m_lblStockValue, "LBL_STOCK_VALUE", 1);
    InitControl(m_lblRanalog, "LBL_RANALOG", 1);
    StopLoadFromLayout();

    CSWGuiListBox* pStoreItems = &m_lbxStoreItems;
    pStoreItems->SetSelectOnLeftClick(1);
    CSWGuiListBox* pInventoryItems = &m_lbxInventoryItems;
    pInventoryItems->SetSelectOnLeftClick(1);
    CSWGuiLabel* pProtoLabel = (CSWGuiLabel*)m_lbxDescription.m_pProtoItem;
    m_lblDescription.Initialize(pProtoLabel->m_cExtent, pProtoLabel, 1.0f);
    pStoreItems->m_bShowScrollBar = 1;
    pInventoryItems->m_bShowScrollBar = 1;
    SetActiveControl(pStoreItems, 0);
    pInventoryItems->m_bEnabled = 0;

    m_n68 = 0;
    m_n3EC8 = 0;
    m_n3EC0 = 0;
    m_nComponents = 0;
    m_pStore = new CSWSStore(OBJECT_INVALID);
    m_nCreateFilter = 0;

    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    CSWSCreature* pCreature;
    if (oidCreature == OBJECT_INVALID)
    {
        unsigned long oidPC =
            pServer->GetPlayerList()->GetHead()->AsSWSPlayer()->m_oidPCObject;
        pCreature = pServer->GetCreatureByGameObjectID(oidPC);
    }
    else
        pCreature = pServer->GetCreatureByGameObjectID(oidCreature);
    m_nCreatureId = pCreature->m_nId;
    float fFactor =
        pCreature->GetStats()->GetSkillRank(CSWRules::SKILL_REPAIR, 0, 0) * 0.05f;
    if (fFactor >= 1.0f)
        fFactor = 1.0f;
    m_fRepairFactor = fFactor;
    m_n3ED8 = 0;
    m_nBackground = 1;

    for (int i = 0; i < 5; i++)
    {
        m_abtnCreateFilters[i].AddEvent(
            CSWGuiControl::EVENT_SELECT, this,
            (CSWGuiHandler)&CSWGuiCreateItem::OnCreateFilterPicked);
        m_abtnCreateFilters[i].AddEvent(
            CSWGuiControl::EVENT_CONFIRM, this,
            (CSWGuiHandler)&CSWGuiCreateItem::OnCreateFilterPicked);
        m_abtnCreateFilters[i].SetFocusable(0);
        m_abtnCreateFilters[i].m_nIndex = i;
    }
    UpdateCreateFilters();
    for (int i = 0; i < 5; i++)
    {
        m_abtnBreakdownFilters[i].AddEvent(
            CSWGuiControl::EVENT_SELECT, this,
            (CSWGuiHandler)&CSWGuiCreateItem::OnBreakdownFilterPicked);
        m_abtnBreakdownFilters[i].AddEvent(
            CSWGuiControl::EVENT_CONFIRM, this,
            (CSWGuiHandler)&CSWGuiCreateItem::OnBreakdownFilterPicked);
        m_abtnBreakdownFilters[i].SetFocusable(0);
        m_abtnBreakdownFilters[i].m_nIndex = i;
    }
    CSWGuiButton* pAccept = &m_btnAccept;
    pAccept->AddEvent(CSWGuiControl::EVENT_SELECT, this,
                      (CSWGuiHandler)&CSWGuiCreateItem::OnAcceptPicked);
    pAccept->m_bFocusable = 0;
    m_btnExamine.SetFocusable(0);
    m_btnCancel.SetFocusable(0);
    m_nBreakdownFilter = 0;
    UpdateBreakdownFilters();
}
// PC 0x007333E0
void CSWGuiCreateItem::OnPanelAdded()
{
    if (g_pAppManager->m_pServerExoApp->GetPartyTable()->m_nItemComponent < 0)
        g_pAppManager->m_pServerExoApp->GetPartyTable()->m_nItemComponent = 0;
    if (m_pUpgradeSelection != 0 && m_pUpgradeSelection->m_bDisableUpgrade != 0)
        m_btnCancel.m_cText.m_cParams.SetStrRef(1582);
    else
        m_btnCancel.m_cText.m_cParams.SetStrRef(219);
    m_nCreateFilter = 4;
    UpdateCreateFilters();
    InitializeStoreType();
    SetItemList();
    PopulateStore();
    CSWGuiPanel::OnPanelAdded();
    ShowBuyGUI();
}
// PC 0x00727FD0
void CSWGuiCreateItem::HandleInputEvent(int nEvent, int nState)
{
    CSWGuiPanel::HandleInputEvent(nEvent, nState);

    if (nState != 0)
    {
        switch (nEvent)
        {
        case GUIEVENT_CANCEL:
        case 46:
            CloseCreateItem(0);
            break;
        case 58:
            if (m_bBuyMode)
                m_lbxDescription.HandleInputEvent(GUIEVENT_DOWN, 1);
            else
                m_lbxDescriptionInv.HandleInputEvent(GUIEVENT_DOWN, 1);
            break;
        case 57:
            if (m_bBuyMode)
                m_lbxDescription.HandleInputEvent(GUIEVENT_UP, 1);
            else
                m_lbxDescriptionInv.HandleInputEvent(GUIEVENT_UP, 1);
            break;
        }
    }
}
// PC 0x0072ACB0
void CSWGuiCreateItem::ShowSellGUI()
{
    CSWGuiListBox* pActiveList;

    m_bBuyMode = 0;
    pActiveList = &m_lbxInventoryItems;
    pActiveList->m_bEnabled = 1;
    m_lbxStoreItems.SetEnabledFlag(0);
    m_lblRelatedSkill.SetEnabledFlag(0);
    m_lblTitle.m_cText.m_cParams.SetStrRef(115425);
    m_lbxDescriptionInv.SetEnabledFlag(1);
    m_lbxDescription.SetEnabledFlag(0);
    for (int i = 0; i < 5; i++)
        m_abtnCreateFilters[i].SetEnabledFlag(0);
    for (int i = 0; i < 5; i++)
        m_abtnBreakdownFilters[i].SetEnabledFlag(1);
    m_btnAccept.m_cText.m_cParams.SetStrRef(115426);
    m_btnExamine.m_cText.m_cParams.SetStrRef(48375);
    m_alblBars[1].SetEnabledFlag(0);
    m_alblBars[2].SetEnabledFlag(0);
    m_lblCost.m_cText.m_cParams.SetStrRef(115427);
    m_lbl1CC.m_cText.m_cParams.SetStrRef(48380);
    m_lbl6EC.m_cText.m_cParams.SetStrRef(48375);
    m_lblCost.m_cText.m_cParams.SetStrRef(115427);
    SetActiveControl(pActiveList, 0);
    PopulateInventoryItemListBox();
}
// PC 0x00731870
void CSWGuiCreateItem::ShowBuyGUI()
{
    CSWGuiListBox* pActiveList;

    m_bBuyMode = 1;
    pActiveList = &m_lbxStoreItems;
    m_lbxInventoryItems.m_bEnabled = 0;
    pActiveList->m_bEnabled = 1;
    m_lblRelatedSkill.SetEnabledFlag(1);
    m_lblRelatedSkill.m_cText.m_cParams.SetText("");
    m_lblTitle.m_cText.m_cParams.SetStrRef(111574);
    m_lbxDescriptionInv.SetEnabledFlag(0);
    m_lbxDescription.SetEnabledFlag(1);
    for (int i = 0; i < 5; i++)
        m_abtnCreateFilters[i].SetEnabledFlag(1);
    for (int i = 0; i < 5; i++)
        m_abtnBreakdownFilters[i].SetEnabledFlag(0);
    m_btnAccept.m_cText.m_cParams.SetStrRef(48379);
    m_btnExamine.m_cText.m_cParams.SetStrRef(48373);
    m_alblBars[1].SetEnabledFlag(1);
    m_alblBars[2].SetEnabledFlag(1);
    m_lblCost.m_cText.m_cParams.SetStrRef(48376);
    m_lbl1CC.m_cText.m_cParams.SetStrRef(48379);
    m_lbl6EC.m_cText.m_cParams.SetStrRef(48373);
    m_lblCost.m_cText.m_cParams.SetStrRef(48376);
    SetActiveControl(pActiveList, 0);
    PopulateStoreItemListBox();
}
// PC 0x007280D0
void CSWGuiCreateItem::BuyItem(CSWSItem* pItem)
{
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    CSWSCreature* pCreature = pServer->GetPlayerCreature();

    if (pCreature != 0 && pCreature->GetItemRepository(1) != 0 && pItem != 0)
    {
        m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_BUY_ITEM);
        m_nComponents -= GetItemBuyValue(pItem);
        pCreature->SetComponent(m_nComponents);
        CSWSItem* pNewItem = new CSWSItem(OBJECT_INVALID);
        pNewItem->CopyItem(pItem);
        pCreature->AcquireItem(&pNewItem, m_pStore->m_nId, OBJECT_INVALID, 1);
        int nSelectedItem = m_lbxStoreItems.m_nSelectedItem;
        if (nSelectedItem >= m_lbxStoreItems.m_lItems.num)
            nSelectedItem = m_lbxStoreItems.m_lItems.num - 1;
        m_lbxStoreItems.SetSelectedControl(nSelectedItem, 0);
        m_lblCreditsValue.m_cText.m_cParams.SetText(CExoString(m_nComponents));
    }
}
// PC 0x00728280
void CSWGuiCreateItem::SetDescription(const CExoString& sDescription)
{
    CSWGuiListBox* pListBox;
    int nHeight;
    CSWGuiControl* pLabel;

    if (m_bBuyMode)
        pListBox = &m_lbxDescription;
    else
        pListBox = &m_lbxDescriptionInv;
    m_lblDescription.m_cText.m_cParams.SetText(sDescription);
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
    m_lblRanalog.m_bEnabled =
        pListBox->m_nContentHeight + pListBox->m_nPadding >
            pListBox->m_cItemExtent.m_nHeight &&
        pListBox->m_lItems.num > 0 && !pListBox->m_bHideScrollIcon;
}
// PC 0x0072AAE0
void CSWGuiCreateItem::SellItem(CSWSItem* pItem)
{
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    CSWSCreature* pCreature = pServer->GetPlayerCreature();

    if (pItem != 0 && pCreature != 0)
    {
        CItemRepository* pInventory = pCreature->GetItemRepository(1);
        if (pInventory != 0)
        {
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELL_ITEM);
            m_nComponents +=
                max(1, (int)(GetItemBuyValue(pItem) * m_fRepairFactor));
            pCreature->SetComponent(m_nComponents);
            if (pItem->m_nStackSize > 1)
                pItem->SplitItem(1);
            else
                pInventory->RemoveItem(pItem);
            int nSelectedItem = m_lbxInventoryItems.m_nSelectedItem;
            PopulateInventoryItemListBox();
            if (nSelectedItem >= m_lbxInventoryItems.m_lItems.num)
                nSelectedItem = m_lbxInventoryItems.m_lItems.num - 1;
            m_lbxInventoryItems.SetSelectedControl(nSelectedItem, 0);
            m_lblCreditsValue.m_cText.m_cParams.SetText(CExoString(m_nComponents));
        }
    }
}
// PC 0x00726010
int CSWGuiCreateItem::CheckBreakdownFilter(CSWSItem* pItem,
                                           CSWSCreature* pCreature, int nSlot)
{
    int bResult = 0;

    if (pItem == 0 || pCreature == 0)
        return 0;

    switch (m_nBreakdownFilter)
    {
    case 0:
        bResult = 1;
        break;
    case 1:
        if (pItem->GetBaseItem() != 0)
            bResult = (pItem->GetBaseItem()->m_nEquipableSlots &
                       (INVENTORY_SLOT_RIGHTHAND | INVENTORY_SLOT_LEFTHAND)) != 0;
        break;
    case 2:
        if (pItem->GetBaseItem() != 0)
            bResult = (pItem->GetBaseItem()->m_nEquipableSlots &
                       (INVENTORY_SLOT_HEAD | INVENTORY_SLOT_ARMOUR |
                        INVENTORY_SLOT_HANDS | INVENTORY_SLOT_RIGHTRING |
                        INVENTORY_SLOT_NECK | INVENTORY_SLOT_BELT |
                        INVENTORY_SLOT_ARMS)) != 0;
        break;
    case 3:
        if (pCreature->GetClientCreature() != 0)
        {
            if (pCreature->GetClientCreature()->IsLeaderUsableItem(pItem, nSlot,
                                                                  0xFF) &&
                pCreature->CanUseItem(pItem, 0, 1))
                bResult = 1;
            else
                bResult = 0;
        }
        break;
    case 4:
        bResult = 1;
        if (pItem->GetBaseItem() != 0)
        {
            if (pItem->GetBaseItem()->m_nItemType == 24)
                bResult = 0;
            if (pItem->GetBaseItem()->m_nEquipableSlots != 0)
                bResult = 0;
        }
        if (pCreature->GetClientCreature() != 0)
        {
            if (pCreature->GetClientCreature()->IsLeaderUsableItem(pItem, nSlot,
                                                                  0xFF) &&
                pCreature->CanUseItem(pItem, 0, 1))
                bResult = 0;
        }
        break;
    default:
        bResult = 0;
        break;
    }
    if (pItem->GetCost() == 0)
        bResult = 0;
    return bResult;
}
// PC 0x007316A0
void CSWGuiCreateItem::PopulateStoreItemListBox()
{
    CItemRepository* pRepository = m_pStore->m_pItemRepository;
    CExoArrayList<CSWGuiControl*> lControls;
    int nEntryIndex = 0;

    for (int i = pRepository->GetNumItems() - 1; i != -1; i--)
    {
        CSWSItem* pItem = pRepository->ItemListGetItem(i);
        if (pItem != 0 && pItem->m_bPlotObject == 0)
        {
            CExoString sTag(pItem->m_sTag.CStr());
            CreateItemEntry(nEntryIndex, lControls, pItem->GetId(), 0, sTag);
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
            m_lblCostValue.m_cText.m_cParams.SetText("");
            m_lblStockValue.m_cText.m_cParams.SetText("");
            m_lblDescription.m_cText.m_cParams.SetText("");
        }
    }
    m_n3EC8 = 0;
}
// PC 0x00729AE0
void CSWGuiCreateItem::PopulateInventoryItemListBox()
{
    CItemRepository* pRepository;
    C2DA* pTable;

    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    CSWSCreature* pCreature = pServer->GetCreatureByGameObjectID(m_nCreatureId);
    if (m_s3EF0.IsEmpty())
    {
        pTable = g_pRules->Get2DArrays()->m_pChemicalCreate;
    }
    else
    {
        pTable = new C2DA("tutchemcreate", 0);
        pTable->Load2DArray();
    }
    if (pTable == 0)
        return;

    if (pCreature != 0)
        pRepository = pCreature->GetItemRepository(1);
    CExoArrayList<CSWGuiControl*> lControls;
    int nEntryIndex = 0;
    CExoString sLabel;
    for (unsigned long i = 0; i < pRepository->GetNumItems(); i++)
    {
        CSWSItem* pItem = pRepository->ItemListGetItem(i);
        int bComponent = 0;
        for (int nRow = 0; nRow < pTable->m_nNumRows; nRow++)
        {
            pTable->GetCExoStringEntry(nRow, "label", &sLabel);
            CExoString sTag;
            sTag = pItem->m_sTag.CStr();
            if (sTag == sLabel)
                bComponent = 1;
        }
        if (pItem != 0 && pItem->m_bPlotObject == 0 && bComponent == 0 &&
            CheckBreakdownFilter(pItem, pCreature, 0))
        {
            CreateItemEntry(nEntryIndex, lControls, pItem->GetId(), 1, "");
        }
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
            m_lblCostValue.m_cText.m_cParams.SetText("");
            m_lblStockValue.m_cText.m_cParams.SetText("");
            m_lblDescription.m_cText.m_cParams.SetText("");
        }
    }
    if (!m_s3EF0.IsEmpty())
    {
        pTable->Unload2DArray();
        delete pTable;
    }
    m_n3EC8 = 0;
}
// PC 0x00729E50
void CSWGuiCreateItem::OnControlStoreAButton(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        if (pControl->m_nEvent != 0)
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
        if (static_cast<CSWGuiStoreItemEntry*>(pControl)->m_n3B4 == 0)
        {
            CSWGuiMessageBox* pMessageBox =
                g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
            pMessageBox->SetAllowCancel(0);
            pMessageBox->SetCallback(0, 0, 0);
            pMessageBox->SetMessageColors(CGuiInGame::COLOR_UPGRADE_BORDER,
                                          CGuiInGame::COLOR_NEW_HIGHLIGHT);
            pMessageBox->SetMessage(STRREF_SKILL_TOO_LOW);
            g_pAppManager->m_pClientExoApp->GetSWGuiManager()->AddPanel(
                pMessageBox, 1, 1);
            return;
        }
        unsigned long oidItem =
            static_cast<CSWGuiStoreItemEntry*>(pControl)->m_oidItem;
        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
        CSWSItem* pItem = pServer->GetItemByGameObjectID(
            pServer->ClientToServerObjectId(oidItem));
        if (pItem != 0)
        {
            if ((unsigned long)m_nComponents >= GetItemBuyValue(pItem))
            {
                CServerExoApp* pCreatureServer = g_pAppManager->m_pServerExoApp;
                CSWSCreature* pCreature = pCreatureServer->GetPlayerCreature();
                unsigned int nConfirmationValue;
                if (pCreature != 0)
                {
                    CSWSCreatureStats* pStats = pCreature->m_pStats;
                    nConfirmationValue = pStats->GetLevel(0) * CONFIRM_VALUE_PER_LEVEL;
                }
                else
                    nConfirmationValue = CONFIRM_VALUE_PER_LEVEL;
                if (GetItemBuyValue(pItem) > nConfirmationValue)
                {
                    CSWGuiMessageBox* pMessageBox =
                        g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
                    CExoString sMessage;
                    g_pTlkTable->SetCustomToken(0, CExoString(nConfirmationValue));
                    sMessage = g_pSWTlkTable->Fetch(STRREF_CONFIRM_CREATE_COST, 0, 0, 0);
                    pMessageBox->SetAllowCancel(1);
                    pMessageBox->SetMessage(sMessage);
                    pMessageBox->SetMessageColors(CGuiInGame::COLOR_UPGRADE_BORDER,
                                                  CGuiInGame::COLOR_NEW_HIGHLIGHT);
                    pMessageBox->SetCallback(this,
                        (CSWGuiHandler)&CSWGuiCreateItem::BuyItemCallback, pControl);
                    g_pAppManager->m_pClientExoApp->GetSWGuiManager()->AddPanel(
                        pMessageBox, 1, 1);
                }
                else
                    BuyItem(pItem);
            }
            else
            {
                CSWGuiMessageBox* pMessageBox =
                    g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
                CExoString sMessage;
                pMessageBox->SetAllowCancel(0);
                pMessageBox->SetMessage(STRREF_NOT_ENOUGH_COMPONENTS);
                pMessageBox->SetMessageColors(CGuiInGame::COLOR_UPGRADE_BORDER,
                                              CGuiInGame::COLOR_NEW_HIGHLIGHT);
                pMessageBox->SetCallback(0, 0, 0);
                g_pAppManager->m_pClientExoApp->GetSWGuiManager()->AddPanel(
                    pMessageBox, 1, 1);
            }
        }
    }
}
// PC 0x0072D270
void CSWGuiCreateItem::OnControlInvAButton(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        if (pControl->m_nEvent == CSWGuiControl::EVENT_CONFIRM)
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
        unsigned long oidItem =
            static_cast<CSWGuiStoreItemEntry*>(pControl)->m_oidItem;
        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
        CSWSItem* pItem = pServer->GetItemByGameObjectID(
            pServer->ClientToServerObjectId(oidItem));
        if (pItem != 0)
        {
            for (int i = 0; i < CSWGuiUpgrade::NUM_UPGRADE_SLOTS; i++)
            {
                if (pItem->m_anUpgradeSlot[i] >= 0)
                {
                    CSWGuiMessageBox* pMessageBox =
                        g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
                    pMessageBox->SetAllowCancel(0);
                    pMessageBox->SetMessage(STRREF_REMOVE_UPGRADES_FIRST);
                    pMessageBox->SetCallback(0, 0, 0);
                    pMessageBox->SetMessageColors(CGuiInGame::COLOR_UPGRADE_BORDER,
                                                  CGuiInGame::COLOR_NEW_HIGHLIGHT);
                    g_pAppManager->m_pClientExoApp->GetSWGuiManager()->AddPanel(
                        pMessageBox, 1, 1);
                    return;
                }
            }
            CServerExoApp* pCreatureServer = g_pAppManager->m_pServerExoApp;
            CSWSCreature* pCreature = pCreatureServer->GetPlayerCreature();
            unsigned int nConfirmationValue;
            if (pCreature != 0 && pCreature->m_pStats->GetLevel(0) * CONFIRM_VALUE_PER_LEVEL <= MAX_BREAKDOWN_CONFIRM_VALUE)
            {
                CSWSCreatureStats* pStats = pCreature->GetStats();
                nConfirmationValue = pStats->GetLevel(0) * CONFIRM_VALUE_PER_LEVEL;
            }
            else
                nConfirmationValue = MAX_BREAKDOWN_CONFIRM_VALUE;
            if (max(1u, pItem->GetCost() / COMPONENT_VALUE_DIVISOR) * m_fRepairFactor >
                nConfirmationValue)
            {
                CSWGuiMessageBox* pMessageBox =
                    g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
                CExoString sMessage;
                g_pTlkTable->SetCustomToken(0, CExoString(nConfirmationValue));
                sMessage = g_pSWTlkTable->Fetch(STRREF_CONFIRM_BREAKDOWN_VALUE, 0, 0, 0);
                pMessageBox->SetAllowCancel(1);
                pMessageBox->SetMessage(sMessage);
                pMessageBox->SetCallback(this,
                    (CSWGuiHandler)&CSWGuiCreateItem::SellItemCallback, pControl);
                pMessageBox->SetMessageColors(CGuiInGame::COLOR_UPGRADE_BORDER,
                                              CGuiInGame::COLOR_NEW_HIGHLIGHT);
                g_pAppManager->m_pClientExoApp->GetSWGuiManager()->AddPanel(
                    pMessageBox, 1, 1);
            }
            else
                SellItem(pItem);
        }
    }
}
// PC 0x00731FC0
void CSWGuiCreateItem::SetItemList()
{
    C2DA* pTable;

    CItemRepository* pRepository = m_pStore->m_pItemRepository;
    CSWSCreature* pCreature =
        g_pAppManager->m_pServerExoApp->GetPlayerCreature();
    if (m_s3EF0.IsEmpty())
    {
        pTable = g_pRules->Get2DArrays()->m_pItemCreate;
    }
    else
    {
        pTable = new C2DA(m_s3EF0, 0);
        pTable->Load2DArray();
        UpdateCreateFilters();
    }
    if (pTable == 0)
        return;

    for (int i = 0; i < pRepository->GetNumItems(); i++)
    {
        CSWSItem* pItem = pRepository->ItemListGetItem(i);
        if (pItem != 0)
            delete pItem;
    }
    pRepository->ClearItems();

    CExoString sLabel;
    int nLevel = pCreature->m_pStats->GetLevel(0);
    int nAlignment = pCreature->m_pStats->GetSimpleAlignmentGoodEvil();
    for (int nRow = 0; nRow < pTable->m_nNumRows; nRow++)
    {
        int nGroup;
        int nSkill;
        int nLevelRequired;
        int nAlignmentRequired;
        int nBaseSkill;

        if (!pTable->GetINTEntry(nRow, "group", &nGroup))
            return;
        if (!pTable->GetINTEntry(nRow, "skill", &nSkill))
            return;
        if (!pTable->GetINTEntry(nRow, "level", &nLevelRequired))
            return;
        if (!pTable->GetINTEntry(nRow, "align", &nAlignmentRequired))
            return;
        if (!pTable->GetINTEntry(nRow, "base_skill", &nBaseSkill))
            return;
        if (nSkill <= pCreature->m_pStats->GetSkillRank(nBaseSkill, 0, 0) + 8 &&
            nGroup == m_nCreateFilter &&
            (nLevelRequired == 0 || nLevel >= nLevelRequired) &&
            (nAlignmentRequired == 0 || nAlignment == nAlignmentRequired))
        {
            CSWSItem* pItem = new CSWSItem(OBJECT_INVALID);
            pTable->GetCExoStringEntry(nRow, "label", &sLabel);
            if (pItem->LoadFromTemplate(CResRef(sLabel.CStr())))
                m_pStore->AcquireItem(pItem, 1);
        }
    }
    if (!m_s3EF0.IsEmpty())
    {
        pTable->Unload2DArray();
        delete pTable;
    }
    PopulateStoreItemListBox();
}
// PC 0x0072F2B0
int CSWGuiCreateItem::CreateItemEntry(int& nEntries,
                                      CExoArrayList<CSWGuiControl*>& lControls,
                                      unsigned long oidItem, int bInventory,
                                      CExoString sTemplate)
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
                            (CSWGuiHandler)&CSWGuiCreateItem::OnControlInvAButton);
        }
        else
        {
            CSWGuiButton* pProto = m_lbxStoreItems.m_pProtoItem->AsButton();
            pNewEntry->Initialize(pProto->m_cText, pProto->m_cBorder,
                               pProto->m_cHilite,
                               m_lbxStoreItems.m_cItemExtent.m_nWidth -
                                   2 * m_lbxStoreItems.m_nPadding);
            pNewEntry->AddEvent(GUIEVENT_ACTIVATE, this,
                            (CSWGuiHandler)&CSWGuiCreateItem::OnControlStoreAButton);
        }
        pNewEntry->AddEvent(GUIEVENT_MOUSE_ENTER, this,
                        (CSWGuiHandler)&CSWGuiCreateItem::OnControlEntered);
    }
    if (bInventory == 0)
    {
        C2DA* pTable;
        if (m_s3EF0.IsEmpty())
        {
            pTable = g_pRules->Get2DArrays()->m_pItemCreate;
        }
        else
        {
            pTable = new C2DA(m_s3EF0, 0);
            pTable->Load2DArray();
        }
        if (pTable == 0)
            return 0;

        CExoString sTag;
        int nFoundRow = -1;
        for (int nRow = 0; nRow < pTable->m_nNumRows; nRow++)
        {
            if (!pTable->GetCExoStringEntry(nRow, "OverrideTag", &sTag))
                sTag = "";
            if (sTag.IsEmpty())
            {
                if (!pTable->GetCExoStringEntry(nRow, "label", &sTag))
                    return 0;
            }
            if (sTag.CompareNoCase(sTemplate))
            {
                nFoundRow = nRow;
                break;
            }
        }
        if (nFoundRow != -1)
        {
            pEntry->m_n3B0 = nFoundRow;
            int nBaseSkill = -1;
            int nSkill = -1;
            if (!pTable->GetINTEntry(nFoundRow, "base_skill", &nBaseSkill))
                return 0;
            if (!pTable->GetINTEntry(nFoundRow, "skill", &nSkill))
                return 0;
            CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
            CSWSCreature* pCreature = pServer->GetPlayerCreature();
            if (nSkill > pCreature->GetStats()->GetSkillRank(nBaseSkill, 0, 0))
            {
                pEntry->SetColors(CGuiInGame::DISABLED_TEXT, CGuiInGame::DISABLED_TEXT,
                                  CGuiInGame::COLOR_NEW_HIGHLIGHT,
                                  CGuiInGame::COLOR_NEW_HIGHLIGHT);
                pEntry->m_n3B4 = 0;
            }
            else
            {
                pEntry->SetColors(CGuiInGame::COLOR_UPGRADE_BORDER,
                                  CGuiInGame::COLOR_UPGRADE_TEXT,
                                  CGuiInGame::COLOR_UPGRADE_TEXT,
                                  CGuiInGame::COLOR_NEW_HIGHLIGHT);
                pEntry->m_n3B4 = 1;
            }
        }
        else
        {
            CExoString sError = "CreateItem::CreateItemEntry() -- Could not find a row for an item. Major error: ";
            sError = sError + sTemplate;
        }
        if (!m_s3EF0.IsEmpty())
        {
            pTable->Unload2DArray();
            delete pTable;
        }
    }
    else
    {
        pEntry->SetColors(CGuiInGame::COLOR_UPGRADE_BORDER,
                          CGuiInGame::COLOR_UPGRADE_TEXT,
                          CGuiInGame::COLOR_NEW_HIGHLIGHT,
                          CGuiInGame::COLOR_NEW_HIGHLIGHT);
    }
    pEntry->SetItem(oidItem, 0, -1);
    pEntry->SetActive(0);
    lControls.Add(pEntry);
    nEntries++;
    return 0;
}
// PC 0x0072A1B0
void CSWGuiCreateItem::OnControlEntered(CSWGuiControl* pControl)
{
    C2DA* pTable;
    if (m_s3EF0.IsEmpty())
    {
        CTwoDimArrays* pArrays = g_pRules->m_p2DArrays;
        pTable = pArrays->m_pItemCreate;
    }
    else
    {
        pTable = new C2DA(m_s3EF0, 0);
        pTable->Load2DArray();
    }

    CExoString sUnused;
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    CItemRepository* pRepository = 0;
    CSWSCreature* pCreature = pServer->GetPlayerCreature();
    if (pCreature != 0)
        pRepository = pCreature->GetItemRepository(1);

    if (pControl->m_bSelected != 0)
    {
        unsigned long oidItem =
            static_cast<CSWGuiStoreItemEntry*>(pControl)->m_oidItem;
        pServer = g_pAppManager->m_pServerExoApp;
        CSWSItem* pItem = pServer->GetItemByGameObjectID(
            pServer->ClientToServerObjectId(oidItem));
        if (pItem != 0)
        {
            if (m_lbxStoreItems.m_bEnabled)
            {
                m_lblCostValue.m_cText.m_cParams.SetText(
                    CExoString(GetItemBuyValue(pItem)));
                int nRow = static_cast<CSWGuiStoreItemEntry*>(pControl)->m_n3B0;
                int nBaseSkill = -1;
                int nRank = -1;
                if (!pTable->GetINTEntry(nRow, "base_skill", &nBaseSkill))
                    return;
                CServerExoApp* pPlayerServer = g_pAppManager->m_pServerExoApp;
                CSWSCreature* pPlayer = pPlayerServer->GetPlayerCreature();
                if (pPlayer != 0)
                    nRank = pPlayer->GetStats()->GetSkillRank(nBaseSkill, 0, 0);
                CExoString sText;
                CExoString sSkillName =
                    g_pRules->m_pSkills[(unsigned short)nBaseSkill].GetNameText();
                CExoString sRank;
                sRank.Format("%d", nRank);
                sText = sSkillName + " (" + sRank + ")";
                m_lblRelatedSkill.m_cText.m_cParams.SetText(sText);
                if (static_cast<CSWGuiStoreItemEntry*>(pControl)->m_n3B4)
                    m_lblRelatedSkill.m_cText.m_cParams.SetColor(
                        CGuiInGame::COLOR_UPGRADE_TEXT);
                else
                    m_lblRelatedSkill.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_RED);
            }
            else
            {
                float fRepairFactor = m_fRepairFactor;
                int nValue = (int)(GetItemBuyValue(pItem) * fRepairFactor);
                if (nValue <= 0)
                    nValue = 1;
                m_lblCostValue.m_cText.m_cParams.SetText(CExoString(nValue));
            }

            if (m_lbxStoreItems.m_bEnabled && pRepository != 0)
            {
                int nCount = pRepository->GetMatchingItemCount(pItem);
                int nEquipped = 0;
                for (int i = 0; i < 3; i++)
                {
                    CSWCCreature* pCharacter = g_pAppManager->m_pClientExoApp
                                                   ->GetSWParty()
                                                   ->GetCharacter(i);
                    if (pCharacter != 0)
                        nEquipped += pCharacter->GetMatchingEquippedItemCount(pItem);
                }
                if (nCount == -1)
                    m_lblStockValue.m_cText.m_cParams.SetText(STRREF_INFINITE);
                else
                    m_lblStockValue.m_cText.m_cParams.SetText(
                        CExoString(nCount + nEquipped));
            }
            else
            {
                if (pItem->m_bInfinite)
                {
                    m_lblStockValue.m_cText.m_cParams.SetStrRef(STRREF_INFINITE);
                }
                else
                {
                    m_lblStockValue.m_cText.m_cParams.SetText(
                        CExoString(pItem->m_nStackSize));
                }
            }

            CExoString sDescription = pItem->GetPropertyDescription();
            int nTag = sDescription.Find('<', 0);
            int nToken = sDescription.Find('{', 0);
            if (nTag != -1 || nToken != -2)
            {
                CSWCCreature* pPlayerCharacter =
                    g_pAppManager->m_pClientExoApp->GetSWParty()->GetPlayerCharacter();
                g_pSWTlkTable->ParseStr(sDescription, (CGameObject*)pPlayerCharacter, 0, 1);
            }

            if (m_lbxStoreItems.m_bEnabled)
            {
                int nRow = static_cast<CSWGuiStoreItemEntry*>(pControl)->m_n3B0;
                int nSkill = -1;
                int nBaseSkill = -1;
                if (!pTable->GetINTEntry(nRow, "base_skill", &nBaseSkill))
                    return;
                if (!pTable->GetINTEntry(nRow, "skill", &nSkill))
                    return;
                CExoString sRequired = g_pTlkTable->GetSimpleString(STRREF_SKILL_REQUIRED_TO_CREATE);
                CExoString sSkillText;
                CExoString sSkill(nSkill);
                sSkillText = g_pRules->m_pSkills[(unsigned short)nBaseSkill].GetNameText() +
                             " (" + sSkill + ")";
                CExoString sNewLine1("\n");
                CExoString sNewLine2("\n");
                sDescription = sRequired + sNewLine2 + sSkillText + sNewLine1 + sDescription;
            }
            SetDescription(sDescription);
        }
    }

    if (!m_s3EF0.IsEmpty())
    {
        pTable->Unload2DArray();
        delete pTable;
    }
}
// PC 0x007262B0
void CSWGuiCreateMedicalItem::PopulateStore()
{
    CServerExoApp* pServer;
    CSWSCreature* pCreature;
    int nChemicals;

    pServer = g_pAppManager->m_pServerExoApp;
    pCreature = pServer->GetCreatureByGameObjectID(m_nCreatureId);
    if (pCreature != 0)
    {
        nChemicals = pCreature->GetChemical();
        m_nChemicals = nChemicals;
        m_lblCreditsValue.m_cText.m_cParams.SetText(CExoString(nChemicals));
    }
}
// PC 0x00726330
void CSWGuiCreateMedicalItem::CloseChemical(CSWGuiControl* pControl)
{
    g_pAppManager->m_pClientExoApp->GetInGameGui()->HideChemicalUpgradeScreen();
}
// PC 0x00726350
void CSWGuiCreateMedicalItem::UpdateCreateFilters()
{
    for (int i = 0; i < 4; i++)
    {
        if (m_abtnCreateFilters[i].m_nIndex == m_nCreateFilter)
            m_abtnCreateFilters[i].m_cText.m_cParams.SetDefaultColor(
                CGuiInGame::COLOR_NEW_HIGHLIGHT);
        else
            m_abtnCreateFilters[i].m_cText.m_cParams.SetDefaultColor(
                CGuiInGame::COLOR_UPGRADE_TEXT);
    }
}
// PC 0x00726390
unsigned long CSWGuiCreateMedicalItem::GetItemBuyValue(CSWSItem* pItem)
{
    if (pItem == 0)
        return 0;
    return max(1u, pItem->GetCost() / CHEMICAL_VALUE_DIVISOR);
}
// PC 0x007283A0
void CSWGuiCreateMedicalItem::HandleInputEvent(int nEvent, int nState)
{
    CSWGuiPanel::HandleInputEvent(nEvent, nState);

    if (nState != 0)
    {
        switch (nEvent)
        {
        case GUIEVENT_CANCEL:
        case 46:
            g_pAppManager->m_pClientExoApp->GetInGameGui()->HideChemicalUpgradeScreen();
            break;
        case 58:
            if (m_bBuyMode)
                m_lbxDescription.HandleInputEvent(GUIEVENT_DOWN, 1);
            else
                m_lbxDescriptionInv.HandleInputEvent(GUIEVENT_DOWN, 1);
            break;
        case 57:
            if (m_bBuyMode)
                m_lbxDescription.HandleInputEvent(GUIEVENT_UP, 1);
            else
                m_lbxDescriptionInv.HandleInputEvent(GUIEVENT_UP, 1);
            break;
        }
    }
}
// PC 0x007284B0
void CSWGuiCreateMedicalItem::BuyItem(CSWSItem* pItem)
{
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    CSWSCreature* pCreature = pServer->GetPlayerCreature();

    if (pCreature != 0 && pCreature->GetItemRepository(1) != 0 && pItem != 0)
    {
        m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_BUY_ITEM);
        m_nChemicals -= GetItemBuyValue(pItem);
        pCreature->SetChemical(m_nChemicals);
        CSWSItem* pNewItem = new CSWSItem(OBJECT_INVALID);
        pNewItem->CopyItem(pItem);
        pCreature->AcquireItem(&pNewItem, m_pStore->m_nId, OBJECT_INVALID, 1);
        int nSelectedItem = m_lbxStoreItems.m_nSelectedItem;
        if (nSelectedItem >= m_lbxStoreItems.m_lItems.num)
            nSelectedItem = m_lbxStoreItems.m_lItems.num - 1;
        m_lbxStoreItems.SetSelectedControl(nSelectedItem, 0);
        m_lblCreditsValue.m_cText.m_cParams.SetText(CExoString(m_nChemicals));
    }
}
// PC 0x00728600
void CSWGuiCreateMedicalItem::BuyItemCallback(CSWGuiControl* pControl)
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
// PC 0x00728660
void CSWGuiCreateMedicalItem::SetDescription(const CExoString& sDescription)
{
    CSWGuiListBox* pListBox;
    int nHeight;
    CSWGuiControl* pLabel;

    if (m_lbxStoreItems.m_bEnabled)
        pListBox = &m_lbxDescription;
    else
        pListBox = &m_lbxDescriptionInv;
    m_lblDescription.m_cText.m_cParams.SetText(sDescription);
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
    m_lblRanalog.m_bEnabled =
        m_lbxDescription.m_nContentHeight + m_lbxDescription.m_nPadding >
            m_lbxDescription.m_cItemExtent.m_nHeight &&
        m_lbxDescription.m_lItems.num > 0 && !m_lbxDescription.m_bHideScrollIcon;
}
// PC 0x0072ADE0
void CSWGuiCreateMedicalItem::PopulateInventoryItemListBox()
{
    CItemRepository* pRepository;

    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    CSWSCreature* pCreature = pServer->GetCreatureByGameObjectID(m_nCreatureId);
    CTwoDimArrays* pArrays = g_pRules->m_p2DArrays;
    C2DA* pTable = pArrays->m_pChemicalCreate;
    if (pTable == 0)
        return;

    if (pCreature != 0)
        pRepository = pCreature->GetItemRepository(1);
    CExoArrayList<CSWGuiControl*> lControls;
    int nEntryIndex = 0;
    int nResult = 0;
    CExoString sLabel;
    unsigned long nItems = 0;
    for (unsigned long i = 0; i < pRepository->GetNumItems(); i++)
    {
        CSWSItem* pItem = pRepository->ItemListGetItem(i);
        if (pItem != 0 && pItem->m_bPlotObject == 0)
            nItems++;
    }

    unsigned long nSorted = 0;
    int nPanelSort = 0;
    int nNextPanelSort = 0;
    for (; nSorted < nItems; nPanelSort = nNextPanelSort)
    {
        for (unsigned long i = 0; i < pRepository->GetNumItems() && nResult == 0; i++)
        {
            CSWSItem* pItem = pRepository->ItemListGetItem(i);
            int bChemical = 0;
            for (int nRow = 0; nRow < pTable->m_nNumRows; nRow++)
            {
                pTable->GetCExoStringEntry(nRow, "label", &sLabel);
                CExoString sTag;
                sTag = pItem->m_sTag.CStr();
                if (sTag == sLabel)
                {
                    bChemical = 1;
                    int nGroup = 0;
                    pTable->GetINTEntry(nRow, "group", &nGroup);
                    if (nGroup == 5)
                        bChemical = 0;
                    break;
                }
            }
            if (pItem != 0 && pItem->m_bPlotObject == 0)
            {
                int nItemPanelSort = pItem->GetBaseItem()->m_nStorePanelSort;
                if (nItemPanelSort == nPanelSort)
                {
                    if (bChemical && pItem->GetCost() > 0u)
                    {
                        nResult = CreateItemEntry(nEntryIndex, lControls,
                                                  pItem->GetId(), 1, "");
                    }
                    nSorted++;
                }
                else if (nItemPanelSort > nPanelSort)
                {
                    if (nNextPanelSort == nPanelSort ||
                        nItemPanelSort < nNextPanelSort)
                        nNextPanelSort = nItemPanelSort;
                }
            }
        }
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
            m_lblCostValue.m_cText.m_cParams.SetText("");
            m_lblStockValue.m_cText.m_cParams.SetText("");
            m_lblDescription.m_cText.m_cParams.SetText("");
        }
    }
    m_n33E8 = 0;
}
// PC 0x0072B1B0
void CSWGuiCreateMedicalItem::OnControlStoreAButton(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        if (pControl->m_nEvent != 0)
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
        if (static_cast<CSWGuiStoreItemEntry*>(pControl)->m_n3B4 == 0)
        {
            CSWGuiMessageBox* pMessageBox =
                g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
            pMessageBox->SetAllowCancel(0);
            pMessageBox->SetCallback(0, 0, 0);
            pMessageBox->SetMessageColors(CGuiInGame::COLOR_UPGRADE_BORDER,
                                          CGuiInGame::COLOR_NEW_HIGHLIGHT);
            pMessageBox->SetMessage(STRREF_SKILL_TOO_LOW);
            g_pAppManager->m_pClientExoApp->GetSWGuiManager()->AddPanel(
                pMessageBox, 1, 1);
        }
        else
        {
            unsigned long oidItem =
                static_cast<CSWGuiStoreItemEntry*>(pControl)->m_oidItem;
            CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
            CSWSItem* pItem = pServer->GetItemByGameObjectID(
                pServer->ClientToServerObjectId(oidItem));
            if (pItem != 0)
            {
                if ((unsigned long)m_nChemicals >= GetItemBuyValue(pItem))
                {
                    CServerExoApp* pCreatureServer = g_pAppManager->m_pServerExoApp;
                    CSWSCreature* pCreature = pCreatureServer->GetPlayerCreature();
                    unsigned int nConfirmationValue;
                    if (pCreature != 0)
                    {
                        CSWSCreatureStats* pStats = pCreature->m_pStats;
                        nConfirmationValue = pStats->GetLevel(0) * CONFIRM_VALUE_PER_LEVEL;
                    }
                    else
                        nConfirmationValue = CONFIRM_VALUE_PER_LEVEL;
                    if (GetItemBuyValue(pItem) > nConfirmationValue)
                    {
                        CSWGuiMessageBox* pMessageBox =
                            g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
                        CExoString sMessage;
                        g_pTlkTable->SetCustomToken(0, CExoString(nConfirmationValue));
                        sMessage = g_pSWTlkTable->Fetch(STRREF_CONFIRM_CREATE_COST, 0, 0, 0);
                        pMessageBox->SetAllowCancel(1);
                        pMessageBox->SetMessage(sMessage);
                        pMessageBox->SetCallback(this,
                            (CSWGuiHandler)&CSWGuiCreateMedicalItem::BuyItemCallback,
                            pControl);
                        pMessageBox->SetMessageColors(CGuiInGame::COLOR_UPGRADE_BORDER,
                                                      CGuiInGame::COLOR_NEW_HIGHLIGHT);
                        g_pAppManager->m_pClientExoApp->GetSWGuiManager()->AddPanel(
                            pMessageBox, 1, 1);
                    }
                    else
                        BuyItem(pItem);
                }
                else
                {
                    CSWGuiMessageBox* pMessageBox =
                        g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
                    pMessageBox->SetAllowCancel(0);
                    pMessageBox->SetCallback(0, 0, 0);
                    pMessageBox->SetMessage(STRREF_NOT_ENOUGH_CHEMICALS);
                    pMessageBox->SetMessageColors(CGuiInGame::COLOR_UPGRADE_BORDER,
                                                  CGuiInGame::COLOR_NEW_HIGHLIGHT);
                    g_pAppManager->m_pClientExoApp->GetSWGuiManager()->AddPanel(
                        pMessageBox, 1, 1);
                }
            }
        }
    }
}
// PC 0x0072B4F0
void CSWGuiCreateMedicalItem::OnControlEntered(CSWGuiControl* pControl)
{
    CTwoDimArrays* pArrays = g_pRules->m_p2DArrays;
    C2DA* pTable = pArrays->m_pChemicalCreate;
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    CItemRepository* pRepository = 0;
    CSWSCreature* pCreature = pServer->GetPlayerCreature();
    if (pCreature != 0)
        pRepository = pCreature->GetItemRepository(1);

    if (pControl->m_bSelected != 0)
    {
        unsigned long oidItem =
            static_cast<CSWGuiStoreItemEntry*>(pControl)->m_oidItem;
        pServer = g_pAppManager->m_pServerExoApp;
        CSWSItem* pItem = pServer->GetItemByGameObjectID(
            pServer->ClientToServerObjectId(oidItem));
        if (pItem != 0)
        {
            if (m_lbxStoreItems.m_bEnabled)
            {
                m_lblCostValue.m_cText.m_cParams.SetText(
                    CExoString(GetItemBuyValue(pItem)));
                int nRow = static_cast<CSWGuiStoreItemEntry*>(pControl)->m_n3B0;
                int nBaseSkill = -1;
                int nRank = -1;
                if (!pTable->GetINTEntry(nRow, "base_skill", &nBaseSkill))
                    return;
                CServerExoApp* pPlayerServer = g_pAppManager->m_pServerExoApp;
                CSWSCreature* pPlayer = pPlayerServer->GetPlayerCreature();
                if (pPlayer != 0)
                    nRank = pPlayer->GetStats()->GetSkillRank(nBaseSkill, 0, 0);
                CExoString sText;
                CExoString sSkillName =
                    g_pRules->m_pSkills[(unsigned short)nBaseSkill].GetNameText();
                CExoString sRank(nRank);
                {
                    CExoString sClose(")");
                    CExoString sOpen(" (");
                    sText = sSkillName + sOpen + sRank + sClose;
                }
                m_lblRelatedSkill.m_cText.m_cParams.SetText(sText);
                if (static_cast<CSWGuiStoreItemEntry*>(pControl)->m_n3B4)
                    m_lblRelatedSkill.m_cText.m_cParams.SetColor(
                        CGuiInGame::COLOR_UPGRADE_TEXT);
                else
                    m_lblRelatedSkill.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_RED);
            }
            else
            {
                int nValue = (int)(GetItemBuyValue(pItem) * m_fRepairFactor);
                if (nValue <= 0)
                    nValue = 1;
                m_lblCostValue.m_cText.m_cParams.SetText(CExoString(nValue));
            }

            if (m_lbxStoreItems.m_bEnabled && pRepository != 0)
            {
                int nCount = pRepository->GetMatchingItemCount(pItem);
                int nEquipped = 0;
                for (int i = 0; i < 3; i++)
                {
                    CSWCCreature* pCharacter = g_pAppManager->m_pClientExoApp
                                                   ->GetSWParty()
                                                   ->GetCharacter(i);
                    if (pCharacter != 0)
                        nEquipped += pCharacter->GetMatchingEquippedItemCount(pItem);
                }
                if (nCount == -1)
                    m_lblStockValue.m_cText.m_cParams.SetText(STRREF_INFINITE);
                else
                    m_lblStockValue.m_cText.m_cParams.SetText(
                        CExoString(nCount + nEquipped));
            }
            else
            {
                if (pItem->m_bInfinite)
                {
                    m_lblStockValue.m_cText.m_cParams.SetStrRef(STRREF_INFINITE);
                }
                else
                {
                    m_lblStockValue.m_cText.m_cParams.SetText(
                        CExoString(pItem->m_nStackSize));
                }
            }

            CExoString sDescription = pItem->GetPropertyDescription();
            if (m_lbxStoreItems.m_bEnabled)
            {
                int nSkill = -1;
                int nBaseSkill = -1;
                int nRow = static_cast<CSWGuiStoreItemEntry*>(pControl)->m_n3B0;
                if (!pTable->GetINTEntry(nRow, "base_skill", &nBaseSkill))
                    return;
                if (!pTable->GetINTEntry(nRow, "skillLevel", &nSkill))
                    return;
                CExoString sRequired = g_pTlkTable->GetSimpleString(STRREF_SKILL_REQUIRED_TO_CREATE);
                CExoString sSkillText;
                CExoString sSkill(nSkill);
                {
                    CExoString sClose(")");
                    CExoString sOpen(" (");
                    sSkillText =
                        g_pRules->m_pSkills[(unsigned short)nBaseSkill].GetNameText() +
                        sOpen + sSkill + sClose;
                }
                CExoString sNewLine1("\n");
                CExoString sNewLine2("\n");
                sDescription = sRequired + sNewLine2 + sSkillText + sNewLine1 + sDescription;
            }
            SetDescription(sDescription);
        }
    }
}
// PC 0x0072BD10
void CSWGuiCreateMedicalItem::SellItem(CSWSItem* pItem)
{
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    CSWSCreature* pCreature = pServer->GetPlayerCreature();

    if (pItem != 0 && pCreature != 0)
    {
        CItemRepository* pInventory = pCreature->GetItemRepository(1);
        if (pInventory != 0)
        {
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELL_ITEM);
            m_nChemicals +=
                max(1, (int)(GetItemBuyValue(pItem) * m_fRepairFactor));
            pCreature->SetChemical(m_nChemicals);
            if (pItem->m_nStackSize > 1)
                pItem->SplitItem(1);
            else
                pInventory->RemoveItem(pItem);
            int nSelectedItem = m_lbxInventoryItems.m_nSelectedItem;
            PopulateInventoryItemListBox();
            if (nSelectedItem >= m_lbxInventoryItems.m_lItems.num)
                nSelectedItem = m_lbxInventoryItems.m_lItems.num - 1;
            m_lbxInventoryItems.SetSelectedControl(nSelectedItem, 0);
            m_lblCreditsValue.m_cText.m_cParams.SetText(CExoString(m_nChemicals));
        }
    }
}
// PC 0x0072BE80
void CSWGuiCreateMedicalItem::SellItemCallback(CSWGuiControl* pControl)
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
// PC 0x0072BEE0
void CSWGuiCreateMedicalItem::ShowSellGUI()
{
    CSWGuiListBox* pActiveList;

    m_bBuyMode = 0;
    pActiveList = &m_lbxInventoryItems;
    pActiveList->m_bEnabled = 1;
    m_lbxStoreItems.m_bEnabled = 0;
    m_lblCost.m_cText.m_cParams.SetStrRef(115432);
    m_lblRelatedSkill.SetEnabledFlag(0);
    m_lbxDescription.SetEnabledFlag(0);
    m_lbxDescriptionInv.SetEnabledFlag(1);
    m_lblTitle.m_cText.m_cParams.SetStrRef(115425);
    m_btnAccept.m_cText.m_cParams.SetStrRef(115426);
    m_btnExamine.m_cText.m_cParams.SetStrRef(48375);
    for (int i = 0; i < 4; i++)
        m_abtnCreateFilters[i].SetEnabledFlag(0);
    m_alblBars[1].SetEnabledFlag(0);
    m_alblBars[2].SetEnabledFlag(0);
    m_lbl1CC.m_cText.m_cParams.SetStrRef(48380);
    m_btnExamine.m_cText.m_cParams.SetStrRef(48375);
    m_lblCost.m_cText.m_cParams.SetStrRef(115432);
    SetActiveControl(pActiveList, 0);
    PopulateInventoryItemListBox();
}
// PC 0x0072D650
void CSWGuiCreateMedicalItem::OnControlInvAButton(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        if (pControl->m_nEvent == CSWGuiControl::EVENT_CONFIRM)
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
        unsigned long oidItem =
            static_cast<CSWGuiStoreItemEntry*>(pControl)->m_oidItem;
        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
        CSWSItem* pItem = pServer->GetItemByGameObjectID(
            pServer->ClientToServerObjectId(oidItem));
        if (pItem != 0)
        {
            CServerExoApp* pCreatureServer = g_pAppManager->m_pServerExoApp;
            CSWSCreature* pCreature = pCreatureServer->GetPlayerCreature();
            unsigned int nConfirmationValue;
            if (pCreature != 0 && pCreature->m_pStats->GetLevel(0) * CONFIRM_VALUE_PER_LEVEL <= MAX_BREAKDOWN_CONFIRM_VALUE)
            {
                CSWSCreatureStats* pStats = pCreature->GetStats();
                nConfirmationValue = pStats->GetLevel(0) * CONFIRM_VALUE_PER_LEVEL;
            }
            else
                nConfirmationValue = MAX_BREAKDOWN_CONFIRM_VALUE;
            if (max(1u, pItem->GetCost() / CHEMICAL_VALUE_DIVISOR) * m_fRepairFactor >
                nConfirmationValue)
            {
                CSWGuiMessageBox* pMessageBox =
                    g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
                CExoString sMessage;
                g_pTlkTable->SetCustomToken(0, CExoString(nConfirmationValue));
                sMessage = g_pSWTlkTable->Fetch(STRREF_CONFIRM_BREAKDOWN_VALUE, 0, 0, 0);
                pMessageBox->SetAllowCancel(1);
                pMessageBox->SetMessage(sMessage);
                pMessageBox->SetCallback(this,
                    (CSWGuiHandler)&CSWGuiCreateMedicalItem::SellItemCallback, pControl);
                pMessageBox->SetMessageColors(CGuiInGame::COLOR_UPGRADE_BORDER,
                                              CGuiInGame::COLOR_NEW_HIGHLIGHT);
                g_pAppManager->m_pClientExoApp->GetSWGuiManager()->AddPanel(
                    pMessageBox, 1, 1);
            }
            else
                SellItem(pItem);
        }
    }
}
// PC 0x0072D8C0
void CSWGuiCreateMedicalItem::DeleteItemEntries()
{
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
// PC 0x0072F8A0
void CSWGuiCreateMedicalItem::OnAcceptPicked(CSWGuiControl* pControl)
{
    if (m_bBuyMode != 0)
    {
        CSWGuiControl* pSelected = m_lbxStoreItems.GetSelectedControl();
        if (pSelected != 0)
            OnControlStoreAButton(pSelected);
    }
    else
    {
        CSWGuiControl* pSelected = m_lbxInventoryItems.GetSelectedControl();
        if (pSelected != 0)
            OnControlInvAButton(pSelected);
    }
}
// PC 0x0072F8F0
CSWGuiCreateMedicalItem::~CSWGuiCreateMedicalItem()
{
    delete m_pStore;
    DeleteItemEntries();
}
// PC 0x0072FB50
void CSWGuiCreateMedicalItem::OnPanelRemoved()
{
    DeleteItemEntries();
    CItemRepository* pRepository = m_pStore->m_pItemRepository;
    for (int i = 0; i < pRepository->GetNumItems(); i++)
    {
        CSWSItem* pItem = pRepository->ItemListGetItem(i);
        if (pItem != 0)
            delete pItem;
    }
    CSWGuiPanel::OnPanelRemoved();
}
// PC 0x0072FBA0
int CSWGuiCreateMedicalItem::CreateItemEntry(int& nEntries,
                                      CExoArrayList<CSWGuiControl*>& lControls,
                                      unsigned long oidItem, int bInventory,
                                      CExoString sTemplate)
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
                            (CSWGuiHandler)&CSWGuiCreateMedicalItem::OnControlInvAButton);
            pNewEntry->AddEvent(CSWGuiControl::EVENT_CONFIRM, this,
                            (CSWGuiHandler)&CSWGuiCreateMedicalItem::OnControlInvAButton);
        }
        else
        {
            CSWGuiButton* pProto = m_lbxStoreItems.m_pProtoItem->AsButton();
            pNewEntry->Initialize(pProto->m_cText, pProto->m_cBorder,
                               pProto->m_cHilite,
                               m_lbxStoreItems.m_cItemExtent.m_nWidth -
                                   2 * m_lbxStoreItems.m_nPadding);
            pNewEntry->AddEvent(GUIEVENT_ACTIVATE, this,
                            (CSWGuiHandler)&CSWGuiCreateMedicalItem::OnControlStoreAButton);
            pNewEntry->AddEvent(CSWGuiControl::EVENT_CONFIRM, this,
                            (CSWGuiHandler)&CSWGuiCreateMedicalItem::OnControlStoreAButton);
        }
        pNewEntry->AddEvent(GUIEVENT_MOUSE_ENTER, this,
                        (CSWGuiHandler)&CSWGuiCreateMedicalItem::OnControlEntered);
    }
    if (bInventory == 0)
    {
        C2DA* pTable = g_pRules->Get2DArrays()->m_pChemicalCreate;


        CExoString sTag;
        int nFoundRow = -1;
        for (int nRow = 0; nRow < pTable->m_nNumRows; nRow++)
        {
            if (!pTable->GetCExoStringEntry(nRow, "label", &sTag))
                return 0;
            if (sTag.CompareNoCase(sTemplate))
            {
                nFoundRow = nRow;
                break;
            }
        }
        if (nFoundRow != -1)
        {
            pEntry->m_n3B0 = nFoundRow;
            int nBaseSkill = -1;
            int nSkill = -1;
            if (!pTable->GetINTEntry(nFoundRow, "base_skill", &nBaseSkill))
                return 0;
            if (!pTable->GetINTEntry(nFoundRow, "skillLevel", &nSkill))
                return 0;
            CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
            CSWSCreature* pCreature = pServer->GetPlayerCreature();
            if (nSkill > pCreature->GetStats()->GetSkillRank(nBaseSkill, 0, 0))
            {
                pEntry->SetColors(CGuiInGame::DISABLED_TEXT, CGuiInGame::DISABLED_TEXT,
                                  CGuiInGame::COLOR_NEW_HIGHLIGHT,
                                  CGuiInGame::COLOR_NEW_HIGHLIGHT);
                pEntry->m_n3B4 = 0;
            }
            else
            {
                pEntry->SetColors(CGuiInGame::COLOR_UPGRADE_BORDER,
                                  CGuiInGame::COLOR_UPGRADE_TEXT,
                                  CGuiInGame::COLOR_UPGRADE_TEXT,
                                  CGuiInGame::COLOR_NEW_HIGHLIGHT);
                pEntry->m_n3B4 = 1;
            }
        }

    }
    else
    {
        pEntry->SetColors(CGuiInGame::COLOR_UPGRADE_BORDER,
                          CGuiInGame::COLOR_UPGRADE_TEXT,
                          CGuiInGame::COLOR_NEW_HIGHLIGHT,
                          CGuiInGame::COLOR_NEW_HIGHLIGHT);
    }
    pEntry->SetItem(oidItem, 0, -1);
    pEntry->SetActive(0);
    lControls.Add(pEntry);
    nEntries++;
    return 0;
}
// PC 0x00731A20
void CSWGuiCreateMedicalItem::PopulateStoreItemListBox()
{
    CItemRepository* pRepository = m_pStore->m_pItemRepository;
    CExoArrayList<CSWGuiControl*> lControls;
    int nEntryIndex = 0;

    for (int i = pRepository->GetNumItems() - 1; i != -1; i--)
    {
        CSWSItem* pItem = pRepository->ItemListGetItem(i);
        if (pItem != 0 && pItem->m_bPlotObject == 0)
        {
            CExoString sTag(pItem->m_sTag.CStr());
            CreateItemEntry(nEntryIndex, lControls, pItem->GetId(), 0, sTag);
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
            m_lblCostValue.m_cText.m_cParams.SetText("");
            m_lblStockValue.m_cText.m_cParams.SetText("");
            m_lblDescription.m_cText.m_cParams.SetText("");
        }
    }
    m_n33E8 = 0;
}
// PC 0x00731BF0
void CSWGuiCreateMedicalItem::ShowBuyGUI()
{
    CSWGuiListBox* pActiveList;

    m_bBuyMode = 1;
    pActiveList = &m_lbxStoreItems;
    m_lbxInventoryItems.SetEnabledFlag(0);
    pActiveList->m_bEnabled = 1;
    m_lblRelatedSkill.SetEnabledFlag(1);
    m_lblCost.m_cText.m_cParams.SetStrRef(48434);
    m_lblTitle.m_cText.m_cParams.SetStrRef(111574);
    m_lbxDescription.SetEnabledFlag(1);
    m_lbxDescriptionInv.SetEnabledFlag(0);
    m_btnAccept.m_cText.m_cParams.SetStrRef(48379);
    m_btnExamine.m_cText.m_cParams.SetStrRef(48373);
    for (int i = 0; i < 4; i++)
        m_abtnCreateFilters[i].SetEnabledFlag(1);
    m_alblBars[1].SetEnabledFlag(1);
    m_alblBars[2].SetEnabledFlag(1);
    m_lbl1CC.m_cText.m_cParams.SetStrRef(48379);
    m_btnExamine.m_cText.m_cParams.SetStrRef(48373);
    m_lblCost.m_cText.m_cParams.SetStrRef(48434);
    SetActiveControl(pActiveList, 0);
    PopulateStoreItemListBox();
}
// PC 0x00732500
void CSWGuiCreateMedicalItem::SetItemList()
{
    CItemRepository* pRepository = m_pStore->m_pItemRepository;
    CSWSCreature* pCreature =
        g_pAppManager->m_pServerExoApp->GetPlayerCreature();
    C2DA* pTable = g_pRules->m_p2DArrays->m_pChemicalCreate;

    for (int i = 0; i < pRepository->GetNumItems(); i++)
    {
        CSWSItem* pItem = pRepository->ItemListGetItem(i);
        if (pItem != 0)
            delete pItem;
    }
    pRepository->ClearItems();
    if (pTable == 0)
        return;

    CExoString sLabel;
    for (int nRow = 0; nRow < pTable->m_nNumRows; nRow++)
    {
        int nGroup;
        int nSkill;
        int nBaseSkill;

        if (!pTable->GetINTEntry(nRow, "group", &nGroup))
            return;
        if (!pTable->GetINTEntry(nRow, "skillLevel", &nSkill))
            return;
        if (!pTable->GetINTEntry(nRow, "base_skill", &nBaseSkill))
            return;
        if (nSkill <= pCreature->GetStats()->GetSkillRank(nBaseSkill, 0, 0) + 8 &&
            nGroup == m_nCreateFilter)
        {
            CSWSItem* pItem = new CSWSItem(OBJECT_INVALID);
            pTable->GetCExoStringEntry(nRow, "label", &sLabel);
            CResRef cResRef(sLabel.CStr());
            pItem->LoadFromTemplate(cResRef);
            m_pStore->AcquireItem(pItem, 1);
        }
    }
    PopulateStoreItemListBox();
}
// PC 0x00732780
void CSWGuiCreateMedicalItem::OnCreateFilterPicked(CSWGuiControl* pControl)
{
    if (m_bBuyMode != 0 && pControl != 0)
    {
        m_nCreateFilter = pControl->m_nIndex;
        UpdateCreateFilters();
        SetItemList();
    }
}
// PC 0x007327B0
void CSWGuiCreateMedicalItem::OnControlXButton(CSWGuiControl* pControl)
{
    m_n33F8 = !m_n33F8;
    m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
    if (m_lbxStoreItems.m_bEnabled)
        ShowSellGUI();
    else
        ShowBuyGUI();
}
// PC 0x007327F0
void CSWGuiCreateMedicalItem::InitializeStoreType()
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
            m_lbl834.SetEnabledFlag(0);
            m_lbl6EC.SetEnabledFlag(0);
            ShowBuyGUI();
            return;
        case CSWSStore::BuySellFlagSell:
            m_lbl834.SetEnabledFlag(0);
            m_lbl6EC.SetEnabledFlag(0);
            ShowSellGUI();
            return;
        case CSWSStore::BuySellFlagBoth:
            m_lbl834.SetEnabledFlag(1);
            m_lbl6EC.SetEnabledFlag(1);
            ShowBuyGUI();
            return;
        }
    }
}
// PC 0x00733480
CSWGuiCreateMedicalItem::CSWGuiCreateMedicalItem(CSWGuiManager* pManager,
                                                 unsigned long oidCreature)
    : CSWGuiPanel(pManager)
{
    m_n33DC = 0;

    StartLoadFromLayout("chemical_p");
    InitControl(m_lbxStoreItems, "LB_SHOPITEMS", 1);
    InitControl(m_lbxInventoryItems, "LB_INVITEMS", 1);
    InitControl(m_lblCredits, "LBL_CREDITS", 1);
    InitControl(m_lblCreditsValue, "LBL_CREDITS_VALUE", 1);
    InitControl(m_lblTitle, "LBL_TITLE", 1);
    InitControl(m_lblTitle2, "LBL_TITLE2", 1);
    InitControl(m_lblRelatedSkill, "LBL_RELATEDSKILL", 1);
    InitControl(m_lbxDescriptionInv, "LB_DESCRIPTIONINV", 1);
    InitControl(m_btnAccept, "BTN_Accept", 1);
    InitControl(m_btnCancel, "BTN_Cancel", 1);
    InitControl(m_btnExamine, "BTN_Examine", 1);
    m_btnCancel.AddEvent(CSWGuiControl::EVENT_SELECT, this,
                         (CSWGuiHandler)&CSWGuiCreateMedicalItem::CloseChemical);
    m_btnExamine.AddEvent(CSWGuiControl::EVENT_SELECT, this,
                          (CSWGuiHandler)&CSWGuiCreateMedicalItem::OnControlXButton);
    InitControl(m_alblBars[0], "LBL_BAR1", 1);
    InitControl(m_alblBars[1], "LBL_BAR2", 1);
    InitControl(m_alblBars[2], "LBL_BAR3", 1);
    InitControl(m_lbxDescriptionInv, "LB_DESCRIPTIONINV", 1);
    InitControl(m_abtnCreateFilters[0], "BTN_CREATE_HEALTH", 1);
    InitControl(m_abtnCreateFilters[1], "BTN_CREATE_STIMS", 1);
    InitControl(m_abtnCreateFilters[3], "BTN_CREATE_GRENADES", 1);
    InitControl(m_abtnCreateFilters[2], "BTN_CREATE_MINES", 1);
    InitControl(m_lbxDescription, "LB_DESCRIPTION", 1);
    m_lbxDescription.m_nSelectSound = CSWGuiManager::GUISOUND_SELECTION;
    InitControl(m_lblCost, "LBL_COST", 1);
    InitControl(m_lblCostValue, "LBL_COST_VALUE", 1);
    InitControl(m_lblStock, "LBL_STOCK", 1);
    InitControl(m_lblStockValue, "LBL_STOCK_VALUE", 1);
    InitControl(m_lblRanalog, "LBL_RANALOG", 1);
    StopLoadFromLayout();

    CSWGuiLabel* pProtoLabel = (CSWGuiLabel*)m_lbxDescription.m_pProtoItem;
    m_lblDescription.Initialize(pProtoLabel->m_cExtent, pProtoLabel, 1.0f);
    CSWGuiListBox* pStoreItems = &m_lbxStoreItems;
    CSWGuiListBox* pInventoryItems = &m_lbxInventoryItems;
    pStoreItems->m_bShowScrollBar = 1;
    pInventoryItems->m_bShowScrollBar = 1;
    pStoreItems->SetSelectOnLeftClick(1);
    pInventoryItems->SetSelectOnLeftClick(1);
    SetActiveControl(&m_lbxStoreItems, 0);
    pInventoryItems->m_bEnabled = 0;

    m_n68 = 0;
    m_n33E8 = 0;
    m_n33E0 = 0;
    m_nChemicals = 0;
    m_pStore = new CSWSStore(OBJECT_INVALID);
    m_nCreateFilter = 0;
    SetItemList();
    m_oidCreature = oidCreature;

    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    CSWSCreature* pCreature;
    if (oidCreature == OBJECT_INVALID)
    {
        unsigned long oidPC =
            pServer->GetPlayerList()->GetHead()->AsSWSPlayer()->m_oidPCObject;
        pCreature = pServer->GetCreatureByGameObjectID(oidPC);
    }
    else
        pCreature = pServer->GetCreatureByGameObjectID(oidCreature);
    m_nCreatureId = pCreature->m_nId;
    float fFactor =
        pCreature->GetStats()->GetSkillRank(CSWRules::SKILL_TREAT_INJURY, 0, 0) * 0.05f;
    if (fFactor >= 1.0f)
        fFactor = 1.0f;
    m_fRepairFactor = fFactor;
    m_n33F8 = 0;

    for (int i = 0; i < 4; i++)
    {
        m_abtnCreateFilters[i].AddEvent(
            CSWGuiControl::EVENT_SELECT, this,
            (CSWGuiHandler)&CSWGuiCreateMedicalItem::OnCreateFilterPicked);
        m_abtnCreateFilters[i].AddEvent(
            CSWGuiControl::EVENT_CONFIRM, this,
            (CSWGuiHandler)&CSWGuiCreateMedicalItem::OnCreateFilterPicked);
        m_abtnCreateFilters[i].SetFocusable(0);
        m_abtnCreateFilters[i].m_nIndex = i;
    }
    m_nCreateFilter = 0;
    UpdateCreateFilters();
    CSWGuiButton* pAccept = &m_btnAccept;
    pAccept->AddEvent(CSWGuiControl::EVENT_SELECT, this,
                      (CSWGuiHandler)&CSWGuiCreateMedicalItem::OnAcceptPicked);
    pAccept->m_bFocusable = 0;
    m_btnExamine.SetFocusable(0);
    m_btnCancel.SetFocusable(0);
    m_nBackground = 1;
}
// PC 0x00733E60
void CSWGuiCreateMedicalItem::OnPanelAdded()
{
    if (g_pAppManager->m_pServerExoApp->GetPartyTable()->m_nItemChemical < 0)
        g_pAppManager->m_pServerExoApp->GetPartyTable()->m_nItemChemical = 0;
    InitializeStoreType();
    PopulateStore();
    CSWGuiPanel::OnPanelAdded();
    ShowBuyGUI();
}
// PC 0x00730070
CSWGuiSwoopUpgrade::CSWGuiSwoopUpgrade(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    CExoString sControl;
    m_pUpgradeTable = new C2DA("swoopupgrade", 0);
    m_pUpgradeTable->Load2DArray();
    {
        CResRef cLayout("swoopup_p");
        StartLoadFromLayout(cLayout);
    }
    InitControl(m_cTitle, "LBL_TITLE", 1);
    InitControl(m_lbItems, "LB_ITEMS", 1);
    InitControl(m_lbDesc, "LB_DESC", 1);
    m_lbDesc.m_nSelectSound = CSWGuiManager::GUISOUND_SELECTION;
    InitControl(m_lbDescLs, "LB_DESC", 1);
    m_lbDesc.m_nSelectSound = CSWGuiManager::GUISOUND_SELECTION;
    InitControl(m_cDescriptionBackground, "LBL_DESCBG", 1);
    InitControl(m_cLightsaberDescriptionBackground, "LBL_DESCBG_LS", 1);
    InitControl(m_3dModel, "3D_MODEL", 1);
    InitControl(m_3dModelLs, "3D_MODEL_LS", 1);
    InitControl(m_cSlotName, "LBL_SLOTNAME", 1);
    InitControl(m_cLightsaberSlotName, "LBL_SLOTNAME", 1);
    InitControl(m_cUpgrades, "LBL_UPGRADES", 1);
    InitControl(m_cUpgradeCount, "LBL_UPGRADE_COUNT", 1);
    InitControl(m_lblA, "LBL_A", 1);
    InitControl(m_lblB, "LBL_B", 1);
    InitControl(m_lblAAction, "LBL_A_ACTION", 1);
    InitControl(m_lblBack, "LBL_BACK", 1);
    InitControl(m_cProperty, "LBL_PROPERTY", 1);
    InitControl(m_lblRanalog, "LBL_RANALOG", 1);
    InitControl(m_lblRanalogLs, "LBL_RANALOG_LS", 1);
    InitControl(m_cAssemble, "BTN_ASSEMBLE", 1);
    InitControl(m_btnBack, "BTN_BACK", 1);
    m_btnBack.AddEvent(GUIEVENT_ACTIVATE, this, (CSWGuiHandler)&CSWGuiSwoopUpgrade::CloseItems);
    m_cAssemble.m_nClickSound = CSWGuiManager::GUISOUND_ASSEMBLE;
    m_cAssemble.AddEvent(GUIEVENT_ACTIVATE, this, (CSWGuiHandler)&CSWGuiSwoopUpgrade::OnAssemble);
    m_cAssemble.AddEvent(GUIEVENT_ACCEPT, this, (CSWGuiHandler)&CSWGuiSwoopUpgrade::OnAssemble);
    m_cAssemble.AddEvent(GUIEVENT_MOUSE_ENTER, this,
                        (CSWGuiHandler)&CSWGuiSwoopUpgrade::OnEnterAssemble);
    m_cAssemble.m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_BLUE;
    m_cAssemble.m_cHilite.m_cParams.m_vColor = CGuiInGame::COLOR_YELLOW;
    m_cAssemble.m_cHilite.m_cParams.SetPulsingAlpha(1, 1, 0);
    for (int i = 0; i < NUM_SWOOP_SLOTS; i++)
    {
        sControl.Format("LBL_UPGRADE3%d", i + 1);
        InitControl(m_aUpgradeLabels3[i], sControl, 1);
        sControl.Format("BTN_UPGRADE3%d", i + 1);
        InitControl(m_aUpgradeButtons3[i], sControl, 1);
        m_aUpgradeButtons3[i].AddEvent(GUIEVENT_ACTIVATE, this,
                                      (CSWGuiHandler)&CSWGuiSwoopUpgrade::OnSlotSelected);
        m_aUpgradeButtons3[i].AddEvent(GUIEVENT_ACCEPT, this,
                                      (CSWGuiHandler)&CSWGuiSwoopUpgrade::OnSlotSelected);
        m_aUpgradeButtons3[i].AddEvent(GUIEVENT_MOUSE_ENTER, this,
                                      (CSWGuiHandler)&CSWGuiSwoopUpgrade::OnEnterSlot);
        m_aUpgradeButtons3[i].m_nIndex = i;
        m_aUpgradeButtons3[i].m_cHilite.m_cParams.SetPulsingAlpha(1, 0, 1);
    }
    StopLoadFromLayout();
    m_cSlotName.m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_BLUE;
    m_cLightsaberSlotName.m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_BLUE;
    m_cUpgradeCount.m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_BLUE;
    m_cUpgrades.m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_BLUE;
    m_cProperty.m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_BLUE;
    m_lbItems.m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_BLUE;
    if (m_lbDescLs.m_pProtoItem != NULL)
    {
        m_cDescription.Initialize(m_lbDescLs.m_pProtoItem->m_cExtent, (CSWGuiLabel*)m_lbDescLs.m_pProtoItem, 1.0f);
    }
    CSWGuiButton* pProto = (CSWGuiButton*)m_lbItems.m_pProtoItem;
    if (pProto != NULL)
    {
        pProto->m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_BLUE;
        pProto->m_cHilite.m_cParams.m_vColor = CGuiInGame::COLOR_YELLOW;
        pProto->m_cHilite.m_cParams.SetPulsingAlpha(1, 1, 0);
    }
    m_bItemListShown = 0;
    m_nItemListMode = 0;
    m_pSelectedItem = 0;
    m_pSelectedItemCopy = 0;
    for (int j = 0; j < NUM_SWOOP_SLOTS; j++)
    {
        m_apItemEntries[j] = 0;
        m_an43CC[j] = 0;
    }
    if (g_bDo3DGUIs != 0)
    {
        CSWGuiScene* pScene = &m_3dModel.m_cScene;
        pScene->m_pScene->SpawnRoom("gui3D_room", Vector(), Quaternion(0, 0, 0, 1));
        pScene->AddModel("upgitem_light", -1);
        pScene = &m_3dModelLs.m_cScene;
        pScene->m_pScene->SpawnRoom("gui3D_room", Vector(), Quaternion(0, 0, 0, 1));
        pScene->AddModel("upgitem_light", -1);
    }
}
// PC 0x00726660
void CSWGuiSwoopUpgrade::OnPanelRemoved()
{
    CSWSItem* pItem;
    int i;
    m_nItemListMode = 0;
    m_pSelectedItem = 0;
    pItem = m_pSelectedItemCopy;
    if (pItem != 0)
    {
        delete pItem;
        m_pSelectedItemCopy = 0;
    }
    for (i = 0; i < NUM_SWOOP_SLOTS; i++)
    {
        if (m_apItemEntries[i] != 0)
        {
            delete m_apItemEntries[i];
            m_apItemEntries[i] = 0;
        }
    }
    m_aItemsTaken.num = 0; m_aItemsAdded.num = 0;
    CSWGuiPanel::OnPanelRemoved();
}
// PC 0x007266C0
void CSWGuiSwoopUpgrade::UpdateItems()
{
    CResRef cIcon;
    CExoString sDescription;
    for (int i = 0; i < NUM_SWOOP_UPGRADE_SLOTS; i++)
    {
        if (m_apItemEntries[i] != 0)
        {
            CSWBaseItem* pBaseItem = g_pRules->m_pBaseItems->GetBaseItem(
                m_apItemEntries[i]->GetBaseItemType());
            cIcon = m_apItemEntries[i]->GetIcon();
        }
        else
        {
            cIcon = SWOOP_SLOT_INFO[i].pszIcon;
        }
        m_aUpgradeLabels3[i].m_cBorder.m_cParams.SetFillImage(cIcon, 0);
    }
}
// PC 0x00726C90
void CSWGuiSwoopUpgrade::OnEnterAssemble(CSWGuiControl* pControl)
{
    m_lblAAction.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
    m_lblAAction.m_cText.m_cParams.SetStrRef(CSWGuiUpgrade::STRREF_ASSEMBLE);
    m_cSlotName.m_cText.m_cParams.SetText("");
    m_cLightsaberSlotName.m_cText.m_cParams.SetText("");
    m_cUpgradeCount.m_cText.m_cParams.SetText("");
    m_cProperty.m_cText.m_cParams.SetText("");
    m_cUpgrades.m_cText.m_cParams.SetText("");
}
// PC 0x00726DF0
void CSWGuiSwoopUpgrade::HandleInputEvent(int nEvent, int nState)
{
    if (nState != 0)
    {
        switch (nEvent)
        {
        case GUIEVENT_SCROLL_DOWN:
            if (m_nItemListMode == 1)
            {
                m_lbDescLs.HandleInputEvent(GUIEVENT_DOWN, 1);
            }
            else
            {
                m_lbDesc.HandleInputEvent(GUIEVENT_DOWN, 1);
            }
            break;
        case GUIEVENT_SCROLL_UP:
            if (m_nItemListMode == 1)
            {
                m_lbDescLs.HandleInputEvent(GUIEVENT_UP, 1);
            }
            else
            {
                m_lbDesc.HandleInputEvent(GUIEVENT_UP, 1);
            }
            break;
        }
    }
    CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
// PC 0x00726E60
void CSWGuiSwoopUpgrade::OnAssemble(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        if (pControl->m_nEvent == 45)
        {
            m_pManager->PlayGuiSound((char)pControl->m_nClickSound);
        }
        for (int i = 0; i < NUM_SWOOP_SLOTS; i++)
        {
            if (m_an43CC[i] != 0)
            {
                g_pAppManager->m_pServerExoApp->GetPartyTable()->SetSwoopUpgrade(i, m_an43CC[i]);
            }
        }
        g_pAppManager->m_pClientExoApp->GetInGameGui()->HideSwoopUpgradeScreen();
    }
}
// PC 0x0072C410
void CSWGuiSwoopUpgrade::CloseItems(CSWGuiControl* pControl)
{
    m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
    if (m_bItemListShown)
    {
        ShowItems(0);
        m_bItemListShown = 0;
        SetActiveControl(m_pItemListControl, 0);
        UpdateItems();
        OnEnterSlot(m_pActiveControl);
    }
    else
    {
        CancelPanel();
        g_pAppManager->m_pClientExoApp->GetInGameGui()->HideSwoopUpgradeScreen();
    }
}

// PC 0x007263D0
void CSWGuiSwoopUpgrade::ShowItems(int bShow)
{
    if (bShow)
    {
        m_3dModel.SetShown(0);
        m_3dModelLs.SetShown(0);
        m_cSlotName.SetShown(0);
        m_cLightsaberSlotName.SetShown(0);
        m_cUpgradeCount.SetShown(0);
        m_cUpgrades.SetShown(0);
        m_cProperty.SetShown(0);
        m_cAssemble.SetShown(0);
        for (int i = 0; i < NUM_SWOOP_SLOTS; i++)
        {
            m_aUpgradeButtons3[i].SetShown(0);
            m_aUpgradeLabels3[i].SetShown(0);
        }
        m_lbItems.SetShown(1);
        return;
    }

    if (m_nItemListMode == 1)
    {
        m_3dModel.SetShown(0);
        m_cSlotName.SetShown(0);
        m_cUpgradeCount.SetShown(0);
        m_cUpgrades.SetShown(0);
        m_cProperty.SetShown(0);
        m_cDescriptionBackground.SetShown(0);
        m_lbDesc.SetShown(0);
        m_lblRanalog.SetShown(0);
        m_3dModelLs.SetShown(1);
        m_cLightsaberSlotName.SetShown(1);
        m_cAssemble.SetShown(1);
        m_cLightsaberDescriptionBackground.SetShown(1);
        m_lbDescLs.SetShown(1);
        m_lblRanalogLs.SetShown(1);
        m_cAssemble.SetMoveToControl(0, &m_aUpgradeButtons3[1]);
        m_cAssemble.SetMoveToControl(2, &m_aUpgradeButtons3[4]);
        for (int i = 0; i < NUM_SWOOP_SLOTS; i++)
        {
            m_aUpgradeButtons3[i].SetShown(1);
            m_aUpgradeLabels3[i].SetShown(1);
        }
    }
    else
    {
        m_3dModel.SetShown(1);
        m_cSlotName.SetShown(1);
        m_cUpgradeCount.SetShown(0);
        m_cUpgrades.SetShown(0);
        m_cProperty.SetShown(1);
        m_cAssemble.SetShown(1);
        m_cDescriptionBackground.SetShown(1);
        m_lbDesc.SetShown(1);
        m_lblRanalog.SetShown(1);
        m_3dModelLs.SetShown(0);
        m_cLightsaberSlotName.SetShown(0);
        m_cLightsaberDescriptionBackground.SetShown(0);
        m_lbDescLs.SetShown(0);
        m_lblRanalogLs.SetShown(0);
        m_cAssemble.SetMoveToControl(0, &m_aUpgradeButtons3[1]);
        m_cAssemble.SetMoveToControl(2, &m_aUpgradeButtons3[1]);
        for (int i = 0; i < NUM_SWOOP_SLOTS; i++)
        {
            m_aUpgradeButtons3[i].SetShown(i < 3);
            m_aUpgradeLabels3[i].SetShown(i < 3);
        }
    }
    m_lbItems.SetShown(0);
}
// PC 0x007267A0
void CSWGuiSwoopUpgrade::OnEnterSlot(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        int nSlot = pControl->m_nIndex;
        unsigned long nStrRef = SWOOP_SLOT_INFO[nSlot].nStrRef;
        int nCount = 0;
        CServerExoApp* pServer = g_pAppManager->GetServerExoApp();
        CSWSItem* pItem = pServer->GetItemByGameObjectID(
            pServer->GetPartyTable()->GetPartyItems()->FindItemWithTag(
                m_asUpgradeTemplates[nSlot]));
        if (pItem != 0)
        {
            nCount = pItem->m_nStackSize;
        }
        if (m_apItemEntries[nSlot] != 0)
        {
            CExoString sName;
            CExoLocString sItemName = m_apItemEntries[nSlot]->m_sLocalizedName;
            sItemName.GetString(g_pAppManager->GetClientExoApp()->GetClientLanguage(),
                                &sName, 0);
            int nTag = sName.Find('<', 0);
            int nToken = sName.Find('{', 0);
            if (nTag != -1 || nToken != -1)
            {
                CSWCCreature* pPlayerCharacter =
                    g_pAppManager->GetClientExoApp()->GetSWParty()->GetPlayerCharacter();
                g_pSWTlkTable->ParseStr(sName, pPlayerCharacter, 1, 1);
            }
            m_cSlotName.m_cText.m_cParams.SetText(sName);
            m_cProperty.m_cText.m_cParams.SetText(sName);
            int nDescStrRef = 0;
            CExoString& sTag = m_apItemEntries[nSlot]->GetTag();
            CExoString sTemplate;
            for (int i = 0; i < m_pUpgradeTable->m_nNumRows; i++)
            {
                m_pUpgradeTable->GetCExoStringEntry(i, "template", &sTemplate);
                if (sTag.LowerCase() == sTemplate.LowerCase())
                {
                    int nLevel;
                    m_pUpgradeTable->GetINTEntry(i, "level", &nLevel);
                    m_pUpgradeTable->GetINTEntry(i, "desc_strref", &nDescStrRef);
                    CExoString sDescription = g_pTlkTable->GetSimpleString(nDescStrRef);
                    m_cProperty.m_cText.m_cParams.SetText(sDescription + " " + nLevel);
                }
            }
        }
        else
        {
            m_cProperty.m_cText.m_cParams.SetStrRef(nStrRef);
            m_cSlotName.m_cText.m_cParams.SetStrRef(nStrRef);
        }
        m_cUpgradeCount.m_cText.m_cParams.SetText(nCount);
        m_cUpgrades.m_cText.m_cParams.SetStrRef(CSWGuiUpgrade::STRREF_UPGRADES);
        if (nCount == 0 && m_apItemEntries[nSlot] == 0)
        {
            m_lblAAction.m_cText.m_cParams.SetColor(CGuiInGame::DISABLED_TEXT);
            m_lblAAction.m_cText.m_cParams.SetStrRef(CSWGuiUpgrade::STRREF_EQUIP);
        }
        else
        {
            m_lblAAction.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
            m_lblAAction.m_cText.m_cParams.SetStrRef(m_apItemEntries[nSlot] != 0 ? CSWGuiUpgrade::STRREF_UNEQUIP : CSWGuiUpgrade::STRREF_EQUIP);
        }
    }
}

// PC 0x00728D40
void CSWGuiSwoopUpgrade::SetDescription(const CExoString& sDescription)
{
    CSWGuiListBox* pListBox;
    CSWGuiLabel* pRanalog;
    int nHeight;
    CSWGuiControl* pControl;

    m_cDescription.m_cText.m_cParams.SetText(sDescription);
    pListBox = &m_lbDesc;
    pRanalog = &m_lblRanalog;
    if (m_nItemListMode == 1)
    {
        pListBox = &m_lbDescLs;
        pRanalog = &m_lblRanalogLs;
    }
    pListBox->ClearItems();
    CSWGuiExtent cExtent = m_cDescription.m_cExtent;
    if (sDescription.GetLength() > 0)
    {
        nHeight = m_cDescription.m_cText.GetIdealHeight();
    }
    else
    {
        nHeight = m_cDescription.m_cText.GetFontHeight();
    }
    cExtent.m_nHeight = nHeight;
    m_cDescription.SetExtent(cExtent);
    pControl = &m_cDescription;
    pListBox->AddControls(&pControl, 1, 0, 0, 0);
    pListBox->SetSelectedControl(&m_cDescription, 0);
    pRanalog->SetEnabledFlag(pListBox->IsOversized());
}
// PC 0x00728E70
void CSWGuiSwoopUpgrade::CancelPanel()
{
    CSWSItem* pItem;
    CItemRepository* pPartyItems;
    int i;

    pPartyItems = g_pAppManager->m_pServerExoApp->GetPartyTable()->GetPartyItems();
    for (i = 0; i < m_aItemsAdded.num; i++)
    {
        pItem = m_aItemsAdded[i];
        for (int j = 0; j < NUM_SWOOP_SLOTS; j++)
        {
            if (pItem == m_apItemEntries[j])
            {
                m_apItemEntries[j] = 0;
                break;
            }
        }
        pPartyItems->AddItem(&pItem, 1, 0, 0);
    }
    for (i = 0; i < m_aItemsTaken.num; i++)
    {
        pItem = m_aItemsTaken[i];
        if (pItem->m_nStackSize > 1)
        {
            pItem->m_nStackSize--;
        }
        else
        {
            pPartyItems->RemoveItem(pItem);
            delete pItem;
        }
    }
}
// PC 0x0072C000
void CSWGuiSwoopUpgrade::OnControlEntered(CSWGuiControl* pControl)
{
    int nSelected = pControl->m_bSelected;
    if (nSelected != 0)
    {
        CSWUpgradeItemEntry* pEntry = (CSWUpgradeItemEntry*)pControl;
        if (pEntry->m_oidItem != OBJECT_INVALID)
        {
            CSWSItem* pItem = g_pAppManager->GetServerExoApp()
                ->GetItemByGameObjectID(pEntry->m_oidItem);
            CExoString sDescription;
            CExoLocString& sItemDescription = pItem->m_sDescIdentified;
            sItemDescription.GetString(g_pAppManager->GetClientExoApp()->GetClientLanguage(),
                                       &sDescription, 0);
            if (sDescription != "")
                SetDescription(sDescription);
            else
                SetDescription(g_pAppManager->GetClientExoApp()->GetGUIString(
                    CSWGuiUpgrade::STRREF_NO_DESCRIPTION));
        }
        else
            SetDescription("");
    }
}

// PC 0x00728780
CSWGuiSwoopUpgrade::~CSWGuiSwoopUpgrade()
{
    m_lbItems.ClearItems();
    m_lbDesc.ClearItems();
    if (m_pUpgradeTable != 0)
    {
        delete m_pUpgradeTable;
        m_pUpgradeTable = 0;
    }
}
// PC 0x00728A40
void CSWGuiSwoopUpgrade::OnPanelAdded()
{
    m_pSelectedItemCopy = new CSWSItem(OBJECT_INVALID);
    m_pSelectedItemCopy->CopyItem(m_pSelectedItem);
    m_aUpgradeButtons3[0].SetMoveToControl(0, &m_aUpgradeButtons3[0]);
    m_aUpgradeButtons3[1].SetMoveToControl(0, &m_aUpgradeButtons3[1]);
    m_aUpgradeButtons3[2].SetMoveToControl(0, &m_aUpgradeButtons3[2]);
    ShowItems(0);
    CExoString sTemplate;
    int nGroup;
    int nLevel;
    for (int nRow = 0; nRow < m_pUpgradeTable->m_nNumRows; nRow++)
    {
        m_pUpgradeTable->GetINTEntry(nRow, "group", &nGroup);
        m_pUpgradeTable->GetCExoStringEntry(nRow, "template", &sTemplate);
        m_pUpgradeTable->GetINTEntry(nRow, "level", &nLevel);
        int nSlot;
        for (nSlot = 0; nSlot < NUM_SWOOP_UPGRADE_SLOTS; nSlot++)
        {
            if (nLevel != 0 && m_apItemEntries[nSlot] == 0 &&
                nGroup == SWOOP_SLOT_INFO[nSlot].nProperty)
            {
                break;
            }
        }
        if (nSlot < NUM_SWOOP_UPGRADE_SLOTS)
        {
            m_asUpgradeTemplates[nSlot] = sTemplate.LowerCase();
            m_aUpgradeLabels3[nSlot].m_cBorder.m_cParams.m_fHilightAlpha = 1.0f;
            m_anUpgradeProperties[nSlot] = nRow;
            if (g_pAppManager->m_pServerExoApp->GetPartyTable()->GetSwoopUpgrade(nSlot) == nRow)
            {
                m_apItemEntries[nSlot] = new CSWSItem(OBJECT_INVALID);
                m_apItemEntries[nSlot]->LoadFromTemplate(sTemplate);
                m_apItemEntries[nSlot]->m_anUpgradeSlot[nSlot] = nRow;
                m_an43CC[nSlot] = nRow;
            }
            else if (g_pAppManager->m_pServerExoApp->GetPartyTable()->GetPartyItems()->FindItemWithTag(
                         m_asUpgradeTemplates[nSlot]) == OBJECT_INVALID)
            {
                m_aUpgradeLabels3[nSlot].m_cBorder.m_cParams.m_fHilightAlpha = 0.25f;
            }
        }
    }
    UpdateItems();
    SetActiveControl(&m_cAssemble, 0);
    CSWGuiPanel::OnPanelAdded();
}

// PC 0x0072C130
void CSWGuiSwoopUpgrade::OnUpgradeSelected(CSWGuiControl* pControl)
{
    if (!pControl->m_bSelected)
        return;
    if (pControl->GetEvent() == GUIEVENT_ACCEPT)
        m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
    CSWUpgradeItemEntry* pEntry = (CSWUpgradeItemEntry*)pControl;
    if (pEntry == 0)
        return;
    CServerExoApp* pServer = g_pAppManager->GetServerExoApp();
    CItemRepository* pRepository = pServer->GetPartyTable()->GetPartyItems();
    int nSlot = m_pItemListControl->m_nIndex;
    {
        int nUpgradeIndex = pEntry->m_nUpgradeIndex;
        if (nUpgradeIndex == -1 &&
            pEntry->m_oidItem != OBJECT_INVALID)
            goto Exit;
    }

    {
    CSWSItem*& pOldItem = m_apItemEntries[nSlot];
    if (pOldItem != 0)
    {
        pOldItem->SetPossessor(pServer->GetPlayerCreatureId(),
                                            0, 0, OBJECT_INVALID);
        int nIndex = m_aItemsAdded.IndexOf(pOldItem);
        if (nIndex != -1)
        {
            m_aItemsAdded.DelIndex(nIndex);
            pRepository->AddItem(&pOldItem, 1, 0, 0);
        }
        else
        {
            pRepository->AddItem(&pOldItem, 1, 0, 0);
            m_aItemsTaken.Add(pOldItem);
        }
        if (nSlot != CSWGuiUpgrade::UPGRADE_SLOT_COLOR_CRYSTAL ||
            m_nItemListMode != CSWGuiUpgrade::ITEMLIST_LIGHTSABER)
        {
            pOldItem->m_anUpgradeSlot[nSlot] = -1;
        }
        pOldItem = 0;
    }
    CSWSItem* pItem = pServer->GetItemByGameObjectID(pEntry->m_oidItem);
    if (pItem != 0)
    {
        int nIndex = m_aItemsTaken.IndexOf(pItem);
        if (nIndex != -1)
        {
            m_aItemsTaken.DelIndex(nIndex);
            if (pItem->m_nStackSize > 1)
                pItem = pItem->SplitItem(1);
            else
                pRepository->RemoveItem(pItem);
        }
        else
        {
            if (pItem->m_nStackSize > 1)
                pItem = pItem->SplitItem(1);
            else
                pRepository->RemoveItem(pItem);
            m_aItemsAdded.Add(pItem);
        }
        int nRows = m_pUpgradeTable->GetNumRows();
        int nUpgradeRow = 0;
        for (int nRow = 0; nRow < nRows; nRow++)
        {
            CExoString sTemplate;
            m_pUpgradeTable->GetCExoStringEntry(nRow, "template", &sTemplate);
            if (_stricmp(pItem->m_sTag.CStr(), sTemplate.CStr()) == 0)
            {
                nUpgradeRow = nRow;
                break;
            }
        }
        m_an43CC[nSlot] = nUpgradeRow;
        m_apItemEntries[nSlot] = pItem;
        m_anUpgradeProperties[nSlot] = pEntry->m_nUpgradeIndex;
    }
    }
Exit:
    ShowItems(0);
    m_bItemListShown = 0;
    SetActiveControl(m_pItemListControl, 0);
    UpdateItems();
    OnEnterSlot(m_pActiveControl);
}
// PC 0x0072C490
CSWUpgradeItemEntry* CSWGuiSwoopUpgrade::CreateItemEntry(unsigned long oidItem, int nUpgradeIndex)
{
    int nWidth = m_lbItems.m_cItemExtent.m_nWidth -
                 2 * m_lbItems.m_nPadding;
    CSWGuiButton* pProto = (CSWGuiButton*)m_lbItems.m_pProtoItem;
    CSWUpgradeItemEntry* pEntry = new CSWUpgradeItemEntry;

    pEntry->Initialize(pProto->m_cText, pProto->m_cBorder, pProto->m_cHilite, nWidth, 0);
    pEntry->AddEvent(CSWGuiControl::EVENT_ENTER, this, (CSWGuiHandler)&CSWGuiSwoopUpgrade::OnControlEntered);
    pEntry->AddEvent(CSWGuiControl::EVENT_SELECT, this, (CSWGuiHandler)&CSWGuiSwoopUpgrade::OnUpgradeSelected);
    pEntry->AddEvent(CSWGuiControl::EVENT_CONFIRM, this, (CSWGuiHandler)&CSWGuiSwoopUpgrade::OnUpgradeSelected);
    pEntry->SetItem(oidItem, 0, OBJECT_INVALID, 0);
    pEntry->m_nUpgradeIndex = nUpgradeIndex;
    return pEntry;
}

// PC 0x0072D950
void CSWGuiSwoopUpgrade::OnSlotSelected(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        if (pControl->GetEvent() == GUIEVENT_ACCEPT)
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);

        int nSlot = pControl->m_nIndex;
        CItemRepository* pRepository = g_pAppManager->GetServerExoApp()
            ->GetPartyTable()->GetPartyItems();
        if (m_nItemListMode == 1)
        {
            CExoArrayList<CSWGuiControl*> aControls;
            int nGroup = (nSlot == 0 || nSlot == 2) ? 0 : nSlot - 2;
            if (nSlot == 1)
            {
                CExoString sTemplate;
                aControls.Add(CreateItemEntry(m_apItemEntries[1]->GetId(), -1));
                for (int i = 0; i < m_pCrystalTable->GetNumRows(); i++)
                {
                    m_pCrystalTable->GetCExoStringEntry(i, "Template", &sTemplate);
                    sTemplate = sTemplate.LowerCase();
                    if (sTemplate != m_apItemEntries[1]->m_sTag)
                    {
                        unsigned long oidItem = pRepository->FindItemWithTag(sTemplate);
                        if (oidItem != OBJECT_INVALID)
                            aControls.Add(CreateItemEntry(oidItem, i));
                    }
                }
            }
            else if (nSlot < 3)
            {
                CExoString sTemplate;
                int nOtherSlot = nSlot == 0 ? 2 : 0;
                aControls.Add(CreateItemEntry(OBJECT_INVALID, -1));
                if (m_apItemEntries[nSlot] != 0)
                    aControls.Add(CreateItemEntry(m_apItemEntries[nSlot]->GetId(), -1));
                for (int i = 0; i < m_pUpgradeTable->GetNumRows(); i++)
                {
                    int nUpgradeGroup;
                    m_pUpgradeTable->GetINTEntry(i, "group", &nUpgradeGroup);
                    if (nUpgradeGroup == nSlot)
                    {
                        m_pUpgradeTable->GetCExoStringEntry(i, "template", &sTemplate);
                        unsigned long oidItem = pRepository->FindItemWithTag(sTemplate.LowerCase());
                        if (oidItem != OBJECT_INVALID &&
                            (m_apItemEntries[nOtherSlot] == 0 || i != m_anUpgradeProperties[nOtherSlot]) &&
                            (m_apItemEntries[nSlot] == 0 || i != m_anUpgradeProperties[nSlot]))
                            aControls.Add(CreateItemEntry(oidItem, i));
                    }
                }
            }
            else
            {
                CExoString sTemplate;
                int nOtherSlot = nSlot == 0 ? 2 : 0;
                aControls.Add(CreateItemEntry(OBJECT_INVALID, -1));
                if (m_apItemEntries[nSlot] != 0)
                    aControls.Add(CreateItemEntry(m_apItemEntries[nSlot]->GetId(), -1));
                for (int i = 0; i < m_pUpgradeTableEx->GetNumRows(); i++)
                {
                    int nUpgradeGroup;
                    m_pUpgradeTableEx->GetINTEntry(i, "group", &nUpgradeGroup);
                    if (nUpgradeGroup == nGroup)
                    {
                        m_pUpgradeTableEx->GetCExoStringEntry(i, "template", &sTemplate);
                        unsigned long oidItem = pRepository->FindItemWithTag(sTemplate.LowerCase());
                        if (oidItem != OBJECT_INVALID &&
                            (m_apItemEntries[nOtherSlot] == 0 || i != m_anUpgradeProperties[nOtherSlot]) &&
                            (m_apItemEntries[nSlot] == 0 || i != m_anUpgradeProperties[nSlot]))
                            aControls.Add(CreateItemEntry(oidItem, i));
                    }
                }
            }
            m_lbItems.AddControls(aControls, 1, 0, 0);
            ShowItems(1);
            SetActiveControl(&m_lbItems, 0);
            m_pItemListControl = pControl;
            m_bItemListShown = 1;
        }
        else
        {
            CExoArrayList<CSWGuiControl*> aControls;
            {
            CExoString sTemplate;
            int nOtherSlot = nSlot == 0 ? 2 : 0;
            aControls.Add(CreateItemEntry(OBJECT_INVALID, -1));
            if (m_apItemEntries[nSlot] != 0)
                aControls.Add(CreateItemEntry(m_apItemEntries[nSlot]->GetId(), -1));
            for (int i = 0; i < m_pUpgradeTable->GetNumRows(); i++)
            {
                int nUpgradeGroup;
                m_pUpgradeTable->GetINTEntry(i, "group", &nUpgradeGroup);
                if (nUpgradeGroup == nSlot)
                {
                    m_pUpgradeTable->GetCExoStringEntry(i, "template", &sTemplate);
                    unsigned long oidItem = pRepository->FindItemWithTag(sTemplate.LowerCase());
                    if (oidItem != OBJECT_INVALID &&
                        (m_apItemEntries[nOtherSlot] == 0 || i != m_anUpgradeProperties[nOtherSlot]) &&
                        (m_apItemEntries[nSlot] == 0 || i != m_anUpgradeProperties[nSlot]))
                    {
                        g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(oidItem);
                        aControls.Add(CreateItemEntry(oidItem, i));
                    }
                }
            }
            }
            m_lbItems.AddControls(aControls, 1, 0, 0);
            ShowItems(1);
            SetActiveControl(&m_lbItems, 0);
            m_pItemListControl = pControl;
            m_bItemListShown = 1;
        }
    }
}
