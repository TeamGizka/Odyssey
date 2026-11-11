#include "exobase.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "serverexoapp.h"
#include "partytables.h"
#include "nwclass.h"
#include "nwscript.h"
#include "nwrace.h"
#include "nwskill.h"
#include "nwccreaturestats.h"
#include "swguiingame.h"
#include "guiingame.h"
#include "swguiabilitieschargen.h"
#include "swguiskillschargen.h"
#include "nwccreature.h"
// PC 0x006F51D0  Xbox 0x0012D460
CSWGuiSkillsCharGen::CSWGuiSkillsCharGen(CSWGuiManager* pManager,
                                       CSWCCreature* pCreature, int bLevelUp)
    : CSWGuiPanel(pManager)
{
    m_pCreature = pCreature;
    m_bLevelUp = bLevelUp;
    CAppManager* pAppManager;
    m_bAllSkills = !bLevelUp ||
        (pCreature->GetServerCreature()->m_pStats->m_bIsPC &&
         (pAppManager = g_pAppManager)->GetServerExoApp() &&
         pAppManager->GetServerExoApp()->GetPartyTable() &&
         g_pAppManager->GetServerExoApp()->GetPartyTable()->m_nControlledNPC == NPC_PLAYER);
    m_bDroid = m_pCreature->m_pStats->m_nRace == CSWRace::CRACE_DROID;
    unsigned char nClass = m_pCreature->m_pStats->GetClass(
        m_pCreature->m_pStats->GetNumClasses() - 1);
    StartLoadFromLayout("SKCHRGEN");
    InitControl(m_lblTitle, "MAIN_TITLE_LBL", 1);
    if (bLevelUp)
    {

        m_lblTitle.m_cText.m_cParams.SetStrRef(STRREF_LEVEL_UP);
    }
    InitControl(m_lblSubTitle, "SUB_TITLE_LBL", 1);
    InitControl(m_lblRemainingBox, "REMAINING_BOX_LBL", 1);
    InitControl(m_lblRemainingSelections, "REMAINING_SELECTIONS_LBL", 1);
    InitControl(m_lblSelectionsRemaining, "SELECTIONS_REMAINING_LBL", 1);
    InitControl(m_lblDescriptionHeader, "DESC_LBL", 1);
    InitControl(m_lblCost, "COST_LBL", 1);
    InitControl(m_lblCostPoints, "COST_POINTS_LBL", 1);
    InitControl(m_lblClassSkill, "CLASSSKL_LBL", 1);
    InitControl(m_lbDescription, "LB_DESC", 1);
    m_lbDescription.m_nSelectSound = CSWGuiManager::GUISOUND_SELECTION;
#ifdef _XBOX
    InitControl(m_lblA, "LBL_A", 1);
    InitControl(m_lblB, "LBL_B", 1);
    InitControl(m_lblY, "LBL_Y", 1);
    InitControl(m_lblAccept, "LBL_ACCEPT", 1);
    InitControl(m_lblRecommended, "LBL_RECOMMENDED", 1);
    InitControl(m_lblBack, "LBL_BACK", 1);
    InitControl(m_lblRanalog, "LBL_RANALOG", 1);
#else
    InitControl(m_btnAccept, "BTN_ACCEPT", 1);
    InitControl(m_btnRecommended, "BTN_RECOMMENDED", 1);
    InitControl(m_btnBack, "BTN_BACK", 1);
#endif
#ifdef _XBOX
    InitControl(m_lblSkill[0], "COMPUTER_USE_LBL", 1);
    InitControl(m_lblMinus[0], "COMPUTER_USE_MINUS_LBL", 1);
    InitControl(m_btnPoints[0], "COMPUTER_USE_POINTS_BTN", 1);
    InitControl(m_lblPlus[0], "COMPUTER_USE_PLUS_LBL", 1);
    m_sDescription[0] = g_pRules->m_pSkills[0].GetDescriptionText();
#else
    InitControl(m_btnPoints[0], "COMPUTER_USE_POINTS_BTN", 1);
    InitControl(m_lblSkill[0], "COMPUTER_USE_LBL", 1);
    m_sDescription[0] = g_pRules->m_pSkills[0].GetDescriptionText();
    InitControl(m_btnMinus[0], "COM_MINUS_BTN", 1);
    InitControl(m_btnPlus[0], "COM_PLUS_BTN", 1);
#endif
#ifdef _XBOX
    InitControl(m_lblSkill[1], "DEMOLITIONS_LBL", 1);
    InitControl(m_lblMinus[1], "DEMOLITIONS_MINUS_LBL", 1);
    InitControl(m_btnPoints[1], "DEMOLITIONS_POINTS_BTN", 1);
    InitControl(m_lblPlus[1], "DEMOLITIONS_PLUS_LBL", 1);
    m_sDescription[1] = g_pRules->m_pSkills[1].GetDescriptionText();
#else
    InitControl(m_btnPoints[1], "DEMOLITIONS_POINTS_BTN", 1);
    InitControl(m_lblSkill[1], "DEMOLITIONS_LBL", 1);
    m_sDescription[1] = g_pRules->m_pSkills[1].GetDescriptionText();
    InitControl(m_btnMinus[1], "DEM_MINUS_BTN", 1);
    InitControl(m_btnPlus[1], "DEM_PLUS_BTN", 1);
#endif
#ifdef _XBOX
    InitControl(m_lblSkill[2], "STEALTH_LBL", 1);
    InitControl(m_lblMinus[2], "STEALTH_MINUS_LBL", 1);
    InitControl(m_btnPoints[2], "STEALTH_POINTS_BTN", 1);
    InitControl(m_lblPlus[2], "STEALTH_PLUS_LBL", 1);
    m_sDescription[2] = g_pRules->m_pSkills[2].GetDescriptionText();
#else
    InitControl(m_btnPoints[2], "STEALTH_POINTS_BTN", 1);
    InitControl(m_lblSkill[2], "STEALTH_LBL", 1);
    m_sDescription[2] = g_pRules->m_pSkills[2].GetDescriptionText();
    InitControl(m_btnMinus[2], "STE_MINUS_BTN", 1);
    InitControl(m_btnPlus[2], "STE_PLUS_BTN", 1);
#endif
#ifdef _XBOX
    InitControl(m_lblSkill[4], "PERSUADE_LBL", 1);
    InitControl(m_lblMinus[4], "PERSUADE_MINUS_LBL", 1);
    InitControl(m_btnPoints[4], "PERSUADE_POINTS_BTN", 1);
    InitControl(m_lblPlus[4], "PERSUADE_PLUS_LBL", 1);
    m_sDescription[4] = g_pRules->m_pSkills[4].GetDescriptionText();
#else
    InitControl(m_btnPoints[4], "PERSUADE_POINTS_BTN", 1);
    InitControl(m_lblSkill[4], "PERSUADE_LBL", 1);
    m_sDescription[4] = g_pRules->m_pSkills[4].GetDescriptionText();
    InitControl(m_btnMinus[4], "PER_MINUS_BTN", 1);
    InitControl(m_btnPlus[4], "PER_PLUS_BTN", 1);
#endif
#ifdef _XBOX
    InitControl(m_lblSkill[5], "REPAIR_LBL", 1);
    InitControl(m_lblMinus[5], "REPAIR_MINUS_LBL", 1);
    InitControl(m_btnPoints[5], "REPAIR_POINTS_BTN", 1);
    InitControl(m_lblPlus[5], "REPAIR_PLUS_LBL", 1);
    m_sDescription[5] = g_pRules->m_pSkills[5].GetDescriptionText();
#else
    InitControl(m_btnPoints[5], "REPAIR_POINTS_BTN", 1);
    InitControl(m_lblSkill[5], "REPAIR_LBL", 1);
    m_sDescription[5] = g_pRules->m_pSkills[5].GetDescriptionText();
    InitControl(m_btnMinus[5], "REP_MINUS_BTN", 1);
    InitControl(m_btnPlus[5], "REP_PLUS_BTN", 1);
#endif
#ifdef _XBOX
    InitControl(m_lblSkill[6], "SECURITY_LBL", 1);
    InitControl(m_lblMinus[6], "SECURITY_MINUS_LBL", 1);
    InitControl(m_btnPoints[6], "SECURITY_POINTS_BTN", 1);
    InitControl(m_lblPlus[6], "SECURITY_PLUS_LBL", 1);
    m_sDescription[6] = g_pRules->m_pSkills[6].GetDescriptionText();
#else
    InitControl(m_btnPoints[6], "SECURITY_POINTS_BTN", 1);
    InitControl(m_lblSkill[6], "SECURITY_LBL", 1);
    m_sDescription[6] = g_pRules->m_pSkills[6].GetDescriptionText();
    InitControl(m_btnMinus[6], "SEC_MINUS_BTN", 1);
    InitControl(m_btnPlus[6], "SEC_PLUS_BTN", 1);
#endif
#ifdef _XBOX
    InitControl(m_lblSkill[3], "AWARENESS_LBL", 1);
    InitControl(m_lblMinus[3], "AWARENESS_MINUS_LBL", 1);
    InitControl(m_btnPoints[3], "AWARENESS_POINTS_BTN", 1);
    InitControl(m_lblPlus[3], "AWARENESS_PLUS_LBL", 1);
    m_sDescription[3] = g_pRules->m_pSkills[3].GetDescriptionText();
#else
    InitControl(m_btnPoints[3], "AWARENESS_POINTS_BTN", 1);
    InitControl(m_lblSkill[3], "AWARENESS_LBL", 1);
    m_sDescription[3] = g_pRules->m_pSkills[3].GetDescriptionText();
    InitControl(m_btnMinus[3], "AWA_MINUS_BTN", 1);
    InitControl(m_btnPlus[3], "AWA_PLUS_BTN", 1);
#endif
#ifdef _XBOX
    InitControl(m_lblSkill[7], "TREAT_INJURY_LBL", 1);
    InitControl(m_lblMinus[7], "TREAT_INJURY_MINUS_LBL", 1);
    InitControl(m_btnPoints[7], "TREAT_INJURY_POINTS_BTN", 1);
    InitControl(m_lblPlus[7], "TREAT_INJURY_PLUS_LBL", 1);
    m_sDescription[7] = g_pRules->m_pSkills[7].GetDescriptionText();
#else
    InitControl(m_btnPoints[7], "TREAT_INJURY_POINTS_BTN", 1);
    InitControl(m_lblSkill[7], "TREAT_INJURY_LBL", 1);
    m_sDescription[7] = g_pRules->m_pSkills[7].GetDescriptionText();
    InitControl(m_btnMinus[7], "TRE_MINUS_BTN", 1);
    InitControl(m_btnPlus[7], "TRE_PLUS_BTN", 1);
#endif
    StopLoadFromLayout();
#ifndef _XBOX
    m_lblDescription.Initialize(m_lbDescription.m_pProtoItem->m_cExtent, static_cast<CSWGuiLabel*>(m_lbDescription.m_pProtoItem), 1.0f);
#else
    m_lblDescription.Initialize(m_lbDescription.m_pProtoItem->m_cExtent, static_cast<CSWGuiLabel*>(m_lbDescription.m_pProtoItem));
#endif
    m_nSelected = 0;
    for (int i = 0; i < 8; i++)
    {
        m_btnPoints[i].AddEvent(CSWGuiControl::EVENT_ENTER, this, (CSWGuiHandler)&CSWGuiSkillsCharGen::OnEnterPointsButton);
        m_btnPoints[i].AddEvent(CSWGuiControl::EVENT_EXIT, this,
            (CSWGuiHandler)&CSWGuiSkillsCharGen::OnExitPointsButton);
        m_btnPoints[i].m_nIndex = i;
        if (!m_bLevelUp)
        {
            m_pCreature->GetStats()->SetSkillRankBase(i, 0);
            CSWCLevelUpStats* pStats = m_pCreature->GetStats();
            if ((unsigned short)i < g_pRules->m_nNumSkills)
            {
                pStats->SkillRankBase(i) = 0;
            }
        }
        m_aStartRank[i] = m_pCreature->GetStats()->GetSkillRank(i);
        {
            CExoString sRank(m_aStartRank[i]);
            m_btnPoints[i].m_cText.m_cParams.SetText(sRank);
        }
#ifdef _XBOX
        m_lblMinus[i].SetEnabledFlag(0);
        m_lblMinus[i].m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_BLUE;
#endif
        int nMaxRank = m_pCreature->GetStats()->GetLevel() + CSWCLevelUpStats::SKILL_RANK_LEVEL_BONUS;
        if (!IsClassSkill(i))
        {
            nMaxRank /= 2;
        }
#ifdef _XBOX
        if (m_pCreature->GetStats()->GetSkillRank(i) == nMaxRank)
        {
            m_lblPlus[i].SetEnabledFlag(0);
        }
        m_lblPlus[i].m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_BLUE;
#else
        if (m_bLevelUp)
        {
            m_btnMinus[i].SetEnabledFlag(m_pCreature->GetStats()->GetSkillRank(i) != m_aStartRank[i]);
        }
        else
        {
            m_btnMinus[i].SetEnabledFlag(m_pCreature->GetStats()->GetSkillRank(i) != 0);
        }
        m_btnPlus[i].SetEnabledFlag(m_pCreature->GetStats()->GetSkillRank(i) != nMaxRank);
        m_btnMinus[i].SetFocusable(0);
        m_btnPlus[i].SetFocusable(0);
        m_btnMinus[i].AddEvent(GUIEVENT_ACTIVATE, this,
            (CSWGuiHandler)&CSWGuiSkillsCharGen::OnMinusButtonPushed);
        m_btnPlus[i].AddEvent(GUIEVENT_ACTIVATE, this,
            (CSWGuiHandler)&CSWGuiSkillsCharGen::OnPlusButtonPushed);
#endif
        if (!m_bAllSkills)
        {
            CSWSkill* pSkill = &g_pRules->m_pSkills[(unsigned short)i];
            if (!pSkill->m_bNPCCanUse ||
                (m_bDroid && !pSkill->m_bDroidCanUse))
            {
                m_lblSkill[i].m_cText.m_cParams.SetColor(CGuiInGame::DISABLED_TEXT);
                m_btnPoints[i].m_cText.m_cParams.SetColor(CGuiInGame::DISABLED_TEXT);
#ifdef _XBOX
                m_lblPlus[i].SetEnabledFlag(0);
#else
                m_btnPlus[i].SetEnabledFlag(0);
#endif
            }
        }
    }
#ifndef _XBOX
    m_btnAccept.SetFocusable(0);
    m_btnAccept.AddEvent(GUIEVENT_ACTIVATE, this,
        (CSWGuiHandler)&CSWGuiPanel::OnAButtonPressed);
    m_btnRecommended.SetFocusable(0);
    m_btnRecommended.AddEvent(GUIEVENT_ACTIVATE, this,
        (CSWGuiHandler)&CSWGuiPanel::OnYButtonPressed);
    m_btnBack.SetFocusable(0);
    m_btnBack.AddEvent(GUIEVENT_ACTIVATE, this,
        (CSWGuiHandler)&CSWGuiPanel::OnBButtonPressed);
#endif
    m_pCreature->GetStats()->AddSkillPointsPerLevel();
    m_nStartPoints = m_nPointsRemaining = m_pCreature->GetStats()->GetSkillPoints();
    m_lblRemainingSelections.m_cText.m_cParams.SetText(m_nPointsRemaining);
    SetActiveControl(&m_btnPoints[m_nSelected], 0);
}
// PC 0x006F47E0  Xbox 0x0012CBB0
CSWGuiSkillsCharGen::~CSWGuiSkillsCharGen()
{
    m_lbDescription.ClearItems();
}
// PC 0x006F49A0  Xbox 0x0012C890
void CSWGuiSkillsCharGen::OnPanelAdded()
{
#ifndef _XBOX
    CSWGuiPanel::OnPanelAdded();
#endif
    g_pAppManager->m_pClientExoApp->ShowTutorialWindow(0x11, OBJECT_INVALID,
                                                       OBJECT_INVALID, 0);
}
// PC 0x006F49D0  Xbox 0x0012C8B0
void CSWGuiSkillsCharGen::HandleCancelButton()
{
    if (m_pParentPanel != NULL)
    {
        if (m_bLevelUp == 0)
        {
            int i;

            for (i = 0; i < 8; i++)
            {
                m_pCreature->m_pStats->SetSkillRankBase(i, 0);
                if ((unsigned short)i < g_pRules->m_nNumSkills)
                {
                    m_pCreature->m_pStats->SkillRankBase(i) = 0;
                }
            }
            m_pCreature->m_pStats->m_nSkillPoints = 0;
        }
        m_pManager->PopModalPanel();
        m_pParentPanel->SetVisible(1);
        m_bDeferredRemoval = 0;
        m_nExitState = EXITSTATE_DELETE;
        if (m_bLevelUp != 0)
        {
            static_cast<CSWGuiLevelUpPanel*>(m_pParentPanel)
                ->CancelledByChild();
        }
        else
        {
            static_cast<CSWGuiCustomPanel*>(m_pParentPanel)->CancelledByChild();
        }
    }
}
// PC 0x006F4A80  Xbox 0x0012C960
void CSWGuiSkillsCharGen::OnExitPointsButton(CSWGuiControl* pControl)
{
    CSWGuiTextParams* pLabelParams;
    CSWGuiTextParams* pButtonParams;

    pLabelParams = &m_lblSkill[pControl->m_nIndex].m_cText.m_cParams;
    pLabelParams->m_bHilightText = 0;
    pButtonParams = &m_btnPoints[pControl->m_nIndex].m_cText.m_cParams;
    pButtonParams->m_bHilightText = 0;
}
// PC 0x006F6370  Xbox 0x0012E630
void CSWGuiSkillsCharGen::OnMinusButton()
{
    CSWCLevelUpStats* pStats = m_pCreature->m_pStats;

    if (pStats->GetSkillRank((unsigned short)m_nSelected) ==
        m_aStartRank[m_nSelected])
    {
        return;
    }
    pStats->DecreaseSkillRank((unsigned short)m_nSelected,
                              m_aStartRank[m_nSelected]);
    CSWCLevelUpStats* pCurrentStats = m_pCreature->m_pStats;
    m_nPointsRemaining = pCurrentStats->m_nSkillPoints;
    pStats = m_pCreature->m_pStats;
    CExoString sRank(pStats->GetSkillRank(
        (unsigned short)m_nSelected));
    m_btnPoints[m_nSelected].m_cText.m_cParams.SetText(sRank);
    CExoString sRemaining(m_nPointsRemaining);
    m_lblRemainingSelections.m_cText.m_cParams.SetText(sRemaining);
    OnEnterPointsButton(&m_btnPoints[m_nSelected]);
#ifdef _XBOX
    CSWGuiControl* pPlusButton = &m_lblPlus[m_nSelected];
#else
    CSWGuiControl* pPlusButton = &m_btnPlus[m_nSelected];
#endif
    pPlusButton->m_bEnabled = 1;
    pStats = m_pCreature->m_pStats;
    if (pStats->GetSkillRank((unsigned short)m_nSelected) ==
        m_aStartRank[m_nSelected])
    {
#ifdef _XBOX
        CSWGuiControl* pMinusButton = &m_lblMinus[m_nSelected];
        pMinusButton->m_bEnabled = 0;
    }
    HilightArrow(&m_lblMinus[m_nSelected]);
#else
        CSWGuiControl* pMinusButton = &m_btnMinus[m_nSelected];
        pMinusButton->m_bEnabled = 0;
        m_btnMinus[m_nSelected].SetActive(0);
    }
