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
#ifndef _XBOX
// PC 0x0070C480 GOG 0x0070C1D0
int CSWGuiSaveLoad::LaunchMostRecentSave()
{
    CSWGuiSaveLoadEntry* pEntry = (CSWGuiSaveLoadEntry*)m_lbSaveGames.GetControl(0);
    if (pEntry != 0)
    {
        CExoString sPCName = pEntry->GetPCName();
        CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
        if (pServerExoApp != 0)
        {
            CExoString sPartyPCName = pServerExoApp->GetPartyTable()->GetPCName();
            if (sPartyPCName.CompareNoCase(sPCName))
            {
                LoadGame(pEntry);
                return 1;
            }
            else
            {
                return 0;
            }
        }
    }
    return 0;
}
#endif

static void ConstructSaveNameEditBox()
{
    new CSWGuiSaveGameEditBox;
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
#ifndef _XBOX
// PC 0x0070BC90 GOG 0x0070B9D0
void CSWGuiSaveLoad::HandleFilterPress(CSWGuiControl* pControl)
{
    if (pControl == 0 || pControl->m_bSelected)
    {
        m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
        if (++m_nCurrentFilter >= m_lFilterNames.num)
            m_nCurrentFilter = 0;
        PopulateGameLB();
    }
}
#endif

void CSWGuiSaveNamePanel::SetTextAndPointer(CSWGuiSaveLoad* pSaveLoad, const CExoString& sText)
{
    m_pSaveLoad = pSaveLoad;
    m_edtSaveName.m_cText.SetText(sText);
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

void CSWGuiSaveLoad::OnSaveErrorDone(CSWGuiControl* pControl)
{
    int nSelected = m_lbSaveGames.m_nSelectedItem;
    PopulateGameList();
    int nLast = m_lbSaveGames.m_lItems.num - 1;
    m_lbSaveGames.SetSelectedControl(nSelected < nLast ? nSelected : nLast, 0);
}

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

#include "nwreentrantserver.h"
#include "nwrules.h"
#include "twodimarrays.h"
#include "twodimarray.h"
#include "LaunchData.h"
#include "nwcmodule.h"
void AurPersistDisplay();
void AurReleaseFrameTexture();
void ExoXboxLaunch(KOTOR_LAUNCH_DATA* pParams, const char* pszModuleName);

#define XBOX_STRREF_MISSING_LIVE_CONTENT 42496
#define XBOX_STRREF_SAVE_CORRUPT 42027

void CSWGuiSaveLoad::LoadGame(CSWGuiControl* pControl)
{
    if (m_bExitToMainMenu ||
        g_pAppManager->GetClientExoApp()->GetInGameGui()->GetMessageBox()->m_bAccepted)
    {
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
            CSWGuiMessageBox* pMessageBox = pClientExoApp->GetInGameGui()->m_pMessageBox;
            pMessageBox->SetAllowCancel(0);
            pMessageBox->SetCallback(NULL, NULL, NULL);
            CExoString sMessage;
            sMessage = g_pTlkTable->GetSimpleString(XBOX_STRREF_MISSING_LIVE_CONTENT);
            g_pTlkTable->SetCustomToken(MISSING_LIVE_CONTENT_TOKEN, pEntry->m_sMissingLiveContent);
            g_pTlkTable->ParseStr(sMessage);
            pMessageBox->SetMessage(sMessage);
            CSWGuiManager* pManager = m_pManager;
            pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
            return;
        }
        CSWPartyTable::ResetCurrentSessionStartTime();
        g_pAppManager->GetClientExoApp()->GetInGameGui()->SetGlobalUnpauseableState(1);
        if (pEntry->m_bRebootAutoSave)
        {
            g_pExoBase->m_pExoAliasList->ChangePath(CExoString("SAVES"), pEntry->m_sUnknown1F8);
            pClientExoApp->SetLoadScreenByModuleName(pEntry->m_sLastModule);
            pClientExoApp->SetLoadMusic(CExoString(""));
            pClientExoApp->ShowLoadScreen(0, 1, 0);
            pClientExoApp->SetLoadScreenByModuleName(pEntry->m_sLastModule);
            unsigned char nGamePlayHint = pEntry->m_nGameplayHint;
            pClientExoApp->SetGamePlayHint(nGamePlayHint);
            unsigned char nStoryHint = pEntry->m_nStoryHint;
            pClientExoApp->SetStoryHint(nStoryHint);
            pClientExoApp->SetLoadScreenHint(
                pClientExoApp->GetNextLoadScreenHintSTRREF(
                    CExoString(""), CClientExoAppInternal::LOAD_SCREEN_HINT_STORY));
            pClientExoApp->SetLoadScreenHintUpdateTimer(0);
            UpdateScreen(0.033333335f, 0, 0);
            if (!LoadPCAutoSave())
            {
                g_pAppManager->m_pReentrantServerStats->m_bOperationActive = 0;
                g_pAppManager->m_pReentrantServerStats->m_nOperation = 0;
                pClientExoApp->HideLoadScreen(0);
                ShowSaveError(XBOX_STRREF_SAVE_CORRUPT);
            }
        }
        else if (m_bExitToMainMenu)
        {
            g_pExoBase->m_pExoAliasList->ChangePath(CExoString("SAVES"), pEntry->m_sUnknown1F8);
            pClientExoApp->SetLoadScreenByModuleName(pEntry->m_sLastModule);
            pClientExoApp->SetLoadMusic(CExoString(""));
            pClientExoApp->ShowLoadScreen(0, 1, 0);
            unsigned char nGamePlayHint = pEntry->m_nGameplayHint;
            pClientExoApp->SetGamePlayHint(nGamePlayHint);
            unsigned char nStoryHint = pEntry->m_nStoryHint;
            pClientExoApp->SetStoryHint(nStoryHint);
            pClientExoApp->SetLoadScreenHint(
                pClientExoApp->GetNextLoadScreenHintSTRREF(
                    CExoString(""), CClientExoAppInternal::LOAD_SCREEN_HINT_STORY));
            pClientExoApp->SetLoadScreenHintUpdateTimer(0);
            CExoString sGameName = pEntry->m_sGameName;
            CExoString sLastModule = pEntry->m_sLastModule;
            if (UnpackGame(pEntry->GetSaveGame(), pEntry->m_sGameName, pControl))
            {
                m_bDeferredRemoval = 0;
                m_nExitState = EXITSTATE_DELETE;
                KOTOR_LAUNCH_DATA cLaunchData;
                cLaunchData.m_nModuleTrans = KOTOR_LAUNCH_DATA::MODULETRANS_LOADGAME;
                cLaunchData.m_nAutoCalInfo = g_pExoInput->GetAutoCalInfo();
                cLaunchData.m_nPortBinding = g_pExoInput->GetPortBinding();
                cLaunchData.m_nSaveSlot = pEntry->m_nSaveGame;
                cLaunchData.SetXboxSavePath(pEntry->m_sUnknown1F8);
                cLaunchData.m_bLoadingSave = 1;
                KOTOR_LAUNCH_OPTIONS_DATA cOptionsData;
                g_pAppManager->GetClientExoApp()->GetClientOptions()->GetOptionsData(&cOptionsData);
                cLaunchData.SetOptionsData(cOptionsData);
                CExoString sBMPResRef;
                if (g_pRules->m_p2DArrays->m_pAreaTransition->GetCExoStringEntry(
                        pEntry->m_sLastModule, CExoString("BMPResRef"), &sBMPResRef) == 0)
                {
                    cLaunchData.SetLoadScreen(sBMPResRef);
                    CExoString sLoadScreen;
                    sLoadScreen.Format("load_%s", pEntry->m_sLastModule);
                    cLaunchData.SetLoadScreen(sLoadScreen);
                    if (!g_pExoResMan->Exists(sBMPResRef, RESTYPE_TGA, (unsigned long*)NULL) &&
                        !g_pExoResMan->Exists(sBMPResRef, RESTYPE_TPC, (unsigned long*)NULL))
                    {
                        g_pRules->m_p2DArrays->m_pAreaTransition->GetCExoStringEntry(
                            CExoString("DEFAULT"), CExoString("BMPResRef"), &sLoadScreen);
                        cLaunchData.SetModuleName(pEntry->m_sLastModule);
                    }
                }
                CExoString sModuleResRef;
                CExoString sModule;
                int bMinGlobalRim = 0;
                C2DA* pMinGlobalRim = g_pRules->m_p2DArrays->m_pMinGlobalRim;
                if (pMinGlobalRim != NULL)
                {
                    for (int nRow = 0; nRow < pMinGlobalRim->m_nNumRows; nRow++)
                    {
                        if (pMinGlobalRim->GetCExoStringEntry(nRow, CExoString("ModuleResRef"), &sModuleResRef) == 1)
                        {
                            if (sModuleResRef.CompareNoCase(pEntry->m_sLastModule) == 1)
                            {
                                bMinGlobalRim = 1;
                                break;
                            }
                        }
                    }
                }
                cLaunchData.m_bMinGlobalRim = bMinGlobalRim;
                cLaunchData.SetLoadMusic(CExoString(""));
                cLaunchData.SetStartWaypoint(pEntry->m_sGameName);
                cLaunchData.m_nLoadScreenHint = g_pAppManager->GetClientExoApp()->GetLoadScreenHint();
                cLaunchData.m_nGamePlayHint = g_pAppManager->GetClientExoApp()->GetGamePlayHint();
                cLaunchData.m_nStoryHint = g_pAppManager->GetClientExoApp()->GetStoryHint();
                cLaunchData.m_nLoadScreenHintUpdateTimer =
                    g_pAppManager->GetClientExoApp()->GetLoadScreenHintUpdateTimer();
                AurPersistDisplay();
                ExoXboxLaunch(&cLaunchData, NULL);
            }
        }
        else
        {
            CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
            pClient->SetLoadScreenByModuleName(pEntry->m_sLastModule);
            pClient->SetLoadMusic(CExoString(""));
            pClient->ShowLoadScreen(0, 1, 0);
            unsigned char nGamePlayHint = pEntry->m_nGameplayHint;
            pClient->SetGamePlayHint(nGamePlayHint);
            unsigned char nStoryHint = pEntry->m_nStoryHint;
            pClient->SetStoryHint(nStoryHint);
            pClient->SetLoadScreenHint(pClient->GetNextLoadScreenHintSTRREF(
                CExoString(""), CClientExoAppInternal::LOAD_SCREEN_HINT_STORY));
            pClient->SetLoadScreenHintUpdateTimer(0);
            if (UnpackGame(pEntry->GetSaveGame(), pEntry->m_sGameName, pControl))
            {
                m_bDeferredRemoval = 0;
                m_nExitState = EXITSTATE_DELETE;
                m_pManager->PopModalPanel();
                pClient->GetInGameGui()->HideSWInGameGui();
                KOTOR_LAUNCH_DATA cLaunchData;
                cLaunchData.m_nModuleTrans = KOTOR_LAUNCH_DATA::MODULETRANS_LOADGAME;
                cLaunchData.m_nAutoCalInfo = g_pExoInput->GetAutoCalInfo();
                cLaunchData.m_nPortBinding = g_pExoInput->GetPortBinding();
                cLaunchData.m_nSaveSlot = pEntry->m_nSaveGame;
                cLaunchData.SetXboxSavePath(pEntry->m_sUnknown1F8);
                cLaunchData.m_bLoadingSave = 1;
                KOTOR_LAUNCH_OPTIONS_DATA cOptionsData;
                g_pAppManager->GetClientExoApp()->GetClientOptions()->GetOptionsData(&cOptionsData);
                cLaunchData.SetOptionsData(cOptionsData);
                CExoString sBMPResRef;
                if (g_pRules->m_p2DArrays->m_pAreaTransition->GetCExoStringEntry(
                        pEntry->m_sLastModule, CExoString("BMPResRef"), &sBMPResRef) == 0)
                {
                    cLaunchData.SetLoadScreen(sBMPResRef);
                    CExoString sLoadScreen;
                    sLoadScreen.Format("load_%s", pEntry->m_sLastModule);
                    cLaunchData.SetLoadScreen(sLoadScreen);
                    if (!g_pExoResMan->Exists(sBMPResRef, RESTYPE_TGA, (unsigned long*)NULL) &&
                        !g_pExoResMan->Exists(sBMPResRef, RESTYPE_TPC, (unsigned long*)NULL))
                    {
                        g_pRules->m_p2DArrays->m_pAreaTransition->GetCExoStringEntry(
                            CExoString("DEFAULT"), CExoString("BMPResRef"), &sLoadScreen);
                        cLaunchData.SetModuleName(pEntry->m_sLastModule);
                    }
                }
                CExoString sModuleResRef;
                CExoString sModule;
                int bMinGlobalRim = 0;
                C2DA* pMinGlobalRim = g_pRules->m_p2DArrays->m_pMinGlobalRim;
                if (pMinGlobalRim != NULL)
                {
                    for (int nRow = 0; nRow < pMinGlobalRim->m_nNumRows; nRow++)
                    {
                        if (pMinGlobalRim->GetCExoStringEntry(nRow, CExoString("ModuleResRef"), &sModuleResRef) == 1)
                        {
                            if (sModuleResRef.CompareNoCase(pEntry->m_sLastModule) == 1)
                            {
                                bMinGlobalRim = 1;
                                break;
                            }
                        }
                    }
                }
                cLaunchData.m_bMinGlobalRim = bMinGlobalRim;
                cLaunchData.SetLoadMusic(CExoString(""));
                cLaunchData.SetStartWaypoint(pEntry->m_sGameName);
                cLaunchData.m_nLoadScreenHint = g_pAppManager->GetClientExoApp()->GetLoadScreenHint();
                cLaunchData.m_nGamePlayHint = g_pAppManager->GetClientExoApp()->GetGamePlayHint();
                cLaunchData.m_nStoryHint = g_pAppManager->GetClientExoApp()->GetStoryHint();
                cLaunchData.m_nLoadScreenHintUpdateTimer =
                    g_pAppManager->GetClientExoApp()->GetLoadScreenHintUpdateTimer();
                AurPersistDisplay();
                ExoXboxLaunch(&cLaunchData, g_pAppManager->GetServerExoApp()->GetModuleName().CStr());
            }
        }
    }
}

void CSWGuiSaveLoad::RemoveGame(CSWGuiControl* pControl)
{
    CSWGuiMessageBox* pMessageBox;
    WCHAR wszTitle[128];

    pMessageBox = g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
    if (pMessageBox->m_bAccepted)
    {
        CSWGuiSaveLoadEntry* pEntry = (CSWGuiSaveLoadEntry*)pControl;
        mbstowcs(wszTitle, pEntry->m_sXboxTitle.CStr(), 128);
        if (XDeleteSaveGame("U:\\", wszTitle) == ERROR_PATH_NOT_FOUND)
        {
            g_pExoBase->m_pExoAliasList->ChangePath("SAVES", "U:\\");
            g_pExoResMan->NukeDirectory(
                CExoString("Saves:") +
                    pEntry->m_sUnknown1F8.Right(pEntry->m_sUnknown1F8.GetLength() - 3),
                1, 1);
        }
        g_pExoBase->m_pExoAliasList->ChangePath("SAVES", "U:\\");
        int nIndex = m_lbSaveGames.m_nSelectedItem;
        PopulateGameList();
        int nLastIndex = m_lbSaveGames.m_lItems.num - 1;
        m_lbSaveGames.SetSelectedControl(nIndex < nLastIndex ? nIndex : nLastIndex, 0);
        UpdateFreeBlocks();
        SetActiveControl(NULL, 0);
        SetActiveControl(&m_lbSaveGames, 0);
    }
}

#define XBOX_STRREF_NOT_ENOUGH_BLOCKS 42140

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

        CSWGuiSaveLoadEntry* pEntry = (CSWGuiSaveLoadEntry*)pControl;
        if (pEntry->m_bDataLoaded)
        {
            unsigned __int64 nFreeBytes;
            g_pExoResMan->GetFreeDiskSpace("SAVES:", &nFreeBytes);
            __int64 nFreeUnits = nFreeBytes >> 14;
            __int64 nSaveUnits = XGetDisplayBlocks(pEntry->m_sUnknown1F8.CStr());
            nFreeUnits += nSaveUnits;
            if (nFreeUnits < SAVE_SPACE_UNITS + nOptionsSpace)
            {
                CSWGuiMessageBox* pMessageBox =
                    g_pAppManager->GetClientExoApp()->GetInGameGui()->m_pMessageBox;
                int nNeededUnits = SAVE_SPACE_UNITS + nOptionsSpace - (int)nFreeUnits;
                pEntry->m_nSaveSpaceNeeded = nNeededUnits;
                g_pTlkTable->SetCustomToken(SAVE_SPACE_TOKEN_MB, CExoString(nNeededUnits));
                CExoString sMessage = g_pTlkTable->GetSimpleString(XBOX_STRREF_NOT_ENOUGH_BLOCKS);
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
                    this, (CSWGuiEventHandler)&CSWGuiSaveLoad::SaveGameToSlot, pEntry);
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
                pEntry->m_nSaveSpaceNeeded = nNeededUnits;
                g_pTlkTable->SetCustomToken(SAVE_SPACE_TOKEN_MB, CExoString(nNeededUnits));
                CExoString sMessage = g_pTlkTable->GetSimpleString(XBOX_STRREF_NOT_ENOUGH_BLOCKS);
                g_pTlkTable->ParseStr(sMessage);
                pMessageBox->SetAllowCancel(0);
                pMessageBox->SetMessage(sMessage);
                pMessageBox->SetCallback(this, NULL, NULL);
                GetManager()->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
            }
            else
            {
                SaveGameToSlot(pEntry);
            }
        }
    }
}

