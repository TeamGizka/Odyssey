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
#include "nwmessage.h"
#include "nwccreature.h"
#include "nwccreaturestats.h"
#include "nwccreatureappearance.h"
#include "swmain.h"
#include "aurcamera.h"
#include "aurscene.h"
#include "math3d.h"

#ifdef _XBOX

inline void SetButtonToggled(CSWGuiButtonToggle& cButton, int bToggled)
{
    cButton.m_bToggled = bToggled;
}

inline void CSWGuiPartySelection::SetAddRemoveLabel(int bAdd)
{
    if (bAdd != 0)
        m_aLabels[LABEL_ADD_REMOVE].m_cText.m_cParams.SetStrRef(STRREF_ADD);
    else
        m_aLabels[LABEL_ADD_REMOVE].m_cText.m_cParams.SetStrRef(STRREF_REMOVE);
}
#endif

#ifndef _XBOX
inline CSWGuiPartySelectionButton::~CSWGuiPartySelectionButton()
{
}

inline CSWGuiPartySelectionButton::CSWGuiPartySelectionButton()
{
    m_bPartySelected = 0;
}
// PC 0x0071DEB0
void CSWGuiPartySelectionButton::SetSelected(int bSelected)
{
    m_bPartySelected = bSelected;
    m_cBorder.m_cParams.m_fHilightAlpha = 1.0f;
    if (bSelected != 0)
    {
        m_cBorder.m_cParams.m_vColor = CGuiInGame::DISABLED_TEXT;
        m_cHilite.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_HIGHLIGHT;
        m_cHilite.m_cParams.SetPulsingAlpha(1, 1, 0);
    }
    else
    {
        m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_GREEN;
        m_cHilite.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_TEXT;
        m_cHilite.m_cParams.SetPulsingAlpha(1, 1, 0);
    }
}
#endif

#ifdef _XBOX
void CSWGuiPartySelection::OnDoneEnter(CSWGuiControl* pControl)
{
    m_lbl3D.SetShown(0);
    m_lblNPCName.SetShown(0);
    m_lblNPCLevel.SetShown(0);
    m_aLabels[2].m_cText.m_cParams.SetColor(CGuiInGame::COLOR_BLUE);
    m_aLabels[2].m_cText.m_cParams.SetStrRef(1073);
}
#endif
// PC 0x0071EE10
void CSWGuiPartySelection::OnToggled(CSWGuiControl* pControl)
{
    int bEnabled = pControl->m_bSelected;
    if (bEnabled != 0)
    {
        CSWGuiMessageBox* pMessageBox;
#ifndef _XBOX
        if (m_bCanChangeParty == 0)
        {
            CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
            CGuiInGame* pInGameGui = pClientExoApp->GetInGameGui();
            pMessageBox = pInGameGui->m_pMessageBox;
            pMessageBox->SetAllowCancel(0);
            pMessageBox->SetCallback(0, 0, 0);
            pMessageBox->SetMessage(m_nCannotChangeStrRef);
            m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
            return;
        }
#endif
        if (pControl->GetEvent() == 45)
        {
            m_pManager->PlayGuiSound(pControl->GetGuiSound());
        }
        CSWGuiPartySelectionButton* pButton;
        CSWGuiPartySelectionData* pData;
#ifdef _XBOX
        pButton = (CSWGuiPartySelectionButton*)pControl;
        pData = (CSWGuiPartySelectionData*)pControl->GetUserData();
#else
        if (pControl == &m_btnAccept)
        {
            pData = m_pSelectedData;
            if (pData == 0)
            {
                return;
            }
            pButton = &pData->m_btnNPC;
        }
        else
        {
            pButton = (CSWGuiPartySelectionButton*)pControl;
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
            else if (m_nSelectedCount < MAX_SELECTED_NPCS)
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
            pMessageBox = g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
            pMessageBox->SetAllowCancel(0);
            pMessageBox->SetCallback(0, 0, 0);
            if (pData->m_bForced != 0)
            {
                pMessageBox->SetMessage(STRREF_MUST_BE_MEMBER);
            }
#ifndef _XBOX
            else if (pButton->m_bPartySelected != 0)
            {
                pMessageBox->SetMessage(STRREF_UNABLE_TO_LEAVE);
            }
#endif
            else
            {
                pMessageBox->SetMessage(STRREF_UNABLE_TO_JOIN);
            }
            m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
        }
    }
}
// PC 0x0071EF70
void CSWGuiPartySelection::OnDone(CSWGuiControl* pControl)
{
    if (pControl != 0)
    {
        if (pControl->m_bSelected == 0)
            return;
        if (pControl->GetEvent() == 45)
        {
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
        }
    }
    if (m_bPlaySound != 0)
    {
        unsigned char nSelected = 0;
        unsigned char nForced = 0;
        unsigned char nSelectable = 0;
        for (int i = 0; i < 12; ++i)
        {
            if (m_aPartyMembers[i].m_bSelectable != 0)
                ++nSelectable;
            if (m_aPartyMembers[i].m_bForced != 0)
                ++nForced;
            if (m_aPartyMembers[i].m_btnNPC.m_bPartySelected != 0)
                ++nSelected;
        }
        if (nSelectable == 0 || nForced == 2)
        {
            AcceptParty(0);
            g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME, 1);
            g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pPartySelection = 0;
            SetModal(1);
        }
        else if (nSelectable > 0)
        {
            CSWGuiMessageBox* pMessageBox =
                g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
            pMessageBox->SetAllowCancel(1);
            if (nSelectable <= 1 || nSelected >= 2)
                pMessageBox->SetMessage(STRREF_CONFIRM_PARTY);
            else
                pMessageBox->SetMessage(STRREF_CONFIRM_SMALL_PARTY);
            pMessageBox->SetCallback(
                this,
                (CSWGuiEventHandler)&CSWGuiPartySelection::ConfirmPartySelection,
                0);
            m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
        }
    }
    else
    {
        if (m_bCanChangeParty == 0)
        {
            for (int nNPC = 0; nNPC < 12; ++nNPC)
                m_aPartyMembers[nNPC].m_btnNPC.SetSelected(m_aPartyMembers[nNPC].m_nPartyIndex != -1);
        }
        AcceptParty(1);
    }
}
// PC 0x0071F1C0
#ifdef _XBOX
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
#ifdef _XBOX
            m_lbl3D.m_cBorder.m_cParams.SetFillImage(sPortrait, 0);
