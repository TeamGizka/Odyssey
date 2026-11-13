#include <string.h>
#include <windows.h>
#include <GL/gl.h>

#include "aurinternal.h"
#include "aurrendergl.h"
#include "aurora.h"
#include "aurtexman.h"
#include "aurmodel.h"
#include "swmain.h"

#define VIDEO_MAX_REFRESH_RATE 85.0f

extern void AurInitExtensions();
extern void AurTextureRefreshAll();

extern void AurInitVertexArrayRange(int nBytes);

CAurora* CAurInternal::AuroraInstance;
int CAurInternal::AuroraCount;

unsigned short CAurInternal::m_nHeadLightEnabled = 0xffff;

short CAurInternal::m_nColorDepth = 0;
short CAurInternal::m_nDepthBits = 0;
// PC 0x00462E20 GOG 0x00462F30
CAurInternalGL* CAurInternal::NewCAuroraInstance()
{
    return CAurInternalGL::NewCAuroraInstance();
}
// PC 0x00461730 GOG 0x00461840
int CAurInternal::FullScreen()
{
    return m_bFullScreen;
}

#ifdef _XBOX

bool CAurInternal::GetValidMode(unsigned long* pnWidth, unsigned long* pnHeight,
                               unsigned char* pnBitsPerPixel,
                               unsigned long* pnRefreshRate, bool bSetMode)
{
    return true;
}
#else
bool CAurInternal::GetValidMode(unsigned long* pnWidth, unsigned long* pnHeight,  // PC 0x00461740 GOG 0x00461850
                               unsigned char* pnBitsPerPixel,
                               unsigned long* pnRefreshRate, bool bSetMode)
{
    bool bFoundMode;
    DWORD nModeNumber;
    DEVMODEA selectedMode;
    DEVMODEA displayMode;

    memset(&displayMode, 0, sizeof(displayMode));
    displayMode.dmSize = sizeof(displayMode);
    memset(&selectedMode, 0, sizeof(selectedMode));
    selectedMode.dmSize = sizeof(selectedMode);
    nModeNumber = 0;
    bFoundMode = false;
    if (EnumDisplaySettingsA(NULL, 0, &displayMode))
    {
        do
        {
            if (displayMode.dmPelsWidth <= *pnWidth &&
                displayMode.dmPelsHeight <= *pnHeight &&
                displayMode.dmBitsPerPel <= *pnBitsPerPixel &&
                displayMode.dmDisplayFrequency <= *pnRefreshRate &&
                displayMode.dmDisplayFrequency <= VIDEO_MAX_REFRESH_RATE &&
                displayMode.dmPelsWidth >= selectedMode.dmPelsWidth &&
                displayMode.dmPelsHeight >= selectedMode.dmPelsHeight &&
                displayMode.dmBitsPerPel >= selectedMode.dmBitsPerPel &&
                (displayMode.dmDisplayFrequency >= selectedMode.dmDisplayFrequency ||
                 displayMode.dmPelsWidth != selectedMode.dmPelsWidth ||
                 displayMode.dmPelsHeight != selectedMode.dmPelsHeight ||
                 displayMode.dmBitsPerPel != selectedMode.dmBitsPerPel))
            {
                selectedMode = displayMode;
                bFoundMode = true;
            }
            memset(&displayMode, 0, sizeof(displayMode));
            displayMode.dmSize = sizeof(displayMode);
            nModeNumber++;
        } while (EnumDisplaySettingsA(NULL, nModeNumber, &displayMode));

    }
    if (!bFoundMode)
    {
        return false;
    }
    if (ChangeDisplaySettingsA(&selectedMode, CDS_TEST) != DISP_CHANGE_SUCCESSFUL)
    {
        return false;
    }
    if (bSetMode)
    {
        *pnWidth = selectedMode.dmPelsWidth;
        *pnHeight = selectedMode.dmPelsHeight;
        *pnBitsPerPixel = (unsigned char)selectedMode.dmBitsPerPel;
        *pnRefreshRate = selectedMode.dmDisplayFrequency;
        return true;
    }

    return selectedMode.dmPelsWidth == *pnWidth &&
           selectedMode.dmPelsHeight == *pnHeight &&
           selectedMode.dmBitsPerPel == *pnBitsPerPixel &&
           selectedMode.dmDisplayFrequency == *pnRefreshRate;
}
#endif
// PC 0x00461950 GOG 0x00461A60
void CAurInternal::RestoreDesktopDisplayMode()
{
    ResetGammaRamp(1.0f, 1);
#ifndef _XBOX
    if (m_bDisplayModeChanged == 0)
    {
        return;
    }
    ChangeDisplaySettingsA(NULL, 0);
#endif
}
// PC 0x00461980 GOG 0x00461A90
void CAurInternal::RestoreAuroraDisplayMode()
{
    ResetGammaRamp(m_fGamma, 1);
#ifndef _XBOX
    if (m_bDisplayModeChanged != 0)
    {
        DEVMODEA* pDisplayMode = (DEVMODEA*)&m_cUnknown04[0];
        if (ChangeDisplaySettingsA(pDisplayMode, CDS_TEST) == DISP_CHANGE_SUCCESSFUL)
        {
            ChangeDisplaySettingsA(pDisplayMode, CDS_FULLSCREEN);
        }
    }
#endif
}
// PC 0x004619C0 GOG 0x00461AD0
void CAurInternal::DestroyCAuroraInstance()
{
    if (AuroraInstance != NULL && --AuroraCount == 0)
    {
        delete AuroraInstance;
        AuroraInstance = NULL;
    }
}

