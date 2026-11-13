#include "swgui.h"

static inline void SwguiTrace2();
static inline void SwguiTrace1();

int g_bDrawSafeRegion = 0;

CExoString CSWGuiManager::FULLSCREEN_BGS[5] = {
    "back", "store", "pazaak", "map", "comp"
};

unsigned long g_nGUIWidth = 800;
unsigned long g_nGUIHeight = 600;

char* g_asMouseCursorNames[] = {
    "", "gui_mp_defaultu", "gui_mp_defaultd", "gui_mp_walku",
    "gui_mp_walkd", "gui_mp_invalidu", "gui_mp_invalid", "gui_mp_bashu",
    "gui_mp_bashd", "gui_mp_bashup", "gui_mp_bashdp", "gui_mp_talku",
    "gui_mp_talkd", "gui_mp_notalku", "gui_mp_notalkd", "gui_mp_followu",
    "gui_mp_followd", "gui_mp_examineu", "gui_mp_examined", "gui_mp_noexamu",
    "gui_mp_noexamd", "gui_mp_transu", "gui_mp_transd", "gui_mp_dooru",
    "gui_mp_doord", "gui_mp_useu", "gui_mp_used", "gui_mp_useup",
    "gui_mp_usedp", "gui_mp_magicu", "gui_mp_magicd", "gui_mp_nomagicu",
    "gui_mp_nomagicd", "gui_mp_dismineu", "gui_mp_dismined", "gui_mp_dismineup",
    "gui_mp_disminedp", "gui_mp_recmineu", "gui_mp_recmined", "gui_mp_recmineup",
    "gui_mp_recminedp", "gui_mp_locku", "gui_mp_lockd", "gui_mp_doorup",
    "gui_mp_doordp", "gui_mp_selectu", "gui_mp_selectd", "gui_mp_createu",
    "gui_mp_created", "gui_mp_nocreatu", "gui_mp_nocreatd", "gui_mp_killu",
    "gui_mp_killd", "gui_mp_nokillu", "gui_mp_nokilld", "gui_mp_healu",
    "gui_mp_heald", "gui_mp_nohealu", "gui_mp_noheald", "gui_mp_pickupu",
    "gui_mp_pickupd", "gui_mp_arrun00", "gui_mp_arrun01", "gui_mp_arrun02",
    "gui_mp_arrun03", "gui_mp_arrun04", "gui_mp_arrun05", "gui_mp_arrun06",
    "gui_mp_arrun07", "gui_mp_arrun08", "gui_mp_arrun09", "gui_mp_arrun10",
    "gui_mp_arrun11", "gui_mp_arrun12", "gui_mp_arrun13", "gui_mp_arrun14",
    "gui_mp_arrun15", "gui_mp_arwalk00", "gui_mp_arwalk01", "gui_mp_arwalk02",
    "gui_mp_arwalk03", "gui_mp_arwalk04", "gui_mp_arwalk05", "gui_mp_arwalk06",
    "gui_mp_arwalk07", "gui_mp_arwalk08", "gui_mp_arwalk09", "gui_mp_arwalk10",
    "gui_mp_arwalk11", "gui_mp_arwalk12", "gui_mp_arwalk13", "gui_mp_arwalk14",
    "gui_mp_arwalk15",
};

Vector CSWGuiManager::DISABLED_TEXT(0.13f, 0.9f, 0.7f);
Vector CSWGuiManager::COLOR_YELLOW(0.98f, 1.0f, 0.0f);

void AurGUIStartLayer();
void AurGUIStopLayer();
int AurGUISetupViewport(int nX, int nY, int nWidth, int nHeight,
                        const Vector& vColor, bool bScissor, float fAlpha);
void AurGUICloseViewport();

#include "swguiingame.h"
#include "nwccreature.h"
#include "nwcanimbase.h"

#include "exobase.h"
#include "exoresman.h"
#include "exoinput.h"
#include "exobase.h"
#include "resgff.h"
#include "twodimarray.h"
#include "appmanager.h"
#include "clientexoapp.h"
#include "clientexoappinternal.h"
#include "clientoptions.h"
#include "aurora.h"
#include "aurcamera.h"
#include <string.h>

CExoString CSWGuiManager::GetScreenResolutionString(unsigned long nWidth,
                                                    unsigned long nHeight)
{
    if (nWidth == 1024)
    {
        if (nHeight == 768)
        {
            return CExoString("1024x768");
        }
    }
    else if (nWidth == 1280)
    {
        if (nHeight == 960)
        {
            return CExoString("1280x960");
        }
        if (nHeight == 1024)
        {
            return CExoString("1280x1024");
        }
    }
    else if (nWidth == 1600 && nHeight == 1200)
    {
        return CExoString("1600x1200");
    }
    return CExoString("800x600");
}


inline CAurCamera* CSWGuiScene::GetCamera()
{
    return m_pCamera;
}

static inline const Vector& SafeRegionWhite(Vector& vColor)
{
    vColor.m_fX = 1.0f;
    vColor.m_fY = 1.0f;
    vColor.m_fZ = 1.0f;
    return vColor;
}

static inline void SwguiTrace2()
{
    SwguiTrace1();
}

static inline void SwguiTrace1()
{
}
// PC 0x0040A0D0
int CSWGuiExtent::Load(CResGFF* pGFF, CResStruct& cStruct)
{
    CResStruct cExtent;
    int bSuccess;
    int nResult;

    nResult = pGFF->GetStructFromStruct(&cExtent, &cStruct, "EXTENT");
    if (nResult != 0)
    {
        m_nLeft = pGFF->ReadFieldINT(&cExtent, "LEFT", bSuccess, 0);
        m_nTop = pGFF->ReadFieldINT(&cExtent, "TOP", bSuccess, 0);
        m_nWidth = pGFF->ReadFieldINT(&cExtent, "WIDTH", bSuccess, 0);
        nResult = m_nHeight =
            pGFF->ReadFieldINT(&cExtent, "HEIGHT", bSuccess, 0);
    }
    return nResult;
}
// PC 0x0040A170
void CSWGuiPanel::HandleInputEvent(int nEvent, int nState)
{
    if (m_pActiveControl != NULL)
    {
        m_pActiveControl->HandleInputEvent(nEvent, nState);
    }
}
// PC 0x0040BE30
CSWGuiManager::CSWGuiManager()
    : m_lPanels(0),
      m_lModalPanels(0)
{
    m_nWidth = 0;
    m_nHeight = 0;
    m_nBackgroundPanels = 0;
    m_nLastKeyTime = 0;
    m_nLastDirection = 0;
    m_pSafeRegionPanel = NULL;
    m_pSafeRegionBorder = NULL;
#ifndef _XBOX
    m_pPCBackground = NULL;
    m_sBackground = "800x600";
#endif
#ifndef _XBOX
    m_sBackground = m_sBackground + FULLSCREEN_BGS[0];
#endif
#ifndef _XBOX
    m_pMouseOverControl = NULL;
#endif
#ifndef _XBOX
    m_pMouseCapture = NULL;
#endif
#ifndef _XBOX
    m_nMouseCaptureType = 0;
    m_bLeftMouseDown = 0;
    m_bUnknown1C1 = 0;
    m_pLastMouseOverControl = NULL;
    m_pKeyFocus = NULL;
    m_pMouseCursor = NULL;
    m_pMouseScene = NULL;
    m_resCursor = "";
    m_bCursorCentered = 0;
    m_nMouseCursor = 1;
    m_pToolTipPanel = new CSWGuiToolTipPanel(this);
    m_fToolTipTimer = 0;
    m_bToolTipShown = 0;
    m_pToolTipControl = NULL;
    m_bToolTipDisabled = 0;
    m_pToolTipDisabledControl = NULL;
    m_pDragControl = NULL;
#endif
    m_apGuiSounds = NULL;
    m_nGuiSounds = 0;
#ifndef _XBOX
    g_pGuiMan = this;
    m_nLastInputEvent = 0;
    if (g_pAppManager->m_pClientExoApp->GetClientOptions() != NULL)
    {
        double fToolTipDelay =
            g_pAppManager->m_pClientExoApp->GetClientOptions()->m_fGuiScale;
        m_fToolTipDelay = fToolTipDelay;
    }
#endif
#ifndef _XBOX
    m_nOldMouseX = -1;
    m_nOldMouseY = -1;
#endif
}
// PC 0x0040BFD0
void CSWGuiManager::AddPanel(CSWGuiPanel* pPanel, int nFlags, int bPlaySound)
{
    if (pPanel != NULL)
    {
        if (bPlaySound != 0)
        {
            PlayGuiSound(pPanel->m_nOpenSound);
        }
        if (GetPosInPanelList(pPanel) != -1)
        {
            if (pPanel->m_nExitState == CSWGuiPanel::EXITSTATE_DELETE ||
                pPanel->m_nExitState != 0)
            {
                pPanel->m_bDeferredRemoval = 0;
                pPanel->m_nExitState = 0;
                if ((nFlags & SWGUI_PANEL_FLAG_PUSH_MODAL) != 0)
                {
                    PushModalPanel(pPanel);
                }
                pPanel->OnPanelAdded();
                DetermineNeedToDraw();
            }
        }
        else
        {
            pPanel->m_bModal = (nFlags & SWGUI_PANEL_FLAG_MODAL) != 0;
            if (pPanel->m_bModal)
            {
                m_nBackgroundPanels++;
            }
            m_lPanels.Add(pPanel);
            if ((nFlags & SWGUI_PANEL_FLAG_PUSH_MODAL) != 0)
            {
                PushModalPanel(pPanel);
            }
            pPanel->OnPanelAdded();
            DetermineNeedToDraw();
        }
    }
}
// PC 0x0040C080
void CSWGuiManager::SendPanelToBack(CSWGuiPanel* pPanel)
{
    if (m_lModalPanels.IndexOf(pPanel) == -1)
    {
        int nIndex = m_lPanels.IndexOf(pPanel);

        if (nIndex != -1)
        {
            m_lPanels.DelIndex(nIndex);
            m_lPanels.Insert(pPanel, 0);
        }
        DetermineNeedToDraw();
    }
}
// PC 0x0040A300
void CSWGuiManager::GetSafeMargin(int& nX, int& nY)
{
    nX = SAFE_MARGIN_X;
    nY = SAFE_MARGIN_Y;
}
// PC 0x0040A320
void CSWGuiManager::UnloadGuiSounds()
{
    if (m_apGuiSounds != NULL)
    {
        for (int i = 0; i < m_nGuiSounds; i++)
        {
            if (m_apGuiSounds[i] != NULL)
            {
                delete m_apGuiSounds[i];
                m_apGuiSounds[i] = NULL;
            }
        }
        delete m_apGuiSounds;
        m_apGuiSounds = NULL;
    }
    m_nGuiSounds = 0;
}
// PC 0x0040A390
void CSWGuiManager::PlayGuiSound(char nSound)
{
    if (nSound != -1 && nSound < m_nGuiSounds)
    {
        if (m_apGuiSounds != NULL && m_apGuiSounds[nSound] != NULL)
        {
            m_apGuiSounds[nSound]->Play();
        }
    }
}

