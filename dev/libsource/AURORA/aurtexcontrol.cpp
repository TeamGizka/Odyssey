#include <string.h>
#include <math.h>
#include <stdlib.h>
#if defined(_WIN32) && !defined(_XBOX)
#include <windows.h>
#else
#include "platform.h"
#endif
#include "aurlist.h"
#include "aurtexcontrol.h"
#include "aurtexman.h"
#include "aurimage.h"
#include "math3d.h"

#include "aurparse.h"

char* firstword(char* sLine);
int Parse(char* sValue, int& nValue);
int Parse(char* sValue, float& fValue);
int rint(float fValue);
int enablerandomimpulses = 1;
int filterwater = 1;
int lowqualityblur2 = 1;
int lowqualityblur = 0;
unsigned char* ImageBuildPerlin(int nSize, float fScale);
unsigned char* ImageBuildParameterPerlin(int nSize, float fScale, int nRange, int nBase);
unsigned char ImageGetPerlinPixel(unsigned char** ppImages, int nRow, int nColumn,
                        int nStart, int nLevels, int nSize);

#define DIRTY_PERLIN_PERSISTENCE 0.5f
float amplitude = 0.03f;
float frequency1 = 60.0f;
float frequency2 = 25.0f;
float distortionspeed = 1.0f;
const float PERLIN_SPEED = 4.0f;

const float WATER_FORCE_SCALE = 1.0f;
const float WATER_DAMPING = 0.025f;
const float WATER_DIRECTION_OFFSET = 0.0f;
const float WATER_WAVE_HEIGHT_MARGIN = 40.0f;
const float WATER_SPEED = 1.0f;
// PC 0x00478C50
TextureController::TextureController(CAurTexture* pTexture)
    : m_lstChannelScale(), m_lstChannelTranslate(0)
{
    m_pTexture = pTexture;
    m_pImage = 0;
    m_nLastUpdate = 0;
    m_bDistort = 0;
    m_nDistortAngle = 1;
    m_fDistortAmplitude = 5.0f;
    m_fDistortSpeed = 1.0f;
}
// PC 0x00476AB0
TextureController::~TextureController()
{
    if (m_pImage != NULL)
    {
        delete[] m_pImage;
    }
}
// PC 0x00476B30
unsigned char* TextureController::Distort(unsigned char* pHeight)
{
    int nBPP = m_pTexture->GetBPP();
    int nWidth = m_pTexture->GetWidth();
    int nHeight = m_pTexture->GetHeight();
    int i = 0;
    int j;
    int nOffsetY;
    int nOffsetX;

    if (m_nDistortAngle != 0)
    {
        for (i = 0; i < nHeight; i++)
        {
            int nRow = i * nWidth;
            for (j = 0; j < nWidth; j++)
            {
                float fAngle = pHeight[nRow + j] * DISTORT_ANGLE_SCALE;
                roundToInt(nOffsetY, cosineLUT(fAngle) * m_fDistortAmplitude - 0.5f);
                roundToInt(nOffsetX, sineLUT(fAngle) * m_fDistortAmplitude - 0.5f);
                memcpy(newimage + (nRow + j) * nBPP,
                    m_pImage + (((i + nHeight + nOffsetY) & (nHeight - 1)) *
                        nWidth + ((j + nOffsetX + nWidth) & (nWidth - 1))) * nBPP,
                    nBPP);
            }
        }
    }
    else
    {
        for (i = 0; i < nHeight; i++)
        {
            int nRow = i * nWidth;
            for (j = 0; j < nWidth; j++)
            {
                float fHeight = (pHeight[nRow + j] - DISTORT_HEIGHT_BIAS) *
                    (1.0f / DISTORT_HEIGHT_RANGE);
                roundToInt(nOffsetY, fHeight * m_fDistortAmplitude - 0.5f);
                roundToInt(nOffsetX, fHeight * m_fDistortAmplitude - 0.5f);
                memcpy(newimage + (nRow + j) * nBPP,
                    m_pImage + (((i + nOffsetY) & (nHeight - 1)) * nWidth +
                        ((nOffsetX + j) & (nWidth - 1))) * nBPP, nBPP);
            }
        }
    }
    return newimage;
}

struct WaterPoint
{
    float fHeight;
    float fVelocity;
};

const float WAVE_SPEED = 1.0f;
const float ARTURO_UPDATE_TIME = 0.03f;

const float ARTURO_DAMPING = 0.0f;

