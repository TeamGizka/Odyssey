#include "swguileveluppanel.h"
#include "swguiskillschargen.h"
#include "swguipowerslvlup.h"
#include "swguilevelupchargen.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "swguiingame.h"
#include "nwccreature.h"
// PC 0x0075D050
void CSWGuiLevelUpPanel::NewAbilityScores(int nStr, int nDex, int nCon,
                                          int nInt, int nWis, int nCha,
                                          int nPoints)
{
    m_pParent->NewAbilityScores(nStr, nDex, nCon, nInt, nWis, nCha, nPoints);
}
// PC 0x0075D0E0
void CSWGuiLevelUpPanel::ChangeState(int bBack)
{
    int nStep;

#ifdef _XBOX
    if (bBack != 0 && m_bCanCancel != 0 && m_nState == m_nFirstState)
    {
        m_pParent->CancelLevelUp();
        CSWGuiSkillInfoBox* pInfoBox =
            g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pSkillInfoBox;
        if (m_pManager->PanelExists(pInfoBox))
        {
            m_pManager->RemovePanel(pInfoBox);
        }
        return;
    }
#else
    if (bBack != 0)
    {
        if (m_nState == m_nFirstState)
        {
            if (m_bCanCancel != 0)
            {
                m_pParent->CancelLevelUp();
                CSWGuiSkillInfoBox* pInfoBox =
                    g_pAppManager->m_pClientExoApp->GetInGameGui()
                        ->m_pSkillInfoBox;
                if (m_pManager->PanelExists(pInfoBox))
                {
                    m_pManager->RemovePanel(pInfoBox);
                }
            }
            return;
        }
    }
#endif

    BOOL bSearching = TRUE;
    if (bBack != 0)
    {
        nStep = m_nState - 1;
    }
    else
    {
        nStep = m_nState + 1;
    }

    while (bSearching && nStep >= 0 && nStep < STATE_COUNT)
    {
        if (m_anStateUsed[nStep] == 1)
        {
            bSearching = FALSE;
        }
        else if (bBack != 0)
        {
            nStep--;
        }
        else
        {
            nStep++;
        }
    }

    if (nStep >= 0 && nStep < STATE_COUNT)
    {
        switch (nStep)
        {
        case STATE_POWERS:
            m_pParent->ClearPowers();
            break;
        case STATE_ABILITIES:
            m_pParent->ClearAbilities();
            break;
        case STATE_SKILLS:
            m_pParent->ClearSkills();
            break;
        case STATE_FEATS:
            m_pParent->ClearFeats();
            break;
        default:
            break;
        }
        m_nState = nStep;
        for (int i = 0; i < STATE_COUNT; i++)
        {
            if (i == m_nState)
            {
#ifndef _XBOX
                m_aNumbers[i].SetEnabled(1);
                m_aSteps[i].SetEnabled(1);
#endif
                m_aHilights[i].SetActive(1);
                m_aNumbers[i].m_cText.m_cParams.SetColor(
                    CGuiInGame::COLOR_YELLOW);
                m_aSteps[i].m_cText.m_cParams.SetColor(
                    CGuiInGame::COLOR_YELLOW);
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
#ifdef _XBOX
                    m_aNumbers[i].m_cText.m_cParams.SetColor(
                        CGuiInGame::COLOR_BLUE);
                    m_aSteps[i].m_cText.m_cParams.SetColor(
                        CGuiInGame::COLOR_BLUE);
                }
                else
                {
                    m_aNumbers[i].m_cText.m_cParams.SetColor(
                        CGuiInGame::DISABLED_TEXT);
                    m_aSteps[i].m_cText.m_cParams.SetColor(
                        CGuiInGame::DISABLED_TEXT);
#else
                    m_aNumbers[i].SetEnabled(0);
                    m_aSteps[i].SetEnabled(0);
                }
                else
                {
                    m_aNumbers[i].SetEnabled(0);
                    m_aSteps[i].SetEnabled(0);
#endif
                }
            }
        }
        SetActiveControl(&m_aSteps[m_nState], 0);
    }

