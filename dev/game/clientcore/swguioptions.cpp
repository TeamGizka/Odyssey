#include <windows.h>
#include "exobase.h"
#include "aurora.h"
#include "aurinternal.h"
#include "twodimarray.h"
#include "exosoundinternal.h"
#include "nwsarea.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "clientexoappinternal.h"
#include "clientoptions.h"
#include "exobase.h"
#include "guiingame.h"
#include "swguioptions.h"
#include "swguiingame.h"
#define IS_VALID_RESOLUTION(nWidth, nHeight) \
    (((nWidth) == CClientExoAppInternal::VIDEO_FALLBACK_WIDTH && \
      (nHeight) == CClientExoAppInternal::VIDEO_FALLBACK_HEIGHT) || \
     ((nWidth) == CClientExoAppInternal::VIDEO_XGA_WIDTH && \
      (nHeight) == CClientExoAppInternal::VIDEO_XGA_HEIGHT) || \
     ((nWidth) == CClientExoAppInternal::VIDEO_SXGA_WIDTH && \
      ((nHeight) == CClientExoAppInternal::VIDEO_SXGA_4_3_HEIGHT || \
       (nHeight) == CClientExoAppInternal::VIDEO_SXGA_HEIGHT)) || \
     ((nWidth) == CClientExoAppInternal::VIDEO_UXGA_WIDTH && \
      (nHeight) == CClientExoAppInternal::VIDEO_UXGA_HEIGHT))

#include "swguiingamegameplay.h"
#include "swguiingameautopause.h"

#ifndef _XBOX
void CSWGuiOptionsGraphics::OnExitSlider(CSWGuiControl* pControl)
{
    CSWGuiImageParams* pParams = &pControl->AsSlider()->m_cImage.m_cParams;
    pParams->m_bPulsing = 0;
}
#endif

void CSWGuiOptionsMouse::OnExitSlider(CSWGuiControl* pControl)
{
    CSWGuiImageParams* pParams = &pControl->AsSlider()->m_cImage.m_cParams;
    pParams->m_bPulsing = 0;
}

#ifndef _XBOX
int CSWGuiOptionsGraphicsAdvanced::OnFrameBuffer(CSWGuiControl* pControl)  // PC 0x0074E750
{
    int bFrameBuffer = (m_bFrameBuffer == 0);
    m_bChanged = 1;
    m_bFrameBuffer = bFrameBuffer;
    return bFrameBuffer;
}
// PC 0x0074E840
void CSWGuiOptionsGraphicsAdvanced::OnSoftShadows(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        int bSoftShadows = m_bSoftShadows;
        m_bChanged = 1;
        m_bSoftShadows = (bSoftShadows == 0);
    }
}
// PC 0x0074E870
int CSWGuiOptionsGraphicsAdvanced::OnVSync(CSWGuiControl* pControl)
{
    int bVSync = (m_bVSync == 0);
    m_bVSync = bVSync;
    return bVSync;
}
// PC 0x0074EBA0
void CSWGuiOptionsGraphicsAdvanced::OnDiscardChanges(CSWGuiControl* pControl)
{
    m_bDiscard = 1;
    OnBButtonPressed(pControl);
}
#endif
// PC 0x0074EEF0
void CSWGuiOptionsSound::HandleInputEvent(int nEvent, int nState)
{
    if (nState != 0)
    {
        switch (nEvent)
        {
        case 45:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            break;
        case 40:
        case 46:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            m_pManager->PopModalPanel();
            m_bDeferredRemoval = 0;
            m_nExitState = EXITSTATE_DELETE;
            break;
#ifndef _XBOX
        case 57:
            m_lbDescription.HandleInputEvent(GUIEVENT_UP, 1);
            break;
        case 58:
            m_lbDescription.HandleInputEvent(GUIEVENT_DOWN, 1);
            break;
#endif
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
// PC 0x0074F0D0
void CSWGuiOptionsSound::SetDescription(unsigned long nStrRef)
{
#ifdef _XBOX
    CSWGuiTextParams* pParams = &m_lblDescription.m_cText.m_cParams;
    pParams->SetStrRef(nStrRef);
    CExoString sText(pParams->m_sText);
    CSWGuiListBox* pListBox = &m_lbDescription;
    pListBox->ClearItems();
    CSWGuiExtent cExtent = m_lblDescription.m_cExtent;
    if (sText.GetLength() > 0)
    {
        cExtent.m_nHeight = m_lblDescription.m_cText.GetIdealHeight();
    }
    else
    {
        cExtent.m_nHeight = m_lblDescription.m_cText.GetFontHeight();
    }
    m_lblDescription.SetExtent(cExtent);
    CSWGuiControl* pControl = &m_lblDescription;
    pListBox->AddControls(&pControl, 1, 0, 0, 0);
    pListBox->SetSelectedControl(&m_lblDescription, 0);
    int bShowScrollIcon = pListBox->m_nContentHeight + pListBox->m_nPadding >
                              pListBox->m_cItemExtent.m_nHeight &&
                          pListBox->m_lItems.num > 0 &&
                          pListBox->m_bHideScrollIcon == 0;
    m_lblRAnalog.m_bEnabled = bShowScrollIcon;
#else
    m_lblDescription.m_cText.m_cParams.SetStrRef(nStrRef);
    CExoString sText(m_lblDescription.m_cText.m_cParams.m_sText);
    m_lbDescription.ClearItems();
    CSWGuiExtent cExtent = m_lblDescription.m_cExtent;
    if (sText.GetLength() > 0)
    {
        cExtent.m_nHeight = m_lblDescription.m_cText.GetIdealHeight();
    }
    else
    {
        cExtent.m_nHeight = m_lblDescription.m_cText.GetFontHeight();
    }
    m_lblDescription.SetExtent(cExtent);
    CSWGuiControl* pControl = &m_lblDescription;
    m_lbDescription.AddControls(&pControl, 1, 0, 0, 0);
    m_lbDescription.SetSelectedControl(&m_lblDescription, 0);
#endif
}
// PC 0x0074F2C0
void CSWGuiOptionsSoundAdvanced::SetEAXText()
{
    CExoString sText;
    switch (m_nEAXMode)
    {
    case 0:
        m_btnEAX.m_cText.m_cParams.SetStrRef(STRREF_EAX_DISABLED);
        break;
    case 2:
        m_btnEAX.m_cText.m_cParams.SetStrRef(STRREF_EAX_2);
        break;
    case 3:
        m_btnEAX.m_cText.m_cParams.SetStrRef(STRREF_EAX_3);
        break;
    default:
        m_btnEAX.m_cText.m_cParams.SetStrRef(STRREF_EAX);
        break;
    }
    CSWGuiButton* pLeft = &m_btnEAXLeft;
    pLeft->m_bEnabled = 1;
    CSWGuiButton* pRight = &m_btnEAXRight;
    pRight->m_bEnabled = 1;
    int nBest;
#ifdef _XBOX
    nBest = 3;
#else
    if (g_pExoSound->GetBestEAXAvailable() > 3)
    {
        nBest = 3;
    }
    else
    {
        nBest = g_pExoSound->GetBestEAXAvailable();
    }
#endif
    if (m_nEAXMode == 0)
    {
        pLeft->m_bEnabled = 0;
    }
    if (m_nEAXMode == nBest)
    {
        pRight->m_bEnabled = 0;
    }
}
// PC 0x0074F3A0
void CSWGuiOptionsSoundAdvanced::SetDescription(unsigned long nStrRef)
{
    m_lblDescription.m_cText.m_cParams.SetStrRef(nStrRef);
    CExoString sText(m_lblDescription.m_cText.m_cParams.m_sText);
    m_lbDescription.ClearItems();
    CSWGuiExtent cExtent = m_lblDescription.m_cExtent;
    if (sText.GetLength() > 0)
    {
        cExtent.m_nHeight = m_lblDescription.m_cText.GetIdealHeight();
    }
    else
    {
        cExtent.m_nHeight = m_lblDescription.m_cText.GetFontHeight();
    }
    m_lblDescription.SetExtent(cExtent);
    CSWGuiControl* pControl = &m_lblDescription;
    m_lbDescription.AddControls(&pControl, 1, 0, 0, 0);
    m_lbDescription.SetSelectedControl(&m_lblDescription, 0);
}
// PC 0x0074F4B0
void CSWGuiOptionsSoundAdvanced::EnableEAXControls(int bEnable)
{
    m_btnEAX.SetEnabled(bEnable);
    Vector* pColor = &CGuiInGame::COLOR_NEW_TEXT;
    if (bEnable == 0)
    {
        pColor = &CGuiInGame::DISABLED_TEXT;
    }
    m_btnEAX.m_cText.m_cParams.SetColor(*pColor);
    m_btnEAXRight.SetEnabled(bEnable);
    m_btnEAXLeft.SetEnabled(bEnable);
}
// PC 0x0074F510
void CSWGuiOptionsSoundAdvanced::OnEnterOption(CSWGuiControl* pControl)
{
    if (pControl == &m_cbForceSoftware)
    {
        SetDescription(48718);
        return;
    }
    if (pControl == &m_btnEAX || pControl == &m_btnEAXRight ||
        pControl == &m_btnEAXLeft)
    {
        SetDescription(48719);
    }
}
// PC 0x0074F560
void CSWGuiOptionsSoundAdvanced::OnDiscardChanges(CSWGuiControl* pControl)
{
    m_bDiscard = 1;
    OnBButtonPressed(pControl);
}
// PC 0x0074F570
void CSWGuiOptionsCheckbox::SetExtent(const CSWGuiExtent& cExtent)
{
    CSWGuiExtent cBox;
    cBox.m_nLeft = cExtent.m_nLeft;
    cBox.m_nTop = cExtent.m_nTop;
    cBox.m_nWidth = 25;
    cBox.m_nHeight = 25;
    cBox.m_nTop += (cExtent.m_nHeight - cBox.m_nHeight) / 2 + 2;
    m_cBorder.SetExtent(cBox);
    m_cHilite.SetExtent(cBox);
    m_cToggleHilite.SetExtent(cBox);
    m_cToggleBorder.SetExtent(cBox);
    CSWGuiExtent cText = cExtent;
    cText.m_nLeft += 30;
    cText.m_nWidth -= 30;
    m_cText.SetExtent(cText);
    m_cExtent = cExtent;
}
// PC 0x0074F650
void CSWGuiOptionsCheckbox::Draw(float fElapsed)
{
    CSWGuiTextParams* pParams;
    if (m_bActive)
    {
        pParams = &m_cText.m_cParams;
        pParams->SetColor(CGuiInGame::COLOR_NEW_HIGHLIGHT);
        pParams->m_bHilightText = 1;
    }
#ifndef _XBOX
    else if (!m_bHilightOnOver)
    {
        m_cText.m_cParams.SetColor(CGuiInGame::DISABLED_TEXT);
    }
#endif
    else
    {
        pParams = &m_cText.m_cParams;
        pParams->SetColor(CGuiInGame::COLOR_NEW_TEXT);
        pParams->m_bHilightText = 0;
    }
    CSWGuiButtonToggle::Draw(fElapsed);
}
// PC 0x0074F6D0
void CSWGuiOptionsCheckbox::Load(CResGFF* pGFF, CResStruct& cStruct)
{
    CSWGuiButtonToggle::Load(pGFF, cStruct);
    m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_TEXT;
    m_cToggleBorder.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_TEXT;
    m_cHilite.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_HIGHLIGHT;
    m_cToggleHilite.m_cParams.m_vColor = CGuiInGame::COLOR_NEW_HIGHLIGHT;
}
// PC 0x0074E890
void CSWGuiOptionsMouse::SetDescription(unsigned long nStrRef)
{
    m_lblDescription.m_cText.m_cParams.SetStrRef(nStrRef);
    CExoString sText(m_lblDescription.m_cText.m_cParams.m_sText);
    m_lbDescription.ClearItems();
    CSWGuiExtent cExtent = m_lblDescription.m_cExtent;
    if (sText.GetLength() > 0)
    {
        cExtent.m_nHeight = m_lblDescription.m_cText.GetIdealHeight();
    }
    else
    {
        cExtent.m_nHeight = m_lblDescription.m_cText.GetFontHeight();
    }
    m_lblDescription.SetExtent(cExtent);
    CSWGuiControl* pControl = &m_lblDescription;
    m_lbDescription.AddControls(&pControl, 1, 0, 0, 0);
    m_lbDescription.SetSelectedControl(&m_lblDescription, 0);
}
// PC 0x0074F970
void CSWGuiOptionsMouse::OnEnterSlider(CSWGuiControl* pControl)
{
    if (pControl == &m_sldMouseSen)
    {
        SetDescription(48721);
    }
    CSWGuiImageParams* pParams = &pControl->AsSlider()->m_cImage.m_cParams;
    pParams->m_bPulsing = 1;
}
// PC 0x0074D6F0
void CSWGuiOptionsMain::OnGameplayOpt(CSWGuiControl* pControl)
{
    int bSelected = pControl->m_bSelected;
    if (bSelected != 0)
    {
        CSWGuiInGameGameplay* pPanel = new CSWGuiInGameGameplay(GetManager());
        m_pManager->AddPanel(pPanel, CSWGuiManager::PANEL_MODE_MODAL, 1);
    }
}
// PC 0x0074D770
void CSWGuiOptionsMain::OnAutopauseOpt(CSWGuiControl* pControl)
{
    int bSelected = pControl->m_bSelected;
    if (bSelected != 0)
    {
        CSWGuiInGameAutoPause* pPanel = new CSWGuiInGameAutoPause(GetManager());
        m_pManager->AddPanel(pPanel, CSWGuiManager::PANEL_MODE_MODAL, 1);
    }
}

void CSWGuiOptionsMain::ClosePanel()
{
    g_pAppManager->m_pClientExoApp->SaveOptions();
}
// PC 0x0074D7F0
void CSWGuiOptionsMain::SetDescription(unsigned long nStrRef)
{
#ifdef _XBOX
    CSWGuiTextParams* pParams = &m_lblDescription.m_cText.m_cParams;
    pParams->SetStrRef(nStrRef);
    CExoString sText(pParams->m_sText);
    CSWGuiListBox* pListBox = &m_lbDescription;
    pListBox->ClearItems();
    CSWGuiExtent cExtent = m_lblDescription.m_cExtent;
    if (sText.GetLength() > 0)
    {
        cExtent.m_nHeight = m_lblDescription.m_cText.GetIdealHeight();
    }
    else
    {
        cExtent.m_nHeight = m_lblDescription.m_cText.GetFontHeight();
    }
    m_lblDescription.SetExtent(cExtent);
    CSWGuiControl* pControl = &m_lblDescription;
    pListBox->AddControls(&pControl, 1, 0, 0, 0);
    pListBox->SetSelectedControl(&m_lblDescription, 0);
    int bShowScrollIcon = pListBox->m_nContentHeight + pListBox->m_nPadding >
                              pListBox->m_cItemExtent.m_nHeight &&
                          pListBox->m_lItems.num > 0 &&
                          pListBox->m_bHideScrollIcon == 0;
    m_lblRAnalog.m_bEnabled = bShowScrollIcon;
#else
    m_lblDescription.m_cText.m_cParams.SetStrRef(nStrRef);
    CExoString sText(m_lblDescription.m_cText.m_cParams.m_sText);
    m_lbDescription.ClearItems();
    CSWGuiExtent cExtent = m_lblDescription.m_cExtent;
    if (sText.GetLength() > 0)
    {
        cExtent.m_nHeight = m_lblDescription.m_cText.GetIdealHeight();
    }
    else
    {
        cExtent.m_nHeight = m_lblDescription.m_cText.GetFontHeight();
    }
    m_lblDescription.SetExtent(cExtent);
    CSWGuiControl* pControl = &m_lblDescription;
    m_lbDescription.AddControls(&pControl, 1, 0, 0, 0);
    m_lbDescription.SetSelectedControl(&m_lblDescription, 0);
#endif
}
// PC 0x0074D900
void CSWGuiOptionsFeedback::HandleInputEvent(int nEvent, int nState)
{
    if (nState != 0)
    {
        switch (nEvent)
        {
        case 40:
        case 46:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            m_pManager->PopModalPanel();
            m_bDeferredRemoval = 0;
            m_nExitState = EXITSTATE_DELETE;
            break;
        case 45:
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
#ifndef _XBOX
        case 57:
            m_lbDescription.HandleInputEvent(GUIEVENT_UP, 1);
            break;
        case 58:
            m_lbDescription.HandleInputEvent(GUIEVENT_DOWN, 1);
            break;
#endif
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
// PC 0x0074DA20
void CSWGuiOptionsFeedback::OnHide(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        int bHide = (g_pAppManager->m_pClientExoApp->GetClientOptions()
                         ->m_bHideUnequippable == 0);
        CClientOptions* pOptions =
            g_pAppManager->m_pClientExoApp->GetClientOptions();
        pOptions->m_bHideUnequippable = bHide;
    }
}
// PC 0x0074DA70
void CSWGuiOptionsFeedback::OnTutorial(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        int bTutorial = (g_pAppManager->m_pClientExoApp->GetClientOptions()
                             ->m_bTutorialPopups == 0);
        CClientOptions* pOptions =
            g_pAppManager->m_pClientExoApp->GetClientOptions();
        pOptions->m_bTutorialPopups = bTutorial;
    }
}
// PC 0x0074DAC0
void CSWGuiOptionsFeedback::OnSubtitles(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        int bSubtitles =
            (g_pAppManager->m_pClientExoApp->GetClientOptions()->m_bSubtitles ==
             0);
        CClientOptions* pOptions =
            g_pAppManager->m_pClientExoApp->GetClientOptions();
        pOptions->m_bSubtitles = bSubtitles;
    }
}
// PC 0x0074DB10
void CSWGuiOptionsFeedback::OnMiniMap(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        int bMiniMap =
            (g_pAppManager->m_pClientExoApp->GetClientOptions()->m_bMiniMap ==
             0);
        CClientOptions* pOptions =
            g_pAppManager->m_pClientExoApp->GetClientOptions();
        pOptions->m_bMiniMap = bMiniMap;
        g_pAppManager->m_pClientExoApp->GetInGameGui()->InitializeMap();
    }
}
// PC 0x0074DB70
void CSWGuiOptionsFeedback::OnFloaty(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
#ifdef _XBOX
        int bFloaty = (g_pAppManager->m_pClientExoApp->GetClientOptions()
                           ->m_nXboxUnmappedBit26 == 0);
        CClientOptions* pOptions =
            g_pAppManager->m_pClientExoApp->GetClientOptions();
        pOptions->m_nXboxUnmappedBit26 = bFloaty;
#else
        int bFloaty = (g_pAppManager->m_pClientExoApp->GetClientOptions()
                           ->m_bFloatingNumbers == 0);
        CClientOptions* pOptions =
            g_pAppManager->m_pClientExoApp->GetClientOptions();
        pOptions->m_bFloatingNumbers = bFloaty;
#endif
    }
}
// PC 0x0074DBC0
void CSWGuiOptionsFeedback::OnSummary(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        int bSummary = (g_pAppManager->m_pClientExoApp->GetClientOptions()
                            ->m_bStatusSummary == 0);
        CClientOptions* pOptions =
            g_pAppManager->m_pClientExoApp->GetClientOptions();
        pOptions->m_bStatusSummary = bSummary;
    }
}
// PC 0x0074DC10
void CSWGuiOptionsFeedback::OnHideInGameGUI(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        CClientOptions* pOptions =
            g_pAppManager->m_pClientExoApp->GetClientOptions();
        if (pOptions != 0)
        {
            pOptions->m_bHideInGameGUI = ~pOptions->m_bHideInGameGUI;
        }
    }
}
// PC 0x0074DC50
void CSWGuiOptionsFeedback::SetUpOptions()
{
    CClientOptions* pOptions =
        g_pAppManager->m_pClientExoApp->GetClientOptions();
    if (pOptions != 0)
    {
#ifdef _XBOX
        unsigned int bHide = pOptions->m_bHideUnequippable;
        CSWGuiButtonToggle* pToggle0 = m_aCheckboxes[0].AsButtonToggle();
        pToggle0->m_bToggled = bHide;
        unsigned int bTutorial = pOptions->m_bTutorialPopups;
        CSWGuiButtonToggle* pToggle1 = m_aCheckboxes[1].AsButtonToggle();
        pToggle1->m_bToggled = bTutorial;
        unsigned int bSubtitles = pOptions->m_bSubtitles;
        CSWGuiButtonToggle* pToggle2 = m_aCheckboxes[2].AsButtonToggle();
        pToggle2->m_bToggled = bSubtitles;
        unsigned int bMiniMap = pOptions->m_bMiniMap;
        CSWGuiButtonToggle* pToggle3 = m_aCheckboxes[3].AsButtonToggle();
        pToggle3->m_bToggled = bMiniMap;
        unsigned int bFloaty = pOptions->m_nXboxUnmappedBit26;
        CSWGuiButtonToggle* pToggle5 = m_aCheckboxes[5].AsButtonToggle();
        pToggle5->m_bToggled = bFloaty;
        unsigned int bRumble = pOptions->m_bFloatingNumbers;
        CSWGuiButtonToggle* pToggle4 = m_aCheckboxes[4].AsButtonToggle();
        pToggle4->m_bToggled = bRumble;
        unsigned int bSummary = pOptions->m_bStatusSummary;
        CSWGuiButtonToggle* pToggle6 = m_aCheckboxes[6].AsButtonToggle();
        pToggle6->m_bToggled = bSummary;
#else
        CSWGuiControl* pControl;
        CSWGuiButtonToggle* pToggle;
        int bValue;
        pControl = m_lbOptions.GetControl(0);
        bValue = pOptions->m_bHideUnequippable;
        pToggle = pControl->AsButtonToggle();
        pToggle->m_bToggled = bValue;
        pControl = m_lbOptions.GetControl(1);
        bValue = (pOptions->m_bTutorialPopups != 0);
        pToggle = pControl->AsButtonToggle();
        pToggle->m_bToggled = bValue;
        pControl = m_lbOptions.GetControl(2);
        bValue = (pOptions->m_bSubtitles != 0);
        pToggle = pControl->AsButtonToggle();
        pToggle->m_bToggled = bValue;
        pControl = m_lbOptions.GetControl(3);
        bValue = (pOptions->m_bMiniMap != 0);
        pToggle = pControl->AsButtonToggle();
        pToggle->m_bToggled = bValue;
        pControl = m_lbOptions.GetControl(4);
        bValue = (pOptions->m_bFloatingNumbers != 0);
        pToggle = pControl->AsButtonToggle();
        pToggle->m_bToggled = bValue;
        pControl = m_lbOptions.GetControl(5);
        bValue = (pOptions->m_bStatusSummary != 0);
        pToggle = pControl->AsButtonToggle();
        pToggle->m_bToggled = bValue;
        pControl = m_lbOptions.GetControl(6);
        bValue = (pOptions->m_bHideInGameGUI != 0);
        pToggle = pControl->AsButtonToggle();
        pToggle->m_bToggled = bValue;
        pControl = m_lbOptions.GetControl(7);
        bValue = (pOptions->m_bEnableTooltips != 0);
        pToggle = pControl->AsButtonToggle();
        pToggle->m_bToggled = bValue;
#endif
    }
}
// PC 0x0074DE00
void CSWGuiOptionsFeedback::SetDescription(unsigned long nStrRef)
{
#ifdef _XBOX
    CSWGuiTextParams* pParams = &m_lblDescription.m_cText.m_cParams;
    pParams->SetStrRef(nStrRef);
    CExoString sText(pParams->m_sText);
    CSWGuiListBox* pListBox = &m_lbDescription;
    pListBox->ClearItems();
    CSWGuiExtent cExtent = m_lblDescription.m_cExtent;
    if (sText.GetLength() > 0)
    {
        cExtent.m_nHeight = m_lblDescription.m_cText.GetIdealHeight();
    }
    else
    {
        cExtent.m_nHeight = m_lblDescription.m_cText.GetFontHeight();
    }
    m_lblDescription.SetExtent(cExtent);
    CSWGuiControl* pControl = &m_lblDescription;
    pListBox->AddControls(&pControl, 1, 0, 0, 0);
    pListBox->SetSelectedControl(&m_lblDescription, 0);
    int bShowScrollIcon = pListBox->m_nContentHeight + pListBox->m_nPadding >
                              pListBox->m_cItemExtent.m_nHeight &&
                          pListBox->m_lItems.num > 0 &&
                          pListBox->m_bHideScrollIcon == 0;
    m_lblRAnalog.m_bEnabled = bShowScrollIcon;
#else
    m_lblDescription.m_cText.m_cParams.SetStrRef(nStrRef);
    CExoString sText(m_lblDescription.m_cText.m_cParams.m_sText);
    m_lbDescription.ClearItems();
    CSWGuiExtent cExtent = m_lblDescription.m_cExtent;
    if (sText.GetLength() > 0)
    {
        cExtent.m_nHeight = m_lblDescription.m_cText.GetIdealHeight();
    }
    else
    {
        cExtent.m_nHeight = m_lblDescription.m_cText.GetFontHeight();
    }
    m_lblDescription.SetExtent(cExtent);
    CSWGuiControl* pControl = &m_lblDescription;
    m_lbDescription.AddControls(&pControl, 1, 0, 0, 0);
    m_lbDescription.SetSelectedControl(&m_lblDescription, 0);
#endif
}
// PC 0x0074DF10
void CSWGuiOptionsFeedback::OnEnableToolTips(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        CClientOptions* pOptions =
            g_pAppManager->m_pClientExoApp->GetClientOptions();
        if (pOptions != 0)
        {
            pOptions->m_bEnableTooltips = ~pOptions->m_bEnableTooltips;
        }
    }
}

