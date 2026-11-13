#include "swguiskillflowchart.h"
#include "guiingame.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "exoresman.h"
#include "nwccreaturestats.h"
#include "nwscreaturestats.h"
#include "nwsobject.h"
#include "nwrules.h"
#include "nwfeat.h"

static inline void SwguiskillflowchartTrace3();
static inline void SwguiskillflowchartTrace2();
static inline void SwguiskillflowchartTrace1();
#define SWGUISKILLFLOWCHART_TRACE() SwguiskillflowchartTrace1()
// PC 0x0073AE40
void CSWGuiSkillFlow::Draw(float fElapsed)
{
    int i;

    for (i = 0; i < 3; i++)
    {
        if (m_aSkills[i].m_nSkill != -1)
        {
            m_aSkills[i].m_cRightImage.Draw(fElapsed);
            m_aSkills[i].m_cLeftImage.Draw(fElapsed);
        }
    }
    for (i = 0; i < 2; i++)
    {
        if (m_aSkills[i + 1].m_nSkill != -1)
        {
            m_aArrows[i].Draw(fElapsed);
        }
    }
}
// PC 0x0073B1B0
CSWGuiSkillFlow::CSWGuiSkillFlow()
{
    int i;

    for (i = 0; i < 3; i++)
    {
        m_aSkills[i].m_cLeftImage.m_cParams.SetDrawStyle(1);
        m_aSkills[i].m_cLeftImage.m_cParams.SetAlignment(CSWGuiTextParams::ALIGNMENT_CENTER);
#ifdef _XBOX
        m_aSkills[i].m_cRightImage.m_cParams.SetImage("lbl_indent", 0);
#else
        CResRef cIndent("UIBIT_ABI_BACK");
        m_aSkills[i].m_cRightImage.m_cParams.SetImage(cIndent, 0);
#endif
    }
    for (i = 0; i < 2; i++)
    {
        m_aArrows[i].m_cParams.SetDrawStyle(1);
        m_aArrows[i].m_cParams.SetAlignment(CSWGuiTextParams::ALIGNMENT_CENTER);
#ifdef _XBOX
        m_aArrows[i].m_cParams.SetImage("lbl_skarr", 0);
#else
        CResRef cArrow("UIBIT_ABI_ARROW");
        m_aArrows[i].m_cParams.SetImage(cArrow, 0);
#endif
    }
    for (i = 0; i < 3; i++)
    {
        m_aSkills[i].m_bSelected = 0;
        m_aSkills[i].m_nSkill = -1;
        m_aSkills[i].m_nStatus = 0;
    }
}

