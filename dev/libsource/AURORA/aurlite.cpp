#include "aurora.h"
#include <aurlist.h>
#include <stdio.h>
#include <string.h>

float DeltaT = 0.1f;

float ViewAngle = 45.0f;
float clipnear = 0.1f;
float clipfar = 50.0f;
int WindowActive = 1;

float GlobalTime;
float FPS;

#ifndef _XBOX
int leftbutton;
int rightbutton;
int mousex;
int mousey;
int inputactive;
PAINTSTRUCT paint;
int inputlength;
char inputline[500];

LRESULT CALLBACK AurLiteWndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
    switch (nMsg)
    {
    case WM_PAINT:
        BeginPaint(hWnd, &paint);
        EndPaint(hWnd, &paint);
        return 0;
    case WM_DESTROY:
    case WM_CLOSE:
        PostQuitMessage(0);
        return 0;
    case WM_CHAR:
        if (inputactive)
        {
            if (wParam == '\r' || wParam == '\n')
                inputactive = 0;
            else if (inputlength < (int)sizeof(inputline) - 1)
            {
                inputline[inputlength++] = (char)wParam;
                inputline[inputlength] = 0;
            }
            return 0;
        }
        if (wParam == '\r' || wParam == '\n')
        {
            inputactive = 1;
            inputlength = 0;
            inputline[0] = 0;
            return 0;
        }
        if (wParam == VK_ESCAPE)
            PostQuitMessage(0);
        return 0;
    case WM_MOUSEMOVE:
        mousex = (short)LOWORD(lParam);
        mousey = (short)HIWORD(lParam);
        return 0;
    case WM_LBUTTONDOWN:
        SetCapture(hWnd);
        leftbutton = 1;
        return 0;
    case WM_RBUTTONDOWN:
        SetCapture(hWnd);
        rightbutton = 1;
        return 0;
    case WM_LBUTTONUP:
        leftbutton = 0;
        if (!rightbutton)
            ReleaseCapture();
        return 0;
    case WM_RBUTTONUP:
        rightbutton = 0;
        if (!leftbutton)
            ReleaseCapture();
        return 0;
    }
    return DefWindowProcA(hWnd, nMsg, wParam, lParam);
}

void CalcFPSDeltaT()
{
    static int bInitialized = 0;
    static unsigned long nStartTime;
    static unsigned long nNextStartTime;
    static unsigned long nCurrentTime;
    static unsigned long nLastTime;
    static int nFrames;
    static int nNextFrames;

    if (!bInitialized)
    {
        nFrames = 0;
        nStartTime = nLastTime = timeGetTime();
        bInitialized = 1;
    }
    nFrames++;
    nNextFrames++;
    nCurrentTime = timeGetTime();

    double fElapsed = (int)(nCurrentTime - nStartTime) / 1000.0;
    double fFPS;
    if (fElapsed != 0.0)
        fFPS = nFrames / fElapsed;
    else
        fFPS = -1.0;
    if (fElapsed < 0.0 || (fElapsed > 2.0 && nFrames > 10))
    {
        nStartTime = nNextStartTime;
        nFrames = nNextFrames;
        nNextStartTime = timeGetTime();
        nNextFrames = 0;
    }

    DeltaT = (int)(nCurrentTime - nLastTime) / 1000.0f;
    if (nLastTime == nCurrentTime)
        DeltaT = 0.002f;
    FPS = (float)fFPS;
    nLastTime = nCurrentTime;
    GlobalTime += DeltaT;
}
#endif

class Binding;
class TimeTrigger;

List<Binding*> bindings;
List<TimeTrigger*> timetriggers;

char* keysdown()
{
    static char sKeys[256];
    sKeys[0] = 0;
#ifndef _XBOX
    for (int nKey = 0; nKey < 256; nKey++)
    {
        if (GetAsyncKeyState(nKey))
            sprintf(sKeys + strlen(sKeys), "%d(%c) ", nKey, (char)nKey);
    }
#endif
    return sKeys;
}
#ifdef _XBOX

class Binding
{
public:
    Binding();
    ~Binding();

    char m_cKey;
    char m_sCommand[256];
    int m_bRepeat;
    int m_bDown;
};

class TimeTrigger
{
public:
    TimeTrigger(float fTime, char* sCommand);

    float m_fTime;
    char m_sCommand[256];
};

Binding* FindBinding(char cKey)
{
    for (int i = 0; i < bindings.m_nCount; i++)
    {
        if (bindings[i]->m_cKey == cKey)
            return bindings[i];
    }
    return 0;
}

Binding::Binding()
{
    m_cKey = 0;
    m_sCommand[0] = 0;
    bindings.Add(this);
    m_bRepeat = 0;
    m_bDown = 0;
}

