#include "guiingame.h"
#include "swguisaveload.h"
#include "swguimaininterfacepc2.h"
#include "swgui.h"
#include "swguidialog.h"
#include "exoresman.h"
#include "nwerffile.h"
#include "swguiingame.h"
#include "swguiingametutorialbox.h"
#include "swguiskillflowchart.h"
#include "swguiingameareatransition.h"
#include "swguifade.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "serverexoapp.h"
#include "nwcmodule.h"
#include "nwcarea.h"
#include "nwsarea.h"
#include "swguiCreateItem.h"
#include "swguilegalscreen.h"
#include "swguiSubCreateItem.h"
#include "swguiingameequip.h"
#include "guiingame.h"
#include "clientexoappinternal.h"
#include "exoinput.h"
#include "swguiDebugLoadModule.h"
#include "swguiPowersFeatsSkillsDebugMenu.h"
#include "exobase.h"
#include "worldtimer.h"
#include "clientoptions.h"
#include "exobase.h"
#include "swguiupgrade.h"
#include "swguistore.h"
#include "nwmessage.h"
#include "exobase.h"
#include "nwcobject.h"
#include "nwrules.h"
#include "twodimarrays.h"
#include "twodimarray.h"
#include "nwcanimbase.h"
#include "swguiingame.h"
#include "swguiingamecharacter.h"
#include "swguiingame.h"
#include "swguiingamemap.h"
#include "swguiingameoptions.h"
#include "nwtlktable.h"
#include "nwsplayer.h"
#include "nwcanimbase.h"
#include "nwscript.h"
#include "nwccreature.h"
#include "aurcamera.h"
#include "partytables.h"
#include "swparty.h"
#include "nwsobject.h"
#include "swbehaviorcamera.h"
#include "swbehaviorcameradialog.h"
#include "swguipartyselection.h"
#include "swguiingamepause.h"
#include "swguiingamesolomodequery.h"
#include "swmain.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <float.h>

#include "nwcplaceable.h"
#include "nwsplaceable.h"
#ifdef _XBOX

#undef SWGUI_PANEL_MODE_OVERLAY
#define SWGUI_PANEL_MODE_OVERLAY 0
#endif

static inline void GuiingameTrace2();
static inline void GuiingameTrace1();
#define GUIINGAME_HOOK() GuiingameTrace2()

static inline CServerExoApp* ServerExoAppLate3(CAppManager* p);
static inline CServerExoApp* ServerExoAppLate2(CAppManager* p);
static inline CServerExoApp* ServerExoAppLate1(CAppManager* p);

static inline CClientExoApp* ClientExoAppLate2(CAppManager* p);
static inline CClientExoApp* ClientExoAppLate1(CAppManager* p);

static inline void GuiInGameTrace();
#define GUIINGAME_TRACE() GuiInGameTrace()

CSWGuiPanel* g_pDynamicGUI = 0;
int trapbadstrref = 0;
#ifdef _XBOX
int g_bCreateAllGUIs = 0;
#else
int g_bCreateAllGUIs = 1;
#endif
int g_bModifiedSound = 1;
int bDisableLetterbox = 0;

Vector CGuiInGame::ENABLED_ICON_COLOR(1.0f, 1.0f, 1.0f);
Vector CGuiInGame::DISABLED_ICON_COLOR(0.35f, 0.35f, 0.35f);
Vector CGuiInGame::STANDARD_BLUE(0.0f, 0.64f, 0.96f);
Vector CGuiInGame::COLOR_TRANSPARENT(-1.0f, -1.0f, -1.0f);
Vector CGuiInGame::COLOR_WHITE(1.0f, 1.0f, 1.0f);
Vector CGuiInGame::COLOR_BLUE(0.0f, 0.66f, 0.98f);
Vector CGuiInGame::COLOR_YELLOW(0.98f, 1.0f, 0.0f);
Vector CGuiInGame::COLOR_CYAN(0.4f, 1.0f, 1.0f);
Vector CGuiInGame::COLOR_RED(0.74f, 0.11f, 0.0f);
Vector CGuiInGame::COLOR_GREEN(0.28f, 0.92f, 0.11f);
Vector CGuiInGame::COLOR_ORANGE(0.98f, 0.45f, 0.0f);
Vector CGuiInGame::COLOR_PURPLE(0.95f, 0.0f, 0.85f);
Vector CGuiInGame::DISABLED_TEXT(0.4f, 0.4f, 0.3f);
Vector CGuiInGame::COLOR_NEW_GREEN(0.05f, 0.35f, 0.27f);
Vector CGuiInGame::COLOR_NEW_HIGHLIGHT(0.7f, 0.7f, 0.6f);
Vector CGuiInGame::COLOR_NEW_TEXT(0.1f, 0.7f, 0.55f);
Vector CGuiInGame::COLOR_NEW_BONUS(0.95f, 0.66f, 0.3f);
Vector CGuiInGame::COLOR_LIGHT_GOLD(0.64f, 0.53f, 0.18f);
Vector CGuiInGame::COLOR_DARK_GOLD(0.31f, 0.25f, 0.09f);
Vector CGuiInGame::COLOR_DARK_TAN(0.4f, 0.4f, 0.3f);
Vector CGuiInGame::COLOR_PAZAAK_GOLD(0.8f, 0.65f, 0.2f);
Vector CGuiInGame::COLOR_UPGRADE_TEXT(0.0f, 0.66f, 0.98f);
Vector CGuiInGame::COLOR_UPGRADE_BORDER(0.0f, 0.33f, 0.5f);
Vector CGuiInGame::COLOR_BLACK;

extern int bDisableLetterbox;

const char* HideGUI(char* sCommand)
{
    if (g_pDynamicGUI != NULL)
    {
        g_pAppManager->GetClientExoApp()->GetSWGuiManager()->RemovePanel(
            g_pDynamicGUI);
#ifdef _XBOX
        g_pAppManager->GetClientExoApp()->SetInputClass(0);
#else
        g_pAppManager->GetClientExoApp()->SetInputClass(0, 1);
#endif
        if (g_pDynamicGUI != NULL)
        {
            delete g_pDynamicGUI;
        }
        g_pDynamicGUI = NULL;
    }
    return "";
}
#ifndef _XBOX
const char* ShowGUI(char* sCommand)
{
    if (*sCommand == 0)
    {
        return "need to specify the GUI to create";
    }
    if (g_pDynamicGUI != NULL)
    {
        g_pAppManager->GetClientExoApp()->GetSWGuiManager()->RemovePanel(
            g_pDynamicGUI);
#ifdef _XBOX
        g_pAppManager->GetClientExoApp()->SetInputClass(0);
#else
        g_pAppManager->GetClientExoApp()->SetInputClass(0, 1);
#endif
        if (g_pDynamicGUI != NULL)
        {
            delete g_pDynamicGUI;
        }
        g_pDynamicGUI = NULL;
    }
    g_pDynamicGUI = new CSWGuiDynamic(
        g_pAppManager->GetClientExoApp()->GetSWGuiManager(), CResRef(sCommand));
    g_pAppManager->GetClientExoApp()->GetSWGuiManager()->AddPanel(
        g_pDynamicGUI, 0, 1);
#ifdef _XBOX
    g_pAppManager->GetClientExoApp()->SetInputClass(2);
#else
    g_pAppManager->GetClientExoApp()->SetInputClass(2, 1);
#endif
    return "";
}
#endif
// PC 0x00685280
CGuiInGame::CGuiInGame()
{
    g_pAppManager->GetClientExoApp()->GetClientOptions()
        ->SetCameraMode(CClientOptions::CAMERA_MODE_ON_A_STICK);
    m_pMessageBuffer = new CGuiInGameMessage[MESSAGE_BUFFER_CAPACITY];
    m_pDialogBuffer = new CGuiInGameDialogMessage[MESSAGE_BUFFER_CAPACITY];
    m_nAbilityMode = 0;
    m_bJournalDone = 0;
    m_nInventoryFilter = 0;
    m_nMessageType = MESSAGE_TYPE_DIALOG;
    m_bDialogEntryShown = 0;
    m_pDialog = 0;
    m_MessageBufferSize = 0;
    m_nDialogBufferSize = 0;
    m_bItemCreateMenuAvailable = 0;
    m_bReplyPending = 0;
    m_LevelUpMode = 0;
    m_nLastGuiPanel = SCREEN_EQUIP;
    m_bShowPauseGui = 0;
    m_pPartySelection = 0;
    m_pDialogTop = 0;
    m_pDialogLetterbox = 0;
    m_pDialogFade = 0;
    m_pGlobalFade = 0;
    m_pGalaxyMap = 0;
    m_pBarkBubble = 0;
    m_pExamineBox = 0;
    m_pContainer = 0;
    m_pItemCreateMenu = 0;
    m_pLoadModuleDebugMenu = 0;
    m_pPowersFeatsSkillsDebugMenu = 0;
    m_pSubItemCreateMenu = 0;
    m_pCameraDialog = 0;
    m_pComputerDialog = 0;
    m_pComputerScreen = 0;
    m_pPartySelectionPanel = 0;
    m_pStorePanel = 0;
    m_pUpgradeScreen = 0;
    m_pMainInterface = 0;
    m_pAreaTransition = 0;
    m_pMessageBox = 0;
    m_pSkillInfoBox = 0;
    m_pControllerLossBox = 0;
    m_pStatusSummary = 0;
    m_pPendingStatusSummary = 0;
    m_pInGameMenu = 0;
    for (int nScreen = 0; nScreen < IN_GAME_SCREEN_COUNT; nScreen++)
        m_apScreenPanels[nScreen] = 0;
    m_asReplyText = 0;
    m_pnReplyValue1 = 0;
    m_pnReplyValue2 = 0;
    m_pnReplyValue3 = 0;
    m_pnReplyValue4 = 0;
    m_pfReplyValue5 = 0;
    m_pfReplyValue6 = 0;
    m_pnReplyValue7 = 0;
    m_pnReplyValue8 = 0;
    m_pfReplyValue9 = 0;
    m_pnReplyValue10 = 0;
    m_pnReplyValue11 = 0;
    m_pvReplyValue12 = 0;
    m_pfReplyValue13 = 0;
    m_pfReplyValue14 = 0;
    m_pnReplyAnimations = 0;
    m_ppnReplyAnimationIDs = 0;
    m_ppnReplyAnimationFlags = 0;
    m_nReplies = 0;
    m_nDialogCameraState = 0;
    m_bShotCameraInitialized = 0;
    m_bAnimatedCameraActive = 0;
    m_fAnimatedCameraTime = 0;
    m_bAnimatedCameraLoaded = 0;
    m_bDialogCameraAnimationActive = 0;
    m_oidComputerCameraTarget = 0;
    m_nDialogAnimations = 0;
    m_nLastDialogAnimations = 0;
    m_nDialogCameraAnimation = CREATURE_ANIMATION_NO_IDLE_FALLBACK;
    m_nStuntModels = 0;
    m_pnStuntModelIDs = 0;
    m_pStuntModelResRefs = 0;
    m_pDialogAnimations = 0;
    m_pDialogAnimationFlags = 0;
    m_pLastDialogAnimations = 0;
    m_pLastDialogAnimationFlags = 0;
    m_pGuiManager = 0;
    m_oidDialogSoundObject = -1;
    m_oidDialogCamera = OBJECT_INVALID;
    m_oidDialogObject1 = OBJECT_INVALID;
    m_oidDialogObject2 = OBJECT_INVALID;
    m_oidPreviousDialogObject1 = OBJECT_INVALID;
    m_oidPreviousDialogObject2 = OBJECT_INVALID;
    m_bPendingDialogBark = 0;
    m_sPendingDialogBark = "";
    m_nGlobalDialogState = 0;
    m_nC0 = 0;
    m_nDialogCameraType = 0;
#ifdef _XBOX
    m_bInitialized = 1;
    CExoIni cIni;
    CExoString sCompass;
    if (cIni.ReadIniEntry(sCompass, "d:\\swkotor.ini", "Display Options", "Compass"))
        m_bInitialized = sCompass.AsINT() != 0;
#endif
    m_nPlaceableCameras = 0;
    m_bComputerCameraShot = 0;
    m_fComputerCameraMicRange = 0.0f;
    m_pDialogSound = 0;
    m_bDialogSoundQueried = 0;
    m_nDialogReply = 0;
#ifndef _XBOX
    m_bInitialized = 1;
#endif
    m_nDialogReplyCount = 0;
    m_bDialogReplySelected = 0;
    m_nDialogReplySelection = 0;
    m_bPaused = 0;
#ifndef _XBOX
    m_nPauseState = 0;
#endif
    m_nDialogSpanningTextRemaining = 0;
    m_nDialogSpanningTextCount = 0;
    m_bHoldWorldFadeInForDialog = 0;
    m_oidPostDialogCharacterSwitch = OBJECT_INVALID;
#ifdef _XBOX
    m_nXboxUnmappedB98 = 0;
    m_nDialogReplyRow = 16;
#endif
    for (int nTutorialByte = 0; nTutorialByte < sizeof(m_aTutorialWindowsShown); nTutorialByte++)
        m_aTutorialWindowsShown[nTutorialByte] = 0;
    m_nGlobalUnpauseableState = 1;
    m_bControllerPauseActive = 0;
    m_bPausedByCombatBeforeControllerPause = 0;
#ifdef _XBOX
    m_bCreditSequenceInProgress = 0;
#endif
    m_nDialogScrollWait = 0;
    m_bDialogScrollFinished = 0;
    m_pDialogAmbientTrack = 0;
#ifdef _XBOX
    m_pCreditsMusic = 0;
#endif
    m_fDialogCameraPullBackRange = 0.0f;
    m_nNoClickDay = 0;
    m_nNoClickTime = 0;
    m_nNoClickDuration = 0;
    m_bUsePartyCharacterForDialog = 0;
    m_nDialogSoundId = OBJECT_INVALID;
    m_cDialogSound = "";
    for (int nParticipant = 0; nParticipant < 3; nParticipant++)
        m_aDialogParticipants[nParticipant] = OBJECT_INVALID;
    m_oidCameraAnimationObject = OBJECT_INVALID;
    m_bDeferredFOVCall = 0;
    m_fDeferredFieldOfView = 0.0f;
    m_bTransitionScriptRunning = 0;
#ifdef _XBOX
    m_nXboxUnmappedC0C = 1;
#endif
    m_bInputActiveBeforeControllerPause = 0;
#ifndef _XBOX
    m_vAreaTransition = Vector(0.0f, 0.0f, 0.0f);
    m_oidAreaTransition = OBJECT_INVALID;
    m_bCameraRelativeAreaTransition = 1;
    m_bDanceDanceMalak = 0;
#endif
}
// PC 0x006858F0
CGuiInGame::~CGuiInGame()
{
    ShutDown();
    ShutDownGlobal();
    delete [] m_pDialogBuffer;
    delete [] m_pMessageBuffer;
    while (m_lDialogNoOrientList.GetHeadPos() != 0)
        delete m_lDialogNoOrientList.RemoveHead();
    if (m_pDialogAmbientTrack != 0)
    {
        delete m_pDialogAmbientTrack;
        m_pDialogAmbientTrack = 0;
    }
    if (m_pDialogSound != 0)
    {
        m_pDialogSound->Stop();
        delete m_pDialogSound;
        m_pDialogSound = 0;
    }
}

void CGuiInGame::ClearLastDialogAnimationData()
{
    m_nLastDialogAnimations = 0;
    if (m_pLastDialogAnimations != 0)
    {
        delete [] m_pLastDialogAnimations;
        m_pLastDialogAnimations = 0;
    }
    if (m_pLastDialogAnimationFlags != 0)
    {
        delete [] m_pLastDialogAnimationFlags;
        m_pLastDialogAnimationFlags = 0;
    }
}
// PC 0x006878E0
int CGuiInGame::CloseDialog()
{
    if (m_bItemCreateMenuAvailable != 0)
    {
#ifdef _XBOX
        ClearDialogReplyText();
#endif
        g_pAppManager->GetClientExoApp()->DisableVideoEffect();
        ClearDialogNonDependantList();
        m_bUsePartyCharacterForDialog = 0;
        if (m_oidPostDialogCharacterSwitch != OBJECT_INVALID)
        {
            if (m_pGuiManager->PanelExists(m_pPartySelectionPanel) == 0)
            {
                for (int nCharacter = 0;
                     nCharacter < g_pAppManager->GetClientExoApp()->GetSWParty()->m_nCharacters;
                     ++nCharacter)
                {
                    CSWCCreature* pCharacter =
                        g_pAppManager->GetClientExoApp()->GetSWParty()->GetCharacter(0);
                    if (pCharacter != 0)
                    {
                        unsigned long oidCharacter = pCharacter->m_nId;
                        if (g_pAppManager->GetServerExoApp()->ClientToServerObjectId(oidCharacter)
                            == m_oidPostDialogCharacterSwitch)
                            break;
                    }
                    g_pAppManager->GetClientExoApp()->ChangeCharacter(CSWParty::LEADER_NEXT, 0);
                }
            }
            m_oidPostDialogCharacterSwitch = OBJECT_INVALID;
        }

        unsigned long oidObject = m_oidDialogObject1;
        CSWCObject* pObject = g_pAppManager->GetClientExoApp()->GetGameObject(oidObject);
        if (pObject != 0 && pObject->AsSWCCreature() != 0)
            pObject->AsSWCCreature()->LookAt(OBJECT_INVALID, 0.0f);
        oidObject = m_oidDialogObject2;
        pObject = g_pAppManager->GetClientExoApp()->GetGameObject(oidObject);
        if (pObject != 0 && pObject->AsSWCCreature() != 0)
            pObject->AsSWCCreature()->LookAt(OBJECT_INVALID, 0.0f);

        m_oidPreviousDialogObject1 = m_oidDialogObject1;
        m_oidDialogObject1 = OBJECT_INVALID;
        m_oidPreviousDialogObject2 = m_oidDialogObject2;
        m_oidDialogObject2 = OBJECT_INVALID;
        ResetDialogAnimations();
        ResetTalkAnimations();
        m_bDialogEntryShown = 0;
        SetGlobalDialogState(0);
        if (m_pDialog == m_pComputerDialog)
        {
            if (g_bCreateAllGUIs == 0)
            {
                CSWGuiDialog* pDialog = m_pDialog;
                pDialog->m_bDeferredRemoval = 1;
                pDialog->m_nExitState = CSWGuiPanel::EXITSTATE_DELETE;
                m_pComputerDialog = 0;
            }
            else
            {
                m_pDialog->SetModal(1);
                m_pComputerDialog = 0;
            }
        }
        else
            m_pGuiManager->RemovePanel(m_pDialog);
        m_pGuiManager->RemovePanel(m_pComputerScreen);
        m_bDialogScrollFinished = 0;
        if (m_pDialog == m_pCameraDialog)
        {
            if (bDisableLetterbox == 0)
            {
                m_pGuiManager->RemovePanel(m_pDialogTop);
                m_pGuiManager->RemovePanel(m_pDialogLetterbox);
                m_pGuiManager->RemovePanel(m_pDialogFade);
            }
            CSWGuiDialogLetterbox* pLetterbox = m_pDialogTop;
            pLetterbox->m_bDone = 0;
            pLetterbox->ResetFade();
            pLetterbox = m_pDialogLetterbox;
            pLetterbox->m_bDone = 0;
            pLetterbox->ResetFade();
            pLetterbox = m_pDialogFade;
            pLetterbox->m_bDone = 0;
            pLetterbox->ResetFade();
        }
        m_pDialog->EndDialog();
        m_pDialog = m_pCameraDialog;
        EndDialogCamera();
        UnloadStuntModelsForDialog();
        ClearRepliesData();
        ClearDialogAnimationData();
        ClearLastDialogAnimationData();
        m_nDialogSpanningTextRemaining = 0;
        m_nDialogSpanningTextCount = 0;
        if (g_pAppManager->GetClientExoApp()->GetInputClass() == INPUT_CLASS_DIALOG)
        {
#ifdef _XBOX
            g_pAppManager->GetClientExoApp()->SetInputClass(INPUT_CLASS_GAME);
#else
            g_pAppManager->GetClientExoApp()->SetInputClass(INPUT_CLASS_GAME, 1);
#endif
            g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_GENERIC);
        }
        g_pAppManager->GetClientExoApp()->ResetDriveAcceleration();
#ifdef _XBOX
        g_pExoInput->CoolDownEvent(GUIEVENT_ACTIVATE, 1000);
#endif
        if (m_bPendingDialogBark != 0)
        {
            CResRef cSound;
            if (m_bItemCreateMenuAvailable != 0)
            {
                m_pBarkBubble->SetBark(OBJECT_INVALID, m_sPendingDialogBark, cSound, 0);
                m_pGuiManager->AddPanel(m_pBarkBubble, SWGUI_PANEL_MODE_OVERLAY, 1);
            }
            m_bPendingDialogBark = 0;
            m_sPendingDialogBark = "";
        }
#ifndef _XBOX
        g_pAppManager->GetClientExoApp()->SetLastClickedOnTarget(OBJECT_INVALID);
#endif
        return m_bDialogEntryShown;
    }
    return 0;
}

void CGuiInGame::LoadCameraModelForDialog(const CResRef& cCameraModel)
{
    if (cCameraModel.IsValid() && !m_bAnimatedCameraLoaded)
    {
        CSWCModule* pModule = g_pAppManager->m_pClientExoApp->GetModule();
        if (pModule != 0)
        {
            pModule->LoadAnimatedCamera(cCameraModel);
            m_bAnimatedCameraLoaded = 1;
        }
    }
}
// PC 0x0067CCF0
int CGuiInGame::SetLevelUpMode(int bMode)
{
    if (m_LevelUpMode != bMode)
    {
        m_LevelUpMode = bMode;
    }
    return bMode;
}
// PC 0x004015A0
void CGuiInGame::ClearMapDisplays()
{
}
// PC 0x004015A0
void CGuiInGame::ShowDebugMenu()
{
}
// PC 0x004015A0
void CGuiInGame::DestroyDeathMenuGUI()
{
}
// PC 0x0067CD10
void CGuiInGame::SetConversationType(unsigned long nType,
                                     unsigned char nComputerType)
{
    if (nType == 1)
    {
        if (m_pComputerDialog == 0)
        {
            m_pComputerDialog = new CSWGuiDialogComputer(m_pGuiManager);
        }
        m_pDialog = m_pComputerDialog;
        m_pComputerDialog->SetType(nComputerType);
    }
    else
    {
        m_pDialog = m_pCameraDialog;
    }
}

