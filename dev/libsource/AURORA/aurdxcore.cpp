#include "../math3d/math3d.h"
#include <new>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "aurora.h"
#include "swmain.h"
#include "aurimage.h"
#include "aurtexman.h"
#include "aurrendergl.h"
#include "aurmodel.h"
#include "aurscene.h"
#include <d3dx8.h>
#include <xgraphics.h>

const unsigned int streamTranslationTable[] =
{
    D3DVSDE_POSITION, D3DVSDE_TEXCOORD0, D3DVSDE_DIFFUSE, D3DVSDE_NORMAL
};

const unsigned int enableTranslationTable[] =
{
    D3DRS_ALPHABLENDENABLE, D3DRS_ALPHATESTENABLE, D3DRS_FOGENABLE, AUR_ENABLE_SPECIAL, AUR_ENABLE_SPECIAL,
    D3DRS_LIGHTING, AUR_ENABLE_SPECIAL, AUR_ENABLE_SPECIAL, AUR_ENABLE_SPECIAL,
    AUR_ENABLE_SPECIAL, D3DRS_STENCILENABLE, D3DRS_ZENABLE,
    AUR_ENABLE_SPECIAL, D3DRS_NORMALIZENORMALS, AUR_ENABLE_SPECIAL,
    D3DRS_FOGSTART, D3DRS_FOGEND, D3DRS_FOGDENSITY, D3DRS_FOGCOLOR
};

const unsigned int queryTranslationTable[] = { D3DRS_FOGENABLE };

const unsigned int blendTranslationTable[] =
{
    D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA, D3DBLEND_ONE, D3DBLEND_ZERO,
    D3DBLEND_SRCCOLOR, D3DBLEND_DESTCOLOR, D3DBLEND_INVDESTALPHA, D3DBLEND_DESTALPHA
};

const unsigned int cmpTranslationTable[] =
{
    D3DCMP_NEVER, D3DCMP_LESS, D3DCMP_EQUAL, D3DCMP_LESSEQUAL,
    D3DCMP_GREATER, D3DCMP_NOTEQUAL, D3DCMP_GREATEREQUAL, D3DCMP_ALWAYS
};

const unsigned int cullTranslationTable[] = { D3DCULL_CCW, D3DCULL_CW };

const unsigned int stencilOpTranslationTable[] =
{
    D3DSTENCILOP_KEEP, D3DSTENCILOP_ZERO, D3DSTENCILOP_REPLACE, D3DSTENCILOP_INCRSAT, D3DSTENCILOP_DECRSAT,
    D3DSTENCILOP_INVERT
};

const unsigned int fillTranslationTable[] = { D3DFILL_POINT, D3DFILL_WIREFRAME, D3DFILL_SOLID };

const unsigned int primTypeTranslationTable[] =
{
    D3DPT_POINTLIST, D3DPT_LINELIST, D3DPT_LINESTRIP, D3DPT_TRIANGLELIST, D3DPT_TRIANGLESTRIP,
    D3DPT_TRIANGLEFAN, D3DPT_QUADLIST, D3DPT_QUADSTRIP, D3DPT_POLYGON, D3DPT_LINELOOP
};

const unsigned int texParamTranslationTable[] =
{
    D3DTSS_ADDRESSU, D3DTSS_ADDRESSV, D3DTSS_MAGFILTER, D3DTSS_MINFILTER
};

const unsigned int textureFormatTranslationTable[] =
{
    D3DFMT_A8R8G8B8, D3DFMT_X8R8G8B8, D3DFMT_A8L8, D3DFMT_L8, D3DFMT_A8B8G8R8,
    D3DFMT_L8, D3DFMT_A8L8, D3DFMT_DXT1, D3DFMT_DXT5, D3DFMT_A4R4G4B4, D3DFMT_LIN_A8R8G8B8,
    D3DFMT_LIN_X8R8G8B8, D3DFMT_R5G6B5, D3DFMT_YUY2, D3DFMT_V16U16
};

void AurReportError(int nLine, const char* sFile, const char* sMessage);

extern int g_bMaterialDirty;
extern D3DMATERIAL8 g_Material;

extern int counttrans;

bool g_bColorMaterial;
int g_nColorMaterialMode = 4;
unsigned short g_nClientState;
DWORD g_aClientStateDeclaration[100];
int g_bCustomVertexShader;
DWORD g_hVertexShaderV;
DWORD g_hVertexShaderVN;
DWORD g_hVertexShaderVNT;
DWORD g_hVertexShaderVNTStreams;
DWORD g_hVertexShaderVNTC;
DWORD g_hVertexShaderVNT2;
DWORD g_hVertexShaderVNT2Streams;
DWORD g_hVertexShaderVNT2C;
DWORD g_hVertexShaderVNT3;
DWORD g_hVertexShaderVNT3C;
DWORD g_hVertexShaderVNT4;
DWORD g_hVertexShaderVNT4C;
float g_fBrightness = 0.05f;
float aquadraticterm = 0.5f;
D3DTexture* g_pFrameTexture;
bool g_bFrameTextureReleased;
int g_nFrameBufferMode;
int g_bFrameBufferModeChanged = 1;
int g_bFrameBufferChanged = 1;
D3DTexture* GLRender::cm_pAttenuationTexture;
D3DXVECTOR4 g_vShadowLightClip;
D3DXVECTOR4 g_vShadowLightView;
int g_nCurrentStencilFail = D3DSTENCILOP_KEEP;
int g_nCurrentStencilZFail = D3DSTENCILOP_KEEP;
int g_nCurrentStencilPass = D3DSTENCILOP_KEEP;
unsigned int GLRender::cm_uiCurrentStage;
unsigned int GLRender::cm_nGLCurrentTID[4];
CAurTexture* GLRender::cm_pFilmNoiseTexture;
CAurTexture* GLRender::cm_pForceDistortionTexture;

LPDIRECT3DDEVICE8 g_pD3DDevice;
bool g_bVertexProgramEnabled;
int g_nTexturingDisabled = 1;
int g_nCurrentBlendSource = -1;
int g_nCurrentBlendDestination = -1;
D3DCOLOR g_nClearColor;
int g_nClearStencil;
float g_fViewPortScaleX = 1.0f;
float g_fViewPortScaleY = 1.0f;
List<D3DVIEWPORT8> g_lstViewPorts;
D3DBaseTexture* GLRender::cm_pCurrentTexture[4];
float* GLRender::cm_pUserShadowData;
unsigned short GLRender::m_UsedVertices[GLRender::MAX_SHADOW_VERTICES];
List<unsigned long> GLRender::cm_listSquaresToRender;
int GLRender::cm_bEnablePausePlane;
bool GLRender::m_bSpeedBlurEnabled;
bool GLRender::m_bSaturationEnabled;
bool GLRender::m_bScanNoiseEnabled;
float GLRender::m_fSaturation;
float GLRender::m_fAccumulationRatio = 0.75f;
Vector GLRender::m_vSaturationMultiplier(1.0f, 1.0f, 1.0f);

int multmatrix = 1;
int enablecolor;
int noclipshadows;
int dontprotectagainstclip = 1;

