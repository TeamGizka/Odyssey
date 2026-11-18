#ifndef __SWGUICONTROL_H__
#define __SWGUICONTROL_H__

#ifndef SWGUI_PCVIRTUAL
#ifdef _XBOX
#define SWGUI_PCVIRTUAL
#else
#define SWGUI_PCVIRTUAL virtual
#endif
#endif

class CSWGuiObject;
class CSWGuiControl;
class CSWGuiNavigable;
class CSWGuiLabel;
class CSWGuiLabelHilight;
class CSWGuiButton;
class CSWGuiButtonToggle;
class CSWGuiListBox;
class CSWGuiSkillFlow;
class CSWGuiSlider;
class CSWGuiProgressBar;
class CSWGuiScrollBar;
class CSWGuiCheckBox;
class CSWGuiEditbox;
class CSWGuiRadioButton;
class CSWGuiSkillFlow;
class CSWGuiManager;
class CSWGuiMessageBox;
class CSWGuiTimedControl;
class CSWGuiInterfaceAction;
class CSWGuiLabel_vtbl;
class CSWGuiSaveGameEditBox_vtbl;
typedef void (CSWGuiObject::*PFNGUIEVENT)(CSWGuiControl* pControl);

#define CONTROL_LABEL         4
#define CONTROL_LABEL_HILIGHT 5
#define CONTROL_BUTTON        6
#define CONTROL_BUTTON_TOGGLE 7
#define CONTROL_SLIDER        8

typedef PFNGUIEVENT CSWGuiEventHandler;
typedef PFNGUIEVENT CSWGuiHandler;
#include "swguidrawable.h"
class CResGFF;
class CResList;
class CResRef;
class CResStruct;
class CSWGuiExtent;
class CSWGuiPanel;

class CSWGuiObject
{
public:
    CSWGuiObject()
    {
    }
    virtual ~CSWGuiObject()  // PC 0x0040A2A0 GOG 0x0040A270
    {
    }
    virtual void SetExtent(const CSWGuiExtent& cExtent)  // PC 0x0040A2B0 GOG 0x0040A280 Xbox 0x00049F00
    {
        m_cExtent = cExtent;
    }
    virtual void SetActiveControl(CSWGuiControl* pControl, int bPlaySound)  // PC 0x006049D0 GOG 0x006049B0 Xbox 0x000C4DE0
    {
    }
    SWGUI_PCVIRTUAL void HandleMouseOver(int bOver)  // PC 0x005EE1C0 GOG 0x005EE1A0
    {
    }
    SWGUI_PCVIRTUAL int HandleMouseCapturedMovement(int nX, int nY)  // PC 0x0040A2D0 GOG 0x0040A2A0
    {
        return 0;
    }
    // PC 0x004015A0 GOG 0x004015A0
    SWGUI_PCVIRTUAL void HandleLoseMouseFocus()
    {
    }
    // PC 0x004015A0 GOG 0x004015A0
    SWGUI_PCVIRTUAL void HandleLMouseDown()
    {
    }
    // PC 0x004015A0 GOG 0x004015A0
    SWGUI_PCVIRTUAL void HandleLMouseUp()
    {
    }
    // PC 0x004015A0 GOG 0x004015A0
    SWGUI_PCVIRTUAL void HandlsRMouseDown()
    {
    }
    // PC 0x004015A0 GOG 0x004015A0
    SWGUI_PCVIRTUAL void HandleRMouseUp()
    {
    }
    // PC 0x004065A0 GOG 0x00406570 Xbox 0x00316120
    virtual CSWGuiPanel* AsSWGuiPanel()
    {
        return 0;
    }
    // PC 0x004065A0 GOG 0x00406570 Xbox 0x00316120
    virtual CSWGuiListBox* AsSWGuiListBox()
    {
        return 0;
    }
    // PC 0x004065A0 GOG 0x00406570
    SWGUI_PCVIRTUAL CSWGuiControl* AsSWGuiControl()
    {
        return 0;
    }

    void SetExtent(CSWGuiExtent* pExtent);

    CSWGuiExtent m_cExtent;
#ifndef _XBOX
    int m_n14;
#endif
};

