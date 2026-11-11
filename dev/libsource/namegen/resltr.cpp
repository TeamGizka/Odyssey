#include <string.h>
#include "exoresman.h"
// PC 0x007121B0  Xbox 0x0027CC80
CResLTR::CResLTR()
{
    m_pLTRData = 0;
    m_pData = 0;
    m_nDataSize = 0;
    m_bServiced = 0;
    m_pppnLetterTable = 0;
    m_pppnLetterTable2 = 0;
    m_pppnLetterTable3 = 0;
    m_ppnLetterTable = 0;
    m_ppnLetterTable2 = 0;
    m_ppnLetterTable3 = 0;
    m_pnLetterTable = 0;
    m_pnLetterTable2 = 0;
    m_pnLetterTable3 = 0;
}
// PC 0x007121F0  Xbox 0x0027CCC0
CResLTR::~CResLTR()
{
}
// PC 0x00712200  Xbox 0x0027CCD0
int CResLTR::OnResourceFreed()
{
    unsigned char i;
    unsigned char j;

    m_bServiced = 0;
    if (m_pnLetterTable != 0)
    {
        delete m_pnLetterTable;
        m_pnLetterTable = 0;
    }
    if (m_pnLetterTable2 != 0)
    {
        delete m_pnLetterTable2;
        m_pnLetterTable2 = 0;
    }
    if (m_pnLetterTable3 != 0)
    {
        delete m_pnLetterTable3;
        m_pnLetterTable3 = 0;
    }
    for (i = 0; i < m_pLTRData->m_nLetterCount; ++i)
    {
        if (m_ppnLetterTable != 0 && m_ppnLetterTable[i] != 0)
        {
            delete m_ppnLetterTable[i];
        }
        if (m_ppnLetterTable2 != 0 && m_ppnLetterTable2[i] != 0)
        {
            delete m_ppnLetterTable2[i];
        }
        if (m_ppnLetterTable3 != 0 && m_ppnLetterTable3[i] != 0)
        {
            delete m_ppnLetterTable3[i];
        }
    }
    if (m_ppnLetterTable != 0)
    {
        delete m_ppnLetterTable;
        m_ppnLetterTable = 0;
    }
    if (m_ppnLetterTable2 != 0)
    {
        delete m_ppnLetterTable2;
        m_ppnLetterTable2 = 0;
    }
    if (m_ppnLetterTable3 != 0)
    {
        delete m_ppnLetterTable3;
        m_ppnLetterTable3 = 0;
    }
    for (i = 0; i < m_pLTRData->m_nLetterCount; ++i)
    {
        for (j = 0; j < m_pLTRData->m_nLetterCount; ++j)
        {
            if (m_pppnLetterTable[i][j] != 0)
            {
                delete m_pppnLetterTable[i][j];
            }
            if (m_pppnLetterTable2[i][j] != 0)
            {
                delete m_pppnLetterTable2[i][j];
            }
            if (m_pppnLetterTable3[i][j] != 0)
            {
                delete m_pppnLetterTable3[i][j];
            }
        }
        if (m_pppnLetterTable[i] != 0)
        {
            delete m_pppnLetterTable[i];
        }
        if (m_pppnLetterTable2[i] != 0)
        {
            delete m_pppnLetterTable2[i];
        }
        if (m_pppnLetterTable3[i] != 0)
        {
            delete m_pppnLetterTable3[i];
        }
    }
    if (m_pppnLetterTable != 0)
    {
        delete m_pppnLetterTable;
        m_pppnLetterTable = 0;
    }
    if (m_pppnLetterTable2 != 0)
    {
        delete m_pppnLetterTable2;
        m_pppnLetterTable2 = 0;
    }
    if (m_pppnLetterTable3 != 0)
    {
        delete m_pppnLetterTable3;
        m_pppnLetterTable3 = 0;
    }
    m_pLTRData = 0;
    m_pData = 0;
    m_nDataSize = 0;
    return 1;
}
// PC 0x00712410  Xbox 0x0027CEE0
int CResLTR::OnResourceServiced()
{
    LTRHeader* pHeader = (LTRHeader*)m_pResource;
    unsigned long nOffset;
    unsigned char* pData;
    unsigned char i;
    unsigned char j;
    if (pHeader == 0)
    {
        return 0;
    }
    if (m_bServiced != 0)
    {
        return 1;
    }
    m_nDataSize = m_nSize - 9;
    m_pLTRData = pHeader;
    if (pHeader->m_nFileType != 0x2052544C)
    {
        OnResourceFreed();
        return 0;
    }
    if (pHeader->m_nVersion != 0x302E3156)
    {
        OnResourceFreed();
        return 0;
    }

    m_pnLetterTable = new float[m_pLTRData->m_nLetterCount];
    m_pnLetterTable2 = new float[m_pLTRData->m_nLetterCount];
    m_pnLetterTable3 = new float[m_pLTRData->m_nLetterCount];
    if (m_pnLetterTable == 0)
    {
        return 0;
    }
    if (m_pnLetterTable2 == 0)
    {
        return 0;
    }
    if (m_pnLetterTable3 == 0)
    {
        return 0;
    }

    m_ppnLetterTable = new float*[m_pLTRData->m_nLetterCount];
    m_ppnLetterTable2 = new float*[m_pLTRData->m_nLetterCount];
    m_ppnLetterTable3 = new float*[m_pLTRData->m_nLetterCount];
    if (m_ppnLetterTable == 0 || m_ppnLetterTable2 == 0 ||
        m_ppnLetterTable3 == 0)
    {
        return 0;
    }
    {
        for (i = 0; i < m_pLTRData->m_nLetterCount; ++i)
        {
            m_ppnLetterTable[i] = new float[m_pLTRData->m_nLetterCount];
            m_ppnLetterTable2[i] =
                new float[m_pLTRData->m_nLetterCount];
            m_ppnLetterTable3[i] =
                new float[m_pLTRData->m_nLetterCount];
            if (m_ppnLetterTable[i] == 0 || m_ppnLetterTable2[i] == 0 ||
                m_ppnLetterTable3[i] == 0)
            {
                return 0;
            }
        }
    }

    m_pppnLetterTable = new float**[m_pLTRData->m_nLetterCount];
    m_pppnLetterTable2 = new float**[m_pLTRData->m_nLetterCount];
    m_pppnLetterTable3 = new float**[m_pLTRData->m_nLetterCount];
    if (m_pppnLetterTable == 0 || m_pppnLetterTable2 == 0 ||
        m_pppnLetterTable3 == 0)
    {
        return 0;
    }
    {
        for (i = 0; i < m_pLTRData->m_nLetterCount; ++i)
        {
            m_pppnLetterTable[i] =
                new float*[m_pLTRData->m_nLetterCount];
            m_pppnLetterTable2[i] =
                new float*[m_pLTRData->m_nLetterCount];
            m_pppnLetterTable3[i] =
                new float*[m_pLTRData->m_nLetterCount];
            if (m_pppnLetterTable[i] == 0 || m_pppnLetterTable2[i] == 0 ||
                m_pppnLetterTable3[i] == 0)
            {
                return 0;
            }
            for (j = 0; j < m_pLTRData->m_nLetterCount; ++j)
            {
                m_pppnLetterTable[i][j] =
                    new float[m_pLTRData->m_nLetterCount];
                m_pppnLetterTable2[i][j] =
                    new float[m_pLTRData->m_nLetterCount];
                m_pppnLetterTable3[i][j] =
                    new float[m_pLTRData->m_nLetterCount];
                if (m_pppnLetterTable[i][j] == 0 ||
                    m_pppnLetterTable2[i][j] == 0 ||
                    m_pppnLetterTable3[i][j] == 0)
                {
                    return 0;
                }
            }
        }
    }

    pData = (unsigned char*)((LTRHeader*)m_pResource + 1);
    m_pData = pData;
    memcpy(m_pnLetterTable, pData, 4 * m_pLTRData->m_nLetterCount);
    {
        unsigned long nTableSize = 4 * m_pLTRData->m_nLetterCount;
        memcpy(m_pnLetterTable2, &pData[nTableSize], nTableSize);
        nOffset = 4 * m_pLTRData->m_nLetterCount + nTableSize;
        memcpy(m_pnLetterTable3, &pData[nOffset],
               4 * m_pLTRData->m_nLetterCount);
    }
    nOffset += 4 * m_pLTRData->m_nLetterCount;
    {
        for (i = 0; i < m_pLTRData->m_nLetterCount; ++i)
        {
            memcpy(m_ppnLetterTable[i], &pData[nOffset],
                   4 * m_pLTRData->m_nLetterCount);
            nOffset += 4 * m_pLTRData->m_nLetterCount;
            memcpy(m_ppnLetterTable2[i], &pData[nOffset],
                   4 * m_pLTRData->m_nLetterCount);
            nOffset += 4 * m_pLTRData->m_nLetterCount;
            memcpy(m_ppnLetterTable3[i], &pData[nOffset],
                   4 * m_pLTRData->m_nLetterCount);
            nOffset += 4 * m_pLTRData->m_nLetterCount;
        }
    }
    {
        for (i = 0; i < m_pLTRData->m_nLetterCount; ++i)
        {
            for (j = 0; j < m_pLTRData->m_nLetterCount; ++j)
            {
                memcpy(m_pppnLetterTable[i][j], &pData[nOffset],
                       4 * m_pLTRData->m_nLetterCount);
                nOffset += 4 * m_pLTRData->m_nLetterCount;
                memcpy(m_pppnLetterTable2[i][j], &pData[nOffset],
                       4 * m_pLTRData->m_nLetterCount);
                nOffset += 4 * m_pLTRData->m_nLetterCount;
                memcpy(m_pppnLetterTable3[i][j], &pData[nOffset],
                       4 * m_pLTRData->m_nLetterCount);
                nOffset += 4 * m_pLTRData->m_nLetterCount;
            }
        }
    }
    if (nOffset > m_nDataSize)
    {
        return 0;
    }
    m_bServiced = 1;
    return 1;
}