#ifdef _XBOX
    if (m_nState == m_nFirstState)
    {
        m_pParent->SetBackPromptEnabled(0);
    }
    else
    {
        m_pParent->SetBackPromptEnabled(1);
    }
#else
    BOOL bEnable = (m_nState != m_nFirstState) || (m_bCanCancel != 0);
    m_btnBack.SetEnabled(bEnable);
    Vector* pColor = &CGuiInGame::COLOR_BLUE;
    if (!bEnable)
    {
        pColor = &CGuiInGame::DISABLED_TEXT;
    }
    m_btnBack.m_cText.m_cParams.SetColor(*pColor);
#endif
}
// PC 0x0075CE20
void CSWGuiLevelUpPanel::OnSelectPowersButton(CSWGuiControl* pControl)
{
    CSWGuiPowersLevelUp* pPanel;

    int bSelected = pControl->m_bSelected;
    if (bSelected != 0)
    {
        pPanel =
            new CSWGuiPowersLevelUp(m_pManager, m_pCreature->GetObjectId());
        pPanel->m_pParentPanel = this;
        m_pManager->AddPanel(pPanel, CSWGuiManager::PANEL_MODE_MODAL, 1);
    }
}
// PC 0x0075CEA0
void CSWGuiLevelUpPanel::OnSelectFeatsButton(CSWGuiControl* pControl)
{
    CSWGuiFeatsCharGen* pPanel;
    unsigned char nGeneralFeats;
    unsigned char nBonusFeats;

    int bSelected = pControl->m_bSelected;
    if (bSelected != 0)
    {
        nGeneralFeats = 0;
        nBonusFeats = 0;
        m_pCreature->GetStats()->CalcNumberFeats(nGeneralFeats, nBonusFeats);
        pPanel = new CSWGuiFeatsCharGen(m_pManager, m_pCreature, 1,
                                        nGeneralFeats == 0);
        pPanel->m_pParentPanel = this;
        m_pManager->AddPanel(pPanel, CSWGuiManager::PANEL_MODE_MODAL, 1);
    }
}
// PC 0x0075CF50
void CSWGuiLevelUpPanel::OnSelectSkillsButton(CSWGuiControl* pControl)
{
    CSWGuiSkillsCharGen* pPanel;

    int bSelected = pControl->m_bSelected;
    if (bSelected != 0)
    {
        pPanel = new CSWGuiSkillsCharGen(m_pManager, m_pCreature, 1);
        pPanel->m_pParentPanel = this;
        m_pManager->AddPanel(pPanel, CSWGuiManager::PANEL_MODE_MODAL, 1);
    }
}
// PC 0x0075CFD0
void CSWGuiLevelUpPanel::OnSelectAbilitiesButton(CSWGuiControl* pControl)
{
    CSWGuiAbilitiesCharGen* pPanel;

    int bSelected = pControl->m_bSelected;
    if (bSelected != 0)
    {
        pPanel = new CSWGuiAbilitiesCharGen(m_pManager, m_pCreature, 1);
        pPanel->m_pParentPanel = this;
        m_pManager->AddPanel(pPanel, CSWGuiManager::PANEL_MODE_MODAL, 1);
    }
}
// PC 0x0075D350
void CSWGuiLevelUpPanel::CancelledByChild()
{
    --m_nState;
    ChangeState(0);
    m_pParent->SetVisible(1);
}
// PC 0x0075D380
void CSWGuiLevelUpPanel::AbilitySelectionCompleted()
{
    m_pParent->SetVisible(1);
    m_pParent->AbilitiesChanged();
    ChangeState(0);
}
// PC 0x0075D3A0
void CSWGuiLevelUpPanel::FeatSelectionCompleted()
{
    m_pParent->SetVisible(1);
    ChangeState(0);
}
// PC 0x0075D3A0
void CSWGuiLevelUpPanel::SkillSelectionCompleted()
{
    m_pParent->SetVisible(1);
    ChangeState(0);
}
// PC 0x0075D3A0
void CSWGuiLevelUpPanel::PowersSelectionCompleted()
{
    m_pParent->SetVisible(1);
    ChangeState(0);
}

