#ifndef __AURINTERNAL_H__
#define __AURINTERNAL_H__

#if defined(_WIN32) && !defined(_XBOX)
#include <windows.h>
#else
#include "platform.h"
#endif

#include "aurora.h"

class CAurInternalGL;

class CAurora
{
public:
    virtual ~CAurora();
    virtual int FullScreen() = 0;
    virtual int Initialize(HWND hApplicationWindow, HWND hRenderWindow,
                           unsigned long nWidth, unsigned long nHeight,
                           unsigned char nBitsPerPixel,
                           unsigned long nRefreshRate, int nUnknown0,
                           int nUnknown1, int nUnknown2) = 0;
    virtual bool GetValidMode(unsigned long* pnWidth, unsigned long* pnHeight,
                              unsigned char* pnBitsPerPixel,
                              unsigned long* pnRefreshRate, bool bSetMode) = 0;
    virtual int InitEditor(HWND hApplicationWindow, HWND hRenderWindow,
                           unsigned long nWidth, unsigned long nHeight,
                           unsigned char nBitsPerPixel,
                           unsigned long nRefreshRate, int bFullScreen) = 0;
    virtual void Reshape(int nWidth, int nHeight) = 0;
    virtual void RestoreDesktopDisplayMode() = 0;
    virtual void RestoreAuroraDisplayMode() = 0;
    virtual int ShutDown(int bReleaseRenderer, int bDestroyWindow) = 0;
    virtual int ResetGammaRamp(float fGamma, int bSaveGamma) = 0;
};

class CAurInternal : public CAurora
{
public:
    CAurInternal();
    virtual ~CAurInternal();

    virtual int FullScreen();
    virtual int Initialize(HWND hApplicationWindow, HWND hRenderWindow,
                           unsigned long nWidth, unsigned long nHeight,
                           unsigned char nBitsPerPixel,
                           unsigned long nRefreshRate, int nUnknown0,
                           int nUnknown1, int nUnknown2) = 0;
    virtual bool GetValidMode(unsigned long* pnWidth, unsigned long* pnHeight,
                             unsigned char* pnBitsPerPixel,
                             unsigned long* pnRefreshRate, bool bSetMode);
    virtual int InitEditor(HWND hApplicationWindow, HWND hRenderWindow,
                           unsigned long nWidth, unsigned long nHeight,
                           unsigned char nBitsPerPixel,
                           unsigned long nRefreshRate, int bFullScreen) = 0;
    virtual void Reshape(int nWidth, int nHeight) = 0;
    virtual void RestoreDesktopDisplayMode();
    virtual void RestoreAuroraDisplayMode();
    virtual int ShutDown(int bReleaseRenderer, int bDestroyWindow) = 0;
    virtual int ResetGammaRamp(float fGamma, int bSaveGamma);
    virtual int GetGLContext();
#ifdef _XBOX

    virtual void MakeCurrentGLContext(HDC hDeviceContext, HGLRC hRenderContext) = 0;
    virtual void SwapBuffers() = 0;
#endif

    static void DestroyCAuroraInstance();
    static CAurInternalGL* NewCAuroraInstance();

    static CAurora* AuroraInstance;
    static int AuroraCount;

    static unsigned short m_nHeadLightEnabled;
    static short m_nColorDepth;

    static short m_nDepthBits;

protected:
#ifndef _XBOX

    char m_cUnknown04[156];
#endif
    int m_nWidth;
    int m_nHeight;
    int m_bDisplayModeChanged;
    int m_bFullScreen;
    float m_fGamma;
};

class CAurInternalGL : public CAurInternal
{
public:
    CAurInternalGL();
    virtual ~CAurInternalGL();

    static CAurInternalGL* NewCAuroraInstance();

    int Initialize(HWND hApplicationWindow, HWND hRenderWindow,
                   unsigned long nWidth, unsigned long nHeight,
                   unsigned char nBitsPerPixel, unsigned long nRefreshRate,
                   int nUnknown0, int nUnknown1, int nUnknown2);
    int InitEditor(HWND hApplicationWindow, HWND hRenderWindow,
                   unsigned long nWidth, unsigned long nHeight,
                   unsigned char nBitsPerPixel, unsigned long nRefreshRate,
                   int bFullScreen);
    void Reshape(int nWidth, int nHeight);
    int ShutDown(int bReleaseRenderer, int bDestroyWindow);
#ifndef _XBOX
    int GetGLContext();
#endif
    virtual void MakeCurrentGLContext(HDC hDeviceContext, HGLRC hRenderContext);
#ifdef _XBOX
    virtual void SwapBuffers();
#endif

protected:
    int m_nGLContext;
#ifdef _XBOX

    LPDIRECT3DDEVICE8 m_pD3DDevice;
#endif
};

extern CAurInternalGL* g_pAurora;

CAurInternalGL* NewCAurora();

void AurEnableHeadLight();
void AurDisableHeadLight();

#endif
