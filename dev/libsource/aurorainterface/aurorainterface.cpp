#include "aurora.h"

#include "exobase.h"
#include "exoresman.h"

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline void AuroraInterfaceTrace();
#define AURORAINTERFACE_TRACE() AuroraInterfaceTrace()

extern int AurTextureSetDim(int, int);

static int g_nMaxTexMem = 16 * 1024 * 1024;
class Model;
Model* BinaryFindModel(char*);

CExoLinkedList<CAuroraModel> CAuroraInterface::m_lstRequestedModel;
CExoString CAuroraInterface::m_pCurrentTexturePack[4];
// PC 0x0070C6E0  Xbox 0x002F6CE0
unsigned char* CAuroraInterface::GetLayeredRaster(char* szName, int* pnWidth,
                                       int* pnHeight, int* pnReserved)
{
    CAuroraLayeredTexture* pTexture;
    int nWidth;
    int nHeight;
    int nReserved;
    {
        CExoString sName(szName);
        pTexture = GetLayeredTexture(sName);
    }
    if (pTexture == NULL)
    {
        return 0;
    }
    pTexture->GetTextureAttrib((unsigned long*)&nWidth,
                               (unsigned long*)&nHeight,
                               (unsigned long*)&nReserved);
    *pnWidth = nWidth;
    *pnHeight = nHeight;
    *pnReserved = nReserved;
    return pTexture->GetTextureDataPtr();
}
// PC 0x0070C790  Xbox 0x002F6D90
unsigned char* CAuroraInterface::GetRaster(char* szName, int* pnWidth, int* pnHeight,
                                int* pnFormat)
{
    CAuroraTexture* pTexture;
    unsigned int nBpp;
    unsigned char nBitsPerPixel;
    int nWidth;
    int nHeight;
    {
        CExoString sName(szName);
        pTexture = GetTexture(sName);
    }
    if (pTexture == NULL)
    {
        return 0;
    }
    pTexture->GetTextureAttrib((unsigned long*)&nWidth,
                               (unsigned long*)&nHeight, &nBitsPerPixel);
    *pnWidth = nWidth;
    nBpp = nBitsPerPixel;
    *pnHeight = nHeight;
    *pnFormat = nBpp >> 3;
    return pTexture->GetTextureDataPtr();
}
// PC 0x0070C850  Xbox 0x002F6E50
void CAuroraInterface::ReleaseLayeredRaster(void* pRaster, int nWidth,
                                            int nHeight)
{
    unsigned short* pPixels;

    pPixels = (unsigned short*)pRaster;
    ReleaseLayeredTextureInternal(
        ((SPLTLayeredTrailer*)(pPixels + nHeight * nWidth + 2))->pOwner);
}
// PC 0x0070C870  Xbox 0x002F6E70
void CAuroraInterface::ReleaseRaster(void* pRaster, int nWidth, int nHeight,
                                    int nBytesPerPixel, int nSize,
                                    int nReservedSize, int nTXISize,
                                    bool bProcessed)
{
    if (bProcessed)
    {
        SProcessedTextureDataTrailer* pTrailer;

        if (nSize != 0)
        {
            pTrailer = (SProcessedTextureDataTrailer*)((unsigned char*)pRaster +
                                                       nSize + nTXISize +
                                                       sizeof(unsigned long));
        }
        else
        {
            pTrailer = (SProcessedTextureDataTrailer*)((unsigned char*)pRaster +
                                                       nReservedSize + nTXISize +
                                                       sizeof(unsigned long));
        }
#ifdef _XBOX
        if (pTrailer->nType == 3006)
#else
        if (pTrailer->nType == RESTYPE_TPC)
#endif
        {
            ReleaseProcessedTextureInternal(pTrailer->pOwner);
            return;
        }
    }
    else if (nSize != 0)
    {
        SCompressedTextureDataTrailer* pTrailer;

        pTrailer = (SCompressedTextureDataTrailer*)((unsigned char*)pRaster -
                                                    26);
        if (pTrailer->nType == RESTYPE_DDS)
        {
            ReleaseCompressedTextureInternal(pTrailer->pOwner);
            return;
        }
    }
    else if (nBytesPerPixel == 2)
    {
        S16BitTextureDataTrailer* pTrailer;

        pTrailer = (S16BitTextureDataTrailer*)((unsigned char*)pRaster -
                                               22);
        if (pTrailer->nType == RESTYPE_4PC)
        {
            Release16BitTextureInternal(pTrailer->pOwner);
            return;
        }
    }
    else
    {
        STGATextureTrailer* pTrailer;

        pTrailer = (STGATextureTrailer*)((unsigned char*)pRaster +
                                         nBytesPerPixel * nHeight * nWidth +
                                         sizeof(unsigned long));
        if (pTrailer->nType == RESTYPE_TGA)
        {
            ReleaseTextureInternal(pTrailer->pOwner);
        }
    }
}
// PC 0x0070C920  Xbox 0x002F6F20
void* CAuroraInterface::GetResource(char* szName, char* szExtension, int* pnSize)
{
    char* szActualExtension;
    CExoString sName;
    char szBuffer[256];
    CAuroraModel* pModel;
    CAuroraModelX* pModelX;
    CAuroraTXI* pTXI;
    CAuroraVisibility* pVisibility;
    CAuroraTexture* pTexture;
    CAuroraProcessedTexture* pProcessedTexture;

    if (szExtension == NULL || pnSize == NULL)
    {
        return 0;
    }
    szActualExtension = szExtension;
    if (*szExtension == '.')
    {
        szActualExtension++;
    }
    sprintf(szBuffer, "%s", szActualExtension);
    strupr(szBuffer);
    sName = szName;
    sName = sName.LowerCase();
    if (strcmp(szBuffer, "MDL") == 0)
    {
        pModel = GetModelInternal(sName, 0, NULL, NULL);
        if (pModel != NULL)
        {
            *pnSize = pModel->GetModelDataSize();
            return pModel->GetModelDataPtr();
        }
    }
    else if (strcmp(szBuffer, "MDX") == 0)
    {
        pModelX = GetModelX(sName);
        if (pModelX != NULL)
        {
            *pnSize = pModelX->GetModelDataSize();
            return pModelX->GetModelDataPtr();
        }
    }
    else if (strcmp(szBuffer, "TXI") == 0)
    {
        pTXI = GetTXI(sName);
        if (pTXI == NULL)
        {
            pProcessedTexture = GetProcessedTexture(sName);
            if (pProcessedTexture != NULL)
            {
                *pnSize = -1 - pProcessedTexture->GetProcessedTextureTXISize();
                return pProcessedTexture->GetProcessedTextureTXIPtr();
            }
        }
        else
        {
            *pnSize = pTXI->GetTXIDataSize();
            return pTXI->GetTXIDataPtr();
        }
    }
    else if (strcmp(szBuffer, "TGA") == 0)
    {
        pTexture = GetTexture(sName);
        if (pTexture != NULL)
        {
            *pnSize = pTexture->GetTextureDataSize();
            return pTexture->GetTextureDataPtr();
        }
    }
    else if (strcmp(szBuffer, "TPC") == 0)
    {
        pProcessedTexture = GetProcessedTexture(sName);
        if (pProcessedTexture != NULL)
        {
            *pnSize = pProcessedTexture->GetProcessedTextureDataSize();
            return pProcessedTexture->GetProcessedTextureDataPtr();
        }
    }
    else if (strcmp(szBuffer, "VIS") == 0)
    {
        pVisibility = GetVisibility(sName);
        if (pVisibility != NULL)
        {
            *pnSize = pVisibility->GetVisibilityDataSize();
            return pVisibility->GetVisibilityDataPtr();
        }
    }
    return 0;
}
// PC 0x0070CBD0  Xbox 0x002F72D0
void CAuroraInterface::ReleaseResource(void* pResource, int nSize)
{
    unsigned char* pData;
    unsigned short nType;
    int nTextureType;

    pData = (unsigned char*)pResource;
    if (nSize >= 0)
    {
        pData -= 6;
        nType = *(unsigned short*)pData;
        pData += sizeof(unsigned short);
        switch (nType)
        {
        case RESTYPE_MDL:
            ReleaseModelInternal(*(CAuroraModel**)pData);
            break;
        case RESTYPE_TXI:
            ReleaseTXIInternal(*(CAuroraTXI**)pData);
            break;
        case RESTYPE_VIS:
            ReleaseVisibilityInternal(*(CAuroraVisibility**)pData);
            break;
        case RESTYPE_MDX:
            ReleaseModelXInternal(*(CAuroraModelX**)pData);
            break;
        }
    }
    else
    {
        pData -= nSize;
        pData += 3;
        nTextureType = *(unsigned short*)pData;
        pData += sizeof(unsigned short);
#ifdef _XBOX
        if (nTextureType >= 3006 && nTextureType <= 3007)
#else
        if (nTextureType >= 3006 && nTextureType <= RESTYPE_TPC)
#endif
        {
            ReleaseProcessedTextureInternal(*(CAuroraProcessedTexture**)pData);
        }
    }
}
// PC 0x0070CC60  Xbox 0x002F7360
int CAuroraInterface::ReadRasterHeader(char* szName, int* pnWidth,
                                       int* pnHeight, int* pnFormat)
{
    if (strlen(szName) > 16)
    {
        return 0;
    }
    CAuroraTexture texture(szName, 0);
    return texture.ReadTextureHeader(pnWidth, pnHeight, pnFormat);
}
// PC 0x0070CD10  Xbox 0x002F7410
int CAuroraInterface::ReadProcessedRasterHeader(char* szName, int* pnWidth,
                                                int* pnHeight, char* pnFormat,
                                                int* pnSize, int* pnTXISize,
                                                float* pfAlpha, char* pnMips)
{
    if (strlen(szName) > 16)
    {
        return 0;
    }
    CAuroraProcessedTexture texture(szName, 0);
    return texture.ReadProcessedTextureHeader(
        pnWidth, pnHeight, pnFormat, pnSize, pnTXISize, pfAlpha, pnMips);
}
// PC 0x0070CDD0  Xbox 0x002F74D0
int CAuroraInterface::ReadCompressedRasterHeader(char* szName, int* pnWidth,
                                                 int* pnHeight, int* pnFormat,
                                                 int* pnSize, float* pfAlpha)
{
    if (strlen(szName) > 16)
    {
        return 0;
    }
    CAuroraCompressedTexture texture(szName, 0);
    return texture.ReadTextureHeader(pnWidth, pnHeight, pnFormat, pnSize,
                                     pfAlpha);
}
// PC 0x0070CE80  Xbox 0x002F7580
int CAuroraInterface::Get16BitRasterHeader(char* szName, int* pnWidth,
                                           int* pnHeight, float* pfAlpha,
                                           int* pnFormat)
{
    if (strlen(szName) > 16)
    {
        return 0;
    }
    CAurora16BitTexture texture(szName, 0);
    return texture.ReadTextureHeader(pnWidth, pnHeight, pfAlpha, pnFormat);
}
// PC 0x0070CF30
int CAuroraInterface::LoadTexturePack(unsigned char nPack,
                                      const CExoString& sName)
{
    if (nPack >= 4)
    {
        return 0;
    }
    if (sName.GetLength() == 0)
    {
        return 0;
    }
    if (g_pExoResMan->AddEncapsulatedResourceFile(sName, 2) == 0)
    {
        return 0;
    }
    m_pCurrentTexturePack[nPack] = sName;
    return 1;
}
// PC 0x0070CF80
int CAuroraInterface::UnloadTexturePack(unsigned char nPack)
{
    CExoString sName;

    if (nPack >= 4)
    {
        return 0;
    }
    sName = m_pCurrentTexturePack[nPack];
    if (sName.GetLength() != 0)
    {
        if (g_pExoResMan->RemoveEncapsulatedResourceFile(sName) == 0)
        {
            return 0;
        }
        m_pCurrentTexturePack[nPack] = "";
    }
    return 1;
}
// PC 0x0070D060
CExoString CAuroraInterface::GetCurrentTexturePack(unsigned char nPack)
{
    if (nPack >= 4)
    {
        return NULL;
    }
    return m_pCurrentTexturePack[nPack];
}

