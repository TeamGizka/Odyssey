#include "exobase.h"
#include "gameobjectarray.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "exotlktable.h"
#include "exoresman.h"
#include "twodimarray.h"
#include "nwclass.h"
#include "twodimarrays.h"
#include "guiingame.h"
#include "nwcdoor.h"
#include "nwcplaceable.h"
#include "nwccreature.h"
#include "swguiingamemap.h"
#include "swguimaininterfacepc2.h"
#include "swguiingametutorialbox.h"
#include "nwrules.h"
#include "swguiingame.h"
#include "swguiingame.inl"

static inline void SwguiingametutorialboxTrace1();
#define SWGUIINGAMETUTORIALBOX_TRACE() SwguiingametutorialboxTrace1()
// PC 0x00734A90 GOG 0x007347A0
void CSWGuiTutorialBox::SetMessage(unsigned long nStrRef)
{
    CExoString sMessage = g_pTlkTable->GetSimpleString(nStrRef);
    g_pTlkTable->ParseStr(sMessage);
    CSWGuiMessageBox::SetMessage(sMessage);
}
// PC 0x00734BE0 GOG 0x00734830
int CSWGuiTutorialBox::SetTutorialInstigator(unsigned long oidInstigator,
                                             unsigned long oidTarget,
                                             unsigned long nActionParam)
{
    m_oidInstigator = oidInstigator;
    m_oidTarget = oidTarget;
    m_nActionParam = nActionParam;
    return nActionParam;
}
// PC 0x00734B20 GOG 0x00734900
int CSWGuiTutorialBox::TutorialReasonIsValid(unsigned char nReason)
{
    SWGUIINGAMETUTORIALBOX_TRACE();
    bool bMissing;
    {
        CExoString sColumn("Message_PC0");
        CTwoDimArrays* p2DArrays = g_pRules->Get2DArrays();
        EXOXBOXASSERT(p2DArrays->m_pTutorial);
        C2DA* pTutorial = p2DArrays->m_pTutorial;
        int nValue;
        bMissing = pTutorial->GetINTEntry(nReason, sColumn, &nValue) == 0;
    }
    return !bMissing;
}
// PC 0x00734BB0 GOG 0x00734990
void CSWGuiTutorialBox::MessageBoxUnpause(CSWGuiControl* pControl)
{
    SWGUIINGAMETUTORIALBOX_TRACE();
    if (m_bWasPausedByCombat == 0)
    {
        g_pAppManager->GetClientExoApp()->SetPausedByCombat(0, 2, 0);
    }
    m_bWasPausedByCombat = 0;
}
// PC 0x00734C10 GOG 0x007349C0
void CSWGuiTutorialBox::PerformCombatFeat(CSWGuiControl* pControl)
{
    CSWCCreature* pCreature =
        g_pAppManager->GetClientExoApp()->GetCreatureByGameObjectID(
            m_oidInstigator);
    MessageBoxUnpause(0);
    if (pCreature != 0)
    {
        pCreature->PerformCombatFeat(m_nActionParam, pCreature);
    }
}
// PC 0x00734C70 GOG 0x00734A20
void CSWGuiTutorialBox::PerformForcePower(CSWGuiControl* pControl)
{
    CSWCCreature* pCreature =
        g_pAppManager->GetClientExoApp()->GetCreatureByGameObjectID(
            m_oidInstigator);
    MessageBoxUnpause(0);
    if (pCreature != 0)
    {
        Vector vTarget;
        pCreature->PerformCastSpell(m_nActionParam, pCreature->m_nId, vTarget,
                                    (unsigned char)m_oidTarget, 0, 0, 0);
    }
}
// PC 0x00734D00 GOG 0x00734AB0
void CSWGuiTutorialBox::PerformUseGrenade(CSWGuiControl* pControl)
{
    CSWCCreature* pInstigator =
        g_pAppManager->GetClientExoApp()->GetCreatureByGameObjectID(
            m_oidInstigator);
    CSWCCreature* pTarget =
        g_pAppManager->GetClientExoApp()->GetCreatureByGameObjectID(
            m_oidTarget);
    MessageBoxUnpause(0);
    if (pInstigator != 0)
    {
        pInstigator->PerformUseHostileItem(m_nActionParam, pTarget);
    }
}
// PC 0x00734D80 GOG 0x00734B30
void CSWGuiTutorialBox::PerformSetMine(CSWGuiControl* pControl)
{
    CSWCCreature* pCreature =
        g_pAppManager->GetClientExoApp()->GetCreatureByGameObjectID(
            m_oidInstigator);
    MessageBoxUnpause(0);
    if (pCreature != 0)
    {
        pCreature->PerformUseItem(m_oidTarget, 0, OBJECT_INVALID,
                                  pCreature->m_vPosition);
    }
}
// PC 0x00734DF0 GOG 0x00734BA0
void CSWGuiTutorialBox::PerformPlayerFlourishWeapons(CSWGuiControl* pControl)
{
    MessageBoxUnpause(0);
    g_pAppManager->m_pClientExoApp->PlayerFlourishWeapons();
}
// PC 0x00734E30 GOG 0x00734BE0
void CSWGuiTutorialBox::PerformBash(CSWGuiControl* pControl)
{
    CSWCCreature* pCreature =
        g_pAppManager->GetClientExoApp()->GetCreatureByGameObjectID(
            m_oidInstigator);
    MessageBoxUnpause(0);
    if (pCreature != 0)
    {
        if (m_nActionParam != 0)
        {
            CSWCDoor* pDoor = (CSWCDoor*)g_pAppManager->GetClientExoApp()
                                  ->GetDoorByGameObjectID(m_oidTarget);
            if (pDoor != 0)
            {
                pDoor->MenuActionBash(0, pCreature);
            }
        }
        else
        {
            CSWCPlaceable* pPlaceable =
                g_pAppManager->GetClientExoApp()->GetPlaceableByGameObjectID(
                    m_oidTarget);
            if (pPlaceable != 0)
            {
                pPlaceable->BashPlaceable(0, pCreature);
            }
        }
    }
}
// PC 0x00734ED0 GOG 0x00734C80
void CSWGuiTutorialBox::PerformAttack(CSWGuiControl* pControl)
{
    CSWCCreature* pCreature =
        g_pAppManager->GetClientExoApp()->GetCreatureByGameObjectID(
            m_oidInstigator);
    MessageBoxUnpause(0);
    if (pCreature != 0)
    {
        CSWCCreature* pTarget =
            g_pAppManager->GetClientExoApp()->GetCreatureByGameObjectID(
                m_oidTarget);
        if (pTarget != 0)
        {
            pTarget->ActionMenuAttack(0, pCreature);
        }
    }
}

