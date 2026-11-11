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
#include "swguiingamecharacter.h"

static inline void SwguiingameabilitiesTrace2();
static inline void SwguiingameabilitiesTrace1();
#define SWGUIINGAMEABILITIES_TRACE() SwguiingameabilitiesTrace2()
// PC 0x006AB8B0  Xbox 0x000E2D00
void CSWGuiInGameAbilities::OnPanelRemoved()
{
    m_lbAbility.ClearItems();
    m_cPowerChart.ClearChart();
    m_cFeatChart.ClearChart();
}
// PC 0x006AB8E0  Xbox 0x000E3000
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
        m_cSelection.SetExtent(cIcon);

        cIcon.m_nLeft += 42;
        cIcon.m_nWidth = m_cExtent.m_nWidth - 42;
        CSWGuiLabel::m_cBorder.SetExtent(cIcon);
        CSWGuiLabelHilight::m_cHilite.SetExtent(cIcon);
        m_cText.SetExtent(CSWGuiExtent::Intersection(
            CSWGuiLabel::m_cBorder.GetInnerExtent(),
            CSWGuiLabelHilight::m_cHilite.GetInnerExtent()));
    }
}
// PC 0x006AB9F0  Xbox 0x000E2D30
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

#ifdef _XBOX
CSWGuiInGameAbilities::CSWGuiInGameAbilities(CSWGuiManager* pManager)  // PC 0x006ADDA0  Xbox 0x000E4460
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
CSWGuiInGameAbilities::CSWGuiInGameAbilities(CSWGuiManager* pManager)  // PC 0x006ADDA0  Xbox 0x000E4460
    : CSWGuiPanel(pManager)
{
    CExoString sName;

    m_nCharacterIndex = CHARACTER_PLAYER;
    StartLoadFromLayout("abilities");
    InitControl(m_lblPortrait, "LBL_PORTRAIT", 1);
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
    InitControl(m_btnChange1, "BTN_CHANGE1", 1);
    InitControl(m_btnChange2, "BTN_CHANGE2", 1);
    InitControl(m_btnCharLeft, "BTN_CHARLEFT", 1);
    InitControl(m_btnCharRight, "BTN_CHARRIGHT", 1);
    StopLoadFromLayout();

    m_btnCharLeft.SetFocusable(0);
    m_btnCharLeft.AddEvent(CSWGuiControl::EVENT_SELECT, this,
                           (CSWGuiHandler)&CSWGuiInGameAbilities::OnSwitchLeft);
    m_btnCharRight.SetFocusable(0);
    m_btnCharRight.AddEvent(CSWGuiControl::EVENT_SELECT, this,
                            (CSWGuiHandler)&CSWGuiInGameAbilities::OnSwitchRight);
    m_btnCharRight.m_cBorder.m_cParams.m_fRotation = 180.0f;
    m_btnCharRight.m_cHilite.m_cParams.m_fRotation = 180.0f;

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
    m_btnChange1.AddEvent(CSWGuiControl::EVENT_SELECT, this,
                          (CSWGuiHandler)&CSWGuiInGameAbilities::OnChangeCharacter);
    m_btnChange2.AddEvent(CSWGuiControl::EVENT_SELECT, this,
                          (CSWGuiHandler)&CSWGuiInGameAbilities::OnChangeCharacter);

    m_btnFeats.SetFocusable(0);
    m_btnPowers.SetFocusable(0);
    m_btnSkills.SetFocusable(0);
    m_btnExit.SetFocusable(0);
    m_btnChange1.SetFocusable(0);
    m_btnChange2.SetFocusable(0);

    m_btnChange1.SetToolTip(38693, GUIEVENT_NEXT_CHARACTER);
#ifndef _XBOX
    m_btnChange2.m_nToolTipStrRef = 38693;
#endif

    m_pCreature = NULL;
}
#endif
// PC 0x006ABAF0  Xbox 0x000E3D00
CSWGuiInGameAbilities::~CSWGuiInGameAbilities()
{
    m_lbDescription.ClearItems();
    m_lbAbility.ClearItems();
}
// PC 0x006ACCE0  Xbox 0x000E3420
void CSWGuiInGameAbilities::OnEnterPower(unsigned long nPower)
{
    int nAdjust = 0;
    CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(nPower);
    char cGoodEvil = pSpell->m_cGoodEvil;
    unsigned char nAlignment =
        m_pCreature->GetServerCreature()->GetStats()->m_nGoodEvilAlignment / 10;
    CExoString sColumn;
    CExoString sBonus;
    CExoString sBonusValue;
    CExoString sSide;
    Vector vColor;

    if (cGoodEvil == 'E')
    {
        sColumn = "EvilCost";
    }
    else if (cGoodEvil == 'G')
    {
        sColumn = "GoodCost";
    }

    if (sColumn != "")
    {
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        EXOXBOXASSERT(p2DArrays->m_pForcePointAdjust);
        C2DA* pForceAdjust = p2DArrays->m_pForcePointAdjust;
        float fMultiplier;
        pForceAdjust->GetFLOATEntry(nAlignment, sColumn, &fMultiplier);
        nAdjust = (int)(pSpell->GetForcePoints() * fMultiplier + 0.5f) - pSpell->GetForcePoints();
    }

    if (nAlignment > 5)
    {
        sSide = g_pTlkTable->GetSimpleString(STRREF_LIGHT_SIDE);
    }
    else if (nAlignment < 5)
    {
        sSide = g_pTlkTable->GetSimpleString(STRREF_DARK_SIDE);
    }

    if (nAdjust < 0)
    {
        sBonusValue.Format("%d", nAdjust);
        vColor = CGuiInGame::COLOR_GREEN;
        sBonus = sSide + " " +
                 g_pTlkTable->GetSimpleString(STRREF_ADJUSTMENT);
    }
    else if (nAdjust > 0)
    {
        sBonusValue.Format("+%d", nAdjust);
        vColor = CGuiInGame::COLOR_RED;
        sBonus = sSide + " " +
                 g_pTlkTable->GetSimpleString(STRREF_ADJUSTMENT);
    }
    else
    {
        sBonusValue.Format("%d", nAdjust);
        vColor = CGuiInGame::COLOR_BLUE;
        sBonus = g_pTlkTable->GetSimpleString(STRREF_ADJUSTMENT);
    }

    m_lblBonusValue.m_cText.m_cParams.SetText(sBonusValue);
    m_lblBonusValue.m_cText.m_cParams.SetColor(vColor);
    m_lblBonus.m_cText.m_cParams.SetText(sBonus);
    m_lblRankValue.m_cText.m_cParams.SetText(pSpell->GetForcePoints());
    m_lblTotalValue.m_cText.m_cParams.SetText(
        CExoString(pSpell->GetForcePoints() + nAdjust));
    m_lblName.m_cText.m_cParams.SetStrRef(pSpell->GetNameStrRef());
    SetDescription(pSpell->GetDescriptionStrRef());
}
// PC 0x006ABCE0  Xbox 0x000E2DC0
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

