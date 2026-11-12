#include "exobase.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "serverexoapp.h"
#include "nwrules.h"
#include "guiingame.h"
#include "nwsobject.h"
#include "nwscreaturestats.h"
#include "nwspell.h"
#include "nwfeat.h"
#include "nwskill.h"
#include "swguipowersfeatsskillsdebugmenu.h"
#include "swmain.h"

static inline void SwguipowersfeatsskillsdebugmenuTrace2();
static inline void SwguipowersfeatsskillsdebugmenuTrace1();
#define SWGUIPOWERSFEATSSKILLSDEBUGMENU_TRACE() SwguipowersfeatsskillsdebugmenuTrace1()
// PC 0x0073D000
void CSWGuiPowersFeatsSkillsDebugMenu::AddPrerequisiteSpells(
    unsigned long nSpell, CSWSCreature* pCreature)
{
    SwguipowersfeatsskillsdebugmenuTrace2();
    const int nNoPrerequisiteSpell = -1;
    CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(nSpell);
    int nClass = pCreature->m_pStats->m_nClassCount - 1;
    if (pSpell != NULL)
    {
        int* pNext = pSpell->m_aPrerequisites;
        if (*pNext != nNoPrerequisiteSpell)
        {
            int* pPrerequisite = pNext;
            do
            {
                unsigned long nPrerequisite = *pNext;
                if (pCreature->m_pStats->HasSpell(0, nPrerequisite, 1) == 0)
                {
                    AddPrerequisiteSpells(nPrerequisite, pCreature);
                }
                pNext = ++pPrerequisite;
            }
            while (*pNext != nNoPrerequisiteSpell);
        }
        pCreature->m_pStats->AddKnownSpell(nClass, nSpell);
    }
}
// PC 0x0073D0A0
void CSWGuiPowersFeatsSkillsDebugMenu::AddPrerequisiteFeats(
    unsigned short nFeat, CSWSCreature* pCreature)
{
    SwguipowersfeatsskillsdebugmenuTrace2();
    CSWFeat* pFeat = g_pRules->GetFeat(nFeat);

    if (pFeat != NULL && pCreature != NULL)
    {
        int nSecond = pFeat->m_nPrereqFeat2;
        int nFirst = pFeat->m_nPrereqFeat1;
        if (nFirst != 0xFFFF && pCreature->m_pStats->HasFeat(nFirst) == 0)
        {
            AddPrerequisiteFeats(nFirst, pCreature);
        }
        if (nSecond != 0xFFFF && pCreature->m_pStats->HasFeat(nSecond) == 0)
        {
            AddPrerequisiteFeats(nSecond, pCreature);
        }
        pCreature->m_pStats->AddFeat(nFeat);
    }
}