struct CSWGuiControlEvent
{
    CSWGuiObject* m_pObject;
    PFNGUIEVENT m_pHandler;
    int m_nEvent;
};

class CSWGuiControl : public CSWGuiObject
{
public:

    static const int EVENT_SELECT = 39;

    static const int EVENT_CONFIRM = 45;

    static const int EVENT_ENTER = 0;
    static const int EVENT_EXIT = 1;

    static const unsigned char GUISOUND_NONE = 0xff;

    CSWGuiControl();
#ifndef _XBOX
#ifndef SWGUICONTROL_SOURCE
    virtual ~CSWGuiControl()  // PC 0x00419640 GOG 0x00419630
    {
    }
#else
    virtual ~CSWGuiControl();
#endif
#endif
    SWGUI_PCVIRTUAL void HandleMouseOver(int bOver);
    SWGUI_PCVIRTUAL void HandleLMouseDown();
    SWGUI_PCVIRTUAL void HandleLMouseUp();
    SWGUI_PCVIRTUAL void HandleRMouseUp();
    SWGUI_PCVIRTUAL void HandleLoseMouseFocus();
    // PC 0x00698BD0 GOG 0x00698980
    SWGUI_PCVIRTUAL CSWGuiControl* AsSWGuiControl()
    {
        return this;
    }
    // PC 0x004065B0 GOG 0x00406580 Xbox 0x002CEDA0
    virtual int GetIdealHeight()
    {
        return m_cExtent.m_nHeight;
    }
    // PC 0x005EE1C0 GOG 0x005EE1A0 Xbox 0x00149D70
    virtual void Draw(float fElapsed)
    {
    }
    virtual void HandleInputEvent(int nEvent, int nState); virtual void SetActive(int bActive);
#ifdef _XBOX
    virtual int HitCheckMouse(int nX, int nY);
#else
    virtual CSWGuiControl* HitCheckMouse(int nX, int nY);
#endif
    virtual void Load(CResGFF* pGFF, CResStruct& cStruct);
    virtual CSWGuiNavigable* AsNavigable()  // PC 0x004065A0 GOG 0x00406570 Xbox 0x00316120
    {
        return 0;
    }
    // PC 0x004065A0 GOG 0x00406570 Xbox 0x00316120
    virtual CSWGuiLabel* AsLabel()
    {
        return 0;
    }
    // PC 0x004065A0 GOG 0x00406570 Xbox 0x00316120
    virtual CSWGuiLabelHilight* AsLabelHilight()
    {
        return 0;
    }
    // PC 0x004065A0 GOG 0x00406570 Xbox 0x00316120
    virtual CSWGuiButton* AsButton()
    {
        return 0;
    }
    // PC 0x004065A0 GOG 0x00406570 Xbox 0x00316120
    virtual CSWGuiButtonToggle* AsButtonToggle()
    {
        return 0;
    }
    // PC 0x004065A0 GOG 0x00406570
    SWGUI_PCVIRTUAL CSWGuiRadioButton* AsRadioButton()
    {
        return 0;
    }
    // PC 0x004065A0 GOG 0x00406570 Xbox 0x00316120
    virtual CSWGuiSlider* AsSlider()
    {
        return 0;
    }
    // PC 0x004065A0 GOG 0x00406570 Xbox 0x00316120
    virtual CSWGuiProgressBar* AsProgressBar()
    {
        return 0;
    }
    // PC 0x004065A0 GOG 0x00406570 Xbox 0x00316120
    virtual CSWGuiScrollBar* AsScrollBar()
    {
        return 0;
    }
    // PC 0x004065A0 GOG 0x00406570 Xbox 0x00316120
    virtual CSWGuiListBox* AsListBox()
    {
        return 0;
    }
    // PC 0x004065A0 GOG 0x00406570
    SWGUI_PCVIRTUAL CSWGuiSkillFlow* AsSkillFlow()
    {
        return 0;
    }
    // PC 0x004065A0 GOG 0x00406570
    SWGUI_PCVIRTUAL CSWGuiEditbox* AsEditBox()
    {
        return 0;
    }