int CAurInternal::GetGLContext()
{
    return 0;
}

#ifdef _XBOX

void CAurInternalGL::MakeCurrentGLContext(HDC hDeviceContext, HGLRC hRenderContext)
{
}
#else
int CAurInternalGL::GetGLContext()  // PC 0x004619F0 GOG 0x00461B00
{
    return m_nGLContext;
}
// PC 0x00461A00 GOG 0x00461B10
void CAurInternalGL::MakeCurrentGLContext(HDC hDeviceContext, HGLRC hRenderContext)
{
    wglMakeCurrent(hDeviceContext, hRenderContext);
}
#endif

#include <math.h>

#ifdef _XBOX

extern LPDIRECT3DDEVICE8 g_pD3DDevice;

int CAurInternal::ResetGammaRamp(float fGamma, int bSaveGamma)
{
    D3DGAMMARAMP ramp;

    if (bSaveGamma == 0)
    {
        m_fGamma = fGamma;
    }
    for (int nIntensity = 0; nIntensity < 256; nIntensity++)
    {
        unsigned char nValue = (unsigned char)(pow(nIntensity * (1.0f / 255.0f), fGamma) * 255.0f);
        ramp.red[nIntensity] = nValue;
        ramp.green[nIntensity] = nValue;
        ramp.blue[nIntensity] = nValue;
    }
    if (g_pD3DDevice != NULL)
    {
        g_pD3DDevice->SetGammaRamp(0, &ramp);
    }
    return 1;
}
#else
int CAurInternal::ResetGammaRamp(float fGamma, int bSaveGamma)  // PC 0x00461A10 GOG 0x00461B20
{
    unsigned short* pRamp;
    int nChannel;
    int nIntensity;
    unsigned short nHighByte;
    int nResult;
    unsigned short ramp[768];

    if (bSaveGamma == 0)
    {
        m_fGamma = fGamma;
    }
    pRamp = ramp;
    for (nChannel = 3; nChannel != 0; nChannel--)
    {
        nIntensity = 0;
        do
        {
            nHighByte = (unsigned short)(pow(nIntensity / 255.0, fGamma) * 255.0);
            *pRamp = (unsigned short)(nIntensity++ | (nHighByte << 8));
            pRamp++;
        } while (nIntensity < 256);
    }
    nResult = (int)GetDC(g_hRenderWnd);
    if (nResult == 0)
    {
        return nResult;
    }
    SetDeviceGammaRamp((HDC)nResult, ramp);
    ReleaseDC(g_hRenderWnd, (HDC)nResult);
    return 1;
}
#endif

inline CAurora::~CAurora()
{
}

CAurInternal::CAurInternal()
{
#ifdef _XBOX
    m_fGamma = 1.0f;
    m_bDisplayModeChanged = 0;
    m_bFullScreen = 0;
#else
    m_bDisplayModeChanged = 0;
    m_bFullScreen = 0;
    m_fGamma = 1.0f;
#endif
    m_nColorDepth = 0;
}
// PC 0x00461AF0 GOG 0x00461C00
void AurEnableHeadLight()
{
    if (CAurInternal::m_nHeadLightEnabled != 1)
    {
        GLRender_EnableLight(0);
    }
    CAurInternal::m_nHeadLightEnabled = 1;
}
// PC 0x00461B10 GOG 0x00461C20
void AurDisableHeadLight()
{
    if (CAurInternal::m_nHeadLightEnabled != 0)
    {
        GLRender_DisableLight(0);
    }
    CAurInternal::m_nHeadLightEnabled = 0;
}
// PC 0x00461B30 GOG 0x00461C40
CAurInternal::~CAurInternal()
{
}

#ifdef _XBOX

static D3DPRESENT_PARAMETERS g_d3dpp;

void CAurInternalGL::SwapBuffers()
{
    g_pD3DDevice->Present(NULL, NULL, NULL, NULL);
}

