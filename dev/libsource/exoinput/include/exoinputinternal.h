#ifndef EXOINPUTINTERNAL_H
#define EXOINPUTINTERNAL_H

class CExoFile;
#if defined(_WIN32) && !defined(_XBOX)
#include <windows.h>
#else
#include "platform.h"
#endif
#include "exobase.h"
#include "exoinput.h"

class CExoInputEventList;
class CExoRawInputInternal;

class CExoInputEventDesc
{
public:
    CExoInputEventDesc()
    {
        m_nValue = 0;
        m_nUnknown8 = 0;
        m_nCoolDown = 0;
    }

    CExoInputEventDesc(const CExoInputEventDesc& desc)
    {
        m_nValue = desc.m_nValue;
        m_nEvent = desc.m_nEvent;
        m_nType = desc.m_nType;
        m_nClass = desc.m_nClass;
        m_nControl = desc.m_nControl;
        m_nUnknown8 = desc.m_nUnknown8;
        m_nCoolDown = desc.m_nCoolDown;
    }
    // PC 0x004065A0
    virtual int IsDetailed()
    {
        return 0;
    }
    // PC 0x004065A0
    virtual int Is2Button()
    {
        return 0;
    }

    int m_nValue;
    int m_nUnknown8;
    unsigned long m_nCoolDown;
    unsigned long m_nEvent;
    int m_nType;
    int m_nClass;
    unsigned long m_nControl;
};

class CExoInputEventDescDetailed : public CExoInputEventDesc
{
public:
    CExoInputEventDescDetailed();
    CExoInputEventDescDetailed(const CExoInputEventDesc& desc);
    // PC 0x0065D0F0
    virtual int IsDetailed()
    {
        return 1;
    }

    int m_bScaled;
    int m_nRelative;
    float m_fScale;
    float m_fExponent;
};

class CExoInputeventDesc2ButtonAxis : public CExoInputEventDescDetailed
{
public:
    CExoInputeventDesc2ButtonAxis();
    // PC 0x0065D0F0
    virtual int Is2Button()
    {
        return 1;
    }

    unsigned long m_nControl2;
    int m_nRelative2;
};

class CExoInputEvent
{
public:
    unsigned long m_nValue;
    unsigned long m_nTimeStamp;
    unsigned long m_nSequence;
    CExoInputEventDesc* m_pDesc;
};

struct CExoInputRepeatEvent
{
    unsigned long m_nEvent;
    unsigned long m_nDelay;
    unsigned long m_nRate;
    unsigned long m_nElapsed;
    unsigned long m_nHeldValue;
    unsigned long m_nLastValue;
    int m_bRepeating;
};

class CExoInputClass
{
public:
    CExoInputClass()  // PC 0x0062CD40
        : m_lstMembers(0)
    {
    }

    CExoString m_sName;
    CExoLinkedList<unsigned long> m_llEvents;
#ifdef _XBOX
    CExoLinkedList<unsigned long> m_llDevices[4];
#else
    CExoLinkedList<unsigned long> m_llDevices[6];
#endif
    CExoArrayList<unsigned long> m_lstMembers;
};

class CExoInputInternal
{
public:
    virtual ~CExoInputInternal();
    virtual void Configure();

#ifdef _XBOX
    CExoInputInternal();
#else
    CExoInputInternal(HINSTANCE hInstance, HWND hWnd);
#endif

    CExoInputEventDesc* GetEvent(unsigned long nEvent, int nEventClass);
    float PollInput(unsigned long nEvent, int nEventClass);
    void ClearEvents();
    void EndAttemptedBind();
    void UpdateJoysticks();
    void WriteEventToFile(unsigned long nTimeStamp, int nEvent,
                          CExoFile* pFile);
    int GetActive();
    void SetActive(int bActive);
#ifndef _XBOX
    void SetMousePos(int nX, int nY, bool bKeepWarpState);
#endif
    CExoArrayList<CExoInputEvent> GetEvents(int& nEvents, int nEventClass);
#ifndef _XBOX
    CExoString GetDisplayCharacterForKey(int nKey);
#endif

    int CreateNewEvent(unsigned long nArg1, int nArg2, int nArg3,
                       unsigned long nArg4, unsigned long nArg5);
    int AddEvent(unsigned long nEvent, unsigned long nInputClass);
    int ScaleEvent(unsigned long nArg1, float fArg2, float fArg3);
    int SetEventRepeatable(unsigned long nArg1, unsigned long nArg2,
                           unsigned long nArg3);
    void ClearEventRepeat(unsigned long nArg1);
    void CoolDownEvent(unsigned long nArg1, unsigned long nArg2);
    void KeyboardModeOn();
    void KeyboardModeOff();
#ifndef _XBOX
    int BufferEvent(void* pArg1);
    int SetMouseMode(unsigned char cArg1);
    void RefreshMouseCursor(int nArg1);
    int SetMouseCursor(int nArg1, int nArg2, int nArg3);
    void HideMouse();
    void ShowMouse();
#endif
    void ClearAllInputEventDescriptions();
#ifndef _XBOX
    void GetMouseDelta(float* fArg1, float* fArg2);
    void UpdateMouseDelta(float fArg1);
    int IsModalKeyDown_Shift();
    int IsModalKeyDown_Ctrl();
    int IsModalKeyDown_Alt();
    int GetLastCapturedKeyboardKey();
#else

    void SetEventNonRepeatable(unsigned long nEvent);
    int GetAutoCalInfo();
    int GetPortBinding();
    int SetAutoCalInfo(unsigned long nFlags);
    int SetPortBinding(unsigned long nBinding);
    int GetCheatCombo(unsigned char nCombo);
    int GetDebugOption();
#endif
    int SetRumble(int nArg1, float fArg2, float fArg3, unsigned long nArg4);
    int GetMainDeviceLost();
    void PauseRumble();
    void UnpauseRumble();

    float ScaledValue(CExoInputEventDesc* pDesc, unsigned long nControl,
                      unsigned long nValue);
    CExoInputEventDesc* IsControlUsed(unsigned long nControl, int nDevice,
                                      int nInputClass);
#ifndef _XBOX
    void BufferEvent(int nClass, unsigned long nControl, unsigned long nValue);
#endif
    int IsEventInClass(unsigned long nEvent, int nInputClass);
    int RemoveEvent(unsigned long nEvent);

    int m_bKeyboardMode;
    CExoInputClass m_aClasses[6];
    CExoArrayList<CExoInputEventDesc*> m_lstEvents;
    CExoLinkedList<CExoInputEventDesc*> m_llCoolDown;
    CExoLinkedList<CExoInputRepeatEvent> m_llRepeat;
    unsigned long m_nLastTime;
    CExoRawInputInternal* m_pRawInput;
    CExoArrayList<CExoInputEvent> m_lstOutput;
    int m_nOutputEvents;
    int m_bActive;
    int m_nDevices;
    CExoCriticalSection m_csInput;
    int m_nUnknown160;
#ifdef _XBOX
    unsigned long m_aControlMap[109];
#else
    unsigned long m_aControlMap[132];
    CExoLinkedList<CExoInputEvent> m_llBuffered;
    unsigned char m_nMouseMode;
    int m_nMouseX;
    int m_nMouseY;
    int m_bMouseWarped;
    int m_nUnknown388;
    int m_nUnknown38C;
    int m_nUnknown390;
    int m_nUnknown394;
    int m_nCursor;
    int m_nUnknown39C;
    float m_fMouseDeltaX;
    float m_fMouseDeltaY;
    int m_bCaptureKeyboardKeys;
    int m_nLastSetMouseDownState;
#endif
};

#endif
