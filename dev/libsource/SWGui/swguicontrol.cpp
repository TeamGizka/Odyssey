#include "aurgui.h"
#include "exotlktable.h"
#include "twodimarray.h"
#include "resgff.h"
#include "exoinput.h"
#include "swgui.h"

class CSWGuiControl;
class CSWGuiExtent;
struct CSWGuiControlEvent;

template void CExoArrayList<CSWGuiControl*>::Allocate(int);

#include "appmanager.h"

#include "clientexoappinternal.h"
#include "exoinputinternal.h"
#include "clientexoapp.h"
#include "nwrules.h"
#include "twodimarrays.h"
#include "aurora.h"
#include "aurora.h"

float CSWGuiListBox::m_fArrowAlpha = 1.0f;
float CSWGuiListBox::m_fArrowAlphaPulseTime = 0.75f;
bool CSWGuiListBox::m_bArrowAlphaPulseUp;

static inline void SwguicontrolTrace1();
// PC 0x00417660  Xbox 0x002FCAA0
void CSWGuiNavigable::SetMoveToControl(int nDirection, CSWGuiControl* pControl)
{
    if (nDirection >= 0 && nDirection < 4)
    {
        m_apMoveToControl[nDirection] = pControl;
    }
}
// PC 0x00417680  Xbox 0x002FCAC0
int CSWGuiNavigable::GetMoveToControl(int nDirection)
{
    if (nDirection >= 0 && nDirection < 4)
    {
        return (int)m_apMoveToControl[nDirection];
    }
    return 0;
}
// PC 0x004015A0
void CSWGuiControl::HandleLoseMouseFocus()
{
}
// PC 0x004176A0
void CSWGuiControl::SetEnabled(int bEnabled)
{
#ifndef _XBOX
    m_bHilightOnOver = bEnabled;
#endif
}
// PC 0x004176C0
void CSWGuiControl::HideToolTip()
{
#ifndef _XBOX
    g_pGuiMan->RemoveToolTip();
#endif
#ifndef _XBOX
    m_bToolTipShown = 0;
#endif
}
// PC 0x004176E0  Xbox 0x002FCB20
#ifdef _XBOX
void CSWGuiLabel::Initialize(const CSWGuiExtent& cExtent,
                             const CSWGuiTextParams& cText,
                             const CSWGuiBorderParams& cBorder)
#else
void CSWGuiLabel::Initialize(const CSWGuiExtent& cExtent,
                             const CSWGuiTextParams& cText,
                             const CSWGuiBorderParams& cBorder, float fScale)
#endif
{
    m_cExtent = cExtent;
    m_cBorder.Initialize(m_cExtent, cBorder);
    CSWGuiExtent cInner = m_cBorder.GetInnerExtent();
#ifdef _XBOX
    m_cText.Initialize(cInner, cText);
#else
    m_cText.Initialize(cInner, cText, fScale);
#endif
}
// PC 0x00417750  Xbox 0x002FCB80
void CSWGuiLabel::Draw(float fElapsed)
{
    m_cBorder.Draw(fElapsed);
    m_cText.Draw(fElapsed);
}
// PC 0x00417780  Xbox 0x002FCC70
void CSWGuiLabel::SetExtent(const CSWGuiExtent& cExtent)
{
    m_cExtent = cExtent;
    m_cBorder.SetExtent(m_cExtent);
    CSWGuiExtent cInner = m_cBorder.GetInnerExtent();
    m_cText.SetExtent(cInner);
}
// PC 0x004177E0  Xbox 0x002FCCD0
void CSWGuiLabelHilight::Initialize(const CSWGuiExtent& cExtent,
                                    const CSWGuiTextParams& cText,
                                    const CSWGuiBorderParams& cBorder,
                                    const CSWGuiBorderParams& cHilite)
{
    m_cExtent = cExtent;
    m_cBorder.Initialize(m_cExtent, cBorder);
    m_cHilite.Initialize(m_cExtent, cHilite);
    CSWGuiExtent cInner = CSWGuiExtent::Intersection(
        m_cBorder.GetInnerExtent(), m_cHilite.GetInnerExtent());
    m_cText.Initialize(cInner, cText, 1.0f);
}
// PC 0x00417880  Xbox 0x002FCD70
void CSWGuiLabelHilight::Draw(float fElapsed)
{
    if (m_bActive)
    {
        m_cHilite.Draw(fElapsed);
    }
    else
    {
        m_cBorder.Draw(fElapsed);
    }
    m_cText.Draw(fElapsed);
}
// PC 0x004178E0  Xbox 0x002FCDD0
void CSWGuiLabelHilight::SetExtent(const CSWGuiExtent& cExtent)
{
    m_cExtent = cExtent;
    m_cBorder.SetExtent(m_cExtent);
    m_cHilite.SetExtent(m_cExtent);
    CSWGuiExtent cInner = CSWGuiExtent::Intersection(
        m_cBorder.GetInnerExtent(), m_cHilite.GetInnerExtent());
    m_cText.SetExtent(cInner);
}
// PC 0x00417970  Xbox 0x002FCEE0
void CSWGuiButton::Initialize(const CSWGuiExtent& cExtent,
                              const CSWGuiTextParams& cText,
                              const CSWGuiBorderParams& cBorder,
                              const CSWGuiBorderParams& cHilite)
{
    m_cExtent = cExtent;
    m_cBorder.Initialize(m_cExtent, cBorder);
    m_cHilite.Initialize(m_cExtent, cHilite);
    CSWGuiExtent cBorderInner = m_cBorder.GetInnerExtent();
    CSWGuiExtent cHiliteInner = m_cHilite.GetInnerExtent();
    CSWGuiExtent cInner = CSWGuiExtent::Intersection(
        m_cBorder.GetInnerExtent(), m_cHilite.GetInnerExtent());
    m_cText.Initialize(cInner, cText, 1.0f);
}
// PC 0x00417A20  Xbox 0x002FCF90
void CSWGuiButton::SetExtent(const CSWGuiExtent& cExtent)
{
    m_cExtent = cExtent;
    m_cBorder.SetExtent(cExtent);
    m_cHilite.SetExtent(cExtent);
    m_cText.SetExtent(CSWGuiExtent::Intersection(m_cBorder.GetInnerExtent(),
                                                 m_cHilite.GetInnerExtent()));
}
// PC 0x00417AB0  Xbox 0x002FD020
void CSWGuiButton::Draw(float fElapsed)
{
    if (m_bActive)
    {
        m_cHilite.Draw(fElapsed);
    }
    else
    {
        m_cBorder.Draw(fElapsed);
    }
    m_cText.Draw(fElapsed);
}
// PC 0x00417B10  Xbox 0x002FD080
void CSWGuiButtonToggle::Draw(float fElapsed)
{
    if (m_bToggled)
    {
        if (m_bActive)
        {
            m_cToggleHilite.Draw(fElapsed);
        }
        else
        {
            m_cToggleBorder.Draw(fElapsed);
        }
    }
    else
    {
        if (m_bActive)
        {
            m_cHilite.Draw(fElapsed);
        }
        else
        {
            m_cBorder.Draw(fElapsed);
        }
    }
    m_cText.Draw(fElapsed);
}
// PC 0x00417BA0  Xbox 0x002FD110
void CSWGuiButtonToggle::SetExtent(const CSWGuiExtent& cExtent)
{
    m_cExtent = cExtent;
    m_cBorder.SetExtent(cExtent);
    m_cHilite.SetExtent(cExtent);
    m_cToggleBorder.SetExtent(cExtent);
    m_cToggleHilite.SetExtent(cExtent);
    CSWGuiExtent cBorderInner = m_cBorder.GetInnerExtent();
    CSWGuiExtent cHiliteInner = m_cHilite.GetInnerExtent();
    CSWGuiExtent cToggleBorderInner = m_cToggleBorder.GetInnerExtent();
    CSWGuiExtent cToggleHiliteInner = m_cToggleHilite.GetInnerExtent();
    CSWGuiExtent::Intersection(
        CSWGuiExtent::Intersection(cBorderInner, cHiliteInner),
        CSWGuiExtent::Intersection(cToggleBorderInner, cToggleHiliteInner));
    m_cText.SetExtent(cExtent);
}
// PC 0x00417CA0  Xbox 0x002FD210
void CSWGuiButtonToggle::Initialize(const CSWGuiExtent& cExtent,
                                    const CSWGuiTextParams& cText,
                                    const CSWGuiBorderParams& cBorder,
                                    const CSWGuiBorderParams& cHilite,
                                    const CSWGuiBorderParams& cToggleBorder,
                                    const CSWGuiBorderParams& cToggleHilite)
{
    m_cExtent = cExtent;
    m_cBorder.Initialize(m_cExtent, cBorder);
    m_cHilite.Initialize(m_cExtent, cHilite);
    m_cToggleBorder.Initialize(m_cExtent, cToggleBorder);
    m_cToggleHilite.Initialize(m_cExtent, cToggleHilite);
    CSWGuiExtent cBorderInner = m_cBorder.GetInnerExtent();
    CSWGuiExtent cHiliteInner = m_cHilite.GetInnerExtent();
    CSWGuiExtent cToggleBorderInner = m_cToggleBorder.GetInnerExtent();
    CSWGuiExtent cToggleHiliteInner = m_cToggleHilite.GetInnerExtent();
    CSWGuiExtent cInner = CSWGuiExtent::Intersection(
        CSWGuiExtent::Intersection(cBorderInner, cHiliteInner),
        CSWGuiExtent::Intersection(cToggleBorderInner, cToggleHiliteInner));
    m_cText.Initialize(cInner, cText, 1.0f);
}
// PC 0x00417DC0  Xbox 0x002FDCD0
void CSWGuiSlider::Initialize(const CSWGuiExtent& cExtent,
                              const CSWGuiBorderParams& cTrack,
                              const CSWGuiBorderParams& cThumb,
                              const CResRef& cImage)
{
    m_cExtent = cExtent;
    m_cTrack.Initialize(m_cExtent, cTrack);
    m_cThumb.Initialize(m_cExtent, cThumb);
    Vector vColor(1.0f, 1.0f, 1.0f);
    CSWGuiImageParams cParams(cImage, 0, 0, 0.0f, 18, 1.0f, vColor);
    m_cImage.Initialize(m_cExtent, cParams);
    SetExtent(m_cExtent);
}
// PC 0x00417E60  Xbox 0x002FD320
void CSWGuiSlider::Draw(float fElapsed)
{
    if (m_bActive)
    {
        m_cThumb.Draw(fElapsed);
    }
    else
    {
        m_cTrack.Draw(fElapsed);
    }
    m_cImage.Draw(fElapsed);
}
// PC 0x00417EC0  Xbox 0x002FD380
void CSWGuiSlider::SetMaxValue(int nMaxValue)
{
    if (m_nMaxValue != nMaxValue && nMaxValue >= 0)
    {
        m_nMaxValue = nMaxValue;
        SetExtent(m_cExtent);
    }
}
// PC 0x0041C3A0  Xbox 0x00300380
void CSWGuiListBox::SetActiveControl(CSWGuiControl* pControl, int bNotify)
{
    SetSelectedControl(pControl, bNotify);
}
// PC 0x0041C3B0  Xbox 0x00300390
void CSWGuiListBox::SetActive(int bActive)
{
    CSWGuiControl::SetActive(bActive);
    if (m_bActive)
    {
        if (m_nSelectedItem == -1 && !m_bScrollOnly)
        {
            SetSelectedControl(0, 0);
        }
        else
        {
            OrganizeControls();
        }
    }
}

