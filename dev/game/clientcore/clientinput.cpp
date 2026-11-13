#include "exobase.h"
#include "exoinput.h"
#include "exoinputinternal.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "clientexoappinternal.h"
#include "swguicustompanel.h"
#include "guiingame.h"
#include "swguimaininterfacepc2.h"
#include "clientoptions.h"
#include "nwcdoor.h"
#include "nwcobject.h"
#include "nwcanimbase.h"
#include "nwccreature.h"
#include "nwcmodule.h"
#include "aurcamera.h"
#include "gameobjectarray.h"
#include "swparty.h"
#include "gameobjectarray.h"
#include "netlayer.h"
#include "serverexoapp.h"
#include "nwscreature.h"
#include "nwcarea.h"
#include "nwsplayer.h"
#include "nwsarea.h"
#include "nwsplaceable.h"
#include "nwcplaceable.h"
#include "nwctrigger.h"
#include "nwccreaturestats.h"
#include "nwrules.h"
#include "swminigame.h"
#include "nwsvartable.h"
#include "swbehaviorcamera.h"
#include "nwreentrantserver.h"
#include "swmain.h"
#include <stdio.h>
#include <float.h>
#include "nwcitem.h"
#if defined(_WIN32) && !defined(_XBOX)
#include <windows.h>
#else
#include "platform.h"
#endif

#define MOUSE_LOOK_THRESHOLD 0.05f
#define FREE_LOOK_TILT_SPEED 300.0f
#define FREE_LOOK_RETURN_STEP 3.0f
#define HOSTILE_TARGET_RANGE 30.0f
#define TARGET_RANGE 10.0f


static inline void ClientInputTrace();
#define CLIENTINPUT_TRACE() ClientInputTrace()

int g_nInputUnused1 = 0;
int g_nInputUnused2 = 0;
int g_nHSMode = 0;
CAurObject* g_pGroundRing = NULL;

int g_bMouseCameraRotateToggle = 0;
int g_bLeftMouseDown = 0;
int g_bShiftActionState = 0;

float g_fMouseSensitivity = 20.0f;

int g_bAllowDebugMenu = 1;
Vector g_vCreatureEnemyHighlight(1.0f, 0.1f, 0.1f);
Vector g_vCreatureFriendHighlight(0.3f, 1.0f, 0.3f);
Vector g_vCreatureNeutralHighlight(0.4f, 0.4f, 1.0f);
Vector g_vItemHighlight(0.3f, 1.0f, 1.0f);
Vector g_vDoorHighlight(0.3f, 1.0f, 1.0f);

CAurObject* g_pDialogIconModel = NULL;

int HandleWMKeyDownMessage(HWND hWnd, unsigned int nKey, long nKeyData);
int HandleWMCharMessage(unsigned short nChar);
// PC 0x0066FC70
void CClientExoAppInternal::DisableInput()
{
    m_bInputDisabled = 1;
}
// PC 0x0066FC80
void CClientExoAppInternal::EnableInput()
{
    m_bInputDisabled = 0;
}
// PC 0x0066FC90
int CClientExoAppInternal::PerformXMouseMoveAction(int nX)
{
    if (m_nMouseX == nX)
    {
        return 0;
    }
    m_nMouseX = nX;
    m_nMouseX = m_nMouseX < 0 ? 0 : m_nMouseX;
    m_nMouseX =
        m_nMouseX >= g_nScreenWidth - 1 ? g_nScreenWidth - 1 : m_nMouseX;
    return 1;
}
// PC 0x0066FCE0
int CClientExoAppInternal::PerformYMouseMoveAction(int nY)
{
    int nNewY = g_nScreenHeight - nY - 1;
    if (m_nMouseY == nNewY)
    {
        return 0;
    }
    m_nMouseY = nNewY;
    m_nMouseY = m_nMouseY < 0 ? 0 : m_nMouseY;
    m_nMouseY =
        m_nMouseY >= g_nScreenHeight - 1 ? g_nScreenHeight - 1 : m_nMouseY;
    return 1;
}

#ifndef _XBOX
void CClientExoAppInternal::PerformLButtonDownAction(int nArg)  // PC 0x0066FD30
{
    int bToggle;
    CClientOptions* pOptions;
    g_bLeftMouseDown = 1;
    if (g_pExoInput != NULL)
    {
#ifndef _XBOX
        g_pExoInput->RefreshMouseCursor(1);
#endif
    }
    if (m_pSWGuiManager != NULL)
    {
        m_pSWGuiManager->SetSoftwareMouseCursor(
            m_pSWGuiManager->m_nMouseCursor,
            m_pSWGuiManager->m_bCursorCentered != 0, 1);
    }
    bToggle = g_bMouseCameraRotateToggle;
    pOptions = g_pAppManager->GetClientExoApp()->GetClientOptions();
    if (pOptions->m_bMouseLook)
    {
        if (m_nInputClass != 2 && m_nInputClass != 3)
        {
            bToggle = (bToggle == 0);
        }
    }
    if ((bToggle == 0 && m_nInputClass != 4) || m_nInputClass == 2 ||
        m_nInputClass == 3)
    {
        if (nArg == 0)
        {
            m_oidLastClickedOnTarget = m_nLastTarget;
        }
        m_pSWGuiManager->HandleLMouseDown(nArg);
    }
}
#endif
// PC 0x0066FDF0
void CClientExoAppInternal::PerformRButtonDownAction()
{
    g_bMouseCameraRotateToggle = 1;
#ifndef _XBOX
    g_pGuiMan->HandleRMouseDown();
#endif
}
// PC 0x0066FE10
int CClientExoAppInternal::PerformRButtonUpAction()
{
    CSWGuiManager* pGuiManager = g_pGuiMan;
    g_bMouseCameraRotateToggle = 0;
#ifndef _XBOX
    return pGuiManager->HandleRMouseUp();
#else
    return 0;
#endif
}
// PC 0x0066FE30
void CClientExoAppInternal::SetMouseCursor(int nCursor, int nArg2, int nArg3)
{
    if (g_pExoInput != NULL)
    {
#ifndef _XBOX
        nCursor = g_pExoInput->SetMouseCursor(nCursor, nArg2, nArg3);
#endif
    }
    if (m_pSWGuiManager != NULL)
    {
#ifndef _XBOX
        m_pSWGuiManager->SetSoftwareMouseCursor(nCursor, nArg2, nArg3);
#endif
    }
}
// PC 0x0066FE70
void CClientExoAppInternal::HideMouse(int nFlags)
{
    m_nMouseHidden |= nFlags;
    if (m_nMouseHidden != 0)
    {
        if (g_pExoInput != NULL)
        {
#ifndef _XBOX
            g_pExoInput->HideMouse();
#endif
        }
        if (m_pSWGuiManager != NULL)
        {
            m_pSWGuiManager->HideSoftwareMouse();
        }
    }
}
// PC 0x0066FEB0
void CClientExoAppInternal::ShowMouse(int nFlags)
{
    m_nMouseHidden = ~nFlags & m_nMouseHidden;
    if (m_nMouseHidden == 0)
    {
        if ((nFlags & 0x4A) != 0)
        {
            m_fMouseHideTime = 0.5f;
        }
        if ((nFlags & 0x14) != 0)
        {
            m_fMouseHideTime = -1.0f;
        }
        if (m_fMouseHideTime <= 0.0f)
        {
#ifndef _XBOX
            g_pExoInput->ShowMouse();
#endif
            if (m_pSWGuiManager != NULL &&
                m_pClientOptions->m_bHardwareMouseCursor == 0)
            {
#ifndef _XBOX
                m_pSWGuiManager->ShowSoftwareMouse();
#endif
            }
#ifndef _XBOX
            g_pExoInput->RefreshMouseCursor(g_bLeftMouseDown);
#endif
        }
    }
}

int NearestObjectCompare(void* pObjectId, CNearestStruct cNearest)
{
    return pObjectId == (void*)cNearest.m_oidObject;
}
// PC 0x0066FF40
int TestNearestStruct(void* pObjectId, CNearestStruct cNearest)
{
    return (unsigned long)pObjectId == cNearest.m_oidObject;
}
// PC 0x0066FF50
int CClientExoAppInternal::ActionIconToMouseCursor(int nAction, int nArg2)
{
    int nCursor;
    switch (nAction & 0xFFFFFFF)
    {
    case -1:
        nCursor = 5;
        break;
    case 1001:
        nCursor = 25;
        break;
    case 1002:
        nCursor = 11;
        break;
    case 1003:
        nCursor = 51;
        break;
    case 1010:
        nCursor = 23;
        break;
    case 1013:
        nCursor = 7;
        break;
    case 1012:
        nCursor = 33;
        break;
    case 1015:
        nCursor = 25;
        break;
    case 1026:
        nCursor = 37;
        break;
    default:
        nCursor = 5;
        break;
    }
    return nCursor;
}
// PC 0x00670050
void CClientExoAppInternal::ReadPlayBackInputEvents()
{
    CLIENTINPUT_TRACE();

    char szBuffer[PLAYBACK_RECORD_LENGTH];
    int nStartTime;
    int nTime;
    char cType;
    unsigned short nKey;
    int nInputEvent;
    int nInputValue;
    unsigned long nPlayerId;
    int bGuaranteed;
    unsigned long nMessageSize;
    unsigned long oidObject;
    float fX, fY, fZ;
    int bConnection = 0;
    int nNewLines = 0;
    int nSpaces = 0;
    int nBytes = 0;
    int nPosition = 0;

    while (nNewLines < 1)
    {
        if (g_pAppManager->GetClientExoApp()->GetCaptureEventFile()->Read(
                &szBuffer[nBytes], 1, 1) == 0)
            return;
        if (szBuffer[nBytes] == '\n')
            nNewLines++;
        nBytes++;
    }
    sscanf(&szBuffer[7], "%d", &nStartTime, &cType);

    while (1)
    {
        nSpaces = 0;
        nPosition = 0;
        while (nSpaces < 2)
        {
            if (g_pAppManager->GetClientExoApp()->GetCaptureEventFile()->Read(
                    &szBuffer[nPosition], 1, 1) == 0)
                return;
            if (szBuffer[nPosition] == ' ')
                nSpaces++;
            nPosition++;
            nBytes++;
        }
        sscanf(szBuffer, "%d %c", &nTime, &cType);

        if (cType == CPlayBackEventNode::KEY_DOWN ||
            cType == CPlayBackEventNode::CHARACTER ||
            cType == CPlayBackEventNode::INPUT ||
            cType == CPlayBackEventNode::INPUT_END)
        {
            nNewLines = 0;
            nPosition = 0;
            while (nNewLines < 1)
            {
                if (g_pAppManager->GetClientExoApp()->GetCaptureEventFile()->Read(
                        &szBuffer[nPosition], 1, 1) == 0)
                    return;
                if (szBuffer[nPosition] == '\n')
                    nNewLines++;
                nPosition++;
                nBytes++;
            }
            CPlayBackEventNode* pEvent = new CPlayBackEventNode;
            pEvent->nTime = nTime - nStartTime;
            pEvent->cType = cType;
            if (cType == CPlayBackEventNode::KEY_DOWN ||
                cType == CPlayBackEventNode::CHARACTER)
            {
                sscanf(szBuffer, "%d", &nKey);
                pEvent->nKey = nKey;
            }
            if (cType == CPlayBackEventNode::INPUT || cType == CPlayBackEventNode::INPUT_END)
            {
                sscanf(szBuffer, "%d %d", &nInputEvent, &nInputValue);
                pEvent->nInputEvent = nInputEvent;
                pEvent->nInputValue = nInputValue;
            }
            m_lstPlayBackEvents.m_pInternal->AddTail(pEvent);
        }
        if (cType == CPlayBackEventNode::MESSAGE)
        {
            if (bConnection == 0)
            {
                bConnection = 1;
                CPlayBackEventNode* pEvent = new CPlayBackEventNode;
                pEvent->nTime = nTime - nStartTime;
                pEvent->cType = CPlayBackEventNode::CONNECTION;
                m_lstPlayBackEvents.m_pInternal->AddTail(pEvent);
            }
            nPosition = 0;
            nSpaces = 0;
            nNewLines = 0;
            while (nSpaces < 3)
            {
                if (g_pAppManager->GetClientExoApp()->GetCaptureEventFile()->Read(
                        &szBuffer[nPosition], 1, 1) == 0)
                    return;
                if (szBuffer[nPosition] == ' ')
                    nSpaces++;
                if (szBuffer[nPosition] == '\n')
                    nNewLines++;
                nPosition++;
                nBytes++;
            }
            sscanf(szBuffer, "%d %d %d ", &nPlayerId, &bGuaranteed, &nMessageSize);
            unsigned char* pMessage = new unsigned char[nMessageSize];
            if (g_pAppManager->GetClientExoApp()->GetCaptureEventFile()->Read(
                    pMessage, nMessageSize, 1) != nMessageSize)
                return;
            nBytes += nMessageSize;
            while (nNewLines < 1)
            {
                if (g_pAppManager->GetClientExoApp()->GetCaptureEventFile()->Read(
                        &szBuffer[nPosition], 1, 1) == 0)
                    return;
                if (szBuffer[nPosition] == '\n')
                    nNewLines++;
                nPosition++;
                nBytes++;
            }
            CPlayBackEventNode* pEvent = new CPlayBackEventNode;
            pEvent->nTime = nTime - nStartTime;
            pEvent->cType = cType;
            pEvent->nMessageSize = nMessageSize;
            pEvent->bGuaranteed = bGuaranteed;
            pEvent->nPlayerId = nPlayerId;
            pEvent->pMessage = pMessage;
            m_lstPlayBackEvents.m_pInternal->AddTail(pEvent);
        }
        if (cType == CPlayBackEventNode::CAMERA)
        {
            nPosition = 0;
            nNewLines = 0;
            while (nNewLines < 1)
            {
                if (g_pAppManager->GetClientExoApp()->GetCaptureEventFile()->Read(
                        &szBuffer[nPosition], 1, 1) == 0)
                    return;
                if (szBuffer[nPosition] == '\n')
                    nNewLines++;
                nPosition++;
                nBytes++;
            }
            CPlayBackEventNode* pEvent = new CPlayBackEventNode;
            pEvent->nTime = nTime - nStartTime;
            pEvent->cType = cType;
            sscanf(szBuffer, "%f %f %f", &fX, &fY, &fZ);
            pEvent->fX = fX;
            pEvent->fY = fY;
            pEvent->fZ = fZ;
            m_lstPlayBackEvents.m_pInternal->AddTail(pEvent);
        }
        if (cType == CPlayBackEventNode::POSITION)
        {
            nPosition = 0;
            nNewLines = 0;
            while (nNewLines < 1)
            {
                if (g_pAppManager->GetClientExoApp()->GetCaptureEventFile()->Read(
                        &szBuffer[nPosition], 1, 1) == 0)
                    return;
                if (szBuffer[nPosition] == '\n')
                    nNewLines++;
                nPosition++;
                nBytes++;
            }
            CPlayBackEventNode* pEvent = new CPlayBackEventNode;
            pEvent->nTime = nTime - nStartTime;
            pEvent->cType = cType;
            sscanf(szBuffer, "%x %f %f %f", &oidObject, &fX, &fY, &fZ);
            pEvent->fX = fX;
            pEvent->fY = fY;
            pEvent->fZ = fZ;
            pEvent->oidObject = oidObject;
            m_lstPlayBackEvents.m_pInternal->AddTail(pEvent);
        }
    }
}

#ifndef _XBOX
// PC 0x00670590
void CClientExoAppInternal::SetInputClass(int nClass, int bHideInterface)

