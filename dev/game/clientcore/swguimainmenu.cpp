#include "swguimainmenu.h"
#include "swguimusic.h"
#include "aurcamera.h"
#include "nwccreature.h"
#include "partytables.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "exoresman.h"
#include "exobase.h"
#include "exotlktable.h"
#include "netlayer.h"
#include "nwmessage.h"
#include "serverexoapp.h"
#include "nwsvartable.h"
#include "worldtimer.h"
#include "guiingame.h"
#include "swguiclassselection.h"
#include "swguisaveload.h"
#include "swguimainmenu.h"
#include "swguioptions.h"
#include "swguiportraitchargen.h"
#include "nwcanimbase.h"
#include "aurscene.h"
#include "aurmodel.h"
#include "clientoptions.h"
#include "nwccreaturestats.h"
#include "nwccreatureappearance.h"

static inline void SwguimainmenuTrace3();
static inline void SwguimainmenuTrace2();
static inline void SwguimainmenuTrace1();
#define SWGUIMAINMENU_TRACE() SwguimainmenuTrace3()

#define SWGUIMAINMENU_HOLD() SwguimainmenuTrace1()

#ifndef _XBOX
static int s_nPreCacheStep = 0;

void CSWGuiMainMenu::Draw(float fElapsed)  // PC 0x006D0C10
{
    CSWGuiPanel::Draw(fElapsed);
    int nDrawCount = m_nDrawCount;
    if (nDrawCount > 4)
    {
        m_bReady = 1;
        if (s_nPreCacheStep < 3)
        {
            if (s_nPreCacheStep == 0)
            {
                CExoString sCombat("HD0:combat");
                g_pExoResMan->CacheResource(sCombat, RESTYPE_ERF);
            }
            else if (s_nPreCacheStep == 1)
            {
                CExoString sFootsteps("HD0:footsteps");
                g_pExoResMan->CacheResource(sFootsteps, RESTYPE_ERF);
            }
            else if (s_nPreCacheStep == 2)
            {
                CExoString sWeapons("HD0:weapons");
                g_pExoResMan->CacheResource(sWeapons, RESTYPE_ERF);
            }
            s_nPreCacheStep++;
        }
        return;
    }
    m_nDrawCount = ++nDrawCount;
}
#endif

#ifndef _XBOX
void CSWGuiMainMenu::LoadFromLayout(CSWGuiManager* pManager)  // PC 0x006D08A0
{
    SWGUIMAINMENU_HOLD();
    CExoString sLayout;
    sLayout = "mainmenu8x6_p";
    if (sLayout.GetLength() > 0)
    {
        CResRef cLayout(sLayout);
        StartLoadFromLayout(cLayout);
        m_bPositioned = 1;
    }
    else
    {
        CResRef cLayout("mainmenu_p");
        StartLoadFromLayout(cLayout);
        m_bPositioned = 0;
    }
    InitControl(m_lbModules, "LB_MODULES", 1);
    InitControl(m_btnExit, "BTN_EXIT", 1);
    InitControl(m_btnWarp, "BTN_WARP", 1);
    InitControl(m_view3D, "LBL_3DVIEW", 1);
    InitControl(m_btnNewGame, "BTN_NEWGAME", 1);
    InitControl(m_btnLoadGame, "BTN_LOADGAME", 1);
    InitControl(m_btnMovies, "BTN_MOVIES", 1);
    InitControl(m_btnMusic, "BTN_MUSIC", 1);
    InitControl(m_btnOptions, "BTN_OPTIONS", 1);
    InitControl(m_lblNewContent, "LBL_NEWCONTENT", 1);
    InitControl(m_alblBackdrop[0], "LBL_GAMELOGO", 1);
    InitControl(m_alblBackdrop[1], "LBL_MENUBG", 1);
    StopLoadFromLayout();
}
#endif
// PC 0x006D0D70
void CSWGuiMainMenu::OnNewGamePicked(CSWGuiControl* pControl)
{
    SWGUIMAINMENU_HOLD();
#ifndef _XBOX
    if (m_nExitState != EXITSTATE_DELETE)
    {
        if (m_bReady)
#endif
        {
#ifdef _XBOX
            int bSelected = pControl->m_bSelected;
            if (bSelected)
#else
            if (pControl->m_bSelected)
#endif
            {
                CSWPartyTable::ResetCurrentSessionStartTime();
#ifndef _XBOX
                g_pAppManager->CreateServer(0);
#endif
                CExoIni cIni;
#ifdef _XBOX
                CExoString sStartModule;
                CExoString sModule;
                if (cIni.ReadIniEntry(sStartModule, "d:\\swplayer.ini", "Game Options",
                                      "NewCharStartLevel"))
                {
                    sModule = sStartModule;
                }
                else
                {
                    sModule = "END_M01AA";
                }
#else
                CExoString sUnused;
                CExoString sModule("001ebo");
                {
                    CExoString sEffects("HD0:effects");
                    g_pExoResMan->CacheResource(sEffects, RESTYPE_ERF);
                }
#endif
                g_pExoResMan->AddResourceDirectory("MODULES:");
                if (!g_pExoResMan->Exists(sModule, RESTYPE_MOD, (unsigned long*)0) &&
                    !g_pExoResMan->Exists(sModule, RESTYPE_RIM, (unsigned long*)0))
                {
#ifdef _XBOX
                    sModule = "END_M01AA";
#else
                    sModule = "001ebo";
#endif
                }
                g_pExoResMan->RemoveResourceDirectory("MODULES:");
                CSWGuiClassSelection* pSelection =
                    new CSWGuiClassSelection(m_pManager, sModule);
                m_pManager->AddPanel(pSelection, CSWGuiManager::PANEL_MODE_FOREGROUND, 1);
                g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_GENERIC);
                m_bDeferredRemoval = 0;
                m_nExitState = EXITSTATE_DELETE;
#ifndef _XBOX
                g_pAppManager->m_pClientExoApp->ClearMainMenuPointer();
#endif
            }
#ifndef _XBOX
            if (g_pAppManager->m_pClientExoApp->GetInGameGui() != NULL)
                g_pAppManager->m_pClientExoApp->GetInGameGui()->ResetTutorialWindowTracker();
#endif
        }
#ifndef _XBOX
    }