void CSWGuiTutorialBox::MessageBoxPause(CSWGuiControl* pControl)
{
    SWGUIINGAMETUTORIALBOX_TRACE();
    int bPaused = g_pAppManager->GetClientExoApp()->GetPausedByCombat();
    m_bWasPausedByCombat = bPaused;
    if (bPaused == 0)
    {
        g_pAppManager->GetClientExoApp()->SetPausedByCombat(1, 2, 0);
    }
}
// PC 0x00734F40 GOG 0x00734CF0
void CSWGuiTutorialBox::PerformNonCombatXButton(CSWGuiControl* pControl)
{
    MessageBoxUnpause(0);
#ifdef _XBOX
    g_pAppManager->m_pClientExoApp->GetInGameGui()
        ->m_pMainInterface->OnNonCombatXButton();
#else
    g_pAppManager->m_pClientExoApp->GetInGameGui();
#endif
}
// PC 0x00734F40 GOG 0x00734CF0
void CSWGuiTutorialBox::PerformCombatXButton(CSWGuiControl* pControl)
{
    MessageBoxUnpause(0);
#ifdef _XBOX
    CGuiInGame* pInGame = g_pAppManager->m_pClientExoApp->GetInGameGui();
    pInGame->m_pMainInterface->OnCombatXButton(GUIEVENT_TOGGLE_MODE);
#else
    g_pAppManager->m_pClientExoApp->GetInGameGui();
#endif
}
// PC 0x00734F80 GOG 0x00734D30
void CSWGuiTutorialBox::PerformCombatYButton(CSWGuiControl* pControl)
{
    MessageBoxUnpause(0);
    g_pAppManager->m_pClientExoApp->GetInGameGui()
        ->m_pMainInterface->OnCombatYButton();
}
// PC 0x00734FD0 GOG 0x00734D80
void CSWGuiTutorialBox::PerformReturnToBase(CSWGuiControl* pControl)
{
    MessageBoxUnpause(0);
    ((CSWGuiInGameMap*)g_pAppManager->m_pClientExoApp->GetInGameGui()
         ->GetGuiInGameScreenPanel(6))
        ->OnXButton(0);
}
// PC 0x00735020 GOG 0x00734DD0
int CSWGuiTutorialBox::SetNextMessage()
{
    SWGUIINGAMETUTORIALBOX_TRACE();
    CExoString sColumn;
    CExoString sIconResRef;
    unsigned long nStrRef = 0;
    sColumn.Format("Message_PC%i", (unsigned char)m_nMessageIndex);
    ++m_nMessageIndex;
    CTwoDimArrays* p2DArrays = g_pRules->Get2DArrays();
    EXOXBOXASSERT(p2DArrays->m_pTutorial);
    C2DA* pTutorial = p2DArrays->m_pTutorial;
    if (pTutorial->GetINTEntry(m_nTutorialReason, sColumn, (int*)&nStrRef) != 0)
    {
        int bFound;
        sColumn.Format("Message_PC%i", (unsigned char)m_nMessageIndex);
        int nStrRef2;
        p2DArrays = g_pRules->Get2DArrays();
        EXOXBOXASSERT(p2DArrays->m_pTutorial);
        pTutorial = p2DArrays->m_pTutorial;
        if (pTutorial->GetINTEntry(
                m_nTutorialReason, sColumn, &nStrRef2) != 0)
        {
            SetOkButtonMessage(38623);
        }
        else
        {
            SetOkButtonMessage(1580);
        }
        {
            CExoString sIconColumn("Icon");
            p2DArrays = g_pRules->Get2DArrays();
            EXOXBOXASSERT(p2DArrays->m_pTutorial);
            pTutorial = p2DArrays->m_pTutorial;
            bFound = pTutorial->GetCExoStringEntry(
                m_nTutorialReason, sIconColumn, &sIconResRef);
        }
#ifndef _XBOX
        if (bFound != 0 &&
            !sIconResRef.CompareNoCase("WHITEBUTTON") &&
            !sIconResRef.CompareNoCase("STARTBUTTON") &&
            !sIconResRef.CompareNoCase("XBUTTON") &&
            !sIconResRef.CompareNoCase("YBUTTON") &&
            !sIconResRef.CompareNoCase("Abutton") &&
            !sIconResRef.CompareNoCase("Backbutton") &&
            !sIconResRef.CompareNoCase("Bbutton") &&
            !sIconResRef.CompareNoCase("Blbutton") &&
            !sIconResRef.CompareNoCase("Whbutton") &&
            !sIconResRef.CompareNoCase("Xbutton") &&
            !sIconResRef.CompareNoCase("Ybutton") &&
            !sIconResRef.CompareNoCase("uibut_Start") &&
            !sIconResRef.CompareNoCase("uibut_X") &&
            !sIconResRef.CompareNoCase("uibut_Y"))
        {
            CResRef cIcon(sIconResRef);
            SetIcon(cIcon);
        }
#else
        if (bFound != 0) SetIcon(sIconResRef);
#endif
        SetAllowCancel(0);
        SetMessage(nStrRef);
    }
    return nStrRef;
}
// PC 0x00735300 GOG 0x00734860
CSWGuiTutorialBox::CSWGuiTutorialBox(CSWGuiManager* pManager)
    : CSWGuiMessageBox(pManager)
{
    m_oidInstigator = OBJECT_INVALID;
    m_oidTarget = OBJECT_INVALID;
    m_nActionParam = 0;
    m_bWasPausedByCombat = 0;
    m_nTutorialReason = 0xFF;
    m_nMessageIndex = 0;
#ifdef _XBOX
    m_lblMessage.m_cText.m_cParams.SetFont("fnt_d16x16");
#else
    CResRef cFont("fnt_d16x16");
    m_lblMessage.m_cText.m_cParams.SetBaseFont(cFont);
#endif
    m_nOpenSound = 15;
}
// PC 0x007353A0 GOG 0x007350C0
void CSWGuiTutorialBox::OnPanelAdded()
{
    CSWGuiMessageBox::OnPanelAdded();
    MessageBoxPause(0);
}
// PC 0x007353E0 GOG 0x00735100
void CSWGuiTutorialBox::HandleInputEvent(int nEvent, int nState)
{
    if (nState != 0)
    {
        switch (nEvent)
        {
        case 39:
        case 40:
        case 45:
        case 46:
#ifndef _XBOX
        case 502:
#endif
        {
            int result = SetNextMessage();
            if (result != 0)
            {
                return;
            }
            break;
        }
        }
    }
    return CSWGuiMessageBox::HandleInputEvent(nEvent, nState);
}
// PC 0x00735440 GOG 0x00735160
int CSWGuiTutorialBox::SetTutorialReason(unsigned char nReason)
{
    m_nTutorialReason = nReason;
    m_nMessageIndex = 0;
    if (!SetNextMessage())
    {
        return FALSE;
    }
    switch (nReason)
    {
    case 0:
        CSWGuiMessageBox::SetCallback(
            this, (CSWGuiHandler)&CSWGuiTutorialBox::PerformCombatFeat, 0);
        return TRUE;
    case 1:
        CSWGuiMessageBox::SetCallback(
            this, (CSWGuiHandler)&CSWGuiTutorialBox::PerformUseGrenade, 0);
        return TRUE;
    case 2:
        CSWGuiMessageBox::SetCallback(
            this, (CSWGuiHandler)&CSWGuiTutorialBox::PerformSetMine, 0);
        return TRUE;
    case 3:
    case 4:
        CSWGuiMessageBox::SetCallback(
            this, (CSWGuiHandler)&CSWGuiTutorialBox::PerformForcePower, 0);
        return TRUE;
    case 0x1F:
        CSWGuiMessageBox::SetCallback(
            this,
            (CSWGuiHandler)&CSWGuiTutorialBox::PerformPlayerFlourishWeapons, 0);
        return TRUE;
    case CClientExoApp::TUTORIAL_BASH:
        CSWGuiMessageBox::SetCallback(
            this, (CSWGuiHandler)&CSWGuiTutorialBox::PerformBash, 0);
        return TRUE;
    case 0x22:
        CSWGuiMessageBox::SetCallback(
            this, (CSWGuiHandler)&CSWGuiTutorialBox::PerformAttack, 0);
        return TRUE;
    case 0x1D:
        CSWGuiMessageBox::SetCallback(
            this, (CSWGuiHandler)&CSWGuiTutorialBox::PerformNonCombatXButton,
            0);
        return TRUE;
    case 0x1E:
        CSWGuiMessageBox::SetCallback(
            this, (CSWGuiHandler)&CSWGuiTutorialBox::PerformCombatXButton, 0);
        return TRUE;
    case 0x20:
        CSWGuiMessageBox::SetCallback(
            this, (CSWGuiHandler)&CSWGuiTutorialBox::PerformCombatYButton, 0);
        return TRUE;
    case 0x27:
        CSWGuiMessageBox::SetCallback(
            this, (CSWGuiHandler)&CSWGuiTutorialBox::PerformReturnToBase, 0);
        return TRUE;
    default:
        CSWGuiMessageBox::SetCallback(
            this, (CSWGuiHandler)&CSWGuiTutorialBox::MessageBoxUnpause, 0);
        return TRUE;
    }
}

static inline void SwguiingametutorialboxTrace2();

static inline void SwguiingametutorialboxTrace1()
{
    SwguiingametutorialboxTrace2();
}

static inline void SwguiingametutorialboxTrace2()
{
}
