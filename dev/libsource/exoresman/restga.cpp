#include "exoresman.h"
// PC 0x00786240
CResTGA::CResTGA()
{
    m_bDataOwned = 0;
    m_bColorMapped = 0;
    m_bRLE = 0;
    m_bServiced = 0;
    m_nDataSize = 0;
    m_nRLESize = 0;
    m_pColorMap = 0;
    m_pTGAData = 0;
    m_pHeader = 0;
    m_bAllocSuffix = 1;
}
// PC 0x00786280
CResTGA::~CResTGA()
{
}
// PC 0x00786290
int CResTGA::GetTGAAttrib(unsigned long* pnWidth, unsigned long* pnHeight, unsigned char* pnDepth)
{
    if (m_pHeader == 0)
    {
        return 0;
    }

    *pnWidth = m_pHeader->nWidth;
    *pnHeight = m_pHeader->nHeight;
    *pnDepth = m_pHeader->nBitsPerPixel;
    return 1;
}
// PC 0x007862D0
int CResTGA::OnResourceFreed()
{
    if (m_bDataOwned && m_pTGAData)
    {
        delete m_pTGAData;
    }

    m_bDataOwned = 0;
    m_bColorMapped = 0;
    m_bRLE = 0;
    m_bServiced = 0;
    m_nDataSize = 0;
    m_nRLESize = 0;
    m_pColorMap = 0;
    m_pTGAData = 0;
    m_pHeader = 0;
    return 1;
}
// PC 0x00786310
int CResTGA::ReadColorMappedRLETGA()
{
    unsigned char* pSource = m_pTGAData;
    m_pTGAData = new unsigned char[m_nDataSize + 10];
    if (m_pTGAData == 0)
    {
        return 0;
    }

    unsigned long nRead = 0;
    unsigned long nWritten = 0;
    while (nWritten < m_nDataSize)
    {
        unsigned long nEnd = (pSource[nRead] & 0x7F) + nWritten + 1;
        if (pSource[nRead++] & 0x80)
        {
            char cValue = pSource[nRead];
            nRead++;
            for (; nWritten < nEnd; nWritten++)
            {
                m_pTGAData[nWritten] = cValue;
            }
        }
        else
        {
            for (; nWritten < nEnd; nRead++)
            {
                m_pTGAData[nWritten] = pSource[nRead];
                nWritten++;
            }
        }
    }

    m_bDataOwned = 1;
    return 1;
}
// PC 0x00786550
int CResTGA::OnResourceServiced()
{
    if (m_pResource == 0)
    {
        return 0;
    }

    if (m_bServiced != 0)
    {
        return 1;
    }

    if (m_pHeader != 0)
    {
        return 0;
    }
    if (m_pTGAData != 0)
    {
        return 0;
    }
    if (m_pColorMap != 0)
    {
        return 0;
    }

    m_pHeader = (TGAHeader*) m_pResource;
    m_pColorMap = (unsigned char*) m_pResource + m_pHeader->nIDLength + 18;
    unsigned short nMapLength = m_pHeader->nColorMapLength;
    int bResult;
    switch (m_pHeader->nImageType)
    {
    case TGA_IMAGE_COLOR_MAPPED:
        m_nDataSize = m_nSize - nMapLength * (m_pHeader->nColorMapDepth >> 3) - m_pHeader->nIDLength - 18;
        m_pTGAData = m_pColorMap + nMapLength * (m_pHeader->nColorMapDepth >> 3);
        m_bColorMapped = 1;
        bResult = 1;
        break;
    case TGA_IMAGE_TRUE_COLOR:
    case TGA_IMAGE_GRAYSCALE:
        m_nDataSize = m_nSize - nMapLength * (m_pHeader->nColorMapDepth >> 3) - m_pHeader->nIDLength - 18;
        m_pTGAData = m_pColorMap + nMapLength * (m_pHeader->nColorMapDepth >> 3);
        bResult = 1;
        break;
    case TGA_IMAGE_RLE_COLOR_MAPPED:
        m_nRLESize = m_nSize - nMapLength * (m_pHeader->nColorMapDepth >> 3) - m_pHeader->nIDLength - 18;
        m_nDataSize = (m_pHeader->nBitsPerPixel >> 3) * m_pHeader->nHeight * m_pHeader->nWidth;
        m_pTGAData = m_pColorMap + nMapLength * (m_pHeader->nColorMapDepth >> 3);
        m_bColorMapped = 1;
        m_bRLE = 1;
        bResult = ReadColorMappedRLETGA();
        break;
    case TGA_IMAGE_RLE_TRUE_COLOR:
        m_nRLESize = m_nSize - nMapLength * (m_pHeader->nColorMapDepth >> 3) - m_pHeader->nIDLength - 18;
        m_nDataSize = (m_pHeader->nBitsPerPixel >> 3) * m_pHeader->nHeight * m_pHeader->nWidth;
        m_pTGAData = m_pColorMap + nMapLength * (m_pHeader->nColorMapDepth >> 3);
        m_bRLE = 1;
        bResult = ReadUnmappedRLETGA();
        break;
    default:
        m_pColorMap = 0;
        m_pHeader = 0;
        return 0;
    }

    if (bResult == 0)
    {
        OnResourceFreed();
        return 0;
    }
#ifndef _XBOX
    if (m_pHeader->nImageType != TGA_IMAGE_GRAYSCALE)
    {
        unsigned int nBytes = m_pHeader->nBitsPerPixel >> 3;
        unsigned long nPixels = m_pHeader->nWidth * m_pHeader->nHeight;
        unsigned char* pPixel = m_pTGAData;
        while (nPixels > 0)
        {
            unsigned char nTemp = pPixel[0];
            pPixel[0] = pPixel[2];
            pPixel[2] = nTemp;
            pPixel += nBytes;
            nPixels--;
        }
    }
#endif
    m_bServiced = 1;
    return 1;
}
// PC 0x007863A0
int CResTGA::ReadUnmappedRLETGA()
{
    unsigned char* pSource = m_pTGAData;
    m_pTGAData = new unsigned char[m_nDataSize + 10];
    if (m_pTGAData == 0)
    {
        return 0;
    }

    int nRead = 0;
    unsigned long nWritten = 0;
    char nDepth = m_pHeader->nBitsPerPixel;
    if (nDepth == 24)
    {
        while (nWritten < m_nDataSize)
        {
            unsigned long nEnd = nWritten + 3 * ((pSource[nRead] & 0x7F) + 1);
            char cRepeat = pSource[nRead] & 0x80;
            nRead++;
            if (cRepeat)
            {
                for (; nWritten < nEnd; nWritten++)
                {
                    m_pTGAData[nWritten] = pSource[nRead];
                    nWritten++;
                    m_pTGAData[nWritten] = pSource[nRead + 1];
                    nWritten++;
                    m_pTGAData[nWritten] = pSource[nRead + 2];
                }
                nRead += 3;
            }
            else
            {
                while (nWritten < nEnd)
                {
                    m_pTGAData[nWritten] = pSource[nRead];
                    nWritten++;
                    nRead++;
                    m_pTGAData[nWritten] = pSource[nRead];
                    nWritten++;
                    nRead++;
                    m_pTGAData[nWritten] = pSource[nRead];
                    nWritten++;
                    nRead++;
                }
            }
        }
    }
    else if (nDepth == 32)
    {
        while (nWritten < m_nDataSize)
        {
            unsigned long nEnd = nWritten + 4 * (pSource[nRead] & 0x7F) + 4;
            char cRepeat = pSource[nRead] & 0x80;
            nRead++;
            if (cRepeat)
            {
                for (; nWritten < nEnd; nWritten++)
                {
                    m_pTGAData[nWritten] = pSource[nRead];
                    nWritten++;
                    m_pTGAData[nWritten] = pSource[nRead + 1];
                    nWritten++;
                    m_pTGAData[nWritten] = pSource[nRead + 2];
                    nWritten++;
                    m_pTGAData[nWritten] = pSource[nRead + 3];
                }
                nRead += 4;
            }
            else
            {
                while (nWritten < nEnd)
                {
                    m_pTGAData[nWritten] = pSource[nRead];
                    nWritten++;
                    nRead++;
                    m_pTGAData[nWritten] = pSource[nRead];
                    nWritten++;
                    nRead++;
                    m_pTGAData[nWritten] = pSource[nRead];
                    nWritten++;
                    nRead++;
                    m_pTGAData[nWritten] = pSource[nRead];
                    nWritten++;
                    nRead++;
                }
            }
        }
    }
    else
    {
        return 0;
    }

    m_bDataOwned = 1;
    return 1;
}
// PC 0x00786840
unsigned char* CResTGA::GetTGADataPtr()
{
    return m_pTGAData;
}
// PC 0x00786860
unsigned long CResTGA::GetTGASize()
{
    return m_nDataSize;
}
