#ifndef EXOLOCSTRINGINTERNAL_H
#define EXOLOCSTRINGINTERNAL_H

#include "exobase.h"

class CExoLocString;

struct ExoLocString_st
{
    int m_nID;
    CExoString m_sString;
};

class CExoLocStringInternal
{
public:
    CExoLocStringInternal();
    ~CExoLocStringInternal();

    int Assign(CExoLocStringInternal* pInternal);
    void ClearLocString();
    int GetString(unsigned long nIndex, int* pnID, CExoString* psString) const;
    int GetString(int nID, CExoString* psString) const;
    int GetStringLength(unsigned long nIndex) const;
    int AddString(int nID, const CExoString& sString);
    void RemoveString(int nID);
    int Compare(const CExoLocString& lsString) const;
    
    mutable CExoLinkedList<ExoLocString_st> m_lstStrings;
    int m_nCount;
    int m_bFetched;
};

#endif
