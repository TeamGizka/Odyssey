#include <string.h>
#include "exobase.h"
#include "appmanager.h"
// PC 0x004DBD60
CObjectTableManager::CObjectTableManager()
{
    m_nTables = 0;
    memset(m_apTables, 0, sizeof(m_apTables));
}
// PC 0x004DBD80
void CObjectTableManager::ClearAll()
{
    for (unsigned long i = 0; i < m_nTables; i++)
    {
        CObjectTable* pTable = m_apTables[i];

        if (pTable)
        {
            if (pTable->m_pTable)
            {
                delete[] pTable->m_pTable;
            }

            delete pTable;
            m_apTables[i] = NULL;
        }
    }
}
// PC 0x004DBDD0
CObjectTableManager::~CObjectTableManager()
{
    ClearAll();
}
// PC 0x004DBDE0
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

    CObjectTable* pTable = m_apTables[nPlayerId];

    if (pTable == NULL)
    {
        return FALSE;
    }

    if (pTable->m_pTable)
    {
        delete[] pTable->m_pTable;
    }

    delete pTable;
    m_apTables[nPlayerId] = NULL;
    return TRUE;
}
