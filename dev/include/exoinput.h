#ifndef EXOINPUT_H
#define EXOINPUT_H

#if defined(_WIN32) && !defined(_XBOX)
#include <windows.h>
#else
#include "platform.h"
#endif

#define WM_MOUSEWHEEL 0x020A
#include "exobase.h"

class CExoFile;
class CExoInputEvent;
class CExoInputEventDesc;
class CExoInputInternal;
class CExoInput;
extern CExoInput* g_pExoInput;

class CExoInput
{
public:

    enum { MOUSE_MODE_SOFTWARE = 1 };
    CExoInput();
    virtual ~CExoInput();

    virtual void Configure();
    void UpdateJoysticks();
    int SetRumble(int nArg1, float fArg2, float fArg3, unsigned long nArg4);
    int GetMainDeviceLost();
    void PauseRumble();
    void UnpauseRumble();
    CExoString GetDisplayCharacterForKey(int nKey);
    unsigned char GetMouseMode();
    int EventExists(unsigned long nEvent);
    int GetLastSetMouseDownState();

    enum EventClassSelector
    {
        EVENTCLASS_ANY = -1
    };

    CExoInputEventDesc* GetEvent(unsigned long nEvent, int nEventClass);
    float PollInput(unsigned long nEvent, int nEventClass);
    void ClearEvents();
    void EndAttemptedBind();

    void WriteEventToFile(unsigned long nTimeStamp, int nEvent,
                          CExoFile* pFile);
    int GetActive();
    void SetActive(int bActive);
    void SetMousePos(int nX, int nY);
    CExoArrayList<CExoInputEvent> GetEvents(int& nEvents, int nEventClass);
    void SetCaptureKeyboardKeys(int bCapture);

    int CreateNewEvent(unsigned long nArg1, int nArg2, int nArg3,
                       unsigned long nArg4, unsigned long nArg5);

    enum EventType
    {
        EVENT_TYPE_POLLED = 0, EVENT_TYPE_BUTTON = 1,

        EVENT_TYPE_STATE = 2, EVENT_TYPE_RELATIVE = 3,
        EVENT_TYPE_PAIR = 4, EVENT_TYPE_VALUE = 5
    };

#ifdef _XBOX
    enum { INPUT_CONTROL_NONE = 109 };
#else
    enum { INPUT_CONTROL_NONE = 132 };
#endif

    enum { INPUT_CONTROL_INDEX_MASK = 0xFF };
    void AddEvent(unsigned long nArg1, unsigned long nArg2);
    int ScaleEvent(unsigned long nArg1, float fArg2, float fArg3);
    int SetEventRepeatable(unsigned long nArg1, unsigned long nArg2,
                           unsigned long nArg3);
    void ClearEventRepeat(unsigned long nArg1);
    void CoolDownEvent(unsigned long nArg1, unsigned long nArg2);
    int GetKeyboardMode();
    void KeyboardModeOn();
    void KeyboardModeOff();
    void BufferEvent(void* pArg1);
    int SetMouseMode(unsigned char cArg1);
    void RefreshMouseCursor(int nArg1);
    int SetMouseCursor(int nArg1, int nArg2, int nArg3);
    void HideMouse();
    void ShowMouse();
    void ClearAllInputEventDescriptions();
    void GetMouseDelta(float* fArg1, float* fArg2);
    void UpdateMouseDelta(float fArg1);
    int IsModalKeyDown_Shift();
    int IsModalKeyDown_Ctrl();
    int IsModalKeyDown_Alt();
    int GetLastCapturedKeyboardKey();
#ifdef _XBOX

    void SetEventNonRepeatable(unsigned long nEvent);
    int GetAutoCalInfo();
    int GetPortBinding();
    int SetAutoCalInfo(unsigned long nFlags);
    int SetPortBinding(unsigned long nBinding);
    int GetCheatCombo(unsigned char nCombo);
    int GetDebugOption();
#endif

    CExoInputInternal* m_pExoInputInternal;

    static const int INPUTDEVICE_NONE;
    static const int INPUTDEVICE_KEYBOARD;
    static const int INPUTDEVICE_MOUSE;

    static const int INPUTDEVICE_JOYSTICKSTART;
    static const int MOUSE_START;
    static const int KEYBOARD_START;
    static const int JOYSTICK_START;

    enum MouseButtonValue
    {
        MOUSE_BUTTON_RELEASED = 0,
        MOUSE_BUTTON_PRESSED = 1,
        MOUSE_BUTTON_DOUBLE_CLICK = -1
    };

