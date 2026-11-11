#include "exobase.h"
#include "exoresman.h"
#include "exotlktable.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "guiingame.h"
#include "swguiingamepause.h"
// PC 0x006C00B0
void CSWGuiInGamePause::Draw(float fElapsed)
{
    return CSWGuiPanel::Draw(fElapsed);
}
// PC 0x006C00C0  Xbox 0x000F4FD0
void CSWGuiInGamePause::SetPauseReason(unsigned char nReason)
{
    int bShowPress = 1;
    unsigned long nStrRef;

    switch (nReason)
    {
    case 1:
        nStrRef = 48212;
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
        nStrRef = 48423;
        break;
    case 11:
        nStrRef = 49118;
        bShowPress = 0;
        break;
    default:
        nStrRef = 1508;
        break;
    }

    CExoString sReason =
        g_pAppManager->GetClientExoApp()->GetGUIString(nStrRef);
    m_lblPauseReason.m_cText.m_cParams.SetText(sReason);

    CSWGuiExtent cExtent;
    CSWGuiExtent cUnpauseExtent;

    int nHeight = m_lblPauseReason.m_cText.GetIdealHeight();
    cExtent = m_lblPauseReason.m_cExtent;
    cExtent.m_nHeight = nHeight;
    m_lblPauseReason.SetExtent(cExtent);

    int nTop = nHeight + cExtent.m_nTop + 2;
    cUnpauseExtent = cExtent;

    if (bShowPress)
    {
        m_lblPress.SetEnabledFlag(1);
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

    cUnpauseExtent.m_nHeight = cPanelExtent.m_nHeight;
    m_btnUnpause.SetExtent(cUnpauseExtent);
}
#ifndef _XBOX
void CSWGuiInGamePause::OnPanelAdded()  // PC 0x006C02F0  Xbox 0x000F5230
{
    if (g_pAppManager->m_pClientExoApp->GetMiniGame() != 0)
    {
        m_cExtent.m_nLeft = (m_pManager->m_nWidth - m_cExtent.m_nWidth) / 2;
        m_cExtent.m_nTop = (m_pManager->m_nHeight - m_cExtent.m_nHeight) / 2;
    }
    else
    {
        CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
        pClient->GetInGameGui()->SetupPauseGuiExtent(&m_cExtent);
    }
    CSWGuiPanel::OnPanelAdded();
}
#endif
void CSWGuiInGamePause::OnUnpauseButtonPressed(CSWGuiControl* pControl)  // PC 0x006C0350
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
// PC 0x006C03B0  Xbox 0x000F5280
CSWGuiInGamePause::CSWGuiInGamePause(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    {
        CResRef cLayout("pause");
        StartLoadFromLayout(cLayout);
    }
    InitControl(m_lblPauseReason, "LBL_PAUSEREASON", 1);
    InitControl(m_lblPress, "LBL_PRESS", 1);
    InitControl(m_btnUnpause, "BTN_UNPAUSE", 1);
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
    m_btnUnpause.SetFocusable(0);
    m_btnUnpause.AddEvent(39, this,
        (PFNGUIEVENT)&CSWGuiPanel::OnAButtonPressed);
    m_btnUnpause.AddEvent(
        39, this, (CSWGuiHandler)&CSWGuiInGamePause::OnUnpauseButtonPressed);
}

#ifdef _XBOX
// PC 0x006C02F0  Xbox 0x000F5230
void CSWGuiInGamePause::OnPanelAdded()
{
    if (g_pAppManager->m_pClientExoApp->GetMiniGame() != 0)
    {
        m_cExtent.m_nLeft = (m_pManager->m_nWidth - m_cExtent.m_nWidth) / 2;
        m_cExtent.m_nTop = (m_pManager->m_nHeight - m_cExtent.m_nHeight) / 2;
    }
    else
    {
        m_cExtent.m_nLeft = m_nOriginalLeft;
        m_cExtent.m_nTop = m_nOriginalTop;
    }
}
#endif
