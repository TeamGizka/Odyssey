#include "swguiingame.h"
#include "exotlktable.h"

template void CExoArrayList<CSWGuiControl*>::Allocate(int);

static inline void SwguiingameTrace2();
static inline void SwguiingameTrace1();
#define SWGUIINGAME_TRACE() SwguiingameTrace1()

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
#include "swguisaveload.h"
#include "nwscript.h"
#include "nwscreaturestats.h"
#include "nwsvartable.h"
#include "swparty.h"
#include "nwtlktable.h"
#include "nwseffect.h"
#include "nwseffectlist.h"
#include "swguiingame.inl"

void CSWGuiMessageBox::OnDeath(CSWGuiControl* pControl)
{
    SWGUIINGAME_TRACE();
    g_pAppManager->m_pClientExoApp->SetEndGamePending(1, 0.0f, 1);
}
#ifndef _XBOX
static inline void SetBorderFillImage(CSWGuiBorderParams& cParams, const CResRef& cImage, int bTile)
{
    cParams.SetFillImage(cImage, bTile);
}
// PC 0x00677700 GOG 0x006774A0
CSWGuiMessageBox::CSWGuiMessageBox(CSWGuiManager* pManager, int nStyle)
    : CSWGuiPanel(pManager)
{
    if (nStyle == 0)
    {
        CSWGuiMessageBox::CSWGuiMessageBox(pManager);
        return;
    }
    m_bAllowCancel = 1;
    m_bShowButtons = 1;
    m_bHideButtons = 0;
    m_bHasIcon = 0;
    m_n9BC = nStyle;
    StartLoadFromLayout("datapad_p");
    SetBorderFillImage(m_pBorder->m_cParams, "", 0);
    InitControl(m_btnOk, "BTN_OK", 1);
    InitControl(m_btnCancel, "BTN_CANCEL", 1);
    InitControl(m_lbMessage, "LB_MESSAGE", 1);
    StopLoadFromLayout();
    m_lbMessage.m_nSelectSound = CSWGuiManager::GUISOUND_SELECTION;
    m_btnOk.AddEvent(CSWGuiControl::EVENT_SELECT, this,
                     (CSWGuiHandler)&CSWGuiMessageBox::OnOkButtonPressed);
    m_btnCancel.AddEvent(CSWGuiControl::EVENT_SELECT, this,
                         (CSWGuiHandler)&CSWGuiMessageBox::OnCancelButtonPressed);
    CSWGuiLabel* pIcon = &m_lblIcon;
    pIcon->SetExtent(CSWGuiExtent(0, 10, 32, 32));
    pIcon->m_bEnabled = 0;
    AddControl(pIcon);
    CSWGuiLabel* pProto = (CSWGuiLabel*)m_lbMessage.m_pProtoItem;
    if (pProto)
        m_lblMessage.Initialize(pProto->m_cExtent, pProto, 1.0f);
    CenterPanel();
    m_cPanelExtent = m_cExtent;
    m_cMessageExtent = m_lbMessage.m_cExtent;
    m_bAccepted = 0;
    m_pCallback = 0;
    m_pCallbackObject = 0;
    m_pCallbackControl = 0;
    m_nPreviousInputClass = 0;
    m_nOpenSound = 15;
    m_bFocusOk = 0;
}
#endif