{

    if (m_nInputClass != nClass)
    {
        CLIENTINPUT_TRACE();
        int nOldClass = m_nInputClass;
        m_nInputClass = nClass;
        g_pExoInput->ClearEvents();

        switch (nClass)
        {
        case INPUT_CLASS_GAME:
            if (m_oidReattachedCreature != OBJECT_INVALID)
            {
                CSWSCreature* pCreature =
                    g_pAppManager->GetServerExoApp()->GetPlayerCreature();
                if (pCreature != NULL)
                {
                    SetPlayerCreature(g_pAppManager->GetClientExoApp()->
                        ServerToClientObjectId(pCreature->GetId()));
                    m_oidReattachedCreature = OBJECT_INVALID;
                    AttachPlayerGameObjectToArea();
                }
            }
            if (m_pModule != NULL && m_pModule->GetCamera() != NULL)
            {
                Vector vOrientation;
                if (GetPlayerCreature() != NULL)
                    vOrientation = GetPlayerCreature()->m_vOrientation;
                CSWCameraOnAStick* pCameraBehavior = (CSWCameraOnAStick*)
                    m_pModule->GetCamera()->GetBehavior(
                        AUR_BEHAVIOR_CAMERA_ONASTICK);
                if (pCameraBehavior != NULL)
                {
                    pCameraBehavior->m_nOrientationState = 0;
                    pCameraBehavior->m_vPlayerOrientation = vOrientation;
                }
            }
            if (GetPlayerCreature() != NULL)
                GetPlayerCreature()->SwitchMode(CSWCCreature::MODE_SWDRIVE);
            if (m_pPlayerControl != NULL)
                m_pPlayerControl->SetEnabled(1);
            ShowMouse(MOUSE_HIDE_MINIGAME);
            if (m_pGuiInGame != NULL)
                m_pGuiInGame->SetSWGuiStatus(
                    CGuiInGame::GUISTATUS_RESTORE_MAIN_INTERFACE, 1);
            m_pGuiInGame->m_pPartySelection = 0;
            break;

        case INPUT_CLASS_DIALOG:
            IndicateObject(NULL, 0);
            break;
        case INPUT_CLASS_GUI:
            if (m_pGuiInGame != NULL &&
                m_pGuiInGame->m_bShowPauseGui == CGuiInGame::GUISTATE_MAIN_INTERFACE)
            {
                if (bHideInterface)
                    m_pGuiInGame->SetSWGuiStatus(
                        CGuiInGame::GUISTATUS_HIDE_MAIN_INTERFACE, 1);
                ShowMouse(MOUSE_HIDE_MINIGAME);
            }
            m_pGuiInGame->m_pPartySelection = 1;
            break;

        case INPUT_CLASS_MINIGAME:
        {
            CSWMiniGame* pMiniGame;
            if (m_pModule != NULL && m_pModule->m_pInfo != NULL &&
                (pMiniGame = m_pModule->m_pInfo->m_pMiniGame) != NULL)
            {
                IndicateObject(NULL, 0);
                HideMouse(MOUSE_HIDE_MINIGAME);
                if (m_pGuiInGame != NULL)
                    m_pGuiInGame->SetSWGuiStatus(CGuiInGame::GUISTATUS_DIALOG, 0);
                if (GetPlayerCreature() != NULL)
                {
                    GetPlayerCreature()->AttachToScene(NULL);
                    m_oidReattachedCreature = GetPlayerCreature()->GetId();
                    SetPlayerCreature(OBJECT_INVALID);
                }
                pMiniGame->SetCamera(m_pModule->GetCamera());
                pMiniGame->Go();
            }
            else
            {
                SetInputClass(nOldClass, 1);
            }
            break;
        }
        }
    }
}
#endif
int CClientExoAppInternal::DismissInGameGUI()  // PC 0x00670810
{
#ifdef _XBOX
    if (m_pGuiInGame->HideSWInGameGui() != 0)
#else
    if (m_pGuiInGame->HideSWInGameGui(0) != 0)
#endif
    {
        SetInputClass(INPUT_CLASS_GAME, 1);
        return 1;
    }
    return 0;
}

#ifndef _XBOX
void CClientExoAppInternal::PerformLButtonUpAction()  // PC 0x00670A20
{
    int bToggle;
    CSWGuiControl* pControl;
    g_bLeftMouseDown = 0;
    if (g_pExoInput != NULL)
    {
#ifndef _XBOX
        g_pExoInput->RefreshMouseCursor(0);
#endif
    }
    if (m_pSWGuiManager != NULL)
    {
        m_pSWGuiManager->SetSoftwareMouseCursor(
            m_pSWGuiManager->m_nMouseCursor,
            m_pSWGuiManager->m_bCursorCentered != 0, 0);
    }
    bToggle = g_bMouseCameraRotateToggle;
    if (g_pAppManager->m_pClientExoApp->GetClientOptions()->m_bMouseLook != 0)
    {
        if (m_nInputClass != 2 && m_nInputClass != 3)
        {
            bToggle = (bToggle == 0);
        }
    }
    if ((bToggle == 0 && m_nInputClass != 4) || m_nInputClass == 2 ||
        m_nInputClass == 3)
    {
        pControl = m_pSWGuiManager->m_pMouseOverControl;
        if (pControl != NULL &&
            m_pGuiInGame->m_pMainInterface->IsTargetActionMenuControl(
                pControl) == 1)
        {
            if (g_pAppManager->m_pServerExoApp != NULL)
            {
                HandleMouseClickInWorld();
            }
        }
        else if (g_pGuiMan->HandleLMouseUp() == 0)
        {
            if (g_pAppManager->m_pServerExoApp != NULL)
            {
                HandleMouseClickInWorld();
            }
        }
    }
}
#endif

#ifndef _XBOX
inline CSWGuiMainInterface* CGuiInGame::GetMainInterface()
{
    return m_pMainInterface;
}

void CClientExoAppInternal::HandleMouseClickInWorld()  // PC 0x00670840
{
    CLIENTINPUT_TRACE();
    if (g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nGlobalDialogState == 0 &&
        m_pGuiInGame->m_pPartySelection == NULL &&
        m_pClientOptions->m_nCameraMode != CClientOptions::CAMERA_MODE_WORLD_CLICK_DISABLED)
    {
        if (m_oidLastClickedOnTarget != OBJECT_INVALID &&
            m_nLastTarget == m_oidMouseOverTarget &&
            m_oidLastClickedOnTarget == m_nLastTarget)
        {
            m_oidLastClickedOnTarget = OBJECT_INVALID;
            if (m_aDefaultActions.num > 0)
            {
                CSWGuiInterfaceAction* pAction = m_aDefaultActions.element;
                if (pAction != NULL)
                {
                    CSWCCreature* pPlayer = g_pAppManager->m_pClientExoApp->GetSWParty()->GetPlayerCharacter();
                    CClientExoApp* pClientApp = g_pAppManager->m_pClientExoApp;
                    CSWCObject* pGameObject = pClientApp->GetGameObject(pAction->m_oidTarget);
                    if (pGameObject != NULL)
                    {
                        CSWCObject* pObject = pGameObject->AsSWCObject();
                        if (pObject != NULL)
                        {
                            m_pSWGuiManager->PlayGuiSound(CSWGuiManager::GUISOUND_WORLD_ACTION);
                            (pObject->*pAction->m_pfnAction)(pAction->m_nAction, pPlayer);
                        }
                    }
                }
            }
        }
        else
        {
            if (m_oidMouseOverTarget != OBJECT_INVALID)
                SetLastTarget(m_oidMouseOverTarget, 0);
            if (g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMainInterface != NULL)
            {
                g_pAppManager->m_pClientExoApp->GetInGameGui()->GetMainInterface()->PopulateMenus(1);
            }
        }
    }
}
#endif

CResRef CClientExoAppInternal::ActionIconToCursorResref(int nAction, int nArg2)
{
    return CResRef(g_asMouseCursorNames[ActionIconToMouseCursor(nAction, nArg2)]);
}