int CAurInternalGL::Initialize(HWND hApplicationWindow, HWND hRenderWindow,
                               unsigned long nWidth, unsigned long nHeight,
                               unsigned char nBitsPerPixel,
                               unsigned long nRefreshRate, int nUnknown0,
                               int nUnknown1, int nUnknown2)
{
    GLRender::InitRenderSystem();
    m_nHeadLightEnabled = (unsigned short)-1;
    LPDIRECT3D8 pD3D = Direct3DCreate8(0);
    if ((int)pD3D < 0)
    {
        return 0;
    }
    memset(&g_d3dpp, 0, sizeof(g_d3dpp));
    XGetVideoStandard();
    g_d3dpp.FullScreen_RefreshRateInHz = (XGetVideoFlags() & XC_VIDEO_FLAGS_PAL_60Hz) ? 60 : 0;
    g_d3dpp.BackBufferWidth = 640;
    g_d3dpp.BackBufferHeight = 480;
    g_d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
    g_d3dpp.BackBufferCount = 1;
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
    g_d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    int bHadDevice = 0;
    if (g_pD3DDevice != NULL)
    {
        bHadDevice = 1;
        GLRender::BackUpLights();
    }
    if (FAILED(pD3D->CreateDevice(0, D3DDEVTYPE_HAL, NULL, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp,
                                  &m_pD3DDevice)))
    {
        return 0;
    }
    g_pD3D = pD3D;
    g_pD3DDevice = m_pD3DDevice;
    if (bHadDevice)
    {
        GLRender::RestoreLights();
    }
    g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, cullTranslationTable[CULL_COUNTERCLOCKWISE]);
    GLRender::ResetViewPort(0, 0, nWidth, nHeight);
    AurInitExtensions();
    TrackMatrices();
    m_nColorDepth = nBitsPerPixel;
    GLRender_Enable(Aurora::ENABLE_VERTEX_PROGRAM);
    g_pBumpyShinySkinPass1 = new VertexProgram(
        usearbvertexprograms ? szBumpyShinySkinVPARB : cBumpyShinySkinVP,
        cBumpyShinySkinVPDeclaration);
    g_pBumpyShinySkinPass1->Enable(NULL, true);
    g_pBumpyShinySkinPass2 = new VertexProgram(cSkinVP, cBumpyShinySkinVPDeclaration);
    g_pBumpyShinySkinPass2->Enable(NULL, true);

    g_pBumpyShinySkinPass2_3bone = new VertexProgram(cSkin3BoneVP, cSkin3BoneVPDeclaration);
    g_pBumpyShinySkinPass2_3bone->Enable(NULL, true);
    g_pBumpyShinyPass1 = new VertexProgram(
        usearbvertexprograms ? szStandardVPARB : cStandardVP,
        cStandardVP2Declaration);
    g_pBumpyShinyPass1->Enable(NULL, true);
    g_pBumpyShinyPass2 = new VertexProgram(
        usearbvertexprograms ? szStandardVPARB : cStandardVP,
        cStandardVPDeclaration);
    g_pBumpyShinyPass2->Enable(NULL, false);
    g_pBumpyShinyPass3 = new VertexProgram(
        usearbvertexprograms ? szStandardVPARB : cStandardVP,
        cBumpyShinyRegularVPDeclaration);
    g_pBumpyShinyPass3->Enable(NULL, true);
    g_pBumpyShinyPass4 = new VertexProgram(
        usearbvertexprograms ? szBumpyShinyVPARB : cBumpyShinyVP,
        cBumpyShinyRegularVPDeclaration);
    g_pBumpyShinyPass4->Enable(NULL, false);
    g_pBumpyShinyPass5 = new VertexProgram(
        usearbvertexprograms ? szBumpyShinyVPARB : cBumpyShinyVP,
        cBumpyShinyLightMappedVPDeclaration);
    g_pBumpyShinyPass5->Enable(NULL, false);
    g_pD3DDevice->SetVertexShader(0);
    GLRender_Disable(Aurora::ENABLE_VERTEX_PROGRAM);
    return 1;
}

void AurSetFrameBufferMode(int nMode)
{
    GLRender::SetFrameBufferMode(nMode);
    if (nMode == 2)
    {
        g_d3dpp.MultiSampleType = D3DMULTISAMPLE_2_SAMPLES_SUPERSAMPLE_VERTICAL_LINEAR;
    }
    else
    {
        g_d3dpp.MultiSampleType = nMode == 4 ? D3DMULTISAMPLE_4_SAMPLES_SUPERSAMPLE_GAUSSIAN : D3DMULTISAMPLE_NONE;
    }
    g_pD3DDevice->Reset(&g_d3dpp);
}

int CAurInternalGL::InitEditor(HWND hApplicationWindow, HWND hRenderWindow,
                               unsigned long nWidth, unsigned long nHeight,
                               unsigned char nBitsPerPixel,
                               unsigned long nRefreshRate, int bFullScreen)
{
    return 0;
}
#endif