void CSWGuiMessageBox::DisplayDeathMessage()
{
    SetAllowCancel(0);
    SetMessage(42351);
    m_pCallbackObject = this;
    m_pCallback = (CSWGuiHandler)&CSWGuiMessageBox::OnDeath;
    m_pCallbackControl = 0;
    m_pManager->AddPanel(this, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
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

#ifdef _XBOX

CSWGuiControllerLossBox::CSWGuiControllerLossBox(CSWGuiManager* pManager)
    : CSWGuiMessageBox(pManager)
{
}
#endif

#ifdef _XBOX

#include "LaunchData.h"
#include "clientoptions.h"
#include "exoinput.h"
#include "clientexoappinternal.h"
void ExoXboxLaunch(KOTOR_LAUNCH_DATA* pParams, const char* pszModuleName);

void CSWGuiMessageBox::OnRestart(CSWGuiControl* pControl)
{
    KOTOR_LAUNCH_DATA cParams;
    cParams.m_nModuleTrans = KOTOR_LAUNCH_DATA::MODULETRANS_NONE;
    cParams.m_nAutoCalInfo = g_pExoInput->GetAutoCalInfo();
    cParams.m_nPortBinding = g_pExoInput->GetPortBinding();
    cParams.m_bRenderQA = RenderQA;

    KOTOR_LAUNCH_OPTIONS_DATA cOptionsData;
    g_pAppManager->GetClientExoApp()->GetClientOptions()->GetOptionsData(&cOptionsData);
    cParams.SetOptionsData(cOptionsData);
    ExoXboxLaunch(&cParams, NULL);
}

void CSWGuiMessageBox::ShowLoadError(unsigned long nStrRef)
{
    SetAllowCancel(0);
    SetMessage(nStrRef);
    g_pExoInput->SetActive(0);
    m_pCallbackObject = this;
    m_pCallback = (CSWGuiHandler)&CSWGuiMessageBox::OnRestart;
    m_pCallbackControl = 0;
}
#endif
// PC 0x0067A960 GOG 0x0067A730
void CSWGuiInGameMessages::ShowEffects()
{
    m_lbMessages.m_bEnabled = 0;
    m_lbDialog.SetEnabledFlag(0);
    CSWGuiListBox* pGood = &m_lbEffectsGood;
    pGood->m_bEnabled = 1;
    CSWGuiListBox* pBad = &m_lbEffectsBad;
    pBad->m_bEnabled = 1;
    m_lbCombat.SetEnabledFlag(0);
    m_lblEffectsGood.SetEnabledFlag(1);
    m_lblEffectsBad.SetEnabledFlag(1);

    m_btnDialog.m_cText.m_cParams.SetDefaultColor(CGuiInGame::COLOR_NEW_TEXT);
    m_btnFeedback.m_cText.m_cParams.SetDefaultColor(CGuiInGame::COLOR_NEW_TEXT);
    m_btnCombat.m_cText.m_cParams.SetDefaultColor(CGuiInGame::COLOR_NEW_TEXT);
    m_btnEffects.m_cText.m_cParams.SetDefaultColor(CGuiInGame::COLOR_NEW_HIGHLIGHT);
    pGood->ClearItems();
    pBad->ClearItems();
    CSWCCreature* pCreature = g_pAppManager->m_pClientExoApp->GetSWParty()->GetCharacter(0);
    if (pCreature == 0)
        return;
    CSWSCreature* pServerCreature = pCreature->GetServerCreature();
    if (pServerCreature == 0)
        return;
    CExoArrayList<CSWGuiControl*> aGood;
    CExoArrayList<CSWGuiControl*> aBad;
    CSWGuiExtent cGoodExtent(0, 0,
        pGood->m_cItemExtent.m_nWidth - 2 * pGood->m_nPadding, 10);
    CSWGuiExtent cBadExtent(0, 0,
        pBad->m_cItemExtent.m_nWidth - 2 * pBad->m_nPadding, 10);
    CSWGuiLabelHilight* pGoodProto = (CSWGuiLabelHilight*)m_lbEffectsGood.m_pProtoItem;
    CSWGuiLabelHilight* pBadProto = (CSWGuiLabelHilight*)m_lbEffectsBad.m_pProtoItem;

    CExoString sName;
    for (int i = 0; i < pServerCreature->GetEffectIcons()->num; i++)
    {
        CEffectIconObject* pIcon = pServerCreature->GetEffectIcons()->element[i];

        unsigned long nStrRef = pIcon->m_nStrRef;
        if (nStrRef >= 1)
        {
            sName = g_pTlkTable->GetSimpleString(nStrRef);

            CSWGuiLabelHilight* pLabel = new CSWGuiLabelHilight();
            if (pIcon->m_bGood)
            {
                pLabel->Initialize(cGoodExtent, pGoodProto);
                pLabel->m_cText.m_cParams.SetText(sName);
                pLabel->m_cText.m_cParams.SetColor(CGuiInGame::COLOR_NEW_TEXT);
                CSWGuiText* pText = &pLabel->m_cText;
                cGoodExtent.m_nHeight = pText->GetIdealHeight() +
                    2 * pLabel->m_cHilite.m_cParams.GetBorderDim();
                pLabel->SetExtent(cGoodExtent);
                pLabel->SetActive(0);
                aGood.Add(pLabel);
            }
            else
            {
                pLabel->Initialize(cBadExtent, pBadProto);
                pLabel->m_cText.m_cParams.SetText(sName);
                pLabel->m_cText.m_cParams.SetColor(CGuiInGame::COLOR_NEW_TEXT);
                CSWGuiText* pText = &pLabel->m_cText;
                cBadExtent.m_nHeight = pText->GetIdealHeight() +
                    2 * pLabel->m_cHilite.m_cParams.GetBorderDim();
                pLabel->SetExtent(cBadExtent);
                pLabel->SetActive(0);
                aBad.Add(pLabel);
            }
        }
    }
    if (aGood.num > 0)
    {
        pGood->AddControls(aGood, 1, 1, 1);
        pGood->SetSelectedControl(0, 0);
    }
    if (aBad.num > 0)
    {
        pBad->AddControls(aBad, 1, 1, 1);
        pBad->SetSelectedControl(0, 0);
    }
    g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nMessageType =
        CGuiInGame::MESSAGE_TYPE_EFFECTS;
}
// PC 0x0067AD90 GOG 0x0067AB60
void CSWGuiMessageBox::SetMessage(CExoString sMessage)
{
    g_pTlkTable->ParseStr(sMessage);
    m_lblMessage.m_cText.m_cParams.SetText(sMessage);
#ifndef _XBOX
    m_lblMessage.m_cText.m_cParams.ReSetFont();
#endif
    FixMessageLabel();
}
// PC 0x00675430 GOG 0x006751D0
void CSWGuiInGameMenu::OnShoulderLeft(CSWGuiControl* pControl)
{
    SWGUIINGAME_TRACE();
    if (pControl->m_bSelected != 0)
    {
        g_pAppManager->m_pClientExoApp->GetInGameGui()->PrevSWInGameGui();
    }
}
// PC 0x00675460 GOG 0x00675200
void CSWGuiInGameMenu::OnShoulderRight(CSWGuiControl* pControl)
{
    SWGUIINGAME_TRACE();
    if (pControl->m_bSelected != 0)
    {
        g_pAppManager->m_pClientExoApp->GetInGameGui()->NextSWInGameGui();
    }
}
// PC 0x00675490 GOG 0x00675230
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
// PC 0x00675520 GOG 0x006752C0
void CSWGuiInGameMenu::OnEquipButtonPressed(CSWGuiControl* pControl)
{
    SWGUIINGAME_TRACE();
    g_pAppManager->GetClientExoApp()->GetInGameGui()->SwitchToSWInGameGui(
        CGuiInGame::SCREEN_EQUIP);
}
// PC 0x00675540 GOG 0x006752E0
void CSWGuiInGameMenu::OnInvButtonPressed(CSWGuiControl* pControl)
{
    SWGUIINGAME_TRACE();
    g_pAppManager->GetClientExoApp()->GetInGameGui()->SwitchToSWInGameGui(
        CGuiInGame::SCREEN_INVENTORY);
}
// PC 0x00675560 GOG 0x00675300
void CSWGuiInGameMenu::OnCharButtonPressed(CSWGuiControl* pControl)
{
    SWGUIINGAME_TRACE();
    g_pAppManager->GetClientExoApp()->GetInGameGui()->SwitchToSWInGameGui(
        CGuiInGame::SCREEN_CHARACTER);
}
// PC 0x00675580 GOG 0x00675320
void CSWGuiInGameMenu::OnMapButtonPressed(CSWGuiControl* pControl)
{
    SWGUIINGAME_TRACE();
    g_pAppManager->GetClientExoApp()->GetInGameGui()->SwitchToSWInGameGui(
        CGuiInGame::SCREEN_MAP);
}
// PC 0x006755A0 GOG 0x00675340
void CSWGuiInGameMenu::OnAbiButtonPressed(CSWGuiControl* pControl)
{
    SWGUIINGAME_TRACE();
    g_pAppManager->GetClientExoApp()->GetInGameGui()->SwitchToSWInGameGui(
        CGuiInGame::SCREEN_ABILITIES);
}
// PC 0x006755C0 GOG 0x00675360
void CSWGuiInGameMenu::OnJouButtonPressed(CSWGuiControl* pControl)
{
    SWGUIINGAME_TRACE();
    g_pAppManager->GetClientExoApp()->GetInGameGui()->SwitchToSWInGameGui(
        CGuiInGame::SCREEN_JOURNAL);
}
// PC 0x006755E0 GOG 0x00675380
void CSWGuiInGameMenu::OnOptButtonPressed(CSWGuiControl* pControl)
{
    SWGUIINGAME_TRACE();
    g_pAppManager->GetClientExoApp()->GetInGameGui()->SwitchToSWInGameGui(
        CGuiInGame::SCREEN_OPTIONS);
}
// PC 0x00675600 GOG 0x006753A0
void CSWGuiInGameMenu::OnMsgButtonPressed(CSWGuiControl* pControl)
{
    SWGUIINGAME_TRACE();
    g_pAppManager->GetClientExoApp()->GetInGameGui()->SwitchToSWInGameGui(
        CGuiInGame::SCREEN_MESSAGES);
}
// PC 0x00675620 GOG 0x006753C0
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
// PC 0x00675680 GOG 0x00675420
void CSWGuiInGameMessages::HandleInputEvent(int nEvent, int nState)
{
    if (nState != 0)
    {
        switch (nEvent)
        {
        case GUIEVENT_CANCEL:
        case GUIEVENT_TOGGLE_MODE:
        case GUIEVENT_ACCEPT:
        case GUIEVENT_CANCEL_ALTERNATE:
        case GUIEVENT_EXIT:
            SetModal(0);
            break;
        }

        static int nModeIdx = 0;
        int nModeSequence[] = { GUIEVENT_RECOMMEND, GUIEVENT_LEFT,
                                GUIEVENT_ACTIVATE, GUIEVENT_RECOMMEND };
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
                g_pAppManager->m_pClientExoApp->GetInGameGui()->UpdateMessageGUI(0);

            }
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
// PC 0x006758B0 GOG 0x00675650
void CSWGuiInGameMessages::ShowDialogMessages()
{
    SWGUIINGAME_TRACE();
    m_lbMessages.m_bEnabled = 0;
    CSWGuiListBox* pDialog = &m_lbDialog;
    pDialog->m_bEnabled = 1;
    m_lbCombat.SetEnabledFlag(0);
    m_lbEffectsGood.SetEnabledFlag(0);
    m_lbEffectsBad.SetEnabledFlag(0);
    m_lblEffectsGood.SetEnabledFlag(0);
    m_lblEffectsBad.SetEnabledFlag(0);

    m_btnDialog.m_cText.m_cParams.SetDefaultColor(CGuiInGame::COLOR_NEW_HIGHLIGHT);
    m_btnFeedback.m_cText.m_cParams.SetDefaultColor(CGuiInGame::COLOR_NEW_TEXT);
    m_btnCombat.m_cText.m_cParams.SetDefaultColor(CGuiInGame::COLOR_NEW_TEXT);
    m_btnEffects.m_cText.m_cParams.SetDefaultColor(CGuiInGame::COLOR_NEW_TEXT);
    SetActiveControl(pDialog, 0);
    g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nMessageType =
        CGuiInGame::MESSAGE_TYPE_DIALOG;
}
// PC 0x00675960 GOG 0x00675700
void CSWGuiInGameMessages::ShowFeedbackMessages()
{
    SWGUIINGAME_TRACE();
    CSWGuiListBox* pMessages = &m_lbMessages;
    pMessages->m_bEnabled = 1;
    m_lbDialog.SetEnabledFlag(0);
    m_lbCombat.SetEnabledFlag(0);
    m_lbEffectsGood.SetEnabledFlag(0);
    m_lbEffectsBad.SetEnabledFlag(0);
    m_lblEffectsGood.SetEnabledFlag(0);
    m_lblEffectsBad.SetEnabledFlag(0);
    m_btnDialog.m_cText.m_cParams.SetDefaultColor(CGuiInGame::COLOR_NEW_TEXT);
    m_btnFeedback.m_cText.m_cParams.SetDefaultColor(CGuiInGame::COLOR_NEW_HIGHLIGHT);
    m_btnCombat.m_cText.m_cParams.SetDefaultColor(CGuiInGame::COLOR_NEW_TEXT);
    m_btnEffects.m_cText.m_cParams.SetDefaultColor(CGuiInGame::COLOR_NEW_TEXT);
    SetActiveControl(pMessages, 0);
    g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nMessageType =
        CGuiInGame::MESSAGE_TYPE_FEEDBACK;
}
// PC 0x00675A10 GOG 0x006757B0
void CSWGuiInGameMessages::ShowCombatMessages()
{
    m_lbMessages.m_bEnabled = 0;
    m_lbDialog.SetEnabledFlag(0);
    m_lbEffectsGood.SetEnabledFlag(0);
    m_lbEffectsBad.SetEnabledFlag(0);
    CSWGuiListBox* pCombat = &m_lbCombat;
    pCombat->m_bEnabled = 1;
    m_lblEffectsGood.SetEnabledFlag(0);
    m_lblEffectsBad.SetEnabledFlag(0);

    m_btnDialog.m_cText.m_cParams.SetDefaultColor(CGuiInGame::COLOR_NEW_TEXT);
    m_btnFeedback.m_cText.m_cParams.SetDefaultColor(CGuiInGame::COLOR_NEW_TEXT);
    m_btnCombat.m_cText.m_cParams.SetDefaultColor(CGuiInGame::COLOR_NEW_HIGHLIGHT);
    m_btnEffects.m_cText.m_cParams.SetDefaultColor(CGuiInGame::COLOR_NEW_TEXT);
    SetActiveControl(pCombat, 0);
    g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nMessageType =
        CGuiInGame::MESSAGE_TYPE_COMBAT;
}

#ifdef _XBOX
void CSWGuiMessageBox::HandleInputEvent(int nEvent, int nState)
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
void CSWGuiMessageBox::HandleInputEvent(int nEvent, int nState)  // PC 0x00675AC0 GOG 0x00675860
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
            if (m_n9BC != 0)
                g_pAppManager->m_pClientExoApp->SetPausedByCombat(0, 0, 0);
            break;
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
#endif
// PC 0x00675BF0 GOG 0x00675990
void CSWGuiMessageBox::SetMessageColors(Vector vBorderColor, Vector vTextColor)
{
    m_pBorder->m_cParams.m_vColor = vBorderColor;
    m_btnOk.m_cBorder.m_cParams.m_vColor = vBorderColor;
    m_btnOk.m_cText.m_cParams.SetDefaultColor(vTextColor);
    m_btnCancel.m_cBorder.m_cParams.m_vColor = vBorderColor;
    m_btnCancel.m_cText.m_cParams.SetDefaultColor(vTextColor);
    m_lblMessage.m_cText.m_cParams.SetColor(vTextColor);
}
// PC 0x00675CD0 GOG 0x00675A70
void CSWGuiMessageBox::SetMessage(unsigned long nStrRef)
{
    SetMessage(g_pTlkTable->GetSimpleString(nStrRef));
}
// PC 0x00675D00 GOG 0x00675AA0
void CSWGuiMessageBox::SetOkButtonMessage(unsigned long nStrRef)
{
    SWGUIINGAME_TRACE();
#ifdef _XBOX
    m_lblOk.m_cText.m_cParams.SetStrRef(nStrRef);
#else
    m_btnOk.m_cText.m_cParams.SetStrRef(nStrRef);
#endif
}
// PC 0x00675D10 GOG 0x00675AB0
void CSWGuiMessageBox::SetCancelButtonMessage(unsigned long nStrRef)
{
    SWGUIINGAME_TRACE();
#ifdef _XBOX
    m_lblCancel.m_cText.m_cParams.SetStrRef(nStrRef);
#else
    m_btnCancel.m_cText.m_cParams.SetStrRef(nStrRef);
#endif
}
// PC 0x0067AF50 GOG 0x0067AD20
CSWGuiDeathDisplay::CSWGuiDeathDisplay(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    StartLoadFromLayout("gameover_p");
    InitControl(m_lblMessage, "LBL_MESSAGE", 1);
    InitControl(m_btnLastSave, "BTN_LASTSAVE", 1);
    InitControl(m_btnLoadGame, "BTN_LOADGAME", 1);
    InitControl(m_btnQuit, "BTN_QUIT", 1);
    StopLoadFromLayout();
    m_btnLastSave.AddEvent(GUIEVENT_ACTIVATE, this,
                           (CSWGuiHandler)&CSWGuiDeathDisplay::OnLastSavePressed);
    m_btnLoadGame.AddEvent(GUIEVENT_ACTIVATE, this,
                           (CSWGuiHandler)&CSWGuiDeathDisplay::OnLoadGamePressed);
    m_btnQuit.AddEvent(GUIEVENT_ACTIVATE, this,
                       (CSWGuiHandler)&CSWGuiDeathDisplay::OnQuitPressed);
    m_btnLastSave.m_cBorder.m_cParams.m_vColor = CGuiInGame::DISABLED_TEXT;
    m_btnLastSave.m_cText.m_cParams.SetDefaultColor(CGuiInGame::COLOR_NEW_GREEN);
    m_btnLoadGame.m_cBorder.m_cParams.m_vColor = CGuiInGame::DISABLED_TEXT;
    m_btnLoadGame.m_cText.m_cParams.SetDefaultColor(CGuiInGame::COLOR_NEW_GREEN);
    m_btnQuit.m_cBorder.m_cParams.m_vColor = CGuiInGame::DISABLED_TEXT;
    m_btnQuit.m_cText.m_cParams.SetDefaultColor(CGuiInGame::COLOR_NEW_GREEN);
    m_fFadeTime = 0.5f;
}
// PC 0x00675D20 GOG 0x00675AC0
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
// PC 0x00675D40 GOG 0x00675AE0
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
// PC 0x00675E60 GOG 0x00675C00
void CSWGuiMessageBox::SetCallback(CSWGuiObject* pObject,
                                  CSWGuiHandler pHandler,
                                  CSWGuiControl* pControl)
{
    m_pCallbackObject = pObject;
    m_pCallback = pHandler;
    m_pCallbackControl = pControl;
}
// PC 0x0067B240 GOG 0x0067B010
CSWGuiInGameMenu::CSWGuiInGameMenu(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    SwguiingameTrace2();
    StartLoadFromLayout("top_p");
    InitControl(m_LblhEqu, "LBLH_EQU", 1);
    InitControl(m_LblhInv, "LBLH_INV", 1);
    InitControl(m_LblhCha, "LBLH_CHA", 1);
    InitControl(m_LblhMap, "LBLH_MAP", 1);
    InitControl(m_LblhAbi, "LBLH_ABI", 1);
    InitControl(m_LblhJou, "LBLH_JOU", 1);
    InitControl(m_LblhOpt, "LBLH_OPT", 1);
    InitControl(m_LblhMsg, "LBLH_MSG", 1);
#ifndef _XBOX
    InitControl(m_btnEqu, "BTN_EQU", 1); InitControl(m_btnInv, "BTN_INV", 1);
    InitControl(m_btnChar, "BTN_CHAR", 1); InitControl(m_btnMap, "BTN_MAP", 1);
    InitControl(m_btnAbi, "BTN_ABI", 1); InitControl(m_btnJou, "BTN_JOU", 1);
    InitControl(m_btnOpt, "BTN_OPT", 1); InitControl(m_btnMsg, "BTN_MSG", 1);
    InitControl(m_lblTopClass1, "LBL_TOP_CLASS1", 1);
    InitControl(m_lblTopClass2, "LBL_TOP_CLASS2", 1);
    InitControl(m_lblTopClass1Level, "LBL_TOP_CLASS1LEVEL", 1);
    InitControl(m_lblTopClass2Level, "LBL_TOP_CLASS2LEVEL", 1);
    InitControl(m_lblCharName, "LBL_CHARNAME", 1);
    m_cChar2350.Initialize((CSWGuiMainInterface*)this, 1);
    InitControl(m_lblChar2, "LBL_CHAR2", 1);
    InitControl(m_lblLevelUp2, "LBL_LEVELUP2", 1);
    InitControl(m_lblBack2, "LBL_BACK2", 1);
    InitControl(m_btnChange2, "BTN_CHANGE2", 1);
    InitControl(m_lblChar3, "LBL_CHAR3", 1);
    InitControl(m_lblLevelUp3, "LBL_LEVELUP3", 1);
    InitControl(m_lblBack3, "LBL_BACK3", 1);
    InitControl(m_btnChange3, "BTN_CHANGE3", 1);
    InitControl(m_lblSecTitle, "LBL_SECTITLE", 1);
#else
    InitControl(m_lblLeftArrow, "LBL_LEFT_ARROW", 1); InitControl(m_lblRightArrow, "LBL_RIGHT_ARROW", 1);
#endif
    StopLoadFromLayout();
#ifndef _XBOX
    m_btnChange2.SetFocusable(0);
    m_btnChange2.AddEvent(CSWGuiControl::EVENT_SELECT, this,
        (CSWGuiHandler)&CSWGuiInGameMenu::OnChangeCharacter);
    m_btnChange3.SetFocusable(0);
    m_btnChange3.AddEvent(CSWGuiControl::EVENT_SELECT, this,
        (CSWGuiHandler)&CSWGuiInGameMenu::OnChangeCharacter);
    m_n3DCC = 0;
#endif

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
#ifndef _XBOX
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

    m_btnEqu.SetFocusable(0); m_btnInv.SetFocusable(0);
    m_btnChar.SetFocusable(0); m_btnMap.SetFocusable(0);
    m_btnAbi.SetFocusable(0); m_btnJou.SetFocusable(0);
    m_btnOpt.SetFocusable(0); m_btnMsg.SetFocusable(0);
    SetActiveControl(&m_btnEqu, 0);
#else
    SetActiveControl(&m_LblhEqu, 0);
    m_bCenterHorizontal = 1;
    m_bCenterVertical = 1;
#endif
#ifndef _XBOX
    m_btnEqu.SetToolTip(STRREF_TOOLTIP_EQUIP, GUIEVENT_MENU_EQUIPMENT);
    m_btnInv.SetToolTip(STRREF_TOOLTIP_INVENTORY, GUIEVENT_MENU_INVENTORY);
    m_btnChar.SetToolTip(STRREF_TOOLTIP_CHARACTER, GUIEVENT_MENU_CHARACTER);
    m_btnMap.SetToolTip(STRREF_TOOLTIP_MAP, GUIEVENT_MENU_MAP);
    m_btnAbi.SetToolTip(STRREF_TOOLTIP_ABILITIES, GUIEVENT_MENU_ABILITIES);
    m_btnJou.SetToolTip(STRREF_TOOLTIP_JOURNAL, GUIEVENT_MENU_JOURNAL);
    m_btnOpt.SetToolTip(STRREF_TOOLTIP_OPTIONS, GUIEVENT_MENU_OPTIONS);
    m_btnMsg.SetToolTip(STRREF_TOOLTIP_PARTY_SELECTION, GUIEVENT_MENU_MESSAGES);
    m_nTopMenuOverrideCharacter = -1;
#endif
}
// PC 0x00675E80 GOG 0x00675C20
void CSWGuiMessageBox::OnPanelAdded()
{
    SWGUIINGAME_TRACE();
    if (m_n9BC != 0)
    {
        m_pBorder->m_cParams.SetFillImage("pnl_datapad", 0);
    }
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
// PC 0x00675FC0 GOG 0x00675D60
void CSWGuiMessageBox::OnPanelRemoved()
{
    SWGUIINGAME_TRACE();
#ifndef _XBOX
    CSWGuiPanel::OnPanelRemoved();
#endif
    m_pCallback = 0;
    m_pCallbackObject = 0;
    m_pCallbackControl = 0;
    if (m_n9BC != 0)
    {
        m_pBorder->m_cParams.SetFillImage("pnl_datapad", 0);
    }
    CSWGuiTextParams* pParams = &m_lblMessage.m_cText.m_cParams;
    pParams->SetAlignment(10);
    pParams->SetFont("dialogfont16x16");
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
    if (m_n9BC == 0)
    {
        SetMessageColors(CGuiInGame::COLOR_NEW_GREEN,
                         CGuiInGame::COLOR_NEW_HIGHLIGHT);
    }
    if (m_n9C4 != 0)
    {
        g_pAppManager->m_pClientExoApp->SetPausedByCombat(0, 0, 0);
        m_n9C4 = 0;
    }
}
// PC 0x006760D0 GOG 0x00675E70
void CSWGuiControllerLossBox::OnAccept(CSWGuiControl* pControl)
{
    SWGUIINGAME_TRACE();
    g_pAppManager->m_pClientExoApp->GetInGameGui()->EndControllerPauseState();
}

#ifndef _XBOX
void CSWGuiMessageBox::OnOkButtonPressed(CSWGuiControl* pControl)  // PC 0x006760F0 GOG 0x00675E90
{
    SWGUIINGAME_TRACE();
    HandleInputEvent(502, m_btnOk.m_bSelected);
}
// PC 0x00676110 GOG 0x00675EB0
void CSWGuiMessageBox::OnCancelButtonPressed(CSWGuiControl* pControl)
{
    SWGUIINGAME_TRACE();
    HandleInputEvent(503, m_btnCancel.m_bSelected);
}
#endif
// PC 0x00676130 GOG 0x00675ED0
void CSWGuiStatusSummary::OnPanelRemoved()
{
#ifndef _XBOX
    g_pGuiMan->RestoreOldMousePosition();
#endif
}
// PC 0x00676140 GOG 0x00675EE0
void CSWGuiStatusSummary::PlayPendingSounds()
{
    if (m_cStatusSummary.m_bLevelUpSound)
        m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_LEVELUP);
    if (m_cStatusSummary.m_bNewQuestSound)
        m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_NEWQUEST);
    if (m_cStatusSummary.m_bCompleteSound)
        m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_QUESTCOMPLETE);
    CSWCCreature* pCreature = g_pAppManager->m_pClientExoApp->GetSWParty()->GetCharacter(0);
    CSWSCreature* pServerCreature = 0;
    if (pCreature)
        pServerCreature = pCreature->GetServerCreature();
    if (m_cStatusSummary.m_bDarkAlignSound)
    {
        m_pManager->PlayGuiSound(GUISOUND_DARK_SIDE);
        m_cStatusSummary.m_bDarkAlignSound = 0;
        if (pServerCreature && pServerCreature->m_bPlayerCreated)
        {
            CGameEffect* pEffect = new CGameEffect(1);
            pEffect->m_nType = CSWSEffectListHandler::EFFECT_VISUALEFFECT;
            pEffect->SetInteger(0, VFX_IMP_DARK_SIDE);
            pEffect->m_nSubType = (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                CGameEffect::DURATION_TYPE_INSTANTANEOUS;
            pEffect->SetCreator(pServerCreature->GetId());
            pServerCreature->ApplyEffect(pEffect, 0, 0);
        }
    }
    if (m_cStatusSummary.m_bLightAlignSound)
    {
        m_pManager->PlayGuiSound(GUISOUND_LIGHT_SIDE);
        m_cStatusSummary.m_bLightAlignSound = 0;
        if (pServerCreature && pServerCreature->m_bPlayerCreated)
        {
            CGameEffect* pEffect = new CGameEffect(1);
            pEffect->m_nType = CSWSEffectListHandler::EFFECT_VISUALEFFECT;
            pEffect->SetInteger(0, VFX_IMP_LIGHT_SIDE);
            pEffect->m_nSubType = (pEffect->m_nSubType & ~CGameEffect::DURATION_MASK) |
                CGameEffect::DURATION_TYPE_INSTANTANEOUS;
            pEffect->SetCreator(pServerCreature->GetId());
            pServerCreature->ApplyEffect(pEffect, 0, 0);
        }
    }
}

// PC 0x00676320 GOG 0x006760C0
void CSWGuiStatusSummary::AddAlignmentShift(char nShift)
{
    if (nShift != 0)
    {
        if (nShift > 0)
            m_cStatusSummary.m_nLightShift += nShift;
        else
            m_cStatusSummary.m_nDarkShift -= nShift;
        if (m_cStatusSummary.m_nLightShift == m_cStatusSummary.m_nDarkShift)
        {
            m_cStatusSummary.m_bDarkAlignSound = 0;
            m_cStatusSummary.m_bLightAlignSound = 0;
            if (!m_cStatusSummary.m_bLevelUpSound && !m_cStatusSummary.m_bNewQuestSound &&
                !m_cStatusSummary.m_bCompleteSound)
                m_cStatusSummary.m_bSoundPending = 0;
        }
        else if (m_cStatusSummary.m_nLightShift > m_cStatusSummary.m_nDarkShift)
        {
            m_cStatusSummary.m_bDarkAlignSound = 0;
            m_cStatusSummary.m_bLightAlignSound = 1;
            m_cStatusSummary.m_bSoundPending = 1;
        }
        else
        {
            m_cStatusSummary.m_bLightAlignSound = 0;
            m_cStatusSummary.m_bDarkAlignSound = 1;
            m_cStatusSummary.m_bSoundPending = 1;
        }
        m_cStatusSummary.m_bDisplaySPending = 1;
    }
}
// PC 0x00676390 GOG 0x00676130
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
// PC 0x006763C0 GOG 0x00676160
void CSWGuiStatusSummary::AddInfluenceGained(char nNPC)
{
    m_cStatusSummary.m_nInfluenceGain |= 1 << nNPC;
    m_cStatusSummary.m_bDisplaySPending = 1;
}
// PC 0x006763E0 GOG 0x00676180
void CSWGuiStatusSummary::AddInfluenceLost(char nNPC)
{
    m_cStatusSummary.m_nInfluenceLost |= 1 << nNPC;
    m_cStatusSummary.m_bDisplaySPending = 1;
}
// PC 0x00676400 GOG 0x006761A0
void CSWGuiStatusSummary::AddMaxFPGained(int nForcePoints)
{
    m_cStatusSummary.m_nMaxForcePointsGain += nForcePoints;
    m_cStatusSummary.m_bDisplaySPending = 1;
}
// PC 0x00676420 GOG 0x006761C0
void CSWGuiStatusSummary::AddMaxFPLost(int nForcePoints)
{
    m_cStatusSummary.m_nMaxForcePointsLost += nForcePoints;
    m_cStatusSummary.m_bDisplaySPending = 1;
}
// PC 0x00676440 GOG 0x006761E0
void CSWGuiStatusSummary::HandleAlignmentMovie()
{
    CExoString sScript("k_align_movie");
    g_pVirtualMachine->RunScript(&sScript, 0, 0);
}
// PC 0x006764B0 GOG 0x00676250
void CSWGuiStatusSummary::HandleLevelUpDLG()
{
    CSWSCreature* pCreature = (CSWSCreature*)g_pAppManager->m_pServerExoApp->GetGameObject(
        g_pAppManager->m_pServerExoApp->GetFirstPCObject());
    if (pCreature != 0 && pCreature->m_bPlayerCreated != 0 && pCreature->m_pStats->CanLevelUp())
    {
        char bFired = 0;
        {
            CExoString sVariable("000_Level_Dlg_Fired");
            g_pAppManager->GetServerExoApp()->GetGlobalVariableTable()->GetValueNumber(sVariable, bFired);
        }
        if (!bFired)
        {
            {
                CExoString sVariable("000_Level_Dlg_Fired");
                g_pAppManager->GetServerExoApp()->GetGlobalVariableTable()->SetValueNumber(sVariable, 1);
            }
            CExoString sScript("k_level_dlg");
            g_pVirtualMachine->RunScript(&sScript, 0, 0);
        }
    }
}
// PC 0x006765F0 GOG 0x00676390
void CSWGuiToolTipPanel::SetToolTipText(const CExoString& sText)
{
    m_lblToolTip.m_cText.m_cParams.SetText(sText);
}

#ifndef _XBOX
void CSWGuiToolTipPanel::PositionToolTip()  // PC 0x00676600 GOG 0x006763A0
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
// PC 0x006766B0 GOG 0x00676450
void CSWGuiInGameMenu::ShowChangeButtons(int bShow)
{
    m_btnChange2.SetShown(bShow);
    m_btnChange2.SetEnabled(bShow);
    m_btnChange3.SetShown(bShow);
    m_btnChange3.SetEnabled(bShow);
}
#endif
// PC 0x00676710 GOG 0x006764B0
void CSWGuiDeathDisplay::OnPanelAdded()
{
    g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GUI, 0);
    CSWGuiExtent cExtent = m_cExtent;
    cExtent.m_nLeft = g_pAppManager->m_pClientExoApp->GetSWGuiManager()->m_nWidth / 2 -
                      cExtent.m_nWidth / 2;
    cExtent.m_nTop = g_pAppManager->m_pClientExoApp->GetSWGuiManager()->m_nHeight / 2 -
                     cExtent.m_nHeight / 2;
    SetExtent(cExtent);
    g_pAppManager->m_pClientExoApp->ShowMouse(32);
    g_pAppManager->m_pClientExoApp->SetGuiGameSlow(1);
    CSWGuiPanel::OnPanelAdded();
}
// PC 0x006767D0 GOG 0x00676570
void CSWGuiDeathDisplay::OnPanelRemoved()
{
    g_pAppManager->m_pClientExoApp->SetGuiGameSlow(0);
}
// PC 0x006767E0 GOG 0x00676580
void CSWGuiDeathDisplay::Draw(float fDeltaTime)
{
    m_fFadeTime -= fDeltaTime;
    if (m_fFadeTime < 0.0)
    {
        m_btnLastSave.m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_GREEN;
        m_btnLastSave.m_cText.m_cParams.SetDefaultColor(CGuiInGame::COLOR_NEW_TEXT);
        m_btnLoadGame.m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_GREEN;
        m_btnLoadGame.m_cText.m_cParams.SetDefaultColor(CGuiInGame::COLOR_NEW_TEXT);
        m_btnQuit.m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_GREEN;
        m_btnQuit.m_cText.m_cParams.SetDefaultColor(CGuiInGame::COLOR_NEW_TEXT);
    }
    CSWGuiPanel::Draw(fDeltaTime);
}
// PC 0x006768B0 GOG 0x00676650
void CSWGuiDeathDisplay::OnQuitPressed(CSWGuiControl* pControl)
{
    if (m_fFadeTime > 0.0)
        return;
    g_pAppManager->m_pClientExoApp->SetEndGamePending(1, 0.0f, 1);
    g_pAppManager->m_pClientExoApp->SetGameOver(0);
    g_pAppManager->m_pClientExoApp->GetInGameGui()->RemoveDeathMessage(1);
}
// PC 0x00676AD0 GOG 0x00676870
void CSWGuiInGameMenu::SetActiveControlID(int nControl, int bActive)
{
    SWGUIINGAME_TRACE();
    if (nControl >= 0 && nControl < m_lControls.num)
        SetActiveControl(m_lControls.element[nControl], bActive);
}
#ifndef _XBOX
// PC 0x00676B00 GOG 0x006768A0
void CSWGuiInGameMenu::Draw(float fElapsed)
{
    CSWCCreature* pCreature;
    if (m_nTopMenuOverrideCharacter != -1)
    {
        CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;
        CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
        pCreature = pClient->GetCreatureByGameObjectID(pClient->ServerToClientObjectId(
            pServerExoApp->GetPartyTable()->GetNPCObject(m_nTopMenuOverrideCharacter, 0, 1)));
        if (pCreature == NULL)
        {
            return;
        }
    }
    else
    {
        pCreature = g_pAppManager->m_pClientExoApp->GetSWParty()->GetCharacter(0);
    }
    CSWSCreature* pServer = pCreature->GetServerCreature();
    CSWSCreatureStats* pStats = pServer->GetStats();
    m_cChar2350.SetPortrait(pCreature->GetPortrait(1));
    int nPoisonState = pServer->m_nPoisonState;
    m_cChar2350.SetHealth(pServer->GetCurrentHitPoints(0), pServer->GetMaxHitPoints(1), nPoisonState);
    int nMaxForce = pServer->GetMaxForcePoints();
    m_cChar2350.SetForce(pStats->GetCurrentForcePoints(), nMaxForce);
    int nState = 0;
    int nBuffCount = 0;
    int nDebuffCount = 0;
    if (pServer->GetDead() || pServer->GetDeadTemp())
    {
        nState = CSWGuiMainInterfaceChar::PORTRAIT_DEAD;
    }
    else if (pServer->GetStats()->CanLevelUp())
    {
        nState = CSWGuiMainInterfaceChar::PORTRAIT_LEVEL_UP;
    }
    else if (pServer->IsDebilitated(0))
    {
        nState = CSWGuiMainInterfaceChar::PORTRAIT_DEBILITATED;
    }
    else
    {
        int nIcon = 0;
        int nEffectCount = pServer->GetEffectIconCount();
        if (nEffectCount > 0)
        {
            do
            {
                if (pServer->m_lstEffectIcons.element[nIcon]->m_bGood)
                    nBuffCount++;
                else
                    nDebuffCount++;
                ++nIcon;
            } while (nIcon < nEffectCount);
        }
    }
    CExoString sName = pCreature->GetName();
    int nTag = sName.Find('<', 0);
    int nToken = sName.Find('{', 0);
    if (nTag != -1 || nToken != -1)
    {
        g_pSWTlkTable->ParseStr(sName, (CGameObject*)pCreature, 0, 1);
    }
    m_lblCharName.m_cText.m_cParams.SetText(sName);
    if (pStats->GetNumClasses() == 1)
    {
        unsigned long nStrRef = g_pRules->m_pClasses[pStats->GetClass(0)].m_nNameStrRef;
        m_lblTopClass1.m_cText.m_cParams.SetText(
            g_pAppManager->GetClientExoApp()->GetGUIString(nStrRef));
        m_lblTopClass1Level.m_cText.m_cParams.SetText(pStats->GetClassLevel(0, 0));
        m_lblTopClass2.m_cText.m_cParams.SetText("");
        m_lblTopClass2Level.m_cText.m_cParams.SetText("");
    }
    else
    {
        unsigned long nStrRef = g_pRules->m_pClasses[pStats->GetClass(0)].m_nNameStrRef;
        m_lblTopClass1.m_cText.m_cParams.SetText(
            g_pAppManager->GetClientExoApp()->GetGUIString(nStrRef));
        m_lblTopClass1Level.m_cText.m_cParams.SetText(pStats->GetClassLevel(0, 0));
        nStrRef = g_pRules->m_pClasses[pStats->GetClass(1)].m_nNameStrRef;
        m_lblTopClass2.m_cText.m_cParams.SetText(
            g_pAppManager->GetClientExoApp()->GetGUIString(nStrRef));
        m_lblTopClass2Level.m_cText.m_cParams.SetText(pStats->GetClassLevel(1, 0));
    }
    m_cChar2350.Show(1, nState, nBuffCount, nDebuffCount);
    CSWGuiPanel::Draw(fElapsed);
    m_cChar2350.DrawEffects(fElapsed, 1);
    int nCharacters = g_pAppManager->m_pClientExoApp->GetSWParty()->GetNumCharacters();
    if (nCharacters == 1 || nCharacters == 0)
    {
        m_lblChar2.SetEnabledFlag(0);
        m_lblBack2.SetEnabledFlag(0);
        m_lblLevelUp2.SetEnabledFlag(0);
        m_btnChange2.SetEnabledFlag(0);
        m_lblChar3.SetEnabledFlag(0);
        m_lblBack3.SetEnabledFlag(0);
        m_lblLevelUp3.SetEnabledFlag(0);
        m_btnChange3.SetEnabledFlag(0);
    }
    else if (nCharacters == 2)
    {
        CSWCCreature* pMember = g_pAppManager->m_pClientExoApp->GetSWParty()->GetCharacter(1);
        CSWSCreature* pMemberServer = pMember->GetServerCreature();
        m_lblChar2.m_cBorder.m_cParams.SetFillImage(pMember->GetPortrait(1));
        m_lblChar2.SetEnabledFlag(1);
        m_lblBack2.SetEnabledFlag(1);
        if (pMemberServer->GetStats()->CanLevelUp())
            m_lblLevelUp2.SetEnabledFlag(1);
        else
            m_lblLevelUp2.SetEnabledFlag(0);
        m_btnChange2.SetEnabledFlag(1);
        m_lblChar3.SetEnabledFlag(0);
        m_lblBack3.SetEnabledFlag(0);
        m_lblLevelUp3.SetEnabledFlag(0);
        m_btnChange3.SetEnabledFlag(0);
    }
    else if (nCharacters == 3)
    {
        CSWCCreature* pMember = g_pAppManager->m_pClientExoApp->GetSWParty()->GetCharacter(1);
        CSWSCreature* pMemberServer = pMember->GetServerCreature();
        m_lblChar2.m_cBorder.m_cParams.SetFillImage(pMember->GetPortrait(1));
        m_lblChar2.SetEnabledFlag(1);
        m_lblBack2.SetEnabledFlag(1);
        if (pMemberServer->GetStats()->CanLevelUp())
            m_lblLevelUp2.SetEnabledFlag(1);
        else
            m_lblLevelUp2.SetEnabledFlag(0);
        m_btnChange2.SetEnabledFlag(1);
        pMember = g_pAppManager->m_pClientExoApp->GetSWParty()->GetCharacter(2);
        pMemberServer = pMember->GetServerCreature();
        m_lblChar3.m_cBorder.m_cParams.SetFillImage(pMember->GetPortrait(1));
        m_lblChar3.SetEnabledFlag(1);
        m_lblBack3.SetEnabledFlag(1);
        if (pMemberServer->GetStats()->CanLevelUp())
            m_lblLevelUp3.SetEnabledFlag(1);
        else
            m_lblLevelUp3.SetEnabledFlag(0);
        m_btnChange3.SetEnabledFlag(1);
    }
}
#endif

// PC 0x00677240 GOG 0x00676FE0
CSWGuiInGameMessages::~CSWGuiInGameMessages()
{
    m_lbDialog.ClearItems();
    m_lbMessages.ClearItems();
    m_lbCombat.ClearItems();
    m_lbEffectsGood.ClearItems();
    m_lbEffectsBad.ClearItems();
    delete[] m_pDialogLabels;
    delete[] m_pMessageLabels;
    delete[] m_pCombatLabels;
}
// PC 0x00677430 GOG 0x006771D0
CSWGuiMessageBox::CSWGuiMessageBox(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    m_bAllowCancel = 1;
    m_bShowButtons = 1;
    m_bHideButtons = 0;
    m_bHasIcon = 0;
    m_n9BC = 0;
    StartLoadFromLayout("confirm_p");
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
    m_n9C4 = 0;
    m_nOpenSound = 15;
}
// PC 0x00677A20 GOG 0x006777C0
void CSWGuiMessageBox::SetAllowCancel(int bAllowCancel)
{
    m_bAllowCancel = bAllowCancel;
    m_nOpenSound = bAllowCancel != 0 ? 15 : 2;
    UpdateControls();
}
// PC 0x00677A50 GOG 0x006777F0
void CSWGuiMessageBox::SetIcon(const CResRef& cIcon)
{
    if (cIcon.IsValid())
    {
        m_bHasIcon = 1;
        m_lblIcon.m_cBorder.m_cParams.SetFillImage(cIcon, 0);
        UpdateControls();
    }
    else
    {
        m_bHasIcon = 0;
        UpdateControls();
    }
}
// PC 0x00675C70 GOG 0x00675A10
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
// PC 0x00677AA0 GOG 0x00677840
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

    if (m_n9BC != 0)
    {
        cPanel.m_nHeight = 300;
        cPanel.m_nWidth = 400;
        cPanel.m_nLeft =
            g_pAppManager->m_pClientExoApp->GetSWGuiManager()->m_nWidth / 2 -
            cPanel.m_nWidth / 2;
    }
    cMessage.m_nWidth += cPanel.m_nWidth - m_cPanelExtent.m_nWidth;
    SetExtent(cPanel);
    if (m_n9BC != 0)
    {
        cMessage.m_nHeight = 208;
        cMessage.m_nWidth = 334;
        cMessage.m_nLeft = 33;
        cMessage.m_nTop = 27;
    }
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
    if (m_n9BC != 0)
    {
        cControl.m_nTop = 245;
        cControl.m_nHeight = 28;
        cControl.m_nWidth = 120;
        cControl.m_nLeft = cPanel.m_nWidth / 2 - cControl.m_nWidth / 2;
    }
    else
    {
        cControl = m_btnOk.m_cExtent;
        cControl.m_nTop = m_lbMessage.m_cExtent.m_nHeight + m_lbMessage.m_cExtent.m_nTop + 4;
        cControl.m_nLeft = nCenter - cControl.m_nWidth / 2;
    }
    m_btnOk.SetExtent(cControl);
    cControl = m_btnCancel.m_cExtent;
    cControl.m_nTop = m_btnOk.m_cExtent.m_nHeight + m_btnOk.m_cExtent.m_nTop + 2;
    cControl.m_nLeft = nCenter - cControl.m_nWidth / 2;
    m_btnCancel.SetExtent(cControl);
    CenterPanel();
#endif
}
// PC 0x00675D90 GOG 0x00675B30
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
// PC 0x006780A0 GOG 0x00677E40
void CSWGuiControllerLossBox::OnPanelAdded()
{
    SetAllowCancel(0);
    SetMessage(STRREF_CONTROLLER_LOST);
    m_pCallbackObject = this;
    m_pCallback = (CSWGuiHandler)&CSWGuiControllerLossBox::OnAccept;
    m_pCallbackControl = 0;
    CSWGuiMessageBox::OnPanelAdded();
}
// PC 0x006780E0 GOG 0x00677E80
CSWGuiStatusSummary::CSWGuiStatusSummary(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    StartLoadFromLayout("statussummary_p");
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
    InitControl(m_aStatusLabels[GUI_STATUS_INFLUENCE_GAIN], "LBL_INFLUENCE_RECV", 1);
    InitControl(m_aStatusDescriptions[GUI_STATUS_INFLUENCE_GAIN], "LBL_INFLUENCE_RECV_DESC", 1);
    InitControl(m_aStatusLabels[GUI_STATUS_INFLUENCE_LOST], "LBL_INFLUENCE_LOST", 1);
    InitControl(m_aStatusDescriptions[GUI_STATUS_INFLUENCE_LOST], "LBL_INFLUENCE_LOST_DESC", 1);
    InitControl(m_aStatusLabels[GUI_STATUS_MAX_FP_GAIN], "LBL_MAX_FP_GAINED", 1);
    InitControl(m_aStatusDescriptions[GUI_STATUS_MAX_FP_GAIN], "LBL_MAX_FP_GAINED_DESC", 1);
    InitControl(m_aStatusLabels[GUI_STATUS_MAX_FP_LOST], "LBL_MAX_FP_LOST", 1);
    InitControl(m_aStatusDescriptions[GUI_STATUS_MAX_FP_LOST], "LBL_MAX_FP_LOST_DESC", 1);
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
    m_cStatusSummary.m_bDarkAlignSound = 0;
    m_cStatusSummary.m_bLightAlignSound = 0;
    m_cStatusSummary.m_bCreditsNet = 0;
    m_cStatusSummary.m_nCredits = 0;
    m_cStatusSummary.m_nLightShift = 0;
    m_cStatusSummary.m_nDarkShift = 0;
    m_cStatusSummary.m_nExperience = 0;
    m_cStatusSummary.m_nStealthExperience = 0;
    m_cStatusSummary.m_nInfluenceGain = 0;
    m_cStatusSummary.m_nInfluenceLost = 0;
    m_cStatusSummary.m_nMaxForcePointsGain = 0;
    m_cStatusSummary.m_nMaxForcePointsLost = 0;
    m_cStatusSummary.m_nSuppressed = 0;
    m_n94 = 0;

#ifndef _XBOX
    m_bPositioned = 1;
#endif
}
// PC 0x00678800 GOG 0x006785A0
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
        case GUI_STATUS_INFLUENCE_GAIN:
            bShow = m_cStatusSummary.m_nInfluenceGain;
            break;
        case GUI_STATUS_INFLUENCE_LOST:
            bShow = m_cStatusSummary.m_nInfluenceLost;
            break;
        case GUI_STATUS_MAX_FP_GAIN:
            bShow = m_cStatusSummary.m_nMaxForcePointsGain;
            break;
        case GUI_STATUS_MAX_FP_LOST:
            bShow = m_cStatusSummary.m_nMaxForcePointsLost;
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
    m_cStatusSummary.m_bDarkAlignSound = 0;
    m_cStatusSummary.m_bLightAlignSound = 0;
    m_cStatusSummary.m_bCreditsNet = 0;
    m_cStatusSummary.m_nCredits = 0;
    m_cStatusSummary.m_nLightShift = 0;
    m_cStatusSummary.m_nDarkShift = 0;
    m_cStatusSummary.m_nExperience = 0;
    m_cStatusSummary.m_nStealthExperience = 0;
    m_cStatusSummary.m_nInfluenceGain = 0;
    m_cStatusSummary.m_nInfluenceLost = 0;
    m_cStatusSummary.m_nMaxForcePointsGain = 0;
    m_cStatusSummary.m_nMaxForcePointsLost = 0;
}

