#include "nwcanimbase.h"
#include "clientexoapp.h"
#include "clientoptions.h"
#include "nwscreaturestats.h"
#include "exotlktable.h"
#include "partytables.h"
#include "swmain.h"
#include "appmanager.h"
#include "aurcamera.h"
#include "clientexoappinternal.h"
#include "aurgui.h"
#include "swparty.h"
#include "nwccreature.h"
#include "nwccreaturestats.h"
#include "swguimaininterfacepc2.h"
#include "swguicustompanel.h"
#include "nwsobject.h"
#include "nwscombatround.h"
#include "nwsitem.h"
#include "guiingame.h"
#include "serverexoapp.h"
#include "nwsmodule.h"
#include "nwcdoor.h"
#include "nwcplaceable.h"
#include "nwsplaceable.h"
#include "nwrules.h"
#include "twodimarrays.h"
#include "nwctrigger.h"
#include "nwcjournal.h"
#include "nwtlktable.h"

#include <math.h>

static inline void Swguimaininterfacepc2Trace3();
static inline void Swguimaininterfacepc2Trace2();
static inline void Swguimaininterfacepc2Trace1();
#define SWGUIMAININTERFACEPC2_TRACE() Swguimaininterfacepc2Trace1()

static inline CClientExoApp* ClientExoAppLate3(CAppManager* p);
static inline CClientExoApp* ClientExoAppLate2(CAppManager* p);
static inline CClientExoApp* ClientExoAppLate1(CAppManager* p);
// PC 0x004015A0
void CSWGuiMainInterface::QueueActiveAction()
{
}

#ifndef _XBOX
CSWGuiControl* CSWGuiMainInterfaceAction::HitCheckMouse(int nX, int nY)  // PC 0x006DD090
{
    CSWGuiControl* pControl = NULL;
    if (m_CSWGuiButton_388.HitCheckMouse(nX, nY) != 0)
    {
        pControl = &m_CSWGuiButton_388;
    }
    else if (m_CSWGuiButton_54C.HitCheckMouse(nX, nY) != 0)
    {
        pControl = &m_CSWGuiButton_54C;
    }
    else if (m_CSWGuiButton_1C4.HitCheckMouse(nX, nY) != 0)
    {
        pControl = &m_CSWGuiButton_1C4;
    }
    else if (m_CSWGuiButton_0.HitCheckMouse(nX, nY) != 0)
    {
        pControl = &m_CSWGuiButton_0;
    }
    return pControl;
}
#endif

#ifndef _XBOX
inline void CSWGuiTargetActionMenu::Update(float fElapsed)  // PC 0x006DD320
{
    if (m_fFeedbackTimer != -1.0f)
    {
        m_fFeedbackTimer -= fElapsed;
        if (m_fFeedbackTimer <= 0.0f)
            m_fFeedbackTimer = -1.0f;
    }
    if (m_fMouseOverTimer != -1.0f)
    {
        m_fMouseOverTimer -= fElapsed;
        if (m_fMouseOverTimer <= 0.0f)
            m_fMouseOverTimer = -1.0f;
    }
}
#endif

#ifndef _XBOX
char CSWGuiTargetActionMenu::IsTargetActionMenuControl(CSWGuiControl* pControl)  // PC 0x006DD440
{
    if (pControl == &m_cNameLabel || pControl == &m_cHealthBar ||
        pControl == &m_cNameBackground || pControl == &m_cHealthBackground)
    {
        return 1;
    }

    for (int nAction = 0; nAction < 3; nAction++)
    {
        if (pControl == &m_aActions[nAction].m_CSWGuiButton_388 ||
            pControl == &m_aActions[nAction].m_CSWGuiButton_54C ||
            pControl == &m_aActions[nAction].m_CSWGuiButton_1C4 ||
            pControl == &m_aActions[nAction].m_CSWGuiButton_0)
        {
            return 2;
        }
    }
    return 0;
}
#endif

#ifndef _XBOX
void CSWGuiTargetActionMenu::ResetFont()
{
#ifndef _XBOX
    m_cNameLabel.ReSetFont();
#endif
#ifndef _XBOX
    m_cNameBackground.ReSetFont();
#endif
#ifndef _XBOX
    m_cHealthBackground.ReSetFont();
#endif
}
#endif

#ifndef _XBOX
void CSWGuiCharButton::UpdateToolTipText()  // PC 0x006DD4C0
{
    if (m_pCharacter != NULL)
    {
        if (m_pCharacter->m_nMaxForce != 0)
        {
            m_sToolTipText.Format("%s %s\n%s: %d/%d\n%s: %d/%d\n%s: %d\n%s: %d",
                m_pCharacter->m_sName.CStr(),
                m_pCharacter->m_sDescription.CStr(),
                g_pTlkTable->GetSimpleString(STRREF_VITALITY).CStr(),
                m_pCharacter->m_nCurHealth, m_pCharacter->m_nMaxHealth,
                g_pTlkTable->GetSimpleString(STRREF_FORCE).CStr(),
                m_pCharacter->m_nCurForce, m_pCharacter->m_nMaxForce,
                g_pTlkTable->GetSimpleString(STRREF_LEVEL).CStr(),
                m_pCharacter->m_nLevel,
                g_pTlkTable->GetSimpleString(STRREF_XP_NEEDED).CStr(),
                m_pCharacter->m_nXPNeeded);
        }
        else
        {
            m_sToolTipText.Format("%s %s\n%s: %d/%d\n%s: %d\n%s: %d",
                m_pCharacter->m_sName.CStr(),
                m_pCharacter->m_sDescription.CStr(),
                g_pTlkTable->GetSimpleString(STRREF_VITALITY).CStr(),
                m_pCharacter->m_nCurHealth, m_pCharacter->m_nMaxHealth,
                g_pTlkTable->GetSimpleString(STRREF_LEVEL).CStr(),
                m_pCharacter->m_nLevel,
                g_pTlkTable->GetSimpleString(STRREF_XP_NEEDED).CStr(),
                m_pCharacter->m_nXPNeeded);
        }
    }
}
#endif
// PC 0x006DD830
void CSWGuiMainInterfaceChar::Adjust(float fElapsed)
{
    CSWGuiExtent cExtent;
    int nIndex;

    float fStep = fElapsed * 4.0f;

    cExtent = m_aBars[0].m_cExtent;
    float fRemaining = 1.0f - fStep;
    int nOffsetX =
        (int)((m_nEndX - m_nStartX) * fRemaining + m_nStartX - cExtent.m_nLeft);
    int nOffsetY =
        (int)((m_nEndY - m_nStartY) * fRemaining + m_nStartY - cExtent.m_nTop);

    for (nIndex = 0; nIndex <= LABEL_CHAR; nIndex++)
    {
        cExtent = m_aLabels[nIndex].m_cExtent;
        cExtent.m_nLeft += nOffsetX;
        cExtent.m_nTop += nOffsetY;
        m_aLabels[nIndex].SetExtent(cExtent);
    }

    for (nIndex = 0; nIndex < 2; nIndex++)
    {
        cExtent = m_aBars[nIndex].m_cExtent;
        cExtent.m_nLeft += nOffsetX;
        cExtent.m_nTop += nOffsetY;
        m_aBars[nIndex].SetExtent(cExtent);
    }

#ifndef _XBOX
    cExtent = m_cCharButton.m_cExtent;
    cExtent.m_nLeft += nOffsetX;
    cExtent.m_nTop += nOffsetY;
    m_cCharButton.SetExtent(cExtent);
#endif
}
// PC 0x006DD9B0
void CSWGuiMainInterfaceChar::DrawStack(float fElapsed, int bLarge, int bBuff)
{
    CSWGuiExtent cViewport = m_aLabels[LABEL_CHAR].m_cExtent;
    AurGUIStartLayer();
    if (AurGUISetupViewport(cViewport.m_nLeft, cViewport.m_nTop,
                            cViewport.m_nWidth, cViewport.m_nHeight,
                            CSWGuiDrawable::NO_COLOURING, false, 1.0f) != 0)
    {
        unsigned char nCount = bBuff ? m_nBuffCount : m_nDebuffCount;
        if (bLarge)
            nCount = min(nCount, MAX_LARGE_STACK_COUNT);
        else
            nCount = min(nCount, MAX_SMALL_STACK_COUNT);

        CSWGuiLabel* pLabel = bBuff ? &m_cBuffLabel : &m_cDebuffLabel;
        unsigned char nHeight = bLarge ? 16 : 8;
        CSWGuiExtent cOriginal = pLabel->m_cExtent;
        CSWGuiExtent cExtent = pLabel->m_cExtent;
        unsigned char nSpacing;
        if (nCount < MAX_SMALL_STACK_COUNT)
        {
            nSpacing = bLarge ? 10 : 5;
            if (bBuff)
                cExtent.m_nTop = cOriginal.m_nTop - (nCount - 1) * (nSpacing >> 1);
            else
                cExtent.m_nTop = cOriginal.m_nTop + (nCount - 1) * (nSpacing >> 1);
        }
        else
        {
            if (bBuff)
                cExtent.m_nTop = 1;
            else
                cExtent.m_nTop = cViewport.m_nHeight - nHeight - 1;
            if (bLarge)
            {
                switch (nCount)
                {
                case 5: nSpacing = 10; break;
                case 6: nSpacing = 8; break;
                case 7: nSpacing = 7; break;
                case 8: nSpacing = 6; break;
                case 9: nSpacing = 5; break;
                }
            }
            else
                nSpacing = 5;
        }
        for (int nIndex = 0; nIndex < nCount; nIndex++)
        {
            pLabel->SetExtent(cExtent);
            pLabel->Draw(fElapsed);
            if (bBuff)
                cExtent.m_nTop += nSpacing;
            else
                cExtent.m_nTop -= nSpacing;
            if ((nCount == 7 || nCount == 8) && (nIndex == 0 || nIndex == 2))
            {
                if (bBuff)
                    cExtent.m_nTop--;
                else
                    cExtent.m_nTop++;
            }
        }
        pLabel->SetExtent(cOriginal);
        AurGUICloseViewport();
    }
    AurGUIStopLayer();
}

void CSWGuiMainInterfaceChar::DrawEffects(float fElapsed, int bLarge)
{
    if (m_aLabels[LABEL_CHAR].m_bEnabled)
    {
        if (m_nBuffCount > 0)
            DrawStack(fElapsed, bLarge, 1);
        if (m_nDebuffCount > 0)
            DrawStack(fElapsed, bLarge, 0);
    }
}
// PC 0x006DCFD0
void CSWGuiButtonToggle::SetToggled(int bToggled)
{
    m_bToggled = bToggled;
}

#ifndef _XBOX
void CSWGuiMainInterface::SetActionDescription(const CExoString& sDescription)  // PC 0x006DDD40
{
    m_cActionText.m_cText.m_cParams.SetText(sDescription);

    CSWGuiExtent cExtent = m_cActionText.m_cExtent;
    int nHeight = m_cActionText.m_cText.GetIdealHeight();
    cExtent.m_nHeight = nHeight;
    cExtent.m_nTop = m_nActionTextBottom - nHeight;
    m_cActionText.SetExtent(cExtent);
    m_cActionTextBack.SetExtent(cExtent);
}
#endif
// PC 0x006DDDD0
int CSWGuiMainInterface::SetTarget(unsigned long nTargetId, int nUnknown)
{
    if (m_nTargetId != nTargetId)
    {
        m_nTargetId = nTargetId;
        m_fTargetUpdateDelay = -1.0f;
    }
    return nTargetId;
}
// PC 0x006DE410
void CSWGuiMainInterface::ShowActionIcon(int& nIcon, const CResRef& cIcon,
                                         const CResRef& cDefaultIcon,
                                         int bEnabled, int bForce)
{
    if (bForce != 0 || cIcon != "")
    {
        if (cIcon.IsValid() != 0)
        {
            m_aActionIcons[nIcon].m_cBorder.m_cParams.SetFillImage(cIcon, 0);
        }
        else
        {
            m_aActionIcons[nIcon].m_cBorder.m_cParams.SetFillImage(cDefaultIcon,
                                                                   0);
        }

        if (bEnabled != 0 || nIcon != 0)
        {
            m_aActionIcons[nIcon].m_cBorder.m_cParams.m_fHilightAlpha = 1.0f;
        }
        else
        {
            m_aActionIcons[0].m_cBorder.m_cParams.m_fHilightAlpha = 0.25f;
        }

        CSWGuiLabel* pIcon = &m_aActionIcons[nIcon];
        nIcon++;
        pIcon->m_bEnabled = 1;
    }
}

#ifndef _XBOX
void CSWGuiMainInterface::OnDefaultActionFocus(CSWGuiControl* pControl)  // PC 0x006DECE0
{
    if (pControl->m_bSelected != 0)
    {
        if (pControl->m_nIndex != m_nDefaultAction)
        {
            m_nDefaultAction = pControl->m_nIndex;
        }
    }
}
#endif

#ifndef _XBOX
void CSWGuiMainInterface::DisplayToolTipMessage()  // PC 0x006DF2F0
{
    m_lblCmbtmodemsg.SetEnabledFlag(0);
    m_cToolTip.SetEnabledFlag(1);
    m_cToolTipBackground.SetEnabledFlag(1);
    m_bToolTipShown = 1;
}
#endif

#ifndef _XBOX
void CSWGuiMainInterface::RemoveToolTipMessage()  // PC 0x006DF320
{
    m_cToolTip.SetEnabledFlag(0);
    if (m_cActionDescriptionState.m_bEnabled != 0)
    {
        m_lblCmbtmodemsg.SetEnabledFlag(1);
    }
    m_cToolTipBackground.SetEnabledFlag(m_lblCmbtmodemsg.m_bEnabled);
    m_bToolTipShown = 0;
}
#endif

#ifndef _XBOX
void CSWGuiMainInterface::ResetFont()  // PC 0x006DF3D0
{
    CSWGuiPanel::ResetFont();
#ifndef _XBOX
    m_cTargetActionMenu.m_cNameLabel.ReSetFont();
#endif
#ifndef _XBOX
    m_cTargetActionMenu.m_cNameBackground.ReSetFont();
#endif
#ifndef _XBOX
    m_cTargetActionMenu.m_cHealthBackground.ReSetFont();
#endif
}
#endif
// PC 0x004DD4D0
template <> void CExoArrayList<CSWGuiFloatyText*>::DelIndex(int nIndex)
{
    num--;
    for (; nIndex < num; nIndex++)
    {
        element[nIndex] = element[nIndex + 1];
    }
}

#ifndef _XBOX
inline int CSWGuiMainInterfaceAction::GetActive()  // PC 0x006DCFF0
{
    return m_CSWGuiButton_1C4.m_bActive != 0 ||
           m_CSWGuiButton_388.m_bActive != 0 ||
           m_CSWGuiButton_54C.m_bActive != 0 || m_CSWGuiButton_0.m_bActive != 0;
}
#endif

#ifdef _XBOX
void CSWGuiMainInterfaceAction::StopSlide()
{
    m_fSlideTimer = 0.0f;
    m_lblIcon.m_cBorder.m_cParams = m_lblIconB.m_cBorder.m_cParams;
    m_lblIcon.SetExtent(m_cIconExtent);
    m_lblIcon.SetEnabledFlag(1);
}

void CSWGuiMainInterfaceAction::Draw(float fElapsed)
{
    if (m_fSlideTimer > 0.0f)
    {
        AurGUIStartLayer();
        if (AurGUISetupViewport(m_cIconExtent.m_nLeft, m_cIconExtent.m_nTop,
                                m_cIconExtent.m_nWidth, m_cIconExtent.m_nHeight,
                                CSWGuiDrawable::NO_COLOURING, false, 1.0f) != 0)
        {
            m_lblIconB.Draw(fElapsed);
            m_lblIcon.SetEnabledFlag(1);
            m_lblIcon.Draw(fElapsed);
            m_lblIcon.SetEnabledFlag(0);
            AurGUICloseViewport();
        }
        AurGUIStopLayer();
    }
}
#else
void CSWGuiMainInterfaceAction::Draw(float fElapsed)  // PC 0x006DD020
{
    if (m_CSWGuiButton_0.m_bEnabled != 0)
    {
        m_CSWGuiButton_0.Draw(fElapsed);
    }
    if (m_CSWGuiButton_388.m_bEnabled != 0)
    {
        m_CSWGuiButton_388.Draw(fElapsed);
    }
    if (m_CSWGuiButton_54C.m_bEnabled != 0)
    {
        m_CSWGuiButton_54C.Draw(fElapsed);
    }
    if (m_CSWGuiButton_1C4.m_bEnabled != 0)
    {
        m_CSWGuiButton_1C4.Draw(fElapsed);
    }
}
#endif

#ifdef _XBOX
void CSWGuiMainInterfaceAction::Update(float fElapsed)
{
    if (m_nFadeSteps != 0)
    {
        m_fFadeTimer -= fElapsed;
        if (m_fFadeTimer <= 0.0f)
        {
            m_nFadeSteps--;
            m_fFadeTimer = 0.1f;
        }

        float fAlpha = 1.0f;
        if (m_nFadeSteps != 0)
        {
            fAlpha = (0.1f - m_fFadeTimer) * 10.0f;
            if (m_nFadeSteps == 2)
            {
                fAlpha = 1.0f - fAlpha;
            }
        }
        m_lblIcon.m_cBorder.m_cParams.m_fHilightAlpha = fAlpha;
    }

    if (m_fSlideTimer > 0.0f)
    {
        m_fSlideTimer -= fElapsed;
        if (m_fSlideTimer <= 0.0f)
        {
            StopSlide();
        }
        else
        {
            CSWGuiExtent cExtentB = m_lblIconB.m_cExtent;
            CSWGuiExtent cExtent = m_lblIcon.m_cExtent;
            int nOffset = (int)(m_fSlideTimer * 10.0f * m_cIconExtent.m_nHeight);
            if (cExtentB.m_nTop < 0)
            {
                cExtentB.m_nTop = -nOffset;
                cExtent.m_nTop = cExtentB.m_nHeight - nOffset;
            }
            else
            {
                cExtentB.m_nTop = nOffset;
                cExtent.m_nTop = nOffset - cExtent.m_nHeight;
            }
            m_lblIconB.SetExtent(cExtentB);
            m_lblIcon.SetExtent(cExtent);
        }
    }
}
#else
void CSWGuiMainInterfaceAction::Update(float fElapsed)  // PC 0x006DD110
{
    if (m_nFadeSteps != 0)
    {
        m_fFadeTimer -= fElapsed;
        if (m_fFadeTimer <= 0.0f)
        {
            m_nFadeSteps--;
            m_fFadeTimer = 0.1f;
        }

        float fAlpha = 1.0f;
        if (m_nFadeSteps != 0)
        {
            fAlpha = (0.1f - m_fFadeTimer) * 10.0f;
            if (m_nFadeSteps == 2)
            {
                fAlpha = 1.0f - fAlpha;
            }
        }
        m_CSWGuiButton_1C4.m_cBorder.m_cParams.m_fHilightAlpha = fAlpha;
        m_CSWGuiButton_1C4.m_cHilite.m_cParams.m_fHilightAlpha = fAlpha;
    }
}
#endif

#ifndef _XBOX
void CSWGuiMainInterfaceAction::Pulse()
{
    m_nFadeSteps = 2;
    m_fFadeTimer = 0.1f;
}
#endif

#ifdef _XBOX
void CSWGuiMainInterfaceAction::SetIcon(const CResRef& cIcon, int bEnabled,
                                        unsigned char nSlide)
{
    if (m_fSlideTimer > 0.0f
            ? cIcon != m_lblIconB.m_cBorder.m_cParams.m_cFillImage
            : cIcon != m_lblIcon.m_cBorder.m_cParams.m_cFillImage)
    {
        if (nSlide != 0)
        {
            if (m_fSlideTimer > 0.0f)
            {
                StopSlide();
            }
            m_lblIconB.m_cBorder.m_cParams.SetFillImage(cIcon, 0);
            CSWGuiExtent cExtent = m_cIconExtent;
            cExtent.m_nLeft = 0;
            cExtent.m_nTop = nSlide == 1 ? cExtent.m_nHeight : -cExtent.m_nHeight;
            m_lblIconB.SetExtent(cExtent);
            cExtent.m_nTop += cExtent.m_nHeight;
            m_lblIcon.SetExtent(cExtent);
            m_lblIcon.SetEnabledFlag(0);
            m_fSlideTimer = 0.1f;
        }
        else
        {
            StopSlide();
            m_lblIcon.m_cBorder.m_cParams.SetFillImage(cIcon, 0);
        }
        m_nFadeSteps = 0;
    }

    m_lblIconB.m_cBorder.m_cParams.m_fHilightAlpha =
        bEnabled != 0 ? 1.0f : 0.25f;
    m_lblIcon.m_cBorder.m_cParams.m_fHilightAlpha =
        bEnabled != 0 ? 1.0f : 0.25f;
}
#else
void CSWGuiMainInterfaceAction::SetIcon(const CResRef& cIcon,  // PC 0x006DD190
                                        const CExoString& sToolTip,
                                        int bEnabled, unsigned char nFadeSteps)
{
    if (cIcon != m_CSWGuiButton_1C4.m_cBorder.m_cParams.m_cFillImage)
    {
        m_CSWGuiButton_1C4.m_cBorder.m_cParams.SetFillImage(cIcon, 0);
        m_CSWGuiButton_1C4.m_cHilite.m_cParams.SetFillImage(cIcon, 0);
        m_nFadeSteps = 0;
    }

    m_CSWGuiButton_1C4.m_cBorder.m_cParams.m_fHilightAlpha =
        bEnabled != 0 ? 1.0f : 0.25f;
    m_CSWGuiButton_1C4.m_cHilite.m_cParams.m_fHilightAlpha =
        bEnabled != 0 ? 1.0f : 0.25f;
}
#endif

#ifndef _XBOX
void CSWGuiTargetActionMenu::SetFriend(int bFriend, int bPoisoned)  // PC 0x006DD210
{
    CExoString sScroll;

    if (bFriend != 0)
    {
        if (bPoisoned != 0)
        {
            CResRef cBar("POISON_BAR");
            m_cHealthBar.m_cFill.m_cParams.SetFillImage(cBar, 0);
        }
        else
        {
            CResRef cBar("FRIEND_BAR");
            m_cHealthBar.m_cFill.m_cParams.SetFillImage(cBar, 0);
        }
        sScroll = "lbl_miscroll_f";
    }
    else
    {
        if (bPoisoned != 0)
        {
            CResRef cBar("POISON_BAR");
            m_cHealthBar.m_cFill.m_cParams.SetFillImage(cBar, 0);
        }
        else
        {
            CResRef cBar("ENEMY_BAR");
            m_cHealthBar.m_cFill.m_cParams.SetFillImage(cBar, 0);
        }
        sScroll = "lbl_miscroll_h";
    }

    for (int nAction = 0; nAction < 3; nAction++)
    {
        CResRef cScroll(sScroll);
        m_aActions[nAction].m_CSWGuiButton_0.m_cBorder.m_cParams.SetFillImage(
            cScroll, 0);
    }
}
#endif

void CSWGuiMainInterface::ShiftControl(CSWGuiControl& cControl, int nOffset)
{
    CSWGuiExtent cExtent = cControl.m_cExtent;
    cExtent.m_nTop += nOffset;
    cControl.SetExtent(cExtent);
}

#ifndef _XBOX
void CSWGuiTargetActionMenu::SetNameLabel(const CExoString& sName)  // PC 0x006DF840
{
    CExoString sText = sName;
    int nTokenStart = sText.Find('<', 0);
    int nCustomStart = sText.Find('{', 0);
    if (nTokenStart != -1 || nCustomStart != -1)
    {
        g_pSWTlkTable->ParseStr(
            sText,
            (CGameObject*)g_pAppManager->m_pClientExoApp->GetSWParty()->GetPlayerCharacter(),
            0, 1);
    }
    m_cNameLabel.m_cText.m_cParams.SetText(sText);

    CSWGuiExtent cExtent = m_cNameLabel.m_cExtent;
    cExtent.m_nHeight = m_cNameLabel.m_cText.GetIdealHeight();
    m_cNameLabel.SetExtent(cExtent);
    m_cNameBackground.SetExtent(cExtent);

    int nBottom = cExtent.m_nTop + cExtent.m_nHeight;
    cExtent = m_cHealthBar.m_cExtent;
    cExtent.m_nTop = nBottom + m_nHealthBarSpacing;
    m_cHealthBar.SetExtent(cExtent);
    m_cHealthBackground.SetExtent(cExtent);

    int nOffset = nBottom + m_nActionSpacing -
                  m_aActions[0].m_CSWGuiButton_0.m_cExtent.m_nTop;
    for (int nAction = 0; nAction < 3; nAction++)
    {
        CSWGuiButton& cButton0 = m_aActions[nAction].m_CSWGuiButton_0;
        CSWGuiButton& cButton1 = m_aActions[nAction].m_CSWGuiButton_1C4;
        CSWGuiButton& cButton2 = m_aActions[nAction].m_CSWGuiButton_388;
        CSWGuiButton& cButton3 = m_aActions[nAction].m_CSWGuiButton_54C;

        m_pMainInterface->ShiftControl(cButton1, nOffset);

        m_pMainInterface->ShiftControl(cButton0, nOffset);

        m_pMainInterface->ShiftControl(cButton2, nOffset);

        m_pMainInterface->ShiftControl(cButton3, nOffset);
    }
}
#endif

#ifndef _XBOX
void CSWGuiTargetActionMenu::UpdateNameLabel(CSWCObject* pTarget)  // PC 0x006DFAB0
{
    CExoString sName;
    if (pTarget != NULL)
    {
        CSWSObject* pServerObject = pTarget->GetServerObject();
        if (pServerObject != NULL)
        {

            CExoString sFeedback;
            unsigned long nFeedbackTime;
            pTarget->GetFeedbackInfo(sName, nFeedbackTime);
            if (nFeedbackTime == 0)
            {
                if (m_fFeedbackTimer > 0.0f)
                {
                    sName = g_pAppManager->GetClientExoApp()->GetGUIString(
                        m_nFeedbackStrRef);
                }
                else
                {
                    int nMenu = m_nSelectedAction;
                    int nActions;
                    if (m_aActions[nMenu].GetActive() &&
                        nMenu >= 0 &&
                        (nActions = m_aActionLists[nMenu].num) > 0)
                    {
                        CExoArrayList<CSWGuiInterfaceAction>& aActions =
                            m_aActionLists[nMenu];
                        CSWGuiInterfaceAction* pAction = NULL;
                        int nSelected =
                            m_aSelectedActions[m_nTargetType][nMenu];
                        if (nSelected != CSWGuiInterfaceAction::INVALID_ACTION)
                        {
                            int nAction = 0;
                            if (nAction < nActions)
                            {
                                do
                                {
                                    if (aActions.element[nAction].m_nAction == nSelected)
                                        pAction = &aActions.element[nAction];
                                    nAction++;
                                } while (nAction < nActions);
                            }
                        }
                        if (pAction == NULL)
                            pAction = &aActions.element[0];
                        sName = pAction->m_sName;
#ifndef _XBOX
                        int nStackSize = pAction->m_nStackSize;
                        if (nStackSize > 1)
                            sName.Format("%s (%d)", sName.CStr(), nStackSize);
#endif
                    }
                    else
                    {
                        sName = pTarget->GetName();
                        if (pTarget->AsSWCPlaceable() != NULL)
                        {
                            CSWSPlaceable* pPlaceable =
                                pTarget->AsSWCPlaceable()->GetServerPlaceable();
                            if (pPlaceable != NULL &&
                                pPlaceable->m_bHasInventory &&
                                pPlaceable->AsSWSPlaceable()->GetItemCount(0) == 0)
                            {
                                int bUseSearch = 0;
                                g_pRules->Get2DArrays()->m_pPlaceables->GetINTEntry(
                                    pTarget->AsSWCPlaceable()->GetAppearance(),
                                    "UseSearch", &bUseSearch);
                                if (bUseSearch)
                                {
                                    sName = sName + " (" +
                                            g_pAppManager->GetClientExoApp()->GetGUIString(STRREF_NOTHING) +
                                            ")";
                                }
                                else
                                {
                                    sName = sName + " (" +
                                            g_pAppManager->GetClientExoApp()->GetGUIString(STRREF_EMPTY) +
                                            ")";
                                }
                            }
                        }
                    }
                }
            }
            m_cHealthBar.SetMaxValue(pServerObject->GetMaxHitPoints(1));
            m_cHealthBar.SetCurValue(pServerObject->GetCurrentHitPoints(0));
        }
    }
    SetNameLabel(sName);
    m_cNameLabel.m_cText.m_cParams.m_fAlpha = 1.0f;
}
#endif

