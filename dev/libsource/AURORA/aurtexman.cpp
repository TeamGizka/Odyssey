#include "aurtexman.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <windows.h>
#include <GL/gl.h>
#include "aurinternal.h"
#include "aurtexcontrol.h"
#include "aurimage.h"
#include "auremitter.h"
#include "aurbehavior.h"

class CAurTextureBasic;
inline void AurTexPchDelIndex() { (void)&List<CAurTextureBasic*>::DelIndex; }
inline void AurTexPchAllocate() { (void)&List<SafePointer<CAurObject>*>::Add; (void)&List<char*>::allocate; (void)&List<int>::allocate; }
inline void AurTexPchListVector() { List<Vector> lstVertices; }
inline void AurTexPchSP(SafePointer<CAurTexture>* p) { delete p; }
#include "aurconsole.h"

int AurTextureCompressionAvailable();
char* firstword(char* sLine);
int Parse(char* sValue, int& nValue);
int Parse(char* sValue, float& fValue);
void Parse(char* sValue, char* sDest);
void Parse(char* sValue, short& nValue);
int Parse(char* sValue, bool& bValue);

float g_fDynamicMemoryRatio = 0.5f;
bool CAurTextureBasic::m_bLowEndEnvMapEnabled = 0;
int maxtexmem = 16 * 1024 * 1024;
float memcheckratio = 0.8f;
float maxtimeformemoryoverflow = 50.0f;
bool g_bIsSuspended = 0;
bool g_bModeIsDynamic = true;

int usedtexturememory = 0;
int useddynamictexturememory = 0;
int usedstatictexturememory = 0;
int usedlightmapmemory = 0;
int usedcomptexmemory = 0;
int usedcubemapmemory = 0;
int usedbumpmapmemory = 0;
int texcurrentframe = 0;

extern "C"
{
unsigned char* (*AurGetRaster)(char*, int*, int*, int*) = ImageRead;
int (*AurGetRasterHeader)(char*, int*, int*, int*) = ImageReadHeader;
void (*AurReleaseRaster)(void*, int, int, int, int, int, int, bool) = 0;
unsigned char* (*AurGetS3TCImage)(char*, int*, int*, int*, int*, float*) = ImageReadS3TC;
int (*AurGetS3TCHeader)(char*, int*, int*, int*, int*, float*) = ImageReadS3TCHeader;
unsigned char* (*AurGet16BitRaster)(char*, int*, int*, float*, int*) = ImageRead16bit;
int (*AurGet16BitRasterHeader)(char*, int*, int*, float*, int*) = ImageRead16bitHeader;
unsigned char* (*AurGetLayeredRaster)(char*, int*, int*, int*) = ImageReadPLT;
void (*AurReleaseLayeredRaster)(void*, int, int) = 0;
unsigned char* (*AurGetProcessedRaster)(char*, int*, int*, char*, int*, int*,
                                       int*, float*, char*) = ImageReadProcessed;
int (*AurGetProcessedRasterHeader)(char*, int*, int*, char*, int*, int*,
                                    float*, char*) = ImageReadProcessedHeader;
}
static int nocompression = 0;
unsigned char* paletteimage = 0;
int paletteheight = 0;

int palettewidth = 256;
int palettebpp = 4;
int alwaysbind = 0;
int countbinds = 0;

int enabletexturecontrollers = 1;

int bindzero = 0;
int countbasictextures = 0;

int orderedlisttime = 0;
int textureinittime = 0;

int constructimagetime = 0;
int glimagetime = 0;
int loadimagetime = 0;
int decodeimagetime = 0;

int gl1;
int gl2;
int gl3;
int texturefindtime = 0;
int texturegettime = 0;
int texturecreatetime = 0;
int textureheadertime = 0;
int texturecount = 0;
int basictexturecount = 0;

float currentoverflow = 0.0f;
float maxdeltat = 0;
int maxtextime = 0;
int maxtexloadtime = 0;
int maxdecodetime = 0;
int maxgltime = 0;

int maxtexid;

static int InitCounterStepsBeforeTextureLists()
{
    static List<int> s_Steps;
    return s_Steps.m_nCount;
}
static int InitCounterStepBeforeTextureLists()
{
    static int s_nStep = atoi("");
    return s_nStep;
}
#ifdef _XBOX

SafePointer<CAurTexture> CAurTextureBasic::m_pNullTexture((CAurTexture*)NULL);
#else
SafePointer<CAurTexture> CAurTextureBasic::m_pNullTexture;
#endif
List<CAurTextureBasic*> CAurTextureBasic::m_OrderedTextureList;
List<CAurTextureBasic*> CAurTextureBasic::m_StaticTextureList;
List<CAurTextureBasic*> CAurTextureBasic::m_DynamicTextureList;
List<CAurTextureBasic*> RefreshTextures;

extern List<int> TextureIndices;
// PC 0x00420000
void AurTextureSetDynamicMemoryRatio(float fRatio)
{
    g_fDynamicMemoryRatio = fRatio;
}

#ifdef _XBOX

void SwapTextures(CAurTextureBasic*& pFirst, CAurTextureBasic*& pSecond)
{
    CAurTextureBasic* pTemp = pFirst;
    pFirst = pSecond;
    pSecond = pTemp;
}
#endif
// PC 0x00420010
void InsertionSortTexture(CAurTextureBasic** ppTextures, int nCount)
{
    for (int i = 1; i < nCount; i++)
    {
#ifdef _XBOX
        CAurTextureBasic* pTexture = ppTextures[i];
        int j = i - 1;
        while (pTexture->m_fSortKey > ppTextures[j]->m_fSortKey)
        {
            SwapTextures(ppTextures[j + 1], ppTextures[j]);
            if (--j < 0)
            {
                break;
            }
        }
#else
        CAurTextureBasic* pTexture = ppTextures[i];
        int j = i - 1;
        if (pTexture->m_fSortKey > ppTextures[j]->m_fSortKey)
        {
            do
            {
                CAurTextureBasic*& pHigherSlot = ppTextures[j + 1];
                CAurTextureBasic* pHigher = pHigherSlot;
                pHigherSlot = ppTextures[j];
                ppTextures[j] = pHigher;
                j--;
                if (j < 0)
                {
                    break;
                }
            } while (pTexture->m_fSortKey > ppTextures[j]->m_fSortKey);
        }
#endif
    }
}
// PC 0x00420070
unsigned char* AurGetImageWrapper(char* sFileName, int* pWidth, int* pHeight, int* pBPP,
                       int* pCompressedSize, int* pReserved6, int* pReserved7,
                       Aurora::AuroraTextureFormats* pFormat, float* pAlpha,
                       bool* pProcessed, short* pNumMipMaps,
                       int bAllowCompression)
{
    if (nocompression)
    {
        bAllowCompression = 0;
    }
    *pCompressedSize = 0;
    *pReserved6 = 0;
    *pReserved7 = 0;
    char cBPP;
    char cNumMipMaps;
    unsigned char* result = AurGetProcessedRaster(sFileName, pWidth, pHeight, &cBPP,
                                       pCompressedSize, pReserved6, pReserved7,
                                       pAlpha, &cNumMipMaps);
    if (result != 0)
    {
        *pBPP = cBPP;
        *pNumMipMaps = cNumMipMaps;
        *pProcessed = true;
        if (*pCompressedSize != 0)
        {
            switch (*pBPP)
            {
            case 3:
                *pFormat = Aurora::TEXTURE_FORMAT_DXT1;
                break;
            case 4:
                *pFormat = Aurora::TEXTURE_FORMAT_DXT5;
                break;
            }
        }
        else
        {
            switch (*pBPP)
            {
            case 1:
                *pFormat = Aurora::TEXTURE_FORMAT_LUMINANCE;
                break;
            case 3:
                *pFormat = Aurora::TEXTURE_FORMAT_RGB;
                break;
            case 4:
                *pFormat = Aurora::TEXTURE_FORMAT_RGBA;
                break;
            }
        }
    }
    if (result == 0 && bAllowCompression != 0 && AurTextureCompressionAvailable())
    {
        result = AurGetS3TCImage(sFileName, pWidth, pHeight, pBPP,
                                 pCompressedSize, pAlpha);
        if (result != 0)
        {
            switch (*pBPP)
            {
            case 3:
                *pFormat = Aurora::TEXTURE_FORMAT_DXT1;
                break;
            case 4:
                *pFormat = Aurora::TEXTURE_FORMAT_DXT5;
                break;
            }
        }
    }
    if (result == 0)
    {
        result = AurGet16BitRaster(sFileName, pWidth, pHeight, pAlpha, (int*)pFormat);
        *pBPP = 2;
        if (result != 0 && AurGet16BitRaster == ImageRead16bit)
        {
            return result;
        }
    }
    if (result == 0)
    {
        result = AurGetRaster(sFileName, pWidth, pHeight, pBPP);
        if (result != 0)
        {
            switch (*pBPP)
            {
            case 1:
                *pFormat = Aurora::TEXTURE_FORMAT_LUMINANCE;
                break;
            case 3:
                *pFormat = Aurora::TEXTURE_FORMAT_RGB;
                break;
            case 4:
                *pFormat = Aurora::TEXTURE_FORMAT_RGBA;
                break;
            }
        }
    }
    return result;
}
// PC 0x00420220
void AurClearPalette()
{
    operator delete(paletteimage);
    paletteimage = NULL;
    paletteheight = 0;
}

TextureController* AurTextureGetController(CAurTexture* pTexture)
{
    return pTexture->GetReferencedTexture()->AsCAurTextureBasic()->m_pController;
}
// PC 0x00420240
bool CAurTexture::IsCubeMap()
{
    CAurTextureBasic* pTexture = GetReferencedTexture()->AsCAurTextureBasic();
    if (pTexture != NULL)
    {
        return pTexture->m_bCubeMap ? true : false;
    }
    return false;
}
// PC 0x004207B0
unsigned char CAurTexture::IsFullyLoaded()
{
    CAurTextureBasic* pTexture = GetReferencedTexture()->AsCAurTextureBasic();
    if (pTexture != NULL)
    {
        return pTexture->m_bFullyLoaded;
    }
    else
    {
        return 0;
    }
}
// PC 0x00420260
unsigned char CAurTexture::IsProcessed()
{
    CAurTextureBasic* pTexture = GetReferencedTexture()->AsCAurTextureBasic();
    if (pTexture != NULL)
    {
        return pTexture->m_bProcessed;
    }
    else
    {
        return 0;
    }
}
// PC 0x00420280
unsigned char CAurTexture::IsTxiLoaded()
{
    CAurTextureBasic* pTexture = GetReferencedTexture()->AsCAurTextureBasic();
    if (pTexture != NULL)
    {
        return pTexture->m_bTxiLoaded;
    }
    else
    {
        return 0;
    }
}
// PC 0x004202C0
void CAurTexture::SetTxiLoaded(bool bLoaded)
{
    CAurTextureBasic* pTexture = GetReferencedTexture()->AsCAurTextureBasic();
    if (pTexture != NULL)
    {
        pTexture->m_bTxiLoaded = bLoaded;
    }
}
// PC 0x004202E0
char* CAurTexture::GetTxiData()
{
    CAurTextureBasic* pTexture = GetReferencedTexture()->AsCAurTextureBasic();
    if (pTexture != NULL)
    {
        return pTexture->m_pTxiData;
    }
    else
    {
        return NULL;
    }
}
// PC 0x00420300
void CAurTexture::SetTxiData(char* pData, int nSize)
{
    CAurTextureBasic* pTexture = GetReferencedTexture()->AsCAurTextureBasic();
    if (pTexture != NULL && pData != NULL && nSize > 0)
    {
        if (pTexture->m_pTxiData != NULL)
        {
            delete[] pTexture->m_pTxiData;
            pTexture->m_pTxiData = NULL;
        }
        pTexture->m_pTxiData = new char[nSize];
        memcpy(pTexture->m_pTxiData, pData, nSize);
        pTexture->m_nTxiSize = nSize;
    }
}
// PC 0x00420370
int CAurTexture::GetTxiSize()
{
    CAurTextureBasic* pTexture = GetReferencedTexture()->AsCAurTextureBasic();
    if (pTexture != NULL)
    {
        return pTexture->m_nTxiSize;
    }
    else
    {
        return 0;
    }
}

bool CAurTexture::IsEnvironmentMapped()
{
    CAurTextureBasic* pTexture = GetReferencedTexture()->AsCAurTextureBasic();
    if (pTexture != NULL)
    {
        return pTexture->m_bEnvMapped;
    }
    else
    {
        return 0;
    }
}
// PC 0x004203B0
Vector CAurTexture::GetSpecularColor()
{
    CAurTextureBasic* pTexture = GetReferencedTexture()->AsCAurTextureBasic();
    Vector vColor;
    if (pTexture != NULL)
    {
        return pTexture->m_vSpecularColor;
    }
    return vColor;
}
// PC 0x00420420
float CAurTexture::GetBumpIntensity()
{
    CAurTextureBasic* pTexture = GetReferencedTexture()->AsCAurTextureBasic();
    if (pTexture != NULL)
    {
        return pTexture->m_fBumpIntensity;
    }
    else
    {
        return 0.0f;
    }
}
// PC 0x00420440
float CAurTexture::GetSpecularBumpIntensity()
{
    CAurTextureBasic* pTexture = GetReferencedTexture()->AsCAurTextureBasic();
    float fIntensity = 0.0f;
    if (pTexture != NULL)
    {
        fIntensity = pTexture->IsSpecularBumpMap()
                         ? pTexture->m_fSpecularBumpIntensity
                         : 0.0f;
    }
    return fIntensity;
}
// PC 0x00420480
float CAurTexture::GetDiffuseBumpIntensity()
{
    CAurTextureBasic* pTexture = GetReferencedTexture()->AsCAurTextureBasic();
    float fIntensity = 0.0f;
    if (pTexture != NULL)
    {
        fIntensity = pTexture->IsDiffuseBumpMap()
                         ? pTexture->m_fDiffuseBumpIntensity
                         : 0.0f;
    }
    return fIntensity;
}
// PC 0x004204C0
bool CAurTexture::IsDiffuseBumpMap()
{
    CAurTextureBasic* pTexture = GetReferencedTexture()->AsCAurTextureBasic();
    if (pTexture != NULL)
    {
        return pTexture->IsBumpMap() && pTexture->m_bDiffuseBumpMap != 0;
    }
    return 0;
}
// PC 0x00420500
bool CAurTexture::IsSpecularBumpMap()
{
    CAurTextureBasic* pTexture = GetReferencedTexture()->AsCAurTextureBasic();
    if (pTexture != NULL)
    {
        return pTexture->IsBumpMap() && pTexture->m_bSpecularBumpMap != 0;
    }
    return 0;
}
// PC 0x00420540
bool CAurTexture::IsBumpMap()
{
    CAurTextureBasic* pTexture = GetReferencedTexture()->AsCAurTextureBasic();
    if (pTexture != NULL)
    {
        return (pTexture->m_nBumpType != 0 && pTexture->IsProcessed()) ||
               (pTexture->m_nBumpType == 1 && pTexture->m_nBPP == 1) ||
               (pTexture->m_nBumpType == 2 && pTexture->m_nBPP == 4);
    }
    return 0;
}
// PC 0x004205A0
bool CAurTexture::IsNormalMap()
{
    CAurTextureBasic* pTexture = GetReferencedTexture()->AsCAurTextureBasic();
    if (pTexture != NULL)
    {
        return pTexture->m_nBumpType == 2 && pTexture->m_nBPP == 4;
    }
    return 0;
}
// PC 0x00420390
unsigned char CAurTexture::IsUsingGlobalAlpha()
{
    CAurTextureBasic* pTexture = GetReferencedTexture()->AsCAurTextureBasic();
    if (pTexture != NULL)
    {
        return pTexture->m_bUsingGlobalAlpha;
    }
    else
    {
        return 0;
    }
}
// PC 0x004205F0
CAurFontInfo* CAurTexture::GetFontInfo()
{
    CAurTextureBasic* pTexture = GetReferencedTexture()->AsCAurTextureBasic();
    if (pTexture != NULL)
    {
        return pTexture->m_pFontInfo;
    }
    else
    {
        return NULL;
    }
}
// PC 0x00420610
unsigned char* CAurTexture::GetImage()
{
    CAurTextureBasic* pTexture = GetReferencedTexture()->AsCAurTextureBasic();
    if (pTexture != NULL)
    {
        return pTexture->m_pImage;
    }
    else
    {
        return NULL;
    }
}
// PC 0x00420630
char* CAurTexture::GetName()
{
    CAurTextureBasic* pTexture = GetReferencedTexture()->AsCAurTextureBasic();
    if (pTexture != NULL)
    {
        return pTexture->m_sName;
    }
    else
    {
        return NULL;
    }
}
// PC 0x00420770
int CAurTexture::GetFileRange()
{
    CAurTextureBasic* pTexture = GetReferencedTexture()->AsCAurTextureBasic();
    if (pTexture != NULL)
    {
        return pTexture->m_nFileRange;
    }
    else
    {
        return 0;
    }
}
// PC 0x00420670
int CAurTexture::GetWidth()
{
    CAurTextureBasic* pTexture = GetReferencedTexture()->AsCAurTextureBasic();
    if (pTexture != NULL)
    {
        return pTexture->m_nWidth;
    }
    else
    {
        return 0;
    }
}
// PC 0x00420690
int CAurTexture::GetHeight()
{
    CAurTextureBasic* pTexture = GetReferencedTexture()->AsCAurTextureBasic();
    if (pTexture != NULL)
    {
        return pTexture->m_nHeight;
    }
    else
    {
        return 0;
    }
}
// PC 0x004206B0
int CAurTexture::GetDepth()
{
    CAurTextureBasic* pTexture = GetReferencedTexture()->AsCAurTextureBasic();
    if (pTexture != NULL)
    {
        return pTexture->m_nDepth;
    }
    else
    {
        return 0;
    }
}
// PC 0x00420730
int CAurTexture::GetBPP()
{
    CAurTextureBasic* pTexture = GetReferencedTexture()->AsCAurTextureBasic();
    if (pTexture != NULL)
    {
        return pTexture->m_nBPP;
    }
    else
    {
        return 0;
    }
}
// PC 0x00420750
int CAurTexture::GetNumMipMaps()
{
    CAurTextureBasic* pTexture = GetReferencedTexture()->AsCAurTextureBasic();
    if (pTexture != NULL)
    {
        return pTexture->m_nNumMipMaps;
    }
    else
    {
        return 0;
    }
}
// PC 0x00420790
int CAurTexture::GetNumX()
{
    CAurTextureBasic* pTexture = GetReferencedTexture()->AsCAurTextureBasic();
    if (pTexture != NULL)
    {
        return pTexture->m_nNumX;
    }
    else
    {
        return 0;
    }
}
// PC 0x00420810
int CAurTexture::GetNumY()
{
    CAurTextureBasic* pTexture = GetReferencedTexture()->AsCAurTextureBasic();
    if (pTexture != NULL)
    {
        return pTexture->m_nNumY;
    }
    else
    {
        return 0;
    }
}

