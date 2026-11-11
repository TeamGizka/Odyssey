#include "appmanager.h"
#include "clientexoapp.h"
#include "serverexoapp.h"
#include "partytables.h"
#include "swparty.h"
#include "nwsobject.h"
#include "nwscreaturestats.h"
#include "exotlktable.h"
#include "nwsmodule.h"
#include "nwsarea.h"
#include "serverexoappinternal.h"
#include "pathfindinfo.h"
#include "nwscript.h"
#include "swguipartyselection.h"
#include "guiingame.h"
#include "nwccreature.h"
// PC 0x006BE310  Xbox 0x000F2A90
void CSWGuiPartySelection::SetForcedNPC(int nFirstNPC, int nSecondNPC)
{
    if (nFirstNPC >= 0 && nFirstNPC < 9)
    {
        m_bForcedNPC = 1;
        m_aPartyMembers[nFirstNPC].m_bForced = 1;
    }
    if (nSecondNPC >= 0 && nSecondNPC < 9)
    {
        m_bForcedNPC = 1;
        m_aPartyMembers[nSecondNPC].m_bForced = 1;
    }
}

#ifdef _XBOX

inline void SetButtonToggled(CSWGuiButtonToggle& cButton, int bToggled)
{
    cButton.m_bToggled = bToggled;
}
// PC 0x006BECC0
inline void CSWGuiPartySelection::SetAddRemoveLabel(int bAdd)
{
    if (bAdd != 0)
        m_aLabels[LABEL_ADD_REMOVE].m_cText.m_cParams.SetStrRef(0x9637);
    else
        m_aLabels[LABEL_ADD_REMOVE].m_cText.m_cParams.SetStrRef(0x9638);
}
#endif

