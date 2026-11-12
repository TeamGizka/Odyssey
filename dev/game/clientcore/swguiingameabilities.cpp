#include "appmanager.h"
#include "clientexoapp.h"
#include "swparty.h"
#include "nwsobject.h"
#include "exoresman.h"
#include "nwclass.h"
#include "nwfeat.h"
#include "nwskill.h"
#include "nwspell.h"
#include "nwrules.h"
#include "exotlktable.h"
#include "nwccreaturestats.h"
#include "partytables.h"
#include "serverexoapp.h"
#include "swguiingame.h"
#include "guiingame.h"
#include "nwccreature.h"
#include "nwsarea.h"
#include "swguiingamecharacter.h"

static inline void SwguiingameabilitiesTrace2();
static inline void SwguiingameabilitiesTrace1();
#define SWGUIINGAMEABILITIES_TRACE() SwguiingameabilitiesTrace2()
// PC 0x00736580
void CSWGuiInGameAbilities::OnPanelRemoved()
{
    m_lbAbility.ClearItems();
    m_cPowerChart.ClearChart();
    m_cFeatChart.ClearChart();
}
// PC 0x00736790
void CSWGuiInGameSkillEntry::SetExtent(const CSWGuiExtent& cExtent)
{
    if (cExtent.m_nWidth > 42)
    {
        m_cExtent = cExtent;
        CSWGuiExtent cIcon;
        cIcon.m_nLeft = m_cExtent.m_nLeft;
        cIcon.m_nTop = m_cExtent.m_nTop;
        cIcon.m_nWidth = 42;
        cIcon.m_nHeight = 42;
        m_cNormal.SetExtent(cIcon);
        m_cHilite.SetExtent(cIcon);
        CSWGuiExtent cSelection;
        cSelection.m_nLeft = m_cExtent.m_nLeft + 7;
        cSelection.m_nTop = m_cExtent.m_nTop + 7;
        cSelection.m_nWidth = 27;
        cSelection.m_nHeight = 27;
        m_cSelection.SetExtent(cSelection);

        cIcon.m_nLeft += 42;
        cIcon.m_nWidth = m_cExtent.m_nWidth - 42;
        CSWGuiLabel::m_cBorder.SetExtent(cIcon);
        CSWGuiLabelHilight::m_cHilite.SetExtent(cIcon);
        m_cText.SetExtent(CSWGuiExtent::Intersection(
            CSWGuiLabel::m_cBorder.GetInnerExtent(),
            CSWGuiLabelHilight::m_cHilite.GetInnerExtent()));
    }
}
// PC 0x007368C0
void CSWGuiInGameSkillEntry::Draw(float fElapsed)
{
    if (m_bActive)
    {
        CSWGuiLabelHilight::m_cHilite.Draw(fElapsed);
        m_cHilite.Draw(fElapsed);
    }
    else
    {
        CSWGuiLabel::m_cBorder.Draw(fElapsed);
        m_cNormal.Draw(fElapsed);
    }
    m_cSelection.Draw(fElapsed);
    m_cText.Draw(fElapsed);
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

#ifdef _XBOX
CSWGuiInGameAbilities::CSWGuiInGameAbilities(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    CExoString sName;

    StartLoadFromLayout("abilities");
    InitControl(m_lblPortrait, "LBL_PORTRAIT", 1);
    InitControl(m_lbDescription, "LB_DESC", 1);
    m_lbDescription.m_nSelectSound = CSWGuiManager::GUISOUND_SELECTION;
    InitControl(m_lbAbility, "LB_ABILITY", 1);
    InitControl(m_lblX, "LBL_X", 1);
    InitControl(m_lblY, "LBL_Y", 1);
    InitControl(m_lblWhite, "LBL_WHITE", 1);
    InitControl(m_lblSkills, "LBL_SKILLS", 1);
    InitControl(m_lblFeats, "LBL_FEATS", 1);
    InitControl(m_lblPowers, "LBL_POWERS", 1);
    InitControl(m_lblBonus, "LBL_BONUS", 1);
    InitControl(m_lblBonusValue, "LBL_BONUSVAL", 1);
    InitControl(m_lblSkillRank, "LBL_SKILLRANK", 1);
    InitControl(m_lblRankValue, "LBL_RANKVAL", 1);
    InitControl(m_lblTotal, "LBL_TOTAL", 1);
    InitControl(m_lblTotalValue, "LBL_TOTALVAL", 1);
    InitControl(m_lblStart, "LBL_START", 1);
    InitControl(m_lblExit, "LBL_EXIT", 1);
    InitControl(m_lblBlack, "LBL_BLACK", 1);
    InitControl(m_lblNextPage, "LBL_NEXTPAGE", 1);
    InitControl(m_lblRAnalog, "LBL_RANALOG", 1);
    InitControl(m_lblName, "LBL_NAME", 1);
    InitControl(m_lblInfoBackground, "LBL_INFOBG", 1);
    StopLoadFromLayout();

    CSWGuiLabelHilight* pProto = (CSWGuiLabelHilight*)m_lbAbility.m_pProtoItem;
    pProto->m_cHilite.m_cParams.m_vColor = CGuiInGame::COLOR_YELLOW;
    pProto->m_cHilite.m_cParams.SetPulsingAlpha(1, 1, 0);
    int nWidth = m_lbAbility.GetItemWidth();
    for (int i = 0; i < 10; i++)
    {
        m_aSkillEntries[i].Initialize(pProto->m_cText, pProto->m_cBorder,
                                      pProto->m_cHilite, nWidth);
        m_aSkillEntries[i].AddEvent(
            CSWGuiControl::EVENT_ENTER, this,
            (CSWGuiHandler)&CSWGuiInGameAbilities::OnEnterSkill);
    }

    m_lblDescription.Initialize(m_lbDescription.m_pProtoItem->m_cExtent,
                                (CSWGuiLabel*)m_lbDescription.m_pProtoItem,
                                1.0f);

    m_pCreature = NULL;
}
#else
CSWGuiInGameAbilities::CSWGuiInGameAbilities(CSWGuiManager* pManager)  // PC 0x00737ED0
    : CSWGuiPanel(pManager)
{
    CExoString sName;

    StartLoadFromLayout("abilities_p");
    InitControl(m_lbDescription, "LB_DESC", 1);
    m_lbDescription.m_nSelectSound = CSWGuiManager::GUISOUND_SELECTION;
    InitControl(m_lbAbility, "LB_ABILITY", 1);
    InitControl(m_lblBonus, "LBL_BONUS", 1);
    InitControl(m_lblBonusValue, "LBL_BONUSVAL", 1);
    InitControl(m_lblSkillRank, "LBL_SKILLRANK", 1);
    InitControl(m_lblRankValue, "LBL_RANKVAL", 1);
    InitControl(m_lblTotal, "LBL_TOTAL", 1);
    InitControl(m_lblTotalValue, "LBL_TOTALVAL", 1);
    InitControl(m_lblName, "LBL_NAME", 1);
    InitControl(m_lblInfoBackground, "LBL_INFOBG", 1);
    InitControl(m_btnFeats, "BTN_FEATS", 1);
    InitControl(m_btnPowers, "BTN_POWERS", 1);
    InitControl(m_btnSkills, "BTN_SKILLS", 1);
    InitControl(m_btnExit, "BTN_EXIT", 1);
    InitControl(m_alblBar[0], "LBL_BAR1", 1);
    InitControl(m_alblBar[1], "LBL_BAR2", 1);
    InitControl(m_alblBar[2], "LBL_BAR3", 1);
    InitControl(m_alblBar[3], "LBL_BAR4", 1);
    InitControl(m_alblBar[4], "LBL_BAR5", 1);
    InitControl(m_alblBar[5], "LBL_BAR6", 1);
    InitControl(m_lblFilter, "LBL_FILTER", 1);
    InitControl(m_lblAbilities, "LBL_ABILITIES", 1);
    InitControl(m_lbDescFeats, "LB_DESC_FEATS", 1);
    StopLoadFromLayout();

    CSWGuiLabelHilight* pProto = (CSWGuiLabelHilight*)m_lbAbility.m_pProtoItem;
    pProto->m_cHilite.m_cParams.SetPulsingAlpha(1, 1, 0);
    int nWidth = m_lbAbility.GetItemWidth();
    for (int i = 0; i < 10; i++)
    {
        m_aSkillEntries[i].Initialize(pProto->m_cText, pProto->m_cBorder,
                                      pProto->m_cHilite, nWidth);
        m_aSkillEntries[i].AddEvent(
            CSWGuiControl::EVENT_ENTER, this,
            (CSWGuiHandler)&CSWGuiInGameAbilities::OnEnterSkill);
    }

    m_lblDescription.Initialize(m_lbDescription.m_pProtoItem->m_cExtent,
                                (CSWGuiLabel*)m_lbDescription.m_pProtoItem,
                                1.0f);
    m_lbAbility.AddEvent(
        CSWGuiListBox::EVENT_SELECTION_CHANGED, this,
        (CSWGuiHandler)&CSWGuiInGameAbilities::OnAbilitySelectionChanged);
    m_lbAbility.SetSelectOnLeftClick(1);

    m_btnFeats.AddEvent(CSWGuiControl::EVENT_SELECT, this,
                        (CSWGuiHandler)&CSWGuiInGameAbilities::OnFeatsButtonPressed);
    m_btnFeats.AddEvent(CSWGuiControl::EVENT_EXIT, this,
                        (CSWGuiHandler)&CSWGuiInGameAbilities::SetButtonColor);
    m_btnPowers.AddEvent(CSWGuiControl::EVENT_SELECT, this,
                         (CSWGuiHandler)&CSWGuiInGameAbilities::OnPowersButtonPressed);
    m_btnPowers.AddEvent(CSWGuiControl::EVENT_EXIT, this,
                         (CSWGuiHandler)&CSWGuiInGameAbilities::SetButtonColor);
    m_btnSkills.AddEvent(CSWGuiControl::EVENT_SELECT, this,
                         (CSWGuiHandler)&CSWGuiInGameAbilities::OnSkillsButtonPressed);
    m_btnSkills.AddEvent(CSWGuiControl::EVENT_EXIT, this,
                         (CSWGuiHandler)&CSWGuiInGameAbilities::SetButtonColor);
    m_btnExit.AddEvent(CSWGuiControl::EVENT_SELECT, this,
                       (CSWGuiHandler)&CSWGuiPanel::OnBButtonPressed);

    m_btnFeats.SetFocusable(0);
    m_btnPowers.SetFocusable(0);
    m_btnSkills.SetFocusable(0);
    m_btnExit.SetFocusable(0);

    m_pCreature = NULL;
}
#endif
// PC 0x00736C80
CSWGuiInGameAbilities::~CSWGuiInGameAbilities()
{
    m_lbDescription.ClearItems();
#ifndef _XBOX
    m_lbDescFeats.ClearItems();
#endif
    m_lbAbility.ClearItems();
}
// PC 0x00737300
void CSWGuiInGameAbilities::OnEnterPower(unsigned long nPower)
{
    CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(nPower);
    if (pSpell == NULL)
    {
        return;
    }
    char cGoodEvil = pSpell->m_cGoodEvil;
    unsigned char nAlignment =
        m_pCreature->GetServerCreature()->GetStats()->m_nGoodEvilAlignment / 10;
    CExoString sColumn;
    CExoString sBonus;
    CExoString sBonusValue;
    Vector vNeutral = CGuiInGame::COLOR_NEW_TEXT;
    Vector vColor;

    if (cGoodEvil == 'E')
    {
        sColumn = "EvilCost";
    }
    else if (cGoodEvil == 'G')
    {
        sColumn = "GoodCost";
    }

    int nRoomForceRating = 0;
    CSWSArea* pArea = m_pCreature->GetServerCreature()->GetArea();
    if (pArea != NULL)
    {
        nRoomForceRating = pArea->GetRoomForceRating(
            m_pCreature->GetServerCreature()->m_vPosition);
    }
    char nCharismaModifier =
        m_pCreature->GetServerCreature()->m_pStats->m_nCharismaModifier;
    int nForm = m_pCreature->GetServerCreature()->GetForm();
    int nCost = pSpell->GetAdjustedForcePoints(
        nAlignment * 10, 1, nRoomForceRating, nForm, nCharismaModifier);
    int nAdjust = nCost - pSpell->GetForcePoints();

    if (nAdjust < 0)
    {
        sBonusValue.Format("%d", nAdjust);
        vColor = CGuiInGame::COLOR_NEW_BONUS;
        sBonus = g_pTlkTable->GetSimpleString(STRREF_ADJUSTMENT);
    }
    else if (nAdjust > 0)
    {
        sBonusValue.Format("+%d", nAdjust);
        vColor = CGuiInGame::COLOR_RED;
        sBonus = g_pTlkTable->GetSimpleString(STRREF_ADJUSTMENT);
    }
    else
    {
        sBonusValue.Format("%d", nAdjust);
        vColor = vNeutral;
        sBonus = g_pTlkTable->GetSimpleString(STRREF_ADJUSTMENT);
    }

    m_lblBonusValue.m_cText.m_cParams.SetText(sBonusValue);
    m_lblBonusValue.m_cText.m_cParams.SetColor(vColor);
    m_lblBonus.m_cText.m_cParams.SetText(sBonus);
    m_lblRankValue.m_cText.m_cParams.SetText(pSpell->GetForcePoints());
    m_lblTotalValue.m_cText.m_cParams.SetText(
        CExoString(pSpell->GetForcePoints() + nAdjust));
    if ((nPower == CSWSpell::FORCE_POWER_FORCE_CRUSH ||
         nPower == CSWSpell::FORCE_POWER_PRECOGNITION ||
         nPower == CSWSpell::FORCE_POWER_BATTLE_PRECOGNITION ||
         nPower == CSWSpell::FORCE_POWER_FORCE_ENLIGHTENMENT ||
         nPower == CSWSpell::FORCE_POWER_BEAST_TRICK ||
         nPower == CSWSpell::FORCE_POWER_BREATH_CONTROL ||
         nPower == CSWSpell::FORCE_POWER_FORCE_SIGHT) &&
        !m_pCreature->GetServerCreature()->m_pStats->HasSpell(0, nPower, 0))
    {
        m_lblName.m_cText.m_cParams.SetStrRef(STRREF_UNKNOWN_FORCE_POWER);
        SetDescription(STRREF_UNKNOWN_FORCE_POWER_DESC);
        m_lblBonusValue.m_cText.m_cParams.SetText("--");
        m_lblBonusValue.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_NEW_TEXT);
        m_lblRankValue.m_cText.m_cParams.SetText("--");
        m_lblTotalValue.m_cText.m_cParams.SetText("--");
    }
    else
    {
        m_lblName.m_cText.m_cParams.SetStrRef(pSpell->GetNameStrRef());
        SetDescription(pSpell->GetDescriptionStrRef());
    }
}
// PC 0x007365B0
void CSWGuiInGameAbilities::CreatePowerList()
{
    if (m_pCreature != NULL)
    {
        CSWSCreature* pServerCreature = m_pCreature->GetServerCreature();
        m_cPowerChart.CreatePowerChart(pServerCreature->m_pStats, 0);
        m_cPowerChart.SetSkillStatusAll(CSWGuiSkillFlow::STATUS_YELLOW);
        m_cPowerChart.SetTopLeft();
    }
}

