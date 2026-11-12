#include "exobase.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "clientoptions.h"
#include "swguiingameautopause.h"

#ifndef _XBOX
CSWGuiOptionsCheckbox::CSWGuiOptionsCheckbox()
{
}
#endif
// PC 0x0075A500
void CSWGuiInGameAutoPause::SetupOptions()
{
    CClientOptions* pOptions =
        g_pAppManager->m_pClientExoApp->GetClientOptions();
    if (pOptions != 0)
    {
        unsigned int bEndOfCombatRound = pOptions->m_bAutopauseEndOfCombatRound;
        CSWGuiButtonToggle* pToggle0 = m_aCheckboxes[0].AsButtonToggle();
        pToggle0->m_bToggled = bEndOfCombatRound;
        unsigned int bEnemySighted = pOptions->m_bAutopauseEnemySighted;
        CSWGuiButtonToggle* pToggle1 = m_aCheckboxes[1].AsButtonToggle();
        pToggle1->m_bToggled = bEnemySighted;
#ifdef _XBOX
        unsigned int bPartyKilled = pOptions->m_bAutopausePartyKilled;
        CSWGuiButtonToggle* pToggle2 = m_aCheckboxes[2].AsButtonToggle();
        pToggle2->m_bToggled = bPartyKilled;
        unsigned int bActionMenu = pOptions->m_bAutopauseActionMenu;
        CSWGuiButtonToggle* pToggle3 = m_aCheckboxes[3].AsButtonToggle();
        pToggle3->m_bToggled = bActionMenu;
        unsigned int bVertMenu = pOptions->m_nXboxUnmappedBit32;
        CSWGuiButtonToggle* pToggle4 = m_aCheckboxes[4].AsButtonToggle();
        pToggle4->m_bToggled = bVertMenu;
#else
        unsigned int bMineSighted = pOptions->m_bAutopauseMineSighted;
        CSWGuiButtonToggle* pToggle2 = m_aCheckboxes[2].AsButtonToggle();
        pToggle2->m_bToggled = bMineSighted;
        unsigned int bPartyKilled = pOptions->m_bAutopausePartyKilled;
        CSWGuiButtonToggle* pToggle3 = m_aCheckboxes[3].AsButtonToggle();
        pToggle3->m_bToggled = bPartyKilled;
        unsigned int bActionMenu = pOptions->m_bAutopauseActionMenu;
        CSWGuiButtonToggle* pToggle4 = m_aCheckboxes[4].AsButtonToggle();
        pToggle4->m_bToggled = bActionMenu;
#endif
        unsigned int bNewTargetSelected =
            pOptions->m_bAutopauseNewTargetSelected;
        CSWGuiButtonToggle* pToggle5 = m_aCheckboxes[5].AsButtonToggle();
        pToggle5->m_bToggled = bNewTargetSelected;
    }
}
// PC 0x0075A650
void CSWGuiInGameAutoPause::SaveOptions()
{
    CClientOptions* pOptions =
        g_pAppManager->m_pClientExoApp->GetClientOptions();
    if (pOptions != 0)
    {
        pOptions->m_bAutopauseEndOfCombatRound =
            m_aCheckboxes[0].AsButtonToggle()->m_bToggled;
        pOptions->m_bAutopauseEnemySighted =
            m_aCheckboxes[1].AsButtonToggle()->m_bToggled;
#ifdef _XBOX
        pOptions->m_bAutopausePartyKilled =
            m_aCheckboxes[2].AsButtonToggle()->m_bToggled;
        pOptions->m_bAutopauseActionMenu =
            m_aCheckboxes[3].AsButtonToggle()->m_bToggled;
        pOptions->m_nXboxUnmappedBit32 =
            m_aCheckboxes[4].AsButtonToggle()->m_bToggled;
#else
        pOptions->m_bAutopauseMineSighted =
            m_aCheckboxes[2].AsButtonToggle()->m_bToggled;
        pOptions->m_bAutopausePartyKilled =
            m_aCheckboxes[3].AsButtonToggle()->m_bToggled;
        pOptions->m_bAutopauseActionMenu =
            m_aCheckboxes[4].AsButtonToggle()->m_bToggled;
#endif
        pOptions->m_bAutopauseNewTargetSelected =
            m_aCheckboxes[5].AsButtonToggle()->m_bToggled;
    }
}

