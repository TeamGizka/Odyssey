#include "resfnt.h"

CResFNT::CResFNT()
{
    m_bAllocSuffix = 1;
    m_bDataOwned = 0;
    m_bColorMapped = 0;
    m_bRLE = 0;
    m_bServiced = 0;
    m_nAllocSize = 0;
    m_nDataSize = 0;
    m_nRLESize = 0;
    m_pColorMap = 0;
    m_pFNTData = 0;
    m_pHeader = 0;
    m_pFontInfo = 0;
    m_pImageEnd = 0;
}

CResFNT::~CResFNT()
{
}

unsigned long* CResFNT::GetFNTDataPtr()
{
    return m_pFontInfo;
}

int CResFNT::GetTGAAttrib(unsigned long* pnWidth, unsigned long* pnHeight, unsigned char* pnDepth)
{
    if (m_pImageEnd == 0)
    {
        return 0;
    }

    *pnWidth = m_pHeader->nWidth;
    *pnHeight = m_pHeader->nHeight;
    *pnDepth = m_pHeader->nBitsPerPixel;
    return 1;
}

unsigned char* CResFNT::GetTGADataPtr()
{
    return m_pFNTData;
}

int CResFNT::ReadUnmappedRLETGA()
{
    unsigned char* pSource = m_pFNTData;
    m_pFNTData = new unsigned char[m_nDataSize];
    if (m_pFNTData == 0)
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
                for (; nWritten < nEnd; nWritten += 3)
                {
                    m_pFNTData[nWritten] = pSource[nRead];
                    m_pFNTData[nWritten + 1] = pSource[nRead + 1];
                    m_pFNTData[nWritten + 2] = pSource[nRead + 2];
                }
                nRead += 3;
            }
            else
            {
                for (; nWritten < nEnd; nWritten += 3)
                {
                    m_pFNTData[nWritten] = pSource[nRead];
                    m_pFNTData[nWritten + 1] = pSource[nRead + 1];
                    m_pFNTData[nWritten + 2] = pSource[nRead + 2];
                    nRead += 3;
                }
            }
        }
    }
    else if (nDepth == 32)
    {
        while (nWritten < m_nDataSize)
        {
            unsigned long nEnd = nWritten + 4 * ((pSource[nRead] & 0x7F) + 1);
            char cRepeat = pSource[nRead] & 0x80;
            nRead++;
            if (cRepeat)
            {
                for (; nWritten < nEnd; nWritten += 4)
                {
                    m_pFNTData[nWritten] = pSource[nRead];
                    m_pFNTData[nWritten + 1] = pSource[nRead + 1];
                    m_pFNTData[nWritten + 2] = pSource[nRead + 2];
                    m_pFNTData[nWritten + 3] = pSource[nRead + 3];
                }
                nRead += 4;
            }
            else
            {
                for (; nWritten < nEnd; nWritten += 4)
                {
                    m_pFNTData[nWritten] = pSource[nRead];
                    m_pFNTData[nWritten + 1] = pSource[nRead + 1];
                    m_pFNTData[nWritten + 2] = pSource[nRead + 2];
                    m_pFNTData[nWritten + 3] = pSource[nRead + 3];
                    nRead += 4;
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

int CResFNT::OnResourceFreed()
{
    if (m_bDataOwned && m_pFNTData)
    {
        delete m_pFNTData;
    }

    m_bDataOwned = 0;
    m_bColorMapped = 0;
    m_bRLE = 0;
    m_bServiced = 0;
    m_nAllocSize = 0;
    m_nDataSize = 0;
    m_nRLESize = 0;
    m_pColorMap = 0;
    m_pFNTData = 0;
    m_pHeader = 0;
    m_pFontInfo = 0;
    m_pImageEnd = 0;
    return 1;
}

int CResFNT::ReadColorMappedRLETGA()
{
    unsigned char* pSource = m_pFNTData;
    m_pFNTData = new unsigned char[m_nDataSize];
    if (m_pFNTData == 0)
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
                m_pFNTData[nWritten] = cValue;
            }
        }
        else
        {
            for (; nWritten < nEnd; nRead++)
            {
                m_pFNTData[nWritten] = pSource[nRead];
                nWritten++;
            }
        }
    }

    m_bDataOwned = 1;
    return 1;
}

int CResFNT::OnResourceServiced()
{
    if (m_pResource == 0)
    {
        return 0;
    }

    if (m_bServiced != 0)
    {
        return 1;
    }

    if (m_pImageEnd != 0)
    {
        return 0;
    }
    if (m_pHeader != 0)
    {
        return 0;
    }
    if (m_pFontInfo != 0)
    {
        return 0;
    }
    if (m_pFNTData != 0)
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
        m_nDataSize = (m_pHeader->nBitsPerPixel >> 3) * m_pHeader->nHeight * m_pHeader->nWidth;
        m_pFNTData = m_pColorMap + nMapLength * (m_pHeader->nColorMapDepth >> 3);
        m_pImageEnd = m_pFNTData + m_nDataSize;
        m_bColorMapped = 1;
        bResult = 1;
        break;
    case TGA_IMAGE_TRUE_COLOR:
        m_nDataSize = (m_pHeader->nBitsPerPixel >> 3) * m_pHeader->nHeight * m_pHeader->nWidth;
        m_pFNTData = m_pColorMap + nMapLength * (m_pHeader->nColorMapDepth >> 3);
        m_pImageEnd = m_pFNTData + m_nDataSize;
        bResult = 1;
        break;
    case TGA_IMAGE_RLE_COLOR_MAPPED:
        m_nRLESize = m_nSize - nMapLength * (m_pHeader->nColorMapDepth >> 3) - m_pHeader->nIDLength - 18;
        m_nDataSize = (m_pHeader->nBitsPerPixel >> 3) * m_pHeader->nHeight * m_pHeader->nWidth;
        m_pFNTData = m_pColorMap + nMapLength * (m_pHeader->nColorMapDepth >> 3);
        m_pImageEnd = m_pFNTData + m_nRLESize;
        m_bColorMapped = 1;
        m_bRLE = 1;
        bResult = ReadColorMappedRLETGA();
        break;
    case TGA_IMAGE_RLE_TRUE_COLOR:
        m_nRLESize = m_nSize - nMapLength * (m_pHeader->nColorMapDepth >> 3) - m_pHeader->nIDLength - 18;
        m_nDataSize = (m_pHeader->nBitsPerPixel >> 3) * m_pHeader->nHeight * m_pHeader->nWidth;
        m_pFNTData = m_pColorMap + nMapLength * (m_pHeader->nColorMapDepth >> 3);
        m_pImageEnd = m_pFNTData + m_nRLESize;
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

    m_pFontInfo = (unsigned long*)(m_pImageEnd + FNT_FONT_INFO_OFFSET);
    m_bServiced = 1;
    return 1;
}