// PC 0x007365F0
void CSWGuiInGameAbilities::UpdateDescriptionListBox(int bShow)
{
#ifdef _XBOX
    CSWGuiListBox* pListBox = &m_lbDescription;
#else
    CSWGuiListBox* pListBox =
        g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nAbilityMode == 2
            ? &m_lbDescFeats
            : &m_lbDescription;
#endif
    pListBox->ClearItems();

    CSWGuiExtent cExtent = m_lblDescription.m_cExtent;
    if (bShow != 0)
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
#ifdef _XBOX
    m_lblRAnalog.SetEnabledFlag(m_lbDescription.IsOversized());
#endif
}
// PC 0x007366A0
void CSWGuiInGameAbilities::SetDescription(unsigned long nStrRef)
{
    STR_RES strDescription;

    g_pTlkTable->Fetch(nStrRef, strDescription, CTlkTable::GENDER_MALE);
    m_lblDescription.m_cText.m_cParams.SetText(strDescription.m_sString);
    UpdateDescriptionListBox(strDescription.m_sString.GetLength() > 0);
}

int CSWGuiInGameAbilities::DisplayPowers()
{
    unsigned char nClass = m_pCreature->m_pStats->GetClass(
        m_pCreature->m_pStats->GetNumClasses() - 1);
    return CSWClass::IsJedi((int)nClass);
}