#ifndef _XBOX
void CSWGuiOptionsGraphics::HandleInputEvent(int nEvent, int nState)  // PC 0x0074DF50
{
    if (nState != 0)
    {
        switch (nEvent)
        {
        case 40:
        case 46:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            m_pManager->PopModalPanel();
            m_bDeferredRemoval = 0;
            m_nExitState = EXITSTATE_DELETE;
            if (m_nUnknown18BC != 0)
            {
                CExoIni cIni;
                CExoString sValue;
                {
                    int bRead = cIni.ReadIniEntry(
                        sValue, ".\\swkotor2.ini", "Graphics Options",
                        "Disable Vertex Buffer Objects");
                    if (bRead != 0 && sValue.AsINT() == 1)
                    {
                        AurDisableVertexBufferObjects();
                    }
                }
                ReInitAurora(g_nScreenWidth, g_nScreenHeight, g_nBitsPerPixel,
                             g_pAurora->FullScreen(), 1);
                {
                    int bRead = cIni.ReadIniEntry(sValue, ".\\swkotor2.ini",
                                                  "Graphics Options",
                                                  "Disable Write-Only VBO");
                    if (bRead != 0)
                    {
                        if (sValue.AsINT() == 1)
                        {
                            AurDisableVertexBufferObjectsWriteOnly();
                        }
                        else
                        {
                            AurEnableVertexBufferObjectsWriteOnly();
                        }
                    }
                }
                m_nUnknown18BC = 0;
            }
            break;
        case 45:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            break;
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
// PC 0x0074E1B0
void CSWGuiOptionsGraphics::OnMoveGammaSlider(CSWGuiControl* pControl)
{
    int nMax = pControl->AsSlider()->m_nMaxValue;
    int nValue = pControl->AsSlider()->m_nCurValue;
    int nGamma = min(nValue + GAMMA_SLIDER_OFFSET, GAMMA_SLIDER_MAX);
    float fGamma = 2.0f - (float)nGamma / nMax * 1.77f;
    g_pAurora->ResetGammaRamp(fGamma, 0);
    g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nBrightness = nValue;
}
#endif
// PC 0x0074F770
void CSWGuiOptionsMouse::OnPanelAdded()
{
    CSWGuiPanel::OnPanelAdded();
}
// PC 0x0074F9B0
void CSWGuiOptionsSound::OnExitSlider(CSWGuiControl* pControl)
{
    CSWGuiImageParams* pParams = &pControl->AsSlider()->m_cImage.m_cParams;
    pParams->m_bPulsing = 0;
}

#ifndef _XBOX
void CSWGuiOptionsGraphics::OnShadows(CSWGuiControl* pControl)  // PC 0x0074E220
{
    if (pControl->m_bSelected != 0)
    {
        CClientOptions* pOptions =
            g_pAppManager->m_pClientExoApp->GetClientOptions();
        if (pOptions != 0)
        {
            pOptions->SetShadows(pOptions->m_nShadows == 0);
            if (pOptions->m_nShadows == 0)
            {
                pOptions->SetSoftShadows(0);
            }
        }
    }
}
// PC 0x0074E270
void CSWGuiOptionsGraphics::OnGrass(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        CClientOptions* pOptions =
            g_pAppManager->m_pClientExoApp->GetClientOptions();
        CClientExoApp* pClient = g_pAppManager->m_pClientExoApp;
        int bGrass = pOptions->m_nGrass;
        pClient->GetClientOptions()->SetGrass(bGrass == 0);
    }
}
// PC 0x0074E2B0
void CSWGuiOptionsGraphics::SetDescription(unsigned long nStrRef)
{
    m_lblDescription.m_cText.m_cParams.SetStrRef(nStrRef);
    CExoString sText(m_lblDescription.m_cText.m_cParams.m_sText);
    m_lbDescription.ClearItems();
    CSWGuiExtent cExtent = m_lblDescription.m_cExtent;
    if (sText.GetLength() > 0)
    {
        cExtent.m_nHeight = m_lblDescription.m_cText.GetIdealHeight();
    }
    else
    {
        cExtent.m_nHeight = m_lblDescription.m_cText.GetFontHeight();
    }
    m_lblDescription.SetExtent(cExtent);
    CSWGuiControl* pControl = &m_lblDescription;
    m_lbDescription.AddControls(&pControl, 1, 0, 0, 0);
    m_lbDescription.SetSelectedControl(&m_lblDescription, 0);
}
// PC 0x0074E3C0
void CSWGuiOptionsGraphics::SetUpOptions()
{
    CClientOptions* pOptions =
        g_pAppManager->m_pClientExoApp->GetClientOptions();
    if (pOptions != 0)
    {
        unsigned char nBrightness = pOptions->m_nBrightness;
        m_sldGamma.AsSlider()->SetCurValue(nBrightness);
        OnMoveGammaSlider(&m_sldGamma);
        if (pOptions->m_nSoftShadows != 0)
        {
            pOptions->SetShadows(1);
        }
        int nShadows = pOptions->m_nShadows;
        CSWGuiButtonToggle* pToggle = m_cbShadows.AsButtonToggle();
        pToggle->m_bToggled = nShadows;
        int nGrass = pOptions->m_nGrass;
        pToggle = m_cbGrass.AsButtonToggle();
        pToggle->m_bToggled = nGrass;
    }
}
// PC 0x0074E470
void CSWGuiOptionsGraphicsAdvanced::SetNewOptions()
{
    CClientOptions* pOptions =
        g_pAppManager->m_pClientExoApp->GetClientOptions();
    if (pOptions != 0)
    {
        pOptions->SetAntiAlias(m_nAntiAlias);
        pOptions->SetTexQual(m_nTexQual);
        pOptions->SetFrameBuffer(m_bFrameBuffer);
        pOptions->SetAnisotropy(m_nAnisotropy);
        pOptions->SetVSync(m_bVSync);
        pOptions->SetSoftShadows(m_bSoftShadows);
        if (m_bSoftShadows != 0)
        {
            pOptions->SetShadows(1);
        }
    }
}
// PC 0x0074E500
void CSWGuiOptionsGraphicsAdvanced::SetTexQualText()
{
    int nStrRef;
    C2DA* p2DA = new C2DA("texpacks", 0);
    if (p2DA != 0)
    {
        if (p2DA->Load2DArray() == 0)
        {
            delete p2DA;
            return;
        }
        if (p2DA->m_nNumRows > 0)
        {
            int bSuccess =
                p2DA->GetINTEntry(m_nTexQual, "StrrefName", &nStrRef);
            if (bSuccess != 0)
            {
                m_btnTexQual.m_cText.m_cParams.SetStrRef(nStrRef);
            }
        }
        CSWGuiButton* pLeft = &m_btnTexQualLeft;
        pLeft->SetEnabledFlag(1);
        m_btnTexQualRight.SetEnabledFlag(1);
        if (m_nTexQual == 0)
        {
            pLeft->SetEnabledFlag(0);
        }
        else if (m_nTexQual == 2)
        {
            m_btnTexQualRight.SetEnabledFlag(0);
        }
        m_bChanged = 1;
        p2DA->Unload2DArray();
        delete p2DA;
    }
}
// PC 0x0074E650
void CSWGuiOptionsGraphicsAdvanced::SetAntiAliasText()
{
    switch (m_nAntiAlias)
    {
    case 0:
        m_btnAntiAlias.m_cText.m_cParams.SetStrRef(STRREF_ANTIALIAS_OFF);
        break;
    case 2:
        m_btnAntiAlias.m_cText.m_cParams.SetStrRef(STRREF_ANTIALIAS_2X);
        break;
    case 4:
        m_btnAntiAlias.m_cText.m_cParams.SetStrRef(STRREF_ANTIALIAS_4X);
        break;
    case 6:
        m_btnAntiAlias.m_cText.m_cParams.SetStrRef(STRREF_ANTIALIAS_6X);
        break;
    case 8:
        m_btnAntiAlias.m_cText.m_cParams.SetStrRef(STRREF_ANTIALIAS_8X);
        break;
    }
    CSWGuiButton* pLeft = &m_btnAntiAliasLeft;
    pLeft->m_bEnabled = 1;
    CSWGuiButton* pRight = &m_btnAntiAliasRight;
    pRight->m_bEnabled = 1;
    if (m_nAntiAlias == 0)
    {
        pLeft->m_bEnabled = 0;
    }
    int nMax = 0;
    if (AurQueryAntiAliasingMode(2) != 0)
    {
        nMax = 2;
    }
    if (AurQueryAntiAliasingMode(4) != 0)
    {
        nMax = 4;
    }
    if (AurQueryAntiAliasingMode(6) != 0)
    {
        nMax = 6;
    }
    if (AurQueryAntiAliasingMode(8) != 0)
    {
        nMax = 8;
    }
    if (m_nAntiAlias == 8 || m_nAntiAlias >= nMax)
    {
        pRight->m_bEnabled = 0;
    }
    m_bChanged = 1;
}

#ifndef _XBOX
CSWGuiOptionsResolution::CSWGuiOptionsResolution(CSWGuiManager* pManager)  // PC 0x007517D0
    : CSWGuiPanel(pManager)
{
    StartLoadFromLayout("optresolution_p");
    InitControl(m_lblResolution, "LBL_RESOLUTION", 1);
    InitControl(m_lbResolutions, "LB_RESOLUTIONS", 1);
    InitControl(m_btnAccept, "BTN_OK", 1);
    InitControl(m_btnBack, "BTN_CANCEL", 1);
    StopLoadFromLayout();
    CSWGuiButton* pOk = &m_btnAccept;
    pOk->AddEvent(
        GUIEVENT_ACTIVATE, this, (CSWGuiHandler)&CSWGuiOptionsResolution::OnResolutionChosen);
    pOk->m_bFocusable = 0;
    CSWGuiButton* pCancel = &m_btnBack;
    pCancel->AddEvent(GUIEVENT_ACTIVATE, this, (CSWGuiHandler)&CSWGuiPanel::OnBButtonPressed);
    pCancel->m_bFocusable = 0;
    CSWGuiListBox* pList = &m_lbResolutions;
    pList->SetSelectOnLeftClick(1);
    SetActiveControl(pList, 0);
    CSWGuiButton* pSelected = 0;
    unsigned long nMode = 0;
    CExoArrayList<CSWGuiControl*> lstControls;
    CExoString sText;
    DEVMODE cDevMode;
    memset(&cDevMode, 0, sizeof(cDevMode));
    cDevMode.dmSize = 156;
    CSWGuiControl* pProtoItem = m_lbResolutions.m_pProtoItem;
    if (pProtoItem != 0)
    {
        CSWGuiButton* pProto = pProtoItem->AsButton();
        if (EnumDisplaySettings(0, 0, &cDevMode))
        {
            do
            {
                if (cDevMode.dmDisplayFrequency ==
                        CAuroraInterface::VIDEO_REFRESH_DEFAULT ||
                    cDevMode.dmDisplayFrequency ==
                        CAuroraInterface::VIDEO_REFRESH_DEFAULT_ALTERNATE)
                {
                    if (cDevMode.dmBitsPerPel ==
                            CClientExoAppInternal::VIDEO_BITS_PER_PIXEL &&
                        IS_VALID_RESOLUTION(cDevMode.dmPelsWidth,
                                            cDevMode.dmPelsHeight))
                    {
                        CSWGuiButton* pButton = new CSWGuiButton;
                        pButton->Initialize(pProto->m_cExtent, pProto);
                        sText.Format("%d x %d", cDevMode.dmPelsWidth,
                                     cDevMode.dmPelsHeight);
                        pButton->m_cText.m_cParams.SetText(sText);
                        pButton->m_nIndex = nMode;
                        pButton->AddEvent(
                            GUIEVENT_ACTIVATE, this,
                            (CSWGuiHandler)&CSWGuiOptionsResolution::
                                OnResolutionChosen);
                        lstControls.Add(pButton);
                        if (cDevMode.dmPelsWidth == g_nScreenWidth &&
                            cDevMode.dmPelsHeight == g_nScreenHeight)
                        {
                            pSelected = pButton;
                        }
                    }
                }
                else
                {
                    if (cDevMode.dmBitsPerPel ==
                            CClientExoAppInternal::VIDEO_BITS_PER_PIXEL &&
                        cDevMode.dmDisplayFrequency >=
                            CClientExoAppInternal::VIDEO_REFRESH_RATE &&
                        IS_VALID_RESOLUTION(cDevMode.dmPelsWidth,
                                            cDevMode.dmPelsHeight) &&
                        (float)cDevMode.dmDisplayFrequency <=
                            (float)CClientExoAppInternal::VIDEO_MAX_STANDARD_REFRESH_RATE)
                    {
                        CSWGuiButton* pButton = new CSWGuiButton;
                        pButton->Initialize(pProto->m_cExtent, pProto);
                        sText.Format("%d x %d @ %d Hz", cDevMode.dmPelsWidth,
                                     cDevMode.dmPelsHeight,
                                     cDevMode.dmDisplayFrequency);
                        pButton->m_cText.m_cParams.SetText(sText);
                        pButton->m_nIndex = nMode;
                        pButton->AddEvent(
                            GUIEVENT_ACTIVATE, this,
                            (CSWGuiHandler)&CSWGuiOptionsResolution::
                                OnResolutionChosen);
                        lstControls.Add(pButton);
                        if (cDevMode.dmDisplayFrequency ==
                            g_nDesiredFullScreenRefreshRate)
                        {
                            if (cDevMode.dmPelsWidth == g_nScreenWidth &&
                                cDevMode.dmPelsHeight == g_nScreenHeight)
                            {
                                pSelected = pButton;
                            }
                        }
                    }
                }
                nMode++;
            } while (EnumDisplaySettings(0, nMode, &cDevMode));
        }
        m_lbResolutions.AddControls(lstControls, 0, 1, 0);
        if (pSelected != 0)
        {
            m_lbResolutions.SetSelectedControl(pSelected, 0);
        }
    }
    m_nUnknown80C = 0;
}
#endif
// PC 0x0074E770
void CSWGuiOptionsGraphicsAdvanced::SetAnisotropyText()
{
    switch (m_nAnisotropy)
    {
    case 1:
        m_btnAnisotropy.m_cText.m_cParams.SetStrRef(STRREF_ANISOTROPY_OFF);
        break;
    case 2:
        m_btnAnisotropy.m_cText.m_cParams.SetStrRef(STRREF_ANISOTROPY_2X);
        break;
    case 4:
        m_btnAnisotropy.m_cText.m_cParams.SetStrRef(STRREF_ANISOTROPY_4X);
        break;
    case 8:
        m_btnAnisotropy.m_cText.m_cParams.SetStrRef(STRREF_ANISOTROPY_8X);
        break;
    case 16:
        m_btnAnisotropy.m_cText.m_cParams.SetStrRef(STRREF_ANISOTROPY_16X);
        break;
    }
    m_btnAnisotropyLeft.SetEnabledFlag(1);
    m_btnAnisotropyRight.SetEnabledFlag(1);
    if (m_nAnisotropy <= 1)
    {
        m_btnAnisotropyLeft.SetEnabledFlag(0);
    }
    else if (m_nAnisotropy >= AurQueryMaximumAnisotropy() ||
             m_nAnisotropy >= 16)
    {
        m_btnAnisotropyRight.SetEnabledFlag(0);
    }
    m_bChanged = 1;
}
// PC 0x0074E890
void CSWGuiOptionsGraphicsAdvanced::SetDescription(unsigned long nStrRef)
{
    m_lblDescription.m_cText.m_cParams.SetStrRef(nStrRef);
    CExoString sText(m_lblDescription.m_cText.m_cParams.m_sText);
    m_lbDescription.ClearItems();
    CSWGuiExtent cExtent = m_lblDescription.m_cExtent;
    if (sText.GetLength() > 0)
    {
        cExtent.m_nHeight = m_lblDescription.m_cText.GetIdealHeight();
    }
    else
    {
        cExtent.m_nHeight = m_lblDescription.m_cText.GetFontHeight();
    }
    m_lblDescription.SetExtent(cExtent);
    CSWGuiControl* pControl = &m_lblDescription;
    m_lbDescription.AddControls(&pControl, 1, 0, 0, 0);
    m_lbDescription.SetSelectedControl(&m_lblDescription, 0);
}
// PC 0x0074E990
void CSWGuiOptionsGraphicsAdvanced::SetUpOptions()
{
    CClientOptions* pOptions =
        g_pAppManager->m_pClientExoApp->GetClientOptions();
    if (pOptions != 0)
    {
        if (AurCheckFrameBufferModificationsAvailable() != 0)
        {
            int nFrameBuffer = pOptions->m_nFrameBuffer;
            m_bFrameBuffer = nFrameBuffer;
            CSWGuiButtonToggle* pToggle = m_cbFrameBuffer.AsButtonToggle();
            pToggle->m_bToggled = nFrameBuffer;
        }
        else
        {
            CSWGuiButtonToggle* pToggle = m_cbFrameBuffer.AsButtonToggle();
            pToggle->m_bToggled = 0;
            m_cbFrameBuffer.SetEnabled(0);
            m_bFrameBuffer = 0;
        }
        int nAnisotropy = pOptions->m_nAnisotropy;
        m_nAnisotropy = nAnisotropy;
        if (AurCheckSoftShadowsAvailable() != 0)
        {
            int nSoftShadows = pOptions->m_nSoftShadows;
            m_bSoftShadows = nSoftShadows;
            CSWGuiButtonToggle* pToggle = m_cbSoftShadows.AsButtonToggle();
            pToggle->m_bToggled = nSoftShadows;
        }
        else
        {
            CSWGuiButtonToggle* pToggle = m_cbSoftShadows.AsButtonToggle();
            pToggle->m_bToggled = 0;
            m_cbSoftShadows.SetEnabled(0);
            m_bSoftShadows = 0;
        }
        int nVSync = pOptions->m_nVSync;
        m_bVSync = nVSync;
        CSWGuiButtonToggle* pToggle = m_cbVSync.AsButtonToggle();
        pToggle->m_bToggled = nVSync;
        m_nAntiAlias = pOptions->m_nAntiAlias;
        m_nTexQual = pOptions->m_nTexQual;
    }
    SetAntiAliasText();
    SetAnisotropyText();
    C2DA* p2DA = new C2DA("texpacks", 0);
    if (p2DA != 0)
    {
        if (p2DA->Load2DArray() == 0)
        {
            delete p2DA;
            return;
        }
        m_nMaxTexQual = p2DA->m_nNumRows - 1;
        p2DA->Unload2DArray();
        delete p2DA;
        SetTexQualText();
    }
}
// PC 0x00750200
void CSWGuiOptionsResolution::HandleInputEvent(int nEvent, int nState)
{
    if (nState != 0)
    {
        switch (nEvent)
        {
        case 40:
        case 46:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            m_pManager->PopModalPanel();
            m_bDeferredRemoval = 0;
            m_nExitState = EXITSTATE_DELETE;
            if (m_nUnknown80C != 0)
            {
                CExoIni cIni;
                CExoString sValue;
                {
                    int bRead = cIni.ReadIniEntry(
                        sValue, ".\\swkotor2.ini", "Graphics Options",
                        "Disable Vertex Buffer Objects");
                    if (bRead != 0 && sValue.AsINT() == 1)
                    {
                        AurDisableVertexBufferObjects();
                    }
                }
                ReInitAurora(g_nScreenWidth, g_nScreenHeight, g_nBitsPerPixel,
                             g_pAurora->FullScreen(), 1);
                {
                    int bRead = cIni.ReadIniEntry(sValue, ".\\swkotor2.ini",
                                                  "Graphics Options",
                                                  "Disable Write-Only VBO");
                    if (bRead != 0)
                    {
                        if (sValue.AsINT() == 1)
                        {
                            AurDisableVertexBufferObjectsWriteOnly();
                        }
                        else
                        {
                            AurEnableVertexBufferObjectsWriteOnly();
                        }
                    }
                }
                m_nUnknown80C = 0;
            }
            break;
        case 39:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            OnResolutionChosen(0);
            break;
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}

#ifndef _XBOX
void CSWGuiOptionsResolution::OnResolutionChosen(CSWGuiControl* pControl)  // PC 0x0074EBB0
{
    if (pControl == 0 || pControl->m_bSelected != 0)
    {
        DWORD nMode = m_lbResolutions.GetSelectedControl()->m_nIndex;
        DEVMODE cDevMode;
        memset(&cDevMode, 0, sizeof(cDevMode));
        cDevMode.dmSize = 156;
        if (EnumDisplaySettings(0, nMode, &cDevMode))
        {
            g_nDesiredFullScreenRefreshRate = cDevMode.dmDisplayFrequency;
        }
        if (g_pAppManager->m_pClientExoApp->SetVideoMode(nMode) != 0)
        {
            CExoString sValue;
            CExoIni cIni;
            sValue.Format("%d", g_nScreenWidth);
            cIni.WriteIniEntry(sValue, ".\\swkotor2.ini", "Graphics Options", "Width");
            sValue.Format("%d", g_nScreenHeight);
            cIni.WriteIniEntry(sValue, ".\\swkotor2.ini", "Graphics Options", "Height");
            sValue.Format("%d", g_nDesiredFullScreenRefreshRate);
            cIni.WriteIniEntry(sValue, ".\\swkotor2.ini", "Graphics Options", "RefreshRate");
        }
        m_pManager->PopModalPanel();
        m_bDeferredRemoval = 0;
        m_nExitState = EXITSTATE_DELETE;
        m_nUnknown80C = 1;
        g_pGuiMan->ReloadPanels(g_nScreenWidth, g_nScreenHeight);
        g_pGuiMan->ReloadToolTipPanel();
    }
}
#endif
#endif  // PC 0x0074EFE0

void CSWGuiOptionsSound::OnMoveVolumeSlider(CSWGuiControl* pControl)
{
    int nMax = pControl->AsSlider()->m_nMaxValue;
    int nValue = pControl->AsSlider()->m_nCurValue;
#ifdef _XBOX
    float fVolume = (float)nValue / (float)nMax;
#else
    float fVolume = (double)nValue / (double)nMax;
#endif
    if (pControl == &m_sldMusic)
    {
        g_pExoSound->SetMusicVolume(fVolume, 0);
        g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nMusicVolume =
            nValue;
    }
    else if (pControl == &m_sldVoice)
    {
        g_pExoSound->SetDialogVolume(fVolume);
        g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nVoiceVolume =
            nValue;
    }
    else if (pControl == &m_sldEffects)
    {
        g_pExoSound->SetSoundEffectVolume(fVolume);
        g_pAppManager->m_pClientExoApp->GetClientOptions()
            ->m_nSoundEffectsVolume = nValue;
    }
    else if (pControl == &m_sldMovie)
    {
        g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nMovieVolume =
            nValue;
    }
}
// PC 0x00750470
void CSWGuiOptionsSound::OnEnterSlider(CSWGuiControl* pControl)
{
#ifdef _XBOX
    int bSlider = 0;
    if (pControl == &m_sldGamma)
    {
        SetDescription(42310);
        bSlider = 1;
    }
    else if (pControl == &m_sldMusic)
    {
        SetDescription(42311);
        bSlider = 1;
    }
    else if (pControl == &m_sldVoice)
    {
        SetDescription(42312);
        bSlider = 1;
    }
    else if (pControl == &m_sldEffects)
    {
        SetDescription(42313);
        bSlider = 1;
    }
    else if (pControl == &m_sldMovie)
    {
        SetDescription(47921);
        bSlider = 1;
    }
    m_lblY.SetEnabledFlag(bSlider);
    m_lblDefault.SetEnabledFlag(bSlider);
#else
    SetDescription(pControl->m_nIndex);
#endif
    CSWGuiImageParams* pParams = &pControl->AsSlider()->m_cImage.m_cParams;
    pParams->m_bPulsing = 1;
}
// PC 0x007504A0
void CSWGuiOptionsSound::OnDefault(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
#ifdef _XBOX
        m_pManager->PlayGuiSound(pControl->m_nClickSound);
        CSWGuiControl* pSlider = &m_sldGamma;
        if (pControl == pSlider)
        {
            g_pAppManager->GetClientExoApp()->GetClientOptions()->SetDefaultVideoOptions();
            pSlider->AsSlider()->SetCurValue(
                g_pAppManager->GetClientExoApp()->GetClientOptions()->m_nBrightness);
            g_pAurora->ResetGammaRamp(
                g_pAppManager->GetClientExoApp()->GetClientOptions()->GetGammaSetting(), 0);
            return;
        }
        if (pControl == (pSlider = &m_sldMusic))
        {
            g_pAppManager->GetClientExoApp()->GetClientOptions()->m_nMusicVolume = 85;
        }
        else if (pControl == (pSlider = &m_sldVoice))
        {
            g_pAppManager->GetClientExoApp()->GetClientOptions()->m_nVoiceVolume = 85;
        }
        else if (pControl == (pSlider = &m_sldEffects))
        {
            g_pAppManager->GetClientExoApp()->GetClientOptions()->m_nSoundEffectsVolume = 85;
        }
        else if (pControl == (pSlider = &m_sldMovie))
        {
            g_pAppManager->GetClientExoApp()->GetClientOptions()->m_nMovieVolume = 85;
        }
        else
        {
            return;
        }
        pSlider->AsSlider()->SetCurValue(85);
        OnMoveVolumeSlider(pSlider);
#else
        CClientOptions* pOptions =
            g_pAppManager->m_pClientExoApp->GetClientOptions();
        pOptions->SetDefaultSoundOptions();
        SetUpOptions();
#endif
    }
}
// PC 0x0074F1E0
void CSWGuiOptionsSound::SetUpOptions()
{
    CClientOptions* pOptions =
        g_pAppManager->m_pClientExoApp->GetClientOptions();
    if (pOptions != 0)
    {
        m_sldMusic.SetCurValue(pOptions->m_nMusicVolume);
        m_sldVoice.SetCurValue(pOptions->m_nVoiceVolume);
        m_sldEffects.SetCurValue(pOptions->m_nSoundEffectsVolume);
        m_sldMovie.SetCurValue(pOptions->m_nMovieVolume);
        OnMoveVolumeSlider(&m_sldMusic);
        OnMoveVolumeSlider(&m_sldVoice);
        OnMoveVolumeSlider(&m_sldEffects);
        OnMoveVolumeSlider(&m_sldMovie);
    }
}
// PC 0x007504D0
void CSWGuiOptionsSoundAdvanced::HandleInputEvent(int nEvent, int nState)
{
    if (nState != 0)
    {
        switch (nEvent)
        {
        case GUIEVENT_PANEL_CLICK:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            break;
        case GUIEVENT_CANCEL:
        case GUIEVENT_CANCEL_ALTERNATE:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            if (m_bDiscard != 0)
            {
                m_bDiscard = 0;
            }
            else
            {
                SetNewOptions();
                if (m_bChanged != 0)
                {
                    CClientOptions* pOptions =
                        g_pAppManager->m_pClientExoApp->GetClientOptions();
                    int nEAXFlags = pOptions->m_nEAXFlags;
#ifndef _XBOX
                    g_pExoSound->ReInitialize(m_nEAXMode,
                                              nEAXFlags == 0);
#endif
                    CExoIni cIni;
                    CExoString sValue;
                    sValue.Format("%d", g_pExoSound->GetEAX());
                    {
                        cIni.WriteIniEntry(sValue, ".\\swkotor2.ini",
                                           "Sound Options", "EAX");
                    }
                    CClientExoApp* pApp = g_pAppManager->m_pClientExoApp;
                    unsigned char nEAX = g_pExoSound->GetEAX();
                    pApp->GetClientOptions()->m_nEAXMode = nEAX;
                    CClientOptions* pOptions2 =
                        g_pAppManager->m_pClientExoApp->GetClientOptions();
                    int nSavedEAXFlags = pOptions2->m_nEAXFlags;
                    sValue.Format("%d", nSavedEAXFlags);
                    {
                        cIni.WriteIniEntry(sValue, ".\\swkotor2.ini",
                                           "Sound Options", "Force Software");
                    }
                }
            }
            m_bChanged = 0;
            m_pManager->PopModalPanel();
            m_bDeferredRemoval = 0;
            m_nExitState = EXITSTATE_DELETE;
            break;
        case GUIEVENT_SCROLL_UP:
            m_lbDescription.HandleInputEvent(GUIEVENT_UP, 1);
            break;
        case GUIEVENT_SCROLL_DOWN:
            m_lbDescription.HandleInputEvent(GUIEVENT_DOWN, 1);
            break;
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
// PC 0x0074F270
void CSWGuiOptionsSoundAdvanced::SetNewOptions()
{
    if (g_pAppManager->m_pClientExoApp->GetClientOptions() != 0)
    {
        unsigned char nMode = m_nEAXMode;
        g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nEAXMode = nMode;
        int nFlags = m_bForceSoftware;
        g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nEAXFlags =
            nFlags;
    }
}
// PC 0x007507C0
void CSWGuiOptionsSoundAdvanced::OnEAXLeft(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        if (m_nEAXMode > 0)
        {
            m_nEAXMode = m_nEAXMode - 1;
            SetEAXText();
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
            m_bChanged = 1;
        }
    }
}
// PC 0x00750800
void CSWGuiOptionsSoundAdvanced::OnEAXRight(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        int nBest;
#ifdef _XBOX
        nBest = 3;
#else
        if (g_pExoSound->GetBestEAXAvailable() > 3)
        {
            nBest = 3;
        }
        else
        {
            nBest = g_pExoSound->GetBestEAXAvailable();
        }
#endif
        if (m_nEAXMode != nBest)
        {
            m_nEAXMode = m_nEAXMode + 1;
            SetEAXText();
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
            m_bChanged = 1;
        }
    }
}
// PC 0x00750860
void CSWGuiOptionsSoundAdvanced::OnForceSoftware(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        m_bForceSoftware = (m_bForceSoftware == 0);
        if (m_bForceSoftware == 0)
        {
            m_nEAXMode = 0;
            SetEAXText();
        }
        EnableEAXControls(m_bForceSoftware == 0);
        m_bChanged = 1;
    }
}
// PC 0x00751E50
void CSWGuiOptionsSoundAdvanced::OnDefault(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        CClientOptions* pOptions =
            g_pAppManager->m_pClientExoApp->GetClientOptions();
        pOptions->SetDefaultAdvancedSoundOptions();
        SetUpOptions();
        m_bChanged = 1;
    }
}
// PC 0x007508C0
void CSWGuiOptionsSoundAdvanced::SetUpOptions()
{
    CClientOptions* pOptions =
        g_pAppManager->m_pClientExoApp->GetClientOptions();
    if (pOptions != 0)
    {
        m_nEAXMode = pOptions->m_nEAXMode;
        SetEAXText();
        int bForceSoftware = pOptions->m_nEAXFlags;
        m_bForceSoftware = bForceSoftware;
        CSWGuiButtonToggle* pToggle = m_cbForceSoftware.AsButtonToggle();
        pToggle->m_bToggled = bForceSoftware;
        EnableEAXControls(pOptions->m_nEAXFlags == 0);
    }
    m_bChanged = 0;
}
// PC 0x0074F780
void CSWGuiOptionsMouse::OnRevMouseButtons(CSWGuiControl* pControl)
{
    int bSelected = pControl->m_bSelected;
    if (bSelected != 0)
    {
        CExoString sValue;
        CExoIni cIni;
        int bReverse = g_pAppManager->m_pClientExoApp->GetClientOptions()
                           ->m_bReverseMouseButtons;
        CClientOptions* pOptions =
            g_pAppManager->m_pClientExoApp->GetClientOptions();
        pOptions->m_bReverseMouseButtons = (bReverse == 0);
        int nReverse = g_pAppManager->m_pClientExoApp->GetClientOptions()
                           ->m_bReverseMouseButtons;
        sValue.Format("%d", nReverse);
        CExoString sKey("Reverse Mouse Buttons");
        CExoString sSection("Game Options");
        CExoString sFile(".\\swkotor2.ini");
        cIni.WriteIniEntry(sValue, sFile, sSection, sKey);
    }
}
// PC 0x00750940
void CSWGuiOptionsMouse::OnEnterRevMouseButtons(CSWGuiControl* pControl)
{
    SetDescription(48722);
}
// PC 0x00750950
void CSWGuiOptionsMouse::OnDefault(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        CClientOptions* pOptions =
            g_pAppManager->m_pClientExoApp->GetClientOptions();
        pOptions->SetDefaultMouseOptions();
        SetUpOptions();
    }
}
// PC 0x00752430
CSWGuiOptionsFeedback::CSWGuiOptionsFeedback(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
#ifdef _XBOX
    StartLoadFromLayout("optfeedback");
    InitControl(m_aCheckboxes[0], "CB_HIDE", 1);
    InitControl(m_aCheckboxes[1], "CB_TUTORIAL", 1);
    InitControl(m_aCheckboxes[2], "CB_SUBTITLES", 1);
    InitControl(m_aCheckboxes[3], "CB_MINIMAP", 1);
    InitControl(m_aCheckboxes[4], "CB_RUMBLE", 1);
    InitControl(m_aCheckboxes[5], "CB_FLOATY", 1);
    InitControl(m_aCheckboxes[6], "CB_SUMMARY", 1);
    InitControl(m_lblA, "LBL_A", 1);
    InitControl(m_lblSelect, "LBL_SELECT", 1);
    InitControl(m_lblB, "LBL_B", 1);
    InitControl(m_lblBack, "LBL_BACK", 1);
    InitControl(m_lblTitle, "LBL_TITLE", 1);
    InitControl(m_lblRAnalog, "LBL_RANALOG", 1);
    InitControl(m_lbDescription, "LB_DESC", 1);
    m_lbDescription.m_nSelectSound = CSWGuiManager::GUISOUND_SELECTION;
    StopLoadFromLayout();
    CSWGuiLabel* pProtoLabel = (CSWGuiLabel*)m_lbDescription.m_pProtoItem;
    m_lblDescription.Initialize(pProtoLabel->m_cExtent, pProtoLabel, 1.0f);
    m_aCheckboxes[0].AddEvent(39, this,
                              (CSWGuiHandler)&CSWGuiOptionsFeedback::OnHide);
    m_aCheckboxes[1].AddEvent(39, this,
                              (CSWGuiHandler)&CSWGuiOptionsFeedback::OnTutorial);
    m_aCheckboxes[2].AddEvent(39, this,
                              (CSWGuiHandler)&CSWGuiOptionsFeedback::OnSubtitles);
    m_aCheckboxes[3].AddEvent(39, this,
                              (CSWGuiHandler)&CSWGuiOptionsFeedback::OnMiniMap);
    m_aCheckboxes[4].AddEvent(39, this,
                              (CSWGuiHandler)&CSWGuiOptionsFeedback::OnRumble);
    m_aCheckboxes[5].AddEvent(39, this,
                              (CSWGuiHandler)&CSWGuiOptionsFeedback::OnFloaty);
    m_aCheckboxes[6].AddEvent(39, this,
                              (CSWGuiHandler)&CSWGuiOptionsFeedback::OnSummary);
    m_aCheckboxes[0].AddEvent(45, this,
                              (CSWGuiHandler)&CSWGuiOptionsFeedback::OnHide);
    m_aCheckboxes[1].AddEvent(45, this,
                              (CSWGuiHandler)&CSWGuiOptionsFeedback::OnTutorial);
    m_aCheckboxes[2].AddEvent(45, this,
                              (CSWGuiHandler)&CSWGuiOptionsFeedback::OnSubtitles);
    m_aCheckboxes[3].AddEvent(45, this,
                              (CSWGuiHandler)&CSWGuiOptionsFeedback::OnMiniMap);
    m_aCheckboxes[4].AddEvent(45, this,
                              (CSWGuiHandler)&CSWGuiOptionsFeedback::OnRumble);
    m_aCheckboxes[5].AddEvent(45, this,
                              (CSWGuiHandler)&CSWGuiOptionsFeedback::OnFloaty);
    m_aCheckboxes[6].AddEvent(45, this,
                              (CSWGuiHandler)&CSWGuiOptionsFeedback::OnSummary);
    for (int i = 0; i < 7; i++)
    {
        m_aCheckboxes[i].AddEvent(
            0, this, (CSWGuiHandler)&CSWGuiOptionsFeedback::OnEnterSlot);
    }
    CSWGuiPanel* pPanel = this;
    pPanel->SetActiveControl(&m_aCheckboxes[0], 0);
    CClientOptions* pOptions =
        g_pAppManager->m_pClientExoApp->GetClientOptions();
    if (pOptions != 0)
    {
        unsigned int bHide = pOptions->m_bHideUnequippable;
        CSWGuiButtonToggle* pToggle0 = m_aCheckboxes[0].AsButtonToggle();
        pToggle0->m_bToggled = bHide;
        unsigned int bTutorial = pOptions->m_bTutorialPopups;
        CSWGuiButtonToggle* pToggle1 = m_aCheckboxes[1].AsButtonToggle();
        pToggle1->m_bToggled = bTutorial;
        unsigned int bSubtitles = pOptions->m_bSubtitles;
        CSWGuiButtonToggle* pToggle2 = m_aCheckboxes[2].AsButtonToggle();
        pToggle2->m_bToggled = bSubtitles;
        unsigned int bMiniMap = pOptions->m_bMiniMap;
        CSWGuiButtonToggle* pToggle3 = m_aCheckboxes[3].AsButtonToggle();
        pToggle3->m_bToggled = bMiniMap;
        unsigned int bFloaty = pOptions->m_nXboxUnmappedBit26;
        CSWGuiButtonToggle* pToggle5 = m_aCheckboxes[5].AsButtonToggle();
        pToggle5->m_bToggled = bFloaty;
        unsigned int bRumble = pOptions->m_bFloatingNumbers;
        CSWGuiButtonToggle* pToggle4 = m_aCheckboxes[4].AsButtonToggle();
        pToggle4->m_bToggled = bRumble;
        unsigned int bSummary = pOptions->m_bStatusSummary;
        CSWGuiButtonToggle* pToggle6 = m_aCheckboxes[6].AsButtonToggle();
        pToggle6->m_bToggled = bSummary;
    }
#else
    StartLoadFromLayout("optfeedback_p");
    InitControl(m_lbOptions, "LB_OPTIONS", 1);
    InitControl(m_lblTitle, "LBL_TITLE", 1);
    InitControl(m_lbDescription, "LB_DESC", 1);
    m_lbDescription.m_nSelectSound = CSWGuiManager::GUISOUND_SELECTION;
    InitControl(m_btnBack, "BTN_BACK", 1);
    InitControl(m_btnDefault, "BTN_DEFAULT", 1);
    InitControl(m_alblBar[0], "LBL_BAR1", 1);
    InitControl(m_alblBar[1], "LBL_BAR2", 1);
    InitControl(m_alblBar[2], "LBL_BAR3", 1);
    StopLoadFromLayout();
    PopulateOptionsListbox();
    CSWGuiLabel* pProtoLabel = (CSWGuiLabel*)m_lbDescription.m_pProtoItem;
    m_lblDescription.Initialize(pProtoLabel->m_cExtent, pProtoLabel, 1.0f);
    CSWGuiButton* pDefault = &m_btnDefault;
    pDefault->AddEvent(39, this,
                       (CSWGuiHandler)&CSWGuiOptionsFeedback::OnDefault);
    pDefault->m_bFocusable = 0;
    pDefault->AddEvent(45, this,
                       (CSWGuiHandler)&CSWGuiOptionsFeedback::OnDefault);
    CSWGuiButton* pBack = &m_btnBack;
    pBack->m_bFocusable = 0;
    pBack->AddEvent(GUIEVENT_ACTIVATE, this, (CSWGuiHandler)&CSWGuiPanel::OnBButtonPressed);
    SetActiveControl(&m_lbOptions, 0);
    SetActiveButton(OPTION_HIDE_UNEQUIPPABLE);
    SetUpOptions();
#endif
}

#ifndef _XBOX
void CSWGuiOptionsGraphics::OnAdvanced(CSWGuiControl* pControl)  // PC 0x00752780
{
    int bSelected = pControl->m_bSelected;
    if (bSelected != 0)
    {
        CSWGuiOptionsGraphicsAdvanced* pPanel =
            new CSWGuiOptionsGraphicsAdvanced(GetManager(), this);
        m_pManager->AddPanel(pPanel, CSWGuiManager::PANEL_MODE_MODAL, 1);
    }
}
#endif
// PC 0x00752800
void CSWGuiOptionsSound::OnAdvanced(CSWGuiControl* pControl)
{
    int bSelected = pControl->m_bSelected;
    if (bSelected != 0)
    {
        CSWGuiOptionsSoundAdvanced* pPanel =
            new CSWGuiOptionsSoundAdvanced(GetManager());
        m_pManager->AddPanel(pPanel, CSWGuiManager::PANEL_MODE_MODAL, 1);
    }
}
// PC 0x0074F8D0
void CSWGuiOptionsMouse::SetUpOptions()
{
    CClientOptions* pOptions =
        g_pAppManager->m_pClientExoApp->GetClientOptions();
    if (pOptions != 0)
    {
        unsigned char nSensitivity = pOptions->m_nMouseSensitivity;
        m_sldMouseSen.AsSlider()->SetCurValue(nSensitivity);
        int bReverse = pOptions->m_bReverseMouseButtons;
        CSWGuiButtonToggle* pToggle = m_cbRevButtons.AsButtonToggle();
        pToggle->m_bToggled = bReverse;
    }
}
// PC 0x0074F940
void CSWGuiOptionsMouse::OnMoveMouseSenSlider(CSWGuiControl* pControl)
{
    pControl->AsSlider();
    int nValue = pControl->AsSlider()->m_nCurValue;
    g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nMouseSensitivity =
        nValue;
}
// PC 0x0074F9D0
void CSWGuiOptionsMain::HandleInputEvent(int nEvent, int nState)
{
    if (nState != 0)
    {
        switch (nEvent)
        {
        case 45:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            break;
        case 40:
        case 46:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
#ifndef _XBOX
            g_pAppManager->m_pClientExoApp->EnableMainMenuIdleTimer();
#endif
            m_pManager->PopModalPanel();
#ifdef _XBOX
            m_pManager->RemovePanel(this);
#else
            m_bDeferredRemoval = 0;
            m_nExitState = EXITSTATE_DELETE;
#endif
            ClosePanel();
            break;
#ifndef _XBOX
        case 57:
            m_lbDescription.HandleInputEvent(GUIEVENT_UP, 1);
            break;
        case 58:
            m_lbDescription.HandleInputEvent(GUIEVENT_DOWN, 1);
            break;
#endif
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
// PC 0x0074FAE0
void CSWGuiOptionsMain::OnEnterSlot(CSWGuiControl* pControl)
{
#ifdef _XBOX
    int nSlot = -1;
    int i = 0;
    CSWGuiButton* pButton = m_btnOptions;
    do
    {
        if (nSlot != -1)
        {
            break;
        }
        if (pControl == pButton)
        {
            nSlot = i;
        }
        i++;
        pButton++;
    } while (i < 5);
    switch (nSlot)
    {
    case 0:
        SetDescription(42273);
        break;
    case 1:
        SetDescription(42274);
        break;
    case 2:
        SetDescription(42275);
        break;
    case 3:
        SetDescription(42276);
        break;
    case 4:
        SetDescription(42277);
        break;
    }
#else
    SetDescription(pControl->m_nIndex);
#endif
}

#ifndef _XBOX
CSWGuiOptionsFeedback::~CSWGuiOptionsFeedback()  // PC 0x0074FAF0
{
}
#endif

#ifndef _XBOX
void CSWGuiOptionsFeedback::PopulateOptionsListbox()  // PC 0x007509A0
{
    CSWGuiControl* pProto = m_lbOptions.m_pProtoItem;
    CExoArrayList<CSWGuiControl*> lstControls;
    CClientOptions* pOptions =
        g_pAppManager->GetClientExoApp()->GetClientOptions();
    if (pOptions == 0)
    {
        return;
    }
    for (int i = 0; i < OPTION_COUNT; i++)
    {
        CSWGuiOptionsCheckbox* pCheckbox = new CSWGuiOptionsCheckbox;
        pCheckbox->Initialize(pProto->m_cExtent, (CSWGuiButtonToggle*)pProto);
        pCheckbox->AddEvent(GUIEVENT_MOUSE_ENTER, this,
                            (CSWGuiHandler)&CSWGuiOptionsFeedback::OnEnterSlot);
        switch (i)
        {
        case OPTION_HIDE_UNEQUIPPABLE:
            pCheckbox->m_cText.m_cParams.SetStrRef(STRREF_HIDE_UNEQUIPPABLE_LABEL);
            pCheckbox->m_nIndex = STRREF_HIDE_UNEQUIPPABLE_DESCRIPTION;
            pCheckbox->AddEvent(GUIEVENT_ACTIVATE, this,
                                (CSWGuiHandler)&CSWGuiOptionsFeedback::OnHide);
            pCheckbox->AddEvent(GUIEVENT_ACCEPT, this,
                                (CSWGuiHandler)&CSWGuiOptionsFeedback::OnHide);
            break;
        case OPTION_TUTORIAL_POPUPS:
            pCheckbox->m_cText.m_cParams.SetStrRef(STRREF_TUTORIAL_POPUPS_LABEL);
            pCheckbox->m_nIndex = STRREF_TUTORIAL_POPUPS_DESCRIPTION;
            pCheckbox->AddEvent(
                GUIEVENT_ACTIVATE, this, (CSWGuiHandler)&CSWGuiOptionsFeedback::OnTutorial);
            pCheckbox->AddEvent(
                GUIEVENT_ACCEPT, this, (CSWGuiHandler)&CSWGuiOptionsFeedback::OnTutorial);
            break;
        case OPTION_SUBTITLES:
            pCheckbox->m_cText.m_cParams.SetStrRef(STRREF_SUBTITLES_LABEL);
            pCheckbox->m_nIndex = STRREF_SUBTITLES_DESCRIPTION;
            pCheckbox->AddEvent(
                GUIEVENT_ACTIVATE, this, (CSWGuiHandler)&CSWGuiOptionsFeedback::OnSubtitles);
            pCheckbox->AddEvent(
                GUIEVENT_ACCEPT, this, (CSWGuiHandler)&CSWGuiOptionsFeedback::OnSubtitles);
            break;
        case OPTION_MINI_MAP:
            pCheckbox->m_cText.m_cParams.SetStrRef(STRREF_MINI_MAP_LABEL);
            pCheckbox->m_nIndex = STRREF_MINI_MAP_DESCRIPTION;
            pCheckbox->AddEvent(
                GUIEVENT_ACTIVATE, this, (CSWGuiHandler)&CSWGuiOptionsFeedback::OnMiniMap);
            pCheckbox->AddEvent(
                GUIEVENT_ACCEPT, this, (CSWGuiHandler)&CSWGuiOptionsFeedback::OnMiniMap);
            break;
        case OPTION_FLOATING_NUMBERS:
            pCheckbox->m_cText.m_cParams.SetStrRef(STRREF_FLOATING_NUMBERS_LABEL);
            pCheckbox->m_nIndex = STRREF_FLOATING_NUMBERS_DESCRIPTION;
            pCheckbox->AddEvent(
                GUIEVENT_ACTIVATE, this, (CSWGuiHandler)&CSWGuiOptionsFeedback::OnFloaty);
            pCheckbox->AddEvent(
                GUIEVENT_ACCEPT, this, (CSWGuiHandler)&CSWGuiOptionsFeedback::OnFloaty);
            break;
        case OPTION_STATUS_SUMMARY:
            pCheckbox->m_cText.m_cParams.SetStrRef(STRREF_STATUS_SUMMARY_LABEL);
            pCheckbox->m_nIndex = STRREF_STATUS_SUMMARY_DESCRIPTION;
            pCheckbox->AddEvent(
                GUIEVENT_ACTIVATE, this, (CSWGuiHandler)&CSWGuiOptionsFeedback::OnSummary);
            pCheckbox->AddEvent(
                GUIEVENT_ACCEPT, this, (CSWGuiHandler)&CSWGuiOptionsFeedback::OnSummary);
            break;
        case OPTION_HIDE_IN_GAME_GUI:
            pCheckbox->m_cText.m_cParams.SetStrRef(STRREF_HIDE_IN_GAME_GUI_LABEL);
            pCheckbox->m_nIndex = STRREF_HIDE_IN_GAME_GUI_DESCRIPTION;
            pCheckbox->AddEvent(
                GUIEVENT_ACTIVATE, this,
                (CSWGuiHandler)&CSWGuiOptionsFeedback::OnHideInGameGUI);
            pCheckbox->AddEvent(
                GUIEVENT_ACCEPT, this,
                (CSWGuiHandler)&CSWGuiOptionsFeedback::OnHideInGameGUI);
            break;
        case OPTION_ENABLE_TOOLTIPS:
            pCheckbox->m_cText.m_cParams.SetStrRef(STRREF_ENABLE_TOOLTIPS_LABEL);
            pCheckbox->m_nIndex = STRREF_ENABLE_TOOLTIPS_DESCRIPTION;
            pCheckbox->AddEvent(
                GUIEVENT_ACTIVATE, this,
                (CSWGuiHandler)&CSWGuiOptionsFeedback::OnEnableToolTips);
            pCheckbox->AddEvent(
                GUIEVENT_ACCEPT, this,
                (CSWGuiHandler)&CSWGuiOptionsFeedback::OnEnableToolTips);
            break;
        }
        lstControls.Add(pCheckbox);
    }
    CSWGuiListBox* pList = &m_lbOptions;
    pList->ClearItems();
    pList->AddControls(lstControls, 1, 1, 0);
    pList->m_bLooping = 0;
}
#endif

#ifndef _XBOX
void CSWGuiOptionsGraphics::OnResolution(CSWGuiControl* pControl)
{
    int bSelected = pControl->m_bSelected;
    if (bSelected != 0)
    {
        CSWGuiPanel* pPanel;
        if (m_bInGame)
        {
            CSWGuiMessageBox* pMessageBox =
                g_pAppManager->m_pClientExoApp->GetInGameGui()->m_pMessageBox;
            pMessageBox->SetAllowCancel(0);
            pMessageBox->SetCallback(NULL, NULL, NULL);
            pMessageBox->SetMessage(STRREF_CANNOT_CHANGE_RESOLUTION);
            pPanel = pMessageBox;
        }
        else
        {
            CSWGuiOptionsResolution* pResolution = new CSWGuiOptionsResolution(GetManager());
            pResolution->CenterPanel();
            pPanel = pResolution;
        }
        m_pManager->AddPanel(pPanel, CSWGuiManager::PANEL_MODE_BACKGROUND, 1);
    }
}
// PC 0x00750CD0
CSWGuiOptionsGraphicsAdvanced::CSWGuiOptionsGraphicsAdvanced(
    CSWGuiManager* pManager, CSWGuiOptionsGraphics* pGraphics)
    : CSWGuiPanel(pManager)
{
    StartLoadFromLayout("optgraphicadv_p");
    InitControl(m_lblTitle, "LBL_TITLE", 1);
    InitControl(m_lbDescription, "LB_DESC", 1);
    InitControl(m_btnAntiAlias, "BTN_ANTIALIAS", 1);
    InitControl(m_btnAntiAliasLeft, "BTN_ANTIALIASLEFT", 1);
    InitControl(m_btnAntiAliasRight, "BTN_ANTIALIASRIGHT", 1);
    InitControl(m_btnTexQual, "BTN_TEXQUAL", 1);
    InitControl(m_btnTexQualLeft, "BTN_TEXQUALLEFT", 1);
    InitControl(m_btnTexQualRight, "BTN_TEXQUALRIGHT", 1);
    InitControl(m_cbFrameBuffer, "CB_FRAMEBUFF", 1);
    InitControl(m_btnAnisotropy, "BTN_ANISOTROPY", 1);
    InitControl(m_btnAnisotropyLeft, "BTN_ANISOTROPYLEFT", 1);
    InitControl(m_btnAnisotropyRight, "BTN_ANISOTROPYRIGHT", 1);
    InitControl(m_cbSoftShadows, "CB_SOFTSHADOWS", 1);
    InitControl(m_cbVSync, "CB_VSYNC", 1);
    InitControl(m_alblBar[0], "LBL_BAR1", 1);
    InitControl(m_alblBar[1], "LBL_BAR2", 1);
    InitControl(m_alblBar[2], "LBL_BAR3", 1);
    InitControl(m_alblBar[3], "LBL_BAR4", 1);
    m_btnAntiAlias.m_nIndex = STRREF_ANTIALIAS_DESCRIPTION;
    m_btnAntiAliasLeft.m_nIndex = STRREF_ANTIALIAS_DESCRIPTION;
    m_btnAntiAliasRight.m_nIndex = STRREF_ANTIALIAS_DESCRIPTION;
    m_btnTexQual.m_nIndex = STRREF_TEXTURE_QUALITY_DESCRIPTION;
    m_btnTexQualLeft.m_nIndex = STRREF_TEXTURE_QUALITY_DESCRIPTION;
    m_btnTexQualRight.m_nIndex = STRREF_TEXTURE_QUALITY_DESCRIPTION;
    m_cbFrameBuffer.m_nIndex = STRREF_FRAME_BUFFER_DESCRIPTION;
    m_btnAnisotropy.m_nIndex = STRREF_ANISOTROPY_DESCRIPTION;
    m_btnAnisotropyLeft.m_nIndex = STRREF_ANISOTROPY_DESCRIPTION;
    m_btnAnisotropyRight.m_nIndex = STRREF_ANISOTROPY_DESCRIPTION;
    m_cbSoftShadows.m_nIndex = STRREF_SOFT_SHADOWS_DESCRIPTION;
    m_cbVSync.m_nIndex = STRREF_VSYNC_DESCRIPTION;
    m_lbDescription.m_nSelectSound = CSWGuiManager::GUISOUND_SELECTION;
    InitControl(m_btnBack, "BTN_BACK", 1);
    InitControl(m_btnDefault, "BTN_DEFAULT", 1);
    InitControl(m_btnCancel, "BTN_CANCEL", 1);
    StopLoadFromLayout();
    CSWGuiLabel* pProtoLabel = (CSWGuiLabel*)m_lbDescription.m_pProtoItem;
    m_lblDescription.Initialize(pProtoLabel->m_cExtent, pProtoLabel, 1.0f);
    m_btnAntiAlias.AddEvent(
        0, this, (CSWGuiHandler)&CSWGuiOptionsGraphicsAdvanced::OnEnterSlot);
    m_btnAntiAliasLeft.AddEvent(
        0, this, (CSWGuiHandler)&CSWGuiOptionsGraphicsAdvanced::OnEnterSlot);
    m_btnAntiAliasRight.AddEvent(
        0, this, (CSWGuiHandler)&CSWGuiOptionsGraphicsAdvanced::OnEnterSlot);
    m_btnTexQual.AddEvent(
        0, this, (CSWGuiHandler)&CSWGuiOptionsGraphicsAdvanced::OnEnterSlot);
    m_btnTexQualLeft.AddEvent(
        0, this, (CSWGuiHandler)&CSWGuiOptionsGraphicsAdvanced::OnEnterSlot);
    m_btnTexQualRight.AddEvent(
        0, this, (CSWGuiHandler)&CSWGuiOptionsGraphicsAdvanced::OnEnterSlot);
    m_cbFrameBuffer.AddEvent(
        0, this, (CSWGuiHandler)&CSWGuiOptionsGraphicsAdvanced::OnEnterSlot);
    m_btnAnisotropy.AddEvent(
        0, this, (CSWGuiHandler)&CSWGuiOptionsGraphicsAdvanced::OnEnterSlot);
    m_btnAnisotropyLeft.AddEvent(
        0, this, (CSWGuiHandler)&CSWGuiOptionsGraphicsAdvanced::OnEnterSlot);
    m_btnAnisotropyRight.AddEvent(
        0, this, (CSWGuiHandler)&CSWGuiOptionsGraphicsAdvanced::OnEnterSlot);
    m_cbSoftShadows.AddEvent(
        0, this, (CSWGuiHandler)&CSWGuiOptionsGraphicsAdvanced::OnEnterSlot);
    m_cbVSync.AddEvent(
        0, this, (CSWGuiHandler)&CSWGuiOptionsGraphicsAdvanced::OnEnterSlot);
    m_btnAntiAliasLeft.AddEvent(
        39, this,
        (CSWGuiHandler)&CSWGuiOptionsGraphicsAdvanced::OnAntiAliasLeft);
    m_btnAntiAliasRight.AddEvent(
        39, this,
        (CSWGuiHandler)&CSWGuiOptionsGraphicsAdvanced::OnAntiAliasRight);
    m_btnTexQualLeft.AddEvent(
        39, this, (CSWGuiHandler)&CSWGuiOptionsGraphicsAdvanced::OnTexQualLeft);
    m_btnTexQualRight.AddEvent(
        39, this,
        (CSWGuiHandler)&CSWGuiOptionsGraphicsAdvanced::OnTexQualRight);
    m_btnAnisotropyLeft.AddEvent(
        39, this,
        (CSWGuiHandler)&CSWGuiOptionsGraphicsAdvanced::OnAnisotropyLeft);
    m_btnAnisotropyRight.AddEvent(
        39, this,
        (CSWGuiHandler)&CSWGuiOptionsGraphicsAdvanced::OnAnisotropyRight);
    m_cbFrameBuffer.AddEvent(
        39, this, (CSWGuiHandler)&CSWGuiOptionsGraphicsAdvanced::OnFrameBuffer);
    m_cbSoftShadows.AddEvent(
        39, this, (CSWGuiHandler)&CSWGuiOptionsGraphicsAdvanced::OnSoftShadows);
    m_cbVSync.AddEvent(39, this,
                       (CSWGuiHandler)&CSWGuiOptionsGraphicsAdvanced::OnVSync);
    SetUpOptions();
    CSWGuiButton* pBack = &m_btnBack;
    pBack->m_bFocusable = 0;
    pBack->AddEvent(GUIEVENT_ACTIVATE, this, (CSWGuiHandler)&CSWGuiPanel::OnBButtonPressed);
    CSWGuiButton* pDefault = &m_btnDefault;
    pDefault->m_bFocusable = 0;
    pDefault->AddEvent(
        39, this, (CSWGuiHandler)&CSWGuiOptionsGraphicsAdvanced::OnDefault);
    CSWGuiButton* pCancel = &m_btnCancel;
    pCancel->m_bFocusable = 0;
    pCancel->AddEvent(
        39, this,
        (CSWGuiHandler)&CSWGuiOptionsGraphicsAdvanced::OnDiscardChanges);
    SetActiveControl(&m_btnAntiAlias, 0);
    if (AurQueryAntiAliasingMode(2) == 0 && AurQueryAntiAliasingMode(4) == 0 &&
        AurQueryAntiAliasingMode(6) == 0 && AurQueryAntiAliasingMode(8) == 0)
    {
        m_btnAntiAlias.SetEnabled(0);
        m_btnAntiAlias.m_cText.m_cParams.SetColor(CGuiInGame::DISABLED_TEXT);
        m_btnAntiAliasLeft.SetEnabled(0);
        m_btnAntiAliasRight.SetEnabled(0);
    }
    m_bDiscard = 0;
    m_bChanged = 0;
    m_pGraphics = pGraphics;
}
#endif
// PC 0x007528D0
CSWGuiOptionsSoundAdvanced::CSWGuiOptionsSoundAdvanced(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    StartLoadFromLayout("optsoundadv_p");
    InitControl(m_cbForceSoftware, "CB_FORCESOFTWARE", 1);
    InitControl(m_btnEAX, "BTN_EAX", 1);
    InitControl(m_btnEAXRight, "BTN_EAXRIGHT", 1);
    InitControl(m_btnEAXLeft, "BTN_EAXLEFT", 1);
    InitControl(m_lblTitle, "LBL_TITLE", 1);
    InitControl(m_lbDescription, "LB_DESC", 1);
    InitControl(m_btnBack, "BTN_BACK", 1);
    InitControl(m_btnDefault, "BTN_DEFAULT", 1);
    InitControl(m_btnCancel, "BTN_CANCEL", 1);
    InitControl(m_alblBar[0], "LBL_BAR1", 1);
    InitControl(m_alblBar[1], "LBL_BAR2", 1);
    InitControl(m_alblBar[2], "LBL_BAR3", 1);
    InitControl(m_alblBar[3], "LBL_BAR4", 1);
    m_cbForceSoftware.m_nIndex = STRREF_FORCE_SOFTWARE_DESCRIPTION;
    m_btnEAX.m_nIndex = STRREF_EAX_DESCRIPTION;
    m_lbDescription.m_nSelectSound = CSWGuiManager::GUISOUND_SELECTION;
    StopLoadFromLayout();
    CSWGuiLabel* pProtoLabel = (CSWGuiLabel*)m_lbDescription.m_pProtoItem;
    m_lblDescription.Initialize(pProtoLabel->m_cExtent, pProtoLabel, 1.0f);
    m_cbForceSoftware.AddEvent(
        39, this, (CSWGuiHandler)&CSWGuiOptionsSoundAdvanced::OnForceSoftware);
    m_cbForceSoftware.AddEvent(
        45, this, (CSWGuiHandler)&CSWGuiOptionsSoundAdvanced::OnForceSoftware);
    m_cbForceSoftware.AddEvent(
        0, this, (CSWGuiHandler)&CSWGuiOptionsSoundAdvanced::OnEnterOption);
    m_btnEAXLeft.AddEvent(
        39, this, (CSWGuiHandler)&CSWGuiOptionsSoundAdvanced::OnEAXLeft);
    m_btnEAXLeft.AddEvent(
        45, this, (CSWGuiHandler)&CSWGuiOptionsSoundAdvanced::OnEAXLeft);
    m_btnEAXLeft.AddEvent(
        0, this, (CSWGuiHandler)&CSWGuiOptionsSoundAdvanced::OnEnterOption);
    m_btnEAXRight.AddEvent(
        39, this, (CSWGuiHandler)&CSWGuiOptionsSoundAdvanced::OnEAXRight);
    m_btnEAXRight.AddEvent(
        45, this, (CSWGuiHandler)&CSWGuiOptionsSoundAdvanced::OnEAXRight);
    m_btnEAXRight.AddEvent(
        0, this, (CSWGuiHandler)&CSWGuiOptionsSoundAdvanced::OnEnterOption);
    m_btnEAX.AddEvent(
        0, this, (CSWGuiHandler)&CSWGuiOptionsSoundAdvanced::OnEnterOption);
    SetUpOptions();
    CSWGuiButton* pBack = &m_btnBack;
    pBack->m_bFocusable = 0;
    pBack->AddEvent(GUIEVENT_ACTIVATE, this, (CSWGuiHandler)&CSWGuiPanel::OnBButtonPressed);
    CSWGuiButton* pDefault = &m_btnDefault;
    pDefault->m_bFocusable = 0;
    pDefault->AddEvent(39, this,
                       (CSWGuiHandler)&CSWGuiOptionsSoundAdvanced::OnDefault);
    CSWGuiButton* pCancel = &m_btnCancel;
    pCancel->m_bFocusable = 0;
    pCancel->AddEvent(
        39, this, (CSWGuiHandler)&CSWGuiOptionsSoundAdvanced::OnDiscardChanges);
    SetActiveControl(&m_cbForceSoftware, 0);
    m_bDiscard = 0;
}
// PC 0x00751E90
CSWGuiOptionsMouse::CSWGuiOptionsMouse(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    StartLoadFromLayout("optmouse_p");
    InitControl(m_sldMouseSen, "SLI_MOUSESEN", 1);
    InitControl(m_lblMouseSen, "LBL_MOUSESEN", 1);
    InitControl(m_cbRevButtons, "CB_REVBUTTONS", 1);
    InitControl(m_lblTitle, "LBL_TITLE", 1);
    InitControl(m_lbDescription, "LB_DESC", 1);
    InitControl(m_btnBack, "BTN_BACK", 1);
    InitControl(m_btnDefault, "BTN_DEFAULT", 1);
    InitControl(m_alblBar[0], "LBL_BAR1", 1);
    InitControl(m_alblBar[1], "LBL_BAR2", 1);
    InitControl(m_alblBar[2], "LBL_BAR3", 1);
    InitControl(m_alblBar[3], "LBL_BAR4", 1);
    m_lbDescription.m_nSelectSound = CSWGuiManager::GUISOUND_SELECTION;
    StopLoadFromLayout();
    CSWGuiLabel* pProtoLabel = (CSWGuiLabel*)m_lbDescription.m_pProtoItem;
    m_lblDescription.Initialize(pProtoLabel->m_cExtent, pProtoLabel, 1.0f);
    m_cbRevButtons.AddEvent(
        39, this, (CSWGuiHandler)&CSWGuiOptionsMouse::OnRevMouseButtons);
    m_cbRevButtons.AddEvent(
        45, this, (CSWGuiHandler)&CSWGuiOptionsMouse::OnRevMouseButtons);
    m_cbRevButtons.AddEvent(
        0, this, (CSWGuiHandler)&CSWGuiOptionsMouse::OnEnterRevMouseButtons);
    m_sldMouseSen.AddEvent(
        39, this, (CSWGuiHandler)&CSWGuiOptionsMouse::OnMoveMouseSenSlider);
    m_sldMouseSen.AddEvent(0, this,
                           (CSWGuiHandler)&CSWGuiOptionsMouse::OnEnterSlider);
    m_sldMouseSen.AddEvent(1, this,
                           (CSWGuiHandler)&CSWGuiOptionsSound::OnExitSlider);
    m_sldMouseSen.AddEvent(
        47, this, (CSWGuiHandler)&CSWGuiOptionsMouse::OnMoveMouseSenSlider);
    m_sldMouseSen.AddEvent(
        48, this, (CSWGuiHandler)&CSWGuiOptionsMouse::OnMoveMouseSenSlider);
    m_sldMouseSen.AddEvent(
        63, this, (CSWGuiHandler)&CSWGuiOptionsMouse::OnMoveMouseSenSlider);
    m_sldMouseSen.AddEvent(
        64, this, (CSWGuiHandler)&CSWGuiOptionsMouse::OnMoveMouseSenSlider);
    m_sldMouseSen.m_nIndex = STRREF_MOUSE_SENSITIVITY_DESCRIPTION;
    m_lblMouseSen.m_nIndex = STRREF_MOUSE_SENSITIVITY_DESCRIPTION;
    m_cbRevButtons.m_nIndex = STRREF_REVERSE_BUTTONS_DESCRIPTION;
    CSWGuiButton* pBack = &m_btnBack;
    pBack->m_bFocusable = 0;
    pBack->AddEvent(GUIEVENT_ACTIVATE, this, (CSWGuiHandler)&CSWGuiPanel::OnBButtonPressed);
    CSWGuiButton* pDefault = &m_btnDefault;
    pDefault->m_bFocusable = 0;
    pDefault->AddEvent(GUIEVENT_ACTIVATE, this, (CSWGuiHandler)&CSWGuiOptionsMouse::OnDefault);
    SetActiveControl(&m_sldMouseSen, 0);
    SetUpOptions();
}
// PC 0x0074FBE0
void CSWGuiOptionsFeedback::OnDefault(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        CClientOptions* pOptions =
            g_pAppManager->m_pClientExoApp->GetClientOptions();
        pOptions->SetDefaultFeedbackOptions();
        SetUpOptions();
    }
}
// PC 0x0074FC10
void CSWGuiOptionsFeedback::OnEnterSlot(CSWGuiControl* pControl)
{
#ifdef _XBOX
    int nSlot = -1;
    int i = 0;
    CSWGuiOptionsCheckbox* pButton = m_aCheckboxes;
    do
    {
        if (nSlot != -1)
        {
            break;
        }
        if (pControl == pButton)
        {
            nSlot = i;
        }
        i++;
        pButton++;
    } while (i < 7);
    switch (nSlot)
    {
    case 0:
        SetDescription(42286);
        break;
    case 1:
        SetDescription(42287);
        break;
    case 2:
        SetDescription(42288);
        break;
    case 3:
        SetDescription(42289);
        break;
    case 4:
        SetDescription(42290);
        break;
    case 5:
        SetDescription(42291);
        break;
    case 6:
        SetDescription(42451);
        break;
    }
#else
    SetDescription(pControl->m_nIndex);
#endif
}

#ifndef _XBOX
void CSWGuiOptionsGraphics::OnEnterSlot(CSWGuiControl* pControl)  // PC 0x0074FC20
{
    SetDescription(pControl->m_nIndex);
}
// PC 0x0074FC30
void CSWGuiOptionsGraphics::OnEnterSlider(CSWGuiControl* pControl)
{
    if (pControl == &m_sldGamma)
    {
        SetDescription(42310);
    }
    CSWGuiImageParams* pParams = &pControl->AsSlider()->m_cImage.m_cParams;
    pParams->m_bPulsing = 1;
}
// PC 0x0074FC70
void CSWGuiOptionsGraphics::OnDefault(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        CClientOptions* pOptions =
            g_pAppManager->m_pClientExoApp->GetClientOptions();
        pOptions->SetDefaultGraphicsOptions();
        SetUpOptions();
    }
}
// PC 0x0074FCA0
void CSWGuiOptionsGraphicsAdvanced::HandleInputEvent(int nEvent, int nState)
{
    if (nState != 0)
    {
        switch (nEvent)
        {
        case 45:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            break;
        case 40:
        case 46:
            if (m_bDiscard != 0)
            {
                m_bDiscard = 0;
            }
            else
            {
                SetNewOptions();
                if (m_bChanged != 0)
                {
                    CExoIni cIni;
                    CExoString sValue;
                    {
                        int bRead = cIni.ReadIniEntry(
                            sValue, ".\\swkotor2.ini", "Graphics Options",
                            "Disable Vertex Buffer Objects");
                        if (bRead != 0 && sValue.AsINT() == 1)
                        {
                            AurDisableVertexBufferObjects();
                        }
                    }
                    CClientExoApp* pApp = g_pAppManager->m_pClientExoApp;
                    pApp->GetClientOptions()->SetAntiAlias(m_nAntiAlias);
                    ReInitAurora(g_nScreenWidth, g_nScreenHeight,
                                 g_nBitsPerPixel, g_pAurora->FullScreen(), 1);
                    {
                        int bRead = cIni.ReadIniEntry(sValue, ".\\swkotor2.ini",
                                                      "Graphics Options",
                                                      "Disable Write-Only VBO");
                        if (bRead != 0)
                        {
                            if (sValue.AsINT() == 1)
                            {
                                AurDisableVertexBufferObjectsWriteOnly();
                            }
                            else
                            {
                                AurEnableVertexBufferObjectsWriteOnly();
                            }
                        }
                    }
                    m_bChanged = 0;
                }
            }
            m_bChanged = 0;
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            m_pManager->PopModalPanel();
            m_bDeferredRemoval = 0;
            m_nExitState = EXITSTATE_DELETE;
            m_pGraphics->SetUpOptions();
            break;
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}
// PC 0x0074FF50
void CSWGuiOptionsGraphicsAdvanced::OnEnterSlot(CSWGuiControl* pControl)
{
    SetDescription(pControl->m_nIndex);
}
// PC 0x0074FF60
void CSWGuiOptionsGraphicsAdvanced::OnDefault(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        CClientOptions* pOptions =
            g_pAppManager->m_pClientExoApp->GetClientOptions();
        pOptions->SetDefaultAdvancedGraphicsOptions();
        SetUpOptions();
    }
}
// PC 0x0074FF90
void CSWGuiOptionsGraphicsAdvanced::OnTexQualLeft(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        if (m_nTexQual > 0)
        {
            m_nTexQual = m_nTexQual - 1;
            SetTexQualText();
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
        }
    }
}
// PC 0x0074FFD0
void CSWGuiOptionsGraphicsAdvanced::OnTexQualRight(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        if (m_nTexQual <= m_nMaxTexQual)
        {
            m_nTexQual = m_nTexQual + 1;
            SetTexQualText();
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
        }
    }
}
// PC 0x00750010
void CSWGuiOptionsGraphicsAdvanced::OnAntiAliasLeft(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        if (m_nAntiAlias > 0)
        {
            int bFound = 0;
            if (m_nAntiAlias >= 8)
            {
                m_nAntiAlias = 6;
                if (AurQueryAntiAliasingMode(6) != 0)
                {
                    bFound = 1;
                }
            }
            if (m_nAntiAlias >= 6 && !bFound)
            {
                m_nAntiAlias = 4;
                if (AurQueryAntiAliasingMode(4) != 0)
                {
                    bFound = 1;
                }
            }
            if (m_nAntiAlias >= 4 && !bFound)
            {
                m_nAntiAlias = 2;
                if (AurQueryAntiAliasingMode(2) != 0)
                {
                    bFound = 1;
                }
            }
            if (!bFound)
            {
                m_nAntiAlias = 0;
            }
            SetAntiAliasText();
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
        }
    }
}
// PC 0x007500D0
void CSWGuiOptionsGraphicsAdvanced::OnAntiAliasRight(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        if (m_nAntiAlias != 8)
        {
            int nAntiAlias = m_nAntiAlias;
            int bFound = 0;
            if (nAntiAlias <= 0)
            {
                nAntiAlias = 2;
                if (AurQueryAntiAliasingMode(nAntiAlias) != 0)
                {
                    bFound = 1;
                }
            }
            if (nAntiAlias <= 2 && !bFound)
            {
                nAntiAlias = 4;
                if (AurQueryAntiAliasingMode(nAntiAlias) != 0)
                {
                    bFound = 1;
                }
            }
            if (nAntiAlias <= 4 && !bFound)
            {
                nAntiAlias = 6;
                if (AurQueryAntiAliasingMode(nAntiAlias) != 0)
                {
                    bFound = 1;
                }
            }
            if (nAntiAlias <= 6 && !bFound)
            {
                nAntiAlias = 8;
                if (AurQueryAntiAliasingMode(nAntiAlias) != 0)
                {
                    bFound = 1;
                }
            }
            if (bFound)
            {
                m_nAntiAlias = nAntiAlias;
            }
            SetAntiAliasText();
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
        }
    }
}
// PC 0x00750170
void CSWGuiOptionsGraphicsAdvanced::OnAnisotropyLeft(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        if (m_nAnisotropy > 0)
        {
            int nAnisotropy = m_nAnisotropy / 2;
            m_nAnisotropy = nAnisotropy;
            if (nAnisotropy <= 0)
            {
                m_nAnisotropy = 1;
            }
            SetAnisotropyText();
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
        }
    }
}
// PC 0x007501C0
void CSWGuiOptionsGraphicsAdvanced::OnAnisotropyRight(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        int nAnisotropy = m_nAnisotropy;
        if (nAnisotropy < AurQueryMaximumAnisotropy())
        {
            m_nAnisotropy = 2 * nAnisotropy;
            SetAnisotropyText();
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
        }
    }
}
#endif
// PC 0x00752F60
void CSWGuiOptionsMain::OnFeedbackOpt(CSWGuiControl* pControl)
{
    int bSelected = pControl->m_bSelected;
    if (bSelected != 0)
    {
        CSWGuiOptionsFeedback* pPanel = new CSWGuiOptionsFeedback(GetManager());
        m_pManager->AddPanel(pPanel, CSWGuiManager::PANEL_MODE_MODAL, 1);
    }
}

