#include "exolocstringinternal.h"
#include "exobase.h"
// PC 0x00635880
int CExoLocStringInternal::AddString(int nID, const CExoString& sString)
{
    CExoLinkedListNode* pPos = m_lstStrings.GetHeadPos();
    ExoLocString_st* pEntry;
    {
        CExoLinkedListNode* pHead = m_lstStrings.GetHeadPos();
        ExoLocString_st* pData;
        if (pHead == NULL)
        {
            pData = NULL;
        }
        else
        {
            pData = (ExoLocString_st*)pHead->m_pData;
        }
        pEntry = pData;
    }
    while (pEntry != NULL && pEntry->m_nID != nID)
    {
        pEntry = m_lstStrings.GetNext(pPos);
    }
    if (pEntry != NULL)
    {
        pEntry->m_sString = sString;
        return 1;
    }

    ExoLocString_st* pNewEntry = new ExoLocString_st;
    if (pNewEntry == NULL)
    {
        return 0;
    }
    pNewEntry->m_nID = nID;
    pNewEntry->m_sString = sString;
    m_lstStrings.AddTail(pNewEntry);
    m_nCount++;
    return 1;
}
// PC 0x00635970
int CExoLocStringInternal::Compare(const CExoLocString& lsString) const
{
    int nCount = m_nCount;
    if (nCount != lsString.GetStringCount())
    {
        return 0;
    }
    CExoString* psThis = new CExoString();
    CExoString* psOther = new CExoString();
    ExoLocString_st* pEntry = m_lstStrings.GetHead();
    CExoLinkedListNode* pPos = m_lstStrings.GetHeadPos();
    while (pEntry != NULL)
    {
        int nID = pEntry->m_nID;
        *psThis = pEntry->m_sString;
        lsString.GetString(nID, psOther, 0);
        if (*psThis != *psOther)
        {
            delete psThis;
            delete psOther;
            return 0;
        }
        pEntry = m_lstStrings.GetNext(pPos);
    }
    delete psThis;
    delete psOther;
    return 1;
}
// PC 0x00635AD0
int CExoLocStringInternal::GetString(unsigned long nIndex, int* pnID,
                                     CExoString* psString) const
{
    if (nIndex >= (unsigned long)m_nCount)
    {
        return 0;
    }
    CExoLinkedListNode* pPos = m_lstStrings.GetHeadPos();
    ExoLocString_st* pEntry = m_lstStrings.GetHead();
    unsigned long nCurrent = 0;
    while (pEntry != NULL)
    {
        if (nCurrent == nIndex)
        {
            *pnID = pEntry->m_nID;
            *psString = pEntry->m_sString;
            return 1;
        }
        pEntry = m_lstStrings.GetNext(pPos);
        nCurrent++;
    }
    return 0;
}
// PC 0x00635B40
int CExoLocStringInternal::GetString(int nID, CExoString* psString) const
{
    CExoLinkedListNode* pPos = m_lstStrings.GetHeadPos();
    ExoLocString_st* pEntry;
    {
        CExoLinkedListNode* pHead = m_lstStrings.GetHeadPos();
        ExoLocString_st* pData;
        if (pHead == NULL)
        {
            pData = NULL;
        }
        else
        {
            pData = (ExoLocString_st*)pHead->m_pData;
        }
        pEntry = pData;
    }
    while (pEntry != NULL && pEntry->m_nID != nID)
    {
        pEntry = m_lstStrings.GetNext(pPos);
    }
    if (pEntry == NULL)
    {
        return 0;
    }
    *psString = pEntry->m_sString;
    return 1;
}
// PC 0x00635BA0
int CExoLocStringInternal::GetStringLength(unsigned long nIndex) const
{
    if (nIndex >= (unsigned long)m_nCount)
    {
        return 0;
    }
    CExoLinkedListNode* pPos = m_lstStrings.GetHeadPos();
    ExoLocString_st* pEntry = m_lstStrings.GetHead();
    unsigned long nCurrent = 0;
    while (pEntry != NULL)
    {
        if (nCurrent == nIndex)
        {
            return pEntry->m_sString.GetLength();
        }
        pEntry = m_lstStrings.GetNext(pPos);
        nCurrent++;
    }
    return 0;
}
// PC 0x00635C80
CExoLocStringInternal::CExoLocStringInternal()
{
    m_nCount = 0;
    m_bFetched = 0;
}
// PC 0x00635CA0
void CExoLocStringInternal::ClearLocString()
{
    while (!m_lstStrings.IsEmpty())
    {
        ExoLocString_st* pEntry = m_lstStrings.RemoveHead();
        if (pEntry != NULL)
        {
            delete pEntry;
        }
    }
    m_nCount = 0;
}
// PC 0x00635C10
void CExoLocStringInternal::RemoveString(int nID)
{
    CExoLinkedListNode* pPos = m_lstStrings.GetHeadPos();
    ExoLocString_st* pEntry;
    {
        CExoLinkedListNode* pHead = m_lstStrings.GetHeadPos();
        ExoLocString_st* pData;
        if (pHead == NULL)
        {
            pData = NULL;
        }
        else
        {
            pData = (ExoLocString_st*)pHead->m_pData;
        }
        pEntry = pData;
    }
    while (pEntry != NULL && pEntry->m_nID != nID)
    {
        pEntry = m_lstStrings.GetNext(pPos);
    }
    if (pEntry != NULL)
    {
        m_lstStrings.Remove(pPos);
        delete pEntry;
        m_nCount--;
    }
}
// PC 0x00635CE0
CExoLocStringInternal::~CExoLocStringInternal()
{
    ClearLocString();
}
// PC 0x00635D40
int CExoLocStringInternal::Assign(CExoLocStringInternal* pInternal)
{
    ClearLocString();

    CExoLinkedListNode* pPos = pInternal->m_lstStrings.GetHeadPos();
    ExoLocString_st* pEntry;
    {
        CExoLinkedListNode* pHead = pInternal->m_lstStrings.GetHeadPos();
        ExoLocString_st* pData;
        if (pHead == NULL)
        {
            pData = NULL;
        }
        else
        {
            pData = (ExoLocString_st*)pHead->m_pData;
        }
        pEntry = pData;
    }
    while (pEntry != NULL)
    {
        ExoLocString_st* pNewEntry = new ExoLocString_st;
        if (pNewEntry == NULL)
        {
            return 0;
        }
        pNewEntry->m_nID = pEntry->m_nID;
        pNewEntry->m_sString = pEntry->m_sString;
        m_nCount++;
        m_lstStrings.AddTail(pNewEntry);
        pEntry = pInternal->m_lstStrings.GetNext(pPos);
    }
    m_bFetched = pInternal->m_bFetched;
    return 1;
}
