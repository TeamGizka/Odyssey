#include "exoresman.h"
#include "exobase.h"
#include <ctype.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>

static inline void ExoresfileTrace2();
static inline void ExoresfileTrace1();
#define EXORESFILE_TRACE() ExoresfileTrace1()

unsigned long CExoKeyTable::m_nCRC32[256];
// PC 0x0040E420
int CExoResFile::CloseAsyncFile()
{
    if (m_bAsyncFileOpened)
    {
        if (m_pAsyncFile)
        {
            delete m_pAsyncFile;
            m_pAsyncFile = NULL;
        }

        m_bAsyncFileOpened = 0;
    }

    return 1;
}
// PC 0x0040F5F0
CExoStringList* CExoKeyTable::GetKeysOfType(unsigned short nType)
{
    if (nType == 0xFFFF)
    {
        return NULL;
    }

    if (m_nTableSize == 0)
    {
        return NULL;
    }

    unsigned long i = 0;
    CExoStringList* pList = new CExoStringList(0, 1, 5);
    EXOXBOXASSERT(pList);

    for (; i < m_nTableSize; i++)
    {
        if (m_pKeyTable[i].m_nType == nType)
        {
            char szResRef[17];

            m_pKeyTable[i].m_resRef.CopyToString(szResRef);
            pList->Add(new CExoString(szResRef));
        }
    }

    if (pList->GetCount() <= 0)
    {
        delete pList;
        pList = NULL;
    }

    return pList;
}
// PC 0x0040D8F0
CExoKeyTable::CExoKeyTable()
{
    m_nUnknown00 = 0;
    m_nPriority = 0;
    m_bDestroying = 0;
    m_nTableSize = 0;
    m_pKeyTable = NULL;
    m_nCollisions = 0;
    m_nUnknown18 = 0xFFFFFFFF;
    m_nTableType = 0;
    m_sResourceName = "";
    m_nPackedFileCount = 0;
    m_pPackedFiles = NULL;

    for (unsigned long i = 0; i <= 0xFF; i++)
    {
        unsigned long nValue = i;
        for (int j = 8; j != 0; j--)
        {
            if (nValue & 1)
            {
                nValue = (nValue >> 1) ^ 0xEDB88320;
            }
            else
            {
                nValue >>= 1;
            }
        }
        m_nCRC32[i] = nValue;
    }
}
// PC 0x0040FB20
CExoKeyTable::~CExoKeyTable()
{
    if (m_pPackedFiles != NULL)
    {
        for (unsigned long i = 0; i < m_nPackedFileCount; i++)
        {
            if (m_pPackedFiles[i] != NULL)
            {
                m_pPackedFiles[i]->UnloadHeader();
                delete m_pPackedFiles[i];
            }
        }
        delete[] m_pPackedFiles;
    }
    if (m_pKeyTable != NULL)
    {
        delete[] m_pKeyTable;
    }

    m_bDestroying = 1;
    m_nUnknown00 = 0;
    m_nTableSize = 0;
    m_pKeyTable = NULL;
    m_nCollisions = 0;
    m_nUnknown18 = 0xFFFFFFFF;
    m_nTableType = 0;
    m_sResourceName = "";
    m_nPackedFileCount = 0;
    m_pPackedFiles = NULL;
}
// PC 0x0040DAD0
int CExoKeyTable::GetEntryCount(int bCountAll)
{
    int nCount = 0;

    for (unsigned long i = 0; i < m_nTableSize; i++)
    {
        CKeyTableEntry* pEntry = &m_pKeyTable[i];

        if (pEntry != NULL)
        {
            if (pEntry->m_nType != 0xFFFF &&
                (bCountAll || !g_pExoResMan->GetIsStaticType(pEntry->m_nType)))
            {
                nCount++;
            }
        }
    }

    return nCount;
}
// PC 0x0040DB30
int CExoKeyTable::LocateBifFile(const CExoString& sFileName)
{
    CExoFile* pFile = new CExoFile(sFileName, RESTYPE_BIF, "rb");

    if (pFile == NULL)
    {
        return 0;
    }

    if (!pFile->FileOpened())
    {
        delete pFile;
        return 0;
    }

    delete pFile;
    return 1;
}
// PC 0x0040DC10
void CExoKeyTable::DestroyTable()
{
    CExoKeyTable* pKeyTable = NULL;
    CKeyTableEntry* pKeyEntry = NULL;

    m_bDestroying = 1;

    for (unsigned long i = 0; i < m_nTableSize; i++)
    {
        if (m_pKeyTable[i].m_pRes != NULL)
        {
            if ((m_pKeyTable[i].m_nResId & 0xC0000000) != 0x40000000)
            {
                g_pExoResMan->GetKeyEntry(m_pKeyTable[i].m_resRef,
                                          m_pKeyTable[i].m_nType, &pKeyTable,
                                          &pKeyEntry);
            }
            else
            {
                pKeyEntry = NULL;
                pKeyTable = NULL;
            }

            if (pKeyEntry == NULL)
            {
                if (m_pKeyTable[i].m_pRes->m_nDemandCount > 0)
                {
                    CExoString sResRef;
                    char sMessage[256];

                    m_pKeyTable[i].m_resRef.CopyToString(sResRef);
                    sprintf(sMessage,
                            "CExoKeyTable::DestroyTable: Resource %s still in "
                            "demand during table deletion",
                            sResRef.CStr());
                    m_pKeyTable[i].m_pRes->m_nFlags |= 0x200;
                }
                else
                {
                    m_pKeyTable[i].m_pRes->Dump(0);
                    m_pKeyTable[i].m_pRes->m_pKeyEntry = NULL;
                    m_pKeyTable[i].m_pRes->m_nResId = 0xFFFFFFFF;
                }
            }
            else
            {
                pKeyEntry->m_pRes = m_pKeyTable[i].m_pRes;
                pKeyEntry->m_pRes->m_nResId = pKeyEntry->m_nResId;
                m_pKeyTable[i].m_pRes->m_pKeyEntry = pKeyEntry;
                m_pKeyTable[i].m_pRes = NULL;

                if (pKeyEntry->m_pRes->m_nDemandCount > 0)
                {
                    pKeyEntry->m_pRes->m_nFlags |= 0x100;
                }
                else
                {
                    pKeyEntry->m_pRes->Dump(0);
                }
            }
        }
    }
}
// PC 0x0040DDC0
unsigned long CExoKeyTable::GetTableIndex(unsigned long& nIndex,
                                          const CResRef& resRef,
                                          unsigned short nType)
{
    if (m_pKeyTable == 0)
    {
        return 0;
    }

    for (unsigned long i = 0; i < m_nTableSize; i++)
    {
        if (m_pKeyTable[i].m_nType == nType &&
            m_pKeyTable[i].m_resRef == resRef)
        {
            nIndex = i;
            return 1;
        }
    }

    return 0;
}
// PC 0x0040DE30
unsigned long CExoKeyTable::Hash(const CResRef& resRef, unsigned short nType)
{
    unsigned long nHash = 0;
    unsigned long i = 0;
    while (i < 16 && resRef.m_resRef[i] != 0)
    {
        unsigned long nIndex = (nHash ^ (tolower(resRef.m_resRef[i++]) + nType)) & 0xFF;
        unsigned long nCRC = m_nCRC32[nIndex];
        nHash = (nHash >> 8) ^ nCRC;
    }

    return nHash % m_nTableSize;
}
// PC 0x0040DE90
CExoPackedFile::~CExoPackedFile()
{
}
// PC 0x0040DEA0
CExoFile* CExoPackedFile::GetFile()
{
    return m_pFile;
}
// PC 0x00406600
CExoFile* CExoPackedFile::GetAsyncFile()
{
    return m_pAsyncFile;
}
// PC 0x0040F030
void CExoResFile::AddRefCount()
{
    if (m_nRefCount <= 0)
    {
        m_nRefCount = 0;
        OpenFile();
    }

    m_nRefCount++;
}
// PC 0x0040F030
void CExoEncapsulatedFile::AddRefCount()
{
    if (m_nRefCount <= 0)
    {
        m_nRefCount = 0;
        OpenFile();
    }

    m_nRefCount++;
}
// PC 0x0040F030
void CExoResourceImageFile::AddRefCount()
{
    if (m_nRefCount <= 0)
    {
        m_nRefCount = 0;
        OpenFile();
    }

    m_nRefCount++;
}
// PC 0x0040E3C0
void CExoResFile::AddAsyncRefCount()
{
    if (m_nAsyncRefCount <= 0)
    {
        m_nAsyncRefCount = 0;
        OpenAsyncFile();
    }

    m_nAsyncRefCount++;
}
// PC 0x0040E3C0
void CExoEncapsulatedFile::AddAsyncRefCount()
{
    if (m_nAsyncRefCount <= 0)
    {
        m_nAsyncRefCount = 0;
        OpenAsyncFile();
    }

    m_nAsyncRefCount++;
}
// PC 0x0040DEB0
int CExoResFile::CloseFile()
{
    if (m_bFileOpened)
    {
#ifndef _XBOX
        m_sFileName.CStr();
#endif
        if (m_pFile)
        {
            delete m_pFile;
            m_pFile = NULL;
        }
        m_bFileOpened = 0;
    }

    return 1;
}
// PC 0x0040DEF0
int CExoResFile::UnloadHeader()
{
    if (m_bHeaderLoaded)
    {
        if (m_pResourceTable)
        {
            delete m_pResourceTable;
            m_pResourceTable = NULL;
        }

        if (m_pHeader)
        {
            delete m_pHeader;
            m_pHeader = NULL;
        }

        m_bHeaderLoaded = 0;
        DeleteRefCount();
    }

    return 1;
}
// PC 0x0040E4B0
void CExoResFile::DeleteRefCount()
{
    int nRefCount = m_nRefCount;

    if (nRefCount <= 1)
    {
        m_nRefCount = 0;
        CloseFile();
    }
    else
    {
        nRefCount--;
        m_nRefCount = nRefCount;
    }
}
// PC 0x0040E4B0
void CExoEncapsulatedFile::DeleteRefCount()
{
    int nRefCount = m_nRefCount;

    if (nRefCount <= 1)
    {
        m_nRefCount = 0;
        CloseFile();
    }
    else
    {
        nRefCount--;
        m_nRefCount = nRefCount;
    }
}
// PC 0x0040E4D0
void CExoResFile::DeleteAsyncRefCount()
{
    int nRefCount = m_nAsyncRefCount;

    if (nRefCount <= 1)
    {
        m_nAsyncRefCount = 0;
        CloseAsyncFile();
    }
    else
    {
        nRefCount--;
        m_nAsyncRefCount = nRefCount;
    }
}
// PC 0x0040E4D0
void CExoEncapsulatedFile::DeleteAsyncRefCount()
{
    int nRefCount = m_nAsyncRefCount;

    if (nRefCount <= 1)
    {
        m_nAsyncRefCount = 0;
        CloseAsyncFile();
    }
    else
    {
        nRefCount--;
        m_nAsyncRefCount = nRefCount;
    }
}
// PC 0x0040DF40
unsigned long CExoResFile::GetResourceSize(unsigned long nId)
{
    return m_pResourceTable[nId & 0x3FFF].nSize;
}
// PC 0x0040DF60
int CExoResFile::Initialize()
{
#ifndef _XBOX
    m_sFileName.CStr();
#endif
    m_pFile = NULL;
    m_pAsyncFile = NULL;
    m_nResourceType = 0;
    m_nFileSize = 0;
    m_nRefCount = 0;
    m_nAsyncRefCount = 0;
    m_bFileOpened = 0;
    m_bAsyncFileOpened = 0;
    m_pHeader = NULL;
    m_pResourceTable = NULL;
    return 1;
}
// PC 0x0040DFA0
int CExoResFile::OpenFile()
{
    if (m_bFileOpened)
    {
        return 1;
    }

    m_pFile = new CExoFile(m_sFileName, RESTYPE_BIF, "rb");

    if (m_pFile == NULL)
    {
        return 0;
    }

    if (!m_pFile->FileOpened())
    {
        delete m_pFile;
        m_pFile = NULL;
        return 0;
    }
#ifndef _XBOX
    m_sFileName.CStr();
#endif
    m_bFileOpened = 1;
    return 1;
}
// PC 0x0040E0B0
int CExoResFile::OpenAsyncFile()
{
    if (m_bAsyncFileOpened)
    {
        return 1;
    }

    m_pAsyncFile = new CExoFile(m_sFileName, RESTYPE_BIF, "rb");

    if (m_pAsyncFile == NULL)
    {
        return 0;
    }

    if (!m_pAsyncFile->FileOpened())
    {
        delete m_pAsyncFile;
        m_pAsyncFile = NULL;
        return 0;
    }

    m_bAsyncFileOpened = 1;

    return 1;
}
// PC 0x0040E1C0
int CExoResFile::LoadHeader(unsigned char nType)
{
    if (m_bHeaderLoaded == 0)
    {
        AddRefCount();

        if (m_pFile == NULL)
        {
            m_nRefCount--;
            return 0;
        }
#ifndef _XBOX
        m_sFileName.CStr();
#endif
        m_nFileSize = m_pFile->GetSize();
        m_pHeader = new CExoResFileHeader;

        if (m_pFile->Read(m_pHeader, sizeof(CExoResFileHeader), 1) !=
            sizeof(CExoResFileHeader))
        {
            delete m_pHeader;
            m_pHeader = NULL;
            DeleteRefCount();
            return 0;
        }

        if (*(unsigned long*)m_pHeader->szFileType != 0x46464942)
        {
            delete m_pHeader;
            m_pHeader = NULL;
            DeleteRefCount();
            return 0;
        }

        if (*(unsigned long*)m_pHeader->szVersion != 0x20203156)
        {
            delete m_pHeader;
            m_pHeader = NULL;
            DeleteRefCount();
            return 0;
        }

        m_pResourceTable = NULL;

        if (m_pHeader->nVariableResourceCount != 0)
        {
            m_pResourceTable =
                new CExoResFileEntry[m_pHeader->nVariableResourceCount];
        }

        if (m_pResourceTable == NULL)
        {
            delete m_pHeader;
            m_pHeader = NULL;
            DeleteRefCount();
            return 0;
        }

        m_pFile->Seek(m_pHeader->nVariableTableOffset, 0);
        m_pFile->Read(m_pResourceTable, sizeof(CExoResFileEntry),
                      m_pHeader->nVariableResourceCount);

        m_bHeaderLoaded = 1;
    }

    return 1;
}
// PC 0x0040E2D0
unsigned long CExoResFile::ReadResource(unsigned long nId, void* pBuffer,
                                        unsigned long nSize,
                                        unsigned long nOffset)
{
    if (m_bFileOpened && pBuffer != NULL)
    {
        unsigned long nEntry = nId & 0x3FFF;

        if (nEntry < m_pHeader->nVariableResourceCount)
        {
            m_pFile->Seek(m_pResourceTable[nEntry].nOffset, 0);

            unsigned long nRead = m_pResourceTable[nEntry].nSize;

            if (nRead > nSize)
            {
                nRead = m_pFile->Read(pBuffer, nSize, 1);
            }
            else
            {
                nRead = m_pFile->Read(pBuffer, nRead, 1);
            }

            return nRead;
        }
    }

    return 0;
}
// PC 0x0040E350
void CExoResFile::ReadResourceAsync(unsigned long nId, void* pBuffer,
                                    unsigned long nSize, unsigned long nOffset)
{
    if (m_bAsyncFileOpened && pBuffer != NULL)
    {
        unsigned long nEntry = nId & 0x3FFF;

        if (nEntry < m_pHeader->nVariableResourceCount)
        {
            m_pAsyncFile->Seek(m_pResourceTable[nEntry].nOffset, 0);

            unsigned long nRead = m_pResourceTable[nEntry].nSize;

            if (nRead > nSize)
            {
                m_pAsyncFile->ReadAsync(pBuffer, nSize, 1);
            }
            else
            {
                m_pAsyncFile->ReadAsync(pBuffer, nRead, 1);
            }
        }
    }
}
// PC 0x0040E3E0
int CExoEncapsulatedFile::CloseFile()
{
    if (m_bFileOpened)
    {
        if (m_pFile)
        {
            delete m_pFile;
            m_pFile = 0;
        }

        m_bFileOpened = 0;
    }

    return 1;
}
// PC 0x0040E420
int CExoEncapsulatedFile::CloseAsyncFile()
{
    if (m_bAsyncFileOpened)
    {
        if (m_pAsyncFile)
        {
            delete m_pAsyncFile;
            m_pAsyncFile = NULL;
        }

        m_bAsyncFileOpened = 0;
    }

    return 1;
}
// PC 0x0040E460
int CExoEncapsulatedFile::UnloadHeader()
{
    if (m_bHeaderLoaded)
    {
        if (m_pResourceList)
        {
            delete[] m_pResourceList;
            m_pResourceList = 0;
        }

        m_lsDescription.ClearLocString();

        if (m_pHeader)
        {
            delete m_pHeader;
            m_pHeader = 0;
        }

        m_bHeaderLoaded = 0;
    }

    return 1;
}
// PC 0x0040E4F0
unsigned long CExoEncapsulatedFile::GetResourceSize(unsigned long nId)
{
    return m_pResourceList[nId & 0x3FFF].nSize;
}
// PC 0x0040E510
int CExoEncapsulatedFile::Initialize()
{

    m_pFile = 0;
    m_pAsyncFile = 0;
    m_nResourceType = 0;
    m_nFileSize = 0;
    m_nRefCount = 0;
    m_nAsyncRefCount = 0;
    m_bFileOpened = 0;
    m_bAsyncFileOpened = 0;
    m_pHeader = 0;
    m_pResourceList = 0;
    m_bFlag = 0;

    return 1;
}
// PC 0x0040E540
int CExoEncapsulatedFile::OpenFile()
{
    if (m_bFileOpened)
    {
        return 1;
    }

    m_pFile = 0;

    if (m_bFlag == 0)
    {
        m_pFile = new CExoFile(m_sFileName, RESTYPE_MOD, "rb");
    }
    else if (m_bFlag == 1)
    {
        m_pFile = new CExoFile(m_sFileName, RESTYPE_SAV, "rb");
    }
    else if (m_bFlag == 2)
    {
        m_pFile = new CExoFile(m_sFileName, 9997, "rb");
    }
    else if (m_bFlag == 3)
    {
        m_pFile = new CExoFile(m_sFileName, 2061, "rb");
    }
    else if (m_bFlag == 4)
    {
        m_pFile = new CExoFile(m_sFileName, RESTYPE_NWM, "rb");
    }

    if (m_pFile)
    {
        if (m_pFile->FileOpened() == 0)
        {
            delete m_pFile;
            m_pFile = 0;
        }
        else
        {
            m_bFileOpened = 1;
            return 1;
        }
    }

    m_bFileOpened = 0;
    return 0;
}
// PC 0x0040E820
int CExoEncapsulatedFile::OpenAsyncFile()
{
    if (m_bAsyncFileOpened)
    {
        return 1;
    }

    m_pAsyncFile = 0;

    if (m_bFlag == 0)
    {
        m_pAsyncFile = new CExoFile(m_sFileName, RESTYPE_MOD, "rb");
    }
    else if (m_bFlag == 1)
    {
        m_pAsyncFile = new CExoFile(m_sFileName, RESTYPE_SAV, "rb");
    }
    else if (m_bFlag == 2)
    {
        m_pAsyncFile = new CExoFile(m_sFileName, 9997, "rb");
    }
    else if (m_bFlag == 3)
    {
        m_pAsyncFile = new CExoFile(m_sFileName, 2061, "rb");
    }
    else if (m_bFlag == 4)
    {
        m_pAsyncFile = new CExoFile(m_sFileName, RESTYPE_NWM, "rb");
    }

    if (m_pAsyncFile)
    {
        if (m_pAsyncFile->FileOpened() == 0)
        {
            delete m_pAsyncFile;
            m_pAsyncFile = 0;
        }
        else
        {
            m_bAsyncFileOpened = 1;
            return 1;
        }
    }

    m_bAsyncFileOpened = 0;
    return 0;
}
// PC 0x0040EB00
int CExoEncapsulatedFile::LoadHeader(unsigned char nType)
{
    CExoString sString;

    if (m_bHeaderLoaded == 0)
    {
        m_pFile = 0;

        if (nType == 0)
        {
            m_pFile = new CExoFile(m_sFileName, RESTYPE_MOD, "rb");
        }
        else if (nType == 1)
        {
            m_pFile = new CExoFile(m_sFileName, RESTYPE_SAV, "rb");
        }
        else if (nType == 2)
        {
            m_pFile = new CExoFile(m_sFileName, 9997, "rb");
        }
        else if (nType == 3)
        {
            m_pFile = new CExoFile(m_sFileName, 2061, "rb");
        }
        else if (nType == 4)
        {
            m_pFile = new CExoFile(m_sFileName, RESTYPE_NWM, "rb");
        }

        if (m_pFile == 0)
        {
            return 0;
        }

        if (m_pFile->FileOpened() == 0)
        {
            delete m_pFile;
            m_pFile = 0;
            return 0;
        }

        m_bFlag = nType;
        m_nFileSize = m_pFile->GetSize();

        m_pHeader = new CExoEncapsulatedFileHeader;

        if (m_pHeader == 0)
        {
            delete m_pFile;
            m_pFile = 0;
            return 0;
        }

        m_pFile->Read(m_pHeader, sizeof(CExoEncapsulatedFileHeader), 1);

        unsigned long nSignature = m_pHeader->nFileType;

        if (nType == 2)
        {
            if (nSignature != 0x20465245)
            {
                delete m_pHeader;
                m_pHeader = 0;
                delete m_pFile;
                m_pFile = 0;
                return 0;
            }
        }
        else if (nType == 3)
        {
            if (nSignature != 0x204B4148)
            {
                delete m_pHeader;
                m_pHeader = 0;
                delete m_pFile;
                m_pFile = 0;
                return 0;
            }
        }
        else if (nSignature != 0x20444F4D)
        {
            delete m_pHeader;
            m_pHeader = 0;
            delete m_pFile;
            m_pFile = 0;
            return 0;
        }

        if (m_pHeader->nVersion != 0x302E3156)
        {
            delete m_pHeader;
            m_pHeader = 0;
            delete m_pFile;
            m_pFile = 0;
            return 0;
        }

        m_pFile->Seek(m_pHeader->nLocalizedStringOffset, 0);

        if (m_pHeader->nLocalizedStringSize)
        {
            unsigned char* pDescription =
                new unsigned char[m_pHeader->nLocalizedStringSize];

            if (pDescription == 0)
            {
                delete m_pHeader;
                m_pHeader = 0;
                delete m_pFile;
                m_pFile = 0;
                return 0;
            }

            m_pFile->Read(pDescription, 1, m_pHeader->nLocalizedStringSize);

            unsigned char* pString = pDescription;
            unsigned long nString;

            for (nString = 0; nString < m_pHeader->nLanguageCount; nString++)
            {
                int nLanguageId = *(int*)pString;
                pString += 4;

                int nLength = *(int*)pString;
                pString += 4;

                sString = CExoString((char*)pString, nLength);

                pString += nLength;

                m_lsDescription.AddString(nLanguageId, sString, 0);
            }

            delete[] pDescription;
        }

        m_pResourceList =
            new CExoEncapsulatedFileResource[m_pHeader->nEntryCount];

        if (m_pResourceList == 0)
        {
            delete m_pHeader;
            m_pHeader = 0;
            delete m_pFile;
            m_pFile = 0;
            return 0;
        }

        m_pFile->Seek(m_pHeader->nResourceListOffset, 0);
        m_pFile->Read(m_pResourceList, sizeof(CExoEncapsulatedFileResource),
                      m_pHeader->nEntryCount);

        m_bHeaderLoaded = 1;

        delete m_pFile;
    }

    return 1;
}
// PC 0x0040EF50
unsigned long CExoEncapsulatedFile::ReadResource(unsigned long nId,
                                                 void* pBuffer,
                                                 unsigned long nSize,
                                                 unsigned long nOffset)
{
    if (m_bFileOpened && pBuffer != NULL)
    {
        unsigned long nEntry = nId & 0x3FFF;

        if (nEntry < m_pHeader->nEntryCount)
        {
            m_pFile->Seek(m_pResourceList[nEntry].nOffset, 0);

            unsigned long nRead = m_pResourceList[nEntry].nSize;

            if (nRead > nSize)
            {
                return m_pFile->Read(pBuffer, nSize, 1);
            }

            return m_pFile->Read(pBuffer, nRead, 1);
        }
    }

    return 0;
}
// PC 0x0040EFC0
void CExoEncapsulatedFile::ReadResourceAsync(unsigned long nId, void* pBuffer,
                                             unsigned long nSize,
                                             unsigned long nOffset)
{
    if (m_bAsyncFileOpened && pBuffer != 0)
    {
        unsigned long nEntry = nId & 0x3FFF;

        if (nEntry < m_pHeader->nEntryCount)
        {
            m_pAsyncFile->Seek(m_pResourceList[nEntry].nOffset, 0);

            unsigned long nRead = m_pResourceList[nEntry].nSize;

            if (nRead > nSize)
            {
                m_pAsyncFile->ReadAsync(pBuffer, nSize, 1);
            }
            else
            {
                m_pAsyncFile->ReadAsync(pBuffer, nRead, 1);
            }
        }
    }
}

