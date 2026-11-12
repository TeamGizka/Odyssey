#include "guiingame.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "clientexoappinternal.h"
#include "serverexoapp.h"
#include "nwcanimbase.h"
#include "exotlktable.h"
#include "nwtlktable.h"
#include "guiingame.h"
#include "swparty.h"
#include "nwsplayer.h"
#include "nwccreature.h"
#include "nwcobject.h"
#include "worldtimer.h"
#ifdef _XBOX
#include "exoinput.h"
#endif
#include "nwcmodule.h"
#include "nwcarea.h"
#include "nwsarea.h"

#define BARK_MIN_SOUND_HEIGHT 0.5f
#define BARK_MAX_TIME_STEP 36.0f

static inline void SwguibarkbubbleTrace1();
#define SWGUIBARKBUBBLE_TRACE() SwguibarkbubbleTrace1()
// PC 0x00712BA0
CSWGuiBarkBubble::CSWGuiBarkBubble(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
#ifdef _XBOX
    StartLoadFromLayout("barkbubble");
#else
    {
        CResRef cLayout("barkbubble_p");
        StartLoadFromLayout(cLayout);
    }
#endif
    InitControl(m_lblBarkText, "LBL_BARKTEXT", 1);
    StopLoadFromLayout();

    m_cInitialExtent = m_cExtent;
    m_fTimeLeft = 0.0f;
    m_pSoundSource = NULL;
    m_bSoundStarted = 0;
    m_bForceVisible = 0;
    m_nStartTime = 0;
    m_nBarkX = -1;
    m_nBarkY = -1;
}
// PC 0x00712CC0
CSWGuiBarkBubble::~CSWGuiBarkBubble()
{
    if (m_pSoundSource != NULL)
    {
        m_pSoundSource->Stop();
        delete m_pSoundSource;
        m_pSoundSource = NULL;
    }
}
// PC 0x00712D50
void CSWGuiBarkBubble::SetBark(unsigned long oidSpeaker,
                               const CExoString& sBarkText,
                               const CResRef& cSound, int bForceVisible)
{
    SWGUIBARKBUBBLE_TRACE();
    SetExtent(m_cInitialExtent);
    unsigned long oidPlayer = g_pAppManager->GetServerExoApp()->GetPlayerCreatureId();
    CSWSPlayer* pPlayer =
        g_pAppManager->GetServerExoApp()->GetClientObjectByObjectId(oidPlayer);
    unsigned long oidPC = pPlayer->m_oidPCObject;
    unsigned long oidClientPC =
        g_pAppManager->GetClientExoApp()->ServerToClientObjectId(oidPC);
    CSWCObject* pPlayerObject =
        g_pAppManager->GetClientExoApp()->GetGameObject(oidClientPC);
    CExoString sText(sBarkText);

    g_pSWTlkTable->ParseStr(sText, (CGameObject*)pPlayerObject, 0, 0);
    m_lblBarkText.m_cText.m_cParams.SetText(sText.CStr());

    int nBorderDim = m_lblBarkText.m_cBorder.m_cParams.GetBorderDim();
    int nHeight = m_lblBarkText.m_cText.GetIdealHeight() + nBorderDim * 2 + 10;
    CSWGuiExtent cExtent = m_cExtent;
    cExtent.m_nHeight = nHeight;
    if (m_nBarkX != -1)
        cExtent.m_nLeft = m_nBarkX;
    if (m_nBarkY != -1)
        cExtent.m_nTop = m_nBarkY;
    float fScaleX = 1.0f;
    float fScaleY = 1.0f;
    if (g_pAppManager->GetClientExoApp()->GetSWGuiManager() != NULL)
    {
        fScaleX = (float)g_pAppManager->GetClientExoApp()->GetSWGuiManager()->m_nWidth / (float)g_nGUIWidth;
        fScaleY = (float)g_pAppManager->GetClientExoApp()->GetSWGuiManager()->m_nHeight / (float)g_nGUIHeight;
    }
    cExtent.m_nLeft = (int)(cExtent.m_nLeft * fScaleX);
    cExtent.m_nTop = (int)(cExtent.m_nTop * fScaleY);
    SetExtent(cExtent);

    m_bDeferredRemoval = 0;
    m_nExitState = 0;
    m_oidSpeaker = oidSpeaker;
    m_bForceVisible = bForceVisible;
    m_fTimeLeft = sText.GetLength() * 0.11f + 1.0f;

    if (m_pSoundSource != NULL)
    {
        m_pSoundSource->Stop();
        delete m_pSoundSource;
        m_pSoundSource = NULL;
    }
    m_pSoundSource = new CExoStreamingSoundSource(1);
    if (oidSpeaker != OBJECT_INVALID)
    {
        Vector vPosition = g_pAppManager->m_pClientExoApp
                               ->GetGameObject(oidSpeaker)
                               ->AsSWCObject()
                               ->m_vPosition;
        CSWCModule* pModule = g_pAppManager->m_pClientExoApp->GetModule();
        if (pModule != NULL && pModule->GetArea() != NULL)
        {
            CSWSArea* pServerArea = pModule->GetArea()->GetServerArea();
            if (pServerArea != NULL)
            {
                float fHeight = g_pAppManager->GetServerExoApp()->ComputeHeight(
                    pServerArea->GetResRef(), vPosition, 0);
                if (vPosition.m_fZ - fHeight < BARK_MIN_SOUND_HEIGHT)
                    vPosition.m_fZ = fHeight + BARK_MIN_SOUND_HEIGHT;
            }
        }
        m_pSoundSource->Set3D(1);
        m_pSoundSource->SetReverbEnabled(0);
        m_pSoundSource->SetPosition(vPosition);
    }
    m_pSoundSource->SetPriorityGroup(26);
    m_pSoundSource->SetVolume(CExoSound::CMAX_VOLUME);
#ifdef _XBOX
    m_bSoundStarted = m_pSoundSource->Play(cSound, 11, 1, 0, 0);

    int nDebugOption = g_pExoInput->GetDebugOption();
    if (nDebugOption == 3)
    {
        m_pSoundSource->SetFixedVariance(1.5f);
    }
    else if (nDebugOption == 1)
    {
        m_pSoundSource->SetFixedVariance(0.75f);
    }
#else
    m_bSoundStarted = m_pSoundSource->Play(cSound, 8, 1, 0, 0);
    if (m_bSoundStarted == 0)
        m_bSoundStarted = m_pSoundSource->Play(cSound, 4, 1, 0, 0);
#endif
}
// PC 0x00712710
void CSWGuiBarkBubble::Pause()
{
    SWGUIBARKBUBBLE_TRACE();
    if (m_fTimeLeft > 0.0f || m_pSoundSource != NULL)
    {
        if (m_pSoundSource != NULL)
        {
            m_pSoundSource->Pause();
        }
        SetModal(0);
    }
}
// PC 0x00712770
void CSWGuiBarkBubble::Resume()
{
    SWGUIBARKBUBBLE_TRACE();
    if (m_fTimeLeft > 0.0f || m_pSoundSource != NULL)
    {
        if (m_pSoundSource != NULL)
        {
            m_pSoundSource->Resume();
        }
#ifdef _XBOX
        g_pAppManager->GetClientExoApp()->GetSWGuiManager()->AddPanel(this, 0, 1);
#else
        g_pAppManager->GetClientExoApp()->GetSWGuiManager()->AddPanel(this, SWGUI_PANEL_MODE_OVERLAY, 1);
#endif
    }
}
// PC 0x007127C0
void CSWGuiBarkBubble::StopBark(int bStopSound)
{
    SWGUIBARKBUBBLE_TRACE();
    if (bStopSound != 0)
    {
        if (m_pSoundSource != NULL)
        {
            m_pSoundSource->Stop();
            delete m_pSoundSource;
            m_pSoundSource = NULL;
        }
    }
    m_fTimeLeft = 0.0f;
    SetModal(0);
}
// PC 0x00712840
void CSWGuiBarkBubble::Draw(float fElapsed)
{
    SWGUIBARKBUBBLE_TRACE();
    CSWGuiExtent cExtent = m_cInitialExtent;
    cExtent.m_nHeight = m_cExtent.m_nHeight;
    if (!g_pAppManager->m_pClientExoApp->GetInGameGui()->GetMiniMapVisible())
    {
        int nMarginX, nMarginY;
        m_pManager->GetSafeMargin(nMarginX, nMarginY);
        cExtent.m_nWidth = m_pManager->m_nWidth - 2 * nMarginX;
    }
    if (m_nBarkX != -1)
        cExtent.m_nLeft = m_nBarkX;
    if (m_nBarkY != -1)
        cExtent.m_nTop = m_nBarkY;
    if (g_pAppManager->m_pClientExoApp->GetCombatMode())
    {
        cExtent.m_nTop +=
            g_pAppManager->m_pClientExoApp->GetInGameGui()->GetInterfaceOffset();
    }
    SetExtent(cExtent);

    int bShowBark = 1;
    if (m_oidSpeaker != OBJECT_INVALID)
    {
        CSWCCreature* pCharacter =
            g_pAppManager->m_pClientExoApp->GetSWParty()->GetCharacter(0);
        if (pCharacter != NULL)
        {
            Vector vPosition = pCharacter->m_vPosition;
            CClientExoApp* pClientExoApp = g_pAppManager->m_pClientExoApp;
            CSWCObject* pSpeaker = pClientExoApp->GetGameObject(m_oidSpeaker);
            if (pSpeaker == NULL || pSpeaker->AsSWCObject() == NULL)
            {
                bShowBark = 0;
            }
            else if (!m_bForceVisible)
            {
                Vector vDifference = vPosition - pSpeaker->AsSWCObject()->m_vPosition;
                bShowBark = vDifference * vDifference < BARK_DISTANCE_SQUARED;
            }
        }
    }

#ifdef _XBOX
    if (m_pSoundSource->IsPlaying())
#else
    if (m_pSoundSource != NULL && m_pSoundSource->IsPlaying())
#endif
    {
        if (bShowBark)
        {
            CSWGuiPanel::Draw(fElapsed);
            if (m_pManager->m_fGuiScale < BARK_MAX_TIME_STEP)
            {
                m_fTimeLeft -= m_pManager->m_fGuiScale;
            }
        }
        else
        {
            SetModal(0);
        }
    }
#ifdef _XBOX
    else if (m_pSoundSource->GetPlayFailed() || !m_bSoundStarted)
#else
    else if (m_pSoundSource != NULL &&
             (m_pSoundSource->GetPlayFailed() || !m_bSoundStarted))
#endif
    {
        if (m_fTimeLeft > 0.0f && bShowBark)
        {
            CSWGuiPanel::Draw(fElapsed);
            if (m_pManager->m_fGuiScale < BARK_MAX_TIME_STEP)
            {
                m_fTimeLeft -= m_pManager->m_fGuiScale;
            }
        }
        else
        {
            SetModal(0);
        }
    }
    else
    {
        if (m_pSoundSource != NULL)
        {
            m_pSoundSource->Stop();
            delete m_pSoundSource;
            m_pSoundSource = NULL;
        }
        SetModal(0);
    }

    if (g_pAppManager->m_pClientExoApp->GetCombatMode())
    {
        m_cExtent.m_nTop -=
            g_pAppManager->m_pClientExoApp->GetInGameGui()->GetInterfaceOffset();
    }
}
// PC 0x00712B10
void CSWGuiBarkBubble::OnPanelAdded()
{
    SWGUIBARKBUBBLE_TRACE();
    m_nStartTime = (__int64)g_pAppManager->m_pClientExoApp->m_pcExoAppInternal
                       ->m_pPauseTimer->GetSnapshotTime() /
                   1000;
#ifndef _XBOX
    CSWGuiPanel::OnPanelAdded();
#endif
}
// PC 0x00712B50
void CSWGuiBarkBubble::SetExtent(const CSWGuiExtent& cExtent)
{
    SWGUIBARKBUBBLE_TRACE();
    CSWGuiExtent cTextExtent;

    cTextExtent.m_nLeft = 5;
    cTextExtent.m_nTop = 5;
    cTextExtent.m_nWidth = cExtent.m_nWidth - 10;
    cTextExtent.m_nHeight = cExtent.m_nHeight - 10;
    m_lblBarkText.SetExtent(cTextExtent);
    CSWGuiPanel::SetExtent(cExtent);
}

static inline void SwguibarkbubbleTrace1()
{
}
