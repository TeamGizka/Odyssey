#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../math3d/math3d.h"
#include "exoresman.h"
#include "aurimage.h"
// PC 0x0045D990  Xbox 0x002CB6E0
void ImageFlipY(unsigned char* pImage, int nWidth, int nHeight, int nBPP)
{
    int nSize = nBPP * nHeight * nWidth;
    unsigned char* pBuffer = new unsigned char[nSize];
    int i;
    int j;

    for (i = 0; i < nHeight; i++)
    {
        for (j = 0; j < nWidth; j++)
        {
            memcpy(&pBuffer[nBPP * (j + nWidth * (nHeight - i - 1))],
                   &pImage[nBPP * (j + nWidth * i)], nBPP);
        }
    }
    memcpy(pImage, pBuffer, nSize);
    delete[] pBuffer;
}
// PC 0x0045DA80  Xbox 0x002CB7D0
void ImageDup(unsigned char* pImage, int nWidth, int nHeight, int nBPP)
{
    int nSize;
    unsigned char* pPixel;

    if (nBPP >= 3)
    {
        nSize = nWidth * nHeight * nBPP;
        for (pPixel = pImage; pPixel - pImage < nSize; pPixel += nBPP)
        {
            unsigned char nTemp = pPixel[0];
            pPixel[0] = pPixel[2];
            pPixel[2] = nTemp;
        }
    }
}
// Xbox 0x002CD040
unsigned char* ImageDup(unsigned char* pImage, int nWidth, int nHeight, int nBPP,
                        int bCompressed)
{
    int nSize;

#ifdef _XBOX
    if (!bCompressed)
    {
        nSize = nWidth * nHeight * nBPP;
    }
    else
    {
        nSize = ImageGetS3TCSize(nWidth, nHeight, nBPP);
    }

    unsigned char* pCopy = (unsigned char*)operator new(nSize);
#else
    if (bCompressed)
    {
        nSize = ImageGetS3TCSize(nWidth, nHeight, nBPP);
    }
    else
    {
        nSize = nWidth * nHeight * nBPP;
    }

    unsigned char* pCopy = new unsigned char[nSize];
#endif
    memcpy(pCopy, pImage, nSize);
    return pCopy;
}
// PC 0x0045DAD0
unsigned char* ImageScale(unsigned char* pImage, int nSrcWidth, int nSrcHeight,
                          int nBPP, int nDestWidth, int nDestHeight)
{
    int nSize = nDestHeight * nDestWidth * nBPP;
    if (nSize == 0)
        return 0;
    unsigned char* pScaled = new unsigned char[nSize];
    int nRowBytes = nDestWidth * nBPP * sizeof(unsigned int);
    unsigned int* pRow = new unsigned int[nDestWidth * nBPP];
    memset(pRow, 0, nRowBytes);

    int nDestRemainingY = nDestHeight;
    int nDestY = 0;
    int nSrcY = 0;
    int nSrcRemainingY = nSrcHeight;
    int nDestRemainingX = 0;
    while (nDestY < nDestHeight)
    {
        int nHeight = nSrcRemainingY;
        if (nDestRemainingY < nHeight)
            nHeight = nDestRemainingY;
        int nSrcX = 0;
        int nSrcRemainingX = nSrcWidth;
        nDestRemainingX = nDestWidth;
        int nDestX = 0;
        while (nDestX < nDestWidth)
        {
            if (nDestRemainingX >= nSrcRemainingX)
            {
                for (int i = 0; i < nBPP; ++i)
                    pRow[nDestX * nBPP + i] += nHeight * nSrcRemainingX *
                        pImage[(nSrcY * nSrcWidth + nSrcX) * nBPP + i];
                nDestRemainingX -= nSrcRemainingX;
                if (nDestRemainingX == 0)
                {
                    nDestRemainingX = nDestWidth;
                    ++nSrcX;
                }
                nSrcRemainingX = nSrcWidth;
                ++nDestX;
            }
            else
            {
                for (int i = 0; i < nBPP; ++i)
                    pRow[nDestX * nBPP + i] += nDestRemainingX * nHeight *
                        pImage[(nSrcY * nSrcWidth + nSrcX) * nBPP + i];
                nSrcRemainingX -= nDestRemainingX;
                nDestRemainingX = nDestWidth;
                ++nSrcX;
            }
        }
        if (nDestRemainingY >= nSrcRemainingY)
        {
            int nArea = nSrcHeight * nSrcWidth;
            for (nDestX = 0; nDestX < nDestWidth; ++nDestX)
            {
                for (int i = 0; i < nBPP; ++i)
                    pScaled[(nDestY * nDestWidth + nDestX) * nBPP + i] =
                        (nArea / 2 + pRow[nDestX * nBPP + i]) / nArea;
            }
            nDestRemainingY -= nSrcRemainingY;
            if (nDestRemainingY == 0)
            {
                nDestRemainingY = nDestHeight;
                ++nSrcY;
            }
            ++nDestY;
            nSrcRemainingY = nSrcHeight;
            memset(pRow, 0, nRowBytes);
        }
        else
        {
            nSrcRemainingY -= nDestRemainingY;
            nDestRemainingY = nDestHeight;
            ++nSrcY;
        }
    }
    delete[] pRow;
    return pScaled;
}
// PC 0x0045DD90  Xbox 0x002CBD20
void ImageGammaCorrect(int nWidth, int nHeight, int nBPP, unsigned char* pImage,
                       float fGamma)
{
    int nSize;
    int i;

    if (fGamma != 1.0f)
    {
        nSize = nHeight * nWidth * nBPP;
        for (i = 0; i < nSize; i++)
        {
            float fInverse = 1.0f / fGamma;
#ifdef _XBOX
            pImage[i] =
                (unsigned char)(pow(pImage[i] * 0.003921569f, fInverse) *
                                255.0f);
#else
            pImage[i] =
                (unsigned char)(pow(pImage[i] * 0.003921569f, fInverse) *
                                255.0);
#endif
        }
    }
}

