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
        m_nInputClassCount = 0;
        m_nCoolDown = 0;
    }

    CExoInputEventDesc(CExoInputEventDesc* pDesc)
    {
        m_nValue = pDesc->m_nValue;
        m_nEvent = pDesc->m_nEvent;
        m_nType = pDesc->m_nType;
        m_nClass = pDesc->m_nClass;
        m_nControl = pDesc->m_nControl;
        m_nInputClassCount = pDesc->m_nInputClassCount;
        m_nCoolDown = pDesc->m_nCoolDown;
    }
    // PC 0x004065A0 GOG 0x00406570
    virtual int IsDetailed()
    {
        return 0;
    }
    // PC 0x004065A0 GOG 0x00406570
    virtual int Is2Button()
    {
        return 0;
    }

    int m_nValue;
    int m_nInputClassCount;
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
    CExoInputEventDescDetailed(CExoInputEventDesc* pDesc);
    // PC 0x0065D0F0 GOG 0x0065CF00
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
    // PC 0x0065D0F0 GOG 0x0065CF00
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
    CExoInputClass()  // PC 0x0062CD40 GOG 0x0062CC30
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

    int CreateNewEvent(unsigned long nEvent, int nType, int nClass,
                       unsigned long nControl, unsigned long nControl2);
    int AddEvent(unsigned long nEvent, unsigned long nInputClass);
    int ScaleEvent(unsigned long nEvent, float fScale, float fExponent);
    int SetEventRepeatable(unsigned long nEvent, unsigned long nDelay,
                           unsigned long nRate);
    void ClearEventRepeat(unsigned long nEvent);
    void CoolDownEvent(unsigned long nEvent, unsigned long nCoolDown);
    void KeyboardModeOn();
    void KeyboardModeOff();
#ifndef _XBOX
    int BufferEvent(void* pEvent);
    int SetMouseMode(unsigned char nMode);
    void RefreshMouseCursor(int bMouseDown);
    int SetMouseCursor(int nCursor, int bCentered, int bMouseDown);
    void HideMouse();
    void ShowMouse();
#endif
    void ClearAllInputEventDescriptions();
#ifndef _XBOX
    void GetMouseDelta(float* pfX, float* pfY);
    void UpdateMouseDelta(float fElapsed);
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
    int SetRumble(int nJoystick, float fLeft, float fRight, unsigned long nDuration);
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
    int m_nCheatCombos;
#ifdef _XBOX
    unsigned long m_aControlMap[109];
#else
    unsigned long m_aControlMap[132];
    CExoLinkedList<CExoInputEvent> m_llBuffered;
    unsigned char m_nMouseMode;
    int m_nMouseX;
    int m_nMouseY;
    int m_bMouseWarped;
    int m_nMouseMinX;
    int m_nMouseMinY;
    int m_nMouseMaxX;
    int m_nMouseMaxY;
    int m_nCursor;
    int m_bCursorCentered;
    float m_fMouseDeltaX;
    float m_fMouseDeltaY;
    int m_bCaptureKeyboardKeys;
    int m_nLastSetMouseDownState;
#endif
};

#endif
