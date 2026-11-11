#include "exoresman.h"
// PC 0x00786740
CResTPC::CResTPC()
{
    m_bCompressed = 0;
    m_bServiced = 0;
    m_nUncompressedSize = 0;
    m_nCompressedSize = 0;
    m_nRemainingSize = 0;
    m_pData = 0;
    m_pHeader = 0;
    m_pEnd = 0;
    m_bAllocSuffix = 1;
}
// PC 0x00786780
CResTPC::~CResTPC()
{
}
// PC 0x00786790
int CResTPC::GetTPCAttrib(int* pnWidth, int* pnHeight, char* pnFormat,
                           int* pnCompressedSize, int* pnUncompressedSize,
                           int* pnRemainingSize, float* pfUnknown,
                           char* pnMipMapCount)
{
    int nResult = (int)m_pHeader;
    if (nResult == 0)
    {
        return nResult;
    }
    else
    {
        *pnWidth = m_pHeader->m_nWidth;
        *pnHeight = m_pHeader->m_nHeight;
        *pfUnknown = m_pHeader->m_fAlphaBlending;
        *pnMipMapCount = m_pHeader->m_nMipMapCount;
        *pnCompressedSize = m_nCompressedSize;
        *pnUncompressedSize = m_nUncompressedSize;
        *pnRemainingSize = m_nRemainingSize;
        if (m_pHeader->m_bFormat1)
        {
            *pnFormat = 1;
            return 1;
        }
        else
        {
            if (m_pHeader->m_bFormat3)
            {
                *pnFormat = 3;
                return 1;
            }
            else
            {
                if (m_pHeader->m_bFormat4)
                {
                    *pnFormat = 4;
                }
                return 1;
            }
        }
    }
}
// PC 0x00786830
unsigned char* CResTPC::GetTPCDataPtr()
{
    return m_pData;
}
// PC 0x00786840
unsigned char* CResTPC::GetTPCTXIPtr()
{
    return m_pEnd;
}
// PC 0x00786850
int CResTPC::GetTPCSize()
{
    if (m_bCompressed != 0)
    {
        return m_nCompressedSize;
    }
    else
    {
        return m_nUncompressedSize;
    }
}
// PC 0x00786860
int CResTPC::GetTPCTXISize()
{
    return m_nRemainingSize;
}
// PC 0x00786870
int CResTPC::OnResourceFreed()
{
    m_bCompressed = 0;
    m_bServiced = 0;
    m_nUncompressedSize = 0;
    m_nCompressedSize = 0;
    m_nRemainingSize = 0;
    m_pData = 0;
    m_pHeader = 0;
    m_pEnd = 0;
    return 1;
}
// PC 0x00786890
int CResTPC::OnResourceServiced()
{
    TPCHeader* pTPC;
    int nFormat;
    int nBlockSize;
    int nDepth;
    int nWidth;
    int nHeight;
    int nDataSize;
    int nMipMap;

    pTPC = (TPCHeader*)m_pResource;
    nDepth = 1;

    if (pTPC == 0)
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
    if (m_pData != 0)
    {
        return 0;
    }

    m_pHeader = pTPC;
    m_pData = (unsigned char*)pTPC + sizeof(TPCHeader);

    if (pTPC->m_bFormat1)
    {
        nFormat = 1;
    }
    else if (pTPC->m_bFormat3)
    {
        nFormat = 3;
    }
    else if (pTPC->m_bFormat4)
    {
        nFormat = 4;
    }

    if (pTPC->m_nDataSize == 0)
    {
#ifdef _XBOX
        nHeight = pTPC->m_nHeight;
        nWidth = pTPC->m_nWidth;
        if (nHeight / nWidth == 6)
        {
            nDepth = 6;
            nHeight /= 6;
        }
#else
        nWidth = pTPC->m_nWidth;
        nHeight = pTPC->m_nHeight;
#endif
        m_nUncompressedSize = 0;
        m_nCompressedSize = 0;
        for (nMipMap = 0; nMipMap < m_pHeader->m_nMipMapCount; nMipMap++)
        {
            m_nUncompressedSize += nHeight * nWidth * nFormat;
            nWidth >>= 1;
            nHeight >>= 1;
        }
#ifdef _XBOX
        if (nDepth == 6)
        {
            m_nUncompressedSize = (m_nUncompressedSize - (m_nUncompressedSize & 127) + 128) * 6;
        }
#endif
        nDataSize = m_nUncompressedSize;
        m_pEnd = m_pData + nDataSize;
        m_nRemainingSize = m_nSize - nDataSize - sizeof(TPCHeader);
    }
    else
    {
        m_bCompressed = 1;
        m_nUncompressedSize = 0;
        m_nCompressedSize = pTPC->m_nDataSize;
        if (nFormat == 3)
        {
            nBlockSize = 8;
        }
        else if (nFormat == 4)
        {
            nBlockSize = 16;
        }
        nHeight = pTPC->m_nHeight;
        nWidth = pTPC->m_nWidth;
        if (nHeight / nWidth == 6)
        {
            nDepth = 6;
            nHeight /= 6;
        }
#ifdef _XBOX
        nDataSize = pTPC->m_nDataSize;
        for (nMipMap = 0; nMipMap < m_pHeader->m_nMipMapCount - 1; nMipMap++)
        {
            nHeight >>= 1;
            nWidth >>= 1;
            nDataSize += ((nHeight + 3) / 4) * ((nWidth + 3) / 4) * nBlockSize;
        }
        if (nDepth == 6)
        {
            nDataSize = (nDataSize - nDataSize % 128 + 128) * 6;
        }
#else
        nDataSize = pTPC->m_nDataSize * nDepth;
        for (nMipMap = 0; nMipMap < m_pHeader->m_nMipMapCount - 1; nMipMap++)
        {
            nHeight >>= 1;
            nWidth >>= 1;
            nDataSize += ((nHeight + 3) / 4) * ((nWidth + 3) / 4) * nBlockSize * nDepth;
        }
#endif
        m_pEnd = m_pData + nDataSize;
        m_nRemainingSize = m_nSize - nDataSize - sizeof(TPCHeader);
    }

    m_bServiced = 1;
    return 1;
}