    SWGUI_PCVIRTUAL void HandleFocusChange(int bFocus);
    SWGUI_PCVIRTUAL int GetIsSelectable();
    SWGUI_PCVIRTUAL void HandleRMouseDown();
    SWGUI_PCVIRTUAL void SetEnabled(int bEnabled);
    // PC 0x004015A0 GOG 0x004015A0
    SWGUI_PCVIRTUAL void ReSetFont()
    {
    }

    SWGUI_PCVIRTUAL int DisplayToolTip();
    // PC 0x006049D0 GOG 0x006049B0
    SWGUI_PCVIRTUAL void DrawDragDrop(int nX, int nY)
    {
    }

    void AddEvent(int nEvent, CSWGuiObject* pObject, CSWGuiHandler pHandler);
    int GetEvent(int nEvent, CSWGuiObject** ppObject, CSWGuiHandler* ppHandler);
    void Load(CSWGuiObject* pParent, CResGFF* pGFF, CResStruct& cStruct,
              const CExoString& sName);
    void Load(CSWGuiObject* pParent, CResGFF* pGFF, CResList& lControls,
              const CExoString& sName);
    CSWGuiControl* GetSelectableParent();
    void AddChildControl(CSWGuiControl* pChild);
    int GetIsChild(CSWGuiControl* pControl);

    void SetEnabledFlag(int bEnabled)
    {
        m_bEnabled = bEnabled;
    }

    void SetShown(int bShown)
    {
        m_bEnabled = bShown;
    }

    void HideToolTip();

    void SetToolTip(unsigned long nStrRef, unsigned long nInputEvent)
    {
#ifndef _XBOX
        m_nToolTipStrRef = nStrRef;
        m_nToolTipInputEvent = nInputEvent;
#endif
    }

    void SetFocusable(int bFocusable)
    {
        m_bFocusable = bFocusable;
    }

    int GetID() const
    {
        return m_nID;
    }

    int GetEvent() const
    {
        return m_nEvent;
    }

    char GetGuiSound() const
    {
        return m_nClickSound;
    }

    void* GetUserData() const
    {
        return (void*)m_nIndex;
    }

#ifndef _XBOX
    CSWGuiControl* m_pParent;
    CExoArrayList<CSWGuiControl*> m_lChildren;
    unsigned long m_nToolTipStrRef;
    CExoString m_sToolTip;
    unsigned long m_nToolTipInputEvent;
#endif
    CSWGuiObject* m_pOwner;
    CExoArrayList<CSWGuiControlEvent> m_lEvents;
    unsigned long m_bActive : 1;
    unsigned long m_bEnabled : 1;
    unsigned long m_bFocusable : 1;
#ifndef _XBOX
    unsigned long m_bHilightOnOver : 1;
#endif

#ifndef _XBOX
    unsigned long m_bControlState4 : 1;
    unsigned long m_bIgnoreMouse : 1;
    unsigned long m_bToolTipShown : 1;
#endif
    unsigned long m_nEvent;
    int m_bSelected;
    int m_nID;
    char m_nClickSound;
    char m_nFocusSound;
    int m_nIndex;
};

class CSWGui3DSceneView : public CSWGuiControl
{
public:
#ifndef _XBOX
    virtual ~CSWGui3DSceneView()  // PC 0x006D1430 GOG 0x006D0700
    {
    }
#endif
    virtual void SetExtent(const CSWGuiExtent& cExtent);
    virtual void Draw(float fElapsed)  // PC 0x006D1490 GOG 0x006D0760 Xbox 0x000A0A80
    {
        m_cScene.Draw(fElapsed);
    }

    CSWGuiScene m_cScene;
};

class CSWGuiNavigable : public CSWGuiControl
{
public:
    CSWGuiNavigable();

#ifndef _XBOX
#ifndef SWGUICONTROL_SOURCE
    virtual ~CSWGuiNavigable()  // PC 0x004196D0 GOG 0x004196C0
    {
    }
#else
    virtual ~CSWGuiNavigable();
#endif
#endif

    SWGUI_PCVIRTUAL void HandleMouseOver(int bOver);
    virtual void HandleInputEvent(int nEvent, int nState);
    virtual void Load(CResGFF* pGFF, CResStruct& cStruct);
    // PC 0x00698BD0 GOG 0x00698980 Xbox 0x0025DD00
    virtual CSWGuiNavigable* AsNavigable()
    {
        return this;
    }

