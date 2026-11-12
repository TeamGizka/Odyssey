#include <aurlist.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

template List<unsigned long>::List(int);

#include "exobase.h"

#include "swguipazaak.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "serverexoapp.h"
#include "partytables.h"
#include "guiingame.h"
#include "swguiingame.h"
#include "swguiingame.inl"
#include "nwccreature.h"

int g_bPazaakShowNPCSideDeck = 0;

#include <math.h>
// PC 0x006D3580
void CSWGuiPazaakCard::Draw(float fElapsed)
{
    if (m_bActive)
    {
        m_cBorder.Draw(fElapsed);
        m_cHilite.Draw(fElapsed);
        return;
    }
    CSWGuiButton::Draw(fElapsed);
}
// PC 0x006D3700
void CSWGuiPazaakCard::HandleInputEvent(int nEvent, int nState)
{
#ifndef _XBOX
    if (nEvent == 39)
#endif
    {
        return CSWGuiNavigable::HandleInputEvent(nEvent, nState);
    }
#ifndef _XBOX
    return CSWGuiButton::HandleInputEvent(nEvent, nState);
#endif
}
// PC 0x006D2F50
inline void CSWPazaak::DrawPlayerCard()
{
    m_cPlayer.AddGameCard(DrawCard());
}
// PC 0x006D2F90
inline int CSWPazaak::PlayOpponentTurn()
{
    m_nAIState = PlayAITurn(m_cOpponent, m_cPlayer, m_nAIState);
    return m_nAIState;
}
// PC 0x006D2FC0
inline void CSWGuiWagerPopup::HandleButtonLess(CSWGuiControl* pControl)
{
    HandleInputEvent(GUIEVENT_LEFT, 1);
}
// PC 0x006D2FD0
inline void CSWGuiWagerPopup::HandleButtonMore(CSWGuiControl* pControl)
{
    HandleInputEvent(GUIEVENT_RIGHT, 1);
}
// PC 0x006D2FE0
inline void CSWGuiWagerPopup::HandleButtonWager(CSWGuiControl* pControl)
{
    HandleInputEvent(45, 1);
}
// PC 0x006D3020
void CSWGuiActivatedButton::Draw(float fElapsed)
{
    if (m_fFlashTimer >= 0.0f)
    {
#ifdef _XBOX
        Vector vColor = CGuiInGame::DISABLED_TEXT;
        float fTime = m_fFlashTimer - fElapsed;
#else
        float fTime = m_fFlashTimer - fElapsed;
        Vector vColor = CGuiInGame::DISABLED_TEXT;
#endif
        m_fFlashTimer = fTime;
        if (fTime < 0.0f)
        {
#ifndef _XBOX
            if (m_bHilightOnOver)
            {
                vColor = CGuiInGame::COLOR_BLUE;
            }
#else
            if (m_bActivated)
            {
                vColor = CGuiInGame::COLOR_BLUE;
            }
#endif
        }
        else
        {
            float fSin = (float)sin(fTime * 23.561945f);
            float fBlend = (fSin + 1.0f) * 0.5f;
#ifdef _XBOX
            float fInverse = 1.0f - fBlend;
            Vector vDim(vColor.m_fX * fInverse,
                        vColor.m_fY * fInverse,
                        (1.0f - fBlend) * vColor.m_fZ);
#else
            Vector vDim(vColor.m_fX * (1.0f - fBlend),
                        vColor.m_fY * (1.0f - fBlend),
                        (1.0f - fBlend) * vColor.m_fZ);
#endif
            Vector vFlash(m_vFlashColor.m_fX * fBlend,
                          m_vFlashColor.m_fY * fBlend,
                          m_vFlashColor.m_fZ * fBlend);
            vColor = Vector(vDim.m_fX + vFlash.m_fX, vDim.m_fY + vFlash.m_fY,
                            vDim.m_fZ + vFlash.m_fZ);
        }
        m_cText.m_cParams.SetColor(vColor);
    }
#ifdef _XBOX
    CSWGuiLabel::Draw(fElapsed);
#else
    CSWGuiButton::Draw(fElapsed);
#endif
}

#ifndef _XBOX
void CSWGuiActivatedButton::SetEnabled(int bEnabled)  // PC 0x006D3160
{
    CSWGuiButton::SetEnabled(bEnabled);
    if (m_fFlashTimer < 0.0f)
    {
        Vector* pColor = &CGuiInGame::COLOR_BLUE;
#ifndef _XBOX
        if (!m_bHilightOnOver)
        {
            pColor = &CGuiInGame::DISABLED_TEXT;
        }
#endif
        m_cText.m_cParams.SetColor(*pColor);
#ifndef _XBOX
        SetActive(m_bHilightOnOver && this == g_pGuiMan->m_pMouseOverControl);
#endif
    }
}
#else
inline void CSWGuiActivatedButton::SetActivated(int bActivated)
{
    m_bActivated = bActivated;
    if (m_fFlashTimer < 0.0f)
    {
        m_cText.m_cParams.SetColor(bActivated ? CGuiInGame::COLOR_BLUE : CGuiInGame::DISABLED_TEXT);
    }
}
#endif
// PC 0x006D3200
void CSWGuiPazaakCard::Load(CResGFF* pGFF, CResStruct& cStruct)
{
    CSWGuiButton::Load(pGFF, cStruct);
    m_cHilite.m_cParams.SetPulsingAlpha(1, 0, 1);
}
// PC 0x006D3230
void CSWGuiPazaakCard::SetCard(CPazaakCard cCard)
{
    static char* s_pszCardLabels[28] = {"+1",
                                       "+2",
                                       "+3",
                                       "+4",
                                       "+5",
                                       "+6",
                                       "-1",
                                       "-2",
                                       "-3",
                                       "-4",
                                       "-5",
                                       "-6",
                                       "\xB1"
                                       "1",
                                       "\xB1"
                                       "2",
                                       "\xB1"
                                       "3",
                                       "\xB1"
                                       "4",
                                       "\xB1"
                                       "5",
                                       "\xB1"
                                       "6",
                                       "1",
                                       "2",
                                       "3",
                                       "4",
                                       "5",
                                       "6",
                                       "7",
                                       "8",
                                       "9",
                                       "10"};

    m_cCard = cCard;
    if (cCard.m_nType >= 0 && cCard.m_nType < 28)
    {
        if (cCard.m_nType >= 12 && cCard.m_nType < 18)
        {
            if (cCard.m_bFlipped != 0)
            {
                CExoString sText(s_pszCardLabels[cCard.m_nType - 6]);
                m_lblCard.m_cText.m_cParams.SetText(sText);
            }
            else
            {
                CExoString sText(s_pszCardLabels[cCard.m_nType - 12]);
                m_lblCard.m_cText.m_cParams.SetText(sText);
            }
        }
        else
        {
            CExoString sText(s_pszCardLabels[cCard.m_nType]);
            m_lblCard.m_cText.m_cParams.SetText(sText);
        }
    }
    else
    {
        CExoString sText("");
        m_lblCard.m_cText.m_cParams.SetText(sText);
    }

    CResRef cImage;
    switch (cCard.m_nType)
    {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
        cImage = "lbl_cardmpos";
        break;
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
        cImage = "lbl_cardmneg";
        break;
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
        if (cCard.m_bFlipped != 0)
        {
            cImage = "lbl_cardraref";
        }
        else
        {
            cImage = "lbl_cardrarem";
        }
        break;
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
        cImage = "lbl_cardstand";
        break;
    case -1:
        cImage = "";
        break;
    default:
        cImage = "lbl_cardback";
        break;
    }
    m_cBorder.m_cParams.SetFillImage(cImage, 0);
}

#ifndef _XBOX
void CSWGuiPazaakCard::DrawDragDrop(int nX, int nY)  // PC 0x006D35C0
{
#ifndef _XBOX
    if (g_pGuiMan->m_pMouseCapture == this &&
        g_pGuiMan->m_nMouseCaptureType == 1)
#else
    if (0)
#endif
    {
        CSWGuiExtent cBorderOrig = m_cBorder.m_cExtent;
        CSWGuiExtent cBorderDrag = m_cBorder.m_cExtent;
        cBorderDrag.m_nLeft = nX - cBorderDrag.m_nWidth / 2;
        cBorderDrag.m_nTop = nY - cBorderDrag.m_nHeight / 2;
        m_cBorder.SetExtent(cBorderDrag);
        m_cBorder.Draw(0.0f);
        CSWGuiExtent cLabelOrig = m_lblCard.m_cExtent;
        CSWGuiExtent cLabelDrag = m_lblCard.m_cExtent;
        cLabelDrag.m_nLeft += cBorderDrag.m_nLeft - cBorderOrig.m_nLeft;
        cLabelDrag.m_nTop += cBorderDrag.m_nTop - cBorderOrig.m_nTop;
        m_lblCard.SetExtent(cLabelDrag);
        m_lblCard.Draw(0.0f);
        m_lblCard.SetExtent(cLabelOrig);
        m_cBorder.SetExtent(cBorderOrig);
    }
    else
    {
#ifndef _XBOX
        g_pGuiMan->m_pDragControl = NULL;
#endif
    }
}
// PC 0x006D3720
void CSWGuiPazaakCard::HandleLMouseDown()
{
#ifndef _XBOX
    m_nDownX = g_pGuiMan->m_nMouseX;
    m_nDownY = g_pGuiMan->m_nMouseY;
#endif
    CSWGuiControl::HandleLMouseDown();
}
// PC 0x006D3740
int CSWGuiPazaakCard::HandleMouseCapturedMovement(int nX, int nY)
{
#ifndef _XBOX
    if (m_bDraggable != 0 && g_pGuiMan->m_nMouseCaptureType == 1 &&
        g_pGuiMan->m_pDragControl != this &&
        *m_cBorder.m_cParams.m_cFillImage.GetResRef() != 0 &&
        (abs(nX - m_nDownX) > 12 || abs(nY - m_nDownY) > 12))
    {
        g_pGuiMan->m_pDragControl = this;
    }
#endif
    return 0;
}
// PC 0x006D37B0
void CSWGuiPazaakCard::HandleLMouseUp()
{
#ifndef _XBOX
    if (g_pGuiMan->m_pDragControl == this)
    {
        g_pGuiMan->m_pDragControl = NULL;
        HandleInputEvent(98283, 1);
        g_pGuiMan->ReleaseMouse(this);
        HandleLoseMouseFocus();
        return;
    }
#endif
    CSWGuiControl::HandleLMouseUp();
}
#endif
// PC 0x006D37F0
void CSWGuiTutorial::PlaceRightOf(CSWGuiControl* pControl)
{
    CSWGuiExtent cExtent = pControl->m_cExtent;
#ifndef _XBOX
    CSWGuiControl* pParent = (CSWGuiControl*)pControl->m_pOwner;
#endif
    int nRight = cExtent.m_nLeft + cExtent.m_nWidth;
    int nCenter = cExtent.m_nTop + cExtent.m_nHeight / 2;
#ifndef _XBOX
    if (pParent != NULL)
    {
        CSWGuiPanel* pPanel = pParent->AsSWGuiPanel();
        if (pPanel != NULL)
        {
            pPanel->GetExtentAccountingForPanelOffset(&cExtent);
        }
        nRight += cExtent.m_nLeft;
        nCenter += cExtent.m_nTop;
    }
#endif
    cExtent = m_cExtent;
    cExtent.m_nLeft = nRight;
    cExtent.m_nTop = nCenter - cExtent.m_nHeight / 2;
    SetExtent(cExtent);
}
// PC 0x006D38A0
void CSWGuiTutorial::PlaceAbove(CSWGuiControl* pControl)
{
#ifndef _XBOX
    m_pPlacedAbove = pControl;
#endif
    CSWGuiExtent cExtent = pControl->m_cExtent;
#ifndef _XBOX
    CSWGuiControl* pParent = (CSWGuiControl*)pControl->m_pOwner;
#endif
    int nLeft = cExtent.m_nLeft;
    int nTop = cExtent.m_nTop;
#ifndef _XBOX
    if (pParent != NULL)
    {
        CSWGuiPanel* pPanel = pParent->AsSWGuiPanel();
        if (pPanel != NULL)
        {
            pPanel->GetExtentAccountingForPanelOffset(&cExtent);
        }
        nLeft += cExtent.m_nLeft;
        nTop += cExtent.m_nTop;
    }
#endif
    cExtent = m_cExtent;
    cExtent.m_nLeft = nLeft;
    cExtent.m_nTop = nTop - cExtent.m_nHeight;
    SetExtent(cExtent);
}
// PC 0x006D4870
void CSWGuiTutorial::HandleInputEvent(int nEvent, int nState)
{
    if (nState != 0)
    {
        switch (nEvent)
        {
        case 39:
#ifndef _XBOX
        case 502:
#endif
            if (m_nHelpIndex < m_lHelp.m_nCount - 1)
            {
                m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
                ++m_nHelpIndex;
                SetAllowCancel(1);
                SetMessage(m_lHelp.m_pList[m_nHelpIndex]);
#ifndef _XBOX
                if (m_pPlacedAbove != NULL)
                {
                    PlaceAbove(m_pPlacedAbove);
                }
#endif
                return;
            }
            break;
        case 40:
#ifndef _XBOX
        case 46:
        case 503:
#endif
            if (m_nHelpIndex > 0)
            {
                m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
                if (--m_nHelpIndex == 0)
                {
                    SetAllowCancel(0);
                }
                SetMessage(m_lHelp.m_pList[m_nHelpIndex]);
#ifndef _XBOX
                if (m_pPlacedAbove != NULL)
                {
                    PlaceAbove(m_pPlacedAbove);
                }
#endif
                return;
            }
            break;
        }
    }
    return CSWGuiMessageBox::HandleInputEvent(nEvent, nState);
}
#ifndef _XBOX
void CSWGuiWagerPopup::Draw(float fElapsed)
{
    CSWGuiPanel::Draw(fElapsed);
}
#endif
void CSWGuiWagerPopup::UpdateWagerText()  // PC 0x006D3940
{
    CExoString sWager(m_nWager);
    m_lblWagerval.m_cText.m_cParams.SetText(sWager);
}
// PC 0x006D39C0
void CSWGuiPazaakStart::HandleWagerExit(int nWager)
{
    m_pPazaak->m_nWager = nWager;
    SetActiveControl(&m_cAvail[0], 0);
}