void CGuiInGame::ClearDialogAnimationData()
{
    m_nDialogAnimations = 0;
    if (m_pDialogAnimations != 0)
    {
        delete [] m_pDialogAnimations;
        m_pDialogAnimations = 0;
    }
    if (m_pDialogAnimationFlags != 0)
    {
        delete [] m_pDialogAnimationFlags;
        m_pDialogAnimationFlags = 0;
    }
}
// PC 0x0067CEC0
void CGuiInGame::SetDialogAnimationData(unsigned long nIndex, unsigned long nAnimation,
                                        unsigned short nFlags)
{
    if (nIndex < m_nDialogAnimations)
    {
        if (m_pDialogAnimations != 0 && m_pDialogAnimationFlags != 0)
        {
            m_pDialogAnimations[nIndex] = nAnimation;
            m_pDialogAnimationFlags[nIndex] = nFlags;
        }
    }
}
// PC 0x0067CF00
void CGuiInGame::SetReplyData(
    unsigned long nReply, const CExoString& sText, unsigned long nValue1,
    unsigned long nValue2, unsigned long nValue3, int nValue4, float fValue5,
    float fValue6, unsigned long nValue7, unsigned short nValue8, float fValue9,
    int nValue10, unsigned char nValue11, const Vector& vValue12,
    float fValue13, float fValue14, unsigned long nAnimations,
    unsigned long* pnAnimationIDs, unsigned short* pnAnimationFlags)
{
    if (nReply < m_nReplies)
    {
        m_asReplyText[nReply] = sText;
        m_pnReplyValue1[nReply] = nValue1;
        m_pnReplyValue2[nReply] = nValue2;
        m_pnReplyValue3[nReply] = nValue3;
        m_pnReplyValue4[nReply] = nValue4;
        m_pfReplyValue5[nReply] = fValue5;
        m_pfReplyValue6[nReply] = fValue6;
        m_pnReplyValue7[nReply] = nValue7;
        m_pnReplyValue8[nReply] = nValue8;
        m_pfReplyValue9[nReply] = fValue9;
        m_pnReplyValue10[nReply] = nValue10;
        m_pnReplyValue11[nReply] = nValue11;
        m_pvReplyValue12[nReply] = vValue12;
        m_pfReplyValue13[nReply] = fValue13;
        m_pfReplyValue14[nReply] = fValue14;
        m_pnReplyAnimations[nReply] = nAnimations;
        if (nAnimations != 0)
        {
            m_ppnReplyAnimationIDs[nReply] = new unsigned long[nAnimations];
            m_ppnReplyAnimationFlags[nReply] = new unsigned short[nAnimations];
        }
        else
        {
            m_ppnReplyAnimationIDs[nReply] = 0;
            m_ppnReplyAnimationFlags[nReply] = 0;
        }
        for (int i = 0; i < (int)nAnimations; i++)
        {
            m_ppnReplyAnimationIDs[nReply][i] = pnAnimationIDs[i];
            m_ppnReplyAnimationFlags[nReply][i] = pnAnimationFlags[i];
        }
    }
}
// PC 0x0067D0A0
CExoString CGuiInGame::GetCameraAnimationName(unsigned short nAnimation)
{
    CExoString sName("none");

    if (nAnimation == CREATURE_ANIMATION_NO_IDLE_FALLBACK)
        return sName;

    switch (nAnimation)
    {
    case CUTSCENE_ANIMATION_FIRST:
        sName = "cut001";
        break;
    case CUTSCENE_ANIMATION_FIRST + 1:
        sName = "cut002";
        break;
    case CUTSCENE_ANIMATION_FIRST + 2:
        sName = "cut003";
        break;
    case CUTSCENE_ANIMATION_FIRST + 3:
        sName = "cut004";
        break;
    case CUTSCENE_ANIMATION_FIRST + 4:
        sName = "cut005";
        break;
    case CUTSCENE_ANIMATION_FIRST + 5:
        sName = "cut006";
        break;
    case CUTSCENE_ANIMATION_FIRST + 6:
        sName = "cut007";
        break;
    case CUTSCENE_ANIMATION_FIRST + 7:
        sName = "cut008";
        break;
    case CUTSCENE_ANIMATION_FIRST + 8:
        sName = "cut009";
        break;
    case CUTSCENE_ANIMATION_FIRST + 9:
        sName = "cut010";
        break;
    case CUTSCENE_ANIMATION_FIRST + 10:
        sName = "cut011";
        break;
    case CUTSCENE_ANIMATION_FIRST + 11:
        sName = "cut012";
        break;
    case CUTSCENE_ANIMATION_FIRST + 12:
        sName = "cut013";
        break;
    case CUTSCENE_ANIMATION_FIRST + 13:
        sName = "cut014";
        break;
    case CUTSCENE_ANIMATION_FIRST + 14:
        sName = "cut015";
        break;
    case CUTSCENE_ANIMATION_FIRST + 15:
        sName = "cut016";
        break;
    case CUTSCENE_ANIMATION_FIRST + 16:
        sName = "cut017";
        break;
    case CUTSCENE_ANIMATION_FIRST + 17:
        sName = "cut018";
        break;
    case CUTSCENE_ANIMATION_FIRST + 18:
        sName = "cut019";
        break;
    case CUTSCENE_ANIMATION_FIRST + 19:
        sName = "cut020";
        break;
    case CUTSCENE_ANIMATION_FIRST + 20:
        sName = "cut021";
        break;
    case CUTSCENE_ANIMATION_FIRST + 21:
        sName = "cut022";
        break;
    case CUTSCENE_ANIMATION_FIRST + 22:
        sName = "cut023";
        break;
    case CUTSCENE_ANIMATION_FIRST + 23:
        sName = "cut024";
        break;
    case CUTSCENE_ANIMATION_FIRST + 24:
        sName = "cut025";
        break;
    case CUTSCENE_ANIMATION_FIRST + 25:
        sName = "cut026";
        break;
    case CUTSCENE_ANIMATION_FIRST + 26:
        sName = "cut027";
        break;
    case CUTSCENE_ANIMATION_FIRST + 27:
        sName = "cut028";
        break;
    case CUTSCENE_ANIMATION_FIRST + 28:
        sName = "cut039";
        break;
    case CUTSCENE_ANIMATION_FIRST + 29:
        sName = "cut030";
        break;
    case CUTSCENE_ANIMATION_FIRST + 30:
        sName = "cut031";
        break;
    case CUTSCENE_ANIMATION_FIRST + 31:
        sName = "cut032";
        break;
    case CUTSCENE_ANIMATION_FIRST + 32:
        sName = "cut033";
        break;
    case CUTSCENE_ANIMATION_FIRST + 33:
        sName = "cut034";
        break;
    case CUTSCENE_ANIMATION_FIRST + 34:
        sName = "cut035";
        break;
    case CUTSCENE_ANIMATION_FIRST + 35:
        sName = "cut036";
        break;
    case CUTSCENE_ANIMATION_FIRST + 36:
        sName = "cut037";
        break;
    case CUTSCENE_ANIMATION_FIRST + 37:
        sName = "cut038";
        break;
    case CUTSCENE_ANIMATION_FIRST + 38:
        sName = "cut039";
        break;
    case CUTSCENE_ANIMATION_FIRST + 39:
        sName = "cut040";
        break;
    case CUTSCENE_ANIMATION_FIRST + 40:
        sName = "cut041";
        break;
    case CUTSCENE_ANIMATION_FIRST + 41:
        sName = "cut042";
        break;
    case CUTSCENE_ANIMATION_FIRST + 42:
        sName = "cut043";
        break;
    case CUTSCENE_ANIMATION_FIRST + 43:
        sName = "cut044";
        break;
    case CUTSCENE_ANIMATION_FIRST + 44:
        sName = "cut045";
        break;
    case CUTSCENE_ANIMATION_FIRST + 45:
        sName = "cut046";
        break;
    case CUTSCENE_ANIMATION_FIRST + 46:
        sName = "cut047";
        break;
    case CUTSCENE_ANIMATION_FIRST + 47:
        sName = "cut048";
        break;
    case CUTSCENE_ANIMATION_FIRST + 48:
        sName = "cut049";
        break;
    case CUTSCENE_ANIMATION_FIRST + 49:
        sName = "cut050";
        break;
    case CUTSCENE_ANIMATION_FIRST + 50:
        sName = "cut051";
        break;
    case CUTSCENE_ANIMATION_FIRST + 51:
        sName = "cut052";
        break;
    case CUTSCENE_ANIMATION_FIRST + 52:
        sName = "cut053";
        break;
    case CUTSCENE_ANIMATION_FIRST + 53:
        sName = "cut054";
        break;
    case CUTSCENE_ANIMATION_FIRST + 54:
        sName = "cut055";
        break;
    case CUTSCENE_ANIMATION_FIRST + 55:
        sName = "cut056";
        break;
    case CUTSCENE_ANIMATION_FIRST + 56:
        sName = "cut057";
        break;
    case CUTSCENE_ANIMATION_FIRST + 57:
        sName = "cut058";
        break;
    case CUTSCENE_ANIMATION_FIRST + 58:
        sName = "cut059";
        break;
    case CUTSCENE_ANIMATION_FIRST + 59:
        sName = "cut060";
        break;
    case CUTSCENE_ANIMATION_FIRST + 60:
        sName = "cut061";
        break;
    case CUTSCENE_ANIMATION_FIRST + 61:
        sName = "cut062";
        break;
    case CUTSCENE_ANIMATION_FIRST + 62:
        sName = "cut063";
        break;
    case CUTSCENE_ANIMATION_FIRST + 63:
        sName = "cut064";
        break;
    case CUTSCENE_ANIMATION_FIRST + 64:
        sName = "cut065";
        break;
    case CUTSCENE_ANIMATION_FIRST + 65:
        sName = "cut066";
        break;
    case CUTSCENE_ANIMATION_FIRST + 66:
        sName = "cut067";
        break;
    case CUTSCENE_ANIMATION_FIRST + 67:
        sName = "cut068";
        break;
    case CUTSCENE_ANIMATION_FIRST + 68:
        sName = "cut069";
        break;
    case CUTSCENE_ANIMATION_FIRST + 69:
        sName = "cut070";
        break;
    case CUTSCENE_ANIMATION_FIRST + 70:
        sName = "cut071";
        break;
    case CUTSCENE_ANIMATION_FIRST + 71:
        sName = "cut072";
        break;
    case CUTSCENE_ANIMATION_FIRST + 72:
        sName = "cut073";
        break;
    case CUTSCENE_ANIMATION_FIRST + 73:
        sName = "cut074";
        break;
    case CUTSCENE_ANIMATION_FIRST + 74:
        sName = "cut075";
        break;
    case CUTSCENE_ANIMATION_FIRST + 75:
        sName = "cut076";
        break;
    case CUTSCENE_ANIMATION_FIRST + 76:
        sName = "cut077";
        break;
    case CUTSCENE_ANIMATION_FIRST + 77:
        sName = "cut078";
        break;
    case CUTSCENE_ANIMATION_FIRST + 78:
        sName = "cut079";
        break;
    case CUTSCENE_ANIMATION_FIRST + 79:
        sName = "cut080";
        break;
    case CUTSCENE_ANIMATION_FIRST + 80:
        sName = "cut081";
        break;
    case CUTSCENE_ANIMATION_FIRST + 81:
        sName = "cut082";
        break;
    case CUTSCENE_ANIMATION_FIRST + 82:
        sName = "cut083";
        break;
    case CUTSCENE_ANIMATION_FIRST + 83:
        sName = "cut084";
        break;
    case CUTSCENE_ANIMATION_FIRST + 84:
        sName = "cut085";
        break;
    case CUTSCENE_ANIMATION_FIRST + 85:
        sName = "cut086";
        break;
    case CUTSCENE_ANIMATION_FIRST + 86:
        sName = "cut087";
        break;
    case CUTSCENE_ANIMATION_FIRST + 87:
        sName = "cut088";
        break;
    case CUTSCENE_ANIMATION_FIRST + 88:
        sName = "cut089";
        break;
    case CUTSCENE_ANIMATION_FIRST + 89:
        sName = "cut090";
        break;
    case CUTSCENE_ANIMATION_FIRST + 90:
        sName = "cut091";
        break;
    case CUTSCENE_ANIMATION_FIRST + 91:
        sName = "cut092";
        break;
    case CUTSCENE_ANIMATION_FIRST + 92:
        sName = "cut093";
        break;
    case CUTSCENE_ANIMATION_FIRST + 93:
        sName = "cut094";
        break;
    case CUTSCENE_ANIMATION_FIRST + 94:
        sName = "cut095";
        break;
    case CUTSCENE_ANIMATION_FIRST + 95:
        sName = "cut096";
        break;
    case CUTSCENE_ANIMATION_FIRST + 96:
        sName = "cut097";
        break;
    case CUTSCENE_ANIMATION_FIRST + 97:
        sName = "cut098";
        break;
    case CUTSCENE_ANIMATION_FIRST + 98:
        sName = "cut099";
        break;
    case CUTSCENE_ANIMATION_FIRST + 99:
        sName = "cut100";
        break;
    case CUTSCENE_ANIMATION_FIRST + 100:
        sName = "cut101";
        break;
    case CUTSCENE_ANIMATION_FIRST + 101:
        sName = "cut102";
        break;
    case CUTSCENE_ANIMATION_FIRST + 102:
        sName = "cut103";
        break;
    case CUTSCENE_ANIMATION_FIRST + 103:
        sName = "cut104";
        break;
    case CUTSCENE_ANIMATION_FIRST + 104:
        sName = "cut105";
        break;
    case CUTSCENE_ANIMATION_FIRST + 105:
        sName = "cut106";
        break;
    case CUTSCENE_ANIMATION_FIRST + 106:
        sName = "cut107";
        break;
    case CUTSCENE_ANIMATION_FIRST + 107:
        sName = "cut108";
        break;
    case CUTSCENE_ANIMATION_FIRST + 108:
        sName = "cut109";
        break;
    case CUTSCENE_ANIMATION_FIRST + 109:
        sName = "cut110";
        break;
    case CUTSCENE_ANIMATION_FIRST + 110:
        sName = "cut111";
        break;
    case CUTSCENE_ANIMATION_FIRST + 111:
        sName = "cut112";
        break;
    case CUTSCENE_ANIMATION_FIRST + 112:
        sName = "cut113";
        break;
    case CUTSCENE_ANIMATION_FIRST + 113:
        sName = "cut114";
        break;
    case CUTSCENE_ANIMATION_FIRST + 114:
        sName = "cut115";
        break;
    case CUTSCENE_ANIMATION_FIRST + 115:
        sName = "cut116";
        break;
    case CUTSCENE_ANIMATION_FIRST + 116:
        sName = "cut117";
        break;
    case CUTSCENE_ANIMATION_FIRST + 117:
        sName = "cut118";
        break;
    case CUTSCENE_ANIMATION_FIRST + 118:
        sName = "cut119";
        break;
    case CUTSCENE_ANIMATION_FIRST + 119:
        sName = "cut120";
        break;
    case CUTSCENE_ANIMATION_FIRST + 120:
        sName = "cut121";
        break;
    case CUTSCENE_ANIMATION_FIRST + 121:
        sName = "cut122";
        break;
    case CUTSCENE_ANIMATION_FIRST + 122:
        sName = "cut123";
        break;
    case CUTSCENE_ANIMATION_FIRST + 123:
        sName = "cut124";
        break;
    case CUTSCENE_ANIMATION_FIRST + 124:
        sName = "cut125";
        break;
    case CUTSCENE_ANIMATION_FIRST + 125:
        sName = "cut126";
        break;
    case CUTSCENE_ANIMATION_FIRST + 126:
        sName = "cut127";
        break;
    case CUTSCENE_ANIMATION_FIRST + 127:
        sName = "cut128";
        break;
    case CUTSCENE_ANIMATION_W_FIRST:
        sName = "cut001w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 1:
        sName = "cut002w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 2:
        sName = "cut003w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 3:
        sName = "cut004w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 4:
        sName = "cut005w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 5:
        sName = "cut006w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 6:
        sName = "cut007w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 7:
        sName = "cut008w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 8:
        sName = "cut009w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 9:
        sName = "cut010w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 10:
        sName = "cut011w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 11:
        sName = "cut012w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 12:
        sName = "cut013w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 13:
        sName = "cut014w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 14:
        sName = "cut015w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 15:
        sName = "cut016w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 16:
        sName = "cut017w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 17:
        sName = "cut018w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 18:
        sName = "cut019w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 19:
        sName = "cut020w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 20:
        sName = "cut021w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 21:
        sName = "cut022w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 22:
        sName = "cut023w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 23:
        sName = "cut024w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 24:
        sName = "cut025w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 25:
        sName = "cut026w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 26:
        sName = "cut027w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 27:
        sName = "cut028w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 28:
        sName = "cut039w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 29:
        sName = "cut030w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 30:
        sName = "cut031w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 31:
        sName = "cut032w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 32:
        sName = "cut033w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 33:
        sName = "cut034w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 34:
        sName = "cut035w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 35:
        sName = "cut036w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 36:
        sName = "cut037w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 37:
        sName = "cut038w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 38:
        sName = "cut039w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 39:
        sName = "cut040w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 40:
        sName = "cut041w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 41:
        sName = "cut042w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 42:
        sName = "cut043w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 43:
        sName = "cut044w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 44:
        sName = "cut045w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 45:
        sName = "cut046w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 46:
        sName = "cut047w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 47:
        sName = "cut048w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 48:
        sName = "cut049w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 49:
        sName = "cut050w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 50:
        sName = "cut051w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 51:
        sName = "cut052w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 52:
        sName = "cut053w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 53:
        sName = "cut054w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 54:
        sName = "cut055w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 55:
        sName = "cut056w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 56:
        sName = "cut057w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 57:
        sName = "cut058w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 58:
        sName = "cut059w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 59:
        sName = "cut060w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 60:
        sName = "cut061w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 61:
        sName = "cut062w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 62:
        sName = "cut063w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 63:
        sName = "cut064w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 64:
        sName = "cut065w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 65:
        sName = "cut066w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 66:
        sName = "cut067w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 67:
        sName = "cut068w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 68:
        sName = "cut069w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 69:
        sName = "cut070w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 70:
        sName = "cut071w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 71:
        sName = "cut072w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 72:
        sName = "cut073w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 73:
        sName = "cut074w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 74:
        sName = "cut075w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 75:
        sName = "cut076w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 76:
        sName = "cut077w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 77:
        sName = "cut078w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 78:
        sName = "cut079w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 79:
        sName = "cut080w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 80:
        sName = "cut081w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 81:
        sName = "cut082w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 82:
        sName = "cut083w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 83:
        sName = "cut084w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 84:
        sName = "cut085w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 85:
        sName = "cut086w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 86:
        sName = "cut087w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 87:
        sName = "cut088w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 88:
        sName = "cut089w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 89:
        sName = "cut090w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 90:
        sName = "cut091w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 91:
        sName = "cut092w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 92:
        sName = "cut093w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 93:
        sName = "cut094w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 94:
        sName = "cut095w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 95:
        sName = "cut096w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 96:
        sName = "cut097w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 97:
        sName = "cut098w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 98:
        sName = "cut099w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 99:
        sName = "cut100w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 100:
        sName = "cut101w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 101:
        sName = "cut102w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 102:
        sName = "cut103w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 103:
        sName = "cut104w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 104:
        sName = "cut105w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 105:
        sName = "cut106w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 106:
        sName = "cut107w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 107:
        sName = "cut108w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 108:
        sName = "cut109w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 109:
        sName = "cut110w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 110:
        sName = "cut111w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 111:
        sName = "cut112w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 112:
        sName = "cut113w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 113:
        sName = "cut114w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 114:
        sName = "cut115w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 115:
        sName = "cut116w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 116:
        sName = "cut117w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 117:
        sName = "cut118w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 118:
        sName = "cut119w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 119:
        sName = "cut120w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 120:
        sName = "cut121w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 121:
        sName = "cut122w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 122:
        sName = "cut123w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 123:
        sName = "cut124w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 124:
        sName = "cut125w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 125:
        sName = "cut126w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 126:
        sName = "cut127w";
        break;
    case CUTSCENE_ANIMATION_W_FIRST + 127:
        sName = "cut128w";
        break;
    case CUTSCENE_ANIMATION_L_FIRST:
        sName = "cut001l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 1:
        sName = "cut002l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 2:
        sName = "cut003l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 3:
        sName = "cut004l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 4:
        sName = "cut005l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 5:
        sName = "cut006l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 6:
        sName = "cut007l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 7:
        sName = "cut008l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 8:
        sName = "cut009l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 9:
        sName = "cut010l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 10:
        sName = "cut011l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 11:
        sName = "cut012l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 12:
        sName = "cut013l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 13:
        sName = "cut014l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 14:
        sName = "cut015l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 15:
        sName = "cut016l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 16:
        sName = "cut017l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 17:
        sName = "cut018l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 18:
        sName = "cut019l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 19:
        sName = "cut020l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 20:
        sName = "cut021l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 21:
        sName = "cut022l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 22:
        sName = "cut023l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 23:
        sName = "cut024l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 24:
        sName = "cut025l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 25:
        sName = "cut026l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 26:
        sName = "cut027l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 27:
        sName = "cut028l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 28:
        sName = "cut039l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 29:
        sName = "cut030l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 30:
        sName = "cut031l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 31:
        sName = "cut032l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 32:
        sName = "cut033l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 33:
        sName = "cut034l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 34:
        sName = "cut035l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 35:
        sName = "cut036l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 36:
        sName = "cut037l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 37:
        sName = "cut038l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 38:
        sName = "cut039l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 39:
        sName = "cut040l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 40:
        sName = "cut041l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 41:
        sName = "cut042l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 42:
        sName = "cut043l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 43:
        sName = "cut044l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 44:
        sName = "cut045l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 45:
        sName = "cut046l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 46:
        sName = "cut047l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 47:
        sName = "cut048l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 48:
        sName = "cut049l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 49:
        sName = "cut050l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 50:
        sName = "cut051l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 51:
        sName = "cut052l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 52:
        sName = "cut053l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 53:
        sName = "cut054l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 54:
        sName = "cut055l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 55:
        sName = "cut056l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 56:
        sName = "cut057l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 57:
        sName = "cut058l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 58:
        sName = "cut059l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 59:
        sName = "cut060l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 60:
        sName = "cut061l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 61:
        sName = "cut062l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 62:
        sName = "cut063l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 63:
        sName = "cut064l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 64:
        sName = "cut065l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 65:
        sName = "cut066l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 66:
        sName = "cut067l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 67:
        sName = "cut068l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 68:
        sName = "cut069l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 69:
        sName = "cut070l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 70:
        sName = "cut071l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 71:
        sName = "cut072l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 72:
        sName = "cut073l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 73:
        sName = "cut074l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 74:
        sName = "cut075l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 75:
        sName = "cut076l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 76:
        sName = "cut077l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 77:
        sName = "cut078l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 78:
        sName = "cut079l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 79:
        sName = "cut080l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 80:
        sName = "cut081l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 81:
        sName = "cut082l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 82:
        sName = "cut083l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 83:
        sName = "cut084l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 84:
        sName = "cut085l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 85:
        sName = "cut086l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 86:
        sName = "cut087l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 87:
        sName = "cut088l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 88:
        sName = "cut089l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 89:
        sName = "cut090l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 90:
        sName = "cut091l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 91:
        sName = "cut092l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 92:
        sName = "cut093l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 93:
        sName = "cut094l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 94:
        sName = "cut095l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 95:
        sName = "cut096l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 96:
        sName = "cut097l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 97:
        sName = "cut098l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 98:
        sName = "cut099l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 99:
        sName = "cut100l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 100:
        sName = "cut101l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 101:
        sName = "cut102l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 102:
        sName = "cut103l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 103:
        sName = "cut104l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 104:
        sName = "cut105l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 105:
        sName = "cut106l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 106:
        sName = "cut107l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 107:
        sName = "cut108l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 108:
        sName = "cut109l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 109:
        sName = "cut110l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 110:
        sName = "cut111l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 111:
        sName = "cut112l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 112:
        sName = "cut113l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 113:
        sName = "cut114l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 114:
        sName = "cut115l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 115:
        sName = "cut116l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 116:
        sName = "cut117l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 117:
        sName = "cut118l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 118:
        sName = "cut119l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 119:
        sName = "cut120l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 120:
        sName = "cut121l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 121:
        sName = "cut122l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 122:
        sName = "cut123l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 123:
        sName = "cut124l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 124:
        sName = "cut125l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 125:
        sName = "cut126l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 126:
        sName = "cut127l";
        break;
    case CUTSCENE_ANIMATION_L_FIRST + 127:
        sName = "cut128l";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST:
        sName = "cut001wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 1:
        sName = "cut002wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 2:
        sName = "cut003wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 3:
        sName = "cut004wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 4:
        sName = "cut005wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 5:
        sName = "cut006wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 6:
        sName = "cut007wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 7:
        sName = "cut008wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 8:
        sName = "cut009wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 9:
        sName = "cut010wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 10:
        sName = "cut011wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 11:
        sName = "cut012wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 12:
        sName = "cut013wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 13:
        sName = "cut014wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 14:
        sName = "cut015wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 15:
        sName = "cut016wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 16:
        sName = "cut017wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 17:
        sName = "cut018wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 18:
        sName = "cut019wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 19:
        sName = "cut020wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 20:
        sName = "cut021wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 21:
        sName = "cut022wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 22:
        sName = "cut023wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 23:
        sName = "cut024wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 24:
        sName = "cut025wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 25:
        sName = "cut026wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 26:
        sName = "cut027wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 27:
        sName = "cut028wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 28:
        sName = "cut039wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 29:
        sName = "cut030wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 30:
        sName = "cut031wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 31:
        sName = "cut032wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 32:
        sName = "cut033wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 33:
        sName = "cut034wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 34:
        sName = "cut035wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 35:
        sName = "cut036wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 36:
        sName = "cut037wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 37:
        sName = "cut038wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 38:
        sName = "cut039wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 39:
        sName = "cut040wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 40:
        sName = "cut041wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 41:
        sName = "cut042wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 42:
        sName = "cut043wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 43:
        sName = "cut044wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 44:
        sName = "cut045wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 45:
        sName = "cut046wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 46:
        sName = "cut047wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 47:
        sName = "cut048wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 48:
        sName = "cut049wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 49:
        sName = "cut050wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 50:
        sName = "cut051wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 51:
        sName = "cut052wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 52:
        sName = "cut053wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 53:
        sName = "cut054wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 54:
        sName = "cut055wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 55:
        sName = "cut056wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 56:
        sName = "cut057wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 57:
        sName = "cut058wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 58:
        sName = "cut059wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 59:
        sName = "cut060wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 60:
        sName = "cut061wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 61:
        sName = "cut062wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 62:
        sName = "cut063wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 63:
        sName = "cut064wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 64:
        sName = "cut065wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 65:
        sName = "cut066wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 66:
        sName = "cut067wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 67:
        sName = "cut068wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 68:
        sName = "cut069wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 69:
        sName = "cut070wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 70:
        sName = "cut071wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 71:
        sName = "cut072wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 72:
        sName = "cut073wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 73:
        sName = "cut074wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 74:
        sName = "cut075wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 75:
        sName = "cut076wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 76:
        sName = "cut077wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 77:
        sName = "cut078wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 78:
        sName = "cut079wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 79:
        sName = "cut080wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 80:
        sName = "cut081wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 81:
        sName = "cut082wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 82:
        sName = "cut083wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 83:
        sName = "cut084wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 84:
        sName = "cut085wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 85:
        sName = "cut086wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 86:
        sName = "cut087wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 87:
        sName = "cut088wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 88:
        sName = "cut089wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 89:
        sName = "cut090wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 90:
        sName = "cut091wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 91:
        sName = "cut092wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 92:
        sName = "cut093wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 93:
        sName = "cut094wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 94:
        sName = "cut095wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 95:
        sName = "cut096wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 96:
        sName = "cut097wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 97:
        sName = "cut098wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 98:
        sName = "cut099wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 99:
        sName = "cut100wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 100:
        sName = "cut101wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 101:
        sName = "cut102wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 102:
        sName = "cut103wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 103:
        sName = "cut104wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 104:
        sName = "cut105wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 105:
        sName = "cut106wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 106:
        sName = "cut107wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 107:
        sName = "cut108wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 108:
        sName = "cut109wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 109:
        sName = "cut110wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 110:
        sName = "cut111wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 111:
        sName = "cut112wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 112:
        sName = "cut113wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 113:
        sName = "cut114wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 114:
        sName = "cut115wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 115:
        sName = "cut116wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 116:
        sName = "cut117wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 117:
        sName = "cut118wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 118:
        sName = "cut119wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 119:
        sName = "cut120wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 120:
        sName = "cut121wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 121:
        sName = "cut122wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 122:
        sName = "cut123wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 123:
        sName = "cut124wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 124:
        sName = "cut125wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 125:
        sName = "cut126wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 126:
        sName = "cut127wl";
        break;
    case CUTSCENE_ANIMATION_WL_FIRST + 127:
        sName = "cut128wl";
        break;
    }

    return sName;
}
// PC 0x0067F0A0
void CGuiInGame::LoadAmbientTrackForDialog(const CResRef& cAmbientTrack)
{
    if (m_pDialogAmbientTrack != 0)
    {
        delete m_pDialogAmbientTrack;
        m_pDialogAmbientTrack = 0;
    }

    if (cAmbientTrack.IsValid())
    {
        m_pDialogAmbientTrack = new CExoStreamingSoundSource(1);
        if (m_pDialogAmbientTrack != 0)
        {
            m_pDialogAmbientTrack->SetPriorityGroup(
                CExoStreamingSoundSource::PRIORITY_GROUP_DIALOG_AMBIENT);
#ifdef _XBOX
            m_pDialogAmbientTrack->Play(cAmbientTrack, 11, 0, 1, 0);
#else
            m_pDialogAmbientTrack->Play(cAmbientTrack, RESTYPE_MP3, 0, 1, 0);
#endif
        }
    }
}
// PC 0x0067F4F0
void CGuiInGame::ShutDownGlobal()
{
    if (m_pMessageBox != 0)
    {
        delete m_pMessageBox;
        m_pMessageBox = 0;
    }
    if (m_pSkillInfoBox != 0)
    {
        delete m_pSkillInfoBox;
        m_pSkillInfoBox = 0;
    }
    if (m_pTutorialBox != 0)
    {
        delete m_pTutorialBox;
        m_pTutorialBox = 0;
    }
    if (m_pControllerLossBox != 0)
    {
        delete m_pControllerLossBox;
        m_pControllerLossBox = 0;
    }
    m_pGuiManager = 0;
}
// PC 0x0067F580
SFeedbackMessage* CGuiInGame::GetMessageBuffer()
{
    return (SFeedbackMessage*)m_pMessageBuffer;
}
// PC 0x0067F590
int CGuiInGame::GetMessageBufferSize()
{
    return m_MessageBufferSize;
}
// PC 0x0067F930
void CGuiInGame::SetSWGuiStatus(int nStatus, int bActive)
{
    switch (nStatus)
    {
    case GUISTATUS_SHOW_MAIN_INTERFACE:
        if (m_bShowPauseGui != GUISTATE_HIDDEN)
            m_bShowPauseGui = GUISTATE_MAIN_INTERFACE;
        break;
    case GUISTATUS_DIALOG:
        m_bShowPauseGui = GUISTATE_DIALOG;
        break;
    case GUISTATUS_HIDE_MAIN_INTERFACE:
        if (m_bShowPauseGui != GUISTATE_DIALOG)
            m_bShowPauseGui = GUISTATE_HIDDEN;
        break;
    case GUISTATUS_RESTORE_MAIN_INTERFACE:
        if (m_bShowPauseGui != GUISTATE_DIALOG)
            m_bShowPauseGui = GUISTATE_MAIN_INTERFACE;
        break;
    default:
        return;
    }

    if (m_bItemCreateMenuAvailable != 0 && m_pGuiManager != 0)
    {
        if (m_bShowPauseGui == GUISTATE_MAIN_INTERFACE)
        {
            if (m_pGuiManager->PanelExists(m_pMainInterface) == 0)
            {
                m_pGuiManager->AddPanel(m_pMainInterface, SWGUI_PANEL_MODE_OVERLAY, 1);
                m_pGuiManager->SendPanelToBack(m_pMainInterface);
            }
#ifndef _XBOX
            if (m_nPauseState != 0 && m_pPausePanel != 0)
            {
                if (m_pGuiManager->PanelExists(m_pPausePanel) == 0)
                {
                    m_pGuiManager->AddPanel(m_pPausePanel, SWGUI_PANEL_MODE_OVERLAY, 1);
                    m_pGuiManager->SendPanelToBack(m_pPausePanel);
                }
            }
#endif
        }
        else
        {
            if (m_pGuiManager->PanelExists(m_pMainInterface) != 0)
                m_pGuiManager->RemovePanel(m_pMainInterface);
            if (bActive != 0)
            {
                if (m_pGuiManager->PanelExists(m_pPausePanel) != 0)
                    m_pPausePanel->SetModal(0);
            }
        }
    }
}
// PC 0x0067FA60
void CGuiInGame::ShowBarkBubble(unsigned long oidObject,
                                 const CExoString& sText,
                                 const CResRef& cSound, int nFlags)
{
    if (m_bItemCreateMenuAvailable != 0)
    {
        m_pBarkBubble->SetBark(oidObject, sText, cSound, nFlags);
        m_pGuiManager->AddPanel(m_pBarkBubble, 0, 1);
    }
}
// PC 0x0067FFF0
int CGuiInGame::IsLoopingDialogStuntAnimation(unsigned short nAnimation)
{
    return (nAnimation >= 0x578u) && (nAnimation <= 0x6BFu);
}
// PC 0x00680220
void CGuiInGame::DialogGuiDoneScroll()
{
    if (m_pDialogLetterbox != 0)
    {
        m_bDialogScrollFinished = 1;
        m_pDialog->ShowDialogMessage();
    }
}
// PC 0x00680240
int CGuiInGame::IsFadingInDialog()
{
    return m_pDialogFade != 0 && !m_pDialogFade->IsFadeDone();
}
// PC 0x00680260
int CGuiInGame::IsCameraDialog()
{
    return m_pDialog != 0 && m_pDialog == m_pCameraDialog;
}
// PC 0x00680570
void CGuiInGame::StartGlobalFade(int nFade, float fA, float fB,
                                 const Vector& vColor)
{
    if (m_pGlobalFade != 0)
    {
        m_pGuiManager->AddPanel(m_pGlobalFade, SWGUI_PANEL_MODE_OVERLAY, 1);
        m_pGlobalFade->SetVisible(1);
        m_pGlobalFade->SetFade(nFade, fA, fB, vColor);
    }
}
// PC 0x006805E0
void CGuiInGame::StopGlobalFade()
{
    if (m_pGlobalFade != 0)
    {
        m_pGlobalFade->ResetFade();
        m_pGuiManager->RemovePanel(m_pGlobalFade);
    }
}
// PC 0x00680610
int CGuiInGame::IsGlobalFading()
{
    return m_pGlobalFade != 0 && !m_pGlobalFade->IsFadeDone();
}
// PC 0x00680650
int CGuiInGame::InitializePlaceableCameras(int nCameras)
{
    if (nCameras > MAX_PLACEABLE_CAMERAS)
    {
        return 0;
    }
    m_nPlaceableCameras = nCameras;
    return 1;
}
// PC 0x00680670
void CGuiInGame::SetPlaceableCamera(int nCamera, int nPlaceableID,
                                    const Vector& vPosition,
                                    const Quaternion& qOrientation,
                                    float fPitch, float fHeight,
                                    float fFieldOfView, float fMicRange)
{
    if (nCamera < m_nPlaceableCameras)
    {
        m_aPlaceableCameras[nCamera].m_nPlaceableID = nPlaceableID;
        m_aPlaceableCameras[nCamera].m_vPosition = vPosition;
        m_aPlaceableCameras[nCamera].m_qOrientation = qOrientation;
        m_aPlaceableCameras[nCamera].m_fPitch = fPitch;
        m_aPlaceableCameras[nCamera].m_fHeight = fHeight;
        m_aPlaceableCameras[nCamera].m_fFieldOfView = fFieldOfView;
        m_aPlaceableCameras[nCamera].m_fMicRange = fMicRange;
    }
}
// PC 0x00680820
CPlaceableCamera* CGuiInGame::GetPlaceableCamera(int nCamera)
{
    if (nCamera > m_nPlaceableCameras)
    {
        return 0;
    }
    return &m_aPlaceableCameras[nCamera];
}
// PC 0x00680C40
void CGuiInGame::UpdateMessageGUI()
{
    ((CSWGuiInGameMessages*)m_apScreenPanels[4])
        ->AddMessages((CGuiInGameMessage*)m_pMessageBuffer, m_MessageBufferSize);
    ((CSWGuiInGameMessages*)m_apScreenPanels[4])
        ->AddDialogMessages((CGuiInGameDialogMessage*)m_pDialogBuffer,
                            m_nDialogBufferSize);
}

void CGuiInGame::StopBarkBubble(int bFade)
{
    if (m_pBarkBubble != 0)
    {
        m_pBarkBubble->StopBark(bFade);
    }
}
// PC 0x00680C90
int CGuiInGame::SetDialogSelection(int nReply, int nReplyCount, int bSelected)
{
    m_nDialogReply = nReply;
    m_nDialogReplyCount = nReplyCount;
    m_nDialogReplySelection = bSelected;
    m_bDialogReplySelected = 1;
    return bSelected;
}
// PC 0x00681420
CExoString CGuiInGame::DialogNodeSpanningTextParse(const CExoString& sText)
{
    char* pBuffer = 0;
    if (m_nDialogSpanningTextRemaining > 0)
    {
        return m_asDialogSpanningText[
            m_nDialogSpanningTextCount - m_nDialogSpanningTextRemaining--];
    }

    if (sText.Find("##", 0) != -1)
    {
        m_nDialogSpanningTextCount = 0;
        int bError = 0;
        char* pText = new char[sText.GetLength() + 1];
        strcpy(pText, sText.CStr());
        pText[sText.GetLength()] = 0;
        int nPosition = 0;
        pBuffer = pText;
        while (pText[nPosition] != 0)
        {
            if ((pText[nPosition] == '#' && pText[nPosition + 1] == '#') ||
                pText[nPosition + 1] == 0)
            {
                if (nPosition == 0 && pText[1] != 0)
                {
                    EXOXBOXASSERT(0);
                    bError = 1;
                    break;
                }
                int nLength = pText[nPosition + 1] == 0 ?
                    nPosition + 1 : nPosition - 1;
                char* pPart = new char[nLength + 1];
                strncpy(pPart, pText, nLength);
                pPart[nLength] = 0;
                CExoString sPart(pPart);
                if (pPart != 0)
                    delete[] pPart;
                if (m_nDialogSpanningTextCount == MAX_DIALOG_SPANNING_TEXT)
                {
                    EXOXBOXASSERT(0);
                    bError = 1;
                    break;
                }
                m_asDialogSpanningText[m_nDialogSpanningTextCount++] = sPart;
                if (pText[nPosition + 1] != 0)
                {
                    if (pText[nPosition + 2] == 0)
                        break;
                    pText += nPosition + 3;
                    nPosition = 0;
                }
                else
                    ++nPosition;
            }
            else
                ++nPosition;
        }
        delete[] pBuffer;
        if (!bError)
        {
            m_nDialogSpanningTextRemaining = m_nDialogSpanningTextCount;
            return m_asDialogSpanningText[
                m_nDialogSpanningTextCount - m_nDialogSpanningTextRemaining--];
        }
    }
    return sText;
}
// PC 0x006818A0
void CGuiInGame::SetMainInterfaceTarget(unsigned long oidTarget, int bFlag)
{
    if (m_pMainInterface != 0)
    {
        m_pMainInterface->SetTarget(oidTarget, bFlag);
    }
}