#ifndef _XBOX
int CSWGuiManager::HandleLMouseUp()  // PC 0x0040A3C0
{
    int bHandled = 0;

    if (m_pMouseCapture != NULL)
    {
        if (m_nMouseCaptureType == 1)
        {
            m_pMouseCapture->HandleLMouseUp();
            bHandled = 1;
        }
        else
        {
            bHandled = 0;
        }
    }
    m_bLeftMouseDown = 0;
    return bHandled;
}
#endif

#ifndef _XBOX
int CSWGuiManager::HandleRMouseUp()  // PC 0x0040A3F0
{
    int bHandled = 0;

    if (m_pMouseCapture != NULL)
    {
        if (m_nMouseCaptureType == 4)
        {
            m_pMouseCapture->HandleRMouseUp();
            bHandled = 1;
        }
        else
        {
            bHandled = 0;
        }
    }
    return bHandled;
}
#endif

#ifndef _XBOX
void CSWGuiManager::CaptureMouse(CSWGuiObject* pObject, int nType)  // PC 0x0040A410
{
    if (m_pMouseCapture != NULL)
    {
        m_pMouseCapture->HandleLoseMouseFocus();
    }
    m_pMouseCapture = pObject;
    m_nMouseCaptureType = nType;
}
#endif

#ifndef _XBOX
void CSWGuiManager::ReleaseMouse(CSWGuiObject* pObject)  // PC 0x0040A450
{
    if (pObject != NULL && m_pMouseCapture == pObject)
    {
        m_pMouseCapture = NULL;
        pObject->HandleLoseMouseFocus();
    }
    m_nMouseCaptureType = 0;
}
#endif

#ifndef _XBOX
void CSWGuiManager::DeactivateSoftwareMouse()  // PC 0x0040A480
{
    if (m_pMouseScene != NULL)
    {
        delete m_pMouseScene;
        m_pMouseScene = NULL;
    }
    if (m_pMouseCursor != NULL)
    {
        m_pMouseCursor = NULL;
    }
}
#endif
// PC 0x0040A4B0
void CSWGuiManager::HideSoftwareMouse()
{
#ifndef _XBOX
    DeactivateSoftwareMouse();
#endif
}

#ifndef _XBOX
void CSWGuiManager::SetSoftwareMouseCursor(int nCursor, int bCentered,  // PC 0x0040A4C0
                                           int bForce)
{
    m_nMouseCursor = nCursor;
    if (bForce != 0)
    {
        nCursor += nCursor % 2;
    }
    m_resCursor = g_asMouseCursorNames[nCursor];
    if (m_pMouseCursor != NULL)
    {
        ((Gob*)m_pMouseCursor)->ReplaceTexture("", m_resCursor.GetResRefStr());
        if (bCentered != m_bCursorCentered)
        {
            char* sAnimation = "center";

            if (bCentered == 0)
            {
                sAnimation = "default";
            }
            ((Gob*)m_pMouseCursor)->PlayAnimation(sAnimation, 1.0f, 0, 0);
            m_pMouseCursor->AnimateExternal(1.0f);
            ((Gob*)m_pMouseCursor)->PlayAnimation("", 0.0f,
                                                  Gob::ANIMATION_FLAG_STOP, 0);
        }
    }
    m_bCursorCentered = bCentered;
}
#endif

#ifndef _XBOX
void CSWGuiManager::ChangeToolTipText(const CExoString& sText)  // PC 0x0040A590
{
    if (m_pToolTipPanel != NULL)
    {
        m_pToolTipPanel->SetToolTipText(sText);
    }
}
#endif

#ifndef _XBOX
void CSWGuiManager::ReloadToolTipPanel()  // PC 0x0040A5A0
{
    if (m_pToolTipPanel != NULL)
    {
        delete m_pToolTipPanel;
    }
    m_pToolTipPanel = new CSWGuiToolTipPanel(this);
}
#endif