#endif
}
// PC 0x006D0FF0
void CSWGuiMainMenu::OnLoadSaveGame(CSWGuiControl* pControl)
{
    SWGUIMAINMENU_HOLD();
#ifndef _XBOX
    if (m_nExitState != EXITSTATE_DELETE)
    {
        if (m_bReady)
#endif
        {
            {
                CExoString sEffects("HD0:effects");
                g_pExoResMan->CacheResource(sEffects, RESTYPE_ERF);
            }
#ifdef _XBOX
            int bSelected = pControl->m_bSelected;
            if (bSelected)
#else
            if (pControl->m_bSelected)
#endif
            {
                m_bIdleTimerDisabled = 1;
                CSWGuiSaveLoad* pSaveLoad = new CSWGuiSaveLoad(GetManager(), 0, 1);
                GetManager()->AddPanel(pSaveLoad, CSWGuiManager::PANEL_MODE_FOREGROUND, 1);
                if (pSaveLoad->m_nExitState != EXITSTATE_DELETE)
                {
                    m_bDeferredRemoval = 0;
                    m_nExitState = EXITSTATE_DELETE;
                    g_pAppManager->m_pClientExoApp->ClearMainMenuPointer();
                }
            }
        }
#ifndef _XBOX
    }
#endif
}
// PC 0x006D1110
void CSWGuiMainMenu::OnMoviesPicked(CSWGuiControl* pControl)
{
    SWGUIMAINMENU_HOLD();
    CSWGuiTitleMovies* pMovies;
    CSWGuiTitleMovies* pNewMovies;
#ifndef _XBOX
    if (m_nExitState != EXITSTATE_DELETE)
    {
        if (m_bReady)
#endif
        {
#ifdef _XBOX
            int bSelected = pControl->m_bSelected;
            if (bSelected)
#else
            if (pControl->m_bSelected)
#endif
            {
#ifdef _XBOX
                g_pAppManager->GetClientExoApp()->BusyWaitMenuMusic();
                pMovies = new CSWGuiTitleMovies(GetManager());
#else
                g_pAppManager->m_pClientExoApp->BusyWaitMenuMusic();
                m_bIdleTimerDisabled = 1;
                pNewMovies = new CSWGuiTitleMovies(m_pManager);
                if (pNewMovies != 0)
                {
                    pMovies = pNewMovies;
                }
                else
                {
                    pMovies = 0;
                }
#endif
                m_pManager->AddPanel(pMovies, CSWGuiManager::PANEL_MODE_MODAL, 1);
            }
        }
#ifndef _XBOX
    }
#endif
}
// PC 0x006D11C0
void CSWGuiMainMenu::OnMusicPicked(CSWGuiControl* pControl)
{
    SWGUIMAINMENU_HOLD();
    if (pControl->m_bSelected)
    {
        g_pAppManager->m_pClientExoApp->BusyWaitMenuMusic();
        m_bIdleTimerDisabled = 1;
        CSWGuiTitleMusic* pMusic = new CSWGuiTitleMusic(m_pManager);
        m_pManager->AddPanel(pMusic, CSWGuiManager::PANEL_MODE_MODAL, 1);
    }
}
// PC 0x006D1250
void CSWGuiMainMenu::OnOptionsPicked(CSWGuiControl* pControl)
{
    SWGUIMAINMENU_HOLD();
    CSWGuiOptionsMain* pOptions;
#ifdef _XBOX
    CSWGuiOptionsMain* pNewOptions;
#endif
#ifndef _XBOX
    if (m_nExitState != EXITSTATE_DELETE)
    {
        if (m_bReady)
#endif
        {
#ifdef _XBOX
            int bSelected = pControl->m_bSelected;
            if (bSelected)
#else
            if (pControl->m_bSelected)
#endif
            {
                m_bIdleTimerDisabled = 1;
#ifdef _XBOX
                pNewOptions = new CSWGuiOptionsMain(GetManager());
                if (pNewOptions != 0)
                {
                    pOptions = pNewOptions;
                }
                else
                {
                    pOptions = 0;
                }
#else
                pOptions = new CSWGuiOptionsMain(GetManager());
#endif
                m_pManager->AddPanel(pOptions, CSWGuiManager::PANEL_MODE_MODAL, 1);
            }
        }
#ifndef _XBOX
    }
#endif
}
// PC 0x006D12F0
void CSWGuiMainMenu::HandleInputEvent(int nEvent, int nState)
{
    SWGUIMAINMENU_HOLD();
    if (nState != 0)
    {
        switch (nEvent)
        {
        case 40:
        case 46:
            if (m_nMenuState == 2)
            {
                m_lbModules.SetEnabledFlag(0);
                m_view3D.SetEnabledFlag(1);
                m_btnLoadGame.SetEnabledFlag(1);
                m_btnMovies.SetEnabledFlag(1);
#ifndef _XBOX
                m_btnMusic.SetEnabledFlag(1);
#endif
                m_btnNewGame.SetEnabledFlag(1);
                m_btnOptions.SetEnabledFlag(1);
#ifdef _XBOX
                m_lblWarp.SetEnabledFlag(1);
                m_lblY.SetEnabledFlag(1);
#else
                m_btnExit.SetEnabledFlag(1);
                m_btnWarp.SetEnabledFlag(1);
#endif
                m_alblBackdrop[0].SetEnabledFlag(1);
                m_alblBackdrop[1].SetEnabledFlag(1);
                SetActiveControl(&m_btnNewGame, 0);
                m_nMenuState = 1;
            }
            break;
        case 45:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            break;
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
// PC 0x006D13D0
void CSWGuiMainMenu::OnEnterButton(CSWGuiControl* pControl)
{
    SWGUIMAINMENU_HOLD();
#ifdef _XBOX
    ((CSWGuiButton*)pControl)
        ->m_cText.m_cParams.SetColor(CGuiInGame::COLOR_YELLOW);
#else
    ((CSWGuiButton*)pControl)
        ->m_cText.m_cParams.SetColor(CGuiInGame::COLOR_NEW_HIGHLIGHT);
#endif
}

void CSWGuiMainMenu::OnExitButton(CSWGuiControl* pControl)
{
}

#ifdef _XBOX

#define SAVE_GAME_DRIVE 'U'
#define SAVE_GAME_BLOCKS_NEEDED 3202
void CSWGuiMainMenu::LaunchDashboard(CSWGuiControl* pControl)
{
    LD_LAUNCH_DASHBOARD cLaunchData;

    if (!g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox->m_bAccepted)
    {
        cLaunchData.dwReason = XLD_LAUNCH_DASHBOARD_MEMORY;
        cLaunchData.dwContext = 0;
        cLaunchData.dwParameter1 = SAVE_GAME_DRIVE;
        cLaunchData.dwParameter2 = SAVE_GAME_BLOCKS_NEEDED;
        XLaunchNewImage(NULL, (PLAUNCH_DATA)&cLaunchData);
    }
}
#else
void CSWGuiMainMenu::LaunchDashboard(CSWGuiControl* pControl)  // PC 0x006D13F0
{
    SWGUIMAINMENU_HOLD();
    g_pAppManager->m_pClientExoApp->ExitProgram();
}
// PC 0x006D1400
void CSWGuiMainMenu::OnQuitButtonPressed(CSWGuiControl* pControl)
{
    SWGUIMAINMENU_HOLD();
#ifndef _XBOX
    if (m_bReady)
#endif
    {
        if (m_nMenuState == 2)
        {
            HandleInputEvent(GUIEVENT_CANCEL_ALTERNATE, 1);
            return;
        }
        g_pAppManager->m_pClientExoApp->ExitProgram();
    }
}
#endif
// PC 0x006D1740
CSWGuiMainMenu::~CSWGuiMainMenu()
{
#ifndef _XBOX
    if (m_pCreature != NULL)
    {
        delete m_pCreature;
        m_pCreature = NULL;
    }
#endif
    m_lbModules.ClearItems();
#ifdef _XBOX
    if (g_pExoResMan->Exists("MAINMENU", RESTYPE_RIM, (unsigned long*)0))
#else
    CResRef cMainMenu("MAINMENU");
    if (g_pExoResMan->Exists(cMainMenu, RESTYPE_RIM, (unsigned long*)0))
#endif
    {
        g_pExoResMan->m_bMainMenuImageLoaded = 1;
    }
}

#ifdef _XBOX

static int s_bSaveSpaceChecked = 0;

inline void CSWGuiMainMenu::OnPanelAdded()
{
    unsigned __int64 nFreeBytes;
    g_pExoResMan->GetFreeDiskSpace("SAVES:", &nFreeBytes);
    __int64 nFreeUnits = nFreeBytes >> 14;
    if (!s_bSaveSpaceChecked && nFreeUnits < 3202)
    {
        int nNeededUnits = abs((int)nFreeUnits - 3202);
        s_bSaveSpaceChecked = 1;
        CErrorMessageData cMessage;
        {
            CExoString sNeeded(nNeededUnits);
            g_pTlkTable->SetCustomToken(0, sNeeded);
        }
        cMessage.m_sMessage = g_pTlkTable->GetSimpleString(42141);
        g_pTlkTable->ParseStr(cMessage.m_sMessage);
        cMessage.m_nUnknown08 = 1;
        cMessage.m_nUnknown0C = 1;
        cMessage.m_pCallbackObject = this;
        cMessage.m_pfnCallback = (CSWGuiHandler)&CSWGuiMainMenu::LaunchDashboard;
        AddErrorMessage(cMessage);
    }
}
#else
inline void CSWGuiMainMenu::OnPanelAdded()  // PC 0x006D14C0
{
    unsigned __int64 nFreeBytes;
    g_pExoResMan->GetFreeDiskSpace("SAVES:", &nFreeBytes);
    __int64 nFreeUnits = nFreeBytes >> 14;
    __int64 nNeededUnits = 3200;
    CExoString sAutoSave;
    sAutoSave.Format("SAVES:%06d - %s", 1, "AUTOSAVE");
    int bCreated = g_pExoResMan->CreateDirectory2(sAutoSave) == 0;
    if (!bCreated)
    {
        nNeededUnits = 4800;
        g_pExoResMan->NukeDirectory(sAutoSave, 1, 1);
    }
    nNeededUnits += 2;
    if (nNeededUnits > nFreeUnits)
    {
        float fNeededMB =
            (float)(int)((nNeededUnits - nFreeUnits) << 14) * 0.000001f;
        CGuiInGame* pInGame = g_pAppManager->m_pClientExoApp->GetInGameGui();
        CSWGuiMessageBox* pMessageBox = pInGame->m_pMessageBox;
        CExoString sMessage;
        CExoString sNeeded;
        sNeeded.Format("%2.3f", fNeededMB);
        g_pTlkTable->SetCustomToken(0, sNeeded);
        sMessage = g_pTlkTable->GetSimpleString(48737);
        g_pTlkTable->ParseStr(sMessage);
        pMessageBox->SetAllowCancel(0);
        pMessageBox->SetMessage(sMessage);
        pMessageBox->SetCallback(
            this, (CSWGuiHandler)&CSWGuiMainMenu::LaunchDashboard, 0);
        m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
    }
    else
    {
        g_pAppManager->m_pClientExoApp->ShowMouse(MOUSE_HIDE_ALL);
        return;
    }
}
#endif
// PC 0x006D1A00
void CSWGuiMainMenu::OnModulePicked(CSWGuiControl* pControl)
{
    int bSelected = pControl->m_bSelected;
    if (bSelected != 0)
    {
        CExoString sMessage;
        CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
        CSWPartyTable::ResetCurrentSessionStartTime();
        g_pAppManager->CreateServer(0);
        g_pAppManager->m_pClientExoApp->GetWorldTimer()->PauseWorldTimer();
        g_pAppManager->m_pServerExoApp->GetWorldTimer()->PauseWorldTimer();
#ifndef _XBOX
        char nPCLevel;
        g_pAppManager->GetServerExoApp()->GetGlobalVariableTable()
            ->GetValueNumber("G_PC_LEVEL", nPCLevel);
        if (nPCLevel == 0)
        {
            g_pAppManager->GetServerExoApp()->GetGlobalVariableTable()
                ->SetValueNumber("G_PC_LEVEL", 12);
            g_pAppManager->GetServerExoApp()->GetGlobalVariableTable()
                ->SetValueNumber("G_PC_Align_Val", 70);
        }
#endif
        pClientExoApp->GetNetLayer()->StartConnectToSession(
            0, pClientExoApp->GetClientName(), "", 10,
            CNetLayer::CONNECTTYPE_PLAYER);
        pClientExoApp->GetSWCMessage()
            ->SendPlayerToServerStatus_RequestStatus();
        sMessage.Format("%c%s.%s %s", 's', "Module", "Load",
                        m_lstModules.element[m_lbModules.m_nSelectedItem].CStr());
        pClientExoApp->GetNetLayer()->SendMessageToPlayer(
            CNetLayer::PLAYERID_SERVER, (unsigned char*)sMessage.CStr(),
            sMessage.GetLength(), 0);
        pClientExoApp->SetPlayerCharacterName("test");
        pClientExoApp->SetInputClass(INPUT_CLASS_GAME, 1);
        pClientExoApp->DisableInput();
        pClientExoApp->SetLoadMusic(pClientExoApp->GetLoadMusicByModuleName(
            m_lstModules.element[m_lbModules.m_nSelectedItem]));
        pClientExoApp->SetLoadScreenByModuleName(m_lstModules.element[m_lbModules.m_nSelectedItem]);
        pClientExoApp->ShowLoadScreen(1, 1, 0);
        pClientExoApp->SetLoadScreenHint(
            pClientExoApp->GetNextLoadScreenHintSTRREF(
                m_lstModules.element[m_lbModules.m_nSelectedItem], 0));
        DeleteModuleList();
        m_lbModules.ClearItems();
        m_bDeferredRemoval = 0;
        m_nExitState = EXITSTATE_DELETE;
#ifndef _XBOX
        g_pAppManager->m_pClientExoApp->ClearMainMenuPointer();
#endif
        g_pAppManager->m_pClientExoApp->GetInGameGui()
            ->SetGlobalUnpauseableState(1);
    }
}
// PC 0x006D1960
void CSWGuiMainMenu::DeleteModuleList()
{
    SWGUIMAINMENU_TRACE();
    while (m_lstModules.num > 0)
    {
        m_lstModules.DelIndex(0);
    }
    m_lstModules.SetSize(0);
    m_lstModules.num = 0;
}
// PC 0x006D1D50
void CSWGuiMainMenu::PopulateModules()
{
    CExoString sModule;
    CExoString sRim;
    CExoString sMod;
    CExoArrayList<CExoString> lstRims;
    CExoArrayList<CExoString> lstMods;
    CExoArrayList<CExoString> lstUnused1;
    CExoArrayList<CExoString> lstUnused2;
    CExoArrayList<CSWGuiControl*> lstControls;
    int nRim = 0;
    int nMod = 0;
    CSWGuiExtent cExtent = m_lbModules.m_cItemExtent;
    CExoString sUnused;

    g_pExoBase->GetDirectoryList(&lstMods, "MODULES:", RESTYPE_MOD, 0, 1);
    g_pExoBase->GetDirectoryList(&lstRims, "MODULES:", RESTYPE_RIM, 0, 1);

    CExoString sPath;
    for (int nPkg = 1; nPkg < CTlkTable::NUM_CONTENT_PKGS; ++nPkg)
    {
        sPath.Format("live%d", nPkg);
        if (g_pExoBase->m_pExoAliasList->GetAliasPath(sPath, 0).GetLength() > 0)
        {
            sPath = sPath + ":modules";
            g_pExoBase->GetDirectoryList(&lstMods, sPath, RESTYPE_MOD, 0, 1);
            g_pExoBase->GetDirectoryList(&lstRims, sPath, RESTYPE_RIM, 0, 1);
        }
    }

    CSWGuiControl* pProtoItem = m_lbModules.m_pProtoItem;
    int nNumRims = lstRims.num;
    int nNumMods = lstMods.num;
    while (nRim < nNumRims || nMod < nNumMods)
    {
        if (nRim >= nNumRims)
        {
            sRim = CExoString("zzzzzzzz.zzz");
        }
        else
        {
            sRim = lstRims.element[nRim];
            int nFound = sRim.Find(CExoString("_s.rim"), 0);
            if (nFound != -1)
            {
                nRim++;
                continue;
            }
        }
        if (nMod >= nNumMods)
        {
            sMod = CExoString("zzzzzzzz.zzz");
        }
        else
        {
            sMod = lstMods.element[nMod];
            int nFound = sMod.Find(CExoString("_s.rim"), 0);
            if (nFound != -1)
            {
                nMod++;
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
                nMod++;
            }
        }
        else
        {
            sModule = sMod;
            nMod++;
        }

        CSWGuiButton* pButton = new CSWGuiButton();
        pButton->Initialize(cExtent, (CSWGuiButton*)pProtoItem);
        pButton->m_cText.m_cParams.SetText(sModule);
        cExtent.m_nHeight = pButton->m_cText.GetIdealHeight() +
                            2 * pButton->m_cBorder.m_cParams.GetBorderDim();
        pButton->SetExtent(cExtent);
        pButton->AddEvent(39, this,
                          (CSWGuiHandler)&CSWGuiMainMenu::OnModulePicked);
        pButton->AddEvent(45, this,
                          (CSWGuiHandler)&CSWGuiMainMenu::OnModulePicked);
        lstControls.Add(pButton);
        m_lstModules.Add(sModule);
    }

    m_lbModules.AddControls(lstControls, 1, 0, 0);
}
// PC 0x006D2500
void CSWGuiMainMenu::OnLoadGamePicked()
{
#ifndef _XBOX
    if (m_nExitState != EXITSTATE_DELETE)
    {
        if (m_bReady)
#endif
        {
#ifndef _XBOX
            {
                CExoString sEffects("HD0:effects");
                g_pExoResMan->CacheResource(sEffects, RESTYPE_ERF);
            }
#endif
            if (m_lbModules.m_lItems.num == 0)
            {
                PopulateModules();
            }
            m_lbModules.SetEnabledFlag(1);
            m_view3D.SetEnabledFlag(0);
            m_btnLoadGame.SetEnabledFlag(0);
            m_btnMovies.SetEnabledFlag(0);
#ifndef _XBOX
            m_btnMusic.SetEnabledFlag(0);
#endif
            m_btnNewGame.SetEnabledFlag(0);
            m_btnOptions.SetEnabledFlag(0);
#ifdef _XBOX
            m_lblWarp.SetEnabledFlag(0);
            m_lblY.SetEnabledFlag(0);
#else
            m_btnExit.SetEnabledFlag(0);
            m_btnWarp.SetEnabledFlag(0);
#endif
            m_alblBackdrop[0].SetEnabledFlag(0);
            m_alblBackdrop[1].SetEnabledFlag(0);
            SetActiveControl(&m_lbModules, 0);
            m_lbModules.SetActiveControl(m_lbModules.GetControl(0), 0);
            m_nMenuState = 2;
        }
#ifndef _XBOX
    }
#endif
}
// PC 0x006D2640
void CSWGuiMainMenu::OnWarpButtonPressed(CSWGuiControl* pControl)
{
    OnLoadGamePicked();
}
// PC 0x006D2650
CSWGuiMainMenu::CSWGuiMainMenu(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
#ifdef _XBOX
    if (g_pExoResMan->Exists("MAINMENU", RESTYPE_RIM, (unsigned long*)0))
    {
        g_pExoResMan->AddResourceImageFile("RIMSXBOX:MAINMENU");
    }
    if (g_pExoResMan->Exists("MAINMENUDX", RESTYPE_RIM, (unsigned long*)0))
    {
        g_pExoResMan->AddResourceImageFile("RIMSXBOX:MAINMENUDX");
    }
#else
    {
        CResRef cMainMenu("MAINMENU");
        if (g_pExoResMan->Exists(cMainMenu, RESTYPE_RIM, (unsigned long*)0))
        {
            g_pExoResMan->AddResourceImageFile("RIMS:MAINMENU");
        }
    }
#endif
    g_pExoResMan->m_bMainMenuImageLoaded = 0;
#ifdef _XBOX
    StartLoadFromLayout("mainmenu");
    InitControl(m_lbModules, "LB_MODULES", 1);
    InitControl(m_view3D, "LBL_3DVIEW", 1);
    InitControl(m_btnNewGame, "BTN_NEWGAME", 1);
    InitControl(m_btnLoadGame, "BTN_LOADGAME", 1);
    InitControl(m_btnMovies, "BTN_MOVIES", 1);
    InitControl(m_btnOptions, "BTN_OPTIONS", 1);
    InitControl(m_lblNewContent, "LBL_NEWCONTENT", 1);
    InitControl(m_lblWarp, "LBL_WARP", 1);
    InitControl(m_lblY, "LBL_Y", 1);
    InitControl(m_alblBackdrop[0], "LBL_GAMELOGO", 1);
    InitControl(m_alblBackdrop[1], "LBL_MENUBG", 1);
    StopLoadFromLayout();
    m_lblY.SetEnabledFlag(0);
    m_lblWarp.SetEnabledFlag(0);
#else
    LoadFromLayout(pManager);
    m_lbl12C4.SetEnabledFlag(0);
    m_lbl140C.SetEnabledFlag(0);
    m_btnWarp.SetEnabledFlag(0);
#endif
    m_lblNewContent.SetEnabledFlag(0);
    m_lbModules.SetEnabledFlag(0);
    m_btnNewGame.AddEvent(39, this,
                          (CSWGuiHandler)&CSWGuiMainMenu::OnNewGamePicked);
    m_btnLoadGame.AddEvent(39, this,
                           (CSWGuiHandler)&CSWGuiMainMenu::OnLoadSaveGame);
    m_btnMovies.AddEvent(39, this,
                         (CSWGuiHandler)&CSWGuiMainMenu::OnMoviesPicked);
    m_btnMusic.AddEvent(39, this,
                        (CSWGuiHandler)&CSWGuiMainMenu::OnMusicPicked);
    m_btnOptions.AddEvent(39, this,
                          (CSWGuiHandler)&CSWGuiMainMenu::OnOptionsPicked);
#ifndef _XBOX
    m_btnWarp.AddEvent(39, this,
                       (CSWGuiHandler)&CSWGuiMainMenu::OnWarpButtonPressed);
    m_btnExit.AddEvent(39, this,
                       (CSWGuiHandler)&CSWGuiMainMenu::OnQuitButtonPressed);
#endif
    m_btnNewGame.AddEvent(45, this,
                          (CSWGuiHandler)&CSWGuiMainMenu::OnNewGamePicked);
    m_btnLoadGame.AddEvent(45, this,
                           (CSWGuiHandler)&CSWGuiMainMenu::OnLoadSaveGame);
    m_btnMovies.AddEvent(45, this,
                         (CSWGuiHandler)&CSWGuiMainMenu::OnMoviesPicked);
    m_btnMusic.AddEvent(45, this,
                        (CSWGuiHandler)&CSWGuiMainMenu::OnMusicPicked);
    m_btnOptions.AddEvent(45, this,
                          (CSWGuiHandler)&CSWGuiMainMenu::OnOptionsPicked);
#ifndef _XBOX
    m_btnWarp.AddEvent(45, this,
                       (CSWGuiHandler)&CSWGuiMainMenu::OnWarpButtonPressed);
    m_btnExit.AddEvent(45, this,
                       (CSWGuiHandler)&CSWGuiMainMenu::OnQuitButtonPressed);
#endif
    m_btnNewGame.AddEvent(0, this,
                          (CSWGuiHandler)&CSWGuiMainMenu::OnEnterButton);
    m_btnLoadGame.AddEvent(0, this,
                           (CSWGuiHandler)&CSWGuiMainMenu::OnEnterButton);
    m_btnMovies.AddEvent(0, this,
                         (CSWGuiHandler)&CSWGuiMainMenu::OnEnterButton);
    m_btnMusic.AddEvent(0, this,
                        (CSWGuiHandler)&CSWGuiMainMenu::OnEnterButton);
    m_btnOptions.AddEvent(0, this,
                          (CSWGuiHandler)&CSWGuiMainMenu::OnEnterButton);
#ifndef _XBOX
    m_btnWarp.AddEvent(GUIEVENT_MOUSE_ENTER, this, (CSWGuiHandler)&CSWGuiMainMenu::OnEnterButton);
    m_btnExit.AddEvent(GUIEVENT_MOUSE_ENTER, this, (CSWGuiHandler)&CSWGuiMainMenu::OnEnterButton);
#endif
    m_btnNewGame.AddEvent(1, this,
                          (CSWGuiHandler)&CSWGuiMainMenu::OnExitButton);
    m_btnLoadGame.AddEvent(1, this,
                           (CSWGuiHandler)&CSWGuiMainMenu::OnExitButton);
    m_btnMovies.AddEvent(1, this, (CSWGuiHandler)&CSWGuiMainMenu::OnExitButton);
    m_btnMusic.AddEvent(1, this, (CSWGuiHandler)&CSWGuiMainMenu::OnExitButton);
    m_btnOptions.AddEvent(1, this,
                          (CSWGuiHandler)&CSWGuiMainMenu::OnExitButton);
#ifndef _XBOX
    m_btnWarp.AddEvent(1, this, (CSWGuiHandler)&CSWGuiMainMenu::OnExitButton);
    m_btnExit.AddEvent(1, this, (CSWGuiHandler)&CSWGuiMainMenu::OnExitButton);
#endif
    CAurCamera* pCamera = m_view3D.m_cScene.m_pCamera;
    CAurScene* pScene = m_view3D.m_cScene.m_pScene;
    if (g_bDo3DGUIs != 0)
    {
        pScene->SpawnRoom("gui3D_room", Vector(), Quaternion(0, 0, 0, 1));
        g_pAppManager->m_pClientExoApp->LoadOptions(1);
        int nSithLord = g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nCurSithLord;
        CExoString sModel;
        switch (nSithLord)
        {
        case 0:
            sModel = "mainmenu01";
            break;
        case 1:
            sModel = "mainmenu02";
            break;
        case 2:
            sModel = "mainmenu03";
            break;
        case 3:
            sModel = "mainmenu04";
            break;
        case 4:
            sModel = "mainmenu05";
            break;
        default:
            sModel = "mainmenu01";
            break;
        }
        CAurObject* pModel = m_view3D.m_cScene.AddModel(sModel, -1);
        if (pModel != 0)
        {
            ((Gob*)pModel)->PlayAnimation("default", 1.0f, 0, 0);
#ifndef _XBOX
            ((Gob*)pModel)->Animate(1.0f);
#endif
            pCamera->AttachToObject(pModel, "camerahook", Gob::ATTACH_NORMAL);
            pCamera->SetViewAngle(22.726f);
        }
        if (nSithLord == 4 && pModel != 0)
        {
            m_pCreature = new CSWCCreature();
            CCAppearanceInfo cAppearance = *m_pCreature->GetAppearance();
            cAppearance.m_nGender =
                g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nPCGender;
            cAppearance.m_nAppearance =
                g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nAppearanceType;
            cAppearance.m_bUseBodyVariation = 1;
            cAppearance.m_nBodyVariation =
                g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nBodyVariation;
            cAppearance.m_nTextureVar =
                g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nTextureVariation;
            m_pCreature->GetStats()->m_nAppearance = cAppearance.m_nAppearance;
            m_pCreature->SetAppearance(&cAppearance, 3, 1);
            m_pCreature->AttachToScene(pScene);
            m_pCreature->SetOrientation(Vector(0.0f, -1.0f, 0.0f));
            m_pCreature->SetPosition(((Gob*)pModel)->GetCutsceneDummyPosition());
            Gob* pGob = m_pCreature->GetAnimationBase()->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN);
            if (pGob != NULL)
                pGob->PlayAnimation("evil", 1.0f, Gob::ANIMATION_FLAG_OUT_OF_PLACE, 0.0f);
            pGob = m_pCreature->GetAnimationBase()->GetModel(CSWCAnimBase::MODEL_SLOT_HEAD);
            if (pGob != NULL)
                pGob->PlayAnimation("evil", 1.0f, Gob::ANIMATION_FLAG_OUT_OF_PLACE, 0.0f);
        }
        else
        {
            m_pCreature = NULL;
        }
    }
    SetActiveControl(&m_btnNewGame, 0);
    m_nMenuState = 1;
#ifndef _XBOX
    g_pExoResMan->RemoveFile("HD0:GAMEINPROGRESS", 0x56);
#endif
    g_pExoResMan->CleanDirectory("GAMEINPROGRESS:", 0, 0);
#ifndef _XBOX
    g_pExoResMan->RemoveFile("HD0:CURRENTGAME", 0x56);
#endif
    g_pExoResMan->CleanDirectory("CURRENTGAME:", 0, 0);
    g_pExoResMan->CleanDirectory("REBOOTDATA:", 0, 0);
    g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_GENERIC);
#ifndef _XBOX
    m_bReady = 0;
    m_nDrawCount = 0;
    m_bIdleTimerDisabled = 0;
#endif
}

static inline void SwguimainmenuTrace3()
{
    SwguimainmenuTrace2();
}

static inline void SwguimainmenuTrace2()
{
    SwguimainmenuTrace1();
}

static inline void SwguimainmenuTrace1()
{
}

#ifdef _XBOX

void CSWGuiMainMenu::OnErrorMessageDone(CSWGuiControl* pControl)
{
    if (m_lstErrorMessages[0].m_pfnCallback != NULL)
    {
        (m_lstErrorMessages[0].m_pCallbackObject->*m_lstErrorMessages[0].m_pfnCallback)(NULL);
    }
    m_lstErrorMessages.DelIndex(0);
    if (m_lstErrorMessages.num != 0)
    {
        ShowErrorMessage();
    }
}

void CSWGuiMainMenu::ShowErrorMessage()
{
    EXOXBOXASSERT(m_lstErrorMessages.num != 0);
    CGuiInGame* pInGame = g_pAppManager->m_pClientExoApp->GetInGameGui();
    CErrorMessageData* pMessage = &m_lstErrorMessages[0];
    CSWGuiMessageBox* pMessageBox = pInGame->m_pMessageBox;
    pMessageBox->SetAllowCancel(pMessage->m_nUnknown0C);
    if (pMessage->m_nUnknown08 != 0)
    {
        pMessageBox->HideButtons();
    }
    pMessageBox->SetMessage(pMessage->m_sMessage);
    pMessageBox->SetCallback(
        this, (CSWGuiHandler)&CSWGuiMainMenu::OnErrorMessageDone, 0);
    m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
}

void CSWGuiMainMenu::AddErrorMessage(const CErrorMessageData& cMessage)
{
    m_lstErrorMessages.Add(cMessage);
    if (m_lstErrorMessages.num == 1)
    {
        ShowErrorMessage();
    }
}
#endif
