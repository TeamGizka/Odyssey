#include "swguiDebugLoadModule.h"

#include "appmanager.h"
#include "clientexoapp.h"
#include "guiingame.h"
#include "serverexoapp.h"
#include "exotlktable.h"
#include "swmain.h"

static inline void SwguiDebugLoadModuleTrace2();
static inline void SwguiDebugLoadModuleTrace1();
#define SWGUIDEBUGLOADMODULE_TRACE() SwguiDebugLoadModuleTrace2()
// PC 0x0073DF70
CSWGuiControl* CSWGuiLoadModuleDebugMenu::CreateOption(
    const CExoString& sText, PFNGUIEVENT pHandler, CSWGuiButton* pPrototype)
{
#ifdef _XBOX
    CSWGuiExtent cExtent;
    CSWGuiButton* pButton;
    CSWGuiBorderParams* pBorder;
    CSWGuiBorderParams* pHilite;
    int nBorderDim;
    int nMaxDim;

    cExtent.m_nHeight = 0;
    cExtent.m_nWidth = 0;
    cExtent.m_nTop = 0;
    cExtent.m_nLeft = 0;
    pButton = new CSWGuiButton();
    pButton->Initialize(cExtent, pPrototype);
    pButton->m_cText.m_cParams.SetText(sText);
    pHilite = &pButton->m_cHilite.m_cParams;
    pBorder = &pButton->m_cBorder.m_cParams;
    nBorderDim = pBorder->GetBorderDim();
    if (pHilite->GetBorderDim() > nBorderDim)
    {
        nMaxDim = pHilite->GetBorderDim();
    }
    else
    {
        nMaxDim = pBorder->GetBorderDim();
    }
    cExtent.m_nHeight = pButton->m_cText.GetIdealHeight() + 2 * nMaxDim;
    pButton->SetExtent(cExtent);
    pButton->AddEvent(45, this, pHandler);
    pButton->AddEvent(GUIEVENT_ACTIVATE, this, pHandler);
    return pButton;
#else
    CSWGuiExtent cExtent = pPrototype->m_cExtent;
    CSWGuiButton* pButton = new CSWGuiButton;
    pButton->Initialize(cExtent, pPrototype);
    pButton->m_cText.m_cParams.SetText(sText);
    pButton->AddEvent(45, this, pHandler);
    pButton->AddEvent(GUIEVENT_ACTIVATE, this, pHandler);
    return pButton;
#endif
}
// PC 0x0073DDC0
void CSWGuiLoadModuleDebugMenu::Back()
{
    SwguiDebugLoadModuleTrace1();
    g_pAppManager->m_pClientExoApp->GetInGameGui()
        ->HideLoadModuleDebugMenu();
    g_pAppManager->m_pClientExoApp->GetInGameGui()
        ->ShowDebugMenu();
}
// PC 0x0073DDF0
void CSWGuiLoadModuleDebugMenu::OnModulePicked(CSWGuiControl* pControl)
{
    SwguiDebugLoadModuleTrace1();
    if (pControl->m_bSelected)
    {
        g_pAppManager->m_pClientExoApp->GetInGameGui()
            ->HideLoadModuleDebugMenu();
        CExoString sModule(
            ((CSWGuiButton*)pControl)->m_cText.m_cParams.m_sText);
        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
        pServer->SetMoveToModuleString(sModule);
        g_pAppManager->m_pServerExoApp->SetMoveToModulePending(1);
    }
}
// PC 0x0073E030
CSWGuiLoadModuleDebugMenu::CSWGuiLoadModuleDebugMenu(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    m_nSelection = 0;
    StartLoadFromLayout("debug_p");
    InitControl(m_CSWGuiListBox_64, "LB_OPTIONS", 1);
    InitControl(m_lblBuild, "LBL_BUILD", 1);
    StopLoadFromLayout();
    CExoString sBuild = CExoString("Build: ") + g_sBuildNumber;
    m_lblBuild.m_cText.m_cParams.SetText(sBuild);

    CExoArrayList<CSWGuiControl*> aControls;
    CSWGuiControl* pProto = m_CSWGuiListBox_64.m_pProtoItem;
    CExoString sModule;
    CExoString sRim;
    CExoString sMod;
    CExoArrayList<CExoString> aRims;
    CExoArrayList<CExoString> aModules;
    CExoArrayList<CExoString> aUnusedRims;
    CExoArrayList<CExoString> aUnusedModules;
    int nRim = 0;
    int nModule = 0;
    CExoString sModulePath;
    g_pExoBase->GetDirectoryList(&aModules, "MODULES:", RESTYPE_MOD, 0, 1);
    g_pExoBase->GetDirectoryList(&aRims, "MODULES:", RESTYPE_RIM, 0, 1);
    CExoString sAlias;
    for (int i = 1; i < CTlkTable::NUM_CONTENT_PKGS; i++)
    {
        sAlias.Format("live%d", i);
        if (g_pExoBase->m_pExoAliasList->GetAliasPath(sAlias, 0).GetLength() > 0)
        {
            sAlias = sAlias + ":modules";
            g_pExoBase->GetDirectoryList(&aModules, sAlias, RESTYPE_MOD, 0, 1);
            g_pExoBase->GetDirectoryList(&aRims, sAlias, RESTYPE_RIM, 0, 1);
        }
    }
    int nRims = aRims.num;
    int nModules = aModules.num;
    while (nRim < nRims || nModule < nModules)
    {
        if (nRim >= nRims)
        {
            sRim = CExoString("zzzzzzzz.zzz");
        }
        else
        {
            sRim = aRims.element[nRim];
            int nFound = sRim.Find(CExoString("_s.rim"), 0);
            if (nFound != -1)
            {
                nRim++;
                continue;
            }
        }
        if (nModule >= nModules)
        {
            sMod = CExoString("zzzzzzzz.zzz");
        }
        else
        {
            sMod = aModules.element[nModule];
            int nFound = sMod.Find(CExoString("_s.rim"), 0);
            if (nFound != -1)
            {
                nRim++;
                continue;
            }
        }
        sRim = sRim.SubString(0, sRim.GetLength() - 4);
        sMod = sMod.SubString(0, sMod.GetLength() - 4);
        if (sRim.LowerCase() <= sMod.LowerCase())
        {
            sModule = sRim;
            nRim++;
            if (sRim == sMod)
            {
                nModule++;
            }
        }
        else
        {
            sModule = sMod;
            nModule++;
        }
        CSWGuiControl* pOption = CreateOption(sModule,
                                             (PFNGUIEVENT)OnModulePicked,
                                             (CSWGuiButton*)pProto);
        aControls.Add(pOption);
    }
    CSWGuiListBox* pList = &m_CSWGuiListBox_64;
    pList->AddControls(aControls, 1, 0, 0);
    pList->m_bLooping = 1;
    SetActiveControl(pList, 0);
    CenterPanel();
}
// PC 0x0073DE90
void CSWGuiLoadModuleDebugMenu::HandleInputEvent(int nEvent, int nState)
{
    SWGUIDEBUGLOADMODULE_TRACE();
    if (nState != 0)
    {
        switch (nEvent)
        {
        case 49:
        case 61:
        {
            int nSelection = m_nSelection;
            if (nSelection > 0)
            {
                m_nSelection = nSelection - 1;
            }
            else if (nSelection == 0)
            {
                m_nSelection = m_nNumOptions - 1;
            }
            if (m_pManager != NULL)
            {
                m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
            }
            break;
        }

        case 50:
        case 62:
        {
            int nSelection = m_nSelection;
            int nOptions = m_nNumOptions;
            if (nSelection < nOptions)
            {
                m_nSelection = nSelection + 1;
            }
            else if (nSelection == nOptions - 1)
            {
                m_nSelection = 0;
            }
            if (m_pManager != NULL)
            {
                m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
            }
            break;
        }

        case 40:
        case 46:
            Back();
            break;
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}

static inline void SwguiDebugLoadModuleTrace2()
{
    SwguiDebugLoadModuleTrace1();
}

static inline void SwguiDebugLoadModuleTrace1()
{
}