#ifndef _XBOX
void CSWGuiInGameAbilities::SetButtonColor(CSWGuiControl* pControl)  // PC 0x00736730
{
    switch (g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nAbilityMode)
    {
    case 0:
        m_btnSkills.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_NEW_HIGHLIGHT);
        break;
    case 1:
        m_btnPowers.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_NEW_HIGHLIGHT);
        break;
    case 2:
        m_btnFeats.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_NEW_HIGHLIGHT);
        break;
    }
}
#endif
// PC 0x00736920
void CSWGuiInGameSkillEntry::Initialize(CSWGuiText& cText,
                                        CSWGuiBorder& cBorder,
                                        CSWGuiBorder& cHilite, int nWidth)
{
    m_cText.m_cParams = cText.m_cParams;
    CSWGuiBorderParams* pParams = &CSWGuiLabel::m_cBorder.m_cParams;
    *pParams = cBorder.m_cParams;
    pParams->m_vColor = CGuiInGame::COLOR_NEW_GREEN;
    pParams = &CSWGuiLabelHilight::m_cHilite.m_cParams;
    *pParams = cHilite.m_cParams;
    pParams->SetPulsingAlpha(1, 1, 0);
    pParams = &m_cNormal.m_cParams;
    {
#ifndef _XBOX
        CResRef cFill("UIBIT_EQP_ITM1"); CResRef cEdge(""); CResRef cCorner("");
        *pParams = CSWGuiBorderParams(cCorner, cEdge, cFill, 0, 0, 1.0f, Vector(1.0f, 1.0f, 1.0f), 0);
#else
        *pParams = CSWGuiBorderParams("", "", "lbl_hex_3", 0, 0, 1.0f, Vector(1.0f, 1.0f, 1.0f), 0);
#endif
        pParams->m_vColor = CGuiInGame::COLOR_NEW_GREEN;
    }
    pParams = &m_cHilite.m_cParams;
    {
#ifndef _XBOX
        CResRef cFill("UIBIT_EQP_ITM1"); CResRef cEdge(""); CResRef cCorner("");
        *pParams = CSWGuiBorderParams(cCorner, cEdge, cFill, 0, 0, 1.0f, Vector(1.0f, 1.0f, 1.0f), 0);
#else
        *pParams = CSWGuiBorderParams("", "", "lbl_hex_3", 0, 0, 1.0f, Vector(1.0f, 1.0f, 1.0f), 0);
#endif
        pParams->SetPulsingAlpha(1, 0, 1);
    }
    {
        CSWGuiExtent cExtent; cExtent.m_nLeft = 0; cExtent.m_nTop = 0; cExtent.m_nWidth = nWidth;
#ifndef _XBOX
        cExtent.m_nHeight = 42;
#else
        cExtent.m_nHeight = 56;
#endif
        SetExtent(cExtent);
    }
}
// PC 0x00736AF0
void CSWGuiInGameSkillEntry::SetSkill(unsigned long nSkill,
                                      unsigned char nState)
{
    CResRef cIcon("inone");
    unsigned long nStrRef = 0;

    switch (nState)
    {
    case 0:
    {
        CSWSkill* pSkill = &g_pRules->m_pSkills[(unsigned short)nSkill];
        if (pSkill != NULL)
        {
            CResRef* pTemp = &(cIcon = pSkill->m_sIcon);
            nStrRef = pSkill->m_nNameStrRef;
        }
        break;
    }
    case 1:
    {
        CSWFeat* pFeat = g_pRules->GetFeat(nSkill);
        if (pFeat != NULL)
        {
            CResRef* pTemp = &(cIcon = pFeat->m_cIcon);
            nStrRef = pFeat->m_nNameStrRef;
        }
        break;
    }
    case 2:
    {
        CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(nSkill);
        if (pSpell != NULL)
        {
            CResRef* pTemp = &(cIcon = pSpell->m_resIcon);
            nStrRef = pSpell->m_nNameStrRef;
        }
        break;
    }
    }

    CSWGuiBorderParams* pParams = &m_cSelection.m_cParams;
    pParams->m_nFillMode = 1;
    pParams->SetFillImage(cIcon, 0);
    m_cText.m_cParams.SetStrRef(nStrRef);
}
// PC 0x00736E90
void CSWGuiInGameAbilities::OnEnterSkill(CSWGuiControl* pControl)
{
    m_nSelectedSkill = pControl->GetID();
    int nRank = m_lstSkills[m_nSelectedSkill].m_nRank;
    CSWSkill* pSkill = m_lstSkills[m_nSelectedSkill].m_pSkill;
    int nBonus = m_lstSkills[m_nSelectedSkill].m_nBonus;
    CExoString sBonus;
    Vector vColor;

    if (nBonus > 0)
    {
        sBonus = CExoString("+") + nBonus;
        vColor = CGuiInGame::COLOR_NEW_BONUS;
    }
    else if (nBonus < 0)
    {
        sBonus = nBonus;
        vColor = CGuiInGame::COLOR_RED;
    }
    else
    {
        sBonus = nBonus;
        vColor = CGuiInGame::COLOR_NEW_TEXT;
    }

    m_lblRankValue.m_cText.m_cParams.SetText(nRank);
    m_lblBonusValue.m_cText.m_cParams.SetText(sBonus);
    m_lblBonusValue.m_cText.m_cParams.SetColor(vColor);
    m_lblTotalValue.m_cText.m_cParams.SetText(
        nRank + nBonus);
    int nNameStrRef = pSkill->m_nNameStrRef;
    m_lblName.m_cText.m_cParams.SetStrRef(nNameStrRef);
    SetDescription(pSkill->GetDescriptionText());
}
// PC 0x00737110
void CSWGuiInGameAbilities::OnEnterFeat(unsigned short nFeat)
{
    SWGUIINGAMEABILITIES_TRACE();
    CSWFeat* pFeat = g_pRules->GetFeat(nFeat);

    SetDescription(pFeat->GetDescriptionText());
    m_lblName.m_cText.m_cParams.SetStrRef(pFeat->m_nNameStrRef);
}

