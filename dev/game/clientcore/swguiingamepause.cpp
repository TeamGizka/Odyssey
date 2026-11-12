#include "exobase.h"
#include "exoresman.h"
#include "exotlktable.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "guiingame.h"
#include "swguiingamepause.h"
#include "swminigame.h"
#ifndef _XBOX
void CSWGuiInGamePause::Draw(float fElapsed)
{
    return CSWGuiPanel::Draw(fElapsed);
}
#endif
void CSWGuiInGamePause::SetPauseReason(unsigned char nReason)  // PC 0x007206F0
{
    int bShowPress = 1;
    unsigned long nStrRef;

    switch (nReason)
    {
#ifdef _XBOX
    case 1:
        nStrRef = 41895;
        bShowPress = 0;
        break;
    case 5:
        nStrRef = 42432;
        break;
    case 7:
        nStrRef = 42482;
        break;
    case 8:
        nStrRef = 42481;
        break;
    case 9:
        nStrRef = 42397;
        break;
    case 10:
        nStrRef = 42579;
        break;
#else
    case 1:
        nStrRef = STRREF_ENEMY_SIGHTED;
        bShowPress = 0;
        break;
    case 5:
        nStrRef = 42432;
        break;
    case 7:
        nStrRef = 42482;
        break;
    case 8:
        nStrRef = 42481;
        break;
    case 9:
        nStrRef = 42397;
        break;
    case 10:
        nStrRef = STRREF_ACTION_QUEUED;
        break;
    case 11:
        nStrRef = STRREF_MINE_SIGHTED;
        bShowPress = 0;
        break;
#endif
    default:
        nStrRef = STRREF_PAUSED;
        break;
    }

    CExoString sReason =
        g_pAppManager->GetClientExoApp()->GetGUIString(nStrRef);
    m_lblPauseReason.m_cText.m_cParams.SetText(sReason);

    CSWGuiExtent cExtent;
#ifndef _XBOX
    CSWGuiExtent cUnpauseExtent;
#endif

    int nHeight = m_lblPauseReason.m_cText.GetIdealHeight();
    cExtent = m_lblPauseReason.m_cExtent;
    cExtent.m_nHeight = nHeight;
    m_lblPauseReason.SetExtent(cExtent);

    int nTop = nHeight + cExtent.m_nTop + 2;
#ifndef _XBOX
    cUnpauseExtent = cExtent;
#endif

    if (bShowPress)
    {
        m_lblPress.SetEnabledFlag(1);
        CExoString sPress =
            g_pAppManager->GetClientExoApp()->GetGUIString(STRREF_PRESS_TO_CONTINUE);
        g_pTlkTable->ParseStr(sPress);
        m_lblPress.m_cText.m_cParams.SetText(sPress);
        nHeight = m_lblPress.m_cText.GetIdealHeight();
        cExtent = m_lblPress.m_cExtent;
        cExtent.m_nHeight = nHeight;
        cExtent.m_nTop = nTop;
        m_lblPress.SetExtent(cExtent);
    }
    else
    {
        m_lblPress.SetEnabledFlag(0);
    }

    CSWGuiExtent cPanelExtent = m_cExtent;
    cPanelExtent.m_nHeight = cExtent.m_nTop + cExtent.m_nHeight + 5;
    SetExtent(cPanelExtent);

#ifndef _XBOX
    cUnpauseExtent.m_nHeight = cPanelExtent.m_nHeight;
    m_btnUnpause.SetExtent(cUnpauseExtent);
#endif
}
#ifndef _XBOX
void CSWGuiInGamePause::OnPanelAdded()  // PC 0x00720970
{
    CSWMiniGame* pMiniGame = g_pAppManager->GetClientExoApp()->GetMiniGame();
    if (pMiniGame != 0)
    {
        int nAttachType = pMiniGame->m_nAttachType;
        if (nAttachType != CSWMiniGame::ATTACH_PLAYER_MODEL)
        {
            m_cExtent.m_nLeft = (m_pManager->m_nWidth - m_cExtent.m_nWidth) / 2;
            m_cExtent.m_nTop = (m_pManager->m_nHeight - m_cExtent.m_nHeight) / 2;
            m_lblPauseReason.m_cText.m_cParams.SetStrRef(STRREF_PAUSED);
            CExoString sPress =
                g_pAppManager->GetClientExoApp()->GetGUIString(STRREF_PRESS_TO_UNPAUSE_MINIGAME);
            g_pTlkTable->ParseStr(sPress);
            m_lblPress.m_cText.m_cParams.SetText(sPress);
        }
        else
        {
            m_cExtent.m_nLeft = (m_pManager->m_nWidth - m_cExtent.m_nWidth) / 2;
            m_cExtent.m_nTop = (m_pManager->m_nHeight - m_cExtent.m_nHeight) / 2;
            m_lblPauseReason.m_cText.m_cParams.SetStrRef(STRREF_PAUSED);
            CExoString sPress =
                g_pAppManager->GetClientExoApp()->GetGUIString(STRREF_PRESS_TO_FORFEIT);
            g_pTlkTable->ParseStr(sPress);
            m_lblPress.m_cText.m_cParams.SetText(sPress);
        }
    }
    else
    {
        CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
        pClient->GetInGameGui()->SetupPauseGuiExtent(&m_cExtent);
    }
    CSWGuiPanel::OnPanelAdded();
}
#endif
void CSWGuiInGamePause::OnUnpauseButtonPressed(CSWGuiControl* pControl)  // PC 0x00720AE0
{
    if (pControl->m_bSelected != 0)
    {
        int bPausedByCombat =
            g_pAppManager->m_pClientExoApp->GetPausedByCombat();
        g_pAppManager->m_pClientExoApp->SetPausedByCombat(bPausedByCombat == 0,
                                                          4, 0);
        if (g_pAppManager->m_pClientExoApp->GetAutoPaused() != 0)
        {
            g_pAppManager->m_pClientExoApp->SetAutoPaused(0, 0);
        }
    }
}
// PC 0x00720C20
CSWGuiInGamePause::CSWGuiInGamePause(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
#ifdef _XBOX
    StartLoadFromLayout("pause");
#else
    {
        CResRef cLayout("pause_p");
        StartLoadFromLayout(cLayout);
    }
#endif
    InitControl(m_lblPauseReason, "LBL_PAUSEREASON", 1);
    InitControl(m_lblPress, "LBL_PRESS", 1);
#ifndef _XBOX
    InitControl(m_btnUnpause, "BTN_UNPAUSE", 1);
#endif
#ifndef _XBOX
    m_bPositioned = 1;
#endif
    StopLoadFromLayout();

    m_nOriginalTop = m_cExtent.m_nTop;
    m_nOriginalLeft = m_cExtent.m_nLeft;

    CSWGuiTextParams* pPressParams = &m_lblPress.m_cText.m_cParams;
    CExoString sPress(pPressParams->m_sText);
    g_pTlkTable->ParseStr(sPress);
    pPressParams->SetText(sPress);
    m_lblPauseReason.m_cText.m_cParams.m_bHilightText = 1;
    pPressParams->m_bHilightText = 1;
#ifndef _XBOX
    m_btnUnpause.SetFocusable(0);
    m_btnUnpause.AddEvent(39, this,
        (PFNGUIEVENT)&CSWGuiPanel::OnAButtonPressed);
    m_btnUnpause.AddEvent(
        39, this, (CSWGuiHandler)&CSWGuiInGamePause::OnUnpauseButtonPressed);
#endif
}

