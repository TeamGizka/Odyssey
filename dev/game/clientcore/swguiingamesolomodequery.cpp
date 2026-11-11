#include "exobase.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "serverexoapp.h"
#include "partytables.h"
#include "nwcarea.h"
#include "nwsarea.h"
#include "nwcmodule.h"
#include "nwccreature.h"
#include "nwsobject.h"
#include "guiingame.h"
#include "swguiingame.h"
#include "swguiingame.inl"
#include "swguiingamesolomodequery.h"
// PC 0x006C2270  Xbox 0x000F4E80
CSWGuiInGameSoloModeQuery::CSWGuiInGameSoloModeQuery(CSWGuiManager* pManager)
    : CSWGuiMessageBox(pManager)
{
    m_nQuery = 0;
}
// PC 0x006C2310  Xbox 0x000F4C60
void CSWGuiInGameSoloModeQuery::Draw(float fElapsed)
{
    CSWCCreature* pPlayerCreature =
        g_pAppManager->m_pClientExoApp->GetPlayerCreature();
    CSWSCreature* pServerCreature = 0;
    if (pPlayerCreature != 0)
    {
        pServerCreature = pPlayerCreature->GetServerCreature();
    }

    CSWSArea* pServerArea = 0;
    if (g_pAppManager->m_pClientExoApp->GetModule() != 0)
    {
        CSWCModule* pModule = g_pAppManager->m_pClientExoApp->GetModule();
        if (pModule->m_pInfo != 0)
        {
            pServerArea = pModule->m_pInfo->GetServerArea();
        }
    }

    if (g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nGlobalDialogState != 0 ||
        (pServerArea != 0 && pServerArea->m_bTransitionPending != 0) ||
        g_pAppManager->m_pClientExoApp->GetAreaNotReadyToDisplay() != 0 ||
        (pServerCreature != 0 &&
         (pServerCreature->GetDead() != 0 || pServerCreature->GetDeadTemp() != 0)))
    {
        SetModal(0);
    }

    return CSWGuiPanel::Draw(fElapsed);
}
// PC 0x006C2400  Xbox 0x000F4D40
void CSWGuiInGameSoloModeQuery::HandleInputEvent(int nEvent, int nState)
{
    if (nState != 0)
    {
        switch (nEvent)
        {
        case 39:
        case 45:
#ifndef _XBOX
        case 502:
#endif
        {
            CSWCCreature* pPlayerCreature =
                g_pAppManager->m_pClientExoApp->GetPlayerCreature();
            g_pAppManager->m_pClientExoApp->TogglePartyFollow();
            if (pPlayerCreature != 0 && m_nQuery != 0)
            {
                pPlayerCreature->PerformSkill(2, 0, pPlayerCreature->m_nId);
            }
        }
        case 40:
        case 46:
#ifndef _XBOX
        case 503:
#endif
            ((CGuiInGame*)g_pAppManager->m_pClientExoApp->GetInGameGui())
                ->HideSoloMode();
            return;
        }
    }
    return CSWGuiMessageBox::HandleInputEvent(nEvent, nState);
}
// PC 0x006C24A0  Xbox 0x000F4DE0
void CSWGuiInGameSoloModeQuery::SetQueryText(int nQuery)
{
    if (nQuery != 0)
    {
        SetMessage(37890);
    }
    else if (g_pAppManager->m_pServerExoApp->GetPartyTable()->m_bSoloMode != 0)
    {
        if (g_pAppManager->m_pClientExoApp->GetPlayerCreature()->m_bRunning)
        {
            SetMessage(37892);
        }
        else
        {
            SetMessage(37891);
        }
    }
    else
    {
        SetMessage(37889);
    }
    m_nQuery = nQuery;
}
