#ifndef __SWMAIN_H__
#define __SWMAIN_H__

#if defined(_WIN32) && !defined(_XBOX)
#include <windows.h>
#else
#include "platform.h"
#endif

#define WM_MOVIE_WINDOWPOS_CHANGING (WM_USER + 7)
#define WM_MOVIE_WINDOWPOS_CHANGED (WM_USER + 8)
#define WM_MOVIE_MODE_ACTIVATE (WM_USER + 9)
#define WM_MOVIE_RESTORE_RESOLUTION (WM_USER + 10)

extern int g_bMovieResolutionLocked;

extern int g_bAllowWindowedMode;

extern int enablemodelrequest;

extern int g_bShowIndicator;

extern float DeltaT;
extern int g_bRenderWindowActive;

extern int g_bDisableSound;

extern int g_bEnableCombatDebugging;

extern int NPCTurbo;

extern int g_nHackAlignment;
enum { HACK_ALIGNMENT_DISABLED = -1 };

void UpdateScreen(float fElapsed, int bRunWorld, int bSkipRender);
void SetWindowTitle();
#ifdef _XBOX
void messagepump(int nUnused = 0);
#else
void messagepump();
#endif
void SetClearColor(char* sColor);
#ifndef _XBOX
const char* DumpLogCounters(char* sBuffer);
#endif
class CExoString;
extern int g_fullScreenDesired;
extern CExoString g_sBuildNumber;
extern int g_bDo3DGUIs;
void HandleLoadError(unsigned long nError);
extern float displayFPS;
extern HINSTANCE g_hInst;
extern int g_nScreenWidth;
extern int g_nScreenHeight;
extern HWND g_hRenderWnd;
extern HWND g_hWnd;
extern CExoString g_sGameName;
extern CExoString g_sVersionNumber;
extern CExoString g_sCopyrightInfo;
extern int g_bSwappingBuffers;
extern int g_bDisableSound;

#ifdef _XBOX
char* DumpLogCounters(char* sBuffer);
#endif

#endif
