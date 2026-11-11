#ifndef EXOINIINTERNAL_H
#define EXOINIINTERNAL_H

#include "exobase.h"

class CFileSection;

class CExoIniInternal
{
public:
    CExoIniInternal();
    ~CExoIniInternal();
    int ReadIniEntry(CExoString&, const CExoString&, const CExoString&,
                     const CExoString&);
    int WriteIniEntry(const CExoString&, const CExoString&, const CExoString&,
                      const CExoString&);

    CExoLinkedList<CFileSection> m_lstSections;
    CExoString m_sFile;

public:
    void Destroy();
};

class CFileKey
{
public:
    CExoString m_sLine;
};

class CFileSection
{
public:
    CFileSection();
    ~CFileSection();

    CExoString m_sName;
    CExoLinkedList<CFileKey> m_lstEntries;
};

#endif