#ifndef _XBOX
void CSWGuiOptionsFeedback::SetActiveButton(int nOption)
{
    if (nOption >= 0 && nOption < OPTION_COUNT)
    {
        m_lbOptions.SetActiveControl(m_lbOptions.GetControl(nOption), 0);
    }
}
#endif

#ifndef _XBOX
CSWGuiOptionsGraphics::CSWGuiOptionsGraphics(CSWGuiManager* pManager, int bInGame)  // PC 0x00752FE0
    : CSWGuiPanel(pManager)
{
    StartLoadFromLayout("optgraphics_p");
    InitControl(m_sldGamma, "SLI_GAMMA", 1);
    InitControl(m_lblTitle, "LBL_TITLE", 1);
    InitControl(m_lbDescription, "LB_DESC", 1);
    InitControl(m_lblGamma, "LBL_GAMMA", 1);
    InitControl(m_btnResolution, "BTN_RESOLUTION", 1);
    InitControl(m_btnResolutionLeft, "BTN_RESOLUTIONLEFT", 1);
    InitControl(m_btnResolutionRight, "BTN_RESOLUTIONRIGHT", 1);
    InitControl(m_cbShadows, "CB_SHADOWS", 1);
    InitControl(m_cbGrass, "CB_GRASS", 1);
    InitControl(m_btnAdvanced, "BTN_ADVANCED", 1);
    m_sldGamma.m_nIndex = 42310;
    m_lblGamma.m_nIndex = 42310;
    m_btnResolution.m_nIndex = 48707;
    m_btnResolutionLeft.m_nIndex = 48707;
    m_btnResolutionRight.m_nIndex = 48707;
    m_cbShadows.m_nIndex = 48708;
    m_cbGrass.m_nIndex = 48709;
    m_btnAdvanced.m_nIndex = 48710;
    m_lbDescription.m_nSelectSound = CSWGuiManager::GUISOUND_SELECTION;
    InitControl(m_btnBack, "BTN_BACK", 1);
    InitControl(m_btnDefault, "BTN_DEFAULT", 1);
    InitControl(m_alblBar[0], "LBL_BAR1", 1);
    InitControl(m_alblBar[1], "LBL_BAR2", 1);
    InitControl(m_alblBar[2], "LBL_BAR3", 1);
    InitControl(m_alblBar[3], "LBL_BAR4", 1);
    StopLoadFromLayout();
    CSWGuiLabel* pProtoLabel = (CSWGuiLabel*)m_lbDescription.m_pProtoItem;
    m_lblDescription.Initialize(pProtoLabel->m_cExtent, pProtoLabel, 1.0f);
    m_sldGamma.SetCurValue(57);
    m_sldGamma.AddEvent(0, this,
                        (CSWGuiHandler)&CSWGuiOptionsGraphics::OnEnterSlider);
    m_sldGamma.AddEvent(1, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnExitSlider);
    m_sldGamma.AddEvent(
        47, this, (CSWGuiHandler)&CSWGuiOptionsGraphics::OnMoveGammaSlider);
    m_sldGamma.AddEvent(
        48, this, (CSWGuiHandler)&CSWGuiOptionsGraphics::OnMoveGammaSlider);
    m_sldGamma.AddEvent(
        63, this, (CSWGuiHandler)&CSWGuiOptionsGraphics::OnMoveGammaSlider);
    m_sldGamma.AddEvent(
        64, this, (CSWGuiHandler)&CSWGuiOptionsGraphics::OnMoveGammaSlider);
    m_sldGamma.AddEvent(42, this,
                        (CSWGuiHandler)&CSWGuiOptionsGraphics::OnDefault);
    m_sldGamma.AddEvent(
        39, this, (CSWGuiHandler)&CSWGuiOptionsGraphics::OnMoveGammaSlider);
    CSWGuiButton* pResolution = &m_btnResolution;
    pResolution->AddEvent(
        0, this, (CSWGuiHandler)&CSWGuiOptionsGraphics::OnEnterSlot);
    m_cbShadows.AddEvent(0, this,
                         (CSWGuiHandler)&CSWGuiOptionsGraphics::OnEnterSlot);
    m_cbGrass.AddEvent(0, this,
                       (CSWGuiHandler)&CSWGuiOptionsGraphics::OnEnterSlot);
    pResolution->m_bFocusable = 0;
    pResolution->AddEvent(
        39, this, (CSWGuiHandler)&CSWGuiOptionsGraphics::OnResolution);
    CSWGuiOptionsCheckbox* pShadows = &m_cbShadows;
    pShadows->m_bFocusable = 0;
    pShadows->AddEvent(39, this,
                       (CSWGuiHandler)&CSWGuiOptionsGraphics::OnShadows);
    CSWGuiOptionsCheckbox* pGrass = &m_cbGrass;
    pGrass->m_bFocusable = 0;
    pGrass->AddEvent(39, this,
                     (CSWGuiHandler)&CSWGuiOptionsGraphics::OnGrass);
    SetUpOptions();
    CSWGuiButton* pBack = &m_btnBack;
    pBack->m_bFocusable = 0;
    pBack->AddEvent(GUIEVENT_ACTIVATE, this, (CSWGuiHandler)&CSWGuiPanel::OnBButtonPressed);
    CSWGuiButton* pDefault = &m_btnDefault;
    pDefault->m_bFocusable = 0;
    pDefault->AddEvent(39, this,
                       (CSWGuiHandler)&CSWGuiOptionsGraphics::OnDefault);
    CSWGuiButton* pAdvanced = &m_btnAdvanced;
    pAdvanced->m_bFocusable = 0;
    pAdvanced->AddEvent(39, this,
                        (CSWGuiHandler)&CSWGuiOptionsGraphics::OnAdvanced);
    SetActiveControl(&m_sldGamma, 0);
    m_nUnknown18BC = 0;
    m_bInGame = bInGame;
    if (bInGame)
    {
        m_btnResolution.m_cBorder.m_cParams.m_vColor = CGuiInGame::DISABLED_TEXT;
        m_btnResolution.m_cText.m_cParams.SetDefaultColor(CGuiInGame::COLOR_NEW_GREEN);
        m_btnResolution.m_cHilite.m_cParams.m_vColor = CGuiInGame::DISABLED_TEXT;
    }
}
#endif
// PC 0x00753970
CSWGuiOptionsSound::CSWGuiOptionsSound(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
#ifdef _XBOX
    StartLoadFromLayout("optgraphics");
    InitControl(m_sldGamma, "SLI_GAMMA", 1);
    InitControl(m_sldMusic, "SLI_MUSIC", 1);
    InitControl(m_sldVoice, "SLI_VO", 1);
    InitControl(m_sldEffects, "SLI_FX", 1);
    InitControl(m_sldMovie, "SLI_MOVIE", 1);
    InitControl(m_lblGamma, "LBL_GAMMA", 1);
    InitControl(m_lblMusic, "LBL_MUSIC", 1);
    InitControl(m_lblVoice, "LBL_VO", 1);
    InitControl(m_lblEffects, "LBL_FX", 1);
    InitControl(m_lblMovie, "LBL_MOVIE", 1);
    InitControl(m_lblB, "LBL_B", 1);
    InitControl(m_lblBack, "LBL_BACK", 1);
    InitControl(m_lblTitle, "LBL_TITLE", 1);
    InitControl(m_lblY, "LBL_Y", 1);
    InitControl(m_lblDefault, "LBL_DEFAULT", 1);
    InitControl(m_lblRAnalog, "LBL_RANALOG", 1);
    InitControl(m_lbDescription, "LB_DESC", 1);
    m_lbDescription.m_nSelectSound = CSWGuiManager::GUISOUND_SELECTION;
    StopLoadFromLayout();
    CSWGuiLabel* pProtoLabel = (CSWGuiLabel*)m_lbDescription.m_pProtoItem;
    m_lblDescription.Initialize(pProtoLabel->m_cExtent, pProtoLabel, 1.0f);
    m_sldGamma.SetCurValue(50);
    m_sldMusic.SetCurValue(50);
    m_sldVoice.SetCurValue(50);
    m_sldEffects.SetCurValue(50);
    m_sldMovie.SetCurValue(50);
    m_sldGamma.AddEvent(0, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnEnterSlider);
    m_sldMusic.AddEvent(0, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnEnterSlider);
    m_sldVoice.AddEvent(0, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnEnterSlider);
    m_sldEffects.AddEvent(0, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnEnterSlider);
    m_sldMovie.AddEvent(0, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnEnterSlider);
    m_sldGamma.AddEvent(1, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnExitSlider);
    m_sldMusic.AddEvent(1, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnExitSlider);
    m_sldVoice.AddEvent(1, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnExitSlider);
    m_sldEffects.AddEvent(1, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnExitSlider);
    m_sldMovie.AddEvent(1, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnExitSlider);
    m_sldGamma.AddEvent(47, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnMoveGammaSlider);
    m_sldMusic.AddEvent(47, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnMoveVolumeSlider);
    m_sldVoice.AddEvent(47, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnMoveVolumeSlider);
    m_sldEffects.AddEvent(47, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnMoveVolumeSlider);
    m_sldMovie.AddEvent(47, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnMoveVolumeSlider);
    m_sldGamma.AddEvent(48, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnMoveGammaSlider);
    m_sldMusic.AddEvent(48, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnMoveVolumeSlider);
    m_sldVoice.AddEvent(48, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnMoveVolumeSlider);
    m_sldEffects.AddEvent(48, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnMoveVolumeSlider);
    m_sldMovie.AddEvent(48, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnMoveVolumeSlider);
    m_sldGamma.AddEvent(63, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnMoveGammaSlider);
    m_sldMusic.AddEvent(63, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnMoveVolumeSlider);
    m_sldVoice.AddEvent(63, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnMoveVolumeSlider);
    m_sldEffects.AddEvent(63, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnMoveVolumeSlider);
    m_sldMovie.AddEvent(63, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnMoveVolumeSlider);
    m_sldGamma.AddEvent(64, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnMoveGammaSlider);
    m_sldMusic.AddEvent(64, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnMoveVolumeSlider);
    m_sldVoice.AddEvent(64, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnMoveVolumeSlider);
    m_sldEffects.AddEvent(64, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnMoveVolumeSlider);
    m_sldMovie.AddEvent(64, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnMoveVolumeSlider);
    m_sldGamma.AddEvent(42, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnDefault);
    m_sldMusic.AddEvent(42, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnDefault);
    m_sldVoice.AddEvent(42, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnDefault);
    m_sldEffects.AddEvent(42, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnDefault);
    m_sldMovie.AddEvent(42, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnDefault);
    m_sldGamma.SetCurValue(
        g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nBrightness);
    m_sldMusic.SetCurValue(
        g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nMusicVolume);
    m_sldVoice.SetCurValue(
        g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nVoiceVolume);
    m_sldEffects.SetCurValue(
        g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nSoundEffectsVolume);
    m_sldMovie.SetCurValue(
        g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nMovieVolume);
    OnMoveGammaSlider(&m_sldGamma);
    OnMoveVolumeSlider(&m_sldMusic);
    OnMoveVolumeSlider(&m_sldVoice);
    OnMoveVolumeSlider(&m_sldEffects);
    OnMoveVolumeSlider(&m_sldMovie);
    SetActiveControl(&m_sldGamma, 0);
#else
    StartLoadFromLayout("optsound_p");
    InitControl(m_sldMusic, "SLI_MUSIC", 1);
    InitControl(m_sldVoice, "SLI_VO", 1);
    InitControl(m_sldEffects, "SLI_FX", 1);
    InitControl(m_sldMovie, "SLI_MOVIE", 1);
    InitControl(m_lblMusic, "LBL_MUSIC", 1);
    InitControl(m_lblVoice, "LBL_VO", 1);
    InitControl(m_lblEffects, "LBL_FX", 1);
    InitControl(m_lblMovie, "LBL_MOVIE", 1);
    InitControl(m_btnAdvanced, "BTN_ADVANCED", 1);
    InitControl(m_lblTitle, "LBL_TITLE", 1);
    InitControl(m_lbDescription, "LB_DESC", 1);
    InitControl(m_alblBar[0], "LBL_BAR1", 1);
    InitControl(m_alblBar[1], "LBL_BAR2", 1);
    InitControl(m_alblBar[2], "LBL_BAR3", 1);
    InitControl(m_alblBar[3], "LBL_BAR4", 1);
    m_sldMusic.m_nIndex = STRREF_MUSIC_VOLUME_DESCRIPTION;
    m_sldVoice.m_nIndex = STRREF_VOICE_VOLUME_DESCRIPTION;
    m_sldEffects.m_nIndex = STRREF_EFFECTS_VOLUME_DESCRIPTION;
    m_sldMovie.m_nIndex = STRREF_MOVIE_VOLUME_DESCRIPTION;
    m_btnAdvanced.m_nIndex = STRREF_ADVANCED_SOUND_DESCRIPTION;
    m_lbDescription.m_nSelectSound = CSWGuiManager::GUISOUND_SELECTION;
    InitControl(m_btnBack, "BTN_BACK", 1);
    InitControl(m_btnDefault, "BTN_DEFAULT", 1);
    StopLoadFromLayout();
    CSWGuiLabel* pProtoLabel = (CSWGuiLabel*)m_lbDescription.m_pProtoItem;
    m_lblDescription.Initialize(pProtoLabel->m_cExtent, pProtoLabel, 1.0f);
    m_sldMusic.SetCurValue(50);
    m_sldVoice.SetCurValue(50);
    m_sldEffects.SetCurValue(50);
    m_sldMovie.SetCurValue(50);
    m_sldMusic.AddEvent(0, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnEnterSlider);
    m_sldVoice.AddEvent(0, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnEnterSlider);
    m_sldEffects.AddEvent(0, this,
                          (CSWGuiHandler)&CSWGuiOptionsSound::OnEnterSlider);
    m_sldMovie.AddEvent(0, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnEnterSlider);
    m_sldMusic.AddEvent(1, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnExitSlider);
    m_sldVoice.AddEvent(1, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnExitSlider);
    m_sldEffects.AddEvent(1, this,
                          (CSWGuiHandler)&CSWGuiOptionsSound::OnExitSlider);
    m_sldMovie.AddEvent(1, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnExitSlider);
    m_sldMusic.AddEvent(47, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnMoveVolumeSlider);
    m_sldVoice.AddEvent(47, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnMoveVolumeSlider);
    m_sldEffects.AddEvent(
        47, this, (CSWGuiHandler)&CSWGuiOptionsSound::OnMoveVolumeSlider);
    m_sldMovie.AddEvent(47, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnMoveVolumeSlider);
    m_sldMusic.AddEvent(48, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnMoveVolumeSlider);
    m_sldVoice.AddEvent(48, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnMoveVolumeSlider);
    m_sldEffects.AddEvent(
        48, this, (CSWGuiHandler)&CSWGuiOptionsSound::OnMoveVolumeSlider);
    m_sldMovie.AddEvent(48, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnMoveVolumeSlider);
    m_sldMusic.AddEvent(63, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnMoveVolumeSlider);
    m_sldVoice.AddEvent(63, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnMoveVolumeSlider);
    m_sldEffects.AddEvent(
        63, this, (CSWGuiHandler)&CSWGuiOptionsSound::OnMoveVolumeSlider);
    m_sldMovie.AddEvent(63, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnMoveVolumeSlider);
    m_sldMusic.AddEvent(64, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnMoveVolumeSlider);
    m_sldVoice.AddEvent(64, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnMoveVolumeSlider);
    m_sldEffects.AddEvent(
        64, this, (CSWGuiHandler)&CSWGuiOptionsSound::OnMoveVolumeSlider);
    m_sldMovie.AddEvent(64, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnMoveVolumeSlider);
    m_sldMusic.AddEvent(42, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnDefault);
    m_sldVoice.AddEvent(42, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnDefault);
    m_sldEffects.AddEvent(42, this,
                          (CSWGuiHandler)&CSWGuiOptionsSound::OnDefault);
    m_sldMovie.AddEvent(42, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnDefault);
    m_sldMusic.AddEvent(39, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnMoveVolumeSlider);
    m_sldVoice.AddEvent(39, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnMoveVolumeSlider);
    m_sldEffects.AddEvent(
        39, this, (CSWGuiHandler)&CSWGuiOptionsSound::OnMoveVolumeSlider);
    m_sldMovie.AddEvent(39, this,
                        (CSWGuiHandler)&CSWGuiOptionsSound::OnMoveVolumeSlider);
    m_btnAdvanced.AddEvent(39, this,
                           (CSWGuiHandler)&CSWGuiOptionsSound::OnAdvanced);
    SetUpOptions();
    CSWGuiButton* pBack = &m_btnBack;
    pBack->m_bFocusable = 0;
    pBack->AddEvent(GUIEVENT_ACTIVATE, this, (CSWGuiHandler)&CSWGuiPanel::OnBButtonPressed);
    CSWGuiButton* pDefault = &m_btnDefault;
    pDefault->m_bFocusable = 0;
    pDefault->AddEvent(GUIEVENT_ACTIVATE, this, (CSWGuiHandler)&CSWGuiOptionsSound::OnDefault);
    SetActiveControl(&m_sldMusic, 0);
#endif
}

