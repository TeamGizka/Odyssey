#include "exobase.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "swguicustompanel.h"
#include "swguiskillschargen.h"
#include "swguiportraitchargen.h"
#include "guiingame.h"
#include "nwccreature.h"

enum SWGuiCustomPanelState
{
    CUSTOM_PANEL_STATE_PORTRAIT,
    CUSTOM_PANEL_STATE_ABILITIES,
    CUSTOM_PANEL_STATE_SKILLS,
    CUSTOM_PANEL_STATE_FEATS,
    CUSTOM_PANEL_STATE_NAME,
    CUSTOM_PANEL_STATE_PLAY
};
// PC 0x0075FF90 GOG 0x0075FDC0
void CSWGuiMainCharGen::NameChanged()
{
    SetNameString(m_pCreature->GetName());
}
// PC 0x0075FFF0 GOG 0x0075FE20
void CSWGuiCustomPanel::SetState(int nState)
{
    m_nState = nState;
#ifdef _XBOX
    for (int i = 0; i < 6; i++)
    {
        if (i == m_nState)
        {
            m_aHilights[i].SetActive(1);
            m_aNumbers[i].m_cText.m_cParams.SetColor(CGuiInGame::COLOR_YELLOW);
            m_aSteps[i].m_cText.m_cParams.SetColor(CGuiInGame::COLOR_YELLOW);
            CSWGuiTextParams* pParams = &m_aNumbers[i].m_cText.m_cParams;
            pParams->m_bHilightText = 1;
            pParams = &m_aSteps[i].m_cText.m_cParams;
            pParams->m_bHilightText = 1;
        }
        else
        {
            m_aHilights[i].SetActive(0);
            CSWGuiTextParams* pParams = &m_aNumbers[i].m_cText.m_cParams;
            pParams->m_bHilightText = 0;
            pParams = &m_aSteps[i].m_cText.m_cParams;
            pParams->m_bHilightText = 0;
            if (i < m_nState)
            {
                m_aNumbers[i].m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
                m_aSteps[i].m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
            }
            else
            {
                m_aNumbers[i].m_cText.m_cParams.SetColor(CGuiInGame::DISABLED_TEXT);
                m_aSteps[i].m_cText.m_cParams.SetColor(CGuiInGame::DISABLED_TEXT);
            }
        }
    }
#else
    if (nState == CUSTOM_PANEL_STATE_PORTRAIT)
    {
        m_btnBack.SetEnabled(0);
        m_btnBack.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_NEW_GREEN);
        m_btnBack.m_cBorder.m_cParams.SetColor(CGuiInGame::DISABLED_TEXT);
    }
    else
    {
        m_btnBack.SetEnabled(1);
        m_btnBack.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_NEW_TEXT);
        m_btnBack.m_cBorder.m_cParams.SetColor(CGuiInGame::COLOR_NEW_GREEN);
    }

    for (int i = 0; i < 6; i++)
    {
        if (i == m_nState)
        {
            m_aHilights[i].SetActive(1);
            m_aNumbers[i].SetEnabled(1);
            m_aSteps[i].SetEnabled(1);
            m_aNumbers[i].m_cText.m_cParams.SetColor(CGuiInGame::COLOR_NEW_HIGHLIGHT);
            m_aSteps[i].m_cText.m_cParams.SetColor(CGuiInGame::COLOR_NEW_HIGHLIGHT);
            m_aSteps[i].m_cBorder.m_cParams.SetColor(CGuiInGame::COLOR_NEW_HIGHLIGHT);
            CSWGuiTextParams* pParams = &m_aNumbers[i].m_cText.m_cParams;
            pParams->m_bHilightText = 1;
            pParams = &m_aSteps[i].m_cText.m_cParams;
            pParams->m_bHilightText = 1;
        }
        else
        {
            m_aHilights[i].SetActive(0);
            CSWGuiTextParams* pParams = &m_aNumbers[i].m_cText.m_cParams;
            pParams->m_bHilightText = 0;
            pParams = &m_aSteps[i].m_cText.m_cParams;
            pParams->m_bHilightText = 0;
            if (i < m_nState)
            {
                m_aNumbers[i].m_cText.m_cParams.SetColor(CGuiInGame::COLOR_NEW_TEXT);
                m_aSteps[i].m_cText.m_cParams.SetColor(CGuiInGame::COLOR_NEW_TEXT);
                m_aSteps[i].m_cBorder.m_cParams.SetColor(CGuiInGame::COLOR_NEW_GREEN);
                m_aNumbers[i].SetEnabled(0);
                m_aSteps[i].SetEnabled(0);
            }
            else
            {
                m_aNumbers[i].m_cText.m_cParams.SetColor(CGuiInGame::COLOR_NEW_GREEN);
                m_aSteps[i].m_cText.m_cParams.SetColor(CGuiInGame::COLOR_NEW_GREEN);
                m_aSteps[i].m_cBorder.m_cParams.SetColor(CGuiInGame::DISABLED_TEXT);
                m_aNumbers[i].SetEnabled(0);
                m_aSteps[i].SetEnabled(0);
            }
        }
    }
