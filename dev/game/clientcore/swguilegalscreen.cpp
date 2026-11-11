#include "swguilegalscreen.h"

#include "exobase.h"
#include "swmain.h"

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

CSWGuiLegalScreen::~CSWGuiLegalScreen()
{
}

void CSWGuiLegalScreen::SetImage(const CResRef& cImage)
{
    CExoTimers* pTimers = g_pExoBase->m_pExoTimers;

    if (m_nShowTime != 0)
    {
        float fAlpha = m_fAlpha;
        while (pTimers->GetLowResolutionTimer() < m_nShowTime + m_nMinShowTime)
        {
            Sleep(LEGALSCREEN_WAIT_SLEEP);
        }

        unsigned long nLastTime = pTimers->GetLowResolutionTimer();
        while (fAlpha > 0.01f)
        {
            unsigned long nTime = pTimers->GetLowResolutionTimer();
            fAlpha -= (float)(nTime - nLastTime) / LEGALSCREEN_FADE_TIME;
            nLastTime = nTime;
            SetAlpha(fAlpha);
            UpdateScreen(1.0f / 30.0f, 0, 0);
            Sleep(LEGALSCREEN_FRAME_SLEEP);
        }
    }

    m_fAlpha = 0.0f;
    SetBackground(cImage);

    if (cImage.IsValid())
    {
        float fAlpha = 0.0f;
        UpdateScreen(1.0f / 30.0f, 0, 0);
        unsigned long nLastTime = pTimers->GetLowResolutionTimer();
        do
        {
            unsigned long nTime = pTimers->GetLowResolutionTimer();
            fAlpha += (float)(nTime - nLastTime) / LEGALSCREEN_FADE_TIME;
            nLastTime = nTime;
            SetAlpha(fAlpha);
            UpdateScreen(1.0f / 30.0f, 0, 0);
            Sleep(LEGALSCREEN_FRAME_SLEEP);
        } while (fAlpha < 0.99f);
    }

    m_nShowTime = pTimers->GetLowResolutionTimer();
}
