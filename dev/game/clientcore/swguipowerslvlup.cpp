#include "exobase.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "nwclass.h"
#include "nwccreaturestats.h"
#include "nwspell.h"
#include "guiingame.h"
#include "swguiingame.h"
#include "swguileveluppanel.h"
#include "swguipowerslvlup.h"
#include "nwccreature.h"
// PC 0x00763590
CSWGuiPowersLevelUp::CSWGuiPowersLevelUp(CSWGuiManager* pManager,
                                         unsigned long nCreature)
    : CSWGuiPanel(pManager)
{
    m_pManager = g_pAppManager->m_pClientExoApp->GetSWGuiManager();
    CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
    m_pCreature = pClientExoApp->GetCreatureByGameObjectID(nCreature);
    CSWSCreatureStats* pServerStats =
        m_pCreature->GetServerCreature()->m_pStats;
    m_bIsPC = pServerStats->m_bIsPC;
    m_nSelectedPower = -1;
#ifdef _XBOX
    StartLoadFromLayout("pwrlvlup");
#else
    {
        CResRef cLayout("pwrlvlup");
        StartLoadFromLayout(cLayout);
    }
#endif

    InitControl(m_lblTitle, "MAIN_TITLE_LBL", 1);
    InitControl(m_lblSubTitle, "SUB_TITLE_LBL", 1);
    InitControl(m_lblRemainingBox, "REMAINING_BOX_LBL", 1);
    InitControl(m_lblRemainingSelections, "REMAINING_SELECTIONS_LBL", 1);
    InitControl(m_lblSelectionsRemaining, "SELECTIONS_REMAINING_LBL", 1);
#ifdef _XBOX
    InitControl(m_lblRecommended, "RECOMMENDED_LBL", 1);
    InitControl(m_lblAccept, "ACCEPT_LBL", 1);
#endif
    InitControl(m_lbDescription, "LB_DESC", 1);
    m_lbDescription.m_nSelectSound = CSWGuiManager::GUISOUND_SELECTION;
#ifdef _XBOX
    InitControl(m_lblBack, "BACK_LBL", 1);
    InitControl(m_lblSelect, "SELECT_LBL", 1);
#endif
    InitControl(m_lbPowers, "LB_POWERS", 1);
#ifdef _XBOX
    InitControl(m_lblA, "LBL_A", 1);
    InitControl(m_lblB, "LBL_B", 1);
    InitControl(m_lblX, "LBL_X", 1);
    InitControl(m_lblY, "LBL_Y", 1);
    InitControl(m_lblRanalog, "LBL_RANALOG", 1);
#endif
    InitControl(m_lblPower, "LBL_POWER", 1);
#ifndef _XBOX
    InitControl(m_btnRecommended, "RECOMMENDED_BTN", 1);
    InitControl(m_btnSelect, "SELECT_BTN", 1);
    InitControl(m_btnAccept, "ACCEPT_BTN", 1);
    InitControl(m_btnBack, "BACK_BTN", 1);
#endif
    StopLoadFromLayout();

    CExoString sPower = g_pTlkTable->GetSimpleString(42488);
    m_sSelectText = g_pTlkTable->GetSimpleString(38455) + " " + sPower;
    m_sDeselectText = g_pTlkTable->GetSimpleString(38456) + " " + sPower;

    m_lblDescription.Initialize(
        m_lbDescription.m_pProtoItem->m_cExtent,
        static_cast<CSWGuiLabel*>(m_lbDescription.m_pProtoItem), 1.0f);
    pServerStats = m_pCreature->GetServerCreature()->m_pStats;
    m_cChart.CreatePowerChart(pServerStats, 1);
    InitiatePowers();

#ifndef _XBOX
    m_lbPowers.AddEvent(
        504, this,
        (CSWGuiHandler)&CSWGuiPowersLevelUp::OnPowerSelectionChanged);
    m_lbPowers.AddEvent(505, this,
                        (CSWGuiHandler)&CSWGuiPowersLevelUp::OnDoubleClick);
    m_lbPowers.SetSelectOnLeftClick(1);
    m_btnRecommended.SetFocusable(0);
    m_btnRecommended.AddEvent(39, this,
                              (CSWGuiHandler)&CSWGuiPanel::OnYButtonPressed);
    m_btnSelect.SetFocusable(0);
    m_btnSelect.AddEvent(39, this,
                         (CSWGuiHandler)&CSWGuiPanel::OnXButtonPressed);
    m_btnAccept.SetFocusable(0);
    m_btnAccept.AddEvent(39, this,
                         (CSWGuiHandler)&CSWGuiPanel::OnAButtonPressed);
    m_btnBack.SetFocusable(0);
    m_btnBack.AddEvent(GUIEVENT_ACTIVATE, this, (CSWGuiHandler)&CSWGuiPanel::OnBButtonPressed);
#endif
    SetActiveControl(&m_lbPowers, 0);
}
// PC 0x007620D0
CSWGuiPowersLevelUp::~CSWGuiPowersLevelUp()
{
    m_lbDescription.ClearItems();
    m_lbPowers.ClearItems();
}