#ifndef _XBOX
void CGuiInGame::SetAreaTransition(unsigned long oidArea,  // PC 0x006818C0
                                   const Vector& vPosition)
{
    m_vAreaTransition = vPosition;
    m_oidAreaTransition = oidArea;
}
#endif
// PC 0x00681A40
void CGuiInGame::RePopulateMainInterface()
{
    if (m_pMainInterface != 0)
    {
        m_pMainInterface->PopulateMenus();
    }
}
// PC 0x00681BE0
CSWGuiPanel* CGuiInGame::GetGuiInGameScreenPanel(unsigned char nScreen)
{
    if (nScreen >= 8u)
    {
        return 0;
    }
    return m_apScreenPanels[nScreen];
}
// PC 0x00681C00
void CGuiInGame::AddFloatyText(unsigned long oidObject,
                             const CExoString& sText,
                             const Vector& vColor, float fDuration)
{
    if (m_pMainInterface != NULL)
        m_pMainInterface->AddFloatyText(oidObject, sText, vColor, fDuration);
}
// PC 0x00681C20
void CGuiInGame::InitializeMap()
{
    if (m_pMainInterface != 0)
    {
        m_pMainInterface->InitializeMap();
    }
}
// PC 0x00681E70
void CGuiInGame::ShowFlashingStatus(unsigned char nStatus)
{
    if (m_pMainInterface != 0)
    {
        m_pMainInterface->ShowFlashingStatus(nStatus);
    }
}
// PC 0x00681F10
int CGuiInGame::SetTutorialReason(unsigned char nReason)
{
    if (m_pTutorialBox == 0)
    {
        return 0;
    }
    return m_pTutorialBox->SetTutorialReason(nReason);
}
#ifndef _XBOX
int CGuiInGame::ChangeCharacter()  // PC 0x00682320
{
    return m_pMainInterface->StartRotation();
}
#endif
void CGuiInGame::SetupPauseGuiExtent(CSWGuiExtent* pExtent)  // PC 0x00682330
{
#ifndef _XBOX
    m_pMainInterface->SetupPauseGuiExtent(pExtent);
#endif
}

int CGuiInGame::IsPauseGUIVisible()
{
    return m_pGuiManager->PanelExists(m_pPausePanel);
}
// PC 0x00682360
void CGuiInGame::SetCombatMessage(unsigned long nMessage)
{
    if (m_pMainInterface != 0)
    {
        m_pMainInterface->SetCombatMessage(nMessage);
    }
}
// PC 0x00682380
void CGuiInGame::ShowDialogMessage()
{
    if (m_pDialog == m_pCameraDialog)
    {
        m_pDialog->ShowDialogMessage();
    }
}
// PC 0x006823F0
void CGuiInGame::RemoveGUIPopUps()
{
    CSWGuiPanel* pPanel;
    pPanel = m_pMessageBox;
    if (m_pGuiManager->PanelExists(pPanel) != 0)
    {
        m_pMessageBox->HandleInputEvent(GUIEVENT_CANCEL, 1);
    }
    pPanel = m_pTutorialBox;
    if (m_pGuiManager->PanelExists(pPanel) != 0)
    {
        m_pTutorialBox->HandleInputEvent(GUIEVENT_CANCEL, 1);
    }
    if (m_pGuiManager->PanelExists(m_pSoloModeQuery) != 0)
    {
        m_pSoloModeQuery->HandleInputEvent(GUIEVENT_CANCEL, 1);
    }
    if (m_pGuiManager->PanelExists(m_pContainer) != 0)
    {
        m_pContainer->HandleInputEvent(GUIEVENT_CANCEL, 1);
    }
}
// PC 0x006824A0
void CGuiInGame::UpdateCreatedInGameGUI(int nOldScreen, int nNewScreen)
{
    if (g_bCreateAllGUIs == 0 && nOldScreen != nNewScreen)
    {
        if (nOldScreen != SCREEN_NONE)
        {
            CSWGuiPanel* pOldPanel = m_apScreenPanels[nOldScreen];
            if (pOldPanel != 0)
            {
                pOldPanel->m_bDeferredRemoval = 0;
                pOldPanel->m_nExitState = CSWGuiPanel::EXITSTATE_DELETE;
                m_apScreenPanels[nOldScreen] = 0;
            }
        }
        if (m_apScreenPanels[nNewScreen] == 0)
        {
            CSWGuiPanel* pNewPanel;
            switch (nNewScreen)
            {
            case SCREEN_EQUIP:
                pNewPanel = new CSWGuiInGameEquip(m_pGuiManager);
                break;
            case SCREEN_INVENTORY:
                pNewPanel = new CSWGuiInGameInventory(m_pGuiManager);
                break;
            case SCREEN_CHARACTER:
                pNewPanel = new CSWGuiInGameCharacter(m_pGuiManager);
                break;
            case SCREEN_ABILITIES:
                pNewPanel = new CSWGuiInGameAbilities(m_pGuiManager);
                break;
            case SCREEN_MESSAGES:
                pNewPanel = new CSWGuiInGameMessages(m_pGuiManager);
                break;
            case SCREEN_JOURNAL:
                pNewPanel = new CSWGuiInGameJournal(m_pGuiManager);
                break;
            case SCREEN_MAP:
                pNewPanel = new CSWGuiInGameMap(m_pGuiManager);
                break;
            case SCREEN_OPTIONS:
                pNewPanel = new CSWGuiInGameOptions(m_pGuiManager);
                break;
            }
            m_apScreenPanels[nNewScreen] = pNewPanel;
        }
    }
}

#ifdef _XBOX
void CGuiInGame::SetGlobalUnpauseableState(int bUnpauseable)
{
    if (bUnpauseable == 1)
    {
        ++m_nGlobalUnpauseableState;
    }
    else if (m_nGlobalUnpauseableState > 0)
    {
        --m_nGlobalUnpauseableState;
    }
    EXOXBOXASSERT(m_nGlobalUnpauseableState != -1);

}
#else
int CGuiInGame::SetGlobalUnpauseableState(int bUnpauseable)  // PC 0x006826D0
{
    if (bUnpauseable == 1)
    {
        return ++m_nGlobalUnpauseableState;
    }
    if (m_nGlobalUnpauseableState > 0)
    {
        return --m_nGlobalUnpauseableState;
    }
    return m_nGlobalUnpauseableState;
}
#endif
// PC 0x006827D0
void CGuiInGame::SetDialogSound(unsigned long nSoundId, const CResRef& cSound)
{
    m_nDialogSoundId = nSoundId;
    m_cDialogSound = cSound;
}
// PC 0x006828A0
void CGuiInGame::SetDeferredFOVCallForAnimatedCutscenes(float fFieldOfView)
{
    m_bDeferredFOVCall = 1;
    m_fDeferredFieldOfView = fFieldOfView;
}
// PC 0x006829D0
void CGuiInGame::ResetInterfaceFont()
{
    if (m_pMainInterface != 0)
    {
        m_pMainInterface->ResetFont();
    }
    for (int nScreen = 0; nScreen < IN_GAME_SCREEN_COUNT; ++nScreen)
    {
        if (m_apScreenPanels[nScreen] != 0)
        {
            m_apScreenPanels[nScreen]->ResetFont();
        }
    }
}
// PC 0x00682A10
void CGuiInGame::ShowDemoScreen(CExoString sImage, int nMinShowTime, int nStrRef, int nLeft, int nTop)
{
    if (m_bItemCreateMenuAvailable != 0 && m_pGuiManager != NULL && m_pDemoScreen == NULL)
    {
        g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_PAUSE_ALL_BUT_MUSIC);
        if (m_bPaused == 0)
        {
            g_pAppManager->m_pClientExoApp->GetSWCMessage()->SendPlayerToServerInput_TogglePauseRequest();
        }
        m_pDemoScreen = new CSWDemoScreen(m_pGuiManager, nMinShowTime, nStrRef, nLeft, nTop);
        m_pGuiManager->AddPanel(m_pDemoScreen, 3, 1);
        CResRef cImage(sImage.CStr());
        m_pDemoScreen->SetImage(cImage);
        g_pAppManager->m_pClientExoApp->SetInputClass(2, 1);
    }
}
// PC 0x00682B40
void CGuiInGame::HideDemoScreen()
{
    if (m_bItemCreateMenuAvailable != 0 && m_pGuiManager != NULL && m_pDemoScreen != NULL)
    {
        if (m_bPaused == 0)
        {
            g_pAppManager->m_pClientExoApp->GetSWCMessage()->SendPlayerToServerInput_TogglePauseRequest();
        }
        g_pAppManager->m_pClientExoApp->SetInputClass(0, 1);
        g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_GENERIC);
        m_pDemoScreen->m_nExitState = 2;
        m_pDemoScreen->m_bDeferredRemoval = 0;
        m_pDemoScreen = NULL;
    }
}
// PC 0x00682C40
void CGuiInGame::DisplayDeathMessage()
{
    if (m_pDeathDisplay == NULL)
    {
        m_pDeathDisplay = new CSWGuiDeathDisplay(m_pGuiManager);
    }
    g_pAppManager->m_pClientExoApp->SetInputClass(2, 0);
    m_pGuiManager->AddPanel(m_pDeathDisplay, 1, 1);
}
// PC 0x00682CE0
void CGuiInGame::RemoveDeathMessage(bool bLoading)
{
    if (m_pDeathDisplay != NULL)
    {
        if (bLoading && m_pGuiManager->PanelExists(m_pDeathDisplay))
        {
            m_pGuiManager->RemovePanel(m_pDeathDisplay);
        }
        m_pDeathDisplay->m_nExitState = 2;
        m_pDeathDisplay->m_bDeferredRemoval = 0;
        m_pDeathDisplay = NULL;
    }
}
// PC 0x00682DA0
int CGuiInGame::GetTopMenuOverrideCharacter()
{
    if (m_pInGameMenu != NULL)
        return m_pInGameMenu->m_nTopMenuOverrideCharacter;
    return -1;
}
// PC 0x00682DC0
int CGuiInGame::GetGlobalFadeActive()
{
    if (m_pGlobalFade != NULL && m_pGuiManager->PanelExists(m_pGlobalFade) &&
        m_pGlobalFade->m_fAlpha >= 0.01f)
    {
        return 1;
    }
    return 0;
}
int CGuiInGame::GetPartyAccessPanelUp(int bAny)
{
    if (m_pGuiManager == 0)
    {
        return 0;
    }
    if (m_pGuiManager->PanelExists(m_apScreenPanels[0]) != 0)
    {
        return 1;
    }
    return bAny == 0 && m_pGuiManager->PanelExists(m_apScreenPanels[1]) != 0;
}
// PC 0x0067CAE0
void CSWGuiStatusSummary::AddExperience(int nExperience)
{
    GUIINGAME_TRACE();
    if (nExperience != 0)
    {
        m_cStatusSummary.m_nExperience += nExperience;
        m_cStatusSummary.m_bDisplaySPending = 1;
    }
}
// PC 0x0067CB00
void CSWGuiStatusSummary::AddStealthExperience(int nExperience)
{
    GUIINGAME_TRACE();
    if (nExperience != 0)
    {
        m_cStatusSummary.m_nStealthExperience += nExperience;
        m_cStatusSummary.m_bDisplaySPending = 1;
    }
}
// PC 0x00682E70
void CGuiInGame::AppendToMsgBuffer(const CExoString& sMessage,
                                   unsigned long nType, unsigned char nColor)
{
    GUIINGAME_TRACE();
    if (sMessage != "")
    {
        if (m_MessageBufferSize >= 64)
        {
            m_MessageBufferSize = 63;
            for (unsigned short i = 0; i < m_MessageBufferSize; i++)
            {
                m_pMessageBuffer[i] = m_pMessageBuffer[i + 1];
            }
        }
        m_pMessageBuffer[m_MessageBufferSize].m_sMessage = sMessage;
        m_pMessageBuffer[m_MessageBufferSize].m_nType = nType;
        m_pMessageBuffer[m_MessageBufferSize++].m_nColor = nColor;
    }
}
// PC 0x0067CC10
void CGuiInGame::AppendToCombatBuffer(const CExoString& sMessage,
                                      unsigned long nType, unsigned char nColor)
{
    if (sMessage != "")
    {
        if (m_nCombatBufferSize >= 64)
        {
            m_nCombatBufferSize = 63;
            for (unsigned long i = 0; i < m_nCombatBufferSize; i++)
            {
                m_pCombatBuffer[i] = m_pCombatBuffer[i + 1];
            }
        }
        m_pCombatBuffer[m_nCombatBufferSize].m_sMessage = sMessage;
        m_pCombatBuffer[m_nCombatBufferSize].m_nType = nType;
        m_pCombatBuffer[m_nCombatBufferSize++].m_nColor = nColor;
    }
}
// PC 0x00685C90
void CGuiInGame::AppendToDialogBuffer(const CExoString& sMessage,
                                      unsigned long oidSpeaker)
{
    if (sMessage != "")
    {
        CClientExoApp* pApp = g_pAppManager->m_pClientExoApp;
        CSWCObject* pObject = pApp->GetGameObject(oidSpeaker);
        if (pObject != 0)
        {
            CExoString sName;
            sName = pObject->AsSWCObject()->GetName();
            AppendToDialogBuffer(sMessage, sName);
        }
    }
}
// PC 0x00682F30
void CGuiInGame::AppendToDialogBuffer(const CExoString& sMessage,
                                      const CExoString& sSpeaker)
{
    GUIINGAME_TRACE();
    if (sMessage != "")
    {
        if (m_nDialogBufferSize >= 64)
        {
            m_nDialogBufferSize = 63;
            for (unsigned short i = 0; i < m_nDialogBufferSize; i++)
            {
                m_pDialogBuffer[i] = m_pDialogBuffer[i + 1];
            }
        }
        m_pDialogBuffer[m_nDialogBufferSize].m_sMessage = sMessage;
        m_pDialogBuffer[m_nDialogBufferSize].m_sSpeaker = sSpeaker;
        m_nDialogBufferSize++;
    }
}
// PC 0x00686840
int CGuiInGame::InitializeDialogAnimationData(unsigned long nAnimations)
{
    int bResult = 0;

    StoreLastDialogAnimationData();
    ClearDialogAnimationData();
    if (nAnimations != 0)
    {
        m_nDialogAnimations = nAnimations;
        m_pDialogAnimations = new unsigned long[nAnimations];
        m_pDialogAnimationFlags = new unsigned short[m_nDialogAnimations];
        if (m_pDialogAnimations != 0 && m_pDialogAnimationFlags != 0)
        {
            bResult = 1;
        }
        else
        {
            delete m_pDialogAnimations;
            delete m_pDialogAnimationFlags;
            bResult = 0;
        }
    }
    return bResult;
}
// PC 0x00686900
void CGuiInGame::UpdateShotCamera(unsigned long nShot, int nCamera,
                                 float fPitch, float fHeight,
                                 unsigned long oidTarget, int nVideoEffect)
{
    unsigned long oidPrevious = m_oidPreviousDialogObject1;
    CSWCObject* pObject = g_pAppManager->GetClientExoApp()->GetGameObject(oidPrevious);
    if (pObject != 0 && pObject->AsSWCCreature() != 0)
        pObject->AsSWCCreature()->LookAt(OBJECT_INVALID, 0.0f);
    oidPrevious = m_oidPreviousDialogObject2;
    pObject = g_pAppManager->GetClientExoApp()->GetGameObject(oidPrevious);
    if (pObject != 0 && pObject->AsSWCCreature() != 0)
        pObject->AsSWCCreature()->LookAt(OBJECT_INVALID, 0.0f);

    if (nShot == DIALOG_SHOT_KEEP && m_oidPreviousDialogObject1 != OBJECT_INVALID)
    {
        m_oidDialogObject1 = m_oidPreviousDialogObject1;
        m_oidDialogObject2 = m_oidPreviousDialogObject2;
    }

    int nDialogState = g_pAppManager->GetClientExoApp()->GetInGameGui()->m_nC0;
    if (nDialogState == 0 && m_pDialog != m_pComputerDialog)
    {
        unsigned long oidFirst = m_oidDialogObject1;
#ifdef _XBOX
        CSWCObject* pXboxFirst = g_pAppManager->GetClientExoApp()->GetGameObject(oidFirst);
        EXOXBOXASSERT(pXboxFirst);
        CSWCObject* pFirst = pXboxFirst->AsSWCObject();
#else
        CSWCObject* pFirst = g_pAppManager->GetClientExoApp()->GetGameObject(oidFirst)->AsSWCObject();
#endif
        unsigned long oidSecond = m_oidDialogObject2;
#ifdef _XBOX
        CSWCObject* pXboxSecond = g_pAppManager->GetClientExoApp()->GetGameObject(oidSecond);
        EXOXBOXASSERT(pXboxSecond);
        CSWCObject* pSecond = pXboxSecond->AsSWCObject();
#else
        CSWCObject* pSecond = g_pAppManager->GetClientExoApp()->GetGameObject(oidSecond)->AsSWCObject();
#endif
        if (pFirst != 0 && pSecond != 0)
        {
            Vector vDirection = normalize(pFirst->m_vPosition - pSecond->m_vPosition);
            Vector vOrientation = pSecond->m_vOrientation;
            if (GetCanOrientInDialog(pSecond->GetId()) &&
                pSecond->AsSWCCreature() != 0 &&
                pSecond->AsSWCCreature()->m_fWalkSpeed > 0.0f)
            {
                int bHeadFollow = 1;
                if (GetCanHeadFollowInDialog(pSecond->GetId()))
                {
                    Vector vTarget = normalize(pFirst->m_vPosition - pSecond->m_vPosition);
                    Vector vFacing = normalize(pSecond->m_vOrientation);
                    float fAngle = max(-1.0f, min(1.0f, vTarget * vFacing));
                    fAngle = (float)acos(fAngle) * RAD_TO_DEG;
                    float fHeadTurn = pSecond->AsSWCCreature()->GetHeadTurnHorizontal();
                    if (!_finite(fAngle))
                        fAngle = 0.0f;
                    if (fAngle > fHeadTurn)
                    {
                        fAngle = fAngle - (fHeadTurn + 1.0f);
                        Vector vCross = cross(vFacing, Vector(0.0f, 0.0f, 1.0f));
                        if (vCross * vTarget > 0.0f)
                            fAngle *= -1.0f;
                        Quaternion qRotation(Vector(0.0f, 0.0f, 1.0f),
                            fAngle * DEG_TO_RAD);
                        vOrientation = qRotation * vFacing;
                    }
                }
                else
                    bHeadFollow = 0;

                pSecond->AsSWCCreature()->SetLockOrientationToObject(OBJECT_INVALID, 0);
                unsigned long oidServer = g_pAppManager->GetServerExoApp()->
                    ClientToServerObjectId(pSecond->GetId());
                CGameObject* pServerObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidServer);
                if (pServerObject != 0 && pServerObject->AsSWSCreature() != 0)
                    pServerObject->AsSWSCreature()->SetLockOrientationToObject(OBJECT_INVALID, 0);
                if (bHeadFollow && pSecond->AsSWCCreature()->LookAt(pFirst->GetId(), 10.0f))
                {
                    if (pServerObject != 0 && pServerObject->AsSWSCreature() != 0)
                        pServerObject->AsSWSCreature()->SetOrientation(vOrientation);
                    pSecond->AsSWCCreature()->SetDesiredOrientation(vOrientation);
                    pSecond->AsSWCCreature()->SetOrientation(vOrientation);
                }
                else
                {
                    if (pServerObject != 0 && pServerObject->AsSWSCreature() != 0)
                        pServerObject->AsSWSCreature()->SetOrientation(vDirection);
                    pSecond->AsSWCCreature()->SetDesiredOrientation(vDirection);
                    pSecond->AsSWCCreature()->SetOrientation(vDirection);
                }
            }

            vOrientation = pFirst->m_vOrientation;
            if (GetCanOrientInDialog(pFirst->GetId()) &&
                pFirst->AsSWCCreature() != 0 &&
                pFirst->AsSWCCreature()->m_fWalkSpeed > 0.0f)
            {
                int bHeadFollow = 1;
                if (GetCanHeadFollowInDialog(pFirst->GetId()))
                {
                    Vector vTarget = normalize(pSecond->m_vPosition - pFirst->m_vPosition);
                    Vector vFacing = normalize(pFirst->m_vOrientation);
                    float fAngle = max(-1.0f, min(1.0f, vTarget * vFacing));
                    fAngle = (float)acos(fAngle) * RAD_TO_DEG;
                    float fHeadTurn = pFirst->AsSWCCreature()->GetHeadTurnHorizontal();
                    if (!_finite(fAngle))
                        fAngle = 0.0f;
                    if (fAngle > fHeadTurn)
                    {
                        fAngle = fAngle - (fHeadTurn + 1.0f);
                        Vector vCross = cross(vFacing, Vector(0.0f, 0.0f, 1.0f));
                        if (vCross * vTarget > 0.0f)
                            fAngle *= -1.0f;
                        Quaternion qRotation(Vector(0.0f, 0.0f, 1.0f),
                            fAngle * DEG_TO_RAD);
                        vOrientation = qRotation * vFacing;
                    }
                }
                else
                    bHeadFollow = 0;

                pFirst->AsSWCCreature()->SetLockOrientationToObject(OBJECT_INVALID, 0);
                unsigned long oidServer = g_pAppManager->GetServerExoApp()->
                    ClientToServerObjectId(pFirst->GetId());
                CGameObject* pServerObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidServer);
                if (pServerObject != 0 && pServerObject->AsSWSCreature() != 0)
                    pServerObject->AsSWSCreature()->SetLockOrientationToObject(OBJECT_INVALID, 0);
                if (bHeadFollow && pFirst->AsSWCCreature()->LookAt(pSecond->GetId(), 10.0f))
                {
                    if (pServerObject != 0 && pServerObject->AsSWSCreature() != 0)
                        pServerObject->AsSWSCreature()->SetOrientation(vOrientation);
                    pFirst->AsSWCCreature()->SetDesiredOrientation(vOrientation);
                    pFirst->AsSWCCreature()->SetOrientation(vOrientation);
                }
                else
                {
                    if (pServerObject != 0 && pServerObject->AsSWSCreature() != 0)
                        pServerObject->AsSWSCreature()->SetOrientation(-vDirection);
                    pFirst->AsSWCCreature()->SetDesiredOrientation(-vDirection);
                    pFirst->AsSWCCreature()->SetOrientation(-vDirection);
                }
            }
        }
    }

    if (nShot != DIALOG_SHOT_KEEP)
    {
        CSWBehaviorCameraDialog* pCamera = reinterpret_cast<CSWBehaviorCameraDialog*>(
            g_pAppManager->GetClientExoApp()->GetModule()->GetCamera()->
                GetBehavior(AUR_BEHAVIOR_CAMERA_DIALOG));
        if (nShot != DIALOG_SHOT_COMPUTER)
        {
            if (pCamera != 0)
            {
                float fPullBackRange = 0.0f;
                if (nShot == DIALOG_SHOT_CLOSEUP)
                {
                    unsigned short nAnimation = 10038;
                    for (int nIndex = 0; nIndex < m_nDialogAnimations; nIndex++)
                    {
                        unsigned long oidAnimation = m_pDialogAnimations[nIndex];
                        unsigned short nDialogAnimation = m_pDialogAnimationFlags[nIndex];
                        unsigned long oidDialogObject = m_oidDialogObject1;
                        if (oidAnimation == oidDialogObject)
                        {
                            nAnimation = nDialogAnimation;
                            break;
                        }
                    }
                    if (g_pAppManager->GetClientExoApp()->GetInGameGui()->IsFireForgetDialogAnimation(nAnimation) ||
                        g_pAppManager->GetClientExoApp()->GetInGameGui()->IsLoopingDialogAnimation(nAnimation))
                    {
                        if (!g_pRules->Get2DArrays()->GetDialogAnimations()->GetFLOATEntry(
                                nAnimation - SERVER_ANIMATION_FIRST,
                                "CU_PB_Range", &fPullBackRange))
                            fPullBackRange = 0.0f;
                        if (fPullBackRange < 0.001 && fPullBackRange > -0.001)
                            fPullBackRange = 0.0f;
                        if (fPullBackRange == 0.0f)
                            m_fDialogCameraPullBackRange = 0.0f;
                    }
                    if (m_fDialogCameraPullBackRange > 0.0f)
                        fPullBackRange = m_fDialogCameraPullBackRange;
                    else
                        m_fDialogCameraPullBackRange = fPullBackRange;
                }
                else
                    m_fDialogCameraPullBackRange = 0.0f;
                pCamera->SetShot(m_oidDialogObject1, m_oidDialogObject2,
                    nShot, fPitch, fHeight, fPullBackRange);
                g_pAppManager->GetClientExoApp()->DisableVideoEffect();
            }
        }
        else if (pCamera != 0)
        {
            int bFound = 0;
            int nIndex;
            for (nIndex = 0; nIndex < m_nPlaceableCameras; nIndex++)
            {
                if (m_aPlaceableCameras[nIndex].m_nPlaceableID == nCamera)
                {
                    bFound = 1;
                    break;
                }
            }
            Vector vPosition;
            Quaternion qOrientation;
            float fFieldOfView = GUI_COMPUTER_CAMERA_DEFAULT_FOV;
            float fMicRange = 0.0f;
            if (bFound)
            {
                vPosition = m_aPlaceableCameras[nIndex].m_vPosition;
                vPosition.m_fZ += m_aPlaceableCameras[nIndex].m_fHeight;
                qOrientation = m_aPlaceableCameras[nIndex].m_qOrientation;
                qOrientation = YawPitchRoll(Yaw(qOrientation),
                    Pitch(qOrientation) + m_aPlaceableCameras[nIndex].m_fPitch,
                    Roll(m_aPlaceableCameras[nIndex].m_qOrientation));
                fFieldOfView = m_aPlaceableCameras[nIndex].m_fFieldOfView;
                fMicRange = m_aPlaceableCameras[nIndex].m_fMicRange;
            }
            pCamera->SetShot(CSWBehaviorCameraDialog::FIXED_CAMERA_SHOT,
                vPosition, qOrientation, fFieldOfView);
            m_bComputerCameraShot = 1;
            m_vComputerCameraShot = vPosition;
            m_fComputerCameraMicRange = fMicRange;
            if (nVideoEffect != CAMERA_VIDEO_EFFECT_DEFAULT &&
                nVideoEffect != CAMERA_VIDEO_EFFECT_UNCHANGED)
                g_pAppManager->GetClientExoApp()->EnableVideoEffect(nVideoEffect);
        }
    }
    m_oidComputerCameraTarget = oidTarget;
    m_bDialogSoundQueried = 0;
}
// PC 0x00687CB0
void CGuiInGame::SetDialogAnimations()
{
    int i;

    for (i = 0; i < 3; ++i)
    {
        CSWCObject* pObject = g_pAppManager->GetClientExoApp()->GetGameObject(m_aDialogParticipants[i]);
        if (pObject != 0 && pObject->AsSWCCreature() != 0)
        {
            CSWCCreature* pCreature = pObject->AsSWCCreature();
            pCreature->m_pAnimBase->SetInCutscene(0);
            m_aDialogParticipants[i] = OBJECT_INVALID;
        }
    }

    int bPreviousSpeaker = 0;
    int bPreviousListener = 0;
    for (i = 0; i < m_nLastDialogAnimations; ++i)
    {
        unsigned long oidObject = m_pLastDialogAnimations[i];
        CSWCObject* pObject = g_pAppManager->GetClientExoApp()->GetGameObject(oidObject);
        if (pObject == 0)
            continue;
        int bFound = 0;
        for (int j = 0; j < m_nDialogAnimations; ++j)
        {
            if (oidObject == m_pDialogAnimations[j])
            {
                bFound = 1;
                break;
            }
        }
        if (oidObject == m_oidPreviousDialogObject2)
            bPreviousListener = 1;
        if (oidObject == m_oidPreviousDialogObject1)
            bPreviousSpeaker = 1;
        if (!bFound)
        {
            if (pObject->AsSWCCreature() != 0)
            {
                CSWCCreature* pCreature = pObject->AsSWCCreature();
                unsigned long oidCreature = pCreature->m_nId;
                if (CanPlayDialogAnimations(oidCreature) && pCreature != 0)
                {
                    pCreature->m_bPlayedOutOfPlace = 0;
                    if (pCreature->GetCurrentAnimation() != CREATURE_ANIMATION_COMBAT_IDLE)
                        pCreature->SetAnimation(CREATURE_ANIMATION_COMBAT_IDLE, 0, 1);
                    pCreature->m_pAnimBase->SetInCutscene(0);
                }
            }
            if (pObject->AsSWCPlaceable() != 0)
            {
                CSWCPlaceable* pPlaceable = pObject->AsSWCPlaceable();
                CSWSPlaceable* pServerPlaceable = pPlaceable->GetServerPlaceable();
                if (pPlaceable != 0)
                {
                    pPlaceable->m_bPlayedOutOfPlace = 0;
                    pPlaceable->SetAnimation(pServerPlaceable->m_nAnimation, 0, 0);
                    pPlaceable->m_pAnimBase->SetInCutscene(0);
                }
#ifdef _XBOX
                else
                {
                    EXOXBOXASSERT(0);
                }
#endif
            }
        }
    }

    if (!bPreviousListener)
    {
        unsigned long oidObject = m_oidPreviousDialogObject2;
        CSWCObject* pObject = g_pAppManager->GetClientExoApp()->GetGameObject(oidObject);
        if (pObject != 0 && pObject->AsSWCObject() != 0)
        {
            if (pObject->AsSWCCreature() != 0)
            {
                if (CanPlayDialogAnimations(pObject->AsSWCCreature()->GetId()))
                {
                    if (pObject->AsSWCObject()->GetCurrentAnimation() != CREATURE_ANIMATION_COMBAT_IDLE)
                        pObject->AsSWCObject()->SetAnimation(CREATURE_ANIMATION_COMBAT_IDLE, 0, 1);
                    CSWCObject* pCutsceneObject = pObject->AsSWCObject();
                    pCutsceneObject->m_pAnimBase->SetInCutscene(0);
                }
            }
            else
            {
                if (pObject->AsSWCObject()->GetCurrentAnimation() != CREATURE_ANIMATION_COMBAT_IDLE)
                    pObject->AsSWCObject()->SetAnimation(CREATURE_ANIMATION_COMBAT_IDLE, 0, 1);
                CSWCObject* pCutsceneObject = pObject->AsSWCObject();
                pCutsceneObject->m_pAnimBase->SetInCutscene(0);
            }
        }
    }
    if (!bPreviousSpeaker)
    {
        unsigned long oidObject = m_oidPreviousDialogObject1;
        CSWCObject* pObject = g_pAppManager->GetClientExoApp()->GetGameObject(oidObject);
        if (pObject != 0 && pObject->AsSWCObject() != 0)
        {
            if (pObject->AsSWCCreature() != 0)
            {
                if (CanPlayDialogAnimations(pObject->AsSWCCreature()->GetId()))
                {
                    if (pObject->AsSWCObject()->GetCurrentAnimation() != CREATURE_ANIMATION_COMBAT_IDLE)
                        pObject->AsSWCObject()->SetAnimation(CREATURE_ANIMATION_COMBAT_IDLE, 0, 1);
                    CSWCObject* pCutsceneObject = pObject->AsSWCObject();
                    pCutsceneObject->m_pAnimBase->SetInCutscene(0);
                }
            }
            else
            {
                if (pObject->AsSWCObject()->GetCurrentAnimation() != CREATURE_ANIMATION_COMBAT_IDLE)
                    pObject->AsSWCObject()->SetAnimation(CREATURE_ANIMATION_COMBAT_IDLE, 0, 1);
                CSWCObject* pCutsceneObject = pObject->AsSWCObject();
                pCutsceneObject->m_pAnimBase->SetInCutscene(0);
            }
        }
    }

    int bSpeaker = 0;
    int bListener = 0;
    for (i = 0; i < m_nDialogAnimations; ++i)
    {
        unsigned long oidObject = m_pDialogAnimations[i];
        unsigned short nAnimation = m_pDialogAnimationFlags[i];
        CSWCObject* pObject = g_pAppManager->GetClientExoApp()->GetGameObject(oidObject);
        if (pObject == 0)
            continue;
        int bFound = 0;
        unsigned short nPreviousAnimation;
        for (int j = 0; j < m_nLastDialogAnimations; ++j)
        {
            if (oidObject == m_pLastDialogAnimations[j])
            {
                bFound = 1;
                nPreviousAnimation = m_pLastDialogAnimationFlags[j];
                break;
            }
        }
        if (oidObject == m_oidDialogObject2)
            bSpeaker = 1;
        if (oidObject == m_oidDialogObject1)
            bListener = 1;
        if (!bFound || nAnimation != nPreviousAnimation)
        {
            if (pObject->AsSWCCreature() != 0)
            {
                CSWCCreature* pCreature = pObject->AsSWCCreature();
                if (pCreature != 0 && CanPlayDialogAnimations(pCreature->GetId()))
                {
                    if (IsLoopingDialogAnimation(nAnimation))
                    {
                        pCreature->m_bPlayedOutOfPlace = 0;
                        pCreature->m_pAnimBase->SetInCutscene(1);
                        pCreature->LookAt(OBJECT_INVALID, 0.0f);
                        if (pCreature->GetCurrentAnimation() != CREATURE_ANIMATION_COMBAT_IDLE)
                        {
                            if (IsOverlayDialogAnimation(nAnimation))
                                pCreature->AbortFireForgetAnimation(0);
                            pCreature->SetAnimation(CREATURE_ANIMATION_COMBAT_IDLE, 0, 1);
                        }
                        pCreature->SetAnimation(nAnimation, 0, 0);
                    }
                    if (IsFireForgetDialogAnimation(nAnimation))
                    {
                        pCreature->m_bPlayedOutOfPlace = 0;
                        pCreature->m_pAnimBase->SetInCutscene(1);
                        pCreature->LookAt(OBJECT_INVALID, 0.0f);
                        if (pCreature->GetCurrentAnimation() != CREATURE_ANIMATION_COMBAT_IDLE)
                            pCreature->SetAnimation(CREATURE_ANIMATION_COMBAT_IDLE, 0, 1);
                        if (nAnimation >= CUTSCENE_ANIMATION_FIRST && nAnimation <= CUTSCENE_ANIMATION_W_LAST)
                        {
                            pCreature->AbortFireForgetAnimation(0);
                            pCreature->RemoveAllFireForgetAnimations();
                            pCreature->AddFireForgetAnimation(nAnimation, 0);
                            pCreature->AnimateFireAndForget();
                        }
                        else
                            pCreature->AddFireForgetAnimation(nAnimation, 0);
                    }
                }
#ifdef _XBOX
                else
                {
                    EXOXBOXASSERT(0);
                }
#endif
            }
            if (pObject->AsSWCPlaceable() != 0)
            {
                CSWCPlaceable* pPlaceable = pObject->AsSWCPlaceable();
                if (pPlaceable != 0)
                {
                    if (IsLoopingDialogAnimation(nAnimation))
                    {
                        pPlaceable->m_bPlayedOutOfPlace = 0;
                        pPlaceable->m_pAnimBase->SetInCutscene(1);
                        if (pPlaceable->GetCurrentAnimation() != CREATURE_ANIMATION_COMBAT_IDLE)
                            pPlaceable->SetAnimation(CREATURE_ANIMATION_COMBAT_IDLE, 0, 0);
                        pPlaceable->SetAnimation(nAnimation, 0, 0);
                    }
                    if (IsFireForgetDialogAnimation(nAnimation))
                    {
                        pPlaceable->m_bPlayedOutOfPlace = 0;
                        pPlaceable->m_pAnimBase->SetInCutscene(1);
                        if (pPlaceable->GetCurrentAnimation() != CREATURE_ANIMATION_COMBAT_IDLE)
                            pPlaceable->SetAnimation(CREATURE_ANIMATION_COMBAT_IDLE, 0, 0);
                        if (nAnimation >= CUTSCENE_ANIMATION_FIRST && nAnimation <= CUTSCENE_ANIMATION_W_LAST)
                        {
                            pPlaceable->AbortFireForgetAnimation(0);
                            pPlaceable->RemoveAllFireForgetAnimations();
                            pPlaceable->AddFireForgetAnimation(nAnimation, 0);
                        }
                        else
                            pPlaceable->AddFireForgetAnimation(nAnimation, 0);
                    }
                }
#ifdef _XBOX
                else
                {
                    EXOXBOXASSERT(0);
                }
#endif
            }
        }
    }

    if (!bSpeaker)
    {
        unsigned long oidObject = m_oidDialogObject2;
        CSWCObject* pObject = g_pAppManager->GetClientExoApp()->GetGameObject(oidObject);
        if (pObject != 0 && pObject->AsSWCObject() != 0 && pObject->AsSWCCreature() != 0 &&
            CanPlayDialogAnimations(pObject->AsSWCCreature()->GetId()))
        {
            CSWCObject* pCutsceneObject = pObject->AsSWCObject();
            pCutsceneObject->m_pAnimBase->SetInCutscene(1);
            if (pObject->AsSWCObject()->GetCurrentAnimation() != CREATURE_ANIMATION_COMBAT_IDLE)
            {
                pObject->AsSWCObject()->AbortFireForgetAnimation(0);
                pObject->AsSWCObject()->SetAnimation(CREATURE_ANIMATION_COMBAT_IDLE, 0, 1);
            }
            if (!pObject->AsSWCCreature()->IsInjured())
                pObject->AsSWCObject()->SetAnimation(CREATURE_ANIMATION_DIALOG_SPEAK, 0, 0);
            else
                pObject->AsSWCObject()->SetAnimation(CREATURE_ANIMATION_DIALOG_SPEAK_INJURED, 0, 0);
        }
    }
    if (!bListener)
    {
        unsigned long oidObject = m_oidDialogObject1;
        CSWCObject* pObject = g_pAppManager->GetClientExoApp()->GetGameObject(oidObject);
        if (pObject != 0 && pObject->AsSWCObject() != 0 && pObject->AsSWCCreature() != 0 &&
            CanPlayDialogAnimations(pObject->AsSWCCreature()->GetId()))
        {
            CSWCObject* pCutsceneObject = pObject->AsSWCObject();
            pCutsceneObject->m_pAnimBase->SetInCutscene(1);
            if (pObject->AsSWCObject()->GetCurrentAnimation() != CREATURE_ANIMATION_COMBAT_IDLE)
            {
                pObject->AsSWCObject()->AbortFireForgetAnimation(0);
                pObject->AsSWCObject()->SetAnimation(CREATURE_ANIMATION_COMBAT_IDLE, 0, 1);
            }
            if (!pObject->AsSWCCreature()->IsInjured())
                pObject->AsSWCObject()->SetAnimation(CREATURE_ANIMATION_DIALOG_LISTEN, 0, 0);
            else
                pObject->AsSWCObject()->SetAnimation(CREATURE_ANIMATION_DIALOG_LISTEN_INJURED, 0, 0);
        }
    }
    for (i = 0; i < 3; ++i)
    {
        CSWCCreature* pCharacter = g_pAppManager->GetClientExoApp()->GetSWParty()->GetCharacter(i);
        if (pCharacter != 0)
        {
            pCharacter->m_pAnimBase->SetInCutscene(1);
            m_aDialogParticipants[i] = pCharacter->m_nId;
        }
        else
            m_aDialogParticipants[i] = OBJECT_INVALID;
    }
}
// PC 0x00683AB0
int CGuiInGame::ResetDialogAnimations()
{
    int nAnimations = m_nLastDialogAnimations;
    for (unsigned int i = 0; i < nAnimations; ++i)
    {
        unsigned long oidObject = m_pLastDialogAnimations[i];
        CClientExoApp* pClientApp = g_pAppManager->m_pClientExoApp;
        CSWCObject* pObject = pClientApp->GetGameObject(oidObject);
        if (pObject != 0)
        {
            if (pObject->AsSWCCreature() != 0)
            {
                CSWCCreature* pCreature = pObject->AsSWCCreature();
                unsigned long oidCreature = pCreature->m_nId;
                if (CanPlayDialogAnimations(oidCreature))
                {
                    pCreature->m_bPlayedOutOfPlace = 0;
                    if (pCreature->GetCurrentAnimation() != CREATURE_ANIMATION_COMBAT_IDLE)
                        pCreature->SetAnimation(CREATURE_ANIMATION_COMBAT_IDLE, 0, 1);
                    pCreature->m_pAnimBase->SetInCutscene(0);
                }
            }
            if (pObject->AsSWCPlaceable() != 0)
            {
                CSWCPlaceable* pPlaceable = pObject->AsSWCPlaceable();
                CSWSPlaceable* pServerPlaceable = pPlaceable->GetServerPlaceable();
                if (pPlaceable != 0)
                {
                    pPlaceable->m_bPlayedOutOfPlace = 0;
                    pPlaceable->SetAnimation(pServerPlaceable->m_nAnimation, 0, 0);
                    pPlaceable->m_pAnimBase->SetInCutscene(0);
                }
#ifdef _XBOX
                else
                {
                    EXOXBOXASSERT(0);
                }
#endif
            }
        }
        nAnimations = m_nLastDialogAnimations;
    }
    return nAnimations;
}
// PC 0x00683BA0
void CGuiInGame::ResetTalkAnimations()
{
    CSWCObject* pObject;

    pObject = g_pAppManager->m_pClientExoApp->GetGameObject(m_oidPreviousDialogObject1);
    if (pObject != 0 && pObject->AsSWCObject() != 0)
    {
        if (pObject->AsSWCCreature() != 0)
        {
            if (CanPlayDialogAnimations(pObject->AsSWCCreature()->GetId()))
            {
                if (pObject->AsSWCObject()->GetCurrentAnimation() != CREATURE_ANIMATION_COMBAT_IDLE)
                    pObject->AsSWCObject()->SetAnimation(CREATURE_ANIMATION_COMBAT_IDLE, 0, 1);
                CSWCObject* pCutsceneObject = pObject->AsSWCObject();
                pCutsceneObject->m_pAnimBase->SetInCutscene(0);
            }
        }
        else
        {
            if (pObject->AsSWCObject()->GetCurrentAnimation() != CREATURE_ANIMATION_COMBAT_IDLE)
                pObject->AsSWCObject()->SetAnimation(CREATURE_ANIMATION_COMBAT_IDLE, 0, 1);
            CSWCObject* pCutsceneObject = pObject->AsSWCObject();
            pCutsceneObject->m_pAnimBase->SetInCutscene(0);
        }
    }

    pObject = g_pAppManager->m_pClientExoApp->GetGameObject(m_oidPreviousDialogObject2);
    if (pObject != 0 && pObject->AsSWCObject() != 0)
    {
        if (pObject->AsSWCCreature() != 0)
        {
            if (CanPlayDialogAnimations(pObject->AsSWCCreature()->GetId()))
            {
                if (pObject->AsSWCObject()->GetCurrentAnimation() != CREATURE_ANIMATION_COMBAT_IDLE)
                    pObject->AsSWCObject()->SetAnimation(CREATURE_ANIMATION_COMBAT_IDLE, 0, 1);
                CSWCObject* pCutsceneObject = pObject->AsSWCObject();
                pCutsceneObject->m_pAnimBase->SetInCutscene(0);
            }
        }
        else
        {
            if (pObject->AsSWCObject()->GetCurrentAnimation() != CREATURE_ANIMATION_COMBAT_IDLE)
                pObject->AsSWCObject()->SetAnimation(CREATURE_ANIMATION_COMBAT_IDLE, 0, 1);
            CSWCObject* pCutsceneObject = pObject->AsSWCObject();
            pCutsceneObject->m_pAnimBase->SetInCutscene(0);
        }
    }
}