unsigned char CAurTexture::GetMipMap()
{
    CAurTextureBasic* pTexture = GetReferencedTexture()->AsCAurTextureBasic();
    if (pTexture != NULL)
    {
        return pTexture->m_bMipMap;
    }
    else
    {
        return 0;
    }
}
// PC 0x004207D0
unsigned char CAurTexture::GetFilter()
{
    CAurTextureBasic* pTexture = GetReferencedTexture()->AsCAurTextureBasic();
    if (pTexture != NULL)
    {
        return pTexture->m_bFilter;
    }
    else
    {
        return 0;
    }
}
// PC 0x004207F0
unsigned char CAurTexture::GetMapTexelsToPixels()
{
    CAurTextureBasic* pTexture = GetReferencedTexture()->AsCAurTextureBasic();
    if (pTexture != NULL)
    {
        return pTexture->m_bMapTexelsToPixels;
    }
    else
    {
        return 0;
    }
}
// PC 0x00420650
int CAurTexture::GetClamp()
{
    CAurTextureBasic* pTexture = GetReferencedTexture()->AsCAurTextureBasic();
    if (pTexture != NULL)
    {
        return pTexture->m_nClamp;
    }
    else
    {
        return 0;
    }
}
// PC 0x00420830
float CAurTexture::GetBumpMapScaling()
{
    CAurTextureBasic* pTexture = GetReferencedTexture()->AsCAurTextureBasic();
    if (pTexture != NULL)
    {
        return pTexture->m_fBumpMapScaling;
    }
    else
    {
        return 0.0f;
    }
}
// PC 0x00420850
float CAurTexture::GetEnvMapAlpha()
{
    CAurTextureBasic* pTexture = GetReferencedTexture()->AsCAurTextureBasic();
    if (pTexture != NULL)
    {
        return pTexture->m_fEnvMapAlpha;
    }
    else
    {
        return 1.0f;
    }
}
// PC 0x00420870
int CAurTexture::GetCompressedSize()
{
    CAurTextureBasic* pTexture = GetReferencedTexture()->AsCAurTextureBasic();
    if (pTexture != NULL)
    {
        return pTexture->m_nCompressedSize;
    }
    else
    {
        return 0;
    }
}
// PC 0x00420890
int CAurTexture::GetCompressionFormat()
{
    CAurTextureBasic* pTexture = GetReferencedTexture()->AsCAurTextureBasic();
    if (pTexture != NULL && pTexture->m_nCompressedSize != 0)
    {
        return pTexture->m_nTextureFormat;
    }
    else
    {
        return 7;
    }
}
// PC 0x004208C0
int CAurTexture::GetTextureFormat()
{
    CAurTextureBasic* pTexture = GetReferencedTexture()->AsCAurTextureBasic();
    if (pTexture != NULL)
    {
        if (pTexture->m_nCompressedSize == 0 &&
            pTexture->m_nTextureFormat == Aurora::TEXTURE_FORMAT_DXT1)
        {
            int nFormat;
            if (pTexture->m_nBPP == 4)
            {
                nFormat = Aurora::TEXTURE_FORMAT_RGBA;
            }
            else if (pTexture->m_nBPP == 3)
            {
                nFormat = Aurora::TEXTURE_FORMAT_RGB;
            }
            else
            {
                nFormat = pTexture->m_nBPP == 2
                              ? Aurora::TEXTURE_FORMAT_LUMINANCE_ALPHA
                              : Aurora::TEXTURE_FORMAT_LUMINANCE;
            }
            pTexture->m_nTextureFormat = nFormat;
        }
        return pTexture->m_nTextureFormat;
    }
    return Aurora::TEXTURE_FORMAT_DXT1;
}
// PC 0x00420990
#ifdef _XBOX
bool TextureController::IsLinear()
#else
char TextureController::IsLinear()
#endif
{
    return 0;
}
// PC 0x00420920
void CAurTexture::SetTextureFormat(Aurora::AuroraTextureFormats nFormat)
{
    CAurTextureBasic* pTexture = GetReferencedTexture()->AsCAurTextureBasic();
    if (pTexture != NULL && pTexture->m_nCompressedSize == 0)
    {
        pTexture->m_nTextureFormat = nFormat;
    }
}
// PC 0x00420BE0
inline LifeTextureController::~LifeTextureController()
{
    delete[] m_pState;
}

inline LifeTextureController::LifeTextureController(CAurTexture* pTexture)
    : TextureController(pTexture)
{
}
// PC 0x00420950
void CAurTexture::FreeImage()
{
    CAurTextureBasic* pTexture = GetReferencedTexture()->AsCAurTextureBasic();
    if (pTexture != NULL)
    {
        pTexture->FreeImage();
    }
}
// PC 0x00420B60
inline PerlinTextureController::~PerlinTextureController()
{
    delete[] m_pPerlinImage;
    delete[] m_pWorkImage;
}

inline PerlinTextureController::PerlinTextureController(CAurTexture* pTexture)
    : TextureController(pTexture)
{
}
// PC 0x00420970
void CAurTexture::IncrementTIDIndex(int nIndex)
{
    CAurTextureBasic* pTexture = GetReferencedTexture()->AsCAurTextureBasic();
    if (pTexture != NULL)
    {
        pTexture->m_nTIDIndex = pTexture->m_pTextureIDs[nIndex];
    }
}
// PC 0x004209C0
inline RandomTextureController::~RandomTextureController()
{
    delete[] m_pRandomImage;
    delete[] m_pChannelImage;
}

inline RandomTextureController::RandomTextureController(CAurTexture* pTexture)
    : TextureController(pTexture)
{
    m_pRandomImage = NULL;
    m_pChannelImage = NULL;
}
// PC 0x00420A40
inline RingDistortTextureController::~RingDistortTextureController()
{
    delete[] m_pRingImage;
}

inline RingDistortTextureController::RingDistortTextureController(CAurTexture* pTexture)
    : TextureController(pTexture)
{
    m_pRingImage = NULL;
    m_fAngle = 0.0f;
}

#ifdef _XBOX

inline HeatDistortTextureController::~HeatDistortTextureController()
{
    delete[] m_pHeatImage;
}

inline HeatDistortTextureController::HeatDistortTextureController(CAurTexture* pTexture)
    : TextureController(pTexture)
{
    m_pHeatImage = NULL;
    m_fAngle = 0.0f;
}
#endif
// PC 0x00420AA0
short AurTextureCalculateHashID(char* sName, short nNumTXI, int* pTXI)
{
    int nHash = 0;
    int nLength = strlen(sName);
    for (int i = 0; i < nLength; i++)
    {
        int nValue;
        if (nHash == 0)
        {
            nValue = tolower(sName[i]);
        }
        else
        {
            nValue = nHash * tolower(sName[i]);
        }
        nHash = nValue % 65101;
    }
    if (nNumTXI > 0)
    {
        for (int j = 0; j < nNumTXI; j++)
        {
            int nValue = pTXI[j];
            if (nHash != 0)
            {
                nValue *= nHash;
            }
            nHash = nValue % 65101;
        }
    }
    return nHash;
}

#ifdef _XBOX
void AurTextureFreeImage(unsigned char* pImage, int nWidth, int nHeight, int nBPP, int nCompressedSize, int nDataSize,
                         int nTxiSize, bool* pbAllocated, bool bProcessed, bool bLayered);
unsigned char* AurGetLayeredImageWrapper(char* sFileName, int* pWidth, int* pHeight, int* pBPP);

void CAurTextureBasic::FreeImage()
{
    if (m_pImage != NULL)
    {
        if (m_nCycleCompressedSize != 0)
        {
            m_bTxiLoaded = 0;
            AurTextureFreeImage(m_pImage, m_nWidth * m_nNumX, m_nHeight * m_nNumY, m_nBPP, m_nCycleCompressedSize,
                                m_nDataSize, m_nTxiSize, (bool*)&m_bRasterAllocated, m_bProcessed, false);
            m_pImage = NULL;
            return;
        }
        int nBytesPerBlock;
        int nFaces = 1;
        if (m_nBPP == 3)
        {
            nBytesPerBlock = 8;
        }
        else if (m_nBPP == 4)
        {
            nBytesPerBlock = 16;
        }
        int nWidth = m_nWidth;
        int nHeight = m_nHeight;
        if (nHeight / nWidth == 6)
        {
            nFaces = 6;
            nHeight /= 6;
        }
        int nSize = m_nCompressedSize;
        if (nSize != 0)
        {
            for (int i = 0; i < m_nNumMipMaps - 1; i++)
            {
                nHeight >>= 1;
                nWidth >>= 1;
                nSize += ((nWidth + 3) / 4) * ((nHeight + 3) / 4) * nBytesPerBlock;
            }
            if (nFaces == 6)
            {
                nSize = nSize - nSize % 128 + 128;
                nSize *= 6;
            }
        }
        m_bTxiLoaded = 0;
        AurTextureFreeImage(m_pImage, m_nWidth, m_nHeight, m_nBPP, nSize, m_nDataSize, m_nTxiSize,
                            (bool*)&m_bRasterAllocated, m_bProcessed, false);
        m_pImage = NULL;
    }
}
#else
void CAurTextureBasic::FreeImage()  // PC 0x00420CC0
{
    if (m_pImage != NULL)
    {
        int nBytesPerBlock;
        int nFaces = 1;
        if (m_nBPP == 3)
        {
            nBytesPerBlock = 8;
        }
        else if (m_nBPP == 4)
        {
            nBytesPerBlock = 16;
        }
        int nWidth = m_nWidth;
        int nHeight = m_nHeight;
        if (nHeight / nWidth == 6)
        {
            nFaces = 6;
            nHeight /= 6;
        }
        int nSize = m_nCompressedSize;
        if (nSize != 0)
        {
            for (int i = 0; i < m_nNumMipMaps - 1; i++)
            {
                nHeight >>= 1;
                nWidth >>= 1;
                nSize +=
                    ((nWidth + 3) / 4) * ((nHeight + 3) / 4) * nBytesPerBlock;
            }
            if (nFaces == 6)
            {
                nSize *= 6;
            }
        }

        char& bRasterAllocated = m_bRasterAllocated;
        m_bTxiLoaded = 0;
        if (bRasterAllocated != 0)
        {
            AurReleaseRaster(m_pImage, m_nWidth, m_nHeight, m_nBPP, nSize,
                             m_nDataSize, m_nTxiSize, m_bProcessed);
            bRasterAllocated = 0;
        }
        else
        {
            delete[] m_pImage;
        }
        m_pImage = NULL;
    }
}
#endif

#ifndef _XBOX
inline
#endif
void AurTextureFreeImage(unsigned char* pImage, int nWidth, int nHeight,
                         int nBPP, int nCompressedSize, int nDataSize,
                         int nTxiSize, bool* pbAllocated, bool bProcessed,
                         bool bLayered)
{
    if (*pbAllocated)
    {
        if (bLayered)
        {
            AurReleaseLayeredRaster(pImage, nWidth, nHeight);
        }
        else
        {
            AurReleaseRaster(pImage, nWidth, nHeight, nBPP, nCompressedSize,
                             nDataSize, nTxiSize, bProcessed);
        }
        *pbAllocated = false;
    }
    else
    {
#ifdef _XBOX
        operator delete(pImage);
#else
        delete[] pImage;
#endif
    }
}

#ifdef _XBOX

unsigned char* CAurTextureBasic::LoadImageA()
{
    FreeImage();
    if (m_sFileName[0] == 0)
    {
        strcpy(m_sFileName, m_sName);
    }
    if (m_nFileRange != 0)
    {
        unsigned char** ppImages = (unsigned char**)operator new(m_nFileRange * sizeof(unsigned char*));
        m_nHeight = 0;
        for (int i = 0; i < m_nFileRange; i++)
        {
            ppImages[i] = NULL;
            char sBuffer[128];
            int nCompressedSize;
            int nWidth, nHeight, nBPP;
            sprintf(sBuffer, "%s%d", m_sFileName, i);
            ppImages[i] = (unsigned char*)AurGetImageWrapper(
                sBuffer, &nWidth, &nHeight, &nBPP, &nCompressedSize, &m_nDataSize, &m_nTxiSize,
                (Aurora::AuroraTextureFormats*)&m_nTextureFormat, &m_fSortKey, (bool*)&m_bProcessed, &m_nNumMipMaps,
                0);
            if (ppImages[i] == NULL)
            {
                sprintf(sBuffer, "%s0%d", m_sFileName, i);
                ppImages[i] = (unsigned char*)AurGetImageWrapper(
                    sBuffer, &nWidth, &nHeight, &nBPP, &nCompressedSize, &m_nDataSize, &m_nTxiSize,
                    (Aurora::AuroraTextureFormats*)&m_nTextureFormat, &m_fSortKey, (bool*)&m_bProcessed,
                    &m_nNumMipMaps, 0);
            }
            m_nWidth = nWidth;
            m_nBPP = nBPP;
            m_nHeight = nHeight;
        }
        unsigned char* pAll = (unsigned char*)operator new(m_nFileRange * m_nBPP * m_nHeight * m_nWidth);
        m_pImage = pAll;
        unsigned char* pDest = pAll;
        for (int i = 0; i < m_nFileRange; i++)
        {
            memcpy(pDest, ppImages[i], m_nBPP * m_nHeight * m_nWidth);
            pDest += m_nWidth * m_nHeight * m_nBPP;
            bool bAllocated = true;
            if (AurReleaseRaster == NULL)
            {
                bAllocated = false;
            }
            AurTextureFreeImage(ppImages[i], m_nWidth, m_nHeight, m_nBPP, 0, m_nDataSize, m_nTxiSize, &bAllocated,
                                m_bProcessed, false);
        }
        if (!IsCubeMap())
        {
            m_nNumY = m_nFileRange;
        }
        m_bRasterAllocated = 0;
        operator delete(ppImages);
        m_nHeight = m_nFileRange * m_nHeight;
    }
    else
    {
        int bAllowCompression = 0;
        if (m_bCubeMap == 0 && m_nNumX * m_nNumY == 1 && m_pController == NULL)
        {
            bAllowCompression = 1;
        }
        m_pImage = AurGetImageWrapper(
            m_sFileName, &m_nWidth, &m_nHeight, &m_nBPP, &m_nCompressedSize, &m_nDataSize, &m_nTxiSize,
            (Aurora::AuroraTextureFormats*)&m_nTextureFormat, &m_fSortKey, (bool*)&m_bProcessed, &m_nNumMipMaps,
            bAllowCompression);
        if (m_nCompressedSize != 0 && (m_nNumX > 1 || m_nNumY > 1))
        {
            m_nCycleCompressedSize = m_nCompressedSize;
            m_nWidth /= m_nNumX;
            m_nHeight /= m_nNumY;
            int nBytesPerBlock = m_nBPP == 4 ? 16 : 8;
            m_nCompressedSize = ((m_nHeight + 3) / 4) * ((m_nWidth + 3) / 4) * nBytesPerBlock;
            short nNumMipMaps = 1;
            int nSize = m_nWidth;
            while (nSize > 1)
            {
                nSize >>= 1;
                nNumMipMaps++;
                m_nCompressedSize += ((nSize + 3) / 4) * ((nSize + 3) / 4) * nBytesPerBlock;
            }
            m_nNumMipMaps = nNumMipMaps;
        }
        if (AurReleaseRaster != NULL && m_pImage != NULL)
        {
            m_bRasterAllocated = 1;
        }
        else
        {
            m_bRasterAllocated = 0;
        }
    }
    return m_pImage;
}
#else
unsigned char* CAurTextureBasic::LoadImageA()  // PC 0x00420DF0
{
    FreeImage();
    if (m_sFileName[0] == 0)
    {
        strcpy(m_sFileName, m_sName);
    }
    if (m_nFileRange != 0)
    {
        unsigned char** ppImages =
            new unsigned char*[m_nFileRange];
        m_nHeight = 0;
        for (int i = 0; i < m_nFileRange; i++)
        {
            ppImages[i] = NULL;
            char sBuffer[128];
            int nCompressedSize;
            int nWidth, nHeight, nBPP;
            sprintf(sBuffer, "%s%d", m_sFileName, i);
            ppImages[i] = (unsigned char*)AurGetImageWrapper(
                sBuffer, &nWidth, &nHeight, &nBPP, &nCompressedSize, &m_nDataSize,
                &m_nTxiSize, (Aurora::AuroraTextureFormats*)&m_nTextureFormat,
                &m_fSortKey, (bool*)&m_bProcessed, &m_nNumMipMaps, 0);
            if (ppImages[i] == NULL)
            {
                sprintf(sBuffer, "%s0%d", m_sFileName, i);
                ppImages[i] = (unsigned char*)AurGetImageWrapper(
                    sBuffer, &nWidth, &nHeight, &nBPP, &nCompressedSize, &m_nDataSize,
                    &m_nTxiSize,
                    (Aurora::AuroraTextureFormats*)&m_nTextureFormat,
                    &m_fSortKey, (bool*)&m_bProcessed, &m_nNumMipMaps, 0);
            }
            m_nWidth = nWidth;
            m_nBPP = nBPP;
            m_nHeight = nHeight;
        }
        unsigned char* pAll = new unsigned char[
            m_nWidth * m_nHeight * m_nBPP * m_nFileRange];
        m_pImage = pAll;
        unsigned char* pDest = pAll;
        for (int i = 0; i < m_nFileRange; i++)
        {
            memcpy(pDest, ppImages[i], m_nWidth * m_nHeight * m_nBPP);
            pDest += m_nWidth * m_nHeight * m_nBPP;
            bool bAllocated = true;
            if (AurReleaseRaster == NULL)
            {
                bAllocated = false;
            }
            AurTextureFreeImage(ppImages[i], m_nWidth, m_nHeight, m_nBPP, 0,
                                m_nDataSize, m_nTxiSize, &bAllocated,
                                m_bProcessed, false);
        }
        if (!IsCubeMap())
        {
            m_nNumY = m_nFileRange;
        }
        m_bRasterAllocated = 0;
        delete[] ppImages;
        m_nHeight *= m_nFileRange;
    }
    else
    {
        int bAllowCompression = 0;
        if (m_bCubeMap == 0 && m_nNumX * m_nNumY == 1 && m_pController == NULL)
        {
            bAllowCompression = 1;
        }
        m_pImage = AurGetImageWrapper(
            m_sFileName, &m_nWidth, &m_nHeight, &m_nBPP, &m_nCompressedSize,
            &m_nDataSize, &m_nTxiSize,
            (Aurora::AuroraTextureFormats*)&m_nTextureFormat, &m_fSortKey,
            (bool*)&m_bProcessed, &m_nNumMipMaps, bAllowCompression);
        if (AurReleaseRaster != NULL && m_pImage != NULL)
        {
            m_bRasterAllocated = 1;
        }
        else
        {
            m_bRasterAllocated = 0;
        }
    }
    return m_pImage;
}
#endif
// PC 0x00421100
unsigned char* CAurTextureBasic::LoadImagePLT()
{
    FreeImage();
    if (m_sFileName[0] == 0)
    {
        strcpy(m_sFileName, m_sName);
    }
#ifdef _XBOX

    unsigned char* pImage = AurGetLayeredImageWrapper(m_sFileName, &m_nWidth, &m_nHeight, &m_nBPP);
    m_pImage = pImage;
    m_nTextureFormat = Aurora::TEXTURE_FORMAT_RGBA;
#else
    int nReserved;
    unsigned char* pImage =
        AurGetLayeredRaster(m_sFileName, &m_nWidth, &m_nHeight, &nReserved);
    m_nBPP = 2;

    if (AurGetLayeredRaster == ImageReadPLT)
    {
        m_nTextureFormat = Aurora::TEXTURE_FORMAT_RGBA;
        m_pImage = pImage;
    }
    else
    {
        m_nTextureFormat = Aurora::TEXTURE_FORMAT_RGBA;
        m_pImage = pImage;
    }
#endif
    if (AurReleaseRaster != NULL && pImage != NULL)
    {
        m_bRasterAllocated = 1;
    }
    else
    {
        m_bRasterAllocated = 0;
    }
    return pImage;
}

