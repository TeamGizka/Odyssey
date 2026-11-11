#include "exobase.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "nwclass.h"
#include "nwccreaturestats.h"
#include "swguiabilitieschargen.h"
#include "swguiingame.h"
#include "nwccreature.h"
// PC 0x006F6B60  Xbox 0x0012EE20
int CSWGuiAbilitiesCharGen::DetermineCostToDecreaseScore(int nScore)
{
    if (m_bLevelUp != 0)
    {
        return -1;
    }
    if (nScore <= 14 && nScore > 8)
    {
        return -1;
    }
    if (nScore >= 15 && nScore <= 16)
    {
        return -2;
    }
    if (nScore < 17)
    {
        return 0;
    }
    return -3;
}
// PC 0x006F6BB0  Xbox 0x0012EE70
int CSWGuiAbilitiesCharGen::DetermineCostToIncreaseScore(int nScore)
{
    if (m_bLevelUp != 0)
    {
        return 1;
    }
    if (nScore > 15)
    {
        return 3;
    }
    return (nScore > 13) + 1;
}
// PC 0x006F7600  Xbox 0x0012FAD0
CSWGuiAbilitiesCharGen::CSWGuiAbilitiesCharGen(CSWGuiManager* pManager,
                                               CSWCCreature* pCreature,
                                               int bLevelUp)
    : CSWGuiPanel(pManager)
{
    STR_RES strRes;

    m_bLevelUp = bLevelUp;
    m_pCreature = pCreature;
    StartLoadFromLayout("ABCHRGEN");

    InitControl(m_lblTitle, "MAIN_TITLE_LBL", 1);
    if (bLevelUp != 0)
    {
        m_lblTitle.m_cText.m_cParams.SetStrRef(0x42F);
    }
    InitControl(m_lblSubTitle, "SUB_TITLE_LBL", 1);
    InitControl(m_lblRemainingBox, "REMAINING_BOX_LBL", 1);
    InitControl(m_lblRemainingSelections, "REMAINING_SELECTIONS_LBL", 1);
    InitControl(m_lblSelectionsRemaining, "SELECTIONS_REMAINING_LBL", 1);
    InitControl(m_lblDescriptionHeader, "DESC_LBL", 1);
    InitControl(m_lblCost, "COST_LBL", 1);
    InitControl(m_lblCostPoints, "COST_POINTS_LBL", 1);
    InitControl(m_lbDescription, "LB_DESC", 1);
    m_lbDescription.m_nSelectSound = CSWGuiManager::GUISOUND_SELECTION;
#ifdef _XBOX
    InitControl(m_lblA, "LBL_A", 1);
    InitControl(m_lblB, "LBL_B", 1);
    InitControl(m_lblY, "LBL_Y", 1);
    InitControl(m_lblAccept, "LBL_ACCEPT", 1);
    InitControl(m_lblRecommended, "LBL_RECOMMENDED", 1);
    InitControl(m_lblBack, "LBL_BACK", 1);
#endif
    InitControl(m_lblAbilityMod, "LBL_ABILITY_MOD", 1);
    InitControl(m_lblModifier, "LBL_MODIFIER", 1);
#ifdef _XBOX
    InitControl(m_lblRanalog, "LBL_RANALOG", 1);
#else
    InitControl(m_btnAccept, "BTN_ACCEPT", 1);
    InitControl(m_btnRecommended, "BTN_RECOMMENDED", 1);
    InitControl(m_btnBack, "BTN_BACK", 1);
#endif

#ifdef _XBOX
    InitControl(m_lblScore[0], "STR_LBL", 1);
    InitControl(m_lblMinus[0], "STR_MINUS_LBL", 1);
    InitControl(m_btnPoints[0], "STR_POINTS_BTN", 1);
    InitControl(m_lblPlus[0], "STR_PLUS_LBL", 1);
#else
    InitControl(m_btnPoints[0], "STR_POINTS_BTN", 1);
    InitControl(m_lblScore[0], "STR_LBL", 1);
    InitControl(m_btnMinus[0], "STR_MINUS_BTN", 1);
    InitControl(m_btnPlus[0], "STR_PLUS_BTN", 1);
#endif
    m_aDescriptionStrRef[0] = 222;
#ifdef _XBOX
    InitControl(m_lblScore[1], "DEX_LBL", 1);
    InitControl(m_lblMinus[1], "DEX_MINUS_LBL", 1);
    InitControl(m_btnPoints[1], "DEX_POINTS_BTN", 1);
    InitControl(m_lblPlus[1], "DEX_PLUS_LBL", 1);
#else
    InitControl(m_btnPoints[1], "DEX_POINTS_BTN", 1);
    InitControl(m_lblScore[1], "DEX_LBL", 1);
    InitControl(m_btnMinus[1], "DEX_MINUS_BTN", 1);
    InitControl(m_btnPlus[1], "DEX_PLUS_BTN", 1);
#endif
    m_aDescriptionStrRef[1] = 223;
#ifdef _XBOX
    InitControl(m_lblScore[2], "CON_LBL", 1);
    InitControl(m_lblMinus[2], "CON_MINUS_LBL", 1);
    InitControl(m_btnPoints[2], "CON_POINTS_BTN", 1);
    InitControl(m_lblPlus[2], "CON_PLUS_LBL", 1);
#else
    InitControl(m_btnPoints[2], "CON_POINTS_BTN", 1);
    InitControl(m_lblScore[2], "CON_LBL", 1);
    InitControl(m_btnMinus[2], "CON_MINUS_BTN", 1);
    InitControl(m_btnPlus[2], "CON_PLUS_BTN", 1);
#endif
    m_aDescriptionStrRef[2] = 224;
#ifdef _XBOX
    InitControl(m_lblScore[3], "WIS_LBL", 1);
    InitControl(m_lblMinus[3], "WIS_MINUS_LBL", 1);
    InitControl(m_btnPoints[3], "WIS_POINTS_BTN", 1);
    InitControl(m_lblPlus[3], "WIS_PLUS_LBL", 1);
#else
    InitControl(m_btnPoints[3], "WIS_POINTS_BTN", 1);
    InitControl(m_lblScore[3], "WIS_LBL", 1);
    InitControl(m_btnMinus[3], "WIS_MINUS_BTN", 1);
    InitControl(m_btnPlus[3], "WIS_PLUS_BTN", 1);
#endif
    m_aDescriptionStrRef[3] = 225;
#ifdef _XBOX
    InitControl(m_lblScore[4], "INT_LBL", 1);
    InitControl(m_lblMinus[4], "INT_MINUS_LBL", 1);
    InitControl(m_btnPoints[4], "INT_POINTS_BTN", 1);
    InitControl(m_lblPlus[4], "INT_PLUS_LBL", 1);
#else
    InitControl(m_btnPoints[4], "INT_POINTS_BTN", 1);
    InitControl(m_lblScore[4], "INT_LBL", 1);
    InitControl(m_btnMinus[4], "INT_MINUS_BTN", 1);
    InitControl(m_btnPlus[4], "INT_PLUS_BTN", 1);
#endif
    m_aDescriptionStrRef[4] = 226;
#ifdef _XBOX
    InitControl(m_lblScore[5], "CHA_LBL", 1);
    InitControl(m_lblMinus[5], "CHA_MINUS_LBL", 1);
    InitControl(m_btnPoints[5], "CHA_POINTS_BTN", 1);
    InitControl(m_lblPlus[5], "CHA_PLUS_LBL", 1);
#else
    InitControl(m_btnPoints[5], "CHA_POINTS_BTN", 1);
    InitControl(m_lblScore[5], "CHA_LBL", 1);
    InitControl(m_btnMinus[5], "CHA_MINUS_BTN", 1);
    InitControl(m_btnPlus[5], "CHA_PLUS_BTN", 1);
#endif
    m_aDescriptionStrRef[5] = 227;
    StopLoadFromLayout();

#ifdef _XBOX
    unsigned char nStrength = m_pCreature->GetStats()->m_aAbilities[0];
    m_aScore[0] = nStrength;
    m_aScore[1] = m_pCreature->GetStats()->m_aAbilities[1];
    m_aScore[2] = m_pCreature->GetStats()->m_aAbilities[2];
    m_aScore[3] = m_pCreature->GetStats()->m_aAbilities[4];
    m_aScore[4] = m_pCreature->GetStats()->m_aAbilities[3];
    m_aScore[5] = m_pCreature->GetStats()->m_aAbilities[5];
    if (m_bLevelUp != 0)
    {
        m_aMinimumScore[0] = m_pCreature->GetStats()->m_aAbilities[0];
        m_aMinimumScore[1] = m_pCreature->GetStats()->m_aAbilities[1];
        m_aMinimumScore[2] = m_pCreature->GetStats()->m_aAbilities[2];
        m_aMinimumScore[3] = m_pCreature->GetStats()->m_aAbilities[4];
        m_aMinimumScore[4] = m_pCreature->GetStats()->m_aAbilities[3];
        m_aMinimumScore[5] = m_pCreature->GetStats()->m_aAbilities[5];
    }
#else
    if (m_bLevelUp != 0)
    {
        CSWSCreature* pServerCreature = m_pCreature->GetServerCreature();
        if (pServerCreature != NULL)
        {
            CSWSCreatureStats* pStats = pServerCreature->m_pStats;
            unsigned char nStrength = pStats->m_nStrengthBase;
            m_aScore[0] = nStrength;
            CSWSCreatureStats* pDexterityStats = pServerCreature->m_pStats;
            m_aScore[1] = pDexterityStats->m_nDexterityBase;
            CSWSCreatureStats* pConstitutionStats = pServerCreature->m_pStats;
            m_aScore[2] = pConstitutionStats->m_nConstitutionBase;
            CSWSCreatureStats* pWisdomStats = pServerCreature->m_pStats;
            m_aScore[3] = pWisdomStats->m_nWisdomBase;
            CSWSCreatureStats* pIntelligenceStats = pServerCreature->m_pStats;
            m_aScore[4] = pIntelligenceStats->m_nIntelligenceBase;
            CSWSCreatureStats* pCharismaStats = pServerCreature->m_pStats;
            m_aScore[5] = pCharismaStats->m_nCharismaBase;
        }
        else
        {
            unsigned char nStrength = m_pCreature->GetStats()->m_aAbilities[0];
            m_aScore[0] = nStrength;
            m_aScore[1] = m_pCreature->GetStats()->m_aAbilities[1];
            m_aScore[2] = m_pCreature->GetStats()->m_aAbilities[2];
            m_aScore[3] = m_pCreature->GetStats()->m_aAbilities[4];
            m_aScore[4] = m_pCreature->GetStats()->m_aAbilities[3];
            m_aScore[5] = m_pCreature->GetStats()->m_aAbilities[5];
        }
        m_aMinimumScore[0] = m_aScore[0];
        m_aMinimumScore[1] = m_aScore[1];
        m_aMinimumScore[2] = m_aScore[2];
        m_aMinimumScore[3] = m_aScore[3];
        m_aMinimumScore[4] = m_aScore[4];
        m_aMinimumScore[5] = m_aScore[5];
    }
    else
    {
        unsigned char nStrength = m_pCreature->GetStats()->m_aAbilities[0];
            m_aScore[0] = nStrength;
        m_aScore[1] = m_pCreature->GetStats()->m_aAbilities[1];
        m_aScore[2] = m_pCreature->GetStats()->m_aAbilities[2];
        m_aScore[3] = m_pCreature->GetStats()->m_aAbilities[4];
        m_aScore[4] = m_pCreature->GetStats()->m_aAbilities[3];
        m_aScore[5] = m_pCreature->GetStats()->m_aAbilities[5];
    }
#endif

    for (int i = 0; i < 6; i++)
    {
        m_btnPoints[i].AddEvent(
            0, this,
            (CSWGuiHandler)&CSWGuiAbilitiesCharGen::OnEnterPointsButton);
        m_btnPoints[i].AddEvent(
            1, this,
            (CSWGuiHandler)&CSWGuiAbilitiesCharGen::OnExitPointsButton);
        {
            CExoString sScore(m_aScore[i]);
            m_btnPoints[i].m_cText.m_cParams.SetText(sScore);
        }
        m_btnPoints[i].m_nIndex = i;
#ifdef _XBOX
        m_lblMinus[i].SetEnabledFlag(0);
        m_lblMinus[i].m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_BLUE;
        m_lblPlus[i].m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_BLUE;
#else
        if (m_bLevelUp != 0)
        {
            m_btnMinus[i].SetEnabledFlag(0);
        }
        else
        {
            m_btnMinus[i].SetEnabledFlag(m_aScore[i] != 8);
            m_btnPlus[i].SetEnabledFlag(m_aScore[i] != 18);
        }
        m_btnMinus[i].SetFocusable(0);
        m_btnPlus[i].SetFocusable(0);
        m_btnMinus[i].AddEvent(
            39, this,
            (CSWGuiHandler)&CSWGuiAbilitiesCharGen::OnMinusButtonPushed);
        m_btnPlus[i].AddEvent(
            39, this,
            (CSWGuiHandler)&CSWGuiAbilitiesCharGen::OnPlusButtonPushed);
#endif
    }
#ifndef _XBOX
    m_btnAccept.SetFocusable(0);
    m_btnAccept.AddEvent(39, this,
                         (CSWGuiHandler)&CSWGuiPanel::OnAButtonPressed);
    m_btnRecommended.SetFocusable(0);
    m_btnRecommended.AddEvent(39, this,
                              (CSWGuiHandler)&CSWGuiPanel::OnYButtonPressed);
    m_btnBack.SetFocusable(0);
    m_btnBack.AddEvent(GUIEVENT_ACTIVATE, this, (CSWGuiHandler)&CSWGuiPanel::OnBButtonPressed);
#endif

    if (m_bLevelUp != 0)
    {
        if (m_pCreature->m_pStats->GetLevel() % 4 == 0)
        {
            m_nPointsRemaining = 1;
        }
        else
        {
            m_nPointsRemaining = 0;
        }
        m_lblSelectionsRemaining.m_cText.m_cParams.SetText("Points Remaining");
    }
    else
    {
        CSWCCreatureStats* pStats = m_pCreature->m_pStats;
        m_nPointsRemaining = pStats->m_nCreationStage;
    }
    m_lblRemainingSelections.m_cText.m_cParams.SetText(m_nPointsRemaining);
    m_lblDescription.Initialize(m_lbDescription.m_pProtoItem->m_cExtent,
                                (CSWGuiLabel*)m_lbDescription.m_pProtoItem, 1.0f);
    SetActiveControl(&m_btnPoints[0], 0);
    m_nSelected = 0;
}
// PC 0x006F6BF0  Xbox 0x0012F2C0
CSWGuiAbilitiesCharGen::~CSWGuiAbilitiesCharGen()
{
    m_lbDescription.ClearItems();
}
// PC 0x006F6DB0  Xbox 0x0012EEB0
void CSWGuiAbilitiesCharGen::OnPanelAdded()
{
#ifndef _XBOX
    CSWGuiPanel::OnPanelAdded();
#endif
    g_pAppManager->m_pClientExoApp->ShowTutorialWindow(0x10, OBJECT_INVALID,
                                                       OBJECT_INVALID, 0);
}
// PC 0x006F6DE0  Xbox 0x0012EED0
void CSWGuiAbilitiesCharGen::HandleCancelButton()
{
    if (m_pParentPanel != NULL)
    {
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
            m_pCreature->m_pStats->m_nCreationStage = 30;
            static_cast<CSWGuiCustomPanel*>(m_pParentPanel)->CancelledByChild();
        }
    }
}
// PC 0x006F6E40  Xbox 0x0012EF30
void CSWGuiAbilitiesCharGen::OnExitPointsButton(CSWGuiControl* pControl)
{
    CSWGuiTextParams* pLabelParams;
    CSWGuiTextParams* pButtonParams;

    pLabelParams = &m_lblScore[pControl->m_nIndex].m_cText.m_cParams;
    pLabelParams->m_bHilightText = 0;
    pButtonParams = &m_btnPoints[pControl->m_nIndex].m_cText.m_cParams;
    pButtonParams->m_bHilightText = 0;
}
// PC 0x006F8480  Xbox 0x00130920
void CSWGuiAbilitiesCharGen::OnMinusButton()
{
    int nScore = m_aScore[m_nSelected];

    if (m_bLevelUp != 0 && nScore == m_aMinimumScore[m_nSelected])
    {
        return;
    }
    if (m_bLevelUp == 0 && nScore == 8)
    {
        CSWGuiMessageBox* pMessageBox =
            g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
        pMessageBox->SetAllowCancel(0);
        pMessageBox->SetMessage(42180);
        pMessageBox->SetCallback(NULL, NULL, NULL);
        m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
        return;
    }
    if (nScore > 8)
    {
        m_nPointsRemaining -= DetermineCostToDecreaseScore(nScore);
        m_aScore[m_nSelected]--;
    }

    CExoString sScore(m_aScore[m_nSelected]);
    m_btnPoints[m_nSelected].m_cText.m_cParams.SetText(sScore);
    CExoString sRemaining(m_nPointsRemaining);
    m_lblRemainingSelections.m_cText.m_cParams.SetText(sRemaining);
    OnEnterPointsButton(&m_btnPoints[m_nSelected]);
    if ((m_bLevelUp != 0 &&
         m_aScore[m_nSelected] == m_aMinimumScore[m_nSelected]) ||
        (m_bLevelUp == 0 && m_aScore[m_nSelected] == 8))
    {
#ifdef _XBOX
        m_lblMinus[m_nSelected].SetEnabledFlag(0);
    }
    m_lblPlus[m_nSelected].SetEnabledFlag(1);
    HilightArrow(&m_lblMinus[m_nSelected]);
#else
        m_btnMinus[m_nSelected].SetEnabledFlag(0);
        m_btnMinus[m_nSelected].SetActive(0);
    }
    m_btnPlus[m_nSelected].SetEnabledFlag(1);