unsigned char TextureController::newimage[262144];
// PC 0x00475B20
void TextureController::SetImage()
{
    int nSize =
        m_pTexture->GetWidth() * m_pTexture->GetHeight() * m_pTexture->GetBPP();

    m_pImage = new unsigned char[nSize];
    memcpy(m_pImage, m_pTexture->GetImage(), nSize);
}
// PC 0x00475B80
void PerlinTextureController::Init()
{
    int nSize;

    if (m_bDistort != 0)
    {
        SetImage();
    }

    m_fElapsed = 0.0f;
    nSize = m_pTexture->GetWidth() * m_pTexture->GetHeight();
    m_pPerlinImage = new unsigned char[nSize];
    m_pWorkImage = new unsigned char[nSize];
    BuildPerlinImage(m_pPerlinImage, m_pTexture->GetWidth());
    BuildPerlinImage(m_pWorkImage, m_pTexture->GetWidth());
}
// PC 0x00475C00
void PerlinTextureController::BuildPerlinImage(unsigned char* pImage, int nSize)
{
    int nImageSize = 4;
    float fScale = 1.0f;
    int nLevels = 1;
    int i;
    int j;
    unsigned char** ppImages;

    while ((4 << nLevels) <= nSize)
    {
        nLevels++;
    }

    ppImages = new unsigned char*[nLevels];
    for (i = 0; i < nLevels; i++, nImageSize *= 2)
    {
        ppImages[i] = ImageBuildPerlin(nImageSize, fScale);
        fScale *= 0.5f;
    }

    for (i = 0; i < nSize; i++)
    {
        for (j = 0; j < nSize; j++)
        {
            pImage[i * nSize + j] =
                ImageGetPerlinPixel(ppImages, i, j, 4, nLevels, nSize);
        }
    }

    for (i = 0; i < nLevels; i++)
    {
        delete[] ppImages[i];
    }
    delete[] ppImages;
}
// PC 0x00475CF0
void WaterTextureController::Init()
{
    int nWidth = m_nWaterWidth;
    int nHeight = m_nWaterHeight;
    int nPixels = m_nWaterWidth * nHeight;
    int i;
    int j;

    m_nUnknown64 = 0;
    if (m_bDistort != 0)
    {
        SetImage();
    }

    m_pfVelocity = new float[nPixels];
    m_pfHeight = new float[nPixels];
    m_pfNextHeight = new float[nPixels];
    for (i = 0; i < nHeight; i++)
    {
        for (j = 0; j < nWidth; j++)
        {
            m_pfVelocity[i * nWidth + j] = 0.0f;
            m_pfHeight[i * nWidth + j] = 127.0f;
        }
    }

    nWidth = m_pTexture->GetWidth();
    nHeight = m_pTexture->GetHeight();
    m_pWaterImage = new unsigned char[nWidth * nHeight];
    m_pDistortImage = new unsigned char[nWidth * nHeight];
    m_fForceAngle = 0.0f;
    m_fWaveAngle = 0.0f;
}
// PC 0x00475DB0
void WaveTextureController::Init()
{
    int nWidth = m_pTexture->GetWidth();
    int nHeight = m_pTexture->GetHeight();
    int nPixels = nHeight * nWidth;
    unsigned char* pImage;
    int i;
    int j;

    if (m_bDistort != 0)
    {
        SetImage();
    }

    m_pfWaveImage = new float[nPixels];
    m_pfOffsetImage = new float[nPixels];
    pImage = m_pTexture->GetImage();
    for (i = 0; i < nHeight; i++)
    {
        for (j = 0; j < nWidth; j++)
        {
            m_pfOffsetImage[i * nWidth + j] = 127.0f;
            if (pImage != NULL)
            {
                m_pfWaveImage[i * nWidth + j] =
                    pImage[m_pTexture->GetBPP() * (i * nWidth + j)];
            }
            else
            {
                m_pfWaveImage[i * nWidth + j] = 127.0f;
            }
            m_pfWaveImage[i * nWidth + j] = rand() % 256;
        }
    }
}
// PC 0x00475ED0
void LifeTextureController::Init()
{
    unsigned char aPixel[4] = {0, 0, 0, 255};
    int nWidth = m_pTexture->GetWidth();
    int nHeight = m_pTexture->GetHeight();
    int nBPP = m_pTexture->GetBPP();
    unsigned char* pImage = m_pTexture->GetImage();
    int i;
    int j;

    m_pState = new unsigned char[nBPP * nHeight * nWidth];
    for (i = 0; i < nHeight; i++)
    {
        for (j = 0; j < nWidth; j++)
        {
            aPixel[0] = 0;
            aPixel[1] = 0;
            aPixel[2] = 0;
            aPixel[3] = 255;
            if (rand() % 100 > 60)
            {
                aPixel[0] = 255;
            }
            memcpy(pImage + (i * nWidth + j) * nBPP, aPixel, nBPP);
            memcpy(m_pState + (i * nWidth + j) * nBPP, aPixel, nBPP);
        }
    }
}
// PC 0x00476000
void LifeTextureController::Control()
{

    if (m_nLastUpdate <= texcurrentframe - LIFE_UPDATE_FRAMES)
    {
        m_nLastUpdate = texcurrentframe;
        unsigned char aPixel[4] = {0, 0, 0, 255};
        int nWidth = m_pTexture->GetWidth();
        int nHeight = m_pTexture->GetHeight();
        int nBPP = m_pTexture->GetBPP();
        int nSize = nBPP * nHeight * nWidth;
        unsigned char* pOldState = new unsigned char[nSize];
        memcpy(pOldState, m_pState, nSize);
        int i;
        int j;
        int k;
        int l;
        for (i = 0; i < nHeight; i++)
        {
            for (j = 0; j < nWidth; j++)
            {
                int nNeighbors = 0;
                int nAlive = 0;
                for (k = -1; k <= 1; k++)
                {
                    for (l = -1; l <= 1; l++)
                    {
                        int nIndex = (((i + k + nHeight) % nHeight) * nWidth +
                                      (j + l + nWidth) % nWidth) * nBPP;
                        if (l == 0 && k == 0)
                        {
                            nAlive = pOldState[nIndex];
                        }
                        else if (pOldState[nIndex] > 0)
                        {
                            nNeighbors++;
                        }
                    }
                }
                int nNewAlive = 0;
                if (nAlive)
                {
                    if (nNeighbors == LIFE_SURVIVE_NEIGHBORS || nNeighbors == LIFE_BIRTH_NEIGHBORS)
                    {
                        nNewAlive = 1;
                    }
                }
                else if (nNeighbors == LIFE_BIRTH_NEIGHBORS)
                {
                    nNewAlive = 1;
                }
                if (nNewAlive)
                {
                    int nColor = 0;
                    if (nBPP > 2)
                    {
                        nColor = min(255, m_pState[(i * nWidth + j) * nBPP + k] + LIFE_COLOR_STEP);
                    }
                    aPixel[0] = 255;
                    aPixel[1] = nColor;
                    aPixel[2] = nColor;
                }
                else
                {
                    aPixel[0] = 0;
                    aPixel[1] = 0;
                    if (m_pState[(i * nWidth + j) * nBPP] != 0)
                    {
                        aPixel[2] = LIFE_DEATH_COLOR;
                    }
                    else if (nBPP > 2)
                    {
                        int nColor = m_pState[(i * nWidth + j) * nBPP + 2];
                        if (nColor - LIFE_COLOR_STEP < 0)
                        {
                            aPixel[2] = 0;
                        }
                        else
                        {
                            aPixel[2] = nColor - LIFE_COLOR_STEP;
                        }
                    }
                }
                memcpy(m_pState + (i * nWidth + j) * nBPP, aPixel, nBPP);
            }
        }
        m_pTexture->GetImage();
        unsigned char* pImage = new unsigned char[nSize];
        memcpy(pImage, m_pState, nSize);
        m_pTexture->glImage(true, NULL);
        m_pTexture->FreeImage();
        delete[] pOldState;
    }
}
// PC 0x004782E0
void ArturoTextureController::Init()
{
    int nImageWidth = m_pTexture->GetWidth();
    int nImageHeight = m_pTexture->GetHeight();
    int nPixels = m_nArturoHeight * m_nArturoWidth;
    int nImageSize = nImageHeight * nImageWidth;
    int i;
    int j;

    m_fArturoFrame = 0;
    if (m_bDistort != 0)
    {
        SetImage();
    }

    m_pfArturoCurrent = new float[nPixels];
    m_pfArturoPrevious = new float[nPixels];
    m_pfArturoNext = new float[nPixels];
    m_pArturoIndex = new unsigned char[nImageSize];
    for (i = 0; i < m_nArturoHeight; i++)
    {
        for (j = 0; j < m_nArturoWidth; j++)
        {
            float fX = (float)j / m_nArturoWidth * 2.0f - 1.0f;
            float fY = (float)i / m_nArturoHeight * 2.0f - 1.0f;

            m_pfArturoPrevious[i * m_nArturoWidth + j] = 0.0f;
            m_pfArturoCurrent[i * m_nArturoWidth + j] =
                (float)sin((1.0f - (float)sqrt(fY * fY + fX * fX)) *
                           PI) *
                127.0f;
        }
    }
    BuildArturoImage();
    BuildArturoImage();
}
// PC 0x00476310
void ArturoTextureController::BuildArturoImage()
{
    int nWidth = m_nArturoWidth;
    int nHeight = m_nArturoHeight;
    int nPixels = nHeight * nWidth;
    int nImpulseColumn = rand() % nWidth;
    int nImpulseRow = rand() % nHeight;
    int i;
    int j;

    for (i = 0; i < nHeight; i++)
    {
        for (j = 0; j < nWidth; j++)
        {
            float fOld = m_pfArturoCurrent[i * nWidth + j];
            float fValue = 0.0f;

            int nNeighbor;
            nNeighbor = i * nWidth + (j - 1 + nWidth) % nWidth;
            fValue += m_pfArturoCurrent[nNeighbor];
            nNeighbor = i * nWidth + (j + 1) % nWidth;
            fValue += m_pfArturoCurrent[nNeighbor];
            fValue += m_pfArturoCurrent[((i - 1 + nHeight) % nHeight) * nWidth + j];
            fValue += m_pfArturoCurrent[((i + 1) % nHeight) * nWidth + j];
            fValue *= 0.5f;
            fValue -= m_pfArturoPrevious[i * nWidth + j];
            fValue = fValue * (1.0f - ARTURO_DAMPING) + fOld * ARTURO_DAMPING;

            if (enablerandomimpulses && i == nImpulseRow &&
                j == nImpulseColumn)
            {
                fValue += rand() % ARTURO_IMPULSE_RANGE - ARTURO_IMPULSE_CENTER;
            }
            m_pfArturoNext[i * nWidth + j] =
                max(0.0f, min(ARTURO_CHANNEL_MAX, fValue + ARTURO_HEIGHT_BIAS)) -
                ARTURO_HEIGHT_BIAS;
        }
    }

    memcpy(m_pfArturoPrevious, m_pfArturoCurrent, nPixels * sizeof(float));
    memcpy(m_pfArturoCurrent, m_pfArturoNext, nPixels * sizeof(float));
}
// PC 0x00476720
float FloatInterpolate(float* pfValues, float fX, float fY, int nWidth,
                       int nHeight)
{
    int nColumn, nRow;
    roundToInt(nColumn, fX - 0.5f);
    roundToInt(nRow, fY - 0.5f);
    int nNextColumn = (nColumn + 1) % nWidth;
    int nNextRow = (nRow + 1) % nHeight;
    float fColumnWeight = fX - nColumn;
    float fRowWeight = fY - nRow;

    return (1.0f - fRowWeight) *
               ((1.0f - fColumnWeight) * pfValues[nRow * nWidth + nColumn] +
                fColumnWeight * pfValues[nRow * nWidth + nNextColumn]) +
           fRowWeight *
               ((1.0f - fColumnWeight) * pfValues[nNextRow * nWidth + nColumn] +
                fColumnWeight * pfValues[nNextRow * nWidth + nNextColumn]);
}
// PC 0x00476520
void CycleTIDTextureController::Control()
{
    if (m_nLastUpdate != texcurrentframe)
    {
        m_nLastUpdate = texcurrentframe;
        m_fElapsed += DeltaT;
        if (m_fElapsed > 1.0f / m_fCycleRate)
        {
            m_fElapsed = 0.0f;
            m_nLastTID = (m_nLastTID + 1) %
                         (m_pTexture->GetNumX() * m_pTexture->GetNumY());
            m_pTexture->IncrementTIDIndex(m_nLastTID);
        }
    }
}
// PC 0x004765A0
void RandomTextureController::Init()
{
    int nWidth = m_pTexture->GetWidth();
    int nHeight = m_pTexture->GetHeight();
    int nBPP = m_pTexture->GetBPP();

    if (m_bDistort != 0)
    {
        SetImage();
    }

    m_pRandomImage = new unsigned char[nWidth * nHeight];
    m_pChannelImage = new unsigned char[nWidth * nHeight * nBPP];
}
// PC 0x00476600
void DirtyTextureController::MakePerlinTexture(unsigned char* pImage, int nRange)
{
    int nWidth = m_pTexture->GetWidth();
    int nImageSize = 4;
    int nSize = m_pTexture->GetHeight();
    float fScale = 1.0f;
    int nLevels = 1;
    int i;
    int j;
    unsigned char** ppImages;

    while ((4 << nLevels) <= nSize)
    {
        nLevels++;
    }

    ppImages = new unsigned char*[nLevels];
    for (i = 0; i < nLevels; i++)
    {
        ppImages[i] = ImageBuildParameterPerlin(nImageSize, fScale, nRange, 0);
        nImageSize *= 2;
        fScale *= DIRTY_PERLIN_PERSISTENCE;
    }

    for (i = 0; i < nSize; i++)
    {
        for (j = 0; j < nSize; j++)
        {
            pImage[i * nSize + j] =
                ImageGetPerlinPixel(ppImages, i, j, 4, nLevels, nSize);
        }
    }

    for (i = 0; i < nLevels; i++)
    {
        delete[] ppImages[i];
    }
    delete[] ppImages;
}
// PC 0x004767F0
void DirtyTextureController::Control()
{
    if (m_nLastUpdate != texcurrentframe && m_pDirtyImage != NULL)
    {
        m_nLastUpdate = texcurrentframe;
    }
}
// PC 0x00476810
void RingDistortTextureController::Init()
{
    int nWidth = m_pTexture->GetWidth();
    int nHeight = m_pTexture->GetHeight();
    int nBPP = m_pTexture->GetBPP();

    m_fAngle = 0.0f;
    m_pRingImage = new unsigned char[nWidth * nHeight * nBPP];
}
// PC 0x00476850
void RingDistortTextureController::Control()
{
    if (m_nLastUpdate != texcurrentframe && m_pRingImage != NULL)
    {
        int nWidth;
        int nHeight;
        float fScale;
        int i;
        int j;
        int nPixel;

        m_nLastUpdate = texcurrentframe;
        nWidth = m_pTexture->GetWidth();
        nHeight = m_pTexture->GetHeight();
        m_pTexture->GetBPP();
#ifdef _XBOX
        nPixel = 0;
        fScale = 1.0f / nWidth;
#else
        fScale = 1.0f / nWidth;
        nPixel = 0;
#endif
        for (i = 0; i < nHeight; i++)
        {
            float fY = ((float)i + 1.0f) * fScale;

            for (j = 0; j < nWidth; j++)
            {
#ifdef _XBOX
                float fX = ((float)j + 1.0f) * fScale - 0.5f;
                float fDistance = sqrt(fX * fX + (fY - 0.5f) * (fY - 0.5f));
                float fRed = cosineLUT(fDistance * frequency1 + m_fAngle * frequency2) * amplitude + 0.5f;
                float fGreen = cosineLUT(fDistance * frequency1 + m_fAngle * frequency2) * amplitude + 0.5f;
                int nRed;
                int nGreen;

                fRed = max(0.0f, min(255.0f, fRed * 255.0f));
                fGreen = max(0.0f, min(255.0f, fGreen * 255.0f));
                roundToInt(nRed, fRed);
                roundToInt(nGreen, fGreen);
                m_pRingImage[nPixel] = 255;
                m_pRingImage[nPixel + 1] = (unsigned char)nGreen;
                m_pRingImage[nPixel + 2] = (unsigned char)nRed;
                m_pRingImage[nPixel + 3] = 255;
                nPixel += 4;
#else
                float fX = ((float)j + 1.0f) * fScale;
                float fDistance =
                    (fX - 0.5f) * (fX - 0.5f) + (fY - 0.5f) * (fY - 0.5f);

                fDistance = sqrt(fDistance);
                float fRed =
                    cosineLUT(fDistance * frequency1 + m_fAngle * frequency2) *
                        amplitude +
                    0.5f;
                float fGreen =
                    cosineLUT(fDistance * frequency1 + m_fAngle * frequency2) *
                        amplitude +
                    0.5f;

                fRed = max(0.0f, min(255.0f, fRed * 255.0f));
                fGreen = max(0.0f, min(255.0f, fGreen * 255.0f));
                m_pRingImage[nPixel + 2] = 255;
                m_pRingImage[nPixel + 1] = (unsigned char)fGreen;
                m_pRingImage[nPixel] = (unsigned char)fRed;
                m_pRingImage[nPixel + 3] = 255;
                nPixel += 4;
#endif
            }
        }
        m_pTexture->glImage(true, m_pRingImage);
        m_fAngle += distortionspeed * DeltaT;
        while (m_fAngle > TWO_PI)
        {
            m_fAngle -= TWO_PI;
        }
    }
}
// PC 0x00476E20
unsigned char* TextureController::Height2Channel(unsigned char* pHeight,
                                                unsigned char* pImage)
{
    int nHeight = m_pTexture->GetHeight();
    int nWidth = m_pTexture->GetWidth();
    int nBPP = m_pTexture->GetBPP();
    unsigned char* pResult;
    if (pImage == NULL)
        pResult = new unsigned char[nHeight * nWidth * nBPP];
    else
        pResult = pImage;

    for (int i = 0; i < nHeight; i++)
    {
        for (int j = 0; j < nWidth; j++)
        {
            int nPixel = (i * nWidth) + j;
            float fScale;
            float fTranslate;
            if (nBPP == CHANNEL_BYTE_COUNT)
            {
                unsigned char nHeightValue = pHeight[nPixel];
                for (int k = 0; k < nBPP; k++)
                {
                    int nOffset = nPixel * CHANNEL_BYTE_COUNT + k;
                    fScale = 1.0f;
                    fTranslate = 0.0f;
                    if (m_lstChannelScale.m_nCount > k)
                        fScale = m_lstChannelScale[k];
                    if (m_lstChannelTranslate.m_nCount > k)
                        fTranslate = m_lstChannelTranslate[k];
                    pResult[nOffset] = 0;
                    if (fScale > 0.0f)
                        pResult[nOffset] =
                            (unsigned char)(nHeightValue * fScale);
                    if (fTranslate > 0.0f)
                        pResult[nOffset] +=
                            (unsigned char)(fTranslate * CHANNEL_BYTE_MAX);
                }
            }
            else
            {
                unsigned char nHeightValue = pHeight[nPixel] >> CHANNEL_NIBBLE_BITS;
                for (int k = 0; k < nBPP; k++)
                {
                    int nOffset = nPixel * nBPP + k;
                    fScale = 1.0f;
                    fTranslate = 0.0f;
                    if (m_lstChannelScale.m_nCount > k * 2)
                        fScale = m_lstChannelScale[k * 2];
                    if (m_lstChannelTranslate.m_nCount > k * 2)
                        fTranslate = m_lstChannelTranslate[k * 2];
                    pResult[nOffset] = 0;
                    if (fScale > 0.0f)
                        pResult[nOffset] =
                            (unsigned char)(nHeightValue * fScale) & CHANNEL_LOW_MASK;
                    if (fTranslate > 0.0f)
                        pResult[nOffset] =
                            (pResult[nOffset] +
                             (unsigned char)(fTranslate * CHANNEL_NIBBLE_MAX)) & CHANNEL_LOW_MASK;
                    fScale = 1.0f;
                    fTranslate = 0.0f;
                    if (m_lstChannelScale.m_nCount > k * 2 + 1)
                        fScale = m_lstChannelScale[k * 2 + 1];
                    if (m_lstChannelTranslate.m_nCount > k * 2 + 1)
                        fTranslate = m_lstChannelTranslate[k * 2 + 1];
                    if (fScale > 0.0f)
                        pResult[nOffset] =
                            (pResult[nOffset] +
                             ((unsigned char)(nHeightValue * fScale) << CHANNEL_NIBBLE_BITS)) & CHANNEL_HIGH_MASK;
                    if (fTranslate > 0.0f)
                        pResult[nOffset] =
                            (pResult[nOffset] +
                             ((unsigned char)(fTranslate * CHANNEL_NIBBLE_MAX) << CHANNEL_NIBBLE_BITS)) & CHANNEL_HIGH_MASK;
                }
            }
        }
    }
    return pResult;
}
// PC 0x00477360
void PerlinTextureController::Control()
{
    if (m_nLastUpdate != texcurrentframe)
    {
        int nSize;
        unsigned char* pImage;
        unsigned char* pResult;
        int i;
        bool bDeleteResult;

        m_nLastUpdate = texcurrentframe;
        nSize = m_pTexture->GetWidth() * m_pTexture->GetWidth();
        m_fElapsed += DeltaT * PERLIN_SPEED;
        if (m_fElapsed > 1.0f)
        {
            memcpy(m_pPerlinImage, m_pWorkImage, nSize);
            BuildPerlinImage(m_pWorkImage, m_pTexture->GetWidth());
            while (m_fElapsed >= 1.0f)
            {
                m_fElapsed -= 1.0f;
            }
        }

        pImage = new unsigned char[nSize];
        for (i = 0; i < nSize; i++)
        {
            pImage[i] =
                (unsigned char)(m_pPerlinImage[i] * (1.0f - m_fElapsed) +
                                m_pWorkImage[i] * m_fElapsed);
        }

        m_pTexture->GetImage();
        bDeleteResult = false;
        if (m_bDistort != 0)
        {
            pResult = Distort(pImage);
        }
        else
        {
            bDeleteResult = true;
            pResult = Height2Channel(pImage, NULL);
        }
        delete[] pImage;
        m_pTexture->glImage(true, pResult);
        if (bDeleteResult)
        {
            delete[] pResult;
        }
    }
}
// PC 0x004775E0
void WaterTextureController::Control()
{
    if (m_nLastUpdate == texcurrentframe)
    {
        return;
    }
    m_nLastUpdate = texcurrentframe;
    if (lowqualityblur == 0)
    {
        if (m_nUnknown64 % 2 == 0)
        {
            m_nUnknown64 = 0;
        }
        else
        {
            ++m_nUnknown64;
            return;
        }
    }
    m_nWaveType = (m_nWaveType + 1) % 3;
    if (m_nWaveType == 0 || lowqualityblur == 0)
    {
        float fTime = (DeltaT < 0.04f ? DeltaT : 0.04f) * m_fDistortSpeed * WATER_SPEED;
        int nWidth = m_nWaterWidth;
        int nHeight = m_nWaterWidth;
        int x;
        int y;
        m_fForceAngle += fTime * m_fForceCycleSpeed * DEG_TO_RAD;
        if (m_fForceAngle > TWO_PI)
        {
            m_fForceAngle -= TWO_PI;
        }
        m_fWaveAngle += fTime * m_fAngleCycleSpeed * DEG_TO_RAD;
        if (m_fWaveAngle > TWO_PI)
        {
            m_fWaveAngle -= TWO_PI;
        }

        float fAmplitude = cosineLUT(m_fForceAngle) * 0.25f + 0.2f;
        float fDirection = cosineLUT(m_fWaveAngle) + WATER_DIRECTION_OFFSET;
        float afKernel[3][3] = {
            {0.5f, 1.0f, 0.5f},
            {1.0f, 0.0f, 1.0f},
            {0.5f, 1.0f, 0.5f}
        };
        afKernel[0][0] += cosineLUT(fDirection - 0.7853982f) * fAmplitude;
        afKernel[0][2] += cosineLUT(fDirection - 2.3561945f) * fAmplitude;
        afKernel[2][2] += cosineLUT(fDirection - 3.9269907f) * fAmplitude;
        afKernel[2][0] += cosineLUT(fDirection - 5.497787f) * fAmplitude;

        int nTextureWidth = m_pTexture->GetWidth();
        float fYScale = (float)m_nWaterHeight / m_pTexture->GetHeight();
        float fXScale = (float)m_nWaterWidth / m_pTexture->GetWidth();
        for (y = 0; y < nHeight; ++y)
        {
            for (x = 0; x < nWidth; ++x)
            {
                float fForce = 0.0f;
                float fVelocity = m_pfVelocity[y * nWidth + x];
                float fHeight = m_pfHeight[y * nWidth + x] - 127.0f;
                for (int j = 0; j < 3; ++j)
                {
                    for (int i = 0; i < 3; ++i)
                    {
                        int nColumn = (nWidth + x + i - 1) % nWidth;
                        int nRow = (y + j + nHeight - 1) % nHeight;
                        float fDifference;
                        if (j == 1 && i == 1)
                        {
                            fDifference = fHeight;
                        }
                        else
                        {
                            fDifference = -(m_pfHeight[nRow * nWidth + nColumn] -
                                            127.0f - fHeight);
                        }
                        fForce -= fDifference * afKernel[j][i] * WATER_FORCE_SCALE;
                    }
                }
                fForce -= WATER_DAMPING * fVelocity;
                WaterPoint sNext;
                sNext.fHeight = fHeight + fVelocity * fTime + 0.5f * fForce * fTime * fTime;
                sNext.fVelocity = fVelocity + fForce * fTime;
                m_pfVelocity[y * nWidth + x] = sNext.fVelocity;
                m_pfNextHeight[y * nWidth + x] = sNext.fHeight;
            }
        }
        if (enablerandomimpulses != 0)
        {
            y = rand() % nHeight;
            x = rand() % nWidth;
            int nImpulse = rand() % 127;
            m_pfVelocity[y * nWidth + x] += nImpulse;
        }
        for (y = 0; y < nHeight; ++y)
        {
            for (x = 0; x < nWidth; ++x)
            {
                float fHeight = m_pfNextHeight[y * nWidth + x] + 127.0f;
                if (fHeight < WATER_WAVE_HEIGHT_MARGIN)
                {
                    fHeight = WATER_WAVE_HEIGHT_MARGIN;
                    m_pfVelocity[y * nWidth + x] = 0.0f;
                }
                if (fHeight > 255.0f - WATER_WAVE_HEIGHT_MARGIN)
                {
                    fHeight = 255.0f - WATER_WAVE_HEIGHT_MARGIN;
                    m_pfVelocity[y * nWidth + x] = 0.0f;
                }
                m_pfHeight[y * nWidth + x] = fHeight;
            }
        }
        for (y = 0; y < m_pTexture->GetHeight(); ++y)
        {
            float fY = y * fYScale;
            for (x = 0; x < nTextureWidth; ++x)
            {
                float fHeight = FloatInterpolate(m_pfHeight, x * fXScale, fY,
                                                 m_nWaterWidth, m_nWaterHeight);
                if (lowqualityblur != 0)
                {
                    m_pDistortImage[y * nTextureWidth + x] = (unsigned char)fHeight;
                }
                else
                {
                    m_pWaterImage[y * nTextureWidth + x] = (unsigned char)fHeight;
                }
            }
        }
        if (lowqualityblur != 0)
        {
            memset(m_pWaterImage, 0, nTextureWidth * m_pTexture->GetHeight());
        }
    }

    if (filterwater != 0)
    {
        if (lowqualityblur != 0)
        {
            ImageLowpassLowQualityBlur(m_pWaterImage, m_pDistortImage,
                m_pTexture->GetWidth(), m_pTexture->GetHeight(), m_nWaveType);
        }
        else if (lowqualityblur2 != 0)
        {
            ImageLowpassLowQualityBlur2(m_pWaterImage,
                m_pTexture->GetWidth(), m_pTexture->GetHeight());
        }
        else
        {
            ImageLowpassBlur(m_pWaterImage,
                m_pTexture->GetWidth(), m_pTexture->GetHeight(), 1);
        }
    }
    if (m_nWaveType == 2 || lowqualityblur == 0)
    {
        unsigned char* pResult = m_pWaterImage;
        bool bDeleteResult = false;
        if (m_pTexture->GetBPP() != 1)
        {
            if (m_bDistort != 0)
            {
                pResult = Distort(m_pWaterImage);
            }
            else
            {
                pResult = Height2Channel(m_pWaterImage, NULL);
                bDeleteResult = true;
            }
        }
        m_pTexture->glImage(true, pResult);
        if (bDeleteResult)
        {
            delete[] pResult;
        }
        ++m_nUnknown64;
    }
}
// PC 0x00477DF0
void WaveTextureController::Control()
{
    if (m_nLastUpdate == texcurrentframe)
    {
        return;
    }
    m_nLastUpdate = texcurrentframe;

    int nWidth = m_pTexture->GetWidth();
    int nHeight = m_pTexture->GetHeight();
    int x, y;
    float fTime = ((DeltaT < 0.04f ? DeltaT : 0.04f) * WAVE_SPEED);
    float afKernel[3][3] = {
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f}
    };
    afKernel[1][0] = fTime * fTime;
    afKernel[0][1] = fTime * fTime;
    afKernel[2][1] = fTime * fTime;
    afKernel[1][2] = fTime * fTime;
    afKernel[1][1] = (2.0f - (4.0f * (fTime * fTime)));

    for (y = 0; y < nHeight; ++y)
    {
        for (x = 0; x < nWidth; ++x)
        {
            float fHeight = 0.0f;
            float fOldHeight = m_pfOffsetImage[y * nWidth + x];
            for (int j = 0; j < 3; ++j)
            {
                for (int i = 0; i < 3; ++i)
                {
                    int nColumn = (nHeight + x + i - 1) % nWidth;
                    int nRow = (nWidth + y + j - 1) % nHeight;
                    fHeight += m_pfWaveImage[nRow * nWidth + nColumn] * afKernel[j][i];
                }
            }
            m_pfOffsetImage[y * nWidth + x] =
                fHeight - fOldHeight;
        }
    }

    float* pImage = m_pfWaveImage;
    m_pfWaveImage = m_pfOffsetImage;
    m_pfOffsetImage = pImage;

    if (enablerandomimpulses != 0)
    {
        int y = rand() % nHeight;
        int x = rand() % nWidth;
        float fImpulse = (float)(rand() % 32);
        m_pfOffsetImage[y * nWidth + x] += fImpulse;
    }

    for (y = 0; y < nHeight; ++y)
    {
        for (x = 0; x < nWidth; ++x)
        {
            float fHeight = m_pfWaveImage[y * nWidth + x];
            if (fHeight < WATER_WAVE_HEIGHT_MARGIN)
            {
                fHeight = WATER_WAVE_HEIGHT_MARGIN;
            }
            if (fHeight > 255.0f - WATER_WAVE_HEIGHT_MARGIN)
            {
                fHeight = 255.0f - WATER_WAVE_HEIGHT_MARGIN;
            }
            m_pfWaveImage[y * nWidth + x] = fHeight;
        }
    }

    unsigned char* pHeight = new unsigned char[nWidth * nHeight];
    for (y = 0; y < nHeight; ++y)
    {
        for (x = 0; x < nWidth; ++x)
        {
            pHeight[y * nWidth + x] = (unsigned char)m_pfWaveImage[y * nWidth + x];
        }
    }

    bool bDeleteResult = false;
    m_pTexture->IsBumpMap();
    unsigned char* pResult;
    if (m_bDistort != 0)
    {
        pResult = Distort(pHeight);
    }
    else
    {
        bDeleteResult = true;
        pResult = Height2Channel(pHeight, NULL);
    }
    m_pTexture->glImage(true, pResult);
    if (bDeleteResult)
    {
        delete[] pResult;
    }
}
// PC 0x00478410
void ArturoTextureController::Control()
{

    static float accumulatedtime = 0.0f;
    static int framemod = -1;

    accumulatedtime += DeltaT;
    if (accumulatedtime > ARTURO_UPDATE_TIME)
    {
        accumulatedtime -= (float)floor(accumulatedtime / ARTURO_UPDATE_TIME) *
            ARTURO_UPDATE_TIME;
        if (m_nLastUpdate != texcurrentframe)
        {
            m_nLastUpdate = texcurrentframe;
            framemod = (framemod + 1) % ARTURO_STAGE_COUNT;
            int nWidth = m_pTexture->GetWidth();
            int nHeight = m_pTexture->GetHeight();

            if (framemod == ARTURO_FIRST_HALF)
            {
                m_fArturoFrame += DeltaT * m_fDistortSpeed * ARTURO_SPEED_SCALE;
                if (m_fArturoFrame > 1.0f)
                {
                    BuildArturoImage();
                    while (m_fArturoFrame > 1.0f)
                    {
                        m_fArturoFrame -= 1.0f;
                    }
                }
            }

            if (m_bDistort != 0 &&
                (framemod < ARTURO_UPLOAD || framemod == ARTURO_FIRST_HALF))
            {
                float fStepX = (float)m_nArturoWidth / nWidth;
                float fStepY = (float)m_nArturoHeight / nHeight;
                int nStartRow = framemod * (nHeight / 2);
                int nEndRow = framemod == ARTURO_SECOND_HALF ?
                    nHeight : (framemod + 1) * (nHeight / 2);
                float fX = 0.0f;
                float fY = nStartRow * fStepY;
                for (int i = nStartRow; i < nEndRow; i++)
                {
                    for (int j = 0; j < nWidth; j++)
                    {
                        float fPrevious = FloatInterpolate(m_pfArturoPrevious,
                            fX, fY, m_nArturoWidth, m_nArturoHeight);
                        float fCurrent = FloatInterpolate(m_pfArturoCurrent,
                            fX, fY, m_nArturoWidth, m_nArturoHeight);
                        int nIndex;
                        roundToInt(nIndex, fPrevious * (1.0f - m_fArturoFrame) +
                            fCurrent * m_fArturoFrame + ARTURO_HEIGHT_BIAS);
                        m_pArturoIndex[i * nWidth + j] = (unsigned char)nIndex;
                        fX += fStepX;
                    }
                    fX = 0.0f;
                    fY += fStepY;
                }
            }

            if (framemod == ARTURO_UPLOAD)
            {
                unsigned char* pImage = m_pArturoIndex;
                bool bDeleteImage = false;
                if (m_pTexture->IsBumpMap() != true)
                {
                    if (m_bDistort != 0)
                    {
                        pImage = Distort(m_pArturoIndex);
                    }
                    else
                    {
                        bDeleteImage = true;
                        pImage = Height2Channel(m_pArturoIndex, NULL);
                    }
                }
                m_pTexture->glImage(true, pImage);
                if (bDeleteImage)
                {
                    delete[] pImage;
                }
            }
        }
    }
}
// PC 0x004786F0
void RandomTextureController::Control()
{
    int nWidth;
    int nHeight;
    int i;
    int j;
    unsigned char* pImage;

    if (m_nLastUpdate != texcurrentframe)
    {
        m_nLastUpdate = texcurrentframe;
        if (m_pChannelImage != NULL)
        {
            nWidth = m_pTexture->GetWidth();
            nHeight = m_pTexture->GetHeight();
            for (i = 0; i < nHeight; i++)
            {
                for (j = 0; j < nWidth; j++)
                {
                    m_pRandomImage[i * nWidth + j] = rand() % 256;
                }
            }

            if (m_bDistort != 0)
            {
                pImage = Distort(m_pRandomImage);
                m_pTexture->glImage(true, pImage);
            }
            else
            {
                m_pChannelImage =
                    Height2Channel(m_pRandomImage, m_pChannelImage);
                m_pTexture->glImage(true, m_pChannelImage);
            }
        }
    }
}
// PC 0x004787B0
void DirtyTextureController::Init(int nARGB, int nSize, int nFormula, int nFunc)
{
    int nBPP = m_pTexture->GetBPP();
    m_nDirtyFrame = 0;
    m_nBlendMode = nFormula;
    unsigned char* pNoiseX = new unsigned char[nSize * nSize];
    unsigned char* pNoiseY = new unsigned char[nSize * nSize];
    unsigned char* pNoiseZ = new unsigned char[nSize * nSize];
    m_pDirtyImage = new unsigned char[nBPP * nSize * nSize * nSize];
    m_pTexture->SetDepth(nSize);
    m_pTexture->SetHeight(nSize);
    m_pTexture->SetWidth(nSize);
    int nWidth = m_pTexture->GetWidth();
    int nHeight = m_pTexture->GetHeight();
    int nAlpha = nARGB >> 24;
    int nRed = (nARGB >> 16) & 0xff;
    int nGreen = (nARGB >> 8) & 0xff;
    int nBlue = nARGB & 0xff;
    m_pTexture->SetDepth(m_pTexture->GetHeight());

    int nOffset = 0;
    int nRange = nRed;
    nRange = nRange > nGreen ? nRange : nGreen;
    nRange = nRange > nBlue ? nRange : nBlue;

    if (nFunc == 1)
    {
        MakePerlinTexture(pNoiseX, nRange);
        MakePerlinTexture(pNoiseY, nRange);
        MakePerlinTexture(pNoiseZ, nRange);
    }

    for (int k = 0; k < nHeight; k++)
    {
        for (int i = 0; i < nWidth; i++)
        {
            for (int j = 0; j < nWidth; j++)
            {
                if (nFunc == 1)
                {
                    int nValue = pNoiseY[k * nHeight + j] + pNoiseX[k * nHeight + i] +
                                 pNoiseZ[i * nWidth + j];
                    if (nAlpha)
                        m_pDirtyImage[nOffset + 3] = nValue % nAlpha;
                    else
                        m_pDirtyImage[nOffset + 3] = 0;
                    if (nRed)
                        m_pDirtyImage[nOffset] = nValue % nRed;
                    else
                        m_pDirtyImage[nOffset] = 0;
                    if (nGreen)
                        m_pDirtyImage[nOffset + 1] = nValue % nGreen;
                    else
                        m_pDirtyImage[nOffset + 1] = 0;
                    if (nBlue)
                        m_pDirtyImage[nOffset + 2] = nValue % nBlue;
                    else
                        m_pDirtyImage[nOffset + 2] = 0;
                }
                else
                {
                    int nValue = (unsigned char)rand();
                    if (nAlpha == 0)
                        m_pDirtyImage[nOffset + 3] = 0;
                    else
                        m_pDirtyImage[nOffset + 3] = nValue % nAlpha;
                    if (nRed == 0)
                        m_pDirtyImage[nOffset] = 0;
                    else
                        m_pDirtyImage[nOffset] = nValue % nRed;
                    if (nGreen == 0)
                        m_pDirtyImage[nOffset + 1] = 0;
                    else
                        m_pDirtyImage[nOffset + 1] = nValue % nGreen;
                    if (nBlue == 0)
                        m_pDirtyImage[nOffset + 2] = 0;
                    else
                        m_pDirtyImage[nOffset + 2] = nValue % nBlue;
                }
                nOffset += 4;
            }
        }
    }

    delete[] pNoiseX;
    delete[] pNoiseY;
    delete[] pNoiseZ;
    m_pTexture->glImage(false, m_pDirtyImage);
}
// PC 0x00478CE0
void TextureController::ParseField(char* sField)
{

    if (stricmp("channelscale", firstword(sField)) == 0)
    {
        Parse(sField + 12, m_lstChannelScale);
    }
    if (stricmp("channeltranslate", firstword(sField)) == 0)
    {
        Parse(sField + 16, m_lstChannelTranslate);
    }
    if (stricmp("distort", firstword(sField)) == 0)
    {
        Parse(sField + 7, m_bDistort);
    }
    if (stricmp("distortangle", firstword(sField)) == 0)
    {
        Parse(sField + 12, m_nDistortAngle);
    }
    if (stricmp("distortionamplitude", firstword(sField)) == 0)
    {
        Parse(sField + 19, m_fDistortAmplitude);
    }
    if (stricmp("speed", firstword(sField)) == 0)
    {
        Parse(sField + 5, m_fDistortSpeed);
    }

    {
        float fValue = -1.0f;
        if (stricmp("channelscale0", firstword(sField)) == 0)
        {
            Parse(sField + 13, fValue);
        }
        if (fValue >= 0.0f)
        {
            if (m_lstChannelScale.m_nCount == 0)
            {
                m_lstChannelScale.SetSize(DEFAULT_CHANNEL_COUNT);
                for (int i = 0; i < DEFAULT_CHANNEL_COUNT; i++)
                {
                    m_lstChannelScale[i] = 1.0f;
                }
            }
            m_lstChannelScale[0] = fValue;
        }

    }
    {
        float fValue = -1.0f;
        if (stricmp("channelscale1", firstword(sField)) == 0)
        {
            Parse(sField + 13, fValue);
        }
        if (fValue >= 0.0f)
        {
            if (m_lstChannelScale.m_nCount == 0)
            {
                m_lstChannelScale.SetSize(DEFAULT_CHANNEL_COUNT);
                for (int i = 0; i < DEFAULT_CHANNEL_COUNT; i++)
                {
                    m_lstChannelScale[i] = 1.0f;
                }
            }
            m_lstChannelScale[1] = fValue;
        }

    }
    {
        float fValue = -1.0f;
        if (stricmp("channelscale2", firstword(sField)) == 0)
        {
            Parse(sField + 13, fValue);
        }
        if (fValue >= 0.0f)
        {
            if (m_lstChannelScale.m_nCount == 0)
            {
                m_lstChannelScale.SetSize(DEFAULT_CHANNEL_COUNT);
                for (int i = 0; i < DEFAULT_CHANNEL_COUNT; i++)
                {
                    m_lstChannelScale[i] = 1.0f;
                }
            }
            m_lstChannelScale[2] = fValue;
        }

    }
    {
        float fValue = -1.0f;
        if (stricmp("channelscale3", firstword(sField)) == 0)
        {
            Parse(sField + 13, fValue);
        }
        if (fValue >= 0.0f)
        {
            if (m_lstChannelScale.m_nCount == 0)
            {
                m_lstChannelScale.SetSize(DEFAULT_CHANNEL_COUNT);
                for (int i = 0; i < DEFAULT_CHANNEL_COUNT; i++)
                {
                    m_lstChannelScale[i] = 1.0f;
                }
            }
            m_lstChannelScale[3] = fValue;
        }

    }
    {
        float fValue = -1.0f;
        if (stricmp("channeltranslate0", firstword(sField)) == 0)
        {
            Parse(sField + 17, fValue);
        }
        if (fValue >= 0.0f)
        {
            if (m_lstChannelTranslate.m_nCount == 0)
            {
                m_lstChannelTranslate.SetSize(DEFAULT_CHANNEL_COUNT);
                for (int i = 0; i < DEFAULT_CHANNEL_COUNT; i++)
                {
                    m_lstChannelTranslate[i] = 1.0f;
                }
            }
            m_lstChannelTranslate[0] = fValue;
        }

    }
    {
        float fValue = -1.0f;
        if (stricmp("channeltranslate1", firstword(sField)) == 0)
        {
            Parse(sField + 17, fValue);
        }
        if (fValue >= 0.0f)
        {
            if (m_lstChannelTranslate.m_nCount == 0)
            {
                m_lstChannelTranslate.SetSize(DEFAULT_CHANNEL_COUNT);
                for (int i = 0; i < DEFAULT_CHANNEL_COUNT; i++)
                {
                    m_lstChannelTranslate[i] = 1.0f;
                }
            }
            m_lstChannelTranslate[1] = fValue;
        }

    }
    {
        float fValue = -1.0f;
        if (stricmp("channeltranslate2", firstword(sField)) == 0)
        {
            Parse(sField + 17, fValue);
        }
        if (fValue >= 0.0f)
        {
            if (m_lstChannelTranslate.m_nCount == 0)
            {
                m_lstChannelTranslate.SetSize(DEFAULT_CHANNEL_COUNT);
                for (int i = 0; i < DEFAULT_CHANNEL_COUNT; i++)
                {
                    m_lstChannelTranslate[i] = 1.0f;
                }
            }
            m_lstChannelTranslate[2] = fValue;
        }

    }
    {
        float fValue = -1.0f;
        if (stricmp("channeltranslate3", firstword(sField)) == 0)
        {
            Parse(sField + 17, fValue);
        }
        if (fValue >= 0.0f)
        {
            if (m_lstChannelTranslate.m_nCount == 0)
            {
                m_lstChannelTranslate.SetSize(DEFAULT_CHANNEL_COUNT);
                for (int i = 0; i < DEFAULT_CHANNEL_COUNT; i++)
                {
                    m_lstChannelTranslate[i] = 1.0f;
                }
            }
            m_lstChannelTranslate[3] = fValue;
        }
    }
}
// PC 0x00479290
WaterTextureController::WaterTextureController(CAurTexture* pTexture)
    : TextureController(pTexture), m_fForceCycleSpeed(RAD_TO_DEG),
      m_fAngleCycleSpeed(RAD_TO_DEG / 20.0f)
{
    m_pfHeight = 0;
    m_pfVelocity = 0;
    m_pfNextHeight = 0;
    m_nWaterHeight = m_nWaterWidth = 31;
    m_pWaterImage = 0;
    m_pDistortImage = 0;
    m_nWaveType = 2;
}
// PC 0x00479380
void WaterTextureController::ParseField(char* sField)
{
    const char* sKeyword;

    TextureController::ParseField(sField);
    sKeyword = firstword(sField);
    if (stricmp("forcecyclespeed", sKeyword) == 0)
    {
        Parse(sField + 15, m_fForceCycleSpeed);
    }
    sKeyword = firstword(sField);
    if (stricmp("anglecyclespeed", sKeyword) == 0)
    {
        Parse(sField + 15, m_fAngleCycleSpeed);
    }
    sKeyword = firstword(sField);
    if (stricmp("waterwidth", sKeyword) == 0)
    {
        Parse(sField + 10, m_nWaterWidth);
    }
    sKeyword = firstword(sField);
    if (stricmp("waterheight", sKeyword) == 0)
    {
        Parse(sField + 11, m_nWaterHeight);
    }
}
// PC 0x00479440
WaveTextureController::WaveTextureController(CAurTexture* pTexture)
    : TextureController(pTexture)
{
    m_pfWaveImage = 0;
    m_pfOffsetImage = 0;
}
// PC 0x004794F0
void WaveTextureController::ParseField(char* sField)
{
    TextureController::ParseField(sField);
}
// PC 0x00479500
ArturoTextureController::ArturoTextureController(CAurTexture* pTexture)
    : TextureController(pTexture)
{
    m_nArturoWidth = 15;
    m_nArturoHeight = 15;
    m_pArturoIndex = 0;
    m_pfArturoNext = 0;
    m_pfArturoCurrent = 0;
    m_pfArturoPrevious = 0;
    m_nLastUpdate = -1;
}
// PC 0x004795E0
void ArturoTextureController::ParseField(char* sField)
{
    const char* sKeyword;

    TextureController::ParseField(sField);
    sKeyword = firstword(sField);
    if (stricmp("arturowidth", sKeyword) == 0)
    {
        Parse(sField + 11, m_nArturoWidth);
    }
    sKeyword = firstword(sField);
    if (stricmp("arturoheight", sKeyword) == 0)
    {
        Parse(sField + 12, m_nArturoHeight);
    }
}
// PC 0x00479650
CycleTIDTextureController::CycleTIDTextureController(CAurTexture* pTexture)
    : TextureController(pTexture)
{
#ifdef _XBOX
    m_fCycleRate = 1.0f;
    m_nLastTID = 0;
    m_fElapsed = 0.0f;
#else
    m_nLastTID = 0;
    m_fElapsed = 0.0f;
    m_fCycleRate = 1.0f;
#endif
}
// PC 0x004015A0
void CycleTIDTextureController::Init()
{
}
// PC 0x004796B0
void CycleTIDTextureController::ParseField(char* sField)
{
    const char* sKeyword = firstword(sField);

    if (stricmp("fps", sKeyword) == 0)
    {
        Parse(sField + 3, m_fCycleRate);
    }
    TextureController::ParseField(sField);
}