void CSWGuiInGameAbilities::SetDescription(const CExoString& sText)
{
    m_lblDescription.m_cText.m_cParams.SetText(sText);
    UpdateDescriptionListBox(sText.GetLength() > 0);
}

#ifndef _XBOX
void CSWGuiInGameAbilities::OnAbilitySelectionChanged(CSWGuiControl* pControl)  // PC 0x007377A0
{
    CSWGuiSkillFlow* pFlow =
        static_cast<CSWGuiSkillFlow*>(m_lbAbility.GetSelectedControl());

    if (g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nAbilityMode == 1)
    {
#ifndef _XBOX
        unsigned int nPower = pFlow->SkillHitCheckMouse(
            m_lbAbility.GetMouseX(), m_lbAbility.GetMouseY());
#endif
#ifndef _XBOX
        unsigned short nId = nPower;
#endif

#ifndef _XBOX
        m_cPowerChart.SetSelectedSkill(nPower);
#endif
#ifndef _XBOX
        OnEnterPower(nId);
#endif
    }
    else if (g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nAbilityMode ==
             2)
    {
#ifndef _XBOX
        unsigned int nFeat = pFlow->SkillHitCheckMouse(m_lbAbility.GetMouseX(),
                                                       m_lbAbility.GetMouseY());
#endif
#ifndef _XBOX
        unsigned short nId = nFeat;
#endif

#ifndef _XBOX
        m_cFeatChart.SetSelectedSkill(nFeat);
#endif
#ifndef _XBOX
        OnEnterFeat(nId);
#endif
    }
}
#endif
// PC 0x00737850
void CSWGuiInGameAbilities::UpdateView()
{
    CExoArrayList<CSWGuiControl*> lstControls;

    SetDescription("");

    switch (g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nAbilityMode)
    {
    case 2:
#ifdef _XBOX
        m_lblSkills.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
#else
        m_btnSkills.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_NEW_TEXT);
#endif
#ifdef _XBOX
        m_lblFeats.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_YELLOW);