#ifndef _XBOX
void CSWGuiOptionsMain::OnGraphicsOpt(CSWGuiControl* pControl)  // PC 0x007542E0
{
    int bSelected = pControl->m_bSelected;
    if (bSelected != 0)
    {
        CSWGuiOptionsGraphics* pPanel = new CSWGuiOptionsGraphics(GetManager(), 0);
        m_pManager->AddPanel(pPanel, CSWGuiManager::PANEL_MODE_MODAL, 1);
    }
}
#endif
// PC 0x007538F0
void CSWGuiOptionsMain::OnSoundOpt(CSWGuiControl* pControl)
{
    int bSelected = pControl->m_bSelected;
    if (bSelected != 0)
    {
        CSWGuiOptionsSound* pPanel = new CSWGuiOptionsSound(GetManager());
        m_pManager->AddPanel(pPanel, CSWGuiManager::PANEL_MODE_MODAL, 1);
    }
}

#ifdef _XBOX

void CSWGuiOptionsMain::OnDownload(CSWGuiControl* pControl)
{
    LAUNCH_DATA cLaunchData;

    g_pAppManager->m_pClientExoApp->BusyWaitMenuMusic();
    memset(&cLaunchData, 0, sizeof(cLaunchData));
    XLaunchNewImage("d:\\downloader.xbe", &cLaunchData);
}