#ifndef _XBOX
void CClientExoAppInternal::GetDefaultActions()  // PC 0x00670B10
{
    int nAction = 0;
    CSWCCreature* pPlayer =
        g_pAppManager->GetClientExoApp()->GetSWParty()->GetPlayerCharacter();
    CSWSCreature* pServerPlayer;
    if (pPlayer != NULL)
        pServerPlayer = pPlayer->GetServerCreature();
    else
        pServerPlayer = NULL;
    CSWGuiMainInterface* pMainInterface = g_pAppManager->GetClientExoApp()->GetInGameGui()->m_pMainInterface;
    unsigned long oidTarget = pMainInterface->m_nTargetId;
    CSWCObject* pGameObject = g_pAppManager->GetClientExoApp()->GetGameObject(oidTarget);
    if (pPlayer != NULL)
    {
        int bDead = pServerPlayer != NULL &&
            (pServerPlayer->GetDead() || pServerPlayer->GetDeadTemp());
        if (pGameObject != NULL)
        {
            CSWCObject* pTarget = pGameObject->AsSWCObject();
            if (pTarget != NULL)
            {
                int nTargetType = pPlayer->GetInterfaceTargetType(pTarget);
                for (int i = 0; i < m_aDefaultActions.num; i++)
                    m_aDefaultActions[i].Clear();
                m_aDefaultActions.SetSize(0);
                switch (nTargetType)
                {
                case CSWCCreature::INTERFACE_TARGET_NONE:
                    m_aDefaultActions.SetSize(1);
                    m_aDefaultActions[0].m_sName = g_pAppManager->GetClientExoApp()->GetGUIString(CClientExoApp::GUI_STRING_NO_ACTION);
                    m_aDefaultActions[0].m_pfnAction = NULL;
                    m_aDefaultActions[0].m_oidTarget = OBJECT_INVALID;
                    m_aDefaultActions[0].m_nAction = CSWGuiInterfaceAction::ACTION_NO_ACTION;
                    m_aDefaultActions[0].m_resIcon = "i_noaction";
                    break;
                case CSWCCreature::INTERFACE_TARGET_DOOR_OR_CONTAINER:
                case CSWCCreature::INTERFACE_TARGET_FRIENDLY:
                    if (pTarget->AsSWCCreature() != NULL)
                    {
                        CSWSCreature* pServerCreature = pTarget->AsSWCCreature()->GetServerCreature();
                        if (pServerCreature != NULL && pServerCreature->m_n258 == 0)
                        {
                            m_aDefaultActions.SetSize(nAction + 1);
                            m_aDefaultActions[0].m_sName = g_pAppManager->GetClientExoApp()->GetGUIString(CClientExoApp::GUI_STRING_DIALOG);
                            m_aDefaultActions[0].m_pfnAction = (PFNMENUACTION)&CSWCCreature::ActionInitiateDialog;
                            m_aDefaultActions[0].m_oidTarget = pTarget->GetId();
                            m_aDefaultActions[0].m_nAction = CSWGuiInterfaceAction::ACTION_DIALOG;
                            m_aDefaultActions[0].m_resIcon = "i_dialog";
                        }
                    }
                    if (pTarget->AsSWCPlaceable() != NULL)
                    {
                        CSWSPlaceable* pPlaceable = pTarget->AsSWCPlaceable()->GetServerPlaceable();
                        int nRestrictMode = pTarget->AsSWCPlaceable()->m_pArea->GetServerArea()->m_nPlayerRestrictMode;
                        CExoString sText;
                        if (pPlaceable != NULL && pPlaceable->m_bHasInventory)
                        {
                            int bUseable = pTarget->AsSWCPlaceable()->m_bUseable;
                            if (bUseable)
                            {
                                m_aDefaultActions.SetSize(nAction + 1);
                                unsigned long nStrRef = CClientExoApp::GUI_STRING_OPEN;
                                int bUseSearch;
                                int bRead = g_pRules->Get2DArrays()->m_pPlaceables->GetINTEntry(
                                    pTarget->AsSWCPlaceable()->GetAppearance(),
                                    "UseSearch", &bUseSearch);
                                if (bRead)
                                {
                                    if (bUseSearch)
                                    {
                                        nStrRef = CClientExoApp::GUI_STRING_SEARCH;
                                    }
                                }
                                else if (pPlaceable->m_bIsBodyBag)
                                {
                                    nStrRef = CClientExoApp::GUI_STRING_SEARCH;
                                }
                                m_aDefaultActions[0].m_sName = g_pAppManager->GetClientExoApp()->GetGUIString(nStrRef);
                                m_aDefaultActions[0].m_nAction = CSWGuiInterfaceAction::ACTION_USE_PLACEABLE;
                                m_aDefaultActions[0].m_pfnAction = (PFNMENUACTION)&CSWCPlaceable::UsePlaceable;
                                m_aDefaultActions[0].m_oidTarget = pTarget->GetId();
                                m_aDefaultActions[0].m_resIcon = "i_openplace";
                                unsigned long oidPlayer = g_pAppManager->GetServerExoApp()->GetPlayerCreatureId();
                                CSWSPlayer* pServerPC = g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(oidPlayer);
                                unsigned long oidServerPC = pServerPC->m_oidPCObject;
                                unsigned long oidPC = g_pAppManager->GetClientExoApp()->ServerToClientObjectId(oidServerPC);
                                CSWCObject* pObject = g_pAppManager->GetClientExoApp()->GetGameObject(oidPC);
                                if (pObject != NULL && pObject->AsSWCCreature() != NULL)
                                {
                                    CSWCCreature* pPC = pObject->AsSWCCreature();
                                    if (pPlayer->GetId() != oidPC && pPC->m_bInParty &&
                                        pPC->GetServerCreature() != NULL && pPC->GetServerCreature()->GetDeadTemp() &&
                                        pTarget->AsSWCPlaceable()->GetServerPlaceable() != NULL &&
                                        pTarget->AsSWCPlaceable()->GetServerPlaceable()->m_bPartyInteract == 0)
                                    {
                                        m_aDefaultActions[0].m_bEnabled = 0;
                                        m_aDefaultActions[0].m_nDisabledReason = CSWGuiInterfaceAction::DISABLED_REASON_DEAD;
                                    }
                                }
                                nAction = 1;
                            }
                            if (pTarget->AsSWCPlaceable()->m_bPlotObject == 0 && nRestrictMode == 0 &&
                                pTarget->AsSWCPlaceable()->m_bLocked)
                            {
                                m_aDefaultActions.SetSize(nAction + 1);
                                m_aDefaultActions[nAction].m_sName = g_pAppManager->GetClientExoApp()->GetGUIString(CClientExoApp::GUI_STRING_BASH);
                                m_aDefaultActions[nAction].m_nAction = CSWGuiInterfaceAction::ACTION_BASH;
                                m_aDefaultActions[nAction].m_pfnAction = (PFNMENUACTION)&CSWCPlaceable::BashPlaceable;
                                m_aDefaultActions[nAction].m_oidTarget = pTarget->GetId();
                                m_aDefaultActions[nAction].m_resIcon = "i_attack";
                                unsigned long oidPlayer = g_pAppManager->GetServerExoApp()->GetPlayerCreatureId();
                                CSWSPlayer* pServerPC = g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(oidPlayer);
                                unsigned long oidServerPC = pServerPC->m_oidPCObject;
                                unsigned long oidPC = g_pAppManager->GetClientExoApp()->ServerToClientObjectId(oidServerPC);
                                CSWCObject* pObject = g_pAppManager->GetClientExoApp()->GetGameObject(oidPC);
                                if (pObject != NULL && pObject->AsSWCCreature() != NULL)
                                {
                                    CSWCCreature* pPC = pObject->AsSWCCreature();
                                    if (pPlayer->GetId() != oidPC && pPC->m_bInParty &&
                                        pPC->GetServerCreature() != NULL && pPC->GetServerCreature()->GetDeadTemp() &&
                                        pTarget->AsSWCPlaceable()->GetServerPlaceable() != NULL &&
                                        pTarget->AsSWCPlaceable()->GetServerPlaceable()->m_bPartyInteract == 0)
                                    {
                                        m_aDefaultActions[nAction].m_bEnabled = 0;
                                        m_aDefaultActions[nAction].m_nDisabledReason = CSWGuiInterfaceAction::DISABLED_REASON_DEAD;
                                    }
                                }
                            }
                        }
                        else
                        {
                            if (pTarget->AsSWCPlaceable()->m_bUseable && pPlaceable != NULL && pPlaceable->m_bIsCorpse == 0)
                            {
                                m_aDefaultActions.SetSize(nAction + 1);
                                m_aDefaultActions[0].m_sName = g_pAppManager->GetClientExoApp()->GetGUIString(CClientExoApp::GUI_STRING_USE);
                                m_aDefaultActions[0].m_nAction = CSWGuiInterfaceAction::ACTION_USE_PLACEABLE;
                                m_aDefaultActions[0].m_pfnAction = (PFNMENUACTION)&CSWCPlaceable::UsePlaceable;
                                m_aDefaultActions[0].m_oidTarget = pTarget->GetId();
                                m_aDefaultActions[0].m_resIcon = "i_useplace";
                                nAction = 1;
                            }
                            if (pTarget->AsSWCPlaceable()->m_bPlotObject == 0 && nRestrictMode == 0)
                            {
                                m_aDefaultActions.SetSize(nAction + 1);
                                m_aDefaultActions[nAction].m_sName = g_pAppManager->GetClientExoApp()->GetGUIString(CClientExoApp::GUI_STRING_BASH);
                                m_aDefaultActions[nAction].m_nAction = CSWGuiInterfaceAction::ACTION_BASH;
                                m_aDefaultActions[nAction].m_pfnAction = (PFNMENUACTION)&CSWCPlaceable::BashPlaceable;
                                m_aDefaultActions[nAction].m_oidTarget = pTarget->GetId();
                                m_aDefaultActions[nAction].m_resIcon = "i_attack";
                            }
                        }
                    }
                    else if (pTarget->AsSWCDoor() != NULL)
                    {
                        CSWSDoor* pDoor = pTarget->AsSWCDoor()->GetServerDoor();
                        int nRestrictMode = pTarget->m_pArea->GetServerArea()->m_nPlayerRestrictMode;
                        CExoString sText;
                        if (pTarget->AsSWCDoor()->GetIsOpen() == 0 && pTarget->AsSWCDoor()->m_bStatic == 0)
                        {
                            m_aDefaultActions.SetSize(nAction + 1);
                            m_aDefaultActions[0].m_sName = g_pAppManager->GetClientExoApp()->GetGUIString(CClientExoApp::GUI_STRING_OPEN);
                            m_aDefaultActions[0].m_nAction = CSWGuiInterfaceAction::ACTION_OPEN_DOOR;
                            m_aDefaultActions[0].m_pfnAction = (PFNMENUACTION)&CSWCDoor::ToggleDoorState;
                            m_aDefaultActions[0].m_oidTarget = pTarget->GetId();
                            m_aDefaultActions[0].m_resIcon = "i_opendoor";
                            if (pTarget->AsSWCDoor()->m_bLocked == 0 && nRestrictMode == 0 && pTarget->AsSWCDoor()->m_bLockable)
                            {
                                m_aDefaultActions.SetSize(nAction + 2);
                                m_aDefaultActions[1].m_sName = g_pAppManager->GetClientExoApp()->GetGUIString(CClientExoApp::GUI_STRING_BASH);
                                m_aDefaultActions[1].m_nAction = CSWGuiInterfaceAction::ACTION_BASH;
                                m_aDefaultActions[1].m_pfnAction = (PFNMENUACTION)&CSWCDoor::MenuActionBash;
                                m_aDefaultActions[1].m_oidTarget = pTarget->GetId();
                                m_aDefaultActions[1].m_resIcon = "i_attack";
                            }
                        }
                    }
                    break;
                case CSWCCreature::INTERFACE_TARGET_TRIGGER:
                    if (pTarget->AsSWCTrigger() != NULL)
                    {
                        if (pPlayer->m_pStats->GetCanUseSkill(CSWRules::SKILL_DEMOLITIONS) && pTarget->AsSWCTrigger()->IsHostile())
                        {
                            m_aDefaultActions.SetSize(nAction + 1);
                            m_aDefaultActions[0].m_sName = g_pAppManager->GetClientExoApp()->GetGUIString(CClientExoApp::GUI_STRING_DISABLE_MINE);
                            m_aDefaultActions[0].m_pfnAction = (PFNMENUACTION)&CSWCTrigger::ActionMenuDisableMine;
                            m_aDefaultActions[0].m_oidTarget = pTarget->GetId();
                            m_aDefaultActions[0].m_nAction = CSWGuiInterfaceAction::ACTION_DISABLE_MINE;
                            m_aDefaultActions[0].m_resIcon = "i_disablemine";
                            nAction = 1;
                        }
                        if (pPlayer->m_pStats->GetCanUseSkill(CSWRules::SKILL_DEMOLITIONS))
                        {
                            m_aDefaultActions.SetSize(nAction + 1);
                            m_aDefaultActions[nAction].m_sName = g_pAppManager->GetClientExoApp()->GetGUIString(CClientExoApp::GUI_STRING_RECOVER_MINE);
                            m_aDefaultActions[nAction].m_pfnAction = (PFNMENUACTION)&CSWCTrigger::ActionMenuRecoverMine;
                            m_aDefaultActions[nAction].m_oidTarget = pTarget->GetId();
                            m_aDefaultActions[nAction].m_nAction = CSWGuiInterfaceAction::ACTION_RECOVER_MINE;
                            m_aDefaultActions[nAction].m_resIcon = "i_recovermine";
                        }
                    }
                    break;
                case CSWCCreature::INTERFACE_TARGET_HOSTILE:
                    if (pTarget->AsSWCCreature() != NULL)
                    {
                        CSWSCreature* pCreature = pTarget->AsSWCCreature()->GetServerCreature();
                        if (pCreature != NULL && pCreature->GetArea()->m_nPlayerRestrictMode == 0)
                        {
                            m_aDefaultActions.SetSize(nAction + 1);
                            m_aDefaultActions[0].m_sName = g_pAppManager->GetClientExoApp()->GetGUIString(CSWGuiInterfaceAction::STRREF_ATTACK);
                            m_aDefaultActions[0].m_pfnAction = (PFNMENUACTION)&CSWCCreature::ActionMenuAttack;
                            m_aDefaultActions[0].m_oidTarget = pTarget->GetId();
                            m_aDefaultActions[0].m_nAction = CSWGuiInterfaceAction::ACTION_ATTACK;
                            m_aDefaultActions[0].m_resIcon = "i_attack";
                        }
                    }
                    break;
                }
            }
        }
    }
}
#endif
#ifndef _XBOX
int CClientExoAppInternal::HandleInputEvent(int nEvent, int nValue)  // PC 0x006717B0
{
    int nResult = 0;
    switch (nEvent)
    {
    case INPUT_EVENT_MOUSE_X:
        nResult = PerformXMouseMoveAction(nValue);
        break;
    case INPUT_EVENT_MOUSE_Y:
        nResult = PerformYMouseMoveAction(nValue);
        break;
    case INPUT_EVENT_MOUSE_LEFT:
#ifndef _XBOX
        if (nValue != 0)
            PerformLButtonDownAction(nValue == CExoInput::MOUSE_BUTTON_DOUBLE_CLICK);
        else
            PerformLButtonUpAction();
#endif
        break;
    case INPUT_EVENT_MOUSE_RIGHT:
        if (nValue != 0)
            PerformRButtonDownAction();
        else
            PerformRButtonUpAction();
        break;
    case INPUT_EVENT_MOUSE_MIDDLE:
    case INPUT_EVENT_RESERVED_246:
    case INPUT_EVENT_RESERVED_247:
    case GUIEVENT_ACTION_MENU_UP:
    case GUIEVENT_ACTION_MENU_DOWN:
    case INPUT_EVENT_RESERVED_252:

        if (nValue != 0)
        {
        }
        break;
    case INPUT_EVENT_MOUSE_WHEEL:
#ifndef _XBOX
        g_pGuiMan->HandleZMouse(nValue);
#endif
        break;
    case INPUT_EVENT_NEXT_CHARACTER:
    case GUIEVENT_NEXT_CHARACTER:
        if (nValue != 0)
        {
            if (m_pClientOptions->m_nCameraMode == CClientOptions::CAMERA_MODE_WORLD_CLICK_DISABLED)
            {
                g_bShowIndicator = 1;
                GetModule()->RestoreCamera();
                SetInputClass(INPUT_CLASS_GAME, 1);
            }
            else
            {
                ChangeCharacterToNextLivingPartyMember(0, 1, 1);
                g_pAppManager->GetClientExoApp()->GetInGameGui()->ChangeCharacter();
            }
            g_pAppManager->GetClientExoApp()->GetSWGuiManager()->PlayGuiSound(CSWGuiManager::GUISOUND_WORLD_ACTION);
        }
        break;
    case INPUT_EVENT_SOLO_MODE:
    case INPUT_EVENT_SOLO_TOGGLE:
        if (nValue != 0)
        {
            if (m_pClientOptions->m_nCameraMode == CClientOptions::CAMERA_MODE_WORLD_CLICK_DISABLED)
            {
                g_bShowIndicator = 1;
                GetModule()->RestoreCamera();
                SetInputClass(INPUT_CLASS_GAME, 1);
            }
            else if (m_pGuiInGame != NULL && m_pGuiInGame->m_pPartySelection == 0)
                m_pGuiInGame->ShowSoloModeQuery(0);
        }
        break;
    case INPUT_EVENT_EXIT:
    case GUIEVENT_EXIT:
        if (nValue != 0)
        {
            int bRestoredCamera = 0;
            int bHandled = 0;
            if (m_pClientOptions->m_nCameraMode == CClientOptions::CAMERA_MODE_WORLD_CLICK_DISABLED)
            {
                g_bShowIndicator = 1;
                GetModule()->RestoreCamera();
                SetInputClass(INPUT_CLASS_GAME, 1);
                bRestoredCamera = 1;
            }
            if (!GetControlledCharacterDead())
            {
                if (m_pGuiInGame->m_nGlobalDialogState == 0 &&
                    g_pAppManager->GetClientExoApp()->GetSWParty() != NULL &&
                    g_pAppManager->GetClientExoApp()->GetSWParty()->GetPlayerCharacter() != NULL)
                {
                    if (m_pGuiInGame->m_bShowPauseGui == CGuiInGame::GUISTATE_HIDDEN)
                    {
                        if (m_pGuiInGame->HideSWInGameGui(0))
                        {
                            SetInputClass(INPUT_CLASS_GAME, 1);
                            bHandled = 1;
                        }
                    }
                    else if (m_nInputClass == INPUT_CLASS_GAME &&
                             m_pGuiInGame->ShowSWInGameGui(CGuiInGame::SCREEN_OPTIONS))
                    {
                        SetInputClass(INPUT_CLASS_GUI, 1);
                        g_pAppManager->GetClientExoApp()->ShowTutorialWindow(TUTORIAL_OPTIONS, OBJECT_INVALID, OBJECT_INVALID, 0);
                        bHandled = 1;
                    }
                }
            }
            else
            {
                bHandled = 1;
                if (m_nGameOver)
                    HurryDeathSequence();
                else
                {
                CSWGuiMessageBox* pMessageBox = m_pGuiInGame->m_pMessageBox;
                pMessageBox->SetAllowCancel(0);
                pMessageBox->SetCallback(NULL, NULL, NULL);
                pMessageBox->SetMessage(STRREF_PLAYER_DEAD);
                m_pSWGuiManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
                }
            }
            if (!bHandled && !bRestoredCamera)
                m_pSWGuiManager->HandleInputEvent(180, 1);
        }
        break;
    case INPUT_EVENT_SCREENSHOT:
        if (nValue != 0)
            DoScreenShot();
        break;
    case INPUT_EVENT_PAUSE:
    case INPUT_EVENT_PAUSE_TOGGLE:
    case INPUT_EVENT_PAUSE_ALTERNATE:
        if (nValue != 0)
        {
            if (m_pClientOptions->m_nCameraMode == CClientOptions::CAMERA_MODE_WORLD_CLICK_DISABLED)
            {
                g_bShowIndicator = 1;
                GetModule()->RestoreCamera();
                SetInputClass(INPUT_CLASS_GAME, 1);
            }
            int bPaused = GetPausedByCombat();
            SetPausedByCombat(!bPaused, PAUSE_REASON_INPUT, 0);
            if (g_pAppManager->GetClientExoApp()->GetAutoPaused())
                g_pAppManager->GetClientExoApp()->SetAutoPaused(0, 0);
            if (!bPaused && g_pAppManager->GetClientExoApp()->ShowTutorialWindow(TUTORIAL_PAUSE, OBJECT_INVALID, OBJECT_INVALID, 0))
                break;
            g_pAppManager->GetClientExoApp()->GetSWGuiManager()->PlayGuiSound(CSWGuiManager::GUISOUND_WORLD_ACTION);
        }
        break;
    case INPUT_EVENT_RESTORE_CAMERA:
    case INPUT_EVENT_CAMERA_RESET:
        if (nValue != 0 && m_pClientOptions->m_nCameraMode == CClientOptions::CAMERA_MODE_WORLD_CLICK_DISABLED)
        {
            g_bShowIndicator = 1;
            GetModule()->RestoreCamera();
            SetInputClass(INPUT_CLASS_GAME, 1);
        }
        break;
    case INPUT_EVENT_FREE_LOOK:
    case INPUT_EVENT_FREE_LOOK_TOGGLE:
        if (GetPlayerCreature() != NULL && GetPlayerCreature()->m_nCombatState == 0 &&
            nValue != 0 && m_nGameOver == 0)
        {
            CSWSCreature* pServerCreature;
            if (m_pClientOptions->m_nCameraMode != CClientOptions::CAMERA_MODE_WORLD_CLICK_DISABLED &&
                GetPlayerCreature()->GetServerCreature() != NULL &&
                (pServerCreature = GetPlayerCreature()->GetServerCreature())->m_lstActions.GetHeadPos() == NULL &&
                g_pAppManager->GetClientExoApp()->GetPausedByCombat() == 0)
            {
                g_bShowIndicator = 0;
                if (g_nHSMode != 1)
                {
                    GetModule()->SetFreeLookCamera(m_pSWParty->GetPlayerCharacter()->GetId());
                    SetInputClass(INPUT_CLASS_FREE_LOOK, 1);
                }
                else
                {
                    GetModule()->SetNavigateCamera();
                    ++g_nHSMode;
                }
            }
            else if (m_pClientOptions->m_nCameraMode == CClientOptions::CAMERA_MODE_WORLD_CLICK_DISABLED)
            {
                g_bShowIndicator = 1;
                GetModule()->RestoreCamera();
                SetInputClass(INPUT_CLASS_GAME, 1);
            }
        }
        break;
    case INPUT_EVENT_MOVE_FORWARD:
    case INPUT_EVENT_MOVE_BACK:
    case INPUT_EVENT_CAMERA_LEFT:
    case INPUT_EVENT_CAMERA_RIGHT:
    case INPUT_EVENT_CAMERA_UP:
    case INPUT_EVENT_CAMERA_DOWN:
        if (nValue != 0 && m_nGameOver == 0 &&
            m_pClientOptions->m_nCameraMode == CClientOptions::CAMERA_MODE_WORLD_CLICK_DISABLED)
        {
            g_bShowIndicator = 1;
            GetModule()->RestoreCamera();
            SetInputClass(INPUT_CLASS_GAME, 1);
        }
        break;
    case INPUT_EVENT_PREVIOUS_TARGET:
    case INPUT_EVENT_SELECT_PREVIOUS:
        if (nValue != 0)
        {
            if (g_pAppManager->GetClientExoApp()->GetInGameGui() != NULL)
            {
                CSWGuiMainInterface* pMainInterface = g_pAppManager->GetClientExoApp()->GetInGameGui()->m_pMainInterface;
                if (pMainInterface != NULL)
                    pMainInterface->m_bSelectingUseItemTarget = 0;
            }
            if (m_pClientOptions->m_nCameraMode == CClientOptions::CAMERA_MODE_WORLD_CLICK_DISABLED)
            {
                g_bShowIndicator = 1;
                GetModule()->RestoreCamera();
                SetInputClass(INPUT_CLASS_GAME, 1);
            }
            else
            {
                SelectNearestObject(1, m_nCombatMode == 0 ? 0 : 2);
                if (m_nCombatMode != 0 && m_pClientOptions->m_bAutopauseNewTargetSelected)
                    SetAutoPaused(1, AUTOPAUSE_NEW_TARGET);
            }
        }
        break;
    case INPUT_EVENT_NEXT_TARGET:
    case INPUT_EVENT_SELECT_NEXT:
        if (nValue != 0)
        {
            if (g_pAppManager->GetClientExoApp()->GetInGameGui()->m_pMainInterface != NULL)
            {
                g_pAppManager->GetClientExoApp()->GetInGameGui()->m_pMainInterface->m_bSelectingUseItemTarget = 0;
            }
            if (m_pClientOptions->m_nCameraMode == CClientOptions::CAMERA_MODE_WORLD_CLICK_DISABLED)
            {
                g_bShowIndicator = 1;
                GetModule()->RestoreCamera();
                SetInputClass(INPUT_CLASS_GAME, 1);
            }
            else
            {
                SelectNearestObject(0, m_nCombatMode == 0 ? 0 : 2);
                if (m_nCombatMode != 0 && m_pClientOptions->m_bAutopauseNewTargetSelected)
                    SetAutoPaused(1, AUTOPAUSE_NEW_TARGET);
            }
        }
        break;
    case INPUT_EVENT_EQUIP:
    case INPUT_EVENT_INVENTORY:
    case INPUT_EVENT_CHARACTER:
    case INPUT_EVENT_ABILITIES:
    case INPUT_EVENT_MESSAGES:
    case INPUT_EVENT_JOURNAL:
    case INPUT_EVENT_MAP:
    case INPUT_EVENT_OPTIONS:
        if (nValue != 0)
        {
            if (m_pClientOptions->m_nCameraMode == CClientOptions::CAMERA_MODE_WORLD_CLICK_DISABLED)
            {
                g_bShowIndicator = 1;
                GetModule()->RestoreCamera();
                SetInputClass(INPUT_CLASS_GAME, 1);
            }
            if (!GetControlledCharacterDead())
            {
                if (m_pGuiInGame->m_nGlobalDialogState == 0 && m_pSWGuiManager->m_lModalPanels.num == 0)
                {
                    int nScreen = nEvent - INPUT_EVENT_EQUIP;
                    if (m_pGuiInGame->m_bShowPauseGui == CGuiInGame::GUISTATE_HIDDEN)
                    {
                        if (m_pGuiInGame->m_nLastGuiPanel == nScreen)
                        {
                            if (m_pGuiInGame->HideSWInGameGui(0))
                                SetInputClass(INPUT_CLASS_GAME, 1);
                        }
                        else
                            m_pGuiInGame->SwitchToSWInGameGui(nScreen);
                    }
                    else if (m_pGuiInGame->ShowSWInGameGui(nScreen))
                        SetInputClass(INPUT_CLASS_GUI, 1);
                }
            }
            else if (m_nGameOver)
                HurryDeathSequence();
        }
        break;
    case INPUT_EVENT_ACTION_UP:
        if (m_pGuiInGame != NULL && m_pGuiInGame->m_bShowPauseGui == CGuiInGame::GUISTATE_MAIN_INTERFACE && m_pGuiInGame->m_pMainInterface != NULL)
            m_pGuiInGame->m_pMainInterface->HandleInputEvent(GUIEVENT_UP_ALTERNATE, nValue);
        break;
    case INPUT_EVENT_ACTION_DOWN:
        if (m_pGuiInGame != NULL && m_pGuiInGame->m_bShowPauseGui == CGuiInGame::GUISTATE_MAIN_INTERFACE && m_pGuiInGame->m_pMainInterface != NULL)
            m_pGuiInGame->m_pMainInterface->HandleInputEvent(GUIEVENT_DOWN_ALTERNATE, nValue);
        break;
    case INPUT_EVENT_QUICK_SAVE:
        if (nValue != 0)
        {
            g_pExoInput->ClearEvents();
            g_pAppManager->GetClientExoApp()->ResetDriveAcceleration();
            DoQuickSave();
            g_pExoInput->CoolDownEvent(INPUT_EVENT_QUICK_LOAD, QUICK_LOAD_COOLDOWN);
        }
        break;
    case INPUT_EVENT_QUICK_LOAD:
        if (nValue != 0 && g_pAppManager->m_pReentrantServerStats->m_bOperationActive == 0)
        {
            g_pExoInput->ClearEvents();
            g_bLeftMouseDown = 0;
            g_bMouseCameraRotateToggle = 0;
            g_pAppManager->GetClientExoApp()->ResetDriveAcceleration();
            g_pAppManager->GetClientExoApp()->GetInGameGui()->DoQuickLoad();
        }
        break;
    case INPUT_EVENT_STEALTH:
        if (nValue != 0 && g_pAppManager->GetClientExoApp()->GetPlayerCreature()->IsStealthCapable())
            m_pSWParty->GetPlayerCharacter()->PartyMenuStealth(0, NULL);
        break;
    case INPUT_EVENT_COMBAT_PAUSE:
        if (nValue != 0)
        {
            if (m_pClientOptions->m_nCameraMode == CClientOptions::CAMERA_MODE_WORLD_CLICK_DISABLED)
            {
                g_bShowIndicator = 1;
                GetModule()->RestoreCamera();
                SetInputClass(INPUT_CLASS_GAME, 1);
            }
            int bPaused = GetPausedByCombat();
            SetPausedByCombat(!bPaused, PAUSE_REASON_INPUT, 0);
            if (g_pAppManager->GetClientExoApp()->GetAutoPaused())
                g_pAppManager->GetClientExoApp()->SetAutoPaused(0, 0);
            if (!bPaused && g_pAppManager->GetClientExoApp()->ShowTutorialWindow(TUTORIAL_PAUSE, OBJECT_INVALID, OBJECT_INVALID, 0))
                break;
            g_pAppManager->GetClientExoApp()->GetSWGuiManager()->PlayGuiSound(CSWGuiManager::GUISOUND_WORLD_ACTION);
        }
        break;
    case INPUT_EVENT_FLOURISH:
        if (nValue != 0)
            g_pAppManager->GetClientExoApp()->PlayerFlourishWeapons();
        break;
    case INPUT_EVENT_SWAP_WEAPONS:
        if (nValue != 0)
            g_pAppManager->GetClientExoApp()->GetInGameGui()->GetMainInterface()->OnSwapWeaponsPressed(NULL);
        break;
    case INPUT_EVENT_FIRE_SECONDARY:
    case INPUT_EVENT_MINIGAME_FIRE:
        if (nValue != 0 && m_pModule != NULL && m_pModule->m_pInfo != NULL && !GetPausedByCombat())
        {
            CSWMiniGame* pMiniGame = m_pModule->m_pInfo->m_pMiniGame;
            if (pMiniGame != NULL && pMiniGame->GetPlayer() != NULL)
            {
                if (pMiniGame->m_nAttachType == CSWMiniGame::ATTACH_CAMERA_HOOK)
                    pMiniGame->GetPlayer()->Fire();
                else
                    pMiniGame->GetPlayer()->OnBrake(0);
            }
        }
        break;
    case INPUT_EVENT_FIRE:
        if (nValue != 0 && m_pModule != NULL && m_pModule->m_pInfo != NULL && !GetPausedByCombat())
        {
            CSWMiniGame* pMiniGame = m_pModule->m_pInfo->m_pMiniGame;
            if (pMiniGame != NULL && pMiniGame->GetPlayer() != NULL)
            {
                if (pMiniGame->m_nAttachType == CSWMiniGame::ATTACH_CAMERA_HOOK)
                    pMiniGame->GetPlayer()->Fire();
                else
                    pMiniGame->GetPlayer()->OnAccelerate(0);
            }
        }
        break;
    case INPUT_EVENT_ROTATE_LEFT:
    case INPUT_EVENT_ROTATE_RIGHT:
        g_bMouseCameraRotateToggle = nValue;
        break;
    case INPUT_EVENT_SHIFT_LEFT:
    case INPUT_EVENT_SHIFT_RIGHT:
        g_bShiftActionState = nValue;
        break;
    case INPUT_EVENT_TOOLTIPS:
#ifndef _XBOX
        if (nValue != 0)
            SetToolTipInstantMode(1);
        else
            SetToolTipInstantMode(0);
#endif
        break;
    case GUIEVENT_ACTIVATE_FRIENDLY_POWER:
        if (nValue != 0)
        {
            if (g_bShiftActionState)
            {
                CGuiInGame* pGuiInGame =
                    g_pAppManager->GetClientExoApp()->GetInGameGui();
                pGuiInGame->m_pMainInterface->SelectNextPersonalAction(0);
            }
            else
            {
                CGuiInGame* pGuiInGame =
                    g_pAppManager->GetClientExoApp()->GetInGameGui();
                pGuiInGame->m_pMainInterface->DoPersonalAction(0, 1);
            }
        }
        break;
    case GUIEVENT_ACTIVATE_MEDICAL_ITEM:
        if (nValue != 0)
        {
            if (g_bShiftActionState)
            {
                CGuiInGame* pGuiInGame =
                    g_pAppManager->GetClientExoApp()->GetInGameGui();
                pGuiInGame->m_pMainInterface->SelectNextPersonalAction(1);
            }
            else
            {
                CGuiInGame* pGuiInGame =
                    g_pAppManager->GetClientExoApp()->GetInGameGui();
                pGuiInGame->m_pMainInterface->DoPersonalAction(1, 1);
            }
        }
        break;
    case GUIEVENT_ACTIVATE_MINE:
        if (nValue != 0)
        {
            if (g_bShiftActionState)
            {
                CGuiInGame* pGuiInGame =
                    g_pAppManager->GetClientExoApp()->GetInGameGui();
                pGuiInGame->m_pMainInterface->SelectNextPersonalAction(3);
            }
            else
            {
                CGuiInGame* pGuiInGame =
                    g_pAppManager->GetClientExoApp()->GetInGameGui();
                pGuiInGame->m_pMainInterface->DoPersonalAction(3, 1);
            }
        }
        break;
    case GUIEVENT_ACTIVATE_NON_MEDICAL_ITEM:
        if (nValue != 0)
        {
            if (g_bShiftActionState)
            {
                CGuiInGame* pGuiInGame =
                    g_pAppManager->GetClientExoApp()->GetInGameGui();
                pGuiInGame->m_pMainInterface->SelectNextPersonalAction(2);
            }
            else
            {
                CGuiInGame* pGuiInGame =
                    g_pAppManager->GetClientExoApp()->GetInGameGui();
                pGuiInGame->m_pMainInterface->DoPersonalAction(2, 1);
            }
        }
        break;
    case GUIEVENT_ACTIVATE_LEFT_ACTION:
        if (nValue != 0)
        {
            if (g_bShiftActionState)
            {
                CGuiInGame* pGuiInGame =
                    g_pAppManager->GetClientExoApp()->GetInGameGui();
                pGuiInGame->m_pMainInterface->m_cTargetActionMenu.SelectNextAction(0);
            }
            else
            {
                CGuiInGame* pGuiInGame =
                    g_pAppManager->GetClientExoApp()->GetInGameGui();
                pGuiInGame->m_pMainInterface->DoTargetAction(0, 1);
            }
        }
        break;
    case GUIEVENT_ACTIVATE_MIDDLE_ACTION:
        if (nValue != 0)
        {
            if (g_bShiftActionState)
            {
                CGuiInGame* pGuiInGame =
                    g_pAppManager->GetClientExoApp()->GetInGameGui();
                pGuiInGame->m_pMainInterface->m_cTargetActionMenu.SelectNextAction(1);
            }
            else
            {
                CGuiInGame* pGuiInGame =
                    g_pAppManager->GetClientExoApp()->GetInGameGui();
                pGuiInGame->m_pMainInterface->DoTargetAction(1, 1);
            }
        }
        break;
    case GUIEVENT_ACTIVATE_RIGHT_ACTION:
        if (nValue != 0)
        {
            if (g_bShiftActionState)
            {
                CGuiInGame* pGuiInGame =
                    g_pAppManager->GetClientExoApp()->GetInGameGui();
                pGuiInGame->m_pMainInterface->m_cTargetActionMenu.SelectNextAction(2);
            }
            else
            {
                CGuiInGame* pGuiInGame =
                    g_pAppManager->GetClientExoApp()->GetInGameGui();
                pGuiInGame->m_pMainInterface->DoTargetAction(2, 1);
            }
        }
        break;
    case INPUT_EVENT_DEFAULT_ACTION:
        if (nValue != 0 && m_nLastTarget != OBJECT_INVALID)
        {
#ifndef _XBOX
            GetDefaultActions();
            if (m_aDefaultActions.num > 0)
            {
                CSWGuiInterfaceAction* pAction = m_aDefaultActions.element;
                if (pAction != NULL)
                {
                    CSWCCreature* pPlayer = g_pAppManager->GetClientExoApp()->GetSWParty()->GetPlayerCharacter();
                    CSWCObject* pGameObject = g_pAppManager->GetClientExoApp()->GetGameObject(pAction->m_oidTarget);
                    if (pGameObject != NULL)
                    {
                        CSWCObject* pObject = pGameObject->AsSWCObject();
                        if (pObject != NULL)
                            (pObject->*pAction->m_pfnAction)(pAction->m_nAction, pPlayer);
                    }
                }
            }
#endif
        }
        break;
    case INPUT_EVENT_CANCEL_COMBAT:
        if (nValue != 0)
        {
            CSWMiniGame* pMiniGame = g_pAppManager->GetClientExoApp()->GetMiniGame();
            if (pMiniGame != NULL && g_pAppManager->GetClientExoApp()->GetPausedByCombat())
            {
                int nAttachType = pMiniGame->m_nAttachType;
                if (nAttachType == CSWMiniGame::ATTACH_PLAYER_MODEL)
                {
                    {
                        CExoString sQuitSwoop("GBL_QUIT_SWOOP");
                        g_pAppManager->GetServerExoApp()->GetGlobalVariableTable()->SetValueBoolean(sQuitSwoop, nAttachType);
                    }
                    g_pAppManager->GetClientExoApp()->SetPausedByCombat(0, PAUSE_REASON_INPUT, 0);
                    g_pAppManager->GetClientExoApp()->SetAutoPaused(0, 0);
                }
            }
            else if (g_pAppManager->GetClientExoApp()->GetCombatMode())
            {
                CSWCCreature* pPlayer = g_pAppManager->GetClientExoApp()->GetSWParty()->GetPlayerCharacter();
                if (pPlayer != NULL)
                {
                    pPlayer->SetCombatMode(0);
                    pPlayer->ClearAllActions();
                }
            }
        }
        break;
    case INPUT_EVENT_CLEAR_ACTION:
    case INPUT_EVENT_CLEAR_ACTION_ALTERNATE:
        if (nValue != 0 && m_pGuiInGame != NULL && m_pGuiInGame->m_pMainInterface != NULL)
            m_pGuiInGame->m_pMainInterface->ClearOneAction();
        break;
    case INPUT_EVENT_QUEUE_ACTION:
        if (nValue != 0 && m_pGuiInGame != NULL && m_pGuiInGame->m_pMainInterface != NULL)
            m_pGuiInGame->m_pMainInterface->QueueActiveAction();
        break;
    case INPUT_EVENT_GUI_ACCEPT:
        if (nValue != 0 && m_nInputClass == INPUT_CLASS_GUI)
            m_pSWGuiManager->HandleInputEvent(nEvent, nValue);
        break;
    }
    return nResult;
}
#endif
#ifndef _XBOX
void CClientExoAppInternal::HandleMouseOver(unsigned long oidObject,  // PC 0x00672810
                                          const Vector& vPosition,
                                          int bForceUpdate)
{
    m_vMouseOverPosition = vPosition;
    m_oidMouseOverTarget = oidObject;
    if (m_pGuiInGame != NULL &&
        (m_pGuiInGame->m_pPartySelection != NULL ||
         m_pGuiInGame->m_nGlobalDialogState == 1 ||
         m_pClientOptions->m_nCameraMode == CClientOptions::CAMERA_MODE_WORLD_CLICK_DISABLED))
    {
        SetMouseCursor(CSWGuiManager::CURSOR_DEFAULT, 0, 0);
        return;
    }

#ifndef _XBOX
    CSWGuiControl* pMouseOverControl = g_pGuiMan->m_pMouseOverControl;
#else
    CSWGuiControl* pMouseOverControl = NULL;
#endif
    if (bForceUpdate == 0 &&
        (oidObject == OBJECT_INVALID || m_nLastTarget == OBJECT_INVALID ||
         oidObject == g_pAppManager->m_pClientExoApp->GetPlayerCreatureId() ||
         pMouseOverControl != NULL || m_nLastTarget != m_oidMouseOverTarget))
    {
        if (m_oidMouseOverTarget != OBJECT_INVALID &&
            m_aNearestObjects.IndexOf((void*)m_oidMouseOverTarget,
                                      TestNearestStruct) != -1 &&
            pMouseOverControl == NULL)
        {
            SetMouseCursor(CSWGuiManager::CURSOR_UNSELECTED_TARGET, 0, 0);
            return;
        }
#ifdef _XBOX
        SetMouseCursor(CSWGuiManager::CURSOR_DEFAULT, 0, 0);
#else
        SetMouseCursor(CSWGuiManager::CURSOR_DEFAULT, 0,
                       g_pExoInput->GetLastSetMouseDownState());
#endif
    }
    else
    {
        int nCursor = CSWGuiManager::CURSOR_DEFAULT;
        CSWGuiInterfaceAction* pAction = NULL;
        GetDefaultActions();
        int bMultipleActions = 0;
        if (m_aDefaultActions.num > 0)
        {
            pAction = m_aDefaultActions.element;
            bMultipleActions = m_aDefaultActions.num > 1;
        }
        int bMouseDown = g_bLeftMouseDown;
        if (m_oidMouseOverTarget != OBJECT_INVALID &&
            m_oidMouseOverTarget != m_pGuiInGame->m_pMainInterface->m_nTargetId)
        {
            SetMouseCursor(CSWGuiManager::CURSOR_UNSELECTED_TARGET, 0, 0);
            return;
        }
        if (pAction != NULL)
        {
            nCursor = ActionIconToMouseCursor(pAction->m_nAction,
                                               bMultipleActions);
        }
        SetMouseCursor(nCursor, 0, bMouseDown);
    }
}
#endif
// PC 0x00672A00
void CClientExoAppInternal::PlayBackInputEvents()
{
    if (m_lstPlayBackEvents.GetHeadPos() != NULL)
    {
        int bContinue = 1;
        while (m_lstPlayBackEvents.GetHeadPos() != NULL && bContinue == 1)
        {
            CPlayBackEventNode* pEvent = m_lstPlayBackEvents.GetAtPos(
                m_lstPlayBackEvents.GetHeadPos());
            unsigned __int64 nTime =
                g_pAppManager->m_pClientExoApp->GetWorldTimer()->GetSnapshotTime() / 1000;
            if (pEvent->nTime < nTime)
            {
                pEvent = m_lstPlayBackEvents.RemoveHead();
                if (pEvent->cType == CPlayBackEventNode::KEY_DOWN)
                    HandleWMKeyDownMessage(NULL, pEvent->nKey, 0);
                if (pEvent->cType == CPlayBackEventNode::CHARACTER)
                    HandleWMCharMessage(pEvent->nKey);
                if (pEvent->cType == CPlayBackEventNode::CONNECTION)
                    g_pAppManager->m_pClientExoApp->GetNetLayer()->SetUpPlayBackConnection();
                if (pEvent->cType == CPlayBackEventNode::MESSAGE)
                {
                    HandleMessage(pEvent->nPlayerId, pEvent->pMessage,
                                  pEvent->nMessageSize, pEvent->bGuaranteed);
                    delete pEvent->pMessage;
                }
                if (pEvent->cType == CPlayBackEventNode::INPUT)
                    HandleInputEvent(pEvent->nInputEvent, pEvent->nInputValue);
                if (pEvent->cType == CPlayBackEventNode::INPUT_END)
                {
                    m_bPlayBackInputEnd = 1;
                    HandleInputEvent(pEvent->nInputEvent, pEvent->nInputValue);
                }
                if (pEvent->cType == CPlayBackEventNode::CAMERA)
                {
                    CAurCamera* pCamera = GetModule()->m_pModuleCamera;
                    char szCommand[PLAYBACK_COMMAND_LENGTH];
                    sprintf(szCommand, "dist %f", pEvent->fX);
                    pCamera->BehaviorMessage(szCommand);
                    sprintf(szCommand, "pitch %f", pEvent->fY);
                    pCamera->BehaviorMessage(szCommand);
                    sprintf(szCommand, "yaw %f", pEvent->fZ);
                    pCamera->BehaviorMessage(szCommand);
                }
                if (pEvent->cType == CPlayBackEventNode::POSITION)
                {
                    Vector vPosition;
                    vPosition.m_fX = pEvent->fX;
                    vPosition.m_fY = pEvent->fY;
                    vPosition.m_fZ = pEvent->fZ;
                    unsigned long oidObject = pEvent->oidObject;
                    CGameObject* pObject;
                    CGameObjectArray* pObjectArray =
                        g_pAppManager->m_pClientExoApp->GetGameObjectArray();
                    if (pObjectArray->GetGameObject(
                            oidObject, &pObject) == CGameObjectArray::SUCCESS &&
                        pObject->AsSWCCreature() != NULL)
                    {
                        pObject->AsSWCCreature()->SetPosition(vPosition);
                        pObject->AsSWCCreature()->m_vDesiredPosition = vPosition;
                    }
                }
                delete pEvent;
            }
            else
                bContinue = 0;
        }
    }
    else
    {
        m_nCaptureEventPlayBack = 0;
        m_pWorldTimer->SetFixedSnapshotRate(0, 30);
    }
}