#endif
}
// PC 0x006F8670  Xbox 0x00130B10
void CSWGuiAbilitiesCharGen::OnPlusButton()
{
    int nRemaining = m_nPointsRemaining;

    if (nRemaining > 0)
    {
        int bLevelUp = m_bLevelUp;

        if (bLevelUp != 0 || m_aScore[m_nSelected] != 18)
        {
            int nScore = m_aScore[m_nSelected];
            int nCost = DetermineCostToIncreaseScore(nScore);
            if (nRemaining - nCost >= 0 && (bLevelUp != 0 || nScore < 18))
            {
                m_nPointsRemaining =
                    nRemaining - DetermineCostToIncreaseScore(nScore);
                m_aScore[m_nSelected]++;
            }

            CExoString sScore(m_aScore[m_nSelected]);
            m_btnPoints[m_nSelected].m_cText.m_cParams.SetText(sScore);
            CExoString sRemaining(m_nPointsRemaining);
            m_lblRemainingSelections.m_cText.m_cParams.SetText(sRemaining);
            OnEnterPointsButton(&m_btnPoints[m_nSelected]);
            if (m_bLevelUp == 0)
            {
                if (m_aScore[m_nSelected] == 18)
                {
#ifdef _XBOX
                    m_lblPlus[m_nSelected].SetEnabledFlag(0);
                }
            }
            m_lblMinus[m_nSelected].SetEnabledFlag(1);
            HilightArrow(&m_lblPlus[m_nSelected]);
#else
                    m_btnPlus[m_nSelected].SetEnabledFlag(0);
                    m_btnPlus[m_nSelected].SetActive(0);
                }
            }
            m_btnMinus[m_nSelected].SetEnabledFlag(1);
#endif
        }
        else
        {
            CSWGuiMessageBox* pMessageBox =
                g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
            pMessageBox->SetAllowCancel(0);
            pMessageBox->SetMessage(42181);
            pMessageBox->SetCallback(NULL, NULL, NULL);
            m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
        }
    }
}
// PC 0x006F8880  Xbox 0x00130D20
void CSWGuiAbilitiesCharGen::HandleInputEvent(int nEvent, int nState)
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
            if (m_nPointsRemaining > 0)
            {
                OnRecommendButton();
            }
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
// PC 0x006F70E0  Xbox 0x0012F4E0
void CSWGuiAbilitiesCharGen::OnEnterPointsButton(CSWGuiControl* pControl)
{
    int i;

    for (i = 0; i < 6; i++)
    {
        if (pControl == &m_btnPoints[i])
        {
            m_nSelected = i;
            m_btnPoints[i].m_cText.m_cParams.SetColor(CGuiInGame::COLOR_YELLOW);
            m_lblScore[i].m_cText.m_cParams.SetColor(CGuiInGame::COLOR_YELLOW);
        }
        else
        {
            m_btnPoints[i].m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
            m_lblScore[i].m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
        }
    }

    int nScore = m_aScore[m_nSelected];
    CExoString sCost(DetermineCostToIncreaseScore(nScore));
    CExoString sModifier;
    CSWCLevelUpStats* pStats = m_pCreature->m_pStats;
    int nModifier =
        pStats->CalcStatModifier((unsigned char)m_aScore[m_nSelected]);

    if (nModifier < 0)
    {
        CExoString sNegative(nModifier);
        sModifier = sNegative;
    }
    else if (nModifier == 0)
    {
        sModifier = "-";
    }
    else if (nModifier > 0)
    {
        sModifier = CExoString("+") + CExoString(nModifier);
    }

    m_lblCostPoints.m_cText.m_cParams.SetText(sCost);
    m_lblAbilityMod.m_cText.m_cParams.SetText(sModifier);
    SetDescription(m_aDescriptionStrRef[m_nSelected]);
    CSWGuiTextParams* pLabelParams = &m_lblScore[m_nSelected].m_cText.m_cParams;
    pLabelParams->m_bHilightText = 1;
    CSWGuiTextParams* pButtonParams =
        &m_btnPoints[m_nSelected].m_cText.m_cParams;
    pButtonParams->m_bHilightText = 1;
}
// PC 0x006F7330  Xbox 0x0012F730
void CSWGuiAbilitiesCharGen::OnAcceptButton()
{
    if (m_nPointsRemaining > 0)
    {
        CSWGuiMessageBox* pMessageBox =
            g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
        pMessageBox->SetAllowCancel(0);
#ifdef _XBOX
        pMessageBox->SetMessage(41816);
#else
        pMessageBox->SetMessage(48217);
#endif
        pMessageBox->SetCallback(NULL, NULL, NULL);
        m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
    }
    else
    {
        CompletePanel();
    }
}
// PC 0x006F6E70  Xbox 0x0012EF60
void CSWGuiAbilitiesCharGen::CompletePanel()
{
    if (m_bLevelUp != 0)
    {
        static_cast<CSWGuiLevelUpPanel*>(m_pParentPanel)
            ->NewAbilityScores(m_aScore[0], m_aScore[5], m_aScore[4],
                               m_aScore[3], m_aScore[2], m_aScore[1],
                               m_nPointsRemaining);
    }
    else
    {
        m_pCreature->m_pStats->SetSTRBase(m_aScore[0]);
        m_pCreature->m_pStats->SetCHABase(m_aScore[5]);
        m_pCreature->m_pStats->SetINTBase(m_aScore[4]);
        m_pCreature->m_pStats->SetWISBase(m_aScore[3]);
        m_pCreature->m_pStats->SetCONBase(m_aScore[2]);
        m_pCreature->m_pStats->SetDEXBase(m_aScore[1], 1);
        m_pCreature->m_pStats->m_nCreationStage = 0;
    }
    m_pManager->PopModalPanel();
    m_pParentPanel->SetVisible(1);
    m_bDeferredRemoval = 0;
    m_nExitState = EXITSTATE_DELETE;
    if (m_bLevelUp != 0)
    {
        static_cast<CSWGuiLevelUpPanel*>(m_pParentPanel)
            ->AbilitySelectionCompleted();
    }
    else
    {
        static_cast<CSWGuiCustomPanel*>(m_pParentPanel)
            ->AbilityCreationCompleted();
    }
}