void CSWGuiSaveLoad::OnCorruptGameSelected(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected)
    {
        CSWGuiMessageBox* pMessageBox =
            g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
        pMessageBox->SetAllowCancel(0);
        pMessageBox->SetMessage(XBOX_STRREF_SAVE_CORRUPT);
        g_pAppManager->m_pClientExoApp->GetSWGuiManager()->AddPanel(
            pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
        pMessageBox->SetCallback(
            this, (CSWGuiHandler)&CSWGuiSaveLoad::OnSaveErrorDone, 0);
        g_pExoInput->SetActive(1);
        g_pAppManager->m_pClientExoApp->EnableInput();
        g_pAppManager->m_pClientExoApp->SetInputClass(2);
    }
}

#define XBOX_STRREF_DAMAGED_SAVE_LABEL 42121

void CSWGuiSaveLoad::ShowGame(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected)
    {
        CSWGuiSaveLoadEntry* pEntry = (CSWGuiSaveLoadEntry*)pControl;
        g_pExoBase->m_pExoAliasList->ChangePath("SAVES", pEntry->m_sUnknown1F8);
        if (!pEntry->m_bEmptySlot && pEntry->m_nSaveGame != 0)
        {
            unsigned char abSignature[XCALCSIG_SIGNATURE_SIZE];
            g_pExoResMan->CreateSignatureFile(
                pEntry->GetGameDirectory() + "\\" + "screen", 3, 0, abSignature, 0);
            if (!g_pExoResMan->VerifySignatureFile(
                    pEntry->GetGameDirectory() + "\\" + "screen", abSignature))
            {
                pEntry->m_bDataLoaded = 1;
                pEntry->m_bBadSave = 1;
                pEntry->AddEvent(GUIEVENT_ACTIVATE, this,
                    (CSWGuiEventHandler)&CSWGuiSaveLoad::OnCorruptGameSelected);
                g_pExoBase->m_pExoDebug->WriteToScriptLogFile(g_pExoBase->m_pExoDebug->m_sLogDirectory);
                g_pExoBase->m_pExoDebug->FlushScriptLogFile();
            }
        }
        if (pEntry->m_bDataLoaded && !pEntry->m_bBadSave)
        {
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
            if (pEntry->m_bMissingLiveContent)
            {
                {
                    CResRef cEmpty("");
                    m_lblScreenshot.m_cBorder.m_cParams.SetFillImage(cEmpty, 0);
                }
                CExoString sMessage;
                sMessage = g_pTlkTable->GetSimpleString(XBOX_STRREF_MISSING_LIVE_CONTENT);
                g_pTlkTable->SetCustomToken(0, pEntry->m_sMissingLiveContent);
                g_pTlkTable->ParseStr(sMessage);
                m_lblScreenshot.m_cText.m_cParams.SetText(sMessage);
            }
            else
            {
                if (pEntry->m_bRebootAutoSave)
                {
                    CResRef cScreenshot(pEntry->m_sScreenshot);
                    m_lblScreenshot.m_cBorder.m_cParams.SetFillImage(cScreenshot, 0);
                }
                else
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
                m_lblScreenshot.m_cText.m_cParams.SetText("");
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
            if (pEntry->m_bBadSave)
            {
                m_lblScreenshot.m_cText.m_cParams.SetStrRef(XBOX_STRREF_DAMAGED_SAVE_LABEL);
            }
            else
            {
                m_lblScreenshot.m_cText.m_cParams.SetStrRef(STRREF_NEW_SLOT);
            }
        }
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
    }
}

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

    unsigned char abSignature[XCALCSIG_SIGNATURE_SIZE];
    g_pExoResMan->CreateSignatureFile(sDirectory + "\\" + "savenfo", 0, 0, abSignature, 0);
    if (!g_pExoResMan->VerifySignatureFile(sDirectory + "\\" + "SAVE_INFO", abSignature))
    {
        m_bDataLoaded = 1;
        m_bBadSave = 1;
        g_pExoBase->m_pExoDebug->WriteToScriptLogFile(g_pExoBase->m_pExoDebug->m_sLogDirectory);
        g_pExoBase->m_pExoDebug->FlushScriptLogFile();
        g_pExoResMan->RemoveResourceDirectory(sDirectory);
        return;
    }

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
    m_bRebootAutoSave = pGFF->ReadFieldBYTE(&cStruct, "REBOOTAUTOSAVE", bSuccess, 0);
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

