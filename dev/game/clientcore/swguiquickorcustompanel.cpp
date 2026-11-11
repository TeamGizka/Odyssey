#include "exobase.h"
#include "exoresman.h"
#include "exotlktable.h"
#include "clientexoappinternal.h"
#include "swguiquickorcustompanel.h"
#include "swguimainchargen.h"
#ifdef _XBOX
#include "guiingame.h"
#endif

static inline void SwguiquickorcustompanelTrace2();
static inline void SwguiquickorcustompanelTrace1();
#define SWGUIQUICKORCUSTOMPANEL_TRACE() SwguiquickorcustompanelTrace2()
// PC 0x006F07D0  Xbox 0x001288F0
void CSWGuiQuickOrCustomPanel::HandleCancelButton()
{
    if (m_pMainCharGen != NULL)
    {
        m_pMainCharGen->RemoveSubPanels();
    }
    m_bDeferredRemoval = 0;
    m_nExitState = EXITSTATE_DELETE;
}
// PC 0x006F0800  Xbox 0x00128920
void CSWGuiQuickOrCustomPanel::OnSelectQuickCharacterButton(
    CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        if (m_pMainCharGen != NULL)
        {
            m_pMainCharGen->SetVisiblePanel(2);
        }
    }
}
// PC 0x006F0830  Xbox 0x00128950
void CSWGuiQuickOrCustomPanel::OnSelectCustomCharacterButton(
    CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        if (m_pMainCharGen != NULL)
        {
            m_pMainCharGen->SetVisiblePanel(3);
        }
    }
}
// PC 0x006F0860  Xbox 0x00128980
void CSWGuiQuickOrCustomPanel::SetDescription(unsigned long nStrRef)
{
    STR_RES strRes;
    CSWGuiControl* pLabel;
    CSWGuiListBox* pList;
    int nLength;
    CSWGuiText* pText;

    g_pTlkTable->Fetch(nStrRef, strRes, CTlkTable::GENDER_MALE);
    m_cDescription.m_cText.m_cParams.SetText(strRes.m_sString);
    pList = &m_cDescriptionList;
    pList->ClearItems();
    CSWGuiExtent extent = m_cDescription.m_cExtent;
    nLength = strRes.m_sString.GetLength();
    pText = &m_cDescription.m_cText;
    if (nLength > 0)
    {
        extent.m_nHeight = pText->GetIdealHeight();
    }
    else
    {
        extent.m_nHeight = pText->GetFontHeight();
    }
    m_cDescription.SetExtent(extent);
    pLabel = &m_cDescription;
    pList->AddControls(&pLabel, 1, 0, 0, 0);
    pList->SetSelectedControl(&m_cDescription, 0);
    m_cRightAnalog.SetShown(pList->m_nContentHeight + pList->m_nPadding >
                                pList->m_cItemExtent.m_nHeight &&
                            pList->m_lItems.num > 0 &&
                            !pList->m_bHideScrollIcon);
}
// PC 0x006F09F0  Xbox 0x00128BF0
CSWGuiQuickOrCustomPanel::CSWGuiQuickOrCustomPanel(CSWGuiManager* pManager,
                                                   CSWCCreature* pCreature)
    : CSWGuiPanel(pManager)
{
    m_pCreature = pCreature;
    StartLoadFromLayout("QORCPNL");
    InitControl(m_cQuickButton, "QUICK_CHAR_BTN", 1);
    InitControl(m_cCustomButton, "CUST_CHAR_BTN", 1);
    InitControl(m_cDescriptionList, "LB_DESC", 1);
    m_cDescriptionList.m_nSelectSound = CSWGuiManager::GUISOUND_SELECTION;
    InitControl(m_cDecoration, "LBL_DECORATION", 1);
    InitControl(m_cRightAnalog, "LBL_RANALOG", 1);
    InitControl(m_cBackground, "LBL_RBG", 1);
#ifndef _XBOX
    InitControl(m_cBackButton, "BTN_BACK", 1);
#endif
    StopLoadFromLayout();
    CSWGuiLabel* pProtoLabel = (CSWGuiLabel*)m_cDescriptionList.m_pProtoItem;
    m_cDescription.Initialize(pProtoLabel->m_cExtent, pProtoLabel, 1.0f);
    m_cQuickButton.AddEvent(
        0, this,
        (PFNGUIEVENT)&CSWGuiQuickOrCustomPanel::OnEnterQuickCharacterButton);
    m_cCustomButton.AddEvent(
        0, this,
        (PFNGUIEVENT)&CSWGuiQuickOrCustomPanel::OnEnterCustomCharacterButton);
    m_cQuickButton.AddEvent(
        GUIEVENT_ACTIVATE, this,
        (PFNGUIEVENT)&CSWGuiQuickOrCustomPanel::OnSelectQuickCharacterButton);
    m_cCustomButton.AddEvent(
        GUIEVENT_ACTIVATE, this,
        (PFNGUIEVENT)&CSWGuiQuickOrCustomPanel::OnSelectCustomCharacterButton);
    m_cQuickButton.AddEvent(
        GUIEVENT_ACCEPT, this,
        (PFNGUIEVENT)&CSWGuiQuickOrCustomPanel::OnSelectQuickCharacterButton);
    m_cCustomButton.AddEvent(
        GUIEVENT_ACCEPT, this,
        (PFNGUIEVENT)&CSWGuiQuickOrCustomPanel::OnSelectCustomCharacterButton);
#ifdef _XBOX
    m_cQuickButton.m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_BLUE;
    m_cQuickButton.m_cHilite.m_cParams.m_vColor = CGuiInGame::COLOR_YELLOW;
#endif
    m_cQuickButton.m_cHilite.m_cParams.SetPulsingAlpha(1, 1, 0);
#ifdef _XBOX
    m_cCustomButton.m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_BLUE;
    m_cCustomButton.m_cHilite.m_cParams.m_vColor = CGuiInGame::COLOR_YELLOW;
#endif
    m_cCustomButton.m_cHilite.m_cParams.SetPulsingAlpha(1, 1, 0);
#ifndef _XBOX
    CSWGuiButton* pBack = &m_cBackButton;
    pBack->m_bFocusable = 0;
    pBack->AddEvent(GUIEVENT_ACTIVATE, this,
                    (PFNGUIEVENT)&CSWGuiPanel::OnBButtonPressed);
#endif
    SetActiveControl(&m_cQuickButton, 0);
    m_bCenterHorizontal = 1;
    m_bCenterVertical = 1;
}
// PC 0x006F0E10  Xbox 0x00128B20
void CSWGuiQuickOrCustomPanel::HandleInputEvent(int nEvent, int nState)
{
    SWGUIQUICKORCUSTOMPANEL_TRACE();
    if (nState != 0)
    {
        switch (nEvent)
        {
        case 45:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            break;
        case 40:
        case 46:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            HandleCancelButton();
            break;
        case 58:
        {
            CSWGuiControl* pList = &m_cDescriptionList;
            pList->HandleInputEvent(GUIEVENT_DOWN, 1);
        }
        break;
        case 57:
        {
            CSWGuiControl* pList = &m_cDescriptionList;
            pList->HandleInputEvent(GUIEVENT_UP, 1);
        }
        break;
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}

static inline void SwguiquickorcustompanelTrace2()
{
    SwguiquickorcustompanelTrace1();
}

static inline void SwguiquickorcustompanelTrace1()
{
}