#ifndef _XBOX
void CSWGuiListBox::HandleMouseMove()  // PC 0x0041C400
{
    int nIndex;

#ifndef _XBOX
    if (!m_bHilightOnOver)
    {
        return;
    }
#endif
    CSWGuiControl* pHit = HitCheckMouseLocal(&nIndex);
    if (pHit != m_pHoverControl)
    {
        if (m_pHoverControl != NULL)
        {
            m_pHoverControl->HideToolTip();
        }
        m_pHoverControl = pHit;
        if (pHit != NULL)
        {
#ifndef _XBOX
            g_pGuiMan->ResetToolTipTimer();
#endif
        }
    }
    if (pHit != NULL)
    {
        if (pHit->AsScrollBar() == NULL && !m_bSelectOnLeftClick &&
            (m_lItems.num <= 0 || m_lItems.element[0]->AsRadioButton() == NULL))
        {
            if (m_nSelectedItem != nIndex)
            {
                SetSelectedControl(nIndex, 1);
            }
        }
    }
}
#endif
// PC 0x0041C4A0
void CSWGuiListBox::HandleLMouseDown()
{
    int nItem;
    int nMouseX;
    int nMouseY;
#ifndef _XBOX
    CSWGuiControl* pHit = HitCheckMouseLocal(&nItem);
#endif

#ifndef _XBOX
    if (pHit != NULL)
    {
        if (pHit->AsScrollBar() != NULL)
        {
#ifndef _XBOX
            ((CSWGuiPanel*)m_pOwner)->GetLocalMouseCoords(&nMouseX, &nMouseY);
#endif
#ifndef _XBOX
            m_nScrollBarPart = pHit->AsScrollBar()->HitCheckScrollbar(nMouseX, nMouseY);
            m_nHeldEvent = GUIEVENT_MOUSE_ENTER;
            m_fRepeatDelay = 0.5f;
            switch (m_nScrollBarPart)
            {
            case CSWGuiScrollBar::SCROLLBAR_TOP_ARROW:
                m_nHeldEvent = GUIEVENT_SCROLLBAR_UP;
                HandleInputEvent(GUIEVENT_SCROLLBAR_UP, 1);
                break;
            case CSWGuiScrollBar::SCROLLBAR_BEFORE_THUMB:
                m_nHeldEvent = GUIEVENT_SCROLLBAR_PAGE_UP;
                HandleInputEvent(GUIEVENT_SCROLLBAR_PAGE_UP, 1);
                break;
            case CSWGuiScrollBar::SCROLLBAR_BOTTOM_ARROW:
                m_nHeldEvent = GUIEVENT_SCROLLBAR_DOWN;
                HandleInputEvent(GUIEVENT_SCROLLBAR_DOWN, 1);
                break;
            case CSWGuiScrollBar::SCROLLBAR_AFTER_THUMB:
                m_nHeldEvent = GUIEVENT_SCROLLBAR_PAGE_DOWN;
                HandleInputEvent(GUIEVENT_SCROLLBAR_PAGE_DOWN, 1);
                break;
            case CSWGuiScrollBar::SCROLLBAR_THUMB:
                m_nDragStartX = nMouseX;
                m_nDragStartY = nMouseY;
                m_nDragStartPosition = IsOversized() ? m_nScrollPosition : m_cScrollBar.m_nCurValue;
                break;
            }
#endif
        }
#ifndef _XBOX
        else if (m_bHilightOnOver)
#else
        else
#endif
        {
#ifndef _XBOX
            if (g_pGuiMan->m_pMouseOverControl != NULL &&
                (g_pGuiMan->m_pMouseOverControl == this ||
                 g_pGuiMan->m_pMouseOverControl->GetSelectableParent() == this) &&
                g_pGuiMan->m_bLeftMouseDown)
            {
                HandleInputEvent(EVENT_DOUBLE_CLICK, 1);
            }
#endif
            if (m_bSelectOnLeftClick ||
                (m_lItems.num > 0 && m_lItems.element[0]->AsRadioButton() != NULL))
            {
                if (m_nSelectedItem != nItem)
                {
                    SetSelectedControl(nItem, 1);
                }
            }
            else
            {
#ifndef _XBOX
                m_nPressedItem = nItem;
#endif
            }
            HandleInputEvent(EVENT_SELECTION_CHANGED, 1);
        }
#ifndef _XBOX
        g_pGuiMan->CaptureMouse(this, CSWGuiManager::MOUSECAPTURE_LEFT);
#endif
    }
#endif
}
// PC 0x0041C6B0
CSWGuiEditbox::CSWGuiEditbox()
{
    m_bFocusable = 1;
}
// PC 0x0041C7D0
void CSWGuiEditbox::Load(CResGFF* pGFF, CResStruct& cStruct)
{
    m_cText.Load(pGFF, cStruct, "TEXT");
    m_cBorder.Load(pGFF, cStruct, "BORDER");
    CSWGuiControl::Load(pGFF, cStruct);
}
// PC 0x0041C890  Xbox 0x002FD770
void CSWGuiNavigable::Load(CResGFF* pGFF, CResStruct& cStruct)
{
    CResStruct cMoveTo;
    int bSuccess;

    if (pGFF->GetStructFromStruct(&cMoveTo, &cStruct, "MOVETO"))
    {
        m_apMoveToControl[1] =
            (CSWGuiControl*)pGFF->ReadFieldINT(&cMoveTo, "LEFT", bSuccess, 0);
        m_apMoveToControl[3] =
            (CSWGuiControl*)pGFF->ReadFieldINT(&cMoveTo, "RIGHT", bSuccess, 0);
        m_apMoveToControl[0] =
            (CSWGuiControl*)pGFF->ReadFieldINT(&cMoveTo, "UP", bSuccess, 0);
        m_apMoveToControl[2] =
            (CSWGuiControl*)pGFF->ReadFieldINT(&cMoveTo, "DOWN", bSuccess, 0);
    }
    CSWGuiControl::Load(pGFF, cStruct);
}
// PC 0x0041B8B0  Xbox 0x002FFDD0
CSWGuiNavigable::CSWGuiNavigable()
{
    for (int i = 0; i < 4; i++)
    { m_apMoveToControl[i] = 0; }
#ifndef _XBOX
    m_bFocusable = 1;
#endif
}
// PC 0x0041B8E0  Xbox 0x002FCAE0
void CSWGuiControl::Load(CResGFF* pGFF, CResStruct& cStruct)
{
    int bSuccess;

    m_cExtent.Load(pGFF, cStruct);
    SetExtent(m_cExtent);
    m_nID = pGFF->ReadFieldINT(&cStruct, "ID", bSuccess, -1);
    int nParentID = pGFF->ReadFieldINT(&cStruct, "Obj_ParentID", bSuccess, -1);
    if (nParentID != -1)
    {
        CSWGuiPanel* pPanel = m_pOwner->AsSWGuiPanel();
        if (pPanel != NULL)
        {
#ifndef _XBOX
            m_pParent = pPanel->GetControl(nParentID);
#endif
#ifndef _XBOX
            m_pParent->AddChildControl(this);
#endif
        }
    }
}
// PC 0x0041B960  Xbox 0x002FCBB0
void CSWGuiLabel::Load(CResGFF* pGFF, CResStruct& cStruct)
{
    m_cText.Load(pGFF, cStruct, "TEXT");
    m_cBorder.Load(pGFF, cStruct, "BORDER");
    CSWGuiControl::Load(pGFF, cStruct);
}
// PC 0x0041BA20  Xbox 0x002FCE60
void CSWGuiLabelHilight::Load(CResGFF* pGFF, CResStruct& cStruct)
{
    m_cHilite.Load(pGFF, cStruct, "HILIGHT");
    CSWGuiLabel::Load(pGFF, cStruct);
}
// PC 0x0041BAA0  Xbox 0x002FFE10
CSWGuiSlider::CSWGuiSlider()
{
    m_nMaxValue = 0;
    m_nCurValue = 0;
    m_nSlideSound = 1;
}
// PC 0x0041BBC0  Xbox 0x002FE120
void CSWGuiProgressBar::Load(CResGFF* pGFF, CResStruct& cStruct)
{
    int bSuccess;
    m_cBorder.Load(pGFF, cStruct, "BORDER");
    m_cFill.Load(pGFF, cStruct, "PROGRESS");
    SetMaxValue(pGFF->ReadFieldINT(&cStruct, "MAXVALUE", bSuccess, 0));
    SetCurValue(pGFF->ReadFieldINT(&cStruct, "CURVALUE", bSuccess, 0));
    SetStartFromLeft(pGFF->ReadFieldBYTE(&cStruct, "STARTFROMLEFT", bSuccess, 1));
    CSWGuiControl::Load(pGFF, cStruct);
}
// PC 0x0041BCD0  Xbox 0x002FE770
void CSWGuiScrollBar::Load(CResGFF* pGFF, CResStruct& cStruct)
{
    int bSuccess;

    m_cBorder.Load(pGFF, cStruct, "BORDER");
    m_cArrow.Load(pGFF, cStruct, "DIR");
    m_cThumb.Load(pGFF, cStruct, "THUMB");
    SetMaxValue(pGFF->ReadFieldINT(&cStruct, "MAXVALUE", bSuccess, 0), 0);
    SetCurValue(pGFF->ReadFieldINT(&cStruct, "CURVALUE", bSuccess, 0));
    SetVisibleValue(pGFF->ReadFieldINT(&cStruct, "VISIBLEVALUE", bSuccess, 0));
    m_bDrawMode = pGFF->ReadFieldBYTE(&cStruct, "DRAWMODE", bSuccess, 0);
    CSWGuiControl::Load(pGFF, cStruct);
}
// PC 0x0041C930  Xbox 0x002FDAA0
void CSWGuiButton::Load(CResGFF* pGFF, CResStruct& cStruct)
{
    m_cBorder.Load(pGFF, cStruct, "BORDER");
    m_cHilite.Load(pGFF, cStruct, "HILIGHT");
    m_cText.Load(pGFF, cStruct, "TEXT");
    CSWGuiNavigable::Load(pGFF, cStruct);
}
// PC 0x0041CA30  Xbox 0x003003E0
CSWGuiButtonToggle::CSWGuiButtonToggle()
{
    m_bToggled = 0;
    m_nToggleEvent = EVENT_SELECT;
    m_nClickSound = 3;
}
// PC 0x0041CB40  Xbox 0x002FDBA0
void CSWGuiButtonToggle::Load(CResGFF* pGFF, CResStruct& cStruct)
{
    int bSuccess;

    m_cToggleBorder.Load(pGFF, cStruct, "SELECTED");
    m_cToggleHilite.Load(pGFF, cStruct, "HILIGHTSELECTED");
    m_bToggled = pGFF->ReadFieldBYTE(&cStruct, "ISSELECTED", bSuccess, 0);
    CSWGuiButton::Load(pGFF, cStruct);
}
// PC 0x0041CC30  Xbox 0x002FDEE0
void CSWGuiSlider::Load(CResGFF* pGFF, CResStruct& cStruct)
{
    int bSuccess;

    m_cTrack.Load(pGFF, cStruct, "BORDER");
    m_cThumb.Load(pGFF, cStruct, "HILIGHT");
    m_cImage.Load(pGFF, cStruct, "THUMB");
    SetMaxValue(pGFF->ReadFieldINT(&cStruct, "MAXVALUE", bSuccess, 0));
    SetCurValue(pGFF->ReadFieldINT(&cStruct, "CURVALUE", bSuccess, 0));
    CSWGuiNavigable::Load(pGFF, cStruct);
}
// PC 0x0041CD60  Xbox 0x00300480
void CSWGuiListBox::Initialize(const CSWGuiExtent& cExtent,
                             const CSWGuiBorderParams& cBorder, int nPadding)
{
    m_cExtent = cExtent;
    CSWGuiExtent cBorderExtent = cExtent;
    CSWGuiExtent cScrollBarExtent = m_cScrollBar.m_cExtent;
    if (m_bScrollBarOnLeft)
    {
        cBorderExtent.m_nLeft += cScrollBarExtent.m_nWidth;
    }
    cBorderExtent.m_nWidth -= cScrollBarExtent.m_nWidth;
    m_cBorder.Initialize(cBorderExtent, cBorder);
    m_cItemExtent = m_cBorder.GetInnerExtent();
    SetPadding(nPadding);
}
#ifndef _XBOX
void CSWGuiListBox::HandleInputEvent(int nEvent, int nState)  // PC 0x0041CE20  Xbox 0x00300540
{
    if (nState)
    {
        int bChanged = 0;
        switch (nEvent)
        {
        case GUIEVENT_UP:
        case GUIEVENT_UP_ALTERNATE:
            if (m_nSelectedItem == -1)
            {
                if (m_bScrollOnly)
                {
                    bChanged = m_nTopVisibleItem != 0;
                    m_nTopVisibleItem = max(m_nTopVisibleItem - 1, 0);
                    OrganizeControls();
                }
                else if (m_lItems.num != 0)
                {
                    SetSelectedControl(0, 0);
                    bChanged = 1;
                }
            }
            else if (IsOversized())
            {
                bChanged = m_nScrollPosition != 1;
                m_nScrollPosition = max(1, m_nScrollPosition - 1);
                OrganizeOversized();
            }
            else if (m_nSelectedItem > 0)
            {
                SetSelectedControl(m_nSelectedItem - 1, 1);
                bChanged = 1;
            }
            else if (m_bLooping)
            {
                SetSelectedControl(m_lItems.num - 1, 1);
                if (m_pOwner && m_pOwner->AsSWGuiPanel())
                    g_pExoInput->ClearEventRepeat(
                        m_pOwner->AsSWGuiPanel()->m_pManager->m_nLastInputEvent);
                bChanged = 1;
            }
            break;

        case GUIEVENT_DOWN:
        case GUIEVENT_DOWN_ALTERNATE:
            if (m_nSelectedItem == -1)
            {
                if (m_bScrollOnly)
                {
                    bChanged = m_nTopVisibleItem != max(m_lItems.num - m_nVisibleItems, 0);
                    m_nTopVisibleItem = min(m_nTopVisibleItem + 1, max(m_lItems.num - m_nVisibleItems, 0));
                    OrganizeControls();
                }
                else if (m_lItems.num != 0)
                {
                    SetSelectedControl(0, 1);
                    bChanged = 1;
                }
            }
            else if (IsOversized())
            {
                bChanged = m_nScrollPosition != GetScrollRange();
                m_nScrollPosition = min(GetScrollRange(), m_nScrollPosition + 1);
                OrganizeOversized();
            }
            else if (m_nSelectedItem < m_lItems.num - 1)
            {
                SetSelectedControl(m_nSelectedItem + 1, 1);
                bChanged = 1;
            }
            else if (m_bLooping)
            {
                SetSelectedControl(0, 1);
                if (m_pOwner && m_pOwner->AsSWGuiPanel())
                    g_pExoInput->ClearEventRepeat(
                        m_pOwner->AsSWGuiPanel()->m_pManager->m_nLastInputEvent);
                bChanged = 1;
            }
            break;
#ifndef _XBOX
        case GUIEVENT_SLIDER_DECREMENT:
        case GUIEVENT_SCROLLBAR_DOWN:
            if (IsOversized())
            {
                m_nScrollPosition = min(GetScrollRange(), m_nScrollPosition + 1);
                OrganizeOversized();
            }
            else if (m_nTopVisibleItem < m_cScrollBar.GetMaxValue() - 1 &&
                     m_nBottomVisibleItem < m_nVisibleItems + m_cScrollBar.GetMaxValue() - 1)
            {
                ++m_nTopVisibleItem;
                m_bScrollToSelected = 0;
                m_cScrollBar.SetCurValue(m_nTopVisibleItem);
                OrganizeControls();
            }
            break;
        case GUIEVENT_SCROLLBAR_PAGE_DOWN:
            if (IsOversized())
            {
                m_nScrollPosition += (int)m_fScrollStep;
                m_nScrollPosition = min(GetScrollRange(), m_nScrollPosition);
                OrganizeOversized();
            }
            else if (m_nTopVisibleItem < m_cScrollBar.GetMaxValue() - 1 &&
                     m_nBottomVisibleItem < m_nVisibleItems + m_cScrollBar.GetMaxValue() - 1)
            {
                if (m_cScrollBar.GetMaxValue() - m_nBottomVisibleItem - 1 < m_cScrollBar.GetVisibleValue())
                {
                    if (m_cScrollBar.GetMaxValue() - m_nBottomVisibleItem - 1 > 0)
                        m_nTopVisibleItem += m_cScrollBar.GetMaxValue() - m_nBottomVisibleItem - 1;
                }
                else
                {
                    m_nTopVisibleItem += m_cScrollBar.GetVisibleValue();
                }
                m_nTopVisibleItem = min(m_cScrollBar.GetMaxValue() - 1, m_nTopVisibleItem);
                m_bScrollToSelected = 0;
                m_cScrollBar.SetCurValue(m_nTopVisibleItem);
                OrganizeControls();
            }
            break;

        case GUIEVENT_SLIDER_INCREMENT:
        case GUIEVENT_SCROLLBAR_UP:
            if (IsOversized())
            {
                m_nScrollPosition = max(1, m_nScrollPosition - 1);
                OrganizeOversized();
            }
            else if (m_nTopVisibleItem > 0)
            {
                --m_nTopVisibleItem;
                m_bScrollToSelected = 0;
                m_cScrollBar.SetCurValue(m_nTopVisibleItem);
                OrganizeControls();
            }
            break;

        case GUIEVENT_SCROLLBAR_PAGE_UP:
            if (IsOversized())
            {
                m_nScrollPosition -= (int)m_fScrollStep;
                m_nScrollPosition = max(1, m_nScrollPosition);
                OrganizeOversized();
            }
            else if (m_nTopVisibleItem > 0)
            {
                m_nTopVisibleItem -= m_cScrollBar.GetVisibleValue();
                m_nTopVisibleItem = max(0, m_nTopVisibleItem);
                m_bScrollToSelected = 0;
                m_cScrollBar.SetCurValue(m_nTopVisibleItem);
                OrganizeControls();
            }
            break;
#endif
        }

        if (bChanged && m_nSelectSound != -1 && m_pOwner &&
            m_pOwner->AsSWGuiPanel())
        {
            CSWGuiPanel* pPanel = m_pOwner->AsSWGuiPanel();
            pPanel->m_pManager->PlayGuiSound(m_nSelectSound);
        }
    }

    if (m_nSelectedItem != -1)
        m_lItems.element[m_nSelectedItem]->HandleInputEvent(nEvent, nState);
    CSWGuiControl::HandleInputEvent(nEvent, nState);
}
#endif
void CSWGuiListBox::ClearItems()  // PC 0x00419A90  Xbox 0x002FE9E0
{
    int i;

    if (m_bOwnsItems)
    {
#ifndef _XBOX
        if (m_pHoverControl != NULL && m_pHoverControl->AsScrollBar() == NULL)
        {
            m_pHoverControl = NULL;
        }
#endif
        for (i = 0; i < GetNumItems(); i++)
        {
            if (m_lItems.element[i] != NULL)
            {
                delete m_lItems.element[i];
            }
            m_lItems.element[i] = NULL;
        }
    }
    else
    {
        for (i = 0; i < GetNumItems(); i++)
        {
            if (m_bRestoreExtents)
            {
                m_lItems.element[i]->SetExtent(m_lItemExtents.element[i]);
            }
            m_lItems.element[i]->m_pOwner = NULL;
        }
    }
#ifndef _XBOX
    m_pHoverControl = NULL;
#endif
    m_lItems.num = 0;
    m_lItemExtents.num = 0;
    m_nSelectedItem = -1;
    m_nScrollPosition = 1;
}
// PC 0x00419B80  Xbox 0x002FEAA0
void CSWGuiListBox::Initialize(const CSWGuiExtent& cExtent, CSWGuiListBox* pProto)
{
    SwguicontrolTrace1();
    Initialize(cExtent, pProto->m_cBorder.m_cParams, pProto->m_nPadding);
    CSWGuiScrollBar* pScrollBar = &pProto->m_cScrollBar;
    return m_cScrollBar.Initialize(pScrollBar->m_cExtent, pScrollBar);
}
#ifndef _XBOX
void CSWGuiListBox::InitializeScrollBar(CSWGuiExtent& cExtent,  // PC 0x00419BD0  Xbox 0x002FEAF0
                                      const CSWGuiBorderParams& cBorder,
                                      const CResRef& sArrow,
                                      const CResRef& sThumb, int bLeft)
{
    m_bScrollBarOnLeft = bLeft;
    if (cExtent.m_nWidth > m_cExtent.m_nWidth)
    {
        cExtent.m_nWidth = m_cExtent.m_nWidth;
    }
    cExtent.m_nTop = m_cExtent.m_nTop;
    cExtent.m_nHeight = m_cExtent.m_nHeight;
    if (m_bScrollBarOnLeft)
    {
        cExtent.m_nLeft = m_cExtent.m_nLeft;
    }
    else
    {
        cExtent.m_nLeft = m_cExtent.m_nLeft + m_cExtent.m_nWidth - cExtent.m_nWidth;
    }
    m_cScrollBar.Initialize(cExtent, cBorder, sArrow, sThumb);
    m_cScrollBar.SetMaxValue(
        (m_lItems.num - (m_nVisibleItems < 1 ? 1 : m_nVisibleItems) + 1 > 1)
            ? m_lItems.num - (m_nVisibleItems < 1 ? 1 : m_nVisibleItems) + 1
            : 1,
        m_bScrollOnly);
    SetExtent(m_cExtent);
}
#endif
CSWGuiListBox::CSWGuiListBox() : m_lItems(0), m_lItemExtents(0)  // PC 0x0041BE40  Xbox 0x002FFEB0
{
    m_nSelectedItem = -1;
    m_nSelectSound = (char)GUISOUND_NONE;
    m_nPadding = 0;
    m_pProtoItem = NULL;
#ifndef _XBOX
    m_bSelectOnLeftClick = 0;
#endif
    m_bOwnsItems = 1;
    m_bRestoreExtents = 0;
    m_bOrganizePending = 1;
    m_bScrollBarOnLeft = 1;
    m_bShowScrollBar = 1;
    m_bLooping = 0;
    m_bHideScrollIcon = 0;
    m_bScrollOnly = 0;
    m_bClipItems = 0;
    m_bPulseArrows = 1;
#ifndef _XBOX
    m_bScrollToSelected = 1;
    m_nPressedItem = -1;
    m_nTopVisibleItem = 0;
    m_nBottomVisibleItem = 0;
#endif
#ifndef _XBOX
    m_nHeldEvent = GUIEVENT_MOUSE_ENTER;
    m_fRepeatDelay = 0.0f;
    m_nDragStartPosition = 0;
    m_pHoverControl = NULL;
#endif
    m_nScrollPosition = 1;
    m_fScrollStep = 1.0f;
}
// PC 0x0041BF80  Xbox 0x002FFFD0
void CSWGuiListBox::SetExtent(const CSWGuiExtent& cExtent)
{
    int nScrollBarWidth = m_cScrollBar.m_cExtent.m_nWidth;
    m_cExtent = cExtent;
    CSWGuiExtent cBorderExtent = cExtent;
    if (m_bScrollBarOnLeft)
    {
        cBorderExtent.m_nLeft += nScrollBarWidth;
    }
    cBorderExtent.m_nWidth -= nScrollBarWidth;
    m_cBorder.SetExtent(cBorderExtent);
    m_cItemExtent = m_cBorder.GetInnerExtent();
    SetScrollBarExtent(nScrollBarWidth, 0);
    m_bOrganizePending = true;
    OrganizeControls();
}
#ifndef _XBOX
void CSWGuiListBox::SetSelectedControl(int nIndex, int bPlaySound)  // PC 0x0041C040  Xbox 0x00300090
{
    if (m_nSelectedItem != -1)
    {
        m_lItems.element[m_nSelectedItem]->HandleInputEvent(1, 1);
    }
    if (nIndex >= 0 && nIndex < m_lItems.num)
    {
        m_nSelectedItem = nIndex;
        m_lItems.element[m_nSelectedItem]->HandleInputEvent(GUIEVENT_MOUSE_ENTER, 1);
        if (bPlaySound)
        {
            ((CSWGuiPanel*)m_pOwner)
                ->m_pManager->PlayGuiSound(
                    m_lItems.element[m_nSelectedItem]->m_nFocusSound);
        }
    }
    else
    {
        m_nSelectedItem = -1;
    }
    m_bScrollToSelected = 1;
    OrganizeControls();
    m_cScrollBar.SetCurValue(m_nTopVisibleItem);
}
#endif
// PC 0x0041C160  Xbox 0x00300150
void CSWGuiListBox::SetSelectedControl(CSWGuiControl* pControl, int bPlaySound)
{
    SetSelectedControl(m_lItems.IndexOf(pControl), bPlaySound);
}
// PC 0x0041C190  Xbox 0x00300180
void CSWGuiListBox::SetPadding(int nPadding)
{
    if (m_nPadding <= m_cItemExtent.m_nWidth / 2 &&
        m_nPadding <= m_cItemExtent.m_nHeight / 2)
    {
        m_nPadding = nPadding;
        m_bOrganizePending = 1;
        OrganizeControls();
    }
}
// PC 0x0041C1D0  Xbox 0x003001C0
void CSWGuiListBox::AddControls(CExoArrayList<CSWGuiControl*>& aControls,
                               int bOwnsItems, int bRestoreExtents,
                               int bHideScrollIcon)
{
    int nID = 0;
    int nCount = aControls.num;

    ClearItems();
    m_bOwnsItems = bOwnsItems;
    m_bRestoreExtents = bRestoreExtents;
    for (int i = 0; i < nCount; i++)
    {
        CSWGuiControl* pControl = aControls[i];
        if (m_lItems.IndexOf(pControl) == -1)
        {
            pControl->m_pOwner = this;
            pControl->m_nID = nID++;
            m_lItems.Add(pControl);
            m_lItemExtents.Add(pControl->m_cExtent);
        }
    }
    m_bHideScrollIcon = bHideScrollIcon;
    m_bOrganizePending = 1;
    OrganizeControls();
}
// PC 0x0041C2C0  Xbox 0x003002B0
void CSWGuiListBox::AddControls(CSWGuiControl** pControls, int nCount,
                               int bOwnsItems, int bRestoreExtents,
                               int bHideScrollIcon)
{
    int nID = 0;

    ClearItems();
    m_bOwnsItems = bOwnsItems;
    m_bRestoreExtents = bRestoreExtents;
    for (int i = 0; i < nCount; i++)
    {
        CSWGuiControl* pControl = pControls[i];
        if (m_lItems.IndexOf(pControl) == -1)
        {
            pControl->m_pOwner = this;
            pControl->m_nID = nID++;
            m_lItems.Add(pControl);
            m_lItemExtents.Add(pControl->m_cExtent);
        }
    }
    m_bHideScrollIcon = bHideScrollIcon;
    m_bOrganizePending = 1;
    OrganizeControls();
}
#ifndef _XBOX
void CSWGuiListBox::OrganizeUnequal()  // PC 0x00419EE0  Xbox 0x002FEC50
{
    int nItems = m_lItems.num;
    CExoString sUnused;
    int nIndex;
    int nTop;
    int i;

    if (m_bOrganizePending)
    {
        if (nItems == 1)
        {
            m_nContentHeight = m_lItems[0]->GetIdealHeight();
        }
        else
        {
            m_nContentHeight = 0;
            for (i = 0; i < nItems - 1; i++)
            {
                int nHeight = m_lItems[i]->GetIdealHeight();
                if (nHeight > m_nContentHeight)
                {
                    m_nContentHeight = nHeight;
                }
            }
        }
        m_nTopVisibleItem = 0;
        m_nBottomVisibleItem = -1;
        m_bTopAnchored = 1;
        if (m_nContentHeight == 0)
        {
            return;
        }
        m_bOrganizePending = 0;
    }
    if (m_bScrollToSelected)
    {
        if (m_nSelectedItem != -1)
        {
            if (m_nTopVisibleItem != -1 && m_nSelectedItem < m_nTopVisibleItem)
            {
                m_nTopVisibleItem = m_nSelectedItem;
                m_nBottomVisibleItem = -1;
                m_bTopAnchored = 1;
            }
            else if (m_nBottomVisibleItem != -1 &&
                     m_nSelectedItem > m_nBottomVisibleItem)
            {
                m_nTopVisibleItem = -1;
                m_nBottomVisibleItem = m_nSelectedItem;
                m_bTopAnchored = 0;
            }
        }
    }
    int nPadding = m_nPadding;
    int nInner = m_cItemExtent.m_nWidth - 2 * nPadding;
    CSWGuiExtent cExtent;
    cExtent.m_nLeft = nPadding;
    cExtent.m_nWidth = nInner;
    cExtent.m_nTop = 0;
    cExtent.m_nHeight = 0;
    if (m_bTopAnchored)
    {
        nIndex = m_nTopVisibleItem;
        nTop = m_nPadding;
    }
    else
    {
        nIndex = m_nBottomVisibleItem;
        nTop = m_cItemExtent.m_nHeight -
               m_lItems[nIndex]->m_cExtent.m_nHeight - m_nPadding;
    }
    cExtent.m_nTop = nTop;
    cExtent.m_nHeight = m_lItems[nIndex]->m_cExtent.m_nHeight;
    m_lItems[nIndex]->SetExtent(cExtent);
    for (i = nIndex - 1; i >= 0; i--)
    {
        int nHeight = m_lItems[i]->m_cExtent.m_nHeight;
        cExtent.m_nHeight = nHeight;
        nTop -= nHeight + m_nPadding;
        cExtent.m_nTop = nTop;
        m_lItems[i]->SetExtent(cExtent);
    }
    CSWGuiExtent* pLast = &m_lItems[nIndex]->m_cExtent;
    nTop = pLast->m_nHeight + pLast->m_nTop + m_nPadding;
    for (i = nIndex + 1; i < nItems; i++)
    {
        int nHeight = m_lItems[i]->m_cExtent.m_nHeight;
        cExtent.m_nHeight = nHeight;
        cExtent.m_nTop = nTop;
        nTop += nHeight + m_nPadding;
        m_lItems[i]->SetExtent(cExtent);
    }
    if (m_bTopAnchored)
    {
        int nNext = m_nTopVisibleItem + 1;
        int bAllFit = 1;
        if (nNext < nItems)
        {
            CSWGuiControl** ppItem = &m_lItems[nNext];
            int nViewHeight = m_cItemExtent.m_nHeight;
            for (; nNext < nItems; nNext++, ppItem++)
            {
                CSWGuiExtent* pExtent = &(*ppItem)->m_cExtent;
                if (pExtent->m_nHeight + pExtent->m_nTop > nViewHeight)
                {
                    m_nBottomVisibleItem = nNext - 1;
                    bAllFit = 0;
                    break;
                }
            }
        }
        if (m_nBottomVisibleItem == -1 || bAllFit)
        {
            m_nBottomVisibleItem = nItems - 1;
        }
    }
    else
    {
        int nPrev = m_nBottomVisibleItem - 1;
        if (nPrev >= 0)
        {
            CSWGuiControl** ppItem = &m_lItems[nPrev];
            for (; nPrev >= 0; nPrev--, ppItem--)
            {
                if ((*ppItem)->m_cExtent.m_nTop < 0)
                {
                    m_nTopVisibleItem = nPrev + 1;
                    break;
                }
            }
        }
        if (m_nTopVisibleItem == -1)
        {
            m_nTopVisibleItem = 0;
        }
        m_bTopAnchored = 1;
    }
    m_nVisibleItems = m_nBottomVisibleItem - m_nTopVisibleItem + 1;
    int nMaxValue;
    if (nItems - m_nVisibleItems + 1 > 1)
    {
        nMaxValue = nItems - m_nVisibleItems + 1;
    }
    else
    {
        nMaxValue = 1;
    }
    m_cScrollBar.SetMaxValue(nMaxValue, m_bScrollOnly);
    m_cScrollBar.SetVisibleValue(m_nBottomVisibleItem - m_nTopVisibleItem + 1);
    m_cScrollBar.SetCurValue(m_nTopVisibleItem);
}
#endif
// PC 0x0041A290  Xbox 0x0004CEC0
inline int CSWGuiListBox::IsOversized()
{
    SwguicontrolTrace1();
    return m_nContentHeight + m_nPadding > m_cItemExtent.m_nHeight &&
           m_lItems.num > 0 && !m_bHideScrollIcon;
}
#ifndef _XBOX
void CSWGuiListBox::OrganizeOversized()  // PC 0x0041A2D0  Xbox 0x002FEFB0
{
    int nRange;
    CSWGuiExtent cExtent;

    cExtent.m_nLeft = m_nPadding;
    cExtent.m_nWidth = m_cItemExtent.m_nWidth - 2 * m_nPadding;
    cExtent.m_nHeight = m_nContentHeight;
    if (m_cItemExtent.m_nHeight != 0)
    {
        nRange =
            (int)(m_nContentHeight * m_fScrollStep / m_cItemExtent.m_nHeight +
                  0.9999f) -
            int(m_fScrollStep) + 1;
    }
    else
    {
        nRange = 1;
    }
    if (m_nScrollPosition == nRange)
    {
        cExtent.m_nTop =
            m_cItemExtent.m_nHeight - m_nContentHeight - m_nPadding;
        m_cScrollBar.SetCurValue(m_nContentHeight);
    }
    else
    {
        cExtent.m_nTop =
            m_nPadding -
            (int)((double)(m_cItemExtent.m_nHeight * (m_nScrollPosition - 1)) /
                  m_fScrollStep);
        if (m_nScrollPosition == 1)
        {
            m_cScrollBar.SetCurValue(0);
        }
        else
        {
            m_cScrollBar.SetCurValue(
                (int)((float)(m_nScrollPosition - 1) / (nRange - 1) *
                      (m_nContentHeight - m_cItemExtent.m_nHeight)));
        }
    }
    m_lItems[0]->SetExtent(cExtent);
}
#endif
void CSWGuiListBox::Draw(float fElapsed)  // PC 0x0041A3E0  Xbox 0x002FF0D0
{
    int nCount = m_lItems.num;
    if (m_bShowScrollBar)
    {
        if (m_bPulseArrows)
            m_cScrollBar.m_cArrow.m_cParams.m_fAlpha = m_fArrowAlpha;
        m_cScrollBar.Draw(fElapsed);
    }
    m_cBorder.Draw(fElapsed);
    if (AurGUISetupViewport(m_cItemExtent.m_nLeft, m_cItemExtent.m_nTop,
                            m_cItemExtent.m_nWidth, m_cItemExtent.m_nHeight,
                            m_vColor, false, 1.0f))
    {
        if (m_nContentHeight + m_nPadding > m_cItemExtent.m_nHeight &&
            m_lItems.num > 0 && !m_bHideScrollIcon)
        {
            m_lItems[0]->Draw(fElapsed);
        }
        else
        {
            for (int i = 0; i < nCount; i++)
            {
                CSWGuiControl* pControl = m_lItems[i];
                int nTop = pControl->m_cExtent.m_nTop;
                int nBottom = pControl->m_cExtent.GetBottom();
                if (nBottom > 0 && nTop <= m_cItemExtent.m_nHeight &&
                    (!m_bClipItems ||
                     (nBottom < m_cItemExtent.m_nHeight && nTop >= 0)))
                    pControl->Draw(fElapsed);
            }
        }
        AurGUICloseViewport();
    }
#ifndef _XBOX
    if (g_pGuiMan->m_pMouseCapture == this && m_nHeldEvent)
    {
        int nX, nY;
        ((CSWGuiPanel*)m_pOwner)->GetLocalMouseCoords(&nX, &nY);
        if (m_cScrollBar.HitCheckScrollbar(nX, nY) == m_nScrollBarPart)
        {
            m_fRepeatDelay -= fElapsed;
            while (m_fRepeatDelay < 0.0f)
            {
                m_fRepeatDelay += 0.1f;
                HandleInputEvent(m_nHeldEvent, 1);
            }
        }
    }
#endif
}
// PC 0x0041A5A0  Xbox 0x002FF200
CSWGuiControl* CSWGuiListBox::GetControl(int nIndex)
{
    if (nIndex < 0 || nIndex >= m_lItems.num)
    {
        return NULL;
    }
    return m_lItems[nIndex];
}
// PC 0x0041A5D0  Xbox 0x002FF230
CSWGuiControl* CSWGuiListBox::GetSelectedControl()
{
    return GetControl(m_nSelectedItem);
}

