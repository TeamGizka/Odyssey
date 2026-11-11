#ifndef __SWGUI_H__
#define __SWGUI_H__

#ifndef SWGUI_PCVIRTUAL
#ifdef _XBOX
#define SWGUI_PCVIRTUAL
#else
#define SWGUI_PCVIRTUAL virtual
#endif
#endif

#define SWGUI_PANEL_MODE_OVERLAY 4

#define SWGUI_PANEL_FLAG_PUSH_MODAL 1
#define SWGUI_PANEL_FLAG_MODAL 2
#define SWGUI_PANEL_FLAG_OVERLAY 4

#define SWGUI_SOUND_REJECT 2

#define SWGUI_SOUND_EQUIP_ITEM 10
#define SWGUI_SOUND_UNEQUIP_ITEM 11

#define GUIEVENT_DELETE 41
class CSWGuiPanel;
class CSWGuiManager;
class CSWGuiMessageBox;
struct CSWGuiTimedControl;
class CSWGuiInterfaceAction;
class CSWCCreature;
class CSWCObject;
typedef void (CSWCObject::*PFNMENUACTION)(unsigned long, CSWCCreature*);
#include "swguicontrol.h"
class CAurObject;
class CClientExoAppInternal;
class CRes;
class CResRef;
class CSWGuiControl;
class CSWGuiExtent;
class CSWGuiObject;
class CSWGuiScene;
class CSWGuiToolTipPanel;
class CExoSoundSource;
#include "resgff.h"

enum SWGuiInputEvent
{

    GUIEVENT_MOUSE_ENTER = 0,
    GUIEVENT_ACTIVATE = 39,
    GUIEVENT_ACCEPT = 45,
    GUIEVENT_CANCEL = 40,

    GUIEVENT_TOGGLE_MODE = 41,

    GUIEVENT_CONFIRM_SELECTION = 41,
    GUIEVENT_RECOMMEND = 42,

    GUIEVENT_PANEL_CLICK = 45,
    GUIEVENT_CANCEL_ALTERNATE = 46,
    GUIEVENT_LEFT = 47,
    GUIEVENT_RIGHT = 48,

    GUIEVENT_UP = 49,
    GUIEVENT_DOWN = 50,
    GUIEVENT_UP_ALTERNATE = 61,
    GUIEVENT_DOWN_ALTERNATE = 62,

    GUIEVENT_LEFT_ALTERNATE = 63,
    GUIEVENT_RIGHT_ALTERNATE = 64,

    GUIEVENT_RIGHT_CLICK = 68,

    GUIEVENT_PREVIOUS = 53,
    GUIEVENT_NEXT = 54,

    GUIEVENT_SCROLL_UP = 57,
    GUIEVENT_SCROLL_DOWN = 58,

    GUIEVENT_NEXT_CHARACTER = 206,

    GUIEVENT_MENU_EQUIPMENT = 209,
    GUIEVENT_MENU_INVENTORY = 210,
    GUIEVENT_MENU_CHARACTER = 211,
    GUIEVENT_MENU_ABILITIES = 212,
    GUIEVENT_MENU_MESSAGES = 213,
    GUIEVENT_MENU_JOURNAL = 214,
    GUIEVENT_MENU_MAP = 215,
    GUIEVENT_MENU_OPTIONS = 216,
    GUIEVENT_PAUSE = 241,
    GUIEVENT_SOLO = 207,
    GUIEVENT_STEALTH = 264,
    GUIEVENT_CLEAR_ONE = 245,
    GUIEVENT_CLEAR_ALL = 240,
    GUIEVENT_EXIT = 223,

    GUIEVENT_SLIDER_INCREMENT = 500,
    GUIEVENT_SLIDER_DECREMENT = 501,

    GUIEVENT_SCROLLBAR_UP = 507,
    GUIEVENT_SCROLLBAR_DOWN = 508,
    GUIEVENT_SCROLLBAR_PAGE_UP = 509,
    GUIEVENT_SCROLLBAR_PAGE_DOWN = 510
};

