#include "swguiingame.h"

#include "appmanager.h"
#include "clientexoapp.h"
#include "clientexoappinternal.h"
#include "twodimarray.h"
#include "guiingame.h"
#include "serverexoapp.h"
#include "exobase.h"
#include "exotlktable.h"

static inline void SwguiingamecreditsTrace1();

static int g_nOldScreenHeight = 0;
static int g_nOldHeight = 0;
static int g_nOldWidth = 0;
static int g_nOldScreenWidth = 0;

static float creditsscrollduration = 305.0f;

void CSWGuiInGameCredits::Initialize()
{
    m_fScrollTime = 0.0f;
    m_fFadeTime = 0.0f;
    m_nPreviousInputClass = INPUT_CLASS_GUI;
    m_nCurrentText = 0;
    m_bFading = 1;
    m_bFadeIn = 1;
    m_fScrollInterval = 0.0f;
    SetNextText();
    m_lblCredit.m_cText.m_cParams.SetAlignment(CSWGuiTextParams::ALIGNMENT_CENTER);
    m_lblCredit.m_cText.m_cParams.m_fAlpha = 0.0f;
}

#ifdef _XBOX
CSWGuiInGameCredits::CSWGuiInGameCredits(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    int nName;
    StartLoadFromLayout("credits");
    InitControl(m_lbxCredits, "LB_CREDITS", 1);
    StopLoadFromLayout();

    CSWGuiLabel* pProto = (CSWGuiLabel*)m_lbxCredits.m_pProtoItem;
    m_lblCredit.Initialize(pProto->m_cExtent, pProto, 1.0f);

    m_nTextCount = 0;
    m_pText = NULL;
    C2DA* pCredits = new C2DA("credits", 0);
    if (pCredits->Load2DArray())
    {
        int nNumRows = pCredits->m_nNumRows;
        m_nTextCount = nNumRows;
        m_pText = new unsigned long[m_nTextCount];
        for (unsigned char nRow = 0; nRow < m_nTextCount; ++nRow)
        {
            if (pCredits->GetINTEntry(nRow, "Name", &nName))
            {
                m_pText[nRow] = nName;
            }
            else
            {
                m_pText[nRow] = 0;
            }
        }

        Initialize();
    }
}
#else
CSWGuiInGameCredits::CSWGuiInGameCredits(CSWGuiManager* pManager,  // PC 0x006E8DF0 GOG 0x006E8AB0
                                       const CExoString& sMusic)
    : CSWGuiPanel(pManager)
{
    int nName;
#ifndef _XBOX
    m_bPositioned = 1;
#endif
    {
        CResRef cLayout("credits_p");
        StartLoadFromLayout(cLayout);
    }
    InitControl(m_lbxCredits, "LB_CREDITS", 1);
    StopLoadFromLayout();

    CClientExoAppInternal* pClientExoAppInternal = g_pAppManager->GetClientExoApp()->m_pcExoAppInternal;
    if (pClientExoAppInternal->m_bEvilEndingHackInProgress)
    {
        g_nOldWidth = g_pGuiMan->m_nWidth;
        g_nOldHeight = g_pGuiMan->m_nHeight;
        g_nOldScreenWidth = g_nScreenWidth;
        g_nOldScreenHeight = g_nScreenHeight;
        g_nScreenWidth = 640;
        g_nScreenHeight = 480;
#ifndef _XBOX
        g_pGuiMan->SetSize(640, 480);
#endif
    }

    CSWGuiExtent cExtent;
    cExtent.m_nHeight = 0;
    cExtent.m_nWidth = 0;
    cExtent.m_nLeft = 0;
    cExtent.m_nTop = 0;
    cExtent.m_nWidth = g_pGuiMan->m_nWidth;
    cExtent.m_nHeight = g_pGuiMan->m_nHeight;
    CSWGuiPanel::SetExtent(cExtent);

    cExtent.m_nLeft = 40;
    cExtent.m_nTop = 40;
    cExtent.m_nWidth = m_cExtent.m_nWidth - 80;
    cExtent.m_nHeight = m_cExtent.m_nHeight - 80;
    m_lbxCredits.SetExtent(cExtent);

    cExtent.m_nLeft += 4;
    cExtent.m_nTop += 4;
    cExtent.m_nWidth -= 8;
    cExtent.m_nHeight -= 8;

    CSWGuiLabel* pProto = (CSWGuiLabel*)m_lbxCredits.m_pProtoItem;
    m_lblCredit.Initialize(cExtent, pProto, (float)m_cExtent.m_nHeight / 480.0f);

    cExtent.m_nLeft = 0;
    cExtent.m_nTop = 0;
    cExtent.m_nWidth = g_pGuiMan->m_nWidth;
    cExtent.m_nHeight = g_pGuiMan->m_nHeight;
    m_lblExit.Initialize(cExtent, pProto, 1.0f);
    AddControl(&m_lblExit);

    m_lblExit.AddEvent(GUIEVENT_ACTIVATE, this,
                      (CSWGuiHandler)&CSWGuiPanel::OnAButtonPressed);
    {
        CResRef cFont("fnt_credits");
        m_lblCredit.m_cText.m_cParams.SetBaseFont(cFont);
    }

    m_nTextCount = 0;
    m_pText = NULL;
    C2DA* pCredits = new C2DA("credits", 0);
    if (pCredits->Load2DArray())
    {
        int nNumRows = pCredits->m_nNumRows;
        m_nTextCount = nNumRows;
        m_pText = new unsigned long[m_nTextCount];
        for (unsigned char nRow = 0; nRow < m_nTextCount; ++nRow)
        {

            if (pCredits->GetINTEntry(nRow, "Name", &nName))
            {
                m_pText[nRow] = nName;
            }
            else
            {
                m_pText[nRow] = 0;
            }
        }

        m_fScrollInterval = 0.0f;
        m_sMusic = sMusic;
        m_pMusic = NULL;
        Initialize();
    }
}
#endif

