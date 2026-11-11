#include "exobase.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "guiingame.h"
#include "swguiingametutorialbox.h"
#include "swguiingame.h"
#include "swguiabilitieschargen.h"
#include "swguifeatschargen.h"
#include "nwccreature.h"

static inline void SwguifeatschargenTrace1();

static inline void SWGuiFeatsCharGenTrace();
#define SWGUIFEATSCHARGEN_TRACE() SWGuiFeatsCharGenTrace()
// PC 0x006F3D60  Xbox 0x0012BB00
CSWGuiFeatsCharGen::CSWGuiFeatsCharGen(CSWGuiManager* pManager,
                                     CSWCCreature* pCreature,
                                     int bLevelUp, int bBonusFeatSelection)
    : CSWGuiPanel(pManager)
{
    m_pManager = g_pAppManager->m_pClientExoApp->GetSWGuiManager();
    m_pCreature = pCreature;
    m_bLevelUp = bLevelUp;
    m_bBonusFeatSelection = bBonusFeatSelection;
    m_nSelectedFeat = FEAT_INVALID;
#ifdef _XBOX
    StartLoadFromLayout("FTCHRGEN");
#else
    {
        CResRef cLayout("FTCHRGEN");
        StartLoadFromLayout(cLayout);
    }
#endif
    InitControl(m_lblMainTitle, "MAIN_TITLE_LBL", 1);
    InitControl(m_lblSubTitle, "SUB_TITLE_LBL", 1);
    InitControl(m_lblRemainingBox, "REMAINING_BOX_LBL", 1);
    InitControl(m_lblStdRemainingSelections, "STD_REMAINING_SELECTIONS_LBL", 1);
    InitControl(m_lblStdSelectionsRemaining, "STD_SELECTIONS_REMAINING_LBL", 1);
    InitControl(m_lblDesc, "DESC_LBL", 1);
    InitControl(m_lbDesc, "LB_DESC", 1);
    m_lbDesc.m_nSelectSound = CSWGuiManager::GUISOUND_SELECTION;
    InitControl(m_lbFeats, "LB_FEATS", 1);
#ifdef _XBOX
    InitControl(m_lblA, "LBL_A", 1);
    InitControl(m_lblB, "LBL_B", 1);
    InitControl(m_lblX, "LBL_X", 1);
    InitControl(m_lblY, "LBL_Y", 1);
    InitControl(m_lblAccept, "LBL_ACCEPT", 1);
    InitControl(m_lblRecommended, "LBL_RECOMMENDED", 1);
    InitControl(m_lblBack, "LBL_BACK", 1);
    InitControl(m_lblSelect, "LBL_SELECT", 1);
    InitControl(m_lblRanalog, "LBL_RANALOG", 1);
#endif
    InitControl(m_lblName, "LBL_NAME", 1);
#ifndef _XBOX
    InitControl(m_btnAccept, "BTN_ACCEPT", 1);
    InitControl(m_btnRecommended, "BTN_RECOMMENDED", 1);
    InitControl(m_btnBack, "BTN_BACK", 1);
    InitControl(m_btnSelect, "BTN_SELECT", 1);
#endif
    StopLoadFromLayout();
    CExoString sFeat = g_pTlkTable->GetSimpleString(STRREF_FEAT);
    m_sFeatUnavailableText = g_pTlkTable->GetSimpleString(STRREF_ADD) +
                             " " + sFeat;
    m_sFeatLockedText = g_pTlkTable->GetSimpleString(STRREF_REMOVE) +
                        " " + sFeat;
    if (bLevelUp)
    {
        m_lblMainTitle.m_cText.m_cParams.SetStrRef(STRREF_LEVEL_UP);
    }
    if (bBonusFeatSelection)
    {
        m_lblSubTitle.m_cText.m_cParams.SetStrRef(STRREF_BONUS_FEATS);
    }
#ifndef _XBOX
    m_lblDescription.Initialize(m_lbDesc.m_pProtoItem->m_cExtent, (CSWGuiLabel*)m_lbDesc.m_pProtoItem, 1.0f);
#else
    m_lblDescription.Initialize(m_lbDesc.m_pProtoItem->m_cExtent, (CSWGuiLabel*)m_lbDesc.m_pProtoItem);
#endif
    InitiateFeats();
#ifndef _XBOX
    m_lbFeats.AddEvent(CSWGuiListBox::EVENT_SELECTION_CHANGED, this, (CSWGuiHandler)&CSWGuiFeatsCharGen::OnFeatSelectionChanged);
#ifndef _XBOX
    m_lbFeats.AddEvent(CSWGuiListBox::EVENT_DOUBLE_CLICK, this, (CSWGuiHandler)&CSWGuiFeatsCharGen::OnDoubleClick);
#endif
    m_lbFeats.SetSelectOnLeftClick(1);
    m_btnAccept.SetFocusable(0);
    m_btnAccept.AddEvent(GUIEVENT_ACTIVATE, this,
                         (CSWGuiHandler)&CSWGuiPanel::OnXButtonPressed);
    m_btnRecommended.SetFocusable(0);
    m_btnRecommended.AddEvent(GUIEVENT_ACTIVATE, this,
                              (CSWGuiHandler)&CSWGuiPanel::OnYButtonPressed);
    m_btnBack.SetFocusable(0);
    m_btnBack.AddEvent(GUIEVENT_ACTIVATE, this,
                       (CSWGuiHandler)&CSWGuiPanel::OnBButtonPressed);
    m_btnSelect.SetFocusable(0);
    m_btnSelect.AddEvent(GUIEVENT_ACTIVATE, this,
                         (CSWGuiHandler)&CSWGuiPanel::OnAButtonPressed);
#endif
    SetActiveControl(&m_lbFeats, 0);
}
// PC 0x006F2A50  Xbox 0x0012AE00
CSWGuiFeatsCharGen::~CSWGuiFeatsCharGen()
{
    m_lbDesc.ClearItems();
    m_lbFeats.ClearItems();
}
// PC 0x006F2F30  Xbox 0x0012B040
void CSWGuiFeatsCharGen::OnPanelAdded()
{
#ifndef _XBOX
    CSWGuiPanel::OnPanelAdded();
#endif
    if (m_bShowGrantedTutorial)
    {
        if (g_pAppManager->m_pClientExoApp->ShowTutorialWindow(
                0x12, OBJECT_INVALID, OBJECT_INVALID, 0))
        {
            CGuiInGame* pGui =
                g_pAppManager->m_pClientExoApp->GetInGameGui();
            pGui->m_pTutorialBox->SetCallback(
                this, (CSWGuiHandler)&CSWGuiFeatsCharGen::ShowGranted, NULL);
        }
        else
        {
            CGuiInGame* pGui =
                g_pAppManager->m_pClientExoApp->GetInGameGui();
            m_pManager->AddPanel(pGui->m_pSkillInfoBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
        }
    }
}
// PC 0x006F2C40  Xbox 0x0012AB40
void CSWGuiFeatsCharGen::ShowGranted(CSWGuiControl* pControl)
{
    SWGUIFEATSCHARGEN_TRACE();
    CGuiInGame* pGui = g_pAppManager->m_pClientExoApp->GetInGameGui();
    m_pManager->AddPanel(pGui->m_pSkillInfoBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
}
// PC 0x006F2C70  Xbox 0x0012AB70
void CSWGuiFeatsCharGen::HandleCancelButton()
{
    SWGUIFEATSCHARGEN_TRACE();
    if (m_pParentPanel != NULL)
    {
        m_pManager->PopModalPanel();
        m_pParentPanel->SetVisible(1);
        m_bDeferredRemoval = 0;
        m_nExitState = EXITSTATE_DELETE;
        if (m_bLevelUp)
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

#ifndef _XBOX
void CSWGuiFeatsCharGen::Draw(float fElapsed)  // PC 0x006F2CC0
{
    SWGUIFEATSCHARGEN_TRACE();
    if (m_nFeatState >= 2)
    {
        if (m_nFeatState > 3)
        {
            if (m_nFeatState == 4)
            {
                CSWGuiTextParams* pParams =
                    &m_btnSelect.m_cText.m_cParams;

                pParams->SetText(m_sFeatUnavailableText);
                pParams->SetColor(CGuiInGame::DISABLED_TEXT);
                m_btnSelect.m_cBorder.m_cParams.m_vColor =
                    CGuiInGame::DISABLED_ICON_COLOR;
            }
        }
        else
        {
            CSWGuiTextParams* pParams = &m_btnSelect.m_cText.m_cParams;

            pParams->SetText(m_sFeatLockedText);
            pParams->SetColor(CGuiInGame::DISABLED_TEXT);
            m_btnSelect.m_cBorder.m_cParams.m_vColor =
                CGuiInGame::DISABLED_ICON_COLOR;
        }
    }
    return CSWGuiPanel::Draw(fElapsed);
}
#endif
// PC 0x006F2FB0  Xbox 0x0012B0C0
void CSWGuiFeatsCharGen::OnEnterFeat(unsigned short nFeat)
{
#ifdef _XBOX
    unsigned char nState;
    DetermineFeat(nFeat, nState);
    m_nSelectedFeat = nFeat;
    CSWFeat* pFeat = g_pRules->GetFeat(nFeat);

    switch (nState)
    {
    case FEAT_AVAILABLE:
        {
            CSWGuiTextParams* pParams = &m_lblSelect.m_cText.m_cParams;
            pParams->SetText(m_sFeatUnavailableText);
            pParams->SetColor(CGuiInGame::COLOR_BLUE);
            break;
        }
    case FEAT_CHOSEN:
        {
            CSWGuiTextParams* pParams = &m_lblSelect.m_cText.m_cParams;
            pParams->SetText(m_sFeatLockedText);
            pParams->SetColor(CGuiInGame::COLOR_BLUE);
            break;
        }
    case FEAT_LOCKED:
    case FEAT_GRANTED:
        {
            CSWGuiTextParams* pParams = &m_lblSelect.m_cText.m_cParams;
            pParams->SetText(m_sFeatLockedText);
            pParams->SetColor(CGuiInGame::DISABLED_TEXT);
            break;
        }
    case FEAT_UNAVAILABLE:
        {
            CSWGuiTextParams* pParams = &m_lblSelect.m_cText.m_cParams;
            pParams->SetText(m_sFeatUnavailableText);
            pParams->SetColor(CGuiInGame::DISABLED_TEXT);
            break;
        }
    }
#else
    DetermineFeat(nFeat, m_nFeatState);
    m_nSelectedFeat = nFeat;
    CSWFeat* pFeat = g_pRules->GetFeat(nFeat);

    switch (m_nFeatState)
    {
    case FEAT_AVAILABLE:
        {
            CSWGuiTextParams* pParams = &m_btnSelect.m_cText.m_cParams;
            pParams->SetText(m_sFeatUnavailableText);
            pParams->SetColor(CGuiInGame::COLOR_BLUE);
            m_btnSelect.m_cBorder.m_cParams.m_vColor =
                CGuiInGame::ENABLED_ICON_COLOR;
            break;
        }
    case FEAT_CHOSEN:
        {
            CSWGuiTextParams* pParams = &m_btnSelect.m_cText.m_cParams;
            pParams->SetText(m_sFeatLockedText);
            pParams->SetColor(CGuiInGame::COLOR_BLUE);
            m_btnSelect.m_cBorder.m_cParams.m_vColor =
                CGuiInGame::ENABLED_ICON_COLOR;
            break;
        }
    case FEAT_LOCKED:
    case FEAT_GRANTED:
        {
            CSWGuiTextParams* pParams = &m_btnSelect.m_cText.m_cParams;
            pParams->SetText(m_sFeatLockedText);
            pParams->SetColor(CGuiInGame::DISABLED_TEXT);
            m_btnSelect.m_cBorder.m_cParams.m_vColor =
                CGuiInGame::DISABLED_ICON_COLOR;
            break;
        }
    case FEAT_UNAVAILABLE:
        {
            CSWGuiTextParams* pParams = &m_btnSelect.m_cText.m_cParams;
            pParams->SetText(m_sFeatUnavailableText);
            pParams->SetColor(CGuiInGame::DISABLED_TEXT);
            m_btnSelect.m_cBorder.m_cParams.m_vColor =
                CGuiInGame::DISABLED_ICON_COLOR;
            break;
        }
    }
#endif
    m_lblName.m_cText.m_cParams.SetStrRef(pFeat->m_nNameStrRef);
    SetDescription(pFeat->m_nDescriptionStrRef);
}
// PC 0x006F30F0  Xbox 0x0012B190
void CSWGuiFeatsCharGen::BuildButtons()
{
    int i;
    int j;
    int k;
    int m;

    m_cChart.SetSkillStatusAll(CSWGuiSkillFlow::STATUS_UNAVAILABLE);
    for (i = 0; i < m_lGranted.num; ++i)
    {
        m_cChart.SetSkillStatus(m_lGranted.element[i],
                              CSWGuiSkillFlow::STATUS_GRANTED);
    }
    for (j = 0; j < m_lChosen.num; ++j)
    {
        m_cChart.SetSkillStatus(m_lChosen.element[j],
                              CSWGuiSkillFlow::STATUS_CHOSEN);
    }
    for (k = 0; k < m_lAvailable.num; ++k)
    {
        m_cChart.SetSkillStatus(m_lAvailable.element[k],
                              CSWGuiSkillFlow::STATUS_AVAILABLE);
    }
    for (m = 0; m < m_lLocked.num; ++m)
    {
        m_cChart.SetSkillStatus(m_lLocked.element[m],
                              CSWGuiSkillFlow::STATUS_GRANTED_ALTERNATE);
    }
    if (m_nSelectedFeat == FEAT_INVALID)
    {
        m_lbFeats.AddControls(
            *reinterpret_cast<CExoArrayList<CSWGuiControl*>*>(&m_cChart.m_cFlows),
            0, 1, 0);
        m_nSelectedFeat = m_cChart.SetTopLeft();
    }
    m_cChart.SetSelectedSkill(m_nSelectedFeat);
    unsigned char nRow = m_cChart.m_nSelectedRow;
    m_lbFeats.SetSelectedControl(nRow, 0);
    OnEnterFeat(m_nSelectedFeat);
    CExoString sRemaining(m_nFeatsRemaining);
    m_lblStdRemainingSelections.m_cText.m_cParams.SetText(sRemaining);
}
// PC 0x006F3290  Xbox 0x0012B330
void CSWGuiFeatsCharGen::BuildAvailableList()
{
    CExoString sFeat;
    CSWCLevelUpStats* pStats = m_pCreature->m_pStats;

    for (int i = 0; i < g_pRules->GetNumFeats(); i++)
    {
        unsigned short nFeat = g_pRules->m_pFeatIndices[(unsigned short)i];
        if (m_lGranted.IndexOf(nFeat) == -1 &&
            m_lLocked.IndexOf(nFeat) == -1 &&
            m_lChosen.IndexOf(nFeat) == -1 &&
            m_lAvailable.IndexOf(nFeat) == -1 &&
            pStats->CanChooseFeat(nFeat, m_nGeneralFeatChoices,
                                  m_nBonusFeatChoices, &m_lChosen))
        {
            m_lAvailable.Add(nFeat);
        }
    }
}

#ifndef _XBOX
void CSWGuiFeatsCharGen::OnFeatSelectionChanged(CSWGuiControl* pControl)  // PC 0x006F3420
{
    CSWGuiSkillFlow* pFlow =
        static_cast<CSWGuiSkillFlow*>(m_lbFeats.GetSelectedControl());
#ifndef _XBOX
    unsigned int nFeat =
        pFlow->SkillHitCheckMouse(m_lbFeats.m_nMouseX, m_lbFeats.m_nMouseY);
#endif
#ifndef _XBOX
    unsigned short nId = nFeat;
#endif

#ifndef _XBOX
    m_cChart.SetSelectedSkill(nFeat);
#endif
#ifndef _XBOX
    return OnEnterFeat(nId);
#endif
}
#endif
// PC 0x006F3460  Xbox 0x0012B4E0
void CSWGuiFeatsCharGen::InitiateFeats()
{
    CSWCLevelUpStats* pStats = m_pCreature->m_pStats;
    if (!m_bLevelUp && !m_bBonusFeatSelection)
    {
        pStats->ClearFeats();
    }
    for (int i = 0; i < (unsigned short)pStats->GetNumFeats(); i++)
    {
        m_lGranted.Add(pStats->GetFeat(i));
    }

    m_bShowGrantedTutorial = 0;
    if (!m_bBonusFeatSelection)
    {
        CExoArrayList<unsigned long> lGranted;
        pStats->AddGrantedFeats(&lGranted);
        if (lGranted.num > 0)
        {
            for (int i = 0; i < lGranted.num; i++)
            {
                m_lLocked.Add((unsigned short)lGranted.element[i]);
            }
            for (int i = lGranted.num - 1; i >= 0; i--)
            {
                if (m_lGranted.Contains((unsigned short)lGranted.element[i]))
                {
                    lGranted.DelIndex(i);
                }
            }
            CSWGuiSkillInfoBox* pInfoBox =
                g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pSkillInfoBox;
            pInfoBox->SetSkillList(lGranted, CSWGuiInGameSkillEntry::ABILITY_FEAT);
            pInfoBox->SetMessage(42258);
            m_bShowGrantedTutorial = 1;
        }
    }

    pStats->CalcNumberFeats(m_nGeneralFeats, m_nBonusFeats);
    m_nGeneralFeatChoices = m_nGeneralFeats ? m_nGeneralFeats + 1 : 0;
    m_nBonusFeatChoices = m_nBonusFeats ? m_nBonusFeats + 1 : 0;
    if (m_bBonusFeatSelection)
    {
        m_nGeneralFeats = 0;
    }
    else
    {
        m_nBonusFeats = 0;
    }
    m_nFeatsRemaining = m_nGeneralFeats + m_nBonusFeats;
    if (m_nFeatsRemaining == 0)
    {
        OnAcceptButton();
    }
    else
    {
        m_cChart.CreateFeatChart(pStats, m_bBonusFeatSelection);
        BuildAvailableList();
        BuildButtons();
    }
}
// PC 0x006F37E0  Xbox 0x0012B860
void CSWGuiFeatsCharGen::AddChosenFeat(unsigned short nFeat)
{
    int nIndex = m_lAvailable.IndexOf(nFeat);
    if (nIndex == -1)
    {
        return;
    }
    m_lAvailable.DelIndex(nIndex);

    int bInsert = FALSE;
    CExoString sName = g_pRules->GetFeat(nFeat)->GetNameText();
    int i = 0;
    while (!bInsert)
    {
        if (i < GetNumChosenFeats())
        {
            if (g_pRules->GetFeat(m_lChosen.element[i])->GetNameText() > sName)
                bInsert = TRUE;
            else
                i++;
        }
        else
            break;
    }
    m_nFeatsRemaining--;
    if (bInsert)
    {
        m_lChosen.Insert(nFeat, i);
    }
    else
    {
        m_lChosen.Add(nFeat);
    }
    BuildAvailableList();
}
// PC 0x006F3990  Xbox 0x0012BA10
void CSWGuiFeatsCharGen::RemoveChosenFeat(unsigned short nFeat)
{
    int nIndex = m_lChosen.IndexOf(nFeat);
    if (nIndex == -1)
    {
        return;
    }
    m_lChosen.DelIndex(nIndex);
    m_nFeatsRemaining++;

    CSWCLevelUpStats* pStats = m_pCreature->m_pStats;
    int bRemoved = FALSE;
    for (int i = 0; i < m_lChosen.num; i++)
    {
        if (!pStats->HasFeatPrereq(g_pRules->GetFeat(m_lChosen.element[i]),
                                   &m_lChosen))
        {
            RemoveChosenFeat(m_lChosen.element[i]);
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
// PC 0x006F3A80  Xbox 0x0012C4C0
void CSWGuiFeatsCharGen::OnRecommendButton()
{
    CExoArrayList<unsigned short> lRecommended;
    CExoArrayList<unsigned long> lFeatInfo;

    unsigned char nClass = m_pCreature->m_pStats->GetClass(
        (unsigned char)(m_pCreature->m_pStats->GetNumClasses() - 1));
    CSWCLevelUpStats* pStats = m_pCreature->m_pStats;
    int nPackageClass = nClass;
    pStats->AddDefaultPackageFeats(
        nPackageClass, &lRecommended, !m_bBonusFeatSelection, m_bBonusFeatSelection);

    while (m_lChosen.num != 0)
    {
        RemoveChosenFeat(m_lChosen.element[0]);
    }
    lFeatInfo.SetSize(lRecommended.num);
    for (int i = 0; i < lRecommended.num; i++)
    {
        AddChosenFeat(lRecommended.element[i]);
        lFeatInfo.element[i] = lRecommended.element[i];
    }

    CSWGuiSkillInfoBox* pInfoBox =
        g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pSkillInfoBox;
    pInfoBox->SetSkillList(lFeatInfo, CSWGuiInGameSkillEntry::ABILITY_FEAT);
    pInfoBox->SetMessage(42256);
    m_pManager->AddPanel(pInfoBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
    BuildButtons();
}
// PC 0x006F3C20  Xbox 0x0012C660
void CSWGuiFeatsCharGen::OnFeatPicked(unsigned short nFeat)
{
    unsigned char nState;
    DetermineFeat(nFeat, nState);

    unsigned long nStrRef;
    switch (nState)
    {
    case FEAT_AVAILABLE:
        if (m_nFeatsRemaining > 0)
        {
            AddChosenFeat(nFeat);
            BuildButtons();
            return;
        }
        nStrRef = STRREF_NO_FEAT_CHOICES_REMAINING;
        break;
    case FEAT_CHOSEN:
        RemoveChosenFeat(nFeat);
        BuildButtons();
        return;
    case FEAT_LOCKED:
        nStrRef = STRREF_FEAT_AUTOMATIC_THIS_LEVEL;
        break;
    case FEAT_GRANTED:
        nStrRef = STRREF_FEAT_ALREADY_OWNED;
        break;
    case FEAT_UNAVAILABLE:
        nStrRef = STRREF_FEAT_PREREQUISITES_NOT_MET;
        break;
    default:
        return;
    }

    CSWGuiMessageBox* pMessageBox =
        g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
    pMessageBox->SetAllowCancel(0);
    pMessageBox->SetCallback(NULL, NULL, NULL);
    pMessageBox->SetMessage(nStrRef);
    m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
}
// PC 0x006ABA50  Xbox 0x000E2D90
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
void CSWGuiFeatsCharGen::OnDoubleClick(CSWGuiControl* pControl)  // PC 0x006F3CF0
{
    int nSelected = m_cChart.GetSelected();
    CSWGuiSkillFlow* pFlow =
        static_cast<CSWGuiSkillFlow*>(m_lbFeats.GetSelectedControl());
    int nHit =
        pFlow->SkillHitCheckMouse(m_lbFeats.m_nMouseX, m_lbFeats.m_nMouseY);

    if (nHit == nSelected)
    {
        OnFeatPicked(nHit);
    }
}
#endif
// PC 0x006F44C0  Xbox 0x0012C300
void CSWGuiFeatsCharGen::OnAcceptButton()
{
    if (m_nFeatsRemaining > 0 && m_lAvailable.num > 0)
    {
        CSWGuiMessageBox* pMessageBox =
            g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
        pMessageBox->SetAllowCancel(0);
        pMessageBox->SetCallback(NULL, NULL, NULL);
#ifdef _XBOX
        pMessageBox->SetMessage(42187);
#else
        pMessageBox->SetMessage(48215);
#endif
        m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
        return;
    }

    for (int i = 0; i < m_lChosen.num; i++)
    {
        CSWCLevelUpStats* pStats = m_pCreature->m_pStats;
        unsigned short nFeat = m_lChosen.element[i];
        pStats->m_lstFeats.AddUnique(nFeat);
    }
    m_pManager->PopModalPanel();
    m_pParentPanel->SetVisible(1);
    m_bDeferredRemoval = 0;
    m_nExitState = EXITSTATE_DELETE;
    m_lbFeats.ClearItems();
    m_cChart.ClearChart();

    unsigned char nGeneralFeats;
    unsigned char nBonusFeats;
    CSWCLevelUpStats* pStats = m_pCreature->m_pStats;
    pStats->CalcNumberFeats(nGeneralFeats, nBonusFeats);
    if (!m_bBonusFeatSelection && nBonusFeats > 0)
    {
        CSWGuiFeatsCharGen* pBonusPanel =
            new CSWGuiFeatsCharGen(m_pManager, m_pCreature, m_bLevelUp, 1);
        pBonusPanel->m_pParentPanel = m_pParentPanel;
        m_pManager->AddPanel(pBonusPanel, CSWGuiManager::PANEL_MODE_MODAL, 1);
    }
    else if (m_bLevelUp)
    {
        static_cast<CSWGuiLevelUpPanel*>(m_pParentPanel)
            ->FeatSelectionCompleted();
    }
    else
    {
        static_cast<CSWGuiCustomPanel*>(m_pParentPanel)
            ->FeatCreationCompleted();
    }
}
// PC 0x006F4680  Xbox 0x0012C730
void CSWGuiFeatsCharGen::HandleInputEvent(int nEvent, int nState)
{
    if (nState != 0)
    {
        switch (nEvent)
        {
        case GUIEVENT_CANCEL:
        case GUIEVENT_CANCEL_ALTERNATE:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            HandleCancelButton();
            break;
        case GUIEVENT_CONFIRM_SELECTION:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            OnAcceptButton();
            break;
        case GUIEVENT_ACTIVATE:
        case GUIEVENT_ACCEPT:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            OnFeatPicked(m_cChart.GetSelected());
            break;
        case GUIEVENT_RECOMMEND:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            OnRecommendButton();
            break;
        case GUIEVENT_LEFT:
        case GUIEVENT_RIGHT:
        case GUIEVENT_UP:
        case GUIEVENT_DOWN:
        case GUIEVENT_UP_ALTERNATE:
        case GUIEVENT_DOWN_ALTERNATE:
        case GUIEVENT_LEFT_ALTERNATE:
        case GUIEVENT_RIGHT_ALTERNATE:
            OnEnterFeat(m_cChart.HandleInput(nEvent));
            break;
        case GUIEVENT_SCROLL_DOWN:
            m_lbDesc.HandleInputEvent(GUIEVENT_DOWN, 1);
            break;
        case GUIEVENT_SCROLL_UP:
            m_lbDesc.HandleInputEvent(GUIEVENT_UP, 1);
            break;
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
// PC 0x006F2D30  Xbox 0x0012ABC0
void CSWGuiFeatsCharGen::DetermineFeat(unsigned short nFeat,
                                      unsigned char& nState)
{
    if (m_lAvailable.IndexOf(nFeat) != -1)
    {
        nState = FEAT_AVAILABLE;
    }
    else if (m_lChosen.IndexOf(nFeat) != -1)
    {
        nState = FEAT_CHOSEN;
    }
    else if (m_lGranted.IndexOf(nFeat) != -1)
    {
        nState = FEAT_GRANTED;
    }
    else if (m_lLocked.IndexOf(nFeat) != -1)
    {
        nState = FEAT_LOCKED;
    }
    else
    {
        nState = FEAT_UNAVAILABLE;
    }
}

#ifdef _XBOX
// PC 0x0041A290  Xbox 0x0004CEC0
inline int CSWGuiListBox::IsOversized()
{
    return m_nContentHeight + m_nPadding > m_cItemExtent.m_nHeight &&
           m_lItems.num > 0 && !m_bHideScrollIcon;
}
#endif
// PC 0x006F2DF0  Xbox 0x0012AC80
void CSWGuiFeatsCharGen::SetDescription(unsigned long nStrRef)
{
    SwguifeatschargenTrace1();
    STR_RES strRes;

    g_pTlkTable->Fetch(nStrRef, strRes, CTlkTable::GENDER_MALE);
    m_lblDescription.m_cText.m_cParams.SetText(strRes.m_sString);
    m_lbDesc.ClearItems();
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
    m_lbDesc.AddControls(&pControl, 1, 0, 0, 0);
    m_lbDesc.SetSelectedControl(&m_lblDescription, 0);
#ifdef _XBOX
    m_lblRanalog.SetEnabledFlag(m_lbDesc.IsOversized());
#endif
}

static inline void SWGuiFeatsCharGenTrace()
{
}

static inline void SwguifeatschargenTrace1()
{
}
