#include "exorawinputinternal.h"
#include <math.h>
#include <limits.h>
#include "swmain.h"

static inline void ExoinputinternalTrace3();
static inline void ExoinputinternalTrace2();
static inline void ExoinputinternalTrace1();
#define EXOINPUTINTERNAL_TRACE() ExoinputinternalTrace3()

float fMaxMickies = 100.0f;

int g_nDebugBreakMinSeverity = INT_MAX;

const unsigned long KEYBOARD_KEY_DOWN = 0x80;

#include "exoinputinternal.h"

#ifndef _XBOX
void CExoInputInternal::EndAttemptedBind()  // PC 0x004015A0
{
}
// PC 0x004015A0
void CExoInputInternal::UpdateJoysticks()
{
}
#endif
// PC 0x004015A0
void CExoInputInternal::Configure()
{
}
// PC 0x00401590
int CExoInputInternal::SetRumble(int nArg1, float fArg2, float fArg3,
                               unsigned long nArg4)
{
#ifdef _XBOX
    return m_pRawInput->SetRumble(nArg1, fArg2, fArg3, nArg4);
#else
    return 0;
#endif
}

#ifndef _XBOX
int CExoInputInternal::IsModalKeyDown_Shift()
{
    return (GetKeyState(VK_SHIFT) >> 15) & 1;
}

int CExoInputInternal::IsModalKeyDown_Ctrl()
{
    return (GetKeyState(VK_CONTROL) >> 15) & 1;
}

int CExoInputInternal::IsModalKeyDown_Alt()
{
    return (GetKeyState(VK_MENU) >> 15) & 1;
}
#endif
// PC 0x0062BFD0
CExoInputEventDescDetailed::CExoInputEventDescDetailed()
{
    m_fScale = 1.0f;
    m_fExponent = 1.0f;
    m_bScaled = 0;
    m_nRelative = 0;
}
// PC 0x0062C000
CExoInputEventDescDetailed::CExoInputEventDescDetailed(
    const CExoInputEventDesc& desc)
    : CExoInputEventDesc(desc)
{
    m_fScale = 1.0f;
    m_fExponent = 1.0f;
    m_bScaled = 0;
    m_nRelative = 0;
}
// PC 0x0062C060
CExoInputeventDesc2ButtonAxis::CExoInputeventDesc2ButtonAxis()
{
    m_nRelative2 = 0;
}

#ifndef _XBOX
float CExoInputInternal::ScaledValue(CExoInputEventDesc* pDesc,  // PC 0x0062C090
                                     unsigned long nControl,
                                     unsigned long nValue)
{
    float fValue = (float)(int)nValue;

    if (!pDesc->IsDetailed() ||
        ((CExoInputEventDescDetailed*)pDesc)->m_bScaled == 0)
    {
        return fValue;
    }

    if (nControl >= CExoInput::KEYBOARD_RETURN &&
        nControl <= CExoInput::KEYBOARD_SPACE)
    {
        return fValue * 0.0078125f;
    }

    if (nControl == CExoInput::MOUSE_XAXIS ||
        nControl == CExoInput::MOUSE_YAXIS)
    {
        return max(-1.0f, min(1.0f, fValue * 0.01f));
    }

    if (nControl == CExoInput::JOYSTICK_XAXIS ||
        nControl == CExoInput::JOYSTICK_YAXIS)
    {
        if (nValue != 0)
        {
            float fMax = m_pRawInput->GetMaxUseable(pDesc->m_nClass,
                                                    m_aControlMap[nControl]);
            float fMin = m_pRawInput->GetMinUseable(pDesc->m_nClass,
                                                    m_aControlMap[nControl]);
            float fScaled =
                min(1.0f, ((float)fabs(fValue) - fMin - 1.0f) / (fMax - fMin));
            return fScaled * (((int)nValue < 0) ? -1.0f : 1.0f);
        }
        return fValue;
    }

    if (nControl >= CExoInput::JOYSTICK_SLIDER0 ||
        nControl <= CExoInput::JOYSTICK_SLIDER2)
    {
        if (nValue != 0)
        {
            float fMax = m_pRawInput->GetMaxUseable(pDesc->m_nClass,
                                                    m_aControlMap[nControl]);
            float fMin = m_pRawInput->GetMinUseable(pDesc->m_nClass,
                                                    m_aControlMap[nControl]);
            float fScaled =
                min(1.0f, ((float)fabs(fValue) - fMin - 1.0f) / (fMax - fMin));
            return fScaled * (((int)nValue < 0) ? -1.0f : 1.0f);
        }
    }

    return fValue;
}
#endif
// PC 0x0062C250
int CExoInputInternal::GetActive()
{
    if (m_pRawInput != NULL)
    {
        return m_pRawInput->m_bActive;
    }
    return 0;
}
// PC 0x0062C270
void CExoInputInternal::KeyboardModeOn()
{
    if (m_pRawInput != NULL)
    {
        m_pRawInput->KeyboardModeOn();
    }
    m_bKeyboardMode = 1;
}
// PC 0x0062C290
void CExoInputInternal::KeyboardModeOff()
{
    if (m_pRawInput != NULL)
    {
        m_pRawInput->KeyboardModeOff();
        m_bKeyboardMode = 0;
    }
    else
    {
        m_bKeyboardMode = 1;
    }
}
// PC 0x0062C2C0
void CExoInputInternal::SetActive(int bActive)
{
    m_bActive = bActive;
    if (m_pRawInput != NULL)
    {
        m_pRawInput->SetActive(bActive);
    }
}
// PC 0x0062C2E0
int CExoInputInternal::GetMainDeviceLost()
{
    return m_pRawInput->GetMainDeviceLost();
}
// PC 0x0062C2F0
void CExoInputInternal::PauseRumble()
{
#ifdef _XBOX
    m_pRawInput->PauseRumble();
#else
    m_pRawInput->m_nLastKey = 1;
#endif
}
// PC 0x0062C300
void CExoInputInternal::UnpauseRumble()
{
#ifdef _XBOX
    m_pRawInput->UnpauseRumble();
#else
    m_pRawInput->m_nLastKey = 0;
#endif
}
// PC 0x0062C740
CExoInputEventDesc* CExoInputInternal::IsControlUsed(unsigned long nControl,
                                                     int nDevice,
                                                     int nInputClass)
{
#ifdef _XBOX
    if (nInputClass >= 0 && nInputClass < 6 && nDevice >= 0 && nDevice < 4 &&
        nControl < 0x6D)
    {
        CExoLinkedList<unsigned long>& llDevice =
            m_aClasses[nInputClass].m_llDevices[nDevice];
        CExoLinkedListNode* pNode = llDevice.GetHeadPos();
        while (pNode != NULL)
        {
            unsigned long nEvent =
                *(unsigned long*)llDevice.GetAtPos(pNode);
            CExoInputEventDesc* pDesc = m_lstEvents.element[nEvent];
            if (pDesc->m_nControl == nControl ||
                (pDesc->Is2Button() &&
                 ((CExoInputeventDesc2ButtonAxis*)pDesc)->m_nControl2 ==
                     nControl))
            {
                return pDesc;
            }
            llDevice.GetNext(pNode);
        }
    }
    return NULL;
#else
    if (nInputClass < 0 || nInputClass >= 6)
    {
        return NULL;
    }
    if (nDevice < 0 || nDevice >= 6)
    {
        return NULL;
    }
    if (nControl >= 0x84)
    {
        return NULL;
    }

    CExoLinkedList<unsigned long>& llDevice =
        m_aClasses[nInputClass].m_llDevices[nDevice];
    CExoLinkedListNode* pNode = llDevice.GetHeadPos();
    while (pNode != NULL)
    {
        unsigned long nEvent =
            *(unsigned long*)llDevice.GetAtPos(pNode);
        CExoInputEventDesc* pDesc = m_lstEvents.element[nEvent];
        if (pDesc->m_nControl == nControl ||
            (pDesc->Is2Button() &&
             ((CExoInputeventDesc2ButtonAxis*)pDesc)->m_nControl2 == nControl))
        {
            return pDesc;
        }
        llDevice.GetNext(pNode);
    }
    return NULL;
#endif
}
// PC 0x0062C7E0
int CExoInputInternal::SetEventRepeatable(unsigned long nEvent,
                                          unsigned long nDelay,
                                          unsigned long nRate)
{
    CExoInputEventDesc* pDesc = m_lstEvents.element[nEvent];
    if (pDesc == NULL || pDesc->m_nType != 1)
    {
        return 0;
    }

    CExoLinkedListNode* pNode = m_llRepeat.GetHeadPos();
    while (pNode != NULL)
    {
        CExoInputRepeatEvent* pRepeat =
            (CExoInputRepeatEvent*)m_llRepeat.GetAtPos(pNode);
        if (pRepeat != NULL && pRepeat->m_nEvent == nEvent)
        {
            pRepeat->m_nDelay = nDelay;
            pRepeat->m_nRate = nRate;
            return 1;
        }
        m_llRepeat.GetNext(pNode);
    }

    CExoInputRepeatEvent* pNewRepeat = new CExoInputRepeatEvent;
    if (pNewRepeat == NULL)
    {
        return 0;
    }
    pNewRepeat->m_nEvent = nEvent;
    pNewRepeat->m_nDelay = nDelay;
    pNewRepeat->m_nRate = nRate;
    pNewRepeat->m_nElapsed = 0;
    pNewRepeat->m_bRepeating = 0;
    pNewRepeat->m_nLastValue = 0;
    pNewRepeat->m_nHeldValue = 0;
    m_llRepeat.AddHead(pNewRepeat);
    return 1;
}
// PC 0x0062C8B0
void CExoInputInternal::ClearEventRepeat(unsigned long nEvent)
{
    CExoInputEventDesc* pDesc = m_lstEvents.element[nEvent];
    if (pDesc != NULL && pDesc->m_nType == 1)
    {
        CExoLinkedListNode* pNode = m_llRepeat.GetHeadPos();
        while (pNode != NULL)
        {
            CExoInputRepeatEvent* pRepeat =
                (CExoInputRepeatEvent*)m_llRepeat.GetAtPos(pNode);
            if (pRepeat != NULL && pRepeat->m_nEvent == nEvent)
            {
                pRepeat->m_nElapsed = 0;
                pRepeat->m_bRepeating = 0;
                pRepeat->m_nHeldValue = 0;
                pRepeat->m_nLastValue = 0;
                return;
            }
            m_llRepeat.GetNext(pNode);
        }
    }
}
// PC 0x0062C930
void CExoInputInternal::CoolDownEvent(unsigned long nEvent,
                                      unsigned long nCoolDown)
{
    CExoLinkedListNode* pNode = m_llCoolDown.GetHeadPos();
    CExoInputEventDesc** ppDesc = &m_lstEvents.element[nEvent];

    if (*ppDesc != NULL)
    {
        (*ppDesc)->m_nCoolDown = nCoolDown;
        if (pNode != NULL)
        {
            while (pNode != NULL)
            {
                CExoInputEventDesc* pCooling =
                    *(CExoInputEventDesc**)m_llCoolDown.GetAtPos(
                        pNode);
                if (pCooling != NULL &&
                    pCooling->m_nEvent == (*ppDesc)->m_nEvent)
                {
                    if (nCoolDown == 0)
                    {
                        m_llCoolDown.Remove(pNode);
                    }
                    return;
                }
                m_llCoolDown.GetNext(pNode);
            }
        }
        m_llCoolDown.AddTail(ppDesc);
    }
}
// PC 0x0062C9D0
void CExoInputInternal::ClearEvents()
{
    if (m_pRawInput != NULL)
    {
        m_pRawInput->ClearMouseBuffer();
        m_pRawInput->ClearKeyboardBuffer();

        for (int i = 0; i < m_lstEvents.num; i++)
        {
            CExoInputEventDesc* pDesc = m_lstEvents.element[i];
            if (pDesc != NULL)
            {
                switch (pDesc->m_nType)
                {
                case 0:
                case 1:
                case 2:
                case 5:
                    pDesc->m_nValue = 0;
                    break;

                case 3:
                    ((CExoInputEventDescDetailed*)pDesc)->m_nRelative = 0;
                    pDesc->m_nValue = 0;
                    break;

                case 4:
#ifndef _XBOX
                    ((CExoInputeventDesc2ButtonAxis*)pDesc)->m_nRelative2 = 0;
#endif
                    ((CExoInputEventDescDetailed*)pDesc)->m_nRelative = 0;
                    pDesc->m_nValue = 0;
                    break;
                }
            }
        }

        CExoLinkedListNode* pNode = m_llRepeat.GetHeadPos();
        while (pNode != NULL)
        {
            CExoInputRepeatEvent* pRepeat =
                (CExoInputRepeatEvent*)m_llRepeat.GetAtPos(pNode);
            pRepeat->m_nElapsed = 0;
            pRepeat->m_bRepeating = 0;
            m_llRepeat.GetNext(pNode);
        }

#ifndef _XBOX
        while (m_llBuffered.GetHeadPos() != NULL)
        {
            CExoInputEvent* pEvent = m_llBuffered.RemoveHead();
            if (pEvent != NULL)
            {
                if (pEvent->m_pDesc != NULL)
                {
                    delete pEvent->m_pDesc;
                    pEvent->m_pDesc = NULL;
                }
                delete pEvent;
            }
        }
#endif
    }
}
// PC 0x0062CAD0
void CExoInputInternal::WriteEventToFile(unsigned long nTimeStamp, int nEvent,
                                         CExoFile* pFile)
{
    if (pFile != NULL || nEvent < 0 || nEvent >= m_nOutputEvents)
    {
        CExoInputEventDesc* pDesc = m_lstOutput[nEvent].m_pDesc;
        CExoString sEvent;
        unsigned long nValue = m_lstOutput[nEvent].m_nValue;
        sEvent.Format("%d %c %d %d\n", nTimeStamp, 'I', pDesc->m_nEvent,
                      nValue);
        pFile->Write(sEvent);
    }
}

