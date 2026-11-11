#include <string.h>
#include <stdio.h>
#include "swmain.h"

#include "exobase.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "serverexoapp.h"
#include "clientoptions.h"
#include "guiingame.h"
#include "swguisaveload.h"
#include "nwerffile.h"
#include "exotlktable.h"
#include "partytables.h"
#include "nwsvartable.h"
#include "clientexoappinternal.h"
#include "netlayer.h"
#include "worldtimer.h"
#include "nwmessage.h"

static inline void SwguisaveloadTrace2();
static inline void SwguisaveloadTrace1();
#define SWGUISAVELOAD_TRACE() SwguisaveloadTrace1()
// PC 0x006CC0E0  Xbox 0x00115CE0
void CSWGuiSaveLoad::VerifyLoadGame(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected)
    {
#ifdef _XBOX
        CSWGuiControl* pSelected = pControl;
#else
        CSWGuiControl* pSelected = m_lbSaveGames.GetSelectedControl();
        if (pSelected != NULL)
#endif
        {
            if (m_bExitToMainMenu)
            {
                LoadGame(pSelected);
            }
            else
            {
                CSWGuiMessageBox* pMessageBox =
                    g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
                pMessageBox->SetAllowCancel(1);
                pMessageBox->SetMessage(STRREF_CONFIRM_LOAD_GAME);
                pMessageBox->SetCallback(
                    this, (CSWGuiEventHandler)&CSWGuiSaveLoad::LoadGame,
                    pSelected);
                m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
            }
        }
    }
}
// PC 0x006C9A90  Xbox 0x00110F50
int CSWGuiSaveLoad::CopyGameToFutureGame(unsigned long nSaveGame,
                                       const CExoString& sSaveName)
{

    CExoString sSavePath;
    CExoString sDirectory;
    CExoString sFileName;
    CExoString sTemp;

    if (nSaveGame != (unsigned long)-1)
    {
        sDirectory.Format("%06d - %s", nSaveGame, sSaveName.CStr());
    }
    else
    {
        sDirectory = sSaveName;
    }
    sSavePath = "SAVES:";
    sSavePath = sSavePath + sDirectory;
    g_pExoResMan->CreateDirectory2("FUTUREGAME:");
    g_pExoResMan->CleanDirectory("FUTUREGAME:", 0, 0);
    sFileName = sSavePath + "\\" + "SAVEGAME";

    CERFFile* pERF = new CERFFile;
    pERF->Read(sFileName);
    pERF->ReadHeaderVariance(sFileName);
    pERF->ExportFilesFromERF(
        sFileName, "FUTUREGAME:", (void*)-1, LoadScreenUpdate, 0,
        g_pAppManager->m_pClientExoApp->GetLoadStep(0));
    delete pERF;
    return 1;
}
// PC 0x006C9D50
void CSWGuiSaveNamePanel::HandleOkButton(CSWGuiControl* pControl)
{
    {
        CExoString sSaveName(m_edtSaveName.m_cText.m_sText);
        CSWGuiSaveLoad* pSaveLoad = m_pSaveLoad;

        pSaveLoad->WriteGame(sSaveName);
    }
    m_bDeferredRemoval = 0;
    m_nExitState = EXITSTATE_DELETE;
    m_pManager->PopModalPanel();
}

