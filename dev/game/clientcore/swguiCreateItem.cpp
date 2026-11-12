#include "swguiCreateItem.h"

#include "appmanager.h"
#include "clientexoapp.h"
#include "exoresman.h"
#include "guiingame.h"
#include "nwitem.h"
#include "nwrules.h"
#include "twodimarray.h"
#include "swmain.h"

static inline void SwguiCreateItemTrace1();
// PC 0x0073E8C0
void CSWGuiCreateItemMenu::HandleInputEvent(int nEvent, int nState)
{
    if (nState != 0)
    {
        switch (nEvent)
        {
        case GUIEVENT_UP:
        case GUIEVENT_UP_ALTERNATE:
            if (m_nSelectedSubType > 0)
            {
                m_nSelectedSubType--;
            }
            else if (m_nSelectedSubType == 0)
            {
                m_nSelectedSubType = g_pRules->m_pBaseItems->m_nNumBaseItems;
            }
            if (m_pManager != NULL)
            {
                m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
            }
            break;

        case GUIEVENT_DOWN:
        case GUIEVENT_DOWN_ALTERNATE:
        {
            int nBaseItems = g_pRules->m_pBaseItems->m_nNumBaseItems;
            int nSelection = m_nSelectedSubType;
            if (nSelection < nBaseItems)
            {
                m_nSelectedSubType = nSelection + 1;
            }
            else if (nSelection == nBaseItems)
            {
                m_nSelectedSubType = 0;
            }
            if (m_pManager != NULL)
            {
                m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
            }
            break;
        }

        case GUIEVENT_CANCEL:
        case GUIEVENT_CANCEL_ALTERNATE:
            g_pAppManager->m_pClientExoApp->GetInGameGui()->HideItemCreateMenu();
            break;
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
// PC 0x0073EA30
CSWGuiButton* CSWGuiCreateItemMenu::CreateOption(const CExoString& sText,
                                                 CSWGuiHandler pHandler,
                                                 CSWGuiButton* pPrototype)
{
    CSWGuiExtent cExtent = pPrototype->m_cExtent;
    CSWGuiButton* pButton;

    pButton = new CSWGuiButton();
    pButton->Initialize(cExtent, pPrototype);
    pButton->m_cText.m_cParams.SetText(sText);
    pButton->AddEvent(GUIEVENT_ACTIVATE, this, pHandler);
    pButton->AddEvent(GUIEVENT_ACCEPT, this, pHandler);
    return pButton;
}
// PC 0x0073E9B0
void CSWGuiCreateItemMenu::Back(CSWGuiControl* pControl)
{
    SwguiCreateItemTrace1();
    if (pControl->m_bSelected != 0)
    {
        g_pAppManager->m_pClientExoApp->GetInGameGui()->HideItemCreateMenu();
        g_pAppManager->m_pClientExoApp->GetInGameGui()->ShowDebugMenu();
    }
}
// PC 0x0073E9F0
void CSWGuiCreateItemMenu::ItemSubTypeMenu(CSWGuiControl* pControl)
{
    SwguiCreateItemTrace1();
    unsigned long nSubType;

    if (pControl->m_bSelected != 0)
    {
        g_pAppManager->m_pClientExoApp->GetInGameGui()->HideItemCreateMenu();
        nSubType = pControl->m_nID - 1;
        g_pAppManager->m_pClientExoApp->GetInGameGui()->ShowSubItemCreateMenu(
            nSubType);
    }
}
// PC 0x0073EAF0
CSWGuiCreateItemMenu::CSWGuiCreateItemMenu(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager), m_nSelectedSubType(0)
{
    CSWGuiControl* pPrototype;
    int nBaseItems;
    int i;

    StartLoadFromLayout("debug_p");
    InitControl(m_lbOptions, "LB_OPTIONS", 1);
    InitControl(m_lblBuild, "LBL_BUILD", 1);
    StopLoadFromLayout();

    CExoString sBuildText = CExoString("Build: ") + g_sBuildNumber;
    m_lblBuild.m_cText.m_cParams.SetText(sBuildText);

    CExoArrayList<CSWGuiControl*> aControls;
    pPrototype = m_lbOptions.m_pProtoItem;
    nBaseItems = g_pRules->m_pBaseItems->m_nNumBaseItems;
    aControls.SetSize(nBaseItems + 1);

    {
        CExoString sBack("Back");
        CSWGuiControl** ppOption = &aControls.element[0];
        *ppOption = CreateOption(
            sBack, (CSWGuiHandler)&CSWGuiCreateItemMenu::Back,
            (CSWGuiButton*)pPrototype);
    }

    for (i = 1; i <= nBaseItems; i++)
    {
        CExoString sText;

        if (g_pRules->m_pBaseItems->GetBaseItem(i - 1)->m_nName != 0)
        {
            sText = g_pRules->m_pBaseItems->GetBaseItem(i - 1)->GetNameText();
        }
        else
        {
            C2DA* pBaseItems = new C2DA("BASEITEMS", 0);
#ifdef _XBOX
            EXOXBOXASSERT(pBaseItems->Load2DArray());
#else
            pBaseItems->Load2DArray();
#endif
            CExoString sLabel;
            pBaseItems->GetCExoStringEntry(i - 1, "Label", &sLabel);
            sText.Format("No StrRef: %s", sLabel);
            delete pBaseItems;
        }
        CSWGuiControl** ppOption = &aControls.element[i];
        *ppOption = CreateOption(
            sText, (CSWGuiHandler)&CSWGuiCreateItemMenu::ItemSubTypeMenu,
            (CSWGuiButton*)pPrototype);
    }

    CSWGuiListBox* pListBox = &m_lbOptions;
    pListBox->AddControls(aControls, 1, 0, 0);
    pListBox->m_bLooping = 1;
    SetActiveControl(pListBox, 0);
    CenterPanel();
}

static inline void SwguiCreateItemTrace1()
{
}
