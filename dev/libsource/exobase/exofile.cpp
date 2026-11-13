#include <string.h>
#include "exobase.h"
#include "exofileinternal.h"
// PC 0x00632140 GOG 0x00632030
CExoAliasList::CExoAliasList()
{
    m_pInternal = new CExoAliasListInternal;
}
// PC 0x006321B0 GOG 0x006320A0
void CExoAliasList::Add(const CExoString& sAlias, const CExoString& sPath)
{
    m_pInternal->Add(sAlias, sPath);
}

#ifdef _XBOX

void CExoAliasList::ChangePath(const CExoString& sAlias, const CExoString& sPath)
{
    m_pInternal->ChangePath(sAlias, sPath);
}
#endif
// PC 0x006321C0 GOG 0x006320B0
const CExoString& CExoAliasList::GetAliasPath(const CExoString& sAlias,
                                              int nLength) const
{
    return m_pInternal->GetAliasPath(sAlias, nLength);
}
// PC 0x006321D0 GOG 0x006320C0
CExoString CExoAliasList::ResolveFileName(const CExoString& sAlias,
                                          unsigned short nResType) const
{
    return m_pInternal->ResolveFileName(sAlias, nResType);
}
// PC 0x00632200 GOG 0x006320F0
CExoFile::CExoFile(const CExoString& path, unsigned short resourceType,
                   const CExoString& mode)
{
    m_pInternal = new CExoFileInternal(path, resourceType, mode);
}
// PC 0x00632270 GOG 0x00632160
unsigned long CExoFile::GetOffset()
{
    return m_pInternal->GetOffset();
}
// PC 0x00632280 GOG 0x00632170
int CExoFile::GetSize()
{
    return m_pInternal->GetSize();
}
// PC 0x00632290 GOG 0x00632180
unsigned long CExoFile::Read(void* data, unsigned long size,
                             unsigned long count)
{
    return m_pInternal->Read(data, size, count);
}
// PC 0x006322A0 GOG 0x00632190
unsigned long CExoFile::Read(CExoString& text, unsigned long count)
{
    return m_pInternal->Read(&text, count);
}
// PC 0x006322B0 GOG 0x006321A0
void CExoFile::ReadAsync(void* data, unsigned long size, unsigned long count)
{
    m_pInternal->ReadAsync(data, size, count);
}
// PC 0x006322C0 GOG 0x006321B0
int CExoFile::ReadAsyncComplete()
{
    return m_pInternal->ReadAsyncComplete();
}
// PC 0x006322D0 GOG 0x006321C0
unsigned long CExoFile::Write(const char* text)
{
    return m_pInternal->Write(text);
}
// PC 0x006322E0 GOG 0x006321D0
CExoAliasList::~CExoAliasList()
{
    if (m_pInternal != NULL)
    {
        delete m_pInternal;
        m_pInternal = NULL;
    }
}
// PC 0x00632310 GOG 0x00632200
CExoFile::~CExoFile()
{
    if (m_pInternal)
    {
        delete m_pInternal;
        m_pInternal = 0;
    }
}
// PC 0x00632340 GOG 0x00632230
int CExoFile::FileOpened()
{
    return m_pInternal->FileOpened();
}
// PC 0x00632350 GOG 0x00632240
int CExoFile::Flush()
{
    return m_pInternal->Flush();
}
// PC 0x00632370 GOG 0x00632260
unsigned long CExoFile::Write(const void* data, unsigned long size,
                              unsigned long count)
{
    return m_pInternal->Write(data, size, count);
}
// PC 0x006323A0 GOG 0x00632290
unsigned long CExoFile::Write(const CExoString& text)
{
    return m_pInternal->Write(text);
}
// PC 0x006323F0 GOG 0x006322E0
int CExoFile::Seek(int offset, int origin)
{
    return m_pInternal->Seek(offset, origin);
}
// PC 0x00632420 GOG 0x00632310
int CExoFile::SeekBeginning()
{
    return m_pInternal->SeekBeginning();
}
// PC 0x00632440 GOG 0x00632330
int CExoFile::SeekEnd()
{
    return m_pInternal->SeekEnd();
}

inline unsigned long CExoFileInternal::Write(const CExoString& text)
{
    if (m_pFile != NULL)
    {
        return fwrite(text.CStr(), 1, text.GetLength(), m_pFile);
    }
    return 0;
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

unsigned long CExoFile::ReadAsyncBytesRead()
{
    return m_pInternal->ReadAsyncBytesRead();
}
#endif
