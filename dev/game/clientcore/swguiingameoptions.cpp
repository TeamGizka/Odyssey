#include "exobase.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "swguiingameoptions.h"
#include "guiingame.h"
#include "swguisaveload.h"
#include "swguiingamegameplay.h"
#include "swguiingameautopause.h"
#include "swguioptions.h"
#include "swguicustompanel.h"

void CSWGuiInGameOptions::OnLoadGame(CSWGuiControl* pControl)
{
    int bSelected = pControl->m_bSelected;
    if (bSelected != 0)
    {
        CClientExoApp* pClientApp = g_pAppManager->m_pClientExoApp;
        pClientApp->SaveOptions();
        CSWGuiSaveLoad* pPanel = new CSWGuiSaveLoad(GetManager(), 0, 0);
        m_pManager->AddPanel(pPanel, CSWGuiManager::PANEL_MODE_MODAL, 1);
    }
}

void CSWGuiInGameOptions::OnSaveGame(CSWGuiControl* pControl)
{
    int bSelected = pControl->m_bSelected;
    if (bSelected != 0)
    {
        CClientExoApp* pClientApp = g_pAppManager->m_pClientExoApp;
        pClientApp->SaveOptions();
        CSWGuiSaveLoad* pPanel = new CSWGuiSaveLoad(GetManager(), 1, 0);
        m_pManager->AddPanel(pPanel, CSWGuiManager::PANEL_MODE_MODAL, 1);
    }
}
// PC 0x00735730 GOG 0x00735470
void CSWGuiInGameOptions::OnGameplay(CSWGuiControl* pControl)
{
    int bSelected = pControl->m_bSelected;
    if (bSelected != 0)
    {
        CSWGuiInGameGameplay* pPanel = new CSWGuiInGameGameplay(GetManager());
        m_pManager->AddPanel(pPanel, CSWGuiManager::PANEL_MODE_MODAL, 1);
    }
}
// PC 0x007357B0 GOG 0x007354F0
void CSWGuiInGameOptions::OnFeedback(CSWGuiControl* pControl)
{
    int bSelected = pControl->m_bSelected;
    if (bSelected != 0)
    {
        CSWGuiOptionsFeedback* pPanel = new CSWGuiOptionsFeedback(GetManager());
        m_pManager->AddPanel(pPanel, CSWGuiManager::PANEL_MODE_MODAL, 1);
    }
}
// PC 0x00735830 GOG 0x00735570
void CSWGuiInGameOptions::OnAutopause(CSWGuiControl* pControl)
{
    int bSelected = pControl->m_bSelected;
    if (bSelected != 0)
    {
        CSWGuiInGameAutoPause* pPanel = new CSWGuiInGameAutoPause(GetManager());
        m_pManager->AddPanel(pPanel, CSWGuiManager::PANEL_MODE_MODAL, 1);
    }
}

