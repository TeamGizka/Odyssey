#ifndef AURIMAGE_H
#define AURIMAGE_H

#include <stdio.h>

class Vector;

unsigned char* ImageReadProcessed(char*, int*, int*, char*, int*, int*,
                                   int*, float*, char*);
int ImageReadProcessedHeader(char*, int*, int*, char*, int*, int*, float*, char*);

enum ImageS3TCFormats
{
    IMAGE_S3TC_FORMAT_16_BYTE_BLOCK = 4
};

#pragma pack(push, 1)

struct TGAOutHeader
{
    unsigned char nIDLength;
    unsigned char nColorMapType;
    unsigned char nImageType;
    unsigned char aColorMapSpec[5];
    short nXOrigin;
    short nYOrigin;
    short nWidth;
    short nHeight;
    unsigned char nBitsPerPixel;
    unsigned char nDescriptor;
};

#pragma pack(pop)

#define IMAGE_MAX_SCHEME_SIZE 15

#define IMAGE_PERLIN_BIAS 127

extern int g_nImagesDecoded;
extern int shortcircuitdecode;
#ifdef _XBOX
extern int g_nImagesDownSampled;

void ImageCopy(unsigned char* pDest, unsigned char* pSource, int nWidth, int nHeight, int nBPP);
unsigned char* ImageDownSampleBy(int nWidth, int nHeight, unsigned char* pImage, int nFactor);
unsigned char* ImageDownSample(int nWidth, int nHeight, int nBPP, unsigned char* pImage);
unsigned char* ImageSkipS3TCLevels(int nWidth, int nHeight, int nBPP, unsigned char* pImage, int nLevels);
#endif

void ImageFlipY(unsigned char* pImage, int nWidth, int nHeight, int nBPP);
void ImageDup(unsigned char* pImage, int nWidth, int nHeight, int nBPP);
unsigned char* ImageDup(unsigned char* pImage, int nWidth, int nHeight, int nBPP,
                        int bCompressed);
unsigned char* ImageScale(unsigned char* pImage, int nSrcWidth, int nSrcHeight,
                          int nBPP, int nDestWidth, int nDestHeight);
void ImageGammaCorrect(int nWidth, int nHeight, int nBPP, unsigned char* pImage,
                       float fGamma);
unsigned char* ImageDecode(unsigned char* pImage, int nWidth, int nHeight,
                           unsigned char* pPalette, int nPaletteCount,
                           int* pnScheme, int nSchemeSize);
unsigned char* ImageRead16bit(char* sFileName, int* pnWidth, int* pnHeight,
                              float* pfScale, int* pnReserved);
int ImageRead16bitHeader(char* sFileName, int* pnWidth, int* pnHeight,
                         float* pfScale, int* pnReserved);
int ImageReadS3TCHeader(FILE* pFile, int* pnWidth, int* pnHeight, int* pnFormat,
                        int* pnSize, float* pfScale);
int ImageReadS3TCHeader(char* sFileName, int* pnWidth, int* pnHeight,
                        int* pnFormat, int* pnSize, float* pfScale);
int ImageGetS3TCSize(int nWidth, int nHeight, int nFormat);
int ImageReadTGAHeader(FILE* pFile, int* pnWidth, int* pnHeight, int* pnBPP);
unsigned char* ImageReadTGAImage(FILE* pFile, int nWidth, int nHeight,
                                 int nBPP);
unsigned char* ImageReadTGA(char* sFileName, int* pnWidth, int* pnHeight,
                            int* pnBPP);
unsigned char* ImageRead(char* sFileName, int* pnWidth, int* pnHeight,
                         int* pnBPP);
int ImageReadHeader(char* sFileName, int* pnWidth, int* pnHeight, int* pnBPP);
unsigned char* ImageReadPLT(char* sFileName, int* pnWidth, int* pnHeight,
                            int* pnReserved);
void ImageJulia(int nWidth, int nHeight, int nBPP, unsigned char* pImage,
                float fCReal, float fCImaginary);
unsigned char ImageInterpolate(unsigned char* pImage, float fX, float fY,
                               int nWidth, int nHeight);
unsigned char* ImageBuildPerlin(int nSize, float fScale);
unsigned char* ImageBuildParameterPerlin(int nSize, float fScale, int nRange, int nBase);
unsigned char ImageGetPerlinPixel(unsigned char** ppImages, int nRow, int nColumn,
                        int nStart, int nLevels, int nSize);
float ImageGetAlphaMean(unsigned char* pImage, int nWidth, int nHeight,
                        int nBPP);
void ImageLowpassLowQualityBlur(unsigned char* pImage, unsigned char* pSource,
                                int nWidth, int nHeight, int nSection);
void ImageLowpassLowQualityBlur2(unsigned char* pImage, int nWidth,
                                 int nHeight);
void ImageLowpassBlur(unsigned char* pImage, int nWidth, int nHeight, int nBPP);
unsigned char* ImageGetSubImage(unsigned char* pImage, int nWidth, int nHeight,
                                int nBPP, int nX, int nY, int nSubWidth,
                                int nSubHeight);
unsigned char* ImageQuarterRotated(unsigned char* pImage, int nWidth,
                                   int nHeight, int nBPP, int nRotation);
void ImageFillAlpha(unsigned char* pImage, int nWidth, int nHeight, int nBPP,
                    const Vector& vColor);
void ImageWriteTGA(char* sFileName, int nWidth, int nHeight, int nBPP,
                   unsigned char* pImage, bool bFlipY = true);
unsigned char* ImageReadS3TC(FILE* pFile, int* pnWidth, int* pnHeight,
                             int* pnFormat, int* pnSize, float* pfScale);
unsigned char* ImageReadS3TC(char* sFileName, int* pnWidth, int* pnHeight,
                             int* pnFormat, int* pnSize, float* pfScale);

#endif
