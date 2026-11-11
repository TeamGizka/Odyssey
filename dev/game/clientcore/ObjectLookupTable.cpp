#include <string.h>
#include "exobase.h"
#include "appmanager.h"
// PC 0x004AE5D0  Xbox 0x0001C2B0
CObjectTableManager::CObjectTableManager()
{
    m_nTables = 0;
    memset(m_apTables, 0, sizeof(m_apTables));
}
// PC 0x004AE5F0  Xbox 0x0001C2D0
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
// PC 0x004AE640  Xbox 0x0001C320
CObjectTableManager::~CObjectTableManager()
{
    ClearAll();
}
// PC 0x004AE650  Xbox 0x0001C330
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
