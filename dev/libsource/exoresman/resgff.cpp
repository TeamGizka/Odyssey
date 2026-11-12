#include "resgff.h"
#include "math3d.h"
#include <string.h>

char* CResGFF::VERSION = "V3.2";

const int CResGFF::STRUCT_STARTSIZE = 110;
const int CResGFF::FIELD_STARTSIZE = 646;
const int CResGFF::LABEL_STARTSIZE = 98;
const int CResGFF::DATAFIELD_STARTSIZE = 1836;
const int CResGFF::DATALAYOUTFIELD_STARTSIZE = 4052;
const int CResGFF::DATALAYOUTLIST_STARTSIZE = 4052;
// PC 0x00413F20
void CResGFF::Pack(unsigned char nFlag, unsigned long nSize)
{
    if (m_nFieldIndicesLost == 0 && m_nListIndicesLost == 0)
    {
        return;
    }

    if (m_pHeader == NULL || m_pFieldIndices == NULL ||
        m_pListIndices == NULL || m_pStructs == NULL || m_pFields == NULL)
    {
        return;
    }

    if (m_nFieldIndicesLost + m_nListIndicesLost <= nSize)
    {
        return;
    }

    if (100 * (m_nFieldIndicesLost + m_nListIndicesLost) /
            (unsigned long)GetTotalSize() <= nFlag)
    {
        return;
    }

    if (m_nFieldIndicesLost > 0)
    {
        unsigned long nNewSize = m_pHeader->m_nFieldIndicesCount -
                                 m_nFieldIndicesLost;
        if (nNewSize != 0)
        {
            unsigned char* pNew = new unsigned char[nNewSize];
            unsigned long nNewOffset = 0;
            for (unsigned long i = 0; i < m_pHeader->m_nStructCount; i++)
            {
                if (m_pStructs[i].m_nFieldCount > 1)
                {
                    unsigned long nOffset = m_pStructs[i].m_nDataOrDataOffset;
                    if (nOffset < m_pHeader->m_nFieldIndicesCount &&
                        m_pHeader->m_nFieldIndicesCount - nOffset >=
                            4 * m_pStructs[i].m_nFieldCount)
                    {
                        memcpy(pNew + nNewOffset, m_pFieldIndices + nOffset,
                               4 * m_pStructs[i].m_nFieldCount);
                        m_pStructs[i].m_nDataOrDataOffset = nNewOffset;
                        nNewOffset += 4 * m_pStructs[i].m_nFieldCount;
                    }
                    else
                    {
                        m_pStructs[i].m_nDataOrDataOffset = -1;
                        m_pStructs[i].m_nFieldCount = 0;
                    }
                }
            }
            delete m_pFieldIndices;
            m_pFieldIndices = pNew;
            m_pHeader->m_nFieldIndicesCount = nNewOffset;
        }
        else
        {
            m_pHeader->m_nFieldIndicesCount = 0;
        }
        m_nFieldIndicesLost = 0;
    }

    if (m_nListIndicesLost > 0)
    {
        unsigned long nNewSize = m_pHeader->m_nListIndicesCount -
                                 m_nListIndicesLost;
        if (nNewSize != 0)
        {
            unsigned char* pNew = new unsigned char[nNewSize];
            unsigned long nNewOffset = 0;
            for (unsigned long i = 0; i < m_pHeader->m_nFieldCount; i++)
            {
                if (m_pFields[i].m_nType == 15)
                {
                    unsigned long nOffset = m_pFields[i].m_nDataOrDataOffset;
                    if (nOffset < m_pHeader->m_nListIndicesCount &&
                        m_pHeader->m_nListIndicesCount - nOffset >= 4)
                    {
                        unsigned long nListSize =
                            4 * *(unsigned long*)(m_pListIndices + nOffset);
                        if (m_pHeader->m_nListIndicesCount - nOffset >=
                            nListSize + 4)
                        {
                            memcpy(pNew + nNewOffset, m_pListIndices + nOffset,
                                   nListSize + 4);
                            m_pFields[i].m_nDataOrDataOffset = nNewOffset;
                            nNewOffset += nListSize + 4;
                            continue;
                        }
                    }
                    m_pFields[i].m_nDataOrDataOffset = -1;
                }
            }
            delete m_pListIndices;
            m_pListIndices = pNew;
            m_pHeader->m_nListIndicesCount = nNewOffset;
        }
        else
        {
            m_pHeader->m_nListIndicesCount = 0;
        }
        m_nListIndicesLost = 0;
    }
}
// PC 0x00411710
CResGFF::CResGFF()
{
    m_pHeader = NULL;
    m_pStructs = NULL;
    m_nStructsAlloc = 0;
    m_pFields = NULL;
    m_nFieldsAlloc = 0;
    m_pLabels = NULL;
    m_nLabelsAlloc = 0;
    m_pFieldData = NULL;
    m_nFieldDataAlloc = 0;
    m_pFieldIndices = NULL;
    m_nFieldIndicesAlloc = 0;
    m_nFieldIndicesLost = 0;
    m_pListIndices = NULL;
    m_nListIndicesAlloc = 0;
    m_nListIndicesLost = 0;
    m_cFlags = 0;
    m_bLoaded = 0;
    m_bInResource = 0;
    m_bDemanded = 0;
    m_nStructAllocInc = STRUCT_STARTSIZE;
    m_nFieldAllocInc = FIELD_STARTSIZE;
    m_nLabelAllocInc = LABEL_STARTSIZE;
    m_nFieldDataAllocInc = DATAFIELD_STARTSIZE;
    m_nFieldIndicesAllocInc = DATALAYOUTFIELD_STARTSIZE;
    m_nListIndicesAllocInc = DATALAYOUTLIST_STARTSIZE;
    memset(m_szFileType, 0, 4);
}
// PC 0x004117A0
CResGFF::CResGFF(unsigned short nResType, char* szFileType,
                 const CResRef& cResRef)
{
    m_nFieldIndicesAllocInc = DATALAYOUTFIELD_STARTSIZE;
    m_nListIndicesAllocInc = DATALAYOUTLIST_STARTSIZE;
    m_pHeader = NULL;
    m_pStructs = NULL;
    m_nStructsAlloc = 0;
    m_pFields = NULL;
    m_nFieldsAlloc = 0;
    m_pLabels = NULL;
    m_nLabelsAlloc = 0;
    m_pFieldData = NULL;
    m_nFieldDataAlloc = 0;
    m_pFieldIndices = NULL;
    m_nFieldIndicesAlloc = 0;
    m_nFieldIndicesLost = 0;
    m_pListIndices = NULL;
    m_nListIndicesAlloc = 0;
    m_nListIndicesLost = 0;
    m_nStructAllocInc = STRUCT_STARTSIZE;
    m_nFieldAllocInc = FIELD_STARTSIZE;
    m_nLabelAllocInc = LABEL_STARTSIZE;
    m_nFieldDataAllocInc = DATAFIELD_STARTSIZE;
    m_cFlags = 0;
    m_bLoaded = 0;
    m_bInResource = 0;
    m_bDemanded = 0;
    memset(m_szFileType, 0, 4);
    strncpy(m_szFileType, szFileType, 4);

    g_pExoResMan->SetResObject(cResRef, nResType, this);
    Demand();
    if (m_pResource == NULL)
    {
        g_pExoResMan->ReleaseResObject(this);
    }
    else
    {
        m_bDemanded = 1;
    }
}
// PC 0x004118B0
int CResGFF::OnResourceServiced()
{
    if (m_pResource == NULL)
    {
        return 0;
    }

    if (m_pHeader != NULL && !m_bInResource)
    {
        delete m_pListIndices;
        m_pListIndices = NULL;
        delete m_pFieldIndices;
        m_pFieldIndices = NULL;
        delete m_pFieldData;
        m_pFieldData = NULL;
        delete m_pLabels;
        m_pLabels = NULL;
        delete m_pFields;
        m_pFields = NULL;
        delete m_pStructs;
        m_pStructs = NULL;
        delete m_pHeader;
        m_pHeader = NULL;
    }

    unsigned char* pData = (unsigned char*)m_pResource;
    m_pHeader = (CResGFFHeader*)pData;
    if (m_pHeader->m_nFileType != ((unsigned char)m_szFileType[0] |
             ((unsigned char)m_szFileType[1] << 8) |
             ((unsigned char)m_szFileType[2] << 16) |
             ((unsigned char)m_szFileType[3] << 24)))
    {
        m_pHeader = NULL;
        return 0;
    }
    if (m_pHeader->m_nVersion != ((unsigned char)VERSION[0] |
             ((unsigned char)VERSION[1] << 8) |
             ((unsigned char)VERSION[2] << 16) |
             ((unsigned char)VERSION[3] << 24)))
    {
        m_pHeader = NULL;
        return 0;
    }

    if (m_pHeader->m_nStructCount != 0)
    {
        m_pStructs = (CResGFFStruct*)(pData + m_pHeader->m_nStructOffset);
    }
    if (m_pHeader->m_nFieldCount != 0)
    {
        m_pFields = (CResGFFField*)(pData + m_pHeader->m_nFieldOffset);
    }
    if (m_pHeader->m_nLabelCount != 0)
    {
        m_pLabels = (CResGFFLabel*)(pData + m_pHeader->m_nLabelOffset);
    }
    if (m_pHeader->m_nFieldDataCount != 0)
    {
        m_pFieldData = pData + m_pHeader->m_nFieldDataOffset;
    }
    if (m_pHeader->m_nFieldIndicesCount != 0)
    {
        m_pFieldIndices = pData + m_pHeader->m_nFieldIndicesOffset;
    }
    if (m_pHeader->m_nListIndicesCount != 0)
    {
        m_pListIndices = pData + m_pHeader->m_nListIndicesOffset;
    }

    m_bInResource = 1;
    m_bLoaded = 1;
    return 1;
}
// PC 0x004119F0
int CResGFF::OnResourceFreed()
{
    m_bLoaded = 0;
    if (m_bInResource)
    {
        m_pListIndices = NULL;
        m_pFieldIndices = NULL;
        m_pFieldData = NULL;
        m_pLabels = NULL;
        m_pFields = NULL;
        m_pStructs = NULL;
        m_pHeader = NULL;
        m_bInResource = 0;
    }
    return 1;
}
// PC 0x00411A30
void CResGFF::ReleaseResource()
{
    if (m_bInResource)
    {
        if (m_bDemanded)
        {
            Release();
            g_pExoResMan->ReleaseResObject(this);
            m_bDemanded = 0;
        }
        m_bInResource = 0;
    }

    m_pHeader = NULL;
    m_pStructs = NULL;
    m_nStructsAlloc = 0;
    m_pFields = NULL;
    m_nFieldsAlloc = 0;
    m_pLabels = NULL;
    m_nLabelsAlloc = 0;
    m_pFieldData = NULL;
    m_nFieldDataAlloc = 0;
    m_pFieldIndices = NULL;
    m_nFieldIndicesAlloc = 0;
    m_nFieldIndicesLost = 0;
    m_pListIndices = NULL;
    m_nListIndicesAlloc = 0;
    m_nListIndicesLost = 0;
}
// PC 0x00411AA0
inline CResGFFField* CResGFF::GetField(CResGFFStruct* pStruct, unsigned long nIndex)
{
    if (pStruct != NULL)
    {
        CResGFFField* pFields = m_pFields;
        if (pFields != NULL)
        {
            if (nIndex < pStruct->m_nFieldCount)
            {
                if (pStruct->m_nFieldCount != 1)
                {
                    unsigned long nOffset =
                        pStruct->m_nDataOrDataOffset + 4 * nIndex;
                    unsigned char* pIndices = m_pFieldIndices;
                    if (pIndices != NULL &&
                        nOffset + 4 <= m_pHeader->m_nFieldIndicesCount)
                    {
                        unsigned long* pEntry =
                            (unsigned long*)(pIndices + nOffset);
                        return pFields + *pEntry;
                    }
                }
                else
                {
                    if (pStruct->m_nDataOrDataOffset <
                        m_pHeader->m_nFieldCount)
                    {
                        return pFields + pStruct->m_nDataOrDataOffset;
                    }
                }
            }
        }
    }
    return NULL;
}