inline unsigned long CGuiInGame::GetDialogCamera()
{
    return m_oidDialogCamera;
}
// PC 0x006884E0
int CGuiInGame::HandleDialogEntry(
    unsigned long nPlayerId, unsigned long oidTarget,
    unsigned long oidSpeaker, unsigned long oidListener,
    unsigned long oidCamera, unsigned char nGender,
    const CExoString& sText, unsigned long nAnimations,
    unsigned long* pAnimationObjects, unsigned short* pAnimations,
    unsigned long nCameraAngle, int nCameraId, float fCameraHeight,
    float fTargetHeight, unsigned long oidCameraTarget,
    unsigned short nCameraAnimation, float fFieldOfView, int nVideoEffect,
    unsigned char nFadeType, const Vector& vFadeColor,
    float fFadeDelay, float fFadeLength, int bSoundActive)
{
    if (g_pAppManager->GetClientExoApp()->GetInGameGui()
            ->GetHoldWorldFadeInForDialog() != 0)
    {
        g_pAppManager->GetClientExoApp()->GetInGameGui()
            ->m_bHoldWorldFadeInForDialog = 0;
        Vector vBlack;
        CGuiInGame* pGui = g_pAppManager->GetClientExoApp()->GetInGameGui();
        if (pGui->m_pGlobalFade != NULL)
        {
            pGui->m_pGuiManager->AddPanel(pGui->m_pGlobalFade,
                                        SWGUI_PANEL_MODE_OVERLAY, 1);
            pGui->m_pGlobalFade->SetVisible(1);
            pGui->m_pGlobalFade->SetFade(1, 0.0f, 1.0f, vBlack);
        }
    }

    InitializeDialogAnimationData(nAnimations);
    for (unsigned long i = 0; i < nAnimations; ++i)
        SetDialogAnimationData(i, pAnimationObjects[i], pAnimations[i]);

    CExoString sMessage(sText);
    unsigned long oidPlayer =
        g_pAppManager->GetServerExoApp()->GetPlayerCreatureId();
    CSWSPlayer* pPlayer =
        g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(oidPlayer);
    unsigned long oidPC = pPlayer->m_oidPCObject;
    unsigned long oidClient =
        g_pAppManager->GetClientExoApp()->ServerToClientObjectId(oidPC);
    CSWCObject* pObject =
        g_pAppManager->GetClientExoApp()->GetGameObject(oidClient);
    g_pSWTlkTable->ParseStr(sMessage, (CGameObject*)pObject, nGender, 0);
    sMessage = DialogNodeSpanningTextParse(sMessage);
    AppendToDialogBuffer(sMessage, oidSpeaker);
    CleanUpReplyNode();

    if (!g_pAppManager->GetClientExoApp()->GetClientOptions()->m_bSubtitles)
    {
        if (g_bDisableSound)
            m_nDialogScrollWait = 1;
        else if (m_nC0 == 0)
        {
            if (m_pDialog != NULL && m_pDialog == m_pCameraDialog)
                m_nDialogScrollWait = bSoundActive;
            else
                m_nDialogScrollWait = 1;
        }
    }
    ShowDialogEntry(sMessage, oidTarget, oidSpeaker);
    UnlockDialogParticipantOrientations();

    if (GetDialogCamera() == OBJECT_INVALID)
    {
        m_oidDialogCamera = oidCamera;
        if (oidListener == OBJECT_INVALID)
        {
            m_oidPreviousDialogObject2 = m_oidDialogObject2;
            m_oidDialogObject2 = oidCamera;
        }
        else
        {
            m_oidPreviousDialogObject2 = m_oidDialogObject2;
            m_oidDialogObject2 = oidListener;
        }
    }
    else if (oidListener == OBJECT_INVALID)
    {
        if (oidSpeaker == m_oidDialogObject1)
        {
            m_oidPreviousDialogObject2 = m_oidDialogObject2;
            m_oidDialogObject2 = oidCamera;
        }
        else
        {
            m_oidPreviousDialogObject2 = m_oidDialogObject2;
            m_oidDialogObject2 = m_oidDialogObject1;
        }
    }
    else
    {
        m_oidPreviousDialogObject2 = m_oidDialogObject2;
        m_oidDialogObject2 = oidListener;
    }
    m_oidPreviousDialogObject1 = m_oidDialogObject1;
    m_oidDialogObject1 = oidSpeaker;
    SetDialogAnimations();

    if (nFadeType != DIALOG_FADE_NONE)
    {
        int bFadeIn = 1;
        if (nFadeType == DIALOG_FADE_OUT_ALTERNATE || nFadeType == DIALOG_FADE_OUT)
            bFadeIn = 0;
        if (m_pDialogFade != NULL)
            m_pDialogFade->SetFade(bFadeIn, fFadeDelay, fFadeLength, vFadeColor);
    }
    if (m_pDialog != NULL && m_pDialog == m_pCameraDialog)
    {
        if (SetAnimatedCamera(nCameraAnimation, fFieldOfView) == 0)
        {
            InitializeShotCamera();
            UpdateShotCamera(nCameraAngle, nCameraId, fCameraHeight,
                             fTargetHeight, oidCameraTarget, nVideoEffect);
        }
    }
    else if (nCameraAngle == DIALOG_SHOT_COMPUTER)
    {
        InitializeShotCamera();
        SetComputerCameraShot(nCameraId, oidCameraTarget, nVideoEffect);
    }
    return 1;
}

#ifndef _XBOX
void CGuiInGame::UpdateAreaTransitionGUI()  // PC 0x00684920
{
    if (m_bItemCreateMenuAvailable != 0 && m_pGuiManager != 0 &&
        m_pAreaTransition != 0)
    {
        CSWCModule* pModule;
        CSWSArea* pServerArea;
        if (g_pAppManager->m_pClientExoApp->GetModule() != 0 &&
            (pModule = g_pAppManager->m_pClientExoApp->GetModule())->m_pInfo != 0 &&
            (pServerArea = pModule->m_pInfo->GetServerArea()) != 0 &&
            m_bShowPauseGui == GUISTATE_MAIN_INTERFACE)
        {
            Vector vPosition = m_pAreaTransition->m_vTransitionPosition;
            if (m_pAreaTransition->m_oidTransitionObject == m_oidAreaTransition &&
                magnitude(vPosition - m_vAreaTransition) < 0.0001f)
                return;

            if (m_oidAreaTransition == OBJECT_INVALID)
                HideAreaTransition();
            else if (pServerArea->m_bTransitionPending != 0)
            {
                if (m_pGuiManager->PanelExists(m_pAreaTransition))
                    m_pGuiManager->SendPanelToBack(m_pAreaTransition);
            }
            else
            {
                m_pAreaTransition->SetTransitionObject(m_oidAreaTransition,
                                                       m_vAreaTransition);
                if (!m_pGuiManager->PanelExists(m_pAreaTransition))
                {
                    m_pGuiManager->AddPanel(m_pAreaTransition,
                        SWGUI_PANEL_MODE_OVERLAY, 1);
                    m_pGuiManager->SendPanelToBack(m_pAreaTransition);
                }
            }
        }
        else
            HideAreaTransition();
    }
}
#endif
// PC 0x00682FE0
void CGuiInGame::ShowDialogEntry(const CExoString& sMessage,
                                 unsigned long oidOwner, unsigned long nFlags)
{
    GUIINGAME_TRACE();
    if (m_nReplies <= 1u)
    {
        g_pAppManager->m_pClientExoApp->m_pcExoAppInternal->HideMouse(16);
    }
    if (m_bDialogEntryShown)
    {
        m_pDialog->Reset();
    }
    else
    {
        m_bDialogEntryShown = 1;
#ifdef _XBOX
        g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_DIALOG);
#else
        g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_DIALOG, 1);
