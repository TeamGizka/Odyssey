#include <aurlist.h>
#include "clientexoappinternal.h"
#include "swbehaviorcamera.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "clientoptions.h"
#include "gameobjectarray.h"
#include "guiingame.h"
#include "exobase.h"
#include "nwcjournal.h"
#include "nwcmodule.h"
#include "nwmessage.h"
#include "swparty.h"
#include "swplayercontrol.h"
#include "swguiloadscreen.h"
#include "swguimaininterfacepc2.h"
#include "swguilegalscreen.h"
#include "serverexoapp.h"
#include "swgui.h"
#include "nwccreature.h"
#include "nwccreatureappearance.h"
#include "nwsobject.h"
#include "nwsplayer.h"
#include "nwsdialog.h"
#include "math3d.h"
#include "nwtlktable.h"
#include "serverinfo.h"
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include "aurora.h"
#include "aurinternal.h"
#include "exoinput.h"
#include "exobase.h"
#include "nwreentrantserver.h"
#include "nwcarea.h"
#include "swcroom.h"
#include "aurscene.h"
#include "partytables.h"
#include "nwcitem.h"
#include "nwcanimbase.h"
#include "nwscript.h"
#include "nwitem.h"
#include "pathfindinfo.h"
#include "swguiingame.h"
#include "nwcambientsound.h"
#include "swmain.h"
#include "swguimainmenu.h"
#include "aurcamera.h"
#include "nwrules.h"
#include "twodimarrays.h"
#include "twodimarray.h"
#include "swguipazaak.h"
#include "swpattern.h"
#include "aurtexman.h"
#include "swminigame.h"
#include "nwcvisualeffect.h"
#include "nwcdoor.h"
#include "nwcplaceable.h"
#include "nwctrigger.h"
#include "nwsdoor.h"
#include "nwsplaceable.h"
#include "clientaimaster.h"
#include "soundset.h"
#include "netlayer.h"
#include <locale.h>
#include <time.h>
#include "swguiingametutorialbox.h"
#include "nwsmodule.h"
#include "nwsarea.h"
#include "nwscreature.h"
#include "nwdoorsurfacemesh.h"
#include "nwspell.h"
#include "nwfeat.h"
#include "nwskill.h"
#ifdef _XBOX

#define VIDEO_DEFAULT_DYNAMIC_LIGHTS 8
#define OPTIONS_SIGNATURE_SIZE 20
#include "swguistartscreen.h"
#include "exoinputinternal.h"

#define RESTYPE_XMV 13
enum
{
    MOVIE_SKIP_EVENT_FIRST = 16,
    MOVIE_SKIP_EVENT_SECOND = 17,
    MOVIE_SKIP_EVENT_THIRD = 73,
    MOVIE_SKIP_EVENT_FOURTH = 74,
    MOVIE_SKIP_COOLDOWN = 250,
    INPUT_CLASS_MOVIE = 5,
    STRREF_MOVIE_LOADING = 47863,
    STRREF_MOVIE_SKIPPING = 47864
};
#endif

FILE* eventDumpFile = 0;
int g_bLoggedIn = 0;

int g_nClientUnused1 = 0;
int g_bEnableScreenCapture = 1;
int g_bUseJoystick = 1;
static int printanimqueue = 1;

int g_nAnimQueueSize = 3;
float g_fAnimQueueTime = 3.0f;
int g_bFirst = 1;
static float g_fMicPosFromCam = 0.75f;
int g_nSetMovieFrameRate = 0;
float g_AIUpdateTime = 0;
float g_AnimationUpdateTime = 0;
float g_RenderUpdateTime = 0;
float g_GuiUpdateTime = 0;
float g_SndUpdateTime = 0;
int SlowGame = 0;
int g_bCritFileFail = 0;
int CameraVertAxisFlip = 1;
int g_bShowIndicator = 1;
int CameraHorzAxisFlip = 0;
unsigned long LockRenderDev = 0;

int g_nClientUnused2 = 0;

int g_nConnectType = CNetLayer::CONNECTTYPE_PLAYER;

static unsigned __int64 g_starttime;
static unsigned __int64 g_endtime;
CExoString g_sErrorString;
// PC 0x00640B60 GOG 0x00640A10
int CClientExoAppInternal::Initialize()
{
#ifndef _XBOX
    if (m_pGuiLoadScreen == NULL)
    {
        m_pGuiLoadScreen = new CSWGuiLoadScreen(m_pSWGuiManager);
    }
#endif

    if (g_pRules == NULL)
    {
        g_pRules = new CSWRules();
    }

    if (g_pRules != NULL)
    {
        for (unsigned char nMaterial = 0;
             nMaterial < g_pRules->Get2DArrays()->GetSurfaceMaterial()->GetNumRows();
             nMaterial++)
        {
            int bWalk;
            g_pRules->Get2DArrays()->GetSurfaceMaterial()->GetINTEntry(nMaterial, "Walk", &bWalk);
            m_nHitCheckMaterialMask |= bWalk << nMaterial;
        }
    }

    m_pGuiInGame->InitializeGlobal();
    SetEventDescriptions();

    if (m_pMoviePlayer != NULL && m_pMoviePlayer->IsMoviePlaying())
    {
        m_pGuiInGame->SetGlobalUnpauseableState(1);
        g_pExoInput->ClearEvents();
#ifdef _XBOX
        g_pExoInput->CoolDownEvent(16, 250);
        g_pExoInput->CoolDownEvent(17, 250);
#endif
        g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_PAUSE_ALL);
    }

    m_bClientInitialized = 1;
    return 1;
}
// PC 0x00643930 GOG 0x006437E0
CWorldTimer* CClientExoAppInternal::GetActiveTimer(unsigned long oidObject)
{
    if (oidObject != OBJECT_INVALID && GetActivePauseState() > 0)
    {
        if (GetActivePauseState() == PAUSE_STATE_COMBAT)
        {
            if (IsOnExclusionList(oidObject) == 1)
                return m_pPauseTimer;
        }
        else if (GetActivePauseState() == PAUSE_STATE_PAUSED)
        {
            if (IsOnExclusionList(oidObject) == 1)
                return m_pCombatTimer;
        }
        else
            return NULL;
    }
    return m_pWorldTimer;
}
// PC 0x0069E520 GOG 0x0069E300
CGuiAreaLoadscreen* CClientExoAppInternal::StartLoadBar(int, char*, int)
{
    return 0;
}
// PC 0x00406600 GOG 0x004065D0
CSWCModule* CClientExoAppInternal::GetModule()
{
    return m_pModule;
}
// PC 0x006396D0 GOG 0x006395D0
CExoString CSWCModule::GetModuleTitle()
{
    return m_sModuleTitle;
}
// PC 0x006396F0 GOG 0x006395F0
inline CExoImeUI::CExoImeUI()
    : m_sResult(),
      m_sComposition(),
      m_sCandidate()
{
    m_nCompositionActive = 0;
    m_nUnknown104 = 0;
}
// PC 0x00639770 GOG 0x00639670
void CSWCPlayer::SetPrimaryPCPortrait(CResRef cPortrait)
{
    m_cPrimaryPCPortrait.CResRef::operator=(cPortrait);
}
// PC 0x00639790 GOG 0x00639690
char* SetFramesPerSecond(char* szFrameRate)
{
    int nFrameRate = atoi(szFrameRate);

    if (nFrameRate != 0)
    {
        g_pAppManager->m_pClientExoApp->GetWorldTimer()->SetFixedSnapshotRate(
            1, nFrameRate);
        g_pAppManager->m_pClientExoApp->GetPauseTimer()->SetFixedSnapshotRate(
            1, nFrameRate);
        g_pAppManager->m_pClientExoApp->GetAnimationTimer()
            ->SetFixedSnapshotRate(1, nFrameRate);
        g_pAppManager->m_pServerExoApp->GetWorldTimer()->SetFixedSnapshotRate(
            1, nFrameRate);
        g_pAppManager->m_pServerExoApp->GetPauseTimer()->SetFixedSnapshotRate(
            1, nFrameRate);

        return "Frame rate set.";
    }

    g_pAppManager->m_pClientExoApp->GetWorldTimer()->SetFixedSnapshotRate(0, 0);
    g_pAppManager->m_pClientExoApp->GetPauseTimer()->SetFixedSnapshotRate(0, 0);
    g_pAppManager->m_pClientExoApp->GetAnimationTimer()->SetFixedSnapshotRate(
        0, 0);
    g_pAppManager->m_pServerExoApp->GetWorldTimer()->SetFixedSnapshotRate(0, 0);
    g_pAppManager->m_pServerExoApp->GetPauseTimer()->SetFixedSnapshotRate(0, 0);

    return "Fixed frame rate off.";
}
// PC 0x006398B0 GOG 0x006397B0
void CSWCPlayer::SetPrimaryPCPortraitId(unsigned short nPortraitId)
{
    CExoString sPrefix;
    CExoString sBaseResRef;

    if (nPortraitId != PORTRAIT_ID_RESERVED_FFFF)
    {
        sPrefix = "po_";
        {
            CExoString sColumn("BaseResRef");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pPortraits = p2DArrays->m_pPortrait;
            pPortraits->GetCExoStringEntry(nPortraitId, sColumn, &sBaseResRef);
        }
        sBaseResRef = sPrefix + sBaseResRef;
#ifdef _XBOX
        SetPrimaryPCPortrait(sBaseResRef.CStr());
#else
        {
            CResRef cPortrait((char*)sBaseResRef.CStr());
            m_cPrimaryPCPortrait.CResRef::operator=(cPortrait);
        }
#endif
    }

    m_nPrimaryPCPortraitId = nPortraitId;
}
// PC 0x006399E0 GOG 0x006398E0
CAurCamera* CClientExoAppInternal::GetModuleCamera()
{
    if (m_pModule != NULL)
    {
        return m_pModule->m_pModuleCamera;
    }

    return 0;
}

void CClientExoAppInternal::ShutDownLegalScreen()
{
    if (m_pLegalScreen != NULL)
    {
        m_pLegalScreen->SetImage("");
        m_pSWGuiManager->RemovePanel(m_pLegalScreen);
        if (m_pLegalScreen != NULL)
        {
            delete m_pLegalScreen;
        }
        m_pLegalScreen = NULL;
    }
}
// PC 0x006399F0 GOG 0x006398F0
int CClientExoAppInternal::HandleMessage(unsigned long nPlayerId,
                                         unsigned char* pMessage,
                                         unsigned long nSize, int bGuaranteed)
{
    if (g_pAppManager->GetClientExoApp()->GetCaptureEvents() != 0 &&
        g_pAppManager->GetClientExoApp()->GetCaptureEventFile() != NULL)
    {
        CExoString sEvent;

        sEvent.Format("%d %c %d %d %d ",
                      (unsigned int)(g_pAppManager->GetClientExoApp()
                                         ->GetWorldTimer()
                                         ->GetSnapshotTime() /
                                     1000),
                      'M', nPlayerId, bGuaranteed, nSize);
        g_pAppManager->GetClientExoApp()->GetCaptureEventFile()->Write(sEvent);
        g_pAppManager->GetClientExoApp()->GetCaptureEventFile()->Write(
            pMessage, nSize, 1);
        sEvent.Format("\n");
        g_pAppManager->GetClientExoApp()->GetCaptureEventFile()->Write(sEvent);
    }

    if (bGuaranteed == 0)
    {
        if (*pMessage == 'P')
        {
            m_pSWCMessage->HandleServerToPlayerMessage(pMessage, nSize);
        }
        if (*pMessage == 'S')
        {
            m_pUnknown1C0->HandleServerToSysAdminMessage(pMessage, nSize);
        }
    }
    return 1;
}
// PC 0x00639B50 GOG 0x00639A50
void CClientExoAppInternal::UpdateRoom(const Vector& vPosition)
{
    if (m_pModule != NULL)
    {
        CSWCArea* pArea = m_pModule->m_pInfo;
        if (pArea != NULL)
        {
            CAurScene* pScene = pArea->m_pScene;
            if (pScene != NULL && !pScene->GetIgnoreVisibilityGraph())
            {
                int nRoom = pArea->GetRoomIndex(vPosition, 0);
                CSWCRoom* pRoom = pArea->GetRoom(nRoom);

                if (nRoom < 0)
                {
                    g_pAppManager->m_pClientExoApp->GetInGameGui();
                    return;
                }

#ifdef _XBOX
                int nXboxRoomIndex = pRoom->m_nRoomIndex;
                EXOXBOXASSERT(nXboxRoomIndex >= 0);
                pScene->SetCurrentRoom(nXboxRoomIndex);
#else
                pScene->SetCurrentRoom(pRoom->m_nRoomIndex);
#endif
                if (pRoom != NULL)
                {
                    g_pExoSound->SetDesiredEnvironment(pRoom->m_nEnvAudio);
                    float fScale = pRoom->m_fEnvAudioVolume;
                    g_pExoSound->SetDesiredAmbientScale(fScale);
                }
            }
        }
    }
}
// PC 0x00639BE0 GOG 0x00639AE0
int CClientExoAppInternal::GetPauseState(unsigned char nPauseType)
{
    return nPauseType & m_PauseState.m_nPauseState;
}
// PC 0x00639C00 GOG 0x00639B00
unsigned char CClientExoAppInternal::GetActivePauseState()
{
    if (GetPauseState(PAUSE_TYPE_COMBAT_MASK))
        return PAUSE_STATE_COMBAT;
    else if (GetPauseState(PAUSE_TYPE_PAUSED_MASK))
        return PAUSE_STATE_PAUSED;
    return 0;
}
// PC 0x0065D0F0 GOG 0x0065CF00
int CClientExoAppInternal::ContinueMessageProcessing()
{
    return 1;
}
// PC 0x00639C10 GOG 0x00639B10
void CClientExoAppInternal::CreateQuestJournal()
{
    if (m_pQuestJournal == NULL)
    {
        m_pQuestJournal = new CSWCJournal;
    }
}
// PC 0x00639C80 GOG 0x00639B80
void CClientExoAppInternal::CreateWorldJournal()
{
    if (m_pWorldJournal == NULL)
    {
        m_pWorldJournal = new CSWCWorldJournal;
    }
}

void CClientExoAppInternal::DestroyWorldJournal()
{
    if (m_pWorldJournal != NULL)
    {
        delete m_pWorldJournal;
        m_pWorldJournal = NULL;
    }
}
// PC 0x00639CF0 GOG 0x00639BF0
void CClientExoAppInternal::JournalUpdated(int nType, int nIndex,
                                           const CExoString& sName)
{
    if (m_pQuestJournal != NULL)
    {
        m_pQuestJournal->JournalUpdated(nType, nIndex, sName);
    }
}

#ifndef _XBOX
void CClientExoAppInternal::ExitProgram()  // PC 0x00639D00 GOG 0x00639C00
{
    PostQuitMessage(0);
}
#endif
// PC 0x00639D10 GOG 0x00639C10
CSWCModule* CClientExoAppInternal::CreateModule()
{
    return m_pModule = new CSWCModule;
}

#ifdef _XBOX
int CClientExoAppInternal::SendSaveGameRequest(unsigned long nPlayerId,
                                               const CExoString& sSaveName)
{
    if (m_pSWCMessage != NULL)
    {
        return m_pSWCMessage->SendPlayerToServerModule_SaveGame(
            nPlayerId, sSaveName);
    }

    return 0;
}
#else
int CClientExoAppInternal::SendSaveGameRequest(unsigned long nPlayerId,  // PC 0x00639D80 GOG 0x00639C80
                                               const CExoString& sSaveName,
                                               const CExoString& sSaveDir)
{
    if (m_pSWCMessage != NULL)
    {
        return m_pSWCMessage->SendPlayerToServerModule_SaveGame(
            nPlayerId, sSaveName, sSaveDir);
    }

    return 0;
}
#endif
// PC 0x00639DA0 GOG 0x00639CA0
int CClientExoAppInternal::SendLoadGameRequest(unsigned long nPlayerId,
                                               const CExoString& sSaveName,
                                               const CExoString& sSaveDir)
{
    if (m_pSWCMessage != NULL)
    {
        return m_pSWCMessage->SendPlayerToServerModule_LoadGame(
            nPlayerId, sSaveName, sSaveDir);
    }

    return 0;
}
// PC 0x00639DC0 GOG 0x00639CC0
void CClientExoAppInternal::ServerModeChange(unsigned char nServerMode)
{
    m_nServerMode = nServerMode;
}
// PC 0x00639DD0 GOG 0x00639CD0
void CClientExoAppInternal::SetCaptureEventPlayBackRate(int nRate)
{
    m_nCaptureEventPlayBackRate = nRate;
    CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;

    pClientExoApp->GetWorldTimer()->SetFixedSnapshotRate(1, nRate);
}
#ifndef _XBOX
// PC 0x00639E00 GOG 0x00639D00
void CClientExoAppInternal::SetEventDescriptions()
{
    g_pExoInput->ClearAllInputEventDescriptions();
    g_pExoInput->CreateNewEvent(INPUT_KEY_RETURN, CExoInput::EVENT_TYPE_BUTTON,
        CExoInput::INPUTDEVICE_KEYBOARD, CExoInput::KEYBOARD_RETURN, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(INPUT_KEY_RETURN, INPUT_CONTEXT_PC_GUI);
    g_pExoInput->AddEvent(INPUT_KEY_RETURN, INPUT_CONTEXT_DIALOG);
    g_pExoInput->AddEvent(INPUT_KEY_RETURN, INPUT_CONTEXT_FREE_LOOK);
    g_pExoInput->CreateNewEvent(INPUT_KEY_UP, CExoInput::EVENT_TYPE_BUTTON,
        CExoInput::INPUTDEVICE_KEYBOARD, CExoInput::KEYBOARD_UP_ARROW, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(INPUT_KEY_UP, INPUT_CONTEXT_PC_GUI);
    g_pExoInput->AddEvent(INPUT_KEY_UP, INPUT_CONTEXT_DIALOG);
    g_pExoInput->CreateNewEvent(INPUT_KEY_DOWN, CExoInput::EVENT_TYPE_BUTTON,
        CExoInput::INPUTDEVICE_KEYBOARD, CExoInput::KEYBOARD_DOWN_ARROW, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(INPUT_KEY_DOWN, INPUT_CONTEXT_PC_GUI);
    g_pExoInput->AddEvent(INPUT_KEY_DOWN, INPUT_CONTEXT_DIALOG);
    g_pExoInput->CreateNewEvent(INPUT_KEY_LEFT, CExoInput::EVENT_TYPE_BUTTON,
        CExoInput::INPUTDEVICE_KEYBOARD, CExoInput::KEYBOARD_LEFT_ARROW, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(INPUT_KEY_LEFT, INPUT_CONTEXT_PC_GUI);
    g_pExoInput->AddEvent(INPUT_KEY_LEFT, INPUT_CONTEXT_DIALOG);
    g_pExoInput->CreateNewEvent(INPUT_KEY_RIGHT, CExoInput::EVENT_TYPE_BUTTON,
        CExoInput::INPUTDEVICE_KEYBOARD, CExoInput::KEYBOARD_RIGHT_ARROW, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(INPUT_KEY_RIGHT, INPUT_CONTEXT_PC_GUI);
    g_pExoInput->AddEvent(INPUT_KEY_RIGHT, INPUT_CONTEXT_DIALOG);
#ifndef _XBOX
    g_pExoInput->CreateNewEvent(INPUT_KEY_NUMPADENTER, CExoInput::EVENT_TYPE_BUTTON,
        CExoInput::INPUTDEVICE_KEYBOARD, CExoInput::KEYBOARD_NUMPADENTER, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(INPUT_KEY_NUMPADENTER, INPUT_CONTEXT_PC_GUI);
    g_pExoInput->AddEvent(INPUT_KEY_NUMPADENTER, INPUT_CONTEXT_DIALOG);
#endif
    g_pExoInput->CreateNewEvent(INPUT_SCREENSHOT, CExoInput::EVENT_TYPE_BUTTON,
        CExoInput::INPUTDEVICE_KEYBOARD, CExoInput::KEYBOARD_PRINTSCREEN, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(INPUT_SCREENSHOT, INPUT_CONTEXT_PC);
    g_pExoInput->AddEvent(INPUT_SCREENSHOT, INPUT_CONTEXT_PC_GUI);
    g_pExoInput->AddEvent(INPUT_SCREENSHOT, INPUT_CONTEXT_DIALOG);
    g_pExoInput->AddEvent(INPUT_SCREENSHOT, INPUT_CONTEXT_FREE_LOOK);
    g_pExoInput->AddEvent(INPUT_SCREENSHOT, INPUT_CONTEXT_MINIGAME);
    g_pExoInput->CreateNewEvent(INPUT_JOYSTICK_Y, CExoInput::EVENT_TYPE_RELATIVE,
        CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_YAXIS, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(INPUT_JOYSTICK_Y, INPUT_CONTEXT_PC);
    g_pExoInput->ScaleEvent(INPUT_JOYSTICK_Y, 1.0f, 1.0f);
    g_pExoInput->CreateNewEvent(INPUT_JOYSTICK_Y_BUTTON, CExoInput::EVENT_TYPE_BUTTON,
        CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_YAXIS, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(INPUT_JOYSTICK_Y_BUTTON, INPUT_CONTEXT_FREE_LOOK);
    g_pExoInput->CreateNewEvent(INPUT_JOYSTICK_X, CExoInput::EVENT_TYPE_RELATIVE,
        CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_XAXIS, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(INPUT_JOYSTICK_X, INPUT_CONTEXT_PC);
    g_pExoInput->ScaleEvent(INPUT_JOYSTICK_X, 1.0f, 1.0f);
    g_pExoInput->CreateNewEvent(INPUT_JOYSTICK_X_BUTTON, CExoInput::EVENT_TYPE_BUTTON,
        CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_XAXIS, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(INPUT_JOYSTICK_X_BUTTON, INPUT_CONTEXT_FREE_LOOK);
    g_pExoInput->CreateNewEvent(INPUT_JOYSTICK_SLIDER0, CExoInput::EVENT_TYPE_RELATIVE,
        CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_SLIDER0, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(INPUT_JOYSTICK_SLIDER0, INPUT_CONTEXT_PC);
    g_pExoInput->AddEvent(INPUT_JOYSTICK_SLIDER0, INPUT_CONTEXT_FREE_LOOK);
    g_pExoInput->ScaleEvent(INPUT_JOYSTICK_SLIDER0, 1.0f, 1.0f);
    g_pExoInput->CreateNewEvent(INPUT_JOYSTICK_SLIDER1, CExoInput::EVENT_TYPE_RELATIVE,
        CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_SLIDER1, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(INPUT_JOYSTICK_SLIDER1, INPUT_CONTEXT_PC);
    g_pExoInput->AddEvent(INPUT_JOYSTICK_SLIDER1, INPUT_CONTEXT_FREE_LOOK);
    g_pExoInput->ScaleEvent(INPUT_JOYSTICK_SLIDER1, 1.0f, 1.0f);
    g_pExoInput->CreateNewEvent(INPUT_JOYSTICK_BUTTON8, CExoInput::EVENT_TYPE_BUTTON,
        CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_BUTTON8, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(INPUT_JOYSTICK_BUTTON8, INPUT_CONTEXT_PC);
    g_pExoInput->AddEvent(INPUT_JOYSTICK_BUTTON8, INPUT_CONTEXT_FREE_LOOK);
    g_pExoInput->CreateNewEvent(INPUT_MINIGAME_MOUSE_X, CExoInput::EVENT_TYPE_RELATIVE,
        CExoInput::INPUTDEVICE_MOUSE, CExoInput::MOUSE_XAXIS, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(INPUT_MINIGAME_MOUSE_X, INPUT_CONTEXT_MINIGAME);
    g_pExoInput->CreateNewEvent(INPUT_MINIGAME_MOUSE_Y, CExoInput::EVENT_TYPE_RELATIVE,
        CExoInput::INPUTDEVICE_MOUSE, CExoInput::MOUSE_YAXIS, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(INPUT_MINIGAME_MOUSE_Y, INPUT_CONTEXT_MINIGAME);
    g_pExoInput->CreateNewEvent(INPUT_MINIGAME_MOUSE_BUTTON0, CExoInput::EVENT_TYPE_BUTTON,
        CExoInput::INPUTDEVICE_MOUSE, CExoInput::MOUSE_BUTTON0, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(INPUT_MINIGAME_MOUSE_BUTTON0, INPUT_CONTEXT_MINIGAME);
    g_pExoInput->CreateNewEvent(INPUT_MINIGAME_MOUSE_BUTTON1, CExoInput::EVENT_TYPE_BUTTON,
        CExoInput::INPUTDEVICE_MOUSE, CExoInput::MOUSE_BUTTON1, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(INPUT_MINIGAME_MOUSE_BUTTON1, INPUT_CONTEXT_MINIGAME);
    g_pExoInput->CreateNewEvent(INPUT_MOUSE_X, CExoInput::EVENT_TYPE_RELATIVE,
        CExoInput::INPUTDEVICE_MOUSE, CExoInput::MOUSE_XAXIS, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(INPUT_MOUSE_X, INPUT_CONTEXT_PC_GUI);
    g_pExoInput->AddEvent(INPUT_MOUSE_X, INPUT_CONTEXT_PC);
    g_pExoInput->AddEvent(INPUT_MOUSE_X, INPUT_CONTEXT_DIALOG);
    g_pExoInput->AddEvent(INPUT_MOUSE_X, INPUT_CONTEXT_FREE_LOOK);
    g_pExoInput->CreateNewEvent(INPUT_MOUSE_Y, CExoInput::EVENT_TYPE_RELATIVE,
        CExoInput::INPUTDEVICE_MOUSE, CExoInput::MOUSE_YAXIS, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(INPUT_MOUSE_Y, INPUT_CONTEXT_PC_GUI);
    g_pExoInput->AddEvent(INPUT_MOUSE_Y, INPUT_CONTEXT_PC);
    g_pExoInput->AddEvent(INPUT_MOUSE_Y, INPUT_CONTEXT_DIALOG);
    g_pExoInput->AddEvent(INPUT_MOUSE_Y, INPUT_CONTEXT_FREE_LOOK);
    g_pExoInput->CreateNewEvent(INPUT_MOUSE_BUTTON0, CExoInput::EVENT_TYPE_BUTTON,
        CExoInput::INPUTDEVICE_MOUSE, CExoInput::MOUSE_BUTTON0, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(INPUT_MOUSE_BUTTON0, INPUT_CONTEXT_PC);
    g_pExoInput->AddEvent(INPUT_MOUSE_BUTTON0, INPUT_CONTEXT_PC_GUI);
    g_pExoInput->AddEvent(INPUT_MOUSE_BUTTON0, INPUT_CONTEXT_DIALOG);
    g_pExoInput->AddEvent(INPUT_MOUSE_BUTTON0, INPUT_CONTEXT_FREE_LOOK);
    g_pExoInput->CreateNewEvent(INPUT_MOUSE_BUTTON1, CExoInput::EVENT_TYPE_BUTTON,
        CExoInput::INPUTDEVICE_MOUSE, CExoInput::MOUSE_BUTTON1, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(INPUT_MOUSE_BUTTON1, INPUT_CONTEXT_PC);
    g_pExoInput->AddEvent(INPUT_MOUSE_BUTTON1, INPUT_CONTEXT_PC_GUI);
    g_pExoInput->AddEvent(INPUT_MOUSE_BUTTON1, INPUT_CONTEXT_DIALOG);
    g_pExoInput->AddEvent(INPUT_MOUSE_BUTTON1, INPUT_CONTEXT_FREE_LOOK);
    g_pExoInput->CreateNewEvent(INPUT_MOUSE_BUTTON2, CExoInput::EVENT_TYPE_BUTTON,
        CExoInput::INPUTDEVICE_MOUSE, CExoInput::MOUSE_BUTTON2, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(INPUT_MOUSE_BUTTON2, INPUT_CONTEXT_PC_GUI);
    g_pExoInput->AddEvent(INPUT_MOUSE_BUTTON2, INPUT_CONTEXT_FREE_LOOK);
    g_pExoInput->AddEvent(INPUT_MOUSE_BUTTON2, INPUT_CONTEXT_PC);
#ifndef _XBOX
    g_pExoInput->CreateNewEvent(INPUT_MOUSE_Z, CExoInput::EVENT_TYPE_BUTTON,
        CExoInput::INPUTDEVICE_MOUSE, CExoInput::MOUSE_ZAXIS, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(INPUT_MOUSE_Z, INPUT_CONTEXT_PC_GUI);
    g_pExoInput->AddEvent(INPUT_MOUSE_Z, INPUT_CONTEXT_PC);
#endif
    g_pExoInput->AddEvent(INPUT_MOUSE_Z, INPUT_CONTEXT_DIALOG);
    g_pExoInput->AddEvent(INPUT_MOUSE_Z, INPUT_CONTEXT_FREE_LOOK);

    CExoIni cIni;
    CExoString sAction;
    CExoString sKey;
    CExoString sLanguage;
    int anKeysUsed[256];
    memset(anKeysUsed, 0, sizeof(anKeysUsed));
    int nKey;
    int nValue;
    int nKey2;
    int nRepeatWait;
    int nRepeatRate;
    float fScaleMag;
    float fScaleExp;

    C2DA* pKeymap;
    if ((pKeymap = g_pRules->Get2DArrays()->m_pKeymap) != NULL)
    {
        int nLanguage = m_nClientLanguage;
        sLanguage = CExoString("Language") + nLanguage;
        int nAction = INPUT_ACTION_FIRST;
        while (1)
        {
            sAction = CExoString("Action") + nAction;
            bool bDisabled = (pKeymap = g_pRules->Get2DArrays()->m_pKeymap)->GetINTEntry(sAction, "Disabled", &nKey) && nKey == 1;
            if (!bDisabled && (pKeymap = g_pRules->Get2DArrays()->m_pKeymap)->GetINTEntry(sAction, sLanguage, &nKey) && nKey >= 0)
                break;
            ++nAction;
            if (nAction > INPUT_ACTION_LAST)
            {
                CExoString sNumber(0);
                sLanguage = CExoString("Language") + sNumber;
                break;
            }
        }
    }

    for (unsigned long nAction = INPUT_ACTION_FIRST; nAction <= INPUT_ACTION_LAST; ++nAction)
    {
        sAction = CExoString("Action") + nAction;
        bool bDisabled = (pKeymap = g_pRules->Get2DArrays()->m_pKeymap)->GetINTEntry(sAction, "Disabled", &nKey) && nKey == 1;
        if (!bDisabled)
        {
            if (cIni.ReadIniEntry(sKey, ".\\swkotor2.ini", "Keymapping", sAction))
            {
                nKey = sKey.AsINT() & CExoInput::INPUT_CONTROL_INDEX_MASK;
                if (anKeysUsed[nKey] == 0 || nKey == 0)
                {
                    int nEventType = 0;
                    if ((pKeymap = g_pRules->Get2DArrays()->m_pKeymap) != NULL)
                    {
                        if (!(pKeymap = g_pRules->Get2DArrays()->m_pKeymap)->GetINTEntry(sAction, "EventType", &nEventType))
                            nEventType = 0;
                    }
                    if (nEventType == 1)
                        g_pExoInput->CreateNewEvent(nAction, CExoInput::EVENT_TYPE_POLLED,
                            CExoInput::INPUTDEVICE_KEYBOARD, nKey, CExoInput::INPUT_CONTROL_NONE);
                    else
                        g_pExoInput->CreateNewEvent(nAction, CExoInput::EVENT_TYPE_BUTTON,
                            CExoInput::INPUTDEVICE_KEYBOARD, nKey, CExoInput::INPUT_CONTROL_NONE);
                }
            }
            else if ((pKeymap = g_pRules->Get2DArrays()->m_pKeymap) != NULL &&
                     (pKeymap = g_pRules->Get2DArrays()->m_pKeymap)->GetINTEntry(sAction, sLanguage, &nKey) && nKey >= 0)
            {
                if (anKeysUsed[nKey] == 0 || nKey == 0)
                {
                    int nEventType = 0;
                    if ((pKeymap = g_pRules->Get2DArrays()->m_pKeymap) != NULL)
                    {
                        if (!(pKeymap = g_pRules->Get2DArrays()->m_pKeymap)->GetINTEntry(sAction, "EventType", &nEventType))
                            nEventType = 0;
                    }
                    if (nEventType == 1)
                        g_pExoInput->CreateNewEvent(nAction, CExoInput::EVENT_TYPE_POLLED,
                            CExoInput::INPUTDEVICE_KEYBOARD, nKey, CExoInput::INPUT_CONTROL_NONE);
                    else
                        g_pExoInput->CreateNewEvent(nAction, CExoInput::EVENT_TYPE_BUTTON,
                            CExoInput::INPUTDEVICE_KEYBOARD, nKey, CExoInput::INPUT_CONTROL_NONE);
                    if (nKey == 0)
                        continue;
                }
                sKey.Format("%d", nKey);
                cIni.WriteIniEntry(sKey, ".\\swkotor2.ini", "Keymapping", sAction);
            }
        }
    }

    for (unsigned long nAction = INPUT_PAIRED_ACTION_FIRST; nAction <= INPUT_PAIRED_ACTION_LAST; ++nAction)
    {
        sAction = CExoString("Action") + nAction + CExoString("A");
        bool bDisabled = (pKeymap = g_pRules->Get2DArrays()->m_pKeymap)->GetINTEntry(sAction, "Disabled", &nKey) && nKey == 1;
        if (!bDisabled)
        {
            int bReadKey = cIni.ReadIniEntry(sKey, ".\\swkotor2.ini", "Keymapping", sAction);
            nKey = -1;
            if (bReadKey)
                nKey = sKey.AsINT() & CExoInput::INPUT_CONTROL_INDEX_MASK;
            else if ((pKeymap = g_pRules->Get2DArrays()->m_pKeymap) != NULL)
            {
                (pKeymap = g_pRules->Get2DArrays()->m_pKeymap)->GetINTEntry(sAction, sLanguage, &nKey);
                if (nKey != 0)
                {
                    sKey.Format("%d", nKey);
                    cIni.WriteIniEntry(sKey, ".\\swkotor2.ini", "Keymapping", sAction);
                }
            }
            nKey2 = -1;
            sAction = CExoString("Action") + nAction + CExoString("B");
            if (cIni.ReadIniEntry(sKey, ".\\swkotor2.ini", "Keymapping", sAction))
                nKey2 = sKey.AsINT() & CExoInput::INPUT_CONTROL_INDEX_MASK;
            else if ((pKeymap = g_pRules->Get2DArrays()->m_pKeymap) != NULL)
            {
                (pKeymap = g_pRules->Get2DArrays()->m_pKeymap)->GetINTEntry(sAction, sLanguage, &nKey2);
                if (nKey2 != 0)
                {
                    sKey.Format("%d", nKey2);
                    cIni.WriteIniEntry(sKey, ".\\swkotor2.ini", "Keymapping", sAction);
                }
            }
            if (nKey >= 0 && nKey2 >= 0 &&
                ((anKeysUsed[nKey] == 0 && anKeysUsed[nKey2] == 0 && nKey != nKey2) ||
                 (nKey == 0 && nKey2 == 0)))
                g_pExoInput->CreateNewEvent(nAction, CExoInput::EVENT_TYPE_PAIR,
                    CExoInput::INPUTDEVICE_KEYBOARD, nKey2, nKey);
        }
    }

    for (unsigned long nAction = INPUT_ACTION_FIRST; nAction <= INPUT_ACTION_LAST; ++nAction)
    {
        sAction = CExoString("Action") + nAction;
        bool bDisabled = (pKeymap = g_pRules->Get2DArrays()->m_pKeymap)->GetINTEntry(sAction, "Disabled", &nKey) && nKey == 1;
        if (!bDisabled && g_pExoInput->EventExists(nAction))
        {
            if ((pKeymap = g_pRules->Get2DArrays()->m_pKeymap)->GetINTEntry(sAction, "ICPC", &nValue) && nValue)
                g_pExoInput->AddEvent(nAction, INPUT_CONTEXT_PC);
            if ((pKeymap = g_pRules->Get2DArrays()->m_pKeymap)->GetINTEntry(sAction, "ICMiniGame", &nValue) && nValue)
                g_pExoInput->AddEvent(nAction, INPUT_CONTEXT_MINIGAME);
            if ((pKeymap = g_pRules->Get2DArrays()->m_pKeymap)->GetINTEntry(sAction, "ICPCGUI", &nValue) && nValue)
                g_pExoInput->AddEvent(nAction, INPUT_CONTEXT_PC_GUI);
            if ((pKeymap = g_pRules->Get2DArrays()->m_pKeymap)->GetINTEntry(sAction, "ICDialog", &nValue) && nValue)
                g_pExoInput->AddEvent(nAction, INPUT_CONTEXT_DIALOG);
            if ((pKeymap = g_pRules->Get2DArrays()->m_pKeymap)->GetINTEntry(sAction, "ICFreeLook", &nValue) && nValue)
                g_pExoInput->AddEvent(nAction, INPUT_CONTEXT_FREE_LOOK);
            if ((pKeymap = g_pRules->Get2DArrays()->m_pKeymap)->GetINTEntry(sAction, "ICMovie", &nValue) && nValue)
                g_pExoInput->AddEvent(nAction, INPUT_CONTEXT_MOVIE);
            if ((pKeymap = g_pRules->Get2DArrays()->m_pKeymap)->GetINTEntry(sAction, "Repeatable", &nValue) && nValue)
            {
                if ((pKeymap = g_pRules->Get2DArrays()->m_pKeymap)->GetINTEntry(sAction, "RepeatWait", &nRepeatWait) &&
                    (pKeymap = g_pRules->Get2DArrays()->m_pKeymap)->GetINTEntry(sAction, "RepeatRate", &nRepeatRate))
                    g_pExoInput->SetEventRepeatable(nAction, nRepeatWait, nRepeatRate);
            }
            if ((pKeymap = g_pRules->Get2DArrays()->m_pKeymap)->GetINTEntry(sAction, "Scale", &nValue) && nValue)
            {
                if ((pKeymap = g_pRules->Get2DArrays()->m_pKeymap)->GetFLOATEntry(sAction, "ScaleMag", &fScaleMag) &&
                    (pKeymap = g_pRules->Get2DArrays()->m_pKeymap)->GetFLOATEntry(sAction, "ScaleExp", &fScaleExp))
                    g_pExoInput->ScaleEvent(nAction, fScaleMag, fScaleExp);
            }
        }
    }

    for (unsigned long nAction = INPUT_PAIRED_ACTION_FIRST; nAction <= INPUT_PAIRED_ACTION_LAST; ++nAction)
    {
        sAction = CExoString("Action") + nAction + CExoString("A");
        bool bDisabled = (pKeymap = g_pRules->Get2DArrays()->m_pKeymap)->GetINTEntry(sAction, "Disabled", &nKey) && nKey == 1;
        if (!bDisabled && g_pExoInput->EventExists(nAction))
        {
            if ((pKeymap = g_pRules->Get2DArrays()->m_pKeymap)->GetINTEntry(sAction, "ICPC", &nValue) && nValue)
                g_pExoInput->AddEvent(nAction, INPUT_CONTEXT_PC);
            if ((pKeymap = g_pRules->Get2DArrays()->m_pKeymap)->GetINTEntry(sAction, "ICMiniGame", &nValue) && nValue)
                g_pExoInput->AddEvent(nAction, INPUT_CONTEXT_MINIGAME);
            if ((pKeymap = g_pRules->Get2DArrays()->m_pKeymap)->GetINTEntry(sAction, "ICPCGUI", &nValue) && nValue)
                g_pExoInput->AddEvent(nAction, INPUT_CONTEXT_PC_GUI);
            if ((pKeymap = g_pRules->Get2DArrays()->m_pKeymap)->GetINTEntry(sAction, "ICDialog", &nValue) && nValue)
                g_pExoInput->AddEvent(nAction, INPUT_CONTEXT_DIALOG);
            if ((pKeymap = g_pRules->Get2DArrays()->m_pKeymap)->GetINTEntry(sAction, "ICFreeLook", &nValue) && nValue)
                g_pExoInput->AddEvent(nAction, INPUT_CONTEXT_FREE_LOOK);
            if ((pKeymap = g_pRules->Get2DArrays()->m_pKeymap)->GetINTEntry(sAction, "ICMovie", &nValue) && nValue)
                g_pExoInput->AddEvent(nAction, INPUT_CONTEXT_MOVIE);
            if ((pKeymap = g_pRules->Get2DArrays()->m_pKeymap)->GetINTEntry(sAction, "Repeatable", &nValue) && nValue)
            {
                if ((pKeymap = g_pRules->Get2DArrays()->m_pKeymap)->GetINTEntry(sAction, "RepeatWait", &nRepeatWait) &&
                    (pKeymap = g_pRules->Get2DArrays()->m_pKeymap)->GetINTEntry(sAction, "RepeatRate", &nRepeatRate))
                    g_pExoInput->SetEventRepeatable(nAction, nRepeatWait, nRepeatRate);
            }
            if ((pKeymap = g_pRules->Get2DArrays()->m_pKeymap)->GetINTEntry(sAction, "Scale", &nValue) && nValue)
            {
                if ((pKeymap = g_pRules->Get2DArrays()->m_pKeymap)->GetFLOATEntry(sAction, "ScaleMag", &fScaleMag) &&
                    (pKeymap = g_pRules->Get2DArrays()->m_pKeymap)->GetFLOATEntry(sAction, "ScaleExp", &fScaleExp))
                    g_pExoInput->ScaleEvent(nAction, fScaleMag, fScaleExp);
            }
        }
    }
}
#endif
static CResRef* AllocateResRefArray(int nCount)
{
    return new CResRef[nCount];
}
// PC 0x0063BE40 GOG 0x0063BD40
void CClientExoAppInternal::LoadItemDropSoundList()
{
    if (m_pItemDropSounds != NULL)
        return;

    C2DA* pInventorySounds = new C2DA("inventorysnds", 0);
    if (pInventorySounds == NULL)
        return;
    if (!pInventorySounds->Load2DArray())
        return;

    CExoString sInventorySound;
    m_pItemDropSounds = new CResRef[pInventorySounds->GetNumRows()];
    for (unsigned char nRow = 0; nRow < pInventorySounds->GetNumRows(); nRow++)
    {
        if (pInventorySounds->GetCExoStringEntry(nRow, "InventorySound",
                                               &sInventorySound))
        {
            m_pItemDropSounds[nRow] = (char*)sInventorySound.CStr();
        }
    }
    delete pInventorySounds;
}
#ifndef _XBOX
// PC 0x00640D20 GOG 0x00640BD0
void CClientExoAppInternal::UpdateSoundEngine()
{
    if (m_pModule != NULL)
    {
        CAurCamera* pCamera = m_pModule->GetCamera();
        CSWCCreature* pCreature = GetPlayerCreature();
        if (pCamera != NULL)
        {
            CSWMiniGame* pMiniGame = m_pModule->GetMiniGame();
            if (pMiniGame != NULL && pMiniGame->GetPlayer() != NULL)
            {
                Vector vPlayerPosition = pMiniGame->GetPlayer()->GetPosition();
            }
            else if (pCreature == NULL)
                goto RenderSound;

            Vector vPosition;
            Vector vDirection;
            Quaternion qOrientation;
            vPosition = pCamera->GetPosition();
            qOrientation = pCamera->GetOrientation();

            if (g_fMicPosFromCam < 0.0f)
                g_fMicPosFromCam = 0.0f;
            else if (g_fMicPosFromCam > 1.0f)
                g_fMicPosFromCam = 1.0f;

            if (pCamera != NULL)
            {
                CSWCameraOnAStick* pCameraOnAStick =
                    (CSWCameraOnAStick*)pCamera->GetBehavior(AUR_BEHAVIOR_CAMERA_ONASTICK);
                if (pCameraOnAStick != NULL)
                {
                    Vector vTarget;
                    vTarget = pCameraOnAStick->GetTargetHeadPosition();
                    vPosition += g_fMicPosFromCam * (vTarget - vPosition);
                }
                else if (pCamera->GetBehavior(AUR_BEHAVIOR_CAMERA_COMBAT) != NULL)
                {
                    CSWCCreature* pCharacter = m_pSWParty->GetCharacter(0);
                    Vector vTarget = pCharacter->m_vPosition;
                    vTarget.m_fZ += pCharacter->GetHeadHeightOffset();
                    vPosition += (vTarget - vPosition) * g_fMicPosFromCam;
                }
                else if (g_pAppManager->GetClientExoApp()->GetInGameGui()->m_nGlobalDialogState &&
                         g_pAppManager->GetClientExoApp()->GetInGameGui()->m_bComputerCameraShot)
                {
                    vPosition -= qOrientation.zdir() *
                        (1.0f * g_pAppManager->GetClientExoApp()->GetInGameGui()->m_fComputerCameraMicRange);
                }
            }

            g_pExoSound->SetListenerPosition(vPosition);
            vDirection = qOrientation.zdir();
            vDirection.m_fX *= -1.0f;
            vDirection.m_fY *= -1.0f;
            vDirection.m_fZ *= -1.0f;
            g_pExoSound->SetListenerOrientation(vDirection, qOrientation.ydir());
        }
    }
RenderSound:

    g_pExoSound->Render();
}
#endif

int CClientExoAppInternal::IsValidResolution(unsigned long nWidth,
                                          unsigned long nHeight)
{
    if (nWidth != VIDEO_FALLBACK_WIDTH && nWidth != VIDEO_XGA_WIDTH &&
        nWidth != VIDEO_SXGA_WIDTH && nWidth != VIDEO_UXGA_WIDTH)
        return 0;
    if (nHeight != VIDEO_FALLBACK_HEIGHT && nHeight != VIDEO_XGA_HEIGHT &&
        nHeight != VIDEO_SXGA_4_3_HEIGHT && nHeight != VIDEO_SXGA_HEIGHT &&
        nHeight != VIDEO_UXGA_HEIGHT)
        return 0;
    return 1;
}
#ifndef _XBOX
// PC 0x0063BFD0 GOG 0x0063BED0
int CClientExoAppInternal::ReadVideoModeSettings(unsigned long* pnWidth,
                                               unsigned long* pnHeight,
                                               unsigned char* pnBitsPerPixel,
                                               unsigned long* pnRefreshRate)
{
    CExoString sValue;
    CExoIni cIni;
    int bReadSettings = cIni.ReadIniEntry(sValue, ".\\swkotor2.ini",
                                         "Graphics Options", "Width");
    *pnWidth = sValue.AsINT();
    cIni.ReadIniEntry(sValue, ".\\swkotor2.ini", "Graphics Options", "Height");
    *pnHeight = sValue.AsINT();
    *pnBitsPerPixel = VIDEO_BITS_PER_PIXEL;

    if (cIni.ReadIniEntry(sValue, ".\\swkotor2.ini", "Graphics Options", "RefreshRate"))
    {
        g_nDesiredFullScreenRefreshRate = sValue.AsINT();
        *pnRefreshRate = g_nDesiredFullScreenRefreshRate;
    }
    else
    {
        *pnRefreshRate = g_nDesiredFullScreenRefreshRate;
    }

    if ((*pnWidth != VIDEO_FALLBACK_WIDTH && *pnWidth != VIDEO_XGA_WIDTH &&
         *pnWidth != VIDEO_SXGA_WIDTH && *pnWidth != VIDEO_UXGA_WIDTH) ||
        (*pnHeight != VIDEO_FALLBACK_HEIGHT && *pnHeight != VIDEO_XGA_HEIGHT &&
         *pnHeight != VIDEO_SXGA_4_3_HEIGHT && *pnHeight != VIDEO_SXGA_HEIGHT &&
         *pnHeight != VIDEO_UXGA_HEIGHT))
    {
        *pnWidth = VIDEO_FALLBACK_WIDTH;
        *pnHeight = VIDEO_FALLBACK_HEIGHT;
    }
    if (*pnBitsPerPixel != VIDEO_BITS_PER_PIXEL)
        *pnBitsPerPixel = VIDEO_BITS_PER_PIXEL;
    if (*pnRefreshRate < VIDEO_REFRESH_RATE)
        *pnRefreshRate = VIDEO_REFRESH_RATE;

    return bReadSettings;
}
#endif
void CClientExoAppInternal::StopMovie()  // PC 0x004015A0 GOG 0x004015A0
{
}
// PC 0x004015A0 GOG 0x004015A0
void CClientExoAppInternal::EndLoadBar()
{
}
// PC 0x00641090 GOG 0x00640F40
int CClientExoAppInternal::IsOnExclusionList(unsigned long oidObject)
{
    if (m_pPauseExclusionList == NULL)
    {
        return 1;
    }

    CExoLinkedListNode* pNode = m_pPauseExclusionList->GetHeadPos();

    while (pNode != NULL)
    {
        if ((unsigned long)m_pPauseExclusionList->GetAtPos(pNode) == oidObject)
        {
            return 1;
        }

        m_pPauseExclusionList->GetNext(pNode);
    }

    return 0;
}
// PC 0x00641100 GOG 0x00640FB0
void CClientExoAppInternal::SetCaptureEvents(const CExoString& sFileName)
{
    m_nCaptureEvents = 1;
    if (m_nCaptureEventPlayBack == 1)
    {
        m_nCaptureEvents = 0;
        return;
    }
    if (sFileName == "")
    {
        m_nCaptureEvents = 0;
        return;
    }
    if (m_pCaptureEventFile != NULL)
    {
        delete m_pCaptureEventFile;
        m_pCaptureEventFile = NULL;
    }
    CExoString sPath;
    sPath.Format("HD0:%s", sFileName.CStr());
    m_pCaptureEventFile = new CExoFile(sPath, RESTYPE_TXT, "wb");
    if (m_pCaptureEventFile != NULL)
    {
        if (m_pCaptureEventFile->FileOpened())
        {
            unsigned long nTime = (unsigned long)(
                g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000);
            CExoString sEvent;
            sEvent.Format("Start: %d\n", nTime);
            m_pCaptureEventFile->Write(sEvent);
        }
        else
        {
            delete m_pCaptureEventFile;
            m_pCaptureEventFile = NULL;
        }
    }
}
// PC 0x006412F0 GOG 0x006411A0
void CClientExoAppInternal::SetCaptureEventPlayBack(const CExoString& sFileName)
{
    m_nCaptureEventPlayBack = 1;
    if (m_nCaptureEvents == 1)
    {
        m_nCaptureEventPlayBack = 0;
        return;
    }
    if (m_pCaptureEventFile != NULL)
    {
        delete m_pCaptureEventFile;
        m_pCaptureEventFile = NULL;
        while (m_lstPlayBackEvents.GetHeadPos() != NULL)
            m_lstPlayBackEvents.RemoveTail();
    }
    CExoString sPath;
    sPath.Format("HD0:%s", sFileName.CStr());
    m_pCaptureEventFile = new CExoFile(sPath, RESTYPE_TXT, "rb");
    if (m_pCaptureEventFile != NULL)
    {
        if (m_pCaptureEventFile->FileOpened())
        {
            ReadPlayBackInputEvents();
            g_pAppManager->GetClientExoApp()->GetWorldTimer()->SetFixedSnapshotRate(
                1, m_nCaptureEventPlayBackRate);
        }
        else
        {
            delete m_pCaptureEventFile;
            m_pCaptureEventFile = NULL;
        }
    }
}

#ifndef _XBOX
unsigned long CClientExoAppInternal::GetNearestVideoMode(unsigned long nWidth, unsigned long nHeight,
                                                         unsigned char nBitsPerPixel,
                                                         unsigned long nRefreshRate)
{
    if (nWidth == 0)
        nWidth = g_nScreenWidth;
    if (nHeight == 0)
        nHeight = g_nScreenHeight;
    if (nBitsPerPixel == 0)
        nBitsPerPixel = g_nBitsPerPixel;
    if (nRefreshRate == 0)
        nRefreshRate = g_nDesiredFullScreenRefreshRate;
    DWORD nNearestMode = CAuroraInterface::GetNearestVideoMode(nWidth, nHeight, nBitsPerPixel, nRefreshRate);
    DEVMODE cDevMode;
    memset(&cDevMode, 0, sizeof(cDevMode));
    cDevMode.dmSize = sizeof(cDevMode);
    if (!EnumDisplaySettings(NULL, nNearestMode, &cDevMode))
    {
        return -1;
    }
    if (cDevMode.dmBitsPerPel != nBitsPerPixel)
    {
        return -1;
    }
    return nNearestMode;
}
#endif

#ifdef _XBOX

void CClientExoAppInternal::ReadAndSetVideoMode(int bStartup, int bFullScreen)
{
    CExoString sValue;
    CExoIni cIni;

    cIni.ReadIniEntry(sValue, "d:\\swkotor2.ini", "Display Options", "Width");
    unsigned long nWidth = sValue.AsINT();
    cIni.ReadIniEntry(sValue, "d:\\swkotor2.ini", "Display Options", "Height");
    unsigned long nHeight = sValue.AsINT();
    cIni.ReadIniEntry(sValue, "d:\\swkotor2.ini", "Display Options", "BitsPerPixels");
    unsigned char nBitsPerPixel = sValue.AsINT();
    cIni.ReadIniEntry(sValue, "d:\\swkotor2.ini", "Display Options", "RefreshRate");
    unsigned long nRefreshRate = sValue.AsINT();

    if (!cIni.ReadIniEntry(sValue, "d:\\swkotor2.ini", "Video Options", "NumDynamicLights"))
    {
        sValue.Format("%d", VIDEO_DEFAULT_DYNAMIC_LIGHTS);
        cIni.WriteIniEntry(sValue, "d:\\swkotor2.ini", "Video Options", "NumDynamicLights");
    }

    m_pClientOptions->SetFullScreenEnabled(bFullScreen);
    CAuroraInterface::GetExactVideoMode(nWidth, nHeight, nBitsPerPixel, nRefreshRate);
}
#else
void CClientExoAppInternal::ReadAndSetVideoMode(int bStartup, int bFullScreen)  // PC 0x00641490 GOG 0x00641340
{
    unsigned long nWidth = 0;
    unsigned long nHeight = 0;
    unsigned long nRefreshRate = 0;
    unsigned char nBitsPerPixel = 0;
    CExoString sValue;
    CExoIni cIni;
    int bReadSettings = ReadVideoModeSettings(&nWidth, &nHeight,
                                             &nBitsPerPixel, &nRefreshRate);
    m_pClientOptions->SetFullScreenEnabled(bFullScreen);
    if (bStartup)
    {
        nWidth = VIDEO_STARTUP_WIDTH;
        nHeight = VIDEO_STARTUP_HEIGHT;
        nBitsPerPixel = VIDEO_BITS_PER_PIXEL;
        g_fullScreenDesired = 1;
    }
    unsigned long nVideoMode = CAuroraInterface::GetExactVideoMode(
        nWidth, nHeight, nBitsPerPixel, nRefreshRate);
    if ((!bReadSettings && !bStartup) || nVideoMode == -1)
    {
        nVideoMode = GetNearestVideoMode(VIDEO_FALLBACK_WIDTH, VIDEO_FALLBACK_HEIGHT,
                                         VIDEO_BITS_PER_PIXEL, VIDEO_REFRESH_RATE);
    }
    if (bFullScreen)
        SetVideoMode(nVideoMode, VIDEO_MODE_FULLSCREEN);
    else
        SetVideoMode(nVideoMode, VIDEO_MODE_WINDOWED);
    if (bStartup)
    {
        ReadVideoModeSettings(&nWidth, &nHeight, &nBitsPerPixel, &nRefreshRate);
        g_nScreenWidth = nWidth;
        g_nScreenHeight = nHeight;
        g_nBitsPerPixel = nBitsPerPixel;
    }
}
#endif
#ifndef _XBOX
void CClientExoAppInternal::StopServices()  // PC 0x00650110 GOG 0x0064FFC0
{
#ifndef _XBOX
    UnloadSuperModelSet();
    ShutDownMoviePlayer();
#endif
    if (m_pSWGuiManager->PanelExists(m_pMainMenu))
        delete m_pMainMenu;
    m_pMainMenu = NULL;
    if (m_pStartupPanel != NULL)
    {
        delete m_pStartupPanel;
        m_pStartupPanel = NULL;
    }
    if (m_pSoundSetList != NULL)
    {
        delete m_pSoundSetList;
        m_pSoundSetList = NULL;
    }
    if (m_pItemDropSounds != NULL)
    {
        delete m_pItemDropSounds;
        m_pItemDropSounds = NULL;
    }
    if (m_pModule != NULL)
    {
        delete m_pModule;
        m_pModule = NULL;
    }
    if (m_pGameObjectArray != NULL)
    {
        delete m_pGameObjectArray;
        m_pGameObjectArray = NULL;
    }
    DestroyWorldJournal();
    if (m_pImeUI != NULL)
    {
        delete m_pImeUI;
        m_pImeUI = NULL;
    }
    if (m_pGuiInGame != NULL)
    {
        m_pGuiInGame->ShutDown();
        delete m_pGuiInGame;
        m_pGuiInGame = NULL;
    }
    if (m_pSWGuiManager != NULL)
    {
        delete m_pSWGuiManager;
        m_pSWGuiManager = NULL;
    }
    if (m_pClientAIMaster != NULL)
    {
        delete m_pClientAIMaster;
        m_pClientAIMaster = NULL;
    }
    while (m_lstPlayerList.m_pInternal->m_pHead != NULL)
    {
        CSWCPlayer* pPlayer = m_lstPlayerList.RemoveHead();
        if (pPlayer != NULL)
            delete pPlayer;
    }
    if (m_pServerInfo != NULL)
    {
        delete m_pServerInfo;
        m_pServerInfo = NULL;
    }
    if (m_pUnknown10 != NULL)
    {
        m_pUnknown10->ShutDown();
        delete m_pUnknown10;
        m_pUnknown10 = NULL;
    }
    if (g_pExoInput != NULL)
    {
        delete g_pExoInput;
        g_pExoInput = NULL;
    }
    if (m_pMenuMusic != NULL)
    {
        delete m_pMenuMusic;
        m_pMenuMusic = NULL;
    }
    if (m_pLoadMusic != NULL)
    {
        delete m_pLoadMusic;
        m_pLoadMusic = NULL;
    }
    if (g_pExoSound != NULL)
    {
        delete g_pExoSound;
        g_pExoSound = NULL;
    }
    if (m_pSWCMessage != NULL)
    {
        delete m_pSWCMessage;
        m_pSWCMessage = NULL;
    }
    if (g_pTlkTable != NULL)
    {
        delete g_pTlkTable;
        g_pTlkTable = NULL;
    }
    if (m_pSoundSetList != NULL)
    {
        delete m_pSoundSetList;
        m_pSoundSetList = NULL;
    }
    if (g_pAuroraInterface != NULL)
    {
        delete g_pAuroraInterface;
        g_pAuroraInterface = NULL;
    }
    if (g_pAurora != NULL)
    {
        DestroyCAurora();
        g_pAurora = NULL;
    }
    if (g_pExoResMan != NULL)
    {
        g_pExoResMan->RemoveResourceDirectory("PORTRAITS:");
        g_pExoResMan->RemoveResourceDirectory("SERVERVAULT:");
        g_pExoResMan->RemoveFixedKeyTableFile("HD0:CHITIN");
        g_pExoResMan->RemoveResourceDirectory("OVERRIDE:");
        g_pExoResMan->RemoveResourceDirectory("HD0:MOVIES");
        g_pExoResMan->RemoveResourceDirectory("HD0:STREAMMUSIC");
        g_pExoResMan->RemoveResourceDirectory("HD0:STREAMVOICE");
        g_pExoResMan->RemoveResourceDirectory("TEMPCLIENT:");
        if (g_pExoResMan->Exists("GLOBAL", RESTYPE_RIM, (unsigned long*)NULL))
            g_pExoResMan->RemoveResourceImageFile("RIMS:GLOBAL");
        g_pExoResMan->RemoveResourceDirectory("RIMS:");
        if (g_pExoResMan != NULL)
        {
            delete g_pExoResMan;
            g_pExoResMan = NULL;
        }
    }
    if (m_pWorldTimer != NULL)
    {
        delete m_pWorldTimer;
        m_pWorldTimer = NULL;
    }
    if (m_pCombatTimer != NULL)
    {
        delete m_pCombatTimer;
        m_pCombatTimer = NULL;
    }
    if (m_pPauseTimer != NULL)
    {
        delete m_pPauseTimer;
        m_pPauseTimer = NULL;
    }
    if (m_pAnimationTimer != NULL)
    {
        delete m_pAnimationTimer;
        m_pAnimationTimer = NULL;
    }
    if (m_pClientOptions != NULL)
    {
        delete m_pClientOptions;
        m_pClientOptions = NULL;
    }
    if (m_pUnknown1C0 != NULL)
    {
        delete m_pUnknown1C0;
        m_pUnknown1C0 = NULL;
    }
    if (m_pPlayerControl != NULL)
        delete m_pPlayerControl;
#ifdef _XBOX
    if (m_pMoviePlayer != NULL)
    {
        delete m_pMoviePlayer;
        m_pMoviePlayer = NULL;
    }
#endif
    DestroyQuestJournal();
    if (m_pCharacterFile != NULL)
    {
        delete[] m_pCharacterFile;
        m_pCharacterFile = NULL;
    }
    m_nCharacterFileSize = 0;
    m_nCharacterFileId = 0;
}
CSWCPlayer* CClientExoAppInternal::GetPlayerByPlayerID(unsigned long nPlayerId)  // PC 0x00641650 GOG 0x00641500
{
    CExoLinkedListNode* pNode = m_lstPlayerList.GetHeadPos();
    void* pData = m_lstPlayerList.GetHead();
    CSWCPlayer* pPlayer = (CSWCPlayer*)pData;

    while (pPlayer != NULL)
    {
        if (pPlayer->m_nPlayerId == nPlayerId)
        {
            return pPlayer;
        }

        pPlayer = (CSWCPlayer*)m_lstPlayerList.GetNext(pNode);
    }

    return NULL;
}
#endif
CSWCPlayer* CClientExoAppInternal::GetPlayerByGameObjectID(unsigned long oidObject)  // PC 0x00641690 GOG 0x00641540
{
    CExoLinkedListNode* pNode = m_lstPlayerList.GetHeadPos();
    void* pData = m_lstPlayerList.GetHead();
    CSWCPlayer* pPlayer = (CSWCPlayer*)pData;

    while (pPlayer != NULL)
    {
        if (pPlayer->m_oidControlledObject == oidObject)
        {
            return pPlayer;
        }

        pPlayer = (CSWCPlayer*)m_lstPlayerList.GetNext(pNode);
    }

    return NULL;
}
// PC 0x006416D0 GOG 0x00641580
void CClientExoAppInternal::AttachPlayerGameObjectToArea()
{
    CSWCCreature* pCreature = GetPlayerCreature();
    if (pCreature != NULL)
    {
        CSWCModule* pModule = m_pModule;
        pCreature->AttachToScene(pModule->m_pInfo->m_pScene);
        if (enablemodelrequest)
        {
            if (pCreature->m_nCameraFollowDelay > 0)
            {
                pCreature->m_bCameraFollowPending = 1;
                pCreature->m_bCameraFollowSnap = 1;
            }
        }
        else
        {
            unsigned long nId = pCreature->m_nId;
            m_pModule->SetCameraFollow(nId, 1);
        }

        if (m_pPlayerControl != NULL)
        {
            m_pPlayerControl->SetPlayer(pCreature->m_nId);
            m_pPlayerControl->SetCamera(m_pModule->GetCamera());
        }
        else
        {
            m_pPlayerControl = new CSWPlayerControlCamRelative(
                pCreature->GetObjectId(), m_pModule->GetCamera());
        }
    }
}
#ifndef _XBOX
// PC 0x006417F0 GOG 0x006416A0
void CClientExoAppInternal::UpdateCamera(float fDeltaTime)
{
    if (m_pModule == NULL)
        return;

    float fTimeStep = (float)(__int64)g_pAppManager->GetClientExoApp()
                         ->GetPauseTimer()->GetSnapshotTimeDifference() /
                     1000000.0f;
    bool bKeepHistory = g_bKeepHistory;
    g_bKeepHistory = false;
    CAurCamera* pCamera = m_pModule->GetCamera();
    float fMouseSensitivity =
        g_pAppManager->GetClientExoApp()->GetClientOptions()->GetMouseSenSetting();
    float fMouseX = 0.0f;
    float fMouseY = 0.0f;
#ifndef _XBOX
    g_pExoInput->GetMouseDelta(&fMouseX, &fMouseY);
#endif
    if (g_pAppManager->GetClientExoApp()->GetClientOptions()->m_bReverseIngame)
        fMouseY *= -1.0f;

    float fTurn = min(1.0f, max(-1.0f, g_pExoInput->PollInput(CAMERA_TURN_INPUT, 0)));
    int nWidth = g_pAppManager->GetClientExoApp()->GetSWGuiManager()->m_nWidth;
    int nBorder = max(CAMERA_EDGE_MIN_PIXELS,
                      (int)(nWidth * ScreenFramePercentage));
#ifndef _XBOX
    int bLeft = g_pGuiMan->m_nMouseX <= nBorder;
    int bRight = g_pGuiMan->m_nMouseX >= nWidth - nBorder;
    int nHeight = g_pAppManager->GetClientExoApp()->GetSWGuiManager()->m_nHeight;
    int nBorderY = max(CAMERA_EDGE_MIN_PIXELS,
                       (int)(nHeight * ScreenFramePercentage));
    int bTop = g_pGuiMan->m_nMouseY <= nBorderY;
    int bBottom = g_pGuiMan->m_nMouseY >= nHeight - nBorderY;
#endif
    int bMouseTurn = g_bMouseCameraRotateToggle;
    if (g_pAppManager->GetClientExoApp()->GetClientOptions()->m_bMouseLook)
        bMouseTurn = (bMouseTurn == 0);

    if (fTurn != 0.0f)
        m_pModule->AcclTurnCamera((CameraHorzAxisFlip ? 1 : -1) * (0.0f - fTurn),
                                 fDeltaTime);
#ifndef _XBOX
    else if ((bLeft || bRight || bTop || bBottom) && !bMouseTurn &&
             g_pAppManager->GetClientExoApp()->GetClientOptions()->m_nCameraMode !=
                 CClientOptions::CAMERA_MODE_WORLD_CLICK_DISABLED)
    {
        if (bLeft || bRight)
            m_pModule->AcclTurnCamera((bLeft ? 1 : -1) *
                                     (CameraHorzAxisFlip ? 1 : -1), fDeltaTime);
        if (bTop || bBottom)
            m_pModule->AcclTiltCamera((bTop ? 1 : -1) *
                                     (CameraVertAxisFlip ? 1 : -1));
    }
#endif
    else if (!bMouseTurn &&
             g_pAppManager->GetClientExoApp()->GetClientOptions()->m_nCameraMode !=
                 CClientOptions::CAMERA_MODE_WORLD_CLICK_DISABLED)
        m_pModule->TurnCamera(0.0f, fDeltaTime);
    else
    {
        m_pModule->TurnCamera((CameraHorzAxisFlip ? 1 : -1) *
                             (fMouseX * fMouseSensitivity), fDeltaTime);
        int nFlip = CameraHorzAxisFlip ? 1 : -1;
        m_pModule->TiltCamera(nFlip * (fMouseY * fMouseSensitivity * 0.1f));
    }

    if (m_fCameraZoom != 0.0f)
    {
        CSWCModule* pModule = m_pModule;
        pModule->m_nReserved104 = 1;
    }
    if (pCamera != NULL)
    {
        float fExpansion = pCamera->GetDist() * 2.0f / 150.0f;
        g_HitCheckBoxExpansion.m_fX = fExpansion;
        g_HitCheckBoxExpansion.m_fY = fExpansion;
        g_HitCheckBoxExpansion.m_fZ = fExpansion;
    }
    Vector vPosition = pCamera->GetPosition();
    Vector vScroll;
    if (m_nCameraScrollX != 0)
        vScroll.m_fX = m_nCameraScrollX * pCamera->MouseDir(CAMERA_SCROLL_PIXELS, 0).m_fX;
    if (m_nCameraScrollY != 0)
        vScroll.m_fY = m_nCameraScrollY * pCamera->MouseDir(0, CAMERA_SCROLL_PIXELS).m_fY;
    m_pModule->ScrollCamera(vScroll);
    g_bKeepHistory = bKeepHistory;

    GetPlayerCreature();
    m_pModule->UpdateCameraBumpAndShake(fTimeStep);
    if (m_nCombatMode == 0)
        m_pModule->UpdateCameraFollow(fTimeStep);
    m_fCameraZoom = m_pModule->UpdateCameraCollision(fTimeStep, m_fCameraZoom);
    if (fabs(m_fCameraZoom) > CLIENT_CAMERA_ZOOM_THRESHOLD)
        m_pModule->ZoomCamera(m_fCameraZoom);

    if (g_pAppManager->GetClientExoApp()->GetCaptureEvents() == CAPTURE_EVENTS_RECORD &&
        m_pModule->m_nReserved80 == 1)
    {
        float fDistance = pCamera->GetDist();
        float fPitch = pCamera->GetPitch();
        float fYaw = pCamera->GetYaw();
        CExoString sEvent;
        sEvent.Format("%d %c %f %f %f\n",
            (unsigned int)(g_pAppManager->GetClientExoApp()->GetWorldTimer()
                               ->GetSnapshotTime() / 1000),
            CPlayBackEventNode::CAMERA, fDistance, fPitch, fYaw);
        g_pAppManager->GetClientExoApp()->GetCaptureEventFile()->Write(sEvent);
        CSWCModule* pModule = m_pModule;
        pModule->m_nReserved80 = 0;
    }
    m_fCameraZoom = 0.0f;
    m_fCameraTilt = 0.0f;
    m_fCameraTurn = 0.0f;
    m_pModule->m_nReserved80 = 0;
}
#endif
void CClientExoAppInternal::AddObjectToHitcheckIgnoreList(  // PC 0x00641E20 GOG 0x00641CD0
    unsigned long oidObject)
{
    CExoLinkedListNode* pNode = m_lstHitcheckIgnore.GetHeadPos();
    unsigned long* pOid = m_lstHitcheckIgnore.GetHead();

    while (pNode != NULL)
    {
        if (pOid != NULL && *pOid == oidObject)
        {
            return;
        }

        pOid = m_lstHitcheckIgnore.GetNext(pNode);
    }

    unsigned long* pNew = new unsigned long;
    *pNew = oidObject;
    m_lstHitcheckIgnore.AddTail(pNew);
}
// PC 0x00641E80 GOG 0x00641D30
void CClientExoAppInternal::RemoveObjectFromHitcheckIgnoreList(
    unsigned long oidObject)
{
    CExoLinkedListNode* pNode = m_lstHitcheckIgnore.GetHeadPos();
    unsigned long* pOid = m_lstHitcheckIgnore.GetHead();

    while (pNode != NULL)
    {
        if (pOid != NULL && *pOid == oidObject)
        {
            m_lstHitcheckIgnore.m_pInternal->Remove(pNode);
            delete pOid;
            return;
        }

        pOid = m_lstHitcheckIgnore.GetNext(pNode);
    }
}
// PC 0x00643990 GOG 0x00643840
void CClientExoAppInternal::ClearPauseExclusionList()
{
    while (m_pPauseExclusionList->GetHead() != NULL)
    {
        unsigned long oidObject = (unsigned long)m_pPauseExclusionList->RemoveHead();
        CGameObject* pObject;
        m_pGameObjectArray->GetGameObject(oidObject, &pObject);
        if (pObject != NULL)
        {
            unsigned long nDay;
            unsigned long nTime;
            GetActiveTimer(pObject->m_nId)->GetWorldTime(&nDay, &nTime);
            pObject->ResetUpdateTimes(nDay, nTime);
            pObject->AsSWCObject()->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1)
                ->DisableForceAnimations();
        }
    }
}
// PC 0x00643A80 GOG 0x00643930
void CClientExoAppInternal::AddToPauseExclusionList(unsigned long oidObject)
{
    CGameObject* pObject;
    m_pGameObjectArray->GetGameObject(oidObject, &pObject);

    if (pObject != NULL)
    {
        pObject->AsSWCObject()->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1)
            ->EnableForceAnimations();
        m_pPauseExclusionList->AddTail((unsigned long*)oidObject);

        unsigned long nDay;
        unsigned long nTime;
        unsigned long oidTimerObject = pObject->m_nId;
        GetActiveTimer(oidTimerObject)->GetWorldTime(&nDay, &nTime);
        ((CSWCObject*)pObject)->ResetUpdateTimes(nDay, nTime);
    }
}

inline void ClientExoAppInternalReleaseList(CExoLinkedList<unsigned long>* pList)
{
    delete pList;
}

inline void ClientExoAppInternalReserveActions(CExoArrayList<CSWGuiInterfaceAction>* pActions, int nSize)
{
    pActions->Allocate(nSize);
}
// PC 0x00643B10 GOG 0x006439C0
void CClientExoAppInternal::SetPauseState(unsigned char nPauseState, int bPause,
                                        CExoLinkedList<unsigned long>* pExclusionList)
{
    CWorldTimer* pTimer;
    if (GetPauseState(nPauseState) != bPause)
    {
        if (bPause == 1)
        {
            m_PauseState.m_nPauseState |= nPauseState;
            m_pWorldTimer->PauseWorldTimer();
            m_pWorldTimer->SetSnapshotTime();
            m_pWorldTimer->SetSnapshotTime();
            if (GetActivePauseState() == PAUSE_STATE_COMBAT)
            {
                m_pCombatTimer->PauseWorldTimer();
                m_pCombatTimer->SetSnapshotTime();
                m_pCombatTimer->SetSnapshotTime();
            }
            if (g_pAppManager->m_pClientExoApp->GetModule())
            {
                CSWCModule* pModule = g_pAppManager->m_pClientExoApp->GetModule();
                if (pModule->m_pInfo && pModule->m_pInfo->m_pScene)
                    pModule->m_pInfo->m_pScene->DisableAnimations();
            }
        }
        else
        {
            m_PauseState.m_nPauseState &= ~nPauseState;
            if (nPauseState == PAUSE_TYPE_COMBAT_MASK)
            {
                m_pPauseTimer->PauseWorldTimer();
                m_pAnimationTimer->PauseWorldTimer();
                m_pPauseTimer->ResetTimer(m_pCombatTimer);
                m_pAnimationTimer->ResetTimer(m_pCombatTimer);
                m_pPauseTimer->SetSnapshotTime();
                m_pAnimationTimer->SetSnapshotTime();
                m_pCombatTimer->SetSnapshotTime();
                pTimer = m_pPauseTimer;
                pTimer->m_nPreviousSnapshotTime = pTimer->GetSnapshotTime();
                pTimer = m_pAnimationTimer;
                pTimer->m_nPreviousSnapshotTime = pTimer->GetSnapshotTime();
                pTimer = m_pCombatTimer;
                pTimer->m_nPreviousSnapshotTime = pTimer->GetSnapshotTime();
                if (GetActivePauseState() != PAUSE_STATE_PAUSED)
                {
                    m_pWorldTimer->SetSnapshotTime();
                    pTimer = m_pWorldTimer;
                    pTimer->m_nPreviousSnapshotTime = pTimer->GetSnapshotTime();
                    m_pWorldTimer->UnpauseWorldTimer();
                    if (m_pModule && m_pModule->m_pInfo && m_pModule->m_pInfo->m_pScene)
                        m_pModule->m_pInfo->m_pScene->EnableAnimations();
                }
                m_pCombatTimer->UnpauseWorldTimer();
                m_pPauseTimer->UnpauseWorldTimer();
                m_pAnimationTimer->UnpauseWorldTimer();
            }
            else if (nPauseState == PAUSE_TYPE_PAUSED_MASK)
            {
                m_pCombatTimer->PauseWorldTimer();
                m_pPauseTimer->PauseWorldTimer();
                m_pAnimationTimer->PauseWorldTimer();
                m_pCombatTimer->ResetTimer(m_pWorldTimer);
                m_pPauseTimer->ResetTimer(m_pWorldTimer);
                m_pAnimationTimer->PauseWorldTimer();
                m_pWorldTimer->SetSnapshotTime();
                m_pCombatTimer->SetSnapshotTime();
                m_pPauseTimer->SetSnapshotTime();
                m_pAnimationTimer->SetSnapshotTime();
                pTimer = m_pPauseTimer;
                pTimer->m_nPreviousSnapshotTime = pTimer->GetSnapshotTime();
                pTimer = m_pAnimationTimer;
                pTimer->m_nPreviousSnapshotTime = pTimer->GetSnapshotTime();
                pTimer = m_pCombatTimer;
                pTimer->m_nPreviousSnapshotTime = pTimer->GetSnapshotTime();
                pTimer = m_pWorldTimer;
                pTimer->m_nPreviousSnapshotTime = pTimer->GetSnapshotTime();
                m_pWorldTimer->UnpauseWorldTimer();
                m_pCombatTimer->UnpauseWorldTimer();
                m_pPauseTimer->UnpauseWorldTimer();
                m_pAnimationTimer->UnpauseWorldTimer();
                if (m_pModule && m_pModule->m_pInfo && m_pModule->m_pInfo->m_pScene)
                    m_pModule->m_pInfo->m_pScene->EnableAnimations();
            }
        }
        ClearPauseExclusionList();
        if (pExclusionList)
        {
            CExoLinkedListNode* pNode = pExclusionList->GetHeadPos();
            while (pNode)
            {
                AddToPauseExclusionList((unsigned long)pExclusionList->GetAtPos(pNode));
                pExclusionList->GetNext(pNode);
            }
            delete pExclusionList;
        }
    }
}

void CClientExoAppInternal::CleanUpModuleServices()
{
    m_bModuleServicesActive = 0;
    if (m_pWorldJournal != NULL)
        m_pWorldJournal->DeleteAllEntries();
    if (m_pQuestJournal != NULL)
        m_pQuestJournal->ClearAll();
    if (m_pGuiInGame != NULL)
        m_pGuiInGame->ShutDown();
    if (m_pPlayerControl != NULL)
        m_pPlayerControl->Initialize();
}
// PC 0x00643E00 GOG 0x00643CB0
void CClientExoAppInternal::UnloadModule()
{
    SetClearColor("0 0 0");
    m_nPlayerCreatureId = CGameObjectArray::INVALIDOBJID;
    if (m_pSWParty != NULL)
        m_pSWParty->AddCharacter(m_nPlayerCreatureId);

    SetPausedByCombat(0, 0, 0);
    SetPauseState(PAUSE_STATE_COMBAT, 0, NULL);
    SetPauseState(PAUSE_STATE_PAUSED, 0, NULL);
    ClearPauseExclusionList();
    if (m_pModule != NULL)
    {
        delete m_pModule;
        m_pModule = NULL;
        g_pAppManager->GetObjectTableManager(0)->RemovePlayer(0);
        delete m_pGameObjectArray;
        CleanUpModuleServices();
        m_pGameObjectArray = new CGameObjectArray(1);
        g_pAppManager->GetClientExoApp()->SetAreaNotReadyToDisplay(1);
#ifndef _XBOX
        m_fEnemyAutoPauseDelay = 0.0f;
        m_fEnemyAutoPauseTimer = 0.0f;
        m_fCombatAutoPauseTimer = 0.0f;
        m_fMineSightedTime = 0;
        m_nDelayedAutoPauseType = AUTO_PAUSE_TYPE_UNSET;
        m_AutoPauseFlags.m_Bits.m_bAutoPaused = 0;
        m_bEnemySighted = 0;
        m_bMineSighted = 0;
        m_nCombatMode = 0;
#endif
    }
}
// PC 0x00643FE0 GOG 0x00643E90
int CClientExoAppInternal::Uninitialize()
{
    CSWCVisualEffect::UnloadCachedVFX();
    m_bModuleServicesActive = 0;

    CQueuedAnim* pAnimation;
    while (m_lstPlayerAnimations.m_pInternal->m_pHead != NULL)
    {
        pAnimation = (CQueuedAnim*)m_lstPlayerAnimations.m_pInternal->RemoveHead();
        if (pAnimation != NULL)
        {
            delete pAnimation;
        }
    }

    EmptyHitcheckIgnoreList();

    if (g_pDialogIconModel != NULL)
    {
        delete g_pDialogIconModel;
        g_pDialogIconModel = NULL;
    }
    if (g_pGroundRing != NULL)
    {
        delete g_pGroundRing;
        g_pGroundRing = NULL;
    }
    if (g_pRules != NULL)
    {
        delete g_pRules;
        g_pRules = NULL;
    }
    if (m_pGuiLoadScreen != NULL)
    {
        delete m_pGuiLoadScreen;
        m_pGuiLoadScreen = NULL;
    }
    if (m_pLegalScreen != NULL)
    {
        delete m_pLegalScreen;
        m_pLegalScreen = NULL;
    }

    return SetTexturePack(0xFF, 1);
}
// PC 0x00641EF0 GOG 0x00641DA0
void CClientExoAppInternal::EmptyHitcheckIgnoreList()
{
    unsigned long* pOid;

    while (m_lstHitcheckIgnore.GetHeadPos() != NULL)
    {
        pOid = (unsigned long*)m_lstHitcheckIgnore.RemoveHead();

        if (pOid != NULL)
        {
            delete pOid;
        }
    }
}

#ifndef _XBOX
int CClientExoAppInternal::StartServices(int bPlayLoadMusic,  // PC 0x006440C0 GOG 0x00643F70
                                        const CExoString& sModule,
                                        const CExoString& sLoadMusic,
                                        const CExoString& sLoadScreen,
                                        unsigned long nPlayerId)
{
    CExoIni cIni;
    CExoString sValue;
    CExoString sDialogFile;

    m_pUnknown1C0 = new CClientMsgSysAdmin;
    m_pWorldTimer = new CWorldTimer;
    m_pCombatTimer = new CWorldTimer;
    m_pPauseTimer = new CWorldTimer;
    m_pAnimationTimer = new CWorldTimer;
    m_pGameObjectArray = new CGameObjectArray(1);
    g_pExoResMan = new CExoResMan;

    if (cIni.ReadIniEntry(sValue, ".\\swkotor2.ini", "Graphics Options",
                          "AllowWindowedMode"))
        g_bAllowWindowedMode = sValue.AsINT() != 0;
    g_pAurora = NewCAurora();
    CreateWorldJournal();
    m_pUnknown10 = new CNetLayer;
    if (m_pServerInfo == NULL)
        m_pServerInfo = new CServerInfo;
    m_pSWCMessage = new CSWCMessage;
    m_pClientAIMaster = new CClientAIMaster;
    m_pClientAIMaster->SetExoAppInternal(this);

    if (cIni.ReadIniEntry(sValue, ".\\swkotor2.ini", "Game Options",
                          "EnableReleaseLogging"))
        g_pExoBase->m_pExoDebug->SetEnabled(sValue.AsINT() != 0);
    g_bExtendedResourceTracking = false;
    g_pExoBase->m_pExoDebug->OpenLogFiles("swc");

    g_pExoResMan->AddResourceDirectory("TEMPCLIENT:");
    g_pExoResMan->AddResourceDirectory("OVERRIDE:");
    g_pSWTlkTable = new CSWTlkTable;
    g_pTlkTable = g_pSWTlkTable;
    if (!bPlayLoadMusic)
    {
        g_pExoResMan->CacheResource("HD0:DATAXBOX\\templates", RESTYPE_BIF);
        g_pExoResMan->CacheResource("HD0:DATAXBOX\\gui", RESTYPE_BIF);
    }
    g_pExoResMan->AddFixedKeyTableFile("HD0:CHITIN");
    g_pExoResMan->AddResourceDirectory("RIMS:");
    g_pAuroraInterface = new CAuroraInterface;
    g_pExoResMan->AddEncapsulatedResourceFile("HD0:patch", 1);
    InitializeSoundOptions();
    g_pExoResMan->AddResourceDirectory("HD0:MOVIES");
    if (g_pExoResMan->Exists("GLOBAL", RESTYPE_RIM, (unsigned long*)NULL))
        g_pExoResMan->AddResourceImageFile("RIMS:GLOBAL");

    if (bPlayLoadMusic)
    {
        m_sLoadMusic = sLoadMusic;
        StartLoadMusic();
    }
    sDialogFile = g_pExoResMan->CacheResource("HD0:DIALOG", RESTYPE_TLK);
    g_pTlkTable->OpenFile(sDialogFile);
    AddDownloadedResources();
    m_pUnknown10->Initialize(g_pAppManager->GetClientExoApp());
    srand(GetTickCount());
    m_pModule = new CSWCModule;
    if (m_pModule != NULL)
    {
        int bTextureQuality = cIni.ReadIniEntry(sValue, ".\\swkotor2.ini",
                                               "Graphics Options", "Texture Quality");
        int nTextureQuality = sValue.AsINT();
        if (bTextureQuality && nTextureQuality < CClientOptions::TEXTURE_QUALITY_COUNT)
            SetTexturePack(nTextureQuality, 0);
        else
            SetTexturePack(0, 1);

        WIN32_FIND_DATA cFindData;
        memset(&cFindData, 0, sizeof(cFindData));
        HANDLE hFind = FindFirstFile("d:\\guilocal.erf", &cFindData);
        if (hFind != INVALID_HANDLE_VALUE)
        {
            g_pExoResMan->AddEncapsulatedResourceFile(
                g_pExoResMan->CacheResource("HD0:guilocal", RESTYPE_ERF), 2);
        }
        FindClose(hFind);

        int bFullScreen;
        if (cIni.ReadIniEntry(sValue, ".\\swkotor2.ini", "Graphics Options", "FullScreen"))
            bFullScreen = sValue.AsINT();
        else
        {
            bFullScreen = 0;
            sValue.Format("%d", bFullScreen);
            cIni.WriteIniEntry(sValue, ".\\swkotor2.ini", "Graphics Options", "FullScreen");
        }
        if (cIni.ReadIniEntry(sValue, ".\\swkotor2.ini", "Game Options", "Disable Movies"))
            m_bMovieDisabled = sValue.AsINT();
        else
            m_bMovieDisabled = 0;
        m_bSkipIntro = 0;
        int bPlayMovies = m_bMovieDisabled == 0;
        m_pClientOptions->SetDefaultOptions();
        LoadOptions(0);
        ReadAndSetVideoMode(bPlayMovies && bFullScreen, bFullScreen);

        int bReadEntry = cIni.ReadIniEntry(sValue, ".\\swkotor2.ini", "Graphics Options", "V-Sync");
        if (bReadEntry &&
            g_pAppManager->GetClientExoApp()->GetClientOptions() != NULL)
            g_pAppManager->GetClientExoApp()->GetClientOptions()->SetVSync(sValue.AsINT());
        bReadEntry = cIni.ReadIniEntry(sValue, ".\\swkotor2.ini", "Graphics Options", "Anisotropy");
        if (bReadEntry &&
            g_pAppManager->GetClientExoApp()->GetClientOptions() != NULL)
            g_pAppManager->GetClientExoApp()->GetClientOptions()->SetAnisotropy(sValue.AsINT());
        bReadEntry = cIni.ReadIniEntry(sValue, ".\\swkotor2.ini", "Graphics Options", "Anti Aliasing");
        if (bReadEntry &&
            g_pAppManager->GetClientExoApp()->GetClientOptions() != NULL)
        {
            g_pAppManager->GetClientExoApp()->GetClientOptions()->SetAntiAlias(sValue.AsINT());
            if (sValue.AsINT() > 0 && (!bPlayMovies || !bFullScreen))
                ReInitAurora(g_nScreenWidth, g_nScreenHeight, g_nBitsPerPixel,
                             g_pAppManager->GetClientExoApp()->GetClientOptions()->m_bFullScreen, 1);
        }
        m_sClientName = g_pTlkTable->GetSimpleString(0);
        m_nClientLanguage = g_pTlkTable->GetLanguageVersion();
        if (m_nClientLanguage == CTlkTable::LANGUAGE_POLISH)
            setlocale(LC_CTYPE, "Polish_Poland.1250");
        if (m_nClientLanguage >= CTlkTable::LANGUAGE_ASIAN_START)
        {
            m_bClientImeUI = 1;
            m_pImeUI = new CExoImeUI;
        }
        CreateQuestJournal();
        m_pSoundSetList = new CSoundSetList;
        LoadItemDropSoundList();
        InitializeClientOptions();
        m_pSWGuiManager = new CSWGuiManager;
#ifndef _XBOX
        m_pSWGuiManager->SetSize(g_nScreenWidth, g_nScreenHeight);
#endif
        if (g_pExoInput != NULL)
        {
            g_pExoInput->SetMousePos(g_nScreenWidth / 2, g_nScreenHeight / 2, false);
            if (g_pExoInput->GetMouseMode() == CExoInput::MOUSE_MODE_SOFTWARE)
                m_pSWGuiManager->ActivateSoftwareMouse();
        }
        SetMouseCursor(CSWGuiManager::CURSOR_DEFAULT, 0, 0);
        m_pSWGuiManager->LoadGuiSounds();
        m_pGuiInGame = new CGuiInGame;
        if (m_pGuiInGame != NULL)
            InitializeMoviePlayer();
    }
    return 0;
}
#endif
// PC 0x00645160 GOG 0x00645010
void CClientExoAppInternal::SetHitcheckIgnoreList(List<CAurObject*>* pIgnoreList)
{
    CExoLinkedListNode* pNode = m_lstHitcheckIgnore.GetHeadPos();
    unsigned long* pOid = m_lstHitcheckIgnore.GetHead();

    while (pNode != NULL)
    {
        if (pOid != NULL)
        {
            CGameObject* pGameObject = GetGameObject(*pOid);
            if (pGameObject != NULL)
            {
                CSWCObject* pObject = pGameObject->AsSWCObject();
                if (pObject != NULL)
                {
                    CSWCCreature* pCreature = pObject->AsSWCCreature();
                    CSWCItem* pItem = pObject->AsSWCItem();

                    if (pObject->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1) != NULL)
                    {
                        pIgnoreList->Add(pObject->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1));
                    }

                    if (pCreature != NULL)
                    {
                        CSWCCreatureAppearance* pAppearance = pCreature->GetAppearance();
                        if (pAppearance != NULL)
                        {
                            pItem = g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(
                                pAppearance->m_nUnmapped10);
                            if (pItem != NULL &&
                                pItem->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1) != NULL)
                            {
                                pIgnoreList->Add(pItem->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1));
                            }
                            if (pCreature->GetModel(CSWCAnimBase::MODEL_SLOT_HEAD, 1) != NULL)
                            {
                                pIgnoreList->Add(pCreature->GetModel(CSWCAnimBase::MODEL_SLOT_HEAD, 1));
                            }
                            if (pAppearance->m_nUnmapped10 == CGameObjectArray::INVALIDOBJID)
                            {
                                if (pCreature->GetModel(CSWCAnimBase::MODEL_SLOT_HEAD, 1) != NULL)
                                {
                                    pIgnoreList->Add(pCreature->GetModel(CSWCAnimBase::MODEL_SLOT_HEAD, 1));
                                }
                            }
                            else
                            {
                                pItem = g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(
                                    pAppearance->m_nUnmapped10);
                                if (pItem != NULL &&
                                    pItem->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1) != NULL)
                                {
                                    pIgnoreList->Add(pItem->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1));
                                }
                            }
                        }

                        pItem = g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(
                            pCreature->GetEquippedItemID(INVENTORY_SLOT_RIGHTHAND));
                        if (pItem != NULL)
                        {
                            CSWBaseItem* pBaseItem = g_pRules->GetBaseItemArray()->GetBaseItem(
                                pItem->GetBaseItemType());
                            if (pItem->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1) != NULL)
                            {
                                pIgnoreList->Add(pItem->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1));
                            }
                        }

                        pItem = g_pAppManager->GetClientExoApp()->GetItemByGameObjectID(
                            pCreature->GetEquippedItemID(INVENTORY_SLOT_LEFTHAND));
                        if (pItem != NULL &&
                            pItem->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1) != NULL)
                        {
                            pIgnoreList->Add(pItem->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1));
                        }
                    }
                }
            }
        }
        pOid = m_lstHitcheckIgnore.GetNext(pNode);
    }

    if (m_pModule != NULL && m_pModule->m_pInfo != NULL)
    {
        CExoLinkedList<CAurObject>* pIgnoreObjects = &m_pModule->m_pInfo->m_lstHitCheckIgnoreObjects;
        CExoLinkedListNode* pAreaNode = pIgnoreObjects->GetHeadPos();
        CAurObject* pIgnoreObject = pIgnoreObjects->GetHead();
        while (pAreaNode != NULL)
        {
            if (pIgnoreObject != NULL)
            {
                pIgnoreList->Add(pIgnoreObject);
            }
            pIgnoreObject = pIgnoreObjects->GetNext(pAreaNode);
        }
    }
}

#ifndef _XBOX
void CClientExoAppInternal::RestartMoviesIfNecessary()  // PC 0x00645490 GOG 0x00645340
{
    if (m_pMoviePlayer != NULL &&
        (unsigned long)m_pMoviePlayer->GetNumberOfMoviesToPlay() > 0)
    {
        HideMouse(8);
        ChangeResolutionForMovie();
        m_pMoviePlayer->RestartMoviesAsync();
        WaitForMovie(1);
        RestoreResolutionFromMovie();
        ShowMouse(8);

        m_pMoviePlayer->HideFadePanel();
    }
}
#endif
// PC 0x006454F0 GOG 0x006453A0
void CClientExoAppInternal::AddCreatureModelDirectlyToIgnoreList(
    unsigned long oidCreature, List<CAurObject*>* pIgnoreList)
{
    CSWCCreature* pCreature = GetCreatureByGameObjectID(oidCreature);

    if (pCreature != NULL)
    {
        Gob* pModel = pCreature->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1);
        if (pModel != NULL)
        {
            pIgnoreList->Add(pModel);
        }

        pModel = pCreature->GetModel(CSWCAnimBase::MODEL_SLOT_HEAD, 1);
        if (pModel != NULL)
        {
            pIgnoreList->Add(pModel);
        }

        CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
        CSWCItem* pItem = pClientExoApp->GetItemByGameObjectID(
            pCreature->GetEquippedItemID(INVENTORY_SLOT_HEAD));
        if (pItem != NULL)
        {
            pModel = pItem->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1);
            if (pModel != NULL)
            {
                pIgnoreList->Add(pModel);
            }
        }

        pClientExoApp = g_pAppManager->m_pClientExoApp;
        pItem = pClientExoApp->GetItemByGameObjectID(
            pCreature->GetEquippedItemID(INVENTORY_SLOT_RIGHTHAND));
        if (pItem != NULL)
        {
            for (unsigned char nModel = CSWCAnimBase::MODEL_SLOT_MAIN;
                 nModel >= CSWCAnimBase::MODEL_SLOT_ITEM_IGNORE_MIN; nModel--)
            {
                pModel = pItem->GetModel(nModel, 1);
                if (pModel != NULL)
                {
                    pIgnoreList->Add(pModel);
                }
            }
        }

        pClientExoApp = g_pAppManager->m_pClientExoApp;
        pItem = pClientExoApp->GetItemByGameObjectID(
            pCreature->GetEquippedItemID(INVENTORY_SLOT_LEFTHAND));
        if (pItem != NULL)
        {
            for (unsigned char nModel = CSWCAnimBase::MODEL_SLOT_MAIN;
                 nModel >= CSWCAnimBase::MODEL_SLOT_ITEM_IGNORE_MIN; nModel--)
            {
                pModel = pItem->GetModel(nModel, 1);
                if (pModel != NULL)
                {
                    pIgnoreList->Add(pModel);
                }
            }
        }
    }
}
// PC 0x00645700 GOG 0x006455B0
void CClientExoAppInternal::AddItemModelDirectlyToIgnoreList(
    unsigned long oidItem, List<CAurObject*>* pIgnoreList)
{
    CSWCItem* pItem = GetItemByGameObjectID(oidItem);

    if (pItem != NULL)
    {
        for (unsigned char nModel = CSWCAnimBase::MODEL_SLOT_MAIN;
             nModel >= 251; nModel--)
        {
            Gob* pModel = pItem->GetModel(nModel, 1);

            if (pModel != NULL)
            {
                pIgnoreList->Add(pModel);
            }
        }
    }
}
// PC 0x006457B0 GOG 0x00645660
void CClientExoAppInternal::AddSimpleObjectModelDirectlyToIgnoreList(
    unsigned long oidObject, List<CAurObject*>* pIgnoreList)
{
    CClientExoApp* pClientApp = g_pAppManager->m_pClientExoApp;
    CSWCObject* pObject = pClientApp->GetGameObject(oidObject);

    if (pObject != NULL)
    {
        pObject = pObject->AsSWCObject();

        if (pObject != NULL)
        {
            Gob* pModel = pObject->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1);

            if (pModel != NULL)
            {
                pIgnoreList->Add(pModel);
            }
        }
    }
}
// PC 0x00645800 GOG 0x006456B0
void CClientExoAppInternal::StartMenuMusic(int bCultTheme, int nVolume,
                                        int nPriorityGroup)
{
    CExoString sMusic("mus_sion");
    int bChangeMusic = m_pMenuMusic != NULL &&
                      m_pMenuMusic->GetFilePlaying() != sMusic;

    if (m_bStartMenuMusicOnMovieEnd || bChangeMusic)
    {
        m_bStartMenuMusicOnMovieEnd = 0;
        if (bChangeMusic)
        {
            StopMenuMusic();
        }

        if (m_pMenuMusic == NULL)
        {
            m_pMenuMusic = new CExoStreamingSoundSource;
            m_pMenuMusic->SetLooping(1);
            m_pMenuMusic->SetVolume(nVolume);
            m_pMenuMusic->SetPriorityGroup(nPriorityGroup);
#ifdef _XBOX

#define RESTYPE_XBOX_STREAM 11
            CExoString sFileName;
            sFileName.Format("HD0:STREAMMUSIC\%s", sMusic.CStr());
            g_pExoResMan->CacheResource(sFileName, RESTYPE_XBOX_STREAM);
            m_pMenuMusic->Play(sMusic, RESTYPE_XBOX_STREAM, 0, 1, 1);
        }
        else if (!m_pMenuMusic->IsPlaying())
        {
            m_pMenuMusic->Play(sMusic, RESTYPE_XBOX_STREAM, 0, 1, 1);
        }
#else
            CResRef cMusic(sMusic);
            m_pMenuMusic->Play(cMusic, RESTYPE_MP3, 0, 1, 0);
        }
        else if (!m_pMenuMusic->IsPlaying())
        {
            CResRef cMusic(sMusic);
            m_pMenuMusic->Play(cMusic, RESTYPE_MP3, 0, 1, 1);
        }
#endif
    }
}
// PC 0x00641F20 GOG 0x00641DD0
void CClientExoAppInternal::StopMenuMusic()
{
    if (m_pMenuMusic != NULL)
    {
#ifdef _XBOX
        unsigned long nStart =
            (unsigned long)(g_pExoBase->m_pExoTimers->GetHighResolutionTimer() /
                            1000);
        m_pMenuMusic->FadeAndStop(500);
        do
        {
            g_pExoSound->Render();
        } while ((unsigned long)(g_pExoBase->m_pExoTimers
                                     ->GetHighResolutionTimer() /
                                 1000) -
                     nStart <
                 500);
#else
        FadeAndStopStreamingSoundSource(m_pMenuMusic, 500);
#endif
        delete m_pMenuMusic;
        m_pMenuMusic = NULL;
    }
}
// PC 0x00641F60 GOG 0x00641E10
void CClientExoAppInternal::StartLoadMusic()
{
#ifdef _XBOX
    if (m_pMenuMusic != NULL)
    {
        StopMenuMusic();
    }
#else
    StopMenuMusic();
#endif

    if (g_pExoSound != NULL && m_aMoveToModuleMovies.num == 0)
    {
        if (m_pLoadMusic == NULL)
        {
            if (m_sLoadMusic != "")
            {
#ifdef _XBOX
                CExoString sFileName;
                sFileName.Format("HD0:STREAMMUSIC\\%s", m_sLoadMusic.CStr());
                {
                    g_pExoResMan->CacheResource(sFileName, RESTYPE_WAV);
                }
#endif

                m_pLoadMusic = new CExoStreamingSoundSource(1);
                m_pLoadMusic->SetLooping(1);
                m_pLoadMusic->SetPriorityGroup(
                    CExoStreamingSoundSource::PRIORITY_GROUP_MUSIC);
#ifdef _XBOX
                m_pLoadMusic->Play(m_sLoadMusic, RESTYPE_WAV, 0, 1, 1);
#else
                m_pLoadMusic->Play(m_sLoadMusic, RESTYPE_MP3, 0, 1, 0);
#endif
            }
            else
            {
                g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_PAUSE_ALL);
            }
        }
        else if (!m_pLoadMusic->IsPlaying())
        {
            m_pLoadMusic->Resume();
        }
    }
}
// PC 0x006420C0 GOG 0x00641F70
int CClientExoAppInternal::GetObjectName(unsigned long oidObject, CExoString& sName)
{
    CExoLinkedListNode* pNode;
    int nStrRef;
    STR_RES strRes;
    CGameObject* pObject = GetGameObject(oidObject);

    if (pObject == NULL)
    {
        pNode = m_lstPlayerList.GetHeadPos();
        while (pNode != NULL)
        {
            CSWCPlayer* pPlayer = (CSWCPlayer*)m_lstPlayerList.GetAtPos(pNode);
            if (pPlayer != NULL && pPlayer->GetPrimaryPC() == oidObject)
            {
                sName = pPlayer->m_sPrimaryPCName;
                return 1;
            }
            m_lstPlayerList.GetNext(pNode);
        }
        sName = "";
        return 0;
    }
    else if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
    {
        sName = pObject->AsSWCCreature()->GetStats()->m_sFirstName;
        return 1;
    }
    else if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
    {
        if (pObject->AsSWCPlaceable()->GetName() == "")
        {
            int nAppearance = pObject->AsSWCPlaceable()->m_nAppearance;
            g_pRules->Get2DArrays()->m_pPlaceables->GetINTEntry(
                nAppearance,
                CTwoDimArrays::COL2DA_PLACEABLES_STRREF, &nStrRef);
            g_pTlkTable->Fetch(nStrRef, strRes, CTlkTable::GENDER_MALE);
            sName = strRes.m_sString;
        }
        else
        {
            sName = pObject->AsSWCPlaceable()->GetName();
        }
        return 1;
    }
    else if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR)
    {
        sName = pObject->AsSWCDoor()->GetName();
        return 1;
    }
    else if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_ITEM)
    {
        sName = pObject->AsSWCItem()->GetName(1);
        return 1;
    }
    else if (pObject->m_nObjectType == CGameObject::OBJECT_TYPE_TRIGGER)
    {
        sName = pObject->AsSWCTrigger()->GetName();
        if (sName == "")
        {
            STR_RES strMine;
            g_pTlkTable->Fetch(CSWCTrigger::STRREF_MINE, strMine, CTlkTable::GENDER_MALE);
            sName = strMine.m_sString;
        }
    }
    else
    {
        sName = "";
        return 0;
    }
    return 1;
}

// PC 0x0063C370 GOG 0x0063C270
CSWCItem* CClientExoAppInternal::GetItemByGameObjectID(unsigned long nId)
{
    CGameObject* pObject = NULL;

    if (m_pGameObjectArray->GetGameObject(nId, &pObject) ==
            CGameObjectArray::SUCCESS &&
        pObject != NULL &&
        pObject->m_nObjectType == CGameObject::OBJECT_TYPE_ITEM)
    {
        return pObject->AsSWCItem();
    }

    return NULL;
}

#ifndef _XBOX
void CClientExoAppInternal::QueryExitProgram()  // PC 0x006423C0 GOG 0x00642270
{
    if (g_pAppManager->GetClientExoApp() == NULL
        || g_pAppManager->GetClientExoApp()->GetInGameGui() == NULL
        || g_pAppManager->GetClientExoApp()->GetInGameGui()->m_nC0 != 0)
    {
        return;
    }

    g_pAppManager->GetClientExoApp()->StopMovie();

    if (m_pClientOptions->m_nCameraMode ==
        CClientOptions::CAMERA_MODE_WORLD_CLICK_DISABLED)
    {
        if (GetPlayerCreature() != NULL
            && GetPlayerCreature()->m_nCombatState == 0
            && m_nGameOver == 0
            && m_pModule != NULL)
        {
            g_bShowIndicator = 1;
            m_pModule->RestoreCamera();
            SetInputClass(INPUT_CLASS_GAME, 1);
        }
    }

#ifdef _XBOX
    if (m_PauseFlags.m_Bits.m_bAutoPaused == 0)
#else
    if (m_AutoPauseFlags.m_Bits.m_bAutoPaused == 0)
#endif
    {
        if (m_pGuiInGame->m_nGlobalDialogState == 0)
        {
            CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
            int nPauseState;
            if (pServerExoApp != NULL)
            {
                nPauseState = pServerExoApp->GetPauseState(PAUSE_STATE_COMBAT);
            }
            else
            {
                nPauseState = 0;
            }
            if (m_PauseFlags.m_Bits.m_bTogglePauseAtEndOfFrame == 0
                && nPauseState != PAUSE_STATE_PAUSED)
            {
                m_PauseFlags.m_Bits.m_bTogglePauseAtEndOfFrame = 1;
                m_nTogglePauseType = 4;
                m_bTogglePause = 1;
                g_pExoInput->PauseRumble();
                g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_PAUSE);
                m_nSoundPausedByCombat = 1;
            }
        }

        SetAutoPaused(1, 0);
    }

    if (m_pModule != NULL && m_pModule->GetMiniGame() != NULL)
    {
        ShowMouse(MOUSE_HIDE_MINIGAME);
    }

    CSWGuiMessageBox* pMessageBox =
        g_pAppManager->GetClientExoApp()->GetInGameGui()->m_pMessageBox;
    pMessageBox->SetAllowCancel(1);
    pMessageBox->SetMessage(0xA56C);
    g_pAppManager->GetClientExoApp()->GetSWGuiManager()->AddPanel(
        pMessageBox, 1, 1);
    pMessageBox->SetCallback(
        pMessageBox, (CSWGuiHandler)&CClientExoAppInternal::HandleExitGameButton, NULL);
    g_pExoInput->SetActive(1);
    g_pAppManager->GetClientExoApp()->EnableInput();
    g_pAppManager->GetClientExoApp()->SetInputClass(INPUT_CLASS_GUI, 1);
}
#endif
// PC 0x00642580 GOG 0x00642430
void CClientExoAppInternal::SetInputActive(int nA, int nB)
{
    if (g_pExoInput != NULL)
    {
        g_pExoInput->SetActive(nA);
    }

    if (nA == 0)
    {
        TurnOffKeyboardModal();
    }

    if (nB != 0)
    {
        g_pExoInput->ClearEvents();
    }

#ifndef _XBOX
    if (nA != 0)
    {
        ShowMouse(0);
    }
#endif
}
// PC 0x006425D0 GOG 0x00642480
void CClientExoAppInternal::ChangeCharacter(int nCharacter, int bPlaySound)
{
    CSWCCreature* pCharacter = m_pSWParty->GetPlayerCharacter();
    m_pSWParty->SetLeader(nCharacter);
    if (pCharacter != m_pSWParty->GetPlayerCharacter())
    {
        pCharacter = m_pSWParty->GetPlayerCharacter();
        Vector vPosition = pCharacter->m_vPosition;
        m_pSWParty->ResetAllFollowData(vPosition, Yaw(pCharacter->m_vOrientation), 1);
        CSWSCreature* pServerCreature = pCharacter->GetServerCreature();
        if (pServerCreature != NULL)
        {
            pServerCreature->m_pPathfindInformation->m_nField_34 = OBJECT_INVALID;
            pServerCreature->RemoveAction(CSWSObject::ACTION_FOLLOW_LEADER);
            if (pServerCreature->GetDead() || pServerCreature->GetDeadTemp())
            {
                bPlaySound = 0;
            }
        }
        m_pModule->SetCameraFollow(pCharacter->m_nId, 1);
        if (bPlaySound)
        {
#ifdef _XBOX
            int nRoll = rand() % 3;
            if (nRoll == 0)
            {
                pCharacter->PlaySoundSetSound(7, 1);
            }
            else if (nRoll == 1)
            {
                pCharacter->PlaySoundSetSound(8, 1);
            }
            else
            {
                pCharacter->PlaySoundSetSound(9, 1);
            }
#else
            PlayCharacterSelectSound(pCharacter);
#endif
        }
        if (m_pPlayerControl != NULL)
        {
            m_pPlayerControl->ResetDriveAcceleration();
        }
#ifndef _XBOX
        m_bCharacterChangeInProgress = 1;
#endif
    }
}

static inline int ClientexoappinternalHiliteCount(CExoArrayList<CHostileHilite>* pList)
{
    return pList->num;
}
// PC 0x00645960 GOG 0x00645810
void CClientExoAppInternal::ShowObject(CSWCObject* pObject, int bLookAt,
                                      int bMoveCamera, int bUnused)
{
    IndicateObject(pObject, 0);
    if (pObject == NULL)
    {
        CSWCCreature* pPlayer = m_pSWParty->GetCharacter(0);
        if (pPlayer != NULL)
            pPlayer->LookAt(OBJECT_INVALID, CLIENT_OBJECT_LOOK_DURATION);
        return;
    }

    if (bLookAt)
    {
        CSWCCreature* pPlayer = m_pSWParty->GetCharacter(0);
        if (pPlayer != NULL)
            pPlayer->LookAt(pObject->GetId(), CLIENT_OBJECT_LOOK_DURATION);
        if (bMoveCamera || m_nCombatMode)
        {
            CSWCameraOnAStick* pCamera = (CSWCameraOnAStick*)
                GetModule()->GetCamera()->GetBehavior(AUR_BEHAVIOR_CAMERA_ONASTICK);
            if (pCamera != NULL)
                pCamera->SetLookAtTarget(pObject);
            if (m_nCombatMode && pObject->AsSWCCreature() != NULL &&
                pObject->AsSWCCreature()->IsHostile())
            {
                int bFound = 0;
                pObject->Hilite(CGuiInGame::COLOR_RED,
                                CSWCObject::HILITE_CHANNEL_HOSTILE, 0.0f, 1);
                for (int i = 0; i < ClientexoappinternalHiliteCount(&m_aHostileHilites) && !bFound; i++)
                {
                    if (m_aHostileHilites[i].m_oidObject == pObject->GetId())
                    {
                        m_aHostileHilites[i].m_fTimeRemaining =
                            CLIENT_HOSTILE_HILITE_DURATION;
                        bFound = 1;
                    }
                }
                if (!bFound)
                {
                    int nIndex = m_aHostileHilites.num;
                    m_aHostileHilites.SetSize(nIndex + 1);
                    m_aHostileHilites[nIndex].m_oidObject = pObject->GetId();
                    m_aHostileHilites[nIndex].m_fTimeRemaining =
                        CLIENT_HOSTILE_HILITE_DURATION;
                }
            }
        }
    }

    if (RenderDEV)
    {
        CSWSObject* pServer = pObject->GetServerObject();
        if (LockRenderDev != 0)
        {
            if (LockRenderDev == CLIENT_RENDER_LOCK_PLAYER)
                LockRenderDev = g_pAppManager->GetServerExoApp()->GetPlayerCreatureId();
            CGameObject* pLocked = g_pAppManager->GetServerExoApp()
                                      ->GetGameObject(LockRenderDev);
            if (pLocked != NULL && pLocked->AsSWSObject() != NULL)
                pServer = pLocked->AsSWSObject();
        }
        if (pServer != NULL)
        {
            pServer->m_sTag.CStr();
            CSWSCreature* pCreature = pServer->AsSWSCreature();
            if (pCreature != NULL)
            {
                int nPoints = pCreature->m_pPathfindInformation->m_nWayPointCount / 2;
                float* pPoints = pCreature->m_pPathfindInformation->m_pWayPoints;
                if (ShowPath && pPoints != NULL)
                {

                    static int nPathNumbers[CLIENT_DEBUG_PATH_SQUARES];
                    static int InitArray = 1;
                    for (int i = 0; i < CLIENT_DEBUG_PATH_SQUARES; i++)
                    {
                        if (InitArray)
                            nPathNumbers[i] = -1;
                        AurRemoveDrawableSquare(nPathNumbers[i]);
                        nPathNumbers[i] = -1;
                    }
                    InitArray = 0;
                    Vector vStart(pPoints[0], pPoints[1], 0.0f);
                    vStart.m_fZ = g_pAppManager->GetServerExoApp()->ComputeHeight(
                        g_pAppManager->GetServerExoApp()->GetModule()->GetArea()->GetResRef(),
                        vStart, 0);
                    for (int i = 1; i < nPoints; i++)
                    {
                        Vector vEnd(pPoints[i * 2], pPoints[i * 2 + 1], 0.0f);
                        vEnd.m_fZ = g_pAppManager->GetServerExoApp()->ComputeHeight(
                            g_pAppManager->GetServerExoApp()->GetModule()->GetArea()->GetResRef(),
                            vEnd, 0);
                        Vector avSquare[4] = {vStart, vStart, vEnd, vEnd};
                        nPathNumbers[i - 1] = (AurAddDrawableSquare)((float*)avSquare, AUR_DRAWABLE_COLOR_WHITE,
                                                                     "clientexoappinternal.cpp", 13250);
                    }
                }
                if (pCreature->m_lstActions.GetHeadPos() != NULL)
                {
                    CExoLinkedListNode* pNode = pCreature->m_lstActions.GetHeadPos();
                    int nActions = 0;
                    while (pNode != NULL)
                    {
                        CSWSObjectActionNode* pAction = pCreature->m_lstActions.GetAtPos(pNode);
                        pCreature->m_lstActions.GetNext(pNode);
                        if (++nActions > 5)
                            break;
                    }
                }
                pCreature->GetSpotCheckDistance();
                pCreature->GetListenCheckDistance();
                CSWSCreature* pPlayer = g_pAppManager->GetClientExoApp()
                    ->GetSWParty()->GetCharacter(0)->GetServerCreature();
                magnitude(pCreature->m_vPosition - pPlayer->m_vPosition);
                pCreature->GetLockOrientationToObject();
                pCreature->GetClientCreature()->GetCurrentAnimation();
                if (pCreature->m_bInParty)
                {
                    unsigned long oidCreature = pCreature->m_nId;
#ifdef _XBOX
                    int nXboxIndex = g_pAppManager->GetClientExoApp()->GetSWParty()->GetIndex(
                        g_pAppManager->GetClientExoApp()->ServerToClientObjectId(
                            oidCreature));
                    g_pAppManager->GetClientExoApp()->GetSWParty();
                    EXOXBOXASSERT(nXboxIndex >= 0 && nXboxIndex < 3);
#else
                    g_pAppManager->GetClientExoApp()->GetSWParty()->GetIndex(
                        g_pAppManager->GetClientExoApp()->ServerToClientObjectId(
                            oidCreature));
                    g_pAppManager->GetClientExoApp()->GetSWParty();
#endif
                }
            }
            CSWSPlaceable* pPlaceable = pServer->AsSWSPlaceable();
            if (pPlaceable != NULL)
            {
                {
                    Vector vPoint1 = pPlaceable->m_cSurfaceMesh.GetUsePoint(0);
                    Vector vPoint2 = pPlaceable->m_cSurfaceMesh.GetUsePoint(0);
                }
                {
                    Vector vPoint1 = pPlaceable->m_cSurfaceMesh.GetUsePoint(1);
                    Vector vPoint2 = pPlaceable->m_cSurfaceMesh.GetUsePoint(1);
                }
            }
            CSWSDoor* pDoor = pServer->AsSWSDoor();
            if (pDoor != NULL)
            {
                {
                    Vector vPoint1 = pDoor->m_apCollisionMesh[0]->LocalToWorld(pDoor->m_apCollisionMesh[0]->m_avUsePoints[0]);
                    Vector vPoint2 = pDoor->m_apCollisionMesh[0]->LocalToWorld(pDoor->m_apCollisionMesh[0]->m_avUsePoints[0]);
                    Vector vPoint3 = pDoor->m_apCollisionMesh[0]->LocalToWorld(pDoor->m_apCollisionMesh[0]->m_avUsePoints[0]);
                }
                {
                    Vector vPoint1 = pDoor->m_apCollisionMesh[0]->LocalToWorld(pDoor->m_apCollisionMesh[0]->m_avUsePoints[1]);
                    Vector vPoint2 = pDoor->m_apCollisionMesh[0]->LocalToWorld(pDoor->m_apCollisionMesh[0]->m_avUsePoints[1]);
                    Vector vPoint3 = pDoor->m_apCollisionMesh[0]->LocalToWorld(pDoor->m_apCollisionMesh[0]->m_avUsePoints[1]);
                }
                {
                    Vector vPoint1 = pDoor->m_apCollisionMesh[1]->LocalToWorld(pDoor->m_apCollisionMesh[1]->m_avUsePoints[0]);
                    Vector vPoint2 = pDoor->m_apCollisionMesh[1]->LocalToWorld(pDoor->m_apCollisionMesh[1]->m_avUsePoints[0]);
                    Vector vPoint3 = pDoor->m_apCollisionMesh[1]->LocalToWorld(pDoor->m_apCollisionMesh[1]->m_avUsePoints[0]);
                }
                {
                    Vector vPoint1 = pDoor->m_apCollisionMesh[1]->LocalToWorld(pDoor->m_apCollisionMesh[1]->m_avUsePoints[1]);
                    Vector vPoint2 = pDoor->m_apCollisionMesh[1]->LocalToWorld(pDoor->m_apCollisionMesh[1]->m_avUsePoints[1]);
                    Vector vPoint3 = pDoor->m_apCollisionMesh[1]->LocalToWorld(pDoor->m_apCollisionMesh[1]->m_avUsePoints[1]);
                }
                {
                    Vector vPoint1 = pDoor->m_apCollisionMesh[2]->LocalToWorld(pDoor->m_apCollisionMesh[2]->m_avUsePoints[0]);
                    Vector vPoint2 = pDoor->m_apCollisionMesh[2]->LocalToWorld(pDoor->m_apCollisionMesh[2]->m_avUsePoints[0]);
                    Vector vPoint3 = pDoor->m_apCollisionMesh[2]->LocalToWorld(pDoor->m_apCollisionMesh[2]->m_avUsePoints[0]);
                }
                {
                    Vector vPoint1 = pDoor->m_apCollisionMesh[2]->LocalToWorld(pDoor->m_apCollisionMesh[2]->m_avUsePoints[1]);
                    Vector vPoint2 = pDoor->m_apCollisionMesh[2]->LocalToWorld(pDoor->m_apCollisionMesh[2]->m_avUsePoints[1]);
                    Vector vPoint3 = pDoor->m_apCollisionMesh[2]->LocalToWorld(pDoor->m_apCollisionMesh[2]->m_avUsePoints[1]);
                }
            }
        }
    }
    if (RenderQA)
    {
        CSWSObject* pServer = pObject->GetServerObject();
        if (pServer != NULL)
        {
            pServer->m_sTag.CStr();
            CExoString sDialog;
            pServer->GetDialogResref().CopyToString(sDialog);
            sDialog.CStr();
            if (pServer->AsSWSCreature() != NULL)
            {
                pServer->AsSWSCreature()->m_nCurrentAction;
                CSWSCreature* pVarOwner = pServer->AsSWSCreature();
                pVarOwner->m_lVarTable.GetLocalBoolean(62);
                pServer->AsSWSCreature()->GetUserActionsPending();
                pServer->AsSWSCreature()->m_nCurrentHitPoints;
                pServer->AsSWSCreature()->GetStats()->GetCurrentForcePoints();
                pServer->AsSWSCreature()->GetMaxForcePoints();
                for (int i = 0; i < g_pRules->m_pSpellArray->GetNumSpells(); i++)
                {
                    if (pServer->AsSWSCreature()->GetStats()->HasSpell(0, i, 1) &&
                        g_pRules->m_pSpellArray->GetSpell(i) != NULL)
                    {
                        CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(i);
                        pSpell->m_sLabel.CStr();
                    }
                }
                for (int i = 0; i < g_pRules->m_nNumFeats; i++)
                {
                    if (pServer->AsSWSCreature()->GetStats()->HasFeat(i) &&
                        g_pRules->GetFeat(i) != NULL)
                        g_pRules->GetFeat(i)->GetNameText().CStr();
                }
                pServer->AsSWSCreature()->GetStats()->GetSkillRank(CSWSkill::SKILL_COMPUTER_USE, NULL, 0);
                pServer->AsSWSCreature()->GetStats()->GetSkillRank(CSWSkill::SKILL_DEMOLITIONS, NULL, 0);
                pServer->AsSWSCreature()->GetStats()->GetSkillRank(CSWSkill::SKILL_STEALTH, NULL, 0);
                pServer->AsSWSCreature()->GetStats()->GetSkillRank(CSWSkill::SKILL_AWARENESS, NULL, 0);
                pServer->AsSWSCreature()->GetStats()->GetSkillRank(CSWSkill::SKILL_PERSUADE, NULL, 0);
                pServer->AsSWSCreature()->GetStats()->GetSkillRank(CSWSkill::SKILL_REPAIR, NULL, 0);
                pServer->AsSWSCreature()->GetStats()->GetSkillRank(CSWSkill::SKILL_SECURITY, NULL, 0);
                pServer->AsSWSCreature()->GetStats()->GetSkillRank(CSWSkill::SKILL_TREAT_INJURY, NULL, 0);
            }
            if (pServer->AsSWSDoor() != NULL)
                pServer->AsSWSDoor()->m_nOpenState;
        }
    }
}
// PC 0x006462B0 GOG 0x00646160
void CClientExoAppInternal::DoPassiveSelection(float fDeltaTime)
{
    CSWCCreature* pPlayer = m_pSWParty->GetPlayerCharacter();
    int bShown = 0;
    if (pPlayer != NULL)
    {
        CSWSCreature* pServerCreature = pPlayer->GetServerCreature();
        if (pServerCreature != NULL)
        {
            CExoArrayList<unsigned long> aForward;
            CExoArrayList<unsigned long> aNearest;
            pServerCreature->GetNearestObjects(CLIENT_PASSIVE_SELECTION_RANGE,
                CLIENT_PASSIVE_SELECTION_HEIGHT, aForward, aNearest);
            int nObjects = aNearest.num;
            m_aNearestObjects.num = 0;
            m_aNearestObjects.SetSize(nObjects);
            if (nObjects != 0)
            {
                int i;
                int bLastTargetFound = 0;
                int bNewTarget = 0;
                int bHostileCreature = 0;
                int bNoEnemy = 1;
                int bNoMine = 1;
                for (i = 0; i < nObjects; i++)
                {
                    m_aNearestObjects[i].m_bInFront = aForward.Contains(aNearest[i]);
                    aNearest[i] = ServerToClientObjectId(aNearest[i]);
                    m_aNearestObjects[i].m_oidObject = aNearest[i];
                    m_aNearestObjects[i].m_nCanSee = CNearestStruct::CAN_SEE_UNKNOWN;
                    if (!bHostileCreature)
                    {
                        CGameObject* pObject = GetGameObject(m_aNearestObjects[i].m_oidObject);
                        if (pObject != NULL)
                        {
                            if (pObject->AsSWCCreature() != NULL &&
                                pObject->AsSWCCreature()->IsHostile())
                            {
                                bNoEnemy = 0;
                                bHostileCreature = 1;
                            }
                            else if (pObject->AsSWCTrigger() != NULL &&
                                     pObject->AsSWCTrigger()->IsHostile())
                            {
                                bNoMine = 0;
                            }
                        }
                    }
                    if (m_aNearestObjects[i].m_oidObject == m_nLastTarget)
                        bLastTargetFound = 1;
                }

                if (bHostileCreature && m_nCombatMode && !bLastTargetFound)
                {
                    if (m_fTargetChanging <= 0.0f)
                    {
                        CSWSCreature* pTarget = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(
                            g_pAppManager->GetServerExoApp()->ClientToServerObjectId(m_nLastTarget));
                        if (pTarget != NULL && (pTarget->GetDead() || pTarget->GetDeadTemp()))
                        {

                            m_fTargetChanging = CLIENT_TARGET_CHANGE_DELAY;
                            bLastTargetFound = 1;
                        }
                    }
                    else if ((m_fTargetChanging -= fDeltaTime) > 0.0f)
                    {
                        CSWSCreature* pTarget = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(
                            g_pAppManager->GetServerExoApp()->ClientToServerObjectId(m_nLastTarget));
                        if (pTarget != NULL && (pTarget->GetDead() || pTarget->GetDeadTemp()))
                            bLastTargetFound = 1;
                        else
                            m_fTargetChanging = 0.0f;
                    }
                }
                else
                    m_fTargetChanging = 0.0f;

                int bEnemyVisible = 0;
                int bMineVisible = 0;
                if (!bNoEnemy || !bNoMine)
                {
                    for (i = 0; i < m_aNearestObjects.num; i++)
                    {
                        CGameObject* pObject = GetGameObject(m_aNearestObjects[i].m_oidObject);
                        if (pObject != NULL && pObject->AsSWCObject() != NULL &&
                            pObject->AsSWCObject()->IsHostile())
                        {
                            if (m_aNearestObjects[i].m_nCanSee == CNearestStruct::CAN_SEE_UNKNOWN)
                                m_aNearestObjects[i].m_nCanSee = pPlayer->CanSee(pObject->AsSWCObject());
                            if (m_aNearestObjects[i].m_nCanSee)
                            {
                                if (pObject->AsSWCTrigger() != NULL && pObject->AsSWCTrigger()->m_bTrap)
                                    bMineVisible = 1;
                                else
                                    bEnemyVisible = 1;
#ifdef _XBOX
                                if (!GetPausedByCombat())
#else
                                if (!GetPausedByCombat() && m_nDelayedAutoPauseType == AUTO_PAUSE_TYPE_UNSET)
#endif
                                {
                                    if (pObject->AsSWCTrigger() != NULL && pObject->AsSWCTrigger()->m_bTrap)
                                    {
                                        bNoMine = 1;
#ifndef _XBOX
                                        if (!m_bMineSighted)
                                        {
                                            CClientOptions* pOptions = m_pClientOptions;
                                            if (pOptions->m_bAutopauseMineSighted && !m_nCombatMode)
                                                SetAutoPaused(1, CClientExoApp::AUTOPAUSE_MINE_SIGHTED);
                                            m_nLastTarget = m_aNearestObjects[i].m_oidObject;
                                            bLastTargetFound = 1;
                                            bNewTarget = 1;
                                            break;
                                        }
#endif
                                    }
                                    else
                                    {
                                        bNoEnemy = 1;
                                        if (!m_bEnemySighted)
                                        {
                                            g_pAppManager->GetClientExoApp()->ShowTutorialWindow(
                                                CClientExoApp::TUTORIAL_ENEMY_SIGHTED, OBJECT_INVALID, OBJECT_INVALID, 0);
                                            CClientOptions* pOptions = m_pClientOptions;
                                            if (pOptions->m_bAutopauseEnemySighted && !m_nCombatMode)
                                                SetAutoPaused(1, CClientExoApp::AUTOPAUSE_ENEMY_SIGHTED);
                                            m_nLastTarget = m_aNearestObjects[i].m_oidObject;
                                            bLastTargetFound = 1;
                                            bNewTarget = 1;
                                            break;
                                        }
                                    }
                                }
                                if (bNoEnemy && bNoMine)
                                    break;
                            }
                        }
                    }
                }
                if (!m_bEnemySighted || bEnemyVisible)
                {
                    m_bEnemySighted = bEnemyVisible;
                    m_fCombatAutoPauseTimer = CLIENT_SIGHTED_LINGER_TIME;
                }
                else if (m_fCombatAutoPauseTimer < 0.0f)
                    m_bEnemySighted = 0;
                else
                    m_fCombatAutoPauseTimer -= fDeltaTime;
#ifndef _XBOX
                if (!m_bMineSighted || bMineVisible)
                {
                    m_bMineSighted = bMineVisible;
                    m_fMineSightedTime = CLIENT_SIGHTED_LINGER_TIME;
                }
                else if (m_fMineSightedTime < 0.0f)
                    m_bMineSighted = 0;
                else
                    m_fMineSightedTime -= fDeltaTime;
#endif

                if (bLastTargetFound)
                {
                    CGameObject* pObject = GetGameObject(m_nLastTarget);
                    if (pObject != NULL)
                    {
                        int nIndex = aNearest.IndexOf((unsigned long)m_nLastTarget);
                        if (nIndex != -1 && m_aNearestObjects[nIndex].m_nCanSee == CNearestStruct::CAN_SEE_UNKNOWN)
                            m_aNearestObjects[nIndex].m_nCanSee = pPlayer->CanSee(pObject->AsSWCObject());
                        int bCanSee;
                        if (nIndex == -1)
                            bCanSee = pPlayer->CanSee(pObject->AsSWCObject());
                        else
                            bCanSee = m_aNearestObjects[nIndex].m_nCanSee;
                        if (bCanSee)
                        {
                            m_fTargetInvisibleTime = 0.0f;
                            ShowObject(pObject->AsSWCObject(), bNewTarget || m_PauseFlags.m_Bits.m_bLastTargetChanged,
                                       bNewTarget, 0);
                            bShown = 1;
                        }
                        else
                        {
                            m_fTargetInvisibleTime += fDeltaTime;
                            if (m_fTargetInvisibleTime < CLIENT_TARGET_INVISIBLE_LIMIT)
                            {
                                ShowObject(pObject->AsSWCObject(), 0, 0, 0);
                                bShown = 1;
                            }
                            else
                                m_fTargetInvisibleTime = 0.0f;
                        }
                    }
                }
                if (!bShown)
                {
                    if (m_nCombatMode && bHostileCreature)
                    {
                        CGameObject* pNonHostile = NULL;
                        for (i = 0; i < aForward.num; i++)
                        {
                            aForward[i] = ServerToClientObjectId(aForward[i]);
                            CGameObject* pObject = GetGameObject(aForward[i]);
                            if (pObject != NULL && pObject->AsSWCObject() != NULL)
                            {
                                if (pObject->AsSWCObject()->IsHostile())
                                {
                                    int nIndex = aNearest.IndexOf(aForward[i]);
                                    if (m_aNearestObjects[nIndex].m_nCanSee == CNearestStruct::CAN_SEE_UNKNOWN)
                                        m_aNearestObjects[nIndex].m_nCanSee = pPlayer->CanSee(pObject->AsSWCObject());
                                    if (m_aNearestObjects[nIndex].m_nCanSee)
                                    {
                                        m_nLastTarget = aForward[i];
                                        ShowObject(pObject->AsSWCObject(), 1, 0, 0);
                                        bShown = 1;
                                        break;
                                    }
                                }
                                else if (pNonHostile == NULL)
                                    pNonHostile = pObject;
                            }
                        }
                        if (!bShown)
                        for (i = 0; i < m_aNearestObjects.num; i++)
                        {
                            CGameObject* pObject = GetGameObject(m_aNearestObjects[i].m_oidObject);
                            if (pObject != NULL && pObject->AsSWCObject() != NULL && pObject->AsSWCObject()->IsHostile())
                            {
                                if (m_aNearestObjects[i].m_nCanSee == CNearestStruct::CAN_SEE_UNKNOWN)
                                    m_aNearestObjects[i].m_nCanSee = pPlayer->CanSee(pObject->AsSWCObject());
                                if (m_aNearestObjects[i].m_nCanSee)
                                {
                                    m_nLastTarget = m_aNearestObjects[i].m_oidObject;
                                    ShowObject(pObject->AsSWCObject(), 1, 0, 0);
                                    bShown = 1;
                                    break;
                                }
                            }
                        }
                        if (!bShown && pNonHostile != NULL)
                        {
                            m_nLastTarget = pNonHostile->GetId();
                            ShowObject(pNonHostile->AsSWCObject(), 1, 0, 0);
                            bShown = 1;
                        }
                    }
                    else
                    {
                        for (i = 0; i < aForward.num; i++)
                        {
                            aForward[i] = ServerToClientObjectId(aForward[i]);
                            CGameObject* pObject = GetGameObject(aForward[i]);
                            if (pObject != NULL)
                            {
                                int nIndex = aNearest.IndexOf(aForward[i]);
                                if (m_aNearestObjects[nIndex].m_nCanSee == CNearestStruct::CAN_SEE_UNKNOWN)
                                    m_aNearestObjects[nIndex].m_nCanSee = pPlayer->CanSee(pObject->AsSWCObject());
                                if (m_aNearestObjects[nIndex].m_nCanSee)
                                {
                                    m_nLastTarget = aForward[i];
                                    ShowObject(pObject->AsSWCObject(), 1, 0, 0);
                                    bShown = 1;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    if (!bShown)
    {
        m_nLastTarget = OBJECT_INVALID;
        ShowObject(NULL, 0, 0, 0);
    }
    if (m_pSWParty != NULL)
        m_pSWParty->m_aPartyMemberData[0].m_oidCombatTarget = m_nLastTarget;
    m_PauseFlags.m_Bits.m_bLastTargetChanged = 0;
}
// PC 0x00646D20 GOG 0x00646BD0
CSWCObject* CClientExoAppInternal::SelectNearestObject(int bReverse,
                                                       unsigned char nFilter)
{
    ShowTutorialWindow(8, OBJECT_INVALID, OBJECT_INVALID, 0);

    if (m_aNearestObjects.num == 0)
    {
        return NULL;
    }

    CSWCObject* pObject;
    if (nFilter != 0)
    {
        int bHostileVisible = FALSE;
        for (int i = m_aNearestObjects.num - 1; i >= 0; i--)
        {
            CGameObject* pGameObject =
                GetGameObject(m_aNearestObjects[i].m_oidObject);
            if (pGameObject != NULL)
            {
                pObject = pGameObject->AsSWCObject();
                if (pObject != NULL)
                {
                    if (m_aNearestObjects[i].m_nCanSee ==
                        CNearestStruct::CAN_SEE_UNKNOWN)
                    {
                        m_aNearestObjects[i].m_nCanSee =
                            m_pSWParty->GetPlayerCharacter()->CanSee(pObject);
                    }

                    if (m_aNearestObjects[i].m_nCanSee)
                    {
                        if (pObject->AsSWCCreature() != NULL &&
                            pObject->IsHostile())
                        {
                            bHostileVisible = TRUE;
                            break;
                        }
                        continue;
                    }
                }
            }
            else
            {
                pObject = NULL;
            }

            m_aNearestObjects.DelIndex(i);
        }

        if (!bHostileVisible)
        {
            nFilter = 0;
        }
    }

    int nIndex;
    if (m_nLastTarget != OBJECT_INVALID &&
        (nIndex = m_aNearestObjects.IndexOf((void*)m_nLastTarget,
                                            TestNearestStruct)) >= 0 &&
        nIndex < m_aNearestObjects.num)
    {
        if (bReverse)
        {
            if (nIndex != 0)
                nIndex--;
            else
                nIndex = m_aNearestObjects.num - 1;
        }
        else
        {
            if (nIndex < m_aNearestObjects.num - 1)
                nIndex++;
            else
                nIndex = 0;
        }
    }
    else if (bReverse)
    {
        nIndex = m_aNearestObjects.num - 1;
    }
    else
    {
        nIndex = 0;
    }

    int nCount = m_aNearestObjects.num;
    int bFound = FALSE;
    while (!bFound && m_aNearestObjects.num != 0 && nCount != 0)
    {
        pObject = NULL;
        CGameObject* pGameObject =
            GetGameObject(m_aNearestObjects[nIndex].m_oidObject);
        int bKeep = FALSE;
        if (pGameObject != NULL &&
            (pObject = pGameObject->AsSWCObject()) != NULL)
        {
            bKeep = TRUE;
            if (nFilter == 0 || (nFilter == 1 && !pObject->IsHostile()) ||
                (nFilter == 2 && pObject->AsSWCCreature() != NULL &&
                 pObject->IsHostile()))
            {
                if (m_aNearestObjects[nIndex].m_nCanSee ==
                    CNearestStruct::CAN_SEE_UNKNOWN)
                {
                    m_aNearestObjects[nIndex].m_nCanSee =
                        m_pSWParty->GetPlayerCharacter()->CanSee(pObject);
                }

                if (m_aNearestObjects[nIndex].m_nCanSee)
                    bFound = TRUE;
                else
                    bKeep = FALSE;
            }
        }

        if (!bKeep)
        {
            m_aNearestObjects.DelIndex(nIndex);
            if (bReverse)
            {
                nIndex--;
                if (nIndex < 0 || nIndex >= m_aNearestObjects.num)
                    nIndex = m_aNearestObjects.num - 1;
            }
            else if (nIndex >= m_aNearestObjects.num)
            {
                nIndex = 0;
            }
        }
        else if (bReverse)
        {
            nIndex--;
            if (nIndex < 0 || nIndex >= m_aNearestObjects.num)
                nIndex = m_aNearestObjects.num - 1;
        }
        else
        {
            nIndex++;
            if (nIndex >= m_aNearestObjects.num)
                nIndex = 0;
        }

        nCount--;
    }

    if (bFound)
    {
        m_nLastTarget = pObject->m_nId;
        ShowObject(pObject, 1, 1, 1);
    }
    else if (m_aNearestObjects.num == 0)
    {
        m_nLastTarget = OBJECT_INVALID;
        ShowObject(NULL, 1, 0, 1);
    }

    return pObject;
}
// PC 0x00647090 GOG 0x00646F40
void CClientExoAppInternal::SetLoadBarProgress(int nProgress, int bUpdateHint)
{
    g_pExoInput->UpdateJoysticks();

    if (m_pGuiLoadScreen != NULL)
    {
        if (bUpdateHint && nProgress < CLIENT_LOAD_HINT_PROGRESS_LIMIT)
            UpdateLoadingScreenHint();

        CSWGuiLoadScreen* pLoadScreen = m_pGuiLoadScreen;
        pLoadScreen->m_pbProgress.SetCurValue(nProgress);

        CExoString sProgress;
        sProgress.Format("Load Bar = %d", nProgress);
#ifdef _XBOX
        m_pMoviePlayer->HideFadePanel(1);
#endif
    }
}
// PC 0x00647130 GOG 0x00646FE0
int CClientExoAppInternal::PlayRumblePattern(int nPattern)
{
    if (nPattern < 0 || nPattern >= m_aRumblePatterns.num)
    {
        return 0;
    }

    int nInstance = m_aRumbleInstances.num;
    if (m_aRumblePatterns.element[nPattern].m_bLoop)
    {
        for (int i = 0; i < nInstance; i++)
        {
            if (m_aRumbleInstances.element[i].m_nPattern == nPattern)
            {
                return 0;
            }
        }
    }

    m_aRumbleInstances.SetSize(nInstance + 1);
    m_aRumbleInstances.element[nInstance].m_nPattern = nPattern;
    m_aRumbleInstances.element[nInstance].m_fTime = 0.0f;
    m_aRumbleInstances.element[nInstance].m_bFirstUpdate = 1;
    return 1;
}
// PC 0x006471E0 GOG 0x00647090
CExoString CClientExoAppInternal::GetGUIString(unsigned long nStrRef)
{
    int nLow = 0;
    int nCount = m_aGuiStrings.num;
    int nHigh = nCount - 1;
    int nMid = 0;
    int bFound = FALSE;
    CExoString sString;

    while (nLow < nHigh && !bFound)
    {
        nMid = nLow + (nHigh - nLow) / 2;

        if (m_aGuiStrings[nMid].m_nStrRef == nStrRef)
        {
            sString = m_aGuiStrings[nMid].m_sString;
            bFound = TRUE;
        }
        else if (m_aGuiStrings[nMid].m_nStrRef < nStrRef)
        {
            nHigh = nMid;
        }
        else
        {
            nLow = nMid + 1;
        }
    }

    if (!bFound)
    {
        CGuiStringStruct stGuiString;

        sString = g_pTlkTable->GetSimpleString(nStrRef);
        stGuiString.m_nStrRef = nStrRef;
        stGuiString.m_sString = sString;

        if (m_aGuiStrings.num > 0 &&
            m_aGuiStrings[nMid].m_nStrRef < nStrRef)
        {
            nMid++;
        }

        m_aGuiStrings.Insert(stGuiString, nMid);
    }

    g_pTlkTable->ParseStr(sString);

    return sString;
}
// PC 0x00647380 GOG 0x00647230
int CClientExoAppInternal::CrownFutureLeader()
{
    if (m_oidFutureLeader != OBJECT_INVALID)
    {
        int nIndex = m_pSWParty->GetIndex(m_oidFutureLeader);
        CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
        CSWSCreature* pNewLeader = pServer->GetCreatureByGameObjectID(
            pServer->ClientToServerObjectId(m_oidFutureLeader));
        unsigned long oidOldLeader = m_nPlayerCreatureId;

        if (nIndex > 0)
        {
            m_pSWParty->SetLeader(nIndex);
        }
        else if (nIndex == 0)
        {
            CSWSPlayer* pPlayer = (CSWSPlayer*)pServer->GetPlayerList()->GetHead();
            SetPlayerCreature(m_oidFutureLeader);
            pPlayer->SetGameObject((CSWSObject*)pNewLeader);
        }

        if (oidOldLeader != m_oidFutureLeader)
        {
            CSWSCreature* pOldLeader = pServer->GetCreatureByGameObjectID(
                pServer->ClientToServerObjectId(oidOldLeader));
            CExoArrayList<CSWVisibilityNode*> lstVisibility;
            CExoArrayList<CSWVisibilityNode*>& lstOldVisibility = pOldLeader->m_lstVisibilityList;
            lstVisibility.SetSize(lstOldVisibility.num);
            for (int i = 0; i < lstOldVisibility.num; i++)
            {
                lstVisibility[i] = lstOldVisibility[i];
            }

            lstOldVisibility.num = 0;
            lstOldVisibility.SetSize(pNewLeader->m_lstVisibilityList.num);
            for (int j = 0; j < pNewLeader->m_lstVisibilityList.num; j++)
            {
                lstOldVisibility[j] = pNewLeader->m_lstVisibilityList[j];
            }

            pNewLeader->m_lstVisibilityList.num = 0;
            pNewLeader->m_lstVisibilityList.SetSize(lstVisibility.num);
            for (int k = 0; k < lstVisibility.num; k++)
            {
                pNewLeader->m_lstVisibilityList[k] = lstVisibility[k];
            }
        }

        m_pPlayerControl->SetPlayer(m_oidFutureLeader);
        m_oidFutureLeader = OBJECT_INVALID;
        return 1;
    }
    return 0;
}
// PC 0x00647600 GOG 0x006474B0
void CClientExoAppInternal::LookUpAndPerformRumbleWithCutOff(
    int nEntry, unsigned char nTable, const Vector& vPosition)
{
    C2DA* p2DA;
    int nRow;
    int bUseRowLabel;
    CExoString sRowLabel;

    switch (nTable)
    {
    case CClientExoApp::RUMBLE_TABLE_FOOTSTEPSOUNDS:
#ifdef _XBOX
        p2DA = g_pRules->Get2DArrays()->GetFootstepSounds();
#else
        p2DA = g_pRules->Get2DArrays()->m_pFootstepSounds;
#endif
        bUseRowLabel = FALSE;
        nRow = nEntry;
        break;
    case CClientExoApp::RUMBLE_TABLE_VISUALEFFECTS:
        p2DA = g_pRules->Get2DArrays()->m_pVisualEffect;
        bUseRowLabel = TRUE;
        sRowLabel = CExoString(nEntry);
        break;
    default:
        return;
    }

    if (p2DA == NULL)
    {
        return;
    }

    int nRumblePattern;
    if (bUseRowLabel)
    {
        if (!p2DA->GetINTEntry(sRowLabel, "RumblePattern",
                               &nRumblePattern))
        {
            return;
        }
    }
    else
    {
        if (!p2DA->GetINTEntry(nRow, "RumblePattern",
                               &nRumblePattern))
        {
            return;
        }
    }

    CSWCCreature* pCreature =
        g_pAppManager->GetClientExoApp()->GetPlayerCreature();
    if (pCreature == NULL)
    {
        return;
    }

    float fRumbleCutOff;
    if (bUseRowLabel)
    {
        if (!p2DA->GetFLOATEntry(sRowLabel, "RumbleCutOff",
                                 &fRumbleCutOff))
        {
            fRumbleCutOff = CLIENT_RUMBLE_DEFAULT_CUTOFF;
        }
    }
    else
    {
        if (!p2DA->GetFLOATEntry(nRow, "RumbleCutOff",
                                 &fRumbleCutOff))
        {
            fRumbleCutOff = CLIENT_RUMBLE_DEFAULT_CUTOFF;
        }
    }

    Vector vListener;
    if (g_pAppManager->GetClientExoApp()->GetInGameGui()->m_nGlobalDialogState)
    {
        g_pExoSound->GetListenerPosition(&vListener);
    }
    else
    {
        vListener = pCreature->m_vPosition;
    }

    Vector vDelta;
    vDelta = vListener - vPosition;
    if (vDelta * vDelta > fRumbleCutOff * fRumbleCutOff)
    {
        return;
    }

    PlayRumblePattern(nRumblePattern);
}

#ifndef _XBOX
// PC 0x00647890 GOG 0x00647740
int CClientExoAppInternal::AddMovieToExoArrayList(
    const CExoString& sMovie, CExoArrayList<CExoString>& lstMovies)
{
    if (sMovie != "")
    {
        int nAlias = sMovie.Find(":", 0);
        if (nAlias != -1)
        {
            lstMovies.AddUnique(sMovie);
            return 1;
        }

        CExoString sFileName;
        CExoString sAlias;
        sFileName.Format("MOVIES:%s", sMovie.CStr());
        int bFound = 0;
        CExoFile* pFile = new CExoFile(sFileName, RESTYPE_BIK, "rb");

        if (pFile->FileOpened())
        {
            bFound = 1;
        }
        else
        {
            for (int nPackage = 1; nPackage < CTlkTable::NUM_CONTENT_PKGS; ++nPackage)
            {
                if (pFile != NULL)
                {
                    delete pFile;
                    pFile = NULL;
                }

                sAlias.Format("LIVE%d", nPackage);
                if (g_pExoBase->m_pExoAliasList->GetAliasPath(sAlias, 0).GetLength())
                {
                    sFileName.Format("LIVE%d:movies\\%s", nPackage, sMovie.CStr());
                    pFile = new CExoFile(sFileName, RESTYPE_BIK, "rb");
                    if (pFile->FileOpened())
                    {
                        bFound = 1;
                        break;
                    }
                }
            }
        }

        if (pFile != NULL)
        {
            delete pFile;
        }

        if (bFound)
        {
            sFileName = g_pExoBase->m_pExoAliasList->ResolveFileName(sFileName, RESTYPE_BIK);
            lstMovies.AddUnique(sFileName);
            m_pClientOptions->SetMovieShown(sFileName, 1);
            return 1;
        }
    }
    return 0;
}
// PC 0x00647C00 GOG 0x00647AB0
int CClientExoAppInternal::AddMovieToMovieQueue(const CExoString& sMovie,
                                                int nFlags)
{
    m_aMovieQueueFlags.Add(nFlags);

    return AddMovieToExoArrayList(sMovie, m_aMovieQueue);
}
#endif
// PC 0x00647C90 GOG 0x00647B40
template <> inline CExoArrayList<CSWGuiInterfaceAction>::~CExoArrayList()
{
    delete[] element;
    element = NULL;
}
// PC 0x00647CC0 GOG 0x00647B70
CClientExoAppInternal::CClientExoAppInternal()
{
    m_bStartMenuMusicOnMovieEnd = 1;
    m_nUnknown1D4 = 1;
    m_nUnknown26C = 0;
    m_bClientInitialized = 0;
    m_nDebugMode = 0;
    m_nUnknown19C = 0;
    m_nPlayerCreatureId = OBJECT_INVALID;
    m_oidReattachedCreature = OBJECT_INVALID;
    m_nUnknown44 = 0;
    m_pMainMenu = NULL;
    m_pStartupPanel = NULL;
    m_nUnknown48 = 0;
    m_nUnknown4C = 0;
    m_nPlayModulePanel = 0;
    m_pModule = NULL;
    m_nUnknown54 = 0;
    m_nUnknown68 = 0;
    m_pSWCMessage = NULL;
    m_pQuestJournal = NULL;
    m_pWorldJournal = NULL;
    m_nUnknown6C = 0;
    m_nUnknown188 = 0;
    m_nLoadBarState = 0;
    m_pSWGuiManager = NULL;
    m_pGuiLoadScreen = NULL;
    m_pLegalScreen = NULL;
    m_pServerInfo = NULL;
    m_nInputClass = 0;
    m_bPlayBackInputEnd = 0;
    m_bInputDisabled = 0;
    m_nCombatMode = 0;
    m_bEnemySighted = 0;
#ifndef _XBOX
    m_bMineSighted = 0;
#endif
    m_nInputHistoryCount = 0;
    m_nHitCheckMaterialMask = 0;
    memset(m_anKeyboardModal, 0, sizeof(m_anKeyboardModal));
    m_nUnknownEC = 0;
    m_nUnknownF0 = 0;
    m_nUnknownE4 = 0;
    m_nUnknownE8 = 0;
    m_nUnknownF4 = 0;
    m_nUnknownF8 = 0;
#ifdef _XBOX
    m_fXboxUnmapped120 = 0.0f;
    m_fXboxUnmapped124 = 0.0f;
    m_fXboxUnmapped128 = 0.0f;
    m_fXboxUnmapped12C = 0.0f;
#else
    m_Unknown11C.Clear();
#endif
    m_nCameraScrollX = 0;
    m_nCameraScrollY = 0;
    m_bClientImeUI = 0;
    m_pImeUI = NULL;
    m_bScreenShot = 0;
    m_pCaptureEventFile = NULL;
    m_nCaptureEvents = 0;
    m_nCaptureEventPlayBack = 0;
    m_nCaptureEventPlayBackRate = CAPTURE_PLAYBACK_DEFAULT_RATE;
    m_pGuiInGame = NULL;
    m_oidUnknown58 = CGameObjectArray::INVALIDOBJID;
    m_oidUnknown170 = CGameObjectArray::INVALIDOBJID;
    m_nUnknown174 = 0;
    m_nUnknown178 = 0;
    m_nUnknown17C = 0;
    m_fPlayerAnimationTime = 0.0f;
    m_nVideoMode = VIDEO_MODE_UNSET;
    m_nTexturePack = TEXTURE_PACK_UNSET;
    m_pCharacterFile = NULL;
    m_nCharacterFileSize = 0;
    m_nCharacterFileId = 0;
    m_pSoundSetList = NULL;
    m_pItemDropSounds = NULL;
    m_PauseState.m_nPauseState = 0;
    m_pPauseExclusionList = new CExoLinkedList<unsigned long>;
    m_pWorldTimer = NULL;
    m_pCombatTimer = NULL;
    m_pPauseTimer = NULL;
    m_pAnimationTimer = NULL;
    m_pMoviePlayer = NULL;
    m_bEnableMouseAtEndOfLoadModuleMovie = 0;
    m_nUnknown60 = 0;
    m_bApplicationIdsMatch = 0;
    m_bInStartNewModule = 0;
    m_nUnknown1F0 = 0;
    m_fCameraZoom = 0.0f;
    m_fCameraTilt = 0.0f;
    m_fCameraTurn = 0.0f;
    m_oidUnknown224 = CGameObjectArray::INVALIDOBJID;
    m_pMenuMusic = NULL;
    m_pLoadMusic = NULL;
    m_nUnknown244 = 0;
    m_nUnknown240 = 0;
    m_bModuleServicesActive = 0;
    m_nUnknown24C = 0;
    m_bExitGame = 0;
    m_bExitGamePending = 0;
    m_nUnknown258 = 0;
    m_nUnknown25C = 0;
#ifdef _XBOX
    m_nXboxUnmapped264 = 0;
#endif
    m_bEndGamePending = 0;
    m_nEndGamePendingUnknown = 1;
    m_nCharacterExportState = CHARACTER_EXPORT_IDLE;
    m_bShutDownToMainMenuPending = 0;
    m_nUnknown268 = 0;
    m_pMiniGameObjectArray = new CSWMiniGameObjectArray;
    m_pSWParty = new CSWParty;
    m_nAreaNotReadyToDisplay = 1;
    m_fDriveActionDelay = -1.0f;
    m_pPlayerControl = NULL;
    m_nUnknown29C = 1;
    m_nGameOver = 0;
    m_bGameOverPending = 1;
    m_nDeathSequenceLastTime = (unsigned long)(g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000);
    m_fDeathSequenceElapsed = 0.0f;
    m_fDeathSequenceScale = 0.0f;
    m_oidLastPartyMemberTempKilled = OBJECT_INVALID;
    m_nLastTarget = OBJECT_INVALID;
#ifndef _XBOX
    m_oidLastClickedOnTarget = OBJECT_INVALID;
#endif
    m_bCanSendDialog = 0;
    m_nCanSendDialogTime = 0;
    m_bDialogSent = 0;
    m_bDialogModelsSent = 0;
    m_sLoadScreenImage = "";
    m_bShowLoadScreen = 0;
    m_nLoadFromIFO = 0;
    m_bRunScript = 0;
    m_nUnknown1B0 = 0;
#ifdef _XBOX
    m_bMinGlobalRim = 0;
#endif
    m_nLoadingSaveGame = 0;
    m_oidFutureLeader = OBJECT_INVALID;
    m_oidUnknown364 = OBJECT_INVALID;
    m_fTargetInvisibleTime = 0;
    m_fMovingTargetTime = 0.0f;
    m_fStatusSummaryDelay = 0.0f;
    m_fEndGamePendingDelay = 0.0f;
    m_PauseFlags.m_Bits.m_bTogglePauseAtEndOfFrame = 0;
#ifndef _XBOX
    m_bTogglePause = 0;
#endif
#ifdef _XBOX
    m_PauseFlags.m_Bits.m_bAutoPaused = 0;
#else
    m_AutoPauseFlags.m_Bits.m_bAutoPaused = 0;
#endif
    m_fEnemyAutoPauseDelay = 0.0f;
    m_fEnemyAutoPauseTimer = 0.0f;
    m_fCombatAutoPauseTimer = 0.0f;
#ifndef _XBOX
    m_fMineSightedTime = 0;
    m_nDelayedAutoPauseType = AUTO_PAUSE_TYPE_UNSET;
#endif
    m_fEnemyAutoPauseCooldown = 0.0f;
    m_nUnknown3A4 = 0;
    m_nVideoEffect = CLIENT_VIDEO_EFFECT_DISABLED;
    m_nTutorialX = OBJECT_INVALID;
    m_nTutorialY = OBJECT_INVALID;
    m_nTutorialZ = 0;
    m_nTutorialReason = TUTORIAL_REASON_UNSET;
    m_oidCachedCreature = OBJECT_INVALID;
    m_pCachedCreature = NULL;
#ifndef _XBOX
    m_nUnknown3D4 = 0;
    m_nUnknown3D0 = 0;
    m_nMouseY = 0;
    m_nMouseX = 0;
    m_nMouseHidden = 0;
    m_fMouseHideTime = -1.0f;
#endif
    m_nAttackMashTutorialTimer = 0;
    memset(m_aLoadSteps, 0, 5);
    memset(m_aLiveContentNames, 0, sizeof(m_aLiveContentNames));
    m_nLoadScreenHintUpdateTimer = 0;
    m_nLoadScreenStartTime = 0;
    m_nGamePlayHint = 0;
    memset(m_anStoryHint, 0, sizeof(m_anStoryHint));
    m_bMainMenuTransition = 0;
    m_PauseFlags.m_Bits.m_bLastTargetChanged = 0;
    m_PauseFlags.m_Bits.m_nUnused1 = 0;
    m_fTargetChanging = 0.0f;
    m_pClientOptions = new CClientOptions;
    m_nModuleResourcesLoaded = 0;
    m_nUnknown49C = 0;
    m_nUnknown4A0 = 0;
    m_pEvilEndingHackSound = NULL;
#ifndef _XBOX
    m_oidMouseOverTarget = OBJECT_INVALID;
    m_bCharacterChangeInProgress = 0;
    m_bCreditSequenceInProgress = 0;
    m_bEvilEndingHackInProgress = 0;
    m_nSuperModelSet = 1;
    m_nSoundPausedByCombat = 0;
#endif
}
// PC 0x006483D0 GOG 0x00648280
CClientExoAppInternal::~CClientExoAppInternal()
{
    if (m_pSWParty != NULL)
    {
        delete m_pSWParty;
        m_pSWParty = NULL;
    }
    if (m_pPauseExclusionList != NULL)
    {
        delete m_pPauseExclusionList;
    }
    if (m_pMiniGameObjectArray != NULL)
    {
        delete m_pMiniGameObjectArray;
        m_pMiniGameObjectArray = NULL;
    }
    if (m_pEvilEndingHackSound != NULL)
    {
        delete m_pEvilEndingHackSound;
        m_pEvilEndingHackSound = NULL;
    }
}
#ifndef _XBOX
void CClientExoAppInternal::DisplayMainMenu(int bUnknown)  // PC 0x00648720 GOG 0x006485D0
{
    if (m_pMainMenu != NULL && m_pSWGuiManager->PanelExists(m_pMainMenu))
    {
        return;
    }

    if (m_pStartupPanel != NULL)
    {
        if (m_pSWGuiManager->PanelExists(m_pStartupPanel))
        {
            CSWGuiPanel* pPanel = m_pStartupPanel;
            pPanel->m_bDeferredRemoval = 0;
            pPanel->m_nExitState = CSWGuiPanel::EXITSTATE_DELETE;
        }
        else
        {
            delete m_pStartupPanel;
        }
        m_pStartupPanel = NULL;
    }

    if (m_pMainMenu != NULL)
    {
        m_pMainMenu->SetModal(0);
        CSWGuiPanel* pPanel = m_pMainMenu;
        pPanel->m_bDeferredRemoval = 0;
        pPanel->m_nExitState = CSWGuiPanel::EXITSTATE_DELETE;
        m_pMainMenu = NULL;
    }

    m_pMainMenu = new CSWGuiMainMenu(m_pSWGuiManager);
    SetInputClass(INPUT_CLASS_GUI, 1);
    m_pSWGuiManager->AddPanel(m_pMainMenu,
                             CSWGuiManager::PANEL_MODE_FOREGROUND, 1);
    StartMenuMusic(1, MENU_MUSIC_VOLUME, MENU_MUSIC_PRIORITY_GROUP);
    if (m_bMainMenuTransition == 1)
    {
        m_bMainMenuTransition = 0;
    }
    m_pGuiInGame->SetGlobalUnpauseableState(0);
}
#endif
#ifdef _XBOX

void CClientExoAppInternal::DisplayStartScreen()
{
    int bSkipped = 0;

    if (m_pStartupPanel == NULL)
    {
        m_pStartupPanel = new CSWGuiStartScreen(m_pSWGuiManager);
    }

    if (m_pLegalScreen != NULL)
    {
        m_pLegalScreen->SetImage("");
        m_pSWGuiManager->RemovePanel(m_pLegalScreen);
        if (m_pLegalScreen != NULL)
        {
            delete m_pLegalScreen;
        }
        m_pLegalScreen = NULL;
        g_pExoResMan->RemoveResourceImageFile("RIMSXBOX:LEGAL");
        g_pExoResMan->RemoveResourceImageFile("RIMSXBOX:LEGALDX");
    }

    PlayMovie("sizzle2", bSkipped, 1, 0);
    if (!bSkipped)
    {
        m_pSWGuiManager->AddPanel(m_pStartupPanel,
                                  CSWGuiManager::PANEL_MODE_FOREGROUND, 1);
        SetInputClass(INPUT_CLASS_GUI, 1);
        StartMenuMusic(1, MENU_MUSIC_VOLUME, MENU_MUSIC_PRIORITY_GROUP);
    }
    else
    {
        DisplayMainMenu(1);
    }
}
#endif
#include "nwcjournalinl.h"
// PC 0x0064ECB0 GOG 0x0064EB60
void CClientExoAppInternal::StallServerCode()
{
    CSWReentrantServerStats* pStats = g_pAppManager->m_pReentrantServerStats;

    if (pStats->m_bOperationStartPending == 1)
    {
        pStats->m_bOperationStartPending = 0;
    }
    else if (pStats->m_bOperationEndPending == 1)
    {
        pStats->m_bOperationEndPending = 0;
        pStats = g_pAppManager->m_pReentrantServerStats;
        pStats->m_bOperationActive = 0;
        pStats->m_nOperation = 0;

        if (m_bShutDownToMainMenuPending != 0)
        {
            ShutDownToMainMenu();
        }

        m_bShutDownToMainMenuPending = 0;
    }
}
// PC 0x00648910 GOG 0x006487C0
void CClientExoAppInternal::ShutDownToMainMenu()
{
    CGameObject* pObject = NULL;

    if (m_bExitGamePending != 0)
    {
        m_bExitGame = 1;
    }

    g_pAppManager->DestroyServer();

    if (m_pModule != NULL)
    {
        CSWCArea* pArea = m_pModule->m_pInfo;
        if (pArea != NULL)
        {
            m_pGameObjectArray->Delete(pArea->GetId(), &pObject);
            pArea->UnloadArea();
        }

        UnloadModule();
    }

    if (m_bExitGamePending == 0)
    {
        DisplayMainMenu(0);
        g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_GENERIC);
        m_bStartMenuMusicOnMovieEnd = 1;
        StartMenuMusic(1, MENU_MUSIC_VOLUME, MENU_MUSIC_PRIORITY_GROUP);

        if (m_nLoadBarState != 0)
        {
            g_pAppManager->m_pReentrantServerStats->m_bOperationEndPending = 0;
            CSWReentrantServerStats* pStats =
                g_pAppManager->m_pReentrantServerStats;
            pStats->m_bOperationActive = 0;
            pStats->m_nOperation = 0;
            m_nLoadBarState = 0;
        }
    }
}
// PC 0x006489E0 GOG 0x00648890
void CClientExoAppInternal::AddObjectModelDirectlyToIgnoreList(
    unsigned long oidObject, List<CAurObject*>* pIgnoreList)
{
    CGameObject* pObject = GetGameObject(oidObject);

    if (pObject != NULL)
    {
        switch (pObject->m_nObjectType)
        {
        case SW_OBJECT_TYPE_CREATURE:
            AddCreatureModelDirectlyToIgnoreList(oidObject, pIgnoreList);
            break;
        case SW_OBJECT_TYPE_ITEM:
            AddItemModelDirectlyToIgnoreList(oidObject, pIgnoreList);
            break;
        default:
            AddSimpleObjectModelDirectlyToIgnoreList(oidObject, pIgnoreList);
            break;
        }
    }
}
// PC 0x00648A50 GOG 0x00648900
void CClientExoAppInternal::AddFeedbackMessage(unsigned short nType,
                                               CSWCCMessageData* pMessageData,
                                               const CExoString& sMessage)
{
    unsigned long oidObject;
    CExoString sObjectName;
    int bCombatMessage = 0;
    CExoString sFeedback("");
    CGuiInGame* pGuiInGame = m_pGuiInGame;
    CSWCCreature* pCreature = GetPlayerCreature();
    switch (nType)
    {
        case CSWSCreature::FEEDBACK_CANNOT_USE_SKILL:
        {
            sFeedback = GetGUIString(0x59Bu);
            break;
        }
        case CSWSCreature::FEEDBACK_SKILL_COOLDOWN:
        {
            int nValue = pMessageData->GetInteger(0);
            g_pTlkTable->SetCustomToken(0, CExoString(nValue));
            sFeedback = g_pSWTlkTable->Fetch(0x59Cu, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_002:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_003:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_223:
        {
            sFeedback = GetGUIString(0);
            bCombatMessage = 1;
            break;
        }
        case CSWSCreature::FEEDBACK_OBJECT_ITEM_TEXT:
        {
            oidObject = pMessageData->GetObjectID(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            g_pTlkTable->SetCustomToken(1, pMessageData->GetString(0));
            sFeedback = g_pSWTlkTable->Fetch(0, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_OBJECT_ITEM_NOTICE:
        {
            oidObject = pMessageData->GetObjectID(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            g_pTlkTable->SetCustomToken(1, pMessageData->GetString(0));
            sFeedback = g_pSWTlkTable->Fetch(0, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_OBJECT_COUNT:
        {
            oidObject = pMessageData->GetObjectID(0);
            int nValue2 = pMessageData->GetInteger(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(1, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(1, sObjectName);
            }
            g_pTlkTable->SetCustomToken(0, CExoString(nValue2));
            sFeedback = g_pSWTlkTable->Fetch(0, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_046:
        {
            oidObject = pMessageData->GetObjectID(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            sFeedback = g_pSWTlkTable->Fetch(0, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_150:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_048:
        {
            oidObject = pMessageData->GetObjectID(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            sFeedback = g_pSWTlkTable->Fetch(0, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_057:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_HEAL_NO_AFFLICTION:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_HEAL_ALREADY_FULL:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_CANNOT_STEALTH_IN_COMBAT:
        {
            sFeedback = GetGUIString(0x5ACu);
            break;
        }
        case CSWSCreature::FEEDBACK_TARGET_MUST_BE_AWARE:
        {
            sFeedback = GetGUIString(0x594u);
            break;
        }
        case CSWSCreature::FEEDBACK_CANNOT_MOVE:
        {
            sFeedback = GetGUIString(0x595u);
            break;
        }
        case CSWSCreature::FEEDBACK_CANNOT_ATTACK_FRIEND:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_ATTACK_LINE_OF_SIGHT_BLOCKED:
        {
            sFeedback = GetGUIString(0x5ECu);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_008:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_009:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_LEVEL_UP:
        {
            sFeedback = GetGUIString(0x59Eu);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_012:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_CONTAINER_FULL:
        {
            sFeedback = GetGUIString(0x5EBu);
            break;
        }
        case FEEDBACK_TRAP_TRIGGERED:
        {
            sFeedback = GetGUIString(0x5B5u);
            break;
        }
        case CSWSCreature::FEEDBACK_HEALED:
        {
            oidObject = pMessageData->GetObjectID(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            int nValue2 = pMessageData->GetInteger(0);
            g_pTlkTable->SetCustomToken(1, nValue2);
            sFeedback = g_pSWTlkTable->Fetch(0x5D7u, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_FORCE_POINTS_REGAINED:
        {
            bCombatMessage = 1;
            oidObject = pMessageData->GetObjectID(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            int nValue2 = pMessageData->GetInteger(0);
            g_pTlkTable->SetCustomToken(1, nValue2);
            sFeedback = g_pSWTlkTable->Fetch(0x407u, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_EXPERIENCE_POINTS_GAINED:
        {
            int nAmount = pMessageData->GetInteger(0);
            sFeedback.Format("%s %i", GetGUIString(1536).CStr(), nAmount);
            break;
        }
        case CSWSCreature::FEEDBACK_EXPERIENCE_POINTS_LOST:
        {
            int nAmount = pMessageData->GetInteger(0);
            sFeedback.Format("%s %i", GetGUIString(1537).CStr(), nAmount);
            break;
        }
        case FEEDBACK_MESSAGE_JOURNAL_UPDATED:
        {
            CExoString sItem = pMessageData->GetString(0);
            sFeedback.Format("%s %s", GetGUIString(CSWCJournal::STRREF_CSWCJOURNAL_QUESTENTRYADDED).CStr(), sItem.CStr());
            break;
        }
        case CSWSCreature::FEEDBACK_COMBAT_MODE_WEAPON_INVALID:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_OBJECT_LOCKED:
        {
            oidObject = pMessageData->GetObjectID(0);
            CExoString sSound;
            CSWCDoor* pDoor = g_pAppManager->GetClientExoApp()->GetDoorByGameObjectID(oidObject);
            CSWCPlaceable* pPlaceable = g_pAppManager->GetClientExoApp()->GetPlaceableByGameObjectID(oidObject);
            int nSoundAppearance;
            if (pDoor != NULL)
            {
                int nAppearance = pDoor->m_nAppearanceType;
                if (nAppearance == 0)
                {
                    int nGenericType = pDoor->m_nGenericType;
                    g_pRules->Get2DArrays()->m_pGenericDoors->GetINTEntry(nGenericType,
                        CTwoDimArrays::COL2DA_GENERICDOORS_SOUNDAPPTYPE, &nSoundAppearance);
                }
                else
                    g_pRules->Get2DArrays()->m_pDoorTypes->GetINTEntry(nAppearance,
                        CTwoDimArrays::COL2DA_DOORTYPES_SOUNDAPPTYPE, &nSoundAppearance);
                C2DA* pSoundTable = g_pRules->Get2DArrays()->m_pPlaceableSounds;
                pSoundTable->GetCExoStringEntry(nSoundAppearance,
                    CTwoDimArrays::COL2DA_PLACEABLESOUNDS_LOCKED, &sSound);
                if (sSound.GetLength() > 0)
                    g_pExoSound->Play3DOneShotSound(sSound, pDoor->m_vPosition,
                        1.5f, CExoSound::PRIORITY_GROUP_DOOR, 0, 0, 0.0f, 0.0f);
                pDoor->SetFeedbackInfo(GetGUIString(1439));
            }
            else if (pPlaceable != NULL)
            {
                int nAppearance = pPlaceable->m_nAppearance;
                g_pRules->Get2DArrays()->m_pPlaceables->GetINTEntry(nAppearance,
                    CTwoDimArrays::COL2DA_PLACEABLES_SOUNDAPPTYPE, &nSoundAppearance);
                C2DA* pSoundTable = g_pRules->Get2DArrays()->m_pPlaceableSounds;
                pSoundTable->GetCExoStringEntry(nSoundAppearance,
                    CTwoDimArrays::COL2DA_PLACEABLESOUNDS_LOCKED, &sSound);
                if (sSound.GetLength() > 0)
                    g_pExoSound->Play3DOneShotSound(sSound, pPlaceable->m_vPosition,
                        0.0f, CExoSound::PRIORITY_GROUP_DOOR, 0, 0, 0.0f, 0.0f);
                pPlaceable->SetFeedbackInfo(GetGUIString(1439));
            }
        }
        sFeedback = GetGUIString(1437);
        break;
        case CSWSCreature::FEEDBACK_OBJECT_ALREADY_UNLOCKED:
        {
            sFeedback = GetGUIString(0x596u);
            break;
        }
        case CSWSCreature::FEEDBACK_OBJECT_KEY_REQUIRED:
        {
            sFeedback = GetGUIString(0x597u);
            break;
        }
        case CSWSCreature::FEEDBACK_KEY_USED:
        {
            sFeedback = GetGUIString(0x598u);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_017:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_018:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_019:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_020:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_054:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_186:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_213:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_TARGET_NOT_VISIBLE:
        {
            sFeedback = GetGUIString(0x599u);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_052:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_118:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_022:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_061:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_065:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_147:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_072:
        {
            if (pCreature != NULL)
            {
                if (GetObjectName(pCreature->GetId(), sObjectName))
                    g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            else
            {
                sObjectName = sFeedback = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            sFeedback = g_pSWTlkTable->Fetch(0, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_210:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_211:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_ITEM_NO_USES:
        {
            sFeedback = GetGUIString(0x59Au);
            break;
        }
        case CSWSCreature::FEEDBACK_DIALOG_TARGET_TOO_FAR:
        {
            sFeedback = GetGUIString(0x5ABu);
            break;
        }
        case CSWSCreature::FEEDBACK_DIALOG_TARGET_BUSY:
        {
            sFeedback = GetGUIString(0x593u);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_152:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_074:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_075:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_COUNTER_PAIR:
        {
            int nValue = pMessageData->GetInteger(0);
            int nValue2 = pMessageData->GetInteger(1);
            g_pTlkTable->SetCustomToken(0, nValue2);
            g_pTlkTable->SetCustomToken(1, nValue);
            sFeedback = g_pSWTlkTable->Fetch(0, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_076:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_077:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_ITEM_LEVEL_RESTRICTION:
        {
            sFeedback = GetGUIString(0x5BEu);
            break;
        }
        case CSWSCreature::FEEDBACK_ITEM_PROFICIENCY_REQUIRED:
        {
            sFeedback = GetGUIString(0x5C3u);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_120:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_ITEM_ALIGNMENT_RESTRICTION:
        {
            sFeedback = GetGUIString(0x5E8u);
            break;
        }
        case CSWSCreature::FEEDBACK_ITEM_CLASS_RESTRICTION:
        {
            sFeedback = GetGUIString(0x5E9u);
            break;
        }
        case CSWSCreature::FEEDBACK_ITEM_RACE_RESTRICTION:
        {
            sFeedback = GetGUIString(0x5EAu);
            break;
        }
        case CSWSCreature::FEEDBACK_ITEM_FEAT_RESTRICTION:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_ITEM_GENDER_RESTRICTION:
        {
            sFeedback = GetGUIString(47993);
            break;
        }
        case CSWSCreature::FEEDBACK_ITEM_ATTRIBUTE_RESTRICTION:
        {
            sFeedback = GetGUIString(117590);
            break;
        }
        case CSWSCreature::FEEDBACK_ITEM_CNPC_RESTRICTION:
        {
            sFeedback = GetGUIString(130591);
            break;
        }
        case CSWSCreature::FEEDBACK_WEAPON_EQUIPPED_ONE_HANDED:
        {
            sFeedback = GetGUIString(0x5C5u);
            break;
        }
        case CSWSCreature::FEEDBACK_WEAPON_EQUIPPED_TWO_HANDED:
        {
            sFeedback = GetGUIString(0x5C4u);
            break;
        }
        case CSWSCreature::FEEDBACK_EQUIPPED_ITEM_SWAPPED:
        {
            sFeedback = GetGUIString(0x5C6u);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_124:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_AC_NATURAL_BONUS_ACTIVE:
        {
            sFeedback = GetGUIString(0x5DEu);
            break;
        }
        case CSWSCreature::FEEDBACK_AC_ARMOUR_BONUS_ACTIVE:
        {
            sFeedback = GetGUIString(0x5DFu);
            break;
        }
        case CSWSCreature::FEEDBACK_AC_SHIELD_BONUS_ACTIVE:
        {
            sFeedback = GetGUIString(0x5E0u);
            break;
        }
        case CSWSCreature::FEEDBACK_AC_DEFLECTION_BONUS_ACTIVE:
        {
            sFeedback = GetGUIString(0x5E1u);
            break;
        }
        case CSWSCreature::FEEDBACK_CANNOT_EQUIP_ARMOUR_IN_COMBAT:
        {
            sFeedback = GetGUIString(0x5E2u);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_200:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_CANNOT_DROP_ARMOUR_IN_COMBAT:
        {
            sFeedback = GetGUIString(0x5E3u);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_100:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_099:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_101:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_102:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_103:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_DAMAGE_IMMUNITY:
        {
            CExoString sDamage;
            bCombatMessage = 1;
            oidObject = pMessageData->GetObjectID(0);
            int nDamage = pMessageData->GetInteger(0);
            unsigned short nDamageType = (unsigned short)pMessageData->GetInteger(1);
            if ((nDamageType & DAMAGE_TYPE_BLUDGEONING) ||
                (nDamageType & DAMAGE_TYPE_PIERCING) ||
                (nDamageType & DAMAGE_TYPE_SLASHING))
            {
                g_pTlkTable->SetCustomToken(0, "");
                sDamage = g_pSWTlkTable->Fetch(1423, NULL, 0, 0);
            }
            else if (nDamageType & DAMAGE_TYPE_UNIVERSAL)
            {
                g_pTlkTable->SetCustomToken(0, "");
                sDamage = g_pSWTlkTable->Fetch(1422, NULL, 0, 0);
            }
            else if (nDamageType & DAMAGE_TYPE_ACID)
            {
                g_pTlkTable->SetCustomToken(0, "");
                sDamage = g_pSWTlkTable->Fetch(1440, NULL, 0, 0);
            }
            else if (nDamageType & DAMAGE_TYPE_COLD)
            {
                g_pTlkTable->SetCustomToken(0, "");
                sDamage = g_pSWTlkTable->Fetch(1441, NULL, 0, 0);
            }
            else if (nDamageType & DAMAGE_TYPE_LIGHT_SIDE)
            {
                g_pTlkTable->SetCustomToken(0, "");
                sDamage = g_pSWTlkTable->Fetch(1442, NULL, 0, 0);
            }
            else if (nDamageType & DAMAGE_TYPE_ELECTRICAL)
            {
                g_pTlkTable->SetCustomToken(0, "");
                sDamage = g_pSWTlkTable->Fetch(1443, NULL, 0, 0);
            }
            else if (nDamageType & DAMAGE_TYPE_FIRE)
            {
                g_pTlkTable->SetCustomToken(0, "");
                sDamage = g_pSWTlkTable->Fetch(1444, NULL, 0, 0);
            }
            else if (nDamageType & DAMAGE_TYPE_DARK_SIDE)
            {
                g_pTlkTable->SetCustomToken(0, "");
                sDamage = g_pSWTlkTable->Fetch(1445, NULL, 0, 0);
            }
            else if (nDamageType & DAMAGE_TYPE_SONIC)
            {
                g_pTlkTable->SetCustomToken(0, "");
                sDamage = g_pSWTlkTable->Fetch(1446, NULL, 0, 0);
            }
            else if (nDamageType & DAMAGE_TYPE_ION)
            {
                g_pTlkTable->SetCustomToken(0, "");
                sDamage = g_pSWTlkTable->Fetch(1447, NULL, 0, 0);
            }
            else if (nDamageType & DAMAGE_TYPE_BLASTER)
            {
                g_pTlkTable->SetCustomToken(0, "");
                sDamage = g_pSWTlkTable->Fetch(1448, NULL, 0, 0);
            }
            else if (nDamageType & DAMAGE_TYPE_POISON)
            {
                g_pTlkTable->SetCustomToken(0, "");
                sDamage = g_pSWTlkTable->Fetch(41902, NULL, 0, 0);
            }
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            g_pTlkTable->SetCustomToken(1, sDamage);
            sFeedback = g_pSWTlkTable->Fetch(1458, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_DAMAGE_RESISTED:
        {
            oidObject = pMessageData->GetObjectID(0);
            int nValue2 = pMessageData->GetInteger(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            g_pTlkTable->SetCustomToken(1, CExoString(nValue2));
            sFeedback = g_pSWTlkTable->Fetch(0x5AEu, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_DAMAGE_RESISTANCE_REMAINING:
        {
            oidObject = pMessageData->GetObjectID(0);
            int nValue2 = pMessageData->GetInteger(0);
            int nValue3 = pMessageData->GetInteger(1);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            g_pTlkTable->SetCustomToken(1, CExoString(nValue2));
            g_pTlkTable->SetCustomToken(2, CExoString(nValue3));
            sFeedback = g_pSWTlkTable->Fetch(0x5B0u, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_DAMAGE_REDUCTION:
        {
            oidObject = pMessageData->GetObjectID(0);
            int nValue2 = pMessageData->GetInteger(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            g_pTlkTable->SetCustomToken(1, CExoString(nValue2));
            sFeedback = g_pSWTlkTable->Fetch(0x5AFu, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_DAMAGE_REDUCTION_REMAINING:
        {
            oidObject = pMessageData->GetObjectID(0);
            int nValue2 = pMessageData->GetInteger(0);
            int nValue3 = pMessageData->GetInteger(1);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            g_pTlkTable->SetCustomToken(1, CExoString(nValue2));
            g_pTlkTable->SetCustomToken(2, CExoString(nValue3));
            sFeedback = g_pSWTlkTable->Fetch(0x5B1u, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_ATTRIBUTE_REDUCED:
        {
            oidObject = pMessageData->GetObjectID(0);
            int nAbility = pMessageData->GetInteger(0);
            int nAmount = pMessageData->GetInteger(1);
            float fDuration = pMessageData->GetFloat(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            unsigned long nStrRef = 0;
            switch (nAbility)
            {
                case CSWCCreatureStats::ABILITY_STRENGTH: nStrRef = 211; break;
                case CSWCCreatureStats::ABILITY_DEXTERITY: nStrRef = 212; break;
                case CSWCCreatureStats::ABILITY_CONSTITUTION: nStrRef = 213; break;
                case CSWCCreatureStats::ABILITY_WISDOM: nStrRef = 214; break;
                case CSWCCreatureStats::ABILITY_INTELLIGENCE: nStrRef = 215; break;
                case CSWCCreatureStats::ABILITY_CHARISMA: nStrRef = 216; break;
            }
            CExoString sAbility = g_pSWTlkTable->Fetch(nStrRef, NULL, 0, 0);
            g_pTlkTable->SetCustomToken(1, sAbility);
            g_pTlkTable->SetCustomToken(2, nAmount);
            g_pTlkTable->SetCustomToken(3, (int)fDuration);
            sFeedback = g_pSWTlkTable->Fetch(42408, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_SPELL_IMMUNITY:
        {
            bCombatMessage = 1;
            oidObject = pMessageData->GetObjectID(0);
            CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(pMessageData->GetInteger(0));
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            g_pTlkTable->SetCustomToken(1, pSpell->GetSpellNameText());
            sFeedback = g_pSWTlkTable->Fetch(1458, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_OBJECT_COUNTER:
        {
            oidObject = pMessageData->GetObjectID(0);
            int nValue2 = pMessageData->GetInteger(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            g_pSWTlkTable->SetCustomToken(1, CExoString(nValue2));
            sFeedback = g_pSWTlkTable->Fetch(0, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_OBJECT_COUNTER_REMAINING:
        {
            oidObject = pMessageData->GetObjectID(0);
            int nValue2 = pMessageData->GetInteger(0);
            int nValue3 = pMessageData->GetInteger(1);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            g_pSWTlkTable->SetCustomToken(1, CExoString(nValue2));
            g_pSWTlkTable->SetCustomToken(2, CExoString(nValue3));
            sFeedback = g_pSWTlkTable->Fetch(0, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_WEAPON_INEFFECTIVE:
        {
            sFeedback = GetGUIString(0x5C1u);
            break;
        }
        case CSWSCreature::FEEDBACK_FEAT_USES_EXHAUSTED:
        {
            sFeedback = GetGUIString(0x5E6u);
            break;
        }
        case CSWSCreature::FEEDBACK_PARTY_LEADER_CHANGED:
        {
            oidObject = pMessageData->GetObjectID(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            sFeedback = g_pSWTlkTable->Fetch(0x592u, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_032:
        {
            oidObject = pMessageData->GetObjectID(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            sFeedback = g_pSWTlkTable->Fetch(0, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_033:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_034:
        {
            oidObject = pMessageData->GetObjectID(0);
            if ( GetObjectName(oidObject, sObjectName) != 0 )
            {
              g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            else
            {
            sObjectName = GetGUIString(649);
            }
            sFeedback = g_pSWTlkTable->Fetch(0, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_035:
        {
            oidObject = pMessageData->GetObjectID(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            sFeedback = g_pSWTlkTable->Fetch(0, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_036:
        {
            oidObject = pMessageData->GetObjectID(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            sFeedback = g_pSWTlkTable->Fetch(0, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_037:
        {
            oidObject = pMessageData->GetObjectID(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            sFeedback = g_pSWTlkTable->Fetch(0, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_PARTY_MEMBER_JOINED:
        {
            oidObject = pMessageData->GetObjectID(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            sFeedback = g_pSWTlkTable->Fetch(0x591u, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_039:
        {
            oidObject = pMessageData->GetObjectID(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            sFeedback = g_pSWTlkTable->Fetch(0, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_040:
        {
            oidObject = pMessageData->GetObjectID(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            sFeedback = g_pSWTlkTable->Fetch(0, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_041:
        {
            oidObject = pMessageData->GetObjectID(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            sFeedback = g_pSWTlkTable->Fetch(0, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_042:
        {
            oidObject = pMessageData->GetObjectID(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            sFeedback = g_pSWTlkTable->Fetch(0, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_INVITATION_EXPIRED:
        {
            oidObject = pMessageData->GetObjectID(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            sFeedback = g_pSWTlkTable->Fetch(0, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_PARTY_MEMBER_LEFT:
        {
            oidObject = pMessageData->GetObjectID(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            sFeedback = g_pSWTlkTable->Fetch(0x590u, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_045:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_196:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_197:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_202:
        {
            oidObject = pMessageData->GetObjectID(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            sFeedback = g_pSWTlkTable->Fetch(0, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_078:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_079:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_224:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_080:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_081:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_219:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_220:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_221:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_FORCE_POWER_NAME:
        {
            CExoString sSpell;
            CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(pMessageData->GetInteger(0));
            if (pSpell != NULL)
                sSpell = pSpell->GetSpellNameText();
            g_pTlkTable->SetCustomToken(0, sSpell);
            sFeedback = g_pSWTlkTable->Fetch(0, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_FLOATY_DATA:
        {
            int nValue = pMessageData->GetInteger(0);
            sFeedback = g_pSWTlkTable->Fetch(nValue, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_FLOATY_STRING:
        {
            sFeedback = pMessageData->GetString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_CUSTOM_STRING:
        {
            sFeedback = pMessageData->GetString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_EXPERIENCE_POINTS_RECEIVED:
        {
            int nValue = pMessageData->GetInteger(0);
            g_pTlkTable->SetCustomToken(0, nValue);
            sFeedback = g_pSWTlkTable->Fetch(0xA5C6u, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_106:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_ITEM_RECEIVED:
        {
            g_pTlkTable->SetCustomToken(0, pMessageData->GetString(0));
            sFeedback = g_pSWTlkTable->Fetch(0x5A9u, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_ITEM_LOST:
        {
            g_pTlkTable->SetCustomToken(0, pMessageData->GetString(0));
            sFeedback = g_pSWTlkTable->Fetch(0x5AAu, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_INVENTORY_ITEM_DROPPED:
        {
            sFeedback = GetGUIString(0x5BCu);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_097:
        {
            sFeedback = GetGUIString(0);
            break;
        }
        case CSWSCreature::FEEDBACK_GOLD_ADDED:
        {
            int nValue = pMessageData->GetInteger(0);
            g_pTlkTable->SetCustomToken(0, CExoString(nValue));
            sFeedback = g_pSWTlkTable->Fetch(0x5D5u, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_COMPONENTS_ADDED:
        {
            int nValue = pMessageData->GetInteger(0);
            g_pTlkTable->SetCustomToken(0, CExoString(nValue));
            sFeedback = g_pSWTlkTable->Fetch(128117, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_CHEMICALS_ADDED:
        {
            int nValue = pMessageData->GetInteger(0);
            g_pTlkTable->SetCustomToken(0, CExoString(nValue));
            sFeedback = g_pSWTlkTable->Fetch(128118, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_GOLD_REMOVED:
        {
            int nValue = pMessageData->GetInteger(0);
            g_pTlkTable->SetCustomToken(0, CExoString(nValue));
            sFeedback = g_pSWTlkTable->Fetch(0x5D6u, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_ALIGNMENT_SHIFT:
        {
            int nShift = pMessageData->GetInteger(1);
            unsigned long nStrRef;
            if (nShift >= 10)
                nStrRef = 41921;
            else if (nShift >= 5)
                nStrRef = 41922;
            else
                nStrRef = 41923;
            CExoString sShift = GetGUIString(nStrRef);
            g_pTlkTable->SetCustomToken(0, sShift);
            CExoString sAlignment;
            switch (pMessageData->GetInteger(0))
            {
                case ALIGNMENT_LIGHT_SIDE:
                    sAlignment = GetGUIString(41924);
                    break;
                default:
                    sAlignment = GetGUIString(41925);
                    break;
            }
            g_pTlkTable->SetCustomToken(1, sAlignment);
            sFeedback = g_pSWTlkTable->Fetch(1471, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_DISEASE_IMMUNITY:
        {
            bCombatMessage = 1;
            oidObject = pMessageData->GetObjectID(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            sFeedback = g_pSWTlkTable->Fetch(0x5C7u, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_CRITICAL_HITS_IMMUNITY:
        {
            bCombatMessage = 1;
            oidObject = pMessageData->GetObjectID(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            sFeedback = g_pSWTlkTable->Fetch(0x5C8u, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_DEATH_IMMUNITY:
        {
            bCombatMessage = 1;
            oidObject = pMessageData->GetObjectID(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            sFeedback = g_pSWTlkTable->Fetch(0, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_FEAR_IMMUNITY:
        {
            bCombatMessage = 1;
            oidObject = pMessageData->GetObjectID(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            sFeedback = g_pSWTlkTable->Fetch(0x5C9u, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_HORRIFIED_IMMUNITY:
        {
            bCombatMessage = 1;
            oidObject = pMessageData->GetObjectID(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            sFeedback = g_pSWTlkTable->Fetch(0xA623u, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_KNOCKDOWN_IMMUNITY:
        {
            bCombatMessage = 1;
            oidObject = pMessageData->GetObjectID(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            sFeedback = g_pSWTlkTable->Fetch(0x5CAu, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_PARALYSIS_IMMUNITY:
        {
            bCombatMessage = 1;
            oidObject = pMessageData->GetObjectID(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            sFeedback = g_pSWTlkTable->Fetch(0x5CBu, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_NEGATIVE_LEVEL_IMMUNITY:
        {
            bCombatMessage = 1;
            oidObject = pMessageData->GetObjectID(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            sFeedback = g_pSWTlkTable->Fetch(0x5CCu, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_MIND_SPELLS_IMMUNITY:
        {
            bCombatMessage = 1;
            oidObject = pMessageData->GetObjectID(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            sFeedback = g_pSWTlkTable->Fetch(0x5CDu, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_POISON_IMMUNITY:
        {
            bCombatMessage = 1;
            oidObject = pMessageData->GetObjectID(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            sFeedback = g_pSWTlkTable->Fetch(0x5CEu, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_SNEAK_ATTACK_IMMUNITY:
        {
            bCombatMessage = 1;
            oidObject = pMessageData->GetObjectID(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            sFeedback = g_pSWTlkTable->Fetch(0x5CFu, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_SLEEP_IMMUNITY:
        {
            bCombatMessage = 1;
            oidObject = pMessageData->GetObjectID(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            sFeedback = g_pSWTlkTable->Fetch(0x5D0u, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_DAZED_IMMUNITY:
        {
            bCombatMessage = 1;
            oidObject = pMessageData->GetObjectID(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            sFeedback = g_pSWTlkTable->Fetch(0, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_CONFUSED_IMMUNITY:
        {
            bCombatMessage = 1;
            oidObject = pMessageData->GetObjectID(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            sFeedback = g_pSWTlkTable->Fetch(0x5D1u, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_STUN_IMMUNITY:
        {
            bCombatMessage = 1;
            oidObject = pMessageData->GetObjectID(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            sFeedback = g_pSWTlkTable->Fetch(0x5D2u, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_BLINDNESS_IMMUNITY:
        {
            bCombatMessage = 1;
            oidObject = pMessageData->GetObjectID(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            sFeedback = g_pSWTlkTable->Fetch(0, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_DEAFNESS_IMMUNITY:
        {
            bCombatMessage = 1;
            oidObject = pMessageData->GetObjectID(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            sFeedback = g_pSWTlkTable->Fetch(0, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_CURSED_IMMUNITY:
        {
            bCombatMessage = 1;
            oidObject = pMessageData->GetObjectID(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            sFeedback = g_pSWTlkTable->Fetch(0, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_ENTANGLE_IMMUNITY:
        {
            bCombatMessage = 1;
            oidObject = pMessageData->GetObjectID(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            sFeedback = g_pSWTlkTable->Fetch(0x5D3u, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_SILENCE_IMMUNITY:
        {
            bCombatMessage = 1;
            oidObject = pMessageData->GetObjectID(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            sFeedback = g_pSWTlkTable->Fetch(0, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_SLOW_IMMUNITY:
        {
            bCombatMessage = 1;
            oidObject = pMessageData->GetObjectID(0);
            if (GetObjectName(oidObject, sObjectName))
                g_pTlkTable->SetCustomToken(0, sObjectName);
            else
            {
                sObjectName = GetGUIString(649);
                g_pTlkTable->SetCustomToken(0, sObjectName);
            }
            sFeedback = g_pSWTlkTable->Fetch(0x5D4u, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_SCRIPT_REPORT:
        {
            CExoString sReport;
            CExoString sTag;
            oidObject = pMessageData->GetObjectID(0);
            sReport = pMessageData->GetString(0);
            sTag = pMessageData->GetString(1);
            CExoString sReason = GetGUIString(pMessageData->GetInteger(0));
            CExoString sObject;
            sObject.Format("OID: %08x, Tag: %s, %s", oidObject, sTag.CStr(), sReason.CStr());
            g_pTlkTable->SetCustomToken(0, sObject);
            g_pTlkTable->SetCustomToken(1, sReport);
            sFeedback = g_pSWTlkTable->Fetch(1496, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_ACTION_LIST_OVERFLOW:
        case CSWSCreature::FEEDBACK_ACTION_LIST_ERASED:
        {
            CExoString sObjectName;
            CExoString sTag;
            oidObject = pMessageData->GetObjectID(0);
            sTag = pMessageData->GetString(1);
            int nCount = pMessageData->GetInteger(0);
            int nLimit = pMessageData->GetInteger(1);
            CExoString sDetails;
            sDetails.Format("(%d > %d) OID: %08x, Tag: %s", nCount, nLimit, oidObject, sTag.CStr());
            g_pTlkTable->SetCustomToken(0, sDetails);
            if (nType == CSWSCreature::FEEDBACK_ACTION_LIST_ERASED)
                sFeedback = g_pSWTlkTable->Fetch(1500, NULL, 0, 0);
            else
                sFeedback = g_pSWTlkTable->Fetch(1497, NULL, 0, 0);
            sFeedback.CStr();

            goto FeedbackDone;
        }
        case CSWSCreature::FEEDBACK_EFFECT_LIST_OVERFLOW:
        case CSWSCreature::FEEDBACK_EFFECT_LIST_ERASED:
        {
            CExoString sObjectName;
            CExoString sTag;
            oidObject = pMessageData->GetObjectID(0);
            sTag = pMessageData->GetString(1);
            int nCount = pMessageData->GetInteger(0);
            int nLimit = pMessageData->GetInteger(1);
            CExoString sDetails;
            sDetails.Format("(%d > %d) OID: %08x, Tag: %s", nCount, nLimit, oidObject, sTag.CStr());
            g_pTlkTable->SetCustomToken(0, sDetails);
            if (nType == CSWSCreature::FEEDBACK_EFFECT_LIST_ERASED)
                sFeedback = g_pSWTlkTable->Fetch(1501, NULL, 0, 0);
            else
                sFeedback = g_pSWTlkTable->Fetch(1498, NULL, 0, 0);
            sFeedback.CStr();
            sFeedback.CStr();
            sDetails.CStr(); sFeedback.CStr();
            sDetails.CStr();
            goto FeedbackDone;
        }
        case CSWSCreature::FEEDBACK_AI_UPDATE_TIME_OVERFLOW:
        {
            CExoString sEvent;
            CExoString sTag;
            CExoString sScripts;
            oidObject = pMessageData->GetObjectID(0);
            sTag = pMessageData->GetString(1);
            sScripts = pMessageData->GetString(2);
            int nCount = pMessageData->GetInteger(0);
            int nLimit = pMessageData->GetInteger(1);
            int nAction = pMessageData->GetInteger(2);
            int nFlags = pMessageData->GetInteger(3);
            CExoString sDetails;
            sDetails.Format("(%d > %d) Scripts: (%s), Action: %d [Flags: %02x], OID: %08x, Tag: %s",
                nCount, nLimit, sScripts.CStr(), nAction, nFlags, oidObject, sTag.CStr());
            g_pTlkTable->SetCustomToken(0, sDetails);
            sFeedback = g_pSWTlkTable->Fetch(1499, NULL, 0, 0);
            sFeedback.CStr();
            goto FeedbackDone;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_201:
        {
            sFeedback = g_pSWTlkTable->Fetch(0, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_PAUSED:
        {
            sFeedback = g_pSWTlkTable->Fetch(0x5E4u, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNPAUSED:
        {
            sFeedback = g_pSWTlkTable->Fetch(0x5E5u, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_225:
        {
            sFeedback = g_pSWTlkTable->Fetch(0, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_226:
        {
            sFeedback = g_pSWTlkTable->Fetch(0, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_214:
        {
            sFeedback = g_pSWTlkTable->Fetch(0, NULL, 0, 0);
            break;
        }
        case CSWSCreature::FEEDBACK_UNKNOWN_215:
        {
            sFeedback = g_pSWTlkTable->Fetch(0, NULL, 0, 0);
            break;
        }
        case FEEDBACK_MESSAGE_CHARACTER_EXPORT_SUCCESS:
        {
            sFeedback = g_pSWTlkTable->Fetch(0, NULL, 0, 0);
            break;
        }
        case FEEDBACK_MESSAGE_CHARACTER_EXPORT_FAILED:
        {
            sFeedback = g_pSWTlkTable->Fetch(0, NULL, 0, 0);
            break;
        }
    }
    if (!sFeedback.IsEmpty())
    {
        sFeedback = sMessage + sFeedback;
        if (bCombatMessage)
            pGuiInGame->AppendToCombatBuffer(sFeedback, 0x80, 0);
        else
            pGuiInGame->AppendToMsgBuffer(sFeedback, 0x80, 0);
    }
FeedbackDone:
    delete pMessageData;
}
// PC 0x0064E670 GOG 0x0064E520
int CClientExoAppInternal::PlayLoadModuleMovies()
{
#ifdef _XBOX
    if (m_pMoviePlayer == NULL)
    {
        return 0;
    }
#else
    if (m_pMoviePlayer == NULL || m_bMovieDisabled != 0)
    {
        return 0;
    }
#endif

    CExoArrayList<CExoString> lstMovies(
        *g_pAppManager->m_pClientExoApp->GetMoveToModuleMovies());

    if (lstMovies.num > 0)
    {
        if (g_pAppManager->m_pServerExoApp != NULL)
        {
            g_pAppManager->m_pClientExoApp->GetWorldTimer()->PauseWorldTimer();
            g_pAppManager->m_pServerExoApp->GetWorldTimer()->PauseWorldTimer();
            m_pGuiInGame->SetGlobalUnpauseableState(1);
            g_pExoInput->ClearEvents();
#ifdef _XBOX
            g_pExoInput->CoolDownEvent(MOVIE_SKIP_EVENT_FIRST, MOVIE_SKIP_COOLDOWN);
            g_pExoInput->CoolDownEvent(MOVIE_SKIP_EVENT_SECOND, MOVIE_SKIP_COOLDOWN);
#endif
            g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_PAUSE_ALL);
        }

#ifdef _XBOX
        ShowLoadScreen(1, 1, 0);
        m_pMoviePlayer->ShowFadePanel();
        m_pMoviePlayer->PlayMoviesAsync(lstMovies, 0);
#else
        ChangeResolutionForMovie();
        ShowLoadScreen(1, 1, 0);
        m_pMoviePlayer->ShowFadePanel();
        m_pMoviePlayer->PlayMoviesAsync(lstMovies, 0, NULL);
#endif
        Sleep(0);
        m_bEnableMouseAtEndOfLoadModuleMovie = 1;
        return 1;
    }

    return 0;
}
// PC 0x006426F0 GOG 0x006425A0
void CClientExoAppInternal::ShowLoadScreen(int bFade, int bPlayMusic, int bSaving)
{
    if (g_pExoSound != NULL)
    {
        if (bSaving)
            g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_PAUSE_ALL);
        else if (m_sLoadMusic != "")
            g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_PAUSE_ALL_BUT_MUSIC);
    }

    if (m_bShowLoadScreen && m_sLoadScreenImage.GetLength())
    {
        if (m_pGuiLoadScreen == NULL)
            m_pGuiLoadScreen = new CSWGuiLoadScreen(m_pSWGuiManager);

#ifdef _XBOX
        m_pGuiLoadScreen->SetImage(m_sLoadScreenImage);
#else
        CResRef cImage(m_sLoadScreenImage);
        m_pGuiLoadScreen->SetImage(cImage);
#endif
        m_pGuiLoadScreen->SetSaving(bSaving);

        if (m_pSWGuiManager != NULL && !m_pSWGuiManager->PanelExists(m_pGuiLoadScreen))
        {
            m_nLoadScreenStartTime = g_pExoBase->m_pExoTimers->GetLowResolutionTimer();
            if (bFade && m_pGuiInGame != NULL)
            {
                Vector vBlack(0.0f, 0.0f, 0.0f);
                m_pGuiInGame->StartGlobalFade(0, 0.0f, 0.0f, vBlack);
            }
            m_pSWGuiManager->AddPanel(m_pGuiLoadScreen, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
        }

        SetInputActive(0, 0);
#ifndef _XBOX
        HideMouse(CLIENT_MOUSE_HIDE_LOAD_SCREEN);
#endif
        if (bPlayMusic)
            StartLoadMusic();
    }
}

#ifdef _XBOX
void CClientExoAppInternal::HideLoadScreen(int bFade)
{
#else
void CClientExoAppInternal::HideLoadScreen(int bFade, int bStopLoadMusic,
                                           int bUpdateScreen)  // PC 0x006428D0 GOG 0x00642780
{
    ShowMouse(CLIENT_MOUSE_HIDE_LOAD_SCREEN);
#endif

    if (m_pGuiLoadScreen != NULL && m_pSWGuiManager != NULL)
    {
        if (bFade)
        {
            Vector vBlack(0.0f, 0.0f, 0.0f);
            m_pGuiInGame->StartGlobalFade(0, 0.0f, 0.0f, vBlack);
        }

        m_pSWGuiManager->PopModalPanel();
#ifdef _XBOX
        m_pSWGuiManager->RemovePanel(m_pGuiLoadScreen);
#else
        if (m_pSWGuiManager->RemovePanel(m_pGuiLoadScreen))
        {
            m_pGuiLoadScreen->AddHintToMessageBuffer();
        }
#endif

        m_pGuiLoadScreen->m_pbProgress.SetCurValue(0);
        m_pGuiLoadScreen->SetBackground("");
        SetInputActive(1, 0);
#ifdef _XBOX
        StopLoadMusic();
#endif

#ifdef _XBOX
        if (!bFade)
#else
        if (!bFade && bUpdateScreen)
#endif
        {
#ifndef _XBOX
            float afClearColor[4];
            AurGetClearColor(afClearColor);
            AurSetClearColor(0.0f, 0.0f, 0.0f, 1.0f);
#endif
            UpdateScreen(1.0f / 30.0f, 0, 0);
#ifndef _XBOX
            AurSetClearColor(afClearColor[0], afClearColor[1], afClearColor[2],
                             afClearColor[3]);
#endif
        }
    }
}

#ifdef _XBOX

int CClientExoAppInternal::AddMoveToModuleMovie(const CExoString& sMovie)
{
    if (sMovie != "")
    {
        int nAlias = sMovie.Find(":", 0);
        if (nAlias != -1)
        {
            m_aMoveToModuleMovies.AddUnique(sMovie);
            return 1;
        }

        CExoString sFileName;
        CExoString sAlias;
        sFileName.Format("MOVIES:%s", sMovie.CStr());
        int bFound = 0;
        CExoFile* pFile = new CExoFile(sFileName, RESTYPE_XMV, "rb");

        if (!pFile->FileOpened())
        {
            for (int nPackage = 1; nPackage < CTlkTable::NUM_CONTENT_PKGS; ++nPackage)
            {
                if (pFile != NULL)
                {
                    delete pFile;
                    pFile = NULL;
                }

                sAlias.Format("LIVE%d", nPackage);
                if (g_pExoBase->m_pExoAliasList->GetAliasPath(sAlias, 0).GetLength())
                {
                    sFileName.Format("LIVE%d:movies\\%s", nPackage, sMovie.CStr());
                    pFile = new CExoFile(sFileName, RESTYPE_XMV, "rb");
                    if (pFile->FileOpened())
                    {
                        bFound = 1;
                        break;
                    }
                }
            }
        }
        else
        {
            bFound = 1;
        }

        if (pFile != NULL)
        {
            delete pFile;
        }

        if (bFound)
        {
            sFileName = g_pExoBase->m_pExoAliasList->ResolveFileName(sFileName, RESTYPE_XMV);
            m_aMoveToModuleMovies.AddUnique(sFileName);
            m_pClientOptions->SetMovieShown(sMovie, 1);
            return 1;
        }
    }
    return 0;
}
#else
int CClientExoAppInternal::AddMoveToModuleMovie(const CExoString& sMovie)  // PC 0x0064E7C0 GOG 0x0064E670
{
    return AddMovieToExoArrayList(sMovie, m_aMoveToModuleMovies);
}
#endif
// PC 0x0064E7E0 GOG 0x0064E690
void CClientExoAppInternal::AddFloatyText(unsigned long oidObject,
                                        unsigned char nType, int nValue)
{
    CClientOptions* pOptions = g_pAppManager->GetClientExoApp()->GetClientOptions();
    if (pOptions == NULL || pOptions->m_bFloatingNumbers)
    {
        Vector vColor;
        CExoString sText;
        float fDuration;
        switch (nType)
        {
        case FLOATY_TEXT_HEAL:
            sText = nValue;
            vColor = CGuiInGame::COLOR_GREEN;
            fDuration = 1.5f;
            break;
        case FLOATY_TEXT_DAMAGE:
            sText = nValue;
            vColor = CGuiInGame::COLOR_RED;
            fDuration = 1.5f;
            break;
        case FLOATY_TEXT_MISS:
            sText = GetGUIString(STRREF_FLOATY_MISS);
            vColor = CGuiInGame::COLOR_WHITE;
            fDuration = 1.5f;
            break;
        case FLOATY_TEXT_XP:
            sText.Format("%s %d", GetGUIString(STRREF_FLOATY_XP).CStr(), nValue);
            vColor = CGuiInGame::COLOR_PURPLE;
            fDuration = 3.0f;
            break;
        case FLOATY_TEXT_LEVEL:
            sText.Format("%s %d", GetGUIString(STRREF_FLOATY_LEVEL).CStr(), nValue);
            vColor = CGuiInGame::COLOR_ORANGE;
            fDuration = 3.0f;
            break;
        case FLOATY_TEXT_SNEAK_ATTACK:
            sText = GetGUIString(STRREF_FLOATY_SNEAK_ATTACK);
            vColor = CGuiInGame::COLOR_WHITE;
            fDuration = 1.5f;
            break;
        case FLOATY_TEXT_SPOTTED:
            sText = GetGUIString(STRREF_FLOATY_SPOTTED);
            vColor = CGuiInGame::COLOR_WHITE;
            fDuration = 1.5f;
            break;
        default:
            return;
        }
        m_pGuiInGame->AddFloatyText(oidObject, sText, vColor, fDuration);
    }
}

#ifdef _XBOX

int CClientExoAppInternal::PlayMovie(const CExoString& sMovie, int& nResult,
                                     int bAnyButtonSkips, int bNoMusic)
{

    m_PauseFlags.m_Bits.m_nUnused1 = bAnyButtonSkips;
    int bPlayMusic = 0;

    if (!bNoMusic && sMovie.CompareNoCase("55"))
    {
        bPlayMusic = 1;
        if (m_pEvilEndingHackSound != NULL)
        {
            delete m_pEvilEndingHackSound;
            m_pEvilEndingHackSound = NULL;
        }

        m_pModule->GetArea()->GetAmbientSound()->StopSounds(1);
        m_pGuiInGame->LoadAmbientTrackForDialog(CResRef());
        g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_GENERIC);
        m_pEvilEndingHackSound = new CExoStreamingSoundSource;
        m_pEvilEndingHackSound->SetPriorityGroup(8);

#define XB7_MOVIE_VOLUME max(0.0f, g_pAppManager->GetClientExoApp()->GetClientOptions()->m_nMovieVolume * 127.0f)
        m_pEvilEndingHackSound->SetVolume(
            (unsigned char)((XB7_MOVIE_VOLUME > 127.0f) ? 127.0f : XB7_MOVIE_VOLUME));
#undef XB7_MOVIE_VOLUME

        m_pEvilEndingHackSound->Play("evil_end", 11, 1, 1, 1);
        m_pGuiInGame->SetXboxUnmappedC0C(0);
    }

    if (!AddMoveToModuleMovie(sMovie))
    {
        return 0;
    }

    m_pGuiInGame->SetGlobalUnpauseableState(1);
    nResult = 0;
    CExoString sRepeat("F0");
    CExoString sCurrent;
    int bInputActive = g_pExoInput->GetActive();

    if (bInputActive == 0 && g_pExoInput != NULL)
    {
        g_pExoInput->SetActive(1);
    }

    g_pExoInput->ClearEvents();
    g_pExoInput->CoolDownEvent(MOVIE_SKIP_EVENT_FIRST, MOVIE_SKIP_COOLDOWN);
    g_pExoInput->CoolDownEvent(MOVIE_SKIP_EVENT_SECOND, MOVIE_SKIP_COOLDOWN);

    do
    {
        if (!bPlayMusic)
        {
            g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_PAUSE_ALL);
        }
        m_pMoviePlayer->PlayMoviesAsync(m_aMoveToModuleMovies, bPlayMusic);
        m_pMoviePlayer->ShowFadePanel();
        nResult = WaitForMovie();
    } while (sRepeat == sCurrent);

    if (bInputActive == 0)
    {
        if (g_pExoInput != NULL)
        {
            g_pExoInput->SetActive(0);
        }
        TurnOffKeyboardModal();
    }

    m_pGuiInGame->SetGlobalUnpauseableState(0);
    if (!bPlayMusic)
    {
        g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_GENERIC);
    }
    m_pClientOptions->SetMovieShown(sMovie, 1);
    m_PauseFlags.m_Bits.m_nUnused1 = 0;
    return 1;
}

#else
int CClientExoAppInternal::PlayMovie(const CExoString& sMovie, int& nA, int nB,  // PC 0x0064ED10 GOG 0x0064EBC0
                                     int nC, int nSoundModeFlags)
{
    if (m_bMovieDisabled != 0)
    {
        return 0;
    }

    if (!AddMovieToExoArrayList(sMovie, m_aMoveToModuleMovies))
    {
        return 0;
    }

    return PlayMoviesInExoArrayList(m_aMoveToModuleMovies, nA, 1, 0, NULL,
                                    nSoundModeFlags);
}

#ifndef _XBOX
int CClientExoAppInternal::PlayMovieQueue(int nFlags)  // PC 0x0064ED60 GOG 0x0064EC10
{
    int nMovie;
    int nResult = PlayMoviesInExoArrayList(m_aMovieQueue, nMovie, 0, nFlags,
                                           &m_aMovieQueueFlags, 0);
    int& nMovies = m_aMovieQueue.num;
    nMovies = 0;
    m_aMovieQueueFlags.num = 0;

    return nResult;
}
#endif
// PC 0x0064EB10 GOG 0x0064E9C0
int CClientExoAppInternal::PlayMoviesInExoArrayList(
    CExoArrayList<CExoString>& lstMovies, int& nResult, int nWait, int nMode,
    CExoArrayList<int>* lstSkippable, int nSoundModeFlags)
{
    if (m_bMovieDisabled != 0)
    {
        return 0;
    }

    m_pGuiInGame->SetGlobalUnpauseableState(1);
    nResult = 0;

    int bInputActive = g_pExoInput->GetActive();

    if (bInputActive == 0)
    {
        if (g_pExoInput != NULL)
        {
            g_pExoInput->SetActive(1);
        }

        ShowMouse(0);
    }

    g_pExoInput->ClearEvents();
    g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_PAUSE_ALL,
                              nSoundModeFlags);
    HideMouse(8);
    ChangeResolutionForMovie();
    m_pMoviePlayer->ShowFadePanel();
    m_pMoviePlayer->PlayMoviesAsync(lstMovies, nMode, lstSkippable);
    Sleep(0);
    nResult = WaitForMovie(nWait);
    RestoreResolutionFromMovie();
    ShowMouse(8);
    m_pMoviePlayer->HideFadePanel();
    lstMovies.num = 0;

    if (lstSkippable != NULL)
    {
        lstSkippable->num = 0;
    }

    if (bInputActive == 0)
    {
        if (g_pExoInput != NULL)
        {
            g_pExoInput->SetActive(0);
        }

        TurnOffKeyboardModal();
    }

    m_pGuiInGame->SetGlobalUnpauseableState(0);

    return 1;
}
// PC 0x00642A00 GOG 0x006428B0
int CClientExoAppInternal::WaitForMovie(int nFlags)
{
    int nPumpCount = 0;
    int bCancelled = 0;
    int bInputActive = g_pExoInput->GetActive();
    int bRenderWindowActive = g_bRenderWindowActive;

    SetInputActive(1, 0);

    if (m_bEvilEndingHackInProgress)
    {
        if (m_pEvilEndingHackSound != NULL)
        {
            m_pEvilEndingHackSound->Stop();
            delete m_pEvilEndingHackSound;
        }

        CResRef cNoTrack("");
        g_pAppManager->GetClientExoApp()->GetInGameGui()->LoadAmbientTrackForDialog(cNoTrack);
        m_pEvilEndingHackSound = new CExoStreamingSoundSource;
        m_pEvilEndingHackSound->SetVolume(
            (unsigned char)(g_pAppManager->GetClientExoApp()->GetClientOptions()->m_nMovieVolume * 1.27f));
        CResRef cEvilEnding("evil_ending");
        m_pEvilEndingHackSound->Play(cEvilEnding, RESTYPE_MP3, 0, 1, 0);
    }

    while (IsMoviePlaying() == 1)
    {
        if (nPumpCount == 500 && !bCancelled)
        {
            messagepump();
            if (m_bExitGamePending == 1)
            {
                if (m_pMoviePlayer != NULL)
                {
                    m_pMoviePlayer->CancelMovie(1, 0);
                }
                bCancelled = 1;
            }
            nPumpCount = 0;
        }
        else
        {
            nPumpCount++;
        }
    }

    if (nFlags)
    {
        g_pAppManager->GetClientExoApp()->RemoveMoveToModuleMovies();
        g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_GENERIC);
        if (m_bEnableMouseAtEndOfLoadModuleMovie)
        {
            m_pGuiInGame->SetGlobalUnpauseableState(0);
            g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_GENERIC);
            CWorldTimer* pClientTimer = g_pAppManager->GetClientExoApp()->GetWorldTimer();
            pClientTimer->UnpauseWorldTimer();
            CWorldTimer* pServerTimer = g_pAppManager->GetServerExoApp()->GetWorldTimer();
            pServerTimer->UnpauseWorldTimer();
            m_bEnableMouseAtEndOfLoadModuleMovie = 0;
        }
    }

    if (g_bRenderWindowActive == bRenderWindowActive)
    {
        SetInputActive(bInputActive, 0);
    }

    ShowMouse(8);
    return 0;
}
// PC 0x00642C80 GOG 0x00642B30
void CClientExoAppInternal::ShutDownMoviePlayer()
{
    if (m_pMoviePlayer != NULL)
    {
        while (m_pMoviePlayer->IsMoviePlaying() == 1)
        {
            m_pMoviePlayer->CancelMovie(1, 1);
            Sleep(5);
        }

        delete m_pMoviePlayer;
        m_pMoviePlayer = NULL;
    }
}
#endif
// PC 0x00642CF0 GOG 0x00642BA0
void CClientExoAppInternal::RunDeathSequence()
{
    m_nGameOver = 1;
    m_bGameOverPending = 1;
    m_pSWGuiManager->HideAllPanels();
    m_pGuiInGame->DisplayDeathMessage();

    CSWCCreature* pCreature =
        GetCreatureByGameObjectID(m_oidLastPartyMemberTempKilled);
    if (m_pModule != NULL && pCreature != NULL)
    {
        CAurCamera* pCamera = m_pModule->m_pModuleCamera;
        if (pCamera != NULL)
        {
            pCamera->SetBehavior(new CSWCameraDeath(
                pCamera->AsCAurObject(), pCreature->GetCameraFollowPart()));
        }
    }

    Vector vZero;
    m_pGuiInGame->StartGlobalFade(0, 12.0f, 1.0f, vZero);
}

#ifndef _XBOX
#endif
// PC 0x00642E10 GOG 0x00642CC0
int CClientExoAppInternal::StopRumblePattern(int nPattern)
{
    for (int i = 0; i < m_aRumbleInstances.num; i++)
    {
        if (m_aRumbleInstances.element[i].m_nPattern == nPattern)
        {
            m_aRumbleInstances.DelIndex(i);
            return 1;
        }
    }

    return 0;
}
#ifndef _XBOX
void CClientExoAppInternal::UpdateRumble(float fDeltaTime)  // PC 0x00642E60 GOG 0x00642D10
{
    float fLeftMagnitude = 0.0f;
    float fRightMagnitude = 0.0f;
    if (fDeltaTime > 0.0f && !m_nAreaNotReadyToDisplay &&
        !m_pGuiInGame->IsGlobalFadeObscuring() &&
        !m_pGuiInGame->IsGlobalFading() && !GetLoadScreenVisible())
    {
        for (int i = m_aRumbleInstances.num - 1; i >= 0; i--)
        {
            CSWRumbleInstance& cInstance = m_aRumbleInstances.element[i];
            if (cInstance.m_bFirstUpdate)
                cInstance.m_bFirstUpdate = 0;
            else
                cInstance.m_fTime += fDeltaTime;

            float fLeft;
            float fRight;
            if (!m_aRumblePatterns.element[cInstance.m_nPattern].GetMagnitudes(
                    cInstance.m_fTime, fLeft, fRight))
                m_aRumbleInstances.DelIndex(i);
            else
            {
                fLeftMagnitude = fLeftMagnitude > fLeft ? fLeftMagnitude : fLeft;
                fRightMagnitude = fRightMagnitude > fRight ? fRightMagnitude : fRight;
            }
        }
    }
    g_pExoInput->SetRumble(0, fLeftMagnitude, fRightMagnitude,
                          CLIENT_RUMBLE_DURATION);
}
#endif
void CClientExoAppInternal::UpdateHostileHilite(float fElapsedTime)  // PC 0x00642FA0 GOG 0x00642E50
{
    for (int i = m_aHostileHilites.num - 1; i >= 0; i--)
    {
        m_aHostileHilites[i].m_fTimeRemaining -= fElapsedTime;
        if (m_aHostileHilites[i].m_fTimeRemaining <= 0.0f)
        {
            CSWCCreature* pCreature =
                GetCreatureByGameObjectID(m_aHostileHilites[i].m_oidObject);
            if (pCreature != NULL)
            {
                pCreature->UnHilite(CSWCObject::HILITE_CHANNEL_HOSTILE);
            }
            m_aHostileHilites.DelIndex(i);
        }
    }
}
// PC 0x0063C3C0 GOG 0x0063C2C0
CSWCCreature* CClientExoAppInternal::GetCreatureByGameObjectID(
    unsigned long nId)
{
    CGameObjectArray* pArray = m_pGameObjectArray;
    CGameObject* pObject = NULL;
    if (pArray->GetGameObject(nId, &pObject) == CGameObjectArray::SUCCESS &&
        pObject != NULL && pObject->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
        return pObject->AsSWCCreature();
    return NULL;
}
// PC 0x0063C410 GOG 0x0063C310
CSWCArea* CClientExoAppInternal::GetAreaByGameObjectID(unsigned long nId)
{
    CGameObject* pObject = NULL;

    if (m_pGameObjectArray->GetGameObject(nId, &pObject) ==
            CGameObjectArray::SUCCESS &&
        pObject != NULL &&
        pObject->m_nObjectType == CGameObject::OBJECT_TYPE_AREA)
    {
        return pObject->AsSWCArea();
    }

    return NULL;
}
// PC 0x0063C460 GOG 0x0063C360
CSWCTrigger* CClientExoAppInternal::GetTriggerByGameObjectID(
    unsigned long oidObject)
{
    CGameObject* pObject = NULL;

    if (m_pGameObjectArray->GetGameObject(oidObject, &pObject) ==
            CGameObjectArray::SUCCESS &&
        pObject != NULL &&
        pObject->m_nObjectType == CGameObject::OBJECT_TYPE_TRIGGER)
    {
        return pObject->AsSWCTrigger();
    }

    return NULL;
}

// PC 0x0063C4B0 GOG 0x0063C3B0
CSWCProjectile* CClientExoAppInternal::GetProjectileByGameObjectID(
    unsigned long oidObject)
{
    CGameObject* pObject = NULL;
    if (m_pGameObjectArray->GetGameObject(oidObject, &pObject) == CGameObjectArray::SUCCESS &&
        pObject != NULL && pObject->m_nObjectType == CGameObject::OBJECT_TYPE_PROJECTILE)
        return pObject->AsSWCProjectile();
    return NULL;
}
// PC 0x0063C500 GOG 0x0063C400
CSWCPlaceable* CClientExoAppInternal::GetPlaceableByGameObjectID(
    unsigned long oidObject)
{
    CGameObject* pObject = NULL;

    if (m_pGameObjectArray->GetGameObject(oidObject, &pObject) ==
            CGameObjectArray::SUCCESS &&
        pObject != NULL &&
        pObject->m_nObjectType == CGameObject::OBJECT_TYPE_PLACEABLE)
    {
        return pObject->AsSWCPlaceable();
    }

    return NULL;
}
// PC 0x0063C550 GOG 0x0063C450
CSWCDoor* CClientExoAppInternal::GetDoorByGameObjectID(unsigned long oidObject)
{
    CGameObject* pObject = NULL;

    if (m_pGameObjectArray->GetGameObject(oidObject, &pObject) ==
            CGameObjectArray::SUCCESS &&
        pObject != NULL &&
        pObject->m_nObjectType == CGameObject::OBJECT_TYPE_DOOR)
    {
        return pObject->AsSWCDoor();
    }

    return NULL;
}
// PC 0x0063C5A0 GOG 0x0063C4A0
CSWCAreaOfEffectObject* CClientExoAppInternal::GetAreaOfEffectByGameObjectID(
    unsigned long oidObject)
{
    CGameObject* pObject = NULL;

    if (m_pGameObjectArray->GetGameObject(oidObject, &pObject) ==
            CGameObjectArray::SUCCESS &&
        pObject != NULL &&
        pObject->m_nObjectType == CGameObject::OBJECT_TYPE_AREAOFEFFECTOBJECT)
    {
        return pObject->AsSWCAreaOfEffectObject();
    }

    return NULL;
}
// PC 0x0063C5F0 GOG 0x0063C4F0
CSWCSoundObject* CClientExoAppInternal::GetSoundObjectByGameObjectID(
    unsigned long oidObject)
{
    CGameObject* pObject = NULL;

    if (m_pGameObjectArray->GetGameObject(oidObject, &pObject) ==
            CGameObjectArray::SUCCESS &&
        pObject != NULL &&
        pObject->m_nObjectType == CGameObject::OBJECT_TYPE_SOUND)
    {
        return pObject->AsSWCSoundObject();
    }

    return NULL;
}
// PC 0x0063C640 GOG 0x0063C540
void CClientExoAppInternal::ResetDriveAcceleration()
{
    if (m_pPlayerControl != NULL)
    {
        m_pPlayerControl->ResetDriveAcceleration();
    }
}
// PC 0x0063C650 GOG 0x0063C550
int CClientExoAppInternal::InitializeInGameGUI()
{
    return m_pGuiInGame->Initialize();
}
// PC 0x0063C660 GOG 0x0063C560
CSWCCreature* CClientExoAppInternal::GetPlayerCreature()
{
    if (m_oidCachedCreature == m_nPlayerCreatureId)
    {
        return m_pCachedCreature;
    }

    CClientExoApp* pClientExoApp = g_pAppManager->GetClientExoApp();
    CSWCCreature* pCreature =
        pClientExoApp->GetCreatureByGameObjectID(m_nPlayerCreatureId);

    m_pCachedCreature = pCreature;
    m_oidCachedCreature = m_nPlayerCreatureId;

    return pCreature;
}
// PC 0x0063C6A0 GOG 0x0063C5A0
int CClientExoAppInternal::ClearCachedCreatures(CSWCCreature* pCreature)
{
    int nCachedCreature = (int)m_pCachedCreature;

    if (m_pCachedCreature == pCreature)
    {
        m_oidCachedCreature = OBJECT_INVALID;
        m_pCachedCreature = NULL;
    }

    return nCachedCreature;
}
// PC 0x0063C6D0 GOG 0x0063C5D0
int CClientExoAppInternal::SetDebugMode(int nDebugMode)
{
    m_nDebugMode = nDebugMode;
    return nDebugMode;
}

#ifndef _XBOX
int CClientExoAppInternal::SetTexturePack(unsigned char nTexturePack, int nUnknown)  // PC 0x0063C6E0 GOG 0x0063C5E0
{
    CExoString sTexture;
    CExoString sTexturePath;
    CExoString sGuiPath;
    CExoString sGui;
    int bLocalTable = 0;
    float fDynMemRatio = 0.0f;
    int nMem = 0;

    if (nTexturePack == m_nTexturePack)
    {
        return 1;
    }

    m_nTexturePack = nTexturePack;
    C2DA* pTexPacks;
    if (g_pRules != NULL && g_pRules->m_p2DArrays != NULL)
    {
        pTexPacks = g_pRules->m_p2DArrays->m_pTexPacks;
    }
    else
    {
        bLocalTable = 1;
        pTexPacks = new C2DA("texpacks", 1);
        if (pTexPacks == NULL)
        {
            return 0;
        }
        if (!pTexPacks->Load2DArray())
        {
            delete pTexPacks;
            return 0;
        }
    }

    if (pTexPacks != NULL)
    {
        if (pTexPacks->GetNumRows() != 0 && nTexturePack < pTexPacks->GetNumRows())
        {
            pTexPacks->GetCExoStringEntry(nTexturePack, "Texture", &sTexture);
            pTexPacks->GetCExoStringEntry(nTexturePack, "Gui", &sGui);
            pTexPacks->GetFLOATEntry(nTexturePack, "DynMemRatio", &fDynMemRatio);
            pTexPacks->GetINTEntry(nTexturePack, "Mem", &nMem);

            AurTextureResetAll();
            CAuroraInterface::SetTexturePack(CAuroraInterface::TEXTURE_PACK_TEXTURE,
                                            CExoString("TEXTUREPACKS:") + sTexture);
            CAuroraInterface::SetTexturePack(CAuroraInterface::TEXTURE_PACK_GUI,
                                            CExoString("TEXTUREPACKS:") + sGui);
            AurTextureSetDynamicMemoryRatio(fDynMemRatio);
            AurTextureSetMem(nMem);

            if (bLocalTable)
            {
                pTexPacks->Unload2DArray();
                delete pTexPacks;
            }
            m_nTexturePack = nTexturePack;
            return 1;
        }
        pTexPacks->Unload2DArray();
        delete pTexPacks;
    }
    return 0;
}
#endif

#ifndef _XBOX
int CClientExoAppInternal::SetVideoMode(unsigned long nVideoMode,  // PC 0x0063CA70 GOG 0x0063C970
                                      unsigned char nModeSelection)
{
    if (nVideoMode == m_nVideoMode)
        return 1;
    DEVMODE cDevMode;
    memset(&cDevMode, 0, sizeof(cDevMode));
    cDevMode.dmSize = sizeof(cDevMode);
    if (!EnumDisplaySettings(NULL, nVideoMode, &cDevMode))
        return 0;

    g_nScreenWidth = cDevMode.dmPelsWidth;
    g_nScreenHeight = cDevMode.dmPelsHeight;
    g_nBitsPerPixel = cDevMode.dmBitsPerPel;

    int bFullScreen;
    if (nModeSelection == VIDEO_MODE_WINDOWED)
        bFullScreen = 0;
    else if (nModeSelection == VIDEO_MODE_FULLSCREEN)
        bFullScreen = 1;
    else
        bFullScreen = g_pAurora->FullScreen();

    int nWidth = g_nScreenWidth;
    int nHeight = g_nScreenHeight;
    if (g_nReInitializing == 1)
        g_pAurora->RestoreDesktopDisplayMode();
    g_nScreenWidth = nWidth;
    g_nScreenHeight = nHeight;

    RECT cDesktopRect;
    GetWindowRect(GetDesktopWindow(), &cDesktopRect);
    if (!g_bAllowWindowedMode)
        bFullScreen = 1;
    if (!bFullScreen &&
        (cDesktopRect.bottom <= VIDEO_WINDOW_HEIGHT_LIMIT ||
         cDesktopRect.right <= VIDEO_WINDOW_WIDTH_LIMIT))
        ReInitAurora(g_nScreenWidth, g_nScreenHeight,
                     g_nBitsPerPixel, 1, 1);
    else if (!bFullScreen &&
             (cDesktopRect.bottom <= g_nScreenHeight ||
              cDesktopRect.right <= g_nScreenWidth))
        ReInitAurora(g_nScreenWidth, g_nScreenHeight,
                     g_nBitsPerPixel, 1, 1);
    else
        ReInitAurora(g_nScreenWidth, g_nScreenHeight,
                     g_nBitsPerPixel, bFullScreen, 1);

    m_nVideoMode = nVideoMode;
    if (m_pSWGuiManager)
    {
        m_pSWGuiManager->SetSize(g_nScreenWidth, g_nScreenHeight);
        if (m_pMainMenu && m_pSWGuiManager->PanelExists(m_pMainMenu))
        {
            CSWGuiManager* pManager = m_pSWGuiManager;
            m_pMainMenu->LoadFromLayout(pManager);
        }
        if (m_pGuiLoadScreen)
        {
            delete m_pGuiLoadScreen;
            m_pGuiLoadScreen = new CSWGuiLoadScreen(m_pSWGuiManager);
        }
        if (m_pGuiInGame)
            m_pGuiInGame->ResetInterfaceForSize();
    }
    return 1;
}
#endif
// PC 0x0063CCD0 GOG 0x0063CBD0
void CClientExoAppInternal::SetCharacterFile(unsigned long nDataSize,
                                             unsigned char* pCharacterData,
                                             int nCharacterFileId)
{
    if (m_pCharacterFile != NULL)
    {
        delete [] m_pCharacterFile;
        m_pCharacterFile = NULL;
    }

    if (pCharacterData != NULL)
    {
        m_pCharacterFile = new unsigned char[nDataSize + 1];
        memcpy(m_pCharacterFile, pCharacterData, nDataSize);
        m_pCharacterFile[nDataSize] = 0;
    }

    m_nCharacterFileSize = nDataSize;
    m_nCharacterFileId = nCharacterFileId;
}
// PC 0x0063CD60 GOG 0x0063CC60
void CClientExoAppInternal::FadeAndStopStreamingSoundSource(
    CExoStreamingSoundSource* pSource, unsigned long nFadeTime)
{
    unsigned long nStart =
        (unsigned long)(g_pExoBase->m_pExoTimers->GetHighResolutionTimer() /
                        1000);
    unsigned long nElapsed;

    pSource->FadeAndStop(nFadeTime);

    if (nFadeTime > 0)
    {
        do
        {
            g_pExoSound->Render();
            nElapsed = (unsigned long)(g_pExoBase->m_pExoTimers
                                           ->GetHighResolutionTimer() /
                                       1000) -
                       nStart;
        } while (nElapsed < nFadeTime);
    }
}
// PC 0x0063CDD0 GOG 0x0063CCD0
void CClientExoAppInternal::StopLoadMusic()
{
    if (m_pLoadMusic != NULL)
    {
        m_pLoadMusic->Stop();
        delete m_pLoadMusic;
        m_pLoadMusic = NULL;
    }
}
// PC 0x0063CE10 GOG 0x0063CD10
CExoStreamingSoundSource* CClientExoAppInternal::AquireLoadMusic(const CResRef& cResRef)
{
    CExoStreamingSoundSource* pLoadMusic = NULL;
    if (m_pLoadMusic != NULL)
    {
        CExoString sLoadMusic;
        cResRef.CopyToString(sLoadMusic);
        if (sLoadMusic.CompareNoCase(m_sLoadMusic))
        {
            pLoadMusic = m_pLoadMusic;
            m_pLoadMusic = NULL;
        }
    }
    return pLoadMusic;
}

// PC 0x0063CEA0 GOG 0x0063CDA0
void CClientExoAppInternal::IndicateObject(CSWCObject* pObject, int nFlags)
{
    m_pGuiInGame->SetMainInterfaceTarget(
        pObject != NULL ? pObject->m_nId : OBJECT_INVALID, nFlags);
}

#ifndef _XBOX
void CClientExoAppInternal::HandleExitGameButton(CSWGuiControl* pControl)  // PC 0x0063CED0 GOG 0x0063CDD0
{
    if (g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox->m_bAccepted)
    {
        m_bExitGame = 1;
        m_bExitGamePending = 1;
        PostQuitMessage(0);
    }
}
#endif

#ifndef _XBOX
// PC 0x0063CF20 GOG 0x0063CE20
void CClientExoAppInternal::InitializeClientOptions()
{
    CExoIni cIni;
    CExoString sValue;

    if (cIni.ReadIniEntry(sValue, ".\\swkotor2.ini", "Game Options",
                         "TooltipDelay Sec"))
    {
        if (sValue.AsINT() <= 0)
        {
            sValue.Format("1");
            cIni.WriteIniEntry(sValue, ".\\swkotor2.ini", "Game Options",
                               "TooltipDelay Sec");
        }
    }
    else
    {
        cIni.WriteIniEntry(sValue, ".\\swkotor2.ini", "Game Options",
                           "TooltipDelay Sec");
    }

    if (cIni.ReadIniEntry(sValue, ".\\swkotor2.ini", "Graphics Options",
                         "EnableHardwareMouse"))
    {
        m_pClientOptions->SetEnableHardwareMouseCursor(sValue.AsINT() != 0);
    }
    else
    {
        m_pClientOptions->SetEnableHardwareMouseCursor(1);
        sValue.Format("%d", 1);
        cIni.WriteIniEntry(sValue, ".\\swkotor2.ini", "Graphics Options",
                           "EnableHardwareMouse");
    }
}
#endif

#ifndef _XBOX
#define DEFAULT_SOUND_VOICES 64
void CClientExoAppInternal::InitializeSoundOptions()  // PC 0x0063D280 GOG 0x0063D180
{
    CExoIni cIni;
    CExoString sSound;
    CExoString sValue;
    int nEAXMode = CExoSound::EAX_MODE_DEFAULT;
    int bForceSoftware = 0;
    int bSoundInit = 0;

    if (cIni.ReadIniEntry(sValue, ".\\swkotor2.ini", "Sound Options", "Sound Init"))
    {
        bSoundInit = sValue.AsINT();
    }
    sValue.Format("%d", 1);
    cIni.WriteIniEntry(sValue, ".\\swkotor2.ini", "Sound Options", "Sound Init");

    unsigned char n2DVoices = DEFAULT_SOUND_VOICES;
    unsigned char n3DVoices = DEFAULT_SOUND_VOICES;
    if (cIni.ReadIniEntry(sValue, ".\\swkotor2.ini", "Sound Options", "Number 2D Voices"))
    {
        n2DVoices = sValue.AsINT();
    }
    if (cIni.ReadIniEntry(sValue, ".\\swkotor2.ini", "Sound Options", "Number 3D Voices"))
    {
        n3DVoices = sValue.AsINT();
    }

    if (bSoundInit)
    {
        nEAXMode = CExoSound::EAX_MODE_DISABLED;
        bForceSoftware = 1;
    }
    else
    {
        if (cIni.ReadIniEntry(sValue, ".\\swkotor2.ini", "Sound Options", "EAX"))
        {
            nEAXMode = sValue.AsINT();
        }
        if (cIni.ReadIniEntry(sValue, ".\\swkotor2.ini", "Sound Options", "Force Software"))
        {
            bForceSoftware = sValue.AsINT();
            g_pAppManager->GetClientExoApp()->GetClientOptions()->m_nEAXFlags = bForceSoftware;
        }
    }

    g_pExoSound = new CExoSound(n2DVoices, n3DVoices, nEAXMode, !bForceSoftware);

    if (cIni.ReadIniEntry(sValue, ".\\swkotor2.ini", "Sound Options", "Environment Effects Nonstreaming"))
    {
        g_pExoSound->EnableNonStreamingEnvironmentEffects(sValue.AsINT());
    }
    if (cIni.ReadIniEntry(sValue, ".\\swkotor2.ini", "Sound Options", "Environment Effects Streaming"))
    {
        g_pExoSound->EnableStreamingEnvironmentEffects(sValue.AsINT());
    }
    if (cIni.ReadIniEntry(sValue, ".\\swkotor2.ini", "Sound Options", "2D3D Bias"))
    {
        g_pExoSound->Set2D3DBias(sValue.AsFLOAT());
    }

    sValue.Format("%d", g_pExoSound->GetEAX());
    cIni.WriteIniEntry(sValue, ".\\swkotor2.ini", "Sound Options", "EAX");
    g_pAppManager->GetClientExoApp()->GetClientOptions()->SetEAXMode((unsigned char)g_pExoSound->GetEAX());

    sValue.Format("%d", bForceSoftware);
    cIni.WriteIniEntry(sValue, ".\\swkotor2.ini", "Sound Options", "Force Software");
    sValue.Format("%.2f", g_pExoSound->Get2D3DBias());
    cIni.WriteIniEntry(sValue, ".\\swkotor2.ini", "Sound Options", "2D3D Bias");
    sValue.Format("%d", g_pExoSound->GetNumber2DVoices());
    cIni.WriteIniEntry(sValue, ".\\swkotor2.ini", "Sound Options", "Number 2D Voices");
    sValue.Format("%d", g_pExoSound->GetNumber3DVoices());
    cIni.WriteIniEntry(sValue, ".\\swkotor2.ini", "Sound Options", "Number 3D Voices");
    sValue.Format("%d", 0);
    cIni.WriteIniEntry(sValue, ".\\swkotor2.ini", "Sound Options", "Sound Init");
}
#endif
#ifndef _XBOX
void CClientExoAppInternal::TurnOffKeyboardModal()  // PC 0x0063DD00 GOG 0x0063DC00
{
    for (unsigned int i = 0; i < 7; i++)
    {
        if (m_anKeyboardModal[i] != 0)
        {
            memset(m_anKeyboardModal, 0, sizeof(m_anKeyboardModal));
            m_nUnknownFC = 1;
            m_nUnknownF4 = 0;
            m_nUnknownF8 = 0;
            m_pSWCMessage->SendPlayerToServerInput_AbortDriveControl();
            break;
        }
    }

    if (m_pModule != NULL && m_pModule->m_pInfo != NULL)
    {
        m_pModule->m_pInfo->HiliteAreaObjects(0);
    }
}
#endif
void CClientExoAppInternal::ShutDownLoadGameLoadBar()  // PC 0x0063DD70 GOG 0x0063DC70
{
    if (m_nLoadBarState != 0)
    {
        g_pAppManager->m_pReentrantServerStats->m_bOperationEndPending = 0;

        CSWReentrantServerStats* pStats =
            g_pAppManager->m_pReentrantServerStats;
        pStats->m_bOperationActive = 0;
        pStats->m_nOperation = 0;
        m_nLoadBarState = 0;
    }
}
// PC 0x0063DDA0 GOG 0x0063DCA0
void CClientExoAppInternal::TogglePartyFollow()
{
    CExoString sSoloModeScript("k_sup_solo");

    int bSoloMode =
        g_pAppManager->m_pServerExoApp->GetPartyTable()->m_bSoloMode;
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    pServer->GetPartyTable()->SetSoloMode(!bSoloMode, 1);
    g_pVirtualMachine->RunScript(&sSoloModeScript, OBJECT_INVALID, 0);
}
// PC 0x0063DE40 GOG 0x0063DD40
int CClientExoAppInternal::ServerToClientObjectId(unsigned long oidServerObject)
{
    if (oidServerObject != OBJECT_INVALID)
    {
        return oidServerObject | 0x80000000;
    }

    return OBJECT_INVALID;
}
// PC 0x0063DE60 GOG 0x0063DD60
int CClientExoAppInternal::GetLoadScreenVisible()
{
    if (m_pGuiLoadScreen != NULL && m_pSWGuiManager != NULL)
    {
        return m_pSWGuiManager->PanelExists(m_pGuiLoadScreen);
    }

    return 0;
}
// PC 0x0063DE80 GOG 0x0063DD80
CExoString& CClientExoAppInternal::SetPlayerCharacterName(
    const CExoString& sName)
{
    CExoString& sPlayerCharacterName = m_sPlayerCharacterName = sName;
    m_bPlayerCharacterNameSet = 1;

    return sPlayerCharacterName;
}
// PC 0x0063DEB0 GOG 0x0063DDB0
CExoString& CClientExoAppInternal::GetPlayerCharacterName()
{
    m_bPlayerCharacterNameSet = 0;

    return m_sPlayerCharacterName;
}
// PC 0x0063DED0 GOG 0x0063DDD0
int CClientExoAppInternal::IsPlayerCreature(CSWCCreature* pCreature)
{
    if (pCreature == NULL)
    {
        return 0;
    }

    unsigned long oidPlayer = m_nPlayerCreatureId;

    if (oidPlayer != OBJECT_INVALID)
    {
        return pCreature->m_nId == oidPlayer;
    }

    unsigned long oidServer =
        g_pAppManager->m_pServerExoApp->GetPlayerCreatureId();
    CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
    return pCreature->m_nId == pClientExoApp->ServerToClientObjectId(oidServer);
}
// PC 0x0063DF30 GOG 0x0063DE30
int CClientExoAppInternal::SetPlayerCreature(unsigned long oidCreature)
{
    m_nPlayerCreatureId = oidCreature;

    if (m_pSWParty != NULL)
    {
        return m_pSWParty->AddCharacter(oidCreature);
    }

    return oidCreature;
}
// PC 0x0063DF50 GOG 0x0063DE50
int CClientExoAppInternal::IsMoviePlaying()
{
    if (m_pMoviePlayer == NULL)
    {
        return 0;
    }

    return m_pMoviePlayer->IsMoviePlaying();
}
// PC 0x0063DF70 GOG 0x0063DE70
void CClientExoAppInternal::InitializeMoviePlayer()
{
    m_pMoviePlayer = NULL;
    m_pMoviePlayer = new CExoMoviePlayer();
#ifdef _XBOX
    if (g_pExoSound != NULL)
    {
        m_pMoviePlayer->Initialize((void*)g_pExoSound->GetEAX());
    }
#else
    m_pMoviePlayer->Initialize(NULL);
#endif
}
// PC 0x0063DFE0 GOG 0x0063DEE0
void CClientExoAppInternal::BarkString(unsigned long oidObject,
                                      const CExoString& sText,
                                      const CResRef& cSound, int nFlags)
{
    CSWCObject* pObject =
        g_pAppManager->m_pClientExoApp->GetGameObject(oidObject);

    if (pObject != NULL)
    {
        pObject->AsSWCObject()->BarkString(oidObject, sText, cSound,
                                                  nFlags);
        return;
    }

    m_pGuiInGame->ShowBarkBubble(OBJECT_INVALID, sText, cSound, nFlags);
}
inline CExoStreamingSoundSource* ClientExoAppInternalGetDialogSound(CGuiInGame* pGuiInGame)
{
    return pGuiInGame->m_pDialogSound;
}
// PC 0x0063E050 GOG 0x0063DF50
int CClientExoAppInternal::IsSoundPlayingInDialog()
{
    int bPlaying = 0;

    if (m_pGuiInGame->m_bDialogSoundQueried != 0)
    {
        CGameObject* pGameObject = GetGameObject(m_pGuiInGame->m_oidDialogSoundObject);

        if (pGameObject != NULL && pGameObject->AsSWCObject() != NULL &&
            m_pGuiInGame->m_pDialogSound != NULL)
        {
            if (g_bVODebugEnabled)
            {
                char szSound[17];
                CExoStreamingSoundSource* pDialogSound =
                    m_pGuiInGame->m_pDialogSound;
                pDialogSound->GetFilePlaying().CopyToString(szSound);
                EXOLOG("IsSoundPlayingInDialog: %s mode %d\n", szSound,
                       g_pExoSound->GetSoundMode());
            }

            bPlaying = m_pGuiInGame->m_pDialogSound->IsPlaying();
        }
    }
    else
    {
        if (g_bVODebugEnabled)
        {
            char szSound[17];
            if (m_pGuiInGame->m_pDialogSound != NULL)
            {
                ClientExoAppInternalGetDialogSound(m_pGuiInGame)->GetFilePlaying().CopyToString(szSound);
            }
            EXOLOG("IsSoundPlayingInDialog: %s mode %d\n", szSound,
                   g_pExoSound->GetSoundMode());
        }

        m_pGuiInGame->m_bDialogSoundQueried = 1;
        bPlaying = 1;
    }

    return bPlaying;
}
// PC 0x0063E150 GOG 0x0063DFC0
void CClientExoAppInternal::StopSoundPlayingInDialog()
{
    CGameObject* pGameObject =
        GetGameObject(m_pGuiInGame->m_oidDialogSoundObject);

    if (pGameObject == NULL)
    {
        return;
    }

    CSWCObject* pObject = pGameObject->AsSWCObject();

    if (pObject == NULL)
    {
        return;
    }

    if (pObject->AsSWCCreature() != NULL)
    {
        pObject->AsSWCCreature()->StopLipSync();
    }

    if (m_pGuiInGame->m_pDialogSound != NULL)
    {
        m_pGuiInGame->m_pDialogSound->Stop();
    }
}
// PC 0x0063E1C0 GOG 0x0063E030
int CClientExoAppInternal::GetPausedByCombat()
{
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;

    if (pServerExoApp != NULL)
    {
        return pServerExoApp->GetPauseState(PAUSE_STATE_COMBAT);
    }

    return 0;
}
// PC 0x0063E1E0 GOG 0x0063E050
void CClientExoAppInternal::SetPausedByCombat(int bPaused,
                                            unsigned char nPauseType,
                                            int bForce)
{
    if ((m_pGuiInGame->m_nGlobalDialogState == 1 && bPaused == 0) ||
        (m_pGuiInGame->m_nGlobalDialogState == 0 || bForce == 1))
    {
        int bCurrentlyPaused = GetPausedByCombat();
        if (!m_PauseFlags.m_Bits.m_bTogglePauseAtEndOfFrame &&
            bCurrentlyPaused != bPaused)
        {
            m_PauseFlags.m_Bits.m_bTogglePauseAtEndOfFrame = 1;
            m_nTogglePauseType = nPauseType;
#ifndef _XBOX
            m_bTogglePause = bPaused;
#endif
            if (nPauseType == PAUSE_REASON_CONTAINER)
                return;
            if (bPaused == 1)
            {
                g_pExoInput->PauseRumble();
                g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_PAUSE);
#ifndef _XBOX
                m_nSoundPausedByCombat = 1;
#endif
            }
            else
            {
                g_pExoInput->UnpauseRumble();
                g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_GENERIC);
#ifndef _XBOX
                m_nSoundPausedByCombat = 0;
#endif
            }
        }
    }
}
// PC 0x0063E2B0 GOG 0x0063E120
int CClientExoAppInternal::GetControlledCharacterDead()
{
    CSWCCreature* pCharacter =
        g_pAppManager->m_pClientExoApp->GetSWParty()->GetCharacter(0);
    CSWSCreature* pServerCreature;

    if (pCharacter != NULL &&
        (pServerCreature = pCharacter->GetServerCreature()) != NULL)
    {
        return pServerCreature->GetDeadTemp();
    }

    return 0;
}
// PC 0x0063E300 GOG 0x0063E170
int CClientExoAppInternal::SetGameOver(int bGameOver)
{
    m_nGameOver = bGameOver;
    m_bGameOverPending = 1;

    return bGameOver;
}

#ifndef _XBOX
void CClientExoAppInternal::UpdateDeathSequence()  // PC 0x0063E320 GOG 0x0063E190
{
    unsigned long nTime = (unsigned long)(
        g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000);
    unsigned long nElapsed = nTime - m_nDeathSequenceLastTime;
    m_nDeathSequenceLastTime = nTime;

    if (m_nGameOver)
    {
        m_fDeathSequenceElapsed += nElapsed * 0.001f;
        if (m_bGameOverPending)
        {
            m_bGameOverPending = 0;
            m_fDeathSequenceScale =
                (float)(-log(m_fDeathSequenceElapsed /
                             CLIENT_DEATH_SEQUENCE_DURATION) /
                        (1.0 - CLIENT_DEATH_SEQUENCE_MIN_SPEED));
        }
    }
}
#endif
// PC 0x0063E3C0 GOG 0x0063E230
void CClientExoAppInternal::HurryDeathSequence()
{
    Vector vZero;

    m_pGuiInGame->StartGlobalFade(0, 0.0f, 0.5f, vZero);
}
// PC 0x0063E400 GOG 0x0063E270
void CClientExoAppInternal::SetGameSpeed(float fSpeed)
{
    if (g_pAppManager != NULL)
    {
        if (g_pAppManager->m_pClientExoApp != NULL)
        {
            g_pAppManager->m_pClientExoApp->GetWorldTimer()->SetSnapshotScale(
                fSpeed);
            g_pAppManager->m_pClientExoApp->GetPauseTimer()->SetSnapshotScale(
                fSpeed);
            g_pAppManager->m_pClientExoApp->GetAnimationTimer()
                ->SetSnapshotScale(fSpeed);
        }

        if (g_pAppManager->GetServerExoApp() != NULL)
        {
            g_pAppManager->GetServerExoApp()->GetWorldTimer()->SetSnapshotScale(
                fSpeed);
            g_pAppManager->GetServerExoApp()->GetPauseTimer()->SetSnapshotScale(
                fSpeed);
        }
    }
}
// PC 0x0063E480 GOG 0x0063E2F0
void CClientExoAppInternal::SetLastPartyMemberTempKilled(
    unsigned long oidObject)
{
    if (oidObject != OBJECT_INVALID)
    {
        CSWSCreature* pCreature =
            g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(
                oidObject);

        if (pCreature != NULL && pCreature->m_bInParty != 0)
        {
            m_oidLastPartyMemberTempKilled =
                g_pAppManager->m_pClientExoApp->ServerToClientObjectId(
                    oidObject);
        }
    }
}
// PC 0x0063E4D0 GOG 0x0063E340
void CClientExoAppInternal::SetCanSendDialog(int bCanSend)
{
    m_bCanSendDialog = bCanSend;
    m_nCanSendDialogTime =
        (int)((__int64)g_pAppManager->m_pClientExoApp->m_pcExoAppInternal
                  ->m_pPauseTimer->GetSnapshotTime() /
              1000);
}
// PC 0x0063E510 GOG 0x0063E380
int CClientExoAppInternal::GetCanSendDialog(
    unsigned long oidSpeaker, unsigned long oidListener, CSWSDialog* pDialog)
{
    CGameObject* pObject =
        g_pAppManager->GetServerExoApp()->GetGameObject(oidListener);
    if (pObject != NULL && pObject->AsSWSCreature() != NULL &&
        g_pAppManager->GetClientExoApp()->GetInGameGui()->m_n102C == 0)
    {
        pObject->AsSWSCreature()->SetActivity(CSWSCreature::ACTIVITY_STEALTH, 0, 1);
    }
    pObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidSpeaker);
    if (pObject != NULL && pObject->AsSWSCreature() != NULL &&
        g_pAppManager->GetClientExoApp()->GetInGameGui()->m_n102C == 0)
    {
        pObject->AsSWSCreature()->SetActivity(CSWSCreature::ACTIVITY_STEALTH, 0, 1);
    }

#ifdef _XBOX
    if (GetPausedByCombat() || m_PauseFlags.m_Bits.m_bAutoPaused)
#else
    if (GetPausedByCombat() || m_AutoPauseFlags.m_Bits.m_bAutoPaused)
#endif
        return 0;
    if (m_bDialogSent)
        return 1;
    if (!m_bCanSendDialog)
        return 0;
    if (!m_nCanSendDialogTime)
        return 0;

    CWorldTimer* pPauseTimer = g_pAppManager->GetClientExoApp()
        ->m_pcExoAppInternal->m_pPauseTimer;
    unsigned long nTime = (unsigned long)((__int64)
        pPauseTimer->GetSnapshotTime() / WORLD_TIMER_SNAPSHOT_TICKS_PER_MILLISECOND);
    if (nTime - m_nCanSendDialogTime <= CLIENT_DIALOG_SEND_WAIT_INTERVAL)
        return 0;
    m_nCanSendDialogTime = nTime - CLIENT_DIALOG_SEND_RETRY_BACKDATE;

    if (oidSpeaker != OBJECT_INVALID)
    {
        unsigned long oidClient =
            g_pAppManager->GetClientExoApp()->ServerToClientObjectId(oidSpeaker);
        CSWCObject* pClientObject =
            g_pAppManager->GetClientExoApp()->GetGameObject(oidClient);
        if (pClientObject == NULL || pClientObject->AsSWCObject() == NULL)
            return 0;
    }
    if (oidListener != OBJECT_INVALID)
    {
        unsigned long oidClient =
            g_pAppManager->GetClientExoApp()->ServerToClientObjectId(oidListener);
        CSWCObject* pClientObject =
            g_pAppManager->GetClientExoApp()->GetGameObject(oidClient);
        if (pClientObject == NULL || pClientObject->AsSWCObject() == NULL)
            return 0;
    }

    if (!m_bDialogModelsSent)
    {
        int bNonDependant;
        for (int i = 0; i < pDialog->m_nStunts; ++i)
        {
            unsigned long oidModel = pDialog->GetDialogModel(i, bNonDependant);
            if (!bNonDependant)
            {
                CSWCObject* pClientObject =
                    g_pAppManager->GetClientExoApp()->GetGameObject(oidModel);
                if (pClientObject == NULL || pClientObject->AsSWCObject() == NULL)
                    return 0;
            }
        }
        unsigned long oidPlayer =
            g_pAppManager->GetServerExoApp()->GetPlayerCreatureId();
        CSWSPlayer* pPlayer =
            g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(oidPlayer);
        unsigned long oidOwner = pDialog->m_oidDialogOwner;
        CGameObject* pOwner =
            g_pAppManager->GetServerExoApp()->GetGameObject(oidOwner);
        if (pOwner == NULL || pOwner->AsSWSObject() == NULL)
            return 0;
        CSWSObject* pDialogOwner = pOwner->AsSWSObject();
        pDialog->SendDialogModels(pDialogOwner, pPlayer->GetPlayerId());
        if (!m_bDialogModelsSent)
            return 0;
    }
    m_bDialogSent = 1;
    return 1;
}
// PC 0x0063E7B0 GOG 0x0063E620
int CClientExoAppInternal::GetClientMeleeAnimation(
    unsigned long oidAttacker, unsigned long oidTarget,
    unsigned short nAnimation, int nAttack, int bOverride,
    int nOverrideAnimation)
{
    CSWCCreature* pAttacker = GetCreatureByGameObjectID(
        ServerToClientObjectId(oidAttacker));
    CSWCCreature* pTarget = GetCreatureByGameObjectID(
        ServerToClientObjectId(oidTarget));

    int nResult = 0;
    if (pAttacker != NULL)
    {
        if (bOverride != 0)
        {
            nResult = nOverrideAnimation;
            pAttacker->m_nAttackAnimationOverride = nOverrideAnimation;
        }
        else
        {
            unsigned char nWieldClass =
                pAttacker->GetAnimationBase()->m_nWieldClass;
            int bInCombat = pAttacker->InCreatureCombat(pTarget);
            pAttacker->ResolveAttackIndex(nAnimation, bInCombat, -1);
            return pAttacker->UpdateMeleeAttackData(
                nAnimation, nAttack, bInCombat, nWieldClass);
        }
    }
    return nResult;
}
// PC 0x0063E8C0 GOG 0x0063E730
int CClientExoAppInternal::GetClientRangedAnimation(unsigned long nA, int nB,
                                                    int nC, int nD)
{
    unsigned long oidObject;
    if (nA != OBJECT_INVALID)
    {
        oidObject = nA | 0x80000000;
    }
    else
    {
        oidObject = OBJECT_INVALID;
    }

    CGameObject* pObject = NULL;
    CSWCCreature* pCreature;

    if (m_pGameObjectArray->GetGameObject(oidObject, &pObject) ==
            CGameObjectArray::SUCCESS &&
        pObject != NULL &&
        pObject->m_nObjectType == CGameObject::OBJECT_TYPE_CREATURE)
    {
        pCreature = pObject->AsSWCCreature();
    }
    else
    {
        pCreature = NULL;
    }

    int nResult = 0;
    if (pCreature != NULL)
    {
        if (nC != 0)
        {
            nResult = nD;
            pCreature->m_nAttackAnimationOverride = nD;
        }
        else
        {
            CSWCAnimBase* pAnimBase = pCreature->GetAnimationBase();
            return pCreature->UpdateRangedAttackData(nB,
                                                     pAnimBase->m_nWieldClass);
        }
    }
    return nResult;
}
// PC 0x0063E950 GOG 0x0063E7C0
void CClientExoAppInternal::SetLoadScreenByModuleName(const CExoString& sModule)
{
    CExoString sBMPResRef;
    if (sModule.GetLength() == 0)
    {
        m_sLoadScreenImage = "";
        m_bShowLoadScreen = 1;
        return;
    }

    bool bMissingImage;
    {
        CExoString sColumn("BMPResRef");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pLoadscreens = p2DArrays->m_pAreaTransition;
        bMissingImage = pLoadscreens->GetCExoStringEntry(
            sModule, sColumn, &sBMPResRef) == 0;
    }
    if (bMissingImage)
    {
        sBMPResRef.Format("load_%s", sModule);
        int nResourceExists;
        nResourceExists = g_pExoResMan->Exists(sBMPResRef, RESTYPE_TGA, (unsigned long*)NULL);
        if (!nResourceExists)
        {
#ifdef _XBOX
            nResourceExists = g_pExoResMan->Exists(sBMPResRef, RESTYPE_TPC, (unsigned long*)NULL);
#else
            CResRef cImage(sBMPResRef);
            CExoResMan* pResMan = g_pExoResMan;
            nResourceExists = pResMan->Exists(cImage, RESTYPE_TPC, (unsigned long*)NULL);
#endif
        }
        if (!nResourceExists)
        {
            CExoString sColumn("BMPResRef");
            CExoString sDefault("DEFAULT");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pLoadscreens = p2DArrays->m_pAreaTransition;
            pLoadscreens->GetCExoStringEntry(
                sDefault, sColumn, &sBMPResRef);
        }
    }
    if (!m_sLoadScreenImage.CompareNoCase(sBMPResRef))
    {
        m_sLoadScreenImage = sBMPResRef;
        m_bShowLoadScreen = 1;
    }
}
#ifndef _XBOX
// PC 0x0063EB30 GOG 0x0063E9A0
CExoString CClientExoAppInternal::GetLoadMusicByModuleName(const CExoString& sModule)
{
    CExoString sMusicResRef;
    if (sModule.GetLength() == 0)
    {
        sMusicResRef = "";
        return sMusicResRef;
    }

    bool bMissingMusic;
    {
        CExoString sColumn("MusicResRef");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pLoadscreens = p2DArrays->m_pAreaTransition;
        bMissingMusic = pLoadscreens->GetCExoStringEntry(
            sModule, sColumn, &sMusicResRef) == 0;
    }
    if (bMissingMusic)
    {
        sMusicResRef.Format("load_%s", sModule);
        int nResourceExists;
        nResourceExists = g_pExoResMan->Exists(sMusicResRef, RESTYPE_MP3, (unsigned long*)NULL);
        if (!nResourceExists)
        {
            CExoString sColumn("MusicResRef");
            CExoString sDefault("DEFAULT");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            C2DA* pLoadscreens = p2DArrays->m_pAreaTransition;
            pLoadscreens->GetCExoStringEntry(
                sDefault, sColumn, &sMusicResRef);
        }
    }
    return sMusicResRef;
}
#endif
void CClientExoAppInternal::SetLoadScreenHint(unsigned long nHint)  // PC 0x0063ECE0 GOG 0x0063EB50
{
    if (m_pGuiLoadScreen != NULL)
    {
        m_pGuiLoadScreen->SetHint(nHint);
    }
}

unsigned long CClientExoAppInternal::GetLoadScreenHint()
{
    if (m_pGuiLoadScreen != NULL)
    {
        return m_pGuiLoadScreen->m_nHintStrRef;
    }
    return 0;
}
// PC 0x0063ED00 GOG 0x0063EB70
void CClientExoAppInternal::StartPazaakGame(int nSideDeck, const CExoString& sEndScript,
                                         int nWager, int bTutorial, unsigned long oidOpponent)
{
    if (m_PazaakState.m_bIsRunning)
        return;

    m_PazaakState.m_bIsRunning = 1;
    CSWCCreature* pPlayer = g_pAppManager->m_pClientExoApp->GetSWParty()->GetPlayerCharacter();
    unsigned long oidPlayer = pPlayer->m_nId;
    oidPlayer = g_pAppManager->m_pServerExoApp->ClientToServerObjectId(oidPlayer);
    CSWSCreature* pCreature = g_pAppManager->m_pServerExoApp->GetCreatureByGameObjectID(oidPlayer);
    if (pCreature != 0 && nWager > 0)
    {
        int nGold = pCreature->GetGold();
        nWager = nWager < nGold ? nWager : nGold;
    }
    m_PazaakState.m_nResult = 0;
    m_PazaakState.m_nResultUnused = nWager;
    m_PazaakState.m_sEndScript = sEndScript;
    m_PazaakState.m_nSideDeck = nSideDeck;
    g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_PAUSE_ALL_BUT_MUSIC);

    CSWGuiPazaakStart* pPanel = new CSWGuiPazaakStart(m_pSWGuiManager, nWager, nSideDeck,
                                                   bTutorial, oidOpponent);
    SetInputClass(INPUT_CLASS_GUI, 1);
    m_pSWGuiManager->AddPanel(pPanel, CSWGuiManager::PANEL_MODE_MODAL, 1);
    m_pSWGuiManager->PlayGuiSound(CSWGuiManager::GUISOUND_PAZAAK_START);
}
// PC 0x0063EE40 GOG 0x0063ECB0
void CClientExoAppInternal::EndPazaakGame(int nResult, int nWager,
                                        int bPlayAgain, unsigned long oidOpponent)
{
    m_PazaakState.m_nResult = nResult;
    m_PazaakState.m_nResultUnused = nWager;
    SetInputClass(INPUT_CLASS_GAME, 1);
    m_PazaakState.m_bIsRunning = 0;

    CSWCCreature* pPlayer = g_pAppManager->m_pClientExoApp->GetSWParty()->GetPlayerCharacter();
    unsigned long oidPlayer = pPlayer->m_nId;
    oidPlayer = g_pAppManager->m_pServerExoApp->ClientToServerObjectId(oidPlayer);
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    CSWSCreature* pCreature = pServer->GetCreatureByGameObjectID(oidPlayer);
    if (pCreature != 0 && nWager > 0)
    {
        int nOldGold = pCreature->GetGold();
        if (nResult == 0)
        {
            nWager = nOldGold - nWager > 0 ? nOldGold - nWager : 0;
        }
        else
        {
            nWager = nOldGold + nWager;
        }
        if (nWager != nOldGold)
        {
            pCreature->SetGold(nWager);
            g_pAppManager->m_pClientExoApp->GetInGameGui()->UpdateStatus(GUI_STATUS_CREDITS, nWager - nOldGold);
        }
    }

    if (bPlayAgain)
    {
        StartPazaakGame(m_PazaakState.m_nSideDeck, m_PazaakState.m_sEndScript, 0, 1, oidOpponent);
        return;
    }
    if (g_pVirtualMachine != 0 && m_PazaakState.m_sEndScript[0] != 0)
        g_pVirtualMachine->RunScript(&m_PazaakState.m_sEndScript, OBJECT_INVALID, 0);
    m_pSWGuiManager->PlayGuiSound(CSWGuiManager::GUISOUND_PAZAAK_END);
    g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_GENERIC);
}
// PC 0x0063EF70 GOG 0x0063EDE0
void CClientExoAppInternal::SetCombatMode(int nMode)
{
    if (nMode == 0)
    {
        m_nAttackMashTutorialTimer = 0;
    }

    if (m_nCombatMode != nMode)
    {
        m_nCombatMode = nMode;
        m_pGuiInGame->RePopulateMainInterface();

        if (m_pModule != NULL)
        {
            if (m_nCombatMode != 0)
            {
                m_pModule->SetCombatCamera();
            }
            else
            {
                m_pModule->RestoreCamera();
            }
        }
    }
}
// PC 0x0063EFC0 GOG 0x0063EE30
void CClientExoAppInternal::UpdateCombatMode()
{
    CSWCObject* pTargetObject = NULL;
    CServerExoApp* pServer = g_pAppManager->GetServerExoApp();
    for (int i = 0; i < m_pSWParty->GetNumCharacters(); i++)
    {
        CSWCCreature* pCharacter = m_pSWParty->GetCharacter(i);
        if (pCharacter == NULL)
            continue;

        if (pCharacter->m_bCombatMode)
        {
            EXOXBOXASSERT(i >= 0 && i < 3);
            CSWCCreature* pTarget = GetCreatureByGameObjectID(
                m_pSWParty->m_aPartyMemberData[i].m_oidCombatTarget);
            if (pTarget == NULL)
            {
                CGameObject* pObject = GetGameObject(
                    m_pSWParty->m_aPartyMemberData[i].m_oidCombatTarget);
                if (pObject != NULL)
                    pTargetObject = pObject->AsSWCObject();
            }
            if (i == 0 && m_fTargetChanging > 0.0f)
                continue;
            if (pTarget != NULL && pTarget->IsHostile())
                continue;
            if (pTargetObject != NULL)
            {
                if (pTargetObject->AsSWCDoor() != NULL &&
                    pTargetObject->AsSWCDoor()->GetServerDoor() != NULL &&
                    pTargetObject->AsSWCDoor()->GetServerDoor()->m_bLocked)
                    continue;
                if (pTargetObject->AsSWCPlaceable() != NULL &&
                    pTargetObject->AsSWCPlaceable()->GetServerPlaceable() != NULL &&
                    pTargetObject->AsSWCPlaceable()->GetServerPlaceable()->m_bLocked)
                    continue;
            }

            CSWSCreature* pServerCreature = pCharacter->GetServerCreature();
            if (pServerCreature != NULL)
            {
                unsigned long oidAttackTarget = pServerCreature->m_oidAttemptedAttackTarget;
                unsigned long oidSpellTarget = pServerCreature->m_oidAttemptedSpellTarget;
                unsigned long oidAttacker = pServerCreature->m_oidGoingToBeAttackedBy;
                if (oidAttackTarget != OBJECT_INVALID)
                {
                    CGameObject* pObject = pServer->GetGameObject(oidAttackTarget);
                    if (pObject != NULL && pObject->AsSWSObject() != NULL &&
                        !pObject->AsSWSObject()->GetDead())
                        continue;
                }
                if (oidSpellTarget != OBJECT_INVALID && oidSpellTarget != oidAttackTarget)
                {
                    CGameObject* pObject = pServer->GetGameObject(oidSpellTarget);
                    if (pObject != NULL && pObject->AsSWSObject() != NULL &&
                        !pObject->AsSWSObject()->GetDead())
                        continue;
                }
                if (oidAttacker != OBJECT_INVALID && oidAttacker != oidAttackTarget &&
                    oidAttacker != oidSpellTarget)
                {
                    CGameObject* pObject = pServer->GetGameObject(oidAttacker);
                    if (pObject != NULL && pObject->AsSWSObject() != NULL &&
                        !pObject->AsSWSObject()->GetDead())
                    {
                        if (pObject->AsSWSCreature() == NULL)
                            continue;
                        CSWCCreature* pClientCreature = pObject->AsSWSCreature()->GetClientCreature();
                        if (pClientCreature == NULL || pClientCreature->IsHostile())
                            continue;
                    }
                }
            }
            m_fCombatAutoPauseTimer = -1.0f;
            pCharacter->SetCombatMode(0);
        }
        else
        {
            CSWSCreature* pServerCreature = pCharacter->GetServerCreature();
            if (pServerCreature != NULL)
            {
                unsigned long oidAttackTarget = pServerCreature->m_oidAttemptedAttackTarget;
                unsigned long oidSpellTarget = pServerCreature->m_oidAttemptedSpellTarget;
                if (oidAttackTarget != OBJECT_INVALID)
                {
                    CGameObject* pObject = pServer->GetGameObject(oidAttackTarget);
                    if (pObject != NULL && pObject->AsSWSObject() != NULL &&
                        !pObject->AsSWSObject()->GetDead())
                    {
                        pCharacter->SetCombatMode(1);
                        continue;
                    }
                }
                if (oidSpellTarget != OBJECT_INVALID && oidSpellTarget != pServerCreature->m_nId &&
                    oidSpellTarget != oidAttackTarget)
                {
                    CGameObject* pObject = pServer->GetGameObject(oidSpellTarget);
                    if (pObject != NULL && pObject->AsSWSObject() != NULL &&
                        !pObject->AsSWSObject()->GetDead())
                        pCharacter->SetCombatMode(1);
                }
            }
        }
    }

    if (GetPlayerCreature() != NULL && GetPlayerCreature()->m_bCombatMode)
    {
        int nIndex = m_pSWParty->GetIndex(m_nPlayerCreatureId);
        if (nIndex >= 0)
        {
            unsigned long nMessage = m_pSWParty->GetMemberData(nIndex)->m_nCombatMessage;
            if (nMessage != CSWGuiMainInterface::STRREF_COMBAT_MODE_ENGAGED)
                m_pGuiInGame->SetCombatMessage(nMessage);
        }
    }
}
// PC 0x0063F410 GOG 0x0063F270
void CClientExoAppInternal::SetAutoPaused(int bAuto, unsigned char nPauseType)
{
#ifdef _XBOX
    if (m_PauseFlags.m_Bits.m_bAutoPaused != bAuto)
#else
    if (m_AutoPauseFlags.m_Bits.m_bAutoPaused != bAuto)
#endif
    {
        if (bAuto)
        {
            if (GetPausedByCombat())
            {
                return;
            }

            if (m_fEnemyAutoPauseDelay > 0.0f)
            {
#ifdef _XBOX
                m_fEnemyAutoPauseTimer = 1.0f;
                m_fEnemyAutoPauseDelay = 0.0f;
#else
                m_fEnemyAutoPauseTimer = 1.0f;
                m_fEnemyAutoPauseDelay = 0.0f;
                m_nDelayedAutoPauseType = nPauseType;
#endif
                return;
            }

            if (m_pGuiInGame->m_nGlobalDialogState != 0)
            {
                return;
            }

            if (nPauseType == 9)
            {
                if (m_fEnemyAutoPauseCooldown > 0.0f)
                {
                    return;
                }
                m_fEnemyAutoPauseCooldown = 2.0f;
            }

#ifdef _XBOX
            m_PauseFlags.m_Bits.m_bAutoPaused = 1;
#else
            m_AutoPauseFlags.m_Bits.m_bAutoPaused = 1;
#endif
            SetPausedByCombat(1, nPauseType, 0);
        }
        else
        {
#ifdef _XBOX
            m_PauseFlags.m_Bits.m_bAutoPaused = 0;
#else
            m_AutoPauseFlags.m_Bits.m_bAutoPaused = 0;
#endif
            if (GetPausedByCombat())
            {
                SetPausedByCombat(0, 0, 0);
            }
        }
    }
}
// PC 0x00643070 GOG 0x00642F20
void CClientExoAppInternal::EnableVideoEffect(int nEffect)
{
    int bReadEntry;
    float fSaturation = 0.0f;
    Vector vModulation;
    int nEnableSaturation = 0;
    int nEnableScanNoise = 0;
    int nEnableClairvoyance = 0;
    int nEnableForceSight = 0;
    int nEnableClairvoyanceFull = 0;
    int nEnableFury = 0;
    CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
    EXOXBOXASSERT(p2DArrays->m_pVideoEffects);
    C2DA* pVideoEffects = p2DArrays->m_pVideoEffects;

    DisableVideoEffect();
    if (pVideoEffects != NULL && nEffect >= 0)
    {
        bReadEntry = pVideoEffects->GetINTEntry(nEffect, "EnableScanNoise",
                                               &nEnableScanNoise);
        if (bReadEntry)
        {
            if (nEnableScanNoise)
            {
                m_nVideoEffect = nEffect;
                AurEnableScanNoise();
            }
        }
        bReadEntry = pVideoEffects->GetINTEntry(nEffect, "EnableSaturation",
                                               &nEnableSaturation);
        if (bReadEntry)
        {
            if (nEnableSaturation)
            {
                m_nVideoEffect = nEffect;
                pVideoEffects->GetFLOATEntry(nEffect, "ModulationRed_PC",
                                             &vModulation.m_fX);
                pVideoEffects->GetFLOATEntry(nEffect, "ModulationGreen_PC",
                                             &vModulation.m_fY);
                pVideoEffects->GetFLOATEntry(nEffect, "ModulationBlue_PC",
                                             &vModulation.m_fZ);
                pVideoEffects->GetFLOATEntry(nEffect, "Saturation_PC",
                                             &fSaturation);
                AurEnableSaturation();
                AurSetSaturationParams(vModulation, fSaturation);
            }
        }
        bReadEntry = pVideoEffects->GetINTEntry(nEffect, "EnableClairvoyance",
                                               &nEnableClairvoyance);
        if (bReadEntry)
        {
            if (nEnableClairvoyance)
            {
                m_nVideoEffect = nEffect;
                AurEnableClairvoyance(true);
            }
        }
        bReadEntry = pVideoEffects->GetINTEntry(nEffect, "EnableForceSight",
                                               &nEnableForceSight);
        if (bReadEntry)
        {
            if (nEnableForceSight)
            {
                m_nVideoEffect = nEffect;
                AurEnableForceSight();
            }
        }
        bReadEntry = pVideoEffects->GetINTEntry(nEffect,
                                               "EnableClairvoyanceFull",
                                               &nEnableClairvoyanceFull);
        if (bReadEntry)
        {
            if (nEnableClairvoyanceFull)
            {
                m_nVideoEffect = nEffect;
                AurEnableClairvoyance(false);
            }
        }
        bReadEntry = pVideoEffects->GetINTEntry(nEffect, "EnableFury",
                                               &nEnableFury);
        if (bReadEntry)
        {
            if (nEnableFury)
            {
                m_nVideoEffect = nEffect;
                AurEnableForceFury(nEnableFury);
            }
        }
    }
}
// PC 0x0063F600 GOG 0x0063F460
void CClientExoAppInternal::DisableVideoEffect()
{
    m_nVideoEffect = -2;
    AurDisableSaturation();
    AurDisableScanNoise();
    AurDisableClairvoyance();
    AurDisableForceSight();
    AurEnableForceFury(0);
}
// PC 0x0063F630 GOG 0x0063F490
void CClientExoAppInternal::DelayNextEnemyAutoPause()
{
    if (m_fEnemyAutoPauseTimer <= 0.0f)
    {
        m_fEnemyAutoPauseDelay = 5.0f;
    }
}
// PC 0x0063F650 GOG 0x0063F4B0
int CClientExoAppInternal::ShowTutorialWindow(unsigned char nTutorial,
                                              unsigned long nX,
                                              unsigned long nY, unsigned long nActionParam)
{
    if (!m_pGuiInGame->TutorialReasonWillShowWindow((unsigned char)nTutorial))
    {
        return 0;
    }

    if (!m_pGuiInGame->SetTutorialReason((unsigned char)nTutorial))
    {
        return 0;
    }

    m_nTutorialX = nX;
    m_nTutorialY = nY;
    m_nTutorialZ = nActionParam;
    m_nTutorialReason = (unsigned char)nTutorial;

    return 1;
}

#ifndef _XBOX
void CClientExoAppInternal::AddDownloadedResources()  // PC 0x0063F6B0 GOG 0x0063F510
{
    EXOXBOXASSERT(g_pExoResMan);
    CExoString sResourceName;
    CExoString sAlias;
    for (int nPackage = 1; nPackage < CTlkTable::NUM_CONTENT_PKGS; ++nPackage)
    {
        sResourceName.Format("live%d", nPackage);
        sAlias.Format("LIVE%d", nPackage);
        if (g_pExoBase->m_pExoAliasList->GetAliasPath(sAlias, 0).GetLength())
        {
            CResRef cTalkTable(sResourceName);
            if (g_pExoResMan->Exists(cTalkTable, RESTYPE_TLK, (unsigned long*)NULL))
            {
                sAlias.Format("LIVE%d:live%d", nPackage, nPackage);
                g_pTlkTable->AddFile(sAlias, nPackage);
            }
            CResRef cKeyTable(sResourceName);
            if (g_pExoResMan->Exists(cKeyTable, 9999, (unsigned long*)NULL))
            {
                sAlias.Format("LIVE%d:live%d", nPackage, nPackage);
                g_pExoResMan->AddFixedKeyTableFile(sAlias);
            }
            CResRef cResourceImage(sResourceName);
            if (g_pExoResMan->Exists(cResourceImage, RESTYPE_RIM, (unsigned long*)NULL))
            {
                sAlias.Format("LIVE%d:RIMSXBOX\\live%d", nPackage, nPackage);
                g_pExoResMan->AddResourceImageFile(sAlias);
            }
            CResRef cModule(sResourceName);
            if (g_pExoResMan->Exists(cModule, RESTYPE_MOD, (unsigned long*)NULL))
            {
                sAlias.Format("LIVE%d:live%d", nPackage, nPackage);
                g_pExoResMan->AddEncapsulatedResourceFile(sAlias, 2);
            }
            CExoString sDXResourceName = sResourceName + "DX";
            CResRef cDXResourceImage(sResourceName);
            if (g_pExoResMan->Exists(cDXResourceImage, RESTYPE_RIM, (unsigned long*)NULL))
            {
                sAlias.Format("LIVE%d:RIMSXBOX\\live%ddx", nPackage, nPackage);
                g_pExoResMan->AddResourceImageFile(sAlias);
            }
            sAlias.Format("LIVE%d:OVERRIDE\\textures", nPackage);
            g_pExoResMan->AddEncapsulatedResourceFile(sAlias, 1);
        }
    }
}
#endif
// PC 0x0063F9A0 GOG 0x0063F800
void CClientExoAppInternal::PlayerFlourishWeapons()
{
    CSWCCreature* pPlayerCreature = GetPlayerCreature();
    if (pPlayerCreature->m_n464 > 0)
    {
        return;
    }

    GetPlayerCreature()->ComputeWeaponOverlays(0, 1);

    CSWSCreature* pServerCreature = GetPlayerCreature()->GetServerCreature();

    if (pServerCreature != NULL && pServerCreature->GetDead() == 0 &&
        pServerCreature->GetDeadTemp() == 0)
    {
        if (pServerCreature->GetArea()->GetPlayerRestrictMode() != 0)
        {
            return;
        }

        switch (pServerCreature->GetClientCreature()->GetAnimationBase()
                    ->m_nWieldClass)
        {
        case CSWCAnimBase::WIELD_CLASS_1:
        case CSWCAnimBase::WIELD_CLASS_UNARMED:
            return;
        }

        unsigned long oidWeapon =
            GetPlayerCreature()->GetEquippedItemID(INVENTORY_SLOT_RIGHTHAND);

        if (oidWeapon != OBJECT_INVALID)
        {
            CSWCObject* pWeaponObject =
                g_pAppManager->GetClientExoApp()->GetGameObject(oidWeapon);

            if (pWeaponObject != NULL && pWeaponObject->AsSWCItem() != NULL)
            {
                pWeaponObject->AsSWCItem()->PowerItem(1, 0, 1, 0);
            }
        }

        oidWeapon =
            GetPlayerCreature()->GetEquippedItemID(INVENTORY_SLOT_LEFTHAND);

        if (oidWeapon != OBJECT_INVALID)
        {
            CSWCObject* pWeaponObject =
                g_pAppManager->GetClientExoApp()->GetGameObject(oidWeapon);

            if (pWeaponObject != NULL && pWeaponObject->AsSWCItem() != NULL)
            {
                pWeaponObject->AsSWCItem()->PowerItem(1, 0, 1, 0);
            }
        }

        pServerCreature->SetCombatState(1, CSWSCreature::COMBAT_STATE_EQUIPMENT);
    }

    if (GetPlayerMovementInput())
    {
        GetPlayerCreature()->GetAnimationBase()->SetOverlayAnimation(
            GetPlayerCreature()->GetAnimationBase()->ServerToClientAnimation(
                10157),
            1);
    }
    else
    {
        GetPlayerCreature()->InsertFireForgetAnimation(10158, 1500, 1.0f,
                                                       NULL, 0);
    }
    pPlayerCreature->m_n464 = 1000;
}

#ifdef _XBOX

int CClientExoAppInternal::SaveOptions()
{
    unsigned char aSignature[OPTIONS_SIGNATURE_SIZE];
    CResStruct cStruct;

    g_pExoResMan->AddResourceDirectory("OPTIONS:");
    CResGFF* pGFF = new CResGFF;
    CExoString sFile = CExoString("OPTIONS:") + "opt";

    if (!pGFF->CreateGFFFile(&cStruct, "OPT ", "V3.2"))
    {
        g_pExoResMan->RemoveResourceDirectory("OPTIONS:");
        return 0;
    }

    g_pAppManager->GetClientExoApp()->GetClientOptions()->SaveToGFF(pGFF, &cStruct);
    pGFF->WriteGFFFile(sFile.CStr(), 0);
    if (pGFF != NULL)
    {
        delete pGFF;
    }
    g_pExoResMan->CreateSignatureFile(sFile.CStr(), 0, 0, aSignature, 1);
    g_pExoResMan->WriteSignatureFile(sFile.CStr(), aSignature);
    g_pExoResMan->RemoveResourceDirectory("OPTIONS:");
    return 1;
}

int CClientExoAppInternal::LoadOptions()
{
    unsigned char aSignature[OPTIONS_SIGNATURE_SIZE];
    CResStruct cStruct;
    CExoString sFile = CExoString("OPTIONS:") + "opt";

    g_pExoResMan->AddResourceDirectory("OPTIONS:");
    if (!g_pExoResMan->Exists("opt", 0, (unsigned long*)NULL))
    {
        m_pClientOptions->SetDefaults();
        SaveOptions();
        g_pExoResMan->RemoveResourceDirectory("OPTIONS:");
        return 1;
    }

    g_pExoResMan->CreateSignatureFile(sFile.CStr(), 0, 0, aSignature, 1);
    if (!g_pExoResMan->VerifySignatureFile(sFile.CStr(), aSignature))
    {

        m_bMainMenuTransition = 1;
        m_pClientOptions->SetDefaults();
        SaveOptions();
        g_pExoResMan->RemoveResourceDirectory("OPTIONS:");
        return 0;
    }

    CResGFF* pGFF = new CResGFF(0, "OPT ", "opt");
    pGFF->GetTopLevelStruct(&cStruct);
    g_pAppManager->GetClientExoApp()->GetClientOptions()->LoadFromGFF(pGFF, &cStruct);
    g_pAurora->ResetGammaRamp(
        g_pAppManager->GetClientExoApp()->GetClientOptions()->GetGammaSetting(), 0);
    if (pGFF != NULL)
    {
        delete pGFF;
    }
    g_pExoResMan->RemoveResourceDirectory("OPTIONS:");
    return 1;
}
#else
int CClientExoAppInternal::SaveOptions()  // PC 0x0063FC00 GOG 0x0063FA60
{
    CClientOptions* pClientOptions =
        g_pAppManager->m_pClientExoApp->GetClientOptions();

    return pClientOptions->SaveOptions();
}
// PC 0x0063FC20 GOG 0x0063FA80
int CClientExoAppInternal::LoadOptions(int bSetAntiAliasing)
{
    CClientExoApp* pClientExoApp;

    if (m_pClientOptions->LoadOptions(bSetAntiAliasing) == 0)
    {
        m_pClientOptions->SetDefaultOptions();
    }

    pClientExoApp = g_pAppManager->m_pClientExoApp;

    g_pAurora->ResetGammaRamp(
        pClientExoApp->GetClientOptions()->GetGammaSetting(), 0);

    return 1;
}
#endif
// PC 0x00643450 GOG 0x00643300
int CClientExoAppInternal::ChangeCharacterToNextLivingPartyMember(
    int bCheckDead, int bPlaySound, unsigned char nPartyMembers)
{
    if (nPartyMembers == 0)
    {
        return 1;
    }

    if (g_pAppManager->GetServerExoApp() == NULL)
    {
        return 0;
    }

    unsigned long oidPlayerCreature =
        g_pAppManager->GetServerExoApp()->GetPlayerCreatureId();
    CSWSPlayer* pPlayer = g_pAppManager->GetServerExoApp()
                              ->GetClientObjectByObjectId(oidPlayerCreature);
    int bFoundLiving = 0;
    if (pPlayer != NULL)
    {
        unsigned long oidOriginalCharacter =
            m_pSWParty->GetPlayerCharacter()->GetObjectId();
        CSWSCreature* pPlayerServerCreature;
        if (bCheckDead != 0)
        {
            unsigned long oidPCObject = pPlayer->m_oidPCObject;
            pPlayerServerCreature = g_pAppManager->GetServerExoApp()
                                        ->GetCreatureByGameObjectID(
                                            oidPCObject);
            if (pPlayerServerCreature != NULL)
            {
                if (pPlayerServerCreature->GetDead() == 0 &&
                    pPlayerServerCreature->GetDeadTemp() == 0)
                {
                    bFoundLiving = 1;
                }
            }
        }

        CSWCCreature* pCharacter;
        unsigned char nScanned = 0;
        int nTries = nPartyMembers * 3;
        while (nScanned != nPartyMembers && nTries > 0)
        {
            ChangeCharacter(CSWParty::LEADER_NEXT, 0);
            CSWParty* pParty = m_pSWParty;
            pCharacter = pParty->GetPlayerCharacter();
            if (pCharacter != NULL)
            {
                int bDead = pCharacter->GetServerCreature()->GetDead();
                int bDeadTemp = pCharacter->GetServerCreature()->GetDeadTemp();
                if (bDead || bDeadTemp)
                {
                    nTries--;
                }
                if (bDead == 0 && bDeadTemp == 0 &&
                    (bFoundLiving == 0 ||
                     pPlayerServerCreature->GetId() == pPlayer->m_oidPCObject))
                {
                    nScanned++;
                }
            }
        }

        if (nScanned == nPartyMembers && nTries > 0)
        {
            if (oidOriginalCharacter !=
                m_pSWParty->GetPlayerCharacter()->GetObjectId())
            {
                if (bPlaySound != 0)
                {
                    PlayCharacterSelectSound(pCharacter);
                }
                return 1;
            }
            return 0;
        }
        if (nTries == 0)
        {
            return 0;
        }
        unsigned long oidPCObject = pPlayer->m_oidPCObject;
        int nIndex = g_pAppManager->GetClientExoApp()->GetSWParty()
                         ->GetIndex(oidPCObject);
        ChangeCharacter(nIndex, 0);
        return 0;
    }
    return 0;
}
// PC 0x00643630 GOG 0x006434E0
void CClientExoAppInternal::UpdateLoadingScreenHint()
{
    if (GetLoadScreenHint() != 0)
    {
        unsigned long nTime = g_pExoBase->m_pExoTimers->GetLowResolutionTimer();
        unsigned long nLastTime = m_nLoadScreenStartTime;
        if (nLastTime == 0)
            nLastTime = nTime;
        unsigned long nElapsed = nTime - nLastTime;
        m_nLoadScreenStartTime = nTime;
        nTime = m_nLoadScreenHintUpdateTimer;
        m_nLoadScreenHintUpdateTimer = nTime + nElapsed;
        if (m_nLoadScreenHintUpdateTimer >= LOAD_SCREEN_HINT_INTERVAL)
        {
            m_nLoadScreenHintUpdateTimer = 0;
            if (g_pAppManager->GetServerExoApp() == NULL ||
                g_pAppManager->GetServerExoApp()->GetModuleName().GetLength() <= 0)
            {
                CExoString sModule("");
                SetLoadScreenHint(GetNextLoadScreenHintSTRREF(
                    sModule, LOAD_SCREEN_HINT_GAMEPLAY));
            }
            else
            {
                SetLoadScreenHint(GetNextLoadScreenHintSTRREF(
                    g_pAppManager->GetServerExoApp()->GetModuleName(),
                    LOAD_SCREEN_HINT_STORY));
            }
        }
    }
}
// PC 0x0063FC80 GOG 0x0063FAE0
int CClientExoAppInternal::GetNextLoadScreenHintSTRREF(
    const CExoString& sModule, unsigned char nHintType)
{
    if (!sModule.IsEmpty())
    {
        unsigned char nArea = sModule[0] - '0';
        if (!sModule.CompareNoCase("950COR") && nArea < STORY_HINT_AREAS)
        {
            CExoIni cIni;
            CExoString sValue;
            cIni.ReadIniEntry(sValue, ".\\swkotor2.ini", "Game Options",
                              "UnlockedPlanetSongs");
            int nUnlocked = sValue.AsINT();
            nUnlocked |= 1 << nArea;
            cIni.WriteIniEntry(CExoString(nUnlocked), ".\\swkotor2.ini",
                               "Game Options", "UnlockedPlanetSongs");
        }
    }

    int nStrRef = 0;
    int nHint = 0;
    int bStoryHint;
    switch (nHintType)
    {
    case LOAD_SCREEN_HINT_GAMEPLAY:
        bStoryHint = 0;
        break;
    case LOAD_SCREEN_HINT_STORY:
        bStoryHint = 1;
        break;
    default:
        if (g_pAppManager->GetServerExoApp() == NULL ||
            g_pAppManager->GetServerExoApp()->m_pcExoAppInternal == NULL ||
            !g_pAppManager->GetServerExoApp()->GetModulePreviouslyVisited(sModule))
            bStoryHint = 0;
        else
            bStoryHint = 1;
        break;
    }
    if (!bStoryHint)
    {
        C2DA* pHints = new C2DA("loadscreenhints", 0);
        if (pHints == NULL)
            return 0;
        if (!pHints->Load2DArray())
        {
            delete pHints;
            return 0;
        }
        if (pHints->GetINTEntry(m_nGamePlayHint, "GamePlayHint", &nHint))
        {
            nStrRef = nHint;
            ++m_nGamePlayHint;
        }
        else
        {
            m_nGamePlayHint = 0;
            if (pHints->GetINTEntry(m_nGamePlayHint, "GamePlayHint", &nHint))
            {
                nStrRef = nHint;
                ++m_nGamePlayHint;
            }
        }
        pHints->Unload2DArray();
        delete pHints;
        return nStrRef;
    }
    else
    {
        C2DA* pStoryHints = new C2DA("loadstoryhints", 0);
        if (pStoryHints != NULL)
        {
            if (pStoryHints->Load2DArray())
            {
                unsigned char nArea = sModule[0] - '0';
                if (nArea <= STORY_HINT_AREAS)
                {
                    int nRows = pStoryHints->GetNumRows() + 1;
                    CExoString sAreaNumber("AreaNumber");
                    int nTry = 0;
                    do
                    {
                        if (!pStoryHints->GetINTEntry(m_anStoryHint[nArea], sAreaNumber, &nHint))
                        {
                            m_anStoryHint[nArea] = 0;
                        }
                        else if (nHint != nArea)
                        {
                            ++m_anStoryHint[nArea];
                        }
                        else
                        {
                            unsigned char bNoHint =
                                pStoryHints->GetINTEntry(m_anStoryHint[nArea], "StoryHint", &nHint) == 0;
                            nStrRef = bNoHint ? 0 : nHint;
                            ++m_anStoryHint[nArea];
                            pStoryHints->Unload2DArray();
                            delete pStoryHints;
                            return nStrRef;
                        }
                    } while (++nTry <= nRows);
                    pStoryHints->Unload2DArray();
                    delete pStoryHints;
                    return nStrRef;
                }
                pStoryHints->Unload2DArray();
            }
            delete pStoryHints;
        }
    }
    return 0;
}
// PC 0x00640190 GOG 0x0063FFF0
void CClientExoAppInternal::SetMouseMode(unsigned char nMode)
{
    HideMouse(4);
#ifndef _XBOX
    g_pExoInput->SetMouseMode(nMode);
#endif

    if (m_pSWGuiManager != NULL)
    {
        if (nMode == 1)
        {
#ifndef _XBOX
            m_pSWGuiManager->ActivateSoftwareMouse();
#endif
        }
        else
        {
#ifndef _XBOX
            m_pSWGuiManager->DeactivateSoftwareMouse();
#endif
        }
    }

    ShowMouse(4);
}
// PC 0x006401E0 GOG 0x00640040
void CClientExoAppInternal::CancelMovie()
{
    if (m_pMoviePlayer != NULL)
    {
#ifdef _XBOX
        m_pMoviePlayer->CancelMovie();
#else
        m_pMoviePlayer->CancelMovie(1, 0);
#endif
    }
}
// PC 0x00640200 GOG 0x00640060
void CClientExoAppInternal::SetLastTarget(unsigned long oidObject, int bChanged)
{
    m_PauseFlags.m_Bits.m_bLastTargetChanged =
        (m_nLastTarget == oidObject) ? 0 : bChanged;
    m_nLastTarget = oidObject;
}
// PC 0x00640240 GOG 0x006400A0
void CClientExoAppInternal::BusyWaitMenuMusic()
{
    unsigned long nStart =
        (unsigned long)(g_pExoBase->m_pExoTimers->GetHighResolutionTimer() /
                        1000);
    unsigned long nElapsed;

    if (m_pMenuMusic != NULL)
    {
        m_pMenuMusic->FadeAndStop(2500);
    }

    do
    {
        g_pExoSound->Render();
        nElapsed =
            (unsigned long)(g_pExoBase->m_pExoTimers->GetHighResolutionTimer() /
                            1000) -
            nStart;
    } while (nElapsed < 750);

    if (m_pMenuMusic != NULL)
    {
        m_pMenuMusic->Stop();
    }
}
// PC 0x006402C0 GOG 0x00640120
void CClientExoAppInternal::BusyWaitMuteSound()
{
    unsigned long nStart =
        (unsigned long)(g_pExoBase->m_pExoTimers->GetHighResolutionTimer() /
                        1000);
    unsigned long nElapsed;

    g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_MUTE);

    do
    {
        g_pExoSound->Render();
        nElapsed =
            (unsigned long)(g_pExoBase->m_pExoTimers->GetHighResolutionTimer() /
                            1000) -
            nStart;
    } while (nElapsed < 500);
}
// PC 0x00640330 GOG 0x00640190
void CClientExoAppInternal::DoQuickSave()
{
    CExoString sSaveDir("QUICKSAVE");
    CExoString sSaveName("QUICKSAVE");

    CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;

#ifdef _XBOX
    pClientExoApp->SendSaveGameRequest(0, sSaveName);
#else
    pClientExoApp->SendSaveGameRequest(0, sSaveName, sSaveDir);
#endif
}

#ifndef _XBOX
void CClientExoAppInternal::SetToolTipInstantMode(int bInstant)  // PC 0x006403C0 GOG 0x00640220
{
    if (m_pGuiInGame != NULL)
    {
        if (bInstant != 0)
        {
            g_pGuiMan->m_fToolTipDelay = 0.0f;

            if (m_pGuiInGame->m_pMainInterface != NULL)
            {
                m_pGuiInGame->m_pMainInterface->DisplayToolTipMessage();
            }
        }
        else
        {
            double fToolTipDelay =
                g_pAppManager->m_pClientExoApp->GetClientOptions()->m_fGuiScale;

            g_pGuiMan->m_fToolTipDelay = fToolTipDelay;

            if (m_pGuiInGame->m_pMainInterface != NULL)
            {
                m_pGuiInGame->m_pMainInterface->RemoveToolTipMessage();
            }
        }
    }
}
#endif
// PC 0x00640430 GOG 0x00640290
void CClientExoAppInternal::HideMovieFadePanel()
{
    if (m_pMoviePlayer != NULL)
    {
#ifdef _XBOX
        m_pMoviePlayer->HideFadePanel(0);
#else
        m_pMoviePlayer->HideFadePanel();
#endif
    }
}
// PC 0x00640440 GOG 0x006402A0
void CClientExoAppInternal::PauseAllSoundsForMovieLostFocus()
{
    g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_LOSTFOCUS, 0);
}
// PC 0x00640460 GOG 0x006402C0
void CClientExoAppInternal::ResumeAllStreamingMusicForMovie()
{
    g_pExoSound->ResumeAllStreamingMusicForMovie();
}
#ifndef _XBOX
void CClientExoAppInternal::StartCreditSequence(const Vector& vColor,  // PC 0x00640470 GOG 0x006402D0
                                              const CExoString& sMusic)
{
    if (!m_bCreditSequenceInProgress)
    {
#ifdef _XBOX
        CSWGuiInGameCredits* pCredits = new CSWGuiInGameCredits(g_pGuiMan);
#else
        CSWGuiInGameCredits* pCredits = new CSWGuiInGameCredits(g_pGuiMan, sMusic);
#endif
        if (pCredits != NULL)
        {
            if (!g_pGuiMan->PanelExists(pCredits))
            {
                if (m_bEvilEndingHackInProgress)
                {
                    g_pGuiMan->AddPanel(pCredits, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
                }
                else
                {
                    g_pGuiMan->AddPanel(pCredits, CSWGuiManager::PANEL_MODE_MODAL, 1);
                }
            }
            pCredits->m_vColor = vColor;
            if (m_pModule != NULL && m_pModule->m_pInfo != NULL &&
                m_pModule->m_pInfo->m_pAmbientSound != NULL)
            {
                m_pModule->m_pInfo->m_pAmbientSound->StopSounds(0);
            }
            m_bCreditSequenceInProgress = 1;
        }
    }
}
// PC 0x00640560 GOG 0x006403C0
void CClientExoAppInternal::StopCreditSequence()
{
    m_bCreditSequenceInProgress = 0;
}
#endif
void CClientExoAppInternal::PlayCharacterSelectSound(CSWCCreature* pCreature)  // PC 0x00640570 GOG 0x006403D0
{
    int nRoll = rand() % 3;

    if (nRoll == 0)
    {
        pCreature->PlaySoundSetSound(7, 1);
        return;
    }

    if (nRoll == 1)
    {
        pCreature->PlaySoundSetSound(8, 1);
        return;
    }

    pCreature->PlaySoundSetSound(9, 1);
}
// PC 0x006405C0 GOG 0x00640420
void CClientExoAppInternal::EnableMouseAtEndOfLoadModuleMovie()
{
    if (m_pMoviePlayer == NULL || !m_pMoviePlayer->IsMoviePlaying())
    {
        if (m_bEnableMouseAtEndOfLoadModuleMovie != 0)
        {
            m_bEnableMouseAtEndOfLoadModuleMovie = 0;
            ShowMouse(8);
        }
    }
}
// PC 0x00640600 GOG 0x00640460
CExoString CClientExoAppInternal::GetDisplayCharacterForKey(int nKey)
{
#ifndef _XBOX
    if (nKey >= CExoInput::KEYBOARD_START &&
        nKey < CExoInput::JOYSTICK_START)
    {
        return g_pExoInput->GetDisplayCharacterForKey(nKey);
    }
#endif
    return g_pTlkTable->GetSimpleString(48724);
}
#ifndef _XBOX
void CClientExoAppInternal::SetEvilEndingHackInProgress(int bInProgress)  // PC 0x00640660 GOG 0x006404C0
{
    m_bEvilEndingHackInProgress = bInProgress;

    if (bInProgress == 0 && m_pEvilEndingHackSound != NULL)
    {
        m_pEvilEndingHackSound->Stop();
        delete m_pEvilEndingHackSound;
        m_pEvilEndingHackSound = NULL;
    }
}
#endif
void CClientExoAppInternal::RestoreMousePositionTo(int nX, int nY)  // PC 0x006406B0 GOG 0x00640510
{
    m_nMouseX = nX;
    m_nMouseY = nY;
}
// PC 0x006406D0 GOG 0x00640530
void CClientExoAppInternal::DoScreenShot()
{
    WIN32_FIND_DATA cFindData;
    HANDLE hFind;
    for (int nIndex = 0; nIndex < SCREENSHOT_MAX_INDEX; nIndex++)
    {
        sprintf(m_szScreenShot, "K2_%05i.tga", nIndex);
        hFind = FindFirstFile(m_szScreenShot, &cFindData);
        if (hFind == INVALID_HANDLE_VALUE)
            break;
        FindClose(hFind);
    }
    if (hFind == INVALID_HANDLE_VALUE)
        FindClose(hFind);
    m_bScreenShot = 1;
}
// PC 0x00640750 GOG 0x006405B0
void CClientExoAppInternal::UnloadSuperModelSet()
{
    switch (m_nSuperModelSet)
    {
    case 1:
        g_pExoResMan->RemoveEncapsulatedResourceFile("SUPERMODELS:smseta");
        break;
    case 2:
        g_pExoResMan->RemoveEncapsulatedResourceFile("SUPERMODELS:smsetb");
        break;
    case 3:
        g_pExoResMan->RemoveEncapsulatedResourceFile("SUPERMODELS:smsetc");
        break;
    }
}

int CClientExoAppInternal::DontUseThisSetPlayerCreature(unsigned long oidObject)
{
    unsigned long oidNormalized;

    if (oidObject != OBJECT_INVALID)
    {
        oidNormalized = oidObject | OBJECT_CLIENT_ID_MASK;
    }
    else
    {
        oidNormalized = OBJECT_INVALID;
    }

    CSWCCreature* pCreature = GetCreatureByGameObjectID(oidNormalized);

    if (pCreature != NULL)
    {
        m_oidCachedCreature = oidObject;
        m_nPlayerCreatureId = oidObject;
        m_pCachedCreature = pCreature;
        return 1;
    }
    return 0;
}

#ifndef _XBOX
const unsigned long LEGAL_SCREEN_MIN_SHOW_TIME = 2000;

void CClientExoAppInternal::BeginIntro()  // PC 0x0064EDA0 GOG 0x0064EC50
{
    if (!m_bSkipIntro)
    {
        AddMovieToMovieQueue("leclogo", 1);
        AddMovieToMovieQueue("obsidianent", 1);
        int nMovie;
        CExoArrayList<int>* pFlags = &m_aMovieQueueFlags;
        CExoArrayList<CExoString>* pMovies = &m_aMovieQueue;
        PlayMoviesInExoArrayList(*pMovies, nMovie, 0, 1, pFlags, 0);
        pMovies->num = 0;
        pFlags->num = 0;

        if (m_pLegalScreen == NULL)
        {
            m_pLegalScreen = new CSWGuiLegalScreen(m_pSWGuiManager,
                                                   LEGAL_SCREEN_MIN_SHOW_TIME);
        }

        m_pSWGuiManager->AddPanel(m_pLegalScreen, 1, 1);
        m_pLegalScreen->SetImage("legal");
        m_pLegalScreen->SetImage("");
        m_pSWGuiManager->RemovePanel(m_pLegalScreen);
        delete m_pLegalScreen;
        m_pLegalScreen = NULL;
    }
}
#endif

void CClientExoAppInternal::ClearCameraInput()
{
    m_pModule->TurnCamera(0.0f, 0.0f);
    m_pModule->AcclTurnCamera(0.0f, 0.0f);
}

inline void CSWGuiManager::SetGuiScale(float fScale)
{
    m_fGuiScale = fScale;
}

#include "guiingameinl.h"

#include "clientexoappinternalinl.h"
#ifndef _XBOX
int CClientExoAppInternal::MainLoop()  // PC 0x0064EF30 GOG 0x0064EDE0
{
    int bAreaReady = 0;
    static int a;

    if (m_pGuiInGame != NULL)
        m_pGuiInGame->SetAreaTransition(OBJECT_INVALID, Vector(0.0f, 0.0f, 0.0f));

    if (GetAreaNotReadyToDisplay() != 0 && m_pModule != NULL && GetPlayerCreature() != NULL)
    {
        if (!m_pModule->GetMiniGame() && m_oidFutureLeader != OBJECT_INVALID)
            CrownFutureLeader();
        else
        {
            if (m_pModule->GetMiniGame())
                SetInputClass(INPUT_CLASS_MINIGAME, 1);
            else
            {
                SetInputClass(INPUT_CLASS_GAME, 1);
                if (m_pPlayerControl != NULL)
                    m_pPlayerControl->SetEnabled(1);
                if (m_pGuiInGame != NULL)
                    m_pGuiInGame->SetSWGuiStatus(CGuiInGame::GUISTATUS_SHOW_MAIN_INTERFACE, 1);
                m_pGuiInGame->InitializeMap();
            }
            EnableInput();
            m_pModule->GetModuleTitle();
#ifdef _XBOX
            WaitForMovie();
#else
            WaitForMovie(1);
            if (IsMoviePlaying() != 0 ||
                g_bRenderWindowActive)
                HideLoadScreen(1, 1, 1);
            if (IsMoviePlaying() == 1)
                WaitForMovie(1);
            else
                m_aMoveToModuleMovies.RemoveAll(0);
            if (!m_pModule->GetMiniGame())
                ShowMouse(MOUSE_HIDE_MINIGAME);
            RestoreResolutionFromMovie();
            m_pMoviePlayer->HideFadePanel();
#endif
            if (g_bRenderWindowActive || m_pModule->GetMiniGame())
            {
#ifdef _XBOX
                HideLoadScreen(1);
#else
                HideLoadScreen(1, 1, 1);
#endif
                m_pCombatTimer->PauseWorldTimer();
                m_pPauseTimer->PauseWorldTimer();
                m_pAnimationTimer->PauseWorldTimer();
                m_pCombatTimer->ResetTimer(GetWorldTimer());
                m_pPauseTimer->ResetTimer(GetWorldTimer());
                m_pAnimationTimer->ResetTimer(GetWorldTimer());
                m_pWorldTimer->SetSnapshotTime();
                m_pCombatTimer->SetSnapshotTime();
                m_pPauseTimer->SetSnapshotTime();
                m_pAnimationTimer->SetSnapshotTime();
                CWorldTimer* pTimer = m_pPauseTimer;
                pTimer->m_nPreviousSnapshotTime = pTimer->GetSnapshotTime();
                pTimer = m_pAnimationTimer;
                pTimer->m_nPreviousSnapshotTime = pTimer->GetSnapshotTime();
                pTimer = m_pCombatTimer;
                pTimer->m_nPreviousSnapshotTime = pTimer->GetSnapshotTime();
                pTimer = m_pWorldTimer;
                pTimer->m_nPreviousSnapshotTime = pTimer->GetSnapshotTime();
                m_pWorldTimer->UnpauseWorldTimer();
                m_pCombatTimer->UnpauseWorldTimer();
                m_pPauseTimer->UnpauseWorldTimer();
                m_pAnimationTimer->UnpauseWorldTimer();
                g_pAppManager->GetServerExoApp()->GetWorldTimer()->SetSnapshotTime();
                g_pAppManager->GetServerExoApp()->GetWorldTimer()->UnpauseWorldTimer();
                if (!g_pAppManager->GetServerExoApp()->GetAutoSavePending())
                {
                    m_pModule->m_pInfo->StartSounds();
                    if (g_pAppManager->GetClientExoApp()->GetInGameGui()->m_nGlobalDialogState ||
                        g_pAppManager->GetClientExoApp()->GetInGameGui()->m_bHoldWorldFadeInForDialog)
                        g_pAppManager->GetClientExoApp()->GetInGameGui()->m_bHoldWorldFadeInForDialog = 1;
                    else
                        g_pAppManager->GetClientExoApp()->GetInGameGui()->StartGlobalFade(
                            1, 0.5f, 1.0f, Vector(0.0f, 0.0f, 0.0f));
                    SetCanSendDialog(1);
                    g_pExoInput->EndAttemptedBind();
                    m_pGuiInGame->SetGlobalUnpauseableState(0);
                    g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_GENERIC);
                }
                bAreaReady = 1;
                DumpLogCounters(NULL);
            }
        }
    }

    if (GetActivePauseState() == 0)
        m_pWorldTimer->SetSnapshotTime();
    if (GetActivePauseState() != PAUSE_STATE_COMBAT)
        m_pCombatTimer->SetSnapshotTime();
    m_pPauseTimer->SetSnapshotTime();
    CSWReentrantServerStats* pStats = g_pAppManager->m_pReentrantServerStats;
    if (pStats->m_bOperationActive == 1)
    {
        if (!g_pAppManager->GetClientExoApp()->GetCaptureEventPlayBack())
        {
            m_pUnknown10->ProcessReceivedFrames(1);
            m_pUnknown10->UpdateStatusLoop(CNetLayer::APPLICATION_CLIENT);
        }
        StallServerCode();
        pStats = g_pAppManager->m_pReentrantServerStats;
        if (pStats->m_bOperationActive &&
            (pStats->m_nOperation == CSWReentrantServerStats::OPERATION_LOAD_MODULE ||
             g_pAppManager->m_pReentrantServerStats->m_nOperation == CSWReentrantServerStats::OPERATION_LOAD_GAME))
        {
            m_nAreaNotReadyToDisplay = 1;
            if (g_pAppManager->m_pReentrantServerStats->m_nUnknown56 == 4)
            {
                DeltaT = (int)m_pWorldTimer->GetSnapshotTimeDifference() / 1000000.0f;
                ProcessInput(DeltaT);
            }
            return m_bExitGame && m_bExitGamePending;
        }
    }
    DeltaT = (int)m_pWorldTimer->GetSnapshotTimeDifference() / 1000000.0f;
    g_pExoResMan->Update(0);
    if (m_pSWParty != NULL && m_pModule != NULL && m_pModule->m_pInfo != NULL)
        m_pSWParty->Update(DeltaT);
    AurMakeCurrent();
    ProcessInput(DeltaT);
    UpdateDeathSequence();
    if (m_bEndGamePending &&
        !g_pAppManager->GetClientExoApp()->GetInGameGui()->m_bControllerPauseActive)
    {
        if (m_fEndGamePendingDelay > 0.0f)
        {
            if (!m_pSWGuiManager->PanelExists(m_pGuiInGame->GetDeathDisplay()) &&
                m_nEndGamePendingUnknown)
            {
                CGuiInGame* pGui = m_pGuiInGame;
                pGui->DisplayDeathMessage();
            }
        }
        else if (!m_nGameOver)
        {
#ifndef _XBOX
            m_pGuiInGame->RemoveDeathMessage(true);
            m_pGuiInGame->HideSWInGameGui(0);
#endif
            if (g_pAppManager->GetServerExoApp() != NULL)
                g_pAppManager->GetServerExoApp()->UnloadModule();
            g_pAppManager->DestroyServer();
            DisplayMainMenu(0);
            m_pGuiInGame->ShutDown();
            m_bEndGamePending = 0;
            m_fEndGamePendingDelay = 0.0f;
            m_nEndGamePendingUnknown = 1;
            g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_GENERIC);
            m_bStartMenuMusicOnMovieEnd = 1;
            StartMenuMusic(1, MENU_MUSIC_VOLUME, MENU_MUSIC_PRIORITY_GROUP);
            m_pGuiInGame->SetSWGuiStatus(CGuiInGame::GUISTATUS_DIALOG, 1);
        }
    }
    int bSlowMotion = m_nUnknown1B0;
    if (bSlowMotion || SlowGame)
        SetGameSpeed(0.25f);
    else
        SetGameSpeed(1.0f);
    if (!g_pAppManager->GetClientExoApp()->GetCaptureEventPlayBack())
    {
        m_pUnknown10->ProcessReceivedFrames(1);
        m_pUnknown10->UpdateStatusLoop(CNetLayer::APPLICATION_CLIENT);
    }
    if (m_bRunScript)
    {
        g_pVirtualMachine->RunScript(&m_sRunScript, 0, 0);
        m_bRunScript = 0;
    }
    if (GetPlayerCreature() != NULL &&
        GetPlayerCreature()->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1) != NULL)
    {
        if (m_fPlayerAnimationTime <= DeltaT)
        {
            m_fPlayerAnimationTime = 0.0f;
            if (m_lstPlayerAnimations.GetHeadPos() == NULL)
                a = 0;
            else
            {
                CExoString sAnimation;
                CQueuedAnim* pAnimation = m_lstPlayerAnimations.RemoveHead();
                float fTime;
                float fLength;
                if (pAnimation != NULL)
                {
                    if (pAnimation->sAnimation.GetLength() > 0)
                        GetPlayerCreature()->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1)->GetAnimation(
                            pAnimation->sAnimation.CStr(), &fTime, &fLength);
                    else
                    {
                        sAnimation = GetPlayerCreature()->GetAnimationBase()->GetAnimationName(pAnimation->nAnimation);
                        GetPlayerCreature()->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1)->GetAnimation(
                            sAnimation.CStr(), &fTime, &fLength);
                    }
                    while (m_lstPlayerAnimations.GetHeadPos() != NULL &&
                           fLength == Gob::ANIMATION_LENGTH_NOT_FOUND)
                    {
                        pAnimation = m_lstPlayerAnimations.RemoveHead();
                        if (pAnimation != NULL)
                            GetPlayerCreature()->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1)->GetAnimation(
                                pAnimation->sAnimation.CStr(), &fTime, &fLength);
                    }
                    if (pAnimation != NULL)
                    {
                        if (printanimqueue)
                        {
                            CExoString sText;
                            if (pAnimation->sAnimation.GetLength() > 0)
                                sText.Format("Playing next anim in queue: %s", pAnimation->sAnimation.CStr());
                            else
                                sText.Format("Playing next anim in queue: %s", sAnimation.CStr());
                            AurPostString(sText.CStr(), 0, g_nAnimQueueSize + a++ % 5, g_fAnimQueueTime);
                        }
                        if (pAnimation->sAnimation.GetLength() > 0)
                        {
                            if (fLength > 0.0f)
                            {
                                if (pAnimation->bLooping)
                                {
                                    GetPlayerCreature()->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1)->PlayAnimation(
                                        pAnimation->sAnimation.CStr(), 1.0f, 0, 0.0f);
                                    m_fPlayerAnimationTime = pAnimation->fDuration;
                                }
                                else
                                {
                                    GetPlayerCreature()->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1)->PlayAnimation(
                                        pAnimation->sAnimation.CStr(), 1.0f, Gob::ANIMATION_FLAG_NO_LOOP, 0.0f);
                                    m_fPlayerAnimationTime = fLength;
                                }
                            }
                        }
                        else if (GetPlayerCreature()->AnimationLooping(pAnimation->nAnimation))
                        {
                            GetPlayerCreature()->SetAnimation(pAnimation->nAnimation, 0, 0);
                            m_fPlayerAnimationTime = pAnimation->fDuration;
                        }
                        else
                        {
                            GetPlayerCreature()->AddFireForgetAnimation(pAnimation->nAnimation, NULL);
                            m_fPlayerAnimationTime = GetPlayerCreature()->GetAnimationBase()->GetAnimationLength(
                                pAnimation->nAnimation) * 0.001f;
                        }
                    }
                }
            }
        }
        else
            m_fPlayerAnimationTime -= DeltaT;
    }

    CSWCCreature* pCharacter = m_pSWParty->GetCharacter(0);
    CSWCModule* pModule = m_pModule;
    unsigned long nDay;
    unsigned long nTime;
    g_pAppManager->GetClientExoApp()->GetWorldTimer()->GetWorldTime(&nDay, &nTime);
    if (pModule != NULL && pModule->m_pInfo != NULL)
    {
        CSWCArea* pArea = pModule->m_pInfo;
        if (!m_nUnknown26C && m_nUnknown188)
            m_nUnknown26C = 1;
        else if (m_nUnknown26C && !m_nUnknown188)
        {
            pArea->StartSounds();
            m_nUnknown26C = 0;
        }
        CAurScene* pScene = pArea->m_pScene;
        if (pScene != NULL && pCharacter != NULL)
        {
            if (!m_pGuiInGame->m_nGlobalDialogState)
                pScene->SetSceneFocus(pCharacter->m_vPosition);
            else
                pScene->SetSceneFocus(m_pGuiInGame->GetDialogCameraPosition(1));
        }
        CGuiInGame* pGui = m_pGuiInGame;
        CSWGuiTutorialBox* pTutorialBox = pGui->m_pTutorialBox;
        int bTutorial = m_pSWGuiManager->PanelExists(pTutorialBox);
        if (!g_pAppManager->GetClientExoApp()->GetCaptureEventPlayBack() &&
            m_nInputClass != INPUT_CLASS_MINIGAME && !bTutorial)
            UpdateCamera(DeltaT);
        if (bTutorial)
        {
            ClearCameraInput();
        }
        g_starttime = g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
        m_pClientAIMaster->UpdateState();
        g_endtime = g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
        g_AIUpdateTime = (int)(g_endtime - g_starttime) * 0.001f;
        if (g_pAppManager->GetClientExoApp()->GetClientOptions()->m_nCameraMode == CClientOptions::CAMERA_MODE_NAVIGATE &&
            g_pAppManager->GetClientExoApp()->GetModule() != NULL &&
            g_pAppManager->GetClientExoApp()->GetModule()->GetCamera() != NULL)
            UpdateRoom(g_pAppManager->GetClientExoApp()->GetModule()->GetCamera()->GetPosition());
        else if (pCharacter != NULL)
        {
            if (!m_pGuiInGame->m_nGlobalDialogState)
                UpdateRoom(pCharacter->m_vDesiredPosition);
            else
                UpdateRoom(m_pGuiInGame->GetDialogCameraPosition(0));
        }
    }
    if (m_pGuiInGame != NULL && m_pGuiInGame->m_oidCameraAnimationObject != OBJECT_INVALID &&
        g_pAppManager->GetServerExoApp() != NULL)
    {
        unsigned long oidServerObject =
            g_pAppManager->GetServerExoApp()->ClientToServerObjectId(m_pGuiInGame->m_oidCameraAnimationObject);
        CGameObject* pObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidServerObject);
        if (pObject != NULL && pObject->AsSWSObject() != NULL)
        {
            pObject->AsSWSObject()->UpdateDialog();
            if (pObject->AsSWSObject()->m_bDialogEnded)
                pObject->AsSWSObject()->DeleteDialog();
        }
    }
    if (m_pGuiInGame != NULL)
    {
        m_pGuiInGame->UpdateDialog();
        m_pGuiInGame->UpdateAreaTransitionGUI();
    }
    if ((m_nInputClass == INPUT_CLASS_GAME || m_nInputClass == 4) &&
        !m_nAreaNotReadyToDisplay && !m_pGuiInGame->IsGlobalFading() &&
        !m_pGuiInGame->IsGlobalFadeObscuring())
    {
        DoPassiveSelection(DeltaT);
        UpdateCombatMode();
    }
    UpdateHostileHilite((__int64)g_pAppManager->m_pClientExoApp->m_pcExoAppInternal->m_pPauseTimer->GetSnapshotTimeDifference() / 1000000.0f);
    g_starttime = g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
    if (m_pGuiInGame->GetPendingStatusSummary() && m_nInputClass == INPUT_CLASS_GAME &&
        !m_nAreaNotReadyToDisplay && !m_pGuiInGame->m_bHoldWorldFadeInForDialog &&
        !m_pGuiInGame->m_nGlobalDialogState && !m_pGuiInGame->IsGlobalFading() &&
        !m_pGuiInGame->IsGlobalFadeObscuring())
    {
        if (m_fStatusSummaryDelay == 0.0f)
            m_fStatusSummaryDelay = 0.25f;
        else
        {
            if (DeltaT < 0.25f)
                m_fStatusSummaryDelay -= DeltaT;
            if (m_fStatusSummaryDelay <= 0.0f)
            {
                m_fStatusSummaryDelay = 0.0f;
                m_pGuiInGame->ShowStatusSummary();
            }
        }
    }
    else if (m_fStatusSummaryDelay != 0.0f)
        m_fStatusSummaryDelay = 0.0f;
    m_pSWGuiManager->Update(DeltaT);
    bool bForceSight = g_ForceSight;
    g_ForceSight = false;
    m_pSWGuiManager->SetGuiScale(DeltaT);
    DeltaT = (__int64)g_pAppManager->m_pClientExoApp->m_pcExoAppInternal->m_pPauseTimer->GetSnapshotTimeDifference() / 1000000.0f;
    if (m_fMouseHideTime > 0.0f)
    {
        m_fMouseHideTime -= DeltaT;
        if (m_fMouseHideTime <= 0.0f)
            ShowMouse(0);
    }
    m_pSWGuiManager->Draw(DeltaT);
    if (!m_pSWGuiManager->PanelExists(m_pGuiInGame->GetMainInterface()))
        m_pGuiInGame->RePopulateMainInterface();
    DeltaT = m_pSWGuiManager->m_fGuiScale;
    g_ForceSight = bForceSight;
    g_endtime = g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
    g_GuiUpdateTime = (int)(g_endtime - g_starttime) * 0.001f;
    g_starttime = g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
    UpdateRumble(DeltaT);
    if ((g_pAppManager->GetServerExoApp() != NULL &&
         !g_pAppManager->GetServerExoApp()->GetMoveToModulePending()) ||
        g_pAppManager->GetServerExoApp() == NULL)
        m_pGuiInGame->UpdateControllerPauseState();
    if (m_nAttackMashTutorialTimer > 0)
    {
        unsigned long nElapsed = (unsigned long)(DeltaT * 1000.0f);
        if ((unsigned long)m_nAttackMashTutorialTimer <= nElapsed)
            m_nAttackMashTutorialTimer = 0;
        else
            m_nAttackMashTutorialTimer -= nElapsed;
    }
    if (g_pExoSound != NULL)
        UpdateSoundEngine();
    g_endtime = g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
    g_SndUpdateTime = (int)(g_endtime - g_starttime) * 0.001f;
    if (m_bScreenShot && g_bEnableScreenCapture)
    {
        AurSnapshot(m_szScreenShot);
        m_bScreenShot = 0;
    }
    if (bAreaReady)
        m_nAreaNotReadyToDisplay = 0;
    if (m_fEnemyAutoPauseTimer > 0.0f)
    {
        m_fEnemyAutoPauseTimer -= DeltaT;
        if (m_fEnemyAutoPauseTimer <= 0.0f && !m_nCombatMode)
        {
#ifdef _XBOX
            SetAutoPaused(1, CClientExoApp::AUTOPAUSE_ENEMY_SIGHTED);
#else
            SetAutoPaused(1, 1);
            SetAutoPaused(1, m_nDelayedAutoPauseType);
            m_nDelayedAutoPauseType = AUTO_PAUSE_TYPE_UNSET;
#endif
        }
    }
    else if (m_fEnemyAutoPauseDelay > 0.0f)
        m_fEnemyAutoPauseDelay -= DeltaT;
    if (m_fEnemyAutoPauseCooldown > 0.0f)
        m_fEnemyAutoPauseCooldown -= DeltaT;
    if (m_PauseFlags.m_Bits.m_bTogglePauseAtEndOfFrame)
    {
#ifdef _XBOX
        m_pSWCMessage->SendPlayerToServerInput_TogglePauseRequest();
        m_pGuiInGame->SetPaused(GetPausedByCombat(), m_nTogglePauseType);
#else
        CSWSArea* pServerArea;
        if (m_pModule == NULL || m_pModule->m_pInfo == NULL ||
            (pServerArea = m_pModule->m_pInfo->GetServerArea()) == NULL ||
            !pServerArea->m_bTransitionPending)
        {
            if (GetPausedByCombat() != m_bTogglePause)
                m_pSWCMessage->SendPlayerToServerInput_TogglePauseRequest();
            m_pGuiInGame->SetPaused(GetPausedByCombat(), m_nTogglePauseType);
        }
#endif
        m_PauseFlags.m_Bits.m_bTogglePauseAtEndOfFrame = 0;
    }
    if (m_nTutorialReason != TUTORIAL_REASON_UNSET)
    {
        m_pGuiInGame->ShowTutorialWindow(m_nTutorialReason, m_nTutorialX, m_nTutorialY, m_nTutorialZ);
        m_nTutorialReason = TUTORIAL_REASON_UNSET;
    }
    return m_bExitGame && m_bExitGamePending;
}
#endif
void CClientExoAppInternal::DestroyQuestJournal()
{
    if (m_pQuestJournal != NULL)
    {
        delete m_pQuestJournal;
        m_pQuestJournal = NULL;
    }
}

#ifndef _XBOX
const char* playcredits(char* sCommand)
{

    CClientExoAppInternal* pInternal = g_pAppManager->m_pClientExoApp->m_pcExoAppInternal;
    pInternal->m_aMovieQueueFlags.Add(0);
    pInternal->AddMovieToExoArrayList(CExoString("55"), pInternal->m_aMovieQueue);
    pInternal = g_pAppManager->m_pClientExoApp->m_pcExoAppInternal;
    pInternal->m_aMovieQueueFlags.Add(1);
    pInternal->AddMovieToExoArrayList(CExoString("credits"), pInternal->m_aMovieQueue);
    g_pAppManager->m_pClientExoApp->m_pcExoAppInternal->PlayMovieQueue(1);
    return "Success";
}
#endif

#ifdef _XBOX

int CClientExoAppInternal::ChangeCharacterToNextLivingPartyMember(
    int bCheckDead, int bPlaySound)
{
    unsigned long oidPlayerCreature =
        g_pAppManager->GetServerExoApp()->GetPlayerCreatureId();
    CSWSPlayer* pPlayer = g_pAppManager->GetServerExoApp()
                              ->GetClientObjectByObjectId(oidPlayerCreature);
    int bFoundLiving = 0;
    if (pPlayer != NULL)
    {
        if (bCheckDead != 0)
        {
            unsigned long oidPCObject = pPlayer->m_oidPCObject;
            CSWSCreature* pPlayerServerCreature =
                g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(
                    oidPCObject);
            if (pPlayerServerCreature != NULL &&
                pPlayerServerCreature->GetDead() == 0 &&
                pPlayerServerCreature->GetDeadTemp() == 0)
            {
                bFoundLiving = 1;
            }
        }

        for (unsigned char nMember = 0;
             nMember < m_pSWParty->GetNumCharacters(); nMember++)
        {
            ChangeCharacter(CSWParty::LEADER_NEXT, bPlaySound);
            CSWCCreature* pCharacter = m_pSWParty->GetPlayerCharacter();
            if (pCharacter != NULL)
            {
                unsigned long oidServer =
                    g_pAppManager->GetServerExoApp()->ClientToServerObjectId(
                        pCharacter->GetObjectId());
                CSWSCreature* pServerCreature =
                    g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(
                        oidServer);
                if (pServerCreature->GetDead() == 0 &&
                    pServerCreature->GetDeadTemp() == 0 &&
                    (bFoundLiving == 0 ||
                     pServerCreature->GetId() == pPlayer->m_oidPCObject))
                {
                    return 1;
                }
            }
        }
    }
    return 0;
}
#endif

#ifdef _XBOX

int CClientExoAppInternal::WaitForMovie()
{
    int nPumpCount = 0;
    int bInputActive = g_pExoInput->GetActive();
    int bCancelled = 0;

    if (g_pExoInput != NULL)
    {
        g_pExoInput->SetActive(1);
    }

    if (IsMoviePlaying())
    {
        m_pMoviePlayer->SetFadeMsg(STRREF_MOVIE_LOADING);
    }

    while (IsMoviePlaying())
    {
        if (nPumpCount == 500 && !bCancelled)
        {
            Sleep(2);
            int nEvents;
            CExoArrayList<CExoInputEvent> aEvents = g_pExoInput->GetEvents(nEvents, INPUT_CLASS_MOVIE);
            if (nEvents)
            {
                for (int i = 0; i < nEvents; i++)
                {
                    unsigned long nEvent = aEvents[i].m_pDesc->m_nEvent;
                    if (aEvents[i].m_nValue &&
                        (nEvent == MOVIE_SKIP_EVENT_FIRST || nEvent == MOVIE_SKIP_EVENT_SECOND ||
                         (m_PauseFlags.m_Bits.m_nUnused1 &&
                          (nEvent == MOVIE_SKIP_EVENT_THIRD || nEvent == MOVIE_SKIP_EVENT_FOURTH))))
                    {
                        m_pMoviePlayer->SetFadeMsg(STRREF_MOVIE_SKIPPING);
                        m_pMoviePlayer->CancelMovie();
                        while (m_pMoviePlayer->IsMoviePlaying())
                        {
                            Sleep(2);
                        }
                        g_pExoInput->ClearEvents();
                        bCancelled = 1;
                        break;
                    }
                }
            }
            nPumpCount = 0;
        }
        else
        {
            nPumpCount++;
        }
    }

    m_pMoviePlayer->HideFadePanel(0);
    g_pAppManager->GetClientExoApp()->RemoveMoveToModuleMovies();
    if (m_bEnableMouseAtEndOfLoadModuleMovie)
    {
        m_pGuiInGame->SetGlobalUnpauseableState(0);
        g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_GENERIC);
        CWorldTimer* pClientTimer = g_pAppManager->GetClientExoApp()->GetWorldTimer();
        pClientTimer->UnpauseWorldTimer();
        CWorldTimer* pServerTimer = g_pAppManager->GetServerExoApp()->GetWorldTimer();
        pServerTimer->UnpauseWorldTimer();
    }

    if (g_pExoInput != NULL)
    {
        g_pExoInput->SetActive(bInputActive);
    }
    if (bInputActive == 0)
    {
        TurnOffKeyboardModal();
    }

    m_pMoviePlayer->SetFadeMsg(0);
    return bCancelled;
}
#endif

#ifdef _XBOX

inline void XboxSetGuiManagerSize(CSWGuiManager* pManager, int nWidth, int nHeight)
{
    pManager->m_nWidth = nWidth;
    pManager->m_nHeight = nHeight;
}

void CClientExoAppInternal::StartServices(int bPlayLoadMusic,
                                        const CExoString& sModule,
                                        const CExoString& sLoadMusic,
                                        const CExoString& sLoadScreen,
                                        unsigned long nPlayerId)
{
    CExoIni cIni;
    CExoString sValue;
    CExoString sFileName;
    int nLegalDisplayMinimum = 0;

    m_pUnknown1C0 = new CClientMsgSysAdmin();
    m_pWorldTimer = new CWorldTimer;
    m_pCombatTimer = new CWorldTimer;
    m_pPauseTimer = new CWorldTimer;
    m_pAnimationTimer = new CWorldTimer;
    m_pGameObjectArray = new CGameObjectArray(1);
    g_pExoResMan = new CExoResMan;

    if (cIni.ReadIniEntry(sValue, "d:\\swkotor2.ini", "Display Options",
                          "AllowWindowedMode"))
        g_bAllowWindowedMode = sValue.AsINT() != 0;
    g_pAurora = NewCAurora();
    g_pExoInput = new CExoInput;
    CreateWorldJournal();
    m_pUnknown10 = new CNetLayer;
    if (m_pServerInfo == NULL)
        m_pServerInfo = new CServerInfo;
    m_pSWCMessage = new CSWCMessage;
    m_pClientAIMaster = new CClientAIMaster;
    m_pClientAIMaster->SetExoAppInternal(this);

    if (cIni.ReadIniEntry(sValue, "d:\\swkotor2.ini", "Game Options",
                          "EnableReleaseLogging"))
        g_pExoBase->m_pExoDebug->SetEnabled(sValue.AsINT() != 0);
    if (cIni.ReadIniEntry(sValue, "d:\\swkotor2.ini", "Game Options",
                          "ExtendedResourceTracking"))
        g_bExtendedResourceTracking = sValue.AsINT() > 0;
    else
        g_bExtendedResourceTracking = false;
    if (cIni.ReadIniEntry(sValue, "d:\\swkotor2.ini", "Game Options",
                          "LegalDisplayMinimum"))
        nLegalDisplayMinimum = sValue.AsINT();
    g_pExoBase->m_pExoDebug->OpenLogFiles("swc");

    g_pExoResMan->AddResourceDirectory("TEMPCLIENT:");
    g_pExoResMan->AddResourceDirectory("OVERRIDE:");
    g_pExoResMan->AddResourceDirectory("ERRORTEX:");
    g_pSWTlkTable = new CSWTlkTable;
    g_pTlkTable = g_pSWTlkTable;
    if (bPlayLoadMusic)
        MountDownloadedContent(0, 0);
    g_pExoResMan->CacheResource("HD0:DATAXBOX\\items", RESTYPE_BIF);
    g_pExoResMan->CacheResource("HD0:DATAXBOX\\scripts", RESTYPE_BIF);
    g_pExoResMan->CacheResource("HD0:DATAXBOX\\gui", RESTYPE_BIF);
    g_pExoResMan->AddResourceDirectory("RIMSXBOX:");
    if (g_pExoResMan->Exists("SUBGLOBAL", RESTYPE_RIM, (unsigned long*)NULL))
        g_pExoResMan->AddResourceImageFile("RIMSXBOX:SUBGLOBAL");
    if (g_pExoResMan->Exists("SUBGLOBAL", RESTYPE_RIM, (unsigned long*)NULL))
        g_pExoResMan->AddResourceImageFile("RIMSXBOX:SUBGLOBALDX");
    g_pAuroraInterface = new CAuroraInterface;
    g_pExoResMan->AddResourceDirectory("SERVERVAULT:");
    g_pExoResMan->AddResourceDirectory("PORTRAITS:");
    g_pExoResMan->AddEncapsulatedResourceFile("OVERRIDE:textures", 1);
    ReInitAurora(g_nScreenWidth, g_nScreenHeight, g_nBitsPerPixel, g_nReInitializing == 2);
    if (bPlayLoadMusic)
    {
        g_pAurora->ResetGammaRamp(
            g_pAppManager->GetClientExoApp()->GetClientOptions()->GetGammaSetting(), 0);
    }
    else
    {
        g_pAppManager->m_pClientExoApp->GetClientOptions()->SetDefaults();
        g_pAppManager->m_pClientExoApp->LoadOptions(1);
    }
    int nMissing = m_nXboxUnmapped468;
    if (nMissing == 1)
        g_pExoResMan->ReportMissingFile("", 0xffff, 1);
    m_pSWGuiManager = new CSWGuiManager;
    XboxSetGuiManagerSize(m_pSWGuiManager, g_nScreenWidth, g_nScreenHeight);
    if (!bPlayLoadMusic)
    {
        if (g_pExoResMan->Exists("LEGAL", RESTYPE_RIM, (unsigned long*)NULL))
            g_pExoResMan->AddResourceImageFile("RIMSXBOX:LEGAL");
        if (g_pExoResMan->Exists("LEGALDX", RESTYPE_RIM, (unsigned long*)NULL))
            g_pExoResMan->AddResourceImageFile("RIMSXBOX:LEGALDX");
        m_pLegalScreen = new CSWGuiLegalScreen(m_pSWGuiManager, nLegalDisplayMinimum);
        m_pSWGuiManager->AddPanel(m_pLegalScreen, 1, 1);
        ((CSWGuiLegalScreen*)m_pLegalScreen)->SetImage("leclogo");
    }
    InitializeMoviePlayer();
    sFileName = g_pExoResMan->CacheResource("HD0:DIALOG", RESTYPE_TLK);
    if (!g_pTlkTable->OpenFile(sFileName))
        g_pExoResMan->ReportMissingFile("dialog", RESTYPE_TLK, 1);
    g_pExoResMan->AddFixedKeyTableFile("HD0:CHITIN");
    if (bPlayLoadMusic)
    {
        if (m_pGuiLoadScreen == NULL)
        {
            m_pGuiLoadScreen = new CSWGuiLoadScreen(m_pSWGuiManager);
            m_sLoadScreenImage = sModule;
            m_bShowLoadScreen = 1;
            ShowLoadScreen(1, 1, 0);
            if (m_pGuiLoadScreen != NULL)
                m_pGuiLoadScreen->SetHint(nPlayerId);
            unsigned char nLoadStep = m_aLoadSteps[0];
            SetLoadBarProgress(nLoadStep - 5, 1);
            if (m_aMoveToModuleMovies.num == 0)
            {
                m_pGuiLoadScreen->Draw(1.0f / 30.0f);
                UpdateScreen(1.0f / 30.0f, 0, 0);
            }
        }
        PlayLoadModuleMovies();
    }
    g_pExoResMan->AddResourceDirectory("HD0:MOVIES");
    g_pExoResMan->AddResourceDirectory("HD0:STREAMWAVES");
    g_pExoResMan->AddResourceDirectory("HD0:STREAMMUSIC");
    if (sLoadScreen == "" && sModule != "")
    {
        SetLoadBarProgress(20, 1);
        UpdateScreen(1.0f / 30.0f, 0, 0);
    }
    if (m_bMinGlobalRim == 1)
    {
        if (g_pExoResMan->Exists("MINIGLOBAL", RESTYPE_RIM, (unsigned long*)NULL))
        {
            sFileName = g_pExoResMan->CacheResource("RIMSXBOX:MINIGLOBAL", RESTYPE_RIM);
            g_pExoResMan->AddResourceImageFile(sFileName);
        }
        if (g_pExoResMan->Exists("MINIGLOBALDX", RESTYPE_RIM, (unsigned long*)NULL))
        {
            sFileName = g_pExoResMan->CacheResource("RIMSXBOX:MINIGLOBALDX", RESTYPE_RIM);
            g_pExoResMan->AddResourceImageFile(sFileName);
        }
    }
    else
    {
        if (g_pExoResMan->Exists("GLOBAL", RESTYPE_RIM, (unsigned long*)NULL))
        {
            sFileName = g_pExoResMan->CacheResource("RIMSXBOX:GLOBAL", RESTYPE_RIM);
            g_pExoResMan->AddResourceImageFile(sFileName);
        }
        if (!bPlayLoadMusic)
            ((CSWGuiLegalScreen*)m_pLegalScreen)->SetImage("biowarelogo");
        if (g_pExoResMan->Exists("GLOBALDX", RESTYPE_RIM, (unsigned long*)NULL))
        {
            sFileName = g_pExoResMan->CacheResource("RIMSXBOX:GLOBALDX", RESTYPE_RIM);
            g_pExoResMan->AddResourceImageFile(sFileName);
        }
    }
    if (sLoadScreen == "" && sModule != "")
    {
        SetLoadBarProgress(30, 1);
        UpdateScreen(1.0f / 30.0f, 0, 0);
    }
    InitializeSoundOptions();
    if (sLoadScreen == "" && sModule != "")
    {
        SetLoadBarProgress(40, 1);
        UpdateScreen(1.0f / 30.0f, 0, 0);
    }
    if (bPlayLoadMusic)
    {
        m_sLoadMusic = sLoadMusic;
        StartLoadMusic();
    }
    AddDownloadedResources();
    if (sLoadScreen == "" && sModule != "")
    {
        SetLoadBarProgress(50, 1);
        UpdateScreen(1.0f / 30.0f, 0, 0);
    }
    m_pUnknown10->Initialize(g_pAppManager->GetClientExoApp());
    srand(GetTickCount());
    m_pModule = new CSWCModule;
    if (m_pModule != NULL)
    {
        if (sLoadScreen == "" && sModule != "")
        {
            SetLoadBarProgress(60, 1);
            UpdateScreen(1.0f / 30.0f, 0, 0);
        }
        int bTexturePack = cIni.ReadIniEntry(sValue, "d:\\swkotor2.ini",
                                            "Display Options", "TexturePack");
        int nTexturePack = sValue.AsINT();
        if (bTexturePack && nTexturePack < 4)
            SetTexturePack(nTexturePack, 0);
        else
            SetTexturePack(3, 1);
        AddPatchFiles(0);

        int bFullScreen;
        if (cIni.ReadIniEntry(sValue, "d:\\swkotor2.ini", "Display Options", "FullScreen"))
            bFullScreen = sValue.AsINT();
        else
        {
            bFullScreen = 0;
            sValue.Format("%d", bFullScreen);
            cIni.WriteIniEntry(sValue, "d:\\swkotor2.ini", "Display Options", "FullScreen");
        }
        if (cIni.ReadIniEntry(sValue, "d:\\swkotor2.ini", "Display Options", "Disable Movies"))
            m_bMovieDisabled = sValue.AsINT();
        else
            m_bMovieDisabled = 0;
        if (cIni.ReadIniEntry(sValue, "d:\\swkotor2.ini", "Display Options",
                              "Disable Intro Movies"))
            m_bSkipIntro = sValue.AsINT();
        else
            m_bSkipIntro = 0;
        m_nXboxUnmapped264 = g_pAppManager->GetCodeWord();
        if (sLoadScreen == "" && sModule != "")
        {
            SetLoadBarProgress(70, 1);
            UpdateScreen(1.0f / 30.0f, 0, 0);
        }
        ReadAndSetVideoMode(1, bFullScreen);
        if (cIni.ReadIniEntry(sValue, "d:\\swplayer.ini", "Profile", "Player Name") == 1)
            m_sClientName = sValue;
        else
            m_sClientName = g_pTlkTable->GetSimpleString(0);
        m_nClientLanguage = g_pTlkTable->GetLanguageVersion();
        if (m_nClientLanguage >= CTlkTable::LANGUAGE_ASIAN_START)
            m_bClientImeUI = 1;
        CreateQuestJournal();
        m_pSoundSetList = new CSoundSetList;
        LoadItemDropSoundList();
        if (sLoadScreen == "" && sModule != "")
        {
            SetLoadBarProgress(80, 1);
            UpdateScreen(1.0f / 30.0f, 0, 0);
        }
        InitializeClientOptions();
        m_pSWGuiManager->LoadGuiSounds();
        m_pGuiInGame = new CGuiInGame;
        if (m_pGuiInGame != NULL)
        {
            C2DA* pRumble = new C2DA("rumble", 0);
            pRumble->Load2DArray();
            m_aRumblePatterns.SetSize(pRumble->m_nNumRows);
            for (int nRow = 0; nRow < pRumble->m_nNumRows; nRow++)
                m_aRumblePatterns.element[nRow].LoadPattern(nRow, pRumble);
            delete pRumble;
            if (!bPlayLoadMusic)
            {
                if (m_pGuiLoadScreen == NULL)
                    m_pGuiLoadScreen = new CSWGuiLoadScreen(m_pSWGuiManager);
            }
            else
            {
                if (sLoadScreen == "" && sModule != "")
                    SetLoadBarProgress(90, 1);
                else
                {
                    unsigned char nLoadStep = m_aLoadSteps[0];
                    SetLoadBarProgress(nLoadStep, 1);
                }
                UpdateScreen(1.0f / 30.0f, 0, 0);
            }
        }
    }

}

void CClientExoAppInternal::SetEventDescriptions()
{
    g_pExoInput->CreateNewEvent(7, CExoInput::EVENT_TYPE_RELATIVE, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_YAXIS, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(7, INPUT_CONTEXT_PC);
    g_pExoInput->ScaleEvent(7, 1.0f, 1.0f);
    g_pExoInput->CreateNewEvent(4, CExoInput::EVENT_TYPE_BUTTON, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_YAXIS, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(4, INPUT_CONTEXT_FREE_LOOK);
    g_pExoInput->CreateNewEvent(8, CExoInput::EVENT_TYPE_RELATIVE, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_XAXIS, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(8, INPUT_CONTEXT_PC);
    g_pExoInput->ScaleEvent(8, 1.0f, 1.0f);
    g_pExoInput->CreateNewEvent(3, CExoInput::EVENT_TYPE_BUTTON, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_XAXIS, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(3, INPUT_CONTEXT_FREE_LOOK);
    g_pExoInput->CreateNewEvent(12, CExoInput::EVENT_TYPE_RELATIVE, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_SLIDER0, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(12, INPUT_CONTEXT_PC);
    g_pExoInput->AddEvent(12, INPUT_CONTEXT_FREE_LOOK);
    g_pExoInput->ScaleEvent(12, 1.0f, 1.0f);
    g_pExoInput->CreateNewEvent(13, CExoInput::EVENT_TYPE_RELATIVE, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_SLIDER1, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(13, INPUT_CONTEXT_PC);
    g_pExoInput->AddEvent(13, INPUT_CONTEXT_FREE_LOOK);
    g_pExoInput->ScaleEvent(13, 1.0f, 1.0f);
    g_pExoInput->CreateNewEvent(21, CExoInput::EVENT_TYPE_BUTTON, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_BUTTON7, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(21, INPUT_CONTEXT_MINIGAME);
    g_pExoInput->CreateNewEvent(24, CExoInput::EVENT_TYPE_BUTTON, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_BUTTON0, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(24, INPUT_CONTEXT_MINIGAME);
    g_pExoInput->CreateNewEvent(25, CExoInput::EVENT_TYPE_BUTTON, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_BUTTON1, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(25, INPUT_CONTEXT_MINIGAME);
    g_pExoInput->CreateNewEvent(26, CExoInput::EVENT_TYPE_BUTTON, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_BUTTON2, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(26, INPUT_CONTEXT_MINIGAME);
    g_pExoInput->CreateNewEvent(27, CExoInput::EVENT_TYPE_BUTTON, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_BUTTON3, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(27, INPUT_CONTEXT_MINIGAME);
    g_pExoInput->CreateNewEvent(28, CExoInput::EVENT_TYPE_BUTTON, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_BUTTON6, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(28, INPUT_CONTEXT_MINIGAME);
    g_pExoInput->CreateNewEvent(33, CExoInput::EVENT_TYPE_POLLED, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_BUTTON12, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(33, INPUT_CONTEXT_MINIGAME);
    g_pExoInput->CreateNewEvent(34, CExoInput::EVENT_TYPE_POLLED, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_BUTTON13, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(34, INPUT_CONTEXT_MINIGAME);
    g_pExoInput->CreateNewEvent(36, CExoInput::EVENT_TYPE_POLLED, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_BUTTON11, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(36, INPUT_CONTEXT_MINIGAME);
    g_pExoInput->CreateNewEvent(35, CExoInput::EVENT_TYPE_POLLED, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_BUTTON10, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(35, INPUT_CONTEXT_MINIGAME);
    g_pExoInput->CreateNewEvent(31, CExoInput::EVENT_TYPE_RELATIVE, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_SLIDER1, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(31, INPUT_CONTEXT_MINIGAME);
    g_pExoInput->ScaleEvent(31, 1.0f, 1.0f);
    g_pExoInput->CreateNewEvent(32, CExoInput::EVENT_TYPE_RELATIVE, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_SLIDER0, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(32, INPUT_CONTEXT_MINIGAME);
    g_pExoInput->ScaleEvent(32, 1.0f, 1.0f);
    g_pExoInput->CreateNewEvent(22, CExoInput::EVENT_TYPE_RELATIVE, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_XAXIS, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(22, INPUT_CONTEXT_MINIGAME);
    g_pExoInput->ScaleEvent(22, 1.0f, 1.0f);
    g_pExoInput->CreateNewEvent(23, CExoInput::EVENT_TYPE_RELATIVE, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_YAXIS, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(23, INPUT_CONTEXT_MINIGAME);
    g_pExoInput->ScaleEvent(23, 1.0f, 1.0f);
    g_pExoInput->CreateNewEvent(11, CExoInput::EVENT_TYPE_BUTTON, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_BUTTON8, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(11, INPUT_CONTEXT_PC);
    g_pExoInput->AddEvent(11, INPUT_CONTEXT_FREE_LOOK);
    g_pExoInput->CreateNewEvent(2, CExoInput::EVENT_TYPE_BUTTON, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_BUTTON5, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(2, INPUT_CONTEXT_PC);
    g_pExoInput->AddEvent(2, INPUT_CONTEXT_FREE_LOOK);
    g_pExoInput->CreateNewEvent(37, CExoInput::EVENT_TYPE_BUTTON, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_BUTTON5, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(37, INPUT_CONTEXT_MINIGAME);
    g_pExoInput->CreateNewEvent(38, CExoInput::EVENT_TYPE_BUTTON, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_BUTTON8, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(38, INPUT_CONTEXT_MINIGAME);
    g_pExoInput->CreateNewEvent(45, CExoInput::EVENT_TYPE_BUTTON, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_BUTTON8, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(45, INPUT_CONTEXT_PC_GUI);
    g_pExoInput->AddEvent(45, INPUT_CONTEXT_DIALOG);
    g_pExoInput->CreateNewEvent(16, CExoInput::EVENT_TYPE_BUTTON, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_BUTTON9, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(16, INPUT_CONTEXT_MOVIE);
    g_pExoInput->CreateNewEvent(17, CExoInput::EVENT_TYPE_BUTTON, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_BUTTON1, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(17, INPUT_CONTEXT_MOVIE);
    g_pExoInput->CreateNewEvent(73, CExoInput::EVENT_TYPE_BUTTON, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_BUTTON0, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(73, INPUT_CONTEXT_MOVIE);
    g_pExoInput->CreateNewEvent(74, CExoInput::EVENT_TYPE_BUTTON, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_BUTTON8, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(74, INPUT_CONTEXT_MOVIE);
    g_pExoInput->CreateNewEvent(47, CExoInput::EVENT_TYPE_BUTTON, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_BUTTON10, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(47, INPUT_CONTEXT_PC);
    g_pExoInput->AddEvent(47, INPUT_CONTEXT_PC_GUI);
    g_pExoInput->AddEvent(47, INPUT_CONTEXT_DIALOG);
    g_pExoInput->AddEvent(47, INPUT_CONTEXT_FREE_LOOK);
    g_pExoInput->SetEventRepeatable(47, 500, 20);
    g_pExoInput->CreateNewEvent(48, CExoInput::EVENT_TYPE_BUTTON, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_BUTTON11, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(48, INPUT_CONTEXT_PC);
    g_pExoInput->AddEvent(48, INPUT_CONTEXT_PC_GUI);
    g_pExoInput->AddEvent(48, INPUT_CONTEXT_DIALOG);
    g_pExoInput->AddEvent(48, INPUT_CONTEXT_FREE_LOOK);
    g_pExoInput->SetEventRepeatable(48, 500, 20);
    g_pExoInput->CreateNewEvent(49, CExoInput::EVENT_TYPE_BUTTON, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_BUTTON12, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(49, INPUT_CONTEXT_PC);
    g_pExoInput->AddEvent(49, INPUT_CONTEXT_PC_GUI);
    g_pExoInput->AddEvent(49, INPUT_CONTEXT_DIALOG);
    g_pExoInput->AddEvent(49, INPUT_CONTEXT_FREE_LOOK);
    g_pExoInput->SetEventRepeatable(49, 500, 20);
    g_pExoInput->CreateNewEvent(50, CExoInput::EVENT_TYPE_BUTTON, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_BUTTON13, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(50, INPUT_CONTEXT_PC);
    g_pExoInput->AddEvent(50, INPUT_CONTEXT_PC_GUI);
    g_pExoInput->AddEvent(50, INPUT_CONTEXT_DIALOG);
    g_pExoInput->AddEvent(50, INPUT_CONTEXT_FREE_LOOK);
    g_pExoInput->SetEventRepeatable(50, 500, 20);
    g_pExoInput->CreateNewEvent(51, CExoInput::EVENT_TYPE_BUTTON, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_YAXIS, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(51, INPUT_CONTEXT_PC_GUI);
    g_pExoInput->AddEvent(51, INPUT_CONTEXT_DIALOG);
    g_pExoInput->SetEventRepeatable(51, 500, 20);
    g_pExoInput->CreateNewEvent(52, CExoInput::EVENT_TYPE_BUTTON, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_XAXIS, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(52, INPUT_CONTEXT_PC_GUI);
    g_pExoInput->AddEvent(52, INPUT_CONTEXT_DIALOG);
    g_pExoInput->SetEventRepeatable(52, 500, 20);
    g_pExoInput->CreateNewEvent(55, CExoInput::EVENT_TYPE_BUTTON, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_SLIDER0, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(55, INPUT_CONTEXT_PC_GUI);
    g_pExoInput->AddEvent(55, INPUT_CONTEXT_DIALOG);
    g_pExoInput->SetEventRepeatable(55, 250, 20);
    g_pExoInput->CreateNewEvent(56, CExoInput::EVENT_TYPE_BUTTON, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_SLIDER1, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(56, INPUT_CONTEXT_PC_GUI);
    g_pExoInput->AddEvent(56, INPUT_CONTEXT_DIALOG);
    g_pExoInput->SetEventRepeatable(56, 500, 20);
    g_pExoInput->CreateNewEvent(39, CExoInput::EVENT_TYPE_BUTTON, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_BUTTON0, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(39, INPUT_CONTEXT_PC);
    g_pExoInput->AddEvent(39, INPUT_CONTEXT_PC_GUI);
    g_pExoInput->AddEvent(39, INPUT_CONTEXT_DIALOG);
    g_pExoInput->AddEvent(39, INPUT_CONTEXT_FREE_LOOK);
    g_pExoInput->CreateNewEvent(40, CExoInput::EVENT_TYPE_BUTTON, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_BUTTON1, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(40, INPUT_CONTEXT_PC);
    g_pExoInput->AddEvent(40, INPUT_CONTEXT_PC_GUI);
    g_pExoInput->AddEvent(40, INPUT_CONTEXT_DIALOG);
    g_pExoInput->AddEvent(40, INPUT_CONTEXT_FREE_LOOK);
    g_pExoInput->CreateNewEvent(41, CExoInput::EVENT_TYPE_BUTTON, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_BUTTON2, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(41, INPUT_CONTEXT_PC);
    g_pExoInput->AddEvent(41, INPUT_CONTEXT_PC_GUI);
    g_pExoInput->AddEvent(41, INPUT_CONTEXT_DIALOG);
    g_pExoInput->AddEvent(41, INPUT_CONTEXT_FREE_LOOK);
    g_pExoInput->CreateNewEvent(42, CExoInput::EVENT_TYPE_BUTTON, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_BUTTON3, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(42, INPUT_CONTEXT_PC);
    g_pExoInput->AddEvent(42, INPUT_CONTEXT_PC_GUI);
    g_pExoInput->AddEvent(42, INPUT_CONTEXT_DIALOG);
    g_pExoInput->AddEvent(42, INPUT_CONTEXT_FREE_LOOK);
    g_pExoInput->CreateNewEvent(43, CExoInput::EVENT_TYPE_BUTTON, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_BUTTON4, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(43, INPUT_CONTEXT_PC_GUI);
    g_pExoInput->AddEvent(43, INPUT_CONTEXT_DIALOG);
    g_pExoInput->CreateNewEvent(44, CExoInput::EVENT_TYPE_BUTTON, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_BUTTON5, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(44, INPUT_CONTEXT_PC_GUI);
    g_pExoInput->AddEvent(44, INPUT_CONTEXT_DIALOG);
    g_pExoInput->CreateNewEvent(46, CExoInput::EVENT_TYPE_BUTTON, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_BUTTON9, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(46, INPUT_CONTEXT_PC_GUI);
    g_pExoInput->AddEvent(46, INPUT_CONTEXT_DIALOG);
    g_pExoInput->CreateNewEvent(53, CExoInput::EVENT_TYPE_BUTTON, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_BUTTON6, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(53, INPUT_CONTEXT_PC_GUI);
    g_pExoInput->AddEvent(53, INPUT_CONTEXT_DIALOG);
    g_pExoInput->SetEventRepeatable(53, 500, 40);
    g_pExoInput->CreateNewEvent(54, CExoInput::EVENT_TYPE_BUTTON, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_BUTTON7, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(54, INPUT_CONTEXT_PC_GUI);
    g_pExoInput->AddEvent(54, INPUT_CONTEXT_DIALOG);
    g_pExoInput->SetEventRepeatable(54, 500, 40);
    g_pExoInput->CreateNewEvent(15, CExoInput::EVENT_TYPE_BUTTON, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_BUTTON14, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(15, INPUT_CONTEXT_PC);
    g_pExoInput->AddEvent(15, INPUT_CONTEXT_MINIGAME);
    g_pExoInput->CreateNewEvent(1, CExoInput::EVENT_TYPE_BUTTON, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_BUTTON15, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(1, INPUT_CONTEXT_PC);
    g_pExoInput->AddEvent(1, INPUT_CONTEXT_FREE_LOOK);
    g_pExoInput->CreateNewEvent(9, CExoInput::EVENT_TYPE_BUTTON, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_BUTTON4, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(9, INPUT_CONTEXT_PC);
    g_pExoInput->AddEvent(9, INPUT_CONTEXT_FREE_LOOK);
    g_pExoInput->CreateNewEvent(10, CExoInput::EVENT_TYPE_BUTTON, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_BUTTON9, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(10, INPUT_CONTEXT_PC);
    g_pExoInput->AddEvent(10, INPUT_CONTEXT_FREE_LOOK);
    g_pExoInput->CreateNewEvent(6, CExoInput::EVENT_TYPE_BUTTON, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_BUTTON6, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(6, INPUT_CONTEXT_PC);
    g_pExoInput->AddEvent(6, INPUT_CONTEXT_FREE_LOOK);
    g_pExoInput->SetEventRepeatable(6, 500, 60);
    g_pExoInput->CreateNewEvent(5, CExoInput::EVENT_TYPE_BUTTON, CExoInput::INPUTDEVICE_JOYSTICKSTART, CExoInput::JOYSTICK_BUTTON7, CExoInput::INPUT_CONTROL_NONE);
    g_pExoInput->AddEvent(5, INPUT_CONTEXT_PC);
    g_pExoInput->AddEvent(5, INPUT_CONTEXT_FREE_LOOK);
    g_pExoInput->SetEventRepeatable(5, 500, 60);
}

int CClientExoAppInternal::SetTexturePack(unsigned char nTexturePack, int nUnknown)
{
    switch (nTexturePack)
    {
    case 0:
    case 1:
        AurTextureSetDynamicMemoryRatio(0.625f);
        AurTextureSetMem(0xe00000);
        break;
    case 2:
        AurTextureSetDynamicMemoryRatio(0.75f);
        AurTextureSetMem(0x1e00000);
        break;
    case 3:
        AurTextureSetDynamicMemoryRatio(0.625f);
        AurTextureSetMem(0x6000000);
        break;
    default:
        return 0;
    }
    m_nTexturePack = nTexturePack;
    return 1;
}

int CClientExoAppInternal::AddPatchFiles(int bRemoveFirst)
{
    CExoIni cIni;
    CExoString sValue;
    CExoString sKey;
    CExoString sFile;
    int bResult = 1;
    int nIndex;

    sKey.Format("%s%03i", "PatchFile", 0);
    nIndex = 1;
    while (cIni.ReadIniEntry(sValue, "d:\\nwnpatch.ini", "Patch", sKey))
    {
        sFile = "PATCH:";
        sFile = sFile + sValue;
        if (bRemoveFirst)
            bResult &= g_pExoResMan->RemoveEncapsulatedResourceFile(sFile);
        bResult &= g_pExoResMan->AddEncapsulatedResourceFile(sFile, 2);
        sKey.Format("%s%03i", "PatchFile", nIndex);
        nIndex++;
    }
    return bResult;
}

void CClientExoAppInternal::InitializeClientOptions()
{
    CExoIni cIni;
    CExoString sValue;

    if (cIni.ReadIniEntry(sValue, "d:\\swplayer.ini", "Game Options", "Memory Level") == 1)
    {
        if (sValue.AsINT() != 0)
        {
          if (cIni.ReadIniEntry(sValue, "d:\\swplayer.ini", "Game Options", "Memory Access") == 1)
          {
            switch (sValue.AsINT())
            {
            case 0:
                m_pClientOptions->SetGoreLevel(0);
                break;
            case 1:
                m_pClientOptions->SetGoreLevel(1);
                break;
            case 2:
                m_pClientOptions->SetGoreLevel(2);
                break;
            default:
                m_pClientOptions->SetGoreLevel(0);
                sValue.Format("%d", 0);
                cIni.WriteIniEntry(sValue, "d:\\swplayer.ini", "Game Options", "Memory Access");
                break;
            }
          }
          else
          {
            m_pClientOptions->SetGoreLevel(0);
          }
        }
        else
        {
            m_pClientOptions->SetGoreLevel(0);
        }
    }
    else
    {
        if (g_pTlkTable != NULL)
        {
            if (g_pTlkTable->GetLanguageVersion() == 0)
            {
                sValue.Format("%d", 1);
                cIni.WriteIniEntry(sValue, "d:\\swplayer.ini", "Game Options", "Memory Level");
                sValue.Format("%d", 1);
                cIni.WriteIniEntry(sValue, "d:\\swplayer.ini", "Game Options", "Memory Access");
                m_pClientOptions->SetGoreLevel(1);
            }
            else
            {
                sValue.Format("%d", 0);
                cIni.WriteIniEntry(sValue, "d:\\swplayer.ini", "Game Options", "Memory Level");
                sValue.Format("%d", 0);
                cIni.WriteIniEntry(sValue, "d:\\swplayer.ini", "Game Options", "Memory Access");
                m_pClientOptions->SetGoreLevel(0);
            }
        }
        else
        {
            sValue.Format("%d", 0);
            cIni.WriteIniEntry(sValue, "d:\\swplayer.ini", "Game Options", "Memory Level");
            sValue.Format("%d", 0);
            cIni.WriteIniEntry(sValue, "d:\\swplayer.ini", "Game Options", "Memory Access");
            m_pClientOptions->SetGoreLevel(0);
        }
    }

    if (cIni.ReadIniEntry(sValue, "d:\\swplayer.ini", "Control Options", "Zoom Speed"))
    {
        float fValue = sValue.AsFLOAT();
        if (fValue >= 1.0f && fValue <= 10.0f)
        {
            m_pClientOptions->SetZoomSpeed(fValue);
        }
        else
        {
            m_pClientOptions->SetZoomSpeed(3.0f);
        }
    }
    else
    {
        sValue.Format("%f", 3.0);
        cIni.WriteIniEntry(sValue, "d:\\swplayer.ini", "Control Options", "Zoom Speed");
    }

    if (cIni.ReadIniEntry(sValue, "d:\\swplayer.ini", "Control Options", "CameraTurnSpeed"))
    {
        float fValue = sValue.AsFLOAT();
        if (fValue >= 1.0f && fValue <= 10.0f)
        {
            m_pClientOptions->SetCameraTurnSpeed(fValue);
        }
        else
        {
            m_pClientOptions->SetCameraTurnSpeed(1.0f);
        }
    }
    else
    {
        sValue.Format("%f", 1.0);
        cIni.WriteIniEntry(sValue, "d:\\swplayer.ini", "Control Options", "CameraTurnSpeed");
    }

    if (cIni.ReadIniEntry(sValue, "d:\\swplayer.ini", "Game Options", "TooltipDelay uSec"))
    {
        if (sValue.AsINT() <= 0)
        {
            sValue.Format("1");
            cIni.WriteIniEntry(sValue, "d:\\swplayer.ini", "Game Options", "TooltipDelay uSec");
        }
    }
    else
    {
        cIni.WriteIniEntry(sValue, "d:\\swplayer.ini", "Game Options", "TooltipDelay uSec");
    }

    if (cIni.ReadIniEntry(sValue, "d:\\swplayer.ini", "Game Options", "Hide SecondStoryTiles"))
    {
        int nValue = sValue.AsINT();
        if (nValue >= 0 && nValue <= 2)
        {
            m_pClientOptions->SetGeometryFadeMode(nValue);
        }
        else
        {
            m_pClientOptions->SetGeometryFadeMode(0);
            sValue.Format("%d", 0);
            cIni.WriteIniEntry(sValue, "d:\\swplayer.ini", "Game Options", "Hide SecondStoryTiles");
        }
    }
    else
    {
        m_pClientOptions->SetGeometryFadeMode(0);
        sValue.Format("%d", 0);
        cIni.WriteIniEntry(sValue, "d:\\swplayer.ini", "Game Options", "Hide SecondStoryTiles");
    }

    if (cIni.ReadIniEntry(sValue, "d:\\swkotor2.ini", "Video Options", "AntiAliasing Mode"))
    {
        int nValue = sValue.AsINT();
        if (nValue >= 0 && nValue <= 4)
        {
            m_pClientOptions->SetAntiAliasingMode(nValue);
        }
        else
        {
            m_pClientOptions->SetAntiAliasingMode(0);
            sValue.Format("%d", 0);
            cIni.WriteIniEntry(sValue, "d:\\swkotor2.ini", "Video Options", "AntiAliasing Mode");
        }
    }
    else
    {
        m_pClientOptions->SetAntiAliasingMode(0);
        sValue.Format("%d", 0);
        cIni.WriteIniEntry(sValue, "d:\\swkotor2.ini", "Video Options", "AntiAliasing Mode");
    }

    if (cIni.ReadIniEntry(sValue, "d:\\swkotor2.ini", "Video Options", "Enable VSync"))
    {
        CClientOptions* pClientOptions = m_pClientOptions;
        pClientOptions->SetVSyncEnabled(sValue.AsINT() != 0);
    }
    else
    {
        m_pClientOptions->SetVSyncEnabled(0);
        sValue.Format("%d", 0);
        cIni.WriteIniEntry(sValue, "d:\\swkotor2.ini", "Video Options", "Enable VSync");
    }

    if (m_pClientOptions->GetShinyWaterSupported())
    {
        if (cIni.ReadIniEntry(sValue, "d:\\swkotor2.ini", "Video Options", "ShinyWater"))
        {
            CClientOptions* pClientOptions = m_pClientOptions;
            pClientOptions->SetShinyWater(sValue.AsINT() != 0);
        }
        else
        {
            m_pClientOptions->SetShinyWater(0);
            sValue.Format("%d", 0);
            cIni.WriteIniEntry(sValue, "d:\\swkotor2.ini", "Video Options", "ShinyWater");
        }
    }
    else
    {
        m_pClientOptions->SetShinyWater(0);
        sValue.Format("%d", 0);
        cIni.WriteIniEntry(sValue, "d:\\swkotor2.ini", "Video Options", "ShinyWater");
    }

    if (cIni.ReadIniEntry(sValue, "d:\\swkotor2.ini", "Video Options", "NumShadowCastingLights"))
    {
        int nValue = sValue.AsINT();
        if (nValue >= 1 && nValue <= 3)
        {
            m_pClientOptions->SetNumShadowCastingLights(nValue);
        }
        else
        {
            m_pClientOptions->SetNumShadowCastingLights(3);
            sValue.Format("%d", 3);
            cIni.WriteIniEntry(sValue, "d:\\swkotor2.ini", "Video Options", "NumShadowCastingLights");
        }
    }
    else
    {
        m_pClientOptions->SetNumShadowCastingLights(3);
        sValue.Format("%d", 3);
        cIni.WriteIniEntry(sValue, "d:\\swkotor2.ini", "Video Options", "NumShadowCastingLights");
    }

    if (cIni.ReadIniEntry(sValue, "d:\\swkotor2.ini", "Video Options", "EnableVisualEffectsHigh"))
    {
        CClientOptions* pClientOptions = m_pClientOptions;
        pClientOptions->SetVisualEffectsHigh((sValue.AsINT() != 0));
    }
    else
    {
        m_pClientOptions->SetVisualEffectsHigh(1);
        sValue.Format("%d", 1);
        cIni.WriteIniEntry(sValue, "d:\\swkotor2.ini", "Video Options", "EnableVisualEffectsHigh");
    }

    if (cIni.ReadIniEntry(sValue, "d:\\swkotor2.ini", "Video Options", "Enable Truform"))
    {
        CClientOptions* pClientOptions = m_pClientOptions;
        pClientOptions->SetTruform(sValue.AsINT() != 0);
    }
    else
    {
        m_pClientOptions->SetTruform(0);
        sValue.Format("%d", 0);
        cIni.WriteIniEntry(sValue, "d:\\swkotor2.ini", "Video Options", "Enable Truform");
    }

    if (cIni.ReadIniEntry(sValue, "d:\\swkotor2.ini", "Video Options", "Enable AnisotropicFiltering"))
    {
        CClientOptions* pClientOptions = m_pClientOptions;
        pClientOptions->SetAnisotropicFiltering(sValue.AsINT() != 0);
    }
    else
    {
        m_pClientOptions->SetAnisotropicFiltering(0);
        sValue.Format("%d", 0);
        cIni.WriteIniEntry(sValue, "d:\\swkotor2.ini", "Video Options", "Enable AnisotropicFiltering");
    }

    if (cIni.ReadIniEntry(sValue, "d:\\swkotor2.ini", "Video Options", "CreatureWindSetting"))
    {
        int nWind = sValue.AsINT();
        switch (nWind)
        {
        case 0:
            break;
        case CClientOptions::WIND_PLAYER:
            break;
        case CClientOptions::WIND_ALL:
            break;
        default:
            nWind = CClientOptions::WIND_PLAYER;
            sValue.Format("%d", CClientOptions::WIND_PLAYER);
            cIni.WriteIniEntry(sValue, "d:\\swkotor2.ini", "Video Options", "CreatureWindSetting");
            break;
        }
        m_pClientOptions->SetWindMode(nWind);
    }
    else
    {
        m_pClientOptions->SetWindMode(CClientOptions::WIND_PLAYER);
        sValue.Format("%d", CClientOptions::WIND_PLAYER);
        cIni.WriteIniEntry(sValue, "d:\\swkotor2.ini", "Video Options", "CreatureWindSetting");
    }

    if (!cIni.ReadIniEntry(sValue, "d:\\swplayer.ini", "Profile", "ShowMessageOfTheDay"))
    {
        sValue.Format("%d", 1);
        cIni.WriteIniEntry(sValue, "d:\\swplayer.ini", "Profile", "ShowMessageOfTheDay");
    }

    if (cIni.ReadIniEntry(sValue, "d:\\swplayer.ini", "Control Options", "Enable ScreenEdgeCameraTurn"))
    {
        CClientOptions* pClientOptions = m_pClientOptions;
        pClientOptions->SetScreenEdgeCameraTurn((sValue.AsINT() != 0));
    }
    else
    {
        sValue.Format("%d", 1);
        cIni.WriteIniEntry(sValue, "d:\\swplayer.ini", "Control Options", "Enable ScreenEdgeCameraTurn");
    }

    if (cIni.ReadIniEntry(sValue, "d:\\swkotor2.ini", "Video Options", "EnableGrass"))
    {
        CClientOptions* pClientOptions = m_pClientOptions;
        pClientOptions->SetGrassEnabled(sValue.AsINT() != 0);
    }
    else
    {
        m_pClientOptions->SetGrassEnabled(1);
        sValue.Format("%d", 1);
        cIni.WriteIniEntry(sValue, "d:\\swkotor2.ini", "Video Options", "EnableGrass");
    }

    if (cIni.ReadIniEntry(sValue, "d:\\swkotor2.ini", "Video Options", "EnableFastGrass"))
    {
        CClientOptions* pClientOptions = m_pClientOptions;
        pClientOptions->SetFastGrass(sValue.AsINT() != 0);
    }
    else
    {
        m_pClientOptions->SetFastGrass(1);
        sValue.Format("%d", 1);
        cIni.WriteIniEntry(sValue, "d:\\swkotor2.ini", "Video Options", "EnableFastGrass");
    }
}

#include "aurconsole.h"
#include "LaunchData.h"
#define KOTOR_XBOX_TITLE_ID 0x4c410003
#define CONTENT_SIGNATURE_SIZE 20
#define STRREF_CHECKING_CONTENT 47939
int g_bAcceptCorruptDownloads = 0;
ConsoleFunc ConVg_bAcceptCorruptDownloads("g_bAcceptCorruptDownloads", &g_bAcceptCorruptDownloads);
void CClientExoAppInternal::MountDownloadedContent(int bRemove, int bShowProgress)
{
    HRESULT hr = S_OK;
    CExoString sValue;
    CExoIni cIni;
    int nProgress = 0;
    HANDLE hFind;
    char szPath[128];
    char szName[128];
    WIN32_FIND_DATA cFindData;
    XCONTENT_FIND_DATA cContentData;
    BYTE aSignature[2000];

    int bLocal = cIni.ReadIniEntry(sValue, "d:\\swkotor2.ini", "Game Options", "UseLocalContent");
    if (bLocal && sValue.AsINT())
    {
        hFind = FindFirstFile("d:\\live*", &cFindData);
        if (hFind != INVALID_HANDLE_VALUE)
        {
            do
            {
                CExoString sAlias("DOWNLOADS:");
                g_pExoBase->m_pExoAliasList->ChangePath("DOWNLOADS", "d:\\");
                CExoString sDirectory = sAlias + cFindData.cFileName;
                sprintf(szPath, "D:\\%s", cFindData.cFileName);
                if (bRemove == 1)
                {
                    g_pExoResMan->RemoveResourceDirectory(sDirectory);
                }
                else
                {
                    g_pExoResMan->AddResourceDirectory(sDirectory);
                    CExoArrayList<CExoString> lstDirectories;
                    g_pExoBase->GetDirectoryList(&lstDirectories, sDirectory, 0xffff, 1, 0);
                    for (int nDirectory = 0; nDirectory < lstDirectories.num; nDirectory++)
                    {
                        if (!lstDirectories.element[nDirectory].CompareNoCase("modules"))
                        {
                            g_pExoResMan->AddResourceDirectory(sDirectory + "\\" +
                                                               lstDirectories.element[nDirectory]);
                        }
                    }
                    CExoString sLive = CExoString("Live") + sDirectory.Right(1);
                    g_pExoBase->m_pExoAliasList->Add(sLive, szPath);
                    int nPackage = atoi(sDirectory.Right(1).CStr());
                    m_aLiveContentNames[nPackage].m_bCorrupt = 0;
                    m_aLiveContentNames[nPackage].m_bExists = 1;
                    m_aLiveContentNames[nPackage].m_sName = sLive;
                }
            } while (FindNextFile(hFind, &cFindData));
            FindClose(hFind);
        }
    }
    else
    {
        hFind = XFindFirstContent("T:\\", 0xFFFFFFFF, &cContentData);
        if (hFind == INVALID_HANDLE_VALUE)
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
        }
        else
        {
            if (bShowProgress)
            {
                SetLoadBarProgress(0, 0);
                SetLoadScreenByModuleName("default");
                if (m_pGuiLoadScreen != NULL)
                {
                    m_pGuiLoadScreen->SetHint(STRREF_CHECKING_CONTENT);
                }
                ShowLoadScreen(1, 0, 0);
            }
            do
            {
                int nPackage = (int)(cContentData.qwOfferingId & 0xf);
                if (bShowProgress)
                {
                    nProgress = 0;
                    SetLoadBarProgress(0, 0);
                    UpdateScreen(1.0f / 30.0f, 0, 0);
                }
                int bCorrupt = 1;
                int nVerified = 0;
                int nFailed = 0;
                if (!XGetContentInstallLocationFromIDs(KOTOR_XBOX_TITLE_ID, cContentData.qwOfferingId, szPath))
                {
                    hr = HRESULT_FROM_WIN32(GetLastError());
                }
                if (SUCCEEDED(hr))
                {
                    HANDLE hSignatures = XLoadContentSignatures(szPath);
                    if (hSignatures == NULL)
                    {
                        hr = HRESULT_FROM_WIN32(GetLastError());
                    }
                    else
                    {
                        CExoString sAlias("DOWNLOADS:");
                        CExoString sRoot("T:\\");
                        if (_strnicmp(szPath, sRoot.CStr(), sRoot.GetLength()) == 0)
                        {
                            CExoString sDirectory = sAlias + (szPath + 3);
                            if (bRemove == 1)
                            {
                                g_pExoResMan->RemoveResourceDirectory(sDirectory);
                            }
                            else
                            {
                                g_pExoResMan->AddResourceDirectory(sDirectory);
                                CExoArrayList<CExoString> lstFiles;
                                g_pExoBase->GetDirectoryList(&lstFiles, sDirectory, 0xffff, 0, 0);
                                CExoArrayList<CExoString> lstDirectories;
                                g_pExoBase->GetDirectoryList(&lstDirectories, sDirectory, 0xffff, 1, 0);
                                int nDirectory;
                                for (nDirectory = 0; nDirectory < lstDirectories.num; nDirectory++)
                                {
                                    CExoArrayList<CExoString> lstSubFiles;
                                    if (bShowProgress)
                                    {
                                        nProgress += 30 / lstDirectories.num;
                                        SetLoadBarProgress(nProgress, 0);
                                        UpdateScreen(1.0f / 30.0f, 0, 0);
                                    }
                                    if (g_pExoBase->GetDirectoryList(
                                            &lstSubFiles,
                                            sDirectory + "\\" + lstDirectories.element[nDirectory], 0xffff,
                                            0, 0))
                                    {
                                        for (int nSubFile = 0; nSubFile < lstSubFiles.num; nSubFile++)
                                        {
                                            lstFiles.Add(lstDirectories.element[nDirectory] + "\\" +
                                                         lstSubFiles.element[nSubFile]);
                                        }
                                    }
                                }
                                if (g_nBootFlags & (1 << nPackage))
                                {
                                    nFailed = 1;
                                }
                                if (bShowProgress)
                                {
                                    for (int nFile = 0; nFile < lstFiles.num; nFile++)
                                    {
                                        if (!lstFiles.element[nFile].CompareNoCase("contentmeta.xbx"))
                                        {
                                            nProgress += 70 / lstFiles.num;
                                            SetLoadBarProgress(nProgress, 0);
                                            UpdateScreen(1.0f / 30.0f, 0, 0);
                                            CExoFile cFile(sDirectory + "\\" + lstFiles.element[nFile], 0xffff,
                                                           "rb");
                                            int nSize = cFile.GetSize();
                                            LPBYTE pFileSignature = NULL;
                                            DWORD nFileSignatureSize = CONTENT_SIGNATURE_SIZE;
                                            if (nSize < 0 ||
                                                !XLocateSignatureByName(hSignatures,
                                                                        lstFiles.element[nFile].CStr(), 0, 0,
                                                                        &pFileSignature, &nFileSignatureSize))
                                            {
                                                GetLastError();
                                                nFailed++;
                                                break;
                                            }
                                            LPBYTE pData = new BYTE[nSize];
                                            cFile.Read(pData, nSize, 1);
                                            DWORD nSignatureSize = CONTENT_SIGNATURE_SIZE;
                                            BOOL bCalculated =
                                                XCalculateContentSignature(pData, nSize, aSignature, &nSignatureSize);
                                            if (!bCalculated || nSignatureSize != CONTENT_SIGNATURE_SIZE)
                                            {
                                                GetLastError();
                                            }
                                            delete[] pData;
                                            if (!bCalculated || nSignatureSize != nFileSignatureSize ||
                                                memcmp(aSignature, pFileSignature, nSignatureSize) != 0)
                                            {
                                                nFailed++;
                                                break;
                                            }
                                            nVerified++;
                                        }
                                    }
                                    if (!nFailed)
                                    {

                                        LPBYTE pExtraSignature = NULL;
                                        DWORD nExtraSignatureSize = CONTENT_SIGNATURE_SIZE;
                                        nFailed = XLocateSignatureByIndex(hSignatures, nVerified, &pExtraSignature,
                                                                          &nExtraSignatureSize);
                                    }
                                }
                                if (nFailed && !g_bAcceptCorruptDownloads)
                                {
                                    g_pExoResMan->RemoveResourceDirectory(sDirectory);
                                }
                                else
                                {
                                    bCorrupt = 0;
                                    for (nDirectory = 0; nDirectory < lstDirectories.num; nDirectory++)
                                    {
                                        if (!lstDirectories.element[nDirectory].CompareNoCase("modules"))
                                        {
                                            g_pExoResMan->AddResourceDirectory(
                                                sDirectory + "\\" + lstDirectories.element[nDirectory]);
                                        }
                                    }
                                    CExoString sLive = CExoString("Live") + sDirectory.Right(1);
                                    g_pExoBase->m_pExoAliasList->Add(sLive, szPath);
                                }
                            }
                        }
                        XCloseContentSignatures(hSignatures);
                    }
                }
                if (bRemove != 1)
                {
                    int nLength = wcslen(cContentData.szDisplayName);
                    unsigned int nOut = 0;
                    for (int nChar = 0; nChar < nLength && nOut < sizeof(szName) - 1; nChar++)
                    {
                        int nBytes = wctomb(&szName[nOut], cContentData.szDisplayName[nChar]);
                        if (nBytes >= 0)
                        {
                            nOut += nBytes;
                        }
                        else
                        {
                            szName[nOut++] = 1;
                        }
                    }
                    m_aLiveContentNames[nPackage].m_bCorrupt = bCorrupt;
                    m_aLiveContentNames[nPackage].m_bExists = 1;
                    szName[nOut] = 0;
                    m_aLiveContentNames[nPackage].m_sName.Format("%s", szName);
                }
                g_nBootFlags |= (unsigned char)bCorrupt << nPackage;
                if (bShowProgress)
                {
                    SetLoadBarProgress(100, 0);
                    UpdateScreen(1.0f / 30.0f, 0, 0);
                }
            } while (XFindNextContent(hFind, &cContentData));
            CloseHandle(hFind);
            if (bShowProgress)
            {
                HideLoadScreen(1);
            }
        }
    }
}

#define XBOX_DEFAULT_SOUND_VOICES 64
void CClientExoAppInternal::InitializeSoundOptions()
{
    CExoIni cIni;
    CExoString sProvider;
    CExoString sVolume;
    CExoString s3DVoices;
    CExoString s2DVoices;
    CExoString sSpeakerType;
    CExoString sEnvironmentEffects;
    CExoString sEnvironmentEffectsLevel;
    CExoString s2D3DBias;
    unsigned char n2DVoices = XBOX_DEFAULT_SOUND_VOICES;
    unsigned char n3DVoices = XBOX_DEFAULT_SOUND_VOICES;

    if (cIni.ReadIniEntry(s2DVoices, "d:\\swkotor2.ini", "Sound Options", "Number 2D Voices"))
        n2DVoices = s2DVoices.AsINT();
    if (cIni.ReadIniEntry(s3DVoices, "d:\\swkotor2.ini", "Sound Options", "Number 3D Voices"))
        n3DVoices = s3DVoices.AsINT();

    int bProvider = cIni.ReadIniEntry(sProvider, "d:\\swkotor2.ini", "Sound Options", "3D Provider");
    if (bProvider)
        g_pExoSound = new CExoSound(n2DVoices, n3DVoices, sProvider);
    else
        g_pExoSound = new CExoSound(n2DVoices, n3DVoices, "DirectSound3D Hardware Support");

    if (cIni.ReadIniEntry(sVolume, "d:\\swkotor2.ini", "Sound Options", "Voice Volume"))
        g_pExoSound->SetDialogVolume(sVolume.AsFLOAT());
    if (cIni.ReadIniEntry(sVolume, "d:\\swkotor2.ini", "Sound Options", "Music Volume"))
        g_pExoSound->SetMusicVolume(sVolume.AsFLOAT(), 0);
    if (cIni.ReadIniEntry(sVolume, "d:\\swkotor2.ini", "Sound Options", "SoundFX Volume"))
        g_pExoSound->SetSoundEffectVolume(sVolume.AsFLOAT());
    if (cIni.ReadIniEntry(sSpeakerType, "d:\\swkotor2.ini", "Sound Options", "Speaker Type"))
        g_pExoSound->SetSpeakerType(sSpeakerType.AsINT());
    if (cIni.ReadIniEntry(sEnvironmentEffects, "d:\\swkotor2.ini", "Sound Options",
                          "Environment Effects"))
        g_pExoSound->EnableEnvironmentEffects(sEnvironmentEffects.AsINT());
    if (cIni.ReadIniEntry(sEnvironmentEffectsLevel, "d:\\swkotor2.ini", "Sound Options",
                          "Environment Effects Level"))
        g_pExoSound->SetEnvironmentEffectLevel(sEnvironmentEffectsLevel.AsFLOAT());
    if (cIni.ReadIniEntry(s2D3DBias, "d:\\swkotor2.ini", "Sound Options", "2D3D Bias"))
        g_pExoSound->Set2D3DBias(s2D3DBias.AsFLOAT());
}

void CClientExoAppInternal::AddDownloadedResources()
{
    EXOXBOXASSERT(g_pExoResMan);
    CExoString sResourceName;
    CExoString sAlias;
    for (int nPackage = 1; nPackage < CTlkTable::NUM_CONTENT_PKGS; ++nPackage)
    {
        sResourceName.Format("live%d", nPackage);
        sAlias.Format("LIVE%d", nPackage);
        if (g_pExoBase->m_pExoAliasList->GetAliasPath(sAlias, 0).GetLength())
        {
            if (g_pExoResMan->Exists(sResourceName, RESTYPE_TLK, (unsigned long*)NULL))
            {
                sAlias.Format("LIVE%d:live%d", nPackage, nPackage);
                g_pTlkTable->AddFile(sAlias, nPackage);
            }
            if (g_pExoResMan->Exists(sResourceName, 9999, (unsigned long*)NULL))
            {
                sAlias.Format("LIVE%d:live%d", nPackage, nPackage);
                g_pExoResMan->AddFixedKeyTableFile(sAlias);
            }
            if (g_pExoResMan->Exists(sResourceName, RESTYPE_RIM, (unsigned long*)NULL))
            {
                sAlias.Format("LIVE%d:RIMSXBOX\\live%d", nPackage, nPackage);
                g_pExoResMan->AddResourceImageFile(sAlias);
            }
            if (g_pExoResMan->Exists(sResourceName, RESTYPE_MOD, (unsigned long*)NULL))
            {
                sAlias.Format("LIVE%d:live%d", nPackage, nPackage);
                g_pExoResMan->AddEncapsulatedResourceFile(sAlias, 2);
            }
            CExoString sDXResourceName = sResourceName + "DX";
            if (g_pExoResMan->Exists(sResourceName, RESTYPE_RIM, (unsigned long*)NULL))
            {
                sAlias.Format("LIVE%d:RIMSXBOX\\live%ddx", nPackage, nPackage);
                g_pExoResMan->AddResourceImageFile(sAlias);
            }
            sAlias.Format("LIVE%d:OVERRIDE\\textures", nPackage);
            g_pExoResMan->AddEncapsulatedResourceFile(sAlias, 1);
        }
    }
}

#include "LaunchData.h"
void AurPersistDisplay();
void ExoXboxLaunch(KOTOR_LAUNCH_DATA* pParams, const char* pszModuleName);

#define XBOX_DEATH_SEQUENCE_SPEED_RANGE 0.8f
void CClientExoAppInternal::UpdateDeathSequence()
{
    unsigned long nTime = (unsigned long)(
        g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000);
    unsigned long nElapsed = nTime - m_nDeathSequenceLastTime;
    m_nDeathSequenceLastTime = nTime;

    if (m_nGameOver)
    {
        m_fDeathSequenceElapsed += nElapsed * 0.001f;
        float fElapsed = m_fDeathSequenceElapsed;
        float fScale = m_fDeathSequenceScale;
        float fSpeed = CLIENT_DEATH_SEQUENCE_MIN_SPEED;
        if (m_bGameOverPending)
        {
            m_bGameOverPending = 0;
            fScale = m_fDeathSequenceScale =
                (float)-(log(fElapsed / CLIENT_DEATH_SEQUENCE_DURATION) / XBOX_DEATH_SEQUENCE_SPEED_RANGE);
        }
        if (m_fDeathSequenceElapsed <= CLIENT_DEATH_SEQUENCE_DURATION)
        {
            fSpeed = -(float)log(fElapsed / CLIENT_DEATH_SEQUENCE_DURATION);
            fSpeed /= fScale;
            fSpeed += CLIENT_DEATH_SEQUENCE_MIN_SPEED;
        }
        SetGameSpeed(fSpeed);

        if (!m_pGuiInGame->IsGlobalFading() && m_fEndGamePendingDelay == 0.0f)
        {
            KOTOR_LAUNCH_DATA cParams;
            cParams.m_nModuleTrans = KOTOR_LAUNCH_DATA::MODULETRANS_NONE;
            cParams.m_nAutoCalInfo = g_pExoInput->GetAutoCalInfo();
            cParams.m_nPortBinding = g_pExoInput->GetPortBinding();
            cParams.m_bRenderQA = RenderQA;

            KOTOR_LAUNCH_OPTIONS_DATA cOptionsData;
            g_pAppManager->m_pClientExoApp->GetClientOptions()->GetOptionsData(&cOptionsData);
            cParams.SetOptionsData(cOptionsData);

            SetLoadScreenByModuleName("default");
            SetLoadMusic("");
            SetLoadBarProgress(0, 1);
            if (m_pGuiLoadScreen != NULL)
            {
                m_pGuiLoadScreen->SetHint(0);
            }
            ShowLoadScreen(1, 1, 0);
            UpdateScreen(1.0f / 30.0f, 0, 0);

            cParams.SetLoadScreen("load_default");
            cParams.m_nLoadScreenHint = 0;
            AurPersistDisplay();
            ExoXboxLaunch(&cParams, g_pAppManager->m_pServerExoApp->GetModuleName().CStr());
        }
    }
}
#endif
#ifdef _XBOX

enum
{
    STRREF_XBOX_MAIN_MENU_MESSAGE = 0xBAF4,
    STRREF_XBOX_CORRUPT_CONTENT = 0xBB02
};

void CClientExoAppInternal::AddCorruptContentMessage()
{
    EXOXBOXASSERT(m_pMainMenu != NULL);
    CSWGuiMainMenu::CErrorMessageData cMessage;
    cMessage.m_sMessage = g_pTlkTable->GetSimpleString(STRREF_XBOX_MAIN_MENU_MESSAGE);
    m_pMainMenu->AddErrorMessage(cMessage);
}

void CClientExoAppInternal::DisplayMainMenu(int bScanContent)
{
    if (m_pMainMenu != NULL && m_pSWGuiManager->PanelExists(m_pMainMenu))
    {
        EXOXBOXASSERT(m_pMainMenu->m_nExitState != CSWGuiPanel::EXITSTATE_DELETE);
        return;
    }

    if (m_pStartupPanel != NULL)
    {
        if (m_pSWGuiManager->PanelExists(m_pStartupPanel))
        {
            CSWGuiPanel* pPanel = m_pStartupPanel;
            pPanel->m_bDeferredRemoval = 0;
            pPanel->m_nExitState = CSWGuiPanel::EXITSTATE_DELETE;
        }
        else
        {
            delete m_pStartupPanel;
        }
        m_pStartupPanel = NULL;
    }

    m_pMainMenu = new CSWGuiMainMenu(m_pSWGuiManager);
    SetInputClass(INPUT_CLASS_GUI);
    m_pSWGuiManager->AddPanel(m_pMainMenu,
                             CSWGuiManager::PANEL_MODE_FOREGROUND, 1);
    if (bScanContent)
    {
        MountDownloadedContent(0, bScanContent);
        unsigned char nCorrupt = 0;
        CExoString sCorruptContent;
        for (int nPackage = 1; nPackage < CTlkTable::NUM_CONTENT_PKGS; ++nPackage)
        {
            if (g_pAppManager->m_pClientExoApp->GetLiveContentExists(nPackage) &&
                g_pAppManager->m_pClientExoApp->GetLiveContentCorrupt(nPackage))
            {
                if (nCorrupt)
                {
                    sCorruptContent = sCorruptContent + ", ";
                }
                sCorruptContent = sCorruptContent + g_pAppManager->m_pClientExoApp->GetLiveContentName(nPackage);
                nCorrupt++;
            }
        }
        if (nCorrupt > 0)
        {
            CSWGuiMainMenu::CErrorMessageData cMessage;
            g_pTlkTable->SetCustomToken(0, sCorruptContent);
            cMessage.m_sMessage = g_pTlkTable->GetSimpleString(STRREF_XBOX_CORRUPT_CONTENT);
            g_pTlkTable->ParseStr(cMessage.m_sMessage);
            m_pMainMenu->AddErrorMessage(cMessage);
        }
    }
    StartMenuMusic(1, MENU_MUSIC_VOLUME, MENU_MUSIC_PRIORITY_GROUP);
    if (m_bMainMenuTransition == 1)
    {
        AddCorruptContentMessage();
        m_bMainMenuTransition = 0;
    }
    m_pGuiInGame->SetGlobalUnpauseableState(0);
}
#endif
#ifdef _XBOX

void CClientExoAppInternal::TurnOffKeyboardModal()
{
    for (unsigned int i = 0; i < 7; i++)
    {
        if (m_anKeyboardModal[i] != 0)
        {
            for (unsigned int j = 0; j < 7; j++)
                m_anKeyboardModal[j] = 0;
            m_nUnknownFC = 1;
            m_nUnknownF4 = 0;
            m_nUnknownF8 = 0;
            m_pSWCMessage->SendPlayerToServerInput_AbortDriveControl();
            break;
        }
    }

    if (m_pModule != NULL && m_pModule->m_pInfo != NULL)
    {
        m_pModule->m_pInfo->HiliteAreaObjects(0);
    }
}
#endif
#ifdef _XBOX

CExoString CClientExoAppInternal::GetLoadMusicByModuleName(const CExoString& sModule)
{
    CExoString sMusicResRef;
    {
        CExoString sColumn("MusicResRef");
        CExoString sDefault("DEFAULT");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pLoadscreens = p2DArrays->m_pAreaTransition;
        pLoadscreens->GetCExoStringEntry(sDefault, sColumn, &sMusicResRef);
    }
    return sMusicResRef;
}
#endif
#ifdef _XBOX

#define XBOX_END_GAME_INPUT_LOCK_MS 500
int CClientExoAppInternal::MainLoop()
{
    int bAreaReady = 0;
    static int a;

    if (m_pGuiInGame != NULL)
        m_pGuiInGame->HideAreaTransition();

    if (GetAreaNotReadyToDisplay() != 0 && m_pModule != NULL && GetPlayerCreature() != NULL)
    {
        if (!m_pModule->GetMiniGame() && m_oidFutureLeader != OBJECT_INVALID)
            CrownFutureLeader();
        else
        {
            if (m_pModule->GetMiniGame())
                SetInputClass(INPUT_CLASS_MINIGAME, 1);
            else
            {
                SetInputClass(INPUT_CLASS_GAME, 1);
                if (m_pPlayerControl != NULL)
                    m_pPlayerControl->SetEnabled(1);
                if (m_pGuiInGame != NULL)
                    m_pGuiInGame->SetSWGuiStatus(CGuiInGame::GUISTATUS_SHOW_MAIN_INTERFACE, 1);
                m_pGuiInGame->InitializeMap();
            }
            EnableInput();
            m_pModule->GetModuleTitle();
            WaitForMovie();
            HideLoadScreen(1);
            m_pCombatTimer->PauseWorldTimer();
            m_pPauseTimer->PauseWorldTimer();
            m_pAnimationTimer->PauseWorldTimer();
            m_pCombatTimer->ResetTimer(GetWorldTimer());
            m_pPauseTimer->ResetTimer(GetWorldTimer());
            m_pAnimationTimer->ResetTimer(GetWorldTimer());
            m_pWorldTimer->SetSnapshotTime();
            m_pCombatTimer->SetSnapshotTime();
            m_pPauseTimer->SetSnapshotTime();
            m_pAnimationTimer->SetSnapshotTime();
            CWorldTimer* pTimer = m_pPauseTimer;
            pTimer->m_nPreviousSnapshotTime = pTimer->GetSnapshotTime();
            pTimer = m_pAnimationTimer;
            pTimer->m_nPreviousSnapshotTime = pTimer->GetSnapshotTime();
            pTimer = m_pCombatTimer;
            pTimer->m_nPreviousSnapshotTime = pTimer->GetSnapshotTime();
            pTimer = m_pWorldTimer;
            pTimer->m_nPreviousSnapshotTime = pTimer->GetSnapshotTime();
            m_pWorldTimer->UnpauseWorldTimer();
            m_pCombatTimer->UnpauseWorldTimer();
            m_pPauseTimer->UnpauseWorldTimer();
            m_pAnimationTimer->UnpauseWorldTimer();
            g_pAppManager->GetServerExoApp()->GetWorldTimer()->SetSnapshotTime();
            g_pAppManager->GetServerExoApp()->GetWorldTimer()->UnpauseWorldTimer();
            if (!g_pAppManager->GetServerExoApp()->GetAutoSavePending())
            {
                m_pModule->m_pInfo->StartSounds();
                if (g_pAppManager->GetClientExoApp()->GetInGameGui()->m_nGlobalDialogState ||
                    g_pAppManager->GetClientExoApp()->GetInGameGui()->m_bHoldWorldFadeInForDialog)
                    g_pAppManager->GetClientExoApp()->GetInGameGui()->m_bHoldWorldFadeInForDialog = 1;
                else
                    g_pAppManager->GetClientExoApp()->GetInGameGui()->StartGlobalFade(
                        1, 0.5f, 1.0f, Vector(0.0f, 0.0f, 0.0f));
                SetCanSendDialog(1);
                g_pExoInput->EndAttemptedBind();
                m_pGuiInGame->SetGlobalUnpauseableState(0);
                g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_GENERIC);
            }
            bAreaReady = 1;
            DumpLogCounters(NULL);
        }
    }

    if (GetActivePauseState() == 0)
        m_pWorldTimer->SetSnapshotTime();
    if (GetActivePauseState() != PAUSE_STATE_COMBAT)
        m_pCombatTimer->SetSnapshotTime();
    m_pPauseTimer->SetSnapshotTime();
    CSWReentrantServerStats* pStats = g_pAppManager->m_pReentrantServerStats;
    if (pStats->m_bOperationActive == 1)
    {
        if (!g_pAppManager->GetClientExoApp()->GetCaptureEventPlayBack())
        {
            m_pUnknown10->ProcessReceivedFrames(1);
            m_pUnknown10->UpdateStatusLoop(CNetLayer::APPLICATION_CLIENT);
        }
        StallServerCode();
        pStats = g_pAppManager->m_pReentrantServerStats;
        if (pStats->m_bOperationActive &&
            (pStats->m_nOperation == CSWReentrantServerStats::OPERATION_LOAD_MODULE ||
             pStats->m_nOperation == CSWReentrantServerStats::OPERATION_LOAD_GAME))
        {
            m_nAreaNotReadyToDisplay = 1;
            if (g_pAppManager->m_pReentrantServerStats->m_nUnknown56 == 4)
            {
                DeltaT = (int)m_pWorldTimer->GetSnapshotTimeDifference() / 1000000.0f;
                ProcessInput(DeltaT);
            }
            return m_bExitGame && m_bExitGamePending;
        }
    }
    DeltaT = (int)m_pWorldTimer->GetSnapshotTimeDifference() / 1000000.0f;
    g_pExoResMan->Update(0);
    if (m_pSWParty != NULL && m_pModule != NULL && m_pModule->m_pInfo != NULL)
        m_pSWParty->Update(DeltaT);
    AurMakeCurrent();
    ProcessInput(DeltaT);
    UpdateDeathSequence();
    int bEndGameReady = 1;
    if (m_bEndGamePending && m_fEndGamePendingDelay <= 0.0f)
    {
        if (!m_nUnknown49C)
        {
            m_nUnknown49C = 1;
            g_pExoInput->SetActive(0);
            bEndGameReady = 0;
            g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_MUTE);
            m_nUnknown4A0 = (unsigned long)(g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000);
        }
        else
        {
            bEndGameReady = (unsigned long)(g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000) -
                            m_nUnknown4A0 > XBOX_END_GAME_INPUT_LOCK_MS;
            g_pExoInput->UpdateJoysticks();
            g_pAppManager->GetClientExoApp()->GetInGameGui()->UpdateControllerPauseState();
        }
    }
    if (m_bEndGamePending && bEndGameReady == 1 &&
        !g_pAppManager->GetClientExoApp()->GetInGameGui()->m_bControllerPauseActive)
    {
        if (m_fEndGamePendingDelay > 0.0f)
        {
            if (m_pSWGuiManager->PanelExists(m_pGuiInGame->m_pMessageBox) ||
                !m_nEndGamePendingUnknown)
                m_fEndGamePendingDelay -= DeltaT;
            else
            {
                CGuiInGame* pGui = m_pGuiInGame;
                pGui->m_pMessageBox->DisplayDeathMessage();
            }
        }
        else
        {
            KOTOR_LAUNCH_DATA cParams;
            cParams.m_nModuleTrans = KOTOR_LAUNCH_DATA::MODULETRANS_NONE;
            cParams.m_nAutoCalInfo = g_pExoInput->GetAutoCalInfo();
            cParams.m_nPortBinding = g_pExoInput->GetPortBinding();
            cParams.m_bRenderQA = RenderQA;

            KOTOR_LAUNCH_OPTIONS_DATA cOptionsData;
            g_pAppManager->m_pClientExoApp->GetClientOptions()->GetOptionsData(&cOptionsData);
            cParams.SetOptionsData(cOptionsData);

            SetLoadScreenByModuleName("default");
            SetLoadMusic("");
            SetLoadBarProgress(0, 1);
            if (m_pGuiLoadScreen != NULL)
            {
                m_pGuiLoadScreen->SetHint(0);
            }
            ShowLoadScreen(1, 1, 0);
            UpdateScreen(1.0f / 30.0f, 0, 0);

            cParams.SetLoadScreen("load_default");
            cParams.m_nLoadScreenHint = 0;
            AurPersistDisplay();
            ExoXboxLaunch(&cParams, g_pAppManager->m_pServerExoApp->GetModuleName().CStr());
        }
    }
    int bSlowMotion = m_nUnknown1B0;
    if (bSlowMotion || SlowGame)
        SetGameSpeed(0.25f);
    else
        SetGameSpeed(1.0f);
    if (!g_pAppManager->GetClientExoApp()->GetCaptureEventPlayBack())
    {
        m_pUnknown10->ProcessReceivedFrames(1);
        m_pUnknown10->UpdateStatusLoop(CNetLayer::APPLICATION_CLIENT);
    }
    if (m_bRunScript)
    {
        g_pVirtualMachine->RunScript(&m_sRunScript, 0, 0);
        m_bRunScript = 0;
    }
    if (GetPlayerCreature() != NULL &&
        GetPlayerCreature()->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1) != NULL)
    {
        if (m_fPlayerAnimationTime <= DeltaT)
        {
            m_fPlayerAnimationTime = 0.0f;
            if (m_lstPlayerAnimations.GetHeadPos() == NULL)
                a = 0;
            else
            {
                CExoString sAnimation;
                CQueuedAnim* pAnimation = m_lstPlayerAnimations.RemoveHead();
                float fTime;
                float fLength;
                if (pAnimation != NULL)
                {
                    if (pAnimation->sAnimation.GetLength() > 0)
                        GetPlayerCreature()->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1)->GetAnimation(
                            pAnimation->sAnimation.CStr(), &fTime, &fLength);
                    else
                    {
                        sAnimation = GetPlayerCreature()->GetAnimationBase()->GetAnimationName(pAnimation->nAnimation);
                        GetPlayerCreature()->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1)->GetAnimation(
                            sAnimation.CStr(), &fTime, &fLength);
                    }
                    while (m_lstPlayerAnimations.GetHeadPos() != NULL &&
                           fLength == Gob::ANIMATION_LENGTH_NOT_FOUND)
                    {
                        pAnimation = m_lstPlayerAnimations.RemoveHead();
                        if (pAnimation != NULL)
                            GetPlayerCreature()->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1)->GetAnimation(
                                pAnimation->sAnimation.CStr(), &fTime, &fLength);
                    }
                    if (pAnimation != NULL)
                    {
                        if (printanimqueue)
                        {
                            CExoString sMessage;
                            if (pAnimation->sAnimation.GetLength() > 0)
                                sMessage.Format("Playing next anim in queue: %s", pAnimation->sAnimation.CStr());
                            else
                                sMessage.Format("Playing next anim in queue: %s", sAnimation.CStr());
                            AurPostString(sMessage.CStr(), 0, a++ % 5 + g_nAnimQueueSize, g_fAnimQueueTime);
                        }
                        if (pAnimation->sAnimation.GetLength() > 0)
                        {
                            if (fLength > 0.0f)
                            {
                                if (pAnimation->bLooping)
                                {
                                    GetPlayerCreature()->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1)->PlayAnimation(
                                        pAnimation->sAnimation.CStr(), 1.0f, 0, 0.0f);
                                    m_fPlayerAnimationTime = pAnimation->fDuration;
                                }
                                else
                                {
                                    GetPlayerCreature()->GetModel(CSWCAnimBase::MODEL_SLOT_MAIN, 1)->PlayAnimation(
                                        pAnimation->sAnimation.CStr(), 1.0f, Gob::ANIMATION_FLAG_NO_LOOP, 0.0f);
                                    m_fPlayerAnimationTime = fLength;
                                }
                            }
                        }
                        else if (GetPlayerCreature()->AnimationLooping(pAnimation->nAnimation))
                        {
                            GetPlayerCreature()->SetAnimation(pAnimation->nAnimation, 0, 0);
                            m_fPlayerAnimationTime = pAnimation->fDuration;
                        }
                        else
                        {
                            GetPlayerCreature()->AddFireForgetAnimation(pAnimation->nAnimation, NULL);
                            m_fPlayerAnimationTime = GetPlayerCreature()->GetAnimationBase()->GetAnimationLength(
                                pAnimation->nAnimation) * 0.001f;
                        }
                    }
                }
            }
        }
        else
            m_fPlayerAnimationTime -= DeltaT;
    }

    CSWCCreature* pCharacter = m_pSWParty->GetCharacter(0);
    CSWCModule* pModule = m_pModule;
    unsigned long nDay;
    unsigned long nTime;
    g_pAppManager->GetClientExoApp()->GetWorldTimer()->GetWorldTime(&nDay, &nTime);
    if (pModule != NULL && pModule->m_pInfo != NULL)
    {
        CSWCArea* pArea = pModule->m_pInfo;
        if (!m_nUnknown26C && m_nUnknown188)
            m_nUnknown26C = 1;
        else if (m_nUnknown26C && !m_nUnknown188)
        {
            pArea->StartSounds();
            m_nUnknown26C = 0;
        }
        CAurScene* pScene = pArea->m_pScene;
        if (pScene != NULL && pCharacter != NULL)
        {
            if (!m_pGuiInGame->m_nGlobalDialogState)
                pScene->SetSceneFocus(pCharacter->m_vPosition);
            else
                pScene->SetSceneFocus(m_pGuiInGame->GetDialogCameraPosition(1));
        }
        if (!g_pAppManager->GetClientExoApp()->GetCaptureEventPlayBack() &&
            m_nInputClass != INPUT_CLASS_MINIGAME &&
            !m_pSWGuiManager->PanelExists(m_pGuiInGame->m_pTutorialBox))
            UpdateCamera(DeltaT);
        g_starttime = g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
        m_pClientAIMaster->UpdateState();
        g_endtime = g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
        g_AIUpdateTime = (int)(g_endtime - g_starttime) * 0.001f;
        if (g_pAppManager->GetClientExoApp()->GetClientOptions()->m_nCameraMode == CClientOptions::CAMERA_MODE_NAVIGATE &&
            g_pAppManager->GetClientExoApp()->GetModule() != NULL &&
            g_pAppManager->GetClientExoApp()->GetModule()->GetCamera() != NULL)
            UpdateRoom(g_pAppManager->GetClientExoApp()->GetModule()->GetCamera()->GetPosition());
        else if (pCharacter != NULL)
        {
            if (!m_pGuiInGame->m_nGlobalDialogState)
                UpdateRoom(pCharacter->m_vDesiredPosition);
            else
                UpdateRoom(m_pGuiInGame->GetDialogCameraPosition(0));
        }
    }
    if (m_pGuiInGame != NULL && m_pGuiInGame->m_oidCameraAnimationObject != OBJECT_INVALID)
    {
        unsigned long oidServerObject =
            g_pAppManager->GetServerExoApp()->ClientToServerObjectId(m_pGuiInGame->m_oidCameraAnimationObject);
        CGameObject* pObject = g_pAppManager->GetServerExoApp()->GetGameObject(oidServerObject);
        if (pObject != NULL && pObject->AsSWSObject() != NULL)
        {
            pObject->AsSWSObject()->UpdateDialog();
            if (pObject->AsSWSObject()->m_bDialogEnded)
                pObject->AsSWSObject()->DeleteDialog();
        }
    }
    if (m_pGuiInGame != NULL)
        m_pGuiInGame->UpdateDialog();
    if ((m_nInputClass == INPUT_CLASS_GAME || m_nInputClass == 4) &&
        !m_nAreaNotReadyToDisplay && !m_pGuiInGame->IsGlobalFading() &&
        !m_pGuiInGame->IsGlobalFadeObscuring())
    {
        DoPassiveSelection(DeltaT);
        UpdateCombatMode();
    }
    UpdateHostileHilite((__int64)g_pAppManager->GetClientExoApp()->GetExoAppInternal()->m_pPauseTimer->GetSnapshotTimeDifference() / 1000000.0f);
    g_starttime = g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
    if (m_pGuiInGame->GetPendingStatusSummary() && m_nInputClass == INPUT_CLASS_GAME &&
        !m_nAreaNotReadyToDisplay && !m_pGuiInGame->m_bHoldWorldFadeInForDialog &&
        !m_pGuiInGame->m_nGlobalDialogState && !m_pGuiInGame->IsGlobalFading() &&
        !m_pGuiInGame->IsGlobalFadeObscuring())
    {
        if (m_fStatusSummaryDelay == 0.0f)
            m_fStatusSummaryDelay = 0.25f;
        else
        {
            if (DeltaT < 0.25f)
                m_fStatusSummaryDelay -= DeltaT;
            if (m_fStatusSummaryDelay <= 0.0f)
            {
                m_fStatusSummaryDelay = 0.0f;
                m_pGuiInGame->ShowStatusSummary();
            }
        }
    }
    else if (m_fStatusSummaryDelay != 0.0f)
        m_fStatusSummaryDelay = 0.0f;
    m_pSWGuiManager->Update(DeltaT);
    m_pSWGuiManager->SetGuiScale(DeltaT);
    DeltaT = (__int64)g_pAppManager->GetClientExoApp()->GetExoAppInternal()->m_pPauseTimer->GetSnapshotTimeDifference() / 1000000.0f;
    m_pSWGuiManager->Draw(DeltaT);
    DeltaT = m_pSWGuiManager->m_fGuiScale;
    g_endtime = g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
    g_GuiUpdateTime = (int)(g_endtime - g_starttime) * 0.001f;
    g_starttime = g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
    UpdateRumble(DeltaT);
    m_pGuiInGame->UpdateControllerPauseState();
    if (m_nAttackMashTutorialTimer > 0)
    {
        unsigned long nElapsed = (unsigned long)(DeltaT * 1000.0f);
        if ((unsigned long)m_nAttackMashTutorialTimer <= nElapsed)
            m_nAttackMashTutorialTimer = 0;
        else
            m_nAttackMashTutorialTimer -= nElapsed;
    }
    if (g_pExoSound != NULL)
        UpdateSoundEngine();
    g_endtime = g_pExoBase->m_pExoTimers->GetHighResolutionTimer();
    g_SndUpdateTime = (int)(g_endtime - g_starttime) * 0.001f;
    if (m_bScreenShot && g_bEnableScreenCapture)
    {

        AurSnapshot(m_szScreenShot);
        m_bScreenShot = 0;
    }
    if (bAreaReady)
        m_nAreaNotReadyToDisplay = 0;
    if (m_fEnemyAutoPauseTimer > 0.0f)
    {
        m_fEnemyAutoPauseTimer -= DeltaT;
        if (m_fEnemyAutoPauseTimer <= 0.0f && !m_nCombatMode)
            SetAutoPaused(1, CClientExoApp::AUTOPAUSE_ENEMY_SIGHTED);
    }
    else if (m_fEnemyAutoPauseDelay > 0.0f)
        m_fEnemyAutoPauseDelay -= DeltaT;
    if (m_fEnemyAutoPauseCooldown > 0.0f)
        m_fEnemyAutoPauseCooldown -= DeltaT;
    if (m_PauseFlags.m_Bits.m_bTogglePauseAtEndOfFrame)
    {
        m_pSWCMessage->SendPlayerToServerInput_TogglePauseRequest();
        m_pGuiInGame->SetPaused(GetPausedByCombat(), m_nTogglePauseType);
        m_PauseFlags.m_Bits.m_bTogglePauseAtEndOfFrame = 0;
    }
    if (m_nTutorialReason != TUTORIAL_REASON_UNSET)
    {
        m_pGuiInGame->ShowTutorialWindow(m_nTutorialReason, m_nTutorialX, m_nTutorialY, m_nTutorialZ);
        m_nTutorialReason = TUTORIAL_REASON_UNSET;
    }
    return m_bExitGame && m_bExitGamePending;
}
#endif
#ifdef _XBOX

enum
{
    XBOX_CAMERA_TILT_INPUT = 12,
    XBOX_CAMERA_TURN_INPUT = 13
};
void CClientExoAppInternal::UpdateCamera(float fDeltaTime)
{
    if (m_pModule == NULL)
        return;

    float fTimeStep = (float)(__int64)g_pAppManager->GetClientExoApp()
                         ->GetPauseTimer()->GetSnapshotTimeDifference() /
                     1000000.0f;
    bool bKeepHistory = g_bKeepHistory;
    g_bKeepHistory = false;
    CAurCamera* pCamera = m_pModule->GetCamera();
    float fTilt = g_pExoInput->PollInput(XBOX_CAMERA_TILT_INPUT, m_nInputClass);
    m_pModule->TiltCamera((CameraVertAxisFlip ? 1 : -1) * fTilt);
    float fTurn = -g_pExoInput->PollInput(XBOX_CAMERA_TURN_INPUT, m_nInputClass);
    m_pModule->TurnCamera((CameraHorzAxisFlip ? 1 : -1) * fTurn, fDeltaTime);

    if (m_fCameraZoom != 0.0f)
    {
        CSWCModule* pModule = m_pModule;
        pModule->m_nReserved104 = 1;
    }
    if (pCamera != NULL)
    {
        float fExpansion = pCamera->GetDist() * 2.0f / 150.0f;
        g_HitCheckBoxExpansion.m_fX = fExpansion;
        g_HitCheckBoxExpansion.m_fY = fExpansion;
        g_HitCheckBoxExpansion.m_fZ = fExpansion;
    }
    Vector vPosition = pCamera->GetPosition();
    Vector vScroll;
    if (m_nCameraScrollX != 0)
        vScroll.m_fX = m_nCameraScrollX * pCamera->MouseDir(CAMERA_SCROLL_PIXELS, 0).m_fX;
    if (m_nCameraScrollY != 0)
        vScroll.m_fY = m_nCameraScrollY * pCamera->MouseDir(0, CAMERA_SCROLL_PIXELS).m_fY;
    m_pModule->ScrollCamera(vScroll);
    g_bKeepHistory = bKeepHistory;

    GetPlayerCreature();
    m_pModule->UpdateCameraBumpAndShake(fTimeStep);
    if (m_nCombatMode == 0)
        m_pModule->UpdateCameraFollow(fTimeStep);
    m_fCameraZoom = m_pModule->UpdateCameraCollision(fTimeStep, m_fCameraZoom);
    if (fabs(m_fCameraZoom) > CLIENT_CAMERA_ZOOM_THRESHOLD)
        m_pModule->ZoomCamera(m_fCameraZoom);

    if (g_pAppManager->GetClientExoApp()->GetCaptureEvents() == CAPTURE_EVENTS_RECORD &&
        m_pModule->m_nReserved80 == 1)
    {
        float fDistance = pCamera->GetDist();
        float fPitch = pCamera->GetPitch();
        float fYaw = pCamera->GetYaw();
        CExoString sEvent;
        sEvent.Format("%d %c %f %f %f\n",
            (unsigned int)(g_pAppManager->GetClientExoApp()->GetWorldTimer()
                               ->GetSnapshotTime() / 1000),
            CPlayBackEventNode::CAMERA, fDistance, fPitch, fYaw);
        g_pAppManager->GetClientExoApp()->GetCaptureEventFile()->Write(sEvent);
        CSWCModule* pModule = m_pModule;
        pModule->m_nReserved80 = 0;
    }
    m_fCameraZoom = 0.0f;
    m_fCameraTilt = 0.0f;
    m_fCameraTurn = 0.0f;
    m_pModule->m_nReserved80 = 0;
}
#endif
#ifdef _XBOX

void CClientExoAppInternal::UpdateRumble(float fDeltaTime)
{
    float fLeftMagnitude = 0.0f;
    float fRightMagnitude = 0.0f;
    CClientOptions* pOptions = g_pAppManager->GetClientExoApp()->GetClientOptions();
    if (fDeltaTime > 0.0f && pOptions != NULL && pOptions->m_bFloatingNumbers &&
        !m_nAreaNotReadyToDisplay &&
        !m_pGuiInGame->IsGlobalFadeObscuring() &&
        !m_pGuiInGame->IsGlobalFading() && !GetLoadScreenVisible())
    {
        for (int i = m_aRumbleInstances.num - 1; i >= 0; i--)
        {
            CSWRumbleInstance& cInstance = m_aRumbleInstances.element[i];
            if (cInstance.m_bFirstUpdate)
                cInstance.m_bFirstUpdate = 0;
            else
                cInstance.m_fTime += fDeltaTime;

            float fLeft;
            float fRight;
            if (!m_aRumblePatterns.element[cInstance.m_nPattern].GetMagnitudes(
                    cInstance.m_fTime, fLeft, fRight))
                m_aRumbleInstances.DelIndex(i);
            else
            {
                fLeftMagnitude = fLeftMagnitude > fLeft ? fLeftMagnitude : fLeft;
                fRightMagnitude = fRightMagnitude > fRight ? fRightMagnitude : fRight;
            }
        }
    }
    g_pExoInput->SetRumble(0, fLeftMagnitude, fRightMagnitude,
                          CLIENT_RUMBLE_DURATION);
}
#endif
#ifdef _XBOX

void CClientExoAppInternal::StopServices()
{
#ifndef _XBOX
    ShutDownMoviePlayer();
#endif
    if (m_pSWGuiManager->PanelExists(m_pMainMenu))
        delete m_pMainMenu;
    m_pMainMenu = NULL;
    if (m_pStartupPanel != NULL)
    {
        delete m_pStartupPanel;
        m_pStartupPanel = NULL;
    }
    if (m_pSoundSetList != NULL)
    {
        delete m_pSoundSetList;
        m_pSoundSetList = NULL;
    }
    if (m_pItemDropSounds != NULL)
    {
        delete m_pItemDropSounds;
        m_pItemDropSounds = NULL;
    }
    if (m_pModule != NULL)
    {
        delete m_pModule;
        m_pModule = NULL;
    }
    if (m_pGameObjectArray != NULL)
    {
        delete m_pGameObjectArray;
        m_pGameObjectArray = NULL;
    }
    DestroyWorldJournal();
    if (m_pImeUI != NULL)
    {

        m_pImeUI = NULL;
    }
    if (m_pGuiInGame != NULL)
    {
        m_pGuiInGame->ShutDown();
        delete m_pGuiInGame;
        m_pGuiInGame = NULL;
    }
    if (m_pSWGuiManager != NULL)
    {
        delete m_pSWGuiManager;
        m_pSWGuiManager = NULL;
    }
    if (m_pClientAIMaster != NULL)
    {
        delete m_pClientAIMaster;
        m_pClientAIMaster = NULL;
    }
    while (m_lstPlayerList.m_pInternal->m_pHead != NULL)
    {
        CSWCPlayer* pPlayer = m_lstPlayerList.RemoveHead();
        if (pPlayer != NULL)
            delete pPlayer;
    }
    if (m_pServerInfo != NULL)
    {
        delete m_pServerInfo;
        m_pServerInfo = NULL;
    }
    if (m_pUnknown10 != NULL)
    {
        m_pUnknown10->ShutDown();
        delete m_pUnknown10;
        m_pUnknown10 = NULL;
    }
    if (g_pExoInput != NULL)
    {
        delete g_pExoInput;
        g_pExoInput = NULL;
    }
    if (m_pMenuMusic != NULL)
    {
        delete m_pMenuMusic;
        m_pMenuMusic = NULL;
    }
    if (m_pLoadMusic != NULL)
    {
        delete m_pLoadMusic;
        m_pLoadMusic = NULL;
    }
    if (g_pExoSound != NULL)
    {
        delete g_pExoSound;
        g_pExoSound = NULL;
    }
    if (m_pSWCMessage != NULL)
    {
        delete m_pSWCMessage;
        m_pSWCMessage = NULL;
    }
    if (g_pTlkTable != NULL)
    {
        delete g_pTlkTable;
        g_pTlkTable = NULL;
    }
    if (m_pSoundSetList != NULL)
    {
        delete m_pSoundSetList;
        m_pSoundSetList = NULL;
    }
    if (g_pAuroraInterface != NULL)
    {
        delete g_pAuroraInterface;
        g_pAuroraInterface = NULL;
    }
    if (g_pAurora != NULL)
    {
        DestroyCAurora();
        g_pAurora = NULL;
    }
    if (g_pExoResMan != NULL)
    {
        g_pExoResMan->RemoveResourceDirectory("PORTRAITS:");
        g_pExoResMan->RemoveResourceDirectory("SERVERVAULT:");
        g_pExoResMan->RemoveFixedKeyTableFile("HD0:CHITIN");
        MountDownloadedContent(1, 0);
        g_pExoResMan->RemoveResourceDirectory("OVERRIDE:");
        g_pExoResMan->RemoveResourceDirectory("HD0:MOVIES");
        g_pExoResMan->RemoveResourceDirectory("HD0:STREAMMUSIC");
        g_pExoResMan->RemoveResourceDirectory("HD0:STREAMWAVES");
        g_pExoResMan->RemoveResourceDirectory("TEMPCLIENT:");
        if (g_pExoResMan->Exists("GLOBAL", RESTYPE_RIM, (unsigned long*)NULL))
            g_pExoResMan->RemoveResourceImageFile("RIMSXBOX:GLOBAL");
        if (g_pExoResMan->Exists("GLOBALDX", RESTYPE_RIM, (unsigned long*)NULL))
            g_pExoResMan->RemoveResourceImageFile("RIMSXBOX:GLOBALDX");
        if (g_pExoResMan->Exists("SUBGLOBAL", RESTYPE_RIM, (unsigned long*)NULL))
            g_pExoResMan->RemoveResourceImageFile("RIMSXBOX:SUBGLOBAL");
        if (g_pExoResMan->Exists("SUBGLOBAL", RESTYPE_RIM, (unsigned long*)NULL))
            g_pExoResMan->RemoveResourceImageFile("RIMSXBOX:SUBGLOBALDX");
        g_pExoResMan->RemoveResourceDirectory("RIMSXBOX:");
        if (g_pExoResMan != NULL)
        {
            delete g_pExoResMan;
            g_pExoResMan = NULL;
        }
    }
    if (m_pWorldTimer != NULL)
    {
        delete m_pWorldTimer;
        m_pWorldTimer = NULL;
    }
    if (m_pCombatTimer != NULL)
    {
        delete m_pCombatTimer;
        m_pCombatTimer = NULL;
    }
    if (m_pPauseTimer != NULL)
    {
        delete m_pPauseTimer;
        m_pPauseTimer = NULL;
    }
    if (m_pAnimationTimer != NULL)
    {
        delete m_pAnimationTimer;
        m_pAnimationTimer = NULL;
    }
    if (m_pClientOptions != NULL)
    {
        delete m_pClientOptions;
        m_pClientOptions = NULL;
    }
    if (m_pUnknown1C0 != NULL)
    {
        delete m_pUnknown1C0;
        m_pUnknown1C0 = NULL;
    }
    if (m_pPlayerControl != NULL)
        delete m_pPlayerControl;
#ifdef _XBOX
    if (m_pMoviePlayer != NULL)
    {
        delete m_pMoviePlayer;
        m_pMoviePlayer = NULL;
    }
#endif
    DestroyQuestJournal();
    if (m_pCharacterFile != NULL)
    {
        delete[] m_pCharacterFile;
        m_pCharacterFile = NULL;
    }
    m_nCharacterFileSize = 0;
    m_nCharacterFileId = 0;
}
CSWCPlayer* CClientExoAppInternal::GetPlayerByPlayerID(unsigned long nPlayerId)
{
    CExoLinkedListNode* pNode = m_lstPlayerList.GetHeadPos();
    void* pData = m_lstPlayerList.GetHead();
    CSWCPlayer* pPlayer = (CSWCPlayer*)pData;

    while (pPlayer != NULL)
    {
        if (pPlayer->m_nPlayerId == nPlayerId)
        {
            return pPlayer;
        }

        pPlayer = (CSWCPlayer*)m_lstPlayerList.GetNext(pNode);
    }

    return NULL;
}
#endif
#ifdef _XBOX

void CClientExoAppInternal::UpdateSoundEngine()
{
    if (m_pModule != NULL)
    {
        CAurCamera* pCamera = m_pModule->GetCamera();
        CSWCCreature* pCreature = GetPlayerCreature();
        if (pCamera != NULL)
        {
            CSWMiniGame* pMiniGame = m_pModule->GetMiniGame();
            if (pMiniGame != NULL && pMiniGame->GetPlayer() != NULL)
            {
                Vector vPlayerPosition = pMiniGame->GetPlayer()->GetPosition();
            }
            else if (pCreature == NULL)
                goto RenderSound;

            Vector vPosition;
            Vector vDirection;
            Quaternion qOrientation;
            vPosition = pCamera->GetPosition();
            qOrientation = pCamera->GetOrientation();

            if (g_fMicPosFromCam < 0.0f)
                g_fMicPosFromCam = 0.0f;
            else if (g_fMicPosFromCam > 1.0f)
                g_fMicPosFromCam = 1.0f;

            if (pCamera != NULL)
            {
                CSWCameraOnAStick* pCameraOnAStick =
                    (CSWCameraOnAStick*)pCamera->GetBehavior(AUR_BEHAVIOR_CAMERA_ONASTICK);
                if (pCameraOnAStick != NULL)
                {
                    Vector vTarget;
                    vTarget = pCameraOnAStick->GetTargetHeadPosition();
                    vPosition += g_fMicPosFromCam * (vTarget - vPosition);
                }
                else if (pCamera->GetBehavior(AUR_BEHAVIOR_CAMERA_COMBAT) != NULL)
                {
                    CSWCCreature* pCharacter = m_pSWParty->GetCharacter(0);
                    Vector vTarget = pCharacter->m_vPosition;
                    vTarget.m_fZ += pCharacter->GetHeadHeightOffset();
                    vPosition += (vTarget - vPosition) * g_fMicPosFromCam;
                }
                else if (g_pAppManager->GetClientExoApp()->GetInGameGui()->m_nGlobalDialogState &&
                         g_pAppManager->GetClientExoApp()->GetInGameGui()->m_bComputerCameraShot)
                {
                    float fMicRange =
                        g_pAppManager->GetClientExoApp()->GetInGameGui()->m_fComputerCameraMicRange;
                    vPosition -= qOrientation.zdir() * fMicRange;
                }
            }

            g_pExoSound->SetListenerPosition(vPosition);
            vDirection = qOrientation.zdir();
            vDirection.m_fX *= -1.0f;
            vDirection.m_fY *= -1.0f;
            vDirection.m_fZ *= -1.0f;
            g_pExoSound->SetListenerOrientation(vDirection, qOrientation.ydir());
        }
    }
RenderSound:

    g_pExoSound->Render();
}
#endif

#ifdef _XBOX

#include "aurconsole.h"

void soundrolloff(char* szArgs)
{
    float fFactor = (float)atof(szArgs);
    g_pExoSound->SetRollOffFactor(fFactor);
}
ConsoleFunc ConFsoundrolloff("soundrolloff", soundrolloff);
ConsoleFunc ConFSetFramesPerSecond("SetFramesPerSecond", SetFramesPerSecond);

char* SetMovieFrameRate(char* szArgs)
{
    int nFrameRate = atoi(szArgs);
    if (nFrameRate != 0)
    {
        g_nSetMovieFrameRate = nFrameRate;
        return "Frame rate will be set after the next module load.";
    }
    g_nSetMovieFrameRate = 0;
    return "Frame rate returned to normal.";
}
ConsoleFunc ConFSetMovieFrameRate("SetMovieFrameRate", SetMovieFrameRate);

char* MarkLog(char* szArgs)
{
    g_pExoBase->m_pExoDebug->WriteToScriptLogFile(g_pExoBase->m_pExoDebug->m_sLogDirectory);
    g_pExoBase->m_pExoDebug->FlushScriptLogFile();
    return "Log has been duly noted!";
}
ConsoleFunc ConFMarkLog("MarkLog", MarkLog);

#include "nwsmodule.h"
#include "nwsarea.h"

char* SpawnChar(char* szArgs)
{
    CSWSCreature* pPlayer = g_pAppManager->m_pServerExoApp->GetPlayerCreature();
    if (pPlayer != NULL)
    {
        Vector vPosition = pPlayer->m_vPosition + pPlayer->m_vOrientation * 5.0f;
        CSWSCreature* pCreature = new CSWSCreature(0x7f000000, 0);
        if (pCreature->LoadFromTemplate(szArgs, 0) == 1)
        {
            g_pAppManager->GetServerExoApp()->GetModule()->GetArea()->ComputeSafeLocation(
                vPosition, 10.0f, pCreature->GetPathfindInformation(), 1, &vPosition);
            pCreature->AddToArea(g_pAppManager->GetServerExoApp()->GetModule()->GetArea(),
                                 vPosition.m_fX, vPosition.m_fY, vPosition.m_fZ, 1, 1);
            return "SUCCESS!";
        }
        return "ERROR: Failed to load template.";
    }
    return "ERROR: No player creature present.";
}
ConsoleFunc ConFSpawnChar("SpawnChar", SpawnChar);
#endif