    int GetMoveToControl(int nDirection);
    void SetMoveToControl(int nDirection, CSWGuiControl* pControl);

    CSWGuiControl* m_apMoveToControl[4];
#ifdef _XBOX
    int m_n4C;
#endif
};

class CSWGuiLabel : public CSWGuiControl
{
public:
#ifdef _XBOX
    CSWGuiLabel()
    {
    }
#else
    CSWGuiLabel();
#endif
#ifndef _XBOX
#ifndef SWGUICONTROL_SOURCE
    virtual ~CSWGuiLabel()  // PC 0x00419700 GOG 0x004196F0
    {
    }
#else
    virtual ~CSWGuiLabel();
#endif
#endif
    using CSWGuiObject::SetExtent;
    virtual void SetExtent(const CSWGuiExtent& cExtent);
    virtual void Draw(float fElapsed);
    virtual void Load(CResGFF* pGFF, CResStruct& cStruct);
    // PC 0x00698BD0 GOG 0x00698980 Xbox 0x0025DD00
    virtual CSWGuiLabel* AsLabel()
    {
        return this;
    }

    SWGUI_PCVIRTUAL void SetEnabled(int bEnabled);
    SWGUI_PCVIRTUAL void ReSetFont();
#ifdef _XBOX

    virtual void Initialize(const CSWGuiExtent& cExtent,
                            const CSWGuiTextParams& cText,
                            const CSWGuiBorderParams& cBorder);
    virtual void Initialize(const CSWGuiExtent& cExtent, CSWGuiLabel* pProto);
    void Initialize(const CSWGuiExtent& cExtent, const CSWGuiTextParams& cText,
                    const CSWGuiBorderParams& cBorder, float)
    {
        Initialize(cExtent, cText, cBorder);
    }
    void Initialize(const CSWGuiExtent& cExtent, CSWGuiLabel* pProto, float)
    {
        Initialize(cExtent, pProto);
    }
#else
    virtual void Initialize(const CSWGuiExtent& cExtent,
                            const CSWGuiTextParams& cText,
                            const CSWGuiBorderParams& cBorder, float fScale);
    virtual void Initialize(const CSWGuiExtent& cExtent, CSWGuiLabel* pProto,
                            float fScale);
#endif

    CSWGuiBorder m_cBorder;
    CSWGuiText m_cText;
};

class CSWGuiLabelHilight : public CSWGuiLabel
{
public:
#ifndef _XBOX
    virtual ~CSWGuiLabelHilight()  // PC 0x0041B000 GOG 0x0041AFF0
    {
    }
#endif
    using CSWGuiLabel::Initialize;
    virtual void SetExtent(const CSWGuiExtent& cExtent);
    virtual void Draw(float fElapsed);
#ifndef _XBOX
    virtual void SetActive(int bActive);
#endif
    virtual void Load(CResGFF* pGFF, CResStruct& cStruct);
    // PC 0x00698BD0 GOG 0x00698980 Xbox 0x0025DD00
    virtual CSWGuiLabelHilight* AsLabelHilight()
    {
        return this;
    }

    virtual void Initialize(const CSWGuiExtent& cExtent,
                            const CSWGuiTextParams& cText,
                            const CSWGuiBorderParams& cBorder,
                            const CSWGuiBorderParams& cHilite);
    virtual void Initialize(const CSWGuiExtent& cExtent,
                            CSWGuiLabelHilight* pProto);

    CSWGuiBorder m_cHilite;
};

class CSWGuiButton : public CSWGuiNavigable
{
public:
    using CSWGuiObject::SetExtent;

#ifndef _XBOX
#ifndef SWGUICONTROL_SOURCE
    virtual ~CSWGuiButton()  // PC 0x00419790 GOG 0x00419780
    {
    }
#else
    virtual ~CSWGuiButton();
#endif
#endif
#ifndef SWGUICONTROL_SOURCE
    // PC 0x0041BB50 GOG 0x0041BB40 Xbox 0x00097B00
    CSWGuiButton()
    {
    }
#else
    CSWGuiButton();
#endif
    virtual void SetExtent(const CSWGuiExtent& cExtent);
    virtual void Draw(float fElapsed);
    virtual void HandleInputEvent(int nEvent, int nState);
    virtual void SetActive(int bActive);
    virtual void Load(CResGFF* pGFF, CResStruct& cStruct);
    // PC 0x00698BD0 GOG 0x00698980 Xbox 0x0025DD00
    virtual CSWGuiButton* AsButton()
    {
        return this;
    }