#ifndef _XBOX
CExoInputInternal::CExoInputInternal(HINSTANCE hInstance, HWND hWnd)  // PC 0x0062DB40
{
    m_bActive = 0;
    m_pRawInput = NULL;
    m_nOutputEvents = 0;
    m_bKeyboardMode = 0;
    m_bCaptureKeyboardKeys = 0;
    m_nUnknown388 = 0;
    m_nUnknown38C = 0;
    m_nUnknown390 = 0;
    m_nUnknown394 = 0;

    m_aControlMap[CExoInput::JOYSTICK_BUTTON0] = DIJOFS_BUTTON(0);
    m_aControlMap[CExoInput::JOYSTICK_BUTTON1] = DIJOFS_BUTTON(1);
    m_aControlMap[CExoInput::JOYSTICK_BUTTON2] = DIJOFS_BUTTON(2);
    m_aControlMap[CExoInput::JOYSTICK_BUTTON3] = DIJOFS_BUTTON(3);
    m_aControlMap[CExoInput::JOYSTICK_BUTTON4] = DIJOFS_BUTTON(4);
    m_aControlMap[CExoInput::JOYSTICK_BUTTON5] = DIJOFS_BUTTON(5);
    m_aControlMap[CExoInput::JOYSTICK_BUTTON6] = DIJOFS_BUTTON(6);
    m_aControlMap[CExoInput::JOYSTICK_BUTTON7] = DIJOFS_BUTTON(7);
    m_aControlMap[CExoInput::JOYSTICK_BUTTON8] = DIJOFS_BUTTON(8);
    m_aControlMap[CExoInput::JOYSTICK_BUTTON9] = DIJOFS_BUTTON(9);
    m_aControlMap[CExoInput::JOYSTICK_BUTTON10] = DIJOFS_BUTTON(10);
    m_aControlMap[CExoInput::JOYSTICK_BUTTON11] = CExoRawInputInternal::HAT_UP;
    m_aControlMap[CExoInput::JOYSTICK_BUTTON12] = CExoRawInputInternal::HAT_RIGHT;
    m_aControlMap[CExoInput::JOYSTICK_BUTTON13] = CExoRawInputInternal::HAT_DOWN;
    m_aControlMap[CExoInput::JOYSTICK_BUTTON14] = CExoRawInputInternal::HAT_LEFT;
    m_aControlMap[CExoInput::JOYSTICK_XAXIS] = DIJOFS_X;
    m_aControlMap[CExoInput::JOYSTICK_YAXIS] = DIJOFS_Y;
    m_aControlMap[CExoInput::JOYSTICK_HAT] = DIJOFS_POV(0);
    m_aControlMap[CExoInput::JOYSTICK_SLIDER0] = DIJOFS_SLIDER(0);
    m_aControlMap[CExoInput::JOYSTICK_SLIDER1] = DIJOFS_SLIDER(1);
    m_aControlMap[CExoInput::JOYSTICK_SLIDER2] = DIJOFS_SLIDER(2);
    m_aControlMap[CExoInput::MOUSE_BUTTON0] = DIMOFS_BUTTON0;
    m_aControlMap[CExoInput::MOUSE_BUTTON1] = DIMOFS_BUTTON1;
    m_aControlMap[CExoInput::MOUSE_BUTTON2] = DIMOFS_BUTTON2;
    m_aControlMap[CExoInput::MOUSE_XAXIS] = DIMOFS_X;
    m_aControlMap[CExoInput::MOUSE_YAXIS] = DIMOFS_Y;
    m_aControlMap[CExoInput::MOUSE_ZAXIS] = DIMOFS_Z;
    m_aControlMap[CExoInput::KEYBOARD_RETURN] = DIK_RETURN;
    m_aControlMap[CExoInput::KEYBOARD_SPACE] = DIK_SPACE;
    m_aControlMap[CExoInput::KEYBOARD_LEFT_ARROW] = DIK_LEFT;
    m_aControlMap[CExoInput::KEYBOARD_RIGHT_ARROW] = DIK_RIGHT;
    m_aControlMap[CExoInput::KEYBOARD_UP_ARROW] = DIK_UP;
    m_aControlMap[CExoInput::KEYBOARD_DOWN_ARROW] = DIK_DOWN;
    m_aControlMap[CExoInput::KEYBOARD_NUMPAD1] = DIK_NUMPAD1;
    m_aControlMap[CExoInput::KEYBOARD_NUMPAD2] = DIK_NUMPAD2;
    m_aControlMap[CExoInput::KEYBOARD_NUMPAD3] = DIK_NUMPAD3;
    m_aControlMap[CExoInput::KEYBOARD_NUMPAD4] = DIK_NUMPAD4;
    m_aControlMap[CExoInput::KEYBOARD_NUMPAD5] = DIK_NUMPAD5;
    m_aControlMap[CExoInput::KEYBOARD_NUMPAD6] = DIK_NUMPAD6;
    m_aControlMap[CExoInput::KEYBOARD_NUMPAD7] = DIK_NUMPAD7;
    m_aControlMap[CExoInput::KEYBOARD_NUMPAD8] = DIK_NUMPAD8;
    m_aControlMap[CExoInput::KEYBOARD_NUMPAD9] = DIK_NUMPAD9;
    m_aControlMap[CExoInput::KEYBOARD_NUMPAD0] = DIK_NUMPAD0;
    m_aControlMap[CExoInput::KEYBOARD_NUMPADMINUS] = DIK_SUBTRACT;
    m_aControlMap[CExoInput::KEYBOARD_NUMPADPLUS] = DIK_ADD;
    m_aControlMap[CExoInput::KEYBOARD_NUMPADDECIMAL] = DIK_DECIMAL;
    m_aControlMap[CExoInput::KEYBOARD_LEFTSHIFT] = DIK_LSHIFT;
    m_aControlMap[CExoInput::KEYBOARD_RIGHTSHIFT] = DIK_RSHIFT;
    m_aControlMap[CExoInput::KEYBOARD_LEFTALT] = DIK_LMENU;
    m_aControlMap[CExoInput::KEYBOARD_RIGHTALT] = DIK_RMENU;
    m_aControlMap[CExoInput::KEYBOARD_LEFTCTRL] = DIK_LCONTROL;
    m_aControlMap[CExoInput::KEYBOARD_RIGHTCTRL] = DIK_RCONTROL;
    m_aControlMap[CExoInput::KEYBOARD_TAB] = DIK_TAB;
    m_aControlMap[CExoInput::KEYBOARD_ESC] = DIK_ESCAPE;
    m_aControlMap[CExoInput::KEYBOARD_HOME] = DIK_HOME;
    m_aControlMap[CExoInput::KEYBOARD_END] = DIK_END;
    m_aControlMap[CExoInput::KEYBOARD_PAGEUP] = DIK_PRIOR;
    m_aControlMap[CExoInput::KEYBOARD_PAGEDOWN] = DIK_NEXT;
    m_aControlMap[CExoInput::KEYBOARD_INSERT] = DIK_INSERT;
    m_aControlMap[CExoInput::KEYBOARD_DELETE] = DIK_DELETE;
    m_aControlMap[CExoInput::KEYBOARD_PRINTSCREEN] = DIK_SYSRQ;
    m_aControlMap[CExoInput::KEYBOARD_F1] = DIK_F1;
    m_aControlMap[CExoInput::KEYBOARD_F2] = DIK_F2;
    m_aControlMap[CExoInput::KEYBOARD_F3] = DIK_F3;
    m_aControlMap[CExoInput::KEYBOARD_F4] = DIK_F4;
    m_aControlMap[CExoInput::KEYBOARD_F5] = DIK_F5;
    m_aControlMap[CExoInput::KEYBOARD_F6] = DIK_F6;
    m_aControlMap[CExoInput::KEYBOARD_F7] = DIK_F7;
    m_aControlMap[CExoInput::KEYBOARD_F8] = DIK_F8;
    m_aControlMap[CExoInput::KEYBOARD_F9] = DIK_F9;
    m_aControlMap[CExoInput::KEYBOARD_F10] = DIK_F10;
    m_aControlMap[CExoInput::KEYBOARD_F11] = DIK_F11;
    m_aControlMap[CExoInput::KEYBOARD_F12] = DIK_F12;
    m_aControlMap[CExoInput::KEYBOARD_F13] = DIK_F13;
    m_aControlMap[CExoInput::KEYBOARD_F14] = DIK_F14;
    m_aControlMap[CExoInput::KEYBOARD_F15] = DIK_F15;
    m_aControlMap[CExoInput::KEYBOARD_A] = DIK_A;
    m_aControlMap[CExoInput::KEYBOARD_B] = DIK_B;
    m_aControlMap[CExoInput::KEYBOARD_C] = DIK_C;
    m_aControlMap[CExoInput::KEYBOARD_D] = DIK_D;
    m_aControlMap[CExoInput::KEYBOARD_E] = DIK_E;
    m_aControlMap[CExoInput::KEYBOARD_F] = DIK_F;
    m_aControlMap[CExoInput::KEYBOARD_G] = DIK_G;
    m_aControlMap[CExoInput::KEYBOARD_H] = DIK_H;
    m_aControlMap[CExoInput::KEYBOARD_I] = DIK_I;
    m_aControlMap[CExoInput::KEYBOARD_J] = DIK_J;
    m_aControlMap[CExoInput::KEYBOARD_K] = DIK_K;
    m_aControlMap[CExoInput::KEYBOARD_L] = DIK_L;
    m_aControlMap[CExoInput::KEYBOARD_M] = DIK_M;
    m_aControlMap[CExoInput::KEYBOARD_N] = DIK_N;
    m_aControlMap[CExoInput::KEYBOARD_O] = DIK_O;
    m_aControlMap[CExoInput::KEYBOARD_P] = DIK_P;
    m_aControlMap[CExoInput::KEYBOARD_Q] = DIK_Q;
    m_aControlMap[CExoInput::KEYBOARD_R] = DIK_R;
    m_aControlMap[CExoInput::KEYBOARD_S] = DIK_S;
    m_aControlMap[CExoInput::KEYBOARD_T] = DIK_T;
    m_aControlMap[CExoInput::KEYBOARD_U] = DIK_U;
    m_aControlMap[CExoInput::KEYBOARD_V] = DIK_V;
    m_aControlMap[CExoInput::KEYBOARD_W] = DIK_W;
    m_aControlMap[CExoInput::KEYBOARD_X] = DIK_X;
    m_aControlMap[CExoInput::KEYBOARD_Y] = DIK_Y;
    m_aControlMap[CExoInput::KEYBOARD_Z] = DIK_Z;
    m_aControlMap[CExoInput::KEYBOARD_1] = DIK_1;
    m_aControlMap[CExoInput::KEYBOARD_2] = DIK_2;
    m_aControlMap[CExoInput::KEYBOARD_3] = DIK_3;
    m_aControlMap[CExoInput::KEYBOARD_4] = DIK_4;
    m_aControlMap[CExoInput::KEYBOARD_5] = DIK_5;
    m_aControlMap[CExoInput::KEYBOARD_6] = DIK_6;
    m_aControlMap[CExoInput::KEYBOARD_7] = DIK_7;
    m_aControlMap[CExoInput::KEYBOARD_8] = DIK_8;
    m_aControlMap[CExoInput::KEYBOARD_9] = DIK_9;
    m_aControlMap[CExoInput::KEYBOARD_0] = DIK_0;
    m_aControlMap[CExoInput::KEYBOARD_MINUS] = DIK_MINUS;
    m_aControlMap[CExoInput::KEYBOARD_EQUALS] = DIK_EQUALS;
    m_aControlMap[CExoInput::KEYBOARD_BACK] = DIK_BACK;
    m_aControlMap[CExoInput::KEYBOARD_LBRACKET] = DIK_LBRACKET;
    m_aControlMap[CExoInput::KEYBOARD_RBRACKET] = DIK_RBRACKET;
    m_aControlMap[CExoInput::KEYBOARD_SEMICOLON] = DIK_SEMICOLON;
    m_aControlMap[CExoInput::KEYBOARD_APOSTROPHE] = DIK_APOSTROPHE;
    m_aControlMap[CExoInput::KEYBOARD_GRAVE] = DIK_GRAVE;
    m_aControlMap[CExoInput::KEYBOARD_BACKSLASH] = DIK_BACKSLASH;
    m_aControlMap[CExoInput::KEYBOARD_COMMA] = DIK_COMMA;
    m_aControlMap[CExoInput::KEYBOARD_PERIOD] = DIK_PERIOD;
    m_aControlMap[CExoInput::KEYBOARD_SLASH] = DIK_SLASH;
    m_aControlMap[CExoInput::KEYBOARD_MULTIPLY] = DIK_MULTIPLY;
    m_aControlMap[CExoInput::KEYBOARD_NUMPADCOMMA] = DIK_NUMPADCOMMA;
    m_aControlMap[CExoInput::KEYBOARD_DIVIDE] = DIK_DIVIDE;
    m_aControlMap[CExoInput::KEYBOARD_NUMPADENTER] = DIK_NUMPADENTER;
    m_aControlMap[CExoInput::KEYBOARD_CAPSLOCK] = DIK_CAPITAL;
    m_aControlMap[CExoInput::KEYBOARD_PAUSE] = DIK_PAUSE;
    m_aControlMap[CExoInput::KEYBOARD_OEM_102] = DIK_OEM_102;

    m_pRawInput = new CExoRawInputInternal(hInstance, hWnd);
    if (m_pRawInput != NULL)
    {
        if (m_pRawInput->m_bInitialized == 0)
        {
            delete m_pRawInput;
            m_pRawInput = NULL;
        }
        else
        {
            int nDevices = m_pRawInput->m_nJoysticks + 2;
            m_nDevices = nDevices;
            if (m_pRawInput->m_nJoysticks > 4)
            {
                m_nDevices = CExoRawInputInternal::MAX_INPUT_DEVICES;
            }

            m_lstEvents.SetSize(1);
            m_lstEvents[0] = NULL;

            for (int i = 0; i < 6; i++)
            {
                m_aClasses[i].m_sName = "Unnamed Input Class";
                m_aClasses[i].m_lstMembers.SetSize(1);
                m_aClasses[i].m_lstMembers = CExoArrayList<unsigned long>();
            }

            m_nLastTime = (unsigned long)(
                g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000);
            m_nUnknown160 = 0;
            m_nMouseMode = 0;
            m_nMouseX = -1;
            m_nMouseY = -1;
            m_bMouseWarped = 0;
            m_nCursor = -1;
            m_nUnknown39C = 0;
            m_nLastSetMouseDownState = 0;
        }
    }
}
#endif
// PC 0x0062CE40
CExoInputInternal::~CExoInputInternal()
{
    if (m_pRawInput != NULL)
    {
        delete m_pRawInput;
        m_pRawInput = NULL;
    }

    int nEvents = m_lstEvents.num;
    for (int i = 0; i < nEvents; i++)
    {
        if (m_lstEvents.element[i] != NULL)
        {
            delete m_lstEvents.element[i];
        }
    }
    m_lstEvents.SetSize(0);

    CExoLinkedListNode* pPos = m_llRepeat.GetHeadPos();
    while (pPos != NULL)
    {
        delete m_llRepeat.GetAtPos(pPos);
        m_llRepeat.GetNext(pPos);
    }

#ifndef _XBOX
    while (m_llBuffered.GetHeadPos() != NULL)
    {
        CExoInputEvent* pEvent = m_llBuffered.RemoveHead();
        if (pEvent != NULL)
        {
            if (pEvent->m_pDesc != NULL)
            {
                delete pEvent->m_pDesc;
                pEvent->m_pDesc = NULL;
            }
            delete pEvent;
        }
    }
#endif
}
// PC 0x0062E630
CExoInputEventDesc* CExoInputInternal::GetEvent(unsigned long nEvent,
                                                int nEventClass)
{
    CExoInputEventDesc* pEvent = NULL;
#ifdef _XBOX
    if (nEventClass >= 0 && nEventClass < 6)
    {
        if (IsEventInClass(nEvent, nEventClass) != 0)
        {
            pEvent = m_lstEvents.element[nEvent];
        }
    }
#else
    if (nEventClass < 0 || nEventClass >= 6)
    {
        if (nEventClass == -1 && nEvent < (unsigned long)m_lstEvents.num)
        {
            pEvent = m_lstEvents.element[nEvent];
        }
    }
    else if (IsEventInClass(nEvent, nEventClass) != 0)
    {
        pEvent = m_lstEvents.element[nEvent];
    }
#endif
    return pEvent;
}
// PC 0x0062D040
int CExoInputInternal::IsEventInClass(unsigned long nEvent, int nInputClass)
{
    if (nInputClass >= 0 && nInputClass < 6)
    {
        int nOldSize = m_aClasses[nInputClass].m_lstMembers.num;
        if ((int)nEvent / 32 >= nOldSize)
        {
            m_aClasses[nInputClass].m_lstMembers.SetSize(
                2 * ((int)nEvent / 32) + 1);
            int nNewSize = m_aClasses[nInputClass].m_lstMembers.num;
            for (int i = nOldSize; i < nNewSize; i++)
            {
                m_aClasses[nInputClass].m_lstMembers.element[i] = 0;
            }
        }
        if ((1 << ((int)nEvent % 32)) &
            m_aClasses[nInputClass].m_lstMembers.element[(int)nEvent / 32])
        {
            return 1;
        }
    }
    return 0;
}
// PC 0x0062D0E0
int CExoInputInternal::CreateNewEvent(unsigned long nEvent, int nType,
                                      int nClass, unsigned long nControl,
                                      unsigned long nControl2)
{
#ifdef _XBOX
    if (nClass >= 4)
#else
    if (nClass >= 6)
#endif
    {
        return 0;
    }

    int nOldSize = m_lstEvents.num;
    if (nEvent >= (unsigned long)nOldSize)
    {
        m_lstEvents.SetSize(nEvent * 2 + 1);
        int nNewSize = m_lstEvents.num;
        for (int i = nOldSize; i < nNewSize; i++)
        {
            m_lstEvents.element[i] = NULL;
        }
    }

    if (m_lstEvents.element[nEvent] == NULL)
    {
        CExoInputEventDesc* pDesc = NULL;
        switch (nType)
        {
        case 3:
            pDesc = new CExoInputEventDescDetailed;
            pDesc->m_nType = nType;
            pDesc->m_nClass = nClass;
            pDesc->m_nEvent = nEvent;
            pDesc->m_nControl = nControl;
            break;
        case 0:
        case 1:
        case 2:
        case 5:
            pDesc = new CExoInputEventDesc;
            pDesc->m_nType = nType;
            pDesc->m_nClass = nClass;
            pDesc->m_nEvent = nEvent;
            pDesc->m_nControl = nControl;
            break;
        case 4:
        {
            CExoInputeventDesc2ButtonAxis* pPair = new CExoInputeventDesc2ButtonAxis;
            pPair->m_nType = nType;
            pPair->m_nClass = nClass;
            pPair->m_nEvent = nEvent;
            pPair->m_nControl = nControl;
            pPair->m_nControl2 = nControl2;
            pDesc = pPair;
        }
        break;
        }

        m_lstEvents.element[nEvent] = pDesc;
        return 1;
    }

    return 0;
}
// PC 0x0062D260
int CExoInputInternal::AddEvent(unsigned long nEvent, unsigned long nInputClass)
{
#ifdef _XBOX
    if (nInputClass < 6)
    {
        CExoInputEventDesc* pDesc = m_lstEvents.element[nEvent];
        if (pDesc == NULL)
        {
            return 0;
        }
        if (IsEventInClass(nEvent, nInputClass))
        {
            return 0;
        }
        if (pDesc->m_nControl != 0x6D &&
            IsControlUsed(pDesc->m_nControl, pDesc->m_nClass, nInputClass) != NULL)
        {
            return 0;
        }
        if (pDesc->Is2Button())
        {
            unsigned long nControl2 =
                ((CExoInputeventDesc2ButtonAxis*)pDesc)->m_nControl2;
            if (nControl2 != 0x6D &&
                IsControlUsed(nControl2, pDesc->m_nClass, nInputClass) != NULL)
            {
                return 0;
            }
        }

        pDesc->m_nUnknown8++;
        if (pDesc->m_nClass == CExoInput::INPUTDEVICE_NONE)
        {
            m_aClasses[nInputClass].m_llEvents.AddHead(&pDesc->m_nEvent);
        }
        else
        {
            m_aClasses[nInputClass].m_llDevices[pDesc->m_nClass].AddHead(
                &pDesc->m_nEvent);
            m_aClasses[nInputClass].m_lstMembers.element[(int)nEvent / 32] |=
                1 << ((int)nEvent % 32);
        }
        return 1;
    }
    return 0;
#else
    if (nInputClass < 6)
    {
        CExoInputEventDesc* pDesc = m_lstEvents.element[nEvent];
        if (pDesc != NULL && IsEventInClass(nEvent, nInputClass) == 0 &&
            (pDesc->m_nControl == 0x84 ||
             IsControlUsed(pDesc->m_nControl, pDesc->m_nClass, nInputClass) ==
                 NULL))
        {
            if (pDesc->Is2Button() != 0)
            {
                unsigned long nControl2 =
                    ((CExoInputeventDesc2ButtonAxis*)pDesc)->m_nControl2;
                if (nControl2 != 0x84 &&
                    IsControlUsed(nControl2, pDesc->m_nClass, nInputClass) !=
                        NULL)
                {
                    return 0;
                }
            }

            pDesc->m_nUnknown8++;
            if (pDesc->m_nClass == CExoInput::INPUTDEVICE_NONE)
            {
                m_aClasses[nInputClass].m_llEvents.AddHead(&pDesc->m_nEvent);
            }
            else
            {
                m_aClasses[nInputClass].m_llDevices[pDesc->m_nClass].AddHead(
                    &pDesc->m_nEvent);
                m_aClasses[nInputClass]
                    .m_lstMembers.element[(int)nEvent / 32] |=
                    1 << ((int)nEvent % 32);
            }
            return 1;
        }
    }
    return 0;
#endif
}
// PC 0x0062D370
int CExoInputInternal::RemoveEvent(unsigned long nEvent)
{
    CExoInputEventDesc* pDesc = m_lstEvents.element[nEvent];
    if (pDesc == NULL)
    {
        return -1;
    }
    m_lstEvents.element[nEvent] = NULL;

    int nClass = -1;
    for (int nTest = 0; nTest < 6; nTest++)
    {
        if (IsEventInClass(nEvent, nTest))
        {
            nClass = nTest;
            break;
        }
    }
    if (nClass == -1)
    {
        return -1;
    }

    CExoLinkedListNode* pPos =
        m_aClasses[nClass].m_llDevices[pDesc->m_nClass].GetHeadPos();
    while (pPos != NULL)
    {
        unsigned long nFound =
            *m_aClasses[nClass].m_llDevices[pDesc->m_nClass].GetAtPos(pPos);
        if (nFound = nEvent)
        {
            break;
        }
        m_aClasses[nClass].m_llDevices[pDesc->m_nClass].GetNext(pPos);
    }
    if (pPos == NULL)
    {
        return -1;
    }

    m_aClasses[nClass].m_llDevices[pDesc->m_nClass].Remove(pPos);
    m_aClasses[nClass].m_lstMembers.element[(int)nEvent / 32] &=
        0 << ((int)nEvent % 32);
    delete pDesc;
    return nClass;
}
// PC 0x0062D490
int CExoInputInternal::ScaleEvent(unsigned long nEvent, float fScale,
                                  float fExponent)
{
    CExoInputEventDesc* pDesc = m_lstEvents.element[nEvent];
    if (pDesc == NULL)
    {
        return 0;
    }

    if (pDesc->IsDetailed() != 0)
    {
        CExoInputEventDescDetailed* pScaled = (CExoInputEventDescDetailed*)pDesc;
        pScaled->m_fScale = fScale;
        pScaled->m_fExponent = fExponent;
        pScaled->m_bScaled = 1;
        return 1;
    }

    switch (pDesc->m_nType)
    {
    case 1:
    case 3:
    case 4:
        return 0;
    case 0:
    case 2:
    case 5:
    {
        CExoInputEventDescDetailed* pScaled =
            new CExoInputEventDescDetailed(*pDesc);
        pScaled->m_fScale = fScale;
        pScaled->m_fExponent = fExponent;
        pScaled->m_bScaled = 1;
        unsigned long nClass = RemoveEvent(nEvent);
        m_lstEvents.element[nEvent] = pScaled;
        AddEvent(nEvent, nClass);
        return 1;
    }
    }
    return 0;
}