void CSWGuiPowersFeatsSkillsDebugMenu::HideMenu()
{
    CreateMainPowersFeatsSkillsMenu();
    g_pAppManager->m_pClientExoApp->GetInGameGui()->HidePowersFeatsSkillsDebugMenu();
}
// PC 0x0073D130
void CSWGuiPowersFeatsSkillsDebugMenu::OnSkillPicked(CSWGuiControl* pControl)
{
    SwguipowersfeatsskillsdebugmenuTrace2();
    if (pControl->m_bSelected != 0)
    {
        int i;
        for (i = 0; m_lbOptions.GetControl(i) != pControl; i++)
        {
        }
        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
        CSWSCreature* pCreature = pServer->GetPlayerCreature();
        if (pCreature != NULL)
        {
            CSWSCreatureStats* pStats = pCreature->m_pStats;
            char nLevel = pStats->GetLevel(0);
            pStats->SetSkillRank(i, nLevel + 3);
        }
        HideMenu();
    }
}
// PC 0x0073D730
void CSWGuiPowersFeatsSkillsDebugMenu::CreateSkillsSubMenu()
{
    int i = 0;
    m_nSelected = 0;
    m_lbOptions.ClearItems();
    CExoArrayList<CSWGuiControl*> aControls;
    CSWGuiButton* pProto = static_cast<CSWGuiButton*>(m_lbOptions.m_pProtoItem);
    int nSkills = g_pRules->m_nNumSkills;
    if (nSkills != 0)
    {
        aControls.Allocate(nSkills);
    }
    aControls.num = nSkills;
    for (; i < nSkills; i++)
    {
        CExoString sText("Add ");
        sText = sText + g_pRules->m_pSkills[(unsigned short)i].GetNameText();
        CSWGuiControl** ppOption = &aControls.element[i];
        *ppOption = CreateOption(sText, (PFNGUIEVENT)OnSkillPicked, pProto);
    }
    CSWGuiListBox* pList = &m_lbOptions;
    pList->AddControls(aControls, 1, 0, 0);
    pList->m_bLooping = 1;
    SetActiveControl(pList, 0);
    m_nMenu = MENU_SKILLS;
    m_lbOptions.SetActiveControl(m_lbOptions.GetControl(m_nSelected), 0);
}
// PC 0x0073D280
void CSWGuiPowersFeatsSkillsDebugMenu::OnPowerPicked(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        int nSpell = pControl->m_nIndex;
        CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(nSpell);
        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
        CSWSCreature* pCreature = pServer->GetPlayerCreature();
        if (pSpell != NULL && pCreature != NULL)
        {
            AddPrerequisiteSpells(nSpell, pCreature);
        }
        HideMenu();
    }
}
// PC 0x0073D300
void CSWGuiPowersFeatsSkillsDebugMenu::OnFeatPicked(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        int i;
        for (i = 0; m_lbOptions.GetControl(i) != pControl; i++)
        {
        }
        int nFeat = g_pRules->m_pFeatIndices[(unsigned short)i];
        CSWFeat* pFeat = g_pRules->GetFeat(nFeat);
        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
        CSWSCreature* pCreature = pServer->GetPlayerCreature();
        if (pFeat != NULL && pCreature != NULL)
        {
            AddPrerequisiteFeats(nFeat, pCreature);
        }
        HideMenu();
    }
}
// PC 0x0073D8F0
void CSWGuiPowersFeatsSkillsDebugMenu::OnAddSkillPicked(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        CreateSkillsSubMenu();
    }
}
// PC 0x0073D3A0
void CSWGuiPowersFeatsSkillsDebugMenu::CreatePowersSubMenu()
{
    int i = 0;
    m_nSelected = 0;
    m_lbOptions.ClearItems();
    CExoArrayList<CSWGuiControl*> aControls;
    CSWGuiButton* pProto = static_cast<CSWGuiButton*>(m_lbOptions.m_pProtoItem);
    int nSpells = g_pRules->m_pSpellArray->m_nNumSpells;
    for (; i < nSpells; i++)
    {
        CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(i);
        unsigned char nUserType = pSpell->m_nUserType;
        if (nUserType == CSWSpell::USER_TYPE_FORCE_POWER && pSpell->m_nNameStrRef != 0)
        {
            CExoString sText("Add ");
            sText = sText + g_pRules->m_pSpellArray->GetSpell(i)->GetSpellNameText();
            CSWGuiControl* pOption =
                CreateOption(sText, (PFNGUIEVENT)OnPowerPicked, pProto);
            pOption->m_nIndex = i;
            aControls.Add(pOption);
        }
    }
    CSWGuiListBox* pList = &m_lbOptions;
    pList->AddControls(aControls, 1, 0, 0);
    pList->m_bLooping = 1;
    SetActiveControl(pList, 0);
    m_nMenu = MENU_POWERS;
    m_lbOptions.SetActiveControl(m_lbOptions.GetControl(m_nSelected), 0);
}
// PC 0x0073D580
void CSWGuiPowersFeatsSkillsDebugMenu::CreateFeatsSubMenu()
{
    int i = 0;
    m_nSelected = 0;
    m_lbOptions.ClearItems();
    CExoArrayList<CSWGuiControl*> aControls;
    CSWGuiButton* pProto = static_cast<CSWGuiButton*>(m_lbOptions.m_pProtoItem);
    int nFeats = g_pRules->m_nNumFeatEntries;
    if (nFeats != 0)
    {
        aControls.Allocate(nFeats);
    }
    aControls.num = nFeats;
    for (; i < nFeats; i++)
    {
        CExoString sText("Add ");
        unsigned short nFeat = g_pRules->m_pFeatIndices[(unsigned short)i];
        sText = sText + g_pRules->GetFeat(nFeat)->GetNameText();
        CSWGuiControl** ppOption = &aControls.element[i];
        *ppOption = CreateOption(sText, (PFNGUIEVENT)OnFeatPicked, pProto);
    }
    CSWGuiListBox* pList = &m_lbOptions;
    pList->AddControls(aControls, 1, 0, 0);
    pList->m_bLooping = 1;
    SetActiveControl(pList, 0);
    m_nMenu = MENU_FEATS;
    m_lbOptions.SetActiveControl(m_lbOptions.GetControl(m_nSelected), 0);
}
// PC 0x0073D8D0
void CSWGuiPowersFeatsSkillsDebugMenu::OnAddPowerPicked(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        CreatePowersSubMenu();
    }
}
// PC 0x0073D910
void CSWGuiPowersFeatsSkillsDebugMenu::OnAddFeatPicked(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        CreateFeatsSubMenu();
    }
}
// PC 0x0073D930
void CSWGuiPowersFeatsSkillsDebugMenu::CreateMainPowersFeatsSkillsMenu()
{
    CSWGuiListBox* pList = &m_lbOptions;
    m_nSelected = 0;
    pList->ClearItems();
    CExoArrayList<CSWGuiControl*> aControls;
    CSWGuiButton* pProto = static_cast<CSWGuiButton*>(m_lbOptions.m_pProtoItem);
    aControls.Allocate(3);
    aControls.num = 3;
    {
        CExoString sText("Add Power");
        CSWGuiControl** ppOption = &aControls.element[0];
        *ppOption = CreateOption(sText, (PFNGUIEVENT)OnAddPowerPicked, pProto);
    }
    {
        CExoString sText("Add Skill");
        CSWGuiControl** ppOption = &aControls.element[1];
        *ppOption = CreateOption(sText, (PFNGUIEVENT)OnAddSkillPicked, pProto);
    }
    {
        CExoString sText("Add Feat");
        CSWGuiControl** ppOption = &aControls.element[2];
        *ppOption = CreateOption(sText, (PFNGUIEVENT)OnAddFeatPicked, pProto);
    }
    pList->AddControls(aControls, 1, 0, 0);
    pList->m_bLooping = 1;
    SetActiveControl(pList, 0);
    m_nMenu = MENU_MAIN;
    m_lbOptions.SetActiveControl(m_lbOptions.GetControl(m_nSelected), 0);
}
// PC 0x0073DAC0
CSWGuiPowersFeatsSkillsDebugMenu::CSWGuiPowersFeatsSkillsDebugMenu(
    CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    StartLoadFromLayout("debug_p");
    InitControl(m_lbOptions, "LB_OPTIONS", 1);
    InitControl(m_lblBuild, "LBL_BUILD", 1);
    StopLoadFromLayout();

    CExoString sBuild = CExoString("Build: ") + g_sBuildNumber;

    m_lblBuild.m_cText.m_cParams.SetText(sBuild);
    CreateMainPowersFeatsSkillsMenu();
    CenterPanel();
}
// PC 0x0073DCA0
void CSWGuiPowersFeatsSkillsDebugMenu::Back()
{
    if (m_nMenu == MENU_MAIN)
    {
        g_pAppManager->m_pClientExoApp->GetInGameGui()
            ->HidePowersFeatsSkillsDebugMenu();
        g_pAppManager->m_pClientExoApp->GetInGameGui()->ShowDebugMenu();
        return;
    }
    CreateMainPowersFeatsSkillsMenu();
}
// PC 0x0073DCE0
void CSWGuiPowersFeatsSkillsDebugMenu::HandleInputEvent(int nEvent, int nState)
{
    if (nState != 0)
    {
        switch (nEvent)
        {
        case 49:
        case 61:
            if (m_nSelected > 0)
            {
                m_nSelected--;
            }
            else if (m_nSelected == 0)
            {
                m_nSelected = m_nNumOptions - 1;
            }
            if (m_pManager != NULL)
            {
                m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
            }
            break;
        case 50:
        case 62:
            if (m_nSelected < m_nNumOptions)
            {
                m_nSelected++;
            }
            else if (m_nSelected == m_nNumOptions - 1)
            {
                m_nSelected = 0;
            }
            if (m_pManager != NULL)
            {
                m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
            }
            break;
        case 40:
        case 46:
            Back();
            break;
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
// PC 0x0073D1C0
CSWGuiControl* CSWGuiPowersFeatsSkillsDebugMenu::CreateOption(
    const CExoString& sText, PFNGUIEVENT pHandler, CSWGuiButton* pPrototype)
{
#ifdef _XBOX
    CSWGuiButton* pButton = NULL;
    CSWGuiExtent cExtent;
    CSWGuiBorderParams* pBorder;
    CSWGuiBorderParams* pHilite;
    int nBorderDim;
    int nHiliteDim;

    cExtent.m_nLeft = cExtent.m_nTop = cExtent.m_nWidth = cExtent.m_nHeight = 0;
    pButton = new CSWGuiButton;
    pButton->Initialize(cExtent, pPrototype);
    pButton->m_cText.m_cParams.SetText(sText);
    pHilite = &pButton->m_cHilite.m_cParams;
    pBorder = &pButton->m_cBorder.m_cParams;
    nBorderDim = pBorder->GetBorderDim();
    nHiliteDim = pHilite->GetBorderDim();
    if (nHiliteDim > nBorderDim)
    {
        nBorderDim = pHilite->GetBorderDim();
    }
    else
    {
        nBorderDim = pBorder->GetBorderDim();
    }
    cExtent.m_nHeight = pButton->m_cText.GetIdealHeight() + 2 * nBorderDim;
    pButton->SetExtent(cExtent);
    pButton->AddEvent(GUIEVENT_ACTIVATE, this, pHandler);
    pButton->AddEvent(GUIEVENT_ACCEPT, this, pHandler);
    return pButton;
#else
    CSWGuiExtent cExtent = pPrototype->m_cExtent;
    CSWGuiButton* pButton = new CSWGuiButton;
    pButton->Initialize(cExtent, pPrototype);
    pButton->m_cText.m_cParams.SetText(sText);
    pButton->AddEvent(GUIEVENT_ACTIVATE, this, pHandler);
    pButton->AddEvent(GUIEVENT_ACCEPT, this, pHandler);
    return pButton;
#endif
}

static inline void SwguipowersfeatsskillsdebugmenuTrace2()
{
    SwguipowersfeatsskillsdebugmenuTrace1();
}

static inline void SwguipowersfeatsskillsdebugmenuTrace1()
{
}
