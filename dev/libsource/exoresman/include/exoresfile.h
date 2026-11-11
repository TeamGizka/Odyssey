#ifndef __EXORESFILE_H__
#define __EXORESFILE_H__

#include "exobase.h"

class CExoPackedFile
{
public:
    CExoPackedFile()
    {
        m_bFileOpened = 0;
        m_bHeaderLoaded = 0;
    }

    virtual ~CExoPackedFile();

    virtual void AddRefCount() = 0;

    virtual void AddAsyncRefCount() = 0;

    virtual int CloseFile() = 0;
    virtual int CloseAsyncFile() = 0;
    virtual void DeleteRefCount() = 0;
    virtual void DeleteAsyncRefCount() = 0;

    virtual CExoFile* GetFile();
    virtual CExoFile* GetAsyncFile();

    virtual unsigned long GetResourceSize(unsigned long nId) = 0;
    virtual int Initialize() = 0;
    virtual int OpenFile() = 0;
    virtual int OpenAsyncFile() = 0;

    virtual unsigned long ReadResource(unsigned long nId, void* pBuffer,
                                       unsigned long nSize,
                                       unsigned long nOffset) = 0;
    virtual void ReadResourceAsync(unsigned long nId, void* pBuffer,
                                   unsigned long nSize,
                                   unsigned long nOffset) = 0;

    virtual int LoadHeader(unsigned char nType) = 0;
    virtual int UnloadHeader() = 0;

    CExoString m_sFileName;
    unsigned short m_nResourceType;
    unsigned long m_nFileSize;
    CExoFile* m_pFile;
    CExoFile* m_pAsyncFile;
    int m_nRefCount;
    int m_nAsyncRefCount;
    int m_bFileOpened;
    int m_bAsyncFileOpened;
    int m_bHeaderLoaded;
};

struct CExoResFileHeader
{
    char szFileType[4];
    char szVersion[4];
    unsigned long nVariableResourceCount;
    unsigned long nFixedResourceCount;
    unsigned long nVariableTableOffset;
};

struct CExoResFileEntry
{
    unsigned long nId;
    unsigned long nOffset;
    unsigned long nSize;
    unsigned long nType;
};

class CExoResFile : public CExoPackedFile
{
public:
    CExoResFile();

    virtual ~CExoResFile();

    virtual void AddRefCount();
    virtual void AddAsyncRefCount();
    virtual int CloseFile();
    virtual int CloseAsyncFile();
    virtual void DeleteRefCount();
    virtual void DeleteAsyncRefCount();

    virtual unsigned long GetResourceSize(unsigned long nId);
    virtual int Initialize();
    virtual int OpenFile();
    virtual int OpenAsyncFile();

    virtual unsigned long ReadResource(unsigned long nId, void* pBuffer,
                                       unsigned long nSize,
                                       unsigned long nOffset);
    virtual void ReadResourceAsync(unsigned long nId, void* pBuffer,
                                   unsigned long nSize, unsigned long nOffset);

    virtual int LoadHeader(unsigned char nType);
    virtual int UnloadHeader();

    CExoResFileHeader* m_pHeader;
    CExoResFileEntry* m_pResourceTable;
};

#endif