#ifndef _XBOX
int CExoInputInternal::BufferEvent(void* pEvent)  // PC 0x0062D560
{
    MSG* pMessage = (MSG*)pEvent;
    int nDevice;
    unsigned long nControl;
    unsigned long nValue;

    switch (pMessage->message)
    {
    case WM_MOUSEMOVE:
        if (m_bMouseWarped != 0)
        {
            m_bMouseWarped = 0;
        }
        else
        {
            m_nMouseX = (short)LOWORD(pMessage->lParam);
            m_nMouseY = (short)HIWORD(pMessage->lParam);
        }
        return 1;

    case WM_LBUTTONDOWN:
        if (m_bMouseWarped == 0)
        {
            int nX = (short)LOWORD(pMessage->lParam);
            int nY = (short)HIWORD(pMessage->lParam);
            BufferEvent(CExoInput::INPUTDEVICE_MOUSE, CExoInput::MOUSE_XAXIS, nX);
            BufferEvent(CExoInput::INPUTDEVICE_MOUSE, CExoInput::MOUSE_YAXIS, nY);
            m_nMouseX = -1;
            m_nMouseY = -1;
        }
        else
        {
            m_bMouseWarped = 0;
        }
        nDevice = CExoInput::INPUTDEVICE_MOUSE;
        nValue = CExoInput::MOUSE_BUTTON_PRESSED;
        nControl = CExoInput::MOUSE_BUTTON0;
        break;

    case WM_LBUTTONDBLCLK:
        if (m_bMouseWarped == 0)
        {
            int nX = (short)LOWORD(pMessage->lParam);
            int nY = (short)HIWORD(pMessage->lParam);
            BufferEvent(CExoInput::INPUTDEVICE_MOUSE, CExoInput::MOUSE_XAXIS, nX);
            BufferEvent(CExoInput::INPUTDEVICE_MOUSE, CExoInput::MOUSE_YAXIS, nY);
            m_nMouseX = -1;
            m_nMouseY = -1;
        }
        else
        {
            m_bMouseWarped = 0;
        }
        nDevice = CExoInput::INPUTDEVICE_MOUSE;
        nValue = CExoInput::MOUSE_BUTTON_DOUBLE_CLICK;
        nControl = CExoInput::MOUSE_BUTTON0;
        break;

    case WM_LBUTTONUP:
        if (m_bMouseWarped == 0)
        {
            int nX = (short)LOWORD(pMessage->lParam);
            int nY = (short)HIWORD(pMessage->lParam);
            BufferEvent(CExoInput::INPUTDEVICE_MOUSE, CExoInput::MOUSE_XAXIS, nX);
            BufferEvent(CExoInput::INPUTDEVICE_MOUSE, CExoInput::MOUSE_YAXIS, nY);
            m_nMouseX = -1;
            m_nMouseY = -1;
        }
        else
        {
            m_bMouseWarped = 0;
        }
        nDevice = CExoInput::INPUTDEVICE_MOUSE;
        nValue = CExoInput::MOUSE_BUTTON_RELEASED;
        nControl = CExoInput::MOUSE_BUTTON0;
        break;

    case WM_RBUTTONDOWN:
        if (m_bMouseWarped == 0)
        {
            int nX = (short)LOWORD(pMessage->lParam);
            int nY = (short)HIWORD(pMessage->lParam);
            BufferEvent(CExoInput::INPUTDEVICE_MOUSE, CExoInput::MOUSE_XAXIS, nX);
            BufferEvent(CExoInput::INPUTDEVICE_MOUSE, CExoInput::MOUSE_YAXIS, nY);
            m_nMouseX = -1;
            m_nMouseY = -1;
        }
        else
        {
            m_bMouseWarped = 0;
        }
        nDevice = CExoInput::INPUTDEVICE_MOUSE;
        nValue = CExoInput::MOUSE_BUTTON_PRESSED;
        nControl = CExoInput::MOUSE_BUTTON1;
        break;

    case WM_RBUTTONDBLCLK:
        if (m_bMouseWarped == 0)
        {
            int nX = (short)LOWORD(pMessage->lParam);
            int nY = (short)HIWORD(pMessage->lParam);
            BufferEvent(CExoInput::INPUTDEVICE_MOUSE, CExoInput::MOUSE_XAXIS, nX);
            BufferEvent(CExoInput::INPUTDEVICE_MOUSE, CExoInput::MOUSE_YAXIS, nY);
            m_nMouseX = -1;
            m_nMouseY = -1;
        }
        else
        {
            m_bMouseWarped = 0;
        }
        nDevice = CExoInput::INPUTDEVICE_MOUSE;
        nValue = CExoInput::MOUSE_BUTTON_DOUBLE_CLICK;
        nControl = CExoInput::MOUSE_BUTTON1;
        break;

    case WM_RBUTTONUP:
        if (m_bMouseWarped == 0)
        {
            int nX = (short)LOWORD(pMessage->lParam);
            int nY = (short)HIWORD(pMessage->lParam);
            BufferEvent(CExoInput::INPUTDEVICE_MOUSE, CExoInput::MOUSE_XAXIS, nX);
            BufferEvent(CExoInput::INPUTDEVICE_MOUSE, CExoInput::MOUSE_YAXIS, nY);
            m_nMouseX = -1;
            m_nMouseY = -1;
        }
        else
        {
            m_bMouseWarped = 0;
        }
        nDevice = CExoInput::INPUTDEVICE_MOUSE;
        nValue = CExoInput::MOUSE_BUTTON_RELEASED;
        nControl = CExoInput::MOUSE_BUTTON1;
        break;

    case WM_MOUSEWHEEL:
        if (m_bMouseWarped == 0)
        {
            int nX = (short)LOWORD(pMessage->lParam);
            int nY = (short)HIWORD(pMessage->lParam);
            BufferEvent(CExoInput::INPUTDEVICE_MOUSE, CExoInput::MOUSE_XAXIS, nX);
            BufferEvent(CExoInput::INPUTDEVICE_MOUSE, CExoInput::MOUSE_YAXIS, nY);
            m_nMouseX = -1;
            m_nMouseY = -1;
        }
        else
        {
            m_bMouseWarped = 0;
        }
        nDevice = CExoInput::INPUTDEVICE_MOUSE;
        nValue = (short)HIWORD(pMessage->wParam);
        nControl = CExoInput::MOUSE_ZAXIS;
        break;

    case WM_MBUTTONDOWN:
        if (m_bMouseWarped == 0)
        {
            int nX = LOWORD(pMessage->lParam);
            int nY = HIWORD(pMessage->lParam);
            BufferEvent(CExoInput::INPUTDEVICE_MOUSE, CExoInput::MOUSE_XAXIS, nX);
            BufferEvent(CExoInput::INPUTDEVICE_MOUSE, CExoInput::MOUSE_YAXIS, nY);
            m_nMouseX = -1;
            m_nMouseY = -1;
        }
        else
        {
            m_bMouseWarped = 0;
        }
        nDevice = CExoInput::INPUTDEVICE_MOUSE;
        nValue = CExoInput::MOUSE_BUTTON_PRESSED;
        nControl = CExoInput::MOUSE_BUTTON2;
        break;

    case WM_MBUTTONUP:
        if (m_bMouseWarped == 0)
        {
            int nX = LOWORD(pMessage->lParam);
            int nY = HIWORD(pMessage->lParam);
            BufferEvent(CExoInput::INPUTDEVICE_MOUSE, CExoInput::MOUSE_XAXIS, nX);
            BufferEvent(CExoInput::INPUTDEVICE_MOUSE, CExoInput::MOUSE_YAXIS, nY);
            m_nMouseX = -1;
            m_nMouseY = -1;
        }
        else
        {
            m_bMouseWarped = 0;
        }
        nDevice = CExoInput::INPUTDEVICE_MOUSE;
        nValue = CExoInput::MOUSE_BUTTON_RELEASED;
        nControl = CExoInput::MOUSE_BUTTON2;
        break;

    default:
        return 0;
    }
    BufferEvent(nDevice, nControl, nValue);
    return 1;
}
// PC 0x0062CB80
void CExoInputInternal::BufferEvent(int nClass, unsigned long nControl,
                                    unsigned long nValue)
{
    CExoInputEvent* pData = new CExoInputEvent;
    pData->m_nValue = nValue;
    pData->m_nSequence = 0;
    pData->m_nTimeStamp = 0;
    pData->m_pDesc = new CExoInputEventDesc;
    pData->m_pDesc->m_nClass = nClass;
    pData->m_pDesc->m_nControl = nControl;
    pData->m_pDesc->m_nEvent = 0;
    pData->m_pDesc->m_nType = 0;
    m_llBuffered.AddHead(pData);
}
// PC 0x0062DA00
int CExoInputInternal::SetMouseMode(unsigned char nMode)
{
    if (nMode != m_nMouseMode)
    {
        m_nMouseMode = nMode;
        if (nMode == 1)
        {
            while (ShowCursor(FALSE) >= 0)
            {
            }
        }
        RefreshMouseCursor(0);
    }
    return 1;
}
// PC 0x0062CBF0
void CExoInputInternal::RefreshMouseCursor(int bMouseDown)
{
    int nCursor = m_nCursor;
    m_nCursor = -1;
    if (nCursor == -1)
    {
        nCursor = 1;
    }
    SetMouseCursor(nCursor, m_nUnknown39C, bMouseDown);
}
// PC 0x0062C310
int CExoInputInternal::SetMouseCursor(int nCursor, int nArg2, int bMouseDown)
{
    int nResource = nCursor;

    m_nLastSetMouseDownState = bMouseDown;
    if (bMouseDown != 0)
    {
        nResource = nCursor + (nCursor % 2);
    }

    if (m_nCursor != nResource)
    {
        m_nCursor = nCursor;
        if (m_nMouseMode == 0)
        {
            HCURSOR hCursor =
                LoadCursorA(g_hInst, MAKEINTRESOURCE((WORD)nResource));
            if (hCursor != NULL)
            {
                HCURSOR hOld = SetCursor(hCursor);
                if (hOld != hCursor)
                {
                    DestroyCursor(hOld);
                }
            }
        }
    }
    return nResource;
}
// PC 0x0062C380
void CExoInputInternal::HideMouse()
{
    CURSORINFO ci;

    ci.cbSize = sizeof(CURSORINFO);
    if (m_nMouseMode == 0)
    {
        if (GetCursorInfo(&ci) && (ci.flags & CURSOR_SHOWING) != 0)
        {
            while (ShowCursor(FALSE) >= 0)
            {
            }
        }
    }
}
// PC 0x0062C3C0
void CExoInputInternal::ShowMouse()
{
    CURSORINFO ci;

    ci.cbSize = sizeof(CURSORINFO);
    if (m_nMouseMode == 0)
    {
        if (GetCursorInfo(&ci) && (ci.flags & CURSOR_SHOWING) == 0)
        {
            while (ShowCursor(TRUE) < 0)
            {
            }
        }
    }
}
// PC 0x0062C400
void CExoInputInternal::GetMouseDelta(float* fX, float* fY)
{
    *fX = m_fMouseDeltaX;
    *fY = m_fMouseDeltaY;
}
// PC 0x0062C420
void CExoInputInternal::UpdateMouseDelta(float fElapsed)
{
    CExoMouseState state;

    memset(&state, 0, sizeof(CExoMouseState));

    if (m_pRawInput->GetMouseState(state) == 1)
    {
        m_fMouseDeltaX =
            max(-1.0f, min(1.0f, (float)-state.m_nX / fMaxMickies));
        m_fMouseDeltaY =
            max(-1.0f, min(1.0f, (float)-state.m_nY / fMaxMickies));
    }
    else
    {
        m_fMouseDeltaX = 0.0f;
        m_fMouseDeltaY = 0.0f;
    }
}
// PC 0x0062C540
int CExoInputInternal::GetLastCapturedKeyboardKey()
{
    CExoDeviceBuffer buffer;
    int nKey = 0;

    if (m_pRawInput != NULL && m_bCaptureKeyboardKeys != 0)
    {
        m_pRawInput->GetKeyboardBuffer(buffer);
        for (unsigned int i = 0; i < buffer.m_nEvents; i++)
        {
            CExoDeviceEvent* pEvent = &buffer.m_pEvents[i];
            if (pEvent->m_nData & KEYBOARD_KEY_DOWN)
            {
                for (int nControl = CExoInput::KEYBOARD_START;
                     nControl < CExoInput::JOYSTICK_START; nControl++)
                {
                    if (pEvent->m_nOffset == m_aControlMap[nControl])
                    {
                        nKey = nControl;
                        break;
                    }
                }
            }
        }
        return nKey;
    }
    return 0;
}
// PC 0x0062C630
CExoString CExoInputInternal::GetDisplayCharacterForKey(int nKey)
{
    return m_pRawInput->GetDisplayCharacterDirectInputValue(
        m_aControlMap[nKey]);
}
// PC 0x0062CC20
void CExoInputInternal::SetMousePos(int nX, int nY, bool bKeepWarpState)
{
    POINT pt;
    pt.x = nX;
    pt.y = g_nScreenHeight - nY - 1;
    ClientToScreen(g_hRenderWnd, &pt);
    SetCursorPos(pt.x, pt.y);
    if (!bKeepWarpState)
    {
        m_bMouseWarped = 1;
    }
    for (int i = 0; i < m_lstOutput.num; i++)
    {
        CExoInputEvent* pEvent = &m_lstOutput[i];
        if (pEvent != NULL)
        {
            CExoInputEventDesc* pDesc = pEvent->m_pDesc;
            if (pDesc != NULL)
            {
                if (pDesc->m_nControl == CExoInput::MOUSE_XAXIS &&
                    pDesc->IsDetailed() != 0)
                {
                    pDesc->m_nValue = nX;
                }
                if (pDesc->m_nControl == CExoInput::MOUSE_YAXIS &&
                    pDesc->IsDetailed() != 0)
                {
                    pDesc->m_nValue = nY;
                }
            }
        }
    }
}
#endif
// PC 0x0062E690
float CExoInputInternal::PollInput(unsigned long nEvent, int nEventClass)
{
    float fValue = 0.0f;
    if (nEventClass >= 0 && nEventClass < 6)
    {
        if (IsEventInClass(nEvent, nEventClass) == 0)
        {
            return 0.0f;
        }

        CExoInputEventDesc* pDesc = GetEvent(nEvent, nEventClass);
        CExoInputEventDescDetailed* pScaled = (CExoInputEventDescDetailed*)pDesc;
        switch (pDesc->m_nType)
        {
        case 1:
            return 0.0f;
        case 0:
        case 2:
        case 5:
            fValue = ScaledValue(pDesc, pDesc->m_nControl, pDesc->m_nValue);
            break;
        case 4:
            fValue =
                ScaledValue(pDesc, pDesc->m_nControl, pScaled->m_nRelative) -
                ScaledValue(pDesc,
                            ((CExoInputeventDesc2ButtonAxis*)pDesc)->m_nControl2,
                            ((CExoInputeventDesc2ButtonAxis*)pDesc)->m_nRelative2);
            break;
        case 3:
            fValue = ScaledValue(pDesc, pDesc->m_nControl,
                                 pScaled->m_nRelative - pDesc->m_nValue);
            pDesc->m_nValue = pScaled->m_nRelative;
            break;
        }

        if (pDesc->IsDetailed() != 0)
        {
            if (fValue < 0.0f)
            {
                fValue = pow(fValue, pScaled->m_fExponent);
                if (fValue > 0.0f)
                {
                    fValue = -fValue;
                }
            }
            else
            {
                fValue = pow(fValue, pScaled->m_fExponent);
            }
            fValue *= pScaled->m_fScale;
        }
    }
    return fValue;
}
// PC 0x0062E7B0
CExoArrayList<CExoInputEvent> CExoInputInternal::GetEvents(int& nEvents, int nEventClass)
{
    int nDevice = -1;
    CExoDeviceBuffer aBuffers[CExoRawInputInternal::MAX_INPUT_DEVICES];
    nEvents = 0;
    m_lstOutput.RemoveAll(0);
#ifdef _XBOX
    m_pRawInput->UpdateJoysticks();
    m_pRawInput->UpdateRumble();
    m_nDevices = m_pRawInput->m_nJoysticks;
#endif
    if (m_pRawInput == NULL)
        return CExoArrayList<CExoInputEvent>();
    if (nEventClass >= 6)
    {
#ifdef _XBOX
        g_pExoBase->m_pExoDebug->Warning(1381, "exoinputinternal.cpp",
#else
        g_pExoBase->m_pExoDebug->Warning(1483, "exoinputinternal.cpp",
#endif
            "CExoInputInternal::GetEvents() Invalid InputClass parameter");
        return CExoArrayList<CExoInputEvent>();
    }

    CExoLinkedListNode* apPositions[CExoRawInputInternal::MAX_INPUT_DEVICES];
    for (int i = 0; i < m_nDevices; ++i)
        apPositions[i] = m_aClasses[nEventClass].m_llDevices[i].GetHeadPos();
#ifdef _XBOX
    m_pRawInput->GetKeyboardBuffer(aBuffers[CExoInput::INPUTDEVICE_KEYBOARD]);
    m_pRawInput->GetMouseBuffer(aBuffers[CExoInput::INPUTDEVICE_MOUSE]);
    for (int i = 0; i < m_nDevices; ++i)
        m_pRawInput->GetJoystickBuffer(i, aBuffers[i]);
    if (m_pRawInput->GetCheatCombo(0))
        m_nUnknown160 = 1;

    int nBindControl;
    int nBindValue;
    int nBindSequence;
    int nBindTimeStamp;
    if (m_pRawInput->GetBindEvent(nBindControl, nBindValue, nBindSequence,
                                  nBindTimeStamp) &&
        m_nDevices > 0)
    {
        aBuffers[0].m_pEvents[aBuffers[0].m_nEvents].m_nOffset = nBindControl;
        aBuffers[0].m_pEvents[aBuffers[0].m_nEvents].m_nData = nBindValue;
        aBuffers[0].m_pEvents[aBuffers[0].m_nEvents].m_nSequence = nBindSequence;
        aBuffers[0].m_pEvents[aBuffers[0].m_nEvents].m_nTimeStamp = nBindTimeStamp;
        aBuffers[0].m_nEvents++;
    }
#else
    if (!m_bCaptureKeyboardKeys)
        m_pRawInput->GetKeyboardBuffer(aBuffers[CExoInput::INPUTDEVICE_KEYBOARD]);
    for (int i = 2; i < m_nDevices; ++i)
        m_pRawInput->GetJoystickBuffer(i - 2, aBuffers[i]);
#endif

    unsigned long anPositions[CExoRawInputInternal::MAX_INPUT_DEVICES];
    int bEmpty = 1;
    for (int i = 0; i < m_nDevices; ++i)
    {
        anPositions[i] = 0;
        if (aBuffers[i].GetNumEvents() > 0)
            bEmpty = 0;
    }
    int nTotal = 0;
    m_nOutputEvents = 0;
    for (int i = 0; i < m_nDevices; ++i)
        nTotal += aBuffers[i].GetNumEvents();
    int bHaveEvents = nTotal != 0 && !bEmpty;
    if (bHaveEvents)
    {
        unsigned long nSequence;
        int bDone;
        do
        {
            int bFound = 0;
            for (int i = 0; i < m_nDevices; ++i)
            {
                if ((nDevice == -1 || i == nDevice ||
                     anPositions[nDevice] >= aBuffers[nDevice].GetNumEvents()) && !bFound)
                {
                    while (!bFound && anPositions[i] < aBuffers[i].GetNumEvents())
                    {
                        CExoDeviceEvent* pEvent = &aBuffers[i].m_pEvents[anPositions[i]];
                        apPositions[i] = m_aClasses[nEventClass].m_llDevices[i].GetHeadPos();
                        if (apPositions[i] != NULL)
                        {
                            unsigned long* pEventID = m_aClasses[nEventClass].m_llDevices[i].GetHead();
                            do
                            {
                                CExoInputEventDesc* pDesc = m_lstEvents.element[*pEventID];
                                if (m_aControlMap[pDesc->m_nControl] == pEvent->m_nOffset)
                                {
                                    bFound = 1;
                                    break;
                                }
                                if (pDesc->Is2Button() &&
                                    m_aControlMap[((CExoInputeventDesc2ButtonAxis*)pDesc)->m_nControl2] == pEvent->m_nOffset)
                                {
                                    bFound = 1;
                                    break;
                                }
                                pEventID = m_aClasses[nEventClass].m_llDevices[i].GetNext(apPositions[i]);
                            } while (apPositions[i] != NULL);
                        }
                        if (!bFound)
                            ++anPositions[i];
                    }
                }
            }
            if (bFound)
            {
                int bSelected = 0;
                for (int i = 0; bSelected != TRUE && i < m_nDevices; ++i)
                {
                    if (anPositions[i] < aBuffers[i].GetNumEvents())
                    {
                        unsigned long nEventSequence;
                        if (nDevice == -1)
                            nEventSequence = aBuffers[i].m_pEvents[anPositions[i]].m_nSequence;
                        else
                        {
                            nEventSequence = aBuffers[i].m_pEvents[anPositions[i]].m_nSequence;
#ifdef _XBOX
                            if (nSequence != nEventSequence)
#else
                            if (!DISEQUENCE_COMPARE(nSequence, ==, nEventSequence))
#endif
                                continue;
                        }
                        nSequence = nEventSequence;
                        nDevice = i;
                        bSelected = 1;
                    }
                }
                if (!bSelected)
                {
                    ++nSequence;
                    nDevice = -1;
                }
                else
                {
                    CExoDeviceEvent* pEvent = &aBuffers[nDevice].m_pEvents[anPositions[nDevice]];
                    unsigned long nEvent = *m_aClasses[nEventClass].m_llDevices[nDevice].GetAtPos(apPositions[nDevice]);
                    CExoInputEventDesc* pDesc = m_lstEvents.element[nEvent];
                    if (pDesc->m_nCoolDown == 0)
                    {
                        switch (pDesc->m_nType)
                        {
                        case CExoInput::EVENT_TYPE_BUTTON:
                        {
                            if (m_pRawInput->IsDigital(nDevice, pEvent->m_nOffset))
                            {
                                CExoInputEvent cEvent;
                                cEvent.m_nSequence = pEvent->m_nSequence;
                                cEvent.m_nTimeStamp = pEvent->m_nTimeStamp;
                                cEvent.m_nValue = pEvent->m_nData;
                                cEvent.m_pDesc = pDesc;
                                pDesc->m_nValue = cEvent.m_nValue;
                                m_lstOutput.Add(cEvent);
                                ++m_nOutputEvents;
                            }
                            else
                            {
                                int nThreshold = m_pRawInput->GetControlThreshold(pEvent->m_nOffset);
                                int nValue;
                                if ((int)pEvent->m_nData < -nThreshold)
                                    nValue = -1;
                                else
                                    nValue = (int)pEvent->m_nData > nThreshold;
                                int nLast = m_pRawInput->GetLastState(nDevice, pEvent->m_nOffset);
                                nLast = nLast < -nThreshold ? -1 : nLast > nThreshold;
                                if (nValue != nLast)
                                {
                                    CExoInputEvent cEvent;
                                    cEvent.m_nSequence = pEvent->m_nSequence;
                                    cEvent.m_nTimeStamp = pEvent->m_nTimeStamp;
                                    cEvent.m_nValue = nValue;
                                    cEvent.m_pDesc = pDesc;
                                    pDesc->m_nValue = nValue;
                                    m_lstOutput.Add(cEvent);
                                    ++m_nOutputEvents;
                                }
                            }
                            break;
                        }
                        case CExoInput::EVENT_TYPE_RELATIVE:
                            ((CExoInputEventDescDetailed*)pDesc)->m_nRelative += pEvent->m_nData;
                            break;
                        case CExoInput::EVENT_TYPE_POLLED:
                        case CExoInput::EVENT_TYPE_STATE:
                        case CExoInput::EVENT_TYPE_VALUE:
                            pDesc->m_nValue = pEvent->m_nData;
                            break;
                        case CExoInput::EVENT_TYPE_PAIR:
                            if (pEvent->m_nOffset == m_aControlMap[pDesc->m_nControl])
                                ((CExoInputeventDesc2ButtonAxis*)pDesc)->m_nRelative = pEvent->m_nData;
                            else
                                ((CExoInputeventDesc2ButtonAxis*)pDesc)->m_nRelative2 = pEvent->m_nData;
                            break;
                        }
                    }
                    ++anPositions[nDevice];
                }
            }
            bDone = 1;
            for (int i = 0; i < m_nDevices; ++i)
            {
                if (anPositions[i] < aBuffers[i].GetNumEvents())
                    bDone = 0;
            }
        } while (!bDone);
    }

#ifndef _XBOX
    if (m_pRawInput != NULL && m_pRawInput->m_bActive)
    {
        if (m_nMouseX != -1)
            BufferEvent(CExoInput::INPUTDEVICE_MOUSE, CExoInput::MOUSE_XAXIS, m_nMouseX);
        if (m_nMouseY != -1)
            BufferEvent(CExoInput::INPUTDEVICE_MOUSE, CExoInput::MOUSE_YAXIS, m_nMouseY);
    }
    while (m_llBuffered.GetHeadPos() != NULL)
    {
        CExoInputEvent* pEvent = m_llBuffered.RemoveTail();
        CExoLinkedList<unsigned long>& llDevice =
            m_aClasses[nEventClass].m_llDevices[pEvent->m_pDesc->m_nClass];
        CExoLinkedListNode* pPosition = llDevice.GetHeadPos();
        while (pPosition != NULL)
        {
            unsigned long nEvent = *llDevice.GetAtPos(pPosition);
            CExoInputEventDesc* pDesc = m_lstEvents.element[nEvent];
            if ((pDesc->m_nControl & 0xFF) == pEvent->m_pDesc->m_nControl)
            {
                CExoInputEvent cEvent;
                cEvent.m_nSequence = pEvent->m_nSequence;
                cEvent.m_nTimeStamp = pEvent->m_nTimeStamp;
                cEvent.m_nValue = pEvent->m_nValue;
                cEvent.m_pDesc = pDesc;
                if (pDesc->IsDetailed())
                    ((CExoInputEventDescDetailed*)pDesc)->m_nRelative = cEvent.m_nValue;
                m_lstOutput.Add(cEvent);
                ++m_nOutputEvents;
                break;
            }
            llDevice.GetNext(pPosition);
        }
        delete pEvent->m_pDesc;
        delete pEvent;
    }
    m_nMouseX = -1;
    m_nMouseY = -1;
#endif

    unsigned long nTime = (unsigned long)(g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000);
    unsigned long nElapsed = nTime - m_nLastTime;
    m_nLastTime = nTime;
    CExoLinkedListNode* pPosition = m_llCoolDown.GetHeadPos();
    while (pPosition != NULL)
    {
        CExoInputEventDesc* pDesc = *m_llCoolDown.GetAtPos(pPosition);
        if (pDesc != NULL)
        {
            if (pDesc->m_nCoolDown <= nElapsed)
            {
                CExoLinkedListNode* pOldPosition = pPosition;
                pDesc->m_nCoolDown = 0;
                m_llCoolDown.GetNext(pPosition);
                m_llCoolDown.Remove(pOldPosition);
            }
            else
            {
                pDesc->m_nCoolDown -= nElapsed;
                m_llCoolDown.GetNext(pPosition);
            }
        }
        else
        {
            m_llCoolDown.GetNext(pPosition);
        }
    }

    pPosition = m_llRepeat.GetHeadPos();
    while (pPosition != NULL)
    {
        CExoInputRepeatEvent* pRepeat = m_llRepeat.GetAtPos(pPosition);
        if (pRepeat != NULL && IsEventInClass(pRepeat->m_nEvent, nEventClass))
        {
            CExoInputEventDesc* pDesc = m_lstEvents.element[pRepeat->m_nEvent];
            if (pDesc->m_nValue != 0)
            {
                if (pRepeat->m_nElapsed == 0 && !pRepeat->m_bRepeating)
                {
                    pRepeat->m_nElapsed = 1;
                    pRepeat->m_nHeldValue = pDesc->m_nValue;
                    pRepeat->m_nLastValue = pRepeat->m_nHeldValue;
                }
                else
                {
                    if (pRepeat->m_nHeldValue != pDesc->m_nValue)
                    {
                        pRepeat->m_nElapsed = 1;
                        pRepeat->m_nHeldValue = pDesc->m_nValue;
                        pRepeat->m_nLastValue = pRepeat->m_nHeldValue;
                        pRepeat->m_bRepeating = 0;
                        break;
                    }
                    pRepeat->m_nElapsed += nElapsed;
                    if (pRepeat->m_bRepeating)
                    {
                        if (pRepeat->m_nElapsed > pRepeat->m_nRate)
                        {
                            CExoInputEvent cEvent;
                            if (pRepeat->m_nLastValue == pRepeat->m_nHeldValue)
                            {
                                cEvent.m_nValue = 0;
                                pRepeat->m_nLastValue = 0;
                            }
                            else
                            {
                                cEvent.m_nValue = pRepeat->m_nHeldValue;
                                pRepeat->m_nLastValue = pRepeat->m_nHeldValue;
                            }
                            cEvent.m_nSequence = 0;
                            cEvent.m_nTimeStamp = 0;
                            cEvent.m_pDesc = pDesc;
                            m_lstOutput.Add(cEvent);
                            ++m_nOutputEvents;
                            pRepeat->m_nElapsed = 0;
                        }
                    }
                    else if (pRepeat->m_nElapsed > pRepeat->m_nDelay)
                    {
                        CExoInputEvent cEvent;
                        cEvent.m_nValue = 0;
                        cEvent.m_nTimeStamp = 0;
                        cEvent.m_nSequence = 0;
                        cEvent.m_pDesc = pDesc;
                        pRepeat->m_nLastValue = 0;
                        m_lstOutput.Add(cEvent);
                        ++m_nOutputEvents;
                        pRepeat->m_bRepeating = 1;
                        pRepeat->m_nElapsed = 0;
                    }
                }
            }
            else
            {
                pRepeat->m_nElapsed = 0;
                pRepeat->m_bRepeating = 0;
                pRepeat->m_nHeldValue = 0;
                pRepeat->m_nLastValue = 0;
            }
        }
        m_llRepeat.GetNext(pPosition);
    }
    nEvents = m_nOutputEvents;
    return m_lstOutput;
}
// PC 0x0062F240
void CExoInputInternal::ClearAllInputEventDescriptions()
{
    EXOINPUTINTERNAL_TRACE();
    for (int nClass = 0; nClass < 6; nClass++)
    {
        for (int nDevice = 0; nDevice < m_nDevices; nDevice++)
        {
            while (m_aClasses[nClass].m_llDevices[nDevice].GetHeadPos() != NULL)
            {
                m_aClasses[nClass].m_llDevices[nDevice].RemoveHead();
            }
        }
        for (int i = 0; i < m_aClasses[nClass].m_lstMembers.num; i++)
        {
            m_aClasses[nClass].m_lstMembers.element[i] = 0;
        }
    }
    m_lstEvents.num = 0;
}

static inline void ExoinputinternalTrace3()
{
    ExoinputinternalTrace2();
}

static inline void ExoinputinternalTrace2()
{
    ExoinputinternalTrace1();
}

static inline void ExoinputinternalTrace1()
{
}

#ifdef _XBOX

int CExoInputInternal::GetAutoCalInfo()
{
    return m_pRawInput->GetAutoCalInfo();
}

int CExoInputInternal::GetPortBinding()
{
    return m_pRawInput->GetPortBinding();
}

int CExoInputInternal::SetAutoCalInfo(unsigned long nFlags)
{
    return m_pRawInput->SetAutoCalInfo(nFlags);
}

int CExoInputInternal::SetPortBinding(unsigned long nBinding)
{
    return m_pRawInput->SetPortBinding(nBinding);
}

void CExoInputInternal::UpdateJoysticks()
{
    m_pRawInput->UpdateJoysticks();
}

int CExoInputInternal::GetCheatCombo(unsigned char nCombo)
{
    if (nCombo > 1)
    {
        return 0;
    }
    return (m_nUnknown160 & (1 << nCombo)) != 0;
}

int CExoInputInternal::GetDebugOption()
{
    if (m_pRawInput != NULL)
    {
        return m_pRawInput->GetDebugOption();
    }
    return 0;
}

void CExoInputInternal::EndAttemptedBind()
{
    m_pRawInput->m_bBindEnded = 1;
}
#endif

#ifdef _XBOX

float CExoInputInternal::ScaledValue(CExoInputEventDesc* pDesc,
                                     unsigned long nControl,
                                     unsigned long nValue)
{
    float fValue = (float)(int)nValue;
    float fResult = fValue;

    if (pDesc->IsDetailed() &&
        ((CExoInputEventDescDetailed*)pDesc)->m_bScaled != 0 && nValue != 0)
    {
        float fMax = m_pRawInput->GetMaxUseable(pDesc->m_nClass,
                                                m_aControlMap[nControl]);
        float fMin = m_pRawInput->GetMinUseable(pDesc->m_nClass,
                                                m_aControlMap[nControl]);
        float fScaled = ((float)fabs(fResult) - fMin - 1.0f) / (fMax - fMin);
        fResult = min(1.0f, fScaled) * ((fValue < 0.0f) ? -1.0f : 1.0f);
    }

    return fResult;
}
#endif

#ifdef _XBOX

CExoInputInternal::CExoInputInternal()
{
    m_bActive = 0;
    m_pRawInput = NULL;
    m_nOutputEvents = 0;
    m_bKeyboardMode = 0;

    m_aControlMap[CExoInput::JOYSTICK_BUTTON0] = GAMEPAD_ANALOG_BUTTON0;
    m_aControlMap[CExoInput::JOYSTICK_BUTTON1] = GAMEPAD_ANALOG_BUTTON0 + 1;
    m_aControlMap[CExoInput::JOYSTICK_BUTTON2] = GAMEPAD_ANALOG_BUTTON0 + 2;
    m_aControlMap[CExoInput::JOYSTICK_BUTTON3] = GAMEPAD_ANALOG_BUTTON0 + 3;
    m_aControlMap[CExoInput::JOYSTICK_BUTTON4] = GAMEPAD_ANALOG_BUTTON0 + 4;
    m_aControlMap[CExoInput::JOYSTICK_BUTTON5] = GAMEPAD_ANALOG_BUTTON0 + 5;
    m_aControlMap[CExoInput::JOYSTICK_BUTTON6] = GAMEPAD_ANALOG_BUTTON0 + 6;
    m_aControlMap[CExoInput::JOYSTICK_BUTTON7] = GAMEPAD_ANALOG_BUTTON0 + 7;
    m_aControlMap[CExoInput::JOYSTICK_BUTTON8] = GAMEPAD_DIGITAL_BUTTON0 + 4;
    m_aControlMap[CExoInput::JOYSTICK_BUTTON9] = GAMEPAD_DIGITAL_BUTTON0 + 5;
    m_aControlMap[CExoInput::JOYSTICK_BUTTON10] = GAMEPAD_DIGITAL_BUTTON0 + 2;
    m_aControlMap[CExoInput::JOYSTICK_BUTTON11] = GAMEPAD_DIGITAL_BUTTON0 + 3;
    m_aControlMap[CExoInput::JOYSTICK_BUTTON12] = GAMEPAD_DIGITAL_BUTTON0;
    m_aControlMap[CExoInput::JOYSTICK_BUTTON13] = GAMEPAD_DIGITAL_BUTTON0 + 1;
    m_aControlMap[CExoInput::JOYSTICK_BUTTON14] = GAMEPAD_DIGITAL_BUTTON0 + 6;
    m_aControlMap[CExoInput::JOYSTICK_BUTTON15] = GAMEPAD_DIGITAL_BUTTON0 + 7;
    m_aControlMap[CExoInput::JOYSTICK_XAXIS] = GAMEPAD_THUMB_LX;
    m_aControlMap[CExoInput::JOYSTICK_YAXIS] = GAMEPAD_THUMB_LY;
    m_aControlMap[CExoInput::JOYSTICK_SLIDER0] = GAMEPAD_THUMB_RY;
    m_aControlMap[CExoInput::JOYSTICK_SLIDER1] = GAMEPAD_THUMB_RX;

    m_pRawInput = new CExoRawInputInternal;
    if (m_pRawInput != NULL)
    {
        if (m_pRawInput->m_bInitialized == 0)
        {
            delete m_pRawInput;
            m_pRawInput = NULL;
        }
        else
        {
            int nDevices = m_pRawInput->m_nJoysticks;
            m_nDevices = nDevices;

            m_lstEvents.SetSize(1);
            m_lstEvents[0] = NULL;

            for (int i = 0; i < 6; i++)
            {
                m_aClasses[i].m_sName = "Unnamed Input Class";
                m_aClasses[i].m_lstMembers.SetSize(1);
                m_aClasses[i].m_lstMembers = CExoArrayList<unsigned long>();
            }

            m_nLastTime = (unsigned long)(
                g_pExoBase->m_pExoTimers->GetHighResolutionTimer() / 1000);
            m_nUnknown160 = 0;
        }
    }
}
#endif

#ifdef _XBOX

void CExoInputInternal::SetEventNonRepeatable(unsigned long nEvent)
{
    CExoInputEventDesc* pDesc = m_lstEvents.element[nEvent];
    if (pDesc != NULL && pDesc->m_nType == 1)
    {
        CExoLinkedListNode* pNode = m_llRepeat.GetHeadPos();
        while (pNode != NULL)
        {
            CExoInputRepeatEvent* pRepeat =
                (CExoInputRepeatEvent*)m_llRepeat.GetAtPos(pNode);
            if (pRepeat != NULL && pRepeat->m_nEvent == nEvent)
            {
                m_llRepeat.Remove(pNode);
                delete pRepeat;
                return;
            }
            m_llRepeat.GetNext(pNode);
        }
    }
}
#endif