#define GUIEVENT_ACTIVATE_LEFT_ACTION 226
#define GUIEVENT_ACTIVATE_MIDDLE_ACTION 228
#define GUIEVENT_ACTIVATE_RIGHT_ACTION 230
#define GUIEVENT_ACTIVATE_FRIENDLY_POWER 232
#define GUIEVENT_ACTIVATE_MEDICAL_ITEM 234
#define GUIEVENT_ACTIVATE_MINE 236
#define GUIEVENT_ACTIVATE_NON_MEDICAL_ITEM 238
#define GUIEVENT_ACTION_MENU_UP 248
#define GUIEVENT_ACTION_MENU_DOWN 249

#define GUIEVENT_MOUSE_WHEEL_UP 500
#define GUIEVENT_MOUSE_WHEEL_DOWN 501

class CSWGuiPanel : public CSWGuiObject
{
public:

    enum { EXITSTATE_REMOVE = 1, EXITSTATE_DELETE = 2 };

    CSWGuiPanel(CSWGuiManager* pManager);
    virtual ~CSWGuiPanel();

    virtual void SetExtent(const CSWGuiExtent& cExtent);
    virtual void SetActiveControl(CSWGuiControl* pControl, int bPlaySound);
    // PC 0x00698BD0
    virtual CSWGuiPanel* AsSWGuiPanel()
    {
        return this;
    }

    virtual void Draw(float fElapsed);
    // PC 0x005EE1C0
    virtual void Update(float fElapsed)
    {
    }

    virtual void HandleInputEvent(int nEvent, int nState);
    virtual CSWGuiControl* HitCheckMouse(int nX, int nY);
    SWGUI_PCVIRTUAL CSWGuiControl* GetActiveControl()
    {
        return m_pActiveControl;
    }
    virtual void OnPanelAdded();
    virtual void OnPanelRemoved();
    SWGUI_PCVIRTUAL void OnAButtonPressed(CSWGuiControl* pControl)  // PC 0x0040B890
    {
        return HandleInputEvent(GUIEVENT_ACTIVATE, 1);
    }
    SWGUI_PCVIRTUAL void OnBButtonPressed(CSWGuiControl* pControl)  // PC 0x0040B8A0
    {
        return HandleInputEvent(GUIEVENT_CANCEL, 1);
    }
    SWGUI_PCVIRTUAL void OnXButtonPressed(CSWGuiControl* pControl)  // PC 0x0040B8B0
    {
        return HandleInputEvent(41, 1);
    }
    SWGUI_PCVIRTUAL void OnYButtonPressed(CSWGuiControl* pControl)  // PC 0x0040B8C0
    {
        return HandleInputEvent(42, 1);
    }
    SWGUI_PCVIRTUAL void OnBlackButtonPressed(CSWGuiControl* pControl)  // PC 0x0040B8D0
    {
        return HandleInputEvent(43, 1);
    }
    SWGUI_PCVIRTUAL CExoString GetFullScreenBG();
    SWGUI_PCVIRTUAL void ResetFont();

    void SetVisible(int bVisible);

