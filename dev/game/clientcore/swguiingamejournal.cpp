#include "exobase.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "exotlktable.h"
#include "swguiingame.h"

#include "swguiingame.h"
#include "nwclass.h"
#include "twodimarrays.h"
#include "twodimarray.h"
#include "guiingame.h"
#include "swparty.h"
#include "nwccreature.h"
#include "nwtlktable.h"

static inline void SwguiingamejournalTrace1();
#define SWGUIINGAMEJOURNAL_TRACE() SwguiingamejournalTrace1()

int g_nJournalSortType = 0;
int g_bOverrideJournalSortType = 0;

unsigned long CSWGuiInGameJournal::s_pnSortLabels[4] =
{
    32173, 32174, 32175, 32176
};
// PC 0x0069B540 GOG 0x0069B2F0
void CGuiInGame::SetJournalDone(int bDone)
{
    m_bJournalDone = bDone;
}
// PC 0x00406600 GOG 0x004065D0
inline int CSWCJournal::GetDoneSortBy()
{
    return m_lstDone.m_nSortBy;
}
// PC 0x0069B560 GOG 0x0069B310
inline void CSWCJournal::ClearActiveHasChanged()
{
    m_lstActive.m_bHasChanged = 0;
}
// PC 0x0069B570 GOG 0x0069B320
inline void CSWCJournal::ClearDoneHasChanged()
{
    m_lstDone.m_bHasChanged = 0;
}
// PC 0x0069B580 GOG 0x0069B330
inline void CSWGuiJournalItemEntry::SetEncouraged(int bEncouraged)
{
    if (m_bEncouraged != bEncouraged)
    {
        m_bEncouraged = bEncouraged;
        if (m_bEncouraged)
        {
            m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_BONUS;
            m_cBorder.m_cParams.m_fHilightAlpha = 0.5f;
            m_cHilite.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_BONUS;
        }
        else
        {
            m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_GREEN;
            m_cHilite.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_HIGHLIGHT;
        }
    }
}
// PC 0x0069B930 GOG 0x0069B6E0
CSWGuiInGameJournal::CSWGuiInGameJournal(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    m_pUnknownFB8 = 0;
#ifdef _XBOX
    m_pQuestItemPanel = new CSWGuiQuestItem(m_pManager);
#else
    m_pMessagesPanel = new CSWGuiInGameMessages(m_pManager);
#endif

#ifdef _XBOX
    StartLoadFromLayout("journal");
#else
    {
        CResRef cLayout("journal_p");
        StartLoadFromLayout(cLayout);
    }
#endif
#ifdef _XBOX
    InitControl(m_lblSwapText, "LBL_SWAPTEXT", 1);
    InitControl(m_lblSwapButton, "LBL_SWAPBUTTON", 1);
    InitControl(m_lblQuestItems, "LBL_QUESTITEMS", 1);
    InitControl(m_lblXButton, "LBL_XBUTTON", 1);
    InitControl(m_lblStart, "LBL_START", 1);
    InitControl(m_lblExit, "LBL_EXIT", 1);
    InitControl(m_lblImage, "LBLA_IMAGE", 1);
    InitControl(m_lblTitle, "LBL_TITLE", 1);
    InitControl(m_lbxItems, "LB_ITEMS", 1);
    InitControl(m_lblItemDescription, "LBL_ITEM_DESCRIPTION", 1);
    InitControl(m_lblSort, "LBL_SORT", 1);
    InitControl(m_lblSortButton, "LBL_SORTBUTTON", 1);
    InitControl(m_lblRAnalog, "LBL_RANALOG", 1);
#else
    InitControl(m_lblTitle, "LBL_TITLE", 1);
    InitControl(m_lbxItems, "LB_ITEMS", 1);
    InitControl(m_lblItemDescription, "LBL_ITEM_DESCRIPTION", 1);
    InitControl(m_btnSwapText, "BTN_SWAPTEXT", 1);
    InitControl(m_btnExit, "BTN_EXIT", 1);
    InitControl(m_btnFilterTime, "BTN_FILTER_TIME", 1);
    InitControl(m_btnFilterName, "BTN_FILTER_NAME", 1);
    InitControl(m_btnFilterPlanet, "BTN_FILTER_PLANET", 1);
    InitControl(m_btnMessages, "BTN_MESSAGES", 1);
    InitControl(m_alblBar[0], "LBL_BAR1", 1);
    InitControl(m_alblBar[1], "LBL_BAR2", 1);
    InitControl(m_alblBar[2], "LBL_BAR3", 1);
    InitControl(m_alblBar[3], "LBL_BAR4", 1);
    InitControl(m_alblBar[4], "LBL_BAR5", 1);
#endif
    StopLoadFromLayout();

    CSWGuiLabel* pProtoLabel = (CSWGuiLabel*)m_lblItemDescription.m_pProtoItem;
    m_lblDescription.Initialize(pProtoLabel->m_cExtent, pProtoLabel, 1.0f);
    CSWGuiButton* pProto = (CSWGuiButton*)m_lbxItems.m_pProtoItem;
#ifdef _XBOX
    pProto->m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_BLUE;
    pProto->m_cHilite.m_cParams.m_vColor = CGuiInGame::COLOR_YELLOW;
#else
    pProto->m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_GREEN;
    pProto->m_cHilite.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_HIGHLIGHT;
#endif
    pProto->m_cHilite.m_cParams.SetPulsingAlpha(1, 1, 0);

    CSWGuiListBox* pItems = &m_lbxItems;
    pItems->m_bShowScrollBar = 1;

#ifndef _XBOX
    CSWGuiButton* pSwapText = &m_btnSwapText;
    pSwapText->SetFocusable(0);
    pSwapText->AddEvent(CSWGuiControl::EVENT_SELECT, this,
                       (CSWGuiHandler)&CSWGuiPanel::OnYButtonPressed);
    CSWGuiButton* pExit = &m_btnExit;
    pExit->SetFocusable(0);
    pExit->AddEvent(CSWGuiControl::EVENT_SELECT, this,
                   (CSWGuiHandler)&CSWGuiPanel::OnBButtonPressed);

    pItems->SetSelectOnLeftClick(1);

    m_btnFilterTime.SetFocusable(0);
    m_btnFilterTime.m_nIndex = CSWCSortedJournalArrayList::SORT_BY_DATE;
    m_btnFilterTime.AddEvent(CSWGuiControl::EVENT_SELECT, this,
                             (CSWGuiHandler)&CSWGuiInGameJournal::OnFilterPressed);
    m_btnFilterName.SetFocusable(0);
    m_btnFilterName.m_nIndex = CSWCSortedJournalArrayList::SORT_BY_NAME;
    m_btnFilterName.AddEvent(CSWGuiControl::EVENT_SELECT, this,
                             (CSWGuiHandler)&CSWGuiInGameJournal::OnFilterPressed);
    m_btnFilterPlanet.SetFocusable(0);
    m_btnFilterPlanet.m_nIndex = CSWCSortedJournalArrayList::SORT_BY_PLANET;
    m_btnFilterPlanet.AddEvent(CSWGuiControl::EVENT_SELECT, this,
                               (CSWGuiHandler)&CSWGuiInGameJournal::OnFilterPressed);
    CSWGuiButton* pMessages = &m_btnMessages;
    pMessages->SetFocusable(0);
    pMessages->AddEvent(CSWGuiControl::EVENT_SELECT, this,
                        (CSWGuiHandler)&CSWGuiPanel::OnXButtonPressed);
#endif
    SetActiveControl(pItems, 0);

    int nNextSort = g_nJournalSortType + 1;
    if (nNextSort >= 4)
    {
        nNextSort = 0;
    }
#ifndef _XBOX
    UpdateFilterLabelHighlights();
#endif
    CExoString sSort = g_pTlkTable->GetSimpleString(STRREF_SORT) + " " +
                       g_pTlkTable->GetSimpleString(s_pnSortLabels[nNextSort]);
#ifdef _XBOX
    m_lblSort.m_cText.m_cParams.SetText(sSort);

    if (g_pAppManager->GetClientExoApp()->GetInGameGui()->m_bJournalDone)
    {
        m_lblSwapText.m_cText.m_cParams.SetStrRef(STRREF_ACTIVE_QUESTS);
        sSort =
            g_pTlkTable->GetSimpleString(STRREF_COMPLETED_QUESTS) + " - " +
            g_pTlkTable->GetSimpleString(s_pnSortLabels[g_nJournalSortType]);
        m_lblTitle.m_cText.m_cParams.SetText(sSort);
    }
    else
    {
        m_lblSwapText.m_cText.m_cParams.SetStrRef(STRREF_COMPLETED_QUESTS);
        sSort =
            g_pTlkTable->GetSimpleString(STRREF_ACTIVE_QUESTS) + " - " +
            g_pTlkTable->GetSimpleString(s_pnSortLabels[g_nJournalSortType]);
        m_lblTitle.m_cText.m_cParams.SetText(sSort);
    }
#else
    if (g_pAppManager->GetClientExoApp()->GetInGameGui()->m_bJournalDone)
    {
        m_btnSwapText.m_cText.m_cParams.SetStrRef(STRREF_COMPLETED_QUESTS_BY);
    }
    else
    {
        m_btnSwapText.m_cText.m_cParams.SetStrRef(STRREF_ACTIVE_QUESTS_BY);
    }
#endif
    m_lblDescription.m_cText.m_cParams.SetText("");
}
// PC 0x0069B7C0 GOG 0x0069B570
CSWGuiInGameJournal::~CSWGuiInGameJournal()
{
    m_lblItemDescription.ClearItems();
    m_lbxItems.ClearItems();
#ifdef _XBOX
    delete m_pQuestItemPanel;
#else
    if (m_pMessagesPanel != 0)
    {
        delete m_pMessagesPanel;
        m_pMessagesPanel = 0;
    }
#endif
}
// PC 0x0069B650 GOG 0x0069B400
CSWJournalEntry* CSWGuiInGameJournal::GetQuestEntryAt(int nIndex)
{
    SWGUIINGAMEJOURNAL_TRACE();
    CSWCJournal* pJournal = g_pAppManager->m_pClientExoApp->GetQuestJournal();
    if (g_pAppManager->m_pClientExoApp->GetInGameGui()->m_bJournalDone)
    {
        if (nIndex < pJournal->GetNumDoneEntries())
        {
            return pJournal->GetDoneEntryAt(nIndex);
        }
        return 0;
    }
    if (nIndex >= pJournal->GetNumActiveEntries())
    {
        return 0;
    }
    return pJournal->GetActiveEntryAt(nIndex);
}
#ifndef _XBOX
// PC 0x0069B640 GOG 0x0069B3F0
void CSWGuiInGameJournal::OnControlExited(CSWGuiControl* pControl)
{
    ((CSWGuiJournalItemEntry*)pControl)->SetEncouraged(0);
}
// PC 0x0069B6C0 GOG 0x0069B470
void CSWGuiInGameJournal::UpdateFilterLabelHighlights()
{
    m_btnFilterTime.m_cText.m_cParams.SetDefaultColor(CGuiInGame::COLOR_NEW_TEXT);
    m_btnFilterName.m_cText.m_cParams.SetDefaultColor(CGuiInGame::COLOR_NEW_TEXT);
    m_btnFilterPlanet.m_cText.m_cParams.SetDefaultColor(CGuiInGame::COLOR_NEW_TEXT);
    switch (g_nJournalSortType)
    {
    case CSWCSortedJournalArrayList::SORT_BY_DATE:
        m_btnFilterTime.m_cText.m_cParams.SetDefaultColor(CGuiInGame::COLOR_NEW_HIGHLIGHT);
        break;
    case CSWCSortedJournalArrayList::SORT_BY_NAME:
        m_btnFilterName.m_cText.m_cParams.SetDefaultColor(CGuiInGame::COLOR_NEW_HIGHLIGHT);
        break;
    case CSWCSortedJournalArrayList::SORT_BY_PLANET:
        m_btnFilterPlanet.m_cText.m_cParams.SetDefaultColor(CGuiInGame::COLOR_NEW_HIGHLIGHT);
        break;
    }
}
// PC 0x0069B740 GOG 0x0069B4F0
void CSWGuiInGameJournal::OnFilterPressed(CSWGuiControl* pControl)
{
    g_nJournalSortType = pControl->m_nIndex;
    CSWCJournal* pJournal = g_pAppManager->m_pClientExoApp->GetQuestJournal();
    void (CSWCJournal::*pfnSetSortBy)(int);
    if (g_pAppManager->m_pClientExoApp->GetInGameGui()->m_bJournalDone)
    {
        pfnSetSortBy = &CSWCJournal::SetDoneSortBy;
    }
    else
    {
        pfnSetSortBy = &CSWCJournal::SetActiveSortBy;
    }
    (pJournal->*pfnSetSortBy)(g_nJournalSortType);
    UpdateFilterLabelHighlights();
}
#endif