void CSWGuiPowersLevelUp::OnPanelAdded()
{
#ifndef _XBOX
    CSWGuiPanel::OnPanelAdded();
#endif
    g_pAppManager->m_pClientExoApp->ShowTutorialWindow(0x13, OBJECT_INVALID,
                                                       OBJECT_INVALID, 0);
}
// PC 0x00762020
void CSWGuiPowersLevelUp::HandleCancelButton()
{
    if (m_pParentPanel != NULL)
    {
        m_pManager->PopModalPanel();
        m_pParentPanel->SetVisible(1);
        m_bDeferredRemoval = 0;
        m_nExitState = EXITSTATE_DELETE;
        m_pParentPanel->CancelledByChild();
    }
}
// PC 0x00762350
void CSWGuiPowersLevelUp::OnAcceptButton()
{
    if (m_nPowersRemaining > 0 && m_lAvailable.num > 0)
    {
        CSWGuiMessageBox* pMessageBox =
            g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
        pMessageBox->SetAllowCancel(0);
        pMessageBox->SetCallback(NULL, NULL, NULL);
#ifdef _XBOX
        pMessageBox->SetMessage(42188);
#else
        pMessageBox->SetMessage(125861);
#endif
        m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
    }
    else
    {
        unsigned int nClass =
            (unsigned char)(m_pCreature->m_pStats->m_nClassCount - 1);
        for (int i = 0; i < m_lChosen.num; i++)
        {
            m_pCreature->m_pStats->AddKnownSpell(nClass, m_lChosen.element[i]);
        }
        m_pManager->PopModalPanel();
        m_pParentPanel->SetVisible(1);
        m_bDeferredRemoval = 0;
        m_nExitState = EXITSTATE_DELETE;
        m_pParentPanel->PowersSelectionCompleted();
    }
}