#else
        m_btnFeats.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_NEW_HIGHLIGHT);
#endif
#ifdef _XBOX
        m_lblPowers.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
#else
        m_btnPowers.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_NEW_TEXT);
#endif
#ifdef _XBOX
        m_lblNextPage.m_cText.m_cParams.SetText(m_lblSkills.m_cText.m_cParams.m_sText);
#endif
        m_lblInfoBackground.SetEnabledFlag(0);
        m_lblSkillRank.SetEnabledFlag(0);
        m_lblRankValue.SetEnabledFlag(0);
        m_lblBonus.SetEnabledFlag(0);
        m_lblBonusValue.SetEnabledFlag(0);
        m_lblTotal.SetEnabledFlag(0);
        m_lblTotalValue.SetEnabledFlag(0);
#ifndef _XBOX
        m_lbDescFeats.SetEnabledFlag(1);
#endif
        m_lbDescription.SetEnabledFlag(0);
        m_lbAbility.AddControls(
            *reinterpret_cast<CExoArrayList<CSWGuiControl*>*>(&m_cFeatChart.m_cFlows),
            0, 0, 0);
        {
            unsigned char nRow = m_cFeatChart.m_nSelectedRow;
            m_lbAbility.SetSelectedControl(nRow, 0);
        }
        OnEnterFeat(m_cFeatChart.GetSelected());
        g_pAppManager->m_pClientExoApp->ShowTutorialWindow(0x25, OBJECT_INVALID,
                                                           OBJECT_INVALID, 0);
        break;
    case 1:
#ifdef _XBOX
        m_lblSkills.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
#else
        m_btnSkills.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_NEW_TEXT);
#endif
#ifdef _XBOX
        m_lblFeats.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
#else
        m_btnFeats.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_NEW_TEXT);
#endif
#ifdef _XBOX
        m_lblPowers.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_YELLOW);
#else
        m_btnPowers.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_NEW_HIGHLIGHT);
#endif
#ifdef _XBOX
        m_lblNextPage.m_cText.m_cParams.SetText(m_lblFeats.m_cText.m_cParams.m_sText);
#endif
        m_lblName.m_cText.m_cParams.SetText("");
        m_lblRankValue.m_cText.m_cParams.SetText("");
        m_lblBonusValue.m_cText.m_cParams.SetText("");
        m_lblTotalValue.m_cText.m_cParams.SetText("");
        m_lblSkillRank.m_cText.m_cParams.SetStrRef(STRREF_BASE_COST);
        m_lblTotal.m_cText.m_cParams.SetStrRef(STRREF_COST_PER_USE);
        m_lblInfoBackground.SetEnabledFlag(1);
        m_lblSkillRank.SetEnabledFlag(1);
        m_lblRankValue.SetEnabledFlag(1);
        m_lblBonus.SetEnabledFlag(1);
        m_lblBonusValue.SetEnabledFlag(1);
        m_lblTotal.SetEnabledFlag(1);
        m_lblTotalValue.SetEnabledFlag(1);
#ifndef _XBOX
        m_lbDescFeats.SetEnabledFlag(0);
#endif
        m_lbDescription.SetEnabledFlag(1);
        m_lbAbility.AddControls(
            *reinterpret_cast<CExoArrayList<CSWGuiControl*>*>(&m_cPowerChart.m_cFlows),
            0, 0, 0);
        {
            unsigned char nRow = m_cPowerChart.m_nSelectedRow;
            m_lbAbility.SetSelectedControl(nRow, 0);
        }
        OnEnterPower(m_cPowerChart.GetSelected());
        g_pAppManager->m_pClientExoApp->ShowTutorialWindow(0x26, OBJECT_INVALID,
                                                           OBJECT_INVALID, 0);
        break;
    case 0:
#ifdef _XBOX
        m_lblSkills.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_YELLOW);
#else
        m_btnSkills.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_NEW_HIGHLIGHT);
#endif
#ifdef _XBOX
        m_lblFeats.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
#else
        m_btnFeats.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_NEW_TEXT);
#endif
#ifdef _XBOX
        m_lblPowers.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
#else
        m_btnPowers.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_NEW_TEXT);
#endif
#ifdef _XBOX
        if (DisplayPowers())
            m_lblNextPage.m_cText.m_cParams.SetText(m_lblPowers.m_cText.m_cParams.m_sText);
        else
            m_lblNextPage.m_cText.m_cParams.SetText(m_lblFeats.m_cText.m_cParams.m_sText);
#endif
        m_lblSkillRank.m_cText.m_cParams.SetStrRef(STRREF_SKILL_RANK);
        m_lblBonus.m_cText.m_cParams.SetStrRef(STRREF_BONUS);
        m_lblTotal.m_cText.m_cParams.SetStrRef(STRREF_TOTAL_RANK);
        m_lblInfoBackground.SetEnabledFlag(1);
        m_lblSkillRank.SetEnabledFlag(1);
        m_lblRankValue.SetEnabledFlag(1);
        m_lblBonus.SetEnabledFlag(1);
        m_lblBonusValue.SetEnabledFlag(1);
        m_lblTotal.SetEnabledFlag(1);
        m_lblTotalValue.SetEnabledFlag(1);