#ifndef _XBOX
CSWGuiControl* CSWGuiListBox::HitCheckMouseLocal(int* pnIndex)  // PC 0x0041A5E0
{
    int nX, nY;
#ifndef _XBOX
    ((CSWGuiPanel*)m_pOwner)->GetLocalMouseCoords(&nX, &nY);
#endif

    CSWGuiExtent cExtent = m_cScrollBar.m_cExtent;
    if (nX >= cExtent.m_nLeft &&
        nX <= cExtent.m_nLeft + cExtent.m_nWidth &&
        nY >= cExtent.m_nTop &&
        nY <= cExtent.m_nTop + cExtent.m_nHeight)
    {
        if (pnIndex != NULL)
        {
            *pnIndex = -1;
        }
        return &m_cScrollBar;
    }

    int nLocalX = nX - m_cItemExtent.m_nLeft;
    int nLocalY = nY - m_cItemExtent.m_nTop;
    m_nMouseX = nLocalX;
    m_nMouseY = nLocalY;
    if (m_lItems.num > 0)
    {
        for (int i = 0; i < m_lItems.num; i++)
        {
            CSWGuiControl* pHit = m_lItems[i]->HitCheckMouse(nLocalX, nLocalY);
            if (pHit != NULL)
            {
                if (pnIndex != NULL)
                {
                    *pnIndex = i;
                }
                return pHit;
            }
        }
    }
    return NULL;
}
#endif
// PC 0x0041A6C0
int CSWGuiListBox::GetIsSelectable()
{
    int bSelectable = 0;

#ifndef _XBOX
    if (!m_bHilightOnOver)
    {
        return 0;
    }
#endif
    CSWGuiListBox* pListBox = AsSWGuiListBox();
    if (pListBox != NULL)
    {
#ifndef _XBOX
        CSWGuiControl* pControl = pListBox->HitCheckMouseLocal(NULL);
#endif
#ifndef _XBOX
        if (pControl != NULL)
        {
            if (pControl->GetIsSelectable())
            {
                bSelectable = 1;
            }
        }
#endif
    }
    return bSelectable;
}
// PC 0x0041A700
void CSWGuiListBox::HandleLMouseUp()
{
    int nItem;

#ifndef _XBOX
    m_nHeldEvent = GUIEVENT_MOUSE_ENTER;
    CSWGuiControl* pHit = HitCheckMouseLocal(&nItem);
#endif
#ifndef _XBOX
    if (pHit != NULL && m_bHilightOnOver)
    {
        if (m_nPressedItem != -1)
        {
            if (m_nPressedItem == nItem)
            {
                HandleInputEvent(GUIEVENT_ACTIVATE, 1);
            }
        }
        else if ((m_bSelectOnLeftClick ||
                  (m_lItems.num > 0 &&
                   m_lItems.element[0]->AsRadioButton() != NULL)) &&
                 g_pGuiMan->m_bLeftMouseDown && pHit->AsScrollBar() == NULL)
        {
            HandleInputEvent(GUIEVENT_ACTIVATE, 1);
        }
    }
#endif
#ifndef _XBOX
    m_nPressedItem = -1;
#endif
#ifndef _XBOX
    g_pGuiMan->ReleaseMouse(this);
#endif
#ifndef _XBOX
    m_nScrollBarPart = CSWGuiScrollBar::SCROLLBAR_NONE;
#endif
}
// PC 0x0041A7B0
void CSWGuiListBox::ReSetFont()
{
    for (int i = 0; i < m_lItems.num; i++)
    {
        m_lItems[i]->ReSetFont();
    }
    if (m_pProtoItem != NULL)
    {
        m_pProtoItem->ReSetFont();
    }
}
// PC 0x0041A800
int CSWGuiListBox::DisplayToolTip()
{
#ifndef _XBOX
    CSWGuiControl* pControl = HitCheckMouseLocal(NULL);
#endif
#ifndef _XBOX
    if (pControl != NULL)
    {
        return pControl->DisplayToolTip();
    }
#endif
    return 0;
}
// PC 0x0041A820
void CSWGuiEditbox::HandleFocusChange(int bFocus)
{
    if (bFocus)
    {
        g_pExoInput->KeyboardModeOn();
#ifndef _XBOX
        g_pGuiMan->m_pKeyFocus = this;
#endif
        m_cText.SetCaretVisible(1);
    }
    else
    {
        g_pExoInput->KeyboardModeOff();
#ifndef _XBOX
        g_pGuiMan->m_pKeyFocus = NULL;
#endif
        m_cText.SetCaretVisible(0);
    }
    CSWGuiControl::HandleFocusChange(bFocus);
}
// PC 0x0041A870  Xbox 0x002FF270
template <> CExoArrayList<CSWGuiControlEvent>::CExoArrayList(int nSize)
{
    num = 0;
    array_size = 0;
    element = NULL;
    if (nSize != 0)
    {
        Allocate(nSize);
    }
}
// PC 0x0041A8A0
template <> void CExoArrayList<CSWGuiControlEvent>::Add(CSWGuiControlEvent value)
{
    if (num == array_size)
    {
        Allocate(array_size != 0 ? array_size * 2 : EXOARRAYLIST_INITIAL_ALLOCATION);
    }
    element[num++] = value;
}
// PC 0x0041A8F0  Xbox 0x002FF2A0
template <> inline void CExoArrayList<CSWGuiExtent>::Allocate(int nSize)
{
    CSWGuiExtent* pOld = element;

    array_size = nSize;
    element = new CSWGuiExtent[nSize];

    for (int i = 0; i < num; i++)
    {
        element[i] = pOld[i];
    }

    if (pOld != NULL)
    {
        delete[] pOld;
    }
}
// PC 0x00417EE0  Xbox 0x002FD3A0
void CSWGuiSlider::SetCurValue(int nValue)
{
    if (nValue < 0)
    {
        nValue = 0;
    }
    else if (nValue > m_nMaxValue)
    {
        nValue = m_nMaxValue;
    }
    m_nCurValue = nValue;
    SetExtent(m_cExtent);
}
// PC 0x00417F10  Xbox 0x002FD3D0
void CSWGuiProgressBar::Initialize(const CSWGuiExtent& cExtent,
                                   const CSWGuiBorderParams& cBorder,
                                   const CSWGuiBorderParams& cFill)
{
    m_cExtent = cExtent;
    m_cBorder.Initialize(m_cExtent, cBorder);
    m_cFill.Initialize(m_cExtent, cFill);
    SetExtent(m_cExtent);
}
// PC 0x00417F60  Xbox 0x002FD420
void CSWGuiProgressBar::Draw(float fElapsed)
{
    m_cBorder.Draw(fElapsed);
    m_cFill.Draw(fElapsed);
}
// PC 0x00417F90  Xbox 0x002FD450
void CSWGuiProgressBar::SetMaxValue(int nMaxValue)
{
    if (m_nMaxValue != nMaxValue && nMaxValue >= 0)
    {
        m_nMaxValue = nMaxValue;
        SetExtent(m_cExtent);
    }
}
// PC 0x00417FB0  Xbox 0x002FD470
void CSWGuiProgressBar::SetCurValue(int nValue)
{
    if (nValue < 0)
    {
        nValue = 0;
    }
    else if (nValue > m_nMaxValue)
    {
        nValue = m_nMaxValue;
    }
    if (m_nCurValue != nValue)
    {
        m_nCurValue = nValue;
        SetExtent(m_cExtent);
    }
}
// PC 0x00417FE0  Xbox 0x002FD4A0
void CSWGuiProgressBar::SetStartFromLeft(int bDrawMode)
{
    if (m_bStartFromLeft != bDrawMode)
    {
        m_bStartFromLeft = bDrawMode;
        SetExtent(m_cExtent);
    }
}
// PC 0x00418010  Xbox 0x002FD530
int CSWGuiScrollBar::GetTopArrowOn()
{
    if (m_nVisibleValue < m_nMaxValue)
    {
#ifdef _XBOX
        if (m_nXboxUnmapped48 >= 1)
#else
        if (m_nCurValue >= 1)
#endif
        {
            return 1;
        }
    }
    return 0;
}
// PC 0x00418030  Xbox 0x002FD550
int CSWGuiScrollBar::GetBottomArrowOn()
{
    int nLast = m_nMaxValue;
#ifdef _XBOX
    if (!m_bExtendedRange && m_nVisibleValue <= 1 &&
        m_nXboxUnmapped48 != m_nMaxValue && m_nCurValue != m_nMaxValue)
    {
        nLast++;
    }
    return m_nVisibleValue < m_nMaxValue &&
           m_nVisibleValue + m_nXboxUnmapped48 < nLast;
#else
    if (!m_bExtendedRange && m_nVisibleValue <= 1 && m_nCurValue != m_nMaxValue)
    {
        nLast++;
    }
    return m_nVisibleValue < m_nMaxValue &&
           m_nVisibleValue + m_nCurValue < nLast;
#endif
}