#ifndef _XBOX
void CSWGuiManager::StoreCurrentMousePosition()  // PC 0x0040A620
{
    m_nOldMouseX = m_nMouseX;
    m_nOldMouseY = m_nMouseY;
}
#endif
// PC 0x0040A630
CSWGuiExtent CSWGuiExtent::Intersection(const CSWGuiExtent& extentFirst,
                                        const CSWGuiExtent& extentSecond)
{
    CSWGuiExtent extentResult;

    extentResult.m_nLeft = (extentFirst.m_nLeft > extentSecond.m_nLeft)
                               ? extentFirst.m_nLeft
                               : extentSecond.m_nLeft;
    extentResult.m_nTop = (extentFirst.m_nTop > extentSecond.m_nTop)
                              ? extentFirst.m_nTop
                              : extentSecond.m_nTop;
    extentResult.m_nWidth =
        ((extentFirst.m_nLeft + extentFirst.m_nWidth <
          extentSecond.m_nLeft + extentSecond.m_nWidth)
             ? extentFirst.m_nLeft + extentFirst.m_nWidth
             : extentSecond.m_nLeft + extentSecond.m_nWidth) -
        extentResult.m_nLeft;
    extentResult.m_nHeight =
        ((extentFirst.m_nTop + extentFirst.m_nHeight <
          extentSecond.m_nTop + extentSecond.m_nHeight)
             ? extentFirst.m_nTop + extentFirst.m_nHeight
             : extentSecond.m_nTop + extentSecond.m_nHeight) -
        extentResult.m_nTop;
    return extentResult;
}
// PC 0x0040A6B0
CSWGuiExtent CSWGuiExtent::Union(const CSWGuiExtent& extentFirst,
                                 const CSWGuiExtent& extentSecond)
{
    CSWGuiExtent extentResult;

    extentResult.m_nLeft = (extentFirst.m_nLeft < extentSecond.m_nLeft)
                               ? extentFirst.m_nLeft
                               : extentSecond.m_nLeft;
    extentResult.m_nTop = (extentFirst.m_nTop < extentSecond.m_nTop)
                              ? extentFirst.m_nTop
                              : extentSecond.m_nTop;
    extentResult.m_nWidth =
        ((extentFirst.m_nLeft + extentFirst.m_nWidth >
          extentSecond.m_nLeft + extentSecond.m_nWidth)
             ? extentFirst.m_nLeft + extentFirst.m_nWidth
             : extentSecond.m_nLeft + extentSecond.m_nWidth) -
        extentResult.m_nLeft;
    extentResult.m_nHeight =
        ((extentFirst.m_nTop + extentFirst.m_nHeight >
          extentSecond.m_nTop + extentSecond.m_nHeight)
             ? extentFirst.m_nTop + extentFirst.m_nHeight
             : extentSecond.m_nTop + extentSecond.m_nHeight) -
        extentResult.m_nTop;
    return extentResult;
}
// PC 0x0040A730
void CSWGuiPanel::SetExtent(const CSWGuiExtent& cExtent)
{
    CSWGuiExtent cBorderExtent;

    m_cExtent = cExtent;
    cBorderExtent.m_nLeft = 0;
    cBorderExtent.m_nTop = 0;
    cBorderExtent.m_nWidth = cExtent.m_nWidth;
    cBorderExtent.m_nHeight = cExtent.m_nHeight;
    m_pBorder->SetExtent(cBorderExtent);
}
// PC 0x0040A790
void CSWGuiPanel::CenterPanel()
{
    m_cExtent.m_nLeft = (m_pManager->m_nWidth - m_cExtent.m_nWidth) / 2;
    m_cExtent.m_nTop = (m_pManager->m_nHeight - m_cExtent.m_nHeight) / 2;

    CSWGuiExtent cBorderExtent;
    cBorderExtent.m_nLeft = 0;
    cBorderExtent.m_nTop = 0;
    cBorderExtent.m_nWidth = m_cExtent.m_nWidth;
    cBorderExtent.m_nHeight = m_cExtent.m_nHeight;
    m_pBorder->SetExtent(cBorderExtent);
#ifndef _XBOX
    m_bPositioned = 1;
#endif
}
// PC 0x0040A7F0
void CSWGuiPanel::SetActiveControl(CSWGuiControl* pControl, int bPlaySound)
{
    if (m_pActiveControl != pControl)
    {
        if (m_pActiveControl != NULL)
        {
            m_pActiveControl->HandleInputEvent(1, 1);
        }
        m_pActiveControl = pControl;
        if (pControl != NULL)
        {
            pControl->HandleInputEvent(GUIEVENT_MOUSE_ENTER, 1);
            if (bPlaySound != 0 && m_pManager != NULL)
            {
                m_pManager->PlayGuiSound(pControl->m_nFocusSound);
            }
        }
    }
}
// PC 0x0040A840
void CSWGuiPanel::StartLoadFromLayout(const CResRef& cLayout, int bScalePosition, int bScaleSize)
{
    if (!m_bLoadingLayout)
    {
        CResGFF* pGFF = new CResGFF(RESTYPE_GUI, "GUI ", cLayout);

        m_pLayout = pGFF;
        if (pGFF->Demand() != NULL)
        {
            CResStruct cStruct;
            CResStruct cBorder;
            int bSuccess;

            pGFF->GetTopLevelStruct(&cStruct);
            m_cExtent.Load(pGFF, cStruct);
            m_vColor = pGFF->ReadFieldVector(&cStruct, "COLOR", bSuccess,
                                             Vector(-1.0f, -1.0f, -1.0f));
            if (pGFF->GetStructFromStruct(&cBorder, &cStruct, "BORDER"))
            {
                {
                    CExoString sBorder("BORDER");

                    m_pBorder->Load(pGFF, cStruct, sBorder);
                }
                CSWGuiExtent cExtent;

                cExtent.m_nLeft = 0;
                cExtent.m_nTop = 0;
                cExtent.m_nWidth = m_cExtent.m_nWidth;
                cExtent.m_nHeight = m_cExtent.m_nHeight;
                m_pBorder->SetExtent(cExtent);
            }
            else
            {
                CResRef cDefault("");

                CSWGuiBorderParams& cParams = m_pBorder->m_cParams;

                CResRef* pFill = &pGFF->ReadFieldCResRef(&cStruct, "BACKGROUND",
                                                         bSuccess, cDefault);

                cParams.SetFillImage(*pFill, 0);
            }
            SetAlpha(pGFF->ReadFieldFLOAT(&cStruct, "ALPHA", bSuccess, 1.0f));
            pGFF->GetList(&m_lLayoutControls, &cStruct, "CONTROLS");
            m_bLoadingLayout = 1;
        }

        CSWGuiExtent cExtent = m_cExtent;
        float fScaleX;
        float fScaleY;
        if (g_pAppManager->m_pClientExoApp->GetSWGuiManager() != NULL)
        {
            fScaleX = (float)g_pAppManager->m_pClientExoApp->GetSWGuiManager()->m_nWidth / (float)g_nGUIWidth;
            fScaleY = (float)g_pAppManager->m_pClientExoApp->GetSWGuiManager()->m_nHeight / (float)g_nGUIHeight;
        }
        else
        {
            fScaleX = (float)g_nScreenWidth / (float)g_nGUIWidth;
            fScaleY = (float)g_nScreenHeight / (float)g_nGUIHeight;
        }
        if (bScaleSize)
        {
            cExtent.m_nHeight = (int)(cExtent.m_nHeight * fScaleY);
            cExtent.m_nWidth = (int)(cExtent.m_nWidth * fScaleX);
        }
        if (bScalePosition)
        {
            cExtent.m_nTop = (int)(cExtent.m_nTop * fScaleY);
            cExtent.m_nLeft = (int)(cExtent.m_nLeft * fScaleX);
        }
        SetExtent(cExtent);
        m_pBorder->SetExtent(cExtent);
        m_bPositioned = 1;
    }
}
// PC 0x0040AB70
void CSWGuiPanel::SetBackground(const CResRef& cBackground)
{
    m_pBorder->m_cParams.SetFillImage(cBackground, 0);
}
// PC 0x0040AB90
void CSWGuiPanel::ConnectControlMoves()
{
    int nControls = m_lControls.num;

    for (int i = 0; i < nControls; i++)
    {
        CSWGuiControl* pControl = m_lControls.element[i];

        if (pControl != NULL)
        {
            CSWGuiNavigable* pNavigable = pControl->AsNavigable();

            if (pNavigable != NULL)
            {
                for (int j = 0; j < 4; j++)
                {
                    int nMoveTo = pNavigable->GetMoveToControl(j);

                    if (nMoveTo < 0 || nMoveTo >= nControls)
                    {
                        pNavigable->SetMoveToControl(j, NULL);
                    }
                    else
                    {
                        pNavigable->SetMoveToControl(
                            j, m_lControls.element[nMoveTo]);
                    }
                }
            }
        }
    }
}
// PC 0x0040A180
CExoString CSWGuiPanel::GetFullScreenBG()
{
    CExoString sBackground = "800x600";

    if (g_nScreenWidth == 1024)
    {
        if (g_nScreenHeight == 768)
        {
            sBackground = "1024x768";
        }
    }
    else if (g_nScreenWidth == 1280)
    {
        if (g_nScreenHeight == 960 || g_nScreenHeight == 1024)
        {
            sBackground = "1280x960";
        }
    }
    else if (g_nScreenWidth == 1600)
    {
        if (g_nScreenHeight == 1200)
        {
            sBackground = "1600x1200";
        }
    }

#ifndef _XBOX
    sBackground = sBackground + CSWGuiManager::FULLSCREEN_BGS[m_nBackground];
#endif
    return sBackground;
}
// PC 0x0040AC00
void CSWGuiPanel::ResetFont()
{
#ifndef _XBOX
    if (m_bUseFont != 0)
    {
        for (int i = 0; i < m_lControls.num; i++)
        {
            if (m_lControls.element[i] != NULL)
            {
                m_lControls.element[i]->ReSetFont();
            }
        }
    }
#endif
}
// PC 0x0040AC40
void CSWGuiPanel::GetExtentAccountingForPanelOffset(CSWGuiExtent* pExtent)
{
    *pExtent = m_cExtent;
    if (!m_bModal)
    {
        if (m_bCenterHorizontal)
        {
            pExtent->m_nLeft += (m_pManager->m_nWidth - 640) / 2;
        }
        if (m_bCenterVertical)
        {
            pExtent->m_nTop += (m_pManager->m_nHeight - 480) / 2;
        }
    }
}
// PC 0x0040ACB0
CSWGuiManager::~CSWGuiManager()
{
    if (m_pSafeRegionBorder != NULL)
    {
        delete m_pSafeRegionBorder;
        m_pSafeRegionBorder = NULL;
    }
    if (m_pSafeRegionPanel != NULL)
    {
        delete m_pSafeRegionPanel;
        m_pSafeRegionPanel = NULL;
    }
#ifndef _XBOX
    if (m_pPCBackground != NULL)
    {
        delete m_pPCBackground;
        m_pPCBackground = NULL;
    }
#endif
#ifndef _XBOX
    DeactivateSoftwareMouse();
    if (m_pToolTipPanel != NULL)
    {
        delete m_pToolTipPanel;
        m_pToolTipPanel = NULL;
    }
#endif
    UnloadGuiSounds();
#ifndef _XBOX
    g_pGuiMan = NULL;
#endif
}
// PC 0x0040AD80
int CSWGuiManager::GetPosInModalStack(CSWGuiPanel* pPanel)
{
    for (int i = 0; i < m_lModalPanels.num; i++)
    {
        if (m_lModalPanels.element[i] == pPanel)
        {
            return i;
        }
    }
    return -1;
}
// PC 0x0040C0D0
int CSWGuiManager::PanelExists(CSWGuiPanel* pPanel)
{
    return GetPosInPanelList(pPanel) != -1;
}
// PC 0x0040C0F0
int CSWGuiManager::PushModalPanel(CSWGuiPanel* pPanel)
{
    if (pPanel == NULL)
    {
        return 0;
    }
#ifndef _XBOX
    if (m_pMouseOverControl != NULL)
    {
        m_pMouseOverControl->HandleMouseOver(0);
    }
#endif
    if (GetPosInPanelList(pPanel) == -1)
    {
        AddPanel(pPanel, 0, 1);
    }
    if (GetPosInModalStack(pPanel) != -1)
    {
        return 0;
    }
#ifndef _XBOX
    if (m_pKeyFocus != NULL)
    {
        m_pKeyFocus->HandleFocusChange(0);
    }
#endif
    m_lModalPanels.Add(pPanel);
    return 1;
}
// PC 0x0040ADB0
int CSWGuiManager::GetPosInPanelList(CSWGuiPanel* pPanel)
{
    for (int i = 0; i < m_lPanels.num; i++)
    {
        if (m_lPanels.element[i] != NULL && m_lPanels.element[i] == pPanel)
        {
            return i;
        }
    }
    return -1;
}
// PC 0x0040ADF0
int CSWGuiManager::HitCheckMouse(int nX, int nY, CSWGuiPanel** ppPanel,
                                 CSWGuiControl** ppControl, int bModalOnly)
{
    int nModalCount = m_lModalPanels.num;

    if (nModalCount != 0)
    {
        CSWGuiControl* pHit =
            m_lModalPanels.element[nModalCount - 1]->HitCheckMouse(nX, nY);

        if (ppPanel != NULL)
        {
            if (pHit != NULL)
            {
                *ppPanel = m_lModalPanels.element[nModalCount - 1];
            }
            else
            {
                *ppPanel = NULL;
            }
        }
        if (ppControl != NULL)
        {
            *ppControl = pHit;
        }
        return (int)pHit;
    }
    for (int i = m_lPanels.num - 1; i >= 0; i--)
    {
        CSWGuiControl* pHit = m_lPanels.element[i]->HitCheckMouse(nX, nY);

        if (pHit != NULL)
        {
            if (ppPanel != NULL)
            {
                *ppPanel = m_lPanels.element[i];
            }
            if (ppControl != NULL)
            {
                *ppControl = pHit;
            }
            return 1;
        }
    }
    if (ppPanel != NULL)
    {
        *ppPanel = NULL;
    }
    if (ppControl != NULL)
    {
        *ppControl = NULL;
    }
    return 0;
}
// PC 0x0040C160
CSWGuiPanel* CSWGuiManager::PopModalPanel()
{
    int nPanels = m_lModalPanels.num;
    if (nPanels == 0)
    {
        return NULL;
    }
    CSWGuiPanel* pPanel = m_lModalPanels.element[nPanels - 1];
    CExoArrayList<CSWGuiPanel*>* pModalPanels = &m_lModalPanels;
    pModalPanels->DelIndex(nPanels - 1);
    DetermineNeedToDraw();
#ifndef _XBOX
    if (m_lModalPanels.num > 0)
    {
        if (pModalPanels->element[m_lModalPanels.num - 1]
                ->GetActiveControl() != NULL)
        {
            pModalPanels->element[m_lModalPanels.num - 1]
                ->GetActiveControl()
                ->HandleFocusChange(1);
        }
    }
#endif
    return pPanel;
}
// PC 0x0040AED0
void CSWGuiManager::DetermineNeedToDraw()
{
    int bBackgroundFound = 0;

    int nModalPanels = m_lModalPanels.num;

    for (int i = nModalPanels - 1; i >= 0; i--)
    {
        CSWGuiPanel* pPanel = m_lModalPanels.element[i];

        if (pPanel != NULL)
        {
            if (pPanel->m_bVisible)
            {
                if (bBackgroundFound != 0)
                {
                    pPanel->m_bNeedsDraw = 0;
                }
                else
                {
                    pPanel->m_bNeedsDraw = 1;
                    if (pPanel->m_bModal)
                    {
                        bBackgroundFound = 1;
#ifndef _XBOX
                        m_sBackground = pPanel->GetFullScreenBG();
#endif
                    }
                }
            }
        }
    }
    int nPanels = m_lPanels.num;
    for (int j = nPanels - 1; j >= 0; j--)
    {
        CSWGuiPanel* pPanel = m_lPanels.element[j];

        if (pPanel != NULL)
        {
            if ((pPanel->m_bVisible) && GetPosInModalStack(pPanel) == -1)
            {
                if (bBackgroundFound != 0)
                {
                    pPanel->m_bNeedsDraw = 0;
                }
                else
                {
                    pPanel->m_bNeedsDraw = 1;
                    if (pPanel->m_bModal)
                    {
                        bBackgroundFound = 1;
#ifndef _XBOX
                        m_sBackground = pPanel->GetFullScreenBG();
#endif
                    }
                }
            }
        }
    }
}
// PC 0x0040B000
int CSWGuiManager::LoadGuiSounds()
{
    C2DA* p2DA = new C2DA("guisounds", 0);

    if (p2DA == NULL)
    {
        return 0;
    }
    if (p2DA->Load2DArray() == 0)
    {
        delete p2DA;
        return 0;
    }
    if (p2DA->m_nNumRows > 0)
    {
        CExoString sSoundResRef;

        int nNumRows = p2DA->m_nNumRows;
        m_nGuiSounds = nNumRows;
        m_apGuiSounds = new CExoSoundSource*[m_nGuiSounds];
        if (m_apGuiSounds != NULL)
        {
            for (int i = 0; i < m_nGuiSounds; i++)
            {
                if (p2DA->GetCExoStringEntry(i, "SoundResRef",
                                             &sSoundResRef))
                {
                    m_apGuiSounds[i] =
                        new CExoSoundSource(sSoundResRef);
                    if (m_apGuiSounds[i] != NULL)
                    {
                        m_apGuiSounds[i]->SetPriorityGroup(CExoSound::PRIORITY_GROUP_GUI);
                    }
                }
            }
        }
    }
    delete p2DA;
    return 1;
}
// PC 0x0040B1D0
int CSWGuiManager::IsOnTop(CSWGuiPanel* pPanel)
{
    int nIndex = m_lModalPanels.num - 1;

    while (nIndex >= 0)
    {
        if (m_lModalPanels.element[nIndex]->m_nExitState == 0)
        {
            return m_lModalPanels.element[nIndex] == pPanel;
        }
        nIndex--;
    }
    nIndex = m_lPanels.num - 1;
    while (nIndex >= 0)
    {
        if (m_lPanels.element[nIndex]->m_nExitState == 0)
        {
            return m_lPanels.element[nIndex] == pPanel;
        }
        nIndex--;
    }
    return 0;
}