#ifndef _XBOX
        m_lbDescFeats.SetEnabledFlag(0);
#endif
        m_lbDescription.SetEnabledFlag(1);
        {
            int nCount = m_lstSkills.num;
            if (nCount >= 10)
            {
                nCount = 10;
            }
            for (int i = 0; i < nCount; i++)
            {
                m_aSkillEntries[i].SetActive(0);
                lstControls.Add(&m_aSkillEntries[i]);
            }
        }
        m_lbAbility.AddControls(lstControls, 0, 1, 0);
        m_lbAbility.SetSelectedControl(m_nSelectedSkill, 0);
        g_pAppManager->m_pClientExoApp->ShowTutorialWindow(0x24, OBJECT_INVALID,
                                                           OBJECT_INVALID, 0);
        break;
    }

    SetActiveControl(NULL, 0);
    SetActiveControl(&m_lbAbility, 0);
}
// PC 0x00737D30
void CSWGuiInGameAbilities::CreateSkillList()
{
    int nNumSkills = g_pRules->m_nNumSkills;
    CExoArrayList<CSWGuiSkillStruct>& lstSkills = m_lstSkills;
    lstSkills.num = 0;
    CSWSCreature* pCreature = m_pCreature->GetServerCreature();

    int i = 0;
    if (nNumSkills > 0)
    {
        CSWGuiInGameSkillEntry* pEntry = m_aSkillEntries;
        do
        {
            CSWSkill* pSkill = &g_pRules->m_pSkills[(unsigned short)i];
            if (pSkill != NULL)
            {
                CSWGuiSkillStruct cSkill;
                cSkill.m_pSkill = pSkill;
                cSkill.m_nRank = pCreature->m_pStats->GetSkillRank(i, NULL, 1);
                cSkill.m_nBonus =
                    pCreature->m_pStats->GetSkillRank(i, NULL, 0) - cSkill.m_nRank;
                m_lstSkills.Add(cSkill);
                pEntry->SetSkill(i, CSWGuiInGameSkillEntry::ABILITY_SKILL);
                ++pEntry;
            }
            ++i;
        } while (i < nNumSkills);
    }
    m_nSelectedSkill = 0;
}

#ifndef _XBOX
void CSWGuiInGameAbilities::OnFeatsButtonPressed(CSWGuiControl* pControl)
{
    g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nAbilityMode = 2;
    UpdateView();
}
#endif

#ifndef _XBOX
void CSWGuiInGameAbilities::OnPowersButtonPressed(CSWGuiControl* pControl)
{
    g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nAbilityMode = 1;
    UpdateView();
}
#endif

#ifndef _XBOX
void CSWGuiInGameAbilities::OnSkillsButtonPressed(CSWGuiControl* pControl)  // PC 0x00737EA0
{
    g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nAbilityMode = 0;
    UpdateView();
}
#endif

void CSWGuiInGameAbilities::CreateFeatList()
{
    if (m_pCreature != NULL)
    {
        CSWSCreature* pServerCreature = m_pCreature->GetServerCreature();
        m_cFeatChart.CreateFeatChart(pServerCreature->m_pStats);
        m_cFeatChart.SetSkillStatusAll(CSWGuiSkillFlow::STATUS_YELLOW);
        m_cFeatChart.SetTopLeft();
    }
}