#define XBOX_SAVE_SLOT_AUTO 0

#define XBOX_RESTYPE_SAVE_IMAGE 3011

#define XBOX_STRREF_SAVE_GAME_TITLE 1594

void CSWGuiSaveLoad::SaveGameToSlot(CSWGuiControl* pControl)
{
    CSWGuiSaveLoadEntry* pEntry = (CSWGuiSaveLoadEntry*)pControl;
    unsigned long nSaveGame = pEntry->m_nSaveGame;
    if (nSaveGame > MAX_SAVE_GAME)
    {
        nSaveGame = m_nMaxSaveGame;
        if (nSaveGame > MAX_SAVE_GAME)
        {
            CSWGuiMessageBox* pMessageBox =
                g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
            CExoString sMessage = g_pTlkTable->GetSimpleString(STRREF_MAXIMUM_SAVES);
            g_pTlkTable->ParseStr(sMessage);
            pMessageBox->SetAllowCancel(0);
            pMessageBox->SetMessage(sMessage);
            m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
            return;
        }
        pEntry->m_nSaveGame = nSaveGame;
    }
    if (pEntry->m_bDataLoaded)
    {
        CSWGuiMessageBox* pMessageBox =
            g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
        if (!pMessageBox->m_bAccepted)
        {
            return;
        }
    }
    AurReleaseFrameTexture();
    WCHAR wszTitle[256];
    if (!pEntry->m_bNewSlot)
    {
        mbstowcs(wszTitle, pEntry->m_sXboxTitle.CStr(), 255);
        XDeleteSaveGame("U:\\", wszTitle);
    }
    CExoString sDrive("U:\\");
    CExoString sTitle;
    unsigned long nSeconds = g_pAppManager->m_pServerExoApp->GetPartyTable()->GetTotalPlayedSeconds();
    unsigned long nHours = nSeconds / 3600;
    unsigned long nMinutes = nSeconds / 60 % 60;
    {
        CExoString sToken((int)pEntry->m_nSaveGame);
        g_pTlkTable->SetCustomToken(0, sToken);
    }
    {
        CExoString sToken((int)nHours);
        g_pTlkTable->SetCustomToken(1, sToken);
    }
    {
        CExoString sToken((int)nMinutes);
        g_pTlkTable->SetCustomToken(2, sToken);
    }
    sTitle = g_pTlkTable->GetSimpleString(XBOX_STRREF_SAVE_GAME_TITLE);
    g_pTlkTable->ParseStr(sTitle);
    pEntry->SetXboxTitle(sTitle);
    mbstowcs(wszTitle, sTitle.CStr(), 255);
    XDeleteSaveGame(sDrive.CStr(), wszTitle);
    char szSaveDirectory[256];
    if (XCreateSaveGame(sDrive.CStr(), wszTitle, OPEN_ALWAYS, 1, szSaveDirectory, 254) == ERROR_SUCCESS)
    {
        pEntry->m_sUnknown1F8 = CExoString(szSaveDirectory);
        pEntry->m_bNewSlot = 0;
        g_pAppManager->m_pClientExoApp->BusyWaitMuteSound();
        g_pAppManager->m_pClientExoApp->BusyWaitMuteSound();
        g_pExoBase->m_pExoAliasList->ChangePath("SAVES", pEntry->m_sUnknown1F8);
        if (pEntry->m_nSaveGame == XBOX_SAVE_SLOT_AUTO)
        {
            if (!g_pExoResMan->FileCopy("HD0:AutoSaveImage", "SAVES:SaveImage",
                                        XBOX_RESTYPE_SAVE_IMAGE, RESTYPE_INVALID))
            {
                g_pExoResMan->ReportMissingFile("AutoSaveImage", XBOX_RESTYPE_SAVE_IMAGE, 1);
            }
        }
        else
        {
            if (!g_pExoResMan->FileCopy("HD0:PlaySaveImage", "SAVES:SaveImage",
                                        XBOX_RESTYPE_SAVE_IMAGE, RESTYPE_INVALID))
            {
                g_pExoResMan->ReportMissingFile("PlaySaveImage", XBOX_RESTYPE_SAVE_IMAGE, 1);
            }
        }
        CExoString sGameName;
        sGameName.Format("Slot%d", pEntry->m_nSaveGame);
        g_pAppManager->m_pClientExoApp->SendSaveGameRequest(pEntry->m_nSaveGame, sGameName);
        m_bDeferredRemoval = 0;
        m_nExitState = EXITSTATE_DELETE;
        m_pManager->PopModalPanel();
    }
    else
    {
        pEntry->m_sUnknown1F8 = CExoString("U:\\");
        m_bDeferredRemoval = 0;
        m_nExitState = EXITSTATE_DELETE;
        m_pManager->PopModalPanel();
        CSWGuiMessageBox* pMessageBox =
            g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
        CExoString sMessage = g_pTlkTable->GetSimpleString(STRREF_MAXIMUM_SAVES);
        g_pTlkTable->ParseStr(sMessage);
        pMessageBox->SetAllowCancel(0);
        pMessageBox->SetMessage(sMessage);
        m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
    }
}

#define XBOX_FUTUREGAME_LOAD_STEP 40

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

    XCALCSIG_SIGNATURE cDataSignature;
    unsigned char aHeaderSignature[XCALCSIG_SIGNATURE_SIZE];
    unsigned char aHeaderVarSignature[XCALCSIG_SIGNATURE_SIZE];
    unsigned char aGlobalsSignature[XCALCSIG_SIGNATURE_SIZE];
    unsigned char aPartySignature[XCALCSIG_SIGNATURE_SIZE];
    CERFFile* pERF = new CERFFile;
    pERF->Read(sFileName, aHeaderSignature);
    if (!g_pExoResMan->VerifySignatureFile(sSavePath + "\\" + "SAVE_HEADER", aHeaderSignature))
    {
        g_pExoBase->m_pExoDebug->WriteToScriptLogFile(g_pExoBase->m_pExoDebug->m_sLogDirectory);
        g_pExoBase->m_pExoDebug->FlushScriptLogFile();
        delete pERF;
        return 0;
    }
    pERF->ReadHeaderVariance(sFileName, aHeaderVarSignature);
    if (!g_pExoResMan->VerifySignatureFile(sSavePath + "\\" + "SAVE_HEADERVAR", aHeaderVarSignature))
    {
        delete pERF;
        return 0;
    }
    HANDLE hSignature = XCalculateSignatureBegin(0);
    pERF->ExportFilesFromERF(
        sFileName, "FUTUREGAME:", hSignature, LoadScreenUpdate, 0, XBOX_FUTUREGAME_LOAD_STEP);
    XCalculateSignatureEnd(hSignature, &cDataSignature);
    if (!g_pExoResMan->VerifySignatureFile(sSavePath + "\\" + "SAVE_DATA", (unsigned char*)&cDataSignature))
    {
        g_pExoBase->m_pExoDebug->WriteToScriptLogFile(g_pExoBase->m_pExoDebug->m_sLogDirectory);
        g_pExoBase->m_pExoDebug->FlushScriptLogFile();
        return 0;
    }
    g_pExoResMan->CreateSignatureFile(sSavePath + "\\" + "GLOBALVARS", 0, 0, aGlobalsSignature, 0);
    if (!g_pExoResMan->VerifySignatureFile(sSavePath + "\\" + "SAVE_VARS", aGlobalsSignature))
    {
        g_pExoBase->m_pExoDebug->WriteToScriptLogFile(g_pExoBase->m_pExoDebug->m_sLogDirectory);
        g_pExoBase->m_pExoDebug->FlushScriptLogFile();
        return 0;
    }
    g_pExoResMan->CreateSignatureFile(sSavePath + "\\" + "PARTYTABLE", 0, 0, aPartySignature, 0);
    if (!g_pExoResMan->VerifySignatureFile(sSavePath + "\\" + "SAVE_PARTY", aPartySignature))
    {
        g_pExoBase->m_pExoDebug->WriteToScriptLogFile(g_pExoBase->m_pExoDebug->m_sLogDirectory);
        g_pExoBase->m_pExoDebug->FlushScriptLogFile();
        return 0;
    }
    delete pERF;
    return 1;
}