#ifdef _XBOX

void CSWGuiPazaakGame::HandleInputEvent(int nEvent, int nState)
{
    if (nState != 0)
    {
        switch (nEvent)
        {
        case 40:
        case 46:
        {
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            CSWGuiMessageBox* pMessageBox =
                g_pAppManager->m_pClientExoApp->GetInGameGui()
                    ->m_pMessageBox;
            pMessageBox->SetAllowCancel(1);
            pMessageBox->SetCallback(
                this, (CSWGuiHandler)&CSWGuiPazaakGame::HandleQuitDialog,
                NULL);
            pMessageBox->SetMessage(42425);
            m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
            break;
        }
        case 41:
            if (m_nState == 3 && m_pActiveControl != NULL)
            {
                int nIndex = m_pActiveControl->m_nID;
                if (nIndex >= 0 && nIndex < 4 &&
                    m_pActiveControl == &m_cPlayerHand[nIndex])
                {
                    if (m_pPazaak->CanUsePlayerSidedeckCard(nIndex) != 0)
                    {
                        m_btnX.StartFlash(CGuiInGame::COLOR_CYAN);
                        if (m_cTutorial.m_bEnabled != 0 &&
                            m_pPazaak->GetPlayerTotal() <= 12)
                        {
                            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
                            m_cTutorial.ShowHelp(38642, 32330, 38626);
                        }
                        else
                        {
                            m_pPazaak->UsePlayerSidedeckCard(nIndex);
                            CResRef cSound(m_szPlaySideSound);
                            g_pExoSound->PlayOneShotSound(cSound, CExoSound::PRIORITY_GROUP_GUI, 0, 0, 0.0f, 0.0f);
                            m_nState = 4;
                            m_fTimer = 0.4f;
                            RefreshDisplay();
                            ReconnectSidedeckButtons();
                        }
                    }
                }
            }
            break;
        case 43:
            if (m_bUnknown86DC0 == 0)
            {
                m_bUnknown86DC0 = 1;
                unsigned long aHelp[2];
                aHelp[0] = 38643;
                aHelp[1] = 38644;
                m_cTutorial.ShowMultipleHelp(aHelp, 2);
            }
            if (m_pActiveControl != NULL)
            {
                int nCard = m_pActiveControl->m_nID;
                if (nCard >= 0 && nCard < 4 &&
                    m_pActiveControl == &m_cPlayerHand[nCard])
                {
                    CPazaakCard cCard = m_pPazaak->m_cPlayer.m_aSideDeck[nCard];
                    if (cCard.m_nType >= 12 && cCard.m_nType < 18)
                    {
                        m_btnBlack.StartFlash(CGuiInGame::COLOR_CYAN);
                        m_pPazaak->m_cPlayer.m_aSideDeck[nCard].m_bFlipped =
                            m_pPazaak->m_cPlayer.m_aSideDeck[nCard].m_bFlipped == 0;
                        m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
                        RefreshDisplay();
                    }
                }
            }
            break;
        case 39:
        case 45:
            if (m_pPazaak->m_cPlayer.m_bStanding == 0)
            {
                if (m_nState == 3 || m_nState == 4)
                {
                    m_btnContinue.StartFlash(CGuiInGame::COLOR_CYAN);
                    m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
                    if (m_cTutorial.m_bEnabled != 0 &&
                        m_pPazaak->GetPlayerTotal() >= 16)
                    {
                        m_cTutorial.ShowHelp(38641, 32332, 38626);
                    }
                    else
                    {
                        m_nState = 5;
                        m_fTimer = 0.4f;
                        RefreshDisplay();
                    }
                }
            }
            break;
        case 42:
            if (m_pPazaak->m_cPlayer.m_bStanding == 0)
            {
                if (m_nState == 3 || m_nState == 4)
                {
                    m_btnStand.StartFlash(CGuiInGame::COLOR_CYAN);
                    m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
                    if (m_cTutorial.m_bEnabled != 0 &&
                        m_pPazaak->GetPlayerTotal() <= 13)
                    {
                        m_cTutorial.ShowHelp(38640, 32333, 38626);
                    }
                    else
                    {
                        m_pPazaak->m_cPlayer.m_bStanding = 1;
                        m_nState = 5;
                        m_fTimer = 0.4f;
                        RefreshDisplay();
                    }
                }
            }
            break;
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
#else
void CSWGuiPazaakGame::HandleInputEvent(int nEvent, int nState)  // PC 0x006D50F0
{
    if (nState != 0)
    {
        if (nEvent == 40 || nEvent == 46)
        {
            if (m_nHandAction != 0)
            {
                CancelHandAction();
            }
            else
            {
                m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
                CSWGuiMessageBox* pMessageBox =
                    g_pAppManager->m_pClientExoApp->GetInGameGui()
                        ->m_pMessageBox;
                pMessageBox->SetAllowCancel(1);
                pMessageBox->SetCallback(
                    this, (CSWGuiHandler)&CSWGuiPazaakGame::HandleQuitDialog,
                    NULL);
                pMessageBox->SetMessage(42425);
                m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
            }
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
#endif
// PC 0x006D66C0
void CSWGuiPazaakGame::HandleEndDialog(CSWGuiControl* pControl)
{
    if (m_pPazaak->m_cPlayer.m_nWins >= 3 ||
        m_pPazaak->m_cOpponent.m_nWins >= 3)
    {
        m_cTutorial.ShowHelp(38649, 0xFFFFFFFF, 38626);
        m_nState = 12;
    }
    else
    {
        m_cTutorial.ShowHelp(38648, 0xFFFFFFFF, 38625);
        m_nState = 11;
    }
}
// PC 0x006D6730
void CSWGuiPazaakGame::HandleTutorialClose(CSWGuiControl* pControl)
{
    int nHelp;
    CSWGuiTutorial* pTutorial = &m_cTutorial;
    if (pTutorial->m_nHelpIndex < pTutorial->m_lHelp.m_nCount)
    {
        nHelp = pTutorial->m_lHelp.m_pList[pTutorial->m_nHelpIndex];
    }
    else
    {
        nHelp = 0;
    }
    if (m_cTutorial.m_bAccepted)
    {
        switch (nHelp)
        {
        case 38633:
        {
#ifdef _XBOX
            unsigned long aHelp[3];
#else
            unsigned long aHelp[5];
#endif
            aHelp[0] = 38634;
            aHelp[1] = 38635;
            aHelp[2] = 38636;
#ifdef _XBOX
            pTutorial->ShowMultipleHelp(aHelp, 3);
#else
            aHelp[3] = 38643;
            aHelp[4] = 38644;
            pTutorial->ShowMultipleHelp(aHelp, 5);
#endif
            pTutorial->PlaceAbove(&m_cPlayerHand[0]);
            break;
        }
        case 38636:
#ifndef _XBOX
        case 38644:
#endif
        {
            unsigned long aHelp[3];
            aHelp[0] = 38637;
            aHelp[1] = 38638;
            aHelp[2] = 38639;
            pTutorial->ShowMultipleHelp(aHelp, 3);
#ifdef _XBOX
            pTutorial->PlaceAbove(&m_lblXbutton);
#else
            pTutorial->PlaceAbove(&m_btnContinue);
#endif
            break;
        }
        case 38640:
            m_pPazaak->m_cPlayer.m_bStanding = 1;
            m_nState = STATE_END_PLAYER_TURN;
            m_fTimer = 0.4f;
            RefreshDisplay();
            break;
        case 38641:
            m_nState = STATE_END_PLAYER_TURN;
            m_fTimer = 0.4f;
            RefreshDisplay();
            break;
        case 38642:
        {
#ifdef _XBOX
            m_pPazaak->UsePlayerSidedeckCard(m_pActiveControl->m_nID);
#else
            m_pPazaak->UsePlayerSidedeckCard(m_nPlayedCard);
            m_nPlayedCard = -1;
#endif
            g_pExoSound->PlayOneShotSound(m_szPlaySideSound, CExoSound::SOUND_TYPE_GUI, 0, 0, 0.0f, 0.0f);
            m_nState = STATE_PLAYER_SIDE_CARD_PLAYED;
            RefreshDisplay();
            ReconnectSidedeckButtons();
            break;
        }
        case 38649:
            m_bDeferredRemoval = 0;
            m_nExitState = EXITSTATE_DELETE;
            g_pAppManager->m_pClientExoApp->EndPazaakGame(
                m_pPazaak->m_cPlayer.m_nWins > m_pPazaak->m_cOpponent.m_nWins,
                m_pPazaak->m_nWager, 0, m_pPazaak->m_oidOpponent);
            break;
        }
    }
    else if ((unsigned int)nHelp >= 38648 && (unsigned int)nHelp <= 38649)
    {
        m_nState = STATE_EXIT_GAME;
    }
}

#ifndef _XBOX
void CSWGuiPazaakGame::HandleContinue(CSWGuiControl* pControl)  // PC 0x006D6940
{
    if (pControl == NULL || pControl->m_bSelected != 0)
    {
        if (m_pPazaak->m_cPlayer.m_bStanding == 0)
        {
            if (m_nState == 3 || m_nState == 4)
            {
                if (m_nHandAction == 0)
                {
                    m_btnContinue.StartFlash(CGuiInGame::COLOR_CYAN);
                    m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
                    if (m_cTutorial.m_bEnabled != 0)
                    {
                        if (m_pPazaak->GetPlayerTotal() >= 16)
                        {
                            m_cTutorial.ShowHelp(38641, 32332, 38626);
                        }
                        else
                        {
                            m_fTimer = 0.4f;
                            m_nState = 5;
                            RefreshDisplay();
                        }
                    }
                    else
                    {
                        m_fTimer = 0.4f;
                        m_nState = 5;
                        RefreshDisplay();
                    }
                }
            }
        }
    }
}
// PC 0x006D6A20
void CSWGuiPazaakGame::HandleStand(CSWGuiControl* pControl)
{
    if (pControl == NULL || pControl->m_bSelected != 0)
    {
        if (m_pPazaak->m_cPlayer.m_bStanding == 0)
        {
            if (m_nState == 3 || m_nState == 4)
            {
                m_btnStand.StartFlash(CGuiInGame::COLOR_CYAN);
                m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
                if (m_cTutorial.m_bEnabled != 0 &&
                    m_pPazaak->GetPlayerTotal() <= 13)
                {
                    m_cTutorial.ShowHelp(38640, 32333, 38626);
                }
                else
                {
                    m_pPazaak->m_cPlayer.m_bStanding = 1;
                    m_nState = 5;
                    m_fTimer = 0.4f;
                    RefreshDisplay();
                }
            }
        }
    }
}
// PC 0x006D6AD0
void CSWGuiPazaakGame::HandlePlayHandCard(int nIndex)
{
    if (m_nState == 3 && nIndex >= 0 && nIndex < 4)
    {
        if (m_pPazaak->CanUsePlayerSidedeckCard(nIndex) != 0)
        {
            m_btnX.StartFlash(CGuiInGame::COLOR_CYAN);
            if (m_cTutorial.m_bEnabled != 0 &&
                m_pPazaak->GetPlayerTotal() <= 12)
            {
                m_nPlayedCard = nIndex;
                m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
                m_cTutorial.ShowHelp(38642, 32330, 38626);
            }
            else
            {
                m_pPazaak->UsePlayerSidedeckCard(nIndex);
                CResRef cSound(m_szPlaySideSound);
                g_pExoSound->PlayOneShotSound(cSound, CExoSound::PRIORITY_GROUP_GUI, 0, 0, 0.0f, 0.0f);
                m_nState = 4;
                m_fTimer = 0.4f;
                RefreshDisplay();
                ReconnectSidedeckButtons();
            }
        }
    }
}
#endif

#ifndef _XBOX
void CSWGuiPazaakGame::HandleHandCardDrop(CSWGuiControl* pControl)  // PC 0x006D6C60
{
    CSWGuiControl* pDropTarget = g_pGuiMan->m_pMouseOverControl;
    CSWGuiControl* pOldActive = m_pActiveControl;
    if (pDropTarget != NULL)
    {
        if (pDropTarget == &m_btnBlack)
        {
            m_pActiveControl = pControl;
            HandleFlipHandCard();
            m_pActiveControl = pOldActive;
        }
        else if (m_nState == 3)
        {
            BOOL bFound = pDropTarget == &m_btnX;
            if (!bFound)
            {
                for (int i = 0; i < 9; i++)
                {
                    if (pDropTarget == &m_cPlayerBoard[i])
                    {
                        bFound = TRUE;
                        break;
                    }
                }
            }
            if (bFound)
            {
                HandlePlayHandCard(pControl->m_nID);
            }
        }
    }
}
#endif

#ifndef _XBOX
void CSWGuiPazaakGame::HandleHandCardDoubleClick(CSWGuiControl* pControl)  // PC 0x006D6CE0
{
    if (pControl != NULL && m_nState == 3)
    {
        HandlePlayHandCard(pControl->m_nID);
    }
}
#endif

CSWGuiActivatedButton::CSWGuiActivatedButton()
{
    m_fFlashTimer = -1.0f;
#ifndef _XBOX
    m_bHilightOnOver = 1;
#else
    m_bActivated = 1;
#endif
}
// PC 0x006D46E0
CSWGuiPazaakCard::CSWGuiPazaakCard()
{
#ifndef _XBOX
    m_bDraggable = 1;
    m_bSelectable = 1;
#endif
    m_cCard = CPazaakCard(-1);
    CSWGuiLabel* pLabel = &m_lblCard;
#ifndef _XBOX
    pLabel->m_bIgnoreMouse = 1;
#endif
}
#ifndef _XBOX
CSWGuiPazaakCard::~CSWGuiPazaakCard()  // PC 0x006D47A0
{
#ifndef _XBOX
    if (g_pGuiMan->m_pDragControl == this)
    {
        g_pGuiMan->m_pDragControl = NULL;
    }
#endif
}
#endif
CSWGuiTutorial::CSWGuiTutorial(CSWGuiManager* pManager)  // PC 0x006D5310
    : CSWGuiMessageBox(pManager), m_lHelp(0)
{
    m_nHelpIndex = 0;
#ifndef _XBOX
    m_pPlacedAbove = NULL;
#endif
    m_bEnabled = 1;
}
// PC 0x006D53A0
void CSWGuiTutorial::ShowHelp(unsigned long nHelpStrRef,
                              unsigned long nTitleStrRef,
                              unsigned long nButtonStrRef)
{
    m_lHelp.m_nCount = 0;
    m_nHelpIndex = 0;
#ifndef _XBOX
    m_pPlacedAbove = NULL;
#endif
    if (m_nExitState != 0)
    {
        m_bDeferredRemoval = 0;
        m_nExitState = 0;
        m_pManager->RemovePanel(this);
    }
    if (m_bEnabled != 0)
    {
        m_lHelp.Add(nHelpStrRef);
        if (nTitleStrRef == 0xFFFFFFFF && nButtonStrRef == 38626)
        {
            SetAllowCancel(0);
#ifdef _XBOX
            m_lblOk.m_cText.m_cParams.SetStrRef(38623);
#else
            m_btnOk.m_cText.m_cParams.SetStrRef(38623);
#endif
        }
        else
        {
            SetAllowCancel(1);
#ifdef _XBOX
            m_lblOk.m_cText.m_cParams.SetStrRef(nTitleStrRef);
            m_lblCancel.m_cText.m_cParams.SetStrRef(nButtonStrRef);
#else
            m_btnOk.m_cText.m_cParams.SetStrRef(nTitleStrRef);
            m_btnCancel.m_cText.m_cParams.SetStrRef(nButtonStrRef);
#endif
        }
        SetMessage(nHelpStrRef);
        m_pManager->AddPanel(this, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
    }
}
// PC 0x006D5490
void CSWGuiTutorial::ShowMultipleHelp(const unsigned long* pStrRefs, int nCount)
{
    if (m_bEnabled)
    {
        ShowHelp(pStrRefs[0], 0xFFFFFFFF, 38626);
        for (int i = 1; i < nCount; i++)
        {
            m_lHelp.Add(pStrRefs[i]);
        }
#ifdef _XBOX
        m_lblCancel.m_cText.m_cParams.SetStrRef(38624);
#else
        m_btnCancel.m_cText.m_cParams.SetStrRef(38624);
#endif
    }
}
#ifndef _XBOX
void CSWGuiWagerPopup::HandleInputEvent(int nEvent, int nState)  // PC 0x006D4950
{
    if (nState != 0)
    {
        switch (nEvent)
        {
        case 39:
        case 45:
            m_pStart->HandleWagerExit(m_nWager);
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            m_pManager->PopModalPanel();
            m_pManager->RemovePanel(this);
            break;
        case 40:
        case 46:
        {
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            CSWGuiMessageBox* pMessageBox =
                g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
            pMessageBox->SetAllowCancel(1);
            pMessageBox->SetCallback(
                m_pStart, (CSWGuiHandler)&CSWGuiPazaakStart::HandleQuitDialog,
                NULL);
            pMessageBox->SetMessage(42424);
            m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
            break;
        }
        case 47:
        case 50:
        case 58:
        case 59:
        case 62:
        case 63:
            if (m_nWager > 1)
            {
                m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
                m_nWager = m_nWager - 1;
                UpdateWagerText();
            }
            break;
        case 48:
        case 49:
        case 57:
        case 60:
        case 61:
        case 64:
            if (m_nWager < m_nMaxWager)
            {
                m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
                m_nWager = m_nWager + 1;
                UpdateWagerText();
            }
            break;
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
#endif
void CSWGuiPazaakStart::HandleQuitDialog(CSWGuiControl* pControl)  // PC 0x006D39E0
{
    if (g_pAppManager->m_pClientExoApp->GetInGameGui()
            ->m_pMessageBox->m_bAccepted)
    {
        if (m_pManager->PanelExists(m_pWagerPopup))
        {
            m_pManager->RemovePanel(m_pWagerPopup);
        }
        CSWPazaak* pPazaak = m_pPazaak;
        m_bDeferredRemoval = 0;
        m_nExitState = EXITSTATE_DELETE;
        g_pAppManager->m_pClientExoApp->EndPazaakGame(0, 0, 0,
                                                      pPazaak->m_oidOpponent);
    }
}
// PC 0x006D3A60
int CSWGuiPazaakStart::IsAvailCardActive()
{
    int nIndex = m_pActiveControl->m_nID;
    if (nIndex >= 0 && nIndex < AvailCardCount)
    {
        return m_pActiveControl == &m_cAvail[nIndex];
    }
    return 0;
}
// PC 0x006D5530
CSWGuiWagerPopup::CSWGuiWagerPopup(CSWGuiManager* pManager,
                                   CSWGuiPazaakStart* pStart, int nMaxWager)
    : CSWGuiPanel(pManager)
{
    m_pStart = pStart;
    m_nMaxWager = nMaxWager;
    m_nWager = nMaxWager;
    StartLoadFromLayout("pazaakwager");
    InitControl(m_lblTitle, "LBL_TITLE", 1);
    InitControl(m_lblMaximum, "LBL_MAXIMUM", 1);
    InitControl(m_lblWagerval, "LBL_WAGERVAL", 1);
#ifdef _XBOX
    InitControl(m_lblLess, "LBL_LESS", 1);
    InitControl(m_lblMore, "LBL_MORE", 1);
    InitControl(m_lblA, "LBL_A", 1);
    InitControl(m_lblB, "LBL_B", 1);
    InitControl(m_lblWager, "LBL_WAGER", 1);
    InitControl(m_lblQuit, "LBL_QUIT", 1);
#else
    InitControl(m_btnLess, "BTN_LESS", 1);
    InitControl(m_btnMore, "BTN_MORE", 1);
    InitControl(m_btnWager, "BTN_WAGER", 1);
    InitControl(m_btnQuit, "BTN_QUIT", 1);
#endif
    InitControl(m_lblBg, "LBL_BG", 1);
    StopLoadFromLayout();
#ifdef _XBOX
    m_lblLess.m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_BLUE;
    m_lblMore.m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_BLUE;
#else
    m_btnLess.AddEvent(CSWGuiSpeedButton::EVENT_REPEAT_LEFT, this,
                       (CSWGuiHandler)&CSWGuiWagerPopup::HandleButtonLess);
    m_btnMore.AddEvent(CSWGuiSpeedButton::EVENT_REPEAT_LEFT, this,
                       (CSWGuiHandler)&CSWGuiWagerPopup::HandleButtonMore);
    m_btnWager.AddEvent(GUIEVENT_ACTIVATE, this,
                        (CSWGuiHandler)&CSWGuiWagerPopup::HandleButtonWager);
    m_btnQuit.AddEvent(GUIEVENT_ACTIVATE, this,
                      (CSWGuiHandler)&CSWGuiWagerPopup::HandleButtonQuit);
    m_btnLess.m_fRepeatDelay = 1.0f / 15.0f;
    m_btnMore.m_fRepeatDelay = 1.0f / 15.0f;
    m_btnLess.SetFocusable(0);
    m_btnMore.SetFocusable(0);
    m_btnWager.SetFocusable(0);
    m_btnQuit.SetFocusable(0);
#endif
    CSWCCreature* pPlayer =
        g_pAppManager->GetClientExoApp()->GetSWParty()->GetPlayerCharacter();
    unsigned long oidServer =
        g_pAppManager->GetServerExoApp()->ClientToServerObjectId(pPlayer->GetObjectId());
    CSWSCreature* pCreature =
        g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidServer);
    int nGold = 0;
    if (pCreature != NULL)
    {
        nGold = pCreature->GetGold();
    }
    UpdateWagerText();
    CExoString sText = g_pTlkTable->GetSimpleString(STRREF_MAXIMUM_WAGER) + " " + m_nMaxWager +
                       "\n" + g_pTlkTable->GetSimpleString(STRREF_CREDITS) + " " + nGold;
    m_lblMaximum.m_cText.m_cParams.SetText(sText);
    m_bCenterHorizontal = 1;
    m_bCenterVertical = 1;
}
// PC 0x006D8CD0
CSWGuiPazaakStart::CSWGuiPazaakStart(CSWGuiManager* pManager, int nWager,
                                     int nSideDeck, int bTutorial,
                                     unsigned long oidOpponent)
    : CSWGuiPanel(pManager),
      m_cMessageBox(pManager),
      m_cTutorial(pManager)
{
    m_pWagerPopup = NULL;
#ifdef _XBOX
    m_bChosenActive = 0;
#endif
    m_bSideDeckFull = 0;
    m_pChosenCards = &m_cChosen[0];
#ifndef _XBOX
    m_pAvailCards = &m_cAvail[0];
#endif
    for (int i = 0; i < AvailCardCount; i++)
    {
        m_anAvailCount[i] = 0;
    }
    for (int i = 0; i < 10; i++)
    {
        m_aChosen[i] = CPazaakCard(-1);
    }
    m_cTutorial.m_bEnabled = bTutorial;
    m_pPazaak = new CSWPazaak;
    m_pPazaak->m_nWager = nWager;
    m_pPazaak->m_oidOpponent = oidOpponent;
    if (nWager > 0)
    {
        CSWPazaak& cPazaak = *m_pPazaak;
        cPazaak.m_nWager = 1;
        m_pWagerPopup = new CSWGuiWagerPopup(pManager, this, nWager);
    }
    CPazaakCard aSideDeck[10];
    m_pPazaak->LoadInitialSideDeck(nSideDeck, aSideDeck);
    CSWPazaak* pPazaak = m_pPazaak;
    pPazaak->m_cOpponent.ChooseSidedeck(aSideDeck);

    StartLoadFromLayout("pazaaksetup");
    InitControl(m_lblTitle, "LBL_TITLE", 1);
#ifdef _XBOX
    InitControl(m_lblYbutton, "LBL_YBUTTON", 1);
    InitControl(m_btnMove, "LBL_YTEXT", 1);
    InitControl(m_lblAbutton, "LBL_ABUTTON", 1);
    InitControl(m_btnAtext, "LBL_ATEXT", 1);
    InitControl(m_lblLbutton, "LBL_LBUTTON", 1);
#else
    InitControl(m_btnAtext, "BTN_ATEXT", 1);
    InitControl(m_btnClearCards, "BTN_CLEARCARDS", 1);
#endif
    InitControl(m_lblLtext, "LBL_LTEXT", 1);
#ifdef _XBOX
    InitControl(m_lblRbutton, "LBL_RBUTTON", 1);
#endif
    InitControl(m_lblRtext, "LBL_RTEXT", 1);
    char szChosenButton[16];
    char szChosenLabel[16];
    strcpy(szChosenButton, "BTN_CHOSENx    ");
    strcpy(szChosenLabel, "LBL_CHOSENx    ");
    for (int i = 0; i < 10; i++)
    {
        sprintf(&szChosenButton[10], "%d", i);
        sprintf(&szChosenLabel[10], "%d", i);
        InitControl(m_cChosen[i], szChosenButton, 1);
        InitControl(m_cChosen[i].m_lblCard, szChosenLabel, 1);
        m_cChosen[i].m_nID = i;
        m_cChosen[i].AddEvent(
            GUIEVENT_MOUSE_ENTER, this, (CSWGuiHandler)&CSWGuiPazaakStart::OnAvailCardEntered);
#ifndef _XBOX
        m_cChosen[i].AddEvent(
            CSWGuiPazaakCard::EVENT_CARD_DROP, this,
            (CSWGuiHandler)&CSWGuiPazaakStart::HandleChosenCardDrop);
#endif
#ifndef _XBOX
        m_cChosen[i].AddEvent(
            GUIEVENT_ACTIVATE, this, (CSWGuiHandler)&CSWGuiPazaakStart::HandleMoveCard);
#endif
    }
    char szAvailButton[16];
    char szAvailLabel[16];
    char szAvailCount[16];
    strcpy(szAvailButton, "BTN_AVAILxx    ");
    strcpy(szAvailLabel, "LBL_AVAILxx    ");
    strcpy(szAvailCount, "LBL_AVAILNUMxx ");
    for (int i = 0; i < AvailCardCount; i++)
    {
        CSWGuiLabel* pCount = &m_lblAvailCount[i];
        int nRow = i / 6;
        int nColumn = i % 6;
        sprintf(&szAvailButton[9], "%d%d", nRow, nColumn);
        sprintf(&szAvailLabel[9], "%d%d", nRow, nColumn);
        sprintf(&szAvailCount[12], "%d%d", nRow, nColumn);
        InitControl(m_cAvail[i], szAvailButton, 1);
        InitControl(m_cAvail[i].m_lblCard, szAvailLabel, 1);
        InitControl(*pCount, szAvailCount, 1);
        m_cAvail[i].m_nID = i;
        m_cAvail[i].AddEvent(
            GUIEVENT_MOUSE_ENTER, this, (CSWGuiHandler)&CSWGuiPazaakStart::OnAvailCardEntered);
#ifndef _XBOX
        m_cAvail[i].AddEvent(
            CSWGuiPazaakCard::EVENT_CARD_DROP, this,
            (CSWGuiHandler)&CSWGuiPazaakStart::HandleAvailCardDrop);
#endif
#ifndef _XBOX
        m_cAvail[i].AddEvent(GUIEVENT_ACTIVATE, this,
                             (CSWGuiHandler)&CSWGuiPazaakStart::HandleMoveCard);
#endif
#ifndef _XBOX
        pCount->m_bIgnoreMouse = 1;
#endif
    }
#ifndef _XBOX
    InitControl(m_lblHelp, "LBL_HELP", 1);
#endif
    StopLoadFromLayout();
#ifndef _XBOX
    m_btnAtext.AddEvent(GUIEVENT_ACTIVATE, this,
                         (CSWGuiHandler)&CSWGuiPazaakStart::HandleAcceptButton);
#endif
#ifndef _XBOX
    m_nBackground = PC_BACKGROUND_SUFFIX_PAZAAK;
#endif
}
// PC 0x006D5D10
void CSWGuiPazaakStart::OnPanelAdded()
{
    int i;
    if (m_pWagerPopup != NULL)
    {
        SetActiveControl(NULL, 0);
        m_pManager->AddPanel(m_pWagerPopup, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
    }
    CSWPartyTable* pPartyTable =
        g_pAppManager->m_pServerExoApp->GetPartyTable();
    for (i = 0; i < AvailCardCount; i++)
    {
        m_anAvailCount[i] = pPartyTable->m_anPazaakCards[i];
        if (m_anAvailCount[i] == 0)
        {
            m_anAvailCount[i] = -1;
            m_cAvail[i].SetCard(-1);
        }
    }
    m_bSideDeckFull = 1;
    for (i = 0; i < 10; i++)
    {
        m_aChosen[i] = CPazaakCard(pPartyTable->m_anPazaakSideDeck[i]);
        int nCard = m_aChosen[i].m_nType;
#ifdef _XBOX
        if (m_anAvailCount[nCard] > 0)
        {
            m_anAvailCount[nCard] = m_anAvailCount[nCard] - 1;
        }
        else
        {
            m_aChosen[i] = CPazaakCard(-1);
        }
#else
        if (nCard < 0 || m_anAvailCount[nCard] <= 0)
        {
            m_aChosen[i] = CPazaakCard(-1);
        }
        else
        {
            m_anAvailCount[nCard] = m_anAvailCount[nCard] - 1;
        }
#endif
        m_cChosen[i].SetCard(m_aChosen[i]);
        if (m_aChosen[i].m_nType < 0)
        {
            m_bSideDeckFull = 0;
        }
    }
    for (i = 0; i < AvailCardCount; i++)
    {
        if (m_anAvailCount[i] >= 0)
        {
            m_lblAvailCount[i].m_cText.m_cParams.SetText(
                CExoString(m_anAvailCount[i]));
        }
#ifndef _XBOX
        if (m_anAvailCount[i] == 0)
        {
            m_cAvail[i].SetDimmed(1);
#ifndef _XBOX
            m_cAvail[i].m_bDraggable = 0;
#endif
        }
#endif
    }
    for (i = 0; i < 10; i++)
    {
        m_cChosen[i].SetMoveToControl(1, &m_cChosen[(i + 9) % 10]);
        m_cChosen[i].SetMoveToControl(3, &m_cChosen[(i + 1) % 10]);
        m_cChosen[i].SetMoveToControl(0, &m_cChosen[(i + 8) % 10]);
        m_cChosen[i].SetMoveToControl(2, &m_cChosen[(i + 2) % 10]);
    }
    for (i = 0; i < AvailCardCount; i++)
    {
        m_cAvail[i].SetMoveToControl(0, &m_cAvail[(i + 18) % AvailCardCount]);
        m_cAvail[i].SetMoveToControl(2, &m_cAvail[(i + 6) % AvailCardCount]);
        m_cAvail[i].SetMoveToControl(1, &m_cAvail[6 * (i / 6) + (i + 5) % 6]);
        m_cAvail[i].SetMoveToControl(3, &m_cAvail[6 * (i / 6) + (i + 1) % 6]);
    }
    if (m_pWagerPopup == NULL)
    {
        SetActiveControl(&m_cAvail[0], 0);
    }
    ShowDisabledButtons();
    unsigned long aHelp[3];
    aHelp[0] = 38627;
    aHelp[1] = 38628;
    aHelp[2] = 38629;
    m_cTutorial.ShowMultipleHelp(aHelp, 3);
#ifndef _XBOX
    CSWGuiPanel::OnPanelAdded();
#endif
}
#ifndef _XBOX
int CSWGuiPazaakStart::AddChosenCard(int nAvail, int nSlot)  // PC 0x006D6190
{
    if (m_anAvailCount[nAvail] < 1)
    {
        return 0;
    }
    if (nSlot < 0)
    {
        for (nSlot = 0; nSlot < ChosenCardCount; nSlot++)
        {
            if (m_aChosen[nSlot].m_nType < 0)
            {
                break;
            }
        }
    }
    if (nSlot >= ChosenCardCount)
    {
        return 0;
    }
    CPazaakCard cCard;
    cCard.m_nType = nAvail;
    m_aChosen[nSlot] = cCard;
    m_anAvailCount[nAvail] = m_anAvailCount[nAvail] - 1;
    {
        CExoString sCount(m_anAvailCount[nAvail]);
        m_lblAvailCount[nAvail].m_cText.m_cParams.SetText(sCount);
    }
    CResRef cImage = m_cAvail[nAvail].m_cBorder.m_cParams.m_cFillImage;
    m_cChosen[nSlot].m_cBorder.m_cParams.SetFillImage(cImage, 0);
    if (m_anAvailCount[nAvail] == 0)
    {
        m_cAvail[nAvail].SetDimmed(1);
#ifndef _XBOX
        m_cAvail[nAvail].m_bDraggable = 0;
#endif
    }
    CExoString sText = m_cAvail[nAvail].m_lblCard.m_cText.m_cParams.m_sText;
    m_cChosen[nSlot].m_lblCard.m_cText.m_cParams.SetText(sText);
    m_bSideDeckFull = 1;
    for (int i = 0; i < ChosenCardCount; i++)
    {
        int nType = m_aChosen[i].m_nType;
        if (nType < 0)
        {
            m_bSideDeckFull = 0;
            break;
        }
    }
    ShowDisabledButtons();
    return 1;
}
#endif
int CSWGuiPazaakStart::RemoveChosenCard(int nSlot)  // PC 0x006D6390
{
    int nCard = m_aChosen[nSlot].m_nType;
    if (nCard < 0)
    {
        return 0;
    }
    m_bSideDeckFull = 0;
    ShowDisabledButtons();
    CPazaakCard cEmpty;
    cEmpty.m_nType = -1;
    cEmpty.m_bFlipped = 0;
    m_aChosen[nSlot] = cEmpty;
    m_anAvailCount[nCard] = m_anAvailCount[nCard] + 1;
    {
        CExoString sCount(m_anAvailCount[nCard]);
        m_lblAvailCount[nCard].m_cText.m_cParams.SetText(sCount);
    }
#ifndef _XBOX
    m_cAvail[nCard].SetDimmed(0);
#endif
#ifndef _XBOX
    m_cAvail[nCard].m_bDraggable = 1;
#endif
    m_cChosen[nSlot].m_cBorder.m_cParams.SetFillImage("", 0);
    m_cChosen[nSlot].m_lblCard.m_cText.m_cParams.SetText("");
    return 1;
}
// PC 0x006D4AF0
CSWGuiPazaakStart::~CSWGuiPazaakStart()
{
    if (m_pWagerPopup != NULL)
    {
        delete m_pWagerPopup;
        m_pWagerPopup = NULL;
    }
    if (m_pPazaak != NULL)
    {
        delete m_pPazaak;
        m_pPazaak = NULL;
    }
}

#ifdef _XBOX

void CSWGuiPazaakStart::ShowDisabledButtons()
{
    m_btnAtext.SetActivated(m_bSideDeckFull);
    if (m_pActiveControl != NULL)
    {
        int nIndex = m_pActiveControl->m_nID;
        if (m_bChosenActive == 0)
        {
            int bEnabled = m_bSideDeckFull == 0;
            if (&m_cAvail[nIndex] == m_pActiveControl &&
                m_anAvailCount[nIndex] < 1)
            {
                bEnabled = 0;
            }
            m_btnMove.SetActivated(bEnabled);
        }
        else
        {
            int bEnabled = 1;
            if (&m_cChosen[nIndex] == m_pActiveControl &&
                m_aChosen[nIndex].m_nType < 0)
            {
                bEnabled = 0;
            }
            m_btnMove.SetActivated(bEnabled);
        }
    }
    else
    {
        m_btnMove.SetActivated(0);
    }
}
#else
void CSWGuiPazaakStart::ShowDisabledButtons()  // PC 0x006D4C50
{
    m_btnAtext.SetEnabled(m_bSideDeckFull);
}
#endif

void CSWGuiPazaakStart::OnChosenCardEntered(CSWGuiControl* pControl)
{
    ShowDisabledButtons();
}

void CSWGuiPazaakStart::OnAvailCardEntered(CSWGuiControl* pControl)
{
    ShowDisabledButtons();
}

#ifndef _XBOX
void CSWGuiPazaakStart::HandleAvailCardDrop(CSWGuiControl* pControl)  // PC 0x006D64F0
{
    int nAvail = pControl->m_nID;
    CSWGuiControl* pDropTarget = g_pGuiMan->m_pMouseOverControl;
    if (pDropTarget != NULL)
    {
        int nChosen = pDropTarget->m_nID;
        if (nChosen >= 0 && nChosen < 10 && pDropTarget == &m_cChosen[nChosen])
        {
            RemoveChosenCard(nChosen);
            AddChosenCard(nAvail, nChosen);
        }
    }
}
#endif

#ifndef _XBOX
void CSWGuiPazaakStart::HandleChosenCardDrop(CSWGuiControl* pControl)  // PC 0x006D6540
{
    int nSlot = pControl->m_nID;
    int nSlotCard = m_aChosen[nSlot].m_nType;
    CSWGuiControl* pDropTarget = g_pGuiMan->m_pMouseOverControl;
    if (pDropTarget != NULL)
    {
        int nTarget = pDropTarget->m_nID;
        if (nTarget >= 0)
        {
            int nSource;
            if (nTarget < 10 && pDropTarget == &m_cChosen[nTarget])
            {
                nSource = 2;
            }
            else
            {
                if (nTarget >= AvailCardCount || pDropTarget != &m_cAvail[nTarget])
                {
                    return;
                }
                nSource = 1;
            }
            RemoveChosenCard(nSlot);
            if (nSource == 2)
            {
                int nTargetCard = m_aChosen[nTarget].m_nType;
                RemoveChosenCard(nTarget);
                if (nTargetCard >= 0)
                {
                    AddChosenCard(nTargetCard, nSlot);
                }
                if (nSlotCard >= 0)
                {
                    AddChosenCard(nSlotCard, nTarget);
                }
            }
        }
    }
}
#endif
// PC 0x006D7950
CSWGuiPazaakGame::CSWGuiPazaakGame(CSWGuiManager* pManager, CSWPazaak* pPazaak,
                                   int bTutorial, unsigned long oidOpponent)
    : CSWGuiPanel(pManager),
      m_cMessageBox(pManager),
      m_cTutorial(pManager)
{
#ifndef _XBOX
    m_pPazaak = pPazaak;
#endif
    m_szStartTurnSound = "mgs_startturn";
    m_szDrawMainSound = "mgs_drawmain";
    m_szPlaySideSound = "mgs_playside";
    m_szWarnBustSound = "mgs_warnbust";
#ifdef _XBOX
    m_pPazaak = pPazaak;
#endif
    m_nState = STATE_START_GAME;
    m_fTimer = -1.0f;
    m_bUnknown86DC0 = 0;
    m_bShownFirstCardHelp = 0;
    m_bShownOpponentTurnHelp = 0;
    m_bShownGameStartHelp = 0;
    m_bShownTwentyHelp = 0;
    m_nUnknown86DC5 = 0;
#ifndef _XBOX
    m_nPlayedCard = -1;
#endif
    m_szEndSound = NULL;
#ifndef _XBOX
    m_nHandAction = 0;
#endif
    m_cTutorial.SetCallback(
        this, (CSWGuiHandler)&CSWGuiPazaakGame::HandleTutorialClose, NULL);
    m_cTutorial.m_bEnabled = bTutorial;
    m_cMessageBox.SetAllowCancel(0);

    StartLoadFromLayout("pazaakgame");
    InitControl(m_lblPlayerName, "LBL_PLRNAME", 1);
    InitControl(m_lblNpcName, "LBL_NPCNAME", 1);
    InitControl(m_lblPlayerTotal, "LBL_PLRTOTAL", 1);
    InitControl(m_lblNpcTotal, "LBL_NPCTOTAL", 1);
    InitControl(m_lblPlayerSideDeck, "LBL_PLRSIDEDECK", 1);
    InitControl(m_lblNpcSideDeck, "LBL_NPCSIDEDECK", 1);
    InitControl(m_lblPlayerTurn, "LBL_PLRTURN", 1);
    InitControl(m_lblNpcTurn, "LBL_NPCTURN", 1);
    m_lblPlayerTurn.SetShown(0);
    m_lblNpcTurn.SetShown(0);
    InitControl(m_lblPlayerScore[0], "LBL_PLRSCORE0", 1);
    InitControl(m_lblPlayerScore[1], "LBL_PLRSCORE1", 1);
    InitControl(m_lblPlayerScore[2], "LBL_PLRSCORE2", 1);
    InitControl(m_lblNpcScore[0], "LBL_NPCSCORE0", 1);
    InitControl(m_lblNpcScore[1], "LBL_NPCSCORE1", 1);
    InitControl(m_lblNpcScore[2], "LBL_NPCSCORE2", 1);
#ifndef _XBOX
    InitControl(m_lblFlipIcon, "LBL_FLIPICON", 1);
    InitControl(m_lblFlipLegend, "LBL_FLIPLEGEND", 1);
    InitControl(m_lblChangeIcon, "LBL_CHANGEICON", 1);
    InitControl(m_lblChangeLegend, "LBL_CHANGELEGEND", 1);
    InitControl(m_btnForfeitGame, "BTN_FORFEITGAME", 1);
#endif
    int i;
    {
        char szPlayerButton[16];
        char szNpcButton[16];
        char szPlayerLabel[16];
        char szNpcLabel[16];
        strcpy(szPlayerButton, "BTN_PLRx       ");
        strcpy(szNpcButton, "BTN_NPCx       ");
        strcpy(szPlayerLabel, "LBL_PLRx       ");
        strcpy(szNpcLabel, "LBL_NPCx       ");
        for (i = 0; i < 9; i++)
        {
#ifndef _XBOX
            CSWGuiPazaakCard* pNpcCard = &m_cNpcBoard[i];
#endif
            sprintf(&szPlayerButton[7], "%d", i);
            sprintf(&szNpcButton[7], "%d", i);
            sprintf(&szPlayerLabel[7], "%d", i);
            sprintf(&szNpcLabel[7], "%d", i);
            InitControl(m_cPlayerBoard[i], szPlayerButton, 1);
            InitControl(m_cPlayerBoard[i].m_lblCard, szPlayerLabel, 1);
#ifdef _XBOX
            InitControl(m_cNpcBoard[i], szNpcButton, 1);
            InitControl(m_cNpcBoard[i].m_lblCard, szNpcLabel, 1);
#else
            InitControl(*pNpcCard, szNpcButton, 1);
            InitControl(pNpcCard->m_lblCard, szNpcLabel, 1);
#endif
#ifndef _XBOX
            pNpcCard->m_bIgnoreMouse = 1;
#endif
#ifndef _XBOX
            m_cPlayerBoard[i].SetEnabled(0);
#endif
#ifndef _XBOX
            m_cPlayerBoard[i].m_bDraggable = 0;
#endif
        }
    }
    {
        char szHandButton[16];
        char szNpcHandButton[16];
        char szHandLabel[16];
        char szNpcHandLabel[16];
#ifndef _XBOX
        char szFlipButton[16];
#endif
        strcpy(szHandButton, "BTN_PLRSIDEx   ");
        strcpy(szNpcHandButton, "BTN_NPCSIDEx   ");
        strcpy(szHandLabel, "LBL_PLRSIDEx   ");
        strcpy(szNpcHandLabel, "LBL_NPCSIDEx   ");
#ifndef _XBOX
        strcpy(szFlipButton, "BTN_FLIPx      ");
#endif
        for (i = 0; i < 4; i++)
        {
            sprintf(&szHandButton[11], "%d", i);
            sprintf(&szNpcHandButton[11], "%d", i);
            sprintf(&szHandLabel[11], "%d", i);
            sprintf(&szNpcHandLabel[11], "%d", i);
#ifndef _XBOX
            sprintf(&szFlipButton[8], "%d", i);
#endif
            InitControl(m_cPlayerHand[i], szHandButton, 1);
            InitControl(m_cPlayerHand[i].m_lblCard, szHandLabel, 1);
#ifndef _XBOX
            InitControl(m_btnFlip[i], szFlipButton, 1);
#endif
            m_cPlayerHand[i].m_nID = i;
#ifndef _XBOX
            m_btnFlip[i].m_nID = i;
#endif
            InitControl(m_cNpcHand[i], szNpcHandButton, 1);
            InitControl(m_cNpcHand[i].m_lblCard, szNpcHandLabel, 1);
#ifdef _XBOX
            m_cPlayerHand[i].AddEvent(
                GUIEVENT_MOUSE_ENTER, this, (CSWGuiHandler)&CSWGuiPazaakGame::OnHandCardEntered);
#else
            m_cPlayerHand[i].AddEvent(
                GUIEVENT_ACTIVATE, this, (CSWGuiHandler)&CSWGuiPazaakGame::HandleHandcardAction);
#ifndef _XBOX
            m_cPlayerHand[i].AddEvent(
                98283, this, (CSWGuiHandler)&CSWGuiPazaakGame::HandleHandCardDrop);
#endif
            m_cPlayerHand[i].AddEvent(
                505, this,
                (CSWGuiHandler)&CSWGuiPazaakGame::HandleHandCardDoubleClick);
            m_cPlayerHand[i].AddEvent(
                68, this,
                (CSWGuiHandler)&CSWGuiPazaakGame::HandleHandCardRightClick);
#ifndef _XBOX
            m_cNpcHand[i].m_bIgnoreMouse = 1;
#endif
            m_btnFlip[i].AddEvent(
                GUIEVENT_ACTIVATE, this, (CSWGuiHandler)&CSWGuiPazaakGame::HandleFlipButtons);
#endif
        }
    }
#ifdef _XBOX
    InitControl(m_lblXbutton, "LBL_XBUTTON", 1);
    InitControl(m_btnContinue, "LBL_XTEXT", 1);
    InitControl(m_lblYbutton, "LBL_YBUTTON", 1);
    InitControl(m_btnStand, "LBL_YTEXT", 1);
    InitControl(m_lblAbutton, "LBL_ABUTTON", 1);
    InitControl(m_btnX, "LBL_ATEXT", 1);
    InitControl(m_lblBbutton, "LBL_BBUTTON", 1);
    InitControl(m_btnBlack, "LBL_BTEXT", 1);
#else
    InitControl(m_btnContinue, "BTN_XTEXT", 1);
    InitControl(m_btnStand, "BTN_YTEXT", 1);
#endif
    StopLoadFromLayout();
#ifndef _XBOX
    m_btnContinue.AddEvent(GUIEVENT_ACTIVATE, this,
                           (CSWGuiHandler)&CSWGuiPazaakGame::HandleContinue);
    m_btnStand.AddEvent(GUIEVENT_ACTIVATE, this,
                        (CSWGuiHandler)&CSWGuiPazaakGame::HandleStand);
    m_btnX.AddEvent(GUIEVENT_ACTIVATE, this, (CSWGuiHandler)&CSWGuiPazaakGame::HandleXButton);
    m_btnBlack.AddEvent(GUIEVENT_ACTIVATE, this,
                        (CSWGuiHandler)&CSWGuiPazaakGame::HandleBlackButton);
#endif
    CSWSCreature* pCreature =
        g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(oidOpponent);
    if (pCreature != NULL)
    {
        m_lblNpcName.m_cText.m_cParams.SetText(
            pCreature->GetStats()->GetFullName());
    }
#ifndef _XBOX
    if (m_btnContinue.m_cText.GetIdealHeight() >
        m_btnContinue.m_cExtent.m_nHeight)
    {
        m_btnContinue.m_cText.m_cParams.SetAlignment(10);
    }
    if (m_btnStand.m_cText.GetIdealHeight() > m_btnStand.m_cExtent.m_nHeight)
    {
        m_btnStand.m_cText.m_cParams.SetAlignment(10);
    }
#endif
#ifndef _XBOX
    m_nBackground = PC_BACKGROUND_SUFFIX_PAZAAK;
#endif
}

#ifdef _XBOX

void CSWGuiPazaakStart::HandleInputEvent(int nEvent, int nState)
{
    if (nState != 0)
    {
        switch (nEvent)
        {
        case 39: case 45:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            if (m_bSideDeckFull != 0)
            {
                m_cMessageBox.SetMessage(32322);
                m_cMessageBox.SetCallback(
                    this, (CSWGuiHandler)&CSWGuiPazaakStart::HandleStartDialog,
                    NULL);
                m_pManager->AddPanel(&m_cMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
            }
            else
            {
                CSWGuiMessageBox* pMessageBox =
                    g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
                pMessageBox->SetAllowCancel(0);
                pMessageBox->SetCallback(NULL, NULL, NULL);
                pMessageBox->SetMessage(38630);
                m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
            }
            break;
        case 40:
        case 46:
        {
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            CSWGuiMessageBox* pMessageBox =
                g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
            pMessageBox->SetAllowCancel(1);
            pMessageBox->SetCallback(
                this, (CSWGuiHandler)&CSWGuiPazaakStart::HandleQuitDialog,
                NULL);
            pMessageBox->SetMessage(42424);
            m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
            break;
        }
        case 42:
        {
            int nIndex = m_pActiveControl->m_nID;
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            int nChanged = 0;
            if (m_bChosenActive != 0)
            {
                if (&m_cChosen[nIndex] == m_pActiveControl)
                {
                    nChanged = RemoveChosenCard(nIndex);
                }
            }
            else
            {
                if (&m_cAvail[nIndex] == m_pActiveControl)
                {
                    nChanged = AddChosenCard(nIndex);
                }
            }
            if (nChanged != 0)
            {
                m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            }
            ShowDisabledButtons();
            break;
        }
        case 53:
            if (m_bChosenActive != 0)
            {
                CSWGuiControl* pActive = m_pActiveControl;
                m_bChosenActive = 0;
                SetActiveControl(m_pChosenCards, 0);
                m_pChosenCards = pActive;
                m_btnMove.m_cText.m_cParams.SetStrRef(38601);
                m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
            }
            break;
        case 54:
            if (m_bChosenActive == 0)
            {
                CSWGuiControl* pActive = m_pActiveControl;
                m_bChosenActive = 1;
                SetActiveControl(m_pChosenCards, 0);
                m_pChosenCards = pActive;
                m_btnMove.m_cText.m_cParams.SetStrRef(38602);
                m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
            }
            break;
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
#else
void CSWGuiPazaakStart::HandleInputEvent(int nEvent, int nState)  // PC 0x006D7790
{
    if (nState != 0)
    {
        int nCode = nEvent;
        if (nEvent == 186)
        {
            if (ClearSideDeck() != 0)
            {
                nCode = 53;
            }
            else
            {
                nCode = 54;
            }
        }
        switch (nCode)
        {
        case 40:
        case 46:
        {
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            CSWGuiMessageBox* pMessageBox =
                g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
            pMessageBox->SetAllowCancel(1);
            pMessageBox->SetCallback(
                this, (CSWGuiHandler)&CSWGuiPazaakStart::HandleQuitDialog,
                NULL);
            pMessageBox->SetMessage(42424);
            m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
            break;
        }
        case 42:
            HandleMoveCard(m_pActiveControl);
            break;
        case 53:
            if (ClearSideDeck() != 0)
            {
                m_pChosenCards = m_pActiveControl;
            }
            if (IsAvailCardActive() != 0)
            {
                break;
            }
            SetActiveControl(m_pAvailCards, 0);
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
            break;
        case 54:
            if (IsAvailCardActive() != 0)
            {
                m_pAvailCards = m_pActiveControl;
            }
            if (ClearSideDeck() != 0)
            {
                break;
            }
            SetActiveControl(m_pChosenCards, 0);
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
            break;
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
#endif
// PC 0x006D8A90
void CSWGuiPazaakStart::HandleStartDialog(CSWGuiControl* pControl)
{
    if (m_cMessageBox.m_bAccepted)
    {
        CSWPartyTable* pPartyTable =
            g_pAppManager->GetServerExoApp()->GetPartyTable();
        for (int i = 0; i < 10; i++)
        {
            pPartyTable->m_anPazaakSideDeck[i] = m_aChosen[i].m_nType;
        }
        CSWPazaak* pPazaak = m_pPazaak;
        pPazaak->m_cPlayer.ChooseSidedeck(m_aChosen);
        m_pManager->PopModalPanel();
        m_bDeferredRemoval = 0;
        m_nExitState = EXITSTATE_DELETE;
        CSWGuiPazaakGame* pGame =
            new CSWGuiPazaakGame(m_pManager, m_pPazaak, m_cTutorial.GetEnabled(),
                                 m_pPazaak->GetOpponent());
        m_pPazaak = NULL;
        m_pManager->AddPanel(pGame, CSWGuiManager::PANEL_MODE_MODAL, 1);
    }
}
// PC 0x006D8BE0
void CSWGuiPazaakStart::HandleAcceptButton(CSWGuiControl* pControl)
{
    if (pControl != NULL && pControl->m_bSelected != 0)
    {
        m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
        if (m_bSideDeckFull != 0)
        {
            m_cMessageBox.SetMessage(32322);
            m_cMessageBox.SetCallback(
                this, (CSWGuiHandler)&CSWGuiPazaakStart::HandleStartDialog,
                NULL);
            m_pManager->AddPanel(&m_cMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
        }
        else
        {
            CSWGuiMessageBox* pMessageBox =
                g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
            pMessageBox->SetAllowCancel(0);
            pMessageBox->SetCallback(NULL, NULL, NULL);
            pMessageBox->SetMessage(38630);
            m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
        }
    }
}
// PC 0x006D6DC0
void CSWGuiPazaakGame::OnPanelAdded()
{
    StartNewGame();
#ifdef _XBOX
    SetActiveControl(&m_cPlayerHand[3], 0);
#endif
    CServerExoApp* pServer = g_pAppManager->m_pServerExoApp;
    CSWSCreature* pCreature =
        pServer->GetPlayerCreature();
    CSWSCreatureStats* pStats = pCreature->m_pStats;
    m_lblPlayerName.m_cText.m_cParams.SetText(pStats->GetFullName());
    ReconnectSidedeckButtons();
#ifndef _XBOX
    CSWGuiPanel::OnPanelAdded();
#endif
}
// PC 0x006D6E50
int CSWGuiPazaakGame::DoGameSequence()
{
    int nResult = 0;
    switch (m_nState)
    {
    case 0:
        m_nState = 1;
        nResult = 1;
        break;
    case 1:
    {
        if (m_pPazaak->m_cPlayer.m_bStanding != 0)
        {
            m_nState = 5;
            nResult = 1;
            break;
        }
        m_lblPlayerTurn.SetEnabledFlag(1);
        m_lblNpcTurn.SetEnabledFlag(0);
        g_pExoSound->PlayOneShotSound(m_szStartTurnSound, CExoSound::PRIORITY_GROUP_GUI, 0, 0, 0.0f, 0.0f);
        m_nState = 2;
        m_fTimer = 0.4f;
        break;
    }
    case 2:
    {
        m_pPazaak->DrawPlayerCard();
        g_pExoSound->PlayOneShotSound(m_szDrawMainSound, CExoSound::PRIORITY_GROUP_GUI, 0, 0, 0.0f, 0.0f);
        if (m_bShownFirstCardHelp == 0)
        {
            m_bShownFirstCardHelp = 1;
            m_cTutorial.ShowHelp(38633, 0xFFFFFFFF, 38626);
            m_cTutorial.PlaceRightOf(&m_cPlayerBoard[0]);
        }
        if (m_pPazaak->m_cPlayer.m_bStanding != 0)
        {
            m_btnStand.StartFlash(CGuiInGame::COLOR_CYAN);
            m_fTimer = 0.4f;
        }
        if (m_pPazaak->GetPlayerTotal() > 20)
        {
            g_pExoSound->PlayOneShotSound(m_szWarnBustSound, CExoSound::PRIORITY_GROUP_GUI, 0, 0, 0.0f, 0.0f);
        }
        m_nState = 3;
#ifndef _XBOX
        CSWGuiControl* pActive = GetActiveControl();
        SetActiveControl(NULL, 0);
        SetActiveControl(pActive, 0);
#endif
        RefreshDisplay();
        break;
    }
    case 3:
    case 4:
        if (m_pPazaak->m_cPlayer.m_bStanding == 0)
        {
            break;
        }
        m_nState = 5;
        nResult = 1;
        break;
    case 5:
    {
        int nTotal = m_pPazaak->GetPlayerTotal();
        if (nTotal > 20)
        {
            m_cTutorial.ShowHelp(38647, 0xFFFFFFFF, 38626);
        }
        else if (nTotal == 20 && m_bShownTwentyHelp == 0)
        {
            m_bShownTwentyHelp = 1;
            m_cTutorial.ShowHelp(38646, 0xFFFFFFFF, 38626);
        }
        if (m_pPazaak->DetermineWinner() != 0)
        {
            m_nState = 8;
            nResult = 1;
            break;
        }
        m_nState = 6;
    }
    case 6:
    {
        if (m_pPazaak->m_cOpponent.m_bStanding != 0)
        {
            m_nState = 8;
            nResult = 1;
            break;
        }
        m_lblPlayerTurn.SetEnabledFlag(0);
        m_lblNpcTurn.SetEnabledFlag(1);
        g_pExoSound->PlayOneShotSound(m_szStartTurnSound, CExoSound::PRIORITY_GROUP_GUI, 0, 0, 0.0f, 0.0f);
        m_fTimer = 0.4f;
        m_nState = 7;
        if (m_bShownOpponentTurnHelp != 0)
        {
            break;
        }
        m_bShownOpponentTurnHelp = 1;
        m_cTutorial.ShowHelp(38645, 0xFFFFFFFF, 38626);
        break;
    }
    case 7:
    {
        int nAction = m_pPazaak->PlayOpponentTurn();
        switch (nAction)
        {
        case 1:
        {
            g_pExoSound->PlayOneShotSound(m_szDrawMainSound, CExoSound::PRIORITY_GROUP_GUI, 0, 0, 0.0f, 0.0f);
            CSWPazaak* pPazaak = m_pPazaak;
            if (pPazaak->m_cOpponent.GetTotal() > 20)
            {
                g_pExoSound->PlayOneShotSound(m_szWarnBustSound, CExoSound::PRIORITY_GROUP_GUI, 0, 0, 0.0f, 0.0f);
            }
            break;
        }
        case 2:
        {
            m_btnX.StartFlash(CGuiInGame::COLOR_RED);
            g_pExoSound->PlayOneShotSound(m_szPlaySideSound, CExoSound::PRIORITY_GROUP_GUI, 0, 0, 0.0f, 0.0f);
            break;
        }
        case 3:
            m_btnStand.StartFlash(CGuiInGame::COLOR_RED);
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            break;
        case 4:
            m_btnContinue.StartFlash(CGuiInGame::COLOR_RED);
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            break;
        default:
            break;
        }
        if (nAction == 1 || nAction == 2)
        {
            m_fTimer = 0.8f;
            CSWPazaak* pPazaak = m_pPazaak;
            if (pPazaak->m_cOpponent.GetTotal() == 20)
            {
                if (m_bShownTwentyHelp == 0)
                {
                    m_bShownTwentyHelp = 1;
                    m_cTutorial.ShowHelp(38646, 0xFFFFFFFF, 38626);
                }
            }
        }
        if (nAction != 0)
        {
            RefreshDisplay();
            break;
        }
        m_fTimer = 0.4f;
        m_nState = 8;
    }
    case 8:
    {
        int nWinner = m_pPazaak->DetermineWinner();
        if (nWinner != 0)
        {
            m_cMessageBox.SetAllowCancel(0);
            if (nWinner == 1)
            {
                m_pPazaak->m_cPlayer.m_nWins++;
                if (m_pPazaak->m_cPlayer.m_nWins >= 3)
                {
                    m_cMessageBox.SetMessage(32336);
                    m_szEndSound = "mgs_winmatch";
                    m_nState = 9;
                }
                else
                {
                    m_cMessageBox.SetMessage(32334);
                    m_szEndSound = "mgs_winset";
                    m_nState = 9;
                }
            }
            else if (nWinner == 2)
            {
                m_pPazaak->m_cOpponent.m_nWins++;
                if (m_pPazaak->m_cOpponent.m_nWins >= 3)
                {
                    m_cMessageBox.SetMessage(32337);
                    m_szEndSound = "mgs_losematch";
                    m_nState = 9;
                }
                else
                {
                    m_cMessageBox.SetMessage(32335);
                    m_szEndSound = "mgs_loseset";
                    m_nState = 9;
                }
            }
            else
            {
                m_cMessageBox.SetMessage(32338);
                m_nState = 9;
            }
        }
        else
        {
            m_nState = 1;
        }
        RefreshDisplay();
        break;
    }
    case 9:
    {
        if (m_szEndSound != 0)
        {
            g_pExoSound->PlayOneShotSound(m_szEndSound, CExoSound::PRIORITY_GROUP_GUI, 0, 0, 0.0f, 0.0f);
            m_szEndSound = 0;
        }
        m_cMessageBox.SetCallback(
            this, (CSWGuiHandler)&CSWGuiPazaakGame::HandleEndDialog, NULL);
        m_pManager->AddPanel(&m_cMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
        m_nState = 10;
        RefreshDisplay();
        break;
    }
    case 11:
        StartNewGame();
        break;
    case 12:
        m_bDeferredRemoval = 0;
        m_nExitState = EXITSTATE_DELETE;
        g_pAppManager->m_pClientExoApp->EndPazaakGame(
            m_pPazaak->m_cPlayer.m_nWins > m_pPazaak->m_cOpponent.m_nWins,
            m_pPazaak->m_nWager, 0, m_pPazaak->m_oidOpponent);
        break;
    default:
        break;
    }
    return nResult;
}
// PC 0x006D76C0
void CSWGuiPazaakGame::Draw(float fElapsed)
{
    if (m_fTimer >= 0.0f)
    {
        m_fTimer = m_fTimer - fElapsed;
        if (m_fTimer < 0.0f)
        {
            RefreshDisplay();
        }
    }
    else
    {
        while (m_pManager->IsOnTop(this) != 0 && DoGameSequence() != 0)
        {
        }
    }
    return CSWGuiPanel::Draw(fElapsed);
}

#ifndef _XBOX
inline void CSWGuiPazaakGame::CancelHandAction()
{
    m_nHandAction = 0;
    SetHandEnabled(0);
}
// PC 0x006D7740
void CSWGuiPazaakGame::HandleHandcardAction(CSWGuiControl* pControl)
{
    if (pControl == NULL || pControl->m_bSelected != 0)
    {
        int nAction = m_nHandAction;
        if (nAction == 1)
        {
            HandlePlayHandCard(pControl->m_nID);
        }
        else if (nAction == 2)
        {
            HandleFlipHandCard();
        }
        m_nHandAction = 0;
        SetHandEnabled(0);
    }
}
#endif
#ifndef _XBOX
void CSWGuiPazaakStart::HandleMoveCard(CSWGuiControl* pControl)  // PC 0x006D6D00
{
    if (pControl != NULL && pControl->m_bSelected != 0)
    {
        int nIndex = pControl->m_nID;
        m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
        CSWGuiControl* pActive = m_pActiveControl;
        int nChanged;
        if (ClearSideDeck() != 0)
        {
            nChanged = 0;
            if (&m_cChosen[nIndex] == pActive)
            {
                nChanged = RemoveChosenCard(nIndex);
            }
        }
        else
        {
            nChanged = 0;
            if (IsAvailCardActive() != 0 && &m_cAvail[nIndex] == pActive)
            {
                nChanged = AddChosenCard(nIndex, -1);
            }
        }
        if (nChanged != 0)
        {
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
        }
        ShowDisabledButtons();
    }
}
#endif
CSWGuiPazaakGame::~CSWGuiPazaakGame()  // PC 0x006D4E70
{
    if (m_pPazaak != NULL)
    {
        delete m_pPazaak;
        m_pPazaak = NULL;
    }
}
// PC 0x006D6640
void CSWGuiPazaakGame::StartNewGame()
{
    m_pPazaak->ClearGameBoard();
    m_nState = 0;
    RefreshDisplay();
    m_bShownTwentyHelp = 0;
    if (m_bShownGameStartHelp == 0)
    {
        m_bShownGameStartHelp = 1;
        unsigned long aHelp[2];
        aHelp[0] = 38631;
        aHelp[1] = 38632;
        m_cTutorial.ShowMultipleHelp(aHelp, 2);
    }
#ifndef _XBOX
    m_nHandAction = 0;
    SetHandEnabled(0);
    SetActiveControl(&m_btnContinue, 0);
#endif
}
// PC 0x006D3A90
int CSWGuiPazaakStart::ClearSideDeck()
{
    int nIndex = m_pActiveControl->m_nID;
    if (nIndex >= 0 && nIndex < 10)
    {
        return m_pActiveControl == &m_cChosen[nIndex];
    }
    return 0;
}

inline CSWPazaak* CSWGuiPazaakGame::GetPazaak() { return m_pPazaak; }

static inline int PazaakCardsDiffer(CPazaakCard cA, CPazaakCard cB)
{
    return cA.m_nType != cB.m_nType || cA.m_bFlipped != cB.m_bFlipped;
}

#define PAZAAK_REFRESH_DISABLED 0
// PC 0x006D3AC0
int CSWGuiPazaakGame::RefreshDisplay()
{
    if (PAZAAK_REFRESH_DISABLED)
    {
        return 0;
    }
    int bPlayerChanged = 0;
    int bNpcChanged = 0;
    int bPlayerStand = GetPazaak()->m_cPlayer.m_bStanding;
    int bNpcStand = GetPazaak()->m_cOpponent.m_bStanding;
    if (m_nState >= 10)
    {
        bPlayerStand = 0;
        bNpcStand = 0;
    }
    CPazaakCard cCard;
    for (int i = 0; i < 9; i++)
    {
        cCard = GetPazaak()->m_cPlayer.m_aBoard[i];
        if (PazaakCardsDiffer(m_cPlayerBoard[i].m_cCard, cCard))
        {
            m_cPlayerBoard[i].SetCard(cCard);
            bPlayerChanged = 1;
        }
        m_cPlayerBoard[i].SetDimmed(bPlayerStand);

        cCard = GetPazaak()->m_cOpponent.m_aBoard[i];
        if (PazaakCardsDiffer(m_cNpcBoard[i].m_cCard, cCard))
        {
            m_cNpcBoard[i].SetCard(cCard);
            bNpcChanged = 1;
        }
        m_cNpcBoard[i].SetDimmed(bNpcStand);
    }

#ifndef _XBOX
    int bAnyCard = 0;
    int bAnyFlip = 0;
#endif
    for (int i = 0; i < 4; i++)
    {
        cCard = GetPazaak()->m_cPlayer.m_aSideDeck[i];
#ifndef _XBOX
        bAnyFlip = bAnyFlip || (cCard.m_nType >= 12 && cCard.m_nType < 18);
#endif
        if (PazaakCardsDiffer(m_cPlayerHand[i].m_cCard, cCard))
        {
            m_cPlayerHand[i].SetCard(cCard);
#ifndef _XBOX
            m_btnFlip[i].SetEnabled(cCard.m_nType >= 12 && cCard.m_nType < 18);
            m_btnFlip[i].SetShown(cCard.m_nType >= 12 && cCard.m_nType < 18);
#endif
        }
#ifndef _XBOX
        m_cPlayerHand[i].SetDimmed(m_nState != 3);
        CPazaakCard cShown = m_cPlayerHand[i].m_cCard;
        if (cShown.m_nType >= 0)
        {
            bAnyCard = 1;
        }
#endif
        cCard = GetPazaak()->m_cOpponent.m_aSideDeck[i];
        if (g_bPazaakShowNPCSideDeck == 0)
        {
            if (cCard.m_nType >= 0)
            {
                cCard = CPazaakCard(-2);
            }
            if (PazaakCardsDiffer(m_cNpcHand[i].m_cCard, cCard))
            {
                m_cNpcHand[i].SetCard(cCard);
            }
        }
        else
        {
            if (PazaakCardsDiffer(m_cNpcHand[i].m_cCard, cCard))
            {
                m_cNpcHand[i].SetCard(cCard);
            }
        }
    }
#ifndef _XBOX
    m_lblFlipIcon.SetShown(bAnyFlip);
    m_lblFlipLegend.SetShown(bAnyFlip);
#endif
    if (bPlayerChanged != 0)
    {
        CExoString sTotal(GetPazaak()->GetPlayerTotal());
        m_lblPlayerTotal.m_cText.m_cParams.SetText(sTotal);
    }
    if (bNpcChanged != 0)
    {
        CExoString sTotal(GetPazaak()->GetOpponentTotal());
        m_lblNpcTotal.m_cText.m_cParams.SetText(sTotal);
    }
    if (m_nState == 10)
    {
        int nPlayerWin = GetPazaak()->m_cPlayer.m_nWins - 1;
        if (nPlayerWin >= 0 && nPlayerWin < 3)
        {
#ifdef _XBOX
            m_lblPlayerScore[nPlayerWin].m_cBorder.m_cParams.SetFillImage("lbl_winmark02", 0);
#else
            CResRef cMark("lbl_winmark02");
            m_lblPlayerScore[nPlayerWin].m_cBorder.m_cParams.SetFillImage(cMark,
                                                                          0);
#endif
        }
        int nNpcWin = GetPazaak()->m_cOpponent.m_nWins - 1;
        if (nNpcWin >= 0 && nNpcWin < 3)
        {
#ifdef _XBOX
            m_lblNpcScore[nNpcWin].m_cBorder.m_cParams.SetFillImage("lbl_winmark02", 0);
#else
            CResRef cMark("lbl_winmark02");
            m_lblNpcScore[nNpcWin].m_cBorder.m_cParams.SetFillImage(cMark, 0);
#endif
        }
    }
#ifdef _XBOX
    m_btnX.SetActivated(m_nState == 3 && m_pActiveControl != NULL);
    int bFlip = 0;
    if (m_pActiveControl != NULL)
    {
        int nCard = m_pActiveControl->m_nID;
        if (nCard >= 0 && nCard < 4 &&
            m_pActiveControl == &m_cPlayerHand[nCard])
        {
            CPazaakCard cFlip = GetPazaak()->m_cPlayer.m_aSideDeck[nCard];
            bFlip = cFlip.m_nType >= 12 && cFlip.m_nType < 18;
        }
    }
    m_btnBlack.SetActivated(bFlip);
    int bContinue = GetPazaak()->m_cPlayer.m_bStanding == 0 &&
                    (m_nState == 3 || m_nState == 4);
    m_btnContinue.SetActivated(bContinue);
    m_btnStand.SetActivated(bContinue);
#else
    m_btnX.SetEnabled(m_nState == 3 && m_pActiveControl != NULL &&
                      m_nHandAction == 0 && bAnyCard != 0);
    m_btnBlack.SetEnabled(bAnyCard != 0 && m_nHandAction == 0);
    int bContinue = GetPazaak()->m_cPlayer.m_bStanding == 0 &&
                    (m_nState == 3 || m_nState == 4) && m_nHandAction == 0;
    m_btnContinue.SetEnabled(bContinue);
    m_btnStand.SetEnabled(bContinue);
#endif
}
// PC 0x006D41E0
void CSWGuiPazaakGame::ReconnectSidedeckButtons()
{
    int bActiveEmptied = 0;
    int nLastCard = -1;
    for (int i = 3; i >= 0; i--)
    {
        m_cPlayerHand[i].SetMoveToControl(0, NULL);
        m_cPlayerHand[i].SetMoveToControl(2, NULL);
        CPazaakCard cCard = m_cPlayerHand[i].m_cCard;
        if (cCard.m_nType >= 0)
        {
            nLastCard = i;
        }
    }
    for (int i = 3; i >= 0; i--)
    {
        CPazaakCard cCard = m_cPlayerHand[i].m_cCard;
        if (cCard.m_nType < 0)
        {
            if (m_pActiveControl == &m_cPlayerHand[i])
            {
                bActiveEmptied = 1;
            }
        }
        else
        {
            if (nLastCard >= 0)
            {
                m_cPlayerHand[i].SetMoveToControl(3, &m_cPlayerHand[nLastCard]);
                m_cPlayerHand[nLastCard].SetMoveToControl(1, &m_cPlayerHand[i]);
            }
            nLastCard = i;
        }
    }
#ifdef _XBOX
    if (bActiveEmptied != 0)
#else
    if (bActiveEmptied != 0 && m_nHandAction != 0)
#endif
    {
        if (nLastCard >= 0)
        {
            SetActiveControl(&m_cPlayerHand[nLastCard], 0);
        }
        else
        {
            SetActiveControl(NULL, 0);
        }
    }
}
// PC 0x006D4310
void CSWGuiPazaakGame::HandleQuitDialog(CSWGuiControl* pControl)
{
    if (g_pAppManager->m_pClientExoApp->GetInGameGui()
            ->m_pMessageBox->m_bAccepted)
    {
        m_bDeferredRemoval = 0;
        m_nExitState = EXITSTATE_DELETE;
        g_pAppManager->m_pClientExoApp->EndPazaakGame(0, m_pPazaak->m_nWager, 0,
                                                      m_pPazaak->m_oidOpponent);
    }
}

#ifndef _XBOX
int CSWGuiPazaakGame::SetHandEnabled(int bEnabled)  // PC 0x006D4370
{
    int nLastCard = -1;
    for (int i = 3; i >= 0; i--)
    {
        CPazaakCard cCard = m_cPlayerHand[i].m_cCard;
        if (cCard.m_nType >= 0)
        {
#ifndef _XBOX
            m_cPlayerHand[i].m_bSelectable = bEnabled;
#endif
            nLastCard = i;
        }
        else
        {
#ifndef _XBOX
            m_cPlayerHand[i].m_bSelectable = 0;
#endif
        }
    }
    m_btnContinue.SetEnabled(bEnabled == 0);
    m_btnStand.SetEnabled(bEnabled == 0);
    m_btnX.SetEnabled(bEnabled == 0);
    m_btnBlack.SetEnabled(bEnabled == 0);
    if (bEnabled != 0)
    {
        if (nLastCard >= 0)
        {
            SetActiveControl(&m_cPlayerHand[nLastCard], 0);
        }
    }
    else
    {
        SetActiveControl(&m_btnContinue, 0);
    }
    return RefreshDisplay();
}
// PC 0x006D4420
void CSWGuiPazaakGame::HandleXButton(CSWGuiControl* pControl)
{
    if (pControl == NULL || pControl->m_bSelected != 0)
    {
        m_nHandAction = 1;
        SetHandEnabled(1);
    }
}
// PC 0x006D4450
void CSWGuiPazaakGame::HandleBlackButton(CSWGuiControl* pControl)
{
    if (pControl == NULL || pControl->m_bSelected != 0)
    {
        m_nHandAction = 2;
        SetHandEnabled(1);
    }
}
// PC 0x006D4480
void CSWGuiPazaakGame::HandleFlipHandCard()
{
    if (m_pActiveControl != NULL)
    {
        int nCard = m_pActiveControl->m_nID;
        if (nCard >= 0 && nCard < 4 &&
            m_pActiveControl == &m_cPlayerHand[nCard])
        {
            CPazaakCard cCard = m_pPazaak->m_cPlayer.m_aSideDeck[nCard];
            if (cCard.m_nType >= 12 && cCard.m_nType < 18)
            {
                m_btnBlack.StartFlash(CGuiInGame::COLOR_CYAN);
                m_pPazaak->m_cPlayer.m_aSideDeck[nCard].m_bFlipped =
                    m_pPazaak->m_cPlayer.m_aSideDeck[nCard].m_bFlipped == 0;
                m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
                RefreshDisplay();
            }
        }
    }
}
// PC 0x006D45C0
void CSWGuiPazaakGame::HandleHandCardRightClick(CSWGuiControl* pControl)
{
    if (pControl != NULL)
    {
        CSWGuiControl* pOldActive = m_pActiveControl;
        m_pActiveControl = pControl;
        HandleFlipHandCard();
        m_pActiveControl = pOldActive;
    }
}
// PC 0x006D45E0
void CSWGuiPazaakGame::HandleFlipButtons(CSWGuiControl* pControl)
{
    CSWGuiControl* pOldActive = m_pActiveControl;
    m_pActiveControl = &m_cPlayerHand[pControl->m_nID];
    HandleFlipHandCard();
    m_pActiveControl = pOldActive;
}
#endif

#ifdef _XBOX
void CSWGuiPazaakGame::OnHandCardEntered(CSWGuiControl* pControl)
{
    RefreshDisplay();
}

int CSWGuiPazaakStart::AddChosenCard(int nAvail)
{
    if (m_anAvailCount[nAvail] < 1)
    {
        return 0;
    }
    int nSlot;
    for (nSlot = 0; nSlot < ChosenCardCount; nSlot++)
    {
        if (m_aChosen[nSlot].m_nType < 0)
        {
            break;
        }
    }
    if (nSlot == ChosenCardCount)
    {
        return 0;
    }
    CPazaakCard cCard;
    cCard.m_nType = nAvail;
    m_aChosen[nSlot] = cCard;
    m_anAvailCount[nAvail] = m_anAvailCount[nAvail] - 1;
    {
        CExoString sCount(m_anAvailCount[nAvail]);
        m_lblAvailCount[nAvail].m_cText.m_cParams.SetText(sCount);
    }
    CResRef cImage = m_cAvail[nAvail].m_cBorder.m_cParams.m_cFillImage;
    m_cChosen[nSlot].m_cBorder.m_cParams.SetFillImage(cImage, 0);
    CExoString sText = m_cAvail[nAvail].m_lblCard.m_cText.m_cParams.m_sText;
    m_cChosen[nSlot].m_lblCard.m_cText.m_cParams.SetText(sText);
    m_bSideDeckFull = 1;
    for (int i = 0; i < ChosenCardCount; i++)
    {
        int nType = m_aChosen[i].m_nType;
        if (nType < 0)
        {
            m_bSideDeckFull = 0;
            break;
        }
    }
    ShowDisabledButtons();
    return 1;
}

void CSWGuiWagerPopup::Draw(float fElapsed)
{
    for (int i = m_lFlashLabels.num - 1; i >= 0; i--)
    {
        m_lFlashLabels.element[i].fTimer -= fElapsed;
        if (m_lFlashLabels.element[i].fTimer <= 0.0f)
        {
            m_lFlashLabels.element[i].pLabel->m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_BLUE;
            m_lFlashLabels.DelIndex(i);
        }
    }
    CSWGuiPanel::Draw(fElapsed);
}

void CSWGuiWagerPopup::FlashLabel(CSWGuiLabel* pLabel)
{
    int nCount = m_lFlashLabels.num;
    for (int i = 0; i < nCount; i++)
    {
        if (m_lFlashLabels.element[i].pLabel == pLabel)
        {
            m_lFlashLabels.element[i].fTimer = 0.25f;
            return;
        }
    }
    m_lFlashLabels.SetSize(nCount + 1);
    m_lFlashLabels.element[nCount].pLabel = pLabel;
    m_lFlashLabels.element[nCount].fTimer = 0.25f;
    pLabel->m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_YELLOW;
}

void CSWGuiWagerPopup::HandleInputEvent(int nEvent, int nState)
{
    if (nState != 0)
    {
        switch (nEvent)
        {
        case 39:
        case 45:
            m_pStart->HandleWagerExit(m_nWager);
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            m_pManager->PopModalPanel();
            m_pManager->RemovePanel(this);
            break;
        case 40:
        case 46:
        {
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            CSWGuiMessageBox* pMessageBox =
                g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
            pMessageBox->SetAllowCancel(1);
            pMessageBox->SetCallback(
                m_pStart, (CSWGuiHandler)&CSWGuiPazaakStart::HandleQuitDialog,
                NULL);
            pMessageBox->SetMessage(42424);
            m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
            break;
        }
        case 47:
        case 50:
        case 58:
        case 59:
        case 62:
        case 63:
            if (m_nWager > 1)
            {
                m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
                m_nWager = m_nWager - 1;
                UpdateWagerText();
                FlashLabel(&m_lblLess);
            }
            break;
        case 48:
        case 49:
        case 57:
        case 60:
        case 61:
        case 64:
            if (m_nWager < m_nMaxWager)
            {
                m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
                m_nWager = m_nWager + 1;
                UpdateWagerText();
                FlashLabel(&m_lblMore);
            }
            break;
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
#endif
