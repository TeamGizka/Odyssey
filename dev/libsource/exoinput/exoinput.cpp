#include "exoinput.h"
#include "exoinputinternal.h"
#include "swmain.h"

static inline void ExoinputTrace1();
#define EXOINPUT_TRACE() ExoinputTrace1()

#ifndef _XBOX
const int CExoInput::INPUTDEVICE_NONE = -1;
const int CExoInput::INPUTDEVICE_KEYBOARD = 0;
const int CExoInput::INPUTDEVICE_MOUSE = 1;
const int CExoInput::INPUTDEVICE_JOYSTICKSTART = 2;

const int CExoInput::MOUSE_START = 0;
const int CExoInput::KEYBOARD_START = 6;
const int CExoInput::JOYSTICK_START = 110;

const int CExoInput::MOUSE_BUTTON0 = 0;
const int CExoInput::MOUSE_BUTTON1 = 1;
const int CExoInput::MOUSE_BUTTON2 = 2;
const int CExoInput::MOUSE_XAXIS = 3;
const int CExoInput::MOUSE_YAXIS = 4;
const int CExoInput::MOUSE_ZAXIS = 5;

const int CExoInput::KEYBOARD_RETURN = 6;
const int CExoInput::KEYBOARD_LEFT_ARROW = 7;
const int CExoInput::KEYBOARD_RIGHT_ARROW = 8;
const int CExoInput::KEYBOARD_UP_ARROW = 9;
const int CExoInput::KEYBOARD_DOWN_ARROW = 10;
const int CExoInput::KEYBOARD_NUMPAD1 = 11;
const int CExoInput::KEYBOARD_NUMPAD2 = 12;
const int CExoInput::KEYBOARD_NUMPAD3 = 13;
const int CExoInput::KEYBOARD_NUMPAD4 = 14;
const int CExoInput::KEYBOARD_NUMPAD5 = 15;
const int CExoInput::KEYBOARD_NUMPAD6 = 16;
const int CExoInput::KEYBOARD_NUMPAD7 = 17;
const int CExoInput::KEYBOARD_NUMPAD8 = 18;
const int CExoInput::KEYBOARD_NUMPAD9 = 19;
const int CExoInput::KEYBOARD_NUMPAD0 = 20;
const int CExoInput::KEYBOARD_NUMPADDECIMAL = 21;
const int CExoInput::KEYBOARD_NUMPADMINUS = 22;
const int CExoInput::KEYBOARD_NUMPADPLUS = 23;
const int CExoInput::KEYBOARD_LEFTSHIFT = 24;
const int CExoInput::KEYBOARD_RIGHTSHIFT = 25;
const int CExoInput::KEYBOARD_LEFTALT = 26;
const int CExoInput::KEYBOARD_RIGHTALT = 27;
const int CExoInput::KEYBOARD_LEFTCTRL = 28;
const int CExoInput::KEYBOARD_RIGHTCTRL = 29;
const int CExoInput::KEYBOARD_TAB = 30;
const int CExoInput::KEYBOARD_ESC = 31;
const int CExoInput::KEYBOARD_HOME = 32;
const int CExoInput::KEYBOARD_END = 33;
const int CExoInput::KEYBOARD_PAGEUP = 34;
const int CExoInput::KEYBOARD_PAGEDOWN = 35;
const int CExoInput::KEYBOARD_INSERT = 36;
const int CExoInput::KEYBOARD_DELETE = 37;
const int CExoInput::KEYBOARD_PRINTSCREEN = 38;
const int CExoInput::KEYBOARD_F1 = 39;
const int CExoInput::KEYBOARD_F2 = 40;
const int CExoInput::KEYBOARD_F3 = 41;
const int CExoInput::KEYBOARD_F4 = 42;
const int CExoInput::KEYBOARD_F5 = 43;
const int CExoInput::KEYBOARD_F6 = 44;
const int CExoInput::KEYBOARD_F7 = 45;
const int CExoInput::KEYBOARD_F8 = 46;
const int CExoInput::KEYBOARD_F9 = 47;
const int CExoInput::KEYBOARD_F10 = 48;
const int CExoInput::KEYBOARD_F11 = 49;
const int CExoInput::KEYBOARD_F12 = 50;
const int CExoInput::KEYBOARD_A = 51;
const int CExoInput::KEYBOARD_B = 52;
const int CExoInput::KEYBOARD_C = 53;
const int CExoInput::KEYBOARD_D = 54;
const int CExoInput::KEYBOARD_E = 55;
const int CExoInput::KEYBOARD_F = 56;
const int CExoInput::KEYBOARD_G = 57;
const int CExoInput::KEYBOARD_H = 58;
const int CExoInput::KEYBOARD_I = 59;
const int CExoInput::KEYBOARD_J = 60;
const int CExoInput::KEYBOARD_K = 61;
const int CExoInput::KEYBOARD_L = 62;
const int CExoInput::KEYBOARD_M = 63;
const int CExoInput::KEYBOARD_N = 64;
const int CExoInput::KEYBOARD_O = 65;
const int CExoInput::KEYBOARD_P = 66;
const int CExoInput::KEYBOARD_Q = 67;
const int CExoInput::KEYBOARD_R = 68;
const int CExoInput::KEYBOARD_S = 69;
const int CExoInput::KEYBOARD_T = 70;
const int CExoInput::KEYBOARD_U = 71;
const int CExoInput::KEYBOARD_V = 72;
const int CExoInput::KEYBOARD_W = 73;
const int CExoInput::KEYBOARD_X = 74;
const int CExoInput::KEYBOARD_Y = 75;
const int CExoInput::KEYBOARD_Z = 76;
const int CExoInput::KEYBOARD_1 = 77;
const int CExoInput::KEYBOARD_2 = 78;
const int CExoInput::KEYBOARD_3 = 79;
const int CExoInput::KEYBOARD_4 = 80;
const int CExoInput::KEYBOARD_5 = 81;
const int CExoInput::KEYBOARD_6 = 82;
const int CExoInput::KEYBOARD_7 = 83;
const int CExoInput::KEYBOARD_8 = 84;
const int CExoInput::KEYBOARD_9 = 85;
const int CExoInput::KEYBOARD_0 = 86;
const int CExoInput::KEYBOARD_SPACE = 87;
const int CExoInput::KEYBOARD_NUMPADENTER = 88;
const int CExoInput::KEYBOARD_CAPSLOCK = 89;
const int CExoInput::KEYBOARD_PAUSE = 90;
const int CExoInput::KEYBOARD_F13 = 91;
const int CExoInput::KEYBOARD_F14 = 92;
const int CExoInput::KEYBOARD_F15 = 93;
const int CExoInput::KEYBOARD_MINUS = 94;
const int CExoInput::KEYBOARD_EQUALS = 95;
const int CExoInput::KEYBOARD_BACK = 96;
const int CExoInput::KEYBOARD_LBRACKET = 97;
const int CExoInput::KEYBOARD_RBRACKET = 98;
const int CExoInput::KEYBOARD_SEMICOLON = 99;
const int CExoInput::KEYBOARD_APOSTROPHE = 100;
const int CExoInput::KEYBOARD_GRAVE = 101;
const int CExoInput::KEYBOARD_BACKSLASH = 102;
const int CExoInput::KEYBOARD_COMMA = 103;
const int CExoInput::KEYBOARD_PERIOD = 104;
const int CExoInput::KEYBOARD_SLASH = 105;
const int CExoInput::KEYBOARD_MULTIPLY = 106;
const int CExoInput::KEYBOARD_NUMPADCOMMA = 107;
const int CExoInput::KEYBOARD_DIVIDE = 108;
const int CExoInput::KEYBOARD_OEM_102 = 109;
const int CExoInput::JOYSTICK_XAXIS = 110;
const int CExoInput::JOYSTICK_YAXIS = 111;
const int CExoInput::JOYSTICK_HAT = 112;
const int CExoInput::JOYSTICK_SLIDER0 = 113;
const int CExoInput::JOYSTICK_SLIDER1 = 114;
const int CExoInput::JOYSTICK_SLIDER2 = 115;
const int CExoInput::JOYSTICK_BUTTON0 = 116;
const int CExoInput::JOYSTICK_BUTTON1 = 117;
const int CExoInput::JOYSTICK_BUTTON2 = 118;
const int CExoInput::JOYSTICK_BUTTON3 = 119;
const int CExoInput::JOYSTICK_BUTTON4 = 120;
const int CExoInput::JOYSTICK_BUTTON5 = 121;
const int CExoInput::JOYSTICK_BUTTON6 = 122;
const int CExoInput::JOYSTICK_BUTTON7 = 123;
const int CExoInput::JOYSTICK_BUTTON8 = 124;
const int CExoInput::JOYSTICK_BUTTON9 = 125;
const int CExoInput::JOYSTICK_BUTTON10 = 126;
const int CExoInput::JOYSTICK_BUTTON11 = 127;
const int CExoInput::JOYSTICK_BUTTON12 = 128;
const int CExoInput::JOYSTICK_BUTTON13 = 129;
const int CExoInput::JOYSTICK_BUTTON14 = 130;
const int CExoInput::JOYSTICK_BUTTON15 = 131;
#else