    CSWGuiManager* GetManager()
    {
        return m_pManager;
    }
    void SetAlpha(float fAlpha)  // PC 0x00404B70
    {
        m_fAlpha = max(min(fAlpha, 1.0f), 0.0f);
    }
    void SetBackground(const CResRef& cBackground);
    void CenterPanel();
    void ConnectControlMoves();
    void GetExtentAccountingForPanelOffset(CSWGuiExtent* pExtent);
#ifndef _XBOX
    void GetLocalMouseCoords(int* pnX, int* pnY);
#endif
    void AddControl(CSWGuiControl* pControl);
    CSWGuiControl* GetControl(int nIndex);
    void SetModal(int bModal)  // PC 0x00639690
    {
        m_bDeferredRemoval = bModal;
        if (m_nExitState != EXITSTATE_DELETE)
        {
            m_nExitState = EXITSTATE_REMOVE;
        }
    }
    void StartLoadFromLayout(const CResRef& cLayout);
    void StopLoadFromLayout();
    void InitControl(CSWGuiControl& cControl, const CExoString& sName,
                     int bUnknown);

#ifndef _XBOX
    int m_bUseFont;
#endif
    CSWGuiManager* m_pManager;
    CSWGuiControl* m_pActiveControl;
    CExoArrayList<CSWGuiControl*> m_lControls;
    CResGFF* m_pLayout;
    CResList m_lLayoutControls;
#ifndef _XBOX
    unsigned long m_bPositioned : 1;
#endif
    unsigned long m_bLoadingLayout : 1;
    unsigned long m_bVisible : 1;
    unsigned long m_bModal : 1;
#ifndef _XBOX
    unsigned long m_bOverlay : 1;
#endif
    unsigned long m_bCenterHorizontal : 1;
    unsigned long m_bCenterVertical : 1;
    unsigned long m_bNeedsDraw : 1;
    unsigned long m_bDeferredRemoval : 1;
    unsigned long m_nExitState : 2;
    unsigned long : 21;
#ifndef _XBOX
    unsigned char m_nBackground;
#endif
    float m_fAlpha;
    Vector m_vColor;
    CSWGuiBorder* m_pBorder;
    char m_nOpenSound;
};

class CSWGuiDynamic : public CSWGuiPanel
{
public:
    CSWGuiDynamic(CSWGuiManager* pManager, const CResRef& cLayout);
    virtual ~CSWGuiDynamic();
};

class CSWGuiManager
{
public:

    void ClearPanels()
    {
        m_lPanels = CExoArrayList<CSWGuiPanel*>();
    }
    static CExoString FULLSCREEN_BGS[5];

    enum { SAFE_MARGIN_X = 48, SAFE_MARGIN_Y = 36 };

    enum { CURSOR_DEFAULT = 1, CURSOR_UNSELECTED_TARGET = 45 };

    static const char SOUND_ACTION_CHANGE = 7;
    static const unsigned long DIRECTION_CHANGE_DELAY = 150;
    enum
    {
        MOUSECAPTURE_LEFT = 1,
        GUISOUND_CLICK = 0,
        GUISOUND_SELL_ITEM = 11,
        GUISOUND_BUY_ITEM = 9,
        GUISOUND_SELECTION = 1,

        GUISOUND_ACTION_DISABLED = 2,
        GUISOUND_WORLD_ACTION = 6,
        GUISOUND_PAZAAK_END = 5,

        GUISOUND_PARTY_ACCEPT = 5,
        GUISOUND_PAZAAK_START = 4,

        GUISOUND_LEVELUP = 12,
        GUISOUND_NEWQUEST = 13,
        GUISOUND_QUESTCOMPLETE = 14,

        GUISOUND_ASSEMBLE = 16,

        PANEL_MODE_MODAL = 3,

        PANEL_MODE_BACKGROUND = 1,

        PANEL_MODE_FOREGROUND = 2
    };

    static const char ACTION_CYCLE_SOUND = 7;

    static Vector DISABLED_TEXT;
    static Vector COLOR_YELLOW;