#define XBOX_MARK_SAVE_CORRUPT(sPath)                                                                   \
    CExoFile cCorruptFile(sPath + "\\" + "CORRUPT", 0, "wb+");                                       \
    cCorruptFile.Write("CORRUPT");                                                                    \
    cCorruptFile.Flush();                                                                             \
    g_pAppManager->m_pReentrantServerStats->StartStallEvent(1);                                       \
    g_pAppManager->m_pReentrantServerStats->m_nUnknown56 = 4;                                         \
    g_pExoBase->m_pExoDebug->WriteToScriptLogFile(g_pExoBase->m_pExoDebug->m_sLogDirectory);          \
    g_pExoBase->m_pExoDebug->FlushScriptLogFile()

int CSWGuiSaveLoad::LoadPCAutoSave()
{

    CExoString sSavePath;
    CExoString sDirectory;
    CExoString sFileName;
    CExoString sTemp;

    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(0, 1);
    UpdateScreen(0.033333335f, 0, 0);
    sDirectory.Format("%06d - %s", XBOX_SAVE_SLOT_AUTO, "AUTOSAVE");
    sSavePath = "SAVES:";
    sSavePath = sSavePath + sDirectory;
    if (!g_pExoResMan->CreateDirectory2("GAMEINPROGRESS:"))
    {
        g_pExoResMan->CleanDirectory("GAMEINPROGRESS:", 0, 0);
    }
    if (!g_pExoResMan->CreateDirectory2("REBOOTDATA:"))
    {
        g_pExoResMan->CleanDirectory("REBOOTDATA:", 0, 0);
    }
    g_pExoResMan->CreateDirectory2("TEMP:");

    XCALCSIG_SIGNATURE cDataSignature;
    XCALCSIG_SIGNATURE cRebootSignature;
    XCALCSIG_SIGNATURE cInfoSignature;
    XCALCSIG_SIGNATURE cPlayerSignature;
    unsigned char aHeaderSignature[XCALCSIG_SIGNATURE_SIZE];
    unsigned char aHeaderVarSignature[XCALCSIG_SIGNATURE_SIZE];
    HANDLE hSignature;

    sFileName = sSavePath + "\\" + "SAVEGAME";
    CERFFile* pERF = new CERFFile;
    pERF->Read(sFileName, aHeaderSignature);
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(5, 1);
    UpdateScreen(0.033333335f, 0, 0);
    if (!g_pExoResMan->VerifySignatureFile(sSavePath + "\\" + "SAVE_HEADER", aHeaderSignature))
    {
        XBOX_MARK_SAVE_CORRUPT(sSavePath);
        delete pERF;
        return 0;
    }
    pERF->ReadHeaderVariance(sFileName, aHeaderVarSignature);
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(10, 1);
    UpdateScreen(0.033333335f, 0, 0);
    if (!g_pExoResMan->VerifySignatureFile(sSavePath + "\\" + "SAVE_HEADERVAR", aHeaderVarSignature))
    {
        XBOX_MARK_SAVE_CORRUPT(sSavePath);
        delete pERF;
        return 0;
    }
    hSignature = XCalculateSignatureBegin(0);
    pERF->ExportFilesFromERF(sFileName, "GAMEINPROGRESS:", hSignature, LoadScreenUpdate, 10, 10);
    XCalculateSignatureEnd(hSignature, &cDataSignature);
    delete pERF;

    sFileName = sSavePath + "\\" + "REBOOT";
    pERF = new CERFFile;
    pERF->Read(sFileName, aHeaderSignature);
    if (!g_pExoResMan->VerifySignatureFile(sSavePath + "\\" + "REBOOT_HEADER", aHeaderSignature))
    {
        XBOX_MARK_SAVE_CORRUPT(sSavePath);
        delete pERF;
        return 0;
    }
    pERF->ReadHeaderVariance(sFileName, aHeaderVarSignature);
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(25, 1);
    UpdateScreen(0.033333335f, 0, 0);
    if (!g_pExoResMan->VerifySignatureFile(sSavePath + "\\" + "REBOOT_HEADERVAR", aHeaderVarSignature))
    {
        XBOX_MARK_SAVE_CORRUPT(sSavePath);
        delete pERF;
        return 0;
    }
    hSignature = XCalculateSignatureBegin(0);
    pERF->ExportFilesFromERF(sFileName, "REBOOTDATA:", hSignature, LoadScreenUpdate, 25, 5);
    XCalculateSignatureEnd(hSignature, &cRebootSignature);
    delete pERF;

    g_pExoResMan->AddResourceDirectory(sSavePath);
    CResGFF* pGFF = new CResGFF(0, "NFO ", "savenfo");
    CResStruct cTopLevel;
    pGFF->GetTopLevelStruct(&cTopLevel);
    if (pGFF->GetSize() > 0)
    {
        hSignature = XCalculateSignatureBegin(0);
        XCalculateSignatureUpdate(hSignature, (BYTE*)pGFF->GetData(), pGFF->GetSize());
        XCalculateSignatureEnd(hSignature, &cInfoSignature);
    }
    else
    {
        memset(&cInfoSignature, 0, sizeof(cInfoSignature));
    }
    sFileName = sSavePath + "\\";
    CExoFile cPlayerInfo(sFileName + "pifo", RESTYPE_IFO, "rb");
    if (cPlayerInfo.GetSize() > 0)
    {
        BYTE* pBuffer = new BYTE[cPlayerInfo.GetSize()];
        cPlayerInfo.Read(pBuffer, 1, cPlayerInfo.GetSize());
        hSignature = XCalculateSignatureBegin(0);
        XCalculateSignatureUpdate(hSignature, pBuffer, cPlayerInfo.GetSize());
        XCalculateSignatureEnd(hSignature, &cPlayerSignature);
        delete[] pBuffer;
    }
    else
    {
        memset(&cPlayerSignature, 0, sizeof(cPlayerSignature));
    }
    g_pAppManager->GetClientExoApp()->SetLoadBarProgress(35, 1);
    UpdateScreen(0.033333335f, 0, 0);
    bool bCorrupt =
        !g_pExoResMan->VerifySignatureFile(sFileName + "SAVE_DATA", (unsigned char*)&cDataSignature) ||
        !g_pExoResMan->VerifySignatureFile(sFileName + "SAVE_INFO", (unsigned char*)&cInfoSignature) ||
        !g_pExoResMan->VerifySignatureFile(sFileName + "REBOOT_DATA", (unsigned char*)&cRebootSignature) ||
        !g_pExoResMan->VerifySignatureFile(sFileName + "PLAYER_INFO", (unsigned char*)&cPlayerSignature);
    if (bCorrupt)
    {
        XBOX_MARK_SAVE_CORRUPT(sSavePath);
        delete pGFF;
        g_pExoResMan->RemoveResourceDirectory(sSavePath);
        return 0;
    }
    KOTOR_LAUNCH_DATA cLaunchData;
    CResStruct cLaunchParams;
    if (pGFF->GetStructFromStruct(&cLaunchParams, &cTopLevel, "LAUNCHPARAMS"))
    {
        cLaunchData.LoadFromGFF(pGFF, cLaunchParams);
        g_pExoResMan->FileCopy(sFileName + "pifo", "TEMP:pifo", RESTYPE_IFO, RESTYPE_INVALID);
        delete pGFF;
        g_pAppManager->GetClientExoApp()->SetLoadBarProgress(40, 1);
        UpdateScreen(0.033333335f, 0, 0);
        cLaunchData.m_nAutoCalInfo = g_pExoInput->GetAutoCalInfo();
        cLaunchData.m_nPortBinding = g_pExoInput->GetPortBinding();
        KOTOR_LAUNCH_OPTIONS_DATA cOptionsData;
        g_pAppManager->GetClientExoApp()->GetClientOptions()->GetOptionsData(&cOptionsData);
        cLaunchData.SetOptionsData(cOptionsData);
        cLaunchData.m_nLoadScreenHint = g_pAppManager->GetClientExoApp()->GetLoadScreenHint();
        cLaunchData.SetLoadMusic(CExoString(""));
        cLaunchData.m_nGamePlayHint = g_pAppManager->GetClientExoApp()->GetGamePlayHint();
        cLaunchData.m_nStoryHint = g_pAppManager->GetClientExoApp()->GetStoryHint();
        cLaunchData.m_nLoadScreenHintUpdateTimer =
            g_pAppManager->GetClientExoApp()->GetLoadScreenHintUpdateTimer();
        cLaunchData.m_bLoadAutoSave = 1;
        g_pExoResMan->RemoveResourceDirectory(sSavePath);
        AurPersistDisplay();
        ExoXboxLaunch(&cLaunchData, m_bExitToMainMenu ? NULL
            : g_pAppManager->GetClientExoApp()->GetModule()->GetModuleTitle().CStr());
        return 1;
    }
    XBOX_MARK_SAVE_CORRUPT(sSavePath);
    delete pGFF;
    g_pExoResMan->RemoveResourceDirectory(sSavePath);
    return 0;
}

#define XBOX_SAVE_SLOT_FIRST_NORMAL 1

#define XBOX_SAVE_SLOT_CORRUPT 1000