static void ConstructSaveNameEditBox()
{
    new CSWGuiSaveGameEditBox;
}
// PC 0x006C9DE0  Xbox 0x00111970
inline void KOTOR_AUTOSAVE_PARAMS::LoadFromGFF(CResGFF* pGFF, CResStruct& cStruct)
{
    int bSuccess;

    m_sLoadMusic = pGFF->ReadFieldCExoString(&cStruct, "LOADMUSIC", bSuccess,
                                            m_sLoadMusic);
    m_sStartWaypoint = pGFF->ReadFieldCExoString(&cStruct, "STARTWAYPOINT",
                                                bSuccess, m_sStartWaypoint);
    m_sMovie1 = pGFF->ReadFieldCExoString(&cStruct, "MOVIE1", bSuccess,
                                         m_sMovie1);
    m_sMovie2 = pGFF->ReadFieldCExoString(&cStruct, "MOVIE2", bSuccess,
                                         m_sMovie2);
    m_sMovie3 = pGFF->ReadFieldCExoString(&cStruct, "MOVIE3", bSuccess,
                                         m_sMovie3);
    m_sMovie4 = pGFF->ReadFieldCExoString(&cStruct, "MOVIE4", bSuccess,
                                         m_sMovie4);
    m_sMovie5 = pGFF->ReadFieldCExoString(&cStruct, "MOVIE5", bSuccess,
                                         m_sMovie5);
    m_sMovie6 = pGFF->ReadFieldCExoString(&cStruct, "MOVIE6", bSuccess,
                                         m_sMovie6);

    unsigned long nYear = pGFF->ReadFieldDWORD(&cStruct, "TIME_YEAR", bSuccess, 0);
    unsigned char nMonth = pGFF->ReadFieldBYTE(&cStruct, "TIME_MONTH", bSuccess, 0);
    unsigned char nDay = pGFF->ReadFieldBYTE(&cStruct, "TIME_DAY", bSuccess, 0);
    unsigned char nHour = pGFF->ReadFieldBYTE(&cStruct, "TIME_HOUR", bSuccess, 0);
    unsigned short nMinute = pGFF->ReadFieldWORD(&cStruct, "TIME_MINUTE", bSuccess, 0);
    unsigned short nSecond = pGFF->ReadFieldWORD(&cStruct, "TIME_SECOND", bSuccess, 0);
    unsigned short nMillisecond = pGFF->ReadFieldWORD(&cStruct, "TIME_MILLISECOND",
                                                     bSuccess, 0);
    m_nTimeYear = nYear;
    m_nTimeMonth = nMonth;
    m_nTimeDay = nDay;
    m_nTimeHour = nHour;
    m_nTimeMinute = nMinute;
    m_nTimeSecond = nSecond;
    m_nTimeMillisecond = nMillisecond;
    m_nTimePauseDay = pGFF->ReadFieldDWORD(&cStruct, "TIME_PAUSEDAY", bSuccess, 0);
    m_nTimePauseTime = pGFF->ReadFieldDWORD(&cStruct, "TIME_PAUSETIME", bSuccess, 0);

    CResStruct cStatusSummary;
    if (pGFF->GetStructFromStruct(&cStatusSummary, &cStruct, "STATUSSUMMARY"))
    {
        m_cStatusSummary.LoadFromGFF(pGFF, cStatusSummary);
    }
}
// PC 0x006CC680  Xbox 0x00116750
CSWGuiSaveLoad::CSWGuiSaveLoad(CSWGuiManager* pManager, int bSaveMode,
                             int bExitToMainMenu)
    : CSWGuiPanel(pManager)
{
    StartLoadFromLayout("saveload");
    InitControl(m_lblPanelName, "LBL_PANELNAME", 1);
    InitControl(m_lblScreenshot, "LBL_SCREENSHOT", 1);
#ifdef _XBOX
    InitControl(m_lblA, "LBL_A", 1);
    InitControl(m_lblB, "LBL_B", 1);
    InitControl(m_lblX, "LBL_X", 1);
    InitControl(m_lblSaveload, "LBL_SAVELOAD", 1);
    InitControl(m_lblBack, "LBL_BACK", 1);
    InitControl(m_lblDelete, "LBL_DELETE", 1);
#endif
    InitControl(m_lblPlanetName, "LBL_PLANETNAME", 1);
    InitControl(m_lblAreaName, "LBL_AREANAME", 1);
    InitControl(m_aPartyMemberLabels[0], "LBL_PM1", 1);
    InitControl(m_aPartyMemberLabels[1], "LBL_PM2", 1);
    InitControl(m_aPartyMemberLabels[2], "LBL_PM3", 1);
    InitControl(m_lbSaveGames, "LB_GAMES", 1);
#ifdef _XBOX
    InitControl(m_lblFreeblocks, "LBL_FREEBLOCKS", 1);
    InitControl(m_lblFreeblocksAmt, "LBL_FREEBLOCKS_AMT", 1);
#else
    InitControl(m_btnSaveLoad, "BTN_SAVELOAD", 1);
    InitControl(m_btnBack, "BTN_BACK", 1);
    InitControl(m_btnDelete, "BTN_DELETE", 1);
#endif
    StopLoadFromLayout();
#ifndef _XBOX
    m_sResourceDirectory = "";
    m_lbSaveGames.SetSelectOnLeftClick(1);
    m_btnSaveLoad.SetFocusable(0);
    m_btnSaveLoad.AddEvent(GUIEVENT_ACTIVATE, this,
                         (CSWGuiHandler)&CSWGuiPanel::OnAButtonPressed);
    m_btnBack.SetFocusable(0);
    m_btnBack.AddEvent(GUIEVENT_ACTIVATE, this,
                      (CSWGuiHandler)&CSWGuiPanel::OnBButtonPressed);
    m_btnDelete.SetFocusable(0);
    m_btnDelete.AddEvent(GUIEVENT_ACTIVATE, this,
                        (CSWGuiHandler)&CSWGuiPanel::OnXButtonPressed);
    m_btnDelete.AddEvent(GUIEVENT_ACTIVATE, this,
                        (CSWGuiHandler)&CSWGuiSaveLoad::DeleteGame);
    if (bSaveMode)
    {
        m_btnSaveLoad.AddEvent(GUIEVENT_ACTIVATE, this,
                              (CSWGuiHandler)&CSWGuiSaveLoad::HandleSaveButton);
    }
    else
    {
        m_btnSaveLoad.AddEvent(GUIEVENT_ACTIVATE, this,
                              (CSWGuiHandler)&CSWGuiSaveLoad::VerifyLoadGame);
    }
#endif
    CSWGuiButton* pProto = m_lbSaveGames.m_pProtoItem->AsButton();
    pProto->m_cBorder.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
    pProto->m_cHilite.m_cParams.SetColor(CGuiInGame::COLOR_YELLOW);
    pProto->m_cHilite.m_cParams.SetPulsingAlpha(1, 1, 0);
#ifdef _XBOX

    g_pExoBase->m_pExoAliasList->Add("SAVES", "U:\\");
    UpdateFreeBlocks();
#endif
    m_bSaveMode = bSaveMode;
    m_bExitToMainMenu = bExitToMainMenu;
#ifdef _XBOX
    m_nMaxSaveGame = 1;
#else
    m_nMaxSaveGame = 2;
#endif
    if (m_bSaveMode)
    {
#ifdef _XBOX
        m_lblSaveload.m_cText.m_cParams.SetStrRef(STRREF_SAVE);
#else
        m_btnSaveLoad.m_cText.m_cParams.SetStrRef(STRREF_SAVE);
#endif
        m_lblPanelName.m_cText.m_cParams.SetStrRef(STRREF_SAVE_GAME);
    }
    else
    {
#ifdef _XBOX
        m_lblSaveload.m_cText.m_cParams.SetStrRef(STRREF_LOAD);
#else
        m_btnSaveLoad.m_cText.m_cParams.SetStrRef(STRREF_LOAD);
#endif
        m_lblPanelName.m_cText.m_cParams.SetStrRef(STRREF_LOAD_GAME);
    }
    PopulateGameList();
    g_pExoResMan->AddResourceDirectory("TEMP:");
    SetActiveControl(&m_lbSaveGames, 0);
}
// PC 0x006C8250  Xbox 0x0010EF80
CExoString CSWGuiSaveLoadEntry::GetGameDirectory()
{
    CExoString sDirectory;

    sDirectory.Format("SAVES:%06d - %s", m_nSaveGame, m_sGameName);
    return sDirectory;
}
// PC 0x006C82F0
void CSWGuiSaveGameEditBox::HandleKeyPress(int nKey)
{
    static char szTmp[2];

    switch (nKey)
    {
    case 8:
    case 127:
        m_cText.RemoveLastChar();
        break;
    case 27:
        if (m_pOwner != NULL)
        {
            CSWGuiPanel* pPanel = m_pOwner->AsSWGuiPanel();
            if (pPanel != NULL)
            {
                pPanel->HandleInputEvent(GUIEVENT_CANCEL, 1);
            }
        }
        break;
    case 10:
    case 13:
        if (m_pOwner != NULL)
        {
            CSWGuiPanel* pPanel = m_pOwner->AsSWGuiPanel();
            if (pPanel != NULL)
            {
                pPanel->HandleInputEvent(GUIEVENT_ACTIVATE, 1);
            }
        }
        break;
    default:
        if (nKey >= ' ')
        {
            szTmp[0] = (char)nKey;
            szTmp[1] = 0;
            m_cText.AddNewChar(szTmp);
        }
        break;
    }
}
// PC 0x006C8410
void CSWGuiSaveNamePanel::OnPanelAdded()
{
    CSWGuiExtent cExtent;

    m_edtSaveName.SetFocus();
    CSWGuiPanel::OnPanelAdded();
    cExtent = m_cExtent;
#ifndef _XBOX
    g_pGuiMan->StoreCurrentMousePosition();
    g_pGuiMan->MoveMouseToPosition(cExtent.m_nLeft, cExtent.m_nTop);
#endif
}
// PC 0x006C8460
void CSWGuiSaveNamePanel::OnPanelRemoved()
{
    CSWGuiPanel::OnPanelRemoved();
#ifndef _XBOX
    g_pGuiMan->RestoreOldMousePosition();
#endif
}
// PC 0x006C8470
void CSWGuiSaveNamePanel::HandleCancelButton(CSWGuiControl* pControl)
{
    m_bDeferredRemoval = 0;
    m_nExitState = EXITSTATE_DELETE;
    m_pManager->PopModalPanel();
}
// PC 0x006C8490  Xbox 0x0010F020
inline int CStatusSummary::LoadFromGFF(CResGFF* pGFF, CResStruct& cStruct)
{
    int bSuccess;

    m_bDisplaySPending = pGFF->ReadFieldBYTE(&cStruct, "DISPLAYSPENDING",
                                             bSuccess, m_bDisplaySPending);
    m_bItemReceived = pGFF->ReadFieldBYTE(&cStruct, "ITEMRECEIVED", bSuccess,
                                          m_bItemReceived);
    m_bItemLost =
        pGFF->ReadFieldBYTE(&cStruct, "ITEMLOST", bSuccess, m_bItemLost);
    m_bJournal = pGFF->ReadFieldBYTE(&cStruct, "JOURNAL", bSuccess, m_bJournal);
    m_nLightShift =
        pGFF->ReadFieldBYTE(&cStruct, "LIGHTSHIFT", bSuccess, m_nLightShift);
    m_nDarkShift =
        pGFF->ReadFieldBYTE(&cStruct, "DARKSHIFT", bSuccess, m_nDarkShift);
    m_nCredits = pGFF->ReadFieldINT(&cStruct, "CREDITS", bSuccess, m_nCredits);
    m_bCreditsNet =
        pGFF->ReadFieldBYTE(&cStruct, "CREDITSNET", bSuccess, m_bCreditsNet);
    m_nExperience = pGFF->ReadFieldINT(&cStruct, "XP", bSuccess, m_nExperience);
    m_nStealthExperience = pGFF->ReadFieldINT(&cStruct, "STEALTHXP", bSuccess,
                                              m_nStealthExperience);
    m_bSoundPending = pGFF->ReadFieldBYTE(&cStruct, "SOUNDPENDING", bSuccess,
                                          m_bSoundPending);
    m_bLevelUpSound = pGFF->ReadFieldBYTE(&cStruct, "LEVELUPSOUND", bSuccess,
                                          m_bLevelUpSound);
    m_bNewQuestSound = pGFF->ReadFieldBYTE(&cStruct, "NEWQUESTSOUND", bSuccess,
                                           m_bNewQuestSound);
    m_bCompleteSound = pGFF->ReadFieldBYTE(&cStruct, "COMPLETESOUND", bSuccess,
                                           m_bCompleteSound);
    m_nSuppressed =
        pGFF->ReadFieldINT(&cStruct, "SUPPRESSED", bSuccess, m_nSuppressed);
    return m_nSuppressed;
}