#ifndef _XBOX
int CAurInternalGL::Initialize(HWND hApplicationWindow, HWND hRenderWindow,  // PC 0x00461B40 GOG 0x00461C50
                               unsigned long nWidth, unsigned long nHeight,
                               unsigned char nBitsPerPixel,
                               unsigned long nRefreshRate, int nUnknown0,
                               int nUnknown1, int nUnknown2)
{
    HDC hDeviceContext;
    DEVMODEA displayMode;
    DWORD nStyle;
    HWND hInsertAfter;
    RECT windowRect;
    int anAttributes[30];
    float afAttributes[2];
    PIXELFORMATDESCRIPTOR multisampleFormat;
    int bAntiAliasingFailed;

    static int initialized = 1;
    if (initialized)
    {
        initialized = 0;
        GLRender::InitializeAntiAliasing();
    }
    m_nHeadLightEnabled = (unsigned short)-1;
    if (nUnknown0 != 0)
    {
        memset(&displayMode, 0, sizeof(displayMode));
        displayMode.dmSize = sizeof(displayMode);
        DWORD nModeNumber = 0;
        if (EnumDisplaySettingsA(NULL, 0, &displayMode))
        {
            do
            {
                if (displayMode.dmPelsWidth == nWidth &&
                    displayMode.dmPelsHeight == nHeight &&
                    displayMode.dmBitsPerPel == nBitsPerPixel &&
                    displayMode.dmDisplayFrequency == nRefreshRate &&
                    displayMode.dmDisplayFrequency <= VIDEO_MAX_REFRESH_RATE)
                {
                    if (ChangeDisplaySettingsA(&displayMode, CDS_TEST) !=
                        DISP_CHANGE_SUCCESSFUL)
                        return 0;
                    memcpy(m_cUnknown04, &displayMode, sizeof(displayMode));
                    ChangeDisplaySettingsA(&displayMode, CDS_FULLSCREEN);
                    m_bDisplayModeChanged = 1;
                    break;
                }
                memset(&displayMode, 0, sizeof(displayMode));
                ++nModeNumber;
                displayMode.dmSize = sizeof(displayMode);
            } while (EnumDisplaySettingsA(NULL, nModeNumber, &displayMode));
        }
        if (m_bDisplayModeChanged == 0)
            nUnknown0 = 0;
    }
    if (nUnknown0 != 0)
    {
        nStyle = WS_POPUP | WS_CLIPCHILDREN;
        hInsertAfter = HWND_TOPMOST;
        m_bFullScreen = 1;
    }
    else
    {
        nStyle = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN;
        hInsertAfter = HWND_NOTOPMOST;
        m_bFullScreen = 0;
    }
    SetWindowLongA(hRenderWindow, GWL_STYLE, nStyle);
    windowRect.left = 0;
    windowRect.right = nWidth;
    windowRect.top = 0;
    windowRect.bottom = nHeight;
    AdjustWindowRect(&windowRect, nStyle, FALSE);
    SetWindowPos(hRenderWindow, hInsertAfter, 0, 0,
                 windowRect.right - windowRect.left,
                 windowRect.bottom - windowRect.top,
                 SWP_FRAMECHANGED | SWP_NOCOPYBITS);
    SendMessageA(hRenderWindow, WM_SIZE, SIZE_RESTORED,
                 MAKELPARAM(windowRect.right - windowRect.left,
                            windowRect.bottom - windowRect.top));
    ShowWindow(hRenderWindow, SW_SHOWNORMAL);
    hDeviceContext = GetDC(hRenderWindow);
    if (hDeviceContext == NULL)
        return 0;

    m_nColorDepth = 32;
    m_nDepthBits = 24;
    bAntiAliasingFailed = 0;
    if (g_nAliasingMode != 0)
    {
        if (!AurSetAntiAliasingMode(g_nAliasingMode))
            bAntiAliasingFailed = 1;
        else if (g_nAliasingMode != 0)
        {
            unsigned int nFormats;
            int nFormat;
            anAttributes[0] = WGL_DRAW_TO_WINDOW_ARB;
            anAttributes[1] = 1;
            anAttributes[2] = WGL_ACCELERATION_ARB;
            anAttributes[3] = WGL_FULL_ACCELERATION_ARB;
            anAttributes[4] = WGL_COLOR_BITS_ARB;
            anAttributes[5] = nUnknown1;
            anAttributes[6] = WGL_ALPHA_BITS_ARB;
            anAttributes[7] = 8;
            anAttributes[8] = WGL_DEPTH_BITS_ARB;
            anAttributes[9] = 24;
            if (nUnknown2 <= 24)
                anAttributes[9] = nUnknown2;
            anAttributes[10] = WGL_DOUBLE_BUFFER_ARB;
            anAttributes[11] = 1;
            anAttributes[12] = WGL_SAMPLE_BUFFERS_ARB;
            anAttributes[13] = 1;
            anAttributes[14] = WGL_SAMPLES_ARB;
            anAttributes[15] = g_nAliasingMode;
            anAttributes[16] = WGL_STENCIL_BITS_ARB;
            anAttributes[17] = 8;
            anAttributes[18] = 0;
            anAttributes[19] = 0;
            afAttributes[0] = 0;
            afAttributes[1] = 0;
            BOOL bFormatChosen = wglChoosePixelFormatARB(hDeviceContext, anAttributes,
                                       afAttributes, 1, &nFormat, &nFormats);
            if (bFormatChosen &&
                nFormats > 0)
            {
                if (!SetPixelFormat(hDeviceContext, nFormat, &multisampleFormat))
                    bAntiAliasingFailed = 1;
                m_nColorDepth = nUnknown1;
                m_nDepthBits = 24;
            }
            else
                bAntiAliasingFailed = 1;
        }
    }
    if (bAntiAliasingFailed || g_nAliasingMode == 0)
    {
        PIXELFORMATDESCRIPTOR pixelFormat;
        pixelFormat.nSize = sizeof(pixelFormat);
        pixelFormat.nVersion = 1;
        pixelFormat.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pixelFormat.iPixelType = PFD_TYPE_RGBA;
        pixelFormat.cColorBits = 32;
        pixelFormat.cRedBits = 0;
        pixelFormat.cRedShift = 0;
        pixelFormat.cGreenBits = 0;
        pixelFormat.cGreenShift = 0;
        pixelFormat.cBlueBits = 0;
        pixelFormat.cBlueShift = 0;
        pixelFormat.cAlphaBits = 8;
        pixelFormat.cAlphaShift = 0;
        pixelFormat.cAccumBits = 0;
        pixelFormat.cAccumRedBits = 0;
        pixelFormat.cAccumGreenBits = 0;
        pixelFormat.cAccumBlueBits = 0;
        pixelFormat.cAccumAlphaBits = 0;
        pixelFormat.cDepthBits = 24;
        pixelFormat.cStencilBits = 8;
        pixelFormat.cAuxBuffers = 0;
        pixelFormat.iLayerType = PFD_MAIN_PLANE;
        pixelFormat.bReserved = 0;
        pixelFormat.dwLayerMask = 0;
        pixelFormat.dwVisibleMask = 0;
        pixelFormat.dwDamageMask = 0;
        int nPixelFormat = ChoosePixelFormat(hDeviceContext, &pixelFormat);
        if (nPixelFormat == 0)
        {
            ReleaseDC(hRenderWindow, hDeviceContext);
            return 0;
        }
        if (!SetPixelFormat(hDeviceContext, nPixelFormat, NULL))
        {
            ReleaseDC(hRenderWindow, hDeviceContext);
            return 0;
        }
        DescribePixelFormat(hDeviceContext, nPixelFormat, sizeof(pixelFormat), &pixelFormat);
        m_nColorDepth = pixelFormat.cColorBits;
        m_nDepthBits = pixelFormat.cDepthBits;
    }
    m_nGLContext = (int)wglCreateContext(hDeviceContext);
    if (m_nGLContext == 0)
    {
        ReleaseDC(hRenderWindow, hDeviceContext);
        return 0;
    }
    if (!wglMakeCurrent(hDeviceContext, (HGLRC)m_nGLContext))
    {
        wglDeleteContext((HGLRC)m_nGLContext);
        ReleaseDC(hRenderWindow, hDeviceContext);
        return 0;
    }
    AurInitExtensions();
    if (AurVertexProgramsAvailable())
        TrackMatrices();
    AurInitVertexArrayRange(16 * 1024 * 1024);
    GLRender::InitRenderSystem();
    AurTextureRefreshAll();
    GLRender::InitializeImageSpaceSoftShadows();
    GLRender::InitializeFrameBufferModifications();
    if (AurVertexProgramsAvailable())
    {
        glEnable(enableTranslationTable[ENABLE_VERTEX_PROGRAM]);
        g_pBumpyShinySkinPass1 = new VertexProgram(
            usearbvertexprograms ? szBumpyShinySkinVPARB : cBumpyShinySkinVP,
            cBumpyShinySkinVPDeclaration);
        g_pBumpyShinySkinPass1->Enable(NULL, false);
        g_pBumpyShinySkinPass2 = new VertexProgram(
            usearbvertexprograms ? szSkinVPARB : cSkinVP,
            cBumpyShinySkinVPDeclaration);
        g_pBumpyShinySkinPass2->Enable(NULL, true);
        g_pBumpyShinyPass1 = new VertexProgram(
            usearbvertexprograms ? szStandardVPARB : cStandardVP,
            cStandardVP2Declaration);
        g_pBumpyShinyPass1->Enable(NULL, true);
        g_pBumpyShinyPass2 = new VertexProgram(
            usearbvertexprograms ? szStandardFogVPARB : cStandardVP,
            cStandardVPDeclaration);
        g_pBumpyShinyPass2->Enable(NULL, false);
        g_pBumpyShinyPass3 = new VertexProgram(
            usearbvertexprograms ? szStandardVPARB : cStandardVP,
            cBumpyShinyRegularVPDeclaration);
        g_pBumpyShinyPass3->Enable(NULL, true);
        g_pBumpyShinyPass4 = new VertexProgram(
            usearbvertexprograms ? szBumpyShinyVPARB : cBumpyShinyVP,
            cBumpyShinyRegularVPDeclaration);
        g_pBumpyShinyPass4->Enable(NULL, false);
        g_pBumpyShinyPass5 = new VertexProgram(
            usearbvertexprograms ? szBumpyShinyVPARB : cBumpyShinyVP,
            cBumpyShinyLightMappedVPDeclaration);
        g_pBumpyShinyPass5->Enable(NULL, false);
        g_pSkinBumpedOutVP = new VertexProgram(
            usearbvertexprograms ? szSkinBumpedOutVPARB : cSkinBumpedOutVP,
            cSkinBumpedOutVPDeclaration);
        g_pSkinBumpedOutVP->Enable(NULL, false);
        g_pSkinBumpedOutWithTangentVP = new VertexProgram(
            usearbvertexprograms ? szSkinBumpedOutVPARB : cSkinBumpedOutVP,
            cBumpyShinySkinVPDeclaration);
        g_pSkinBumpedOutWithTangentVP->Enable(NULL, false);
        g_pBumpedOutVP = new VertexProgram(
            usearbvertexprograms ? szBumpedOutVPARB : cBumpedOutVP,
            cStandardVPDeclaration);
        g_pBumpedOutVP->Enable(NULL, false);
        glDisable(enableTranslationTable[ENABLE_VERTEX_PROGRAM]);
    }
    if (g_nAliasingMode > 0 && !bAntiAliasingFailed)
        glEnable(GL_MULTISAMPLE_ARB);
    else if (g_nOldAliasingMode > 0)
        glDisable(GL_MULTISAMPLE_ARB);
    wglSwapIntervalEXT(GLRender::cm_bVSync != 0);
    return 1;
}
#endif

