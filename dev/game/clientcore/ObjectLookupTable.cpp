#include <string.h>
#include "exobase.h"
#include "appmanager.h"
#include "gameobjectarray.h"

// Dead code: not in the PC executable
CObjectLookupTable::CObjectLookupTable()
{
    m_pTable = new unsigned long[OBJECT_LOOKUP_TABLE_SIZE];

    for (unsigned long i = 0; i < OBJECT_LOOKUP_TABLE_SIZE; i++)
    {
        m_pTable[i] = OBJECT_INVALID;
    }

    m_nReserved = 0;
    m_nFirst = 0xFFFFFFFF;
    m_nNext = 0;
}

// Dead code: not in the PC executable
CObjectLookupTable::~CObjectLookupTable()
{
    if (m_pTable)
    {
        delete[] m_pTable;
    }
}

// Dead code: not in the PC executable
unsigned long CObjectLookupTable::AddID(unsigned long nID)
{
    if (m_nFirst == m_nNext || m_nFirst == 0xFFFFFFFF)
    {
        m_nFirst++;

        if (m_nFirst == OBJECT_LOOKUP_TABLE_SIZE)
        {
            EXOLOG("LookUpTable wrapped\n");
            m_nFirst = 0;
        }
    }

    m_pTable[m_nNext] = nID;

    unsigned long nIndex = m_nNext;

    m_nNext++;

    if (m_nNext == OBJECT_LOOKUP_TABLE_SIZE)
    {
        m_nNext = 0;
    }

    return nIndex;
}

// Dead code: not in the PC executable
unsigned long CObjectLookupTable::GetIDFromIndex(unsigned long nIndex)
{
    return m_pTable[nIndex];
}

// Dead code: not in the PC executable
unsigned long CObjectLookupTable::GetIndexFromID(unsigned long nID)
{
    if (m_nFirst != 0xFFFFFFFF)
    {
        unsigned long nIndex = m_nFirst;

        do
        {
            if (m_pTable[nIndex] == nID)
            {
                return nIndex;
            }

            nIndex++;

            if (nIndex == OBJECT_LOOKUP_TABLE_SIZE)
            {
                nIndex = 0;
            }
        }
        while (nIndex != m_nNext);
    }

    return 0xFFFFFFFF;
}

// Dead code: not in the PC executable
void CObjectLookupTable::Touch(unsigned long nIndex)
{
    unsigned long nID = m_pTable[nIndex];

    if (m_nFirst != m_nNext || nIndex < m_nFirst)
    {
        memmove(&m_pTable[nIndex], &m_pTable[nIndex + 1], (m_nNext - nIndex - 1) * sizeof(unsigned long));
        m_pTable[m_nNext - 1] = nID;
    }
    else
    {
        if (nIndex > m_nFirst)
        {
            memmove(&m_pTable[m_nFirst + 1], &m_pTable[m_nFirst], (nIndex - m_nFirst) * sizeof(unsigned long));
            m_pTable[m_nFirst] = nID;
        }

        m_nFirst++;
        m_nNext++;

        if (m_nFirst == OBJECT_LOOKUP_TABLE_SIZE)
        {
            m_nFirst = 0;
            m_nNext = 0;
        }
    }
}

// Dead code: not in the PC executable
void CObjectLookupTable::GetUpdate(void* pData, unsigned long* pSize)
{
    if (m_nFirst == m_nNext)
    {
        *pSize = (OBJECT_LOOKUP_TABLE_SIZE + 1) * sizeof(unsigned long);
        pData = new unsigned long[OBJECT_LOOKUP_TABLE_SIZE + 1];
        *(unsigned long*)pData = m_nFirst;
        memcpy((unsigned long*)pData + 1, m_pTable, OBJECT_LOOKUP_TABLE_SIZE * sizeof(unsigned long));
    }
    else
    {
        *pSize = (m_nNext + 1) * sizeof(unsigned long);
        pData = new unsigned long[m_nNext + 1];
        *(unsigned long*)pData = m_nFirst;
        memcpy((unsigned long*)pData + 1, m_pTable, m_nNext * sizeof(unsigned long));
    }
}