#ifndef _XBOX
int CSWGuiManager::ActivateSoftwareMouse()  // PC 0x0040B260
{
    m_pMouseScene = new CSWGuiScene();
    if (m_pMouseScene == NULL)
    {
        return 0;
    }
    {
        CSWGuiExtent cExtent(0, 0, m_nWidth, m_nHeight);
        m_pMouseScene->SetExtent(cExtent);
    }
    m_pMouseCursor = m_pMouseScene->AddModel("gui_mouse", -1);
    if (m_pMouseCursor == NULL)
    {
        return 0;
    }
    ((Gob*)m_pMouseCursor)->ReplaceTexture("", m_resCursor.GetResRefStr());
    m_pMouseScene->GetCamera()->SetPosition(
        Vector(m_nWidth * 0.5f * 0.01f, m_nHeight * 0.5f * 0.01f, m_nWidth * 5.8f / 640.0f));
    m_pMouseCursor->SetPosition(
        Vector(m_nMouseX * 0.01f, (m_nHeight - m_nMouseY) * 0.01f, 0.0f));
    if (m_bCursorCentered)
    {
        ((Gob*)m_pMouseCursor)->PlayAnimation("center", 1.0f, 0, 0);
        m_pMouseCursor->AnimateExternal(1.0f);
        ((Gob*)m_pMouseCursor)->PlayAnimation("", 0.0f, Gob::ANIMATION_FLAG_STOP, 0);
    }
    ((Gob*)m_pMouseCursor)->SetIllumination(Vector(1.0f, 1.0f, 1.0f), 1);
    return 1;
}
#endif

#ifndef _XBOX
void CSWGuiManager::ShowSoftwareMouse()  // PC 0x0040B490
{
    if (m_pMouseCursor == NULL)
    {
        ActivateSoftwareMouse();
    }
}
#endif

#ifndef _XBOX
int CSWGuiManager::HandleKeyPress(int nKey)  // PC 0x0040B4A0
{
    if (m_lModalPanels.num != 0)
    {
        CSWGuiPanel* pTop = m_lModalPanels.element[m_lModalPanels.num - 1];
        CSWGuiEditbox* pFocus = m_pKeyFocus;

        if (pTop->GetActiveControl() == pFocus)
        {
            pFocus->HandleKeyPress(nKey);
        }
    }
    else if (m_pKeyFocus != NULL)
    {
        m_pKeyFocus->HandleKeyPress(nKey);
        return 1;
    }
    return 0;
}
#endif

#ifndef _XBOX
CSWGuiControl* CSWGuiManager::UpdateMouseOverControl()  // PC 0x0040B500
{
    CSWGuiPanel* pPanel;
    CSWGuiControl* pControl;

    HitCheckMouse(m_nMouseX, m_nMouseY, &pPanel, &pControl, 0);
    if (pControl != m_pMouseOverControl)
    {
        if (m_pMouseOverControl != NULL)
        {
            m_pMouseOverControl->HandleMouseOver(0);
        }
        m_pMouseOverControl = pControl;
    }
    CSWGuiControl* pLast = m_pLastMouseOverControl;
    CSWGuiControl* pOver = m_pMouseOverControl;
    m_pLastMouseOverControl = pControl;
    if (pOver != pLast)
    {
        m_bLeftMouseDown = 0;
    }
    return pOver;
}
#endif
// PC 0x0040B560
void CSWGuiManager::GetUpdatedFontName(char* sFont)
{
    if (strcmp("fnt_console", sFont) != 0)
    {
        unsigned char nLength = strlen(sFont);

        switch (g_pAppManager->m_pClientExoApp->GetClientOptions()->m_nFontMode)
        {
        case 1:
            sFont[nLength] = 'a';
            sFont[nLength + 1] = 0;
            break;
        case 2:
            sFont[nLength] = 'b';
            sFont[nLength + 1] = 0;
            break;
        default:
            if (g_nScreenWidth < 1280)
            {
                sFont[nLength] = 'a';
            }
            else if (g_nScreenWidth < 2500)
            {
                sFont[nLength] = 'b';
            }
            else
            {
                sFont[nLength] = 'b';
            }
            sFont[nLength + 1] = 0;
            break;
        }
    }
}

#ifndef _XBOX
void CSWGuiManager::SetSize(int nWidth, int nHeight)  // PC 0x0040C1D0
{
    if (m_nWidth != nWidth || m_nHeight != nHeight)
    {
        m_nHeight = nHeight;
        m_nWidth = nWidth;
        UpdateAllFonts();
        if (m_pPCBackground != NULL)
        {
            delete m_pPCBackground;
            m_pPCBackground = NULL;
            DetermineNeedToDraw();
        }
    }
}
#endif
// PC 0x0040C220
void CSWGuiManager::DrawSafeRegion(float fElapsed)
{
    if (m_pSafeRegionPanel == NULL)
    {
        m_pSafeRegionPanel = new CSWGuiPanel(this);
        m_pSafeRegionBorder = new CSWGuiLabel();
        m_pSafeRegionPanel->AddControl(m_pSafeRegionBorder);
        CSWGuiPanel* pPanel = m_pSafeRegionPanel;
        {
            Vector vColor(-1.0f, -1.0f, -1.0f);
            pPanel->m_vColor = vColor;
            CResRef cCorner("border1b");
            CResRef cEdge("border2b");
            m_pSafeRegionBorder->Initialize(
                CSWGuiExtent(), CSWGuiTextParams(),
                CSWGuiBorderParams(cEdge, cCorner, CResRef(), 0, 0, 1.0f,
                                   SafeRegionWhite(vColor), 0), 1.0f);
        }
    }
    CSWGuiExtent cScreen(0, 0, m_nWidth, m_nHeight);
    m_pSafeRegionPanel->SetExtent(cScreen);
    int nX = SAFE_MARGIN_X - m_pSafeRegionBorder->m_cBorder.m_cParams.GetBorderDim();
    int nY = SAFE_MARGIN_Y - m_pSafeRegionBorder->m_cBorder.m_cParams.GetBorderDim();
    m_pSafeRegionBorder->SetExtent(
        CSWGuiExtent(nX, nY, m_nWidth - 2 * nX, m_nHeight - 2 * nY));
    m_pSafeRegionPanel->Draw(fElapsed);
}

#ifndef _XBOX
void CSWGuiManager::UpdateAllFonts()  // PC 0x0040B620
{
    for (int i = 0; i < m_lPanels.num; i++)
    {
#ifndef _XBOX
        if (m_lPanels.element[i]->m_bUseFont != 0)
        {
            m_lPanels.element[i]->ResetFont();
        }
#endif
    }
    if (m_pToolTipPanel != NULL)
    {
        m_pToolTipPanel->ResetFont();
    }
    if (g_pAppManager->m_pClientExoApp->GetInGameGui() != NULL)
    {
        g_pAppManager->m_pClientExoApp->GetInGameGui()->ResetInterfaceFont();
    }
}
#endif

