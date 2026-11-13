#include "exobase.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "guiingame.h"
#include "swguiingametutorialbox.h"

static inline void SwguiexamineTrace1();
static inline void SwguiexamineTrace2();
#define SWGUIEXAMINE_TRACE() SwguiexamineTrace1()
// PC 0x0073CF30 GOG 0x0073CD00
void CSWGuiExamine::HandleInputEvent(int nEvent, int nState)
{
    SWGUIEXAMINE_TRACE();
    if (nState != 0)
    {
        switch (nEvent)
        {
        case 39:
        case 40:
        case 45:
        case 46:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            g_pAppManager->m_pClientExoApp->GetInGameGui()
                ->HideExamineBox();
            return;
        }
    }
    return CSWGuiMessageBox::HandleInputEvent(nEvent, nState);
}

// PC 0x0073CF90 GOG 0x0073CC90
CSWGuiExamine::CSWGuiExamine(CSWGuiManager* pManager)
    : CSWGuiMessageBox(pManager)
{
#ifdef _XBOX
    m_lblOk.m_cText.m_cParams.SetStrRef(STRREF_ACCEPT_BUTTON);
#else
    m_btnOk.m_cText.m_cParams.SetStrRef(STRREF_ACCEPT_BUTTON);
#endif
    SetAllowCancel(0);
    m_nReturnInputMode = 0;
}

static inline void SwguiexamineTrace1()
{
    SwguiexamineTrace2();
}

static inline void SwguiexamineTrace2()
{
}