inline CClientOptions* CClientExoAppInternal::GetClientOptions()
{
    return m_pClientOptions;
}

#ifndef _XBOX
void CClientExoAppInternal::ProcessInput(float fTimeStep)  // PC 0x00672C80
{
    int bInputHandled = 0;
    int nEvents;
    CExoString sInput;
    if (m_bInputDisabled)
        return;

    int bMouseCameraRotate = g_bMouseCameraRotateToggle;
    if (g_pAppManager->GetClientExoApp()->GetClientOptions()->m_bMouseLook &&
        m_nInputClass != INPUT_CLASS_GUI && m_nInputClass != INPUT_CLASS_DIALOG)
        bMouseCameraRotate = !bMouseCameraRotate;

    if (g_pAppManager->GetClientExoApp()->GetCaptureEventPlayBack())
    {
        PlayBackInputEvents();
        return;
    }

    float fMouseTime = (int)m_pPauseTimer->GetSnapshotTimeDifference() * 0.000001f;
#ifndef _XBOX
    g_pExoInput->UpdateMouseDelta(fMouseTime);
#endif
    int bPeragusDialog = 0;
    if (g_pAppManager->GetClientExoApp()->GetModule() != NULL)
    {
        int bPeragus = g_pAppManager->GetClientExoApp()->GetModule()->GetModuleTitle().CompareNoCase("107PER");
        if (bPeragus && m_nInputClass == INPUT_CLASS_DIALOG)
            bPeragusDialog = 1;
    }
    if (g_pExoInput->GetActive() || bPeragusDialog)
    {
        int i;
        CExoArrayList<CExoInputEvent> aEvents =
            g_pExoInput->GetEvents(nEvents, m_nInputClass);
        if (m_pGuiInGame->m_bShowPauseGui == CGuiInGame::GUISTATE_MAIN_INTERFACE ||
            m_nInputClass == INPUT_CLASS_GUI || m_nInputClass == INPUT_CLASS_DIALOG)
        {
            int bDirection = 0;
            for (int i = 0; i < nEvents; i++)
            {
                int nValue = aEvents[i].m_nValue;
                if (nValue)
                {
                    if (m_nInputHistoryCount < INPUT_HISTORY_LENGTH)
                    {
                        CExoInputEventDesc* pDesc = aEvents[i].m_pDesc;
                        unsigned long nEvent = pDesc->m_nEvent;
                        if (pDesc != NULL)
                        {
                            m_anInputHistory[m_nInputHistoryCount] = nEvent;
                            m_nInputHistoryCount++;
                        }
                    }
                    else
                    {
                        for (int j = 0; j < INPUT_HISTORY_LENGTH - 1; j++)
                            m_anInputHistory[j] = m_anInputHistory[j + 1];
                        CExoInputEventDesc* pDesc = aEvents[i].m_pDesc;
                        m_anInputHistory[INPUT_HISTORY_LENGTH - 1] = pDesc->m_nEvent;
                    }
                    CExoInputEventDesc* pDesc = aEvents[i].m_pDesc;
                    switch (pDesc->m_nEvent)
                    {
                    case 1: case 2: case 10: case 11:
                    case 39: case 40: case 41: case 42:
                    case 43: case 44: case 45: case 46:
                    case 53: case 54:
                        if (bDirection)
                        {
                            aEvents.DelIndex(i);
                            i--;
                            nEvents--;
                            break;
                        }
                        bDirection = 1;
                    default:
                        if (aEvents[i].m_pDesc->m_nEvent >= 49 &&
                            aEvents[i].m_pDesc->m_nEvent <= 50)
                        {
                            int nInsert = -1;
                            for (int j = i + 1; j < nEvents; j++)
                            {
                                int nValue = aEvents[j].m_nValue;
                                if (nValue)
                                {
                                    CExoInputEventDesc* pNextDesc = aEvents[j].m_pDesc;
                                    unsigned long nNext = pNextDesc->m_nEvent;
                                    if (nNext >= 47 && nNext <= 48)
                                        nInsert = j + 1;
                                }
                            }
                            if (nInsert != -1)
                            {
                                if (nInsert >= aEvents.num)
                                    aEvents.Add(aEvents[i]);
                                else
                                    aEvents.Insert(aEvents[i], nInsert);
                                aEvents.DelIndex(i);
                            }
                        }
                        break;
                    }
                }
            }
        }

        for (i = 0; i < nEvents; i++)
        {
            if (g_pAppManager->GetClientExoApp()->GetCaptureEvents() &&
                g_pAppManager->GetClientExoApp()->GetCaptureEventFile() != NULL)
            {
                g_pExoInput->WriteEventToFile((unsigned long)
                    (g_pAppManager->GetClientExoApp()->GetWorldTimer()->
                        GetSnapshotTime() / WORLD_TIMER_SNAPSHOT_TICKS_PER_MILLISECOND),
                    i, g_pAppManager->GetClientExoApp()->GetCaptureEventFile());
            }
            CExoInputEventDesc* pDesc = aEvents[i].m_pDesc;
            int nEvent = pDesc->m_nEvent;
            int nValue = aEvents[i].m_nValue;
            CGuiInGame* pGuiInGame = g_pAppManager->GetClientExoApp()->GetInGameGui();
            int bGuiEvent = 0;
            int bGameEvent = 0;
            if (nEvent >= 39 && nEvent <= 64)
                bGuiEvent = 1;
            else
            {
                if (g_pAppManager->GetClientExoApp()->GetInGameGui()->m_bShowPauseGui != CGuiInGame::GUISTATE_MAIN_INTERFACE &&
                    ((nEvent >= 180 && nEvent <= 187) || nEvent == 206 ||
                     nEvent == 223 || nEvent == 243 || nEvent == 244 ||
                     (nEvent >= 254 && nEvent <= 262)))
                    bGuiEvent = 1;
            }
            if (bGuiEvent && nValue)
                bGameEvent = 1;
            if (bGameEvent &&
                GetClientOptions()->m_nCameraMode == CClientOptions::CAMERA_MODE_WORLD_CLICK_DISABLED &&
                !g_pAppManager->GetClientExoApp()->GetInGameGui()->m_bControllerPauseActive)
            {
                g_bShowIndicator = 1;
                GetModule()->RestoreCamera();
                SetInputClass(INPUT_CLASS_GAME, 1);
            }
            else if (bGuiEvent)
            {
                if (m_pGuiInGame->m_bShowPauseGui == CGuiInGame::GUISTATE_MAIN_INTERFACE ||
                    m_nInputClass == INPUT_CLASS_GUI ||
                    m_nInputClass == INPUT_CLASS_DIALOG)
                    m_pSWGuiManager->HandleInputEvent(nEvent, nValue);
            }
            else
                bInputHandled = HandleInputEvent(nEvent, nValue);

            if ((!bMouseCameraRotate || m_nInputClass == INPUT_CLASS_GUI) &&
                m_nInputClass != INPUT_CLASS_FREE_LOOK)
            {
                bInputHandled = 0;
#ifndef _XBOX
                m_pSWGuiManager->HandleMouseMove(m_nMouseX, m_nMouseY);
#endif
            }
#ifndef _XBOX
            else if (bInputHandled && g_pGuiMan != NULL)
                g_pGuiMan->ResetToolTipTimer();
#endif
        }

        if (GetModule() != NULL && GetModule()->m_pModuleCamera != NULL &&
            !bMouseCameraRotate)
        {
            char nTargetControl = 0;
#ifndef _XBOX
            if (m_pSWGuiManager->m_pMouseOverControl != NULL)
                nTargetControl = m_pGuiInGame->m_pMainInterface->
                    IsTargetActionMenuControl(m_pSWGuiManager->m_pMouseOverControl);
#endif
            if (nTargetControl)
            {
                if (nTargetControl == 1)
                {
                    Vector vPosition;
                    unsigned long oidTarget = m_nLastTarget;
                    HandleMouseOver(oidTarget, vPosition, 1);
                }
                else
                {
                    Vector vPosition;
                    HandleMouseOver(OBJECT_INVALID, vPosition, 0);
                }
                int nLastTarget = m_nLastTarget;
                CSWGuiMainInterface* pMainInterface = m_pGuiInGame->m_pMainInterface;
                pMainInterface->m_oidStickyTarget = nLastTarget;
            }
            else
            {
                CHitInfo hitInfo(0, CHitInfo::HIT_ALL_NON_TRIGGER, 0);
#ifndef _XBOX
                hitInfo.m_bResult4 = 1;
#endif
                Vector vHitPosition;
#ifndef _XBOX
                int nMouseX = g_pGuiMan->m_nMouseX;
                int nScreenMouseY = g_pGuiMan->m_nMouseY;
#endif
#ifndef _XBOX
                int nMouseY = g_pGuiMan->m_nHeight - nScreenMouseY;
#endif
                EmptyHitcheckIgnoreList();
                unsigned long oidPlayer = m_nPlayerCreatureId;
                AddObjectToHitcheckIgnoreList(oidPlayer);
                CSWCObject* pHitObject = NULL;
                int bRetry;
                do
                {
                    bRetry = 0;
                    SetHitcheckIgnoreList(&hitInfo.m_lstHits);
#ifndef _XBOX
                    GetModule()->GetCamera()->HitCheck(
                        nMouseX, nMouseY, &vHitPosition, &hitInfo);
#endif
                    if (hitInfo.m_pOutputObject != NULL && hitInfo.m_pOutputObject->GetExternalData() != NULL)
                    {
                        switch (hitInfo.m_pOutputObject->GetExternalDataType())
                        {
                        case SW_OBJECT_TYPE_CREATURE:
                            pHitObject = ((CGameObject*)hitInfo.m_pOutputObject->GetExternalData())->AsSWCObject();
                            break;
                        case SW_OBJECT_TYPE_PLACEABLE:
                            pHitObject = ((CGameObject*)hitInfo.m_pOutputObject->GetExternalData())->AsSWCObject();
                            if (pHitObject != NULL && pHitObject->AsSWCPlaceable() != NULL)
                            {
                                CSWCPlaceable* pPlaceable = pHitObject->AsSWCPlaceable();
                                CSWSPlaceable* pServerPlaceable = pPlaceable->GetServerPlaceable();
                                if (pServerPlaceable == NULL)
                                    pHitObject = NULL;
                                else if (!pPlaceable->m_bUseable || pServerPlaceable->m_bStatic)
                                {
                                    unsigned char nAppearance = pPlaceable->m_nAppearance;
                                    int bIgnore = 0;
                                    g_pRules->Get2DArrays()->m_pPlaceables->GetINTEntry(
                                        nAppearance,
                                        CTwoDimArrays::COL2DA_PLACEABLES_IGNORESTATICHITCHECK,
                                        &bIgnore);
                                    if (bIgnore)
                                    {
                                        bRetry = 1;
                                        AddObjectToHitcheckIgnoreList(pPlaceable->GetId());
                                    }
                                    pHitObject = NULL;
                                }
                            }
                            break;
                        case SW_OBJECT_TYPE_DOOR:
                            pHitObject = ((CGameObject*)hitInfo.m_pOutputObject->GetExternalData())->AsSWCObject();
                            if (pHitObject != NULL && pHitObject->AsSWCDoor() != NULL &&
                                (pHitObject->AsSWCDoor()->m_bStatic ||
                                 pHitObject->AsSWCDoor()->GetIsOpen()))
                                pHitObject = NULL;
                            break;
                        case SW_OBJECT_TYPE_ITEM:
                            pHitObject = ((CSWCItem*)hitInfo.m_pOutputObject->GetExternalData())->AsSWCObject();
                            break;
                        case SW_OBJECT_TYPE_TRIGGER:
                            pHitObject = ((CGameObject*)hitInfo.m_pOutputObject->GetExternalData())->AsSWCObject();
                            if (pHitObject != NULL && pHitObject->AsSWCTrigger() != NULL &&
                                pHitObject->AsSWCTrigger()->m_bTrap &&
                                pHitObject->AsSWCTrigger()->GetCurrentAnimation() != 10144)
                                pHitObject = NULL;
                            break;
                        }
                    }
                } while (bRetry);

                if (pHitObject != NULL && pHitObject->AsSWCCreature() != NULL)
                {
                    if (pHitObject->AsSWCCreature()->m_oidParentCorpse != OBJECT_INVALID)
                    {
                        unsigned long oidParent = pHitObject->AsSWCCreature()->m_oidParentCorpse;
                        CGameObject* pParent = g_pAppManager->GetClientExoApp()->GetGameObject(oidParent);
                        if (pParent != NULL && pParent->AsSWCObject() != NULL)
                            pHitObject = pParent->AsSWCObject();
                    }
                    else if (pHitObject->AsSWCCreature() != NULL &&
                             (pHitObject->AsSWCCreature()->GetServerCreature() == NULL ||
                              pHitObject->AsSWCCreature()->GetServerCreature()->GetDead() ||
                              pHitObject->AsSWCCreature()->GetServerCreature()->GetDeadTemp()))
                    {
                        int bCorpse = GetModule()->GetArea()->GetIsObjectCorpse(pHitObject->AsSWCCreature()->GetId());
                        if (bCorpse)
                            pHitObject = NULL;
                    }
                }
                if (pHitObject != NULL && pHitObject->AsSWCItem() != NULL)
                {
                    CSWCCreature* pEquippedBy = pHitObject->AsSWCItem()->GetEquippedBy();
                    if (pEquippedBy != NULL)
                        pHitObject = pEquippedBy->AsSWCObject();
                }
                if (pHitObject != NULL && GetPlayerCreature() != NULL)
                {
                    Vector vCamera = GetModule()->GetCamera()->GetPosition();
                    Vector vPlayer;
                    if (GetPlayerCreature() != NULL)
                        vPlayer = GetPlayerCreature()->m_vPosition;
                    else if (m_pSWParty != NULL && m_pSWParty->GetCharacter(0) != NULL)
                        vPlayer = m_pSWParty->GetCharacter(0)->m_vPosition;
                    Vector vObject = pHitObject->AsSWCObject()->m_vPosition;
                    Vector vToPlayer;
                    Vector vToObject;
                    Vector vDelta;
                    vToPlayer = vPlayer - vCamera;
                    vToObject = vObject - vCamera;
                    vDelta = vObject - vPlayer;
                    float fRange = pHitObject->IsHostile() ? HOSTILE_TARGET_RANGE : TARGET_RANGE;
                    int bTooFar = vDelta.m_fX * vDelta.m_fX + vDelta.m_fY * vDelta.m_fY > fRange * fRange;
                    if (vToObject.m_fX * vToPlayer.m_fX + vToObject.m_fY * vToPlayer.m_fY >= 0.0f &&
                        !bTooFar)
                    {
#ifndef _XBOX
                        if (m_pSWGuiManager->m_pMouseOverControl != NULL)
#else
                        if (0)
#endif
                        {
                            CSWGuiMainInterface* pMainInterface = m_pGuiInGame->m_pMainInterface;
                            pMainInterface->m_oidStickyTarget = OBJECT_INVALID;
                        }
                        else
                            m_pGuiInGame->GetMainInterface()->m_oidStickyTarget = pHitObject->GetId();
                    }
                    else
                        m_pGuiInGame->GetMainInterface()->m_oidStickyTarget = OBJECT_INVALID;
                    HandleMouseOver(pHitObject->GetId(), vHitPosition, 0);
                }
                else if (GetModule()->GetMiniGame() || GetPlayerCreature() == NULL)
                {
                    HandleMouseOver(OBJECT_INVALID, vHitPosition, 0);
                    CSWGuiMainInterface* pMainInterface = m_pGuiInGame->m_pMainInterface;
                    pMainInterface->m_oidStickyTarget = OBJECT_INVALID;
                }
                else
                {
                    Vector vScreen;
#ifndef _XBOX
                    int nY = g_pGuiMan->m_nMouseY;
                    int nX = g_pGuiMan->m_nMouseX;
#endif
#ifndef _XBOX
                    Vector vMouse((float)nX, (float)nY, 0.0f);
#endif
                    CSWCObject* pNearest = NULL;
                    float fNearest = FLT_MAX;
                    CSWCCreature* pPlayer = GetPlayerCreature();
                    for (int j = 0; j < m_aNearestObjects.num; j++)
                    {
                        CGameObject* pObject = GetGameObject(m_aNearestObjects[j].m_oidObject);
                        if (pObject != NULL && pObject->AsSWCObject() != NULL)
                        {
                            if (m_aNearestObjects[j].m_nCanSee == CNearestStruct::CAN_SEE_UNKNOWN)
                                m_aNearestObjects[j].m_nCanSee = pPlayer->CanSee(pObject->AsSWCObject());
                            if (m_aNearestObjects[j].m_nCanSee)
                            {
                                pObject->AsSWCObject()->GetReticlePoint(vScreen);
#ifndef _XBOX
                                Vector vOffset = vScreen - vMouse;
#endif
#ifndef _XBOX
                                float fDistance = vOffset * vOffset;
#endif
                                Vector vCamera = GetModule()->GetCamera()->GetPosition();
                                Vector vPlayer = pPlayer->m_vPosition;
                                Vector vObject = pObject->AsSWCObject()->m_vPosition;
                                Vector vToPlayer;
                                Vector vToObject;
                                Vector vDelta;
                                vToPlayer = vPlayer - vCamera;
                                vToObject = vObject - vCamera;
                                vDelta = vObject - vPlayer;
                                float fRange = pObject->AsSWCObject()->IsHostile() ? HOSTILE_TARGET_RANGE : TARGET_RANGE;
                                {
                                    int bTooFar = vDelta.m_fX * vDelta.m_fX + vDelta.m_fY * vDelta.m_fY > fRange * fRange;
                                    EXOLOG("ProcessInput: nearest object too far %p", &bTooFar);
                                    int bInFront = vToObject.m_fX * vToPlayer.m_fX +
                                                   vToObject.m_fY * vToPlayer.m_fY >= 0.0f;
#ifndef _XBOX
                                    if (pObject->AsSWCObject()->GetStickyMouseThreshhold() >= fDistance &&
                                        fDistance < fNearest && bInFront && !bTooFar)
                                    {
                                        pNearest = pObject->AsSWCObject();
                                        fNearest = fDistance;
                                    }
#endif
                                }
                            }
                        }
                    }
                    if (pNearest != NULL && pNearest->GetId() == m_nLastTarget)
                    {
                        if (g_pAppManager->GetClientExoApp()->GetInGameGui()->GetMainInterface()->m_aPersonalActionLists[0].num <= 0 ||
                            g_pAppManager->GetClientExoApp()->GetInGameGui()->GetMainInterface()->m_aPersonalActionLists[0].element == NULL ||
#ifndef _XBOX
                            m_pSWGuiManager->m_pMouseOverControl != NULL)
#else
                            0)
#endif
                        {
                            CSWGuiMainInterface* pMainInterface = m_pGuiInGame->m_pMainInterface;
                            pMainInterface->m_oidStickyTarget = OBJECT_INVALID;
                        }
                        else
                            m_pGuiInGame->GetMainInterface()->m_oidStickyTarget = pNearest->GetId();
                    }
#ifndef _XBOX
                    else if (m_pSWGuiManager->m_pMouseOverControl != NULL)
#else
                    else if (0)
#endif
                        m_pGuiInGame->GetMainInterface()->m_oidStickyTarget = OBJECT_INVALID;
                    else
                        m_pGuiInGame->GetMainInterface()->m_oidStickyTarget =
                            pNearest != NULL ? pNearest->GetId() : OBJECT_INVALID;
                    HandleMouseOver(pNearest != NULL ? pNearest->GetId() : OBJECT_INVALID,
                                    vHitPosition, 0);
                }
            }
        }
        else if (m_pGuiInGame != NULL && m_pGuiInGame->m_pMainInterface != NULL)
        {
            Vector vPosition;
            HandleMouseOver(OBJECT_INVALID, vPosition, 0);
            CSWGuiMainInterface* pMainInterface = m_pGuiInGame->m_pMainInterface;
            pMainInterface->m_oidStickyTarget = OBJECT_INVALID;
        }
    }

    if (bMouseCameraRotate)
    {
#ifndef _XBOX
        CSWGuiControl* pControl = m_pSWGuiManager->m_pMouseOverControl;
#endif
#ifndef _XBOX
        if (pControl != NULL)
        {
            CSWGuiControl* pParent = pControl->GetSelectableParent();
            pControl->HandleMouseOver(0);
            pControl->HandleFocusChange(0);
            if (pParent != NULL)
            {
                pParent->HandleMouseOver(0);
                pParent->HandleFocusChange(0);
            }
            CSWGuiManager* pSWGuiManager = m_pSWGuiManager;
#ifndef _XBOX
            pSWGuiManager->m_pMouseOverControl = NULL;
#endif
        }
#endif
    }

    CExoString sCapture;
    g_pAppManager->GetClientExoApp()->GetPlayerCreature();
    if (g_pAppManager->GetClientExoApp()->GetCaptureEventFile() != NULL)
    {
        CSWCCreature* pPlayer = g_pAppManager->GetClientExoApp()->GetPlayerCreature();
        if (pPlayer != NULL)
        {
            Vector vPosition = pPlayer->m_vPosition;
            sCapture.Format("%d %c %f %f %f\n", (unsigned long)
                (g_pAppManager->GetClientExoApp()->GetWorldTimer()->GetSnapshotTime() /
                 WORLD_TIMER_SNAPSHOT_TICKS_PER_MILLISECOND),
                CPlayBackEventNode::PLAYER_POSITION, vPosition.m_fX, vPosition.m_fY, vPosition.m_fZ);
            g_pAppManager->GetClientExoApp()->GetCaptureEventFile()->Write(sCapture);
        }
    }

    CSWCCreature* pCharacter = g_pAppManager->GetClientExoApp()->GetSWParty()->GetCharacter(0);
    if (!GetControlledCharacterDead() && m_pPlayerControl != NULL)
    {
        float fUpDown, fLeftRight;
        int bEnabled = m_pPlayerControl->m_bEnabled;
        if (bEnabled && m_nInputClass == INPUT_CLASS_GAME &&
            !m_nAreaNotReadyToDisplay &&
            g_pAppManager->GetClientExoApp()->GetClientOptions()->m_nCameraMode != CClientOptions::CAMERA_MODE_NAVIGATE)
        {
            fUpDown = g_pExoInput->PollInput(7, INPUT_CLASS_GAME);
            fLeftRight = g_pExoInput->PollInput(8, INPUT_CLASS_GAME);
            g_pExoInput->PollInput(13, INPUT_CLASS_GAME);
            if (fUpDown == 0.0f && fLeftRight == 0.0f)
            {
                fLeftRight = min(1.0f, max(-1.0f, g_pExoInput->PollInput(281, 0)));
                fUpDown = min(1.0f, max(-1.0f, g_pExoInput->PollInput(280, 0)));
            }
            if (g_pAppManager->GetClientExoApp()->GetInGameGui()->GetGlobalFadeActive())
            {
                fUpDown = 0.0f;
                fLeftRight = 0.0f;
            }
            int bRotate = g_bMouseCameraRotateToggle;
            if (g_pAppManager->GetClientExoApp()->GetClientOptions()->m_bMouseLook)
                bRotate = !bRotate;
            float fMouseX, fMouseY;
            g_pExoInput->GetMouseDelta(&fMouseX, &fMouseY);
            if (bRotate && !g_bLeftMouseDown &&
                (fMouseY > MOUSE_LOOK_THRESHOLD || fMouseY < -MOUSE_LOOK_THRESHOLD || CameraLookUp > 0.0f) &&
                (fMouseX < MOUSE_LOOK_THRESHOLD || fMouseX > -MOUSE_LOOK_THRESHOLD))
            {
                CSWCameraOnAStick* pCamera = (CSWCameraOnAStick*)
                    g_pAppManager->GetClientExoApp()->GetModule()->
                        GetCamera()->GetBehavior(AUR_BEHAVIOR_CAMERA_ONASTICK);
                if (pCamera != NULL)
                {
                    C2DA* pCameraStyle = g_pRules->Get2DArrays()->m_pCameraStyle;
                    pCameraStyle->GetFLOATEntry(0, CExoString("FL_LOOKUP"), &CameraLookUp);
                    pCameraStyle->GetFLOATEntry(0, CExoString("FL_LOOKDOWN"), &CameraLookDown);
                    pCamera->m_fDefaultTiltSpeed = pCamera->m_fTiltSpeed;
                    pCamera->m_fTiltSpeed = FREE_LOOK_TILT_SPEED;
                }
            }
            else
            {
                float fTiltUp, fTiltDown, fTiltSpeed;
                C2DA* pCameraStyle = g_pRules->Get2DArrays()->m_pCameraStyle;
                pCameraStyle->GetFLOATEntry(0, CExoString("TILTUP"), &fTiltUp);
                pCameraStyle->GetFLOATEntry(0, CExoString("TILTDOWN"), &fTiltDown);
                pCameraStyle->GetFLOATEntry(0, CExoString("TILTSPEED"), &fTiltSpeed);
                CSWCameraOnAStick* pCamera = (CSWCameraOnAStick*)
                    g_pAppManager->GetClientExoApp()->GetModule()->
                        GetCamera()->GetBehavior(AUR_BEHAVIOR_CAMERA_ONASTICK);
                if (CameraLookUp > fTiltUp)
                {
                    CameraLookUp -= FREE_LOOK_RETURN_STEP;
                    if (CameraLookUp < fTiltUp)
                        CameraLookUp = fTiltUp;
                }
                else if (pCamera != NULL)
                    pCamera->m_fTiltSpeed = pCamera->m_fDefaultTiltSpeed;
                if (CameraLookDown > fTiltDown)
                {
                    CameraLookDown -= FREE_LOOK_RETURN_STEP;
                    if (CameraLookDown < fTiltDown)
                        CameraLookDown = fTiltDown;
                }
                else if (pCamera != NULL)
                    pCamera->m_fTiltSpeed = fTiltSpeed;
            }
            if (bRotate && g_bLeftMouseDown && fUpDown <= 0.0f)
            {
                CSWCameraOnAStick* pCamera = (CSWCameraOnAStick*)
                    g_pAppManager->GetClientExoApp()->GetModule()->
                        GetCamera()->GetBehavior(AUR_BEHAVIOR_CAMERA_ONASTICK);
                if (CameraLookUp > 0.0f)
                {
                    CameraLookUp -= FREE_LOOK_RETURN_STEP;
                    if (CameraLookUp < 0.0f)
                        CameraLookUp = 0.0f;
                }
                else if (pCamera != NULL)
                    pCamera->m_fTiltSpeed = pCamera->m_fDefaultTiltSpeed;
                if (CameraLookDown > 0.0f)
                {
                    CameraLookDown -= FREE_LOOK_RETURN_STEP;
                    if (CameraLookDown < 0.0f)
                        CameraLookDown = 0.0f;
                }
                else if (pCamera != NULL)
                    pCamera->m_fTiltSpeed = pCamera->m_fDefaultTiltSpeed;
                fUpDown = -1.0f;
            }
#ifndef _XBOX
            if (g_pExoInput->PollInput(268, 0) != 0.0f)
                m_pPlayerControl->SetPlayerWalking(1);
            else
                m_pPlayerControl->SetPlayerWalking(0);
#endif
            if (pCharacter != NULL)
            {
                if (pCharacter->IsDriveAnimationBlocked() == 1)
                {
                    if (pCharacter->m_bCombatMode && !GetClientOptions()->m_bCombatMovement)
                    {
                        fUpDown = 0.0f;
                        fLeftRight = 0.0f;
                        m_fDriveActionDelay = -1.0f;
                    }
                    else if (fUpDown == 0.0f && fLeftRight == 0.0f)
                        m_fDriveActionDelay = -1.0f;
                    else if (m_fDriveActionDelay <= 0.0f)
                        m_fDriveActionDelay = 0.3f;
                    if (m_fDriveActionDelay > 0.0f)
                    {
                        m_fDriveActionDelay -= fTimeStep;
                        CSWSCreature* pServer = pCharacter->GetServerCreature();
                        int bCommandable = 1;
                        if (pServer != NULL)
                            bCommandable = pServer->m_bCommandable;
                        if (m_fDriveActionDelay <= 0.0f)
                        {
                            if (bCommandable)
                            {
                                if (m_pPlayerControl->GetCurSpeed() > 0.25f)
                                {
                                    pCharacter->ClearAllActions();
                                    pCharacter->AbortFireForgetAnimation(0);
                                    pCharacter->SetLockOrientationToObject(OBJECT_INVALID, 0);
                                    pCharacter->SetEngagedState(0);
                                    CSWCameraOnAStick* pCamera = (CSWCameraOnAStick*)
                                        g_pAppManager->GetClientExoApp()->GetModule()->
                                            GetCamera()->GetBehavior(AUR_BEHAVIOR_CAMERA_ONASTICK);
                                    if (pCamera != NULL)
                                        pCamera->ClearCombatCamera();
                                    pCharacter->SetWayPoints(0, NULL);
                                }
                                else
                                    m_fDriveActionDelay = 0.1f;
                            }
                        }
                        else
                        {
                            fUpDown = 0.0f;
                            fLeftRight = 0.0f;
                        }
                    }
                }
                int nIndex = m_pSWParty->GetIndex(pCharacter->GetId());
                if (nIndex >= 0)
                {
                    CSWPartyMemberData* pMember = &m_pSWParty->m_aPartyMemberData[nIndex];
                    if (pMember != NULL && pCharacter->m_bCombatMode)
                    {
                        if (m_pPlayerControl->GetCurSpeed() < 0.25f)
                        {
                            if (pMember->m_nCombatMessage == CSWPartyMemberData::COMBAT_MESSAGE_MOVING_CANCELS_COMBAT)
                                pMember->SetCombatMessage(CSWPartyMemberData::COMBAT_MESSAGE_ENGAGED);
                        }
                        else
                            pMember->SetCombatMessage(CSWPartyMemberData::COMBAT_MESSAGE_MOVING_CANCELS_COMBAT);
                    }
                }
            }
            if (!m_nCombatMode && m_fEnemyAutoPauseTimer <= 0.0f &&
                m_nLastTarget != OBJECT_INVALID && m_pPlayerControl->GetCurSpeed() >= 0.25f)
            {
                m_fMovingTargetTime += fTimeStep;
                if (m_fMovingTargetTime >= 0.5f)
                {
                    m_nLastTarget = OBJECT_INVALID;
                    m_fMovingTargetTime = 0.0f;
                }
            }
            else
                m_fMovingTargetTime = 0.0f;
            m_pPlayerControl->UpDown(fUpDown);
            m_pPlayerControl->LeftRight(fLeftRight);
        }
        else
        {
            if (g_pAppManager->GetClientExoApp()->GetClientOptions()->GetCameraMode() == CClientOptions::CAMERA_MODE_NAVIGATE)
            {
                fUpDown = g_pExoInput->PollInput(7, INPUT_CLASS_GAME);
                fLeftRight = g_pExoInput->PollInput(8, INPUT_CLASS_GAME);
                if (fUpDown == 0.0f && fLeftRight == 0.0f)
                {
                    fLeftRight = g_pExoInput->PollInput(281, INPUT_CLASS_GAME);
                    fUpDown = g_pExoInput->PollInput(280, INPUT_CLASS_GAME);
                }
                if (g_pAppManager->GetClientExoApp()->GetInGameGui()->GetGlobalFadeActive())
                {
                    fUpDown = 0.0f;
                    fLeftRight = 0.0f;
                }
                float fTilt = g_pExoInput->PollInput(12, INPUT_CLASS_GAME);
                float fTurn = g_pExoInput->PollInput(13, INPUT_CLASS_GAME);
                CSWCameraNavigate* pCamera = (CSWCameraNavigate*)
                    g_pAppManager->GetClientExoApp()->GetModule()->GetCamera()->GetBehavior(-1);
                if (pCamera != NULL && pCamera->GetType() == AUR_BEHAVIOR_CAMERA_NAVIGATE)
                {
                    pCamera->m_fSpeed = fUpDown * 10.0f;
                    pCamera->m_fStrafe = fLeftRight * 10.0f;
                    pCamera->m_fTilt = fTilt * -30.0f;
                    pCamera->m_fTurn = fTurn * -60.0f;
                    pCamera->m_fRoll = fTurn * fUpDown * -90.0f;
                }
            }
            if (g_pAppManager->GetClientExoApp()->GetClientOptions()->m_nCameraMode == CClientOptions::CAMERA_MODE_WORLD_CLICK_DISABLED)
            {
                fUpDown = g_pExoInput->PollInput(7, INPUT_CLASS_GAME);
                fLeftRight = g_pExoInput->PollInput(8, INPUT_CLASS_GAME);
                if (fUpDown == 0.0f && fLeftRight == 0.0f)
                {
                    fLeftRight = g_pExoInput->PollInput(281, INPUT_CLASS_FREE_LOOK);
                    fUpDown = g_pExoInput->PollInput(280, INPUT_CLASS_FREE_LOOK);
                }
                if (fUpDown != 0.0f || fLeftRight != 0.0f)
                    g_pAppManager->GetClientExoApp()->GetModule()->AcclTiltCamera(fUpDown);
            }
            m_pPlayerControl->UpDown(0.0f);
            m_pPlayerControl->LeftRight(0.0f);
        }
    }
    if (GetPlayerCreature() != NULL)
    {
        m_pPlayerControl->Control(fTimeStep);
        if (m_pModule != NULL && m_pModule->m_pInfo != NULL &&
            m_pModule->m_pInfo->m_pMiniGame != NULL)
        {
            if (m_pModule->m_pInfo->m_pMiniGame == (CSWMiniGame*)m_pModule->m_pInfo)
                m_pModule->m_pInfo->m_pMiniGame->Control(fTimeStep);
            else
                m_pModule->m_pInfo->m_pMiniGame->Control(fTimeStep);
        }
    }
    else if (m_pModule != NULL && m_pModule->m_pInfo != NULL &&
             m_pModule->m_pInfo->m_pMiniGame != NULL)
    {
        CSWMiniGame* pMiniGame = m_pModule->m_pInfo->m_pMiniGame;
        switch (pMiniGame->m_nAttachType)
        {
        case CSWMiniGame::ATTACH_PLAYER_MODEL:
            {
                float fX = g_pExoInput->PollInput(23, INPUT_CLASS_MINIGAME) / g_fMouseSensitivity;
                fX = min(1.0f, max(-1.0f, fX));
                if (fX == 0.0)
                    fX = min(1.0f, max(-1.0f,
                        g_pExoInput->PollInput(283, INPUT_CLASS_MINIGAME) + g_pExoInput->PollInput(286, INPUT_CLASS_MINIGAME)));
                Vector vAxis(fX, 0.0f, 0.0f);
                pMiniGame->UpdateAxis(vAxis, fTimeStep);
                pMiniGame->UpdateMouse(fX, 0.0f, fTimeStep);
#ifndef _XBOX
                if (!GetPausedByCombat() && g_pAppManager->GetClientExoApp()->GetInputClass())
                    g_pExoInput->SetMousePos(m_pSWGuiManager->m_nWidth / 2,
                                            m_pSWGuiManager->m_nHeight / 2, true);
#endif
            }
            break;
        case CSWMiniGame::ATTACH_CAMERA_HOOK:
            {
                float fX = min(1.0f, max(-1.0f,
                    g_pExoInput->PollInput(283, INPUT_CLASS_MINIGAME) + g_pExoInput->PollInput(286, INPUT_CLASS_MINIGAME)));
                float fY = min(1.0f, max(-1.0f,
                    g_pExoInput->PollInput(282, INPUT_CLASS_MINIGAME) + g_pExoInput->PollInput(285, INPUT_CLASS_MINIGAME)));
                Vector vAxis(fX, 0.0f,
                    g_pAppManager->GetClientExoApp()->GetClientOptions() != NULL &&
                    g_pAppManager->GetClientExoApp()->GetClientOptions()->m_bReverseMinigameYAxis ? -fY : fY);
                pMiniGame->UpdateAxis(vAxis, fTimeStep);
                fX = g_pExoInput->PollInput(23, INPUT_CLASS_MINIGAME) / g_fMouseSensitivity;
                fY = g_pExoInput->PollInput(24, INPUT_CLASS_MINIGAME) * (-1.0f / g_fMouseSensitivity);
                fX = min(1.0f, max(-1.0f, fX));
                fY = min(1.0f, max(-1.0f, fY));
                if (g_pAppManager->GetClientExoApp()->GetClientOptions() != NULL &&
                    g_pAppManager->GetClientExoApp()->GetClientOptions()->m_bReverseMinigameYAxis)
                    fY = -fY;
                pMiniGame->UpdateMouse(fX, fY, fTimeStep);
#ifndef _XBOX
                if (!GetPausedByCombat() && g_pAppManager->GetClientExoApp()->GetInputClass())
                    g_pExoInput->SetMousePos(m_pSWGuiManager->m_nWidth / 2,
                                            m_pSWGuiManager->m_nHeight / 2, true);
#endif
            }
            break;
        }
        pMiniGame->Control(fTimeStep);
    }
}
#endif
static inline void ClientInputTrace()
{
}