CResGFFStruct* CResGFF::GetStruct(CResStruct* pStructure)
{
    if (pStructure != NULL && m_pHeader != NULL && m_pStructs != NULL &&
        pStructure->m_nStructIndex < m_pHeader->m_nStructCount)
    {
        return &m_pStructs[pStructure->m_nStructIndex];
    }
    return NULL;
}
// PC 0x00411B00
CResGFFField* CResGFF::GetField(CResStruct* pStructure, unsigned long nIndex)
{
    CResGFFStruct* pStruct = GetStruct(pStructure);
    if (pStruct != NULL)
    {
        CResGFFField* pFields = m_pFields;
        if (pFields != NULL)
        {
            unsigned long nCount = pStruct->m_nFieldCount;
            if (nIndex < nCount)
            {
                if (nCount == 1)
                {
                    unsigned long nData = pStruct->m_nDataOrDataOffset;
                    if (nData < m_pHeader->m_nFieldCount)
                    {
                        return pFields + nData;
                    }
                }
                else
                {
                    if (m_pFieldIndices != NULL &&
                        pStruct->m_nDataOrDataOffset + 4 * nIndex + 4 <=
                            m_pHeader->m_nFieldIndicesCount)
                    {
                        unsigned long* pEntry =
                            (unsigned long*)(m_pFieldIndices + 4 * nIndex +
                                             pStruct->m_nDataOrDataOffset);
                        return pFields + *pEntry;
                    }
                }
            }
        }
    }
    return NULL;
}
// PC 0x00411B90
unsigned char* CResGFF::GetDataField(CResGFFField* pField,
                                     unsigned long& nSize)
{
    if (pField == NULL)
    {
        return NULL;
    }
    if (m_pFieldData == NULL)
    {
        return NULL;
    }

    unsigned long nOffset = pField->m_nDataOrDataOffset;
    unsigned long nCount = m_pHeader->m_nFieldDataCount;
    if (nOffset < nCount)
    {
        nSize = nCount - nOffset;
        return m_pFieldData + pField->m_nDataOrDataOffset;
    }
    return NULL;
}
// PC 0x00411BD0
unsigned char* CResGFF::GetDataLayoutList(CResGFFField* pField,
                                          unsigned long& nSize)
{
    if (pField == NULL)
    {
        return NULL;
    }
    if (m_pListIndices == NULL)
    {
        return NULL;
    }

    unsigned long nOffset = pField->m_nDataOrDataOffset;
    unsigned long nCount = m_pHeader->m_nListIndicesCount;
    if (nOffset < nCount)
    {
        nSize = nCount - nOffset;
        return m_pListIndices + pField->m_nDataOrDataOffset;
    }
    return NULL;
}
// PC 0x00411C10
int CResGFF::InitializeForWriting()
{
    CResGFFHeader* pHeader = (CResGFFHeader*)new char[sizeof(CResGFFHeader)];
    pHeader->m_nStructOffset = 0;
    pHeader->m_nFieldOffset = 0;
    pHeader->m_nLabelOffset = 0;
    pHeader->m_nFieldDataOffset = 0;
    pHeader->m_nFieldIndicesOffset = 0;
    pHeader->m_nListIndicesOffset = 0;

    if (m_pHeader != NULL)
    {
        pHeader->m_nFileType = m_pHeader->m_nFileType;
        pHeader->m_nVersion = m_pHeader->m_nVersion;
        pHeader->m_nStructCount = m_pHeader->m_nStructCount;
        m_nStructsAlloc = m_nStructAllocInc + m_pHeader->m_nStructCount;
        pHeader->m_nFieldCount = m_pHeader->m_nFieldCount;
        m_nFieldsAlloc = m_nFieldAllocInc + m_pHeader->m_nFieldCount;
        pHeader->m_nLabelCount = m_pHeader->m_nLabelCount;
        m_nLabelsAlloc = m_nLabelAllocInc + m_pHeader->m_nLabelCount;
        pHeader->m_nFieldDataCount = m_pHeader->m_nFieldDataCount;
        m_nFieldDataAlloc = m_nFieldDataAllocInc + m_pHeader->m_nFieldDataCount;
        pHeader->m_nFieldIndicesCount = m_pHeader->m_nFieldIndicesCount;
        m_nFieldIndicesAlloc =
            m_nFieldIndicesAllocInc + m_pHeader->m_nFieldIndicesCount;
        pHeader->m_nListIndicesCount = m_pHeader->m_nListIndicesCount;
        m_nListIndicesAlloc =
            m_nListIndicesAllocInc + m_pHeader->m_nListIndicesCount;
    }
    else
    {
        pHeader->m_nFileType = 0;
        pHeader->m_nVersion = 0;
        pHeader->m_nStructCount = 0;
        m_nStructsAlloc = STRUCT_STARTSIZE;
        pHeader->m_nFieldCount = 0;
        m_nFieldsAlloc = FIELD_STARTSIZE;
        pHeader->m_nLabelCount = 0;
        m_nLabelsAlloc = LABEL_STARTSIZE;
        pHeader->m_nFieldDataCount = 0;
        m_nFieldDataAlloc = DATAFIELD_STARTSIZE;
        pHeader->m_nFieldIndicesCount = 0;
        m_nFieldIndicesAlloc = DATALAYOUTFIELD_STARTSIZE;
        pHeader->m_nListIndicesCount = 0;
        m_nListIndicesAlloc = DATALAYOUTLIST_STARTSIZE;
    }

    CResGFFStruct* pStructs = (CResGFFStruct*)new char[12 * m_nStructsAlloc];
    if (m_pStructs != NULL)
    {
        memcpy(pStructs, m_pStructs, 12 * m_pHeader->m_nStructCount);
    }

    CResGFFField* pFields = (CResGFFField*)new char[12 * m_nFieldsAlloc];
    if (m_pFields != NULL)
    {
        memcpy(pFields, m_pFields, 12 * m_pHeader->m_nFieldCount);
    }

    CResGFFLabel* pLabels = (CResGFFLabel*)new char[16 * m_nLabelsAlloc];
    if (m_pLabels != NULL)
    {
        memcpy(pLabels, m_pLabels, 16 * m_pHeader->m_nLabelCount);
    }

    unsigned char* pFieldData = new unsigned char[m_nFieldDataAlloc];
    if (m_pFieldData != NULL)
    {
        memcpy(pFieldData, m_pFieldData, m_pHeader->m_nFieldDataCount);
    }

    unsigned char* pFieldIndices = new unsigned char[m_nFieldIndicesAlloc];
    if (m_pFieldIndices != NULL)
    {
        memcpy(pFieldIndices, m_pFieldIndices, m_pHeader->m_nFieldIndicesCount);
    }

    unsigned char* pListIndices = new unsigned char[m_nListIndicesAlloc];
    if (m_pListIndices != NULL)
    {
        memcpy(pListIndices, m_pListIndices, m_pHeader->m_nListIndicesCount);
    }

    if (m_bInResource)
    {
        ReleaseResource();
    }
    else
    {
        delete m_pListIndices;
        delete m_pFieldIndices;
        delete m_pFieldData;
        delete m_pLabels;
        delete m_pFields;
        delete m_pStructs;
        delete m_pHeader;
    }

    m_pListIndices = pListIndices;
    m_pFieldIndices = pFieldIndices;
    m_pFieldData = pFieldData;
    m_pHeader = pHeader;
    m_pLabels = pLabels;
    m_pFields = pFields;
    m_pStructs = pStructs;
    return (int)pStructs;
}
// PC 0x00411ED0
int CResGFF::AddStruct(unsigned long nStructType)
{
    if (m_pHeader != NULL && m_pStructs != NULL)
    {
        unsigned long nIndex = m_pHeader->m_nStructCount;
        if (nIndex >= m_nStructsAlloc)
        {
            m_nStructsAlloc = m_nStructAllocInc + nIndex;
            CResGFFStruct* pNew =
                (CResGFFStruct*)new char[12 * m_nStructsAlloc];
            memcpy(pNew, m_pStructs, 12 * nIndex);
            delete m_pStructs;
            m_pStructs = pNew;
        }

        m_pHeader->m_nStructCount = nIndex + 1;
        m_pStructs[nIndex].m_nType = nStructType;
        m_pStructs[nIndex].m_nDataOrDataOffset = -1;
        m_pStructs[nIndex].m_nFieldCount = 0;
        return nIndex;
    }
    return -1;
}
// PC 0x00411F80
int CResGFF::AddLabel(char* szLabel)
{
    if (m_pHeader == NULL || m_pLabels == NULL)
    {
        return -1;
    }

    unsigned long nCount = m_pHeader->m_nLabelCount;
    CResGFFLabel label;
    memset(&label, 0, sizeof(label));
    strncpy((char*)label.m_anName, szLabel, 16);

    for (unsigned long i = nCount; i > 0; i--)
    {
        CResGFFLabel* pLabel = &m_pLabels[i - 1];
        if (label.m_anName[0] == pLabel->m_anName[0] &&
            label.m_anName[1] == pLabel->m_anName[1] &&
            label.m_anName[2] == pLabel->m_anName[2] &&
            label.m_anName[3] == pLabel->m_anName[3])
        {
            return i - 1;
        }
    }

    if (nCount >= m_nLabelsAlloc)
    {
        m_nLabelsAlloc = m_nLabelAllocInc + nCount;
        CResGFFLabel* pNew = (CResGFFLabel*)new char[16 * m_nLabelsAlloc];
        memcpy(pNew, m_pLabels, 16 * nCount);
        delete m_pLabels;
        m_pLabels = pNew;
    }

    m_pHeader->m_nLabelCount = nCount + 1;
    m_pLabels[nCount] = label;
    return nCount;
}
// PC 0x004120B0
unsigned char* CResGFF::AddDataField(unsigned long nOldOffset,
                                     unsigned long nOldSize,
                                     unsigned long nSize)
{
    if (m_pHeader != NULL && m_pFieldData != NULL)
    {
        unsigned long nOffset = m_pHeader->m_nFieldDataCount;
        if (nOffset == nOldOffset + nOldSize)
        {
            nOffset = nOldOffset;
        }

        unsigned long nEnd = nSize + nOffset;
        if (nEnd >= m_nFieldDataAlloc)
        {
            m_nFieldDataAlloc = m_nFieldDataAllocInc + nOffset + nSize;
            unsigned char* pNew = new unsigned char[m_nFieldDataAlloc];
            memcpy(pNew, m_pFieldData, m_pHeader->m_nFieldDataCount);
            delete m_pFieldData;
            m_pFieldData = pNew;
        }

        if (nOffset != nOldOffset)
        {
            memcpy(m_pFieldData + nOffset, m_pFieldData + nOldOffset,
                   nOldSize);
        }
        m_pHeader->m_nFieldDataCount = nEnd;
        return m_pFieldData + nOffset;
    }
    return NULL;
}
// PC 0x00412180
unsigned char* CResGFF::AddDataLayoutField(unsigned long nOldOffset,
                                           unsigned long nOldSize,
                                           unsigned long nSize)
{
    if (m_pHeader != NULL && m_pFieldIndices != NULL)
    {
        unsigned long nOffset = m_pHeader->m_nFieldIndicesCount;
        if (nOffset == nOldOffset + nOldSize)
        {
            nOffset = nOldOffset;
        }

        unsigned long nEnd = nSize + nOffset;
        if (nEnd >= m_nFieldIndicesAlloc)
        {
            m_nFieldIndicesAlloc = m_nFieldIndicesAllocInc + nOffset + nSize;
            unsigned char* pNew = new unsigned char[m_nFieldIndicesAlloc];
            memcpy(pNew, m_pFieldIndices, m_pHeader->m_nFieldIndicesCount);
            delete m_pFieldIndices;
            m_pFieldIndices = pNew;
        }

        if (nOffset != nOldOffset)
        {
            memcpy(m_pFieldIndices + nOffset, m_pFieldIndices + nOldOffset,
                   nOldSize);
            m_nFieldIndicesLost += nOldSize;
        }
        m_pHeader->m_nFieldIndicesCount = nEnd;
        return m_pFieldIndices + nOffset;
    }
    return NULL;
}
// PC 0x00412250
unsigned char* CResGFF::AddDataLayoutList(unsigned long nOldOffset,
                                          unsigned long nOldSize,
                                          unsigned long nSize)
{
    if (m_pHeader != NULL && m_pListIndices != NULL)
    {
        unsigned long nOffset = m_pHeader->m_nListIndicesCount;
        if (nOffset == nOldOffset + nOldSize)
        {
            nOffset = nOldOffset;
        }

        unsigned long nEnd = nSize + nOffset;
        if (nEnd >= m_nListIndicesAlloc)
        {
            m_nListIndicesAlloc = m_nListIndicesAllocInc + nOffset + nSize;
            unsigned char* pNew = new unsigned char[m_nListIndicesAlloc];
            memcpy(pNew, m_pListIndices, m_pHeader->m_nListIndicesCount);
            delete m_pListIndices;
            m_pListIndices = pNew;
        }

        if (nOffset != nOldOffset)
        {
            memcpy(m_pListIndices + nOffset, m_pListIndices + nOldOffset,
                   nOldSize);
            m_nListIndicesLost += nOldSize;
        }
        m_pHeader->m_nListIndicesCount = nEnd;
        return m_pListIndices + nOffset;
    }
    return NULL;
}
// PC 0x00412320
int CResGFF::GetElementType(CResStruct* pStructure)
{
    CResGFFStruct* pStruct = GetStruct(pStructure);
    if (pStruct == NULL)
    {
        return -1;
    }
    return pStruct->m_nType;
}
// PC 0x00412360
int CResGFF::GetFieldCount(CResStruct* pStructure)
{
    CResGFFStruct* pStruct = GetStruct(pStructure);
    if (pStruct == NULL)
    {
        return -1;
    }
    return pStruct->m_nFieldCount;
}
// PC 0x004123A0
int CResGFF::GetTopLevelStruct(CResStruct* pStructure)
{
    if (pStructure == NULL)
    {
        return 0;
    }
    pStructure->m_nStructIndex = 0;
    return 1;
}
// PC 0x004123C0
int CResGFF::CreateGFFFile(CResStruct* pStructure, const CExoString& sFileType,
                           const CExoString& sVersion)
{
    CExoString sGFFVersion(VERSION);

    if (pStructure != NULL && (unsigned long)sFileType.GetLength() >= 4 &&
        (unsigned long)sGFFVersion.GetLength() >= 4)
    {
        if (m_bInResource)
        {
            ReleaseResource();
        }
        InitializeForWriting();
        m_pHeader->m_nFileType =
            (unsigned char)sFileType[0] | ((unsigned char)sFileType[1] << 8) |
            ((unsigned char)sFileType[2] << 16) |
            ((unsigned char)sFileType[3] << 24);
        m_pHeader->m_nVersion = (unsigned char)sGFFVersion[0] |
                                ((unsigned char)sGFFVersion[1] << 8) |
                                ((unsigned char)sGFFVersion[2] << 16) |
                                ((unsigned char)sGFFVersion[3] << 24);
        pStructure->m_nStructIndex = AddStruct(-1);
        if (pStructure->m_nStructIndex == -1)
        {
            return 0;
        }
        return 1;
    }
    return 0;
}
// PC 0x00412550
int CResGFF::WriteGFFData(CExoFile& file, unsigned long& nSize)
{
    if (m_pHeader == NULL)
    {
        return 0;
    }

    nSize = sizeof(CResGFFHeader);
    m_pHeader->m_nStructOffset = nSize;
    nSize = 12 * m_pHeader->m_nStructCount + nSize;
    m_pHeader->m_nFieldOffset = nSize;
    nSize = 12 * m_pHeader->m_nFieldCount + nSize;
    m_pHeader->m_nLabelOffset = nSize;
    nSize = 16 * m_pHeader->m_nLabelCount + nSize;
    m_pHeader->m_nFieldDataOffset = nSize;
    nSize = m_pHeader->m_nFieldDataCount + nSize;
    m_pHeader->m_nFieldIndicesOffset = nSize;
    nSize = m_pHeader->m_nFieldIndicesCount + nSize;
    m_pHeader->m_nListIndicesOffset = nSize;
    nSize += m_pHeader->m_nListIndicesCount;

    if (file.Write(m_pHeader, sizeof(CResGFFHeader), 1) !=
        sizeof(CResGFFHeader))
    {
        return 0;
    }

    unsigned long nStructBytes = 12 * m_pHeader->m_nStructCount;
    if (file.Write(m_pStructs, nStructBytes, 1) != nStructBytes)
    {
        return 0;
    }

    unsigned long nFieldBytes = 12 * m_pHeader->m_nFieldCount;
    if (file.Write(m_pFields, nFieldBytes, 1) != nFieldBytes)
    {
        return 0;
    }

    unsigned long nLabelBytes = 16 * m_pHeader->m_nLabelCount;
    if (file.Write(m_pLabels, nLabelBytes, 1) != nLabelBytes)
    {
        return 0;
    }

    unsigned long nFieldDataBytes = m_pHeader->m_nFieldDataCount;
    if (file.Write(m_pFieldData, nFieldDataBytes, 1) != nFieldDataBytes)
    {
        return 0;
    }

    unsigned long nFieldIndicesBytes = m_pHeader->m_nFieldIndicesCount;
    if (file.Write(m_pFieldIndices, nFieldIndicesBytes, 1) !=
        nFieldIndicesBytes)
    {
        return 0;
    }

    unsigned long nListIndicesBytes = m_pHeader->m_nListIndicesCount;
    return file.Write(m_pListIndices, nListIndicesBytes, 1) ==
           nListIndicesBytes;
}
// PC 0x004126C0
int CResGFF::GetTotalSize()
{
    if (m_pHeader == NULL)
    {
        return 0;
    }
    return m_pHeader->m_nFieldIndicesCount + m_pHeader->m_nListIndicesCount +
           16 * m_pHeader->m_nLabelCount +
           12 * (m_pHeader->m_nStructCount + m_pHeader->m_nFieldCount) +
           m_pHeader->m_nFieldDataCount + sizeof(CResGFFHeader);
}
// PC 0x004126F0
CResGFF::~CResGFF()
{
    if (!m_bInResource)
    {
        delete m_pListIndices;
        m_pListIndices = NULL;
        delete m_pFieldIndices;
        m_pFieldIndices = NULL;
        delete m_pFieldData;
        m_pFieldData = NULL;
        delete m_pLabels;
        m_pLabels = NULL;
        delete m_pFields;
        m_pFields = NULL;
        delete m_pStructs;
        m_pStructs = NULL;
        delete m_pHeader;
        m_pHeader = NULL;
    }
    else
    {
        ReleaseResource();
    }
}
// PC 0x004127B0
unsigned long CResGFF::GetFieldByLabel(CResStruct* pStructure, char* szLabel)
{
    if (szLabel != NULL && m_pLabels != NULL)
    {
        CResGFFLabel label;
        memset(&label, 0, sizeof(label));
        strncpy((char*)label.m_anName, szLabel, 16);

        CResGFFStruct* pStruct = GetStruct(pStructure);
        unsigned long nCount;
        if (pStruct == NULL)
        {
            nCount = -1;
        }
        else
        {
            nCount = pStruct->m_nFieldCount;
        }

        unsigned long i = 0;
        if (nCount > 0)
        {
            do
            {
                CResGFFField* pField = GetField(pStruct, i);
                if (pField == NULL)
                {
                    break;
                }
                if (pField->m_nLabelIndex >= m_pHeader->m_nLabelCount)
                {
                    break;
                }
                CResGFFLabel* pLabel = &m_pLabels[pField->m_nLabelIndex];
                if (label.m_anName[0] == pLabel->m_anName[0] &&
                    label.m_anName[1] == pLabel->m_anName[1] &&
                    label.m_anName[2] == pLabel->m_anName[2] &&
                    label.m_anName[3] == pLabel->m_anName[3])
                {
                    return i;
                }
                i++;
            } while (i < nCount);
        }
    }
    return -1;
}
// PC 0x004128B0
CResGFFField* CResGFF::AddField(CResStruct* pStructure, char* szLabel,
                                unsigned long nFieldType)
{
    if (szLabel == NULL)
    {
        return NULL;
    }

    CResGFFStruct* pStruct = GetStruct(pStructure);
    if (pStruct == NULL)
    {
        return NULL;
    }

    unsigned long nField = m_pHeader->m_nFieldCount;
    if (nField >= m_nFieldsAlloc)
    {
        m_nFieldsAlloc = m_nFieldAllocInc + nField;
        CResGFFField* pNew = (CResGFFField*)new char[12 * m_nFieldsAlloc];
        memcpy(pNew, m_pFields, 12 * nField);
        delete m_pFields;
        m_pFields = pNew;
    }
    m_pHeader->m_nFieldCount = nField + 1;

    unsigned long nCount = pStruct->m_nFieldCount;
    if (nCount == 0)
    {
        pStruct->m_nDataOrDataOffset = nField;
    }
    else
    {
        unsigned char* pIndices;
        if (nCount == 1)
        {
            pIndices = AddDataLayoutField(0xFFFFFFFF, 0, 8);
            if (pIndices == NULL)
            {
                return NULL;
            }
            unsigned long* pFirst = (unsigned long*)pIndices;
            *pFirst = pStruct->m_nDataOrDataOffset;
        }
        else
        {
            pIndices = AddDataLayoutField(pStruct->m_nDataOrDataOffset,
                                          4 * nCount, 4 * nCount + 4);
            if (pIndices == NULL)
            {
                return NULL;
            }
        }
        unsigned long* pEntry = (unsigned long*)(pIndices + 4 * nCount);
        *pEntry = nField;
        pStruct->m_nDataOrDataOffset = pIndices - m_pFieldIndices;
    }
    pStruct->m_nFieldCount = nCount + 1;

    m_pFields[nField].m_nLabelIndex = AddLabel(szLabel);
    m_pFields[nField].m_nType = nFieldType;
    m_pFields[nField].m_nDataOrDataOffset = -1;
    return &m_pFields[nField];
}
// PC 0x004129F0
int CResGFF::GetFieldType(CResStruct* pStructure, char* szLabel,
                          unsigned long nIndex)
{
    unsigned long nField;
    if (szLabel != NULL)
    {
        nField = GetFieldByLabel(pStructure, szLabel);
    }
    else
    {
        nField = nIndex;
    }

    CResGFFField* pField = GetField(pStructure, nField);
    if (pField == NULL)
    {
        return -1;
    }
    return pField->m_nType;
}