#endif
        g_pAppManager->m_pClientExoApp->ResetDriveAcceleration();
        m_pBarkBubble->StopBark(1);
    }
    m_pDialog->SetOwner(oidOwner);
    if (!m_pGuiManager->PanelExists(m_pDialog))
    {
        int nPanelType = 0;
        if (m_pDialog == m_pCameraDialog)
        {
            if (bDisableLetterbox == 0)
            {
                m_pDialogFade->SetMiddle();
                m_pGuiManager->AddPanel(m_pDialogFade, 0, 1);
                m_pDialogTop->SetTop();
                m_pGuiManager->AddPanel(m_pDialogTop, 0, 1);
                m_pDialogLetterbox->SetBottom();
                m_pGuiManager->AddPanel(m_pDialogLetterbox, 0, 1);
            }
        }
        else if (m_pDialog == m_pComputerDialog)
        {
            nPanelType = 2;
        }
        m_pGuiManager->AddPanel(m_pDialog, nPanelType, 1);
        SetSWGuiStatus(GUISTATUS_HIDE_MAIN_INTERFACE, 1);
    }
    m_pDialog->SetDialogMessage(sMessage);
}
// PC 0x0067CDB0
int CGuiInGame::InitializeStuntModelData(unsigned long nModels)
{
    GUIINGAME_TRACE();
    int nResult = 0;
    if (nModels != 0)
    {
        if (m_pnStuntModelIDs != 0)
        {
            delete[] m_pnStuntModelIDs;
            m_pnStuntModelIDs = 0;
        }
        if (m_pStuntModelResRefs != 0)
        {
            delete[] m_pStuntModelResRefs;
            m_pStuntModelResRefs = 0;
        }
        m_nStuntModels = nModels;
        m_pnStuntModelIDs = new unsigned long[nModels];
        m_pStuntModelResRefs = new CResRef[m_nStuntModels];
        if (m_pnStuntModelIDs != 0 && m_pStuntModelResRefs != 0)
        {
            nResult = 1;
        }
        else
        {
            delete[] m_pnStuntModelIDs;
            m_pnStuntModelIDs = 0;
            delete[] m_pStuntModelResRefs;
            m_pStuntModelResRefs = 0;
            m_nStuntModels = 0;
            nResult = 0;
        }
    }
    return nResult;
}
// PC 0x00683100
int CGuiInGame::StoreLastDialogAnimationData()
{
    GUIINGAME_TRACE();
    int i;

    ClearLastDialogAnimationData();
    m_nLastDialogAnimations = m_nDialogAnimations;
    if (m_nLastDialogAnimations != 0)
    {
        m_pLastDialogAnimations = new unsigned long[m_nDialogAnimations];
        m_pLastDialogAnimationFlags = new unsigned short[m_nDialogAnimations];
    }
    for (i = 0; i < m_nDialogAnimations; i++)
    {
        m_pLastDialogAnimations[i] = m_pDialogAnimations[i];
        m_pLastDialogAnimationFlags[i] = m_pDialogAnimationFlags[i];
    }
    return i;
}
// PC 0x006831D0
void CGuiInGame::ClearRepliesData()
{
    GUIINGAME_TRACE();
    if (m_asReplyText != 0)
    {
        delete[] m_asReplyText;
        m_asReplyText = 0;
    }
    if (m_pnReplyValue1 != 0)
    {
        delete m_pnReplyValue1;
        m_pnReplyValue1 = 0;
    }
    if (m_pnReplyValue2 != 0)
    {
        delete m_pnReplyValue2;
        m_pnReplyValue2 = 0;
    }
    if (m_pnReplyValue3 != 0)
    {
        delete m_pnReplyValue3;
        m_pnReplyValue3 = 0;
    }
    if (m_pnReplyValue4 != 0)
    {
        delete m_pnReplyValue4;
        m_pnReplyValue4 = 0;
    }
    if (m_pfReplyValue5 != 0)
    {
        delete m_pfReplyValue5;
        m_pfReplyValue5 = 0;
    }
    if (m_pfReplyValue6 != 0)
    {
        delete m_pfReplyValue6;
        m_pfReplyValue6 = 0;
    }
    if (m_pnReplyValue7 != 0)
    {
        delete m_pnReplyValue7;
        m_pnReplyValue7 = 0;
    }
    if (m_pnReplyValue8 != 0)
    {
        delete m_pnReplyValue8;
        m_pnReplyValue8 = 0;
    }
    if (m_pfReplyValue9 != 0)
    {
        delete m_pfReplyValue9;
        m_pfReplyValue9 = 0;
    }
    if (m_pnReplyValue10 != 0)
    {
        delete m_pnReplyValue10;
        m_pnReplyValue10 = 0;
    }
    if (m_pnReplyValue11 != 0)
    {
        delete m_pnReplyValue11;
        m_pnReplyValue11 = 0;
    }
    if (m_pvReplyValue12 != 0)
    {
        delete m_pvReplyValue12;
        m_pvReplyValue12 = 0;
    }
    if (m_pfReplyValue13 != 0)
    {
        delete m_pfReplyValue13;
        m_pfReplyValue13 = 0;
    }
    if (m_pfReplyValue14 != 0)
    {
        delete m_pfReplyValue14;
        m_pfReplyValue14 = 0;
    }
    for (int i = 0; i < m_nReplies; i++)
    {
        if (m_pnReplyAnimations[i] != 0)
        {
            unsigned long* pnIDs = m_ppnReplyAnimationIDs[i];
            unsigned short* pnFlags = m_ppnReplyAnimationFlags[i];
            if (pnIDs != 0)
            {
                delete pnIDs;
            }
            if (pnFlags != 0)
            {
                delete pnFlags;
            }
        }
    }
    if (m_ppnReplyAnimationIDs != 0)
    {
        delete m_ppnReplyAnimationIDs;
        m_ppnReplyAnimationIDs = 0;
    }
    if (m_ppnReplyAnimationFlags != 0)
    {
        delete m_ppnReplyAnimationFlags;
        m_ppnReplyAnimationFlags = 0;
    }
    if (m_pnReplyAnimations != 0)
    {
        delete m_pnReplyAnimations;
        m_pnReplyAnimations = 0;
    }
    m_nReplies = 0;
}
// PC 0x00683410
int CGuiInGame::InitializeRepliesData(unsigned long nReplies)
{
    int nResult = 0;
    ClearRepliesData();
    m_nReplies = nReplies;
    m_asReplyText = new CExoString[nReplies];
    m_pnReplyValue1 = new unsigned long[m_nReplies];
    m_pnReplyValue2 = new unsigned long[m_nReplies];
    m_pnReplyValue3 = new unsigned long[m_nReplies];
    m_pnReplyValue4 = new int[m_nReplies];
    m_pfReplyValue5 = new float[m_nReplies];
    m_pfReplyValue6 = new float[m_nReplies];
    m_pnReplyValue7 = new unsigned long[m_nReplies];
    m_pnReplyValue8 = new unsigned short[m_nReplies];
    m_pfReplyValue9 = new float[m_nReplies];
    m_pnReplyValue10 = new int[m_nReplies];
    m_pnReplyValue11 = new unsigned char[m_nReplies];
    m_pvReplyValue12 = new Vector[m_nReplies];
    m_pfReplyValue13 = new float[m_nReplies];
    m_pfReplyValue14 = new float[m_nReplies];
    m_pnReplyAnimations = new int[m_nReplies];
    m_ppnReplyAnimationIDs = new unsigned long*[m_nReplies];
    m_ppnReplyAnimationFlags = new unsigned short*[m_nReplies];
    if (m_asReplyText != 0 &&
        m_pnReplyValue1 != 0 &&
        m_pnReplyValue2 != 0 &&
        m_pnReplyValue3 != 0 &&
        m_pnReplyValue4 != 0 &&
        m_pfReplyValue5 != 0 &&
        m_pfReplyValue6 != 0 &&
        m_pnReplyValue7 != 0 &&
        m_pnReplyValue8 != 0 &&
        m_pfReplyValue9 != 0 &&
        m_pnReplyValue10 != 0 &&
        m_pnReplyValue11 != 0 &&
        m_pvReplyValue12 != 0 &&
        m_pfReplyValue13 != 0 &&
        m_pfReplyValue14 != 0 &&
        m_pnReplyAnimations != 0 &&
        *m_ppnReplyAnimationIDs != 0 &&
        *m_ppnReplyAnimationFlags != 0)
    {
        nResult = 1;
    }
    return nResult;
}
// PC 0x0067F060
int CGuiInGame::InitializeShotCamera()
{
    GUIINGAME_TRACE();
    if (m_bShotCameraInitialized == 0)
    {
        CSWCModule* pModule = g_pAppManager->m_pClientExoApp->GetModule();
        if (pModule != 0)
        {
            int nDialogCameraType = m_nDialogCameraType;
            pModule->SetDialogCamera(nDialogCameraType);
        }
        m_bShotCameraInitialized = 1;
    }
    return m_bShotCameraInitialized;
}
#ifndef _XBOX
int CGuiInGame::SetAnimatedCamera(unsigned short nAnimation, float fFieldOfView)  // PC 0x006836D0
{
    GUIINGAME_TRACE();
    if (m_bAnimatedCameraLoaded == 0 ||
        nAnimation < CUTSCENE_ANIMATION_FIRST ||
        nAnimation > CUTSCENE_ANIMATION_WL_LAST)
    {
        return 0;
    }

    CSWCModule* pModule = g_pAppManager->m_pClientExoApp->GetModule();
    if (pModule == 0)
    {
        EXOXBOXASSERT(0);
        return 0;
    }

    unsigned long oidCamera = m_oidCameraAnimationObject;
    m_bAnimatedCameraActive = 1;
    m_bShotCameraInitialized = 0;
    m_nDialogCameraAnimation = nAnimation;
    CGameObject* pObject = g_pAppManager->m_pServerExoApp->GetGameObject(oidCamera);
    if (pObject != 0 && pObject->AsSWSObject() != 0)
    {
        pObject->AsSWSObject()->TimeStampCameraAnimation();
    }
    unsigned long oidTarget = m_oidDialogObject1;
    m_fAnimatedCameraTime = pModule->SetAnimatedCamera(
        nAnimation, fFieldOfView, oidTarget);
    return 1;
}
#endif
// PC 0x0067F160
void CGuiInGame::SetComputerCameraShot(int nCamera, unsigned long oidTarget,
                                     int nVideoEffect)
{
    GUIINGAME_TRACE();
    if (m_nPlaceableCameras != 0)
    {
        m_pGuiManager->RemovePanel(m_pDialog);
        m_pGuiManager->AddPanel(m_pComputerScreen, 0, 1);
        m_pComputerScreen->SetOwner(m_pDialog->m_nOwner);
        CSWBehaviorCameraDialog* pCamera = reinterpret_cast<CSWBehaviorCameraDialog*>(
            g_pAppManager->m_pClientExoApp->GetModule()->GetCamera()->
                GetBehavior(AUR_BEHAVIOR_CAMERA_DIALOG));
        if (pCamera != 0)
        {
            int bFound = 0;
            int nIndex;
            for (nIndex = 0; nIndex < m_nPlaceableCameras; nIndex++)
            {
                if (m_aPlaceableCameras[nIndex].m_nPlaceableID == nCamera)
                {
                    bFound = 1;
                    break;
                }
            }
            Vector vPosition;
            Quaternion qOrientation;
            float fFieldOfView = GUI_COMPUTER_CAMERA_DEFAULT_FOV;
            float fMicRange = 0.0f;
            if (bFound)
            {
                vPosition = m_aPlaceableCameras[nIndex].m_vPosition;
                vPosition.m_fZ += m_aPlaceableCameras[nIndex].m_fHeight;
                qOrientation = m_aPlaceableCameras[nIndex].m_qOrientation;
                qOrientation = YawPitchRoll(Yaw(qOrientation),
                    Pitch(qOrientation) + m_aPlaceableCameras[nIndex].m_fPitch,
                    Roll(m_aPlaceableCameras[nIndex].m_qOrientation));
                fFieldOfView = m_aPlaceableCameras[nIndex].m_fFieldOfView;
                fMicRange = m_aPlaceableCameras[nIndex].m_fMicRange;
            }
            pCamera->SetShot(CSWBehaviorCameraDialog::FIXED_CAMERA_SHOT,
                vPosition, qOrientation, fFieldOfView);
            m_bComputerCameraShot = 1;
            m_vComputerCameraShot = vPosition;
            m_fComputerCameraMicRange = fMicRange;
            if (nVideoEffect == CAMERA_VIDEO_EFFECT_DEFAULT)
                nVideoEffect = 0;
            if (nVideoEffect != CAMERA_VIDEO_EFFECT_UNCHANGED)
                g_pAppManager->m_pClientExoApp->EnableVideoEffect(nVideoEffect);
        }
        m_oidComputerCameraTarget = oidTarget;
    }
}
// PC 0x0067F480
void CGuiInGame::EndDialogCamera()
{
    GUIINGAME_TRACE();
    CSWCModule* pModule = g_pAppManager->m_pClientExoApp->GetModule();
    pModule->RestoreCamera();
    m_bAnimatedCameraActive = 0;
    m_fAnimatedCameraTime = 0.0f;
    m_bShotCameraInitialized = 0;
    m_bAnimatedCameraLoaded = 0;
    m_nDialogCameraState = 0;
    m_nDialogCameraAnimation = CREATURE_ANIMATION_NO_IDLE_FALLBACK;
    m_oidDialogCamera = OBJECT_INVALID;
    m_oidDialogObject1 = OBJECT_INVALID;
    m_oidDialogObject2 = OBJECT_INVALID;
    m_oidPreviousDialogObject1 = OBJECT_INVALID;
    m_oidPreviousDialogObject2 = OBJECT_INVALID;
}
// PC 0x00683790
void CGuiInGame::ShutDown()
{
    if (m_bItemCreateMenuAvailable != 0)
    {
        m_pGuiManager = 0;
        if (m_pBarkBubble != 0)
        {
            delete m_pBarkBubble;
            m_pBarkBubble = 0;
        }
        if (m_pExamineBox != 0)
        {
            delete m_pExamineBox;
            m_pExamineBox = 0;
        }
        if (m_pContainer != 0)
        {
            delete m_pContainer;
            m_pContainer = 0;
        }
        if (m_pItemCreateMenu != 0)
        {
            delete m_pItemCreateMenu;
            m_pItemCreateMenu = 0;
        }
        if (m_pLoadModuleDebugMenu != 0)
        {
            delete m_pLoadModuleDebugMenu;
            m_pLoadModuleDebugMenu = 0;
        }
        if (m_pPowersFeatsSkillsDebugMenu != 0)
        {
            delete m_pPowersFeatsSkillsDebugMenu;
            m_pPowersFeatsSkillsDebugMenu = 0;
        }
        if (m_pSubItemCreateMenu != 0)
        {
            delete m_pSubItemCreateMenu;
            m_pSubItemCreateMenu = 0;
        }
        if (m_pInGameMenu != 0)
        {
            delete m_pInGameMenu;
            m_pInGameMenu = 0;
        }
        if (m_pDialogTop != 0)
        {
            delete m_pDialogTop;
            m_pDialogTop = 0;
        }
        if (m_pDialogLetterbox != 0)
        {
            delete m_pDialogLetterbox;
            m_pDialogLetterbox = 0;
        }
        if (m_pDialogFade != 0)
        {
            delete m_pDialogFade;
            m_pDialogFade = 0;
        }
        if (m_pCameraDialog != 0)
        {
            delete m_pCameraDialog;
            m_pCameraDialog = 0;
        }
        if (m_pComputerDialog != 0)
        {
            delete m_pComputerDialog;
            m_pComputerDialog = 0;
        }
        if (m_pComputerScreen != 0)
        {
            delete m_pComputerScreen;
            m_pComputerScreen = 0;
        }
        if (m_pGlobalFade != 0)
        {
            delete m_pGlobalFade;
            m_pGlobalFade = 0;
        }
        if (m_pPartySelectionPanel != 0)
        {
            delete m_pPartySelectionPanel;
            m_pPartySelectionPanel = 0;
        }
        if (m_pPausePanel != 0)
        {
            delete m_pPausePanel;
            m_pPausePanel = 0;
        }
        if (m_pSoloModeQuery != 0)
        {
            delete m_pSoloModeQuery;
            m_pSoloModeQuery = 0;
        }
        if (m_pAreaTransition != 0)
        {
            delete m_pAreaTransition;
            m_pAreaTransition = 0;
        }
        if (m_pStatusSummary != 0)
        {
            delete m_pStatusSummary;
            m_pStatusSummary = 0;
        }
        if (m_pPendingStatusSummary != 0)
        {
            delete m_pPendingStatusSummary;
            m_pPendingStatusSummary = 0;
        }
        if (m_pStorePanel != 0)
        {
            delete m_pStorePanel;
            m_pStorePanel = 0;
        }
        if (m_pMainInterface != 0)
        {
            delete m_pMainInterface;
            m_pMainInterface = 0;
        }
        for (int nScreen = 0; nScreen < IN_GAME_SCREEN_COUNT; ++nScreen)
        {
            if (m_apScreenPanels[nScreen] != 0)
            {
                delete m_apScreenPanels[nScreen];
                m_apScreenPanels[nScreen] = 0;
            }
        }
        if (m_pGalaxyMap != 0)
        {
            delete m_pGalaxyMap;
            m_pGalaxyMap = 0;
        }
        if (m_pnStuntModelIDs != 0)
        {
            delete [] m_pnStuntModelIDs;
            m_pnStuntModelIDs = 0;
        }
        if (m_pStuntModelResRefs != 0)
        {
            delete [] m_pStuntModelResRefs;
            m_pStuntModelResRefs = 0;
        }
        if (m_pDialogAnimations != 0)
        {
            delete [] m_pDialogAnimations;
            m_pDialogAnimations = 0;
        }
        if (m_pDialogAnimationFlags != 0)
        {
            delete [] m_pDialogAnimationFlags;
            m_pDialogAnimationFlags = 0;
        }
        if (m_pLastDialogAnimations != 0)
        {
            delete [] m_pDialogAnimations;
            m_pDialogAnimations = 0;
        }
        if (m_pLastDialogAnimationFlags != 0)
        {
            delete [] m_pLastDialogAnimationFlags;
            m_pLastDialogAnimationFlags = 0;
        }
        m_nStuntModels = 0;
        ClearRepliesData();
        ClearDialogAnimationData();
        ClearLastDialogAnimationData();
        m_bPaused = 0;
#ifndef _XBOX
        m_nPauseState = 0;
#endif
        m_bItemCreateMenuAvailable = 0;
    }
}
// PC 0x0067F5A0
int CGuiInGame::LoadStuntModelsForDialog()
{
    GUIINGAME_TRACE();
    char sModel[17];
    for (unsigned long nModel = 0; nModel < m_nStuntModels; ++nModel)
    {
        CClientExoApp* pClientApp = g_pAppManager->m_pClientExoApp;
        CSWCObject* pObject = pClientApp->GetGameObject(
            m_pnStuntModelIDs[nModel]);
        if (pObject != 0)
        {
            CSWCObject* pClientObject = pObject->AsSWCObject();
            if (pClientObject != 0)
            {
                m_pStuntModelResRefs[nModel].CopyToString(sModel);
                if (sModel[0] != 0)
                {
                    Gob* pModel = pClientObject->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 0);
                    if (pModel == 0)
                        return 0;
                    pModel->LoadAddInAnimations(sModel);
                    pModel = pClientObject->GetModel(CSWCAnimBase::MODEL_SLOT_HEAD, 0);
                    if (pModel != 0)
                        pModel->LoadAddInAnimations(sModel);
                }
            }
        }
    }
    return 1;
}
// PC 0x0067F670
void CGuiInGame::UnloadStuntModelsForDialog()
{
    GUIINGAME_TRACE();
    for (unsigned long nModel = 0; nModel < m_nStuntModels; ++nModel)
    {
        CSWCObject* pObject = g_pAppManager->GetClientExoApp()->GetGameObject(
            m_pnStuntModelIDs[nModel]);
        if (pObject != 0)
        {
            CSWCObject* pClientObject = pObject->AsSWCObject();
            if (pClientObject != 0)
            {
                Gob* pModel = pClientObject->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 0);
                if (pModel != 0)
                {
                    pModel->PlayAnimation(0, 1.0f, 0, 0.0f);
                    pModel->RemoveAddInAnimations();
                }
                Gob* pHeadModel = pClientObject->GetModel(CSWCAnimBase::MODEL_SLOT_HEAD, 0);
                if (pHeadModel != 0)
                {
                    pHeadModel->PlayAnimation(0, 1.0f, 0, 0.0f);
                    pHeadModel->RemoveAddInAnimations();
                }
                CExoString sAnimation;
                unsigned short nAnimation;
                if (pClientObject->AsSWCCreature() != 0)
                {
                    sAnimation = pClientObject->GetAnimationBase()->GetAnimationName(
                        CREATURE_ANIMATION_COMBAT_IDLE);
                    nAnimation = CREATURE_ANIMATION_COMBAT_IDLE;
                }
                if (pClientObject->AsSWCPlaceable() != 0)
                {
                    CSWSPlaceable* pServerPlaceable = pClientObject->AsSWCPlaceable()->GetServerPlaceable();
                    if (pServerPlaceable != 0)
                    {
                        nAnimation = pServerPlaceable->GetAnimation();
                        sAnimation = pClientObject->AsSWCPlaceable()->GetAnimationBase()->GetAnimationName(nAnimation);
                    }
                    else
                    {
                        sAnimation = pClientObject->AsSWCPlaceable()->GetAnimationBase()->GetAnimationName(
                            PLACEABLE_ANIMATION_BASE);
                        nAnimation = PLACEABLE_ANIMATION_BASE;
                    }
                }
                if (pModel != 0)
                    pModel->PlayAnimation(sAnimation.CStr(), 1.0f, Gob::ANIMATION_FLAG_NO_BLEND, 0.0f);
                if (pHeadModel != 0)
                    pHeadModel->PlayAnimation(sAnimation.CStr(), 1.0f, Gob::ANIMATION_FLAG_NO_BLEND, 0.0f);
                if (pClientObject->AsSWCCreature() != 0)
                    pClientObject->SetAnimation(nAnimation, 0, 0);
                if (pClientObject->AsSWCPlaceable() != 0)
                    pClientObject->AsSWCPlaceable()->SetAnimation(nAnimation, 0, 0);
            }
        }
    }
    if (m_pnStuntModelIDs != 0)
    {
        delete [] m_pnStuntModelIDs;
        m_pnStuntModelIDs = 0;
    }
    if (m_pStuntModelResRefs != 0)
    {
        delete [] m_pStuntModelResRefs;
        m_pStuntModelResRefs = 0;
    }
    m_nStuntModels = 0;
}
// PC 0x00683D10
#ifdef _XBOX
int CGuiInGame::ShowSWInGameGui()
#else
int CGuiInGame::ShowSWInGameGui(int nMenu)
#endif
{
    GUIINGAME_TRACE();
    if (m_bItemCreateMenuAvailable != 0)
    {
        CSWGuiManager* pGuiManager = g_pAppManager->m_pClientExoApp->GetSWGuiManager();
        if (pGuiManager != 0)
        {
            g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_PAUSE_ALL_BUT_MUSIC);
            g_pAppManager->m_pClientExoApp->IndicateObject(0, 0);
            SetSWGuiStatus(GUISTATUS_HIDE_MAIN_INTERFACE, 1);
            CExoString sOpenScript("k_sup_guiopen");
            g_pVirtualMachine->RunScript(&sOpenScript, 0, 0);

#ifdef _XBOX
            if (g_pAppManager->m_pClientExoApp->GetSWParty()->GetPlayerCharacter() != 0)
#else
            if (nMenu >= 0 && nMenu < IN_GAME_SCREEN_COUNT)
                m_nLastGuiPanel = nMenu;
            else if (g_pAppManager->m_pClientExoApp->GetSWParty()->GetPlayerCharacter() != 0)
#endif
            {
                if (g_pAppManager->m_pClientExoApp->GetSWParty()->GetPlayerCharacter()->
                    GetServerCreature()->m_pStats->CanLevelUp() != 0)
                    m_nLastGuiPanel = SCREEN_CHARACTER;
            }

            UpdateCreatedInGameGUI(SCREEN_NONE, m_nLastGuiPanel);

            pGuiManager->AddPanel(m_pInGameMenu, 0, 1);
            pGuiManager->AddPanel(m_apScreenPanels[m_nLastGuiPanel],
                                 CSWGuiManager::PANEL_MODE_FOREGROUND, 1);
            pGuiManager->SendPanelToBack(m_apScreenPanels[m_nLastGuiPanel]);
            m_pInGameMenu->SetActiveControlID(m_nLastGuiPanel, 0);
            m_pPartySelection = 1;
            if (m_bPaused == 0)
                g_pAppManager->m_pClientExoApp->GetSWCMessage()->
                    SendPlayerToServerInput_TogglePauseRequest();
            m_pBarkBubble->Pause();
            if (g_pVirtualMachine != 0)
            {
                CExoString sScreenScript("k_pend_screenchg");
                g_pVirtualMachine->RunScript(&sScreenScript, OBJECT_INVALID, 0);
            }

            m_pGuiManager->PlayGuiSound(CSWGuiManager::GUISOUND_PAZAAK_START);
        }
    }
    return m_pPartySelection;
}

#ifdef _XBOX
int CGuiInGame::HideSWInGameGui()
{
    GUIINGAME_TRACE();
    if (m_bItemCreateMenuAvailable != 0)
    {
        CSWGuiManager* pGuiManager = g_pAppManager->m_pClientExoApp->GetSWGuiManager();
        if (pGuiManager != 0 && pGuiManager->m_lModalPanels.num == 0)
        {
            SetSWGuiStatus(GUISTATUS_RESTORE_MAIN_INTERFACE, 1);
            pGuiManager->RemovePanel(m_pInGameMenu);
            if (g_bCreateAllGUIs != 0)
                pGuiManager->RemovePanel(m_apScreenPanels[m_nLastGuiPanel]);

            int nOldScreen = m_nLastGuiPanel;
            m_pPartySelection = 0;
            UpdateCreatedInGameGUI(nOldScreen, SCREEN_NONE);
            if (m_bPaused != 0)
            {
                if (g_bInScreenshotMode == 0 && !m_pGuiManager->PanelExists(m_pPausePanel))
                {
                    m_pGuiManager->AddPanel(m_pPausePanel, SWGUI_PANEL_MODE_OVERLAY, 1);
                    m_pGuiManager->SendPanelToBack(m_pPausePanel);
                }
            }
            else
            {
                g_pAppManager->m_pClientExoApp->GetSWCMessage()
                    ->SendPlayerToServerInput_TogglePauseRequest();
            }

            m_pBarkBubble->Resume();
            m_pGuiManager->PlayGuiSound(5);
            g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_GENERIC);
            return 1;
        }
    }
    return 0;
}
#else
int CGuiInGame::HideSWInGameGui(int bHidePause)  // PC 0x00683F10
{
    GUIINGAME_TRACE();
    if (m_bItemCreateMenuAvailable != 0)
    {
        CSWGuiManager* pGuiManager = g_pAppManager->m_pClientExoApp->GetSWGuiManager();
        if (pGuiManager != 0 && pGuiManager->m_lModalPanels.num == 0)
        {
            SetSWGuiStatus(GUISTATUS_RESTORE_MAIN_INTERFACE, 1);
            pGuiManager->RemovePanel(m_pInGameMenu);
            if (g_bCreateAllGUIs != 0)
                pGuiManager->RemovePanel(m_apScreenPanels[m_nLastGuiPanel]);

            int nOldScreen = m_nLastGuiPanel;
            m_pPartySelection = 0;
            UpdateCreatedInGameGUI(nOldScreen, SCREEN_NONE);
            if (m_bPaused != 0)
            {
                if (bHidePause == 0)
                {
                    if (g_bInScreenshotMode == 0 && !m_pGuiManager->PanelExists(m_pPausePanel))
                    {
                        m_pGuiManager->AddPanel(m_pPausePanel, SWGUI_PANEL_MODE_OVERLAY, 1);
                        m_pGuiManager->SendPanelToBack(m_pPausePanel);
                    }
                }
            }
            else
            {
                g_pAppManager->m_pClientExoApp->GetSWCMessage()
                    ->SendPlayerToServerInput_TogglePauseRequest();
            }
            if (bHidePause != 0 && m_pGuiManager->PanelExists(m_pPausePanel))
                m_pPausePanel->SetModal(0);

            m_pBarkBubble->Resume();

            m_pGuiManager->PlayGuiSound(5);
            g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_GENERIC);
            return 1;
        }
    }
    return 0;
}
#endif
// PC 0x00684070
void CGuiInGame::NextSWInGameGui()
{
    GUIINGAME_TRACE();
    if (m_bItemCreateMenuAvailable != 0)
    {
        CSWGuiManager* pGuiManager = g_pAppManager->m_pClientExoApp->GetSWGuiManager();
        if (g_bCreateAllGUIs != 0 && m_pPartySelection != 0 && pGuiManager != 0)
            pGuiManager->RemovePanel(m_apScreenPanels[m_nLastGuiPanel]);

        int nOldScreen = m_nLastGuiPanel;
        ++m_nLastGuiPanel;
        if (m_nLastGuiPanel >= IN_GAME_SCREEN_COUNT)
            m_nLastGuiPanel = SCREEN_EQUIP;
        UpdateCreatedInGameGUI(nOldScreen,
            m_pPartySelection != 0 && pGuiManager != 0 ? m_nLastGuiPanel : SCREEN_NONE);

        if (m_pPartySelection != 0 && pGuiManager != 0)
        {
            pGuiManager->AddPanel(m_apScreenPanels[m_nLastGuiPanel],
                                 CSWGuiManager::PANEL_MODE_FOREGROUND, 1);
            pGuiManager->SendPanelToBack(m_apScreenPanels[m_nLastGuiPanel]);
            m_pInGameMenu->SetActiveControlID(m_nLastGuiPanel, 1);
            if (g_pVirtualMachine != 0)
            {
                CExoString sScript("k_pend_screenchg");
                g_pVirtualMachine->RunScript(&sScript, OBJECT_INVALID, 0);
            }
        }
    }
}
// PC 0x00684190
void CGuiInGame::PrevSWInGameGui()
{
    GUIINGAME_TRACE();
    if (m_bItemCreateMenuAvailable != 0)
    {
        CSWGuiManager* pGuiManager = g_pAppManager->m_pClientExoApp->GetSWGuiManager();
        if (g_bCreateAllGUIs != 0 && m_pPartySelection != 0 && pGuiManager != 0)
            pGuiManager->RemovePanel(m_apScreenPanels[m_nLastGuiPanel]);

        int nOldScreen = m_nLastGuiPanel;
        --m_nLastGuiPanel;
        if (m_nLastGuiPanel < 0)
            m_nLastGuiPanel = IN_GAME_SCREEN_COUNT - 1;
        UpdateCreatedInGameGUI(nOldScreen,
            m_pPartySelection != 0 && pGuiManager != 0 ? m_nLastGuiPanel : SCREEN_NONE);

        if (m_pPartySelection != 0 && pGuiManager != 0)
        {
            pGuiManager->AddPanel(m_apScreenPanels[m_nLastGuiPanel],
                                 CSWGuiManager::PANEL_MODE_FOREGROUND, 1);
            pGuiManager->SendPanelToBack(m_apScreenPanels[m_nLastGuiPanel]);
            m_pInGameMenu->SetActiveControlID(m_nLastGuiPanel, 1);
            if (g_pVirtualMachine != 0)
            {
                CExoString sScript("k_pend_screenchg");
                g_pVirtualMachine->RunScript(&sScript, OBJECT_INVALID, 0);
            }
        }
    }
}
// PC 0x006842B0
void CGuiInGame::SwitchToSWInGameGui(int nMenu)
{
    GUIINGAME_TRACE();
    if (nMenu >= 0 && nMenu < IN_GAME_SCREEN_COUNT &&
        nMenu != m_nLastGuiPanel && m_bItemCreateMenuAvailable != 0)
    {
        CSWGuiManager* pGuiManager = g_pAppManager->m_pClientExoApp->GetSWGuiManager();
        if (g_bCreateAllGUIs != 0 && m_pPartySelection != 0 && pGuiManager != 0)
            pGuiManager->RemovePanel(m_apScreenPanels[m_nLastGuiPanel]);

        UpdateCreatedInGameGUI(m_nLastGuiPanel,
            m_pPartySelection != 0 && pGuiManager != 0 ? nMenu : SCREEN_NONE);
        m_nLastGuiPanel = nMenu;

        if (m_pPartySelection != 0 && pGuiManager != 0)
        {
            pGuiManager->AddPanel(m_apScreenPanels[m_nLastGuiPanel],
                                 CSWGuiManager::PANEL_MODE_FOREGROUND, 1);
            pGuiManager->SendPanelToBack(m_apScreenPanels[m_nLastGuiPanel]);
            m_pInGameMenu->SetActiveControlID(m_nLastGuiPanel, 1);
            if (g_pVirtualMachine != 0)
            {
                CExoString sScript("k_pend_screenchg");
                g_pVirtualMachine->RunScript(&sScript, OBJECT_INVALID, 0);
            }
        }
    }
}
// PC 0x0067FAA0
void CGuiInGame::ShowGalaxyMapGui(int nPlanet)
{
    GUIINGAME_TRACE();
    if (m_bItemCreateMenuAvailable != 0 && m_pPartySelection == 0 &&
        g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nGlobalDialogState == 0)
    {
        if (g_pAppManager->m_pClientExoApp->GetPausedByCombat() == 0)
            g_pAppManager->m_pClientExoApp->SetPausedByCombat(1, 0, 0);
        g_pAppManager->m_pClientExoApp->IndicateObject(0, 0);
        g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_PAUSE_ALL_BUT_MUSIC);
        SetSWGuiStatus(GUISTATUS_HIDE_MAIN_INTERFACE, 1);
        m_pBarkBubble->SetBark(OBJECT_INVALID, "", "", 0);
#ifdef _XBOX
        g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GUI);
#else
        g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GUI, 1);
#endif
        if (m_pGalaxyMap == 0)
            m_pGalaxyMap = new CSWGuiInGameGalaxyMap(m_pGuiManager);
        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
        pServer->GetPartyTable()->SetSelectedPlanet(nPlanet);
        m_pGuiManager->AddPanel(m_pGalaxyMap, CSWGuiManager::PANEL_MODE_FOREGROUND, 1);

        m_pGuiManager->PlayGuiSound(CSWGuiManager::GUISOUND_PAZAAK_START);
    }
}
// PC 0x0067FC30
void CGuiInGame::HideGalaxyMapGui()
{
    GUIINGAME_TRACE();
    if (m_bItemCreateMenuAvailable != 0)
    {
        if (g_pAppManager->m_pClientExoApp->GetPausedByCombat() != 0)
        {
            g_pAppManager->m_pClientExoApp->SetPausedByCombat(0, 0, 0);
        }
        SetSWGuiStatus(GUISTATUS_RESTORE_MAIN_INTERFACE, 1);
#ifdef _XBOX
        g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME);
#else
        g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME, 1);
#endif
        m_pGuiManager->RemovePanel(m_pGalaxyMap);
        if (g_bCreateAllGUIs == 0)
        {
            delete m_pGalaxyMap;
            m_pGalaxyMap = 0;
        }

        m_pGuiManager->PlayGuiSound(5);
        g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_GENERIC);
    }
}
// PC 0x0067FCE0
void CGuiInGame::ShowItemCreateMenu()
{
    GUIINGAME_TRACE();
    if (m_bItemCreateMenuAvailable != 0)
    {
        m_pGuiManager->AddPanel(m_pItemCreateMenu, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
#ifdef _XBOX
        g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GUI);
#else
        g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GUI, 1);
#endif
    }
}
// PC 0x0067FD10
void CGuiInGame::HideItemCreateMenu()
{
    GUIINGAME_TRACE();
    if (m_bItemCreateMenuAvailable != 0)
    {
        if (m_pGuiManager->PanelExists(m_pItemCreateMenu) != 0)
        {
            m_pGuiManager->PopModalPanel();
            m_pGuiManager->RemovePanel(m_pItemCreateMenu);
            if (g_pAppManager->m_pClientExoApp->GetMiniGame() != 0)
            {
#ifdef _XBOX
                g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_MINIGAME);
#else
                g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_MINIGAME, 1);
#endif
            }
            else
            {
#ifdef _XBOX
                g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME);
#else
                g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME, 1);
#endif
            }
        }
    }
}
// PC 0x0067FD80
void CGuiInGame::ShowSubItemCreateMenu(unsigned long nSubType)
{
    GUIINGAME_TRACE();
    if (m_bItemCreateMenuAvailable != 0)
    {
        m_pSubItemCreateMenu->LoadItems(nSubType);
        m_pGuiManager->AddPanel(m_pSubItemCreateMenu, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
#ifdef _XBOX
        g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GUI);
#else
        g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GUI, 1);
#endif
    }
}
// PC 0x0067FDC0
void CGuiInGame::HideSubItemCreateMenu()
{
    GUIINGAME_TRACE();
    if (m_bItemCreateMenuAvailable != 0)
    {
        if (m_pGuiManager->PanelExists(m_pSubItemCreateMenu) != 0)
        {
            m_pGuiManager->PopModalPanel();
            m_pGuiManager->RemovePanel(m_pSubItemCreateMenu);
            if (g_pAppManager->m_pClientExoApp->GetMiniGame() != 0)
            {
#ifdef _XBOX
                g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_MINIGAME);
#else
                g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_MINIGAME, 1);
#endif
            }
            else
            {
#ifdef _XBOX
                g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME);
#else
                g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME, 1);
#endif
            }
            m_pSubItemCreateMenu->ClearItems();
        }
    }
}

void CGuiInGame::ShowExamineBox(unsigned long nStrRef, int nReturnInputClass)
{
    GUIINGAME_TRACE();
    if (m_bItemCreateMenuAvailable != 0)
    {
        m_pExamineBox->SetMessage(nStrRef);
        m_pExamineBox->m_nReturnInputMode = nReturnInputClass;
        m_pGuiManager->AddPanel(m_pExamineBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
#ifdef _XBOX
        g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GUI);
#else
        g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GUI, 1);
#endif
    }
}
// PC 0x0067FE40
void CGuiInGame::HideExamineBox()
{
    GUIINGAME_TRACE();
    if (m_bItemCreateMenuAvailable != 0)
    {
        if (m_pGuiManager->PanelExists(m_pExamineBox) != 0)
        {
            m_pGuiManager->PopModalPanel();
            m_pGuiManager->RemovePanel(m_pExamineBox);
            if (m_pExamineBox->m_nReturnInputMode == 1)
            {
#ifdef _XBOX
                g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GUI);
#else
                g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GUI, 1);
#endif
                return;
            }
#ifdef _XBOX
            g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME);
#else
            g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME, 1);
#endif
        }
    }
}
// PC 0x0067FEB0
void CGuiInGame::ShowContainerGui(unsigned long oidContainer, int nGuiType)
{
    GUIINGAME_TRACE();
    if (m_bItemCreateMenuAvailable != 0)
    {
        m_pContainer->SetContainer(oidContainer, nGuiType);
        m_pGuiManager->AddPanel(m_pContainer, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
#ifdef _XBOX
        g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GUI);
#else
        g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GUI, 1);
#endif
        g_pAppManager->m_pClientExoApp->SetPausedByCombat(1, 3, 0);
    }
}
// PC 0x0067FF10
void CGuiInGame::HideContainerGui()
{
    GUIINGAME_TRACE();
    if (m_bItemCreateMenuAvailable != 0 &&
        m_pGuiManager->PanelExists(m_pContainer) != 0)
    {
        m_pGuiManager->PopModalPanel();
        m_pGuiManager->RemovePanel(m_pContainer);
#ifdef _XBOX
        g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME);
#else
        g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME, 1);
