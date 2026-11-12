#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "aurmodel.h"
#include "aurrendergl.h"
#include "aurinternal.h"
#include "aurgui.h"
#include "aurconsole.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "swmain.h"
#include "aurscene.h"
#include "aurscenemanager.h"

float g_fGlobalAlpha = 1.0f;
unsigned int g_rgbaPauseOverlayColor = 0x7f000000u;
bool g_bVertexBufferObjectsWriteOnly = true;

char versionstring[32];
float CAurGUIDraw::cm_fViewportAlpha[10];

float sleepseconds = 0.0f;
float memoryfree = 0.0f;
int g_nCurrentAnisotropy = 0;
char disablepostprocessing = 0;
bool g_bDisableVertexBufferObjects = false;
// PC 0x00462E30
CAurInternalGL* NewCAurora()
{
    return CAurInternal::NewCAuroraInstance();
}
// PC 0x00462E40
void DestroyCAurora()
{
    CAurInternal::DestroyCAuroraInstance();
}

#ifdef _XBOX

void sleep(int nMilliseconds)
{
    clock_t nEndTime = clock() + nMilliseconds;
    while (nEndTime > clock())
        ;
}
#else
void sleep(unsigned long nMilliseconds)
{
    unsigned long nEndTime = clock() + nMilliseconds;
    while (nEndTime > clock())
        ;
}
#endif
// PC 0x00462E50
int AurMakeCurrent()
{
#ifdef _XBOX
    if (sleepseconds != 0.0f)
        sleep((int)(sleepseconds * 1000.0f));
#else
    if (sleepseconds != 0.0f)
    {
        int nSleepTime = (int)(sleepseconds * 1000.0f);
        clock_t nEndTime = nSleepTime + clock();
        while (nEndTime > clock())
            ;
    }
#endif

    MEMORYSTATUS msStatus;

    GlobalMemoryStatus(&msStatus);
    memoryfree =
        (float)msStatus.dwAvailPhys * 0.0009765625f;
    return AurTextureMakeCurrent();
}

inline CAurPathInfo::~CAurPathInfo()
{
}
// PC 0x00462EC0
CAurPathInfo::CAurPathInfo()
{
    m_pfCoordinates = NULL;
    m_nCoordinates = 0;
}
// PC 0x00462EF0
void AurEnableShadows()
{
    enableshadows = 1;
}
// PC 0x00462EE0
void AurDisableGrass()
{
    enablegrass = 1;
}
// PC 0x00463190
void AurEnableGrass()
{
    enablegrass = 0;
}
// PC 0x00462F10
int AurEnableSoftShadows()
{
    enablesoftshadows = 1;
    enableshadows = 1;
    return 1;
}
// PC 0x00462F20
void AurDisableSoftShadows()
{
    enablesoftshadows = 0;
}
// PC 0x00462F30
void AurEnableFrameBufferEffects()
{
    doframebuffer = 1;
}
// PC 0x00462F40
void AurDisableFrameBufferEffects()
{
    doframebuffer = 0;
}

#ifndef _XBOX
// PC 0x00462F50
void AurSetVSyncState(bool bEnabled)
{
    if (AuroraUseARBShaders())
    {
        wglSwapIntervalEXT(bEnabled != 0);
        GLRender::cm_bVSync = bEnabled;
    }
}
#endif

const char* AurGetVersionString()
{
    sprintf(versionstring, "SWPC 0027");
    return versionstring;
}

#ifndef _XBOX
void AurGetGLError()  // PC 0x00462F80
{
    int nError = glGetError();
    if (nError != 0)
    {
        gluErrorString(nError);
    }
}
#endif
// PC 0x00462FA0
void AurSetPauseOverlayColor(float fRed, float fGreen, float fBlue,
                             float fAlpha)
{
    g_rgbaPauseOverlayColor = (unsigned int)(fRed * 255.0f) |
                              ((unsigned int)(fGreen * 255.0f) << 8) |
                              ((unsigned int)(fBlue * 255.0f) << 16) |
                              ((unsigned int)(fAlpha * 255.0f) << 24);
}