#ifndef _XBOX
void CSWGuiManager::DisplayToolTip(const CExoString& sText)  // PC 0x0040B690
{
    if (m_bToolTipDisabled == 0)
    {
        if (g_pAppManager->m_pClientExoApp->GetClientOptions()
                ->m_bEnableTooltips)
        {
            m_bToolTipShown = 1;
            m_pToolTipPanel->SetToolTipText(sText);
            m_pToolTipPanel->PositionToolTip();
        }
    }
}
#endif
// PC 0x0040C480
void CSWGuiManager::HideAllPanels()
{
    SwguiTrace2();
    for (int i = 0; i < m_lPanels.num; i++)
    {
        m_lPanels.element[i]->SetVisible(0);
    }
}
#ifndef _XBOX
// PC 0x0040C4B0
void CSWGuiManager::ReloadPanels(int nWidth, int nHeight)
{
    CSWGuiExtent cExtent;
    float fScaleX;
    float fScaleY;
    int i;
    int j;
    for (i = 0; i < m_lPanels.num; i++)
    {
        cExtent = m_lPanels.element[i]->m_cExtent;
        fScaleX = (float)nWidth / cExtent.m_nWidth;
        fScaleY = (float)nHeight / cExtent.m_nHeight;
        cExtent.m_nLeft = (int)(cExtent.m_nLeft * fScaleX);
        cExtent.m_nTop = (int)(cExtent.m_nTop * fScaleY);
        cExtent.m_nWidth = (int)(cExtent.m_nWidth * fScaleX);
        cExtent.m_nHeight = (int)(cExtent.m_nHeight * fScaleY);
        m_lPanels.element[i]->SetExtent(cExtent);
        m_lPanels.element[i]->m_pBorder->SetExtent(cExtent);
        for (j = 0; j < m_lPanels.element[i]->m_lControls.num; j++)
        {
            if (m_lPanels.element[i]->GetControl(j) != NULL &&
                m_lPanels.element[i]->GetControl(j)->AsSWGuiControl() != NULL)
            {
                cExtent = m_lPanels.element[i]->GetControl(j)->AsSWGuiControl()->m_cExtent;
                cExtent.m_nLeft = (int)(cExtent.m_nLeft * fScaleX);
                cExtent.m_nTop = (int)(cExtent.m_nTop * fScaleY);
                cExtent.m_nWidth = (int)(cExtent.m_nWidth * fScaleX);
                cExtent.m_nHeight = (int)(cExtent.m_nHeight * fScaleY);
                m_lPanels.element[i]->GetControl(j)->SetExtent(cExtent);
            }
        }
    }
    for (i = 0; i < m_lModalPanels.num; i++)
    {
        cExtent = m_lModalPanels.element[i]->m_cExtent;
        fScaleX = (float)nWidth / cExtent.m_nWidth;
        fScaleY = (float)nHeight / cExtent.m_nHeight;
        cExtent.m_nLeft = (int)(cExtent.m_nLeft * fScaleX);
        cExtent.m_nTop = (int)(cExtent.m_nTop * fScaleY);
        cExtent.m_nWidth = (int)(cExtent.m_nWidth * fScaleX);
        cExtent.m_nHeight = (int)(cExtent.m_nHeight * fScaleY);
        m_lModalPanels.element[i]->SetExtent(cExtent);
        m_lModalPanels.element[i]->m_pBorder->SetExtent(cExtent);
        for (j = 0; j < m_lModalPanels.element[i]->m_lControls.num; j++)
        {
            if (m_lModalPanels.element[i]->GetControl(j) != NULL &&
                m_lModalPanels.element[i]->GetControl(j)->AsSWGuiControl() != NULL)
            {
                cExtent = m_lModalPanels.element[i]->GetControl(j)->AsSWGuiControl()->m_cExtent;
                cExtent.m_nLeft = (int)(cExtent.m_nLeft * fScaleX);
                cExtent.m_nTop = (int)(cExtent.m_nTop * fScaleY);
                cExtent.m_nWidth = (int)(cExtent.m_nWidth * fScaleX);
                cExtent.m_nHeight = (int)(cExtent.m_nHeight * fScaleY);
                m_lModalPanels.element[i]->GetControl(j)->SetExtent(cExtent);
            }
        }
    }
    if (g_pAppManager->m_pClientExoApp->GetInGameGui() != NULL)
    {
        CGuiInGame* pInGameGui = g_pAppManager->m_pClientExoApp->GetInGameGui();
        for (i = 0; i < CGuiInGame::IN_GAME_SCREEN_COUNT; i++)
        {
            if (pInGameGui->m_apScreenPanels[i] != NULL)
            {
                cExtent = pInGameGui->m_apScreenPanels[i]->m_cExtent;
                fScaleX = (float)nWidth / cExtent.m_nWidth;
                fScaleY = (float)nHeight / cExtent.m_nHeight;
                cExtent.m_nLeft = (int)(cExtent.m_nLeft * fScaleX);
                cExtent.m_nTop = (int)(cExtent.m_nTop * fScaleY);
                cExtent.m_nWidth = (int)(cExtent.m_nWidth * fScaleX);
                cExtent.m_nHeight = (int)(cExtent.m_nHeight * fScaleY);
                pInGameGui->m_apScreenPanels[i]->SetExtent(cExtent);
                pInGameGui->m_apScreenPanels[i]->m_pBorder->SetExtent(cExtent);
                for (j = 0; j < pInGameGui->m_apScreenPanels[i]->m_lControls.num; j++)
                {
                    if (pInGameGui->m_apScreenPanels[i]->GetControl(j) != NULL &&
                        pInGameGui->m_apScreenPanels[i]->GetControl(j)->AsSWGuiControl() != NULL)
                    {
                        cExtent = pInGameGui->m_apScreenPanels[i]->GetControl(j)->AsSWGuiControl()->m_cExtent;
                        cExtent.m_nLeft = (int)(cExtent.m_nLeft * fScaleX);
                        cExtent.m_nTop = (int)(cExtent.m_nTop * fScaleY);
                        cExtent.m_nWidth = (int)(cExtent.m_nWidth * fScaleX);
                        cExtent.m_nHeight = (int)(cExtent.m_nHeight * fScaleY);
                        pInGameGui->m_apScreenPanels[i]->GetControl(j)->SetExtent(cExtent);
                    }
                }
            }
        }
    }
}
#endif
// PC 0x0040CA40
void CSWGuiPanel::OnPanelRemoved()
{
#ifndef _XBOX
    g_pGuiMan->RemoveToolTip();
    CSWGuiEditbox* pFocus = g_pGuiMan->m_pKeyFocus;
#endif

#ifndef _XBOX
    if (pFocus != NULL)
    {
        if (pFocus == GetActiveControl())
        {
#ifndef _XBOX
            g_pGuiMan->m_pKeyFocus->HandleFocusChange(0);
#endif
        }
    }
#endif
    CSWGuiManager* pManager = m_pManager;
#ifndef _XBOX
    CSWGuiControl* pOver = pManager->m_pMouseOverControl;
#endif

#ifndef _XBOX
    if (pOver != NULL)
    {
        int nControls = m_lControls.num;
        for (int nIndex = 0; nIndex < nControls; nIndex++)
        {
            if (m_lControls.element[nIndex] == pOver)
            {
#ifndef _XBOX
                pManager->m_pMouseOverControl = NULL;
#endif
                pOver->HandleMouseOver(0);
                return;
            }
        }
    }
#endif
}

#ifndef _XBOX
void CSWGuiManager::MoveMouseToPosition(int nX, int nY)  // PC 0x0040D060
{
    m_nMouseX = nX;
    m_nMouseY = nY;
#ifndef _XBOX
    g_pExoInput->SetMousePos(nX, g_nScreenHeight - nY - 1, false);
#endif
    HandleMouseMove(nX, nY);
}
#endif

#ifndef _XBOX
void CSWGuiManager::RestoreOldMousePosition()  // PC 0x0040D0A0
{
    if (m_nOldMouseX >= 0)
    {
        m_nMouseX = m_nOldMouseX;
        m_nMouseY = m_nOldMouseY;
#ifndef _XBOX
        g_pExoInput->SetMousePos(m_nOldMouseX,
                                 g_nScreenHeight - m_nOldMouseY - 1, false);
#endif
        g_pAppManager->m_pClientExoApp->m_pcExoAppInternal
            ->RestoreMousePositionTo(m_nMouseX, m_nMouseY);
        m_nOldMouseX = -1;
        m_nOldMouseY = -1;
        HandleMouseMove(m_nMouseX, m_nMouseY);
    }
}
#endif
// PC 0x0040D100
int CSWGuiManager::RemovePanel(CSWGuiPanel* pPanel)
{
    if (pPanel == NULL)
    {
        return 0;
    }
    if (GetPosInPanelList(pPanel) == -1)
    {
        return 0;
    }
    int nModalPos = GetPosInModalStack(pPanel);

    if (nModalPos != -1)
    {
        if (nModalPos == m_lModalPanels.num - 1)
        {
            PopModalPanel();
        }
        else
        {
            m_lModalPanels.DelIndex(nModalPos);
        }
    }
    pPanel->OnPanelRemoved();
    int nPos = GetPosInPanelList(pPanel);

    if (nPos == -1)
    {
        return 0;
    }
    m_lPanels.DelIndex(nPos);
    if (pPanel->m_bModal)
    {
        m_nBackgroundPanels--;
    }
    DetermineNeedToDraw();
#ifndef _XBOX
    m_pMouseOverControl = NULL;
#endif
#ifndef _XBOX
    HandleMouseMove(m_nMouseX, m_nMouseY);
#endif
#ifndef _XBOX
    m_pMouseCapture = NULL;
#endif
    return 1;
}

