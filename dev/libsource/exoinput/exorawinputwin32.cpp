#include <initguid.h>
#include "exorawinputinternal.h"
#include <string.h>
#include <math.h>
#include "swmain.h"

static int nCurrentJoystick = 0;

int g_nRawJoystickUnused1;
int g_nRawJoystickUnused2;
// PC 0x00600DB0
CExoDeviceBuffer::CExoDeviceBuffer()
{
    m_nEvents = 0;
    m_nCapacity = 0;
    m_pEvents = NULL;
}
// PC 0x0062F2C0
CExoDeviceBuffer::~CExoDeviceBuffer()
{
    if (m_pEvents != NULL)
    {
        delete m_pEvents;
        m_pEvents = NULL;
    }
}
// PC 0x004065A0
int CExoRawInputInternal::GetMainDeviceLost()
{
    return 0;
}
// PC 0x0062F2E0
void CExoRawInputInternal::ClearKeyboardBuffer()
{
    if (m_bKeyboardMode == 0 && m_pKeyboard != NULL && m_bActive != 0)
    {
        HRESULT nResult;
        do
        {
            DWORD nEvents = -1;
            nResult = m_pKeyboard->GetDeviceData(
                sizeof(DIDEVICEOBJECTDATA), NULL, &nEvents, 0);
            if (nResult == DI_OK)
                break;
            if (nResult != DIERR_NOTACQUIRED)
            {
                if (nResult == DIERR_INPUTLOST)
                    m_pKeyboard->Acquire();
                return;
            }
            nResult = m_pKeyboard->Acquire();
        } while (nResult == DIERR_INPUTLOST);
    }
}
// PC 0x0062F350
void CExoRawInputInternal::ClearMouseBuffer()
{
    if (m_pMouse != NULL && m_bActive != 0)
    {
        HRESULT nResult;
        do
        {
            DWORD nEvents = -1;
            nResult = m_pMouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA),
                                             NULL, &nEvents, 0);
            if (nResult == DI_OK)
                break;
            if (nResult != DIERR_NOTACQUIRED)
            {
                if (nResult == DIERR_INPUTLOST)
                    m_pMouse->Acquire();
                return;
            }
            nResult = m_pMouse->Acquire();
        } while (nResult == DIERR_INPUTLOST);
    }
}