#ifdef _XBOX

void CClientExoAppInternal::HandleInputEvent(int nEvent, int nValue)
{
    switch (nEvent)
    {
    case INPUT_EVENT_NEXT_CHARACTER:
        if (nValue != 0)
        {
            if (m_pClientOptions->m_nCameraMode == CClientOptions::CAMERA_MODE_WORLD_CLICK_DISABLED)
            {
                g_bShowIndicator = 1;
                GetModule()->RestoreCamera();
                SetInputClass(INPUT_CLASS_GAME, 1);
            }
            else
            {
                ChangeCharacterToNextLivingPartyMember(0, 1);
                g_pAppManager->GetClientExoApp()->GetInGameGui()->ChangeCharacter();
            }
            g_pAppManager->GetClientExoApp()->GetSWGuiManager()->PlayGuiSound(CSWGuiManager::GUISOUND_WORLD_ACTION);
        }
        break;
    case INPUT_EVENT_SOLO_MODE:
        if (nValue != 0)
        {
            if (m_pClientOptions->m_nCameraMode == CClientOptions::CAMERA_MODE_WORLD_CLICK_DISABLED)
            {
                g_bShowIndicator = 1;
                GetModule()->RestoreCamera();
                SetInputClass(INPUT_CLASS_GAME, 1);
            }
            else if (m_pGuiInGame != NULL && m_pGuiInGame->m_pPartySelection == 0)
                m_pGuiInGame->ShowSoloModeQuery(0);
        }
        break;
    case INPUT_EVENT_EXIT:
        if (nValue != 0)
        {
            if (m_pClientOptions->m_nCameraMode == CClientOptions::CAMERA_MODE_WORLD_CLICK_DISABLED)
            {
                g_bShowIndicator = 1;
                GetModule()->RestoreCamera();
                SetInputClass(INPUT_CLASS_GAME, 1);
            }
            if (!GetControlledCharacterDead())
            {
                if (m_pGuiInGame->m_nGlobalDialogState == 0 &&
                    g_pAppManager->GetClientExoApp()->GetSWParty() != NULL &&
                    g_pAppManager->GetClientExoApp()->GetSWParty()->GetPlayerCharacter() != NULL)
                {
                    if (m_pGuiInGame->ShowSWInGameGui())
                    {
                        if (m_nInputClass != INPUT_CLASS_GUI)
                        {
                            m_nInputClass = INPUT_CLASS_GUI;
                            g_pExoInput->ClearEvents();
                        }
                        g_pAppManager->GetClientExoApp()->ShowTutorialWindow(TUTORIAL_OPTIONS, OBJECT_INVALID, OBJECT_INVALID, 0);
                    }
                }
            }
            else
            {
                if (m_nGameOver)
                    HurryDeathSequence();
                else
                {
                    CSWGuiMessageBox* pMessageBox = m_pGuiInGame->m_pMessageBox;
                    pMessageBox->SetAllowCancel(0);
                    pMessageBox->SetCallback(NULL, NULL, NULL);
                    pMessageBox->SetMessage(STRREF_PLAYER_DEAD);
                    m_pSWGuiManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
                }
            }
        }
        break;
    case INPUT_EVENT_PAUSE:
    case 37:
    case 38:
        if (nValue != 0)
        {
            if (m_pClientOptions->m_nCameraMode == CClientOptions::CAMERA_MODE_WORLD_CLICK_DISABLED)
            {
                g_bShowIndicator = 1;
                GetModule()->RestoreCamera();
                SetInputClass(INPUT_CLASS_GAME, 1);
            }
            int bPaused = GetPausedByCombat();
            SetPausedByCombat(!bPaused, PAUSE_REASON_INPUT, 0);
            if (g_pAppManager->GetClientExoApp()->GetAutoPaused())
                g_pAppManager->GetClientExoApp()->SetAutoPaused(0, 0);
            if (bPaused || !g_pAppManager->GetClientExoApp()->ShowTutorialWindow(TUTORIAL_PAUSE, OBJECT_INVALID, OBJECT_INVALID, 0))
                g_pAppManager->GetClientExoApp()->GetSWGuiManager()->PlayGuiSound(CSWGuiManager::GUISOUND_WORLD_ACTION);
        }
        break;
    case INPUT_EVENT_RESTORE_CAMERA:
        if (nValue != 0 && m_pClientOptions->m_nCameraMode == CClientOptions::CAMERA_MODE_WORLD_CLICK_DISABLED)
        {
            g_bShowIndicator = 1;
            GetModule()->RestoreCamera();
            SetInputClass(INPUT_CLASS_GAME, 1);
        }
        break;
    case INPUT_EVENT_FREE_LOOK:
        if (GetPlayerCreature()->m_nCombatState == 0 && nValue != 0 && m_nGameOver == 0)
        {
            CSWSCreature* pServerCreature;
            if (m_pClientOptions->m_nCameraMode != CClientOptions::CAMERA_MODE_WORLD_CLICK_DISABLED &&
                GetPlayerCreature()->GetServerCreature() != NULL &&
                (pServerCreature = GetPlayerCreature()->GetServerCreature())->m_lstActions.GetHeadPos() == NULL &&
                g_pAppManager->GetClientExoApp()->GetPausedByCombat() == 0)
            {
                g_bShowIndicator = 0;
                if (g_nHSMode != 1)
                {
                    GetModule()->SetFreeLookCamera(m_pSWParty->GetPlayerCharacter()->GetId());
                    if (m_nInputClass != INPUT_CLASS_FREE_LOOK)
                    {
                        m_nInputClass = INPUT_CLASS_FREE_LOOK;
                        g_pExoInput->ClearEvents();
                    }
                }
                else
                {
                    GetModule()->SetNavigateCamera();
                    ++g_nHSMode;
                }
            }
            else if (m_pClientOptions->m_nCameraMode == CClientOptions::CAMERA_MODE_WORLD_CLICK_DISABLED)
            {
                g_bShowIndicator = 1;
                GetModule()->RestoreCamera();
                SetInputClass(INPUT_CLASS_GAME, 1);
            }
        }
        break;
    case INPUT_EVENT_MOVE_FORWARD:
    case INPUT_EVENT_MOVE_BACK:
    case 71:
    case 72:
        if (nValue != 0 && m_nGameOver == 0 &&
            m_pClientOptions->m_nCameraMode == CClientOptions::CAMERA_MODE_WORLD_CLICK_DISABLED)
        {
            g_bShowIndicator = 1;
            GetModule()->RestoreCamera();
            SetInputClass(INPUT_CLASS_GAME, 1);
        }
        break;
    case INPUT_EVENT_PREVIOUS_TARGET:
        if (nValue != 0)
        {
            if (m_pClientOptions->m_nCameraMode == CClientOptions::CAMERA_MODE_WORLD_CLICK_DISABLED)
            {
                g_bShowIndicator = 1;
                GetModule()->RestoreCamera();
                SetInputClass(INPUT_CLASS_GAME, 1);
            }
            else
            {
                SelectNearestObject(1, m_nCombatMode == 0 ? 0 : 2);
                if (m_nCombatMode != 0 && m_pClientOptions->m_bAutopauseNewTargetSelected)
                    SetAutoPaused(1, AUTOPAUSE_NEW_TARGET);
            }
        }
        break;
    case INPUT_EVENT_NEXT_TARGET:
        if (nValue != 0)
        {
            if (m_pClientOptions->m_nCameraMode == CClientOptions::CAMERA_MODE_WORLD_CLICK_DISABLED)
            {
                g_bShowIndicator = 1;
                GetModule()->RestoreCamera();
                SetInputClass(INPUT_CLASS_GAME, 1);
            }
            else
            {
                SelectNearestObject(0, m_nCombatMode == 0 ? 0 : 2);
                if (m_nCombatMode != 0 && m_pClientOptions->m_bAutopauseNewTargetSelected)
                    SetAutoPaused(1, AUTOPAUSE_NEW_TARGET);
            }
        }
        break;
    case 21:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
        if (nValue != 0 && m_pModule != NULL && m_pModule->m_pInfo != NULL && !GetPausedByCombat())
        {
            CSWMiniGame* pMiniGame = m_pModule->m_pInfo->m_pMiniGame;
            if (pMiniGame != NULL && pMiniGame->GetPlayer() != NULL)
                pMiniGame->GetPlayer()->Fire();
        }
        break;
    }
}
#endif

