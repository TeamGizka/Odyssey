#include "exobase.h"
#include "resgff.h"
#include "nwsobject.h"
#include "gameobjectarray.h"
#include "nwjournal.h"
#include "nwsvartable.h"

inline void ScriptVarDestroy(CSWSScriptVar* pVar)
{
    pVar->~CSWSScriptVar();
}

inline void ScriptVarTableReserve(CSWSScriptVarTable* pTable, int nSize, CSWSScriptVar* pVars)
{
    pTable->Allocate(nSize);
    delete[] pVars;
}

const int CSWSScriptVarTable::VARTYPE_INTEGER = 1;
const int CSWSScriptVarTable::VARTYPE_FLOAT = 2;
const int CSWSScriptVarTable::VARTYPE_STRING = 3;
const int CSWSScriptVarTable::VARTYPE_OBJECT = 4;
const int CSWSScriptVarTable::VARTYPE_LOCATION = 5;
// PC 0x005A08C0 GOG 0x005A0930
void CSWSScriptVarTable::SaveVarTable(CResGFF* pRes, CResStruct* pStruct)
{
    CScriptLocation lLocation;
    CResList lstVars;
    CResStruct sElement;
    CResStruct sLocation;

    pRes->AddList(&lstVars, pStruct, "VarTable");

    for (int i = 0; i < num; i++)
    {
        pRes->AddListElement(&sElement, &lstVars, 0);
        pRes->WriteFieldCExoString(&sElement, element[i].m_sName, "Name");
        pRes->WriteFieldDWORD(&sElement, element[i].m_nType, "Type");

        switch (element[i].m_nType)
        {
        case SCRIPTVAR_TYPE_FLOAT:
        {
            float fValue = 0.0f;
            CSWSScriptVar* pVar =
                MatchIndex(element[i].m_sName, SCRIPTVAR_TYPE_FLOAT, 0);
            if (pVar != NULL)
            {
                fValue = pVar->m_fValue;
            }
            pRes->WriteFieldFLOAT(&sElement, fValue, "Value");
            break;
        }
        case SCRIPTVAR_TYPE_INT:
        {
            int nValue = 0;
            CSWSScriptVar* pVar =
                MatchIndex(element[i].m_sName, SCRIPTVAR_TYPE_INT, 0);
            if (pVar != NULL)
            {
                nValue = pVar->m_nValue;
            }
            pRes->WriteFieldINT(&sElement, nValue, "Value");
            break;
        }
        case SCRIPTVAR_TYPE_LOCATION:
            lLocation = GetLocation(element[i].m_sName);
            pRes->AddStructToStruct(&sLocation, &sElement, "Value", 1);
            lLocation.SaveLocation(pRes, &sLocation);
            break;
        case SCRIPTVAR_TYPE_OBJECT:
        {
            unsigned long oidValue = CGameObjectArray::INVALIDOBJID;
            CSWSScriptVar* pVar =
                MatchIndex(element[i].m_sName, SCRIPTVAR_TYPE_OBJECT, 0);
            if (pVar != NULL)
            {
                oidValue = pVar->m_oidValue;
            }
            pRes->WriteFieldDWORD(&sElement, oidValue, "Value");
            break;
        }
        case SCRIPTVAR_TYPE_STRING:
            pRes->WriteFieldCExoString(&sElement, GetString(element[i].m_sName),
                                       "Value");
            break;
        default:
            break;
        }
    }
}
// PC 0x0059FE90 GOG 0x0059FF00
CSWSScriptVar* CSWSScriptVarTable::MatchIndex(const CExoString& sName,
                                              unsigned long nType, int bCreate)
{
    for (int i = 0; i < num; i++)
    {
        if (element[i].m_nType == nType && element[i].m_sName == sName)
        {
            return &element[i];
        }
    }

    if (bCreate)
    {
        CSWSScriptVar var;
        var.m_sName = sName;
        var.m_nType = nType;
        var.m_nValue = 0;

        if (nType == SCRIPTVAR_TYPE_STRING)
        {
            var.m_psValue = new CExoString;
        }
        else if (nType == SCRIPTVAR_TYPE_LOCATION)
        {
            var.m_plValue = new CScriptLocation;
        }

        Add(var);
        int nCount = num;
        return &element[nCount - 1];
    }

    return NULL;
}
// PC 0x0059FFE0 GOG 0x005A0050
void CSWSScriptVarTable::DeleteIndex(const CExoString& sName,
                                     unsigned long nType)
{
    for (int i = 0; i < num; i++)
    {
        if (element[i].m_nType == nType && element[i].m_sName == sName)
        {
            DelIndex(i);
            return;
        }
    }
}
// PC 0x005A0030 GOG 0x005A00A0
int CSWSScriptVarTable::GetInt(const CExoString& sName)
{
    int nValue = 0;
    CSWSScriptVar* pVar = MatchIndex(sName, SCRIPTVAR_TYPE_INT, 0);

    if (pVar != NULL)
    {
        nValue = pVar->m_nValue;
    }

    return nValue;
}
// PC 0x005A0060 GOG 0x005A00D0
float CSWSScriptVarTable::GetFloat(const CExoString& sName)
{
    float fValue = 0.0f;
    CSWSScriptVar* pVar = MatchIndex(sName, SCRIPTVAR_TYPE_FLOAT, 0);

    if (pVar != NULL)
    {
        fValue = pVar->m_fValue;
    }

    return fValue;
}
// PC 0x005A0090 GOG 0x005A0100
CExoString CSWSScriptVarTable::GetString(const CExoString& sName)
{
    CExoString sValue;
    CSWSScriptVar* pVar = MatchIndex(sName, SCRIPTVAR_TYPE_STRING, 0);

    if (pVar != NULL)
    {
        sValue = *pVar->m_psValue;
    }

    return sValue;
}
// PC 0x005A0120 GOG 0x005A0190
unsigned long CSWSScriptVarTable::GetObject(const CExoString& sName)
{
    unsigned long oidValue = CGameObjectArray::INVALIDOBJID;
    CSWSScriptVar* pVar = MatchIndex(sName, SCRIPTVAR_TYPE_OBJECT, 0);

    if (pVar != NULL)
    {
        oidValue = pVar->m_oidValue;
    }

    return oidValue;
}
// PC 0x005A0150 GOG 0x005A01C0
CScriptLocation CSWSScriptVarTable::GetLocation(const CExoString& sName)
{
    CScriptLocation lLocation;
    CSWSScriptVar* pVar = MatchIndex(sName, SCRIPTVAR_TYPE_LOCATION, 0);

    if (pVar != NULL)
    {
        lLocation = *pVar->m_plValue;
    }

    return lLocation;
}
#ifndef _XBOX
void CSWSScriptVarTable::SetInt(const CExoString& sName, int nValue, int nFlag)  // PC 0x005A01F0 GOG 0x005A0260
{
    CSWSScriptVar* pVar = MatchIndex(sName, SCRIPTVAR_TYPE_INT, 1);
    pVar->m_nValue = nValue;

    if (m_pCreature != NULL)
    {
        if (sName.GetLength() > 10 && m_pCreature->GetJournal() != NULL)
        {
            if (sName.Left(10) == "NW_JOURNAL")
            {
                if (sName[11] == 'E')
                {
                    m_pCreature->GetJournal()->SetState(
                        sName.SubString(16, 128), nValue, nFlag);
                }
                else if (sName[11] == 'D')
                {
                    m_pCreature->GetJournal()->SetDate(sName.SubString(15, 128),
                                                       nValue, nFlag);
                }
                else if (sName[11] == 'T')
                {
                    m_pCreature->GetJournal()->SetTime(sName.SubString(15, 128),
                                                       nValue, nFlag);
                }
                else if (sName[11] == 'P')
                {
                    m_pCreature->GetJournal()->SetPicture(
                        sName.SubString(18, 128), nValue, nFlag);
                }
            }
        }
    }
}
// PC 0x005A03D0 GOG 0x005A0440
void CSWSScriptVarTable::SetFloat(const CExoString& sName, float fValue)
{
    MatchIndex(sName, SCRIPTVAR_TYPE_FLOAT, 1)->m_fValue = fValue;
}
#endif
void CSWSScriptVarTable::SetString(const CExoString& sName, CExoString& sValue)  // PC 0x005A03F0 GOG 0x005A0460
{
    CSWSScriptVar* pVar = MatchIndex(sName, SCRIPTVAR_TYPE_STRING, 1);
    *pVar->m_psValue = sValue;
}
// PC 0x005A0410 GOG 0x005A0480
void CSWSScriptVarTable::SetObject(const CExoString& sName,
                                   unsigned long oidValue)
{
    MatchIndex(sName, SCRIPTVAR_TYPE_OBJECT, 1)->m_oidValue = oidValue;
}
// PC 0x005A0430 GOG 0x005A04A0
void CSWSScriptVarTable::SetLocation(const CExoString& sName,
                                     CScriptLocation lLocation)
{
    *MatchIndex(sName, SCRIPTVAR_TYPE_LOCATION, 1)->m_plValue = lLocation;
}
// PC 0x005A0490 GOG 0x005A0500
CSWSScriptVarTable::~CSWSScriptVarTable()
{
    for (int i = 0; i < num; i++)
    {
        if (element[i].m_nType == SCRIPTVAR_TYPE_STRING)
        {
            if (element[i].m_psValue != NULL)
            {
                delete element[i].m_psValue;
            }
        }

        if (element[i].m_nType == SCRIPTVAR_TYPE_LOCATION)
        {
            if (element[i].m_plValue != NULL)
            {
                delete element[i].m_plValue;
            }
        }
    }
}
// PC 0x005A0560 GOG 0x005A05D0
void CSWSScriptVarTable::DestroyFloat(const CExoString& sName)
{
    DeleteIndex(sName, SCRIPTVAR_TYPE_FLOAT);
}
// PC 0x005A0570 GOG 0x005A05E0
void CSWSScriptVarTable::DestroyString(const CExoString& sName)
{
    DeleteIndex(sName, SCRIPTVAR_TYPE_STRING);
}
// PC 0x005A0580 GOG 0x005A05F0
void CSWSScriptVarTable::DestroyObject(const CExoString& sName)
{
    DeleteIndex(sName, SCRIPTVAR_TYPE_OBJECT);
}
// PC 0x005A0590 GOG 0x005A0600
void CSWSScriptVarTable::LoadVarTable(CResGFF* pRes, CResStruct* pStruct)
{
    CScriptLocation lLocation;
    CExoString sName;
    CExoString sString;
    CResList lstVars;
    CResStruct sElement;
    CResStruct sLocation;
    int bSuccess;

    if (pRes->GetList(&lstVars, pStruct, "VarTable"))
    {
        {
            for (unsigned long i = 0; i < pRes->GetListCount(&lstVars); i++)
            {
                pRes->GetListElement(&sElement, &lstVars, i);
                sName =
                    pRes->ReadFieldCExoString(&sElement, "Name", bSuccess, "");

                switch (pRes->ReadFieldDWORD(&sElement, "Type", bSuccess, 0))
                {
                case SCRIPTVAR_TYPE_FLOAT:
                {
                    float fValue = pRes->ReadFieldFLOAT(&sElement, "Value",
                                                        bSuccess, 0.0f);
                    MatchIndex(sName, SCRIPTVAR_TYPE_FLOAT, 1)->m_fValue =
                        fValue;
                    break;
                }
                case SCRIPTVAR_TYPE_INT:
                {
                    int nValue =
                        pRes->ReadFieldINT(&sElement, "Value", bSuccess, 0);
                    SetInt(sName, nValue, 1);
                    break;
                }
                case SCRIPTVAR_TYPE_LOCATION:
                {
                    pRes->GetStructFromStruct(&sLocation, &sElement, "Value");
                    lLocation.LoadLocation(pRes, &sLocation);
                    SetLocation(sName, lLocation);
                    break;
                }
                case SCRIPTVAR_TYPE_OBJECT:
                {
                    unsigned long oidValue =
                        pRes->ReadFieldDWORD(&sElement, "Value", bSuccess, 0);
                    MatchIndex(sName, SCRIPTVAR_TYPE_OBJECT, 1)->m_oidValue =
                        oidValue;
                    break;
                }
                case SCRIPTVAR_TYPE_STRING:
                {
                    sString = pRes->ReadFieldCExoString(&sElement, "Value",
                                                        bSuccess, "");
                    CSWSScriptVar* pVar =
                        MatchIndex(sName, SCRIPTVAR_TYPE_STRING, 1);
                    *pVar->m_psValue = sString;
                    break;
                }
                default:
                    break;
                }
            }
        }
    }
}