#ifndef _XBOX
void CSWGuiTargetActionMenu::Draw(float fElapsed)  // PC 0x006DD380
{
    if (m_bVisible != 0)
    {
        AurGUIStartLayer();
        if (AurGUISetupViewport(m_cExtent.m_nLeft, m_cExtent.m_nTop,
                                m_cExtent.m_nWidth, m_cExtent.m_nHeight,
                                CSWGuiDrawable::NO_COLOURING, false, 1.0f) != 0)
        {
            m_cHealthBackground.Draw(fElapsed);
            m_cNameBackground.Draw(fElapsed);
            m_cNameLabel.Draw(fElapsed);
            m_cHealthBar.Draw(fElapsed);
            for (int nAction = 0; nAction < 3; nAction++)
            {
                m_aActions[nAction].Draw(fElapsed);
            }
            AurGUICloseViewport();
        }
        AurGUIStopLayer();
    }
}
#endif

#ifndef _XBOX
CSWGuiControl* CSWGuiTargetActionMenu::HitCheckMouse(int nX, int nY)  // PC 0x006DFF50
{
    CSWGuiControl* pHit = NULL;

    if (m_bVisible != 0)
    {
        nX -= m_cExtent.m_nLeft;
        nY -= m_cExtent.m_nTop;
        if (m_cNameLabel.HitCheckMouse(nX, nY) != 0)
        {
            pHit = &m_cNameLabel;
        }
        else if (m_cHealthBar.HitCheckMouse(nX, nY) != 0)
        {
            pHit = &m_cHealthBar;
        }
        else
        {
            int bActions = 0;
            for (int nList = 0; nList < 3 && !bActions; nList++)
            {
                if (m_aActionLists[nList].num != 0)
                {
                    bActions = 1;
                }
            }
            if (bActions)
            {
                for (int nAction = 0; nAction < 3 && pHit == NULL; nAction++)
                {
                    pHit = m_aActions[nAction].HitCheckMouse(nX, nY);
                }
            }
        }
    }

    if (m_bMouseOver != 0 && pHit == NULL)
    {
        m_fMouseOverTimer = 0.5f;
    }
    m_bMouseOver = pHit != NULL;
    return pHit;
}
#endif

#ifndef _XBOX
void CSWGuiTargetActionMenu::PositionMenu(CSWCCreature* pCreature,  // PC 0x006E0050
                                        CSWCObject* pTarget)
{
    if (pTarget != NULL)
    {
        int bActions = 0;
        m_bVisible = 1;
        for (int nMenu = 0; nMenu < 3 && !bActions; nMenu++)
        {
            if (m_aActionLists[nMenu].num != 0)
                bActions = 1;
        }

        int nOldHeight = m_cExtent.m_nHeight;
        if (bActions)
            m_cExtent.m_nHeight =
                m_aActions[0].m_CSWGuiButton_0.m_cExtent.GetBottom();
        else
            m_cExtent.m_nHeight = m_cHealthBar.m_cExtent.GetBottom();

        if (m_bMouseOver || m_fMouseOverTimer > 0.0f)
        {
            m_cExtent.m_nTop += nOldHeight - m_cExtent.m_nHeight;
        }
        else
        {
            Vector vPoint;
            int nPoint;
            if (pTarget->AsSWCCreature() != NULL)
                nPoint = pTarget->AsSWCCreature()->GetMenuPoint(vPoint);
            else
                nPoint = pTarget->GetReticlePoint(vPoint);

            int nWidth =
                g_pAppManager->GetClientExoApp()->GetSWGuiManager()->m_nWidth;
            int nHeight =
                g_pAppManager->GetClientExoApp()->GetSWGuiManager()->m_nHeight;

            if (nPoint != CSWCObject::RETICLE_POINT_UNAVAILABLE)
            {
                if (nPoint == CSWCObject::RETICLE_POINT_BEHIND)
                {
                    if (nWidth / 2 > vPoint.m_fX)
                        m_cExtent.m_nLeft = m_cBounds.GetRight() - m_cExtent.m_nWidth;
                    else
                        m_cExtent.m_nLeft = m_cBounds.m_nLeft;

                    if (m_cBounds.m_nTop + MENU_POINT_SPACING > vPoint.m_fY)
                        m_cExtent.m_nTop = m_cBounds.GetBottom() -
                            m_cExtent.m_nHeight - MENU_POINT_SPACING;
                    else if (m_cBounds.GetBottom() - MENU_POINT_SPACING < vPoint.m_fY)
                        m_cExtent.m_nTop = m_cBounds.m_nTop;
                    else if (nHeight / 2 > vPoint.m_fY)
                        m_cExtent.m_nTop = nHeight -
                            (int)(m_cExtent.m_nHeight + vPoint.m_fY +
                                  (float)MENU_POINT_SPACING);
                    else
                        m_cExtent.m_nTop = (int)(vPoint.m_fY -
                            (float)MENU_POINT_SPACING - m_cExtent.m_nHeight);
                }
                else
                {
                    float fLeft = vPoint.m_fX - m_cExtent.m_nWidth / 2;
                    if (m_cBounds.m_nLeft > fLeft)
                    {
                        m_cExtent.m_nLeft = m_cBounds.m_nLeft;
                        if (m_cBounds.m_nTop + MENU_POINT_SPACING > vPoint.m_fY)
                            m_cExtent.m_nTop = m_cBounds.m_nTop;
                        else if (m_cBounds.GetBottom() - MENU_POINT_SPACING < vPoint.m_fY)
                            m_cExtent.m_nTop = m_cBounds.GetBottom() -
                                m_cExtent.m_nHeight - MENU_POINT_SPACING;
                        else
                            m_cExtent.m_nTop = (int)(vPoint.m_fY -
                                (float)MENU_POINT_SPACING - m_cExtent.m_nHeight);
                    }
                    else if (vPoint.m_fX + m_cExtent.m_nWidth / 2 >
                             m_cBounds.GetRight())
                    {
                        m_cExtent.m_nLeft = m_cBounds.GetRight() - m_cExtent.m_nWidth;
                        if (m_cBounds.m_nTop + MENU_POINT_SPACING > vPoint.m_fY)
                            m_cExtent.m_nTop = m_cBounds.m_nTop;
                        else if (m_cBounds.GetBottom() - MENU_POINT_SPACING < vPoint.m_fY)
                            m_cExtent.m_nTop = m_cBounds.GetBottom() -
                                m_cExtent.m_nHeight - MENU_POINT_SPACING;
                        else
                            m_cExtent.m_nTop = (int)(vPoint.m_fY -
                                (float)MENU_POINT_SPACING - m_cExtent.m_nHeight);
                    }
                    else
                    {
                        if (m_cBounds.m_nTop + MENU_POINT_SPACING > vPoint.m_fY)
                        {
                            m_cExtent.m_nTop = m_cBounds.m_nTop;
                            m_cExtent.m_nLeft = (int)fLeft;
                        }
                        else if (m_cBounds.GetBottom() - MENU_POINT_SPACING < vPoint.m_fY)
                        {
                            m_cExtent.m_nTop = m_cBounds.GetBottom() -
                                m_cExtent.m_nHeight - MENU_POINT_SPACING;
                            m_cExtent.m_nLeft = (int)fLeft;
                        }
                        else
                        {
                            m_cExtent.m_nLeft = (int)vPoint.m_fX - m_cExtent.m_nWidth / 2;
                            m_cExtent.m_nTop = (int)vPoint.m_fY -
                                m_cExtent.m_nHeight - MENU_POINT_SPACING;
                        }
                    }
                }
            }

            m_cExtent.m_nLeft = max(m_cBounds.m_nLeft,
                min(m_cBounds.GetRight() - m_cExtent.m_nWidth, m_cExtent.m_nLeft));
            m_cExtent.m_nTop = max(m_cBounds.m_nTop,
                min(m_cBounds.GetBottom() - m_cExtent.m_nHeight, m_cExtent.m_nTop));
        }
    }
    else
    {
        m_bVisible = 0;
    }
}
#endif
#ifndef _XBOX
void CSWGuiTargetActionMenu::SelectNextAction(int nMenu)  // PC 0x006E0570
{
    if (nMenu >= 0 && nMenu < 3 && m_aActionLists[nMenu].num != 0)
    {
        CSWParty* pParty = g_pAppManager->m_pClientExoApp->GetSWParty();
        pParty->GetPlayerCharacter();

        CSWGuiInterfaceAction* pNext = NULL;
        int nSelected = m_aSelectedActions[m_nTargetType][nMenu];
        if (nSelected != CSWGuiInterfaceAction::INVALID_ACTION)
        {
            for (int nAction = 0; nAction < m_aActionLists[nMenu].num;
                 nAction++)
            {
                if (m_aActionLists[nMenu].element[nAction].m_nAction ==
                    nSelected)
                {
                    if (nAction + 1 < m_aActionLists[nMenu].num)
                    {
                        pNext = &m_aActionLists[nMenu].element[nAction + 1];
                    }
                    else
                    {
                        pNext = &m_aActionLists[nMenu].element[0];
                    }
                    break;
                }
            }
        }

        if (m_aActionLists[nMenu].num > 1)
        {
            if (pNext == NULL)
            {
                pNext = &m_aActionLists[nMenu].element[1];
            }
            m_aSelectedActions[m_nTargetType][nMenu] = pNext->m_nAction;
            g_pAppManager->m_pClientExoApp->GetSWGuiManager()->PlayGuiSound(
                CSWGuiManager::SOUND_ACTION_CHANGE);
        }
    }
}
#endif

#ifndef _XBOX
void CSWGuiTargetActionMenu::SelectPrevAction(int nMenu)  // PC 0x006E0640
{
    if (nMenu >= 0 && nMenu < 3 && m_aActionLists[nMenu].num != 0)
    {
        g_pAppManager->m_pClientExoApp->GetSWParty()->GetPlayerCharacter();

        CSWGuiInterfaceAction* pPrev = NULL;
        int nSelected = m_aSelectedActions[m_nTargetType][nMenu];
        if (nSelected != -1)
        {
            for (int nAction = 0; nAction < m_aActionLists[nMenu].num;
                 nAction++)
            {
                if (m_aActionLists[nMenu].element[nAction].m_nAction ==
                    nSelected)
                {
                    if (nAction > 0)
                    {
                        pPrev = &m_aActionLists[nMenu].element[nAction - 1];
                    }
                    else
                    {
                        pPrev = &m_aActionLists[nMenu]
                                     .element[m_aActionLists[nMenu].num - 1];
                    }
                    break;
                }
            }
        }

        if (m_aActionLists[nMenu].num > 1)
        {
            if (pPrev == NULL)
            {
                pPrev = &m_aActionLists[nMenu]
                             .element[m_aActionLists[nMenu].num - 1];
            }
            m_aSelectedActions[m_nTargetType][nMenu] = pPrev->m_nAction;
            g_pAppManager->m_pClientExoApp->GetSWGuiManager()->PlayGuiSound(7);
        }
    }
}
#endif

#ifndef _XBOX
int CSWGuiCharButton::DisplayToolTip()  // PC 0x006E09C0
{
    if (m_bEnabled == 0)
    {
        return 0;
    }

    UpdateToolTipText();
#ifndef _XBOX
    g_pGuiMan->DisplayToolTip(m_sToolTipText);
#endif
#ifndef _XBOX
    m_bToolTipShown = 1;
#endif
    return 1;
}
#endif

#ifndef _XBOX
void CSWGuiCharButton::Draw(float fElapsed)  // PC 0x006E0A00
{
#ifndef _XBOX
    if (m_bToolTipShown != 0 && m_pCharacter != NULL &&
        m_pCharacter->m_bToolTipDirty != 0)
    {
        UpdateToolTipText();
        g_pGuiMan->ChangeToolTipText(m_sToolTipText);
        m_pCharacter->m_bToolTipDirty = 0;
    }
#endif
    CSWGuiButton::Draw(fElapsed);
}
#endif
// PC 0x006DD720
void CSWGuiMainInterfaceChar::Show(int bShow, unsigned char nState,
                                   unsigned char nBuffCount,
                                   unsigned char nDebuffCount)
{
    m_aLabels[LABEL_CHAR].SetEnabledFlag(bShow & 1);
    m_aLabels[LABEL_BACK].SetEnabledFlag(bShow & 1);
    m_aBars[BAR_VIT].SetEnabledFlag(bShow & 1);
    m_aBars[BAR_FORCE].SetEnabledFlag(bShow & 1);
    m_aLabels[LABEL_DEBILATATED].SetEnabledFlag(bShow != 0 && (nState & PORTRAIT_DEBILITATED) != 0);
    m_aLabels[LABEL_DISABLED].SetEnabledFlag(bShow != 0 && (nState & PORTRAIT_DEAD) != 0);
    m_aLabels[LABEL_LEVELUP].SetEnabledFlag(bShow != 0 && (nState & PORTRAIT_LEVEL_UP) != 0);
#ifdef _XBOX
    m_aLabels[LABEL_LVLUPBG].SetEnabledFlag(bShow != 0 && (nState & PORTRAIT_LEVEL_UP) != 0);
#endif
    m_nBuffCount = nState != 0 ? 0 : nBuffCount;
    m_nDebuffCount = nState != 0 ? 0 : nDebuffCount;
#ifndef _XBOX
    m_cCharButton.SetEnabledFlag(bShow & 1);
#endif
}
// PC 0x006DDBC0
void CSWGuiMainInterfaceStatus::Initialize(CSWGuiMainInterface* pInterface,
                                           unsigned char nStatusEffect)
{
    CExoString sLabelName;

    m_nStatusEffect = nStatusEffect;
#ifdef _XBOX
    m_bWasEnabled = 0;
    m_bPaused = 0;
#endif

    switch (nStatusEffect)
    {
    case GUI_STATUS_JOURNAL:
        sLabelName = "LBL_JOURNAL";
        break;

    case GUI_STATUS_CREDITS:
        sLabelName = "LBL_CASH";
        break;

    case GUI_STATUS_XP:
        sLabelName = "LBL_PLOTXP";
        break;

    case GUI_STATUS_STEALTH_XP:
        sLabelName = "LBL_STEALTHXP";
        break;

    case GUI_STATUS_DARK_SHIFT:
        sLabelName = "LBL_DARKSHIFT";
        break;

    case GUI_STATUS_LIGHT_SHIFT:
        sLabelName = "LBL_LIGHTSHIFT";
        break;

    case 6:
        m_cLabel.m_bEnabled = 0;
        return;

    case GUI_STATUS_ITEM_RECEIVED:
        sLabelName = "LBL_ITEMRCVD";
        break;

    case GUI_STATUS_ITEM_LOST:
        sLabelName = "LBL_ITEMLOST";
        break;
    }

    pInterface->InitControl(m_cLabel, sLabelName, 1);
    m_cLabel.m_bEnabled = 0;
    m_cLabel.m_cBorder.m_cParams.SetPulsingAlpha(1, 0, 1);
}
// PC 0x006DDCC0
void CSWGuiMainInterfaceStatus::Update(float fElapsed)
{
#ifdef _XBOX
    if (m_bPaused)
    {
        if (fElapsed > 0.0f)
        {
            m_cLabel.m_bEnabled = m_bWasEnabled;
            m_bPaused = 0;
        }
    }
    else if (fElapsed == 0.0f)
    {
        if (g_pAppManager->m_pClientExoApp->GetInGameGui()->IsPauseGUIVisible())
        {
            m_bWasEnabled = m_cLabel.m_bEnabled;
            m_bPaused = 1;
            m_cLabel.m_bEnabled = 0;
        }
    }
#endif
    if (m_cLabel.m_bEnabled)
    {
#ifdef _XBOX
        if (m_bDisplayed && fElapsed > 0.0f)
#else
        if ((m_nDisplayFlags & STATUS_EFFECT_DISPLAYED) && fElapsed > 0.0f)
#endif
        {
            if (m_nStatusEffect != 0)
            {
                g_pAppManager->m_pClientExoApp->GetInGameGui()->UpdateStatus(101,
                                                                             0);
            }
#ifdef _XBOX
            m_bDisplayed = 0;
#else
            m_nDisplayFlags &= ~STATUS_EFFECT_DISPLAYED;
#endif
        }

        m_fDisplayTime -= fElapsed;
        if (m_fDisplayTime <= 0.0f)
        {
            m_cLabel.m_bEnabled = 0;
        }
    }
}
// PC 0x006E0AB0
CSWGuiMainInterfaceStatus::CSWGuiMainInterfaceStatus()
{
}

#ifdef _XBOX
void CSWGuiMainInterfaceAction::Show(int bActive, int bEmpty, int bArrow)
{
    if (m_fSlideTimer > 0.0f)
    {
        if (!bActive)
        {
            StopSlide();
        }
    }
    else
    {
        m_lblIcon.SetExtent(bActive ? m_cIconExtent : m_cIconBExtent);
    }
    m_lbhBorder.SetExtent(bActive ? m_cBorderExtent : m_cBorderBExtent);
    m_lbhBorder.m_cBorder.m_cParams.SetFillImage(
        bEmpty ? "lbl_mibox03" : "lbl_mibox01", 0);
    m_lbhArrow.SetEnabledFlag(bArrow);
    if (bArrow)
    {
        m_lbhArrow.SetActive(bActive);
        m_lbhArrow.SetExtent(bActive ? m_cArrowExtent : m_cArrowBExtent);
    }
    if (bEmpty)
    {
        SetIcon("", 1, 0);
    }
}
#else
void CSWGuiMainInterfaceAction::Show(int bEmpty, int bEnabled)  // PC 0x006DF6B0
{
    Swguimaininterfacepc2Trace1();
    float fBorderAlpha;
    float fHiliteAlpha;

    if (bEmpty != 0)
    {
        {
            CExoString sEmpty("");
            CResRef cEmpty("");

            SetIcon(cEmpty, sEmpty, 1, 0);
        }
        fBorderAlpha = 0.5f;
    }
    else
    {
        fBorderAlpha = 1.0f;
    }
    m_CSWGuiButton_0.m_cBorder.m_cParams.m_fHilightAlpha = fBorderAlpha;

    if (bEmpty != 0)
    {
        fHiliteAlpha = 0.5f;
    }
    else
    {
        fHiliteAlpha = 1.0f;
    }
    m_CSWGuiButton_0.m_cHilite.m_cParams.m_fHilightAlpha = fHiliteAlpha;

    m_CSWGuiButton_388.SetEnabledFlag(bEnabled);
    m_CSWGuiButton_54C.SetEnabledFlag(bEnabled);
}
#endif

#ifdef _XBOX
CSWGuiMainInterfaceAction::~CSWGuiMainInterfaceAction()
{
}
#else
CSWGuiMainInterfaceAction::~CSWGuiMainInterfaceAction()  // PC 0x006DF7D0
{
    Swguimaininterfacepc2Trace1();
}
#endif

#ifndef _XBOX
void CSWGuiTargetActionMenu::PopulateMenus(CSWCCreature* pCreature,  // PC 0x006E2FA0
                                         int bDisabled, CSWCObject* pTarget)
{
    m_nTargetType = pCreature->GetTargetInterfaceTargetType(pTarget);
    for (int nMenu = 0; nMenu < 3; nMenu++)
    {
        for (int nAction = 0; nAction < m_aActionLists[nMenu].num; nAction++)
        {
            m_aActionLists[nMenu][nAction].Clear();
        }
        m_aActionLists[nMenu].num = 0;
        pCreature->GetTargetActions(pTarget, nMenu, m_aActionLists[nMenu]);
    }

    for (int nMenu = 0; nMenu < 3; nMenu++)
    {
        int bFound = 0;
        int nAction;
        for (nAction = 0; nAction < m_aActionLists[nMenu].num; nAction++)
        {
            if (m_aActionLists[nMenu][nAction].m_nAction ==
                m_aSelectedActions[m_nTargetType][nMenu])
            {
                bFound = 1;
                break;
            }
        }
        if (bFound)
        {
            m_aActions[nMenu].Show(0, m_aActionLists[nMenu].num > 1);
            m_aActions[nMenu].SetIcon(
                m_aActionLists[nMenu][nAction].m_resIcon,
                m_aActionLists[nMenu][nAction].m_sName,
                m_aActionLists[nMenu][nAction].m_bEnabled &&
                    bDisabled == 0, 0);
        }
        else if (m_aActionLists[nMenu].num > 0)
        {
            m_aActions[nMenu].Show(0, m_aActionLists[nMenu].num > 1);
            m_aActions[nMenu].SetIcon(
                m_aActionLists[nMenu][0].m_resIcon,
                m_aActionLists[nMenu][0].m_sName,
                m_aActionLists[nMenu][0].m_bEnabled &&
                    bDisabled == 0, 0);
        }
        else
        {
            m_aActions[nMenu].Show(1, 0);
        }
    }
    UpdateNameLabel(pTarget);
    PositionMenu(pCreature, pTarget);
}
#endif

#ifndef _XBOX
void CSWGuiTargetActionMenu::DoTargetAction(int nMenu, int bUnknown)  // PC 0x006E0710
{
    if (m_pMainInterface != NULL)
    {
        m_pMainInterface->ResetPartyMemberItemUse();
    }
    CExoArrayList<CSWGuiInterfaceAction>& lActions = m_aActionLists[nMenu];
    if (lActions.num != 0)
    {
        CClientExoApp* pClientApp = g_pAppManager->GetClientExoApp();
        CSWCCreature* pPlayer = pClientApp->GetSWParty()->GetPlayerCharacter();
        CSWGuiInterfaceAction* pAction = NULL;
        int nSelected = m_aSelectedActions[m_nTargetType][nMenu];
        if (nSelected != CSWGuiInterfaceAction::INVALID_ACTION)
        {
            for (int nAction = 0; nAction < lActions.num; nAction++)
            {
                if (lActions[nAction].m_nAction == nSelected)
                {
                    pAction = &lActions[nAction];
                    break;
                }
            }
        }
        if (pAction == NULL)
            pAction = &lActions[0];

        if (pPlayer != NULL && pAction != NULL)
        {
            CSWCObject* pGameObject = g_pAppManager->GetClientExoApp()->GetGameObject(
                pAction->m_oidTarget);
            if (pGameObject != NULL)
            {
                CSWCObject* pObject = pGameObject->AsSWCObject();
                if (pObject != NULL)
                {
                    if (pAction->m_bEnabled && pAction->m_pfnAction != NULL)
                    {
                        int bStartCombat = 0;
                        if (pPlayer->m_bCombatMode == 0)
                        {
                            if (pClientApp->GetCombatMode() == 0 &&
                                m_nTargetType == CSWCCreature::TARGET_INTERFACE_HOSTILE)
                                bStartCombat = 1;
                            pPlayer->ClearAllQueuedCombatActions();
                            if ((m_nTargetType == CSWCCreature::TARGET_INTERFACE_DOOR_OR_CONTAINER &&
                                 pAction->m_nAction == CSWGuiInterfaceAction::ACTION_BASH) ||
                                m_nTargetType == CSWCCreature::TARGET_INTERFACE_HOSTILE)
                            {
                                m_pMainInterface->m_fCombatMessageTime = -1.0f;
                                m_pMainInterface->SetCombatMessage(
                                    CSWGuiMainInterface::STRREF_COMBAT_MODE_ENGAGED);
                            }
                        }
                        else if (pClientApp->GetAutoPaused())
                        {
                            pClientApp->GetInGameGui()->SetPaused(
                                1, CGuiInGame::PAUSE_REASON_ACTION_SELECTED);
                        }
                        g_pAppManager->GetClientExoApp()->GetSWGuiManager()->PlayGuiSound(
                            CSWGuiManager::GUISOUND_WORLD_ACTION);
                        (pObject->*pAction->m_pfnAction)(pAction->m_nAction, pPlayer);
                        m_aActions[nMenu].Pulse();
                        if (pPlayer->m_bCombatMode == 0)
                        {
                            if (bStartCombat)
                                pPlayer->SetCombatMode(1);
                            if (pPlayer->GetServerCreature() != NULL)
                                pPlayer->GetServerCreature()->ClearAllActions(0);
                        }
                    }
                    else
                    {
                        if (pAction->m_nDisabledReason != CSWGuiInterfaceAction::DISABLED_REASON_NONE)
                        {
                            switch (pAction->m_nDisabledReason)
                            {
                            case CSWGuiInterfaceAction::DISABLED_REASON_FORCE_POINTS:
                                m_nFeedbackStrRef = CSWGuiMainInterface::STRREF_FORCE_DEPLETED;
                                break;
                            case CSWGuiInterfaceAction::DISABLED_REASON_FORBIDDEN_ITEM:
                                m_nFeedbackStrRef = CSWGuiMainInterface::STRREF_RESTRICTED_BY_ARMOR;
                                break;
                            case CSWGuiInterfaceAction::DISABLED_REASON_REQUIRED_ITEM:
                                m_nFeedbackStrRef = CSWGuiMainInterface::STRREF_MISSING_ITEM;
                                break;
                            case CSWGuiInterfaceAction::DISABLED_REASON_RANGE:
                                m_nFeedbackStrRef = CSWGuiMainInterface::STRREF_TARGET_TOO_CLOSE;
                                break;
                            case CSWGuiInterfaceAction::DISABLED_REASON_FULL_HEALTH:
                                m_nFeedbackStrRef = CSWGuiMainInterface::STRREF_FULL_HEALTH;
                                break;
                            case CSWGuiInterfaceAction::DISABLED_REASON_DEAD:
                                m_nFeedbackStrRef = CSWGuiMainInterface::STRREF_PC_DEAD;
                                break;
                            }
                            m_fFeedbackTimer = 5.0f;
                        }
                        g_pAppManager->GetClientExoApp()->GetSWGuiManager()->PlayGuiSound(
                            CSWGuiManager::GUISOUND_ACTION_DISABLED);
                    }
                }
            }
        }
    }
}
#endif
// PC 0x006E31A0
void CSWGuiMainInterfaceChar::Initialize(CSWGuiMainInterface* pMainInterface,
                                        int nCharacter)
{
    CExoString sControl;
    sControl.Format("LBL_CHAR%d", nCharacter);
    pMainInterface->InitControl(m_aLabels[LABEL_CHAR], sControl, 1);
    sControl.Format("PB_VIT%d", nCharacter);
    pMainInterface->InitControl(m_aBars[BAR_VIT], sControl, 1);
    sControl.Format("PB_FORCE%d", nCharacter);
    pMainInterface->InitControl(m_aBars[BAR_FORCE], sControl, 1);
    sControl.Format("LBL_LEVELUP%d", nCharacter);
    pMainInterface->InitControl(m_aLabels[LABEL_LEVELUP], sControl, 1);
    sControl.Format("LBL_BACK%d", nCharacter);
    pMainInterface->InitControl(m_aLabels[LABEL_BACK], sControl, 1);
    sControl.Format("LBL_DEBILATATED%d", nCharacter);
    pMainInterface->InitControl(m_aLabels[LABEL_DEBILATATED], sControl, 1);
    sControl.Format("LBL_DISABLED%d", nCharacter);
    pMainInterface->InitControl(m_aLabels[LABEL_DISABLED], sControl, 1);
    sControl.Format("LBL_CMBTEFCTINC%d", nCharacter);
    pMainInterface->InitControl(m_cBuffLabel, sControl, 0);
    sControl.Format("LBL_CMBTEFCTRED%d", nCharacter);
    pMainInterface->InitControl(m_cDebuffLabel, sControl, 0);
#ifndef _XBOX
    sControl.Format("BTN_CHAR%d", nCharacter);
    pMainInterface->InitControl(m_cCharButton, sControl, 1);
#endif

    m_aLabels[LABEL_LEVELUP].m_cBorder.m_cParams.SetPulsingAlpha(1, 0, 1);
    CSWGuiExtent cPortrait = m_aLabels[LABEL_CHAR].m_cExtent;
    CSWGuiExtent cExtent = m_cBuffLabel.m_cExtent;
    cExtent.m_nLeft -= cPortrait.m_nLeft;
    cExtent.m_nTop -= cPortrait.m_nTop;
    m_cBuffLabel.SetExtent(cExtent);
    cExtent = m_cDebuffLabel.m_cExtent;
    cExtent.m_nLeft -= cPortrait.m_nLeft;
    cExtent.m_nTop -= cPortrait.m_nTop;
    m_cDebuffLabel.SetExtent(cExtent);
    float fScaleX = pMainInterface->m_fScaleX;
    cExtent = m_aBars[BAR_VIT].m_cExtent;
    m_nStartX = (short)(cExtent.m_nLeft * fScaleX);
    float fScaleY = pMainInterface->m_fScaleY;
    m_nStartY = (short)(cExtent.m_nTop * fScaleY);
#ifndef _XBOX
    m_cCharButton.m_nIndex = nCharacter;
    m_cCharButton.SetFocusable(0);
    m_cCharButton.AddEvent(GUIEVENT_ACTIVATE, pMainInterface,
        (PFNGUIEVENT)&CSWGuiMainInterface::OnCharacterClicked);
    m_cCharButton.AddEvent(GUIEVENT_RIGHT_CLICK, pMainInterface,
        (PFNGUIEVENT)&CSWGuiMainInterface::OnCharacterRightClicked);
    m_cCharButton.AddEvent(GUIEVENT_MOUSE_ENTER, pMainInterface,
        (PFNGUIEVENT)&CSWGuiMainInterface::OnCharacterEntered);
    m_cCharButton.AddEvent(GUIEVENT_MOUSE_EXIT, pMainInterface,
        (PFNGUIEVENT)&CSWGuiMainInterface::OnCharacterExited);
    m_cCharButton.m_pCharacter = this;
    m_nCurHealth = 0;
    m_nMaxHealth = 0;
    m_nCurForce = 0;
    m_nMaxForce = 0;
    m_nLevel = 0;
    m_nXPNeeded = 0;
    m_bToolTipDirty = 0;
#endif
}
// PC 0x00679DC0
CSWGuiMainInterfaceChar::CSWGuiMainInterfaceChar()
{
}
// PC 0x00679E80
CSWGuiMainInterfaceChar::~CSWGuiMainInterfaceChar()
{
    Swguimaininterfacepc2Trace1();
}

