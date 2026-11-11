#ifndef EXORAWINPUTINTERNAL_H
#define EXORAWINPUTINTERNAL_H

#ifndef _XBOX
#include <windows.h>
#else
#include <xtl.h>
#endif
#include "exobase.h"
#include "exoinput.h"

#ifndef _XBOX
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#endif

struct CExoDeviceEvent
{
    unsigned long m_nOffset;
    unsigned long m_nData;
    unsigned long m_nTimeStamp;
    unsigned long m_nSequence;
    unsigned long m_nAppData;
};

class CExoDeviceBuffer
{
public:
    CExoDeviceBuffer();
    ~CExoDeviceBuffer();

    unsigned long GetNumEvents() { return m_nEvents; }

    CExoDeviceEvent* m_pEvents;
    unsigned long m_nEvents;
    int m_nCapacity;
};

#ifndef _XBOX
struct CExoMouseState
{
    long m_nX;
    long m_nY;
    long m_nZ;
    unsigned char m_aButtons[4];
};

struct CExoJoystickState
{
    DIJOYSTATE m_State;
    unsigned long m_nTime;
    long m_aAxisRange[8];
};

class CExoRawInputInternal
{
public:

    enum
    {
        JOYSTICK_BUTTON_DOWN_MASK = 0x80,
        JOYSTICK_POV_CENTERED = 0xFFFF
    };

    enum { MAX_INPUT_DEVICES = 6 };

    enum
    {
        HAT_UP = 900,
        HAT_DOWN = 904,
        HAT_RIGHT = 908,
        HAT_LEFT = 912
    };

    CExoRawInputInternal(HINSTANCE hInstance, HWND hWnd);
    ~CExoRawInputInternal();

    int GetJoystickBuffer(int nJoystick, CExoDeviceBuffer& buffer);
    int GetLastState(unsigned long nDevice, unsigned long nControl);
    int GetControlThreshold(unsigned long nControl);
    int IsDigital(unsigned long nDevice, unsigned long nControl);
    float GetMaxUseable(unsigned long nDevice, unsigned long nControl);
    float GetMinUseable(unsigned long nDevice, unsigned long nControl);
    int GetKeyboardBuffer(CExoDeviceBuffer& buffer);
    void KeyboardModeOn();
    void KeyboardModeOff();
    void SetActive(int bActive);
    CExoString GetDisplayCharacterDirectInputValue(unsigned long nKey);
    int ShutDownDirectInput();
    void ShutDownDirectInputMouse();
    int InitializeDirectInputMouse();
    int GetMouseState(CExoMouseState& state);
    void ClearKeyboardBuffer();
    void ClearMouseBuffer();
    void ClearJoystickBuffer(int nJoystick);
    int GetMainDeviceLost();

    int m_bInitialized;
    int m_bActive;
    int m_bKeyboardMode;
    int m_bInputEnabled;
    int m_nLastKey;
    int m_nMouseButtons;
    int m_nJoysticks;
    IDirectInput8* m_pDirectInput;
    IDirectInputDevice8* m_pKeyboard;
    IDirectInputDevice8* m_pMouse;
    IDirectInputDevice8** m_pJoysticks;
    CExoJoystickState* m_pJoystickStates;
    CExoJoystickState* m_pLastJoystickStates;

};
#else

enum
{
    GAMEPAD_ANALOG_BUTTON0 = 200,
    GAMEPAD_ANALOG_BUTTON7 = 207,
    GAMEPAD_DIGITAL_BUTTON0 = 208,
    GAMEPAD_DIGITAL_START = 212,
    GAMEPAD_DIGITAL_BUTTON7 = 215,
    GAMEPAD_THUMB_LX = 216,
    GAMEPAD_THUMB_LY = 217,
    GAMEPAD_THUMB_RX = 218,
    GAMEPAD_THUMB_RY = 219
};