inline int CSWGuiScrollBar::GetMaxValue()
{
    return m_nMaxValue;
}

inline int CSWGuiScrollBar::GetVisibleValue()
{
    return m_nVisibleValue;
}
// PC 0x00418070  Xbox 0x002FD590
void CSWGuiScrollBar::SetMaxValue(int nMaxValue, bool bExtendedRange)
{
    if (m_nMaxValue != nMaxValue && nMaxValue >= 0)
    {
        m_nMaxValue = nMaxValue;
        m_bExtendedRange = bExtendedRange;
        SetExtent(m_cExtent);
    }
}
// PC 0x004180A0  Xbox 0x002FD5C0
void CSWGuiScrollBar::SetCurValue(int nValue)
{
    int nCurValue = nValue;
    if (nValue < 0)
    {
        nCurValue = 0;
    }
    else
    {
        if (nValue > m_nMaxValue)
        {
            nCurValue = m_nMaxValue;
        }
    }
    m_nCurValue = nCurValue;
    return SetExtent(m_cExtent);
}
// PC 0x004180D0  Xbox 0x002FD5F0
void CSWGuiScrollBar::SetVisibleValue(int nValue)
{
    if (m_nVisibleValue != nValue)
    {
        if (nValue < 0 || (nValue == 0 && m_nMaxValue != 0))
        {
            m_nVisibleValue = 1;
        }
#ifdef _XBOX
        else if (nValue > m_nMaxValue)
        {
            m_nVisibleValue = m_nMaxValue;
        }
#endif
        else
        {
            m_nVisibleValue = nValue;
        }
    }
    SetExtent(m_cExtent);
}
// PC 0x00418100  Xbox 0x002FE8E0
void CSWGuiScrollBar::Initialize(const CSWGuiExtent& cExtent,
                                 const CSWGuiBorderParams& cBorder,
                                 const CResRef& sArrow, const CResRef& sThumb)
{
    m_cExtent = cExtent;
    m_cBorder.Initialize(m_cExtent, cBorder);
    Vector vColor(1.0f, 1.0f, 1.0f);
    CSWGuiImageParams cParams(CResRef(), 0, 0, 0.0f, 18, 1.0f, vColor);
    cParams.SetImage(sArrow, 0);
    m_cArrow.Initialize(m_cExtent, cParams);
    cParams.SetImage(sThumb, 0);
    m_cThumb.Initialize(m_cExtent, cParams);
    return SetExtent(m_cExtent);
}
// PC 0x004181D0  Xbox 0x0004DC60
inline void CSWGuiControl::SetActive(int bActive)
{
    m_bActive = bActive;
}
// PC 0x004181F0  Xbox 0x002FEB90
void CSWGuiListBox::SetScrollBarExtent(int nWidth, int bUpdate)
{
    int nTotalWidth = m_cExtent.m_nWidth;
    if (nTotalWidth != 0)
    {
        if (nWidth > nTotalWidth)
        {
            nWidth = nTotalWidth;
        }
        CSWGuiExtent cBar;
        cBar.m_nTop = m_cExtent.m_nTop;
        cBar.m_nHeight = m_cExtent.m_nHeight;
        cBar.m_nWidth = nWidth;
        if (m_bScrollBarOnLeft)
        {
            cBar.m_nLeft = m_cExtent.m_nLeft;
        }
        else
        {
            cBar.m_nLeft = nTotalWidth + m_cExtent.m_nLeft - nWidth;
        }
        m_cScrollBar.SetExtent(cBar);
        if (bUpdate)
        {
            SetExtent(m_cExtent);
        }
    }
}
// PC 0x00418270  Xbox 0x002FEC10
void CSWGuiListBox::SetScrollBarOnLeft(int bLeft)
{
    if ((m_bScrollBarOnLeft != 0) == bLeft)
    {
        return;
    }
    m_bScrollBarOnLeft = bLeft;
    SetScrollBarExtent(m_cScrollBar.m_cExtent.m_nWidth, 1);
}
// PC 0x004182B0  Xbox 0x002FD730
int CSWGuiListBox::GetScrollRange()
{
    int nRange;
    if (m_cItemExtent.m_nHeight != 0)
    {
        nRange = (int)(m_nContentHeight * m_fScrollStep /
                       m_cItemExtent.m_nHeight + 0.9999f) - int(m_fScrollStep) + 1;
    }
    else
    {
        nRange = 1;
    }
    return nRange;
}
// PC 0x00418300  Xbox 0x002FD630
void CSWGuiListBox::UpdateArrowPulsing(float fElapsed)
{
    float fFrom;
    float fTo;

    m_fArrowAlphaPulseTime = m_fArrowAlphaPulseTime - fElapsed;
    if (m_fArrowAlphaPulseTime <= 0.0f)
    {
        m_bArrowAlphaPulseUp = m_bArrowAlphaPulseUp == 0;
        m_fArrowAlphaPulseTime = 0.75f;
    }
    if (m_bArrowAlphaPulseUp)
    {
        fTo = 1.0f;
        fFrom = 0.2f;
    }
    else
    {
        fTo = 0.2f;
        fFrom = 1.0f;
    }
    float fProgress = (0.75f - m_fArrowAlphaPulseTime) * 1.3333334f;
    m_fArrowAlpha = (fTo - fFrom) * fProgress + fFrom;
}
// PC 0x00418380  Xbox 0x002FF240
void CSWGui3DSceneView::SetExtent(const CSWGuiExtent& cExtent)
{
    m_cScene.SetExtent(cExtent);
    m_cExtent = cExtent;
}
// PC 0x004183B0
void CSWGuiEditbox::Initialize(const CSWGuiExtent& cExtent,
                               const CSWGuiTextParams& cText,
                               const CSWGuiBorderParams& cBorder)
{
    m_cExtent = cExtent;
    m_cBorder.Initialize(m_cExtent, cBorder);
    CSWGuiExtent cInner = m_cBorder.GetInnerExtent();
    m_cText.Initialize(cInner, cText, 1.0f);
}
// PC 0x00418420
void CSWGuiEditbox::Draw(float fElapsed)
{
    m_cBorder.Draw(fElapsed);
    m_cText.Draw(fElapsed);
}
// PC 0x00418450
void CSWGuiEditbox::SetExtent(const CSWGuiExtent& cExtent)
{
    m_cExtent = cExtent;
    m_cBorder.SetExtent(m_cExtent);
    CSWGuiExtent cInner = m_cBorder.GetInnerExtent();
    m_cText.SetExtent(cInner);
}
// PC 0x004184B0
void CSWGuiEditbox::SetEnabled(int bEnabled)
{
    CSWGuiControl::SetEnabled(bEnabled);
    if (bEnabled)
    {
        m_cText.m_cParams.RestoreDefaultColor();
    }
    else
    {
        m_cText.m_cParams.SetColor(CSWGuiManager::DISABLED_TEXT);
    }
}
// PC 0x004184F0
void CSWGuiEditbox::HandleKeyPress(int nKey)
{
    static char szTmp[2];

    switch (nKey)
    {
    case 28:
        HandleInputEvent(GUIEVENT_LEFT, 1);
        break;
    case 29:
        HandleInputEvent(GUIEVENT_UP, 1);
        break;
    case 30:
        HandleInputEvent(GUIEVENT_RIGHT, 1);
        break;
    case 31:
        HandleInputEvent(GUIEVENT_DOWN, 1);
        break;
    case 8:
    case 127:
        m_cText.RemoveLastChar();
        break;
    case 27:
        if (m_pOwner != NULL)
        {
            CSWGuiPanel* pPanel = m_pOwner->AsSWGuiPanel();
            if (pPanel != NULL)
            {
                pPanel->HandleInputEvent(GUIEVENT_CANCEL, 1);
            }
        }
        break;
    case 10:
    case 13:
        if (m_pOwner != NULL)
        {
            CSWGuiPanel* pPanel = m_pOwner->AsSWGuiPanel();
            if (pPanel != NULL)
            {
                pPanel->HandleInputEvent(GUIEVENT_ACTIVATE, 1);
            }
        }
        break;
    default:
        if (nKey != '_' && nKey != '/' && nKey != '\\' && nKey >= ' ')
        {
            szTmp[0] = (char)nKey;
            szTmp[1] = 0;
            m_cText.AddNewChar(szTmp);
        }
        break;
    }
}
// PC 0x00418660
void CSWGuiEditbox::SetFocus()
{
    HandleFocusChange(1);
}
// PC 0x00418670
int CSWGuiEditbox::GetIsSelectable()
{
#ifndef _XBOX
    if (m_bHilightOnOver)
    {
        return 1;
    }
#endif
    return 0;
}
// PC 0x00418680
void CSWGuiEditbox::HandleLMouseDown()
{
#ifndef _XBOX
    if (m_bHilightOnOver)
    {
        HandleInputEvent(GUIEVENT_MOUSE_ENTER, 1);
        HandleFocusChange(1);
        g_pGuiMan->CaptureMouse(this, CSWGuiManager::MOUSECAPTURE_LEFT);
    }
#endif
}
// PC 0x00418680
void CSWGuiRadioButton::HandleLMouseDown()
{
#ifndef _XBOX
    if (m_bHilightOnOver)
    {
        HandleInputEvent(GUIEVENT_MOUSE_ENTER, 1);
        HandleFocusChange(1);
        g_pGuiMan->CaptureMouse(this, CSWGuiManager::MOUSECAPTURE_LEFT);
    }
#endif
}
// PC 0x004186B0
void CSWGuiEditbox::HandleLMouseUp()
{
#ifndef _XBOX
    g_pGuiMan->ReleaseMouse(this);
#endif
    HandleLoseMouseFocus();
}
// PC 0x004186D0
void CSWGuiEditbox::ReSetFont()
{
    m_cText.m_cParams.ReSetFont();
}

#ifndef _XBOX
template <> inline void CExoArrayList<CSWGuiControlEvent>::Allocate(int nSize)  // PC 0x004186E0  Xbox 0x002FD6C0
{
    CSWGuiControlEvent* pOld = element;

    array_size = nSize;
    element = new CSWGuiControlEvent[nSize];

    for (int i = 0; i < num; i++)
    {
        element[i] = pOld[i];
    }

    if (pOld != NULL)
    {
        delete[] pOld;
    }
}
#endif
// PC 0x0041A980
void CSWGuiNavigable::HandleMouseOver(int bOver)
{
    if (bOver)
    {
        if (!m_bActive)
        {
            HandleInputEvent(EVENT_ENTER, 1);
            g_pGuiMan->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
        }
    }
    else
    {
        HandleInputEvent(EVENT_EXIT, 1);
    }
    CSWGuiControl::HandleMouseOver(bOver);
}
#ifndef _XBOX
void CSWGuiNavigable::HandleInputEvent(int nEvent, int nState)  // PC 0x0041A9D0  Xbox 0x002FF330
{
    if (nState)
    {
        CSWGuiControl* pNext = this;
        while (pNext != NULL)
        {
            if (pNext->AsNavigable() != NULL)
            {
                CSWGuiControl** apMoveTo =
                    pNext->AsNavigable()->m_apMoveToControl;
                switch (nEvent)
                {
                case 50:
                case 62:
                    pNext = apMoveTo[2];
                    break;
                case 49:
                case 61:
                    pNext = apMoveTo[0];
                    break;
                case 47:
                case 63:
                    pNext = apMoveTo[1];
                    break;
                case 48:
                case 64:
                    pNext = apMoveTo[3];
                    break;
                default:
                    break;
                }
                if (pNext == NULL)
                {
                    break;
                }
#ifndef _XBOX
                if (!pNext->m_bHilightOnOver && pNext != this)
                {
                    continue;
                }
#endif
            }
            if (m_pOwner != NULL && pNext != this)
            {
                m_pOwner->SetActiveControl(pNext, 1);
            }
            break;
        }
    }
    return CSWGuiControl::HandleInputEvent(nEvent, nState);
}
#endif
CSWGuiControl::CSWGuiControl()  // PC 0x0041AA80  Xbox 0x002FF3C0
#ifndef _XBOX
    : m_lChildren(0), m_lEvents(0)