#ifdef _XBOX

void CAurTextureBasic::ConstructImage()
{
    ImageGammaCorrect(m_nWidth, m_nHeight, m_nBPP, m_pImage, m_fGamma);
    if (m_pScheme != NULL)
    {
        LoadImagePLT();
        unsigned char* pImage = m_pImage;
        m_nCompressedSize = 0;
        if (pImage != NULL)
        {
            m_pImage = ImageDecode(pImage, m_nWidth, m_nHeight, paletteimage, paletteheight, m_pScheme,
                                   m_nSchemeSize);
            AurTextureFreeImage(pImage, m_nWidth, m_nHeight, m_nBPP, m_nCompressedSize, m_nDataSize, m_nTxiSize,
                                (bool*)&m_bRasterAllocated, m_bProcessed, true);
            m_nBPP = 4;
        }
        m_fSortKey = 1.0f;
    }
    else
    {
        LoadImageA();
    }
    ClampMissingImageSize();
    if (m_pImage == NULL)
    {
        if (m_pController != NULL)
        {
            m_nWidth = m_nDefaultWidth;
            m_nHeight = m_nDefaultHeight;
            m_nBPP = 4;
            m_pImage = (unsigned char*)operator new(4 * m_nWidth * m_nHeight);
            memset(m_pImage, 0x7F, m_nBPP * m_nHeight * m_nWidth);
        }
        else
        {
            m_bInvalid = 1;
        }
    }
    ReduceToLevel();
    if (m_bEnvMapped != 0 && CAurTextureBasic::m_bLowEndEnvMapEnabled != 0)
    {
        ImageFillAlpha(m_pImage, m_nWidth, m_nHeight, m_nBPP, m_vLowEndEnvMapColor);
    }
    m_bFullyLoaded = 1;
}
#else
void CAurTextureBasic::ConstructImage()  // PC 0x00421180
{
    ImageGammaCorrect(m_nWidth, m_nHeight, m_nBPP, m_pImage, m_fGamma);
    if (m_pScheme != NULL)
    {
        LoadImagePLT();
        unsigned char* pImage = m_pImage;
        m_nCompressedSize = 0;
        if (pImage != NULL)
        {
            m_pImage = ImageDecode(pImage, m_nWidth, m_nHeight, paletteimage,
                                   paletteheight, m_pScheme, m_nSchemeSize);

            char& bRasterAllocated = m_bRasterAllocated;
            if (bRasterAllocated != 0)
            {
                AurReleaseLayeredRaster(pImage, m_nWidth, m_nHeight);
                bRasterAllocated = 0;
            }
            else
            {
                delete[] pImage;
            }
            m_nBPP = 4;
        }
        m_fSortKey = 1.0f;
    }
    else
    {
        LoadImageA();
    }
    if (m_pImage == NULL)
    {
        if (m_nBumpType != 0)
        {
            m_nBPP = 1;
        }
        m_nWidth = max(2, m_nWidth);
        m_nHeight = max(2, m_nHeight);
    }
    if (m_pImage == NULL)
    {
        if (m_pController != NULL)
        {
            m_nWidth = m_nDefaultWidth;
            m_nHeight = m_nDefaultHeight;
            m_nBPP = 4;
            m_pImage = new unsigned char[4 * m_nWidth * m_nHeight];
            memset(m_pImage, 0x7F, m_nWidth * m_nHeight * m_nBPP);
        }
        else
        {
            m_bInvalid = 1;
        }
    }
    if (m_bEnvMapped != 0 && CAurTextureBasic::m_bLowEndEnvMapEnabled != 0)
    {
        ImageFillAlpha(m_pImage, m_nWidth, m_nHeight, m_nBPP,
                       m_vLowEndEnvMapColor);
    }
    m_bFullyLoaded = 1;
}
#endif
// PC 0x00421330
unsigned char* ImageQuarterRotated(unsigned char* pImage, int nWidth,
                                   int nHeight, int nBPP, int nRotation)
{
    unsigned char* pDest = new unsigned char[nWidth * nHeight * nBPP];
    nRotation %= 4;
    if (nRotation < 0)
    {
        nRotation += 4;
    }
    for (int y = 0; y < nHeight; y++)
    {
        for (int x = 0; x < nWidth; x++)
        {
            int nY[4];
            int nX[4];
            nY[0] = y;
            nX[0] = x;
            nY[1] = x;
            nX[1] = nHeight - 1 - y;
            nY[2] = nHeight - 1 - y;
            nX[2] = nWidth - 1 - x;
            nY[3] = nWidth - 1 - x;
            nX[3] = y;
            memcpy(pDest + nBPP * (nX[nRotation] + nWidth * nY[nRotation]),
                   pImage + (x + y * nWidth) * nBPP, nBPP);
        }
    }
    return pDest;
}
// PC 0x00421460
void CAurTextureBasic::SetScheme(int* pScheme, int nCount)
{
    if (m_pScheme == NULL)
    {
        m_pScheme = new int[256];
    }
    for (int i = 0; i < 256; i++)
    {
        m_pScheme[i] = (i < nCount) ? pScheme[i] : 0;
    }
    m_nSchemeSize = nCount;
}

#include "aurrendergl.h"
#include "aurscenemanager.h"
#include "aurparse.h"
// PC 0x004065A0
int AurTextureSetDim(int nWidth, int nHeight)
{
    return 0;
}
// PC 0x00421600
int AurTextureGetMaxTexID()
{
    return maxtexid;
}
// PC 0x004015A0
void AurTextureClearAll()
{
}
// PC 0x00421660
Normal* convertHeightFieldToNormalMap(unsigned char* pHeightField,
                                             int nWidth, int nHeight,
                                             int nRowWidth, int nRowCount,
                                             float fScale)
{
    Normal* pNormalMap = new Normal[nWidth * nHeight];
    for (int y = 0; y < nHeight; y++)
    {
        for (int x = 0; x < nWidth; x++)
        {
            float fHeight = pHeightField[y * nRowWidth + x] * (1.0f / 255.0f);
            float fDX =
                (fHeight - pHeightField[y * nRowWidth + (x + 1) % nRowWidth] *
                               (1.0f / 255.0f)) *
                fScale;
            float fDY =
                (fHeight - pHeightField[((y + 1) % nRowCount) * nRowWidth + x] *
                               (1.0f / 255.0f)) *
                fScale;
            float fInvLength = 1.0f / (float)sqrt(fDY * fDY + fDX * fDX + 1.0f);
            Normal* pPixel = &pNormalMap[y * nWidth + x];
            float fNormalY = fInvLength * fDY;
            float fNormalX = -(fInvLength * fDX);
            pPixel->nx = (unsigned char)(fNormalY * 127.0f + 128.0f);
            pPixel->ny = (unsigned char)(fNormalX * 127.0f + 128.0f);
            pPixel->nz = (unsigned char)(fInvLength * 127.0f + 128.0f);
            pPixel->mag = 255;
        }
    }
    return pNormalMap;
}
// PC 0x004217D0
Normal* downSampleNormalMap(Normal* pNormalMap, int nWidth, int nHeight,
                           int nDestWidth, int nDestHeight)
{
    const float fDecodeComponent = 1.0 / 127.0;
    const float fDecodeMagnitude = 1.0 / 255.0;
    Normal* pDest;
    float fNX, fNY, fNZ, fLength, fInverseLength;
    float fM00, fM01, fM10, fM11;
    int y, x, nDestY, nDestX;

    pDest = new Normal[nDestWidth * nDestHeight];
    for (y = 0; y < nHeight; y += 2)
    {
        for (x = 0; x < nWidth; x += 2)
        {
            fM00 = fDecodeMagnitude * pNormalMap[y * nWidth + x].mag;
            fM01 = fDecodeMagnitude * pNormalMap[y * nWidth + (x + 1) % nHeight].mag;
            fM10 = fDecodeMagnitude * pNormalMap[((y + 1) % nWidth) * nWidth + x].mag;
            fM11 = fDecodeMagnitude * pNormalMap[((y + 1) % nWidth) * nWidth + (x + 1) % nHeight].mag;

            fNX = fM00 * (fDecodeComponent * pNormalMap[y * nWidth + x].nx - 1.0f);
            fNX += fM01 * (fDecodeComponent * pNormalMap[y * nWidth + (x + 1) % nHeight].nx - 1.0f);
            fNX += fM10 * (fDecodeComponent * pNormalMap[((y + 1) % nWidth) * nWidth + x].nx - 1.0f);
            fNX += fM11 * (fDecodeComponent * pNormalMap[((y + 1) % nWidth) * nWidth + (x + 1) % nHeight].nx - 1.0f);
            fNY = fM00 * (fDecodeComponent * pNormalMap[y * nWidth + x].ny - 1.0f);
            fNY += fM01 * (fDecodeComponent * pNormalMap[y * nWidth + (x + 1) % nHeight].ny - 1.0f);
            fNY += fM10 * (fDecodeComponent * pNormalMap[((y + 1) % nWidth) * nWidth + x].ny - 1.0f);
            fNY += fM11 * (fDecodeComponent * pNormalMap[((y + 1) % nWidth) * nWidth + (x + 1) % nHeight].ny - 1.0f);
            fNZ = fM00 * (fDecodeComponent * pNormalMap[y * nWidth + x].nz - 1.0f);
            fNZ += fM01 * (fDecodeComponent * pNormalMap[y * nWidth + (x + 1) % nHeight].nz - 1.0f);
            fNZ += fM10 * (fDecodeComponent * pNormalMap[((y + 1) % nWidth) * nWidth + x].nz - 1.0f);
            fNZ += fM11 * (fDecodeComponent * pNormalMap[((y + 1) % nWidth) * nWidth + (x + 1) % nHeight].nz - 1.0f);

            if (nDestWidth == 1 || nDestHeight == 1)
            {
                fNX = 0.0; fNY = 0.0; fNZ = 1.0;
            }
            else if (nDestWidth == 2 && nDestHeight == 2)
            {
                fNX = 0.0; fNY = 0.0; fNZ = 1.0;
            }
            else if (nDestWidth == 4 && nDestHeight == 4)
            {
                fNX *= 0.5f; fNY *= 0.5f;
            }
            fLength = (float)sqrt(fNX * fNX + fNY * fNY + fNZ * fNZ);
            if (fLength == 0.0)
            {
                fNX = 0.0; fNY = 0.0; fNZ = 1.0;
            }
            else
            {
                fInverseLength = 1.0f / fLength;
                fNX *= fInverseLength;
                fNY *= fInverseLength;
                fNZ *= fInverseLength;
            }
            nDestY = y >> 1;
            nDestX = x >> 1;
            pDest[nDestY * nDestWidth + nDestX].nx = (unsigned char)(128 + 127 * fNX);
            pDest[nDestY * nDestWidth + nDestX].ny = (unsigned char)(128 + 127 * fNY);
            pDest[nDestY * nDestWidth + nDestX].nz = (unsigned char)(128 + 127 * fNZ);
            fLength *= 0.25f;
            if (fLength > 1.0)
                pDest[nDestY * nDestWidth + nDestX].mag = 255;
            else
                pDest[nDestY * nDestWidth + nDestX].mag = (unsigned char)(255 * fLength);
        }
    }
    delete[] pNormalMap;
    return pDest;
}

#ifdef _XBOX

inline void GLRender::DeleteTextures(unsigned int nCount, const unsigned int* pnTextures)
{
    for (unsigned int i = 0; i < nCount; i++)
    {
        D3DBaseTexture* pTexture = g_listTextures.m_pList[pnTextures[i]];
        g_listTextures.m_pList[pnTextures[i]] = NULL;
        if (pTexture != NULL)
        {
            if (g_pCurrentTexture == pTexture)
            {
                g_pCurrentTexture = NULL;
                g_nCurrentTexture = 0;
                g_pCurrentTextureState = NULL;
            }
            int nType = g_listTextureStates.m_pList[pnTextures[i]]->m_nType;
            if (nType == 1)
            {
                g_pD3DDevice->SetTexture(0, NULL);
                g_pD3DDevice->SetTexture(1, NULL);
                g_pD3DDevice->SetTexture(2, NULL);
                g_pD3DDevice->SetTexture(3, NULL);
                delete pTexture;
            }
            else if (nType == 0)
            {
                pTexture->Release();
            }
        }
        if (g_listTextureStates.m_pList[pnTextures[i]] != NULL)
        {
            delete g_listTextureStates.m_pList[pnTextures[i]];
            g_listTextureStates.m_pList[pnTextures[i]] = NULL;
        }
    }
}

inline void GLRender::SetTextureParameteri(int nTarget, int nParam, int nValue)
{
    switch (nValue)
    {
    case 2:
        nValue = D3DTADDRESS_CLAMP;
        break;
    case 1:
        nValue = D3DTADDRESS_CLAMPTOEDGE;
        break;
    case 3:
    case 4:
        nValue = D3DTEXF_POINT;
        break;
    case 5:
        nValue = D3DTEXF_LINEAR;
        break;
    case 6:
        g_pD3DDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
        nValue = D3DTEXF_LINEAR;
        break;
    case 7:
        g_pD3DDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_POINT);
        nValue = D3DTEXF_POINT;
        break;
    }
    g_pD3DDevice->SetTextureStageState(cm_uiCurrentStage, (D3DTEXTURESTAGESTATETYPE)texParamTranslationTable[nParam],
                                       nValue);
}

inline void GLRender::GenTextures(unsigned int nCount, unsigned int* pnTextures)
{
    for (unsigned int i = 0; i < nCount; i++)
    {
        int nTexture;
        for (nTexture = 1; nTexture < g_listTextureStates.m_nCount; nTexture++)
        {
            if (g_listTextureStates.m_pList[nTexture] == NULL)
            {
                pnTextures[i] = nTexture;
                g_listTextureStates.m_pList[nTexture] = new DXTextureState;
                break;
            }
        }
        if (nTexture == g_listTextureStates.m_nCount)
        {
            g_listTextureStates.Add(new DXTextureState);
            g_listTextures.Add(NULL);
            pnTextures[i] = nTexture;
        }
    }
}