#ifndef _XBOX
void CSWGuiInGameOptions::OnGraphics(CSWGuiControl* pControl)
{
    int bSelected = pControl->m_bSelected;
    if (bSelected != 0)
    {
        CSWGuiOptionsGraphics* pPanel = new CSWGuiOptionsGraphics(GetManager(), 1);
        m_pManager->AddPanel(pPanel, CSWGuiManager::PANEL_MODE_MODAL, 1);
    }
}
#endif
// PC 0x00735930 GOG 0x00735670
void CSWGuiInGameOptions::OnSound(CSWGuiControl* pControl)
{
    int bSelected = pControl->m_bSelected;
    if (bSelected != 0)
    {
        CSWGuiOptionsSound* pPanel = new CSWGuiOptionsSound(GetManager());
        m_pManager->AddPanel(pPanel, CSWGuiManager::PANEL_MODE_MODAL, 1);
    }
}
// PC 0x007359B0 GOG 0x007356F0
void CSWGuiInGameOptions::ReallyQuit(CSWGuiControl* pControl)
{
    CGuiInGame* pInGameGui = g_pAppManager->m_pClientExoApp->GetInGameGui();
    if (pInGameGui->m_pMessageBox->m_bAccepted != 0)
    {
        g_pAppManager->m_pClientExoApp->SetEndGamePending(1, 0.0f, 1);
    }
}
// PC 0x007359E0 GOG 0x00735720
void CSWGuiInGameOptions::HandleInputEvent(int nEvent, int nState)
{
    if (nState != 0)
    {
        switch (nEvent)
        {
        case 40:
        case 45:
        case 46:
#ifndef _XBOX
        case 223:
#endif
#ifdef _XBOX
            if (g_pAppManager->m_pClientExoApp->GetInGameGui()->HideSWInGameGui() != 0)
#else
            if (g_pAppManager->m_pClientExoApp->GetInGameGui()->HideSWInGameGui(
                    0) != 0)
#endif
            {
#ifdef _XBOX
                g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME);
#else
                g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME, 1);
#endif
            }
        case 53:
        case 54:
            g_pAppManager->m_pClientExoApp->SaveOptions();
            break;
        default:
            break;
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
// PC 0x00735B20 GOG 0x007359C0
void CSWGuiInGameOptions::OnPanelAdded()
{
#ifndef _XBOX
    CSWGuiPanel::OnPanelAdded();
#endif
#ifndef _XBOX
    CSWGuiControl* pControl = m_pManager->m_pMouseOverControl;
#endif
#ifndef _XBOX
    if (pControl != 0 && pControl->AsButton() != 0)
    {
        int nSlot = -1;
        for (int i = 0; i < 8; i++)
        {
            if (pControl == &m_aOptions[i])
            {
                nSlot = i;
                break;
            }
        }
        if (nSlot >= 0 && nSlot < 8)
        {
            SetActiveControl(&m_aOptions[nSlot], 0);
        }
    }
#endif
    if (GetActiveControl() == 0)
    {
        SetActiveControl(&m_aOptions[0], 0);
    }
}

// PC 0x00735B90 GOG 0x00735330
void CSWGuiInGameOptions::OnPanelRemoved()
{
    SetActiveControl(0, 0);
#ifndef _XBOX
    CSWGuiPanel::OnPanelRemoved();
#endif
}
// PC 0x00735BB0 GOG 0x00735A30
void CSWGuiInGameOptions::OnQuit(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        g_pAppManager->m_pClientExoApp->SaveOptions();
        CSWGuiMessageBox* pMessageBox =
            g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
        pMessageBox->SetAllowCancel(1);
        pMessageBox->SetCallback(
            this, (CSWGuiHandler)&CSWGuiInGameOptions::ReallyQuit, 0);
        pMessageBox->SetMessage(42348);
        m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
    }
}

// PC 0x00735C20 GOG 0x00735860
void CSWGuiInGameOptions::SetDescription(unsigned long nStrRef)
{
    CSWGuiTextParams* pParams = &m_lblDescription.m_cText.m_cParams;
    pParams->SetStrRef(nStrRef);
    CExoString sText(pParams->m_sText);
    CSWGuiListBox* pListBox = &m_lbDescription;
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
    m_lblScrollIcon.m_bEnabled = bShowScrollIcon;
}
// PC 0x00735D80 GOG 0x00735AA0
void CSWGuiInGameOptions::OnEnterSlot(CSWGuiControl* pControl)
{
    int nSlot = -1;
    int i = 0;
    CSWGuiButton* pButton = m_aOptions;
    do
    {
        if (nSlot != -1)
        {
            break;
        }
        if (pControl == pButton)
        {
            nSlot = i;
        }
        i++;
        pButton++;
#ifdef _XBOX
    } while (i < 7);
    switch (nSlot)
    {
    case 0:
        SetDescription(42300);
        break;
    case 1:
        SetDescription(42301);
        break;
    case 2:
        SetDescription(42273);
        break;
    case 3:
        SetDescription(42274);
        break;
    case 4:
        SetDescription(42275);
        break;
    case 5:
        SetDescription(42276);
        break;
    case 6:
        SetDescription(42302);
        break;
    }
#else
    } while (i < 8);
    switch (nSlot)
    {
    case 0:
        SetDescription(STRREF_DESC_LOAD_GAME);
        break;
    case 1:
        SetDescription(STRREF_DESC_SAVE_GAME);
        break;
    case 2:
        SetDescription(STRREF_DESC_GAMEPLAY);
        break;
    case 3:
        SetDescription(STRREF_DESC_FEEDBACK);
        break;
    case 4:
        SetDescription(STRREF_DESC_AUTOPAUSE);
        break;
    case 5:
        SetDescription(STRREF_DESC_GRAPHICS);
        break;
    case 6:
        SetDescription(STRREF_DESC_SOUND);
        break;
    case 7:
        SetDescription(STRREF_DESC_QUIT);
        break;
    }
