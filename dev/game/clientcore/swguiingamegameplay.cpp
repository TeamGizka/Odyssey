#include "exobase.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "clientoptions.h"
#include "nwclass.h"
#include "twodimarrays.h"
#include "twodimarray.h"
#include "guiingame.h"
#include "swguiingamegameplay.h"
#include "swguiinOPTKeyMapping.h"
#include "nwrules.h"
// PC 0x006E6170
void CSWGuiInGameGameplay::OnPanelAdded()
{
    CSWGuiPanel::OnPanelAdded();
}
// PC 0x006E6180
void CSWGuiInGameGameplay::HandleInputEvent(int nEvent, int nState)
{
    if (nState != 0)
    {
        switch (nEvent)
        {
        case GUIEVENT_CANCEL:
        case GUIEVENT_CANCEL_ALTERNATE:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            m_pManager->PopModalPanel();
            m_bDeferredRemoval = 0;
            m_nExitState = EXITSTATE_DELETE;
            break;
        case GUIEVENT_ACCEPT:
            if (m_pActiveControl != 0 &&
                m_pActiveControl->AsButtonToggle() != 0)
            {
                m_pManager->PlayGuiSound(3);
                CSWGuiButtonToggle* pToggle =
                    m_pActiveControl->AsButtonToggle();
                m_pActiveControl->AsButtonToggle()->SetToggled(
                    !pToggle->m_bToggled);
            }
            break;
        case GUIEVENT_SCROLL_UP:
            m_lbxDescription.HandleInputEvent(GUIEVENT_UP, 1);
            break;
        case GUIEVENT_SCROLL_DOWN:
            m_lbxDescription.HandleInputEvent(GUIEVENT_DOWN, 1);
            break;
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
// PC 0x006E62A0
void CSWGuiInGameGameplay::OnAutoLevelUp(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        int bAutoLevelUp = !g_pAppManager->m_pClientExoApp->GetClientOptions()
                                ->m_bAutoLevelUpNPCs;
        g_pAppManager->m_pClientExoApp->GetClientOptions()->SetAutoLevelUpNPCs(
            bAutoLevelUp);
    }
}
// PC 0x006E62E0
void CSWGuiInGameGameplay::OnAutoSave(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        int bAutoSave =
            !g_pAppManager->m_pClientExoApp->GetClientOptions()->m_bAutoSave;
        CClientOptions* pOptions =
            g_pAppManager->m_pClientExoApp->GetClientOptions();
        pOptions->m_bAutoSave = bAutoSave;
    }
}
// PC 0x006E6330
void CSWGuiInGameGameplay::OnInvertCam(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        int bInvertCam =
            !g_pAppManager->m_pClientExoApp->GetClientOptions()->m_bMouseLook;
        CClientOptions* pOptions =
            g_pAppManager->m_pClientExoApp->GetClientOptions();
        pOptions->m_bMouseLook = bInvertCam;
    }
}
// PC 0x006E6380
void CSWGuiInGameGameplay::OnReverse(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        int bReverse = !g_pAppManager->m_pClientExoApp->GetClientOptions()
                            ->m_bReverseMinigameYAxis;
        CClientOptions* pOptions =
            g_pAppManager->m_pClientExoApp->GetClientOptions();
        pOptions->m_bReverseMinigameYAxis = bReverse;
    }
}
// PC 0x006E63D0
void CSWGuiInGameGameplay::OnCombatMovement(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        int bCombatMovement =
            !g_pAppManager->m_pClientExoApp->GetClientOptions()
                 ->m_bCombatMovement;
        CClientOptions* pOptions =
            g_pAppManager->m_pClientExoApp->GetClientOptions();
        pOptions->m_bCombatMovement = bCombatMovement;
    }
}
// PC 0x006E6420
void CSWGuiInGameGameplay::OnKeyMap(CSWGuiControl* pControl)
{
#ifndef _XBOX
    int bSelected = pControl->m_bSelected;
    if (bSelected != 0)
    {
        CSWGuiInGameOptKeyMappings* pPanel =
            new CSWGuiInGameOptKeyMappings(GetManager());
        m_pManager->AddPanel(pPanel, CSWGuiManager::PANEL_MODE_MODAL, 1);
    }
#endif
}
// PC 0x006E64A0
void CSWGuiInGameGameplay::OnMouse(CSWGuiControl* pControl)
{
    int bSelected = pControl->m_bSelected;
    if (bSelected != 0)
    {
        CSWGuiOptionsMouse* pPanel = new CSWGuiOptionsMouse(GetManager());
        m_pManager->AddPanel(pPanel, CSWGuiManager::PANEL_MODE_MODAL, 1);
    }
}
// PC 0x006E6520  Xbox 0x0011F700
void CSWGuiInGameGameplay::SetDifficultyText()
{
    int nNameStrRef;
    int bFound;
    {
        CExoString sColumn("NAME");
        CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
        C2DA* pDifficulty = p2DArrays->m_pDifficultyOptions;
        bFound = pDifficulty->GetINTEntry(m_nDifficulty, sColumn, &nNameStrRef);
    }
    if (bFound)
    {
        m_btnDifficulty.m_cText.m_cParams.SetStrRef(nNameStrRef);
    }

    m_btnDiffLeft.SetEnabledFlag(1);
    m_btnDiffRight.SetEnabledFlag(1);
    if (m_nDifficulty == 0)
    {
        m_btnDiffLeft.SetEnabledFlag(0);
    }
    else if (m_nDifficulty ==
             g_pRules->m_p2DArrays->m_pDifficultyOptions->m_nNumRows - 2)
    {
        m_btnDiffRight.SetEnabledFlag(0);
    }
}
// PC 0x006E6620
void CSWGuiInGameGameplay::SetDescription(unsigned long nStrRef)
{
    m_lblDescription.m_cText.m_cParams.SetStrRef(nStrRef);
    CExoString sText = m_lblDescription.m_cText.m_cParams.m_sText;
    m_lbxDescription.ClearItems();
    CSWGuiExtent cExtent = m_lblDescription.m_cExtent;
    if (sText.GetLength() > 0)
    {
        cExtent.m_nHeight = m_lblDescription.m_cText.GetIdealHeight();
    }
    else
    {
        cExtent.m_nHeight = m_lblDescription.m_cText.GetFontHeight();
    }
    CSWGuiObject* pLabel = &m_lblDescription;
    pLabel->SetExtent(cExtent);
    CSWGuiControl* pControl = &m_lblDescription;
    m_lbxDescription.AddControls(&pControl, 1, 0, 0, 0);
    m_lbxDescription.SetSelectedControl(&m_lblDescription, 0);
}
// PC 0x006E6710
void CSWGuiInGameGameplay::OnEnterDiff(CSWGuiControl* pControl)
{
    CSWGuiTextParams* pText = &m_btnDifficulty.m_cText.m_cParams;
    pText->SetColor(CGuiInGame::COLOR_YELLOW);
    pText->m_bHilightText = 1;
    SetDifficultyText();
    SetDescription(0xA519);
}
// PC 0x006E6750
void CSWGuiInGameGameplay::OnExitDiff(CSWGuiControl* pControl)
{
    CSWGuiTextParams* pText = &m_btnDifficulty.m_cText.m_cParams;
    pText->SetColor(CGuiInGame::COLOR_BLUE);
    pText->m_bHilightText = 0;
}
// PC 0x006E6770
void CSWGuiInGameGameplay::SetUpOptions()
{
    CClientOptions* pOptions =
        g_pAppManager->m_pClientExoApp->GetClientOptions();
    if (pOptions != 0)
    {
        CSWGuiButtonToggle* pToggle;
        int bValue;
        m_nDifficulty = pOptions->m_nDifficulty;
        SetDifficultyText();
        bValue = pOptions->m_bAutoLevelUpNPCs;
        pToggle = m_cbAutoLevelUp.AsButtonToggle();
        pToggle->m_bToggled = bValue;
        bValue = (pOptions->m_bMouseLook != 0);
        pToggle = m_cbInvertCam.AsButtonToggle();
        pToggle->m_bToggled = bValue;
        bValue = (pOptions->m_bAutoSave != 0);
        pToggle = m_cbAutoSave.AsButtonToggle();
        pToggle->m_bToggled = bValue;
        bValue = (pOptions->m_bReverseMinigameYAxis != 0);
        pToggle = m_cbReverse.AsButtonToggle();
        pToggle->m_bToggled = bValue;
        bValue = (pOptions->m_bCombatMovement != 0);
        pToggle = m_cbCombatMovement.AsButtonToggle();
        pToggle->m_bToggled = bValue;
    }
}
// PC 0x006E68A0
void CSWGuiInGameGameplay::OnEnterSlot(CSWGuiControl* pControl)
{
    SetDescription(pControl->m_nIndex);
}
// PC 0x006E68B0
void CSWGuiInGameGameplay::OnDefault(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        g_pAppManager->m_pClientExoApp->GetClientOptions()
            ->SetDefaultGameplayOptions();
        SetUpOptions();
    }
}
// PC 0x006E68E0
void CSWGuiInGameGameplay::OnDifficultyLeft(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        if (m_nDifficulty > 0)
        {
            char nStored = --m_nDifficulty;
            g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nDifficulty =
                nStored;
            SetDifficultyText();
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
        }
    }
}
// PC 0x006E6930
void CSWGuiInGameGameplay::OnDifficultyRight(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        if (m_nDifficulty !=
            g_pRules->m_p2DArrays->m_pDifficultyOptions->m_nNumRows - 2)
        {
            ++m_nDifficulty;
            CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
            char nStored = m_nDifficulty;
            pClient->GetClientOptions()->m_nDifficulty = nStored;
            SetDifficultyText();
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
        }
    }
}
// PC 0x006E69A0
CSWGuiInGameGameplay::CSWGuiInGameGameplay(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    StartLoadFromLayout("optgameplay");
    InitControl(m_btnDifficulty, "BTN_DIFFICULTY", 1);
    InitControl(m_btnDiffRight, "BTN_DIFFRIGHT", 1);
    InitControl(m_btnDiffLeft, "BTN_DIFFLEFT", 1);
    InitControl(m_cbInvertCam, "CB_INVERTCAM", 1);
    InitControl(m_cbAutoLevelUp, "CB_LEVELUP", 1);
    InitControl(m_cbAutoSave, "CB_AUTOSAVE", 1);
    InitControl(m_cbReverse, "CB_REVERSE", 1);
    InitControl(m_cbCombatMovement, "CB_DISABLEMOVE", 1);
    InitControl(m_sliMouseSen, "SLI_MOUSESEN", 1);
    InitControl(m_lblMouseSen, "LBL_MOUSESEN", 1);
    InitControl(m_lblTitle, "LBL_TITLE", 1);
    InitControl(m_lbxDescription, "LB_DESC", 1);
    InitControl(m_btnBack, "BTN_BACK", 1);
    InitControl(m_btnDefault, "BTN_DEFAULT", 1);
    InitControl(m_btnKeyMap, "BTN_KEYMAP", 1);
    InitControl(m_btnMouse, "BTN_MOUSE", 1);
    m_lbxDescription.m_nSelectSound = CSWGuiManager::GUISOUND_SELECTION;
    StopLoadFromLayout();

    m_lblDescription.Initialize(m_lbxDescription.m_pProtoItem->m_cExtent,
                                (CSWGuiLabel*)m_lbxDescription.m_pProtoItem,
                                1.0f);

    m_btnDifficulty.AddEvent(GUIEVENT_LEFT, this,
                             (PFNGUIEVENT)&CSWGuiInGameGameplay::OnDifficultyLeft);
    m_btnDifficulty.AddEvent(GUIEVENT_RIGHT, this,
                             (PFNGUIEVENT)&CSWGuiInGameGameplay::OnDifficultyRight);
    m_btnDifficulty.AddEvent(CSWGuiControl::EVENT_ENTER, this,
                             (PFNGUIEVENT)&CSWGuiInGameGameplay::OnEnterDiff);
    m_btnDifficulty.AddEvent(CSWGuiControl::EVENT_EXIT, this,
                             (PFNGUIEVENT)&CSWGuiInGameGameplay::OnExitDiff);
    m_btnDiffLeft.SetFocusable(0);
    m_btnDiffLeft.AddEvent(GUIEVENT_ACTIVATE, this,
                           (PFNGUIEVENT)&CSWGuiInGameGameplay::OnDifficultyLeft);
    m_btnDiffRight.SetFocusable(0);
    m_btnDiffRight.AddEvent(GUIEVENT_ACTIVATE, this,
                            (PFNGUIEVENT)&CSWGuiInGameGameplay::OnDifficultyRight);
    m_cbAutoLevelUp.AddEvent(GUIEVENT_ACTIVATE, this,
                             (PFNGUIEVENT)&CSWGuiInGameGameplay::OnAutoLevelUp);
    m_cbAutoLevelUp.AddEvent(GUIEVENT_ACCEPT, this,
                             (PFNGUIEVENT)&CSWGuiInGameGameplay::OnAutoLevelUp);
    m_cbAutoLevelUp.AddEvent(CSWGuiControl::EVENT_ENTER, this,
                             (PFNGUIEVENT)&CSWGuiInGameGameplay::OnEnterSlot);
    m_cbInvertCam.AddEvent(GUIEVENT_ACTIVATE, this,
                           (PFNGUIEVENT)&CSWGuiInGameGameplay::OnInvertCam);
    m_cbInvertCam.AddEvent(GUIEVENT_ACCEPT, this,
                           (PFNGUIEVENT)&CSWGuiInGameGameplay::OnInvertCam);
    m_cbInvertCam.AddEvent(CSWGuiControl::EVENT_ENTER, this,
                           (PFNGUIEVENT)&CSWGuiInGameGameplay::OnEnterSlot);
    m_cbAutoSave.AddEvent(GUIEVENT_ACTIVATE, this,
                          (PFNGUIEVENT)&CSWGuiInGameGameplay::OnAutoSave);
    m_cbAutoSave.AddEvent(GUIEVENT_ACCEPT, this,
                          (PFNGUIEVENT)&CSWGuiInGameGameplay::OnAutoSave);
    m_cbAutoSave.AddEvent(CSWGuiControl::EVENT_ENTER, this,
                          (PFNGUIEVENT)&CSWGuiInGameGameplay::OnEnterSlot);
    m_cbReverse.AddEvent(GUIEVENT_ACTIVATE, this,
                         (PFNGUIEVENT)&CSWGuiInGameGameplay::OnReverse);
    m_cbReverse.AddEvent(GUIEVENT_ACCEPT, this,
                         (PFNGUIEVENT)&CSWGuiInGameGameplay::OnReverse);
    m_cbReverse.AddEvent(CSWGuiControl::EVENT_ENTER, this,
                         (PFNGUIEVENT)&CSWGuiInGameGameplay::OnEnterSlot);
    m_cbCombatMovement.AddEvent(GUIEVENT_ACTIVATE, this,
                                (PFNGUIEVENT)&CSWGuiInGameGameplay::OnCombatMovement);
    m_cbCombatMovement.AddEvent(GUIEVENT_ACCEPT, this,
                                (PFNGUIEVENT)&CSWGuiInGameGameplay::OnCombatMovement);
    m_cbCombatMovement.AddEvent(CSWGuiControl::EVENT_ENTER, this,
                                (PFNGUIEVENT)&CSWGuiInGameGameplay::OnEnterSlot);
    m_btnKeyMap.SetFocusable(0);
    m_btnKeyMap.AddEvent(GUIEVENT_ACTIVATE, this, (PFNGUIEVENT)&CSWGuiInGameGameplay::OnKeyMap);
    m_btnMouse.SetFocusable(0);
    m_btnMouse.AddEvent(GUIEVENT_ACTIVATE, this, (PFNGUIEVENT)&CSWGuiInGameGameplay::OnMouse);
    m_btnBack.SetFocusable(0);
    m_btnBack.AddEvent(GUIEVENT_ACTIVATE, this,
                       (PFNGUIEVENT)&CSWGuiPanel::OnBButtonPressed);
    m_btnDefault.SetFocusable(0);
    m_btnDefault.AddEvent(GUIEVENT_ACTIVATE, this, (PFNGUIEVENT)&CSWGuiInGameGameplay::OnDefault);

    m_cbInvertCam.m_nIndex = STRREF_INVERT_CAMERA;
    m_cbAutoLevelUp.m_nIndex = STRREF_AUTO_LEVELUP;
    m_cbAutoSave.m_nIndex = STRREF_AUTO_SAVE;
    m_cbReverse.m_nIndex = STRREF_REVERSE_MOUSE;
    m_cbCombatMovement.m_nIndex = STRREF_COMBAT_MOVEMENT;
    m_btnMouse.m_nIndex = STRREF_MOUSE_OPTIONS;
    m_btnKeyMap.m_nIndex = STRREF_KEYMAP_OPTIONS;

    SetActiveControl(&m_btnDifficulty, 0);
    SetUpOptions();
}