static const DWORD s_aDeclarationVN[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(2, D3DVSDT_NORMPACKED3),
    D3DVSD_END()
};
static const DWORD s_aDeclarationVNT[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(2, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(9, D3DVSDT_FLOAT2),
    D3DVSD_END()
};
static const DWORD s_aDeclarationVNTStreams[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_STREAM(1),
    D3DVSD_REG(2, D3DVSDT_FLOAT3),
    D3DVSD_STREAM(3),
    D3DVSD_REG(9, D3DVSDT_FLOAT2),
    D3DVSD_END()
};
static const DWORD s_aDeclarationVNTC[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(2, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(9, D3DVSDT_FLOAT2),
    D3DVSD_STREAM(2),
    D3DVSD_REG(3, D3DVSDT_D3DCOLOR),
    D3DVSD_END()
};
static const DWORD s_aDeclarationVNT2[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(2, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(9, D3DVSDT_FLOAT2),
    D3DVSD_REG(10, D3DVSDT_FLOAT2),
    D3DVSD_END()
};
static const DWORD s_aDeclarationVNT2Streams[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_STREAM(1),
    D3DVSD_REG(2, D3DVSDT_FLOAT3),
    D3DVSD_STREAM(3),
    D3DVSD_REG(9, D3DVSDT_FLOAT2),
    D3DVSD_STREAM(4),
    D3DVSD_REG(10, D3DVSDT_FLOAT2),
    D3DVSD_END()
};
static const DWORD s_aDeclarationGrass[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_STREAM(1),
    D3DVSD_REG(2, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(9, D3DVSDT_FLOAT2),
    D3DVSD_REG(10, D3DVSDT_FLOAT2),
    D3DVSD_END()
};
static const DWORD s_aDeclarationVNT2C[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(2, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(9, D3DVSDT_FLOAT2),
    D3DVSD_REG(10, D3DVSDT_FLOAT2),
    D3DVSD_STREAM(2),
    D3DVSD_REG(3, D3DVSDT_D3DCOLOR),
    D3DVSD_END()
};
static const DWORD s_aDeclarationVNT3[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(2, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(9, D3DVSDT_FLOAT2),
    D3DVSD_REG(10, D3DVSDT_FLOAT2),
    D3DVSD_REG(11, D3DVSDT_FLOAT2),
    D3DVSD_END()
};
static const DWORD s_aDeclarationVNT3C[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(2, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(9, D3DVSDT_FLOAT2),
    D3DVSD_REG(10, D3DVSDT_FLOAT2),
    D3DVSD_REG(11, D3DVSDT_FLOAT2),
    D3DVSD_STREAM(2),
    D3DVSD_REG(3, D3DVSDT_D3DCOLOR),
    D3DVSD_END()
};
static const DWORD s_aDeclarationVNT4[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(2, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(9, D3DVSDT_FLOAT2),
    D3DVSD_REG(10, D3DVSDT_FLOAT2),
    D3DVSD_REG(11, D3DVSDT_FLOAT2),
    D3DVSD_REG(12, D3DVSDT_FLOAT2),
    D3DVSD_END()
};
static const DWORD s_aDeclarationVNT4C[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(2, D3DVSDT_NORMPACKED3),
    D3DVSD_REG(9, D3DVSDT_FLOAT2),
    D3DVSD_REG(10, D3DVSDT_FLOAT2),
    D3DVSD_REG(11, D3DVSDT_FLOAT2),
    D3DVSD_REG(12, D3DVSDT_FLOAT2),
    D3DVSD_STREAM(2),
    D3DVSD_REG(3, D3DVSDT_D3DCOLOR),
    D3DVSD_END()
};
static const DWORD s_aDeclarationV[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_END()
};

unsigned long TranslatePoolUsage(unsigned long nUsage)
{
    unsigned long nD3DUsage = 0;
    if (nUsage & 2)
    {
        nD3DUsage = D3DUSAGE_DYNAMIC;
    }
    if (nUsage & 0x40)
    {
        nD3DUsage |= D3DUSAGE_WRITEONLY;
    }
    if (nUsage & 0x80)
    {
        nD3DUsage |= D3DUSAGE_DYNAMIC;
    }
    return nD3DUsage;
}

unsigned long TranslatePoolFormat(unsigned long nFormat)
{
    unsigned long nFVF = 0;
    if (nFormat & 1)
    {
        nFVF = D3DFVF_DIFFUSE;
    }
    if (nFormat & 2)
    {
        nFVF |= D3DFVF_NORMAL;
    }
    if (nFormat & 8)
    {
        nFVF |= D3DFVF_SPECULAR;
    }
    if (nFormat & 0x10)
    {
        nFVF |= D3DFVF_XYZ;
    }
    if (nFormat & 0x1000)
    {
        nFVF |= D3DFVF_TEX1;
    }
    if (nFormat & 0x2000)
    {
        nFVF |= D3DFVF_TEX2;
    }
    if (nFormat & 0x4000)
    {
        nFVF |= D3DFVF_TEX3;
    }
    return nFVF;
}

unsigned long TranslatePoolLockFlags(unsigned long nFlags)
{
    unsigned long nD3DFlags = 0;
    if (nFlags & 2)
    {
        nD3DFlags = D3DLOCK_NOOVERWRITE;
    }
    if (nFlags & 8)
    {
        nD3DFlags |= D3DLOCK_READONLY;
    }
    return nD3DFlags;
}

int GetPrimitiveCount(int nPrimitive, int nVertices)
{
    switch (nPrimitive)
    {
    case Aurora::PRIMITIVE_TRIANGLE_STRIP:
    case Aurora::PRIMITIVE_TRIANGLE_FAN:
        return nVertices - 2;
    case Aurora::PRIMITIVE_TRIANGLES:
        return nVertices / 3;
    case Aurora::PRIMITIVE_POINTS:
        return nVertices;
    case Aurora::PRIMITIVE_LINES:
        return nVertices / 2;
    case Aurora::PRIMITIVE_LINE_STRIP:
        return nVertices - 1;
    case Aurora::PRIMITIVE_QUADS:
        return nVertices / 4;
    }
    return 0;
}

D3DResource* RequestPoolManager::GetPool(unsigned long nPool)
{
    if (nPool == (unsigned long)INVALID_POOL)
    {
        return NULL;
    }
    if (nPool & POOL_VERTEX_FLAG)
    {
        return m_ppVertexPools[nPool & POOL_INDEX_MASK];
    }
    return m_ppIndexPools[nPool];
}

Vector Quaternion::axis() const
{
    Vector vAxis(m_fX, m_fY, m_fZ);
    float dAngle = acos(m_fW) * 2.0;
    if (AbsoluteValue(dAngle) < 1e-7)
    {
        return Vector(1.0f, 0.0f, 0.0f);
    }
    float fScale = (float)(1.0 / sin(dAngle * 0.5));
    return Vector(vAxis.m_fX * fScale, vAxis.m_fY * fScale, vAxis.m_fZ * fScale);
}

void GLRender::SetColor4f(float fRed, float fGreen, float fBlue, float fAlpha)
{
    D3DCOLORVALUE color;
    color.r = fRed;
    color.g = fGreen;
    color.b = fBlue;
    g_bMaterialDirty = 1;
    color.a = fAlpha;
    if (!g_bColorMaterial)
    {
        g_Material.Diffuse = color;
    }
    else
    {
        switch (g_nColorMaterialMode)
        {
        case 0:
            g_Material.Emissive = color;
            break;
        case 1:
            g_Material.Ambient = color;
            break;
        case 2:
            g_Material.Diffuse = color;
            break;
        case 3:
            g_Material.Specular = color;
            break;
        case 4:
            g_Material.Ambient = color;
            g_Material.Diffuse = color;
            break;
        }
    }
}

void GLRender::SetColorv(const float* pColor)
{
    SetColor4f(pColor[0], pColor[1], pColor[2], pColor[3]);
}

void GLRender::CopyTextureToTextureMemory(void* pImage, D3DLOCKED_RECT* pLock, int nWidth, int nHeight, int nFormat)
{
    char szMessage[256];
    int bLinear = 0;
    int x;
    int y;
    unsigned char* pBuffer;

    if (nFormat == 10)
    {
        bLinear = 1;
    }
    else if (nFormat == 11)
    {
        bLinear = 1;
        nFormat = 1;
    }
    if (nFormat == 3)
    {
        XGSwizzleRect(pImage, 0, NULL, pLock->pBits, nWidth, nHeight, NULL, 1);
    }
    else if (nFormat == 2)
    {
        XGSwizzleRect(pImage, 0, NULL, pLock->pBits, nWidth, nHeight, NULL, 2);
    }
    else if (nFormat == 0)
    {
        XGSwizzleRect(pImage, 0, NULL, pLock->pBits, nWidth, nHeight, NULL, 4);
    }
    else if (nFormat == 4)
    {
        unsigned char* pPixel = (unsigned char*)pImage;
        for (y = 0; y < nHeight; y++)
        {
            for (x = 0; x < nWidth; x++)
            {
                unsigned char nRed = pPixel[(y * nWidth + x) * 4];
                pPixel[(y * nWidth + x) * 4] = pPixel[(y * nWidth + x) * 4 + 2];
                pPixel[(y * nWidth + x) * 4 + 2] = nRed;
            }
        }
        XGSwizzleRect(pImage, 0, NULL, pLock->pBits, nWidth, nHeight, NULL, 4);
    }
    else if (nFormat == 10)
    {
        memcpy(pLock->pBits, pImage, nWidth * nHeight * 4);
    }
    else if (nFormat == 1)
    {
        int nSize = nWidth * nHeight * 4;
        pBuffer = new unsigned char[nSize];
        memset(pBuffer, 0, nSize);
        unsigned char* pSource = (unsigned char*)pImage;
        for (y = 0; y < nHeight; y++)
        {
            for (x = 0; x < nWidth; x++)
            {
                pBuffer[(y * nWidth + x) * 4] = pSource[(y * nWidth + x) * 3];
                pBuffer[(y * nWidth + x) * 4 + 1] = pSource[(y * nWidth + x) * 3 + 1];
                pBuffer[(y * nWidth + x) * 4 + 2] = pSource[(y * nWidth + x) * 3 + 2];
            }
        }
        if (bLinear)
        {

            memcpy(pLock->pBits, pImage, nSize);
            delete[] pBuffer;
        }
        else
        {
            XGSwizzleRect(pBuffer, 0, NULL, pLock->pBits, nWidth, nHeight, NULL, 4);
            delete[] pBuffer;
        }
    }
    else if (nFormat == 12)
    {
        pBuffer = new unsigned char[nWidth * nHeight * 2];
        unsigned char* pSource = (unsigned char*)pImage;
        for (y = 0; y < nHeight; y++)
        {
            for (x = 0; x < nWidth; x++)
            {
                unsigned char nBlue = (unsigned int)(pSource[(y * nWidth + x) * 4] + 4) < 255
                                          ? pSource[(y * nWidth + x) * 4] + 4
                                          : 255;
                unsigned char nGreen = (unsigned int)(pSource[(y * nWidth + x) * 4 + 1] + 2) < 255
                                           ? pSource[(y * nWidth + x) * 4 + 1] + 2
                                           : 255;
                unsigned char nRed = (unsigned int)(pSource[(y * nWidth + x) * 4 + 2] + 4) < 255
                                         ? pSource[(y * nWidth + x) * 4 + 2] + 4
                                         : 255;
                ((unsigned short*)pBuffer)[y * nWidth + x] = ((nRed >> 3) << 11) | ((nGreen >> 2) << 5) | (nBlue >> 3);
            }
        }
        XGSwizzleRect(pBuffer, 0, NULL, pLock->pBits, nWidth, nHeight, NULL, 2);
        delete[] pBuffer;
    }
    else if (nFormat == 13)
    {
        memcpy(pLock->pBits, pImage, pLock->Pitch * nHeight);
    }
    else if (nFormat == 14)
    {
        XGSwizzleRect(pImage, 0, NULL, pLock->pBits, nWidth, nHeight, NULL, 4);
    }
    else
    {
        sprintf(szMessage, "texture format: %d not implemented", nFormat);
        AurReportError(2996, "aurdxcore.cpp", szMessage);
    }
}

int GetCompressedContactSheetOffset(int nIndex, int nWidth, int nBytesPerPixel)
{
    int nMipWidth = nWidth;
    int nBlockSize = nBytesPerPixel == 4 ? 16 : 8;
    int nSize = nBlockSize * ((nWidth + 3) / 4) * ((nWidth + 3) / 4);

    while (nMipWidth > 1)
    {
        nMipWidth >>= 1;
        nSize += nBlockSize * ((nMipWidth + 3) / 4) * ((nMipWidth + 3) / 4);
    }

    if (nSize % 128 != 0)
    {
        nSize += 128 - nSize % 128;
    }

    return nIndex * nSize;
}

void GLRender::EnableClientState(int nArray)
{
    switch (nArray)
    {
    case CLIENT_STATE_TEXTURE_COORD:
        switch (cm_uiCurrentStage)
        {
        case 3:
            g_nClientState |= 0x40;
            break;
        case 2:
            g_nClientState |= 0x20;
            break;
        case 1:
            g_nClientState |= 0x10;
            break;
        case 0:
            g_nClientState |= 8;
            break;
        }
        break;
    case CLIENT_STATE_VERTEX:
        g_nClientState |= 1;
        break;
    case CLIENT_STATE_COLOR:
        g_nClientState |= 4;
        g_pD3DDevice->SetRenderState(D3DRS_COLORVERTEX, TRUE);
        g_pD3DDevice->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_COLOR1);
        break;
    case CLIENT_STATE_NORMAL:
        g_nClientState |= 2;
        break;
    }
}

void GLRender::DisableClientState(int nArray)
{
    switch (nArray)
    {
    case CLIENT_STATE_TEXTURE_COORD:
        switch (cm_uiCurrentStage)
        {
        case 0:
            g_nClientState &= ~8;
            cm_pCurrentTexture[0] = NULL;
            break;
        case 1:
            g_nClientState &= ~0x10;
            cm_pCurrentTexture[1] = NULL;
            break;
        case 2:
            g_nClientState &= ~0x20;
            cm_pCurrentTexture[2] = NULL;
            break;
        case 3:
            g_nClientState &= ~0x40;
            cm_pCurrentTexture[3] = NULL;
            break;
        }
        break;
    case CLIENT_STATE_VERTEX:
        g_nClientState &= ~1;
        break;
    case CLIENT_STATE_COLOR:
        g_nClientState &= ~4;
        g_pD3DDevice->SetRenderState(D3DRS_COLORVERTEX, FALSE);
        g_pD3DDevice->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);
        break;
    case CLIENT_STATE_NORMAL:
        g_nClientState &= ~2;
        break;
    }
}

int GLRender::InitializeVertexShaders()
{
    g_pD3DDevice->CreateVertexShader(s_aDeclarationVN, NULL, &g_hVertexShaderVN, 0);
    g_pD3DDevice->CreateVertexShader(s_aDeclarationV, NULL, &g_hVertexShaderV, 0);
    g_pD3DDevice->CreateVertexShader(s_aDeclarationVNT, NULL, &g_hVertexShaderVNT, 0);
    g_pD3DDevice->CreateVertexShader(s_aDeclarationVNTStreams, NULL, &g_hVertexShaderVNTStreams, 0);
    g_pD3DDevice->CreateVertexShader(s_aDeclarationVNTC, NULL, &g_hVertexShaderVNTC, 0);
    g_pD3DDevice->CreateVertexShader(s_aDeclarationVNT2, NULL, &g_hVertexShaderVNT2, 0);
    g_pD3DDevice->CreateVertexShader(s_aDeclarationVNT2Streams, NULL, &g_hVertexShaderVNT2Streams, 0);
    g_pD3DDevice->CreateVertexShader(s_aDeclarationGrass, NULL, &g_hGrassVertexShader, 0);
    g_pD3DDevice->CreateVertexShader(s_aDeclarationVNT2C, NULL, &g_hVertexShaderVNT2C, 0);
    g_pD3DDevice->CreateVertexShader(s_aDeclarationVNT3, NULL, &g_hVertexShaderVNT3, 0);
    g_pD3DDevice->CreateVertexShader(s_aDeclarationVNT3C, NULL, &g_hVertexShaderVNT3, 0);
    g_pD3DDevice->CreateVertexShader(s_aDeclarationVNT4, NULL, &g_hVertexShaderVNT4, 0);
    g_pD3DDevice->CreateVertexShader(s_aDeclarationVNT4C, NULL, &g_hVertexShaderVNT4C, 0);
    return 1;
}

DWORD GLRender::CreateClientStateVertexShader()
{
    DWORD hShader;
    int nCount = 0;
    if (g_nClientState & 1)
    {
        g_aClientStateDeclaration[nCount++] = D3DVSD_STREAM(0);
        g_aClientStateDeclaration[nCount++] = D3DVSD_REG(0, D3DVSDT_FLOAT3);
    }
    if (g_nClientState & 2)
    {
        g_aClientStateDeclaration[nCount++] = D3DVSD_STREAM(0);
        g_aClientStateDeclaration[nCount++] = D3DVSD_REG(2, D3DVSDT_NORMPACKED3);
    }
    if (g_nClientState & 4)
    {
        g_aClientStateDeclaration[nCount++] = D3DVSD_STREAM(0);
        g_aClientStateDeclaration[nCount++] = D3DVSD_REG(3, D3DVSDT_D3DCOLOR);
    }
    if (g_nClientState & 8)
    {
        g_aClientStateDeclaration[nCount++] = D3DVSD_STREAM(0);
        g_aClientStateDeclaration[nCount++] = D3DVSD_REG(9, D3DVSDT_FLOAT2);
    }
    if (g_nClientState & 0x10)
    {
        g_aClientStateDeclaration[nCount++] = D3DVSD_STREAM(0);
        g_aClientStateDeclaration[nCount++] = D3DVSD_REG(10, D3DVSDT_FLOAT2);
    }
    if (g_nClientState & 0x20)
    {
        g_aClientStateDeclaration[nCount++] = D3DVSD_STREAM(0);
        g_aClientStateDeclaration[nCount++] = D3DVSD_REG(11, D3DVSDT_FLOAT2);
    }
    if (g_nClientState & 0x40)
    {
        g_aClientStateDeclaration[nCount++] = D3DVSD_STREAM(0);
        g_aClientStateDeclaration[nCount++] = D3DVSD_REG(12, D3DVSDT_FLOAT2);
    }
    if (g_nClientState & 0x80)
    {
        g_aClientStateDeclaration[nCount++] = D3DVSD_STREAM(7);
        g_aClientStateDeclaration[nCount++] = D3DVSD_REG(1, D3DVSDT_FLOAT4);
        g_aClientStateDeclaration[nCount++] = D3DVSD_STREAM(8);
        g_aClientStateDeclaration[nCount++] = D3DVSD_REG(10, D3DVSDT_FLOAT2);
    }
    g_aClientStateDeclaration[nCount] = D3DVSD_END();
    g_pD3DDevice->CreateVertexShader(g_aClientStateDeclaration, NULL, &hShader, 0);
    return hShader;
}

DWORD GLRender::GetVertexShader(int bSeparateStreams)
{
    static int bVertexShadersCreated = InitializeVertexShaders();

    switch (g_nClientState)
    {
    case 1:
        return g_hVertexShaderV;
    case 11:
        if (bSeparateStreams)
        {
            return g_hVertexShaderVNTStreams;
        }
        return g_hVertexShaderVNT;
    case 15:
        return g_hVertexShaderVNTC;
    case 27:
        if (bSeparateStreams)
        {
            return g_hVertexShaderVNT2Streams;
        }
        return g_hVertexShaderVNT2;
    case 31:
        return g_hVertexShaderVNT2C;
    case 59:
        return g_hVertexShaderVNT3;
    case 63:
        return g_hVertexShaderVNT3C;
    case 123:
        return g_hVertexShaderVNT4;
    case 127:
        return g_hVertexShaderVNT4C;
    case 3:
        return g_hVertexShaderVN;
    }
    g_bCustomVertexShader = 1;
    return CreateClientStateVertexShader();
}

void GLRender::PushCurrentMatrix()
{
    g_nMatrixStackDepth++;
    switch (g_nMatrixMode)
    {
    case 2:
        g_pWorldMatrixStack->Push();
        break;
    case 1:
        g_pViewMatrixStack->Push();
        break;
    case 3:
        g_pTextureMatrixStack->Push();
        break;
    case 0:
        g_pProjectionMatrixStack->Push();
        break;
    }
}

void GLRender::SetActiveTexture(unsigned long nStage)
{
    cm_uiCurrentStage = nStage;
}

void GLRender::SetTextureStageFlag(unsigned long nStage, int nValue)
{
    cm_nGLCurrentTID[nStage] = nValue;
}

unsigned int GLRender::GetCurrentStageTID()
{
    return cm_nGLCurrentTID[cm_uiCurrentStage];
}

DXVertexProgram::DXVertexProgram()
{
    m_pConstants = NULL;
    m_pCompiledShader = NULL;
    m_pErrors = NULL;
    m_hShader = 0;
}

void GLRender::SetOrthoTransform(double fLeft, double fRight, double fBottom, double fTop, double fNear, double fFar)
{
    if (g_pD3DDevice)
    {
        D3DXMATRIX mProjection;
        D3DXMatrixOrthoOffCenterRH(&mProjection, (float)fLeft, (float)fRight, (float)fBottom, (float)fTop,
                                   (float)fNear, (float)fFar);
        g_pD3DDevice->SetTransform(D3DTS_PROJECTION, &mProjection);
        g_pProjectionMatrixStack->LoadMatrix(&mProjection);
    }
}

static float AdjustBrightness(float fValue)
{
    fValue += g_fBrightness;
    if (fValue > 1.0f)
    {
        fValue = 1.0f;
    }
    else if (fValue < 0.0f)
    {
        fValue = 0.0f;
    }
    float fResult;
    if (fValue < 0.5)
    {
        fResult = ((1.0f - aquadraticterm) + aquadraticterm * fValue * 2.0f) * fValue;
    }
    else
    {
        fResult = 1.0f - (1.0f - fValue) * (1.0f - fValue) * aquadraticterm * 2.0f - (1.0f - aquadraticterm) * (1.0f - fValue);
    }
    if (fResult > 1.0f)
    {
        return 1.0f;
    }
    if (fResult < 0.0f)
    {
        return 0.0f;
    }
    return fResult;
}

void GLRender::SetFilmNoiseTexture(CAurTexture* pTexture)
{
    cm_pFilmNoiseTexture = pTexture;
}

void GLRender::SetForceDistortionTexture(CAurTexture* pTexture)
{
    cm_pForceDistortionTexture = pTexture;
}

void GLRender::ReleaseFrameTexture()
{
    g_bFrameTextureReleased = true;
    if (g_pFrameTexture)
    {
        g_pFrameTexture->Release();
    }
    g_pFrameTexture = NULL;
}

void GLRender::SetFrameBufferMode(int nMode)
{
    g_nFrameBufferMode = nMode;
    g_bFrameBufferModeChanged = 1;
    g_bFrameBufferChanged = 1;
}

void GLRender::InitAttenuationTexture()
{
    unsigned char pixels[64 * 64 * 4];

    for (int y = 0; y < 64; y++)
    {
        for (int x = 0; x < 64; x++)
        {
            float fX = (float)(x - 32) * 0.125f;
            float fY = (float)(y - 32) * 0.125f;
            float fValue = 1.0f / ((fX * fX + fY * fY) * 4.5f + 1.0f);
            if (x == 0 || y == 0 || x == 63 || y == 63)
            {
                fValue = 0.0f;
            }
            unsigned char nValue = (unsigned char)(fValue * 255.0f);
            pixels[(y * 64 + x) * 4 + 0] = nValue;
            pixels[(y * 64 + x) * 4 + 1] = nValue;
            pixels[(y * 64 + x) * 4 + 2] = nValue;
            pixels[(y * 64 + x) * 4 + 3] = 0xFF;
        }
    }

    g_pD3DDevice->CreateTexture(64, 64, 1, 0, D3DFMT_A8R8G8B8, 0, &cm_pAttenuationTexture);
    D3DSurface* pSurface;
    cm_pAttenuationTexture->GetSurfaceLevel(0, &pSurface);
    D3DLOCKED_RECT lock;
    pSurface->LockRect(&lock, NULL, 0);
    XGSwizzleRect(pixels, 0, NULL, lock.pBits, 64, 64, NULL, 4);
    pSurface->Release();
}

void GLRender::SetShadowLightPosition(const Vector& vPosition)
{
    Vector vLight = vPosition;
    D3DXMATRIX mViewProjection;
    D3DXMatrixMultiply(&mViewProjection, g_pViewMatrixStack->GetTop(), g_pProjectionMatrixStack->GetTop());
    D3DXVECTOR4 vLight4(vLight.m_fX, vLight.m_fY, vLight.m_fZ, 1.0f);
    D3DXVec4Transform(&g_vShadowLightClip, &vLight4, &mViewProjection);
    vLight4.x = vLight.m_fX;
    vLight4.y = vLight.m_fY;
    vLight4.z = vLight.m_fZ;
    vLight4.w = 1.0f;
    D3DXVec4Transform(&g_vShadowLightView, &vLight4, g_pViewMatrixStack->GetTop());
}

char* AllocateShadowMemory(int nSize)
{
    static char* data = new char[nSize];
    static int c_size = nSize;

    if (nSize > maxshadowvar)
    {
        maxshadowvar = nSize;
    }

    if (nSize > c_size)
    {
        delete[] data;
        data = new char[nSize];
        c_size = nSize;
    }

    return data;
}

void GLRender::Rotate(float fAngle, float fX, float fY, float fZ)
{
    switch (g_nMatrixMode)
    {
    case 3:
        g_pTextureMatrixStack->RotateAxisLocal(&D3DXVECTOR3(fX, fY, fZ), fAngle);
        break;
    case 2:
        g_pWorldMatrixStack->RotateAxisLocal(&D3DXVECTOR3(fX, fY, fZ), fAngle);
        break;
    case 1:
        g_pViewMatrixStack->RotateAxisLocal(&D3DXVECTOR3(fX, fY, fZ), fAngle);
        break;
    }
}

void GLRender::Translate(float fX, float fY, float fZ)
{
    switch (g_nMatrixMode)
    {
    case 0:
        g_pProjectionMatrixStack->TranslateLocal(fX, fY, fZ);
        g_pD3DDevice->SetTransform(D3DTS_PROJECTION, g_pProjectionMatrixStack->GetTop());
        break;
    case 2:
        g_pWorldMatrixStack->TranslateLocal(fX, fY, fZ);
        break;
    case 1:
        g_pViewMatrixStack->TranslateLocal(fX, fY, fZ);
        break;
    case 3:
        g_pTextureMatrixStack->TranslateLocal(fX, fY, fZ);
        break;
    }
}

void GLRender::MultMatrix(const float* pMatrix)
{
    D3DXMATRIX mMatrix(pMatrix);
    switch (g_nMatrixMode)
    {
    case 2:
        g_pWorldMatrixStack->MultMatrixLocal(&mMatrix);
        break;
    case 1:
        g_pViewMatrixStack->MultMatrixLocal(&mMatrix);
        break;
    case 3:
        g_pTextureMatrixStack->MultMatrixLocal(&mMatrix);
        break;
    case 0:
        g_pProjectionMatrixStack->MultMatrixLocal(&mMatrix);
        break;
    }
}

void GLRender::Scale(float fX, float fY, float fZ)
{
    if (fX != 1.0f || fY != 1.0f || fZ != 1.0f)
    {
        switch (g_nMatrixMode)
        {
        case 2:
            g_pWorldMatrixStack->ScaleLocal(fX, fY, fZ);
            break;
        case 1:
            g_pViewMatrixStack->ScaleLocal(fX, fY, fZ);
            break;
        case 3:
            g_pTextureMatrixStack->ScaleLocal(fX, fY, fZ);
            break;
        case 0:
            g_pProjectionMatrixStack->ScaleLocal(fX, fY, fZ);
            break;
        }
    }
}

void GLRender::SetStencilOp(int nFail, int nZFail, int nPass)
{
    if (stencilOpTranslationTable[nFail] != g_nCurrentStencilFail)
    {
        g_pD3DDevice->SetRenderState(D3DRS_STENCILFAIL, stencilOpTranslationTable[nFail]);
        g_nCurrentStencilFail = stencilOpTranslationTable[nFail];
    }
    if (stencilOpTranslationTable[nZFail] != g_nCurrentStencilZFail)
    {
        g_pD3DDevice->SetRenderState(D3DRS_STENCILZFAIL, stencilOpTranslationTable[nZFail]);
        g_nCurrentStencilZFail = stencilOpTranslationTable[nZFail];
    }
    if (stencilOpTranslationTable[nPass] != g_nCurrentStencilPass)
    {
        g_pD3DDevice->SetRenderState(D3DRS_STENCILPASS, stencilOpTranslationTable[nPass]);
        g_nCurrentStencilPass = stencilOpTranslationTable[nPass];
    }
}

void RequestPoolManager::ResizePool(unsigned long nPool, unsigned long nSize, D3DDevice* pDevice)
{
    if (nPool == (unsigned long)INVALID_POOL)
    {
        return;
    }
    if (nPool & POOL_VERTEX_FLAG)
    {
        D3DVertexBuffer* pOldBuffer = m_ppVertexPools[nPool & POOL_INDEX_MASK];
        D3DVertexBuffer* pNewBuffer;
        pDevice->CreateVertexBuffer(nSize, 0, 0, D3DPOOL_DEFAULT, &pNewBuffer);
        BYTE* pSource;
        pOldBuffer->Lock(0, 0, &pSource, D3DLOCK_READONLY);
        BYTE* pDest;
        pNewBuffer->Lock(0, 0, &pDest, 0);
        memcpy(pDest, pSource, m_pVertexPoolSize[nPool & POOL_INDEX_MASK]);
        pOldBuffer->Release();
        totalvertexpool += nSize - m_pVertexPoolSize[nPool & POOL_INDEX_MASK];
        totalvertexpool4k += ((nSize + POOL_PAGE_ROUNDING) & POOL_PAGE_MASK) -
                                  ((m_pVertexPoolSize[nPool & POOL_INDEX_MASK] + POOL_PAGE_ROUNDING) & POOL_PAGE_MASK);
        m_ppVertexPools[nPool & POOL_INDEX_MASK] = pNewBuffer;
        m_pVertexPoolSize[nPool & POOL_INDEX_MASK] = nSize;
    }
    else
    {
        D3DIndexBuffer* pOldBuffer = m_ppIndexPools[nPool];
        D3DIndexBuffer* pNewBuffer;
        pDevice->CreateIndexBuffer(nSize, 0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &pNewBuffer);
        BYTE* pSource;
        pOldBuffer->Lock(0, 0, &pSource, 0);
        BYTE* pDest;
        pNewBuffer->Lock(0, 0, &pDest, 0);
        memcpy(pDest, pSource, m_pIndexPoolSize[nPool]);
        pOldBuffer->Release();
        totalindexpool += nSize - m_pIndexPoolSize[nPool];
        totalindexpool4 += ((nSize + POOL_INDEX_ROUNDING) & POOL_INDEX_ROUND_MASK) -
                                    ((m_pIndexPoolSize[nPool] + POOL_INDEX_ROUNDING) & POOL_INDEX_ROUND_MASK);
        m_ppIndexPools[nPool] = pNewBuffer;
        m_pIndexPoolSize[nPool] = nSize;
    }
}

void* RequestPoolManager::LockPool(unsigned long nPool, unsigned long nOffset, unsigned long nSize,
                                   unsigned long nFlags)
{
    if (nPool == (unsigned long)INVALID_POOL)
    {
        return NULL;
    }
    BYTE* pData;
    if (nPool & POOL_VERTEX_FLAG)
    {
        nPool &= POOL_INDEX_MASK;
        if (nFlags == POOL_LOCK_READ_ONLY)
        {
            m_ppVertexPools[nPool]->Lock(0, 0, &pData, D3DLOCK_READONLY);
            return pData;
        }
        m_ppVertexPools[nPool]->Lock(0, 0, &pData, 0);
        return pData;
    }
    m_ppIndexPools[nPool]->Lock(nOffset, 0, &pData, 0);
    return pData;
}

void RequestPoolManager::ReleasePools()
{
    unsigned int nPool;
    for (nPool = 0; nPool < m_nVertexPools; nPool++)
    {
        if (m_ppVertexPools[nPool])
        {
            m_ppVertexPools[nPool]->Release();
            m_ppVertexPools[nPool] = NULL;
        }
    }
    for (nPool = 0; nPool < m_nIndexPools; nPool++)
    {
        if (m_ppIndexPools[nPool])
        {
            m_ppIndexPools[nPool]->Release();
            m_ppIndexPools[nPool] = NULL;
        }
    }
}

RequestPoolManager::~RequestPoolManager()
{
    delete[] m_ppVertexPools;
    delete[] m_ppIndexPools;
    delete[] m_pVertexPoolSize;
    delete[] m_pIndexPoolSize;
}

struct DXShapeVertex
{
    float x;
    float y;
    float z;
    D3DCOLOR color;
};

void DXBox::Render(D3DDevice* pDevice)
{
    pDevice->SetStreamSource(0, m_pVertexBuffer, sizeof(DXShapeVertex));
    pDevice->SetVertexShader(D3DFVF_XYZ | D3DFVF_DIFFUSE);
    pDevice->SetRenderState(D3DRS_COLORVERTEX, TRUE);
    pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 12);
    pDevice->SetRenderState(D3DRS_COLORVERTEX, FALSE);
}

DXBox::DXBox(D3DDevice* pDevice)
{
    DXShapeVertex vertices[36];
    vertices[0].x = 0.0f;
    vertices[0].y = 0.0f;
    vertices[0].z = 0.0f;
    vertices[1].x = 1.0f;
    vertices[1].y = 0.0f;
    vertices[1].z = 0.0f;
    vertices[2].x = 0.0f;
    vertices[2].y = 0.0f;
    vertices[2].z = 1.0f;
    vertices[3].x = 0.0f;
    vertices[3].y = 0.0f;
    vertices[3].z = 1.0f;
    vertices[4].x = 1.0f;
    vertices[4].y = 0.0f;
    vertices[4].z = 0.0f;
    vertices[5].x = 1.0f;
    vertices[5].y = 0.0f;
    vertices[5].z = 1.0f;
    vertices[6].x = 0.0f;
    vertices[6].y = 0.0f;
    vertices[6].z = 0.0f;
    vertices[7].x = 0.0f;
    vertices[7].y = 0.0f;
    vertices[7].z = 1.0f;
    vertices[8].x = 0.0f;
    vertices[8].y = 1.0f;
    vertices[8].z = 0.0f;
    vertices[9].x = 0.0f;
    vertices[9].y = 1.0f;
    vertices[9].z = 0.0f;
    vertices[10].x = 0.0f;
    vertices[10].y = 0.0f;
    vertices[10].z = 1.0f;
    vertices[11].x = 0.0f;
    vertices[11].y = 1.0f;
    vertices[11].z = 1.0f;
    vertices[12].x = 0.0f;
    vertices[12].y = 1.0f;
    vertices[12].z = 0.0f;
    vertices[13].x = 0.0f;
    vertices[13].y = 1.0f;
    vertices[13].z = 1.0f;
    vertices[14].x = 1.0f;
    vertices[14].y = 1.0f;
    vertices[14].z = 0.0f;
    vertices[15].x = 1.0f;
    vertices[15].y = 1.0f;
    vertices[15].z = 0.0f;
    vertices[16].x = 0.0f;
    vertices[16].y = 1.0f;
    vertices[16].z = 1.0f;
    vertices[17].x = 1.0f;
    vertices[17].y = 1.0f;
    vertices[17].z = 1.0f;
    vertices[18].x = 1.0f;
    vertices[18].y = 0.0f;
    vertices[18].z = 0.0f;
    vertices[19].x = 1.0f;
    vertices[19].y = 1.0f;
    vertices[19].z = 0.0f;
    vertices[20].x = 1.0f;
    vertices[20].y = 0.0f;
    vertices[20].z = 1.0f;
    vertices[21].x = 1.0f;
    vertices[21].y = 0.0f;
    vertices[21].z = 1.0f;
    vertices[22].x = 1.0f;
    vertices[22].y = 1.0f;
    vertices[22].z = 0.0f;
    vertices[23].x = 1.0f;
    vertices[23].y = 1.0f;
    vertices[23].z = 1.0f;
    vertices[24].x = 1.0f;
    vertices[24].y = 0.0f;
    vertices[24].z = 0.0f;
    vertices[25].x = 0.0f;
    vertices[25].y = 0.0f;
    vertices[25].z = 0.0f;
    vertices[26].x = 0.0f;
    vertices[26].y = 1.0f;
    vertices[26].z = 0.0f;
    vertices[27].x = 1.0f;
    vertices[27].y = 0.0f;
    vertices[27].z = 0.0f;
    vertices[28].x = 0.0f;
    vertices[28].y = 1.0f;
    vertices[28].z = 0.0f;
    vertices[29].x = 1.0f;
    vertices[29].y = 1.0f;
    vertices[29].z = 0.0f;
    vertices[30].x = 0.0f;
    vertices[30].y = 1.0f;
    vertices[30].z = 1.0f;
    vertices[31].x = 0.0f;
    vertices[31].y = 0.0f;
    vertices[31].z = 1.0f;
    vertices[32].x = 1.0f;
    vertices[32].y = 0.0f;
    vertices[32].z = 1.0f;
    vertices[33].x = 0.0f;
    vertices[33].y = 1.0f;
    vertices[33].z = 1.0f;
    vertices[34].x = 1.0f;
    vertices[34].y = 0.0f;
    vertices[34].z = 1.0f;
    vertices[35].x = 1.0f;
    vertices[35].y = 1.0f;
    vertices[35].z = 1.0f;
    for (int i = 0; i < 36; i++)
    {
        vertices[i].color = 0xFFFFFF00;
    }
    if (FAILED(pDevice->CreateVertexBuffer(sizeof(vertices), 0, D3DFVF_XYZ | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT,
                                           &m_pVertexBuffer)))
    {
        return;
    }
    BYTE* pData;
    m_pVertexBuffer->Lock(0, 0, &pData, 0);
    memcpy(pData, vertices, sizeof(vertices));
}

DXCircle::~DXCircle()
{
    m_pVertexBuffer->Release();
}

void DXCircle::Render(D3DDevice* pDevice)
{
    pDevice->SetStreamSource(0, m_pVertexBuffer, sizeof(DXShapeVertex));
    pDevice->SetVertexShader(D3DFVF_XYZ | D3DFVF_DIFFUSE);
    pDevice->DrawPrimitive(D3DPT_LINESTRIP, 0, 8);
}

DXCircle::DXCircle(D3DDevice* pDevice)
{
    DXShapeVertex vertices[9];
    for (int i = 0; i < 9; i++)
    {
        float fAngle = (float)(i * 2) * (D3DX_PI / 8.0f);
        vertices[i].x = sinf(fAngle);
        vertices[i].y = cosf(fAngle);
        vertices[i].z = 0.0f;
        vertices[i].color = 0xFFFF00FF;
    }
    if (FAILED(pDevice->CreateVertexBuffer(sizeof(vertices), 0, D3DFVF_XYZ | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT,
                                           &m_pVertexBuffer)))
    {
        return;
    }
    BYTE* pData;
    m_pVertexBuffer->Lock(0, 0, &pData, 0);
    memcpy(pData, vertices, sizeof(vertices));
}

void GLRender::RestoreVertexPrograms()
{
    for (int i = 0; i < g_listVertexPrograms.m_nCount; i++)
    {
        if (g_listVertexPrograms.m_pList[i])
        {
            g_pD3DDevice->CreateVertexShader(
                (DWORD*)g_listVertexPrograms.m_pList[i]->m_pErrors->GetBufferPointer(),
                (DWORD*)g_listVertexPrograms.m_pList[i]->m_pCompiledShader->GetBufferPointer(),
                &g_listVertexPrograms.m_pList[i]->m_hShader, 0);
        }
    }
}

void GLRender::Transform(const Vector& vPosition, const Quaternion& qOrientation, int nForce)
{
    static float m[16];

    if (multmatrix != 0 || nForce != 0)
    {
        ++counttrans;
        D3DXMATRIX mMatrix;
        if (multmatrix > 0)
        {
            MakeMatrix4x4(m, vPosition, qOrientation);
            mMatrix = D3DXMATRIX(m);
            switch (g_nMatrixMode)
            {
            case 3:
                g_pTextureMatrixStack->MultMatrixLocal(&mMatrix);
                break;
            case 2:
                g_pWorldMatrixStack->MultMatrixLocal(&mMatrix);
                break;
            case 1:
                g_pViewMatrixStack->MultMatrixLocal(&mMatrix);
                break;
            }
        }
        else
        {
            D3DXQUATERNION q(qOrientation.m_fX, qOrientation.m_fY, qOrientation.m_fZ, qOrientation.m_fW);
            D3DXMatrixRotationQuaternion(&mMatrix, &q);
            switch (g_nMatrixMode)
            {
            case 3:
                g_pTextureMatrixStack->TranslateLocal(vPosition.m_fX, vPosition.m_fY, vPosition.m_fZ);
                g_pTextureMatrixStack->MultMatrixLocal(&mMatrix);
                break;
            case 2:
                g_pWorldMatrixStack->TranslateLocal(vPosition.m_fX, vPosition.m_fY, vPosition.m_fZ);
                g_pWorldMatrixStack->MultMatrixLocal(&mMatrix);
                break;
            case 1:
                g_pViewMatrixStack->TranslateLocal(vPosition.m_fX, vPosition.m_fY, vPosition.m_fZ);
                g_pViewMatrixStack->MultMatrixLocal(&mMatrix);
                break;
            }
        }
    }
}

void GLRender::RenderObjectsVTCQuadUP(RenderVertexVTC* pVertices, int nVertices)
{
    if (nVertices == 0)
    {
        return;
    }
    int nMatrixMode = g_nMatrixMode;
    g_nMatrixMode = 1;
    g_pD3DDevice->SetTransform(D3DTS_VIEW, g_pViewMatrixStack->GetTop());
    g_nMatrixMode = 2;
    g_pD3DDevice->SetTransform(D3DTS_WORLD, g_pWorldMatrixStack->GetTop());
    g_nMatrixMode = nMatrixMode;
    g_pD3DDevice->SetVertexShader(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
    if (nVertices > 60000)
    {
        nVertices = 60000;
    }
    g_pD3DDevice->DrawPrimitiveUP(D3DPT_QUADLIST, nVertices / 4, pVertices, sizeof(RenderVertexVTC));
}

void GLRender::RenderObjectsVTQuadUP(RenderVertexVT* pVertices, int nVertices)
{
    if (nVertices == 0)
    {
        return;
    }
    int nMatrixMode = g_nMatrixMode;
    g_nMatrixMode = 1;
    g_pD3DDevice->SetTransform(D3DTS_VIEW, g_pViewMatrixStack->GetTop());
    g_nMatrixMode = 2;
    g_pD3DDevice->SetTransform(D3DTS_WORLD, g_pWorldMatrixStack->GetTop());
    g_nMatrixMode = nMatrixMode;
    g_pD3DDevice->SetTexture(cm_uiCurrentStage, g_pCurrentTexture);
    SetTextureStageFlag(cm_uiCurrentStage, 0);
    static unsigned long nFVF = TranslatePoolFormat(0x1010);
    g_pD3DDevice->SetVertexShader(nFVF);
    if (nVertices > 60000)
    {
        nVertices = 60000;
    }
    g_pD3DDevice->DrawPrimitiveUP(D3DPT_QUADLIST, nVertices / 4, pVertices, sizeof(RenderVertexVT));
}

void GLRender::CreateTextureImageRaw(int nTarget, int nLevel, int nComponents, int nWidth, int nHeight, int nBorder,
                          int nFormat, void* pImage)
{
    D3DTexture* pTexture;
    g_pD3DDevice->CreateTexture(nWidth, nHeight, 1, 0, (D3DFORMAT)textureFormatTranslationTable[nFormat],
                                D3DPOOL_DEFAULT, &pTexture);
    g_listTextures.m_pList[g_nCurrentTexture] = pTexture;
    g_pCurrentTexture = pTexture;
    cm_pCurrentTexture[cm_uiCurrentStage] = pTexture;
    g_pCurrentTextureState->m_nUnknown0 = 4;
    D3DSurface* pSurface;
    pTexture->GetSurfaceLevel(0, &pSurface);
    D3DLOCKED_RECT lock;
    pSurface->LockRect(&lock, NULL, 0);
    CopyTextureToTextureMemory(pImage, &lock, nWidth, nHeight, nFormat);
    pSurface->Release();
}

void GLRender::CreateTexSubImage(int nTarget, int nLevel, int nWidth, int nHeight, int nFormat, void* pImage)
{
    D3DTexture* pTexture = (D3DTexture*)g_pCurrentTexture;
    D3DSurface* pSurface;
    if (nLevel == 0)
    {
        pTexture->GetSurfaceLevel(0, &pSurface);
    }
    else
    {
        pTexture->GetSurfaceLevel(nLevel, &pSurface);
    }
    D3DLOCKED_RECT lock;
    pSurface->LockRect(&lock, NULL, 0);
    CopyTextureToTextureMemory(pImage, &lock, nWidth, nHeight, nFormat);
    pSurface->Release();
}

void GLRender::CreateCompressedTexImage(int nTarget, int bMipMaps, int nLevel, int nFormat, int nWidth, int nHeight,
                                    int nBorder, int nSize, void* pData)
{
    D3DLOCKED_RECT lock;
    if (nLevel == 0)
    {
        D3DTexture* pTexture;
        g_pD3DDevice->CreateTexture(nWidth, nHeight, bMipMaps == 0, nLevel,
                                    (D3DFORMAT)textureFormatTranslationTable[nFormat], D3DPOOL_DEFAULT, &pTexture);
        g_listTextures.m_pList[g_nCurrentTexture] = pTexture;
        g_pCurrentTexture = pTexture;
        cm_pCurrentTexture[cm_uiCurrentStage] = pTexture;
        D3DSurface* pSurface;
        pTexture->GetSurfaceLevel(0, &pSurface);
        pSurface->LockRect(&lock, NULL, 0);
        memcpy(lock.pBits, pData, nSize);
        pSurface->Release();
    }
    else
    {
        D3DSurface* pLevel;
        ((D3DTexture*)g_pCurrentTexture)->GetSurfaceLevel(nLevel, &pLevel);
        pLevel->LockRect(&lock, NULL, 0);
        memcpy(lock.pBits, pData, nSize);
        pLevel->Release();
    }
}

void GLRender::CreateMipMaps(int nTarget, int nComponents, int nWidth, int nHeight, int nFormat, void* pImage)
{
    D3DTexture* pTexture;
    g_pD3DDevice->CreateTexture(nWidth, nHeight, 0, 0, (D3DFORMAT)textureFormatTranslationTable[nFormat],
                                D3DPOOL_DEFAULT, &pTexture);
    cm_pCurrentTexture[cm_uiCurrentStage] = pTexture;
    g_pCurrentTexture = pTexture;
    g_pCurrentTextureState->m_nUnknown0 = nComponents;
    D3DBaseTexture* pOldTexture = g_listTextures.m_pList[g_nCurrentTexture];
    if (pOldTexture)
    {
        int nOldType = g_listTextureStates.m_pList[g_nCurrentTexture]->m_nType;
        if (nOldType == 1)
        {
            g_pD3DDevice->SetTexture(0, NULL);
            g_pD3DDevice->SetTexture(1, NULL);
            g_pD3DDevice->SetTexture(2, NULL);
            g_pD3DDevice->SetTexture(3, NULL);
            delete pOldTexture;
        }
        else if (nOldType == 0)
        {
            pOldTexture->Release();
        }
    }
    g_pCurrentTextureState->m_nType = 0;
    g_listTextures.m_pList[g_nCurrentTexture] = pTexture;
    pTexture->GetLevelCount();
    D3DSurface* pSurface;
    pTexture->GetSurfaceLevel(0, &pSurface);
    D3DLOCKED_RECT lock;
    pSurface->LockRect(&lock, NULL, 0);
    CopyTextureToTextureMemory(pImage, &lock, nWidth, nHeight, nFormat);
    pSurface->Release();
    D3DXFilterTexture(pTexture, NULL, 0, D3DX_FILTER_LINEAR);
}

int g_nMatrixMode = 2;
int g_nMatrixStackDepth;
LPD3DXMATRIXSTACK g_pProjectionMatrixStack;
LPD3DXMATRIXSTACK g_pViewMatrixStack;
LPD3DXMATRIXSTACK g_pWorldMatrixStack;
LPD3DXMATRIXSTACK g_pTextureMatrixStack;
List<DXVertexProgram*> g_listVertexPrograms;
int g_bVertexProgramBound;
int g_nCurrentTexture;
List<D3DBaseTexture*> g_listTextures;
List<DXTextureState*> g_listTextureStates;
DXTextureState* g_pCurrentTextureState;
D3DBaseTexture* g_pCurrentTexture;
int g_bMaterialDirty = 1;
D3DTexture* GLRender::cm_pConvolutionTextureLarge;
D3DTexture* GLRender::cm_pConvolutionTextureMedium;
D3DTexture* GLRender::cm_pConvolutionTextureSmall;
D3DTexture* GLRender::cm_pBlueTexture;
D3DTexture* GLRender::cm_pGreenTexture;
D3DTexture* GLRender::cm_pRedTexture;
int g_bConvolutionSetUp;
DWORD g_hConvolutionVertexShader;
float multiplier = 0.4f;

float g_afConvolutionWeights[25] =
{
    0.12f, 0.25f, 0.29f, 0.25f, 0.12f,
    0.25f, 0.49f, 0.58f, 0.49f, 0.25f,
    0.29f, 0.58f, 0.67f, 0.58f, 0.29f,
    0.25f, 0.49f, 0.58f, 0.49f, 0.25f,
    0.12f, 0.25f, 0.29f, 0.25f, 0.12f
};
D3DMATERIAL8 g_Material;
DWORD g_hGrassVertexShader;

char g_szPixelShaderTexture[] =
    "xps.1.1\n"
    "tex t0\n"
    "mov r0, t0\n"
    "xfc fog.a, r0.rgb, fog.rgb, zero, zero, zero, r0.a;\n";
char g_szPixelShaderFourTextureWeighted[] =
    "xps.1.1\n"
    "tex t0\n"
    "tex t1\n"
    "tex t2\n"
    "tex t3\n"
    "mul r0, t0, c0\n"
    "mad r0, t1, c1, r0\n"
    "mad r0, t2, c2, r0\n"
    "mad r0, t3, c3, r0\n"
    "mul r0, r0, v0\n";
char g_szPixelShaderTextureModulate[] =
    "xps.1.1 \n"
    "tex t0 \n"
    "mul r0, t0, v0 \n"
    "xfc fog.a, r0.rgb, fog.rgb, zero, zero, zero, r0.a;\n";
char g_szPixelShaderFourTextureAdd[] =
    "xps.1.1\n"
    "tex t0\n"
    "tex t1\n"
    "tex t2\n"
    "tex t3\n"
    "mov r0, t0\n"
    "add r0, r0, t1\n"
    "add r0, r0, t2\n"
    "add r0, r0, t3\n";
char g_szPixelShaderThreeTextureAdd[] =
    "xps.1.1\n"
    "tex t0\n"
    "tex t1\n"
    "tex t2\n"
    "mov r0, t0\n"
    "add r0, r0, t1\n"
    "add r0, r0, t2\n";
char g_szPixelShaderFog[] =
    "xps.1.1 \n"
    "mov r0.a, 1-zero;\n"
    "xfc 1-zero, fog.rgb, zero, zero, zero, zero,  1-fog.a;\n";
char g_szPixelShaderEnvironment[] =
    "xps.1.1 \n"
    "tex t0 \n"
    "tex t1 \n"
    "mul r0.rgb, t0, v0 \n"
    "+mov r0.a, 1-zero\n"
    "mov r1.rgb, t1 \n"
    "+mov r1.a, 1-zero\n"
    "mul r1, r1, 1-t0.a \n"
    "add r0, r0, r1 \n"
    "xfc fog.a, r0.rgb, fog.rgb, zero, zero, zero,  c0.a;\n";
char g_szPixelShaderEnvironmentLightMap[] =
    "xps.1.1 \n"
    "tex t0 \n"
    "tex t1 \n"
    "tex t2 \n"
    "mul r0.rgb, t0, v0 \n"
    "+mov r0.a, 1-zero\n"
    "mad r0, t0, t1, r0 \n"
    "mov r1.rgb, t2 \n"
    "+mov r1.a, 1-zero\n"
    "mul r1, r1, 1-t0.a \n"
    "add r0, r0, r1 \n"
    "xfc fog.a, r0.rgb, fog.rgb, zero, zero, zero,  c0.a;\n";
char g_szPixelShaderNormalMapLight[] =
    "xps.1.1\n"
    "tex t0\n"
    "tex t1\n"
    "tex t2\n"
    "tex t3\n"
    "dp3 r0, t0_bx2, t1_bx2\n"
    "mul t0, c0, v1\n"
    "mul t1, t2, t3\n"
    "mad r1, v0, t3, t1\n"
    "mad r0, t0, r0, r1\n"
    "xfc fog.a, r0.rgb, fog.rgb, zero, zero, zero, 1-zero;\n";
char g_szPixelShaderNormalMapModulate[] =
    "xps.1.1\n"
    "tex t0\n"
    "tex t1\n"
    "tex t2\n"
    "tex t3\n"
    "dp3 r0, t0_bx2, t1_bx2\n"
    "mul r1, c1, v1\n"
    "mad r0, r0, r1, v0\n"
    "add r0, r0, t2\n"
    "mul r0, r0, t3\n"
    "xfc fog.a, r0.rgb, fog.rgb, zero, zero, zero,1-zero;\n";
char g_szPixelShaderLightMap[] =
    "xps.1.1 \n"
    "tex t0 \n"
    "tex t1 \n"
    "add r0, t1, v0 \n"
    "mul r0, t0, r0 \n"
    "mul r0.a, t0.a, v0.a \n"
    "xfc fog.a, r0.rgb, fog.rgb, zero, zero, zero, r0.a;\n";
char g_szPixelShaderLightMapOnly[] =
    "xps.1.1 \n"
    "tex t1 \n"
    "add r0, t1, v0 \n"
    "xfc fog.a, r0.rgb, fog.rgb, zero, zero, zero, r0.a;\n";
char g_szPixelShaderDepthSprite[] =
    "xps.1.1 \n"
    "tex t0 \n"
    "tex t1 \n"
    "texm3x2pad t2, t1_hl \n"
    "texm3x2depth t3, t1_hl \n"
    "mul r0, t0, v0 \n";
char g_szPixelShaderBumpSelfShadow[] =
    "xps.1.1 \n"
    "tex t0 \n"
    "tex t1 \n"
    "dp3 r0, c4, t1 \n"
    "mov r1, t1 \n"
    "dp3 r0.rgb, r1_bx2, t0_bx2 \n"
    "mov r1, r0_sat \n"
    "mul r1, r0, r1 \n"
    "cnd r0, r0.a, r1, c7 \n"
    "mul r0, r0, c2 \n"
    "xfc fog.a, r0.rgb, fog.rgb, zero, zero, zero, r0.a;\n";
char g_szPixelShaderBumpDiffuse[] =
    "xps.1.1 \n"
    "tex t0 \n"
    "tex t1 \n"
    "dp3 r0, c4, t1 \n"
    "mov_x4 r0.rgba, r0_bx2 \n"
    "mov r0, r0_sat \n"
    "mov r1, t1 \n"
    "dp3 r0.rgb, r1_bx2, t0_bx2 \n"
    "mul r0, r0_sat, c2 \n"
    "xfc fog.a, r0.rgb, fog.rgb, zero, zero, zero, r0.a;\n";
char g_szPixelShaderBumpEnvironment[] =
    "xps.1.1 \n"
    "tex t0 \n"
    "texm3x3pad t1, t0_bx2 \n"
    "texm3x3pad t2, t0_bx2\n"
    "texm3x3vspec t3, t0_bx2\n"
    "mov r0, t3 \n"
    "xfc fog.a, r0.rgb, fog.rgb, zero, zero, zero, c0.a;\n";
char g_szPixelShaderBumpLightMap[] =
    "xps.1.1\n"
    "tex t0\n"
    "tex t1\n"
    "tex t2\n"
    "tex t3\n"
    "dp3 r1, t2_bx2, t3_bx2\n"
    "mul r1.rgb, r1_sat, t1.rgb\n"
    "mul r0, r1, c1\n"
    "mul r0, r0, c0\n"
    "mul r0, r0, t0\n"
    "mov r0.a, zero\n"
    "xfc fog.a, r0.rgb, fog.rgb, zero, zero, zero, zero;\n";
char g_szPixelShaderBumpSpecular[] =
    "xps.1.1\n"
    "tex t0\n"
    "tex t1\n"
    "tex t2\n"
    "tex t3\n"
    "xdd r0, r1, t0_bx2, t1_bx2, t0_bx2, t2_bx2\n"
    "xdm r1, r0, r1.b, r1.b, r0, c1\n"
    "xdd r0.rgb, r1.rgb, r0, v1, v1, c0\n"
    "add r0, v0, r0\n"
    "dp3 r0.rgb, t3, r0\n"
    "dp3 r1.rgb, r1.a, r1\n"
    "add r0, r0, r1\n"
    "xfc fog.a, r0.rgb, fog.rgb, zero, zero, zero, r0.a;\n";
char g_szPixelShaderBumpSpecularPower[] =
    "xps.1.1\n"
    "tex t0\n"
    "tex t1\n"
    "tex t2\n"
    "tex t3\n"
    "xdd r0, r1, t0_bx2, t1_bx2, t0_bx2, t2_bx2\n"
    "mul r1, r1, r1\n"
    "mul r0.rgb, r0.rgb, c1.rgb\n"
    "+mul r1.a, r1.b, r1.b\n"
    "xmma r0.rgb, r1.rgb, discard.rgb, r0, v1, v1, c0\n"
    "+mul r1.a, r1.a, r1.a\n"
    "add r0.rgb, v0.rgb, r0.rgb\n"
    "+mul r1.a, r1.a, r1.a\n"
    "xmma r0.rgb, r1.rgb, discard.rgb, t3.rgb, r0.rgb, r1.a, r1.rgb\n"
    "mul r1.rgb, 1-t3.a, r1.rgb\n"
    "add r0.rgb, r0.rgb, r1.rgb\n"
    "xfc fog.a, r0.rgb, fog.rgb, zero, zero, zero, v0.a;\n";
char g_szPixelShaderColorLookup[] =
    "xps.1.1\n"
    "def c0, 1.f, 0., 0., 0.f\n"
    "def c1, 0.f, 1.f, 0.f, 0.f\n"
    "def c2, 0.f, 0.f, 1.f, 0.f\n"
    "tex t0\n"
    "texreg2ar t1, t0\n"
    "texreg2gb t2, t0\n"
    "texreg2gb t3, t0\n"
    "mul r0, t1, c0\n"
    "mad r0, t2, c1, r0\n"
    "mad r0, t3, c2, r0\n"
    "mov r0.a, v0.a\n";
char g_szPixelShaderAverage[] =
    "xps.1.1\n"
    "def c3, 0.25, 0.25, 0.25, 0.25\n"
    "tex t0\n"
    "tex t1\n"
    "tex t2\n"
    "tex t3\n"
    "mul r0, c3, t0\n"
    "mad r0, c3, t1, r0\n"
    "mad r0, c3, t2, r0\n"
    "mad r0, c3, t3, r0\n";
char g_szPixelShaderAdd[] =
    "xps.1.1\n"
    "tex t0\n"
    "tex t1\n"
    "add r0, t0, t1\n";
char g_szPixelShaderDependentLookup[] =
    "xps.1.1\n"
    "tex t0\n"
    "texreg2gb t1, t0\n"
    "mov r0, t1\n"
    "mov r0.a, t0.a\n";
char g_szPixelShaderDistortion[] =
    "xps.1.1\n"
    "tex t0\n"
    "texm3x2pad t1, t0_bx2\n"
    "texm3x2tex t2, t0_bx2\n"
    "add r0, t2, c0\n";
char g_szPixelShaderColorMatrix[] =
    "xps.1.1\n"
    "def c0, 0.f, 0.f, 0.f, 1.f\n"
    "def c4, 1.f, 0., 0., 0.f\n"
    "def c5, 0.f, 1.f, 0.f, 0.f\n"
    "def c6, 0.f, 0.f, 1.f, 0.f\n"
    "tex t0\n"
    "mov r0, c0\n"
    "dp3 r1, t0, c1\n"
    "mad r0, r1, c4, r0\n"
    "dp3 r1, t0, c2\n"
    "mad r0, r1, c5, r0\n"
    "dp3 r1, t0, c3\n"
    "mad r0, r1, c6, r0\n";
char g_szPixelShaderBlend[] =
    "xps.1.1\n"
    "tex t0\n"
    "tex t1\n"
    "mul r0, t0, c0\n"
    "mad r0, t1, c1, r0\n"
    "mov r0.a, c0.a\n";
char g_szPixelShaderColorLookupAlpha[] =
    "xps.1.1\n"
    "def c0, 1.f, 0., 0., 0.f\n"
    "def c1, 0.f, 1.f, 0.f, 0.f\n"
    "def c2, 0.f, 0.f, 1.f, 0.f\n"
    "tex t0\n"
    "texreg2ar t1, t0\n"
    "texreg2gb t2, t0\n"
    "texreg2gb t3, t0\n"
    "mul r0, t1, c0\n"
    "mad r0, t2, c1, r0\n"
    "mad r0, t3, c2, r0\n"
    "mov r0.a, v0.a\n";
char g_szPixelShaderWhite[] =
    "xps.1.1\n"
    "mov r0, 1-zero\n"
    "xfc 1-zero, r0.rgb, fog.rgb, zero, zero, zero, r0.a;\n";

void GLRender::CreateCubeTexture(CAurTexture* pTexture, bool bMipMaps,
                                 int nIndex)
{
    static int s1[6] = {1, 0, 2, 3, 4, 5};

    int nHeight = pTexture->GetHeight();
    unsigned char* pImage = pTexture->GetImage();
    int nWidth = pTexture->GetWidth();
    int nBPP = pTexture->GetBPP();
    pTexture->GetNumX();
    pTexture->GetNumY();
    int bMipMap = pTexture->GetMipMap();
    pTexture->GetClamp();
    int bFilter = pTexture->GetFilter();

    D3DCubeTexture* pCubeTexture;
    g_pD3DDevice->CreateCubeTexture(nWidth, bMipMap ? 0 : 1, 0, D3DFMT_A8R8G8B8,
                                    0, &pCubeTexture);
    D3DBaseTexture* pOldTexture = g_listTextures.m_pList[g_nCurrentTexture];
    int nOldType = g_listTextureStates.m_pList[g_nCurrentTexture]->m_nType;
    g_listTextures.m_pList[g_nCurrentTexture] = pCubeTexture;
    if (g_listTextureStates.m_pList[g_nCurrentTexture] == NULL)
    {
        g_listTextureStates.m_pList[g_nCurrentTexture] = new DXTextureState;
    }
    g_pCurrentTextureState = g_listTextureStates.m_pList[g_nCurrentTexture];
    g_listTextureStates.m_pList[g_nCurrentTexture]->m_nWrapR = 1;
    g_listTextureStates.m_pList[g_nCurrentTexture]->m_nWrapS = 1;
    g_listTextureStates.m_pList[g_nCurrentTexture]->m_nWrapT = 1;
    int nFilterMode = bFilter ? 5 : 4;
    g_listTextureStates.m_pList[g_nCurrentTexture]->m_nMagFilter = nFilterMode;
    g_listTextureStates.m_pList[g_nCurrentTexture]->m_nMinFilter = nFilterMode;
    if (bMipMap)
    {
        nFilterMode = bFilter ? 6 : 7;
    }
    g_listTextureStates.m_pList[g_nCurrentTexture]->m_nMipFilter = nFilterMode;
    if (pOldTexture)
    {
        if (nOldType == 1)
        {
            g_pD3DDevice->SetTexture(0, NULL);
            g_pD3DDevice->SetTexture(1, NULL);
            g_pD3DDevice->SetTexture(2, NULL);
            g_pD3DDevice->SetTexture(3, NULL);
            delete pOldTexture;
        }
        else if (nOldType == 0)
        {
            pOldTexture->Release();
        }
    }
    g_listTextureStates.m_pList[g_nCurrentTexture]->m_nType = 0;
    g_pCurrentTexture = pCubeTexture;
    cm_pCurrentTexture[cm_uiCurrentStage] = pCubeTexture;

    int nFaceHeight = nHeight / 6;
    unsigned char* pPixels = new unsigned char[nWidth * nWidth * 4];
    for (unsigned int i = 0; i < 6; i++)
    {
        D3DSurface* pSurface;
        pCubeTexture->GetCubeMapSurface((D3DCUBEMAP_FACES)s1[i], 0, &pSurface);
        D3DLOCKED_RECT lock;
        pSurface->LockRect(&lock, NULL, 0);
        unsigned char* pFace = pImage + i * nFaceHeight * nBPP * nWidth;
        int x;
        int y;
        if (!pTexture->m_bEnvironmentMapped)
        {
            if (i == 5)
            {
                for (y = 0; y < nWidth; y++)
                {
                    for (x = 0; x < nWidth; x++)
                    {
                        pPixels[lock.Pitch * y + x * 4 + 0] = pFace[(y * nWidth + x) * nBPP + 0];
                        pPixels[lock.Pitch * y + x * 4 + 1] = pFace[(y * nWidth + x) * nBPP + 1];
                        pPixels[lock.Pitch * y + x * 4 + 2] = pFace[(y * nWidth + x) * nBPP + 2];
                        pPixels[lock.Pitch * y + x * 4 + 3] = 255;
                    }
                }
            }
            else
            {
                for (y = 0; y < nWidth; y++)
                {
                    for (x = 0; x < nWidth; x++)
                    {
                        pPixels[lock.Pitch * y + x * 4 + 0] = pFace[(x * nWidth + nWidth - 1 - y) * nBPP + 0];
                        pPixels[lock.Pitch * y + x * 4 + 1] = pFace[(x * nWidth + nWidth - 1 - y) * nBPP + 1];
                        pPixels[lock.Pitch * y + x * 4 + 2] = pFace[(x * nWidth + nWidth - 1 - y) * nBPP + 2];
                        pPixels[lock.Pitch * y + x * 4 + 3] = 255;
                    }
                }
            }
        }
        else if (i == 0)
        {
            for (y = 0; y < nWidth; y++)
            {
                for (x = 0; x < nWidth; x++)
                {
                    pPixels[lock.Pitch * y + x * 4 + 0] = pFace[(x * nWidth + nWidth - 1 - y) * nBPP + 0];
                    pPixels[lock.Pitch * y + x * 4 + 1] = pFace[(x * nWidth + nWidth - 1 - y) * nBPP + 1];
                    pPixels[lock.Pitch * y + x * 4 + 2] = pFace[(x * nWidth + nWidth - 1 - y) * nBPP + 2];
                    pPixels[lock.Pitch * y + x * 4 + 3] = 255;
                }
            }
        }
        else if (i == 1)
        {
            for (y = 0; y < nWidth; y++)
            {
                for (x = 0; x < nWidth; x++)
                {
                    pPixels[lock.Pitch * y + x * 4 + 0] = pFace[((nWidth - 1 - x) * nWidth + y) * nBPP + 0];
                    pPixels[lock.Pitch * y + x * 4 + 1] = pFace[((nWidth - 1 - x) * nWidth + y) * nBPP + 1];
                    pPixels[lock.Pitch * y + x * 4 + 2] = pFace[((nWidth - 1 - x) * nWidth + y) * nBPP + 2];
                    pPixels[lock.Pitch * y + x * 4 + 3] = 255;
                }
            }
        }
        else if (i == 2)
        {
            for (y = 0; y < nWidth; y++)
            {
                for (x = 0; x < nWidth; x++)
                {
                    pPixels[lock.Pitch * y + x * 4 + 0] = pFace[(y * nWidth + x) * nBPP + 0];
                    pPixels[lock.Pitch * y + x * 4 + 1] = pFace[(y * nWidth + x) * nBPP + 1];
                    pPixels[lock.Pitch * y + x * 4 + 2] = pFace[(y * nWidth + x) * nBPP + 2];
                    pPixels[lock.Pitch * y + x * 4 + 3] = 255;
                }
            }
        }
        else if (i == 3)
        {
            for (y = 0; y < nWidth; y++)
            {
                for (x = 0; x < nWidth; x++)
                {
                    pPixels[lock.Pitch * y + x * 4 + 0] = pFace[(nWidth * nWidth - 1 - y * nWidth - x) * nBPP + 0];
                    pPixels[lock.Pitch * y + x * 4 + 1] = pFace[(nWidth * nWidth - 1 - y * nWidth - x) * nBPP + 1];
                    pPixels[lock.Pitch * y + x * 4 + 2] = pFace[(nWidth * nWidth - 1 - y * nWidth - x) * nBPP + 2];
                    pPixels[lock.Pitch * y + x * 4 + 3] = 255;
                }
            }
        }
        else if (i == 4)
        {
            for (y = 0; y < nWidth; y++)
            {
                for (x = 0; x < nWidth; x++)
                {
                    pPixels[lock.Pitch * y + x * 4 + 0] = pFace[(nWidth * nWidth - 1 - y * nWidth - x) * nBPP + 0];
                    pPixels[lock.Pitch * y + x * 4 + 1] = pFace[(nWidth * nWidth - 1 - y * nWidth - x) * nBPP + 1];
                    pPixels[lock.Pitch * y + x * 4 + 2] = pFace[(nWidth * nWidth - 1 - y * nWidth - x) * nBPP + 2];
                    pPixels[lock.Pitch * y + x * 4 + 3] = 255;
                }
            }
        }
        else
        {
            for (y = 0; y < nWidth; y++)
            {
                for (x = 0; x < nWidth; x++)
                {
                    pPixels[lock.Pitch * y + x * 4 + 0] = pFace[((nWidth - 1 - y) * nWidth + x) * nBPP + 0];
                    pPixels[lock.Pitch * y + x * 4 + 1] = pFace[((nWidth - 1 - y) * nWidth + x) * nBPP + 1];
                    pPixels[lock.Pitch * y + x * 4 + 2] = pFace[((nWidth - 1 - y) * nWidth + x) * nBPP + 2];
                    pPixels[lock.Pitch * y + x * 4 + 3] = 255;
                }
            }
        }
        CopyTextureToTextureMemory(pPixels, &lock, nWidth, nWidth, Aurora::TEXTURE_FORMAT_RGBA);
        pSurface->Release();
    }
    delete[] pPixels;
    if (bMipMap)
    {
        D3DXFilterCubeTexture(pCubeTexture, NULL, 0, D3DX_FILTER_LINEAR);
    }
}

void GLRender::CreateProcessedCubeTexture(CAurTexture* pTexture, bool bSubImage,
                                          int nIndex)
{
    pTexture->GetHeight();
    unsigned char* pImage = pTexture->GetImage();
    pTexture->GetWidth();
    int nBPP = pTexture->GetBPP();
    pTexture->GetNumX();
    pTexture->GetNumY();
    int bMipMap = pTexture->GetMipMap();
    pTexture->GetClamp();
    int bFilter = pTexture->GetFilter();

    int nFormat;
    if (pTexture->GetCompressedSize() > 0)
    {
        nFormat = pTexture->GetCompressionFormat();
    }
    else if (nBPP == 4)
    {
        nFormat = Aurora::TEXTURE_FORMAT_RGBA;
    }
    else if (nBPP == 3)
    {
        nFormat = Aurora::TEXTURE_FORMAT_RGB;
    }
    else if (nBPP == 2)
    {
        nFormat = Aurora::TEXTURE_FORMAT_LUMINANCE_ALPHA;
    }
    else
    {
        nFormat = Aurora::TEXTURE_FORMAT_LUMINANCE;
    }

    D3DBaseTexture* pOldTexture = g_listTextures.m_pList[g_nCurrentTexture];
    int nOldType = g_listTextureStates.m_pList[g_nCurrentTexture]->m_nType;
    D3DCubeTexture* pCubeTexture = new D3DCubeTexture;
    XGSetCubeTextureHeader(pTexture->GetWidth(), pTexture->GetNumMipMaps(), 0,
                           (D3DFORMAT)textureFormatTranslationTable[nFormat],
                           0, pCubeTexture, 0, 0);
    pCubeTexture->Register(pImage);
    g_listTextures.m_pList[g_nCurrentTexture] = pCubeTexture;
    if (g_listTextureStates.m_pList[g_nCurrentTexture] == NULL)
    {
        g_listTextureStates.m_pList[g_nCurrentTexture] = new DXTextureState;
    }
    g_pCurrentTextureState = g_listTextureStates.m_pList[g_nCurrentTexture];
    g_listTextureStates.m_pList[g_nCurrentTexture]->m_nWrapR = 1;
    g_listTextureStates.m_pList[g_nCurrentTexture]->m_nWrapS = 1;
    g_listTextureStates.m_pList[g_nCurrentTexture]->m_nWrapT = 1;
    int nFilterMode = bFilter ? 5 : 4;
    g_listTextureStates.m_pList[g_nCurrentTexture]->m_nMagFilter = nFilterMode;
    g_listTextureStates.m_pList[g_nCurrentTexture]->m_nMinFilter = nFilterMode;
    if (bMipMap)
    {
        nFilterMode = bFilter ? 6 : 7;
    }
    g_listTextureStates.m_pList[g_nCurrentTexture]->m_nMipFilter = nFilterMode;
    if (pOldTexture)
    {
        if (nOldType == 1)
        {
            g_pD3DDevice->SetTexture(0, NULL);
            g_pD3DDevice->SetTexture(1, NULL);
            g_pD3DDevice->SetTexture(2, NULL);
            g_pD3DDevice->SetTexture(3, NULL);
            delete pOldTexture;
        }
        else if (nOldType == 0)
        {
            pOldTexture->Release();
        }
    }
    g_listTextureStates.m_pList[g_nCurrentTexture]->m_nType = 1;
    g_pCurrentTexture = pCubeTexture;
    cm_pCurrentTexture[cm_uiCurrentStage] = pCubeTexture;
}

void GLRender::ReadPixelsFromBuffer(int nX, int nY, int nWidth, int nHeight, int nFormat,
                          unsigned char* pPixels)
{
    D3DSurface* pSurface;
    g_pD3DDevice->GetRenderTarget(&pSurface);
    if (nFormat == Aurora::TEXTURE_FORMAT_RGB)
    {
        D3DSURFACE_DESC desc;
        pSurface->GetDesc(&desc);
        D3DLOCKED_RECT lock;
        pSurface->LockRect(&lock, NULL, D3DLOCK_READONLY | D3DLOCK_TILED);
        for (unsigned int y = 0; y < desc.Height; y++)
        {
            for (unsigned int x = 0; x < desc.Width; x++)
            {
                pPixels[(desc.Width * y + x) * 3 + 0] = ((unsigned char*)lock.pBits)[lock.Pitch * y + x * 4 + 2];
                pPixels[(desc.Width * y + x) * 3 + 1] = ((unsigned char*)lock.pBits)[lock.Pitch * y + x * 4 + 1];
                pPixels[(desc.Width * y + x) * 3 + 2] = ((unsigned char*)lock.pBits)[lock.Pitch * y + x * 4 + 0];
            }
        }
        pSurface->Release();
        return;
    }
    else if (nFormat == Aurora::TEXTURE_FORMAT_RGBA)
    {
        D3DSURFACE_DESC desc;
        pSurface->GetDesc(&desc);
        D3DLOCKED_RECT lock;
        pSurface->LockRect(&lock, NULL, D3DLOCK_READONLY | D3DLOCK_TILED);
        for (unsigned int y = 0; y < desc.Height; y++)
        {
            for (unsigned int x = 0; x < desc.Width; x++)
            {
                pPixels[(desc.Width * y + x) * 4 + 0] = ((unsigned char*)lock.pBits)[lock.Pitch * y + x * 4 + 2];
                pPixels[(desc.Width * y + x) * 4 + 1] = ((unsigned char*)lock.pBits)[lock.Pitch * y + x * 4 + 1];
                pPixels[(desc.Width * y + x) * 4 + 2] = ((unsigned char*)lock.pBits)[lock.Pitch * y + x * 4 + 0];
                pPixels[(desc.Width * y + x) * 4 + 3] = ((unsigned char*)lock.pBits)[lock.Pitch * y + x * 4 + 4];
            }
        }
    }
    pSurface->Release();
}

void GLRender::DrawLightmappedGrass(void* pVertices, int nVertexStride,
                                    void* pIndices, int nStride, int nFirst,
                                    int nQuads)
{
    int nMatrixMode = g_nMatrixMode;
    g_nMatrixMode = 1;
    g_pD3DDevice->SetTransform(D3DTS_VIEW, g_pViewMatrixStack->GetTop());
    g_nMatrixMode = 2;
    g_pD3DDevice->SetTransform(D3DTS_WORLD, g_pWorldMatrixStack->GetTop());
    g_nMatrixMode = nMatrixMode;
    if (g_bMaterialDirty)
    {
        g_pD3DDevice->SetMaterial(&g_Material);
        g_bMaterialDirty = 0;
    }
    g_pD3DDevice->SetStreamSource(0, (D3DVertexBuffer*)pVertices, nVertexStride);
    g_pD3DDevice->SetStreamSource(1, (D3DVertexBuffer*)pIndices, nStride);
    g_pD3DDevice->SetVertexShader(g_hGrassVertexShader);
    g_pD3DDevice->DrawVertices(D3DPT_QUADLIST, nFirst, nQuads * 4);
    g_pD3DDevice->SetStreamSource(0, NULL, 0);
    g_pD3DDevice->SetStreamSource(1, NULL, 0);
}

void GLRender::DrawElementsInShadow(unsigned int nProgram,
                                    unsigned long nLength,
                                    const char* sProgram,
                                    unsigned long* pnProgram)
{
    DXVertexProgram* pProgram = g_listVertexPrograms.m_pList[nProgram];
    if (pProgram->m_pCompiledShader)
    {
        pProgram->m_pCompiledShader->Release();
        pProgram->m_pConstants->Release();
        pProgram->m_pConstants = NULL;
        pProgram->m_pCompiledShader = NULL;
    }
    XGAssembleShader(NULL, sProgram, nLength, SASM_SKIPVALIDATION,
                     &pProgram->m_pConstants, &pProgram->m_pCompiledShader,
                     NULL, NULL, NULL, NULL, NULL);
    if (pProgram->m_hShader)
    {
        g_pD3DDevice->DeleteVertexShader(pProgram->m_hShader);
    }
    g_pD3DDevice->CreateVertexShader(
        pnProgram, (DWORD*)pProgram->m_pCompiledShader->GetBufferPointer(),
        &pProgram->m_hShader, 0);
    g_pD3DDevice->SetVertexShader(pProgram->m_hShader);
    g_bVertexProgramBound = 1;
}

bool GLRender::IsProgramNV(int nProgram)
{
    if (nProgram < g_listVertexPrograms.m_nCount &&
        g_listVertexPrograms.m_pList[nProgram])
    {
        return true;
    }
    return false;
}

void GLRender::BindVertexProgram(int nProgram)
{
    if (nProgram == 0)
    {
        g_bVertexProgramBound = 0;
        return;
    }
    g_bVertexProgramBound = 1;
    g_pD3DDevice->SetVertexShader(g_listVertexPrograms.m_pList[nProgram]->m_hShader);
}

void GLRender::TrackMatrix()
{
    D3DXMATRIX mWorldView;
    D3DXMATRIX mWorldViewProjection;
    D3DXMatrixMultiply(&mWorldView, g_pWorldMatrixStack->GetTop(),
                       g_pViewMatrixStack->GetTop());
    D3DXMatrixMultiply(&mWorldViewProjection, &mWorldView,
                       g_pProjectionMatrixStack->GetTop());
    D3DXMatrixTranspose(&mWorldViewProjection, &mWorldViewProjection);
    g_pD3DDevice->SetVertexShaderConstant(0, &mWorldViewProjection, 4);
    D3DXMatrixTranspose(&mWorldView, &mWorldView);
    g_pD3DDevice->SetVertexShaderConstant(8, &mWorldView, 4);
}

void GLRender::InitExtensions()
{
    InitAttenuationTexture();
}

int GLRender::InitializePixelShaders()
{
    LPXGBUFFER pCompiledShader;
    LPXGBUFFER pErrors;

    XGAssembleShader(NULL, g_szPixelShaderTexture, strlen(g_szPixelShaderTexture),
                     SASM_DEBUG, NULL, &pCompiledShader, &pErrors, NULL, NULL,
                     NULL, NULL);
    g_pD3DDevice->CreatePixelShader(
        (D3DPIXELSHADERDEF*)pCompiledShader->GetBufferPointer(),
        &g_ahPixelShader[PIXEL_SHADER_TEXTURE]);
    pCompiledShader->Release();
    pErrors->Release();

    XGAssembleShader(NULL, g_szPixelShaderFourTextureWeighted, strlen(g_szPixelShaderFourTextureWeighted),
                     SASM_DEBUG, NULL, &pCompiledShader, &pErrors, NULL, NULL,
                     NULL, NULL);
    g_pD3DDevice->CreatePixelShader(
        (D3DPIXELSHADERDEF*)pCompiledShader->GetBufferPointer(),
        &g_ahPixelShader[PIXEL_SHADER_FOUR_TEXTURE_WEIGHTED]);
    pCompiledShader->Release();
    pErrors->Release();

    XGAssembleShader(NULL, g_szPixelShaderFourTextureAdd, strlen(g_szPixelShaderFourTextureAdd),
                     SASM_DEBUG, NULL, &pCompiledShader, &pErrors, NULL, NULL,
                     NULL, NULL);
    g_pD3DDevice->CreatePixelShader(
        (D3DPIXELSHADERDEF*)pCompiledShader->GetBufferPointer(),
        &g_ahPixelShader[PIXEL_SHADER_FOUR_TEXTURE_ADD]);
    pCompiledShader->Release();
    pErrors->Release();

    XGAssembleShader(NULL, g_szPixelShaderThreeTextureAdd, strlen(g_szPixelShaderThreeTextureAdd),
                     SASM_DEBUG, NULL, &pCompiledShader, &pErrors, NULL, NULL,
                     NULL, NULL);
    g_pD3DDevice->CreatePixelShader(
        (D3DPIXELSHADERDEF*)pCompiledShader->GetBufferPointer(),
        &g_ahPixelShader[PIXEL_SHADER_THREE_TEXTURE_ADD]);
    pCompiledShader->Release();
    pErrors->Release();

    if (FAILED(XGAssembleShader(NULL, g_szPixelShaderFog, strlen(g_szPixelShaderFog),
                         SASM_DEBUG, NULL, &pCompiledShader, &pErrors, NULL, NULL,
                         NULL, NULL)))
    {
        pErrors->GetBufferPointer();
    }
    g_pD3DDevice->CreatePixelShader(
        (D3DPIXELSHADERDEF*)pCompiledShader->GetBufferPointer(),
        &g_ahPixelShader[PIXEL_SHADER_FOG]);
    pCompiledShader->Release();
    if (pErrors)
    {
        pErrors->Release();
    }

    if (FAILED(XGAssembleShader(NULL, g_szPixelShaderEnvironment, strlen(g_szPixelShaderEnvironment),
                         SASM_DEBUG, NULL, &pCompiledShader, &pErrors, NULL, NULL,
                         NULL, NULL)))
    {
        pErrors->GetBufferPointer();
    }
    g_pD3DDevice->CreatePixelShader(
        (D3DPIXELSHADERDEF*)pCompiledShader->GetBufferPointer(),
        &g_ahPixelShader[PIXEL_SHADER_ENVIRONMENT]);
    pCompiledShader->Release();
    if (pErrors)
    {
        pErrors->Release();
    }

    if (FAILED(XGAssembleShader(NULL, g_szPixelShaderEnvironmentLightMap, strlen(g_szPixelShaderEnvironmentLightMap),
                         SASM_DEBUG, NULL, &pCompiledShader, &pErrors, NULL, NULL,
                         NULL, NULL)))
    {
        pErrors->GetBufferPointer();
    }
    g_pD3DDevice->CreatePixelShader(
        (D3DPIXELSHADERDEF*)pCompiledShader->GetBufferPointer(),
        &g_ahPixelShader[PIXEL_SHADER_ENVIRONMENT_LIGHT_MAP]);
    pCompiledShader->Release();
    if (pErrors)
    {
        pErrors->Release();
    }

    if (FAILED(XGAssembleShader(NULL, g_szPixelShaderLightMap, strlen(g_szPixelShaderLightMap),
                         SASM_DEBUG, NULL, &pCompiledShader, &pErrors, NULL, NULL,
                         NULL, NULL)))
    {
        pErrors->GetBufferPointer();
    }
    g_pD3DDevice->CreatePixelShader(
        (D3DPIXELSHADERDEF*)pCompiledShader->GetBufferPointer(),
        &g_ahPixelShader[PIXEL_SHADER_LIGHT_MAP]);
    pCompiledShader->Release();
    if (pErrors)
    {
        pErrors->Release();
    }

    if (FAILED(XGAssembleShader(NULL, g_szPixelShaderLightMapOnly, strlen(g_szPixelShaderLightMapOnly),
                         SASM_DEBUG, NULL, &pCompiledShader, &pErrors, NULL, NULL,
                         NULL, NULL)))
    {
        pErrors->GetBufferPointer();
    }
    g_pD3DDevice->CreatePixelShader(
        (D3DPIXELSHADERDEF*)pCompiledShader->GetBufferPointer(),
        &g_ahPixelShader[PIXEL_SHADER_LIGHT_MAP_ONLY]);
    pCompiledShader->Release();
    if (pErrors)
    {
        pErrors->Release();
    }

    if (FAILED(XGAssembleShader(NULL, g_szPixelShaderBumpSelfShadow, strlen(g_szPixelShaderBumpSelfShadow),
                         SASM_DEBUG, NULL, &pCompiledShader, &pErrors, NULL, NULL,
                         NULL, NULL)))
    {
        pErrors->GetBufferPointer();
    }
    g_pD3DDevice->CreatePixelShader(
        (D3DPIXELSHADERDEF*)pCompiledShader->GetBufferPointer(),
        &g_ahPixelShader[PIXEL_SHADER_BUMP_SELF_SHADOW]);
    pCompiledShader->Release();
    if (pErrors)
    {
        pErrors->Release();
    }

    if (FAILED(XGAssembleShader(NULL, g_szPixelShaderBumpDiffuse, strlen(g_szPixelShaderBumpDiffuse),
                         SASM_DEBUG, NULL, &pCompiledShader, &pErrors, NULL, NULL,
                         NULL, NULL)))
    {
        pErrors->GetBufferPointer();
    }
    g_pD3DDevice->CreatePixelShader(
        (D3DPIXELSHADERDEF*)pCompiledShader->GetBufferPointer(),
        &g_ahPixelShader[PIXEL_SHADER_BUMP_DIFFUSE]);
    pCompiledShader->Release();
    if (pErrors)
    {
        pErrors->Release();
    }

    if (FAILED(XGAssembleShader(NULL, g_szPixelShaderBumpEnvironment, strlen(g_szPixelShaderBumpEnvironment),
                         SASM_DEBUG, NULL, &pCompiledShader, &pErrors, NULL, NULL,
                         NULL, NULL)))
    {
        pErrors->GetBufferPointer();
    }
    g_pD3DDevice->CreatePixelShader(
        (D3DPIXELSHADERDEF*)pCompiledShader->GetBufferPointer(),
        &g_ahPixelShader[PIXEL_SHADER_BUMP_ENVIRONMENT]);
    pCompiledShader->Release();
    if (pErrors)
    {
        pErrors->Release();
    }

    if (FAILED(XGAssembleShader(NULL, g_szPixelShaderBumpLightMap, strlen(g_szPixelShaderBumpLightMap),
                         SASM_DEBUG, NULL, &pCompiledShader, &pErrors, NULL, NULL,
                         NULL, NULL)))
    {
        pErrors->GetBufferPointer();
    }
    g_pD3DDevice->CreatePixelShader(
        (D3DPIXELSHADERDEF*)pCompiledShader->GetBufferPointer(),
        &g_ahPixelShader[PIXEL_SHADER_BUMP_LIGHT_MAP]);
    pCompiledShader->Release();
    if (pErrors)
    {
        pErrors->Release();
    }

    if (FAILED(XGAssembleShader(NULL, g_szPixelShaderBumpSpecular, strlen(g_szPixelShaderBumpSpecular),
                         SASM_DEBUG, NULL, &pCompiledShader, &pErrors, NULL, NULL,
                         NULL, NULL)))
    {
        pErrors->GetBufferPointer();
    }
    g_pD3DDevice->CreatePixelShader(
        (D3DPIXELSHADERDEF*)pCompiledShader->GetBufferPointer(),
        &g_ahPixelShader[PIXEL_SHADER_BUMP_SPECULAR]);
    pCompiledShader->Release();
    if (pErrors)
    {
        pErrors->Release();
    }

    if (FAILED(XGAssembleShader(NULL, g_szPixelShaderBumpSpecularPower, strlen(g_szPixelShaderBumpSpecularPower),
                         SASM_DEBUG, NULL, &pCompiledShader, &pErrors, NULL, NULL,
                         NULL, NULL)))
    {
        pErrors->GetBufferPointer();
    }
    g_pD3DDevice->CreatePixelShader(
        (D3DPIXELSHADERDEF*)pCompiledShader->GetBufferPointer(),
        &g_ahPixelShader[PIXEL_SHADER_BUMP_SPECULAR_POWER]);
    pCompiledShader->Release();
    if (pErrors)
    {
        pErrors->Release();
    }

    if (FAILED(XGAssembleShader(NULL, g_szPixelShaderNormalMapLight, strlen(g_szPixelShaderNormalMapLight),
                         SASM_DEBUG, NULL, &pCompiledShader, &pErrors, NULL, NULL,
                         NULL, NULL)))
    {
        pErrors->GetBufferPointer();
    }
    g_pD3DDevice->CreatePixelShader(
        (D3DPIXELSHADERDEF*)pCompiledShader->GetBufferPointer(),
        &g_ahPixelShader[PIXEL_SHADER_NORMAL_MAP_LIGHT]);
    pCompiledShader->Release();
    if (pErrors)
    {
        pErrors->Release();
    }

    if (FAILED(XGAssembleShader(NULL, g_szPixelShaderNormalMapModulate, strlen(g_szPixelShaderNormalMapModulate),
                         SASM_DEBUG, NULL, &pCompiledShader, &pErrors, NULL, NULL,
                         NULL, NULL)))
    {
        pErrors->GetBufferPointer();
    }
    g_pD3DDevice->CreatePixelShader(
        (D3DPIXELSHADERDEF*)pCompiledShader->GetBufferPointer(),
        &g_ahPixelShader[PIXEL_SHADER_NORMAL_MAP_MODULATE]);
    pCompiledShader->Release();
    if (pErrors)
    {
        pErrors->Release();
    }

    if (FAILED(XGAssembleShader(NULL, g_szPixelShaderColorLookup, strlen(g_szPixelShaderColorLookup),
                         SASM_DEBUG, NULL, &pCompiledShader, &pErrors, NULL, NULL,
                         NULL, NULL)))
    {
        pErrors->GetBufferPointer();
    }
    g_pD3DDevice->CreatePixelShader(
        (D3DPIXELSHADERDEF*)pCompiledShader->GetBufferPointer(),
        &g_ahPixelShader[PIXEL_SHADER_COLOR_LOOKUP]);
    pCompiledShader->Release();
    if (pErrors)
    {
        pErrors->Release();
    }

    if (FAILED(XGAssembleShader(NULL, g_szPixelShaderAverage, strlen(g_szPixelShaderAverage),
                         SASM_DEBUG, NULL, &pCompiledShader, &pErrors, NULL, NULL,
                         NULL, NULL)))
    {
        pErrors->GetBufferPointer();
    }
    g_pD3DDevice->CreatePixelShader(
        (D3DPIXELSHADERDEF*)pCompiledShader->GetBufferPointer(),
        &g_ahPixelShader[PIXEL_SHADER_AVERAGE]);
    pCompiledShader->Release();
    if (pErrors)
    {
        pErrors->Release();
    }

    if (FAILED(XGAssembleShader(NULL, g_szPixelShaderAdd, strlen(g_szPixelShaderAdd),
                         SASM_DEBUG, NULL, &pCompiledShader, &pErrors, NULL, NULL,
                         NULL, NULL)))
    {
        pErrors->GetBufferPointer();
    }
    g_pD3DDevice->CreatePixelShader(
        (D3DPIXELSHADERDEF*)pCompiledShader->GetBufferPointer(),
        &g_ahPixelShader[PIXEL_SHADER_ADD]);
    pCompiledShader->Release();
    if (pErrors)
    {
        pErrors->Release();
    }

    if (FAILED(XGAssembleShader(NULL, g_szPixelShaderBlend, strlen(g_szPixelShaderBlend),
                         SASM_DEBUG, NULL, &pCompiledShader, &pErrors, NULL, NULL,
                         NULL, NULL)))
    {
        pErrors->GetBufferPointer();
    }
    g_pD3DDevice->CreatePixelShader(
        (D3DPIXELSHADERDEF*)pCompiledShader->GetBufferPointer(),
        &g_ahPixelShader[PIXEL_SHADER_BLEND]);
    pCompiledShader->Release();
    if (pErrors)
    {
        pErrors->Release();
    }

    if (FAILED(XGAssembleShader(NULL, g_szPixelShaderDependentLookup, strlen(g_szPixelShaderDependentLookup),
                         SASM_DEBUG, NULL, &pCompiledShader, &pErrors, NULL, NULL,
                         NULL, NULL)))
    {
        pErrors->GetBufferPointer();
    }
    g_pD3DDevice->CreatePixelShader(
        (D3DPIXELSHADERDEF*)pCompiledShader->GetBufferPointer(),
        &g_ahPixelShader[PIXEL_SHADER_DEPENDENT_LOOKUP]);
    pCompiledShader->Release();
    if (pErrors)
    {
        pErrors->Release();
    }

    if (FAILED(XGAssembleShader(NULL, g_szPixelShaderDistortion, strlen(g_szPixelShaderDistortion),
                         SASM_DEBUG, NULL, &pCompiledShader, &pErrors, NULL, NULL,
                         NULL, NULL)))
    {
        pErrors->GetBufferPointer();
    }
    g_pD3DDevice->CreatePixelShader(
        (D3DPIXELSHADERDEF*)pCompiledShader->GetBufferPointer(),
        &g_ahPixelShader[PIXEL_SHADER_DISTORTION]);
    pCompiledShader->Release();
    if (pErrors)
    {
        pErrors->Release();
    }

    if (FAILED(XGAssembleShader(NULL, g_szPixelShaderColorMatrix, strlen(g_szPixelShaderColorMatrix),
                         SASM_DEBUG, NULL, &pCompiledShader, &pErrors, NULL, NULL,
                         NULL, NULL)))
    {
        pErrors->GetBufferPointer();
    }
    g_pD3DDevice->CreatePixelShader(
        (D3DPIXELSHADERDEF*)pCompiledShader->GetBufferPointer(),
        &g_ahPixelShader[PIXEL_SHADER_COLOR_MATRIX]);
    pCompiledShader->Release();
    if (pErrors)
    {
        pErrors->Release();
    }

    if (FAILED(XGAssembleShader(NULL, g_szPixelShaderDepthSprite, strlen(g_szPixelShaderDepthSprite),
                         SASM_DEBUG, NULL, &pCompiledShader, &pErrors, NULL, NULL,
                         NULL, NULL)))
    {
        pErrors->GetBufferPointer();
    }
    g_pD3DDevice->CreatePixelShader(
        (D3DPIXELSHADERDEF*)pCompiledShader->GetBufferPointer(),
        &g_ahPixelShader[PIXEL_SHADER_DEPTH_SPRITE]);
    pCompiledShader->Release();
    if (pErrors)
    {
        pErrors->Release();
    }

    if (FAILED(XGAssembleShader(NULL, g_szPixelShaderColorLookupAlpha, strlen(g_szPixelShaderColorLookupAlpha),
                         SASM_DEBUG, NULL, &pCompiledShader, &pErrors, NULL, NULL,
                         NULL, NULL)))
    {
        pErrors->GetBufferPointer();
    }
    g_pD3DDevice->CreatePixelShader(
        (D3DPIXELSHADERDEF*)pCompiledShader->GetBufferPointer(),
        &g_ahPixelShader[PIXEL_SHADER_COLOR_LOOKUP_ALPHA]);
    pCompiledShader->Release();
    if (pErrors)
    {
        pErrors->Release();
    }

    if (FAILED(XGAssembleShader(NULL, g_szPixelShaderTextureModulate, strlen(g_szPixelShaderTextureModulate),
                         SASM_DEBUG, NULL, &pCompiledShader, &pErrors, NULL, NULL,
                         NULL, NULL)))
    {
        pErrors->GetBufferPointer();
    }
    g_pD3DDevice->CreatePixelShader(
        (D3DPIXELSHADERDEF*)pCompiledShader->GetBufferPointer(),
        &g_ahPixelShader[PIXEL_SHADER_TEXTURE_MODULATE]);
    pCompiledShader->Release();
    if (pErrors)
    {
        pErrors->Release();
    }

    if (FAILED(XGAssembleShader(NULL, g_szPixelShaderWhite, strlen(g_szPixelShaderWhite),
                         SASM_DEBUG, NULL, &pCompiledShader, &pErrors, NULL, NULL,
                         NULL, NULL)))
    {
        pErrors->GetBufferPointer();
    }
    g_pD3DDevice->CreatePixelShader(
        (D3DPIXELSHADERDEF*)pCompiledShader->GetBufferPointer(),
        &g_ahPixelShader[PIXEL_SHADER_WHITE]);
    pCompiledShader->Release();
    if (pErrors)
    {
        pErrors->Release();
    }

    return 1;
}

void GLRender::SetPerspectiveTransform(double fFieldOfView, double fAspect,
                              double fNearClip, double fFarClip)
{
    if (g_pD3DDevice)
    {
        D3DXMATRIX mProjection;
        D3DXMatrixPerspectiveFovRH(&mProjection, DegToRad((float)fFieldOfView),
                                   (float)fAspect, (float)fNearClip,
                                   (float)fFarClip);
        g_pD3DDevice->SetTransform(D3DTS_PROJECTION, &mProjection);
        g_pProjectionMatrixStack->LoadMatrix(&mProjection);
    }
}

void GLRender::SetViewTransform(Vector& vPosition, Quaternion& qOrientation)
{
    g_nMatrixMode = 1;
    LoadMatrixIdentity();
    if (multmatrix != 0)
    {
        Vector vOrigin;
        float fMatrix[16];
        MakeMatrix4x4(fMatrix, vOrigin, -qOrientation);
        MultMatrix(fMatrix);
        Translate(-vPosition.m_fX, -vPosition.m_fY, -vPosition.m_fZ);
    }
    else
    {
        Vector vAxis = qOrientation.axis();
        float fAngle = (float)(acos(qOrientation.m_fW) * 2.0) * -RAD_TO_DEG;
        Rotate(fAngle, vAxis.m_fX, vAxis.m_fY, vAxis.m_fZ);
        Translate(-vPosition.m_fX, -vPosition.m_fY, -vPosition.m_fZ);
    }
}

void GLRender::PopCurrentMatrix()
{
    g_nMatrixStackDepth--;
    switch (g_nMatrixMode)
    {
    case 2:
        g_pWorldMatrixStack->Pop();
        break;
    case 1:
        g_pViewMatrixStack->Pop();
        break;
    case 0:
        g_pProjectionMatrixStack->Pop();
        {
            int nMatrixMode = g_nMatrixMode;
            g_nMatrixMode = 0;
            g_pD3DDevice->SetTransform(D3DTS_PROJECTION,
                                       g_pProjectionMatrixStack->GetTop());
            g_nMatrixMode = nMatrixMode;
        }
        break;
    case 3:
        g_pTextureMatrixStack->Pop();
        break;
    }
}

void GLRender::MakeNormalizationCubeMap(int nSize)
{
    DXTextureState* pState = g_listTextureStates.m_pList[g_nCurrentTexture];
    pState->m_nWrapS = 1;
    pState->m_nWrapT = 1;
    pState->m_nMinFilter = 5;
    pState->m_nMipFilter = 5;
    pState->m_nMagFilter = 5;
    D3DCubeTexture* pCubeTexture;
    g_pD3DDevice->CreateCubeTexture(nSize, 1, 0, D3DFMT_X8R8G8B8, 0,
                                    &pCubeTexture);
    D3DBaseTexture* pOldTexture = g_listTextures.m_pList[g_nCurrentTexture];
    g_listTextures.m_pList[g_nCurrentTexture] = pCubeTexture;
    if (pOldTexture)
    {
        if (g_listTextureStates.m_pList[g_nCurrentTexture]->m_nType == 1)
        {
            g_pD3DDevice->SetTexture(0, NULL);
            g_pD3DDevice->SetTexture(1, NULL);
            g_pD3DDevice->SetTexture(2, NULL);
            g_pD3DDevice->SetTexture(3, NULL);
            delete pOldTexture;
        }
        else if (g_listTextureStates.m_pList[g_nCurrentTexture]->m_nType == 0)
        {
            pOldTexture->Release();
        }
    }
    g_pCurrentTexture = pCubeTexture;

    DWORD* pSourceBits = new DWORD[nSize * nSize];
    for (DWORD i = 0; i < 6; i++)
    {
        LPDIRECT3DSURFACE8 pCubeMapFace;
        pCubeTexture->GetCubeMapSurface((D3DCUBEMAP_FACES)i, 0, &pCubeMapFace);

        DWORD* pPixel = pSourceBits;
        D3DXVECTOR3 n;
        FLOAT w, h;
        for (DWORD y = 0; y < (DWORD)nSize; y++)
        {
            h = (FLOAT)y / (FLOAT)(nSize - 1);
            h = (h * 2.0f) - 1.0f;
            for (DWORD x = 0; x < (DWORD)nSize; x++)
            {
                w = (FLOAT)x / (FLOAT)(nSize - 1);
                w = (w * 2.0f) - 1.0f;
                switch (i)
                {
                case D3DCUBEMAP_FACE_POSITIVE_X:
                    n.x = +1.0f;
                    n.y = -h;
                    n.z = -w;
                    break;
                case D3DCUBEMAP_FACE_NEGATIVE_X:
                    n.x = -1.0f;
                    n.y = -h;
                    n.z = +w;
                    break;
                case D3DCUBEMAP_FACE_POSITIVE_Y:
                    n.x = +w;
                    n.y = +1.0f;
                    n.z = +h;
                    break;
                case D3DCUBEMAP_FACE_NEGATIVE_Y:
                    n.x = +w;
                    n.y = -1.0f;
                    n.z = -h;
                    break;
                case D3DCUBEMAP_FACE_POSITIVE_Z:
                    n.x = +w;
                    n.y = -h;
                    n.z = +1.0f;
                    break;
                case D3DCUBEMAP_FACE_NEGATIVE_Z:
                    n.x = -w;
                    n.y = -h;
                    n.z = -1.0f;
                    break;
                }
                D3DXVec3Normalize(&n, &n);
                DWORD r = (DWORD)((n.x + 1.0f) * 127.5);
                DWORD g = (DWORD)((n.y + 1.0f) * 127.5);
                DWORD b = (DWORD)((n.z + 1.0f) * 127.5);
                *pPixel++ = (((0x7f00 | r) << 8) | g) << 8 | b;
            }
        }

        D3DLOCKED_RECT lock;
        pCubeMapFace->LockRect(&lock, 0, 0L);
        XGSwizzleRect(pSourceBits, 0, NULL, lock.pBits, nSize, nSize, NULL,
                      sizeof(DWORD));
        pCubeMapFace->Release();
    }
    delete[] pSourceBits;
}

DXVertexProgram::~DXVertexProgram()
{
    if (m_pConstants)
    {
        m_pConstants->Release();
    }
    if (m_pCompiledShader)
    {
        m_pCompiledShader->Release();
    }
    if (m_pErrors)
    {
        m_pErrors->Release();
    }
    if (m_hShader)
    {
        g_pD3DDevice->DeleteVertexShader(m_hShader);
    }
}

void GLRender::ConvertHeightFieldAndLoadNormalMapTexture(
    unsigned char* pHeightField, int nWidth, int nHeight, int nRowWidth,
    int nRowCount, float fScale, int bUpdate, bool bNormalMap)
{
    Normal* pNormalMap;
    if (bNormalMap)
    {
        pNormalMap = new Normal[nWidth * nHeight];
        memcpy(pNormalMap, pHeightField, nWidth * nHeight * 4);
    }
    else
    {
        pNormalMap = convertHeightFieldToNormalMap(
            pHeightField, nWidth, nHeight, nRowWidth, nRowCount, fScale);
    }

    int nLevel = 0;
    D3DTexture* pD3DTexture;
    if (bUpdate && g_pCurrentTexture)
    {
        pD3DTexture = (D3DTexture*)g_pCurrentTexture;
    }
    else
    {
        g_pD3DDevice->CreateTexture(nWidth, nHeight, 0, 0, D3DFMT_A8R8G8B8, 0,
                                    &pD3DTexture);
        g_listTextureStates.m_pList[g_nCurrentTexture] = new DXTextureState;
    }
    g_pCurrentTextureState = g_listTextureStates.m_pList[g_nCurrentTexture];
    g_listTextures.m_pList[g_nCurrentTexture] = pD3DTexture;
    D3DSurface* pSurface;
    pD3DTexture->GetSurfaceLevel(0, &pSurface);
    D3DLOCKED_RECT lock;
    pSurface->LockRect(&lock, NULL, 0);
    CopyTextureToTextureMemory(pNormalMap, &lock, nWidth, nHeight, Aurora::TEXTURE_FORMAT_RGBA);
    pSurface->Release();

    while (nWidth > 1 || nHeight > 1)
    {
        ++nLevel;
        int nNewWidth = nWidth >> 1;
        int nNewHeight = nHeight >> 1;
        if (nNewWidth == 0)
            nNewWidth = 1;
        if (nNewHeight == 0)
            nNewHeight = 1;
        pNormalMap = downSampleNormalMap(pNormalMap, nWidth, nHeight,
                                          nNewWidth, nNewHeight);
        pD3DTexture->GetSurfaceLevel(nLevel, &pSurface);
        pSurface->LockRect(&lock, NULL, 0);
        CopyTextureToTextureMemory(pNormalMap, &lock, nNewWidth, nNewHeight,
                    Aurora::TEXTURE_FORMAT_RGBA);
        pSurface->Release();
        nWidth = nNewWidth;
        nHeight = nNewHeight;
    }
    delete[] pNormalMap;
}

void GLRender::RenderOverlayConvolution(D3DTexture* pTexture)
{
    static const DWORD dwDeclaration[] =
    {
        D3DVSD_STREAM(0),
        D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT3),
        D3DVSD_REG(D3DVSDE_DIFFUSE, D3DVSDT_D3DCOLOR),
        D3DVSD_REG(D3DVSDE_TEXCOORD0, D3DVSDT_FLOAT2),
        D3DVSD_REG(D3DVSDE_TEXCOORD1, D3DVSDT_FLOAT2),
        D3DVSD_REG(D3DVSDE_TEXCOORD2, D3DVSDT_FLOAT2),
        D3DVSD_REG(D3DVSDE_TEXCOORD3, D3DVSDT_FLOAT2),
        D3DVSD_END()
    };
    int i;
    D3DSurface* pSurface;

    if (!g_bConvolutionSetUp)
    {
        D3DLOCKED_RECT lock;
        g_pD3DDevice->CreateTexture(1, FRAMEBUFFER_LOOKUP_SIZE, 1, 0,
                                    D3DFMT_A8R8G8B8, 0, &cm_pRedTexture);
        g_pD3DDevice->CreateTexture(FRAMEBUFFER_LOOKUP_SIZE, 1, 1, 0,
                                    D3DFMT_A8R8G8B8, 0, &cm_pGreenTexture);
        g_pD3DDevice->CreateTexture(1, FRAMEBUFFER_LOOKUP_SIZE, 1, 0,
                                    D3DFMT_A8R8G8B8, 0, &cm_pBlueTexture);

        cm_pRedTexture->LockRect(0, &lock, NULL, 0);
        DWORD* pBits = (DWORD*)lock.pBits;
        for (i = 0; i < FRAMEBUFFER_LOOKUP_SIZE; i++)
        {
            if (i < FRAMEBUFFER_RED_THRESHOLD)
            {
                pBits[(lock.Pitch >> 2) * i] = 0;
            }
            else
            {
                float fLookup = (i - FRAMEBUFFER_RED_THRESHOLD) *
                    (1.0f / (FRAMEBUFFER_LOOKUP_SIZE - FRAMEBUFFER_RED_THRESHOLD - 1));
                pBits[(lock.Pitch >> 2) * i] =
                    (unsigned int)(fLookup * (AUR_FRAMEBUFFER_CHANNEL_MAX * AUR_FRAMEBUFFER_LOOKUP_GAIN))
                    << FRAMEBUFFER_BLUE_SHIFT;
            }
        }
        XGSwizzleRect(pBits, 0, NULL, lock.pBits, 1, FRAMEBUFFER_LOOKUP_SIZE,
                      NULL, sizeof(DWORD));

        cm_pGreenTexture->LockRect(0, &lock, NULL, 0);
        pBits = (DWORD*)lock.pBits;
        for (i = 0; i < FRAMEBUFFER_LOOKUP_SIZE; i++)
        {
            if (i < FRAMEBUFFER_GREEN_THRESHOLD)
            {
                pBits[i] = 0;
            }
            else
            {
                float fLookup = (i - FRAMEBUFFER_GREEN_THRESHOLD) *
                    (1.0f / (FRAMEBUFFER_LOOKUP_SIZE - FRAMEBUFFER_GREEN_THRESHOLD - 1));
                pBits[i] =
                    (unsigned int)(fLookup * (AUR_FRAMEBUFFER_CHANNEL_MAX * AUR_FRAMEBUFFER_LOOKUP_GAIN))
                    << FRAMEBUFFER_GREEN_SHIFT;
            }
        }
        XGSwizzleRect(pBits, 0, NULL, lock.pBits, FRAMEBUFFER_LOOKUP_SIZE, 1,
                      NULL, sizeof(DWORD));

        cm_pBlueTexture->LockRect(0, &lock, NULL, 0);
        pBits = (DWORD*)lock.pBits;
        for (i = 0; i < FRAMEBUFFER_LOOKUP_SIZE; i++)
        {
            if (i < FRAMEBUFFER_BLUE_THRESHOLD)
            {
                pBits[(lock.Pitch >> 2) * i] = 0;
            }
            else
            {
                float fLookup = (i - FRAMEBUFFER_BLUE_THRESHOLD) *
                    (1.0f / (FRAMEBUFFER_LOOKUP_SIZE - FRAMEBUFFER_BLUE_THRESHOLD - 1));
                pBits[(lock.Pitch >> 2) * i] =
                    (unsigned int)(fLookup * (AUR_FRAMEBUFFER_CHANNEL_MAX * AUR_FRAMEBUFFER_LOOKUP_GAIN));
            }
        }
        XGSwizzleRect(pBits, 0, NULL, lock.pBits, 1, FRAMEBUFFER_LOOKUP_SIZE,
                      NULL, sizeof(DWORD));

        g_pD3DDevice->CreateVertexShader(dwDeclaration, NULL,
                                         &g_hConvolutionVertexShader, 0);
        g_bConvolutionSetUp = 1;
        g_pD3DDevice->CreateTexture(80, 60, 1, 0, D3DFMT_LIN_A8R8G8B8, 0,
                                    &cm_pConvolutionTextureSmall);
        g_pD3DDevice->CreateTexture(160, 120, 1, 0, D3DFMT_LIN_A8R8G8B8, 0,
                                    &cm_pConvolutionTextureMedium);
        g_pD3DDevice->CreateTexture(320, 240, 1, 0, D3DFMT_LIN_A8R8G8B8, 0,
                                    &cm_pConvolutionTextureLarge);
    }

    g_pD3DDevice->SetVertexShader(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX4);
    SetBlendFunc(Aurora::BLEND_SRC_ALPHA, Aurora::BLEND_ONE);
    g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    D3DSurface* pDepthStencil;
    g_pD3DDevice->GetDepthStencilSurface(&pDepthStencil);

    cm_pConvolutionTextureLarge->GetSurfaceLevel(0, &pSurface);
    g_pD3DDevice->SetRenderTarget(pSurface, NULL);
    pSurface->Release();
    g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0, 1.0f, 0);
    g_pD3DDevice->SetTexture(0, pTexture);
    g_pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
    g_pD3DDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
    g_pD3DDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
    g_pD3DDevice->SetTexture(1, cm_pRedTexture);
    g_pD3DDevice->SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetTextureStageState(1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
    g_pD3DDevice->SetTextureStageState(1, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
    g_pD3DDevice->SetTextureStageState(1, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
    g_pD3DDevice->SetTexture(2, cm_pGreenTexture);
    g_pD3DDevice->SetTextureStageState(2, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetTextureStageState(2, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetTextureStageState(2, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
    g_pD3DDevice->SetTextureStageState(2, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
    g_pD3DDevice->SetTextureStageState(2, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
    g_pD3DDevice->SetTexture(3, cm_pBlueTexture);
    g_pD3DDevice->SetTextureStageState(3, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetTextureStageState(3, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetTextureStageState(3, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
    g_pD3DDevice->SetTextureStageState(3, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
    g_pD3DDevice->SetTextureStageState(3, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
    g_pD3DDevice->SetPixelShader(g_ahPixelShader[PIXEL_SHADER_COLOR_LOOKUP]);
    g_pD3DDevice->Begin(D3DPT_QUADLIST);
    for (i = 0; i < 4; i++)
    {
        g_pD3DDevice->SetVertexDataColor(D3DVSDE_DIFFUSE, 0xffffffff);
        if (g_nFrameBufferMode == 0)
        {
            float t = (i == 0 || i == 3) ? 0.0f : (float)g_nScreenHeight;
            float s = (i == 0 || i == 1) ? 0.0f : (float)g_nScreenWidth;
            g_pD3DDevice->SetVertexData2f(D3DVSDE_TEXCOORD0, s, t);
        }
        else
        {
            float t = (i == 0 || i == 3) ? 0.0f : (float)(g_nScreenHeight * 2);
            float s = (i == 0 || i == 1) ? 0.0f : (float)g_nScreenWidth;
            g_pD3DDevice->SetVertexData2f(D3DVSDE_TEXCOORD0, s, t);
        }
        float y = (i == 0 || i == 3) ? 1.0f : -1.0f;
        float x = (i == 0 || i == 1) ? -1.0f : 1.0f;
        g_pD3DDevice->SetVertexData4f(D3DVSDE_POSITION, x, y, 0.0f, 1.0f);
    }
    g_pD3DDevice->End();

    cm_pConvolutionTextureMedium->GetSurfaceLevel(0, &pSurface);
    g_pD3DDevice->SetRenderTarget(pSurface, NULL);
    pSurface->Release();
    g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0, 1.0f, 0);
    g_pD3DDevice->SetTexture(0, cm_pConvolutionTextureLarge);
    g_pD3DDevice->SetTexture(1, NULL);
    g_pD3DDevice->SetTexture(2, NULL);
    g_pD3DDevice->SetTexture(3, NULL);
    g_pD3DDevice->SetPixelShader(g_ahPixelShader[PIXEL_SHADER_TEXTURE]);
    g_pD3DDevice->Begin(D3DPT_QUADLIST);
    for (i = 0; i < 4; i++)
    {
        g_pD3DDevice->SetVertexDataColor(D3DVSDE_DIFFUSE, 0xffffffff);
        float t = (i == 0 || i == 3) ? 0.0f : 240.0f;
        float s = (i == 0 || i == 1) ? 0.0f : 320.0f;
        g_pD3DDevice->SetVertexData2f(D3DVSDE_TEXCOORD0, s, t);
        float y = (i == 0 || i == 3) ? 1.0f : -1.0f;
        float x = (i == 0 || i == 1) ? -1.0f : 1.0f;
        g_pD3DDevice->SetVertexData4f(D3DVSDE_POSITION, x, y, 0.0f, 1.0f);
    }
    g_pD3DDevice->End();

    cm_pConvolutionTextureSmall->GetSurfaceLevel(0, &pSurface);
    g_pD3DDevice->SetRenderTarget(pSurface, NULL);
    pSurface->Release();
    g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0, 1.0f, 0);
    g_pD3DDevice->SetTexture(0, cm_pConvolutionTextureMedium);
    g_pD3DDevice->Begin(D3DPT_QUADLIST);
    for (i = 0; i < 4; i++)
    {
        g_pD3DDevice->SetVertexDataColor(D3DVSDE_DIFFUSE, 0xffffffff);
        float t = (i == 0 || i == 3) ? 0.0f : 120.0f;
        float s = (i == 0 || i == 1) ? 0.0f : 160.0f;
        g_pD3DDevice->SetVertexData2f(D3DVSDE_TEXCOORD0, s, t);
        float y = (i == 0 || i == 3) ? 1.0f : -1.0f;
        float x = (i == 0 || i == 1) ? -1.0f : 1.0f;
        g_pD3DDevice->SetVertexData4f(D3DVSDE_POSITION, x, y, 0.0f, 1.0f);
    }
    g_pD3DDevice->End();

    cm_pConvolutionTextureLarge->GetSurfaceLevel(0, &pSurface);
    g_pD3DDevice->SetRenderTarget(pSurface, NULL);
    pSurface->Release();
    g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0, 1.0f, 0);
    g_pD3DDevice->SetTexture(0, cm_pConvolutionTextureSmall);
    g_pD3DDevice->SetTexture(1, cm_pConvolutionTextureSmall);
    g_pD3DDevice->SetTexture(2, cm_pConvolutionTextureSmall);
    g_pD3DDevice->SetTexture(3, cm_pConvolutionTextureSmall);
    g_pD3DDevice->SetVertexShader(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX4);
    g_pD3DDevice->SetPixelShader(g_ahPixelShader[PIXEL_SHADER_FOUR_TEXTURE_WEIGHTED]);
    int nLine = 0;
    int nColumn = 0;
    for (int nTap = 0; nTap < 25; nTap += 4)
    {
        for (int j = 0; j < 4; j++)
        {
            float fWeight[4];
            fWeight[0] = g_afConvolutionWeights[nTap + j];
            fWeight[1] = g_afConvolutionWeights[nTap + j];
            fWeight[2] = g_afConvolutionWeights[nTap + j];
            fWeight[3] = g_afConvolutionWeights[nTap + j];
            g_pD3DDevice->SetPixelShaderConstant(j, fWeight, 1);
        }
        g_pD3DDevice->Begin(D3DPT_QUADLIST);
        for (i = 0; i < 4; i++)
        {
            int nRow;
            int nColumn1 = (nColumn + 1) % 5;
            int nColumn2 = (nColumn + 2) % 5;
            int nColumn3 = (nColumn + 3) % 5;
            g_pD3DDevice->SetVertexDataColor(
                D3DVSDE_DIFFUSE,
                ((int)(multiplier * 255.0f) << 24) | 0xffffff);
            float t0 = (i == 0 || i == 3) ? (float)nLine - 1.0f : (float)(nLine + 59);
            float s0 = (i == 0 || i == 1) ? (float)nColumn - 1.0f : (float)nColumn + 80.0f - 1.0f;
            g_pD3DDevice->SetVertexData2f(D3DVSDE_TEXCOORD0, s0, t0);
            nRow = (nColumn + 1 > 4) ? nLine + 1 : nLine;
            float t1 = (i == 0 || i == 3) ? (float)nColumn1 - 1.0f : (float)(nColumn1 + 59);
            float s1 = (i == 0 || i == 1) ? (float)nRow - 1.0f : (float)(nRow + 79);
            g_pD3DDevice->SetVertexData2f(D3DVSDE_TEXCOORD1, s1, t1);
            nRow = (nColumn + 2 > 4) ? nLine + 1 : nLine;
            float t2 = (i == 0 || i == 3) ? (float)nColumn2 - 1.0f : (float)(nColumn2 + 59);
            float s2 = (i == 0 || i == 1) ? (float)nRow - 1.0f : (float)(nRow + 79);
            g_pD3DDevice->SetVertexData2f(D3DVSDE_TEXCOORD2, s2, t2);
            nRow = (nColumn + 3 > 4) ? nLine + 1 : nLine;
            float t3 = (i == 0 || i == 3) ? (float)nColumn3 - 1.0f : (float)(nColumn3 + 59);
            float s3 = (i == 0 || i == 1) ? (float)nRow - 1.0f : (float)(nRow + 79);
            g_pD3DDevice->SetVertexData2f(D3DVSDE_TEXCOORD3, s3, t3);
            float y = (i == 0 || i == 3) ? 1.0f : -1.0f;
            float x = (i == 0 || i == 1) ? -1.0f : 1.0f;
            g_pD3DDevice->SetVertexData4f(D3DVSDE_POSITION, x, y, 0.0f, 1.0f);
        }
        g_pD3DDevice->End();
        nColumn += 4;
        if (nColumn > 4)
        {
            nLine++;
        }
        nColumn %= 5;
    }

    D3DSurface* pBackBuffer;
    g_pD3DDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
    g_pD3DDevice->SetRenderTarget(pBackBuffer, pDepthStencil);
    g_pD3DDevice->SetVertexShader(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
    g_pD3DDevice->SetTexture(0, cm_pConvolutionTextureLarge);
    g_pD3DDevice->SetTexture(1, NULL);
    g_pD3DDevice->SetTexture(2, NULL);
    g_pD3DDevice->SetTexture(3, NULL);
    g_pD3DDevice->SetPixelShader(g_ahPixelShader[PIXEL_SHADER_TEXTURE]);
    g_pD3DDevice->Begin(D3DPT_QUADLIST);
    for (i = 0; i < 4; i++)
    {
        g_pD3DDevice->SetVertexDataColor(D3DVSDE_DIFFUSE, 0xffffffff);
        float t = (i == 0 || i == 3) ? 0.0f : 240.0f;
        float s = (i == 0 || i == 1) ? 0.0f : 320.0f;
        g_pD3DDevice->SetVertexData2f(D3DVSDE_TEXCOORD0, s, t);
        float y = (i == 0 || i == 3) ? 1.0f : -1.0f;
        float x = (i == 0 || i == 1) ? -1.0f : 1.0f;
        g_pD3DDevice->SetVertexData4f(D3DVSDE_POSITION, x, y, 0.0f, 1.0f);
    }
    g_pD3DDevice->End();
    pDepthStencil->Release();
    pBackBuffer->Release();
    g_pD3DDevice->SetPixelShader(0);
    g_pD3DDevice->SetTexture(0, NULL);
    g_pD3DDevice->SetTexture(1, NULL);
    g_pD3DDevice->SetTexture(2, NULL);
    g_pD3DDevice->SetTexture(3, NULL);
    SetTextureStageFlag(0, 0);
    SetTextureStageFlag(1, 0);
    SetTextureStageFlag(2, 0);
    SetTextureStageFlag(3, 0);
}

int GetCompressedContactSheetOffset(int nIndex, int nWidth, int nBytesPerPixel);

int totalvertexpool;
int totalvertexpool4k;
int totalindexpool;
int totalindexpool4;

int rerunoverbrighten;
static D3DTexture* g_pBrightnessBlueTexture;
static D3DTexture* g_pBrightnessGreenTexture;
static D3DTexture* g_pBrightnessRedTexture;
static int g_bBrightnessSetUp;

void GLRender::RenderBrightnessAdjustment(unsigned long nTexture)
{
    SetBlendFunc(BLEND_ONE, BLEND_ZERO);
    if (!g_bBrightnessSetUp || rerunoverbrighten)
    {
        D3DLOCKED_RECT lock;
        unsigned int i;
        rerunoverbrighten = 0;
        g_bBrightnessSetUp = 1;
        g_pD3DDevice->CreateTexture(1, FRAMEBUFFER_LOOKUP_SIZE, 1, 0, D3DFMT_A8R8G8B8, 0,
                                    &g_pBrightnessRedTexture);
        g_pD3DDevice->CreateTexture(FRAMEBUFFER_LOOKUP_SIZE, 1, 1, 0, D3DFMT_A8R8G8B8, 0,
                                    &g_pBrightnessGreenTexture);
        g_pD3DDevice->CreateTexture(1, FRAMEBUFFER_LOOKUP_SIZE, 1, 0, D3DFMT_A8R8G8B8, 0,
                                    &g_pBrightnessBlueTexture);

        g_pBrightnessRedTexture->LockRect(0, &lock, NULL, 0);
        DWORD* pBits = (DWORD*)lock.pBits;
        for (i = 0; i < FRAMEBUFFER_LOOKUP_SIZE; i++)
        {
            float fValue = AdjustBrightness(i * (1.0f / 255.0f));
            pBits[(lock.Pitch >> 2) * i] = (unsigned int)(fValue * 255.0f) << FRAMEBUFFER_BLUE_SHIFT;
        }
        XGSwizzleRect(pBits, 0, NULL, lock.pBits, 1, FRAMEBUFFER_LOOKUP_SIZE, NULL, sizeof(DWORD));

        g_pBrightnessGreenTexture->LockRect(0, &lock, NULL, 0);
        pBits = (DWORD*)lock.pBits;
        for (i = 0; i < FRAMEBUFFER_LOOKUP_SIZE; i++)
        {
            float fValue = AdjustBrightness(i * (1.0f / 255.0f));
            pBits[i] = (unsigned int)(fValue * 255.0f) << FRAMEBUFFER_GREEN_SHIFT;
        }
        XGSwizzleRect(pBits, 0, NULL, lock.pBits, FRAMEBUFFER_LOOKUP_SIZE, 1, NULL, sizeof(DWORD));

        g_pBrightnessBlueTexture->LockRect(0, &lock, NULL, 0);
        pBits = (DWORD*)lock.pBits;
        for (i = 0; i < FRAMEBUFFER_LOOKUP_SIZE; i++)
        {
            float fValue = AdjustBrightness(i * (1.0f / 255.0f));
            pBits[(lock.Pitch >> 2) * i] = (unsigned int)(fValue * 255.0f);
        }
        XGSwizzleRect(pBits, 0, NULL, lock.pBits, 1, FRAMEBUFFER_LOOKUP_SIZE, NULL, sizeof(DWORD));
    }

    g_pD3DDevice->SetTexture(0, (D3DBaseTexture*)nTexture);
    SetTextureStageFlag(0, 0);
    g_pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_NONE);
    g_pD3DDevice->SetTexture(1, g_pBrightnessRedTexture);
    SetTextureStageFlag(1, 0);
    g_pD3DDevice->SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetTextureStageState(1, D3DTSS_MIPFILTER, D3DTEXF_NONE);
    g_pD3DDevice->SetTexture(2, g_pBrightnessGreenTexture);
    SetTextureStageFlag(2, 0);
    g_pD3DDevice->SetTextureStageState(2, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetTextureStageState(2, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetTextureStageState(2, D3DTSS_MIPFILTER, D3DTEXF_NONE);
    g_pD3DDevice->SetTexture(3, g_pBrightnessBlueTexture);
    SetTextureStageFlag(3, 0);
    g_pD3DDevice->SetTextureStageState(3, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetTextureStageState(3, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetTextureStageState(3, D3DTSS_MIPFILTER, D3DTEXF_NONE);
    g_pD3DDevice->SetPixelShader(g_ahPixelShader[PIXEL_SHADER_COLOR_LOOKUP_ALPHA]);
    g_pD3DDevice->Begin(D3DPT_QUADLIST);
    for (unsigned int i = 0; i < 4; i++)
    {
        g_pD3DDevice->SetVertexDataColor(D3DVSDE_DIFFUSE, 0xffffffff);
        if (g_nFrameBufferMode == 0)
        {
            float t = (i == 0 || i == 3) ? 0.0f : 480.0f;
            float s = (i == 0 || i == 1) ? 0.0f : 640.0f;
            g_pD3DDevice->SetVertexData2f(D3DVSDE_TEXCOORD0, s, t);
        }
        else
        {
            float t = (i == 0 || i == 3) ? 0.0f : 960.0f;
            float s = (i == 0 || i == 1) ? 0.0f : 640.0f;
            g_pD3DDevice->SetVertexData2f(D3DVSDE_TEXCOORD0, s, t);
        }
        float y = (i == 0 || i == 3) ? 1.0f : -1.0f;
        float x = (i == 0 || i == 1) ? -1.0f : 1.0f;
        g_pD3DDevice->SetVertexData4f(D3DVSDE_POSITION, x, y, 0.0f, 1.0f);
    }
    g_pD3DDevice->End();
    g_pD3DDevice->SetPixelShader(0);
    g_pD3DDevice->SetTexture(0, NULL);
    g_pD3DDevice->SetTexture(1, NULL);
    g_pD3DDevice->SetTexture(2, NULL);
    g_pD3DDevice->SetTexture(3, NULL);
    SetTextureStageFlag(0, 0);
    SetTextureStageFlag(1, 0);
    SetTextureStageFlag(2, 0);
    SetTextureStageFlag(3, 0);
}

void GLRender::RenderOverlayOverbrightenATI(unsigned long nTexture)
{
    SetBlendFunc(BLEND_ONE, BLEND_ZERO);
    g_pD3DDevice->SetTexture(0, (D3DBaseTexture*)nTexture);
    SetTextureStageFlag(0, 0);
    g_pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetPixelShader(g_ahPixelShader[PIXEL_SHADER_ADD]);
    SetActiveTexture(1);
    cm_pFilmNoiseTexture->Bind();
    g_pD3DDevice->Begin(D3DPT_QUADLIST);
    for (int i = 0; i < 4; i++)
    {
        g_pD3DDevice->SetVertexDataColor(D3DVSDE_DIFFUSE, 0xffffffff);
        if (g_nFrameBufferMode == 0)
        {
            float t = (i == 0 || i == 3) ? 0.0f : 480.0f;
            float s = (i == 0 || i == 1) ? 0.0f : 640.0f;
            g_pD3DDevice->SetVertexData2f(D3DVSDE_TEXCOORD0, s, t);
        }
        else
        {
            float t = (i == 0 || i == 3) ? 0.0f : 960.0f;
            float s = (i == 0 || i == 1) ? 0.0f : 640.0f;
            g_pD3DDevice->SetVertexData2f(D3DVSDE_TEXCOORD0, s, t);
        }
        float fNoise = (i == 0 || i == 3) ? 0.0f : 7.5f;
        g_pD3DDevice->SetVertexData2f(D3DVSDE_TEXCOORD1, 0.0f, fNoise);
        float y = (i == 0 || i == 3) ? 1.0f : -1.0f;
        float x = (i == 0 || i == 1) ? -1.0f : 1.0f;
        g_pD3DDevice->SetVertexData4f(D3DVSDE_POSITION, x, y, 0.0f, 1.0f);
    }
    g_pD3DDevice->End();
    g_pD3DDevice->SetPixelShader(0);
    SetActiveTexture(0);
}

void GLRender::RenderSaturationAdjustment(unsigned long nTexture)
{
    SetBlendFunc(BLEND_ONE, BLEND_ZERO);
    g_pD3DDevice->SetTexture(0, (D3DBaseTexture*)nTexture);
    SetTextureStageFlag(0, 0);
    g_pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetPixelShader(g_ahPixelShader[PIXEL_SHADER_COLOR_MATRIX]);
    float fDesaturation = 1.0f - m_fSaturation;
    Vector vRed;
    Vector vGreen;
    Vector vBlue;
    vRed.m_fX = m_fSaturation + 0.3086f * fDesaturation;
    vGreen.m_fX = 0.3086f * fDesaturation;
    vBlue.m_fX = 0.3086f * fDesaturation;
    vRed.m_fY = 0.6094f * fDesaturation;
    vGreen.m_fY = m_fSaturation + 0.6094f * fDesaturation;
    vBlue.m_fY = 0.6094f * fDesaturation;
    vRed.m_fZ = 0.0820f * fDesaturation;
    vGreen.m_fZ = 0.0820f * fDesaturation;
    vBlue.m_fZ = 0.0820f * fDesaturation + m_fSaturation;
    vRed = vRed * m_vSaturationMultiplier.m_fX;
    vGreen = vGreen * m_vSaturationMultiplier.m_fY;
    vBlue = vBlue * m_vSaturationMultiplier.m_fZ;
    g_pD3DDevice->SetPixelShaderConstant(1, Homogenize(vRed, 0.0f), 1);
    g_pD3DDevice->SetPixelShaderConstant(2, Homogenize(vGreen, 0.0f), 1);
    g_pD3DDevice->SetPixelShaderConstant(3, Homogenize(vBlue, 0.0f), 1);
    g_pD3DDevice->Begin(D3DPT_QUADLIST);
    for (int i = 0; i < 4; i++)
    {
        g_pD3DDevice->SetVertexDataColor(D3DVSDE_DIFFUSE, 0xffff0000);
        if (g_nFrameBufferMode == 0)
        {
            float t = (i == 0 || i == 3) ? 0.0f : 480.0f;
            float s = (i == 0 || i == 1) ? 0.0f : 640.0f;
            g_pD3DDevice->SetVertexData2f(D3DVSDE_TEXCOORD0, s, t);
        }
        else
        {
            float t = (i == 0 || i == 3) ? 0.0f : 960.0f;
            float s = (i == 0 || i == 1) ? 0.0f : 640.0f;
            g_pD3DDevice->SetVertexData2f(D3DVSDE_TEXCOORD0, s, t);
        }
        float y = (i == 0 || i == 3) ? 1.0f : -1.0f;
        float x = (i == 0 || i == 1) ? -1.0f : 1.0f;
        g_pD3DDevice->SetVertexData4f(D3DVSDE_POSITION, x, y, 0.0f, 1.0f);
    }
    g_pD3DDevice->End();
    g_pD3DDevice->SetPixelShader(0);
    SetActiveTexture(0);
}

void GLRender::FrameBufferModifications2(unsigned long nFlags)
{
    if (doframebuffer && g_nFrameBufferMode != 4 && visualizepass4 && m_bSaturationEnabled &&
        (nFlags & FRAMEBUFFER_SATURATION))
    {
        g_pD3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
        g_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
        static unsigned long nShader = TranslatePoolFormat(RequestPoolManager::POOL_FORMAT_VTC);
        g_pD3DDevice->SetVertexShader(nShader);
        Enable(ENABLE_BLEND);

        D3DXMATRIX mProjection;
        D3DXMatrixOrthoOffCenterRH(&mProjection, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
        D3DXMATRIX mIdentity;
        D3DXMatrixIdentity(&mIdentity);
        g_pD3DDevice->SetTransform(D3DTS_PROJECTION, &mProjection);
        g_pD3DDevice->SetTransform(D3DTS_VIEW, &mIdentity);
        g_pD3DDevice->SetTransform(D3DTS_WORLD, &mIdentity);

        D3DSurface* pBackBuffer;
        g_pD3DDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
        D3DSURFACE_DESC desc;
        pBackBuffer->GetDesc(&desc);
        D3DSurface* pSurface;
        g_pFrameTexture->GetSurfaceLevel(0, &pSurface);
        g_pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
        g_pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
        g_pD3DDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
        g_pD3DDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
        g_pD3DDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
        if (visualizepass4 && m_bSaturationEnabled)
        {
            g_pD3DDevice->CopyRects(pBackBuffer, NULL, 1, pSurface, NULL);
            RenderSaturationAdjustment((unsigned long)g_pFrameTexture);
        }
        SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
        pSurface->Release();
        pBackBuffer->Release();

        g_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
        g_pD3DDevice->SetRenderState(D3DRS_COLORVERTEX, FALSE);
        g_pD3DDevice->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);
        g_pD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
        g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
        g_pD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
        g_pD3DDevice->SetTransform(D3DTS_PROJECTION, g_pProjectionMatrixStack->GetTop());
    }
}

extern D3DXVECTOR4 g_vShadowLightClip;
extern D3DXVECTOR4 g_vShadowLightView;

void GLRender::SetShadowLightConstants()
{
    float fConstant[4];

    fConstant[0] = g_vShadowLightClip.x;
    fConstant[1] = g_vShadowLightClip.y;
    fConstant[2] = g_vShadowLightClip.z;
    fConstant[3] = g_vShadowLightClip.w;
    g_pD3DDevice->SetVertexShaderConstant(SHADOW_CONSTANT_LIGHT_CLIP, fConstant, 1);
    fConstant[0] = g_vShadowLightView.x;
    fConstant[1] = g_vShadowLightView.y;
    fConstant[2] = g_vShadowLightView.z;
    fConstant[3] = g_vShadowLightView.w;
    g_pD3DDevice->SetVertexShaderConstant(SHADOW_CONSTANT_LIGHT_VIEW, fConstant, 1);
}

void GLRender::RenderShadowArray(int nVertices, int nEdges, int nPool)
{
    static unsigned short* indexToken = new unsigned short[AUR_SHADOW_INDEX_COUNT];
    if (nEdges == 0)
        return;

    int nNear = nVertices * 2;
    int nFar = nNear + 1;
    int nIndex = 0;
    for (int i = 0; i < nEdges; ++i)
    {
        int nFirst = m_UsedVertices[PartTriMesh::m_VertexIndices[i * 2]] * 2 - 2;
        int nSecond = m_UsedVertices[PartTriMesh::m_VertexIndices[i * 2 + 1]] * 2 - 2;
        if (i % 2 == 0)
        {
            indexToken[nIndex++] = nNear;
            indexToken[nIndex++] = nFirst;
            indexToken[nIndex++] = nSecond;
            indexToken[nIndex++] = nFirst + 1;
            indexToken[nIndex++] = nSecond + 1;
            indexToken[nIndex++] = nFar;
        }
        else
        {
            indexToken[nIndex++] = nFar;
            indexToken[nIndex++] = nSecond + 1;
            indexToken[nIndex++] = nFirst + 1;
            indexToken[nIndex++] = nSecond;
            indexToken[nIndex++] = nFirst;
            indexToken[nIndex++] = nNear;
        }
    }

    int nMode = g_nMatrixMode;
    g_nMatrixMode = 1;
    g_pD3DDevice->SetTransform(D3DTS_VIEW, g_pViewMatrixStack->GetTop());
    g_nMatrixMode = 2;
    g_pD3DDevice->SetTransform(D3DTS_WORLD, g_pWorldMatrixStack->GetTop());
    g_nMatrixMode = nMode;

    g_pD3DDevice->SetVertexShader(D3DFVF_XYZ);
    g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    SetStencilOp(0, 3, 0);
    int nStrip = nEdges * 6 - 2;
    g_pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLESTRIP, 0, 0, nStrip, indexToken,
                                         D3DFMT_INDEX16, cm_pUserShadowData, 12);
    g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
    SetStencilOp(0, 4, 0);
    g_pD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLESTRIP, 0, 0, nStrip, indexToken,
                                         D3DFMT_INDEX16, cm_pUserShadowData, 12);
    countfaces += nEdges * 8;
}

void GLRender::EnableLight(unsigned long nLight)
{
    if (!g_lstLights.Contains(nLight))
    {
        g_lstLights.Add(nLight);

        D3DLIGHT8 light;
        memset(&light, 0, sizeof(light));
        light.Type = D3DLIGHT_POINT;
        light.Range = 100000.0f;
        light.Attenuation0 = 1.0f;
        light.Direction = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
        D3DXCOLOR color(1.0f, 1.0f, 1.0f, 0.0f);
        light.Diffuse = color;
        color.r = 0.2f;
        color.g = 0.2f;
        color.b = 0.2f;
        light.Ambient = color;
        g_pD3DDevice->SetLight(nLight, &light);
    }
    g_pD3DDevice->LightEnable(nLight, TRUE);
}

void GLRender::GenPrograms(int nCount, unsigned int* pnPrograms)
{
    for (unsigned int i = 0; i < nCount; i++)
    {
        int nProgram;
        for (nProgram = 1; nProgram < g_listVertexPrograms.m_nCount; nProgram++)
        {
            if (g_listVertexPrograms[nProgram] == NULL)
            {
                pnPrograms[i] = nProgram;
                g_listVertexPrograms[nProgram] = new DXVertexProgram;
                break;
            }
        }
        if (nProgram == g_listVertexPrograms.m_nCount)
        {
            g_listVertexPrograms.Add(new DXVertexProgram);
            pnPrograms[i] = nProgram;
        }
    }
}

void RequestPoolManager::DeletePool(unsigned long nPool)
{
    if (nPool == INVALID_POOL)
    {
        return;
    }
    if (nPool & POOL_VERTEX_FLAG)
    {
        nPool &= POOL_INDEX_MASK;
        if (m_ppVertexPools[nPool])
        {
            if (m_pVertexPoolState[nPool] == POOL_STATE_ADOPTED)
            {
                g_pD3DDevice->SetStreamSource(0, NULL, 0);
                delete m_ppVertexPools[nPool];
            }
            else
            {
                m_ppVertexPools[nPool]->Release();
            }
            m_ppVertexPools[nPool] = NULL;
        }
        totalvertexpool -= m_pVertexPoolSize[nPool];
        totalvertexpool4k -= (m_pVertexPoolSize[nPool] + POOL_PAGE_ROUNDING) & POOL_PAGE_MASK;
        m_pVertexPoolSize[nPool] = 0;
        m_pVertexPoolState[nPool] = POOL_STATE_FREE;
        m_lstFreeVertexPools.Add(nPool);
    }
    else
    {
        if (!m_ppIndexPools[nPool])
        {
            AurReportError(1004, "aurdxcore.cpp", "Trying to delete a non-existant index pool!");
        }
        else
        {
            if (m_pIndexPoolState[nPool] == POOL_STATE_ADOPTED)
            {
                g_pD3DDevice->SetIndices(NULL, 0);
                delete m_ppIndexPools[nPool];
            }
            else
            {
                m_ppIndexPools[nPool]->Release();
            }
            m_ppIndexPools[nPool] = NULL;
        }
        totalindexpool -= m_pIndexPoolSize[nPool & POOL_INDEX_MASK];
        totalindexpool4 -=
            (m_pIndexPoolSize[nPool & POOL_INDEX_MASK] + POOL_INDEX_ROUNDING) & POOL_INDEX_ROUND_MASK;
        m_pIndexPoolSize[nPool] = 0;
        m_pIndexPoolState[nPool] = POOL_STATE_FREE;
        m_lstFreeIndexPools.Add(nPool);
    }
}

unsigned long RequestPoolManager::AddVertexPool(D3DVertexBuffer* pBuffer, unsigned long nSize)
{
    totalvertexpool += nSize;
    totalvertexpool4k += (nSize + POOL_PAGE_ROUNDING) & POOL_PAGE_MASK;
    if (m_lstFreeVertexPools.m_nCount > 0)
    {
        unsigned long nPool = m_lstFreeVertexPools[m_lstFreeVertexPools.m_nCount - 1];
        m_ppVertexPools[nPool] = pBuffer;
        m_pVertexPoolSize[nPool] = nSize;
        m_pVertexPoolState[nPool] = POOL_STATE_ADOPTED;
        m_lstFreeVertexPools.DelIndex(m_lstFreeVertexPools.m_nCount - 1);
        return nPool | POOL_VERTEX_FLAG;
    }

    unsigned char* pState = new unsigned char[m_nVertexPools * 2];
    memset(pState, 0, m_nVertexPools * 2);
    memcpy(pState, m_pVertexPoolState, m_nVertexPools);
    delete[] m_pVertexPoolState;
    m_pVertexPoolState = pState;
    m_pVertexPoolState[m_nVertexPools] = POOL_STATE_ADOPTED;

    unsigned long* pSize = new unsigned long[m_nVertexPools * 2];
    for (unsigned int i = 1; i < m_nVertexPools; i++)
    {
        m_lstFreeVertexPools.Add(m_nVertexPools + i);
    }
    memcpy(pSize, m_pVertexPoolSize, m_nVertexPools * sizeof(unsigned long));
    delete[] m_pVertexPoolSize;
    m_pVertexPoolSize = pSize;
    m_pVertexPoolSize[m_nVertexPools] = nSize;

    D3DVertexBuffer** ppPools = new D3DVertexBuffer*[m_nVertexPools * 2];
    memset(ppPools, 0, m_nVertexPools * 2 * sizeof(D3DVertexBuffer*));
    memcpy(ppPools, m_ppVertexPools, m_nVertexPools * sizeof(D3DVertexBuffer*));
    delete[] m_ppVertexPools;
    m_ppVertexPools = ppPools;
    m_ppVertexPools[m_nVertexPools] = pBuffer;

    unsigned long nPool = m_nVertexPools;
    m_nVertexPools *= 2;
    return nPool | POOL_VERTEX_FLAG;
}

unsigned long RequestPoolManager::AddIndexPool(D3DIndexBuffer* pBuffer, unsigned long nSize)
{
    totalindexpool += nSize;
    if (m_lstFreeIndexPools.m_nCount > 0)
    {
        unsigned long nPool = m_lstFreeIndexPools[m_lstFreeIndexPools.m_nCount - 1];
        m_ppIndexPools[nPool] = pBuffer;
        m_pIndexPoolSize[nPool] = nSize;
        m_pIndexPoolState[nPool] = POOL_STATE_ADOPTED;
        m_lstFreeIndexPools.DelIndex(m_lstFreeIndexPools.m_nCount - 1);
        return nPool;
    }

    unsigned char* pState = new unsigned char[m_nIndexPools * 2];
    memset(pState, 0, m_nIndexPools * 2);
    memcpy(pState, m_pIndexPoolState, m_nIndexPools);
    delete[] m_pIndexPoolState;
    m_pIndexPoolState = pState;
    m_pIndexPoolState[m_nIndexPools] = POOL_STATE_ADOPTED;

    for (unsigned int i = 1; i < m_nIndexPools; i++)
    {
        m_lstFreeIndexPools.Add(m_nIndexPools + i);
    }
    unsigned long* pSize = new unsigned long[m_nIndexPools * 2];
    memset(pSize, 0, m_nIndexPools * 2 * sizeof(unsigned long));
    memcpy(pSize, m_pIndexPoolSize, m_nIndexPools * sizeof(unsigned long));
    delete[] m_pIndexPoolSize;
    m_pIndexPoolSize = pSize;
    m_pIndexPoolSize[m_nIndexPools] = nSize;

    D3DIndexBuffer** ppPools = new D3DIndexBuffer*[m_nIndexPools * 2];
    memset(ppPools, 0, m_nIndexPools * 2 * sizeof(D3DIndexBuffer*));
    memcpy(ppPools, m_ppIndexPools, m_nIndexPools * sizeof(D3DIndexBuffer*));
    delete[] m_ppIndexPools;
    m_ppIndexPools = ppPools;
    m_ppIndexPools[m_nIndexPools] = pBuffer;

    unsigned long nPool = m_nIndexPools;
    m_nIndexPools *= 2;
    return nPool;
}

unsigned long RequestPoolManager::RequestIndexPool(unsigned long nSize, unsigned long nUsage,
                                                   D3DFORMAT nFormat, unsigned long* pnPool,
                                                   D3DDevice* pDevice)
{
    if (nSize == 0)
    {
        *pnPool = INVALID_POOL;
        return INVALID_POOL;
    }
    totalindexpool += nSize;
    totalindexpool4 += (nSize + POOL_INDEX_ROUNDING) & POOL_INDEX_ROUND_MASK;

    D3DIndexBuffer* pBuffer;
    pDevice->CreateIndexBuffer(nSize, nUsage, nFormat, D3DPOOL_DEFAULT, &pBuffer);
    if (m_lstFreeIndexPools.m_nCount > 0)
    {
        unsigned long nPool = m_lstFreeIndexPools[m_lstFreeIndexPools.m_nCount - 1];
        m_lstFreeIndexPools.DelIndex(m_lstFreeIndexPools.m_nCount - 1);
        m_ppIndexPools[nPool] = pBuffer;
        m_pIndexPoolSize[nPool] = nSize;
        m_pIndexPoolState[nPool] = POOL_STATE_CREATED;
        *pnPool = nPool;
        return nPool;
    }

    unsigned char* pState = new unsigned char[m_nIndexPools * 2];
    memset(pState, 0, m_nIndexPools * 2);
    memcpy(pState, m_pIndexPoolState, m_nIndexPools);
    delete[] m_pIndexPoolState;
    m_pIndexPoolState = pState;
    m_pIndexPoolState[m_nIndexPools] = POOL_STATE_CREATED;

    unsigned long* pSize = new unsigned long[m_nIndexPools * 2];
    for (unsigned int i = 1; i < m_nIndexPools; i++)
    {
        m_lstFreeIndexPools.Add(m_nIndexPools + i);
    }
    memset(pSize, 0, m_nIndexPools * 2 * sizeof(unsigned long));
    memcpy(pSize, m_pIndexPoolSize, m_nIndexPools * sizeof(unsigned long));
    delete[] m_pIndexPoolSize;
    m_pIndexPoolSize = pSize;
    m_pIndexPoolSize[m_nIndexPools] = nSize;

    D3DIndexBuffer** ppPools = new D3DIndexBuffer*[m_nIndexPools * 2];
    memset(ppPools, 0, m_nIndexPools * 2 * sizeof(D3DIndexBuffer*));
    memcpy(ppPools, m_ppIndexPools, m_nIndexPools * sizeof(D3DIndexBuffer*));
    delete[] m_ppIndexPools;
    m_ppIndexPools = ppPools;
    m_ppIndexPools[m_nIndexPools] = pBuffer;

    *pnPool = m_nIndexPools;
    m_nIndexPools *= 2;
    return *pnPool;
}

unsigned long RequestPoolManager::RequestVertexPool(unsigned long nSize, unsigned long nUsage,
                                                    unsigned long nFVF, unsigned long* pnPool,
                                                    D3DDevice* pDevice)
{
    if (nSize == 0)
    {
        *pnPool = INVALID_POOL;
        return INVALID_POOL;
    }
    totalvertexpool += nSize;
    totalvertexpool4k += (nSize + POOL_PAGE_ROUNDING) & POOL_PAGE_MASK;

    D3DVertexBuffer* pBuffer;
    pDevice->CreateVertexBuffer(nSize, nUsage, nFVF, D3DPOOL_DEFAULT, &pBuffer);
    if (m_lstFreeVertexPools.m_nCount > 0)
    {
        unsigned long nPool = m_lstFreeVertexPools[m_lstFreeVertexPools.m_nCount - 1];
        m_ppVertexPools[nPool] = pBuffer;
        *pnPool = nPool | POOL_VERTEX_FLAG;
        m_pVertexPoolSize[nPool] = nSize;
        m_pVertexPoolState[nPool] = POOL_STATE_CREATED;
        m_lstFreeVertexPools.DelIndex(m_lstFreeVertexPools.m_nCount - 1);
        return *pnPool;
    }

    unsigned char* pState = new unsigned char[m_nVertexPools * 2];
    memset(pState, 0, m_nVertexPools * 2);
    memcpy(pState, m_pVertexPoolState, m_nVertexPools);
    delete[] m_pVertexPoolState;
    m_pVertexPoolState = pState;
    m_pVertexPoolState[m_nVertexPools] = POOL_STATE_CREATED;

    unsigned long* pSize = new unsigned long[m_nVertexPools * 2];
    for (unsigned int i = 1; i < m_nVertexPools; i++)
    {
        m_lstFreeVertexPools.Add(m_nVertexPools + i);
    }
    memcpy(pSize, m_pVertexPoolSize, m_nVertexPools * sizeof(unsigned long));
    delete[] m_pVertexPoolSize;
    m_pVertexPoolSize = pSize;
    m_pVertexPoolSize[m_nVertexPools] = nSize;

    D3DVertexBuffer** ppPools = new D3DVertexBuffer*[m_nVertexPools * 2];
    memset(ppPools, 0, m_nVertexPools * 2 * sizeof(D3DVertexBuffer*));
    memcpy(ppPools, m_ppVertexPools, m_nVertexPools * sizeof(D3DVertexBuffer*));
    delete[] m_ppVertexPools;
    m_ppVertexPools = ppPools;
    m_ppVertexPools[m_nVertexPools] = pBuffer;

    *pnPool = m_nVertexPools;
    m_nVertexPools *= 2;
    *pnPool |= POOL_VERTEX_FLAG;
    return *pnPool;
}

RequestPoolManager::RequestPoolManager() : m_lstFreeVertexPools(0), m_lstFreeIndexPools(0)
{
    unsigned int i;

    m_nVertexPools = POOL_INITIAL_COUNT;
    m_lstFreeVertexPools.SetSize(POOL_INITIAL_COUNT);
    for (i = 0; i < POOL_INITIAL_COUNT; i++)
    {
        m_lstFreeVertexPools[i] = i;
    }
    m_nIndexPools = POOL_INITIAL_COUNT;
    m_lstFreeIndexPools.SetSize(POOL_INITIAL_COUNT);
    for (i = 0; i < POOL_INITIAL_COUNT; i++)
    {
        m_lstFreeIndexPools[i] = i;
    }

    m_pVertexPoolSize = new unsigned long[m_nVertexPools];
    memset(m_pVertexPoolSize, 0, m_nVertexPools * sizeof(unsigned long));
    m_ppVertexPools = new D3DVertexBuffer*[m_nVertexPools];
    memset(m_ppVertexPools, 0, m_nVertexPools * sizeof(D3DVertexBuffer*));
    m_pIndexPoolSize = new unsigned long[m_nIndexPools];
    memset(m_pIndexPoolSize, 0, m_nIndexPools * sizeof(unsigned long));
    m_ppIndexPools = new D3DIndexBuffer*[m_nIndexPools];
    memset(m_ppIndexPools, 0, m_nIndexPools * sizeof(D3DIndexBuffer*));
    m_pVertexPoolState = new unsigned char[m_nVertexPools];
    m_pIndexPoolState = new unsigned char[m_nIndexPools];
    memset(m_pVertexPoolState, 0, m_nVertexPools);
    memset(m_pIndexPoolState, 0, m_nIndexPools);
}

RequestPoolManager* RequestPoolManager::GetInstance()
{
    static RequestPoolManager instance;
    return &instance;
}

void GLRender::DrawSphereBound(float fRadius, Vector& vCenter, const Vector& vColor, float fScale,
                               bool bBillboard)
{
    g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    DXCircle circle(g_pD3DDevice);
    PushCurrentMatrix();

    D3DXMATRIX mWorld = *g_pWorldMatrixStack->GetTop();
    D3DXMATRIX mView = *g_pViewMatrixStack->GetTop();
    D3DXMATRIX mMatrix;
    D3DXMatrixMultiply(&mMatrix, &mWorld, &mView);
    g_pWorldMatrixStack->LoadMatrix(&mMatrix);

    float* fMatrix = mMatrix;
    fMatrix[12] = vCenter.m_fX;
    fMatrix[13] = vCenter.m_fY;
    fMatrix[14] = vCenter.m_fZ;
    float fXY = fMatrix[1];
    fMatrix[1] = fMatrix[4];
    fMatrix[4] = fXY;
    float fXZ = fMatrix[2];
    fMatrix[2] = fMatrix[8];
    fMatrix[8] = fXZ;
    float fYZ = fMatrix[6];
    fMatrix[6] = fMatrix[9];
    fMatrix[9] = fYZ;
    float fInverseScale = 1.0f / fScale;
    for (int i = 0; i < 16; i++)
    {
        fMatrix[i] = fInverseScale * fMatrix[i];
    }
    g_nMatrixMode = 2;
    MultMatrix(fMatrix);
    float fSize = fRadius * fScale;
    Scale(fSize, fSize, fSize);

    g_nMatrixMode = 2;
    g_pD3DDevice->SetTransform(D3DTS_WORLD, g_pWorldMatrixStack->GetTop());
    g_nMatrixMode = 2;
    circle.Render(g_pD3DDevice);
    PopCurrentMatrix();
}

void GLRender::RenderBox(const Vector& vMinimum, const Vector& vMaximum, const Vector& vColor)
{
    static DXBox box(g_pD3DDevice);

    g_pD3DDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
    g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    g_nMatrixMode = 2;
    PushCurrentMatrix();
    Translate(vMinimum.m_fX, vMinimum.m_fY, vMinimum.m_fZ);
    Scale(vMaximum.m_fX - vMinimum.m_fX, vMaximum.m_fY - vMinimum.m_fY, vMaximum.m_fZ - vMinimum.m_fZ);
    int nMode = g_nMatrixMode;
    g_nMatrixMode = 2;
    g_pD3DDevice->SetTransform(D3DTS_WORLD, g_pWorldMatrixStack->GetTop());
    g_nMatrixMode = nMode;
    box.Render(g_pD3DDevice);
    g_pD3DDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
    g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
    PopCurrentMatrix();
}

void RequestPoolManager::UnlockPool(unsigned long nPool)
{
    if (nPool == INVALID_POOL)
    {
        return;
    }
    if (nPool & POOL_VERTEX_FLAG)
    {
        D3DVertexBuffer* pBuffer = m_ppVertexPools[nPool & POOL_INDEX_MASK];
        if (pBuffer)
        {
            pBuffer->Unlock();
        }
    }
}

unsigned long GLRender::RequestPool(Aurora::AuroraBufferRequestType nType, unsigned long nSize,
                                    unsigned long nUsage, unsigned long nPoolFlags,
                                    unsigned long* pnPool)
{
    static RequestPoolManager* pManager = RequestPoolManager::GetInstance();
    if (nType == Aurora::POOL_VERTEX)
    {
        pManager->RequestVertexPool(nSize, TranslatePoolUsage(nUsage), TranslatePoolFormat(nPoolFlags),
                                    pnPool, g_pD3DDevice);
    }
    else
    {
        pManager->RequestIndexPool(nSize, D3DUSAGE_DYNAMIC, D3DFMT_INDEX16, pnPool, g_pD3DDevice);
    }
    return *pnPool;
}

void* GLRender::LockPool(unsigned long nPool, unsigned long nOffset, unsigned long nSize,
                         unsigned long nCount)
{
    static RequestPoolManager* pManager = RequestPoolManager::GetInstance();
    return pManager->LockPool(nPool, nOffset, nSize, TranslatePoolLockFlags(nCount));
}

unsigned long GLRender::AddVertexPool(D3DVertexBuffer* pBuffer, unsigned long nSize)
{
    static RequestPoolManager* pManager = RequestPoolManager::GetInstance();
    return pManager->AddVertexPool(pBuffer, nSize);
}

unsigned long GLRender::AddIndexPool(D3DIndexBuffer* pBuffer, unsigned long nSize)
{
    static RequestPoolManager* pManager = RequestPoolManager::GetInstance();
    return pManager->AddIndexPool(pBuffer, nSize);
}

D3DResource* GLRender::GetPool(unsigned long nPool)
{
    static RequestPoolManager* pManager = RequestPoolManager::GetInstance();
    return pManager->GetPool(nPool);
}

void GLRender::UnlockPool(unsigned long nPool)
{
    static RequestPoolManager* pManager = RequestPoolManager::GetInstance();
    pManager->UnlockPool(nPool);
}

void GLRender::DeletePool(unsigned long nPool)
{
    if (nPool != RequestPoolManager::INVALID_POOL)
    {
        static RequestPoolManager* pManager = RequestPoolManager::GetInstance();
        pManager->DeletePool(nPool);
    }
}

void GLRender::ReleasePools()
{
    static RequestPoolManager* pManager = RequestPoolManager::GetInstance();
    pManager->ReleasePools();
}

void GLRender::ResizePool(unsigned long nPool, unsigned long nSize)
{
    static RequestPoolManager* pManager = RequestPoolManager::GetInstance();
    pManager->ResizePool(nPool, nSize, g_pD3DDevice);
}

void GLRender::RenderObjectsVTCQuadUPVP(RenderVertexVTC* pVertices, int nVertices)
{
    if (nVertices)
    {
        TrackMatrix();
        if (nVertices > MAX_QUAD_VERTICES)
        {
            nVertices = MAX_QUAD_VERTICES;
        }
        g_pD3DDevice->DrawPrimitiveUP(D3DPT_QUADLIST, nVertices / 4, pVertices, sizeof(RenderVertexVTC));
    }
}

void GLRender::CreateTexture(CAurTexture* pTexture, bool bSubImage, int nIndex, bool bLinear)
{
    int nHeight = pTexture->GetHeight();
    unsigned char* pImage = pTexture->GetImage();
    int nWidth = pTexture->GetWidth();
    int nBPP = pTexture->GetBPP();
    int nNumX = pTexture->GetNumX();
    int nNumY = pTexture->GetNumY();
    int bMipMap = pTexture->GetMipMap();
    int nClamp = pTexture->GetClamp();
    unsigned char bFilter = pTexture->GetFilter();
    int nSubWidth = nWidth / nNumX;
    int nSubHeight = nHeight / nNumY;
    int nX = nIndex % nNumX * nSubWidth;
    int nY = nIndex / nNumX * nSubHeight;
    unsigned char* pSubImage;
    if (nNumX == 1 && nNumY == 1)
        pSubImage = pImage;
    else
        pSubImage = ImageGetSubImage(pImage, nWidth, nHeight, nBPP, nX, nY,
                                    nSubWidth, nSubHeight);

    if (pTexture->IsBumpMap())
    {
        BuildTextureNormalMap(pSubImage, pTexture->GetBumpMapScaling(),
                              nSubWidth, nSubHeight, nBPP, bSubImage,
                              pTexture->IsNormalMap());
        if (!bSubImage)
            g_listTextureStates.m_pList[g_nCurrentTexture] = new DXTextureState;
        g_pCurrentTextureState = g_listTextureStates.m_pList[g_nCurrentTexture];
        g_pCurrentTextureState->m_nWrapS = nClamp & TEXTURE_CLAMP_S ? 2 : 3;
        g_pCurrentTextureState->m_nWrapT = g_pCurrentTextureState->m_nWrapS;
        int nMagFilter = bFilter ? 5 : 4;
        g_pCurrentTextureState->m_nMagFilter = nMagFilter;
        g_pCurrentTextureState->m_nMinFilter = nMagFilter;
        int nMipFilter;
        if (bMipMap)
            nMipFilter = bFilter ? 6 : 7;
        else
            nMipFilter = nMagFilter;
        g_pCurrentTextureState->m_nMipFilter = nMipFilter;
        return;
    }

    if (pTexture->GetCompressedSize() > 0)
    {
        int nLevel = 0;
        int nBlockSize = nBPP == 4 ? 16 : 8;
        unsigned char* pMipImage = pImage;
        int nCompressedSize = pTexture->GetCompressedSize();
        int nCompressionFormat = pTexture->GetCompressionFormat();
        CreateCompressedTexImage(0, bMipMap, 0, nCompressionFormat, nWidth, nHeight, 0, nCompressedSize, pImage);
        if (bMipMap)
        {
            int nSize = pTexture->GetCompressedSize();
            int nMipWidth = nWidth >> 1;
            int nMipHeight = nHeight >> 1;
            while (nMipWidth || nMipHeight)
            {
                if (nMipWidth == 0)
                    nMipWidth = 1;
                if (nMipHeight == 0)
                    nMipHeight = 1;
                nLevel++;
                pMipImage += nSize;
                nSize = ((nMipHeight + 3) / 4) * ((nMipWidth + 3) / 4) * nBlockSize;
                nCompressionFormat = pTexture->GetCompressionFormat();
                CreateCompressedTexImage(0, bMipMap, nLevel, nCompressionFormat, nMipWidth, nMipHeight, 0, nSize,
                                         pMipImage);
                nMipWidth >>= 1;
                nMipHeight >>= 1;
            }
        }
    }
    else
    {
        int nFormat = pTexture->GetTextureFormat();
        if (nSubWidth & (nSubWidth - 1))
            bLinear = true;
        else if (nSubHeight & (nSubHeight - 1))
            bLinear = true;
        if (bLinear)
        {
            if (nBPP == 4)
                nFormat = 10;
            else if (nBPP == 3)
                nFormat = 11;
        }
        if (bMipMap && !(nWidth & (nWidth - 1)) && !(nHeight & (nHeight - 1)))
            CreateMipMaps(0, nBPP, nSubWidth, nSubHeight, nFormat, pSubImage);
        else if (bSubImage)
            CreateTexSubImage(0, 0, nSubWidth, nSubHeight, nFormat, pSubImage);
        else
            CreateTextureImageRaw(0, 0, nBPP, nSubWidth, nSubHeight, 0, nFormat, pSubImage);
    }

    if (!bSubImage)
        g_listTextureStates.m_pList[g_nCurrentTexture] = new DXTextureState;
    g_pCurrentTextureState = g_listTextureStates.m_pList[g_nCurrentTexture];
    if (bLinear)
        g_pCurrentTextureState->m_nWrapS = 2;
    else
        g_pCurrentTextureState->m_nWrapS = nClamp & TEXTURE_CLAMP_S ? 2 : 3;
    g_pCurrentTextureState->m_nWrapT = g_pCurrentTextureState->m_nWrapS;
    int nMagFilter = bFilter ? 5 : 4;
    g_pCurrentTextureState->m_nMagFilter = nMagFilter;
    g_pCurrentTextureState->m_nMinFilter = nMagFilter;
    int nMipFilter;
    if (bMipMap)
        nMipFilter = bFilter ? 6 : 7;
    else
        nMipFilter = nMagFilter;
    g_pCurrentTextureState->m_nMipFilter = nMipFilter;
    SetTextureParameteri(0, 0, nClamp & TEXTURE_CLAMP_S ? 2 : 3);
    SetTextureParameteri(0, 1, nClamp & TEXTURE_CLAMP_T ? 2 : 3);
    SetTextureParameteri(0, 2, nMagFilter);
    int nMinFilter;
    if (bMipMap)
        nMinFilter = bFilter ? 6 : 7;
    else
        nMinFilter = nMagFilter;
    SetTextureParameteri(0, 3, nMinFilter);
    if (pSubImage != pImage)
        delete pSubImage;
}

#define AUR_REPLACE_CURRENT_TEXTURE(pTexture)     {         D3DBaseTexture* pOldTexture = g_listTextures.m_pList[g_nCurrentTexture];         if (pOldTexture)         {             int nOldType = g_listTextureStates.m_pList[g_nCurrentTexture]->m_nType;             if (nOldType == 1)             {                 g_pD3DDevice->SetTexture(0, NULL);                 g_pD3DDevice->SetTexture(1, NULL);                 g_pD3DDevice->SetTexture(2, NULL);                 g_pD3DDevice->SetTexture(3, NULL);                 delete pOldTexture;             }             else if (nOldType == 0)             {                 pOldTexture->Release();             }         }         g_listTextures.m_pList[g_nCurrentTexture] = (pTexture);         g_pCurrentTexture = (pTexture);         cm_pCurrentTexture[cm_uiCurrentStage] = (pTexture);     }

void GLRender::CreateProcessedTexture(CAurTexture* pTexture, bool bSubImage, int nIndex, bool bLinear)
{
    int nHeight = pTexture->GetHeight();
    unsigned char* pImage = pTexture->GetImage();
    int nWidth = pTexture->GetWidth();
    int nBPP = pTexture->GetBPP();
    int nNumX = pTexture->GetNumX();
    int nNumY = pTexture->GetNumY();
    int bMipMap = pTexture->GetMipMap();
    int nClamp = pTexture->GetClamp();
    unsigned char bFilter = pTexture->GetFilter();
    int nSubWidth = nWidth;
    int nSubHeight = nHeight;
    unsigned char* pSubImage;
    if (nNumX == 1 && nNumY == 1)
    {
        pSubImage = pImage;
    }
    else
    {
        if ((nWidth & (nWidth - 1)) || (nHeight & (nHeight - 1)))
            AurReportError(3717, "aurdxcore.cpp", "Bad cycle texture!  The subimages are not powers of 2");
        if (nNumX > 1 && nSubWidth != nSubHeight)
            AurReportError(3721, "aurdxcore.cpp",
                       "Bad cycle texture!  If numx > 1 then the subimages must be square!");
        pSubImage = pImage + GetCompressedContactSheetOffset(nIndex, nSubWidth, nBPP);
    }

    if (pTexture->IsBumpMap())
    {
        int nFormat = nBPP == 1 ? Aurora::TEXTURE_FORMAT_LUMINANCE : Aurora::TEXTURE_FORMAT_RGBA;
        int nType;
        if (nBPP == 1)
        {
            BuildTextureNormalMap(pSubImage, pTexture->GetBumpMapScaling(),
                                  nSubWidth, nSubHeight, nBPP, bSubImage,
                                  pTexture->IsNormalMap());
            nType = 2;
        }
        else if (bSubImage)
        {
            CreateTexSubImage(0, 0, nSubWidth, nSubHeight, nFormat, pSubImage);
            nType = 1;
        }
        else
        {
            D3DTexture* pNewTexture = new D3DTexture;
            XGSetTextureHeader(nSubWidth, nSubHeight, pTexture->GetNumMipMaps(), 0,
                               (D3DFORMAT)textureFormatTranslationTable[nFormat], 0, pNewTexture, 0, 0);
            pNewTexture->Register(pSubImage);
            AUR_REPLACE_CURRENT_TEXTURE(pNewTexture);
            nType = 1;
        }
        if (!bSubImage)
        {
            if (g_listTextureStates.m_pList[g_nCurrentTexture] == NULL)
                g_listTextureStates.m_pList[g_nCurrentTexture] = new DXTextureState;
        }
        g_pCurrentTextureState = g_listTextureStates.m_pList[g_nCurrentTexture];
        g_listTextureStates.m_pList[g_nCurrentTexture]->m_nType = nType;
        g_listTextureStates.m_pList[g_nCurrentTexture]->m_nUnknown0 = 4;
        g_listTextureStates.m_pList[g_nCurrentTexture]->m_nWrapS = nClamp & TEXTURE_CLAMP_S ? 2 : 3;
        g_listTextureStates.m_pList[g_nCurrentTexture]->m_nWrapT = g_pCurrentTextureState->m_nWrapS;
        int nMagFilter = bFilter ? 5 : 4;
        g_listTextureStates.m_pList[g_nCurrentTexture]->m_nMagFilter = nMagFilter;
        g_listTextureStates.m_pList[g_nCurrentTexture]->m_nMinFilter = nMagFilter;
        int nMipFilter;
        if (bMipMap)
            nMipFilter = bFilter ? 6 : 7;
        else
            nMipFilter = nMagFilter;
        g_listTextureStates.m_pList[g_nCurrentTexture]->m_nMipFilter = nMipFilter;
        return;
    }

    if (pTexture->GetCompressedSize() > 0)
    {
        int nFormat = pTexture->GetCompressionFormat();
        int nLevels = pTexture->GetNumMipMaps();
        D3DTexture* pNewTexture = new D3DTexture;
        XGSetTextureHeader(nSubWidth, nSubHeight, nLevels, 0,
                           (D3DFORMAT)textureFormatTranslationTable[nFormat], 0, pNewTexture, 0, 0);
        pNewTexture->Register(pSubImage);
        AUR_REPLACE_CURRENT_TEXTURE(pNewTexture);
    }
    else
    {
        int nFormat;
        if (nBPP == 4)
            nFormat = Aurora::TEXTURE_FORMAT_RGBA;
        else if (nBPP == 3)
            nFormat = Aurora::TEXTURE_FORMAT_RGB;
        else
            nFormat = nBPP == 2 ? Aurora::TEXTURE_FORMAT_LUMINANCE_ALPHA : Aurora::TEXTURE_FORMAT_LUMINANCE;
        if (bLinear)
        {
            if (nBPP == 4)
                nFormat = 10;
            else if (nBPP == 3)
                nFormat = 11;
        }
        if (bSubImage)
        {
            CreateTexSubImage(0, 0, nSubWidth, nSubHeight, nFormat, pSubImage);
        }
        else
        {
            D3DTexture* pNewTexture = new D3DTexture;
            XGSetTextureHeader(nSubWidth, nSubHeight, pTexture->GetNumMipMaps(), 0,
                               (D3DFORMAT)textureFormatTranslationTable[nFormat], 0, pNewTexture, 0, 0);
            pNewTexture->Register(pSubImage);
            AUR_REPLACE_CURRENT_TEXTURE(pNewTexture);
        }
    }

    if (g_listTextureStates.m_pList[g_nCurrentTexture] == NULL)
        g_listTextureStates.m_pList[g_nCurrentTexture] = new DXTextureState;
    g_pCurrentTextureState = g_listTextureStates.m_pList[g_nCurrentTexture];
    g_listTextureStates.m_pList[g_nCurrentTexture]->m_nType = 1;
    g_listTextureStates.m_pList[g_nCurrentTexture]->m_nUnknown0 = 4;
    int nWrapS = nClamp & TEXTURE_CLAMP_S ? 2 : 3;
    g_listTextureStates.m_pList[g_nCurrentTexture]->m_nWrapS = nWrapS;
    g_listTextureStates.m_pList[g_nCurrentTexture]->m_nWrapT = g_pCurrentTextureState->m_nWrapS;
    int nMagFilter = bFilter ? 5 : 4;
    g_listTextureStates.m_pList[g_nCurrentTexture]->m_nMagFilter = nMagFilter;
    g_listTextureStates.m_pList[g_nCurrentTexture]->m_nMinFilter = nMagFilter;
    int nMipFilter;
    if (bMipMap)
        nMipFilter = bFilter ? 6 : 7;
    else
        nMipFilter = nMagFilter;
    g_listTextureStates.m_pList[g_nCurrentTexture]->m_nMipFilter = nMipFilter;
    SetTextureParameteri(0, 0, nWrapS);
    SetTextureParameteri(0, 1, nClamp & TEXTURE_CLAMP_T ? 2 : 3);
    SetTextureParameteri(0, 2, nMagFilter);
    int nMinFilter;
    if (bMipMap)
        nMinFilter = bFilter ? 6 : 7;
    else
        nMinFilter = nMagFilter;
    SetTextureParameteri(0, 3, nMinFilter);
}

static unsigned int lookup;

void GLRender::MakeLookUpTexture(float fGamma)
{
    if (lookup == 0 || g_listTextures[lookup] == NULL)
    {
        unsigned char* pPixels = new unsigned char[256 * 256 * 2];
        unsigned char* pPixel = pPixels;
        for (int i = 0; i < 256; ++i)
        {
            unsigned char nGamma = (unsigned char)(pow(i * (1.0f / 255.0f), fGamma) * 255.99);
            for (int j = 0; j < 256; ++j)
            {
                double fValue = j * (1.0 / 255.0);
                *pPixel++ = (unsigned char)(fValue * 255.99);
                *pPixel++ = nGamma;
            }
        }
        GenTextures(1, &lookup);
        BindTexture(ENABLE_TEXTURE_2D, lookup);
        CreateMipMaps(0, 2, 256, 256, 2, pPixels);
        DXTextureState* pState = g_listTextureStates[lookup];
        pState->m_nWrapS = 1;
        pState->m_nWrapT = 1;
        pState->m_nMinFilter = 5;
        pState->m_nMipFilter = 6;
        pState->m_nMagFilter = 5;
        delete[] pPixels;
    }
}

void GLRender::DrawFromVerts(int nMode, List<Vector>& lstVertices)
{
    static RequestPoolManager* pManager = RequestPoolManager::GetInstance();
    static unsigned long nPool;
    static unsigned long nRequest =
        pManager->RequestVertexPool(lstVertices.m_nCount * sizeof(RenderVertexVC), D3DUSAGE_DYNAMIC,
                                    TranslatePoolFormat(RequestPoolManager::POOL_FORMAT_VC), &nPool,
                                    g_pD3DDevice);
    static int nPoolVertices = lstVertices.m_nCount;
    if (lstVertices.m_nCount == 0)
    {
        return;
    }

    D3DCOLORVALUE color = g_Material.Diffuse;
    unsigned long nColor = D3DCOLOR_COLORVALUE(color.r, color.g, color.b, color.a);
    g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

    RenderVertexVC* pVertices;
    int i;
    if (nMode != Aurora::PRIMITIVE_LINE_LOOP)
    {
        if (nPoolVertices < lstVertices.m_nCount)
        {
            pManager->ResizePool(nPool, lstVertices.m_nCount * sizeof(RenderVertexVC), g_pD3DDevice);
            nPoolVertices = lstVertices.m_nCount;
        }
        pVertices = (RenderVertexVC*)pManager->LockPool(nPool, 0, 0, 0);
        for (i = 0; i < lstVertices.m_nCount; i++)
        {
            pVertices[i].position[0] = lstVertices.m_pList[i].m_fX;
            pVertices[i].position[1] = lstVertices.m_pList[i].m_fY;
            pVertices[i].position[2] = lstVertices.m_pList[i].m_fZ;
            *(unsigned long*)pVertices[i].color = nColor;
        }
    }
    else
    {
        if (nPoolVertices < lstVertices.m_nCount + 1)
        {
            pManager->ResizePool(nPool, (lstVertices.m_nCount + 1) * sizeof(RenderVertexVC), g_pD3DDevice);
            nPoolVertices = lstVertices.m_nCount + 1;
        }
        pVertices = (RenderVertexVC*)pManager->LockPool(nPool, 0, 0, 0);
        for (i = 0; i < lstVertices.m_nCount; i++)
        {
            pVertices[i].position[0] = lstVertices.m_pList[i].m_fX;
            pVertices[i].position[1] = lstVertices.m_pList[i].m_fY;
            pVertices[i].position[2] = lstVertices.m_pList[i].m_fZ;
            *(unsigned long*)pVertices[i].color = nColor;
        }
        pVertices[lstVertices.m_nCount].position[0] = lstVertices.m_pList[0].m_fX;
        pVertices[lstVertices.m_nCount].position[1] = lstVertices.m_pList[0].m_fY;
        pVertices[lstVertices.m_nCount].position[2] = lstVertices.m_pList[0].m_fZ;
        *(unsigned long*)pVertices[lstVertices.m_nCount].color = nColor;
    }
    pManager->UnlockPool(nPool);
    D3DVertexBuffer* pBuffer = (D3DVertexBuffer*)pManager->GetPool(nPool);

    int nMatrixMode = g_nMatrixMode;
    g_nMatrixMode = 1;
    g_pD3DDevice->SetTransform(D3DTS_VIEW, g_pViewMatrixStack->GetTop());
    g_nMatrixMode = 2;
    g_pD3DDevice->SetTransform(D3DTS_WORLD, g_pWorldMatrixStack->GetTop());
    g_nMatrixMode = nMatrixMode;
    g_pD3DDevice->SetStreamSource(0, pBuffer, sizeof(RenderVertexVC));
    static unsigned long nShader = TranslatePoolFormat(RequestPoolManager::POOL_FORMAT_VC);
    g_pD3DDevice->SetVertexShader(nShader);

    if (nMode == Aurora::PRIMITIVE_TRIANGLE_STRIP)
    {
        g_pD3DDevice->DrawVertices(D3DPT_TRIANGLESTRIP, 0, lstVertices.m_nCount);
    }
    else if (nMode == Aurora::PRIMITIVE_QUADS)
    {
        for (i = 0; i < lstVertices.m_nCount; i += 4)
        {
            g_pD3DDevice->DrawVertices(D3DPT_TRIANGLEFAN, 0, 4);
        }
    }
    else if (nMode == Aurora::PRIMITIVE_POLYGON)
    {
        g_pD3DDevice->DrawVertices(D3DPT_TRIANGLEFAN, 0, lstVertices.m_nCount);
    }
    else if (nMode == Aurora::PRIMITIVE_LINE_LOOP)
    {
        g_pD3DDevice->DrawVertices(D3DPT_LINESTRIP, 0, lstVertices.m_nCount + 1);
    }
}

void GLRender::RenderObjectsVC(Aurora::AuroraPrimitiveTypes nPrimitive, unsigned long nPool, int nVertices)
{
    static RequestPoolManager* pManager = RequestPoolManager::GetInstance();
    int nPrimitives = GetPrimitiveCount(nPrimitive, nVertices);

    int nMode = g_nMatrixMode;
    g_nMatrixMode = 1;
    g_pD3DDevice->SetTransform(D3DTS_VIEW, g_pViewMatrixStack->GetTop());
    g_nMatrixMode = 2;
    g_pD3DDevice->SetTransform(D3DTS_WORLD, g_pWorldMatrixStack->GetTop());
    g_nMatrixMode = nMode;
    if (g_bMaterialDirty)
    {
        g_pD3DDevice->SetMaterial(&g_Material);
        g_bMaterialDirty = 0;
    }

    g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    g_pD3DDevice->SetStreamSource(0, (D3DVertexBuffer*)pManager->GetPool(nPool), sizeof(RenderVertexVC));
    static unsigned long nShader = TranslatePoolFormat(RequestPoolManager::POOL_FORMAT_VC);
    g_pD3DDevice->SetVertexShader(nShader);
    g_pD3DDevice->DrawPrimitive((D3DPRIMITIVETYPE)primTypeTranslationTable[nPrimitive], 0, nPrimitives);
}

int g_nStreamBufferIndex = -1;

void GLRender::SetInterleavedBuffer(unsigned long nStride, int nFlags, unsigned long nPool,
                                    int nVertexCount, MdlNodeTriMesh* pMesh, unsigned long nOffset)
{
    if (g_nStreamBufferIndex >= g_lstShaderSources.m_nCount - 1)
    {
        g_lstShaderSources.Add((char*)new D3DVertexBuffer);
    }
    g_nStreamBufferIndex++;
    D3DVertexBuffer* pBuffer = (D3DVertexBuffer*)g_lstShaderSources[g_nStreamBufferIndex];
    memset(pBuffer, 0, sizeof(D3DVertexBuffer));
    XGSetVertexBufferHeader(0, 0, 0, 0, pBuffer, GetPool(nPool)->Data + nOffset);
    g_pD3DDevice->SetStreamSource(0, pBuffer, nStride);
}

void GLRender::DrawElements(Aurora::AuroraPrimitiveTypes nPrimitive, unsigned long nCount,
                            unsigned long nOffset, unsigned long nPool)
{
    static RequestPoolManager* pManager = RequestPoolManager::GetInstance();

    if (usevertexprograms && g_bVertexProgramEnabled && g_bVertexProgramBound)
    {
        TrackMatrix();
        g_pD3DDevice->SetIndices((D3DIndexBuffer*)pManager->GetPool(nPool), 0);
        g_pD3DDevice->DrawIndexedPrimitive((D3DPRIMITIVETYPE)primTypeTranslationTable[nPrimitive], 0, 0,
                                           nOffset, GetPrimitiveCount(nPrimitive, nCount));
        return;
    }

    int nMode = g_nMatrixMode;
    g_nMatrixMode = 1;
    g_pD3DDevice->SetTransform(D3DTS_VIEW, g_pViewMatrixStack->GetTop());
    g_nMatrixMode = 2;
    g_pD3DDevice->SetTransform(D3DTS_WORLD, g_pWorldMatrixStack->GetTop());
    g_nMatrixMode = nMode;
    if (g_bMaterialDirty)
    {
        g_pD3DDevice->SetMaterial(&g_Material);
        g_bMaterialDirty = 0;
    }

    D3DIndexBuffer* pIndices = (D3DIndexBuffer*)pManager->GetPool(nPool);
    DWORD hShader = GetVertexShader(0);
    g_pD3DDevice->SetVertexShader(hShader);
    g_pD3DDevice->SetIndices(pIndices, 0);
    g_pD3DDevice->DrawIndexedPrimitive((D3DPRIMITIVETYPE)primTypeTranslationTable[nPrimitive], 0, 0,
                                       nOffset, GetPrimitiveCount(nPrimitive, nCount));
    if (g_bCustomVertexShader == 1)
    {
        g_pD3DDevice->SetVertexShader(0);
        g_pD3DDevice->DeleteVertexShader(hShader);
        g_bCustomVertexShader = 0;
    }
}

int g_bHighResolutionTimer = 1;
LARGE_INTEGER g_nTimerFrequency;
int g_bTimerScaled;
float g_fTimerScale;
__int64 g_nTimerDivisor;
__int64 g_nRenderStartTime;
__int64 AurGetHighResolutionTime();

void GLRender::InitRenderSystem()
{
    if (FAILED(D3DXCreateMatrixStack(0, &g_pWorldMatrixStack)))
        return;
    if (FAILED(D3DXCreateMatrixStack(0, &g_pViewMatrixStack)))
        return;
    if (FAILED(D3DXCreateMatrixStack(0, &g_pProjectionMatrixStack)))
        return;
    if (FAILED(D3DXCreateMatrixStack(0, &g_pTextureMatrixStack)))
        return;
    g_pWorldMatrixStack->LoadIdentity();
    g_pViewMatrixStack->LoadIdentity();
    g_pProjectionMatrixStack->LoadIdentity();
    g_pTextureMatrixStack->LoadIdentity();

    g_Material.Power = 0.0f;
    g_Material.Diffuse.r = 0.8f;
    g_Material.Diffuse.g = 0.8f;
    g_Material.Diffuse.b = 0.8f;
    g_Material.Diffuse.a = 1.0f;
    g_Material.Ambient.r = 0.2f;
    g_Material.Ambient.g = 0.2f;
    g_Material.Ambient.b = 0.2f;
    g_Material.Ambient.a = 1.0f;
    g_Material.Specular.r = 0.0f;
    g_Material.Specular.g = 0.0f;
    g_Material.Specular.b = 0.0f;
    g_Material.Specular.a = 1.0f;
    g_Material.Emissive.r = 0.0f;
    g_Material.Emissive.g = 0.0f;
    g_Material.Emissive.b = 0.0f;
    g_Material.Emissive.a = 1.0f;

    g_listTextureStates.SetSize(0);
    g_listTextures.SetSize(0);
    g_listVertexPrograms.SetSize(0);
    for (int i = 0; i < 16; i++)
    {
        g_listTextures.Add(NULL);
        g_listTextureStates.Add(NULL);
        g_listVertexPrograms.Add(NULL);
    }

    LARGE_INTEGER nFrequency;
    g_bHighResolutionTimer = QueryPerformanceFrequency(&nFrequency);
    if (g_bHighResolutionTimer == 1)
    {
        g_nTimerFrequency = nFrequency;
        g_bTimerScaled = 0;
        if ((unsigned __int64)nFrequency.QuadPart / 1000000 < 200)
        {
            g_bTimerScaled = 1;
            g_fTimerScale = (float)(1.0 / (nFrequency.QuadPart * 0.000001));
        }
        else
        {
            g_fTimerScale = (float)(1.0 / (nFrequency.QuadPart * 0.000001));
            g_nTimerDivisor = nFrequency.QuadPart / 1000000;
        }
    }
    g_nRenderStartTime = AurGetHighResolutionTime();
}

void GLRender::SetPoolBufferHeaders(unsigned long nPool, D3DVertexBuffer* pBuffer,
                                    D3DVertexBuffer* pOffsetBuffer, unsigned long nOffset)
{
    D3DResource* pPool = GetPool(nPool);
    XGSetVertexBufferHeader(0, 0, 0, 0, pBuffer, pPool->Data);
    XGSetVertexBufferHeader(0, 0, 0, 0, pOffsetBuffer, pPool->Data + nOffset);
}

static inline void UnbindCurrentTexture()
{
    g_pCurrentTextureState = NULL;
    g_pCurrentTexture = NULL;
    g_nCurrentTexture = 0;
    GLRender::cm_pCurrentTexture[GLRender::cm_uiCurrentStage] = NULL;
    g_pD3DDevice->SetTexture(GLRender::cm_uiCurrentStage, NULL);
    GLRender::SetTextureStageFlag(GLRender::cm_uiCurrentStage, 0);
}

void GLRender::DisableCombiners(CAurTexture* pTexture)
{
    if (tex1on)
    {
        SetActiveTexture(1);
        if (envmapon)
        {
            Disable(ENABLE_TEXTURE_CUBE_MAP);
            Disable(ENABLE_TEXTURE_GEN_S);
            Disable(ENABLE_TEXTURE_GEN_T);
            Disable(ENABLE_TEXTURE_GEN_R);
            Disable(ENABLE_TEXTURE_2D);
            envmapon = 0;
        }
        UnbindCurrentTexture();
        UnbindCurrentTexture();
        tex1on = 0;
    }
    SetActiveTexture(0);
    g_pD3DDevice->SetPixelShader(0);
}

int envmapon;
int tex1on;
unsigned long g_ahPixelShader[28];
bool g_bRendererShutDown;
int g_nLightBackUpDepth;
List<D3DLIGHT8> g_lstLightBackUp;
List<unsigned long> g_lstVisibilityTests;
List<unsigned long> g_lstLights;
int g_bInitializeLights = 1;
D3DVIEWPORT8 g_ViewPort;
List<char*> g_lstShaderSources;
List<char*> g_lstShaderBuffers;
LPDIRECT3D8 g_pD3D;
float g_fFrameBufferTime;

int visualizeregular = 1;
int visualizepass2 = 1;
int visualizepass3 = 1;
int visualizepass4 = 1;
int visualizepass5 = 1;
int visualizepass6 = 1;
int doframebuffer = 1;
int visualizepass1;
int dooldflares;

Vector distortionadd(0.005f, 0.005f, 0.006f);
unsigned int GLRender::cm_pHeatDistortionVertexProg;
bool GLRender::cm_bFirstFrameBufferFrame = true;

char* AllocateShadowMemory(int nSize);

extern unsigned int lookup;

void GLRender::EnablePixelShaders(int nShader, CAurTexture* pTexture0,
                               CAurTexture* pTexture1, CAurTexture* pTexture2,
                               CAurTexture* pTexture3)
{
    switch (nShader)
    {
        case 0:
            DisableCombiners(pTexture0);
            g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
            g_pD3DDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
            g_pD3DDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);
            g_pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
            g_pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
            g_pD3DDevice->SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
            g_pD3DDevice->SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
            g_pD3DDevice->SetTextureStageState(2, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
            g_pD3DDevice->SetTextureStageState(2, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
            g_pD3DDevice->SetTextureStageState(3, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
            g_pD3DDevice->SetTextureStageState(3, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
            g_pD3DDevice->SetTexture(1, NULL);
            g_pD3DDevice->SetTexture(2, NULL);
            g_pD3DDevice->SetTexture(3, NULL);
            g_pD3DDevice->SetTexture(0, NULL);
            SetTextureStageFlag(1, 0);
            SetTextureStageFlag(2, 0);
            SetTextureStageFlag(3, 0);
            SetTextureStageFlag(0, 0);
            g_pD3DDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE);
            return;

        case 1:
            g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
            SetActiveTexture(0);
            g_pD3DDevice->SetPixelShader(g_ahPixelShader[PIXEL_SHADER_TEXTURE]);
            return;

        case 2:
            g_pD3DDevice->SetPixelShader(g_ahPixelShader[PIXEL_SHADER_FOG]);
            g_pD3DDevice->SetPixelShaderConstant(0, Homogenize(currentscene->m_vFogColor, 0.0f), 1);
            return;

        case 3:
            SetActiveTexture(0);
            pTexture3->Bind();
            g_pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
            g_pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
            SetActiveTexture(1);
            g_pD3DDevice->SetTexture(1, cm_pAttenuationTexture);
            SetTextureStageFlag(cm_uiCurrentStage, 0);
            g_pD3DDevice->SetTextureStageState(1, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
            g_pD3DDevice->SetTextureStageState(1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
            g_pD3DDevice->SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
            g_pD3DDevice->SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
            SetActiveTexture(2);
            pTexture1->Bind();
            g_pD3DDevice->SetTextureStageState(2, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
            g_pD3DDevice->SetTextureStageState(2, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
            SetActiveTexture(3);
            pTexture2->Bind();
            g_pD3DDevice->SetTextureStageState(3, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
            g_pD3DDevice->SetTextureStageState(3, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
            g_pD3DDevice->SetPixelShader(g_ahPixelShader[PIXEL_SHADER_BUMP_LIGHT_MAP]);
            SetActiveTexture(0);
            return;

        case 17:
            envmapon = 1;
            tex1on = 1;
            SetActiveTexture(1);
            EnableClientState(0);
            pTexture1->Bind();
            g_pD3DDevice->SetTextureStageState(cm_uiCurrentStage, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3);
            g_pD3DDevice->SetTextureStageState(cm_uiCurrentStage, D3DTSS_TEXCOORDINDEX,
                                               cm_uiCurrentStage | D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR);
            g_pD3DDevice->SetPixelShader(g_ahPixelShader[PIXEL_SHADER_ENVIRONMENT]);
            return;

        case 18:
            envmapon = 1;
            tex1on = 1;
            SetActiveTexture(1);
            EnableClientState(0);
            pTexture1->Bind();
            g_pD3DDevice->SetPixelShader(g_ahPixelShader[PIXEL_SHADER_ENVIRONMENT]);
            return;

        case 19:
            envmapon = 1;
            tex1on = 1;
            SetActiveTexture(2);
            pTexture1->Bind();
            g_pD3DDevice->SetTextureStageState(cm_uiCurrentStage, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3);
            g_pD3DDevice->SetTextureStageState(cm_uiCurrentStage, D3DTSS_TEXCOORDINDEX,
                                               cm_uiCurrentStage | D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR);
            SetActiveTexture(1);
            pTexture2->Bind();
            g_pD3DDevice->SetPixelShader(g_ahPixelShader[PIXEL_SHADER_ENVIRONMENT_LIGHT_MAP]);
            return;

        case 33:
            cm_pCurrentTexture[1] = NULL;
            g_pD3DDevice->SetTexture(1, NULL);
            SetTextureStageFlag(1, 0);
            cm_pCurrentTexture[2] = NULL;
            g_pD3DDevice->SetTexture(2, NULL);
            SetTextureStageFlag(2, 0);
            SetActiveTexture(0);
            g_pD3DDevice->SetPixelShader(0);
            g_pD3DDevice->SetTextureStageState(2, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
            g_pD3DDevice->SetTextureStageState(2, D3DTSS_TEXCOORDINDEX, 2);
            tex1on = 0;
            return;

        case 22:
            SetActiveTexture(0);
            pTexture0->Bind();
            SetActiveTexture(1);
            pTexture1->Bind();
            tex1on = 1;
            g_pD3DDevice->SetPixelShader(g_ahPixelShader[PIXEL_SHADER_LIGHT_MAP]);
            SetActiveTexture(0);
            return;

        case 23:
            SetActiveTexture(1);
            pTexture1->Bind();
            tex1on = 1;
            g_pD3DDevice->SetPixelShader(g_ahPixelShader[PIXEL_SHADER_LIGHT_MAP_ONLY]);
            SetActiveTexture(0);
            return;

        case 14:
        case 15:
        case 16:
            tex1on = 1;
            g_pD3DDevice->SetPixelShaderConstant(4, Homogenize(Vector(0.0f, 0.0f, 1.0f), 0.0f), 1);
            g_pD3DDevice->SetPixelShaderConstant(7, Homogenize(Vector(0.0f, 0.0f, 0.0f), 0.0f), 1);
            g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
            SetActiveTexture(1);
            pTexture3->Bind();
            SetActiveTexture(0);
            pTexture1->Bind();
            g_pD3DDevice->SetPixelShader(g_ahPixelShader[PIXEL_SHADER_BUMP_SELF_SHADOW]);
            return;

        case 4:
        case 12:
        case 13:
            tex1on = 1;
            g_pD3DDevice->SetPixelShaderConstant(4, Homogenize(Vector(0.0f, 0.0f, 1.0f), 0.0f), 1);
            g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
            SetActiveTexture(1);
            pTexture1->Bind();
            SetActiveTexture(0);
            pTexture0->Bind();
            g_pD3DDevice->SetPixelShader(g_ahPixelShader[PIXEL_SHADER_BUMP_DIFFUSE]);
            return;

        case 5:
            tex1on = 1;
            SetActiveTexture(0);
            pTexture0->Bind();
            SetActiveTexture(1);
            pTexture1->Bind();
            g_pD3DDevice->SetTextureStageState(cm_uiCurrentStage, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
            g_pD3DDevice->SetTextureStageState(cm_uiCurrentStage, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
            SetActiveTexture(2);
            pTexture2->Bind();
            g_pD3DDevice->SetTextureStageState(cm_uiCurrentStage, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
            g_pD3DDevice->SetTextureStageState(cm_uiCurrentStage, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
            SetActiveTexture(3);
            pTexture3->Bind();
            g_pD3DDevice->SetPixelShader(g_ahPixelShader[PIXEL_SHADER_BUMP_SPECULAR_POWER]);
            g_pD3DDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
            return;

        case 28:
            SetActiveTexture(1);
            g_pD3DDevice->SetTexture(1, NULL);
            SetTextureStageFlag(1, 0);
            g_pD3DDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
            g_pD3DDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);
            cm_pCurrentTexture[1] = NULL;
            SetActiveTexture(0);
            tex1on = 0;
            g_pD3DDevice->SetPixelShader(0);
            return;

        case 11:
            g_pD3DDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE);
            cm_pCurrentTexture[1] = NULL;
            g_pD3DDevice->SetTexture(1, NULL);
            SetTextureStageFlag(1, 0);
            cm_pCurrentTexture[2] = NULL;
            g_pD3DDevice->SetTexture(2, NULL);
            SetTextureStageFlag(2, 0);
            cm_pCurrentTexture[3] = NULL;
            g_pD3DDevice->SetTexture(3, NULL);
            SetTextureStageFlag(3, 0);
            SetActiveTexture(0);
            g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
            g_pD3DDevice->SetPixelShader(0);
            tex1on = 0;
            return;

        case 20:
            g_pD3DDevice->SetPixelShader(g_ahPixelShader[PIXEL_SHADER_BUMP_ENVIRONMENT]);
            return;

        case 24:
            g_pD3DDevice->SetTextureStageState(3, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
            g_pD3DDevice->SetTextureStageState(3, D3DTSS_TEXCOORDINDEX, 3);
            return;

        case 21:
            g_pD3DDevice->SetPixelShader(g_ahPixelShader[PIXEL_SHADER_BUMP_SPECULAR]);
            return;

        case 25:
            SetActiveTexture(0);
            pTexture0->Bind();
            SetActiveTexture(3);
            pTexture3->Bind();
            g_pD3DDevice->SetTextureStageState(3, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3);
            g_pD3DDevice->SetTextureStageState(3, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR | 3);
            g_pD3DDevice->SetTextureStageState(cm_uiCurrentStage, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
            g_pD3DDevice->SetTextureStageState(cm_uiCurrentStage, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
            g_pD3DDevice->SetTextureStageState(cm_uiCurrentStage, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP);
            SetActiveTexture(0);
            return;

        case 26:
            MakeLookUpTexture(5.0f);
            SetActiveTexture(0);
            pTexture1->Bind();
            SetActiveTexture(1);
            SetActiveTexture(2);
            BindTexture(Aurora::ENABLE_TEXTURE_2D, lookup);
            SetActiveTexture(3);
            return;

        case 6:
            tex1on = 1;
            g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
            g_pD3DDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
            SetActiveTexture(0);
            pTexture0->Bind();
            SetActiveTexture(1);
            pTexture1->Bind();
            g_pD3DDevice->SetTextureStageState(cm_uiCurrentStage, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
            g_pD3DDevice->SetTextureStageState(cm_uiCurrentStage, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
            SetActiveTexture(2);
            pTexture2->Bind();
            SetActiveTexture(3);
            pTexture3->Bind();
            g_pD3DDevice->SetPixelShader(g_ahPixelShader[PIXEL_SHADER_NORMAL_MAP_MODULATE]);
            return;

        case 7:
            tex1on = 1;
            SetActiveTexture(0);
            pTexture0->Bind();
            SetActiveTexture(1);
            pTexture1->Bind();
            g_pD3DDevice->SetTextureStageState(cm_uiCurrentStage, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
            g_pD3DDevice->SetTextureStageState(cm_uiCurrentStage, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
            SetActiveTexture(2);
            pTexture2->Bind();
            SetActiveTexture(3);
            pTexture3->Bind();
            g_pD3DDevice->SetPixelShader(g_ahPixelShader[PIXEL_SHADER_NORMAL_MAP_LIGHT]);
            g_pD3DDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
            return;

        case 29:
            SetActiveTexture(0);
            pTexture0->Bind();
            SetActiveTexture(1);
            pTexture1->Bind();
            SetActiveTexture(2);
            pTexture1->Bind();
            SetActiveTexture(3);
            pTexture1->Bind();
            g_pD3DDevice->SetPixelShader(g_ahPixelShader[PIXEL_SHADER_DEPTH_SPRITE]);
            SetActiveTexture(0);
            return;

        case 30:
            g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
            SetActiveTexture(0);
            pTexture0->Bind();
            g_pD3DDevice->SetPixelShader(g_ahPixelShader[PIXEL_SHADER_TEXTURE_MODULATE]);
            return;

        case 31:
            g_pD3DDevice->SetPixelShader(g_ahPixelShader[PIXEL_SHADER_WHITE]);
            return;

        case 32:
            g_pD3DDevice->SetPixelShader(0);
            return;

        case 8:
        case 9:
        case 10:
        case 27:
            break;
    }
}

void GLRender::Shutdown(int bReleaseRenderer)
{
    if (bReleaseRenderer && g_bRendererShutDown != true)
    {
        int i;
        g_bRendererShutDown = true;
        for (i = 0; i < g_lstShaderSources.m_nCount; i++)
        {
            delete g_lstShaderSources.m_pList[i];
        }
        if (cm_pAttenuationTexture)
        {
            cm_pAttenuationTexture->Release();
            cm_pAttenuationTexture = NULL;
        }
        if (g_pWorldMatrixStack)
        {
            g_pWorldMatrixStack->Release();
            g_pWorldMatrixStack = NULL;
        }
        if (g_pViewMatrixStack)
        {
            g_pViewMatrixStack->Release();
            g_pViewMatrixStack = NULL;
        }
        if (g_pProjectionMatrixStack)
        {
            g_pProjectionMatrixStack->Release();
            g_pProjectionMatrixStack = NULL;
        }
        if (g_pTextureMatrixStack)
        {
            g_pTextureMatrixStack->Release();
            g_pTextureMatrixStack = NULL;
        }
        ReleasePools();
        for (i = 0; i < g_listTextures.m_nCount; i++)
        {
            if (g_listTextures.m_pList[i])
            {
                int nType = g_listTextureStates.m_pList[i]->m_nType;
                if (nType == 1)
                {
                    g_pD3DDevice->SetTexture(0, NULL);
                    g_pD3DDevice->SetTexture(1, NULL);
                    g_pD3DDevice->SetTexture(2, NULL);
                    g_pD3DDevice->SetTexture(3, NULL);
                    delete g_listTextures.m_pList[i];
                }
                else if (nType == 0)
                {
                    g_listTextures.m_pList[i]->Release();
                }
                g_listTextures.m_pList[i] = NULL;
            }
            if (g_listTextureStates.m_pList[i])
            {
                delete g_listTextureStates.m_pList[i];
                g_listTextureStates.m_pList[i] = NULL;
            }
        }
        for (i = 0; i < g_listVertexPrograms.m_nCount; i++)
        {
            if (g_listVertexPrograms.m_pList[i])
            {
                delete g_listVertexPrograms.m_pList[i];
            }
            g_listVertexPrograms.m_pList[i] = NULL;
        }
        if (g_pD3DDevice)
        {
            g_pD3DDevice->Release();
            g_pD3DDevice = NULL;
        }
        if (g_pD3D)
        {
            g_pD3D->Release();
            g_pD3D = NULL;
        }
        for (i = 0; i < g_lstShaderBuffers.m_nCount; i++)
        {
            delete g_lstShaderBuffers.m_pList[i];
        }
        g_lstLightBackUp.SetSize(0);
    }
}

void GLRender::AddSquareToRender(unsigned long nPool)
{
    if (nPool != (unsigned long)-1)
    {
        cm_listSquaresToRender.Add(nPool);
    }
}

void GLRender::RenderSquares(unsigned int nPool)
{
    static RequestPoolManager* pPoolManager = RequestPoolManager::GetInstance();
    D3DDevice_SetStreamSource(0, (D3DVertexBuffer*)pPoolManager->GetPool(nPool), 16);
    g_pD3DDevice->DrawPrimitive(D3DPT_LINESTRIP, 0, 4);
}

void GLRender::RenderDrawableSquares()
{
    g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    g_pD3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
    g_pD3DDevice->SetVertexShader(D3DFVF_XYZ | D3DFVF_DIFFUSE);
    for (int nStage = 0; nStage < 4; nStage++)
    {
        g_pD3DDevice->SetTexture(nStage, NULL);
        SetTextureStageFlag(nStage, 0);
    }
    g_pD3DDevice->SetTransform(D3DTS_WORLD, g_pWorldMatrixStack->GetTop());

    for (int i = 0; i < cm_listSquaresToRender.m_nCount; ++i)
    {
        RenderSquares(cm_listSquaresToRender.m_pList[i]);
    }

    g_pD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
}

void GLRender::BackUpLights()
{
    if (g_nLightBackUpDepth++ == 0)
    {
        static RequestPoolManager* pPoolManager = RequestPoolManager::GetInstance();
        if (g_pD3DDevice)
        {
            g_lstLightBackUp.m_nCount = 0;
            for (int i = 0; i < g_lstLights.m_nCount; i++)
            {
                D3DLIGHT8 light;
                g_pD3DDevice->GetLight(g_lstLights.m_pList[i], &light);
                g_lstLightBackUp.Add(light);
            }
        }
    }
}

void GLRender::RestoreLights()
{
    if (--g_nLightBackUpDepth == 0)
    {
        static RequestPoolManager* pPoolManager = RequestPoolManager::GetInstance();
        if (g_pD3DDevice)
        {
            RestoreVertexPrograms();
            InitializeVertexShaders();
            InitializePixelShaders();
            for (int i = 0; i < g_lstLights.m_nCount; i++)
            {
                g_pD3DDevice->SetLight(g_lstLights.m_pList[i], &g_lstLightBackUp.m_pList[i]);
                EnableLight(g_lstLights.m_pList[i]);
            }
            g_lstLightBackUp.SetSize(0);
        }
    }
}

void GLRender::ValidatePool(unsigned long nPool)
{
    static RequestPoolManager* pPoolManager = RequestPoolManager::GetInstance();
}

void GLRender::RenderToTexture(int nUnused, D3DTexture* pTarget, D3DBaseTexture* pSource,
                               int nWidth, int nHeight, int nTargetWidth, int nTargetHeight)
{
    static RequestPoolManager* pPoolManager = RequestPoolManager::GetInstance();
    D3DSurface* pSurface;
    pTarget->GetSurfaceLevel(0, &pSurface);
    g_pD3DDevice->SetRenderTarget(pSurface, NULL);
    pSurface->Release();
    g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0, 1.0f, 0);
    g_pD3DDevice->SetTexture(0, pSource);
    g_pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    SetTextureStageFlag(0, 0);
    g_pD3DDevice->Begin(D3DPT_QUADLIST);
    for (int i = 0; i < 4; i++)
    {
        g_pD3DDevice->SetVertexDataColor(D3DVSDE_DIFFUSE, 0xffffffff);
        g_pD3DDevice->SetVertexData2f(D3DVSDE_TEXCOORD0,
                                      (i == 0 || i == 1) ? 0.0f : (float)nWidth,
                                      (i == 0 || i == 3) ? 0.0f : (float)nHeight);
        g_pD3DDevice->SetVertexData4f(D3DVSDE_POSITION,
                                      (i == 0 || i == 1) ? -1.0f : 1.0f,
                                      (i == 0 || i == 3) ? 1.0f : -1.0f, 0.0f, 1.0f);
    }
    g_pD3DDevice->End();
}

void GLRender::CopyFrameBufferTexture(D3DBaseTexture* pSource, D3DTexture* pTarget)
{
    D3DSurface* pDepthStencil;
    g_pD3DDevice->GetDepthStencilSurface(&pDepthStencil);
    if (g_nFrameBufferMode == 0)
    {
        RenderToTexture(0, pTarget, pSource, g_nScreenWidth, g_nScreenHeight, 64, 64);
    }
    else if (g_nFrameBufferMode == 2)
    {
        RenderToTexture(0, pTarget, pSource, g_nScreenWidth, g_nScreenHeight * 2, 64, 64);
    }
    D3DSurface* pBackBuffer;
    g_pD3DDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
    g_pD3DDevice->SetRenderTarget(pBackBuffer, pDepthStencil);
    pBackBuffer->Release();
    pDepthStencil->Release();
}

static char g_szHeatDistortionVertexProg[] =
    "vs.1.1 \n"
    "mov oPos, v0;\n"
    "mov r0, v9;\n"
    "mov oT0, r0;\n"
    "mov r1, v10;\n"
    "mov r2, v11;\n"
    "mov r3.x, r1.x;\n"
    "mov r3.y, c[15].x;\n"
    "mov r3.z, r2.x;\n"
    "mov r4.x, c[15].x;\n"
    "mov r4.y, r1.y;\n"
    "mov r4.z, r2.y;\n"
    "mov oT1.xyz, r3.xyz;\n"
    "mov oT2.xyz, r4.xyz;\n"
    "mov oD0.xyzw, c[15].yyyy;\n";
static unsigned long g_adwHeatDistortionDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT3),
    D3DVSD_REG(D3DVSDE_DIFFUSE, D3DVSDT_D3DCOLOR),
    D3DVSD_REG(D3DVSDE_TEXCOORD0, D3DVSDT_FLOAT2),
    D3DVSD_REG(D3DVSDE_TEXCOORD1, D3DVSDT_FLOAT2),
    D3DVSD_REG(D3DVSDE_TEXCOORD2, D3DVSDT_FLOAT2),
    D3DVSD_END()
};

void GLRender::RenderForceDistortion(unsigned long nTexture)
{
    float fParameter[4] = { 0.0f, 1.0f, 0.0f, 0.0f };
    g_pD3DDevice->SetVertexShaderConstant(DISTORTION_ENV_PARAMETER, fParameter, 1);
    if (!IsProgramNV(cm_pHeatDistortionVertexProg))
    {
        GenPrograms(1, &cm_pHeatDistortionVertexProg);
        DrawElementsInShadow(cm_pHeatDistortionVertexProg, strlen(g_szHeatDistortionVertexProg),
                             g_szHeatDistortionVertexProg, g_adwHeatDistortionDeclaration);
    }
    BindVertexProgram(cm_pHeatDistortionVertexProg);
    float fSize = (float)cm_pForceDistortionTexture->GetWidth();
    SetBlendFunc(Aurora::BLEND_ONE, Aurora::BLEND_ZERO);
    SetActiveTexture(0);
    cm_pForceDistortionTexture->Bind();
    g_pD3DDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
    g_pD3DDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
    g_pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    SetActiveTexture(2);
    g_pD3DDevice->SetTexture(2, (D3DBaseTexture*)nTexture);
    g_pD3DDevice->SetTextureStageState(2, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
    g_pD3DDevice->SetTextureStageState(2, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
    g_pD3DDevice->SetTextureStageState(2, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetTextureStageState(2, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetPixelShader(g_ahPixelShader[PIXEL_SHADER_DISTORTION]);
    g_pD3DDevice->SetPixelShaderConstant(0, Homogenize(distortionadd, 1.0f), 1);
    Enable(Aurora::ENABLE_STENCIL_TEST);
    g_pD3DDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_LESS);
    g_pD3DDevice->SetRenderState(D3DRS_STENCILREF, 0);
    g_pD3DDevice->SetRenderState(D3DRS_STENCILMASK, DISTORTION_STENCIL_MASK);
    SetStencilOp(STENCIL_KEEP, STENCIL_KEEP, STENCIL_KEEP);
    Disable(Aurora::ENABLE_FOG);
    g_pD3DDevice->Begin(D3DPT_QUADLIST);
    for (int i = 0; i < 4; i++)
    {
        g_pD3DDevice->SetVertexDataColor(D3DVSDE_DIFFUSE, 0xffffffff);
        float t = (i == 0 || i == 3) ? 0.0f : fSize;
        float s = (i == 0 || i == 1) ? 0.0f : fSize;
        g_pD3DDevice->SetVertexData2f(D3DVSDE_TEXCOORD0, s, t);
        g_pD3DDevice->SetVertexData2f(D3DVSDE_TEXCOORD1, 64.0f, 48.0f);
        if (g_nFrameBufferMode == 0)
        {
            float fScreenT = (i == 0 || i == 3) ? 0.0f : 480.0f;
            float fScreenS = (i == 0 || i == 1) ? 0.0f : 640.0f;
            g_pD3DDevice->SetVertexData2f(D3DVSDE_TEXCOORD2, fScreenS, fScreenT);
        }
        else
        {
            float fScreenT = (i == 0 || i == 3) ? 0.0f : 960.0f;
            float fScreenS = (i == 0 || i == 1) ? 0.0f : 640.0f;
            g_pD3DDevice->SetVertexData2f(D3DVSDE_TEXCOORD2, fScreenS, fScreenT);
        }
        float y = (i == 0 || i == 3) ? 1.0f : -1.0f;
        float x = (i == 0 || i == 1) ? -1.0f : 1.0f;
        g_pD3DDevice->SetVertexData4f(D3DVSDE_POSITION, x, y, 0.0f, 1.0f);
    }
    g_pD3DDevice->End();
    Disable(Aurora::ENABLE_STENCIL_TEST);
    g_pD3DDevice->SetPixelShader(0);
    g_pD3DDevice->SetVertexShader(0);
    SetActiveTexture(0);
    g_pD3DDevice->SetTexture(0, NULL);
    g_pD3DDevice->SetTexture(1, NULL);
    g_pD3DDevice->SetTexture(2, NULL);
    g_pD3DDevice->SetTexture(3, NULL);
    SetTextureStageFlag(0, 0);
    SetTextureStageFlag(1, 0);
    SetTextureStageFlag(2, 0);
    SetTextureStageFlag(3, 0);
}

void GLRender::ResetViewPort(int nX, int nY, int nWidth, int nHeight)
{
    static RequestPoolManager* pPoolManager = RequestPoolManager::GetInstance();
    if (g_pD3DDevice)
    {
        if (g_bInitializeLights)
        {
            D3DXVECTOR3 vDirection(0.0f, 0.0f, 0.0f);
            D3DCOLORVALUE color;
            int i = 0;
            color.a = 1.0f;
            for (; i < 8; i++)
            {
                g_lstLights.Add(i);
                D3DLIGHT8 light;
                memset(&light, 0, sizeof(light));
                light.Type = D3DLIGHT_POINT;
                light.Range = 100000.0f;
                light.Attenuation0 = 1.0f;
                light.Direction = vDirection;
                if (i == 0)
                {
                    color.r = 1.0f;
                    color.g = 1.0f;
                    color.b = 1.0f;
                }
                else
                {
                    color.r = 0.0f;
                    color.g = 0.0f;
                    color.b = 0.0f;
                }
                light.Diffuse = color;
                color.r = 0.0f;
                color.g = 0.0f;
                color.b = 0.0f;
                light.Ambient = color;
                g_pD3DDevice->SetLight(i, &light);
            }
            g_bInitializeLights = 0;
        }
        BackUpLights();
        D3DVIEWPORT8 viewport;
        memset(&viewport, 0, sizeof(viewport));
        viewport.X = nX;
        viewport.Y = nY;
        viewport.Width = nWidth;
        viewport.Height = nHeight;
        viewport.MinZ = 0.0f;
        viewport.MaxZ = 1.0f;
        g_ViewPort = viewport;
        g_pD3DDevice->SetViewport(&viewport);
        RestoreLights();
        g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
        g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
        g_pD3DDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
        g_pD3DDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
        g_lstViewPorts.SetSize(0);
        g_lstViewPorts.Add(viewport);
    }
}

struct RenderVertexVNC
{
    D3DXVECTOR3 position;
    D3DXVECTOR3 normal;
    D3DCOLOR color;
};

void GLRender::DrawTriangles(int nFaces, List<MaxFace>& lFaces, unsigned long nVertexPool,
                             int nVertexStride, int nPositionOffset, short nVertexCount,
                             unsigned long nColorPool, int nPoolOffset)
{
    static RequestPoolManager* pPoolManager = RequestPoolManager::GetInstance();
    static unsigned long nPool = RequestPool(Aurora::POOL_VERTEX, lFaces.m_nCount * 3 * sizeof(RenderVertexVNC),
                                             RequestPoolManager::POOL_FLARE_USAGE, 0x13, &nPool);
    static int nPoolVertices = lFaces.m_nCount * 3;
    if (lFaces.m_nCount * 3 > nPoolVertices)
    {
        ResizePool(nPool, lFaces.m_nCount * 3 * sizeof(RenderVertexVNC));
        nPoolVertices = lFaces.m_nCount * 3;
    }
    RenderVertexVNC* pTriangles = (RenderVertexVNC*)pPoolManager->LockPool(nPool, 0, 0, 0);
    unsigned long* pColors = (unsigned long*)pPoolManager->LockPool(nColorPool, 0, 0, D3DLOCK_READONLY);
    char* pVertices = (char*)pPoolManager->LockPool(nVertexPool, 0, 0, D3DLOCK_READONLY) + nPoolOffset;
    for (int i = 0; i < lFaces.m_nCount; ++i)
    {
        Vector avVertex[3];
        for (int j = 0; j < 3; ++j)
        {
            RenderVertexVNC* pVertex = &pTriangles[i * 3 + j];
            D3DXVECTOR3* pPosition = (D3DXVECTOR3*)(pVertices +
                lFaces.m_pList[i].m_anVertexIndices[j] * nVertexStride + nPositionOffset);
            pVertex->position.x = pPosition->x;
            pVertex->position.y = pPosition->y;
            pVertex->position.z = pPosition->z;
            pVertex->color = pColors[lFaces.m_pList[i].m_nSurfaceType];
            avVertex[j] = Vector(pVertex->position.x, pVertex->position.y,
                                 pVertex->position.z);
        }
        Vector vNormal = NormalOf(avVertex[0], avVertex[1], avVertex[2]);
        for (int k = 0; k < 3; ++k)
        {
            pTriangles[i * 3 + k].normal.x = vNormal.m_fX;
            pTriangles[i * 3 + k].normal.y = vNormal.m_fY;
            pTriangles[i * 3 + k].normal.z = vNormal.m_fZ;
        }
    }
    pPoolManager->UnlockPool(nColorPool);
    pPoolManager->UnlockPool(nPool);
    pPoolManager->UnlockPool(nVertexPool);
    int nMatrixMode = g_nMatrixMode;
    g_nMatrixMode = 1;
    g_pD3DDevice->SetTransform(D3DTS_VIEW, g_pViewMatrixStack->GetTop());
    g_nMatrixMode = 2;
    g_pD3DDevice->SetTransform(D3DTS_WORLD, g_pWorldMatrixStack->GetTop());
    g_nMatrixMode = nMatrixMode;
    D3DDevice_SetStreamSource(0, (D3DVertexBuffer*)pPoolManager->GetPool(nPool), sizeof(RenderVertexVNC));
    static unsigned long hShader = TranslatePoolFormat(0x13);
    g_pD3DDevice->SetVertexShader(hShader);
    g_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, lFaces.m_nCount);
}

void GLRender::RemoveSquareToRender(unsigned long nPool)
{
    if (nPool != (unsigned long)-1)
    {
        if (cm_listSquaresToRender.Contains(nPool))
        {
            cm_listSquaresToRender.Remove(nPool);
            DeletePool(nPool);
        }
    }
}

void GLRender::RemoveAllSquaresToRender()
{
    int i = cm_listSquaresToRender.m_nCount;
    while (i != 0)
    {
        RemoveSquareToRender(cm_listSquaresToRender.m_pList[--i]);
    }
}

struct ConvolutionVertex
{
    float x, y, z;
    D3DCOLOR color;
    float s0, t0;
    float s1, t1;
    float s2, t2;
    float s3, t3;
};

static const DWORD dwDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(D3DVSDE_POSITION, D3DVSDT_FLOAT3),
    D3DVSD_REG(D3DVSDE_DIFFUSE, D3DVSDT_D3DCOLOR),
    D3DVSD_REG(D3DVSDE_TEXCOORD0, D3DVSDT_FLOAT2),
    D3DVSD_REG(D3DVSDE_TEXCOORD1, D3DVSDT_FLOAT2),
    D3DVSD_REG(D3DVSDE_TEXCOORD2, D3DVSDT_FLOAT2),
    D3DVSD_REG(D3DVSDE_TEXCOORD3, D3DVSDT_FLOAT2),
    D3DVSD_END()
};

void GLRender::RenderOverlayConvolutionPyramid(D3DTexture* pTexture)
{
    static RequestPoolManager* pPoolManager = RequestPoolManager::GetInstance();
    static unsigned long nPool = RequestPool(Aurora::POOL_VERTEX, 8 * sizeof(ConvolutionVertex),
                                             RequestPoolManager::POOL_COLOR, 0xf011, &nPool);
    static int bSetUp;
    static D3DTexture* pRedTexture;
    static D3DTexture* pGreenTexture;
    static D3DTexture* pBlueTexture;
    static D3DTexture* apTextures[7];
    int i;
    D3DSurface* pSurface;

    if (!bSetUp)
    {
        D3DLOCKED_RECT lock;
        g_pD3DDevice->CreateTexture(1, FRAMEBUFFER_LOOKUP_SIZE, 1, 0,
                                    D3DFMT_A8R8G8B8, 0, &pRedTexture);
        g_pD3DDevice->CreateTexture(FRAMEBUFFER_LOOKUP_SIZE, 1, 1, 0,
                                    D3DFMT_A8R8G8B8, 0, &pGreenTexture);
        g_pD3DDevice->CreateTexture(1, FRAMEBUFFER_LOOKUP_SIZE, 1, 0,
                                    D3DFMT_A8R8G8B8, 0, &pBlueTexture);

        pRedTexture->LockRect(0, &lock, NULL, 0);
        DWORD* pBits = (DWORD*)lock.pBits;
        for (i = 0; i < FRAMEBUFFER_LOOKUP_SIZE; i++)
        {
            if (i < FRAMEBUFFER_RED_THRESHOLD)
            {
                pBits[(lock.Pitch >> 2) * i] = 0;
            }
            else
            {
                float fLookup = (i - FRAMEBUFFER_RED_THRESHOLD) *
                    (1.0f / (FRAMEBUFFER_LOOKUP_SIZE - FRAMEBUFFER_RED_THRESHOLD - 1));
                pBits[(lock.Pitch >> 2) * i] =
                    (unsigned int)(fLookup * (AUR_FRAMEBUFFER_CHANNEL_MAX * AUR_FRAMEBUFFER_LOOKUP_GAIN))
                    << FRAMEBUFFER_BLUE_SHIFT;
            }
        }
        XGSwizzleRect(pBits, 0, NULL, lock.pBits, 1, FRAMEBUFFER_LOOKUP_SIZE,
                      NULL, sizeof(DWORD));

        pGreenTexture->LockRect(0, &lock, NULL, 0);
        pBits = (DWORD*)lock.pBits;
        for (i = 0; i < FRAMEBUFFER_LOOKUP_SIZE; i++)
        {
            if (i < FRAMEBUFFER_GREEN_THRESHOLD)
            {
                pBits[i] = 0;
            }
            else
            {
                float fLookup = (i - FRAMEBUFFER_GREEN_THRESHOLD) *
                    (1.0f / (FRAMEBUFFER_LOOKUP_SIZE - FRAMEBUFFER_GREEN_THRESHOLD - 1));
                pBits[i] =
                    (unsigned int)(fLookup * (AUR_FRAMEBUFFER_CHANNEL_MAX * AUR_FRAMEBUFFER_LOOKUP_GAIN))
                    << FRAMEBUFFER_GREEN_SHIFT;
            }
        }
        XGSwizzleRect(pBits, 0, NULL, lock.pBits, FRAMEBUFFER_LOOKUP_SIZE, 1,
                      NULL, sizeof(DWORD));

        pBlueTexture->LockRect(0, &lock, NULL, 0);
        pBits = (DWORD*)lock.pBits;
        for (i = 0; i < FRAMEBUFFER_LOOKUP_SIZE; i++)
        {
            if (i < FRAMEBUFFER_BLUE_THRESHOLD)
            {
                pBits[(lock.Pitch >> 2) * i] = 0;
            }
            else
            {
                float fLookup = (i - FRAMEBUFFER_BLUE_THRESHOLD) *
                    (1.0f / (FRAMEBUFFER_LOOKUP_SIZE - FRAMEBUFFER_BLUE_THRESHOLD - 1));
                pBits[(lock.Pitch >> 2) * i] =
                    (unsigned int)(fLookup * (AUR_FRAMEBUFFER_CHANNEL_MAX * AUR_FRAMEBUFFER_LOOKUP_GAIN));
            }
        }
        XGSwizzleRect(pBits, 0, NULL, lock.pBits, 1, FRAMEBUFFER_LOOKUP_SIZE,
                      NULL, sizeof(DWORD));

        g_pD3DDevice->CreateVertexShader(dwDeclaration, NULL,
                                         &g_hConvolutionVertexShader, 0);
        bSetUp = 1;
        g_pD3DDevice->CreateTexture(256, 256, 1, 0, D3DFMT_LIN_A8R8G8B8, 0, &apTextures[0]);
        g_pD3DDevice->CreateTexture(128, 128, 1, 0, D3DFMT_LIN_A8R8G8B8, 0, &apTextures[1]);
        g_pD3DDevice->CreateTexture(64, 64, 1, 0, D3DFMT_LIN_A8R8G8B8, 0, &apTextures[2]);
        g_pD3DDevice->CreateTexture(32, 32, 1, 0, D3DFMT_LIN_A8R8G8B8, 0, &apTextures[3]);
        g_pD3DDevice->CreateTexture(16, 16, 1, 0, D3DFMT_LIN_A8R8G8B8, 0, &apTextures[4]);
        g_pD3DDevice->CreateTexture(8, 8, 1, 0, D3DFMT_LIN_A8R8G8B8, 0, &apTextures[5]);
        g_pD3DDevice->CreateTexture(4, 4, 1, 0, D3DFMT_LIN_A8R8G8B8, 0, &apTextures[6]);

        ConvolutionVertex* pVertices = (ConvolutionVertex*)LockPool(nPool, 0, 0, 1);
        for (i = 0; i < 4; i++)
        {
            pVertices[i].color = 0xffffffff;
            pVertices[i].x = (i == 0 || i == 1) ? -1.0f : 1.0f;
            pVertices[i].y = (i == 0 || i == 3) ? 1.0f : -1.0f;
            pVertices[i].z = 0.0f;
            pVertices[i].s0 = (i == 0 || i == 1) ? 1.0f : 257.0f;
            pVertices[i].t0 = (i == 0 || i == 3) ? 1.0f : 257.0f;
            pVertices[i].s1 = (i == 0 || i == 1) ? 1.0f : 129.0f;
            pVertices[i].t1 = (i == 0 || i == 3) ? 1.0f : 129.0f;
            pVertices[i].s2 = (i == 0 || i == 1) ? 1.0f : 65.0f;
            pVertices[i].t2 = (i == 0 || i == 3) ? 1.0f : 65.0f;
            pVertices[i].s3 = (i == 0 || i == 1) ? 1.0f : 33.0f;
            pVertices[i].t3 = (i == 0 || i == 3) ? 1.0f : 33.0f;
        }
        ConvolutionVertex* pSecondQuad = &pVertices[4];
        for (i = 0; i < 4; i++)
        {
            pSecondQuad[i].color = 0xffffffff;
            pSecondQuad[i].x = (i == 0 || i == 1) ? -1.0f : 1.0f;
            pSecondQuad[i].y = (i == 0 || i == 3) ? 1.0f : -1.0f;
            pSecondQuad[i].z = 0.0f;
            pSecondQuad[i].s0 = (i == 0 || i == 1) ? 1.0f : 17.0f;
            pSecondQuad[i].t0 = (i == 0 || i == 3) ? 1.0f : 17.0f;
            pSecondQuad[i].s1 = (i == 0 || i == 1) ? 1.0f : 9.0f;
            pSecondQuad[i].t1 = (i == 0 || i == 3) ? 1.0f : 9.0f;
            pSecondQuad[i].s2 = (i == 0 || i == 1) ? 1.0f : 5.0f;
            pSecondQuad[i].t2 = (i == 0 || i == 3) ? 1.0f : 5.0f;
            pSecondQuad[i].s3 = (i == 0 || i == 1) ? 1.0f : 3.0f;
            pSecondQuad[i].t3 = (i == 0 || i == 3) ? 1.0f : 3.0f;
        }
        UnlockPool(nPool);
    }

    D3DSurface* pDepthStencil;
    g_pD3DDevice->GetDepthStencilSurface(&pDepthStencil);

    apTextures[0]->GetSurfaceLevel(0, &pSurface);
    g_pD3DDevice->SetRenderTarget(pSurface, NULL);
    pSurface->Release();
    g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0, 1.0f, 0);
    g_pD3DDevice->SetTexture(0, pTexture);
    g_pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetTexture(1, pRedTexture);
    g_pD3DDevice->SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetTexture(2, pGreenTexture);
    g_pD3DDevice->SetTextureStageState(2, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetTextureStageState(2, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetTexture(3, pBlueTexture);
    g_pD3DDevice->SetTextureStageState(3, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetTextureStageState(3, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetPixelShader(g_ahPixelShader[PIXEL_SHADER_COLOR_LOOKUP]);
    g_pD3DDevice->Begin(D3DPT_QUADLIST);
    for (i = 0; i < 4; i++)
    {
        if (g_nFrameBufferMode == 0)
        {
            g_pD3DDevice->SetVertexDataColor(D3DVSDE_DIFFUSE, 0xffffffff);
            float t = (i == 0 || i == 3) ? 0.0f : (float)g_nScreenHeight;
            float s = (i == 0 || i == 1) ? 0.0f : (float)g_nScreenWidth;
            g_pD3DDevice->SetVertexData2f(D3DVSDE_TEXCOORD0, s, t);
            float y = (i == 0 || i == 3) ? 1.0f : -1.0f;
            float x = (i == 0 || i == 1) ? -1.0f : 1.0f;
            g_pD3DDevice->SetVertexData4f(D3DVSDE_POSITION, x, y, 0.0f, 1.0f);
        }
        else if (g_nFrameBufferMode == 2)
        {
            g_pD3DDevice->SetVertexDataColor(D3DVSDE_DIFFUSE, 0xffffffff);
            float t = (i == 0 || i == 3) ? 0.0f : (float)(g_nScreenHeight * 2);
            float s = (i == 0 || i == 1) ? 0.0f : (float)g_nScreenWidth;
            g_pD3DDevice->SetVertexData2f(D3DVSDE_TEXCOORD0, s, t);
            float y = (i == 0 || i == 3) ? 1.0f : -1.0f;
            float x = (i == 0 || i == 1) ? -1.0f : 1.0f;
            g_pD3DDevice->SetVertexData4f(D3DVSDE_POSITION, x, y, 0.0f, 1.0f);
        }
    }
    g_pD3DDevice->End();

    g_pD3DDevice->SetPixelShader(g_ahPixelShader[PIXEL_SHADER_TEXTURE]);
    RenderToTexture(4, apTextures[1], apTextures[0], 256, 256, 128, 128);
    RenderToTexture(8, apTextures[2], apTextures[1], 128, 128, 64, 64);
    RenderToTexture(12, apTextures[3], apTextures[2], 64, 64, 32, 32);
    RenderToTexture(16, apTextures[4], apTextures[3], 32, 32, 16, 16);
    RenderToTexture(20, apTextures[5], apTextures[4], 16, 16, 8, 8);
    RenderToTexture(24, apTextures[6], apTextures[5], 8, 8, 4, 4);

    D3DSurface* pBackBuffer;
    g_pD3DDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
    g_pD3DDevice->SetRenderTarget(pBackBuffer, pDepthStencil);
    SetBlendFunc(Aurora::BLEND_SRC_ALPHA, Aurora::BLEND_ONE);
    g_pD3DDevice->SetVertexShader(g_hConvolutionVertexShader);
    g_pD3DDevice->SetPixelShader(g_ahPixelShader[PIXEL_SHADER_FOUR_TEXTURE_ADD]);
    D3DDevice_SetStreamSource(0, (D3DVertexBuffer*)pPoolManager->GetPool(nPool), sizeof(ConvolutionVertex));
    g_pD3DDevice->SetTexture(0, apTextures[0]);
    SetTextureStageFlag(0, 0);
    g_pD3DDevice->SetTexture(1, apTextures[1]);
    SetTextureStageFlag(1, 0);
    g_pD3DDevice->SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetTextureStageState(1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
    g_pD3DDevice->SetTextureStageState(1, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
    g_pD3DDevice->SetTextureStageState(1, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
    g_pD3DDevice->SetTexture(2, apTextures[2]);
    SetTextureStageFlag(2, 0);
    g_pD3DDevice->SetTextureStageState(2, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetTextureStageState(2, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetTextureStageState(2, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
    g_pD3DDevice->SetTextureStageState(2, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
    g_pD3DDevice->SetTextureStageState(2, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
    g_pD3DDevice->SetTexture(3, apTextures[3]);
    SetTextureStageFlag(3, 0);
    g_pD3DDevice->SetTextureStageState(3, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetTextureStageState(3, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetTextureStageState(3, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
    g_pD3DDevice->SetTextureStageState(3, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
    g_pD3DDevice->SetTextureStageState(3, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
    g_pD3DDevice->DrawPrimitive(D3DPT_QUADLIST, 0, 1);
    g_pD3DDevice->SetPixelShader(g_ahPixelShader[PIXEL_SHADER_THREE_TEXTURE_ADD]);
    g_pD3DDevice->SetTexture(0, apTextures[4]);
    SetTextureStageFlag(0, 0);
    g_pD3DDevice->SetTexture(1, apTextures[5]);
    SetTextureStageFlag(1, 0);
    g_pD3DDevice->SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetTextureStageState(1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
    g_pD3DDevice->SetTextureStageState(1, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
    g_pD3DDevice->SetTextureStageState(1, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
    g_pD3DDevice->SetTexture(2, apTextures[6]);
    SetTextureStageFlag(2, 0);
    g_pD3DDevice->SetTextureStageState(2, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetTextureStageState(2, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetTextureStageState(2, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
    g_pD3DDevice->SetTextureStageState(2, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
    g_pD3DDevice->SetTextureStageState(2, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
    g_pD3DDevice->SetTexture(3, NULL);
    SetTextureStageFlag(3, 0);
    g_pD3DDevice->DrawPrimitive(D3DPT_QUADLIST, 4, 1);
    g_pD3DDevice->SetTexture(0, NULL);
    SetTextureStageFlag(0, 0);
    g_pD3DDevice->SetTexture(1, NULL);
    SetTextureStageFlag(1, 0);
    g_pD3DDevice->SetTexture(2, NULL);
    SetTextureStageFlag(2, 0);
    g_pD3DDevice->SetPixelShader(0);
    pDepthStencil->Release();
    pBackBuffer->Release();
}

void GLRender::RenderAccumulationBlur(D3DTexture* pFrame, D3DTexture* pAccumulation)
{
    static RequestPoolManager* pPoolManager = RequestPoolManager::GetInstance();
    static unsigned long nPool = RequestPool(Aurora::POOL_VERTEX, 128, RequestPoolManager::POOL_COLOR, 0x3011,
                                             &nPool);
    SetBlendFunc(Aurora::BLEND_ONE, Aurora::BLEND_ZERO);
    Vector vRatio(m_fAccumulationRatio, m_fAccumulationRatio, m_fAccumulationRatio);
    g_pD3DDevice->SetVertexShader(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX2);
    g_pD3DDevice->SetTexture(0, pAccumulation);
    SetTextureStageFlag(0, 0);
    g_pD3DDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
    g_pD3DDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
    g_pD3DDevice->SetTexture(1, pFrame);
    SetTextureStageFlag(1, 0);
    g_pD3DDevice->SetTextureStageState(1, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
    g_pD3DDevice->SetTextureStageState(1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
    g_pD3DDevice->SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetTexture(2, NULL);
    g_pD3DDevice->SetTexture(3, NULL);
    g_pD3DDevice->SetPixelShader(g_ahPixelShader[PIXEL_SHADER_BLEND]);
    g_pD3DDevice->SetPixelShaderConstant(0, Homogenize(vRatio, 1.0f), 1);
    g_pD3DDevice->SetPixelShaderConstant(
        1, Homogenize(Vector(1.0f - vRatio.m_fX, 1.0f - vRatio.m_fY, 1.0f - vRatio.m_fZ), 0.0f), 1);
    g_pD3DDevice->Begin(D3DPT_QUADLIST);
    for (int i = 0; i < 4; i++)
    {
        g_pD3DDevice->SetVertexDataColor(D3DVSDE_DIFFUSE, 0xffff0000);
        float t = (i == 0 || i == 3) ? 0.0f : 1.0f;
        float s = (i == 0 || i == 1) ? 0.0f : 1.0f;
        g_pD3DDevice->SetVertexData2f(D3DVSDE_TEXCOORD0, s, t);
        if (g_nFrameBufferMode == 0)
        {
            float fScreenT = (i == 0 || i == 3) ? 0.0f : 480.0f;
            float fScreenS = (i == 0 || i == 1) ? 0.0f : 640.0f;
            g_pD3DDevice->SetVertexData2f(D3DVSDE_TEXCOORD1, fScreenS, fScreenT);
        }
        else
        {
            float fScreenT = (i == 0 || i == 3) ? 0.0f : 960.0f;
            float fScreenS = (i == 0 || i == 1) ? 0.0f : 640.0f;
            g_pD3DDevice->SetVertexData2f(D3DVSDE_TEXCOORD1, fScreenS, fScreenT);
        }
        float y = (i == 0 || i == 3) ? 1.0f : -1.0f;
        float x = (i == 0 || i == 1) ? -1.0f : 1.0f;
        g_pD3DDevice->SetVertexData4f(D3DVSDE_POSITION, x, y, 0.0f, 1.0f);
    }
    g_pD3DDevice->End();
    g_pD3DDevice->SetPixelShader(0);
    SetActiveTexture(0);
    g_pD3DDevice->SetTexture(0, NULL);
    g_pD3DDevice->SetTexture(1, NULL);
    g_pD3DDevice->SetTexture(2, NULL);
    g_pD3DDevice->SetTexture(3, NULL);
    SetTextureStageFlag(0, 0);
    SetTextureStageFlag(1, 0);
    SetTextureStageFlag(2, 0);
    SetTextureStageFlag(3, 0);
}

void GLRender::FrameBufferModifications(unsigned long nFlags)
{
    static int bAccumulationCreated;
    static D3DTexture* pAccumulationTexture;

    if (!doframebuffer || g_nFrameBufferMode == 4)
    {
        return;
    }
    if (g_pFrameTexture == NULL || g_bFrameBufferModeChanged)
    {
        if (g_pFrameTexture)
        {
            g_pFrameTexture->Release();
        }
        if (g_nFrameBufferMode == 0)
        {
            g_pD3DDevice->CreateTexture(g_nScreenWidth, g_nScreenHeight, 1, 0, D3DFMT_LIN_A8R8G8B8, 0,
                                        &g_pFrameTexture);
        }
        else if (g_nFrameBufferMode == 2)
        {
            g_pD3DDevice->CreateTexture(g_nScreenWidth, g_nScreenHeight * 2, 1, 0, D3DFMT_LIN_A8R8G8B8, 0,
                                        &g_pFrameTexture);
        }
        g_bFrameBufferModeChanged = 0;
        cm_bFirstFrameBufferFrame = true;
    }
    if (!bAccumulationCreated)
    {
        bAccumulationCreated = 1;
        g_pD3DDevice->CreateTexture(128, 128, 1, 0, D3DFMT_A8R8G8B8, 0, &pAccumulationTexture);
    }
    Disable(Aurora::ENABLE_ALPHA_TEST);
    D3DXMATRIX mProjection;
    D3DXMatrixOrthoOffCenterRH(&mProjection, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
    D3DXMATRIX mIdentity;
    D3DXMatrixIdentity(&mIdentity);
    g_pD3DDevice->SetTransform(D3DTS_PROJECTION, &mProjection);
    g_pD3DDevice->SetTransform(D3DTS_VIEW, &mIdentity);
    g_pD3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
    g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    g_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    static unsigned long nShader = TranslatePoolFormat(RequestPoolManager::POOL_FORMAT_VTC);
    g_pD3DDevice->SetVertexShader(nShader);
    Enable(Aurora::ENABLE_BLEND);
    D3DSurface* pBackBuffer;
    g_pD3DDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
    D3DSURFACE_DESC desc;
    pBackBuffer->GetDesc(&desc);
    D3DSurface* pSurface;
    g_pFrameTexture->GetSurfaceLevel(0, &pSurface);
    g_pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
    g_pD3DDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
    g_pD3DDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
    g_pD3DDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
    g_pD3DDevice->CopyRects(pBackBuffer, NULL, 0, pSurface, NULL);
    if (!visualizeregular)
    {
        g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0, 1.0f, 0);
    }
    if (visualizepass6 && m_bSpeedBlurEnabled && (nFlags & FRAMEBUFFER_SPEED_BLUR) &&
        !cm_bFirstFrameBufferFrame)
    {
        g_pD3DDevice->CopyRects(pBackBuffer, NULL, 1, pSurface, NULL);
        RenderAccumulationBlur(g_pFrameTexture, pAccumulationTexture);
    }
    if (visualizepass6 && m_bSpeedBlurEnabled && (nFlags & FRAMEBUFFER_SPEED_BLUR))
    {
        g_pD3DDevice->CopyRects(pBackBuffer, NULL, 1, pSurface, NULL);
        g_pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
        g_pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
        SetActiveTexture(0);
        CopyFrameBufferTexture(g_pFrameTexture, pAccumulationTexture);
    }
    if (visualizepass1 && (nFlags & FRAMEBUFFER_OVERBRIGHTEN))
    {
        g_pD3DDevice->CopyRects(pBackBuffer, NULL, 1, pSurface, NULL);
        RenderBrightnessAdjustment((unsigned long)g_pFrameTexture);
    }
    if (visualizepass2 && (nFlags & FRAMEBUFFER_CONVOLUTION))
    {
        g_pD3DDevice->CopyRects(pBackBuffer, NULL, 1, pSurface, NULL);
        if (dooldflares)
        {
            RenderOverlayConvolutionPyramid(g_pFrameTexture);
        }
        else
        {
            RenderOverlayConvolution(g_pFrameTexture);
        }
        static unsigned long nShader = TranslatePoolFormat(RequestPoolManager::POOL_FORMAT_VTC);
        g_pD3DDevice->SetVertexShader(nShader);
    }
    if (visualizepass5 && (nFlags & FRAMEBUFFER_FORCE_DISTORTION))
    {
        g_pD3DDevice->CopyRects(pBackBuffer, NULL, 1, pSurface, NULL);
        RenderForceDistortion((unsigned long)g_pFrameTexture);
    }
    if (visualizepass3 && m_bScanNoiseEnabled && (nFlags & FRAMEBUFFER_NOISE) && cm_pFilmNoiseTexture &&
        !cm_pFilmNoiseTexture->IsNull())
    {
        g_pD3DDevice->CopyRects(pBackBuffer, NULL, 0, pSurface, NULL);
        RenderOverlayOverbrightenATI((unsigned long)g_pFrameTexture);
        static unsigned long nShader = TranslatePoolFormat(RequestPoolManager::POOL_FORMAT_VTC);
        g_pD3DDevice->SetVertexShader(nShader);
    }
    SetBlendFunc(Aurora::BLEND_SRC_ALPHA, Aurora::BLEND_ONE_MINUS_SRC_ALPHA);
    pSurface->Release();
    pBackBuffer->Release();
    g_pD3DDevice->SetTransform(D3DTS_PROJECTION, g_pProjectionMatrixStack->GetTop());
    cm_bFirstFrameBufferFrame = false;
    g_fFrameBufferTime = DeltaT + g_fFrameBufferTime;
}

int GLRender::BuildShadowArray(const Vector& vLight, const float& fRadius,
                               const Plane& plane, const Vector& vNearCap,
                               const Vector& vFarCap, int nVertices,
                               unsigned long nPool, int nOffset, int nStride)
{
    char* pVertices = (char*)LockPool(nPool, 0, 0, RequestPoolManager::POOL_LOCK_READ_ONLY);
    static Vector light2temp;
    static Vector scratch[2];
    int nUsed = 0;
    for (int i = 0; i < nVertices; ++i)
    {
        if (m_UsedVertices[i] == SHADOW_VERTEX_USED)
            m_UsedVertices[i] = ++nUsed;
    }
    int nSize = nUsed * sizeof(scratch);
    cm_pUserShadowData = (float*)AllocateShadowMemory(nSize + sizeof(scratch));
    char* pShadowData = (char*)cm_pUserShadowData;
    for (int iVertex = 0; iVertex < nVertices; ++iVertex)
    {
        int nShadowOffset = m_UsedVertices[iVertex] * 6 - 6;
        if (m_UsedVertices[iVertex] > 0)
        {
            scratch[0] = *(Vector*)(pVertices + nOffset + iVertex * nStride);
            light2temp = normalize(scratch[0] - vLight);
            scratch[1] = vLight + light2temp * fRadius;
            if (!noclipshadows)
            {
                ClipProjection(scratch[0], vLight, plane);
                ClipProjection(scratch[1], vLight, plane);
            }
            if (dontprotectagainstclip &&
                (scratch[0] - scratch[1]) * light2temp > 0.0f)
                scratch[1] = scratch[0];
            memcpy(pShadowData + nShadowOffset * sizeof(float), scratch, sizeof(scratch));
        }
    }
    char* pCap = pShadowData + nSize;
    *(Vector*)pCap = vNearCap;
    pCap += sizeof(Vector);
    *(Vector*)pCap = vFarCap;
    UnlockPool(nPool);
    return nUsed;
}

int __cdecl CompareTransparencyHint(const void* pFirst, const void* pSecond)
{
    MdlNodeTriMesh* pFirstMesh = (*(MdlNode**)pFirst)->AsMdlNodeTriMesh();
    MdlNodeTriMesh* pSecondMesh = (*(MdlNode**)pSecond)->AsMdlNodeTriMesh();
    if (pFirstMesh == 0)
    {
        return pSecondMesh ? -1 : 0;
    }
    if (pSecondMesh == 0)
    {
        return 1;
    }
    int nSecond = pSecondMesh->m_nTransparencyHint;
    int nFirst = pFirstMesh->m_nTransparencyHint;
    if (nFirst == nSecond)
    {
        return 0;
    }
    if (nFirst == 0)
    {
        return 1;
    }
    if (nSecond == 0)
    {
        return -1;
    }
    if (nFirst > nSecond)
    {
        return 1;
    }
    return -1;
}

int __fastcall FindFaceCorner(MaxFace_PostProcess& face, int nPosition)
{
    int nCorner;
    for (nCorner = 3; nCorner--;)
    {
        if (face.m_pFaces[nCorner] == nPosition)
            break;
    }
    return nCorner;
}
