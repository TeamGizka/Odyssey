#include "exofileinternal.h"
#include "exobase.h"

#include <stdio.h>
#if defined(_WIN32) && !defined(_XBOX)
#include <windows.h>
#else
#include "platform.h"
#endif

DWORD WINAPI AsyncReadThreadProc(LPVOID lpParameter);

static CExoFileThread g_AsyncFileThread;
// PC 0x00636F60 GOG 0x00636E60
CExoFileThread::~CExoFileThread()
{
    m_bStop = 1;
    ResumeThread(m_hThread);
    if (m_hThread != (HANDLE)-1)
    {
        WaitForSingleObject(m_hThread, INFINITE);
        CloseHandle(m_hThread);
    }
}
// PC 0x00636FA0 GOG 0x00636EA0
CExoAliasListInternal::CExoAliasListInternal()
{
    m_pAliasList = NULL;
    m_sDefault = "";
}

// PC 0x006373D0 GOG 0x006372D0
unsigned long CExoFileInternal::Read(CExoString* text, unsigned long count)
{
    if (m_pFile != NULL)
    {
        char* pBuffer = new char[count + 1];
        unsigned long nBytesRead = fread(pBuffer, 1, count, m_pFile);
        if (nBytesRead != count)
        {
            if (ferror(m_pFile))
            {
                EXOLOG("CExoFile: read error in %s", m_sFileName.CStr());
                clearerr(m_pFile);
                return 0;
            }
            if (feof(m_pFile))
            {
                EXOLOG("CExoFile: unexpected end of %s", m_sFileName.CStr());
                clearerr(m_pFile);
                return 0;
            }
        }
        pBuffer[nBytesRead] = 0;
        *text = pBuffer;
        operator delete(pBuffer);
        return nBytesRead;
    }
    return 0;
}
// PC 0x00636FF0 GOG 0x00636EF0
inline CExoAliasListInternal::CExoAlias::CExoAlias()
{
}
// PC 0x00637040 GOG 0x00636F40
const CExoString& CExoAliasListInternal::GetAliasPath(const CExoString& sAlias,
                                                      int nLength) const
{
    if (sAlias == "")
    {
        return m_sDefault;
    }

    if (nLength == 0)
    {
        nLength = sAlias.GetLength();
    }

    int bFound = 0;
    CExoAlias* pAlias = m_pAliasList;
    while (!bFound && pAlias != NULL)
    {
        if (pAlias->m_sAlias.ComparePrefixNoCase(sAlias, nLength) == TRUE)
        {
            bFound = 1;
        }
        else
        {
            pAlias = pAlias->m_pNext;
        }
    }

    if (bFound)
    {
        return pAlias->m_sPath;
    }
    return m_sDefault;
}
// PC 0x006370B0 GOG 0x00636FB0
CExoString CExoAliasListInternal::ResolveFileName(const CExoString& sName,
                                                  unsigned short nResType) const
{
    CExoString sResult;
    int nPos = sName.Find(':', 0);

    if (nPos <= 0)
    {
        return "";
    }
    if (g_pExoBase->m_pExoAliasList->GetAliasPath(sName, nPos).GetLength() == 0)
    {
        return "";
    }

    {
        int nAliasLength = nPos;
        if (sName.CStr()[nPos + 1] == '\\')
        {
            nPos++;
        }
        if (g_pExoBase->GetResourceExtension(nResType).GetLength() != 0)
        {
            sResult.Format(
                "%s%s.%s",
                g_pExoBase->m_pExoAliasList->GetAliasPath(sName, nAliasLength)
                    .CStr(),
                sName.CStr() + nPos + 1,
                g_pExoBase->GetResourceExtension(nResType).CStr());
        }
        else
        {
            sResult.Format(
                "%s%s",
                g_pExoBase->m_pExoAliasList->GetAliasPath(sName, nAliasLength)
                    .CStr(),
                sName.CStr() + nPos + 1);
        }
        return sResult;
    }
}
// PC 0x00637250 GOG 0x00637150
CExoFileInternal::CExoFileInternal(const CExoString& path,
                                   unsigned short resourceType,
                                   const CExoString& mode)
{
    CExoString sFileName;

    m_pFile = NULL;
    sFileName =
        g_pExoBase->m_pExoAliasList->ResolveFileName(path, resourceType);
    m_pFile = fopen(sFileName.CStr(), mode.CStr());
    m_sFileName = sFileName;
}
// PC 0x00637320 GOG 0x00637220
CExoFileInternal::~CExoFileInternal()
{
    if (m_pFile)
    {
        fclose(m_pFile);
        m_pFile = NULL;
    }
}
// PC 0x00637350 GOG 0x00637250
unsigned long CExoFileInternal::GetOffset()
{
    if (m_pFile != NULL)
    {
        return ftell(m_pFile);
    }
    return 0;
}
// PC 0x00637370 GOG 0x00637270
unsigned long CExoFileInternal::GetSize()
{
    if (m_pFile != NULL)
    {
        fflush(m_pFile);
        long lOffset = ftell(m_pFile);
        if (lOffset >= 0)
        {
            fseek(m_pFile, 0, SEEK_END);
            unsigned long nSize = ftell(m_pFile);
            fseek(m_pFile, lOffset, SEEK_SET);
            return nSize;
        }
    }
    return (unsigned long)-1;
}
// PC 0x00637470 GOG 0x00637370
unsigned long CExoFileInternal::Read(void* data, unsigned long size,
                                     unsigned long count)
{
    if (m_pFile != NULL)
    {
        unsigned long nRead = fread(data, size, count, m_pFile);
        if (nRead != count)
        {
            if (ferror(m_pFile))
            {
                EXOLOG("CExoFile: read error in %s", m_sFileName.CStr());
                clearerr(m_pFile);
                return 0;
            }
            if (feof(m_pFile))
            {
                EXOLOG("CExoFile: unexpected end of %s", m_sFileName.CStr());
                clearerr(m_pFile);
                return 0;
            }
        }
        return size * nRead;
    }
    return 0;
}
// PC 0x006374F0 GOG 0x006373F0
void CExoFileInternal::ReadAsync(void* data, unsigned long size,
                                 unsigned long count)
{
    g_AsyncFileThread.Read(data, size, count, m_pFile);
}
// PC 0x00637550 GOG 0x00637450
int CExoFileInternal::ReadAsyncComplete()
{
    return g_AsyncFileThread.m_bComplete;
}