#ifdef _XBOX
// PC 0x0041A290
inline int CSWGuiListBox::IsOversized()
{
    return m_nContentHeight + m_nPadding > m_cItemExtent.m_nHeight &&
           m_lItems.num > 0 && !m_bHideScrollIcon;
}
#endif
// PC 0x006ABD20  Xbox 0x000E2E00
void CSWGuiInGameAbilities::UpdateDescriptionListBox(int bShow)
{
    m_lbDescription.ClearItems();

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
    m_lbDescription.AddControls(&pControl, 1, 0, 0, 0);
    m_lbDescription.SetSelectedControl(&m_lblDescription, 0);
#ifdef _XBOX
    m_lblRAnalog.SetEnabledFlag(m_lbDescription.IsOversized());
#endif
}
// PC 0x006ABDC0  Xbox 0x000E2EF0
void CSWGuiInGameAbilities::SetDescription(unsigned long nStrRef)
{
    STR_RES strDescription;

    g_pTlkTable->Fetch(nStrRef, strDescription, CTlkTable::GENDER_MALE);
    m_lblDescription.m_cText.m_cParams.SetText(strDescription.m_sString);
    UpdateDescriptionListBox(strDescription.m_sString.GetLength() > 0);
}
// PC 0x006ABE70  Xbox 0x000E2FB0
int CSWGuiInGameAbilities::DisplayPowers()
{
    unsigned char nClass = m_pCreature->m_pStats->GetClass(
        m_pCreature->m_pStats->GetNumClasses() - 1);
    return CSWClass::IsJedi((int)nClass) != 0 && m_cPowerChart.m_cFlows.num > 0;
}