#ifdef _XBOX
CSWGuiInGameCredits::~CSWGuiInGameCredits()
{
    if (m_pText != NULL) { delete[] m_pText; }
    g_pAppManager->GetClientExoApp()->GetInGameGui()->m_bCreditSequenceInProgress = 0;
    g_pAppManager->GetClientExoApp()->SetInputClass(m_nPreviousInputClass);
    g_pAppManager->GetClientExoApp()->GetInGameGui()->SetGlobalUnpauseableState(0);
    g_pAppManager->GetClientExoApp()->SetInputActive(0, 0);
}
#else
CSWGuiInGameCredits::~CSWGuiInGameCredits()  // PC 0x006E87E0 GOG 0x006E82C0
{
    delete m_pMusic;
    if (m_pText != NULL) { delete[] m_pText; }

    CGuiInGame* pInGameGui = g_pAppManager->GetClientExoApp()->GetInGameGui();
    if (pInGameGui != NULL)
    {
        pInGameGui->FadeAndStopDialogAmbientTrack();
    }
    g_pAppManager->GetClientExoApp()->StopCreditSequence();
    g_pAppManager->GetClientExoApp()->SetInputClass(m_nPreviousInputClass, 1);
    if (pInGameGui != NULL)
    {
        pInGameGui->SetGlobalUnpauseableState(0);
    }

    CClientExoAppInternal* pClientExoAppInternal = g_pAppManager->GetClientExoApp()->m_pcExoAppInternal;
    if (pClientExoAppInternal->m_bEvilEndingHackInProgress)
    {
        g_nScreenWidth = g_nOldScreenWidth;
        g_nScreenHeight = g_nOldScreenHeight;
#ifndef _XBOX
        g_pGuiMan->SetSize(g_nOldWidth, g_nOldHeight);
#endif
        pClientExoAppInternal = g_pAppManager->GetClientExoApp()->m_pcExoAppInternal;
        pClientExoAppInternal->SetEvilEndingHackInProgress(0);
#ifndef _XBOX
        RestoreResolutionFromMovie();
#endif
    }
    g_pAppManager->GetClientExoApp()->ShowMouse(128);
}
#endif
// PC 0x006E8600 GOG 0x006E8440
void CSWGuiInGameCredits::OnPanelAdded()
{
    SwguiingamecreditsTrace1();
#ifndef _XBOX
    g_pAppManager->m_pClientExoApp->HideMouse(CLIENT_MOUSE_HIDE_EVIL_ENDING);
#endif
    g_pAppManager->m_pClientExoApp->GetInGameGui()->SetGlobalUnpauseableState(1);
    m_nPreviousInputClass = g_pAppManager->m_pClientExoApp->GetInputClass();
#ifndef _XBOX
    CSWGuiPanel::OnPanelAdded();
#endif
#ifdef _XBOX
    g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GUI);
#else
    g_pAppManager->m_pClientExoApp->SetInputClass(INPUT_CLASS_GUI, 1);