char* testsetpausecolor(char* sArguments)
{
#ifdef _XBOX

    double afColor[4];
    afColor[0] = atof(GetToken(&sArguments));
    afColor[1] = atof(GetToken(&sArguments));
    afColor[2] = atof(GetToken(&sArguments));
    afColor[3] = atof(GetToken(&sArguments));
    g_rgbaPauseOverlayColor = ((((int)(afColor[3] * 255.0) << 8 | (int)(afColor[0] * 255.0)) << 8 |
                                (int)(afColor[1] * 255.0)) << 8) | (int)(afColor[2] * 255.0);
#else
    float fRed = (float)atof(GetToken(&sArguments));
    float fGreen = (float)atof(GetToken(&sArguments));
    float fBlue = (float)atof(GetToken(&sArguments));
    float fAlpha = (float)atof(GetToken(&sArguments));
    AurSetPauseOverlayColor(fRed, fGreen, fBlue, fAlpha);
#endif
    return "SUCCESS";
}
// PC 0x00463000
void AurEnableForceSight()
{
    g_ForceSight = true;
    if (!AurFrameBufferModificationsAvailable() &&
        !AurATIFrameBufferModificationsAvailable())
        g_ForceSight = false;
}
// PC 0x00463020
void AurDisableForceSight()
{
    g_ForceSight = false;
}
// PC 0x00463030
void AurDisableVertexBufferObjects()
{
    g_bDisableVertexBufferObjects = true;
}
// PC 0x00463040
void AurDisableVertexBufferObjectsWriteOnly()
{
    g_bVertexBufferObjectsWriteOnly = false;
}
// PC 0x00463050
void AurEnableVertexBufferObjectsWriteOnly()
{
    g_bVertexBufferObjectsWriteOnly = true;
}

#ifndef _XBOX
void AurATIUnlockSoftShadowsForHotFix()  // PC 0x00463060
{
    g_nUnlockATISoftShadowsForHotFix = 1;
    enablesoftshadows = 0;
}
#endif

#ifdef _XBOX

void AurReportError(int nLine, const char* sFile, const char* sMessage)
{
    char sReport[1024];
    if (sMessage == NULL)
    {
        sprintf(sReport, "A warning has occured:\n File: %s  Line #: %d\n\n", sFile, nLine);
    }
    else
    {
        sprintf(sReport, "A warning has occured:\n File: %s  Line #: %d \n Programmer says: %s\n\n", sFile, nLine,
                sMessage);
    }
    FILE* pFile = fopen("d:\\_warnings.txt", "at");
    if (pFile != NULL)
    {
        fwrite(sReport, strlen(sReport), 1, pFile);
        fclose(pFile);
    }
}
#endif
// PC 0x00463080
int (AurAddDrawableSquare)(float* pVertices, unsigned long nColor,
                           const char* sFile, int nLine)
{
    unsigned long nPool;
    char sMessage[256];
    sprintf(sMessage, "BOWTIE [%s line %d] (%.2f, %.2f, %.2f)\n", sFile, nLine,
            pVertices[3], pVertices[4], pVertices[5]);
#ifdef _XBOX

    unsigned char nBlue = (unsigned char)(nColor >> 16);
    unsigned char nRed = (unsigned char)nColor;
    ((unsigned char*)&nColor)[0] = nBlue;
    ((unsigned char*)&nColor)[2] = nRed;
#endif
    nPool = GLRender::RequestPool(Aurora::POOL_VERTEX, 80, 0, 17, &nPool);

    float* pSquare = (float*)GLRender::LockPool(nPool, 0, 0, RequestPoolManager::POOL_LOCK_WRITE);

    float* pSource = pVertices;
    float* pDestination = pSquare;
    for (int i = 0; i < 4; ++i)
    {
        pDestination[0] = pSource[0];
        pDestination[1] = pSource[1];
        pDestination[2] = pSource[2];
        ((unsigned long*)pDestination)[3] = nColor;
        pSource += 3;
        pDestination += 4;
    }

    pSquare[16] = pVertices[0];
    pSquare[17] = pVertices[1];
    pSquare[18] = pVertices[2];
    ((unsigned long*)pSquare)[19] = nColor;
    GLRender::UnlockPool(nPool);
    GLRender::AddSquareToRender(nPool);
    return nPool;
}
// PC 0x00463160
void AurRemoveDrawableSquare(unsigned long nSquare)
{
    if (nSquare != (unsigned int)-1)
    {
        GLRender::RemoveSquareToRender(nSquare);
    }
}

#ifdef _XBOX

void AurRemoveAllDrawableSquares()
{
    GLRender::RemoveAllSquaresToRender();
}
#endif

char* addsquaretorender(char* sArguments)
{
    float fVertices[12];
    for (int i = 0; i < 12; ++i)
    {
        fVertices[i] = (float)atof(GetToken(&sArguments));
    }
    unsigned long nSquare = AurAddDrawableSquare(fVertices, 0xffffffff);
    sprintf(returnvalue, "%u", nSquare);
    return returnvalue;
}