#ifndef _XBOX
void CSWGuiPowersLevelUp::Draw(float fElapsed)  // PC 0x00762060
{
    switch (m_nPowerState)
    {
    case POWER_GRANTED:
        m_btnSelect.m_cText.m_cParams.SetText(m_sDeselectText);
        m_btnSelect.m_cText.m_cParams.SetColor(CGuiInGame::DISABLED_TEXT);
        m_btnSelect.m_cBorder.m_cParams.m_vColor = CGuiInGame::DISABLED_ICON_COLOR;
        break;
    case POWER_UNAVAILABLE:
        m_btnSelect.m_cText.m_cParams.SetText(m_sSelectText);
        m_btnSelect.m_cText.m_cParams.SetColor(CGuiInGame::DISABLED_TEXT);
        m_btnSelect.m_cBorder.m_cParams.m_vColor = CGuiInGame::DISABLED_ICON_COLOR;
        break;
    }
    CSWGuiPanel::Draw(fElapsed);
}
#endif
// PC 0x00762640
void CSWGuiPowersLevelUp::OnEnterPower(unsigned long nPower)
{
#ifdef _XBOX
    unsigned char nState;
    DeterminePower(nPower, nState);
    m_nSelectedPower = nPower;
    CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(nPower);

    switch (nState)
    {
    case POWER_AVAILABLE:
        {
            CSWGuiTextParams* pParams = &m_lblSelect.m_cText.m_cParams;
            pParams->SetText(m_sSelectText);
            pParams->SetColor(CGuiInGame::COLOR_BLUE);
            break;
        }
    case POWER_CHOSEN:
        {
            CSWGuiTextParams* pParams = &m_lblSelect.m_cText.m_cParams;
            pParams->SetText(m_sDeselectText);
            pParams->SetColor(CGuiInGame::COLOR_BLUE);
            break;
        }
    case POWER_GRANTED:
        {
            CSWGuiTextParams* pParams = &m_lblSelect.m_cText.m_cParams;
            pParams->SetText(m_sDeselectText);
            pParams->SetColor(CGuiInGame::DISABLED_TEXT);
            break;
        }
    case POWER_UNAVAILABLE:
        {
            CSWGuiTextParams* pParams = &m_lblSelect.m_cText.m_cParams;
            pParams->SetText(m_sSelectText);
            pParams->SetColor(CGuiInGame::DISABLED_TEXT);
            break;
        }
    }
#else
    DeterminePower(nPower, m_nPowerState);
    m_nSelectedPower = nPower;
    CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(nPower);

    switch (m_nPowerState)
    {
    case POWER_AVAILABLE:
        {
            CSWGuiTextParams* pParams = &m_btnSelect.m_cText.m_cParams;
            pParams->SetText(m_sSelectText);
            pParams->SetColor(CGuiInGame::COLOR_BLUE);
            m_btnSelect.m_cBorder.m_cParams.m_vColor =
                CGuiInGame::ENABLED_ICON_COLOR;
            break;
        }
    case POWER_CHOSEN:
        {
            CSWGuiTextParams* pParams = &m_btnSelect.m_cText.m_cParams;
            pParams->SetText(m_sDeselectText);
            pParams->SetColor(CGuiInGame::COLOR_BLUE);
            m_btnSelect.m_cBorder.m_cParams.m_vColor =
                CGuiInGame::ENABLED_ICON_COLOR;
            break;
        }
    case POWER_GRANTED:
        {
            CSWGuiTextParams* pParams = &m_btnSelect.m_cText.m_cParams;
            pParams->SetText(m_sDeselectText);
            pParams->SetColor(CGuiInGame::DISABLED_TEXT);
            m_btnSelect.m_cBorder.m_cParams.m_vColor =
                CGuiInGame::DISABLED_ICON_COLOR;
            break;
        }
    case POWER_UNAVAILABLE:
        {
            CSWGuiTextParams* pParams = &m_btnSelect.m_cText.m_cParams;
            pParams->SetText(m_sSelectText);
            pParams->SetColor(CGuiInGame::DISABLED_TEXT);
            m_btnSelect.m_cBorder.m_cParams.m_vColor =
                CGuiInGame::DISABLED_ICON_COLOR;
            break;
        }
    }
#endif
    m_lblPower.m_cText.m_cParams.SetStrRef(pSpell->m_nNameStrRef);
    SetDescription(pSpell->m_nDescriptionStrRef);
}
// PC 0x00762790
void CSWGuiPowersLevelUp::BuildButtons()
{
    int i;
    int j;
    int k;

    m_cChart.SetSkillStatusAll(CSWGuiSkillFlow::STATUS_UNAVAILABLE);
    for (i = 0; i < m_lGranted.num; i++)
    {
        m_cChart.SetSkillStatus(m_lGranted.element[i], 1);
    }
    for (j = 0; j < m_lChosen.num; j++)
    {
        m_cChart.SetSkillStatus(m_lChosen.element[j], 4);
    }
    for (k = 0; k < m_lAvailable.num; k++)
    {
        if (m_bIsPC == 0 &&
            (m_lAvailable.element[k] == CSWSpell::FORCE_POWER_AFFECT_MIND ||
             m_lAvailable.element[k] == CSWSpell::FORCE_POWER_DOMINATE))
        {
            m_cChart.SetSkillStatus(m_lAvailable.element[k], 3);
        }
        else
        {
            m_cChart.SetSkillStatus(m_lAvailable.element[k], 0);
        }
    }
    if (m_nSelectedPower == -1)
    {
        m_lbPowers.AddControls(
            *reinterpret_cast<CExoArrayList<CSWGuiControl*>*>(&m_cChart.m_cFlows),
            0, 1, 0);
        m_nSelectedPower = m_cChart.SetTopLeft();
    }
    m_cChart.SetSelectedSkill(m_nSelectedPower);
    unsigned char nRow = m_cChart.m_nSelectedRow;
    m_lbPowers.SetSelectedControl(nRow, 0);
    OnEnterPower(m_nSelectedPower);
    CExoString sRemaining(m_nPowersRemaining);
    m_lblRemainingSelections.m_cText.m_cParams.SetText(sRemaining);
}
// PC 0x00762950
void CSWGuiPowersLevelUp::BuildAvailableList()
{
    CExoString sName;
    CSWCLevelUpStats* pStats = m_pCreature->m_pStats;

    for (unsigned short nSpell = 0;
         nSpell < g_pRules->m_pSpellArray->GetNumSpells(); nSpell++)
    {
        if (m_lGranted.IndexOf(nSpell) != -1 ||
            m_lChosen.IndexOf(nSpell) != -1 ||
            m_lAvailable.IndexOf(nSpell) != -1 ||
            !pStats->CanChooseSpell(nSpell, m_nSpellLevel, &m_lChosen))
        {
            continue;
        }

        CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(nSpell);
        if (pSpell->m_aMasterSpells[0] == -2 || pSpell->m_nNameStrRef == 0)
        {
            continue;
        }

        sName = pSpell->GetSpellNameText();
        int bFound = FALSE;
        int i = 0;
        while (!bFound && i < m_lAvailable.num)
        {
            unsigned long nAvailable = m_lAvailable.element[i];
            if (g_pRules->m_pSpellArray->GetSpell(nAvailable)
                    ->GetSpellNameText() > sName)
            {
                bFound = TRUE;
            }
            else
            {
                i++;
            }
        }
        if (bFound)
        {
            m_lAvailable.Insert(nSpell, i);
        }
        else
        {
            m_lAvailable.Add(nSpell);
        }
    }
}