// PC 0x0073AC90
void CSWGuiSkillFlow::SetExtent(const CSWGuiExtent& cExtent)
{
    SwguiskillflowchartTrace1();
    CSWGuiExtent cFlowExtent = cExtent;
    float fScale = 1.0f;
    if (g_pAppManager->m_pClientExoApp->GetSWGuiManager() != NULL)
    {
        fScale = (float)g_pAppManager->m_pClientExoApp->GetSWGuiManager()->m_nWidth / (float)g_nGUIWidth;
    }
    cFlowExtent.m_nWidth = (int)(fScale * FLOW_WIDTH);
    cFlowExtent.m_nHeight = FLOW_HEIGHT;
    CSWGuiExtent cSkillExtent;
    cSkillExtent.m_nLeft = cFlowExtent.m_nLeft;
    cSkillExtent.m_nTop = cFlowExtent.m_nTop + (cFlowExtent.m_nHeight - SKILL_ICON_SIZE) / 2;
    cSkillExtent.m_nHeight = SKILL_ICON_SIZE;
    cSkillExtent.m_nWidth = SKILL_ICON_SIZE;
    CSWGuiExtent cArrowExtent;
    cArrowExtent.m_nLeft = 0;
    cArrowExtent.m_nTop = 0;
    cArrowExtent.m_nWidth = 32;
    cArrowExtent.m_nHeight = 32;
    int i;

    m_aSkills[0].m_cRightImage.SetExtent(cSkillExtent);

    cSkillExtent.m_nLeft =
        cFlowExtent.m_nLeft + (cFlowExtent.m_nWidth - cFlowExtent.m_nHeight) / 2;
    m_aSkills[1].m_cRightImage.SetExtent(cSkillExtent);

    cSkillExtent.m_nLeft =
        cFlowExtent.m_nLeft + cFlowExtent.m_nWidth - cFlowExtent.m_nHeight;
    m_aSkills[2].m_cRightImage.SetExtent(cSkillExtent);

    cArrowExtent.m_nTop = cFlowExtent.m_nTop + (cFlowExtent.m_nHeight - 32) / 2;
    cArrowExtent.m_nLeft =
        cFlowExtent.m_nLeft + (cFlowExtent.m_nWidth + cFlowExtent.m_nHeight) / 4 - 16;
    m_aArrows[0].SetExtent(cArrowExtent);

    cArrowExtent.m_nLeft =
        cFlowExtent.m_nLeft + (3 * cFlowExtent.m_nWidth - cFlowExtent.m_nHeight) / 4 - 16;
    m_aArrows[1].SetExtent(cArrowExtent);

    for (i = 0; i < 3; i++)
    {
        m_aSkills[i].m_cLeftImage.SetExtent(
            m_aSkills[i].m_cRightImage.m_cExtent);
    }
    m_cExtent = cFlowExtent;
}
// PC 0x0073B2E0
void CSWGuiSkillFlow::SetSkillSelected(unsigned char nSkill, int bSelected)
{
    SwguiskillflowchartTrace2();
    if (m_aSkills[nSkill].m_bSelected != bSelected)
    {
        m_aSkills[nSkill].m_bSelected = bSelected;
        SetSkillStatus(nSkill, m_aSkills[nSkill].m_nStatus);
    }
}
// PC 0x0073B8F0
CSWGuiSkillFlowChart::CSWGuiSkillFlowChart()
{
    m_nSelectedCol = 0;
    m_nSelectedRow = 0;
}
// PC 0x0073B910
CSWGuiSkillFlowChart::~CSWGuiSkillFlowChart()
{
    ClearChart();
}
// PC 0x0073BE80
void CSWGuiSkillFlowChart::BuildSortedPowerList(
    CExoArrayList<unsigned long>& lstPowers)
{
    int nSpell;
    int i;
    int bInserted;
    CSWSpell* pSpell;
    CSWSpell* pOther;

    CExoArrayList<unsigned long>* pCachedPowers =
        &g_pAppManager->m_pClientExoApp->GetInGameGui()->m_a1048;
    if (pCachedPowers != NULL && pCachedPowers->num > 0)
    {
        lstPowers = *pCachedPowers;
        return;
    }
    for (nSpell = 0; nSpell < g_pRules->m_pSpellArray->m_nNumSpells; nSpell++)
    {
        pSpell = g_pRules->m_pSpellArray->GetSpell(nSpell);
        if (pSpell == NULL ||
            (pSpell->m_nUserType != CSWSpell::USER_TYPE_FORCE_POWER &&
             pSpell->m_nUserType != USER_TYPE_FORM) ||
            pSpell->m_nNameStrRef == 0)
        {
            continue;
        }
        bInserted = FALSE;
        for (i = 0; i < lstPowers.num; i++)
        {
            unsigned long nPower = lstPowers[i];
            pOther = g_pRules->m_pSpellArray->GetSpell(nPower);
            if (pOther != NULL)
            {
                if (pOther->GetPriority(0) > pSpell->GetPriority(0))
                {
                    bInserted = TRUE;
                    break;
                }
                if (pOther->GetPriority(0) == pSpell->GetPriority(0))
                {
                    int nPowerTier = pSpell->GetSubRadialSpell(0) / CSWSpell::SUBRADIAL_TYPE_SCALE;
                    if ((pOther->GetSubRadialSpell(0) / CSWSpell::SUBRADIAL_TYPE_SCALE) < nPowerTier)
                    {
                        bInserted = TRUE;
                        break;
                    }
                    nPowerTier = pSpell->GetSubRadialSpell(0) / CSWSpell::SUBRADIAL_TYPE_SCALE;
                    if ((pOther->GetSubRadialSpell(0) / CSWSpell::SUBRADIAL_TYPE_SCALE) == nPowerTier &&
                        pOther->GetSlot(0) > pSpell->GetSlot(0))
                    {
                        bInserted = TRUE;
                        break;
                    }
                }
            }
        }
        if (bInserted)
        {
            lstPowers.Insert(nSpell, i);
        }
        else
        {
            lstPowers.Add(nSpell);
        }
    }
    CGuiInGame* pInGameGui = g_pAppManager->m_pClientExoApp->GetInGameGui();
    pInGameGui->m_a1048 = lstPowers;
}
// PC 0x0073B970
void CSWGuiSkillFlowChart::AddPowerSet(unsigned long nPower1,
                                      unsigned long nPower2,
                                      unsigned long nPower3, int bSecret)
{
    CResRef resIcon;
    CSWGuiSkillFlow* pFlow = new CSWGuiSkillFlow;
    unsigned long aPowers[3] = {nPower1, nPower2, nPower3};

    for (int i = 0; i < 3; i++)
    {
        CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(aPowers[i]);
        if (bSecret)
            resIcon = "ip_secret";
        else if (pSpell != NULL)
            resIcon = pSpell->m_resIcon;
        else
            resIcon = "";
        pFlow->SetSkillInfo(i, aPowers[i], resIcon);
    }
    {
        CSWGuiExtent cExtent;
        cExtent.m_nLeft = 0;
        cExtent.m_nTop = 0;
        cExtent.m_nWidth = CSWGuiSkillFlow::FLOW_WIDTH;
        cExtent.m_nHeight = 40;
        pFlow->SetExtent(cExtent);
    }
    m_cFlows.Add(pFlow);
}
// PC 0x0073C440
void CSWGuiSkillFlowChart::BuildSortedFeatList(
    CExoArrayList<unsigned short>& cFeats)
{
    CExoArrayList<unsigned short>* pCachedFeats =
        &g_pAppManager->m_pClientExoApp->GetInGameGui()->m_a103C;
    if (pCachedFeats != NULL && pCachedFeats->num > 0)
    {
        cFeats = *pCachedFeats;
        return;
    }
    for (int nFeat = 0; nFeat < g_pRules->m_nNumFeats; nFeat++)
    {
        CSWFeat* pFeat = g_pRules->GetFeat(nFeat);
        if (nFeat != CSWFeat::FEAT_COMPLEX_UNARMED_ANIMS &&
            nFeat != CSWFeat::FEAT_MINE_IMMUNITY &&
            pFeat != NULL && pFeat->m_nNameStrRef != 0)
        {
            int nDepth = pFeat->m_nPrereqFeat1 == CSWFeat::FEAT_INVALID ? 0 :
                (pFeat->m_nPrereqFeat2 == CSWFeat::FEAT_INVALID ? 1 : 2);
            int nIndex;
            for (nIndex = 0; ; nIndex++)
            {
                if (nIndex < cFeats.num)
                {
                    CSWFeat* pListedFeat = g_pRules->GetFeat(cFeats[nIndex]);
                    if (pListedFeat != NULL)
                    {
                        int nListedDepth =
                            pListedFeat->m_nPrereqFeat1 == CSWFeat::FEAT_INVALID ? 0 :
                            (pListedFeat->m_nPrereqFeat2 == CSWFeat::FEAT_INVALID ? 1 : 2);
                        if (nListedDepth > nDepth)
                        {
                            cFeats.Insert(nFeat, nIndex);
                            break;
                        }
                    }
                }
                else
                {
                    cFeats.Add(nFeat);
                    break;
                }
            }
        }
    }
    CGuiInGame* pInGameGui = g_pAppManager->m_pClientExoApp->GetInGameGui();
    pInGameGui->m_a103C = cFeats;
}