char* removesquaretorender(char* sArguments)
{
#ifdef _XBOX

    AurRemoveDrawableSquare(atoi(GetToken(&sArguments)));
    return "SUCKS-EGGS";
#else
    unsigned long nSquare = atoi(GetToken(&sArguments));
    AurRemoveDrawableSquare(nSquare);
    return _itoa(nSquare, returnvalue, 10);
#endif
}
// PC 0x00463180
void AurInitExtensions()
{
    GLRender::InitExtensions();
}
// PC 0x00462F00
int AurDisableShadows()
{
    enableshadows = 0;
    enablesoftshadows = 0;
    return 0;
}

#ifndef _XBOX
int AurQueryMaximumAnisotropy()  // PC 0x004631A0
{
    return (int)largest_supported_anisotropy;
}
#endif
// PC 0x004631B0
bool AurIsSpeedBlurEnabled()
{
    return GLRender::m_bSpeedBlurEnabled;
}
// PC 0x004631C0
void AurEnableSpeedBlur()
{
    GLRender::m_bSpeedBlurEnabled = true;
}
// PC 0x004631D0
void AurDisableSpeedBlur()
{
    GLRender::m_bSpeedBlurEnabled = false;
}

char* startblur(char* sArguments)
{
    AurEnableSpeedBlur();
    return "blah";
}

char* endblur(char* sArguments)
{
    AurDisableSpeedBlur();
    return "blah";
}
// PC 0x004631E0
void AurEnableSaturation()
{
    GLRender::m_bSaturationEnabled = true;
}
// PC 0x004631F0
void AurDisableSaturation()
{
    GLRender::m_bSaturationEnabled = false;
}
// PC 0x00463200
void AurEnableScanNoise()
{
    GLRender::m_bScanNoiseEnabled = true;
}
// PC 0x00463210
void AurDisableScanNoise()
{
    GLRender::m_bScanNoiseEnabled = false;
}
// PC 0x00463220
void AurEnableClairvoyance(bool bLetterbox)
{
    GLRender::EnableDreamDistortion(bLetterbox);
}
// PC 0x00463230
void AurDisableClairvoyance()
{
    GLRender::DisableDreamDistortion();
}
// PC 0x00463240
void AurEnableForceFury(short nLevel)
{
    GLRender::EnableFury(nLevel);
}
// PC 0x00463250
void AurSetSaturationParams(Vector vColor, float fSaturation)
{
#ifdef _XBOX

    GLRender::m_vSaturationMultiplier = vColor;
#else
    GLRender::m_vSaturationMultiplier.m_fX = vColor.m_fX;
    GLRender::m_vSaturationMultiplier.m_fY = vColor.m_fY;
    GLRender::m_vSaturationMultiplier.m_fZ = vColor.m_fZ;
#endif
    GLRender::m_fSaturation = fSaturation;
}
// PC 0x00463280
void AurSetSpeedBlurRatio(float fRatio)
{
    GLRender::m_fAccumulationRatio = fRatio;
}

