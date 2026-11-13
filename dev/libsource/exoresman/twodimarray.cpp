#include "twodimarray.h"
#include "exoresman.h"

#include <stdio.h>
#include <string.h>
// PC 0x0041D140 GOG 0x0041D130
int C2DA::GetColumnIndex(const CExoString& sColumnLabel)
{
    int i;

    if (m_bPacked)
    {
        for (i = 0; i < m_nNumColumns; i++)
        {
            if (stricmp(&m_pLabelData[m_pColumnLabelOffsets[i]],
                         sColumnLabel.CStr()) == 0)
            {
                return i;
            }
        }
    }
    else
    {
        for (i = 0; i < m_nNumColumns; i++)
        {
            if (m_pColumnLabels[i].CompareNoCase(sColumnLabel))
            {
                return i;
            }
        }
    }

    return -1;
}
// PC 0x0041DDE0 GOG 0x0041DDD0
int C2DA::GetCExoStringEntry(int nRow, int nColumn, CExoString* psEntry)
{
    if (nRow < 0 || nRow >= m_nNumRows || nColumn < 0 ||
        nColumn >= m_nNumColumns)
    {
        *psEntry = m_sDefault;
        return 0;
    }

    if (m_bPacked)
    {
        int nOffset = m_pEntryOffsets[nColumn + nRow * m_nNumColumns];

        return (*psEntry = m_pEntryData + nOffset)[0] != 0;
    }

    if (m_ppEntries[nRow][nColumn] == "")
    {
        *psEntry = "";
        return 0;
    }

    *psEntry = m_ppEntries[nRow][nColumn];
    return psEntry->GetLength() > 0;
}
// PC 0x0041D1F0 GOG 0x0041D1E0
int C2DA::GetCExoStringEntry(int nRow, const CExoString& sColumn,
                             CExoString* psEntry)
{
    int nColumn = GetColumnIndex(sColumn);

    if (nRow < 0 || nRow >= m_nNumRows || nColumn == -1)
    {
        *psEntry = m_sDefault;
        return 0;
    }

    if (m_bPacked)
    {
        int nOffset = m_pEntryOffsets[nColumn + nRow * m_nNumColumns];

        return (*psEntry = m_pEntryData + nOffset)[0] != 0;
    }

    if (m_ppEntries[nRow][nColumn] == "")
    {
        *psEntry = "";
        return 0;
    }

    *psEntry = m_ppEntries[nRow][nColumn];
    return psEntry->GetLength() > 0;
}
// PC 0x0041D2D0 GOG 0x0041D2C0
int C2DA::GetFLOATEntry(int nRow, int nColumn, float* pfEntry)
{
    if (nRow < 0 || nRow >= m_nNumRows || nColumn < 0 ||
        nColumn >= m_nNumColumns)
    {
        *pfEntry = m_sDefault.AsFLOAT();
        return 0;
    }

    if (m_bPacked)
    {
        int nOffset = m_pEntryOffsets[nColumn + nRow * m_nNumColumns];

        *pfEntry = 0.0f;
        sscanf(m_pEntryData + nOffset, "%f", pfEntry);
        return m_pEntryData[nOffset] != 0;
    }

    if (m_ppEntries[nRow][nColumn] == "")
    {
        *pfEntry = 0.0f;
        return 0;
    }

    *pfEntry = m_ppEntries[nRow][nColumn].AsFLOAT();
    return 1;
}
// PC 0x0041D3D0 GOG 0x0041D3C0
int C2DA::GetFLOATEntry(int nRow, const CExoString& sColumn, float* pfEntry)
{
    int nColumn = GetColumnIndex(sColumn);

    if (nRow < 0 || nRow >= m_nNumRows || nColumn == -1)
    {
        *pfEntry = m_sDefault.AsFLOAT();
        return 0;
    }

    if (m_bPacked)
    {
        int nOffset = m_pEntryOffsets[nColumn + nRow * m_nNumColumns];

        *pfEntry = 0.0f;
        sscanf(m_pEntryData + nOffset, "%f", pfEntry);
        return m_pEntryData[nOffset] != 0;
    }

    if (m_ppEntries[nRow][nColumn] == "")
    {
        *pfEntry = 0.0f;
        return 0;
    }

    *pfEntry = m_ppEntries[nRow][nColumn].AsFLOAT();
    return 1;
}
// PC 0x0041D4D0 GOG 0x0041D4C0
int C2DA::GetINTEntry(int nRow, int nColumn, int* pnEntry)
{
    if (nRow < 0 || nRow >= m_nNumRows || nColumn < 0 ||
        nColumn >= m_nNumColumns)
    {
        *pnEntry = m_sDefault.AsINT();
        return 0;
    }

    if (m_bPacked)
    {
        int nOffset = m_pEntryOffsets[nColumn + nRow * m_nNumColumns];

        *pnEntry = 0;
        sscanf(m_pEntryData + nOffset, "%i", pnEntry);
        return m_pEntryData[nOffset] != 0;
    }

    if (m_ppEntries[nRow][nColumn] == "")
    {
        *pnEntry = 0;
        return 0;
    }

    if (m_ppEntries[nRow][nColumn].GetLength() > 2 &&
        m_ppEntries[nRow][nColumn].CStr()[0] == '0' &&
        (m_ppEntries[nRow][nColumn].CStr()[1] == 'x' ||
         m_ppEntries[nRow][nColumn].CStr()[1] == 'X'))
    {
        sscanf(m_ppEntries[nRow][nColumn].CStr(), "%x", pnEntry);
        return 1;
    }

    *pnEntry = m_ppEntries[nRow][nColumn].AsINT();
    return 1;
}
// PC 0x0041D640 GOG 0x0041D630
int C2DA::GetINTEntry(int nRow, const CExoString& sColumn, int* pnEntry)
{
    int nColumn = GetColumnIndex(sColumn);

    if (nRow < 0 || nRow >= m_nNumRows || nColumn == -1)
    {
        *pnEntry = m_sDefault.AsINT();
        return 0;
    }

    if (m_bPacked)
    {
        int nOffset = m_pEntryOffsets[nColumn + nRow * m_nNumColumns];

        *pnEntry = 0;
        sscanf(m_pEntryData + nOffset, "%i", pnEntry);
        return m_pEntryData[nOffset] != 0;
    }

    if (m_ppEntries[nRow][nColumn] == "")
    {
        *pnEntry = 0;
        return 0;
    }

    if (m_ppEntries[nRow][nColumn].GetLength() > 2 &&
        m_ppEntries[nRow][nColumn].CStr()[0] == '0' &&
        (m_ppEntries[nRow][nColumn].CStr()[1] == 'x' ||
         m_ppEntries[nRow][nColumn].CStr()[1] == 'X'))
    {
        sscanf(m_ppEntries[nRow][nColumn].CStr(), "%x", pnEntry);
        return 1;
    }

    *pnEntry = m_ppEntries[nRow][nColumn].AsINT();
    return 1;
}
// PC 0x0041D7B0 GOG 0x0041D7A0
unsigned long C2DA::GetNextLineLength(char* pData, unsigned long nSize)
{
    unsigned long nLength;

    for (nLength = 0; nLength < nSize; nLength++)
    {
        if (*pData == '\n' || *pData == '\r')
        {
            break;
        }

        pData++;
    }

    return nLength;
}
// PC 0x0041D7E0 GOG 0x0041D7D0
int C2DA::GetNextToken(char** ppData, unsigned long* pnSize, char* szToken,
                       unsigned long* pnSkipped)
{
    if (*pnSize == 0)
    {
        *szToken = 0;
        return 0;
    }

    char* pData = *ppData;
    unsigned long nCount = 0;
    unsigned long nSkipped = 0;

    if (*pData == 0)
    {
        pData++;
    }

    while (nSkipped < *pnSize && *pData == ' ' || *pData == '\t')
    {
        pData++;
        nSkipped++;
    }

    *pnSkipped = nSkipped;
    *pnSize -= nSkipped;

    if (*pData == '"')
    {
        pData++;

        while (nCount < *pnSize)
        {
            if (*pData == 0 || *pData == '"')
            {
                break;
            }

            *szToken++ = *pData++;
            nCount++;
        }

        if (*pData == '"')
        {
            pData++;
            nCount++;
        }
    }
    else
    {
        while (nCount < *pnSize)
        {
            if (*pData == 0 || *pData == ' ' || *pData == '\t' ||
                *pData == '\n' || *pData == '\r')
            {
                break;
            }

            *szToken++ = *pData++;
            nCount++;
        }
    }

    *ppData = pData;
    *pnSize -= nCount;
    *szToken = 0;

    return nCount > 0;
}
// PC 0x0041DCC0 GOG 0x0041DCB0
C2DA::C2DA(CResRef cResRef, int bRequest)
{
    m_ppEntries = NULL;
    m_pRowLabels = NULL;
    m_pColumnLabels = NULL;
    m_nNumRows = 0;
    m_nNumColumns = 0;
    m_sDefault = "";
    m_bLoaded = 0;
    m_bPacked = 0;
    m_pEntryData = NULL;
    m_pEntryOffsets = NULL;
    m_pRowLabelOffsets = NULL;
    m_pColumnLabelOffsets = NULL;
    m_pLabelData = NULL;
    SetResRef(cResRef, bRequest);
}
// PC 0x0041DD80 GOG 0x0041DD70
C2DA::~C2DA()
{
    Unload2DArray();
}
// PC 0x0041DEC0 GOG 0x0041DEB0
int C2DA::GetCExoStringEntry(const CExoString& sRow, const CExoString& sColumn,
                             CExoString* psEntry)
{
    int nRow = GetRowIndex(sRow);
    int nColumn = GetColumnIndex(sColumn);

    if (nRow == -1 || nColumn == -1)
    {
        *psEntry = m_sDefault;
        return 0;
    }

    if (m_bPacked)
    {
        int nOffset = m_pEntryOffsets[nColumn + nRow * m_nNumColumns];

        return (*psEntry = m_pEntryData + nOffset)[0] != 0;
    }

    if (m_ppEntries[nRow][nColumn] == "")
    {
        *psEntry = "";
        return 0;
    }

    *psEntry = m_ppEntries[nRow][nColumn];
    return psEntry->GetLength() > 0;
}
// PC 0x0041DFA0 GOG 0x0041DF90
int C2DA::GetCExoStringEntry(const CExoString& sRow, int nColumn,
                             CExoString* psEntry)
{
    int nRow = GetRowIndex(sRow);

    if (nRow == -1 || nColumn < 0 || nColumn >= m_nNumColumns)
    {
        *psEntry = m_sDefault;
        return 0;
    }

    if (m_bPacked)
    {
        int nOffset = m_pEntryOffsets[nColumn + nRow * m_nNumColumns];

        return (*psEntry = m_pEntryData + nOffset)[0] != 0;
    }

    if (m_ppEntries[nRow][nColumn] == "")
    {
        *psEntry = "";
        return 0;
    }

    *psEntry = m_ppEntries[nRow][nColumn];
    return psEntry->GetLength() > 0;
}
// PC 0x0041E080 GOG 0x0041E070
int C2DA::GetFLOATEntry(const CExoString& sRow, const CExoString& sColumn,
                        float* pfEntry)
{
    int nRow = GetRowIndex(sRow);
    int nColumn = GetColumnIndex(sColumn);

    if (nRow == -1 || nColumn == -1)
    {
        *pfEntry = m_sDefault.AsFLOAT();
        return 0;
    }

    if (m_bPacked)
    {
        int nOffset = m_pEntryOffsets[nColumn + nRow * m_nNumColumns];

        *pfEntry = 0.0f;
        sscanf(m_pEntryData + nOffset, "%f", pfEntry);
        return m_pEntryData[nOffset] != 0;
    }

    if (m_ppEntries[nRow][nColumn] == "")
    {
        *pfEntry = 0.0f;
        return 0;
    }

    *pfEntry = m_ppEntries[nRow][nColumn].AsFLOAT();
    return 1;
}
// PC 0x0041E1A0 GOG 0x0041E190
int C2DA::GetFLOATEntry(const CExoString& sRow, int nColumn, float* pfEntry)
{
    int nRow = GetRowIndex(sRow);

    if (nRow == -1 || nColumn < 0 || nColumn >= m_nNumColumns)
    {
        *pfEntry = m_sDefault.AsFLOAT();
        return 0;
    }

    if (m_bPacked)
    {
        int nOffset = m_pEntryOffsets[nColumn + nRow * m_nNumColumns];

        *pfEntry = 0.0f;
        sscanf(m_pEntryData + nOffset, "%f", pfEntry);
    }
    else
    {
        *pfEntry = m_ppEntries[nRow][nColumn].AsFLOAT();
    }

    return 1;
}
// PC 0x0041E270 GOG 0x0041E260
int C2DA::GetINTEntry(const CExoString& sRow, const CExoString& sColumn,
                      int* pnEntry)
{
    int nRow = GetRowIndex(sRow);
    int nColumn = GetColumnIndex(sColumn);

    if (nRow == -1 || nColumn == -1)
    {
        *pnEntry = m_sDefault.AsINT();
        return 0;
    }

    if (m_bPacked)
    {
        int nOffset = m_pEntryOffsets[nColumn + nRow * m_nNumColumns];

        *pnEntry = 0;
        sscanf(m_pEntryData + nOffset, "%i", pnEntry);
        return m_pEntryData[nOffset] != 0;
    }

    if (m_ppEntries[nRow][nColumn] == "")
    {
        *pnEntry = 0;
        return 0;
    }

    if (m_ppEntries[nRow][nColumn].GetLength() > 2 &&
        m_ppEntries[nRow][nColumn].CStr()[0] == '0' &&
        (m_ppEntries[nRow][nColumn].CStr()[1] == 'x' ||
         m_ppEntries[nRow][nColumn].CStr()[1] == 'X'))
    {
        sscanf(m_ppEntries[nRow][nColumn].CStr(), "%x", pnEntry);
        return 1;
    }

    *pnEntry = m_ppEntries[nRow][nColumn].AsINT();
    return 1;
}
// PC 0x0041E400 GOG 0x0041E3F0
int C2DA::GetINTEntry(const CExoString& sRow, int nColumn, int* pnEntry)
{
    int nRow = GetRowIndex(sRow);

    if (nRow == -1 || nColumn < 0 || nColumn >= m_nNumColumns)
    {
        *pnEntry = m_sDefault.AsINT();
        return 0;
    }

    if (m_bPacked)
    {
        int nOffset = m_pEntryOffsets[nColumn + nRow * m_nNumColumns];

        *pnEntry = 0;
        sscanf(m_pEntryData + nOffset, "%i", pnEntry);
        return m_pEntryData[nOffset] != 0;
    }

    if (m_ppEntries[nRow][nColumn] == "")
    {
        *pnEntry = 0;
        return 0;
    }

    if (m_ppEntries[nRow][nColumn].GetLength() > 2 &&
        m_ppEntries[nRow][nColumn].CStr()[0] == '0' &&
        (m_ppEntries[nRow][nColumn].CStr()[1] == 'x' ||
         m_ppEntries[nRow][nColumn].CStr()[1] == 'X'))
    {
        sscanf(m_ppEntries[nRow][nColumn].CStr(), "%x", pnEntry);
        return 1;
    }

    *pnEntry = m_ppEntries[nRow][nColumn].AsINT();
    return 1;
}
// PC 0x0041E570 GOG 0x0041E560
int C2DA::Load2DArray()
{
    unsigned long nSize;
    char* pData;
    char* pLine;
    unsigned long nLine;
    unsigned long nSkipped;
    unsigned long nOffset;
    char* pRow;
    unsigned long nRowSize;
    int nColumn;
    char szToken[256];
    int i;

    Unload2DArray();

    if (!m_pRes->Demand())
    {
        return 0;
    }

    pData = (char*)m_pRes->Get2DADataPtr();
    nSize = m_pRes->Get2DASize();
    int bPacked = m_pRes->m_bPacked;
    m_bPacked = bPacked;

    SkipNewLines(&pData, &nSize);

    pLine = pData;

    nRowSize = GetNextLineLength(pData, nSize);
    nLine = nRowSize;

    GetNextToken(&pLine, &nLine, szToken, &nSkipped);
    strupr(szToken);

    if (strcmp(szToken, "DEFAULT:") == 0)
    {
        GetNextToken(&pLine, &nLine, szToken, &nSkipped);
        m_sDefault = szToken;
        pData = pLine;
        nSize -= nRowSize;
    }
    else if (strcmp(szToken, "DEFAULT") == 0)
    {
        GetNextToken(&pLine, &nLine, szToken, &nSkipped);

        if (szToken[0] == ':')
        {
            if (strlen(szToken + 1) < 1)
            {
                GetNextToken(&pLine, &nLine, szToken, &nSkipped);
                m_sDefault = szToken;
            }
            else
            {
                m_sDefault = szToken + 1;
            }
        }
        else
        {
            m_sDefault = szToken;
        }

        nSize -= nRowSize;
    }

    SkipNewLines(&pData, &nSize);

    m_nNumColumns = 0;
    pLine = pData;
    nLine = GetNextLineLength(pData, nSize);

    while (GetNextToken(&pLine, &nLine, szToken, &nSkipped))
    {
        m_nNumColumns++;
    }

    if (m_nNumColumns <= 0)
    {
        m_bLoaded = 1;
        Unload2DArray();
        return 0;
    }

    if (m_bPacked)
    {
        m_pLabelData = pData;
        m_pColumnLabelOffsets = new unsigned short[m_nNumColumns];

        nOffset = 0;

        for (i = 0; i < m_nNumColumns; i++)
        {
            m_pColumnLabelOffsets[i] = (unsigned short)nOffset;

            while (pData[nOffset] != '\t' && pData[nOffset] != 0)
            {
                nOffset++;
            }

            pData[nOffset++] = 0;
        }

        nOffset++;
        m_nNumRows = *(int*)(pData + nOffset);
        nOffset += 4;
        m_pRowLabelOffsets = new unsigned short[m_nNumRows];

        for (i = 0; i < m_nNumRows; i++)
        {
            m_pRowLabelOffsets[i] = (unsigned short)nOffset;

            while (pData[nOffset] != '\t' && pData[nOffset] != 0)
            {
                nOffset++;
            }

            pData[nOffset++] = 0;
        }

        pData += nOffset;
        m_pEntryOffsets = (unsigned short*)pData;
        pData += (m_nNumRows * m_nNumColumns + 1) * sizeof(unsigned short);
        m_pEntryData = pData;
    }
    else
    {
        m_pColumnLabels = new CExoString[m_nNumColumns];

        if (m_pColumnLabels == NULL)
        {
            m_bLoaded = 1;
            Unload2DArray();
            return 0;
        }

        i = 0;

        while (GetNextToken(&pData, &nSize, szToken, &nSkipped))
        {
            m_pColumnLabels[i] = strlwr(szToken);
            i++;
        }

        SkipNewLines(&pData, &nSize);

        m_nNumRows = 0;
        pRow = pData;
        nRowSize = nSize;

        if (nSize != 0)
        {
            do
            {
                nLine = GetNextLineLength(pData, nSize);
                pData += nLine;
                nSize -= nLine;
                SkipNewLines(&pData, &nSize);
                m_nNumRows++;
            } while (nSize != 0);
        }

        pData = pRow;
        nSize = nRowSize;

        if (m_nNumRows == 0)
        {
            m_bLoaded = 1;
            m_pRes->Release();
            return 1;
        }

        m_pRowLabels = new CExoString[m_nNumRows];

        if (m_pRowLabels == NULL)
        {
            m_bLoaded = 1;
            Unload2DArray();
            return 0;
        }

        m_ppEntries = new CExoString*[m_nNumRows];

        if (m_pRowLabels == NULL)
        {
            m_bLoaded = 1;
            Unload2DArray();
            return 0;
        }

        for (i = 0; i < m_nNumRows; i++)
        {
            m_ppEntries[i] = new CExoString[m_nNumColumns];

            if (m_ppEntries[i] == NULL)
            {
                m_bLoaded = 1;
                Unload2DArray();
                return 0;
            }
        }

        if (nSize != 0)
        {
            i = 0;

            do
            {
                nLine = GetNextLineLength(pData, nSize);
                pRow = pData;
                nRowSize = nSize;
                GetNextToken(&pRow, &nRowSize, szToken, &nSkipped);
                m_pRowLabels[i] = strlwr(szToken);

                for (nColumn = 0; nColumn < m_nNumColumns;)
                {
                    if (GetNextToken(&pRow, &nRowSize, szToken, &nSkipped))
                    {
                        if (strcmp(szToken, "****") == 0)
                        {
                            m_ppEntries[i][nColumn++] = "";
                        }
                        else
                        {
                            m_ppEntries[i][nColumn++] = szToken;
                        }
                    }
                    else
                    {
                        m_ppEntries[i][nColumn++] = m_sDefault;
                    }
                }

                pData += nLine;
                nSize -= nLine;
                SkipNewLines(&pData, &nSize);
                i++;
            } while (nSize != 0);
        }

        m_pRes->Release();
    }

    m_bLoaded = 1;
    return 1;
}
// PC 0x0041D8B0 GOG 0x0041D8A0
int C2DA::GetRowIndex(const CExoString& sRowLabel)
{
    int i;

    if (m_bPacked)
    {
        for (i = 0; i < m_nNumRows; i++)
        {
            if (stricmp(&m_pLabelData[m_pRowLabelOffsets[i]],
                         sRowLabel.CStr()) == 0)
            {
                return i;
            }
        }
    }
    else
    {
        for (i = 0; i < m_nNumRows; i++)
        {
            if (m_pRowLabels[i].CompareNoCase(sRowLabel))
            {
                return i;
            }
        }
    }

    return -1;
}
// PC 0x0041D940 GOG 0x0041D930
int C2DA::SkipNewLines(char** ppData, unsigned long* pnSize)
{
    unsigned long nSize = *pnSize;
    unsigned long nSkipped = 0;
    unsigned long nCount = 0;
    char* pData = *ppData;

    while (nSkipped < *pnSize)
    {
        if (*pData != '\n' && *pData != '\r' && *pData != ' ' && *pData != '\t')
        {
            break;
        }

        nSkipped = nCount;

        while (nSkipped < *pnSize && (*pData == '\n' || *pData == '\r'))
        {
            pData++;
            nSkipped++;
        }

        nCount = nSkipped;

        while (nCount < *pnSize && (*pData == ' ' || *pData == '\t'))
        {
            pData++;
            nCount++;
        }

        if (nCount == nSize)
        {
            nSkipped = nCount;
            break;
        }
    }

    *ppData += nSkipped;
    *pnSize -= nSkipped;

    return nSkipped > 0;
}