#ifndef _XBOX
CSWGuiControl* CSWGuiMainInterface::HitCheckMouse(int nX, int nY)  // PC 0x006E0AC0
{
    CSWGuiControl* pControl = m_cTargetActionMenu.HitCheckMouse(nX, nY);

    if (pControl == NULL)
    {
        return CSWGuiPanel::HitCheckMouse(nX, nY);
    }
    return pControl;
}
#endif
// PC 0x006E0AF0
void CSWGuiMainInterface::Update(float fElapsed)
{
    for (int nText = m_lFloatyText.num - 1; nText >= 0; --nText)
    {
#ifdef _XBOX
        if (m_lFloatyText[nText] != NULL)
            m_lFloatyText[nText]->Update(fElapsed);
#else
        if (m_lFloatyText.element[nText] != NULL)
            m_lFloatyText.element[nText]->Update(fElapsed);
#endif
    }

    if (m_fCombatMessageTime != -1.0f)
    {
        fElapsed = m_fCombatMessageTime - fElapsed;
        m_fCombatMessageTime = fElapsed;
        if (fElapsed <= 0.0f)
        {
            m_fCombatMessageTime = -1.0f;
            CClientExoApp* pClient = g_pAppManager->GetClientExoApp();
            int nMember = pClient->GetSWParty()->GetIndex(pClient->GetPlayerCreatureId());
            if (nMember >= 0)
            {
                CSWParty* pParty = pClient->GetSWParty();
#ifdef _XBOX
                EXOXBOXASSERT(0 <= nMember && nMember < 3);
                pParty->m_aPartyMemberData[nMember].SetCombatMessage(42475);
#else
                pParty->m_aPartyMemberData[nMember].SetCombatMessage(STRREF_COMBAT_MODE_ENGAGED);
#endif
            }
        }
        else
        {
            float fAlpha = 1.0f;
            float fFraction = fElapsed / m_fCombatMessageDuration;
            if (fFraction < 0.5)
                fAlpha = fFraction + fFraction;
            m_lblCmbtmodemsg.m_cText.m_cParams.m_fAlpha = fAlpha;
        }
    }

#ifdef _XBOX
    CSWSCreature* pCreature = g_pAppManager->GetServerExoApp()->GetCreatureByGameObjectID(
        g_pAppManager->GetServerExoApp()->GetPlayerCreatureId());
    if (!m_cActionDescriptionState.m_bUnknown)
    {
        if (pCreature != NULL &&
            pCreature->m_oidAttemptedAttackTarget == OBJECT_INVALID &&
            pCreature->m_oidAttemptedSpellTarget == OBJECT_INVALID)
        {
            m_cActionDescriptionState.m_bTargetChanged = 0;
        }
    }
    else
    {
        m_cActionDescriptionState.m_bUnknown = 0;
    }
    m_lblSolomode.m_bEnabled = g_pAppManager->GetServerExoApp()->GetPartyTable()->m_bSoloMode;
#else
    CSWCCreature* pPlayer = g_pAppManager->GetClientExoApp()->GetPlayerCreature();
    m_tbSolo.m_bEnabled = g_pAppManager->GetServerExoApp()->GetPartyTable()->m_nNumMembers > 0;
    m_tbSolo.m_bToggled = g_pAppManager->GetServerExoApp()->GetPartyTable()->m_bSoloMode;
    m_tbStealth.m_bEnabled = pPlayer != NULL ? pPlayer->IsStealthCapable() : 0;
    m_tbStealth.m_bToggled = pPlayer != NULL ? pPlayer->m_nStealthState : 0;
    m_btnSwapWeapons.m_bEnabled = pPlayer != NULL ? pPlayer->CanSwapWeapons() : 0;
    m_tbPause.m_bToggled = g_pAppManager->GetClientExoApp()->GetPausedByCombat();

    CClientOptions* pOptions = g_pAppManager->GetClientExoApp()->GetClientOptions();
    int bShowMenus = pOptions != NULL ? !pOptions->m_bHideInGameGUI : 1;
    m_btnEqu.SetEnabledFlag(bShowMenus);
    m_btnInv.SetEnabledFlag(bShowMenus);
    m_btnChar.SetEnabledFlag(bShowMenus);
    m_btnAbi.SetEnabledFlag(bShowMenus);
    m_btnMsg.SetEnabledFlag(bShowMenus);
    m_btnJou.SetEnabledFlag(bShowMenus);
    m_btnMap.SetEnabledFlag(bShowMenus);
    m_btnOpt.SetEnabledFlag(bShowMenus);
    m_lblMenubg.SetEnabledFlag(bShowMenus);
#endif
}

#ifdef _XBOX
void CSWGuiMainInterface::UpdateActionDescription(CSWGuiControl* pControl)
{
    CExoString sDescription;
    float fAlpha = 1.0f;

    if (m_fActionDescriptionTimer > 0.0f)
    {
        float fFraction = m_fActionDescriptionTimer / 5.0f;
        if (fFraction < 0.5f)
        {
            fAlpha = fFraction + fFraction;
        }
        sDescription = g_pAppManager->GetClientExoApp()->GetGUIString(
            m_nActionDescription);
    }
    else
    {
        sDescription = m_aPersonalActionLists[m_aSelectedMenu[m_nTargetType]]
                           .element[m_nSelectedAction].m_sName;
    }
    m_cActionText.m_cText.m_cParams.SetText(sDescription);
    m_cActionText.m_cText.m_cParams.m_fAlpha = fAlpha;
}
#else
void CSWGuiMainInterface::UpdateActionDescription()  // PC 0x006E0D90
{
    CExoString sDescription;
    float fAlpha = 1.0f;

    if (m_fActionDescriptionTimer > 0.0f)
    {
        float fFraction = m_fActionDescriptionTimer / 5.0f;
        if (fFraction < 0.5f)
        {
            fAlpha = fFraction + fFraction;
        }
        sDescription = g_pAppManager->GetClientExoApp()->GetGUIString(
            m_nActionDescription);
    }
    else if (m_aPersonalActions[m_nDefaultAction].GetActive() &&
             m_nDefaultAction >= 0)
    {
        int nCount = m_aPersonalActionLists[m_nDefaultAction].num;
        if (nCount > 0)
        {
            CSWGuiInterfaceAction* pAction = NULL;
            int nSelected = m_aSelectedPersonalActions[m_nDefaultAction];
            if (nSelected != CSWGuiInterfaceAction::INVALID_ACTION)
            {
                int nAction = 0;
                if (nAction < nCount)
                {
                    do
                    {
                        if (m_aPersonalActionLists[m_nDefaultAction]
                                .element[nAction].m_nAction == nSelected)
                        {
                            pAction = &m_aPersonalActionLists[m_nDefaultAction]
                                           .element[nAction];
                        }
                        nAction++;
                    } while (nAction < nCount);
                }
            }
            if (pAction == NULL)
            {
                pAction = &m_aPersonalActionLists[m_nDefaultAction].element[0];
            }
            sDescription = pAction->m_sName;
#ifndef _XBOX
            int nStackSize = pAction->m_nStackSize;
            if (nStackSize > 1)
            {
                sDescription.Format("%s (%d)", sDescription.CStr(), nStackSize);
            }
#endif
        }
    }
    SetActionDescription(sDescription);
    m_cActionText.m_cText.m_cParams.m_fAlpha = fAlpha;
}
#endif

#ifdef _XBOX
void CSWGuiMainInterface::PopulateMenus(int bUnknown)
{
    CSWCCreature* pCreature =
        g_pAppManager->GetClientExoApp()->GetSWParty()->GetPlayerCharacter();
    CSWSCreature* pServerCreature =
        pCreature != NULL ? pCreature->GetServerCreature() : NULL;
    CSWCObject* pTarget =
        g_pAppManager->GetClientExoApp()->GetGameObject(m_nTargetId);
    CSWCObject* pTargetObject = NULL;
    int bDisabled = 1;
    int nMenu;
    int nAction;

    if (pServerCreature != NULL && !pServerCreature->GetDead() &&
        !pServerCreature->GetDeadTemp())
    {
        bDisabled = 0;
    }
    if (pCreature->m_nId != m_oidPlayerCharacter)
    {
        for (nMenu = 0; nMenu < 5; nMenu++)
        {
            m_aSelectedMenu[nMenu] = 0;
        }
        m_oidPlayerCharacter = pCreature->m_nId;
    }
    if (pTarget != NULL)
    {
        pTargetObject = pTarget->AsSWCObject();
    }

    int nOldTargetType = m_nTargetType;
    m_nTargetType = pCreature->GetInterfaceTargetType(pTargetObject);
    if (m_nTargetType != nOldTargetType)
    {
        if (nOldTargetType == 4 &&
            !g_pAppManager->GetClientExoApp()->GetTargetChanging())
        {
            pCreature->SetCombatMode(0);
        }
        m_aSelectedMenu[nOldTargetType] = 0;
    }

    for (nMenu = 0; nMenu < 6; nMenu++)
    {
        for (nAction = 0; nAction < m_aPersonalActionLists[nMenu].num;
             nAction++)
        {
            m_aPersonalActionLists[nMenu][nAction].Clear();
        }
        m_aPersonalActionLists[nMenu].num = 0;
        pCreature->GetTargetActions(pTargetObject, nMenu,
                                    m_aPersonalActionLists[nMenu]);
    }

    int bFound;
    if (m_nDefaultAction != CSWGuiInterfaceAction::INVALID_ACTION)
    {
        bFound = 0;
        for (nMenu = 0; nMenu < 6 && !bFound; nMenu++)
        {
            for (nAction = 0;
                 nAction < m_aPersonalActionLists[nMenu].num && !bFound;
                 nAction++)
            {
                if (m_aPersonalActionLists[nMenu][nAction].m_nAction ==
                    m_nDefaultAction)
                {
                    m_aSelectedMenu[m_nTargetType] = nMenu;
                    m_aSelectedPersonalActions[m_nTargetType][nMenu] =
                        m_nDefaultAction;
                    m_nDefaultAction = CSWGuiInterfaceAction::INVALID_ACTION;
                    bFound = 1;
                }
            }
        }
        if (!bFound)
        {
            m_aSelectedMenu[m_nTargetType] = 0;
            m_nDefaultAction = CSWGuiInterfaceAction::INVALID_ACTION;
        }
    }
    else if (m_aSelectedMenu[m_nTargetType] == 0xff)
    {
        m_aSelectedMenu[m_nTargetType] = 0;
    }

    if (m_aPersonalActionLists[m_aSelectedMenu[m_nTargetType]].num == 0)
    {
        bFound = 0;
        m_aSelectedMenu[m_nTargetType] = 0;
        for (nMenu = 5; nMenu >= 0; nMenu--)
        {
            if (m_aPersonalActionLists[nMenu].num > 0)
            {
                m_aSelectedMenu[m_nTargetType] = nMenu;
                bFound = 1;
            }
        }
        if (!bFound)
        {
            m_aSelectedMenu[m_nTargetType] = 0xff;
        }
    }

    for (nMenu = 0; nMenu < 6; nMenu++)
    {
        bFound = 0;
        for (nAction = 0; nAction < m_aPersonalActionLists[nMenu].num;
             nAction++)
        {
            if (m_aPersonalActionLists[nMenu][nAction].m_nAction ==
                m_aSelectedPersonalActions[m_nTargetType][nMenu])
            {
                bFound = 1;
                break;
            }
        }
        if (bFound)
        {
            if (nMenu == m_aSelectedMenu[m_nTargetType])
            {
                m_nSelectedAction = nAction;
            }
            m_aPersonalActions[nMenu].Show(
                nMenu == m_aSelectedMenu[m_nTargetType], 0,
                m_aPersonalActionLists[nMenu].num > 1);
            m_aPersonalActions[nMenu].SetIcon(
                m_aPersonalActionLists[nMenu][nAction].m_resIcon,
                m_aPersonalActionLists[nMenu][nAction].m_bEnabled &&
                    bDisabled == 0, 0);
        }
        else
        {
            if (nMenu == m_aSelectedMenu[m_nTargetType])
            {
                m_nSelectedAction = 0;
            }
            if (m_aPersonalActionLists[nMenu].num > 0)
            {
                m_aPersonalActions[nMenu].Show(
                    nMenu == m_aSelectedMenu[m_nTargetType], 0,
                    m_aPersonalActionLists[nMenu].num > 1);
                m_aPersonalActions[nMenu].SetIcon(
                    m_aPersonalActionLists[nMenu][0].m_resIcon,
                    m_aPersonalActionLists[nMenu][0].m_bEnabled &&
                        bDisabled == 0, 0);
            }
            else
            {
                m_aPersonalActions[nMenu].Show(0, 1, 0);
            }
        }
    }

    SetActiveControl(NULL, 0);
    m_cActionText.m_cText.m_cParams.SetText("");
    if (m_aSelectedMenu[m_nTargetType] != 0xff)
    {
        SetActiveControl(
            &m_aPersonalActions[m_aSelectedMenu[m_nTargetType]].m_lbhBorder, 0);
    }
}
#else
void CSWGuiMainInterface::PopulateMenus(int bUnknown)  // PC 0x006E3530
{
    CSWCCreature* pCreature =
        g_pAppManager->GetClientExoApp()->GetSWParty()->GetPlayerCharacter();
    CSWSCreature* pServerCreature =
        pCreature != NULL ? pCreature->GetServerCreature() : NULL;
    CSWCObject* pTarget =
        g_pAppManager->GetClientExoApp()->GetGameObject(m_nTargetId);
    CSWCObject* pTargetObject = NULL;
    int bDisabled = 1;

    if (pCreature != NULL)
    {
        if (pServerCreature != NULL && !pServerCreature->GetDead() &&
            !pServerCreature->GetDeadTemp())
        {
            bDisabled = 0;
        }
        if (pCreature->m_nId != m_oidPlayerCharacter)
        {
            m_oidPlayerCharacter = pCreature->m_nId;
            RevertActionBarStates();
        }
        if (pTarget != NULL)
        {
            pTargetObject = pTarget->AsSWCObject();
        }
        for (int nMenu = 0; nMenu < 7; nMenu++)
        {
            for (int nAction = 0; nAction < m_aPersonalActionLists[nMenu].num;
                 nAction++)
            {
                m_aPersonalActionLists[nMenu][nAction].Clear();
            }
            m_aPersonalActionLists[nMenu].num = 0;
            pCreature->GetPersonalActions(nMenu, m_aPersonalActionLists[nMenu]);
        }
        m_cTargetActionMenu.PopulateMenus(pCreature, bDisabled, pTargetObject);

        for (int nMenu = 0; nMenu < 6; nMenu++)
        {
            int bFound = 0;
            int nAction;
            for (nAction = 0; nAction < m_aPersonalActionLists[nMenu].num;
                 nAction++)
            {
                if (m_aPersonalActionLists[nMenu][nAction].m_nAction ==
                    m_aSelectedPersonalActions[nMenu])
                {
                    bFound = 1;
                    break;
                }
            }
            if (bFound)
            {
                m_aPersonalActions[nMenu].Show(0,
                    m_aPersonalActionLists[nMenu].num > 1);
                m_aPersonalActions[nMenu].SetIcon(
                    m_aPersonalActionLists[nMenu][nAction].m_resIcon,
                    m_aPersonalActionLists[nMenu][nAction].m_sName,
                    m_aPersonalActionLists[nMenu][nAction].m_bEnabled &&
                        bDisabled == 0, 0);
            }
            else if (m_aPersonalActionLists[nMenu].num > 0)
            {
                m_aPersonalActions[nMenu].Show(0,
                    m_aPersonalActionLists[nMenu].num > 1);
                m_aPersonalActions[nMenu].SetIcon(
                    m_aPersonalActionLists[nMenu][0].m_resIcon,
                    m_aPersonalActionLists[nMenu][0].m_sName,
                    m_aPersonalActionLists[nMenu][0].m_bEnabled &&
                        bDisabled == 0, 0);
            }
            else
            {
                m_aPersonalActions[nMenu].Show(1, 0);
            }
        }
        UpdateActionDescription();
    }
}
#endif
// PC 0x006DDDF0
void CSWGuiMainInterface::GetActionIcon(
    CSWSCombatRound* pCombatRound, CSWSObjectActionNode* pAction,
    CSWSCombatRoundAction* pCombatAction, int& nActionId,
    CResRef& cIcon, CResRef& cDefaultIcon)
{
    cIcon = "";
    cDefaultIcon = "";
    int nActionType = CSWSObject::ACTION_PHYSICAL_ATTACKS;
    if (pAction != NULL)
        nActionType = pAction->m_nActionType;

    switch (nActionType)
    {
    case CSWSObject::ACTION_PHYSICAL_ATTACKS:
        if (pCombatAction != NULL)
        {
            int nSpecialAttack = pCombatAction->m_nSpecialAttack;
            switch (pCombatAction->m_nActionType)
            {
            case CSWSCombatRoundAction::ACTION_ATTACK:
                cIcon = "i_attack";
                cDefaultIcon = "i_attackm";
                break;
            case CSWSCombatRoundAction::ACTION_EQUIP_ITEM:
                cIcon = "i_equip";
                cDefaultIcon = "i_equipm";
                break;
            case CSWSCombatRoundAction::ACTION_UNEQUIP_ITEM:
                cIcon = "i_unequip";
                cDefaultIcon = "i_equipm";
                break;
            case CSWSCombatRoundAction::ACTION_ITEM_CAST_SPELL:
                {
                    CSWSItem* pItem = g_pAppManager->GetServerExoApp()->
                        GetItemByGameObjectID(pCombatAction->m_oidSpellItem);
                    if (pItem != NULL)
                    {
                        cIcon = pItem->GetIcon();
                        cDefaultIcon = "i_useitemm";
                    }
                }
                break;
            case CSWSCombatRoundAction::ACTION_CAST_SPELL:
                {
                    int nSpellId = pCombatAction->m_nSpellId;
                    CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(nSpellId);
                    if (pSpell != NULL)
                    {
                        cIcon = pSpell->m_resIcon;
                        cDefaultIcon = "i_powerm";
                    }
                }
                break;
            case CSWSCombatRoundAction::ACTION_SPECIAL_ATTACK:
                {
                    CSWFeat* pFeat = g_pRules->GetFeat(nSpecialAttack);
                    if (pFeat != NULL)
                    {
                        cIcon = pFeat->m_cIcon;
                        cDefaultIcon = "i_featm";
                    }
                }
                break;
            }
            break;
        }
    case CSWSObject::ACTION_COMBAT:
        if (pCombatAction != NULL)
        {
            switch (pCombatAction->m_nActionType)
            {
            case CSWSCombatRoundAction::ACTION_ATTACK:
                cIcon = "i_attack";
                cDefaultIcon = "i_attackm";
                break;
            case CSWSCombatRoundAction::ACTION_EQUIP_ITEM:
                cIcon = "i_equip";
                cDefaultIcon = "i_equipm";
                break;
            case CSWSCombatRoundAction::ACTION_UNEQUIP_ITEM:
                cIcon = "i_unequip";
                cDefaultIcon = "i_equipm";
                break;
            case CSWSCombatRoundAction::ACTION_ITEM_CAST_SPELL:
                {
                    CSWSItem* pItem = g_pAppManager->GetServerExoApp()->
                        GetItemByGameObjectID(pCombatAction->m_oidSpellItem);
                    if (pItem != NULL)
                    {
                        cIcon = pItem->GetIcon();
                        cDefaultIcon = "i_useitemm";
                    }
                }
                break;
            case CSWSCombatRoundAction::ACTION_CAST_SPELL:
                {
                    int nSpellId = pCombatAction->m_nSpellId;
                    CSWSpell* pSpell = g_pRules->m_pSpellArray->GetSpell(nSpellId);
                    if (pSpell != NULL)
                    {
                        cIcon = pSpell->m_resIcon;
                        cDefaultIcon = "i_powerm";
                    }
                }
                break;
            case CSWSCombatRoundAction::ACTION_SWITCH_WEAPONS:
                cIcon = "i_swtchwpn";
                cDefaultIcon = "i_swtchwpn";
                break;
            case CSWSCombatRoundAction::ACTION_SPECIAL_ATTACK:
                {
                    CSWFeat* pFeat = g_pRules->GetFeat(pCombatAction->m_nSpecialAttack);
                    if (pFeat != NULL)
                    {
                        cIcon = pFeat->m_cIcon;
                        cDefaultIcon = "i_featm";
                    }
                }
                break;
            case CSWSCombatRoundAction::ACTION_STANCE:
                cIcon = "i_stancedef";
                cDefaultIcon = "i_stancedefm";
                break;
            }
        }
        else
        {
            CSWCCreature* pPlayer =
                g_pAppManager->GetClientExoApp()->GetSWParty()->GetPlayerCharacter();
            CSWSCreature* pCreature = NULL;
            if (pPlayer != NULL)
                pCreature = pPlayer->GetServerCreature();
            if (pCreature != NULL)
            {
                if (pCreature->IsInTotalDefense())
                {
                    cIcon = "i_stancedef";
                    cDefaultIcon = "i_stancedefm";
                }
                else
                {
                    cIcon = "i_attack";
                    cDefaultIcon = "i_attackm";
                }
            }
            else
            {
                cIcon = "i_attack";
                cDefaultIcon = "i_attackm";
            }
        }
        break;
    case CSWSObject::ACTION_ITEM_CAST_SPELL:
        {
            unsigned long oidItem =
                (unsigned long)pAction->m_apParam[CSWSObjectActionNode::ITEM_SPELL_ITEM_PARAMETER];
            CSWSItem* pItem = g_pAppManager->GetServerExoApp()->GetItemByGameObjectID(oidItem);
            if (pItem != NULL)
            {
                cIcon = pItem->GetIcon();
                cDefaultIcon = "i_useitemm";
            }
        }
        break;
    case CSWSObject::ACTION_CAST_SPELL:
        {
            int nSpellId = (int)pAction->m_apParam[CSWSObjectActionNode::SPELL_ID_PARAMETER];
            cIcon = g_pRules->m_pSpellArray->GetSpell(nSpellId)->m_resIcon;
            cDefaultIcon = "i_powerm";
        }
        break;
    }
}

void CSWGuiMainInterfaceStatus::ShiftControl(int nOffset)
{
    CSWGuiExtent cExtent = m_cLabel.m_cExtent;
    cExtent.m_nTop += nOffset;
    m_cLabel.SetExtent(cExtent);
}
// PC 0x006E1220
void CSWGuiMainInterface::ShowActionQueue(int bShow)
{
    if (bShow == m_cActionDescriptionState.m_bEnabled)
    {
        return;
    }

    int bMoveMap = 1;
    m_cActionDescriptionState.m_bEnabled = bShow;
    if (!bShow && !m_lblActionQueueBackground.m_bEnabled)
    {
        bMoveMap = 0;
    }

#ifdef _XBOX
    m_lblBg1.SetEnabledFlag(bShow);
    m_CSWGuiLabelHilight_6B1C.SetEnabledFlag(bShow);
    m_lblActionQueueBackground.SetEnabledFlag(bShow);
    m_lblCmbtmodemsg.SetEnabledFlag(bShow);
#else
    for (int nIcon = 0; nIcon < 2; nIcon++)
    {
        m_aCombatbg[nIcon].SetEnabledFlag(bShow);
    }
    m_CSWGuiLabelHilight_6B1C.SetEnabledFlag(bShow);
    m_lblActionQueueBackground.SetEnabledFlag(bShow);
    m_btnClearone.SetEnabledFlag(bShow);
    m_btnClearone2.SetEnabledFlag(bShow);
    m_btnClearall.SetEnabledFlag(bShow);
    if (!m_bToolTipShown)
    {
        m_lblCmbtmodemsg.SetEnabledFlag(bShow);
        m_cToolTipBackground.SetEnabledFlag(bShow);
    }
#endif
    for (int nIcon = 0; nIcon < ACTION_QUEUE_FIRST_ICON; nIcon++)
    {
        m_aActionIcons[nIcon].SetEnabledFlag(bShow);
    }

    int nOffset = m_nInterfaceOffset;
    if (!bShow)
    {
        nOffset = -nOffset;
        m_fCombatMessageTime = -1.0f;
    }
    if (bMoveMap)
    {
#ifdef _XBOX
        ShiftControl(m_lblIndicate, nOffset);
        ShiftControl(m_lblName, nOffset);
#endif
        ShiftControl(m_lblMapborder, nOffset);
        ShiftControl(m_lblMap, nOffset);
#ifdef _XBOX
        ShiftControl(m_pbHealth, nOffset);
#else
        ShiftControl(m_btnMinimap, nOffset);
#endif
        m_cMapViewport.m_nTop += nOffset;
        for (int nStatus = 0; nStatus < GUI_STATUS_COUNT; nStatus++)
        {
            m_aEffects[nStatus].ShiftControl(nOffset);
        }
    }
}
// PC 0x006DE4C0
void CSWGuiMainInterface::SetCombatMessage(unsigned long nMessage)
{
    if (m_fCombatMessageTime != -1.0f)
    {
        return;
    }

    switch (nMessage)
    {
#ifdef _XBOX
    case 42475:
        m_lblCmbtmodemsg.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_RED);
        break;
#endif
    case 42476:
        if (g_pAppManager->GetClientExoApp()->GetAutoPaused())
        {
            return;
        }
        m_lblCmbtmodemsg.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_NEW_HIGHLIGHT);
        break;
    case 42477:
        if (g_pAppManager->GetClientExoApp()->GetAutoPaused())
        {
            return;
        }
        m_lblCmbtmodemsg.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_NEW_HIGHLIGHT);
        break;
    case 42478:
        m_lblCmbtmodemsg.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_NEW_HIGHLIGHT);
        m_fCombatMessageTime = m_fCombatMessageDuration = 2.5f;
        break;
    case 47859:
        if (g_pAppManager->GetClientExoApp()->GetAutoPaused())
        {
            return;
        }
        m_lblCmbtmodemsg.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_NEW_HIGHLIGHT);
        m_fCombatMessageTime = m_fCombatMessageDuration = 2.5f;
        break;
    case 47915:
        m_lblCmbtmodemsg.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_NEW_HIGHLIGHT);
        break;
#ifndef _XBOX
    case 111303:
        m_lblCmbtmodemsg.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_NEW_HIGHLIGHT);
        break;