#ifdef _XBOX
// PC 0x0041A290
inline int CSWGuiListBox::IsOversized()
{
    return m_nContentHeight + m_nPadding > m_cItemExtent.m_nHeight &&
           m_lItems.num > 0 && !m_bHideScrollIcon;
}
#endif
// PC 0x006F6FA0  Xbox 0x0012F090
void CSWGuiAbilitiesCharGen::SetDescription(unsigned long nStrRef)
{
    STR_RES strRes;

    g_pTlkTable->Fetch(nStrRef, strRes, CTlkTable::GENDER_MALE);
    m_lblDescription.m_cText.m_cParams.SetText(strRes.m_sString);
    m_lbDescription.ClearItems();
    CSWGuiExtent cExtent = m_lblDescription.m_cExtent;
    if (strRes.m_sString.GetLength() > 0)
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
// PC 0x006F7390  Xbox 0x0012F790
void CSWGuiAbilitiesCharGen::OnRecommendButton()
{
    unsigned char nLevel;
    CSWClass* pClass;
    int i;

    if (m_bLevelUp != 0)
    {
        nLevel = m_pCreature->m_pStats->GetNumClasses() - 1;
        pClass = &g_pRules->m_pClasses[m_pCreature->m_pStats->GetClass(nLevel)];
        switch (pClass->m_nPrimaryAbility)
        {
        case 0:
            m_aScore[0]++;
            break;
        case 3:
            m_aScore[4]++;
            break;
        case 5:
            m_aScore[5] = m_aScore[5] + 1;
            break;
        case 4:
            m_aScore[3]++;
            break;
        case 1:
            m_aScore[1]++;
            break;
        case 2:
            m_aScore[2]++;
            break;
        }
    }
    else
    {
        nLevel = m_pCreature->m_pStats->GetNumClasses() - 1;
        pClass = &g_pRules->m_pClasses[m_pCreature->m_pStats->GetClass(nLevel)];
        m_aScore[0] = pClass->GetStrength();
        m_aScore[1] = pClass->GetDexterity();
        m_aScore[2] = pClass->GetConstitution();
        m_aScore[3] = pClass->GetWisdom();
        m_aScore[4] = pClass->GetIntelligence();
        m_aScore[5] = pClass->GetCharisma();
    }
    m_nPointsRemaining = 0;
    {
        CExoString sRemaining(0);
        m_lblRemainingSelections.m_cText.m_cParams.SetText(sRemaining);
    }
    for (i = 0; i < 6; i++)
    {
        {
            CExoString sScore(m_aScore[i]);
            m_btnPoints[i].m_cText.m_cParams.SetText(sScore);
        }
        if (m_bLevelUp != 0)
        {
#ifdef _XBOX
            m_lblMinus[i].SetEnabledFlag(m_aScore[i] != m_aMinimumScore[i]);
        }
        else
        {
            m_lblMinus[i].SetEnabledFlag(m_aScore[i] != 8);
            m_lblPlus[i].SetEnabledFlag(m_aScore[i] != 18);
#else
            m_btnMinus[i].SetEnabledFlag(m_aScore[i] != m_aMinimumScore[i]);
        }
        else
        {
            m_btnMinus[i].SetEnabledFlag(m_aScore[i] != 8);
            m_btnPlus[i].SetEnabledFlag(m_aScore[i] != 18);
#endif
        }
    }
    OnEnterPointsButton(&m_btnPoints[m_nSelected]);
}

#ifdef _XBOX
// Xbox 0x0012F210
void CSWGuiAbilitiesCharGen::Draw(float fElapsed)
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
// Xbox 0x0012FA20
void CSWGuiAbilitiesCharGen::HilightArrow(CSWGuiLabel* pLabel)
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
