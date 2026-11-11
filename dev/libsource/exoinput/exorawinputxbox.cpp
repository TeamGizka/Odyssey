#include "exorawinputinternal.h"
#include <stdlib.h>
#include "appmanager.h"
#include "clientexoapp.h"
#include "swmain.h"

#include "clientoptions.h"
#include "LaunchData.h"

void AurPersistDisplay();
void ExoXboxLaunch(KOTOR_LAUNCH_DATA* pParams);

float g_fDeadzonePreCalibration = 0.5f;
float g_fDeadzonePostCalibration = 0.2f;
float g_fDeadzoneCalibrationDetect = 0.95f;
// PC 0x005C5A30
CExoDeviceBuffer::CExoDeviceBuffer()
{
    m_nEvents = 0;
    m_nCapacity = 0;
    m_pEvents = NULL;
}
// PC 0x005E2FF0  Xbox 0x0013BFC0
CExoDeviceBuffer::~CExoDeviceBuffer()
{
    if (m_pEvents != NULL)
    {
        delete m_pEvents;
        m_pEvents = NULL;
    }
}
// PC 0x005E3BE0
void CExoRawInputInternal::KeyboardModeOn()
{
}
// PC 0x005E3C00
void CExoRawInputInternal::KeyboardModeOff()
{
}
// PC 0x005E3010
void CExoRawInputInternal::ClearKeyboardBuffer()
{
}
// PC 0x005E3080
void CExoRawInputInternal::ClearMouseBuffer()
{
}
// PC 0x005E3AE0
int CExoRawInputInternal::GetKeyboardBuffer(CExoDeviceBuffer& buffer)
{
    return 1;
}

