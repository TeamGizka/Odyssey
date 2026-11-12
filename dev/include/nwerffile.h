#ifndef __NWERFFILE_H__
#define __NWERFFILE_H__

class CExoFile;
class CExoLinkedListInternal;
class CRes;
#include "exobase.h"
#include "exoresman.h"
#include <string.h>

class CResGFF;

class CERFString
{
public:
    CERFString()
    {
        m_nLanguageId = 0;
        m_nLength = 0;
        m_pText = NULL;
    }

    ~CERFString()
    {
        if (m_pText != NULL)
        {
            delete m_pText;
            m_pText = NULL;
        }

        m_nLanguageId = 0;
        m_nLength = 0;
    }

    int Read(CExoFile& file);
    int Write(CExoFile& file);
    int SetText(CExoString& sText);

    unsigned long m_nLanguageId;
    unsigned long m_nLength;
    char* m_pText;
};

class CERFKey
{
public:
    CERFKey()
    {
        memset(m_sResRef, 0, 16);
        memset(&m_nUnused, 0, 2);
        m_nResId = 0;
        m_nResType = -1;
    }

    ~CERFKey();

    int Read(CExoFile& file);
    int Write(CExoFile& file);

    void SetName(CExoString& sName);

    char m_sResRef[16];
    unsigned long m_nResId;
    unsigned short m_nResType;
    unsigned short m_nUnused;
};

class CERFRes
{
public:
    CERFRes()
    {
        m_nOffset = 0;
        m_nSize = 0;
        m_pData = NULL;
    }

    ~CERFRes();

    int Read(CExoFile& file);
    int CopyToFile(CExoFile* pIn, CExoFile* pOut, void* pBuffer);
    int Write(CExoFile& file, unsigned long nTableOffset);

    unsigned long m_nOffset;
    unsigned long m_nSize;
    void* m_pData;
};

class CERFFile
{
public:
    CERFFile();
    ~CERFFile();

    void SetVersion(char* sVersion);
#ifdef _XBOX
    int Read(const CExoString& sFileName, void* pSignature = NULL);
#else
    int Read(const CExoString& sFileName);
#endif
    int Create(CExoString& sFileName);
    int WriteHeader();
    int Finish();
#ifdef _XBOX
    int ReadHeaderVariance(const CExoString& sFileName,
                           void* pSignature = NULL);
#else
    int ReadHeaderVariance(const CExoString& sFileName);
#endif
    int WriteStringTable();
    void ExportFilesFromERF(const CExoString& sFileName,
                           const CExoString& sDirectory, void* pUnused,
                           int (*pfnProgress)(int, int, int, int), int nArg1,
                           int nArg2);
    int SetNumEntries(unsigned long nEntries);
    int WriteResource(const CExoString& sFileName, void* pUnused);
    int WriteResource(char* sResRef, unsigned short nResType, CRes* pRes,
                      int bIsGFF, void* pUnused);
    void ImportFiles(const CExoString& sDirectory, void* pUnused,
                     int (*pfnProgress)(int, int, int, int), int nArg1,
                     int nArg2);
    int Reset();

    char m_sFileType[4];
    char m_sVersion[4];
    unsigned long m_nLanguageCount;
    unsigned long m_nLocStringSize;
    unsigned long m_nEntryCount;
    unsigned long m_nOffsetToLocStrings;
    unsigned long m_nOffsetToKeyList;
    unsigned long m_nOffsetToResourceList;
    unsigned long m_nBuildYear;
    unsigned long m_nBuildDay;
    unsigned long m_nDescriptionStrRef;
    char m_aReserved[116];
    CExoLinkedList<CERFString> m_lstStrings;
    CExoLinkedList<CERFKey> m_lstKeys;
    CExoLinkedList<CERFRes> m_lstResources;
    CExoFile* m_pFile;
    unsigned long m_nStringTablePos;
    unsigned long m_nKeyListPos;
    unsigned long m_nResourceListPos;
    unsigned long m_nNumEntries;
    unsigned long m_nCurEntry;
    unsigned long m_nFileSize;
    CExoString m_sFileName;
};

#endif