#ifndef _XBOX
int CAurInternalGL::InitEditor(HWND hApplicationWindow, HWND hRenderWindow,  // PC 0x00462450 GOG 0x00462560
                               unsigned long nWidth, unsigned long nHeight,
                               unsigned char nBitsPerPixel,
                               unsigned long nRefreshRate, int bFullScreen)
{
    HDC hDeviceContext;
    PIXELFORMATDESCRIPTOR pixelFormat;
    HGLRC hContext;
    DEVMODEA displayMode;
    DWORD nStyle;
    HWND hInsertAfter;
    RECT windowRect;

    ShowWindow(hRenderWindow, SW_HIDE);
    if (bFullScreen != 0)
    {
        memset(&displayMode, 0, sizeof(displayMode));
        DWORD nModeNumber = 0;
        displayMode.dmSize = sizeof(displayMode);
        if (EnumDisplaySettingsA(NULL, 0, &displayMode))
        {
            do
            {
                if (displayMode.dmPelsWidth == nWidth &&
                    displayMode.dmPelsHeight == nHeight &&
                    displayMode.dmBitsPerPel == nBitsPerPixel &&
                    displayMode.dmDisplayFrequency == nRefreshRate &&
                    displayMode.dmDisplayFrequency <= VIDEO_MAX_REFRESH_RATE)
                {
                    if (ChangeDisplaySettingsA(&displayMode, CDS_TEST) !=
                        DISP_CHANGE_SUCCESSFUL)
                    {
                        return 0;
                    }
                    memcpy(m_cUnknown04, &displayMode, sizeof(displayMode));
                    ChangeDisplaySettingsA(&displayMode, CDS_FULLSCREEN);
                    m_bDisplayModeChanged = 1;
                    break;
                }
                memset(&displayMode, 0, sizeof(displayMode));
                ++nModeNumber;
                displayMode.dmSize = sizeof(displayMode);
            } while (EnumDisplaySettingsA(NULL, nModeNumber, &displayMode));
        }
        if (m_bDisplayModeChanged == 0)
        {
            ShowWindow(hRenderWindow, SW_SHOWNORMAL);
            return 0;
        }
        nStyle = WS_POPUP;
        hInsertAfter = HWND_TOPMOST;
        m_bFullScreen = 1;
    }
    else
    {
        nStyle = WS_CHILD;
        hInsertAfter = HWND_NOTOPMOST;
        m_bFullScreen = 0;
    }
    hDeviceContext = GetDC(hRenderWindow);
    if (hDeviceContext == NULL)
    {
        return 0;
    }
    memset(&pixelFormat, 0, sizeof(pixelFormat));
    pixelFormat.iPixelType = PFD_TYPE_RGBA;
    pixelFormat.nSize = sizeof(pixelFormat);
    pixelFormat.nVersion = 1;
    pixelFormat.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pixelFormat.cDepthBits = 32;
    pixelFormat.cColorBits = 32;
    pixelFormat.iLayerType = PFD_MAIN_PLANE;
    pixelFormat.cStencilBits = 8;
    int nPixelFormat = ChoosePixelFormat(hDeviceContext, &pixelFormat);
    if (nPixelFormat == 0)
    {
        ReleaseDC(hRenderWindow, hDeviceContext);
        return 0;
    }
    if (!SetPixelFormat(hDeviceContext, nPixelFormat, &pixelFormat))
    {
        ReleaseDC(hRenderWindow, hDeviceContext);
        return 0;
    }
    DescribePixelFormat(hDeviceContext, nPixelFormat, sizeof(pixelFormat), &pixelFormat);
    m_nGLContext = (int)wglCreateContext(hDeviceContext);
    if (m_nGLContext == 0)
    {
        ReleaseDC(hRenderWindow, hDeviceContext);
        return 0;
    }
    if (!wglMakeCurrent(hDeviceContext, (HGLRC)m_nGLContext))
    {
        wglDeleteContext((HGLRC)m_nGLContext);
        ReleaseDC(hRenderWindow, hDeviceContext);
        return 0;
    }
    ReleaseDC(hRenderWindow, hDeviceContext);
    windowRect.left = 0;
    windowRect.right = nWidth;
    windowRect.top = 0;
    windowRect.bottom = nHeight;
    AdjustWindowRect(&windowRect, nStyle, FALSE);
    SetWindowLongA(hRenderWindow, GWL_STYLE, nStyle);
    SetWindowPos(hRenderWindow, hInsertAfter, 0, 0,
                 windowRect.right - windowRect.left,
                 windowRect.bottom - windowRect.top,
                 SWP_FRAMECHANGED | SWP_NOCOPYBITS);
    SetWindowPos(hRenderWindow, NULL, 0, 0, nWidth, nHeight,
                 SWP_FRAMECHANGED | SWP_NOCOPYBITS);
    SendMessageA(hRenderWindow, WM_SIZE, SIZE_RESTORED,
                 MAKELPARAM(nWidth, nHeight));
    ShowWindow(hRenderWindow, SW_SHOWNORMAL);

    static bool l_bInitialized = false;
    if (!l_bInitialized)
    {
        l_bInitialized = true;
        GLRender::InitExtensions();
        if (AurVertexProgramsAvailable())
        {
            glEnable(enableTranslationTable[ENABLE_VERTEX_PROGRAM]);
            g_pBumpyShinySkinPass1 = new VertexProgram(
                usearbvertexprograms ? szBumpyShinySkinVPARB
                                     : cBumpyShinySkinVP,
                cBumpyShinySkinVPDeclaration);
            g_pBumpyShinySkinPass1->Enable(NULL, false);
            g_pBumpyShinySkinPass2 = new VertexProgram(
                usearbvertexprograms ? szSkinVPARB
                                     : cSkinVP,
                cBumpyShinySkinVPDeclaration);
            g_pBumpyShinySkinPass2->Enable(NULL, true);
            g_pBumpyShinySkinPass2_3bone = new VertexProgram(
                cSkin3BoneVP, cSkin3BoneVPDeclaration);
            g_pBumpyShinySkinPass2_3bone->Enable(NULL, true);
            g_pBumpyShinyPass1 = new VertexProgram(
                usearbvertexprograms ? szStandardVPARB
                                     : cStandardVP,
                cStandardVP2Declaration);
            g_pBumpyShinyPass1->Enable(NULL, true);
            g_pBumpyShinyPass2 = new VertexProgram(
                usearbvertexprograms ? szStandardVPARB
                                     : cStandardVP,
                cStandardVPDeclaration);
            g_pBumpyShinyPass2->Enable(NULL, false);
            g_pBumpyShinyPass3 = new VertexProgram(
                usearbvertexprograms ? szStandardVPARB
                                     : cStandardVP,
                cBumpyShinyRegularVPDeclaration);
            g_pBumpyShinyPass3->Enable(NULL, true);
            g_pBumpyShinyPass4 = new VertexProgram(
                usearbvertexprograms ? szBumpyShinyVPARB
                                     : cBumpyShinyVP,
                cBumpyShinyRegularVPDeclaration);
            g_pBumpyShinyPass4->Enable(NULL, false);
            g_pBumpyShinyPass5 = new VertexProgram(
                usearbvertexprograms ? szBumpyShinyVPARB
                                     : cBumpyShinyVP,
                cBumpyShinyLightMappedVPDeclaration);
            g_pBumpyShinyPass5->Enable(NULL, false);
            glDisable(enableTranslationTable[ENABLE_VERTEX_PROGRAM]);
        }
    }
    return 1;
}
#endif