    SWGUI_PCVIRTUAL void SetEnabled(int bEnabled);
    SWGUI_PCVIRTUAL void ReSetFont();
    virtual void Initialize(const CSWGuiExtent& cExtent,
                            const CSWGuiTextParams& cText,
                            const CSWGuiBorderParams& cBorder,
                            const CSWGuiBorderParams& cHilite);
    virtual void Initialize(const CSWGuiExtent& cExtent, CSWGuiButton* pProto);

    CSWGuiBorder m_cBorder;
    CSWGuiBorder m_cHilite;
    CSWGuiText m_cText;
};

class CSWGuiButtonToggle : public CSWGuiButton
{
public:
    CSWGuiButtonToggle();
#ifndef _XBOX
    virtual ~CSWGuiButtonToggle()  // PC 0x0041C4D0 GOG 0x0041C4C0
    {
    }
#endif
    using CSWGuiObject::SetExtent;
    virtual void SetExtent(const CSWGuiExtent& cExtent);
    virtual void Draw(float fElapsed);
    virtual void HandleInputEvent(int nEvent, int nState);
    virtual void Initialize(const CSWGuiExtent& cExtent,
                            const CSWGuiTextParams& cText,
                            const CSWGuiBorderParams& cBorder,
                            const CSWGuiBorderParams& cHilite,
                            const CSWGuiBorderParams& cToggleBorder,
                            const CSWGuiBorderParams& cToggleHilite);
    // PC 0x00698BD0 GOG 0x00698980 Xbox 0x0025DD00
    virtual CSWGuiButtonToggle* AsButtonToggle()
    {
        return this;
    }

    void Load(CResGFF* pGFF, CResStruct& cStruct);
    void SetToggled(int bToggled);
    virtual void Initialize(const CSWGuiExtent& cExtent,
                            CSWGuiButtonToggle* pProto);

    int m_nToggleEvent;
    unsigned long m_bToggled : 1;
    CSWGuiBorder m_cToggleBorder;
    CSWGuiBorder m_cToggleHilite;
};

#define SWGUI_SCROLLBAR_MIN_THUMB_SIZE 4

class CSWGuiScrollBar : public CSWGuiControl
{
public:

    enum ScrollBarPart
    {
        SCROLLBAR_NONE = 0,
        SCROLLBAR_TOP_ARROW = 1,
        SCROLLBAR_BOTTOM_ARROW = 2,
        SCROLLBAR_THUMB = 3,
        SCROLLBAR_BEFORE_THUMB = 4,
        SCROLLBAR_AFTER_THUMB = 5
    };

    CSWGuiScrollBar();
#ifndef _XBOX
#ifndef SWGUICONTROL_SOURCE
    virtual ~CSWGuiScrollBar()  // PC 0x00419CF0 GOG 0x00419CE0
    {
    }
#else
    virtual ~CSWGuiScrollBar();
#endif
#endif
    using CSWGuiObject::SetExtent;
    using CSWGuiControl::Load;
    virtual void SetExtent(const CSWGuiExtent& cExtent);
    virtual void Draw(float fElapsed);
    virtual void Load(CResGFF* pGFF, CResStruct& cStruct);
    // PC 0x00698BD0 GOG 0x00698980 Xbox 0x0025DD00
    virtual CSWGuiScrollBar* AsScrollBar()
    {
        return this;
    }

    virtual void Initialize(const CSWGuiExtent& cExtent,
                            const CSWGuiBorderParams& cBorder,
                            const CResRef& sArrow, const CResRef& sThumb);
    virtual void Initialize(const CSWGuiExtent& cExtent,
                            CSWGuiScrollBar* pProto);

    int GetTopArrowOn();
    int GetBottomArrowOn();
    int GetMaxValue();
    int GetVisibleValue();
    void SetMaxValue(int nMaxValue, bool bExtendedRange);
    void SetCurValue(int nValue);
    void SetVisibleValue(int nValue);
    int HitCheckScrollbar(int nX, int nY);