#else
    : m_lEvents(0)
#endif
{
    m_bActive = 0;
    m_bEnabled = 1;
#ifndef _XBOX
    m_bHilightOnOver = 1;
#endif
#ifndef _XBOX
    m_bControlState4 = 0;
    m_bIgnoreMouse = 0;
#endif
#ifndef _XBOX
    m_bToolTipShown = 0;
#endif
    m_pOwner = NULL;
    m_nIndex = 0;
    m_nClickSound = 0;
#ifndef _XBOX
    m_pParent = NULL;
#endif
#ifndef _XBOX
    m_nToolTipStrRef = 0;
    m_nToolTipInputEvent = 0;
#endif
    m_nID = -1;
    m_nFocusSound = 1;
}
// PC 0x0041AB20  Xbox 0x002FF460
void CSWGuiControl::AddEvent(int nEvent, CSWGuiObject* pObject,
                             CSWGuiHandler pHandler)
{
    for (int i = 0; i < m_lEvents.num; i++)
    {
        CSWGuiControlEvent* pEvent = &m_lEvents.element[i];

        if (pEvent->m_nEvent == nEvent)
        {
            if (pHandler)
            {
                pEvent->m_pObject = pObject;
                pEvent->m_pHandler = pHandler;
            }
            else
            {
                m_lEvents.DelIndex(i);
            }
            return;
        }
    }

    if (pHandler)
    {
        CSWGuiControlEvent cEvent;

        cEvent.m_pObject = pObject;
        cEvent.m_pHandler = pHandler;
        cEvent.m_nEvent = nEvent;
        m_lEvents.Add(cEvent);
    }
}
// PC 0x00418750  Xbox 0x002FD810
void CSWGuiControl::HandleInputEvent(int nEvent, int nState)
{
    if (nState)
    {
        switch (nEvent)
        {
        case 0:
            SetActive(1);
            break;

        case 1:
            if (m_bActive)
            {
                SetActive(0);
            }
            break;
        }
    }

    int nEventCount = m_lEvents.num;
    for (int i = 0; i < nEventCount; i++)
    {
        CSWGuiControlEvent* pEvent = &m_lEvents.element[i];
        if (pEvent->m_nEvent == nEvent && pEvent->m_pHandler != NULL)
        {
            m_nEvent = nEvent;
            m_bSelected = nState;
            return (pEvent->m_pObject->*pEvent->m_pHandler)(this);
        }
    }
}
#ifndef _XBOX
CSWGuiControl* CSWGuiControl::HitCheckMouse(int nX, int nY)  // PC 0x004187C0
{
#ifndef _XBOX
    if (!m_bEnabled || m_bIgnoreMouse ||
        nX < m_cExtent.m_nLeft ||
        nX > m_cExtent.GetRight() ||
        nY < m_cExtent.m_nTop ||
        nY > m_cExtent.GetBottom())
    {
        return NULL;
    }
#endif
    return this;
}
#endif
void CSWGuiControl::Load(CSWGuiObject* pParent, CResGFF* pGFF,  // PC 0x00418800  Xbox 0x002FD8C0
                         CResStruct& cStruct, const CExoString& sName)
{
    CResStruct cChild;

    m_pOwner = pParent;
    if (pGFF->GetStructFromStruct(&cChild, &cStruct, (char*)sName.CStr()))
    {
        Load(pGFF, cChild);
    }
}
// PC 0x00418840  Xbox 0x002FD900
void CSWGuiControl::Load(CSWGuiObject* pParent, CResGFF* pGFF,
                         CResList& lControls, const CExoString& sName)
{
    CResStruct cStruct;
    int bSuccess;

    m_pOwner = pParent;
    int nCount = pGFF->GetListCount(&lControls);
    CExoString sTag;
    for (int i = 0; i < nCount; i++)
    {
        pGFF->GetListElement(&cStruct, &lControls, i);
        sTag = pGFF->ReadFieldCExoString(&cStruct, "TAG", bSuccess, "");
        if (sTag == sName)
        {
            Load(pGFF, cStruct);
            break;
        }
    }
}
// PC 0x0041AB90
void CSWGuiControl::HandleLMouseDown()
{
    if (GetIsSelectable())
    {
#ifndef _XBOX
        g_pGuiMan->CaptureMouse(this, CSWGuiManager::MOUSECAPTURE_LEFT);
#endif
        return;
    }
    CSWGuiControl* pParent = GetSelectableParent();
    if (pParent != NULL)
    {
        pParent->HandleLMouseDown();
    }
    else
    {
#ifndef _XBOX
        g_pGuiMan->CaptureMouse(this, CSWGuiManager::MOUSECAPTURE_LEFT);
#endif
    }
}
// PC 0x0041ABD0
void CSWGuiControl::HandleLMouseUp()
{
#ifndef _XBOX
    CSWGuiControl* pOver = g_pGuiMan->m_pMouseOverControl;
#endif
#ifndef _XBOX
    if (pOver != NULL &&
        (pOver == this || pOver->GetSelectableParent() == this))
    {
#ifndef _XBOX
        if (g_pGuiMan->m_bLeftMouseDown)
        {
            HandleInputEvent(505, 1);
        }
#endif
#ifndef _XBOX
        if (m_bHilightOnOver)
        {
            HandleInputEvent(GUIEVENT_ACTIVATE, 1);
        }
#endif
    }
#endif
#ifndef _XBOX
    g_pGuiMan->ReleaseMouse(this);
#endif
    HandleLoseMouseFocus();
}
// PC 0x0041AC30
void CSWGuiControl::HandleRMouseDown()
{
    if (GetIsSelectable())
    {
#ifndef _XBOX
        g_pGuiMan->CaptureMouse(this, 4);
#endif
        return;
    }
    CSWGuiControl* pParent = GetSelectableParent();
    if (pParent != NULL)
    {
        pParent->HandleRMouseDown();
    }
    else
    {
#ifndef _XBOX
        g_pGuiMan->CaptureMouse(this, 4);
#endif
    }
}
// PC 0x0041AC70
void CSWGuiControl::HandleRMouseUp()
{
#ifndef _XBOX
    CSWGuiControl* pOver = g_pGuiMan->m_pMouseOverControl;
#endif
#ifndef _XBOX
    if (pOver != NULL &&
        (pOver == this || pOver->GetSelectableParent() == this) &&
        m_bHilightOnOver)
    {
        HandleInputEvent(GUIEVENT_RIGHT_CLICK, 1);
    }
#endif
#ifndef _XBOX
    g_pGuiMan->ReleaseMouse(this);
#endif
    HandleLoseMouseFocus();
}
// PC 0x0041ACC0
void CSWGuiControl::AddChildControl(CSWGuiControl* pChild)
{
#ifndef _XBOX
    m_lChildren.Add(pChild);
#endif
}
// PC 0x0041ACD0  Xbox 0x0004DBF0
CSWGuiLabel::CSWGuiLabel()
{
    m_bFocusable = 0;
}
// PC 0x0041AD40  Xbox 0x002FF4D0
void CSWGuiButton::HandleInputEvent(int nEvent, int nState)
{
    if (nState && nEvent == EVENT_SELECT && m_pOwner != NULL)
    {
        CSWGuiManager* pManager = NULL;
        if (m_pOwner->AsSWGuiPanel() != NULL)
        {
            CSWGuiPanel* pPanel = m_pOwner->AsSWGuiPanel();
            pManager = pPanel->m_pManager;
        }
        else if (m_pOwner->AsSWGuiListBox() != NULL)
        {
            CSWGuiListBox* pListBox = m_pOwner->AsSWGuiListBox();
            CSWGuiPanel* pPanel = pListBox->m_pOwner->AsSWGuiPanel();
            pManager = pPanel->m_pManager;
        }
#ifdef _XBOX
        else
        {
            EXOXBOXASSERT(0);
        }
#endif
        if (pManager != NULL)
        {
            pManager->PlayGuiSound(m_nClickSound);
        }
    }
    return CSWGuiNavigable::HandleInputEvent(nEvent, nState);
}
// PC 0x0041ADB0  Xbox 0x002FF550
void CSWGuiButtonToggle::HandleInputEvent(int nEvent, int nState)
{
    if (nState && nEvent == m_nToggleEvent)
    {
        m_bToggled = m_bToggled == 0;
    }
    return CSWGuiButton::HandleInputEvent(nEvent, nState);
}
// PC 0x0041ADF0  Xbox 0x002FF590
void CSWGuiSlider::HandleInputEvent(int nEvent, int nState)
{
    if (nState)
    {
        int bChanged = 0;
        if (m_cExtent.m_nHeight > m_cExtent.m_nWidth)
        {
            switch (nEvent)
            {
            case GUIEVENT_UP: case GUIEVENT_UP_ALTERNATE:
#ifndef _XBOX
            case GUIEVENT_SLIDER_DECREMENT:
#endif
                bChanged = m_nCurValue != 0; SetCurValue(m_nCurValue - 1);
                break;
            case GUIEVENT_DOWN: case GUIEVENT_DOWN_ALTERNATE:
#ifndef _XBOX
            case GUIEVENT_SLIDER_INCREMENT:
#endif
                bChanged = m_nCurValue != m_nMaxValue; SetCurValue(m_nCurValue + 1);
                break;
            }
        }
        else
        {
            switch (nEvent)
            {
            case GUIEVENT_LEFT: case GUIEVENT_LEFT_ALTERNATE:
#ifndef _XBOX
            case GUIEVENT_SLIDER_DECREMENT:
#endif
                bChanged = m_nCurValue != 0; SetCurValue(m_nCurValue - 1);
                break;
            case GUIEVENT_RIGHT: case GUIEVENT_RIGHT_ALTERNATE:
#ifndef _XBOX
            case GUIEVENT_SLIDER_INCREMENT:
#endif
                bChanged = m_nCurValue != m_nMaxValue; SetCurValue(m_nCurValue + 1);
                break;
            }
        }
        if (bChanged && m_pOwner != NULL)
        {
            CSWGuiManager* pManager = NULL;
            if (m_pOwner->AsSWGuiPanel() != NULL)
            {
                CSWGuiPanel* pPanel = m_pOwner->AsSWGuiPanel();
                pManager = pPanel->m_pManager;
            }
            else if (m_pOwner->AsSWGuiListBox() != NULL)
            {
                CSWGuiListBox* pListBox = m_pOwner->AsSWGuiListBox();
                CSWGuiPanel* pPanel = pListBox->m_pOwner->AsSWGuiPanel();
                pManager = pPanel->m_pManager;
            }
            if (pManager != NULL)
            {
                pManager->PlayGuiSound(m_nSlideSound);
            }
        }
    }
    return CSWGuiNavigable::HandleInputEvent(nEvent, nState);
}
// PC 0x0041AED0  Xbox 0x002FF6C0
CSWGuiProgressBar::CSWGuiProgressBar()
{
    m_bStartFromLeft = 1;
#ifndef _XBOX
    m_bFocusable = 0;
#endif
    m_nMaxValue = 0; m_nCurValue = 0;
}
CSWGuiScrollBar::CSWGuiScrollBar()  // PC 0x0041AFE0  Xbox 0x002FF760
{
    m_nMaxValue = 0;
    m_nCurValue = 0;
    m_nVisibleValue = 0;
#ifdef _XBOX
    m_nXboxUnmapped48 = 0;
#endif
    m_bDrawMode = 0;
}
// PC 0x0041B060  Xbox 0x002FF800
CSWGuiListBox::~CSWGuiListBox()
{
    if (m_pProtoItem != NULL)
    {
        delete m_pProtoItem;
        m_pProtoItem = NULL;
    }
    ClearItems();
}
#ifndef _XBOX
void CSWGuiListBox::OrganizeControls()  // PC 0x0041B140  Xbox 0x002FF8B0
{
    int nItems = m_lItems.num;
    if (nItems == 0)
    {
        if (m_bOrganizePending)
        {
            m_cScrollBar.SetMaxValue(1, 0);
            m_cScrollBar.SetCurValue(1);
            m_cScrollBar.SetVisibleValue(1);
            m_bOrganizePending = 0;
        }
        return;
    }
    if (m_bHideScrollIcon)
        return OrganizeUnequal();

    int nStride = m_nContentHeight + m_nPadding;
    if (m_bOrganizePending)
    {
        m_nContentHeight = 0;
        for (int i = 0; i < nItems; i++)
        {
            m_lItems.element[i]->SetExtent(m_lItemExtents.element[i]);
            int nHeight = m_lItems.element[i]->GetIdealHeight();
            if (nHeight > m_nContentHeight)
                m_nContentHeight = nHeight;
        }
        m_nTopVisibleItem = 0;
        if (m_nContentHeight == 0)
        {
            m_cScrollBar.SetMaxValue(1, 0);
            m_cScrollBar.SetCurValue(1);
            m_cScrollBar.SetVisibleValue(1);
            return;
        }
        nStride = m_nContentHeight + m_nPadding;
        m_nVisibleItems = m_cItemExtent.m_nHeight / nStride;
        if (m_nVisibleItems == 0)
        {
            EXOXBOXASSERT(IsOversized());
            m_cScrollBar.SetMaxValue(m_nContentHeight - m_cItemExtent.m_nHeight + 1,
                                    m_bScrollOnly);
            m_cScrollBar.SetVisibleValue(m_cItemExtent.m_nHeight);
        }
        else if (m_bScrollOnly)
        {
            m_cScrollBar.SetMaxValue(max(nItems - m_nVisibleItems + 1, 1), m_bScrollOnly);
            m_cScrollBar.SetVisibleValue(1);
        }
        else
        {
            m_cScrollBar.SetMaxValue(max(nItems - m_nVisibleItems + 1, 1), m_bScrollOnly);
            m_cScrollBar.SetVisibleValue(min(m_nVisibleItems, nItems));
        }
        if (IsOversized())
        {
            CSWGuiLabel* pLabel = m_lItems.element[0]->AsLabel();
            if (pLabel)
            {
                int nFontHeight = pLabel->m_cText.GetFontHeight();
                if (nFontHeight)
                    m_fScrollStep = (float)m_cItemExtent.m_nHeight / nFontHeight;
            }
        }
        m_bOrganizePending = 0;
    }
    if (IsOversized())
        return OrganizeOversized();

    if (m_bScrollToSelected && m_nSelectedItem != -1 &&
        (m_nSelectedItem < m_nTopVisibleItem ||
         m_nSelectedItem >= m_nTopVisibleItem + m_nVisibleItems))
    {
        m_nTopVisibleItem = m_nSelectedItem < m_nTopVisibleItem ? m_nSelectedItem :
                           (short)max(0, m_nSelectedItem - m_nVisibleItems + 1);
    }
    m_cScrollBar.SetCurValue(m_nTopVisibleItem);
    int nExtra = m_cItemExtent.m_nHeight - nStride * m_nVisibleItems - m_nPadding;
    int nExtraHeight = nExtra / m_nVisibleItems;
    nExtra -= m_nVisibleItems * nExtraHeight;
    CSWGuiExtent cExtent;
    cExtent.m_nLeft = m_nPadding;
    cExtent.m_nWidth = m_cItemExtent.m_nWidth - 2 * m_nPadding;
    cExtent.m_nHeight = m_nContentHeight;
    int nTop = m_nPadding - nStride * m_nTopVisibleItem;
    for (int i = 0; i < m_nTopVisibleItem; i++, nTop += nStride)
    {
        cExtent.m_nTop = nTop;
        m_lItems.element[i]->SetExtent(cExtent);
    }
    int nEnd = min(m_nTopVisibleItem + m_nVisibleItems, nItems);
    int nHeight = nExtraHeight + m_nContentHeight;
    for (int i = m_nTopVisibleItem; i < nEnd; i++)
    {
        cExtent.m_nHeight = nHeight;
        cExtent.m_nTop = nTop;
        if (nExtra)
        {
            cExtent.m_nHeight = nHeight + 1;
            nTop++;
            nExtra--;
        }
        m_lItems.element[i]->SetExtent(cExtent);
        nTop += nHeight + m_nPadding;
    }
    cExtent.m_nHeight = m_nContentHeight;
    for (int i = m_nTopVisibleItem + m_nVisibleItems; i < nItems; i++, nTop += nStride)
    {
        cExtent.m_nTop = nTop;
        m_lItems.element[i]->SetExtent(cExtent);
    }
}
#endif
void CSWGuiListBox::LoadProtoItem(CResGFF* pGFF, CResStruct& cStruct)  // PC 0x0041D3E0  Xbox 0x00300860
{
    CResStruct cProtoItem;
    int bSuccess;

    if (pGFF->GetStructFromStruct(&cProtoItem, &cStruct, "PROTOITEM"))
    {
        m_nCONTROLTYPE = pGFF->ReadFieldINT(&cProtoItem, "CONTROLTYPE", bSuccess, 0);
        switch (m_nCONTROLTYPE)
        {
        case CONTROL_LABEL:
            m_pProtoItem = new CSWGuiLabel;
            break;
        case CONTROL_LABEL_HILIGHT:
            m_pProtoItem = new CSWGuiLabelHilight;
            break;
        case CONTROL_BUTTON:
            m_pProtoItem = new CSWGuiButton;
            break;
        case CONTROL_BUTTON_TOGGLE:
            m_pProtoItem = new CSWGuiButtonToggle;
            break;
        case CONTROL_SLIDER:
            m_pProtoItem = new CSWGuiSlider;
            break;
        }
        if (m_pProtoItem != NULL)
        {
            m_pProtoItem->Load(this, pGFF, cStruct, "PROTOITEM");
        }
    }
}
// PC 0x0041D5B0  Xbox 0x00300A40
void CSWGuiListBox::Load(CResGFF* pGFF, CResStruct& cStruct)
{
    int bSuccess;
    m_cBorder.Load(pGFF, cStruct, "BORDER");
    m_cScrollBar.Load(this, pGFF, cStruct, "SCROLLBAR");
    SetScrollBarOnLeft(pGFF->ReadFieldBYTE(&cStruct, "LEFTSCROLLBAR", bSuccess, 1));
    SetPadding(pGFF->ReadFieldINT(&cStruct, "PADDING", bSuccess, 0));
    m_vColor = pGFF->ReadFieldVector(&cStruct, "COLOR", bSuccess, Vector(0.0f, 0.0f, 0.0f));
    int bLooping = pGFF->ReadFieldBYTE(&cStruct, "LOOPING", bSuccess, 0);
    m_bLooping = bLooping;
    CSWGuiNavigable::Load(pGFF, cStruct);
    LoadProtoItem(pGFF, cStruct);
}
// PC 0x0041B670
int CSWGuiListBox::HandleMouseCapturedMovement(int nX, int nY)
{
    int nMouseX;
    int nMouseY;

#ifndef _XBOX
    if ((g_pGuiMan->m_nMouseCaptureType & CSWGuiManager::MOUSECAPTURE_LEFT) &&
        m_nScrollBarPart == CSWGuiScrollBar::SCROLLBAR_THUMB)
    {
        ((CSWGuiPanel*)m_pOwner)->GetLocalMouseCoords(&nMouseX, &nMouseY);
        CSWGuiScrollBar* pScrollBar = &m_cScrollBar;
        if (pScrollBar->m_cExtent.m_nHeight > pScrollBar->m_cExtent.m_nWidth)
        {
            int nDeltaY = nMouseY - m_nDragStartY;
            CSWGuiExtent cThumbExtent = m_cScrollBar.m_cThumb.m_cExtent;
            CSWGuiExtent cInnerExtent = m_cScrollBar.m_cBorder.GetInnerExtent();
            int nTravel = cInnerExtent.m_nHeight - cThumbExtent.m_nHeight;
            int nRange;

            if (IsOversized())
            {
                nRange = GetScrollRange() - 1;
                m_nScrollPosition = (short)(nRange * ((float)nDeltaY / nTravel) +
                                          m_nDragStartPosition + 0.5f);
                m_nScrollPosition = m_nScrollPosition < 1 ? 1 : m_nScrollPosition;
                m_nScrollPosition = GetScrollRange() < m_nScrollPosition ?
                                    GetScrollRange() : m_nScrollPosition;
                OrganizeOversized();
                AurPrintf("pos %d/%d, dY %d (%d-%d)", m_nScrollPosition,
                          GetScrollRange(), nDeltaY, nMouseY, m_nDragStartY);
            }
            else
            {
                nRange = m_cScrollBar.m_nMaxValue;
                int nPosition = (int)((float)nDeltaY / nTravel * (nRange - 1) +
                                      m_nDragStartPosition + 0.5f);
                nPosition = nPosition < 0 ? 0 : nPosition;
                nPosition = nRange - 1 < nPosition ? nRange - 1 : nPosition;
                m_bScrollToSelected = 0;
                m_nTopVisibleItem = nPosition;
                pScrollBar->SetCurValue(nPosition);
                OrganizeControls();
            }
        }
    }
#endif
    return 1;
}
// PC 0x00418960
void CSWGuiControl::HandleFocusChange(int bFocus)
{
    if (bFocus && m_pOwner != NULL)
    {
        CSWGuiPanel* pPanel = m_pOwner->AsSWGuiPanel();
        if (pPanel != NULL)
        {
            if (!m_bActive)
            {
                g_pGuiMan->PlayGuiSound(CSWGuiManager::GUISOUND_SELECTION);
            }
            pPanel->SetActiveControl(this, 0);
        }
    }
}
// PC 0x004189A0
void CSWGuiControl::HandleMouseOver(int bOver)
{
    if (bOver)
    {
#ifndef _XBOX
        g_pGuiMan->ResetToolTipTimer();
#endif
        return;
    }
#ifndef _XBOX
    if (m_bToolTipShown)
    {
        HideToolTip();
    }
#endif
}
// PC 0x004189D0
int CSWGuiControl::GetIsSelectable()
{
    CSWGuiObject* pOwner = m_pOwner;
    if (pOwner != NULL && pOwner->AsSWGuiListBox() != NULL)
    {
        CSWGuiListBox* pListBox = pOwner->AsSWGuiListBox();
#ifndef _XBOX
        if (pListBox->m_bHilightOnOver)
        {
            return AsNavigable() != NULL;
        }
#endif
    }
#ifndef _XBOX
    else if (m_bHilightOnOver)
#else
    else
#endif
    {
        return AsNavigable() != NULL;
    }
    return 0;
}