unsigned long CResGFF::GetFieldSize(CResStruct* pStructure, char* szLabel,
                                    unsigned long nIndex)
{
    unsigned long nField;
    if (szLabel != NULL)
    {
        nField = GetFieldByLabel(pStructure, szLabel);
    }
    else
    {
        nField = nIndex;
    }

    CResGFFField* pField = GetField(pStructure, nField);
    if (pField == NULL)
    {
        return 0;
    }

    unsigned long nSize;
    unsigned char* pData;
    switch (pField->m_nType)
    {
    case 0:
    case 1:
        return 1;
    case 2:
    case 3:
        return 2;
    case 4:
    case 5:
    case 8:
        return 4;
    case 6:
    case 7:
    case 9:
        return 8;
    case 10:
    case 13:
        pData = GetDataField(pField, nSize);
        if (pData != NULL && nSize >= 4)
        {
            return *(unsigned long*)pData;
        }
        break;
    case 11:
        pData = GetDataField(pField, nSize);
        if (pData != NULL && nSize >= 1)
        {
            return *pData;
        }
        break;
    }
    return 0;
}
// PC 0x00412A30
int CResGFF::GetList(CResList* pList, CResStruct* pStructure, char* szLabel)
{
    if (pList == NULL || pStructure == NULL || szLabel == NULL)
    {
        return 0;
    }

    unsigned long nField = GetFieldByLabel(pStructure, szLabel);
    CResGFFField* pField = GetField(pStructure, nField);
    unsigned long nSize;
    if (pField != NULL && pField->m_nType == 15 &&
        GetDataLayoutList(pField, nSize) != NULL && nSize >= 4)
    {
        pList->m_nStructIndex = pStructure->m_nStructIndex;
        strncpy(pList->m_szLabel, szLabel, 16);
        return 1;
    }
    return 0;
}
// PC 0x00412AB0
unsigned long CResGFF::GetListCount(CResList* pList)
{
    unsigned long nSize;
    if (pList != NULL)
    {
        unsigned long nField =
            GetFieldByLabel((CResStruct*)pList, pList->m_szLabel);
        CResGFFField* pField = GetField((CResStruct*)pList, nField);
        if (pField != NULL && pField->m_nType == 15)
        {
            unsigned long* pIndices =
                (unsigned long*)GetDataLayoutList(pField, nSize);
            if (pIndices != NULL && nSize >= 4)
            {
                return *pIndices;
            }
        }
    }
    return 0;
}
// PC 0x00412B00
int CResGFF::GetListElement(CResStruct* pStructure, CResList* pList,
                            unsigned long nIndex)
{
    if (pStructure == NULL || pList == NULL)
    {
        return 0;
    }

    unsigned long nField =
        GetFieldByLabel((CResStruct*)pList, pList->m_szLabel);
    CResGFFField* pField = GetField((CResStruct*)pList, nField);
    if (pField == NULL)
    {
        return 0;
    }
    if (pField->m_nType != 15)
    {
        return 0;
    }

    unsigned long nSize;
    unsigned long* pIndices = (unsigned long*)GetDataLayoutList(pField, nSize);
    if (pIndices == NULL)
    {
        return 0;
    }
    if (nSize < 4 * nIndex + 8)
    {
        return 0;
    }

    unsigned long nStruct = pIndices[nIndex + 1];
    if (nIndex < *pIndices && nStruct < m_pHeader->m_nStructCount)
    {
        pStructure->m_nStructIndex = nStruct;
        return 1;
    }
    return 0;
}
// PC 0x00412B80
int CResGFF::GetStructFromStruct(CResStruct* pNewStruct, CResStruct* pParent,
                                 char* szLabel)
{
    if (pNewStruct == NULL || pParent == NULL || szLabel == NULL)
    {
        return 0;
    }

    unsigned long nField = GetFieldByLabel(pParent, szLabel);
    CResGFFField* pField = GetField(pParent, nField);
    if (pField != NULL && pField->m_nType == 14)
    {
        pNewStruct->m_nStructIndex = pField->m_nDataOrDataOffset;
        return 1;
    }
    return 0;
}
// PC 0x00412BD0
unsigned char CResGFF::ReadFieldBYTE(CResStruct* pStructure, char* szLabel,
                                     int& bSuccess, unsigned char nDefault)
{
    if (pStructure != NULL && szLabel != NULL)
    {
        unsigned long nField = GetFieldByLabel(pStructure, szLabel);
        CResGFFField* pField = GetField(pStructure, nField);
        if (pField != NULL && pField->m_nType == 0)
        {
            bSuccess = 1;
            return pField->m_nByte;
        }
        else
        {
            bSuccess = 0;
            return nDefault;
        }
    }
    else
    {
        bSuccess = 0;
        return nDefault;
    }
}
// PC 0x00412C40
char CResGFF::ReadFieldCHAR(CResStruct* pStructure, char* szLabel,
                            int& bSuccess, char nDefault)
{
    if (pStructure != NULL && szLabel != NULL)
    {
        unsigned long nField = GetFieldByLabel(pStructure, szLabel);
        CResGFFField* pField = GetField(pStructure, nField);
        if (pField != NULL && pField->m_nType == 1)
        {
            bSuccess = 1;
            return pField->m_cChar;
        }
        else
        {
            bSuccess = 0;
            return nDefault;
        }
    }
    else
    {
        bSuccess = 0;
        return nDefault;
    }
}
// PC 0x00412CB0
unsigned short CResGFF::ReadFieldWORD(CResStruct* pStructure, char* szLabel,
                                      int& bSuccess, unsigned short nDefault)
{
    if (pStructure != NULL && szLabel != NULL)
    {
        unsigned long nField = GetFieldByLabel(pStructure, szLabel);
        CResGFFField* pField = GetField(pStructure, nField);
        if (pField != NULL && pField->m_nType == 2)
        {
            bSuccess = 1;
            return pField->m_nWord;
        }
        else
        {
            bSuccess = 0;
            return nDefault;
        }
    }
    else
    {
        bSuccess = 0;
        return nDefault;
    }
}
// PC 0x00412D20
short CResGFF::ReadFieldSHORT(CResStruct* pStructure, char* szLabel,
                              int& bSuccess, short nDefault)
{
    if (pStructure != NULL && szLabel != NULL)
    {
        unsigned long nField = GetFieldByLabel(pStructure, szLabel);
        CResGFFField* pField = GetField(pStructure, nField);
        if (pField != NULL && pField->m_nType == 3)
        {
            bSuccess = 1;
            return pField->m_nShort;
        }
        else
        {
            bSuccess = 0;
            return nDefault;
        }
    }
    else
    {
        bSuccess = 0;
        return nDefault;
    }
}
// PC 0x00412D90
unsigned long CResGFF::ReadFieldDWORD(CResStruct* pStructure, char* szLabel,
                                      int& bSuccess, unsigned long nDefault)
{
    if (pStructure != NULL && szLabel != NULL)
    {
        unsigned long nField = GetFieldByLabel(pStructure, szLabel);
        CResGFFField* pField = GetField(pStructure, nField);
        if (pField != NULL && pField->m_nType == 4)
        {
            bSuccess = 1;
            return pField->m_nDataOrDataOffset;
        }
        else
        {
            bSuccess = 0;
            return nDefault;
        }
    }
    else
    {
        bSuccess = 0;
        return nDefault;
    }
}
// PC 0x00412E00
int CResGFF::ReadFieldINT(CResStruct* pStructure, char* szLabel, int& bSuccess,
                          int nDefault)
{
    if (pStructure != NULL && szLabel != NULL)
    {
        unsigned long nField = GetFieldByLabel(pStructure, szLabel);
        CResGFFField* pField = GetField(pStructure, nField);
        if (pField != NULL && pField->m_nType == 5)
        {
            bSuccess = 1;
            return pField->m_nDataOrDataOffset;
        }
        else
        {
            bSuccess = 0;
            return nDefault;
        }
    }
    else
    {
        bSuccess = 0;
        return nDefault;
    }
}
// PC 0x00412E70
float CResGFF::ReadFieldFLOAT(CResStruct* pStructure, char* szLabel,
                              int& bSuccess, float fDefault)
{
    if (pStructure != NULL && szLabel != NULL)
    {
        unsigned long nField = GetFieldByLabel(pStructure, szLabel);
        CResGFFField* pField = GetField(pStructure, nField);
        if (pField != NULL && pField->m_nType == 8)
        {
            bSuccess = 1;
            union
            {
                unsigned long nValue;
                float fValue;
            } value;
            value.nValue = pField->m_nDataOrDataOffset;
            return value.fValue;
        }
        else
        {
            bSuccess = 0;
            return fDefault;
        }
    }
    else
    {
        bSuccess = 0;
        return fDefault;
    }
}
// PC 0x00412EE0
unsigned __int64 CResGFF::ReadFieldDWORD64(CResStruct* pStructure,
                                           char* szLabel, int& bSuccess,
                                           unsigned __int64 nDefault)
{
    if (pStructure != NULL && szLabel != NULL)
    {
        unsigned long nField = GetFieldByLabel(pStructure, szLabel);
        CResGFFField* pField = GetField(pStructure, nField);
        if (pField != NULL && pField->m_nType == 6)
        {
            unsigned long nSize;
            unsigned __int64* pData =
                (unsigned __int64*)GetDataField(pField, nSize);
            if (pData != NULL && nSize >= 8)
            {
                bSuccess = 1;
                return *pData;
            }
            else
            {
                bSuccess = 0;
                return nDefault;
            }
        }
        else
        {
            bSuccess = 0;
            return nDefault;
        }
    }
    else
    {
        bSuccess = 0;
        return nDefault;
    }
}
// PC 0x00412F80
CResRef CResGFF::ReadFieldCResRef(CResStruct* pStructure, char* szLabel,
                                  int& bSuccess, const CResRef& cDefault)
{
    if (pStructure != NULL && szLabel != NULL)
    {
        unsigned long nField = GetFieldByLabel(pStructure, szLabel);
        CResGFFField* pField = GetField(pStructure, nField);
        if (pField != NULL && pField->m_nType == 11)
        {
            unsigned long nSize;
            unsigned char* pData = GetDataField(pField, nSize);
            if (pData != NULL && nSize >= 1)
            {
                unsigned char nLength = *pData;
                if (nSize < (unsigned long)nLength + 1)
                {
                    bSuccess = 0;
                    return cDefault;
                }
                bSuccess = 1;
                return CResRef(pData + 1, nLength);
            }
        }
    }
    bSuccess = 0;
    return cDefault;
}
// PC 0x00413030
CExoString CResGFF::ReadFieldCExoString(CResStruct* pStructure, char* szLabel,
                                        int& bSuccess,
                                        const CExoString& sDefault)
{
    if (pStructure != NULL && szLabel != NULL)
    {
        unsigned long nField = GetFieldByLabel(pStructure, szLabel);
        CResGFFField* pField = GetField(pStructure, nField);
        if (pField != NULL && pField->m_nType == 10)
        {
            unsigned long nSize;
            unsigned long* pData =
                (unsigned long*)GetDataField(pField, nSize);
            if (pData != NULL && nSize >= 4)
            {
                unsigned long nLength = *pData;
                if (nSize < nLength + 4)
                {
                    bSuccess = 0;
                    return sDefault;
                }
                bSuccess = 1;
                return CExoString((char*)(pData + 1), nLength);
            }
            else
            {
                bSuccess = 0;
                return sDefault;
            }
        }
        else
        {
            bSuccess = 0;
            return sDefault;
        }
    }
    else
    {
        bSuccess = 0;
        return sDefault;
    }
}
// PC 0x00413140
CExoLocString CResGFF::ReadFieldCExoLocString(CResStruct* pStructure,
                                              char* szLabel, int& bSuccess,
                                              const CExoLocString& lsDefault)
{
    CExoLocString lsResult;

    if (pStructure == NULL || szLabel == NULL)
    {
        bSuccess = 0;
        return lsDefault;
    }

    unsigned long nField = GetFieldByLabel(pStructure, szLabel);
    CResGFFField* pField = GetField(pStructure, nField);
    if (pField == NULL || pField->m_nType != 12)
    {
        bSuccess = 0;
        return lsDefault;
    }

    unsigned long nSize;
    unsigned long* pData = (unsigned long*)GetDataField(pField, nSize);
    if (pData == NULL || nSize < 4)
    {
        bSuccess = 0;
        return lsDefault;
    }

    unsigned long nRemaining = *pData;
    if (nSize < nRemaining + 4)
    {
        bSuccess = 0;
        return lsDefault;
    }
    pData++;
    lsResult.m_dwStringRef = *pData;
    pData++;
    unsigned long nStrings = *pData;
    pData++;
    nRemaining -= 8;

    for (unsigned long i = nStrings; i > 0; i--)
    {
        int nID = *pData;
        pData++;
        unsigned long nLength = *pData;
        pData++;
        nRemaining -= 8;
        lsResult.AddString(nID / 2, CExoString((char*)pData, nLength),
                           nID % 2);
        pData = (unsigned long*)((char*)pData + nLength);
        nRemaining -= nLength;
    }

    if (nRemaining != 0)
    {
        bSuccess = 0;
        return lsDefault;
    }
    bSuccess = 1;
    return lsResult;
}
// PC 0x00413320
Quaternion CResGFF::ReadFieldQuaternion(CResStruct* pStructure, char* szLabel,
                                    int& bSuccess, const Quaternion& qDefault)
{
    if (pStructure == NULL || szLabel == NULL)
    {
        bSuccess = 0;
        return qDefault;
    }

    unsigned long nField = GetFieldByLabel(pStructure, szLabel);
    CResGFFField* pField = GetField(pStructure, nField);
    if (pField == NULL || pField->m_nType != 16)
    {
        bSuccess = 0;
        return qDefault;
    }

    unsigned long nSize;
    Quaternion* pData = (Quaternion*)GetDataField(pField, nSize);
    if (pData != NULL && nSize >= 16)
    {
        bSuccess = 1;
        return *pData;
    }
    else
    {
        bSuccess = 0;
        return qDefault;
    }
}
// PC 0x00413410
Vector CResGFF::ReadFieldVector(CResStruct* pStructure, char* szLabel,
                            int& bSuccess, const Vector& vDefault)
{
    if (pStructure == NULL || szLabel == NULL)
    {
        bSuccess = 0;
        return vDefault;
    }

    unsigned long nField = GetFieldByLabel(pStructure, szLabel);
    CResGFFField* pField = GetField(pStructure, nField);
    if (pField == NULL || pField->m_nType != 17)
    {
        bSuccess = 0;
        return vDefault;
    }

    unsigned long nSize;
    Vector* pData = (Vector*)GetDataField(pField, nSize);
    if (pData != NULL && nSize >= 12)
    {
        bSuccess = 1;
        return *pData;
    }
    else
    {
        bSuccess = 0;
        return vDefault;
    }
}
// PC 0x004134F0
void* CResGFF::ReadFieldVOID(CResStruct* pStructure, void* pBuffer,
                             unsigned long nSize, char* szLabel, int& bSuccess,
                             void* pDefault)
{
    if (pStructure != NULL && szLabel != NULL && pBuffer != NULL)
    {
        unsigned long nField = GetFieldByLabel(pStructure, szLabel);
        CResGFFField* pField = GetField(pStructure, nField);
        if (pField != NULL && pField->m_nType == 13)
        {
            unsigned long nAvailable;
            unsigned long* pData =
                (unsigned long*)GetDataField(pField, nAvailable);
            if (pData != NULL && nAvailable >= 4)
            {
                unsigned long nLength = *pData;
                if (nAvailable < nLength + 4)
                {
                    bSuccess = 0;
                    return pDefault;
                }
                bSuccess = 1;
                if (nLength >= nSize)
                {
                    nLength = nSize;
                }
                memcpy(pBuffer, pData + 1, nLength);
                return pBuffer;
            }
            else
            {
                bSuccess = 0;
                return pDefault;
            }
        }
        bSuccess = 0;
        return pDefault;
    }
    bSuccess = 0;
    return pDefault;
}
// PC 0x004135C0
int CResGFF::AddList(CResList* pList, CResStruct* pStructure, char* szLabel)
{
    if (pList == NULL || pStructure == NULL || szLabel == NULL)
    {
        return 0;
    }

    if (m_bInResource)
    {
        InitializeForWriting();
    }

    CResGFFField* pField = AddField(pStructure, szLabel, 15);
    if (pField == NULL)
    {
        return 0;
    }

    unsigned char* pIndices =
        AddDataLayoutList(pField->m_nDataOrDataOffset, 0, 4);
    if (pIndices != NULL)
    {
        pField->m_nDataOrDataOffset = pIndices - m_pListIndices;
        unsigned long* pCount = (unsigned long*)pIndices;
        *pCount = 0;
        pList->m_nStructIndex = pStructure->m_nStructIndex;
        strncpy(pList->m_szLabel, szLabel, 16);
        return 1;
    }
    return 0;
}
// PC 0x00413650
int CResGFF::AddListElement(CResStruct* pStructure, CResList* pList,
                            unsigned long nStructType)
{
    if (pList == NULL || pStructure == NULL)
    {
        return 0;
    }

    if (m_bInResource)
    {
        InitializeForWriting();
    }

    unsigned long nField =
        GetFieldByLabel((CResStruct*)pList, pList->m_szLabel);
    CResGFFField* pField = GetField((CResStruct*)pList, nField);
    if (pField == NULL)
    {
        return 0;
    }
    if (pField->m_nType != 15)
    {
        return 0;
    }

    unsigned long nSize;
    unsigned long* pIndices = (unsigned long*)GetDataLayoutList(pField, nSize);
    if (pIndices == NULL)
    {
        return 0;
    }
    if (nSize < 4)
    {
        return 0;
    }

    unsigned long nCount = *pIndices;
    unsigned long nBytes = 4 * nCount + 4;
    if (nSize < nBytes)
    {
        return 0;
    }

    unsigned char* pNew = AddDataLayoutList(pField->m_nDataOrDataOffset,
                                            nBytes, 4 * nCount + 8);
    if (pNew == NULL)
    {
        return 0;
    }

    pField->m_nDataOrDataOffset = pNew - m_pListIndices;
    pStructure->m_nStructIndex = AddStruct(nStructType);
    if (pStructure->m_nStructIndex != -1)
    {
        unsigned long* pListCount = (unsigned long*)pNew;
        unsigned long* pEntry = (unsigned long*)(pNew + 4 * nCount + 4);
        *pListCount = nCount + 1;
        *pEntry = pStructure->m_nStructIndex;
        return 1;
    }
    return 0;
}
// PC 0x00413720
int CResGFF::AddStructToStruct(CResStruct* pNewStruct, CResStruct* pParent,
                               char* szLabel, unsigned long nStructType)
{
    if (pNewStruct == NULL || pParent == NULL || szLabel == NULL)
    {
        return 0;
    }

    if (m_bInResource)
    {
        InitializeForWriting();
    }

    CResGFFField* pField = AddField(pParent, szLabel, 14);
    if (pField == NULL)
    {
        return 0;
    }

    int nStruct = AddStruct(nStructType);
    if (nStruct != -1)
    {
        pField->m_nDataOrDataOffset = nStruct;
        pNewStruct->m_nStructIndex = nStruct;
        return 1;
    }
    return 0;
}
// PC 0x00413790
BOOL CResGFF::WriteFieldBYTE(CResStruct* pStructure, unsigned char nValue,
                             char* szLabel)
{
    if (pStructure == NULL || szLabel == NULL)
    {
        return 0;
    }

    if (m_bInResource)
    {
        InitializeForWriting();
    }

    CResGFFField* pField = AddField(pStructure, szLabel, 0);
    if (pField != NULL)
    {
        pField->m_nDataOrDataOffset = nValue;
        return 1;
    }
    return 0;
}
// PC 0x004137E0
BOOL CResGFF::WriteFieldCHAR(CResStruct* pStructure, char cValue, char* szLabel)
{
    if (pStructure == NULL || szLabel == NULL)
    {
        return 0;
    }

    if (m_bInResource)
    {
        InitializeForWriting();
    }

    CResGFFField* pField = AddField(pStructure, szLabel, 1);
    if (pField != NULL)
    {
        pField->m_nDataOrDataOffset = cValue;
        return 1;
    }
    return 0;
}
// PC 0x00413830
BOOL CResGFF::WriteFieldWORD(CResStruct* pStructure, unsigned short wValue,
                             char* szLabel)
{
    if (pStructure == NULL || szLabel == NULL)
    {
        return 0;
    }

    if (m_bInResource)
    {
        InitializeForWriting();
    }

    CResGFFField* pField = AddField(pStructure, szLabel, 2);
    if (pField != NULL)
    {
        pField->m_nDataOrDataOffset = wValue;
        return 1;
    }
    return 0;
}
// PC 0x00413880
BOOL CResGFF::WriteFieldSHORT(CResStruct* pStructure, short nValue,
                              char* szLabel)
{
    if (pStructure == NULL || szLabel == NULL)
    {
        return 0;
    }

    if (m_bInResource)
    {
        InitializeForWriting();
    }

    CResGFFField* pField = AddField(pStructure, szLabel, 3);
    if (pField != NULL)
    {
        pField->m_nDataOrDataOffset = nValue;
        return 1;
    }
    return 0;
}
// PC 0x004138D0
BOOL CResGFF::WriteFieldDWORD(CResStruct* pStructure, unsigned long lValue,
                              char* szLabel)
{
    if (pStructure == NULL || szLabel == NULL)
    {
        return 0;
    }

    if (m_bInResource)
    {
        InitializeForWriting();
    }

    CResGFFField* pField = AddField(pStructure, szLabel, 4);
    if (pField != NULL)
    {
        pField->m_nDataOrDataOffset = lValue;
        return 1;
    }
    return 0;
}
// PC 0x00413920
BOOL CResGFF::WriteFieldINT(CResStruct* pStructure, int nValue, char* szLabel)
{
    if (pStructure == NULL || szLabel == NULL)
    {
        return 0;
    }

    if (m_bInResource)
    {
        InitializeForWriting();
    }

    CResGFFField* pField = AddField(pStructure, szLabel, 5);
    if (pField != NULL)
    {
        pField->m_nDataOrDataOffset = nValue;
        return 1;
    }
    return 0;
}
// PC 0x00413970
BOOL CResGFF::WriteFieldDWORD64(CResStruct* pStructure, unsigned __int64 nValue,
                                char* szLabel)
{
    if (pStructure == NULL || szLabel == NULL)
    {
        return 0;
    }

    if (m_bInResource)
    {
        InitializeForWriting();
    }

    CResGFFField* pField = AddField(pStructure, szLabel, 6);
    if (pField == NULL)
    {
        return 0;
    }

    unsigned __int64* pData = (unsigned __int64*)AddDataField(0xFFFFFFFF, 0, 8);
    if (pData == NULL)
    {
        return 0;
    }

    pField->m_nDataOrDataOffset = (unsigned char*)pData - m_pFieldData;
    *pData = nValue;
    return 1;
}
// PC 0x004139E0
BOOL CResGFF::WriteFieldFLOAT(CResStruct* pStructure, float fValue,
                              char* szLabel)
{
    if (pStructure == NULL || szLabel == NULL)
    {
        return 0;
    }

    if (m_bInResource)
    {
        InitializeForWriting();
    }

    CResGFFField* pField = AddField(pStructure, szLabel, 8);
    if (pField != NULL)
    {
        pField->m_fFloat = fValue;
        return 1;
    }
    return 0;
}
// PC 0x00413A30
BOOL CResGFF::WriteFieldCResRef(CResStruct* pStructure, const CResRef& cValue,
                                char* szLabel)
{
    if (pStructure == NULL || szLabel == NULL)
    {
        return 0;
    }

    if (m_bInResource)
    {
        InitializeForWriting();
    }

    CResGFFField* pField = AddField(pStructure, szLabel, 11);
    if (pField == NULL)
    {
        return 0;
    }

    unsigned char nLength = cValue.GetLength();
    unsigned long nRemaining = nLength;
    unsigned char* pData = AddDataField(-1, 0, nRemaining + 1);
    if (pData != NULL)
    {
        pField->m_nDataOrDataOffset = pData - m_pFieldData;
        *pData = nLength;
        pData++;
        unsigned char* pSource = cValue.GetResRef();
        for (unsigned long i = 0; i < nRemaining; i++)
        {
            unsigned char c = pSource[i];
            if (c >= 'A' && c <= 'Z')
            {
                c += 32;
            }
            pData[i] = c;
        }
        return 1;
    }
    return 0;
}
// PC 0x00413AE0
BOOL CResGFF::WriteFieldCExoString(CResStruct* pStructure,
                                   const CExoString& sValue, char* szLabel)
{
    if (pStructure == NULL || szLabel == NULL)
    {
        return 0;
    }

    if (m_bInResource)
    {
        InitializeForWriting();
    }

    CResGFFField* pField = AddField(pStructure, szLabel, 10);
    if (pField == NULL)
    {
        return 0;
    }

    unsigned long nLength = sValue.GetLength();
    unsigned long* pData = (unsigned long*)AddDataField(0xFFFFFFFF, 0,
                                                        nLength + 4);
    if (pData != NULL)
    {
        pField->m_nDataOrDataOffset = (unsigned char*)pData - m_pFieldData;
        *pData = nLength;
        memcpy(pData + 1, sValue.CStr(), nLength);
        return 1;
    }
    return 0;
}
// PC 0x00413BA0
BOOL CResGFF::WriteFieldCExoLocString(CResStruct* pStructure,
                                      const CExoLocString& lsValue,
                                      char* szLabel)
{
    int bResult = 1;
    CExoString sString;

    if (pStructure == NULL || szLabel == NULL)
    {
        return 0;
    }

    if (m_bInResource)
    {
        InitializeForWriting();
    }

    CResGFFField* pField = AddField(pStructure, szLabel, 12);
    if (pField == NULL)
    {
        return 0;
    }

    int nSize = 8;
    for (unsigned long i = 0; i < (unsigned long)lsValue.GetStringCount(); i++)
    {
        nSize += lsValue.GetStringLength(i) + 8;
    }

    int* pData = (int*)AddDataField(0xFFFFFFFF, 0, nSize + 4);
    if (pData == NULL)
    {
        return 0;
    }

    pField->m_nDataOrDataOffset = (unsigned char*)pData - m_pFieldData;
    *pData = nSize;
    pData++;
    *pData = lsValue.m_dwStringRef;
    pData++;
    *pData = lsValue.GetStringCount();
    pData++;

    unsigned long nIndex = 0;
    unsigned long nLength;
    int nID;
    unsigned char nGender;
    if ((unsigned long)lsValue.GetStringCount() > 0)
    {
        while (1)
        {
            if (lsValue.GetString(nIndex, &nID, &sString, &nGender) == 0)
            {
                bResult = 0;
                nLength = 0;
                nID = -1;
                nGender = 0;
            }
            else
            {
                nLength = sString.GetLength();
            }
            *pData = nGender + 2 * nID;
            pData++;
            *pData = nLength;
            pData++;
            memcpy(pData, sString.CStr(), nLength);
            pData = (int*)((char*)pData + nLength);
            nIndex++;
            if (nIndex >= (unsigned long)lsValue.GetStringCount())
            {
                break;
            }
        }
    }
    return bResult;
}
// PC 0x00413D80
BOOL CResGFF::WriteFieldVOID(CResStruct* pStructure, const void* pData,
                             unsigned long nSize, char* szLabel)
{
    if (pStructure == NULL || szLabel == NULL)
    {
        return 0;
    }

    if (m_bInResource)
    {
        InitializeForWriting();
    }

    CResGFFField* pField = AddField(pStructure, szLabel, 13);
    if (pField == NULL)
    {
        return 0;
    }

    unsigned long* pDest = (unsigned long*)AddDataField(0xFFFFFFFF, 0,
                                                        nSize + 4);
    if (pDest != NULL)
    {
        pField->m_nDataOrDataOffset = (unsigned char*)pDest - m_pFieldData;
        *pDest = nSize;
        memcpy(pDest + 1, pData, nSize);
        return 1;
    }
    return 0;
}
// PC 0x00413E10
BOOL CResGFF::WriteFieldQuaternion(CResStruct* pStructure,
                                   const Quaternion& qValue, char* szLabel)
{
    if (pStructure == NULL || szLabel == NULL)
    {
        return 0;
    }

    if (m_bInResource)
    {
        InitializeForWriting();
    }

    CResGFFField* pField = AddField(pStructure, szLabel, 16);
    if (pField == NULL)
    {
        return 0;
    }

    Quaternion* pData = (Quaternion*)AddDataField(0xFFFFFFFF, 0, 16);
    if (pData != NULL)
    {
        Quaternion qTemp = qValue;
        pField->m_nDataOrDataOffset = (unsigned char*)pData - m_pFieldData;
        *pData = qTemp;
        return 1;
    }
    return 0;
}
// PC 0x00413EA0
BOOL CResGFF::WriteFieldVector(CResStruct* pStructure, const Vector& vValue,
                               char* szLabel)
{
    if (pStructure == NULL || szLabel == NULL)
    {
        return 0;
    }

    if (m_bInResource)
    {
        InitializeForWriting();
    }

    CResGFFField* pField = AddField(pStructure, szLabel, 17);
    if (pField == NULL)
    {
        return 0;
    }

    Vector* pData = (Vector*)AddDataField(0xFFFFFFFF, 0, 12);
    if (pData != NULL)
    {
        Vector vTemp = vValue;
        pField->m_nDataOrDataOffset = (unsigned char*)pData - m_pFieldData;
        *pData = vTemp;
        return 1;
    }
    return 0;
}
// PC 0x004141A0
int CResGFF::WriteGFFFile(const CExoString& sFileName, unsigned short nResType)
{
    CExoFile file(sFileName, nResType, "wb");
    if (!file.FileOpened())
    {
        return 0;
    }

    Pack(0, 0);
    return WriteGFFData(file, (unsigned long&)nResType);
}

