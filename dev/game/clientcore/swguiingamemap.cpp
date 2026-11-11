#include "exobase.h"
#include "aurcamera.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "guiingame.h"
#include "exotlktable.h"
#include "serverexoapp.h"
#include "nwscript.h"
#include "partytables.h"
#include "swparty.h"
#include "nwswaypoint.h"
#include "nwsmodule.h"
#include "swguiingamemap.h"
#include "gameobjectarray.h"
#include "nwsmodule.h"
#include "nwsarea.h"
#include "nwswaypoint.h"
#include "swsareamap.h"
#include "swguiclassselection.h"
#include "nwrules.h"
#include "twodimarrays.h"
#include "twodimarray.h"
#include "aurscene.h"
#include "aurmodel.h"
#include "nwcmodule.h"
#include "nwcarea.h"
#include "clientoptions.h"
#include <float.h>

static inline void SwguiingamemapTrace2();
static inline void SwguiingamemapTrace1();
#define SWGUIINGAMEMAP_TRACE() SwguiingamemapTrace2()

int g_bEnableFogOfWar = 1;
int g_bShowFogOfWar = 1;
// PC 0x006927B0
void CSWGuiInGameMap::OnUpArrowPressed(CSWGuiControl* pControl)
{
    HandleInputEvent(GUIEVENT_UP, 1);
}
// PC 0x006927C0
void CSWGuiInGameMap::OnDownArrowPressed(CSWGuiControl* pControl)
{
    HandleInputEvent(GUIEVENT_DOWN, 1);
}
// PC 0x006927D0  Xbox 0x000DF320
void CSWGuiInGameGalaxyMap::OnPanelRemoved()
{
#ifdef _XBOX
    CSWGuiLabelHilight* pButton = m_aPlanetButtons;
#else
    CSWGuiButton* pButton = m_aPlanetButtons;
#endif
    for (int i = 16; i != 0; i--)
    {
        pButton->SetActive(0);
        pButton++;
    }
    m_nSelectedPlanet = -1;
#ifndef _XBOX
    CSWGuiPanel::OnPanelRemoved();
#endif
}
// PC 0x00692810  Xbox 0x000DE4E0
void CSWGuiInGameMap::Draw(float fElapsed)
{
#ifndef _XBOX
    if (!m_bReturnButton)
    {
        m_btnUp.m_cText.m_cParams.SetColor(CGuiInGame::DISABLED_TEXT);
        m_btnUp.m_cBorder.m_cParams.m_vColor = CGuiInGame::DISABLED_ICON_COLOR;
    }
    if (!m_bPartySelection)
    {
        m_btnDown.m_cText.m_cParams.SetColor(CGuiInGame::DISABLED_TEXT);
        m_btnDown.m_cBorder.m_cParams.m_vColor = CGuiInGame::DISABLED_ICON_COLOR;
    }
#endif

    CSWGuiExtent cExtent = m_lblMapNote.m_cExtent;
    CSWGuiPanel::Draw(fElapsed);
#ifndef _XBOX
    cExtent.m_nLeft += (m_pManager->m_nWidth - 640) / 2;
    cExtent.m_nTop += (m_pManager->m_nHeight - 480) / 2;
#endif
    AurGUIStartLayer();
    if (AurGUISetupViewport(cExtent.m_nLeft, cExtent.m_nTop,
                            cExtent.m_nWidth, cExtent.m_nHeight,
                            CSWGuiDrawable::NO_COLOURING, false, 1.0f) != 0)
    {
        m_imgMap.Draw(fElapsed);
        m_cMapHider.Draw(fElapsed);
        AurGUICloseViewport();
    }
    AurGUIStopLayer();
}