// Dead code: not in the PC executable
void CObjectLookupTable::Update(void* pData, unsigned long nSize)
{
    m_nFirst = *(unsigned long*)pData;
    nSize -= sizeof(unsigned long);

    if (nSize == OBJECT_LOOKUP_TABLE_SIZE * sizeof(unsigned long))
    {
        m_nNext = m_nFirst;
    }
    else
    {
        m_nNext = nSize / sizeof(unsigned long);
    }

    memcpy(m_pTable, (unsigned long*)pData + 1, nSize);
    delete[] (unsigned long*)pData;
}
// PC 0x004DBD60 GOG 0x004DBEA0
CObjectTableManager::CObjectTableManager()
{
    m_nTables = 0;
    memset(m_apTables, 0, sizeof(m_apTables));
}
// PC 0x004DBDD0 GOG 0x004DBF10
CObjectTableManager::~CObjectTableManager()
{
    ClearAll();
}
// PC 0x004DBDE0 GOG 0x004DBF20
BOOL CObjectTableManager::RemovePlayer(unsigned long nPlayerId)
{
    if (nPlayerId == 0xFFFFFFFF)
    {
        return FALSE;
    }

    if (nPlayerId >= 96)
    {
        EXOXBOXASSERT(0);
        return FALSE;
    }

    CObjectLookupTable* pTable = m_apTables[nPlayerId];

    if (pTable == NULL)
    {
        return FALSE;
    }

    delete pTable;
    m_apTables[nPlayerId] = NULL;
    return TRUE;
}
// PC 0x004DBD80 GOG 0x004DBEC0
void CObjectTableManager::ClearAll()
{
    for (unsigned long i = 0; i < m_nTables; i++)
    {
        CObjectLookupTable* pTable = m_apTables[i];

        if (pTable)
        {
            delete pTable;
            m_apTables[i] = NULL;
        }
    }
}

// Dead code: not in the PC executable
BOOL CObjectTableManager::CreateNewPlayer(unsigned long nPlayerId)
{
    if (nPlayerId == 0xFFFFFFFF)
    {
        return FALSE;
    }

    if (nPlayerId >= 96)
    {
        EXOXBOXASSERT(0);
        return FALSE;
    }

    if (m_apTables[nPlayerId] != NULL)
    {
        return FALSE;
    }

    m_apTables[nPlayerId] = new CObjectLookupTable;

    if (m_apTables[nPlayerId] == NULL)
    {
        return FALSE;
    }

    if (nPlayerId + 1 > m_nTables)
    {
        m_nTables = nPlayerId + 1;
    }

    return TRUE;
}

// Dead code: not in the PC executable
BOOL CObjectTableManager::Touch(unsigned long nIndex, unsigned long nPlayerId)
{
    if (nPlayerId == 0xFFFFFFFF)
    {
        return FALSE;
    }

    if (nPlayerId >= 96)
    {
        EXOXBOXASSERT(0);
        return FALSE;
    }

    CObjectLookupTable* pTable = m_apTables[nPlayerId];

    if (pTable == NULL)
    {
        return FALSE;
    }

    pTable->Touch(nIndex);
    return TRUE;
}

// Dead code: not in the PC executable
BOOL CObjectTableManager::AddID(unsigned long nID, unsigned long* pIndex, unsigned long nPlayerId)
{
    *pIndex = 0xFFFFFFFF;

    if (nPlayerId == 0xFFFFFFFF)
    {
        return FALSE;
    }

    if (nPlayerId >= 96)
    {
        EXOXBOXASSERT(0);
        return FALSE;
    }

    if (m_apTables[nPlayerId] == NULL)
    {
        if (!CreateNewPlayer(nPlayerId))
        {
            return FALSE;
        }
    }

    *pIndex = m_apTables[nPlayerId]->AddID(nID);
    return TRUE;
}

// Dead code: not in the PC executable
unsigned long CObjectTableManager::GetIDFromIndex(unsigned long nIndex, unsigned long nPlayerId)
{
    if (nPlayerId == 0xFFFFFFFF)
    {
        return OBJECT_INVALID;
    }

    if (m_apTables[nPlayerId] == NULL)
    {
        CreateNewPlayer(nPlayerId);
    }

    return m_apTables[nPlayerId]->GetIDFromIndex(nIndex);
}

// Dead code: not in the PC executable
unsigned long CObjectTableManager::GetIndexFromID(unsigned long nID, unsigned long nPlayerId)
{
    if (nPlayerId == 0xFFFFFFFF)
    {
        return 0xFFFFFFFF;
    }

    if (m_apTables[nPlayerId] == NULL)
    {
        CreateNewPlayer(nPlayerId);
    }

    return m_apTables[nPlayerId]->GetIndexFromID(nID);
}
