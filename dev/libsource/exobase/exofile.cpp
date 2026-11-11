#include <string.h>
#include "exobase.h"
#include "exofileinternal.h"
// PC 0x005E6810  Xbox 0x00134990
CExoAliasList::CExoAliasList()
{
    m_pInternal = new CExoAliasListInternal;
}
// PC 0x005E6880  Xbox 0x00134A00
void CExoAliasList::Add(const CExoString& sAlias, const CExoString& sPath)
{
    m_pInternal->Add(sAlias, sPath);
}

#ifdef _XBOX
// Xbox 0x00134A10
void CExoAliasList::ChangePath(const CExoString& sAlias, const CExoString& sPath)
{
    m_pInternal->ChangePath(sAlias, sPath);
}
#endif
// PC 0x005E6890  Xbox 0x00134A20
const CExoString& CExoAliasList::GetAliasPath(const CExoString& sAlias,
                                              int nLength) const
{
    return m_pInternal->GetAliasPath(sAlias, nLength);
}
// PC 0x005E68A0  Xbox 0x00134A30
CExoString CExoAliasList::ResolveFileName(const CExoString& sAlias,
                                          unsigned short nResType) const
{
    return m_pInternal->ResolveFileName(sAlias, nResType);
}
// PC 0x005E68D0  Xbox 0x00134A60
CExoFile::CExoFile(const CExoString& path, unsigned short resourceType,
                   const CExoString& mode)
{
    m_pInternal = new CExoFileInternal(path, resourceType, mode);
}
// PC 0x005E6940  Xbox 0x00134AD0
unsigned long CExoFile::GetOffset()
{
    return m_pInternal->GetOffset();
}
// PC 0x005E6950  Xbox 0x00134AE0
int CExoFile::GetSize()
{
    return m_pInternal->GetSize();
}
// PC 0x005E6960  Xbox 0x00134AF0
unsigned long CExoFile::Read(void* data, unsigned long size,
                             unsigned long count)
{
    return m_pInternal->Read(data, size, count);
}
// PC 0x005E6970  Xbox 0x00134B00
unsigned long CExoFile::Read(CExoString& text, unsigned long count)
{
    return m_pInternal->Read(&text, count);
}
// PC 0x005E6980  Xbox 0x00134B10
void CExoFile::ReadAsync(void* data, unsigned long size, unsigned long count)
{
    m_pInternal->ReadAsync(data, size, count);
}
// PC 0x005E6990  Xbox 0x00134B20
int CExoFile::ReadAsyncComplete()
{
    return m_pInternal->ReadAsyncComplete();
}
// PC 0x005E69A0  Xbox 0x00134BE0
unsigned long CExoFile::Write(const void* data, unsigned long size,
                              unsigned long count)
{
    return m_pInternal->Write(data, size, count);
}
// PC 0x005E69B0  Xbox 0x00134B50
CExoAliasList::~CExoAliasList()
{
    if (m_pInternal != NULL)
    {
        delete m_pInternal;
        m_pInternal = NULL;
    }
}
// PC 0x005E69E0  Xbox 0x00134B80
CExoFile::~CExoFile()
{
    if (m_pInternal)
    {
        delete m_pInternal;
        m_pInternal = 0;
    }
}
// PC 0x005E6A10  Xbox 0x00134BB0
int CExoFile::FileOpened()
{
    return m_pInternal->FileOpened();
}
// PC 0x005E6A20  Xbox 0x00134BC0
int CExoFile::Flush()
{
    return m_pInternal->Flush();
}
// PC 0x005E6A40  Xbox 0x00134B40
unsigned long CExoFile::Write(const char* text)
{
    return m_pInternal->Write(text);
}
// PC 0x005E6A70  Xbox 0x00134C10
unsigned long CExoFile::Write(const CExoString& text)
{
    return m_pInternal->Write(text);
}
// PC 0x005E6AA0  Xbox 0x00134C50
int CExoFile::Seek(int offset, int origin)
{
    return m_pInternal->Seek(offset, origin);
}
// PC 0x005E6AD0  Xbox 0x00134C80
int CExoFile::SeekBeginning()
{
    return m_pInternal->SeekBeginning();
}
// PC 0x005E6AF0  Xbox 0x00134CA0
int CExoFile::SeekEnd()
{
    return m_pInternal->SeekEnd();
}

#ifndef _XBOX
inline unsigned long CExoFileInternal::Write(const char* text)  // Xbox 0x00138AB0
{
    return Write(text, 1, strlen(text));
}
#endif

inline unsigned long CExoFileInternal::Write(const CExoString& text)
{
#ifdef _XBOX
    if (m_pFile != NULL)
    {
        return fwrite(text.CStr(), 1, text.GetLength(), m_pFile);
    }
    return 0;
#else
    return Write(text.CStr(), 1, text.GetLength());
#endif
}

inline int CExoFileInternal::FileOpened()
{
    return m_pFile != 0;
}

inline int CExoFileInternal::Flush()
{
    return m_pFile ? (fflush(m_pFile) == 0) : 0;
}

inline int CExoFileInternal::Seek(int offset, int origin)
{
    return m_pFile ? (fseek(m_pFile, offset, origin) == 0) : 0;
}

inline int CExoFileInternal::SeekBeginning()
{
    return m_pFile ? (fseek(m_pFile, 0, 0) == 0) : 0;
}

inline int CExoFileInternal::SeekEnd()
{
    return m_pFile ? (fseek(m_pFile, 0, 2) == 0) : 0;
}

#ifdef _XBOX
// Xbox 0x00134B30
unsigned long CExoFile::ReadAsyncBytesRead()
{
    return m_pInternal->ReadAsyncBytesRead();
}
#endif