const int CExoInput::INPUTDEVICE_NONE = -1;
const int CExoInput::INPUTDEVICE_KEYBOARD = 0;
const int CExoInput::INPUTDEVICE_MOUSE = 1;
const int CExoInput::INPUTDEVICE_JOYSTICKSTART = 0;

const int CExoInput::MOUSE_START = 0;
const int CExoInput::KEYBOARD_START = 5;
const int CExoInput::JOYSTICK_START = 87;

const int CExoInput::MOUSE_BUTTON0 = 0;
const int CExoInput::MOUSE_BUTTON1 = 1;
const int CExoInput::MOUSE_BUTTON2 = 2;
const int CExoInput::MOUSE_XAXIS = 3;
const int CExoInput::MOUSE_YAXIS = 4;
const int CExoInput::KEYBOARD_RETURN = 5;
const int CExoInput::KEYBOARD_LEFT_ARROW = 6;
const int CExoInput::KEYBOARD_RIGHT_ARROW = 7;
const int CExoInput::KEYBOARD_UP_ARROW = 8;
const int CExoInput::KEYBOARD_DOWN_ARROW = 9;
const int CExoInput::KEYBOARD_NUMPAD1 = 10;
const int CExoInput::KEYBOARD_NUMPAD2 = 11;
const int CExoInput::KEYBOARD_NUMPAD3 = 12;
const int CExoInput::KEYBOARD_NUMPAD4 = 13;
const int CExoInput::KEYBOARD_NUMPAD5 = 14;
const int CExoInput::KEYBOARD_NUMPAD6 = 15;
const int CExoInput::KEYBOARD_NUMPAD7 = 16;
const int CExoInput::KEYBOARD_NUMPAD8 = 17;
const int CExoInput::KEYBOARD_NUMPAD9 = 18;
const int CExoInput::KEYBOARD_NUMPAD0 = 19;
const int CExoInput::KEYBOARD_NUMPADDECIMAL = 20;
const int CExoInput::KEYBOARD_NUMPADMINUS = 21;
const int CExoInput::KEYBOARD_NUMPADPLUS = 22;
const int CExoInput::KEYBOARD_LEFTSHIFT = 23;
const int CExoInput::KEYBOARD_RIGHTSHIFT = 24;
const int CExoInput::KEYBOARD_LEFTALT = 25;
const int CExoInput::KEYBOARD_RIGHTALT = 26;
const int CExoInput::KEYBOARD_LEFTCTRL = 27;
const int CExoInput::KEYBOARD_RIGHTCTRL = 28;
const int CExoInput::KEYBOARD_TAB = 29;
const int CExoInput::KEYBOARD_ESC = 30;
const int CExoInput::KEYBOARD_HOME = 31;
const int CExoInput::KEYBOARD_END = 32;
const int CExoInput::KEYBOARD_PAGEUP = 33;
const int CExoInput::KEYBOARD_PAGEDOWN = 34;
const int CExoInput::KEYBOARD_INSERT = 35;
const int CExoInput::KEYBOARD_DELETE = 36;
const int CExoInput::KEYBOARD_PRINTSCREEN = 37;
const int CExoInput::KEYBOARD_F1 = 38;
const int CExoInput::KEYBOARD_F2 = 39;
const int CExoInput::KEYBOARD_F3 = 40;
const int CExoInput::KEYBOARD_F4 = 41;
const int CExoInput::KEYBOARD_F5 = 42;
const int CExoInput::KEYBOARD_F6 = 43;
const int CExoInput::KEYBOARD_F7 = 44;
const int CExoInput::KEYBOARD_F8 = 45;
const int CExoInput::KEYBOARD_F9 = 46;
const int CExoInput::KEYBOARD_F10 = 47;
const int CExoInput::KEYBOARD_F11 = 48;
const int CExoInput::KEYBOARD_F12 = 49;
const int CExoInput::KEYBOARD_A = 50;
const int CExoInput::KEYBOARD_B = 51;
const int CExoInput::KEYBOARD_C = 52;
const int CExoInput::KEYBOARD_D = 53;
const int CExoInput::KEYBOARD_E = 54;
const int CExoInput::KEYBOARD_F = 55;
const int CExoInput::KEYBOARD_G = 56;
const int CExoInput::KEYBOARD_H = 57;
const int CExoInput::KEYBOARD_I = 58;
const int CExoInput::KEYBOARD_J = 59;
const int CExoInput::KEYBOARD_K = 60;
const int CExoInput::KEYBOARD_L = 61;
const int CExoInput::KEYBOARD_M = 62;
const int CExoInput::KEYBOARD_N = 63;
const int CExoInput::KEYBOARD_O = 64;
const int CExoInput::KEYBOARD_P = 65;
const int CExoInput::KEYBOARD_Q = 66;
const int CExoInput::KEYBOARD_R = 67;
const int CExoInput::KEYBOARD_S = 68;
const int CExoInput::KEYBOARD_T = 69;
const int CExoInput::KEYBOARD_U = 70;
const int CExoInput::KEYBOARD_V = 71;
const int CExoInput::KEYBOARD_W = 72;
const int CExoInput::KEYBOARD_X = 73;
const int CExoInput::KEYBOARD_Y = 74;
const int CExoInput::KEYBOARD_Z = 75;
const int CExoInput::KEYBOARD_1 = 76;
const int CExoInput::KEYBOARD_2 = 77;
const int CExoInput::KEYBOARD_3 = 78;
const int CExoInput::KEYBOARD_4 = 79;
const int CExoInput::KEYBOARD_5 = 80;
const int CExoInput::KEYBOARD_6 = 81;
const int CExoInput::KEYBOARD_7 = 82;
const int CExoInput::KEYBOARD_8 = 83;
const int CExoInput::KEYBOARD_9 = 84;
const int CExoInput::KEYBOARD_0 = 85;
const int CExoInput::KEYBOARD_SPACE = 86;
const int CExoInput::JOYSTICK_XAXIS = 87;
const int CExoInput::JOYSTICK_YAXIS = 88;
const int CExoInput::JOYSTICK_HAT = 89;
const int CExoInput::JOYSTICK_SLIDER0 = 90;
const int CExoInput::JOYSTICK_SLIDER1 = 91;
const int CExoInput::JOYSTICK_SLIDER2 = 92;
const int CExoInput::JOYSTICK_BUTTON0 = 93;
const int CExoInput::JOYSTICK_BUTTON1 = 94;
const int CExoInput::JOYSTICK_BUTTON2 = 95;
const int CExoInput::JOYSTICK_BUTTON3 = 96;
const int CExoInput::JOYSTICK_BUTTON4 = 97;
const int CExoInput::JOYSTICK_BUTTON5 = 98;
const int CExoInput::JOYSTICK_BUTTON6 = 99;
const int CExoInput::JOYSTICK_BUTTON7 = 100;
const int CExoInput::JOYSTICK_BUTTON8 = 101;
const int CExoInput::JOYSTICK_BUTTON9 = 102;
const int CExoInput::JOYSTICK_BUTTON10 = 103;
const int CExoInput::JOYSTICK_BUTTON11 = 104;
const int CExoInput::JOYSTICK_BUTTON12 = 105;
const int CExoInput::JOYSTICK_BUTTON13 = 106;
const int CExoInput::JOYSTICK_BUTTON14 = 107;
const int CExoInput::JOYSTICK_BUTTON15 = 108;
#endif
// PC 0x0062B6F0
CExoInput::CExoInput()
{
#ifdef _XBOX
    m_pExoInputInternal = new CExoInputInternal;
#else
    m_pExoInputInternal = new CExoInputInternal(g_hInst, g_hRenderWnd);
#endif
}
// PC 0x0062B760
int CExoInput::CreateNewEvent(unsigned long nArg1, int nArg2, int nArg3,
                              unsigned long nArg4, unsigned long nArg5)
{
    return m_pExoInputInternal->CreateNewEvent(nArg1, nArg2, nArg3, nArg4,
                                               nArg5);
}
// PC 0x0062B770
void CExoInput::AddEvent(unsigned long nArg1, unsigned long nArg2)
{
    m_pExoInputInternal->AddEvent(nArg1, nArg2);
}
// PC 0x0062B780
int CExoInput::ScaleEvent(unsigned long nArg1, float fArg2, float fArg3)
{
    return m_pExoInputInternal->ScaleEvent(nArg1, fArg2, fArg3);
}
// PC 0x0062B790
int CExoInput::SetEventRepeatable(unsigned long nArg1, unsigned long nArg2,
                                  unsigned long nArg3)
{
    return m_pExoInputInternal->SetEventRepeatable(nArg1, nArg2, nArg3);
}