#ifdef _XBOX

void CClientExoAppInternal::ProcessInput(float fTimeStep)
{
    int nEvents;
    if (m_bInputDisabled)
        return;

    if (g_pAppManager->GetClientExoApp()->GetCaptureEventPlayBack())
    {
        PlayBackInputEvents();
        return;
    }

    if (g_pExoInput->GetActive())
    {
        int i;
        CExoArrayList<CExoInputEvent> aEvents =
            g_pExoInput->GetEvents(nEvents, m_nInputClass);
        if (m_pGuiInGame->m_bShowPauseGui == CGuiInGame::GUISTATE_MAIN_INTERFACE ||
            m_nInputClass == INPUT_CLASS_GUI || m_nInputClass == INPUT_CLASS_DIALOG)
        {
            int bDirection = 0;
            for (int i = 0; i < nEvents; i++)
            {
                int nValue = aEvents[i].m_nValue;
                if (nValue)
                {
                    if (m_nInputHistoryCount < INPUT_HISTORY_LENGTH)
                    {
                        CExoInputEventDesc* pDesc = aEvents[i].m_pDesc;
                        m_anInputHistory[m_nInputHistoryCount] = pDesc->m_nEvent;
                        m_nInputHistoryCount++;
                    }
                    else
                    {
                        for (int j = 0; j < INPUT_HISTORY_LENGTH - 1; j++)
                            m_anInputHistory[j] = m_anInputHistory[j + 1];
                        CExoInputEventDesc* pDesc = aEvents[i].m_pDesc;
                        m_anInputHistory[INPUT_HISTORY_LENGTH - 1] = pDesc->m_nEvent;
                    }
                    CExoInputEventDesc* pDesc = aEvents[i].m_pDesc;
                    switch (pDesc->m_nEvent)
                    {
                    case 1: case 2: case 10: case 11:
                    case 39: case 40: case 41: case 42:
                    case 43: case 44: case 45: case 46:
                    case 53: case 54:
                        if (bDirection)
                        {
                            aEvents.DelIndex(i);
                            i--;
                            nEvents--;
                            break;
                        }
                        bDirection = 1;
                    default:
                        if (aEvents[i].m_pDesc->m_nEvent >= 49 &&
                            aEvents[i].m_pDesc->m_nEvent <= 50)
                        {
                            int nInsert = -1;
                            for (int j = i + 1; j < nEvents; j++)
                            {
                                int nValue = aEvents[j].m_nValue;
                                if (nValue)
                                {
                                    CExoInputEventDesc* pNextDesc = aEvents[j].m_pDesc;
                                    unsigned long nNext = pNextDesc->m_nEvent;
                                    if (nNext >= 47 && nNext <= 48)
                                        nInsert = j + 1;
                                }
                            }
                            if (nInsert != -1)
                            {
                                if (nInsert >= aEvents.num)
                                    aEvents.Add(aEvents[i]);
                                else
                                    aEvents.Insert(aEvents[i], nInsert);
                                aEvents.DelIndex(i);
                            }
                        }
                        break;
                    }
                }
            }
        }

        for (i = 0; i < nEvents; i++)
        {
            if (g_pAppManager->GetClientExoApp()->GetCaptureEvents() &&
                g_pAppManager->GetClientExoApp()->GetCaptureEventFile() != NULL)
            {
                g_pExoInput->WriteEventToFile((unsigned long)
                    (g_pAppManager->GetClientExoApp()->GetWorldTimer()->
                        GetSnapshotTime() / WORLD_TIMER_SNAPSHOT_TICKS_PER_MILLISECOND),
                    i, g_pAppManager->GetClientExoApp()->GetCaptureEventFile());
            }
            CExoInputEventDesc* pDesc = aEvents[i].m_pDesc;
            int nEvent = pDesc->m_nEvent;
            int nValue = aEvents[i].m_nValue;
            if (nEvent >= 39 && nEvent <= 64)
            {
                if (nValue &&
                    GetClientOptions()->m_nCameraMode == CClientOptions::CAMERA_MODE_WORLD_CLICK_DISABLED &&
                    !g_pAppManager->GetClientExoApp()->GetInGameGui()->m_bControllerPauseActive)
                {
                    g_bShowIndicator = 1;
                    GetModule()->RestoreCamera();
                    SetInputClass(INPUT_CLASS_GAME, 1);
                }
                else if (m_pGuiInGame->m_bShowPauseGui == CGuiInGame::GUISTATE_MAIN_INTERFACE ||
                         m_nInputClass == INPUT_CLASS_GUI ||
                         m_nInputClass == INPUT_CLASS_DIALOG)
                    m_pSWGuiManager->HandleInputEvent(nEvent, nValue);
            }
            else
                HandleInputEvent(nEvent, nValue);
        }
    }

    g_pAppManager->GetClientExoApp()->GetPlayerCreature();
    if (g_pAppManager->GetClientExoApp()->GetCaptureEventFile() != NULL)
    {
        CSWCCreature* pPlayer = g_pAppManager->GetClientExoApp()->GetPlayerCreature();
        if (pPlayer != NULL)
        {
            Vector vPosition = pPlayer->m_vPosition;
            CExoString sCapture;
            sCapture.Format("%d %c %f %f %f\n", (unsigned long)
                (g_pAppManager->GetClientExoApp()->GetWorldTimer()->GetSnapshotTime() /
                 WORLD_TIMER_SNAPSHOT_TICKS_PER_MILLISECOND),
                CPlayBackEventNode::PLAYER_POSITION, vPosition.m_fX, vPosition.m_fY, vPosition.m_fZ);
            g_pAppManager->GetClientExoApp()->GetCaptureEventFile()->Write(sCapture);
        }
    }

    CSWCCreature* pCharacter = g_pAppManager->GetClientExoApp()->GetSWParty()->GetCharacter(0);
    if (!GetControlledCharacterDead() && m_pPlayerControl != NULL)
    {
        float fUpDown, fLeftRight;
        if (m_pPlayerControl->m_bEnabled && m_nInputClass == INPUT_CLASS_GAME &&
            !m_nAreaNotReadyToDisplay &&
            g_pAppManager->GetClientExoApp()->GetClientOptions()->m_nCameraMode != CClientOptions::CAMERA_MODE_NAVIGATE)
        {
            fUpDown = g_pExoInput->PollInput(7, INPUT_CLASS_GAME);
            fLeftRight = g_pExoInput->PollInput(8, INPUT_CLASS_GAME);
            if (pCharacter != NULL)
            {
                if (pCharacter->IsDriveAnimationBlocked() == 1)
                {
                    if (pCharacter->m_bCombatMode && !GetClientOptions()->m_bCombatMovement)
                    {
                        fUpDown = 0.0f;
                        fLeftRight = 0.0f;
                        m_fDriveActionDelay = -1.0f;
                    }
                    else if (fUpDown == 0.0f && fLeftRight == 0.0f)
                        m_fDriveActionDelay = -1.0f;
                    else if (m_fDriveActionDelay <= 0.0f)
                        m_fDriveActionDelay = 0.3f;
                    if (m_fDriveActionDelay > 0.0f)
                    {
                        m_fDriveActionDelay -= fTimeStep;
                        CSWSCreature* pServer = pCharacter->GetServerCreature();
                        int bCommandable = 1;
                        if (pServer != NULL)
                            bCommandable = pServer->m_bCommandable;
                        if (m_fDriveActionDelay <= 0.0f)
                        {
                            if (bCommandable)
                            {
                                if (m_pPlayerControl->GetCurSpeed() > 0.25f)
                                {
                                    pCharacter->ClearAllActions();
                                    pCharacter->AbortFireForgetAnimation(0);
                                    pCharacter->SetLockOrientationToObject(OBJECT_INVALID, 0);
                                    pCharacter->SetEngagedState(0);
                                    CSWCameraOnAStick* pCamera = (CSWCameraOnAStick*)
                                        g_pAppManager->GetClientExoApp()->GetModule()->
                                            GetCamera()->GetBehavior(AUR_BEHAVIOR_CAMERA_ONASTICK);
                                    if (pCamera != NULL)
                                        pCamera->ClearCombatCamera();
                                    pCharacter->SetWayPoints(0, NULL);
                                }
                                else
                                    m_fDriveActionDelay = 0.1f;
                            }
                        }
                        else
                        {
                            fUpDown = 0.0f;
                            fLeftRight = 0.0f;
                        }
                    }
                }
                int nIndex = m_pSWParty->GetIndex(pCharacter->GetId());
                if (nIndex >= 0)
                {
                    CSWPartyMemberData* pMember = m_pSWParty->GetMemberData(nIndex);
                    if (pMember != NULL && pCharacter->m_bCombatMode)
                    {
                        if (m_pPlayerControl->GetCurSpeed() < 0.25f)
                        {
                            if (pMember->m_nCombatMessage == CSWPartyMemberData::COMBAT_MESSAGE_MOVING_CANCELS_COMBAT)
                                pMember->SetCombatMessage(42475);
                        }
                        else
                            pMember->SetCombatMessage(CSWPartyMemberData::COMBAT_MESSAGE_MOVING_CANCELS_COMBAT);
                    }
                }
            }
            if (!m_nCombatMode && m_fEnemyAutoPauseTimer <= 0.0f &&
                m_nLastTarget != OBJECT_INVALID && m_pPlayerControl->GetCurSpeed() >= 0.25f)
            {
                m_fMovingTargetTime = fTimeStep + m_fMovingTargetTime;
                if (m_fMovingTargetTime >= 0.5f)
                {
                    m_nLastTarget = OBJECT_INVALID;
                    m_fMovingTargetTime = 0.0f;
                }
            }
            else
                m_fMovingTargetTime = 0.0f;
            m_pPlayerControl->UpDown(fUpDown);
            m_pPlayerControl->LeftRight(fLeftRight);
        }
        else
        {
            if (g_pAppManager->GetClientExoApp()->GetClientOptions()->GetCameraMode() == CClientOptions::CAMERA_MODE_NAVIGATE)
            {
                fUpDown = g_pExoInput->PollInput(7, INPUT_CLASS_GAME);
                fLeftRight = g_pExoInput->PollInput(8, INPUT_CLASS_GAME);
                float fTilt = g_pExoInput->PollInput(12, INPUT_CLASS_GAME);
                float fTurn = g_pExoInput->PollInput(13, INPUT_CLASS_GAME);
                CSWCameraNavigate* pCamera = (CSWCameraNavigate*)
                    g_pAppManager->GetClientExoApp()->GetModule()->GetCamera()->GetBehavior(-1);
                if (pCamera != NULL && pCamera->GetType() == AUR_BEHAVIOR_CAMERA_NAVIGATE)
                {
                    pCamera->m_fSpeed = fUpDown * 10.0f;
                    pCamera->m_fStrafe = fLeftRight * 10.0f;
                    pCamera->m_fTilt = fTilt * -30.0f;
                    pCamera->m_fTurn = fTurn * -60.0f;
                    pCamera->m_fRoll = fTurn * fUpDown * -90.0f;
                }
            }
            m_pPlayerControl->UpDown(0.0f);
            m_pPlayerControl->LeftRight(0.0f);
        }
    }
    if (GetPlayerCreature() != NULL)
    {
        m_pPlayerControl->Control(fTimeStep);
        if (m_pModule != NULL && m_pModule->m_pInfo != NULL &&
            m_pModule->m_pInfo->m_pMiniGame != NULL)
        {
            m_pModule->m_pInfo->m_pMiniGame->Control(fTimeStep);
        }
    }
    else if (m_pModule != NULL && m_pModule->m_pInfo != NULL &&
             m_pModule->m_pInfo->m_pMiniGame != NULL)
    {
        CSWMiniGame* pMiniGame = m_pModule->m_pInfo->m_pMiniGame;
        float fStickY = g_pExoInput->PollInput(23, INPUT_CLASS_MINIGAME);
        float fPadY = g_pExoInput->PollInput(32, INPUT_CLASS_MINIGAME);
        float fDown = g_pExoInput->PollInput(33, INPUT_CLASS_MINIGAME);
        float fButtonY = g_pExoInput->PollInput(34, INPUT_CLASS_MINIGAME) - fDown;
        if (g_pAppManager->GetClientExoApp()->GetClientOptions()->m_bReverseMinigameYAxis)
        {
            fStickY *= -1.0f;
            fPadY *= -1.0f;
            fButtonY *= -1.0f;
        }
        float fY = min(1.0f, max(-1.0f, fButtonY + fPadY + fStickY));
        float fStickX = g_pExoInput->PollInput(22, INPUT_CLASS_MINIGAME);
        float fPadX = g_pExoInput->PollInput(31, INPUT_CLASS_MINIGAME);
        float fRight = g_pExoInput->PollInput(36, INPUT_CLASS_MINIGAME);
        float fButtonX = fRight - g_pExoInput->PollInput(35, INPUT_CLASS_MINIGAME);
        float fX = min(1.0f, max(-1.0f, fButtonX + fPadX + fStickX));
        pMiniGame->UpdateAxis(fX, fY, fTimeStep);
        pMiniGame->Control(fTimeStep);
    }
}

