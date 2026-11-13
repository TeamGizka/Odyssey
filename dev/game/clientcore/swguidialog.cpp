#include "swguidialog.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "clientoptions.h"
#include "exobase.h"
#include "guiingame.h"
#include "nwrules.h"
#include "nwskill.h"
#include "nwccreature.h"
#include "nwccreaturestats.h"
#include "nwsitem.h"
#include "swparty.h"
#include "serverexoapp.h"
#include "swguiingame.h"
#include "swgui.h"
#include "twodimarray.h"
#include "clientexoappinternal.h"

extern int g_Letterbox_top;
extern int g_Letterbox_bottom;

static inline void SwguidialogTrace2();
static inline void SwguidialogTrace1();
#define SWGUIDIALOG_TRACE() SwguidialogTrace1()

#include "nwrules.h"

#include "nwskill.h"
// PC 0x007088F0 GOG 0x00708630
void CSWGuiDialogComputer::OnPanelAdded()
{
    m_pSound->Play();
#ifndef _XBOX
    CSWGuiPanel::OnPanelAdded();
#endif
}
// PC 0x0070A8A0 GOG 0x0070A5E0
void CSWGuiDialogComputer::OnPanelRemoved()
{
    m_pSound->Stop();
#ifndef _XBOX
    CSWGuiPanel::OnPanelRemoved();
#endif
}

static void SwguidialogTrace3(CSWGuiDialog* pDialog)
{
    if (0)
    {
        pDialog->CSWGuiDialog::~CSWGuiDialog();
    }
}

// PC 0x005EE1C0 GOG 0x005EE1A0
void CSWGuiDialogComputer::SetType(unsigned char)
{
}

static inline void SwguidialogTrace2()
{
    SwguidialogTrace1();
}

static inline void SwguidialogTrace1()
{
}
// PC 0x0070E2A0 GOG 0x0070E900
CSWGuiDialog::CSWGuiDialog(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    m_nOwner = OBJECT_INVALID;
    m_bDialogActive = 1;
    m_bDialogWaiting = 0;
    m_bDialogEnded = 1;
    Reset();
}