#endif
        g_pAppManager->m_pClientExoApp->SetPausedByCombat(0, 0, 0);
    }
}
// PC 0x0067FF70
int CGuiInGame::IsAnimationPlayingInDialog()
{
    GUIINGAME_TRACE();
    int bPlaying = 0;
    unsigned long i = 0;
    if (i < (unsigned long)m_nDialogAnimations)
    {
        do
        {
            CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
            CSWCObject* pGameObject =
                pClientExoApp->GetGameObject(m_pDialogAnimations[i]);
            CSWCCreature* pClientObject = pGameObject->AsSWCCreature();
            if (pClientObject != 0)
            {
                bPlaying = pClientObject->AnimationDialog(pClientObject->m_nCurrentAnimation);
                if (bPlaying != 0)
                {
                    return 1;
                }
            }
#ifdef _XBOX
            else
            {
                EXOXBOXASSERT(0);
            }
#endif
            i++;
        } while (i < m_nDialogAnimations);
    }
    return bPlaying;
}
// PC 0x006843E0
int CGuiInGame::IsLoopingDialogAnimation(unsigned short nAnimation)
{
    GUIINGAME_TRACE();
    if (nAnimation >= SERVER_ANIMATION_FIRST &&
        nAnimation < g_pRules->m_p2DArrays->m_pDialogAnimations->GetNumRows() +
                         SERVER_ANIMATION_FIRST)
    {
        unsigned short nRow = nAnimation - SERVER_ANIMATION_FIRST;
        C2DA* pAnimations = g_pRules->m_p2DArrays->m_pDialogAnimations;
        int bLooping;
        int bDialog;
        if (!pAnimations->GetINTEntry(
                nRow, CTwoDimArrays::COL2DA_DIALOGANIMATIONS_LOOPING,
                &bLooping))
        {
            return 0;
        }
        pAnimations = g_pRules->m_p2DArrays->m_pDialogAnimations;
        if (pAnimations->GetINTEntry(
                nRow, CTwoDimArrays::COL2DA_DIALOGANIMATIONS_DIALOG,
                &bDialog) && bLooping && bDialog)
        {
            return 1;
        }
        return 0;
    }
    else
    {
        C2DA* pAnimations = g_pRules->m_p2DArrays->m_pAnimations;
        int bLooping;
        int bDialog;
        if (!pAnimations->GetINTEntry(
                nAnimation, CTwoDimArrays::COL2DA_ANIMATIONS_LOOPING,
                &bLooping))
        {
            return IsLoopingDialogStuntAnimation(nAnimation);
        }
        pAnimations = g_pRules->m_p2DArrays->m_pAnimations;
        if ((pAnimations->GetINTEntry(
                 nAnimation, CTwoDimArrays::COL2DA_ANIMATIONS_DIALOG,
                 &bDialog) && bLooping && bDialog) ||
            IsLoopingDialogStuntAnimation(nAnimation))
        {
            return 1;
        }
        return 0;
    }
}
// PC 0x00684510
int CGuiInGame::IsFireForgetDialogAnimation(unsigned short nAnimation)
{
    GUIINGAME_TRACE();
    if (nAnimation >= SERVER_ANIMATION_FIRST &&
        nAnimation < g_pRules->m_p2DArrays->m_pDialogAnimations->GetNumRows() +
                         SERVER_ANIMATION_FIRST)
    {
        unsigned short nRow = nAnimation - SERVER_ANIMATION_FIRST;
        C2DA* pAnimations = g_pRules->m_p2DArrays->m_pDialogAnimations;
        int bFireForget;
        int bDialog;
        if (!pAnimations->GetINTEntry(
                nRow, CTwoDimArrays::COL2DA_DIALOGANIMATIONS_FIREFORGET,
                &bFireForget))
        {
            return 0;
        }
        pAnimations = g_pRules->m_p2DArrays->m_pDialogAnimations;
        if (!pAnimations->GetINTEntry(
                nRow, CTwoDimArrays::COL2DA_DIALOGANIMATIONS_DIALOG,
                &bDialog))
        {
            return 0;
        }
        if (bFireForget && bDialog)
        {
            return 1;
        }
        return 0;
    }
    else
    {
        C2DA* pAnimations = g_pRules->m_p2DArrays->m_pAnimations;
        int bFireForget;
        int bDialog;

        if (!pAnimations->GetINTEntry(
                nAnimation, CTwoDimArrays::COL2DA_ANIMATIONS_FIREFORGET,
                &bFireForget))
        {
            return nAnimation >= CUTSCENE_ANIMATION_FIRST &&
                   nAnimation <= CUTSCENE_ANIMATION_W_LAST;
        }
        pAnimations = g_pRules->m_p2DArrays->m_pAnimations;
        if ((pAnimations->GetINTEntry(
                 nAnimation, CTwoDimArrays::COL2DA_ANIMATIONS_DIALOG,
                 &bDialog) && bFireForget && bDialog) ||
            (nAnimation >= CUTSCENE_ANIMATION_FIRST &&
             nAnimation <= CUTSCENE_ANIMATION_W_LAST))
        {
            return 1;
        }
        return 0;
    }
}
// PC 0x00680010
int CGuiInGame::IsOverlayDialogAnimation(unsigned short nAnimation)
{
    GUIINGAME_TRACE();
    if (nAnimation >= SERVER_ANIMATION_FIRST &&
        nAnimation < g_pRules->m_p2DArrays->m_pDialogAnimations->GetNumRows() +
                         SERVER_ANIMATION_FIRST)
    {
        C2DA* pAnimations = g_pRules->m_p2DArrays->m_pDialogAnimations;
        unsigned short nRow = nAnimation - SERVER_ANIMATION_FIRST;
        int bOverlay;
        int bDialog;
        if (pAnimations->GetINTEntry(
                nRow, CTwoDimArrays::COL2DA_DIALOGANIMATIONS_OVERLAY,
                &bOverlay))
        {
            pAnimations = g_pRules->m_p2DArrays->m_pDialogAnimations;
            if (pAnimations->GetINTEntry(
                    nRow, CTwoDimArrays::COL2DA_DIALOGANIMATIONS_DIALOG,
                    &bDialog) && bOverlay && bDialog)
            {
                return 1;
            }
        }
    }
    return 0;
}

void CGuiInGame::ResetComputerCameraShot()
{
    if (m_pGuiManager->PanelExists(m_pComputerScreen))
    {
        m_pGuiManager->RemovePanel(m_pComputerScreen);
#ifdef _XBOX
        m_pGuiManager->AddPanel(m_pDialog, 0, 1);
#else
        m_pGuiManager->AddPanel(m_pDialog, CSWGuiManager::PANEL_MODE_FOREGROUND, 1);
#endif
        g_pAppManager->m_pClientExoApp->DisableVideoEffect();
    }
}
// PC 0x00684640
void CGuiInGame::CleanUpEntryNode()
{
    GUIINGAME_TRACE();
    m_vComputerCameraShot = Vector();
    m_bComputerCameraShot = 0;
    m_fComputerCameraMicRange = 0;
    m_pDialogFade->ResetFade();
    if (m_pDialog == 0 || m_pDialog != m_pCameraDialog)
    {
        ResetComputerCameraShot();
    }
    m_fAnimatedCameraTime = 0.0f;
#ifdef _XBOX
    g_pExoInput->CoolDownEvent(GUIEVENT_ACTIVATE, 0x2EE);
    g_pExoInput->CoolDownEvent(GUIEVENT_ACCEPT, 0x2EE);
#else
    g_pExoInput->CoolDownEvent(CClientExoAppInternal::INPUT_EVENT_MOUSE_LEFT, 0x2EE);
    g_pExoInput->CoolDownEvent(CClientExoAppInternal::INPUT_EVENT_GUI_ACCEPT, 0x2EE);
#endif
}
// PC 0x006846E0
void CGuiInGame::CleanUpReplyNode()
{
    m_vComputerCameraShot = Vector();
    m_bComputerCameraShot = 0;
    m_fComputerCameraMicRange = 0;
    m_pDialogFade->ResetFade();
    ClearRepliesData();
    if (m_pDialog == 0 || m_pDialog != m_pCameraDialog)
    {
        ResetComputerCameraShot();
    }
    m_fAnimatedCameraTime = 0.0f;
#ifdef _XBOX
    g_pExoInput->CoolDownEvent(GUIEVENT_ACTIVATE, 750);
    g_pExoInput->CoolDownEvent(GUIEVENT_ACCEPT, 750);
#else
    g_pExoInput->CoolDownEvent(CClientExoAppInternal::INPUT_EVENT_MOUSE_LEFT, 750);
    g_pExoInput->CoolDownEvent(CClientExoAppInternal::INPUT_EVENT_GUI_ACCEPT, 750);
#endif
}
// PC 0x006800C0
void CGuiInGame::HideLoadModuleDebugMenu()
{
    GUIINGAME_TRACE();
    if (m_bItemCreateMenuAvailable != 0)
    {
        if (m_pGuiManager->PanelExists(m_pLoadModuleDebugMenu) != 0)
        {
            m_pGuiManager->PopModalPanel();
            m_pGuiManager->RemovePanel(m_pLoadModuleDebugMenu);
            if (g_pAppManager->m_pClientExoApp->GetMiniGame() != 0)
            {
#ifdef _XBOX
                g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_MINIGAME);
#else
                g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_MINIGAME, 1);
#endif
            }
            else
            {
#ifdef _XBOX
                g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME);
#else
                g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME, 1);
#endif
            }
        }
    }
}
// PC 0x00680130
void CGuiInGame::HidePowersFeatsSkillsDebugMenu()
{
    GUIINGAME_TRACE();
    if (m_bItemCreateMenuAvailable != 0)
    {
        if (m_pGuiManager->PanelExists(m_pPowersFeatsSkillsDebugMenu) != 0)
        {
            m_pGuiManager->PopModalPanel();
            m_pGuiManager->RemovePanel(m_pPowersFeatsSkillsDebugMenu);
            if (g_pAppManager->m_pClientExoApp->GetMiniGame() != 0)
            {
#ifdef _XBOX
                g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_MINIGAME);
#else
                g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_MINIGAME, 1);
#endif
            }
            else
            {
#ifdef _XBOX
                g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME);
#else
                g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME, 1);
#endif
            }
        }
    }
}
// PC 0x006801A0
void CGuiInGame::UnlockDialogParticipantOrientations()
{
    GUIINGAME_TRACE();
    CSWCObject* pObject =
        g_pAppManager->m_pClientExoApp->GetGameObject(m_oidDialogObject1);
    if (pObject != 0 && pObject->AsSWCCreature() != 0)
        pObject->AsSWCCreature()->SetLockOrientationToObject(OBJECT_INVALID, 0);

    unsigned long oidObject = m_oidDialogObject2;
    pObject = g_pAppManager->m_pClientExoApp->GetGameObject(oidObject);
    if (pObject != 0 && pObject->AsSWCCreature() != 0)
        pObject->AsSWCCreature()->SetLockOrientationToObject(OBJECT_INVALID, 0);
}
// PC 0x00680280
int CGuiInGame::IsInDialog(unsigned long oidObject)
{
    GUIINGAME_TRACE();
    return m_oidDialogObject1 == oidObject || m_oidDialogObject2 == oidObject;
}
// PC 0x006802B0
int CGuiInGame::ShowLevelUpGUI(int bAutomatic)
{
    GUIINGAME_TRACE();
    if (m_bItemCreateMenuAvailable != 0 &&
        g_pAppManager->m_pClientExoApp->GetInGameGui()->m_LevelUpMode != 0)
    {
        if (bAutomatic != 0)
        {
            if (m_bPaused == 0)
            {
                g_pAppManager->m_pClientExoApp->GetSWCMessage()->
                    SendPlayerToServerInput_TogglePauseRequest();
            }
            g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_PAUSE_ALL_BUT_MUSIC);
        }
#ifndef _XBOX
        else if (g_pAppManager->m_pClientExoApp->GetCharacterChangeInProgress() != 0)
        {
            return 0;
        }
#endif
        if (m_apScreenPanels[SCREEN_CHARACTER] == 0)
        {
            m_apScreenPanels[SCREEN_CHARACTER] = new CSWGuiInGameCharacter(m_pGuiManager);
        }
        static_cast<CSWGuiInGameCharacter*>(m_apScreenPanels[SCREEN_CHARACTER])->
            ShowLevelUpGUI(bAutomatic);
#ifdef _XBOX
        g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GUI);
#else
        g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GUI, 1);
#endif
        return 1;
    }
    return 0;
}
// PC 0x006803D0
void CGuiInGame::ShowPartySelection(const CExoString& sExitScript, int bAllowCancel,
                                    int nForceNPC1, int nForceNPC2)
{
    GUIINGAME_TRACE();
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    if (m_bItemCreateMenuAvailable != 0)
    {
        int nControlledNPC = pServer->GetPartyTable()->m_nControlledNPC;
        if (nControlledNPC == -1 &&
            pServer->GetCreatureByGameObjectID(pServer->GetPlayerCreatureId()) != 0 &&
            pServer->GetCreatureByGameObjectID(
                pServer->GetPlayerCreatureId())->GetArea() != 0 &&
            (pServer->GetCreatureByGameObjectID(
                pServer->GetPlayerCreatureId())->GetArea()->m_bUnescapable == 0 ||
             bAllowCancel != 0) &&
            m_pGuiManager->PanelExists(m_pPartySelectionPanel) == 0)
        {
            if (bAllowCancel != 0)
            {
                if (m_bPaused == 0)
                {
                    g_pAppManager->m_pClientExoApp->GetSWCMessage()->
                        SendPlayerToServerInput_TogglePauseRequest();
                }
                m_pGuiManager->PlayGuiSound(CSWGuiManager::GUISOUND_PAZAAK_START);
                g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_PAUSE_ALL_BUT_MUSIC);
            }
            if (m_pPartySelectionPanel == 0)
                m_pPartySelectionPanel = new CSWGuiPartySelection(m_pGuiManager);
            m_pPartySelectionPanel->m_sRunScript = sExitScript;
            m_pPartySelectionPanel->m_bPlaySound = bAllowCancel;
            m_pPartySelectionPanel->SetForcedNPC(nForceNPC1, nForceNPC2);
            m_pPartySelectionPanel->m_nInputClass =
                bAllowCancel != 0 ? INPUT_CLASS_GAME : INPUT_CLASS_GUI;
            m_pGuiManager->AddPanel(m_pPartySelectionPanel, CSWGuiManager::PANEL_MODE_MODAL, 1);
#ifdef _XBOX
            g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GUI);
#else
            g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GUI, 1);
#endif
        }
    }
}
#ifndef _XBOX
int CGuiInGame::IsGlobalFadeObscuring()  // PC 0x00680630
{
    GUIINGAME_TRACE();
    return m_pGlobalFade->m_fAlpha > 0.001f;
}
#endif
#ifdef _XBOX
void CGuiInGame::SetPaused(int bPaused, unsigned char nReason)
{
    GUIINGAME_TRACE();
    if (m_bItemCreateMenuAvailable != 0)
    {
        if (bPaused != 0)
        {
            switch (nReason)
            {
            case 1:
            case 4:
            case 5:
            case 7:
            case 8:
            case 9:
            case 10:
                if (m_pPausePanel != 0)
                    m_pPausePanel->SetPauseReason(nReason);
                if (m_bShowPauseGui == 1 ||
                    g_pAppManager->m_pClientExoApp->GetMiniGame() != 0)
                {
                    if (m_pPausePanel != 0 &&
                        m_pGuiManager->PanelExists(m_pPausePanel) == 0 &&
                        g_bInScreenshotMode == 0)
                    {
                        m_pGuiManager->AddPanel(m_pPausePanel, SWGUI_PANEL_MODE_OVERLAY, 1);
                    }
                }
                else
                {
                    if (m_pPausePanel != 0 && m_pGuiManager->PanelExists(m_pPausePanel) != 0)
                        m_pPausePanel->SetModal(0);
                }
                break;
            }
        }
        else
        {
            if (m_pPausePanel != 0 && m_pGuiManager->PanelExists(m_pPausePanel) != 0)
                m_pPausePanel->SetModal(0);
        }
        m_bPaused = bPaused;
    }
}
#else
void CGuiInGame::SetPaused(int bPaused, unsigned char nReason)  // PC 0x00680700
{
    GUIINGAME_TRACE();
    if (m_bItemCreateMenuAvailable != 0)
    {
        if (bPaused != 0)
        {
            int bPauseGuiEnabled = 0;
            switch (nReason)
            {
            case 1:
            case 4:
            case 5:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
                bPauseGuiEnabled = 1;
                if (m_pPausePanel != 0)
                    m_pPausePanel->SetPauseReason(nReason);
                if (m_bShowPauseGui == 1 ||
                    g_pAppManager->m_pClientExoApp->GetMiniGame() != 0)
                {
                    if (m_pPausePanel != 0 &&
                        m_pGuiManager->PanelExists(m_pPausePanel) == 0 &&
                        g_bInScreenshotMode == 0)
                    {
                        m_pGuiManager->AddPanel(m_pPausePanel, SWGUI_PANEL_MODE_OVERLAY, 1);
                        m_pGuiManager->SendPanelToBack(m_pPausePanel);
                    }
                }
                else
                {
                    if (m_pPausePanel != 0 && m_pGuiManager->PanelExists(m_pPausePanel) != 0)
                        m_pPausePanel->SetModal(0);
                }
                break;
            }
            m_nPauseState = bPauseGuiEnabled;
        }
        else
        {
            if (m_pPausePanel != 0 && m_pGuiManager->PanelExists(m_pPausePanel) != 0)
                m_pPausePanel->SetModal(0);
            m_nPauseState = 0;
        }
        m_bPaused = bPaused;
    }
}
#endif

void CGuiInGame::SetStuntModelData(unsigned long nModel, unsigned long oidObject,
                                   const CResRef& cModel)
{
    if (nModel < m_nStuntModels && m_pnStuntModelIDs != 0 &&
        m_pStuntModelResRefs != 0)
    {
        m_pnStuntModelIDs[nModel] = oidObject;
        m_pStuntModelResRefs[nModel] = cModel;
    }
}
// PC 0x00680850
int CGuiInGame::HandleDialogModels(unsigned long oidPlayer,
                                  unsigned long nModels,
                                  unsigned long* pObjectIds, CResRef* pModels,
                                  const CResRef& cCameraModel,
                                  const CResRef& cAmbientTrack)
{
    InitializeStuntModelData(nModels);
    for (unsigned long nModel = 0; nModel < nModels; ++nModel)
    {
        SetStuntModelData(nModel, pObjectIds[nModel], pModels[nModel]);
    }
    if (!LoadStuntModelsForDialog())
        return 0;
    LoadCameraModelForDialog(cCameraModel);
    LoadAmbientTrackForDialog(cAmbientTrack);
    return 1;
}
// PC 0x00680930
int CGuiInGame::HandleDialogReplyChosen(unsigned long oidSpeaker,
                                      unsigned long nReply,
                                      unsigned long nReplyCount,
                                      const CExoString& sText,
                                      unsigned long nSound,
                                      unsigned char nFlags,
                                      int bSelected)
{
    GUIINGAME_TRACE();
    CExoString sReply(sText);
    if (!sText.IsEmpty())
    {
        unsigned long oidPlayer = g_pAppManager->m_pServerExoApp->GetPlayerCreatureId();
        CSWSPlayer* pPlayer = g_pAppManager->m_pServerExoApp->GetClientObjectByObjectId(oidPlayer);
        unsigned long oidServer = pPlayer->m_oidPCObject;
        unsigned long oidClient = g_pAppManager->m_pClientExoApp->ServerToClientObjectId(oidServer);
        CClientExoApp* pObjectApp = g_pAppManager->m_pClientExoApp;
        CSWCObject* pObject = pObjectApp->GetGameObject(oidClient);
        g_pSWTlkTable->ParseStr(sReply, (CGameObject*)pObject, 0, 0);
    }
    return 1;
}
#ifndef _XBOX
// PC 0x00684790
int CGuiInGame::HandleDialogSound(unsigned long oidSpeaker, const CResRef& cSound)
{
    GUIINGAME_TRACE();
    CSWCObject* pObject = g_pAppManager->m_pClientExoApp->GetGameObject(
        m_oidDialogObject1);
    if (pObject == 0 || pObject->AsSWCObject() == 0)
        return 0;
    CSWCObject* pClientObject = pObject->AsSWCObject();
    if (cSound.IsValid() && pClientObject != 0)
    {
        if (m_pDialogSound != 0)
        {
            m_pDialogSound->Stop();
            delete m_pDialogSound;
        }
        m_pDialogSound = new CExoStreamingSoundSource(1);
        m_pDialogSound->SetPriorityGroup(
            CExoStreamingSoundSource::PRIORITY_GROUP_DIALOG_VOICE);
        if (m_pDialogSound->Play(cSound, RESTYPE_MP3, 1, 0, 0) == 0 &&
            m_pDialogSound->Play(cSound, RESTYPE_WAV, 1, 0, 0) == 0)
        {
            pClientObject->LipSync(cSound);
            return 0;
        }
        SetDialogSound(oidSpeaker, cSound);
    }
    m_oidDialogSoundObject = pClientObject->m_nId;
    return 1;
}
#endif
void CGuiInGame::ShowStoreGui(unsigned long oidStore, unsigned long oidCreature)  // PC 0x00680A00
{
    GUIINGAME_TRACE();
    if (m_bItemCreateMenuAvailable != 0 && m_pPartySelection == 0 &&
        m_nGlobalDialogState == 0)
    {
        if (g_pAppManager->m_pClientExoApp->GetPausedByCombat() == 0)
            g_pAppManager->m_pClientExoApp->SetPausedByCombat(1, 0, 0);
        g_pAppManager->m_pClientExoApp->IndicateObject(0, 0);
        SetSWGuiStatus(GUISTATUS_HIDE_MAIN_INTERFACE, 1);
        g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_PAUSE_ALL_BUT_MUSIC);
        m_pBarkBubble->SetBark(OBJECT_INVALID, "", "", 0);
#ifdef _XBOX
        g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GUI);
#else
        g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GUI, 1);
#endif
        if (m_pStorePanel == 0)
            m_pStorePanel = new CSWGuiStore(m_pGuiManager);
        m_pStorePanel->m_nStoreId = oidStore;
        m_pStorePanel->m_nCreatureId = oidCreature;
        m_pGuiManager->AddPanel(m_pStorePanel, CSWGuiManager::PANEL_MODE_MODAL, 1);

        m_pGuiManager->PlayGuiSound(CSWGuiManager::GUISOUND_PAZAAK_START);
    }
}
// PC 0x00680B90
void CGuiInGame::HideStoreGui()
{
    GUIINGAME_TRACE();
    if (m_bItemCreateMenuAvailable != 0)
    {
        if (g_pAppManager->m_pClientExoApp->GetPausedByCombat() != 0)
        {
            g_pAppManager->m_pClientExoApp->SetPausedByCombat(0, 0, 0);
        }
        SetSWGuiStatus(GUISTATUS_RESTORE_MAIN_INTERFACE, 1);
#ifdef _XBOX
        g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME);
#else
        g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME, 1);
#endif
#ifdef _XBOX
        m_pGuiManager->RemovePanel(m_pStorePanel);
        if (g_bCreateAllGUIs != 0)
        {
            delete m_pStorePanel;
            m_pStorePanel = 0;
        }
#else
        if (g_bCreateAllGUIs == 0)
        {
            CSWGuiPanel* pPanel = m_pStorePanel;
            pPanel->m_bDeferredRemoval = 1;
            pPanel->m_nExitState = CSWGuiPanel::EXITSTATE_DELETE;
            m_pStorePanel = 0;
        }
        else
        {
            m_pGuiManager->RemovePanel(m_pStorePanel);
        }
#endif
        m_pGuiManager->PlayGuiSound(5);
        g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_GENERIC);
    }
}
// PC 0x00680CC0
void CGuiInGame::ShowSoloModeQuery(int nQuery)
{
    GUIINGAME_TRACE();
    if (m_bItemCreateMenuAvailable && m_pGuiManager && m_pSoloModeQuery)
    {
        CSWParty* pParty = g_pAppManager->m_pClientExoApp->GetSWParty();
        int bMultipleCharacters = 0;
        if (pParty)
            bMultipleCharacters = pParty->m_nCharacters > 1;

        CSWCCreature* pPlayer = g_pAppManager->m_pClientExoApp->GetPlayerCreature();
        CSWSCreature* pServerCreature = 0;
        if (pPlayer)
            pServerCreature = pPlayer->GetServerCreature();

        CSWSArea* pServerArea = 0;
        if (g_pAppManager->m_pClientExoApp->GetModule())
        {
            CSWCModule* pModule;
            if ((pModule = g_pAppManager->m_pClientExoApp->GetModule())->m_pInfo != 0)
                pServerArea = pModule->m_pInfo->GetServerArea();
        }

        if (g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nGlobalDialogState ||
            (pServerArea && pServerArea->m_bTransitionPending) ||
            g_pAppManager->m_pClientExoApp->GetAreaNotReadyToDisplay() ||
            (pServerCreature && (pServerCreature->GetDead() || pServerCreature->GetDeadTemp())) ||
            !bMultipleCharacters)
        {
            m_pGuiManager->PlayGuiSound(SWGUI_SOUND_REJECT);
        }
        else
        {
            if (!m_bPaused)
                g_pAppManager->m_pClientExoApp->GetSWCMessage()->SendPlayerToServerInput_TogglePauseRequest();
            m_pSoloModeQuery->SetQueryText(nQuery);
            if (!m_pGuiManager->PanelExists(m_pSoloModeQuery))
                m_pGuiManager->AddPanel(m_pSoloModeQuery, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
#ifdef _XBOX
            g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GUI);
#else
            g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GUI, 1);
#endif
        }
    }
}
// PC 0x00680E40
void CGuiInGame::HideSoloMode()
{
    GUIINGAME_TRACE();
    if (m_pGuiManager->PanelExists(m_pSoloModeQuery))
    {
        if (!m_bPaused)
            g_pAppManager->m_pClientExoApp->GetSWCMessage()->SendPlayerToServerInput_TogglePauseRequest();
        m_pGuiManager->PopModalPanel();
        m_pSoloModeQuery->SetModal(0);
#ifdef _XBOX
        g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME);
#else
        g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME, 1);