int shortcircuitdecode;
// PC 0x0045DE10  Xbox 0x002CBDA0
unsigned char* ImageDecode(unsigned char* pImage, int nWidth, int nHeight,
                           unsigned char* pPalette, int nPaletteCount,
                           int* pnScheme, int nSchemeSize)
{
    unsigned int aColors[IMAGE_MAX_SCHEME_SIZE * 256];
    unsigned int* pDecoded;
    unsigned int* pOut;
    int i;
    int j;

    g_nImagesDecoded++;
    if (shortcircuitdecode)
    {
        return NULL;
    }

    pDecoded = new unsigned int[nWidth * nHeight];
    for (i = 0; i < nSchemeSize; i++)
    {
        memcpy(&aColors[i * 256], &pPalette[pnScheme[i] * 1024], 1024);
    }

    pOut = pDecoded;
    for (i = 0; i < nHeight; i++)
    {
        for (j = 0; j < nWidth; j++)
        {
            *pOut++ = aColors[pImage[2 * (i * nWidth + j) + 1] * 256 +
                              pImage[2 * (i * nWidth + j)]];
        }
    }
    return (unsigned char*)pDecoded;
}
// PC 0x0045DF10  Xbox 0x002CBEA0
unsigned char* ImageRead16bit(char* sFileName, int* pnWidth, int* pnHeight,
                              float* pfScale, int* pnReserved)
{
    char sPath[256];
    FILE* pFile;
    unsigned char* pImage;

    strcpy(sPath, sFileName);
    pFile = fopen(sPath, "rb");
    if (pFile == NULL)
    {
        strcat(sPath, ".4pc");
        pFile = fopen(sPath, "rb");
        if (pFile == NULL)
        {
            return NULL;
        }
    }
    fread(pnWidth, 4, 1, pFile);
    fread(pnHeight, 4, 1, pFile);
    fread(pfScale, 4, 1, pFile);
    fread(pnReserved, 4, 1, pFile);
    pImage = new unsigned char[2 * *pnHeight * *pnWidth];
    fread(pImage, 1, 2 * *pnHeight * *pnWidth, pFile);
    fclose(pFile);
    return pImage;
}
// PC 0x0045E010  Xbox 0x002CBFA0
int ImageRead16bitHeader(char* sFileName, int* pnWidth, int* pnHeight,
                         float* pfScale, int* pnReserved)
{
    char sPath[256];
    FILE* pFile;

    strcpy(sPath, sFileName);
    pFile = fopen(sPath, "rb");
    if (pFile == NULL)
    {
        strcat(sPath, ".4pc");
        pFile = fopen(sPath, "rb");
        if (pFile == NULL)
        {
            return 0;
        }
    }
    fread(pnWidth, 4, 1, pFile);
    fread(pnHeight, 4, 1, pFile);
    fread(pfScale, 4, 1, pFile);
    fread(pnReserved, 4, 1, pFile);
    fclose(pFile);
    return 1;
}
// PC 0x0045E0F0  Xbox 0x002CC080
int ImageReadS3TCHeader(FILE* pFile, int* pnWidth, int* pnHeight, int* pnFormat,
                        int* pnSize, float* pfScale)
{
    if (pFile == NULL)
    {
        return 0;
    }
    *pnSize = 0;
    *pnWidth = 0;
    *pnHeight = 0;
    *pnFormat = 0;
    *pfScale = -1.0f;
    fread(pnWidth, 4, 1, pFile);
    fread(pnHeight, 4, 1, pFile);
    fread(pnFormat, 4, 1, pFile);
    fread(pnSize, 4, 1, pFile);
    fread(pfScale, 4, 1, pFile);
    return 1;
}
// PC 0x0045E180  Xbox 0x002CC120
int ImageReadS3TCHeader(char* sFileName, int* pnWidth, int* pnHeight,
                        int* pnFormat, int* pnSize, float* pfScale)
{
    char sPath[256];
    FILE* pFile;

    if (sFileName == NULL || *sFileName == 0)
    {
        return 0;
    }
    *pnSize = 0;
    *pnWidth = 0;
    *pnHeight = 0;
    *pnFormat = 0;
#ifdef _XBOX

    sprintf(sPath, "d:\\override\\%s.dds", sFileName);
    pFile = fopen(sPath, "rb");
    if (pFile != NULL)
    {
        ImageReadS3TCHeader(pFile, pnWidth, pnHeight, pnFormat, pnSize, pfScale);
        fclose(pFile);
        return 1;
    }
    return 0;
#else
    sprintf(sPath, "%s", sFileName);
    pFile = fopen(sPath, "rb");
    if (pFile == NULL)
    {
        sprintf(sPath, "%s.dds", sFileName);
        pFile = fopen(sPath, "rb");
        if (pFile == NULL)
        {
            return 0;
        }
    }
    ImageReadS3TCHeader(pFile, pnWidth, pnHeight, pnFormat, pnSize, pfScale);
    fclose(pFile);
    return 1;
#endif
}
// PC 0x0045E270  Xbox 0x002CC1F0
int ImageGetS3TCSize(int nWidth, int nHeight, int nFormat)
{
    int nSize = 0;
    int nBlockBytes = 8 * (nFormat == 4) + 8;

    while (true)
    {
        if (nWidth == 0)
        {
            if (nHeight == 0)
            {
                break;
            }
            nWidth = 1;
        }
        if (nHeight == 0)
        {
            nHeight = 1;
        }
        nSize += nBlockBytes * ((nHeight + 3) / 4) * ((nWidth + 3) / 4);
        nWidth >>= 1;
        nHeight >>= 1;
    }
    return nSize;
}
// PC 0x0045E2E0  Xbox 0x002CC260
int ImageReadTGAHeader(FILE* pFile, int* pnWidth, int* pnHeight, int* pnBPP)
{
    TGAHeader header;

    fread(&header, 18, 1, pFile);
    *pnWidth = header.nWidth;
    *pnHeight = header.nHeight;
    *pnBPP = header.nBitsPerPixel >> 3;
    return *pnBPP;
}
// PC 0x0045E320  Xbox 0x002CC2A0
unsigned char* ImageReadTGAImage(FILE* pFile, int nWidth, int nHeight, int nBPP)
{
    unsigned char* pImage = new unsigned char[nBPP * nHeight * nWidth];

    fread(pImage, 1, nBPP * nHeight * nWidth, pFile);
    return pImage;
}
// PC 0x0063E7F0
unsigned char* ImageReadProcessed(char*, int*, int*, char*, int*, int*,
                                   int*, float*, char*)
{
    return 0;
}
// PC 0x0063E7F0
int ImageReadProcessedHeader(char*, int*, int*, char*, int*, int*, float*, char*)
{
    return 0;
}
// PC 0x0045E350  Xbox 0x002CC2D0
unsigned char* ImageReadTGA(char* sFileName, int* pnWidth, int* pnHeight,
                            int* pnBPP)
{
    FILE* pFile;
    unsigned char* pImage;
    char sPath[256];

    if (sFileName != NULL && *sFileName != 0)
    {
#ifdef _XBOX

        sprintf(sPath, "d:\\override\\%s.tga", sFileName);
        pFile = fopen(sPath, "rb");
        if (pFile == NULL)
        {
            return NULL;
        }
        ImageReadTGAHeader(pFile, pnWidth, pnHeight, pnBPP);
        pImage = ImageReadTGAImage(pFile, *pnWidth, *pnHeight, *pnBPP);
        fclose(pFile);
        ImageDup(pImage, *pnWidth, *pnHeight, *pnBPP);
        return pImage;
#else
        sprintf(sPath, "%s", sFileName);
        pFile = fopen(sPath, "rb");
        if (pFile == NULL)
        {
            sprintf(sPath, "%s.tga", sFileName);
            pFile = fopen(sPath, "rb");
        }
        if (pFile != NULL)
        {
            ImageReadTGAHeader(pFile, pnWidth, pnHeight, pnBPP);
            pImage = ImageReadTGAImage(pFile, *pnWidth, *pnHeight, *pnBPP);
            fclose(pFile);
            ImageDup(pImage, *pnWidth, *pnHeight, *pnBPP);
            return pImage;
        }
#endif
    }
    return NULL;
}
// PC 0x0045E430  Xbox 0x002CC390
unsigned char* ImageRead(char* sFileName, int* pnWidth, int* pnHeight,
                         int* pnBPP)
{
    return ImageReadTGA(sFileName, pnWidth, pnHeight, pnBPP);
}
// PC 0x0045E440  Xbox 0x002CC3A0
int ImageReadHeader(char* sFileName, int* pnWidth, int* pnHeight, int* pnBPP)
{
    char sPath[256];
    FILE* pFile;

    if (sFileName == NULL || *sFileName == 0)
    {
        return 0;
    }
    *pnWidth = 0;
    *pnHeight = 0;
    *pnBPP = 0;
#ifdef _XBOX

    sprintf(sPath, "d:\\override\\%s.tga", sFileName);
    pFile = fopen(sPath, "rb");
    if (pFile != NULL)
    {
        ImageReadTGAHeader(pFile, pnWidth, pnHeight, pnBPP);
        fclose(pFile);
        return 1;
    }
    return 0;
#else
    sprintf(sPath, "%s", sFileName);
    pFile = fopen(sPath, "rb");
    if (pFile == NULL)
    {
        sprintf(sPath, "%s.tga", sFileName);
        pFile = fopen(sPath, "rb");
        if (pFile == NULL)
        {
            return 0;
        }
    }
    ImageReadTGAHeader(pFile, pnWidth, pnHeight, pnBPP);
    fclose(pFile);
    return 1;
#endif
}
// PC 0x0045E510  Xbox 0x002CC460
unsigned char* ImageReadPLT(char* sFileName, int* pnWidth, int* pnHeight,
                            int* pnReserved)
{
    char sPath[32];

    struct
    {
        char aSignature[16];
        int nWidth;
        int nHeight;
    } header;

    FILE* pFile;
    unsigned char* pImage;

    sprintf(sPath, "%s.plt", sFileName);
    pFile = fopen(sPath, "rb");
    if (pFile == NULL)
    {
        return NULL;
    }
    fread(&header, 24, 1, pFile);
    *pnWidth = header.nWidth;
    *pnHeight = header.nHeight;
    pImage = new unsigned char[2 * header.nWidth * header.nHeight];
    if (pImage == NULL)
    {
        return NULL;
    }
    fread(pImage, 1, 2 * header.nWidth * header.nHeight, pFile);
    fclose(pFile);
    return pImage;
}

