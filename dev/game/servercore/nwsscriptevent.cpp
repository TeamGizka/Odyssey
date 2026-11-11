#include "exobase.h"
#include "resgff.h"
#include "nwsscriptevent.h"
#include "gameobjectarray.h"
// PC 0x004D6480  Xbox 0x00221360
int CScriptEvent::GetInteger(int nIndex)
{
    if (nIndex < m_lstIntegers.num)
        return m_lstIntegers[nIndex];
    return 0;
}
// PC 0x004D64C0
unsigned long CScriptEvent::GetObjectID(int nIndex)
{
    if (nIndex < m_lstObjectIDs.num)
        return m_lstObjectIDs[nIndex];
    return OBJECT_INVALID;
}
// PC 0x004D7360
CExoString CScriptEvent::GetString(int nIndex)
{
    if (nIndex < m_lstStrings.num)
        return m_lstStrings[nIndex];
    return "";
}
// PC 0x004D64A0  Xbox 0x00221380
float CScriptEvent::GetFloat(int nIndex)
{
    if (nIndex < m_lstFloats.num)
    {
        return m_lstFloats[nIndex];
    }
    else
    {
        return 0.0;
    }
}
// PC 0x004D6AE0
void CScriptEvent::SetString(int nIndex, const CExoString& sValue)
{
    if (nIndex >= m_lstStrings.num)
    {
        m_lstStrings.SetSize(nIndex + 1);
    }

    m_lstStrings[nIndex] = sValue;
}
// PC 0x004D7270  Xbox 0x00221270
int CScriptEvent::operator==(const CScriptEvent& sev) const
{
    int i;

    if (m_lstIntegers.num != sev.m_lstIntegers.num)
    {
        return FALSE;
    }

    for (i = 0; i < m_lstIntegers.num; i++)
    {
        if (m_lstIntegers.element[i] != sev.m_lstIntegers.element[i])
        {
            return FALSE;
        }
    }

    if (m_lstFloats.num != sev.m_lstFloats.num)
    {
        return FALSE;
    }

    for (i = 0; i < m_lstFloats.num; i++)
    {
        if (m_lstFloats.element[i] != sev.m_lstFloats.element[i])
        {
            return FALSE;
        }
    }

    if (m_lstObjectIDs.num != sev.m_lstObjectIDs.num)
    {
        return FALSE;
    }

    for (i = 0; i < m_lstObjectIDs.num; i++)
    {
        if (m_lstObjectIDs.element[i] != sev.m_lstObjectIDs.element[i])
        {
            return FALSE;
        }
    }

    if (m_lstStrings.num != sev.m_lstStrings.num)
    {
        return FALSE;
    }

    for (i = 0; i < m_lstStrings.num; i++)
    {
        if (m_lstStrings.element[i] != sev.m_lstStrings.element[i])
        {
            return FALSE;
        }
    }

    return TRUE;
}
// PC 0x004D73A0  Xbox 0x002213A0
int CScriptEvent::SaveEvent(CResGFF* pRes, CResStruct* pStruct)
{
    CResStruct* pTopStruct;
    CResList cList;
    CResStruct sElement;
    int i;

    pTopStruct = pStruct;
    pRes->WriteFieldWORD(pStruct, m_nType, "EventType");

    if (m_lstIntegers.num > 0)
    {
        pRes->AddList(&cList, pTopStruct, "IntList");
        for (i = 0; i < m_lstIntegers.num; i++)
        {
            pRes->AddListElement(&sElement, &cList, 0x69);
            pRes->WriteFieldINT(&sElement, m_lstIntegers[i], "Parameter");
        }
    }

    if (m_lstFloats.num > 0)
    {
        pRes->AddList(&cList, pTopStruct, "FloatList");
        for (i = 0; i < m_lstFloats.num; i++)
        {
            pRes->AddListElement(&sElement, &cList, 0x69);
            pRes->WriteFieldFLOAT(&sElement, m_lstFloats[i], "Parameter");
        }
    }

    if (m_lstStrings.num > 0)
    {
        pRes->AddList(&cList, pTopStruct, "StringList");
        for (i = 0; i < m_lstStrings.num; i++)
        {
            pRes->AddListElement(&sElement, &cList, 0x69);
            pRes->WriteFieldCExoString(&sElement, m_lstStrings[i], "Parameter");
        }
    }

    if (m_lstObjectIDs.num > 0)
    {
        pRes->AddList(&cList, pTopStruct, "ObjectList");
        for (i = 0; i < m_lstObjectIDs.num; i++)
        {
            pRes->AddListElement(&sElement, &cList, 0x69);
            pRes->WriteFieldDWORD(&sElement, m_lstObjectIDs[i], "Parameter");
        }
    }

    return TRUE;
}
// PC 0x004D7540  Xbox 0x00221540
CScriptEvent::CScriptEvent() :
    m_lstIntegers(0), m_lstFloats(0), m_lstStrings(0), m_lstObjectIDs(0)
{
}
// PC 0x004D7590  Xbox 0x00221590
CScriptEvent::~CScriptEvent()
{
    m_lstIntegers.SetSize(0);
    m_lstFloats.SetSize(0);
    m_lstObjectIDs.SetSize(0);
    m_lstStrings.SetSize(0);
}
// PC 0x004D76B0  Xbox 0x002216B0
void CScriptEvent::SetInteger(int nIndex, int nValue)
{
    if (nIndex >= m_lstIntegers.num)
    {
        m_lstIntegers.SetSize(nIndex + 1);
    }

    m_lstIntegers[nIndex] = nValue;
}
// PC 0x004D7720  Xbox 0x00221720
void CScriptEvent::SetFloat(int nIndex, float fValue)
{
    if (nIndex >= m_lstFloats.num)
    {
        m_lstFloats.SetSize(nIndex + 1);
    }

    m_lstFloats[nIndex] = fValue;
}
// PC 0x004D7780  Xbox 0x00221780
void CScriptEvent::SetObjectID(int nIndex, unsigned long oidObject)
{
    if (nIndex >= m_lstObjectIDs.num)
    {
        m_lstObjectIDs.SetSize(nIndex + 1);
    }

    m_lstObjectIDs[nIndex] = oidObject;
}
// PC 0x004D77F0  Xbox 0x002217F0
void CScriptEvent::CopyScriptEvent(CScriptEvent* pEvent)
{
    int i;

    if (pEvent != NULL)
    {
        m_nType = pEvent->m_nType;

        m_lstIntegers.SetSize(pEvent->m_lstIntegers.num);
        for (i = 0; i < pEvent->m_lstIntegers.num; i++)
        {
            m_lstIntegers[i] = pEvent->m_lstIntegers[i];
        }

        m_lstFloats.SetSize(pEvent->m_lstFloats.num);
        for (i = 0; i < pEvent->m_lstFloats.num; i++)
        {
            m_lstFloats[i] = pEvent->m_lstFloats[i];
        }

        m_lstObjectIDs.SetSize(pEvent->m_lstObjectIDs.num);
        for (i = 0; i < pEvent->m_lstObjectIDs.num; i++)
        {
            m_lstObjectIDs[i] = pEvent->m_lstObjectIDs[i];
        }

        m_lstStrings.SetSize(pEvent->m_lstStrings.num);
        for (i = 0; i < pEvent->m_lstStrings.num; i++)
        {
            m_lstStrings[i] = pEvent->m_lstStrings[i];
        }
    }
}
// PC 0x004D79B0  Xbox 0x002219B0
int CScriptEvent::LoadEvent(CResGFF* pRes, CResStruct* pStruct)
{
    CResList cList;
    CResStruct sElement;
    int bSuccess;
    unsigned long i;
    int nValue;
    float fValue;
    unsigned long nOID;

    m_nType = pRes->ReadFieldWORD(pStruct, "EventType", bSuccess, 0);

    if (pRes->GetList(&cList, pStruct, "IntList"))
    {
        for (i = 0; i < pRes->GetListCount(&cList); i++)
        {
            pRes->GetListElement(&sElement, &cList, i);
            nValue = pRes->ReadFieldINT(&sElement, "Parameter", bSuccess, 0);

            m_lstIntegers.Add(nValue);
        }
    }

    if (pRes->GetList(&cList, pStruct, "FloatList"))
    {
        for (i = 0; i < pRes->GetListCount(&cList); i++)
        {
            pRes->GetListElement(&sElement, &cList, i);
            fValue = pRes->ReadFieldFLOAT(&sElement, "Parameter", bSuccess, 0.0);

            m_lstFloats.Add(fValue);
        }
    }

    if (pRes->GetList(&cList, pStruct, "StringList"))
    {
        CExoString sValue;

        for (i = 0; i < pRes->GetListCount(&cList); i++)
        {
            pRes->GetListElement(&sElement, &cList, i);
            sValue = pRes->ReadFieldCExoString(&sElement, "Parameter", bSuccess, "");
            m_lstStrings.Add(sValue);
        }
    }

    if (pRes->GetList(&cList, pStruct, "ObjectList"))
    {
        for (i = 0; i < pRes->GetListCount(&cList); i++)
        {
            pRes->GetListElement(&sElement, &cList, i);
            nOID = pRes->ReadFieldDWORD(&sElement, "Parameter", bSuccess, 0);

            m_lstObjectIDs.Add(nOID);
        }
    }

    return TRUE;
}