void CSWGuiSaveLoad::PopulateGameList()
{
    CExoArrayList<CExoString> lDirectories;
    CExoArrayList<CSWGuiControl*> lControls;
    CSWGuiExtent cExtent;
    cExtent.m_nHeight = 32;
    CSWGuiButton* pProto = (CSWGuiButton*)m_lbSaveGames.m_pProtoItem;
    XGAME_FIND_DATA cFindData;
    char szSaveName[128];

    if (g_pExoResMan->CreateDirectory2("TEMP:") == 0)
    {
        g_pExoResMan->CleanDirectory("TEMP:", 0, 0);
    }
    HANDLE hFind = XFindFirstSaveGame("U:\\", &cFindData);
    while (hFind != INVALID_HANDLE_VALUE)
    {
        memset(szSaveName, 0, sizeof(szSaveName));
        g_pExoBase->WcsToMbs(szSaveName, cFindData.szSaveGameName, 127);
        CExoString sSavesPath = g_pExoBase->m_pExoAliasList->GetAliasPath(CExoString("SAVES"), 0);
        g_pExoBase->m_pExoAliasList->ChangePath("SAVES", CExoString(cFindData.szSaveGameDirectory));
        lDirectories.RemoveAll(0);
        g_pExoBase->GetDirectoryList(&lDirectories, "SAVES:", RESTYPE_INVALID, 1, 1);
        if (lDirectories.num > 0)
        {
            CSWGuiSaveLoadEntry* pEntry = new CSWGuiSaveLoadEntry;
            pEntry->Initialize(cExtent, pProto);
            pEntry->m_sUnknown1F8 = CExoString(cFindData.szSaveGameDirectory);
            pEntry->LoadData(lDirectories[0]);
            pEntry->SetXboxTitle(CExoString(szSaveName));
            g_pExoBase->m_pExoAliasList->ChangePath(CExoString("SAVES"), sSavesPath);
            if (m_bSaveMode && pEntry->m_nSaveGame == XBOX_SAVE_SLOT_AUTO)
            {
                delete pEntry;
            }
            else
            {
                pEntry->AddEvent(GUIEVENT_ACTIVATE, this,
                    m_bSaveMode ? (CSWGuiEventHandler)&CSWGuiSaveLoad::HandleSaveButton
                    : !pEntry->m_bBadSave ? (CSWGuiEventHandler)&CSWGuiSaveLoad::VerifyLoadGame
                    : (CSWGuiEventHandler)&CSWGuiSaveLoad::OnCorruptGameSelected);
                pEntry->AddEvent(GUIEVENT_DELETE, this,
                    (CSWGuiEventHandler)&CSWGuiSaveLoad::DeleteGame);
                pEntry->AddEvent(GUIEVENT_MOUSE_ENTER, this,
                    (CSWGuiEventHandler)&CSWGuiSaveLoad::ShowGame);
                int nIndex = 0;
                while (lControls.num > 0 && nIndex < lControls.num &&
                       strcmp(((CSWGuiSaveLoadEntry*)lControls[nIndex])->GetGameDirectory().CStr(),
                              pEntry->GetGameDirectory().CStr()) < 0)
                {
                    nIndex++;
                }
                lControls.Insert(pEntry, nIndex);
            }
        }
        else
        {
            CSWGuiSaveLoadEntry* pEntry = new CSWGuiSaveLoadEntry;
            pEntry->Initialize(cExtent, pProto);
            pEntry->m_sUnknown1F8 = CExoString(cFindData.szSaveGameDirectory);
            pEntry->m_nSaveGame = XBOX_SAVE_SLOT_CORRUPT;
            pEntry->SetXboxTitle(CExoString(szSaveName));
            pEntry->m_bBadSave = 1;
            pEntry->AddEvent(GUIEVENT_DELETE, this,
                (CSWGuiEventHandler)&CSWGuiSaveLoad::DeleteGame);
            pEntry->AddEvent(GUIEVENT_MOUSE_ENTER, this,
                (CSWGuiEventHandler)&CSWGuiSaveLoad::ShowGame);
            pEntry->AddEvent(GUIEVENT_ACTIVATE, this,
                (CSWGuiEventHandler)&CSWGuiSaveLoad::OnCorruptGameSelected);
            int nIndex = 0;
            while (lControls.num > 0 && nIndex < lControls.num &&
                   strcmp(((CSWGuiSaveLoadEntry*)lControls[nIndex])->GetGameDirectory().CStr(),
                          pEntry->GetGameDirectory().CStr()) < 0)
            {
                nIndex++;
            }
            lControls.Insert(pEntry, nIndex);
        }
        if (!XFindNextSaveGame(hFind, &cFindData))
        {
            GetLastError();
            XFindClose(hFind);
            hFind = INVALID_HANDLE_VALUE;
        }
    }
    int nNextSlot = XBOX_SAVE_SLOT_FIRST_NORMAL;
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
        pEntry->m_nSaveGame = max(XBOX_SAVE_SLOT_FIRST_NORMAL, nMaxSlot + 1);
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

void KOTOR_LAUNCH_OPTIONS_DATA::LoadFromGFF(CResGFF* pRes, CResStruct& stStruct)
{
    int bSuccess;
    CResList lstMoviesShown;
    CResStruct stMovieShown;

    m_bAutoLevelNPCs = pRes->ReadFieldBYTE(&stStruct, "AUTOLEVELNPCS", bSuccess, m_bAutoLevelNPCs);
    m_bAutoCentre = pRes->ReadFieldBYTE(&stStruct, "AUTOCENTRE", bSuccess, m_bAutoCentre);
    m_bAutoSave = pRes->ReadFieldBYTE(&stStruct, "AUTOSAVE", bSuccess, m_bAutoSave);
    m_bReverse = pRes->ReadFieldBYTE(&stStruct, "REVERSE", bSuccess, m_bReverse);
    m_bCombatMove = pRes->ReadFieldBYTE(&stStruct, "COMBATMOVE", bSuccess, m_bCombatMove);
    m_bHideUnequippable = pRes->ReadFieldBYTE(&stStruct, "HIDEUNEQUIPPABLE", bSuccess, m_bHideUnequippable);
    m_bShowTutorial = pRes->ReadFieldBYTE(&stStruct, "SHOWTUTORIAL", bSuccess, m_bShowTutorial);
    m_bShowSubtitles = pRes->ReadFieldBYTE(&stStruct, "SHOWSUBTITLES", bSuccess, m_bShowSubtitles);
    m_bShowMiniMap = pRes->ReadFieldBYTE(&stStruct, "SHOWMINIMAP", bSuccess, m_bShowMiniMap);
    m_bRumble = pRes->ReadFieldBYTE(&stStruct, "RUMBLE", bSuccess, m_bRumble);
    m_bShowFloaty = pRes->ReadFieldBYTE(&stStruct, "SHOWFLOATY", bSuccess, m_bShowFloaty);
    m_bEndRound = pRes->ReadFieldBYTE(&stStruct, "ENDROUND", bSuccess, m_bEndRound);
    m_bEnemySighted = pRes->ReadFieldBYTE(&stStruct, "ENEMYSIGHTED", bSuccess, m_bEnemySighted);
    m_bPartyKilled = pRes->ReadFieldBYTE(&stStruct, "PARTYKILLED", bSuccess, m_bPartyKilled);
    m_bUseHorzMenu = pRes->ReadFieldBYTE(&stStruct, "USEHORZMENU", bSuccess, m_bUseHorzMenu);
    m_bUseVertMenu = pRes->ReadFieldBYTE(&stStruct, "USEVERTMENU", bSuccess, m_bUseVertMenu);
    m_bTrigger = pRes->ReadFieldBYTE(&stStruct, "TRIGGER", bSuccess, m_bTrigger);
    m_bShowSummary = pRes->ReadFieldBYTE(&stStruct, "SHOWSUMMARY", bSuccess, m_bShowSummary);
    m_nDifficultyOption = pRes->ReadFieldBYTE(&stStruct, "DIFFICULTYOPTION", bSuccess, m_nDifficultyOption);
    m_nGamma = pRes->ReadFieldBYTE(&stStruct, "GAMMA", bSuccess, m_nGamma);
    m_fGammaMin = pRes->ReadFieldFLOAT(&stStruct, "GAMMAMIN", bSuccess, m_fGammaMin);
    m_fGammaMax = pRes->ReadFieldFLOAT(&stStruct, "GAMMAMAX", bSuccess, m_fGammaMax);
    m_nVolumeMusic = pRes->ReadFieldBYTE(&stStruct, "VOLUMEMUSIC", bSuccess, m_nVolumeMusic);
    m_nVolumeVO = pRes->ReadFieldBYTE(&stStruct, "VOLUMEVO", bSuccess, m_nVolumeVO);
    m_nVolumeFX = pRes->ReadFieldBYTE(&stStruct, "VOLUMEFX", bSuccess, m_nVolumeFX);
    m_nNumMovies = pRes->ReadFieldBYTE(&stStruct, "NUMMOVIES", bSuccess, m_nNumMovies);
    if (pRes->GetList(&lstMoviesShown, &stStruct, "MOVIESSHOWN"))
    {
        int nMovies = min(11, pRes->GetListCount(&lstMoviesShown));
        for (int nMovie = 0; nMovie < nMovies; nMovie++)
        {
            if (pRes->GetListElement(&stMovieShown, &lstMoviesShown, nMovie))
            {
                m_aMoviesShown[nMovie] = pRes->ReadFieldBYTE(&stMovieShown, "MOVIESHOWN", bSuccess,
                                                             m_aMoviesShown[nMovie]);
            }
        }
    }
}

void KOTOR_LAUNCH_DATA::LoadFromGFF(CResGFF* pRes, CResStruct& stStruct)
{
    int bSuccess;
    CResStruct stSub;

    m_nModuleTrans = pRes->ReadFieldDWORD(&stStruct, "MODULETRANS", bSuccess, m_nModuleTrans);
    m_bRenderQA = pRes->ReadFieldBYTE(&stStruct, "RENDERQA", bSuccess, m_bRenderQA);
    m_bLowDiskMode = pRes->ReadFieldBYTE(&stStruct, "LOWDISKMODE", bSuccess, m_bLowDiskMode);
    m_nAutoCalInfo = pRes->ReadFieldDWORD(&stStruct, "AUTOCALINFO", bSuccess, m_nAutoCalInfo);
    m_nPortBinding = pRes->ReadFieldDWORD(&stStruct, "PORTBINDING", bSuccess, m_nPortBinding);
    m_nSaveSlot = pRes->ReadFieldINT(&stStruct, "SAVESLOT", bSuccess, m_nSaveSlot);
    SetXboxSavePath(pRes->ReadFieldCExoString(&stStruct, "XBOXSAVEPATH", bSuccess, CExoString(m_szXboxSavePath)));
    SetModuleName(pRes->ReadFieldCExoString(&stStruct, "MODULENAME", bSuccess, CExoString(m_szModuleName)));
    SetLoadScreen(pRes->ReadFieldCExoString(&stStruct, "LOADSCREEN", bSuccess, CExoString(m_szLoadScreen)));
    SetLoadMusic(pRes->ReadFieldCExoString(&stStruct, "LOADMUSIC", bSuccess, CExoString(m_szLoadMusic)));
    SetStartWaypoint(pRes->ReadFieldCExoString(&stStruct, "STARTWAYPOINT", bSuccess, CExoString(m_szStartWaypoint)));
    SetMovie1(pRes->ReadFieldCExoString(&stStruct, "MOVIE1", bSuccess, CExoString(m_szMovie1)));
    SetMovie2(pRes->ReadFieldCExoString(&stStruct, "MOVIE2", bSuccess, CExoString(m_szMovie2)));
    SetMovie3(pRes->ReadFieldCExoString(&stStruct, "MOVIE3", bSuccess, CExoString(m_szMovie3)));
    SetMovie4(pRes->ReadFieldCExoString(&stStruct, "MOVIE4", bSuccess, CExoString(m_szMovie4)));
    SetMovie5(pRes->ReadFieldCExoString(&stStruct, "MOVIE5", bSuccess, CExoString(m_szMovie5)));
    SetMovie6(pRes->ReadFieldCExoString(&stStruct, "MOVIE6", bSuccess, CExoString(m_szMovie6)));
    m_bMinGlobalRim = pRes->ReadFieldBYTE(&stStruct, "MINGLOBALRIM", bSuccess, m_bMinGlobalRim);
    m_bLoadingSave = pRes->ReadFieldBYTE(&stStruct, "LOADINGSAVE", bSuccess, m_bLoadingSave);
    m_bLoadAutoSave = pRes->ReadFieldBYTE(&stStruct, "LOAD_AUTOSAVE", bSuccess, m_bLoadAutoSave);
    unsigned long nYear = pRes->ReadFieldDWORD(&stStruct, "TIME_YEAR", bSuccess, 0);
    unsigned char nMonth = pRes->ReadFieldBYTE(&stStruct, "TIME_MONTH", bSuccess, 0);
    unsigned char nDay = pRes->ReadFieldBYTE(&stStruct, "TIME_DAY", bSuccess, 0);
    unsigned char nHour = pRes->ReadFieldBYTE(&stStruct, "TIME_HOUR", bSuccess, 0);
    unsigned short nMinute = pRes->ReadFieldWORD(&stStruct, "TIME_MINUTE", bSuccess, 0);
    unsigned short nSecond = pRes->ReadFieldWORD(&stStruct, "TIME_SECOND", bSuccess, 0);
    unsigned short nMillisecond = pRes->ReadFieldWORD(&stStruct, "TIME_MILLISECOND", bSuccess, 0);
    SetTime(nYear, nMonth, nDay, nHour, nMinute, nSecond, nMillisecond);
    m_nTimePauseDay = pRes->ReadFieldDWORD(&stStruct, "TIME_PAUSEDAY", bSuccess, 0);
    m_nTimePauseTime = pRes->ReadFieldDWORD(&stStruct, "TIME_PAUSETIME", bSuccess, 0);
    if (pRes->GetStructFromStruct(&stSub, &stStruct, "OPTIONSDATA"))
    {
        m_cOptionsData.LoadFromGFF(pRes, stSub);
    }
    if (pRes->GetStructFromStruct(&stSub, &stStruct, "STATUSSUMMARY"))
    {
        m_cStatusSummary.LoadFromGFF(pRes, stSub);
    }
    unsigned char nDefault = m_nGamePlayHint;
    m_nGamePlayHint = pRes->ReadFieldBYTE(&stStruct, "GAMEPLAYHINT", bSuccess, nDefault);
    nDefault = m_nStoryHint;
    m_nStoryHint = pRes->ReadFieldBYTE(&stStruct, "STORYHINT", bSuccess, nDefault);
}
#endif
// PC 0x00707510 GOG 0x00707100
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
    g_pAppManager->m_pServerExoApp->GetPartyTable()->m_n248 = nSlot;
#ifdef _XBOX
    g_pAppManager->GetClientExoApp()->SendSaveGameRequest(pEntry->m_nSaveGame, sGameName);
#else
    g_pAppManager->GetClientExoApp()->SendSaveGameRequest(pEntry->GetSaveGame(), sGameName,
                                                          sSaveName);
#endif
    m_bDeferredRemoval = 0;
    m_nExitState = EXITSTATE_DELETE;
    GetManager()->PopModalPanel();
}
// PC 0x0070C570 GOG 0x0070C2C0
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
#ifndef _XBOX
            if (m_bExitToMainMenu || m_bEndGameMode)