void ImageJulia(int nWidth, int nHeight, int nBPP, unsigned char* pImage,
                float fCReal, float fCImaginary)
{
    unsigned char aColor[4] = { 0, 0, 0, 255 };

    for (int nY = 0; nY < nHeight; nY++)
    {
        for (int nX = 0; nX < nWidth; nX++)
        {
            float fImaginary = (float)nY / nHeight * 4.0f - 2.0f;
            float fReal = (float)nX / nWidth * 4.0f - 2.0f;
            int nIteration = 0;

            while (fReal * fReal + fImaginary * fImaginary < 4.0f &&
                   nIteration < 255)
            {
                float fNewReal = fReal * fReal - fImaginary * fImaginary + fCReal;

                fImaginary = 2.0f * fReal * fImaginary + fCImaginary;
                fReal = fNewReal;
                nIteration++;
            }
            aColor[0] = (unsigned char)((nIteration % 8) << 5);
            aColor[1] = (unsigned char)(((nIteration / 8) % 8) << 5);
            aColor[2] = (unsigned char)(((nIteration / 64) % 8) << 5);
            memcpy(pImage + (nX + nY * nWidth) * nBPP, aColor, nBPP);
        }
    }
}
// PC 0x0045E5B0  Xbox 0x002CC500
unsigned char ImageInterpolate(unsigned char* pImage, float fX, float fY,
                               int nWidth, int nHeight)
{
    int nX = (int)floor(fX);
    int nY = (int)floor(fY);
    float fXFraction = fX - nX;
    float fYFraction = fY - nY;
    int nNextX = (nX + 1) % nWidth;
    int nNextY = (nY + 1) % nHeight;
    float fValue =
        (pImage[nX + nY * nWidth] * (1.0f - fXFraction) +
         pImage[nNextX + nY * nWidth] * fXFraction) * (1.0f - fYFraction) +
        (pImage[nX + nNextY * nWidth] * (1.0f - fXFraction) +
         pImage[nNextX + nNextY * nWidth] * fXFraction) * fYFraction;
    int nResult;

    roundToInt(nResult, fValue);
    return (unsigned char)nResult;
}
// PC 0x0045E6B0  Xbox 0x002CC600
unsigned char* ImageBuildPerlin(int nSize, float fScale)
{
    unsigned char* pImage = new unsigned char[nSize * nSize];
    int i;

    for (i = 0; i < nSize * nSize; i++)
    {
        pImage[i] = (unsigned char)((rand() % 128 - 64) * fScale + 127.0f);
    }
    return pImage;
}
// PC 0x0045E710  Xbox 0x002CC660
unsigned char ImageGetPerlinPixel(unsigned char** ppImages, int nRow, int nColumn,
                        int nStart, int nLevels, int nSize)
{
    int nLevelSize = nStart;
    float fRow = (float)nRow / nSize;
    float fColumn = (float)nColumn / nSize;
    int nPixel = 0;

    for (int i = 0; i < nLevels; i++)
    {
        nPixel += ImageInterpolate(ppImages[i], nLevelSize * fRow,
                                    nLevelSize * fColumn, nLevelSize, nLevelSize) -
                  IMAGE_PERLIN_BIAS;
        nLevelSize *= 2;
    }
    return (unsigned char)nPixel + IMAGE_PERLIN_BIAS;
}
// PC 0x0045E790  Xbox 0x002CC700
float ImageGetAlphaMean(unsigned char* pImage, int nWidth, int nHeight,
                        int nBPP)
{
    float fSum;
    int nPixels;
    int i;

    if (nBPP < 4)
    {
        return 1.0f;
    }
#ifdef _XBOX

    nPixels = nWidth * nHeight;
    fSum = 0.0f;
    for (i = 0; i < nPixels; i++)
    {
        fSum += pImage[nBPP * i + 3] * 0.003921569f;
    }
#else
    fSum = 0.0f;
    nPixels = nWidth * nHeight;
    i = 0;
    if (nPixels >= 4)
    {
        do
        {
            fSum += pImage[nBPP * i + 3] * 0.003921569f;
            fSum += pImage[nBPP * (i + 1) + 3] * 0.003921569f;
            fSum += pImage[nBPP * (i + 2) + 3] * 0.003921569f;
            fSum += pImage[nBPP * (i + 3) + 3] * 0.003921569f;
            i += 4;
        } while (i + 3 < nPixels);
    }
    for (; i < nPixels; i++)
    {
        fSum += pImage[nBPP * i + 3] * 0.003921569f;
    }
#endif
    return fSum / nPixels;
}
// PC 0x0045E8B0  Xbox 0x002CC820
void ImageLowpassLowQualityBlur(unsigned char* pImage, unsigned char* pSource,
                                int nWidth, int nHeight, int nSection)
{
    static float filter[3][3] =
    {
        {0.0f, 0.1f, 0.0f},
        {0.1f, 0.5f, 0.1f},
        {0.0f, 0.1f, 0.0f}
    };

    int i;
    int j;
    int k;
    int l;

    for (i = nSection * nHeight / 3;
         i < (nSection + 1) * nHeight / 3; i++)
    {
        for (j = 0; j < nWidth; j++)
        {
            for (k = 0; k < 3; k++)
            {
                int nRow = (nHeight + i + k - 1) % nHeight;
                for (l = 0; l < 3; l++)
                {
                    if ((l != 0 && l != 2) || (k != 0 && k != 2))
                    {
                        pImage[i * nWidth + j] += (unsigned char)
                            (pSource[nRow * nWidth +
                                     (nWidth + j + l - 1) % nWidth] *
                             filter[k][l]);
                    }
                }
            }
        }
    }
}
// PC 0x0045EA70  Xbox 0x002CC950
void ImageLowpassLowQualityBlur2(unsigned char* pImage, int nWidth,
                                 int nHeight)
{
    static unsigned char* newimage = new unsigned char[nWidth * nHeight];
    static int oldheight = nHeight;
    static int oldwidth = nWidth;
    static unsigned char integerfilter[3][3] =
    {
        {0, 3, 0},
        {3, 1, 3},
        {0, 3, 0}
    };

    int i;
    int j;
    int k;
    int l;

    if (oldheight != nHeight || oldwidth != nWidth)
    {
        delete[] newimage;
        newimage = new unsigned char[nWidth * nHeight];
        oldheight = nHeight;
        oldwidth = nWidth;
    }
    memset(newimage, 0, nWidth * nHeight);
    for (i = 0; i < nHeight; i++)
    {
        for (j = 0; j < nWidth; j++)
        {
            for (k = 0; k < 3; k++)
            {
                int nRow = (nHeight + i + k - 1) % nHeight;
                for (l = 0; l < 3; l++)
                {
                    if ((l != 0 && l != 2) || (k != 0 && k != 2))
                    {
                        newimage[i * nWidth + j] +=
                            pImage[nRow * nWidth +
                                   (nWidth + j + l - 1) % nWidth] >>
                            integerfilter[k][l];
                    }
                }
            }
        }
    }
    memcpy(pImage, newimage, nWidth * nHeight);
}
// PC 0x0045ECB0  Xbox 0x002CCB90
void ImageLowpassBlur(unsigned char* pImage, int nWidth, int nHeight, int nBPP)
{
    float filter[3][3] = {
        {0.025f, 0.1f, 0.025f},
        {0.1f, 0.5f, 0.1f},
        {0.025f, 0.1f, 0.025f}
    };
    int nSize = nWidth * nHeight * nBPP;
    unsigned char* newimage = new unsigned char[nWidth * nHeight * nBPP];
    int x;
    int y;
    int i;
    int j;
    int k;
    int row;
    int column;

    memset(newimage, 0, nSize);
    for (y = 0; y < nHeight; y++)
    {
        for (x = 0; x < nWidth; x++)
        {
            for (j = 0; j < 3; j++)
            {
                row = (y + nHeight - 1 + j) % nHeight;
                for (i = 0; i < 3; i++)
                {
                    column = (x + nWidth - 1 + i) % nWidth;
                    for (k = 0; k < nBPP; k++)
                    {
                        newimage[(y * nWidth + x) * nBPP + k] +=
                            (unsigned char)(pImage[(row * nWidth + column) *
                                                  nBPP + k] * filter[j][i]);
                    }
                }
            }
        }
    }
    memcpy(pImage, newimage, nSize);
    delete[] newimage;
}
// PC 0x0045F000  Xbox 0x002CCD80
unsigned char* ImageGetSubImage(unsigned char* pImage, int nWidth, int nHeight,
                                int nBPP, int nX, int nY, int nSubWidth,
                                int nSubHeight)
{
    unsigned char* pSubImage = new unsigned char[nBPP * nSubWidth * nSubHeight];

    for (int y = 0; y < nSubHeight; y++)
    {
        for (int x = 0; x < nSubWidth; x++)
        {
            memcpy(pSubImage + (y * nSubWidth + x) * nBPP, pImage + ((y + nY) * nWidth + nX + x) * nBPP, nBPP);
        }
    }
    return pSubImage;
}
// PC 0x0045F0D0  Xbox 0x002CCE50
void ImageFillAlpha(unsigned char* pImage, int nWidth, int nHeight, int nBPP,
                    const Vector& vColor)
{
    if (nBPP == 4)
    {
        for (int i = 0; i < nHeight; i++)
        {
            for (int j = 0; j < nWidth; j++)
            {
                int nOffset = 4 * (j + nWidth * i);
                float fPixelRed = pImage[nOffset];
                float fPixelGreen = pImage[nOffset + 1];
                float fPixelBlue = pImage[nOffset + 2];
                float fPixelAlpha = pImage[nOffset + 3];
                Vector vPixel;
                vPixel.m_fX = fPixelRed / 255.0f;
                vPixel.m_fY = fPixelGreen / 255.0f;
                vPixel.m_fZ = fPixelBlue / 255.0f;
                float fAlpha = fPixelAlpha / 255.0f;
#ifdef _XBOX

                Vector vResult;
                vResult = fAlpha * vPixel + (1.0f - fAlpha) * vColor;
#else
                Vector vResult = fAlpha * vPixel + (1.0f - fAlpha) * vColor;
#endif
                int nRed, nGreen, nBlue;
                roundToInt(nRed, vResult.m_fX * 255.0f);
                roundToInt(nGreen, vResult.m_fY * 255.0f);
                roundToInt(nBlue, vResult.m_fZ * 255.0f);
                pImage[nOffset] = (unsigned char)nRed;
                pImage[nOffset + 1] = (unsigned char)nGreen;
                pImage[nOffset + 2] = (unsigned char)nBlue;
                pImage[nOffset + 3] = 255;
            }
        }
    }
}
// PC 0x0045F2A0  Xbox 0x002CD0A0
void ImageWriteTGA(char* sFileName, int nWidth, int nHeight, int nBPP,
                   unsigned char* pImage)
{
    TGAOutHeader header;
    FILE* pFile;
    int nSize;
    unsigned char* pBuffer;

    header.nIDLength = 0;
    header.nColorMapType = 0;
    header.nImageType = 2;
    header.aColorMapSpec[0] = 0;
    header.aColorMapSpec[1] = 0;
    header.aColorMapSpec[2] = 0;
    header.aColorMapSpec[3] = 0;
    header.aColorMapSpec[4] = 0;
    header.nXOrigin = 0;
    header.nYOrigin = 0;
    header.nWidth = nWidth;
    header.nHeight = nHeight;
    header.nBitsPerPixel = 8 * nBPP;
    header.nDescriptor = 0;

    pFile = fopen(sFileName, "wb");
    fwrite(&header, 0x12, 1, pFile);
#ifdef _XBOX

    pBuffer = ImageDup(pImage, nWidth, nHeight, nBPP, 0);
    ImageFlipY(pBuffer, nWidth, nHeight, nBPP);
    ImageDup(pBuffer, nWidth, nHeight, nBPP);
    nSize = nWidth * nHeight * nBPP;
#else
    nSize = nBPP * nHeight * nWidth;
    pBuffer = (unsigned char*)operator new(nSize);
    memcpy(pBuffer, pImage, nSize);
    ImageFlipY(pBuffer, nWidth, nHeight, nBPP);
    ImageDup(pBuffer, nWidth, nHeight, nBPP);
#endif
    fwrite(pBuffer, 1, nSize, pFile);
    operator delete(pBuffer);
    fclose(pFile);
}
// PC 0x0045F390  Xbox 0x002CD180
unsigned char* ImageReadS3TC(FILE* pFile, int* pnWidth, int* pnHeight,
                             int* pnFormat, int* pnSize, float* pfScale)
{
    if (pFile == NULL)
    {
        return NULL;
    }
    fread(pnWidth, 4, 1, pFile);
    fread(pnHeight, 4, 1, pFile);
    fread(pnFormat, 4, 1, pFile);
    fread(pnSize, 4, 1, pFile);
    fread(pfScale, 4, 1, pFile);

    int nBlockBytes = (*pnFormat == IMAGE_S3TC_FORMAT_16_BYTE_BLOCK) ? 16 : 8;
    unsigned char* pImage = new unsigned char[ImageGetS3TCSize(*pnWidth, *pnHeight, *pnFormat)];
    fread(pImage, 1, *pnSize, pFile);
    unsigned char* pMipImage = pImage;
    int nSize = *pnSize;
    int nWidth = *pnWidth >> 1;
    int nHeight = *pnHeight >> 1;

    while (nWidth != 0 || nHeight != 0)
    {
        if (nWidth == 0)
        {
            nWidth = 1;
        }
        if (nHeight == 0)
        {
            nHeight = 1;
        }
        pMipImage += nSize;
        nSize = ((nHeight + 3) / 4) * ((nWidth + 3) / 4) * nBlockBytes;
        fread(pMipImage, 1, nSize, pFile);
        nWidth >>= 1;
        nHeight >>= 1;
    }
    return pImage;
}
// PC 0x0045F4A0  Xbox 0x002CD290
unsigned char* ImageReadS3TC(char* sFileName, int* pnWidth, int* pnHeight,
                             int* pnFormat, int* pnSize, float* pfScale)
{
    char sPath[256];
    FILE* pFile;
    unsigned char* pImage;

    if (sFileName == NULL || *sFileName == 0)
    {
        return NULL;
    }
#ifdef _XBOX

    sprintf(sPath, "d:\\override\\%s.dds", sFileName);
    pFile = fopen(sPath, "rb");
    if (pFile == NULL)
    {
        return NULL;
    }
#else
    sprintf(sPath, "%s", sFileName);
    pFile = fopen(sPath, "rb");
    if (pFile == NULL)
    {
        sprintf(sPath, "%s.dds", sFileName);
        pFile = fopen(sPath, "rb");
        if (pFile == NULL)
        {
            return NULL;
        }
    }
#endif
    pImage = ImageReadS3TC(pFile, pnWidth, pnHeight, pnFormat, pnSize, pfScale);
    fclose(pFile);
    return pImage;
}