#ifdef _XBOX

void CAurInternalGL::Reshape(int nWidth, int nHeight)
{
    m_nWidth = nWidth;
    m_nHeight = nHeight;
    g_nScreenWidth = nWidth;
    g_nScreenHeight = nHeight;
    if (nWidth != 1 && nHeight != 1)
    {
        int nViewportHeight = nHeight;
        if (nViewportHeight == 0)
        {
            nViewportHeight = 1;
        }
        if (nWidth == 0)
        {
            nWidth = 1;
        }
        GLRender::ResetViewPort(0, 0, nWidth, nViewportHeight);
    }

}
#else
void CAurInternalGL::Reshape(int nWidth, int nHeight)  // PC 0x00462AA0 GOG 0x00462BB0
{
    GLsizei nViewportHeight;
    GLsizei nViewportWidth;

    if (m_nGLContext != 0)
    {
        nViewportHeight = nHeight;
        nViewportWidth = nWidth;
        m_nWidth = nWidth;
        m_nHeight = nHeight;
        g_nScreenWidth = nWidth;
        g_nScreenHeight = nHeight;
        if (nHeight == 0)
        {
            nViewportHeight = 1;
        }
        if (nWidth == 0)
        {
            nViewportWidth = 1;
        }
        glViewport(0, 0, nViewportWidth, nViewportHeight);
    }
}
#endif
#ifdef _XBOX  // PC 0x00462AF0 GOG 0x00462C00
void CAurInternalGL::ShutDown(int bReleaseRenderer)
#else
int CAurInternalGL::ShutDown(int bReleaseRenderer, int bDestroyWindow)
#endif
{
#ifndef _XBOX
    VertexProgramManager::ResetVertexPrograms();
    if (g_pBumpyShinySkinPass1 != NULL)
    {
        delete g_pBumpyShinySkinPass1;
        g_pBumpyShinySkinPass1 = NULL;
    }
    if (g_pBumpyShinySkinPass2 != NULL)
    {
        delete g_pBumpyShinySkinPass2;
        g_pBumpyShinySkinPass2 = NULL;
    }
    if (g_pBumpyShinySkinPass2_3bone != NULL)
    {
        delete g_pBumpyShinySkinPass2_3bone;
        g_pBumpyShinySkinPass2_3bone = NULL;
    }
    if (g_pBumpyShinyPass1 != NULL)
    {
        delete g_pBumpyShinyPass1;
        g_pBumpyShinyPass1 = NULL;
    }
    if (g_pBumpyShinyPass2 != NULL)
    {
        delete g_pBumpyShinyPass2;
        g_pBumpyShinyPass2 = NULL;
    }
    if (g_pBumpyShinyPass3 != NULL)
    {
        delete g_pBumpyShinyPass3;
        g_pBumpyShinyPass3 = NULL;
    }
    if (g_pBumpyShinyPass4 != NULL)
    {
        delete g_pBumpyShinyPass4;
        g_pBumpyShinyPass4 = NULL;
    }
    if (g_pBumpyShinyPass5 != NULL)
    {
        delete g_pBumpyShinyPass5;
        g_pBumpyShinyPass5 = NULL;
    }
    if (g_pSkinBumpedOutVP != NULL)
    {
        delete g_pSkinBumpedOutVP;
        g_pSkinBumpedOutVP = NULL;
    }
    if (g_pSkinBumpedOutWithTangentVP != NULL)
    {
        delete g_pSkinBumpedOutWithTangentVP;
        g_pSkinBumpedOutWithTangentVP = NULL;
    }
    if (g_pBumpedOutVP != NULL)
    {
        delete g_pBumpedOutVP;
        g_pBumpedOutVP = NULL;
    }
#endif
    if (bReleaseRenderer != 0)
    {
        AurTextureClearAll();
    }
    GLRender::Shutdown(bReleaseRenderer);
#ifndef _XBOX
    if (szBumpyShinyVPARB != NULL) {
        delete szBumpyShinyVPARB;
        szBumpyShinyVPARB = NULL;
    }
    if (szBumpyShinySkinVPARB != NULL)
    {
        delete szBumpyShinySkinVPARB;
        szBumpyShinySkinVPARB = NULL;
    }
    if (m_nGLContext != 0)
    {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext((HGLRC)m_nGLContext);
        m_nGLContext = 0;
    }
    if (m_bDisplayModeChanged != 0)
    {
        if (bDestroyWindow != 0)
        {
            ChangeDisplaySettingsA(NULL, 0);
        }
        m_bDisplayModeChanged = 0;
        m_bFullScreen = 0;
    }
#endif
    if (bReleaseRenderer != 0)
    {
        ResetGammaRamp(1.0f, 0);
    }
#ifndef _XBOX
    int nResult = (int)g_hRenderWnd;
    if (g_hRenderWnd != NULL && bDestroyWindow != 0)
    {
        nResult = DestroyWindow(g_hRenderWnd);
        g_hRenderWnd = NULL;
    }
    return nResult;
#endif
}
// PC 0x00462D90 GOG 0x00462EA0
CAurInternalGL* CAurInternalGL::NewCAuroraInstance()
{
    if (AuroraInstance == NULL)
    {
        AuroraInstance = (CAurora*)new CAurInternalGL;
    }
    AuroraCount++;
    return (CAurInternalGL*)AuroraInstance;
}

CAurInternalGL::CAurInternalGL()
{
#ifndef _XBOX
    m_nGLContext = 0;
#endif
}
// PC 0x00462D20 GOG 0x00462E30
CAurInternalGL::~CAurInternalGL()
{
    untrace(NULL); AurStringsFree(); AurFreeFont(); AurDeleteAllFonts();
#ifdef _XBOX
    ShutDown(1);
#else
    ShutDown(1, 1);
#endif
}