#ifndef _XBOX
void CSWGuiInGameAbilities::UpdatePortraits()  // PC 0x006ABEC0
{
    CSWServerInfo* pInfo =
        (CSWServerInfo*)g_pAppManager->m_pServerExoApp->GetServerInfo();
    if (pInfo->m_bPartyPicking)
    {
        int bSecond = 0;
        int bFirst = 0;
        int nIndex = m_btnChange1.m_nIndex;
        char nStart = nIndex;
        if ((char)nIndex == -2)
        {
            nStart = m_nCharacterIndex + 1;
        }
        int nNPC;
        for (nNPC = nStart; nNPC < 9; ++nNPC)
        {
            if (bSecond != 0)
            {
                break;
            }
            if (nNPC != m_nCharacterIndex)
            {
                if (g_pAppManager->m_pServerExoApp->GetPartyTable()
                        ->GetIsNPCAvailable(nNPC) != 0)
                {
                    CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
                    unsigned long oidNPC =
                        g_pAppManager->m_pServerExoApp->GetPartyTable()
                            ->GetNPCObject(nNPC, 0, 1);
                    CSWCCreature* pCreature =
                        pClient->GetCreatureByGameObjectID(
                            pClient->ServerToClientObjectId(oidNPC));
                    if (pCreature != NULL)
                    {
                        if (bFirst != 0)
                        {
                            {
                                CResRef* pPortrait = &pCreature->GetPortrait(1);
                                m_btnChange2.m_cBorder.m_cParams.SetFillImage(
                                    *pPortrait, 0);
                            }
                            {
                                CResRef* pPortrait = &pCreature->GetPortrait(1);
                                m_btnChange2.m_cHilite.m_cParams.SetFillImage(
                                    *pPortrait, 0);
                            }
                            m_btnChange2.m_nIndex = nNPC;
                            bSecond = 1;
                        }
                        else
                        {
                            {
                                CResRef* pPortrait = &pCreature->GetPortrait(1);
                                m_btnChange1.m_cBorder.m_cParams.SetFillImage(
                                    *pPortrait, 0);
                            }
                            {
                                CResRef* pPortrait = &pCreature->GetPortrait(1);
                                m_btnChange1.m_cHilite.m_cParams.SetFillImage(
                                    *pPortrait, 0);
                            }
                            m_btnChange1.m_nIndex = nNPC;
                            bFirst = 1;
                        }
                    }
                }
            }
        }
        if (m_nCharacterIndex != -1 && bSecond == 0)
        {
            if (bFirst == 0)
            {
                {
                    CResRef* pPortrait =
                        &g_pAppManager->m_pClientExoApp->GetPlayerCreature()
                             ->GetPortrait(1);
                    m_btnChange1.m_cBorder.m_cParams.SetFillImage(*pPortrait,
                                                                  0);
                }
                {
                    CResRef* pPortrait =
                        &g_pAppManager->m_pClientExoApp->GetPlayerCreature()
                             ->GetPortrait(1);
                    m_btnChange1.m_cHilite.m_cParams.SetFillImage(*pPortrait,
                                                                  0);
                }
                m_btnChange1.m_nIndex = -1;
                bFirst = 1;
            }
            else
            {
                {
                    CResRef* pPortrait =
                        &g_pAppManager->m_pClientExoApp->GetPlayerCreature()
                             ->GetPortrait(1);
                    m_btnChange2.m_cBorder.m_cParams.SetFillImage(*pPortrait,
                                                                  0);
                }
                {
                    CResRef* pPortrait =
                        &g_pAppManager->m_pClientExoApp->GetPlayerCreature()
                             ->GetPortrait(1);
                    m_btnChange2.m_cHilite.m_cParams.SetFillImage(*pPortrait,
                                                                  0);
                }
                m_btnChange2.m_nIndex = -1;
                bSecond = 1;
            }
        }
        if (bSecond == 0)
        {
            for (int nLow = 0; nLow < nStart; ++nLow)
            {
                if (bSecond != 0)
                {
                    break;
                }
                if (nLow != m_nCharacterIndex)
                {
                    if (g_pAppManager->m_pServerExoApp->GetPartyTable()
                            ->GetIsNPCAvailable(nLow) != 0)
                    {
                        CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
                        unsigned long oidNPC =
                            g_pAppManager->m_pServerExoApp->GetPartyTable()
                                ->GetNPCObject(nLow, 0, 1);
                        CSWCCreature* pCreature =
                            pClient->GetCreatureByGameObjectID(
                                pClient->ServerToClientObjectId(oidNPC));
                        if (pCreature != NULL)
                        {
                            if (bFirst != 0)
                            {
                                {
                                    CResRef* pPortrait =
                                        &pCreature->GetPortrait(1);
                                    m_btnChange2.m_cBorder.m_cParams
                                        .SetFillImage(*pPortrait, 0);
                                }
                                {
                                    CResRef* pPortrait =
                                        &pCreature->GetPortrait(1);
                                    m_btnChange2.m_cHilite.m_cParams
                                        .SetFillImage(*pPortrait, 0);
                                }
                                m_btnChange2.m_nIndex = nLow;
                                bSecond = 1;
                            }
                            else
                            {
                                {
                                    CResRef* pPortrait =
                                        &pCreature->GetPortrait(1);
                                    m_btnChange1.m_cBorder.m_cParams
                                        .SetFillImage(*pPortrait, 0);
                                }
                                {
                                    CResRef* pPortrait =
                                        &pCreature->GetPortrait(1);
                                    m_btnChange1.m_cHilite.m_cParams
                                        .SetFillImage(*pPortrait, 0);
                                }
                                m_btnChange1.m_nIndex = nLow;
                                bFirst = 1;
                            }
                        }
                    }
                }
            }
        }
        m_btnChange1.SetEnabledFlag(bFirst);
        m_btnChange2.SetEnabledFlag(bSecond);
        m_btnCharLeft.SetEnabledFlag(
            g_pAppManager->m_pServerExoApp->GetPartyTable()
                ->GetNumNPCAvailable() > 2);
        m_btnCharRight.SetEnabledFlag(
            g_pAppManager->m_pServerExoApp->GetPartyTable()
                ->GetNumNPCAvailable() > 2);
    }
    else
    {
        CSWParty* pParty = g_pAppManager->m_pClientExoApp->GetSWParty();
        if (pParty->m_nCharacters > 1)
        {
            m_btnChange1.SetEnabledFlag(1);
            {
                CResRef* pPortrait = &pParty->GetCharacter(1)->GetPortrait(1);
                m_btnChange1.m_cBorder.m_cParams.SetFillImage(*pPortrait, 0);
            }
            {
                CResRef* pPortrait = &pParty->GetCharacter(1)->GetPortrait(1);
                m_btnChange1.m_cHilite.m_cParams.SetFillImage(*pPortrait, 0);
            }
        }
        else
        {
            m_btnChange1.SetEnabledFlag(0);
        }
        if (pParty->m_nCharacters > 2)
        {
            m_btnChange2.SetEnabledFlag(1);
            {
                CResRef* pPortrait = &pParty->GetCharacter(2)->GetPortrait(1);
                m_btnChange2.m_cBorder.m_cParams.SetFillImage(*pPortrait, 0);
            }
            {
                CResRef* pPortrait = &pParty->GetCharacter(2)->GetPortrait(1);
                m_btnChange2.m_cHilite.m_cParams.SetFillImage(*pPortrait, 0);
            }
        }
        else
        {
            m_btnChange2.SetEnabledFlag(0);
        }
    }
}
#endif