#ifdef _XBOX

void HeatDistortTextureController::Init()
{
    int nWidth = m_pTexture->GetWidth();
    int nHeight = m_pTexture->GetHeight();
    int nBPP = m_pTexture->GetBPP();

    m_fAngle = 0.0f;
    m_pHeatImage = new unsigned char[nWidth * nHeight * nBPP];
}

void HeatDistortTextureController::Control()
{
    if (m_nLastUpdate != texcurrentframe)
    {
        m_nLastUpdate = texcurrentframe;
        if (m_pHeatImage != NULL)
        {
            int nWidth = m_pTexture->GetWidth();
            int nHeight = m_pTexture->GetHeight();
            int nPixel = 0;
            float fScale = TWO_PI / nHeight;
            float fPhase1 = m_fAngle * 2.67f;
            float fPhase2 = m_fAngle * 1.98f;
            float fPhase3 = m_fAngle * 0.9f;
            int i;
            int j;

            for (i = 0; i < nHeight; i++)
            {
                for (j = 0; j < nWidth; j++)
                {
                    float fY = ((float)i + 1.0f) * fScale;
                    float fWave1 = sineLUT(fY * 2.0f + fPhase1) * 0.021f;
                    float fWave2 = sineLUT(fY * 3.2f + fPhase2) * 0.0096f;
                    float fRed = fWave1 + fWave2 + 0.5f;
                    float fGreen = sineLUT(fY * 1.3f + fPhase3) * 0.05f + 0.5f;
                    int nRed;
                    int nGreen;

                    fRed = max(0.0f, min(255.0f, fRed * 255.0f));
                    fGreen = max(0.0f, min(255.0f, fGreen * 255.0f));
                    roundToInt(nRed, fRed);
                    roundToInt(nGreen, fGreen);
                    m_pHeatImage[nPixel] = 255;
                    m_pHeatImage[nPixel + 1] = (unsigned char)nGreen;
                    m_pHeatImage[nPixel + 2] = (unsigned char)nRed;
                    m_pHeatImage[nPixel + 3] = 255;
                    nPixel += 4;
                }
            }
            m_pTexture->glImage(true, m_pHeatImage);
            m_fAngle += DeltaT;
        }
    }
}

#endif