Binding::~Binding()
{
    bindings.Remove(this);
}

TimeTrigger::TimeTrigger(float fTime, char* sCommand)
{
    m_fTime = fTime;
    strcpy(m_sCommand, sCommand ? sCommand : "");
    int i;
    for (i = 0; i < timetriggers.m_nCount; i++)
    {
        if (!(m_fTime > timetriggers[i]->m_fTime))
            break;
    }
    timetriggers.Insert(this, i);
}

char* at(char* sArguments)
{
    static char sResult[256];
    float fBase = 0.0f;
    float fTime = 0.0f;
    int nRead;
    char sTime[64];
    char* sNumber = sTime;
    sTime[0] = 0;
    sscanf(sArguments, "%s%n", sNumber, &nRead);
    if (!sTime[0])
    {
        sprintf(sResult, "%d time triggers", timetriggers.m_nCount);
        return sResult;
    }
    sArguments += nRead;
    while (*sArguments == ' ' || *sArguments == '\t' || *sArguments == '\r' || *sArguments == '\n')
        sArguments++;
    if (sTime[0] == '+')
    {
        sNumber = sTime + 1;
        fBase = GlobalTime;
    }
    if (sscanf(sNumber, "%f", &fTime) == 1)
    {
        fTime += fBase;
        if (!*sArguments)
        {
            int nCount = timetriggers.m_nCount;
            if (!nCount)
                return "no active timetriggers";
            TimeTrigger* pNearest = timetriggers[0];
            for (int i = 0; i < nCount; i++)
            {
                if (fabs(fTime - timetriggers[i]->m_fTime) < fabs(fTime - pNearest->m_fTime))
                    pNearest = timetriggers[i];
            }
            sprintf(sResult, "At %g: \"%s\"", pNearest->m_fTime, pNearest->m_sCommand);
            return sResult;
        }
        new TimeTrigger(fTime, sArguments);
        sprintf(sResult, "Added at %g: \"%s\"", fTime, sArguments);
        return sResult;
    }
    return "only time triggers supported at this time";
}

char* bind(char* sArguments)
{
    static char sResult[256];
    Binding* pBinding = 0;
    int bRepeat = 0;
    int nRead;
    char sKey[64];
    sKey[0] = 0;
    sResult[0] = 0;
    sscanf(sArguments, "%s%n", sKey, &nRead);
    if (!sKey[0])
    {
        sprintf(sResult, "%d bindings: ", bindings.m_nCount);
        for (int i = 0; i < bindings.m_nCount; i++)
            sprintf(sResult + strlen(sResult), " %c(%d)", bindings[i]->m_cKey, bindings[i]->m_cKey);
    }
    else
    {
        sArguments += nRead;
        while (*sArguments == ' ' || *sArguments == '\t')
            sArguments++;
        if (*sArguments == '+')
        {
            bRepeat = 1;
            sArguments++;
        }
        char cKey = sKey[0];
        if (cKey == '\\')
        {
            int nKey = 0;
            sscanf(sKey + 1, "%d", &nKey);
            cKey = (char)nKey;
        }
        else
        {
            if (strlen(sKey) != 1)
                return "sorry, only basic alpha chars at this time";
            if (cKey >= 'a' && cKey <= 'z')
            {
                cKey -= 32;
                sKey[0] = cKey;
            }
        }
        pBinding = FindBinding(cKey);
        if (!*sArguments)
        {
            if (!pBinding)
                sprintf(sResult, "%s has no binding", sKey);
            else
                sprintf(sResult, "%s bound to %s (%s)", sKey, pBinding->m_sCommand, pBinding->m_bRepeat ? "repeat" : "once");
        }
        else
        {
            char sFirst[64];
            sFirst[0] = 0;
            sscanf(sArguments, "%s", sFirst);
            if (!stricmp(sFirst, "0") || !stricmp(sFirst, "NULL"))
            {
                if (!pBinding)
                    sprintf(sResult, "%s didn't have a binding anyway", sKey);
                else
                {
                    sprintf(sResult, "%s no longer bound to %s", sKey, pBinding->m_sCommand);
                    delete pBinding;
                }
            }
            else
            {
                sprintf(sResult, "%s binding for %s \"%s\" %s", pBinding ? "changing" : "adding", sKey, sArguments, bRepeat ? "repeat" : "once");
                if (!pBinding)
                    pBinding = new Binding;
                pBinding->m_cKey = cKey;
                pBinding->m_bRepeat = bRepeat;
                strcpy(pBinding->m_sCommand, sArguments);
            }
        }
    }
    return sResult;
}
#endif