#ifndef _XBOX
void CSWGuiInGameAbilities::OnSwitchLeft(CSWGuiControl* pControl)  // PC 0x006AC3E0
{
    if (pControl->m_bSelected != 0)
    {
        char nFirst = m_btnChange1.m_nIndex;
        int bFound = 0;
        if (nFirst != -2)
        {
            m_btnChange2.m_cBorder.m_cParams.SetFillImage(
                m_btnChange1.m_cBorder.m_cParams.m_cFillImage, 0);
            m_btnChange2.m_cHilite.m_cParams.SetFillImage(
                m_btnChange1.m_cHilite.m_cParams.m_cFillImage, 0);
            m_btnChange2.m_nIndex = m_btnChange1.m_nIndex;
            int nNPC = nFirst - 1;
            while (nNPC >= 0 && bFound == 0)
            {
                if (nNPC != m_nCharacterIndex)
                {
                    if (g_pAppManager->m_pServerExoApp->GetPartyTable()
                            ->GetIsNPCAvailable(nNPC) != 0)
                    {
                        CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
                        unsigned long oidNPC =
                            g_pAppManager->m_pServerExoApp->GetPartyTable()
                                ->GetNPCObject(nNPC, 0, 1);
                        CSWCCreature* pCreature =
                            pClient->GetCreatureByGameObjectID(
                                pClient->ServerToClientObjectId(oidNPC));
                        if (pCreature != 0)
                        {
                            {
                                CResRef* pPortrait = &pCreature->GetPortrait(1);
                                m_btnChange1.m_cBorder.m_cParams.SetFillImage(
                                    *pPortrait, 0);
                            }
                            {
                                CResRef* pPortrait = &pCreature->GetPortrait(1);
                                m_btnChange1.m_cHilite.m_cParams.SetFillImage(
                                    *pPortrait, 0);
                            }
                            m_btnChange1.m_nIndex = nNPC;
                            bFound = 1;
                        }
                    }
                }
                --nNPC;
            }
            if (bFound == 0)
            {
                if (m_nCharacterIndex == -1 ||
                    (char)m_btnChange2.m_nIndex == -1)
                {
                    for (int nHigh = 8; nHigh > nFirst && bFound == 0; --nHigh)
                    {
                        if (nHigh != m_nCharacterIndex)
                        {
                            if (g_pAppManager->m_pServerExoApp->GetPartyTable()
                                    ->GetIsNPCAvailable(nHigh) != 0)
                            {
                                CClientExoApp* pClient =
                                    g_pAppManager->m_pClientExoApp;
                                unsigned long oidNPC =
                                    g_pAppManager->m_pServerExoApp
                                        ->GetPartyTable()
                                        ->GetNPCObject(nHigh, 0, 1);
                                CSWCCreature* pCreature =
                                    pClient->GetCreatureByGameObjectID(
                                        pClient->ServerToClientObjectId(
                                            oidNPC));
                                if (pCreature != 0)
                                {
                                    {
                                        CResRef* pPortrait =
                                            &pCreature->GetPortrait(1);
                                        m_btnChange1.m_cBorder.m_cParams
                                            .SetFillImage(*pPortrait, 0);
                                    }
                                    {
                                        CResRef* pPortrait =
                                            &pCreature->GetPortrait(1);
                                        m_btnChange1.m_cHilite.m_cParams
                                            .SetFillImage(*pPortrait, 0);
                                    }
                                    m_btnChange1.m_nIndex = nHigh;
                                    bFound = 1;
                                }
                            }
                        }
                    }
                }
                else
                {
                    {
                        CResRef* pPortrait =
                            &g_pAppManager->m_pClientExoApp->GetPlayerCreature()
                                 ->GetPortrait(1);
                        m_btnChange1.m_cBorder.m_cParams.SetFillImage(
                            *pPortrait, 0);
                    }
                    {
                        CResRef* pPortrait =
                            &g_pAppManager->m_pClientExoApp->GetPlayerCreature()
                                 ->GetPortrait(1);
                        m_btnChange1.m_cHilite.m_cParams.SetFillImage(
                            *pPortrait, 0);
                    }
                    m_btnChange1.m_nIndex = -1;
                }
            }
        }
    }
}
#endif

