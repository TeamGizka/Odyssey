#include "exobase.h"

inline CExoLinkedListNode::CExoLinkedListNode()
{
    m_pData = 0;
    m_pNext = 0;
    m_pPrev = 0;
}
// PC 0x005E9AA0  Xbox 0x00134EC0
CExoLinkedListInternal::CExoLinkedListInternal()
{
    m_pTail = 0;
    m_pHead = 0;
    m_nCount = 0;
}
// PC 0x005E9AB0  Xbox 0x00134ED0
CExoLinkedListInternal::~CExoLinkedListInternal()
{
    CExoLinkedListNode* pNode = m_pHead;
    while (pNode)
    {
        CExoLinkedListNode* pDelete = pNode;
        pNode = pNode->m_pNext;
        delete pDelete;
    }
}

CExoLinkedListNode* CExoLinkedListInternal::AddAfter(
    void* pData, CExoLinkedListNode* pPosition)
{
    if (!pPosition)
    {
        g_pExoBase->m_pExoDebug->Warning(
            147, "exolinkedlistinternal.cpp",
            "CExoLinkedListInternal::AddAfter() Position parameter is NULL");
        return 0;
    }
    CExoLinkedListNode* pNode = new CExoLinkedListNode();
    pNode->m_pNext = pPosition->m_pNext;
    if (pPosition->m_pNext)
        pPosition->m_pNext->m_pPrev = pNode;
    pPosition->m_pNext = pNode;
    pNode->m_pPrev = pPosition;
    pNode->m_pData = pData;
    if (m_pTail == pPosition)
        m_pTail = pNode;
    if (!m_pHead)
        m_pHead = pNode;
    m_nCount++;
    return pNode;
}
// PC 0x005E9AD0  Xbox 0x00134EF0
CExoLinkedListNode* CExoLinkedListInternal::AddBefore(
    void* pData, CExoLinkedListNode* pPosition)
{
    if (!pPosition)
    {
        g_pExoBase->m_pExoDebug->Warning(
#ifdef _XBOX
            193, "exolinkedlistinternal.cpp",
#else
            191, "exolinkedlistinternal.cpp",
#endif
            "CExoLinkedListInternal::AddBefore() Position parameter is NULL");
        return 0;
    }
    CExoLinkedListNode* pNode = new CExoLinkedListNode();
    pNode->m_pNext = pPosition;
    pNode->m_pPrev = pPosition->m_pPrev;
    if (pPosition->m_pPrev)
        pPosition->m_pPrev->m_pNext = pNode;
    pPosition->m_pPrev = pNode;
    pNode->m_pData = pData;
    if (!m_pTail)
        m_pTail = pNode;
    if (m_pHead == pPosition)
        m_pHead = pNode;
    m_nCount++;
    return pNode;
}
// PC 0x005E9B60  Xbox 0x00134F80
void* CExoLinkedListInternal::GetAtPos(CExoLinkedListNode* pPosition) const
{
    if (!pPosition)
    {
        g_pExoBase->m_pExoDebug->Warning(
#ifdef _XBOX
            238, "exolinkedlistinternal.cpp",
#else
            236, "exolinkedlistinternal.cpp",
#endif
            "CExoLinkedListInternal::GetAtPos() Position paramter is NULL");
        return 0;
    }
    return pPosition->m_pData;
}
// PC 0x005E9B90  Xbox 0x00134FB0
void* CExoLinkedListInternal::GetNext(CExoLinkedListNode*& pPosition) const
{
    if (!pPosition)
    {
        g_pExoBase->m_pExoDebug->Warning(
#ifdef _XBOX
            257, "exolinkedlistinternal.cpp",
#else
            255, "exolinkedlistinternal.cpp",
#endif
            "CExoLinkedListInternal::GetNext() Position paramter is NULL");
        return 0;
    }
    CExoLinkedListNode* pNext = pPosition->m_pNext;
    if (!pNext)
    {
        pPosition = pNext;
        return 0;
    }
    pPosition = pNext;
    return pNext->m_pData;
}
// PC 0x005E9BD0  Xbox 0x00134FF0
void* CExoLinkedListInternal::GetPrev(CExoLinkedListNode*& pPosition) const
{
    if (!pPosition)
    {
        g_pExoBase->m_pExoDebug->Warning(
#ifdef _XBOX
            283, "exolinkedlistinternal.cpp",
#else
            281, "exolinkedlistinternal.cpp",
#endif
            "CExoLinkedListInternal::GetPreviousg() Position paramter is NULL");
        return 0;
    }
    CExoLinkedListNode* pPrev = pPosition->m_pPrev;
    if (!pPrev)
    {
        pPosition = pPrev;
        return 0;
    }
    pPosition = pPrev;
    return pPrev->m_pData;
}
// PC 0x005E9C10  Xbox 0x00135030
void* CExoLinkedListInternal::Remove(CExoLinkedListNode* pPosition)
{
    if (!pPosition)
    {
        g_pExoBase->m_pExoDebug->Warning(
#ifdef _XBOX
            309, "exolinkedlistinternal.cpp",
#else
            307, "exolinkedlistinternal.cpp",
#endif
            "CExoLinkedListInternal::Remove() Position paramter is NULL");
        return 0;
    }
    CExoLinkedListNode* pPrev = pPosition->m_pPrev;
    CExoLinkedListNode* pNext = pPosition->m_pNext;
    void* pData = pPosition->m_pData;
    if (pPrev)
        pPrev->m_pNext = pNext;
    if (pNext)
        pNext->m_pPrev = pPrev;
    if (m_pTail == pPosition)
        m_pTail = pPosition->m_pPrev;
    if (m_pHead == pPosition)
        m_pHead = pPosition->m_pNext;
    delete pPosition;
    m_nCount--;
    return pData;
}
// PC 0x005E9C80  Xbox 0x001350A0
void* CExoLinkedListInternal::RemoveHead()
{
    CExoLinkedListNode* pNode = m_pHead;
    if (!pNode)
    {
        g_pExoBase->m_pExoDebug->Warning(
#ifdef _XBOX
            359, "exolinkedlistinternal.cpp",
#else
            357, "exolinkedlistinternal.cpp",
#endif
            "CExoLinkedListInternal::RemoveHead() Removing from empty List");
        return 0;
    }
    void* pData = pNode->m_pData;
    m_pHead = pNode->m_pNext;
    if (!m_pHead)
        m_pTail = m_pHead;
    else
        m_pHead->m_pPrev = 0;
    delete pNode;
    m_nCount--;
    return pData;
}
// PC 0x005E9CE0  Xbox 0x00135100
void* CExoLinkedListInternal::RemoveTail()
{
    CExoLinkedListNode* pNode = m_pTail;
    if (!pNode)
    {
        g_pExoBase->m_pExoDebug->Warning(
#ifdef _XBOX
            397, "exolinkedlistinternal.cpp",
#else
            395, "exolinkedlistinternal.cpp",
#endif
            "CExoLinkedListInternal::RemoveTail() Removing from empty List");
        return 0;
    }
    void* pData = pNode->m_pData;
    m_pTail = pNode->m_pPrev;
    if (!m_pTail)
        m_pHead = m_pTail;
    else
        m_pTail->m_pNext = 0;
    delete pNode;
    m_nCount--;
    return pData;
}
// PC 0x005E9D40  Xbox 0x00135160
int CExoLinkedListInternal::Contains(void* pData) const
{
    CExoLinkedListNode* pNode = m_pHead;
    while (pNode)
    {
        if (pNode->m_pData == pData)
            return 1;
        pNode = pNode->m_pNext;
    }
    return 0;
}
// PC 0x005E9D70  Xbox 0x00135190
CExoLinkedListNode* CExoLinkedListInternal::AddHead(void* pData)
{
    CExoLinkedListNode* pNode = new CExoLinkedListNode();
    pNode->m_pNext = m_pHead;
    pNode->m_pPrev = 0;
    if (m_pHead)
        m_pHead->m_pPrev = pNode;
    m_pHead = pNode;
    pNode->m_pData = pData;
    if (!m_pTail)
        m_pTail = pNode;
    m_nCount++;
    return pNode;
}
// PC 0x005E9DC0  Xbox 0x001351E0
CExoLinkedListNode* CExoLinkedListInternal::AddTail(void* pData)
{
    CExoLinkedListNode* pNode = new CExoLinkedListNode();
    pNode->m_pPrev = m_pTail;
    pNode->m_pNext = 0;
    if (m_pTail)
        m_pTail->m_pNext = pNode;
    m_pTail = pNode;
    pNode->m_pData = pData;
    if (!m_pHead)
        m_pHead = pNode;
    m_nCount++;
    return pNode;
}