void CClientExoAppInternal::SetInputClass(int nClass)
{
    if (m_nInputClass != nClass)
    {
        int nOldClass = m_nInputClass;
        m_nInputClass = nClass;
        g_pExoInput->ClearEvents();

        switch (nClass)
        {
        case INPUT_CLASS_GAME:
            if (m_oidReattachedCreature != OBJECT_INVALID)
            {
                CSWSCreature* pCreature =
                    g_pAppManager->GetServerExoApp()->GetPlayerCreature();
                if (pCreature != NULL)
                {
                    SetPlayerCreature(g_pAppManager->GetClientExoApp()->
                        ServerToClientObjectId(pCreature->GetId()));
                    m_oidReattachedCreature = OBJECT_INVALID;
                    AttachPlayerGameObjectToArea();
                }
            }
            if (m_pModule != NULL && m_pModule->GetCamera() != NULL)
            {
                Vector vOrientation = GetPlayerCreature()->m_vOrientation;
                CSWCameraOnAStick* pCameraBehavior = (CSWCameraOnAStick*)
                    m_pModule->GetCamera()->GetBehavior(
                        AUR_BEHAVIOR_CAMERA_ONASTICK);
                if (pCameraBehavior != NULL)
                {
                    pCameraBehavior->m_nOrientationState = 0;
                    pCameraBehavior->m_vPlayerOrientation = vOrientation;
                }
            }
            if (GetPlayerCreature() != NULL)
                GetPlayerCreature()->SwitchMode(CSWCCreature::MODE_SWDRIVE);
            if (m_pPlayerControl != NULL)
                m_pPlayerControl->SetEnabled(1);
            if (m_pGuiInGame != NULL)
                m_pGuiInGame->SetSWGuiStatus(
                    CGuiInGame::GUISTATUS_RESTORE_MAIN_INTERFACE, 1);
            break;

        case INPUT_CLASS_DIALOG:
            IndicateObject(NULL, 0);
            break;

        case INPUT_CLASS_MINIGAME:
        {
            CSWMiniGame* pMiniGame = m_pModule->m_pInfo->m_pMiniGame;
            if (pMiniGame != NULL)
            {
                IndicateObject(NULL, 0);
                if (m_pGuiInGame != NULL)
                    m_pGuiInGame->SetSWGuiStatus(CGuiInGame::GUISTATUS_DIALOG, 0);
                if (GetPlayerCreature() != NULL)
                {
                    GetPlayerCreature()->AttachToScene(NULL);
                    m_oidReattachedCreature = GetPlayerCreature()->GetId();
                    SetPlayerCreature(OBJECT_INVALID);
                }
                pMiniGame->SetCamera(m_pModule->GetCamera());
                pMiniGame->Go();
            }
            else
            {
                SetInputClass(nOldClass);
            }
            break;
        }
        }
    }
}
#endif