void CExoRawInputInternal::ClearJoystickBuffer(int nJoystick)
{
    memset(&m_pJoystickStates[nJoystick], 0, sizeof(DIJOYSTATE));
    m_pJoystickStates[nJoystick].m_nTime =
        (unsigned long)(g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000);
}
// PC 0x0062F3B0
int CExoRawInputInternal::GetJoystickBuffer(int nJoystick,
                                            CExoDeviceBuffer& buffer)
{
    DIJOYSTATE joyState;

    if (buffer.m_pEvents != NULL)
    {
        delete buffer.m_pEvents;
        buffer.m_pEvents = NULL;
    }
    buffer.m_nEvents = 0;
    buffer.m_pEvents = new CExoDeviceEvent[256];
    if (buffer.m_pEvents == NULL)
    {
        return 0;
    }
    memset(buffer.m_pEvents, 0, 0x100);
    if (m_pJoysticks == NULL)
    {
        return 0;
    }
    if (m_pJoysticks[nJoystick] == NULL || m_pJoystickStates == NULL ||
        m_pLastJoystickStates == NULL || m_bActive == 0)
    {
        return 0;
    }
    m_pJoysticks[nJoystick]->Poll();
    HRESULT hr =
        m_pJoysticks[nJoystick]->GetDeviceState(sizeof(DIJOYSTATE), &joyState);
    if (hr != DI_OK)
    {
        if (hr > DIERR_NOTINITIALIZED)
        {
            if (hr == DIERR_INPUTLOST)
            {
                if (m_pJoysticks[nJoystick]->Acquire() != DI_OK)
                {
                    return 0;
                }
            }
            else
            {
                return 0;
            }
        }
        else
        {
            if (hr == DIERR_NOTINITIALIZED)
            {
                return 0;
            }
            switch (hr)
            {
            case DIERR_NOTACQUIRED:
                m_pJoysticks[nJoystick]->Acquire();
                return 0;
            case E_PENDING:
                return 0;
            }
            return 0;
        }
    }
    else
    {
        CExoJoystickState* pState = &m_pJoystickStates[nJoystick];
        unsigned long nTime = (unsigned long)(g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000);

        for (int i = 0; i != 32; i++)
        {
            if ((joyState.rgbButtons[i] ^ pState->m_State.rgbButtons[i]) & 0x80)
            {
                buffer.m_pEvents[buffer.m_nEvents].m_nOffset = 48 + i;
                buffer.m_pEvents[buffer.m_nEvents].m_nData = joyState.rgbButtons[i] >> 7;
                buffer.m_pEvents[buffer.m_nEvents].m_nSequence = 0;
                buffer.m_pEvents[buffer.m_nEvents].m_nTimeStamp = 0;
                ++buffer.m_nEvents;
            }
        }

        unsigned short nNewHats, nOldHats;
        nNewHats = nOldHats = 0;
        for (unsigned long j = 0; j != 4; j++)
        {
            if (joyState.rgdwPOV[j] != pState->m_State.rgdwPOV[j])
            {

                if ((unsigned short)joyState.rgdwPOV[j] != 0xFFFF)
                {
                    unsigned long nAngle = joyState.rgdwPOV[j] / 100;
                    if (nAngle == 0 || nAngle == 45 || nAngle == 315)
                        nNewHats |= 1 << (j);
                    if (nAngle == 45 || nAngle == 90 || nAngle == 135)
                        nNewHats |= 1 << (j + 1);
                    if (nAngle == 135 || nAngle == 180 || nAngle == 225)
                        nNewHats |= 1 << (j + 2);
                    if (nAngle == 225 || nAngle == 270 || nAngle == 315)
                        nNewHats |= 1 << (j + 3);
                }
                if ((unsigned short)pState->m_State.rgdwPOV[j] != 0xFFFF)
                {
                    unsigned long nAngle = pState->m_State.rgdwPOV[j] / 100;
                    if (nAngle == 0 || nAngle == 45 || nAngle == 315)
                        nOldHats |= 1 << (4 * j);
                    if (nAngle == 45 || nAngle == 90 || nAngle == 135)
                        nOldHats |= 1 << (4 * j + 1);
                    if (nAngle == 135 || nAngle == 180 || nAngle == 225)
                        nOldHats |= 1 << (4 * j + 2);
                    if (nAngle == 225 || nAngle == 270 || nAngle == 315)
                        nOldHats |= 1 << (4 * j + 3);
                }
            }
        }
        for (int k = 0; k != 4; k++)
        {
            if (joyState.rgdwPOV[k] != pState->m_State.rgdwPOV[k])
            {
                if (((unsigned short)nNewHats & (1 << (4 * k))) != ((unsigned short)nOldHats & (1 << (4 * k))))
                {
                    buffer.m_pEvents[buffer.m_nEvents].m_nOffset = 900 + k;
                    buffer.m_pEvents[buffer.m_nEvents].m_nData = ((unsigned short)nNewHats & (1 << (4 * k))) != 0;
                    buffer.m_pEvents[buffer.m_nEvents].m_nSequence = 0;
                    buffer.m_pEvents[buffer.m_nEvents].m_nTimeStamp = 0;
                    ++buffer.m_nEvents;
                }
                if (((unsigned short)nNewHats & (1 << (4 * k + 1))) != ((unsigned short)nOldHats & (1 << (4 * k + 1))))
                {
                    buffer.m_pEvents[buffer.m_nEvents].m_nOffset = 908 + k;
                    buffer.m_pEvents[buffer.m_nEvents].m_nData = ((unsigned short)nNewHats & (1 << (4 * k + 1))) != 0;
                    buffer.m_pEvents[buffer.m_nEvents].m_nSequence = 0;
                    buffer.m_pEvents[buffer.m_nEvents].m_nTimeStamp = 0;
                    ++buffer.m_nEvents;
                }
                if (((unsigned short)nNewHats & (1 << (4 * k + 2))) != ((unsigned short)nOldHats & (1 << (4 * k + 2))))
                {
                    buffer.m_pEvents[buffer.m_nEvents].m_nOffset = 904 + k;
                    buffer.m_pEvents[buffer.m_nEvents].m_nData = ((unsigned short)nNewHats & (1 << (4 * k + 2))) != 0;
                    buffer.m_pEvents[buffer.m_nEvents].m_nSequence = 0;
                    buffer.m_pEvents[buffer.m_nEvents].m_nTimeStamp = 0;
                    ++buffer.m_nEvents;
                }
                if (((unsigned short)nNewHats & (1 << (4 * k + 3))) != ((unsigned short)nOldHats & (1 << (4 * k + 3))))
                {
                    buffer.m_pEvents[buffer.m_nEvents].m_nOffset = 912 + k;
                    buffer.m_pEvents[buffer.m_nEvents].m_nData = ((unsigned short)nNewHats & (1 << (4 * k + 3))) != 0;
                    buffer.m_pEvents[buffer.m_nEvents].m_nSequence = 0;
                    buffer.m_pEvents[buffer.m_nEvents].m_nTimeStamp = 0;
                    ++buffer.m_nEvents;
                }
            }
        }

        int nCentre;
        int& rCentre = nCentre;
        float fCentre;
        float fValue;
        rCentre = (int)((double)(pState->m_aAxisRange[1] -
                          pState->m_aAxisRange[0]) * 0.5);
        fCentre = (float)rCentre;
        fValue = (float)joyState.lX - fCentre;
        if (!(fabs(fValue) > fCentre * 0.25f))
            fValue = 0.0f;
        if (fValue != 0.0f || pState->m_State.lX != 0)
        {
            buffer.m_pEvents[buffer.m_nEvents].m_nData = (unsigned long)fValue;
            buffer.m_pEvents[buffer.m_nEvents].m_nOffset = 0;
            buffer.m_pEvents[buffer.m_nEvents].m_nSequence = 0;
            buffer.m_pEvents[buffer.m_nEvents].m_nTimeStamp = 0;
            ++buffer.m_nEvents;
        }
        joyState.lX = (unsigned long)fValue;

        rCentre = (int)((double)(pState->m_aAxisRange[5] -
                          pState->m_aAxisRange[4]) * 0.5);
        fCentre = (float)rCentre;
        fValue = (float)joyState.lY - fCentre;
        if (!(fabs(fValue) > fCentre * 0.25f))
            fValue = 0.0f;
        if (fValue != 0.0f || pState->m_State.lY != 0)
        {
            buffer.m_pEvents[buffer.m_nEvents].m_nData = (unsigned long)fValue;
            buffer.m_pEvents[buffer.m_nEvents].m_nOffset = 4;
            buffer.m_pEvents[buffer.m_nEvents].m_nSequence = 0;
            buffer.m_pEvents[buffer.m_nEvents].m_nTimeStamp = 0;
            ++buffer.m_nEvents;
        }
        joyState.lY = (unsigned long)fValue;

        rCentre = (int)((double)(pState->m_aAxisRange[1] -
                          pState->m_aAxisRange[0]) * 0.5);
        fCentre = (float)rCentre;
        fValue = (float)joyState.rglSlider[0] - fCentre;
        if (!(fabs(fValue) > fCentre * 0.25f))
            fValue = 0.0f;
        if (fValue != 0.0f || pState->m_State.rglSlider[0] != 0)
        {
            buffer.m_pEvents[buffer.m_nEvents].m_nData = (unsigned long)fValue;
            buffer.m_pEvents[buffer.m_nEvents].m_nOffset = 24;
            buffer.m_pEvents[buffer.m_nEvents].m_nSequence = 0;
            buffer.m_pEvents[buffer.m_nEvents].m_nTimeStamp = 0;
            ++buffer.m_nEvents;
        }
        joyState.rglSlider[0] = (unsigned long)fValue;

        rCentre = (int)((double)(pState->m_aAxisRange[1] -
                          pState->m_aAxisRange[0]) * 0.5);
        fCentre = (float)rCentre;
        fValue = (float)joyState.lRz - fCentre;
        if (!(fabs(fValue) > fCentre * 0.25f))
            fValue = 0.0f;
        if (fValue != 0.0f || pState->m_State.lRz != 0)
        {
            buffer.m_pEvents[buffer.m_nEvents].m_nData = (unsigned long)fValue;
            buffer.m_pEvents[buffer.m_nEvents].m_nOffset = 28;
            buffer.m_pEvents[buffer.m_nEvents].m_nSequence = 0;
            buffer.m_pEvents[buffer.m_nEvents].m_nTimeStamp = 0;
            ++buffer.m_nEvents;
        }
        joyState.lRz = (unsigned long)fValue;

        m_pLastJoystickStates[nJoystick].m_State = m_pJoystickStates[nJoystick].m_State;
        m_pJoystickStates[nJoystick].m_State = joyState;
        pState->m_nTime = nTime;
    }
    return 1;
}
// PC 0x0062FC00
int CExoRawInputInternal::GetLastState(unsigned long nDevice,
                                       unsigned long nControl)
{
    if (nDevice >= MAX_INPUT_DEVICES)
    {
        return 0;
    }
    if (nDevice == CExoInput::INPUTDEVICE_KEYBOARD)
    {
        return 1;
    }
    if (nDevice == CExoInput::INPUTDEVICE_MOUSE)
    {
        return 1;
    }
    if ((int)nDevice < CExoInput::INPUTDEVICE_JOYSTICKSTART)
    {
        return 0;
    }
    if ((nControl >= DIJOFS_BUTTON(0) && nControl <= DIJOFS_BUTTON(31)) ||
        nControl == DIJOFS_POV(0) ||
        (nControl != DIJOFS_X && nControl != DIJOFS_Y &&
         nControl != DIJOFS_SLIDER(0) && nControl != DIJOFS_SLIDER(1)))
    {
        return 0;
    }
    DIJOYSTATE* pState = &m_pLastJoystickStates[nDevice - 2].m_State;
    int nResult;
    switch (nControl)
    {
    case DIJOFS_X:
        nResult = pState->lX;
        break;
    case DIJOFS_Y:
        nResult = pState->lY;
        break;
    case DIJOFS_SLIDER(0):
        nResult = pState->rglSlider[0];
        break;
    case DIJOFS_SLIDER(1):
        nResult = pState->lRz;
        break;
    default:
        return 0;
    }
    return nResult;
}
// PC 0x0062FCC0
int CExoRawInputInternal::IsDigital(unsigned long nDevice,
                                    unsigned long nControl)
{
    if (nDevice == CExoInput::INPUTDEVICE_KEYBOARD)
    {
        return 1;
    }
    if (nDevice == CExoInput::INPUTDEVICE_MOUSE)
    {
        if (nControl < 0xC || nControl > 0xF)
        {
            return nControl <= 8;
        }
        return 1;
    }
    return (int)nDevice >= CExoInput::INPUTDEVICE_JOYSTICKSTART &&
           ((nControl >= 0x30 && nControl <= 0x4F) ||
            (nControl >= 0x384 && nControl <= 0x393) || nControl == 32);
}
// PC 0x0062FD30
float CExoRawInputInternal::GetMaxUseable(unsigned long nDevice,
                                          unsigned long nControl)
{
    if (nDevice == CExoInput::INPUTDEVICE_KEYBOARD)
    {
        return 0.0f;
    }
    else
    {
        if (nDevice == CExoInput::INPUTDEVICE_MOUSE)
        {
            return 0.0f;
        }
        else
        {
            if (nControl != 0 && nControl != 4 && nControl != 24 &&
                nControl != 28)
            {
                return 0.0f;
            }
            return 32767.0f;
        }
    }
}
// PC 0x0062FD70
float CExoRawInputInternal::GetMinUseable(unsigned long nDevice,
                                          unsigned long nControl)
{
    if (nDevice == CExoInput::INPUTDEVICE_KEYBOARD)
    {
        return 0.0f;
    }
    else
    {
        if (nDevice == CExoInput::INPUTDEVICE_MOUSE)
        {
            return 0.0f;
        }
        else
        {
            if (nControl != 0 && nControl != 4 && nControl != 24 &&
                nControl != 28)
            {
                return 0.0f;
            }
            return 8191.75f;
        }
    }
}
// PC 0x0062FDB0
int CExoRawInputInternal::GetKeyboardBuffer(CExoDeviceBuffer& buffer)
{
    if (m_pKeyboard == NULL)
    {
        return 0;
    }
    if (m_bActive == 0)
    {
        return 0;
    }
    if (buffer.m_pEvents != NULL)
    {
        delete buffer.m_pEvents;
        buffer.m_pEvents = NULL;
    }
    buffer.m_nEvents = 0;
    if (m_bKeyboardMode == 0)
    {
        buffer.m_pEvents = new CExoDeviceEvent[256];
        if (buffer.m_pEvents == NULL)
        {
            return 0;
        }
        memset(buffer.m_pEvents, 0, 0x100);
        DWORD nEvents = 256;
        HRESULT hr = m_pKeyboard->GetDeviceData(
            sizeof(DIDEVICEOBJECTDATA), (DIDEVICEOBJECTDATA*)buffer.m_pEvents,
            &nEvents, 0);
        if (hr != DI_OK)
        {
            if (hr <= DIERR_INPUTLOST)
            {
                if (hr == DIERR_INPUTLOST)
                {
                    hr = m_pKeyboard->Acquire();
                    if (hr == DI_OK)
                    {
                        return 1;
                    }
                }
                else
                {
                    switch (hr)
                    {
                    case DIERR_NOTBUFFERED:
                        hr = DIERR_NOTBUFFERED;
                        break;
                    case DIERR_NOTACQUIRED:
                        m_pKeyboard->Acquire();
                        return 1;
                    case DIERR_NOTINITIALIZED:
                        return 0;
                    }
                }
            }
            else
            {
                switch (hr)
                {
                case DIERR_INVALIDPARAM:
                    hr = DIERR_INVALIDPARAM;
                    break;
                case DI_BUFFEROVERFLOW:
                    return 1;
                }
            }
            if (hr != DI_OK)
            {
                return 0;
            }
        }
        else
        {
            buffer.m_nEvents = nEvents;
        }
    }
    return 1;
}
// PC 0x0062FEB0
void CExoRawInputInternal::KeyboardModeOn()
{
    m_bKeyboardMode = 1;
    if (m_pKeyboard != NULL)
    {
        m_pKeyboard->Unacquire();
    }
}
// PC 0x0062FED0
void CExoRawInputInternal::KeyboardModeOff()
{
    m_bKeyboardMode = 0;
    if (m_pKeyboard != NULL)
    {
        m_pKeyboard->Acquire();
    }
}
// PC 0x0062FEF0
void CExoRawInputInternal::SetActive(int bActive)
{
    if (bActive != m_bActive)
    {
        if (m_pKeyboard != NULL)
        {
            if (bActive == 1)
            {
                if (m_bKeyboardMode == 0)
                {
                    m_pKeyboard->Acquire();
                }
            }
            else
            {
                m_pKeyboard->Unacquire();
            }
        }
        if (m_pMouse != NULL)
        {
            if (bActive == 1)
            {
                m_pMouse->Acquire();
            }
            else
            {
                m_pMouse->Unacquire();
            }
        }
        for (int i = 0; i < m_nJoysticks; i++)
        {
            if (m_pJoysticks != NULL)
            {
                IDirectInputDevice8* pJoystick = m_pJoysticks[i];
                if (pJoystick != NULL)
                {
                    if (bActive == 1)
                    {
                        pJoystick->Acquire();
                    }
                    else
                    {
                        pJoystick->Unacquire();
                    }
                }
            }
        }
        m_bActive = bActive;
        if (bActive != 0)
        {
            ClearKeyboardBuffer();
            ClearMouseBuffer();
            for (int j = 0; j < m_nJoysticks; j++)
            {
                ClearJoystickBuffer(j);
            }
        }
    }
}
// PC 0x0062FFE0
CExoString CExoRawInputInternal::GetDisplayCharacterDirectInputValue(
    unsigned long nKey)
{
    char sBuffer[80];
    DIPROPSTRING dipStr;

    dipStr.diph.dwObj = nKey;
    dipStr.diph.dwSize = sizeof(DIPROPSTRING);
    dipStr.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    dipStr.diph.dwHow = DIPH_BYOFFSET;
    if (m_pKeyboard->GetProperty(DIPROP_KEYNAME, &dipStr.diph) >= 0)
    {
        memset(sBuffer, 0, sizeof(sBuffer));
        g_pExoBase->WcsToMbs(sBuffer, dipStr.wsz, 79);
        return CExoString(sBuffer);
    }
    return CExoString("N/A");
}