#endif
}
// PC 0x00735E50 GOG 0x00735B70
CSWGuiInGameOptions::CSWGuiInGameOptions(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
#ifdef _XBOX
    StartLoadFromLayout("optionsingame");
#else
    StartLoadFromLayout("optionsingame_p");
#endif
#ifdef _XBOX
    InitControl(m_aOptions[0], "BTN_LOADGAME", 1);
    InitControl(m_aOptions[1], "BTN_SAVEGAME", 1);
    InitControl(m_aOptions[2], "BTN_GAMEPLAY", 1);
    InitControl(m_aOptions[3], "BTN_FEEDBACK", 1);
    InitControl(m_aOptions[4], "BTN_AUTOPAUSE", 1);
    InitControl(m_aOptions[5], "BTN_GRAPHICSSOUND", 1);
    InitControl(m_aOptions[6], "BTN_QUIT", 1);
    InitControl(m_lblTitle, "LBL_TITLE", 1);
    InitControl(m_lblUnknown12C8, "LBL_A", 1);
    InitControl(m_lblUnknown1408, "LBL_SELECT", 1);
    InitControl(m_lblUnknown1548, "LBL_START", 1);
    InitControl(m_lblUnknown1688, "LBL_EXIT", 1);
    InitControl(m_lblScrollIcon, "LBL_RANALOG", 1);
    InitControl(m_lbDescription, "LB_DESC", 1);
    m_lbDescription.m_nSelectSound = CSWGuiManager::GUISOUND_SELECTION;
    StopLoadFromLayout();
    CSWGuiLabel* pProtoLabel = (CSWGuiLabel*)m_lbDescription.m_pProtoItem;
    m_lblDescription.Initialize(pProtoLabel->m_cExtent, pProtoLabel, 1.0f);
    m_aOptions[0].AddEvent(39, this,
                           (CSWGuiHandler)&CSWGuiInGameOptions::OnLoadGame);
    m_aOptions[1].AddEvent(39, this,
                           (CSWGuiHandler)&CSWGuiInGameOptions::OnSaveGame);
    m_aOptions[2].AddEvent(39, this,
                           (CSWGuiHandler)&CSWGuiInGameOptions::OnGameplay);
    m_aOptions[3].AddEvent(39, this,
                           (CSWGuiHandler)&CSWGuiInGameOptions::OnFeedback);
    m_aOptions[4].AddEvent(39, this,
                           (CSWGuiHandler)&CSWGuiInGameOptions::OnAutopause);
    m_aOptions[5].AddEvent(39, this,
                           (CSWGuiHandler)&CSWGuiInGameOptions::OnSound);
    m_aOptions[6].AddEvent(39, this,
                           (CSWGuiHandler)&CSWGuiInGameOptions::OnQuit);
    for (int i = 0; i < 7; i++)
    {
        m_aOptions[i].AddEvent(
            0, this, (CSWGuiHandler)&CSWGuiInGameOptions::OnEnterSlot);
        m_aOptions[i].m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_BLUE;
        m_aOptions[i].m_cHilite.m_cParams.m_vColor = CGuiInGame::COLOR_YELLOW;
        m_aOptions[i].m_cHilite.m_cParams.SetPulsingAlpha(1, 1, 0);
    }
    CSWGuiPanel* pPanel = this;
    pPanel->SetActiveControl(&m_aOptions[0], 0);
#else
    InitControl(m_aOptions[0], "BTN_LOADGAME", 1);
    InitControl(m_aOptions[1], "BTN_SAVEGAME", 1);
    InitControl(m_aOptions[2], "BTN_GAMEPLAY", 1);
    InitControl(m_aOptions[3], "BTN_FEEDBACK", 1);
    InitControl(m_aOptions[4], "BTN_AUTOPAUSE", 1);
    InitControl(m_aOptions[5], "BTN_GRAPHICS", 1);
    InitControl(m_aOptions[6], "BTN_SOUND", 1);
    InitControl(m_aOptions[7], "BTN_QUIT", 1);
    InitControl(m_lblTitle, "LBL_TITLE", 1);
    InitControl(m_lbDescription, "LB_DESC", 1);
    m_lbDescription.m_nSelectSound = CSWGuiManager::GUISOUND_SELECTION;
    InitControl(m_btnExit, "BTN_EXIT", 1);
    InitControl(m_alblBar[0], "LBL_BAR1", 1);
    InitControl(m_alblBar[1], "LBL_BAR2", 1);
    InitControl(m_alblBar[2], "LBL_BAR3", 1);
    InitControl(m_alblBar[3], "LBL_BAR4", 1);
    InitControl(m_alblBar[4], "LBL_BAR5", 1);
    StopLoadFromLayout();
    CSWGuiLabel* pProtoLabel = (CSWGuiLabel*)m_lbDescription.m_pProtoItem;
    m_lblDescription.Initialize(pProtoLabel->m_cExtent, pProtoLabel, 1.0f);
    m_aOptions[0].AddEvent(39, this,
                           (CSWGuiHandler)&CSWGuiInGameOptions::OnLoadGame);
    m_aOptions[1].AddEvent(39, this,
                           (CSWGuiHandler)&CSWGuiInGameOptions::OnSaveGame);
    m_aOptions[2].AddEvent(39, this,
                           (CSWGuiHandler)&CSWGuiInGameOptions::OnGameplay);
    m_aOptions[3].AddEvent(39, this,
                           (CSWGuiHandler)&CSWGuiInGameOptions::OnFeedback);
    m_aOptions[4].AddEvent(39, this,
                           (CSWGuiHandler)&CSWGuiInGameOptions::OnAutopause);
#ifndef _XBOX
    m_aOptions[5].AddEvent(39, this,
                           (CSWGuiHandler)&CSWGuiInGameOptions::OnGraphics);
#endif
    m_aOptions[6].AddEvent(39, this,
                           (CSWGuiHandler)&CSWGuiInGameOptions::OnSound);
    m_aOptions[7].AddEvent(39, this,
                           (CSWGuiHandler)&CSWGuiInGameOptions::OnQuit);
    for (int i = 0; i < 8; i++)
    {
        m_aOptions[i].AddEvent(
            0, this, (CSWGuiHandler)&CSWGuiInGameOptions::OnEnterSlot);
        m_aOptions[i].m_cHilite.m_cParams.SetPulsingAlpha(1, 1, 0);
    }
    CSWGuiButton* pExit = &m_btnExit;
    pExit->m_bFocusable = 0;
    pExit->AddEvent(GUIEVENT_ACTIVATE, this, (CSWGuiHandler)&CSWGuiPanel::OnBButtonPressed);
    CSWGuiPanel* pPanel = this;
    pPanel->SetActiveControl(&m_aOptions[1], 0);
#endif
}
