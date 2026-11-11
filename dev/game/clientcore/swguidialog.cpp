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

static inline void SwguidialogTrace2();
static inline void SwguidialogTrace1();
#define SWGUIDIALOG_TRACE() SwguidialogTrace1()
// PC 0x006A6D70  Xbox 0x000D3550
void CSWGuiDialog::EndDialog()
{
    m_bDialogEnded = 1;
}
// PC 0x006A6D80  Xbox 0x000D3560
int CSWGuiDialog::GetReplyIndex(int nReply)
{
    if (nReply >= m_nReplyCount || nReply < 0)
    {
        return -1;
    }
    return m_aReplyIndex[nReply];
}
// PC 0x006A6DA0  Xbox 0x000D3580
int CSWGuiDialogLetterbox::IsFadeDone()
{
    if (m_fFadeTime == 0.0f)
    {
        return 1;
    }
    return m_fFadeDelay + m_fFadeLength < m_fFadeTime;
}
// PC 0x006A6DE0  Xbox 0x000D4520
void CSWGuiDialogComputer::OnPanelAdded()
{
    m_pSound->Play();
#ifndef _XBOX
    CSWGuiPanel::OnPanelAdded();
#endif
}
// PC 0x006A6E00  Xbox 0x000D4530
void CSWGuiDialogComputer::OnPanelRemoved()
{
    m_pSound->Stop();
#ifndef _XBOX
    CSWGuiPanel::OnPanelRemoved();
#endif
}
// PC 0x006A6E20
CExoString CSWGuiDialogComputer::GetFullScreenBG()
{
    CExoString sBackground = CSWGuiManager::GetScreenResolutionString(
        g_nScreenWidth, g_nScreenHeight);
#ifndef _XBOX
    sBackground = sBackground +
        CSWGuiManager::FULLSCREEN_BGS[PC_BACKGROUND_SUFFIX_COMPUTER] +
        CExoString(m_nBackground);
#endif
    return sBackground;
}