struct CExoGamepadDevice
{
    XINPUT_GAMEPAD m_Gamepad;
    XINPUT_FEEDBACK m_Feedback;
    int m_bLeftMotorOn;
    int m_bRightMotorOn;
    unsigned long m_nLeftDuration;
    unsigned long m_nRightDuration;
    unsigned long m_nLeftStartTime;
    unsigned long m_nRightStartTime;
    unsigned short m_nLeftMotorSpeed;
    unsigned short m_nRightMotorSpeed;
    unsigned char m_bHasLeftMotor;
    unsigned char m_bHasRightMotor;
    HANDLE m_hDevice;
};

struct CExoGamepadState
{
    XINPUT_STATE m_State;
    unsigned long m_nTime;
    unsigned long m_nResetHeldTime;
    float m_fLeftXDeadzone;
    int m_bLeftXCalibrated;
    float m_fLeftYDeadzone;
    int m_bLeftYCalibrated;
    int m_nLeftXMax;
    int m_nLeftXMin;
    int m_nLeftYMax;
    int m_nLeftYMin;
    float m_fRightXDeadzone;
    int m_bRightXCalibrated;
    float m_fRightYDeadzone;
    int m_bRightYCalibrated;
    int m_nRightXMax;
    int m_nRightXMin;
    int m_nRightYMax;
    int m_nRightYMin;
};

class CExoRawInputInternal
{
public:

    enum { MAX_INPUT_DEVICES = 4 };

    CExoRawInputInternal();
    ~CExoRawInputInternal();

    int GetJoystickBuffer(int nJoystick, CExoDeviceBuffer& buffer);
    int GetLastState(unsigned long nDevice, unsigned long nControl);
    int GetControlThreshold(unsigned long nControl);
    int IsDigital(unsigned long nDevice, unsigned long nControl);
    float GetMaxUseable(unsigned long nDevice, unsigned long nControl);
    float GetMinUseable(unsigned long nDevice, unsigned long nControl);
    int GetKeyboardBuffer(CExoDeviceBuffer& buffer);
    int GetMouseBuffer(CExoDeviceBuffer& buffer);
    void KeyboardModeOn();
    void KeyboardModeOff();
    void SetActive(int bActive);
    void ClearKeyboardBuffer();
    void ClearMouseBuffer();
    void ClearJoystickBuffer(int nJoystick);
    int GetMainDeviceLost();

    int GetAutoCalInfo();
    int GetPortBinding();
    int SetAutoCalInfo(unsigned long nFlags);
    int SetPortBinding(unsigned long nBinding);
    void UpdateJoysticks();
    int SetRumble(int nJoystick, float fLeft, float fRight,
                  unsigned long nDuration);
    void UpdateRumble();
    void PauseRumble();
    void UnpauseRumble();
    void ShutDownDirectInput();
    void SetBindEvent(int nPort, int nControl, int nValue, int nSequence,
                      int nTimeStamp);
    int GetBindEvent(int& nControl, int& nValue, int& nSequence,
                     int& nTimeStamp);
    int GetCheatCombo(unsigned char nCombo);
    int GetDebugOption();

    int m_bInitialized;
    int m_bActive;
    int m_bKeyboardMode;
    int m_bRumbleEnabled;
    int m_bRumblePaused;
    unsigned long m_nRumblePauseTime;
    int m_bAttemptingBind;
    int m_bBindEvent;
    int m_bBindEnded;
    int m_bMainDeviceLost;
    int m_nBindControl;
    int m_nBindValue;
    int m_nBindSequence;
    int m_nBindTimeStamp;
    int m_nJoysticks;
    CExoGamepadDevice* m_pDevices;
    CExoGamepadDevice m_aDevices[4];
    CExoGamepadState* m_pStates;
    CExoGamepadState m_aStates[4];
    CExoGamepadState* m_pLastStates;
    CExoGamepadState m_aLastStates[4];
    int m_aPorts[4];
    int m_aRestorePorts[4];
    unsigned long m_nRestoreElapsed;
    unsigned long m_nRestoreTime;
    int m_bRestorePorts;
    int m_nCheatCombo;
    unsigned long m_nDebugOption;
};

extern float g_fDeadzonePreCalibration;
extern float g_fDeadzonePostCalibration;
extern float g_fDeadzoneCalibrationDetect;

#endif

#endif