#ifdef _XBOX
void CSWGuiInGameAbilities::HandleInputEvent(int nEvent, int nState)
{
    if (nState != 0)
    {
        switch (nEvent)
        {
        case 43:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
            g_pAppManager->m_pClientExoApp
                ->ChangeCharacterToNextLivingPartyMember(0, 0);
            SetCharacter(g_pAppManager->m_pClientExoApp->GetSWParty()->GetCharacter(0));
            break;
        case 41:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
            switch (g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nAbilityMode)
            {
            case 0:
                if (DisplayPowers())
                {
                    g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nAbilityMode = 1;
                }
                else
                {
                    g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nAbilityMode = 2;
                }
                break;
            case 1:
                g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nAbilityMode = 2;
                break;
            case 2:
                g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nAbilityMode = 0;
                break;
            }
            UpdateView();
            break;
        case GUIEVENT_SCROLL_DOWN:
            m_lbDescription.HandleInputEvent(GUIEVENT_DOWN, 1);
            break;
        case GUIEVENT_SCROLL_UP:
            m_lbDescription.HandleInputEvent(GUIEVENT_UP, 1);
            break;
        case GUIEVENT_CANCEL:
        case GUIEVENT_ACCEPT:
        case GUIEVENT_CANCEL_ALTERNATE:
            if (g_pAppManager->m_pClientExoApp->GetInGameGui()->HideSWInGameGui())
            {
                g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME);
            }
            break;
        case GUIEVENT_LEFT:
        case GUIEVENT_RIGHT:
        case 63:
        case 64:
            if (g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nAbilityMode != 0)
            {
                m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
            }
        case GUIEVENT_UP:
        case GUIEVENT_DOWN:
        case GUIEVENT_UP_ALTERNATE:
        case GUIEVENT_DOWN_ALTERNATE:
            if (g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nAbilityMode == 1)
            {
                OnEnterPower(m_cPowerChart.HandleInput(nEvent));
            }
            else if (g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nAbilityMode == 2)
            {
                OnEnterFeat(m_cFeatChart.HandleInput(nEvent));
            }
            break;
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
#else
void CSWGuiInGameAbilities::HandleInputEvent(int nEvent, int nState)  // PC 0x007388E0
{
    if (nState != 0)
    {
        switch (nEvent)
        {
        case GUIEVENT_NEXT_CHARACTER:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
            g_pAppManager->m_pClientExoApp->ChangeCharacterToNextLivingPartyMember(
                0, 0, 1);
            SetCharacter(
                g_pAppManager->m_pClientExoApp->GetSWParty()->GetCharacter(0));
            break;
        case GUIEVENT_RECOMMEND:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
            switch (g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nAbilityMode)
            {
            case 0:
                if (DisplayPowers())
                {
                    g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nAbilityMode = 1;
                }
                else
                {
                    g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nAbilityMode = 2;
                }
                break;
            case 1:
                g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nAbilityMode = 2;
                break;
            case 2:
                g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nAbilityMode = 0;
                break;
            }
            UpdateView();
            break;
        case GUIEVENT_SCROLL_DOWN:
            m_lbDescription.HandleInputEvent(GUIEVENT_DOWN, 1);
            break;
        case GUIEVENT_SCROLL_UP:
            m_lbDescription.HandleInputEvent(GUIEVENT_UP, 1);
            break;
        case GUIEVENT_CANCEL:
        case GUIEVENT_ACCEPT:
        case GUIEVENT_CANCEL_ALTERNATE:
        case 223:
            if (g_pAppManager->m_pClientExoApp->GetInGameGui()->HideSWInGameGui(0))
            {
                g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME, 1);
            }
            break;
        case GUIEVENT_LEFT:
        case GUIEVENT_RIGHT:
        case 63:
        case 64:
            if (g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nAbilityMode != 0)
            {
                m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
            }
        case GUIEVENT_UP:
        case GUIEVENT_DOWN:
        case GUIEVENT_UP_ALTERNATE:
        case GUIEVENT_DOWN_ALTERNATE:
            if (g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nAbilityMode == 1)
            {
                OnEnterPower(m_cPowerChart.HandleInput(nEvent));
            }
            else if (g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nAbilityMode == 2)
            {
                OnEnterFeat(m_cFeatChart.HandleInput(nEvent));
            }
            break;
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
#endif
// PC 0x00738780
void CSWGuiInGameAbilities::SetCharacter(CSWCCreature* pCreature)
{
    m_pCreature = pCreature;
    if (pCreature != NULL)
    {
        m_lbAbility.ClearItems();
        CreateFeatList();
        CreateSkillList();
        unsigned char nClass = m_pCreature->m_pStats->GetClass(
            m_pCreature->m_pStats->GetNumClasses() - 1);
        if (CSWClass::IsJedi((int)nClass) != 0)
        {
            CreatePowerList();
#ifdef _XBOX
            m_lblPowers.SetEnabledFlag(1);
#else
            m_btnPowers.SetEnabledFlag(1);
#endif
            UpdateView();
            return;
        }
        else
        {
#ifdef _XBOX
            m_lblPowers.SetEnabledFlag(0);
#else
            m_btnPowers.SetEnabledFlag(0);
#endif
            if (g_pAppManager->m_pClientExoApp->GetInGameGui()
                    ->m_nAbilityMode == 1)
            {
                g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nAbilityMode =
                    0;
            }
        }
        UpdateView();
    }
}

#ifdef _XBOX
void CSWGuiInGameAbilities::OnPanelAdded()
{
    SetCharacter(g_pAppManager->m_pClientExoApp->GetSWParty()->GetCharacter(0));
    m_lblBlack.SetEnabledFlag(g_pAppManager->m_pClientExoApp->GetSWParty()->GetNumCharacters() > 1);
}
#else
void CSWGuiInGameAbilities::OnPanelAdded()  // PC 0x00738870
{
    SetCharacter(g_pAppManager->m_pClientExoApp->GetSWParty()->GetCharacter(0));
    m_btnExit.HandleMouseOver(0);
    CSWGuiPanel::OnPanelAdded();
}
#endif
#ifndef _XBOX
// PC 0x007388B0
void CSWGuiInGameAbilities::UpdateCharacter()
{
    SetCharacter(g_pAppManager->m_pClientExoApp->GetSWParty()->GetCharacter(0));
}
#endif

static inline void SwguiingameabilitiesTrace2()
{
    SwguiingameabilitiesTrace1();
}

static inline void SwguiingameabilitiesTrace1()
{
}