    void PlayGuiSound(char nSound);
    CSWGuiPanel* PopModalPanel();
    void AddPanel(CSWGuiPanel* pPanel, int nFlags, int bPlaySound);
    int PanelExists(CSWGuiPanel* pPanel);
    int IsOnTop(CSWGuiPanel* pPanel);
    int RemovePanel(CSWGuiPanel* pPanel);
#ifndef _XBOX
    void ReleaseMouse(CSWGuiObject* pObject);
#endif

#ifndef _XBOX
    int m_nMouseX;
    int m_nMouseY;
    CSWGuiControl* m_pMouseOverControl;
    int m_nMouseCaptureType;
    CSWGuiObject* m_pMouseCapture;
#endif

#ifndef _XBOX
    CSWGuiControl* m_pLastMouseOverControl;
    CSWGuiEditbox* m_pKeyFocus;
    unsigned long m_bLeftMouseDown : 1;
    unsigned long m_bUnknown1C1 : 1;
    unsigned long m_bCursorCentered : 1;
    unsigned long m_bToolTipShown : 1;
    unsigned long m_nUnknown1CRest : 28;
    CSWGuiScene* m_pMouseScene;
    CAurObject* m_pMouseCursor;
    CResRef m_resCursor;
    int m_nMouseCursor;
    CSWGuiToolTipPanel* m_pToolTipPanel;
    CSWGuiControl* m_pToolTipControl;
    float m_fToolTipTimer;
    float m_fToolTipDelay;
    int m_bToolTipDisabled;
    CSWGuiControl* m_pToolTipDisabledControl;
    CSWGuiControl* m_pDragControl;
    int m_nOldMouseX;
    int m_nOldMouseY;
#endif
    float m_fGuiScale;
    unsigned long m_nLastKeyTime;
    int m_nLastInputEvent;
    short m_nWidth;
    short m_nHeight;
    unsigned char m_nBackgroundPanels;
#ifndef _XBOX
    char m_nCountedPanels;
#endif
    unsigned char m_nLastDirection;
#ifndef _XBOX
    CExoString m_sBackground;
    CSWGuiLabel* m_pPCBackground;
#endif
    CSWGuiPanel* m_pSafeRegionPanel;
    CSWGuiLabel* m_pSafeRegionBorder;
    CExoArrayList<CSWGuiPanel*> m_lPanels;
    CExoArrayList<CSWGuiPanel*> m_lModalPanels;
    CExoSoundSource** m_apGuiSounds;
    char m_nGuiSounds;
#ifndef _XBOX
    void ShowSoftwareMouse();
#endif
    void HideSoftwareMouse();
#ifndef _XBOX
    void DeactivateSoftwareMouse();
#endif
#ifndef _XBOX
    void SetSoftwareMouseCursor(int nCursor, int nArg2, int nArg3);
#endif
#ifndef _XBOX
    int HandleLMouseDown(int nArg);
#endif
#ifndef _XBOX
    int HandleRMouseDown();
#endif
#ifndef _XBOX
    int HandleRMouseUp();
#endif
#ifndef _XBOX
    void UpdateAllFonts();
#endif
    static CExoString GetScreenResolutionString(unsigned long nWidth,
                                                unsigned long nHeight);
    int HitCheckMouse(int nX, int nY, CSWGuiPanel** ppPanel,
                      CSWGuiControl** ppControl, int bModalOnly);
    CSWGuiManager();
    ~CSWGuiManager();
    int LoadGuiSounds();
    void UnloadGuiSounds();
#ifndef _XBOX
    int HandleLMouseUp();
#endif
#ifndef _XBOX
    void CaptureMouse(CSWGuiObject* pObject, int nType);
#endif
#ifndef _XBOX
    int ActivateSoftwareMouse();
#endif
#ifndef _XBOX
    void ChangeToolTipText(const CExoString& sText);
#endif
#ifndef _XBOX
    void ReloadToolTipPanel();
#endif
#ifndef _XBOX
    void StoreCurrentMousePosition();
#endif
    void GetSafeMargin(int& nX, int& nY);
    int GetPosInModalStack(CSWGuiPanel* pPanel);
    int GetPosInPanelList(CSWGuiPanel* pPanel);
    void DetermineNeedToDraw();
#ifndef _XBOX
    void DrawPCBG(float fElapsed);
#endif
    int HandleKeyPress(int nKey);
#ifndef _XBOX
    CSWGuiControl* UpdateMouseOverControl();
#endif
    void GetUpdatedFontName(char* sFont);
#ifndef _XBOX
    void DisplayToolTip(const CExoString& sText);
#endif
#ifndef _XBOX
    void ResetToolTipTimer();
#endif
#ifndef _XBOX
    void SetTemporarilyDisableToolTip(int bDisable);
#endif
    void SendPanelToBack(CSWGuiPanel* pPanel);
    int PushModalPanel(CSWGuiPanel* pPanel);
#ifndef _XBOX
    void SetSize(int nWidth, int nHeight);
#endif
    void DrawSafeRegion(float fElapsed);
    void HideAllPanels();
#ifndef _XBOX
    void RemoveToolTip();
#endif
#ifndef _XBOX
    void HandleMouseMove(int nX, int nY);
#endif
#ifndef _XBOX
    int HandleZMouse(int nDelta);
#endif
#ifndef _XBOX
    void MoveMouseToPosition(int nX, int nY);
#endif
#ifndef _XBOX
    void RestoreOldMousePosition();
#endif
    void HandleInputEvent(int nEvent, int nState);
    void Draw(float fElapsed);
    void Update(float fElapsed);
    void SetGuiScale(float fScale);
};