#ifdef _XBOX
extern int g_nContiguousResourceMemory;
#endif
// PC 0x0040F050
int CExoResourceImageFile::CloseFile()
{
    if (m_bFileOpened)
    {
        if (m_pImageFile)
        {
#ifdef _XBOX
            if (stricmp(m_sFileName.Right(2).CStr(), "dx") == 0)
            {
                g_nContiguousResourceMemory -= XPhysicalSize(m_pImageFile);
                D3D_FreeContiguousMemory(m_pImageFile);
            }
            else
            {
                delete[] m_pImageFile;
            }
#else
            delete[] m_pImageFile;
#endif
            m_pImageFile = NULL;
        }

        m_bFileOpened = 0;
    }

    return 1;
}
// PC 0x0040F080
void CExoResourceImageFile::DeleteRefCount()
{
    int nRefCount = m_nRefCount;

    if (nRefCount <= 1)
    {
        m_nRefCount = 0;
    }
    else
    {
        nRefCount--;
        m_nRefCount = nRefCount;
    }
}
// PC 0x0040F0A0
int CExoResourceImageFile::Initialize()
{
    m_pFile = NULL;
    m_nResourceType = 0;
    m_nFileSize = 0;
    m_nRefCount = 0;
    m_bFileOpened = 0;
    m_pImageFile = NULL;

    return 1;
}
// PC 0x0040F0C0
int CExoResourceImageFile::OpenFile()
{
    if (m_bFileOpened)
    {
        return 1;
    }

    m_pFile = new CExoFile(m_sFileName, RESTYPE_RSV, "rb");

    if (m_pFile == NULL)
    {
        return 0;
    }

    if (!m_pFile->FileOpened())
    {
        delete m_pFile;
        m_pFile = NULL;

        m_pFile = new CExoFile(m_sFileName, RESTYPE_RIM, "rb");

        if (m_pFile == NULL)
        {
            return 0;
        }

        if (!m_pFile->FileOpened())
        {
            return 0;
        }
    }

    unsigned long nFileSize = m_pFile->GetSize();

    m_nFileSize = nFileSize;
#ifdef _XBOX
    if (stricmp(m_sFileName.Right(2).CStr(), "dx") == 0)
    {
        m_pImageFile = (unsigned char*)D3D_AllocContiguousMemory(nFileSize, 128);
        g_nContiguousResourceMemory += XPhysicalSize(m_pImageFile);
    }
    else
    {
        m_pImageFile = new unsigned char[nFileSize];
    }
#else
    m_pImageFile = new unsigned char[nFileSize];
#endif

    if (m_pImageFile == NULL)
    {
        delete m_pFile;
        m_pFile = NULL;

        return 0;
    }

    if (m_pFile->Read(m_pImageFile, nFileSize, 1) != nFileSize)
    {
        delete m_pFile;
        m_pFile = NULL;

        return 0;
    }

    m_bFileOpened = 1;

    delete m_pFile;
    m_pFile = NULL;

    return 1;
}
// PC 0x0040F2B0
void CExoResourceImageFile::ReadResourceAsync(unsigned long nId, void* pBuffer,
                                              unsigned long nSize,
                                              unsigned long nOffset)
{
    ReadResource(nId, pBuffer, nSize, nOffset);
}
// PC 0x0040F2C0
CKeyTableEntry* CExoKeyTable::AddKey(const CResRef& resRef,
                                     unsigned short nType, unsigned long nResId,
                                     int, CExoString sDirectory)
{
    int nCollisions = 0;
    int bFound = 0;
    int bWrapped;
    unsigned long i;

    do
    {
        bWrapped = 0;

        unsigned long nHash = Hash(resRef, nType);
        i = nHash;

        if (resRef == "")
        {
            return NULL;
        }

        while (bFound == 0 && bWrapped == 0)
        {
            if (m_pKeyTable[i].m_resRef == "" ||
                (m_pKeyTable[i].m_resRef == resRef &&
                 m_pKeyTable[i].m_nType == nType))
            {
                if (m_pKeyTable[i].m_nResId != 0xFFFFFFFF &&
                    m_pKeyTable[i].m_resRef != "")
                {
                    char sMessage[100] =
                        "CExoKeyTable::AddKey: Duplicate Resource ";
                    char szResRef[17];

                    m_pKeyTable[i].m_resRef.CopyToString(szResRef);
                    strcat(sMessage, szResRef);
                    strcat(sMessage, " - Ignoring Duplicate");
                    return NULL;
                }

                unsigned long nKeyTableId = m_nId & 0x0FFFFFFF;
                m_pKeyTable[i].m_nResId = nResId;
                m_pKeyTable[i].m_nResId =
                    (m_pKeyTable[i].m_nResId & 0xFFF03FFF) |
                    (nKeyTableId << 14);
                m_pKeyTable[i].m_resRef = resRef;
                m_pKeyTable[i].m_nType = nType;
                m_pKeyTable[i].m_sDirectory = sDirectory;
                m_nCollisions++;
                bFound = 1;
            }
            else
            {
                m_nCollisions++;
                nCollisions++;
                i++;
                if (i >= m_nTableSize)
                {
                    i = 0;
                }
                if (i == nHash)
                {
                    bWrapped = 1;
                }
            }
        }
    } while (bWrapped == 1);

    if (bFound)
    {
        return &m_pKeyTable[i];
    }

    return NULL;
}