// PC 0x00637560 GOG 0x00637460
unsigned long CExoFileInternal::Write(const char* text)
{
    if (m_pFile != NULL)
    {
        return fwrite(text, 1, strlen(text), m_pFile);
    }
    return 0;
}
// PC 0x006375A0 GOG 0x006374A0
DWORD WINAPI AsyncReadThreadProc(LPVOID lpParameter)
{
    while (!g_AsyncFileThread.m_bStop)
    {
        g_AsyncFileThread.m_csAsyncRead.EnterCriticalSection();
        g_AsyncFileThread.AsyncRead();
        g_AsyncFileThread.m_csAsyncRead.LeaveCriticalSection();
        SuspendThread(g_AsyncFileThread.m_hThread);
    }
    ExitThread(1);
}
// PC 0x00637660 GOG 0x00637560
CExoFileThread::CExoFileThread()
{
    m_bComplete = 0;
    m_bStop = 0;
    m_pFile = NULL;
    m_nBytesRead = 0;
    m_nCount = 0;
    m_nSize = 0;
    m_pBuffer = NULL;
    unsigned long nParameter = 0;
    m_hThread = CreateThread(NULL, 0x10000, AsyncReadThreadProc, &nParameter,
                             CREATE_SUSPENDED, &m_nThreadId);
    SetThreadPriority(m_hThread, THREAD_PRIORITY_LOWEST);
}
// PC 0x006376C0 GOG 0x006375C0
void CExoAliasListInternal::Add(const CExoString& sAlias,
                                const CExoString& sPath)
{
    CExoString sNewAlias = sAlias;
    CExoString sNewPath = sPath;

    if (sNewPath == "")
    {
        return;
    }

    sNewAlias = sNewAlias.UpperCase();
    const char* pPath = sNewPath.CStr();
    if (pPath[sNewPath.GetLength() - 1] != '\\')
    {
        sNewPath = sNewPath + "\\";
    }

    int bFound = 0;
    CExoAlias* pPrev = NULL;
    CExoAlias* pAlias = m_pAliasList;
    while (!bFound && pAlias != NULL)
    {
        if (pAlias->m_sAlias > sNewAlias)
        {
            bFound = 1;
        }
        else
        {
            pPrev = pAlias;
            pAlias = pAlias->m_pNext;
        }
    }

    CExoAlias* pNew = new CExoAlias;
    pNew->m_sAlias = sNewAlias;
    pNew->m_sPath = sNewPath;
    if (m_pAliasList == pAlias)
    {
        m_pAliasList = pNew;
    }
    pNew->m_pPrev = pPrev;
    pNew->m_pNext = pAlias;
    if (pPrev != NULL)
    {
        pPrev->m_pNext = pNew;
    }
    if (pAlias != NULL)
    {
        pAlias->m_pPrev = pNew;
    }
}

#ifdef _XBOX

void CExoAliasListInternal::ChangePath(const CExoString& sAlias,
                                   const CExoString& sPath)
{
    CExoString sNewAlias = sAlias;
    CExoString sNewPath = sPath;

    if (!(sNewPath == ""))
    {
        sNewAlias = sNewAlias.UpperCase();
        const char* pPath = sNewPath.CStr();
        if (pPath[sNewPath.GetLength() - 1] != '\\')
        {
            sNewPath = sNewPath + "\\";
        }

        int bFound = 0;
        CExoAlias* pAlias = m_pAliasList;
        while (!bFound && pAlias != NULL)
        {
            if (pAlias->m_sAlias == sNewAlias)
            {
                pAlias->m_sPath = sNewPath;
                bFound = 1;
            }
            else
            {
                pAlias = pAlias->m_pNext;
            }
        }
        if (!bFound)
        {
            Add(sNewAlias, sNewPath);
        }
    }
}
#endif
// PC 0x00637880 GOG 0x00637780
CExoAliasListInternal::~CExoAliasListInternal()
{
    CExoAlias* pAlias = m_pAliasList;
    while (pAlias != NULL)
    {
        CExoAlias* pNext = pAlias->m_pNext;
        delete pAlias;
        pAlias = pNext;
    }
}

#ifdef _XBOX

unsigned long CExoFileInternal::ReadAsyncBytesRead()
{
    return g_AsyncFileThread.m_nBytesRead;
}
#endif