#endif
    default:
        m_lblCmbtmodemsg.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_NEW_HIGHLIGHT);
        m_fCombatMessageTime = m_fCombatMessageDuration = 2.5f;
        break;
    }

    m_nCombatMessage = nMessage;
    CExoString sMessage =
        g_pAppManager->GetClientExoApp()->GetGUIString(nMessage);
    m_lblCmbtmodemsg.m_cText.m_cParams.SetText(sMessage);
    m_lblCmbtmodemsg.m_cText.m_cParams.m_fAlpha = 1.0f;
}
// PC 0x006E1460
void CSWGuiMainInterface::UpdatePortraits()
{
#ifdef _XBOX
    int nShown = 0;
#endif
    CSWParty* pParty = g_pAppManager->m_pClientExoApp->GetSWParty();
#ifndef _XBOX
    CExoString sName;
#endif
    for (int nMember = 0; nMember < CSWParty::MAX_MEMBERS; nMember++)
    {
        CSWCCreature* pCreature = pParty->GetCharacter(nMember);
        CSWSCreature* pServer = pCreature != NULL ? pCreature->GetServerCreature() : NULL;
        int bShow = pServer != NULL;
        unsigned char nState = 0;
        unsigned char nBuffCount = 0;
        unsigned char nDebuffCount = 0;
        int nPortrait = nMember;
        if (m_fPortraitTransitionTime > 0.0f)
        {
            switch (nMember)
            {
            case 0:
                nPortrait = 1;
                break;
            case 1:
                if (pParty->GetNumCharacters() == 2)
                {
                    nPortrait = 0;
                    bShow = 0;
                }
                else
                    nPortrait = 2;
                break;
            case 2:
                if (pParty->GetNumCharacters() == 2)
                    bShow = 1;
                nPortrait = 0;
                break;
            }
        }
        else if (pParty->GetNumCharacters() == 2)
        {
            if (nMember == 1)
            {
                bShow = 0;
                nPortrait = 2;
            }
            else if (nMember == 2)
                bShow = 1;
        }

        if (pServer != NULL)
        {
#ifdef _XBOX
            nShown++;
#else
            m_aCharacters[nPortrait].m_sName = pCreature->GetStats()->GetFirstName();
            m_aCharacters[nPortrait].m_sDescription = pCreature->GetStats()->GetLastName();
#endif
            m_aCharacters[nPortrait].SetPortrait(pCreature->GetPortrait(1));
            int nPoisonState = pServer->m_nPoisonState;
            m_aCharacters[nPortrait].SetHealth(pServer->GetCurrentHitPoints(0),
                                 pServer->GetMaxHitPoints(1), nPoisonState);
#ifdef _XBOX
            short nCurForce = pServer->GetStats()->GetCurrentForcePoints();
            m_aCharacters[nPortrait].SetForce(nCurForce, pServer->GetMaxForcePoints());
#else
            CSWSCreatureStats* pStats = pServer->GetStats();
            int nMaxForce = pServer->GetMaxForcePoints();
            m_aCharacters[nPortrait].SetForce(pStats->GetCurrentForcePoints(), nMaxForce);
#endif
#ifndef _XBOX
            m_aCharacters[nPortrait].SetLevel(pServer->GetStats()->GetLevel(0));
            m_aCharacters[nPortrait].SetXPNeeded((int)(pServer->GetStats()->GetExpNeededForLevelUp() -
                                       pServer->GetStats()->GetExperience()) > 0 ?
                                  pServer->GetStats()->GetExpNeededForLevelUp() -
                                  pServer->GetStats()->GetExperience() : 0);
#endif
            if (pServer->GetDead() || pServer->GetDeadTemp())
                nState = CSWGuiMainInterfaceChar::PORTRAIT_DEAD;
            else if (pServer->GetStats()->CanLevelUp())
            {
                if (g_pAppManager->GetClientExoApp()->GetClientOptions()->m_bAutoLevelUpNPCs &&
                    pCreature->GetId() != g_pAppManager->GetClientExoApp()->GetPlayerCreature()->m_nId &&
                    !pServer->m_bPlayerCreated)
                {
                    pServer->GetStats()->AutoLevelup(1);
                }
                else
                {
                    nState = CSWGuiMainInterfaceChar::PORTRAIT_LEVEL_UP;
                    if (pCreature->m_bLevelUpSent)
                    {
                        g_pAppManager->GetClientExoApp()->GetInGameGui()->UpdateStatus(GUI_STATUS_LEVEL_UP, 0);
                        pCreature->m_bLevelUpSent = 0;
                    }
                }
            }
            else if (pServer->IsDebilitated(0))
            {
                nState = CSWGuiMainInterfaceChar::PORTRAIT_DEBILITATED;
                if (nMember == 0)
                    SetCombatMessage(STRREF_DEBILITATED);
            }
            else
            {
#ifdef _XBOX
                for (int nIcon = 0; nIcon < pServer->GetEffectIconCount(); nIcon++)
                {
                    CEffectIconObject* pIcon = pServer->m_lstEffectIcons.element[nIcon];
                    if (pIcon->m_bGood)
                        nBuffCount++;
                    else
                        nDebuffCount++;
                }
#else
                int nIcon = 0;
                int nEffectCount = pServer->GetEffectIconCount();
                if (nEffectCount > 0)
                {
                    do
                    {
                        CEffectIconObject* pIcon = pServer->m_lstEffectIcons.element[nIcon];
                        if (pIcon->m_nGood)
                        {
                            if (pIcon->m_bGood)
                                nBuffCount++;
                            else
                                nDebuffCount++;
                        }
                        ++nIcon;
                    } while (nIcon < nEffectCount);
                }
#endif
            }
#ifdef _XBOX
            if (!pServer->IsDebilitated(0) && m_nCombatMessage == STRREF_DEBILITATED)
            {
                SetCombatMessage(42475);
            }
#else
            if (!pServer->IsDebilitated(0) && m_nCombatMessage == STRREF_DEBILITATED &&
                m_fCombatMessageTime == -1.0f)
            {
                m_lblCmbtmodemsg.m_cText.m_cParams.SetColor(CGuiInGame::COLOR_NEW_HIGHLIGHT);
                m_nCombatMessage = STRREF_COMBAT_MODE_ENGAGED;
                CExoString sMessage = g_pAppManager->GetClientExoApp()->GetGUIString(STRREF_COMBAT_MODE_ENGAGED);
                m_lblCmbtmodemsg.m_cText.m_cParams.SetText(sMessage);
                m_lblCmbtmodemsg.m_cText.m_cParams.m_fAlpha = 1.0f;
            }
#endif
            m_aCharacters[nPortrait].Show(1, nState, nBuffCount, nDebuffCount);
            if (m_fPortraitTransitionTime > 0.0f && nPortrait == 0)
            {
#ifndef _XBOX
                m_cTransitionCharacter.m_sName = pCreature->GetStats()->GetFirstName();
                m_cTransitionCharacter.m_sDescription = pCreature->GetStats()->GetLastName();
#endif
                m_cTransitionCharacter.SetPortrait(pCreature->GetPortrait(1));
#ifdef _XBOX
                m_cTransitionCharacter.SetHealth(pServer->GetCurrentHitPoints(0),
                    pServer->GetMaxHitPoints(1), pServer->m_nPoisonState);
#else
                int nTransitionPoisonState = pServer->m_nPoisonState;
                m_cTransitionCharacter.SetHealth(pServer->GetCurrentHitPoints(0),
                    pServer->GetMaxHitPoints(1), nTransitionPoisonState);
#endif
#ifdef _XBOX
                short nTransitionForce = pServer->GetStats()->GetCurrentForcePoints();
                m_cTransitionCharacter.SetForce(nTransitionForce, pServer->GetMaxForcePoints());
#else
                CSWSCreatureStats* pTransitionStats = pServer->GetStats();
                int nTransitionMaxForce = pServer->GetMaxForcePoints();
                m_cTransitionCharacter.SetForce(pTransitionStats->GetCurrentForcePoints(), nTransitionMaxForce);
#endif
#ifndef _XBOX
                m_cTransitionCharacter.SetLevel(pServer->GetStats()->GetLevel(0));
                m_cTransitionCharacter.SetXPNeeded((int)(pServer->GetStats()->GetExpNeededForLevelUp() -
                    pServer->GetStats()->GetExperience()) > 0 ?
                    pServer->GetStats()->GetExpNeededForLevelUp() - pServer->GetStats()->GetExperience() : 0);
#endif
                m_cTransitionCharacter.Show(1, nState, nBuffCount, nDebuffCount);
            }
        }
        if (!bShow)
            m_aCharacters[nMember].Show(0, 0, 0, 0);
    }
#ifdef _XBOX
    m_lblBg2.m_bEnabled = nShown > 1;
#endif
}
// PC 0x006DE690
void CSWGuiMainInterface::InitializeMap()
{
    int bMapEnabled = 0;
    CSWSModule* pModule = g_pAppManager->GetServerExoApp()->GetModule();
    CExoString sArea;
    CExoString sMap;
    pModule->m_cAreaResRef.CopyToString(sArea);
    sMap.Format("lbl_map%s", sArea.CStr());

    if (pModule->m_pAreaMap->m_nExploredSize > 0)
    {
        float fScale = pModule->m_pAreaMap->m_fInverseZoom;
        CSWGuiBorderParams* pBorder;
#ifdef _XBOX
        pBorder = &m_lblMap.m_cBorder.m_cParams;
        pBorder->SetFillImage(CResRef(sMap), 0);
#else
        {
            CResRef cMap(sMap);
            pBorder = &m_lblMap.m_cBorder.m_cParams;
            pBorder->SetFillImage(cMap, 0);
        }
#endif
        pBorder->m_nFillMode = CSWGuiBorderParams::FILL_MODE_MAP;
        if (g_pAppManager->GetClientExoApp()->GetSWGuiManager() != NULL &&
            (g_pAppManager->GetClientExoApp()->GetSWGuiManager()->m_nWidth >= 1600 ||
             g_pAppManager->GetClientExoApp()->GetSWGuiManager()->m_nHeight >= 1200))
        {
            m_nMapWidth--;
            m_nMapHeight--;
        }
        {
            CSWGuiExtent cExtent(0, 0, (int)(m_nMapWidth * fScale),
                                (int)(m_nMapHeight * fScale));
            m_lblMap.SetExtent(cExtent);
        }
        bMapEnabled = 1;
    }

    CClientOptions* pOptions = g_pAppManager->GetClientExoApp()->GetClientOptions();
    if (pOptions != NULL)
    {
        m_lblMapborder.m_bEnabled = bMapEnabled && pOptions->m_bMiniMap;
        m_lblArrow.m_bEnabled = bMapEnabled && pOptions->m_bMiniMap;
    }
}
#ifndef _XBOX
int CSWGuiMainInterface::StartRotation()  // PC 0x006DE880
{
    return (int)g_pAppManager->m_pClientExoApp->GetSWParty();
}
#endif
void CSWGuiMainInterface::ShowFlashingStatus(unsigned char nEffect)  // PC 0x006DE890
{
    switch (nEffect)
    {
    case 4:
        m_aEffects[5].Show(0);
        break;

    case 5:
        m_aEffects[4].Show(0);
        break;

    case 3:
        m_aEffects[2].Show(0);
        break;

    case 2:
        m_aEffects[3].Show(0);
        break;
    }

    m_aEffects[nEffect].Show(1);
}
// PC 0x006E0F20
void CSWGuiMainInterface::UpdateActionQueue()
{
    CSWSCreature* pCreature = g_pAppManager->GetClientExoApp()->GetSWParty()->
        GetPlayerCharacter()->GetServerCreature();
    CSWSCombatRound* pCombatRound = pCreature->m_pcCombatRound;
    int bCurrentAction = pCombatRound->m_bRoundStarted != 0 &&
        pCombatRound->GetCurrentAction() != 0;
    CExoString sDescription;
    CExoString sSeparator("-");
    CResRef cIcon;
    CResRef cDefaultIcon;
    int nIcon = 0;
    CExoLinkedListNode* pActionPos = pCreature->m_lstActions.GetHeadPos();
    int bCurrentShown = 0;

    while (pActionPos != NULL && nIcon < ACTION_QUEUE_ICON_COUNT)
    {
        CSWSObjectActionNode* pAction = pCreature->m_lstActions.GetAtPos(pActionPos);
        if (pAction != NULL)
        {
            if (((pAction->m_nActionType == CSWSObject::ACTION_COMBAT && bCurrentAction) ||
                 pAction->m_nActionType == CSWSObject::ACTION_PHYSICAL_ATTACKS ||
                 pAction->m_nActionType == CSWSObject::ACTION_CAST_SPELL ||
                 pAction->m_nActionType == CSWSObject::ACTION_ITEM_CAST_SPELL) && !bCurrentShown)
            {
                int nActionId = -1;
                if (pCombatRound->m_pCurrentAction != NULL)
                {
                    bCurrentShown = 1;
                    GetActionIcon(pCombatRound, pAction, pCombatRound->m_pCurrentAction,
                                  nActionId, cIcon, cDefaultIcon);
                    ShowActionIcon(nIcon, cIcon, cDefaultIcon, bCurrentAction, 0);
                }
                else if (pAction->m_nActionType != CSWSObject::ACTION_COMBAT)
                {
                    GetActionIcon(pCombatRound, pAction, NULL,
                                  nActionId, cIcon, cDefaultIcon);
                    ShowActionIcon(nIcon, cIcon, cDefaultIcon, bCurrentAction, 0);
                    bCurrentShown = 1;
                }
            }

            if (pAction->m_nActionType == CSWSObject::ACTION_COMBAT && pCombatRound->m_pScheduledActions != NULL)
            {
                CExoLinkedListNode* pCombatPos = pCombatRound->m_pScheduledActions->GetHeadPos();
                if (pCombatPos != NULL)
                {
                    while (nIcon < ACTION_QUEUE_ICON_COUNT && pCombatPos != NULL)
                    {
                        CSWSCombatRoundAction* pCombatAction =
                            pCombatRound->m_pScheduledActions->GetAtPos(pCombatPos);
                        int nActionId;
                        GetActionIcon(pCombatRound, pAction, pCombatAction,
                                      nActionId, cIcon, cDefaultIcon);
                        ShowActionIcon(nIcon, cIcon, cDefaultIcon, bCurrentAction, 0);
                        pCombatRound->m_pScheduledActions->GetNext(pCombatPos);
                    }
                }
                else if (!bCurrentShown)
                {
                    int nActionId;
                    CSWSCombatRoundAction* pCurrentAction = pCombatRound->m_pCurrentAction;
                    GetActionIcon(pCombatRound, pAction, pCurrentAction,
                                  nActionId, cIcon, cDefaultIcon);
                    ShowActionIcon(nIcon, cIcon, cDefaultIcon, bCurrentAction, 0);
                }
            }
        }
        pCreature->m_lstActions.GetNext(pActionPos);
    }

    while (nIcon < ACTION_QUEUE_ICON_COUNT)
        m_aActionIcons[nIcon++].SetEnabledFlag(0);
}
// PC 0x006E2610
void CSWGuiMainInterface::RemoveFloatyText(CSWGuiFloatyText* pFloatyText)
{
    if (pFloatyText != NULL)
    {
        int nIndex = m_lFloatyText.IndexOf(pFloatyText);

        if (nIndex != -1)
        {
            m_lFloatyText.DelIndex(nIndex);
        }
        delete pFloatyText;
    }
}
// PC 0x006DE910
void CSWGuiMainInterface::OnCombatYButton()
{
    CSWParty* pParty = g_pAppManager->m_pClientExoApp->GetSWParty();
    CSWCCreature* pPlayer = pParty->GetPlayerCharacter();
    CSWSCreature* pServerCreature = pPlayer->GetServerCreature();

    if (pServerCreature != NULL)
    {
        CSWSCombatRound* pCombatRound = pServerCreature->m_pcCombatRound;
        if (pCombatRound != NULL && pCombatRound->RemoveLastAction() == 0)
        {
            pServerCreature->ClearAllActions(1);
        }
    }
}

#ifndef _XBOX
void CSWGuiMainInterface::UpdateIndicator(float fElapsed)  // PC 0x006E1B70
{
    CSWCObject* pObject = g_pAppManager->GetClientExoApp()->GetGameObject(m_nTargetId);
    int bTarget = 0;
    int nPoint = 0;
    int nSize = STICKY_RETICLE_MAX_SIZE;
    Vector vPoint;
    CResRef cReticle;
    CResRef cArrow;
    CSWGuiExtent cExtent;
    int nWidth = m_pManager->m_nWidth;
    int nHeight = m_pManager->m_nHeight;

    if (pObject != NULL && pObject->AsSWCObject() != NULL)
    {
        CSWCObject* pTarget = pObject->AsSWCObject();
        int bFriendly = 1;
        int nPoisonState = 0;
        CSWCCreature* pPlayer = g_pAppManager->GetClientExoApp()->GetSWParty()->GetPlayerCharacter();
        bTarget = 1;
        nPoint = pTarget->GetReticlePoint(vPoint);
        if (nPoint != CSWCObject::RETICLE_POINT_UNAVAILABLE)
        {
            switch (pObject->m_nObjectType)
            {
            case SW_OBJECT_TYPE_CREATURE:
                {
                    CSWSCreature* pCreature = pTarget->AsSWCCreature()->GetServerCreature();
                    bFriendly = !pTarget->IsHostile() && !g_pAppManager->GetClientExoApp()->GetTargetChanging();
                    int nCreaturePoison = pCreature != NULL ? pCreature->m_nPoisonState : 0;
                    nPoisonState = nCreaturePoison;
                    Vector vOffset = pPlayer->m_vPosition - pTarget->m_vPosition;
                    float fDistanceSquared = vOffset * vOffset;
                    if (fDistanceSquared >= STICKY_RETICLE_FAR_RANGE * STICKY_RETICLE_FAR_RANGE)
                        nSize = STICKY_RETICLE_CREATURE_MIN_SIZE;
                    else if (fDistanceSquared > STICKY_RETICLE_NEAR_RANGE * STICKY_RETICLE_NEAR_RANGE)
                        nSize = STICKY_RETICLE_MAX_SIZE - (int)(((sqrt(fDistanceSquared) - STICKY_RETICLE_NEAR_RANGE) / (STICKY_RETICLE_FAR_RANGE - STICKY_RETICLE_NEAR_RANGE)) * (STICKY_RETICLE_MAX_SIZE - STICKY_RETICLE_CREATURE_MIN_SIZE));

                    if (bFriendly)
                    {
                        cReticle = "friendlyreticle2";
                        cArrow = "friendlyarrow";
                    }
                    else
                    {
                        if (g_pAppManager->GetClientExoApp()->GetCombatMode())
                        {
                            cReticle = "combatreticle";
                            if (m_fTargetUpdateDelay > 0.0f)
                            {
                                m_fTargetUpdateDelay -= fElapsed;
                                if (m_fTargetUpdateDelay <= 0.0f)
                                    m_fTargetUpdateDelay = 0.0f;
                                else
                                    nSize += (int)((1.0f - (0.5f - m_fTargetUpdateDelay) * 2.0f) * STICKY_RETICLE_MAX_SIZE);
                            }
                            else if (m_fTargetUpdateDelay == -1.0f)
                            {
                                nSize += STICKY_RETICLE_MAX_SIZE;
                                m_fTargetUpdateDelay = 0.5f;
                            }
                        }
                        else
                            cReticle = "hostilereticle2";
                        cArrow = "hostilearrow";
                    }
                }
                break;
            case SW_OBJECT_TYPE_TRIGGER:
            case SW_OBJECT_TYPE_PLACEABLE:
            case SW_OBJECT_TYPE_DOOR:
                {
                    CSWCDoor* pDoor = pTarget->AsSWCDoor();
                    CSWCPlaceable* pPlaceable = pTarget->AsSWCPlaceable();
                    CSWCTrigger* pTrigger = pTarget->AsSWCTrigger();
                    if (pDoor != NULL && pDoor->m_bTrapDetected && pDoor->m_bTrap)
                        bFriendly = 0;
                    else if (pTrigger != NULL)
                        bFriendly = !pTrigger->IsHostile();
                    else if (pPlaceable != NULL)
                    {
                        if (pPlaceable->m_bTrap && pPlaceable->m_bTrapDetected)
                            bFriendly = 0;
                        else if (pPlaceable->IsHostile())
                        {
                            if (pPlaceable->GetServerObject()->GetReputation(pPlayer->GetServerObject()->GetId(), 0) <= CSWSObject::MAX_HOSTILE_REPUTATION)
                                bFriendly = 0;
                        }
                    }
                    Vector vOffset = pPlayer->m_vPosition - pTarget->m_vPosition;
                    float fDistanceSquared = vOffset * vOffset;
                    if (fDistanceSquared >= STICKY_RETICLE_OBJECT_FAR_RANGE * STICKY_RETICLE_OBJECT_FAR_RANGE)
                        nSize = STICKY_RETICLE_OBJECT_MIN_SIZE;
                    else if (fDistanceSquared > STICKY_RETICLE_NEAR_RANGE * STICKY_RETICLE_NEAR_RANGE)
                        nSize = STICKY_RETICLE_MAX_SIZE - (int)(((sqrt(fDistanceSquared) - STICKY_RETICLE_NEAR_RANGE) / (STICKY_RETICLE_OBJECT_FAR_RANGE - STICKY_RETICLE_NEAR_RANGE)) * (STICKY_RETICLE_MAX_SIZE - STICKY_RETICLE_OBJECT_MIN_SIZE));
                    cReticle = bFriendly ? "friendlyreticle2" : "hostilereticle2";
                    cArrow = bFriendly ? "friendlyarrow" : "hostilearrow";
                }
                break;
            default:
                return;
            }
        }
        m_cTargetActionMenu.SetFriend(bFriendly, nPoisonState);
        if (nPoint != CSWCObject::RETICLE_POINT_UNAVAILABLE)
        {
            float fRotation = 0.0f;
            CSWGuiExtent cMargin = m_lblArrowMargin.m_cExtent;
            if (nPoint == CSWCObject::RETICLE_POINT_BEHIND)
            {
                cReticle = cArrow;
                nSize = STICKY_RETICLE_MARGIN;
                if (vPoint.m_fX < nWidth / 2)
                {
                    fRotation = 180.0f;
                    cExtent.m_nLeft = cMargin.GetRight() - nSize;
                }
                else
                    cExtent.m_nLeft = cMargin.m_nLeft;
                if (vPoint.m_fY < cMargin.m_nTop + STICKY_RETICLE_MARGIN)
                    cExtent.m_nTop = cMargin.GetBottom() - nSize;
                else if (vPoint.m_fY > cMargin.GetBottom() - STICKY_RETICLE_MARGIN)
                    cExtent.m_nTop = cMargin.m_nTop;
                else if (vPoint.m_fY < nHeight / 2)
                    cExtent.m_nTop = nHeight - (int)(vPoint.m_fY + 16.0f);
                else
                    cExtent.m_nTop = (int)(vPoint.m_fY - 16.0f);
            }
            else if (vPoint.m_fX < cMargin.m_nLeft + STICKY_RETICLE_MARGIN)
            {
                cReticle = cArrow;
                nSize = STICKY_RETICLE_MARGIN;
                cExtent.m_nLeft = cMargin.m_nLeft;
                if (vPoint.m_fY < cMargin.m_nTop + STICKY_RETICLE_MARGIN)
                    cExtent.m_nTop = cMargin.m_nTop;
                else if (vPoint.m_fY > cMargin.GetBottom() - STICKY_RETICLE_MARGIN)
                    cExtent.m_nTop = cMargin.GetBottom() - nSize;
                else
                    cExtent.m_nTop = (int)(vPoint.m_fY - 16.0f);
            }
            else if (vPoint.m_fX > cMargin.GetRight() - STICKY_RETICLE_MARGIN)
            {
                cReticle = cArrow;
                nSize = STICKY_RETICLE_MARGIN;
                cExtent.m_nLeft = cMargin.GetRight() - nSize;
                fRotation = 180.0f;
                if (vPoint.m_fY < cMargin.m_nTop + STICKY_RETICLE_MARGIN)
                    cExtent.m_nTop = cMargin.m_nTop;
                else if (vPoint.m_fY > cMargin.GetBottom() - STICKY_RETICLE_MARGIN)
                    cExtent.m_nTop = cMargin.GetBottom() - nSize;
                else
                    cExtent.m_nTop = (int)(vPoint.m_fY - 16.0f);
            }
            else if (vPoint.m_fY < cMargin.m_nTop + STICKY_RETICLE_MARGIN)
            {
                cReticle = cArrow;
                nSize = STICKY_RETICLE_MARGIN;
                cExtent.m_nTop = cMargin.m_nTop;
                cExtent.m_nLeft = (int)(vPoint.m_fX - 16.0f);
                fRotation = 270.0f;
            }
            else if (vPoint.m_fY > cMargin.GetBottom() - STICKY_RETICLE_MARGIN)
            {
                cReticle = cArrow;
                nSize = STICKY_RETICLE_MARGIN;
                cExtent.m_nTop = cMargin.GetBottom() - nSize;
                cExtent.m_nLeft = (int)(vPoint.m_fX - 16.0f);
                fRotation = 90.0f;
            }
            else
            {
                cExtent.m_nTop = (int)(vPoint.m_fY - nSize / 2);
                cExtent.m_nLeft = (int)(vPoint.m_fX - nSize / 2);
            }
            cExtent.m_nHeight = nSize;
            cExtent.m_nWidth = nSize;
            m_lblReticle.SetExtent(cExtent);
            m_lblReticle.m_cBorder.m_cParams.m_fRotation = fRotation;
            m_lblReticle.m_cBorder.m_cParams.SetFillImage(cReticle, 0);
        }
    }
    m_lblReticle.SetEnabledFlag(nPoint != CSWCObject::RETICLE_POINT_UNAVAILABLE);
    m_cTargetActionMenu.m_bVisible = bTarget;
    UpdateStickyTarget();
}
#endif

#ifndef _XBOX
CSWGuiTargetActionMenu::~CSWGuiTargetActionMenu()  // PC 0x006E41C0
{
}
#endif