#ifndef _XBOX
int CSWGuiControl::GetIsChild(CSWGuiControl* pControl)  // PC 0x00418A20
{
    for (int i = 0; i < m_lChildren.num; i++)
    {
        if (m_lChildren.element[i] == pControl)
        {
            return 1;
        }
    }
    return 0;
}
#endif

#ifndef _XBOX
CSWGuiControl* CSWGuiControl::GetSelectableParent()  // PC 0x00418A50
{
    CSWGuiControl** pp = &m_pParent;
    CSWGuiControl* pNext = *pp;
    while (pNext != NULL)
    {
        CSWGuiControl* p = *pp;
        if (p->GetIsSelectable())
        {
            return p;
        }
        pp = &p->m_pParent;
        pNext = p->m_pParent;
    }
    return NULL;
}
#else
// PC 0x00418A50
CSWGuiControl* CSWGuiControl::GetSelectableParent()
{
    return NULL;
}
#endif
// PC 0x00418A90
int CSWGuiControl::DisplayToolTip()
{
    int nKeyNameStrRef;
#ifndef _XBOX
    if (m_nToolTipStrRef == 0 && m_sToolTip == "")
    {
        if (m_pParent != NULL)
        {
            return m_pParent->DisplayToolTip();
        }
        return 0;
    }
#endif

    CExoString sToolTip;
#ifndef _XBOX
    sToolTip = m_nToolTipStrRef == 0 ? m_sToolTip :
        g_pTlkTable->GetSimpleString(m_nToolTipStrRef);
#endif

#ifndef _XBOX
    if (m_nToolTipInputEvent != 0)
    {
        CExoInputEventDesc* pEvent = g_pExoInput->GetEvent(
            m_nToolTipInputEvent, g_pAppManager->GetClientExoApp()->GetInputClass());
        if (pEvent != NULL)
        {
            int bFound;
            {
                CExoString sColumn("KeyNameStrRef");
                unsigned long nControl = pEvent->m_nControl;
                CTwoDimArrays* p2DArrays = g_pRules->m_p2DArrays;
                C2DA* pBindableKey = p2DArrays->m_pBindableKey;
                bFound = pBindableKey->GetINTEntry(
                    nControl - CExoInput::KEYBOARD_START,
                    sColumn, &nKeyNameStrRef);
            }
            if (bFound)
            {
                sToolTip = sToolTip + " : " +
                    g_pTlkTable->GetSimpleString(nKeyNameStrRef);
            }
        }
    }
#endif

#ifndef _XBOX
    g_pGuiMan->DisplayToolTip(sToolTip);
#endif
#ifndef _XBOX
    m_bToolTipShown = 1;
#endif
    return 1;
}