#endif
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
            SetAddRemoveLabel(((CSWGuiPartySelectionButton*)pControl)->m_bToggled == 0);
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
    }
    else
    {
        m_lbl3D.SetShown(0);
        m_lblNPCName.SetShown(0);
        m_lblNPCLevel.SetShown(0);
        m_aLabels[LABEL_ADD_REMOVE].m_cText.m_cParams.SetColor(
            CGuiInGame::DISABLED_TEXT);
        SetAddRemoveLabel(1);
    }
}
#else
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
        if (pData->m_pCreature == 0)
            pData->m_pCreature = new CSWCCreature();
        if (pCreature != 0)
        {
            CSWSCreatureStats* pStats = pCreature->GetStats();
            CSWCCreatureStats* pPreviewStats = pData->m_pCreature->GetStats();
            pPreviewStats->SetGender(pStats->m_nGender);
            pPreviewStats->SetGoodEvilAlignment(pCreature->GetStats()->m_nGoodEvilAlignment);
            pPreviewStats->SetBaseCNPCAlignment(pCreature->GetStats()->m_n18C);
            for (int nClass = 0; nClass < pStats->GetNumClasses(); ++nClass)
                pPreviewStats->SetClass(nClass, pStats->GetClass(nClass));
            if (g_bDo3DGUIs != 0)
            {
                CCAppearanceInfo aAppearance = *pData->m_pCreature->GetAppearance();
                aAppearance.m_nGender = pCreature->m_cAppearanceInfo.m_nAppearancePart1;
                aAppearance.m_nAppearance = pCreature->m_cAppearanceInfo.m_nAppearance;
                if (aAppearance.m_nAppearance == CCAppearanceInfo::APPEARANCE_HANDMAIDEN_DEFAULT)
                    aAppearance.m_nAppearance = CCAppearanceInfo::APPEARANCE_HANDMAIDEN_ROBED_LIGHT;
                aAppearance.m_bUseBodyVariation = 1;
                aAppearance.m_nBodyVariation = CCAppearanceInfo::BODY_VARIATION_B;
                aAppearance.m_nTextureVar = 1;
                pData->m_pCreature->SetPortraitId(pCreature->GetPortraitId());
                pData->m_pCreature->SetAppearance(&aAppearance,
                    CSWCCreature::APPEARANCE_PART_BODY | CSWCCreature::APPEARANCE_PART_HEAD, 1);
                CAurScene* pScene = m_lbl3D.m_cScene.m_pScene;
                pData->m_pCreature->AttachToScene(pScene);
                pData->m_pCreature->Setup3DScene(m_lbl3D, 0, pCreature, 1);
            }
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
            int bSelected = ((CSWGuiPartySelectionButton*)pControl)->m_bPartySelected;
            SetAddRemoveLabel(bSelected == 0);
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
        if (m_pSelectedData != 0 && m_pSelectedData != pData)
        {
            if (m_pSelectedData->m_pCreature != 0)
                m_pSelectedData->m_pCreature->AttachToScene(0);
            m_pSelectedData->m_btnNPC.SetActive(0);
        }
        m_pSelectedData = pData;
        pData->m_btnNPC.SetActive(1);
    }
    else
    {
        m_lbl3D.SetShown(0);
        m_lblNPCName.SetShown(0);
        m_lblNPCLevel.SetShown(0);
        m_aLabels[LABEL_ADD_REMOVE].m_cText.m_cParams.SetColor(
            CGuiInGame::DISABLED_TEXT);
        SetAddRemoveLabel(1);
        if (m_pSelectedData != 0)
        {
            if (m_pSelectedData->m_pCreature != 0)
                m_pSelectedData->m_pCreature->AttachToScene(0);
            m_pSelectedData->m_btnNPC.SetActive(0);
            m_pSelectedData = 0;
        }
    }
}
#endif
// PC 0x0071DF70
void CSWGuiPartySelection::OnPanelRemoved()
{
    CSWPartyTable* pPartyTable =
        g_pAppManager->m_pServerExoApp->GetPartyTable();
    g_pAppManager->m_pClientExoApp->GetSWParty();
    for (int nNPC = 0; nNPC < 12; ++nNPC)
    {
        if (m_aPartyMembers[nNPC].m_bSpawned == 0)
        {
            pPartyTable->KillNPCObject(nNPC, 1);
        }
        if (m_aPartyMembers[nNPC].m_pCreature != 0)
        {
            delete m_aPartyMembers[nNPC].m_pCreature;
            m_aPartyMembers[nNPC].m_pCreature = 0;
        }
    }
    g_pAppManager->m_pClientExoApp->GetInGameGui()->EnableCharacterChangeButtons(1);
    m_bPlaySound = 0;
    m_n78 = 0;
    CSWGuiPanel::OnPanelRemoved();
}
#ifndef _XBOX
// PC 0x0071E000
void CSWGuiPartySelection::OnExit(CSWGuiControl* pControl)
{
    CSWGuiPartySelectionData* pData =
        (CSWGuiPartySelectionData*)pControl->GetUserData();
    if (pData->m_pCreature != 0)
    {
        delete pData->m_pCreature;
        pData->m_pCreature = 0;
    }
}
#endif
// PC 0x0071E020
void CSWGuiPartySelection::UpdateCount()
{
    CExoString sCount;
    sCount.Format("%d", MAX_SELECTED_NPCS - m_nSelectedCount);
    sCount = sCount + " " + g_pTlkTable->GetSimpleString(STRREF_AVAILABLE_SLOTS);
    m_lblCount.m_cText.m_cParams.SetText(sCount);
}
// PC 0x0071E140
void CSWGuiPartySelection::SetForcedNPC(int nFirstNPC, int nSecondNPC)
{
    if (nFirstNPC >= 0 && nFirstNPC < 12)
    {
        m_bForcedNPC = 1;
        m_aPartyMembers[nFirstNPC].m_bForced = 1;
    }
    if (nSecondNPC >= 0 && nSecondNPC < 12)
    {
        m_bForcedNPC = 1;
        m_aPartyMembers[nSecondNPC].m_bForced = 1;
    }
}
// PC 0x0071E190
void CSWGuiPartySelection::AcceptParty(int bCancelled)
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
    int bFirstRemoval = 1;

    for (int nNPC = 0; nNPC < CSWPartyTable::MAX_NPCS; ++nNPC)
    {
        if (m_aPartyMembers[nNPC].m_nPartyIndex != -1 &&
#ifdef _XBOX
            m_aPartyMembers[nNPC].m_btnNPC.m_bToggled == 0)
#else
            m_aPartyMembers[nNPC].m_btnNPC.m_bPartySelected == 0)
