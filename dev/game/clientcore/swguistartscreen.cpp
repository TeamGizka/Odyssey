#include "swguistartscreen.h"

#include "appmanager.h"
#include "clientexoapp.h"
#include "guiingame.h"
#include "swmain.h"

#define STARTSCREEN_ATTRACT_TIME 90.0f

void CSWGuiStartScreen::HandleInputEvent(int nEvent, int nState)
{
    if (nState)
    {
        if (nEvent == GUIEVENT_ACTIVATE || nEvent == GUIEVENT_ACCEPT)
        {
            m_pManager->PlayGuiSound(0);
            g_pAppManager->m_pClientExoApp->DisplayMainMenu(1);
        }
    }
}

void CSWGuiStartScreen::Draw(float fElapsed)
{
    if (!g_pAppManager->m_pClientExoApp->IsMoviePlaying())
    {
        m_fIdleTime += fElapsed;
        if (m_bResetIdleTime)
        {
            m_fIdleTime = 0.0f;
            m_bResetIdleTime = 0;
        }

        CSWGuiPanel::Draw(fElapsed);

        if (m_fIdleTime > STARTSCREEN_ATTRACT_TIME)
        {
            int bSkipped = 0;
            m_fIdleTime = 0.0f;
            g_pAppManager->GetClientExoApp()->PlayMovie("sizzle", bSkipped, 1, 0);
            m_bResetIdleTime = 1;
            if (bSkipped)
            {
                g_pAppManager->GetClientExoApp()->DisplayMainMenu(1);
            }
        }
    }
}

CSWGuiStartScreen::CSWGuiStartScreen(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    if (m_pManager == NULL)
    {
        m_pManager = g_pAppManager->m_pClientExoApp->GetSWGuiManager();
    }

    StartLoadFromLayout("startscreen");
    InitControl(m_lblStartText, "LBL_STARTTEXT", 1);
    StopLoadFromLayout();

    CSWGuiTextParams* pParams = &m_lblStartText.m_cText.m_cParams;
    pParams->m_bHilightText = 1;
    m_fIdleTime = 0.0f;
    m_bResetIdleTime = 1;
    g_pAppManager->m_pClientExoApp->GetInGameGui()->SetGlobalUnpauseableState(0);
}