    static const int MOUSE_BUTTON0;
    static const int MOUSE_BUTTON1;
    static const int MOUSE_BUTTON2;
    static const int MOUSE_XAXIS;
    static const int MOUSE_YAXIS;
    static const int MOUSE_ZAXIS;
    static const int KEYBOARD_RETURN;
    static const int KEYBOARD_LEFT_ARROW;
    static const int KEYBOARD_RIGHT_ARROW;
    static const int KEYBOARD_UP_ARROW;
    static const int KEYBOARD_DOWN_ARROW;
    static const int KEYBOARD_NUMPAD1;
    static const int KEYBOARD_NUMPAD2;
    static const int KEYBOARD_NUMPAD3;
    static const int KEYBOARD_NUMPAD4;
    static const int KEYBOARD_NUMPAD5;
    static const int KEYBOARD_NUMPAD6;
    static const int KEYBOARD_NUMPAD7;
    static const int KEYBOARD_NUMPAD8;
    static const int KEYBOARD_NUMPAD9;
    static const int KEYBOARD_NUMPAD0;
    static const int KEYBOARD_NUMPADDECIMAL;
    static const int KEYBOARD_NUMPADMINUS;
    static const int KEYBOARD_NUMPADPLUS;
    static const int KEYBOARD_LEFTSHIFT;
    static const int KEYBOARD_RIGHTSHIFT;
    static const int KEYBOARD_LEFTALT;
    static const int KEYBOARD_RIGHTALT;
    static const int KEYBOARD_LEFTCTRL;
    static const int KEYBOARD_RIGHTCTRL;
    static const int KEYBOARD_TAB;
    static const int KEYBOARD_ESC;
    static const int KEYBOARD_HOME;
    static const int KEYBOARD_END;
    static const int KEYBOARD_PAGEUP;
    static const int KEYBOARD_PAGEDOWN;
    static const int KEYBOARD_INSERT;
    static const int KEYBOARD_DELETE;
    static const int KEYBOARD_PRINTSCREEN;
    static const int KEYBOARD_F1;
    static const int KEYBOARD_F2;
    static const int KEYBOARD_F3;
    static const int KEYBOARD_F4;
    static const int KEYBOARD_F5;
    static const int KEYBOARD_F6;
    static const int KEYBOARD_F7;
    static const int KEYBOARD_F8;
    static const int KEYBOARD_F9;
    static const int KEYBOARD_F10;
    static const int KEYBOARD_F11;
    static const int KEYBOARD_F12;
    static const int KEYBOARD_A;
    static const int KEYBOARD_B;
    static const int KEYBOARD_C;
    static const int KEYBOARD_D;
    static const int KEYBOARD_E;
    static const int KEYBOARD_F;
    static const int KEYBOARD_G;
    static const int KEYBOARD_H;
    static const int KEYBOARD_I;
    static const int KEYBOARD_J;
    static const int KEYBOARD_K;
    static const int KEYBOARD_L;
    static const int KEYBOARD_M;
    static const int KEYBOARD_N;
    static const int KEYBOARD_O;
    static const int KEYBOARD_P;
    static const int KEYBOARD_Q;
    static const int KEYBOARD_R;
    static const int KEYBOARD_S;
    static const int KEYBOARD_T;
    static const int KEYBOARD_U;
    static const int KEYBOARD_V;
    static const int KEYBOARD_W;
    static const int KEYBOARD_X;
    static const int KEYBOARD_Y;
    static const int KEYBOARD_Z;
    static const int KEYBOARD_1;
    static const int KEYBOARD_2;
    static const int KEYBOARD_3;
    static const int KEYBOARD_4;
    static const int KEYBOARD_5;
    static const int KEYBOARD_6;
    static const int KEYBOARD_7;
    static const int KEYBOARD_8;
    static const int KEYBOARD_9;
    static const int KEYBOARD_0;
    static const int KEYBOARD_SPACE;
    static const int KEYBOARD_NUMPADENTER;
    static const int KEYBOARD_CAPSLOCK;
    static const int KEYBOARD_PAUSE;
    static const int KEYBOARD_F13;
    static const int KEYBOARD_F14;
    static const int KEYBOARD_F15;
    static const int KEYBOARD_MINUS;
    static const int KEYBOARD_EQUALS;
    static const int KEYBOARD_BACK;
    static const int KEYBOARD_LBRACKET;
    static const int KEYBOARD_RBRACKET;
    static const int KEYBOARD_SEMICOLON;
    static const int KEYBOARD_APOSTROPHE;
    static const int KEYBOARD_GRAVE;
    static const int KEYBOARD_BACKSLASH;
    static const int KEYBOARD_COMMA;
    static const int KEYBOARD_PERIOD;
    static const int KEYBOARD_SLASH;
    static const int KEYBOARD_MULTIPLY;
    static const int KEYBOARD_NUMPADCOMMA;
    static const int KEYBOARD_DIVIDE;
    static const int JOYSTICK_XAXIS;
    static const int JOYSTICK_YAXIS;
    static const int JOYSTICK_HAT;
    static const int JOYSTICK_SLIDER0;
    static const int JOYSTICK_SLIDER1;
    static const int JOYSTICK_SLIDER2;
    static const int JOYSTICK_BUTTON0;
    static const int JOYSTICK_BUTTON1;
    static const int JOYSTICK_BUTTON2;
    static const int JOYSTICK_BUTTON3;
    static const int JOYSTICK_BUTTON4;
    static const int JOYSTICK_BUTTON5;
    static const int JOYSTICK_BUTTON6;
    static const int JOYSTICK_BUTTON7;
    static const int JOYSTICK_BUTTON8;
    static const int JOYSTICK_BUTTON9;
    static const int JOYSTICK_BUTTON10;
    static const int JOYSTICK_BUTTON11;
    static const int JOYSTICK_BUTTON12;
    static const int JOYSTICK_BUTTON13;
    static const int JOYSTICK_BUTTON14;
    static const int JOYSTICK_BUTTON15;
};

#endif