#ifdef _XBOX
void CExoInput::SetEventNonRepeatable(unsigned long nEvent)
{
    m_pExoInputInternal->SetEventNonRepeatable(nEvent);
}
#endif
// PC 0x0062B7A0
void CExoInput::ClearEventRepeat(unsigned long nArg1)
{
    m_pExoInputInternal->ClearEventRepeat(nArg1);
}
// PC 0x0062B7B0
void CExoInput::CoolDownEvent(unsigned long nArg1, unsigned long nArg2)
{
    m_pExoInputInternal->CoolDownEvent(nArg1, nArg2);
}
// PC 0x0062B7C0
CExoInputEventDesc* CExoInput::GetEvent(unsigned long nEvent, int nEventClass)
{
    return m_pExoInputInternal->GetEvent(nEvent, nEventClass);
}
// PC 0x0062B7D0
float CExoInput::PollInput(unsigned long nEvent, int nEventClass)
{
    return m_pExoInputInternal->PollInput(nEvent, nEventClass);
}
// PC 0x0062B7E0
void CExoInput::ClearEvents()
{
    m_pExoInputInternal->ClearEvents();
}
// PC 0x0062B7F0
void CExoInput::WriteEventToFile(unsigned long nTimeStamp, int nEvent,
                                 CExoFile* pFile)
{
    m_pExoInputInternal->WriteEventToFile(nTimeStamp, nEvent, pFile);
}
// PC 0x0062B800
void CExoInput::Configure()
{
    m_pExoInputInternal->Configure();
}
// PC 0x0062B810
int CExoInput::GetActive()
{
    return m_pExoInputInternal->GetActive();
}
// PC 0x0062B820
void CExoInput::KeyboardModeOn()
{
    m_pExoInputInternal->KeyboardModeOn();
}
// PC 0x0062B830
void CExoInput::KeyboardModeOff()
{
    m_pExoInputInternal->KeyboardModeOff();
}
// PC 0x00638950
int CExoInput::GetKeyboardMode()
{
    return m_pExoInputInternal->m_bKeyboardMode;
}