#ifndef _XBOX
void CSWGuiManager::HandleMouseMove(int nX, int nY)  // PC 0x0040CAB0
{
    CSWGuiPanel* pPanel;
    CSWGuiControl* pControl;

    m_nMouseX = nX;
    m_nMouseY = nY;
    if (m_pMouseCursor != NULL)
    {
        ((Gob*)m_pMouseCursor)
            ->SetPosition(Vector((float)nX * 0.01f,
                                 (float)(m_nHeight - nY) * 0.01f, 0.0f));
    }
    if (m_pMouseCapture != NULL &&
        m_pMouseCapture->HandleMouseCapturedMovement(nX, nY) != 0)
    {
        return;
    }
    HitCheckMouse(nX, nY, &pPanel, &pControl, 0);
    CSWGuiControl* pHit = pControl;

    if (pHit != m_pMouseOverControl)
    {
        if (m_bToolTipShown)
        {
            RemoveToolTip();
        }
        SetTemporarilyDisableToolTip(0);
    }
    if (pHit == NULL)
    {
        CSWGuiControl* pOver = m_pMouseOverControl;

        if (pOver == NULL)
        {
            SetTemporarilyDisableToolTip(0);
            return;
        }
        CSWGuiControl* pParent;

        if (pOver->m_bActive && !pOver->m_bFocusable)
        {
            pOver->HandleMouseOver(0);
            pParent = m_pMouseOverControl->GetSelectableParent();
            if (pParent != NULL)
            {
                pParent->HandleMouseOver(0);
            }
        }
        else
        {
            pParent = pOver->GetSelectableParent();
            if (pParent != NULL && pParent->m_bActive &&
                !m_pMouseOverControl->m_bFocusable)
            {
                pParent->HandleMouseOver(0);
            }
        }
        m_pMouseOverControl = NULL;
        SetTemporarilyDisableToolTip(0);
        return;
    }
    if (m_bToolTipDisabled == 1 && pHit->AsListBox() == NULL &&
        pHit != m_pToolTipDisabledControl)
    {
        SetTemporarilyDisableToolTip(0);
    }
    CSWGuiControl* pPrev = m_pMouseOverControl;

    m_pMouseOverControl = pHit;
    if (pHit->AsRadioButton() == 0 && m_pMouseOverControl->AsEditBox() == 0)
    {
        CSWGuiPanel* pHitPanel = pPanel;
        CSWGuiControl* pCurrent = m_pMouseOverControl;

        if (pCurrent != pHitPanel->GetActiveControl())
        {
            if (pCurrent->GetIsSelectable() != 0 &&
                m_pMouseOverControl->m_bFocusable
#ifndef _XBOX
                && m_pMouseOverControl->m_bHilightOnOver
#endif
                )
            {
                if (pHitPanel->GetActiveControl() != NULL)
                {
                    pHitPanel->GetActiveControl()->HandleFocusChange(0);
                }
                m_pMouseOverControl->HandleFocusChange(1);
            }
            else
            {
                CSWGuiControl* pSelectable =
                    m_pMouseOverControl->GetSelectableParent();

                if (pSelectable != NULL && pSelectable->AsRadioButton() == 0 &&
                    pSelectable->AsEditBox() == 0)
                {
                    if (pSelectable->m_bFocusable
#ifndef _XBOX
                        && pSelectable->m_bHilightOnOver
#endif
                        )
                    {
                        if (pSelectable != pHitPanel->GetActiveControl())
                        {
                            pSelectable->HandleFocusChange(1);
                        }
                    }
                    else if (!pSelectable->m_bFocusable
#ifndef _XBOX
                             && pSelectable->m_bHilightOnOver
#endif
                             )
                    {
                        pSelectable->HandleMouseOver(1);
                    }
                }
            }
        }
        pHit = pControl;
    }
    CSWGuiControl* pNow = m_pMouseOverControl;
    int bHilighted = 0;

    if (pPrev != pNow)
    {
        if (pNow->GetIsSelectable() != 0)
        {
            m_pMouseOverControl->HandleMouseOver(1);
#ifndef _XBOX
            if (m_pMouseOverControl->m_bFocusable &&
                m_pMouseOverControl->m_bHilightOnOver)
            {
                bHilighted = 1;
            }
#endif
        }
        CSWGuiControl* pWalk = m_pMouseOverControl;

        do
        {
#ifndef _XBOX
            pWalk = pWalk->m_pParent;
#endif
            if (pWalk != NULL && pWalk != pPrev &&
                pWalk->GetIsSelectable() != 0)
            {
                pWalk->HandleMouseOver(1);
#ifndef _XBOX
                if (pWalk->m_bFocusable && pWalk->m_bHilightOnOver)
                {
                    bHilighted = true;
                }
#endif
            }
        } while (pWalk != NULL);
        if (bHilighted)
        {
            if (pPrev != NULL)
            {
#ifndef _XBOX
                if (pPrev->m_pParent == NULL &&
                    pPrev->GetIsChild(m_pMouseOverControl) == 0)
                {
                    pPrev->HandleMouseOver(0);
                }
#endif
                if (pPrev->GetSelectableParent() != NULL)
                {
                    pPrev->HandleMouseOver(0);
                }
            }
        }
        else if (pPrev != NULL)
        {
            CSWGuiControl* pSelectable;

            if (pPrev->m_bActive && !pPrev->m_bFocusable)
            {
                pPrev->HandleMouseOver(0);
                pSelectable = pPrev->GetSelectableParent();
                if (pSelectable != NULL && pSelectable != m_pMouseOverControl &&
                    pSelectable != m_pMouseOverControl->GetSelectableParent())
                {
                    pSelectable->HandleMouseOver(0);
                }
            }
            else
            {
                pSelectable = pPrev->GetSelectableParent();
                if (pSelectable != NULL && pSelectable->m_bActive &&
                    !pSelectable->m_bFocusable)
                {
                    if (pSelectable != m_pMouseOverControl &&
                        pSelectable !=
                            m_pMouseOverControl->GetSelectableParent())
                    {
                        pSelectable->HandleMouseOver(0);
                    }
                }
            }
        }
        pHit = pControl;
    }
    CSWGuiListBox* pListBox = pHit->AsListBox();

    if (pListBox != NULL)
    {
        if (m_bToolTipDisabled == 1 &&
            pListBox->HitCheckMouseLocal(NULL) != m_pToolTipDisabledControl)
        {
            SetTemporarilyDisableToolTip(0);
        }
        pListBox->HandleMouseMove();
    }
    return;
}
#endif

#ifndef _XBOX
int CSWGuiManager::HandleLMouseDown(int bDown)  // PC 0x0040CE40
{
    m_bLeftMouseDown = bDown;
    if (m_bToolTipShown)
    {
        RemoveToolTip();
    }
    if (m_pMouseCapture != NULL && m_nMouseCaptureType == 1)
    {
        return 0;
    }
    if (g_pAppManager->m_pClientExoApp->GetInputClass() == 3)
    {
        for (int i = m_lPanels.num - 1; i >= 0; i--)
        {
            m_lPanels.element[i]->HandleInputEvent(505, 1);
        }
    }
    UpdateMouseOverControl();
    SetTemporarilyDisableToolTip(1);
    CSWGuiControl* pControl = m_pMouseOverControl;

    if (pControl != NULL)
    {
        pControl->HandleLMouseDown();
        return 1;
    }
    return 0;
}
#endif

#ifndef _XBOX
int CSWGuiManager::HandleRMouseDown()  // PC 0x0040CEE0
{
    if (m_bToolTipShown)
    {
        RemoveToolTip();
    }
    if (m_pMouseCapture != NULL)
    {
        return 0;
    }
    UpdateMouseOverControl();
    SetTemporarilyDisableToolTip(1);
    CSWGuiControl* pControl = m_pMouseOverControl;

    if (pControl != NULL)
    {
        pControl->HandleRMouseDown();
        return 1;
    }
    return 0;
}
#endif

#ifndef _XBOX
int CSWGuiManager::HandleZMouse(int nDelta)  // PC 0x0040CF20
{
    CSWGuiPanel* pPanel;
    CSWGuiControl* pControl;

    if (m_bToolTipShown)
    {
        RemoveToolTip();
    }
    HitCheckMouse(m_nMouseX, m_nMouseY, &pPanel, &pControl, 0);
    CSWGuiControl* pParent = pControl;

    if (pControl != m_pMouseOverControl)
    {
        m_pMouseOverControl = pControl;
    }
    if (pParent == NULL)
    {
        return 0;
    }
    if (pParent->AsListBox() != NULL)
    {
        if (nDelta > 0)
        {
            while (nDelta > 0)
            {
                pParent->AsListBox()->HandleInputEvent(GUIEVENT_MOUSE_WHEEL_UP, 1);
                nDelta -= 120;
            }

            if (m_bToolTipDisabled == 1)
            {
                SetTemporarilyDisableToolTip(0);
            }
            pParent->AsListBox()->HandleMouseMove();
        }
        else if (nDelta < 0)
        {
            while (nDelta < 0)
            {
                pParent->AsListBox()->HandleInputEvent(GUIEVENT_MOUSE_WHEEL_DOWN, 1);
                nDelta += 120;
            }

            if (m_bToolTipDisabled == 1)
            {
                SetTemporarilyDisableToolTip(0);
            }
            pParent->AsListBox()->HandleMouseMove();
        }
        else
        {
            return 1;
        }
    }
    else
    {
        if (pParent->GetIsSelectable() == 0)
        {
            pParent = pParent->GetSelectableParent();
        }
        if (pParent != NULL && pParent->AsButton() != 0 && nDelta != 0)
        {
            pParent->HandleInputEvent((nDelta <= 0) + GUIEVENT_MOUSE_WHEEL_UP, 1);
        }
    }
    return 1;
}
#endif

#ifndef _XBOX
void CSWGuiManager::RemoveToolTip()  // PC 0x0040CA20
{
    SwguiTrace2();
    m_bToolTipShown = 0;
    ResetToolTipTimer();
    if (m_pToolTipControl != NULL)
    {
#ifndef _XBOX
        m_pToolTipControl->m_bToolTipShown = 0;
#endif
        m_pToolTipControl = NULL;
    }
}
#endif

#ifndef _XBOX
void CSWGuiManager::ResetToolTipTimer()  // PC 0x0040B6D0
{
    if (m_bToolTipDisabled == 0)
    {
        if (g_pAppManager->m_pClientExoApp->GetClientOptions()
                ->m_bEnableTooltips)
        {
            m_fToolTipTimer = 0;
        }
    }
    return;
}
#endif

