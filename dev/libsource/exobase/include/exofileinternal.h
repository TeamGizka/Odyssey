#ifndef EXOFILEINTERNAL_H
#define EXOFILEINTERNAL_H

#include "exobase.h"

#include <stdio.h>
#if defined(_WIN32) && !defined(_XBOX)
#include <windows.h>
#else
#include "platform.h"
#endif

class CExoFileInternal
{
public:
    CExoFileInternal(const CExoString& path, unsigned short resourceType,
                     const CExoString& mode);
    ~CExoFileInternal();
    unsigned long GetOffset();
    unsigned long GetSize();
    unsigned long Read(void* data, unsigned long size, unsigned long count);
    unsigned long Read(CExoString* text, unsigned long count);
    void ReadAsync(void* data, unsigned long size, unsigned long count);
    int ReadAsyncComplete();
#ifdef _XBOX
    unsigned long ReadAsyncBytesRead();
#endif
    unsigned long Write(const void* data, unsigned long size,
                        unsigned long count);

    unsigned long Write(const char* text);
    unsigned long Write(const CExoString& text);
    int FileOpened();
    int Flush();
    int Seek(int offset, int origin);
    int SeekBeginning();
    int SeekEnd();

    FILE* m_pFile;
    CExoString m_sFileName;
};

inline unsigned long CExoFileInternal::Write(const void* data,
                                             unsigned long size,
                                             unsigned long count)
{
    if (m_pFile != NULL)
    {
        return size * fwrite(data, size, count, m_pFile);
    }
    return 0;
}


class CExoAliasListInternal
{
public:
    class CExoAlias
    {
    public:
        CExoAlias();

        CExoString m_sAlias;
        CExoString m_sPath;
        CExoAlias* m_pNext;
        CExoAlias* m_pPrev;
    };

    CExoAliasListInternal();
    ~CExoAliasListInternal();
    void Add(const CExoString& sAlias, const CExoString& sPath);
#ifdef _XBOX
    void ChangePath(const CExoString& sAlias, const CExoString& sPath);
#endif
    const CExoString& GetAliasPath(const CExoString& sAlias, int nLength) const;
    CExoString ResolveFileName(const CExoString& sName,
                               unsigned short nResType) const;

    CExoString m_sDefault;
    CExoAlias* m_pAliasList;
};

class CExoFileThread
{
public:
    CExoFileThread();
    ~CExoFileThread();

    void AsyncRead()
    {
        m_csAsyncRead.EnterCriticalSection();
        m_nBytesRead = 0;
        if (m_pFile != NULL)
        {
            m_nBytesRead =
                m_nSize * fread(m_pBuffer, m_nSize, m_nCount, m_pFile);
        }
        m_bComplete = 1;
        m_pFile = NULL;
        m_nCount = 0;
        m_nSize = 0;
        m_pBuffer = NULL;
        m_csAsyncRead.LeaveCriticalSection();
    }

    void Read(void* pBuffer, unsigned long nSize, unsigned long nCount,
              FILE* pFile)
    {
        m_csAsyncRead.EnterCriticalSection();
        m_bComplete = 0;
        m_pFile = pFile;
        m_pBuffer = pBuffer;
        m_nSize = nSize;
        m_nCount = nCount;
        ResumeThread(m_hThread);
        m_csAsyncRead.LeaveCriticalSection();
    }

    HANDLE m_hThread;
    unsigned long m_nThreadId;
    CExoCriticalSection m_csAsyncRead;
    unsigned long m_bComplete;
    unsigned long m_bStop;
    FILE* m_pFile;
    unsigned long m_nSize;
    unsigned long m_nCount;
    unsigned long m_nBytesRead;
    void* m_pBuffer;
};

#endif