    int m_nMaxValue;
    int m_nCurValue;
    int m_nVisibleValue;
#ifdef _XBOX
    int m_nTopValue;
#endif
    unsigned long m_bDrawMode : 1;
    unsigned long m_bExtendedRange : 1;
    CSWGuiBorder m_cBorder;
    CSWGuiImage m_cThumb;
    CSWGuiImage m_cArrow;
};

class CSWGuiListBox : public CSWGuiNavigable
{
public:

    enum Event
    {
        EVENT_SELECTION_CHANGED = 504,
        EVENT_DOUBLE_CLICK = 505
    };

    void SetScrollOnly(int bScrollOnly)
    {
        m_bScrollOnly = bScrollOnly;
    }

    CSWGuiListBox();
    virtual ~CSWGuiListBox();
    using CSWGuiObject::SetExtent;
    virtual void SetExtent(const CSWGuiExtent& cExtent);
    virtual void SetActiveControl(CSWGuiControl* pControl, int bNotify);
    SWGUI_PCVIRTUAL int HandleMouseCapturedMovement(int nX, int nY);
    SWGUI_PCVIRTUAL void HandleLMouseDown();
    SWGUI_PCVIRTUAL void HandleLMouseUp();
    // PC 0x00698BD0 GOG 0x00698980 Xbox 0x0025DD00
    virtual CSWGuiListBox* AsSWGuiListBox()
    {
        return this;
    }

    virtual void Draw(float fElapsed);
    virtual void HandleInputEvent(int nEvent, int nState);
    virtual void SetActive(int bActive);
    virtual void Load(CResGFF* pGFF, CResStruct& cStruct);
    // PC 0x00698BD0 GOG 0x00698980 Xbox 0x0025DD00
    virtual CSWGuiListBox* AsListBox()
    {
        return this;
    }

    SWGUI_PCVIRTUAL int GetIsSelectable();
    SWGUI_PCVIRTUAL void ReSetFont();
    SWGUI_PCVIRTUAL int DisplayToolTip();
    virtual void Initialize(const CSWGuiExtent& cExtent,
                            const CSWGuiBorderParams& cBorder, int nPadding);
    virtual void Initialize(const CSWGuiExtent& cExtent, CSWGuiListBox* pProto);
    virtual void InitializeScrollBar(CSWGuiExtent& cExtent,
                                     const CSWGuiBorderParams& cBorder,
                                     const CResRef& sUpArrow,
                                     const CResRef& sDownArrow, int bLeft);
    SWGUI_PCVIRTUAL void SetSelectOnLeftClick(int bSelect)  // PC 0x0041A950 GOG 0x0041A940
    {
#ifndef _XBOX
        m_bSelectOnLeftClick = bSelect;
#endif
    }

    static void UpdateArrowPulsing(float fElapsed);
    void SetScrollBarExtent(int nWidth, int bUpdate);
    void SetScrollBarOnLeft(int bLeft);
    int GetNumSegments();
    int IsOversized();
    void OrganizeUnequal();
    void OrganizeOversized();

    static float m_fArrowAlpha;
    static float m_fArrowAlphaPulseTime;
    static bool m_bArrowAlphaPulseUp;

#ifndef _XBOX
    CSWGuiControl* HitCheckMouseLocal(int* pnIndex);
#endif
#ifndef _XBOX
    void HandleMouseMove();
#endif
    CSWGuiControl* GetControl(int nIndex);
    CSWGuiControl* GetSelectedControl();

#ifndef _XBOX
    int GetMouseX()
    {
        return m_nMouseX;
    }

    int GetMouseY()
    {
        return m_nMouseY;
    }
#endif

    void ClearItems();
    void AddControls(CExoArrayList<CSWGuiControl*>& aControls, int nA,
                     int nB, int nC);
    void AddControls(CSWGuiControl** pControls, int nCount, int nA, int nB,
                     int nC);

