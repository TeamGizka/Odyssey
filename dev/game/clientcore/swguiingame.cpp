#include "swguiingame.h"
#include "exotlktable.h"

template void CExoArrayList<CSWGuiControl*>::Allocate(int);

static inline void SwguiingameTrace2();
static inline void SwguiingameTrace1();
#define SWGUIINGAME_TRACE() SwguiingameTrace1()
// PC 0x00624970
void CSWGuiInGameMenu::HandleInputEvent(int nEvent, int nState)
{
    switch (nEvent)
    {
    case 243:
        if (nState & 1)
            HandleInputEvent(GUIEVENT_PREVIOUS, 1);
        break;
    case 244:
        if (nState & 1)
            HandleInputEvent(GUIEVENT_NEXT, 1);
        break;
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
// PC 0x006249D0  Xbox 0x0004CD70
void CSWGuiMessageBox::SetMessage(unsigned long nStrRef)
{
    SetMessage(g_pTlkTable->GetSimpleString(nStrRef));
}

#include "swgui.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "serverexoapp.h"
#include "partytables.h"
#include "nwsitem.h"
#include "nwcjournal.h"
#include "guiingame.h"
#include "gameobjectarray.h"
#include "clientexoappinternal.h"
#include "nwccreature.h"
#include "swguiingame.inl"
// PC 0x00624A40  Xbox 0x0004CDA0
void CSWGuiMessageBox::SetCallback(CSWGuiObject* pObject,
                                  CSWGuiHandler pHandler,
                                  CSWGuiControl* pControl)
{
    m_pCallbackObject = pObject;
    m_pCallback = pHandler;
    m_pCallbackControl = pControl;
}
// PC 0x00624A60
void CSWGuiStatusSummary::OnPanelRemoved()
{
#ifndef _XBOX
    g_pGuiMan->RestoreOldMousePosition();
#endif
}
// PC 0x00624A70  Xbox 0x0004CDC0
void CSWGuiStatusSummary::AddAlignmentShift(char nShift)
{
    if (nShift != 0)
    {
        if (nShift > 0)
            m_cStatusSummary.m_nLightShift += nShift;
        else
            m_cStatusSummary.m_nDarkShift -= nShift;
        m_cStatusSummary.m_bDisplaySPending = 1;
    }
}
// PC 0x00624AB0  Xbox 0x0004CE00
void CSWGuiStatusSummary::AddCredits(int nCredits)
{
    if (nCredits != 0)
    {
        if ((m_cStatusSummary.m_nCredits > 0 && nCredits < 0) ||
            (m_cStatusSummary.m_nCredits < 0 && nCredits > 0))
        {
            m_cStatusSummary.m_bCreditsNet = 1;
        }
        m_cStatusSummary.m_nCredits += nCredits;
        m_cStatusSummary.m_bDisplaySPending = 1;
    }
}
// PC 0x00624AE0
void CSWGuiToolTipPanel::SetToolTipText(const CExoString& sText)
{
    m_lblToolTip.m_cText.m_cParams.SetText(sText);
}

#ifndef _XBOX
void CSWGuiToolTipPanel::PositionToolTip()  // PC 0x00624AF0
{
    CSWGuiExtent cExtent;
    cExtent.m_nHeight = 0;
    cExtent.m_nWidth = 0;
    cExtent.m_nTop = 0;
    cExtent.m_nLeft = 0;

    CSWGuiText* pText = m_lblToolTip.m_cText.m_cParams.m_pText;
    if (pText)
    {
        cExtent = pText->GetIdealWidthAndHeight();
        cExtent.m_nHeight += 8;
        cExtent.m_nWidth += 8;

        int nX = g_pGuiMan->m_nMouseX + 15;
        int nY = g_pGuiMan->m_nMouseY + 15;
        if (nX + cExtent.m_nWidth >= g_nScreenWidth)
        {
            nX = g_nScreenWidth - cExtent.m_nWidth - 2;
        }
        if (nY + cExtent.m_nHeight >= g_nScreenHeight)
        {
            nY = g_nScreenHeight - cExtent.m_nHeight - 2;
        }
        cExtent.m_nTop = nY;
        cExtent.m_nLeft = nX;
        m_lblToolTip.SetExtent(cExtent);
    }
}
#endif
// PC 0x00624BD0  Xbox 0x0004CF50
void CSWGuiInGameMenu::SetActiveControlID(int nControl, int bActive)
{
    SWGUIINGAME_TRACE();
    if (nControl >= 0 && nControl < m_lControls.num)
        SetActiveControl(m_lControls.element[nControl], bActive);
}
// PC 0x00624C00  Xbox 0x0004CF80
void CSWGuiInGameMenu::OnShoulderLeft(CSWGuiControl* pControl)
{
    SWGUIINGAME_TRACE();
    if (pControl->m_bSelected != 0)
    {
        g_pAppManager->m_pClientExoApp->GetInGameGui()->PrevSWInGameGui();
    }
}
// PC 0x00624C30  Xbox 0x0004CFB0
void CSWGuiInGameMenu::OnShoulderRight(CSWGuiControl* pControl)
{
    SWGUIINGAME_TRACE();
    if (pControl->m_bSelected != 0)
    {
        g_pAppManager->m_pClientExoApp->GetInGameGui()->NextSWInGameGui();
    }
}
// PC 0x00624C60  Xbox 0x0004CFE0
void CSWGuiInGameMenu::Update(float fElapsed)
{
    SWGUIINGAME_TRACE();
    if (g_pAppManager->m_pServerExoApp->GetPartyTable()->GetPartyItems()->m_nNewItems > 0)
    {
        if (!m_LblhInv.m_cBorder.m_cParams.m_bPulsing)
            m_LblhInv.m_cBorder.m_cParams.SetPulsingAlpha(1, 0, 1);
    }
    else
    {
        m_LblhInv.m_cBorder.m_cParams.SetPulsingAlpha(0, 0, 0);
    }

    if (g_pAppManager->m_pClientExoApp->GetQuestJournal()->HasNewEntries())
    {
        if (!m_LblhJou.m_cBorder.m_cParams.m_bPulsing)
            m_LblhJou.m_cBorder.m_cParams.SetPulsingAlpha(1, 0, 1);
    }
    else
    {
        m_LblhJou.m_cBorder.m_cParams.SetPulsingAlpha(0, 0, 0);
    }
}
// PC 0x00624CF0
void CSWGuiInGameMenu::OnEquipButtonPressed(CSWGuiControl* pControl)
{
    SWGUIINGAME_TRACE();
    g_pAppManager->GetClientExoApp()->GetInGameGui()->SwitchToSWInGameGui(
        CGuiInGame::SCREEN_EQUIP);
}
// PC 0x00624D10  Xbox 0x0004E7E0
void CSWGuiInGameMenu::OnInvButtonPressed(CSWGuiControl* pControl)
{
    SWGUIINGAME_TRACE();
    g_pAppManager->GetClientExoApp()->GetInGameGui()->SwitchToSWInGameGui(
        CGuiInGame::SCREEN_INVENTORY);
}
// PC 0x00624D30  Xbox 0x0004D070
void CSWGuiInGameMenu::OnCharButtonPressed(CSWGuiControl* pControl)
{
    SWGUIINGAME_TRACE();
    g_pAppManager->GetClientExoApp()->GetInGameGui()->SwitchToSWInGameGui(
        CGuiInGame::SCREEN_CHARACTER);
}
// PC 0x00624D50
void CSWGuiInGameMenu::OnMapButtonPressed(CSWGuiControl* pControl)
{
    SWGUIINGAME_TRACE();
    g_pAppManager->GetClientExoApp()->GetInGameGui()->SwitchToSWInGameGui(
        CGuiInGame::SCREEN_MAP);
}
// PC 0x00624D70
void CSWGuiInGameMenu::OnAbiButtonPressed(CSWGuiControl* pControl)
{
    SWGUIINGAME_TRACE();
    g_pAppManager->GetClientExoApp()->GetInGameGui()->SwitchToSWInGameGui(
        CGuiInGame::SCREEN_ABILITIES);
}
// PC 0x00624D90
void CSWGuiInGameMenu::OnJouButtonPressed(CSWGuiControl* pControl)
{
    SWGUIINGAME_TRACE();
    g_pAppManager->GetClientExoApp()->GetInGameGui()->SwitchToSWInGameGui(
        CGuiInGame::SCREEN_JOURNAL);
}
// PC 0x00624DB0
void CSWGuiInGameMenu::OnOptButtonPressed(CSWGuiControl* pControl)
{
    SWGUIINGAME_TRACE();
    g_pAppManager->GetClientExoApp()->GetInGameGui()->SwitchToSWInGameGui(
        CGuiInGame::SCREEN_OPTIONS);
}
// PC 0x00624DD0
void CSWGuiInGameMenu::OnMsgButtonPressed(CSWGuiControl* pControl)
{
    SWGUIINGAME_TRACE();
    g_pAppManager->GetClientExoApp()->GetInGameGui()->SwitchToSWInGameGui(
        CGuiInGame::SCREEN_MESSAGES);
}
// PC 0x00624DF0
void CSWGuiInGameMessages::ShowDialogMessages()
{
    SWGUIINGAME_TRACE();
    m_lbMessages.m_bEnabled = 0;
    CSWGuiListBox* pDialog = &m_lbDialog;
    pDialog->m_bEnabled = 1;
#ifdef _XBOX
    m_lblXDesc.m_cText.m_cParams.SetStrRef(STRREF_SHOW_FEEDBACK);
#else
    m_btnSwitch.m_cText.m_cParams.SetStrRef(STRREF_SHOW_FEEDBACK);
#endif
    SetActiveControl(pDialog, 0);
    g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nMessageType =
        CGuiInGame::MESSAGE_TYPE_DIALOG;

#ifndef _XBOX
    CExoString sTitle;
    sTitle = g_pTlkTable->GetSimpleString(STRREF_MESSAGES) +
        " - " + g_pTlkTable->GetSimpleString(STRREF_DIALOG);
    m_lblTitle.m_cText.m_cParams.SetText(sTitle);
#endif
}
// PC 0x006277C0
CSWGuiToolTipPanel::CSWGuiToolTipPanel(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    SwguiingameTrace2();
    if (g_nScreenWidth == 1600)
    {
        CResRef cLayout("tooltip16X12");
        StartLoadFromLayout(cLayout);
    }
    if (g_nScreenWidth == 1280)
    {
        if (g_nScreenHeight == 960)
        {
            CResRef cLayout("tooltip12X9");
            StartLoadFromLayout(cLayout);
        }
        else if (g_nScreenHeight == 1024)
        {
            CResRef cLayout("tooltip12x10");
            StartLoadFromLayout(cLayout);
        }
    }
    else if (g_nScreenWidth == 1024)
    {
        CResRef cLayout("tooltip10X8");
        StartLoadFromLayout(cLayout);
    }
    else if (g_nScreenWidth == 800)
    {
        CResRef cLayout("tooltip8X6");
        StartLoadFromLayout(cLayout);
    }
    else
    {
        CResRef cLayout("tooltip6X4");
        StartLoadFromLayout(cLayout);
    }
#ifndef _XBOX
    m_bPositioned = 1;
#endif
    InitControl(m_lblToolTip, "tooltip", 1);
    StopLoadFromLayout();
}
// PC 0x00627980
CSWGuiInGameMenu::CSWGuiInGameMenu(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    SwguiingameTrace2();
    StartLoadFromLayout("top");
    InitControl(m_LblhEqu, "LBLH_EQU", 1);
    InitControl(m_LblhInv, "LBLH_INV", 1);
    InitControl(m_LblhCha, "LBLH_CHA", 1);
    InitControl(m_LblhMap, "LBLH_MAP", 1);
    InitControl(m_LblhAbi, "LBLH_ABI", 1);
    InitControl(m_LblhJou, "LBLH_JOU", 1);
    InitControl(m_LblhOpt, "LBLH_OPT", 1);
    InitControl(m_LblhMsg, "LBLH_MSG", 1);
    InitControl(m_btnEqu, "BTN_EQU", 1);
    InitControl(m_btnInv, "BTN_INV", 1);
    InitControl(m_btnChar, "BTN_CHAR", 1);
    InitControl(m_btnMap, "BTN_MAP", 1);
    InitControl(m_btnAbi, "BTN_ABI", 1);
    InitControl(m_btnJou, "BTN_JOU", 1);
    InitControl(m_btnOpt, "BTN_OPT", 1);
    InitControl(m_btnMsg, "BTN_MSG", 1);
    StopLoadFromLayout();

    m_LblhEqu.AddEvent(53, this,
        (CSWGuiHandler)&CSWGuiInGameMenu::OnShoulderLeft);
    m_LblhInv.AddEvent(53, this,
        (CSWGuiHandler)&CSWGuiInGameMenu::OnShoulderLeft);
    m_LblhCha.AddEvent(53, this,
        (CSWGuiHandler)&CSWGuiInGameMenu::OnShoulderLeft);
    m_LblhMap.AddEvent(53, this,
        (CSWGuiHandler)&CSWGuiInGameMenu::OnShoulderLeft);
    m_LblhAbi.AddEvent(53, this,
        (CSWGuiHandler)&CSWGuiInGameMenu::OnShoulderLeft);
    m_LblhJou.AddEvent(53, this,
        (CSWGuiHandler)&CSWGuiInGameMenu::OnShoulderLeft);
    m_LblhOpt.AddEvent(53, this,
        (CSWGuiHandler)&CSWGuiInGameMenu::OnShoulderLeft);
    m_LblhMsg.AddEvent(53, this,
        (CSWGuiHandler)&CSWGuiInGameMenu::OnShoulderLeft);
    m_LblhEqu.AddEvent(54, this,
        (CSWGuiHandler)&CSWGuiInGameMenu::OnShoulderRight);
    m_LblhInv.AddEvent(54, this,
        (CSWGuiHandler)&CSWGuiInGameMenu::OnShoulderRight);
    m_LblhCha.AddEvent(54, this,
        (CSWGuiHandler)&CSWGuiInGameMenu::OnShoulderRight);
    m_LblhMap.AddEvent(54, this,
        (CSWGuiHandler)&CSWGuiInGameMenu::OnShoulderRight);
    m_LblhAbi.AddEvent(54, this,
        (CSWGuiHandler)&CSWGuiInGameMenu::OnShoulderRight);
    m_LblhJou.AddEvent(54, this,
        (CSWGuiHandler)&CSWGuiInGameMenu::OnShoulderRight);
    m_LblhOpt.AddEvent(54, this,
        (CSWGuiHandler)&CSWGuiInGameMenu::OnShoulderRight);
    m_LblhMsg.AddEvent(54, this,
        (CSWGuiHandler)&CSWGuiInGameMenu::OnShoulderRight);
    m_btnEqu.AddEvent(CSWGuiControl::EVENT_SELECT, this,
        (CSWGuiHandler)&CSWGuiInGameMenu::OnEquipButtonPressed);
    m_btnInv.AddEvent(CSWGuiControl::EVENT_SELECT, this,
        (CSWGuiHandler)&CSWGuiInGameMenu::OnInvButtonPressed);
    m_btnChar.AddEvent(CSWGuiControl::EVENT_SELECT, this,
        (CSWGuiHandler)&CSWGuiInGameMenu::OnCharButtonPressed);
    m_btnMap.AddEvent(CSWGuiControl::EVENT_SELECT, this,
        (CSWGuiHandler)&CSWGuiInGameMenu::OnMapButtonPressed);
    m_btnAbi.AddEvent(CSWGuiControl::EVENT_SELECT, this,
        (CSWGuiHandler)&CSWGuiInGameMenu::OnAbiButtonPressed);
    m_btnJou.AddEvent(CSWGuiControl::EVENT_SELECT, this,
        (CSWGuiHandler)&CSWGuiInGameMenu::OnJouButtonPressed);
    m_btnOpt.AddEvent(CSWGuiControl::EVENT_SELECT, this,
        (CSWGuiHandler)&CSWGuiInGameMenu::OnOptButtonPressed);
    m_btnMsg.AddEvent(CSWGuiControl::EVENT_SELECT, this,
        (CSWGuiHandler)&CSWGuiInGameMenu::OnMsgButtonPressed);

    m_btnEqu.SetFocusable(0);
    m_btnInv.SetFocusable(0);
    m_btnChar.SetFocusable(0);
    m_btnMap.SetFocusable(0);
    m_btnAbi.SetFocusable(0);
    m_btnJou.SetFocusable(0);
    m_btnOpt.SetFocusable(0);
    m_btnMsg.SetFocusable(0);
    SetActiveControl(&m_btnEqu, 0);
    m_bCenterHorizontal = 1;
    m_bCenterVertical = 1;

    m_btnEqu.SetToolTip(48219, GUIEVENT_MENU_EQUIPMENT);
    m_btnInv.SetToolTip(48220, GUIEVENT_MENU_INVENTORY);
    m_btnChar.SetToolTip(48225, GUIEVENT_MENU_CHARACTER);
    m_btnMap.SetToolTip(48221, GUIEVENT_MENU_MAP);
    m_btnAbi.SetToolTip(48224, GUIEVENT_MENU_ABILITIES);
    m_btnJou.SetToolTip(48218, GUIEVENT_MENU_JOURNAL);
    m_btnOpt.SetToolTip(48222, GUIEVENT_MENU_OPTIONS);
    m_btnMsg.SetToolTip(48223, GUIEVENT_MENU_MESSAGES);
}
// PC 0x00624F70  Xbox 0x0004D0D0
void CSWGuiInGameMessages::ShowFeedbackMessages()
{
    SWGUIINGAME_TRACE();
    CSWGuiListBox* pMessages = &m_lbMessages;
    pMessages->m_bEnabled = 1;
    m_lbDialog.m_bEnabled = 0;
#ifdef _XBOX
    m_lblXDesc.m_cText.m_cParams.SetStrRef(STRREF_SHOW_DIALOG);
#else
    m_btnSwitch.m_cText.m_cParams.SetStrRef(STRREF_SHOW_DIALOG);
#endif
    SetActiveControl(pMessages, 0);
    g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nMessageType =
        CGuiInGame::MESSAGE_TYPE_FEEDBACK;

#ifndef _XBOX
    CExoString sTitle;
    sTitle = g_pTlkTable->GetSimpleString(STRREF_MESSAGES) +
        " - " + g_pTlkTable->GetSimpleString(STRREF_FEEDBACK);
    m_lblTitle.m_cText.m_cParams.SetText(sTitle);
#endif
}

#ifdef _XBOX
void CSWGuiMessageBox::HandleInputEvent(int nEvent, int nState)  // PC 0x006250F0  Xbox 0x0004D120
{
    SWGUIINGAME_TRACE();
    if (nState != 0)
    {
        int bClose = 0;
        switch (nEvent)
        {
        case GUIEVENT_CANCEL:
        case GUIEVENT_CANCEL_ALTERNATE:
            if (m_bAllowCancel != 0)
                m_bAccepted = 0;
            else
                m_bAccepted = 1;
            bClose = 1;
            break;
        case GUIEVENT_ACTIVATE:
        case GUIEVENT_ACCEPT:
            m_bAccepted = 1;
            bClose = 1;
            break;
        case GUIEVENT_SCROLL_UP:
            m_lbMessage.HandleInputEvent(GUIEVENT_UP, 1);
            break;
        case GUIEVENT_SCROLL_DOWN:
            m_lbMessage.HandleInputEvent(GUIEVENT_DOWN, 1);
            break;
        }
        if (bClose)
        {
        m_bHasIcon = 0;
        m_bAllowCancel = 1;
        m_bShowButtons = 1;
        ResetButtonMessage();
        SetModal(0);
        m_pManager->PopModalPanel();
        m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
        if (m_pCallbackObject != 0 && m_pCallback != 0)
            (m_pCallbackObject->*m_pCallback)(m_pCallbackControl);
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
#else
void CSWGuiMessageBox::HandleInputEvent(int nEvent, int nState)  // PC 0x006250F0  Xbox 0x0004D120
{
    SWGUIINGAME_TRACE();
    if (nState != 0)
    {
        switch (nEvent)
        {
        case GUIEVENT_SCROLL_UP:
            m_lbMessage.HandleInputEvent(GUIEVENT_UP, 1);
            break;
        case GUIEVENT_SCROLL_DOWN:
            m_lbMessage.HandleInputEvent(GUIEVENT_DOWN, 1);
            break;
        case SWGUI_MESSAGEEVENT_CANCEL:
        case GUIEVENT_CANCEL:
        case GUIEVENT_CANCEL_ALTERNATE:
            if (m_bAllowCancel != 0)
                m_bAccepted = 0;
            else
                m_bAccepted = 1;

            goto Exit;
        case SWGUI_MESSAGEEVENT_OK:
            m_bAccepted = 1;
        Exit:
            m_bHasIcon = 0;
            m_bAllowCancel = 1;
            m_bShowButtons = 1;
            ResetButtonMessage();
            SetModal(0);
            m_pManager->PopModalPanel();
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            if (m_pCallbackObject != 0 && m_pCallback != 0)
                (m_pCallbackObject->*m_pCallback)(m_pCallbackControl);
            break;
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
#endif
// PC 0x00625200  Xbox 0x0004D240
void CSWGuiMessageBox::UpdateControls()
{
    SWGUIINGAME_TRACE();
#ifdef _XBOX
    CSWGuiLabel* pA = &m_lblA;
    pA->m_bEnabled = m_bShowButtons;
    CSWGuiLabel* pOk = &m_lblOk;
    pOk->m_bEnabled = m_bShowButtons;
    CSWGuiLabel* pB = &m_lblB;
    CSWGuiLabel* pCancel = &m_lblCancel;
    pB->m_bEnabled = m_bAllowCancel && m_bShowButtons;
    pCancel->m_bEnabled = m_bAllowCancel && m_bShowButtons;
#else
    CSWGuiButton* pOk = &m_btnOk;
    pOk->m_bEnabled = m_bShowButtons;
    CSWGuiButton* pCancel = &m_btnCancel;
    pCancel->m_bEnabled = m_bAllowCancel && m_bShowButtons;
#endif
    CSWGuiLabel* pIcon = &m_lblIcon;
    pIcon->m_bEnabled = m_bHasIcon;
}
// PC 0x00625260
void CSWGuiMessageBox::SetOkButtonMessage(unsigned long nStrRef)
{
    SWGUIINGAME_TRACE();
#ifdef _XBOX
    m_lblOk.m_cText.m_cParams.SetStrRef(nStrRef);
#else
    m_btnOk.m_cText.m_cParams.SetStrRef(nStrRef);
#endif
}
// PC 0x00625270  Xbox 0x0004D2E0
void CSWGuiMessageBox::SetCancelButtonMessage(unsigned long nStrRef)
{
    SWGUIINGAME_TRACE();
#ifdef _XBOX
    m_lblCancel.m_cText.m_cParams.SetStrRef(nStrRef);
#else
    m_btnCancel.m_cText.m_cParams.SetStrRef(nStrRef);
#endif
}
// PC 0x00625280  Xbox 0x0004D300
void CSWGuiMessageBox::HideButtons()
{
    SWGUIINGAME_TRACE();
    m_bHideButtons = 1;
#ifdef _XBOX
    m_lblOk.SetEnabledFlag(0);
    m_lblCancel.SetEnabledFlag(0);
    m_lblA.SetEnabledFlag(0);
    m_lblB.SetEnabledFlag(0);
#else
    m_btnOk.SetEnabledFlag(0);
    m_btnCancel.SetEnabledFlag(0);
#endif
}
// PC 0x006252A0  Xbox 0x0004D330
void CSWGuiMessageBox::ResetButtonMessage()
{
    SWGUIINGAME_TRACE();
#ifdef _XBOX
    m_lblOk.SetEnabledFlag(1);
    m_lblCancel.SetEnabledFlag(1);
    m_lblA.SetEnabledFlag(1);
    m_lblB.SetEnabledFlag(1);
    m_bHideButtons = 0;
    m_lblOk.m_cText.m_cParams.SetStrRef(1580);
    m_lblCancel.m_cText.m_cParams.SetStrRef(1581);
#else
    CSWGuiButton* pOk = &m_btnOk;
    pOk->m_bEnabled = 1;
    CSWGuiButton* pCancel = &m_btnCancel;
    pCancel->m_bEnabled = 1;
    m_bHideButtons = 0;
    m_btnOk.m_cText.m_cParams.SetStrRef(1580);
    m_btnCancel.m_cText.m_cParams.SetStrRef(1581);
#endif
}
// PC 0x006252F0  Xbox 0x0004D390
void CSWGuiMessageBox::AddMessageLabel()
{
    SWGUIINGAME_TRACE();
    CSWGuiListBox* pList = &m_lbMessage;
    CSWGuiExtent extent = m_lblMessage.m_cExtent;
    extent.m_nWidth = pList->m_cItemExtent.m_nWidth - 2 * pList->m_nPadding;
    m_lblMessage.SetExtent(extent);
    if (m_lblMessage.m_cText.m_cParams.m_sText.GetLength() > 0)
        extent.m_nHeight = m_lblMessage.m_cText.GetIdealHeight();
    else
        extent.m_nHeight = m_lblMessage.m_cText.GetFontHeight();
    m_lblMessage.SetExtent(extent);
    CSWGuiControl* pControl = &m_lblMessage;
    pList->AddControls(&pControl, 1, 0, 0, 0);
    pList->SetSelectedControl(&m_lblMessage, 0);
}
// PC 0x0041A290
inline int CSWGuiListBox::IsOversized()
{
    return m_nContentHeight + m_nPadding > m_cItemExtent.m_nHeight &&
           m_lItems.num > 0 && !m_bHideScrollIcon;
}
// PC 0x006253A0  Xbox 0x0004D440
void CSWGuiMessageBox::FixMessageLabel()
{
    CSWGuiExtent cPanel = m_cPanelExtent;
    CSWGuiExtent cMessage = m_cMessageExtent;
    CSWGuiExtent cControl;
#ifdef _XBOX
    CSWGuiExtent cPrompt;
#endif

    if (m_bHasIcon)
    {
        cPanel.m_nHeight += 32;
        cMessage.m_nTop += 32;
        cPanel.m_nTop -= 16;
    }
#ifdef _XBOX
    if (m_bShowButtons && !m_bHideButtons)
    {
        if (!m_bAllowCancel)
        {
            cPanel.m_nHeight -= m_nLblBBottomOffset - 15;
            cPanel.m_nTop += (m_nLblBBottomOffset - 15) / 2;
        }
    }
    else
    {
        cPanel.m_nHeight -= m_nLblABottomOffset - 15;
        cPanel.m_nTop += (m_nLblABottomOffset - 15) / 2;
    }

    if (m_lblOk.m_bEnabled)
    {
        cControl = m_lblOk.m_cExtent;
        cControl.m_nWidth = 20;
        m_lblOk.SetExtent(cControl);
        while (m_lblOk.m_cText.GetIdealHeight() != m_lblOk.m_cText.GetFontHeight())
        {
            cControl.m_nWidth += 10;
            m_lblOk.SetExtent(cControl);
        }
        if (cPanel.m_nWidth <= m_lblA.m_cExtent.m_nWidth + cControl.m_nWidth - 2)
            cPanel.m_nWidth = m_lblA.m_cExtent.m_nWidth + cControl.m_nWidth - 2;
    }
    if (m_lblCancel.m_bEnabled)
    {
        cControl = m_lblCancel.m_cExtent;
        cControl.m_nWidth = 20;
        m_lblCancel.SetExtent(cControl);
        while (m_lblCancel.m_cText.GetIdealHeight() != m_lblCancel.m_cText.GetFontHeight())
        {
            cControl.m_nWidth += 10;
            m_lblCancel.SetExtent(cControl);
        }
        if (cPanel.m_nWidth <= m_lblB.m_cExtent.m_nWidth + cControl.m_nWidth - 2)
            cPanel.m_nWidth = m_lblB.m_cExtent.m_nWidth + cControl.m_nWidth - 2;
    }
#else
    if (m_bShowButtons && !m_bHideButtons)
    {
        if (!m_bAllowCancel)
        {
            cPanel.m_nHeight -= m_btnCancel.m_cExtent.m_nHeight + 2;
            cPanel.m_nTop += (m_btnCancel.m_cExtent.m_nHeight + 2) / 2;
        }
    }
    else
    {
        cPanel.m_nHeight -= m_btnOk.m_cExtent.m_nHeight;
        cPanel.m_nTop += m_btnOk.m_cExtent.m_nHeight / 2;
    }

    if (m_btnOk.m_bEnabled)
    {
        cControl = m_btnOk.m_cExtent;
        cControl.m_nWidth = 100;
        m_btnOk.SetExtent(cControl);
        while (m_btnOk.m_cText.GetIdealHeight() != m_btnOk.m_cText.GetFontHeight())
        {
            cControl.m_nWidth += 10;
            m_btnOk.SetExtent(cControl);
        }
        cControl.m_nWidth += 2 * m_btnOk.m_cBorder.m_cParams.m_nInnerOffset;
        m_btnOk.SetExtent(cControl);
        if (cPanel.m_nWidth <= cControl.m_nWidth - 2)
            cPanel.m_nWidth = cControl.m_nWidth - 2;
    }
    if (m_btnCancel.m_bEnabled)
    {
        cControl = m_btnCancel.m_cExtent;
        cControl.m_nWidth = 100;
        m_btnCancel.SetExtent(cControl);
        while (m_btnCancel.m_cText.GetIdealHeight() != m_btnCancel.m_cText.GetFontHeight())
        {
            cControl.m_nWidth += 10;
            m_btnCancel.SetExtent(cControl);
        }
        cControl.m_nWidth += 2 * m_btnCancel.m_cBorder.m_cParams.m_nInnerOffset;
        m_btnCancel.SetExtent(cControl);
        if (cPanel.m_nWidth <= cControl.m_nWidth - 2)
            cPanel.m_nWidth = cControl.m_nWidth - 2;
    }
#endif

    cMessage.m_nWidth += cPanel.m_nWidth - m_cPanelExtent.m_nWidth;
    SetExtent(cPanel);
    m_lbMessage.SetExtent(cMessage);
    m_lbMessage.SetScrollBarExtent(16, 1);
    AddMessageLabel();
#ifndef _XBOX
    int nScrollWidth = m_lbMessage.m_cScrollBar.m_cExtent.m_nWidth;
    m_lbMessage.SetScrollBarExtent(0, 1);
#endif

    if (m_lbMessage.IsOversized())
    {
        int nFontHeight = m_lblMessage.m_cText.GetFontHeight();
        while (m_lbMessage.IsOversized() &&
               (cPanel.m_nWidth < 440 || cPanel.m_nHeight < 280))
        {
            if (cPanel.m_nHeight > 160 && cPanel.m_nWidth < 440)
            {
                cMessage.m_nWidth += 40;
                cPanel.m_nWidth += 40;
                cPanel.m_nLeft -= 20;
                m_lbMessage.SetExtent(cMessage);
                AddMessageLabel();
                if (!m_lbMessage.IsOversized())
                    break;
            }
            if (cPanel.m_nHeight < 280)
            {
                cMessage.m_nHeight += nFontHeight;
                cPanel.m_nHeight += nFontHeight;
                cPanel.m_nTop -= nFontHeight / 2;
                m_lbMessage.SetExtent(cMessage);
                AddMessageLabel();
            }
        }
        SetExtent(cPanel);
    }

#ifdef _XBOX
    if (m_lbMessage.IsOversized())
        m_lblRAnalog.SetEnabledFlag(1);
    else
    {
        m_lblRAnalog.SetEnabledFlag(0);
        m_lbMessage.SetScrollBarExtent(0, 1);
    }
    cControl = m_lblRAnalog.m_cExtent;
    CSWGuiExtent* pListExtent = &m_lbMessage.m_cExtent;
    cPrompt = *pListExtent;
    cControl.m_nTop = cPrompt.m_nHeight / 2 - cControl.m_nHeight / 2 + cPrompt.m_nTop;
    cControl.m_nLeft = m_cExtent.m_nWidth - m_nLblRAnalogRightOffset;
    m_lblRAnalog.SetExtent(cControl);
    int nCenter = m_cExtent.m_nWidth / 2;
    cControl = m_lblIcon.m_cExtent;
    cControl.m_nLeft = nCenter - cControl.m_nWidth / 2;
    m_lblIcon.SetExtent(cControl);
    cControl = m_lblOk.m_cExtent;
    CSWGuiExtent* pAExtent = &m_lblA.m_cExtent;
    cPrompt = *pAExtent;
    cPrompt.m_nTop = pListExtent->m_nHeight + pListExtent->m_nTop - 4;
    cPrompt.m_nLeft = nCenter - (cPrompt.m_nWidth + cControl.m_nWidth - 2) / 2;
    m_lblA.SetExtent(cPrompt);
    cControl.m_nTop = cPrompt.m_nTop - 1;
    cControl.m_nLeft = cPrompt.m_nLeft + 30;
    m_lblOk.SetExtent(cControl);
    cControl = m_lblCancel.m_cExtent;
    cPrompt = m_lblB.m_cExtent;
    cPrompt.m_nTop = pAExtent->m_nHeight + pAExtent->m_nTop - 14;
    cPrompt.m_nLeft = nCenter - (cPrompt.m_nWidth + cControl.m_nWidth - 2) / 2;
    m_lblB.SetExtent(cPrompt);
    cControl.m_nTop = cPrompt.m_nTop - 1;
    cControl.m_nLeft = cPrompt.m_nLeft + 30;
    m_lblCancel.SetExtent(cControl);
#else
    if (m_lbMessage.IsOversized())
    {
        m_lbMessage.SetScrollBarExtent(nScrollWidth, 1);
        AddMessageLabel();
    }

    int nCenter = m_cExtent.m_nWidth / 2;
    cControl = m_lblIcon.m_cExtent;
    cControl.m_nLeft = nCenter - cControl.m_nWidth / 2;
    m_lblIcon.SetExtent(cControl);
    CSWGuiExtent* pOkExtent = &m_btnOk.m_cExtent;
    cControl = *pOkExtent;
    cControl.m_nTop = m_lbMessage.m_cExtent.m_nHeight + m_lbMessage.m_cExtent.m_nTop + 4;
    cControl.m_nLeft = nCenter - cControl.m_nWidth / 2;
    m_btnOk.SetExtent(cControl);
    cControl = m_btnCancel.m_cExtent;
    cControl.m_nTop = pOkExtent->m_nHeight + pOkExtent->m_nTop + 2;
    cControl.m_nLeft = nCenter - cControl.m_nWidth / 2;
    m_btnCancel.SetExtent(cControl);
    CenterPanel();
#endif
}
// PC 0x006258F0
void CSWGuiMessageBox::OnPanelAdded()
{
    SWGUIINGAME_TRACE();
    int nClass = g_pAppManager->m_pClientExoApp->GetInputClass();
#ifndef _XBOX
    CSWGuiPanel::OnPanelAdded();
#endif
    m_nPreviousInputClass = -1;
    if (nClass != INPUT_CLASS_GUI)
    {
        m_nPreviousInputClass = nClass;
#ifdef _XBOX
        g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GUI);
#else
        g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GUI, 0);
#endif
    }
#ifndef _XBOX
    SetActiveControl(0, 0);
    SetActiveControl(&m_btnOk, 0);
    CSWGuiExtent cExtent = m_cExtent;
    CSWGuiExtent cButtonExtent;
    if (m_bAllowCancel)
    {
        if (m_bFocusOk)
            cButtonExtent = m_btnOk.m_cExtent;
        else
            cButtonExtent = m_btnCancel.m_cExtent;
    }
    else
        cButtonExtent = m_btnOk.m_cExtent;
#ifndef _XBOX
    g_pGuiMan->StoreCurrentMousePosition();
#endif
    int nLeft = cButtonExtent.m_nLeft;
#ifndef _XBOX
    g_pGuiMan->MoveMouseToPosition(
        cExtent.m_nLeft + nLeft + cButtonExtent.m_nWidth / 2,
        cExtent.m_nTop + cButtonExtent.m_nTop + cButtonExtent.m_nHeight / 2);
#endif
#endif
}
// PC 0x00625A00  Xbox 0x0004DA80
void CSWGuiMessageBox::OnPanelRemoved()
{
    SWGUIINGAME_TRACE();
#ifndef _XBOX
    CSWGuiPanel::OnPanelRemoved();
#endif
    if (m_nPreviousInputClass >= 0)
#ifdef _XBOX
        g_pAppManager->m_pClientExoApp->SetInputClass(m_nPreviousInputClass);
#else
        g_pAppManager->m_pClientExoApp->SetInputClass(m_nPreviousInputClass, 1);
#endif
#ifndef _XBOX
    g_pGuiMan->RestoreOldMousePosition();
#endif
#ifndef _XBOX
    m_bFocusOk = 0;
#endif
}
// PC 0x00625A40  Xbox 0x0004DAE0
void CSWGuiControllerLossBox::OnAccept(CSWGuiControl* pControl)
{
    SWGUIINGAME_TRACE();
    g_pAppManager->m_pClientExoApp->GetInGameGui()->EndControllerPauseState();
}
// PC 0x00625A60  Xbox 0x0004DB00
void CSWGuiMessageBox::OnDeath(CSWGuiControl* pControl)
{
    SWGUIINGAME_TRACE();
    g_pAppManager->m_pClientExoApp->SetEndGamePending(1, 0.0f, 1);
}

#ifndef _XBOX
void CSWGuiMessageBox::OnOkButtonPressed(CSWGuiControl* pControl)  // PC 0x00625A80
{
    SWGUIINGAME_TRACE();
    HandleInputEvent(502, m_btnOk.m_bSelected);
}
// PC 0x00625AA0
void CSWGuiMessageBox::OnCancelButtonPressed(CSWGuiControl* pControl)
{
    SWGUIINGAME_TRACE();
    HandleInputEvent(503, m_btnCancel.m_bSelected);
}
#endif
// PC 0x00625AC0  Xbox 0x0004DB20
void CSWGuiStatusSummary::HandleInputEvent(int nEvent, int nState)
{
    SWGUIINGAME_TRACE();
    if (nState != 0)
    {
        switch (nEvent)
        {
        case GUIEVENT_ACTIVATE:
        case GUIEVENT_CANCEL:
        case GUIEVENT_ACCEPT:
        case GUIEVENT_CANCEL_ALTERNATE:
            SetModal(0);
#ifdef _XBOX
            g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME);
#else
            g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME, 1);
#endif
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            if (!m_bWasPaused)
                g_pAppManager->m_pClientExoApp->SetPausedByCombat(0, 0, 0);
            break;
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
// PC 0x00625B50  Xbox 0x0004EEC0
void CSWGuiStatusSummary::DisplayStatus()
{
    SWGUIINGAME_TRACE();
    PlayPendingSounds();

    for (int nStatus = 0; nStatus < GUI_STATUS_COUNT; nStatus++)
    {
        int bShow = 0;
        switch (nStatus)
        {
        case GUI_STATUS_JOURNAL:
            bShow = m_cStatusSummary.m_bJournal;
            break;
        case GUI_STATUS_CREDITS:
            bShow = m_cStatusSummary.m_nCredits != 0;
            break;
        case GUI_STATUS_XP:
            bShow = m_cStatusSummary.m_nExperience != 0;
            break;
        case GUI_STATUS_STEALTH_XP:
            bShow = m_cStatusSummary.m_nStealthExperience != 0;
            break;
        case GUI_STATUS_DARK_SHIFT:
            bShow = m_cStatusSummary.m_nDarkShift > m_cStatusSummary.m_nLightShift;
            break;
        case GUI_STATUS_LIGHT_SHIFT:
            bShow = m_cStatusSummary.m_nLightShift > m_cStatusSummary.m_nDarkShift;
            break;
        case GUI_STATUS_ITEM_RECEIVED:
            bShow = m_cStatusSummary.m_bItemReceived;
            break;
        case GUI_STATUS_ITEM_LOST:
            bShow = m_cStatusSummary.m_bItemLost;
            break;
        }
        if (bShow)
            g_pAppManager->m_pClientExoApp->GetInGameGui()->ShowFlashingStatus(nStatus);
    }

    m_cStatusSummary.m_bDisplaySPending = 0;
    m_cStatusSummary.m_bSoundPending = 0;
    m_cStatusSummary.m_bItemReceived = 0;
    m_cStatusSummary.m_bItemLost = 0;
    m_cStatusSummary.m_bJournal = 0;
    m_cStatusSummary.m_bLevelUpSound = 0;
    m_cStatusSummary.m_bNewQuestSound = 0;
    m_cStatusSummary.m_bCompleteSound = 0;
    m_cStatusSummary.m_bCreditsNet = 0;
    m_cStatusSummary.m_nCredits = 0;
    m_cStatusSummary.m_nLightShift = 0;
    m_cStatusSummary.m_nDarkShift = 0;
    m_cStatusSummary.m_nExperience = 0;
    m_cStatusSummary.m_nStealthExperience = 0;
}
// PC 0x00625C60  Xbox 0x0004EFD0
void CSWGuiStatusSummary::OnPanelAdded()
{
    SWGUIINGAME_TRACE();

    int nTop = 10;
    int nWidth = 150;
    int bJournal = 0;
    CExoString sDescription;
    CSWGuiExtent cExtent;

    PlayPendingSounds();

    for (int nStatus = 0; nStatus < GUI_STATUS_COUNT; nStatus++)
    {
        int bShow = 0;
        int bFlash = 1;
        int bSetDescription = 0;
        switch (nStatus)
        {
        case GUI_STATUS_JOURNAL:
            bShow = m_cStatusSummary.m_bJournal;
            bJournal = bShow;
            break;
        case GUI_STATUS_CREDITS:
            bShow = m_cStatusSummary.m_nCredits != 0 || m_cStatusSummary.m_bCreditsNet;
            if (bShow)
            {
                bSetDescription = 1;
                if (m_cStatusSummary.m_nCredits >= 0)
                {
                    g_pTlkTable->SetCustomToken(0, m_cStatusSummary.m_nCredits);
                    sDescription = g_pAppManager->GetClientExoApp()->GetGUIString(STRREF_CREDITS_RECEIVED);
                }
                else
                {
                    g_pTlkTable->SetCustomToken(0, -m_cStatusSummary.m_nCredits);
                    sDescription = g_pAppManager->GetClientExoApp()->GetGUIString(STRREF_CREDITS_LOST);
                }
                if (m_cStatusSummary.m_bCreditsNet)
                    sDescription = g_pAppManager->GetClientExoApp()->GetGUIString(STRREF_NET_CHANGE) + sDescription;
            }
            break;
        case GUI_STATUS_XP:
            bShow = m_cStatusSummary.m_nExperience != 0;
            if (bShow)
            {
                bSetDescription = 1;
                g_pTlkTable->SetCustomToken(0, m_cStatusSummary.m_nExperience);
                sDescription = g_pAppManager->GetClientExoApp()->GetGUIString(STRREF_XP_RECEIVED);
            }
            break;
        case GUI_STATUS_STEALTH_XP:
            bShow = m_cStatusSummary.m_nStealthExperience != 0;
            if (bShow)
            {
                bSetDescription = 1;
                g_pTlkTable->SetCustomToken(0, m_cStatusSummary.m_nStealthExperience);
                sDescription = g_pAppManager->GetClientExoApp()->GetGUIString(STRREF_STEALTH_XP_RECEIVED);
            }
            break;
        case GUI_STATUS_DARK_SHIFT:
            bShow = m_cStatusSummary.m_nDarkShift > 0;
            bFlash = m_cStatusSummary.m_nDarkShift > m_cStatusSummary.m_nLightShift;
            break;
        case GUI_STATUS_LIGHT_SHIFT:
            bShow = m_cStatusSummary.m_nLightShift > 0;
            bFlash = m_cStatusSummary.m_nLightShift > m_cStatusSummary.m_nDarkShift;
            break;
        case GUI_STATUS_NET_SHIFT:
            bShow = m_cStatusSummary.m_nLightShift > 0 &&
                    m_cStatusSummary.m_nDarkShift > 0 &&
                    m_cStatusSummary.m_nLightShift != m_cStatusSummary.m_nDarkShift;
            bFlash = 0;
            if (bShow)
            {
                bSetDescription = 1;
                if (m_cStatusSummary.m_nLightShift > m_cStatusSummary.m_nDarkShift)
                {
                    sDescription = g_pAppManager->GetClientExoApp()->GetGUIString(STRREF_NET_LIGHT_SHIFT);
                    m_aStatusLabels[GUI_STATUS_NET_SHIFT].m_cBorder.m_cParams.SetFillImage(
                        m_aStatusLabels[GUI_STATUS_LIGHT_SHIFT].m_cBorder.m_cParams.m_cFillImage, 0);
                }
                else
                {
                    sDescription = g_pAppManager->GetClientExoApp()->GetGUIString(STRREF_NET_DARK_SHIFT);
                    m_aStatusLabels[GUI_STATUS_NET_SHIFT].m_cBorder.m_cParams.SetFillImage(
                        m_aStatusLabels[GUI_STATUS_DARK_SHIFT].m_cBorder.m_cParams.m_cFillImage, 0);
                }
            }
            break;
        case GUI_STATUS_ITEM_RECEIVED:
            bShow = m_cStatusSummary.m_bItemReceived;
            break;
        case GUI_STATUS_ITEM_LOST:
            bShow = m_cStatusSummary.m_bItemLost;
            break;
        }

        if (bShow)
        {
            CSWGuiLabel* pLabel = &m_aStatusLabels[nStatus];
            CSWGuiLabel* pDescription = &m_aStatusDescriptions[nStatus];
            pLabel->m_bEnabled = 1;
            pDescription->m_bEnabled = 1;
            if (bSetDescription)
            {
                m_aStatusDescriptions[nStatus].m_cText.m_cParams.SetText(sDescription);
#ifndef _XBOX
                m_aStatusDescriptions[nStatus].m_cText.m_cParams.ReSetFont();
#endif
            }
            cExtent = m_aStatusLabels[nStatus].m_cExtent;
            cExtent.m_nTop = nTop;
            pLabel->SetExtent(cExtent);
            cExtent = m_aStatusDescriptions[nStatus].m_cExtent;
            cExtent.m_nTop = nTop - 1;
            cExtent.m_nWidth = nWidth;
            pDescription->SetExtent(cExtent);
            if (m_aStatusDescriptions[nStatus].m_cText.m_cParams.m_sText != "" && nWidth < 440)
            {
                do
                {
                    if (m_aStatusDescriptions[nStatus].m_cText.GetIdealHeight() ==
                        m_aStatusDescriptions[nStatus].m_cText.GetFontHeight())
                        break;
                    nWidth += 20;
                    cExtent.m_nWidth = nWidth;
                    pDescription->SetExtent(cExtent);
                } while (nWidth < 440);
            }
            if (bFlash)
                g_pAppManager->GetClientExoApp()->GetInGameGui()->ShowFlashingStatus(nStatus);
            nTop += 37;
        }
        else
        {
            CSWGuiLabel* pLabel = &m_aStatusLabels[nStatus];
            pLabel->m_bEnabled = 0;
            m_aStatusDescriptions[nStatus].m_bEnabled = 0;
        }
    }
    nTop -= 7;
    cExtent.m_nHeight = nTop + 32;
    cExtent.m_nWidth = m_aStatusDescriptions[0].m_cExtent.m_nLeft + nWidth + 10;
    cExtent.m_nTop = (m_pManager->m_nHeight - cExtent.m_nHeight) / 2;
    cExtent.m_nLeft = (m_pManager->m_nWidth - cExtent.m_nWidth) / 2;
    SetExtent(cExtent);
    int nPanelWidth = cExtent.m_nWidth;
#ifdef _XBOX
    CSWGuiExtent cOkExtent = m_lblOk.m_cExtent;
    cExtent = m_lblA.m_cExtent;
    cExtent.m_nLeft = (nPanelWidth - cOkExtent.m_nWidth - cExtent.m_nWidth) / 2;
    cOkExtent.m_nLeft = cExtent.m_nWidth + cExtent.m_nLeft;
    cExtent.m_nTop = nTop;
    cOkExtent.m_nTop = nTop - 1;
    m_lblA.SetExtent(cExtent);
    m_lblOk.SetExtent(cOkExtent);
#else
    CSWGuiExtent* pOkExtent = &m_btnOk.m_cExtent;
    cExtent = *pOkExtent;
    cExtent.m_nTop = nTop;
    cExtent.m_nLeft = (nPanelWidth - cExtent.m_nWidth) / 2;
    m_btnOk.SetExtent(cExtent);
#endif
    m_cStatusSummary.m_bDisplaySPending = 0;
    m_cStatusSummary.m_bSoundPending = 0;
    m_cStatusSummary.m_bItemReceived = 0;
    m_cStatusSummary.m_bItemLost = 0;
    m_cStatusSummary.m_bJournal = 0;
    m_cStatusSummary.m_bLevelUpSound = 0;
    m_cStatusSummary.m_bNewQuestSound = 0;
    m_cStatusSummary.m_bCompleteSound = 0;
    m_cStatusSummary.m_bCreditsNet = 0;
    m_cStatusSummary.m_nCredits = 0;
    m_cStatusSummary.m_nLightShift = 0;
    m_cStatusSummary.m_nDarkShift = 0;
    m_cStatusSummary.m_nExperience = 0;
    m_cStatusSummary.m_nStealthExperience = 0;
#ifndef _XBOX
    CSWGuiPanel::OnPanelAdded();
#endif
    if (bJournal)
        g_pAppManager->GetClientExoApp()->ShowTutorialWindow(CClientExoApp::TUTORIAL_JOURNAL,
                                                          OBJECT_INVALID, OBJECT_INVALID, 0);
#ifndef _XBOX
    CSWGuiExtent cPanelExtent = m_cExtent;
    CSWGuiExtent cButtonExtent = *pOkExtent;
    int nMouseX = cButtonExtent.m_nLeft;
    int nMouseY = cPanelExtent.m_nTop;
    nMouseX += cPanelExtent.m_nLeft + cButtonExtent.m_nWidth / 2;
    nMouseY += cButtonExtent.m_nTop + cButtonExtent.m_nHeight / 2;
#endif
#ifndef _XBOX
    g_pGuiMan->StoreCurrentMousePosition();
    g_pGuiMan->MoveMouseToPosition(nMouseX, nMouseY);
#endif
}
// PC 0x00626400  Xbox 0x0004F7F0
CSWGuiInGameMessages::CSWGuiInGameMessages(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    CSWGuiExtent cExtent;
    cExtent.m_nLeft = cExtent.m_nTop =
        cExtent.m_nWidth = cExtent.m_nHeight = 0;

    StartLoadFromLayout("messages");
    CSWGuiListBox* pMessages = &m_lbMessages;
    InitControl(*pMessages, "LB_MESSAGES", 1);
    InitControl(m_lblTitle, "LBL_MESSAGES", 1);
#ifdef _XBOX
    InitControl(m_lblStart, "LBL_START", 1);
    InitControl(m_lblExit, "LBL_EXIT", 1);
    InitControl(m_lblX, "LBL_X", 1);
    InitControl(m_lblXDesc, "LBL_X_DESC", 1);
    InitControl(m_lbDialog, "LB_DIALOG", 1);
#else
    InitControl(m_lbDialog, "LB_DIALOG", 1);
    CSWGuiButton* pShow = &m_btnSwitch;
    InitControl(*pShow, "BTN_SHOW", 1);
    InitControl(m_btnBack, "BTN_EXIT", 1);
#endif
    StopLoadFromLayout();

    CSWGuiLabelHilight* pProto =
        (CSWGuiLabelHilight*)m_lbMessages.m_pProtoItem;
    pProto->m_cHilite.m_cParams.m_vColor = CGuiInGame::COLOR_YELLOW;
    pProto->m_cHilite.m_cParams.SetPulsingAlpha(1, 1, 0);
    m_pMessageLabels = new CSWGuiLabelHilight[64];
    for (unsigned int i = 0; i < 64; i++)
    {
        m_pMessageLabels[i].Initialize(cExtent, pProto);
    }

    pProto = (CSWGuiLabelHilight*)m_lbDialog.m_pProtoItem;
    pProto->m_cHilite.m_cParams.m_vColor = CGuiInGame::COLOR_YELLOW;
    pProto->m_cHilite.m_cParams.SetPulsingAlpha(1, 1, 0);
    m_pDialogLabels = new CSWGuiLabelHilight[64];
    for (unsigned int j = 0; j < 64; j++)
    {
        m_pDialogLabels[j].Initialize(cExtent, pProto);
    }

    pMessages->m_bClipItems = 1;
#ifndef _XBOX
    pShow->AddEvent(CSWGuiControl::EVENT_SELECT, this,
                    (PFNGUIEVENT)&CSWGuiPanel::OnXButtonPressed);
    pShow->m_bFocusable = 0;
    CSWGuiButton* pExit = &m_btnBack;
    pExit->AddEvent(CSWGuiControl::EVENT_SELECT, this,
                    (PFNGUIEVENT)&CSWGuiPanel::OnBButtonPressed);
    pExit->m_bFocusable = 0;
    pMessages->SetSelectOnLeftClick(1);
    m_lbDialog.SetSelectOnLeftClick(1);
#endif
    SetActiveControl(pMessages, 0);
}
// PC 0x00626820  Xbox 0x0004DD00
CSWGuiInGameMessages::~CSWGuiInGameMessages()
{
    m_lbDialog.ClearItems();
    m_lbMessages.ClearItems();
    delete[] m_pDialogLabels;
    delete[] m_pMessageLabels;
}
// PC 0x00626920  Xbox 0x0004DE20
void CSWGuiInGameMessages::AddMessages(CGuiInGameMessage* pMessages, unsigned long nMessages)
{
    CExoArrayList<CSWGuiControl*> aControls;
    CSWGuiListBox* pList = &m_lbMessages;
    int nWidth = pList->m_cItemExtent.m_nWidth - 2 * pList->m_nPadding;
    CSWGuiExtent cExtent;
    cExtent.m_nLeft = 0;
    cExtent.m_nTop = 0;
    cExtent.m_nWidth = nWidth;
    cExtent.m_nHeight = INITIAL_MESSAGE_HEIGHT;
    aControls.SetSize(nMessages);
    for (unsigned long i = 0; i < nMessages; i++)
    {
        m_pMessageLabels[i].m_cText.m_cParams.SetText(pMessages[i].m_sMessage);
#ifndef _XBOX
        m_pMessageLabels[i].m_cText.m_cParams.ReSetFont();
#endif
        Vector vColor;
        switch (pMessages[i].m_nColor)
        {
        case CGuiInGameMessage::COLOR_RED:
            vColor = CGuiInGame::COLOR_RED;
            break;
        default:
            vColor = CGuiInGame::COLOR_BLUE;
            break;
        }
        m_pMessageLabels[i].m_cText.m_cParams.SetColor(vColor);
        m_pMessageLabels[i].SetExtent(cExtent);
        CSWGuiText* pText = &m_pMessageLabels[i].m_cText;
        cExtent.m_nHeight = pText->GetIdealHeight() +
            2 * m_pMessageLabels[i].m_cHilite.m_cParams.GetBorderDim();
        m_pMessageLabels[i].SetExtent(cExtent);
        m_pMessageLabels[i].SetActive(0);
        aControls[i] = &m_pMessageLabels[i];
    }
    if (nMessages)
    {
        pList->AddControls(aControls, 0, 0, 1);
        pList->SetSelectedControl(nMessages - 1, 0);
    }
}
// PC 0x00626B10  Xbox 0x0004E000
void CSWGuiInGameMessages::AddDialogMessages(CGuiInGameDialogMessage* pMessages,
                                            unsigned long nMessages)
{
    CExoArrayList<CSWGuiControl*> lControls;
    CSWGuiListBox* pList = &m_lbDialog;
    CSWGuiExtent cExtent(0, 0,
        pList->m_cItemExtent.m_nWidth - 2 * pList->m_nPadding, 10);
    CExoString sSpeaker;
    lControls.SetSize(nMessages);
    for (unsigned long i = 0; i < nMessages; i++)
    {
        sSpeaker = pMessages[i].m_sSpeaker;
        if (sSpeaker.GetLength() > 0)
            m_pDialogLabels[i].m_cText.m_cParams.SetText(
                sSpeaker + ": " + pMessages[i].m_sMessage);
        else
            m_pDialogLabels[i].m_cText.m_cParams.SetText(pMessages[i].m_sMessage);
#ifndef _XBOX
        m_pDialogLabels[i].m_cText.m_cParams.ReSetFont();
#endif
        m_pDialogLabels[i].SetExtent(cExtent);
        CSWGuiText* pText = &m_pDialogLabels[i].m_cText;
        cExtent.m_nHeight = pText->GetIdealHeight() +
                           2 * m_pDialogLabels[i].m_cHilite.m_cParams.GetBorderDim();
        m_pDialogLabels[i].SetExtent(cExtent);
        m_pDialogLabels[i].SetActive(0);
        lControls.element[i] = &m_pDialogLabels[i];
    }
    if (nMessages != 0)
    {
        pList->AddControls(lControls, 0, 0, 1);
        pList->SetSelectedControl(nMessages - 1, 0);
    }
}
// PC 0x00626D90  Xbox 0x0004E270
void CSWGuiInGameMessages::OnPanelAdded()
{
#ifndef _XBOX
    CSWGuiPanel::OnPanelAdded();
#endif
    CGuiInGame* pInGame = g_pAppManager->m_pClientExoApp->GetInGameGui();
    pInGame->UpdateMessageGUI();
    if (g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nMessageType == CGuiInGame::MESSAGE_TYPE_DIALOG)
        ShowDialogMessages();
    else
        ShowFeedbackMessages();
    g_pAppManager->m_pClientExoApp->ShowTutorialWindow(0xC, OBJECT_INVALID,
                                                       OBJECT_INVALID, 0);
}
// PC 0x00626DF0  Xbox 0x0004E2D0
CSWGuiMessageBox::CSWGuiMessageBox(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    m_bAllowCancel = 1;
    m_bShowButtons = 1;
    m_bHideButtons = 0;
    m_bHasIcon = 0;
    StartLoadFromLayout("confirm");
#ifdef _XBOX
    InitControl(m_lblA, "LBL_A", 1);
    InitControl(m_lblOk, "LBL_OK", 1);
    InitControl(m_lblB, "LBL_B", 1);
    InitControl(m_lblCancel, "LBL_CANCEL", 1);
    InitControl(m_lbMessage, "LB_MESSAGE", 1);
    InitControl(m_lblRAnalog, "LBL_RANALOG", 1);
    StopLoadFromLayout();
#else
    CSWGuiButton* pOk = &m_btnOk;
    InitControl(*pOk, "BTN_OK", 1);
    InitControl(m_btnCancel, "BTN_CANCEL", 1);
    InitControl(m_lbMessage, "LB_MESSAGE", 1);
    StopLoadFromLayout();
    pOk->AddEvent(CSWGuiControl::EVENT_SELECT, this,
                  (CSWGuiHandler)&CSWGuiMessageBox::OnOkButtonPressed);
    m_btnCancel.AddEvent(CSWGuiControl::EVENT_SELECT, this,
                         (CSWGuiHandler)&CSWGuiMessageBox::OnCancelButtonPressed);
#endif
    m_lbMessage.m_nSelectSound = CSWGuiManager::GUISOUND_SELECTION;
    m_lblIcon.SetExtent(CSWGuiExtent(0, 10, 32, 32));
    CSWGuiLabel* pIcon = &m_lblIcon;
    pIcon->m_bEnabled = 0;
    AddControl(pIcon);
    CSWGuiLabel* pProto = (CSWGuiLabel*)m_lbMessage.m_pProtoItem;
    if (pProto)
        m_lblMessage.Initialize(pProto->m_cExtent, pProto, 1.0f);
    CenterPanel();
    m_cPanelExtent = m_cExtent;
    m_cMessageExtent = m_lbMessage.m_cExtent;
#ifdef _XBOX
    m_nLblABottomOffset = m_cExtent.m_nHeight - m_lblA.m_cExtent.m_nTop;
    m_nLblBBottomOffset = m_cExtent.m_nHeight - m_lblB.m_cExtent.m_nTop;
    m_nLblRAnalogRightOffset = m_cExtent.m_nWidth - m_lblRAnalog.m_cExtent.m_nLeft;
#endif
    m_bAccepted = 0;
    m_pCallback = 0;
    m_pCallbackObject = 0;
    m_pCallbackControl = 0;
    m_nPreviousInputClass = 0;
#ifndef _XBOX
    m_bFocusOk = 0;
#endif
    m_nOpenSound = 15;
}
// PC 0x00627130  Xbox 0x0004E700
void CSWGuiMessageBox::SetAllowCancel(int bAllowCancel)
{
    m_bAllowCancel = bAllowCancel;
    m_nOpenSound = bAllowCancel != 0 ? 15 : 2;
    UpdateControls();
}
// PC 0x00627160  Xbox 0x0004E730
void CSWGuiMessageBox::SetIcon(const CResRef& cIcon)
{
    if (cIcon.IsValid())
    {
        m_bHasIcon = 1;
        m_lblIcon.m_cBorder.m_cParams.SetFillImage(cIcon, 0);
        UpdateControls();
    }
}
// PC 0x006271A0  Xbox 0x0004E770
void CSWGuiMessageBox::SetMessage(CExoString sMessage)
{
    g_pTlkTable->ParseStr(sMessage);
    m_lblMessage.m_cText.m_cParams.SetText(sMessage);
#ifndef _XBOX
    m_lblMessage.m_cText.m_cParams.ReSetFont();
#endif
    FixMessageLabel();
}
// PC 0x00627220  Xbox 0x0004E920
void CSWGuiControllerLossBox::OnPanelAdded()
{
    SetAllowCancel(0);
    SetMessage(48398);
    m_pCallbackObject = this;
    m_pCallback = (CSWGuiHandler)&CSWGuiControllerLossBox::OnAccept;
    m_pCallbackControl = 0;
    CSWGuiMessageBox::OnPanelAdded();
}
// PC 0x00627260  Xbox 0x0004E960
void CSWGuiMessageBox::DisplayDeathMessage()
{
    SetAllowCancel(0);
    SetMessage(42351);
    m_pCallbackObject = this;
    m_pCallback = (CSWGuiHandler)&CSWGuiMessageBox::OnDeath;
    m_pCallbackControl = 0;
    m_pManager->AddPanel(this, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
}
// PC 0x00628260  Xbox 0x0004FC40
void CSWGuiInGameMessages::HandleInputEvent(int nEvent, int nState)
{
    if (nState != 0)
    {
        switch (nEvent)
        {
        case GUIEVENT_TOGGLE_MODE:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
#ifdef _XBOX
            if (m_lbMessages.m_bEnabled)
                ShowDialogMessages();
            else
                ShowFeedbackMessages();
#else
            ToggleMessages();
#endif
            break;
        case GUIEVENT_CANCEL:
        case GUIEVENT_ACCEPT:
        case GUIEVENT_CANCEL_ALTERNATE:
#ifndef _XBOX
        case GUIEVENT_EXIT:
#endif
#ifdef _XBOX
            if (g_pAppManager->m_pClientExoApp->GetInGameGui()->HideSWInGameGui())
#else
            if (g_pAppManager->m_pClientExoApp->GetInGameGui()->HideSWInGameGui(0))
#endif
#ifdef _XBOX
                g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME);
#else
                g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GAME, 1);
#endif
            break;
        }

        static int nModeIdx = 0;
        int nModeSequence[] = { GUIEVENT_TOGGLE_MODE, GUIEVENT_LEFT,
                                GUIEVENT_ACTIVATE, GUIEVENT_TOGGLE_MODE };
        if (nEvent != nModeSequence[nModeIdx])
            nModeIdx = 0;
        if (nEvent == nModeSequence[nModeIdx])
        {
            if (++nModeIdx == 4)
            {
                char szMessage[] = "Pvpfh!kw,!Ekexkh!";
                nModeIdx = 0;
                for (unsigned int i = 0; i < sizeof(szMessage) - 1; ++i)
                    szMessage[i] -= i & 3;
                {
                    CExoString sMessage(szMessage);
                    CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
                    pClient->GetInGameGui()->AppendToMsgBuffer(sMessage, CGuiInGame::MESSAGE_MASK_HS_MODE, 0);
                }
                g_nHSMode = 1;
                g_pAppManager->m_pClientExoApp->GetInGameGui()->UpdateMessageGUI();
            }
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
// PC 0x006272A0  Xbox 0x0004E9A0
CSWGuiStatusSummary::CSWGuiStatusSummary(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    StartLoadFromLayout("statussummary");
    InitControl(m_aStatusLabels[GUI_STATUS_JOURNAL], "LBL_JOURNAL", 1);
    InitControl(m_aStatusDescriptions[GUI_STATUS_JOURNAL], "LBL_JOURNAL_DESC", 1);
    InitControl(m_aStatusLabels[GUI_STATUS_CREDITS], "LBL_CREDITS", 1);
    InitControl(m_aStatusDescriptions[GUI_STATUS_CREDITS], "LBL_CREDITS_DESC", 1);
    InitControl(m_aStatusLabels[GUI_STATUS_XP], "LBL_XP", 1);
    InitControl(m_aStatusDescriptions[GUI_STATUS_XP], "LBL_XP_DESC", 1);
    InitControl(m_aStatusLabels[GUI_STATUS_STEALTH_XP], "LBL_STEALTH", 1);
    InitControl(m_aStatusDescriptions[GUI_STATUS_STEALTH_XP], "LBL_STEALTH_DESC", 1);
    InitControl(m_aStatusLabels[GUI_STATUS_DARK_SHIFT], "LBL_DARKSIDE", 1);
    InitControl(m_aStatusDescriptions[GUI_STATUS_DARK_SHIFT], "LBL_DARKSIDE_DESC", 1);
    InitControl(m_aStatusLabels[GUI_STATUS_LIGHT_SHIFT], "LBL_LIGHTSIDE", 1);
    InitControl(m_aStatusDescriptions[GUI_STATUS_LIGHT_SHIFT], "LBL_LIGHTSIDE_DESC", 1);
    InitControl(m_aStatusLabels[GUI_STATUS_NET_SHIFT], "LBL_NETSHIFT", 1);
    InitControl(m_aStatusDescriptions[GUI_STATUS_NET_SHIFT], "LBL_NETSHIFT_DESC", 1);
    InitControl(m_aStatusLabels[GUI_STATUS_ITEM_RECEIVED], "LBL_RECEIVED", 1);
    InitControl(m_aStatusDescriptions[GUI_STATUS_ITEM_RECEIVED], "LBL_RECEIVED_DESC", 1);
    InitControl(m_aStatusLabels[GUI_STATUS_ITEM_LOST], "LBL_LOST", 1);
    InitControl(m_aStatusDescriptions[GUI_STATUS_ITEM_LOST], "LBL_LOST_DESC", 1);
#ifdef _XBOX
    InitControl(m_lblA, "LBL_A", 1);
    InitControl(m_lblOk, "LBL_OK", 1);
    StopLoadFromLayout();
#else
    CSWGuiButton* pOk = &m_btnOk;
    InitControl(*pOk, "BTN_OK", 1);
    StopLoadFromLayout();
    pOk->m_bFocusable = 0;
    pOk->AddEvent(CSWGuiControl::EVENT_SELECT, this,
                  (CSWGuiHandler)&CSWGuiPanel::OnAButtonPressed);
#endif
    m_cStatusSummary.m_bDisplaySPending = 0;
    m_cStatusSummary.m_bSoundPending = 0;
    m_cStatusSummary.m_bItemReceived = 0;
    m_cStatusSummary.m_bItemLost = 0;
    m_cStatusSummary.m_bJournal = 0;
    m_cStatusSummary.m_bLevelUpSound = 0;
    m_cStatusSummary.m_bNewQuestSound = 0;
    m_cStatusSummary.m_bCompleteSound = 0;
    m_cStatusSummary.m_bCreditsNet = 0;
    m_cStatusSummary.m_nCredits = 0;
    m_cStatusSummary.m_nLightShift = 0;
    m_cStatusSummary.m_nDarkShift = 0;
    m_cStatusSummary.m_nExperience = 0;
    m_cStatusSummary.m_nStealthExperience = 0;
    m_cStatusSummary.m_nSuppressed = 0;
#ifndef _XBOX
    m_bPositioned = 1;
#endif
}

void CSWGuiInGameMessages::ToggleMessages()
{
    if (m_lbMessages.m_bEnabled)
        ShowDialogMessages();
    else
        ShowFeedbackMessages();
}

static inline void SwguiingameTrace2()
{
    SwguiingameTrace1();
}

static inline void SwguiingameTrace1()
{
}
