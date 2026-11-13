#include "swguiloadscreen.h"

#include "appmanager.h"
#include "aurgui.h"
#include "clientexoapp.h"
#include "exoresman.h"
#include "guiingame.h"
#include "clientexoappinternal.h"

static inline void SwguiloadscreenTrace1();
#ifndef _XBOX
// PC 0x006D0250 GOG 0x006D02F0
void CSWGuiLoadScreen::OnPanelAdded()
{
    m_lblLogo.m_cBorder.m_cParams.SetFillImage(m_res5A0, 0);
    CSWGuiPanel::OnPanelAdded();
}
// PC 0x006D0270 GOG 0x006D0310
void CSWGuiLoadScreen::OnPanelRemoved()
{
    CResRef cNoLogo("");
    m_lblLogo.m_cBorder.m_cParams.SetFillImage(cNoLogo, 0);
    CSWGuiPanel::OnPanelRemoved();
}
#endif

#ifndef _XBOX
void CSWGuiLoadScreen::Draw(float fElapsed)  // PC 0x006D0430 GOG 0x006D0490
{
    SwguiloadscreenTrace1();
    AurGUIStartLayer();
    if (AurGUISetupViewport(0, 0, m_pManager->m_nWidth, m_pManager->m_nHeight,
                            CGuiInGame::COLOR_BLACK, false, 1.0f) != 0)
    {
        m_lblBackground.Draw(fElapsed);
        AurGUICloseViewport();
    }
    AurGUIStopLayer();
    return CSWGuiPanel::Draw(fElapsed);
}
#endif

static inline void SwguiloadscreenTrace1()
{
}
// PC 0x006D02B0 GOG 0x006D0350
void CSWGuiLoadScreen::SetHint(unsigned long nStrRef)
{
    SwguiloadscreenTrace1();
    CExoString sHint;
    if (nStrRef != 0)
    {
        m_nHintStrRef = nStrRef;
        sHint = g_pTlkTable->GetSimpleString(nStrRef);
        g_pTlkTable->ParseStr(sHint);
    }
    else
    {
        m_nHintStrRef = 0;
    }
    m_lblHint.m_cText.m_cParams.SetText(sHint);
}
// PC 0x006D0370 GOG 0x006D0410
void CSWGuiLoadScreen::AddHintToMessageBuffer()
{
    SwguiloadscreenTrace1();
    CExoString sSpeaker("");
    CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
    ((CGuiInGame*)pClient->GetInGameGui())
        ->AppendToDialogBuffer(m_lblHint.m_cText.m_cParams.m_sText, sSpeaker);
}
// PC 0x006D03F0 GOG 0x006CFF50
void CSWGuiLoadScreen::SetImage(const CResRef& cImage)
{
#ifdef _XBOX
    if (g_pExoResMan->Exists(cImage, RESTYPE_TGA, (unsigned long*)0) != 0 ||
        g_pExoResMan->Exists(cImage, 3006, (unsigned long*)0) != 0)
#else
    if (g_pExoResMan->Exists(cImage, RESTYPE_TGA, (unsigned long*)0) != 0 ||
        g_pExoResMan->Exists(cImage, RESTYPE_TPC, (unsigned long*)0) != 0)
#endif
    {
        SetBackground(cImage);
    }
}
// PC 0x006D0490 GOG 0x006CFF90
CSWGuiLoadScreen::CSWGuiLoadScreen(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
#ifdef _XBOX
    StartLoadFromLayout("loadscreen");
#else
    {
        CResRef cLayout("loadscreen_p");
        StartLoadFromLayout(cLayout);
    }
    m_bPositioned = 1;
#endif
    InitControl(m_pbProgress, "PB_PROGRESS", 1);
    InitControl(m_lblHint, "LBL_HINT", 1);
    InitControl(m_lblLogo, "LBL_LOGO", 1);
    InitControl(m_lblLoading, "LBL_LOADING", 1);
    StopLoadFromLayout();

    m_pbProgress.SetCurValue(0);
    m_lblHint.m_cText.m_cParams.SetText("");

#ifndef _XBOX
    m_lblBackground.SetExtent(CSWGuiExtent(0, 0, pManager->m_nWidth,
                                         pManager->m_nHeight));

    CExoString sBackground("800x600");
    if (g_nScreenWidth == 1024)
    {
        if (g_nScreenHeight == 768)
            sBackground = "1024x768";
    }
    else if (g_nScreenWidth == 1280)
    {
        if (g_nScreenHeight == 960 || g_nScreenHeight == 1024)
            sBackground = "1280x960";
    }
    else if (g_nScreenWidth == 1600 && g_nScreenHeight == 1200)
    {
        sBackground = "1600x1200";
    }
    sBackground = sBackground + "load";
    {
        CResRef cBackground(sBackground);
        CSWGuiBorderParams& cParams = m_lblBackground.m_cBorder.m_cParams;
        cParams.SetFillImage(cBackground, 0);
        cParams.m_nFillMode = CSWGuiBorderParams::FILL_MODE_BACKGROUND;
    }
#endif
    m_bCenterHorizontal = 0;
    m_bCenterVertical = 0;
#ifndef _XBOX
    m_res5A0 = m_lblLogo.m_cBorder.m_cParams.m_cFillImage;
    {
        CResRef cNoLogo("");
        m_lblLogo.m_cBorder.m_cParams.SetFillImage(cNoLogo, 0);
    }
#endif
}