#ifndef _XBOX
void CSWGuiInGameAbilities::OnSwitchRight(CSWGuiControl* pControl)  // PC 0x006AC660
{
    if (pControl->m_bSelected != 0)
    {
        char nSecond = m_btnChange2.m_nIndex;
        int bFound = 0;
        if (nSecond != -2)
        {
            m_btnChange1.m_cBorder.m_cParams.SetFillImage(
                m_btnChange2.m_cBorder.m_cParams.m_cFillImage, 0);
            m_btnChange1.m_cHilite.m_cParams.SetFillImage(
                m_btnChange2.m_cHilite.m_cParams.m_cFillImage, 0);
            m_btnChange1.m_nIndex = m_btnChange2.m_nIndex;
            int nNPC = nSecond + 1;
            while (nNPC < 9 && bFound == 0)
            {
                if (nNPC != m_nCharacterIndex)
                {
                    if (g_pAppManager->m_pServerExoApp->GetPartyTable()
                            ->GetIsNPCAvailable(nNPC) != 0)
                    {
                        CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
                        unsigned long oidNPC =
                            g_pAppManager->m_pServerExoApp->GetPartyTable()
                                ->GetNPCObject(nNPC, 0, 1);
                        CSWCCreature* pCreature =
                            pClient->GetCreatureByGameObjectID(
                                pClient->ServerToClientObjectId(oidNPC));
                        if (pCreature != 0)
                        {
                            {
                                CResRef* pPortrait = &pCreature->GetPortrait(1);
                                m_btnChange2.m_cBorder.m_cParams.SetFillImage(
                                    *pPortrait, 0);
                            }
                            {
                                CResRef* pPortrait = &pCreature->GetPortrait(1);
                                m_btnChange2.m_cHilite.m_cParams.SetFillImage(
                                    *pPortrait, 0);
                            }
                            m_btnChange2.m_nIndex = nNPC;
                            bFound = 1;
                        }
                    }
                }
                ++nNPC;
            }
            if (bFound == 0)
            {
                if (m_nCharacterIndex != -1)
                {
                    {
                        CResRef* pPortrait =
                            &g_pAppManager->m_pClientExoApp->GetPlayerCreature()
                                 ->GetPortrait(1);
                        m_btnChange2.m_cBorder.m_cParams.SetFillImage(
                            *pPortrait, 0);
                    }
                    {
                        CResRef* pPortrait =
                            &g_pAppManager->m_pClientExoApp->GetPlayerCreature()
                                 ->GetPortrait(1);
                        m_btnChange2.m_cHilite.m_cParams.SetFillImage(
                            *pPortrait, 0);
                    }
                    m_btnChange2.m_nIndex = -1;
                }
                else
                {
                    for (int nLow = 0; nLow < nSecond && bFound == 0; ++nLow)
                    {
                        if (nLow != m_nCharacterIndex)
                        {
                            if (g_pAppManager->m_pServerExoApp->GetPartyTable()
                                    ->GetIsNPCAvailable(nLow) != 0)
                            {
                                CClientExoApp* pClient =
                                    g_pAppManager->m_pClientExoApp;
                                unsigned long oidNPC =
                                    g_pAppManager->m_pServerExoApp
                                        ->GetPartyTable()
                                        ->GetNPCObject(nLow, 0, 1);
                                CSWCCreature* pCreature =
                                    pClient->GetCreatureByGameObjectID(
                                        pClient->ServerToClientObjectId(
                                            oidNPC));
                                if (pCreature != 0)
                                {
                                    {
                                        CResRef* pPortrait =
                                            &pCreature->GetPortrait(1);
                                        m_btnChange2.m_cBorder.m_cParams
                                            .SetFillImage(*pPortrait, 0);
                                    }
                                    {
                                        CResRef* pPortrait =
                                            &pCreature->GetPortrait(1);
                                        m_btnChange2.m_cHilite.m_cParams
                                            .SetFillImage(*pPortrait, 0);
                                    }
                                    m_btnChange2.m_nIndex = nLow;
                                    bFound = 1;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
#endif

#ifndef _XBOX
void CSWGuiInGameAbilities::SetButtonColor(CSWGuiControl* pControl)  // PC 0x006AC8D0
{
    switch (g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nAbilityMode)
    {
    case 0:
        m_btnSkills.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_YELLOW);
        break;
    case 1:
        m_btnPowers.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_YELLOW);
        break;
    case 2:
        m_btnFeats.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_YELLOW);
        break;
    }
}
#endif
// PC 0x006AC930  Xbox 0x000E3110
void CSWGuiInGameSkillEntry::Initialize(CSWGuiText& cText,
                                        CSWGuiBorder& cBorder,
                                        CSWGuiBorder& cHilite, int nWidth)
{
    m_cText.m_cParams = cText.m_cParams;
    CSWGuiBorderParams* pParams = &CSWGuiLabel::m_cBorder.m_cParams;
    *pParams = cBorder.m_cParams;
    pParams->m_vColor = CGuiInGame::COLOR_BLUE;
    pParams = &CSWGuiLabelHilight::m_cHilite.m_cParams;
    *pParams = cHilite.m_cParams;
    pParams->m_vColor = CGuiInGame::COLOR_YELLOW;
    pParams->SetPulsingAlpha(1, 1, 0);
    pParams = &m_cNormal.m_cParams;
    {
        CResRef cFill("lbl_hex_3");
        CResRef cEdge("");
        CResRef cCorner("");

        *pParams = CSWGuiBorderParams(cCorner, cEdge, cFill, 0, 0, 1.0f,
                                      Vector(1.0f, 1.0f, 1.0f), 0);
        pParams->m_vColor = CGuiInGame::COLOR_BLUE;
    }
    pParams = &m_cHilite.m_cParams;
    {
        CResRef cFill("lbl_hex_3");
        CResRef cEdge("");
        CResRef cCorner("");

        *pParams = CSWGuiBorderParams(cCorner, cEdge, cFill, 0, 0, 1.0f,
                                      Vector(1.0f, 1.0f, 1.0f), 0);
        pParams->m_vColor = CGuiInGame::COLOR_YELLOW;
        pParams->SetPulsingAlpha(1, 0, 1);
    }

    {
        CSWGuiExtent cExtent;
        cExtent.m_nLeft = 0;
        cExtent.m_nTop = 0;
        cExtent.m_nWidth = nWidth;
        cExtent.m_nHeight = 42;
        SetExtent(cExtent);
    }
}
// PC 0x006ACB40  Xbox 0x000E3320
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
// PC 0x006AD180  Xbox 0x000E38D0
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
        vColor = CGuiInGame::COLOR_GREEN;
    }
    else if (nBonus < 0)
    {
        sBonus = nBonus;
        vColor = CGuiInGame::COLOR_RED;
    }
    else
    {
        sBonus = nBonus;
        vColor = CGuiInGame::COLOR_BLUE;
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
// PC 0x006AD410  Xbox 0x000E3B60
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
void CSWGuiInGameAbilities::OnAbilitySelectionChanged(CSWGuiControl* pControl)  // PC 0x006AD4B0
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
// PC 0x006AD560  Xbox 0x000E3F20
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
        m_btnSkills.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
#endif
#ifdef _XBOX
        m_lblFeats.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_YELLOW);
#else
        m_btnFeats.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_YELLOW);
#endif
#ifdef _XBOX
        m_lblPowers.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
#else
        m_btnPowers.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
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
        m_btnSkills.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
#endif
#ifdef _XBOX
        m_lblFeats.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
#else
        m_btnFeats.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
#endif
#ifdef _XBOX
        m_lblPowers.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_YELLOW);
#else
        m_btnPowers.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_YELLOW);
#endif
#ifdef _XBOX
        m_lblNextPage.m_cText.m_cParams.SetText(m_lblFeats.m_cText.m_cParams.m_sText);
#endif
        m_lblSkillRank.m_cText.m_cParams.SetStrRef(37655);
        m_lblTotal.m_cText.m_cParams.SetStrRef(41905);
        m_lblInfoBackground.SetEnabledFlag(1);
        m_lblSkillRank.SetEnabledFlag(1);
        m_lblRankValue.SetEnabledFlag(1);
        m_lblBonus.SetEnabledFlag(1);
        m_lblBonusValue.SetEnabledFlag(1);
        m_lblTotal.SetEnabledFlag(1);
        m_lblTotalValue.SetEnabledFlag(1);
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
        m_btnSkills.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_YELLOW);
#endif
#ifdef _XBOX
        m_lblFeats.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
#else
        m_btnFeats.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
#endif
#ifdef _XBOX
        m_lblPowers.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
#else
        m_btnPowers.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
#endif
#ifdef _XBOX
        if (DisplayPowers())
            m_lblNextPage.m_cText.m_cParams.SetText(m_lblPowers.m_cText.m_cParams.m_sText);
        else
            m_lblNextPage.m_cText.m_cParams.SetText(m_lblFeats.m_cText.m_cParams.m_sText);
#endif
        m_lblSkillRank.m_cText.m_cParams.SetStrRef(1579);
        m_lblBonus.m_cText.m_cParams.SetStrRef(32129);
        m_lblTotal.m_cText.m_cParams.SetStrRef(41904);
        m_lblInfoBackground.SetEnabledFlag(1);
        m_lblSkillRank.SetEnabledFlag(1);
        m_lblRankValue.SetEnabledFlag(1);
        m_lblBonus.SetEnabledFlag(1);
        m_lblBonusValue.SetEnabledFlag(1);
        m_lblTotal.SetEnabledFlag(1);
        m_lblTotalValue.SetEnabledFlag(1);
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
// PC 0x006AD960  Xbox 0x000E4350
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
void CSWGuiInGameAbilities::OnFeatsButtonPressed(CSWGuiControl* pControl)  // PC 0x006ADA70
{
    g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nAbilityMode = 2;
    UpdateView();
}
#endif

#ifndef _XBOX
void CSWGuiInGameAbilities::OnPowersButtonPressed(CSWGuiControl* pControl)  // PC 0x006ADAA0
{
    g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nAbilityMode = 1;
    UpdateView();
}
#endif

#ifndef _XBOX
void CSWGuiInGameAbilities::OnSkillsButtonPressed(CSWGuiControl* pControl)  // PC 0x006ADAD0
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
void CSWGuiInGameAbilities::HandleInputEvent(int nEvent, int nState)  // PC 0x006AE5F0  Xbox 0x000E4DA0
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
void CSWGuiInGameAbilities::HandleInputEvent(int nEvent, int nState)  // PC 0x006AE5F0  Xbox 0x000E4DA0
{
    if (nState != 0)
    {
        switch (nEvent)
        {
        case 206:
        {
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
            CSWServerInfo* pInfo =
                (CSWServerInfo*)g_pAppManager->m_pServerExoApp->GetServerInfo();
            if (pInfo->m_bPartyPicking)
            {
                int bFound = 0;
                int nNPC = m_nCharacterIndex + 1;
                while (nNPC < 9 && !bFound)
                {
                    if (g_pAppManager->m_pServerExoApp->GetPartyTable()
                            ->GetIsNPCAvailable(nNPC))
                    {
                        CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
                        unsigned long oidNPC =
                            g_pAppManager->m_pServerExoApp->GetPartyTable()
                                ->GetNPCObject(nNPC, 0, 1);
                        CSWCCreature* pCreature = pClient->GetCreatureByGameObjectID(
                            pClient->ServerToClientObjectId(oidNPC));
                        if (pCreature != NULL)
                        {
                            SetCharacter(pCreature);
                            m_nCharacterIndex = nNPC;
                            bFound = 1;
                        }
                    }
                    ++nNPC;
                }
                if (!bFound)
                {
                    m_nCharacterIndex = CHARACTER_PLAYER;
                    SetCharacter(g_pAppManager->m_pClientExoApp->GetSWParty()
                                     ->GetPlayerCharacter());
                }
            }
            else
            {
                g_pAppManager->m_pClientExoApp
                    ->ChangeCharacterToNextLivingPartyMember(0, 0, 1);
                SetCharacter(g_pAppManager->m_pClientExoApp->GetSWParty()
                                 ->GetPlayerCharacter());
            }
            UpdatePortraits();
            break;
        }
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
// PC 0x006ADB00  Xbox 0x000E4C30
void CSWGuiInGameAbilities::SetCharacter(CSWCCreature* pCreature)
{
    m_pCreature = pCreature;
    if (pCreature != NULL)
    {
        CResRef* pPortrait = &pCreature->GetPortrait(1);
        m_lblPortrait.m_cBorder.m_cParams.SetFillImage(*pPortrait, 0);
        m_lbAbility.ClearItems();
        CreateFeatList();
        CreateSkillList();
        unsigned char nClass = m_pCreature->m_pStats->GetClass(
            m_pCreature->m_pStats->GetNumClasses() - 1);
        if (CSWClass::IsJedi((int)nClass) != 0)
        {
            CreatePowerList();
            if (m_cPowerChart.m_cFlows.num > 0)
            {
#ifdef _XBOX
                m_lblPowers.SetEnabledFlag(1);
#else
                m_btnPowers.SetEnabledFlag(1);
#endif
                UpdateView();
                return;
            }
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
void CSWGuiInGameAbilities::OnPanelAdded()  // PC 0x006ADC20  Xbox 0x000E4D50
{
    SetCharacter(g_pAppManager->m_pClientExoApp->GetSWParty()->GetCharacter(0));
    m_lblBlack.SetEnabledFlag(g_pAppManager->m_pClientExoApp->GetSWParty()->GetNumCharacters() > 1);
}
#else
void CSWGuiInGameAbilities::OnPanelAdded()  // PC 0x006ADC20  Xbox 0x000E4D50
{
    m_nCharacterIndex = -1;
    m_btnChange1.m_nIndex = -2;
    m_btnChange2.m_nIndex = -2;
    SetCharacter(g_pAppManager->m_pClientExoApp->GetSWParty()->GetCharacter(0));
    UpdatePortraits();
    m_btnExit.HandleMouseOver(0);
    CSWGuiPanel::OnPanelAdded();
}
#endif

#ifndef _XBOX
void CSWGuiInGameAbilities::OnChangeCharacter(CSWGuiControl* pControl)  // PC 0x006ADC80
{
    int nPartyMember = 1;
    if (pControl->m_bSelected != 0)
    {
        CSWServerInfo* pInfo =
            (CSWServerInfo*)g_pAppManager->m_pServerExoApp->GetServerInfo();
        if (pInfo->m_bPartyPicking)
        {
            char nCharacter = pControl->m_nIndex;
            CSWCCreature* pCreature;
            if (nCharacter == CHARACTER_PLAYER)
            {
                pCreature = g_pAppManager->m_pClientExoApp->GetPlayerCreature();
            }
            else
            {
                CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
                unsigned long oidNPC =
                    g_pAppManager->m_pServerExoApp->GetPartyTable()
                        ->GetNPCObject(nCharacter, 0, 1);
                pCreature = pClient->GetCreatureByGameObjectID(
                    pClient->ServerToClientObjectId(oidNPC));
            }
            if (pCreature != NULL)
            {
                if (m_btnCharRight.m_bEnabled)
                {
                    if (pControl == &m_btnChange1)
                    {
                        m_btnChange1.m_nIndex = m_btnChange2.m_nIndex;
                    }
                }
                else
                {
                    m_btnChange1.m_nIndex = CHARACTER_UNSET;
                }
                m_nCharacterIndex = nCharacter;
                SetCharacter(pCreature);
            }
        }
        else
        {
            if (pControl == &m_btnChange2)
            {
                nPartyMember = 2;
            }
            if (g_pAppManager->m_pClientExoApp
                    ->ChangeCharacterToNextLivingPartyMember(0, 0, nPartyMember))
            {
                m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
                SetCharacter(g_pAppManager->m_pClientExoApp->GetSWParty()
                                 ->GetPlayerCharacter());
            }
            else
            {
                return;
            }
        }
        UpdatePortraits();
    }
}
#endif

static inline void SwguiingameabilitiesTrace2()
{
    SwguiingameabilitiesTrace1();
}

static inline void SwguiingameabilitiesTrace1()
{
}