void CSWGuiSaveLoad::ExitPanel()
{
    m_bDeferredRemoval = 0;
    m_nExitState = EXITSTATE_DELETE;
    if (m_bExitToMainMenu)
    {
        g_pAppManager->m_pClientExoApp->DisplayMainMenu(0);
    }
    else
    {
        m_pManager->PopModalPanel();
    }
}
// PC 0x006C86D0  Xbox 0x0010FA30
void CSWGuiSaveLoad::HandleInputEvent(int nEvent, int nState)
{
    if (nState != 0 &&
        (nEvent == 40 || (nEvent > 44 && nEvent <= 46)))
    {
        m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
        ExitPanel();
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
// PC 0x006CA110  Xbox 0x00112240
CSWGuiSaveLoad::~CSWGuiSaveLoad()
{
    g_pExoResMan->RemoveResourceDirectory("TEMP:");
#ifdef _XBOX
    g_pExoResMan->CleanDirectory("TEMP:", 0, 0);
#else
    if (!m_sResourceDirectory.IsEmpty())
    {
        g_pExoResMan->RemoveResourceDirectory(m_sResourceDirectory);
    }
#endif
}
// PC 0x006CA250  Xbox 0x001123F0
void CSWGuiSaveLoad::LoadPCAutoSave(const CExoString& sModuleName)
{

    CExoString sSavePath;
    CExoString sDirectory;
    CExoString sFileName;
    CExoString sTemp;

    g_pAppManager->GetServerExoApp()->UnloadModule();
    g_pAppManager->GetServerExoApp()->SetLoadingModule(1);
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(5, 1);
    sDirectory.Format("%06d - %s", CSWGuiSaveLoadEntry::SAVE_SLOT_AUTO,
                      "AUTOSAVE");
    sSavePath = "SAVES:";
    sSavePath = sSavePath + sDirectory;
    if (!g_pExoResMan->CreateDirectory2("GAMEINPROGRESS:"))
    {
        g_pExoResMan->CleanDirectory("GAMEINPROGRESS:", 0, 0);
    }
    CERFFile* pERF = new CERFFile;
    sFileName = sSavePath + "\\" + "SAVEGAME";
    pERF->Read(sFileName);
    pERF->ReadHeaderVariance(sFileName);
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(10, 1);
    UpdateScreen(0.033333335f, 0, 0);
    pERF->ExportFilesFromERF(sFileName, "GAMEINPROGRESS:", (void*)-1,
                           LoadScreenUpdate, 10, 10);
    g_pExoResMan->CreateDirectory2("TEMP:");
    sFileName = sSavePath + "\\" + "pifo";
    g_pExoResMan->FileCopy(sFileName, "TEMP:pifo", RESTYPE_IFO, RESTYPE_INVALID);
    g_pAppManager->GetClientExoApp()->SetLoadFromIFO(1);
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(25, 1);
    UpdateScreen(0.033333335f, 0, 0);
    g_pExoResMan->AddResourceDirectory(sSavePath);
    g_pAppManager->GetServerExoApp()->GetPartyTable()->LoadTableInfo();
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(30, 1);
    UpdateScreen(0.033333335f, 0, 0);
    g_pAppManager->GetServerExoApp()->GetGlobalVariableTable()->Load();
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(35, 1);
    UpdateScreen(0.033333335f, 0, 0);
    CResGFF* pGFF = new CResGFF(0, "NFO ", "savenfo");
    CResStruct cTopLevel;
    pGFF->GetTopLevelStruct(&cTopLevel);
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(40, 1);
    UpdateScreen(0.033333335f, 0, 0);
    KOTOR_AUTOSAVE_PARAMS cParams;
    CResStruct cAutoSave;
    if (pGFF->GetStructFromStruct(&cAutoSave, &cTopLevel, "AUTOSAVEPARAMS"))
    {
        cParams.LoadFromGFF(pGFF, cAutoSave);
    }
    delete pGFF;
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(45, 1);
    UpdateScreen(0.033333335f, 0, 0);
    if (cParams.m_sStartWaypoint != "*")
    {
        g_pAppManager->GetServerExoApp()->SetMoveToModuleStartWaypoint(
            cParams.m_sStartWaypoint);
    }
    else
    {
        g_pAppManager->GetServerExoApp()->SetMoveToModuleStartWaypoint("");
    }
    g_pAppManager->GetServerExoApp()->SetMoveToWorldTime(
        cParams.m_nTimeYear, cParams.m_nTimeMonth, cParams.m_nTimeDay,
        cParams.m_nTimeHour, cParams.m_nTimeMinute, cParams.m_nTimeSecond,
        cParams.m_nTimeMillisecond);
    g_pAppManager->GetServerExoApp()->SetMoveToPausedTimeOfDay(cParams.m_nTimePauseTime);
    g_pAppManager->GetServerExoApp()->SetMoveToPausedCalendarDay(cParams.m_nTimePauseDay);
    g_pAppManager->GetClientExoApp()->AddMoveToModuleMovie(cParams.m_sMovie1);
    g_pAppManager->GetClientExoApp()->AddMoveToModuleMovie(cParams.m_sMovie2);
    g_pAppManager->GetClientExoApp()->AddMoveToModuleMovie(cParams.m_sMovie3);
    g_pAppManager->GetClientExoApp()->AddMoveToModuleMovie(cParams.m_sMovie4);
    g_pAppManager->GetClientExoApp()->AddMoveToModuleMovie(cParams.m_sMovie5);
    g_pAppManager->GetClientExoApp()->AddMoveToModuleMovie(cParams.m_sMovie6);
    g_pAppManager->GetClientExoApp()->GetInGameGui()->SetStatusSummary(cParams.m_cStatusSummary);
    g_pAppManager->GetClientExoApp()->SetLoadMusic(cParams.m_sLoadMusic);
    g_pExoResMan->RemoveResourceDirectory(sSavePath);
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(50, 1);
    UpdateScreen(0.033333335f, 0, 0);
    m_bDeferredRemoval = 0;
    m_nExitState = EXITSTATE_DELETE;
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(10, 1);
    g_pAppManager->GetClientExoApp()->SetLoadStep(0, 10);
    g_pAppManager->GetClientExoApp()->SetLoadStep(1, 20);
    g_pAppManager->GetClientExoApp()->SetLoadStep(2, 23);
    g_pAppManager->GetClientExoApp()->SetLoadStep(3, 23);
    g_pAppManager->GetClientExoApp()->SetLoadStep(4, 23);
    g_pAppManager->GetServerExoApp()->SetDoingStartNewModule(1);
    g_pAppManager->GetServerExoApp()->ExecuteLoadModule(sModuleName);
}
// PC 0x006C8750  Xbox 0x0010FB50
int LoadScreenUpdate(int nStep, int nSteps, int nStart, int nRange)
{
#ifdef _XBOX
    g_pAppManager->m_pClientExoApp->SetLoadBarProgress(
        nStart + (int)((float)nStep / (float)nSteps * (float)nRange), 1);
#else
    g_pAppManager->m_pClientExoApp->SetLoadBarProgress(
        nStart + (int)((double)nStep / (double)nSteps * (double)nRange), 1);
#endif
    UpdateScreen(0.033333335f, 0, 0);
    return 0;
}
// PC 0x006C8790
void CSWGuiSaveLoad::WriteGame(const CExoString& sSaveName)
{
    CSWGuiSaveLoadEntry* pEntry =
        (CSWGuiSaveLoadEntry*)m_lbSaveGames.GetSelectedControl();
    unsigned long nSaveGame = pEntry->m_nSaveGame;
    if (nSaveGame > MAX_SAVE_GAME)
    {
        nSaveGame = m_nMaxSaveGame;
        if (nSaveGame > MAX_SAVE_GAME)
        {
            CGuiInGame* pGuiInGame = g_pAppManager->GetClientExoApp()->GetInGameGui();
            CSWGuiMessageBox* pMessageBox = pGuiInGame->m_pMessageBox;
            CExoString sMessage = g_pTlkTable->GetSimpleString(STRREF_MAXIMUM_SAVES);
            g_pTlkTable->ParseStr(sMessage);
            pMessageBox->SetAllowCancel(0);
            pMessageBox->SetMessage(sMessage);
            GetManager()->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
            return;
        }
        pEntry->m_nSaveGame = nSaveGame;
    }
    if (pEntry->m_bDataLoaded)
    {
        CSWGuiMessageBox* pMessageBox =
            g_pAppManager->GetClientExoApp()->GetInGameGui()->m_pMessageBox;
        if (!pMessageBox->m_bAccepted)
        {
            return;
        }
    }
    CExoString sGameName;
    int nIndex = pEntry->m_nSaveGame - 1;
    sGameName.Format("Game%d", nIndex);
    int nSlot = pEntry->m_nSaveGame;
#ifdef _XBOX
    g_pAppManager->GetClientExoApp()->SendSaveGameRequest(nSlot, sGameName);
#else
    g_pAppManager->GetClientExoApp()->SendSaveGameRequest(nSlot, sGameName, sSaveName);
#endif
    m_bDeferredRemoval = 0;
    m_nExitState = EXITSTATE_DELETE;
    m_pManager->PopModalPanel();
}
// PC 0x006CAA90  Xbox 0x00115AB0
void CSWGuiSaveLoad::DeleteGame(CSWGuiControl* pControl)
{
    int bSelected;
    CSWGuiMessageBox* pMessageBox;

    bSelected = pControl->m_bSelected;
    if (bSelected != 0)
    {
        pMessageBox =
            g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
        pMessageBox->SetAllowCancel(1);
        pMessageBox->SetMessage(1592);
        pMessageBox->SetCallback(
            this, (CSWGuiEventHandler)&CSWGuiSaveLoad::RemoveGame, pControl);
        m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
    }
}
// PC 0x006C8900  Xbox 0x00114260
void CSWGuiSaveLoad::RemoveGame(CSWGuiControl* pControl)
{
    CSWGuiMessageBox* pMessageBox;
    CSWGuiControl* pSelected;
    int nIndex;
    int nLastIndex;

    pMessageBox = g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
    if (pMessageBox->m_bAccepted)
    {
        pSelected = m_lbSaveGames.GetSelectedControl();
        if (pSelected != NULL)
        {
            g_pExoResMan->NukeDirectory(
                ((CSWGuiSaveLoadEntry*)pSelected)->GetGameDirectory(), 1, 1);
            nIndex = m_lbSaveGames.m_nSelectedItem;
            PopulateGameList();
            nLastIndex = m_lbSaveGames.m_lItems.num - 1;
            if (nIndex < nLastIndex)
            {
                nLastIndex = nIndex;
            }
            m_lbSaveGames.SetSelectedControl(nLastIndex, 0);
            SetActiveControl(NULL, 0);
            SetActiveControl(&m_lbSaveGames, 0);
        }
    }
}
// PC 0x006C89D0  Xbox 0x0010FDD0
void CSWGuiSaveLoad::ShowGame(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected && pControl->AsButton() != NULL)
    {
        CSWGuiSaveLoadEntry* pEntry = (CSWGuiSaveLoadEntry*)pControl;
        if (pEntry->m_bDataLoaded && !pEntry->m_bBadSave)
        {
#ifndef _XBOX
            m_btnDelete.SetEnabled(1);
#endif
            int nSeparator = pEntry->m_sAreaName.Find("-", 0);
            if (nSeparator == -1)
            {
                m_lblPlanetName.m_cText.m_cParams.SetText(pEntry->m_sAreaName);
                m_lblAreaName.m_cText.m_cParams.SetText("");
            }
            else
            {
                CExoString sPlanet = pEntry->m_sAreaName.Left(nSeparator - 1);
                CExoString sArea = pEntry->m_sAreaName.Right(
                    pEntry->m_sAreaName.GetLength() - nSeparator - 2);
                m_lblPlanetName.m_cText.m_cParams.SetText(sPlanet);
                m_lblAreaName.m_cText.m_cParams.SetText(sArea);
            }
            for (int nPortrait = 0; nPortrait < 3; ++nPortrait)
            {
                CResRef* pPortrait = &pEntry->GetPortrait(nPortrait);
                m_aPartyMemberLabels[nPortrait].m_cBorder.m_cParams.SetFillImage(
                    *pPortrait, 0);
            }
            if (!pEntry->m_bRebootAutoSave && !pEntry->m_bPCAutoSave)
            {
                {
                    CResRef cEmpty("");
                    m_lblScreenshot.m_cBorder.m_cParams.SetFillImage(cEmpty, 0);
                }
                if (!m_sResourceDirectory.IsEmpty())
                {
                    g_pExoResMan->RemoveResourceDirectory(m_sResourceDirectory);
                }
                m_sResourceDirectory = pEntry->GetGameDirectory();
                g_pExoResMan->AddResourceDirectory(m_sResourceDirectory);
                {
                    CResRef cScreenshot("screen");
                    m_lblScreenshot.m_cBorder.m_cParams.SetFillImage(cScreenshot, 0);
                }
            }
            else
            {
                CResRef cScreenshot(pEntry->m_sScreenshot);
                m_lblScreenshot.m_cBorder.m_cParams.SetFillImage(cScreenshot, 0);
            }
            m_lblScreenshot.m_cText.m_cParams.SetText("");
            if (pEntry->m_bCheatUsed)
            {
                m_lblScreenshot.m_cText.m_cParams.SetText("Cheat Used");
                m_lblScreenshot.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_YELLOW);
                m_lblScreenshot.m_bEnabled = 1;
            }
        }
        else
        {
            m_lblPlanetName.m_cText.m_cParams.SetText("");
            m_lblAreaName.m_cText.m_cParams.SetText("");
            for (int nPortrait = 0; nPortrait < 3; ++nPortrait)
            {
                CResRef* pPortrait = &CResRef();
                m_aPartyMemberLabels[nPortrait].m_cBorder.m_cParams.SetFillImage(
                    *pPortrait, 0);
            }
            CResRef* pScreenshot = &CResRef();
            m_lblScreenshot.m_cBorder.m_cParams.SetFillImage(*pScreenshot, 0);
            m_lblScreenshot.m_cText.m_cParams.SetStrRef(STRREF_NEW_SLOT);
#ifndef _XBOX
            m_btnDelete.SetEnabled(0);
#endif
        }
#ifdef _XBOX
        if (!m_bSaveMode)
        {
            m_lblSaveload.m_cText.m_cParams.SetColor(
                pEntry->m_bMissingLiveContent ? CGuiInGame::DISABLED_TEXT : CGuiInGame::COLOR_BLUE);
        }
        else
        {
            m_lblSaveload.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
        }
        m_lblDelete.m_cText.m_cParams.SetColor(
            pEntry->m_bDataLoaded ? CGuiInGame::COLOR_BLUE : CGuiInGame::DISABLED_TEXT);
#else
        if (!m_bSaveMode)
        {
            m_btnSaveLoad.m_cText.m_cParams.SetColor(
                pEntry->m_bMissingLiveContent ? CGuiInGame::DISABLED_TEXT : CGuiInGame::COLOR_BLUE);
            m_btnSaveLoad.m_cBorder.m_cParams.m_vColor =
                pEntry->m_bMissingLiveContent ? CGuiInGame::DISABLED_ICON_COLOR : CGuiInGame::ENABLED_ICON_COLOR;
        }
        else
        {
            m_btnSaveLoad.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
            m_btnSaveLoad.m_cBorder.m_cParams.m_vColor = CGuiInGame::ENABLED_ICON_COLOR;
        }
        m_btnDelete.m_cText.m_cParams.SetColor(
            pEntry->m_bDataLoaded ? CGuiInGame::COLOR_BLUE : CGuiInGame::DISABLED_TEXT);
        m_btnDelete.m_cBorder.m_cParams.m_vColor =
            pEntry->m_bDataLoaded ? CGuiInGame::ENABLED_ICON_COLOR : CGuiInGame::DISABLED_ICON_COLOR;
#endif
    }
}
// PC 0x006C8E50  Xbox 0x001104B0
void CSWGuiSaveLoadEntry::LoadData(const CExoString& sSaveDirectory)
{

    CExoString sLabel;
    CExoString sDirectory;
    CExoString sTemp;

    int nSeparator = sSaveDirectory.Find(" - ", 0);
    m_nSaveGame = sSaveDirectory.Left(nSeparator).AsINT();
    m_sGameName = sSaveDirectory.Right(sSaveDirectory.GetLength() - nSeparator - 3);
    sDirectory = CExoString("SAVES:") + sSaveDirectory;
    g_pExoResMan->AddResourceDirectory(sDirectory);

    CExoString sCorruptFile = g_pExoBase->m_pExoAliasList->ResolveFileName(
        sDirectory + "\\" + "CORRUPT", 0);
    CExoFile cCorruptFile(
        sDirectory + "\\" + "CORRUPT", 0, "rb");
    if (cCorruptFile.FileOpened())
    {
        m_bDataLoaded = 1;
        m_bBadSave = 1;
        g_pExoResMan->RemoveResourceDirectory(sDirectory);
        return;
    }

    CResGFF* pGFF = new CResGFF(0, "NFO ", "savenfo");
    CResStruct cStruct;
    int bSuccess;
    pGFF->GetTopLevelStruct(&cStruct);
    m_sAreaName = pGFF->ReadFieldCExoString(&cStruct, "AREANAME", bSuccess, "");
    m_sLastModule = pGFF->ReadFieldCExoString(&cStruct, "LASTMODULE", bSuccess, "");
    m_nTimePlayed = pGFF->ReadFieldDWORD(&cStruct, "TIMEPLAYED", bSuccess, 0);
#ifndef _XBOX
    m_sSaveGameName = pGFF->ReadFieldCExoString(&cStruct, "SAVEGAMENAME", bSuccess, "");
    if (!bSuccess)
    {
        m_sSaveGameName = CExoString("Old Save Game");
    }
#endif
    m_bCheatUsed = pGFF->ReadFieldBYTE(&cStruct, "CHEATUSED", bSuccess, 0);
    m_bRebootAutoSave = pGFF->ReadFieldBYTE(&cStruct, "REBOOTAUTOSAVE", bSuccess, 0);
    m_bPCAutoSave = pGFF->ReadFieldBYTE(&cStruct, "PCAUTOSAVE", bSuccess, 0);
    m_sScreenshot = pGFF->ReadFieldCExoString(&cStruct, "SCREENSHOT", bSuccess, "");
    unsigned char nDefault = m_nGameplayHint;
    m_nGameplayHint = pGFF->ReadFieldBYTE(&cStruct, "GAMEPLAYHINT", bSuccess, nDefault);
    nDefault = m_nStoryHint;
    m_nStoryHint = pGFF->ReadFieldBYTE(&cStruct, "STORYHINT", bSuccess, nDefault);
    unsigned char nLiveContent = pGFF->ReadFieldBYTE(&cStruct, "LIVECONTENT", bSuccess, 0);
    if (nLiveContent)
    {
        CExoString sAlias;
        for (int nContent = 1; nContent < CTlkTable::NUM_CONTENT_PKGS; nContent++)
        {
            sAlias.Format("LIVE%d", nContent);
            CExoString sContentName = pGFF->ReadFieldCExoString(
                &cStruct, (char*)sAlias.CStr(), bSuccess, sAlias);
            if (g_pExoBase->m_pExoAliasList->GetAliasPath(sAlias, 0).GetLength() == 0 &&
                (nLiveContent & (1 << (nContent - 1))))
            {
                m_bDataLoaded = 1;
                m_bMissingLiveContent = 1;
                m_sMissingLiveContent = sContentName;
                delete pGFF;
                g_pExoResMan->RemoveResourceDirectory(sDirectory);
                return;
            }
        }
    }

    for (int nPortrait = 0; nPortrait < 3; nPortrait++)
    {
        sLabel.Format("%s%d", "PORTRAIT", nPortrait);
        m_aPortraits[nPortrait] = pGFF->ReadFieldCResRef(
            &cStruct, (char*)sLabel.CStr(), bSuccess, "");
    }
    delete pGFF;
    g_pExoResMan->RemoveResourceDirectory(sDirectory);
    m_bBadSave = 0;
    m_bMissingLiveContent = 0;
    m_sMissingLiveContent = "";
    m_bDataLoaded = 1;
}
// PC 0x006C9780  Xbox 0x00110EA0
void CSWGuiSaveLoadEntry::SetXboxTitle(const CExoString& sTitle)
{

    m_sXboxTitle = sTitle;
    CExoString sTemp;

#ifdef _XBOX
    if (m_nSaveGame == SAVE_SLOT_QUICK)
    {
        m_sXboxTitle = g_pTlkTable->GetSimpleString(STRREF_AUTO_SAVE);
    }
#else
    if (m_nSaveGame == SAVE_SLOT_AUTO)
    {
        m_sXboxTitle = g_pTlkTable->GetSimpleString(STRREF_AUTO_SAVE);
    }
    else if (m_nSaveGame == SAVE_SLOT_QUICK)
    {
        m_sXboxTitle = g_pTlkTable->GetSimpleString(STRREF_QUICK_SAVE);
    }

    CExoString sFormattedTitle;
    unsigned long nHours = m_nTimePlayed / 3600;
    unsigned long nMinutes = (m_nTimePlayed / 60) % 60;

    if (m_nSaveGame == SAVE_SLOT_QUICK || m_nSaveGame == SAVE_SLOT_AUTO)
    {
        g_pTlkTable->SetCustomToken(TIME_TOKEN_HOURS, nHours);
        g_pTlkTable->SetCustomToken(TIME_TOKEN_MINUTES, nMinutes);
        sFormattedTitle = g_pTlkTable->GetSimpleString(STRREF_SAVE_TIME);
        g_pTlkTable->ParseStr(sFormattedTitle);
        m_sXboxTitle = m_sXboxTitle + sFormattedTitle;
    }
    else
    {
        g_pTlkTable->SetCustomToken(TITLE_TOKEN_GAME, m_nSaveGame - 1);
        g_pTlkTable->SetCustomToken(TITLE_TOKEN_HOURS, nHours);
        g_pTlkTable->SetCustomToken(TITLE_TOKEN_MINUTES, nMinutes);
        g_pTlkTable->SetCustomToken(TITLE_TOKEN_NAME, m_sSaveGameName);
        sFormattedTitle = g_pTlkTable->GetSimpleString(STRREF_SAVE_TITLE);
        g_pTlkTable->ParseStr(sFormattedTitle);
        m_sXboxTitle = sFormattedTitle;
    }
#endif
    m_cText.m_cParams.SetText(m_sXboxTitle);
}
// PC 0x006CB0E0  Xbox 0x00114870
void CSWGuiSaveLoad::LoadGame(CSWGuiControl* pControl)
{
    if (m_bExitToMainMenu ||
        g_pAppManager->GetClientExoApp()->GetInGameGui()->GetMessageBox()->m_bAccepted)
    {
        g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_GENERIC);
        if (m_bExitToMainMenu)
        {
            g_pAppManager->GetClientExoApp()->BusyWaitMenuMusic();
        }
        else
        {
            g_pAppManager->GetClientExoApp()->BusyWaitMuteSound();
        }
        CClientExoApp* pClientExoApp = g_pAppManager->GetClientExoApp();
        CSWGuiSaveLoadEntry* pEntry = (CSWGuiSaveLoadEntry*)pControl;
        if (pEntry->m_bMissingLiveContent && !g_bEnableLiveContentIgnore)
        {
            CSWGuiMessageBox* pMessageBox = g_pAppManager->GetClientExoApp()->GetInGameGui()->m_pMessageBox;
            pMessageBox->SetAllowCancel(0);
            pMessageBox->SetCallback(NULL, NULL, NULL);
            CExoString sMessage;
            sMessage = g_pTlkTable->GetSimpleString(STRREF_MISSING_LIVE_CONTENT);
            g_pTlkTable->SetCustomToken(MISSING_LIVE_CONTENT_TOKEN, pEntry->m_sMissingLiveContent);
            g_pTlkTable->ParseStr(sMessage);
            pMessageBox->SetMessage(sMessage);
            CSWGuiManager* pManager = m_pManager;
            pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
            return;
        }
        CSWPartyTable::ResetCurrentSessionStartTime();
        g_pAppManager->GetClientExoApp()->GetInGameGui()->SetGlobalUnpauseableState(1);
        if (!pEntry->m_bRebootAutoSave)
        {
            if (pEntry->m_bPCAutoSave)
            {
                m_pManager->PopModalPanel();
#ifndef _XBOX
                pClientExoApp->GetInGameGui()->HideSWInGameGui(0);
#endif
                pClientExoApp->SetLoadScreenByModuleName(pEntry->m_sLastModule);
                pClientExoApp->SetLoadMusic("");
                pClientExoApp->ShowLoadScreen(1, 1, 0);
                pClientExoApp->SetLoadBarProgress(0, 1);
                UpdateScreen(0.033333335f, 0, 0);
                if (!m_bExitToMainMenu)
                {
                    g_pAppManager->DestroyServer();
                    g_pAppManager->GetClientExoApp()->UnloadModule();
                }
                g_pAppManager->CreateServer(0);
                g_pAppManager->GetClientExoApp()->GetWorldTimer()->PauseWorldTimer();
                g_pAppManager->GetServerExoApp()->GetWorldTimer()->PauseWorldTimer();
                pClientExoApp->GetNetLayer()->StartConnectToSession(
                    0, pClientExoApp->GetClientName(), "",
                    CNetLayer::LOCAL_SESSION_PORT, CNetLayer::CONNECTTYPE_PLAYER);
                pClientExoApp->GetSWCMessage()->SendPlayerToServerStatus_RequestStatus();
                pClientExoApp->SetInputClass(INPUT_CLASS_GAME, 1);
                pClientExoApp->DisableInput();
                LoadPCAutoSave(pEntry->m_sLastModule);
                m_bDeferredRemoval = 0;
                m_nExitState = EXITSTATE_DELETE;
            }
            else if (m_bExitToMainMenu)
            {
                pClientExoApp->SetLoadScreenByModuleName(pEntry->m_sLastModule);
                pClientExoApp->SetLoadMusic("");
                pClientExoApp->ShowLoadScreen(0, 1, 0);
                unsigned char nGamePlayHint = pEntry->m_nGameplayHint;
                pClientExoApp->SetGamePlayHint(nGamePlayHint);
                unsigned char nStoryHint = pEntry->m_nStoryHint;
                pClientExoApp->SetStoryHint(nStoryHint);
                pClientExoApp->SetLoadScreenHint(
                    pClientExoApp->GetNextLoadScreenHintSTRREF(
                        "", CClientExoAppInternal::LOAD_SCREEN_HINT_STORY));
                pClientExoApp->SetLoadScreenHintUpdateTimer(0);
                unsigned long nSaveGame = pEntry->m_nSaveGame;
                CExoString sGameName = pEntry->m_sGameName;
                CExoString sLastModule = pEntry->m_sLastModule;
                if (!UnpackGame(pEntry->GetSaveGame(), pEntry->m_sGameName, pControl))
                {
                    return;
                }
                m_bDeferredRemoval = 0;
                m_nExitState = EXITSTATE_DELETE;
                g_pAppManager->CreateServer(0);
                g_pAppManager->GetClientExoApp()->GetWorldTimer()->PauseWorldTimer();
                g_pAppManager->GetServerExoApp()->GetWorldTimer()->PauseWorldTimer();
                pClientExoApp->GetNetLayer()->StartConnectToSession(
                    0, pClientExoApp->GetClientName(), "",
                    CNetLayer::LOCAL_SESSION_PORT, CNetLayer::CONNECTTYPE_PLAYER);
                pClientExoApp->GetSWCMessage()->SendPlayerToServerStatus_RequestStatus();
                pClientExoApp->SetPlayerCharacterName("test");
                pClientExoApp->SetInputClass(INPUT_CLASS_GAME, 1);
                pClientExoApp->DisableInput();
                g_pAppManager->GetServerExoApp()->SetLoadFromSaveGame(1);
                g_pAppManager->GetClientExoApp()->SetLoadingSaveGame(1);
                g_pAppManager->GetClientExoApp()->SendLoadGameRequest(
                    nSaveGame, sGameName, sLastModule);
            }
            else
            {
                CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
                pClient->SetLoadScreenByModuleName(pEntry->m_sLastModule);
                pClient->SetLoadMusic("");
                pClient->ShowLoadScreen(0, 1, 0);
                unsigned char nGamePlayHint = pEntry->m_nGameplayHint;
                pClient->SetGamePlayHint(nGamePlayHint);
                unsigned char nStoryHint = pEntry->m_nStoryHint;
                pClient->SetStoryHint(nStoryHint);
                pClient->SetLoadScreenHint(pClient->GetNextLoadScreenHintSTRREF(
                    "", CClientExoAppInternal::LOAD_SCREEN_HINT_STORY));
                pClient->SetLoadScreenHintUpdateTimer(0);
                if (UnpackGame(pEntry->GetSaveGame(), pEntry->m_sGameName, pControl))
                {
                    m_bDeferredRemoval = 0;
                    m_nExitState = EXITSTATE_DELETE;
                    while (m_pManager->m_lModalPanels.num != 0)
                    {
                        m_pManager->PopModalPanel();
                    }
#ifdef _XBOX
                    pClient->GetInGameGui()->HideSWInGameGui();
#else
                    pClient->GetInGameGui()->HideSWInGameGui(1);
#endif
                    g_pAppManager->GetServerExoApp()->SetLoadFromSaveGame(1);
                    g_pAppManager->GetClientExoApp()->SetLoadingSaveGame(1);
                    g_pAppManager->GetClientExoApp()->SendLoadGameRequest(
                        pEntry->m_nSaveGame, pEntry->m_sGameName, pEntry->m_sLastModule);
                }
            }
        }
    }
}
// PC 0x006CB820
void CSWGuiSaveLoad::PromptForSaveName(CSWGuiControl* pControl)
{
    SwguisaveloadTrace2();
    CExoString sSaveName;

    if (pControl != NULL)
    {
        CSWGuiSaveLoadEntry* pEntry = (CSWGuiSaveLoadEntry*)pControl;
        if (pEntry->m_bDataLoaded)
        {
            CSWGuiMessageBox* pMessageBox =
                g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
            if (!pMessageBox->m_bAccepted)
            {
                return;
            }
#ifndef _XBOX
            sSaveName = pEntry->m_sSaveGameName;
#endif
        }
        else
        {
            sSaveName = CExoString("");
        }
        CSWGuiSaveNamePanel* pPanel = new CSWGuiSaveNamePanel(m_pManager);
        pPanel->SetTextAndPointer(this, sSaveName);
        g_pGuiMan->AddPanel(pPanel, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
    }
}
// PC 0x006CB940  Xbox 0x00115B10
CSWGuiSaveLoadEntry::CSWGuiSaveLoadEntry()
{
    SwguisaveloadTrace2();
    m_bDataLoaded = 0;
    m_bBadSave = 0;
    m_bNewSlot = 0;
    m_bRebootAutoSave = 0;
#ifdef _XBOX
    m_bMissingLiveContent = 0;
#else
    m_bPCAutoSave = 0;
    m_bEmptySlot = 0;
    m_bCheatUsed = 0;
#endif
    m_nGameplayHint = 0;
    m_nStoryHint = 0;
    m_nSaveGame = SAVE_SLOT_QUICK;
    m_nTimePlayed = 0;
    m_nSaveSpaceNeeded = 0;
#ifndef _XBOX
    m_sSaveGameName = "Bad";
#endif
}
// PC 0x006CBB60  Xbox 0x00115680
void CSWGuiSaveLoad::HandleSaveButton(CSWGuiControl* pControl)
{
    int bSelected = pControl->m_bSelected;
    int nOptionsSpace = 0;
    if (bSelected)
    {
        g_pExoResMan->AddResourceDirectory("OPTIONS:");
        {
            CExoFile cOptions("OPTIONS:OPT", 0, "r");
            if (!cOptions.FileOpened() || !cOptions.GetSize())
            {
                nOptionsSpace = OPTIONS_SPACE_UNITS;
            }
        }

        CSWGuiSaveLoadEntry* pEntry =
            (CSWGuiSaveLoadEntry*)m_lbSaveGames.GetSelectedControl();
        if (pEntry != NULL)
        {
            if (pEntry->m_bDataLoaded)
            {
                unsigned __int64 nFreeBytes;
                g_pExoResMan->GetFreeDiskSpace("SAVES:", &nFreeBytes);
                __int64 nFreeUnits = nFreeBytes >> 14;
                __int64 nRequiredUnits = SAVE_SPACE_UNITS + nOptionsSpace;
                nFreeUnits += nRequiredUnits;
                if (nFreeUnits < SAVE_SPACE_UNITS + nOptionsSpace)
                {
                    CSWGuiMessageBox* pMessageBox =
                        g_pAppManager->GetClientExoApp()->GetInGameGui()->m_pMessageBox;
                    int nNeededUnits = SAVE_SPACE_UNITS + nOptionsSpace - (int)nFreeUnits;
                    float fNeededMB = (float)(nNeededUnits << 14) * 0.000001f;
                    pEntry->m_nSaveSpaceNeeded = nNeededUnits;
                    CExoString sMessage;
                    CExoString sNeeded;
                    sNeeded.Format("%2.3f", fNeededMB);
                    g_pTlkTable->SetCustomToken(SAVE_SPACE_TOKEN_MB, sNeeded);
                    sMessage = g_pTlkTable->GetSimpleString(STRREF_SAVE_SPACE_REQUIRED);
                    g_pTlkTable->ParseStr(sMessage);
                    pMessageBox->SetAllowCancel(0);
                    pMessageBox->SetMessage(sMessage);
                    pMessageBox->SetCallback(this, NULL, NULL);
                    GetManager()->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
                }
                else
                {
                    CSWGuiMessageBox* pMessageBox =
                        g_pAppManager->GetClientExoApp()->GetInGameGui()->m_pMessageBox;
                    pMessageBox->SetAllowCancel(1);
                    pMessageBox->SetMessage(STRREF_CONFIRM_OVERWRITE);
                    pMessageBox->SetCallback(
                        this, (CSWGuiEventHandler)&CSWGuiSaveLoad::PromptForSaveName,
                        pEntry);
                    GetManager()->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
                }
            }
            else
            {
                unsigned __int64 nFreeBytes;
                g_pExoResMan->GetFreeDiskSpace("SAVES:", &nFreeBytes);
                __int64 nFreeUnits = nFreeBytes >> 14;
                if (nFreeUnits < SAVE_SPACE_UNITS + nOptionsSpace)
                {
                    CSWGuiMessageBox* pMessageBox =
                        g_pAppManager->GetClientExoApp()->GetInGameGui()->m_pMessageBox;
                    int nNeededUnits = SAVE_SPACE_UNITS + nOptionsSpace - (int)nFreeUnits;
                    float fNeededMB = (float)(nNeededUnits << 14) * 0.000001f;
                    pEntry->m_nSaveSpaceNeeded = nNeededUnits;
                    CExoString sMessage;
                    CExoString sNeeded;
                    sNeeded.Format("%2.3f", fNeededMB);
                    g_pTlkTable->SetCustomToken(SAVE_SPACE_TOKEN_MB, sNeeded);
                    sMessage = g_pTlkTable->GetSimpleString(STRREF_SAVE_SPACE_REQUIRED);
                    g_pTlkTable->ParseStr(sMessage);
                    pMessageBox->SetAllowCancel(0);
                    pMessageBox->SetMessage(sMessage);
                    pMessageBox->SetCallback(this, NULL, NULL);
                    GetManager()->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
                }
                else
                {
                    PromptForSaveName(pEntry);
                }
            }
        }
    }
}
// PC 0x006CAAF0  Xbox 0x001144C0
int CSWGuiSaveLoad::UnpackGame(unsigned long nSaveGame,
                               const CExoString& sSaveName,
                               CSWGuiControl* pControl)
{
    if (CopyGameToFutureGame(nSaveGame, sSaveName) != 0)
    {
        CExoString sCurrentPath = g_pExoBase->m_pExoAliasList->ResolveFileName(
            "GAMEINPROGRESS:", 0xFFFF);
        CExoString sFuturePath =
            g_pExoBase->m_pExoAliasList->ResolveFileName("FUTUREGAME:", RESTYPE_INVALID);

        g_pExoResMan->NukeDirectory("GAMEINPROGRESS:", 1, 1);
        sFuturePath = sFuturePath.Left(sFuturePath.GetLength() - 1);
        sCurrentPath = sCurrentPath.Left(sCurrentPath.GetLength() - 1);
        rename(sFuturePath.CStr(), sCurrentPath.CStr());
        return 1;
    }

    CExoString sDirectory;
    CExoString sFileName;
    if (nSaveGame != (unsigned long)-1)
    {
        sDirectory.Format("%06d - %s", nSaveGame, sSaveName.CStr());
    }
    else
    {
        sDirectory = sSaveName;
    }
    sFileName = "SAVES:";
    sFileName = sFileName + sDirectory;
#ifdef _XBOX
    g_pAppManager->m_pClientExoApp->HideLoadScreen(0);
#else
    g_pAppManager->m_pClientExoApp->HideLoadScreen(0, 1);
#endif
#ifdef _XBOX
    ShowSaveError(42027);
#endif
    CExoFile cFile(sFileName + "\\" + "CORRUPT", 0, "wb+");
    cFile.Write("CORRUPT");
    cFile.Flush();
    return 0;
}
// PC 0x006CAE70
CSWGuiSaveNamePanel::CSWGuiSaveNamePanel(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    SwguisaveloadTrace1();
    m_pSaveLoad = NULL;
    StartLoadFromLayout("savename");
    InitControl(m_btnSave, "BTN_OK", 1);
    InitControl(m_btnCancel, "BTN_CANCEL", 1);
    InitControl(m_edtSaveName, "EDITBOX", 1);
    InitControl(m_lblSaveName, "LBL_TITLE", 1);
    CSWGuiButton* pSaveButton = &m_btnSave;
    CSWGuiButton* pCancelButton = &m_btnCancel;

    pSaveButton->m_bFocusable = 0;
    pCancelButton->m_bFocusable = 0;
    StopLoadFromLayout();
    CenterPanel();
    m_btnSave.AddEvent(39, this,
                        (CSWGuiHandler)&CSWGuiSaveNamePanel::HandleOkButton);
    m_btnCancel.AddEvent(
        39, this, (CSWGuiHandler)&CSWGuiSaveNamePanel::HandleCancelButton);
    m_edtSaveName.m_cText.m_nMaxLength = 16;
}

void CSWGuiSaveNamePanel::SetTextAndPointer(CSWGuiSaveLoad* pSaveLoad, const CExoString& sText)
{
    m_pSaveLoad = pSaveLoad;
    m_edtSaveName.m_cText.SetText(sText);
}
// PC 0x006CC160  Xbox 0x00115D50
void CSWGuiSaveLoad::PopulateGameList()
{
    CExoArrayList<CExoString> lDirectories;
    CExoArrayList<CSWGuiControl*> lControls;
    CSWGuiExtent cExtent;
    cExtent.m_nHeight = 42;
    CSWGuiButton* pProto = (CSWGuiButton*)m_lbSaveGames.m_pProtoItem;
    CExoString sGameDirectory;

    g_pExoBase->GetDirectoryList(&lDirectories, "SAVES:", RESTYPE_INVALID, 1, 1);
    if (g_pExoResMan->CreateDirectory2("TEMP:") == 0)
    {
        g_pExoResMan->CleanDirectory("TEMP:", 0, 0);
    }
    for (int i = 0; i < lDirectories.num; i++)
    {
        CSWGuiSaveLoadEntry* pEntry = new CSWGuiSaveLoadEntry;
        pEntry->Initialize(cExtent, pProto);
        pEntry->LoadData(lDirectories[i]);
        pEntry->SetXboxTitle(lDirectories[i]);
        if (m_bSaveMode &&
            (pEntry->m_nSaveGame == CSWGuiSaveLoadEntry::SAVE_SLOT_AUTO ||
             pEntry->m_nSaveGame == CSWGuiSaveLoadEntry::SAVE_SLOT_QUICK))
        {
            delete pEntry;
        }
        else
        {
            if (m_bSaveMode)
            {
                pEntry->AddEvent(GUIEVENT_ACTIVATE, this,
                    (CSWGuiEventHandler)&CSWGuiSaveLoad::HandleSaveButton);
            }
            else
            {
                pEntry->AddEvent(GUIEVENT_ACTIVATE, this,
                    (CSWGuiEventHandler)&CSWGuiSaveLoad::VerifyLoadGame);
            }
            pEntry->AddEvent(GUIEVENT_DELETE, this,
                (CSWGuiEventHandler)&CSWGuiSaveLoad::DeleteGame);
            pEntry->AddEvent(GUIEVENT_MOUSE_ENTER, this,
                (CSWGuiEventHandler)&CSWGuiSaveLoad::ShowGame);
            lControls.Add(pEntry);
            if (i == 0)
            {
                ShowGame(pEntry);
            }
        }
    }
    int nNextSlot = CSWGuiSaveLoadEntry::SAVE_SLOT_FIRST_NORMAL;
    unsigned long nMaxSlot = 0;
    if (m_bSaveMode)
    {
        for (int i = 0; i < lControls.num; i++)
        {
            unsigned long nSlot = ((CSWGuiSaveLoadEntry*)lControls[i])->m_nSaveGame;
            if (nSlot == nNextSlot)
            {
                nNextSlot++;
            }
            nSlot = max(nMaxSlot, nSlot);
            nMaxSlot = nSlot;
        }
        CSWGuiSaveLoadEntry* pEntry = new CSWGuiSaveLoadEntry;
        pEntry->Initialize(cExtent, pProto);
        pEntry->m_cText.m_cParams.SetStrRef(STRREF_NEW_SLOT);
        pEntry->m_nSaveGame = max(CSWGuiSaveLoadEntry::SAVE_SLOT_FIRST_NORMAL, nMaxSlot + 1);
        pEntry->m_bNewSlot = pEntry->m_bEmptySlot = 1;
        pEntry->AddEvent(GUIEVENT_ACTIVATE, this,
            (CSWGuiEventHandler)&CSWGuiSaveLoad::HandleSaveButton);
        pEntry->AddEvent(GUIEVENT_MOUSE_ENTER, this,
            (CSWGuiEventHandler)&CSWGuiSaveLoad::ShowGame);
        lControls.Insert(pEntry, 0);
    }
    else if (lControls.num == 0)
    {
        ExitPanel();
        CSWGuiMessageBox* pMessageBox =
            g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
        pMessageBox->SetAllowCancel(0);
        pMessageBox->SetCallback(NULL, NULL, NULL);
        pMessageBox->SetMessage(STRREF_NO_SAVED_GAMES);
        GetManager()->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
        return;
    }
    m_lbSaveGames.AddControls(lControls, 1, 0, 0);
    m_nMaxSaveGame = nNextSlot;
}

static inline void SwguisaveloadTrace2()
{
    SwguisaveloadTrace1();
}

static inline void SwguisaveloadTrace1()
{
}

#ifdef _XBOX
#include "exoinput.h"
// Xbox 0x0010FBA0
void CSWGuiSaveLoad::UpdateFreeBlocks()
{
    unsigned __int64 nFreeBlocks;
    g_pExoResMan->GetFreeDiskSpace("SAVES:", &nFreeBlocks);
    nFreeBlocks >>= 14;
    CExoString sText;
    if (nFreeBlocks >= 50000)
    {
        sText = g_pTlkTable->GetSimpleString(42015);
    }
    else if (nFreeBlocks < 1000)
    {
        sText.Format("%d", (int)nFreeBlocks);
    }
    else
    {
        {
            CExoString sThousands((int)(nFreeBlocks / 1000));
            g_pTlkTable->SetCustomToken(0, sThousands);
        }
        CExoString sRemainder;
        sRemainder.Format("%03d", (int)(nFreeBlocks % 1000));
        g_pTlkTable->SetCustomToken(1, sRemainder);
        sText = g_pTlkTable->GetSimpleString(42014);
        g_pTlkTable->ParseStr(sText);
    }
    CSWGuiTextParams* pParams = &m_lblFreeblocksAmt.m_cText.m_cParams;
    pParams->SetText(sText);
    if (nFreeBlocks < 1600)
    {
        pParams->m_bHilightText = 1;
        pParams->SetColor(CGuiInGame::COLOR_RED);
    }
    else
    {
        pParams->m_bHilightText = 0;
        pParams->SetColor(CGuiInGame::COLOR_BLUE);
    }
}
// Xbox 0x00116720
void CSWGuiSaveLoad::OnSaveErrorDone(CSWGuiControl* pControl)
{
    int nSelected = m_lbSaveGames.m_nSelectedItem;
    PopulateGameList();
    int nLast = m_lbSaveGames.m_lItems.num - 1;
    m_lbSaveGames.SetSelectedControl(nSelected < nLast ? nSelected : nLast, 0);
}
// Xbox 0x001118E0
void CSWGuiSaveLoad::ShowSaveError(unsigned long nStrRef)
{
    CSWGuiMessageBox* pMessageBox =
        g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
    pMessageBox->SetAllowCancel(0);
    pMessageBox->SetMessage(nStrRef);
    g_pAppManager->m_pClientExoApp->GetSWGuiManager()->AddPanel(
        pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
    pMessageBox->SetCallback(
        this, (CSWGuiHandler)&CSWGuiSaveLoad::OnSaveErrorDone, 0);
    g_pExoInput->SetActive(1);
    g_pAppManager->m_pClientExoApp->EnableInput();
    g_pAppManager->m_pClientExoApp->SetInputClass(2);
}
#endif