#ifndef _XBOX
CSWGuiControl* CSWGuiInGameMap::HitCheckMouse(int nX, int nY)  // PC 0x00692930
{
    CSWGuiControl* pControl = CSWGuiPanel::HitCheckMouse(nX, nY);
    if (pControl == 0)
    {
#ifndef _XBOX
        if (!m_bPositioned)
        {
            CSWGuiManager* pManager = m_pManager;
            nX = (pManager->m_nWidth - 640) / -2 + nX;
            nY = (pManager->m_nHeight - 480) / -2 + nY;
        }
#endif
        CSWGuiExtent cExtent = m_lblMapNote.m_cExtent;
        return m_cMapHider.HitCheckMouse(nX - cExtent.m_nLeft,
                                         nY - cExtent.m_nTop);
    }
    return pControl;
}
#endif
// PC 0x006929B0  Xbox 0x000DE570
void CSWGuiInGameMap::SetMapNote(const CExoString& sNote)
{
    CExoString sText;
    if (sNote.GetLength() == 0)
    {
        m_lblTitle.m_cText.m_cParams.SetText("");
        return;
    }
    else
    {
        sText = g_pTlkTable->GetSimpleString(STRREF_MAP_NOTE) + ": " + sNote;
        m_lblTitle.m_cText.m_cParams.SetText(sText);
    }
}
// PC 0x00692B00  Xbox 0x000DE6C0
void CSWGuiInGameMap::ReturnToEbonHawk(CSWGuiControl* pControl)
{
    CSWGuiMessageBox* pMessageBox =
        g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
    if (pMessageBox->m_bAccepted)
    {
        if (g_pAppManager->m_pClientExoApp->DismissInGameGUI())
        {
            g_pAppManager->m_pClientExoApp->GetInGameGui()
                ->m_bTransitionScriptRunning = 1;
            CExoString sScript("k_sup_gohawk");
            g_pVirtualMachine->RunScript(&sScript, 0, 0);
            g_pAppManager->m_pClientExoApp->GetInGameGui()
                ->m_bTransitionScriptRunning = 0;
        }
    }
}
// PC 0x00692BC0  Xbox 0x000DE780
void CSWGuiInGameMap::OnXButton(CSWGuiControl* pControl)
{
    CSWGuiMessageBox* pMessageBox;
    if (m_bReturnButton)
    {
        pMessageBox =
            g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
        pMessageBox->SetAllowCancel(1);
        CSWPartyTable* pPartyTable =
            g_pAppManager->m_pServerExoApp->GetPartyTable();
        pMessageBox->SetMessage(pPartyTable->m_nReturnTitleStrref);
        pMessageBox->SetCallback(
            this,
            static_cast<CSWGuiHandler>(&CSWGuiInGameMap::ReturnToEbonHawk), 0);
    }
    else
    {
        if (m_nMessageStrRef == 0)
        {
            m_nMessageStrRef = STRREF_FUNCTION_UNAVAILABLE;
        }
        pMessageBox =
            g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
        pMessageBox->SetAllowCancel(0);
        pMessageBox->SetMessage(m_nMessageStrRef);
        pMessageBox->SetCallback(0, 0, 0);
    }
    m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
}
// PC 0x00693650  Xbox 0x000DF550
void CSWGuiInGameMap::OnPanelAdded()
{
    CServerExoApp* pServer = g_pAppManager->GetServerExoApp();
    CSWCArea* pClientArea = g_pAppManager->GetClientExoApp()->GetModule()->m_pInfo;
    CExoString sAreaName = pClientArea->m_sAreaName;
    m_lblArea.m_cText.m_cParams.SetText(sAreaName);

    CResRef cAreaResRef = pServer->GetModule()->m_cAreaResRef;
    CExoString sAreaResRef;
    CExoString sMapImage;
    cAreaResRef.CopyToString(sAreaResRef);
    sMapImage.Format("lbl_map%s", sAreaResRef.CStr());

    int nMapMode;
    if (pServer->GetModule()->GetAreaMap()->m_nExploredSize > 0)
    {
        CResRef cMapImage(sMapImage);
        m_imgMap.m_cParams.SetImage(cMapImage, 0);
        nMapMode = 1;
    }
    else
    {
        CResRef cMapImage("lbl_mapm28aa");
        m_imgMap.m_cParams.SetImage(cMapImage, 0);
        nMapMode = 0;
    }
    m_imgMap.m_cParams.SetDrawStyle(SWGUI_IMAGE_DRAW_STYLE_NORMAL);
    SetMapNote(m_cMapHider.InitializeMapNotes(nMapMode));

    CSWSCreature* pPlayer =
        pServer->GetPlayerCreature();
    int bUnescapable = pPlayer->GetArea()->m_bUnescapable;
    if (bUnescapable)
    {
        m_nMessageStrRef = STRREF_FUNCTION_UNAVAILABLE;
        m_bPartySelection = 0;
    }
    else
    {
        m_nMessageStrRef = 0;
        m_bPartySelection = 1;
    }

    if (m_bPartySelection)
    {
        if (!g_pAppManager->GetClientExoApp()->GetSWParty()->IsPartyTogether())
        {
            m_nMessageStrRef = STRREF_REJOIN_PARTY;
            m_bPartySelection = 0;
        }
    }

    if (m_bPartySelection)
    {
        float fMinX = FLT_MAX;
        unsigned long oidPlayer =
            g_pAppManager->GetClientExoApp()->GetPlayerCreatureId();
        oidPlayer = g_pAppManager->GetServerExoApp()->ClientToServerObjectId(oidPlayer);
        CSWSCreature* pPlayerCreature =
            g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidPlayer);
        int i;
        for (i = 0; i < g_pAppManager->GetClientExoApp()->GetSWParty()->GetNumCharacters(); i++)
        {
#ifdef _XBOX
            CSWParty* pParty = g_pAppManager->GetClientExoApp()->GetSWParty();
            EXOXBOXASSERT(i >= 0 && i < CSWParty::MAX_MEMBERS);
            unsigned long oidMember = pParty->m_aPartyMemberData[i].m_oidCreature;
#else
            unsigned long oidMember = g_pAppManager->GetClientExoApp()->GetSWParty()
                                          ->m_aPartyMemberData[i].m_oidCreature;
#endif
            oidMember = g_pAppManager->GetServerExoApp()->ClientToServerObjectId(oidMember);
            CSWSCreature* pMember =
                g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidMember);
            if (pMember != NULL &&
                pMember->m_vPosition.m_fX - pMember->GetSpotCheckDistance() < fMinX)
            {
                fMinX = pMember->m_vPosition.m_fX - pMember->GetSpotCheckDistance();
            }
        }

        CSWSArea* pArea = pPlayerCreature->GetArea();
        int nIndex;
        pArea->GetFirstObjectIndiceByX(&nIndex, fMinX);
        for (; m_bPartySelection && nIndex < pArea->m_lstObjectIds.num; nIndex++)
        {
            CSWSCreature* pCreature = g_pAppManager->GetServerExoApp()
                ->GetCreatureByGameObjectID(pArea->m_lstObjectIds.element[nIndex]);
            if (pCreature != NULL)
            {
                for (i = 0; i < g_pAppManager->GetClientExoApp()->GetSWParty()->GetNumCharacters(); i++)
                {
#ifdef _XBOX
                    CSWParty* pParty = g_pAppManager->GetClientExoApp()->GetSWParty();
                    EXOXBOXASSERT(i >= 0 && i < CSWParty::MAX_MEMBERS);
                    unsigned long oidMember = pParty->m_aPartyMemberData[i].m_oidCreature;
#else
                    unsigned long oidMember = g_pAppManager->GetClientExoApp()->GetSWParty()
                                                  ->m_aPartyMemberData[i].m_oidCreature;
#endif
                    oidMember = g_pAppManager->GetServerExoApp()->ClientToServerObjectId(oidMember);
                    CSWSCreature* pMember =
                        g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidMember);
                    if (pMember != NULL &&
                        pMember->GetReputation(pCreature->GetId(), 0) <=
                            CSWSObject::MAX_HOSTILE_REPUTATION)
                    {
                        CSWVisibilityNode* pNode =
                            pCreature->GetVisibleListElement(pMember->GetId());
                        if (pNode != NULL && pNode->m_bSeen)
                        {
                            m_bPartySelection = 0;
                            m_nMessageStrRef = STRREF_ENEMIES_NEARBY;
                        }
                    }
                }
            }
        }
    }

    m_bReturnButton =
        m_bPartySelection && pServer->GetPartyTable()->m_nReturnFlag != 0;

    unsigned long nReturnStrref = pServer->GetPartyTable()->m_nReturnStrref;
#ifdef _XBOX
    m_lblReturn.m_cText.m_cParams.SetStrRef(nReturnStrref);
    m_lblReturn.m_cText.m_cParams.SetColor(
        m_bPartySelection ? CGuiInGame::COLOR_BLUE : CGuiInGame::DISABLED_TEXT);
    m_lblPrtySlct.m_cText.m_cParams.SetColor(
        m_bReturnButton ? CGuiInGame::COLOR_BLUE : CGuiInGame::DISABLED_TEXT);
#else
    m_btnUp.m_cText.m_cParams.SetStrRef(nReturnStrref);
    m_btnUp.m_cText.m_cParams.SetColor(
        m_bReturnButton ? CGuiInGame::COLOR_BLUE : CGuiInGame::DISABLED_TEXT);
    m_btnUp.m_cBorder.m_cParams.m_vColor =
        m_bReturnButton ? CGuiInGame::ENABLED_ICON_COLOR : CGuiInGame::DISABLED_ICON_COLOR;
    m_btnDown.m_cText.m_cParams.SetColor(
        m_bPartySelection ? CGuiInGame::COLOR_BLUE : CGuiInGame::DISABLED_TEXT);
    m_btnDown.m_cBorder.m_cParams.m_vColor =
        m_bPartySelection ? CGuiInGame::ENABLED_ICON_COLOR : CGuiInGame::DISABLED_ICON_COLOR;

    if (g_pAppManager->GetClientExoApp()->GetClientOptions()->m_bUseSmallFonts)
    {
        m_btnDown.m_cText.m_cParams.SetAlignment(CSWGuiTextParams::ALIGNMENT_CENTER);
        m_btnUp.m_cText.m_cParams.SetAlignment(CSWGuiTextParams::ALIGNMENT_CENTER);
    }
