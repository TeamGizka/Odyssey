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

static inline void SwguiingamejournalTrace1();
#define SWGUIINGAMEJOURNAL_TRACE() SwguiingamejournalTrace1()

int g_nJournalSortType = 0;
int g_bOverrideJournalSortType = 0;

unsigned long CSWGuiInGameJournal::s_pnSortLabels[4] =
{
    32173, 32174, 32175, 32176
};
// PC 0x00644740  Xbox 0x000E1300
void CGuiInGame::SetJournalDone(int bDone)
{
    m_bJournalDone = bDone;
}
// PC 0x00644760
inline int CSWCJournal::GetDoneSortBy()
{
    return m_lstDone.m_nSortBy;
}
// PC 0x00644770
inline void CSWCJournal::ClearActiveHasChanged()
{
    m_lstActive.m_bHasChanged = 0;
}
// PC 0x00644780
inline void CSWCJournal::ClearDoneHasChanged()
{
    m_lstDone.m_bHasChanged = 0;
}
// PC 0x00644790  Xbox 0x000E1340
inline void CSWGuiJournalItemEntry::SetEncouraged(int bEncouraged)
{
    if (m_bEncouraged != bEncouraged)
    {
        m_bEncouraged = bEncouraged;
        if (m_bEncouraged)
        {
            m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_PURPLE;
            m_cBorder.m_cParams.m_fHilightAlpha = 0.5f;
            m_cHilite.m_cParams.m_vColor = CGuiInGame::COLOR_PURPLE;
        }
        else
        {
            m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_BLUE;
            m_cHilite.m_cParams.m_vColor = CGuiInGame::COLOR_YELLOW;
        }
    }
}
// PC 0x00644A40  Xbox 0x000E15E0
CSWGuiInGameJournal::CSWGuiInGameJournal(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    m_pUnknownFB8 = 0;
    m_pQuestItemPanel = new CSWGuiQuestItem(m_pManager);

#ifdef _XBOX
    StartLoadFromLayout("journal");
#else
    {
        CResRef cLayout("journal");
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
    InitControl(m_btnQuestItems, "BTN_QUESTITEMS", 1);
    InitControl(m_btnSwapText, "BTN_SWAPTEXT", 1);
    InitControl(m_btnSort, "BTN_SORT", 1);
    InitControl(m_btnExit, "BTN_EXIT", 1);
#endif
    StopLoadFromLayout();

    CSWGuiLabel* pProtoLabel = (CSWGuiLabel*)m_lblItemDescription.m_pProtoItem;
    m_lblDescription.Initialize(pProtoLabel->m_cExtent, pProtoLabel, 1.0f);
    CSWGuiButton* pProto = (CSWGuiButton*)m_lbxItems.m_pProtoItem;
    pProto->m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_BLUE;
    pProto->m_cHilite.m_cParams.m_vColor = CGuiInGame::COLOR_YELLOW;
    pProto->m_cHilite.m_cParams.SetPulsingAlpha(1, 1, 0);

    CSWGuiListBox* pItems = &m_lbxItems;
    pItems->m_bShowScrollBar = 1;

#ifndef _XBOX
    CSWGuiButton* pQuestItems = &m_btnQuestItems;
    pQuestItems->SetFocusable(0);
    pQuestItems->AddEvent(CSWGuiControl::EVENT_SELECT, this,
                         (CSWGuiHandler)&CSWGuiPanel::OnXButtonPressed);
    CSWGuiButton* pSwapText = &m_btnSwapText;
    pSwapText->SetFocusable(0);
    pSwapText->AddEvent(CSWGuiControl::EVENT_SELECT, this,
                       (CSWGuiHandler)&CSWGuiPanel::OnYButtonPressed);
    CSWGuiButton* pSort = &m_btnSort;
    pSort->SetFocusable(0);
    pSort->AddEvent(CSWGuiControl::EVENT_SELECT, this,
                   (CSWGuiHandler)&CSWGuiPanel::OnBlackButtonPressed);
    CSWGuiButton* pExit = &m_btnExit;
    pExit->SetFocusable(0);
    pExit->AddEvent(CSWGuiControl::EVENT_SELECT, this,
                   (CSWGuiHandler)&CSWGuiPanel::OnBButtonPressed);

    pItems->SetSelectOnLeftClick(1);
#endif
    SetActiveControl(pItems, 0);

    int nNextSort = g_nJournalSortType + 1;
    if (nNextSort >= 4)
    {
        nNextSort = 0;
    }
    CExoString sSort = g_pTlkTable->GetSimpleString(STRREF_SORT) + " " +
                       g_pTlkTable->GetSimpleString(s_pnSortLabels[nNextSort]);
#ifdef _XBOX
    m_lblSort.m_cText.m_cParams.SetText(sSort);
#else
    m_btnSort.m_cText.m_cParams.SetText(sSort);
#endif

    if (g_pAppManager->GetClientExoApp()->GetInGameGui()->m_bJournalDone)
    {
#ifdef _XBOX
        m_lblSwapText.m_cText.m_cParams.SetStrRef(STRREF_ACTIVE_QUESTS);
#else
        m_btnSwapText.m_cText.m_cParams.SetStrRef(STRREF_ACTIVE_QUESTS);
#endif
        sSort =
            g_pTlkTable->GetSimpleString(STRREF_COMPLETED_QUESTS) + " - " +
            g_pTlkTable->GetSimpleString(s_pnSortLabels[g_nJournalSortType]);
        m_lblTitle.m_cText.m_cParams.SetText(sSort);
    }
    else
    {
#ifdef _XBOX
        m_lblSwapText.m_cText.m_cParams.SetStrRef(STRREF_COMPLETED_QUESTS);
#else
        m_btnSwapText.m_cText.m_cParams.SetStrRef(STRREF_COMPLETED_QUESTS);
#endif
        sSort =
            g_pTlkTable->GetSimpleString(STRREF_ACTIVE_QUESTS) + " - " +
            g_pTlkTable->GetSimpleString(s_pnSortLabels[g_nJournalSortType]);
        m_lblTitle.m_cText.m_cParams.SetText(sSort);
    }
    m_lblDescription.m_cText.m_cParams.SetText("");
}
// PC 0x00644850  Xbox 0x000E1400
CSWGuiInGameJournal::~CSWGuiInGameJournal()
{
    m_lblItemDescription.ClearItems();
    m_lbxItems.ClearItems();
    delete m_pQuestItemPanel;
}
// PC 0x00644960  Xbox 0x000E1570
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

#ifdef _XBOX
// PC 0x0041A290  Xbox 0x0004CEC0
inline int CSWGuiListBox::IsOversized()
{
    return m_nContentHeight + m_nPadding > m_cItemExtent.m_nHeight &&
           m_lItems.num > 0 && !m_bHideScrollIcon;
}
#endif
// PC 0x00645100  Xbox 0x000E1DD0
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
// PC 0x00645330  Xbox 0x000E2100
void CSWGuiInGameJournal::PopulateItemListBox()
{
    CSWCJournal* pJournal = g_pAppManager->m_pClientExoApp->GetQuestJournal();
    CExoString sText;
    int nLanguage = g_pAppManager->m_pClientExoApp->GetClientLanguage();
    CExoArrayList<CSWGuiControl*> aControls;
    CSWGuiButton* pProto = (CSWGuiButton*)m_lbxItems.m_pProtoItem;
    CSWGuiExtent extent = pProto->m_cExtent;
    extent.m_nWidth =
        m_lbxItems.m_cItemExtent.m_nWidth - 2 * m_lbxItems.m_nPadding;

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
            pEntry->m_lsName.GetString(nLanguage, &sText, 0);
            pItem->m_cText.m_cParams.SetText(sText);
            if (pEntry->m_bNew)
            {
                pItem->SetEncouraged(1);
            }
            aControls.Add(pItem);
        }
        nIndex++;
    }

    m_lbxItems.AddControls(aControls, 1, 1, 0);
    if (m_pUnknownFB8 != 0)
    {
        m_pUnknownFB8->SetActive(0);
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
// PC 0x00645610  Xbox 0x000E2060
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
// PC 0x00645670  Xbox 0x000E23D0
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
// PC 0x006456E0  Xbox 0x000E2440
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
#else
            m_btnSort.m_cText.m_cParams.SetText(sSort);
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
// PC 0x00645E00  Xbox 0x000E2AB0
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
#else
    m_btnSort.m_cText.m_cParams.SetText(sSort);
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
// PC 0x00646070
void CSWGuiInGameJournal::OnPanelRemoved()
{
    MarkNewQuestsObserved();
}

static inline void SwguiingamejournalTrace1()
{
}