#endif
}
// PC 0x006F6570  Xbox 0x0012E830
void CSWGuiSkillsCharGen::OnPlusButton()
{
    int bEnoughPoints = 0;
    int bBelowMaxRank = 0;

    unsigned long nWarning = 0;
    switch (m_nSelected)
    {
    case CSWRules::SKILL_PERSUADE:
        if (!m_bAllSkills)
            nWarning = STRREF_PERSUADE_MAIN_CHARACTER_ONLY;
        break;
    case CSWRules::SKILL_TREAT_INJURY:
    case CSWRules::SKILL_STEALTH:
        if (m_bDroid)
            nWarning = STRREF_DROID_SKILL_UNAVAILABLE;
        break;
    }
    if (nWarning != 0)
    {
        CSWGuiMessageBox* pMessageBox =
            g_pAppManager->GetClientExoApp()->GetInGameGui()->m_pMessageBox;
        pMessageBox->SetAllowCancel(0);
        pMessageBox->SetCallback(NULL, NULL, NULL);
        pMessageBox->SetMessage(nWarning);
        m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
        return;
    }

    int nMaxRank = m_pCreature->GetStats()->GetLevel() + 3;
    int nSkill = m_nSelected;
    int bClassSkill = IsClassSkill(nSkill);
    if (!bClassSkill)
    {
        nMaxRank /= 2;
    }
    int nSkillPoints = GetCreature()->GetStats()->m_nSkillPoints;
    nSkill = m_nSelected;
    if (nSkillPoints >= GetCreature()->GetStats()->GetSkillPointCostForSkillRank(
                                       nSkill, 1))
    {
        bEnoughPoints = 1;
    }
    nSkill = m_nSelected;
    CSWCCreature* pCreature = m_pCreature;
    if (pCreature->GetStats()->GetSkillRank(nSkill) <
        nMaxRank)
    {
        bBelowMaxRank = 1;
    }

    if (bBelowMaxRank != 1 || bEnoughPoints != 1)
    {
        CSWGuiMessageBox* pMessageBox =
            g_pAppManager->GetClientExoApp()->GetInGameGui()->m_pMessageBox;
        CExoString sMessage;
        if (!bEnoughPoints)
        {
            sMessage = g_pTlkTable->GetSimpleString(STRREF_NOT_ENOUGH_SKILL_POINTS);
        }
        else if (!bBelowMaxRank)
        {
            if (bClassSkill)
            {
                sMessage = g_pTlkTable->GetSimpleString(STRREF_CLASS_SKILL_MAX_RANK);
            }
            else
            {
                sMessage = g_pTlkTable->GetSimpleString(
                    STRREF_CROSS_CLASS_SKILL_MAX_RANK);
            }
            g_pTlkTable->SetCustomToken(0, nMaxRank);
            g_pTlkTable->ParseStr(sMessage);
        }
        pMessageBox->SetAllowCancel(0);
        pMessageBox->SetCallback(NULL, NULL, NULL);
        pMessageBox->SetMessage(sMessage);
        m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
        return;
    }

    m_pCreature->GetStats()->IncreaseSkillRank(nSkill);
    m_nPointsRemaining = m_pCreature->GetStats()->m_nSkillPoints;
    CExoString sRank(m_pCreature->GetStats()->GetSkillRank(
        (unsigned short)m_nSelected));
    m_btnPoints[m_nSelected].m_cText.m_cParams.SetText(sRank);
    CExoString sRemaining(m_nPointsRemaining);
    m_lblRemainingSelections.m_cText.m_cParams.SetText(sRemaining);
    OnEnterPointsButton(&m_btnPoints[m_nSelected]);
#ifdef _XBOX
    m_lblMinus[m_nSelected].SetEnabledFlag(1);
    if (m_pCreature->GetStats()->GetSkillRank((unsigned short)m_nSelected) ==
        nMaxRank)
    {
        m_lblPlus[m_nSelected].SetEnabledFlag(0);
    }
    HilightArrow(&m_lblPlus[m_nSelected]);
#else
    m_btnMinus[m_nSelected].SetEnabledFlag(1);
    if (m_pCreature->GetStats()->GetSkillRank((unsigned short)m_nSelected) ==
        nMaxRank)
    {
        m_btnPlus[m_nSelected].SetEnabledFlag(0);
        m_btnPlus[m_nSelected].SetActive(0);
    }
#endif
}
// PC 0x006F6980  Xbox 0x0012EC40
void CSWGuiSkillsCharGen::OnAcceptButton()
{
    if (m_nPointsRemaining > 0)
    {
        CSWGuiMessageBox* pMessageBox =
            g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
        pMessageBox->SetAllowCancel(1);
        pMessageBox->SetMessage(41815);
        pMessageBox->SetCallback(
            this, (CSWGuiHandler)&CSWGuiSkillsCharGen::ConfirmAccept, NULL);
        m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
        return;
    }
    CompletePanel();
}
// PC 0x006F6A10  Xbox 0x0012ECD0
void CSWGuiSkillsCharGen::HandleInputEvent(int nEvent, int nState)
{
    if (nState != 0)
    {
        switch (nEvent)
        {
        case 40:
        case 46:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            HandleCancelButton();
            break;
        case 47:
        case 63:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
            OnMinusButton();
            break;
        case 48:
        case 64:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
            OnPlusButton();
            break;
        case 39:
        case 45:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            OnAcceptButton();
            break;
        case 42:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            OnRecommendButton();
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
// PC 0x006F4BF0  Xbox 0x0012CDD0
void CSWGuiSkillsCharGen::OnEnterPointsButton(CSWGuiControl* pControl)
{
    int i;

    for (i = 0; i < 8; i++)
    {
        int bDisabled = 0;
        if (!m_bAllSkills)
        {
            CSWSkill* pSkill = &g_pRules->m_pSkills[(unsigned short)i];
            if (!pSkill->m_bNPCCanUse)
            {
                bDisabled = 1;
            }
            else if (m_bDroid &&
                     !pSkill->m_bDroidCanUse)
            {
                bDisabled = 1;
            }
        }

        if (pControl == &m_btnPoints[i])
        {
            m_nSelected = i;
            if (bDisabled)
            {
                m_btnPoints[i].m_cText.m_cParams.SetColor(CGuiInGame::DISABLED_TEXT);
                m_lblSkill[i].m_cText.m_cParams.SetColor(CGuiInGame::DISABLED_TEXT);
            }
            else
            {
                m_btnPoints[i].m_cText.m_cParams.SetColor(CGuiInGame::COLOR_YELLOW);
                m_lblSkill[i].m_cText.m_cParams.SetColor(CGuiInGame::COLOR_YELLOW);
            }
        }
        else
        {
            if (bDisabled)
            {
                m_btnPoints[i].m_cText.m_cParams.SetColor(CGuiInGame::DISABLED_TEXT);
                m_lblSkill[i].m_cText.m_cParams.SetColor(CGuiInGame::DISABLED_TEXT);
            }
            else
            {
                m_btnPoints[i].m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
                m_lblSkill[i].m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
            }
        }
    }

    if (IsClassSkill((unsigned short)m_nSelected))
    {
        m_lblClassSkill.m_cText.m_cParams.SetStrRef(38152);
    }
    else
    {
        m_lblClassSkill.m_cText.m_cParams.SetStrRef(38153);
    }
    CSWCLevelUpStats* pStats = m_pCreature->m_pStats;
    CExoString sCost(pStats->GetSkillPointCostForSkillRank(
        (unsigned short)m_nSelected, 1));
    m_lblCostPoints.m_cText.m_cParams.SetText(sCost);
    SetDescription(m_sDescription[m_nSelected]);
    CSWGuiTextParams* pLabelParams = &m_lblSkill[m_nSelected].m_cText.m_cParams;
    pLabelParams->m_bHilightText = 1;
    CSWGuiTextParams* pButtonParams = &m_btnPoints[m_nSelected].m_cText.m_cParams;
    pButtonParams->m_bHilightText = 1;
}
// PC 0x006F4DC0  Xbox 0x0012CFA0
void CSWGuiSkillsCharGen::ConfirmAccept(CSWGuiControl* pControl)
{
    if (g_pAppManager->m_pClientExoApp->GetInGameGui()
            ->m_pMessageBox->m_bAccepted != 0)
    {
        CompletePanel();
    }
}
// PC 0x006F4E20  Xbox 0x0012D000
void CSWGuiSkillsCharGen::OnRecommendButton()
{
    unsigned char nIndex = m_pCreature->GetStats()->GetNumClasses() - 1;
    unsigned char nClass = m_pCreature->GetStats()->GetClass(nIndex);
    unsigned char i;

    if (m_bLevelUp == 0)
    {
        m_pCreature->GetStats()->m_nSkillPoints = 0;
        m_pCreature->GetStats()->AddSkillPointsPerLevel();
        m_pCreature->GetStats()->ClearSkills();
        m_pCreature->GetStats()->UpdateAbilityModifiedSkillRanks();
    }
    else
    {
        CSWCLevelUpStats* pStats = m_pCreature->GetStats();
        unsigned short nPoints = m_nStartPoints;
        pStats->m_nSkillPoints = nPoints;
    }
    for (i = 0; i < g_pRules->m_nNumSkills; i++)
    {
        m_pCreature->GetStats()->SetSkillRankBase(i, m_aStartRank[i]);
    }
    m_pCreature->GetStats()->AddDefaultPackageSkills(nClass, m_bAllSkills);

    for (i = 0; i < g_pRules->m_nNumSkills; i++)
    {
        int nMaxRank = m_pCreature->GetStats()->GetLevel() + 3;
        if (!IsClassSkill(i))
        {
            nMaxRank /= 2;
        }
        {
            CExoString sRank(m_pCreature->GetStats()->GetSkillRank(i));
#ifdef _XBOX
            m_btnPoints[i].m_cText.m_cParams.SetText(sRank);
#else
            CSWGuiButton* pButton = &m_btnPoints[i];
            CSWGuiTextParams* pParams = &pButton->m_cText.m_cParams;
            pParams->SetText(sRank);
#endif
        }
#ifdef _XBOX
        if (m_bLevelUp != 0)
        {
            m_lblMinus[i].SetEnabledFlag(
                m_pCreature->GetStats()->GetSkillRank(i) != m_aStartRank[i]);
        }
        else
        {
            m_lblMinus[i].SetEnabledFlag(
                m_pCreature->GetStats()->GetSkillRank(i) != 0);
        }
        m_lblPlus[i].SetEnabledFlag(
            m_pCreature->GetStats()->GetSkillRank(i) != nMaxRank);
#else
        if (m_bLevelUp != 0)
        {
            m_btnMinus[i].SetEnabledFlag(
                m_pCreature->GetStats()->GetSkillRank(i) != m_aStartRank[i]);
        }
        else
        {
            m_btnMinus[i].SetEnabledFlag(
                m_pCreature->GetStats()->GetSkillRank(i) != 0);
        }
        m_btnPlus[i].SetEnabledFlag(
            m_pCreature->GetStats()->GetSkillRank(i) != nMaxRank);
#endif
        if (!m_bAllSkills)
        {
            CSWSkill* pSkill = &g_pRules->m_pSkills[i];
            if (!pSkill->m_bNPCCanUse ||
                (m_bDroid &&
                 !pSkill->m_bDroidCanUse))
            {
                m_lblSkill[i].m_cText.m_cParams.SetColor(
                    CGuiInGame::DISABLED_TEXT);
                m_btnPoints[i].m_cText.m_cParams.SetColor(
                    CGuiInGame::DISABLED_TEXT);
#ifdef _XBOX
                m_lblPlus[i].SetEnabledFlag(0);
#else
                m_btnPlus[i].SetEnabledFlag(0);
#endif
            }
        }
    }
    m_nPointsRemaining = m_pCreature->GetStats()->m_nSkillPoints;
    m_lblRemainingSelections.m_cText.m_cParams.SetText(m_nPointsRemaining);
    m_pCreature->GetStats()->UpdateAbilityModifiedSkillRanks();
}

#ifdef _XBOX
// PC 0x0041A290  Xbox 0x0004CEC0
inline int CSWGuiListBox::IsOversized()
{
    return m_nContentHeight + m_nPadding > m_cItemExtent.m_nHeight &&
           m_lItems.num > 0 && !m_bHideScrollIcon;
}
#endif
// PC 0x006F4AB0  Xbox 0x0012C990
void CSWGuiSkillsCharGen::SetDescription(const CExoString& sDescription)
{

    m_lblDescription.m_cText.m_cParams.SetText(sDescription);
    m_lbDescription.ClearItems();
    CSWGuiExtent cExtent = m_lblDescription.m_cExtent;
    if (sDescription.GetLength() > 0)
    {
        cExtent.m_nHeight = m_lblDescription.m_cText.GetIdealHeight();
    }
    else
    {
        cExtent.m_nHeight = m_lblDescription.m_cText.GetFontHeight();
    }
    m_lblDescription.SetExtent(cExtent);
    CSWGuiControl* pControl = &m_lblDescription;
    m_lbDescription.AddControls(&pControl, 1, 0, 0, 0);
    m_lbDescription.SetSelectedControl(&m_lblDescription, 0);
#ifdef _XBOX
    m_lblRanalog.SetEnabledFlag(m_lbDescription.IsOversized());
#endif
}
// PC 0x006F4B60  Xbox 0x0012CA90
int CSWGuiSkillsCharGen::IsClassSkill(unsigned short nSkill)
{
    int nClass;

    for (int i = 0; i < m_pCreature->m_pStats->m_nClassCount; i++)
    {
        nClass = m_pCreature->m_pStats->GetClass(i);
        if (g_pRules->m_pClasses[(unsigned char)nClass]
                .IsSkillClassSkill(nSkill) != 0)
        {
            return 1;
        }
    }
    return 0;
}

void CSWGuiSkillsCharGen::CompletePanel()
{
    m_pManager->PopModalPanel();
    m_pParentPanel->SetVisible(1);
    m_bDeferredRemoval = 0;
    m_nExitState = EXITSTATE_DELETE;
    if (m_bLevelUp != 0)
    {
        static_cast<CSWGuiLevelUpPanel*>(m_pParentPanel)->SkillSelectionCompleted();
        return;
    }
    static_cast<CSWGuiCustomPanel*>(m_pParentPanel)->SkillCreationCompleted();
}

#ifdef _XBOX
// Xbox 0x0012CB00
void CSWGuiSkillsCharGen::Draw(float fElapsed)
{
    int nCount = m_lArrowHilights.num;

    for (int i = nCount - 1; i >= 0; i--)
    {
        m_lArrowHilights.element[i].m_fTimeLeft -= fElapsed;
        if (m_lArrowHilights.element[i].m_fTimeLeft <= 0.0f)
        {
            m_lArrowHilights.element[i].m_pLabel->m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_BLUE;
            m_lArrowHilights.DelIndex(i);
        }
    }
    CSWGuiPanel::Draw(fElapsed);
}
// Xbox 0x0012D3B0
void CSWGuiSkillsCharGen::HilightArrow(CSWGuiLabel* pLabel)
{
    int nCount = m_lArrowHilights.num;

    for (int i = 0; i < nCount; i++)
    {
        if (m_lArrowHilights.element[i].m_pLabel == pLabel)
        {
            m_lArrowHilights.element[i].m_fTimeLeft = 0.25f;
            return;
        }
    }
    m_lArrowHilights.SetSize(nCount + 1);
    m_lArrowHilights.element[nCount].m_pLabel = pLabel;
    m_lArrowHilights.element[nCount].m_fTimeLeft = 0.25f;
    pLabel->m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_YELLOW;
}
#endif