#ifndef _XBOX
void CExoInput::UpdateJoysticks()  // PC 0x0062B870
{
    m_pExoInputInternal->UpdateJoysticks();
}
#endif

CExoInput::~CExoInput()
{
    if (m_pExoInputInternal)
    {
        delete m_pExoInputInternal;
        m_pExoInputInternal = NULL;
    }
}
// PC 0x0062B840
void CExoInput::SetActive(int bActive)
{
    m_pExoInputInternal->SetActive(bActive);
}
// PC 0x0062B850
int CExoInput::SetRumble(int nArg1, float fArg2, float fArg3,
                         unsigned long nArg4)
{
    return m_pExoInputInternal->SetRumble(nArg1, fArg2, fArg3, nArg4);
}

#ifdef _XBOX
int CExoInput::GetAutoCalInfo()
{
    return m_pExoInputInternal->GetAutoCalInfo();
}

int CExoInput::GetPortBinding()
{
    return m_pExoInputInternal->GetPortBinding();
}

int CExoInput::SetAutoCalInfo(unsigned long nFlags)
{
    return m_pExoInputInternal->SetAutoCalInfo(nFlags);
}

int CExoInput::SetPortBinding(unsigned long nBinding)
{
    return m_pExoInputInternal->SetPortBinding(nBinding);
}
#endif
// PC 0x0062B870
void CExoInput::EndAttemptedBind()
{
    m_pExoInputInternal->EndAttemptedBind();
}
// PC 0x0062B860
int CExoInput::GetMainDeviceLost()
{
    return m_pExoInputInternal->GetMainDeviceLost();
}