#else
            if (m_bExitToMainMenu)
#endif
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
// PC 0x00708920 GOG 0x00708660
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
// PC 0x0070CD30 GOG 0x0070CA80
CSWGuiSaveLoad::CSWGuiSaveLoad(CSWGuiManager* pManager, int bSaveMode,
                             int bExitToMainMenu, int bEndGameMode)
    : CSWGuiPanel(pManager)
{
#ifdef _XBOX
    StartLoadFromLayout("saveload");
#else
    StartLoadFromLayout("saveload_p");
#endif
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
    InitControl(m_lblPCName, "LBL_PCNAME", 1);
    InitControl(m_lblTimePlayed, "LBL_TIMEPLAYED", 1);
    InitControl(m_btnSaveLoad, "BTN_SAVELOAD", 1);
    InitControl(m_btnBack, "BTN_BACK", 1);
    InitControl(m_btnDelete, "BTN_DELETE", 1);
    InitControl(m_btnFilter, "BTN_FILTER", 1);
    InitControl(m_alblBar[0], "LBL_BAR1", 1);
    InitControl(m_alblBar[1], "LBL_BAR2", 1);
    InitControl(m_alblBar[2], "LBL_BAR3", 1);
    InitControl(m_alblBar[3], "LBL_BAR4", 1);
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
    m_btnFilter.SetFocusable(0);
    m_btnFilter.AddEvent(GUIEVENT_ACTIVATE, this,
                        (CSWGuiHandler)&CSWGuiSaveLoad::HandleFilterPress);
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
#ifdef _XBOX
    pProto->m_cBorder.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
    pProto->m_cHilite.m_cParams.SetColor(CGuiInGame::COLOR_YELLOW);
#endif
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
    m_nCurrentFilter = 0;
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
#ifndef _XBOX
    m_bEndGameMode = bEndGameMode;
    m_n1B6C = 0;
    m_bFiltersLoaded = 0;
#endif
    PopulateGameList();
    g_pExoResMan->AddResourceDirectory("TEMP:");
    SetActiveControl(&m_lbSaveGames, 0);
}
// PC 0x00708300 GOG 0x00707EF0
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
// PC 0x00708420 GOG 0x00708010
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
// PC 0x00708470 GOG 0x00708060
void CSWGuiSaveNamePanel::OnPanelRemoved()
{
    CSWGuiPanel::OnPanelRemoved();
#ifndef _XBOX
    g_pGuiMan->RestoreOldMousePosition();
#endif
}
// PC 0x00708500 GOG 0x007080F0
void CSWGuiSaveNamePanel::HandleCancelButton(CSWGuiControl* pControl)
{
    m_bDeferredRemoval = 0;
    m_nExitState = EXITSTATE_DELETE;
    m_pManager->PopModalPanel();
}
#ifndef _XBOX
// PC 0x00708520 GOG 0x00708110
CExoString CSWGuiSaveLoadEntry::FormatTimeToString()
{
    unsigned __int64 nTimeStamp = m_nTimeStamp;
    if (nTimeStamp == 0)
    {
        if (g_pAppManager->m_pServerExoApp != 0)
        {
            nTimeStamp = g_pAppManager->m_pServerExoApp->GetSystemTimeAsDWORD64();
            m_nTimeStamp = nTimeStamp;
        }
        else
        {
            return CExoString("");
        }
    }
    FILETIME sFileTime;
    *(unsigned __int64*)&sFileTime = nTimeStamp;
    SYSTEMTIME sSystemTime;
    FileTimeToSystemTime(&sFileTime, &sSystemTime);

    CExoString sDay;
    CExoString sYear;
    CExoString sHour;
    CExoString sMinute;
    CExoString sSecond;
    CExoString sMonth;
    sMonth.Format("%.2d", sSystemTime.wMonth);
    sYear.Format("%.4d", sSystemTime.wYear);
    sDay.Format("%.2d", sSystemTime.wDay);
    sHour.Format("%.2d", sSystemTime.wHour);
    sMinute.Format("%.2d", sSystemTime.wMinute);
    sSecond.Format("%.2d", sSystemTime.wSecond);

    CExoString sTimeStamp;
    sTimeStamp = sTimeStamp + sHour + ":" + sMinute + ":" + sSecond;
    sTimeStamp = sTimeStamp + " - ";
    sTimeStamp = sTimeStamp + sDay + ", " + sMonth + ", " + sYear;
    return sTimeStamp;
}
#endif
// PC 0x0070BD00 GOG 0x0070BA40
void CSWGuiSaveLoad::HandleInputEvent(int nEvent, int nState)
{
    if (nState != 0)
    {
        switch (nEvent)
        {
        case 40:
        case 45:
        case 46:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
#ifndef _XBOX
            if (!m_bEndGameMode)
            {
                ExitPanel();
            }
            else
            {
                g_pAppManager->m_pClientExoApp->SetEndGamePending(1, 0.0f, 1);
                m_pManager->RemovePanel(this);
                m_bDeferredRemoval = 0;
                m_nExitState = EXITSTATE_DELETE;
            }
#else
            ExitPanel();
#endif
            break;
#ifndef _XBOX
        case 41:
            HandleFilterPress(0);
            break;
#endif
        }
    }
    CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
// PC 0x0070A8C0 GOG 0x0070A600
CSWGuiSaveLoad::~CSWGuiSaveLoad()
{
#ifndef _XBOX
    for (int i = 0; i < m_lEntries.num; i++)
    {
        delete m_lEntries[i];
    }
#endif
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
#ifndef _XBOX
// PC 0x007088D0 GOG 0x00708610
void CSWGuiSaveLoad::SetEndGameMode()
{
    m_bEndGameMode = 1;
}
void CSWGuiSaveLoad::LoadPCAutoSave(const CExoString& sModuleName)  // PC 0x00708C50 GOG 0x00708990
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
#endif
// PC 0x0070B0D0 GOG 0x0070AE10
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
#ifndef _XBOX
void CSWGuiSaveLoad::RemoveGame(CSWGuiControl* pControl)  // PC 0x00709490 GOG 0x007091D0
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
// PC 0x00709560 GOG 0x007092A0
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
#ifdef _XBOX
                CExoString sPlanet = pEntry->m_sAreaName.Left(nSeparator - 1);
                CExoString sArea = pEntry->m_sAreaName.Right(
                    pEntry->m_sAreaName.GetLength() - nSeparator - 2);
#else
                CExoString sAreaName = pEntry->m_sAreaName;
                CExoString sPlanet;
                CExoString sArea;
                if (sAreaName[nSeparator] == ' ')
                {
                    sPlanet = sAreaName.Left(nSeparator - 1);
                }
                else
                {
                    sPlanet = sAreaName.Left(nSeparator);
                }
                if (sAreaName[nSeparator + 1] == ' ')
                {
                    sArea = sAreaName.Right(sAreaName.GetLength() - nSeparator - 2);
                }
                else
                {
                    sArea = sAreaName.Right(sAreaName.GetLength() - nSeparator - 1);
                }
#endif
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
            m_btnSaveLoad.m_cText.m_cParams.SetDefaultColor(
                pEntry->m_bMissingLiveContent ? CGuiInGame::COLOR_NEW_GREEN : CGuiInGame::COLOR_NEW_TEXT);
            m_btnSaveLoad.m_cBorder.m_cParams.m_vColor =
                pEntry->m_bMissingLiveContent ? CGuiInGame::DISABLED_TEXT : CGuiInGame::COLOR_NEW_GREEN;
        }
        else
        {
            m_btnSaveLoad.m_cText.m_cParams.SetDefaultColor(CGuiInGame::COLOR_NEW_TEXT);
            m_btnSaveLoad.m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_GREEN;
        }
        m_btnDelete.m_cText.m_cParams.SetDefaultColor(
            pEntry->m_bDataLoaded ? CGuiInGame::COLOR_NEW_TEXT : CGuiInGame::COLOR_NEW_GREEN);
        m_btnDelete.m_cBorder.m_cParams.m_vColor =
            pEntry->m_bDataLoaded ? CGuiInGame::COLOR_NEW_GREEN : CGuiInGame::DISABLED_TEXT;
        CExoString sTimePlayed;
        unsigned long nTimePlayed = pEntry->m_nTimePlayed;
        unsigned long nHours = nTimePlayed / 3600;
        unsigned long nMinutes = (nTimePlayed / 60) % 60;
        g_pTlkTable->SetCustomToken(CSWGuiSaveLoadEntry::TIME_TOKEN_HOURS, nHours);
        g_pTlkTable->SetCustomToken(CSWGuiSaveLoadEntry::TIME_TOKEN_MINUTES, nMinutes);
        sTimePlayed = g_pTlkTable->GetSimpleString(STRREF_TIME_PLAYED);
        g_pTlkTable->ParseStr(sTimePlayed);
        m_lblTimePlayed.m_cText.m_cParams.SetText(sTimePlayed);
#endif
    }
}
// PC 0x007076A0 GOG 0x00707290
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
    m_nTimeStamp = pGFF->ReadFieldDWORD64(&cStruct, "TIMESTAMP", bSuccess, 0);
    m_sPCName = pGFF->ReadFieldCExoString(&cStruct, "PCNAME", bSuccess, "");
    m_nSaveNumber = pGFF->ReadFieldDWORD(&cStruct, "SAVENUMBER", bSuccess, 0);
    unsigned char nDefault = m_nGameplayHint;
    m_nGameplayHint = pGFF->ReadFieldBYTE(&cStruct, "GAMEPLAYHINT", bSuccess, nDefault);
    nDefault = GetStoryHint(0);
    m_anStoryHint[0] = pGFF->ReadFieldBYTE(&cStruct, "STORYHINT0", bSuccess, nDefault);
    nDefault = m_anStoryHint[1];
    m_anStoryHint[1] = pGFF->ReadFieldBYTE(&cStruct, "STORYHINT1", bSuccess, nDefault);
    nDefault = m_anStoryHint[2];
    m_anStoryHint[2] = pGFF->ReadFieldBYTE(&cStruct, "STORYHINT2", bSuccess, nDefault);
    nDefault = m_anStoryHint[3];
    m_anStoryHint[3] = pGFF->ReadFieldBYTE(&cStruct, "STORYHINT3", bSuccess, nDefault);
    nDefault = m_anStoryHint[4];
    m_anStoryHint[4] = pGFF->ReadFieldBYTE(&cStruct, "STORYHINT4", bSuccess, nDefault);
    nDefault = m_anStoryHint[5];
    m_anStoryHint[5] = pGFF->ReadFieldBYTE(&cStruct, "STORYHINT5", bSuccess, nDefault);
    nDefault = m_anStoryHint[6];
    m_anStoryHint[6] = pGFF->ReadFieldBYTE(&cStruct, "STORYHINT6", bSuccess, nDefault);
    nDefault = m_anStoryHint[7];
    m_anStoryHint[7] = pGFF->ReadFieldBYTE(&cStruct, "STORYHINT7", bSuccess, nDefault);
    nDefault = m_anStoryHint[8];
    m_anStoryHint[8] = pGFF->ReadFieldBYTE(&cStruct, "STORYHINT8", bSuccess, nDefault);
    nDefault = m_anStoryHint[9];
    m_anStoryHint[9] = pGFF->ReadFieldBYTE(&cStruct, "STORYHINT9", bSuccess, nDefault);
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
#endif
// PC 0x0070B130 GOG 0x0070AE70
CSWGuiSaveLoadEntry::CSWGuiSaveLoadEntry()
{
    SwguisaveloadTrace2();
    m_nGameplayHint = 0;
#ifndef _XBOX
    memset(m_anStoryHint, 0, sizeof(m_anStoryHint));
#else
    m_nStoryHint = 0;
#endif
    m_bDataLoaded = 0;
    m_bBadSave = 0;
    m_bNewSlot = 0;
    m_bRebootAutoSave = 0;
#ifdef _XBOX
    m_bEmptySlot = 0;
#else
    m_bPCAutoSave = 0;
    m_bEmptySlot = 0;
    m_bCheatUsed = 0;
#endif
    m_nSaveGame = SAVE_SLOT_QUICK;
    m_nTimePlayed = 0;
    m_nSaveSpaceNeeded = 0;
#ifndef _XBOX
    m_sSaveGameName = "Bad";
    m_nSaveNumber = 0;
    m_nTimeStamp = 0;
#endif
}
// PC 0x00708260 GOG 0x00707E50
CExoString CSWGuiSaveLoadEntry::GetGameDirectory()
{
    CExoString sDirectory;

    sDirectory.Format("SAVES:%06d - %s", m_nSaveGame, m_sGameName);
    return sDirectory;
}
void CSWGuiSaveLoadEntry::SetXboxTitle(const CExoString& sTitle)  // PC 0x0070A070 GOG 0x00709DB0
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
    else if (m_bBadSave)
    {
        m_sXboxTitle = sTitle;
    }
    else if (m_nSaveGame == SAVE_SLOT_QUICK)
    {
        m_sXboxTitle = g_pTlkTable->GetSimpleString(STRREF_QUICK_SAVE);
    }

    CExoString sFormattedTitle;

    if (m_nSaveGame == SAVE_SLOT_QUICK || m_nSaveGame == SAVE_SLOT_AUTO)
    {
        m_sXboxTitle = m_sXboxTitle + "\n" + FormatTimeToString();
    }
    else
    {
        CExoString sTimeStamp = FormatTimeToString();
        g_pTlkTable->SetCustomToken(TITLE_TOKEN_NUMBER, m_nSaveNumber);
        g_pTlkTable->SetCustomToken(TITLE_TOKEN_NAME, m_sSaveGameName);
        g_pTlkTable->SetCustomToken(TITLE_TOKEN_TIME_STAMP, sTimeStamp);
        sFormattedTitle = g_pTlkTable->GetSimpleString(STRREF_SAVE_TITLE);
        g_pTlkTable->ParseStr(sFormattedTitle);
        m_sXboxTitle = sFormattedTitle;
    }