#ifndef _XBOX
int CAuroraInterface::GetNearestVideoMode(unsigned long nWidth,  // PC 0x0070D0A0
                                          unsigned long nHeight,
                                          unsigned char nBitsPerPixel,
                                          unsigned long nRefreshRate)
{
    DWORD nMode;
    int nBestMode;
    DEVMODEA mode;
    DEVMODEA bestMode;

    memset(&bestMode, 0, sizeof(bestMode));
    memset(&mode, 0, sizeof(mode));
    nMode = 0;
    mode.dmSize = sizeof(mode);
    nBestMode = -1;
    while (EnumDisplaySettingsA(NULL, nMode, &mode))
    {
        if (mode.dmPelsWidth >= VIDEO_MIN_WIDTH &&
            mode.dmPelsHeight >= VIDEO_MIN_HEIGHT &&
            mode.dmBitsPerPel >= VIDEO_MIN_BITS_PER_PIXEL &&
            (mode.dmDisplayFrequency >= VIDEO_MIN_REFRESH_RATE ||
             mode.dmDisplayFrequency == VIDEO_REFRESH_DEFAULT ||
             mode.dmDisplayFrequency == VIDEO_REFRESH_DEFAULT_ALTERNATE))
        {
            int nBitDifference = abs(mode.dmBitsPerPel - nBitsPerPixel);
            int nBestBitDifference = abs(bestMode.dmBitsPerPel - nBitsPerPixel);
            if (nBitDifference < nBestBitDifference)
            {
                memcpy(&bestMode, &mode, sizeof(mode));
                nBestMode = nMode;
            }
            else if (nBitDifference == nBestBitDifference)
            {
                int nWidthDifference = abs(mode.dmPelsWidth - nWidth);
                int nBestWidthDifference = abs(bestMode.dmPelsWidth - nWidth);
                if (nWidthDifference < nBestWidthDifference)
                {
                    memcpy(&bestMode, &mode, sizeof(mode));
                    nBestMode = nMode;
                }
                else if (nWidthDifference == nBestWidthDifference)
                {
                    int nHeightDifference = abs(mode.dmPelsHeight - nHeight);
                    int nBestHeightDifference =
                        abs(bestMode.dmPelsHeight - nHeight);
                    if (nHeightDifference < nBestHeightDifference)
                    {
                        memcpy(&bestMode, &mode, sizeof(mode));
                        nBestMode = nMode;
                    }
                    else if (nHeightDifference == nBestHeightDifference)
                    {
                        int nRefreshDifference =
                            abs(mode.dmDisplayFrequency - nRefreshRate);
                        int nBestRefreshDifference =
                            abs(bestMode.dmDisplayFrequency - nRefreshRate);
                        if (nRefreshDifference < nBestRefreshDifference)
                        {
                            memcpy(&bestMode, &mode, sizeof(mode));
                            nBestMode = nMode;
                        }
                    }
                }
            }
        }
        memset(&mode, 0, sizeof(mode));
        nMode++;
        mode.dmSize = sizeof(mode);
    }
    return nBestMode;
}
#endif