#ifndef _XBOX
void CSWGuiManager::SetTemporarilyDisableToolTip(int bDisable)  // PC 0x0040B700
{
    if (bDisable != 0)
    {
        m_fToolTipTimer = -1.0f;
        if (m_pMouseOverControl != NULL)
        {
            CSWGuiControl* pControl;

            if (m_pMouseOverControl->AsListBox() != NULL)
            {
                pControl =
                    m_pMouseOverControl->AsListBox()->HitCheckMouseLocal(NULL);
            }
            else
            {
                pControl = m_pMouseOverControl;
            }
            m_bToolTipDisabled = bDisable;
            m_pToolTipDisabledControl = pControl;
            return;
        }
    }
    else
    {
        ResetToolTipTimer();
    }
    m_bToolTipDisabled = bDisable;
    m_pToolTipDisabledControl = NULL;
}
#endif
// PC 0x0040B7C0
CSWGuiPanel::CSWGuiPanel(CSWGuiManager* pManager)
    : m_lControls(0)
{
    m_pBorder = new CSWGuiBorder();
    m_pManager = pManager;
    m_pActiveControl = NULL;
    m_pLayout = NULL;
#ifndef _XBOX
    m_nBackground = 0;
    m_bPositioned = 1;
#endif
    m_bLoadingLayout = 0;
    m_bVisible = 1;
    m_bCenterHorizontal = 0;
    m_bCenterVertical = 0;
    m_bNeedsDraw = 1;
    m_bDeferredRemoval = 0;
    m_nExitState = 0;
    m_fAlpha = 1.0f;
    m_nOpenSound = -1;
#ifndef _XBOX
    m_bUseFont = 1;
#endif
}
#ifndef _XBOX
CSWGuiControl* CSWGuiPanel::HitCheckMouse(int nX, int nY)  // PC 0x0040B8E0
{
    CSWGuiExtent cExtent = m_cExtent;
#ifndef _XBOX
    if (!(m_bPositioned))
    {
        nX += (g_nScreenWidth - 640) / -2;
        nY += (g_nScreenHeight - 480) / -2;
    }
    else
#endif
    {
        GetExtentAccountingForPanelOffset(&cExtent);
    }
    if (nX >= cExtent.m_nLeft && nX <= cExtent.m_nLeft + cExtent.m_nWidth &&
        nY >= cExtent.m_nTop && nY <= cExtent.m_nTop + cExtent.m_nHeight)
    {
        int nLocalX = nX - cExtent.m_nLeft;
        int nLocalY = nY - cExtent.m_nTop;

        for (int i = m_lControls.num - 1; i >= 0; i--)
        {
            CSWGuiControl* pControl = m_lControls.element[i];

            if (pControl != NULL)
            {
                CSWGuiControl* pHit = pControl->HitCheckMouse(nLocalX, nLocalY);

                if (pHit != NULL)
                {
                    return pHit;
                }
            }
        }
    }
    return NULL;
}
#endif
void CSWGuiPanel::Draw(float fElapsed)  // PC 0x0040B9B0
{
    if (!m_bNeedsDraw)
    {
        return;
    }
    if (m_cExtent.m_nLeft < 0)
    {
        return;
    }
    if (m_cExtent.m_nTop < 0)
    {
        return;
    }
    if (m_cExtent.m_nHeight <= 0)
    {
        return;
    }
    if (m_cExtent.m_nWidth <= 0)
    {
        return;
    }
    CSWGuiExtent* pExtent = &m_cExtent;
    if (pExtent->m_nLeft + pExtent->m_nWidth > m_pManager->m_nWidth)
    {
        return;
    }
    if (pExtent->m_nTop + pExtent->m_nHeight > m_pManager->m_nHeight)
    {
        return;
    }

    int nControls = m_lControls.num;
    CSWGuiExtent cExtent = *pExtent;

#ifndef _XBOX
    GetExtentAccountingForPanelOffset(&cExtent);
#endif
    AurGUIStartLayer();
    if (AurGUISetupViewport(cExtent.m_nLeft, cExtent.m_nTop,
                            cExtent.m_nWidth, cExtent.m_nHeight,
                            m_vColor, false, m_fAlpha) != 0)
    {
        m_pBorder->Draw(fElapsed);
        for (int i = 0; i < nControls; i++)
        {
            CSWGuiControl* pControl = m_lControls.element[i];

            if (pControl != NULL && pControl->m_bEnabled)
            {
                pControl->Draw(fElapsed);
            }
        }
        AurGUICloseViewport();
    }
    AurGUIStopLayer();
}
// PC 0x0040BAC0
void CSWGuiPanel::OnPanelAdded()
{
    ResetFont();
#ifndef _XBOX
    g_pGuiMan->UpdateMouseOverControl();
    CSWGuiControl* pOver = GetManager()->m_pMouseOverControl;
#endif

#ifndef _XBOX
    if (pOver != NULL)
    {
        for (int i = 0; i < m_lControls.num; i++)
        {
            if (m_lControls.element[i] == pOver)
            {
#ifndef _XBOX
                if (pOver->m_bHilightOnOver)
                {
                    GetManager()->m_pMouseOverControl = pOver;
                    pOver->HandleMouseOver(1);
                    if (m_pActiveControl != NULL && pOver != m_pActiveControl)
                    {
                        if (pOver->AsButton() != 0 && pOver->m_bFocusable)
                        {
                            m_pActiveControl->HandleInputEvent(1, 1);
                        }
                    }
                }
#endif
                return;
            }
        }
    }
#endif
}
// PC 0x0040BB40
void CSWGuiPanel::StopLoadFromLayout()
{
    if (m_bLoadingLayout)
    {
        m_pLayout->Release();
        if (m_pLayout != NULL)
        {
            delete m_pLayout;
        }
        m_pLayout = NULL;
        m_bLoadingLayout = 0;
        ConnectControlMoves();
    }
}
// PC 0x0040BB80
void CSWGuiPanel::InitControl(CSWGuiControl& cControl, const CExoString& sName,
                              int bRequired, int bScale)
{
    cControl.Load(this, m_pLayout, m_lLayoutControls, sName);
    CSWGuiExtent cExtent = cControl.m_cExtent;
    float fScaleX = 1.0f;
    float fScaleY = 1.0f;
    if (bScale && g_pAppManager->m_pClientExoApp->GetSWGuiManager() != NULL)
    {
        fScaleX = (float)g_pAppManager->m_pClientExoApp->GetSWGuiManager()->m_nWidth / (float)g_nGUIWidth;
        fScaleY = (float)g_pAppManager->m_pClientExoApp->GetSWGuiManager()->m_nHeight / (float)g_nGUIHeight;
    }
    cExtent.m_nHeight = (int)(cExtent.m_nHeight * fScaleY);
    cExtent.m_nWidth = (int)(cExtent.m_nWidth * fScaleX);
    cExtent.m_nTop = (int)(cExtent.m_nTop * fScaleY);
    cExtent.m_nLeft = (int)(cExtent.m_nLeft * fScaleX);
    cControl.SetExtent(cExtent);
    if (bRequired != 0)
    {
        int nIndex = cControl.m_nID;

        if (nIndex >= 0)
        {
            if (nIndex < m_lControls.num)
            {
                m_lControls.element[nIndex] = &cControl;
            }
            else
            {
                if (nIndex > m_lControls.num)
                {
                    do
                    {
                        m_lControls.Add(NULL);
                    } while (nIndex > m_lControls.num);
                }
                m_lControls.Add(&cControl);
            }
        }
    }
}
// PC 0x0040BD00
void CSWGuiPanel::AddControl(CSWGuiControl* pControl)
{
    if (m_lControls.IndexOf(pControl) == -1)
    {
        pControl->m_pOwner = this;
        m_lControls.Add(pControl);
    }
}
// PC 0x0040BD30
void CSWGuiPanel::SetVisible(int bVisible)
{
    m_bVisible = bVisible;
    if (m_pManager != NULL)
    {
        m_pManager->DetermineNeedToDraw();
    }
}
// PC 0x0040BD60
CSWGuiControl* CSWGuiPanel::GetControl(int nIndex)
{
    if (nIndex < m_lControls.num)
    {
        return m_lControls.element[nIndex];
    }
    return NULL;
}

#ifndef _XBOX
void CSWGuiPanel::GetLocalMouseCoords(int* pnX, int* pnY)  // PC 0x0040BD80
{
    CSWGuiExtent cExtent;

    *pnX = g_pGuiMan->m_nMouseX;
    *pnY = g_pGuiMan->m_nMouseY;
    cExtent = m_cExtent;
#ifndef _XBOX
    if (!(m_bPositioned))
    {
        *pnX += (g_nScreenWidth - 640) / -2;
        *pnY += (g_nScreenHeight - 480) / -2;
        *pnX -= cExtent.m_nLeft;
        *pnY -= cExtent.m_nTop;
    }
    else
#endif
    {
        GetExtentAccountingForPanelOffset(&cExtent);
        *pnX -= cExtent.m_nLeft;
        *pnY -= cExtent.m_nTop;
    }
}
#endif

enum SWGuiDispatchEvent
{
    GUIEVENT_VERTICAL_AXIS = 51,
    GUIEVENT_HORIZONTAL_AXIS = 52,
    GUIEVENT_VERTICAL_SCROLL_AXIS = 55,
    GUIEVENT_HORIZONTAL_SCROLL_AXIS = 56,
    GUIEVENT_SCROLL_LEFT = 59,
    GUIEVENT_SCROLL_RIGHT = 60,
    GUIEVENT_CANCEL_SHORTCUT = 180,
    GUIEVENT_ACTIVATE_SHORTCUT = 181,
    GUIEVENT_UP_SHORTCUT = 182,
    GUIEVENT_DOWN_SHORTCUT = 183,
    GUIEVENT_LEFT_SHORTCUT = 184,
    GUIEVENT_RIGHT_SHORTCUT = 185,
    GUIEVENT_ACTIVATE_SHORTCUT_ALTERNATE = 187
};

