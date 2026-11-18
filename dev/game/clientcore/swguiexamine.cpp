#include "exobase.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "guiingame.h"
#include "swguiingametutorialbox.h"

static inline void TraceExamineInput();
static inline void TraceExaminePanel();
#define SWGUIEXAMINE_TRACE() TraceExamineInput()
// PC 0x006CE9C0  Xbox 0x000F54C0
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
// PC 0x006CEA30  Xbox 0x000F5460
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

static inline void TraceExamineInput()
{
    TraceExaminePanel();
}

static inline void TraceExaminePanel()
{
}
