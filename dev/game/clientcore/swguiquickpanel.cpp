#include <string.h>

#include "exobase.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "swguiquickpanel.h"

#include "guiingame.h"
#include "swguiingame.h"
#include "nwccreature.h"
#include "nwclass.h"
#include "nwccreaturestats.h"

void CSWGuiQuickPanel::CancelPanel()
{
#ifdef _XBOX
    if (m_nState == 0 && m_pMainCharGen != 0)
    {
        m_pMainCharGen->SetVisiblePanel(1);
        m_pMainCharGen->ClearAbilities();
    }
    else
    {
        m_nState--;
        SetState(m_nState);
    }
#else
    if (m_pMainCharGen != 0)
    {
        m_pMainCharGen->SetVisiblePanel(1);
        m_pMainCharGen->ClearAbilities();
    }
    SetState(0);
#endif
}
// PC 0x00760D20
void CSWGuiQuickPanel::SetState(int nState)
{
    m_nState = nState;
#ifndef _XBOX
    if (nState == 0)
    {
        m_btnBack.SetEnabled(0);
        m_btnBack.m_cText.m_cParams.SetColor(CGuiInGame::DISABLED_TEXT);
    }
    else
    {
        m_btnBack.SetEnabled(1);
        m_btnBack.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
    }
#endif
    for (int i = 0; i < 3; i++)
    {
        if (i == m_nState)
        {
            m_lblHilight[i].SetActive(1);
#ifndef _XBOX
            m_lblNumber[i].SetEnabled(1);
            m_btnStep[i].SetEnabled(1);
#endif
            m_lblNumber[i].m_cText.m_cParams.SetColor(
                CGuiInGame::COLOR_YELLOW);
            m_btnStep[i].m_cText.m_cParams.SetColor(CGuiInGame::COLOR_YELLOW);
            CSWGuiTextParams* pParams = &m_lblNumber[i].m_cText.m_cParams;
            pParams->m_bHilightText = 1;
            pParams = &m_btnStep[i].m_cText.m_cParams;
            pParams->m_bHilightText = 1;
        }
        else
        {
            m_lblHilight[i].SetActive(0);
            CSWGuiTextParams* pParams = &m_lblNumber[i].m_cText.m_cParams;
            pParams->m_bHilightText = 0;
            pParams = &m_btnStep[i].m_cText.m_cParams;
            pParams->m_bHilightText = 0;
            if (i < m_nState)
            {
#ifdef _XBOX
                m_lblNumber[i].m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
                m_btnStep[i].m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
#else
                m_lblNumber[i].SetEnabled(0);
                m_btnStep[i].SetEnabled(0);
#endif
            }
            else
            {
#ifdef _XBOX
                m_lblNumber[i].m_cText.m_cParams.SetColor(CGuiInGame::DISABLED_TEXT);
                m_btnStep[i].m_cText.m_cParams.SetColor(CGuiInGame::DISABLED_TEXT);
#else
                m_lblNumber[i].SetEnabled(0);
                m_btnStep[i].SetEnabled(0);
#endif
            }
        }
    }
    SetActiveControl(&m_btnStep[m_nState], 0);
}
// PC 0x00761180
void CSWGuiQuickPanel::OnSelectPlayButton(CSWGuiControl* pControl)
{
#ifndef _XBOX
    if (m_nState >= 2)
#endif
    {
        if (pControl->m_bSelected != 0)
        {
            m_pMainCharGen->BeginPlayGame();
        }
    }
}
// PC 0x00760F30
void CSWGuiQuickPanel::OnSelectNameButton(CSWGuiControl* pControl)
{
    CSWGuiNameChargen* pPanel;
    CSWGuiNameChargen* pNew;

#ifndef _XBOX
    if (m_nState >= 1)
#endif
    {
        int nResult = pControl->m_bSelected;
        pPanel = NULL;
        if (nResult != 0)
        {
            pNew = new CSWGuiNameChargen(m_pManager, m_pCreature);
            pPanel = pNew;
            pPanel->SetParentPanel(this, 1);
            m_pManager->AddPanel(pPanel, CSWGuiManager::PANEL_MODE_MODAL, 1);
        }
    }
}
// PC 0x00760FC0
void CSWGuiQuickPanel::OnSelectPortraitButton(CSWGuiControl* pControl)
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
            pPortrait->SetParentPanel(this, 1);
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
// PC 0x007610C0
void CSWGuiQuickPanel::CancelledByChild()
{
    if (m_nState == 0)
    {
        m_pMainCharGen->PortraitChanged();
    }
    m_btnStep[m_nState].m_cText.m_cParams.SetColor(CGuiInGame::COLOR_YELLOW);
    CSWGuiButton* pButton = &m_btnStep[m_nState];
    CSWGuiTextParams* pParams = &pButton->m_cText.m_cParams;
    pParams->m_bHilightText = 1;
}
// PC 0x00761110
void CSWGuiQuickPanel::PortraitCreationCompleted()
{
    m_pMainCharGen->PortraitChanged();
    SetState(1);
}
// PC 0x00761130
void CSWGuiQuickPanel::NameCreationCompleted()
{
    m_pMainCharGen->NameChanged();
    SetState(2);
}
// PC 0x00761150
CExoString CSWGuiCustomPanel::GetCurrentName()
{
    return ((CSWGuiMainCharGen*)m_pParentPanel)->m_sName;
}
// PC 0x00761270
void CSWGuiQuickPanel::DoQuickStats()
{
    unsigned char nIndex = m_pCreature->GetStats()->GetNumClasses() - 1;
    int nClass = m_pCreature->GetStats()->GetClass(nIndex);
    CSWClass* pClass = &g_pRules->m_pClasses[nClass];
    m_pCreature->GetStats()->SetSTRBase(pClass->GetStrength());
    m_pCreature->GetStats()->SetDEXBase(pClass->GetDexterity(), 1);
    m_pCreature->GetStats()->SetCONBase(pClass->GetConstitution());
    m_pCreature->GetStats()->SetWISBase(pClass->GetWisdom());
    m_pCreature->GetStats()->SetINTBase(pClass->GetIntelligence());
    m_pCreature->GetStats()->SetCHABase(pClass->GetCharisma());
    CExoArrayList<unsigned short> lstFeats;
    m_pCreature->GetStats()->AddGrantedFeats(0);
    m_pCreature->GetStats()->AddDefaultPackageFeats(nClass, &lstFeats, 1, 1);
    for (int i = 0; i < lstFeats.num; i++)
    {
        m_pCreature->GetStats()->m_lstFeats.AddUnique(lstFeats[i]);
    }
    m_pCreature->GetStats()->m_nSkillPoints = 0;
    m_pCreature->GetStats()->AddSkillPointsPerLevel();
    m_pCreature->GetStats()->AddDefaultPackageSkills(nClass, 1);
    m_pCreature->GetStats()->UpdateAbilityModifiedSkillRanks();
}
// PC 0x007611A0
void CSWGuiQuickPanel::ConfirmCancel(CSWGuiControl* pControl)
{
    CGuiInGame* pInGameGui = g_pAppManager->m_pClientExoApp->GetInGameGui();
    CSWGuiMessageBox* pMessageBox = pInGameGui->m_pMessageBox;
    if (pMessageBox->m_bAccepted)
    {
        CancelPanel();
    }
}
// PC 0x00761950
void CSWGuiQuickPanel::OnPanelAdded()
{
    DoQuickStats();
    m_pMainCharGen->AbilitiesChanged();
#ifndef _XBOX
    CSWGuiPanel::OnPanelAdded();
#endif
}
// PC 0x007611E0
void CSWGuiQuickPanel::HandleInputEvent(int nEvent, int bPressed)
{
    if (bPressed != 0)
    {
        switch (nEvent)
        {
        case 40:
        case 46:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
#ifdef _XBOX
            CancelPanel();
#else
            if (m_nState == 0)
            {
                CancelPanel();
            }
            else
            {
                m_nState--;
                SetState(m_nState);
            }
#endif
            break;
        case 45:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            break;
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, bPressed);
}

CExoString CSWGuiQuickPanel::GetCurrentName()
{
    return m_pMainCharGen->m_sName;
}
// PC 0x007614D0
void CSWGuiQuickPanel::OnCancelPressed(CSWGuiControl* pControl)
{
    if (m_nState != 0)
    {
        CSWGuiMessageBox* pMessageBox =
            g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
        pMessageBox->SetAllowCancel(1);
        pMessageBox->SetMessage(48541);
        pMessageBox->SetCallback(
            this, (CSWGuiEventHandler)&CSWGuiQuickPanel::ConfirmCancel, 0);
        m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
    }
    else
    {
        CancelPanel();
    }
}
// PC 0x00761550
CSWGuiQuickPanel::CSWGuiQuickPanel(CSWGuiManager* pManager,
                                   CSWCCreature* pCreature)
    : CSWGuiPanel(pManager)
{
    CExoString sName;

    m_pCreature = pCreature;
    StartLoadFromLayout("QUICKPNL");
    for (int i = 0; i < 3; i++)
    {
        sName.Format("LBL_%d", i + 1);
        InitControl(m_lblHilight[i], sName, 1);
        m_lblHilight[i].m_cHilite.m_cParams.SetPulsingAlpha(1, 0, 1);
        sName.Format("BTN_STEPNAME%d", i + 1);
        InitControl(m_btnStep[i], sName, 1);
        sName.Format("LBL_NUM%d", i + 1);
        InitControl(m_lblNumber[i], sName, 1);
    }
#ifndef _XBOX
    CSWGuiButton* pBackButton = &m_btnBack;
    InitControl(*pBackButton, "BTN_BACK", 1);
    CSWGuiButton* pCancelButton = &m_btnCancel;
    InitControl(*pCancelButton, "BTN_CANCEL", 1);
#endif
    StopLoadFromLayout();

    m_btnStep[0].AddEvent(
        GUIEVENT_ACTIVATE, this,
        (CSWGuiEventHandler)&CSWGuiQuickPanel::OnSelectPortraitButton);
    m_btnStep[1].AddEvent(
        GUIEVENT_ACTIVATE, this, (CSWGuiEventHandler)&CSWGuiQuickPanel::OnSelectNameButton);
    m_btnStep[2].AddEvent(
        GUIEVENT_ACTIVATE, this, (CSWGuiEventHandler)&CSWGuiQuickPanel::OnSelectPlayButton);
    m_btnStep[0].AddEvent(
        GUIEVENT_ACCEPT, this,
        (CSWGuiEventHandler)&CSWGuiQuickPanel::OnSelectPortraitButton);
    m_btnStep[1].AddEvent(
        GUIEVENT_ACCEPT, this, (CSWGuiEventHandler)&CSWGuiQuickPanel::OnSelectNameButton);
#ifdef _XBOX
    m_btnStep[2].AddEvent(
        GUIEVENT_ACCEPT, this, (CSWGuiEventHandler)&CSWGuiQuickPanel::OnSelectPlayButton);
#else
    m_btnStep[2].AddEvent(
        GUIEVENT_ACCEPT, this, (CSWGuiEventHandler)&CSWGuiQuickPanel::OnCancelPressed);
    pBackButton->m_bFocusable = 0;
    pBackButton->AddEvent(GUIEVENT_ACTIVATE, this, (CSWGuiHandler)&CSWGuiPanel::OnBButtonPressed);
    pCancelButton->m_bFocusable = 0;
    pCancelButton->AddEvent(
        GUIEVENT_ACTIVATE, this, (CSWGuiEventHandler)&CSWGuiQuickPanel::OnCancelPressed);
#endif
    SetState(0);
    m_bCenterHorizontal = 1;
    m_bCenterVertical = 1;
}