#endif
#ifndef _XBOX
    CSWGuiPanel::OnPanelAdded();
#endif
    g_pAppManager->GetClientExoApp()->ShowTutorialWindow(
        CClientExoApp::TUTORIAL_MAP, OBJECT_INVALID, OBJECT_INVALID, 0);
}
// PC 0x00692C70  Xbox 0x000DED50
CExoString CSWGuiMapHider::InitializeMapNotes(int nMapMode)
{
    m_nMapMode = nMapMode;
    CExoString sMapNote("");
    CGameObjectArray* pObjectArray =
        g_pAppManager->m_pServerExoApp->GetObjectArray();
    CSWSArea* pArea = g_pAppManager->m_pServerExoApp->GetModule()->GetArea();
    for (int i = 0; i < pArea->m_lstObjectIds.num; i++)
    {
        unsigned long oidObject = pArea->m_lstObjectIds.element[i];
        CGameObject* pObject;
        if (pObjectArray->GetGameObject(oidObject, &pObject) ==
            CGameObjectArray::SUCCESS)
        {
            CSWSWaypoint* pWaypoint = pObject->AsSWSWaypoint();
            if (pWaypoint != 0 && pWaypoint->m_bHasMapNote)
            {
                unsigned long* poidNote = new unsigned long;
                *poidNote = oidObject;
                m_lstMapNotes.m_pInternal->AddHead(poidNote);
                if (m_pCurrentNote == 0 && pWaypoint->m_bMapNoteEnabled)
                {
                    CSWSAreaMap* pAreaMap =
                        g_pAppManager->m_pServerExoApp->GetModule()->m_pAreaMap;
                    int bExplored = pAreaMap->IsWorldPointExplored(pWaypoint->m_vPosition);
                    if (bExplored)
                    {
                        m_pCurrentNote = m_lstMapNotes.m_pInternal->m_pHead;
                        CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
                        pWaypoint->m_lsMapNote.GetString(
                            pClient->GetClientLanguage(),
                            &sMapNote, 0);
                    }
                }
            }
        }
    }
    return sMapNote;
}
// PC 0x00693BA0  Xbox 0x000DFA60
void CSWGuiInGameMap::OnPanelRemoved()
{
    m_cMapHider.ClearMapNotes();
#ifndef _XBOX
    CSWGuiPanel::OnPanelRemoved();
#endif
}
// PC 0x00692E30  Xbox 0x000DEF00
void CSWGuiMapHider::ClearMapNotes()
{
    while (m_lstMapNotes.m_pInternal->m_pHead != 0)
    {
        delete (unsigned long*)m_lstMapNotes.m_pInternal->GetHead();
        m_lstMapNotes.m_pInternal->RemoveHead();
    }
    m_pCurrentNote = 0;
}
// PC 0x00692E80  Xbox 0x000DEF40
CExoString CSWGuiMapHider::GetNextMapNote()
{
    CExoString sNote("");
    CExoLinkedListNode* pPos = m_pCurrentNote;
    unsigned long oidNote = OBJECT_INVALID;
    if (pPos != 0)
    {
        oidNote = *(unsigned long*)m_lstMapNotes.m_pInternal->GetAtPos(pPos);
    }
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    CGameObject* pObject = pServer->GetGameObject(oidNote);
    if (pObject != 0 && pObject->AsSWSWaypoint() != 0)
    {
        CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
        CSWSWaypoint* pWaypoint = pObject->AsSWSWaypoint();
        pWaypoint->m_lsMapNote.GetString(
            pClient->GetClientLanguage(), &sNote, 0);
    }
    if (pPos != 0)
    {
        m_lstMapNotes.m_pInternal->GetNext(pPos);
    }
    if (pPos == 0)
    {
        pPos = m_lstMapNotes.m_pInternal->m_pHead;
    }
    while (pPos != 0 && m_pCurrentNote != 0 && pPos != m_pCurrentNote)
    {
        oidNote = *(unsigned long*)m_lstMapNotes.m_pInternal->GetAtPos(pPos);
        pServer = g_pAppManager->m_pServerExoApp;
        pObject = pServer->GetGameObject(oidNote);
        if (pObject != 0 && pObject->AsSWSWaypoint() != 0)
        {
            CSWSWaypoint* pWaypoint = pObject->AsSWSWaypoint();
            if (pWaypoint->m_bMapNoteEnabled != 0)
            {
                CSWSAreaMap* pAreaMap =
                    g_pAppManager->m_pServerExoApp->GetModule()->m_pAreaMap;
                int bExplored = pAreaMap->IsWorldPointExplored(pWaypoint->m_vPosition);
                if (bExplored)
                {
                    m_pCurrentNote = pPos;
                    CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
                    pWaypoint->m_lsMapNote.GetString(
                        pClient->GetClientLanguage(), &sNote, 0);
                    break;
                }
            }
        }
        m_lstMapNotes.m_pInternal->GetNext(pPos);
        if (pPos == 0)
        {
            pPos = m_lstMapNotes.m_pInternal->m_pHead;
        }
    }
    return sNote;
}
// PC 0x00693BC0  Xbox 0x000DFA70
void CSWGuiInGameMap::HandleInputEvent(int nEvent, int nState)
{
    if (nState)
    {
        switch (nEvent)
        {
        case GUIEVENT_TOGGLE_MODE:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            if (!g_pAppManager->m_pClientExoApp->ShowTutorialWindow(
                    CClientExoApp::TUTORIAL_RETURN_TO_EBON_HAWK,
                    OBJECT_INVALID, OBJECT_INVALID, 0))
            {
                OnXButton(0);
            }
            break;
        case GUIEVENT_ACTIVATE:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            if (m_bPartySelection)
            {
                CExoString sExitScript("");
                g_pAppManager->GetClientExoApp()->GetInGameGui()
                    ->ShowPartySelection(sExitScript, 0, -1, -1);
            }
            else
            {
                CSWGuiMessageBox* pMessageBox =
                    g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
                pMessageBox->SetAllowCancel(0);
                pMessageBox->SetMessage(m_nMessageStrRef);
                pMessageBox->SetCallback(0, 0, 0);
                m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
            }
            break;
        case GUIEVENT_CANCEL:
        case GUIEVENT_ACCEPT:
        case GUIEVENT_CANCEL_ALTERNATE:
#ifndef _XBOX
        case GUIEVENT_EXIT:
#endif
#ifdef _XBOX
            if (g_pAppManager->m_pClientExoApp->GetInGameGui()->HideSWInGameGui())
#else
            if (g_pAppManager->m_pClientExoApp->GetInGameGui()->HideSWInGameGui(0))
#endif
            {
#ifdef _XBOX
                g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME);
#else
                g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME, 1);
#endif
            }
            break;
        case GUIEVENT_UP:
        case GUIEVENT_UP_ALTERNATE:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
            SetMapNote(m_cMapHider.GetPrevMapNote());
            break;
        case GUIEVENT_DOWN:
        case GUIEVENT_DOWN_ALTERNATE:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
            SetMapNote(m_cMapHider.GetNextMapNote());
            break;
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
// PC 0x00693090  Xbox 0x000DF130
CExoString CSWGuiMapHider::GetPrevMapNote()
{
    CExoString sNote("");
    CExoLinkedListNode* pPos = m_pCurrentNote;
    unsigned long oidNote = OBJECT_INVALID;
    if (pPos != 0)
    {
        oidNote = *(unsigned long*)m_lstMapNotes.m_pInternal->GetAtPos(pPos);
    }
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    CGameObject* pObject = pServer->GetGameObject(oidNote);
    if (pObject != 0 && pObject->AsSWSWaypoint() != 0)
    {
        CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
        CSWSWaypoint* pWaypoint = pObject->AsSWSWaypoint();
        pWaypoint->m_lsMapNote.GetString(pClientExoApp->GetClientLanguage(),
                                         &sNote, 0);
    }
    if (pPos != 0)
    {
        m_lstMapNotes.m_pInternal->GetPrev(pPos);
    }
    if (pPos == 0)
    {
        pPos = m_lstMapNotes.m_pInternal->m_pTail;
    }
    while (pPos != 0 && m_pCurrentNote != 0 && pPos != m_pCurrentNote)
    {
        oidNote = *(unsigned long*)m_lstMapNotes.m_pInternal->GetAtPos(pPos);
        pServer = g_pAppManager->m_pServerExoApp;
        pObject = pServer->GetGameObject(oidNote);
        if (pObject != 0 && pObject->AsSWSWaypoint() != 0)
        {
            CSWSWaypoint* pWaypoint = pObject->AsSWSWaypoint();
            if (pWaypoint->m_bMapNoteEnabled)
            {
                CSWSAreaMap* pAreaMap =
                    g_pAppManager->m_pServerExoApp->GetModule()->m_pAreaMap;
                if (pAreaMap->IsWorldPointExplored(pWaypoint->m_vPosition))
                {
                    m_pCurrentNote = pPos;
                    pWaypoint->m_lsMapNote.GetString(
                        g_pAppManager->GetClientExoApp()->GetClientLanguage(),
                        &sNote, 0);
                    break;
                }
            }
        }
        m_lstMapNotes.m_pInternal->GetPrev(pPos);
        if (pPos == 0)
        {
            pPos = m_lstMapNotes.m_pInternal->m_pTail;
        }
    }
    return sNote;
}

#ifndef _XBOX
void CSWGuiInGameMap::OnMapNoteClicked(CSWGuiControl* pControl)  // PC 0x00693E70
{
    if (pControl->m_bSelected)
    {
        CExoString sNote;
        unsigned long oidNote = pControl->m_nIndex;
        CGameObject* pObject = g_pAppManager->m_pServerExoApp->GetGameObject(oidNote);
        if (pObject != 0 && pObject->AsSWSWaypoint() != 0)
        {
            CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
            CSWSWaypoint* pWaypoint = pObject->AsSWSWaypoint();
            pWaypoint->m_lsMapNote.GetString(pClient->GetClientLanguage(), &sNote, 0);
        }
        SetMapNote(sNote);
        m_cMapHider.SetMapNote(oidNote);
        CSWGuiInGameMap::Draw(0.001f);
        int nMouseY = m_pManager->m_nMouseY;
        m_pManager->HandleMouseMove(m_pManager->m_nMouseX, nMouseY);
    }
}
#endif
// PC 0x006932A0
void CSWGuiMapHider::SetMapNote(unsigned long oidNote)
{
    CExoLinkedListNode* pPos = m_lstMapNotes.m_pInternal->m_pHead;
    while (pPos != 0)
    {
        unsigned long* pNote =
            (unsigned long*)m_lstMapNotes.m_pInternal->GetAtPos(pPos);
        if (oidNote == *pNote)
        {
            m_pCurrentNote = pPos;
            return;
        }
        m_lstMapNotes.m_pInternal->GetNext(pPos);
    }
}

#ifndef _XBOX
CSWGuiControl* CSWGuiMapHider::HitCheckMouse(int nX, int nY)  // PC 0x00693300
{
    if (nX < m_cExtent.m_nLeft ||
        nX > m_cExtent.GetRight() ||
        nY < m_cExtent.m_nTop ||
        nY > m_cExtent.GetBottom())
    {
        return 0;
    }

    int nLocalX = nX - m_cExtent.m_nLeft;
    int nLocalY = nY - m_cExtent.m_nTop;
    if (m_btnHider.HitCheckMouse(nLocalX, nLocalY) != 0)
    {
        return &m_btnHider;
    }
    for (int i = 0; i < m_aControls.num; i++)
    {
        if (m_aControls.element[i]->HitCheckMouse(nLocalX, nLocalY) != 0)
        {
            return m_aControls.element[i];
        }
    }
    return 0;
}
#endif
// PC 0x00693F60  Xbox 0x000DFC80
CSWGuiMapHider::CSWGuiMapHider()
{
    ClearMapNotes();
    m_nMapMode = 0;
#ifdef _XBOX
    m_pBlackDot = 0;
#endif
    m_pBlackDot = NewCAurGUIImage("blackdot");
    m_pArrow = new CSWGuiImage;
    m_pCircle = new CSWGuiImage;
    if (m_pArrow != 0)
    {
        m_pArrow->Initialize(CSWGuiExtent(0, 0, 32, 32),
            CSWGuiImageParams("mm_barrow", 0, 0, 0.0f,
                              CSWGuiImageParams::ALIGNMENT_DEFAULT, 1.0f,
                              Vector(1.0f, 1.0f, 1.0f)));
        CSWGuiImageParams* pParams = &m_pArrow->m_cParams;
        pParams->m_bPulsing = 1;
    }
    if (m_pCircle != 0)
    {
        m_pCircle->Initialize(CSWGuiExtent(0, 0, 16, 16),
            CSWGuiImageParams("lbl_mapcircle", 0, 0, 0.0f,
                              CSWGuiImageParams::ALIGNMENT_DEFAULT, 1.0f,
                              Vector(1.0f, 1.0f, 1.0f)));
        CSWGuiImageParams* pParams = &m_pCircle->m_cParams;
        pParams->m_vColor = CGuiInGame::COLOR_ORANGE;
    }
#ifdef _XBOX
    m_pCurrentNoteImage = new CSWGuiImage;
    m_pNoteImage = new CSWGuiImage;
    if (m_pCurrentNoteImage != 0)
    {
        m_pCurrentNoteImage->Initialize(CSWGuiExtent(0, 0, 20, 20),
            CSWGuiImageParams("whitetarget", 0, 0, 0.0f,
                              CSWGuiImageParams::ALIGNMENT_DEFAULT, 1.0f,
                              Vector(1.0f, 1.0f, 1.0f)));
        CSWGuiImageParams* pParams = &m_pCurrentNoteImage->m_cParams;
        pParams->m_vColor = CGuiInGame::COLOR_YELLOW;
    }
    if (m_pNoteImage != 0)
    {
        m_pNoteImage->Initialize(CSWGuiExtent(0, 0, 14, 14),
            CSWGuiImageParams("whitetarget", 0, 0, 0.0f,
                              CSWGuiImageParams::ALIGNMENT_DEFAULT, 1.0f,
                              Vector(1.0f, 1.0f, 1.0f)));
        CSWGuiImageParams* pParams = &m_pNoteImage->m_cParams;
        pParams->m_vColor = CGuiInGame::COLOR_BLUE;
    }
#else
    m_btnHider.Initialize(CSWGuiExtent(0, 0, 20, 20), CSWGuiTextParams(),
        CSWGuiBorderParams(CResRef(), CResRef(), "whitetarget",
                           2, 0, 1.0f, Vector(1.0f, 1.0f, 1.0f), 0),
        CSWGuiBorderParams(CResRef(), CResRef(), "whitetarget",
                           2, 0, 1.0f, Vector(1.0f, 1.0f, 1.0f), 0));
    m_btnHider.m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_YELLOW;
    m_btnHider.m_cHilite.m_cParams.m_vColor = CGuiInGame::COLOR_YELLOW;
#endif
}
// PC 0x006942D0  Xbox 0x000DFFC0
CSWGuiMapHider::~CSWGuiMapHider()
{
    ClearMapNotes();
    if (m_pBlackDot != 0)
    {
        delete m_pBlackDot;
        m_pBlackDot = 0;
    }
    if (m_pArrow != 0)
    {
        delete m_pArrow;
        m_pArrow = 0;
    }
    if (m_pCircle != 0)
    {
        delete m_pCircle;
        m_pCircle = 0;
    }
#ifdef _XBOX
    if (m_pCurrentNoteImage != 0)
    {
        delete m_pCurrentNoteImage;
        m_pCurrentNoteImage = 0;
    }
    if (m_pNoteImage != 0)
    {
        delete m_pNoteImage;
        m_pNoteImage = 0;
    }
#else
    for (int i = 0; i < m_aControls.num; i++)
    {
        delete m_aControls.element[i];
    }
#endif
}
// PC 0x006943D0  Xbox 0x000DE830
void CSWGuiMapHider::Draw(float fElapsed)
{
    if (m_nMapMode == 0)
    {
        return;
    }
    if (AurGUISetupViewport(m_cExtent.m_nLeft, m_cExtent.m_nTop,
                            m_cExtent.m_nWidth, m_cExtent.m_nHeight,
                            Vector(-1.0f, -1.0f, -1.0f), false, 1.0f) == 0)
    {
        return;
    }

    CSWSAreaMap* pAreaMap =
        g_pAppManager->GetServerExoApp()->GetModule()->GetAreaMap();
    if (pAreaMap != NULL && pAreaMap->m_nExploredSize > 0)
    {
        unsigned long nMapSize;
        int nMapWidth;
        int nMapHeight;
        unsigned long* pMapData =
            pAreaMap->GetMapData(nMapSize, nMapWidth, nMapHeight);
        if (pMapData != NULL && g_bShowFogOfWar)
        {
            float fScaleX = (float)CSWSAreaMap::MAP_PIXEL_WIDTH / nMapWidth;
            float fScaleY = (float)CSWSAreaMap::MAP_PIXEL_HEIGHT / nMapHeight;
            float fDotWidth = fScaleX * 4.0f;
            float fDotHeight = fScaleY * 4.0f;
            float fX = 0.0f;
            float fY = 0.0f;
            int nPixels = (nMapWidth + 1) * (nMapHeight + 1);
            for (int nPixel = 0; nPixel < nPixels; )
            {
                int nBit = nPixel % CSWSAreaMap::EXPLORED_BITS_PER_WORD;
                int nWord = nPixel / CSWSAreaMap::EXPLORED_BITS_PER_WORD;
                if (nWord > (int)nMapSize)
                {
                    break;
                }
                int bExplored = pMapData[nWord] & (1 << nBit);
                if (!bExplored && g_bEnableFogOfWar)
                {
                    m_pBlackDot->DrawBuffered(
                        (fX - fDotWidth * 0.5f) / m_cExtent.m_nWidth,
                        (fY - fDotHeight * 0.5f) / m_cExtent.m_nHeight,
                        fDotWidth / m_cExtent.m_nWidth,
                        fDotHeight / m_cExtent.m_nHeight,
                        0, 0.0f, Vector(-1.0f, -1.0f, -1.0f), 1.0f);
                }
                nPixel++;
                if (nPixel % (nMapWidth + 1) != 0)
                {
                    fX += fScaleX;
                }
                else
                {
                    fX = 0.0f;
                    fY += fScaleY;
                }
            }
        }
        CAurGUIImage::FlushBuffer(CAurGUIImage::cm_fAlpha);

        CSWGuiExtent cExtent;
        int nMapX = 0;
        int nMapY = 0;
        int nButton = 0;
        CExoLinkedListNode* pPos = m_lstMapNotes.m_pInternal->m_pHead;
        while (pPos != NULL)
        {
            unsigned long oidNote = *(unsigned long*)m_lstMapNotes.m_pInternal->GetAtPos(pPos);
            CGameObject* pObject =
                g_pAppManager->GetServerExoApp()->GetGameObject(oidNote);
            if (pObject != NULL && pObject->AsSWSWaypoint() != NULL)
            {
                CSWSWaypoint* pWaypoint = pObject->AsSWSWaypoint();
                if (pWaypoint->m_bMapNoteEnabled &&
                    g_pAppManager->GetServerExoApp()->GetModule()->GetAreaMap()
                        ->IsWorldPointExplored(pWaypoint->m_vPosition))
                {
                    int nNoteX, nNoteY = 0;
                    if (g_pAppManager->GetServerExoApp()->GetModule()->GetAreaMap()
                            ->GetMapPixelFromWorldCoord(pWaypoint->m_vPosition,
                                                        nNoteX, nNoteY))
                    {
#ifdef _XBOX
                        if (pPos == m_pCurrentNote)
                        {
                            cExtent.m_nLeft = nNoteX - 10;
                            cExtent.m_nTop = nNoteY - 10;
                            cExtent.m_nWidth = 20;
                            cExtent.m_nHeight = 20;
                            m_pCurrentNoteImage->SetExtent(cExtent);
                            m_pCurrentNoteImage->Draw(fElapsed);
                        }
                        else
                        {
                            cExtent.m_nLeft = nNoteX - 7;
                            cExtent.m_nTop = nNoteY - 7;
                            cExtent.m_nWidth = 14;
                            cExtent.m_nHeight = 14;
                            m_pNoteImage->SetExtent(cExtent);
                            m_pNoteImage->Draw(fElapsed);
                        }
#else
                        if (pPos == m_pCurrentNote)
                        {
                            cExtent.m_nLeft = nNoteX - 10;
                            cExtent.m_nTop = nNoteY - 10;
                            cExtent.m_nWidth = 20;
                            cExtent.m_nHeight = 20;
                            m_btnHider.m_nIndex = oidNote;
                            m_btnHider.SetExtent(cExtent);
                            m_btnHider.Draw(fElapsed);
                        }
                        else
                        {
                            cExtent.m_nLeft = nNoteX - 7;
                            cExtent.m_nTop = nNoteY - 7;
                            cExtent.m_nWidth = 14;
                            cExtent.m_nHeight = 14;
                            if (m_aControls.num == nButton)
                            {
                                m_aControls.Add(new CSWGuiButton);
                                m_aControls[nButton]->Initialize(
                                    CSWGuiExtent(0, 0, 14, 14), CSWGuiTextParams(),
                                    CSWGuiBorderParams(CResRef(), CResRef(),
                                                       "whitetarget", 2, 0, 1.0f,
                                                       Vector(1.0f, 1.0f, 1.0f), 0),
                                    CSWGuiBorderParams(CResRef(), CResRef(),
                                                       "whitetarget", 2, 0, 1.0f,
                                                       Vector(1.0f, 1.0f, 1.0f), 0));
                                m_aControls[nButton]->m_cBorder.m_cParams.m_vColor =
                                    CGuiInGame::COLOR_BLUE;
                                m_aControls[nButton]->m_cHilite.m_cParams.m_vColor =
                                    CGuiInGame::COLOR_YELLOW;
                                m_aControls[nButton]->m_bFocusable = 0;
#ifndef _XBOX
                                m_aControls[nButton]->AddEvent(
                                    CSWGuiControl::EVENT_SELECT, m_pOwner,
                                    (CSWGuiHandler)&CSWGuiInGameMap::OnMapNoteClicked);
#endif
                            }
                            m_aControls[nButton]->m_nIndex = oidNote;
                            m_aControls[nButton]->SetExtent(cExtent);
                            m_aControls[nButton]->Draw(fElapsed);
                            nButton++;
                        }
#endif
                    }
                }
            }
            m_lstMapNotes.m_pInternal->GetNext(pPos);
        }
#ifndef _XBOX
        if (nButton < m_aControls.num)
        {
            m_aControls.num = nButton;
        }
#endif

        CSWParty* pParty = g_pAppManager->GetClientExoApp()->GetSWParty();
        unsigned char nCharacters =
            pParty != NULL ? pParty->m_nCharacters : CSWParty::MAX_MEMBERS;
        for (int nMember = 1; nMember < nCharacters; nMember++)
        {
            nMapX = 0;
            nMapY = 0;
            if (pAreaMap->GetPartyMemberMapLocation(nMember, nMapX, nMapY) &&
                m_pCircle != NULL)
            {
                cExtent.m_nLeft = nMapX - 8;
                cExtent.m_nTop = nMapY - 8;
                cExtent.m_nWidth = 16;
                cExtent.m_nHeight = 16;
                m_pCircle->SetExtent(cExtent);
                m_pCircle->Draw(fElapsed);
            }
        }

        nMapX = 0;
        nMapY = 0;
        if (m_pArrow != NULL &&
            pAreaMap->GetPartyMemberMapLocation(0, nMapX, nMapY))
        {
            cExtent.m_nLeft = nMapX - 16;
            cExtent.m_nTop = nMapY - 16;
            cExtent.m_nWidth = 32;
            cExtent.m_nHeight = 32;
            if (AurGUISetupViewport(cExtent.m_nLeft, cExtent.m_nTop,
                                    cExtent.m_nWidth, cExtent.m_nHeight,
                                    CSWGuiDrawable::NO_COLOURING, false, 1.0f))
            {
                CSWGuiImageParams* pParams = &m_pArrow->m_cParams;
                float fRotation = pAreaMap->GetPartyMemberMapOrientation(0);
                pParams->m_fRotation = fRotation;
                m_pArrow->Draw(fElapsed);
                AurGUICloseViewport();
            }
        }
    }
    AurGUICloseViewport();
}
// PC 0x00695180  Xbox 0x000E0700
CSWGuiInGameGalaxyMap::CSWGuiInGameGalaxyMap(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    SWGUIINGAMEMAP_TRACE();
    CExoString sIcon;
    CExoString sModel;
    CExoString sTag;
    int nName;
    int nDescription;

    m_nSelectedPlanet = -1;
    StartLoadFromLayout("galaxymap");
    InitControl(m_lblPlanetName, "LBL_DESC", 1);
    InitControl(m_lblPlanetDesc, "LBL_PLANETNAME", 1);
    InitControl(m_cPlanetBackView, "3D_PlanetModel", 1);
    InitControl(m_cPlanetView, "3D_PlanetDisplay", 1);
#ifdef _XBOX
    InitControl(m_lblA, "LBL_A", 1);
    InitControl(m_lblB, "LBL_B", 1);
    InitControl(m_lblAccept, "LBL_ACCEPT", 1);
    InitControl(m_lblBack, "LBL_BACK", 1);
#else
    InitControl(m_btnNext, "BTN_ACCEPT", 1);
    InitControl(m_btnPrev, "BTN_BACK", 1);
#endif
    for (int i = 0; i < PLANET_COUNT; i++)
    {
        int bFound;
        {
            CExoString sColumn("GUITag");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            EXOXBOXASSERT(p2DArrays->m_pJournal);
            C2DA* pPlanetary = p2DArrays->m_pJournal;
            bFound = pPlanetary->GetCExoStringEntry(i, sColumn, &sTag);
        }
        if (bFound)
        {
            InitControl(m_aPlanetButtons[i], sTag, 1);
#ifndef _XBOX
            m_aPlanetButtons[i].m_bFocusable = 0;
            m_aPlanetButtons[i].AddEvent(
                CSWGuiControl::EVENT_SELECT, this,
                (CSWGuiHandler)&CSWGuiInGameGalaxyMap::OnPlanetClicked);
            m_aPlanetButtons[i].m_nIndex = i;
#endif
        }
    }
    StopLoadFromLayout();
#ifndef _XBOX
    m_btnPrev.SetFocusable(0);
    m_btnPrev.AddEvent(CSWGuiControl::EVENT_SELECT, this,
                       (CSWGuiHandler)&CSWGuiPanel::OnBButtonPressed);
    m_btnNext.SetFocusable(0);
    m_btnNext.AddEvent(CSWGuiControl::EVENT_SELECT, this,
                       (CSWGuiHandler)&CSWGuiPanel::OnAButtonPressed);
#endif

    CAurCamera* pCamera = m_cPlanetView.m_cScene.m_pCamera;
    CAurScene* pScene = m_cPlanetView.m_cScene.m_pScene;
    if (g_bDo3DGUIs != 0)
    {
        pScene->SpawnRoom("gui3D_room", Vector(), Quaternion(0.0f, 0.0f, 0.0f, 1.0f));
        CAurObject* pModel = m_cPlanetView.m_cScene.AddModel("galaxy", -1);
        if (pModel != 0)
        {
            ((Gob*)pModel)->PlayAnimation("default", 1.0f, 0, 0);
            pCamera->AttachToObject(pModel, "camerahook", Gob::ATTACH_NORMAL);
            pCamera->SetViewAngle(22.726f);
        }
    }
    pScene = m_cPlanetBackView.m_cScene.m_pScene;
    pScene->SpawnRoom("gui3D_room", Vector(), Quaternion(0.0f, 0.0f, 0.0f, 1.0f));

    for (i = 0; i < PLANET_COUNT; i++)
    {
        {
            CExoString sColumn("Icon");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            EXOXBOXASSERT(p2DArrays->m_pJournal);
            C2DA* pPlanetary = p2DArrays->m_pJournal;
            pPlanetary->GetCExoStringEntry(i, sColumn, &sIcon);
        }
        {
            CExoString sColumn("Model");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            EXOXBOXASSERT(p2DArrays->m_pJournal);
            C2DA* pPlanetary = p2DArrays->m_pJournal;
            pPlanetary->GetCExoStringEntry(i, sColumn, &sModel);
        }
        {
            CExoString sColumn("Name");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            EXOXBOXASSERT(p2DArrays->m_pJournal);
            C2DA* pPlanetary = p2DArrays->m_pJournal;
            pPlanetary->GetINTEntry(i, sColumn, &nName);
        }
        {
            CExoString sColumn("Description");
            CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
            EXOXBOXASSERT(p2DArrays->m_pJournal);
            C2DA* pPlanetary = p2DArrays->m_pJournal;
            pPlanetary->GetINTEntry(i, sColumn, &nDescription);
        }
        m_aPlanetButtons[i].m_cBorder.m_cParams.SetFillImage(sIcon, 0);
        m_aPlanetButtons[i].m_cHilite.m_cParams.SetFillImage(sIcon, 0);
        m_aPlanetButtons[i].SetActive(0);
        m_aPlanets[i].m_nNameStrRef = nName;
        m_aPlanets[i].m_nDescStrRef = nDescription;
        m_aPlanets[i].m_resModel = sModel;
    }
#ifndef _XBOX
    m_nBackground = 3;
#endif
}
// PC 0x00694B40  Xbox 0x000E0090
void CSWGuiInGameGalaxyMap::OnPanelAdded()
{
    CSWPartyTable* pPartyTable =
        g_pAppManager->m_pServerExoApp->GetPartyTable();
    if (pPartyTable->m_nSelectedPlanet == -1)
    {
        int nPlanet = 0;
        while (nPlanet < 16)
        {
            if (pPartyTable->GetPlanetAvailable(nPlanet) != 0 &&
                pPartyTable->GetPlanetSelectable(nPlanet) != 0)
            {
                pPartyTable->SetSelectedPlanet(nPlanet);
                break;
            }
            nPlanet++;
        }
    }
    int nSelected = pPartyTable->m_nSelectedPlanet;
    if (nSelected != -1)
    {
        m_aPlanetButtons[nSelected].SetActive(1);
    }
    DisplayPlanet(pPartyTable->m_nSelectedPlanet, 1);
    for (int i = 0; i < 16; i++)
    {
        m_aPlanetButtons[i].SetEnabledFlag(pPartyTable->GetPlanetAvailable(i));
    }
#ifndef _XBOX
    CSWGuiPanel::OnPanelAdded();
#endif
}
// PC 0x00694BF0  Xbox 0x000E0140
void CSWGuiInGameGalaxyMap::NextPlanet()
{
    CSWPartyTable* pPartyTable =
        g_pAppManager->m_pServerExoApp->GetPartyTable();
    int nSelected = pPartyTable->m_nSelectedPlanet;
    if (nSelected == -1)
    {
        return;
    }
    m_aPlanetButtons[nSelected].SetActive(0);
    int nPlanet = pPartyTable->m_nSelectedPlanet + 1;
    while (nPlanet != pPartyTable->m_nSelectedPlanet)
    {
        if (nPlanet >= 16)
        {
            nPlanet = 0;
        }
        if (pPartyTable->GetPlanetAvailable(nPlanet) != 0 &&
            pPartyTable->GetPlanetSelectable(nPlanet) != 0)
        {
            pPartyTable->SetSelectedPlanet(nPlanet);
            break;
        }
        nPlanet++;
    }
    m_aPlanetButtons[pPartyTable->m_nSelectedPlanet].SetActive(1);
    DisplayPlanet(pPartyTable->m_nSelectedPlanet, 1);
}
// PC 0x00695980  Xbox 0x000E1010
void CSWGuiInGameGalaxyMap::HandleInputEvent(int nEvent, int nState)
{
    CSWGuiPanel::HandleInputEvent(nEvent, nState);
    if (nState != 0)
    {
        switch (nEvent)
        {
        case GUIEVENT_ACTIVATE:
        case GUIEVENT_ACCEPT:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            if (g_pVirtualMachine != 0)
            {
                g_pAppManager->m_pClientExoApp->GetInGameGui()
                    ->m_bTransitionScriptRunning = 1;
                CExoString sScript("k_sup_galaxymap");
                g_pVirtualMachine->RunScript(&sScript, OBJECT_INVALID, 0);
                g_pAppManager->m_pClientExoApp->GetInGameGui()
                    ->m_bTransitionScriptRunning = 0;
            }
            g_pAppManager->m_pClientExoApp->GetInGameGui()->HideGalaxyMapGui();
            break;
        case GUIEVENT_CANCEL:
        case GUIEVENT_CANCEL_ALTERNATE:
#ifndef _XBOX
        case GUIEVENT_EXIT:
#endif
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            g_pAppManager->m_pClientExoApp->GetInGameGui()->HideGalaxyMapGui();
            break;
        case GUIEVENT_LEFT:
        case GUIEVENT_UP:
        case GUIEVENT_UP_ALTERNATE:
        case GUIEVENT_LEFT_ALTERNATE:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
            PrevPlanet();
            break;
        case GUIEVENT_RIGHT:
        case GUIEVENT_DOWN:
        case GUIEVENT_DOWN_ALTERNATE:
        case GUIEVENT_RIGHT_ALTERNATE:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
            NextPlanet();
            break;
        }
    }
}
// PC 0x00694CA0  Xbox 0x000E01F0
void CSWGuiInGameGalaxyMap::PrevPlanet()
{
    CSWPartyTable* pPartyTable =
        g_pAppManager->m_pServerExoApp->GetPartyTable();
    int nSelected = pPartyTable->m_nSelectedPlanet;
    if (nSelected == -1)
    {
        return;
    }
    m_aPlanetButtons[nSelected].SetActive(0);
    int nPlanet = pPartyTable->m_nSelectedPlanet - 1;
    while (nPlanet != pPartyTable->m_nSelectedPlanet)
    {
        if (nPlanet < 0)
        {
            nPlanet = 15;
        }
        if (pPartyTable->GetPlanetAvailable(nPlanet) != 0 &&
            pPartyTable->GetPlanetSelectable(nPlanet) != 0)
        {
            pPartyTable->SetSelectedPlanet(nPlanet);
            break;
        }
        nPlanet--;
    }
    m_aPlanetButtons[pPartyTable->m_nSelectedPlanet].SetActive(1);
    DisplayPlanet(pPartyTable->m_nSelectedPlanet, 1);
}
// PC 0x00694D50  Xbox 0x000E02A0
CSWGuiInGameMap::CSWGuiInGameMap(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    StartLoadFromLayout("map");
    InitControl(m_lblMapNote, "LBL_Map", 0);
    InitControl(m_lblTitle, "LBL_MapNote", 1);
    InitControl(m_lblArea, "LBL_Area", 1);
#ifdef _XBOX
    InitControl(m_lblA, "LBL_A", 1);
    InitControl(m_lblX, "LBL_X", 1);
    InitControl(m_lblPrtySlct, "LBL_PRTYSLCT", 1);
    InitControl(m_lblReturn, "LBL_RETURN", 1);
    InitControl(m_lblExit, "LBL_EXIT", 1);
    InitControl(m_lblStart, "LBL_START", 1);
    InitControl(m_lblUp, "LBL_UP", 1);
    InitControl(m_lblDown, "LBL_DOWN", 1);
    InitControl(m_lblNoteTitle, "LBL_COMPASS", 1);
    StopLoadFromLayout();

    m_imgMap.SetExtent(CSWGuiExtent(0, 0, 512, 256));
    m_cMapHider.SetExtent(CSWGuiExtent(0, 0, 440, 256));
    m_bPartySelection = 1;
    m_bReturnButton = 1;
    m_nMessageStrRef = 0;
#else
    InitControl(m_lblNoteTitle, "LBL_COMPASS", 1);
    InitControl(m_btnUp, "BTN_RETURN", 1);
    InitControl(m_btnDown, "BTN_PRTYSLCT", 1);
    InitControl(m_btnBack, "BTN_EXIT", 1);
    InitControl(m_btnExit, "BTN_UP", 1);
    InitControl(m_btnReturn, "BTN_DOWN", 1);
    StopLoadFromLayout();

    m_imgMap.SetExtent(CSWGuiExtent(0, 0, 512, 256));
    CSWGuiMapHider* pHider = &m_cMapHider;
    pHider->SetExtent(CSWGuiExtent(0, 0, 440, 256));
    AddControl(pHider);
    pHider->m_bEnabled = 0;
    m_bPartySelection = 1;
    m_bReturnButton = 1;
    m_nMessageStrRef = 0;

    CSWGuiButton* pReturn = &m_btnUp;
    pReturn->SetFocusable(0);
    pReturn->AddEvent(GUIEVENT_ACTIVATE, this,
                       (CSWGuiHandler)&CSWGuiPanel::OnXButtonPressed);
    m_btnDown.SetFocusable(0);
    m_btnDown.AddEvent(GUIEVENT_ACTIVATE, this,
                         (CSWGuiHandler)&CSWGuiPanel::OnAButtonPressed);
    m_btnBack.SetFocusable(0);
    m_btnBack.AddEvent(GUIEVENT_ACTIVATE, this,
                         (CSWGuiHandler)&CSWGuiPanel::OnBButtonPressed);
    m_btnExit.SetFocusable(0);
    m_btnExit.AddEvent(GUIEVENT_ACTIVATE, this,
                         (CSWGuiHandler)&CSWGuiInGameMap::OnUpArrowPressed);
    m_btnReturn.SetFocusable(0);
    m_btnReturn.AddEvent(GUIEVENT_ACTIVATE, this,
                           (CSWGuiHandler)&CSWGuiInGameMap::OnDownArrowPressed);
#endif
}
// PC 0x006933A0  Xbox 0x000DF350
void CSWGuiInGameGalaxyMap::DisplayPlanet(int nPlanet, int bZoom)
{
    CExoString sName;
    CExoString sDescription;
    if (nPlanet != -1 && nPlanet != m_nSelectedPlanet)
    {
        if (m_aPlanets[nPlanet].m_nNameStrRef != 0)
            sName = g_pTlkTable->GetSimpleString(m_aPlanets[nPlanet].m_nNameStrRef);
        if (m_aPlanets[nPlanet].m_nDescStrRef != 0)
            sDescription = g_pTlkTable->GetSimpleString(m_aPlanets[nPlanet].m_nDescStrRef);
        m_lblPlanetName.m_cText.m_cParams.SetText(sDescription);
        m_lblPlanetDesc.m_cText.m_cParams.SetText(sName);

        unsigned long nModels = m_cPlanetBackView.m_cScene.m_lModels.num;
        if (nModels > 0)
            m_cPlanetBackView.m_cScene.RemoveModel(0, 1);
        if (m_aPlanets[nPlanet].m_resModel.IsValid())
        {
            CExoString sModel;
            m_aPlanets[nPlanet].m_resModel.CopyToString(sModel);
            CAurCamera* pCamera = m_cPlanetBackView.m_cScene.m_pCamera;
            if (g_bDo3DGUIs != 0)
            {
                CAurObject* pModel = m_cPlanetBackView.m_cScene.AddModel(sModel, -1);
                if (pModel != 0)
                {
                    if (bZoom)
                    {
                        ((Gob*)pModel)->PlayAnimation("zoomin", 1.0f,
                                                     Gob::ANIMATION_FLAG_NO_LOOP, 0.0f);
                        ((Gob*)pModel)->PlayAnimation("rotate", 1.0f,
                                                     Gob::ANIMATION_FLAG_QUEUE, 0.0f);
                    }
                    pCamera->AttachToObject(pModel, "camerahook", Gob::ATTACH_NORMAL);
                    pCamera->SetViewAngle(22.726f);
                }
            }
        }
        m_nSelectedPlanet = nPlanet;
    }
}

#ifndef _XBOX
void CSWGuiInGameGalaxyMap::OnPlanetClicked(CSWGuiControl* pControl)  // PC 0x006935A0
{
    CSWPartyTable* pPartyTable;
    if (pControl->m_bSelected)
    {
        pPartyTable = g_pAppManager->m_pServerExoApp->GetPartyTable();
        if (pPartyTable->m_nSelectedPlanet != -1)
        {
            if (pPartyTable->m_nSelectedPlanet == pControl->m_nIndex)
            {
                return;
            }
        }
        if (pPartyTable->m_nSelectedPlanet != -1)
        {
            m_aPlanetButtons[pPartyTable->m_nSelectedPlanet].SetActive(0);
        }
        pPartyTable->SetSelectedPlanet(pControl->m_nIndex);
        DisplayPlanet(pPartyTable->m_nSelectedPlanet, 1);
    }
}
#endif
// PC 0x00693610
void CSWGuiInGameGalaxyMap::Draw(float fElapsed)
{
    int nSelected =
        g_pAppManager->m_pServerExoApp->GetPartyTable()->m_nSelectedPlanet;
    if (nSelected != -1)
    {
        m_aPlanetButtons[nSelected].SetActive(1);
    }
    return CSWGuiPanel::Draw(fElapsed);
}

static inline void SwguiingamemapTrace2()
{
    SwguiingamemapTrace1();
}

static inline void SwguiingamemapTrace1()
{
}