#ifndef _XBOX
void CSWGuiPowersLevelUp::OnPowerSelectionChanged(CSWGuiControl* pControl)  // PC 0x00762B90
{
    CSWGuiSkillFlow* pFlow =
        static_cast<CSWGuiSkillFlow*>(m_lbPowers.GetSelectedControl());
#ifndef _XBOX
    unsigned int nPower =
        pFlow->SkillHitCheckMouse(m_lbPowers.m_nMouseX, m_lbPowers.m_nMouseY);
#endif
#ifndef _XBOX
    unsigned short nId = nPower;
#endif

#ifndef _XBOX
    m_cChart.SetSelectedSkill(nPower);
#endif
#ifndef _XBOX
    OnEnterPower(nId);
#endif
}
#endif
// PC 0x00762BE0
void CSWGuiPowersLevelUp::InitiatePowers()
{
    CExoString sName;
    CSWCLevelUpStats* pStats = m_pCreature->m_pStats;

    for (unsigned char nClass = 0; nClass < pStats->GetNumClasses(); nClass++)
    {
        for (unsigned char nIndex = 0;
             nIndex < pStats->GetNumberKnownSpells(nClass, 0); nIndex++)
        {
            unsigned long nSpell = pStats->GetKnownSpell(nClass, 0, nIndex);
            int bFound = FALSE;
            int i = 0;
            sName = g_pRules->m_pSpellArray->GetSpell(nSpell)
                        ->GetSpellNameText();
            while (!bFound && i < m_lGranted.num)
            {
                unsigned long nGranted = m_lGranted.element[i];
                if (g_pRules->m_pSpellArray->GetSpell(nGranted)
                        ->GetSpellNameText() > sName)
                {
                    bFound = TRUE;
                }
                else
                {
                    i++;
                }
            }
            if (bFound)
            {
                m_lGranted.Insert(nSpell, i);
            }
            else
            {
                m_lGranted.Add(nSpell);
            }
        }
    }

    m_nPowersRemaining = pStats->ComputeNumberKnownSpellsLeft();
    m_nSpellLevel = m_nPowersRemaining + 1;
    BuildAvailableList();
    BuildButtons();
}
// PC 0x00763080
void CSWGuiPowersLevelUp::AddChosenPower(unsigned long nPower)
{
    int nIndex = m_lAvailable.IndexOf(nPower);
    if (nIndex == -1)
    {
        return;
    }
    m_lAvailable.DelIndex(nIndex);

    int bInsert = FALSE;
    CExoString sName = g_pRules->m_pSpellArray->GetSpell(nPower)->GetSpellNameText();
    int i = 0;
    while (!bInsert)
    {
        if (i < m_lChosen.num)
        {
            int nSpell = m_lChosen.element[i];
            if (g_pRules->m_pSpellArray->GetSpell(nSpell)->GetSpellNameText() > sName)
                bInsert = TRUE;
            else
                i++;
        }
        else
            break;
    }
    m_nPowersRemaining--;
    if (bInsert)
    {
        m_lChosen.Insert(nPower, i);
    }
    else
    {
        m_lChosen.Add(nPower);
    }
    BuildAvailableList();
}
// PC 0x00763230
void CSWGuiPowersLevelUp::RemoveChosenPower(unsigned long nPower)
{
    int nIndex = m_lChosen.IndexOf(nPower);
    if (nIndex == -1)
    {
        return;
    }
    m_lChosen.DelIndex(nIndex);
    m_nPowersRemaining++;

    CSWCLevelUpStats* pStats = m_pCreature->m_pStats;
    int bRemoved = FALSE;
    for (int i = 0; i < m_lChosen.num; i++)
    {
        int nSpell = m_lChosen.element[i];
        if (!pStats->HasSpellPrereq(g_pRules->m_pSpellArray->GetSpell(nSpell),
                                    &m_lChosen))
        {
            RemoveChosenPower(m_lChosen.element[i]);
            bRemoved = TRUE;
            break;
        }
    }
    if (!bRemoved)
    {
        m_lAvailable.num = 0;
        BuildAvailableList();
    }
}
// PC 0x00763320
void CSWGuiPowersLevelUp::OnRecommendButton()
{
    CExoArrayList<unsigned long> lRecommended;
    CSWCLevelUpStats* pStats = m_pCreature->m_pStats;
    int i;

    pStats->AddDefaultPackageSpells(&lRecommended);
    while (m_lChosen.num != 0)
    {
        RemoveChosenPower(m_lChosen.element[0]);
    }
    for (i = 0; i < lRecommended.num; i++)
    {
        AddChosenPower(lRecommended.element[i]);
    }
    CSWGuiSkillInfoBox* pInfoBox =
        g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pSkillInfoBox;
    pInfoBox->SetSkillList(lRecommended, CSWGuiInGameSkillEntry::ABILITY_POWER);
    pInfoBox->SetMessage(0xA511);
    m_pManager->AddPanel(pInfoBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
    BuildButtons();
}
// PC 0x00763D40
void CSWGuiPowersLevelUp::HandleInputEvent(int nEvent, int nState)
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
        case 39:
        case 45:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            OnAcceptButton();
            break;
        case 42:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            OnRecommendButton();
            break;
        case 41:
            OnPowerPicked(m_cChart.GetSelected());
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            break;
        case 47:
        case 48:
        case 49:
        case 50:
        case 61:
        case 62:
        case 63:
        case 64:
            OnEnterPower(m_cChart.HandleInput(nEvent));
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
// PC 0x00763430
void CSWGuiPowersLevelUp::OnPowerPicked(unsigned long nPower)
{
    unsigned char nState;
    unsigned long nMessage;

    DeterminePower(nPower, nState);
    switch (nState)
    {
    case POWER_AVAILABLE:
        if (m_bIsPC == 0 &&
            (nPower == CSWSpell::FORCE_POWER_AFFECT_MIND ||
             nPower == CSWSpell::FORCE_POWER_DOMINATE))
        {
            nMessage = STRREF_POWER_RESTRICTED;
        }
        else if (m_nPowersRemaining > 0)
        {
            AddChosenPower(nPower);
            BuildButtons();
            return;
        }
        else
        {
            nMessage = STRREF_NO_POWERS_REMAINING;
        }
        break;
    case POWER_CHOSEN:
        RemoveChosenPower(nPower);
        BuildButtons();
        return;
    case POWER_GRANTED:
        nMessage = STRREF_POWER_GRANTED;
        break;
    case POWER_UNAVAILABLE:
        nMessage = STRREF_POWER_UNAVAILABLE;
        break;
    default:
        return;
    }

    CSWGuiMessageBox* pMessageBox =
        g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
    pMessageBox->SetAllowCancel(0);
    pMessageBox->SetCallback(NULL, NULL, NULL);
    pMessageBox->SetMessage(nMessage);
    m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
}
// PC 0x00736BE0
inline int CSWGuiSkillFlowChart::GetSelected()
{
    if (m_cFlows.num > 0)
    {
        return static_cast<CSWGuiSkillFlow*>(m_cFlows.element[m_nSelectedRow])
            ->m_aSkills[m_nSelectedCol]
            .m_nSkill;
    }
    return -1;
}

#ifndef _XBOX
void CSWGuiPowersLevelUp::OnDoubleClick(CSWGuiControl* pControl)  // PC 0x00763520
{
    int nSelected = m_cChart.GetSelected();
    CSWGuiSkillFlow* pFlow =
        static_cast<CSWGuiSkillFlow*>(m_lbPowers.GetSelectedControl());
#ifndef _XBOX
    unsigned int nPower = pFlow->SkillHitCheckMouse(m_lbPowers.GetMouseX(),
                                                    m_lbPowers.GetMouseY());
#endif

#ifndef _XBOX
    if (nPower == nSelected)
    {
        OnPowerPicked((unsigned short)nPower);
    }
#endif
}
#endif
// PC 0x00762430
void CSWGuiPowersLevelUp::DeterminePower(unsigned long nPower,
                                         unsigned char& nState)
{
    if (m_lGranted.IndexOf(nPower) != -1)
    {
        nState = POWER_GRANTED;
    }
    else if (m_lChosen.IndexOf(nPower) != -1)
    {
        nState = POWER_CHOSEN;
    }
    else if (m_lAvailable.IndexOf(nPower) != -1)
    {
        nState = POWER_AVAILABLE;
    }
    else
    {
        nState = POWER_UNAVAILABLE;
    }
}

#ifdef _XBOX

inline int CSWGuiListBox::IsOversized()
{
    return m_nContentHeight + m_nPadding > m_cItemExtent.m_nHeight &&
           m_lItems.num > 0 && !m_bHideScrollIcon;
}
#endif
// PC 0x007624F0
void CSWGuiPowersLevelUp::SetDescription(unsigned long nStrRef)
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