#ifndef _XBOX
int CAuroraInterface::GetExactVideoMode(unsigned long nWidth,  // PC 0x0070D220  Xbox 0x002F7630
                                        unsigned long nHeight,
                                        unsigned char nBitsPerPixel,
                                        unsigned long nRefreshRate)
{
    DWORD nMode;
    DEVMODEA mode;

    memset(&mode, 0, sizeof(mode));
    nMode = 0;
    mode.dmSize = sizeof(mode);
    if (EnumDisplaySettingsA(NULL, 0, &mode))
    {
        while (mode.dmPelsWidth != nWidth || mode.dmPelsHeight != nHeight ||
               mode.dmBitsPerPel != nBitsPerPixel ||
               (mode.dmDisplayFrequency != nRefreshRate &&
                mode.dmDisplayFrequency != VIDEO_REFRESH_DEFAULT &&
                mode.dmDisplayFrequency != VIDEO_REFRESH_DEFAULT_ALTERNATE))
        {
            memset(&mode, 0, sizeof(mode));
            nMode++;
            mode.dmSize = sizeof(mode);
            if (!EnumDisplaySettingsA(NULL, nMode, &mode))
            {
                return -1;
            }
        }
        return nMode;
    }
    return -1;
}
#else
// PC 0x0070D220  Xbox 0x002F7630
int CAuroraInterface::GetExactVideoMode(unsigned long nWidth,
                                        unsigned long nHeight,
                                        unsigned char nBitsPerPixel,
                                        unsigned long nRefreshRate)
{
    return 1;
}
#endif
// PC 0x0070D340  Xbox 0x002F76B0
CAuroraInterface::~CAuroraInterface()
{
    AURORAINTERFACE_TRACE();
    CAuroraModel* pModel;

    while (m_lstRequestedModel.m_pInternal->m_pHead != NULL)
    {
        pModel = (CAuroraModel*)m_lstRequestedModel.m_pInternal->RemoveHead();
        if (pModel != NULL)
        {
            pModel->Unload();
            delete pModel;
        }
    }
    AurClearPalette();
}