enum SWGuiInputState
{
    GUIINPUT_RELEASED = 0,
    GUIINPUT_PRESSED = 1,
    GUIINPUT_AXIS_POSITIVE = 1,
    GUIINPUT_AXIS_NEGATIVE = -1
};
// PC 0x0040D1B0
void CSWGuiManager::HandleInputEvent(int nEvent, int nState)
{
    int nIndex;
    m_nLastInputEvent = nEvent;
    if (nState != GUIINPUT_RELEASED)
    {
        switch (nEvent)
        {
        case GUIEVENT_VERTICAL_AXIS:
            if (nState == GUIINPUT_AXIS_POSITIVE)
            {
                nEvent = GUIEVENT_DOWN_ALTERNATE;
            }
            else if (nState == GUIINPUT_AXIS_NEGATIVE)
            {
                nEvent = GUIEVENT_UP_ALTERNATE;
            }
            break;
        case GUIEVENT_HORIZONTAL_AXIS:
            if (nState == GUIINPUT_AXIS_POSITIVE)
            {
                nEvent = GUIEVENT_RIGHT_ALTERNATE;
            }
            else if (nState == GUIINPUT_AXIS_NEGATIVE)
            {
                nEvent = GUIEVENT_LEFT_ALTERNATE;
            }
            break;
        case GUIEVENT_VERTICAL_SCROLL_AXIS:
            if (nState == GUIINPUT_AXIS_POSITIVE)
            {
                nEvent = GUIEVENT_SCROLL_DOWN;
            }
            else if (nState == GUIINPUT_AXIS_NEGATIVE)
            {
                nEvent = GUIEVENT_SCROLL_UP;
            }
            break;
        case GUIEVENT_HORIZONTAL_SCROLL_AXIS:
            if (nState == GUIINPUT_AXIS_POSITIVE)
            {
                nEvent = GUIEVENT_SCROLL_RIGHT;
            }
            else if (nState == GUIINPUT_AXIS_NEGATIVE)
            {
                nEvent = GUIEVENT_SCROLL_LEFT;
            }
            break;
        default:
            break;
        }
        nState = GUIINPUT_PRESSED;
        unsigned long nTime = g_pExoBase->m_pExoTimers->GetLowResolutionTimer();

        switch (nEvent)
        {
        case GUIEVENT_UP:
        case GUIEVENT_DOWN:
            if (m_nLastDirection == 0)
            {
                m_nLastDirection = nEvent;
            }
            else if (m_nLastDirection > GUIEVENT_LEFT - 1 &&
                     m_nLastDirection <= GUIEVENT_RIGHT)
            {
                if (nTime - m_nLastKeyTime < DIRECTION_CHANGE_DELAY)
                {
                    return;
                }
                m_nLastDirection = nEvent;
            }
            m_nLastKeyTime = nTime;
            break;
        case GUIEVENT_LEFT:
        case GUIEVENT_RIGHT:
            if (m_nLastDirection == 0)
            {
                m_nLastDirection = nEvent;
            }
            else if (m_nLastDirection > GUIEVENT_UP - 1 &&
                     m_nLastDirection <= GUIEVENT_DOWN)
            {
                if (nTime - m_nLastKeyTime < DIRECTION_CHANGE_DELAY)
                {
                    return;
                }
                m_nLastDirection = nEvent;
            }
            m_nLastKeyTime = nTime;
            break;
        default:
            break;
        }
    }
#ifndef _XBOX
    switch (nEvent)
    {
    case GUIEVENT_CANCEL_SHORTCUT:
    case GUIEVENT_EXIT:
        if (nState != GUIINPUT_RELEASED)
        {
            nEvent = GUIEVENT_CANCEL;
        }
        break;
    case GUIEVENT_ACTIVATE_SHORTCUT:
    case GUIEVENT_ACTIVATE_SHORTCUT_ALTERNATE:
        nEvent = GUIEVENT_ACTIVATE; break;
    case GUIEVENT_UP_SHORTCUT:
        nEvent = GUIEVENT_UP_ALTERNATE;
        break;
    case GUIEVENT_DOWN_SHORTCUT:
        nEvent = GUIEVENT_DOWN_ALTERNATE;
        break;
    case GUIEVENT_LEFT_SHORTCUT:
        nEvent = GUIEVENT_LEFT_ALTERNATE;
        break;
    case GUIEVENT_RIGHT_SHORTCUT:
        nEvent = GUIEVENT_RIGHT_ALTERNATE;
        break;
    default: break;
    }
#endif
    if (m_lModalPanels.num != 0)
    {
        m_lModalPanels.element[m_lModalPanels.num - 1]->HandleInputEvent(
            nEvent, nState);
    }
    else
    {
        CExoArrayList<CSWGuiPanel*> lPanels(0);
        lPanels.SetSize(m_lPanels.num);
        for (nIndex = 0; nIndex < m_lPanels.num; nIndex++)
        {
            lPanels.element[nIndex] = m_lPanels.element[nIndex];
        }
        nIndex = 0;
        if (lPanels.num > 0)
        {
            do
            {
                CSWGuiPanel* pPanel = lPanels.element[nIndex];

                if (m_lPanels.IndexOf(pPanel) != -1)
                {
                    pPanel->HandleInputEvent(nEvent, nState);
                }
                nIndex++;
            } while (nIndex < lPanels.num);
        }
    }
#ifdef _XBOX
    for (nIndex = m_lPanels.num - 1; nIndex >= 0; nIndex--)
#else
    int nPanels = m_lPanels.num; for (nIndex = nPanels - 1; nIndex >= 0; nIndex--)
#endif
    {
        CSWGuiPanel* pPanel = m_lPanels.element[nIndex];
        if (pPanel->m_nExitState == CSWGuiPanel::EXITSTATE_DELETE)
        {
            RemovePanel(pPanel); delete pPanel;
        }
        else if (pPanel->m_nExitState != 0)
        {
            pPanel->m_bDeferredRemoval = 0; pPanel->m_nExitState = 0;
            RemovePanel(pPanel);
        }
    }
    if (g_pExoResMan->m_bMainMenuImageLoaded) {
#ifdef _XBOX
        g_pExoResMan->RemoveResourceImageFile("RIMSXBOX:MAINMENU"); g_pExoResMan->RemoveResourceImageFile("RIMSXBOX:MAINMENUDX");
#else
        g_pExoResMan->RemoveResourceImageFile("RIMS:MAINMENU");
#endif
        g_pExoResMan->m_bMainMenuImageLoaded = 0; }
    if (g_pExoResMan->m_bCharGenImageLoaded) {
#ifdef _XBOX
        g_pExoResMan->RemoveResourceImageFile("RIMSXBOX:CHARGEN"); g_pExoResMan->RemoveResourceImageFile("RIMSXBOX:CHARGENDX");
#else
        g_pExoResMan->RemoveResourceImageFile("RIMS:CHARGEN");
#endif
        g_pExoResMan->m_bCharGenImageLoaded = 0; }
}
// PC 0x0040D520
void CSWGuiManager::Draw(float fElapsed)
{

    int nPanels = m_lPanels.num;

    AurMakeCurrent();
    CSWGuiListBox::UpdateArrowPulsing(fElapsed);
    CSWGuiDrawable::UpdatePulsing(fElapsed);
    for (int i = 0; i < nPanels; i++)
    {
        CSWGuiPanel* pPanel = m_lPanels.element[i];

        if (pPanel != NULL)
        {
            if ((pPanel->m_bVisible) && GetPosInModalStack(pPanel) == -1)
            {
                pPanel->Draw(fElapsed);
            }
        }
        else
        {
            m_lPanels.DelIndex(i--);
        }
    }
    int nModalPanels = m_lModalPanels.num;

    for (int k = 0; k < nModalPanels; k++)
    {
        CSWGuiPanel* pPanel = m_lModalPanels.element[k];

        if (pPanel != NULL)
        {
            if (pPanel->m_bVisible)
            {
                pPanel->Draw(fElapsed);
            }
        }
        else
        {
            m_lModalPanels.DelIndex(k--);
        }
    }
#ifndef _XBOX
    if (m_pDragControl != NULL)
    {
        AurGUIStartLayer();
        Vector vColor(-1.0f, -1.0f, -1.0f);

        if (AurGUISetupViewport(0, 0, m_nWidth, m_nHeight, vColor, false,
                                1.0f) != 0)
        {
            m_pDragControl->DrawDragDrop(m_nMouseX, m_nMouseY);
            AurGUICloseViewport();
        }
        AurGUIStopLayer();
    }
#endif
    if (g_pAppManager->m_pClientExoApp->GetInFreeLook() == 0)
    {
#ifndef _XBOX
        if (m_bToolTipShown)
        {
            m_pToolTipPanel->Draw(fElapsed);
        }
        if (m_fToolTipTimer >= 0.0f)
        {
            m_fToolTipTimer = fElapsed + m_fToolTipTimer;
            if (m_fToolTipTimer > m_fToolTipDelay)
            {
                CSWGuiControl* pOver = m_pMouseOverControl;

                if (pOver != NULL && pOver->DisplayToolTip() != 0)
                {
                    m_pToolTipControl = m_pMouseOverControl;
                }
                m_fToolTipTimer = -1.0f;
            }
        }
#endif
    }
#ifndef _XBOX
    if (m_pMouseScene != NULL)
    {
        AurGUIStartLayer();
        m_pMouseScene->Draw(fElapsed);
        AurGUIStopLayer();
    }
#endif
    for (int j = m_lPanels.num - 1; j >= 0; j--)
    {
        CSWGuiPanel* pPanel = m_lPanels.element[j];

        if (pPanel->m_nExitState == CSWGuiPanel::EXITSTATE_DELETE)
        {
            RemovePanel(pPanel);
            delete pPanel;
        }
        else if (pPanel->m_nExitState != 0)
        {
            pPanel->m_bDeferredRemoval = 0;
            pPanel->m_nExitState = 0;
            RemovePanel(pPanel);
        }
    }
    if (g_bDrawSafeRegion != 0)
    {
        DrawSafeRegion(fElapsed);
    }
}
// PC 0x0040D740
void CSWGuiManager::Update(float fElapsed)
{

    int nPanels = m_lPanels.num;
    int i = 0;
    if (nPanels > 0)
    {
        CExoArrayList<CSWGuiPanel*>* pPanels = &m_lPanels;
        do
        {
            CSWGuiPanel* pPanel = pPanels->element[i];
            if (pPanel != NULL)
            {
                if (pPanel->m_bVisible && GetPosInModalStack(pPanels->element[i]) == -1)
                {
                    pPanel->Update(fElapsed);
                }
            }
            else
            {
                pPanels->DelIndex(i--);
            }
            ++i;
        }
        while (i < nPanels);
    }
    int nModalPanels = m_lModalPanels.num;
    for (int k = 0; k < nModalPanels; ++k)
    {
        CSWGuiPanel* pPanel = m_lModalPanels.element[k];
        if (pPanel != NULL)
        {
            if (pPanel->m_bVisible)
            {
                pPanel->Update(fElapsed);
            }
        }
        else
        {
            m_lModalPanels.DelIndex(k--);
        }
    }
    for (int j = m_lPanels.num - 1; j >= 0; --j)
    {
        CSWGuiPanel* pPanel = m_lPanels.element[j];
        if (!pPanel->m_bDeferredRemoval)
        {
            unsigned long nExitState = pPanel->m_nExitState;
            if (nExitState == CSWGuiPanel::EXITSTATE_DELETE)
            {
                RemovePanel(pPanel);
                delete pPanel;
            }
            else if (nExitState != 0)
            {
                pPanel->m_bDeferredRemoval = 0;
                pPanel->m_nExitState = 0;
                RemovePanel(pPanel);
            }
        }
    }
}
// PC 0x0040D830
CSWGuiPanel::~CSWGuiPanel()
{
    if (m_pBorder != NULL)
    {
        delete m_pBorder;
    }
    m_pBorder = NULL;
    StopLoadFromLayout();
    if (m_pLayout != NULL)
    {
        delete m_pLayout;
    }
    CSWGuiManager* pManager = m_pManager;

    m_pLayout = NULL;
    if (pManager != NULL && pManager->PanelExists(this) != 0)
    {
        pManager->RemovePanel(this);
    }
}

#ifdef _XBOX

CSWGuiControl* CSWGuiPanel::HitCheckMouse(int nX, int nY)
{
    if (nX >= m_cExtent.m_nLeft && nX <= m_cExtent.GetRight() &&
        nY >= m_cExtent.m_nTop && nY <= m_cExtent.GetBottom())
    {
        int nLocalX = nX - m_cExtent.m_nLeft;
        int nLocalY = nY - m_cExtent.m_nTop;
        for (int i = m_lControls.num - 1; i >= 0; i--)
        {
            if (m_lControls.element[i]->HitCheckMouse(nLocalX, nLocalY))
            {
                return m_lControls.element[i];
            }
        }
    }
    return NULL;
}
#endif