#endif
    }
}
// PC 0x00680ED0
void CGuiInGame::ShowUpgradeScreen(unsigned long oidItem, unsigned long oidCharacter,
                                   int bDisableItemCreation, int bDisableUpgrade,
                                   CExoString sOverride2DA)
{
    GUIINGAME_TRACE();
    if (m_bItemCreateMenuAvailable != 0)
    {
        if (m_pGuiManager != 0)
        {
            if (m_pUpgradeScreen == 0)
            {
                if (oidItem == OBJECT_INVALID ||
                    g_pAppManager->m_pServerExoApp->GetItemByGameObjectID(oidItem) != 0)
                {
                    g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_PAUSE_ALL_BUT_MUSIC);
                    if (m_bPaused == 0)
                    {
                        g_pAppManager->m_pClientExoApp->GetSWCMessage()->
                            SendPlayerToServerInput_TogglePauseRequest();
                    }
                    m_pUpgradeScreen = new CSWGuiUpgradeSelection(m_pGuiManager, oidCharacter);
                    m_pUpgradeScreen->m_oidItem = oidItem;
                    m_pUpgradeScreen->m_oidCharacter = oidCharacter;
                    if (bDisableItemCreation)
                        m_pUpgradeScreen->m_bDisableItemCreation = 1;
                    else
                        m_pUpgradeScreen->m_bDisableItemCreation = 0;
                    if (bDisableUpgrade)
                        m_pUpgradeScreen->m_bDisableUpgrade = 1;
                    else
                        m_pUpgradeScreen->m_bDisableUpgrade = 0;
                    if (!sOverride2DA.IsEmpty())
                        m_pUpgradeScreen->SetOverride2DA(sOverride2DA);
                    else
                        m_pUpgradeScreen->SetOverride2DA("");
                    m_pGuiManager->AddPanel(m_pUpgradeScreen, CSWGuiManager::PANEL_MODE_MODAL, 1);
#ifdef _XBOX
                    g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GUI);
#else
                    g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GUI, 1);
#endif
                }
            }
        }
    }
}
// PC 0x00681170
void CGuiInGame::HideUpgradeScreen()
{
    GUIINGAME_TRACE();
    if (m_bItemCreateMenuAvailable != 0)
    {
        if (m_pGuiManager != 0)
        {
            if (m_pUpgradeScreen != 0)
            {
                if (m_bPaused == 0)
                {
                    g_pAppManager->m_pClientExoApp->GetSWCMessage()->
                        SendPlayerToServerInput_TogglePauseRequest();
                }
#ifdef _XBOX
                g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME);
#else
                g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME, 1);
#endif
                g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_GENERIC);
                CSWGuiUpgradeSelection* pUpgradeScreen = m_pUpgradeScreen;
                pUpgradeScreen->m_bDeferredRemoval = 0;
                pUpgradeScreen->m_nExitState = CSWGuiPanel::EXITSTATE_DELETE;
                m_pUpgradeScreen = 0;
            }
        }
    }
}
// PC 0x00681670
void CGuiInGame::SetShotFromScript(int nCamera, int nVideoEffect)
{
    if (m_nGlobalDialogState != 0)
    {
        if (m_pDialog != m_pComputerDialog)
        {
            CSWBehaviorCameraDialog* pCamera = reinterpret_cast<CSWBehaviorCameraDialog*>(
                g_pAppManager->m_pClientExoApp->GetModule()->GetCamera()->
                    GetBehavior(AUR_BEHAVIOR_CAMERA_DIALOG));
            if (pCamera != 0)
            {
                int bFound = 0;
                int nIndex;
                for (nIndex = 0; nIndex < m_nPlaceableCameras; nIndex++)
                {
                    if (m_aPlaceableCameras[nIndex].m_nPlaceableID == nCamera)
                    {
                        bFound = 1;
                        break;
                    }
                }
                Quaternion qOrientation;
                if (bFound)
                {
                    Vector vPosition = m_aPlaceableCameras[nIndex].m_vPosition;
                    vPosition.m_fZ += m_aPlaceableCameras[nIndex].m_fHeight;
                    qOrientation = m_aPlaceableCameras[nIndex].m_qOrientation;
                    qOrientation = YawPitchRoll(Yaw(qOrientation),
                        Pitch(qOrientation) + m_aPlaceableCameras[nIndex].m_fPitch,
                        Roll(m_aPlaceableCameras[nIndex].m_qOrientation));
                    float fFieldOfView = m_aPlaceableCameras[nIndex].m_fFieldOfView;
                    float fMicRange = m_aPlaceableCameras[nIndex].m_fMicRange;
                    pCamera->SetShot(CSWBehaviorCameraDialog::FIXED_CAMERA_SHOT,
                        vPosition, qOrientation, fFieldOfView);
                    m_bComputerCameraShot = 1;
                    m_vComputerCameraShot = vPosition;
                    m_fComputerCameraMicRange = fMicRange;
                    if (nVideoEffect != CAMERA_VIDEO_EFFECT_DEFAULT &&
                        nVideoEffect != CAMERA_VIDEO_EFFECT_UNCHANGED)
                        g_pAppManager->m_pClientExoApp->EnableVideoEffect(nVideoEffect);
                }
            }
        }
        else
        {
            if (m_bShotCameraInitialized == 0)
                InitializeShotCamera();
            SetComputerCameraShot(nCamera, m_oidComputerCameraTarget, nVideoEffect);
        }
    }
}
// PC 0x006818F0
void CGuiInGame::HideAreaTransition()
{
    GUIINGAME_TRACE();
    if (g_pAppManager->m_pClientExoApp->GetModule() != 0)
    {
        CSWCModule* pModule = g_pAppManager->m_pClientExoApp->GetModule();
        if (pModule->m_pInfo != 0)
        {
            CSWSArea* pServerArea = pModule->m_pInfo->GetServerArea();
            if (pServerArea != 0 && pServerArea->m_bTransitionPending != 0)
            {
                if (m_pGuiManager->PanelExists(m_pAreaTransition) != 0)
                {
                    m_pGuiManager->SendPanelToBack(m_pAreaTransition);
                }
                return;
            }
        }
    }
    if (m_bItemCreateMenuAvailable != 0 && m_pGuiManager != 0
        && m_pAreaTransition != 0)
    {
        Vector vPosition;
        m_pAreaTransition->SetTransitionObject(OBJECT_INVALID, vPosition);
        if (m_pGuiManager->PanelExists(m_pAreaTransition) != 0)
        {
            m_pGuiManager->RemovePanel(m_pAreaTransition);
        }
    }
}
// PC 0x006819C0
int CGuiInGame::GetBarkBubbleVisible(int& nTop)
{
    GUIINGAME_TRACE();
    CSWGuiExtent* pExtent;

    if (m_pBarkBubble == 0 || m_pGuiManager->PanelExists(m_pBarkBubble) == 0)
    {
        return 0;
    }
    pExtent = &m_pBarkBubble->m_cExtent;
    nTop = pExtent->m_nTop + pExtent->m_nHeight;
    return 1;
}
// PC 0x00681A00
Vector CGuiInGame::GetAreaTranstionPosition()
{
    GUIINGAME_TRACE();
    if (m_pAreaTransition == NULL)
    {
        return Vector();
    }
    return m_pAreaTransition->m_vTransitionPosition;
}
// PC 0x00684A90
void CGuiInGame::SetGlobalDialogState(int nState)
{
    GUIINGAME_TRACE();
    m_nGlobalDialogState = nState;
    if (nState == 0)
    {
        m_nC0 = 0;
        g_pAppManager->m_pServerExoApp->SetIgnoreConversationStartRange(0);
        g_pAppManager->m_pClientExoApp->GetInGameGui()->SetGlobalUnpauseableState(0);
#ifndef _XBOX
        if (m_bHoldWorldFadeInForDialog)
        {
            m_bHoldWorldFadeInForDialog = 0;
            StartGlobalFade(1, 0.0f, 1.0f, Vector());
        }
        g_pAppManager->m_pClientExoApp->m_pcExoAppInternal->ShowMouse(16);
#endif
    }
}
// PC 0x00681A60
Vector CGuiInGame::GetDialogCameraPosition(int bLookAhead)
{
    GUIINGAME_TRACE();
    CSWCCreature* pCreature = g_pAppManager->m_pClientExoApp->GetPlayerCreature();
    Vector vResult;
    if (pCreature != 0)
        vResult = pCreature->m_vDesiredPosition;

    if ((m_bShotCameraInitialized || m_bAnimatedCameraActive) &&
        g_pAppManager->m_pClientExoApp->GetModule() != 0)
    {
        CAurCamera* pCamera = g_pAppManager->m_pClientExoApp->GetModule()->GetCamera();
        pCreature = g_pAppManager->m_pClientExoApp->GetPlayerCreature();
        if (pCamera != 0)
        {
            Vector vPosition = pCamera->GetPosition();
            if (bLookAhead)
            {
                Quaternion qOrientation = pCamera->GetOrientation();
                Vector vDirection = -qOrientation.zdir();
                vPosition = vPosition + normalize(vDirection) * GUI_DIALOG_CAMERA_LOOK_AHEAD_DISTANCE;
            }
            vResult = vPosition;
        }
    }
    return vResult;
}
// PC 0x00681C30
void CGuiInGame::UpdateStatus(unsigned char nMessage, int nValue)
{
    CSWGuiStatusSummary* pSummary = m_pStatusSummary;
    if (pSummary != 0)
    {
        if (pSummary->m_cStatusSummary.m_nSuppressed > 0)
        {
            --pSummary->m_cStatusSummary.m_nSuppressed;
            return;
        }

        switch (nMessage)
        {
        case GUI_STATUS_JOURNAL:
            pSummary->m_cStatusSummary.m_bJournal = 1;
            pSummary->m_cStatusSummary.m_bDisplaySPending = 1;
            break;
        case GUI_STATUS_CREDITS:
            pSummary->AddCredits(nValue);
            break;
        case GUI_STATUS_XP:
            pSummary->AddExperience(nValue);
            break;
        case GUI_STATUS_STEALTH_XP:
            pSummary->AddStealthExperience(nValue);
            break;
        case GUI_STATUS_DARK_SHIFT:
        case GUI_STATUS_LIGHT_SHIFT:
            pSummary->AddAlignmentShift(nValue);
            break;
        case GUI_STATUS_ITEM_RECEIVED:
            pSummary->m_cStatusSummary.m_bItemReceived = 1;
            pSummary->m_cStatusSummary.m_bDisplaySPending = 1;
            break;
        case GUI_STATUS_ITEM_LOST:
            pSummary->m_cStatusSummary.m_bItemLost = 1;
            pSummary->m_cStatusSummary.m_bDisplaySPending = 1;
            break;
        case GUI_STATUS_LEVEL_UP:
            pSummary->m_cStatusSummary.m_bLevelUpSound = 1;
            pSummary->m_cStatusSummary.m_bSoundPending = 1;
            break;
        case GUI_STATUS_NEW_QUEST:
            pSummary->m_cStatusSummary.m_bNewQuestSound = 1;
            pSummary->m_cStatusSummary.m_bSoundPending = 1;
            break;
        case GUI_STATUS_QUEST_COMPLETE:
            pSummary->m_cStatusSummary.m_bCompleteSound = 1;
            pSummary->m_cStatusSummary.m_bSoundPending = 1;
            break;
        }
    }
}
// PC 0x00681DB0
int CGuiInGame::GetPendingStatusSummary()
{
    GUIINGAME_TRACE();
    return m_pStatusSummary != 0 &&
           (m_pStatusSummary->m_cStatusSummary.m_bDisplaySPending ||
            m_pStatusSummary->m_cStatusSummary.m_bSoundPending);
}
// PC 0x00689310
void CGuiInGame::ShowStatusSummary()
{
    GUIINGAME_TRACE();
    if (m_pStatusSummary != 0)
    {
        if (m_pStatusSummary->m_cStatusSummary.m_bDisplaySPending &&
            g_pAppManager->m_pClientExoApp->GetClientOptions()
                ->m_bStatusSummary)
        {
            m_pStatusSummary->m_bWasPaused =
                m_bPaused ||
                g_pAppManager->m_pClientExoApp->GetTogglePauseAtEndOfFrame();
            g_pAppManager->m_pClientExoApp->SetPausedByCombat(1, 6, 0);
            m_pGuiManager->AddPanel(m_pStatusSummary, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
#ifdef _XBOX
            g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GUI);
#else
            g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GUI, 0);
#endif
        }
        else
        {
            m_pStatusSummary->DisplayStatus();
        }
    }
}
// PC 0x00681DD0
void CGuiInGame::SetStatusSummary(const CStatusSummary& cSummary)
{
    GUIINGAME_TRACE();
    if (m_pStatusSummary != 0)
    {
        m_pStatusSummary->m_cStatusSummary = cSummary;
    }
    else
    {
        if (m_pPendingStatusSummary == 0)
        {
            m_pPendingStatusSummary = new CStatusSummary;
        }
        *m_pPendingStatusSummary = cSummary;
    }
}
// PC 0x00681E30
void CGuiInGame::GetStatusSummary(CStatusSummary& cSummary)
{
    GUIINGAME_TRACE();
    if (m_pStatusSummary != 0)
    {
        cSummary = m_pStatusSummary->m_cStatusSummary;
    }
}
// PC 0x00681E50
void CGuiInGame::SuppressStatusSummary(int nNumEntries)
{
    GUIINGAME_TRACE();
    if (m_pStatusSummary != 0 && nNumEntries >= 0)
    {
        m_pStatusSummary->m_cStatusSummary.m_nSuppressed = nNumEntries;
    }
}
// PC 0x00681E90
int CGuiInGame::CanPlayDialogAnimations(unsigned long oidObject)
{
    GUIINGAME_TRACE();
    if (oidObject != OBJECT_INVALID)
    {
        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
        unsigned long oidServer = pServer->ClientToServerObjectId(oidObject);
        CGameObject* pObject =
            g_pAppManager->m_pServerExoApp->GetGameObject(oidServer);
        if (pObject == 0)
        {
            return 0;
        }
        if (pObject->AsSWSCreature() != 0)
        {
            CSWSCreature* pCreature = pObject->AsSWSCreature();
            if (pCreature->IsDebilitated() || ((CSWSObject*)pCreature)->GetDead() ||
                pCreature->GetDeadTemp())
            {
                return 0;
            }
        }
    }
    return 1;
}
// PC 0x00684B50
void CGuiInGame::SetLockObjectOrientationInDialog(unsigned long oidObject, int bLock)
{
    GUIINGAME_TRACE();
    if (bLock == 1)
    {
        CSWCObject* pObject = g_pAppManager->m_pClientExoApp->GetGameObject(oidObject);
        if (pObject == 0 || pObject->AsSWCObject() == 0)
            return;

        else
        {
            CExoLinkedListNode* pPosition = m_lDialogNoOrientList.m_pInternal->m_pHead;
            void* pObjectID = m_lDialogNoOrientList.m_pInternal->m_pHead;
            if (pObjectID != 0)
                pObjectID = ((CExoLinkedListNode*)pObjectID)->m_pData;
            while (pPosition != 0)
            {
                if (*(unsigned long*)pObjectID == oidObject)
                    return;
                pObjectID = m_lDialogNoOrientList.m_pInternal->GetNext(pPosition);
            }
            unsigned long* pLockedObjectID = new unsigned long;
            if (pLockedObjectID != 0)
            {
                *pLockedObjectID = oidObject;
                m_lDialogNoOrientList.m_pInternal->AddHead(pLockedObjectID);
            }
        }
    }
    else
    {
        CExoLinkedListNode* pPosition = m_lDialogNoOrientList.m_pInternal->m_pHead;
        void* pObjectID = m_lDialogNoOrientList.m_pInternal->m_pHead;
        if (pObjectID != 0)
            pObjectID = ((CExoLinkedListNode*)pObjectID)->m_pData;
        while (pPosition != 0)
        {
            if (*(unsigned long*)pObjectID == oidObject)
            {
                m_lDialogNoOrientList.m_pInternal->Remove(pPosition);
                return;
            }
            pObjectID = m_lDialogNoOrientList.m_pInternal->GetNext(pPosition);
        }
    }
}
// PC 0x00684C40
int CGuiInGame::GetCanOrientInDialog(unsigned long oidObject)
{
    GUIINGAME_TRACE();
    if (oidObject != OBJECT_INVALID)
    {
        CExoLinkedListNode* pPosition = m_lDialogNoOrientList.m_pInternal->m_pHead;
        void* pObjectID = m_lDialogNoOrientList.m_pInternal->m_pHead;
        if (pObjectID != 0)
        {
            pObjectID = ((CExoLinkedListNode*)pObjectID)->m_pData;
        }
        while (pPosition != 0)
        {
            if (*(unsigned long*)pObjectID == oidObject)
            {
                return 0;
            }
            pObjectID = m_lDialogNoOrientList.m_pInternal->GetNext(pPosition);
        }
        return 1;
    }
    return 0;
}
// PC 0x00684CA0
void CGuiInGame::SetLockHeadFollowInDialog(unsigned long oidObject, int bLock)
{
    GUIINGAME_TRACE();
    if (bLock == 1)
    {
        CClientExoApp* pClientApp = g_pAppManager->m_pClientExoApp;
        CSWCObject* pObject = pClientApp->GetGameObject(oidObject);
        if (pObject == 0 || pObject->AsSWCObject() == 0)
            return;

        CExoLinkedListNode* pPosition = m_lDialogNoHeadFollowList.m_pInternal->m_pHead;
        void* pObjectID = m_lDialogNoHeadFollowList.m_pInternal->m_pHead;
        if (pObjectID != 0)
        {
            pObjectID = ((CExoLinkedListNode*)pObjectID)->m_pData;
        }
        while (pPosition != 0)
        {
            if (*(unsigned long*)pObjectID == oidObject)
                return;
            pObjectID = m_lDialogNoHeadFollowList.m_pInternal->GetNext(pPosition);
        }
        unsigned long* pLockedObjectID = new unsigned long;
        if (pLockedObjectID != 0)
        {
            *pLockedObjectID = oidObject;
            m_lDialogNoHeadFollowList.m_pInternal->AddHead(pLockedObjectID);
            if (pObject->AsSWCCreature() == 0)
                return;
            pObject->AsSWCCreature()->LookAt(OBJECT_INVALID, 0.0f);
        }
    }
    else
    {
        CExoLinkedListNode* pPosition = m_lDialogNoHeadFollowList.m_pInternal->m_pHead;
        void* pObjectID = m_lDialogNoHeadFollowList.m_pInternal->m_pHead;
        if (pObjectID != 0)
        {
            pObjectID = ((CExoLinkedListNode*)pObjectID)->m_pData;
        }
        while (pPosition != 0)
        {
            if (*(unsigned long*)pObjectID == oidObject)
            {
                m_lDialogNoHeadFollowList.m_pInternal->Remove(pPosition);
                return;
            }
            pObjectID = m_lDialogNoHeadFollowList.m_pInternal->GetNext(pPosition);
        }
    }
}
// PC 0x00684DB0
int CGuiInGame::GetCanHeadFollowInDialog(unsigned long oidObject)
{
    GUIINGAME_TRACE();
    if (oidObject != OBJECT_INVALID)
    {
        CExoLinkedListNode* pPosition = m_lDialogNoHeadFollowList.m_pInternal->m_pHead;
        void* pObjectID = m_lDialogNoHeadFollowList.m_pInternal->m_pHead;
        if (pObjectID != 0)
        {
            pObjectID = ((CExoLinkedListNode*)pObjectID)->m_pData;
        }
        while (pPosition != 0)
        {
            if (*(unsigned long*)pObjectID == oidObject)
            {
                return 0;
            }
            pObjectID = m_lDialogNoHeadFollowList.m_pInternal->GetNext(pPosition);
        }
        return 1;
    }
    return 0;
}
// PC 0x00684E10
int CGuiInGame::TutorialReasonWillShowWindow(unsigned char nReason)
{
    GUIINGAME_TRACE();
    if (g_pAppManager->m_pClientExoApp->GetClientOptions()->m_bTutorialPopups)
    {
        int bShown = 0;
        if (nReason < TUTORIAL_WINDOW_COUNT)
        {
            unsigned char nByte = nReason / 8;
            unsigned char nBit = nReason - nByte * 8;
            bShown = m_aTutorialWindowsShown[nByte] & (1 << nBit);
        }
        if (bShown)
        {
            return 0;
        }
        if (m_nGlobalDialogState == 0 &&
            m_pTutorialBox->TutorialReasonIsValid(nReason))
        {
            return 1;
        }
    }
    return 0;
}