#ifdef _XBOX
CSWGuiMainInterface::CSWGuiMainInterface(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    CExoString sControl;
    CSWGuiLabel lblMapview;
    int nIndex;
    CSWGuiFloatyText::m_pMainInterface = this;

    StartLoadFromLayout("maininterface");

    for (nIndex = 0; nIndex < 6; ++nIndex)
        m_aPersonalActions[nIndex].Initialize(this, nIndex + 1);
    for (nIndex = 0; nIndex < 3; ++nIndex)
        m_aCharacters[nIndex].Initialize(this, nIndex + 1);
    m_cTransitionCharacter.Initialize(this, 4);
    for (nIndex = 0; nIndex < 4; ++nIndex)
    {
        sControl.Format("LBL_QUEUE%d", nIndex);
        InitControl(m_aActionIcons[nIndex], sControl, 1);
    }
    for (nIndex = 0; nIndex < GUI_STATUS_COUNT; ++nIndex)
        m_aEffects[nIndex].Initialize(this, nIndex);

    InitControl(m_lblBg1, "LBL_BG1", 1);
    InitControl(m_lblBg2, "LBL_BG2", 1);
    InitControl(m_CSWGuiLabelHilight_6B1C, "LBL_HIGHLIGHT", 1);
    InitControl(m_cActionText, "LBL_ACTIONDESC", 1);
    InitControl(m_cActionTextBack, "LBL_ACTIONDESCBG", 1);
    InitControl(m_lblActiondescbg2, "LBL_ACTIONDESCBG2", 1);
    InitControl(m_lblSolomode, "LBL_SOLOMODE", 1);
    m_cActionText.SetEnabledFlag(0);
    m_cActionTextBack.SetEnabledFlag(0);
    InitControl(m_lblName, "LBL_NAME", 1);
    InitControl(m_lblIndicate, "LBL_INDICATE", 1);
    InitControl(m_pbHealth, "PB_HEALTH", 1);
    InitControl(m_lblMapborder, "LBL_MAPBORDER", 1);
    InitControl(m_lblArrow, "LBL_ARROW", 0);
    InitControl(m_lblMap, "LBL_MAP", 0);
    InitControl(lblMapview, "LBL_MAPVIEW", 0);
    StopLoadFromLayout();

    int nSafeX, nSafeY;
    GetManager()->GetSafeMargin(nSafeX, nSafeY);

    m_lblCmbtmodemsg.Initialize(
        CSWGuiExtent(nSafeX, nSafeY, m_pManager->m_nWidth - 2 * nSafeX, 20),
        CSWGuiTextParams(42475, "fnt_d16x16", CGuiInGame::COLOR_RED,
                         CSWGuiTextParams::ALIGNMENT_CENTER, 1.0f),
        CSWGuiBorderParams(CResRef(), CResRef(), CResRef(), 0, 0, 1.0f,
                           Vector(1.0f, 1.0f, 1.0f), 0));
    m_cActionDescriptionState.m_bTargetChanged = 0;
    m_cActionDescriptionState.m_bUnknown = 0;
    m_nInterfaceOffset = 0;
    m_fCombatMessageTime = -1.0f;
    m_lblCmbtmodemsg.m_cText.m_cParams.SetStrRef(42475);
    m_nInterfaceOffset =
        max(m_nInterfaceOffset, m_lblCmbtmodemsg.m_cText.GetIdealHeight());
    m_lblCmbtmodemsg.m_cText.m_cParams.SetStrRef(42478);
    m_nInterfaceOffset =
        max(m_nInterfaceOffset, m_lblCmbtmodemsg.m_cText.GetIdealHeight());
    m_lblCmbtmodemsg.m_cText.m_cParams.SetStrRef(42477);
    m_nInterfaceOffset =
        max(m_nInterfaceOffset, m_lblCmbtmodemsg.m_cText.GetIdealHeight());
    m_lblCmbtmodemsg.m_cText.m_cParams.SetStrRef(42476);
    m_nInterfaceOffset =
        max(m_nInterfaceOffset, m_lblCmbtmodemsg.m_cText.GetIdealHeight());
    m_lblCmbtmodemsg.SetExtent(CSWGuiExtent(nSafeX, nSafeY,
                                            m_pManager->m_nWidth - 2 * nSafeX,
                                            m_nInterfaceOffset));

    m_lblActionQueueBackground.m_cBorder.m_cParams.SetFillImage("dialog2", 0);
    m_lblActionQueueBackground.m_cBorder.m_cParams.SetCornerImage("confirm1", 0);
    m_lblActionQueueBackground.m_cBorder.m_cParams.SetEdgeImage("confirm2", 0);
    m_nInterfaceOffset +=
        m_lblActionQueueBackground.m_cBorder.m_cParams.GetBorderDim();
    m_lblActionQueueBackground.SetExtent(
        CSWGuiExtent(-10, -10, m_pManager->m_nWidth + 20,
                     m_nInterfaceOffset + nSafeY));
    m_lblActionQueueBackground.SetEnabledFlag(0);
    m_nInterfaceOffset -= 8;
    AddControl(&m_lblActionQueueBackground);
    AddControl(&m_lblCmbtmodemsg);

    m_nMapHeight = m_lblMap.m_cExtent.m_nHeight;
    m_nMapWidth = m_lblMap.m_cExtent.m_nWidth;
    m_cMapViewport = lblMapview.m_cExtent;
    m_cActionTextBackExtent = m_cActionTextBack.m_cExtent;
    m_nActionTextMargin =
        m_cActionTextBackExtent.m_nHeight - m_cActionText.m_cExtent.m_nHeight;
    CSWGuiExtent cArrowExtent = m_lblArrow.m_cExtent;
    cArrowExtent.m_nLeft = (m_cMapViewport.m_nWidth - cArrowExtent.m_nWidth) / 2;
    cArrowExtent.m_nTop = (m_cMapViewport.m_nHeight - cArrowExtent.m_nHeight) / 2;
    m_lblArrow.SetExtent(cArrowExtent);

    cArrowExtent = m_cActionText.m_cExtent;
    cArrowExtent.m_nTop -= m_cActionTextBackExtent.m_nTop;
    cArrowExtent.m_nLeft -= m_cActionTextBackExtent.m_nLeft;
    m_cActionText.SetExtent(cArrowExtent);
    cArrowExtent = m_cActionTextBackExtent;
    cArrowExtent.m_nLeft = 0;
    cArrowExtent.m_nTop = 0;
    m_cActionTextBack.SetExtent(cArrowExtent);

    CExoString sSoloMode = m_lblSolomode.m_cText.m_cParams.m_sText;
    g_pTlkTable->ParseStr(sSoloMode);
    m_lblSolomode.m_cText.m_cParams.SetText(sSoloMode);
    int nInnerOffset = m_lblSolomode.m_cBorder.m_cParams.m_nInnerOffset;
    cArrowExtent = m_lblSolomode.m_cExtent;
    int nBorder =
        m_lblSolomode.m_cBorder.m_cParams.GetBorderDim() - nInnerOffset;
    cArrowExtent.m_nHeight =
        m_lblSolomode.m_cText.GetIdealHeight() + nBorder * 2;
    m_lblSolomode.SetExtent(cArrowExtent);

    m_lblReticle.m_cBorder.m_cParams.m_nFillMode = CSWGuiBorderParams::FILL_MODE_BACKGROUND;
    AddControl(&m_lblReticle);
    m_nTargetType = 0;
    m_fTargetUpdateDelay = -1.0f;
    m_fActionDescriptionTimer = -1.0f;
    m_nTargetId = OBJECT_INVALID;
    m_oidPlayerCharacter = OBJECT_INVALID;

    for (nIndex = 0; nIndex < 6; nIndex++) m_aSelectedMenu[nIndex] = 0xff;
    for (int nType = 0; nType < 5; nType++)
        for (nIndex = 0; nIndex < 6; nIndex++)
            m_aSelectedPersonalActions[nType][nIndex] = CSWGuiInterfaceAction::INVALID_ACTION;
    m_nDefaultAction = CSWGuiInterfaceAction::INVALID_ACTION;
    m_nSelectedAction = 0;
    m_cActionDescriptionState.m_bEnabled = 1;
    m_fPortraitTransitionTime = 0.0f;
    ShowActionQueue(0);
    m_nCombatMessage = 42475;
}
#else
CSWGuiMainInterface::CSWGuiMainInterface(CSWGuiManager* pManager)  // PC 0x006E5170
    : CSWGuiPanel(pManager)
{
    CExoString sControl;
    CSWGuiLabel lblMapview;
    int nIndex;
    CSWGuiFloatyText::m_pMainInterface = this;

    m_fScaleX = 1.0f;
    m_fScaleY = 1.0f;
    float fWidthScale = (float)pManager->m_nWidth / (float)g_nGUIWidth;
    float fHeightScale = (float)pManager->m_nHeight / (float)g_nGUIHeight;
    m_fScaleX = fWidthScale;
    m_fScaleY = fHeightScale;
    StartLoadFromLayout("mipc28x6_p");

#ifndef _XBOX
    m_bPositioned = 1;
#endif
    for (nIndex = 0; nIndex < 6; ++nIndex)
        m_aPersonalActions[nIndex].Initialize(this, nIndex, 0);
    for (nIndex = 0; nIndex < 3; ++nIndex)
        m_aCharacters[nIndex].Initialize(this, nIndex + 1);
    m_cTransitionCharacter.Initialize(this, 4);
    for (nIndex = 0; nIndex < 4; ++nIndex)
    {
        sControl.Format("LBL_QUEUE%d", nIndex);
        InitControl(m_aActionIcons[nIndex], sControl, 1);
    }
    for (nIndex = 0; nIndex < GUI_STATUS_COUNT; ++nIndex)
        m_aEffects[nIndex].Initialize(this, nIndex);
    m_cTargetActionMenu.Initialize(this);
    InitControl(m_aMoulding[0], "LBL_MOULDING1", 1);
    InitControl(m_aMoulding[1], "LBL_MOULDING3", 1);
    InitControl(m_aCombatbg[0], "LBL_COMBATBG2", 1);
    InitControl(m_aCombatbg[1], "LBL_COMBATBG3", 1);

    InitControl(m_btnClearone, "BTN_CLEARONE", 1);
    InitControl(m_btnClearone2, "BTN_CLEARONE2", 1);
    InitControl(m_btnClearall, "BTN_CLEARALL", 1);
    InitControl(m_cActionText, "LBL_ACTIONDESC", 1);
    InitControl(m_cActionTextBack, "LBL_ACTIONDESCBG", 1);
    InitControl(m_lblMapborder, "LBL_MAPBORDER", 1);
    InitControl(m_lblArrow, "LBL_ARROW", 0);
    InitControl(m_lblMap, "LBL_MAP", 0);
    InitControl(lblMapview, "LBL_MAPVIEW", 0);
    InitControl(m_btnMinimap, "BTN_MINIMAP", 1);
    InitControl(m_lblCmbtmodemsg, "LBL_CMBTMODEMSG", 1);
    m_cToolTip.Initialize(m_lblCmbtmodemsg.m_cExtent, &m_lblCmbtmodemsg, 1.0f);
    InitControl(m_cToolTipBackground, "LBL_CMBTMSGBG", 1);
    m_lblCmbtmodemsg.SetEnabledFlag(0);
    m_cToolTipBackground.SetEnabledFlag(0);
    m_cToolTip.SetEnabledFlag(0);
    m_cToolTip.m_cText.m_cParams.SetText(g_pTlkTable->GetSimpleString(STRREF_HOTKEY_HELP));
    InitControl(m_btnEqu, "BTN_EQU", 1);
    InitControl(m_btnInv, "BTN_INV", 1);
    InitControl(m_btnChar, "BTN_CHAR", 1);
    InitControl(m_btnAbi, "BTN_ABI", 1);
    InitControl(m_btnMsg, "BTN_MSG", 1);
    InitControl(m_btnJou, "BTN_JOU", 1);
    InitControl(m_btnMap, "BTN_MAP", 1);
    InitControl(m_btnOpt, "BTN_OPT", 1);
    InitControl(m_lblMenubg, "LBL_MENUBG", 1);
    InitControl(m_lblArrowMargin, "LBL_ARROW_MARGIN", 1);
    InitControl(m_tbPause, "TB_PAUSE", 1);
    InitControl(m_tbSolo, "TB_SOLO", 1);
    InitControl(m_tbStealth, "TB_STEALTH", 1);
    InitControl(m_btnSwapWeapons, "BTN_SWAPWEAPONS", 1);
    StopLoadFromLayout();

    {
        CSWGuiExtent cBounds;
        cBounds.m_nLeft = m_lblArrowMargin.m_cExtent.m_nLeft;
        cBounds.m_nTop = m_lblMenubg.m_cExtent.GetBottom();
        cBounds.m_nHeight = m_tbPause.m_cExtent.m_nTop - cBounds.m_nTop;
        cBounds.m_nWidth = m_pManager->m_nWidth - 2 * cBounds.m_nLeft;
        m_cTargetActionMenu.m_cBounds = cBounds;
    }
    AddControl(&m_cToolTip);
    if (pManager->m_nWidth >= 1024)
    {
        CSWGuiExtent cExtent = m_cExtent;
        cExtent.m_nHeight = g_nScreenHeight;
        cExtent.m_nWidth = g_nScreenWidth;
        CSWGuiPanel::SetExtent(cExtent);
    }
    int nSafeX, nSafeY;
    GetManager()->GetSafeMargin(nSafeX, nSafeY);
    m_nMapHeight = m_lblMap.m_cExtent.m_nHeight;
    m_nMapWidth = m_lblMap.m_cExtent.m_nWidth;
    m_cMapViewport = lblMapview.m_cExtent;
    CSWGuiExtent cNewExtent = m_lblArrow.m_cExtent;
    cNewExtent.m_nLeft = (m_cMapViewport.m_nWidth - cNewExtent.m_nWidth) / 2;
    cNewExtent.m_nTop = (m_cMapViewport.m_nHeight - cNewExtent.m_nHeight) / 2;
    m_nInterfaceOffset = 0;
    m_fCombatMessageTime = -1.0f;
    m_lblArrow.SetExtent(cNewExtent);

    m_lblReticle.m_cBorder.m_cParams.m_nFillMode = CSWGuiBorderParams::FILL_MODE_BACKGROUND;
    CSWGuiLabel* pLabel = &m_lblReticle;
#ifndef _XBOX
    pLabel->m_bIgnoreMouse = 1;
#endif
    AddControl(pLabel);
    pLabel = &m_lblStickyReticle;
    pLabel->SetExtent(CSWGuiExtent(0, 0, 16, 16));
    m_lblStickyReticle.m_cBorder.m_cParams.m_nFillMode = CSWGuiBorderParams::FILL_MODE_BACKGROUND;
#ifndef _XBOX
    pLabel->m_bIgnoreMouse = 1;
#endif
    m_lblStickyReticle.SetEnabledFlag(0);
    AddControl(pLabel);
    m_oidStickyTarget = OBJECT_INVALID;
    m_nTargetId = OBJECT_INVALID;
    m_oidPlayerCharacter = OBJECT_INVALID;
    m_fTargetUpdateDelay = -1.0f;
    m_fActionDescriptionTimer = -1.0f;
    for (nIndex = 0; nIndex < 7; nIndex++)
        m_aSelectedPersonalActions[nIndex] = CSWGuiInterfaceAction::INVALID_ACTION;
    m_cActionDescriptionState.m_bEnabled = 1;
    m_fPortraitTransitionTime = 0.0f;
    ShowActionQueue(0);
    m_nCombatMessage = STRREF_COMBAT_MODE_ENGAGED;
    m_bToolTipShown = 0;
    m_btnEqu.AddEvent(GUIEVENT_ACTIVATE, this, (PFNGUIEVENT)&CSWGuiMainInterface::OnMenuButtonPressed);
    m_btnInv.AddEvent(GUIEVENT_ACTIVATE, this, (PFNGUIEVENT)&CSWGuiMainInterface::OnMenuButtonPressed);
    m_btnChar.AddEvent(GUIEVENT_ACTIVATE, this, (PFNGUIEVENT)&CSWGuiMainInterface::OnMenuButtonPressed);
    m_btnAbi.AddEvent(GUIEVENT_ACTIVATE, this, (PFNGUIEVENT)&CSWGuiMainInterface::OnMenuButtonPressed);
    m_btnJou.AddEvent(GUIEVENT_ACTIVATE, this, (PFNGUIEVENT)&CSWGuiMainInterface::OnMenuButtonPressed);
    m_btnMsg.AddEvent(GUIEVENT_ACTIVATE, this, (PFNGUIEVENT)&CSWGuiMainInterface::OnMenuButtonPressed);
    m_btnMap.AddEvent(GUIEVENT_ACTIVATE, this, (PFNGUIEVENT)&CSWGuiMainInterface::OnMenuButtonPressed);
    m_btnOpt.AddEvent(GUIEVENT_ACTIVATE, this, (PFNGUIEVENT)&CSWGuiMainInterface::OnMenuButtonPressed);
    m_btnMinimap.AddEvent(GUIEVENT_ACTIVATE, this, (PFNGUIEVENT)&CSWGuiMainInterface::OnMenuButtonPressed);
    m_btnEqu.SetFocusable(0);
    m_btnInv.SetFocusable(0);
    m_btnChar.SetFocusable(0);
    m_btnAbi.SetFocusable(0);
    m_btnJou.SetFocusable(0);
    m_btnMsg.SetFocusable(0);
    m_btnMap.SetFocusable(0);
    m_btnOpt.SetFocusable(0);
    m_btnMinimap.SetFocusable(0);
    m_btnClearone.AddEvent(GUIEVENT_ACTIVATE, this, (PFNGUIEVENT)&CSWGuiMainInterface::OnClearOneButtonPressed);
    m_btnClearone2.AddEvent(GUIEVENT_ACTIVATE, this, (PFNGUIEVENT)&CSWGuiMainInterface::OnClearOneButtonPressed);
    m_btnClearall.AddEvent(GUIEVENT_ACTIVATE, this, (PFNGUIEVENT)&CSWGuiMainInterface::OnClearAllButtonPressed);
    m_btnClearone.SetFocusable(0);
    m_btnClearone2.SetFocusable(0);
    m_btnClearall.SetFocusable(0);
    m_lblArrowMargin.SetEnabledFlag(0);
    m_lblArrowMargin.SetFocusable(0);
    m_tbPause.AddEvent(GUIEVENT_ACTIVATE, this, (PFNGUIEVENT)&CSWGuiMainInterface::OnPauseButtonPressed);
    m_tbSolo.AddEvent(GUIEVENT_ACTIVATE, this, (PFNGUIEVENT)&CSWGuiMainInterface::OnSoloButtonPressed);
    m_tbStealth.AddEvent(GUIEVENT_ACTIVATE, this, (PFNGUIEVENT)&CSWGuiMainInterface::OnStealthButtonPressed);
    m_tbPause.SetFocusable(0);
    m_tbSolo.SetFocusable(0);
    m_tbStealth.SetFocusable(0);
    m_btnSwapWeapons.AddEvent(GUIEVENT_ACTIVATE, this, (PFNGUIEVENT)&CSWGuiMainInterface::OnSwapWeaponsPressed);
    m_btnSwapWeapons.SetFocusable(0);
    m_pBlackDot = NewCAurGUIImage("blackdot");
    m_nDefaultAction = 0;
    m_lblCmbtmodemsg.m_cText.m_cParams.SetText("");
    m_nActionTextBottom = m_cActionText.m_cExtent.GetBottom();
    SetActionDescription("");
    UpdateActionDescription();
    m_btnEqu.SetToolTip(STRREF_MENU_EQUIPMENT, GUIEVENT_MENU_EQUIPMENT);
    m_btnInv.SetToolTip(STRREF_MENU_INVENTORY, GUIEVENT_MENU_INVENTORY);
    m_btnChar.SetToolTip(STRREF_MENU_CHARACTER, GUIEVENT_MENU_CHARACTER);
    m_btnMap.SetToolTip(STRREF_MENU_MAP, GUIEVENT_MENU_MAP);
    m_btnMinimap.SetToolTip(STRREF_MENU_MAP, GUIEVENT_MENU_MAP);
    m_btnAbi.SetToolTip(STRREF_MENU_ABILITIES, GUIEVENT_MENU_ABILITIES);
    m_btnJou.SetToolTip(STRREF_MENU_JOURNAL, GUIEVENT_MENU_JOURNAL);
    m_btnOpt.SetToolTip(STRREF_MENU_OPTIONS, GUIEVENT_MENU_OPTIONS);
    m_btnMsg.SetToolTip(STRREF_MENU_MESSAGES, GUIEVENT_MENU_MESSAGES);
    m_tbPause.SetToolTip(STRREF_PAUSE, GUIEVENT_PAUSE);
    m_tbSolo.SetToolTip(STRREF_SOLO, GUIEVENT_SOLO);
    m_tbStealth.SetToolTip(STRREF_STEALTH, GUIEVENT_STEALTH);
    m_btnClearone.SetToolTip(STRREF_CLEAR_ONE, GUIEVENT_CLEAR_ONE);
    m_btnClearone2.SetToolTip(STRREF_CLEAR_ONE, GUIEVENT_CLEAR_ONE);
    m_btnClearall.SetToolTip(STRREF_CLEAR_ALL, GUIEVENT_CLEAR_ALL);
    m_btnSwapWeapons.SetToolTip(STRREF_SWITCH_WEAPONS, GUIEVENT_SWITCH_WEAPONS);
    m_bSelectingUseItemTarget = 0;
    m_nUseItemTargetMember = 0;
    m_oidUseItemPending = OBJECT_INVALID;
    m_fFormMessageTime = 0.0f;
    m_nFormMessage = 0;

    float fScaleX = 1.0f;
    float fScaleY = 1.0f;
    if (g_pAppManager->GetClientExoApp()->GetSWGuiManager() != NULL)
    {
        fScaleX = (float)g_pAppManager->GetClientExoApp()->GetSWGuiManager()->m_nWidth / (float)g_nGUIWidth;
        fScaleY = (float)g_pAppManager->GetClientExoApp()->GetSWGuiManager()->m_nHeight / (float)g_nGUIHeight;
    }
    cNewExtent.m_nLeft = (int)(fScaleX * CANCEL_ITEM_SELECT_LEFT);
    cNewExtent.m_nTop = (int)(fScaleY * CANCEL_ITEM_SELECT_TOP);
    cNewExtent.m_nHeight = (int)(fScaleY * CANCEL_ITEM_SELECT_HEIGHT);
    cNewExtent.m_nWidth = (int)(fScaleX * CANCEL_ITEM_SELECT_WIDTH);
    m_btnCancelPartyItemSelect.m_cBorder.m_cParams.SetCornerImage("uibit_brdr_16bct", 0);
    m_btnCancelPartyItemSelect.m_cBorder.m_cParams.SetEdgeImage("uibit_brdr_16bet", 0);
    m_btnCancelPartyItemSelect.m_cBorder.m_cParams.SetFillImage("uibit_fill_2bt", 0);
    m_btnCancelPartyItemSelect.m_cBorder.m_cParams.m_nBorderDim = 16;
    m_btnCancelPartyItemSelect.m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_GREEN;
    m_btnCancelPartyItemSelect.m_cBorder.m_cParams.m_nInnerOffset = 10;
    m_btnCancelPartyItemSelect.m_cBorder.m_cParams.m_n8 = 12;
    m_btnCancelPartyItemSelect.m_cHilite.m_cParams.SetCornerImage("uibit_brdr_16bct", 0);
    m_btnCancelPartyItemSelect.m_cHilite.m_cParams.SetEdgeImage("uibit_brdr_16bet", 0);
    m_btnCancelPartyItemSelect.m_cHilite.m_cParams.SetFillImage("uibit_fill_2bt", 0);
    m_btnCancelPartyItemSelect.m_cHilite.m_cParams.m_nBorderDim = 16;
    m_btnCancelPartyItemSelect.m_cHilite.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_GREEN;
    m_btnCancelPartyItemSelect.m_cHilite.m_cParams.m_nInnerOffset = 10;
    m_btnCancelPartyItemSelect.m_cHilite.m_cParams.m_n8 = 12;
    m_btnCancelPartyItemSelect.AddEvent(GUIEVENT_ACTIVATE, this, (PFNGUIEVENT)&CSWGuiMainInterface::OnCancelPartyItemSelect);
    m_btnCancelPartyItemSelect.SetFocusable(0);
    m_btnCancelPartyItemSelect.SetExtent(cNewExtent);
    AddControl(&m_btnCancelPartyItemSelect);
    m_bPersonalActionScrolled = 0;
}
#endif
#ifndef _XBOX
CSWGuiMainInterface::~CSWGuiMainInterface()  // PC 0x006E4270
{
#ifndef _XBOX
    if (m_pBlackDot != NULL)
    {
        delete m_pBlackDot;
    }
#endif
}
#endif
CSWGuiMainInterface* CSWGuiFloatyText::m_pMainInterface;
// PC 0x006E45C0
void CSWGuiMainInterface::Draw(float fElapsed)
{
    if (!m_bNeedsDraw)
        return;

    CSWCCreature* pPlayer = ClientExoAppLate3(g_pAppManager)->GetSWParty()->GetPlayerCharacter();
    if (pPlayer == NULL || pPlayer->GetId() == OBJECT_INVALID)
        return;

    if (m_fActionDescriptionTimer != -1.0f)
    {
        m_fActionDescriptionTimer -= fElapsed;
        if (m_fActionDescriptionTimer <= 0.0f)
            m_fActionDescriptionTimer = -1.0f;
    }
#ifndef _XBOX
    UpdatePortraits();
    UpdateGameMenu();
    float fIndicatorElapsed = m_pManager->m_fGuiScale;
    UpdateIndicator(fIndicatorElapsed);
    m_cTargetActionMenu.Update(fElapsed);
#endif
    PopulateMenus(1);
#ifndef _XBOX
    if (m_bSelectingUseItemTarget)
    {
        CSWSCreature* pServerCreature = pPlayer->GetServerCreature();
        if (pServerCreature != NULL &&
            (pServerCreature->IsDebilitated(0) || ((CSWSObject*)pServerCreature)->GetDead() ||
             pServerCreature->GetDeadTemp()))
        {
            m_bSelectingUseItemTarget = 0;
        }
    }
    int bSelectingUseItemTarget = m_bSelectingUseItemTarget;
    if (bSelectingUseItemTarget)
    {
        m_fFormMessageTime = 0.0f;
        m_nFormMessage = 0;
        CExoString sText;
        int nMember = m_nUseItemTargetMember;
        if (nMember == -1)
        {
            sText = g_pAppManager->m_pClientExoApp->GetGUIString(STRREF_USE_ITEM_SELECT_MEMBER);
            sText = sText + "\n" + g_pAppManager->m_pClientExoApp->GetGUIString(STRREF_USE_ITEM_CANCEL);
        }
        else
        {
            if (nMember == 2 &&
                g_pAppManager->m_pClientExoApp->GetSWParty()->m_nCharacters != 3)
            {
                nMember = 1;
            }
            CSWCCreature* pCharacter = g_pAppManager->m_pClientExoApp->GetSWParty()->GetCharacter(nMember);
            CSWSItem* pItem = g_pAppManager->m_pServerExoApp->GetItemByGameObjectID(
                g_pAppManager->m_pServerExoApp->ClientToServerObjectId(m_oidUseItemPending));
            if (pCharacter->GetServerCreature()->CanUseItem(pItem, 0, 1))
            {
                sText = g_pAppManager->m_pClientExoApp->GetGUIString(STRREF_USE_ITEM_ON);
                sText = sText + " " + pCharacter->GetName();
                sText = sText + "\n" + g_pAppManager->m_pClientExoApp->GetGUIString(STRREF_USE_ITEM_CANCEL);
            }
            else
            {
                sText = g_pAppManager->m_pClientExoApp->GetGUIString(STRREF_CANNOT_USE_ITEM_ON);
                sText = sText + " " + pCharacter->GetName();
                sText = sText + "\n" + g_pAppManager->m_pClientExoApp->GetGUIString(STRREF_USE_ITEM_CANCEL);
            }
        }
        m_btnCancelPartyItemSelect.m_cText.m_cParams.SetText(sText);
        m_btnCancelPartyItemSelect.m_cText.m_cParams.SetDefaultColor(CGuiInGame::COLOR_NEW_HIGHLIGHT);
        CSWGuiButton* pCancel = &m_btnCancelPartyItemSelect;
        pCancel->SetEnabled(1);
        pCancel->SetEnabledFlag(1);
        pCancel->Draw(fElapsed);
    }
    else if (m_fFormMessageTime > 0.0f)
    {
        m_fFormMessageTime -= fElapsed;
        if (m_fFormMessageTime <= 0.0f)
        {
            m_fFormMessageTime = 0.0f;
        }
        else
        {
            m_btnCancelPartyItemSelect.m_cText.m_cParams.SetText(
                g_pAppManager->m_pClientExoApp->GetGUIString(m_nFormMessage));
            m_btnCancelPartyItemSelect.m_cText.m_cParams.SetDefaultColor(CGuiInGame::COLOR_NEW_HIGHLIGHT);
            CSWGuiButton* pCancel = &m_btnCancelPartyItemSelect;
            pCancel->SetEnabled(0);
            pCancel->SetEnabledFlag(1);
            pCancel->Draw(fElapsed);
        }
    }
    else
    {
        m_btnCancelPartyItemSelect.SetEnabledFlag(0);
    }
#endif
    for (int nStatus = 0; nStatus < GUI_STATUS_COUNT; ++nStatus)
    {
        float fStatusElapsed = m_pManager->m_fGuiScale;
        m_aEffects[nStatus].Update(fStatusElapsed);
    }

    if (m_fPortraitTransitionTime > 0.0f)
    {
        m_fPortraitTransitionTime -= fElapsed;
        if (m_fPortraitTransitionTime > 0.0f)
        {
            for (int nPortrait = 0; nPortrait < 3; ++nPortrait)
                m_aCharacters[nPortrait].Adjust(m_fPortraitTransitionTime);
            m_cTransitionCharacter.Adjust(m_fPortraitTransitionTime);
        }
        else
        {
            for (int nPortrait = 0; nPortrait < 3; ++nPortrait)
                m_aCharacters[nPortrait].Adjust(0.25f);
            m_cTransitionCharacter.Adjust(0.25f);
            m_cTransitionCharacter.Show(0, 0, 0, 0);
            m_fPortraitTransitionTime = 0.0f;
        }
    }
#ifdef _XBOX
    UpdatePortraits();
    float fIndicatorElapsed = m_pManager->m_fGuiScale;
    UpdateIndicator(fIndicatorElapsed);
#endif
    ShowActionQueue(ClientExoAppLate3(g_pAppManager)->GetCombatMode());
    for (int nAction = 0; nAction < 7; ++nAction)
        m_aPersonalActions[nAction].Update(fElapsed);
    if (m_cActionDescriptionState.m_bEnabled)
        UpdateActionQueue();

    if (g_bShowIndicator)
    {
        DrawMap(fElapsed);
#ifdef _XBOX
        DrawActionDescription(fElapsed);
#endif
        CSWGuiPanel::Draw(fElapsed);
#ifndef _XBOX
        m_cTargetActionMenu.Draw(fElapsed);
#endif
        AurGUIStartLayer();
        if (AurGUISetupViewport(m_cExtent.m_nLeft, m_cExtent.m_nTop,
                                m_cExtent.m_nWidth, m_cExtent.m_nHeight,
                                m_vColor, false, m_fAlpha))
        {
            for (int nText = m_lFloatyText.num - 1; nText >= 0; --nText)
            {
                if (m_lFloatyText[nText] != NULL)
                    m_lFloatyText[nText]->Draw(fElapsed);
            }
            AurGUICloseViewport();
        }
        for (int nPortrait = 0; nPortrait < 3; ++nPortrait)
        {
            int bLarge = nPortrait == 0;
            m_aCharacters[nPortrait].DrawEffects(fElapsed, bLarge);
        }
        CSWGuiMainInterfaceChar* pTransitionCharacter = &m_cTransitionCharacter;
        pTransitionCharacter->DrawEffects(fElapsed, 0);
#ifdef _XBOX
        for (int nAction = 0; nAction < 6; ++nAction)
            m_aPersonalActions[nAction].Draw(fElapsed);
#endif
        AurGUIStopLayer();
    }
}
// PC 0x006E2370
void CSWGuiMainInterface::DrawMap(float fElapsed)
{
    CClientOptions* pOptions = g_pAppManager->GetClientExoApp()->GetClientOptions();
    if (pOptions == NULL || pOptions->m_bMiniMap)
    {
#ifndef _XBOX
        if (g_pAppManager->GetServerExoApp()->GetPartyTable()->m_bDisableMap)
        {
            m_btnMinimap.SetEnabledFlag(0);
            m_lblMap.SetEnabledFlag(0);
            m_lblMapborder.SetEnabledFlag(0);
            return;
        }
#endif
        CSWSAreaMap* pMap = g_pAppManager->GetServerExoApp()->GetModule()->GetAreaMap();
        if (pMap != NULL && pMap->m_nExploredSize > 0)
        {
#ifndef _XBOX
            if (m_lblMap.m_cBorder.m_cParams.m_cFillImage == "")
                InitializeMap();
#endif

            CSWGuiExtent cExtent = m_lblMap.m_cExtent;
            float fScale = pMap->m_fInverseZoom;
            int nMapX;
            int nMapY;
            pMap->GetPartyMemberMapLocation(0, nMapX, nMapY);
#ifndef _XBOX
            nMapX = (int)((float)m_nMapWidth * nMapX / MINIMAP_SOURCE_WIDTH);
            nMapY = (int)((float)m_nMapHeight * nMapY / MINIMAP_SOURCE_HEIGHT);
#endif
            cExtent.m_nLeft = (int)(m_cMapViewport.m_nWidth / 2 - nMapX * fScale);
            cExtent.m_nTop = (int)(m_cMapViewport.m_nHeight / 2 - nMapY * fScale);
            m_lblMap.SetExtent(cExtent);

#ifdef _XBOX
            m_lblArrow.m_cBorder.m_cParams.m_fRotation =
                pMap->GetPartyMemberMapOrientation(0);
#else
            m_lblArrow.m_cBorder.m_cParams.m_fRotation =
                pMap->GetMapRotateCCWFromWorldOrientation(
                    -g_pAppManager->GetClientExoApp()->GetModuleCamera()->GetOrientation().zdir());
#endif

            AurGUIStartLayer();
            if (AurGUISetupViewport(m_cMapViewport.m_nLeft, m_cMapViewport.m_nTop,
                                    m_cMapViewport.m_nWidth, m_cMapViewport.m_nHeight,
                                    CGuiInGame::COLOR_BLACK, false, m_fAlpha))
            {
                m_lblMap.Draw(fElapsed);
#ifndef _XBOX
                UpdateAndDrawFogOfWar(cExtent);
#endif
                m_lblArrow.Draw(fElapsed);
                AurGUICloseViewport();
            }
            AurGUIStopLayer();
#ifndef _XBOX
            m_btnMinimap.SetEnabledFlag(1);
            m_lblMapborder.SetEnabledFlag(1);
            return;
#endif
        }
    }
#ifndef _XBOX
    m_btnMinimap.SetEnabledFlag(0);
#endif
}