#endif
#ifndef _XBOX
    m_pMusic = new CExoStreamingSoundSource;
    if (m_pMusic != NULL)
    {
        m_pMusic->SetPriorityGroup(4);
        CResRef cResRef((char*)m_sMusic.CStr());
        m_pMusic->Play(cResRef, 8, 0, 1, 0);
    }
#endif
}
// PC 0x006E86F0 GOG 0x006E8530
void CSWGuiInGameCredits::HandleInputEvent(int nEvent, int nState)
{
    SwguiingamecreditsTrace1();
    if (nState != 0)
    {
        switch (nEvent)
        {
        case GUIEVENT_ACTIVATE:
        case GUIEVENT_CANCEL:
        case GUIEVENT_ACCEPT:
        case GUIEVENT_CANCEL_ALTERNATE:
            m_bDeferredRemoval = 0;
            m_nExitState = EXITSTATE_DELETE;
            break;
        }
    }
}

#ifdef _XBOX
void CSWGuiInGameCredits::UpdateScrolling(float fElapsed)
{
    m_fScrollTime += fElapsed;
    while (m_fScrollTime >= m_fScrollInterval)
    {
        m_lbxCredits.HandleInputEvent(GUIEVENT_DOWN, 1);
        m_fScrollTime -= m_fScrollInterval;
        if (m_lbxCredits.m_cScrollBar.m_nCurValue ==
            m_lbxCredits.m_cScrollBar.m_nMaxValue)
        {
            m_bDeferredRemoval = 0;
            m_nExitState = EXITSTATE_DELETE;
            break;
        }
    }
}
#else
void CSWGuiInGameCredits::UpdateScrolling(float fElapsed)  // PC 0x006E8740 GOG 0x006E8810
{
    SwguiingamecreditsTrace1();
    if (m_lbxCredits.m_cScrollBar.m_nCurValue ==
        m_lbxCredits.m_cScrollBar.m_nMaxValue)
    {
        if (!m_pMusic->IsPlaying())
        {
            m_bDeferredRemoval = 0;
            m_nExitState = EXITSTATE_DELETE;
        }
        return;
    }

    m_fScrollTime += fElapsed;
    while (m_fScrollTime >= m_fScrollInterval)
    {
        m_lbxCredits.HandleInputEvent(GUIEVENT_DOWN, 1);
        m_fScrollTime -= m_fScrollInterval;
        if (m_lbxCredits.m_cScrollBar.m_nCurValue ==
            m_lbxCredits.m_cScrollBar.m_nMaxValue)
        {
            break;
        }
    }
}
#endif
// PC 0x006E8960 GOG 0x006E8580
void CSWGuiInGameCredits::SetText(unsigned long nStrRef, int bLastText)
{
    SwguiingamecreditsTrace1();
    CSWGuiExtent cExtent;
#ifndef _XBOX
    if (m_bFading)
#endif
    {
        m_lblCredit.m_cText.m_cParams.SetStrRef(nStrRef);
    }
#ifndef _XBOX
    else
    {
        STR_RES strRes; CExoString sText; CExoString sPadding;
        g_pTlkTable->Fetch(nStrRef, strRes, CTlkTable::GENDER_MALE);
        int nPaddingLines = (int)((float)m_lblCredit.m_cExtent.m_nHeight /
            ((float)m_lblCredit.m_cText.GetFontHeight() * m_cExtent.m_nHeight / 480.0f)) + 1;
        sPadding = "";
        for (int i = 0; i < nPaddingLines; ++i)
        {
            sPadding = sPadding + "\n";
        }
        sText = sPadding + strRes.m_sString + sPadding;
        m_lblCredit.m_cText.m_cParams.SetText(sText);
    }
#endif
    m_lbxCredits.ClearItems();
    if (bLastText)
    {
        cExtent = m_lblCredit.m_cExtent;
        cExtent.m_nHeight = m_lblCredit.m_cText.GetIdealHeight();
        m_lblCredit.SetExtent(cExtent);
    }
    CSWGuiControl* pControl = &m_lblCredit;
    m_lbxCredits.AddControls(&pControl, 1, 0, 0, 0);
    m_lbxCredits.SetSelectedControl(&m_lblCredit, 0);
    cExtent = m_lbxCredits.m_cExtent;
    m_lbxCredits.m_fScrollStep = (float)cExtent.m_nHeight;
}
// PC 0x006E8C10 GOG 0x006E88D0
void CSWGuiInGameCredits::SetNextText()
{
    int bLastText = 0;
    if (m_nCurrentText >= m_nTextCount - 1)
    {
        m_bFading = 0;

        m_lblCredit.m_cText.m_cParams.SetAlignment(10);
        m_lblCredit.m_cText.m_cParams.m_fAlpha = 1.0f;
        bLastText = 1;
    }

    SetText(m_pText[m_nCurrentText], bLastText);
    if (m_bFading == 0)
    {

        CSWGuiExtent cExtent = m_lblCredit.m_cExtent;
        m_fScrollInterval = creditsscrollduration / cExtent.m_nHeight;
    }
    ++m_nCurrentText;
}