#ifdef _XBOX
void CSWGuiLabel::Initialize(const CSWGuiExtent& cExtent, CSWGuiLabel* pProto)  // Xbox 0x002FDA20
{
    Initialize(cExtent, pProto->m_cText.m_cParams, pProto->m_cBorder.m_cParams);
}
#else
void CSWGuiLabel::Initialize(const CSWGuiExtent& cExtent, CSWGuiLabel* pProto,  // PC 0x00418CD0
                             float fScale)
{
    Initialize(cExtent, pProto->m_cText.m_cParams, pProto->m_cBorder.m_cParams,
               fScale);
}
#endif
// PC 0x00418D00
void CSWGuiLabel::SetEnabled(int bEnabled)
{
    CSWGuiControl::SetEnabled(bEnabled);
    if (bEnabled)
    {
        m_cText.m_cParams.RestoreDefaultColor();
    }
    else
    {
        m_cText.m_cParams.SetColor(CSWGuiManager::DISABLED_TEXT);
    }
}
// PC 0x00418D40
void CSWGuiLabel::ReSetFont()
{
    m_cText.m_cParams.ReSetFont();
}
// PC 0x00418D50  Xbox 0x002FDA40
void CSWGuiLabelHilight::Initialize(const CSWGuiExtent& cExtent,
                                    CSWGuiLabelHilight* pProto)
{
    Initialize(cExtent, pProto->m_cText.m_cParams, pProto->m_cBorder.m_cParams,
               pProto->m_cHilite.m_cParams);
}
// PC 0x00418D80  Xbox 0x002FDA70
void CSWGuiButton::Initialize(const CSWGuiExtent& cExtent, CSWGuiButton* pProto)
{
    Initialize(cExtent, pProto->m_cText.m_cParams, pProto->m_cBorder.m_cParams,
               pProto->m_cHilite.m_cParams);
}
// PC 0x00418DB0
void CSWGuiButton::SetEnabled(int bEnabled)
{
    CSWGuiControl::SetEnabled(bEnabled);
    if (bEnabled)
    {
        m_cText.m_cParams.RestoreDefaultColor();
    }
    else
    {
        m_cText.m_cParams.SetColor(CSWGuiManager::DISABLED_TEXT);
        SetActive(0);
    }
}
// PC 0x00418DF0
void CSWGuiButton::ReSetFont()
{
    m_cText.m_cParams.ReSetFont();
}
// PC 0x00418E00
void CSWGuiButton::SetActive(int bActive)
{
    CSWGuiControl::SetActive(bActive);
    CSWGuiTextParams* pText = &m_cText.m_cParams;
    if (bActive)
    {
        pText->m_bHilightText = 1;
        pText->SetColor(CSWGuiManager::COLOR_YELLOW);
    }
    else
    {
        pText->m_bHilightText = 0;
        pText->RestoreDefaultColor();
    }
}
// PC 0x00418E40  Xbox 0x002FDC90
void CSWGuiButtonToggle::Initialize(const CSWGuiExtent& cExtent,
                                    CSWGuiButtonToggle* pProto)
{
    Initialize(cExtent, pProto->m_cText.m_cParams, pProto->m_cBorder.m_cParams,
               pProto->m_cHilite.m_cParams, pProto->m_cToggleBorder.m_cParams,
               pProto->m_cToggleHilite.m_cParams);
}
// PC 0x00418E80
void CSWGuiRadioButton::HandleMouseOver(int bOver)
{
    CSWGuiControl::HandleMouseOver(bOver);
}
// PC 0x00418E90
void CSWGuiRadioButton::HandleLMouseUp()
{
#ifndef _XBOX
    CSWGuiControl* pOver = g_pGuiMan->m_pMouseOverControl;
    if (pOver != NULL &&
        (pOver == this || pOver->GetSelectableParent() == this) &&
        g_pGuiMan->m_bLeftMouseDown)
    {
        HandleInputEvent(GUIEVENT_ACTIVATE, 1);
        HandleInputEvent(505, 1);
    }
    g_pGuiMan->ReleaseMouse(this);
#endif
    HandleLoseMouseFocus();
}
// PC 0x00418EF0  Xbox 0x002FDD70
void CSWGuiSlider::Initialize(const CSWGuiExtent& cExtent, CSWGuiSlider* pProto)
{
    Initialize(cExtent, pProto->m_cTrack.m_cParams, pProto->m_cThumb.m_cParams,
               pProto->m_cImage.m_cParams.m_cImage);
}
// PC 0x00418F20  Xbox 0x002FDDA0
void CSWGuiSlider::SetExtent(const CSWGuiExtent& cExtent)
{
    CSWGuiExtent cThumb;
    cThumb.m_nHeight = 0;
    cThumb.m_nWidth = 0;
    cThumb.m_nTop = 0;
    cThumb.m_nLeft = 0;
    m_cExtent = cExtent;
    m_cTrack.SetExtent(m_cExtent);
    m_cThumb.SetExtent(m_cExtent);
    cThumb = m_cImage.GetImageExtent();
    if (m_nMaxValue > 0)
    {
        if (m_cExtent.m_nHeight > m_cExtent.m_nWidth)
        {
            cThumb.m_nWidth = m_cExtent.m_nWidth;
            cThumb.m_nLeft = m_cExtent.m_nLeft;
            if (m_cExtent.m_nHeight > cThumb.m_nHeight)
            {
                cThumb.m_nTop = m_cExtent.m_nTop +
                                m_nCurValue *
                                    (m_cExtent.m_nHeight - cThumb.m_nHeight) /
                                    m_nMaxValue;
            }
            else
            {
                cThumb.m_nTop = 0;
                cThumb.m_nHeight = m_cExtent.m_nHeight;
            }
        }
        else
        {
            cThumb.m_nHeight = m_cExtent.m_nHeight;
            cThumb.m_nTop = m_cExtent.m_nTop;
            if (m_cExtent.m_nWidth > cThumb.m_nWidth)
            {
                cThumb.m_nLeft = m_cExtent.m_nLeft +
                                 m_nCurValue *
                                     (m_cExtent.m_nWidth - cThumb.m_nWidth) /
                                     m_nMaxValue;
            }
            else
            {
                cThumb.m_nLeft = 0;
                cThumb.m_nWidth = m_cExtent.m_nWidth;
            }
        }
    }
    else
    {
        if (m_cExtent.m_nHeight > m_cExtent.m_nWidth)
        {
            cThumb.m_nWidth = m_cExtent.m_nWidth;
            if (cThumb.m_nHeight >= m_cExtent.m_nHeight)
            {
                cThumb.m_nHeight = m_cExtent.m_nHeight;
            }
        }
        else
        {
            cThumb.m_nHeight = m_cExtent.m_nHeight;
            if (cThumb.m_nWidth >= m_cExtent.m_nWidth)
            {
                cThumb.m_nWidth = m_cExtent.m_nWidth;
            }
        }
        cThumb.m_nLeft = m_cExtent.m_nLeft;
        cThumb.m_nTop = m_cExtent.m_nTop;
    }
    m_cImage.SetExtent(cThumb);
}
// PC 0x00419060
int CSWGuiSlider::HitCheckSlider(int nX, int nY)
{
    CSWGuiExtent cThumb = m_cImage.m_cExtent;
    CSWGuiExtent cTrack;
    cTrack = m_cTrack.GetInnerExtent();

    if (m_cExtent.m_nHeight > m_cExtent.m_nWidth)
    {
        if (nY >= cThumb.m_nTop &&
            nY <= cThumb.m_nTop + cThumb.m_nHeight &&
            nX >= cThumb.m_nLeft &&
            nX <= cThumb.m_nLeft + cThumb.m_nWidth)
        {
            return SLIDER_THUMB;
        }
        if (nY > cTrack.m_nTop && nY < cThumb.m_nTop &&
            nX > cTrack.m_nLeft &&
            nX < cTrack.m_nLeft + cTrack.m_nWidth)
        {
            return SLIDER_BEFORE_THUMB;
        }
        if (nY > cThumb.m_nTop + cThumb.m_nHeight &&
            nY < cTrack.m_nTop + cTrack.m_nHeight &&
            nX > cTrack.m_nLeft &&
            nX < cTrack.m_nLeft + cTrack.m_nWidth)
        {
            return SLIDER_AFTER_THUMB;
        }
        return SLIDER_NONE;
    }
    else
    {
        if (nX >= cThumb.m_nLeft &&
            nX <= cThumb.m_nLeft + cThumb.m_nWidth &&
            nY >= cThumb.m_nTop &&
            nY <= cThumb.m_nTop + cThumb.m_nHeight)
        {
            return SLIDER_THUMB;
        }
        if (nX > cTrack.m_nLeft && nX < cThumb.m_nLeft &&
            nY > cTrack.m_nTop &&
            nY < cTrack.m_nTop + cTrack.m_nHeight)
        {
            return SLIDER_BEFORE_THUMB;
        }
        if (nX > cThumb.m_nLeft + cThumb.m_nWidth &&
            nX < cTrack.m_nLeft + cTrack.m_nWidth &&
            nY > cTrack.m_nTop &&
            nY < cTrack.m_nTop + cTrack.m_nHeight)
        {
            return SLIDER_AFTER_THUMB;
        }
        return SLIDER_NONE;
    }
}
// PC 0x004191D0
void CSWGuiSlider::HandleLMouseDown()
{
    int nMouseX;
    int nMouseY;

#ifndef _XBOX
    ((CSWGuiPanel*)m_pOwner)->GetLocalMouseCoords(&nMouseX, &nMouseY);
#endif
#ifndef _XBOX
    m_nDragPart = HitCheckSlider(nMouseX, nMouseY);
    switch (m_nDragPart)
    {
    case 2:
        HandleInputEvent(501, 1);
        break;
    case 3:
        HandleInputEvent(500, 1);
        break;
    }
#endif
#ifndef _XBOX
    g_pGuiMan->CaptureMouse(this, CSWGuiManager::MOUSECAPTURE_LEFT);
#endif
}
// PC 0x00419250
int CSWGuiSlider::HandleMouseCapturedMovement(int nX, int nY)
{
    int nMouseX;
    int nMouseY;
    int nValue;
    int nOffset;

#ifndef _XBOX
    if ((g_pGuiMan->m_nMouseCaptureType & CSWGuiManager::MOUSECAPTURE_LEFT) &&
        m_nDragPart == 1)
    {
        ((CSWGuiPanel*)m_pOwner)->GetLocalMouseCoords(&nMouseX, &nMouseY);
        if (m_cExtent.m_nHeight > m_cExtent.m_nWidth)
        {
            nOffset = nMouseY - m_cExtent.m_nTop;
            if (nOffset < 0)
            {
                nValue = 0;
            }
            else
            {
                nValue = m_nMaxValue;
                if (nOffset <= m_cExtent.m_nHeight)
                {
                    nValue = nOffset * m_nMaxValue / m_cExtent.m_nHeight;
                }
            }
        }
        else
        {
            nOffset = nMouseX - m_cExtent.m_nLeft;
            if (nOffset < 0)
            {
                nValue = 0;
            }
            else
            {
                nValue = m_nMaxValue;
                if (nOffset <= m_cExtent.m_nWidth)
                {
                    nValue = nOffset * m_nMaxValue / m_cExtent.m_nWidth;
                }
            }
        }
        SetCurValue(nValue);
    }
#endif
    return 1;
}
// PC 0x004192D0  Xbox 0x002FE010
void CSWGuiProgressBar::Initialize(const CSWGuiExtent& cExtent,
                                   CSWGuiProgressBar* pProto)
{
    Initialize(cExtent, pProto->m_cBorder.m_cParams, pProto->m_cFill.m_cParams);
}
#ifndef _XBOX
void CSWGuiProgressBar::SetExtent(const CSWGuiExtent& cExtent)  // PC 0x00419300  Xbox 0x002FE030
{
    CSWGuiExtent cFill;

    m_cExtent = cExtent;
    cFill.m_nHeight = 0;
    cFill.m_nWidth = 0;
    cFill.m_nTop = 0;
    cFill.m_nLeft = 0;
    m_cBorder.SetExtent(m_cExtent);
    cFill = m_cBorder.GetInnerExtent();
    if (m_nMaxValue > 0)
    {
        if (m_cExtent.m_nHeight > m_cExtent.m_nWidth)
        {
            int nHeight = (int)((double)m_nCurValue / (double)m_nMaxValue *
                                cFill.m_nHeight);
            if (!m_bStartFromLeft)
            {
                cFill.m_nTop += cFill.m_nHeight - nHeight;
            }
            cFill.m_nHeight = nHeight;
        }
        else
        {
            int nWidth = (int)((double)m_nCurValue / (double)m_nMaxValue *
                               cFill.m_nWidth);
            if (!m_bStartFromLeft)
            {
                cFill.m_nLeft += cFill.m_nWidth - nWidth;
            }
            cFill.m_nWidth = nWidth;
        }
    }
    m_cFill.SetExtent(cFill);
}
#endif
void CSWGuiScrollBar::SetExtent(const CSWGuiExtent& cExtent)  // PC 0x004193F0  Xbox 0x002FE3F0
{
    m_cExtent = cExtent;
    AurMakeCurrent();
    CSWGuiExtent cArrow = m_cArrow.GetImageExtent();
    CSWGuiExtent cBorder;
    CSWGuiExtent cThumb;

    if (m_cExtent.m_nHeight > m_cExtent.m_nWidth)
    {
        if (cArrow.m_nHeight * 2 > m_cExtent.m_nHeight)
        {
            cArrow.m_nHeight = m_cExtent.m_nHeight / 2;
        }
        if (cArrow.m_nWidth < m_cExtent.m_nWidth)
        {
            cArrow.m_nLeft = m_cExtent.m_nLeft +
                            (m_cExtent.m_nWidth - cArrow.m_nWidth) / 2;
        }
        else
        {
            cArrow.m_nLeft = m_cExtent.m_nLeft;
            cArrow.m_nWidth = m_cExtent.m_nWidth;
        }
        m_cArrow.SetExtent(cArrow);
        cBorder.m_nLeft = m_cExtent.m_nLeft;
        cBorder.m_nTop = m_cExtent.m_nTop + cArrow.m_nHeight;
        cBorder.m_nHeight = m_cExtent.m_nHeight - cArrow.m_nHeight * 2;
        cBorder.m_nWidth = m_cExtent.m_nWidth;
    }
    else
    {
        if (cArrow.m_nWidth * 2 > m_cExtent.m_nWidth)
        {
            cArrow.m_nWidth = m_cExtent.m_nWidth / 2;
        }
        if (cArrow.m_nHeight < m_cExtent.m_nHeight)
        {
            cArrow.m_nTop = m_cExtent.m_nTop +
                           (m_cExtent.m_nHeight - cArrow.m_nHeight) / 2;
        }
        else
        {
            cArrow.m_nTop = m_cExtent.m_nTop;
            cArrow.m_nHeight = m_cExtent.m_nHeight;
        }
        m_cArrow.SetExtent(cArrow);
        cBorder.m_nLeft = m_cExtent.m_nLeft + cArrow.m_nWidth;
        cBorder.m_nTop = m_cExtent.m_nTop;
        cBorder.m_nWidth = m_cExtent.m_nWidth - cArrow.m_nWidth * 2;
        cBorder.m_nHeight = m_cExtent.m_nHeight;
    }

    m_cBorder.SetExtent(cBorder);
    CSWGuiExtent cInner = m_cBorder.GetInnerExtent();
    if (m_cExtent.m_nHeight > m_cExtent.m_nWidth)
    {
        cThumb.m_nWidth = cInner.m_nWidth;
        cThumb.m_nLeft = cInner.m_nLeft;
        if (m_nMaxValue > 1)
        {
            cThumb.m_nHeight = (int)((float)m_nVisibleValue /
                                    (float)(m_nMaxValue + m_nVisibleValue) *
                                    cInner.m_nHeight);
            if (cThumb.m_nHeight <= SWGUI_SCROLLBAR_MIN_THUMB_SIZE)
            {
                cThumb.m_nHeight = SWGUI_SCROLLBAR_MIN_THUMB_SIZE;
            }
            cThumb.m_nTop = cInner.m_nTop +
                            (cInner.m_nHeight - cThumb.m_nHeight) *
                                m_nCurValue / (m_nMaxValue - 1);
        }
        else
        {
            cThumb.m_nHeight = cInner.m_nHeight;
            cThumb.m_nTop = cInner.m_nTop;
        }
    }
    else
    {
        cThumb.m_nHeight = cInner.m_nHeight;
        cThumb.m_nTop = cInner.m_nTop;
        if (m_nMaxValue)
        {
            cThumb.m_nWidth = (int)((float)m_nVisibleValue /
                                   (float)(m_nMaxValue + m_nVisibleValue) *
                                   cInner.m_nWidth);
            if (cThumb.m_nWidth <= SWGUI_SCROLLBAR_MIN_THUMB_SIZE)
            {
                cThumb.m_nWidth = SWGUI_SCROLLBAR_MIN_THUMB_SIZE;
            }
            cThumb.m_nLeft = cInner.m_nLeft +
                             (cInner.m_nWidth - cThumb.m_nWidth) *
                                 m_nCurValue / m_nMaxValue;
        }
        else
        {
            cThumb.m_nWidth = cInner.m_nWidth;
            cThumb.m_nLeft = cInner.m_nLeft;
        }
    }
    m_cThumb.SetExtent(cThumb);
}
// PC 0x00419660  Xbox 0x002FE640
void CSWGuiScrollBar::Draw(float fElapsed)
{
    int bTopArrowOn = 0;
    int bBottomArrowOn = 0;
    switch (m_bDrawMode)
    {
    case 0:
        m_cBorder.Draw(fElapsed);
        m_cThumb.Draw(fElapsed);
        bTopArrowOn = 1;
        bBottomArrowOn = 1;
        break;
    case 1:
        bTopArrowOn = GetTopArrowOn();
        bBottomArrowOn = GetBottomArrowOn();
        break;
    }

    CSWGuiExtent cArrow = m_cArrow.m_cExtent;
    CSWGuiImageParams* pParams = &m_cArrow.m_cParams;
    if (bTopArrowOn)
    {
        if (m_cExtent.m_nHeight > m_cExtent.m_nWidth)
            cArrow.m_nTop = m_cExtent.m_nTop;
        else
            cArrow.m_nLeft = m_cExtent.m_nLeft;
        pParams->m_nDrawStyle = SWGUI_IMAGE_DRAW_STYLE_NORMAL;
        m_cArrow.SetExtent(cArrow);
        m_cArrow.Draw(fElapsed);
    }
    if (bBottomArrowOn)
    {
        if (m_cExtent.m_nHeight > m_cExtent.m_nWidth)
        {
            cArrow.m_nTop = m_cExtent.GetBottom() - cArrow.m_nHeight;
            pParams->m_nDrawStyle = SWGUI_IMAGE_DRAW_STYLE_FLIP_VERTICAL;
        }
        else
        {
            cArrow.m_nLeft = m_cExtent.GetRight() - cArrow.m_nWidth;
            pParams->m_nDrawStyle = SWGUI_IMAGE_DRAW_STYLE_FLIP_HORIZONTAL;
        }
        m_cArrow.SetExtent(cArrow);
        m_cArrow.Draw(fElapsed);
    }
}
// PC 0x00419790  Xbox 0x002FE9B0
void CSWGuiScrollBar::Initialize(const CSWGuiExtent& cExtent,
                                 CSWGuiScrollBar* pProto)
{
    return Initialize(cExtent, pProto->m_cBorder.m_cParams,
                      pProto->m_cArrow.m_cParams.m_cImage,
                      pProto->m_cThumb.m_cParams.m_cImage);
}
// PC 0x004197C0
int CSWGuiScrollBar::HitCheckScrollbar(int nX, int nY)
{
    CSWGuiExtent cArrow = m_cArrow.m_cExtent;
    CSWGuiExtent cExtent = m_cExtent;
    CSWGuiExtent cThumb = m_cThumb.m_cExtent;
    CSWGuiExtent cTrack;
    cTrack = m_cBorder.GetInnerExtent();

    if (m_cExtent.m_nHeight > m_cExtent.m_nWidth)
    {
        if (nY >= cThumb.m_nTop &&
            nY <= cThumb.m_nTop + cThumb.m_nHeight &&
            nX >= cThumb.m_nLeft &&
            nX <= cThumb.m_nLeft + cThumb.m_nWidth)
        {
            return SCROLLBAR_THUMB;
        }
        if (nY > cExtent.m_nTop &&
            nY <= cExtent.m_nTop + cArrow.m_nHeight &&
            nX > cExtent.m_nLeft &&
            nX < cExtent.m_nLeft + cExtent.m_nWidth)
        {
            return SCROLLBAR_TOP_ARROW;
        }
        if (nY >= cExtent.m_nTop + cExtent.m_nHeight - cArrow.m_nHeight &&
            nY < cExtent.m_nTop + cExtent.m_nHeight &&
            nX > cExtent.m_nLeft &&
            nX < cExtent.m_nLeft + cExtent.m_nWidth)
        {
            return SCROLLBAR_BOTTOM_ARROW;
        }
        if (nY > cExtent.m_nTop + cArrow.m_nHeight &&
            nY < cThumb.m_nTop &&
            nX > cTrack.m_nLeft &&
            nX < cTrack.m_nLeft + cTrack.m_nWidth)
        {
            return SCROLLBAR_BEFORE_THUMB;
        }
        if (nY > cThumb.m_nTop + cThumb.m_nHeight &&
            nY < cExtent.m_nTop + cExtent.m_nHeight - cArrow.m_nHeight &&
            nX > cTrack.m_nLeft &&
            nX < cTrack.m_nLeft + cTrack.m_nWidth)
        {
            return SCROLLBAR_AFTER_THUMB;
        }
        return SCROLLBAR_NONE;
    }
    else
    {
        if (nX > cExtent.m_nLeft &&
            nX <= cExtent.m_nLeft + cExtent.m_nWidth + cArrow.m_nWidth &&
            nY > cExtent.m_nTop &&
            nY < cExtent.m_nTop + cExtent.m_nHeight)
        {
            return SCROLLBAR_TOP_ARROW;
        }
        if (nX >= cExtent.m_nLeft + cExtent.m_nWidth - cArrow.m_nWidth &&
            nX < cExtent.m_nLeft + cExtent.m_nWidth &&
            nY > cExtent.m_nTop &&
            nY < cExtent.m_nTop + cExtent.m_nHeight)
        {
            return SCROLLBAR_BOTTOM_ARROW;
        }
        if (nX >= cThumb.m_nLeft &&
            nX <= cThumb.m_nLeft + cThumb.m_nWidth &&
            nY >= cThumb.m_nTop &&
            nY <= cThumb.m_nTop + cThumb.m_nHeight)
        {
            return SCROLLBAR_THUMB;
        }
        if (nX > cExtent.m_nLeft + cArrow.m_nWidth &&
            nX < cThumb.m_nLeft &&
            nY > cTrack.m_nTop &&
            nY < cTrack.m_nTop + cTrack.m_nHeight)
        {
            return SCROLLBAR_BEFORE_THUMB;
        }
        if (nX > cThumb.m_nLeft + cThumb.m_nWidth &&
            nX < cExtent.m_nLeft + cExtent.m_nWidth - cArrow.m_nWidth &&
            nY > cTrack.m_nTop &&
            nY < cTrack.m_nTop + cTrack.m_nHeight)
        {
            return SCROLLBAR_AFTER_THUMB;
        }
        return SCROLLBAR_NONE;
    }
}

static inline void SwguicontrolTrace1()
{
}