#ifndef _XBOX
void CSWGuiMainInterface::UpdateAndDrawFogOfWar(CSWGuiExtent const& cExtent)  // PC 0x006DE950
{
    CSWSAreaMap* pMap = g_pAppManager->GetServerExoApp()->GetModule()->GetAreaMap();
    if (pMap != NULL && pMap->m_nExploredSize > 0)
    {
        unsigned long nMapSize;
        int nMapWidth;
        int nMapHeight;
        unsigned long* pMapData = pMap->GetMapData(nMapSize, nMapWidth, nMapHeight);
        if (pMapData != NULL)
        {
            float fScaleX = (float)CSWSAreaMap::MAP_PIXEL_WIDTH / nMapWidth;
            float fScaleY = (float)CSWSAreaMap::MAP_PIXEL_HEIGHT / nMapHeight;
            float fDotWidth = fScaleX * FOG_POINT_SPAN;
            float fDotHeight = fScaleY * FOG_POINT_SPAN;
            float fInverseX = 1.0f / fScaleX;
            int nOffsetX = (int)(fDotWidth * fInverseX * -0.5f);
            int nStartX = nOffsetX - (int)(cExtent.m_nLeft * fInverseX);
            int nEndX = nStartX + (int)(m_cMapViewport.m_nWidth * fInverseX) - 2 * nOffsetX;
            float fTop = (float)cExtent.m_nTop;
            float fInverseY = 1.0f / fScaleY;
            int nOffsetY = (int)(fDotHeight * fInverseY * -0.5f);
            int nStartY = nOffsetY - (int)(fTop * fInverseY);
            int nEndY = nStartY + (int)(m_cMapViewport.m_nHeight * fInverseY) - 2 * nOffsetY;
            float fY = fTop + nStartY * fScaleY;
            int nRowStart = nStartY * (nMapWidth + 1);
            if (nStartY < 0)
            {
                nRowStart += -nStartY * (nMapWidth + 1);
                fY -= nStartY * fScaleY;
                nStartY = 0;
            }
            if (nEndY > nMapHeight)
                nEndY = nMapHeight;
            if (nStartX < 0)
                nStartX = 0;
            if (nEndX > nMapWidth)
                nEndX = nMapWidth;

            for (int nY = nStartY; nY <= nEndY; nY++)
            {
                float fX = cExtent.m_nLeft + nStartX * fScaleX;
                for (int nX = nStartX; nX <= nEndX; nX++)
                {
                    int nPixel = nRowStart + nX;
                    if ((pMapData[nPixel / CSWSAreaMap::EXPLORED_BITS_PER_WORD] &
                         (1 << (nPixel % CSWSAreaMap::EXPLORED_BITS_PER_WORD))) == 0)
                    {
                        m_pBlackDot->DrawBuffered(
                            (fX - fDotWidth * 0.5f) / m_cMapViewport.m_nWidth,
                            (fY - fDotHeight * 0.5f) / m_cMapViewport.m_nHeight,
                            fDotWidth / m_cMapViewport.m_nWidth,
                            fDotHeight / m_cMapViewport.m_nHeight,
                            0, 0.0f, Vector(-1.0f, -1.0f, -1.0f), 1.0f);
                    }
                    fX += fScaleX;
                }
                fY += fScaleY;
                nRowStart += nMapWidth + 1;
            }
            CAurGUIImage::FlushBuffer(CAurGUIImage::cm_fAlpha);
        }
    }
}
#endif

#ifndef _XBOX
void CSWGuiMainInterface::OnMenuButtonPressed(CSWGuiControl* pControl)  // PC 0x006DEC10
{
    if (pControl->m_bSelected != 0)
    {
        CSWGuiButton* pButton = pControl->AsButton();
        int nMenu;

        if (pButton == &m_btnEqu)
        {
            nMenu = 0;
        }
        else if (pButton == &m_btnInv)
        {
            nMenu = 1;
        }
        else if (pButton == &m_btnChar)
        {
            nMenu = 2;
        }
        else if (pButton == &m_btnAbi)
        {
            nMenu = 3;
        }
        else if (pButton == &m_btnMsg)
        {
            nMenu = 4;
        }
        else if (pButton == &m_btnJou)
        {
            nMenu = 5;
        }
        else if (pButton == &m_btnMap ||
                 pButton == &m_btnMinimap)
        {
            nMenu = 6;
        }
        else
        {
            nMenu = 7;
        }

        if (g_pAppManager->m_pClientExoApp->GetInGameGui()->ShowSWInGameGui(
                nMenu) != 0)
        {
#ifdef _XBOX
            g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GUI);
#else
            g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GUI, 1);
#endif
        }
    }
}
#endif

#ifndef _XBOX
void CSWGuiMainInterface::OnTargetActionFocus(CSWGuiControl* pControl)  // PC 0x006DED00
{
    if (pControl->m_bSelected != 0)
    {
        m_cTargetActionMenu.m_nSelectedAction = pControl->m_nIndex - 6;
    }
}
#endif

#ifndef _XBOX
void CSWGuiMainInterface::OnTargetUpArrowPressed(CSWGuiControl* pControl)  // PC 0x006E28D0
{
    if (pControl->m_bSelected != 0)
    {
        ResetPartyMemberItemUse();
        int nIndex = pControl->m_nIndex;
        if (g_pAppManager->m_pClientExoApp->GetClientOptions()
                ->m_bAutopauseActionMenu != 0)
        {
            g_pAppManager->m_pClientExoApp->SetAutoPaused(1, 7);
        }
        g_pAppManager->m_pClientExoApp->ShowTutorialWindow(5, OBJECT_INVALID,
                                                           OBJECT_INVALID, 0);
        m_cTargetActionMenu.SelectPrevAction(nIndex - 6);
    }
}
#endif

#ifndef _XBOX
void CSWGuiMainInterface::OnTargetDownArrowPressed(CSWGuiControl* pControl)  // PC 0x006E2950
{
    if (pControl->m_bSelected != 0)
    {
        ResetPartyMemberItemUse();
        int nIndex = pControl->m_nIndex;
        if (g_pAppManager->m_pClientExoApp->GetClientOptions()
                ->m_bAutopauseActionMenu != 0)
        {
            g_pAppManager->m_pClientExoApp->SetAutoPaused(1, 7);
        }
        g_pAppManager->m_pClientExoApp->ShowTutorialWindow(5, OBJECT_INVALID,
                                                           OBJECT_INVALID, 0);
        m_cTargetActionMenu.SelectNextAction(nIndex - 6);
    }
}
#endif

#ifndef _XBOX
void CSWGuiMainInterface::OnPauseButtonPressed(CSWGuiControl* pControl)  // PC 0x006DED20
{
    if (pControl->m_bSelected != 0)
    {
        int bPausedByCombat =
            g_pAppManager->m_pClientExoApp->GetPausedByCombat();

        g_pAppManager->m_pClientExoApp->SetPausedByCombat(bPausedByCombat == 0,
                                                          4, 0);
        if (g_pAppManager->m_pClientExoApp->GetAutoPaused() != 0)
        {
            g_pAppManager->m_pClientExoApp->SetAutoPaused(0, 0);
        }
        g_pAppManager->m_pClientExoApp->ShowTutorialWindow(6, OBJECT_INVALID,
                                                           OBJECT_INVALID, 0);
    }
}
#endif

#ifndef _XBOX
void CSWGuiMainInterface::OnSoloButtonPressed(CSWGuiControl* pControl)  // PC 0x006DEDA0
{
    if (pControl->m_bSelected != 0)
    {
        g_pAppManager->m_pClientExoApp->GetInGameGui()->ShowSoloModeQuery(0);
    }
}
#endif

#ifndef _XBOX
void CSWGuiMainInterface::OnStealthButtonPressed(CSWGuiControl* pControl)  // PC 0x006DEDD0
{
    if (pControl->m_bSelected != 0)
    {
        CSWCCreature* pPlayer =
            g_pAppManager->m_pClientExoApp->GetPlayerCreature();
        if (pPlayer->IsStealthCapable() != 0)
        {
            g_pAppManager->m_pClientExoApp->GetSWParty()
                ->GetPlayerCharacter()
                ->PartyMenuStealth(0, NULL);
        }
    }
}
#endif

#ifndef _XBOX
void CSWGuiMainInterface::OnCharacterClicked(CSWGuiControl* pControl)  // PC 0x006E29D0
{
    if (pControl->m_bSelected != 0)
    {
        if (m_bSelectingUseItemTarget)
        {
            int nMember = pControl->m_nIndex - 1;
            if (nMember == 2 &&
                g_pAppManager->m_pClientExoApp->GetSWParty()->m_nCharacters != 3)
            {
                nMember = 1;
            }
            m_nUseItemTargetMember = nMember;
            CSWCCreature* pLeader =
                g_pAppManager->m_pClientExoApp->GetSWParty()->GetCharacter(0);
            if (pLeader != NULL)
            {
                pLeader->PartyMenuUseItemSelect(
                    m_oidUseItemPending | OBJECT_LISTTYPE_MASK, NULL);
                return;
            }
            ResetPartyMemberItemUse();
            return;
        }
        int nButton = pControl->m_nIndex - 1;

        if (nButton == 0)
        {
            CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
            CSWCCreature* pCreature = pClient->GetSWParty()->GetCharacter(0);
            if (pCreature != NULL)
            {
                CSWSCreature* pServerCreature = pCreature->GetServerCreature();
                if (pServerCreature != NULL)
                {
                    if (pServerCreature->m_pStats->CanLevelUp() != 0)
                    {
                        m_btnChar.m_bSelected = 1;
                        OnMenuButtonPressed(&m_btnChar);
                    }
                    else
                    {
                        m_btnEqu.m_bSelected = 1;
                        OnMenuButtonPressed(&m_btnEqu);
                    }
                }
            }
        }
        else if (nButton < CSWParty::MAX_MEMBERS)
        {
            int nIndex = pControl->m_nIndex - 1;
            if (g_pAppManager->m_pClientExoApp->GetSWParty()->m_nCharacters <
                CSWParty::MAX_MEMBERS)
            {
                nIndex = 1;
            }

            CSWCCreature* pCreature =
                g_pAppManager->m_pClientExoApp->GetSWParty()->GetCharacter(
                    nIndex);
            if (pCreature != NULL && pCreature->GetServerCreature() != NULL &&
                pCreature->GetServerCreature()->GetDeadTemp() == 0)
            {
                g_pAppManager->m_pClientExoApp->ChangeCharacter(nIndex, 1);
            }
        }
    }
}
#endif
// PC 0x006E4EB0
void CSWGuiMainInterface::AddFloatyText(unsigned long oidObject,
                                        const CExoString& sText,
                                        const Vector& vColor, float fDuration)
{
    Swguimaininterfacepc2Trace3();
    CSWGuiFloatyText* pFloatyText = new CSWGuiFloatyText;
    pFloatyText->CreateEntry(oidObject, sText, vColor, fDuration);

    for (int i = m_lFloatyText.num - 1; i >= 0; i--)
    {
        CSWGuiFloatyText* pOldText = m_lFloatyText.element[i];
        if (pOldText->m_oidObject == oidObject)
        {
            if (++pOldText->m_bActive <= SWGUI_FLOATY_TEXT_MAX_LINES)
            {
                pOldText->m_cExtent.m_nTop -= pOldText->m_cExtent.m_nHeight;
                pOldText->SetExtent(pOldText->m_cExtent);
            }
            else
            {
                delete m_lFloatyText.element[i];
                m_lFloatyText.DelIndex(i);
            }
        }
    }

    m_lFloatyText.Add(pFloatyText);
}

#ifndef _XBOX
void CSWGuiMainInterface::OnDefaultActionLeft(CSWGuiControl* pControl)  // PC 0x006E3980
{
    Swguimaininterfacepc2Trace3();
    if (pControl->m_bSelected != 0)
    {
        int nIndex = pControl->m_nIndex;
        if (nIndex >= 0 && nIndex < 6)
        {
            DoPersonalAction(nIndex, 0);
            return;
        }

        DoTargetAction(nIndex - 6, 1);
    }
}
#endif

#ifdef _XBOX
void CSWGuiMainInterfaceAction::Initialize(CSWGuiMainInterface* pMainInterface,
                                          int nAction)
{
    CExoString sControl;
    CSWGuiLabel cLabel;

    sControl.Format("LBL_ICON%d", nAction);
    pMainInterface->InitControl(m_lblIcon, sControl, 1);
    m_cIconExtent = m_lblIcon.m_cExtent;
    sControl.Format("LBH_BORDER%d", nAction);
    pMainInterface->InitControl(m_lbhBorder, sControl, 1);
    m_cBorderExtent = m_lbhBorder.m_cExtent;
    sControl.Format("LBH_ARROW%d", nAction);
    pMainInterface->InitControl(m_lbhArrow, sControl, 1);
    m_cArrowExtent = m_lbhArrow.m_cExtent;
    sControl.Format("LBL_ICON%dB", nAction);
    pMainInterface->InitControl(cLabel, sControl, 0);
    m_cIconBExtent = cLabel.m_cExtent;
    sControl.Format("LBH_BORDER%dB", nAction);
    pMainInterface->InitControl(cLabel, sControl, 0);
    m_cBorderBExtent = cLabel.m_cExtent;
    sControl.Format("LBH_ARROW%dB", nAction);
    pMainInterface->InitControl(cLabel, sControl, 0);
    m_cArrowBExtent = cLabel.m_cExtent;
    m_lbhBorder.AddEvent(GUIEVENT_MOUSE_ENTER, pMainInterface,
        (PFNGUIEVENT)&CSWGuiMainInterface::UpdateActionDescription);
}
#else
void CSWGuiMainInterfaceAction::Initialize(CSWGuiMainInterface* pMainInterface,  // PC 0x006E3B70
                                          int nAction, int bTarget)
{

    CExoString sControl;
    CSWGuiLabel cLabel;
    m_bTarget = bTarget;
    CExoString sType(bTarget ? "TARGET" : "ACTION");

    sControl.Format("BTN_%s%d", sType.CStr(), nAction);
    int bAddToPanel = !bTarget;
    pMainInterface->InitControl(m_CSWGuiButton_0, sControl, bAddToPanel);
    sControl.Format("LBL_%s%d", sType.CStr(), nAction);
    pMainInterface->InitControl(m_CSWGuiButton_1C4, sControl, bAddToPanel);
    sControl.Format("BTN_%sUP%d", sType.CStr(), nAction);
    pMainInterface->InitControl(m_CSWGuiButton_388, sControl, bAddToPanel);
    sControl.Format("BTN_%sDOWN%d", sType.CStr(), nAction);
    pMainInterface->InitControl(m_CSWGuiButton_54C, sControl, bAddToPanel);

    if (m_bTarget)
    {
        m_CSWGuiButton_0.AddChildControl(&m_CSWGuiButton_1C4);
#ifndef _XBOX
        m_CSWGuiButton_1C4.m_pParent = &m_CSWGuiButton_0;
#endif
        m_CSWGuiButton_0.AddChildControl(&m_CSWGuiButton_388);
#ifndef _XBOX
        m_CSWGuiButton_388.m_pParent = &m_CSWGuiButton_0;
#endif
        m_CSWGuiButton_0.AddChildControl(&m_CSWGuiButton_54C);
#ifndef _XBOX
        m_CSWGuiButton_54C.m_pParent = &m_CSWGuiButton_0;
#endif
        nAction += 6;
    }

    PFNGUIEVENT pUp;
    PFNGUIEVENT pDown;
    PFNGUIEVENT pFocus;
    if (bTarget)
    {
        pUp = (PFNGUIEVENT)&CSWGuiMainInterface::OnTargetUpArrowPressed;
        pDown = (PFNGUIEVENT)&CSWGuiMainInterface::OnTargetDownArrowPressed;
        pFocus = (PFNGUIEVENT)&CSWGuiMainInterface::OnTargetActionFocus;
    }
    else
    {
        pUp = (PFNGUIEVENT)&CSWGuiMainInterface::OnActionUpArrowPressed;
        pDown = (PFNGUIEVENT)&CSWGuiMainInterface::OnActionDownArrowPressed;
        pFocus = (PFNGUIEVENT)&CSWGuiMainInterface::OnDefaultActionFocus;
    }

    m_CSWGuiButton_1C4.SetFocusable(0);
    m_CSWGuiButton_1C4.AddEvent(GUIEVENT_ACTIVATE, pMainInterface,
        (PFNGUIEVENT)&CSWGuiMainInterface::OnDefaultActionLeft);
    m_CSWGuiButton_1C4.AddEvent(GUIEVENT_MOUSE_ENTER, pMainInterface, pFocus);
    m_CSWGuiButton_1C4.AddEvent(GUIEVENT_MOUSE_WHEEL_UP, pMainInterface, pUp);
    m_CSWGuiButton_1C4.AddEvent(GUIEVENT_MOUSE_WHEEL_DOWN, pMainInterface, pDown);
    m_CSWGuiButton_1C4.m_nIndex = nAction;

    m_CSWGuiButton_0.SetFocusable(0);
    m_CSWGuiButton_0.AddEvent(GUIEVENT_MOUSE_ENTER, pMainInterface, pFocus);
    m_CSWGuiButton_0.AddEvent(GUIEVENT_MOUSE_WHEEL_UP, pMainInterface, pUp);
    m_CSWGuiButton_0.AddEvent(GUIEVENT_MOUSE_WHEEL_DOWN, pMainInterface, pDown);
    m_CSWGuiButton_0.AddEvent(GUIEVENT_MOUSE_EXIT, pMainInterface,
        (PFNGUIEVENT)&CSWGuiMainInterface::OnPersonalActionExit);
    m_CSWGuiButton_0.m_nIndex = nAction;

    m_CSWGuiButton_388.SetFocusable(0);
    m_CSWGuiButton_388.AddEvent(GUIEVENT_ACTIVATE, pMainInterface, pUp);
    m_CSWGuiButton_388.AddEvent(GUIEVENT_MOUSE_ENTER, pMainInterface, pFocus);
    m_CSWGuiButton_388.AddEvent(GUIEVENT_MOUSE_WHEEL_UP, pMainInterface, pUp);
    m_CSWGuiButton_388.AddEvent(GUIEVENT_MOUSE_WHEEL_DOWN, pMainInterface, pDown);
    m_CSWGuiButton_388.m_nIndex = nAction;

    m_CSWGuiButton_54C.SetFocusable(0);
    m_CSWGuiButton_54C.AddEvent(GUIEVENT_ACTIVATE, pMainInterface, pDown);
    m_CSWGuiButton_54C.AddEvent(GUIEVENT_MOUSE_ENTER, pMainInterface, pFocus);
    m_CSWGuiButton_54C.AddEvent(GUIEVENT_MOUSE_WHEEL_UP, pMainInterface, pUp);
    m_CSWGuiButton_54C.AddEvent(GUIEVENT_MOUSE_WHEEL_DOWN, pMainInterface, pDown);
    m_CSWGuiButton_54C.m_cBorder.m_cParams.m_fRotation = 180.0f;
    m_CSWGuiButton_54C.m_cHilite.m_cParams.m_fRotation = 180.0f;
    m_CSWGuiButton_54C.m_nIndex = nAction;

    m_CSWGuiButton_388.SetToolTip(STRREF_ACTION_MENU_UP, GUIEVENT_ACTION_MENU_UP);
    m_CSWGuiButton_54C.SetToolTip(STRREF_ACTION_MENU_DOWN, GUIEVENT_ACTION_MENU_DOWN);
    switch (nAction)
    {
    case 4:
        m_CSWGuiButton_1C4.m_nToolTipStrRef = STRREF_SELECT_BEHAVIOR;
        break;
    case 5:
        m_CSWGuiButton_1C4.m_nToolTipStrRef = STRREF_ACTIVATE_FORM;
        break;
    case 0:
        m_CSWGuiButton_1C4.SetToolTip(STRREF_ACTIVATE_FRIENDLY_POWER,
                                    GUIEVENT_ACTIVATE_FRIENDLY_POWER);
        break;
    case 1:
        m_CSWGuiButton_1C4.SetToolTip(STRREF_ACTIVATE_MEDICAL_ITEM,
                                    GUIEVENT_ACTIVATE_MEDICAL_ITEM);
        break;
    case 2:
        m_CSWGuiButton_1C4.SetToolTip(STRREF_ACTIVATE_NON_MEDICAL_ITEM,
                                    GUIEVENT_ACTIVATE_NON_MEDICAL_ITEM);
        break;
    case 3:
        m_CSWGuiButton_1C4.SetToolTip(STRREF_ACTIVATE_MINE, GUIEVENT_ACTIVATE_MINE);
        break;
    case 6:
        m_CSWGuiButton_1C4.SetToolTip(STRREF_ACTIVATE_LEFT_ACTION,
                                    GUIEVENT_ACTIVATE_LEFT_ACTION);
        break;
    case 7:
        m_CSWGuiButton_1C4.SetToolTip(STRREF_ACTIVATE_MIDDLE_ACTION,
                                    GUIEVENT_ACTIVATE_MIDDLE_ACTION);
        break;
    case 8:
        m_CSWGuiButton_1C4.SetToolTip(STRREF_ACTIVATE_RIGHT_ACTION,
                                    GUIEVENT_ACTIVATE_RIGHT_ACTION);
        break;
    }
}
#endif

#ifndef _XBOX
void CSWGuiMainInterface::DoTargetAction(int nAction, int bUnknown)  // PC 0x006E2670
{
    if (nAction >= 0 && nAction < 3)
    {
        if (g_bShiftActionState != 0)
        {
            ClearAllActions();
        }
        m_cTargetActionMenu.DoTargetAction(nAction, 1);
    }
}
#endif

#ifndef _XBOX
void CSWGuiMainInterface::DoPersonalAction(int nMenu, int bUnknown)  // PC 0x006E26B0
{
    if (nMenu >= 0 && nMenu < 7)
    {
        if (g_bShiftActionState != 0)
        {
            ClearAllActions();
        }
        ResetPartyMemberItemUse();
        m_nDefaultAction = nMenu;

        if (m_aPersonalActionLists[nMenu].num != 0)
        {
            CClientExoApp* pClientApp = g_pAppManager->m_pClientExoApp;
            CSWCCreature* pPlayer =
                pClientApp->GetSWParty()->GetPlayerCharacter();

            CSWGuiInterfaceAction* pAction = NULL;
            int nSelected = m_aSelectedPersonalActions[nMenu];
            if (nSelected != CSWGuiInterfaceAction::INVALID_ACTION)
            {
                for (int nAction = 0;
                     nAction < m_aPersonalActionLists[nMenu].num; nAction++)
                {
                    if (m_aPersonalActionLists[nMenu][nAction].m_nAction ==
                        nSelected)
                    {
                        pAction = &m_aPersonalActionLists[nMenu][nAction];
                        break;
                    }
                }
            }
            if (pAction == NULL)
            {
                pAction = &m_aPersonalActionLists[nMenu][0];
            }

            if (pPlayer != NULL && pAction != NULL)
            {
                CSWCObject* pGameObject =
                    g_pAppManager->GetClientExoApp()->GetGameObject(
                        pAction->m_oidTarget);
                if (pGameObject != NULL)
                {
                    CSWCObject* pObject = pGameObject->AsSWCObject();
                    if (pObject != NULL)
                    {
                        if (pAction->m_bEnabled && pAction->m_pfnAction != NULL)
                        {
                            if (pPlayer->m_bCombatMode == 0)
                            {
                                pPlayer->ClearAllQueuedCombatActions();
                                m_pManager->PlayGuiSound(
                                    CSWGuiManager::GUISOUND_WORLD_ACTION);
                            }
                            else if (pClientApp->GetAutoPaused())
                            {
                                pClientApp->GetInGameGui()->SetPaused(1, 10);
                            }
                            (pObject->*pAction->m_pfnAction)(pAction->m_nAction,
                                                             pPlayer);
                            m_aPersonalActions[nMenu].Pulse();
                            if (pPlayer->m_bCombatMode == 0 &&
                                pPlayer->GetServerCreature() != NULL)
                            {
                                pPlayer->GetServerCreature()->ClearAllActions(0);
                            }
                        }
                        else
                        {
                            if (pAction->m_nDisabledReason !=
                                CSWGuiInterfaceAction::DISABLED_REASON_NONE)
                            {
                                switch (pAction->m_nDisabledReason)
                                {
                                case CSWGuiInterfaceAction::DISABLED_REASON_FORCE_POINTS:
                                    m_nActionDescription = STRREF_FORCE_DEPLETED;
                                    break;
                                case CSWGuiInterfaceAction::DISABLED_REASON_FORBIDDEN_ITEM:
                                    m_nActionDescription = STRREF_RESTRICTED_BY_ARMOR;
                                    break;
                                case CSWGuiInterfaceAction::DISABLED_REASON_REQUIRED_ITEM:
                                    m_nActionDescription = STRREF_MISSING_ITEM;
                                    break;
                                case CSWGuiInterfaceAction::DISABLED_REASON_RANGE:
                                    m_nActionDescription = STRREF_TARGET_TOO_CLOSE;
                                    break;
                                case CSWGuiInterfaceAction::DISABLED_REASON_FULL_HEALTH:
                                    m_nActionDescription = STRREF_FULL_HEALTH;
                                    break;
                                case CSWGuiInterfaceAction::DISABLED_REASON_DEAD:
                                    m_nActionDescription = STRREF_PC_DEAD;
                                    break;
                                }
                                m_fActionDescriptionTimer = 5.0f;
                            }
                            m_pManager->PlayGuiSound(2);
                        }
                    }
                }
            }
        }
    }
}
#endif