// PC 0x0069C070 GOG 0x0069BE20
void CSWGuiInGameJournal::OnControlEntered(CSWGuiControl* pControl)
{
    if (pControl != 0)
    {
        unsigned long nIndex =
            ((CSWGuiJournalItemEntry*)pControl)->m_nEntryIndex;
        m_lblItemDescription.ClearItems();
        CSWJournalEntry* pEntry = GetQuestEntryAt(nIndex);
        if (pEntry != 0)
        {
            m_aNewQuests.AddUnique(nIndex);
            CExoString sText;
            int nLanguage = g_pAppManager->m_pClientExoApp->GetClientLanguage();
            pEntry->m_lsText.GetString(nLanguage, &sText, 0);
            if (pEntry->m_nQuestRow != -1)
            {
                int nStrRef;
                {
                    CExoString sColumn("Name");
                    CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
                    EXOXBOXASSERT(p2DArrays->m_pJournal);
                    C2DA* pJournal2DA = p2DArrays->m_pJournal;
                    pJournal2DA->GetINTEntry(pEntry->m_nQuestRow, sColumn,
                                             &nStrRef);
                }
                sText = g_pTlkTable->GetSimpleString(nStrRef) + ":\n" + sText;
            }
#ifndef _XBOX
            if (sText.Find('<', 0) != -1 || sText.Find('{', 0) != -1)
            {
                g_pSWTlkTable->ParseStr(
                    sText,
                    (CGameObject*)g_pAppManager->m_pClientExoApp->GetSWParty()->GetPlayerCharacter(),
                    0, 1);
            }
#endif
            m_lblDescription.m_cText.m_cParams.SetText(sText);
        }
    }
    CSWGuiExtent extent = m_lblDescription.m_cExtent;
    if (m_lblDescription.m_cText.m_cParams.m_sText.GetLength() > 0)
    {
        extent.m_nHeight = m_lblDescription.m_cText.GetIdealHeight();
    }
    else
    {
        extent.m_nHeight = m_lblDescription.m_cText.GetFontHeight();
    }
    CSWGuiLabel* pDescription = &m_lblDescription;
    pDescription->SetExtent(extent);
    CSWGuiControl* pLabel = pDescription;
    m_lblItemDescription.AddControls(&pLabel, 1, 0, 0, 0);
    m_lblItemDescription.SetSelectedControl(pDescription, 0);
#ifdef _XBOX
    m_lblRAnalog.SetEnabledFlag(m_lblItemDescription.IsOversized());
#endif
}
// PC 0x0069C320 GOG 0x0069C0D0
void CSWGuiInGameJournal::PopulateItemListBox()
{
    CSWCJournal* pJournal = g_pAppManager->m_pClientExoApp->GetQuestJournal();
    CExoString sText;
    int nLanguage = g_pAppManager->m_pClientExoApp->GetClientLanguage();
    CExoArrayList<CSWGuiControl*> aControls;
    CSWGuiListBox* pItems = &m_lbxItems;
    CSWGuiButton* pProto = (CSWGuiButton*)pItems->m_pProtoItem;
    CSWGuiExtent extent = pProto->m_cExtent;
    extent.m_nWidth =
        pItems->m_cItemExtent.m_nWidth - 2 * pItems->m_nPadding;

    int nIndex = 0;
    CSWJournalEntry* pEntry;
    while ((pEntry = GetQuestEntryAt(nIndex)) != 0)
    {
        CSWGuiJournalItemEntry* pItem = new CSWGuiJournalItemEntry;
        if (pItem != 0)
        {
            pItem->m_nEntryIndex = nIndex;
            pItem->Initialize(extent, pProto);
            pItem->AddEvent(
                0, this, (PFNGUIEVENT)&CSWGuiInGameJournal::OnControlEntered);
#ifndef _XBOX
            pItem->AddEvent(
                1, this, (PFNGUIEVENT)&CSWGuiInGameJournal::OnControlExited);
#endif
            pEntry->m_lsName.GetString(nLanguage, &sText, 0);
#ifndef _XBOX
            if (sText.Find('<', 0) != -1 || sText.Find('{', 0) != -1)
            {
                g_pSWTlkTable->ParseStr(
                    sText,
                    (CGameObject*)g_pAppManager->m_pClientExoApp->GetSWParty()->GetPlayerCharacter(),
                    0, 1);
            }
#endif
            pItem->m_cText.m_cParams.SetText(sText);
            if (pEntry->m_bNew)
            {
                pItem->SetEncouraged(1);
            }
            aControls.Add(pItem);
        }
        nIndex++;
    }

    pItems->AddControls(aControls, 1, 1, 0);
    if (m_pUnknownFB8 != 0)
    {
        m_pUnknownFB8->SetActive(0);
    }
    if (aControls.num > 0)
    {
        pItems->SetSelectedControl(0, 0);
    }
    else
    {
        pItems->SetSelectedControl(-1, 0);
        m_lblDescription.m_cText.m_cParams.SetText("");
        OnControlEntered(0);
    }
    void (CSWCJournal::*pfnClearHasChanged)();
    if (g_pAppManager->m_pClientExoApp->GetInGameGui()->m_bJournalDone)
    {
        pfnClearHasChanged = &CSWCJournal::ClearDoneHasChanged;
    }
    else
    {
        pfnClearHasChanged = &CSWCJournal::ClearActiveHasChanged;
    }
    (pJournal->*pfnClearHasChanged)();
}
// PC 0x0069C670 GOG 0x0069C420
void CSWGuiInGameJournal::MarkNewQuestsObserved()
{
    for (int i = 0; i < m_aNewQuests.num; i++)
    {
        CSWJournalEntry* pEntry = GetQuestEntryAt(m_aNewQuests.element[i]);
        if (pEntry != 0)
        {
            pEntry->m_bNew = 0;
        }
    }
    m_aNewQuests.SetSize(0);
}
// PC 0x0069C6D0 GOG 0x0069C480
void CSWGuiInGameJournal::Draw(float fElapsed)
{
    CSWCJournal* pJournal = g_pAppManager->m_pClientExoApp->GetQuestJournal();
    if ((!g_pAppManager->m_pClientExoApp->GetInGameGui()->m_bJournalDone &&
         pJournal->m_lstActive.m_bHasChanged != 0) ||
        (g_pAppManager->m_pClientExoApp->GetInGameGui()->m_bJournalDone &&
         pJournal->m_lstDone.m_bHasChanged != 0))
    {
        PopulateItemListBox();
    }
    return CSWGuiPanel::Draw(fElapsed);
}
// PC 0x0069C740 GOG 0x0069C4F0
#ifdef _XBOX
void CSWGuiInGameJournal::HandleInputEvent(int nEvent, int nState)
{
    if (nState != 0)
    {
        CSWCJournal* pJournal =
            g_pAppManager->GetClientExoApp()->GetQuestJournal();
        switch (nEvent)
        {
        case 43:
        {
            int (CSWCJournal::*pfnGetSortBy)();
            if (g_pAppManager->GetClientExoApp()->GetInGameGui()->GetJournalDone())
            {
                pfnGetSortBy = &CSWCJournal::GetDoneSortBy;
            }
            else
            {
                pfnGetSortBy = &CSWCJournal::GetActiveSortBy;
            }
            g_nJournalSortType = (pJournal->*pfnGetSortBy)() + 1;
            if (g_nJournalSortType >= 4)
            {
                g_nJournalSortType = 0;
            }
            void (CSWCJournal::*pfnSetSortBy)(int);
            if (g_pAppManager->GetClientExoApp()->GetInGameGui()->GetJournalDone())
            {
                pfnSetSortBy = &CSWCJournal::SetDoneSortBy;
            }
            else
            {
                pfnSetSortBy = &CSWCJournal::SetActiveSortBy;
            }
            (pJournal->*pfnSetSortBy)(g_nJournalSortType);
            int nNextSort = g_nJournalSortType + 1;
            if (nNextSort >= 4)
            {
                nNextSort = 0;
            }
            CExoString sSort =
                g_pTlkTable->GetSimpleString(STRREF_SORT) + " " +
                g_pTlkTable->GetSimpleString(s_pnSortLabels[nNextSort]);
#ifdef _XBOX
            m_lblSort.m_cText.m_cParams.SetText(sSort);
#endif
            sSort = g_pTlkTable->GetSimpleString(
                        g_pAppManager->GetClientExoApp()->GetInGameGui()
                                ->GetJournalDone()
                            ? STRREF_COMPLETED_QUESTS
                            : STRREF_ACTIVE_QUESTS) +
                    " - " +
                    g_pTlkTable->GetSimpleString(
                        s_pnSortLabels[g_nJournalSortType]);
            m_lblTitle.m_cText.m_cParams.SetText(sSort);
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
        }
        break;
        case 42:
        {
            MarkNewQuestsObserved();
            int (CSWCJournal::*pfnGetSortBy)();
            if (g_pAppManager->GetClientExoApp()->GetInGameGui()->GetJournalDone())
            {
                pfnGetSortBy = &CSWCJournal::GetDoneSortBy;
            }
            else
            {
                pfnGetSortBy = &CSWCJournal::GetActiveSortBy;
            }
            g_nJournalSortType = (pJournal->*pfnGetSortBy)();
            g_pAppManager->GetClientExoApp()->GetInGameGui()->SetJournalDone(
                !g_pAppManager->GetClientExoApp()->GetInGameGui()
                     ->GetJournalDone());
            PopulateItemListBox();
            CExoString sTitle;
            if (g_pAppManager->GetClientExoApp()->GetInGameGui()->GetJournalDone())
            {
#ifdef _XBOX
                m_lblSwapText.m_cText.m_cParams.SetStrRef(STRREF_ACTIVE_QUESTS);
#else
                m_btnSwapText.m_cText.m_cParams.SetStrRef(STRREF_ACTIVE_QUESTS);
#endif
                sTitle = g_pTlkTable->GetSimpleString(STRREF_COMPLETED_QUESTS) + " - " +
                         g_pTlkTable->GetSimpleString(
                             s_pnSortLabels[g_nJournalSortType]);
                m_lblTitle.m_cText.m_cParams.SetText(sTitle);
            }
            else
            {
#ifdef _XBOX
                m_lblSwapText.m_cText.m_cParams.SetStrRef(STRREF_COMPLETED_QUESTS);
#else
                m_btnSwapText.m_cText.m_cParams.SetStrRef(STRREF_COMPLETED_QUESTS);
#endif
                sTitle = g_pTlkTable->GetSimpleString(STRREF_ACTIVE_QUESTS) + " - " +
                         g_pTlkTable->GetSimpleString(
                             s_pnSortLabels[g_nJournalSortType]);
                m_lblTitle.m_cText.m_cParams.SetText(sTitle);
            }
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
        }
        break;
        case 41:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            m_pManager->AddPanel(m_pQuestItemPanel, CSWGuiManager::PANEL_MODE_MODAL, 1);
            break;
        case 40:
        case 45:
        case 46:
#ifdef _XBOX
            if (g_pAppManager->GetClientExoApp()->GetInGameGui()->HideSWInGameGui() != 0)
#else
        case 223:
            if (g_pAppManager->GetClientExoApp()->GetInGameGui()->HideSWInGameGui(
                    0) != 0)
#endif
            {
#ifdef _XBOX
                g_pAppManager->GetClientExoApp()->SetInputClass(INPUT_CLASS_GAME);
#else
                g_pAppManager->GetClientExoApp()->SetInputClass(INPUT_CLASS_GAME, 1);
#endif
            }
            break;
        case 57:
            m_lblItemDescription.HandleInputEvent(GUIEVENT_UP, 1);
            break;
        case 58:
            m_lblItemDescription.HandleInputEvent(GUIEVENT_DOWN, 1);
            break;
        default:
            break;
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
#else
void CSWGuiInGameJournal::HandleInputEvent(int nEvent, int nState)
{
    if (nState != 0)
    {
        if (m_pManager->PanelExists(m_pMessagesPanel))
        {
            return;
        }
        CSWCJournal* pJournal =
            g_pAppManager->GetClientExoApp()->GetQuestJournal();
        switch (nEvent)
        {
        case 42:
        {
            MarkNewQuestsObserved();
            int bDone = g_pAppManager->GetClientExoApp()->GetInGameGui()->GetJournalDone();
            int nSortBy = bDone ? pJournal->GetDoneSortBy() : pJournal->GetActiveSortBy();
            CExoString sTitle;
            g_pAppManager->GetClientExoApp()->GetInGameGui()->SetJournalDone(!bDone);
            if (bDone)
            {
                pJournal->SetActiveSortBy(nSortBy);
                m_btnSwapText.m_cText.m_cParams.SetStrRef(STRREF_ACTIVE_QUESTS_BY);
            }
            else
            {
                pJournal->SetDoneSortBy(nSortBy);
                m_btnSwapText.m_cText.m_cParams.SetStrRef(STRREF_COMPLETED_QUESTS_BY);
            }
            PopulateItemListBox();
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
        }
        break;
        case 41:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            m_pManager->AddPanel(m_pMessagesPanel, CSWGuiManager::PANEL_MODE_FOREGROUND, 1);
            m_pManager->SendPanelToBack(m_pMessagesPanel);
            m_pManager->SendPanelToBack(this);
            break;
        case 40:
        case 45:
        case 46:
        case 223:
            if (g_pAppManager->GetClientExoApp()->GetInGameGui()->HideSWInGameGui(
                    0) != 0)
            {
                g_pAppManager->GetClientExoApp()->SetInputClass(INPUT_CLASS_GAME, 1);
            }
            break;
        case 57:
            m_lblItemDescription.HandleInputEvent(GUIEVENT_UP, 1);
            break;
        case 58:
            m_lblItemDescription.HandleInputEvent(GUIEVENT_DOWN, 1);
            break;
        default:
            break;
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
#endif
// PC 0x0069C9E0 GOG 0x0069C790
#ifdef _XBOX
void CSWGuiInGameJournal::OnPanelAdded()
{
    CSWCJournal* pJournal = g_pAppManager->m_pClientExoApp->GetQuestJournal();
    int nSortType = g_nJournalSortType;
    if (g_bOverrideJournalSortType != 0)
    {
        nSortType = 0;
        g_bOverrideJournalSortType = 0;
    }
    void (CSWCJournal::*pfnSetSortBy)(int);
    if (g_pAppManager->m_pClientExoApp->GetInGameGui()->m_bJournalDone)
    {
        pfnSetSortBy = &CSWCJournal::SetDoneSortBy;
    }
    else
    {
        pfnSetSortBy = &CSWCJournal::SetActiveSortBy;
    }
    (pJournal->*pfnSetSortBy)(nSortType);
    int nNextSort = nSortType + 1;
    if (nNextSort >= 4)
    {
        nNextSort = 0;
    }
    CExoString sSort = g_pTlkTable->GetSimpleString(STRREF_SORT) + " " +
                       g_pTlkTable->GetSimpleString(s_pnSortLabels[nNextSort]);
#ifdef _XBOX
    m_lblSort.m_cText.m_cParams.SetText(sSort);
#endif
    sSort = g_pTlkTable->GetSimpleString(
                g_pAppManager->m_pClientExoApp->GetInGameGui()->m_bJournalDone
                    ? STRREF_COMPLETED_QUESTS
                    : STRREF_ACTIVE_QUESTS) +
            " - " + g_pTlkTable->GetSimpleString(s_pnSortLabels[nSortType]);
    m_lblTitle.m_cText.m_cParams.SetText(sSort);
    PopulateItemListBox();
#ifndef _XBOX
    m_btnExit.HandleMouseOver(0);
#endif
#ifndef _XBOX
    CSWGuiPanel::OnPanelAdded();
#endif
}
#else
void CSWGuiInGameJournal::OnPanelAdded()
{
    CSWCJournal* pJournal = g_pAppManager->m_pClientExoApp->GetQuestJournal();
    int nSortType = g_nJournalSortType;
    if (g_bOverrideJournalSortType != 0)
    {
        nSortType = 0;
        g_bOverrideJournalSortType = 0;
    }
    void (CSWCJournal::*pfnSetSortBy)(int);
    if (g_pAppManager->m_pClientExoApp->GetInGameGui()->m_bJournalDone)
    {
        pfnSetSortBy = &CSWCJournal::SetDoneSortBy;
    }
    else
    {
        pfnSetSortBy = &CSWCJournal::SetActiveSortBy;
    }
    (pJournal->*pfnSetSortBy)(nSortType);
    PopulateItemListBox();
    m_btnExit.HandleMouseOver(0);
    CSWGuiPanel::OnPanelAdded();
}
#endif
// PC 0x0069CA60 GOG 0x0069C810
void CSWGuiInGameJournal::OnPanelRemoved()
{
#ifndef _XBOX
    if (m_pManager->PanelExists(m_pMessagesPanel))
    {
        m_pMessagesPanel->SetModal(0);
    }
#endif
    MarkNewQuestsObserved();
}

static inline void SwguiingamejournalTrace1()
{
}
