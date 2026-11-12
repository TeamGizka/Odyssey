#include "swguilegalscreen.h"

#include "exobase.h"
#include "swmain.h"
#include "clientexoapp.h"
#include "guiingame.h"
#include "exotlktable.h"

#define LEGALSCREEN_FADE_TIME 250.0f
#define LEGALSCREEN_WAIT_SLEEP 50
#define LEGALSCREEN_FRAME_SLEEP 20

CSWGuiLegalScreen::CSWGuiLegalScreen(CSWGuiManager* pManager,
                                     unsigned long nMinShowTime)
    : CSWGuiPanel(pManager)
{
    SetExtent(CSWGuiExtent(0, 0, pManager->m_nWidth, pManager->m_nHeight));
    m_nShowTime = 0;
    m_nMinShowTime = nMinShowTime;
}

void CSWGuiLegalScreen::SetImage(const CResRef& cImage)
{
    float fAlpha = m_fAlpha;

    if (m_nShowTime != 0)
    {
        while (g_pExoBase->m_pExoTimers->GetLowResolutionTimer() < m_nShowTime + m_nMinShowTime)
        {
            Sleep(LEGALSCREEN_WAIT_SLEEP);
        }

        unsigned long nLastTime = g_pExoBase->m_pExoTimers->GetLowResolutionTimer();
        unsigned long nTime = nLastTime;
        while (fAlpha > 0.01f)
        {
            fAlpha -= (float)(nTime - nLastTime) / LEGALSCREEN_FADE_TIME;
            nLastTime = nTime;
            nTime = g_pExoBase->m_pExoTimers->GetLowResolutionTimer();
            SetAlpha(max(0.0f, fAlpha));
            UpdateScreen(1.0f / 30.0f, 0, 0);
            Sleep(LEGALSCREEN_FRAME_SLEEP);
        }
    }

    fAlpha = 0.0f;
    SetAlpha(fAlpha);
    SetBackground(cImage);

    if (cImage.IsValid())
    {
        UpdateScreen(1.0f / 30.0f, 0, 0);
        unsigned long nLastTime = g_pExoBase->m_pExoTimers->GetLowResolutionTimer();
        unsigned long nTime = nLastTime;
        do
        {
            fAlpha += (float)(nTime - nLastTime) / LEGALSCREEN_FADE_TIME;
            nLastTime = nTime;
            nTime = g_pExoBase->m_pExoTimers->GetLowResolutionTimer();
            SetAlpha(min(1.0f, fAlpha));
            UpdateScreen(1.0f / 30.0f, 0, 0);
            Sleep(LEGALSCREEN_FRAME_SLEEP);
        } while (fAlpha < 0.99);
    }

    m_nShowTime = g_pExoBase->m_pExoTimers->GetLowResolutionTimer();
}

// PC 0x006CA770
void CSWDemoScreen::SetImage(const CResRef& cImage)
{
    float fAlpha = 1.0f;
    SetAlpha(fAlpha);
    SetBackground(cImage);
    m_nShowTime = g_pExoBase->m_pExoTimers->GetLowResolutionTimer();
}

// PC 0x006CA7A0
void CSWDemoScreen::HandleInputEvent(int nEvent, int nState)
{
    if (g_pExoBase->m_pExoTimers->GetLowResolutionTimer() >= m_nShowTime + m_nMinShowTime)
    {
        m_bShowing = 0;
        g_pAppManager->GetClientExoApp()->GetInGameGui()->StartGlobalFade(1, 0.0f, 1.0f, Vector(0.0f, 0.0f, 0.0f));
        g_pAppManager->GetClientExoApp()->GetInGameGui()->HideDemoScreen();
    }
}

// PC 0x006CA820
void CSWDemoScreen::Update(float fElapsed)
{
    if (g_pExoBase->m_pExoTimers->GetLowResolutionTimer() >= m_nShowTime + m_nMinShowTime * 2)
    {
        m_bDeferredRemoval = 0;
        m_nExitState = EXITSTATE_DELETE;
        m_bShowing = 0;
        g_pAppManager->GetClientExoApp()->GetInGameGui()->StartGlobalFade(1, 0.0f, 1.0f, Vector(0.0f, 0.0f, 0.0f));
        g_pAppManager->GetClientExoApp()->GetInGameGui()->HideDemoScreen();
    }
}

// PC 0x006CA8F0
CSWDemoScreen::CSWDemoScreen(CSWGuiManager* pManager, unsigned long nMinShowTime, unsigned long nStrRef,
                             unsigned long nLeft, unsigned long nTop)
    : CSWGuiPanel(pManager)
{
    CExoString sText;

    SetExtent(CSWGuiExtent(0, 0, pManager->m_nWidth, pManager->m_nHeight));
    StartLoadFromLayout("demo_p");
    InitControl(m_lblDemoText, "LBL_DEMOTEXT", 1);
    StopLoadFromLayout();

    m_nShowTime = 0;
    m_nMinShowTime = nMinShowTime;
    m_nShowTime = g_pExoBase->m_pExoTimers->GetLowResolutionTimer();
    m_bShowing = 1;

    if (nStrRef != 0)
    {
        sText = g_pTlkTable->GetSimpleString(nStrRef);
        g_pTlkTable->ParseStr(sText);
        CSWGuiExtent cExtent = m_lblDemoText.m_cExtent;
        if (nLeft > 0)
        {
            cExtent.m_nLeft = nLeft;
        }
        if (nTop > 0)
        {
            cExtent.m_nTop = nTop;
        }
        m_lblDemoText.m_cText.m_cParams.SetText(sText);
        m_lblDemoText.SetExtent(cExtent);
        m_lblDemoText.SetEnabledFlag(1);
    }
    else
    {
        m_lblDemoText.m_cText.m_cParams.SetText("");
    }
}