int CAuroraInterface::GetHighestPriorityTextureFormat(
    CResRef resRef, int bDDS, int b4PC, int bTGA, int bTPC)
{
    unsigned long nDDS = CExoResMan::TABLE_NONE;
    unsigned long n4PC = CExoResMan::TABLE_NONE;
    unsigned long nTGA = CExoResMan::TABLE_NONE;
    unsigned long nTPC = CExoResMan::TABLE_NONE;

    if (bTPC && !g_pExoResMan->Exists(resRef, RESTYPE_TPC, &nTPC))
    {
        nTPC = CExoResMan::TABLE_NONE;
    }
    if (bDDS && !g_pExoResMan->Exists(resRef, RESTYPE_DDS, &nDDS))
    {
        nDDS = CExoResMan::TABLE_NONE;
    }
    if (b4PC && !g_pExoResMan->Exists(resRef, RESTYPE_4PC, &n4PC))
    {
        n4PC = CExoResMan::TABLE_NONE;
    }
    if (bTGA && !g_pExoResMan->Exists(resRef, RESTYPE_TGA, &nTGA))
    {
        nTGA = CExoResMan::TABLE_NONE;
    }
    if (bTPC && nTPC == CExoResMan::TABLE_DIRECTORY)
    {
        return RESTYPE_TPC;
    }
    if (bDDS && nDDS == CExoResMan::TABLE_DIRECTORY)
    {
        return RESTYPE_DDS;
    }
    if (b4PC && n4PC == CExoResMan::TABLE_DIRECTORY)
    {
        return RESTYPE_4PC;
    }
    if (bTGA && nTGA == CExoResMan::TABLE_DIRECTORY)
    {
        return RESTYPE_TGA;
    }
    if (bTPC && nTPC == CExoResMan::TABLE_RESOURCE_IMAGE)
    {
        return RESTYPE_TPC;
    }
    if (bDDS && nDDS == CExoResMan::TABLE_RESOURCE_IMAGE)
    {
        return RESTYPE_DDS;
    }
    if (b4PC && n4PC == CExoResMan::TABLE_RESOURCE_IMAGE)
    {
        return RESTYPE_4PC;
    }
    if (bTGA && nTGA == CExoResMan::TABLE_RESOURCE_IMAGE)
    {
        return RESTYPE_TGA;
    }
    if (bTPC && nTPC == CExoResMan::TABLE_ENCAPSULATED)
    {
        return RESTYPE_TPC;
    }
    if (bDDS && nDDS == CExoResMan::TABLE_ENCAPSULATED)
    {
        return RESTYPE_DDS;
    }
    if (b4PC && n4PC == CExoResMan::TABLE_ENCAPSULATED)
    {
        return RESTYPE_4PC;
    }
    if (bTGA && nTGA == CExoResMan::TABLE_ENCAPSULATED)
    {
        return RESTYPE_TGA;
    }
    if (bTPC && nTPC == CExoResMan::TABLE_KEY)
    {
        return RESTYPE_TPC;
    }
    if (bDDS && nDDS == CExoResMan::TABLE_KEY)
    {
        return RESTYPE_DDS;
    }
    if (b4PC && n4PC == CExoResMan::TABLE_KEY)
    {
        return RESTYPE_4PC;
    }
    if (bTGA && nTGA == CExoResMan::TABLE_KEY)
    {
        return RESTYPE_TGA;
    }
    return 0;
}
// PC 0x0070D380  Xbox 0x002F76F0
unsigned char* CAuroraInterface::GetCompressedRaster(char* szName, int* pnWidth,
                                          int* pnHeight, int* pnBpp,
                                          int* pnSize, float* pfAlpha)
{
    AURORAINTERFACE_TRACE();
    CAuroraCompressedTexture* pTexture;
    unsigned char nBitsPerPixel;
    int nWidth;
    int nHeight;
    int nDataSize;
    float fAlpha;
    if (GetHighestPriorityTextureFormat(szName, 1, 1, 1, 0) !=
        RESTYPE_DDS)
    {
        return 0;
    }
    {
        CExoString sName(szName);
        pTexture = GetCompressedTexture(sName);
    }
    if (pTexture == NULL)
    {
        return 0;
    }
    pTexture->GetCompressedTextureAttrib(
        (unsigned long*)&nWidth, (unsigned long*)&nHeight, &nBitsPerPixel,
        (unsigned long*)&nDataSize, &fAlpha);
    *pnWidth = nWidth;
    *pnHeight = nHeight;
    *pnBpp = nBitsPerPixel;
    *pnSize = nDataSize;
    *pfAlpha = fAlpha;
    return pTexture->GetCompressedTextureDataPtr();
}
// PC 0x0070D510  Xbox 0x002F7880
unsigned char* CAuroraInterface::GetProcessedRaster(char* szName, int* pnWidth,
                                         int* pnHeight, char* pnBpp,
                                         int* pnSize, int* pnTXISize,
                                         int* pnMips, float* pfAlpha,
                                         char* pnFormat)
{
    AURORAINTERFACE_TRACE();
    CAuroraProcessedTexture* pTexture;
    if (GetHighestPriorityTextureFormat(szName, 1, 1, 1, 1) !=
        RESTYPE_TPC)
    {
        return 0;
    }
    {
        CExoString sName(szName);
        pTexture = GetProcessedTexture(sName);
    }
    if (pTexture == NULL)
    {
        return 0;
    }
    pTexture->GetProcessedTextureAttrib(pnWidth, pnHeight, pnBpp, pnSize,
                                        pnTXISize, pnMips, pfAlpha, pnFormat);
    return pTexture->GetProcessedTextureDataPtr();
}
// PC 0x0070D9C0  Xbox 0x002F7C20
CAuroraInterface::CAuroraInterface()
{
    AurGetResource = GetResource;
    AurReleaseResource = ReleaseResource;
    AurGetRaster = GetRaster;
    AurGetRasterHeader = ReadRasterHeader;
    AurReleaseRaster = ReleaseRaster;
    AurGetLayeredRaster = GetLayeredRaster;
    AurReleaseLayeredRaster = ReleaseLayeredRaster;
    AurGetS3TCImage = GetCompressedRaster;
    AurGetS3TCHeader = ReadCompressedRasterHeader;
    AurGet16BitRaster = Get16BitRaster;
    AurGet16BitRasterHeader = Get16BitRasterHeader;
    AurGetProcessedRaster = GetProcessedRaster;
    AurGetProcessedRasterHeader = ReadProcessedRasterHeader;
    AurTextureSetDim(0, 0);
    AurTextureSetMem(g_nMaxTexMem);
}
// PC 0x0070D6B0  Xbox 0x002F7A20
unsigned char* CAuroraInterface::Get16BitRaster(char* szName, int* pnWidth, int* pnHeight,
                                     float* pfAlpha, int* pnFormat)
{
    AURORAINTERFACE_TRACE();
    CAurora16BitTexture* pTexture;
    int nWidth;
    int nHeight;
    float fAlpha;
    int nTextureFormat;
    if (GetHighestPriorityTextureFormat(szName, 0, 1, 1, 0) !=
        RESTYPE_4PC)
    {
        return 0;
    }
    {
        CExoString sName(szName);
        pTexture = Get16BitTexture(sName);
    }
    if (pTexture == NULL)
    {
        return 0;
    }
    pTexture->GetTextureAttrib((unsigned long*)&nWidth,
                               (unsigned long*)&nHeight, &fAlpha,
                               &nTextureFormat);
    *pnWidth = nWidth;
    *pnHeight = nHeight;
    *pfAlpha = fAlpha;
    *pnFormat = nTextureFormat;
    return pTexture->GetTextureDataPtr();
}
// PC 0x0070D800
int CAuroraInterface::SetTexturePack(unsigned char nPack,
                                     const CExoString& sName)
{
    AURORAINTERFACE_TRACE();
    int bSameTexturePack;

    if (nPack >= 4)
    {
        return 0;
    }
    bSameTexturePack = sName == GetCurrentTexturePack(nPack);
    if (bSameTexturePack != 0)
    {
        return 1;
    }
    if (UnloadTexturePack(nPack) == 0)
    {
        return 0;
    }
    return LoadTexturePack(nPack, sName);
}
// PC 0x0070D8D0  Xbox 0x002F7B70
void CAuroraInterface::RequestModel(char* szName)
{
    AURORAINTERFACE_TRACE();
    int bNew;
    CExoLinkedListNode* pPosition;
    CAuroraModel* pModel;

    bNew = 0;
    {
        CExoString sName(szName);
        pModel = GetModelInternal(sName, 1, &bNew, &pPosition);
    }
    if (pModel != NULL)
    {
        if (bNew != 0)
        {
            if (pPosition != NULL)
            {
                m_lstRequestedModel.m_pInternal->AddBefore(pModel, pPosition);
            }
            else
            {
                m_lstRequestedModel.m_pInternal->AddTail(pModel);
            }
        }
    }
}

namespace RenderInterface
{
inline bool IsResident(char* szName)
{
    return szName != NULL && stricmp(szName, "NULL") != 0
               ? (unsigned char)(BinaryFindModel(szName) != NULL)
               : (unsigned char)false;
}
}
// PC 0x0070D980
int CAuroraInterface::IsResident(char* szName)
{
    AURORAINTERFACE_TRACE();
    return RenderInterface::IsResident(szName);
}

static inline void AuroraInterfaceTrace()
{
}