void CExoRawInputInternal::ShutDownDirectInputMouse()
{
    if (m_pMouse != NULL)
    {
        m_pMouse->Unacquire();
        m_pMouse->Release();
        m_pMouse = NULL;
    }
}
// PC 0x00630090
int CExoRawInputInternal::ShutDownDirectInput()
{
    int nResult = (int)m_pDirectInput;
    if (nResult != 0)
    {
        if (m_pKeyboard != NULL)
        {
            m_pKeyboard->Unacquire();
            m_pKeyboard->Release();
            m_pKeyboard = NULL;
        }
        ShutDownDirectInputMouse();
        for (int i = 0; i < m_nJoysticks; i++)
        {
            if (m_pJoysticks[i] != NULL)
            {
                m_pJoysticks[i]->Unacquire();
                m_pJoysticks[i]->Release();
                m_pJoysticks[i] = NULL;
            }
        }
        if (m_pJoystickStates != NULL)
        {
            delete m_pJoystickStates;
            m_pJoystickStates = NULL;
        }
        if (m_pLastJoystickStates != NULL)
        {
            delete m_pLastJoystickStates;
        }
        if (m_pJoysticks != NULL)
        {
            delete m_pJoysticks;
            m_pJoysticks = NULL;
        }
        nResult = m_pDirectInput->Release();
        m_pDirectInput = NULL;
    }
    return nResult;
}
// PC 0x00630150
CExoRawInputInternal::CExoRawInputInternal(HINSTANCE hInstance, HWND hWnd)
{
    DIPROPDWORD dipBuffer;

    m_bInitialized = 0;
    m_pDirectInput = NULL;
    m_pKeyboard = NULL;
    m_pMouse = NULL;
    m_bActive = 0;
    m_bKeyboardMode = 0;
    m_bInputEnabled = 1;
    m_nLastKey = 0;
    m_nMouseButtons = 0;
    HRESULT hResult = E_FAIL;
    do
    {
        if ((DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8A,
                                (void**)&m_pDirectInput, NULL) != DI_OK &&
             DirectInput8Create(hInstance, 0x300, IID_IDirectInput8A,
                                (void**)&m_pDirectInput, NULL) != DI_OK) ||
            m_pDirectInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, NULL) != DI_OK ||
            m_pKeyboard->SetDataFormat(&c_dfDIKeyboard) != DI_OK ||
            m_pKeyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE) != DI_OK)
            break;
        dipBuffer.diph.dwSize = sizeof(DIPROPDWORD);
        dipBuffer.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        dipBuffer.diph.dwObj = 0;
        dipBuffer.diph.dwHow = 0;
        dipBuffer.dwData = 256;
        hResult = m_pKeyboard->SetProperty(DIPROP_BUFFERSIZE, &dipBuffer.diph);
    } while (0);
    if (hResult != DI_OK)
    {
        ShutDownDirectInput();
        return;
    }
    m_pJoysticks = NULL;
    m_pJoystickStates = NULL;
    m_pLastJoystickStates = NULL;
    m_nJoysticks = 0;
    m_bInitialized = 1;
    nCurrentJoystick = 0;
}
// PC 0x00630260
CExoRawInputInternal::~CExoRawInputInternal()
{
    ShutDownDirectInput();
}
// PC 0x00630270
int CExoRawInputInternal::InitializeDirectInputMouse()
{
    DIPROPDWORD dipBuffer;

    if (m_pDirectInput->CreateDevice(GUID_SysMouse, &m_pMouse, NULL) != 0)
    {
        ShutDownDirectInput();
        return 0;
    }
    if (m_pMouse->SetDataFormat(&c_dfDIMouse) != 0)
    {
        ShutDownDirectInput();
        return 0;
    }
    if (m_pMouse->SetCooperativeLevel(g_hRenderWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE) != 0)
    {
        ShutDownDirectInput();
        return 0;
    }
    dipBuffer.diph.dwObj = 0;
    dipBuffer.diph.dwHow = 0;
    dipBuffer.diph.dwSize = sizeof(DIPROPDWORD);
    dipBuffer.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    dipBuffer.dwData = 256;
    if (m_pMouse->SetProperty(DIPROP_BUFFERSIZE, &dipBuffer.diph) != 0)
    {
        ShutDownDirectInput();
        return 0;
    }
    if (m_pMouse->Acquire() != 0)
    {
        ShutDownDirectInput();
    }
    return 1;
}
// PC 0x00630320
int CExoRawInputInternal::GetMouseState(CExoMouseState& state)
{
    if (m_pMouse == NULL)
    {
        InitializeDirectInputMouse();
    }
    int bReady = 0;
    if (m_pMouse != NULL && m_bActive != 0)
    {
        bReady = 1;
    }
    if (bReady == 0)
    {
        return 0;
    }
    HRESULT hr = m_pMouse->GetDeviceState(sizeof(CExoMouseState), &state);
    if (hr != DI_OK)
    {
        switch (hr)
        {
        case DIERR_INPUTLOST:
        case DIERR_NOTACQUIRED:
            hr = m_pMouse->Acquire();
            break;
        case DIERR_NOTINITIALIZED:
        case E_PENDING:
            return 0;
        default:
            return 0;
        }
    }
    if (hr != DI_OK)
    {
        return 0;
    }
    return 1;
}
// PC 0x006955A0
int CExoRawInputInternal::GetControlThreshold(unsigned long nControl)
{
    return 0;
}