#ifdef _XBOX

void CSWSScriptVarTable::SetInt(const CExoString& sName, int nValue, int nFlag)
{
    MatchIndex(sName, SCRIPTVAR_TYPE_INT, 1)->m_nValue = nValue;

    if (m_pCreature != NULL)
    {
        if (sName.GetLength() > 10)
        {
            if (m_pCreature->GetJournal() != NULL && sName.Left(10) == "NW_JOURNAL")
            {
                if (sName[11] == 'E')
                {
                    m_pCreature->GetJournal()->SetState(
                        sName.SubString(16, 128), nValue, nFlag);
                }
                else if (sName[11] == 'D')
                {
                    m_pCreature->GetJournal()->SetDate(sName.SubString(15, 128),
                                                       nValue, nFlag);
                }
                else if (sName[11] == 'T')
                {
                    m_pCreature->GetJournal()->SetTime(sName.SubString(15, 128),
                                                       nValue, nFlag);
                }
                else if (sName[11] == 'P')
                {
                    m_pCreature->GetJournal()->SetPicture(
                        sName.SubString(18, 128), nValue, nFlag);
                }
            }
        }
    }
}

void CSWSScriptVarTable::SetFloat(const CExoString& sName, float fValue)
{
    MatchIndex(sName, SCRIPTVAR_TYPE_FLOAT, 1)->m_nValue = *(int*)&fValue;
}
#endif
