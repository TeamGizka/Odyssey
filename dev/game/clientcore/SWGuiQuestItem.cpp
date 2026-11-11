#include "exobase.h"

#include "appmanager.h"
#include "clientexoapp.h"
#include "serverexoapp.h"
#include "nwsobject.h"
#include "nwsitem.h"
#include "swparty.h"
#include "nwccreature.h"
#include "swguiingame.h"
#include "swguiingameequip.h"
// PC 0x006C00B0
void CSWGuiQuestItem::Draw(float fElapsed)
{
    CSWGuiPanel::Draw(fElapsed);
}
// PC 0x006D24D0  Xbox 0x00120EE0
void CSWGuiQuestItem::HandleInputEvent(int nEvent, int nState)
{
    if (nState != 0)
    {
        switch (nEvent)
        {
        case GUIEVENT_CANCEL:
        case GUIEVENT_CANCEL_ALTERNATE:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            m_pManager->PopModalPanel();
            SetModal(0);
            break;
        case GUIEVENT_SCROLL_UP:
            m_lbxDescription.HandleInputEvent(GUIEVENT_UP, 1);
            break;
        case GUIEVENT_SCROLL_DOWN:
            m_lbxDescription.HandleInputEvent(GUIEVENT_DOWN, 1);
            break;
        }
    }

    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
// PC 0x006D2580  Xbox 0x00120F90
void CSWGuiQuestItem::OnControlEntered(CSWGuiControl* pControl)
{
    if (pControl != 0 && pControl->m_bSelected != 0)
    {
        m_pSelectedItemControl = pControl;

        CSWSItem* pItem = g_pAppManager->m_pServerExoApp->GetItemByGameObjectID(
            static_cast<CSWGuiInGameItemEntry*>(pControl)->m_oidItem);
        if (pItem != 0)
        {
            m_lblDescription.m_cText.m_cParams.SetText(pItem->GetPropertyDescription());
        }
        else
        {
            CExoString sDescription("Error: Invalid item");
            m_lblDescription.m_cText.m_cParams.SetText(sDescription);
        }
    }

    CSWGuiExtent cExtent = m_lblDescription.m_cExtent;
    if (m_lblDescription.m_cText.m_cParams.m_sText.GetLength() > 0)
    {
        cExtent.m_nHeight = m_lblDescription.m_cText.GetIdealHeight();
    }
    else
    {
        cExtent.m_nHeight = m_lblDescription.m_cText.GetFontHeight();
    }
    m_lblDescription.SetExtent(cExtent);

    CSWGuiControl* pLabel = &m_lblDescription;
    m_lbxDescription.AddControls(&pLabel, 1, 0, 0, 0);
    m_lbxDescription.SetSelectedControl(&m_lblDescription, 0);
}
// PC 0x006D26C0  Xbox 0x00121450
CSWGuiQuestItem::CSWGuiQuestItem(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    m_pSelectedItemControl = 0;

#ifdef _XBOX
    StartLoadFromLayout("QuestItem");
#else
    {
        CResRef cLayout("QuestItem");
        StartLoadFromLayout(cLayout);
    }
#endif
    CSWGuiListBox* pItems = &m_lbxItems;
    InitControl(*pItems, "LB_ITEMS", 1);
#ifdef _XBOX
    InitControl(m_lblB, "LBL_B", 1);
    InitControl(m_lblBack, "LBL_BACK", 1);
    InitControl(m_lbxDescription, "LB_ITEM_DESCRIPTION", 1);
    InitControl(m_lblTitle, "LBL_TITLE", 1);
    InitControl(m_lblRAnalog, "LBL_RANALOG", 1);
#else
    InitControl(m_lbxDescription, "LB_ITEM_DESCRIPTION", 1);
    InitControl(m_lblTitle, "LBL_TITLE", 1);
    CSWGuiButton* pBack = &m_btnBack;
    InitControl(*pBack, "BTN_BACK", 1);
#endif

    CSWGuiLabel* pProtoLabel = (CSWGuiLabel*)m_lbxDescription.m_pProtoItem;
    m_lblDescription.Initialize(pProtoLabel->m_cExtent, pProtoLabel, 1.0f);

    StopLoadFromLayout();
    pItems->m_bShowScrollBar = 1;
    m_lblDescription.m_cText.m_cParams.SetText("");

#ifndef _XBOX
    pBack->m_bFocusable = 0;
    pBack->AddEvent(GUIEVENT_ACTIVATE, this, (CSWGuiHandler)&CSWGuiPanel::OnBButtonPressed);
#endif

    SetActiveControl(pItems, 0);
}
// PC 0x006D28F0  Xbox 0x00121120
CSWGuiQuestItem::~CSWGuiQuestItem()
{
    m_lbxItems.ClearItems();
    m_lbxDescription.ClearItems();

    while (m_lstItemEntries.m_pInternal->m_pHead != 0)
    {
        delete m_lstItemEntries.RemoveHead();
    }
}
// PC 0x006D2C20  Xbox 0x00121710
void CSWGuiQuestItem::OnPanelAdded()
{
    PopulateItemListBox();
#ifndef _XBOX
    CSWGuiPanel::OnPanelAdded();
#endif
}
// PC 0x006D29F0  Xbox 0x00121240
void CSWGuiQuestItem::PopulateItemListBox()
{
    CSWParty* pParty = g_pAppManager->m_pClientExoApp->GetSWParty();
    CSWCCreature* pPlayerCharacter = pParty->GetPlayerCharacter();
    CSWSCreature* pServerCreature = pPlayerCharacter->GetServerCreature();
    CItemRepository* pRepository = pServerCreature->GetItemRepository(1);

    CExoArrayList<CSWGuiControl*> aControls;
    CSWGuiButton* pProto = (CSWGuiButton*)m_lbxItems.m_pProtoItem;
    int nWidth = m_lbxItems.m_cItemExtent.m_nWidth - 2 * m_lbxItems.m_nPadding;

    unsigned long i;
    for (i = 0; i < (unsigned long)pRepository->m_lstItems.num; i++)
    {
        CSWSItem* pItem = pRepository->ItemListGetItem(i);
        if (pItem != 0 && pItem->m_bPlotObject != 0)
        {
            CSWGuiInGameItemEntry* pItemEntry = new CSWGuiInGameItemEntry;
            if (pItemEntry != 0)
            {
                pItemEntry->Initialize(pProto->m_cText, pProto->m_cBorder,
                                       pProto->m_cHilite, nWidth, 0);
                pItemEntry->AddEvent(
                    CSWGuiControl::EVENT_ENTER, this,
                    (PFNGUIEVENT)&CSWGuiQuestItem::OnControlEntered);
                pItemEntry->SetItem(pItem->GetId(), 0, 0);
                m_lstItemEntries.AddTail(pItemEntry);
                aControls.Add(pItemEntry);
            }
        }
    }

    m_lbxItems.AddControls(aControls, 0, 1, 0);
    if (m_pSelectedItemControl != 0)
    {
        m_pSelectedItemControl->SetActive(0);
    }

    if (aControls.num > 0)
    {
        m_lbxItems.SetSelectedControl(0, 0);
    }
    else
    {
        m_lbxItems.SetSelectedControl(-1, 0);
        m_lblDescription.m_cText.m_cParams.SetText("");
        OnControlEntered(0);
    }
}