#define SWGUI_MESSAGEEVENT_OK 502
#define SWGUI_MESSAGEEVENT_CANCEL 503

class CSWGuiMessageBox : public CSWGuiPanel
{
public:
    CSWGuiMessageBox(CSWGuiManager* pManager);
    virtual void SetMessage(CExoString sMessage);
    virtual void SetMessage(unsigned long nStrRef);
    virtual void SetOkButtonMessage(unsigned long nStrRef);
    virtual void SetCancelButtonMessage(unsigned long nStrRef);
    virtual void HideButtons();
    virtual void ResetButtonMessage();
    virtual void HandleInputEvent(int nEvent, int nState);
    virtual void OnPanelAdded();
    virtual void OnPanelRemoved();

    void SetAllowCancel(int bAllowCancel);
    void SetIcon(const CResRef& cIcon);
    void SetCallback(CSWGuiObject* pObject, CSWGuiHandler pHandler,
                    CSWGuiControl* pControl);
    void UpdateControls();
    void AddMessageLabel();
    void FixMessageLabel();
#ifndef _XBOX
    void OnOkButtonPressed(CSWGuiControl* pControl); void OnCancelButtonPressed(CSWGuiControl* pControl);
#else
    void ShowLoadError(unsigned long nStrRef); void OnRestart(CSWGuiControl* pControl);
#endif
    void OnDeath(CSWGuiControl* pControl); void DisplayDeathMessage();

    unsigned long m_bAccepted : 1;
    unsigned long m_bAllowCancel : 1;
    unsigned long m_bShowButtons : 1;
    unsigned long m_bHideButtons : 1;
    unsigned long m_bHasIcon : 1;
    unsigned long m_nMessageFlagsUnused : 27;
    CSWGuiHandler m_pCallback;
    CSWGuiObject* m_pCallbackObject;
    CSWGuiControl* m_pCallbackControl;
#ifdef _XBOX
    CSWGuiLabel m_lblA;
    CSWGuiLabel m_lblB;
    CSWGuiLabel m_lblOk;
    CSWGuiLabel m_lblCancel;
#endif
    CSWGuiLabel m_lblMessage;
#ifdef _XBOX
    CSWGuiLabel m_lblRAnalog;
#endif
    CSWGuiLabel m_lblIcon;
#ifndef _XBOX
    CSWGuiButton m_btnOk;
    CSWGuiButton m_btnCancel;
#endif
    CSWGuiListBox m_lbMessage;
    CSWGuiExtent m_cPanelExtent;
    CSWGuiExtent m_cMessageExtent;
#ifdef _XBOX