void CSWGuiDialog::SetReplyActive(CSWGuiControl* pControl)
{
    SWGUIDIALOG_TRACE();
    EXOXBOXASSERT(pControl->AsLabel());
    CSWGuiLabel* pLabel = pControl->AsLabel();
    pLabel->m_cText.m_cParams.SetColor(m_vReplyHighlightColor);
}
// PC 0x0070FCD0 GOG 0x0070FA20
CSWGuiDialogComputerCamera::CSWGuiDialogComputerCamera(CSWGuiManager* pManager)
    : CSWGuiDialog(pManager)
{
    m_pTop = 0;
#ifdef _XBOX
    StartLoadFromLayout("computercamera");
    InitControl(m_lblCameraTitle, "LBL_A", 1);
    InitControl(m_lblReturn, "LBL_RETURN", 1);
    StopLoadFromLayout();
#else
    {
        CResRef cLayout("computercam_p");
        StartLoadFromLayout(cLayout);
    }

    CSWGuiExtent cExtent = m_cExtent;
    cExtent.m_nHeight = g_nScreenHeight;
    cExtent.m_nWidth = g_nScreenWidth;
    CSWGuiPanel::SetExtent(cExtent);

    InitControl(m_lblReturn, "LBL_RETURN", 1);
    cExtent = m_lblReturn.m_cExtent;
    cExtent.m_nLeft = (g_nScreenWidth - cExtent.m_nWidth) / 2;
    cExtent.m_nTop = g_nScreenHeight - 75;
    m_lblReturn.SetExtent(cExtent);

    StopLoadFromLayout();
#endif
}
// PC 0x0070D440 GOG 0x0070D430
void CSWGuiDialog::SetReplyInactive(CSWGuiControl* pControl)
{
    SWGUIDIALOG_TRACE();
    EXOXBOXASSERT(pControl->AsLabel());
    CSWGuiLabel* pLabel = pControl->AsLabel();
    pLabel->m_cText.m_cParams.SetColor(m_vReplyColor);
}
// PC 0x0070D4A0 GOG 0x0070D490
void CSWGuiDialog::SetDialogMessage(const CExoString& sMessage)
{
    SWGUIDIALOG_TRACE();
    m_lbReplies.SetEnabledFlag(0);
    m_lblMessage.SetEnabledFlag(0);
    m_sMessage = sMessage;
    if (m_bDialogEnded != 0 &&
        ((CGuiInGame*)g_pAppManager->m_pClientExoApp->GetInGameGui())
                ->IsCameraDialog() != 0)
    {
        m_bDialogEnded = 0;
        m_bDialogWaiting = 1;
    }
    else
    {
        ShowDialogMessage();
    }
    m_bDialogActive = 1;
}
// PC 0x0070D530 GOG 0x0070D520
void CSWGuiDialog::ShowDialogMessage()
{
    SWGUIDIALOG_TRACE();
    m_bDialogWaiting = 0;
    if (g_pAppManager->m_pClientExoApp->GetClientOptions()->m_bSubtitles ||
        g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nDialogScrollWait !=
            0)
    {
        m_lblMessage.m_cText.m_cParams.SetText(m_sMessage);
        g_pAppManager->m_pClientExoApp->GetInGameGui()->m_nDialogScrollWait = 0;
    }
    m_lblMessage.SetEnabledFlag(1);
}
// PC 0x0070D5A0 GOG 0x0070D190
void CSWGuiDialog::EndDialog()
{
    m_bDialogEnded = 1;
}
// PC 0x0070D5B0 GOG 0x0070D590
int CSWGuiDialog::SelectReply(int nReply)
{
    SWGUIDIALOG_TRACE();
    if (nReply < m_nReplyCount)
    {
        CClientExoApp* pApp = g_pAppManager->m_pClientExoApp;
        pApp->GetInGameGui()->SetDialogSelection(nReply, m_nReplyCount, 1);
        g_pAppManager->m_pClientExoApp->GetInGameGui()->UpdateDialog();
        return 1;
    }
    return 0;
}
// PC 0x0070D600 GOG 0x0070D5E0
void CSWGuiDialog::Reset()
{
    SWGUIDIALOG_TRACE();
    m_lbReplies.SetEnabledFlag(0);
    m_lblMessage.SetEnabledFlag(0);
    m_nSelectedReply = -1;
    m_nReplyCount = 0;
    m_sMessage = "";
    for (int i = 0; i < MAX_REPLIES; i++)
    {
        m_lblReply[i].m_cText.m_cParams.SetColor(m_vReplyColor);
        CExoString sText("");
        m_lblReply[i].m_cText.m_cParams.SetText(sText);
    }
}
// PC 0x0070D6D0 GOG 0x0070D1A0
int CSWGuiDialog::GetReplyIndex(int nReply)
{
    if (nReply >= m_nReplyCount || nReply < 0)
    {
        return -1;
    }
    return m_aReplyIndex[nReply];
}
// PC 0x0070D6F0 GOG 0x0070D6B0
void CSWGuiDialog::HandleInputEvent(int nEvent, int nState)
{
    SWGUIDIALOG_TRACE();
    if (nState)
    {
        switch (nEvent)
        {
        case GUIEVENT_ACTIVATE:
        case GUIEVENT_ACCEPT:
            if (m_bDialogActive)
            {
                if (!m_bDialogWaiting)
                {
                    int nOwner = m_nOwner;
                    g_pAppManager->m_pServerExoApp->SkipPastDialog(
                        g_pAppManager->m_pServerExoApp->ClientToServerObjectId(nOwner));
                }
            }
            else
            {
                m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
#ifdef _XBOX
                SelectReply(m_nSelectedReply);
#else
                SelectReply(m_lbReplies.m_nSelectedItem);
#endif
            }
            break;
#ifndef _XBOX
        case GUIEVENT_DIALOG_SKIP:
            if (m_bDialogActive && !m_bDialogWaiting)
            {
                int nOwner2 = m_nOwner;
                g_pAppManager->m_pServerExoApp->SkipPastDialog(
                    g_pAppManager->m_pServerExoApp->ClientToServerObjectId(nOwner2));
            }
            break;
#endif
        case GUIEVENT_UP:
        case GUIEVENT_UP_ALTERNATE:
            if (m_nSelectedReply > 0)
            {
                --m_nSelectedReply;
            }
            break;
        case GUIEVENT_DOWN:
        case GUIEVENT_DOWN_ALTERNATE:
            if (m_nSelectedReply < m_nReplyCount - 1)
            {
                ++m_nSelectedReply;
            }
            break;
#ifndef _XBOX
        case GUIEVENT_REPLY_1:
        case GUIEVENT_REPLY_2:
        case GUIEVENT_REPLY_3:
        case GUIEVENT_REPLY_4:
        case GUIEVENT_REPLY_5:
        case GUIEVENT_REPLY_6:
        case GUIEVENT_REPLY_7:
        case GUIEVENT_REPLY_8:
        case GUIEVENT_REPLY_9:
            {
                int nReply = nEvent - GUIEVENT_REPLY_1;
                if (nReply < m_nReplyCount)
                {
                    m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
                    SelectReply(nReply);
                }
            }
            break;
#endif
        }
#ifdef _XBOX
        if (!m_bDialogActive || nEvent == CSWGuiControl::EVENT_ENTER ||
            nEvent == CSWGuiControl::EVENT_EXIT)
#else
        if (nEvent != GUIEVENT_ACTIVATE && nEvent != GUIEVENT_ACCEPT &&
            (!m_bDialogActive || nEvent == CSWGuiControl::EVENT_ENTER ||
             nEvent == CSWGuiControl::EVENT_EXIT))
#endif
        {
            CSWGuiPanel::HandleInputEvent(nEvent, nState);
        }
    }
}
// PC 0x0070D850 GOG 0x0070D810
int CSWGuiDialog::HandleSelectReply(CSWGuiControl* pControl)
{
    SWGUIDIALOG_TRACE();
    m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
    return SelectReply(m_lbReplies.m_nSelectedItem);
}
// PC 0x0070D870 GOG 0x0070D830
void CSWGuiDialogLetterbox::SetExtentSpecial(CSWGuiExtent cExtent)
{
    SWGUIDIALOG_TRACE();
    switch (m_nPosition)
    {
    case POSITION_TOP:
        cExtent.m_nTop = 0;
        g_Letterbox_top = cExtent.m_nHeight;
        break;
    case POSITION_BOTTOM:
        cExtent.m_nHeight = m_pManager->m_nHeight - cExtent.m_nTop;
        g_Letterbox_bottom = cExtent.m_nTop;
        break;
    }
    if (m_bDone)
    {
        static_cast<CSWGuiExtent&>(m_cTarget) = cExtent;
        m_cExtent = cExtent;
    }
    else
    {
        switch (m_nPosition)
        {
        case POSITION_TOP:
            if (cExtent.m_nHeight > m_cTarget.m_nHeight)
            {
                m_cTarget.m_nHeight = cExtent.m_nHeight;
            }
            break;
        case POSITION_BOTTOM:
            if (cExtent.m_nTop < m_cTarget.m_nTop)
            {
                int nDifference = m_cTarget.m_nTop - cExtent.m_nTop;
                m_cTarget.m_nTop -= nDifference;
                m_cTarget.m_nHeight += nDifference;
            }
            break;
        }
    }
}
// PC 0x0070D910 GOG 0x0070D8D0
void CSWGuiDialogLetterbox::SetExtent(const CSWGuiExtent& cExtent)
{
    SWGUIDIALOG_TRACE();
    CSWGuiExtent cNewExtent = cExtent;
    switch (m_nPosition)
    {
    case POSITION_TOP:
        cNewExtent.m_nTop = 0;
        break;
    case POSITION_BOTTOM:
        cNewExtent.m_nHeight = m_pManager->m_nHeight - cNewExtent.m_nTop;
        break;
    }
    CSWGuiPanel::SetExtent(cNewExtent);
}
// PC 0x0070D980 GOG 0x0070D940
void CSWGuiDialogLetterbox::SetTop()
{
    SWGUIDIALOG_TRACE();
    if (m_pManager == 0)
    {
        m_pManager = g_pAppManager->m_pClientExoApp->GetSWGuiManager();
    }
    CSWGuiManager* pManager = m_pManager;
#ifdef _XBOX
    int nBarHeight =
        (int)((int)(pManager->m_nHeight - (int)(pManager->m_nWidth * 0.42857149f)) * 0.5f);
#else
    unsigned long nBarHeight =
        (unsigned long)((int)(pManager->m_nHeight -
                              (unsigned long)(pManager->m_nWidth *
                                              0.42857149f)) *
                        0.5f);
#endif
    m_cStart.m_nHeight = 0;
    m_cTarget.m_nTop = 0;
    m_cTarget.m_nLeft = 0;
    m_cTarget.m_nWidth = pManager->m_nWidth;
    m_cExtent.m_nTop = 0;
    m_cExtent.m_nLeft = 0;
    m_cTarget.m_nHeight = nBarHeight;
    m_cExtent.m_nWidth = pManager->m_nWidth;
    m_cExtent.m_nHeight = 0;
    m_nPosition = POSITION_TOP;
    g_Letterbox_top = 0;
}
// PC 0x0070DA10 GOG 0x0070D9D0
void CSWGuiDialogLetterbox::SetBottom()
{
    SWGUIDIALOG_TRACE();
    if (m_pManager == 0)
    {
        m_pManager = g_pAppManager->m_pClientExoApp->GetSWGuiManager();
    }
    CSWGuiManager* pManager = m_pManager;
    int nScreenHeight = pManager->m_nHeight;
#ifdef _XBOX
    int nBarHeight =
        (int)((int)(nScreenHeight - (int)(pManager->m_nWidth * 0.42857149f)) * 0.5f);
#else
    unsigned long nBarHeight =
        (unsigned long)((int)(nScreenHeight -
                              (unsigned long)(pManager->m_nWidth *
                                              0.42857149f)) *
                        0.5f);
#endif
    m_cStart.m_nTop = nScreenHeight;
    m_cTarget.m_nTop = pManager->m_nHeight - nBarHeight;
    m_cTarget.m_nLeft = 0;
    m_cTarget.m_nWidth = pManager->m_nWidth;
    m_cTarget.m_nHeight = nBarHeight;
    m_cExtent.m_nTop = pManager->m_nHeight;
    m_cExtent.m_nLeft = 0;
    m_cExtent.m_nWidth = pManager->m_nWidth;
    m_cExtent.m_nHeight = nBarHeight;
    g_Letterbox_bottom = m_cExtent.m_nTop;
    m_nPosition = POSITION_BOTTOM;
}
// PC 0x0070DAF0 GOG 0x0070DAB0
void CSWGuiDialogLetterbox::SetFade(int bFadeIn, float fDelay, float fLength,
                                    const Vector& vColor)
{
    SWGUIDIALOG_TRACE();
    m_vColor = vColor;
    if (bFadeIn != 0)
    {
        SetAlpha(1.0f);
    }
    else
    {
        SetAlpha(0.0f);
    }
    m_bFadeIn = bFadeIn;
    m_fFadeDelay = fDelay;
    m_fFadeLength = fLength;
    m_fFadeTime = 0.1f;
}
// PC 0x0070DB50 GOG 0x0070D1C0
int CSWGuiDialogLetterbox::IsFadeDone()
{
    if (m_fFadeTime == 0.0f)
    {
        return 1;
    }
    return m_fFadeDelay + m_fFadeLength < m_fFadeTime;
}
// PC 0x0070DB90 GOG 0x0070DF60
void CSWGuiDialogTop::SetReply(const CExoString& sReply)
{
    SWGUIDIALOG_TRACE();
    if (g_pAppManager->m_pClientExoApp->GetInGameGui()
            ->m_pDialogTop->m_bDone)
    {
        int nMarginX;
        int nMarginY;
        m_pManager->GetSafeMargin(nMarginX, nMarginY);
        m_lblReply.m_cText.m_cParams.SetText(sReply);
        int nHeight = m_lblReply.m_cText.GetIdealHeight();
        CSWGuiExtent cExtent = m_lblReply.m_cExtent;
        if (nHeight > m_cExtent.m_nHeight)
        {
            m_cExtent.m_nHeight = nHeight;
            cExtent.m_nHeight = nHeight;
        }
        else
        {
            m_cExtent.m_nHeight =
                (m_pManager->m_nHeight -
                 (int)(m_pManager->m_nWidth * 0.42857149f)) /
                    2 -
                nMarginY - 5;
            cExtent.m_nHeight = m_cExtent.m_nHeight;
        }
        m_lblReply.SetExtent(cExtent);
        cExtent = m_cExtent;
        cExtent.m_nLeft = 0;
        cExtent.m_nTop = 0;
        cExtent.m_nWidth = m_pManager->m_nWidth;
        cExtent.m_nHeight += nMarginY;
        CGuiInGame* pGui = g_pAppManager->m_pClientExoApp->GetInGameGui();
        pGui->m_pDialogTop->SetExtentSpecial(cExtent);
    }
}
// PC 0x0070DCD0 GOG 0x0070E120
void CSWGuiDialogCinematic::SetExtent(const CSWGuiExtent& cExtent)
{
    SWGUIDIALOG_TRACE();
    CSWGuiPanel::SetExtent(cExtent);
    CSWGuiExtent cChildExtent = m_lbReplies.m_cExtent;
    cChildExtent.m_nWidth = m_cExtent.m_nWidth;
    m_lbReplies.SetExtent(cChildExtent);
    cChildExtent = m_lblMessage.m_cExtent;
    cChildExtent.m_nWidth = m_cExtent.m_nWidth;
    m_lblMessage.SetExtent(cChildExtent);
}
// PC 0x0070DD60 GOG 0x0070E1B0
void CSWGuiDialogCinematic::ShowDialogMessage()
{
    SWGUIDIALOG_TRACE();
    CGuiInGame* pGui;

    CSWGuiDialog::ShowDialogMessage();
    CSWGuiExtent cExtent = m_lblMessage.m_cExtent;
    int nGrow = m_lblMessage.m_cText.GetIdealHeight() - cExtent.m_nHeight;
    if (nGrow > 0)
    {
        m_cExtent.m_nTop -= nGrow;
        m_cExtent.m_nHeight += nGrow;
        cExtent.m_nHeight += nGrow;
    }
    m_lblMessage.SetExtent(cExtent);
    cExtent = m_cExtent;
    cExtent.m_nLeft = 0;
    cExtent.m_nWidth = m_pManager->m_nWidth;
    cExtent.m_nHeight = m_pManager->m_nHeight - cExtent.m_nTop;
    pGui = g_pAppManager->m_pClientExoApp->GetInGameGui();
    pGui->m_pDialogLetterbox->SetExtentSpecial(cExtent);
}
// PC 0x0070DE50 GOG 0x0070E2A0
int CSWGuiDialogCinematic::SelectReply(int nReply)
{
    if (CSWGuiDialog::SelectReply(nReply))
    {
        m_pTop->SetReply("");
        if (m_pManager->PanelExists(m_pTop))
        {
            GetManager()->RemovePanel(m_pTop);
        }
    }
    return 1;
}
// PC 0x0070DF20 GOG 0x0070E370
void CSWGuiDialogCinematic::Reset()
{
    SWGUIDIALOG_TRACE();
    CSWGuiExtent cExtent;
    cExtent.m_nHeight = 0;
    cExtent.m_nWidth = 0;
    cExtent.m_nTop = 0;
    cExtent.m_nLeft = 0;
    int nMarginX;
    int nMarginY;
    m_pManager->GetSafeMargin(nMarginX, nMarginY);
    cExtent.m_nLeft = nMarginX;
    int nHeight = m_pManager->m_nHeight;
    cExtent.m_nTop =
        nHeight -
        (nHeight - (int)(m_pManager->m_nWidth * 0.42857149f)) / 2;
    cExtent.m_nWidth = m_pManager->m_nWidth - nMarginX * 2;
#ifdef _XBOX
    cExtent.m_nHeight = 66;
    int nShrink = m_cExtent.m_nHeight - 66;
#else
    cExtent.m_nHeight = 100;
    int nShrink = m_cExtent.m_nHeight - 100;
#endif
    SetExtent(cExtent);
    cExtent.m_nLeft = 0;
    cExtent.m_nWidth = m_pManager->m_nWidth;
    cExtent.m_nHeight = m_pManager->m_nHeight - cExtent.m_nTop;
    CGuiInGame* pGui = g_pAppManager->m_pClientExoApp->GetInGameGui();
    pGui->m_pDialogLetterbox->SetExtent(cExtent);
    cExtent = m_lblMessage.m_cExtent;
    cExtent.m_nHeight -= nShrink;
    m_lblMessage.SetExtent(cExtent);
    {
        CExoString sEmpty("");
        m_lblMessage.m_cText.m_cParams.SetText(sEmpty);
    }
    CSWGuiDialog::Reset();
}
// PC 0x0070E080 GOG 0x0070E630
void CSWGuiDialogComputer::HandleInputEvent(int nEvent, int nState)
{
    SwguidialogTrace1();
    if (nState != 0)
    {
        switch (nEvent)
        {
        case 57:
            m_lbMessage.HandleInputEvent(GUIEVENT_UP, 1);
            break;
        case 58:
            m_lbMessage.HandleInputEvent(GUIEVENT_DOWN, 1);
            break;
        }
    }
    return CSWGuiDialog::HandleInputEvent(nEvent, nState);
}
// PC 0x0070E120 GOG 0x0070D240
CExoString CSWGuiDialogComputer::GetFullScreenBG()
{
    CExoString sBackground("800x600");
    if (g_nScreenWidth == 1024)
    {
        if (g_nScreenHeight == 768)
            sBackground = "1024x768";
    }
    else if (g_nScreenWidth == 1280)
    {
        if (g_nScreenHeight == 960 || g_nScreenHeight == 1024)
            sBackground = "1280x960";
    }
    else if (g_nScreenWidth == 1600 && g_nScreenHeight == 1200)
    {
        sBackground = "1600x1200";
    }
#ifndef _XBOX
    sBackground = sBackground +
        CSWGuiManager::FULLSCREEN_BGS[PC_BACKGROUND_SUFFIX_COMPUTER] +
        CExoString(m_nBackground);
#endif
    return sBackground;
}
#ifndef _XBOX
// PC 0x0070E270 GOG 0x0070E8E0
void CSWGuiDialogComputerCamera::SetLiveFeedMessageVisible(int bVisible)
{
    m_lblReturn.SetEnabledFlag(bVisible);
}
#endif
// PC 0x0070E420 GOG 0x0070EC70
CSWGuiDialogLetterbox::CSWGuiDialogLetterbox(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    m_vColor = CGuiInGame::COLOR_BLACK;
    ResetFade();
    m_nPosition = 0;
    m_bDone = 0;
}
// PC 0x0070E560 GOG 0x0070EDB0
CSWGuiManager* CSWGuiDialogLetterbox::SetMiddle()
{
    if (m_pManager == 0)
    {
        m_pManager = g_pAppManager->m_pClientExoApp->GetSWGuiManager();
    }
    m_nPosition = POSITION_MIDDLE;
    ResetFade();
    CSWGuiManager* pManager = m_pManager;
    m_cExtent.m_nTop = 0;
    m_cExtent.m_nLeft = 0;
    m_cExtent.m_nWidth = pManager->m_nWidth;
    m_cExtent.m_nHeight = pManager->m_nHeight;
    return pManager;
}
// PC 0x0070DAB0 GOG 0x0070DA70
void CSWGuiDialogLetterbox::ResetFade()
{
    SWGUIDIALOG_TRACE();
    if (m_nPosition == POSITION_MIDDLE)
    {
        SetAlpha(0.0f);
    }
    else
    {
        SetAlpha(1.0f);
    }
    m_fFadeDelay = 0;
    m_fFadeLength = 0;
    m_fFadeTime = 0.0f;
    m_bFadeIn = 0;
}
// PC 0x0070E5D0 GOG 0x0070DB10
void CSWGuiDialogLetterbox::Draw(float fElapsed)
{
    SWGUIDIALOG_TRACE();
    if (m_nPosition == POSITION_MIDDLE && m_fFadeTime > 0.0f)
    {
        if (fElapsed < 5.0f)
        {
            m_fFadeTime += fElapsed;
        }
        if (m_fFadeTime > m_fFadeDelay)
        {
            float fAlpha;
#ifdef _XBOX
            float fFraction = (m_fFadeTime - m_fFadeDelay) / m_fFadeLength;
            if (m_bFadeIn)
            {
                fAlpha = 1.0f - fFraction;
                if (0.0f > fAlpha)
                {
                    fAlpha = 0.0f;
                }
            }
            else if (1.0f < fFraction)
            {
                fAlpha = 1.0f;
            }
            else
            {
                fAlpha = fFraction;
            }
#else
            if (m_bFadeIn)
            {
                fAlpha = max(0.0f, 1.0f - (m_fFadeTime - m_fFadeDelay) / m_fFadeLength);
            }
            else
            {
                fAlpha = min(1.0f, (m_fFadeTime - m_fFadeDelay) / m_fFadeLength);
            }
#endif
            SetAlpha(fAlpha);
        }
    }
    if (!m_bDone)
    {
        if (fElapsed > 0.5f)
        {
            fElapsed = 0.5f;
        }
        if (m_nPosition == POSITION_TOP)
        {
            CSWGuiExtent cExtent = m_cTarget;
            if (m_cExtent.m_nHeight < m_cTarget.m_nHeight)
            {
                float fScrollRate = (m_cTarget.m_nHeight - m_cStart.m_nHeight) * 2.0f;
                int nScroll = (int)(fScrollRate * fElapsed);
                cExtent.m_nHeight = m_cExtent.m_nHeight + nScroll;
            }
            else
            {
                m_bDone = 1;
            }
            SetExtent(cExtent);
            g_Letterbox_top = m_cExtent.m_nHeight;
        }
        else if (m_nPosition == POSITION_BOTTOM)
        {
            CSWGuiExtent cExtent = m_cTarget;
            if (m_cExtent.m_nTop > m_cTarget.m_nTop)
            {
                float fScrollRate = (m_cTarget.m_nTop - m_cStart.m_nTop) * 2.0f;
                int nScroll = (int)(fScrollRate * fElapsed);
                cExtent.m_nTop = m_cExtent.m_nTop + nScroll;
                cExtent.m_nHeight = m_cExtent.m_nHeight - nScroll;
            }
            else
            {
                m_bDone = 1;
                g_pAppManager->m_pClientExoApp->GetInGameGui()->DialogGuiDoneScroll();
                cExtent = m_cTarget;
            }
            SetExtent(cExtent);
            g_Letterbox_bottom = m_cExtent.m_nTop;
        }
    }
    CSWGuiPanel::Draw(fElapsed);
}
// PC 0x0070E800 GOG 0x0070DD40
CSWGuiDialogTop::CSWGuiDialogTop(CSWGuiManager* pManager,
                               const CResRef& cFont, const Vector& vColor)
    : CSWGuiPanel(pManager)
{
    int nMarginX;
    int nMarginY;
    m_pManager->GetSafeMargin(nMarginX, nMarginY);
    m_cExtent.m_nLeft = nMarginX;
    m_cExtent.m_nTop = nMarginY;
    m_cExtent.m_nWidth = m_pManager->m_nWidth - 2 * nMarginX;
    m_cExtent.m_nHeight =
        (m_pManager->m_nHeight -
         (int)(m_pManager->m_nWidth * 0.42857149f)) / 2 - nMarginY - 5;

    CSWGuiExtent cExtent = m_lblReply.m_cExtent;
    cExtent.m_nWidth = m_cExtent.m_nWidth;
    m_lblReply.SetExtent(cExtent);
    m_vColor = CGuiInGame::COLOR_TRANSPARENT;

    {
        CExoString sReply("");
        m_lblReply.Initialize(
            CSWGuiExtent(0, 0, m_cExtent.m_nWidth, m_cExtent.m_nHeight),
            CSWGuiTextParams(sReply, cFont, vColor,
                             CSWGuiTextParams::ALIGNMENT_DIALOG_REPLY, 1.0f),
            CSWGuiBorderParams(CResRef(), CResRef(), CResRef(), 0, 0, 1.0f,
                               Vector(1.0f, 1.0f, 1.0f), 0),
            1.0f);
    }
    AddControl(&m_lblReply);
#ifndef _XBOX
    m_bPositioned = 1;
#endif
}
// PC 0x0070EA20 GOG 0x0070EE90
CSWGuiDialogCinematic::CSWGuiDialogCinematic(CSWGuiManager* pManager)
    : CSWGuiDialog(pManager)
{
    m_vReplyColor = CGuiInGame::COLOR_NEW_TEXT;
    m_vReplyHighlightColor = CGuiInGame::COLOR_NEW_HIGHLIGHT;
    StartLoadFromLayout("dialog_p", 0, 0);
    InitControl(m_lbReplies, "LB_REPLIES", 1, 0);
    CSWGuiListBox& cReplies = m_lbReplies;
    InitControl(m_lblMessage, "LBL_MESSAGE", 1, 0);
    StopLoadFromLayout();

    int nMarginX, nMarginY;
    m_pManager->GetSafeMargin(nMarginX, nMarginY);
    CSWGuiExtent cExtent;
    cExtent.m_nTop = cExtent.m_nWidth = cExtent.m_nHeight = 0;
    cExtent.m_nLeft = nMarginX;
    cExtent.m_nTop =
        (m_pManager->m_nHeight - (int)(m_pManager->m_nWidth / -2.333333f)) / 2;
    cExtent.m_nWidth = m_pManager->m_nWidth - 2 * nMarginX;
#ifdef _XBOX
    cExtent.m_nHeight = 66;
#else
    cExtent.m_nHeight = 100;
#endif
    SetExtent(cExtent);

    CSWGuiLabel* pProto = (CSWGuiLabel*)m_lbReplies.m_pProtoItem;
    for (int i = 0; i < MAX_REPLIES; ++i)
    {
        int nReplyWidth = cReplies.m_cItemExtent.m_nWidth - 2 * cReplies.m_nPadding;
        CSWGuiExtent cReplyExtent;
        cReplyExtent.m_nLeft = 0;
        cReplyExtent.m_nTop = 0;
        cReplyExtent.m_nWidth = nReplyWidth;
        cReplyExtent.m_nHeight = 20;
        m_lblReply[i].Initialize(cReplyExtent, pProto, 1.0f);
        m_lblReply[i].AddEvent(0, this,
                             (CSWGuiHandler)&CSWGuiDialog::SetReplyActive);
        m_lblReply[i].AddEvent(1, this,
                             (CSWGuiHandler)&CSWGuiDialog::SetReplyInactive);
    }
    m_lblMessage.m_cText.m_cParams.SetColor(m_vReplyColor);

#ifdef _XBOX
    m_pTop = new CSWGuiDialogTop(m_pManager,
                                 m_lblMessage.m_cText.m_cParams.m_cFont,
                                 m_vReplyColor);
#else
    CResRef cFont;
    {
#ifndef _XBOX
        CExoString sFont(m_lblMessage.m_cText.m_cParams.m_szBaseFont);
        cFont = sFont;
#else
        cFont = m_lblMessage.m_cText.m_cParams.m_cFont;
#endif
    }
    m_pTop = new CSWGuiDialogTop(m_pManager, cFont, m_vReplyColor);
#endif
#ifndef _XBOX
    m_bPositioned = 1;
#endif
}
// PC 0x0070ECC0 GOG 0x0070E0A0
CSWGuiDialogCinematic::~CSWGuiDialogCinematic()
{
    if (m_pTop)
    {
        delete m_pTop;
        m_pTop = 0;
    }
}
// PC 0x0070EDD0 GOG 0x0070E570
CSWGuiDialogComputer::~CSWGuiDialogComputer()
{
    if (m_pSound)
    {
        delete m_pSound;
        m_pSound = 0;
    }

    m_lbMessage.ClearItems();
}
// PC 0x0070EE90 GOG 0x0070E690
void CSWGuiDialogComputer::UpdateSkills()
{
    SwguidialogTrace1();
    CSWCCreature* pCreature =
        g_pAppManager->GetClientExoApp()->GetSWParty()->GetCharacter(0);
    CItemRepository* pRepository =
        pCreature->GetServerCreature()->GetItemRepository(1);
    int nSpikes = 0;
    int nRepairParts = 0;
    for (unsigned int i = 0; i < pRepository->GetNumItems(); i++)
    {
        CSWSItem* pItem = pRepository->ItemListGetItem(i);
        if (pItem->m_sTag == "k_computer_spike")
        {
            nSpikes += pItem->GetStackSize();
        }
        else if (pItem->m_sTag == "k_repair_part")
        {
            nRepairParts += pItem->GetStackSize();
        }
    }
    m_aInfo[0].m_lblValue.m_cText.m_cParams.SetText(pCreature->GetStats()->CSWCCreatureStats::GetSkillRank(
            CSWRules::SKILL_COMPUTER_USE));
    m_aInfo[1].m_lblValue.m_cText.m_cParams.SetText(pCreature->GetStats()->CSWCCreatureStats::GetSkillRank(
            CSWRules::SKILL_REPAIR));
    m_aInfo[2].m_lblValue.m_cText.m_cParams.SetText(nSpikes);
    m_aInfo[3].m_lblValue.m_cText.m_cParams.SetText(nRepairParts);
}
// PC 0x0070F050 GOG 0x0070E850
CSWGuiDialogComputerCamera::~CSWGuiDialogComputerCamera()
{
    if (m_pTop)
    {
        delete m_pTop;
        m_pTop = 0;
    }
}
// PC 0x0070F100 GOG 0x0070EA00
void CSWGuiDialog::SetReplies(int nCount, CExoString* pReplies,
                              unsigned long* pReplyIndices, int nStartIndex,
                              int bUnknown)
{
    Reset();
    CSWGuiExtent cExtent;
    cExtent.m_nLeft = cExtent.m_nTop = cExtent.m_nWidth = cExtent.m_nHeight =
        0;
    CExoArrayList<CSWGuiControl*> lReplies;
    if (nCount >= MAX_REPLIES)
    {
        nCount = MAX_REPLIES;
    }
    m_nReplyCount = nCount;
    for (int i = 0; i < m_nReplyCount; i++)
    {
        if (!pReplies[i].IsEmpty())
        {
#ifdef _XBOX
            m_lblReply[i].m_cText.m_cParams.SetText(pReplies[i]);
#else
            CExoString sNumber;
            sNumber.Format("%d. ", i + 1);
            m_lblReply[i].m_cText.m_cParams.SetText(sNumber + pReplies[i]);
#endif
#ifndef _XBOX
            m_lblReply[i].m_cText.m_cParams.ReSetFont();
#endif
            cExtent = m_lblReply[i].m_cExtent;
            cExtent.m_nHeight = m_lblReply[i].m_cText.GetIdealHeight();
            m_lblReply[i].SetExtent(cExtent);
#ifndef _XBOX
            m_lblReply[i].AddEvent(
                CSWGuiControl::EVENT_SELECT, this,
                (CSWGuiHandler)&CSWGuiDialog::HandleSelectReply);
#endif
            lReplies.Add(&m_lblReply[i]);
        }
        m_aReplyIndex[i] = pReplyIndices[i];
    }
    if (bUnknown)
    {
        m_lbReplies.AddControls(lReplies, 0, 0, 1);
        m_lbReplies.SetEnabledFlag(1);
        SetActiveControl(&m_lbReplies, 0);
        m_nSelectedReply = 0;
        m_lbReplies.SetSelectedControl(0, 0);
#ifndef _XBOX
        short nBottom = m_lbReplies.m_nBottomVisibleItem;
        int nNum = m_lbReplies.m_lItems.num;
        m_lbReplies.SetShowScrollBar(nBottom < nNum - 1);
#endif
    }
    m_bDialogActive = 0;
#ifndef _XBOX
    m_pManager->HandleMouseMove(m_pManager->m_nMouseX,
                                m_pManager->m_nMouseY);
#endif
}
// PC 0x0070F400 GOG 0x0070F150
void CSWGuiDialogCinematic::SetReplies(int nCount, CExoString* pReplies,
                                       unsigned long* pReplyIndices,
                                       int nStartIndex, int bUnknown)
{
    if (bUnknown)
    {
        m_pTop->SetReply(m_sMessage);
    }
    CSWGuiDialog::SetReplies(nCount, pReplies, pReplyIndices, nStartIndex,
                             bUnknown);
    if (m_lbReplies.m_bEnabled)
    {
        CSWGuiExtent cExtent = m_lbReplies.m_cExtent;
        int nFontHeight = m_lblReply[0].m_cText.GetFontHeight();
        cExtent.m_nHeight -= cExtent.m_nHeight % nFontHeight;
        m_lbReplies.SetExtent(cExtent);
    }
    if (bUnknown && !m_pManager->PanelExists(m_pTop))
    {
        m_pManager->AddPanel(m_pTop, 0, 1);
    }
}
// PC 0x0070F4D0 GOG 0x0070F220
CSWGuiDialogComputer::CSWGuiDialogComputer(CSWGuiManager* pManager)
    : CSWGuiDialog(pManager)
{
    CExoString sControl;
    m_pSound = new CExoSoundSource("computer_lp_04");
    if (m_pSound)
    {
        m_pSound->SetLooping(1);
        m_pSound->SetVolume(30);
    }

    StartLoadFromLayout("computer_p");
    InitControl(m_lbMessage, "LB_MESSAGE", 1);
    InitControl(m_lbReplies, "LB_REPLIES", 1);
    CSWGuiListBox& cReplies = m_lbReplies;
    InitControl(m_aInfo[INFO_COMP_SKILL].m_lblName, "LBL_COMP_SKILL", 1);
    InitControl(m_aInfo[INFO_COMP_SKILL].m_lblValue, "LBL_COMP_SKILL_VAL", 1);
    InitControl(m_aInfo[INFO_REP_SKILL].m_lblName, "LBL_REP_SKILL", 1);
    InitControl(m_aInfo[INFO_REP_SKILL].m_lblValue, "LBL_REP_SKILL_VAL", 1);
    InitControl(m_aInfo[INFO_COMP_SPIKES].m_lblName, "LBL_COMP_SPIKES", 1);
    InitControl(m_aInfo[INFO_COMP_SPIKES].m_lblValue, "LBL_COMP_SPIKES_VAL", 1);
    InitControl(m_aInfo[INFO_REP_UNITS].m_lblName, "LBL_REP_UNITS", 1);
    InitControl(m_aInfo[INFO_REP_UNITS].m_lblValue, "LBL_REP_UNITS_VAL", 1);
    InitControl(m_lblBar[0], "LBL_BAR1", 1);
    InitControl(m_lblBar[1], "LBL_BAR2", 1);
    InitControl(m_lblBar[2], "LBL_BAR3", 1);
    InitControl(m_lblBar[3], "LBL_BAR4", 1);
    InitControl(m_lblBar[4], "LBL_BAR5", 1);
    InitControl(m_lblBar[5], "LBL_BAR6", 1);
    InitControl(m_lblBar[6], "LBL_BAR7", 1);
    InitControl(m_lblBar[7], "LBL_BAR8", 1);
    InitControl(m_lblBar[8], "LBL_BAR9", 1);
    InitControl(m_lblBar[9], "LBL_BAR10", 1);
    StopLoadFromLayout();

    CSWSkill* pSkill = &g_pRules->m_pSkills[CSWRules::SKILL_COMPUTER_USE];
    m_aInfo[INFO_COMP_SKILL].m_lblName.m_cText.m_cParams.SetStrRef(pSkill->GetNameStrRef());
    pSkill = &g_pRules->m_pSkills[CSWRules::SKILL_REPAIR];
    m_aInfo[INFO_REP_SKILL].m_lblName.m_cText.m_cParams.SetStrRef(pSkill->GetNameStrRef());

    {
        CSWGuiLabel* pProto = (CSWGuiLabel*)m_lbReplies.m_pProtoItem;
        for (int i = 0; i < MAX_REPLIES; ++i)
        {
            int nReplyWidth = cReplies.m_cItemExtent.m_nWidth - 2 * cReplies.m_nPadding;
            CSWGuiExtent cExtent;
            cExtent.m_nLeft = 0;
            cExtent.m_nTop = 0;
            cExtent.m_nWidth = nReplyWidth;
            cExtent.m_nHeight = 20;
            m_lblReply[i].Initialize(cExtent, pProto, 1.0f);
            m_lblReply[i].AddEvent(CSWGuiControl::EVENT_ENTER, this,
                                 (CSWGuiHandler)&CSWGuiDialog::SetReplyActive);
            m_lblReply[i].AddEvent(CSWGuiControl::EVENT_EXIT, this,
                                 (CSWGuiHandler)&CSWGuiDialog::SetReplyInactive);
        }
    }
    CSWGuiLabel* pProto = (CSWGuiLabel*)m_lbMessage.m_pProtoItem;
    m_lblMessage.Initialize(pProto->m_cExtent, pProto, 1.0f);
    m_vReplyColor = pProto->m_cText.m_cParams.m_vColor;
    m_lblMessage.m_cText.m_cParams.SetColor(m_vReplyColor);
    m_vReplyHighlightColor = CGuiInGame::COLOR_WHITE;
}
// PC 0x0070FBB0 GOG 0x0070F900
void CSWGuiDialogComputer::SetReplies(int nCount, CExoString* pReplies,
                                      unsigned long* pReplyIndices,
                                      int nStartIndex, int bUnknown)
{
    CSWGuiDialog::SetReplies(nCount, pReplies, pReplyIndices, nStartIndex,
                             bUnknown);
    if (bUnknown != 0)
    {
        m_lblMessage.SetEnabledFlag(1);
    }
    UpdateSkills();
}
// PC 0x0070FBF0 GOG 0x0070F940
void CSWGuiDialogComputer::SetDialogMessage(const CExoString& sMessage)
{
    SwguidialogTrace2();
    UpdateSkills();
    CSWGuiDialog::SetDialogMessage(sMessage);
    m_lbMessage.SetEnabledFlag(m_lblMessage.m_bEnabled);
    if (m_lbMessage.m_bEnabled)
    {
        m_lbMessage.ClearItems();
        CSWGuiExtent cExtent = m_lblMessage.m_cExtent;
        int nHeight;
        if (sMessage.GetLength() > 0)
        {
            nHeight = m_lblMessage.m_cText.GetIdealHeight();
        }
        else
        {
            nHeight = m_lblMessage.m_cText.GetFontHeight();
        }
        cExtent.m_nHeight = nHeight;
        m_lblMessage.SetExtent(cExtent);
        CSWGuiControl* pMessage = &m_lblMessage;
        m_lbMessage.AddControls(&pMessage, 1, 0, 0, 0);
        m_lbMessage.SetSelectedControl(&m_lblMessage, 0);
    }
}