#ifndef _XBOX
int AurQueryAntiAliasingMode(int nMode)  // PC 0x00463290
{
    if (nMode >= 0)
    {
        if (nMode == 0)
            return 1;
        if (AurIsAAliasingAvailable() != 0)
        {
            int nPixelFormat;
            int nSamples;
            int nSampleAttribute;
            unsigned int nNumFormats;
            float fFloatAttributes[2];
            int nPixelFormatAttributes[30];

            nPixelFormatAttributes[0] = WGL_DRAW_TO_WINDOW_ARB;
            nPixelFormatAttributes[1] = 1;
            nPixelFormatAttributes[2] = WGL_ACCELERATION_ARB;
            nPixelFormatAttributes[3] = WGL_FULL_ACCELERATION_ARB;
            nPixelFormatAttributes[4] = WGL_COLOR_BITS_ARB;
            HDC hDC = GetDC(g_hRenderWnd);
            nPixelFormatAttributes[5] = CAurInternal::m_nColorDepth;
            nPixelFormatAttributes[6] = WGL_ALPHA_BITS_ARB;
            nPixelFormatAttributes[7] = 8;
            nPixelFormatAttributes[8] = WGL_DEPTH_BITS_ARB;
            nPixelFormatAttributes[9] = CAurInternal::m_nDepthBits;
            nPixelFormatAttributes[10] = WGL_STENCIL_BITS_ARB;
            nPixelFormatAttributes[11] = 8;
            nPixelFormatAttributes[12] = WGL_DOUBLE_BUFFER_ARB;
            nPixelFormatAttributes[13] = 1;
            nPixelFormatAttributes[14] = WGL_SAMPLE_BUFFERS_ARB;
            nPixelFormatAttributes[15] = 1;
            nPixelFormatAttributes[16] = WGL_SAMPLES_ARB;
            nPixelFormatAttributes[17] = nMode;
            nPixelFormatAttributes[18] = 0;
            nPixelFormatAttributes[19] = 0;
            fFloatAttributes[0] = 0.0f;
            fFloatAttributes[1] = 0.0f;

            if (wglChoosePixelFormatARB(hDC, nPixelFormatAttributes,
                                       fFloatAttributes, 1, &nPixelFormat,
                                       &nNumFormats) != 0 && nNumFormats > 0)
            {
                nSampleAttribute = WGL_SAMPLES_ARB;
                wglGetPixelFormatAttribivARB(hDC, nPixelFormat, 0, 1,
                                            &nSampleAttribute, &nSamples);
                ReleaseDC(g_hRenderWnd, hDC);
                return nSamples == nMode;
            }
            ReleaseDC(g_hRenderWnd, hDC);
        }
    }
    return 0;
}
// PC 0x00463410
void AurSetClearColor(float fRed, float fGreen, float fBlue, float fAlpha)
{
    glClearColor(fRed, fGreen, fBlue, fAlpha);
}
// PC 0x00463430
void AurGetClearColor(float* pColor)
{
    glGetFloatv(GL_COLOR_CLEAR_VALUE, pColor);
}
// PC 0x00463450
int SetAnisotropicFilter(int nLevel)
{
    if (AurAnisotropicFilterAvailable() != 0 &&
        (float)nLevel <= largest_supported_anisotropy)
    {
        GLRender::cm_nAnisotropy = nLevel;
        g_nCurrentAnisotropy = nLevel;
        return nLevel;
    }

    return g_nCurrentAnisotropy;
}

int AurSetAnisotropy(int nLevel)
{
    SetAnisotropicFilter(nLevel);
    return g_nCurrentAnisotropy;
}

char* setanisotropy(char* sArguments)
{
    int nLevel = atoi(sArguments);
    if (AurAnisotropicFilterAvailable() == 0 ||
        (float)nLevel > largest_supported_anisotropy)
    {
        return "FAILURE";
    }
    GLRender::cm_nAnisotropy = nLevel;
    g_nCurrentAnisotropy = nLevel;
    return "SUCCESS";
}
// PC 0x00463480
extern "C" int AurSetAntiAliasingMode(int nMode)
{
    int nResult = AurQueryAntiAliasingMode(nMode);
    if (nResult == 0)
    {
        return nResult;
    }

    g_nOldAliasingMode = g_nAliasingMode;
    g_nAliasingMode = nMode;
    return 1;
}

char* setaliasing(char* sArguments)
{
    int nMode = atoi(sArguments);
    if (nMode < 0)
    {
        return "FAILURE";
    }
    if (nMode != 0 && (nMode > 7 || AurIsAAliasingAvailable() == 0))
    {
        return "FAILURE";
    }
    g_nOldAliasingMode = g_nAliasingMode;
    g_nAliasingMode = nMode;
    return "SUCCESS";
}
#else

void AurPersistDisplay()
{
    g_pD3DDevice->PersistDisplay();
}

void AurReleaseFrameTexture()
{
    GLRender::ReleaseFrameTexture();
}
#endif

Scene* FindScene(char* sName);

int NodeHasTileData(BSPNode* pNode, void* pData)
{
    return pNode->m_pData->m_pTileData != 0;
}

void AuroraSetTileFoggedByWar(Vector& vPosition, bool bFogged)
{
    Scene* pScene = FindScene("mainscene");
    BSPNode* pNode = BSPSearch(pScene->m_pBSPRoot, vPosition, NodeHasTileData, 0);
#ifdef _XBOX
    nodedata* pData = pNode->m_pData;
    pData->m_pTileData->m_bFade = bFogged;
#else
    pNode->m_pData->m_pTileData->m_bFade = bFogged;
#endif
    if (bFogged)
    {
#ifdef _XBOX
        pData->m_pTileData->m_fFadeAlpha = 0.0f;
#else
        pNode->m_pData->m_pTileData->m_fFadeAlpha = 0.0f;
#endif

        pScene->m_lstPendingObjects.Add(pNode);
    }
    else
    {
        pScene->m_lstPendingObjects.Remove(pNode);
    }
}