static CKeyTableEntry* AllocateKeyTableEntries(unsigned long nCount)
{
    return new CKeyTableEntry[nCount];
}
// PC 0x0040DA00
CKeyTableEntry* CExoKeyTable::AllocateTable(unsigned long nSize)
{
    CKeyTableEntry* pTable = new CKeyTableEntry[nSize];
    for (unsigned long i = 0; i < nSize; i++)
    {
        CKeyTableEntry* pEntry = &pTable[i];
        if (pEntry != NULL)
        {
            pEntry->m_resRef = "";
            pEntry->m_pRes = NULL;
            pEntry->m_nResId = 0xFFFFFFFF;
            pEntry->m_nReserved = 0;
            pEntry->m_nType = 0xFFFF;
            pEntry->m_sDirectory = "";
        }
    }

    return pTable;
}
// PC 0x0040F520
CKeyTableEntry* CExoKeyTable::FindKey(const CResRef& resRef,
                                      unsigned short nType)
{
    CKeyTableEntry* pEntry = NULL;
    int bWrapped = 0;

    if (resRef == "")
    {
        return NULL;
    }

    if (m_nTableSize == 0)
    {
        return NULL;
    }

    unsigned long nHash = Hash(resRef, nType);
    unsigned long i = nHash;

    while (pEntry == NULL && bWrapped == 0)
    {
        if (m_pKeyTable[i].m_resRef == resRef &&
            m_pKeyTable[i].m_nType == nType)
        {
            pEntry = &m_pKeyTable[i];
            break;
        }

        if (m_pKeyTable[i].m_resRef == "")
        {
            break;
        }

        i++;
        if (i >= m_nTableSize)
        {
            i = 0;
        }
        if (i == nHash)
        {
            bWrapped = 1;
        }
    }

    return pEntry;
}
// PC 0x0040F750
CExoResFile::CExoResFile()
{
    ExoresfileTrace2();
    m_nRefCount = 0;
    Initialize();
}
// PC 0x0040F7D0
CExoResFile::~CExoResFile()
{
    ExoresfileTrace2();
    CloseFile();
}
// PC 0x0040F860
CExoEncapsulatedFile::CExoEncapsulatedFile()
{
    ExoresfileTrace2();
    Initialize();
}
// PC 0x0040F8E0
CExoEncapsulatedFile::~CExoEncapsulatedFile()
{
    ExoresfileTrace2();
    CloseFile();
}