#ifndef _XBOX
void CSWGuiLevelUpPanel::OnCancelPressed(CSWGuiControl* pControl)  // PC 0x0075D430
{
    if (m_bCanCancel != 0)
    {
        m_pParent->ClearPowers();
        m_pParent->ClearFeats();
        m_pParent->ClearSkills();
        m_pParent->ClearAbilities();
        m_nState = 0;
        ChangeState(1);
    }
}
// PC 0x0075D060
void CSWGuiLevelUpPanel::SetCanCancel(int bCanCancel)
{
    m_bCanCancel = bCanCancel;
    BOOL bEnable = (m_nState != m_nFirstState) || (bCanCancel != 0);
    m_btnBack.SetEnabled(bEnable);
    Vector* pColor = &CGuiInGame::COLOR_BLUE;
    if (!bEnable)
    {
        pColor = &CGuiInGame::DISABLED_TEXT;
    }
    m_btnBack.m_cText.m_cParams.SetColor(*pColor);
    m_btnCancel.SetShown(m_bCanCancel);
}
#endif
// PC 0x0075DD30
void CSWGuiLevelUpPanel::OnPanelAdded()
{
    m_nState = -1;
    ChangeState(0);
    m_nFirstState = m_nState;
#ifdef _XBOX
    m_pParent->SetBackPromptEnabled(m_bCanCancel);
#else
    if (m_bCanCancel == 0)
    {
        m_btnBack.SetEnabled(0);
        m_btnBack.m_cText.m_cParams.SetColor(CGuiInGame::DISABLED_TEXT);
        m_btnCancel.m_bEnabled = 0;
    }
    CSWGuiPanel::OnPanelAdded();
#endif
}
// PC 0x0075D530
void CSWGuiLevelUpPanel::HandleInputEvent(int nEvent, int nState)
{
    if (nState != 0)
    {
        switch (nEvent)
        {
        case 40:
        case 46:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            ChangeState(1);
            break;
        case 45:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            break;
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}

void CSWGuiLevelUpPanel::PlayGame()
{
    if (m_anStateUsed[2] == 0)
    {
        m_pCreature->m_pStats->AddGrantedFeats(0);
        FeatSelectionCompleted();
    }
    m_pParent->BeginPlayGame();
}

void CSWGuiLevelUpPanel::OnSelectAcceptButton(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        PlayGame();
    }
}
// PC 0x0075D5B0
CSWGuiLevelUpPanel::CSWGuiLevelUpPanel(CSWGuiManager* pManager,
                                       unsigned long nCreature)
    : CSWGuiPanel(pManager)
{
    CExoString sName;
    int nNumber;

    m_pManager = g_pAppManager->GetClientExoApp()->GetSWGuiManager();
    m_pCreature =
        g_pAppManager->GetClientExoApp()->GetCreatureByGameObjectID(nCreature);
    m_bCanCancel = 0;
    nNumber = 0;

    StartLoadFromLayout("LEVELUPPNL");
    for (int i = 0; i < 5; i++)
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
    InitControl(m_btnBack, "BTN_BACK", 1);
    InitControl(m_btnCancel, "BTN_CANCEL", 1);
#endif
    StopLoadFromLayout();

    if (m_pCreature->GetStats()->GetLevel() % 4 == 0)
    {
        m_aSteps[0].AddEvent(
            39, this,
            (PFNGUIEVENT)&CSWGuiLevelUpPanel::OnSelectAbilitiesButton);
        m_aSteps[0].AddEvent(
            45, this,
            (PFNGUIEVENT)&CSWGuiLevelUpPanel::OnSelectAbilitiesButton);
        m_anStateUsed[0] = 1;
        nNumber = 1;
        m_aNumbers[0].m_cText.m_cParams.SetText(nNumber);
    }
    else
    {
        m_aSteps[0].SetEnabledFlag(0);
        m_aHilights[0].SetEnabledFlag(0);
        m_aNumbers[0].SetEnabledFlag(0);
        m_anStateUsed[0] = 0;
    }

    m_pCreature->GetStats()->m_nSkillPoints = 0;
    m_pCreature->GetStats()->AddSkillPointsPerLevel();
    int nSkillPoints = m_pCreature->GetStats()->m_nSkillPoints;
    if (nSkillPoints > 0)
    {
        m_aSteps[1].AddEvent(
            39, this, (PFNGUIEVENT)&CSWGuiLevelUpPanel::OnSelectSkillsButton);
        m_aSteps[1].AddEvent(
            45, this, (PFNGUIEVENT)&CSWGuiLevelUpPanel::OnSelectSkillsButton);
        nNumber++;
        m_anStateUsed[1] = 1;
        m_aNumbers[1].m_cText.m_cParams.SetText(nNumber);
    }
    else
    {
        m_aSteps[1].SetEnabledFlag(0);
        m_aHilights[1].SetEnabledFlag(0);
        m_aNumbers[1].SetEnabledFlag(0);
        m_anStateUsed[1] = 0;
    }

    unsigned char nGeneralFeats = 0;
    unsigned char nBonusFeats = 0;
    m_pCreature->GetStats()->CalcNumberFeats(nGeneralFeats, nBonusFeats);
    if (nGeneralFeats + nBonusFeats > 0)
    {
        m_aSteps[2].AddEvent(
            39, this, (PFNGUIEVENT)&CSWGuiLevelUpPanel::OnSelectFeatsButton);
        m_aSteps[2].AddEvent(
            45, this, (PFNGUIEVENT)&CSWGuiLevelUpPanel::OnSelectFeatsButton);
        nNumber++;
        m_anStateUsed[2] = 1;
        m_aNumbers[2].m_cText.m_cParams.SetText(nNumber);
    }
    else
    {
        m_aSteps[2].SetEnabledFlag(0);
        m_aHilights[2].SetEnabledFlag(0);
        m_aNumbers[2].SetEnabledFlag(0);
        m_anStateUsed[2] = 0;

        CExoArrayList<unsigned long> lstGrantedFeats;
        m_pCreature->GetStats()->AddGrantedFeats(&lstGrantedFeats);
        if (lstGrantedFeats.num > 0)
        {
            CSWGuiSkillInfoBox* pInfoBox =
                g_pAppManager->GetClientExoApp()->GetInGameGui()->m_pSkillInfoBox;
            pInfoBox->SetSkillList(lstGrantedFeats, CSWGuiInGameSkillEntry::ABILITY_FEAT);
            pInfoBox->SetMessage(0xA512);
            m_pManager->AddPanel(pInfoBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
        }
    }

    int nSpells = m_pCreature->GetStats()->ComputeNumberKnownSpellsLeft();
    if (nSpells > 0)
    {
        m_aSteps[3].AddEvent(
            39, this, (PFNGUIEVENT)&CSWGuiLevelUpPanel::OnSelectPowersButton);
        m_aSteps[3].AddEvent(
            45, this, (PFNGUIEVENT)&CSWGuiLevelUpPanel::OnSelectPowersButton);
        nNumber++;
        m_anStateUsed[3] = 1;
        m_aNumbers[3].m_cText.m_cParams.SetText(nNumber);
    }
    else
    {
        m_aSteps[3].SetEnabledFlag(0);
        m_aHilights[3].SetEnabledFlag(0);
        m_aNumbers[3].SetEnabledFlag(0);
        m_anStateUsed[3] = 0;
    }

    m_aSteps[4].AddEvent(
        39, this, (PFNGUIEVENT)&CSWGuiLevelUpPanel::OnSelectAcceptButton);
    m_aSteps[4].AddEvent(
        45, this, (PFNGUIEVENT)&CSWGuiLevelUpPanel::OnSelectAcceptButton);
    nNumber++;
    m_anStateUsed[4] = 1;
    m_aNumbers[4].m_cText.m_cParams.SetText(nNumber);

    m_bCenterHorizontal = 1;
    m_bCenterVertical = 1;
#ifndef _XBOX
    m_btnBack.SetFocusable(0);
    m_btnBack.AddEvent(39, this,
                       (PFNGUIEVENT)&CSWGuiPanel::OnBButtonPressed);
    m_btnCancel.SetFocusable(0);
    m_btnCancel.AddEvent(39, this,
                         (PFNGUIEVENT)&CSWGuiLevelUpPanel::OnCancelPressed);
#endif
}