    int m_nLblABottomOffset;
    int m_nLblBBottomOffset;
    int m_nLblRAnalogRightOffset;
#endif
    int m_nPreviousInputClass;
#ifndef _XBOX
    int m_bFocusOk;
#endif
};

struct CSWGuiTimedControl
{
public:
    CSWGuiControl* m_pControl;
    float m_fTimeLeft;
};

#ifndef _XBOX
#pragma pack(push, 4)
#endif
class CSWGuiInterfaceAction
{
public:

    static const int INVALID_ACTION = -1;

    enum Action
    {

        ACTION_DIALOG = 1002,
        ACTION_OPEN_DOOR = 1010,
        ACTION_SECURITY = 1011,
        ACTION_DISABLE_MINE = 1012,
        ACTION_BASH = 1013,
        ACTION_USE_PLACEABLE = 1015,
        ACTION_RECOVER_MINE = 1026,
        ACTION_NO_ACTION = 1028
    };

    static const unsigned long SPELL_ACTION_MASK = 0x20000000;

    static const unsigned long FEAT_ACTION_MASK = 0x10000000;

    enum { STRREF_SELF = 38005 };
    enum DisabledReason
    {
        DISABLED_REASON_NONE = 0,
        DISABLED_REASON_FORCE_POINTS = 1,
        DISABLED_REASON_FORBIDDEN_ITEM = 2,
        DISABLED_REASON_REQUIRED_ITEM = 3,
        DISABLED_REASON_RANGE = 4,

        DISABLED_REASON_FULL_HEALTH = 5,
        DISABLED_REASON_DEAD = 6
    };

    enum { STRREF_ATTACK = 375 };
    enum { ACTION_ATTACK = 1003 };
    // PC 0x0050B780
    CSWGuiInterfaceAction()
    {
        Clear();
    }
    void Clear();

    CExoString m_sName;
    int m_nAction;
    PFNMENUACTION m_pfnAction;
    unsigned long m_oidTarget;
    CResRef m_resIcon;
    unsigned long m_bEnabled : 1;
    unsigned long m_nDisabledReason : 4;

    unsigned long : 27;
#ifndef _XBOX
    int m_nStackSize;
#endif
};
#ifndef _XBOX
#pragma pack(pop)
#endif

#include "swguiskillflowchart.h"
#include "swguicontrol.h"
#include "exobase.h"
#include "exoresman.h"
#include "math3d.h"
#include "swguidrawable.h"

class CSWGuiControl;
class CSWGuiPanel;
class CSWGuiObject;
class CSWGuiButtonToggle;

class CSWGuiManager;

#include "swguicontrol.h"
#include "exobase.h"
#include "exoresman.h"
#include "math3d.h"
#include "swguidrawable.h"

class CSWGuiManager;
class CSWGuiControl;
class CSWGuiObject;

class CSWGuiText;
#include "swguiskillflowchart.h"
#include "swgui.h"
#include "swguicontrol.h"
#include "exobase.h"
#include "exoresman.h"
#include "math3d.h"
#include "swguidrawable.h"
#include "resgff.h"

class CSWGuiControl;
class CSWGuiPanel;
class CSWGuiObject;

class CSWGuiManager;
class CSWGuiNavigable;
class CSWGuiListBox;

class CSWGuiListBox;

class CSWGuiBorder;
class CSWGuiToolTipPanel;
class CAurObject;
class CExoSoundSource;

class CSWGuiToolTipPanel : public CSWGuiPanel
{
public:
    CSWGuiToolTipPanel(CSWGuiManager* pManager);

    void SetToolTipText(const CExoString& sText);
#ifndef _XBOX
    void PositionToolTip();
#endif

    CSWGuiLabel m_lblToolTip;
};

extern int g_bDrawSafeRegion;

#define PC_BACKGROUND_SUFFIX_COMPUTER 4

#define PC_BACKGROUND_SUFFIX_PAZAAK 2
extern char* g_asMouseCursorNames[];

#endif