inline void GLRender::BindTexture(Aurora::AuroraEnableEnums nTarget, unsigned long nTexture)
{
    if (nTexture == 0)
    {
        g_pCurrentTextureState = NULL;
        g_pCurrentTexture = NULL;
        g_nCurrentTexture = 0;
        cm_pCurrentTexture[cm_uiCurrentStage] = NULL;
        g_pD3DDevice->SetTexture(cm_uiCurrentStage, NULL);
        SetTextureStageFlag(cm_uiCurrentStage, 0);
        return;
    }
    g_nCurrentTexture = nTexture;
    D3DBaseTexture* pTexture = g_listTextures.m_pList[nTexture];
    if (pTexture == NULL)
    {
        g_pCurrentTexture = NULL;
        g_pCurrentTextureState = g_listTextureStates.m_pList[nTexture];
        g_nCurrentTexture = nTexture;
        return;
    }
    pTexture->GetType();
    cm_pCurrentTexture[cm_uiCurrentStage] = pTexture;
    g_pCurrentTexture = pTexture;
    g_pCurrentTextureState = g_listTextureStates.m_pList[nTexture];
    int bCubeMap = g_pCurrentTextureState->m_nWrapR != 0;
    SetTextureParameteri(bCubeMap, 0, g_pCurrentTextureState->m_nWrapS);
    SetTextureParameteri(bCubeMap, 1, g_pCurrentTextureState->m_nWrapT);
    SetTextureParameteri(bCubeMap, 3, g_pCurrentTextureState->m_nMipFilter);
    SetTextureParameteri(bCubeMap, 2, g_pCurrentTextureState->m_nMinFilter);
    if (g_pCurrentTextureState->m_nMagFilter == 4)
    {
        g_pD3DDevice->SetTextureStageState(cm_uiCurrentStage, D3DTSS_MIPFILTER, D3DTEXF_POINT);
    }
    else
    {
        g_pD3DDevice->SetTextureStageState(cm_uiCurrentStage, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
    }
    g_pD3DDevice->SetTexture(cm_uiCurrentStage, pTexture);
    SetTextureStageFlag(cm_uiCurrentStage, 0);
    if (g_pCurrentTextureState->m_nWrapR != 0)
    {
        g_pD3DDevice->SetTextureStageState(cm_uiCurrentStage, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
        g_pD3DDevice->SetTextureStageState(cm_uiCurrentStage, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
        g_pD3DDevice->SetTextureStageState(cm_uiCurrentStage, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP);
    }
}

#else
inline int GLRender::BindTexture(Aurora::AuroraEnableEnums nTarget,  // PC 0x00421C40
                                 unsigned long nTexture)
{
    glBindTexture(enableTranslationTable[nTarget], nTexture);
    glTexParameteri(enableTranslationTable[nTarget], GL_TEXTURE_MAX_ANISOTROPY_EXT,
                    GLRender::cm_nAnisotropy);
    GLRender::cm_nGLCurrentTID[GLRender::cm_uiCurrentStage] = nTexture;
    return GLRender::cm_uiCurrentStage;
}
#endif
// PC 0x00421BE0
void AurTextureSuspendUpdates()
{
    g_bIsSuspended = true;
}
// PC 0x00421BF0
void AurTextureResumeUpdates()
{
    g_bIsSuspended = false;
}
// PC 0x00421C00
void AurTextureSetModeStatic()
{
    g_bModeIsDynamic = false;
}
// PC 0x00421C10
void AurTextureSetModeDynamic()
{
    g_bModeIsDynamic = true;
}
// PC 0x00421C20
bool AurTextureEnvMapDisabled()
{
    return CAurTextureBasic::m_bLowEndEnvMapEnabled;
}

#ifndef _XBOX
template <> inline SafePointer<CAurTexture>::SafePointer()  // PC 0x00421C30
    : m_pObject(0)
{
}
#endif

void AurTextureFreeAll()
{
    int i;

    for (i = 0; i < CAurTextureBasic::m_StaticTextureList.m_nCount; i++)
    {
        CAurTextureBasic* pTexture = CAurTextureBasic::m_StaticTextureList[i];
        if (pTexture->m_pImage != NULL)
        {
            pTexture->FreeImage();
        }
    }
    for (i = 0; i < CAurTextureBasic::m_DynamicTextureList.m_nCount; i++)
    {
        CAurTextureBasic* pTexture = CAurTextureBasic::m_DynamicTextureList[i];
        if (pTexture->m_pImage != NULL)
        {
            pTexture->FreeImage();
        }
    }
}

#ifndef _XBOX
inline void GetViewport(GLint* pViewport)
{
    glGetIntegerv(GL_VIEWPORT, pViewport);
}
#endif
#ifdef _XBOX
void AurSnapshot(char* sFileName)
#else
void AurSnapshot(char* sFileName, bool bScreenSize)
#endif
{
    static int k;
    char sBuffer[64];
    if (sFileName == NULL || strcmp(sFileName, "NULL") == 0)
    {
        sprintf(sBuffer, "snap%d.tga", ++k);
    }
    else
    {
        strcpy(sBuffer, sFileName);
        int nLen = strlen(sBuffer);
        if (nLen <= 3 || stricmp(sBuffer + nLen - 4, ".tga") != 0)
        {
            strcat(sBuffer, ".tga");
        }
    }
#ifdef _XBOX
    D3DVIEWPORT8 viewport;
    g_pD3DDevice->GetViewport(&viewport);
    int nWidth = viewport.Width;
#else
    GLint viewport[4];
    GetViewport(viewport);
    int nWidth = viewport[2];
    int nHeight = viewport[3];
#endif
    if (nWidth % 4 != 0)
    {
        nWidth = nWidth - nWidth % 4 + 4;
    }
#ifdef _XBOX
    {
        int nHeight = viewport.Height;
        int nSize = nHeight * nWidth * 3;
        unsigned char* pPixels = new unsigned char[nSize];
        memset(pPixels, 0, nSize);
#else
    if (bScreenSize)
    {
        unsigned char* pPixels = new unsigned char[3 * nWidth * nHeight];
        memset(pPixels, 0, 3 * nWidth * nHeight);
#endif
#ifdef _XBOX
        GLRender::ReadPixelsFromBuffer(0, 0, nWidth, nHeight, Aurora::TEXTURE_FORMAT_RGB, pPixels);
#else
        glReadPixels(0, 0, nWidth, nHeight, textureFormatTranslationTable[Aurora::TEXTURE_FORMAT_RGB], 0x1401, pPixels);
#endif
        ImageFlipY(pPixels, nWidth, nHeight, 3);
        ImageWriteTGA(sBuffer, nWidth, nHeight, 3, pPixels);
        delete[] pPixels;
    }
#ifndef _XBOX
    else {
        AurSaveGameSnapshot(sFileName, nWidth, false, false);
    }
#endif
}

#ifndef _XBOX
template <> inline void List<Vector>::allocate(int nAllocated)  // PC 0x005752B0
{
    Vector* pOldList = m_pList;
    m_nAllocated = nAllocated;
    m_pList = new Vector[nAllocated];
    for (int i = 0; i < m_nCount; i++)
        m_pList[i] = pOldList[i];
    if (pOldList != 0)
        delete[] pOldList;
}
// PC 0x00422AA0
template <> inline void List<Vector>::SetSize(int nSize)
{
    if (nSize == 0)
    {
        if (m_pList != 0)
        {
            delete[] m_pList;
            m_pList = 0;
        }
        m_nAllocated = 0;
    }
    else
    {
        allocate(nSize);
    }
    m_nCount = nSize;
}
// PC 0x00422AE0
template <> inline void List<Vector>::Add(Vector item)
{
    if (m_nCount == m_nAllocated)
    {
        allocate(m_nAllocated ? m_nAllocated * 2 : INITIAL_ALLOCATION);
    }
    m_pList[m_nCount++] = item;
}
#endif

#include "aurcamera.h"
// PC 0x00421C90
void AurTextureBindTID(unsigned int nTextureID, bool bCubeMap)
{
    bool bBound = false;
    if (AurCubeMapAvailable())
    {
        if (bCubeMap)
        {
            bBound = true;
            GLRender_Enable(ENABLE_TEXTURE_CUBE_MAP);
        }
        else
        {
            GLRender_Disable(ENABLE_TEXTURE_CUBE_MAP);
        }
    }
    if (nTextureID <= 1 || alwaysbind >= 0)
    {
        if (bBound)
        {
            GLRender::BindTexture(ENABLE_TEXTURE_CUBE_MAP, nTextureID);
            countbinds++;
        }
        else
        {
            GLRender::BindTexture(ENABLE_TEXTURE_2D, nTextureID);
            countbinds++;
        }
    }

}
// PC 0x00421D60
CAurTexture* CAurTexture::GetReferencedTexture()
{
    if (m_pReferenced.m_pObject != NULL)
    {
        return (CAurTexture*)m_pReferenced.m_pObject;
    }
    return this;
}
// PC 0x00421D70
unsigned char CAurTexture::IsInvalid()
{
    CAurTextureBasic* pTexture = GetReferencedTexture()->AsCAurTextureBasic();
    if (pTexture != NULL)
    {
        return pTexture->m_bInvalid;
    }
    return this != CAurTextureBasic::m_pNullTexture.m_pObject;
}
// PC 0x00421DA0
unsigned char CAurTexture::IsNull()
{
    return GetReferencedTexture() == CAurTextureBasic::m_pNullTexture.m_pObject;
}
// PC 0x00421E50
inline CAurFontInfo::~CAurFontInfo()
{
}

const char* savegamesnapshot(char* sCommand)
{
    char sName[64];
#ifdef _XBOX
    int nSize = 0;
#else
    int nSize;
#endif
    if (sscanf(sCommand, "%s %d", sName, &nSize) != 2) { return "Usage: savegamesnapshot imagename dimension"; }
#ifdef _XBOX
    AurSaveGameSnapshot(sName, nSize, true);
#else
    AurSaveGameSnapshot(sName, nSize, false, true);
#endif
    return "Attempted to save save game file";
}

Gob* FindGob(char* sName);

const char* replacetexture(char* sCommand)
{
    char sNew[64];
    unsigned short aParts[256];

    sNew[0] = 0;
    char* sOld = strchr(sCommand, ' ');
    for (int i = 0; i < 256; ++i)
    {
        aParts[i] = 0;
    }
    sprintf(returnvalue, "Failure");
    if (sOld == NULL)
    {
        return returnvalue;
    }
    *sOld = 0;
    Gob* pGob = FindGob(sCommand);
    if (pGob == NULL)
    {
        sprintf(returnvalue, "ERROR:  Unable to find gob with name %s", sCommand);
        return returnvalue;
    }
    sscanf(sOld, "%s", sNew);
#ifdef _XBOX
    if (stricmp(sNew, "NULL") == 0)
#else
    if (strcmp(sNew, "NULL") == 0)
#endif
    {
        sNew[0] = 0;
    }
#ifdef _XBOX

    char* sSpace = strchr(sOld + 1, ' ');
    if (sSpace == NULL)
    {
        return returnvalue;
    }
    *sSpace = 0;
    char* sTexture = sSpace + 1;
    sSpace = strchr(sTexture, ' ');
    if (sSpace != NULL)
    {
        char* sParts = sSpace + 1;
        *sSpace = 0;
        unsigned char nCount = 0;
        char* sNext = strchr(sParts, ' ');
        while (sNext != NULL)
        {
            sNext = strchr(sNext + 1, ' ');
            ++nCount;
        }
        nCount = (unsigned char)(nCount + 1) >> 1;
        if (nCount == 0)
        {
            return returnvalue;
        }
        for (int nPart = 0; nPart < nCount; ++nPart)
        {
            char* sEnd = strchr(sParts, ' ');
            *sEnd = 0;
            unsigned char nHigh = (unsigned char)atoi(sParts);
            sParts = sEnd + 1;
            sEnd = strchr(sParts, ' ');
            if (sEnd != NULL)
            {
                *sEnd = 0;
            }
            unsigned char nLow = (unsigned char)atoi(sParts);
            sParts = sEnd + 1;
            aParts[nPart] = (unsigned short)(nHigh * 256 + nLow);
        }
        if (pGob->ReplaceTexture(sNew, sTexture, nCount, aParts, 0) != 0)
        {
            sprintf(returnvalue, "Success: %i", usedtexturememory);
        }
        else
        {
            sprintf(returnvalue, "WARNING: no parts on model use texture %s", sNew);
        }
    }
    else
    {
        if (pGob->ReplaceTexture(sNew, sTexture) != 0)
        {
            sprintf(returnvalue, "Success: %i", usedtexturememory);
        }
        else
        {
            sprintf(returnvalue, "WARNING: no parts on model use texture %s", sNew);
        }
    }
    return returnvalue;
#else
    char* sTexture = strchr(sOld + 1, ' ');
    if (sTexture == NULL)
    {
        return returnvalue;
    }
    *sTexture++ = 0;
    char* sParts = strchr(sTexture, ' ');
    int nReplaced;
    if (sParts == NULL)
    {
        nReplaced = pGob->ReplaceTexture(sNew, sTexture);
    }
    else
    {
        *sParts++ = 0;
        unsigned char nCount = 0;
        for (char* sNext = strchr(sParts, ' '); sNext != NULL;
             sNext = strchr(sNext + 1, ' '))
        {
            ++nCount;
        }
        nCount = (unsigned char)(nCount + 1) >> 1;
        if (nCount == 0)
        {
            return returnvalue;
        }
        for (int nPart = 0; nPart < nCount; ++nPart)
        {
            char* sEnd = strchr(sParts, ' ');
            *sEnd = 0;
            unsigned char nHigh = (unsigned char)atoi(sParts);
            sParts = sEnd + 1;
            sEnd = strchr(sParts, ' ');
            if (sEnd != NULL)
            {
                *sEnd = 0;
            }
            aParts[nPart] = (unsigned short)(nHigh * 256 + (unsigned char)atoi(sParts));
            sParts = sEnd + 1;
        }
        nReplaced = pGob->ReplaceTexture(sNew, sTexture, nCount, aParts, 0);
    }
    if (nReplaced != 0)
    {
        sprintf(returnvalue, "Success: %i", nReplaced);
    }
    else
    {
        sprintf(returnvalue, "WARNING: no parts on model use texture %s", sNew);
    }
    return returnvalue;
#endif
}
// PC 0x00423B00
void CAurTextureBasic::ParseField(char* sField)
{
    char sProcedureType[64];
    sProcedureType[0] = 0;
    if (stricmp("proceduretype", firstword(sField)) == 0)
    {
        Parse(sField + 13, sProcedureType);
    }
    if (strlen(sProcedureType) != 0)
    {
        if (m_pController != NULL)
        {
            delete m_pController;
            m_pController = NULL;
        }
        if (strcmp(sProcedureType, "water") == 0)
        {
            m_pController =
                (TextureController*)new WaterTextureController(this);
        }
        if (strcmp(sProcedureType, "life") == 0)
        {
            m_pController = (TextureController*)new LifeTextureController(this);
        }
        if (strcmp(sProcedureType, "perlin") == 0)
        {
            m_pController =
                (TextureController*)new PerlinTextureController(this);
        }
        if (strcmp(sProcedureType, "arturo") == 0)
        {
            m_pController =
                (TextureController*)new ArturoTextureController(this);
        }
        if (strcmp(sProcedureType, "wave") == 0)
        {
            m_pController = (TextureController*)new WaveTextureController(this);
        }
        if (strcmp(sProcedureType, "cycle") == 0)
        {
            m_pController =
                (TextureController*)new CycleTIDTextureController(this);
        }
        if (strcmp(sProcedureType, "random") == 0)
        {
            m_pController =
                (TextureController*)new RandomTextureController(this);
        }
        if (strcmp(sProcedureType, "ringtexdistort") == 0)
        {
            m_pController =
                (TextureController*)new RingDistortTextureController(this);
        }
#ifdef _XBOX
        if (strcmp(sProcedureType, "heattexdistort") == 0)
        {
            m_pController = (TextureController*)new HeatDistortTextureController(this);
        }
#endif
    }
    if (stricmp("filerange", firstword(sField)) == 0)
    {
        Parse(sField + 9, m_nFileRange);
    }
    if (stricmp("defaultwidth", firstword(sField)) == 0)
    {
        Parse(sField + 12, m_nDefaultWidth);
    }
    if (stricmp("defaultheight", firstword(sField)) == 0)
    {
        Parse(sField + 13, m_nDefaultHeight);
    }
    if (stricmp("downsamplemax", firstword(sField)) == 0)
    {
        Parse(sField + 13, m_nMaxLevel);
    }
    if (stricmp("downsamplemin", firstword(sField)) == 0)
    {
        Parse(sField + 13, m_nBaseLevel);
    }
    if (stricmp("mipmap", firstword(sField)) == 0)
    {
        Parse(sField + 6, (bool&)m_bMipMap);
    }
    if (stricmp("filter", firstword(sField)) == 0)
    {
        Parse(sField + 6, (bool&)m_bFilter);
    }
    if (stricmp("maptexelstopixels", firstword(sField)) == 0)
    {
        Parse(sField + 17, (bool&)m_bMapTexelsToPixels);
    }
    if (stricmp("gamma", firstword(sField)) == 0)
    {
        Parse(sField + 5, m_fGamma);
    }
    if (stricmp("isbumpmap", firstword(sField)) == 0)
    {
        Parse(sField + 9, m_nBumpType);
    }
    if (stricmp("clamp", firstword(sField)) == 0)
    {
        Parse(sField + 5, m_nClamp);
    }
    if (stricmp("alphamean", firstword(sField)) == 0)
    {
        Parse(sField + 9, m_fSortKey);
    }
    if (stricmp("isdiffusebumpmap", firstword(sField)) == 0)
    {
        Parse(sField + 16, (bool&)m_bDiffuseBumpMap);
    }
    if (stricmp("isspecularbumpmap", firstword(sField)) == 0)
    {
        Parse(sField + 17, (bool&)m_bSpecularBumpMap);
    }
    if (stricmp("bumpmapscaling", firstword(sField)) == 0)
    {
        Parse(sField + 14, m_fBumpMapScaling);
    }
    if (stricmp("specularcolor", firstword(sField)) == 0)
    {
        Parse(sField + 13, m_vSpecularColor);
    }
    if (stricmp("numx", firstword(sField)) == 0)
    {
        Parse(sField + 4, m_nNumX);
    }
    if (stricmp("numy", firstword(sField)) == 0)
    {
        Parse(sField + 4, m_nNumY);
    }
    if (stricmp("cube", firstword(sField)) == 0)
    {
        Parse(sField + 4, (bool&)m_bCubeMap);
    }
    if (stricmp("bumpintensity", firstword(sField)) == 0)
    {
        Parse(sField + 13, m_fBumpIntensity);
    }
    if (stricmp("temporary", firstword(sField)) == 0)
    {
        Parse(sField + 9, (bool&)m_bTemporary);
    }
    if (stricmp("useglobalalpha", firstword(sField)) == 0)
    {
        Parse(sField + 14, (bool&)m_bUsingGlobalAlpha);
    }
    if (stricmp("isenvironmentmapped", firstword(sField)) == 0)
    {
        Parse(sField + 19, (bool&)m_bEnvMapped);
    }
    if (stricmp("envmapalpha", firstword(sField)) == 0)
    {
        Parse(sField + 11, m_fEnvMapAlpha);
    }
    if (stricmp("diffusebumpintensity", firstword(sField)) == 0)
    {
        Parse(sField + 20, m_fDiffuseBumpIntensity);
    }
    if (stricmp("specularbumpintensity", firstword(sField)) == 0)
    {
        Parse(sField + 21, m_fSpecularBumpIntensity);
    }
    m_pFontInfo->ParseField(sField);
    if (m_pController != NULL)
    {
        m_pController->ParseField(sField);
    }
    if (m_nBaseLevel > 0)
    {
        m_nLevel = m_nBaseLevel;
    }
}
// PC 0x00422B30
int CAurTexture::glImage(bool bMipMaps, unsigned char* pImage)
{
    CAurTextureBasic* pTexture = GetReferencedTexture()->AsCAurTextureBasic();
    int nResult = 0;
    if (pTexture != NULL)
    {
        if (pImage != NULL)
        {
            pTexture->m_pImage = pImage;
            nResult = pTexture->glImage(bMipMaps);
            pTexture->m_pImage = NULL;
        }
        else
        {
            return pTexture->glImage(bMipMaps);
        }
    }
    return nResult;
}
// PC 0x00422B80
void CAurTexture::SetIsEnvironmentMapped(bool bMapped)
{
    CAurTextureBasic* pTexture = GetReferencedTexture()->AsCAurTextureBasic();
    if (pTexture != NULL)
    {
        pTexture->m_bEnvMapped = bMapped;
        if (bMapped)
        {
            RefreshTextures.AddUnique(pTexture);
        }
    }
}
// PC 0x00422BB0
void CAurTexture::Bind()
{
    if (bindzero)
    {
        AurTextureBindTID(0, false);
        return;
    }
    if (IsNull())
    {
        AurTextureBindTID(0, false);
        return;
    }
    CAurTextureBasic* pBasic = AsCAurTextureBasic();
    if (pBasic == NULL)
    {
        pBasic = GetReferencedTexture()->AsCAurTextureBasic();
    }
    if (pBasic != NULL)
    {
        static unsigned int nCurrentTID;
        nCurrentTID = AurTextureGetTID(pBasic);
#ifdef _XBOX

        if (nCurrentTID != GLRender::GetCurrentStageTID())
        {
            AurTextureBindTID(nCurrentTID, pBasic->IsCubeMap());
            TextureController* pController = AurTextureGetController(pBasic);
            if (pController != NULL)
            {
                if (enabletexturecontrollers)
                {
                    pController->Control();
                }
            }
        }
#else
        AurTextureBindTID(nCurrentTID, pBasic->IsCubeMap());
        CAurTextureBasic* pReferenced =
            pBasic->GetReferencedTexture()->AsCAurTextureBasic();
        TextureController* pController = pReferenced->m_pController;
        if (pController != NULL)
        {
            if (enabletexturecontrollers)
            {
                pController->Control();
            }
        }
#endif
        pBasic->m_bControllerDirty = 1;
    }
    else
    {
        AurTextureBindTID(0, false);
    }
}
// PC 0x00422CC0
void CAurTextureBasic::AddToOrderedLists()
{
    int i = 0;
#ifdef _XBOX
    int nCount = m_OrderedTextureList.m_nCount;
    CAurTextureBasic** ppList = m_OrderedTextureList.m_pList;
    while (i < nCount && m_nOrder > ppList[i]->m_nOrder)
    {
        i++;
    }
    while (i < nCount && ppList[i]->m_nOrder == m_nOrder)
    {
        i++;
    }
#else
    if (m_OrderedTextureList.m_nCount > 0)
    {
        while (i < m_OrderedTextureList.m_nCount &&
               m_nOrder > m_OrderedTextureList[i]->m_nOrder)
        {
            i++;
        }
        while (i < m_OrderedTextureList.m_nCount &&
               m_OrderedTextureList[i]->m_nOrder == m_nOrder)
        {
            i++;
        }
    }
#endif
    m_OrderedTextureList.Insert(this, i);
#ifdef _XBOX

    if (m_bDynamic != 0)
    {
        m_DynamicTextureList.Add(this);
    }
    else
    {
        m_StaticTextureList.Add(this);
    }
#else
    if (m_bDynamic != 0)
    {
        if (m_DynamicTextureList.m_nCount == m_DynamicTextureList.m_nAllocated)
        {
            m_DynamicTextureList.allocate(
                m_DynamicTextureList.m_nAllocated != 0
                    ? 2 * m_DynamicTextureList.m_nAllocated
                    : List<CAurTextureBasic*>::INITIAL_ALLOCATION);
        }
        m_DynamicTextureList.m_pList[m_DynamicTextureList.m_nCount] = this;
        m_DynamicTextureList.m_nCount++;
    }
    else
    {
        if (m_StaticTextureList.m_nCount == m_StaticTextureList.m_nAllocated)
        {
            m_StaticTextureList.allocate(
                m_StaticTextureList.m_nAllocated != 0
                    ? 2 * m_StaticTextureList.m_nAllocated
                    : List<CAurTextureBasic*>::INITIAL_ALLOCATION);
        }
        m_StaticTextureList.m_pList[m_StaticTextureList.m_nCount] = this;
        m_StaticTextureList.m_nCount++;
    }
#endif
}
// PC 0x00422DB0
void CAurTextureBasic::RemoveFromOrderedLists()
{
    if (m_OrderedTextureList.Contains(this))
    {
        m_OrderedTextureList.Remove(this);
    }
    if (m_bDynamic != 0)
    {
        m_DynamicTextureList.Remove(this);
    }
    else
    {
        m_StaticTextureList.Remove(this);
    }
}
// PC 0x00422E00
void CAurTextureBasic::BuildAndStoreTexture()
{
    if (m_bSpecial != 0)
    {
        if (strcmp(m_sName, "NormCubeMap") == 0)
        {
            AsNormalizeVectorCubeMap();
        }
        return;
    }
    if (!m_bFullyLoaded)
    {
        ConstructImage();
    }
#ifdef _XBOX
    else
    {
        ReduceToLevel();
    }
#endif
    if (m_pImage != NULL)
    {
        glImage(false);
        if (m_fSortKey == -1.0f)
        {
            if (m_nCompressedSize > 0)
            {
                m_fSortKey = 1.0f;
            }
            else
            {
                m_fSortKey =
                    ImageGetAlphaMean(m_pImage, m_nWidth, m_nHeight, m_nBPP);
            }
        }
    }
    if (m_nCompressedSize == 0 && m_pController != NULL)
    {
        m_pController->Init();
    }
    if (!IsProcessed())
    {
        FreeImage();
    }
}
// PC 0x00422EB0
void AurTextureSetLayered(CAurTexture* pTexture, char* sExtra, int* pTXI,
                          int nNumTXI)
{
    if (pTexture == NULL)
    {
        return;
    }
    CAurTextureBasic* pBasic =
        pTexture->GetReferencedTexture()->AsCAurTextureBasic();
    if (sExtra != NULL && *sExtra != 0)
    {
        strcpy(pBasic->m_sFileName, sExtra);
    }
    pBasic->SetScheme(pTXI, nNumTXI);
    pBasic->m_nBPP = 4;
#ifdef _XBOX
    pBasic->CalculateHashID();
#else
    pBasic->m_nOrder = AurTextureCalculateHashID(
        pBasic->m_sFileName, pBasic->m_nSchemeSize, pBasic->m_pScheme);
#endif
    pBasic->RemoveFromOrderedLists();
    pBasic->AddToOrderedLists();
    RefreshTextures.AddUnique(pBasic);
}

void AurTextureRefreshLowEndEnvMapped()
{
    for (int i = 0; i < CAurTextureBasic::m_StaticTextureList.m_nCount; i++)
    {
        CAurTextureBasic* pTexture = CAurTextureBasic::m_StaticTextureList[i];
        if (pTexture->m_bEnvMapped)
        {
            pTexture->Reset();
            RefreshTextures.Add(pTexture);
        }
    }
    for (int j = 0; j < CAurTextureBasic::m_DynamicTextureList.m_nCount; j++)
    {
        CAurTextureBasic* pTexture = CAurTextureBasic::m_DynamicTextureList[j];
        if (pTexture->m_bEnvMapped)
        {
            pTexture->Reset();
            RefreshTextures.Add(pTexture);
        }
    }
}

void AurTextureEnableLowEndEnvMapping(Vector vColor)
{
    CAurTextureBasic::m_vLowEndEnvMapColor = vColor;
    CAurTextureBasic::m_bLowEndEnvMapEnabled = true;
#ifdef _XBOX
    int i;
    for (i = 0; i < CAurTextureBasic::m_StaticTextureList.m_nCount; i++)
    {
        if (CAurTextureBasic::m_StaticTextureList[i]->m_bEnvMapped)
        {
            CAurTextureBasic::m_StaticTextureList[i]->Reset();
            RefreshTextures.Add(CAurTextureBasic::m_StaticTextureList[i]);
        }
    }
    for (i = 0; i < CAurTextureBasic::m_DynamicTextureList.m_nCount; i++)
    {
        if (CAurTextureBasic::m_DynamicTextureList[i]->m_bEnvMapped)
        {
            CAurTextureBasic::m_DynamicTextureList[i]->Reset();
            RefreshTextures.Add(CAurTextureBasic::m_DynamicTextureList[i]);
        }
    }
#else
    AurTextureRefreshLowEndEnvMapped();
#endif
}

void AurTextureDisableLowEndEnvMapping()
{
    CAurTextureBasic::m_bLowEndEnvMapEnabled = false;
#ifdef _XBOX
    int i;
    for (i = 0; i < CAurTextureBasic::m_StaticTextureList.m_nCount; i++)
    {
        if (CAurTextureBasic::m_StaticTextureList[i]->m_bEnvMapped)
        {
            CAurTextureBasic::m_StaticTextureList[i]->Reset();
            RefreshTextures.Add(CAurTextureBasic::m_StaticTextureList[i]);
        }
    }
    for (i = 0; i < CAurTextureBasic::m_DynamicTextureList.m_nCount; i++)
    {
        if (CAurTextureBasic::m_DynamicTextureList[i]->m_bEnvMapped)
        {
            CAurTextureBasic::m_DynamicTextureList[i]->Reset();
            RefreshTextures.Add(CAurTextureBasic::m_DynamicTextureList[i]);
        }
    }
#else
    AurTextureRefreshLowEndEnvMapped();
#endif
}

const char* enablelowendenvmapping(char* sCommand)
{
    Vector vColor;

    if (sscanf(sCommand, "%f %f %f", &vColor.m_fX, &vColor.m_fY, &vColor.m_fZ) != 3)
    {
        return "Usage: enablelowendenvmapping red green blue";
    }
    AurTextureEnableLowEndEnvMapping(vColor);
    return "Success.";
}

const char* disablelowendenvmapping(char* sCommand)
{
    AurTextureDisableLowEndEnvMapping();
    return "Success.";
}

int sorttexturelist(const void* pA, const void* pB)
{
#ifdef _XBOX
    return strcmp((const char*)pA, (const char*)pB);
#else
    int nResult = strcmp((const char*)pA, (const char*)pB);
    if (nResult != 0)
    {
        return nResult < 0 ? -1 : 1;
    }
    return nResult;
#endif
}

const char* listalltextures(char* sCommand)
{
    char sLines[1000][50];
    char sSpaces[33];
    TEXTUREMEMORYSTATS stats;
    int nCount = 0;
    int nTotal = 0;
    int i, j;

    strcpy(sSpaces, "                                ");
#ifdef _XBOX
    FILE* pFile = fopen("d:\\texturelist.txt", "wa");
#else
    FILE* pFile = fopen("texturelist.txt", "wa");
#endif
    if (pFile == NULL)
    {
        return "Failure.";
    }
    for (i = 0; i < CAurTextureBasic::m_OrderedTextureList.m_nCount; i++)
    {
        CAurTextureBasic* pTexture = CAurTextureBasic::m_OrderedTextureList[i];
        if (strstr(pTexture->m_sFileName, "_lm") == NULL &&
            strstr(pTexture->m_sFileName, "_a00") == NULL)
        {
            pTexture->GetMemoryUsage(&stats);
#ifdef _XBOX
            sprintf(sLines[nCount], "%s.txb - %d\n", pTexture->m_sFileName, stats.m_nTotal);
#else
            if (pTexture->GetCompressedSize() != 0)
            {
                sprintf(sLines[nCount], "%s.dds - %d\n", pTexture->m_sFileName,
                        stats.m_nTotal);
            }
            else
            {
                sprintf(sLines[nCount], "%s.tga - %d\n", pTexture->m_sFileName,
                        stats.m_nTotal);
            }
#endif
            nCount++;
            nTotal += stats.m_nTotal;
        }
    }
    qsort(sLines, nCount, 50, sorttexturelist);
    for (i = 0; i < nCount; i++)
    {
        int nLength = strlen(sLines[i]);
        for (j = 0; j < nLength; j++)
        {
            if (sLines[i][j] == '.' &&
                (sLines[i][j + 1] == 'd' || sLines[i][j + 1] == 't'))
            {
                break;
            }
        }
        fwrite(sSpaces, 1, 32 - j, pFile);
        fwrite(sLines[i], 1, strlen(sLines[i]), pFile);
    }
    sprintf(sLines[0], "\n\n                                %d - %d\n", nCount, nTotal);
    fwrite(sLines[0], 1, strlen(sLines[0]), pFile);
    fclose(pFile);
    return "Success.";
}

static TEXTUREMEMORYSTATS g_staticTMS;
static TEXTUREMEMORYSTATS g_dynamicTMS;
// PC 0x00423930
CAurFontInfo::CAurFontInfo() : m_lstUpperLeft(0), m_lstLowerRight(0)
{
    m_nNumChars = 0;
    m_fHeight = 0.0f;
    m_fBaselineHeight = 0.0f;
    m_fTextureWidth = 0.0f;
    m_fSpacingR = 0.0f;
    m_fLineSpacing = 0.0f;
}
// PC 0x00423980
void CAurFontInfo::ParseField(char* sField)
{
    if (stricmp("numchars", firstword(sField)) == 0)
    {
        Parse(sField + 8, m_nNumChars);
    }
    if (stricmp("fontheight", firstword(sField)) == 0)
    {
        Parse(sField + 10, m_fHeight);
    }
    if (stricmp("baselineheight", firstword(sField)) == 0)
    {
        Parse(sField + 14, m_fBaselineHeight);
    }
    if (stricmp("texturewidth", firstword(sField)) == 0)
    {
        Parse(sField + 12, m_fTextureWidth);
    }
    if (stricmp("spacingR", firstword(sField)) == 0)
    {
        Parse(sField + 8, m_fSpacingR);
    }
    if (stricmp("spacingB", firstword(sField)) == 0)
    {
        Parse(sField + 8, m_fLineSpacing);
    }
    if (stricmp("upperleftcoords", firstword(sField)) == 0)
    {
        Parse(sField + 15, m_lstUpperLeft);
    }
    if (stricmp("lowerrightcoords", firstword(sField)) == 0)
    {
        Parse(sField + 16, m_lstLowerRight);
    }
}
// PC 0x00423AD0
void AurTextureRefreshAll()
{
    if (initialized)
    {
        AurFreeFont();
        AurInitFont();
    }
    AurTextureSetMem(maxtexmem);
}

unsigned int AurTextureGetTID(CAurTexture* pTexture);
// PC 0x00422F50
void AurTextureBuildAndStoreAll()
{
    int nCount = RefreshTextures.m_nCount;
    if (nCount != 0)
    {
        do
        {
            nCount--;
            RefreshTextures[nCount]->BuildAndStoreTexture();
            RefreshTextures.m_nCount = nCount;
        } while (nCount != 0);
        InsertionSortTexture(CAurTextureBasic::m_StaticTextureList.m_pList,
                             CAurTextureBasic::m_StaticTextureList.m_nCount);
        basictexturecount = CAurTextureBasic::m_OrderedTextureList.m_nCount;
        TextureIndices.m_nCount = 0;
        maxtexid = 0;
        for (int i = 0; i < CAurTextureBasic::m_StaticTextureList.m_nCount; i++)
        {
            CAurTextureBasic* pTexture =
                CAurTextureBasic::m_StaticTextureList[i];
            if (!pTexture->m_bInvalid)
            {
                for (int j = 0; j < pTexture->m_nNumX * pTexture->m_nNumY; j++)
                {
                    unsigned int nTID = pTexture->m_pTextureIDs[j];
                    TextureIndices.Add(nTID);
                    if (pTexture->m_pTextureIDs[j] > (unsigned int)maxtexid)
                    {
                        maxtexid = pTexture->m_pTextureIDs[j];
                    }
                }
            }
        }
    }
}
// PC 0x004230E0
List<int>* AurTextureGetOrdering()
{
    TextureIndices.m_nCount = 0;
    maxtexid = 0;
    for (int i = 0; i < CAurTextureBasic::m_StaticTextureList.m_nCount; i++)
    {
        CAurTextureBasic* pTexture = CAurTextureBasic::m_StaticTextureList[i];
        if (!pTexture->m_bInvalid)
        {
            for (int j = 0; j < pTexture->m_nNumX * pTexture->m_nNumY; j++)
            {
                int nTID = pTexture->m_pTextureIDs[j];
                if (nTID < 5000 && nTID > 0)
                {
                    TextureIndices.Add(nTID);
                    if (pTexture->m_pTextureIDs[j] > (unsigned int)maxtexid)
                    {
                        maxtexid = pTexture->m_pTextureIDs[j];
                    }
                }
            }
        }
    }
    return &TextureIndices;
}

#ifndef _XBOX
int AurTextureGetAvailableTextureMemory()
{
    for (int i = 0; i < CAurTextureBasic::m_OrderedTextureList.m_nCount; i++)
    {
        CAurTextureBasic* pTexture = CAurTextureBasic::m_OrderedTextureList[i];
        if (pTexture->m_pTextureIDs != NULL)
        {
            GLRender_DeleteTextures(pTexture->m_nNumX * pTexture->m_nNumY,
                             pTexture->m_pTextureIDs);
            delete[] pTexture->m_pTextureIDs;
            pTexture->m_pTextureIDs = NULL;
        }
        pTexture->m_nTIDIndex = 0;
    }
    int nMemory = GLRender::GetAvailableTextureMemory();
    RefreshTextures = CAurTextureBasic::m_OrderedTextureList;
    AurTextureBuildAndStoreAll();
    return nMemory;
}
#endif
// PC 0x00423230
void AurTextureMemCheck(List<CAurTextureBasic*>& lstTextures, int nBudget,
                        TEXTUREMEMORYSTATS* pStats, float fRatio)
{
    AurTextureCalculateNewTexMem(lstTextures, pStats);
    int nMemory = pStats->m_nTotal;
#ifdef _XBOX
    float fLimit = (float)nBudget * fRatio;
#define fRatio fLimit
#else
    fRatio = (float)nBudget * fRatio;
#endif
    bool bOverflow = false;
    if ((float)nMemory > fRatio)
    {
        currentoverflow += DeltaT;
        if (currentoverflow > maxtimeformemoryoverflow)
        {
            bOverflow = true;
        }
    }
    else
    {
        currentoverflow = 0.0f;
    }
    if (bOverflow || nMemory > nBudget)
    {
        currentoverflow = 0.0f;
        TEXTUREMEMORYSTATS stats;
        while ((float)nMemory > fRatio)
        {
            CAurTextureBasic* pWorst = NULL;
            int nWorstSize = 0;
            int nWorstLevels = 0;
            for (int i = 0; i < lstTextures.m_nCount; i++)
            {
                CAurTextureBasic* pTexture = lstTextures[i];
#ifdef _XBOX
                if (pTexture->CanReduceLevel())
#else
                int nShift = pTexture->m_nLevel - pTexture->m_nUploadedLevels;
                if (pTexture->m_nLevel < pTexture->m_nMaxLevel &&
                    (pTexture->m_nWidth >> nShift) > 2 &&
                    (pTexture->m_nHeight >> nShift) > 2)
#endif
                {
                    int nLevelsLeft = pTexture->m_nMaxLevel - pTexture->m_nLevel;
                    pTexture->GetMemoryUsage(&stats);
                    if (stats.m_nTotal > nWorstSize ||
                        (stats.m_nTotal == nWorstSize && nLevelsLeft > nWorstLevels))
                    {
                        nWorstLevels = nLevelsLeft;
                        nWorstSize = stats.m_nTotal;
                        pWorst = pTexture;
                    }
                }
            }
            if (pWorst == NULL)
            {
                break;
            }
            pWorst->GetMemoryUsage(&stats);
            nMemory -= stats.m_nTotal;
            pStats->m_nBumpMap -= stats.m_nBumpMap;
            pStats->m_nStatic -= stats.m_nStatic;
            pStats->m_nCubeMap -= stats.m_nCubeMap;
            pStats->m_nInterface -= stats.m_nInterface;
            pStats->m_nTotal -= stats.m_nTotal;
#ifdef _XBOX
            pWorst->RaiseLevel(1);
#else
            pWorst->m_nLevel =
                min(pWorst->m_nLevel + 1, pWorst->m_nMaxLevel);
            if (pWorst->m_nLevel != pWorst->m_nUploadedLevels)
            {
                pWorst->m_bMemoryDirty = 1;
                RefreshTextures.AddUnique(pWorst);
            }
#endif
            pWorst->GetMemoryUsage(&stats);
            nMemory += stats.m_nTotal;
            pStats->m_nBumpMap += stats.m_nBumpMap;
            pStats->m_nStatic += stats.m_nStatic;
            pStats->m_nCubeMap += stats.m_nCubeMap;
            pStats->m_nInterface += stats.m_nInterface;
            pStats->m_nTotal += stats.m_nTotal;
        }
    }
#ifdef _XBOX
#undef fRatio
#endif
}
// PC 0x004234C0
int AurTextureResetAll()
{
    RefreshTextures = CAurTextureBasic::m_OrderedTextureList;
    int i;
    for (i = 0; i < RefreshTextures.m_nCount; i++)
    {
        RefreshTextures[i]->Reset();
    }
    return RefreshTextures.m_nCount;
}
#ifdef _XBOX  // PC 0x00423500
int AurTextureSetMem(int nMem)
#else
bool AurTextureSetMem(int nMem)
#endif
{
    maxtexmem = nMem;
    roundToInt(nMem, nMem * g_fDynamicMemoryRatio);
    RefreshTextures = CAurTextureBasic::m_OrderedTextureList;
    for (int i = 0; i < RefreshTextures.m_nCount; i++)
    {
        RefreshTextures[i]->Reset();
    }
    AurTextureMemCheck(CAurTextureBasic::m_DynamicTextureList, nMem, &g_dynamicTMS, memcheckratio);
    useddynamictexturememory = g_dynamicTMS.m_nTotal;
    AurTextureMemCheck(CAurTextureBasic::m_StaticTextureList, maxtexmem - nMem, &g_staticTMS, 1.0f);
    usedstatictexturememory = g_staticTMS.m_nTotal;
    usedbumpmapmemory = g_dynamicTMS.m_nBumpMap + g_staticTMS.m_nBumpMap;
    usedcomptexmemory =
        g_dynamicTMS.m_nStatic + g_staticTMS.m_nStatic;
    usedcubemapmemory =
        g_dynamicTMS.m_nCubeMap + g_staticTMS.m_nCubeMap;
    usedlightmapmemory =
        g_dynamicTMS.m_nInterface + g_staticTMS.m_nInterface;
    usedtexturememory = usedstatictexturememory + useddynamictexturememory;
    RefreshTextures = CAurTextureBasic::m_OrderedTextureList;
    AurTextureBuildAndStoreAll();
    return usedtexturememory <= maxtexmem;
}
// PC 0x00423650
int AurTextureMakeCurrent()
{
    if (!g_bIsSuspended)
    {
        int nDynamicBudget;
        roundToInt(nDynamicBudget, maxtexmem * g_fDynamicMemoryRatio);
        constructimagetime = 0;
        decodeimagetime = 0;
        glimagetime = 0;
#ifdef _XBOX

        if (useddynamictexturememory > nDynamicBudget || RefreshTextures.m_nCount != 0)
        {
            AurTextureMemCheck(CAurTextureBasic::m_DynamicTextureList, nDynamicBudget, &g_dynamicTMS, memcheckratio);
            useddynamictexturememory = g_dynamicTMS.m_nTotal;
            usedtexturememory = usedstatictexturememory + useddynamictexturememory;
            usedbumpmapmemory = g_staticTMS.m_nBumpMap + g_dynamicTMS.m_nBumpMap;
            usedcomptexmemory = g_staticTMS.m_nStatic + g_dynamicTMS.m_nStatic;
            usedcubemapmemory = g_staticTMS.m_nCubeMap + g_dynamicTMS.m_nCubeMap;
            usedlightmapmemory = g_staticTMS.m_nInterface + g_dynamicTMS.m_nInterface;
            if (RefreshTextures.m_nCount != 0)
            {
                AurTextureMemCheck(CAurTextureBasic::m_StaticTextureList, maxtexmem - nDynamicBudget, &g_staticTMS,
                                   1.0f);
                usedstatictexturememory = g_staticTMS.m_nTotal;
                usedtexturememory = usedstatictexturememory + useddynamictexturememory;
                usedbumpmapmemory = g_staticTMS.m_nBumpMap + g_dynamicTMS.m_nBumpMap;
                usedcomptexmemory = g_staticTMS.m_nStatic + g_dynamicTMS.m_nStatic;
                usedcubemapmemory = g_staticTMS.m_nCubeMap + g_dynamicTMS.m_nCubeMap;
                usedlightmapmemory = g_staticTMS.m_nInterface + g_dynamicTMS.m_nInterface;
                AurTextureBuildAndStoreAll();
                if (maxtextime < currenttextime)
                    maxtextime = currenttextime;
                if (maxtexloadtime < loadimagetime)
                    maxtexloadtime = loadimagetime;
                if (maxdecodetime < decodeimagetime)
                    maxdecodetime = decodeimagetime;
                if (maxgltime < glimagetime)
                    maxgltime = glimagetime;
                loadimagetime = 0;
            }
        }
#else
        if (RefreshTextures.m_nCount != 0)
        {
            AurTextureBuildAndStoreAll();
            if (maxtextime < currenttextime)
                maxtextime = currenttextime;
            if (maxtexloadtime < loadimagetime)
                maxtexloadtime = loadimagetime;
            if (maxdecodetime < decodeimagetime)
                maxdecodetime = decodeimagetime;
            if (maxgltime < glimagetime)
                maxgltime = glimagetime;
            loadimagetime = 0;
        }
#endif
    }
    if (DeltaT > maxdeltat)
        maxdeltat = DeltaT;
    return ++texcurrentframe;
}
// PC 0x00421DB0
void CAurTextureBasic::Reset()
{
    FreeImage();
    m_nCompressedSize = 0;
    m_nDataSize = 0;
#ifdef _XBOX
    m_nCycleCompressedSize = 0;
#endif
    m_nTxiSize = 0;
    m_nNumMipMaps = 0;
    m_nUploadedLevels = 0;
    m_nLevel = m_nBaseLevel;
    m_bMemoryDirty = 1;
    m_bFullyLoaded = 0;
    m_bProcessed = 0;
    m_bTxiLoaded = 0;
#ifdef _XBOX
    if (m_pTxiData != NULL)
    {
        operator delete(m_pTxiData);
        m_pTxiData = NULL;
    }
    if (m_pTextureIDs != NULL)
    {
        GLRender_DeleteTextures(m_nNumX * m_nNumY, m_pTextureIDs);
        operator delete(m_pTextureIDs);
        m_pTextureIDs = NULL;
        m_nTIDIndex = 0;
    }
#else
    if (m_pTxiData != NULL)
    {
        delete[] m_pTxiData;
        m_pTxiData = NULL;
    }
    if (m_pTextureIDs != NULL)
    {
        GLRender_DeleteTextures(m_nNumX * m_nNumY, m_pTextureIDs);
        delete[] m_pTextureIDs;
        m_pTextureIDs = NULL;
        m_nTIDIndex = 0;
    }
#endif
}

List<int> TextureIndices;
// PC 0x00421EB0
void CAurTextureBasic::GetMemoryUsage(TEXTUREMEMORYSTATS* pStats)
{
    if (m_bMemoryDirty == 0)
    {
        memset(pStats, 0, sizeof(TEXTUREMEMORYSTATS));
        pStats->m_nTotal = m_nMemorySize;
        if (strstr(m_sFileName, "_lm") != NULL ||
            strstr(m_sFileName, "_a00") != NULL)
        {
            pStats->m_nInterface = m_nMemorySize;
        }
        else if (m_bCubeMap != 0)
        {
            pStats->m_nCubeMap = m_nMemorySize;
        }
        else if (IsBumpMap())
        {
            pStats->m_nBumpMap = m_nMemorySize;
        }
        else if (m_nCompressedSize > 0)
        {
            pStats->m_nStatic = m_nMemorySize;
        }
        return;
    }
    if (m_bFullyLoaded == 0)
    {
        memset(pStats, 0, sizeof(TEXTUREMEMORYSTATS));
        return;
    }
    if (this == m_pNullTexture.m_pObject)
    {
        memset(pStats, 0, sizeof(TEXTUREMEMORYSTATS));
        return;
    }
    int nShift = m_nLevel - m_nUploadedLevels;
    if (m_nCompressedSize > 0)
    {
        m_nMemorySize =
            ImageGetS3TCSize(m_nWidth >> nShift, m_nHeight >> nShift, m_nBPP);
    }
    else
    {
        int nWidth = max(2, m_nWidth >> nShift);
        int nHeight = max(2, m_nHeight >> nShift);
        int nBPP = m_nBPP;
        if (CAurInternal::m_nColorDepth == 16 && nBPP > 2)
        {
            nBPP = 2;
        }
        else if (nBPP == 3)
        {
            nBPP = 4;
        }
        m_nMemorySize = nWidth * nHeight * nBPP;
        if (m_bMipMap != 0)
        {
            m_nMemorySize = (4 * m_nMemorySize) / 3;
        }
    }
    m_bMemoryDirty = 0;
    memset(pStats, 0, sizeof(TEXTUREMEMORYSTATS));
    pStats->m_nTotal = m_nMemorySize;
    if (strstr(m_sFileName, "_lm") != NULL ||
        strstr(m_sFileName, "_a00") != NULL)
    {
        pStats->m_nInterface = m_nMemorySize;
    }
    else if (m_bCubeMap != 0)
    {
        pStats->m_nCubeMap = m_nMemorySize;
    }
    else if (IsBumpMap())
    {
        pStats->m_nBumpMap = m_nMemorySize;
    }
    else if (m_nCompressedSize > 0)
    {
        pStats->m_nStatic = m_nMemorySize;
    }
}

#ifndef _XBOX
unsigned int AurTextureReadScreen(int nWidth, int nHeight, unsigned char* pBuffer)
{
    unsigned int nTextureID;
    glReadPixels(0, 0, nWidth, nHeight, GL_RGB, GL_UNSIGNED_BYTE, pBuffer);
    glGenTextures(1, &nTextureID);
    return nTextureID;
}
#endif
// PC 0x00422110
int CAurTextureBasic::glImage(bool bMipMaps)
{
#ifndef _XBOX
    if (AurATIValidate())
    {
        glFinish();
    }
#endif
    if (m_pImage == NULL)
    {
        m_nTIDIndex = 0;
        return 0;
    }
    if (m_nTIDIndex == 0 || m_pTextureIDs == NULL)
    {
        if (m_pTextureIDs != NULL)
        {
            operator delete(m_pTextureIDs);
        }
        m_pTextureIDs = (unsigned int*)operator new(4 * (m_nNumX * m_nNumY));
        GLRender_GenTextures(m_nNumX * m_nNumY, (GLuint*)m_pTextureIDs);
        m_nTIDIndex = m_pTextureIDs[0];
    }
    for (int i = 0; i < m_nNumX * m_nNumY; i++)
    {
        AurTextureBindTID(m_pTextureIDs[i], m_bCubeMap);
        if (m_bCubeMap)
        {
            if (IsProcessed())
            {
                GLRender::CreateProcessedCubeTexture(this, bMipMaps, i);
            }
            else
            {
                GLRender::CreateCubeTexture(this, bMipMaps, i);
            }
        }
        else
        {
            if (IsProcessed())
            {
#ifdef _XBOX
                GLRender::CreateProcessedTexture(this, bMipMaps, i, false);
#else
                GLRender::CreateProcessedTexture(this, bMipMaps, i);
#endif
            }
            else
            {
#ifdef _XBOX
                bool bLinear = false;
                if (m_pController != NULL)
                {
                    bLinear = m_pController->IsLinear();
                }
                GLRender::CreateTexture(this, bMipMaps, i, bLinear);
#else
                if (m_pController != NULL)
                {
                    m_pController->IsLinear();
                }
                GLRender::CreateTexture(this, bMipMaps, i);
#endif
            }
        }
    }
    if (m_bCubeMap && AurCubeMapAvailable())
    {
        GLRender_Disable(ENABLE_TEXTURE_CUBE_MAP);
    }
    return m_nTIDIndex;
}
// PC 0x00422280
char* AurTextureFind(char* sName, char* sExtra, int* pTXI, int nNumTXI)
{
    if (sExtra == NULL)
    {
        for (int i = 0; i < CAurTextureBasic::m_OrderedTextureList.m_nCount; i++)
        {
            if (stricmp(sName,
                         CAurTextureBasic::m_OrderedTextureList.m_pList[i]->m_sName) == 0)
            {
                return (char*)CAurTextureBasic::m_OrderedTextureList.m_pList[i];
            }
        }
        return NULL;
    }
    else
    {
        unsigned short nHash = (unsigned short)AurTextureCalculateHashID(
            sExtra, (short)nNumTXI, pTXI);
        int i;
        for (i = 0; i < CAurTextureBasic::m_OrderedTextureList.m_nCount; i++)
        {
            if (nHash <= CAurTextureBasic::m_OrderedTextureList.m_pList[i]->m_nOrder)
            {
                break;
            }
        }
        for (; i < CAurTextureBasic::m_OrderedTextureList.m_nCount; i++)
        {
            CAurTextureBasic* pTexture = CAurTextureBasic::m_OrderedTextureList.m_pList[i];
            if (nHash != pTexture->m_nOrder)
            {
                break;
            }
            if (nNumTXI == CAurTextureBasic::m_OrderedTextureList.m_pList[i]->m_nSchemeSize)
            {
                if (nNumTXI == pTexture->m_nSchemeSize &&
                    stricmp(sExtra, pTexture->m_sFileName) == 0)
                {
                    bool bMatch = true;
                    for (int j = 0; j < nNumTXI; j++)
                    {
                        if (pTXI[j] != pTexture->m_pScheme[j])
                        {
                            bMatch = false;
                            break;
                        }
                    }
                    if (bMatch)
                    {
                        return (char*)pTexture;
                    }
                }
            }
        }
        return NULL;
    }
}
// PC 0x004223B0
unsigned int AurTextureGetTID(CAurTexture* pTexture)
{
#ifdef _XBOX
    if (pTexture == NULL || pTexture->IsNull())
#else
    if (pTexture == NULL ||
        pTexture->GetReferencedTexture() ==
            (CAurTexture*)CAurTextureBasic::m_pNullTexture.m_pObject)
#endif
    {
        return 0;
    }
    return pTexture->GetReferencedTexture()->AsCAurTextureBasic()->m_nTIDIndex;
}

#ifdef _XBOX

void CAurTextureBasic::AsNormalizeVectorCubeMap()
{
    unsigned int* pTextureIDs = m_pTextureIDs;
    m_nLevel = 0;
    m_nMaxLevel = 0;
    m_nBaseLevel = 0;
    m_bSpecial = 1;
    m_nBPP = 3;
    m_bMipMap = 1;
    m_bFilter = 1;
    m_bCubeMap = 1;
    m_nWidth = 64;
    m_nHeight = 384;
    if (pTextureIDs == NULL)
    {
        m_pTextureIDs = new unsigned int[1];
    }
    GLRender::GenTextures(1, (unsigned int*)&m_nTIDIndex);
    m_pTextureIDs[0] = m_nTIDIndex;
    GLRender::Enable(ENABLE_NORMALIZE);
    GLRender::SetActiveTexture(1);
    GLRender::BindTexture(ENABLE_TEXTURE_CUBE_MAP, m_nTIDIndex);
    GLRender::MakeNormalizationCubeMap(64);
    GLRender::Disable(ENABLE_TEXTURE_CUBE_MAP);
    GLRender::SetActiveTexture(0);
}
#else
void CAurTextureBasic::AsNormalizeVectorCubeMap()  // PC 0x004223E0
{
    unsigned int* pTextureIDs = m_pTextureIDs;
    m_nLevel = 0;
    m_nMaxLevel = 0;
    m_nBaseLevel = 0;
    m_bSpecial = 1;
    m_nBPP = 3;
    m_bMipMap = 1;
    m_bFilter = 1;
    m_bCubeMap = 1;
    m_nWidth = 256;
    m_nHeight = 1536;
    if (pTextureIDs == NULL)
    {
        m_pTextureIDs = new unsigned int[1];
    }
    glGenTextures(1, (GLuint*)&m_nTIDIndex);
    m_pTextureIDs[0] = m_nTIDIndex;
    glEnable(enableTranslationTable[ENABLE_NORMALIZE]);
    GLRender::SetActiveTexture(1);
    unsigned int nTexture = m_nTIDIndex;
    glBindTexture(enableTranslationTable[ENABLE_TEXTURE_CUBE_MAP], nTexture);
    glTexParameteri(enableTranslationTable[ENABLE_TEXTURE_CUBE_MAP], GL_TEXTURE_MAX_ANISOTROPY_EXT,
                    GLRender::cm_nAnisotropy);
    GLRender::cm_nGLCurrentTID[GLRender::cm_uiCurrentStage] = nTexture;
    GLRender::MakeNormalizationCubeMap(256);
    glTexParameteri(texParamTargetTranslationTable[TEX_PARAM_TARGET_2D],
                    texParamTranslationTable[TEX_PARAM_MAG_FILTER], GL_LINEAR);
    glTexParameteri(texParamTargetTranslationTable[TEX_PARAM_TARGET_2D],
                    texParamTranslationTable[TEX_PARAM_MIN_FILTER], GL_LINEAR_MIPMAP_LINEAR);
    glDisable(enableTranslationTable[ENABLE_TEXTURE_CUBE_MAP]);
    GLRender::SetActiveTexture(0);
}
#endif

extern int constructimagetime;
extern int decodeimagetime;
extern int glimagetime;
extern int maxtextime;
extern int maxtexloadtime;
extern int loadimagetime;
extern int maxdecodetime;
extern int maxgltime;
extern float maxdeltat;
// PC 0x00422530
void AurTextureRelease(CAurTexture* pTexture)
{
    if (pTexture != NULL && pTexture->m_lstSafePointers.m_nCount == 0)
    {
        if (pTexture->AsCAurTextureBasic() != NULL)
        {
            CAurTextureBasic* pBasic = pTexture->AsCAurTextureBasic();
            if (pBasic != NULL)
            {
                delete pBasic;
            }
        }
        else
        {
            delete pTexture;
        }
    }
}
// PC 0x00422570
void AurTextureCalculateNewTexMem(List<CAurTextureBasic*>& lstTextures,
                                  TEXTUREMEMORYSTATS* pStats)
{
    pStats->m_nBumpMap = 0;
    pStats->m_nStatic = 0;
    pStats->m_nCubeMap = 0;
    pStats->m_nInterface = 0;
    pStats->m_nTotal = 0;
    int i;
    for (i = 0; i < lstTextures.m_nCount; i++)
    {
        CAurTextureBasic* pTexture = lstTextures.m_pList[i];
        if (!pTexture->m_bFullyLoaded)
        {
            pTexture->ConstructImage();
        }
        if (pTexture->m_bTemporary == 0)
        {
            TEXTUREMEMORYSTATS stats;
            pTexture->GetMemoryUsage(&stats);
            pStats->m_nBumpMap += stats.m_nBumpMap;
            pStats->m_nStatic += stats.m_nStatic;
            pStats->m_nCubeMap += stats.m_nCubeMap;
            pStats->m_nInterface += stats.m_nInterface;
            pStats->m_nTotal += stats.m_nTotal;
        }
    }
}
// PC 0x00422610
void AurTextureForceControllerUpdate()
{
    for (int i = 0; i < CAurTextureBasic::m_OrderedTextureList.m_nCount; i++)
    {
        CAurTextureBasic* pTexture = CAurTextureBasic::m_OrderedTextureList.m_pList[i];
        if (pTexture->m_bControllerDirty != 0 &&
            pTexture->m_pController != NULL)
        {
            pTexture->Bind();
            pTexture->m_bControllerDirty = 0;
        }
    }
}
// PC 0x00422660
int BuildTextureNormalMap(unsigned char* pHeightField, float fScale,
                          int& nWidth, int& nHeight, int nLevel, int nBPP,
                          bool bMipMaps)
{
    int nOrigWidth;
    int bBadWidth = 0;
    if ((nWidth & (nWidth - 1)) != 0)
    {
        if (((nWidth - 1) & (nWidth - 2)) != 0)
        {
            bBadWidth = 1;
        }
        else
        {
            nOrigWidth = nWidth;
            nWidth = nWidth - 1;
        }
    }
    else
    {
        nOrigWidth = nWidth;
    }
    int nOrigHeight = nHeight;
    if ((nHeight & (nHeight - 1)) != 0)
    {
        if (((nHeight - 1) & (nHeight - 2)) != 0)
        {
            return false;
        }
        nHeight = nHeight - 1;
    }
    if (bBadWidth)
    {
        return false;
    }
    GLRender::ConvertHeightFieldAndLoadNormalMapTexture(
        pHeightField, nWidth, nHeight, nOrigWidth, nOrigHeight, fScale, nBPP,
        bMipMaps);
    return true;
}

#ifdef _XBOX
Scene* FindScene(char* sName);

void AurSaveGameSnapshot(char* sFileName, int nSize, bool bSquare)
{
    static int k;
    char sBuffer[64];
    if (sFileName == NULL || strcmp(sFileName, "NULL") == 0)
    {
        sprintf(sBuffer, "snap%d.tga", ++k);
    }
    else
    {
        strcpy(sBuffer, sFileName);
        int nLen = strlen(sBuffer);
        if (nLen <= 3 || stricmp(sBuffer + nLen - 4, ".tga") != 0)
        {
            strcat(sBuffer, ".tga");
        }
    }
    D3DVIEWPORT8 viewport;
    g_pD3DDevice->GetViewport(&viewport);
    int nWidth = viewport.Width;
    int nHeight = viewport.Height;
    if (nWidth % 4 != 0)
    {
        nWidth = nWidth - nWidth % 4 + 4;
    }
    int nOutWidth;
    int nOutHeight;
    if (nWidth > nHeight)
    {
        if (bSquare)
        {
            nOutWidth = nSize;
            nOutHeight = nSize;
        }
        else
        {
            nOutWidth = nSize;
            nOutHeight = (int)((float)nHeight * (float)nSize / (float)nWidth);
        }
    }
    Camera* pCamera = (Camera*)FindGob("camera");
    GLRender::Clear(1 | 2);
    D3DTexture* pTexture;
    g_pD3DDevice->CreateTexture(nOutWidth, nOutHeight, 0, 0, D3DFMT_LIN_A8R8G8B8, 0, &pTexture);
    D3DSurface* pSurface;
    pTexture->GetSurfaceLevel(0, &pSurface);
    D3DSurface* pBackBuffer;
    g_pD3DDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
    D3DSurface* pDepthStencil;
    g_pD3DDevice->GetDepthStencilSurface(&pDepthStencil);
    g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0, 0.0f, 0);
    g_pD3DDevice->SetRenderTarget(pSurface, pDepthStencil);
    Scene* pScene = FindScene("mainscene");
    GLRender::SetClearColor(pScene->m_vFogColor.m_fX, pScene->m_vFogColor.m_fY, pScene->m_vFogColor.m_fZ, 1.0f);
    GLRender::Clear(1 | 2);
    GLRender::LoadMatrixIdentity();
    skippauseplaneforscreenshot = true;
    visualizepass6 = 0;
    doframebuffer = 0;
    pCamera->RenderScene();
    skippauseplaneforscreenshot = false;
    visualizepass6 = 1;
    doframebuffer = 1;
    D3DLOCKED_RECT lockedRect;
    pSurface->LockRect(&lockedRect, NULL, D3DLOCK_READONLY | D3DLOCK_TILED);
    ImageDup((unsigned char*)lockedRect.pBits, nOutWidth, nOutHeight, 4);
    ImageWriteTGA(sBuffer, nOutWidth, nOutHeight, 4, (unsigned char*)lockedRect.pBits);
    pSurface->Release();
    g_pD3DDevice->SetRenderTarget(pBackBuffer, pDepthStencil);
    pBackBuffer->Release();
    pDepthStencil->Release();
    pTexture->Release();
}
#else
void AurSaveGameSnapshot(char* sFileName, int nSize, bool bSquare, bool bCrop)  // PC 0x004226E0
{
    static int k;
    char sBuffer[64];
    if (sFileName == NULL || strcmp(sFileName, "NULL") == 0)
    {
        sprintf(sBuffer, "snap%d.tga", ++k);
    }
    else
    {
        strcpy(sBuffer, sFileName);
        int nLen = strlen(sBuffer);
        if (nLen <= 3 || stricmp(sBuffer + nLen - 4, ".tga") != 0)
        {
            strcat(sBuffer, ".tga");
        }
    }
    GLint viewport[4];
    GetViewport(viewport);
    int nWidth = viewport[2];
    int nHeight = viewport[3];
    if (nWidth % 4 != 0)
    {
        nWidth = nWidth - nWidth % 4 + 4;
    }
    int nOutWidth;
    int nOutHeight;
    if (nWidth > nHeight)
    {
        if (bSquare)
        {
            nOutWidth = nSize;
            nOutHeight = nSize;
        }
        else
        {
            nOutWidth = nSize;
            nOutHeight = (int)((float)nHeight * (float)nSize / (float)nWidth);
        }
    }
    Camera* pCamera = (Camera*)FindGob("camera");
    GLRender::Clear(1 | 2);
    glLoadIdentity();
    skippauseplaneforscreenshot = true;
    pCamera->RenderScene();
    skippauseplaneforscreenshot = false;
    unsigned int nPixelBytes = 3 * nWidth * nHeight;
    unsigned char* pPixels = new unsigned char[nPixelBytes];
    memset(pPixels, 0, nPixelBytes);
    glReadPixels(0, 0, nWidth, nHeight,
                 textureFormatTranslationTable[Aurora::TEXTURE_FORMAT_RGB],
                 GL_UNSIGNED_BYTE, pPixels);
    ImageFlipY(pPixels, nWidth, nHeight, 3);
    unsigned char* pScaled = ImageScale(pPixels, nWidth,
                                        nHeight, 3, nOutWidth, nOutHeight);
    if (bSquare || !bCrop)
    {
        ImageWriteTGA(sBuffer, nOutWidth, nOutHeight, 3, pScaled);
    }
    else
    {
        int nSquare = nOutWidth > nOutHeight ? nOutWidth : nOutHeight;
        unsigned int nSquareBytes = 3 * nSquare * nSquare;
        unsigned char* pSquare = new unsigned char[nSquareBytes];
        memset(pSquare, 0x7F, nSquareBytes);
        memcpy(pSquare, pScaled, 3 * nOutWidth * nOutHeight);
        ImageWriteTGA(sBuffer, nSquare, nSquare, 3, pSquare);
        delete[] pSquare;
    }
    delete[] pScaled;
    delete[] pPixels;
}
#endif
// PC 0x00424390
void CAurTextureBasic::Init(char* sName)
{
    int nResourceSize = 0;
    if (!IsTxiLoaded())
    {
        if (m_pFontInfo == NULL)
        {
            m_pFontInfo = new CAurFontInfo();
        }
        void* pRes = AurResGet(sName, ".txi", &nResourceSize, true);
        if (pRes == NULL)
        {
            if (m_pFontInfo != NULL)
            {
                delete m_pFontInfo;
            }
            m_pFontInfo = NULL;
            SetTxiLoaded(true);
            return;
        }
        int nAbsSize = nResourceSize < 0 ? -1 - nResourceSize : nResourceSize;
        if (nAbsSize > 0)
        {
            SetTxiData(AurResGetDataBytes(0, pRes), nAbsSize);
            char* pLine;
            while ((pLine = AurResGetNextLine()) != NULL)
            {
                while (*pLine == ' ' || *pLine == '\t')
                {
                    pLine++;
                }
                ParseField(pLine);
            }
        }
        AurResFree(pRes, nResourceSize);
        if (m_pFontInfo != NULL && m_pFontInfo->m_lstUpperLeft.m_nCount == 0)
        {
            delete m_pFontInfo;
            m_pFontInfo = NULL;
        }
        SetTxiLoaded(true);
    }
    else
    {
        unsigned char* pData = (unsigned char*)GetTxiData();
        int nSize = GetTxiSize();
        char sLine[8192];
        if (pData != NULL && nSize > 0)
        {
            char* pLine;
            do
            {
                int i = 0;
                do
                {
                    sLine[i++] = *pData++;
                } while (i < nSize && sLine[i - 1] != '\n');
                nSize -= i;
                if (nSize < 0)
                {
                    pLine = NULL;
                }
                else
                {
                    sLine[i] = 0;
                    pLine = sLine;
                    while (*pLine == ' ' || *pLine == '\t')
                    {
                        pLine++;
                    }
                    ParseField(pLine);
                }
            } while (nSize != 0 && pLine != NULL && nSize > 0);
        }
    }
    if (!AurCubeMapAvailable())
    {
        m_bCubeMap = 0;
    }
}
// PC 0x00424580
int AurTextureSetAttributes(CAurTexture* pTexture, char* sField)
{
    if (pTexture == NULL)
    {
        return 0;
    }
    CAurTextureBasic* pBasic =
        pTexture->GetReferencedTexture()->AsCAurTextureBasic();
    pBasic->Init(sField);
    RefreshTextures.AddUnique(pBasic);
    return 1;
}
// PC 0x004245C0
int AurTextureMessage(CAurTexture* pTexture, char* sField)
{
    if (pTexture == NULL)
    {
        return 0;
    }
    CAurTextureBasic* pBasic =
        pTexture->GetReferencedTexture()->AsCAurTextureBasic();
    if (pBasic == NULL)
    {
        return 0;
    }
    pBasic->ParseField(sField);
    RefreshTextures.AddUnique(pBasic);
    return 1;
}
// PC 0x00424970
CAurTexture* AurTextureGetNULL()
{
    if (CAurTextureBasic::m_pNullTexture.m_pObject == NULL)
    {
        CAurTexture* pTexture = new CAurTexture();
        CAurTextureBasic::m_pNullTexture = pTexture;
    }
    return (CAurTexture*)CAurTextureBasic::m_pNullTexture.m_pObject;
}
// PC 0x004246D0
CAurTexture::CAurTexture()
{
    m_pReferenced = NULL;
    m_bEnvironmentMapped = 0;
}
// PC 0x00424A00
CAurTextureBasic::CAurTextureBasic(char* sName, char* sExtra)
{
    m_sFileName[0] = '\0';
    if (sName != NULL)
    {
        strcpy(m_sName, sName);
    }
    else
    {
        strcpy(m_sName, sExtra);
    }
    strcpy(m_sFileName, sExtra);
    m_fGamma = 1.0f;
    m_fBumpMapScaling = 1.0f;
    m_fBumpIntensity = 1.0f;
    m_fSortKey = -1.0f;
    m_fEnvMapAlpha = 1.0f;
    m_fDiffuseBumpIntensity = 1.0f;
    m_fSpecularBumpIntensity = 1.0f;
    m_nMaxLevel = 15;
    m_nBaseLevel = 0;
    m_nLevel = 0;
    m_nUploadedLevels = 0;
    m_nWidth = 2;
    m_nHeight = 2;
    m_nDefaultWidth = 2;
    m_nDefaultHeight = 2;
    m_nDepth = 0;
    m_nBPP = 0;
    m_nTIDIndex = 0;
    m_nClamp = 0;
    m_nNumX = 1;
    m_nNumY = 1;
    m_nSchemeSize = 0;
    m_nFileRange = 0;
    m_nNumMipMaps = 0;
    m_nCompressedSize = 0;
    m_nDataSize = 0;
#ifdef _XBOX
    m_nCycleCompressedSize = 0;
#endif
    m_nTxiSize = 0;
    m_nTextureFormat = Aurora::TEXTURE_FORMAT_DXT1;
    m_bDynamic = g_bModeIsDynamic;
    m_bTemporary = 0;
    m_bCubeMap = 0;
    m_bDiffuseBumpMap = 1;
    m_bSpecularBumpMap = 1;
    m_bEnvMapped = 0;
    m_bMipMap = 1;
    m_bFilter = 1;
    m_bMapTexelsToPixels = 0;
    m_bUsingGlobalAlpha = 0;
    m_bInvalid = 0;
    m_bFullyLoaded = 0;
    m_bSpecial = 0;
    m_bRasterAllocated = 0;
    m_nBumpType = 0;
    m_bProcessed = 0;
    m_bTxiLoaded = 0;
    m_bControllerDirty = 1;
    m_vSpecularColor = Vector(1.0f, 1.0f, 1.0f);
    m_pFontInfo = new CAurFontInfo();
    m_pImage = NULL;
    m_pController = NULL;
    m_pTextureIDs = NULL;
    m_pScheme = NULL;
    m_pTxiData = NULL;
    m_nOrder = 0;
#ifdef _XBOX
    CalculateHashID();
#else
    m_nOrder = AurTextureCalculateHashID(m_sFileName, m_nSchemeSize, NULL);
#endif
    AddToOrderedLists();
    countbasictextures++;
    m_bF0 = 0;
    m_nMemorySize = -1;
    m_bMemoryDirty = 1;
}
// PC 0x00424730
CAurTexture::~CAurTexture()
{
    CAurTexture* pTarget = m_pReferenced;
    m_pReferenced = NULL;
    AurTextureRelease(pTarget);
}
// PC 0x004247E0
CAurTextureBasic::~CAurTextureBasic()
{
    if (m_pFontInfo != NULL)
    {
        delete m_pFontInfo;
    }
    delete[] m_pScheme;
    if (m_pController != NULL)
    {
        delete m_pController;
    }
    if (m_bTemporary == 0)
    {
        TEXTUREMEMORYSTATS stats;
        GetMemoryUsage(&stats);
        usedtexturememory -= stats.m_nTotal;
        usedlightmapmemory -= stats.m_nInterface;
        usedcomptexmemory -= stats.m_nStatic;
        usedcubemapmemory -= stats.m_nCubeMap;
        usedbumpmapmemory -= stats.m_nBumpMap;
    }
    if (m_pTextureIDs != NULL)
    {
        int nTextures = m_nNumX * m_nNumY;
        GLRender_DeleteTextures(nTextures, m_pTextureIDs);
        delete[] m_pTextureIDs;
        m_pTextureIDs = NULL;
        m_nTIDIndex = 0;
    }
    if (m_pTxiData != NULL)
    {
        delete[] m_pTxiData;
        m_pTxiData = NULL;
    }
    FreeImage();
    countbasictextures--;
    RemoveFromOrderedLists();
    if (RefreshTextures.Contains(this))
    {
        RefreshTextures.Remove(this);
    }
}
// PC 0x00424C90
CAurTextureBasic* AurTextureGetNormalizationCubeMap()
{
    if (!AurCubeMapAvailable())
    {
        return NULL;
    }
    CAurTextureBasic* pTexture = (CAurTextureBasic*)AurTextureFind(
        "NormCubeMap", "NormCubeMap", NULL, 0);
    if (pTexture != NULL)
    {
        return pTexture->AsCAurTextureBasic();
    }
    pTexture = new CAurTextureBasic("NormCubeMap", "NormCubeMap");
    pTexture->AsNormalizeVectorCubeMap();
    return pTexture;
}
// PC 0x00424D40
CAurTexture* AurTextureGet(char* sName, CAurTexture* pReference, char* sExtra,
                           int* pTXI, int nNumTXI)
{
    char sExtraBuf[32];
    CAurTextureBasic* pTexture;
    CAurTexture* pResult;
    sExtraBuf[0] = 0;
    if (pReference == NULL)
    {
        if (sExtra != NULL && *sExtra != 0)
        {
            strcpy(sExtraBuf, sExtra);
        }
        else
        {
            strcpy(sExtraBuf, sName);
        }
        pTexture =
            (CAurTextureBasic*)AurTextureFind(sName, sExtraBuf, pTXI, nNumTXI);
        if (pTexture != NULL)
        {
            return pTexture;
        }
    }
    if (pReference != NULL)
    {
        pResult = new CAurTexture();
#ifdef _XBOX
        pResult->SetReferencedTexture(pReference);
#else
        pResult->m_pReferenced = pReference;
#endif
    }
    else
    {
        pTexture = new CAurTextureBasic(sName, sExtraBuf);
        pTexture->Init(pTexture->m_sFileName);
        RefreshTextures.Add(pTexture);
        pResult = pTexture;
    }
    return pResult;
}
// PC 0x00424EA0
int AurTextureReplaceSingleReference(CAurTexture* pTexture, char* sName,
                                      char* sExtra, char* sNewName, int* pTXI,
                                      int nNumTXI)
{
    if (pTexture == NULL)
    {
        return 0;
    }
    CAurTextureBasic* pBasic =
        pTexture->GetReferencedTexture()->AsCAurTextureBasic();
    if (pBasic == NULL)
    {
        return 0;
    }
    if (sName != NULL && stricmp(sName, pBasic->m_sName) != 0)
    {
        return 0;
    }
    bool bEnvMapped = pTexture->IsEnvironmentMapped();
    if (pBasic->m_bDynamic == 0)
    {
#ifdef _XBOX
        AurTextureSetModeStatic();
#else
        g_bModeIsDynamic = false;
#endif
    }
    bool bReleased = false;
    CAurTexture* pNew;
    if (sNewName != NULL)
    {
        char sNameBuf[32];
        strcpy(sNameBuf, pBasic->m_sName);
        if (stricmp(sNewName, pBasic->m_sFileName) == 0)
        {
#ifdef _XBOX
            pTexture->SetReferencedTexture(NULL);
#else
            pTexture->m_pReferenced = NULL;
#endif
            AurTextureRelease((CAurTexture*)pBasic);
            bReleased = true;
        }
        pNew = AurTextureGet(sNameBuf, NULL, sNewName, pTXI, nNumTXI);
    }
    else if (sName != NULL)
    {
        if (stricmp(sName, pBasic->m_sName) == 0)
        {
#ifdef _XBOX
            pTexture->SetReferencedTexture(NULL);
#else
            pTexture->m_pReferenced = NULL;
#endif
            AurTextureRelease((CAurTexture*)pBasic);
            bReleased = true;
            pNew = AurTextureGet(sName, NULL, sExtra, pTXI, nNumTXI);
        }
        else
        {
#ifdef _XBOX
            AurTextureSetModeDynamic();
#else
            g_bModeIsDynamic = true;
#endif
            return 0;
        }
    }
    else
    {
        pNew = AurTextureGet(pBasic->m_sName, NULL, sExtra, pTXI, nNumTXI);
    }
    pNew->SetIsEnvironmentMapped(bEnvMapped);
    CAurTextureBasic* pNewBasic = pNew->AsCAurTextureBasic();
#ifdef _XBOX
    AurTextureSetModeDynamic();
#else
    g_bModeIsDynamic = true;
#endif
    if (!bReleased && pNewBasic == pBasic->AsCAurTextureBasic())
    {
        return 0;
    }
#ifdef _XBOX
    pTexture->SetReferencedTexture(pNew);
#else
    pTexture->m_pReferenced = pNew;
#endif
    if (!bReleased)
    {
        AurTextureRelease((CAurTexture*)pBasic);
    }
    if (pTXI != NULL && pNewBasic->m_bFullyLoaded != 0)
    {
        return 0;
    }
    return 1;
}
// PC 0x00425090
int AurTextureReplaceTileReferences(CAurScene* pScene, const Vector& vPos,
                                    char* sOldName, char* sNewName)
{
    CAurTexture* pFound =
        (CAurTexture*)AurTextureFind(sNewName, sNewName, NULL, 0);
    if (pFound == NULL)
    {
        return 0;
    }
    CAurTextureBasic* pOld = pFound->AsCAurTextureBasic();
    if (pOld->m_bDynamic == 0)
    {
#ifdef _XBOX
        AurTextureSetModeStatic();
#else
        g_bModeIsDynamic = false;
#endif
    }
    CAurTexture* pNew = AurTextureGet(sOldName, NULL, NULL, NULL, 0);
#ifdef _XBOX
    AurTextureSetModeDynamic();
#else
    g_bModeIsDynamic = true;
#endif
    if (pNew->IsInvalid())
    {
        AurTextureRelease(pNew);
        return 0;
    }
    List<CAurTexture*> lstTileTextures;
    List<CAurTexture*> lstMatches;
    SceneBSPGetTileTextures((Scene*)pScene, vPos, lstTileTextures);
    for (int i = 0; i < lstTileTextures.m_nCount; i++)
    {
        if (lstTileTextures[i] != NULL && lstTileTextures[i]->m_pReferenced == pOld)
        {
            lstMatches.AddUnique(lstTileTextures[i]);
            break;
        }
    }
    if (lstMatches.m_nCount > 0)
    {
        for (int j = 0; j < lstMatches.m_nCount; j++)
        {
#ifdef _XBOX
            lstMatches[j]->SetReferencedTexture(pNew);
#else
            CAurTexture* pTile = lstMatches[j];
            pTile->m_pReferenced = pNew;
#endif
            AurTextureRelease((CAurTexture*)pOld);
        }
        return 1;
    }
    return 0;
}
// PC 0x00425320
CAurTexture* AurTextureGetReference(char* sName, char* sExtra)
{
    CAurTexture* pTexture = AurTextureGet(sName, NULL, sExtra, NULL, 0);
    return AurTextureGet(NULL, pTexture, NULL, NULL, 0);
}

int currenttextime;

Vector CAurTextureBasic::m_vLowEndEnvMapColor(0.0f, 0.0f, 0.0f);

#ifdef _XBOX

const char* snapshot(char* sCommand)
{
    char sName[64];

    if (sscanf(sCommand, "%s", sName) != 1)
    {
        AurSnapshot(NULL);
        return "Attempted to save file";
    }
    AurSnapshot(sName);
    return "Attempted to save file";
}

const char* settexturemem(char* sCommand)
{
    return AurTextureSetMem(atoi(sCommand)) ? "Success" : "Failure";
}

const char* refreshalltextures(char* sCommand)
{
    AurTextureRefreshAll();
    return "refreshed textures";
}

const char* texturemessage(char* sCommand)
{
    char sName[64];
    int nRead;

    sName[0] = 0;
    nRead = 0;
    sscanf(sCommand, "%s%n", sName, &nRead);
    CAurTexture* pTexture = (CAurTexture*)AurTextureFind(sName, sName, NULL, 0);
    if (pTexture == NULL)
    {
        return "no texture by that name";
    }
    AurTextureMessage(pTexture, skipspace(sCommand + nRead));
    return "OK";
}
#endif

#ifdef _XBOX

unsigned char* AurGetLayeredImageWrapper(char* sFileName, int* pWidth, int* pHeight, int* pBPP)
{
    int nReserved;
    unsigned char* pImage = AurGetLayeredRaster(sFileName, pWidth, pHeight, &nReserved);
    *pBPP = 2;

    return pImage;
}

int logbase2(unsigned int nValue);

void CAurTextureBasic::ClampMissingImageSize()
{
    if (m_pImage == NULL)
    {
        if (m_nBumpType != 0)
        {
            m_nBPP = 1;
        }
        m_nWidth = max(2, m_nWidth);
        m_nHeight = max(2, m_nHeight);
    }
}

void CAurTextureBasic::ReduceToLevel()
{
    int nFaces = 1;
    if (m_nLevel == m_nUploadedLevels)
    {
        return;
    }
    unsigned char* pImage = m_pImage;
    if (pImage != NULL && m_nLevel != 0)
    {
        if (m_nCompressedSize > 0)
        {
            int nBlockSize = m_nBPP == 4 ? 16 : 8;
            m_pImage = ImageSkipS3TCLevels(m_nWidth, m_nHeight, m_nBPP, pImage, m_nLevel);
            int nBytesPerBlock;
            if (m_nBPP == 3)
            {
                nBytesPerBlock = 8;
            }
            else if (m_nBPP == 4)
            {
                nBytesPerBlock = 16;
            }
            int nWidth = m_nWidth;
            int nHeight = m_nHeight;
            if (nHeight / nWidth == 6)
            {
                nFaces = 6;
                nHeight /= 6;
            }
            int nSize = m_nCompressedSize;
            if (nSize != 0)
            {
                for (int i = 0; i < m_nNumMipMaps - 1; i++)
                {
                    nHeight >>= 1;
                    nWidth >>= 1;
                    nSize += ((nWidth + 3) / 4) * ((nHeight + 3) / 4) * nBytesPerBlock;
                }
                if (nFaces == 6)
                {
                    nSize = nSize - nSize % 128 + 128;
                    nSize *= 6;
                }
            }
            AurTextureFreeImage(pImage, m_nWidth, m_nHeight, m_nBPP, nSize, m_nDataSize, m_nTxiSize,
                                (bool*)&m_bRasterAllocated, m_bProcessed, false);
            m_nNumMipMaps -= m_nLevel;
            m_nHeight >>= m_nLevel;
            m_nWidth >>= m_nLevel;
            nHeight = m_nHeight;
            nWidth = m_nWidth;
            m_nCompressedSize = ((nHeight + 3) / 4) * ((nWidth + 3) / 4) * nBlockSize;
        }
        else if (m_nBPP == 4)
        {
            int nLogWidth = logbase2(m_nWidth);
            int nLogHeight = logbase2(m_nHeight);
            int nLevels = min(nLogWidth, nLogHeight);
            nLevels = min(nLevels, m_nLevel);
            int nFactor;
            if (nLevels != 0)
            {
                nFactor = 2 << (nLevels - 1);
                m_pImage = ImageDownSampleBy(m_nWidth, m_nHeight, pImage, nFactor);
                AurTextureFreeImage(pImage, m_nWidth, m_nHeight, m_nBPP, m_nCompressedSize, m_nDataSize, m_nTxiSize,
                                    (bool*)&m_bRasterAllocated, m_bProcessed, false);
            }
            else
            {
                nFactor = 1;
            }
            m_nWidth /= nFactor;
            m_nHeight /= nFactor;
            m_nNumMipMaps -= nLevels;
        }
        else
        {
            int i;
            for (i = 0; i < m_nLevel && m_nWidth > 2 && m_nHeight > 2; i++)
            {
                unsigned char* pOld = m_pImage;
                m_pImage = ImageDownSample(m_nWidth, m_nHeight, m_nBPP, pOld);
                AurTextureFreeImage(pOld, m_nWidth, m_nHeight, m_nBPP, m_nCompressedSize, m_nDataSize, m_nTxiSize,
                                    (bool*)&m_bRasterAllocated, m_bProcessed, false);
                m_nWidth /= 2;
                m_nHeight /= 2;
            }
            if (i < m_nLevel)
            {
                m_nLevel = i;
            }
            m_nNumMipMaps -= m_nLevel;
        }
    }
    m_nUploadedLevels = m_nLevel;
}

#endif

#ifdef _XBOX

void CAurTextureBasic::CalculateHashID()
{
    m_nOrder = AurTextureCalculateHashID(m_sFileName, m_nSchemeSize, m_pScheme);
}

void CAurTexture::SetReferencedTexture(CAurTexture* pTexture)
{
    m_pReferenced = pTexture;
}

int CAurTextureBasic::CanReduceLevel()
{
    int nShift = m_nLevel - m_nUploadedLevels;
    if (m_nLevel < m_nMaxLevel && (m_nWidth >> nShift) > 2 && (m_nHeight >> nShift) > 2)
    {
        return 1;
    }
    return 0;
}

void CAurTextureBasic::RaiseLevel(int nLevels)
{
    m_nLevel = min(m_nLevel + nLevels, m_nMaxLevel);
    if (m_nLevel != m_nUploadedLevels)
    {
        m_bMemoryDirty = 1;
        RefreshTextures.AddUnique(this);
    }
}

int AurAddPalette(int nIndex, char* sName)
{
    int nRows = (nIndex + 1) * 256;
    if (nRows > paletteheight)
    {
        unsigned char* pOld = paletteimage;
        int nOldHeight = paletteheight;
        paletteheight = nRows;
        paletteimage = (unsigned char*)operator new(palettebpp * palettewidth * nRows);
        if (pOld != NULL)
        {
            ImageCopy(paletteimage, pOld, palettewidth, nOldHeight, palettebpp);
            operator delete(pOld);
        }
    }
    int nWidth;
    int nHeight;
    int nBPP;
    int nCompressedSize;
    int nDataSize;
    int nTxiSize;
    Aurora::AuroraTextureFormats nFormat;
    float fAlpha;
    bool bProcessed;
    short nNumMipMaps;
    int nBytesPerBlock;
    int nFaces = 1;
    unsigned char* pImage = AurGetImageWrapper(sName, &nWidth, &nHeight, &nBPP, &nCompressedSize, &nDataSize, &nTxiSize,
                                               &nFormat, &fAlpha, &bProcessed, &nNumMipMaps, 0);
    if (pImage == NULL)
    {
        return -1;
    }
    ImageFlipY(pImage, nWidth, nHeight, nBPP);
    ImageCopy(paletteimage + nIndex * 0x40000, pImage, nWidth, nHeight, nBPP);
    bool bAllocated = true;
    if (AurReleaseRaster == NULL)
    {
        bAllocated = false;
    }
    if (nBPP == 3)
    {
        nBytesPerBlock = 8;
    }
    else if (nBPP == 4)
    {
        nBytesPerBlock = 16;
    }
    int nLevelWidth = nWidth;
    int nLevelHeight = nHeight;
    if (nLevelHeight / nLevelWidth == 6)
    {
        nFaces = 6;
        nLevelHeight /= 6;
    }
    int nSize = nCompressedSize;
    if (nSize != 0)
    {
        for (int i = 0; i < nNumMipMaps - 1; i++)
        {
            nLevelHeight >>= 1;
            nLevelWidth >>= 1;
            nSize += ((nLevelWidth + 3) / 4) * ((nLevelHeight + 3) / 4) * nBytesPerBlock;
        }
        if (nFaces == 6)
        {
            nSize = nSize - nSize % 128 + 128;
            nSize *= 6;
        }
    }
    AurTextureFreeImage(pImage, nWidth, nHeight, nBPP, nSize, nDataSize, nTxiSize, &bAllocated, bProcessed, false);
    return nIndex;
}

const char* addpalette(char* sCommand)
{
    char sName[64];
    int nIndex = 0;

    sName[0] = 0;
    sscanf(sCommand, "%s%d", sName, &nIndex);
    if (sName[0] == 0)
    {
        return "no such file";
    }
    if (AurAddPalette(nIndex, sName) == -1)
    {
        return "ERROR: something didn't work";
    }
    return "OK";
}

const char* setpalettes(char* sCommand)
{
    AurAddPalette(0, "pal_skin01");
    AurAddPalette(1, "pal_skin01");
    AurAddPalette(2, "pal_hair01");
    AurAddPalette(3, "pal_armor01");
    AurAddPalette(4, "pal_cloth01");
    return "Palettes are set";
}

#endif