#ifdef _XBOX
void CSWGuiInGameCredits::UpdateFading(float fElapsed)
{
    const float fFadeInDuration = 3.0f;
    const float fFadeOutDuration = 2.0f;
    m_fFadeTime += fElapsed;
    if (m_fFadeTime >= 1.0f)
    {
        if (m_bFadeIn != 0)
        {
            if (m_fFadeTime >= fFadeInDuration)
            {
                m_lblCredit.m_cText.m_cParams.m_fAlpha = 1.0f;
                m_fFadeTime -= (int)(m_fFadeTime / fFadeInDuration) *
                    fFadeInDuration;
                m_bFadeIn = 0;
            }
            else
            {
                m_lblCredit.m_cText.m_cParams.m_fAlpha = 1.0f;
            }
        }
        else if (m_fFadeTime >= fFadeOutDuration)
        {
            m_lblCredit.m_cText.m_cParams.m_fAlpha = 0.0f;
            m_fFadeTime -= (int)(m_fFadeTime / fFadeOutDuration) *
                fFadeOutDuration;
            m_bFadeIn = 1;
            SetNextText();
        }
        else
        {
            m_lblCredit.m_cText.m_cParams.m_fAlpha = 0.0f;
        }
    }
    else
    {
        if (m_bFadeIn != 0)
        {
            m_lblCredit.m_cText.m_cParams.m_fAlpha = m_fFadeTime;
        }
        else
        {
            m_lblCredit.m_cText.m_cParams.m_fAlpha = 1.0f - m_fFadeTime;
        }
    }
}
#else
void CSWGuiInGameCredits::UpdateFading(float fElapsed)  // PC 0x006E8CB0 GOG 0x006E8970
{

    const float fFadeInDuration = 3.0f;
    const float fFadeOutDuration = 2.0f;
    m_fFadeTime += fElapsed;
    if (m_fFadeTime >= 1.0f)
    {
        if (m_bFadeIn != 0)
        {
            m_lblCredit.m_cText.m_cParams.m_fAlpha = 1.0f;
            if (m_fFadeTime >= fFadeInDuration)
            {
                m_fFadeTime -= (int)(m_fFadeTime / fFadeInDuration) *
                    fFadeInDuration;
                m_bFadeIn = 0;
            }
        }
        else
        {
            m_lblCredit.m_cText.m_cParams.m_fAlpha = 0.0f;
            if (m_fFadeTime >= fFadeOutDuration)
            {
                m_fFadeTime -= (int)(m_fFadeTime / fFadeOutDuration) *
                    fFadeOutDuration;
                m_bFadeIn = 1;
                SetNextText();
            }
        }
    }
    else
    {
        if (m_bFadeIn != 0)
        {
            m_lblCredit.m_cText.m_cParams.m_fAlpha = m_fFadeTime;
        }
        else
        {
            m_lblCredit.m_cText.m_cParams.m_fAlpha = 1.0f - m_fFadeTime;
        }
    }
}
#endif

#ifdef _XBOX
void CSWGuiInGameCredits::Draw(float fElapsed)
{
    if (m_bFading != 0)
    {
        UpdateFading(fElapsed);
    }
    else
    {
        UpdateScrolling(fElapsed);
    }
    return CSWGuiPanel::Draw(fElapsed);
}
#else
void CSWGuiInGameCredits::Draw(float fElapsed)  // PC 0x006E8DA0 GOG 0x006E8A60
{
    CServerExoApp* pServerExoApp = g_pAppManager->m_pServerExoApp;

    if (pServerExoApp == NULL || pServerExoApp->GetPauseState(2) == 0)
    {
        if (m_bFading != 0)
        {
            UpdateFading(fElapsed);
        }
        else
        {
            UpdateScrolling(fElapsed);
        }
    }
    return CSWGuiPanel::Draw(fElapsed);
}
#endif

static inline void SwguiingamecreditsTrace1()
{
}