// PC 0x00678930 GOG 0x006786D0
void CSWGuiStatusSummary::OnPanelAdded()
{
    SWGUIINGAME_TRACE();

    int bJournal = 0;
    CExoString sDescription;
    CSWGuiExtent cExtent;
    cExtent.m_nLeft = cExtent.m_nTop = cExtent.m_nWidth = cExtent.m_nHeight = 0;

    PlayPendingSounds();
    if (m_cStatusSummary.m_nDarkShift || m_cStatusSummary.m_nLightShift)
        m_n94 = 1;

    float fScaleX = 1.0f;
    float fScaleY = 1.0f;
    if (g_pAppManager->GetClientExoApp()->GetSWGuiManager() != NULL)
    {
        fScaleX = (float)g_pAppManager->GetClientExoApp()->GetSWGuiManager()->m_nWidth / (float)g_nGUIWidth;
        fScaleY = (float)g_pAppManager->GetClientExoApp()->GetSWGuiManager()->m_nHeight / (float)g_nGUIHeight;
    }
    int nTop = (int)(fScaleY * 10.0f);
    int nWidth = (int)(fScaleX * 150.0f);

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
        {
            CGuiInGame* pInGameGui = g_pAppManager->GetClientExoApp()->GetInGameGui();
            CExoString sItems;
            for (int nItem = 0; nItem < pInGameGui->m_a1030.num; nItem++)
            {
                CExoLocString sLocName(pInGameGui->m_a1030[nItem]);
                CExoString sName;
                sLocName.GetString(g_pAppManager->GetClientExoApp()->GetClientLanguage(), &sName, 0);
                if (sItems == "")
                    sItems = sName + "\n";
                else
                    sItems.Format("%s%s\n", sItems.CStr(), sName.CStr());
                int nTokenStart = sItems.Find('<', 0);
                int nCustomStart = sItems.Find('{', 0);
                if (nTokenStart != -1 || nCustomStart != -1)
                    g_pSWTlkTable->ParseStr(sItems,
                        g_pAppManager->GetClientExoApp()->GetSWParty()->GetPlayerCharacter(), 0, 1);
            }
            if (sItems == "")
                bShow = 0;
            else
            {
                bShow = 1;
                bSetDescription = 1;
                g_pTlkTable->SetCustomToken(0, sItems);
                sDescription = g_pAppManager->GetClientExoApp()->GetGUIString(STRREF_ITEMS_RECEIVED);
            }
            break;
        }
        case GUI_STATUS_ITEM_LOST:
            bShow = m_cStatusSummary.m_bItemLost;
            break;
        case GUI_STATUS_INFLUENCE_GAIN:
        case GUI_STATUS_INFLUENCE_LOST:
        {
            CExoString sNames;
            int nNPC = 0;
            CSWPartyTable* pPartyTable = g_pAppManager->m_pServerExoApp->GetPartyTable();
            int nMask = nStatus == GUI_STATUS_INFLUENCE_GAIN ? m_cStatusSummary.m_nInfluenceGain
                                                              : m_cStatusSummary.m_nInfluenceLost;
            for (; nNPC < INFLUENCE_NPC_COUNT; nNPC++)
            {
                if (((nMask >> nNPC) & 1) == 0)
                    continue;
                CExoString sName;
                switch (nNPC)
                {
                case NPC_ATTON:
                    sName = g_pSWTlkTable->Fetch(STRREF_NAME_ATTON, NULL, 0, 0);
                    break;
                case NPC_BAO_DUR:
                    sName = g_pSWTlkTable->Fetch(STRREF_NAME_BAO_DUR, NULL, 0, 0);
                    break;
                case NPC_MANDALORE:
                    sName = g_pSWTlkTable->Fetch(STRREF_NAME_MANDALORE, NULL, 0, 0);
                    break;
                case NPC_G0T0:
                    sName = g_pSWTlkTable->Fetch(STRREF_NAME_G0T0, NULL, 0, 0);
                    break;
                case NPC_HANDMAIDEN:
                    sName = g_pSWTlkTable->Fetch(STRREF_NAME_HANDMAIDEN, NULL, 0, 0);
                    break;
                case NPC_HK_47:
                    sName = g_pSWTlkTable->Fetch(STRREF_NAME_HK_47, NULL, 0, 0);
                    break;
                case NPC_KREIA:
                    sName = g_pSWTlkTable->Fetch(STRREF_NAME_KREIA, NULL, 0, 0);
                    break;
                case NPC_MIRA:
                    sName = g_pSWTlkTable->Fetch(STRREF_NAME_MIRA, NULL, 0, 0);
                    break;
                case NPC_T3_M4:
                    sName = g_pSWTlkTable->Fetch(STRREF_NAME_T3_M4, NULL, 0, 0);
                    break;
                case NPC_VISAS:
                    sName = g_pSWTlkTable->Fetch(STRREF_NAME_VISAS, NULL, 0, 0);
                    break;
                case NPC_HANHARR:
                    sName = g_pSWTlkTable->Fetch(STRREF_NAME_HANHARR, NULL, 0, 0);
                    break;
                case NPC_DISCIPLE:
                    sName = g_pSWTlkTable->Fetch(STRREF_NAME_DISCIPLE, NULL, 0, 0);
                    break;
                case NPC_REMOTE:
                    sName = g_pSWTlkTable->Fetch(STRREF_NAME_REMOTE, NULL, 0, 0);
                    break;
                }
                if (sNames == "")
                    sNames = sName;
                else
                    sNames.Format("%s, %s", sNames.CStr(), sName.CStr());
            }
            if (sNames == "")
                bShow = 0;
            else
            {
                bShow = 1;
                bSetDescription = 1;
                g_pTlkTable->SetCustomToken(0, sNames);
                if (nStatus == GUI_STATUS_INFLUENCE_GAIN)
                    sDescription = g_pAppManager->GetClientExoApp()->GetGUIString(STRREF_INFLUENCE_GAINED);
                else
                    sDescription = g_pAppManager->GetClientExoApp()->GetGUIString(STRREF_INFLUENCE_LOST);
            }
            break;
        }
        case GUI_STATUS_MAX_FP_GAIN:
            bShow = m_cStatusSummary.m_nMaxForcePointsGain;
            break;
        case GUI_STATUS_MAX_FP_LOST:
            bShow = m_cStatusSummary.m_nMaxForcePointsLost;
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
                m_aStatusDescriptions[nStatus].m_cText.m_cParams.ReSetFont();
            }
            cExtent = m_aStatusLabels[nStatus].m_cExtent;
            cExtent.m_nTop = nTop;
            cExtent.m_nHeight = STATUS_ICON_SIZE;
            cExtent.m_nWidth = STATUS_ICON_SIZE;
            pLabel->SetExtent(cExtent);
            cExtent = m_aStatusDescriptions[nStatus].m_cExtent;
            cExtent.m_nTop = nTop - 1;
            if (nStatus != GUI_STATUS_ITEM_RECEIVED)
                cExtent.m_nHeight = STATUS_DESCRIPTION_HEIGHT;
            else
                cExtent.m_nHeight = m_aStatusDescriptions[GUI_STATUS_ITEM_RECEIVED].m_cText.GetIdealHeight();
            cExtent.m_nWidth = nWidth;
            pDescription->SetExtent(cExtent);
            if (nStatus == GUI_STATUS_ITEM_RECEIVED)
            {
                if (m_aStatusDescriptions[GUI_STATUS_ITEM_RECEIVED].m_cText.m_cParams.m_sText != "")
                {
                    int nItemWidth = (int)(fScaleX * 300.0f);
                    if (nItemWidth > nWidth)
                        nWidth = nItemWidth;
                    cExtent.m_nWidth = nWidth;
                    m_aStatusDescriptions[GUI_STATUS_ITEM_RECEIVED].SetExtent(cExtent);
                }
            }
            else if (m_aStatusDescriptions[nStatus].m_cText.m_cParams.m_sText != "" && nWidth < 440)
            {
                do
                {
                    if (m_aStatusDescriptions[nStatus].m_cText.GetIdealHeight() ==
                        m_aStatusDescriptions[nStatus].m_cText.GetFontHeight())
                        break;
                    nWidth += (int)(fScaleX * 20.0f);
                    cExtent.m_nWidth = nWidth;
                    pDescription->SetExtent(cExtent);
                } while (nWidth < 440);
            }
            if (bFlash)
                g_pAppManager->GetClientExoApp()->GetInGameGui()->ShowFlashingStatus(nStatus);
            if (nStatus == GUI_STATUS_ITEM_RECEIVED)
            {
                int nIdealHeight = m_aStatusDescriptions[GUI_STATUS_ITEM_RECEIVED].m_cText.GetIdealHeight();
                if (nIdealHeight < STATUS_ROW_HEIGHT)
                    nTop += STATUS_ROW_HEIGHT;
                else
                    nTop += nIdealHeight;
            }
            else
                nTop += STATUS_ROW_HEIGHT;
        }
        else
        {
            CSWGuiLabel* pLabel = &m_aStatusLabels[nStatus];
            pLabel->m_bEnabled = 0;
            m_aStatusDescriptions[nStatus].m_bEnabled = 0;
        }
    }
    cExtent.m_nHeight = nTop - (int)(fScaleY * -32.0f);
    cExtent.m_nWidth = m_aStatusDescriptions[0].m_cExtent.m_nLeft - (int)(fScaleX * -10.0f) + nWidth;
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
    m_cStatusSummary.m_bDarkAlignSound = 0;
    m_cStatusSummary.m_bLightAlignSound = 0;
    m_cStatusSummary.m_bCreditsNet = 0;
    m_cStatusSummary.m_nCredits = 0;
    m_cStatusSummary.m_nLightShift = 0;
    m_cStatusSummary.m_nDarkShift = 0;
    m_cStatusSummary.m_nExperience = 0;
    m_cStatusSummary.m_nStealthExperience = 0;
    m_cStatusSummary.m_nInfluenceGain = 0;
    m_cStatusSummary.m_nInfluenceLost = 0;
    m_cStatusSummary.m_nMaxForcePointsGain = 0;
    m_cStatusSummary.m_nMaxForcePointsLost = 0;
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
// PC 0x006799B0 GOG 0x00679750
CSWGuiToolTipPanel::CSWGuiToolTipPanel(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    SwguiingameTrace2();
    StartLoadFromLayout("tooltip8X6_p");
    InitControl(m_lblToolTip, "tooltip", 1);
    StopLoadFromLayout();
}
#ifndef _XBOX
// PC 0x00679A70 GOG 0x00679810
void CSWGuiInGameMenu::OnChangeCharacter(CSWGuiControl* pControl)
{
    int nDirection = CHANGE_DIRECTION_NEXT;
    if (pControl->m_bSelected == 0)
        return;
    if (m_n3DCC != 0)
    {
        CSWGuiMessageBox* pMessageBox =
            g_pAppManager->GetClientExoApp()->GetInGameGui()->m_pMessageBox;
        pMessageBox->SetAllowCancel(0);
        pMessageBox->SetCallback(NULL, NULL, NULL);
        pMessageBox->SetMessage(STRREF_PARTY_ORDER_LOCKED);
        m_pManager->AddPanel(pMessageBox, 1, 1);
        return;
    }
    if (pControl == &m_btnChange3)
        nDirection = CHANGE_DIRECTION_SECOND;
    if (g_pAppManager->GetClientExoApp()->ChangeCharacterToNextLivingPartyMember(0, 0, nDirection))
        m_pManager->PlayGuiSound(1);
    g_pAppManager->GetClientExoApp()->GetInGameGui()->UpdateAbilitiesCharacter();
    g_pAppManager->GetClientExoApp()->GetInGameGui()->UpdateInventory();
}
#endif
// PC 0x00679B50 GOG 0x006798F0
void CSWGuiDeathDisplay::OnLoadGamePressed(CSWGuiControl* pControl)
{
    if (m_fFadeTime > 0.0)
        return;
    g_pAppManager->GetClientExoApp()->SetGameOver(0);
    g_pAppManager->m_pClientExoApp->SetEndGamePending(0, 0.0f, 1);
    g_pAppManager->m_pClientExoApp->SaveOptions();
    CSWGuiSaveLoad* pSaveLoad = new CSWGuiSaveLoad(GetManager(), 0, 0, 1);
    pSaveLoad->SetEndGameMode();
    if (pSaveLoad->m_lEntries.num > 0)
    {
        g_pAppManager->m_pClientExoApp->GetInGameGui()->RemoveDeathMessage(1);
        m_pManager->AddPanel(pSaveLoad, CSWGuiManager::PANEL_MODE_MODAL, 1);
    }
    else
    {
        pSaveLoad->m_bDeferredRemoval = 0;
        pSaveLoad->m_nExitState = CSWGuiPanel::EXITSTATE_DELETE;
    }
}
// PC 0x00679C50 GOG 0x00679A00
void CSWGuiDeathDisplay::OnLastSavePressed(CSWGuiControl* pControl)
{
    if (m_fFadeTime > 0.0)
        return;
    g_pAppManager->GetClientExoApp()->SetGameOver(0);
    g_pAppManager->m_pClientExoApp->SetEndGamePending(0, 0.0f, 1);
    g_pAppManager->m_pClientExoApp->SaveOptions();
    CSWGuiSaveLoad* pSaveLoad = new CSWGuiSaveLoad(GetManager(), 0, 0, 1);
    pSaveLoad->SetEndGameMode();
    if (pSaveLoad->m_lEntries.num > 0)
    {
        if (pSaveLoad->LaunchMostRecentSave())
        {
            pSaveLoad->m_bDeferredRemoval = 0;
            pSaveLoad->m_nExitState = CSWGuiPanel::EXITSTATE_DELETE;
            g_pAppManager->m_pClientExoApp->GetInGameGui()->RemoveDeathMessage(0);
        }
        else
        {
            pSaveLoad->m_bDeferredRemoval = 0;
            pSaveLoad->m_nExitState = CSWGuiPanel::EXITSTATE_DELETE;
            CSWGuiMessageBox* pMessageBox =
                g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
            pMessageBox->SetAllowCancel(0);
            pMessageBox->SetCallback(0, 0, 0);
            pMessageBox->SetMessage(CSWGuiSaveLoad::STRREF_NO_SAVED_GAMES);
            m_pManager->AddPanel(pMessageBox, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
        }
    }
    else
    {
        pSaveLoad->m_bDeferredRemoval = 0;
        pSaveLoad->m_nExitState = CSWGuiPanel::EXITSTATE_DELETE;
    }
}
// PC 0x00679F60 GOG 0x00679D30
void CSWGuiInGameMessages::AddMessages(CGuiInGameMessage* pMessages, unsigned long nMessages)
{
    CExoArrayList<CSWGuiControl*> aControls;
    CSWGuiListBox* pList = &m_lbMessages;
    CSWGuiExtent& cItemExtent = pList->m_cItemExtent;
    int nWidth = cItemExtent.m_nWidth - 2 * pList->m_nPadding;
    CSWGuiExtent cExtent;
    cExtent.m_nLeft = 0;
    cExtent.m_nTop = 0;
    cExtent.m_nWidth = nWidth;
    cExtent.m_nHeight = INITIAL_MESSAGE_HEIGHT;
    aControls.SetSize(nMessages);
    for (unsigned long i = 0; i < nMessages; i++)
    {
        CExoString sMessage(pMessages[i].m_sMessage);
        int nToken = sMessage.Find('<', 0);
        int nCustomToken = sMessage.Find('{', 0);
        int bHasToken = nToken != -1 || nCustomToken != -1;
        if (bHasToken)
        {
            g_pSWTlkTable->ParseStr(sMessage,
                (CGameObject*)g_pAppManager->m_pClientExoApp->GetSWParty()->GetPlayerCharacter(),
                0, 1);
            int nLength = sMessage.GetLength();
            while (sMessage[nLength - 1] == ' ')
            {
                sMessage = sMessage.SubString(0, sMessage.GetLength() - 1);
                nLength = sMessage.GetLength();
                if (nLength < 1)
                    break;
            }
        }
        m_pMessageLabels[i].m_cText.m_cParams.SetText(sMessage);
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
            vColor = CGuiInGame::COLOR_NEW_TEXT;
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
// PC 0x0067A280 GOG 0x0067A050
void CSWGuiInGameMessages::AddDialogMessages(CGuiInGameDialogMessage* pMessages,
                                            unsigned long nMessages)
{
    CExoArrayList<CSWGuiControl*> lControls;
    CSWGuiListBox* pList = &m_lbDialog;
    CSWGuiExtent cExtent(0, 0,
        pList->m_cItemExtent.m_nWidth - 2 * pList->m_nPadding, INITIAL_MESSAGE_HEIGHT);
    CExoString sSpeaker;
    lControls.SetSize(nMessages);
    for (unsigned long i = 0; i < nMessages; i++)
    {
        sSpeaker = pMessages[i].m_sSpeaker;
        int nToken = sSpeaker.Find('<', 0);
        int nCustomToken = sSpeaker.Find('{', 0);
        if (nToken != -1 || nCustomToken != -1)
        {
            g_pSWTlkTable->ParseStr(sSpeaker,
                (CGameObject*)g_pAppManager->m_pClientExoApp->GetSWParty()->GetPlayerCharacter(),
                0, 1);
            int nLength = sSpeaker.GetLength();
            while (sSpeaker[nLength - 1] == ' ')
            {
                sSpeaker = sSpeaker.SubString(0, sSpeaker.GetLength() - 1);
                nLength = sSpeaker.GetLength();
                if (nLength < 1)
                    break;
            }
        }
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
// PC 0x0067A630 GOG 0x0067A400
void CSWGuiInGameMessages::AddCombatMessages(CGuiInGameMessage* pMessages, unsigned long nMessages)
{
    CExoArrayList<CSWGuiControl*> aControls;
    CSWGuiListBox* pList = &m_lbCombat;
    int nWidth = pList->m_cItemExtent.m_nWidth - 2 * pList->m_nPadding;
    CSWGuiExtent cExtent;
    cExtent.m_nLeft = 0;
    cExtent.m_nTop = 0;
    cExtent.m_nWidth = nWidth;
    cExtent.m_nHeight = INITIAL_MESSAGE_HEIGHT;
    aControls.SetSize(nMessages);
    for (unsigned long i = 0; i < nMessages; i++)
    {
        CExoString sMessage(pMessages[i].m_sMessage);
        int nToken = sMessage.Find('<', 0);
        int nCustomToken = sMessage.Find('{', 0);
        if (nToken != -1 || nCustomToken != -1)
        {
            g_pSWTlkTable->ParseStr(sMessage,
                (CGameObject*)g_pAppManager->m_pClientExoApp->GetSWParty()->GetPlayerCharacter(),
                0, 1);
            int nLength = sMessage.GetLength();
            while (sMessage[nLength - 1] == ' ')
            {
                sMessage = sMessage.SubString(0, sMessage.GetLength() - 1);
                nLength = sMessage.GetLength();
                if (nLength < 1)
                    break;
            }
        }
        m_pCombatLabels[i].SetExtent(cExtent);
        m_pCombatLabels[i].m_cText.m_cParams.SetText(sMessage);
#ifndef _XBOX
        m_pCombatLabels[i].m_cText.m_cParams.ReSetFont();
#endif
        Vector vColor;
        switch (pMessages[i].m_nColor)
        {
        case CGuiInGameMessage::COLOR_RED:
            vColor = CGuiInGame::COLOR_RED;
            break;
        default:
            vColor = CGuiInGame::COLOR_NEW_TEXT;
            break;
        }
        m_pCombatLabels[i].m_cText.m_cParams.SetColor(vColor);
        m_pCombatLabels[i].SetExtent(cExtent);
        CSWGuiText* pText = &m_pCombatLabels[i].m_cText;
        cExtent.m_nHeight = pText->GetIdealHeight() +
            2 * m_pCombatLabels[i].m_cHilite.m_cParams.GetBorderDim();
        m_pCombatLabels[i].SetExtent(cExtent);
        m_pCombatLabels[i].SetActive(0);
        aControls[i] = &m_pCombatLabels[i];
    }
    if (nMessages)
    {
        pList->AddControls(aControls, 0, 0, 1);
        pList->SetSelectedControl(nMessages - 1, 0);
    }
}
// PC 0x0067C060 GOG 0x0067BE30
void CSWGuiInGameMessages::OnPanelAdded()
{
#ifndef _XBOX
    CSWGuiPanel::OnPanelAdded();
#endif
    g_pAppManager->m_pClientExoApp->GetInGameGui()->UpdateMessageGUI(this);
    int nMessageType = g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nMessageType;
    if (nMessageType == CGuiInGame::MESSAGE_TYPE_DIALOG)
        ShowDialogMessages();
    else if (nMessageType == CGuiInGame::MESSAGE_TYPE_FEEDBACK)
        ShowFeedbackMessages();
    else if (nMessageType == CGuiInGame::MESSAGE_TYPE_COMBAT)
        ShowCombatMessages();
    else if (nMessageType == CGuiInGame::MESSAGE_TYPE_EFFECTS)
        ShowEffects();


    g_pAppManager->m_pClientExoApp->ShowTutorialWindow(0xC, OBJECT_INVALID,
                                                       OBJECT_INVALID, 0);
}
// PC 0x0067C0F0 GOG 0x0067BEC0
void CSWGuiInGameMessages::ChangeMessageDisplay(CSWGuiControl* pControl)
{
    int nIndex = pControl->m_nIndex;
    if (nIndex == MESSAGE_DISPLAY_DIALOG)
        ShowDialogMessages();
    else if (nIndex == MESSAGE_DISPLAY_FEEDBACK)
        ShowFeedbackMessages();
    else if (nIndex == MESSAGE_DISPLAY_COMBAT)
        ShowCombatMessages();
    else if (nIndex == MESSAGE_DISPLAY_EFFECTS)
        ShowEffects();
}

// PC 0x0067C130 GOG 0x0067BF00
CSWGuiInGameMessages::CSWGuiInGameMessages(CSWGuiManager* pManager)

    : CSWGuiPanel(pManager)
{
    CSWGuiExtent cExtent;
    cExtent.m_nLeft = cExtent.m_nTop =
        cExtent.m_nWidth = cExtent.m_nHeight = 0;

    StartLoadFromLayout("messages_p");
    CSWGuiListBox* pMessages = &m_lbMessages;
    InitControl(*pMessages, "LB_MESSAGES", 1);
    InitControl(m_lblTitle, "LBL_MESSAGES", 1);
    InitControl(m_lbDialog, "LB_DIALOG", 1);
    InitControl(m_lbCombat, "LB_COMBAT", 1);
    InitControl(m_lbEffectsGood, "LB_EFFECTS_GOOD", 1);
    InitControl(m_lbEffectsBad, "LB_EFFECTS_BAD", 1);
    InitControl(m_lblEffectsGood, "LBL_EFFECTS_GOOD", 1);
    InitControl(m_lblEffectsBad, "LBL_EFFECTS_BAD", 1);
    InitControl(m_btnBack, "BTN_EXIT", 1);
    InitControl(m_alblBar[0], "LBL_BAR1", 1);
    InitControl(m_alblBar[1], "LBL_BAR2", 1);
    InitControl(m_alblBar[2], "LBL_BAR3", 1);
    InitControl(m_alblBar[3], "LBL_BAR4", 1);
    InitControl(m_alblBar[4], "LBL_BAR5", 1);
    InitControl(m_alblBar[5], "LBL_BAR6", 1);
    InitControl(m_btnDialog, "BTN_DIALOG", 1);
    InitControl(m_btnFeedback, "BTN_FEEDBACK", 1);
    InitControl(m_btnCombat, "BTN_COMBAT", 1);
    InitControl(m_btnEffects, "BTN_EFFECTS", 1);
    InitControl(m_lblFilter, "LBL_FILTER", 1);
    StopLoadFromLayout();

    CSWGuiLabelHilight* pProto =
        (CSWGuiLabelHilight*)m_lbMessages.m_pProtoItem;
    pProto->m_cHilite.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_HIGHLIGHT;
    pProto->m_cHilite.m_cParams.SetPulsingAlpha(1, 1, 0);
    m_pMessageLabels = new CSWGuiLabelHilight[64];
    for (unsigned int i = 0; i < 64; i++)
    {
        m_pMessageLabels[i].Initialize(cExtent, pProto);
    }

    pProto = (CSWGuiLabelHilight*)m_lbDialog.m_pProtoItem;
    pProto->m_cHilite.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_HIGHLIGHT;
    pProto->m_cHilite.m_cParams.SetPulsingAlpha(1, 1, 0);
    m_pDialogLabels = new CSWGuiLabelHilight[64];
    for (unsigned int j = 0; j < 64; j++)
    {
        m_pDialogLabels[j].Initialize(cExtent, pProto);
    }

    pProto->m_cHilite.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_HIGHLIGHT;
    pProto->m_cHilite.m_cParams.SetPulsingAlpha(1, 1, 0);
    m_pCombatLabels = new CSWGuiLabelHilight[64];
    for (unsigned int k = 0; k < 64; k++)
    {
        m_pCombatLabels[k].Initialize(cExtent, pProto);
    }

    CSWGuiButton* pExit = &m_btnBack;
    pExit->AddEvent(CSWGuiControl::EVENT_SELECT, this,
                    (PFNGUIEVENT)&CSWGuiPanel::OnBButtonPressed);
    pExit->m_bFocusable = 0;
    m_btnDialog.AddEvent(CSWGuiControl::EVENT_SELECT, this,
                         (PFNGUIEVENT)&CSWGuiInGameMessages::ChangeMessageDisplay);
    m_btnDialog.m_nIndex = MESSAGE_DISPLAY_DIALOG;
    m_btnDialog.SetFocusable(0);
    m_btnFeedback.AddEvent(CSWGuiControl::EVENT_SELECT, this,
                           (PFNGUIEVENT)&CSWGuiInGameMessages::ChangeMessageDisplay);
    m_btnFeedback.m_nIndex = MESSAGE_DISPLAY_FEEDBACK;
    m_btnFeedback.SetFocusable(0);
    m_btnCombat.AddEvent(CSWGuiControl::EVENT_SELECT, this,
                         (PFNGUIEVENT)&CSWGuiInGameMessages::ChangeMessageDisplay);
    m_btnCombat.m_nIndex = MESSAGE_DISPLAY_COMBAT;
    m_btnCombat.SetFocusable(0);
    m_btnEffects.AddEvent(CSWGuiControl::EVENT_SELECT, this,
                          (PFNGUIEVENT)&CSWGuiInGameMessages::ChangeMessageDisplay);
    m_btnEffects.m_nIndex = MESSAGE_DISPLAY_EFFECTS;
    m_btnEffects.SetFocusable(0);
    pMessages->SetSelectOnLeftClick(1);
    m_lbDialog.SetSelectOnLeftClick(1);
    m_lbCombat.SetSelectOnLeftClick(1);
    SetActiveControl(pMessages, 0);
}
// PC 0x0067CA30 GOG 0x0067C800
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
#ifndef _XBOX
            HandleAlignmentMovie();
            HandleLevelUpDLG();
            g_pAppManager->m_pClientExoApp->GetInGameGui()->m_a1030.num = 0;
#endif
            if (!m_bWasPaused)
                g_pAppManager->m_pClientExoApp->SetPausedByCombat(0, 0, 0);
            break;
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
