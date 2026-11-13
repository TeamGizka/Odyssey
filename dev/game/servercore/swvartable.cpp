#include "exobase.h"
#include "resgff.h"
#include "nwsvartable.h"
// PC 0x005A11E0 GOG 0x005A1250
CSWVarTable::CSWVarTable()
{
    for (int i = 0; i < 5; i++)
        m_nLocalBoolean[i] = 0;
    for (int j = 0; j < 32; j++)
        m_aLocalNumber[j] = 0;
}
// PC 0x005A1210 GOG 0x005A1280
int CSWVarTable::GetLocalBoolean(int nIndex)
{
    if (nIndex < 0 || nIndex >= 160)
    {
        return 0;
    }
    return m_nLocalBoolean[nIndex / 32] & (1 << (nIndex % 32));
}
// PC 0x005A1260 GOG 0x005A12D0
void CSWVarTable::SetLocalBoolean(int nIndex, int bValue)
{
    if (nIndex < 0 || nIndex >= 160)
    {
        return;
    }

    if (bValue != 0)
    {
        m_nLocalBoolean[nIndex / 32] |= 1 << (nIndex % 32);
    }
    else
    {
        m_nLocalBoolean[nIndex / 32] &= ~(1 << (nIndex % 32));
    }
}
// PC 0x005A12E0 GOG 0x005A1350
unsigned char CSWVarTable::GetLocalNumber(int nIndex)
{
    if (nIndex < 0 || nIndex >= 32)
    {
        return 0;
    }
    return m_aLocalNumber[nIndex];
}
// PC 0x005A1300 GOG 0x005A1370
void CSWVarTable::SetLocalNumber(int nIndex, unsigned char nValue)
{
    if (nIndex >= 0 && nIndex < 32)
    {
        m_aLocalNumber[nIndex] = nValue;
    }
}
// PC 0x005A1320 GOG 0x005A1390
void CSWVarTable::LoadVarTable(CResGFF* pRes, CResStruct* pStruct)
{
    CResStruct sVarTable;
    CResList lstVariables;
    CResStruct sVariable;
    int bSuccess;
    int nCount;
    int i;

    if (pRes->GetStructFromStruct(&sVarTable, pStruct, "SWVarTable"))
    {
        if (pRes->GetList(&lstVariables, &sVarTable, "BitArray"))
        {
            pRes->GetListCount(&lstVariables);
            if (pRes->GetListCount(&lstVariables) < 5)
            {
                nCount = pRes->GetListCount(&lstVariables);
            }
            else
            {
                nCount = 5;
            }

            for (i = 0; i < nCount; i++)
            {
                if (pRes->GetListElement(&sVariable, &lstVariables, i))
                {
                    m_nLocalBoolean[i] = pRes->ReadFieldDWORD(
                        &sVariable, "Variable", bSuccess, 0);
                }
            }
        }

        if (pRes->GetList(&lstVariables, &sVarTable, "ByteArray"))
        {
            pRes->GetListCount(&lstVariables);
            if (pRes->GetListCount(&lstVariables) < 32)
            {
                nCount = pRes->GetListCount(&lstVariables);
            }
            else
            {
                nCount = 32;
            }

            for (i = 0; i < nCount; i++)
            {
                if (pRes->GetListElement(&sVariable, &lstVariables, i))
                {
                    m_aLocalNumber[i] = pRes->ReadFieldBYTE(
                        &sVariable, "Variable", bSuccess, 0);
                }
            }
        }
    }
}
// PC 0x005A1480 GOG 0x005A14F0
void CSWVarTable::SaveVarTable(CResGFF* pRes, CResStruct* pStruct)
{
    CResStruct sVarTable;
    CResList lstVariables;
    CResStruct sVariable;
    int i;

    pRes->AddStructToStruct(&sVarTable, pStruct, "SWVarTable", 0);
    pRes->AddList(&lstVariables, &sVarTable, "BitArray");
    for (i = 0; i < 5; i++)
    {
        pRes->AddListElement(&sVariable, &lstVariables, 0);
        pRes->WriteFieldDWORD(&sVariable, m_nLocalBoolean[i], "Variable");
    }

    pRes->AddList(&lstVariables, &sVarTable, "ByteArray");
    for (i = 0; i < 32; i++)
    {
        pRes->AddListElement(&sVariable, &lstVariables, 0);
        pRes->WriteFieldBYTE(&sVariable, m_aLocalNumber[i], "Variable");
    }
}
