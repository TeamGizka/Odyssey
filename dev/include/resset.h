#ifndef __RESSET_H__
#define __RESSET_H__

#include "exobase.h"
#include "exoresman.h"

struct SETEntry
{
    CExoString m_sEntry;
    CExoString m_sValue;
};

struct SETSection
{
    CExoString m_sSection;
    SETEntry* m_pEntry;
    unsigned long m_nNumEntries;
};

class CResSET : public CRes
{
public:
    CResSET();
    virtual ~CResSET();

    int CopySETData(unsigned char* pSETData);
    char* GetSETDataPtr();
    unsigned long GetSETSize();
    int IsLoaded();

    virtual int OnResourceFreed();
    virtual int OnResourceServiced();

    int CreateSectionTable();
    void GetSectionEntryValue(char* szSection, char* szEntry, char* szValue);
    char* SkipWhiteSpace(char* pLine);
    char* GetNextLine(unsigned long nOffsetStart, unsigned long* pOffsetEnd);

    int m_bServiced;
    unsigned long m_nSETSize;
    SETSection* m_pSection;
    unsigned long m_nNumSections;
};

#endif