CExoResourceImageFile::CExoResourceImageFile()
{
    ExoresfileTrace2();
    m_pFile = NULL;
    m_nResourceType = 0;
    m_nFileSize = 0;
    m_nRefCount = 0;
    m_bFileOpened = 0;
    m_pImageFile = NULL;
}
// PC 0x0040F970
CExoResourceImageFile::~CExoResourceImageFile()
{
    ExoresfileTrace2();
    CExoResourceImageFile::CloseFile();
}
// PC 0x0040F9F0
unsigned long CExoResourceImageFile::GetResourceSize(unsigned long nId)
{
    ExoresfileTrace2();
    CExoResourceImageFileEntry sEntry = GetKeyListEntry(nId & 0x3FFF);

    return sEntry.m_nSize;
}
// PC 0x0040FA30
void* CExoResourceImageFile::GetResource(unsigned long nId)
{
    ExoresfileTrace2();
    CExoResourceImageFileEntry sEntry = GetKeyListEntry(nId & 0x3FFF);

    return m_pImageFile + sEntry.m_nOffset;
}
// PC 0x0040FA70
unsigned long CExoResourceImageFile::ReadResource(unsigned long nId,
                                                  void* pBuffer,
                                                  unsigned long nSize,
                                                  unsigned long nOffset)
{
    ExoresfileTrace2();
    if (m_bFileOpened && pBuffer != NULL)
    {
        unsigned long nEntry = nId & 0x3FFF;

        CExoResourceImageFileHeader* pHeader =
            (CExoResourceImageFileHeader*)m_pImageFile;

        if (nEntry < pHeader->m_nEntryCount)
        {
            CExoResourceImageFileEntry* pEntry = ((
                CExoResourceImageFileEntry**)(m_pImageFile +
                                              pHeader->m_nEntryOffset))[nEntry];

            unsigned long nRead = pEntry->m_nSize;

            if (nRead >= nSize)
            {
                nRead = nSize;
            }

            memcpy(pBuffer, m_pImageFile + pEntry->m_nOffset, nRead);

            return nRead;
        }
    }

    return 0;
}
// PC 0x00410D70
int CExoKeyTable::AddDirectoryContents(int bFlag)
{
    CExoString sFile;
    CExoArrayList<CExoString> lstFiles;
    CExoArrayList<CExoString> lstSubDirectories;
    CExoArrayList<CExoArrayList<CExoString> > lstSubDirectoryFiles;
    int nSubDirectories = 0;
    int nSubDirectoryFiles = 0;
    int i = 0;
    int j = 0;
    CResRef resRef;

    CExoString sOverride("OVERRIDE");
    if (m_sResourceName.ComparePrefixNoCase(sOverride, sOverride.GetLength()))
    {
        g_pExoBase->GetDirectoryList(&lstSubDirectories, m_sResourceName,
                                     RESTYPE_INVALID, 1, 0);
        nSubDirectories = lstSubDirectories.num;
    }

    CExoString sDirectory;
    int n;
    for (n = 0; n < nSubDirectories; n++)
    {
        sDirectory = CExoString("OVERRIDE:") + lstSubDirectories[n];

        CExoArrayList<CExoString> lstSubFiles;
        g_pExoBase->GetDirectoryList(&lstSubFiles, sDirectory, RESTYPE_INVALID,
                                     0, 0);
        lstSubDirectoryFiles.Add(lstSubFiles);
        nSubDirectoryFiles += lstSubDirectoryFiles[n].num;
    }

    g_pExoBase->GetDirectoryList(&lstFiles, m_sResourceName, RESTYPE_INVALID, 0, 0);

    int nFiles = lstFiles.num;

    m_nTableSize = 2 * (nFiles + nSubDirectoryFiles) + 1;
    m_pKeyTable = AllocateTable(m_nTableSize);

    if (m_pKeyTable == NULL)
    {
        return 0;
    }

    for (; i < nFiles; i++)
    {
        sFile = lstFiles[i];

        unsigned short nType = g_pExoResMan->GetResTypeFromFile(sFile);

        if (nType != 0xFFFF)
        {
            g_pExoResMan->GetResRefFromFile(resRef, sFile);

            if (resRef != "")
            {
                unsigned long nResId = (m_nId | 0xFFFFFC00) << 20;

                AddKey(resRef, nType, nResId, bFlag, CExoString(""));
            }
        }
    }

    for (n = 0; n < nSubDirectories; n++)
    {
        sDirectory = lstSubDirectories[n];

        for (; j < lstSubDirectoryFiles[n].num; j++)
        {
            sFile = lstSubDirectoryFiles[n][j];

            unsigned short nType = g_pExoResMan->GetResTypeFromFile(sFile);

            if (nType != 0xFFFF)
            {
                g_pExoResMan->GetResRefFromFile(resRef, sFile);

                if (resRef != "")
                {
                    unsigned long nResId = (m_nId | 0xFFFFFC00) << 20;

                    AddKey(resRef, nType, nResId, bFlag, sDirectory);
                }
            }
        }
        j = 0;
    }

    return 1;
}
// PC 0x0040FC00
int CExoKeyTable::AddEncapsulatedContents(int bFlag)
{
    CExoString sString1;
    CExoString sString2;
    CResRef resRef;
    unsigned char nType;
    unsigned long nCount;
    CExoEncapsulatedFileHeader sHeader;
    CExoEncapsulatedFileKey sKey;

    CExoFile* pFile = new CExoFile(m_sResourceName, RESTYPE_NWM, "rb");

    if (pFile == NULL)
    {
        m_nUnknown00 = 0;
        return 0;
    }

    if (!pFile->FileOpened())
    {
        delete pFile;
        pFile = NULL;
    }
    else
    {
        nType = 4;
    }

    if (pFile == NULL)
    {
        pFile = new CExoFile(m_sResourceName, RESTYPE_MOD, "rb");

        if (pFile == NULL)
        {
            m_nUnknown00 = 0;
            return 0;
        }

        if (!pFile->FileOpened())
        {
            delete pFile;
            pFile = NULL;
        }
        else
        {
            nType = 0;
        }
    }

    if (pFile == NULL)
    {
        pFile = new CExoFile(m_sResourceName, RESTYPE_SAV, "rb");

        if (pFile == NULL)
        {
            m_nUnknown00 = 0;
            return 0;
        }

        if (!pFile->FileOpened())
        {
            delete pFile;
            pFile = NULL;
        }
        else
        {
            nType = 1;
        }
    }

    if (pFile == NULL)
    {
        pFile = new CExoFile(m_sResourceName, 9997, "rb");

        if (pFile == NULL)
        {
            m_nUnknown00 = 0;
            return 0;
        }

        if (!pFile->FileOpened())
        {
            delete pFile;
            pFile = NULL;
        }
        else
        {
            nType = 2;
        }
    }

    if (pFile == NULL)
    {
        pFile = new CExoFile(m_sResourceName, 2061, "rb");

        if (pFile == NULL)
        {
            m_nUnknown00 = 0;
            return 0;
        }

        if (!pFile->FileOpened())
        {
            delete pFile;
            pFile = NULL;
        }
        else
        {
            nType = 3;
        }
    }

    if (pFile == NULL)
    {
        m_nUnknown00 = 0;
        return 0;
    }

    pFile->Read(&sHeader, sizeof(CExoEncapsulatedFileHeader), 1);

    if (nType == 2)
    {
        if (sHeader.nFileType != 0x20465245)
        {
            m_nUnknown00 = 0;
            delete pFile;
            return 0;
        }
    }
    else if (nType == 3)
    {
        if (sHeader.nFileType != 0x204B4148)
        {
            m_nUnknown00 = 0;
            delete pFile;
            return 0;
        }
    }
    else if (sHeader.nFileType != 0x20444F4D)
    {
        m_nUnknown00 = 0;
        delete pFile;
        return 0;
    }

    if (sHeader.nVersion != 0x302E3156)
    {
        m_nUnknown00 = 0;
        delete pFile;
        return 0;
    }

    if (sHeader.nEntryCount <= 0)
    {
        m_nUnknown00 = 0;
        delete pFile;
        return 0;
    }

    nCount = sHeader.nEntryCount;

    m_nTableSize = (unsigned long)(nCount * 1.2);
    m_pKeyTable = AllocateTable(m_nTableSize);

    if (m_pKeyTable == NULL)
    {
        m_nUnknown00 = 0;
        delete pFile;
        return 0;
    }

    pFile->Seek(sHeader.nKeyListOffset, 0);

    for (unsigned long i = 0; i < sHeader.nEntryCount; i++)
    {
        pFile->Read(&sKey, sizeof(CExoEncapsulatedFileKey), 1);
        resRef = sKey.szResRef;
        sKey.nResId =
            (sKey.nResId & 0xFFFFF) | (((m_nId & 0x3FF) | 0xFFFFF800) << 20);
        AddKey(resRef, sKey.nResType, sKey.nResId, bFlag, CExoString(""));
    }

    delete pFile;

    m_nPackedFileCount = 1;
    m_pPackedFiles = new CExoPackedFile*[1];

    if (m_pPackedFiles == NULL)
    {
        return 0;
    }

    m_pPackedFiles[0] = new CExoEncapsulatedFile;

    if (m_pPackedFiles[0] == NULL)
    {
        delete[] m_pPackedFiles;
        m_pPackedFiles = NULL;
        return 0;
    }

    m_pPackedFiles[0]->m_sFileName = m_sResourceName;
    m_pPackedFiles[0]->LoadHeader(nType);

    return 1;
}
// PC 0x004101F0
int CExoKeyTable::AddResourceImageContents(int bFlag)
{
    ExoresfileTrace2();
    CResRef resRef;
    CExoResourceImageFileEntry sEntry;
    unsigned long i;

    m_nPackedFileCount = 1;
    m_pPackedFiles = new CExoPackedFile*[1];

    if (m_pPackedFiles == NULL)
    {
        return 0;
    }

    m_pPackedFiles[0] = new CExoResourceImageFile;

    if (m_pPackedFiles[0] == NULL)
    {
        delete[] m_pPackedFiles;
        m_pPackedFiles = NULL;
        return 0;
    }

    m_pPackedFiles[0]->m_sFileName = m_sResourceName;

    if (!m_pPackedFiles[0]->OpenFile())
    {
        return 0;
    }

    CExoResourceImageFileHeader* pHeader =
        (CExoResourceImageFileHeader*)((CExoResourceImageFile*)
                                           m_pPackedFiles[0])
            ->m_pImageFile;

    if (pHeader->m_nEntryCount <= 0)
    {
        m_nUnknown00 = 0;
        delete m_pPackedFiles[0];
        delete[] m_pPackedFiles;
        m_pPackedFiles = NULL;
        return 0;
    }

    m_nTableSize = (unsigned long)(pHeader->m_nEntryCount * 1.2);
    m_pKeyTable = AllocateTable(m_nTableSize);

    if (m_pKeyTable == NULL)
    {
        m_nUnknown00 = 0;
        delete m_pPackedFiles[0];
        delete[] m_pPackedFiles;
        m_pPackedFiles = NULL;
        return 0;
    }

    for (i = 0; i < pHeader->m_nEntryCount; i++)
    {
        sEntry =
            ((CExoResourceImageFile*)m_pPackedFiles[0])->GetKeyListEntry(i);
        resRef = (unsigned char*)sEntry.m_szResRef;
        sEntry.m_nId =
            (sEntry.m_nId & 0xFFFFF) | (((m_nId & 0x3FF) | 0x400) << 20);
        AddKey(resRef, sEntry.m_nType, sEntry.m_nId, bFlag, CExoString(""));
    }

    return 1;
}
// PC 0x004103D0
int CExoKeyTable::AddKeyTableContents(int bFlag)
{
    CExoString sDrive;
    CExoString sFileName;
    CExoString sCD;
    CResRef resRef;
    CExoKeyFileHeader sHeader;
    CExoKeyFileKeyEntry sKey;
    unsigned char* pFileTable;
    unsigned long nFileTableSize;
    CExoKeyFileBifEntry* pEntries;
    int nDot;
    unsigned long i;

    CExoFile* pFile = new CExoFile(m_sResourceName, 9999, "rb");

    if (pFile == NULL)
    {
        m_nUnknown00 = 0;
        return 0;
    }

    if (!pFile->FileOpened())
    {
        m_nUnknown00 = 0;
        delete pFile;
        return 0;
    }

    pFile->Read(&sHeader, 24, 1);

    if (sHeader.nFileType != 0x2059454B)
    {
        m_nUnknown00 = 0;
        delete pFile;
        return 0;
    }

    if (sHeader.nVersion != 0x20203156)
    {
        m_nUnknown00 = 0;
        delete pFile;
        return 0;
    }

    if (sHeader.nBifCount <= 0 && sHeader.nKeyCount <= 0)
    {
        m_nUnknown00 = 0;
        delete pFile;
        return 0;
    }

    m_nTableSize =
        1023 - (unsigned long)((float)sHeader.nKeyCount * -1.1764705882352942);
    m_pKeyTable = AllocateTable(m_nTableSize);

    if (m_pKeyTable == NULL)
    {
        m_nUnknown00 = 0;
        delete pFile;
        return 0;
    }

    nFileTableSize = sHeader.nKeyTableOffset - sHeader.nBifTableOffset;
    pFileTable = (unsigned char*)EXOMALLOC(nFileTableSize, "exoresfile.cpp", 991);

    if (pFileTable == NULL)
    {
        m_nUnknown00 = 0;
        delete[] m_pKeyTable;
        m_pKeyTable = NULL;
        delete pFile;
        return 0;
    }

    pFile->Seek(sHeader.nBifTableOffset, 0);
    pFile->Read(pFileTable, nFileTableSize, 1);

    int nColon = m_sResourceName.Find(':', 0);

    if (nColon <= 0)
    {
        return 0;
    }

    sDrive = m_sResourceName.SubString(0, nColon);

    m_nPackedFileCount = sHeader.nBifCount;
    m_pPackedFiles = new CExoPackedFile*[sHeader.nBifCount];

    if (m_pPackedFiles == NULL)
    {
        delete[] m_pKeyTable;
        m_pKeyTable = NULL;
        EXOFREE(pFileTable, "exoresfile.cpp", 1055);
        delete pFile;
        return 0;
    }

    pEntries = (CExoKeyFileBifEntry*)pFileTable;

    for (i = 0; i < sHeader.nBifCount; i++)
    {
        m_pPackedFiles[i] = new CExoResFile;

        if (m_pPackedFiles[i] == NULL)
        {
            delete[] m_pPackedFiles;
            m_pPackedFiles = NULL;
            delete[] m_pKeyTable;
            m_pKeyTable = NULL;
            EXOFREE(pFileTable, "exoresfile.cpp", 1081);
            delete pFile;
            return 0;
        }

        m_pPackedFiles[i]->m_nResourceType = pEntries[i].nDrives;
        m_pPackedFiles[i]->m_nFileSize = pEntries[i].nFileSize;
        pEntries[i].nNameOffset -= sHeader.nBifTableOffset;
        sFileName = (char*)(pFileTable + pEntries[i].nNameOffset);

        nDot = sFileName.Find('.', 0);

        if (nDot < 0)
        {
            delete[] m_pPackedFiles;
            m_pPackedFiles = NULL;
            delete[] m_pKeyTable;
            m_pKeyTable = NULL;
            EXOFREE(pFileTable, "exoresfile.cpp", 1119);
            delete pFile;
            return 0;
        }

#ifdef _XBOX
        CExoString sCache("CACHE:");

        if (sFileName.SubString(0, 4).LowerCase() == "live")
        {
            sCache.Format("CACHE:%s\\", sFileName.SubString(0, 5));
        }

        m_pPackedFiles[i]->m_sFileName = sCache + sFileName.SubString(0, nDot);

        if (!LocateBifFile(m_pPackedFiles[i]->m_sFileName))
        {
            m_pPackedFiles[i]->m_sFileName =
                sDrive + ":" + sFileName.SubString(0, nDot);
        }

        if (!m_pPackedFiles[i]->LoadHeader(0))
        {
            if (sFileName.SubString(0, 4).LowerCase() != "live")
            {
                EXOFREE(pFileTable, __FILE__, __LINE__);
                delete pFile;
                return 0;
            }
        }
#else
        m_pPackedFiles[i]->m_sFileName =
            sDrive + ":" + sFileName.SubString(0, nDot);

        if (m_pPackedFiles[i]->m_nResourceType != 1 &&
            !LocateBifFile(m_pPackedFiles[i]->m_sFileName))
        {
            sCD = "CD0:";
            m_pPackedFiles[i]->m_sFileName = sCD + sFileName.SubString(0, nDot);
        }

        m_pPackedFiles[i]->LoadHeader(0);
#endif
    }

    pFile->Seek(sHeader.nKeyTableOffset, 0);

    for (i = 0; i < sHeader.nKeyCount; i++)
    {
        pFile->Read(sKey.szResRef, 16, 1);
        pFile->Read(&sKey.nType, 2, 1);
        pFile->Read(&sKey.nId, 4, 1);
        resRef = sKey.szResRef;
        sKey.nId &= 0x3FFFFFFF;
        AddKey(resRef, sKey.nType, sKey.nId, bFlag, CExoString(""));
    }

    delete pFile;
    EXOFREE(pFileTable, "exoresfile.cpp", 1213);

    return 1;
}
// PC 0x00411340
int CExoKeyTable::BuildNewTable(unsigned long nTableType,
                                const CExoString& sName, unsigned long nId,
                                int bFlag)
{
    if (sName == "")
    {
        return 0;
    }

    m_sResourceName = sName;

#ifdef _XBOX

    int bResult;

    switch (nTableType)
    {
    case 1:
        m_nId = nId;
        m_nTableType = 1;
        bResult = AddKeyTableContents(bFlag);
        if (!bResult)
        {
            g_pExoResMan->ReportMissingFile(sName, 9999, 0);
        }
        return bResult;
    case 2:
        m_nId = nId;
        m_nTableType = 2;
        return AddDirectoryContents(bFlag);
    case 3:
        m_nId = nId;
        m_nTableType = 3;
        bResult = AddEncapsulatedContents(bFlag);
        if (!bResult)
        {
            g_pExoResMan->ReportMissingFile(sName, 9997, 0);
        }
        return bResult;
    case 4:
        m_nId = nId;
        m_nTableType = 4;
        bResult = AddResourceImageContents(bFlag);
        if (!bResult)
        {
            g_pExoResMan->ReportMissingFile(sName, RESTYPE_RIM, 0);
        }
        return bResult;
    }
#else
    switch (nTableType)
    {
    case 1:
        m_nId = nId;
        m_nTableType = 1;
        return AddKeyTableContents(bFlag);
    case 2:
        m_nId = nId;
        m_nTableType = 2;
        return AddDirectoryContents(bFlag);
    case 3:
        m_nId = nId;
        m_nTableType = 3;
        return AddEncapsulatedContents(bFlag);
    case 4:
        m_nId = nId;
        m_nTableType = 4;
        return AddResourceImageContents(bFlag);
    }
#endif

    return 0;
}
// PC 0x00411410
void CExoKeyTable::RebuildTable()
{
    unsigned long nIndex = 0;
    CKeyTableEntry* pKeyEntry = NULL;
    CExoKeyTable* pKeyTable = NULL;

    if (m_pKeyTable == NULL)
    {
        if (m_nTableType != 0 && m_sResourceName != "")
        {
            BuildNewTable(m_nTableType, m_sResourceName, m_nId, 1);
        }
    }
    else
    {
        CKeyTableEntry* pOldTable = m_pKeyTable;
        unsigned long nOldTableSize = m_nTableSize;
#ifndef _XBOX
        if (m_nTableType == 4)
        {
            if (m_pPackedFiles != NULL)
            {
                m_pPackedFiles[0]->CloseFile();
            }
        }
#endif
        m_nTableSize = 0;
        m_pKeyTable = NULL;
        BuildNewTable(m_nTableType, m_sResourceName, m_nId, 0);

        m_bDestroying = 1;

        for (unsigned long i = 0; i < nOldTableSize; i++)
        {
            if (pOldTable[i].m_pRes != NULL)
            {
                if (GetTableIndex(nIndex, pOldTable[i].m_resRef,
                                  pOldTable[i].m_nType))
                {
                    m_pKeyTable[nIndex].m_pRes = pOldTable[i].m_pRes;
                    m_pKeyTable[nIndex].m_pRes->m_nResId =
                        m_pKeyTable[nIndex].m_nResId;
                    pOldTable[i].m_pRes = NULL;
                    m_pKeyTable[nIndex].m_pRes->m_pKeyEntry =
                        &m_pKeyTable[nIndex];
                }
                else
                {
                    g_pExoResMan->GetKeyEntry(pOldTable[i].m_resRef,
                                              pOldTable[i].m_nType, &pKeyTable,
                                              &pKeyEntry);

                    if (pKeyEntry == NULL)
                    {
                        if (pOldTable[i].m_pRes->m_nDemandCount > 0)
                        {
                            pOldTable[i].m_pRes->m_nFlags |= 0x200;
                        }
                        else
                        {
                            pOldTable[i].m_pRes->Dump(0);
                            pOldTable[i].m_pRes->m_pKeyEntry = NULL;
                            pOldTable[i].m_pRes->m_nResId = 0xFFFFFFFF;
                        }
                    }
                    else
                    {
                        pKeyEntry->m_pRes = pOldTable[i].m_pRes;
                        pKeyEntry->m_pRes->m_nResId = pKeyEntry->m_nResId;
                        pOldTable[i].m_pRes = NULL;
                        pKeyEntry->m_pRes->m_pKeyEntry = pKeyEntry;

                        if (pKeyEntry->m_pRes->m_nDemandCount > 0)
                        {
                            pKeyEntry->m_pRes->m_nFlags |= 0x100;
                        }
                        else
                        {
                            pKeyEntry->m_pRes->Dump(0);
                        }
                    }
                }
            }
        }

        delete[] pOldTable;

        for (unsigned long j = 0; j < m_nTableSize; j++)
        {
            if (m_pKeyTable[j].m_pRes == NULL)
            {
                g_pExoResMan->GetKeyEntry(m_pKeyTable[j].m_resRef,
                                          m_pKeyTable[j].m_nType, &pKeyTable,
                                          &pKeyEntry);

                if (pKeyEntry != NULL)
                {
                    CRes* pRes = pKeyEntry->m_pRes;

                    if (pRes != NULL && pKeyTable->m_nId < m_nId)
                    {
                        if (pRes->m_nDemandCount > 0)
                        {
                            pRes->m_nFlags |= 0x100;
                        }
                        else
                        {
                            pRes->Dump(0);
                        }

                        m_pKeyTable[j].m_pRes = pKeyEntry->m_pRes;
                        m_pKeyTable[j].m_pRes->m_nResId =
                            m_pKeyTable[j].m_nResId;
                        pKeyEntry->m_pRes = NULL;
                        m_pKeyTable[j].m_pRes->m_pKeyEntry = &m_pKeyTable[j];
                    }
                }
            }
        }

        m_bDestroying = 0;
    }
}
// PC 0x004015A0
void CExoResourceImageFile::AddAsyncRefCount()
{
}
// PC 0x0065D0F0
int CExoResourceImageFile::CloseAsyncFile()
{
    return 1;
}
// PC 0x004015A0
void CExoResourceImageFile::DeleteAsyncRefCount()
{
}
// PC 0x0065D0F0
int CExoResourceImageFile::OpenAsyncFile()
{
    return 1;
}
// PC 0x006955A0
int CExoResourceImageFile::LoadHeader(unsigned char)
{
    return 0;
}
// PC 0x0065D0F0
int CExoResourceImageFile::UnloadHeader()
{
    return 1;
}

static inline void ExoresfileTrace2()
{
    ExoresfileTrace1();
}

static inline void ExoresfileTrace1()
{
}