#ifndef _XBOX
inline CSWGuiPartySelectionButton::~CSWGuiPartySelectionButton()  // PC 0x006BE360
{
}
// Xbox 0x000F33D0
inline CSWGuiPartySelectionButton::CSWGuiPartySelectionButton()
{
    m_bPartySelected = 0;
}
// PC 0x006BE370
void CSWGuiPartySelectionButton::SetSelected(int bSelected)
{
    m_bPartySelected = bSelected;
    if (bSelected != 0)
    {
        m_cBorder.m_cParams.m_fHilightAlpha = 1.0f;
        m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_GREEN;
        m_cHilite.m_cParams.m_vColor = CGuiInGame::COLOR_GREEN;
        m_cHilite.m_cParams.SetPulsingAlpha(1, 1, 0);
    }
    else
    {
        m_cBorder.m_cParams.m_fHilightAlpha = 0.0f;
        m_cHilite.m_cParams.m_vColor = CGuiInGame::COLOR_YELLOW;
        m_cHilite.m_cParams.SetPulsingAlpha(1, 1, 0);
    }
}
#endif
// PC 0x006BE420
inline CSWGuiPartySelectionData::~CSWGuiPartySelectionData()
{
}
// PC 0x006BE490  Xbox 0x000F2B00
void CSWGuiPartySelection::OnPanelRemoved()
{
    CSWPartyTable* pPartyTable =
        g_pAppManager->m_pServerExoApp->GetPartyTable();
    int nNPC = 0;
    CSWGuiPartySelectionData* pData = m_aPartyMembers;
    do
    {
        if (pData->m_bSpawned == 0)
        {
            pPartyTable->KillNPCObject(nNPC, 1);
        }
        ++nNPC;
        ++pData;
    } while (nNPC < 9);
#ifndef _XBOX
    CSWGuiPanel::OnPanelRemoved();
#endif
}
// PC 0x006BFA40  Xbox 0x000F4020
CSWGuiPartySelection::CSWGuiPartySelection(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    CExoString sName;
    m_bForcedNPC = 0;
    StartLoadFromLayout("partyselection");
    for (int nNPC = 0; nNPC < 9; ++nNPC)
    {
        sName.Format("LBL_NA%d", nNPC);
        InitControl(m_aPartyMembers[nNPC].m_lblNotAvailable, sName, 1);
        sName.Format("LBL_CHAR%d", nNPC);
        InitControl(m_aPartyMembers[nNPC].m_lblCharacter, sName, 1);
        sName.Format("BTN_NPC%d", nNPC);
        InitControl(m_aPartyMembers[nNPC].m_btnNPC, sName, 1);
#ifdef _XBOX
        m_aPartyMembers[nNPC].m_btnNPC.m_nToggleEvent = -1;
#endif
        m_aPartyMembers[nNPC].m_btnNPC.AddEvent(CSWGuiControl::EVENT_SELECT, this,
            (CSWGuiEventHandler)&CSWGuiPartySelection::OnToggled);
        m_aPartyMembers[nNPC].m_btnNPC.AddEvent(CSWGuiControl::EVENT_CONFIRM, this,
            (CSWGuiEventHandler)&CSWGuiPartySelection::OnToggled);
        m_aPartyMembers[nNPC].m_btnNPC.AddEvent(CSWGuiControl::EVENT_ENTER, this,
            (CSWGuiEventHandler)&CSWGuiPartySelection::OnEnter);
        m_aPartyMembers[nNPC].m_btnNPC.AddEvent(CSWGuiControl::EVENT_EXIT, this,
            (CSWGuiEventHandler)&CSWGuiPartySelection::OnExit);
#ifdef _XBOX
        m_aPartyMembers[nNPC].m_btnNPC.m_nIndex = (int)&m_aPartyMembers[nNPC];
#else
        m_aPartyMembers[nNPC].m_btnNPC.SetPartyData(&m_aPartyMembers[nNPC]);
#endif
        m_aPartyMembers[nNPC].m_btnNPC.m_cHilite.m_cParams.m_vColor = CGuiInGame::COLOR_YELLOW;
        m_aPartyMembers[nNPC].m_btnNPC.m_cHilite.m_cParams.SetPulsingAlpha(1, 1, 0);
#ifdef _XBOX
        m_aPartyMembers[nNPC].m_btnNPC.m_cToggleBorder.m_cParams.m_vColor = CGuiInGame::COLOR_GREEN;
        m_aPartyMembers[nNPC].m_btnNPC.m_cToggleHilite.m_cParams.m_vColor = CGuiInGame::COLOR_GREEN;
        m_aPartyMembers[nNPC].m_btnNPC.m_cToggleHilite.m_cParams.SetPulsingAlpha(1, 1, 0);
#endif
        m_aPartyMembers[nNPC].m_bForced = 0;
        m_aPartyMembers[nNPC].m_nNPC = nNPC;
    }
    InitControl(m_lblCount, "LBL_COUNT", 1);
#ifdef _XBOX
    InitControl(m_lblB, "LBL_B", 1);
    InitControl(m_lblBack, "LBL_BACK", 1);
#endif
    InitControl(m_lblNPCName, "LBL_NPC_NAME", 1);
    InitControl(m_lblNPCLevel, "LBL_NPC_LEVEL", 1);
    InitControl(m_lbl3D, "LBL_3D", 1);
    InitControl(m_aLabels[LABEL_TITLE], "LBL_TITLE", 1);
#ifdef _XBOX
    InitControl(m_aLabels[1], "LBL_A", 1);
    InitControl(m_aLabels[LABEL_ADD_REMOVE], "LBL_ACCEPT", 1);
#endif
    InitControl(m_aLabels[LABEL_AVAILABLE], "LBL_AVAILABLE", 1);
    InitControl(m_aLabels[LABEL_BEVEL_L], "LBL_BEVEL_L", 1);
    InitControl(m_aLabels[LABEL_BEVEL_R], "LBL_BEVEL_R", 1);
    InitControl(m_aLabels[LABEL_BEVEL_M], "LBL_BEVEL_M", 1);
    InitControl(m_btnDone, "BTN_DONE", 1);
#ifndef _XBOX
    InitControl(m_btnBack, "BTN_BACK", 1);
    InitControl(m_btnAccept, "BTN_ACCEPT", 1);
#endif
    StopLoadFromLayout();
    m_aLabels[LABEL_BEVEL_R].m_cBorder.m_cParams.m_fRotation = 180.0f;
#ifdef _XBOX
    m_btnDone.m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_BLUE;
    m_btnDone.m_cHilite.m_cParams.m_vColor = CGuiInGame::COLOR_YELLOW;
    m_btnDone.m_cHilite.m_cParams.SetPulsingAlpha(1, 1, 0);
#endif
    m_btnDone.AddEvent(CSWGuiControl::EVENT_SELECT, this,
        (CSWGuiEventHandler)&CSWGuiPartySelection::OnDone);
    m_btnDone.AddEvent(CSWGuiControl::EVENT_CONFIRM, this,
        (CSWGuiEventHandler)&CSWGuiPartySelection::OnDone);
#ifdef _XBOX
    m_btnDone.AddEvent(CSWGuiControl::EVENT_ENTER, this,
        (CSWGuiEventHandler)&CSWGuiPartySelection::OnDoneEnter);
#else
    m_btnBack.SetFocusable(0);
    m_btnBack.AddEvent(CSWGuiControl::EVENT_SELECT, this,
        (CSWGuiEventHandler)&CSWGuiPanel::OnBButtonPressed);
    m_btnAccept.SetFocusable(0);
    m_btnAccept.AddEvent(CSWGuiControl::EVENT_SELECT, this,
        (CSWGuiEventHandler)&CSWGuiPartySelection::OnToggled);
    m_pSelectedControl = 0;
    m_btnDone.SetFocusable(0);
#endif
    m_bPlaySound = 0;
}
// PC 0x006BEDE0  Xbox 0x000F3430
void CSWGuiPartySelection::HandleInputEvent(int nEvent, int bPressed)
{
    if (bPressed != 0 && (nEvent == 40 || nEvent == 46))
    {
        m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
        if (m_bPlaySound == 0 && m_bForcedNPC == 0)
        {
#ifdef _XBOX
            for (int nNPC = 0; nNPC < 9; ++nNPC)
            {
                SetButtonToggled(m_aPartyMembers[nNPC].m_btnNPC, m_aPartyMembers[nNPC].m_nPartyIndex != -1);
            }
#else
            CSWGuiPartySelectionData* pData = m_aPartyMembers;
            int nCount = 9;
            do
            {
#ifdef _XBOX
                pData->m_btnNPC.m_bToggled = pData->m_nPartyIndex != -1;
#else
                pData->m_btnNPC.SetSelected(pData->m_nPartyIndex != -1);
#endif
                ++pData;
                --nCount;
            } while (nCount != 0);
#endif
            AcceptParty();
        }
        else
        {
            CSWGuiMessageBox* pMessageBox =
                g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
            pMessageBox->SetAllowCancel(0);
            pMessageBox->SetCallback(0, 0, 0);
            pMessageBox->SetMessage(42405);
            m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, bPressed);
}
// PC 0x006BEEB0  Xbox 0x000F3610
void CSWGuiPartySelection::OnPanelAdded()
{
    CServerExoApp* pServer = g_pAppManager->GetServerExoApp();
    CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
    CSWParty* pParty = pClient->GetSWParty();
    CSWPartyTable* pPartyTable = pServer->GetPartyTable();
#ifdef _XBOX
    if (m_bForcedNPC != 0 || m_bPlaySound != 0)
        m_lblBack.m_cText.m_cParams.SetColor(CGuiInGame::DISABLED_TEXT);
    else
        m_lblBack.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
#else
    if (m_bForcedNPC != 0 || m_bPlaySound != 0)
    {
        m_btnBack.m_cText.m_cParams.SetColor(CGuiInGame::DISABLED_TEXT);
        m_btnBack.m_cBorder.m_cParams.m_vColor = CGuiInGame::DISABLED_ICON_COLOR;
    }
    else
    {
        m_btnBack.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
        m_btnBack.m_cBorder.m_cParams.m_vColor = CGuiInGame::ENABLED_ICON_COLOR;
    }
#endif
    if (m_bForcedNPC != 0)
        m_nSelectedCount = 0;
    else
        m_nSelectedCount = pPartyTable->m_nNumMembers;
    pParty->SetLeader(CSWParty::LEADER_PLAYER);
    pPartyTable->AddGameInProgress();
    for (int nNPC = 0; nNPC < CSWPartyTable::MAX_NPCS; ++nNPC)
    {
        CResRef cPortrait;
#ifdef _XBOX
        SetButtonToggled(m_aPartyMembers[nNPC].m_btnNPC, 0);
#else
        m_aPartyMembers[nNPC].m_btnNPC.SetSelected(0);
#endif
        if (pPartyTable->GetIsNPCAvailable(nNPC))
        {
            unsigned long oidNPC = pPartyTable->GetNPCObject(nNPC, 0, 1);
            CSWSCreature* pCreature = pServer->GetCreatureByGameObjectID(oidNPC);
            if (pCreature != 0)
            {
                pPartyTable->SaveMember(nNPC, 0);
                cPortrait = pCreature->GetPortrait();
                m_aPartyMembers[nNPC].m_bSpawned = 1;
            }
            else
            {
                oidNPC = pPartyTable->GetNPCObject(nNPC, 1, 1);
                pCreature = pServer->GetCreatureByGameObjectID(oidNPC);
                if (pCreature != 0)
                    cPortrait = pCreature->GetPortrait();
                m_aPartyMembers[nNPC].m_bSpawned = 0;
            }
            m_aPartyMembers[nNPC].m_bSelectable = 1;
            if (pPartyTable->GetIsMember(nNPC))
            {
                unsigned long oidClient = pClient->ServerToClientObjectId(oidNPC);
                m_aPartyMembers[nNPC].m_nPartyIndex = pParty->GetIndex(oidClient);
            }
            else
                m_aPartyMembers[nNPC].m_nPartyIndex = -1;
            if (m_bForcedNPC != 0)
            {
                if (m_aPartyMembers[nNPC].m_bForced != 0)
                {
#ifdef _XBOX
                    SetButtonToggled(m_aPartyMembers[nNPC].m_btnNPC, 1);
#else
                    m_aPartyMembers[nNPC].m_btnNPC.SetSelected(1);
#endif
                    ++m_nSelectedCount;
                    m_aPartyMembers[nNPC].m_bSelectable = 0;
                }
            }
            else if (pPartyTable->GetIsMember(nNPC))
#ifdef _XBOX
                SetButtonToggled(m_aPartyMembers[nNPC].m_btnNPC, 1);
#else
                m_aPartyMembers[nNPC].m_btnNPC.SetSelected(1);
#endif
            if (pPartyTable->GetNPCSelectability(nNPC))
                m_aPartyMembers[nNPC].m_lblCharacter.m_cBorder.m_cParams.m_fHilightAlpha = 1.0f;
            else
            {
                m_aPartyMembers[nNPC].m_lblCharacter.m_cBorder.m_cParams.m_fHilightAlpha = 0.25f;
                m_aPartyMembers[nNPC].m_bSelectable = 0;
            }
            m_aPartyMembers[nNPC].m_lblCharacter.SetShown(1);
            m_aPartyMembers[nNPC].m_lblCharacter.m_cBorder.m_cParams.SetFillImage(cPortrait, 0);
            m_aPartyMembers[nNPC].m_lblNotAvailable.SetShown(0);
        }
        else
        {
            m_aPartyMembers[nNPC].m_bSelectable = 0;
            m_aPartyMembers[nNPC].m_bSpawned = 1;
            m_aPartyMembers[nNPC].m_nPartyIndex = -1;
            m_aPartyMembers[nNPC].m_lblNotAvailable.SetShown(1);
            m_aPartyMembers[nNPC].m_lblCharacter.SetShown(0);
        }
    }
    UpdateCount();
    pPartyTable->RemoveGameInProgress();
#ifdef _XBOX
    SetActiveControl(&m_btnDone, 0);
#else
    SetActiveControl(&m_btnAccept, 0);
    CSWGuiPanel::OnPanelAdded();
#endif
    g_pAppManager->GetClientExoApp()->ShowTutorialWindow(
        CClientExoApp::TUTORIAL_PARTY_SELECTION, OBJECT_INVALID, OBJECT_INVALID, 0);
}
// PC 0x006BE4E0  Xbox 0x000F2B90
void CSWGuiPartySelection::UpdateCount()
{
    CExoString sCount;
    sCount.Format("%d", 2 - m_nSelectedCount);
    m_lblCount.m_cText.m_cParams.SetText(sCount);
}
// PC 0x006BE560  Xbox 0x000F2C10
void CSWGuiPartySelection::AcceptParty()
{

    Vector vPosition;
    int nFormationMember = 1;
    CServerExoApp* pServer = g_pAppManager->GetServerExoApp();
    CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
    CSWSCreature* pPlayer =
        pServer->GetCreatureByGameObjectID(pServer->GetPlayerCreatureId());
    CSWParty* pParty = pClient->GetSWParty();
    CSWPartyTable* pPartyTable = pServer->GetPartyTable();
    CSWCCreature* pClientCreature;

    for (int nNPC = 0; nNPC < CSWPartyTable::MAX_NPCS; ++nNPC)
    {
        if (m_aPartyMembers[nNPC].m_nPartyIndex != -1 &&
#ifdef _XBOX
            m_aPartyMembers[nNPC].m_btnNPC.m_bToggled == 0)
#else
            m_aPartyMembers[nNPC].m_btnNPC.m_bPartySelected == 0)
#endif
        {
            pClientCreature =
                pParty->GetCharacter(m_aPartyMembers[nNPC].m_nPartyIndex);
            if (pClientCreature != 0)
            {
                for (int nMember = 0; nMember < CSWPartyTable::MAX_NPCS; ++nMember)
                {
                    if (m_aPartyMembers[nMember].m_nPartyIndex != -1 &&
                        m_aPartyMembers[nMember].m_nPartyIndex >
                            m_aPartyMembers[nNPC].m_nPartyIndex)
                        --m_aPartyMembers[nMember].m_nPartyIndex;
                }
                pClientCreature->SetInParty(0);
                CSWSCreature* pCreature = pClientCreature->GetServerCreature();
                if (pCreature != 0)
                {
                    pCreature->SetInParty(0, 1);
                    pPartyTable->RemoveMember(nNPC);
                    m_aPartyMembers[nNPC].m_bSpawned = 0;
                }
            }
        }
    }

    Vector vOrientation = pPlayer->m_vOrientation;
    pParty->RecaulateFollowPoint();
    for (int nNPC = 0; nNPC < CSWPartyTable::MAX_NPCS; ++nNPC)
    {
        if (m_aPartyMembers[nNPC].m_nPartyIndex == -1 &&
#ifdef _XBOX
            m_aPartyMembers[nNPC].m_btnNPC.m_bToggled != 0)
#else
            m_aPartyMembers[nNPC].m_btnNPC.m_bPartySelected != 0)
#endif
        {
            int nPartyMember = pParty->m_nCharacters;
            EXOXBOXASSERT(nPartyMember >= 0 && nPartyMember < 3);
            Vector vOffset = pParty->m_aPartyMemberData[nPartyMember].m_vFormationOffset;
            EXOXBOXASSERT(nPartyMember >= 0 && nPartyMember < 3);
            Vector vFollowPosition = pParty->m_aPartyMemberData[nPartyMember].m_vFormationPosition;
            if (!vFollowPosition.IsZero())
            {
                EXOXBOXASSERT(nPartyMember >= 0 && nPartyMember < 3);
                int nPathIndex = pParty->m_aPartyMemberData[nPartyMember].m_nFormationPathIndex;
                float fFacing = pParty->m_cFollowPaths.GetPathData(nPathIndex)->m_fFacing;
                vPosition = vFollowPosition +
                    Quaternion(Vector(0.0f, 0.0f, 1.0f), DegToRad(fFacing)) * vOffset;
            }
            else
            {
                vPosition = pParty->GetWorldFormationPosition(
                    pPlayer->m_vPosition, vOrientation, nFormationMember);
            }
            Vector vSpawnOrientation;
            Vector vSpawnPosition;
            unsigned long oidCreature = pPartyTable->SpawnNPC(
                nNPC, 1, vSpawnPosition, vSpawnOrientation, 1);
            pServer->ComputeSafeLocation(vPosition, 5.0f, oidCreature, 1, &vPosition);
            CSWSCreature* pCreature = pServer->GetCreatureByGameObjectID(oidCreature);
            pClientCreature = pCreature->GetClientCreature();
            Vector vLeaderPosition = pParty->GetCharacter(0)->m_vPosition;
            CPathfindInformation* pPathfind = pCreature->GetPathfindInformation();
            float fDistance = pPathfind->m_fStepScale;
            float fWidth = pPathfind->m_fHalfStep;
            int nLineResult = pServer->GetModule()->GetArea()->TestDirectLine(
                vPosition, vLeaderPosition, fDistance, fWidth, 1, 0);
            if (nLineResult != CSWSArea::DIRECT_LINE_CLEAR &&
                nLineResult != CSWSArea::DIRECT_LINE_BLOCKED_CREATURE)
            {
                EXOXBOXASSERT(nPartyMember >= 0 && nPartyMember < 3);
                int nPathIndex = pParty->m_aPartyMemberData[nPartyMember].m_nFormationPathIndex;
                vPosition = pParty->m_cFollowPaths.GetPathData(nPathIndex)->m_vPosition;
                pServer->ComputeSafeLocation(vPosition, 5.0f, oidCreature, 1, &vPosition);
            }
            pCreature->SetPosition(vPosition, 1, 1, 0);
            pCreature->SetOrientation(vOrientation);
            pPartyTable->AddMember(nNPC, oidCreature);
            pCreature->ClearAllActions(1);
            pCreature->GetPathfindInformation()->ResetWayPointData();
            pCreature->SetInParty(1, 1);
            pCreature->AIUpdate();
            if (pPlayer->GetVisibleListElement(pCreature->GetId()) == 0)
                pPlayer->AddToVisibleList(pCreature->GetId(), 1, 1, 0, 0);
            if (pClientCreature != 0)
                pClientCreature->SetInParty(1);
            ++nFormationMember;
            m_aPartyMembers[nNPC].m_bSpawned = 1;
        }
    }
    for (int nNPC = 0; nNPC < CSWPartyTable::MAX_NPCS; ++nNPC)
        m_aPartyMembers[nNPC].m_bForced = 0;
    pServer->m_pcExoAppInternal->UpdateClientGameObjects(1);
    m_bForcedNPC = 0;
    pClient->SetRunScript(m_sRunScript);
    if (g_bCreateAllGUIs != 0)
    {
        SetModal(0);
    }
    else
    {
        g_pAppManager->GetClientExoApp()->GetInGameGui()->m_pPartySelectionPanel = 0;
        m_bDeferredRemoval = 0;
        m_nExitState = EXITSTATE_DELETE;
    }
    GetManager()->PopModalPanel();
    if (m_sRunScript != "")
    {
        g_pAppManager->GetClientExoApp()->GetInGameGui()->m_bTransitionScriptRunning = 1;
        g_pVirtualMachine->RunScript(&m_sRunScript, 0, 0);
        m_sRunScript = "";
        g_pAppManager->GetClientExoApp()->GetInGameGui()->m_bTransitionScriptRunning = 0;
    }
    if (m_nInputClass == INPUT_CLASS_GAME && g_pAppManager->GetClientExoApp()->GetPausedByCombat() != 0)
        g_pAppManager->GetClientExoApp()->SetPausedByCombat(0, 0, 0);
    if (m_bPlaySound != 0)
    {
        GetManager()->PlayGuiSound(CSWGuiManager::GUISOUND_PARTY_ACCEPT);
        g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_GENERIC);
    }
    g_pAppManager->GetClientExoApp()->SetInputClass(m_nInputClass, 1);
    CGuiInGame* pInGame = g_pAppManager->GetClientExoApp()->GetInGameGui();
    pInGame->m_pMessageBox->m_nPreviousInputClass = m_nInputClass;
}
// PC 0x006BF2A0  Xbox 0x000F38D0
void CSWGuiPartySelection::OnToggled(CSWGuiControl* pControl)
{
    CSWGuiPartySelectionButton* pButton = (CSWGuiPartySelectionButton*)pControl;
    int bEnabled = pControl->m_bSelected;
    if (bEnabled != 0)
    {
        if (pControl->GetEvent() == 45)
        {
            m_pManager->PlayGuiSound(pControl->GetGuiSound());
        }
        CSWGuiPartySelectionData* pData;
#ifdef _XBOX
        pData = (CSWGuiPartySelectionData*)pControl->GetUserData();
#else
        if (pControl == &m_btnAccept)
        {
            pData = (CSWGuiPartySelectionData*)m_pSelectedControl;
            if (pData == 0)
            {
                return;
            }
            pButton = &pData->m_btnNPC;
        }
        else
        {
            pData = (CSWGuiPartySelectionData*)pControl->GetUserData();
        }
#endif
        if (pData->m_bSelectable != 0)
        {
#ifdef _XBOX
            if (pButton->m_bToggled != 0)
#else
            if (pButton->m_bPartySelected != 0)
#endif
            {
                --m_nSelectedCount;
#ifdef _XBOX
                pButton->m_bToggled = 0;
#else
                pButton->SetSelected(0);
#endif
            }
            else if (m_nSelectedCount < 2)
            {
                ++m_nSelectedCount;
#ifdef _XBOX
                pButton->m_bToggled = 1;
#else
                pButton->SetSelected(1);
#endif
            }
#ifdef _XBOX
            SetAddRemoveLabel(pButton->m_bToggled == 0);
#else
            SetAddRemoveLabel(pButton->m_bPartySelected == 0);
#endif
            UpdateCount();
        }
        else
        {
            CSWGuiMessageBox* pMessageBox =
                g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
            pMessageBox->SetAllowCancel(0);
            pMessageBox->SetCallback(0, 0, 0);
            if (pData->m_bForced != 0)
            {
                pMessageBox->SetMessage(42406);
            }
            else
            {
                pMessageBox->SetMessage(42376);
            }
            m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
        }
    }
}
// PC 0x006BF3B0  Xbox 0x000F39F0
void CSWGuiPartySelection::OnDone(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        if (pControl->GetEvent() == 45)
        {
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
        }
        if (m_bPlaySound != 0)
        {
            unsigned char nSelected = 0;
            unsigned char nForced = 0;
            unsigned char nSelectable = 0;
            for (int i = 0; i < 9; ++i)
            {
                if (m_aPartyMembers[i].m_bSelectable != 0)
                    ++nSelectable;
                if (m_aPartyMembers[i].m_bForced != 0)
                    ++nForced;
#ifdef _XBOX
                if (m_aPartyMembers[i].m_btnNPC.m_bToggled != 0)
#else
                if (m_aPartyMembers[i].m_btnNPC.m_bPartySelected != 0)
#endif
                    ++nSelected;
            }
            if (nSelectable == 0 || nForced == 2)
            {
                AcceptParty();
#ifndef _XBOX
                g_pAppManager->m_pClientExoApp->GetInGameGui()
                    ->m_pPartySelection = 0;
#endif
            }
            else if (nSelectable > 0)
            {
                CSWGuiMessageBox* pMessageBox =
                    g_pAppManager->m_pClientExoApp->GetInGameGui()
                        ->m_pMessageBox;
                pMessageBox->SetAllowCancel(1);
                if (nSelectable <= 1 || nSelected >= 2)
                    pMessageBox->SetMessage(38328);
                else
                    pMessageBox->SetMessage(38329);
                pMessageBox->SetCallback(
                    this,
                    (CSWGuiEventHandler)&CSWGuiPartySelection::
                        ConfirmPartySelection,
                    0);
                m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
            }
        }
        else
        {
            AcceptParty();
        }
    }
}
// PC 0x006BEC90  Xbox 0x000F33A0
void CSWGuiPartySelection::ConfirmPartySelection(CSWGuiControl*)
{
    CGuiInGame* pInGame = g_pAppManager->m_pClientExoApp->GetInGameGui();
    if (pInGame->m_pMessageBox->m_bAccepted != 0)
    {
        AcceptParty();
    }
}
// PC 0x006BF5B0  Xbox 0x000F3B30
void CSWGuiPartySelection::OnEnter(CSWGuiControl* pControl)
{
    CServerExoApp* pServer = g_pAppManager->GetServerExoApp();
    CSWPartyTable* pPartyTable = pServer->GetPartyTable();
    CSWGuiPartySelectionData* pData =
        (CSWGuiPartySelectionData*)pControl->GetUserData();
    if (pPartyTable->GetIsNPCAvailable(pData->m_nNPC))
    {
        CSWSCreature* pCreature = pServer->GetCreatureByGameObjectID(
            pPartyTable->GetNPCObject(pData->m_nNPC, 0, 1));
        if (pCreature != 0)
        {
            CExoString sPortrait;
            pCreature->GetPortrait().CopyToString(sPortrait);
            sPortrait = sPortrait + "3";
            if (pCreature->GetStats()->m_nGoodEvilAlignment <
                CSWSCreatureStats::DARK_PORTRAIT_ALIGNMENT_THRESHOLD)
            {
                CExoString sEvilPortrait = sPortrait + "e";
                if (g_pExoResMan->Exists(sEvilPortrait, RESTYPE_TGA,
                                       (unsigned long*)0) ||
#ifdef _XBOX
                    g_pExoResMan->Exists(sEvilPortrait, RESTYPE_TXB,
#else
                    g_pExoResMan->Exists(sEvilPortrait, RESTYPE_TPC,
#endif
                                       (unsigned long*)0))
                    sPortrait = sEvilPortrait;
            }
            m_lbl3D.m_cBorder.m_cParams.SetFillImage(sPortrait, 0);
            CExoString sLevel;
            sLevel.Format("%s %d %s",
                g_pTlkTable->GetSimpleString(STRREF_LEVEL).CStr(),
                pCreature->GetStats()->GetLevel(0),
                pCreature->GetStats()->GetClassString(
                    pCreature->GetStats()->GetNumClasses() - 1).CStr());
            m_lblNPCName.m_cText.m_cParams.SetText(
                pCreature->GetStats()->GetFullName());
            m_lblNPCLevel.m_cText.m_cParams.SetText(sLevel);
            m_lbl3D.SetShown(1);
            m_lblNPCName.SetShown(1);
            m_lblNPCLevel.SetShown(1);
            if (m_nSelectedCount < MAX_SELECTED_NPCS && pData->m_bSelectable != 0)
                m_aLabels[LABEL_ADD_REMOVE].m_cText.m_cParams.SetColor(
                    CGuiInGame::COLOR_BLUE);
            else
                m_aLabels[LABEL_ADD_REMOVE].m_cText.m_cParams.SetColor(
                    CGuiInGame::DISABLED_TEXT);
#ifdef _XBOX
            SetAddRemoveLabel(((CSWGuiPartySelectionButton*)pControl)->m_bToggled == 0);
#else
            int bSelected = ((CSWGuiPartySelectionButton*)pControl)->m_bPartySelected;
            SetAddRemoveLabel(bSelected == 0);
#endif
        }
        else
        {
            m_lblNPCName.m_cText.m_cParams.SetText("I am broken");
            m_lblNPCLevel.m_cText.m_cParams.SetText("So very very broken");
            m_lblNPCName.SetShown(1);
            m_lblNPCLevel.SetShown(1);
            m_lbl3D.SetShown(0);
            m_aLabels[LABEL_ADD_REMOVE].m_cText.m_cParams.SetColor(
                CGuiInGame::DISABLED_TEXT);
            SetAddRemoveLabel(1);
        }
#ifndef _XBOX
        if (m_pSelectedControl != 0)
            m_pSelectedControl->SetActive(0);
        m_pSelectedControl = &pData->m_btnNPC;
        m_pSelectedControl->SetActive(1);
#endif
    }
    else
    {
        m_lbl3D.SetShown(0);
        m_lblNPCName.SetShown(0);
        m_lblNPCLevel.SetShown(0);
        m_aLabels[LABEL_ADD_REMOVE].m_cText.m_cParams.SetColor(
            CGuiInGame::DISABLED_TEXT);
        SetAddRemoveLabel(1);
#ifndef _XBOX
        if (m_pSelectedControl != 0)
        {
            m_pSelectedControl->SetActive(0);
            m_pSelectedControl = 0;
        }
#endif
    }
}

#ifndef _XBOX
void CSWGuiPartySelection::SetAddRemoveLabel(int bAdd)  // PC 0x006BECC0
{
    CSWGuiTextParams* pButtonLabel;
    if (bAdd != 0)
    {
        m_aLabels[2].m_cText.m_cParams.SetStrRef(0x9637);
        pButtonLabel = &m_btnAccept.m_cText.m_cParams;
        pButtonLabel->SetStrRef(0x9637);
        if (m_nSelectedCount >= 2)
        {
            pButtonLabel->SetColor(CGuiInGame::DISABLED_TEXT);
            m_btnAccept.m_cBorder.m_cParams.m_vColor =
                CGuiInGame::DISABLED_ICON_COLOR;
            return;
        }
    }
    else
    {
        m_aLabels[2].m_cText.m_cParams.SetStrRef(0x9638);
        pButtonLabel = &m_btnAccept.m_cText.m_cParams;
        pButtonLabel->SetStrRef(0x9638);
    }
    pButtonLabel->SetColor(CGuiInGame::COLOR_BLUE);
    m_btnAccept.m_cBorder.m_cParams.m_vColor = CGuiInGame::ENABLED_ICON_COLOR;
}
#endif

#ifdef _XBOX
void CSWGuiPartySelection::OnDoneEnter(CSWGuiControl* pControl)  // Xbox 0x000F2B40
{
    m_lbl3D.SetShown(0);
    m_lblNPCName.SetShown(0);
    m_lblNPCLevel.SetShown(0);
    m_aLabels[2].m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
    m_aLabels[2].m_cText.m_cParams.SetStrRef(1073);
}
#endif