#ifdef _XBOX
// Xbox 0x002CB820
void ImageCopy(unsigned char* pDest, unsigned char* pSource, int nWidth, int nHeight, int nBPP)
{
    memcpy(pDest, pSource, nWidth * nHeight * nBPP);
}
// Xbox 0x002CB850
unsigned char* ImageDownSampleBy(int nWidth, int nHeight, unsigned char* pImage, int nFactor)
{
    int nNewWidth = nWidth / nFactor;
    g_nImagesDownSampled++;
    int nNewHeight = nHeight / nFactor;
    unsigned char* pDest = (unsigned char*)operator new(nNewHeight * nNewWidth * 4);
    int nRed = 0;
    int nGreen = 0;
    int nBlue = 0;
    int nAlpha = 0;
    for (int y = 0; y < nNewHeight; y++)
    {
        for (int x = 0; x < nNewWidth; x++)
        {
            for (int j = 0; j < nFactor; j++)
            {
                for (int i = 0; i < nFactor; i++)
                {
                    nRed += pImage[((y * nWidth + x) * nFactor + j * nWidth + i) * 4];
                    nGreen += pImage[((y * nWidth + x) * nFactor + j * nWidth + i) * 4 + 1];
                    nBlue += pImage[((y * nWidth + x) * nFactor + j * nWidth + i) * 4 + 2];
                    nAlpha += pImage[((y * nWidth + x) * nFactor + j * nWidth + i) * 4 + 3];
                }
            }
            nRed /= nFactor * nFactor;
            nGreen /= nFactor * nFactor;
            nBlue /= nFactor * nFactor;
            nAlpha /= nFactor * nFactor;
            pDest[(y * nNewWidth + x) * 4] = nRed;
            pDest[(y * nNewWidth + x) * 4 + 1] = nGreen;
            pDest[(y * nNewWidth + x) * 4 + 2] = nBlue;
            pDest[(y * nNewWidth + x) * 4 + 3] = nAlpha;
            nRed = nGreen = nBlue = nAlpha = 0;
        }
    }
    return pDest;
}
// Xbox 0x002CB9F0
unsigned char* ImageDownSample(int nWidth, int nHeight, int nBPP, unsigned char* pImage)
{
    g_nImagesDownSampled++;
    int nNewWidth = nWidth / 2;
    int nNewHeight = nHeight / 2;
    unsigned char* pDest = (unsigned char*)operator new(nNewHeight * nNewWidth * nBPP);
    int x;
    int y;
    int i;
    if (nBPP == 3 || nBPP == 4 || nBPP == 1)
    {
        for (y = 0; y < nNewHeight; y++)
        {
            for (x = 0; x < nNewWidth; x++)
            {
                for (i = 0; i < nBPP; i++)
                {
                    pDest[(y * nNewWidth + x) * nBPP + i] =
                        (pImage[((2 * y + 1) * nWidth + 2 * x) * nBPP + i] +
                         pImage[((2 * y + 1) * nWidth + 2 * x + 1) * nBPP + i] +
                         pImage[2 * (y * nWidth + x) * nBPP + nBPP + i] + pImage[2 * (y * nWidth + x) * nBPP + i]) /
                        4;
                }
            }
        }
    }
    else if (nBPP == 2)
    {
        for (y = 0; y < nNewHeight; y++)
        {
            for (x = 0; x < nNewWidth; x++)
            {
                for (i = 0; i < 2; i++)
                {
                    pDest[(y * nNewWidth + x) * 2 + i] = 0;
                    pDest[(y * nNewWidth + x) * 2 + i] =
                        (((pImage[((2 * y + 1) * nWidth + 2 * x + 1) * 2 + i] & 0xF0) +
                          (pImage[(2 * (y * nWidth + x) + 1) * 2 + i] & 0xF0) +
                          (pImage[((2 * y + 1) * nWidth + 2 * x) * 2 + i] & 0xF0) +
                          (pImage[2 * (y * nWidth + x) * 2 + i] & 0xF0)) /
                         4) &
                        0xF0;
                    pDest[(y * nNewWidth + x) * 2 + i] |=
                        (((pImage[((2 * y + 1) * nWidth + 2 * x + 1) * 2 + i] & 0x0F) +
                          (pImage[(2 * (y * nWidth + x) + 1) * 2 + i] & 0x0F) +
                          (pImage[((2 * y + 1) * nWidth + 2 * x) * 2 + i] & 0x0F) +
                          (pImage[2 * (y * nWidth + x) * 2 + i] & 0x0F)) /
                         4) &
                        0x0F;
                }
            }
        }
    }
    return pDest;
}
// Xbox 0x002CBC70
unsigned char* ImageSkipS3TCLevels(int nWidth, int nHeight, int nBPP, unsigned char* pImage, int nLevels)
{
    int i = 0;
    nBPP = nBPP == 4 ? 16 : 8;
    unsigned char* pLevel = pImage;
    pImage = NULL;
    while (nWidth != 0 || nHeight != 0)
    {
        if (i == nLevels)
        {
            pImage = pLevel;
        }
        if (nWidth == 0)
        {
            nWidth = 1;
        }
        if (nHeight == 0)
        {
            nHeight = 1;
        }
        pLevel += ((nWidth + 3) / 4) * ((nHeight + 3) / 4) * nBPP;
        i++;
        nWidth >>= 1;
        nHeight >>= 1;
    }
    unsigned char* pDest = (unsigned char*)operator new(pLevel - pImage);
    memcpy(pDest, pImage, pLevel - pImage);
    return pDest;
}

#endif