static unsigned long GetLineLength(char* pData, unsigned long nSize)
{
    unsigned long nLength;

    for (nLength = 0; nLength < nSize; nLength++)
    {
        if (*pData == '\n' || *pData == '\r')
        {
            break;
        }

        pData++;
    }

    return nLength;
}
// PC 0x0041D9E0 GOG 0x0041D9D0
void C2DA::Unload2DArray()
{
    int i;

    if (m_bLoaded)
    {
        if (m_ppEntries)
        {
            for (i = 0; i < m_nNumRows; i++)
            {
                delete[] m_ppEntries[i];
            }

            delete[] m_ppEntries;
            m_ppEntries = NULL;
        }

        if (m_pColumnLabels)
        {
            delete[] m_pColumnLabels;
            m_pColumnLabels = NULL;
        }

        if (m_pRowLabels)
        {
            delete[] m_pRowLabels;
            m_pRowLabels = NULL;
        }

        m_nNumRows = 0;
        m_nNumColumns = 0;
        m_sDefault = "";
        m_pRes->Release();
        m_bLoaded = 0;
        m_bPacked = 0;
        m_pEntryData = NULL;
        m_pEntryOffsets = NULL;
        delete[] m_pRowLabelOffsets;
        m_pRowLabelOffsets = NULL;
        delete[] m_pColumnLabelOffsets;
        m_pColumnLabelOffsets = NULL;
        m_pLabelData = NULL;
    }
}