#ifdef _XBOX
void CExoInput::UpdateJoysticks()
{
    m_pExoInputInternal->UpdateJoysticks();
}
#endif
// PC 0x0062B880
void CExoInput::PauseRumble()
{
    m_pExoInputInternal->PauseRumble();
}
// PC 0x0062B890
void CExoInput::UnpauseRumble()
{
    m_pExoInputInternal->UnpauseRumble();
}

#ifdef _XBOX
int CExoInput::GetCheatCombo(unsigned char nCombo)
{
    return m_pExoInputInternal->GetCheatCombo(nCombo);
}

int CExoInput::GetDebugOption()
{
    return m_pExoInputInternal->GetDebugOption();
}
#endif

#ifndef _XBOX
void CExoInput::BufferEvent(void* pArg1)  // PC 0x0062B8A0
{
    m_pExoInputInternal->BufferEvent(pArg1);
}
// PC 0x0062B8B0
int CExoInput::SetMouseMode(unsigned char cArg1)
{
    return m_pExoInputInternal->SetMouseMode(cArg1);
}
// PC 0x0062B8C0
void CExoInput::RefreshMouseCursor(int nArg1)
{
    m_pExoInputInternal->RefreshMouseCursor(nArg1);
}
// PC 0x0062B8D0
int CExoInput::SetMouseCursor(int nArg1, int nArg2, int nArg3)
{
    return m_pExoInputInternal->SetMouseCursor(nArg1, nArg2, nArg3);
}
// PC 0x0062B8E0
void CExoInput::SetMousePos(int nX, int nY)
{
    m_pExoInputInternal->SetMousePos(nX, nY);
}
// PC 0x0062B8F0
void CExoInput::HideMouse()
{
    m_pExoInputInternal->HideMouse();
}
// PC 0x0062B900
void CExoInput::ShowMouse()
{
    m_pExoInputInternal->ShowMouse();
}
#endif
// PC 0x0062B910
void CExoInput::ClearAllInputEventDescriptions()
{
    m_pExoInputInternal->ClearAllInputEventDescriptions();
}

