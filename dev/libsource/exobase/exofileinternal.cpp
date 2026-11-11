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

CExoString g_sErrorString;
// PC 0x005EB570  Xbox 0x001385B0
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
// PC 0x005EB5B0  Xbox 0x001385F0
CExoAliasListInternal::CExoAliasListInternal()
{
    m_pAliasList = NULL;
    m_sDefault = "";
}
// PC 0x005EB600  Xbox 0x00138640
inline CExoAliasListInternal::CExoAlias::CExoAlias()
{
}
// PC 0x005EB650  Xbox 0x00138690
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
// PC 0x005EB6B0  Xbox 0x001386F0
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
// PC 0x005EB840  Xbox 0x00138880
CExoFileInternal::CExoFileInternal(const CExoString& path,
                                   unsigned short resourceType,
                                   const CExoString& mode)
{
#ifdef _XBOX
    CExoString sFileName;

    m_pFile = NULL;
    sFileName =
        g_pExoBase->m_pExoAliasList->ResolveFileName(path, resourceType);
    m_pFile = fopen(sFileName.CStr(), mode.CStr());
#else
    m_pFile = NULL;
    m_sMode = mode;
    m_sFileName =
        g_pExoBase->m_pExoAliasList->ResolveFileName(path, resourceType);
    m_pFile = fopen(m_sFileName.CStr(), mode.CStr());
#endif
}
// PC 0x005EB900
CExoFileInternal::~CExoFileInternal()
{
    if (m_pFile)
    {
        fclose(m_pFile);
        m_pFile = NULL;
    }
}
// PC 0x005EB970  Xbox 0x00138950
unsigned long CExoFileInternal::GetOffset()
{
    if (m_pFile != NULL)
    {
        return ftell(m_pFile);
    }
    return 0;
}
// PC 0x005EB990  Xbox 0x00138970
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
// PC 0x005EB9F0  Xbox 0x001389D0
unsigned long CExoFileInternal::Read(CExoString* text, unsigned long count)
{
    if (m_pFile != NULL)
    {
        char* pBuffer = new char[count + 1];
        unsigned long nBytesRead = fread(pBuffer, 1, count, m_pFile);
        pBuffer[nBytesRead] = 0;
        *text = pBuffer;
        operator delete(pBuffer);
        return nBytesRead;
    }
    return 0;
}
// PC 0x005EBA40  Xbox 0x00138A20
unsigned long CExoFileInternal::Read(void* data, unsigned long size,
                                     unsigned long count)
{
#ifdef _XBOX
    if (m_pFile != NULL)
    {
        return size * fread(data, size, count, m_pFile);
    }
    return 0;
#else
    int nAttempt = 0;
    if (m_pFile != NULL)
    {
        int bRecovered = 0;
        long lOffset = ftell(m_pFile);
        unsigned long nBytesRead = size * fread(data, size, count, m_pFile);
        unsigned long nExpected = size * count;
        while (nBytesRead != count * size && ferror(m_pFile) && nAttempt < 10)
        {
            bRecovered = 1;
            g_sErrorString.Format(
                "%sFAILED READ (%s) - ATTEMPT %d - Asked for %d "
                "bytes got %d bytes\n",
                m_sFileName.CStr(), g_sErrorString.CStr(), nAttempt, nExpected,
                nBytesRead);
            Sleep(500);
            if (m_pFile != NULL)
            {
                fclose(m_pFile);
            }
            else
            {
                g_sErrorString.Format("%sFile handle was null\n",
                                      g_sErrorString.CStr(), nAttempt);
            }
            m_pFile = fopen(m_sFileName.CStr(), m_sMode.CStr());
            if (m_pFile != NULL)
            {
                fseek(m_pFile, lOffset, SEEK_SET);
                nBytesRead = size * fread(data, size, count, m_pFile);
            }
            else
            {
                g_sErrorString.Format("%sFile handle failed to reopen\n",
                                      g_sErrorString.CStr(), nAttempt);
            }
            ++nAttempt;
        }
        if (nBytesRead != nExpected)
        {
            g_sErrorString.Format("%sFile write failed.  Giving up\n",
                                  g_sErrorString.CStr());
        }
        else if (bRecovered)
        {
            g_sErrorString.Format("%sFile write recovered.\n",
                                  g_sErrorString.CStr());
        }
        return nBytesRead;
    }
    return 0;
#endif
}
// PC 0x005EBBF0  Xbox 0x00138A50
void CExoFileInternal::ReadAsync(void* data, unsigned long size,
                                 unsigned long count)
{
    g_AsyncFileThread.Read(data, size, count, m_pFile);
}
// PC 0x005EBC50  Xbox 0x00138F30
int CExoFileInternal::ReadAsyncComplete()
{
    return g_AsyncFileThread.m_bComplete;
}

#ifdef _XBOX
// Xbox 0x00138AB0
unsigned long CExoFileInternal::Write(const char* text)
{
    if (m_pFile != NULL)
    {
        return fwrite(text, 1, strlen(text), m_pFile);
    }
    return 0;
}
#else
unsigned long CExoFileInternal::Write(const void* data, unsigned long size,  // PC 0x005EBC60
                                      unsigned long count)
{
    int nAttempt = 0;
    if (m_pFile != NULL)
    {
        long lOffset;
        int bRecovered = 0;
        lOffset = ftell(m_pFile);
        unsigned long nBytesWritten = size * fwrite(data, size, count, m_pFile);
        unsigned long nExpected = size * count;
        while (nBytesWritten != count * size && ferror(m_pFile) &&
               nAttempt < 10)
        {
            bRecovered = 1;
            g_sErrorString.Format(
                "%sFAILED WRITE (%s) - ATTEMPT %d - Tried to write %d bytes at "
                "position %d, actually wrote %d bytes\n",
                m_sFileName.CStr(), g_sErrorString.CStr(), nAttempt, nExpected,
                lOffset, nBytesWritten);
            Sleep(500);
            if (m_pFile != NULL)
            {
                fclose(m_pFile);
            }
            else
            {
                g_sErrorString.Format("%sFile handle was null\n",
                                      g_sErrorString.CStr());
            }
            m_pFile = fopen(m_sFileName.CStr(), m_sMode.CStr());
            if (m_pFile != NULL)
            {
                fseek(m_pFile, lOffset, SEEK_SET);
                nBytesWritten = size * fwrite(data, size, count, m_pFile);
            }
            else
            {
                g_sErrorString.Format("%sFile handle failed to reopen\n",
                                      g_sErrorString.CStr());
            }
            ++nAttempt;
        }
        if (nBytesWritten != nExpected)
        {
            g_sErrorString.Format("%sFile write failed.  Giving up\n",
                                  g_sErrorString.CStr());
        }
        else if (bRecovered)
        {
            g_sErrorString.Format("%sFile write recovered.\n",
                                  g_sErrorString.CStr());
        }
        return nBytesWritten;
    }
    return 0;
}
#endif
// PC 0x005EBE10  Xbox 0x00138AF0
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
// PC 0x005EBED0  Xbox 0x00138BB0
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
// PC 0x005EBF30  Xbox 0x00138C10
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
// Xbox 0x00138DC0
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
// PC 0x005EC0E0  Xbox 0x00138F50
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
// Xbox 0x00138F40
unsigned long CExoFileInternal::ReadAsyncBytesRead()
{
    return g_AsyncFileThread.m_nBytesRead;
}
#endif