int C2DA::SaveBinary2da(char* sFileName)
{
    int nRow;
    int nColumn;
    int nEntry;
    int nNumEntries;
    int nNumStrings;
    int* pStringIndices;
    int* pStringOffsets;
    int nDataSize;
    CExoString* pStrings;
    FILE* pFile;

    if (!m_bLoaded)
    {
        return 0;
    }

    nNumEntries = m_nNumRows * m_nNumColumns;
    pStrings = new CExoString[nNumEntries];
    nNumStrings = 0;
    pStringIndices = new int[m_nNumRows * m_nNumColumns];
    pStringOffsets = new int[m_nNumRows * m_nNumColumns];

    for (nRow = 0; nRow < m_nNumRows; nRow++)
    {
        for (nColumn = 0; nColumn < m_nNumColumns; nColumn++)
        {
            for (nEntry = 0; nEntry < nNumStrings; nEntry++)
            {
                if (pStrings[nEntry] == m_ppEntries[nRow][nColumn])
                {
                    break;
                }
            }

            if (nEntry == nNumStrings)
            {
                pStrings[nNumStrings++] = m_ppEntries[nRow][nColumn];
            }

            pStringIndices[nRow * m_nNumColumns + nColumn] = nEntry;
        }
    }

    nDataSize = 0;
    for (nEntry = 0; nEntry < nNumStrings; nEntry++)
    {
        pStringOffsets[nEntry] = nDataSize;
        nDataSize += strlen(pStrings[nEntry].CStr()) + 1;
    }

    if (nDataSize >= 0x10000)
    {
        delete[] pStrings;
        delete[] pStringIndices;
        delete[] pStringOffsets;
        return 0;
    }

    pFile = fopen(sFileName, "wb");
    if (!pFile)
    {
        delete[] pStrings;
        delete[] pStringIndices;
        delete[] pStringOffsets;
        return 0;
    }

    fprintf(pFile, "2DA ");
    fprintf(pFile, "V2.b");
    fprintf(pFile, "\n");
    if (m_sDefault[0])
    {
        fprintf(pFile, "DEFAULT: %s\n", m_sDefault.CStr());
    }

    for (nColumn = 0; nColumn < m_nNumColumns; nColumn++)
    {
        fprintf(pFile, "%s\t", m_pColumnLabels[nColumn].CStr());
    }
    fprintf(pFile, "%c", 0);

    fwrite(&m_nNumRows, 4, 1, pFile);
    for (nRow = 0; nRow < m_nNumRows; nRow++)
    {
        fprintf(pFile, "%s\t", m_pRowLabels[nRow].CStr());
    }

    for (nRow = 0; nRow < m_nNumRows; nRow++)
    {
        for (nColumn = 0; nColumn < m_nNumColumns; nColumn++)
        {
            fwrite(&pStringOffsets[pStringIndices[nRow * m_nNumColumns + nColumn]],
                   2, 1, pFile);
        }
    }
    fwrite(&nDataSize, 2, 1, pFile);

    for (nEntry = 0; nEntry < nNumStrings; nEntry++)
    {
        if (pStrings[nEntry][0])
        {
            fprintf(pFile, "%s", pStrings[nEntry].CStr());
        }
        fprintf(pFile, "%c", 0);
    }

    fclose(pFile);
    delete[] pStrings;
    delete[] pStringIndices;
    delete[] pStringOffsets;
    return 1;
}