#ifndef _XBOX
void CExoInput::GetMouseDelta(float* fArg1, float* fArg2)  // PC 0x0062B920
{
    m_pExoInputInternal->GetMouseDelta(fArg1, fArg2);
}
// PC 0x0062B930
void CExoInput::UpdateMouseDelta(float fArg1)
{
    m_pExoInputInternal->UpdateMouseDelta(fArg1);
}

int CExoInput::IsModalKeyDown_Shift()
{
    return m_pExoInputInternal->IsModalKeyDown_Shift();
}

int CExoInput::IsModalKeyDown_Ctrl()
{
    return m_pExoInputInternal->IsModalKeyDown_Ctrl();
}

int CExoInput::IsModalKeyDown_Alt()
{
    return m_pExoInputInternal->IsModalKeyDown_Alt();
}
// PC 0x0062B940
int CExoInput::GetLastCapturedKeyboardKey()
{
    return m_pExoInputInternal->GetLastCapturedKeyboardKey();
}
// PC 0x0062B950
CExoString CExoInput::GetDisplayCharacterForKey(int nKey)
{
    return m_pExoInputInternal->GetDisplayCharacterForKey(nKey);
}
#endif
// PC 0x0062B9C0
CExoArrayList<CExoInputEvent> CExoInput::GetEvents(int& nEvents, int nEventClass)
{
    return m_pExoInputInternal->GetEvents(nEvents, nEventClass);
}

#ifndef _XBOX
unsigned char CExoInput::GetMouseMode()  // PC 0x0062B9F0
{
    EXOINPUT_TRACE();
    return m_pExoInputInternal->m_nMouseMode;
}
#endif
// PC 0x0062BA00
int CExoInput::EventExists(unsigned long nEvent)
{
    EXOINPUT_TRACE();
    if (m_pExoInputInternal->m_lstEvents.num > (int)nEvent &&
        m_pExoInputInternal->m_lstEvents.element[nEvent] != NULL)
    {
        return TRUE;
    }

    return FALSE;
}

#ifndef _XBOX
void CExoInput::SetCaptureKeyboardKeys(int bCapture)  // PC 0x0062BA30
{
    EXOINPUT_TRACE();
    m_pExoInputInternal->m_bCaptureKeyboardKeys = bCapture;
}
// PC 0x0062BA40
int CExoInput::GetLastSetMouseDownState()
{
    EXOINPUT_TRACE();
    return m_pExoInputInternal->m_nLastSetMouseDownState;
}
#endif

static inline void ExoinputTrace1()
{
}
