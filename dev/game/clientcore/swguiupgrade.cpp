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
// PC 0x007292B0
void CSWGuiUpgrade::OnUpgradeSelected(CSWGuiControl* pControl)
{
    if (!pControl->m_bSelected)
        return;
    if (pControl->GetEvent() == GUIEVENT_ACCEPT)
        m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
    EXOXBOXASSERT(m_nItemListMode == ITEMLIST_LIGHTSABER);

    EXOXBOXASSERT(m_nItemListMode == 1);
    CSWUpgradeItemEntry* pEntry = (CSWUpgradeItemEntry*)pControl;
    CServerExoApp* pServer = g_pAppManager->GetServerExoApp();
    CItemRepository* pRepository = pServer->GetPartyTable()->GetPartyItems();
    int nSlot = m_pItemListControl->m_nIndex;
    if (nSlot == UPGRADE_SLOT_COLOR_CRYSTAL)
    {
        if (pEntry->m_nUpgradeIndex == -1)
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
        unsigned char nUpgradeIndex = pEntry->m_nUpgradeIndex;
        m_pCrystalTable->GetCExoStringEntry(nUpgradeIndex,
                                           sColumn, &sTemplate);
        CSWSItem* pItem = new CSWSItem(OBJECT_INVALID);
#ifdef _XBOX
        pItem->LoadFromTemplate(sTemplate);
#else
        CResRef resTemplate(sTemplate);
        pItem->LoadFromTemplate(resTemplate);
#endif
        pItem->SetPossessor(pServer->GetPlayerCreatureId(), 0, 0, OBJECT_INVALID);
        pItem->SetIdentified(1);
        int nUpgrades = m_pSelectedItem->m_nUpgrades;
        pItem->m_nUpgrades = nUpgrades;
        unsigned long bStolen = m_pSelectedItem->m_bStolen;
        pItem->m_bStolen = bStolen;
        delete m_pSelectedItem;
        m_pSelectedItem = pItem;
        Update3DView();
    }
    else
    {
        if (pEntry->m_nUpgradeIndex == -1 &&
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
        pOldItem = 0;
        if (nSlot != UPGRADE_SLOT_COLOR_CRYSTAL)
            m_pSelectedItem->m_nUpgrades &= ~(CSWSItem::UPGRADE_PROPERTY_MASK << m_anUpgradeProperties[nSlot]);
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
        m_apItemEntries[nSlot] = pItem;
        if (nSlot != UPGRADE_SLOT_COLOR_CRYSTAL)
        {
            m_anUpgradeProperties[nSlot] = pEntry->m_nUpgradeIndex;
            int nUpgradeMask = CSWSItem::UPGRADE_PROPERTY_MASK << m_anUpgradeProperties[nSlot];
            m_pSelectedItem->m_nUpgrades |= nUpgradeMask;
        }
    }
Exit:
    CloseItems();
}
// PC 0x007297E0
CSWUpgradeItemEntry* CSWGuiUpgrade::CreateItemEntry(unsigned long oidItem, int nUpgradeIndex)
{
    int nWidth = m_lbItems.m_cItemExtent.m_nWidth -
                 2 * m_lbItems.m_nPadding;
    CSWGuiButton* pProto = (CSWGuiButton*)m_lbItems.m_pProtoItem;
    CSWUpgradeItemEntry* pEntry = new CSWUpgradeItemEntry;

    pEntry->Initialize(pProto->m_cText, pProto->m_cBorder, pProto->m_cHilite, nWidth);
    pEntry->AddEvent(CSWGuiControl::EVENT_ENTER, this, (CSWGuiHandler)&CSWGuiUpgrade::OnControlEntered);
    pEntry->AddEvent(CSWGuiControl::EVENT_SELECT, this, (CSWGuiHandler)&CSWGuiUpgrade::OnUpgradeSelected);
    pEntry->AddEvent(CSWGuiControl::EVENT_CONFIRM, this, (CSWGuiHandler)&CSWGuiUpgrade::OnUpgradeSelected);
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
    { { 0, "i_powerc", 36977 }, { -1, "i_colorc", 36978 },
      { 0, "i_powerc", 36977 }, { -1, "", 0 } },
    { { 4, "i_scope", 32487 }, { 5, "i_energy", 32488 },
      { 6, "i_beam", 32489 }, { 7, "i_hair", 32490 } },
    { { 1, "i_vcell", 32493 }, { 2, "i_durasteel", 32494 },
      { 3, "i_imp_eng", 32495 }, { -1, "", 0 } },
    { { -1, "", 0 }, { 8, "i_armorrein", 32491 },
      { 9, "i_armorrein", 32492 }, { -1, "", 0 } }
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
        cCountExtent.m_nWidth = m_CSWGuiText_334.m_cParams.m_sText.GetLength() > 2 ? 42 : 21;
        cCountExtent.m_nHeight = ENTRY_HEIGHT;
        cCountExtent.m_nLeft += ENTRY_HEIGHT - cCountExtent.m_nWidth;
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
                                     CSWGuiBorder& cHilite, int nWidth)
{
    m_cText.m_cParams = cText.m_cParams;
    CSWGuiBorderParams* pParams = &m_cBorder.m_cParams;
    *pParams = cBorder.m_cParams;
    pParams->m_vColor = CGuiInGame::COLOR_BLUE;
    pParams = &m_cHilite.m_cParams;
    *pParams = cHilite.m_cParams;
    pParams->m_vColor = CGuiInGame::COLOR_YELLOW;
    pParams->SetPulsingAlpha(1, 1, 0);
    pParams = &m_CSWGuiBorder_1D8.m_cParams;
    {
#ifndef _XBOX
        CResRef cFill("lbl_hex_3"); CResRef cEdge(""); CResRef cCorner(""); *pParams = CSWGuiBorderParams(cCorner, cEdge, cFill, 0, 0, 1.0f, Vector(1.0f, 1.0f, 1.0f), 0);
#else
        *pParams = CSWGuiBorderParams("", "", "lbl_hex_3", 0, 0, 1.0f, Vector(1.0f, 1.0f, 1.0f), 0);
#endif
        pParams->m_vColor = CGuiInGame::COLOR_BLUE;
    }
    pParams = &m_CSWGuiBorder_24C.m_cParams;
    {
#ifndef _XBOX
        CResRef cFill("lbl_hex_3"); CResRef cEdge(""); CResRef cCorner(""); *pParams = CSWGuiBorderParams(cCorner, cEdge, cFill, 0, 0, 1.0f, Vector(1.0f, 1.0f, 1.0f), 0);
#else
        *pParams = CSWGuiBorderParams("", "", "lbl_hex_3", 0, 0, 1.0f, Vector(1.0f, 1.0f, 1.0f), 0);
#endif
        pParams->m_vColor = CGuiInGame::COLOR_YELLOW;
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
// PC 0x00727F30
CSWUpgradeItemEntry::CSWUpgradeItemEntry()
{
    m_bShowItemSuffix = 0;
    m_oidItem = OBJECT_INVALID;
    m_nUpgradeIndex = -1;
}
// PC 0x00733EB0
CSWGuiUpgradeSelection::CSWGuiUpgradeSelection(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{

#ifdef _XBOX
    StartLoadFromLayout("upgradesel");
#else
    {
        CResRef cLayout("upgradesel");
        StartLoadFromLayout(cLayout);
    }
#endif
    InitControl(m_lblTitle, "LBL_TITLE", 1);
    InitControl(m_aTypeButtons[0], "BTN_LIGHTSABER", 1);
    InitControl(m_aTypeButtons[1], "BTN_RANGED", 1);
    InitControl(m_aTypeButtons[2], "BTN_MELEE", 1);
    InitControl(m_aTypeButtons[3], "BTN_ARMOR", 1);
    InitControl(m_aTypeLabels[0], "LBL_LSABER", 1);
    InitControl(m_aTypeLabels[1], "LBL_RANGED", 1);
    InitControl(m_aTypeLabels[2], "LBL_MELEE", 1);
    InitControl(m_aTypeLabels[3], "LBL_ARMOR", 1);
#ifdef _XBOX
    InitControl(m_lblA, "LBL_A", 1);
    InitControl(m_lblB, "LBL_B", 1);
    InitControl(m_lblBack, "LBL_BACK", 1);
    InitControl(m_lblUpgradeitems, "LBL_UPGRADEITEMS", 1);
#else
    InitControl(m_btnBack, "BTN_BACK", 1);
    InitControl(m_btnUpgradeitems, "BTN_UPGRADEITEMS", 1);
#endif
    StopLoadFromLayout();

    m_oidItem = OBJECT_INVALID;
    m_pItemSelect = new CSWGuiUpgradeItemSelect(pManager);
    m_pItemSelect->m_pSelectionPanel = this;
    for (int i = 0; i < 4; i++)
    {
        m_aTypeButtons[i].AddEvent(GUIEVENT_ACTIVATE, this,
                                  (CSWGuiHandler)&CSWGuiUpgradeSelection::OnTypeSelected);
        m_aTypeButtons[i].AddEvent(GUIEVENT_ACCEPT, this,
                                  (CSWGuiHandler)&CSWGuiUpgradeSelection::OnTypeSelected);
        m_aTypeButtons[i].AddEvent(GUIEVENT_MOUSE_ENTER, this,
                                  (CSWGuiHandler)&CSWGuiUpgradeSelection::OnEnter);
        m_aTypeButtons[i].m_nIndex = i;
        m_aTypeButtons[i].m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_BLUE;
        m_aTypeButtons[i].m_cHilite.m_cParams.m_vColor = CGuiInGame::COLOR_YELLOW;
        m_aTypeButtons[i].m_cHilite.m_cParams.SetPulsingAlpha(1, 1, 0);
        m_aTypeLabels[i].SetEnabledFlag(0);
    }
#ifndef _XBOX
    m_btnBack.SetFocusable(0);
    m_btnBack.AddEvent(GUIEVENT_ACTIVATE, this,
                      (CSWGuiHandler)&CSWGuiPanel::OnBButtonPressed);
    m_btnUpgradeitems.SetFocusable(0);
    m_btnUpgradeitems.AddEvent(GUIEVENT_ACTIVATE, this,
                              (CSWGuiHandler)&CSWGuiUpgradeSelection::OnTypeSelected);
    m_pSelectedType = 0;
#endif
}
// PC 0x00731CF0
CSWGuiUpgradeItemSelect::CSWGuiUpgradeItemSelect(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{

#ifdef _XBOX
    StartLoadFromLayout("upgradeitems");
#else
    {
        CResRef cLayout("upgradeitems");
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
#ifndef _XBOX
    SetActiveControl(&m_lbItems, 0);
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
                               pProto->m_cHilite, nWidth);
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
        int bCanEquip = 1;
        if (!pCreature->CanEquipItem(pItem, &nSlot, 0, 0, 0))
        {
            pServer->GetPartyTable()->GetPartyItems()->AddItem(&pItem, 1, 0, 0);
            m_pUpgradeItem->SetItem(pItem->m_nId, 0, OBJECT_INVALID, 0);
            bCanEquip = 0;
        }
        if (m_bDualWield)
        {
            if (m_bLeftHandItem)
            {
                pCreature->EquipItem(INVENTORY_SLOT_RIGHTHAND,
                    pServer->GetItemByGameObjectID(m_oidOtherWeapon), 1, 0, 0);
                if (bCanEquip)
                {
                    pCreature->EquipItem(INVENTORY_SLOT_LEFTHAND, pItem, 1, 0, 0);
                }
            }
            else
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
    if (m_pPresetItem != 0)
    {
        SetModal(0);
        m_pManager->PopModalPanel();
        m_pSelectionPanel->AutoUpgradeDone();
        m_pPresetItem = 0;
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
            m_cText.m_cParams.SetText(sName);
            CExoString sCount;
            int nCount = pItem->m_nStackSize;
            if (nCount > 1)
            {
                if (nCount >= 100)
                {
                    m_CSWGuiBorder_1D8.m_cParams.SetFillImage("lbl_hex_7", 0);
                    m_CSWGuiBorder_24C.m_cParams.SetFillImage("lbl_hex_7", 0);
                }
                else
                {
                    m_CSWGuiBorder_1D8.m_cParams.SetFillImage("lbl_hex_6", 0);
                    m_CSWGuiBorder_24C.m_cParams.SetFillImage("lbl_hex_6", 0);
                }
                sCount.Format("%d", nCount);
            }
            else
            {
                m_CSWGuiBorder_1D8.m_cParams.SetFillImage("lbl_hex_3", 0);
                m_CSWGuiBorder_24C.m_cParams.SetFillImage("lbl_hex_3", 0);
            }
            m_CSWGuiText_334.m_cParams = CSWGuiTextParams(sCount, "fnt_d16x16",
                CGuiInGame::COLOR_BLUE, CSWGuiTextParams::ALIGNMENT_ITEM_COUNT, 1.0f);
            cIcon = pItem->GetIcon();
        }
        else
        {
            m_CSWGuiBorder_1D8.m_cParams.SetFillImage("lbl_hex_3", 0);
            m_CSWGuiBorder_24C.m_cParams.SetFillImage("lbl_hex_3", 0);
            m_cText.m_cParams.SetText(g_pAppManager->GetClientExoApp()->GetGUIString(CClientExoApp::GUI_STRREF_NONE));
            m_CSWGuiText_334.m_cParams = CSWGuiTextParams(CExoString(), "fnt_d16x16",
                CGuiInGame::COLOR_BLUE, CSWGuiTextParams::ALIGNMENT_ITEM_COUNT, 1.0f);
            cIcon = "inone";
        }
    }
    else
    {
        m_CSWGuiBorder_1D8.m_cParams.SetFillImage("lbl_hex_3", 0);
        m_CSWGuiBorder_24C.m_cParams.SetFillImage("lbl_hex_3", 0);
        m_cText.m_cParams.SetText(g_pAppManager->GetClientExoApp()->GetGUIString(CClientExoApp::GUI_STRREF_NONE));
        m_CSWGuiText_334.m_cParams = CSWGuiTextParams(CExoString(), "fnt_d16x16",
            CGuiInGame::COLOR_BLUE, CSWGuiTextParams::ALIGNMENT_ITEM_COUNT, 1.0f);
        cIcon = "inone";
    }
    CSWGuiBorderParams* pParams = &m_CSWGuiBorder_2C0.m_cParams;
    pParams->m_nFillMode = CSWGuiBorderParams::FILL_MODE_BACKGROUND;
    pParams->SetFillImage(cIcon, 0);
}
// PC 0x00726EE0
CSWGuiUpgradeSelection::~CSWGuiUpgradeSelection()
{
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
    if (m_oidItem != OBJECT_INVALID)
    {
        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
        CSWSItem* pItem = pServer->GetItemByGameObjectID(m_oidItem);
        if (pItem != 0)
        {
            int nType = pItem->GetUpgradableType(pTable);
            m_pItemSelect->m_nUpgradeType = nType;
            m_pItemSelect->m_pPresetItem = pItem;
            m_pManager->AddPanel(m_pItemSelect, CSWGuiManager::PANEL_MODE_MODAL, 1);
            goto Exit;
        }
    }
    {
        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
        CSWPartyTable* pParty = pServer->GetPartyTable();
        CItemRepository* pItems = pParty->GetPartyItems();
        for (int i = 0; i < 4; i++)
        {
            m_anUpgradable[i] = 0;
        }
#ifdef _XBOX
        unsigned long oidPlayer = pServer->GetPlayerList()->GetHead()->AsSWSPlayer()->m_oidPCObject;
        CSWSCreature* pCreature = pServer->GetCreatureByGameObjectID(oidPlayer);
#else
        CSWSPlayer* pPlayer = pServer->GetPlayerList()->GetHead()->AsSWSPlayer();
        CSWSCreature* pCreature = pServer->GetCreatureByGameObjectID(pPlayer->m_oidPCObject);
#endif
        GetUpgradableItems(pCreature, m_anUpgradable, pTable);
        for (unsigned int nNPC = 0; nNPC < CSWPartyTable::MAX_NPCS; nNPC++)
        {
            if (pParty->GetIsNPCAvailable(nNPC) && pParty->GetNPCSelectability(nNPC))
            {
                pCreature = pServer->GetCreatureByGameObjectID(pParty->GetNPCObject(nNPC, 0, 1));
                if (pCreature != 0)
                {
                    GetUpgradableItems(pCreature, m_anUpgradable, pTable);
                }
            }
        }
        for (unsigned int nItem = 0; nItem < pItems->GetNumItems(); nItem++)
        {
            CSWSItem* pItem = pItems->ItemListGetItem(nItem);
            if (pItem != 0)
            {
                int nType = pItem->GetUpgradableType(pTable);
                if (nType > 0)
                {
                    m_anUpgradable[nType - 1] = 1;
                }
            }
        }
        for (int nType = 0; nType < 4; nType++)
        {
            m_aTypeLabels[nType].SetEnabledFlag(m_anUpgradable[nType]);
            if (m_anUpgradable[nType] != 0)
            {
                m_aTypeButtons[nType].m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
#ifndef _XBOX
                m_aTypeButtons[nType].SetEnabled(1);
#endif
            }
            else
            {
                m_aTypeButtons[nType].m_cText.m_cParams.SetColor(CGuiInGame::DISABLED_TEXT);
#ifndef _XBOX
                m_aTypeButtons[nType].SetEnabled(0);
#endif
            }
        }
        SetActiveControl(&m_aTypeButtons[0], 0);
    }
Exit:
    delete pTable;
#ifndef _XBOX
    CSWGuiPanel::OnPanelAdded();
#endif
}

void CSWGuiUpgradeSelection::GetUpgradableItems(CSWSCreature* pCreature,
                                                int* pnUpgradable, C2DA* pTable)
{
    int i;
    CSWInventory* pInventory;
    CSWSItem* pItem;
    int nType;

    for (i = 0; i < 18; i++)
    {
        pInventory = pCreature->m_pInventory;
        pItem = pInventory->GetItemInSlot(1 << i);
        if (pItem != 0)
        {
            nType = pItem->GetUpgradableType(pTable);
            if (nType > 0)
            {
                pnUpgradable[nType - 1] = 1;
            }
        }
    }
}

void CSWGuiUpgradeSelection::AutoUpgradeDone()
{
    g_pAppManager->m_pClientExoApp->GetInGameGui()->HideUpgradeScreen();
}
// PC 0x00723CF0
void CSWGuiUpgradeSelection::HandleInputEvent(int nEvent, int nState)
{
    CGuiInGame* pInGame;

    if (nState != 0 && (nEvent == 40 || nEvent == 46))
    {
        m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
        pInGame = (CGuiInGame*)g_pAppManager->m_pClientExoApp->GetInGameGui();
        pInGame->HideUpgradeScreen();
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
// PC 0x00723D50
void CSWGuiUpgradeSelection::OnTypeSelected(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        if (pControl->m_nEvent == 45)
        {
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
        }
#ifndef _XBOX
        if (pControl != &m_btnUpgradeitems ||
            (pControl = m_pSelectedType) != 0)
#endif
        {
            int nType = pControl->m_nIndex;
            if (m_anUpgradable[nType] != 0)
            {
                m_pItemSelect->m_nUpgradeType = nType + 1;
                m_pManager->AddPanel(m_pItemSelect, CSWGuiManager::PANEL_MODE_MODAL, 1);
            }
        }
    }
}

void CSWGuiUpgradeSelection::OnEnter(CSWGuiControl* pControl)
{
    int nType = pControl->m_nIndex;
#ifdef _XBOX
    if (nType >= 0 && nType < 4)
    {
        if (m_anUpgradable[nType] != 0)
        {
            m_lblUpgradeitems.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
        }
        else
        {
            m_lblUpgradeitems.m_cText.m_cParams.SetColor(CGuiInGame::DISABLED_TEXT);
        }
    }
#else
    if (pControl->m_bSelected != 0)
    {
        CSWGuiRadioButton* pButton = pControl->AsRadioButton();
        if (pButton != 0)
        {
            if (m_anUpgradable[nType] != 0)
            {
                m_btnUpgradeitems.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
                m_btnUpgradeitems.m_cBorder.m_cParams.m_vColor = CGuiInGame::ENABLED_ICON_COLOR;
                m_pSelectedType = pButton;
            }
            else
            {
                m_btnUpgradeitems.m_cText.m_cParams.SetColor(CGuiInGame::DISABLED_TEXT);
                m_btnUpgradeitems.m_cBorder.m_cParams.m_vColor = CGuiInGame::DISABLED_ICON_COLOR;
                m_pSelectedType = 0;
            }
        }
    }
#endif
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

#ifdef _XBOX

inline int CSWGuiListBox::IsOversized()
{
    return m_nContentHeight + m_nPadding > m_cItemExtent.m_nHeight &&
           m_lItems.num > 0 && !m_bHideScrollIcon;
}
#endif
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
            pEntry->Initialize(pProto->m_cText, pProto->m_cBorder, pProto->m_cHilite, nWidth);
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
        m_3dModelLs.SetShown(1);
        m_cLightsaberSlotName.SetShown(1);
        m_cAssemble.SetShown(1);
        m_cLightsaberDescriptionBackground.SetShown(1);
        m_lbDescLs.SetShown(1);
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
        SetActiveControl(&m_aUpgradeButtons3[0], 0);
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
        m_3dModelLs.SetShown(0);
        m_cLightsaberSlotName.SetShown(0);
        m_cLightsaberDescriptionBackground.SetShown(0);
        m_lbDescLs.SetShown(0);
        switch (m_nItemListMode)
        {
        case ITEMLIST_BLASTER:
            for (int i = 0; i < 4; i++)
            {
                m_aUpgradeButtons4[i].SetShown(1);
                m_aUpgradeLabels4[i].SetShown(1);
            }
            SetActiveControl(&m_aUpgradeButtons4[0], 0);
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
            SetActiveControl(&m_aUpgradeButtons4[1], 0);
            break;
        case ITEMLIST_MELEE:
            for (int i = 0; i < 4; i++)
            {
                m_aUpgradeButtons4[i].SetShown(0);
                m_aUpgradeLabels4[i].SetShown(0);
            }
            SetActiveControl(&m_aUpgradeButtons4[0], 0);
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
        CResRef cLayout("upgrade");
        StartLoadFromLayout(cLayout);
    }
#endif
    InitControl(m_cTitle, "LBL_TITLE", 1);
    InitControl(m_lbItems, "LB_ITEMS", 1);
    InitControl(m_lbDesc, "LB_DESC", 1);
    m_lbDesc.m_nSelectSound = CSWGuiManager::GUISOUND_SELECTION;
    InitControl(m_lbDescLs, "LB_DESC_LS", 1);

    m_lbDesc.m_nSelectSound = CSWGuiManager::GUISOUND_SELECTION;
    InitControl(m_cDescriptionBackground, "LBL_DESCBG", 1);
    InitControl(m_cLightsaberDescriptionBackground, "LBL_DESCBG_LS", 1);
    InitControl(m_3dModel, "3D_MODEL", 1);
    InitControl(m_3dModelLs, "3D_MODEL_LS", 1);
    InitControl(m_cSlotName, "LBL_SLOTNAME", 1);
    InitControl(m_cLightsaberSlotName, "LBL_LSSLOTNAME", 1);
    InitControl(m_cUpgrades, "LBL_UPGRADES", 1);
    InitControl(m_cUpgradeCount, "LBL_UPGRADE_COUNT", 1);
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
#endif
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
    StopLoadFromLayout();
    m_cSlotName.m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_BLUE;
    m_cLightsaberSlotName.m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_BLUE;
    m_cUpgradeCount.m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_BLUE;
    m_cUpgrades.m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_BLUE;
    m_cProperty.m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_BLUE;
    m_lbItems.m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_BLUE;
#ifndef _XBOX
    m_CSWGuiLabel_1F60.Initialize(m_lbDescLs.m_pProtoItem->m_cExtent, (CSWGuiLabel*)m_lbDescLs.m_pProtoItem, 1.0f);
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
    for (i = 0; i < NUM_UPGRADE_SLOTS; i++)
        m_apItemEntries[i] = 0;
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
        pLabels = m_aUpgradeLabels4;
        break;
    case ITEMLIST_ARMOR:
        m_aUpgradeButtons4[1].SetMoveToControl(1, &m_aUpgradeButtons4[2]);
        m_aUpgradeButtons4[2].SetMoveToControl(3, &m_aUpgradeButtons4[1]);
        nFirstSlot = 1;
        pLabels = m_aUpgradeLabels4;
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
            CExoLocString& sItemDescription = pItem->m_sDescIdentified;
            sItemDescription.GetString(g_pAppManager->GetClientExoApp()->GetClientLanguage(),
                                       &sDescription, 0);
#ifdef _XBOX
            if (sDescription != "")
                SetDescription(sDescription);
            else
                SetDescription(g_pAppManager->GetClientExoApp()->GetGUIString(STRREF_NO_DESCRIPTION));
#else
            if (sDescription == "")
                sDescription = g_pAppManager->GetClientExoApp()->GetGUIString(STRREF_NO_DESCRIPTION);
            int nSlot = (int)m_pItemListControl->GetUserData();
            if (m_nItemListMode == ITEMLIST_LIGHTSABER && nSlot != UPGRADE_SLOT_COLOR_CRYSTAL)
            {
                unsigned char nUpgradeIndex = pEntry->m_nUpgradeIndex;
                sDescription = m_pSelectedItem->GetKeyedPropertyString(nUpgradeIndex) + sDescription;
            }
            SetDescription(sDescription);
#endif
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
            ~(1 << m_anUpgradeProperties[nSlot]);
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
                pCreature->UnequipItem(m_pPresetItem);
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
            if (nSlot == UPGRADE_SLOT_COLOR_CRYSTAL)
            {
                CExoString sTemplate;
                aControls.Add(CreateItemEntry(m_apItemEntries[UPGRADE_SLOT_COLOR_CRYSTAL]->GetId(), -1));
                for (int i = 0; i < m_pCrystalTable->GetNumRows(); i++)
                {
                    m_pCrystalTable->GetCExoStringEntry(i, "Template", &sTemplate);
                    sTemplate = sTemplate.LowerCase();
                    if (sTemplate != m_apItemEntries[UPGRADE_SLOT_COLOR_CRYSTAL]->m_sTag)
                    {
                        unsigned long oidItem = pRepository->FindItemWithTag(sTemplate);
                        if (oidItem != OBJECT_INVALID)
                            aControls.Add(CreateItemEntry(oidItem, i));
                    }
                }
            }
            else
            {
                CExoString sTemplate;
                int nOtherSlot = nSlot == UPGRADE_SLOT_POWER_CRYSTAL_1
                    ? UPGRADE_SLOT_POWER_CRYSTAL_2 : UPGRADE_SLOT_POWER_CRYSTAL_1;
                aControls.Add(CreateItemEntry(OBJECT_INVALID, -1));
                if (m_apItemEntries[nSlot] != 0)
#ifdef _XBOX
                    aControls.Add(CreateItemEntry(m_apItemEntries[nSlot]->GetId(), -1));
#else
                    aControls.Add(CreateItemEntry(m_apItemEntries[nSlot]->GetId(), m_anUpgradeProperties[nSlot]));
#endif
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
                            aControls.Add(CreateItemEntry(oidItem, i));
                    }
                }
            }
#ifndef _XBOX
            m_pItemListControl = pControl;
#endif
            m_lbItems.AddControls(aControls, 1, 0, 0);
            ShowItems(1);
            SetActiveControl(&m_lbItems, 0);
#ifdef _XBOX
            m_pItemListControl = pControl;
#endif
            m_bItemListShown = 1;
        }
        else
        {
            CSWSItem*& pOldItem = m_apItemEntries[nSlot];
            if (pOldItem != 0)
            {
                m_pSelectedItem->m_nUpgrades &=
                    ~(CSWSItem::UPGRADE_PROPERTY_MASK << m_anUpgradeProperties[nSlot]);
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
                if (pItem == 0)
                    return;
                CSWSCreature* pCreature = 0;
                unsigned long nEquipSlot = 0;
                if (m_pItemSelect->GetUpgradeItemEquipped())
                {
                    nEquipSlot = m_pItemSelect->GetUpgradeItemProperty();
                    pCreature = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(
                        m_pItemSelect->GetUpgradeItemId());
                }
                m_pSelectedItem->m_nUpgrades |=
                    CSWSItem::UPGRADE_PROPERTY_MASK << m_anUpgradeProperties[nSlot];
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
            if (m_nItemListMode == ITEMLIST_LIGHTSABER)
            {
                m_lbDescLs.HandleInputEvent(GUIEVENT_DOWN, 1);
            }
            else
            {
                m_lbDesc.HandleInputEvent(GUIEVENT_DOWN, 1);
            }
            break;
        case GUIEVENT_SCROLL_UP:
            if (m_nItemListMode == ITEMLIST_LIGHTSABER)
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
    if (m_nItemListMode == ITEMLIST_LIGHTSABER)
    {
        pListBox = &m_lbDescLs;
#ifdef _XBOX
        pRanalog = &m_lblRanalogLs;
#endif
    }
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
    int nBody = max(1, min(10, nBodyVariation));
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
    int nFirstSlot = 0;
    CSWGuiLabel* pLabels;
    switch (m_nItemListMode)
    {
    case ITEMLIST_LIGHTSABER:
    case ITEMLIST_MELEE:
        pLabels = m_aUpgradeLabels3;
        break;
    case ITEMLIST_ARMOR:
        nFirstSlot = 1;
    case ITEMLIST_BLASTER:
        pLabels = m_aUpgradeLabels4;
        break;
    }
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
    for (int i = nFirstSlot; i < nEndSlot; i++)
    {
        if (m_apItemEntries[i] != 0)
        {
            CSWBaseItem* pBaseItem = g_pRules->m_pBaseItems->GetBaseItem(
                m_apItemEntries[i]->GetBaseItemType());
            cIcon = m_apItemEntries[i]->GetIcon();
        }
        else
        {
            cIcon = SLOT_INFO[m_nItemListMode - 1][i].pszIcon;
        }
        pLabels[i].m_cBorder.m_cParams.SetFillImage(cIcon, 0);
    }
    SetDescription(m_pSelectedItem->GetPropertyDescription());
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
                m_cLightsaberSlotName.m_cText.m_cParams.SetText(sName);
            }
            else
                m_cLightsaberSlotName.m_cText.m_cParams.SetStrRef(nStrRef);
#ifdef _XBOX
            m_lblAAction.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
            m_lblAAction.m_cText.m_cParams.SetStrRef(36841);
#endif
        }
        else
        {
            int nCount = 0;
            CServerExoApp* pServer = g_pAppManager->GetServerExoApp();
            CSWSItem* pItem = pServer->GetItemByGameObjectID(
                pServer->GetPartyTable()->GetPartyItems()->FindItemWithTag(
                    m_asUpgradeTemplates[nSlot]));
            if (pItem != 0)
                nCount = pItem->m_nStackSize;
            m_cSlotName.m_cText.m_cParams.SetStrRef(nStrRef);
            {
                CExoString sCount(nCount);
                m_cUpgradeCount.m_cText.m_cParams.SetText(sCount);
            }
            m_cUpgrades.m_cText.m_cParams.SetStrRef(STRREF_UPGRADES);
            CSWSItem* pSelectedItem = m_pSelectedItem;
            m_cProperty.m_cText.m_cParams.SetText(
                pSelectedItem->GetKeyedPropertyString(m_anUpgradeProperties[nSlot]));
#ifdef _XBOX
            if (nCount == 0 && m_apItemEntries[nSlot] == 0)
            {
                m_lblAAction.m_cText.m_cParams.SetColor(CGuiInGame::DISABLED_TEXT);
                m_lblAAction.m_cText.m_cParams.SetStrRef(31387);
            }
            else
            {
                m_lblAAction.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
                m_lblAAction.m_cText.m_cParams.SetStrRef(m_apItemEntries[nSlot] != 0 ? 36840 : 31387);
            }
#endif
        }
    }
}

#ifdef _XBOX
void CSWGuiUpgrade::OnEnterAssemble(CSWGuiControl* pControl)
{
    m_lblAAction.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
    m_lblAAction.m_cText.m_cParams.SetStrRef(42021);
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