int CExoRawInputInternal::GetMouseBuffer(CExoDeviceBuffer& buffer)
{
    return 1;
}
// PC 0x0063E7F0
int CExoRawInputInternal::GetMainDeviceLost()
{
    return m_bMainDeviceLost;
}
// Xbox 0x0013BFE0
void CExoRawInputInternal::ClearJoystickBuffer(int nJoystick)
{
    unsigned long nTime =
        (unsigned long)(g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000);

    memset(&m_pStates[nJoystick].m_State, 0, sizeof(XINPUT_STATE));
    m_pStates[nJoystick].m_nTime = nTime;
    memset(&m_pLastStates[nJoystick].m_State, 0, sizeof(XINPUT_STATE));
    m_pLastStates[nJoystick].m_nTime = nTime;
    m_pStates[nJoystick].m_nResetHeldTime = 0;
    m_pLastStates[nJoystick].m_nResetHeldTime = 0;

    m_pStates[nJoystick].m_fLeftXDeadzone = g_fDeadzonePreCalibration;
    m_pStates[nJoystick].m_bLeftXCalibrated = 0;
    m_pStates[nJoystick].m_fLeftYDeadzone = g_fDeadzonePreCalibration;
    m_pStates[nJoystick].m_bLeftYCalibrated = 0;
    m_pStates[nJoystick].m_nLeftXMax = 0;
    m_pStates[nJoystick].m_nLeftXMin = 0;
    m_pStates[nJoystick].m_nLeftYMax = 0;
    m_pStates[nJoystick].m_nLeftYMin = 0;
    m_pStates[nJoystick].m_fRightXDeadzone = g_fDeadzonePreCalibration;
    m_pStates[nJoystick].m_bRightXCalibrated = 0;
    m_pStates[nJoystick].m_fRightYDeadzone = g_fDeadzonePreCalibration;
    m_pStates[nJoystick].m_bRightYCalibrated = 0;
    m_pStates[nJoystick].m_nRightXMax = 0;
    m_pStates[nJoystick].m_nRightXMin = 0;
    m_pStates[nJoystick].m_nRightYMax = 0;
    m_pStates[nJoystick].m_nRightYMin = 0;
}
// Xbox 0x0013C140
int CExoRawInputInternal::GetAutoCalInfo()
{
    int nFlags = 0;

    if (m_pStates[0].m_bLeftXCalibrated)
        nFlags |= 0x0008;
    if (m_pStates[0].m_bLeftYCalibrated)
        nFlags |= 0x0004;
    if (m_pStates[0].m_bRightXCalibrated)
        nFlags |= 0x0002;
    if (m_pStates[0].m_bRightYCalibrated)
        nFlags |= 0x0001;
    if (m_pStates[1].m_bLeftXCalibrated)
        nFlags |= 0x0080;
    if (m_pStates[1].m_bLeftYCalibrated)
        nFlags |= 0x0040;
    if (m_pStates[1].m_bRightXCalibrated)
        nFlags |= 0x0020;
    if (m_pStates[1].m_bRightYCalibrated)
        nFlags |= 0x0010;
    if (m_pStates[2].m_bLeftXCalibrated)
        nFlags |= 0x0800;
    if (m_pStates[2].m_bLeftYCalibrated)
        nFlags |= 0x0400;
    if (m_pStates[2].m_bRightXCalibrated)
        nFlags |= 0x0200;
    if (m_pStates[2].m_bRightYCalibrated)
        nFlags |= 0x0100;
    if (m_pStates[3].m_bLeftXCalibrated)
        nFlags |= 0x8000;
    if (m_pStates[3].m_bLeftYCalibrated)
        nFlags |= 0x4000;
    if (m_pStates[3].m_bRightXCalibrated)
        nFlags |= 0x2000;
    if (m_pStates[3].m_bRightYCalibrated)
        nFlags |= 0x1000;

    return nFlags;
}
// Xbox 0x0013C230
int CExoRawInputInternal::GetPortBinding()
{
    int nAssignment = 0;

    for (int nSlot = 0; nSlot < 4; nSlot++)
    {
        int nPort = m_aPorts[nSlot];
        int nNibble = 15;
        if (nPort != -1)
            nNibble = nPort;
        nAssignment |= nNibble << (nSlot * 4);
    }
    nAssignment |= m_nDebugOption << 30;
    return nAssignment;
}
// Xbox 0x0013C2A0
int CExoRawInputInternal::SetAutoCalInfo(unsigned long nFlags)
{
    m_pStates[0].m_bLeftXCalibrated = (nFlags >> 3) & 1;
    m_pStates[0].m_bLeftYCalibrated = (nFlags >> 2) & 1;
    m_pStates[0].m_bRightXCalibrated = (nFlags >> 1) & 1;
    m_pStates[0].m_bRightYCalibrated = nFlags & 1;
    if (m_pStates[0].m_bLeftXCalibrated)
        m_pStates[0].m_fLeftXDeadzone = g_fDeadzonePostCalibration;
    if (m_pStates[0].m_bLeftYCalibrated)
        m_pStates[0].m_fLeftYDeadzone = g_fDeadzonePostCalibration;
    if (m_pStates[0].m_bRightXCalibrated)
        m_pStates[0].m_fRightXDeadzone = g_fDeadzonePostCalibration;
    if (m_pStates[0].m_bRightYCalibrated)
        m_pStates[0].m_fRightYDeadzone = g_fDeadzonePostCalibration;

    m_pStates[1].m_bLeftXCalibrated = (nFlags >> 7) & 1;
    m_pStates[1].m_bLeftYCalibrated = (nFlags >> 6) & 1;
    m_pStates[1].m_bRightXCalibrated = (nFlags >> 5) & 1;
    m_pStates[1].m_bRightYCalibrated = (nFlags >> 4) & 1;
    if (m_pStates[1].m_bLeftXCalibrated)
        m_pStates[1].m_fLeftXDeadzone = g_fDeadzonePostCalibration;
    if (m_pStates[1].m_bLeftYCalibrated)
        m_pStates[1].m_fLeftYDeadzone = g_fDeadzonePostCalibration;
    if (m_pStates[1].m_bRightXCalibrated)
        m_pStates[1].m_fRightXDeadzone = g_fDeadzonePostCalibration;
    if (m_pStates[1].m_bRightYCalibrated)
        m_pStates[1].m_fRightYDeadzone = g_fDeadzonePostCalibration;

    m_pStates[2].m_bLeftXCalibrated = (nFlags >> 11) & 1;
    m_pStates[2].m_bLeftYCalibrated = (nFlags >> 10) & 1;
    m_pStates[2].m_bRightXCalibrated = (nFlags >> 9) & 1;
    m_pStates[2].m_bRightYCalibrated = (nFlags >> 8) & 1;
    if (m_pStates[2].m_bLeftXCalibrated)
        m_pStates[2].m_fLeftXDeadzone = g_fDeadzonePostCalibration;
    if (m_pStates[2].m_bLeftYCalibrated)
        m_pStates[2].m_fLeftYDeadzone = g_fDeadzonePostCalibration;
    if (m_pStates[2].m_bRightXCalibrated)
        m_pStates[2].m_fRightXDeadzone = g_fDeadzonePostCalibration;
    if (m_pStates[2].m_bRightYCalibrated)
        m_pStates[2].m_fRightYDeadzone = g_fDeadzonePostCalibration;

    m_pStates[3].m_bLeftXCalibrated = (nFlags >> 15) & 1;
    m_pStates[3].m_bLeftYCalibrated = (nFlags >> 14) & 1;
    m_pStates[3].m_bRightXCalibrated = (nFlags >> 13) & 1;
    m_pStates[3].m_bRightYCalibrated = (nFlags >> 12) & 1;
    if (m_pStates[3].m_bLeftXCalibrated)
        m_pStates[3].m_fLeftXDeadzone = g_fDeadzonePostCalibration;
    if (m_pStates[3].m_bLeftYCalibrated)
        m_pStates[3].m_fLeftYDeadzone = g_fDeadzonePostCalibration;
    if (m_pStates[3].m_bRightXCalibrated)
        m_pStates[3].m_fRightXDeadzone = g_fDeadzonePostCalibration;
    if (m_pStates[3].m_bRightYCalibrated)
        m_pStates[3].m_fRightYDeadzone = g_fDeadzonePostCalibration;

    return 1;
}
// PC 0x005E3930  Xbox 0x0013C5D0
int CExoRawInputInternal::GetLastState(unsigned long nDevice,
                                       unsigned long nControl)
{
    int nPort = m_aPorts[nDevice];
    if (nPort < 0 || nPort >= 4)
    {
        return 0;
    }

    CExoGamepadState* pState = &m_pLastStates[nPort];
    if (nControl >= GAMEPAD_ANALOG_BUTTON0 && nControl <= GAMEPAD_ANALOG_BUTTON7)
    {
        return pState->m_State.Gamepad.bAnalogButtons[nControl -
                                                      GAMEPAD_ANALOG_BUTTON0];
    }
    if (nControl >= GAMEPAD_DIGITAL_BUTTON0 &&
        nControl <= GAMEPAD_DIGITAL_BUTTON7)
    {
        return 0;
    }
    if (nControl >= GAMEPAD_THUMB_LX && nControl <= GAMEPAD_THUMB_RY)
    {
        int nValue = 0;
        switch (nControl)
        {
        case GAMEPAD_THUMB_LX:
            nValue = pState->m_State.Gamepad.sThumbLX;
            break;
        case GAMEPAD_THUMB_LY:
            nValue = pState->m_State.Gamepad.sThumbLY;
            break;
        case GAMEPAD_THUMB_RX:
            nValue = pState->m_State.Gamepad.sThumbRX;
            break;
        case GAMEPAD_THUMB_RY:
            nValue = pState->m_State.Gamepad.sThumbRY;
            break;
        }
        return nValue;
    }
    return 0;
}
// PC 0x005E3A60  Xbox 0x0013C680
float CExoRawInputInternal::GetMaxUseable(unsigned long nDevice,
                                          unsigned long nControl)
{
    if (nControl >= GAMEPAD_DIGITAL_BUTTON0 &&
        nControl <= GAMEPAD_DIGITAL_BUTTON7)
    {
        return 0.0f;
    }
    if (nControl >= GAMEPAD_ANALOG_BUTTON0 && nControl <= GAMEPAD_ANALOG_BUTTON7)
    {
        return 255.0f;
    }
    if (nControl >= GAMEPAD_THUMB_LX && nControl <= GAMEPAD_THUMB_RY)
    {
        return 32767.0f;
    }
    return 0.0f;
}
// PC 0x005E3AA0  Xbox 0x0013C6D0
float CExoRawInputInternal::GetMinUseable(unsigned long nDevice,
                                          unsigned long nControl)
{
    int nPort = m_aPorts[nDevice];

    if (nControl >= GAMEPAD_DIGITAL_BUTTON0 &&
        nControl <= GAMEPAD_DIGITAL_BUTTON7)
    {
        return 0.0f;
    }
    if (nControl >= GAMEPAD_ANALOG_BUTTON0 && nControl <= GAMEPAD_ANALOG_BUTTON7)
    {
        return 55.0f;
    }
    if (nControl >= GAMEPAD_THUMB_LX && nControl <= GAMEPAD_THUMB_RY)
    {
        CExoGamepadState* pState = &m_pStates[nPort];
        switch (nControl)
        {
        case GAMEPAD_THUMB_LX:
            return pState->m_fLeftXDeadzone * 32767.0f;
        case GAMEPAD_THUMB_LY:
            return pState->m_fLeftYDeadzone * 32767.0f;
        case GAMEPAD_THUMB_RX:
            return pState->m_fRightXDeadzone * 32767.0f;
        case GAMEPAD_THUMB_RY:
            return pState->m_fRightYDeadzone * 32767.0f;
        }
    }
    return 0.0f;
}
// PC 0x005E39F0  Xbox 0x0013C790
int CExoRawInputInternal::IsDigital(unsigned long nDevice,
                                    unsigned long nControl)
{
    if (nControl >= GAMEPAD_DIGITAL_BUTTON0 &&
        nControl <= GAMEPAD_DIGITAL_BUTTON7)
    {
        return 1;
    }
    return 0;
}
// PC 0x0063E7C0  Xbox 0x0013C7B0
int CExoRawInputInternal::GetControlThreshold(unsigned long nControl)
{
    if (nControl >= GAMEPAD_THUMB_LX && nControl <= GAMEPAD_THUMB_RY)
    {
        return 15000;
    }
    return 0;
}
// Xbox 0x0013C7D0
void CExoRawInputInternal::UpdateJoysticks()
{
    DWORD dwInsertions;
    DWORD dwRemovals;

    if (XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &dwInsertions, &dwRemovals))
    {
        if (dwRemovals != 0)
        {
            for (int nPort = 0; nPort < 4; nPort++)
            {
                if ((dwRemovals & (1 << nPort)) &&
                    m_pDevices[nPort].m_hDevice != NULL)
                {
                    XInputClose(m_pDevices[nPort].m_hDevice);
                    m_pDevices[nPort].m_hDevice = NULL;

                    int nJoystick = -1;
                    for (int i = 0; i < 4 && nJoystick == -1; i++)
                    {
                        if (nPort == m_aPorts[i])
                        {
                            nJoystick = i;
                        }
                    }
                    if (nJoystick == -1)
                    {
                        return;
                    }

                    if (nJoystick == 0 && !m_bAttemptingBind)
                    {
                        m_bAttemptingBind = 1;
                        m_bMainDeviceLost = 1;
                    }

                    for (int j = nJoystick; j + 1 != m_nJoysticks; j++)
                    {
                        int nSwap = m_aPorts[j + 1];
                        m_aPorts[j + 1] = m_aPorts[j];
                        m_aPorts[j] = nSwap;
                    }
                    m_nJoysticks--;
                    m_aPorts[m_nJoysticks] = -1;

                    ClearJoystickBuffer(nPort);
                    m_pDevices[nPort].m_bLeftMotorOn = 0;
                    m_pDevices[nPort].m_nLeftDuration = 0;
                    m_pDevices[nPort].m_nLeftStartTime = 0;
                    m_pDevices[nPort].m_nLeftMotorSpeed = 0;
                    m_pDevices[nPort].m_bRightMotorOn = 0;
                    m_pDevices[nPort].m_nRightDuration = 0;
                    m_pDevices[nPort].m_nRightStartTime = 0;
                    m_pDevices[nPort].m_nRightMotorSpeed = 0;
                    m_pDevices[nPort].m_bHasLeftMotor = 0;
                    m_pDevices[nPort].m_bHasRightMotor = 0;
                    m_pDevices[nPort].m_Feedback.Rumble.wLeftMotorSpeed = 0;
                    m_pDevices[nPort].m_Feedback.Rumble.wRightMotorSpeed = 0;

                    if (m_nJoysticks < 0)
                    {
                        m_nJoysticks = 0;
                        return;
                    }
                }
            }
        }

        if (dwInsertions != 0)
        {
            for (int nPort = 0; nPort < 4; nPort++)
            {
                if ((dwInsertions & (1 << nPort)) &&
                    m_pDevices[nPort].m_hDevice == NULL)
                {
                    XINPUT_CAPABILITIES caps;

                    m_pDevices[nPort].m_hDevice = XInputOpen(
                        XDEVICE_TYPE_GAMEPAD, nPort, XDEVICE_NO_SLOT, NULL);
                    if (XInputGetCapabilities(m_pDevices[nPort].m_hDevice,
                                              &caps) == ERROR_SUCCESS)
                    {
                        if (caps.SubType != XINPUT_DEVSUBTYPE_GC_GAMEPAD &&
                            caps.SubType != XINPUT_DEVSUBTYPE_GC_GAMEPAD_ALT &&
                            caps.SubType != XINPUT_DEVSUBTYPE_GC_ARCADE_STICK &&
                            caps.SubType != XINPUT_DEVSUBTYPE_GC_DIGITAL_ARCADE_STICK &&
                            caps.SubType != XINPUT_DEVSUBTYPE_GC_FISHING_ROD &&
                            caps.SubType != XINPUT_DEVSUBTYPE_GC_FLIGHT_STICK &&
                            caps.SubType != XINPUT_DEVSUBTYPE_GC_LIGHTGUN &&
                            caps.SubType != XINPUT_DEVSUBTYPE_GC_RADIO_FLIGHT_CONTROL &&
                            caps.SubType != XINPUT_DEVSUBTYPE_GC_SNOWBOARD &&
                            caps.SubType != XINPUT_DEVSUBTYPE_GC_WHEEL)
                        {
                            XInputClose(m_pDevices[nPort].m_hDevice);
                            m_pDevices[nPort].m_hDevice = NULL;
                        }
                        else
                        {
                            m_aPorts[m_nJoysticks] = nPort;
                            m_nJoysticks++;

                            m_pDevices[nPort].m_bLeftMotorOn = 0;
                            m_pDevices[nPort].m_nLeftDuration = 0;
                            m_pDevices[nPort].m_nLeftStartTime = 0;
                            m_pDevices[nPort].m_nLeftMotorSpeed = 0;
                            m_pDevices[nPort].m_bRightMotorOn = 0;
                            m_pDevices[nPort].m_nRightDuration = 0;
                            m_pDevices[nPort].m_nRightStartTime = 0;
                            m_pDevices[nPort].m_nRightMotorSpeed = 0;
                            m_pDevices[nPort].m_bHasLeftMotor =
                                caps.Out.Rumble.wLeftMotorSpeed > 0;
                            m_pDevices[nPort].m_bHasRightMotor =
                                caps.Out.Rumble.wRightMotorSpeed > 0;
                            m_pDevices[nPort].m_Feedback.Rumble.wLeftMotorSpeed = 0;
                            m_pDevices[nPort].m_Feedback.Rumble.wRightMotorSpeed = 0;
                            XInputSetState(m_pDevices[nPort].m_hDevice,
                                           &m_pDevices[nPort].m_Feedback);

                            if (m_nJoysticks > 4)
                            {
                                m_nJoysticks = 3;
                                return;
                            }
                        }
                    }
                    else
                    {
                        XInputClose(m_pDevices[nPort].m_hDevice);
                        m_pDevices[nPort].m_hDevice = NULL;
                    }
                }
            }
        }
    }

    if (m_bRestorePorts)
    {
        unsigned long nTime = (unsigned long)(
            g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000);
        unsigned long nElapsed = nTime - m_nRestoreTime;
        m_nRestoreTime = nTime;
        m_nRestoreElapsed += nElapsed;

        if (!m_bBindEnded)
        {
            int i;
            for (i = 0; i < 4; i++)
            {
                int nPort = m_aRestorePorts[i];
                if (nPort == -1)
                {
                    if (i == 0)
                    {
                        m_bRestorePorts = 0;
                        m_bBindEnded = 1;
                        m_bAttemptingBind = 1;
                        m_bMainDeviceLost = 1;
                        return;
                    }
                    break;
                }
                if (nPort >= 0 && nPort < 4 &&
                    m_pDevices[nPort].m_hDevice == NULL)
                {
                    if (m_nRestoreElapsed < 400)
                    {
                        m_bRestorePorts = 0;
                        m_bBindEnded = 1;
                        m_bAttemptingBind = 1;
                        m_bMainDeviceLost = 1;
                    }
                    return;
                }
            }

            m_bRestorePorts = 0;
            m_bAttemptingBind = 0;
            m_bBindEnded = 1;

            int aPorts[4];
            for (i = 0; i < 4; i++)
            {
                int nPort = m_aRestorePorts[i];
                int j;
                if (nPort != -1)
                {
                    aPorts[i] = nPort;
                    for (j = 0; j < 4; j++)
                    {
                        if (m_aPorts[j] == nPort)
                        {
                            m_aPorts[j] = -1;
                            break;
                        }
                    }
                }
                else
                {
                    int nFree = -1;
                    for (j = 0; j < 4; j++)
                    {
                        if (m_aPorts[j] != -1)
                        {
                            nFree = m_aPorts[j];
                            m_aPorts[j] = -1;
                            break;
                        }
                    }
                    aPorts[i] = nFree;
                }
            }
            for (i = 0; i < 4; i++)
            {
                m_aPorts[i] = aPorts[i];
            }
        }
        else
        {
            m_bRestorePorts = 0;
            m_bMainDeviceLost = 1;
        }
    }

    m_nCheatCombo = 0;
}
// Xbox 0x0013CC60
int CExoRawInputInternal::SetRumble(int nJoystick, float fLeft, float fRight,
                                    unsigned long nDuration)
{
    if (!m_bRumbleEnabled || m_bRumblePaused || m_bMainDeviceLost)
    {
        return 1;
    }
    if (nJoystick >= 0 && nJoystick < m_nJoysticks && fLeft >= 0.0f &&
        fLeft <= 1.0f && fRight >= 0.0f && fRight <= 1.0f &&
        nDuration < 10000000)
    {
        int nPort = m_aPorts[nJoystick];

        if (fLeft == 0.0f || !m_pDevices[nPort].m_bHasLeftMotor)
        {
            m_pDevices[nPort].m_bLeftMotorOn = 0;
            m_pDevices[nPort].m_nLeftDuration = 0;
            m_pDevices[nPort].m_nLeftStartTime = 0;
            m_pDevices[nPort].m_nLeftMotorSpeed = 0;
        }
        else
        {
            float fSpeed = fLeft * 65535.0f;
            if (fSpeed < 8000.0f)
            {
                m_pDevices[nPort].m_nLeftMotorSpeed = 8000;
                m_pDevices[nPort].m_Feedback.Rumble.wLeftMotorSpeed = 18000;
            }
            else
            {
                double fValue = fSpeed;
                if (fSpeed < 18000.0f && !m_pDevices[nPort].m_bLeftMotorOn)
                {
                    m_pDevices[nPort].m_nLeftMotorSpeed = (unsigned short)fValue;
                    m_pDevices[nPort].m_Feedback.Rumble.wLeftMotorSpeed = 18000;
                }
                else
                {
                    unsigned short nSpeed = (unsigned short)fValue;
                    m_pDevices[nPort].m_nLeftMotorSpeed = nSpeed;
                    m_pDevices[nPort].m_Feedback.Rumble.wLeftMotorSpeed = nSpeed;
                }
            }
            m_pDevices[nPort].m_bLeftMotorOn = 1;
            m_pDevices[nPort].m_nLeftDuration = nDuration;
            m_pDevices[nPort].m_nLeftStartTime = (unsigned long)(
                g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000);
        }

        if (fRight == 0.0f || !m_pDevices[nPort].m_bHasRightMotor)
        {
            m_pDevices[nPort].m_bRightMotorOn = 0;
            m_pDevices[nPort].m_nRightDuration = 0;
            m_pDevices[nPort].m_nRightStartTime = 0;
            m_pDevices[nPort].m_nRightMotorSpeed = 0;
        }
        else
        {
            fRight *= 65535.0f;
            if (fRight < 8500.0f)
            {
                m_pDevices[nPort].m_nRightMotorSpeed = 8500;
                m_pDevices[nPort].m_Feedback.Rumble.wRightMotorSpeed = 14500;
            }
            else
            {
                double fValue = fRight;
                if (fRight < 14500.0f && !m_pDevices[nPort].m_bRightMotorOn)
                {
                    m_pDevices[nPort].m_nRightMotorSpeed = (unsigned short)fValue;
                    m_pDevices[nPort].m_Feedback.Rumble.wRightMotorSpeed = 14500;
                }
                else
                {
                    unsigned short nSpeed = (unsigned short)fValue;
                    m_pDevices[nPort].m_nRightMotorSpeed = nSpeed;
                    m_pDevices[nPort].m_Feedback.Rumble.wRightMotorSpeed = nSpeed;
                }
            }
            m_pDevices[nPort].m_bRightMotorOn = 1;
            m_pDevices[nPort].m_nRightDuration = nDuration;
            m_pDevices[nPort].m_nRightStartTime = (unsigned long)(
                g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000);
        }

        XInputSetState(m_pDevices[nPort].m_hDevice, &m_pDevices[nPort].m_Feedback);
        return 1;
    }
    return 0;
}
// Xbox 0x0013CF00
void CExoRawInputInternal::UpdateRumble()
{
    unsigned long nTime =
        (unsigned long)(g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000);

    if (!m_bRumblePaused)
    {
        for (int i = 0; i < m_nJoysticks; i++)
        {
            int nPort = m_aPorts[i];

            if (m_pDevices[nPort].m_bLeftMotorOn &&
                m_pDevices[nPort].m_nLeftDuration <
                    nTime - m_pDevices[nPort].m_nLeftStartTime &&
                m_pDevices[nPort].m_nLeftDuration != -1)
            {
                m_pDevices[nPort].m_bLeftMotorOn = 0;
                m_pDevices[nPort].m_nLeftDuration = 0;
                m_pDevices[nPort].m_nLeftStartTime = 0;
                m_pDevices[nPort].m_nLeftMotorSpeed = 0;
            }
            if (m_pDevices[nPort].m_bRightMotorOn &&
                m_pDevices[nPort].m_nRightDuration <
                    nTime - m_pDevices[nPort].m_nRightStartTime &&
                m_pDevices[nPort].m_nRightDuration != -1)
            {
                m_pDevices[nPort].m_bRightMotorOn = 0;
                m_pDevices[nPort].m_nRightDuration = 0;
                m_pDevices[nPort].m_nRightStartTime = 0;
                m_pDevices[nPort].m_nRightMotorSpeed = 0;
            }

            m_pDevices[nPort].m_Feedback.Rumble.wLeftMotorSpeed =
                m_pDevices[nPort].m_nLeftMotorSpeed;
            m_pDevices[nPort].m_Feedback.Rumble.wRightMotorSpeed =
                m_pDevices[nPort].m_nRightMotorSpeed;
            if (m_pDevices[nPort].m_Feedback.Header.dwStatus != ERROR_IO_PENDING)
            {
                XInputSetState(m_pDevices[nPort].m_hDevice,
                               &m_pDevices[nPort].m_Feedback);
            }
        }
    }
}
// PC 0x005E3C20  Xbox 0x0013D030
void CExoRawInputInternal::SetActive(int bActive)
{
    if (bActive != m_bActive)
    {
        m_bActive = bActive;
    }
}
// Xbox 0x0013D040
void CExoRawInputInternal::ShutDownDirectInput()
{
    if (m_pStates != NULL)
    {
        m_pStates = NULL;
    }
    if (m_pLastStates != NULL)
    {
        m_pLastStates = NULL;
    }
    if (m_pDevices != NULL)
    {
        for (int i = 0; i < m_nJoysticks; i++)
        {
            if (m_pDevices[i].m_hDevice != NULL)
            {
                XInputClose(m_pDevices[i].m_hDevice);
                m_pDevices[i].m_hDevice = NULL;
            }
        }
    }
}
// Xbox 0x0013D0A0
void CExoRawInputInternal::SetBindEvent(int nPort, int nControl, int nValue,
                                        int nSequence, int nTimeStamp)
{
    if (m_bBindEvent)
    {
        return;
    }

    m_nBindControl = nControl;
    m_nBindValue = nValue;
    m_nBindSequence = nSequence;
    m_bAttemptingBind = 0;
    m_nBindTimeStamp = nTimeStamp;
    m_bMainDeviceLost = 0;
    m_bBindEvent = 1;

    if (m_aPorts[0] != nPort)
    {
        for (int i = 0; i < 4; i++)
        {
            if (m_aPorts[i] == nPort)
            {
                m_aPorts[i] = m_aPorts[0];
                m_aPorts[0] = nPort;
            }
        }
    }
}
// Xbox 0x0013D150
int CExoRawInputInternal::GetBindEvent(int& nControl, int& nValue,
                                       int& nSequence, int& nTimeStamp)
{
    int bBindEvent = m_bBindEvent;
    m_bBindEvent = 0;
    nControl = m_nBindControl;
    nValue = m_nBindValue;
    nSequence = m_nBindSequence;
    nTimeStamp = m_nBindTimeStamp;
    return bBindEvent;
}
// Xbox 0x0013D190
void CExoRawInputInternal::PauseRumble()
{
    if (!m_bRumblePaused)
    {
        m_nRumblePauseTime = (unsigned long)(
            g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000);
        for (int i = 0; i < m_nJoysticks; i++)
        {
            int nPort = m_aPorts[i];
            if (m_pDevices[nPort].m_hDevice != NULL)
            {
                m_pDevices[nPort].m_Feedback.Rumble.wLeftMotorSpeed = 0;
                m_pDevices[nPort].m_Feedback.Rumble.wRightMotorSpeed = 0;
                XInputSetState(m_pDevices[nPort].m_hDevice,
                               &m_pDevices[nPort].m_Feedback);
            }
        }
        m_bRumblePaused = 1;
    }
}
// Xbox 0x0013D220
void CExoRawInputInternal::UnpauseRumble()
{
    if (m_bRumblePaused)
    {
        m_bRumblePaused = 0;
        unsigned long nTime = (unsigned long)(
            g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000);
        for (int i = 0; i < m_nJoysticks; i++)
        {
            int nPort = m_aPorts[i];
            if (m_pDevices[nPort].m_hDevice != NULL)
            {
                float fLeft = m_pDevices[nPort].m_nLeftMotorSpeed * (1.0f / 65535.0f);
                float fRight = m_pDevices[nPort].m_nRightMotorSpeed * (1.0f / 65535.0f);
                unsigned long nElapsed = 0;
                unsigned long nDuration = 0;
                if (m_pDevices[nPort].m_nLeftDuration > 0)
                {
                    nElapsed = m_nRumblePauseTime - m_pDevices[nPort].m_nLeftStartTime;
                    nDuration = m_pDevices[nPort].m_nLeftDuration;
                }
                if (m_pDevices[nPort].m_nRightDuration > 0)
                {
                    nElapsed = m_nRumblePauseTime - m_pDevices[nPort].m_nRightStartTime;
                    nDuration = m_pDevices[nPort].m_nRightDuration;
                }
                SetRumble(i, fLeft, fRight, nDuration - nElapsed);
            }
        }
    }
}
// Xbox 0x0013D2F0
int CExoRawInputInternal::GetCheatCombo(unsigned char nCombo)
{
    if (nCombo > 1)
    {
        return 0;
    }
    return (m_nCheatCombo & (1 << nCombo)) != 0;
}
// Xbox 0x0013D320
int CExoRawInputInternal::GetDebugOption()
{
    return m_nDebugOption;
}
// Xbox 0x0013D330
CExoRawInputInternal::CExoRawInputInternal()
{
    m_bInitialized = 0;
    m_bActive = 0;
    m_bKeyboardMode = 0;
    m_bAttemptingBind = 1;
    m_bBindEvent = 0;
    m_bMainDeviceLost = 0;
    m_nBindControl = -1;
    m_nBindValue = -1;
    m_nBindSequence = 0;
    m_nBindTimeStamp = 0;
    m_bRestorePorts = 0;
    m_bBindEnded = 1;
    m_nRestoreElapsed = 0;
    m_nRestoreTime = (unsigned long)(
        g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000);
    m_bRumbleEnabled = 1;
    m_bRumblePaused = 0;
    m_nRumblePauseTime = 0;
    m_pDevices = m_aDevices;
    m_pStates = m_aStates;
    m_pLastStates = m_aLastStates;

    XInitDevices(0, NULL);

    for (int i = 0; i < 4; i++)
    {
        m_aPorts[i] = -1;
        m_aRestorePorts[i] = -1;
    }
    m_nJoysticks = 0;

    for (int nPort = 0; nPort < 4; nPort++)
    {
        ClearJoystickBuffer(nPort);
        memset(&m_pDevices[nPort], 0, sizeof(CExoGamepadDevice));
        m_pDevices[nPort].m_bLeftMotorOn = 0;
        m_pDevices[nPort].m_nLeftDuration = 0;
        m_pDevices[nPort].m_nLeftStartTime = 0;
        m_pDevices[nPort].m_nLeftMotorSpeed = 0;
        m_pDevices[nPort].m_bRightMotorOn = 0;
        m_pDevices[nPort].m_nRightDuration = 0;
        m_pDevices[nPort].m_nRightStartTime = 0;
        m_pDevices[nPort].m_nRightMotorSpeed = 0;
        m_pDevices[nPort].m_bHasLeftMotor = 0;
        m_pDevices[nPort].m_bHasRightMotor = 0;
        m_pDevices[nPort].m_hDevice = NULL;
    }

    m_bInitialized = 1;
    UpdateJoysticks();
    m_nDebugOption = 0;
}
// PC 0x005E3F90  Xbox 0x0013D490
CExoRawInputInternal::~CExoRawInputInternal()
{
    ShutDownDirectInput();
}
// Xbox 0x0013D4A0
int CExoRawInputInternal::SetPortBinding(unsigned long nBinding)
{
    m_bRestorePorts = 1;
    m_bBindEnded = 0;
    m_nRestoreTime = (unsigned long)(
        g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000);

    int nPorts = (int)nBinding;
    for (int nSlot = 0; nSlot < 4; nSlot++)
    {
        m_aRestorePorts[nSlot] = ((nPorts >> (nSlot * 4)) & 15) == 15 ? -1 : (nPorts >> (nSlot * 4)) & 15;
    }
    m_nDebugOption = nBinding >> 30;

    UpdateJoysticks();
    return 0;
}
// PC 0x005E30E0  Xbox 0x0013D550
int CExoRawInputInternal::GetJoystickBuffer(int nJoystick,
                                            CExoDeviceBuffer& buffer)
{
    XINPUT_STATE state;
    int nPort = m_aPorts[nJoystick];

    buffer.m_nEvents = 0;
    if (buffer.m_pEvents == NULL)
    {
        buffer.m_pEvents = new CExoDeviceEvent[256];
        if (buffer.m_pEvents == NULL)
        {
            return 0;
        }
    }
    memset(buffer.m_pEvents, 0, 256);

    if (m_pDevices[nPort].m_hDevice == NULL || !m_bActive ||
        XInputGetState(m_pDevices[nPort].m_hDevice, &state) != ERROR_SUCCESS)
    {
        return 0;
    }

    CExoGamepadState* pState = &m_pStates[nPort];
    unsigned long nTime =
        (unsigned long)(g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000);

    for (unsigned char nAnalog = 0; nAnalog < 8; nAnalog++)
    {
        if (state.Gamepad.bAnalogButtons[nAnalog] <= 55)
        {
            state.Gamepad.bAnalogButtons[nAnalog] = 0;
        }
        if (state.Gamepad.bAnalogButtons[nAnalog] !=
            pState->m_State.Gamepad.bAnalogButtons[nAnalog])
        {
            if (m_bAttemptingBind)
            {
                if (nAnalog == XINPUT_GAMEPAD_A)
                {
                    SetBindEvent(nPort, GAMEPAD_ANALOG_BUTTON0,
                                 state.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_A],
                                 0, 0);
                }
            }
            else
            {
                buffer.m_pEvents[buffer.m_nEvents].m_nOffset =
                    GAMEPAD_ANALOG_BUTTON0 + nAnalog;
                buffer.m_pEvents[buffer.m_nEvents].m_nData =
                    state.Gamepad.bAnalogButtons[nAnalog];
                buffer.m_pEvents[buffer.m_nEvents].m_nSequence = 0;
                buffer.m_pEvents[buffer.m_nEvents].m_nTimeStamp = 0;
                buffer.m_nEvents++;
            }
        }
    }

    for (unsigned char nDigital = 0; nDigital < 8; nDigital++)
    {
        if ((state.Gamepad.wButtons & (1 << nDigital)) !=
            (pState->m_State.Gamepad.wButtons & (1 << nDigital)))
        {
            if (m_bAttemptingBind)
            {
                if (nDigital == 4)
                {
                    SetBindEvent(
                        nPort, GAMEPAD_DIGITAL_START,
                        (state.Gamepad.wButtons & XINPUT_GAMEPAD_START) != 0, 0,
                        0);
                }
            }
            else
            {
                buffer.m_pEvents[buffer.m_nEvents].m_nOffset =
                    GAMEPAD_DIGITAL_BUTTON0 + nDigital;
                buffer.m_pEvents[buffer.m_nEvents].m_nData =
                    (state.Gamepad.wButtons & (1 << nDigital)) != 0;
                buffer.m_pEvents[buffer.m_nEvents].m_nSequence = 0;
                buffer.m_pEvents[buffer.m_nEvents].m_nTimeStamp = 0;
                buffer.m_nEvents++;
            }
        }
    }

    if (!pState->m_bLeftXCalibrated)
    {
        if (state.Gamepad.sThumbLX > pState->m_nLeftXMax)
            pState->m_nLeftXMax = state.Gamepad.sThumbLX;
        if (state.Gamepad.sThumbLX < pState->m_nLeftXMin)
            pState->m_nLeftXMin = state.Gamepad.sThumbLX;
        if ((float)pState->m_nLeftXMax >=
                g_fDeadzoneCalibrationDetect * 32767.0f &&
            (float)pState->m_nLeftXMin <=
                g_fDeadzoneCalibrationDetect * -32767.0f)
        {
            pState->m_fLeftXDeadzone = g_fDeadzonePostCalibration;
            pState->m_bLeftXCalibrated = 1;
        }
    }
    if (!pState->m_bLeftYCalibrated)
    {
        if (state.Gamepad.sThumbLY > pState->m_nLeftYMax)
            pState->m_nLeftYMax = state.Gamepad.sThumbLY;
        if (state.Gamepad.sThumbLY < pState->m_nLeftYMin)
            pState->m_nLeftYMin = state.Gamepad.sThumbLY;
        if ((float)pState->m_nLeftYMax >=
                g_fDeadzoneCalibrationDetect * 32767.0f &&
            (float)pState->m_nLeftYMin <=
                g_fDeadzoneCalibrationDetect * -32767.0f)
        {
            pState->m_fLeftYDeadzone = g_fDeadzonePostCalibration;
            pState->m_bLeftYCalibrated = 1;
        }
    }
    if (!pState->m_bRightXCalibrated)
    {
        if (state.Gamepad.sThumbRX > pState->m_nRightXMax)
            pState->m_nRightXMax = state.Gamepad.sThumbRX;
        if (state.Gamepad.sThumbRX < pState->m_nRightXMin)
            pState->m_nRightXMin = state.Gamepad.sThumbRX;
        if ((float)pState->m_nRightXMax >=
                g_fDeadzoneCalibrationDetect * 32767.0f &&
            (float)pState->m_nRightXMin <=
                g_fDeadzoneCalibrationDetect * -32767.0f)
        {
            pState->m_fRightXDeadzone = g_fDeadzonePostCalibration;
            pState->m_bRightXCalibrated = 1;
        }
    }
    if (!pState->m_bRightYCalibrated)
    {
        if (state.Gamepad.sThumbRY > pState->m_nRightYMax)
            pState->m_nRightYMax = state.Gamepad.sThumbRY;
        if (state.Gamepad.sThumbRY < pState->m_nRightYMin)
            pState->m_nRightYMin = state.Gamepad.sThumbRY;
        if ((float)pState->m_nRightYMax >=
                g_fDeadzoneCalibrationDetect * 32767.0f &&
            (float)pState->m_nRightYMin <=
                g_fDeadzoneCalibrationDetect * -32767.0f)
        {
            pState->m_fRightYDeadzone = g_fDeadzonePostCalibration;
            pState->m_bRightYCalibrated = 1;
        }
    }

    if (m_nJoysticks > 1 && m_aPorts[0] != 3 && m_aPorts[nJoystick] == 3)
    {
        if (state.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_B] > 200)
            m_nDebugOption = 0;
        if (state.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_BLACK] > 200)
            m_nDebugOption = 1;
        if (state.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_WHITE] > 200)
            m_nDebugOption = 3;
    }

    if (((m_nCheatCombo & 1) || nJoystick == 0) && m_nJoysticks > 1)
    {
        m_nCheatCombo =
            state.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_Y] > 200 &&
            state.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] > 200 &&
            state.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] > 200;
    }

    if (pState->m_fLeftXDeadzone * 32767.0f >
        (float)abs(state.Gamepad.sThumbLX))
    {
        state.Gamepad.sThumbLX = 0;
    }
    if (state.Gamepad.sThumbLX != 0 || pState->m_State.Gamepad.sThumbLX != 0)
    {
        if (!m_bAttemptingBind)
        {
            buffer.m_pEvents[buffer.m_nEvents].m_nOffset = GAMEPAD_THUMB_LX;
            buffer.m_pEvents[buffer.m_nEvents].m_nData = state.Gamepad.sThumbLX;
            buffer.m_pEvents[buffer.m_nEvents].m_nSequence = 0;
            buffer.m_pEvents[buffer.m_nEvents].m_nTimeStamp = 0;
            buffer.m_nEvents++;
        }
    }

    if (pState->m_fLeftYDeadzone * 32767.0f >
        (float)abs(state.Gamepad.sThumbLY))
    {
        state.Gamepad.sThumbLY = 0;
    }
    if (state.Gamepad.sThumbLY != 0 || pState->m_State.Gamepad.sThumbLY != 0)
    {
        if (!m_bAttemptingBind)
        {
            buffer.m_pEvents[buffer.m_nEvents].m_nOffset = GAMEPAD_THUMB_LY;
            buffer.m_pEvents[buffer.m_nEvents].m_nData = -state.Gamepad.sThumbLY;
            if (state.Gamepad.sThumbLY == -32768)
                state.Gamepad.sThumbLY = 32767;
            else
                state.Gamepad.sThumbLY = -state.Gamepad.sThumbLY;
            buffer.m_pEvents[buffer.m_nEvents].m_nSequence = 0;
            buffer.m_pEvents[buffer.m_nEvents].m_nTimeStamp = 0;
            buffer.m_nEvents++;
        }
    }

    if (pState->m_fRightXDeadzone * 32767.0f >
        (float)abs(state.Gamepad.sThumbRX))
    {
        state.Gamepad.sThumbRX = 0;
    }
    if (state.Gamepad.sThumbRX != 0 || pState->m_State.Gamepad.sThumbRX != 0)
    {
        if (!m_bAttemptingBind)
        {
            buffer.m_pEvents[buffer.m_nEvents].m_nOffset = GAMEPAD_THUMB_RX;
            buffer.m_pEvents[buffer.m_nEvents].m_nData = state.Gamepad.sThumbRX;
            buffer.m_pEvents[buffer.m_nEvents].m_nSequence = 0;
            buffer.m_pEvents[buffer.m_nEvents].m_nTimeStamp = 0;
            buffer.m_nEvents++;
        }
    }

    if (pState->m_fRightYDeadzone * 32767.0f >
        (float)abs(state.Gamepad.sThumbRY))
    {
        state.Gamepad.sThumbRY = 0;
    }
    if (state.Gamepad.sThumbRY != 0 || pState->m_State.Gamepad.sThumbRY != 0)
    {
        if (!m_bAttemptingBind)
        {
            buffer.m_pEvents[buffer.m_nEvents].m_nOffset = GAMEPAD_THUMB_RY;
            buffer.m_pEvents[buffer.m_nEvents].m_nData = -state.Gamepad.sThumbRY;
            if (state.Gamepad.sThumbRY == -32768)
                state.Gamepad.sThumbRY = 32767;
            else
                state.Gamepad.sThumbRY = -state.Gamepad.sThumbRY;
            buffer.m_pEvents[buffer.m_nEvents].m_nSequence = 0;
            buffer.m_pEvents[buffer.m_nEvents].m_nTimeStamp = 0;
            buffer.m_nEvents++;
        }
    }

    if (nJoystick == 0)
    {
        if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_START) &&
            (state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK))
        {
            pState->m_nResetHeldTime += nTime - pState->m_nTime;
            if (pState->m_nResetHeldTime > 3000)
            {
                KOTOR_LAUNCH_DATA launchData;
                KOTOR_LAUNCH_OPTIONS_DATA options;

                launchData.m_nAutoCalInfo = GetAutoCalInfo();
                launchData.m_nPortBinding = GetPortBinding();
                g_pAppManager->m_pClientExoApp->GetClientOptions()
                    ->GetOptionsData(&options);
                launchData.SetOptionsData(options);

                g_pAppManager->m_pClientExoApp->SetLoadScreenByModuleName(
                    "default");
                g_pAppManager->m_pClientExoApp->SetLoadMusic("");
                g_pAppManager->m_pClientExoApp->SetLoadBarProgress(0, 1);
                g_pAppManager->m_pClientExoApp->SetLoadScreenHint(0);
                g_pAppManager->m_pClientExoApp->ShowLoadScreen(1, 1, 0);
                UpdateScreen(1.0f / 30.0f, 0, 0);

                launchData.SetLoadScreen("load_default");
                launchData.m_nLoadScreenHint = 0;
                AurPersistDisplay();
                ExoXboxLaunch(&launchData);
            }
        }
        else
        {
            pState->m_nResetHeldTime = 0;
        }
    }

    m_pLastStates[nPort].m_State = m_pStates[nPort].m_State;
    m_pStates[nPort].m_State = state;
    pState->m_nTime = nTime;
    return 1;
}
