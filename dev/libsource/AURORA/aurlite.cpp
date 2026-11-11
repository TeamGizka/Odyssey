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