#endif
        {
            if (bFirstRemoval != 0)
            {
                pParty->SetLeader(-3);
                CSWPartyTable* pTable = g_pAppManager->GetServerExoApp()->GetPartyTable();
                for (int nMember = 0; nMember < CSWPartyTable::MAX_NPCS; ++nMember)
                {
                    if (pTable->GetIsMember(nMember))
                        m_aPartyMembers[nMember].m_nPartyIndex = pParty->GetIndex(
                            g_pAppManager->m_pClientExoApp->ServerToClientObjectId(pTable->GetNPCObject(nMember, 0, 0)));
                }
                bFirstRemoval = 0;
            }
            pClientCreature =
                pParty->GetCharacter(m_aPartyMembers[nNPC].m_nPartyIndex);
            if (pClientCreature != 0)
            {
                for (int nMember = 0; nMember < CSWPartyTable::MAX_NPCS; ++nMember)
                {
                    int nPartyIndex = m_aPartyMembers[nMember].m_nPartyIndex;
                    if (nPartyIndex != -1 &&
                        nPartyIndex > m_aPartyMembers[nNPC].m_nPartyIndex)
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
    if (g_pAppManager->GetClientExoApp()->GetInGameGui()->HideSWInGameGui(0))
        g_pAppManager->GetClientExoApp()->SetInputClass(INPUT_CLASS_GAME, 1);
    if (m_sRunScript != "")
    {
        g_pAppManager->GetClientExoApp()->GetInGameGui()->m_bTransitionScriptRunning = 1;
        g_nRunScriptVar = bCancelled == 0;
        g_pVirtualMachine->RunScript(&m_sRunScript, 0, 0);
        m_sRunScript = "";
        g_pAppManager->GetClientExoApp()->GetInGameGui()->m_bTransitionScriptRunning = 0;
    }
    if (m_bPlaySound != 0 && g_pAppManager->GetClientExoApp()->GetPausedByCombat() != 0)
        g_pAppManager->GetClientExoApp()->GetSWCMessage()->SendPlayerToServerInput_TogglePauseRequest();
    if (m_bPlaySound != 0)
    {
        GetManager()->PlayGuiSound(CSWGuiManager::GUISOUND_PARTY_ACCEPT);
        g_pExoSound->SetSoundMode(CExoSound::CEXOSOUND_MODE_GENERIC);
        g_pAppManager->GetClientExoApp()->SetInputClass(INPUT_CLASS_GAME, 1);
        SetModal(0);
    }
}
// PC 0x0071E9C0
void CSWGuiPartySelection::ConfirmPartySelection(CSWGuiControl*)
{
    CGuiInGame* pInGame = g_pAppManager->m_pClientExoApp->GetInGameGui();
    if (pInGame->m_pMessageBox->m_bAccepted != 0)
    {
        AcceptParty(0);
        g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME, 1);
        g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pPartySelection = 0;
        SetModal(1);
    }
}

#ifndef _XBOX
// PC 0x0071EA40
void CSWGuiPartySelection::SetAddRemoveLabel(int bAdd)
{
    CSWGuiTextParams* pButtonLabel;
    if (bAdd != 0)
    {
        m_aLabels[LABEL_ADD_REMOVE].m_cText.m_cParams.SetStrRef(STRREF_ADD);
        pButtonLabel = &m_btnAccept.m_cText.m_cParams;
        pButtonLabel->SetStrRef(STRREF_ADD);
    }
    else
    {
        m_aLabels[LABEL_ADD_REMOVE].m_cText.m_cParams.SetStrRef(STRREF_REMOVE);
        pButtonLabel = &m_btnAccept.m_cText.m_cParams;
        pButtonLabel->SetStrRef(STRREF_REMOVE);
    }
    if ((bAdd == 0 || m_nSelectedCount < MAX_SELECTED_NPCS) && m_bCanChangeParty != 0)
    {
        pButtonLabel->SetDefaultColor(CGuiInGame::COLOR_NEW_TEXT);
        m_btnAccept.m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_GREEN;
    }
    else
    {
        pButtonLabel->SetDefaultColor(CGuiInGame::COLOR_NEW_GREEN);
        m_btnAccept.m_cBorder.m_cParams.m_vColor = CGuiInGame::DISABLED_TEXT;
    }
}
// PC 0x0071EB00
void CSWGuiPartySelection::UpdateVisibleCharacters()
{
    CSWPartyTable* pPartyTable = g_pAppManager->m_pServerExoApp->GetPartyTable();
    CSWGuiPartySelectionButton* pShared;
    if (pPartyTable->GetIsNPCAvailable(NPC_HANHARR))
    {
        m_aPartyMembers[NPC_MIRA].m_lblCharacter.SetShown(0);
        m_aPartyMembers[NPC_MIRA].m_lblNotAvailable.SetShown(0);
        m_aPartyMembers[NPC_MIRA].m_btnNPC.SetShown(0);
        m_aPartyMembers[NPC_MIRA].m_bHidden = 1;
        m_aPartyMembers[NPC_HANHARR].m_lblCharacter.SetShown(1);
        m_aPartyMembers[NPC_HANHARR].m_lblNotAvailable.SetShown(1);
        pShared = &m_aPartyMembers[NPC_HANHARR].m_btnNPC;
        pShared->SetShown(1);
        m_aPartyMembers[NPC_HANHARR].m_bHidden = 0;
        m_aPartyMembers[NPC_HK_47].m_btnNPC.SetMoveToControl(2, pShared);
        m_aPartyMembers[NPC_ATTON].m_btnNPC.SetMoveToControl(0, &m_btnDone);
        m_aPartyMembers[NPC_T3_M4].m_btnNPC.SetMoveToControl(1, pShared);
        m_aPartyMembers[NPC_VISAS].m_btnNPC.SetMoveToControl(3, pShared);
        pShared->SetMoveToControl(2, &m_btnDone);
        pShared->SetMoveToControl(0, &m_aPartyMembers[NPC_HK_47].m_btnNPC);
        pShared->SetMoveToControl(1, &m_aPartyMembers[NPC_VISAS].m_btnNPC);
        pShared->SetMoveToControl(3, &m_aPartyMembers[NPC_T3_M4].m_btnNPC);
    }
    else
    {
        m_aPartyMembers[NPC_HANHARR].m_btnNPC.SetShown(0);
        m_aPartyMembers[NPC_HANHARR].m_lblCharacter.SetShown(0);
        m_aPartyMembers[NPC_HANHARR].m_lblNotAvailable.SetShown(0);
        m_aPartyMembers[NPC_HANHARR].m_bHidden = 1;
        pShared = &m_aPartyMembers[NPC_MIRA].m_btnNPC;
        pShared->SetShown(1);
        m_aPartyMembers[NPC_MIRA].m_lblCharacter.SetShown(1);
        m_aPartyMembers[NPC_MIRA].m_lblNotAvailable.SetShown(1);
        m_aPartyMembers[NPC_MIRA].m_bHidden = 0;
        m_aPartyMembers[NPC_HK_47].m_btnNPC.SetMoveToControl(2, pShared);
        m_aPartyMembers[NPC_ATTON].m_btnNPC.SetMoveToControl(0, &m_btnDone);
        m_aPartyMembers[NPC_T3_M4].m_btnNPC.SetMoveToControl(1, pShared);
        m_aPartyMembers[NPC_VISAS].m_btnNPC.SetMoveToControl(3, pShared);
        pShared->SetMoveToControl(2, &m_btnDone);
        pShared->SetMoveToControl(0, &m_aPartyMembers[NPC_HK_47].m_btnNPC);
        pShared->SetMoveToControl(1, &m_aPartyMembers[NPC_VISAS].m_btnNPC);
        pShared->SetMoveToControl(3, &m_aPartyMembers[NPC_T3_M4].m_btnNPC);
    }
    if (pPartyTable->GetIsNPCAvailable(NPC_DISCIPLE))
    {
        m_aPartyMembers[NPC_HANDMAIDEN].m_btnNPC.SetShown(0);
        m_aPartyMembers[NPC_HANDMAIDEN].m_lblCharacter.SetShown(0);
        m_aPartyMembers[NPC_HANDMAIDEN].m_lblNotAvailable.SetShown(0);
        m_aPartyMembers[NPC_HANDMAIDEN].m_bHidden = 1;
        pShared = &m_aPartyMembers[NPC_DISCIPLE].m_btnNPC;
        pShared->SetShown(1);
        m_aPartyMembers[NPC_DISCIPLE].m_lblCharacter.SetShown(1);
        m_aPartyMembers[NPC_DISCIPLE].m_lblNotAvailable.SetShown(1);
        m_aPartyMembers[NPC_DISCIPLE].m_bHidden = 0;
        m_aPartyMembers[NPC_CANDEROUS].m_btnNPC.SetMoveToControl(2, pShared);
        m_aPartyMembers[NPC_KREIA].m_btnNPC.SetMoveToControl(0, pShared);
        m_aPartyMembers[NPC_G0T0].m_btnNPC.SetMoveToControl(1, pShared);
        m_aPartyMembers[NPC_G0T0].m_btnNPC.SetMoveToControl(3, pShared);
        pShared->SetMoveToControl(0, &m_aPartyMembers[NPC_CANDEROUS].m_btnNPC);
        pShared->SetMoveToControl(2, &m_aPartyMembers[NPC_KREIA].m_btnNPC);
        pShared->SetMoveToControl(1, &m_aPartyMembers[NPC_G0T0].m_btnNPC);
        pShared->SetMoveToControl(3, &m_aPartyMembers[NPC_G0T0].m_btnNPC);
    }
    else
    {
        m_aPartyMembers[NPC_DISCIPLE].m_btnNPC.SetShown(0);
        m_aPartyMembers[NPC_DISCIPLE].m_lblCharacter.SetShown(0);
        m_aPartyMembers[NPC_DISCIPLE].m_lblNotAvailable.SetShown(0);
        m_aPartyMembers[NPC_DISCIPLE].m_bHidden = 1;
        pShared = &m_aPartyMembers[NPC_HANDMAIDEN].m_btnNPC;
        pShared->SetShown(1);
        m_aPartyMembers[NPC_HANDMAIDEN].m_lblCharacter.SetShown(1);
        m_aPartyMembers[NPC_HANDMAIDEN].m_lblNotAvailable.SetShown(1);
        m_aPartyMembers[NPC_HANDMAIDEN].m_bHidden = 0;
        m_aPartyMembers[NPC_CANDEROUS].m_btnNPC.SetMoveToControl(2, pShared);
        m_aPartyMembers[NPC_KREIA].m_btnNPC.SetMoveToControl(0, pShared);
        m_aPartyMembers[NPC_G0T0].m_btnNPC.SetMoveToControl(1, pShared);
        m_aPartyMembers[NPC_G0T0].m_btnNPC.SetMoveToControl(3, pShared);
        pShared->SetMoveToControl(0, &m_aPartyMembers[NPC_CANDEROUS].m_btnNPC);
        pShared->SetMoveToControl(2, &m_aPartyMembers[NPC_KREIA].m_btnNPC);
        pShared->SetMoveToControl(1, &m_aPartyMembers[NPC_G0T0].m_btnNPC);
        pShared->SetMoveToControl(3, &m_aPartyMembers[NPC_G0T0].m_btnNPC);
    }
}
#endif
// PC 0x0071F6C0
inline CSWGuiPartySelectionData::~CSWGuiPartySelectionData()
{
}
// PC 0x0071F730
void CSWGuiPartySelection::HandleInputEvent(int nEvent, int bPressed)
{
    if (bPressed != 0)
    {
        switch (nEvent)
        {
        case 40:
        case 46:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            if ((m_bPlaySound == 0 && m_bForcedNPC == 0) || m_n78 != 0)
            {
                for (int nNPC = 0; nNPC < 12; ++nNPC)
                    m_aPartyMembers[nNPC].m_btnNPC.SetSelected(m_aPartyMembers[nNPC].m_nPartyIndex != -1);
                AcceptParty(1);
            }
            else
            {
                CSWGuiMessageBox* pMessageBox =
                    g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
                pMessageBox->SetAllowCancel(0);
                pMessageBox->SetCallback(0, 0, 0);
                pMessageBox->SetMessage(STRREF_CANNOT_CANCEL);
                m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
            }
            break;
        case GUIEVENT_ACCEPT:
            OnDone(0);
            return;
        }
    }
    CSWGuiPanel::HandleInputEvent(nEvent, bPressed);
}
// PC 0x0071F820
#ifdef _XBOX
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
#else
void CSWGuiPartySelection::OnPanelAdded()
{
    CServerExoApp* pServer = g_pAppManager->GetServerExoApp();
    CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
    CSWParty* pParty = pClient->GetSWParty();
    CSWPartyTable* pPartyTable = pServer->GetPartyTable();
    int nFirstAvailable = -1;
    int bUnescapable = pServer->GetCreatureByGameObjectID(pServer->GetPlayerCreatureId())->GetArea()->m_bUnescapable;
    m_bCanChangeParty = bUnescapable == 0 || m_bPlaySound != 0;
    m_nCannotChangeStrRef = STRREF_CANNOT_CHANGE_PARTY;
    CSWSCreature* pPC = (CSWSCreature*)g_pAppManager->m_pServerExoApp->GetGameObject(
        g_pAppManager->m_pServerExoApp->GetFirstPCObject());
    if (pPC != 0 && pPC->m_bPlayerCreated == 0)
    {
        m_bCanChangeParty = 0;
        m_nCannotChangeStrRef = STRREF_CANNOT_CHANGE_PARTY;
    }
    if (m_bCanChangeParty != 0 && !g_pAppManager->m_pClientExoApp->GetSWParty()->IsPartyTogether())
    {
        m_bCanChangeParty = 0;
        m_nCannotChangeStrRef = STRREF_CANNOT_CHANGE_UNCONSCIOUS;
    }
    if (m_bCanChangeParty != 0 && g_pAppManager->m_pClientExoApp->GetSWParty()->IsPartyInCombat())
    {
        m_bCanChangeParty = 0;
        m_nCannotChangeStrRef = STRREF_CANNOT_CHANGE_COMBAT;
    }
    if (m_bPlaySound != 0)
    {
        m_bCanChangeParty = 1;
        m_nCannotChangeStrRef = STRREF_CANNOT_CHANGE_PARTY;
    }
    if ((m_bForcedNPC != 0 || m_bPlaySound != 0) && m_n78 == 0)
    {
        m_btnBack.m_cText.m_cParams.SetDefaultColor(CGuiInGame::COLOR_NEW_GREEN);
        m_btnBack.m_cBorder.m_cParams.m_vColor = CGuiInGame::DISABLED_TEXT;
    }
    else
    {
        m_btnBack.m_cText.m_cParams.SetDefaultColor(CGuiInGame::COLOR_NEW_TEXT);
        m_btnBack.m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_GREEN;
    }
    if (m_bForcedNPC != 0)
        m_nSelectedCount = 0;
    else
    {
        int nMembers = pPartyTable->m_nNumMembers;
        m_nSelectedCount = nMembers;
    }
    pPartyTable->AddGameInProgress();
    UpdateVisibleCharacters();
    for (int nPuppet = 0; nPuppet < 3; ++nPuppet)
        pPartyTable->SavePuppet(nPuppet, 0);
    for (int nNPC = 0; nNPC < CSWPartyTable::MAX_NPCS; ++nNPC)
    {
        CResRef cPortrait;
        m_aPartyMembers[nNPC].m_btnNPC.SetSelected(0);
        if (pPartyTable->GetIsNPCAvailable(nNPC))
        {
            if (nFirstAvailable == -1)
                nFirstAvailable = nNPC;
            unsigned long oidNPC = pPartyTable->GetNPCObject(nNPC, 0, 1);
            CSWSCreature* pCreature = pServer->GetCreatureByGameObjectID(oidNPC);
            if (pCreature != 0)
            {
                pPartyTable->SaveMember(nNPC, 0);
                cPortrait = pCreature->GetPortraitBasedOnAlignment();
                m_aPartyMembers[nNPC].m_bSpawned = 1;
            }
            else
            {
                oidNPC = pPartyTable->GetNPCObject(nNPC, 1, 1);
                pCreature = pServer->GetCreatureByGameObjectID(oidNPC);
                if (pCreature != 0)
                    cPortrait = pCreature->GetPortraitBasedOnAlignment();
                m_aPartyMembers[nNPC].m_bSpawned = 0;
            }
            m_aPartyMembers[nNPC].m_bSelectable = 1;
            if (pPartyTable->GetIsMember(nNPC))
            {
                unsigned long oidClient = pClient->ServerToClientObjectId(oidNPC);
                m_aPartyMembers[nNPC].m_nPartyIndex = pParty->GetIndex(oidClient);
                if (m_aPartyMembers[nNPC].m_nPartyIndex == -1)
                {
                    CExoString sError;
                    sError.Format("While creating the party select screen, NPC %d's ID is Out of Synch with what's in the Client Side Party Table.", nNPC);
                }
            }
            else
                m_aPartyMembers[nNPC].m_nPartyIndex = -1;
            if (m_bForcedNPC != 0)
            {
                if (m_aPartyMembers[nNPC].m_bForced != 0)
                {
                    m_aPartyMembers[nNPC].m_btnNPC.SetSelected(1);
                    ++m_nSelectedCount;
                    m_aPartyMembers[nNPC].m_bSelectable = 0;
                }
            }
            else if (pPartyTable->GetIsMember(nNPC))
                m_aPartyMembers[nNPC].m_btnNPC.SetSelected(1);
            if (pPartyTable->GetNPCSelectability(nNPC))
                m_aPartyMembers[nNPC].m_lblCharacter.m_cBorder.m_cParams.m_fHilightAlpha = 1.0f;
            else
            {
                m_aPartyMembers[nNPC].m_lblCharacter.m_cBorder.m_cParams.m_fHilightAlpha = 0.25f;
                m_aPartyMembers[nNPC].m_bSelectable = 0;
            }
            if (m_aPartyMembers[nNPC].m_bHidden == 0)
            {
                m_aPartyMembers[nNPC].m_lblCharacter.SetShown(1);
                m_aPartyMembers[nNPC].m_lblNotAvailable.SetShown(0);
            }
            m_aPartyMembers[nNPC].m_lblCharacter.m_cBorder.m_cParams.SetFillImage(cPortrait, 0);
        }
        else
        {
            m_aPartyMembers[nNPC].m_bSelectable = 0;
            m_aPartyMembers[nNPC].m_bSpawned = 1;
            m_aPartyMembers[nNPC].m_nPartyIndex = -1;
            if (m_aPartyMembers[nNPC].m_bHidden == 0)
            {
                m_aPartyMembers[nNPC].m_lblNotAvailable.SetShown(1);
                m_aPartyMembers[nNPC].m_lblCharacter.SetShown(0);
            }
        }
    }
    UpdateCount();
    pPartyTable->RemoveGameInProgress();
    if (nFirstAvailable != -1)
    {
        SetActiveControl(&m_aPartyMembers[nFirstAvailable].m_btnNPC, 0);
        OnEnter(&m_aPartyMembers[nFirstAvailable].m_btnNPC);
    }
    else
        SetActiveControl(&m_btnBack, 0);
    CSWGuiPanel::OnPanelAdded();
    g_pAppManager->GetClientExoApp()->ShowTutorialWindow(
        CClientExoApp::TUTORIAL_PARTY_SELECTION, OBJECT_INVALID, OBJECT_INVALID, 0);
    g_pAppManager->m_pClientExoApp->GetInGameGui()->EnableCharacterChangeButtons(0);
}
#endif
// PC 0x0071FDE0
CSWGuiPartySelection::CSWGuiPartySelection(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    CExoString sName;
    m_bForcedNPC = 0;
#ifdef _XBOX
    StartLoadFromLayout("partyselection");
#else
    {
        CResRef cLayout("partyselect_p");
        StartLoadFromLayout(cLayout);
    }
#endif
    for (int nNPC = 0; nNPC < 12; ++nNPC)
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
#ifdef _XBOX
        m_aPartyMembers[nNPC].m_btnNPC.AddEvent(CSWGuiControl::EVENT_CONFIRM, this,
            (CSWGuiEventHandler)&CSWGuiPartySelection::OnToggled);
#endif
        m_aPartyMembers[nNPC].m_btnNPC.AddEvent(CSWGuiControl::EVENT_ENTER, this,
            (CSWGuiEventHandler)&CSWGuiPartySelection::OnEnter);
        m_aPartyMembers[nNPC].m_btnNPC.AddEvent(CSWGuiControl::EVENT_EXIT, this,
            (CSWGuiEventHandler)&CSWGuiPartySelection::OnExit);
#ifdef _XBOX
        m_aPartyMembers[nNPC].m_btnNPC.m_nIndex = (int)&m_aPartyMembers[nNPC];
#else
        m_aPartyMembers[nNPC].m_btnNPC.SetPartyData(&m_aPartyMembers[nNPC]);
#endif
#ifdef _XBOX
        m_aPartyMembers[nNPC].m_btnNPC.m_cHilite.m_cParams.m_vColor = CGuiInGame::COLOR_YELLOW;
#else
        m_aPartyMembers[nNPC].m_btnNPC.m_cHilite.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_HIGHLIGHT;
#endif
        m_aPartyMembers[nNPC].m_btnNPC.m_cHilite.m_cParams.SetPulsingAlpha(1, 1, 0);
#ifdef _XBOX
        m_aPartyMembers[nNPC].m_btnNPC.m_cToggleBorder.m_cParams.m_vColor = CGuiInGame::COLOR_GREEN;
        m_aPartyMembers[nNPC].m_btnNPC.m_cToggleHilite.m_cParams.m_vColor = CGuiInGame::COLOR_GREEN;
        m_aPartyMembers[nNPC].m_btnNPC.m_cToggleHilite.m_cParams.SetPulsingAlpha(1, 1, 0);
#endif
        m_aPartyMembers[nNPC].m_bForced = 0;
        m_aPartyMembers[nNPC].m_nNPC = nNPC;
        m_aPartyMembers[nNPC].m_pCreature = 0;
        m_aPartyMembers[nNPC].m_bHidden = 0;
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
#ifdef _XBOX
    InitControl(m_aLabels[LABEL_BEVEL_R], "LBL_BEVEL_R", 1);
#endif
    InitControl(m_aLabels[LABEL_BEVEL_M], "LBL_BEVEL_M", 1);
    InitControl(m_btnDone, "BTN_DONE", 1);
#ifndef _XBOX
    InitControl(m_btnBack, "BTN_BACK", 1);
    InitControl(m_btnAccept, "BTN_ACCEPT", 1);
    InitControl(m_alblBar[0], "LBL_BAR1", 1);
    InitControl(m_alblBar[1], "LBL_BAR2", 1);
    InitControl(m_alblBar[2], "LBL_BAR3", 1);
    InitControl(m_alblBar[3], "LBL_BAR4", 1);
    InitControl(m_alblBar[4], "LBL_BAR5", 1);
    InitControl(m_lblNameBack, "LBL_NAMEBACK", 1);
    CAurCamera* pCamera = m_lbl3D.m_cScene.m_pCamera;
    CAurScene* pScene = m_lbl3D.m_cScene.m_pScene;
    if (g_bDo3DGUIs != 0)
    {
        pScene->SpawnRoom("gui3D_room", Vector(), Quaternion(0, 0, 0, 1));
        CAurObject* pLight = m_lbl3D.m_cScene.AddModel("charrec_light", -1);
        if (pLight != 0)
        {
            pCamera->AttachToObject(pLight, "camerahook", Gob::ATTACH_TRANSLATE);
            pCamera->SetViewAngle(22.726f);
        }
        m_lbl3D.SetShown(0);
    }
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
#ifdef _XBOX
    m_btnDone.AddEvent(CSWGuiControl::EVENT_CONFIRM, this,
        (CSWGuiEventHandler)&CSWGuiPartySelection::OnDone);
    m_btnDone.AddEvent(CSWGuiControl::EVENT_ENTER, this,
        (CSWGuiEventHandler)&CSWGuiPartySelection::OnDoneEnter);
#else
    m_btnBack.SetFocusable(0);
    m_btnBack.AddEvent(CSWGuiControl::EVENT_SELECT, this,
        (CSWGuiEventHandler)&CSWGuiPanel::OnBButtonPressed);
    m_btnAccept.SetFocusable(0);
    m_btnAccept.AddEvent(CSWGuiControl::EVENT_SELECT, this,
        (CSWGuiEventHandler)&CSWGuiPartySelection::OnToggled);
    m_pSelectedData = 0;
    m_btnDone.SetFocusable(0);
    m_bCanChangeParty = 1;
#endif
    m_bPlaySound = 0;
#ifndef _XBOX
    m_n78 = 0;
    m_nCannotChangeStrRef = STRREF_CANNOT_CHANGE_PARTY;
#endif
}