#ifdef _XBOX

void CSWGuiInGamePause::OnPanelAdded()
{
    CSWMiniGame* pMiniGame = g_pAppManager->GetClientExoApp()->GetMiniGame();
    if (pMiniGame != 0)
    {
        int nAttachType = pMiniGame->m_nAttachType;
        if (nAttachType != CSWMiniGame::ATTACH_PLAYER_MODEL)
        {
            m_cExtent.m_nLeft = (m_pManager->m_nWidth - m_cExtent.m_nWidth) / 2;
            m_cExtent.m_nTop = (m_pManager->m_nHeight - m_cExtent.m_nHeight) / 2;
            m_lblPauseReason.m_cText.m_cParams.SetStrRef(STRREF_PAUSED);
            CExoString sPress =
                g_pAppManager->GetClientExoApp()->GetGUIString(STRREF_PRESS_TO_UNPAUSE_MINIGAME);
            g_pTlkTable->ParseStr(sPress);
            m_lblPress.m_cText.m_cParams.SetText(sPress);
        }
        else
        {
            m_cExtent.m_nLeft = (m_pManager->m_nWidth - m_cExtent.m_nWidth) / 2;
            m_cExtent.m_nTop = (m_pManager->m_nHeight - m_cExtent.m_nHeight) / 2;
            m_lblPauseReason.m_cText.m_cParams.SetStrRef(STRREF_PAUSED);
            CExoString sPress =
                g_pAppManager->GetClientExoApp()->GetGUIString(STRREF_PRESS_TO_FORFEIT);
            g_pTlkTable->ParseStr(sPress);
            m_lblPress.m_cText.m_cParams.SetText(sPress);
        }
    }
    else
    {
        m_cExtent.m_nLeft = m_nOriginalLeft;
        m_cExtent.m_nTop = m_nOriginalTop;
    }
}

void CSWGuiInGamePause::Draw(float fElapsed)
{
    if (g_pAppManager->m_pClientExoApp->GetCombatMode())
    {
        m_cExtent.m_nTop += g_pAppManager->m_pClientExoApp->GetInGameGui()->GetInterfaceOffset();
    }
    CSWGuiPanel::Draw(fElapsed);
    if (g_pAppManager->m_pClientExoApp->GetCombatMode())
    {
        m_cExtent.m_nTop -= g_pAppManager->m_pClientExoApp->GetInGameGui()->GetInterfaceOffset();
    }
}
#endif