#ifndef _XBOX
void CSWGuiMainInterface::OnActionUpArrowPressed(CSWGuiControl* pControl)  // PC 0x006E3A70
{
    if (pControl->m_bSelected != 0)
    {
        ResetPartyMemberItemUse();
        m_bPersonalActionScrolled = 1;
        int nIndex = pControl->m_nIndex;
        if (g_pAppManager->m_pClientExoApp->GetClientOptions()
                ->m_bAutopauseActionMenu != 0)
        {
            g_pAppManager->m_pClientExoApp->SetAutoPaused(1, 7);
        }
        g_pAppManager->m_pClientExoApp->ShowTutorialWindow(5, OBJECT_INVALID,
                                                           OBJECT_INVALID, 0);
        SelectPrevPersonalAction(nIndex);
    }
}
#endif

#ifndef _XBOX
void CSWGuiMainInterface::OnActionDownArrowPressed(CSWGuiControl* pControl)  // PC 0x006E3AF0
{
    if (pControl->m_bSelected != 0)
    {
        ResetPartyMemberItemUse();
        int nIndex = pControl->m_nIndex;
        m_bPersonalActionScrolled = 1;
        if (g_pAppManager->m_pClientExoApp->GetClientOptions()
                ->m_bAutopauseActionMenu != 0)
        {
            g_pAppManager->m_pClientExoApp->SetAutoPaused(1, 7);
        }
        g_pAppManager->m_pClientExoApp->ShowTutorialWindow(5, OBJECT_INVALID,
                                                           OBJECT_INVALID, 0);
        SelectNextPersonalAction(nIndex);
    }
}
#endif

#ifndef _XBOX
void CSWGuiMainInterface::OnClearOneButtonPressed(CSWGuiControl* pControl)  // PC 0x006E2B50
{
    if (pControl->m_bSelected != 0)
    {
        ClearOneAction();
    }
}
#endif

#ifndef _XBOX
void CSWGuiMainInterface::ClearOneAction()  // PC 0x006DEE60
{
    if (m_btnClearone.m_bEnabled != 0)
    {
        if (g_pAppManager->m_pClientExoApp->ShowTutorialWindow(
                32, OBJECT_INVALID, OBJECT_INVALID, 0) == 0)
        {
            OnCombatYButton();
        }
    }
}
#endif

#ifndef _XBOX
void CSWGuiMainInterface::OnClearAllButtonPressed(CSWGuiControl* pControl)  // PC 0x006E2BA0
{
    if (pControl->m_bSelected != 0)
    {
        ClearAllActions();
    }
}
// PC 0x006DE600
void CSWGuiMainInterface::UpdateGameMenu()
{
    if (g_pAppManager->GetServerExoApp()->GetPartyTable()->GetPartyItems()->m_nNewItems > 0)
    {
        if (!m_btnInv.m_cBorder.m_cParams.m_bPulsing)
        {
            m_btnInv.m_cBorder.m_cParams.SetPulsingAlpha(1, 0, 1);
        }
    }
    else
    {
        m_btnInv.m_cBorder.m_cParams.SetPulsingAlpha(0, 0, 0);
    }
    if (g_pAppManager->GetClientExoApp()->GetQuestJournal()->HasNewEntries())
    {
        if (!m_btnJou.m_cBorder.m_cParams.m_bPulsing)
        {
            m_btnJou.m_cBorder.m_cParams.SetPulsingAlpha(1, 0, 1);
        }
    }
    else
    {
        m_btnJou.m_cBorder.m_cParams.SetPulsingAlpha(0, 0, 0);
    }
}
// PC 0x006DEE20
void CSWGuiMainInterface::OnCharacterEntered(CSWGuiControl* pControl)
{
    if (m_bSelectingUseItemTarget && pControl != NULL)
    {
        m_nUseItemTargetMember = pControl->m_nIndex - 1;
    }
}
// PC 0x006DEE40
void CSWGuiMainInterface::OnCharacterExited(CSWGuiControl* pControl)
{
    if (m_bSelectingUseItemTarget)
    {
        m_nUseItemTargetMember = -1;
    }
}
// PC 0x006E2F70
void CSWGuiMainInterface::OnCancelPartyItemSelect(CSWGuiControl* pControl)
{
    if (pControl != NULL && pControl->m_bSelected != 0)
    {
        m_nUseItemTargetMember = -1;
        m_bSelectingUseItemTarget = 0;
    }
}
// PC 0x006E39E0
void CSWGuiMainInterface::OnPersonalActionExit(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0 && m_bPersonalActionScrolled)
    {
        int nMenu = pControl->m_nIndex;
        int nSelected = m_aSelectedPersonalActions[nMenu];
        if (nSelected != CSWGuiInterfaceAction::INVALID_ACTION)
        {
            CSWGuiInterfaceAction* pAction = NULL;
            for (int nAction = 0; nAction < m_aPersonalActionLists[nMenu].num; nAction++)
            {
                if (m_aPersonalActionLists[nMenu][nAction].m_nAction == nSelected)
                {
                    pAction = &m_aPersonalActionLists[nMenu][nAction];
                    break;
                }
            }
            if (pAction != NULL && pAction->m_n38)
            {
                RevertActionBarStates();
            }
        }
        m_bPersonalActionScrolled = 0;
    }
}
// PC 0x006E37E0
void CSWGuiMainInterface::RevertActionBarStates()
{
    int nImplantAction = -1;
    CSWCCreature* pPlayer = g_pAppManager->GetClientExoApp()->GetSWParty()->GetPlayerCharacter();
    if (pPlayer == NULL)
    {
        return;
    }
    CSWSCreature* pServerCreature = pPlayer->GetServerCreature();
    if (pServerCreature == NULL)
    {
        return;
    }
    char nImplant = 0;
    if (pServerCreature->m_pStats->HasFeat(CSWFeat::FEAT_IMPLANT_SWITCHING))
    {
        g_pAppManager->GetServerExoApp()->GetGlobalVariableTable()->GetValueNumber(
            "CANDEROUS_CUR_IMPLANT", nImplant);
        switch (nImplant)
        {
        case CSWCCreature::CANDEROUS_IMPLANT_REGEN:
            nImplantAction = CSWCCreature::ACTION_SWAP_IMPLANT_REGEN;
            break;
        case CSWCCreature::CANDEROUS_IMPLANT_STR:
            nImplantAction = CSWCCreature::ACTION_SWAP_IMPLANT_STR;
            break;
        case CSWCCreature::CANDEROUS_IMPLANT_END:
            nImplantAction = CSWCCreature::ACTION_SWAP_IMPLANT_END;
            break;
        case CSWCCreature::CANDEROUS_IMPLANT_AGI:
            nImplantAction = CSWCCreature::ACTION_SWAP_IMPLANT_AGI;
            break;
        }
    }
    int nBehaviorAction;
    switch (pServerCreature->m_pStats->m_nAIState)
    {
    case CSWCCreature::AI_STATE_AGGRESSIVE:
        nBehaviorAction = CSWCCreature::ACTION_BEHAVIOR_AGGRESSIVE;
        break;
    case CSWCCreature::AI_STATE_PASSIVE:
        nBehaviorAction = CSWCCreature::ACTION_BEHAVIOR_PASSIVE;
        break;
    case CSWCCreature::AI_STATE_RANGED:
        nBehaviorAction = CSWCCreature::ACTION_BEHAVIOR_RANGED;
        break;
    case CSWCCreature::AI_STATE_STATIONARY:
        nBehaviorAction = CSWCCreature::ACTION_BEHAVIOR_STATIONARY;
        break;
    case CSWCCreature::AI_STATE_SUPPORT:
        nBehaviorAction = CSWCCreature::ACTION_BEHAVIOR_SUPPORT;
        break;
    default:
        nBehaviorAction = -1;
        break;
    }
    int nFormAction = pServerCreature->m_n1110;
    if (nFormAction > 0)
    {
        nFormAction &= ~ACTION_FORM_FLAG;
    }
    if (nImplantAction > 0)
    {
        m_aSelectedPersonalActions[CSWCCreature::PERSONAL_ACTION_FORCE_POWERS] = nImplantAction;
    }
    if (nBehaviorAction > 0)
    {
        m_aSelectedPersonalActions[CSWCCreature::PERSONAL_ACTION_BEHAVIORS] = nBehaviorAction;
    }
    if (nFormAction > 0)
    {
        m_aSelectedPersonalActions[CSWCCreature::PERSONAL_ACTION_STANCES] = nFormAction;
    }
    PopulateMenus(0);
}
#endif
#ifndef _XBOX
void CSWGuiMainInterface::ClearAllActions()  // PC 0x006DEEA0
{
    CSWParty* pParty = g_pAppManager->m_pClientExoApp->GetSWParty();
    CSWCCreature* pPlayer = pParty->GetPlayerCharacter();

    if (pPlayer != NULL)
    {
        pPlayer->SetCombatMode(0);
        pPlayer->ClearAllActions();
        m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
        m_fTargetUpdateDelay = -1.0f;
        m_fActionDescriptionTimer = -1.0f;
    }
}
#endif
#ifndef _XBOX
void CSWGuiMainInterface::SelectNextPersonalAction(int nMenu)  // PC 0x006E2BC0
{
    if (nMenu >= 0 && nMenu < 6 &&
        m_aPersonalActionLists[nMenu].num != 0)
    {
        CSWParty* pParty = g_pAppManager->m_pClientExoApp->GetSWParty();
        CSWCCreature* pPlayer = pParty->GetPlayerCharacter();

        CSWGuiInterfaceAction* pNext = NULL;
        int nSelected = m_aSelectedPersonalActions[nMenu];
        if (nSelected != CSWGuiInterfaceAction::INVALID_ACTION)
        {
            for (int nAction = 0;
                 nAction < m_aPersonalActionLists[nMenu].num; nAction++)
            {
                if (m_aPersonalActionLists[nMenu].element[nAction].m_nAction ==
                    nSelected)
                {
                    if (nAction + 1 < m_aPersonalActionLists[nMenu].num)
                    {
                        pNext = &m_aPersonalActionLists[nMenu].element[nAction + 1];
                    }
                    else
                    {
                        pNext = &m_aPersonalActionLists[nMenu].element[0];
                    }
                    break;
                }
            }
        }

        if (m_aPersonalActionLists[nMenu].num > 1)
        {
            if (pNext == NULL)
            {
                pNext = &m_aPersonalActionLists[nMenu].element[1];
            }
            m_aSelectedPersonalActions[nMenu] = pNext->m_nAction;
            m_pManager->PlayGuiSound(CSWGuiManager::ACTION_CYCLE_SOUND);
        }
    }
}
#endif

#ifndef _XBOX
void CSWGuiMainInterface::SelectPrevPersonalAction(int nMenu)
{
    if (nMenu >= 0 && nMenu < 6 &&
        m_aPersonalActionLists[nMenu].num != 0)
    {
        CSWParty* pParty = g_pAppManager->m_pClientExoApp->GetSWParty();
        CSWCCreature* pPlayer = pParty->GetPlayerCharacter();

        CSWGuiInterfaceAction* pPrev = NULL;
        int nSelected = m_aSelectedPersonalActions[nMenu];
        if (nSelected != CSWGuiInterfaceAction::INVALID_ACTION)
        {
            for (int nAction = 0;
                 nAction < m_aPersonalActionLists[nMenu].num; nAction++)
            {
                if (m_aPersonalActionLists[nMenu].element[nAction].m_nAction ==
                    nSelected)
                {
                    if (nAction > 0)
                    {
                        pPrev = &m_aPersonalActionLists[nMenu].element[nAction - 1];
                    }
                    else
                    {
                        pPrev = &m_aPersonalActionLists[nMenu].element
                                     [m_aPersonalActionLists[nMenu].num - 1];
                    }
                    break;
                }
            }
        }

        if (m_aPersonalActionLists[nMenu].num > 1)
        {
            if (pPrev == NULL)
            {
                pPrev = &m_aPersonalActionLists[nMenu].element
                             [m_aPersonalActionLists[nMenu].num - 1];
            }
            m_aSelectedPersonalActions[nMenu] = pPrev->m_nAction;
            m_pManager->PlayGuiSound(CSWGuiManager::ACTION_CYCLE_SOUND);
        }
    }
}
#endif

#ifndef _XBOX
void CSWGuiMainInterface::UpdateStickyTarget()  // PC 0x006DEEF0
{
    int bRotate = g_bMouseCameraRotateToggle;
    CSWCObject* pObject = g_pAppManager->GetClientExoApp()->GetGameObject(m_oidStickyTarget);
    CSWCObject* pTarget = pObject != NULL ? pObject->AsSWCObject() : NULL;
    if (g_pAppManager->GetClientExoApp()->GetClientOptions()->m_bMouseLook)
        bRotate = !bRotate;
    if (pTarget != NULL && bRotate)
    {
        m_oidStickyTarget = 0;
        return;
    }

    CSWCCreature* pPlayer = g_pAppManager->GetClientExoApp()->GetSWParty()->GetPlayerCharacter();
    if (pTarget != NULL && pPlayer != NULL)
    {
        CSWGuiExtent cExtent = m_lblStickyReticle.m_cExtent;
        Vector vPoint;
        int nPoint = pTarget->GetReticlePoint(vPoint);
        int nSize = STICKY_RETICLE_MAX_SIZE;
        int bFriendly = 1;
        if (nPoint == CSWCObject::RETICLE_POINT_UNAVAILABLE)
            return;

        Vector vOffset = pPlayer->m_vPosition - pTarget->m_vPosition;
        float fDistanceSquared = vOffset * vOffset;
        if (pTarget->AsSWCCreature() != NULL)
        {
            if (fDistanceSquared >= STICKY_RETICLE_FAR_RANGE * STICKY_RETICLE_FAR_RANGE)
                nSize = STICKY_RETICLE_CREATURE_MIN_SIZE;
            else if (fDistanceSquared > STICKY_RETICLE_NEAR_RANGE * STICKY_RETICLE_NEAR_RANGE)
            {
                double fScale = (sqrt(fDistanceSquared) - STICKY_RETICLE_NEAR_RANGE) /
                                (STICKY_RETICLE_FAR_RANGE - STICKY_RETICLE_NEAR_RANGE);
                nSize = STICKY_RETICLE_MAX_SIZE - (int)(fScale * (STICKY_RETICLE_MAX_SIZE - STICKY_RETICLE_CREATURE_MIN_SIZE));
            }
            bFriendly = !pTarget->IsHostile();
        }
        else
        {
            if (fDistanceSquared >= STICKY_RETICLE_OBJECT_FAR_RANGE * STICKY_RETICLE_OBJECT_FAR_RANGE)
                nSize = STICKY_RETICLE_OBJECT_MIN_SIZE;
            else if (fDistanceSquared > STICKY_RETICLE_NEAR_RANGE * STICKY_RETICLE_NEAR_RANGE)
            {
                double fScale = (sqrt(fDistanceSquared) - STICKY_RETICLE_NEAR_RANGE) /
                                (STICKY_RETICLE_OBJECT_FAR_RANGE - STICKY_RETICLE_NEAR_RANGE);
                nSize = STICKY_RETICLE_MAX_SIZE - (int)(fScale * (STICKY_RETICLE_MAX_SIZE - STICKY_RETICLE_OBJECT_MIN_SIZE));
            }
            if (pTarget->AsSWCDoor() != NULL && pTarget->AsSWCDoor()->m_bTrapDetected && pTarget->AsSWCDoor()->m_bTrap)
                bFriendly = 0;
            else if (pTarget->AsSWCTrigger() != NULL)
                bFriendly = !pTarget->IsHostile();
            else if ((pTarget->AsSWCPlaceable() != NULL && pTarget->AsSWCPlaceable()->m_bTrap && pTarget->AsSWCPlaceable()->m_bTrapDetected) ||
                     (pTarget->AsSWCPlaceable() != NULL && pTarget->AsSWCPlaceable()->IsHostile() &&
                      pTarget->AsSWCPlaceable()->GetServerObject()->GetReputation(pPlayer->GetServerObject()->GetId(), 0) <= CSWSObject::MAX_HOSTILE_REPUTATION))
                bFriendly = 0;
        }

        CSWGuiExtent cMargin = m_lblArrowMargin.m_cExtent;
        int bShow = 1;
        if (nPoint == CSWCObject::RETICLE_POINT_BEHIND)
            bShow = 0;
        else if (cMargin.m_nLeft + STICKY_RETICLE_MARGIN > vPoint.m_fX ||
            cMargin.GetRight() - STICKY_RETICLE_MARGIN < vPoint.m_fX)
            bShow = 0;
        else if (cMargin.m_nTop + STICKY_RETICLE_MARGIN > vPoint.m_fY ||
            cMargin.GetBottom() - STICKY_RETICLE_MARGIN < vPoint.m_fY)
            bShow = 0;
        else
        {
            cExtent.m_nTop = (int)(vPoint.m_fY - nSize / 2);
            cExtent.m_nLeft = (int)(vPoint.m_fX - nSize / 2);
        }
        cExtent.m_nHeight = nSize;
        cExtent.m_nWidth = nSize;
        if (bShow)
        {
            m_lblStickyReticle.SetEnabledFlag(1);
            m_lblStickyReticle.SetExtent(cExtent);
            CResRef cReticle(bFriendly ? "friendlyreticle" : "hostilereticle");
            m_lblStickyReticle.m_cBorder.m_cParams.SetFillImage(cReticle, 0);
            m_lblStickyReticle.m_cBorder.m_cParams.m_fHilightAlpha = 0.5f;
        }
        else
            m_lblStickyReticle.SetEnabledFlag(0);
    }
    else
        m_lblStickyReticle.SetEnabledFlag(0);
}
#endif

#ifndef _XBOX
void CSWGuiMainInterface::SetupPauseGuiExtent(CSWGuiExtent* pExtent)  // PC 0x006DF370
{
    pExtent->m_nTop = 11;
    CClientOptions* pOptions =
        g_pAppManager->m_pClientExoApp->GetClientOptions();
    int bShowGUI;
    if (pOptions != NULL)
    {
        bShowGUI = !pOptions->m_bHideInGameGUI;
    }
    else
    {
        bShowGUI = TRUE;
    }
    if (bShowGUI)
    {
        int nOffset = m_btnEqu.m_cExtent.m_nTop +
                      m_btnEqu.m_cExtent.m_nHeight - 1;
        pExtent->m_nTop += nOffset;
    }
    pExtent->m_nLeft = m_pManager->m_nWidth - pExtent->m_nWidth - 6;
}
#endif
// PC 0x006DF410
void CSWGuiFloatyText::CreateEntry(unsigned long oidObject,
                                   const CExoString& sText,
                                   const Vector& vColor, float fDuration)
{
    m_fTimeLeft = fDuration;
    m_fDuration = fDuration;
    m_bActive = TRUE;
    m_oidObject = oidObject;
    m_cText.m_cParams.SetAlignment(CSWGuiTextParams::ALIGNMENT_CENTER);
    m_cText.m_cParams.SetText(sText);
    m_cText.m_cParams.SetColor(vColor);
    m_cText.m_cParams.m_fAlpha = 1.0f;
#ifdef _XBOX
    m_cText.m_cParams.SetFont("fnt_d16x16");
#else
    {
        CResRef cFont("fnt_d16x16");
        m_cText.m_cParams.SetBaseFont(cFont);
    }
#endif

    Vector vPosition;
    CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
    CSWCObject* pObject = pClientExoApp->GetGameObject(m_oidObject);
    if (pObject != NULL)
    {
        pObject = pObject->AsSWCObject();
        if (pObject != NULL)
        {
            pObject->GetReticlePoint(vPosition);
            m_cExtent.m_nWidth = 200;
            m_cExtent.m_nHeight = m_cText.GetFontHeight();
            m_cExtent.m_nTop =
                (int)(vPosition.m_fY - 32.0 - m_cExtent.m_nHeight);
            m_cExtent.m_nLeft = (int)(vPosition.m_fX - 100.0f);
            SetExtent(m_cExtent);
            if (pObject->AsSWCCreature() != NULL)
            {
                Gob* pModel = pObject->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1);
#ifdef _XBOX
                g_pAppManager->m_pClientExoApp->GetModuleCamera();
#endif
                Vector vScreen =
                    g_pAppManager->m_pClientExoApp->GetModuleCamera()
                        ->MapToScreen(pModel->GetPosition(), true);
                m_nScreenX =
                    (short)(g_pAppManager->m_pClientExoApp->GetSWGuiManager()
                                ->m_nWidth *
                            vScreen.m_fX);
                m_nScreenY =
                    (short)(g_pAppManager->m_pClientExoApp->GetSWGuiManager()
                                ->m_nHeight *
                            vScreen.m_fY);
            }
        }
    }
}
// PC 0x006E2D60
void CSWGuiFloatyText::Update(float fDeltaTime)
{
    m_fTimeLeft -= fDeltaTime;
    if (m_fTimeLeft <= 0.0f)
    {
        m_pMainInterface->RemoveFloatyText(this);
        return;
    }

    CSWCObject* pObject =
        g_pAppManager->GetClientExoApp()->GetGameObject(m_oidObject);
    if (pObject != NULL)
    {
        CSWCObject* pSWCObject = pObject->AsSWCObject();
        if (pSWCObject != NULL && pSWCObject->AsSWCCreature() != NULL)
        {
            Gob* pModel = pSWCObject->GetModel(CSWCObject::MODEL_SLOT_DEFAULT, 1);
            CAurCamera* pCamera =
                g_pAppManager->GetClientExoApp()->GetModuleCamera();

            Vector vView = -pCamera->GetOrientation().zdir();
            if (vView * pModel->GetPosition() > vView * pCamera->GetPosition())
            {
                Vector vScreen =
                    pCamera->MapToScreen(pModel->GetPosition(), true);
                short nScreenX =
                    (short)(g_pAppManager->GetClientExoApp()->GetSWGuiManager()
                                ->m_nWidth *
                            vScreen.m_fX);
                short nScreenY =
                    (short)(g_pAppManager->GetClientExoApp()->GetSWGuiManager()
                                ->m_nHeight *
                            vScreen.m_fY);
                m_cExtent.m_nTop += nScreenY - m_nScreenY;
                m_cExtent.m_nLeft += nScreenX - m_nScreenX;
                m_nScreenY = nScreenY;
                m_nScreenX = nScreenX;
                SetExtent(m_cExtent);
                m_bEnabled = TRUE;
            }
            else
            {
                m_bEnabled = FALSE;
            }
        }
    }

    m_cText.m_cParams.m_fAlpha = m_fTimeLeft / m_fDuration;
}

#ifdef _XBOX
CSWGuiMainInterfaceAction::CSWGuiMainInterfaceAction()
{
    m_nFadeSteps = 0;
    m_fSlideTimer = 0.0f;
}
#else
CSWGuiMainInterfaceAction::CSWGuiMainInterfaceAction()  // PC 0x006DF630
{
    m_nFadeSteps = 0;
    m_bTarget = 0;
}
#endif
// PC 0x006DF5F0
void CSWGuiMainInterface::OnSwapWeaponsPressed(CSWGuiControl* pControl)
{
    CSWCCreature* pCreature = g_pAppManager->m_pClientExoApp->GetPlayerCreature();
    if (pCreature != NULL)
    {
        pCreature->ActionMenuSwitchWeapons(0, NULL);
    }
}
// PC 0x006DF610
void CSWGuiMainInterface::ResetPartyMemberItemUse()
{
    m_nUseItemTargetMember = -1;
    m_bSelectingUseItemTarget = 0;
}

#ifndef _XBOX
CSWGuiTargetActionMenu::CSWGuiTargetActionMenu()  // PC 0x006E5060
{
    for (int nTargetType = 0; nTargetType < 4; nTargetType++)
    {
        for (int nMenu = 0; nMenu < 3; nMenu++)
        {
            m_aSelectedActions[nTargetType][nMenu] =
                CSWGuiInterfaceAction::INVALID_ACTION;
        }
    }
    m_nTargetType = CSWCCreature::TARGET_INTERFACE_INVALID;
    m_bVisible = 0;
    m_bMouseOver = 0;
    m_fFeedbackTimer = -1.0f;
    m_fMouseOverTimer = -1.0f;
    m_nSelectedAction = 0;
}
#endif

#ifndef _XBOX
void CSWGuiTargetActionMenu::Initialize(CSWGuiMainInterface* pMainInterface)  // PC 0x006E4010
{
    m_pMainInterface = pMainInterface;

    for (int nMenu = 0; nMenu < 3; nMenu++)
    {
        m_aActions[nMenu].Initialize(pMainInterface, nMenu, 1);
    }

    pMainInterface->InitControl(m_cNameLabel, "LBL_NAME", 0);
    pMainInterface->InitControl(m_cNameBackground, "LBL_NAMEBG", 0);
    pMainInterface->InitControl(m_cHealthBackground, "LBL_HEALTHBG", 0);
    pMainInterface->InitControl(m_cHealthBar, "PB_HEALTH", 0);

    CSWGuiExtent& cNameExtent = m_cNameLabel.m_cExtent;
    m_cExtent.m_nWidth = cNameExtent.m_nWidth;
    char nBottom = cNameExtent.m_nTop + cNameExtent.m_nHeight;
    m_nHealthBarSpacing = m_cHealthBar.m_cExtent.m_nTop - nBottom;
    m_nActionSpacing =
        m_aActions[0].m_CSWGuiButton_0.m_cExtent.m_nTop - nBottom;

    SetNameLabel("");
}
#endif

#ifndef _XBOX
void CSWGuiMainInterface::OnCharacterRightClicked(CSWGuiControl* pControl)  // PC 0x005EE1C0
{
}
#endif

static inline CClientExoApp* ClientExoAppLate3(CAppManager* p)
{
    return ClientExoAppLate2(p);
}

static inline CClientExoApp* ClientExoAppLate2(CAppManager* p)
{
    return ClientExoAppLate1(p);
}

static inline CClientExoApp* ClientExoAppLate1(CAppManager* p)
{
    return p->m_pClientExoApp;
}

static inline void Swguimaininterfacepc2Trace3()
{
    Swguimaininterfacepc2Trace2();
}

static inline void Swguimaininterfacepc2Trace2()
{
    Swguimaininterfacepc2Trace1();
}

static inline void Swguimaininterfacepc2Trace1()
{
}

#ifdef _XBOX

enum
{
    XBOX_RETICLE_OBJECT_FAR_RANGE = 10,
    XBOX_ITEM_ACTION_MASK = 0x40000000,
    XBOX_STRREF_ATTACK_QUEUED = 42475,
    XBOX_STRREF_ATTACK_ALREADY_QUEUED = 42478
};

