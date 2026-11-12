#include "exobase.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "clientexoappinternal.h"
#include "worldtimer.h"
#include "exotlktable.h"
#include "exoresman.h"
#include "guiingame.h"
#include "swguifade.h"
// PC 0x00674F80
int CSWGuiFade::IsFadeDone()
{
    if (m_fFadeElapsed == 0.0f)
    {
        return 1;
    }
    return m_fFadeLength + m_fFadeStart < m_fFadeElapsed;
}
// PC 0x00674EF0
void CSWGuiFade::SetFade(int bFadeIn, float fFadeStart, float fFadeLength,
                         Vector const& vColor)
{
    m_vColor = vColor;
#ifndef _XBOX
    m_cExtent.m_nWidth = m_pManager->m_nWidth;
    m_cExtent.m_nHeight = m_pManager->m_nHeight;
#endif
    if (bFadeIn != 0)
    {
        SetAlpha(1.0f);
    }
    else
    {
        SetAlpha(0.0f);
    }
    m_fFadeStart = fFadeStart;
    m_fFadeLength = fFadeLength;
    m_bFadeIn = bFadeIn;
    m_fFadeElapsed = 0.1f;
    m_nFadeTime = (__int64)g_pAppManager->m_pClientExoApp->m_pcExoAppInternal
                      ->m_pPauseTimer->GetSnapshotTime() /
                  1000;
}
// PC 0x00674FB0
void CSWGuiFade::Draw(float fElapsed)
{
    unsigned long nTime = (unsigned long)((__int64)g_pAppManager->m_pClientExoApp
                                              ->m_pcExoAppInternal->m_pPauseTimer
                                              ->GetSnapshotTime() /
                                          1000);
    unsigned long nDelta = nTime - m_nFadeTime;
    m_nFadeTime = nTime;
    float fDelta = (float)nDelta * 0.001f;

    if (g_pAppManager->m_pClientExoApp->GetInGameGui() &&
        g_pAppManager->m_pClientExoApp->GetInGameGui()->GetFadeOverridePanelVisible())
    {
        return;
    }

    if (m_fFadeElapsed > 0.0f)
    {
        if (fDelta < 5.0f)
        {
            m_fFadeElapsed += fDelta;
        }
        if (m_fFadeElapsed > m_fFadeStart)
        {
            float fPercent = (m_fFadeElapsed - m_fFadeStart) / m_fFadeLength;
            float fAlpha;
            if (m_bFadeIn)
            {
                fPercent = 1.0f - fPercent;
                if (0.0f > fPercent)
                {
                    fAlpha = 0.0f;
                }
                else
                {
                    fAlpha = fPercent;
                }
            }
            else if (1.0f < fPercent)
            {
                fAlpha = 1.0f;
            }
            else
            {
                fAlpha = fPercent;
            }
            SetAlpha(fAlpha);
        }
    }
    CSWGuiPanel::Draw(fElapsed);
}
#ifndef _XBOX
CSWGuiControl* CSWGuiFade::HitCheckMouse(int nX, int nY)  // PC 0x006750E0
{
    if (m_fAlpha > 0.001f)
    {
        return CSWGuiPanel::HitCheckMouse(nX, nY);
    }
    return 0;
}
#endif
void CSWGuiFade::SetMsg(unsigned long nStrRef)  // PC 0x00675100
{
    CExoString sText;
    if (nStrRef != 0)
    {
        sText = g_pTlkTable->GetSimpleString(nStrRef);
        g_pTlkTable->ParseStr(sText);
        m_lblMessage.m_cText.m_cParams.SetText(sText);
    }
    else
    {
        CExoString sText("");
        m_lblMessage.m_cText.m_cParams.SetText(sText);
    }
}
// PC 0x006751E0
void CSWGuiFade::ResetFade()
{
    m_fAlpha = 0.0f;
    m_vColor = Vector();
    SetMsg(0);
    m_fFadeStart = 0.0f;
    m_fFadeLength = 0.0f;
    m_fFadeElapsed = 0.0f;
    m_bFadeIn = 0;
    m_nFadeTime = (__int64)g_pAppManager->m_pClientExoApp->m_pcExoAppInternal
                      ->m_pPauseTimer->GetSnapshotTime() /
                  1000;
}
// PC 0x00675250
CSWGuiFade::CSWGuiFade(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    if (m_pManager == 0)
    {
        m_pManager = g_pAppManager->m_pClientExoApp->GetSWGuiManager();
    }
#ifdef _XBOX
    StartLoadFromLayout("fade");
#else
    {
        CResRef cLayout("fade_p");
        StartLoadFromLayout(cLayout);
    }
#endif
    InitControl(m_lblMessage, "LBL_MSG", 1);
    StopLoadFromLayout();
    m_cExtent.m_nTop = 0;
    m_cExtent.m_nLeft = 0;
    m_cExtent.m_nWidth = m_pManager->m_nWidth;
    m_cExtent.m_nHeight = m_pManager->m_nHeight;
    ResetFade();
}

#ifdef _XBOX

int CSWGuiFade::IsObscuring()
{
    return m_fAlpha > 0.001f;
}
#endif