    void SetShowScrollBar(int bShow) { m_bShowScrollBar = bShow; }
    void SetSelectedControl(CSWGuiControl* pControl, int bNotify);
    void SetSelectedControl(int nIndex, int bNotify);
    int GetNumItems()
    {
        return m_lItems.num;
    }
#ifndef SWGUISTORE_SOURCE
    void SetTopVisible(short nTopItem)  // PC 0x00721090 GOG 0x00720DA0
    {
        m_nTopVisibleItem = min((int)nTopItem, max(0, GetNumItems() - m_nVisibleItems));
    }
#else
    void SetTopVisible(short nTopItem);
#endif

    int GetItemWidth()
    {
        return m_cItemExtent.m_nWidth - 2 * m_nPadding;
    }

    void SetPadding(int nPadding);
    void OrganizeControls();
    void LoadProtoItem(CResGFF* pGFF, CResStruct& cStruct);

#ifndef _XBOX
    int m_nPressedItem;
    int m_nScrollBarPart;
    int m_nDragStartX;
    int m_nDragStartY;
    int m_nDragStartPosition;
    int m_nMouseX;
    int m_nMouseY;
    int m_nHeldEvent;
    float m_fRepeatDelay;
#endif
    CSWGuiBorder m_cBorder;
    CSWGuiScrollBar m_cScrollBar;
    CSWGuiExtent m_cItemExtent;
    CExoArrayList<CSWGuiControl*> m_lItems;
    CExoArrayList<CSWGuiExtent> m_lItemExtents;
    int m_nContentHeight;
    float m_fScrollStep;
#ifndef _XBOX
    unsigned long m_bSelectOnLeftClick : 1;
#endif
    unsigned long m_bOwnsItems : 1;
    unsigned long m_bRestoreExtents : 1;
    unsigned long m_bOrganizePending : 1;
    unsigned long m_bScrollBarOnLeft : 1;
    unsigned long m_bShowScrollBar : 1;
    unsigned long m_bLooping : 1; unsigned long m_bTopAnchored : 1;
    unsigned long m_bHideScrollIcon : 1; unsigned long m_bScrollOnly : 1;
    unsigned long m_bClipItems : 1;
    unsigned long m_bPulseArrows : 1;
#ifndef _XBOX
    unsigned long m_bScrollToSelected : 1;
#endif
    unsigned long m_nCONTROLTYPE : 4;
    unsigned char m_nPadding;
    char m_nSelectSound;
    short m_nScrollPosition;
    short m_nVisibleItems;
    short m_nSelectedItem;
    short m_nTopVisibleItem;
    short m_nBottomVisibleItem;
    CSWGuiControl* m_pProtoItem;
    Vector m_vColor;
#ifndef _XBOX
    CSWGuiControl* m_pHoverControl;
#endif
};
#ifndef SWGUICONTROL_SOURCE
// PC 0x00419D90 GOG 0x00419D80 Xbox 0x00049F50
inline int CSWGuiListBox::IsOversized()
{
    return m_nContentHeight + m_nPadding > m_cItemExtent.m_nHeight &&
           m_lItems.num > 0 && !m_bHideScrollIcon;
}
#endif

class CSWGuiSlider : public CSWGuiNavigable
{
public:

    enum SliderPart
    {
        SLIDER_NONE = 0,
        SLIDER_THUMB = 1,
        SLIDER_BEFORE_THUMB = 2,
        SLIDER_AFTER_THUMB = 3
    };

    CSWGuiSlider();
#ifndef _XBOX
    virtual ~CSWGuiSlider()  // PC 0x0041B530 GOG 0x0041B520
    {
    }
#endif
    using CSWGuiObject::SetExtent;
    virtual void SetExtent(const CSWGuiExtent& cExtent);
    SWGUI_PCVIRTUAL int HandleMouseCapturedMovement(int nX, int nY);
    SWGUI_PCVIRTUAL void HandleLMouseDown();
    // PC 0x00698BD0 GOG 0x00698980 Xbox 0x0025DD00
    virtual CSWGuiSlider* AsSlider()
    {
        return this;
    }

    virtual void Draw(float fElapsed);
    virtual void HandleInputEvent(int nEvent, int nState);
    virtual void Load(CResGFF* pGFF, CResStruct& cStruct);
    virtual void Initialize(const CSWGuiExtent& cExtent,
                            const CSWGuiBorderParams& cTrack,
                            const CSWGuiBorderParams& cThumb,
                            const CResRef& cImage);
    virtual void Initialize(const CSWGuiExtent& cExtent, CSWGuiSlider* pProto);