void CSWGuiMainInterface::UpdateIndicator(float fElapsed)
{
    CSWCObject* pObject = g_pAppManager->GetClientExoApp()->GetGameObject(m_nTargetId);
    int bTarget = 0;
    int nPoint = 0;
    int nSize = STICKY_RETICLE_MAX_SIZE;
    Vector vPoint;
    CResRef cReticle;
    CResRef cArrow;
    CSWGuiExtent cExtent;
    int nWidth = m_pManager->m_nWidth;
    int nHeight = m_pManager->m_nHeight;

    if (pObject != NULL && pObject->AsSWCObject() != NULL)
    {
        CSWCObject* pTarget = pObject->AsSWCObject();
        int bFriendly = 1;
        int nPoisonState = 0;
        int nMarginX;
        int nMarginY;
        m_pManager->GetSafeMargin(nMarginX, nMarginY);
        CSWCCreature* pPlayer = g_pAppManager->GetClientExoApp()->GetSWParty()->GetPlayerCharacter();
        bTarget = 1;
        nPoint = pTarget->GetReticlePoint(vPoint);
        if (nPoint != CSWCObject::RETICLE_POINT_UNAVAILABLE)
        {
            switch (pObject->m_nObjectType)
            {
            case SW_OBJECT_TYPE_CREATURE:
                {
                    CSWSCreature* pCreature = pTarget->AsSWCCreature()->GetServerCreature();
                    bFriendly = !pTarget->IsHostile() && !g_pAppManager->GetClientExoApp()->GetTargetChanging();
                    int nCreaturePoison = pCreature != NULL ? pCreature->m_nPoisonState : 0;
                    nPoisonState = nCreaturePoison;
                    Vector vOffset = pPlayer->m_vPosition - pTarget->m_vPosition;
                    float fDistanceSquared = vOffset * vOffset;
                    if (fDistanceSquared >= STICKY_RETICLE_FAR_RANGE * STICKY_RETICLE_FAR_RANGE)
                        nSize = STICKY_RETICLE_CREATURE_MIN_SIZE;
                    else if (fDistanceSquared > STICKY_RETICLE_NEAR_RANGE * STICKY_RETICLE_NEAR_RANGE)
                        nSize = STICKY_RETICLE_MAX_SIZE - (int)(((sqrt(fDistanceSquared) - STICKY_RETICLE_NEAR_RANGE) / (STICKY_RETICLE_FAR_RANGE - STICKY_RETICLE_NEAR_RANGE)) * (STICKY_RETICLE_MAX_SIZE - STICKY_RETICLE_CREATURE_MIN_SIZE));

                    if (bFriendly)
                    {
                        cReticle = "friendlyreticle";
                        cArrow = "friendlyarrow";
                    }
                    else
                    {
                        if (g_pAppManager->GetClientExoApp()->GetCombatMode())
                        {
                            cReticle = "combatreticle";
                            if (m_fTargetUpdateDelay > 0.0f)
                            {
                                m_fTargetUpdateDelay -= fElapsed;
                                if (m_fTargetUpdateDelay <= 0.0f)
                                    m_fTargetUpdateDelay = 0.0f;
                                else
                                    nSize += (int)((1.0f - (0.5f - m_fTargetUpdateDelay) * 2.0f) * STICKY_RETICLE_MAX_SIZE);
                            }
                            else if (m_fTargetUpdateDelay == -1.0f)
                            {
                                m_fTargetUpdateDelay = 0.5f;
                                nSize += STICKY_RETICLE_MAX_SIZE;
                            }
                        }
                        else
                            cReticle = "hostilereticle";
                        cArrow = "hostilearrow";
                    }
                }
                break;
            case SW_OBJECT_TYPE_TRIGGER:
            case SW_OBJECT_TYPE_PLACEABLE:
            case SW_OBJECT_TYPE_DOOR:
                {
                    CSWCDoor* pDoor = pTarget->AsSWCDoor();
                    CSWCPlaceable* pPlaceable = pTarget->AsSWCPlaceable();
                    CSWCTrigger* pTrigger = pTarget->AsSWCTrigger();
                    if (pDoor != NULL && pDoor->m_bTrapDetected && pDoor->m_bTrap)
                        bFriendly = 0;
                    else if (pTrigger != NULL)
                        bFriendly = !pTrigger->IsHostile();
                    else if (pPlaceable != NULL)
                    {
                        if (pPlaceable->m_bTrap && pPlaceable->m_bTrapDetected)
                            bFriendly = 0;
                        else if (pPlaceable->IsHostile())
                        {
                            if (pPlaceable->GetServerObject()->GetReputation(pPlayer->GetServerObject()->GetId(), 0) <= CSWSObject::MAX_HOSTILE_REPUTATION)
                                bFriendly = 0;
                        }
                    }
                    Vector vOffset = pPlayer->m_vPosition - pTarget->m_vPosition;
                    float fDistanceSquared = vOffset * vOffset;
                    if (fDistanceSquared >= XBOX_RETICLE_OBJECT_FAR_RANGE * XBOX_RETICLE_OBJECT_FAR_RANGE)
                        nSize = STICKY_RETICLE_OBJECT_MIN_SIZE;
                    else if (fDistanceSquared > STICKY_RETICLE_NEAR_RANGE * STICKY_RETICLE_NEAR_RANGE)
                        nSize = STICKY_RETICLE_MAX_SIZE - (int)(((sqrt(fDistanceSquared) - STICKY_RETICLE_NEAR_RANGE) / (XBOX_RETICLE_OBJECT_FAR_RANGE - STICKY_RETICLE_NEAR_RANGE)) * (STICKY_RETICLE_MAX_SIZE - STICKY_RETICLE_OBJECT_MIN_SIZE));
                    cReticle = bFriendly ? "friendlyreticle" : "hostilereticle";
                    cArrow = bFriendly ? "friendlyarrow" : "hostilearrow";
                }
                break;
            default:
                return;
            }
        }

        CSWSObject* pServerObject = pTarget->GetServerObject();
        if (pServerObject != NULL)
        {
            float fAlpha = 1.0f;
            CExoString sName;
            unsigned long nFeedbackTime;
            pTarget->GetFeedbackInfo(sName, nFeedbackTime);
            if (nFeedbackTime != 0)
            {
                float fFraction = nFeedbackTime * 0.0002f;
                if (fFraction < 0.5)
                    fAlpha = fFraction * 2.0f;
            }
            else
                sName = pTarget->GetName();
            m_lblName.m_cText.m_cParams.SetText(sName);
            m_lblName.m_cText.m_cParams.m_fAlpha = fAlpha;
            m_pbHealth.SetMaxValue(pServerObject->GetMaxHitPoints(1));
            m_pbHealth.SetCurValue(pServerObject->GetCurrentHitPoints(0));
        }

        if (bFriendly)
        {
            if (nPoisonState)
                m_pbHealth.m_cFill.m_cParams.SetFillImage("POISON_BAR", 0);
            else
                m_pbHealth.m_cFill.m_cParams.SetFillImage("FRIEND_BAR", 0);
            m_lblIndicate.m_cBorder.m_cParams.SetFillImage("lbl_miindic01f", 0);
        }
        else
        {
            if (nPoisonState)
                m_pbHealth.m_cFill.m_cParams.SetFillImage("POISON_BAR", 0);
            else
                m_pbHealth.m_cFill.m_cParams.SetFillImage("ENEMY_BAR", 0);
            m_lblIndicate.m_cBorder.m_cParams.SetFillImage("lbl_miindic01e", 0);
        }

        if (nPoint != CSWCObject::RETICLE_POINT_UNAVAILABLE)
        {
            float fRotation = 0.0f;
            if (nPoint == CSWCObject::RETICLE_POINT_BEHIND)
            {
                cReticle = cArrow;
                nSize = STICKY_RETICLE_MARGIN;
                if (vPoint.m_fX < nWidth / 2)
                {
                    fRotation = 180.0f;
                    cExtent.m_nLeft = nWidth - nMarginX - nSize;
                }
                else
                    cExtent.m_nLeft = nMarginX;
                if (vPoint.m_fY < nMarginY)
                    cExtent.m_nTop = nHeight - nMarginY - nSize;
                else if (vPoint.m_fY > nHeight - nMarginY)
                    cExtent.m_nTop = nMarginY;
                else if (vPoint.m_fY < nHeight / 2)
                    cExtent.m_nTop = nHeight - (int)(vPoint.m_fY + 16.0f);
                else
                    cExtent.m_nTop = (int)(vPoint.m_fY - 16.0f);
            }
            else if (vPoint.m_fX < nMarginX)
            {
                cReticle = cArrow;
                nSize = STICKY_RETICLE_MARGIN;
                cExtent.m_nLeft = nMarginX;
                if (vPoint.m_fY < nMarginY)
                    cExtent.m_nTop = nMarginY;
                else if (vPoint.m_fY > nHeight - nMarginY)
                    cExtent.m_nTop = nHeight - nMarginY - nSize;
                else
                    cExtent.m_nTop = (int)(vPoint.m_fY - 16.0f);
            }
            else if (vPoint.m_fX > nWidth - nMarginX)
            {
                cReticle = cArrow;
                nSize = STICKY_RETICLE_MARGIN;
                cExtent.m_nLeft = nWidth - nMarginX - nSize;
                fRotation = 180.0f;
                if (vPoint.m_fY < nMarginY)
                    cExtent.m_nTop = nMarginY;
                else if (vPoint.m_fY > nHeight - nMarginY)
                    cExtent.m_nTop = nHeight - nMarginY - nSize;
                else
                    cExtent.m_nTop = (int)(vPoint.m_fY - 16.0f);
            }
            else if (vPoint.m_fY < nMarginY)
            {
                cReticle = cArrow;
                nSize = STICKY_RETICLE_MARGIN;
                cExtent.m_nTop = nMarginY;
                cExtent.m_nLeft = (int)(vPoint.m_fX - 16.0f);
                fRotation = 270.0f;
            }
            else if (vPoint.m_fY > nHeight - nMarginY)
            {
                cReticle = cArrow;
                nSize = STICKY_RETICLE_MARGIN;
                cExtent.m_nTop = nHeight - nMarginY - nSize;
                cExtent.m_nLeft = (int)(vPoint.m_fX - 16.0f);
                fRotation = 90.0f;
            }
            else
            {
                cExtent.m_nTop = (int)(vPoint.m_fY - nSize / 2);
                cExtent.m_nLeft = (int)(vPoint.m_fX - nSize / 2);
            }
            cExtent.m_nHeight = nSize;
            cExtent.m_nWidth = nSize;
            m_lblReticle.SetExtent(cExtent);
            m_lblReticle.m_cBorder.m_cParams.m_fRotation = fRotation;
            m_lblReticle.m_cBorder.m_cParams.SetFillImage(cReticle, 0);
        }
    }
    m_lblReticle.SetEnabledFlag(nPoint != CSWCObject::RETICLE_POINT_UNAVAILABLE);
    m_lblName.SetEnabledFlag(bTarget);
    m_lblIndicate.SetEnabledFlag(bTarget);
    m_pbHealth.SetEnabledFlag(bTarget);
}

void CSWGuiMainInterface::DrawActionDescription(float fElapsed)
{
    CSWGuiExtent cExtent = m_cActionText.m_cExtent;
    int nTextHeight = m_cActionText.m_cText.GetIdealHeight();
    cExtent.m_nHeight = nTextHeight;
    m_cActionText.SetExtent(cExtent);

    cExtent = m_cActionTextBackExtent;
    cExtent.m_nTop = cExtent.m_nHeight - m_nActionTextMargin + cExtent.m_nTop - nTextHeight;
    cExtent.m_nHeight = m_cActionTextBackExtent.GetBottom() - cExtent.m_nTop + 1;

    AurGUIStartLayer();
    if (AurGUISetupViewport(cExtent.m_nLeft, cExtent.m_nTop, cExtent.m_nWidth,
                            cExtent.m_nHeight, CSWGuiDrawable::NO_COLOURING,
                            false, m_fAlpha))
    {
        m_cActionTextBack.Draw(fElapsed);
        m_cActionText.Draw(fElapsed);
        AurGUICloseViewport();
    }
    AurGUIStopLayer();

    CSWGuiExtent cSoloExtent = m_lblSolomode.m_cExtent;
    cSoloExtent.m_nTop = cExtent.m_nTop - cSoloExtent.m_nHeight;
    m_lblSolomode.SetExtent(cSoloExtent);
}

void CSWGuiMainInterface::OnCombatXButton(int nEvent)
{
    if (m_aSelectedMenu[m_nTargetType] != 0xff)
    {
        CClientExoApp* pClientApp = g_pAppManager->m_pClientExoApp;
        CSWCCreature* pPlayer =
            pClientApp->GetSWParty()->GetPlayerCharacter();
        CSWGuiInterfaceAction* pAction =
            &m_aPersonalActionLists[m_aSelectedMenu[m_nTargetType]]
                 .element[m_nSelectedAction];

        if (pPlayer != NULL && pAction != NULL)
        {
            if (pAction->m_bEnabled && pAction->m_pfnAction != NULL)
            {
                int bEnterCombat = 0;
                int bDoAction = 1;
                if (nEvent == GUIEVENT_ACTIVATE)
                {
                    if (!pClientApp->GetCombatMode() && m_nTargetType == 4)
                    {
                        bEnterCombat = 1;
                        if (m_aSelectedMenu[4] == 5 || m_aSelectedMenu[4] == 4)
                        {
                            bEnterCombat = 0;
                        }
                        else if (m_aSelectedMenu[4] == 3)
                        {
                            if (pAction->m_nAction & XBOX_ITEM_ACTION_MASK)
                            {
                                CSWSItem* pItem =
                                    g_pAppManager->GetServerExoApp()
                                        ->GetItemByGameObjectID(
                                            pAction->m_nAction &
                                            ~XBOX_ITEM_ACTION_MASK);
                                if (pItem != NULL &&
                                    pItem->GetBaseItem()->m_nItemType ==
                                        CSWBaseItem::ITEM_TYPE_MINE)
                                {
                                    bEnterCombat = 0;
                                }
                            }
                        }
                    }
                    pPlayer->ClearAllQueuedCombatActions();
                    if (pClientApp->GetAutoPaused())
                    {
                        pClientApp->SetAutoPaused(0, 0);
                    }
                    if ((m_nTargetType == 1 &&
                         pAction->m_nAction ==
                             CSWGuiInterfaceAction::ACTION_BASH) ||
                        m_nTargetType == 4)
                    {
                        if (m_aSelectedMenu[m_nTargetType] == 0 &&
                            m_cActionDescriptionState.m_bTargetChanged &&
                            m_nXboxUnmapped5F18 == pAction->m_nAction)
                        {
                            m_pManager->PlayGuiSound(2);
                            SetCombatMessage(XBOX_STRREF_ATTACK_ALREADY_QUEUED);
                            bDoAction = 0;
                        }
                        else
                        {
                            m_cActionDescriptionState.m_bTargetChanged = 1;
                            m_nXboxUnmapped5F18 = pAction->m_nAction;
                            m_cActionDescriptionState.m_bUnknown = 1;
                            m_pManager->PlayGuiSound(6);
                            m_fCombatMessageTime = -1.0f;
                            SetCombatMessage(XBOX_STRREF_ATTACK_QUEUED);
                        }
                    }
                    else
                    {
                        m_cActionDescriptionState.m_bTargetChanged = 0;
                        m_pManager->PlayGuiSound(6);
                    }
                }
                else
                {
                    if (m_nTargetType == 1 &&
                        pAction->m_nAction == CSWGuiInterfaceAction::ACTION_BASH)
                    {
                        bDoAction = 0;
                    }
                    if (pClientApp->GetAutoPaused())
                    {
                        pClientApp->GetInGameGui()->SetPaused(
                            1, CGuiInGame::PAUSE_REASON_ACTION_SELECTED);
                    }
                }
                if (bDoAction)
                {
                    (((CSWCObject*)pAction->m_oidTarget)->*pAction->m_pfnAction)(
                        pAction->m_nAction, pPlayer);
                }
                m_aPersonalActions[m_aSelectedMenu[m_nTargetType]].Pulse();
                if (nEvent == GUIEVENT_ACTIVATE)
                {
                    if (bEnterCombat)
                    {
                        pPlayer->SetCombatMode(1);
                    }
                    if (bDoAction && pPlayer->GetServerCreature() != NULL)
                    {
                        pPlayer->GetServerCreature()->ClearAllActions(0);
                    }
                    m_aSelectedMenu[m_nTargetType] = 0xff;
                }
            }
            else
            {
                if (pAction->m_nDisabledReason !=
                    CSWGuiInterfaceAction::DISABLED_REASON_NONE)
                {
                    switch (pAction->m_nDisabledReason)
                    {
                    case CSWGuiInterfaceAction::DISABLED_REASON_FORCE_POINTS:
                        m_nActionDescription = STRREF_FORCE_DEPLETED;
                        break;
                    case CSWGuiInterfaceAction::DISABLED_REASON_FORBIDDEN_ITEM:
                        m_nActionDescription = STRREF_RESTRICTED_BY_ARMOR;
                        break;
                    case CSWGuiInterfaceAction::DISABLED_REASON_REQUIRED_ITEM:
                        m_nActionDescription = STRREF_MISSING_ITEM;
                        break;
                    case CSWGuiInterfaceAction::DISABLED_REASON_RANGE:
                        m_nActionDescription = STRREF_TARGET_TOO_CLOSE;
                        break;
                    case CSWGuiInterfaceAction::DISABLED_REASON_FULL_HEALTH:
                        m_nActionDescription = STRREF_FULL_HEALTH;
                        break;
                    case CSWGuiInterfaceAction::DISABLED_REASON_DEAD:
                        m_nActionDescription = STRREF_PC_DEAD;
                        break;
                    }
                    m_fActionDescriptionTimer = 5.0f;
                }
                m_pManager->PlayGuiSound(2);
            }
        }
    }
}

void CSWGuiMainInterface::OnNonCombatXButton()
{
    if (m_nTargetType == 4)
    {
        CSWCCreature* pPlayer =
            g_pAppManager->m_pClientExoApp->GetSWParty()->GetPlayerCharacter();
        pPlayer->SetCombatMode(1);
        m_pManager->PlayGuiSound(8);
        OnCombatXButton(41);
    }
}

void CSWGuiMainInterface::HandleInputEvent(int nEvent, int nState)
{
    if (nState != 0)
    {
        CClientExoApp* pClientApp = g_pAppManager->m_pClientExoApp;
        CSWCCreature* pPlayer =
            pClientApp->GetSWParty()->GetPlayerCharacter();
        CSWSCreature* pServerCreature =
            pPlayer != NULL ? pPlayer->GetServerCreature() : NULL;
        int bDead = pServerCreature == NULL || pServerCreature->GetDead() ||
                    pServerCreature->GetDeadTemp();

        int nOldMenu;
        int nCount;
        switch (nEvent)
        {
        case 41:
            if (!bDead)
            {
                if (!pClientApp->GetCombatMode())
                {
                    if (!pClientApp->ShowTutorialWindow(29, OBJECT_INVALID,
                                                        OBJECT_INVALID, 0))
                    {
                        OnNonCombatXButton();
                    }
                }
                else
                {
                    m_pManager->PlayGuiSound(8);
                    if (!pClientApp->ShowTutorialWindow(30, OBJECT_INVALID,
                                                        OBJECT_INVALID, 0))
                    {
                        OnCombatXButton(nEvent);
                    }
                }
            }
            break;
        case GUIEVENT_ACTIVATE:
            if (!bDead)
            {
                OnCombatXButton(nEvent);
            }
            break;
        case GUIEVENT_CANCEL:
            if (pClientApp->GetCombatMode())
            {
                CSWCCreature* pLeader =
                    pClientApp->GetSWParty()->GetPlayerCharacter();
                if (pLeader != NULL)
                {
                    pLeader->SetCombatMode(0);
                    pLeader->ClearAllActions();
                    m_pManager->PlayGuiSound(0);
                    m_fTargetUpdateDelay = -1.0f;
                    m_fActionDescriptionTimer = -1.0f;
                    m_cActionDescriptionState.m_bTargetChanged = 0;
                }
            }
            else
            {
                pClientApp->GetInGameGui()->StopBarkBubble(0);
            }
            break;
        case 42:
            if (!bDead)
            {
                m_pManager->PlayGuiSound(0);
                if (pClientApp->GetCombatMode())
                {
                    if (!pClientApp->ShowTutorialWindow(32, OBJECT_INVALID,
                                                        OBJECT_INVALID, 0))
                    {
                        OnCombatYButton();
                    }
                }
                else
                {
                    if (!pClientApp->ShowTutorialWindow(31, OBJECT_INVALID,
                                                        OBJECT_INVALID, 0))
                    {
                        pClientApp->PlayerFlourishWeapons();
                    }
                }
            }
            break;
        case GUIEVENT_LEFT:
            if (!bDead && m_aSelectedMenu[m_nTargetType] != 0xff)
            {
                nOldMenu = m_aSelectedMenu[m_nTargetType];
                do
                {
                    if (m_aSelectedMenu[m_nTargetType] == 0)
                    {
                        m_aSelectedMenu[m_nTargetType] = 5;
                    }
                    else
                    {
                        m_aSelectedMenu[m_nTargetType]--;
                    }
                } while (m_aPersonalActionLists[m_aSelectedMenu[m_nTargetType]]
                                 .num == 0 &&
                         m_aSelectedMenu[m_nTargetType] != nOldMenu);
                if (nOldMenu != m_aSelectedMenu[m_nTargetType])
                {
                    m_fActionDescriptionTimer = -1.0f;
                }
                m_pManager->PlayGuiSound(1);
                if (pClientApp->GetClientOptions()->m_bAutopauseActionMenu &&
                    m_nTargetType == 4)
                {
                    pClientApp->SetAutoPaused(1, 7);
                }
            }
            break;
        case GUIEVENT_RIGHT:
            if (!bDead && m_aSelectedMenu[m_nTargetType] != 0xff)
            {
                nOldMenu = m_aSelectedMenu[m_nTargetType];
                do
                {
                    m_aSelectedMenu[m_nTargetType] =
                        (m_aSelectedMenu[m_nTargetType] + 1) % 6;
                } while (m_aPersonalActionLists[m_aSelectedMenu[m_nTargetType]]
                                 .num == 0 &&
                         m_aSelectedMenu[m_nTargetType] != nOldMenu);
                if (nOldMenu != m_aSelectedMenu[m_nTargetType])
                {
                    m_fActionDescriptionTimer = -1.0f;
                }
                m_pManager->PlayGuiSound(1);
                if (pClientApp->GetClientOptions()->m_bAutopauseActionMenu &&
                    m_nTargetType == 4)
                {
                    pClientApp->SetAutoPaused(1, 7);
                }
            }
            break;
        case GUIEVENT_UP:
            if (!bDead && m_aSelectedMenu[m_nTargetType] != 0xff &&
                m_aPersonalActionLists[m_aSelectedMenu[m_nTargetType]].num > 1)
            {
                m_nSelectedAction++;
                if (m_nSelectedAction >=
                    m_aPersonalActionLists[m_aSelectedMenu[m_nTargetType]].num)
                {
                    m_nSelectedAction = 0;
                }
                m_aSelectedPersonalActions[m_nTargetType]
                                          [m_aSelectedMenu[m_nTargetType]] =
                    m_aPersonalActionLists[m_aSelectedMenu[m_nTargetType]]
                        .element[m_nSelectedAction].m_nAction;
                m_aPersonalActions[m_aSelectedMenu[m_nTargetType]].SetIcon(
                    m_aPersonalActionLists[m_aSelectedMenu[m_nTargetType]]
                        .element[m_nSelectedAction].m_resIcon,
                    m_aPersonalActionLists[m_aSelectedMenu[m_nTargetType]]
                        .element[m_nSelectedAction].m_bEnabled,
                    2);
                m_fActionDescriptionTimer = -1.0f;
                m_pManager->PlayGuiSound(7);
                if (pClientApp->GetClientOptions()->m_nXboxUnmappedBit32 &&
                    m_nTargetType == 4)
                {
                    pClientApp->SetAutoPaused(1, 7);
                }
                pClientApp->ShowTutorialWindow(5, OBJECT_INVALID,
                                               OBJECT_INVALID, 0);
            }
            break;
        case GUIEVENT_DOWN:
            if (!bDead && m_aSelectedMenu[m_nTargetType] != 0xff &&
                (nCount = m_aPersonalActionLists[m_aSelectedMenu[m_nTargetType]]
                              .num) > 1)
            {
                if (m_nSelectedAction == 0)
                {
                    m_nSelectedAction = nCount - 1;
                }
                else
                {
                    m_nSelectedAction--;
                }
                m_aSelectedPersonalActions[m_nTargetType]
                                          [m_aSelectedMenu[m_nTargetType]] =
                    m_aPersonalActionLists[m_aSelectedMenu[m_nTargetType]]
                        .element[m_nSelectedAction].m_nAction;
                m_aPersonalActions[m_aSelectedMenu[m_nTargetType]].SetIcon(
                    m_aPersonalActionLists[m_aSelectedMenu[m_nTargetType]]
                        .element[m_nSelectedAction].m_resIcon,
                    m_aPersonalActionLists[m_aSelectedMenu[m_nTargetType]]
                        .element[m_nSelectedAction].m_bEnabled,
                    1);
                m_fActionDescriptionTimer = -1.0f;
                m_pManager->PlayGuiSound(7);
                if (pClientApp->GetClientOptions()->m_nXboxUnmappedBit32 &&
                    m_nTargetType == 4)
                {
                    pClientApp->SetAutoPaused(1, 7);
                }
                pClientApp->ShowTutorialWindow(5, OBJECT_INVALID,
                                               OBJECT_INVALID, 0);
            }
            break;
        }
    }
    CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
#endif

#ifdef _XBOX

void CSWGuiMainInterfaceChar::SetDestination(int nPosition, int nCount, CSWGuiManager* pManager)
{
    switch (nPosition)
    {
    case 0:
        m_nEndX = m_nStartX;
        m_nEndY = pManager->m_nHeight;
        break;
    case 1:
        m_nEndX = pManager->m_nWidth - 128;
        m_nEndY = pManager->m_nHeight - 105;
        break;
    case 2:
        if (nCount == 2)
        {
            m_nEndX = pManager->m_nWidth - 128;
            m_nEndY = pManager->m_nHeight - 105;
        }
        else
        {
            m_nEndX = pManager->m_nWidth - 117;
            m_nEndY = pManager->m_nHeight - 141;
        }
        break;
    case 3:
        m_nEndX = pManager->m_nWidth - 106;
        m_nEndY = m_nStartY;
        break;
    }
}

void CSWGuiMainInterface::StartRotation()
{
    int nCount = g_pAppManager->m_pClientExoApp->GetSWParty()->GetNumCharacters();
    if (nCount > 1)
    {
        m_fPortraitTransitionTime = 0.25f;
        for (int nPortrait = 0; nPortrait < 3; nPortrait++)
            m_aCharacters[nPortrait].SetDestination(nPortrait, nCount, m_pManager);
        m_cTransitionCharacter.SetDestination(3, nCount, m_pManager);
    }
}

void CSWGuiCharBar::SetExtent(const CSWGuiExtent& cExtent)
{
    CSWGuiExtent cFill = cExtent;

    m_cExtent = cExtent;
    m_cStartExtent = cExtent;
    cFill.m_nLeft = cFill.m_nTop = 0;
    m_cBorder.SetExtent(cExtent);
    if (m_nMaxValue > 0)
    {
        if (m_cExtent.m_nHeight > m_cExtent.m_nWidth)
        {
            int nHeight = (int)((float)m_nCurValue / (float)m_nMaxValue * m_cStartExtent.m_nHeight);
            if (!m_bStartFromLeft)
            {
                m_cStartExtent.m_nTop += m_cStartExtent.m_nHeight - nHeight;
                cFill.m_nTop += nHeight - m_cStartExtent.m_nHeight;
            }
            m_cStartExtent.m_nHeight = nHeight;
        }
        else
        {
            int nWidth = (int)((float)m_nCurValue / (float)m_nMaxValue * m_cStartExtent.m_nWidth);
            if (!m_bStartFromLeft)
            {
                m_cStartExtent.m_nLeft += m_cStartExtent.m_nWidth - nWidth;
                cFill.m_nLeft += nWidth - m_cStartExtent.m_nWidth;
            }
            m_cStartExtent.m_nWidth = nWidth;
        }
    }
    m_cFill.SetExtent(cFill);
}

void CSWGuiCharBar::Draw(float fElapsed)
{
    m_cBorder.Draw(fElapsed);
    if (AurGUISetupViewport(m_cStartExtent.m_nLeft, m_cStartExtent.m_nTop, m_cStartExtent.m_nWidth, m_cStartExtent.m_nHeight,
                            CSWGuiDrawable::NO_COLOURING, false, 1.0f) != 0)
    {
        m_cFill.Draw(fElapsed);
        AurGUICloseViewport();
    }
}
#endif