#include "swmain.h"
// PC 0x00681F70
int CGuiInGame::ShowTutorialWindow(unsigned char nReason,
                                  unsigned long oidInstigator,
                                  unsigned long oidTarget,
                                  unsigned long nActionParam)
{
    GUIINGAME_TRACE();
    if (!g_pAppManager->m_pClientExoApp->GetClientOptions()->m_bTutorialPopups
        || nReason >= TUTORIAL_WINDOW_COUNT)
    {
        return 0;
    }

    int bShown = 0;
    if (nReason < TUTORIAL_WINDOW_COUNT)
    {
        unsigned char nByte = nReason / 8;
        unsigned char nBit = nReason - nByte * 8;
        bShown = m_aTutorialWindowsShown[nByte] & (1 << nBit);
    }
    if (bShown)
    {
        return 0;
    }
    {
        unsigned char nByte = nReason / 8;
        unsigned char nBit = nReason - nByte * 8;
        m_aTutorialWindowsShown[nByte] |= 1 << nBit;
    }
    m_pTutorialBox->SetTutorialInstigator(oidInstigator, oidTarget, nActionParam);
    CClientExoApp* pClientApp = g_pAppManager->m_pClientExoApp;
    if (!pClientApp->GetSWGuiManager()->PanelExists(m_pTutorialBox))
    {
        CClientExoApp* pApp = g_pAppManager->m_pClientExoApp;
        pApp->GetSWGuiManager()->AddPanel(m_pTutorialBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
    }
    return 1;
}
// PC 0x00682040
void CGuiInGame::EnterControllerPauseState()
{
    GUIINGAME_TRACE();
    CSWGuiPanel* pPanel;
    int bPausedByCombat;

    pPanel =
        g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pControllerLossBox;
    if (pPanel != 0)
    {
        m_pGuiManager->AddPanel(pPanel, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
    }
    if (g_pAppManager != 0 && g_pAppManager->m_pServerExoApp != 0)
    {
        bPausedByCombat = g_pAppManager->m_pClientExoApp->GetPausedByCombat();
        m_bPausedByCombatBeforeControllerPause = bPausedByCombat;
        if (bPausedByCombat == 0)
        {
            g_pAppManager->m_pClientExoApp->SetPausedByCombat(1, 0, 1);
        }
    }
}
// PC 0x006820A0
void CGuiInGame::ResetInterfaceForSize()
{
    GUIINGAME_TRACE();
    if (m_pMainInterface != 0)
    {
        delete m_pMainInterface;
        m_pMainInterface = new CSWGuiMainInterface(
            g_pAppManager->m_pClientExoApp->GetSWGuiManager());
    }
    if (m_pCameraDialog != 0)
    {
        delete m_pCameraDialog;
        m_pCameraDialog = new CSWGuiDialogCinematic(
            g_pAppManager->m_pClientExoApp->GetSWGuiManager());
    }
    if (m_pItemCreateMenu != 0)
        m_pItemCreateMenu->CenterPanel();
    if (m_pLoadModuleDebugMenu != 0)
        m_pLoadModuleDebugMenu->CenterPanel();
    if (m_pPowersFeatsSkillsDebugMenu != 0)
        m_pPowersFeatsSkillsDebugMenu->CenterPanel();
    if (m_pSubItemCreateMenu != 0)
        m_pSubItemCreateMenu->CenterPanel();
    if (m_pMessageBox != 0)
    {
        delete m_pMessageBox;
        m_pMessageBox = new CSWGuiMessageBox(
            g_pAppManager->m_pClientExoApp->GetSWGuiManager());
    }
    if (m_pContainer != 0)
        m_pContainer->CenterPanel();
    if (m_pSkillInfoBox != 0)
        m_pSkillInfoBox->CenterPanel();
    if (m_pTutorialBox != 0)
        m_pTutorialBox->CenterPanel();
}
// PC 0x00682260
void CGuiInGame::UpdateControllerPauseState()
{
    if (g_pExoInput->GetMainDeviceLost() != 0 &&
        m_nGlobalUnpauseableState <= 0 && m_bControllerPauseActive == 0)
    {
        m_bInputActiveBeforeControllerPause = g_pExoInput->GetActive();
        g_pExoInput->SetActive(1);
        EnterControllerPauseState();
        m_bControllerPauseActive = 1;
    }
}
// PC 0x006822C0
void CGuiInGame::EndControllerPauseState()
{
    GUIINGAME_TRACE();
    int bInputActive;

    bInputActive = m_bInputActiveBeforeControllerPause == 1;
    m_bControllerPauseActive = 0;
    if (bInputActive)
    {
        g_pExoInput->SetActive(1);
    }
    else
    {
        g_pExoInput->SetActive(0);
    }
    if (g_pAppManager != 0 && g_pAppManager->m_pServerExoApp != 0 &&
        m_bPausedByCombatBeforeControllerPause == 0)
    {
        g_pAppManager->m_pClientExoApp->SetPausedByCombat(0, 0, 1);
    }
}
// PC 0x00684E90
int CGuiInGame::IsOnDialogNonDependantList(const CExoString& sName)
{
    GUIINGAME_TRACE();
    if (!sName.CompareNoCase(""))
    {
        CExoString* pName = (CExoString*)m_lDialogNonDependantList.m_pInternal->GetHead();
        CExoLinkedListNode* pPosition = m_lDialogNonDependantList.m_pInternal->m_pHead;
        while (pPosition != 0)
        {
            if (sName.CompareNoCase(pName->CStr()))
            {
                return 1;
            }
            pName = (CExoString*)m_lDialogNonDependantList.m_pInternal->GetNext(pPosition);
        }
    }
    return 0;
}
// PC 0x00684F00
void CGuiInGame::ClearDialogNonDependantList()
{
    while (m_lDialogNonDependantList.GetHeadPos() != 0)
    {
        CExoString* pName =
            (CExoString*)m_lDialogNonDependantList.RemoveHead();
        if (pName != 0)
        {
            delete pName;
        }
    }
}
// PC 0x00682340
unsigned char CGuiInGame::GetInterfaceOffset()
{
    GUIINGAME_TRACE();
    if (m_pMainInterface != 0)
    {
        return m_pMainInterface->m_nInterfaceOffset;
    }
    return 0;
}
// PC 0x006823A0
int CGuiInGame::GetMiniMapVisible()
{
    GUIINGAME_TRACE();
    if (!g_pAppManager->m_pClientExoApp->GetClientOptions()->m_bMiniMap)
    {
        return 0;
    }
    if (g_pAppManager->m_pClientExoApp->GetMiniGame() != 0)
    {
        return 0;
    }
    if (m_pMainInterface != 0)
    {
        return m_pMainInterface->m_lblMapborder.m_bEnabled;
    }
    return 1;
}
// PC 0x00682700
void CGuiInGame::MarkNoClickEvent(unsigned long nMilliseconds)
{
    GUIINGAME_TRACE();
    CClientExoApp* pApp = g_pAppManager->m_pClientExoApp;
    pApp->GetWorldTimer()->GetWorldTime(&m_nNoClickDay, &m_nNoClickTime);
    m_nNoClickDuration = nMilliseconds;
}
// PC 0x00682740
int CGuiInGame::GetCanClick()
{
    GUIINGAME_TRACE();
    unsigned long nElapsedTime = 0;
    unsigned long nCurrentTime;
    unsigned long nCurrentDay;
    unsigned long nElapsedDay;
    CClientExoApp* pApp = g_pAppManager->m_pClientExoApp;
    pApp->GetWorldTimer()->GetWorldTime(&nCurrentDay, &nCurrentTime);
    CClientExoApp* pCurrentApp = g_pAppManager->m_pClientExoApp;
    pCurrentApp->GetWorldTimer()->SubtractWorldTimes(
        nCurrentDay, nCurrentTime, m_nNoClickDay, m_nNoClickTime, &nElapsedDay,
        &nElapsedTime);
    if (nElapsedDay > 0)
    {
        return 1;
    }
    return nElapsedTime >= m_nNoClickDuration;
}
// PC 0x00682800
void CGuiInGame::UpdateDialogLipSync()
{
    GUIINGAME_TRACE();
    if (m_nDialogSoundId != OBJECT_INVALID && m_pDialogSound != 0 &&
        m_pDialogSound->IsPlaying() && m_pDialogSound->GetHasStartedStreaming())
    {
        CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
        CSWCObject* pObject = pClient->GetGameObject(m_nDialogSoundId);
        if (pObject != 0 && pObject->AsSWCObject() != 0)
        {
            pObject->AsSWCObject()->LipSync(m_cDialogSound);
            m_cDialogSound = "";
            m_nDialogSoundId = OBJECT_INVALID;
        }
    }
}
// PC 0x006828C0
int CGuiInGame::GetFadeOverridePanelVisible()
{
    GUIINGAME_TRACE();
    if (m_bItemCreateMenuAvailable != 0 && m_pGuiManager != 0)
    {
        return (m_pGalaxyMap != 0 && m_pGuiManager->PanelExists(m_pGalaxyMap) && m_pGalaxyMap->m_bVisible) ||
            (m_pPartySelectionPanel != 0 && m_pGuiManager->PanelExists(m_pPartySelectionPanel) && m_pPartySelectionPanel->m_bVisible) ||
            (m_pStorePanel != 0 && m_pGuiManager->PanelExists(m_pStorePanel) && m_pStorePanel->m_bVisible) ||
            (m_pInGameMenu != 0 && m_pGuiManager->PanelExists(m_pInGameMenu) && m_pInGameMenu->m_bVisible) ||
            g_pAppManager->m_pClientExoApp->GetPazaakIsRunning();
    }
    return 0;
}
// PC 0x00682980
void CGuiInGame::FadeAndStopDialogAmbientTrack()
{
    GUIINGAME_TRACE();
    if (m_pDialogAmbientTrack != 0)
    {
        g_pAppManager->m_pClientExoApp->FadeAndStopStreamingSoundSource(
            m_pDialogAmbientTrack, 500);
        delete m_pDialogAmbientTrack;
        m_pDialogAmbientTrack = 0;
    }
}
// PC 0x006888C0
void CGuiInGame::AddToDialogNonDependantList(const CExoString& sName)
{
    GUIINGAME_HOOK();
    if (!sName.CompareNoCase("") && !sName.IsEmpty() &&
        !IsOnDialogNonDependantList(sName))
    {
        CExoString* pName = new CExoString;
        *pName = sName;
        m_lDialogNonDependantList.AddHead(pName);
    }
}
// PC 0x00688960
int CGuiInGame::UnpackQuickSaveGame(unsigned long nSaveGame,
                                  const CExoString& sSaveName)
{
    if (CopyQuickSaveGameToFutureGame(nSaveGame, sSaveName))
    {
        CExoString sCurrentPath = g_pExoBase->m_pExoAliasList->ResolveFileName(
            "GAMEINPROGRESS:", 0xFFFF);
        CExoString sFuturePath = g_pExoBase->m_pExoAliasList->ResolveFileName(
            "FUTUREGAME:", 0xFFFF);
        g_pExoResMan->NukeDirectory("GAMEINPROGRESS:", 1, 1);
        sFuturePath = sFuturePath.Left(sFuturePath.GetLength() - 1);
        sCurrentPath = sCurrentPath.Left(sCurrentPath.GetLength() - 1);
        rename(sFuturePath.CStr(), sCurrentPath.CStr());
        return 1;
    }

    CExoString sDirectory;
    CExoString sSavePath;
    if (nSaveGame != SAVEGAME_UNNUMBERED)
    {
        sDirectory.Format("%06d - %s", nSaveGame, sSaveName.CStr());
    }
    else
    {
        sDirectory = sSaveName;
    }
    sSavePath = "SAVES:";
    sSavePath = sSavePath + sDirectory;
#ifdef _XBOX
    g_pAppManager->m_pClientExoApp->HideLoadScreen(0);
#else
    g_pAppManager->m_pClientExoApp->HideLoadScreen(0, 1);
#endif
    CExoFile cFile(sSavePath + "\\" + "CORRUPT", 0, "wb+");
    cFile.Write("CORRUPT");
    cFile.Flush();
    return 0;
}
// PC 0x00684F40
int CGuiInGame::CopyQuickSaveGameToFutureGame(unsigned long nSaveGame,
                                            const CExoString& sSaveName)
{
    CExoString sSavePath;
    CExoString sDirectory;
    CExoString sFileName;
    CExoString sTemp;

    if (nSaveGame != SAVEGAME_UNNUMBERED)
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
        g_pAppManager->GetClientExoApp()->GetLoadStep(0));
    delete pERF;
    return 1;
}
#ifndef _XBOX
int CGuiInGame::GetCurrentEquipScreenNPC()
{
    GUIINGAME_TRACE();
    int nNPC = -1;

    if (m_pGuiManager != 0 && m_pGuiManager->PanelExists(m_apScreenPanels[0]) != 0)
    {
        nNPC = ((CSWGuiInGameEquip*)m_apScreenPanels[0])->m_nCharacter;
    }
    return nNPC;
}
#endif

#include "swguiingame.inl"
// PC 0x00685B20
void CGuiInGame::InitializeGlobal()
{
    if (m_pGuiManager == 0)
    {
        m_pGuiManager = g_pAppManager->m_pClientExoApp->GetSWGuiManager();
    }
    m_pMessageBox = new CSWGuiMessageBox(m_pGuiManager);
    m_pSkillInfoBox = new CSWGuiSkillInfoBox(m_pGuiManager);
    m_pTutorialBox = new CSWGuiTutorialBox(m_pGuiManager);
    m_pControllerLossBox = new CSWGuiControllerLossBox(m_pGuiManager);
}
// PC 0x00685D50
int CGuiInGame::Initialize()
{
    if (m_bItemCreateMenuAvailable == 0)
    {
        if (m_pGuiManager == 0)
        {
            m_pGuiManager = ClientExoAppLate2(g_pAppManager)->GetSWGuiManager();
        }
        m_pItemCreateMenu = new CSWGuiCreateItemMenu(m_pGuiManager);
        m_pLoadModuleDebugMenu = new CSWGuiLoadModuleDebugMenu(m_pGuiManager);
        m_pPowersFeatsSkillsDebugMenu = new CSWGuiPowersFeatsSkillsDebugMenu(m_pGuiManager);

        CClientExoApp* pClient = ClientExoAppLate2(g_pAppManager);
        pClient->SetLoadBarProgress((int)(pClient->GetLoadStep(1) +
            (pClient->GetLoadStep(0) + pClient->GetLoadStep(3) * 0.2f)), 1);
        UpdateScreen(1.0f / 30.0f, 0, 0);

        m_pSubItemCreateMenu = new CSWGuiCreateItemSubMenu(m_pGuiManager);
        m_pExamineBox = new CSWGuiExamine(m_pGuiManager);
        m_pBarkBubble = new CSWGuiBarkBubble(m_pGuiManager);
        m_pContainer = new CSWGuiContainer(m_pGuiManager);
        m_pCameraDialog = new CSWGuiDialogCinematic(m_pGuiManager);
        if (g_bCreateAllGUIs != 0)
        {
            m_pComputerDialog = new CSWGuiDialogComputer(m_pGuiManager);
        }
        m_pComputerScreen = new CSWGuiDialogComputerCamera(m_pGuiManager);
        m_pDialogTop = new CSWGuiDialogLetterbox(m_pGuiManager);
        if (m_pMessageBox == 0)
            m_pMessageBox = new CSWGuiMessageBox(m_pGuiManager);
        if (m_pSkillInfoBox == 0)
            m_pSkillInfoBox = new CSWGuiSkillInfoBox(m_pGuiManager);
        if (m_pTutorialBox == 0)
            m_pTutorialBox = new CSWGuiTutorialBox(m_pGuiManager);
        if (m_pControllerLossBox == 0)
            m_pControllerLossBox = new CSWGuiControllerLossBox(m_pGuiManager);

        pClient = ClientExoAppLate2(g_pAppManager);
        pClient->SetLoadBarProgress((int)(pClient->GetLoadStep(1) +
            (pClient->GetLoadStep(0) + pClient->GetLoadStep(3) * 0.4f)), 1);
        UpdateScreen(1.0f / 30.0f, 0, 0);

        m_pDialogLetterbox = new CSWGuiDialogLetterbox(m_pGuiManager);
        m_pDialogFade = new CSWGuiDialogLetterbox(m_pGuiManager);
        m_pGlobalFade = new CSWGuiFade(m_pGuiManager);
        m_pInGameMenu = new CSWGuiInGameMenu(m_pGuiManager);
        m_pPausePanel = new CSWGuiInGamePause(m_pGuiManager);
        m_pSoloModeQuery = new CSWGuiInGameSoloModeQuery(m_pGuiManager);
        m_pAreaTransition = new CSWGuiInGameAreaTransition(m_pGuiManager);
        if (g_bCreateAllGUIs != 0)
        {
            m_pPartySelectionPanel = new CSWGuiPartySelection(m_pGuiManager);
            m_pStorePanel = new CSWGuiStore(m_pGuiManager);
            m_apScreenPanels[SCREEN_EQUIP] = new CSWGuiInGameEquip(m_pGuiManager);
            m_apScreenPanels[SCREEN_INVENTORY] = new CSWGuiInGameInventory(m_pGuiManager);
        }

        pClient = ClientExoAppLate2(g_pAppManager);
        pClient->SetLoadBarProgress((int)(pClient->GetLoadStep(1) +
            (pClient->GetLoadStep(0) + pClient->GetLoadStep(3) * 0.6f)), 1);
        UpdateScreen(1.0f / 30.0f, 0, 0);

        if (g_bCreateAllGUIs != 0)
        {
            m_apScreenPanels[SCREEN_CHARACTER] = new CSWGuiInGameCharacter(m_pGuiManager);
        }
        m_pStatusSummary = new CSWGuiStatusSummary(m_pGuiManager);
        if (m_pPendingStatusSummary != 0)
        {
            m_pStatusSummary->m_cStatusSummary = *m_pPendingStatusSummary;
        }
        m_pMainInterface = new CSWGuiMainInterface(m_pGuiManager);

        pClient = ClientExoAppLate2(g_pAppManager);
        pClient->SetLoadBarProgress((int)(pClient->GetLoadStep(1) +
            (pClient->GetLoadStep(0) + pClient->GetLoadStep(3) * 0.8f)), 1);
        UpdateScreen(1.0f / 30.0f, 0, 0);

        if (g_bCreateAllGUIs != 0)
        {
            m_apScreenPanels[SCREEN_MAP] = new CSWGuiInGameMap(m_pGuiManager);
            m_apScreenPanels[SCREEN_ABILITIES] = new CSWGuiInGameAbilities(m_pGuiManager);
            m_apScreenPanels[SCREEN_JOURNAL] = new CSWGuiInGameJournal(m_pGuiManager);
            m_apScreenPanels[SCREEN_OPTIONS] = new CSWGuiInGameOptions(m_pGuiManager);
            m_apScreenPanels[SCREEN_MESSAGES] = new CSWGuiInGameMessages(m_pGuiManager);
            m_pGalaxyMap = new CSWGuiInGameGalaxyMap(m_pGuiManager);
        }

        ClientExoAppLate2(g_pAppManager)->SetLoadBarProgress((int)(
            (float)ClientExoAppLate2(g_pAppManager)->GetLoadStep(3) +
            (float)ClientExoAppLate2(g_pAppManager)->GetLoadStep(1) +
            (float)ClientExoAppLate2(g_pAppManager)->GetLoadStep(0)), 1);
        UpdateScreen(1.0f / 30.0f, 0, 0);
        m_pDialog = m_pCameraDialog;
        m_bItemCreateMenuAvailable = 1;
    }
    return m_bItemCreateMenuAvailable;
}
// PC 0x00688CF0
void CGuiInGame::HandleDialogSelection(int nReply, int bSelected)
{
    m_bDialogReplySelected = 0;
    if (nReply != DIALOG_SELECTION_CLOSE)
    {
        int nReplyIndex;
        if (nReply >= 0)
            nReplyIndex = m_pDialog->GetReplyIndex(nReply);
        else
            nReplyIndex = DIALOG_REPLY_INDEX_NONE;
        int bEscape = nReply == DIALOG_REPLY_ESCAPE;
        CSWCMessage cMessage;
        unsigned long oidPlayer = g_pAppManager->GetServerExoApp()->GetPlayerCreatureId();
        CSWSPlayer* pPlayer = g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(oidPlayer);
        unsigned long oidDialogOwner = m_pDialog->m_nOwner;
        unsigned long oidOwner = g_pAppManager->GetServerExoApp()->ClientToServerObjectId(oidDialogOwner);
        CGameObject* pObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidOwner);
        m_pDialog->SetDialogMessage(m_asReplyText[nReply]);
        if (bSelected)
            InitializeDialogAnimationData(m_pnReplyAnimations[nReply]);
        for (int nAnimation = 0; nAnimation < m_pnReplyAnimations[nReply]; ++nAnimation)
        {
            SetDialogAnimationData(nAnimation,
                m_ppnReplyAnimationIDs[nReply][nAnimation],
                m_ppnReplyAnimationFlags[nReply][nAnimation]);
        }

        unsigned char nFade = m_pnReplyValue11[nReply];
        if (nFade != DIALOG_FADE_NONE)
        {
            int bFadeIn = 1;
            if (nFade == DIALOG_FADE_OUT_ALTERNATE || nFade == DIALOG_FADE_OUT)
                bFadeIn = 0;
            const Vector& vColor = m_pvReplyValue12[nReply];
            float fLength = m_pfReplyValue14[nReply];
            float fDelay = m_pfReplyValue13[nReply];
            if (m_pDialogFade != 0)
                m_pDialogFade->SetFade(bFadeIn, fDelay, fLength, vColor);
        }

        if (bSelected)
        {
            UnlockDialogParticipantOrientations();
            unsigned long oidListener = m_pnReplyValue2[nReply];
            if (oidListener == OBJECT_INVALID)
            {
                m_oidPreviousDialogObject2 = m_oidDialogObject2;
                m_oidDialogObject2 = m_oidDialogObject1;
            }
            else
            {
                m_oidPreviousDialogObject2 = m_oidDialogObject2;
                m_oidDialogObject2 = oidListener;
            }
            unsigned long oidSpeaker = m_oidDialogCamera;
            m_oidPreviousDialogObject1 = m_oidDialogObject1;
            m_oidDialogObject1 = oidSpeaker;
            SetDialogAnimations();
            if (m_pDialog != 0 && m_pDialog == m_pCameraDialog)
            {
                if (SetAnimatedCamera(m_pnReplyValue8[nReply], m_pfReplyValue9[nReply]) == 0)
                {
                    if (m_bShotCameraInitialized == 0)
                        InitializeShotCamera();
                    UpdateShotCamera(m_pnReplyValue3[nReply], m_pnReplyValue4[nReply],
                                     m_pfReplyValue5[nReply], m_pfReplyValue6[nReply],
                                     m_pnReplyValue7[nReply], m_pnReplyValue10[nReply]);
                }
            }
            else if (m_pnReplyValue3[nReply] == DIALOG_SHOT_COMPUTER)
            {
                if (m_bShotCameraInitialized == 0)
                    InitializeShotCamera();
                SetComputerCameraShot(m_pnReplyValue4[nReply], m_pnReplyValue7[nReply],
                                      m_pnReplyValue10[nReply]);
            }
        }
        if (bEscape)
            CloseDialog();
        if (pObject != 0 && pObject->AsSWSObject() != 0)
        {
            unsigned long nReplyContext = m_pDialog->m_nReplyContext;
            unsigned long nPlayerId = pPlayer->GetPlayerId();
            pObject->AsSWSObject()->ReplyDialog(nPlayerId, nReplyIndex,
                                              bEscape, nReplyContext, 0);
        }
    }
    else
        CloseDialog();
}
// PC 0x00689080
void CGuiInGame::UpdateDialog()
{
    if (m_nGlobalDialogState == 0)
        return;
    if (m_bDeferredFOVCall == 1)
    {
        CAurCamera* pCamera = g_pAppManager->GetClientExoApp()->GetModule()->GetCamera();
        if (pCamera != 0)
        {
            pCamera->SetViewAngle(m_fDeferredFieldOfView);
            m_bDeferredFOVCall = 0;
        }
    }
    if (m_bDialogReplySelected == 0)
        return;
    unsigned long oidTarget = m_oidDialogObject1;
    int nReply = m_nDialogReply;
    if (m_pnReplyValue2 == 0)
        return;
    if (m_pnReplyValue2[nReply] != OBJECT_INVALID)
        oidTarget = m_pnReplyValue2[nReply];
    unsigned long oidSpeaker = m_oidDialogCamera;
    CSWCObject* pObject = g_pAppManager->GetClientExoApp()->GetGameObject(oidTarget);
    if (pObject == 0 || pObject->AsSWCObject() == 0)
        return;
    pObject = g_pAppManager->GetClientExoApp()->GetGameObject(oidSpeaker);
    if (pObject == 0 || pObject->AsSWCObject() == 0)
        return;
    for (int i = 0; i < (int)m_pnReplyAnimations[nReply]; ++i)
    {
        unsigned long oidAnimation = m_ppnReplyAnimationIDs[nReply][i];
        pObject = g_pAppManager->GetClientExoApp()->GetGameObject(oidAnimation);
        if (pObject == 0 || pObject->AsSWCObject() == 0)
            return;
    }
    AppendToDialogBuffer(m_asReplyText[m_nDialogReply], oidSpeaker);
    if (m_nDialogReplySelection != 0)
    {
        int nReplyIndex;
        if (m_nDialogReply >= 0)
            nReplyIndex = m_pDialog->GetReplyIndex(m_nDialogReply);
        else
            nReplyIndex = -1;
        int bEscape = m_nDialogReply == DIALOG_REPLY_ESCAPE;
        unsigned long oidPlayer = g_pAppManager->GetServerExoApp()->GetPlayerCreatureId();
        CSWSPlayer* pPlayer = g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(oidPlayer);
        unsigned long oidDialogOwner = m_pDialog->m_nOwner;
        unsigned long oidOwner = g_pAppManager->GetServerExoApp()->ClientToServerObjectId(oidDialogOwner);
        CGameObject* pServerObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidOwner);
        if (m_nC0 == 0)
        {
            int nAnimations = m_pnReplyAnimations[nReply];
            InitializeDialogAnimationData(nAnimations);
            for (int i = 0; i < nAnimations; ++i)
                SetDialogAnimationData(i, m_ppnReplyAnimationIDs[nReply][i],
                                       m_ppnReplyAnimationFlags[nReply][i]);
            SetDialogAnimations();
        }
        if (pServerObject != 0 && pServerObject->AsSWSObject() != 0)
        {
            unsigned long nReplyContext = m_pDialog->m_nReplyContext;
            unsigned long nPlayerId = pPlayer->GetPlayerId();
            pServerObject->AsSWSObject()->ReplyDialog(nPlayerId,
                nReplyIndex, bEscape, nReplyContext, 1);
        }
        m_bDialogReplySelected = 0;
    }
    else
        HandleDialogSelection(m_nDialogReply, m_nDialogReplyCount);
}
// PC 0x006893F0
void CGuiInGame::DoQuickLoad()
{
    CExoArrayList<CExoString> lstSaves;
    CExoString sSaveName;

    g_pExoBase->GetDirectoryList(&lstSaves, "SAVES:", RESTYPE_INVALID, 1, 1);
    if (!g_pExoResMan->CreateDirectory2("TEMP:"))
    {
        g_pExoResMan->CleanDirectory("TEMP:", 0, 0);
    }

    for (int nSave = 0; nSave < lstSaves.num; nSave++)
    {
        CSWGuiSaveLoadEntry* pEntry = new CSWGuiSaveLoadEntry;
        pEntry->LoadData(lstSaves[nSave]);
        if (pEntry->m_nSaveGame == CSWGuiSaveLoadEntry::SAVE_SLOT_QUICK)
        {
            g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_GENERIC);
            ClientExoAppLate2(g_pAppManager)->BusyWaitMuteSound();
            CClientExoApp* pClient = ClientExoAppLate2(g_pAppManager);

            if (pEntry->m_bMissingLiveContent && !g_bEnableLiveContentIgnore)
            {
                CSWGuiMessageBox* pMessageBox = ClientExoAppLate2(g_pAppManager)->GetInGameGui()->m_pMessageBox;
                pMessageBox->SetAllowCancel(0);
                pMessageBox->SetCallback(NULL, NULL, NULL);
                CExoString sMessage;
                sMessage = g_pTlkTable->GetSimpleString(CSWGuiSaveLoad::STRREF_MISSING_LIVE_CONTENT);
                g_pTlkTable->SetCustomToken(CSWGuiSaveLoad::MISSING_LIVE_CONTENT_TOKEN,
                                           pEntry->m_sMissingLiveContent);
                g_pTlkTable->ParseStr(sMessage);
                pMessageBox->SetMessage(sMessage);
                m_pGuiManager->AddPanel(pMessageBox, SWGUI_PANEL_FLAG_PUSH_MODAL, 1);
                return;
            }

            CSWPartyTable::ResetCurrentSessionStartTime();
            ClientExoAppLate2(g_pAppManager)->GetInGameGui()->SetGlobalUnpauseableState(1);
            pClient->SetLoadScreenByModuleName(pEntry->m_sLastModule);
            pClient->SetLoadMusic("");
            pClient->ShowLoadScreen(0, 1, 0);
            unsigned char nGameplayHint = pEntry->m_nGameplayHint;
            pClient->SetGamePlayHint(nGameplayHint);
            unsigned char nStoryHint = pEntry->m_nStoryHint;
            pClient->SetStoryHint(nStoryHint);
            pClient->SetLoadScreenHint(pClient->GetNextLoadScreenHintSTRREF(
                "", CClientExoAppInternal::LOAD_SCREEN_HINT_STORY));
            pClient->SetLoadScreenHintUpdateTimer(0);

            unsigned long nSaveGame = pEntry->m_nSaveGame;
            if (!UnpackQuickSaveGame(nSaveGame, pEntry->m_sGameName))
            {
                return;
            }
            while (m_pGuiManager->m_lModalPanels.num != 0)
            {
                m_pGuiManager->PopModalPanel();
            }
#ifdef _XBOX
            pClient->GetInGameGui()->HideSWInGameGui();
#else
            pClient->GetInGameGui()->HideSWInGameGui(1);
#endif
            ServerExoAppLate3(g_pAppManager)->SetLoadFromSaveGame(1);
            nSaveGame = pEntry->m_nSaveGame;
            ClientExoAppLate2(g_pAppManager)->SendLoadGameRequest(
                nSaveGame, pEntry->m_sGameName, pEntry->m_sLastModule);
            delete pEntry;
            return;
        }
        delete pEntry;
    }
    g_pExoResMan->RemoveResourceDirectory("TEMP:");
}
// PC 0x00689860
void CGuiInGame::ShowDialogReplies(unsigned long nStartIndex,
                                  unsigned long nReplyContext)
{
#ifndef _XBOX
    if (m_nReplies > 1u ||
        (m_nReplies > 0u && m_asReplyText[0] != ""))
    {
        g_pAppManager->GetClientExoApp()->m_pcExoAppInternal->ShowMouse(MOUSE_HIDE_ALL);
    }
#endif
    if (m_pDialog != 0)
    {
        unsigned long oidServerPlayer = g_pAppManager->GetServerExoApp()->GetPlayerCreatureId();
        CSWSPlayer* pServerPlayer = g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(oidServerPlayer);
        unsigned long oidServerCreature = pServerPlayer->m_oidPCObject;
        unsigned long oidPlayer = g_pAppManager->GetClientExoApp()->ServerToClientObjectId(oidServerCreature);
        if (m_nReplies == 1 && m_asReplyText[0] == "")
        {
            m_pDialog->SetReplies(m_nReplies, m_asReplyText, m_pnReplyValue1,
                                  nStartIndex, 0);
            m_pDialog->m_nReplyContext = nReplyContext;
            int nReplyCount = 1;
            m_pDialog->SetDialogMessage(m_asReplyText[0]);
            if (m_asReplyText[0].IsEmpty())
                nReplyCount = 0;
            m_nDialogReply = 0;
            m_nDialogReplyCount = nReplyCount;
            m_nDialogReplySelection = 0;
            m_bDialogReplySelected = 1;
            UpdateDialog();
            m_pDialog->SetActiveControl(0, 0);
            return;
        }
        m_pDialog->SetReplies(m_nReplies, m_asReplyText, m_pnReplyValue1,
                              nStartIndex, 1);
        m_pDialog->m_nReplyContext = nReplyContext;
        UnlockDialogParticipantOrientations();
        int bPlayerInParty = 0;
        CSWCCreature* pPlayer = g_pAppManager->GetClientExoApp()->GetCreatureByGameObjectID(oidPlayer);
        if (pPlayer != 0 && pPlayer->m_bInParty)
            bPlayerInParty = 1;
        m_oidPreviousDialogObject2 = m_oidDialogObject2;
        m_oidDialogObject2 = m_oidDialogObject1;
        CSWCCreature* pCharacter = g_pAppManager->GetClientExoApp()->GetSWParty()->GetCharacter(0);
        if (bPlayerInParty && !m_bUsePartyCharacterForDialog)
        {
            m_oidPreviousDialogObject1 = m_oidDialogObject1;
            m_oidDialogObject1 = oidPlayer;
        }
        else if (pCharacter != 0)
        {
            unsigned long oidCharacter = pCharacter->GetObjectId();
            m_oidPreviousDialogObject1 = m_oidDialogObject1;
            m_oidDialogObject1 = oidCharacter;
        }
        else
        {
            m_oidPreviousDialogObject1 = m_oidDialogObject1;
            m_oidDialogObject1 = oidPlayer;
        }
        if (m_pDialog != 0 && m_pDialog == m_pCameraDialog)
        {
            if (!m_bShotCameraInitialized)
                InitializeShotCamera();
            UpdateShotCamera(m_pnReplyValue3[0], m_pnReplyValue4[0],
                             m_pfReplyValue5[0], m_pfReplyValue6[0],
                             m_pnReplyValue7[0], m_pnReplyValue10[0]);
        }
    }
}
// PC 0x00689AC0
int CGuiInGame::HandleDialogReplies(
    unsigned long oidPlayer, unsigned long oidSpeaker,
    unsigned char nComputerType, CExoString* psText,
    unsigned long* pnValue1, unsigned long* pnValue2,
    unsigned long* pnValue3, int* pnValue4, float* pfValue5,
    float* pfValue6, unsigned long* pnValue7, unsigned short* pnValue8,
    float* pfValue9, int* pnValue10, unsigned char* pnValue11,
    Vector* pvValue12, float* pfValue13, float* pfValue14,
    unsigned long nEntries, unsigned long nReplies,
    unsigned long nReplyContext, unsigned long* pnAnimations,
    unsigned long** ppnAnimationIDs, unsigned short** ppnAnimationFlags,
    int bUnknown)
{
    unsigned long nTotalReplies = nEntries + nReplies;
    if (nTotalReplies > 0)
    {
        CExoString sReply;
        InitializeRepliesData(nTotalReplies);
        for (unsigned long nReply = 0; nReply < nTotalReplies; ++nReply)
        {
            sReply = psText[nReply];
            if (!sReply.IsEmpty())
            {
                unsigned long oidPC = g_pAppManager->m_pServerExoApp->GetPlayerCreatureId();
                CSWSPlayer* pPlayer = g_pAppManager->m_pServerExoApp->GetClientObjectByObjectId(oidPC);
                unsigned long oidServer = pPlayer->m_oidPCObject;
                CClientExoApp* pConversionApp = g_pAppManager->m_pClientExoApp;
                unsigned long oidClient = pConversionApp->ServerToClientObjectId(oidServer);
                CClientExoApp* pObjectApp = g_pAppManager->m_pClientExoApp;
                CSWCObject* pObject = pObjectApp->GetGameObject(oidClient);
                g_pSWTlkTable->ParseStr(sReply, (CGameObject*)pObject,
                                       nReply > nEntries, 0);
            }
            SetReplyData(nReply, sReply, pnValue1[nReply], pnValue2[nReply],
                         pnValue3[nReply], pnValue4[nReply], pfValue5[nReply],
                         pfValue6[nReply], pnValue7[nReply], pnValue8[nReply],
                         pfValue9[nReply], pnValue10[nReply], pnValue11[nReply],
                         pvValue12[nReply], pfValue13[nReply], pfValue14[nReply],
                         pnAnimations[nReply], ppnAnimationIDs[nReply],
                         ppnAnimationFlags[nReply]);
        }
    }
    CleanUpEntryNode();
    ShowDialogReplies(nReplies, nReplyContext);
    return 1;
}

static inline void GuiInGameTrace()
{
}

static inline CClientExoApp* ClientExoAppLate2(CAppManager* p)
{
    return ClientExoAppLate1(p);
}

static inline CClientExoApp* ClientExoAppLate1(CAppManager* p)
{
    return p->m_pClientExoApp;
}

static inline CServerExoApp* ServerExoAppLate3(CAppManager* p)
{
    return ServerExoAppLate2(p);
}

static inline CServerExoApp* ServerExoAppLate2(CAppManager* p)
{
    return ServerExoAppLate1(p);
}

static inline CServerExoApp* ServerExoAppLate1(CAppManager* p)
{
    return p->m_pServerExoApp;
}

static inline void GuiingameTrace2()
{
    GuiingameTrace1();
}

static inline void GuiingameTrace1()
{
}

#ifdef _XBOX

void CGuiInGame::ClearDialogReplyText()
{
    CExoString sBlank(" ");
    AurPostString(sBlank.CStr(), 5, 15, 5.0f);
    for (int i = 0; i < 5; i++)
    {
        AurPostString(sBlank.CStr(), 5, i + 16, 5.0f);
    }
    m_nDialogReplyRow = 16;
}
#endif

#ifdef _XBOX

void CGuiInGame::ChangeCharacter()
{
    m_pMainInterface->StartRotation();
}
#endif

#ifdef _XBOX

void CGuiInGame::UpdateAreaTransitionGUI(unsigned long oidArea,
                                         const Vector& vPosition)
{
    CSWCModule* pModule;
    CSWSArea* pServerArea;
    if (g_pAppManager->m_pClientExoApp->GetModule() != 0 &&
        (pModule = g_pAppManager->m_pClientExoApp->GetModule())->m_pInfo != 0 &&
        (pServerArea = pModule->m_pInfo->GetServerArea()) != 0 &&
        oidArea != OBJECT_INVALID &&
        m_bShowPauseGui == GUISTATE_MAIN_INTERFACE)
    {
        if (pServerArea->m_bTransitionPending != 0)
        {
            if (m_pGuiManager->PanelExists(m_pAreaTransition))
                m_pGuiManager->SendPanelToBack(m_pAreaTransition);
        }
        else if (m_bItemCreateMenuAvailable != 0 && m_pGuiManager != 0 &&
                 m_pAreaTransition != 0)
        {
            m_pAreaTransition->SetTransitionObject(oidArea, vPosition);
            if (!m_pGuiManager->PanelExists(m_pAreaTransition))
            {
                m_pGuiManager->AddPanel(m_pAreaTransition,
                    SWGUI_PANEL_MODE_OVERLAY, 1);
                m_pGuiManager->SendPanelToBack(m_pAreaTransition);
            }
        }
    }
    else
        HideAreaTransition();
}

unsigned long CGuiInGame::GetAreaTransitionObject()
{
    if (m_pAreaTransition == NULL)
    {
        return OBJECT_INVALID;
    }
    return m_pAreaTransition->m_oidTransitionObject;
}
#endif

#ifdef _XBOX

int CGuiInGame::IsGlobalFadeObscuring()
{
    GUIINGAME_TRACE();
    return m_pGlobalFade->IsObscuring();
}
#endif

#ifndef _XBOX
void CGuiInGame::StartCreditSequence(const Vector& vColor, const CExoString& sMusic)
{
}
#endif

#ifdef _XBOX
#include "nwcambientsound.h"

void CGuiInGame::StartCreditSequence(const Vector& vColor, const CExoString& sMusic)
{
    if (!m_bCreditSequenceInProgress)
    {
        CSWGuiInGameCredits* pCredits = new CSWGuiInGameCredits(m_pGuiManager);
        if (pCredits != NULL)
        {
            if (!m_pGuiManager->PanelExists(pCredits))
            {
                m_pGuiManager->AddPanel(pCredits, CSWGuiManager::PANEL_MODE_MODAL, 1);
            }
            pCredits->m_vColor = vColor;
            g_pAppManager->m_pClientExoApp->GetModule()->m_pInfo->GetAmbientSound()->StopSounds(0);
            if (m_pCreditsMusic != 0)
            {
                delete m_pCreditsMusic;
                m_pCreditsMusic = 0;
            }
            m_pCreditsMusic = new CExoStreamingSoundSource;
            if (m_pCreditsMusic != 0)
            {
                m_pCreditsMusic->SetPriorityGroup(CExoStreamingSoundSource::PRIORITY_GROUP_DIALOG_AMBIENT);
                if (m_nXboxUnmappedC0C == 1)
                {
                    m_pCreditsMusic->Play(sMusic.CStr(), 11, 0, 1, 0);
                }
            }
            m_bCreditSequenceInProgress = 1;
        }
    }
}

void CGuiInGame::PostDialogConversationText(const CExoString& sConversation)
{
    if (m_nXboxUnmappedB98)
    {
        CExoString sText;
        sText.Format("Conv: %s", sConversation.CStr());
        AurPostString(sText.CStr(), 5, 15, 10000.0f);
    }
}

void CGuiInGame::PostDialogSoundText(const CExoString& sSound)
{
    if (m_nXboxUnmappedB98)
    {
        int nRow = m_nDialogReplyRow++;
        if (m_nDialogReplyRow - 16 >= 5)
        {
            m_nDialogReplyRow = 16;
        }
        int nClearRow = nRow + 1;
        if (nClearRow >= 21)
        {
            nClearRow = 16;
        }
        CExoString sText;
        sText.Format(" ");
        AurPostString(sText.CStr(), 5, nClearRow, 5.0f);
        if (nRow == 16)
        {
            sText.Format("Audio/Lip: - %s", sSound.CStr());
        }
        else
        {
            sText.Format("           - %s", sSound.CStr());
        }
        AurPostString(sText.CStr(), 5, nRow, 10000.0f);
    }
}

int CGuiInGame::HandleDialogSound(unsigned long oidSpeaker, const CResRef& cSound)
{
    GUIINGAME_TRACE();
    CExoString sSound;
    cSound.CopyToString(sSound);
    PostDialogSoundText(sSound);
    CSWCObject* pObject = g_pAppManager->m_pClientExoApp->GetGameObject(
        m_oidDialogObject1);
    if (pObject == 0 || pObject->AsSWCObject() == 0)
        return 0;
    CSWCObject* pClientObject = pObject->AsSWCObject();
    if (cSound.IsValid() && pClientObject != 0)
    {
        if (m_pDialogSound != 0)
        {
            m_pDialogSound->Stop();
            delete m_pDialogSound;
        }
        m_pDialogSound = new CExoStreamingSoundSource;
        m_pDialogSound->SetPriorityGroup(
            CExoStreamingSoundSource::PRIORITY_GROUP_DIALOG_VOICE);
        if (m_pDialogSound->Play(cSound, 11, 1, 0, 0) == 0 &&
            m_pDialogSound->Play(cSound, RESTYPE_WAV, 1, 0, 0) == 0)
        {
            return 0;
        }
        int nDebugOption = g_pExoInput->GetDebugOption();
        if (nDebugOption == 3)
        {
            m_pDialogSound->SetFixedVariance(1.5f);
        }
        else if (nDebugOption == 1)
        {
            m_pDialogSound->SetFixedVariance(0.75f);
        }
        SetDialogSound(oidSpeaker, cSound);
    }
    m_oidDialogSoundObject = pClientObject->m_nId;
    return 1;
}

const char* ShowGUI(char* sCommand)
{
    if (*sCommand == 0)
    {
        return "need to specify the GUI to create";
    }
    HideGUI("");
    g_pDynamicGUI = new CSWGuiDynamic(
        g_pAppManager->m_pClientExoApp->GetSWGuiManager(), sCommand);
    g_pAppManager->m_pClientExoApp->GetSWGuiManager()->AddPanel(
        g_pDynamicGUI, 0, 1);
    g_pAppManager->m_pClientExoApp->SetInputClass(2);
    return "";
}

const char* creditsinprogress(char* sCommand)
{
    return g_pAppManager->m_pClientExoApp->GetInGameGui()->m_bCreditSequenceInProgress ? "True" : "False";
}

const char* showcredits(char* sCommand)
{
    Vector vColor(0.0f, 0.0f, 0.0f);
    sscanf(sCommand, "%f %f %f", &vColor.m_fX, &vColor.m_fY, &vColor.m_fZ);
    CExoString sMusic("");
    g_pAppManager->GetClientExoApp()->GetInGameGui()->StartCreditSequence(vColor, sMusic);
    return "Success";
}

int CGuiInGame::SetAnimatedCamera(unsigned short nAnimation, float fFieldOfView)
{
    GUIINGAME_TRACE();
    if (m_bAnimatedCameraLoaded != 0 &&
        nAnimation >= CUTSCENE_ANIMATION_FIRST &&
        nAnimation <= CUTSCENE_ANIMATION_WL_LAST)
    {
        CSWCModule* pModule = g_pAppManager->m_pClientExoApp->GetModule();
        if (pModule != 0)
        {
            unsigned long oidCamera = m_oidCameraAnimationObject;
            m_bAnimatedCameraActive = 1;
            m_bShotCameraInitialized = 0;
            m_nDialogCameraAnimation = nAnimation;
            CGameObject* pObject = g_pAppManager->m_pServerExoApp->GetGameObject(oidCamera);
            if (pObject != 0 && pObject->AsSWSObject() != 0)
            {
                pObject->AsSWSObject()->TimeStampCameraAnimation();
            }
            unsigned long oidTarget = m_oidDialogObject1;
            m_fAnimatedCameraTime = pModule->SetAnimatedCamera(
                nAnimation, fFieldOfView, oidTarget);
            return 1;
        }
        EXOXBOXASSERT(0);
    }
    return 0;
}
#endif
