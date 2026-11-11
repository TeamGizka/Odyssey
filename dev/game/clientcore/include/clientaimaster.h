#ifndef __CLIENTAIMASTER_H__
#define __CLIENTAIMASTER_H__

class CExoLinkedListInternal;
class CSWCObject;
class CClientExoAppInternal;
#include "exobase.h"
#include "nwcobject.h"

class CClientAIList
{
public:
    CClientAIList()
    {
        m_pCurrent = NULL;
        m_nCount = 0;
    }

    ~CClientAIList();

    int AddObject(CSWCObject* pObject);
    int RemoveObject(CSWCObject* pObject);
    CSWCObject* GetNextObject();

    int m_nCount;
    CExoLinkedList<unsigned long> m_lstObjects;
    CExoLinkedListNode* m_pCurrent;
};

class CClientAIMaster
{
public:
    CClientAIMaster();
    ~CClientAIMaster();

    void SetExoAppInternal(CClientExoAppInternal* pInternal);

    int AddObject(CSWCObject* pObject);
    int RemoveObject(CSWCObject* pObject);
    int UpdateState();

    CClientExoAppInternal* m_pExoAppInternal;
    CClientAIList m_aiList;
};

#endif