static void SwguidialogTrace3(CSWGuiDialog* pDialog)
{
    if (0)
    {
        pDialog->CSWGuiDialog::~CSWGuiDialog();
    }
}
// PC 0x006A6FC0  Xbox 0x000D35C0
void CSWGuiDialog::SetReplyActive(CSWGuiControl* pControl)
{
    SWGUIDIALOG_TRACE();
    EXOXBOXASSERT(pControl->AsLabel());
    CSWGuiLabel* pLabel = pControl->AsLabel();
    pLabel->m_cText.m_cParams.SetColor(CGuiInGame::COLOR_YELLOW);
}
// PC 0x006A6FE0  Xbox 0x000D35F0
void CSWGuiDialog::SetReplyInactive(CSWGuiControl* pControl)
{
    SWGUIDIALOG_TRACE();
    EXOXBOXASSERT(pControl->AsLabel());
    CSWGuiLabel* pLabel = pControl->AsLabel();
    pLabel->m_cText.m_cParams.SetColor(m_vReplyColor);
}
// PC 0x006A7010  Xbox 0x000D3630
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
// PC 0x006A70A0  Xbox 0x000D36C0
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
// PC 0x006A7110  Xbox 0x000D3730
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
// PC 0x006A7160  Xbox 0x000D3780
void CSWGuiDialog::Reset()
{
    SWGUIDIALOG_TRACE();
    m_lbReplies.SetEnabledFlag(0);
    m_lblMessage.SetEnabledFlag(0);
    m_nSelectedReply = -1;
    m_nReplyCount = 0;
    m_sMessage = "";
    for (int i = 0; i < 20; i++)
    {
        m_lblReply[i].m_cText.m_cParams.SetColor(m_vReplyColor);
        CExoString sText("");
        m_lblReply[i].m_cText.m_cParams.SetText(sText);
    }
}
// PC 0x006A7230  Xbox 0x000D3850
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
// PC 0x006A7390
int CSWGuiDialog::HandleSelectReply(CSWGuiControl* pControl)
{
    SWGUIDIALOG_TRACE();
    m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
    return SelectReply(m_lbReplies.m_nSelectedItem);
}
// PC 0x006A73B0  Xbox 0x000D3930
void CSWGuiDialogLetterbox::SetExtentSpecial(CSWGuiExtent cExtent)
{
    SWGUIDIALOG_TRACE();
    switch (m_nPosition)
    {
    case POSITION_TOP:
        cExtent.m_nTop = 0;
        break;
    case POSITION_BOTTOM:
        cExtent.m_nHeight = m_pManager->m_nHeight - cExtent.m_nTop;
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
// PC 0x006A7440  Xbox 0x000D39C0
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
// PC 0x006A74B0  Xbox 0x000D3A20
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
}
// PC 0x006A7540  Xbox 0x000D3AA0
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
    m_nPosition = POSITION_BOTTOM;
}
// PC 0x006A75E0  Xbox 0x000D3B30
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
// PC 0x006A7620  Xbox 0x000D3B70
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
// PC 0x006A7680  Xbox 0x000D3BE0
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
        }
    }
    CSWGuiPanel::Draw(fElapsed);
}
// PC 0x006A78C0  Xbox 0x000D4A50
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
// PC 0x006A7AE0  Xbox 0x000D3E10
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
// PC 0x006A8B40  Xbox 0x000D5280
CSWGuiDialogCinematic::CSWGuiDialogCinematic(CSWGuiManager* pManager)
    : CSWGuiDialog(pManager)
{
    m_vReplyColor = CGuiInGame::COLOR_BLUE;
    StartLoadFromLayout("dialog");
    InitControl(m_lbReplies, "LB_REPLIES", 1);
    CSWGuiListBox& cReplies = m_lbReplies;
    InitControl(m_lblMessage, "LBL_MESSAGE", 1);
    StopLoadFromLayout();

    int nMarginX, nMarginY;
    m_pManager->GetSafeMargin(nMarginX, nMarginY);
    CSWGuiExtent cExtent;
#ifdef _XBOX
    cExtent.m_nTop = cExtent.m_nWidth = cExtent.m_nHeight = 0;
    cExtent.m_nLeft = nMarginX;
#else
    cExtent.m_nLeft = nMarginX;
    cExtent.m_nTop = cExtent.m_nWidth = cExtent.m_nHeight = 0;
#endif
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
    for (int i = 0; i < 20; ++i)
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
// PC 0x006A7C20  Xbox 0x000D4C70
CSWGuiDialogCinematic::~CSWGuiDialogCinematic()
{
    if (m_pTop)
    {
        delete m_pTop;
        m_pTop = 0;
    }
}
// PC 0x006A7CA0  Xbox 0x000D3F50
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
// PC 0x006A7D30  Xbox 0x000D3FE0
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
// PC 0x006A7E20  Xbox 0x000D40D0
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
// PC 0x006A7EF0  Xbox 0x000D41A0
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
// PC 0x006A8DE0  Xbox 0x000D4CF0
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
// PC 0x006A8EB0  Xbox 0x000D54E0
CSWGuiDialogComputer::CSWGuiDialogComputer(CSWGuiManager* pManager)
    : CSWGuiDialog(pManager)
{
    CExoString sControl;
    m_vReplyColor = Vector(0.67f, 0.56f, 0.87f);
    m_pSound = new CExoSoundSource("computer_lp_04");
    if (m_pSound)
    {
        m_pSound->SetLooping(1);
        m_pSound->SetVolume(30);
    }

    StartLoadFromLayout("computer");
    InitControl(m_lbMessage, "LB_MESSAGE", 1);
    InitControl(m_lbReplies, "LB_REPLIES", 1);
    CSWGuiListBox& cReplies = m_lbReplies;
#ifdef _XBOX
    InitControl(m_lblRAnalog, "LBL_RANALOG", 1);
#endif
    InitControl(m_lblObscure, "LBL_OBSCURE", 1);
    for (int i = 0; i < 4; ++i)
    {
        sControl.Format("LBL_STATIC%d", i + 1);
        InitControl(m_lblStatic[i], sControl, 1);
        m_lblStatic[i].m_cBorder.m_cParams.m_fHilightAlpha = 0.4f;
    }

    InitControl(m_aInfo[INFO_COMP_SKILL].m_lblName, "LBL_COMP_SKILL", 1);
    InitControl(m_aInfo[INFO_COMP_SKILL].m_lblIcon, "LBL_COMP_SKILL_ICON", 1);
    InitControl(m_aInfo[INFO_COMP_SKILL].m_lblValue, "LBL_COMP_SKILL_VAL", 1);
    InitControl(m_aInfo[INFO_REP_SKILL].m_lblName, "LBL_REP_SKILL", 1);
    InitControl(m_aInfo[INFO_REP_SKILL].m_lblIcon, "LBL_REP_SKILL_ICON", 1);
    InitControl(m_aInfo[INFO_REP_SKILL].m_lblValue, "LBL_REP_SKILL_VAL", 1);
    InitControl(m_aInfo[INFO_COMP_SPIKES].m_lblName, "LBL_COMP_SPIKES", 1);
    InitControl(m_aInfo[INFO_COMP_SPIKES].m_lblIcon, "LBL_COMP_SPIKES_ICON", 1);
    InitControl(m_aInfo[INFO_COMP_SPIKES].m_lblValue, "LBL_COMP_SPIKES_VAL", 1);
    InitControl(m_aInfo[INFO_REP_UNITS].m_lblName, "LBL_REP_UNITS", 1);
    InitControl(m_aInfo[INFO_REP_UNITS].m_lblIcon, "LBL_REP_UNITS_ICON", 1);
    InitControl(m_aInfo[INFO_REP_UNITS].m_lblValue, "LBL_REP_UNITS_VAL", 1);
    StopLoadFromLayout();

    CSWSkill* pSkill = &g_pRules->m_pSkills[CSWRules::SKILL_COMPUTER_USE];
    m_aInfo[INFO_COMP_SKILL].m_lblName.m_cText.m_cParams.SetStrRef(pSkill->GetNameStrRef());
    m_aInfo[INFO_COMP_SKILL].m_lblIcon.m_cBorder.m_cParams.SetFillImage(pSkill->m_sIcon, 0);
    pSkill = &g_pRules->m_pSkills[CSWRules::SKILL_REPAIR];
    m_aInfo[INFO_REP_SKILL].m_lblName.m_cText.m_cParams.SetStrRef(pSkill->GetNameStrRef());
    m_aInfo[INFO_REP_SKILL].m_lblIcon.m_cBorder.m_cParams.SetFillImage(pSkill->m_sIcon, 0);

    {
        CSWGuiLabel* pProto = (CSWGuiLabel*)m_lbReplies.m_pProtoItem;
        for (int i = 0; i < 20; ++i)
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
    m_lblMessage.m_cText.m_cParams.SetColor(m_vReplyColor);
}
// PC 0x006A8110  Xbox 0x000D4E20
CSWGuiDialogComputer::~CSWGuiDialogComputer()
{
    if (m_pSound)
    {
        delete m_pSound;
        m_pSound = 0;
    }

    m_lbMessage.ClearItems();
}
// PC 0x006A81E0  Xbox 0x000D4300
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
// PC 0x006A8240  Xbox 0x000D4360
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
// PC 0x006A8400  Xbox 0x000D4540
void CSWGuiDialogComputer::SetType(unsigned char nType)
{
    SwguidialogTrace1();
    C2DA* pTypes = new C2DA("comptypes", 0);
    if (!pTypes->Load2DArray())
    {
        return;
    }
    if (nType >= pTypes->GetNumRows())
    {
        if (pTypes->GetNumRows() == 0)
        {
            return;
        }
        nType = 0;
    }
#ifndef _XBOX
    m_nBackground = nType;
#endif
    CExoString sComputerBackground;
    pTypes->GetCExoStringEntry(nType, "ComputerBackground",
                              &sComputerBackground);
    SetBackground(sComputerBackground);
    delete pTypes;
}
// PC 0x006A94E0  Xbox 0x000D4F00
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
// PC 0x006A9520  Xbox 0x000D4F40
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
// PC 0x006A95F0  Xbox 0x000D5B60
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
        CResRef cLayout("computercamera");
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
// PC 0x006A8520  Xbox 0x000D5080
CSWGuiDialogComputerCamera::~CSWGuiDialogComputerCamera()
{
    if (m_pTop)
    {
        delete m_pTop;
        m_pTop = 0;
    }
}
// PC 0x006A85B0  Xbox 0x000D5110
CSWGuiDialog::CSWGuiDialog(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    m_nOwner = OBJECT_INVALID;
    m_bDialogActive = 1;
    m_bDialogWaiting = 0;
    m_bDialogEnded = 1;
    Reset();
}
// PC 0x006A86A0  Xbox 0x000D46F0
void CSWGuiDialog::SetReplies(int nCount, CExoString* pReplies,
                              unsigned long* pReplyIndices, int nStartIndex,
                              int bUnknown)
{
    Reset();
    CSWGuiExtent cExtent;
    cExtent.m_nLeft = cExtent.m_nTop = cExtent.m_nWidth = cExtent.m_nHeight =
        0;
    CExoArrayList<CSWGuiControl*> lReplies;
#ifndef _XBOX
    CExoString sNumber;
#endif
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
            if (i < NUMBERED_REPLIES)
            {
                sNumber.Format("%d. ", i + 1);
            }
            else
            {
                sNumber = "-. ";
            }
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
// PC 0x006A8930  Xbox 0x000D48A0
CSWGuiDialogLetterbox::CSWGuiDialogLetterbox(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
#ifdef _XBOX
    m_vColor = CGuiInGame::COLOR_BLACK;
    ResetFade();
    m_nPosition = 0;
    m_bDone = 0;
#else
    m_nPosition = 0;
    m_bDone = 0;
    m_vColor = CGuiInGame::COLOR_BLACK;
    ResetFade();
#endif
}

#include "nwrules.h"

#include "nwskill.h"
// PC 0x006A8A60  Xbox 0x000D49E0
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

static inline void SwguidialogTrace2()
{
    SwguidialogTrace1();
}

static inline void SwguidialogTrace1()
{
}