#endif
    m_cText.m_cParams.SetText(m_sXboxTitle);
}
#ifndef _XBOX
void CSWGuiSaveLoad::LoadGame(CSWGuiControl* pControl)  // PC 0x0070BDB0 GOG 0x0070BB00
{
    if (m_bExitToMainMenu ||
        g_pAppManager->GetClientExoApp()->GetInGameGui()->GetMessageBox()->m_bAccepted ||
        m_bEndGameMode)
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
                for (int nArea = 0; nArea < CSWGuiSaveLoadEntry::STORY_HINT_AREAS; nArea++)
                {
                    unsigned char nStoryHint = pEntry->GetStoryHint(nArea);
                    pClientExoApp->SetStoryHint(nStoryHint, nArea);
                }
                pClientExoApp->SetLoadScreenHint(
                    pClientExoApp->GetNextLoadScreenHintSTRREF(
                        pEntry->m_sLastModule, CClientExoAppInternal::LOAD_SCREEN_HINT_STORY));
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
                pClient->SetGamePlayHint(pEntry->m_nGameplayHint);
                for (int nArea = 0; nArea < CSWGuiSaveLoadEntry::STORY_HINT_AREAS; nArea++)
                {
                    pClient->SetStoryHint(pEntry->GetStoryHint(nArea), nArea);
                }
                pClient->SetLoadScreenHint(pClient->GetNextLoadScreenHintSTRREF(
                    pEntry->m_sLastModule, CClientExoAppInternal::LOAD_SCREEN_HINT_STORY));
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
#endif
void CSWGuiSaveLoad::PromptForSaveName(CSWGuiControl* pControl)  // PC 0x0070AFB0 GOG 0x0070ACF0
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
void CSWGuiSaveNamePanel::HandleOkButton(CSWGuiControl* pControl)  // PC 0x00708480 GOG 0x00708070
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
#ifndef _XBOX
// PC 0x00709BC0 GOG 0x00709900
CSWGuiSaveLoadEntry::CSWGuiSaveLoadEntry(CSWGuiSaveLoadEntry* pEntry)
{
    CSWGuiExtent cExtent;
    cExtent.m_nHeight = SAVE_ENTRY_HEIGHT;
    Initialize(cExtent, pEntry);
    m_nGameplayHint = pEntry->GetGameplayHint();
    for (int nArea = 0; nArea < STORY_HINT_AREAS; nArea++)
        m_anStoryHint[nArea] = pEntry->GetStoryHint(nArea);
    m_bDataLoaded = pEntry->m_bDataLoaded;
    m_bRebootAutoSave = pEntry->m_bRebootAutoSave;
    m_bPCAutoSave = pEntry->m_bPCAutoSave;
    m_nSaveGame = pEntry->GetSaveGame();
    m_nTimePlayed = pEntry->GetTimePlayed();
    m_bNewSlot = pEntry->m_bNewSlot;
    m_nSaveSpaceNeeded = pEntry->GetSaveSpaceNeeded();
    m_bBadSave = pEntry->m_bBadSave;
    m_bEmptySlot = pEntry->m_bEmptySlot;
    m_bCheatUsed = pEntry->m_bCheatUsed;
    m_sSaveGameName = pEntry->m_sSaveGameName;
    m_bMissingLiveContent = pEntry->m_bMissingLiveContent;
    m_sGameName = pEntry->m_sGameName;
    m_sAreaName = pEntry->m_sAreaName;
    m_sLastModule = pEntry->m_sLastModule;
    m_sUnknown1F8 = pEntry->m_sUnknown1F8;
    m_sXboxTitle = pEntry->m_sXboxTitle;
    m_sScreenshot = pEntry->m_sScreenshot;
    m_aPortraits[0] = pEntry->GetPortrait(0);
    m_aPortraits[1] = pEntry->GetPortrait(1);
    m_aPortraits[2] = pEntry->GetPortrait(2);
    m_nTimeStamp = pEntry->GetTimeStamp();
    m_sPCName = pEntry->GetPCName();
    m_nSaveNumber = pEntry->GetSaveNumber();

    CSWGuiHandler pHandler = 0;
    CSWGuiObject* pObject = 0;
    pEntry->GetEvent(GUIEVENT_ACTIVATE, &pObject, &pHandler);
    if (pHandler != 0 && pObject != 0)
        AddEvent(GUIEVENT_ACTIVATE, pObject, pHandler);
    pEntry->GetEvent(GUIEVENT_DELETE, &pObject, &pHandler);
    if (pHandler != 0 && pObject != 0)
        AddEvent(GUIEVENT_DELETE, pObject, pHandler);
    pEntry->GetEvent(GUIEVENT_MOUSE_ENTER, &pObject, &pHandler);
    if (pHandler != 0 && pObject != 0)
        AddEvent(GUIEVENT_MOUSE_ENTER, pObject, pHandler);
    pEntry->GetEvent(GUIEVENT_RECOMMEND, &pObject, &pHandler);
    if (pHandler != 0 && pObject != 0)
        AddEvent(GUIEVENT_RECOMMEND, pObject, pHandler);
}
#endif
int CSWGuiSaveLoad::UnpackGame(unsigned long nSaveGame,  // PC 0x0070B380 GOG 0x0070B0C0
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
    else
    {
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
}
#ifndef _XBOX
void CSWGuiSaveLoad::HandleSaveButton(CSWGuiControl* pControl)  // PC 0x0070B710 GOG 0x0070B450
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
#endif
#ifndef _XBOX
int CSWGuiSaveLoad::CopyGameToFutureGame(unsigned long nSaveGame,  // PC 0x0070A2E0 GOG 0x0070A020
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
#endif
int LoadScreenUpdate(int nStep, int nSteps, int nStart, int nRange)  // PC 0x007074D0 GOG 0x007070C0
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
// PC 0x0070A5A0 GOG 0x0070A2E0
CSWGuiSaveNamePanel::CSWGuiSaveNamePanel(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    SwguisaveloadTrace1();
    m_pSaveLoad = NULL;
    StartLoadFromLayout("savename_p");
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
#ifndef _XBOX
// PC 0x0070A7E0 GOG 0x0070A520
void CSWGuiSaveLoad::SortGamesByTimeStamp(CExoArrayList<CSWGuiControl*>& lControls)
{
    for (int i = 0; i < lControls.num - 1; i++)
    {
        int nNewest = i;
        unsigned __int64 nNewestTime = ((CSWGuiSaveLoadEntry*)lControls[i])->m_nTimeStamp;
        CSWGuiControl* pControl = lControls[i];
        for (int j = i + 1; j < lControls.num; j++)
        {
            unsigned __int64 nTime = ((CSWGuiSaveLoadEntry*)lControls[j])->m_nTimeStamp;
            if (nTime > nNewestTime)
            {
                nNewest = j;
                nNewestTime = nTime;
            }
        }
        lControls[i] = lControls[nNewest];
        lControls[nNewest] = pControl;
    }
}
// PC 0x0070ABD0 GOG 0x0070A910
void CSWGuiSaveLoad::PopulateGameLB()
{
    CExoArrayList<CSWGuiControl*> lControls;
    m_lblTimePlayed.m_cText.m_cParams.SetText("");
    m_lbSaveGames.ClearItems();
    if (m_lFilterNames.num > 0)
    {
        m_lblPCName.m_cText.m_cParams.SetText(m_lFilterNames[m_nCurrentFilter]);
    }
    else
    {
        m_lblPCName.m_cText.m_cParams.SetText("");
    }
    m_lblPlanetName.m_cText.m_cParams.SetText("");
    m_lblAreaName.m_cText.m_cParams.SetText("");
    for (int nPortrait = 0; nPortrait < 3; ++nPortrait)
    {
        CResRef* pPortrait = &CResRef();
        m_aPartyMemberLabels[nPortrait].m_cBorder.m_cParams.SetFillImage(*pPortrait, 0);
    }
    m_lblScreenshot.m_cText.m_cParams.SetStrRef(STRREF_NEW_SLOT);
    CResRef* pScreenshot = &CResRef();
    m_lblScreenshot.m_cBorder.m_cParams.SetFillImage(*pScreenshot, 0);
    for (int i = 0; i < m_lEntries.num; i++)
    {
        CSWGuiSaveLoadEntry* pEntry = new CSWGuiSaveLoadEntry(m_lEntries[i]);
        int bShow = 1;
        if (m_lFilterNames.num > 0)
        {
            bShow = pEntry->GetPCName() == m_lFilterNames[m_nCurrentFilter];
            if (!bShow)
            {
                if (pEntry->GetPCName().IsEmpty())
                {
                    bShow = 1;
                }
            }
        }
        if (bShow)
        {
            if (pEntry->m_bNewSlot)
            {
                lControls.Insert(pEntry, 0);
            }
            else
            {
                lControls.Add(pEntry);
            }
        }
    }
    if (lControls.num > 0)
    {
        m_lbSaveGames.AddControls(lControls, 1, 0, 0);
        m_lbSaveGames.SetSelectedControl(m_lbSaveGames.GetControl(0), 0);
        SetActiveControl(&m_lbSaveGames, 0);
    }
    else if (!m_bSaveMode)
    {
        m_lblScreenshot.m_cText.m_cParams.SetStrRef(STRREF_NO_SAVES);
    }
}
void CSWGuiSaveLoad::PopulateGameList()  // PC 0x0070C600 GOG 0x0070C350
{
    CExoArrayList<CExoString> lDirectories;
    CExoArrayList<CSWGuiControl*> lControls;
    CSWGuiExtent cExtent;
    cExtent.m_nHeight = CSWGuiSaveLoadEntry::SAVE_ENTRY_HEIGHT;
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
            CSWGuiSaveLoadEntry* pEntry = (CSWGuiSaveLoadEntry*)lControls[i];
            unsigned long nSlot = pEntry->m_nSaveGame;
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
    SortGamesByTimeStamp(lControls);
    if (!m_bFiltersLoaded)
    {
        CExoString sPCName;
        for (int i = 0; i < lControls.num; i++)
        {
            sPCName = ((CSWGuiSaveLoadEntry*)lControls[i])->GetPCName();
            if (!sPCName.IsEmpty())
            {
                m_lFilterNames.AddUnique(sPCName);
            }
        }
    }
    if (!m_bFiltersLoaded)
    {
        m_nCurrentFilter = 0;
    }
    if (!m_bFiltersLoaded)
    {
        if (!m_bExitToMainMenu && g_pAppManager->m_pServerExoApp->GetPartyTable() != 0)
        {
            CExoString sPCName = g_pAppManager->GetServerExoApp()->GetPartyTable()->GetPCName();
            if (!m_lFilterNames.Contains(sPCName))
            {
                m_lFilterNames.Insert(sPCName, 0);
            }
            else
            {
                for (int i = 0; i < m_lFilterNames.num; i++)
                {
                    if (m_lFilterNames[i] == sPCName)
                    {
                        m_nCurrentFilter = i;
                        break;
                    }
                }
            }
        }
        m_bFiltersLoaded = 1;
    }
    m_lEntries.num = 0;
    for (int i = 0; i < lControls.num; i++)
    {
        m_lEntries.Add((CSWGuiSaveLoadEntry*)lControls[i]);
    }
    PopulateGameLB();
    m_nMaxSaveGame = nNextSlot;
}
#endif