#ifdef _XBOX
// PC 0x0041A9D0  Xbox 0x002FF330
void CSWGuiNavigable::HandleInputEvent(int nEvent, int nState)
{
    if (nState)
    {
        CSWGuiControl* pNext = NULL;
        switch (nEvent)
        {
        case 50:
        case 62:
            pNext = m_apMoveToControl[2];
            break;
        case 49:
        case 61:
            pNext = m_apMoveToControl[0];
            break;
        case 47:
        case 63:
            pNext = m_apMoveToControl[1];
            break;
        case 48:
        case 64:
            pNext = m_apMoveToControl[3];
            break;
        }
        if (pNext != NULL && m_pOwner != NULL)
        {
            m_pOwner->SetActiveControl(pNext, 1);
        }
    }
    return CSWGuiControl::HandleInputEvent(nEvent, nState);
}
void CSWGuiProgressBar::SetExtent(const CSWGuiExtent& cExtent)  // PC 0x00419300  Xbox 0x002FE030
{
    CSWGuiExtent cFill;

    m_cExtent = cExtent;
    cFill.m_nHeight = 0;
    cFill.m_nWidth = 0;
    cFill.m_nTop = 0;
    cFill.m_nLeft = 0;
    m_cBorder.SetExtent(m_cExtent);
    cFill = m_cBorder.GetInnerExtent();
    if (m_nMaxValue > 0)
    {
        if (m_cExtent.m_nHeight > m_cExtent.m_nWidth)
        {
            int nHeight = (int)((float)m_nCurValue / (float)m_nMaxValue * cFill.m_nHeight);
            if (!m_bStartFromLeft)
            {
                cFill.m_nTop += cFill.m_nHeight - nHeight;
            }
            cFill.m_nHeight = nHeight;
        }
        else
        {
            int nWidth = (int)((float)m_nCurValue / (float)m_nMaxValue * cFill.m_nWidth);
            if (!m_bStartFromLeft)
            {
                cFill.m_nLeft += cFill.m_nWidth - nWidth;
            }
            cFill.m_nWidth = nWidth;
        }
    }
    m_cFill.SetExtent(cFill);
}
// PC 0x0041C040  Xbox 0x00300090
void CSWGuiListBox::SetSelectedControl(int nIndex, int bPlaySound)
{
    if (m_nSelectedItem != -1)
    {
        m_lItems.element[m_nSelectedItem]->HandleInputEvent(1, 1);
    }
    if (nIndex >= 0 && nIndex < m_lItems.num)
    {
        m_nSelectedItem = nIndex;
        if (!m_bHideScrollIcon && !m_bScrollOnly)
        {
            m_cScrollBar.SetCurValue(m_nSelectedItem + 1);
        }
        m_lItems.element[m_nSelectedItem]->HandleInputEvent(GUIEVENT_MOUSE_ENTER, 1);
        if (bPlaySound)
        {
            ((CSWGuiPanel*)m_pOwner)
                ->m_pManager->PlayGuiSound(
                    m_lItems.element[m_nSelectedItem]->m_nFocusSound);
        }
    }
    else
    {
        m_nSelectedItem = -1;
    }
    OrganizeControls();
}
// PC 0x00419EE0  Xbox 0x002FEC50
void CSWGuiListBox::OrganizeUnequal()
{
    int nItems = m_lItems.num;
    CExoString sUnused;
    int nIndex;
    int nTop;
    int i;

    if (m_bOrganizePending)
    {
        if (nItems == 1)
        {
            m_nContentHeight = m_lItems[0]->GetIdealHeight();
        }
        else
        {
            m_nContentHeight = 0;
            for (i = 0; i < nItems - 1; i++)
            {
                int nHeight = m_lItems[i]->GetIdealHeight();
                if (nHeight > m_nContentHeight)
                {
                    m_nContentHeight = nHeight;
                }
            }
        }
        m_nTopVisibleItem = 0;
        m_nBottomVisibleItem = -1;
        m_bTopAnchored = 1;
        if (m_nContentHeight == 0)
        {
            return;
        }
        m_cScrollBar.SetMaxValue(nItems, 0);
        m_bOrganizePending = 0;
    }
    if (m_nSelectedItem != -1)
    {
        if (m_nTopVisibleItem != -1 && m_nSelectedItem < m_nTopVisibleItem)
        {
            m_nTopVisibleItem = m_nSelectedItem;
            m_nBottomVisibleItem = -1;
            m_bTopAnchored = 1;
        }
        else if (m_nBottomVisibleItem != -1 &&
                 m_nSelectedItem > m_nBottomVisibleItem)
        {
            m_nTopVisibleItem = -1;
            m_nBottomVisibleItem = m_nSelectedItem;
            m_bTopAnchored = 0;
        }
    }
    int nPadding = m_nPadding;
    int nInner = m_cItemExtent.m_nWidth - 2 * nPadding;
    CSWGuiExtent cExtent;
    cExtent.m_nLeft = nPadding;
    cExtent.m_nWidth = nInner;
    cExtent.m_nTop = 0;
    cExtent.m_nHeight = 0;
    if (m_bTopAnchored)
    {
        nIndex = m_nTopVisibleItem;
        nTop = m_nPadding;
    }
    else
    {
        nIndex = m_nBottomVisibleItem;
        nTop = m_cItemExtent.m_nHeight -
               m_lItems[nIndex]->m_cExtent.m_nHeight - m_nPadding;
    }
    cExtent.m_nTop = nTop;
    cExtent.m_nHeight = m_lItems[nIndex]->m_cExtent.m_nHeight;
    m_lItems[nIndex]->SetExtent(cExtent);
    for (i = nIndex - 1; i >= 0; i--)
    {
        int nHeight = m_lItems[i]->m_cExtent.m_nHeight;
        cExtent.m_nHeight = nHeight;
        nTop -= nHeight + m_nPadding;
        cExtent.m_nTop = nTop;
        m_lItems[i]->SetExtent(cExtent);
    }
    CSWGuiExtent* pLast = &m_lItems[nIndex]->m_cExtent;
    nTop = pLast->m_nHeight + pLast->m_nTop + m_nPadding;
    for (i = nIndex + 1; i < nItems; i++)
    {
        int nHeight = m_lItems[i]->m_cExtent.m_nHeight;
        cExtent.m_nHeight = nHeight;
        cExtent.m_nTop = nTop;
        nTop += nHeight + m_nPadding;
        m_lItems[i]->SetExtent(cExtent);
    }
    if (m_bTopAnchored)
    {
        int nNext = m_nTopVisibleItem + 1;
        if (nNext < nItems)
        {
            CSWGuiControl** ppItem = &m_lItems[nNext];
            int nViewHeight = m_cItemExtent.m_nHeight;
            for (; nNext < nItems; nNext++, ppItem++)
            {
                CSWGuiExtent* pExtent = &(*ppItem)->m_cExtent;
                if (pExtent->m_nHeight + pExtent->m_nTop > nViewHeight)
                {
                    m_nBottomVisibleItem = nNext - 1;
                    break;
                }
            }
        }
        if (m_nBottomVisibleItem == -1)
        {
            m_nBottomVisibleItem = nItems - 1;
        }
    }
    else
    {
        int nPrev = m_nBottomVisibleItem - 1;
        if (nPrev >= 0)
        {
            CSWGuiControl** ppItem = &m_lItems[nPrev];
            for (; nPrev >= 0; nPrev--, ppItem--)
            {
                if ((*ppItem)->m_cExtent.m_nTop < 0)
                {
                    m_nTopVisibleItem = nPrev + 1;
                    break;
                }
            }
        }
        if (m_nTopVisibleItem == -1)
        {
            m_nTopVisibleItem = 0;
        }
    }
    m_cScrollBar.SetVisibleValue(m_nBottomVisibleItem - m_nTopVisibleItem + 1);
    m_cScrollBar.m_nXboxUnmapped48 = m_nTopVisibleItem;
    m_cScrollBar.SetCurValue(m_nSelectedItem + 1);
}
// PC 0x0041B140  Xbox 0x002FF8B0
void CSWGuiListBox::OrganizeControls()
{
    int nItems = m_lItems.num;
    if (nItems == 0)
    {
        if (m_bOrganizePending)
        {
            m_cScrollBar.SetMaxValue(1, 0);
            m_cScrollBar.SetCurValue(1);
            m_cScrollBar.SetVisibleValue(1);
            m_cScrollBar.m_nXboxUnmapped48 = 1;
            m_bOrganizePending = 0;
        }
        return;
    }
    if (m_bHideScrollIcon)
        return OrganizeUnequal();

    int nStride = m_nContentHeight + m_nPadding;
    if (m_bOrganizePending)
    {
        m_nContentHeight = 0;
        for (int i = 0; i < nItems; i++)
        {
            m_lItems.element[i]->SetExtent(m_lItemExtents.element[i]);
            int nHeight = m_lItems.element[i]->GetIdealHeight();
            if (nHeight > m_nContentHeight)
                m_nContentHeight = nHeight;
        }
        m_nTopVisibleItem = 0;
        if (m_nContentHeight == 0)
        {
            m_cScrollBar.SetMaxValue(1, 0);
            m_cScrollBar.SetCurValue(1);
            m_cScrollBar.SetVisibleValue(1);
            m_cScrollBar.m_nXboxUnmapped48 = 1;
            return;
        }
        nStride = m_nContentHeight + m_nPadding;
        m_nVisibleItems = (m_cItemExtent.m_nHeight - m_nPadding) / nStride;
        if (m_nVisibleItems == 0)
        {
            EXOXBOXASSERT(IsOversized());
            m_cScrollBar.SetMaxValue(m_nContentHeight, m_bScrollOnly);
            m_cScrollBar.SetVisibleValue(m_cItemExtent.m_nHeight);
        }
        else if (m_bScrollOnly)
        {
            m_cScrollBar.SetMaxValue(max(nItems - m_nVisibleItems + 1, 1), m_bScrollOnly);
            m_cScrollBar.SetVisibleValue(1);
        }
        else
        {
            m_cScrollBar.SetMaxValue(nItems, m_bScrollOnly);
            m_cScrollBar.SetVisibleValue(min(m_nVisibleItems, nItems));
        }
        if (IsOversized())
        {
            CSWGuiLabel* pLabel = m_lItems.element[0]->AsLabel();
            if (pLabel)
            {
                int nFontHeight = pLabel->m_cText.GetFontHeight();
                if (nFontHeight)
                    m_fScrollStep = (float)m_cItemExtent.m_nHeight / nFontHeight;
            }
        }
        m_bOrganizePending = 0;
    }
    if (IsOversized())
        return OrganizeOversized();

    if (m_nSelectedItem != -1 &&
        (m_nSelectedItem < m_nTopVisibleItem ||
         m_nSelectedItem >= m_nTopVisibleItem + m_nVisibleItems))
    {
        m_nTopVisibleItem = m_nSelectedItem < m_nTopVisibleItem ? m_nSelectedItem :
                           (short)max(0, m_nSelectedItem - m_nVisibleItems + 1);
    }
    m_cScrollBar.m_nXboxUnmapped48 = m_nTopVisibleItem;
    if (m_bScrollOnly)
    {
        m_cScrollBar.SetCurValue(m_nTopVisibleItem);
    }
    int nExtra = m_cItemExtent.m_nHeight - nStride * m_nVisibleItems - m_nPadding;
    int nExtraHeight = nExtra / m_nVisibleItems;
    nExtra -= m_nVisibleItems * nExtraHeight;
    CSWGuiExtent cExtent;
    cExtent.m_nLeft = m_nPadding;
    cExtent.m_nWidth = m_cItemExtent.m_nWidth - 2 * m_nPadding;
    cExtent.m_nHeight = m_nContentHeight;
    int nTop = m_nPadding - nStride * m_nTopVisibleItem;
    for (int i = 0; i < m_nTopVisibleItem; i++, nTop += nStride)
    {
        cExtent.m_nTop = nTop;
        m_lItems.element[i]->SetExtent(cExtent);
    }
    int nEnd = min(m_nTopVisibleItem + m_nVisibleItems, nItems);
    int nHeight = nExtraHeight + m_nContentHeight;
    for (int i = m_nTopVisibleItem; i < nEnd; i++)
    {
        cExtent.m_nHeight = nHeight;
        cExtent.m_nTop = nTop;
        if (nExtra)
        {
            cExtent.m_nHeight = nHeight + 1;
            nTop++;
            nExtra--;
        }
        m_lItems.element[i]->SetExtent(cExtent);
        nTop += nHeight + m_nPadding;
    }
    cExtent.m_nHeight = m_nContentHeight;
    for (int i = m_nTopVisibleItem + m_nVisibleItems; i < nItems; i++, nTop += nStride)
    {
        cExtent.m_nTop = nTop;
        m_lItems.element[i]->SetExtent(cExtent);
    }
}
// PC 0x0041CE20  Xbox 0x00300540
void CSWGuiListBox::HandleInputEvent(int nEvent, int nState)
{
    if (nState)
    {
        int bChanged = 0;
        switch (nEvent)
        {
        case GUIEVENT_DOWN:
        case GUIEVENT_DOWN_ALTERNATE:
            if (m_nSelectedItem == -1)
            {
                if (m_bScrollOnly)
                {
                    bChanged = m_nTopVisibleItem != max(m_lItems.num - m_nVisibleItems, 0);
                    m_nTopVisibleItem = min(m_nTopVisibleItem + 1, max(m_lItems.num - m_nVisibleItems, 0));
                    OrganizeControls();
                }
                else if (m_lItems.num != 0)
                {
                    SetSelectedControl(0, 1);
                    bChanged = 1;
                }
            }
            else if (IsOversized())
            {
                bChanged = m_nScrollPosition != GetScrollRange();
                m_nScrollPosition = min(GetScrollRange(), m_nScrollPosition + 1);
                OrganizeOversized();
            }
            else if (m_nSelectedItem < m_lItems.num - 1)
            {
                SetSelectedControl(m_nSelectedItem + 1, 1);
                bChanged = 1;
            }
            else if (m_bLooping)
            {
                SetSelectedControl(0, 1);
                if (m_pOwner && m_pOwner->AsSWGuiPanel())
                    g_pExoInput->ClearEventRepeat(
                        m_pOwner->AsSWGuiPanel()->m_pManager->m_nLastInputEvent);
                bChanged = 1;
            }
            break;

        case GUIEVENT_UP:
        case GUIEVENT_UP_ALTERNATE:
            if (m_nSelectedItem == -1)
            {
                if (m_bScrollOnly)
                {
                    bChanged = m_nTopVisibleItem != 0;
                    m_nTopVisibleItem = max(m_nTopVisibleItem - 1, 0);
                    OrganizeControls();
                }
                else if (m_lItems.num != 0)
                {
                    SetSelectedControl(0, 0);
                    bChanged = 1;
                }
            }
            else if (IsOversized())
            {
                bChanged = m_nScrollPosition != 1;
                m_nScrollPosition = max(1, m_nScrollPosition - 1);
                OrganizeOversized();
            }
            else if (m_nSelectedItem > 0)
            {
                SetSelectedControl(m_nSelectedItem - 1, 1);
                bChanged = 1;
            }
            else if (m_bLooping)
            {
                SetSelectedControl(m_lItems.num - 1, 1);
                if (m_pOwner && m_pOwner->AsSWGuiPanel())
                    g_pExoInput->ClearEventRepeat(
                        m_pOwner->AsSWGuiPanel()->m_pManager->m_nLastInputEvent);
                bChanged = 1;
            }
            break;
        }

        if (bChanged && m_nSelectSound != -1 && m_pOwner &&
            m_pOwner->AsSWGuiPanel())
        {
            CSWGuiPanel* pPanel = m_pOwner->AsSWGuiPanel();
            pPanel->m_pManager->PlayGuiSound(m_nSelectSound);
        }
    }

    if (m_nSelectedItem != -1)
        m_lItems.element[m_nSelectedItem]->HandleInputEvent(nEvent, nState);
    CSWGuiControl::HandleInputEvent(nEvent, nState);
}
void CSWGuiListBox::OrganizeOversized()  // PC 0x0041A2D0  Xbox 0x002FEFB0
{
    int nRange;
    CSWGuiExtent cExtent;

    cExtent.m_nLeft = m_nPadding;
    cExtent.m_nWidth = m_cItemExtent.m_nWidth - 2 * m_nPadding;
    cExtent.m_nHeight = m_nContentHeight;
    if (m_cItemExtent.m_nHeight != 0)
    {
        nRange =
            (int)(m_nContentHeight * m_fScrollStep / m_cItemExtent.m_nHeight +
                  0.9999f) -
            int(m_fScrollStep) + 1;
    }
    else
    {
        nRange = 1;
    }
    if (m_nScrollPosition == nRange)
    {
        cExtent.m_nTop =
            m_cItemExtent.m_nHeight - m_nContentHeight - m_nPadding;
        m_cScrollBar.SetCurValue(m_nContentHeight);
        m_cScrollBar.m_nXboxUnmapped48 = m_nContentHeight;
    }
    else
    {
        cExtent.m_nTop =
            m_nPadding -
            (int)((float)(m_cItemExtent.m_nHeight * (m_nScrollPosition - 1)) /
                  m_fScrollStep);
        if (m_nScrollPosition == 1)
        {
            m_cScrollBar.SetCurValue(0);
            m_cScrollBar.m_nXboxUnmapped48 = 0;
        }
        else
        {
            int nValue = (int)((float)(m_nScrollPosition - 1) / (nRange - 1) *
                               (m_nContentHeight - m_cItemExtent.m_nHeight));
            m_cScrollBar.SetCurValue(nValue);
            m_cScrollBar.m_nXboxUnmapped48 = nValue;
        }
    }
    m_lItems[0]->SetExtent(cExtent);
}
void CSWGuiListBox::InitializeScrollBar(CSWGuiExtent& cExtent,  // PC 0x00419BD0  Xbox 0x002FEAF0
                                      const CSWGuiBorderParams& cBorder,
                                      const CResRef& sArrow,
                                      const CResRef& sThumb, int bLeft)
{
    m_bScrollBarOnLeft = bLeft;
    if (cExtent.m_nWidth > m_cExtent.m_nWidth)
    {
        cExtent.m_nWidth = m_cExtent.m_nWidth;
    }
    cExtent.m_nTop = m_cExtent.m_nTop;
    cExtent.m_nHeight = m_cExtent.m_nHeight;
    if (m_bScrollBarOnLeft)
    {
        cExtent.m_nLeft = m_cExtent.m_nLeft;
    }
    else
    {
        cExtent.m_nLeft = m_cExtent.m_nLeft + m_cExtent.m_nWidth - cExtent.m_nWidth;
    }
    m_cScrollBar.Initialize(cExtent, cBorder, sArrow, sThumb);
    m_cScrollBar.SetMaxValue(m_lItems.num, 0);
    SetExtent(m_cExtent);
}
// Xbox 0x002FD880
int CSWGuiControl::HitCheckMouse(int nX, int nY)
{
    if (nX < m_cExtent.m_nLeft || nX > m_cExtent.GetRight() || nY < m_cExtent.m_nTop || nY > m_cExtent.GetBottom())
    {
        return 0;
    }
    return 1;
}
#endif