    void SetMaxValue(int nMaxValue);
    void SetCurValue(int nValue);
    int GetMaxValue() { return m_nMaxValue; }
    int HitCheckSlider(int nX, int nY);

#ifndef _XBOX
    int m_nDragPart;
#endif
    int m_nMaxValue;
    int m_nCurValue;
    char m_nSlideSound;
    CSWGuiBorder m_cTrack;
    CSWGuiBorder m_cThumb;
    CSWGuiImage m_cImage;
};

class CSWGuiProgressBar : public CSWGuiControl
{
public:
    CSWGuiProgressBar();
#ifndef _XBOX
    virtual ~CSWGuiProgressBar()  // PC 0x0041A7A0 GOG 0x0041A790
    {
    }
#endif
    using CSWGuiObject::SetExtent;
    virtual void SetExtent(const CSWGuiExtent& cExtent);
    virtual void Draw(float fElapsed);
    virtual void Load(CResGFF* pGFF, CResStruct& cStruct);
    // PC 0x00698BD0 GOG 0x00698980 Xbox 0x0025DD00
    virtual CSWGuiProgressBar* AsProgressBar()
    {
        return this;
    }

    virtual void Initialize(const CSWGuiExtent& cExtent,
                            const CSWGuiBorderParams& cBorder,
                            const CSWGuiBorderParams& cFill);
    virtual void Initialize(const CSWGuiExtent& cExtent,
                            CSWGuiProgressBar* pProto);

    void SetMaxValue(int nMaxValue);
    void SetCurValue(int nValue);
    void SetStartFromLeft(int bDrawMode);

    int m_nMaxValue;
    int m_nCurValue;

    unsigned long m_bStartFromLeft : 1;
    CSWGuiBorder m_cBorder;
    CSWGuiBorder m_cFill;
};

#ifndef _XBOX
class CSWGuiProgressBarReveal : public CSWGuiProgressBar
{
public:
    virtual ~CSWGuiProgressBarReveal()  // PC 0x00676920 GOG 0x006766C0
    {
    }
    virtual void SetExtent(const CSWGuiExtent& cExtent);
    virtual void Draw(float fElapsed);

    CSWGuiExtent m_cStartExtent;
};
#endif

class CSWGuiEditbox : public CSWGuiNavigable
{
public:
    virtual void Initialize(const CSWGuiExtent& cExtent,
                            const CSWGuiTextParams& cText,
                            const CSWGuiBorderParams& cBorder);
    virtual void HandleKeyPress(int nKey);
    virtual void Draw(float);
    virtual void SetExtent(CSWGuiExtent const&);
    SWGUI_PCVIRTUAL void SetEnabled(int);
    void SetFocus();
    SWGUI_PCVIRTUAL int GetIsSelectable();
    SWGUI_PCVIRTUAL void HandleLMouseDown();
    SWGUI_PCVIRTUAL void HandleLMouseUp();
    SWGUI_PCVIRTUAL void ReSetFont();
    SWGUI_PCVIRTUAL void HandleFocusChange(int);
    CSWGuiEditbox();
#ifndef _XBOX
    virtual ~CSWGuiEditbox()  // PC 0x0041C140 GOG 0x0041C130
    {
    }
#endif
    virtual void Load(CResGFF*, CResStruct&);
    // PC 0x00698BD0 GOG 0x00698980
    SWGUI_PCVIRTUAL CSWGuiEditbox* AsEditBox()
    {
        return this;
    }

    CSWGuiBorder m_cBorder;
    CSWGuiEditText m_cText;
};

class CSWGuiRadioButton : public CSWGuiButton
{
public:
#ifndef _XBOX
    virtual ~CSWGuiRadioButton()
    {
    }
#endif

    SWGUI_PCVIRTUAL void HandleMouseOver(int bOver);
    SWGUI_PCVIRTUAL void HandleLMouseDown();
    SWGUI_PCVIRTUAL void HandleLMouseUp();
    // PC 0x00698BD0 GOG 0x00698980
    SWGUI_PCVIRTUAL CSWGuiRadioButton* AsRadioButton()
    {
        return this;
    }
};

#endif
