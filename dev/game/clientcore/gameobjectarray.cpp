#include "exobase.h"
#include "gameobjectarray.h"
#include "nwsobject.h"

const unsigned char CGameObject::OBJECT_TYPE_GUI = 1;
const unsigned char CGameObject::OBJECT_TYPE_TILE = 2;
const unsigned char CGameObject::OBJECT_TYPE_MODULE = 3;
const unsigned char CGameObject::OBJECT_TYPE_AREA = 4;
const unsigned char CGameObject::OBJECT_TYPE_CREATURE = 5;
const unsigned char CGameObject::OBJECT_TYPE_ITEM = 6;
const unsigned char CGameObject::OBJECT_TYPE_TRIGGER = 7;
const unsigned char CGameObject::OBJECT_TYPE_PROJECTILE = 8;
const unsigned char CGameObject::OBJECT_TYPE_PLACEABLE = 9;
const unsigned char CGameObject::OBJECT_TYPE_DOOR = 10;
const unsigned char CGameObject::OBJECT_TYPE_AREAOFEFFECTOBJECT = 11;
const unsigned char CGameObject::OBJECT_TYPE_WAYPOINT = 12;
const unsigned char CGameObject::OBJECT_TYPE_ENCOUNTER = 13;
const unsigned char CGameObject::OBJECT_TYPE_STORE = 14;
const unsigned char CGameObject::OBJECT_TYPE_SOUND = 16;
const unsigned long CGameObjectArray::ARRAY_SIZE = 4096;
const unsigned long CGameObjectArray::INVALIDOBJID = 0x7f000000UL;
const unsigned long CGameObjectArray::MINCHAROBJID = 0x7f000001UL;
const unsigned long CGameObjectArray::MAXCHAROBJID = 0x7fffffffUL;
const unsigned long CGameObjectArray::MINOBJECTID = 0;
const unsigned long CGameObjectArray::MAXOBJECTID = 0x7effffffUL;
const unsigned long CGameObjectArray::CHAROBJECT_MASK = 0x7f000000UL;
const unsigned long CGameObjectArray::LISTTYPE_MASK = 0x80000000UL;
const unsigned long CGameObjectArray::LISTINDEX_MASK = 0x7ffff000UL;
const unsigned long CGameObjectArray::ARRAYINDEX_MASK = 0xfffUL;
const unsigned long CGameObjectArray::ARRAYINDEX_SHIFT = 12;
const unsigned char CGameObjectArray::SUCCESS = 0;
const unsigned char CGameObjectArray::BAD_ID = 1;
const unsigned char CGameObjectArray::OUT_OF_MEMORY = 2;
const unsigned char CGameObjectArray::INVALIDLIST = 3;
const unsigned char CGameObjectArray::NULL_GAMEOBJECT = 4;
const unsigned char CGameObjectArray::INTERNALOBJECT = 0;
const unsigned char CGameObjectArray::EXTERNALOBJECT = 1;
// PC 0x004D7D60  Xbox 0x00092420
CGameObject::CGameObject(unsigned char nObjectType, unsigned long oidSelf)
{
    m_nId = oidSelf;
    m_nObjectType = nObjectType;
}
// PC 0x004D7D80  Xbox 0x00092440
CGameObjectArray::CGameObjectArray(int bExternal)
{
    m_bExternal = bExternal;

    if (bExternal)
    {
        m_pArray = new CGameObjectArrayNode*[0x2000];
        for (unsigned long i = 0; i < 0x2000; i++)
        {
            m_pArray[i] = NULL;
        }
    }
    else
    {
        m_pArray = new CGameObjectArrayNode*[ARRAY_SIZE];
        for (unsigned long i = 0; i < ARRAY_SIZE; i++)
        {
            m_pArray[i] = NULL;
        }
    }

    for (int i = 0; i < 2; i++)
    {
        m_nNextObjectId[i] = 0;
        m_nPrevObjectId[i] = MAXCHAROBJID;
    }
}
// PC 0x004D7E10  Xbox 0x000924D0
CGameObjectArray::~CGameObjectArray()
{
    int nSize = m_bExternal ? 0x2000 : 0x1000;

    for (int i = 0; i < nSize; i++)
    {
        while (m_pArray[i] != NULL)
        {
            CGameObjectArrayNode* pNode = m_pArray[i];

            if (pNode->m_pObject != NULL)
            {
                delete pNode->m_pObject;
            }

            if (m_pArray[i] == pNode)
            {
                m_pArray[i] = pNode->m_pNext;
                delete pNode;
            }
        }
    }

    delete m_pArray;
    m_pArray = NULL;
}
// PC 0x004D7E90  Xbox 0x00092550
unsigned char CGameObjectArray::AddObjectAtPos(unsigned long oidObject,
                                               CGameObject* pObject)
{
    if (pObject != NULL)
    {
        CGameObjectArrayNode* pNew;
        CGameObjectArrayNode* pNode;
        CGameObjectArrayNode* pPrev;
        unsigned long nIndex;

        while (1)
        {
            unsigned short nList = (unsigned short)((oidObject >> 31) & 1);
            unsigned long nId = oidObject & 0x7FFFFFFF;
            unsigned long nNewId;

            nIndex = (oidObject & ARRAYINDEX_MASK) + (nList << ARRAYINDEX_SHIFT);

            if ((nId & CHAROBJECT_MASK) != 0)
            {
                if (nId < MINCHAROBJID || nId > MAXCHAROBJID)
                {
                    return BAD_ID;
                }
                if (m_nPrevObjectId[nList] >= nId)
                {
                    m_nPrevObjectId[nList] = nId - 1;
                }
                nNewId = m_nPrevObjectId[nList];
            }
            else
            {
                if (nId > MAXOBJECTID)
                {
                    return BAD_ID;
                }
                if (m_nNextObjectId[nList] <= nId)
                {
                    m_nNextObjectId[nList] = nId + 1;
                }
                nNewId = m_nNextObjectId[nList];
            }

            pNew = new CGameObjectArrayNode;
            pNew->m_oidObject = nId;
            pNew->m_pObject = pObject;
            pNew->m_pNext = NULL;

            pNode = m_pArray[nIndex];
            if (pNode == NULL)
            {
                m_pArray[nIndex] = pNew;
                return SUCCESS;
            }

            pPrev = NULL;
            do
            {
                if (pNode->m_oidObject <= nId)
                {
                    break;
                }
                pPrev = pNode;
                pNode = pNode->m_pNext;
            } while (pNode != NULL);

            if (pNode == NULL)
            {
                pPrev->m_pNext = pNew;
                return SUCCESS;
            }

            if (pNode->m_oidObject != nId)
            {
                break;
            }

            CGameObject* pOld = pNode->m_pObject;
            delete pNew;
            if (nList != 0)
            {
                nNewId |= LISTTYPE_MASK;
            }
            oidObject = nNewId;
            pObject = pOld;
            if (pObject == NULL)
            {
                return NULL_GAMEOBJECT;
            }
        }

        if (pPrev == NULL)
        {
            pNew->m_pNext = m_pArray[nIndex];
            m_pArray[nIndex] = pNew;
        }
        else
        {
            pNew->m_pNext = pPrev->m_pNext;
            pPrev->m_pNext = pNew;
        }
        return SUCCESS;
    }

    return NULL_GAMEOBJECT;
}
// PC 0x004D7FF0  Xbox 0x000926B0
unsigned char CGameObjectArray::AddExternalObject(unsigned long& oidObject,
                                                  CGameObject* pObject,
                                                  int bAscending)
{
    if (pObject == NULL)
    {
        return NULL_GAMEOBJECT;
    }

    unsigned long nId = oidObject & 0x7FFFFFFF;
    unsigned long nIndex = (nId & ARRAYINDEX_MASK) + ARRAY_SIZE;

    if ((nId & CHAROBJECT_MASK) != 0)
    {
        if (nId < MINCHAROBJID)
        {
            return BAD_ID;
        }
        if (nId > MAXCHAROBJID)
        {
            return BAD_ID;
        }
        if (m_nPrevObjectId[1] >= nId)
        {
            m_nPrevObjectId[1] = nId - 1;
        }
    }
    else
    {
        if (nId > MAXOBJECTID)
        {
            return BAD_ID;
        }
        if (m_nNextObjectId[1] <= nId)
        {
            m_nNextObjectId[1] = nId + 1;
        }
    }

    CGameObjectArrayNode* pNew = new CGameObjectArrayNode;
    pNew->m_pNext = NULL;
    pNew->m_oidObject = nId;
    pNew->m_pObject = pObject;

    CGameObjectArrayNode* pNode = m_pArray[nIndex];
    if (pNode == NULL)
    {
        m_pArray[nIndex] = pNew;
    }
    else
    {
        CGameObjectArrayNode* pCur = m_pArray[nIndex];
        CGameObjectArrayNode* pPrev = NULL;
        do
        {
            if (pCur->m_oidObject <= nId)
            {
                break;
            }
            pPrev = pCur;
            pCur = pCur->m_pNext;
        } while (pCur != NULL);

        if (pCur == NULL)
        {
            pPrev->m_pNext = pNew;
        }
        else if (pPrev == NULL)
        {
            pNew->m_pNext = pNode;
            m_pArray[nIndex] = pNew;
        }
        else
        {
            pNew->m_pNext = pPrev->m_pNext;
            pPrev->m_pNext = pNew;
        }
    }

    oidObject |= LISTTYPE_MASK;
    return SUCCESS;
}
// PC 0x004D80E0  Xbox 0x000927A0
unsigned char CGameObjectArray::AddInternalObject(unsigned long& oidObject,
                                                  CGameObject* pObject,
                                                  int bDescending)
{
    if (pObject == NULL)
    {
        return NULL_GAMEOBJECT;
    }

    if (bDescending)
    {
        EXOXBOXASSERT(m_nPrevObjectId[0] > OBJECT_INVALID + 1);
        oidObject = m_nPrevObjectId[0]--;
    }
    else
    {
        EXOXBOXASSERT(m_nNextObjectId[0] < OBJECT_INVALID - 1);
        oidObject = m_nNextObjectId[0]++;
    }

    unsigned long nIndex = oidObject & ARRAYINDEX_MASK;

    CGameObjectArrayNode* pNew = new CGameObjectArrayNode;
    pNew->m_oidObject = oidObject;
    pNew->m_pObject = pObject;
    pNew->m_pNext = NULL;

    CGameObjectArrayNode* pHead = m_pArray[nIndex];
    if (pHead == NULL)
    {
        m_pArray[nIndex] = pNew;
        return SUCCESS;
    }

    CGameObjectArrayNode* pCur = pHead;
    CGameObjectArrayNode* pPrev = NULL;
    do
    {
        if (pCur->m_oidObject <= pNew->m_oidObject)
        {
            break;
        }
        pPrev = pCur;
        pCur = pCur->m_pNext;
    } while (pCur != NULL);

    if (pCur == NULL)
    {
        pPrev->m_pNext = pNew;
        return SUCCESS;
    }

    if (pPrev == NULL)
    {
        pNew->m_pNext = pHead;
        m_pArray[nIndex] = pNew;
    }
    else
    {
        pNew->m_pNext = pPrev->m_pNext;
        pPrev->m_pNext = pNew;
    }
    return SUCCESS;
}
// PC 0x004D81A0  Xbox 0x00092880
unsigned char CGameObjectArray::Delete(unsigned long oidObject,
                                       CGameObject** ppObject)
{
    unsigned long nList = (unsigned char)(oidObject >> 31) & 1;
    unsigned long nId = oidObject & 0x7FFFFFFF;
    unsigned long nIndex = (oidObject & ARRAYINDEX_MASK) + (nList << ARRAYINDEX_SHIFT);
    CGameObjectArrayNode* pNode = m_pArray[nIndex];
    if (pNode == NULL)
    {
        return BAD_ID;
    }

    CGameObjectArrayNode* pPrev = NULL;
    do
    {
        if (pNode->m_oidObject == nId)
        {
            break;
        }
        pPrev = pNode;
        pNode = pNode->m_pNext;
    } while (pNode != NULL);

    if (pNode == NULL)
    {
        return BAD_ID;
    }

    *ppObject = pNode->m_pObject;
    if (pPrev == NULL)
    {
        m_pArray[nIndex] = pNode->m_pNext;
    }
    else
    {
        pPrev->m_pNext = pNode->m_pNext;
    }
    delete pNode;
    return SUCCESS;
}
// PC 0x004D8230  Xbox 0x00092910
unsigned char CGameObjectArray::GetGameObject(unsigned long oidObject,
                                              CGameObject** ppObject)
{
    unsigned long nList = (unsigned char)(oidObject >> 31) & 1;
    unsigned long nId = oidObject & 0x7FFFFFFF;

    CGameObjectArrayNode* pNode = m_pArray[(oidObject & ARRAYINDEX_MASK) + (nList << ARRAYINDEX_SHIFT)];
    while (pNode != NULL && pNode->m_oidObject != nId)
    {
        pNode = pNode->m_pNext;
    }

    if (pNode == NULL)
    {
        *ppObject = NULL;
        return BAD_ID;
    }

    *ppObject = pNode->m_pObject;
    return SUCCESS;
}
// PC 0x004D8290  Xbox 0x00092970
unsigned char CGameObjectArray::Delete(unsigned long oidObject)
{
    CGameObject* pObject;
    return Delete(oidObject, &pObject);
}