CSWGuiOptionsMain::CSWGuiOptionsMain(CSWGuiManager* pManager)
    : CSWGuiPanel(pManager)
{
    StartLoadFromLayout("optionsmain");
    InitControl(m_btnOptions[0], "BTN_GAMEPLAY", 1);
    InitControl(m_btnOptions[1], "BTN_FEEDBACK", 1);
    InitControl(m_btnOptions[2], "BTN_AUTOPAUSE", 1);
    InitControl(m_btnOptions[3], "BTN_GRAPHICSSOUND", 1);
    InitControl(m_btnOptions[4], "BTN_DOWNLOAD", 1);
    InitControl(m_lblA, "LBL_A", 1);
    InitControl(m_lblSelect, "LBL_SELECT", 1);
    InitControl(m_lblB, "LBL_B", 1);
    InitControl(m_lblBack, "LBL_BACK", 1);
    InitControl(m_lblTitle, "LBL_TITLE", 1);
    InitControl(m_lblRAnalog, "LBL_RANALOG", 1);
    InitControl(m_lbDescription, "LB_DESC", 1);
    m_lbDescription.m_nSelectSound = CSWGuiManager::GUISOUND_SELECTION;
    StopLoadFromLayout();
    CSWGuiLabel* pProtoLabel = (CSWGuiLabel*)m_lbDescription.m_pProtoItem;
    m_lblDescription.Initialize(pProtoLabel->m_cExtent, pProtoLabel, 1.0f);
    m_btnOptions[0].AddEvent(39, this,
                             (CSWGuiHandler)&CSWGuiOptionsMain::OnGameplayOpt);
    m_btnOptions[1].AddEvent(39, this,
                             (CSWGuiHandler)&CSWGuiOptionsMain::OnFeedbackOpt);
    m_btnOptions[2].AddEvent(39, this,
                             (CSWGuiHandler)&CSWGuiOptionsMain::OnAutopauseOpt);
    m_btnOptions[3].AddEvent(39, this,
                             (CSWGuiHandler)&CSWGuiOptionsMain::OnSoundOpt);
    m_btnOptions[4].AddEvent(39, this,
                             (CSWGuiHandler)&CSWGuiOptionsMain::OnDownload);
    m_btnOptions[0].AddEvent(45, this,
                             (CSWGuiHandler)&CSWGuiOptionsMain::OnGameplayOpt);
    m_btnOptions[1].AddEvent(45, this,
                             (CSWGuiHandler)&CSWGuiOptionsMain::OnFeedbackOpt);
    m_btnOptions[2].AddEvent(45, this,
                             (CSWGuiHandler)&CSWGuiOptionsMain::OnAutopauseOpt);
    m_btnOptions[3].AddEvent(45, this,
                             (CSWGuiHandler)&CSWGuiOptionsMain::OnSoundOpt);
    m_btnOptions[4].AddEvent(45, this,
                             (CSWGuiHandler)&CSWGuiOptionsMain::OnDownload);
    for (int i = 0; i < NUM_OPTIONS; i++)
    {
        m_btnOptions[i].AddEvent(
            0, this, (CSWGuiHandler)&CSWGuiOptionsMain::OnEnterSlot);
        m_btnOptions[i].m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_BLUE;
        m_btnOptions[i].m_cHilite.m_cParams.m_vColor = CGuiInGame::COLOR_YELLOW;
        m_btnOptions[i].m_cHilite.m_cParams.SetPulsingAlpha(1, 1, 0);
    }
    CSWGuiPanel* pPanel = this; pPanel->SetActiveControl(&m_btnOptions[0], 0);
}
#else
CSWGuiOptionsMain::CSWGuiOptionsMain(CSWGuiManager* pManager)  // PC 0x00754360
    : CSWGuiPanel(pManager)
{
#ifdef _XBOX
    StartLoadFromLayout("optionsmain");
    InitControl(m_btnOptions[0], "BTN_GAMEPLAY", 1);
    InitControl(m_btnOptions[1], "BTN_FEEDBACK", 1);
    InitControl(m_btnOptions[2], "BTN_AUTOPAUSE", 1);
    InitControl(m_btnOptions[3], "BTN_GRAPHICSSOUND", 1);
    InitControl(m_btnOptions[4], "BTN_DOWNLOAD", 1);
    InitControl(m_lblA, "LBL_A", 1);
    InitControl(m_lblSelect, "LBL_SELECT", 1);
    InitControl(m_lblB, "LBL_B", 1);
    InitControl(m_lblBack, "LBL_BACK", 1);
    InitControl(m_lblTitle, "LBL_TITLE", 1);
    InitControl(m_lblRAnalog, "LBL_RANALOG", 1);
    InitControl(m_lbDescription, "LB_DESC", 1);
    m_lbDescription.m_nSelectSound = CSWGuiManager::GUISOUND_SELECTION;
    StopLoadFromLayout();
    CSWGuiLabel* pProtoLabel = (CSWGuiLabel*)m_lbDescription.m_pProtoItem;
    m_lblDescription.Initialize(pProtoLabel->m_cExtent, pProtoLabel, 1.0f);
    m_btnOptions[0].AddEvent(39, this,
                             (CSWGuiHandler)&CSWGuiOptionsMain::OnGameplayOpt);
    m_btnOptions[1].AddEvent(39, this,
                             (CSWGuiHandler)&CSWGuiOptionsMain::OnFeedbackOpt);
    m_btnOptions[2].AddEvent(39, this,
                             (CSWGuiHandler)&CSWGuiOptionsMain::OnAutopauseOpt);
    m_btnOptions[3].AddEvent(39, this,
                             (CSWGuiHandler)&CSWGuiOptionsMain::OnSoundOpt);
    m_btnOptions[4].AddEvent(39, this,
                             (CSWGuiHandler)&CSWGuiOptionsMain::OnDownloadOpt);
    m_btnOptions[0].AddEvent(45, this,
                             (CSWGuiHandler)&CSWGuiOptionsMain::OnGameplayOpt);
    m_btnOptions[1].AddEvent(45, this,
                             (CSWGuiHandler)&CSWGuiOptionsMain::OnFeedbackOpt);
    m_btnOptions[2].AddEvent(45, this,
                             (CSWGuiHandler)&CSWGuiOptionsMain::OnAutopauseOpt);
    m_btnOptions[3].AddEvent(45, this,
                             (CSWGuiHandler)&CSWGuiOptionsMain::OnSoundOpt);
    m_btnOptions[4].AddEvent(45, this,
                             (CSWGuiHandler)&CSWGuiOptionsMain::OnDownloadOpt);
    for (int i = 0; i < 5; i++)
    {
        m_btnOptions[i].AddEvent(
            0, this, (CSWGuiHandler)&CSWGuiOptionsMain::OnEnterSlot);
        m_btnOptions[i].m_cBorder.m_cParams.m_vColor = CGuiInGame::COLOR_BLUE;
        m_btnOptions[i].m_cHilite.m_cParams.m_vColor = CGuiInGame::COLOR_YELLOW;
        m_btnOptions[i].m_cHilite.m_cParams.SetPulsingAlpha(1, 1, 0);
    }
    CSWGuiPanel* pPanel = this;
    pPanel->SetActiveControl(&m_btnOptions[0], 0);
#else
    StartLoadFromLayout("optionsmain_p");
    InitControl(m_lblTitle, "LBL_TITLE", 1);
    InitControl(m_lbDescription, "LB_DESC", 1);
    InitControl(m_btnOptions[0], "BTN_GAMEPLAY", 1);
    InitControl(m_btnOptions[1], "BTN_FEEDBACK", 1);
    InitControl(m_btnOptions[2], "BTN_AUTOPAUSE", 1);
    InitControl(m_btnOptions[3], "BTN_GRAPHICS", 1);
    InitControl(m_btnOptions[4], "BTN_SOUND", 1);
    m_btnOptions[0].m_nIndex = STRREF_GAMEPLAY_DESCRIPTION;
    m_btnOptions[1].m_nIndex = STRREF_FEEDBACK_DESCRIPTION;
    m_btnOptions[2].m_nIndex = STRREF_AUTOPAUSE_DESCRIPTION;
    m_btnOptions[3].m_nIndex = STRREF_GRAPHICS_DESCRIPTION;
    m_btnOptions[4].m_nIndex = STRREF_SOUND_DESCRIPTION;
    InitControl(m_btnBack, "BTN_BACK", 1);
    InitControl(m_alblBar[0], "LBL_BAR1", 1);
    InitControl(m_alblBar[1], "LBL_BAR2", 1);
    InitControl(m_alblBar[2], "LBL_BAR3", 1);
    m_lbDescription.m_nSelectSound = CSWGuiManager::GUISOUND_SELECTION;
    StopLoadFromLayout();
    CSWGuiButton* pBack = &m_btnBack;
    pBack->AddEvent(GUIEVENT_ACTIVATE, this, (CSWGuiHandler)&CSWGuiPanel::OnBButtonPressed);
    pBack->m_bFocusable = 0;
    CSWGuiLabel* pProtoLabel = (CSWGuiLabel*)m_lbDescription.m_pProtoItem;
    m_lblDescription.Initialize(pProtoLabel->m_cExtent, pProtoLabel, 1.0f);
    m_btnOptions[0].AddEvent(39, this,
                             (CSWGuiHandler)&CSWGuiOptionsMain::OnGameplayOpt);
    m_btnOptions[1].AddEvent(39, this,
                             (CSWGuiHandler)&CSWGuiOptionsMain::OnFeedbackOpt);
    m_btnOptions[2].AddEvent(39, this,
                             (CSWGuiHandler)&CSWGuiOptionsMain::OnAutopauseOpt);
    m_btnOptions[3].AddEvent(39, this,
                             (CSWGuiHandler)&CSWGuiOptionsMain::OnGraphicsOpt);
    m_btnOptions[4].AddEvent(39, this,
                             (CSWGuiHandler)&CSWGuiOptionsMain::OnSoundOpt);
    m_btnOptions[0].AddEvent(45, this,
                             (CSWGuiHandler)&CSWGuiOptionsMain::OnGameplayOpt);
    m_btnOptions[1].AddEvent(45, this,
                             (CSWGuiHandler)&CSWGuiOptionsMain::OnFeedbackOpt);
    m_btnOptions[2].AddEvent(45, this,
                             (CSWGuiHandler)&CSWGuiOptionsMain::OnAutopauseOpt);
    m_btnOptions[3].AddEvent(45, this,
                             (CSWGuiHandler)&CSWGuiOptionsMain::OnGraphicsOpt);
    m_btnOptions[4].AddEvent(45, this,
                             (CSWGuiHandler)&CSWGuiOptionsMain::OnSoundOpt);
    for (int i = 0; i < 6; i++)
    {
        m_btnOptions[i].AddEvent(
            0, this, (CSWGuiHandler)&CSWGuiOptionsMain::OnEnterSlot);
        m_btnOptions[i].m_cHilite.m_cParams.SetPulsingAlpha(1, 1, 0);
    }
    CSWGuiPanel* pPanel = this;
    pPanel->SetActiveControl(&m_btnOptions[0], 0);
#endif
}
#endif
// PC 0x00759190
void CSWGuiOptionsMouse::HandleInputEvent(int nEvent, int nState)
{
    if (nState != 0)
    {
        switch (nEvent)
        {
        case 40:
        case 46:
            m_pManager->PlayGuiSound(CSWGuiManager::GUISOUND_CLICK);
            m_pManager->PopModalPanel();
            m_bDeferredRemoval = 0;
            m_nExitState = EXITSTATE_DELETE;
            break;
        case 45:
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
        case 57:
            m_lbDescription.HandleInputEvent(GUIEVENT_UP, 1);
            break;
        case 58:
            m_lbDescription.HandleInputEvent(GUIEVENT_DOWN, 1);
            break;
        }
    }
    return CSWGuiPanel::HandleInputEvent(nEvent, nState);
}

#ifdef _XBOX

void CSWGuiOptionsFeedback::OnRumble(CSWGuiControl* pControl)
{
    if (pControl->m_bSelected != 0)
    {
        int bRumble = (g_pAppManager->m_pClientExoApp->GetClientOptions()
                           ->m_bFloatingNumbers == 0);
        CClientOptions* pOptions =
            g_pAppManager->m_pClientExoApp->GetClientOptions();
        pOptions->m_bFloatingNumbers = bRumble;
    }
}

void CSWGuiOptionsSound::OnMoveGammaSlider(CSWGuiControl* pControl)
{
    int nValue = pControl->AsSlider()->m_nCurValue;
    g_pAppManager->GetClientExoApp()->GetClientOptions()->m_nBrightness = nValue;
    g_pAurora->ResetGammaRamp(
        g_pAppManager->GetClientExoApp()->GetClientOptions()->GetGammaSetting(), 0);
}
#endif