#include "nwspell.h"
// PC 0x0073BB10
void CSWGuiSkillFlowChart::AddFeatSet(unsigned long nFeat1,
                                     unsigned long nFeat2,
                                     unsigned long nFeat3)
{
    CResRef resIcon;
    CSWGuiSkillFlow* pFlow = new CSWGuiSkillFlow;
    unsigned long aFeats[3] = {nFeat1, nFeat2, nFeat3};

    for (int i = 0; i < 3; i++)
    {
        CSWFeat* pFeat = g_pRules->GetFeat(aFeats[i]);
        if (pFeat != NULL)
            resIcon = pFeat->m_cIcon;
        else
            resIcon = "";
        pFlow->SetSkillInfo(i, aFeats[i], resIcon);
    }
    {
        CSWGuiExtent cExtent;
        cExtent.m_nLeft = 0;
        cExtent.m_nTop = 0;
        cExtent.m_nWidth = CSWGuiSkillFlow::FLOW_WIDTH;
        cExtent.m_nHeight = 40;
        pFlow->SetExtent(cExtent);
    }
    m_cFlows.Add(pFlow);
}
// PC 0x0073B370
void CSWGuiSkillFlowChart::SetSelectedSkill(unsigned long nSkill)
{
    int i;
    int nIndex;
    int j;

    for (i = 0; i < m_cFlows.num; i++)
    {
        nIndex = -1;
        for (j = 0; j < 3; j++)
        {
            if (static_cast<CSWGuiSkillFlow*>(m_cFlows[i])
                    ->m_aSkills[j]
                    .m_nSkill == nSkill)
            {
                nIndex = j;
                break;
            }
        }
        if (nIndex != -1)
        {
            if (m_nSelectedCol != nIndex || m_nSelectedRow != i)
            {
                static_cast<CSWGuiSkillFlow*>(m_cFlows[m_nSelectedRow])
                    ->SetSkillSelected(m_nSelectedCol, 0);
                m_nSelectedCol = nIndex;
                m_nSelectedRow = i;
            }
            static_cast<CSWGuiSkillFlow*>(m_cFlows[m_nSelectedRow])
                ->SetSkillSelected(m_nSelectedCol, 1);
            return;
        }
    }
}
// PC 0x0073B520
int CSWGuiSkillFlowChart::SetTopLeft()
{
    CSWGuiSkillFlow* pFlow;

    if (m_nSelectedCol != 0 || m_nSelectedRow != 0)
    {
        if (m_nSelectedRow < m_cFlows.num)
        {
            static_cast<CSWGuiSkillFlow*>(m_cFlows[m_nSelectedRow])
                ->SetSkillSelected(m_nSelectedCol, 0);
        }
        m_nSelectedRow = 0;
        m_nSelectedCol = 0;
    }
    if (m_cFlows.num > 0)
    {
        pFlow = static_cast<CSWGuiSkillFlow*>(m_cFlows[0]);
        if (!pFlow->m_aSkills[0].m_bSelected)
        {
            pFlow->m_aSkills[0].m_bSelected = 1;
            pFlow->SetSkillStatus(0, pFlow->m_aSkills[0].m_nStatus);
        }
        return static_cast<CSWGuiSkillFlow*>(m_cFlows[0])
            ->m_aSkills[0]
            .m_nSkill;
    }
    return -1;
}
// PC 0x0073B590
int CSWGuiSkillFlowChart::HandleInput(int nEvent)
{
    int nOldRow = m_nSelectedRow;
    int nOldCol = m_nSelectedCol;

    switch (nEvent)
    {
    case GUIEVENT_DOWN:
    case GUIEVENT_DOWN_ALTERNATE:
        if (m_nSelectedRow == m_cFlows.num - 1)
            m_nSelectedRow = 0;
        else
            ++m_nSelectedRow;
        while (m_cFlows[m_nSelectedRow]->m_aSkills[m_nSelectedCol].m_nSkill == CSWGuiSkillFlow::SKILL_INVALID)
            --m_nSelectedCol;
        break;

    case GUIEVENT_UP:
    case GUIEVENT_UP_ALTERNATE:
        if (m_nSelectedRow > 0)
            --m_nSelectedRow;
        else
            m_nSelectedRow = m_cFlows.num - 1;
        while (m_cFlows[m_nSelectedRow]->m_aSkills[m_nSelectedCol].m_nSkill == CSWGuiSkillFlow::SKILL_INVALID)
            --m_nSelectedCol;
        break;

    case GUIEVENT_RIGHT:
    case GUIEVENT_RIGHT_ALTERNATE:
        if (m_nSelectedCol < 2 &&
            m_cFlows[m_nSelectedRow]->m_aSkills[(unsigned char)(m_nSelectedCol + 1)].m_nSkill != CSWGuiSkillFlow::SKILL_INVALID)
        {
            g_pAppManager->GetClientExoApp()->GetSWGuiManager()->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
            ++m_nSelectedCol;
        }
        break;

    case GUIEVENT_LEFT:
    case GUIEVENT_LEFT_ALTERNATE:
        if (m_nSelectedCol > 0)
        {
            g_pAppManager->GetClientExoApp()->GetSWGuiManager()->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
            --m_nSelectedCol;
        }
        break;
    }

    if (nOldRow != m_nSelectedRow || nOldCol != m_nSelectedCol)
    {
        m_cFlows[nOldRow]->SetSkillSelected(nOldCol, 0);
        m_cFlows[m_nSelectedRow]->SetSkillSelected(m_nSelectedCol, 1);
    }
    return m_cFlows[m_nSelectedRow]->m_aSkills[m_nSelectedCol].m_nSkill;
}
// PC 0x0073CD10
CSWGuiSkillInfoBox::CSWGuiSkillInfoBox(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    SwguiskillflowchartTrace3();
#ifdef _XBOX
    StartLoadFromLayout("skillinfo");
#else
    {
        CResRef resLayout("skillinfo_p");
        StartLoadFromLayout(resLayout);
    }
#endif
    InitControl(m_lbSkills, "LB_SKILLS", 1);
    InitControl(m_lblTitle, "LBL_MESSAGE", 1);
#ifdef _XBOX
    InitControl(m_lblA, "LBL_A", 1);
    InitControl(m_lblOk, "LBL_OK", 1);
#else
    InitControl(m_btnExit, "BTN_OK", 1);
#endif
    StopLoadFromLayout();

    m_lbSkills.SetScrollOnly(1);
    m_lbSkills.m_nSelectSound = CSWGuiManager::GUISOUND_SELECTION;
#ifndef _XBOX
    m_lbSkills.SetEnabled(0);
#endif
    CSWGuiLabelHilight* pProto = m_lbSkills.m_pProtoItem->AsLabelHilight();
    int nWidth = m_lbSkills.GetItemWidth();
    for (int i = 0; i < MAX_SKILL_ENTRIES; i++)
    {
        m_aSkillEntries[i].Initialize(pProto->m_cText, pProto->m_cBorder,
                                     pProto->m_cHilite, nWidth);
    }
#ifndef _XBOX
    m_btnExit.SetFocusable(0);
    m_btnExit.AddEvent(GUIEVENT_ACTIVATE, this,
                     (CSWGuiHandler)&CSWGuiPanel::OnAButtonPressed);
#endif
    CenterPanel();
    m_pCallbackObject = NULL;
    m_pCallback = NULL;
}
// PC 0x0073AEB0
void CSWGuiSkillFlow::SetSkillStatus(unsigned char nSkill, unsigned char nStatus)
{
    SwguiskillflowchartTrace1();
    m_aSkills[nSkill].m_nStatus = nStatus;
    m_aSkills[nSkill].m_cLeftImage.m_cParams.m_fAlpha = 1.0f;
    m_aSkills[nSkill].m_cRightImage.m_cParams.m_fAlpha = 1.0f;
    if (nSkill > 0)
        m_aArrows[nSkill - 1].m_cParams.m_fAlpha = 1.0f;

    switch (nStatus)
    {
    case STATUS_GRANTED:
    case STATUS_GRANTED_ALTERNATE:
        m_aSkills[nSkill].m_cRightImage.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_TEXT;
        if (m_aSkills[nSkill].m_bSelected)
            m_aSkills[nSkill].m_cRightImage.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_TEXT;
        else
            m_aSkills[nSkill].m_cRightImage.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_TEXT;
        break;
    case STATUS_CHOSEN:
        m_aSkills[nSkill].m_cRightImage.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_HIGHLIGHT;
        break;
    case STATUS_UNAVAILABLE:
        m_aSkills[nSkill].m_cLeftImage.m_cParams.m_fAlpha = 0.25f;
        m_aSkills[nSkill].m_cRightImage.m_cParams.m_vColor = CGuiInGame::COLOR_RED;
        if (nSkill > 0)
            m_aArrows[nSkill - 1].m_cParams.m_fAlpha = 0.0f;
        break;
    case STATUS_YELLOW_DIM_ARROW:
        m_aSkills[nSkill].m_cRightImage.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_BONUS;
        if (nSkill > 0)
            m_aArrows[nSkill - 1].m_cParams.m_fAlpha = 0.25f;
        break;
    case STATUS_AVAILABLE:
        m_aSkills[nSkill].m_cLeftImage.m_cParams.m_fAlpha = 0.25f;
        if (m_aSkills[nSkill].m_bSelected)
            m_aSkills[nSkill].m_cRightImage.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_GREEN;
        else
            m_aSkills[nSkill].m_cRightImage.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_GREEN;
        break;
    case STATUS_YELLOW:
        if (m_aSkills[nSkill].m_bSelected)
            m_aSkills[nSkill].m_cRightImage.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_HIGHLIGHT;
        else
            m_aSkills[nSkill].m_cRightImage.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_GREEN;
        break;
    }
    if (m_aSkills[nSkill].m_bSelected)
        m_aSkills[nSkill].m_cRightImage.m_cParams.m_bPulsing = 1;
    else
        m_aSkills[nSkill].m_cRightImage.m_cParams.m_bPulsing = 0;
}
// PC 0x0073B070
unsigned int CSWGuiSkillFlow::SkillHitCheckMouse(int nX, int nY)
{
    SwguiskillflowchartTrace1();
    int nHit;
    int i;
    CSWGuiExtent cExtent;

    nHit = 0;
    for (i = 0; i < 3 && m_aSkills[i].m_nSkill != -1; i++)
    {
        cExtent = m_aSkills[i].m_cRightImage.m_cExtent;
        if (nX >= cExtent.m_nLeft && nX <= cExtent.m_nLeft + cExtent.m_nWidth &&
            nY >= cExtent.m_nTop && nY <= cExtent.m_nTop + cExtent.m_nHeight)
        {
            nHit = i;
            break;
        }
    }
    return m_aSkills[nHit].m_nSkill;
}
// PC 0x0073B330
int CSWGuiSkillFlowChart::ClearChart()
{
    int nResult;
    int i;

    nResult = m_cFlows.num;
    for (i = 0; i < nResult; i++)
    {
        if (m_cFlows[i] != 0)
        {
            delete static_cast<CSWGuiSkillFlow*>(m_cFlows[i]);
        }
        m_cFlows[i] = 0;
        nResult = m_cFlows.num;
    }
    m_cFlows.num = 0;
    return nResult;
}
// PC 0x0073B480
void CSWGuiSkillFlowChart::SetSkillStatus(unsigned long nSkill,
                                          unsigned char nStatus)
{
    int i;
    int nIndex;
    int j;

    for (i = 0; i < m_cFlows.num; i++)
    {
        nIndex = -1;
        for (j = 0; j < 3; j++)
        {
            if (static_cast<CSWGuiSkillFlow*>(m_cFlows[i])
                    ->m_aSkills[j]
                    .m_nSkill == nSkill)
            {
                nIndex = j;
                break;
            }
        }
        if (nIndex != -1)
        {
            static_cast<CSWGuiSkillFlow*>(m_cFlows[i])
                ->SetSkillStatus(nIndex, nStatus);
            return;
        }
    }
}
// PC 0x0073BCB0
void CSWGuiSkillInfoBox::SetSkillList(const CExoArrayList<unsigned long>& lSkills,
                                      unsigned char nState)
{
    CExoArrayList<CSWGuiControl*> lControls;
    EXOXBOXASSERT(lSkills.num <= MAX_SKILL_ENTRIES);
    for (int i = 0; i < lSkills.num; i++)
    {
        m_aSkillEntries[i].SetSkill(lSkills.element[i], nState);
        lControls.Add(&m_aSkillEntries[i]);
    }
    m_lbSkills.AddControls(lControls, 0, 1, 0);
    SetActiveControl(&m_lbSkills, 0);
}
// PC 0x0073C120
void CSWGuiSkillFlowChart::CreatePowerChart(CSWSCreatureStats* pStats,
                                          int bAllPowers)
{
    SwguiskillflowchartTrace2();
    CSWSpell* pSpell;
    CSWSpell* pNextSpell;
    CExoArrayList<unsigned long> aPowers;
    unsigned long nNextPower;
    unsigned long nAltPower;
    int bContinue;
    int i;
    int j;

    ClearChart();
    BuildSortedPowerList(aPowers);

    bContinue = 1;
    for (i = 0; bContinue && i < aPowers.num; i++)
    {
        unsigned long nPower = aPowers[i];
        pSpell = g_pRules->m_pSpellArray->GetSpell(nPower);
        if (pSpell == NULL)
        {
            continue;
        }
        if (pSpell->GetPriority(0) > CSWSpell::POWER_PRIORITY_BASE)
        {
            bContinue = 0;
            continue;
        }
        int bAvailable = 1;
        if (bAllPowers)
        {
            if (pSpell->GetSpellLevel(pStats->GetClass(pStats->GetNumClasses() - 1)) ==
                CSWSpell::SPELL_LEVEL_INVALID)
            {
                bAvailable = 0;
            }
        }
        if ((bAllPowers && bAvailable) ||
            (!bAllPowers && pStats->HasSpell(0, aPowers[i], 0)))
        {
            nNextPower = CSWGuiSkillFlow::SKILL_INVALID;
            nAltPower = CSWGuiSkillFlow::SKILL_INVALID;
            for (j = i + 1; j < aPowers.num &&
                            (nNextPower == CSWGuiSkillFlow::SKILL_INVALID ||
                             nAltPower == CSWGuiSkillFlow::SKILL_INVALID); j++)
            {
                if (!bAllPowers && !pStats->HasSpell(0, aPowers[j], 0))
                {
                    continue;
                }
                unsigned long nNext = aPowers[j];
                pNextSpell = g_pRules->m_pSpellArray->GetSpell(nNext);
                if (pNextSpell != NULL)
                {
                    int nPowerType = pSpell->GetSubRadialSpell(0) / CSWSpell::SUBRADIAL_TYPE_SCALE;
                    if (pNextSpell->GetSubRadialSpell(0) / CSWSpell::SUBRADIAL_TYPE_SCALE != nPowerType)
                    {
                        continue;
                    }
                    if (pNextSpell->GetSlot(0) != pSpell->GetSlot(0))
                    {
                        continue;
                    }
                    if (pNextSpell->GetPriority(0) == CSWSpell::POWER_PRIORITY_FIRST_UPGRADE)
                    {
                        nNextPower = aPowers[j];
                    }
                    else if (pNextSpell->GetPriority(0) == CSWSpell::POWER_PRIORITY_SECOND_UPGRADE)
                    {
                        nAltPower = aPowers[j];
                    }
                }
            }
            AddPowerSet(aPowers[i], nNextPower, nAltPower, 0);
        }
        else
        {
            if (pStats->m_pCreature->m_bPlayerCreated && !bAllPowers && bAvailable &&
                !pStats->HasSpell(0, aPowers[i], 0) &&
                (aPowers[i] == FORCE_POWER_FORCE_CRUSH ||
                 aPowers[i] == FORCE_POWER_PRECOGNITION ||
                 aPowers[i] == CSWSpell::FORCE_POWER_BATTLE_PRECOGNITION ||
                 aPowers[i] == FORCE_POWER_FORCE_ENLIGHTENMENT ||
                 aPowers[i] == CSWSpell::FORCE_POWER_BEAST_TRICK ||
                 aPowers[i] == FORCE_POWER_BREATH_CONTROL ||
                 aPowers[i] == FORCE_POWER_FORCE_SIGHT))
            {
                AddPowerSet(aPowers[i], CSWGuiSkillFlow::SKILL_INVALID,
                            CSWGuiSkillFlow::SKILL_INVALID, 1);
            }
        }
    }
}
// PC 0x0073C580
void CSWGuiSkillFlowChart::CreateFeatChart(CSWSCreatureStats* pStats)
{
    SwguiskillflowchartTrace2();
    CSWClass* pClass;
    CSWFeat* pFeat;
    CSWFeat* pNextFeat;
    CExoArrayList<unsigned short> aFeats;
    unsigned long nNextFeat;
    unsigned long nAltFeat;
    int bContinue;
    int i;
    int j;

    pClass = &g_pRules->m_pClasses[pStats->GetClass(
        pStats->GetNumClasses() - 1)];
    ClearChart();
    BuildSortedFeatList(aFeats);

    bContinue = 1;
    for (i = 0; bContinue && i < aFeats.num; i++)
    {
        pFeat = g_pRules->GetFeat(aFeats[i]);
        if (aFeats[i] == CSWFeat::FEAT_COMPLEX_UNARMED_ANIMS ||
            aFeats[i] == CSWFeat::FEAT_MINE_IMMUNITY)
        {
            continue;
        }
        if (pFeat == NULL)
        {
            continue;
        }
        if (pFeat->GetPrereqFeat1() != CSWFeat::FEAT_INVALID)
        {
            bContinue = 0;
            continue;
        }
        if (!pStats->HasFeat(aFeats[i]))
        {
            continue;
        }

        nNextFeat = CSWGuiSkillFlow::SKILL_INVALID;
        nAltFeat = CSWGuiSkillFlow::SKILL_INVALID;
        for (j = i + 1; j < aFeats.num &&
                        (nNextFeat == CSWGuiSkillFlow::SKILL_INVALID ||
                         nAltFeat == CSWGuiSkillFlow::SKILL_INVALID); j++)
        {
            if (!pStats->HasFeat(aFeats[j]))
            {
                continue;
            }
            pNextFeat = g_pRules->GetFeat(aFeats[j]);
            if (pNextFeat != NULL)
            {
                unsigned short nPrereqFeat = pNextFeat->m_nPrereqFeat1;
                if (nPrereqFeat != aFeats[i])
                {
                    continue;
                }
                if (pClass->IsFeatUseable(aFeats[j]) ||
                    pStats->HasFeat(aFeats[j]))
                {
                    if (pNextFeat->GetPrereqFeat2() == CSWFeat::FEAT_INVALID)
                    {
                        nNextFeat = aFeats[j];
                    }
                    else
                    {
                        nAltFeat = aFeats[j];
                    }
                }
            }
        }
        AddFeatSet(aFeats[i], nNextFeat, nAltFeat);
    }
}
// PC 0x0073C7A0
void CSWGuiSkillFlowChart::CreateFeatChart(CSWCCreatureStats* pStats,
                                           int bBonusFeats)
{
    SwguiskillflowchartTrace2();
    CSWClass* pClass;
    CSWFeat* pFeat;
    CSWFeat* pNextFeat;
    CExoArrayList<unsigned short> aFeats;
    unsigned long nNextFeat;
    unsigned long nAltFeat;
    unsigned char nLevel;
    int bContinue;
    int bCompanionFeat;
    int i;
    int j;

    pClass = &g_pRules->m_pClasses[pStats->GetClass(
        pStats->GetNumClasses() - 1)];
    ClearChart();
    BuildSortedFeatList(aFeats);

    bContinue = 1;
    bCompanionFeat = 0;
    for (i = 0; bContinue && i < aFeats.num; i++)
    {
        pFeat = g_pRules->GetFeat(aFeats[i]);
        if (aFeats[i] == CSWFeat::FEAT_COMPLEX_UNARMED_ANIMS ||
            aFeats[i] == CSWFeat::FEAT_MINE_IMMUNITY)
        {
            continue;
        }
        if (pFeat == NULL)
        {
            continue;
        }
        if (pFeat->m_nPrereqFeat1 != CSWFeat::FEAT_INVALID)
        {
            bContinue = 0;
            continue;
        }
        if (!pStats->HasFeat(aFeats[i]) &&
            !pClass->IsFeatUseable(aFeats[i]))
        {
            continue;
        }
        int bSelectable = 0;
        if ((pClass->IsGrantedFeat(aFeats[i], nLevel) &&
             pStats->HasFeat(aFeats[i])) ||
            (bBonusFeats && pClass->IsBonusFeat(aFeats[i])) ||
            (!bBonusFeats && pClass->IsNormalFeat(aFeats[i])))
        {
            bSelectable = 1;
        }
        if (pStats->GetTag().CompareNoCase("handmaiden"))
        {
            if (pFeat->m_bHandmaiden > 0)
            {
                bCompanionFeat = 1;
                bSelectable = 1;
            }
        }
        else if (pStats->GetTag().CompareNoCase("baodur"))
        {
            if (pFeat->m_bBaoDur == 0xff)
            {
                continue;
            }
            if (pFeat->m_bBaoDur > 0)
            {
                bCompanionFeat = 1;
                bSelectable = 1;
            }
        }
        else if (pStats->GetTag().CompareNoCase("kreia"))
        {
            if (pFeat->m_bKreia == 0xff)
            {
                continue;
            }
            if (!bSelectable)
            {
                continue;
            }
        }
        else if (pStats->GetTag().CompareNoCase("hanharr"))
        {
            if (pFeat->m_bHanharr > 0)
            {
                bCompanionFeat = 1;
                bSelectable = 1;
            }
        }
        else if (pStats->GetTag().CompareNoCase("hk47"))
        {
            if (pFeat->m_bHK47 > 0)
            {
                bCompanionFeat = 1;
                bSelectable = 1;
            }
        }
        else if (pStats->GetTag().CompareNoCase("g0t0"))
        {
            if (pFeat->m_bG0T0 > 0)
            {
                bCompanionFeat = 1;
                bSelectable = 1;
            }
        }
        else if (pStats->GetTag().CompareNoCase("atton"))
        {
            if (pFeat->m_bAtton > 0)
            {
                bCompanionFeat = 1;
                bSelectable = 1;
            }
        }
        if (!bSelectable)
        {
            continue;
        }

        nNextFeat = CSWGuiSkillFlow::SKILL_INVALID;
        nAltFeat = CSWGuiSkillFlow::SKILL_INVALID;
        for (j = i + 1; j < aFeats.num &&
                        (nNextFeat == CSWGuiSkillFlow::SKILL_INVALID || nAltFeat == CSWGuiSkillFlow::SKILL_INVALID);
             j++)
        {
            pNextFeat = g_pRules->GetFeat(aFeats[j]);
            int bExcluded = 0;
            if (pStats->GetTag().CompareNoCase("baodur") &&
                pNextFeat->m_bBaoDur == 0xff)
            {
                bExcluded = 1;
            }
            else if (pStats->GetTag().CompareNoCase("kreia") &&
                     pNextFeat->m_bKreia == 0xff)
            {
                bExcluded = 1;
            }
            if (pNextFeat != NULL)
            {
                unsigned short nPrereqFeat = pNextFeat->m_nPrereqFeat1;
                if (nPrereqFeat != aFeats[i])
                {
                    continue;
                }
                if (pStats->HasFeat(aFeats[j]) ||
                    pClass->IsFeatUseable(aFeats[j]) || bCompanionFeat)
                {
                    if (pNextFeat->m_nPrereqFeat2 == CSWFeat::FEAT_INVALID)
                    {
                        if (!bExcluded)
                        {
                            nNextFeat = aFeats[j];
                        }
                    }
                    else
                    {
                        if (!bExcluded)
                        {
                            nAltFeat = aFeats[j];
                        }
                    }
                }
            }
        }
        AddFeatSet(aFeats[i], nNextFeat, nAltFeat);
    }
}
// PC 0x0073B4E0
int CSWGuiSkillFlowChart::SetSkillStatusAll(unsigned char nStatus)
{
    int nResult;
    int i;
    int j;

    nResult = m_cFlows.num;
    for (i = 0; i < nResult; i++)
    {
        for (j = 0; j < 3; j++)
        {
            static_cast<CSWGuiSkillFlow*>(m_cFlows[i])
                ->SetSkillStatus(j, nStatus);
        }
        nResult = m_cFlows.num;
    }
    return nResult;
}
// PC 0x0073B7B0
CSWGuiSkillInfoBox::~CSWGuiSkillInfoBox()
{
    m_lbSkills.ClearItems();
}
// PC 0x0073B0F0
void CSWGuiSkillInfoBox::HandleInputEvent(int nEvent, int nState)
{
    SWGUISKILLFLOWCHART_TRACE();
    if (nState != 0)
    {
        switch (nEvent)
        {
        case 39:
        case 45:
        case 46:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            SetModal(0);
            m_pManager->PopModalPanel();
            if (m_pCallbackObject != NULL && m_pCallback != NULL)
            {
                (m_pCallbackObject->*m_pCallback)(NULL);
            }
            break;
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
// PC 0x0073B180
void CSWGuiSkillInfoBox::SetCallback(CSWGuiObject* pObject, CSWGuiHandler pfnCallback)
{
    m_pCallback = pfnCallback;
    m_pCallbackObject = pObject;
}
// PC 0x0073B1A0
void CSWGuiSkillInfoBox::OnPanelRemoved()
{
    m_pCallback = NULL;
    m_pCallbackObject = NULL;
}

static inline void SwguiskillflowchartTrace3()
{
    SwguiskillflowchartTrace2();
}

static inline void SwguiskillflowchartTrace2()
{
    SwguiskillflowchartTrace1();
}

static inline void SwguiskillflowchartTrace1()
{
}