void CSWGuiInGameAutoPause::ClosePanel()
{
    SaveOptions();
    m_pManager->PopModalPanel();
    m_bDeferredRemoval = 0;
    m_nExitState = EXITSTATE_DELETE;
}
// PC 0x0075A740
void CSWGuiInGameAutoPause::HandleInputEvent(int nEvent, int nState)
{
    if (nState != 0)
    {
        switch (nEvent)
        {
        case 40:
        case 46:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            ClosePanel();
            break;
        case 45:
            if (m_pActiveControl != 0 &&
                m_pActiveControl->AsButtonToggle() != 0)
            {
                m_pManager->PlayGuiSound(3);
                CSWGuiButtonToggle* pToggle =
                    m_pActiveControl->AsButtonToggle();
                m_pActiveControl->AsButtonToggle()->SetToggled(
                    !pToggle->m_bToggled);
            }
            break;
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
// PC 0x0075A7F0
void CSWGuiInGameAutoPause::OnDefault(CSWGuiControl* pControl)
{
    CClientOptions* pOptions =
        g_pAppManager->m_pClientExoApp->GetClientOptions();
    pOptions->SetDefaultAutopauseOptions();
    SetupOptions();
}
// PC 0x0075A820
void CSWGuiInGameAutoPause::SetDescription(unsigned long nStrRef)
{
#ifdef _XBOX
    CSWGuiTextParams* pParams = &m_lblDescription.m_cText.m_cParams;
    pParams->SetStrRef(nStrRef);
    CExoString sText(pParams->m_sText);
    CSWGuiListBox* pListBox = &m_lbxDetails;
    pListBox->ClearItems();
    CSWGuiExtent cExtent = m_lblDescription.m_cExtent;
    if (sText.GetLength() > 0)
    {
        cExtent.m_nHeight = m_lblDescription.m_cText.GetIdealHeight();
    }
    else
    {
        cExtent.m_nHeight = m_lblDescription.m_cText.GetFontHeight();
    }
    m_lblDescription.SetExtent(cExtent);
    CSWGuiControl* pControl = &m_lblDescription;
    pListBox->AddControls(&pControl, 1, 0, 0, 0);
    pListBox->SetSelectedControl(&m_lblDescription, 0);
    int bShowScrollIcon = pListBox->m_nContentHeight + pListBox->m_nPadding >
                              pListBox->m_cItemExtent.m_nHeight &&
                          pListBox->m_lItems.num > 0 &&
                          pListBox->m_bHideScrollIcon == 0;
    m_lblRAnalog.m_bEnabled = bShowScrollIcon;
#else
    m_lblDescription.m_cText.m_cParams.SetStrRef(nStrRef);
    CExoString sText = m_lblDescription.m_cText.m_cParams.m_sText;
    m_lbxDetails.ClearItems();
    CSWGuiExtent cExtent = m_lblDescription.m_cExtent;
    if (sText.GetLength() > 0)
    {
        cExtent.m_nHeight = m_lblDescription.m_cText.GetIdealHeight();
    }
    else
    {
        cExtent.m_nHeight = m_lblDescription.m_cText.GetFontHeight();
    }
    m_lblDescription.SetExtent(cExtent);
    CSWGuiControl* pControl = &m_lblDescription;
    m_lbxDetails.AddControls(&pControl, 1, 0, 0, 0);
    m_lbxDetails.SetSelectedControl(&m_lblDescription, 0);
#endif
}
// PC 0x0075A930
void CSWGuiInGameAutoPause::OnEnterSlot(CSWGuiControl* pControl)
{
    SetDescription(pControl->m_nIndex);
}
// PC 0x0075A940
CSWGuiInGameAutoPause::CSWGuiInGameAutoPause(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
#ifdef _XBOX
    StartLoadFromLayout("optautopause");
    InitControl(m_aCheckboxes[0], "CB_ENDROUND", 1);
    InitControl(m_aCheckboxes[1], "CB_ENEMYSIGHTED", 1);
    InitControl(m_aCheckboxes[2], "CB_PARTYKILLED", 1);
    InitControl(m_aCheckboxes[3], "CB_HORIZMENU", 1);
    InitControl(m_aCheckboxes[4], "CB_VERTMENU", 1);
    InitControl(m_aCheckboxes[5], "CB_TRIGGERS", 1);
    InitControl(m_lblA, "LBL_A", 1);
    InitControl(m_lblB, "LBL_B", 1);
    InitControl(m_lblSelect, "LBL_SELECT", 1);
    InitControl(m_lblBack, "LBL_BACK", 1);
    InitControl(m_lblTitle, "LBL_TITLE", 1);
    InitControl(m_lblRAnalog, "LBL_RANALOG", 1);
    InitControl(m_lbxDetails, "LB_DETAILS", 1);
    m_lbxDetails.m_nSelectSound = CSWGuiManager::GUISOUND_SELECTION;
    CSWGuiOptionsCheckbox* pCheckbox = m_aCheckboxes;
    for (int nSlot = 6; nSlot != 0; nSlot--)
    {
        pCheckbox->AddEvent(0, this,
                            (PFNGUIEVENT)&CSWGuiInGameAutoPause::OnEnterSlot);
        pCheckbox++;
    }
    m_aCheckboxes[0].m_nIndex = 42445;
    m_aCheckboxes[1].m_nIndex = 42446;
    m_aCheckboxes[2].m_nIndex = 42447;
    m_aCheckboxes[3].m_nIndex = 42448;
    m_aCheckboxes[4].m_nIndex = 42449;
    m_aCheckboxes[5].m_nIndex = 42480;
    StopLoadFromLayout();
    CSWGuiLabel* pProtoLabel = (CSWGuiLabel*)m_lbxDetails.m_pProtoItem;
    m_lblDescription.Initialize(pProtoLabel->m_cExtent, pProtoLabel, 1.0f);
    SetupOptions();
    SetActiveControl(&m_aCheckboxes[0], 0);
#else
    {
        CResRef cLayout("optautopause_p");
        StartLoadFromLayout(cLayout);
    }
    InitControl(m_aCheckboxes[0], "CB_ENDROUND", 1);
    InitControl(m_aCheckboxes[1], "CB_ENEMYSIGHTED", 1);
    InitControl(m_aCheckboxes[2], "CB_MINESIGHTED", 1);
    InitControl(m_aCheckboxes[3], "CB_PARTYKILLED", 1);
    InitControl(m_aCheckboxes[4], "CB_ACTIONMENU", 1);
    InitControl(m_aCheckboxes[5], "CB_TRIGGERS", 1);
    InitControl(m_lblTitle, "LBL_TITLE", 1);
    InitControl(m_lbxDetails, "LB_DETAILS", 1);
    InitControl(m_btnDefault, "BTN_DEFAULT", 1);
    CSWGuiButton* pBack = &m_btnBack;
    InitControl(*pBack, "BTN_BACK", 1);
    InitControl(m_alblBar[0], "LBL_BAR1", 1);
    InitControl(m_alblBar[1], "LBL_BAR2", 1);
    InitControl(m_alblBar[2], "LBL_BAR3", 1);
    InitControl(m_alblBar[3], "LBL_BAR4", 1);
    m_lbxDetails.m_nSelectSound = CSWGuiManager::GUISOUND_SELECTION;
    CSWGuiOptionsCheckbox* pCheckbox = m_aCheckboxes;
    for (int nSlot = 6; nSlot != 0; nSlot--)
    {
        pCheckbox->AddEvent(0, this,
                            (PFNGUIEVENT)&CSWGuiInGameAutoPause::OnEnterSlot);
        pCheckbox++;
    }
    m_aCheckboxes[0].m_nIndex = STRREF_DESC_END_ROUND;
    m_aCheckboxes[1].m_nIndex = STRREF_DESC_ENEMY_SIGHTED;
    m_aCheckboxes[2].m_nIndex = STRREF_DESC_MINE_SIGHTED;
    m_aCheckboxes[3].m_nIndex = STRREF_DESC_PARTY_KILLED;
    m_aCheckboxes[4].m_nIndex = STRREF_DESC_ACTION_MENU;
    m_aCheckboxes[5].m_nIndex = STRREF_DESC_TARGET_CHANGE;
    StopLoadFromLayout();

    CSWGuiLabel* pProtoLabel = (CSWGuiLabel*)m_lbxDetails.m_pProtoItem;
    m_lblDescription.Initialize(pProtoLabel->m_cExtent, pProtoLabel, 1.0f);
    CSWGuiButton* pDefault = &m_btnDefault;
    pDefault->m_bFocusable = 0;
    pDefault->AddEvent(39, this,
                       (PFNGUIEVENT)&CSWGuiInGameAutoPause::OnDefault);
    SetupOptions();
    pBack->m_bFocusable = 0;
    pBack->AddEvent(GUIEVENT_ACTIVATE, this, (CSWGuiHandler)&CSWGuiPanel::OnBButtonPressed);
    SetActiveControl(&m_aCheckboxes[0], 0);
#endif
}
