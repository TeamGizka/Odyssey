#include "swguiingameareatransition.h"

#include "appmanager.h"
#include "clientexoapp.h"
#include "exobase.h"
#include "exoresman.h"
#include "guiingame.h"
#include "nwcdoor.h"
#include "nwctrigger.h"
#include "nwsobject.h"
#include "gameobjectarray.h"
#include "swparty.h"

static inline void SwguiingameareatransitionTrace1();
// PC 0x007346C0
CSWGuiInGameAreaTransition::CSWGuiInGameAreaTransition(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
#ifndef _XBOX
    { CResRef cLayout("areatransition"); StartLoadFromLayout(cLayout); }
#else
    StartLoadFromLayout("areatransition");
#endif
    InitControl(m_lblTextBackground, "LBL_TEXTBG", 1);
    InitControl(m_lblIcon, "LBL_ICON", 1);
    InitControl(m_lblDescription, "LBL_DESCRIPTION", 1);
    StopLoadFromLayout();
    m_v454 = m_lblTextBackground.m_cBorder.m_cParams.m_vColor;
    m_v460 = m_lblIcon.m_cBorder.m_cParams.m_vColor;
    m_v46C = m_lblDescription.m_cText.m_cParams.m_vColor;

    m_oidTransitionObject = OBJECT_INVALID;
    m_bCenterHorizontal = 1; m_bCenterVertical = 0;
    m_nDefaultTop = m_cExtent.m_nTop;
}
// PC 0x00734400
void CSWGuiInGameAreaTransition::SetTransitionObject(unsigned long oidObject,
                                                     const Vector& vPosition)
{
    SwguiingameareatransitionTrace1();
    if (m_oidTransitionObject != oidObject)
    {
        CClientExoApp* pClientApp = g_pAppManager->m_pClientExoApp;
        CSWCObject* pObject = pClientApp->GetGameObject(oidObject);
        if (pObject != NULL)
        {
            CSWCObject* pClientObject = pObject->AsSWCObject();
            if (pClientObject != NULL)
            {
                CExoString sName;
                if (pClientObject->m_nObjectType == 10)
                {
                    sName =
                        pClientObject->AsSWCDoor()->GetTransitionDestination();
                }
                else if (pClientObject->m_nObjectType == 7)
                {
                    sName = pClientObject->AsSWCTrigger()->m_sTag;
                }
                int nDash;
                {
                    CExoString sDash("-");
                    nDash = sName.Find(sDash, 0);
                }
                if (nDash != -1)
                {
                    sName = sName.Right(sName.GetLength() - nDash - 2);
                }
                m_lblDescription.m_cText.m_cParams.SetText(sName);
            }
        }
        m_oidTransitionObject = oidObject;
    }
    m_vTransitionPosition = vPosition;
}
// PC 0x007345A0
void CSWGuiInGameAreaTransition::Draw(float fElapsed)
{
    SwguiingameareatransitionTrace1();
    int nBarkBubbleTop;
    CSWGuiExtent extent = m_cExtent;
    CClientExoApp* pClientApp = g_pAppManager->m_pClientExoApp;

    if (pClientApp->GetInGameGui()->GetBarkBubbleVisible(nBarkBubbleTop) != 0)
    {
        extent.m_nTop = nBarkBubbleTop;
    }
    else
    {
        extent.m_nTop = m_nDefaultTop;
    }
    SetExtent(extent);

    CSWParty* pParty = g_pAppManager->m_pClientExoApp->GetSWParty();
    if (pParty->IsPartyTogether() != 0)
    {
        m_lblTextBackground.m_cBorder.m_cParams.m_vColor = m_v454;
        m_lblIcon.m_cBorder.m_cParams.m_vColor = m_v460;
        m_lblDescription.m_cText.m_cParams.SetColor(m_v46C);
    }
    else
    {
        m_lblTextBackground.m_cBorder.m_cParams.m_vColor =
            CGuiInGame::DISABLED_ICON_COLOR;
        m_lblIcon.m_cBorder.m_cParams.m_vColor = CGuiInGame::DISABLED_ICON_COLOR;
        m_lblDescription.m_cText.m_cParams.SetColor(CGuiInGame::DISABLED_TEXT);
    }
    return CSWGuiPanel::Draw(fElapsed);
}

static inline void SwguiingameareatransitionTrace1()
{
}