#endif
    SetActiveControl(&m_aSteps[m_nState], 0);
}
// PC 0x00760240 GOG 0x0075FDB0
void CSWGuiCustomPanel::SetParentPanel(CSWGuiPanel* pPanel)
{
    m_pParentPanel = pPanel;
}
// PC 0x00760250 GOG 0x00760070
void CSWGuiCustomPanel::OnSelectPortraitButton(CSWGuiControl* pControl)
{
#ifndef _XBOX
    if (m_nState >= 0)
#endif
    {
#ifdef _XBOX
        int nResult = pControl->m_bSelected;
        if (nResult != 0)
#else
        if (pControl->m_bSelected != 0)
#endif
        {
            CSWGuiPortraitCharGen* pPortrait =
                new CSWGuiPortraitCharGen(m_pManager, m_pCreature);
            pPortrait->SetParentPanel(this, 2);
            if (m_pCreature->GetAnimationBase() != 0)
            {
                ((Gob*)m_pCreature->GetAnimationBase()->GetModel(CSWCObject::MODEL_SLOT_DEFAULT))
                    ->PlayAnimation("pause2", 1.0f, 0, 0);
                ((Gob*)m_pCreature->GetAnimationBase()->GetModel(254))
                    ->PlayAnimation("pause2", 1.0f, 0, 0);
            }
            m_pManager->AddPanel(pPortrait, CSWGuiManager::PANEL_MODE_MODAL, 1);
        }
    }
}
// PC 0x00760350 GOG 0x00760190
void CSWGuiCustomPanel::OnSelectNameButton(CSWGuiControl* pControl)
{
    CSWGuiNameChargen* pPanel;
    CSWGuiNameChargen* pNew;

#ifndef _XBOX
    if (m_nState >= 4)
#endif
    {
        int nResult = pControl->m_bSelected;
        pPanel = NULL;
        if (nResult != 0)
        {
            pNew = new CSWGuiNameChargen(m_pManager, m_pCreature);
            pPanel = pNew;
            pPanel->SetParentPanel(this, 2);
            m_pManager->AddPanel(pPanel, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
        }
    }
}
// PC 0x007603E0 GOG 0x00760220
void CSWGuiCustomPanel::OnSelectFeatsButton(CSWGuiControl* pControl)
{
    CSWGuiFeatsCharGen* pNew;

#ifndef _XBOX
    if (m_nState >= 3)
#endif
    {
        int nResult = pControl->m_bSelected;
        if (nResult != 0)
        {
            pNew = new CSWGuiFeatsCharGen(m_pManager, m_pCreature, 0, 0);
            pNew->m_pParentPanel = this;
            m_pManager->AddPanel(pNew, CSWGuiManager::PANEL_MODE_MODAL, 1);
        }
    }
}
// PC 0x00760470 GOG 0x007602B0
void CSWGuiCustomPanel::OnSelectSkillsButton(CSWGuiControl* pControl)
{
    CSWGuiSkillsCharGen* pNew;

#ifndef _XBOX
    if (m_nState >= 2)
#endif
    {
        int nResult = pControl->m_bSelected;
        if (nResult != 0)
        {
            pNew = new CSWGuiSkillsCharGen(m_pManager, m_pCreature, 0);
            pNew->m_pParentPanel = this;
            m_pManager->AddPanel(pNew, CSWGuiManager::PANEL_MODE_MODAL, 1);
        }
    }
}
// PC 0x00760500 GOG 0x00760340
void CSWGuiCustomPanel::OnSelectAbilitiesButton(CSWGuiControl* pControl)
{
    CSWGuiAbilitiesCharGen* pNew;

#ifndef _XBOX
    if (m_nState >= 1)
#endif
    {
        int nResult = pControl->m_bSelected;
        if (nResult != 0)
        {
            pNew = new CSWGuiAbilitiesCharGen(m_pManager, m_pCreature, 0);
            pNew->m_pParentPanel = this;
            m_pManager->AddPanel(pNew, CSWGuiManager::PANEL_MODE_MODAL, 1);
        }
    }
}
// PC 0x00760590 GOG 0x007603D0
void CSWGuiCustomPanel::CancelledByChild()
{
    m_aSteps[m_nState].m_cText.m_cParams.SetColor(CGuiInGame::COLOR_NEW_HIGHLIGHT);
    CSWGuiButton* pButton = &m_aSteps[m_nState];
    CSWGuiTextParams* pParams = &pButton->m_cText.m_cParams;
    pParams->m_bHilightText = 1;
}
// PC 0x007605D0 GOG 0x00760410
void CSWGuiCustomPanel::PortraitCreationCompleted()
{
    ((CSWGuiMainCharGen*)m_pParentPanel)->PortraitChanged();
    SetState(1);
}
// PC 0x007605F0 GOG 0x00760430
void CSWGuiCustomPanel::NameCreationCompleted()
{
    ((CSWGuiMainCharGen*)m_pParentPanel)->NameChanged();
    SetState(5);
}
// PC 0x00760610 GOG 0x00760450
void CSWGuiCustomPanel::AbilityCreationCompleted()
{
    ((CSWGuiMainCharGen*)m_pParentPanel)->AbilitiesChanged();
    SetState(2);
}
// PC 0x00760630 GOG 0x00760470
void CSWGuiCustomPanel::SkillCreationCompleted()
{
    SetState(3);
}
// PC 0x00760640 GOG 0x00760480
void CSWGuiCustomPanel::FeatCreationCompleted()
{
    SetState(4);
}
// PC 0x00760650 GOG 0x00760490
void CSWGuiCustomPanel::ClearAbilities()
{
#ifdef _XBOX
    ((CSWGuiMainCharGen*)m_pParentPanel)->ClearAbilities();
#else
    CSWGuiMainCharGen* pMainCharGen = (CSWGuiMainCharGen*)m_pParentPanel;
    if (pMainCharGen != NULL)
    {
        pMainCharGen->ClearAbilities();
    }
#endif
    m_pCreature->m_pStats->m_nCreationStage = 30;
}
// PC 0x00760680 GOG 0x007604F0
void CSWGuiCustomPanel::CancelPanel()
{
#ifdef _XBOX
    if (m_nState == 0 && m_pParentPanel != NULL)
    {
        ((CSWGuiMainCharGen*)m_pParentPanel)->SetVisiblePanel(1);
    }
    else
    {
        m_nState--;
        SetState(m_nState);
        if (m_nState - 1 == 0)
        {
            ClearAbilities();
        }
    }
#else
    CSWGuiMainCharGen* pMainCharGen = (CSWGuiMainCharGen*)m_pParentPanel;
    if (pMainCharGen != NULL)
    {
        pMainCharGen->SetVisiblePanel(1);
        ClearAbilities();
    }
    SetState(0);
#endif
}
// PC 0x007606C0 GOG 0x00760170
void CSWGuiCustomPanel::OnSelectPlayButton(CSWGuiControl* pControl)
{
    if (m_nState >= 5)
    {
        if (pControl->m_bSelected != 0)
        {
            ((CSWGuiMainCharGen*)m_pParentPanel)->BeginPlayGame();
        }
    }
}
// PC 0x007606E0 GOG 0x00760530
void CSWGuiCustomPanel::ConfirmCancel(CSWGuiControl* pControl)
{
    CGuiInGame* pInGame = g_pAppManager->m_pClientExoApp->GetInGameGui();
    if (pInGame->m_pMessageBox->m_bAccepted)
    {
        CancelPanel();
    }
}
// PC 0x00760740 GOG 0x00760590
void CSWGuiCustomPanel::HandleInputEvent(int nEvent, int nState)
{
    int nCurrent;

    if (nState != 0)
    {
        switch (nEvent)
        {
        case 40:
        case 46:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
#ifdef _XBOX
            CancelPanel();
#else
            nCurrent = m_nState;
            if (nCurrent == 0)
            {
                CancelPanel();
                break;
            }
            m_nState = nCurrent - 1;
            SetState(nCurrent - 1);
            switch (m_nState)
            {
            case CUSTOM_PANEL_STATE_ABILITIES:
                ClearAbilities();
                break;
            case CUSTOM_PANEL_STATE_SKILLS:
                if (m_pParentPanel != 0)
                {
                    ((CSWGuiMainCharGen*)m_pParentPanel)->ClearFeats();
                }
                break;
            }
#endif
            break;
        case 45:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            break;
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
// PC 0x00760800 GOG 0x00760650
void CSWGuiCustomPanel::OnCancelPressed(CSWGuiControl* pControl)
{
    if (m_nState != 0)
    {
        CSWGuiMessageBox* pMessageBox =
            g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
        pMessageBox->SetAllowCancel(1);
        pMessageBox->SetMessage(109863);
        pMessageBox->SetCallback(
            this, (CSWGuiEventHandler)&CSWGuiCustomPanel::ConfirmCancel, 0);
        m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
    }
    else
    {
        CancelPanel();
    }
}
// PC 0x00760890 GOG 0x007606E0
CSWGuiCustomPanel::CSWGuiCustomPanel(CSWGuiManager* pManager,
                                     CSWCCreature* pCreature)
    : CSWGuiPanel(pManager)
{
    CExoString sName;

    m_pManager = g_pAppManager->m_pClientExoApp->GetSWGuiManager();
    m_pCreature = pCreature;
#ifdef _XBOX
    StartLoadFromLayout("CUSTPNL");
#else
    StartLoadFromLayout("CUSTPNL_p", 0, 1);
#endif

    for (int i = 0; i < 6; i++)
    {
        sName.Format("LBL_%d", i + 1);
        InitControl(m_aHilights[i], sName, 1);
        m_aHilights[i].m_cHilite.m_cParams.SetPulsingAlpha(1, 0, 1);
        sName.Format("BTN_STEPNAME%d", i + 1);
        InitControl(m_aSteps[i], sName, 1);
        sName.Format("LBL_NUM%d", i + 1);
        InitControl(m_aNumbers[i], sName, 1);
    }

#ifndef _XBOX
    CSWGuiButton* pBack = &m_btnBack;
    InitControl(*pBack, "BTN_BACK", 1);
    CSWGuiButton* pCancel = &m_btnCancel;
    InitControl(*pCancel, "BTN_CANCEL", 1);
#endif
    StopLoadFromLayout();

    m_aSteps[0].AddEvent(CSWGuiControl::EVENT_SELECT, this,
        (PFNGUIEVENT)&CSWGuiCustomPanel::OnSelectPortraitButton);
    m_aSteps[1].AddEvent(CSWGuiControl::EVENT_SELECT, this,
        (PFNGUIEVENT)&CSWGuiCustomPanel::OnSelectAbilitiesButton);
    m_aSteps[2].AddEvent(CSWGuiControl::EVENT_SELECT, this,
        (PFNGUIEVENT)&CSWGuiCustomPanel::OnSelectSkillsButton);
    m_aSteps[3].AddEvent(CSWGuiControl::EVENT_SELECT, this,
        (PFNGUIEVENT)&CSWGuiCustomPanel::OnSelectFeatsButton);
    m_aSteps[4].AddEvent(CSWGuiControl::EVENT_SELECT, this,
        (PFNGUIEVENT)&CSWGuiCustomPanel::OnSelectNameButton);
    m_aSteps[5].AddEvent(CSWGuiControl::EVENT_SELECT, this,
        (PFNGUIEVENT)&CSWGuiCustomPanel::OnSelectPlayButton);
    m_aSteps[0].AddEvent(CSWGuiControl::EVENT_CONFIRM, this,
        (PFNGUIEVENT)&CSWGuiCustomPanel::OnSelectPortraitButton);
    m_aSteps[1].AddEvent(CSWGuiControl::EVENT_CONFIRM, this,
        (PFNGUIEVENT)&CSWGuiCustomPanel::OnSelectAbilitiesButton);
    m_aSteps[2].AddEvent(CSWGuiControl::EVENT_CONFIRM, this,
        (PFNGUIEVENT)&CSWGuiCustomPanel::OnSelectSkillsButton);
    m_aSteps[3].AddEvent(CSWGuiControl::EVENT_CONFIRM, this,
        (PFNGUIEVENT)&CSWGuiCustomPanel::OnSelectFeatsButton);
    m_aSteps[4].AddEvent(CSWGuiControl::EVENT_CONFIRM, this,
        (PFNGUIEVENT)&CSWGuiCustomPanel::OnSelectNameButton);
    m_aSteps[5].AddEvent(CSWGuiControl::EVENT_CONFIRM, this,
        (PFNGUIEVENT)&CSWGuiCustomPanel::OnSelectPlayButton);

#ifndef _XBOX
    pBack->m_bFocusable = 0;
    pBack->AddEvent(CSWGuiControl::EVENT_SELECT, this,
        (PFNGUIEVENT)&CSWGuiPanel::OnBButtonPressed);
    pCancel->m_bFocusable = 0;
    pCancel->AddEvent(CSWGuiControl::EVENT_SELECT, this,
        (PFNGUIEVENT)&CSWGuiCustomPanel::OnCancelPressed);
#endif
    SetState(CUSTOM_PANEL_STATE_PORTRAIT);
#ifdef _XBOX
    m_bCenterHorizontal = 1;
    m_bCenterVertical = 1;
#endif
}
