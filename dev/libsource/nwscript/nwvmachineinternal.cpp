#include "nwscript.h"
#include "nwvmachineinternal.h"

#include <string.h>


static inline void VirtualMachineTrace();
#define VIRTUALMACHINE_TRACE() VirtualMachineTrace()


#define VM_CODE_INTEGER(p) ((((p)[0] << 24) & 0xff000000) + (((p)[1] << 16) & 0x00ff0000) + \
    (((p)[2] << 8) & 0x0000ff00) + ((p)[3] & 0x000000ff))
#define VM_CODE_SHORT(p) ((short)((((p)[0] << 8) & 0xff00) + ((p)[1] & 0x00ff)))

// PC 0x005D1C40  Xbox 0x001587C0
void CVirtualMachineStack::AssignLocationToLocation(int nSource, int nDestination)
{
    VIRTUALMACHINE_TRACE();
    CExoString* pSourceString;
    CExoString* pString;

    if (nSource >= 0 && nDestination >= 0 && m_pVirtualMachine != 0)
    {
        if (nDestination == m_nStackPointer)
        {
            AddToTopOfStack(m_pTypes[nSource]);
        }

        if (m_pTypes[nSource] == VM_STACK_STRING)
        {
            delete m_pValues[nDestination].m_pString;
            pSourceString = m_pValues[nSource].m_pString;
            pString = new CExoString();
            *pString = *pSourceString;
            m_pValues[nDestination].m_pString = pString;
        }
        else if (m_pTypes[nSource] >= VM_STACK_ENGINE_STRUCTURE_FIRST &&
                 m_pTypes[nSource] <= VM_STACK_ENGINE_STRUCTURE_LAST)
        {
            if (m_pValues[nDestination].m_pEngineStructure != 0)
            {
                m_pVirtualMachine->m_pCommandImplementer->DestroyGameDefinedStructure(
                    m_pTypes[nSource] - VM_STACK_ENGINE_STRUCTURE_FIRST,
                    m_pValues[nDestination].m_pEngineStructure);
            }
            m_pValues[nDestination].m_pEngineStructure =
                m_pVirtualMachine->m_pCommandImplementer->CopyGameDefinedStructure(
                    m_pTypes[nSource] - VM_STACK_ENGINE_STRUCTURE_FIRST,
                    m_pValues[nSource].m_pEngineStructure);
        }
        else
        {
            m_pValues[nDestination] = m_pValues[nSource];
        }
        m_pTypes[nDestination] = m_pTypes[nSource];
    }
}
// PC 0x005D1D80  Xbox 0x00158900
int CVirtualMachineStack::SaveStack(CResGFF* pGFF, CResStruct* pStruct)
{
    VIRTUALMACHINE_TRACE();
    CResList cStackList;
    CResStruct cElement;
    int nLocation;

    pGFF->WriteFieldINT(pStruct, m_nBasePointer, "BasePointer");
    pGFF->WriteFieldINT(pStruct, m_nStackPointer, "StackPointer");
    pGFF->WriteFieldINT(pStruct, m_nAllocated, "TotalSize");
    if (m_nAllocated > 0)
    {
        pGFF->AddList(&cStackList, pStruct, "Stack");
        for (nLocation = 0; nLocation < m_nStackPointer; nLocation++)
        {
            pGFF->AddListElement(&cElement, &cStackList, nLocation);
            pGFF->WriteFieldCHAR(&cElement, m_pTypes[nLocation], "Type");
            switch (m_pTypes[nLocation])
            {
            case VM_STACK_INTEGER:
                pGFF->WriteFieldINT(&cElement,
                    m_pValues[nLocation].m_nInteger, "Value");
                break;
            case VM_STACK_FLOAT:
                pGFF->WriteFieldFLOAT(&cElement,
                    m_pValues[nLocation].m_fFloat, "Value");
                break;
            case VM_STACK_STRING:
                pGFF->WriteFieldCExoString(&cElement,
                    *m_pValues[nLocation].m_pString, "Value");
                break;
            case VM_STACK_OBJECT:
                pGFF->WriteFieldDWORD(&cElement,
                    m_pValues[nLocation].m_oidObject, "Value");
                break;
            default:
                m_pVirtualMachine->m_pCommandImplementer->SaveGameDefinedStructure(
                    m_pTypes[nLocation] - VM_STACK_ENGINE_STRUCTURE_FIRST,
                    m_pValues[nLocation].m_pEngineStructure, pGFF, &cElement);
                break;
            }
        }
    }

    return 1;
}
// PC 0x005D1EF0  Xbox 0x00158A70
int CVirtualMachineStack::LoadStack(CResGFF* pGFF, CResStruct* pStruct)
{
    VIRTUALMACHINE_TRACE();
    CResList cStackList;
    CResStruct cElement;
    int bSuccess;
    int nLocation;
    CExoString* pString;

    m_nBasePointer = pGFF->ReadFieldINT(pStruct, "BasePointer", bSuccess, 0);
    m_nStackPointer = pGFF->ReadFieldINT(pStruct, "StackPointer", bSuccess, 0);
    m_nAllocated = pGFF->ReadFieldINT(pStruct, "TotalSize", bSuccess, 0);
    if (m_nAllocated > 0)
    {
        m_pTypes = new char[m_nAllocated];
        m_pValues = new CVirtualMachineStackValue[m_nAllocated];
        pGFF->GetList(&cStackList, pStruct, "Stack");
        for (nLocation = 0; nLocation < m_nStackPointer; nLocation++)
        {
            pGFF->GetListElement(&cElement, &cStackList, nLocation);
            m_pTypes[nLocation] =
                pGFF->ReadFieldCHAR(&cElement, "Type", bSuccess, 0);
            switch (m_pTypes[nLocation])
            {
            case VM_STACK_INTEGER:
                m_pValues[nLocation].m_nInteger =
                    pGFF->ReadFieldINT(&cElement, "Value", bSuccess, 0);
                break;
            case VM_STACK_FLOAT:
                {
                    float fValue = pGFF->ReadFieldFLOAT(&cElement, "Value",
                                                        bSuccess, 0.0f);
                    memcpy(&m_pValues[nLocation].m_nInteger, &fValue,
                           sizeof(fValue));
                }
                break;
            case VM_STACK_STRING:
                pString = new CExoString();
                *pString = pGFF->ReadFieldCExoString(&cElement, "Value",
                                                     bSuccess, "");
                m_pValues[nLocation].m_pString = pString;
                break;
            case VM_STACK_OBJECT:
                m_pValues[nLocation].m_oidObject =
                    pGFF->ReadFieldDWORD(&cElement, "Value", bSuccess, 0);
                break;
            default:
                m_pVirtualMachine->m_pCommandImplementer->LoadGameDefinedStructure(
                    m_pTypes[nLocation] - VM_STACK_ENGINE_STRUCTURE_FIRST,
                    &m_pValues[nLocation].m_pEngineStructure, pGFF, &cElement);
                break;
            }
        }
    }

    return 1;
}
// PC 0x005D2140  Xbox 0x00158CC0
void CVirtualMachineStack::ClearStack()
{
    VIRTUALMACHINE_TRACE();
    int nLocation;

    if (m_pTypes != 0 && m_pValues != 0)
    {
        for (nLocation = m_nStackPointer - 1; nLocation >= 0; nLocation--)
        {
            if (m_pTypes[nLocation] == VM_STACK_STRING)
            {
                delete m_pValues[nLocation].m_pString;
            }
            else if (m_pTypes[nLocation] >= VM_STACK_ENGINE_STRUCTURE_FIRST &&
                     m_pTypes[nLocation] <= VM_STACK_ENGINE_STRUCTURE_LAST)
            {
                m_pVirtualMachine->m_pCommandImplementer->DestroyGameDefinedStructure(
                    m_pTypes[nLocation] - VM_STACK_ENGINE_STRUCTURE_FIRST,
                    m_pValues[nLocation].m_pEngineStructure);
            }

            m_pValues[nLocation].m_pEngineStructure = 0;
        }
    }

    m_nBasePointer = 0;
    m_nStackPointer = 0;
    m_nAllocated = 0;
    if (m_pTypes != 0)
    {
        delete[] m_pTypes;
        m_pTypes = 0;
    }

    if (m_pValues != 0)
    {
        delete[] m_pValues;
        m_pValues = 0;
    }
}
// PC 0x005D15A0  Xbox 0x00158690
void CVirtualMachineStack::AddToTopOfStack(int nType)
{
    CVirtualMachineStackValue* pValues;
    char* pTypes;

    if (m_nStackPointer == m_nAllocated)
    {
        m_nAllocated += 256;
        pValues = new CVirtualMachineStackValue[m_nAllocated];
        pTypes = new char[m_nAllocated];
        if (m_nStackPointer != 0)
        {
            memcpy(pValues, m_pValues,
                   sizeof(CVirtualMachineStackValue) * m_nStackPointer);
            memcpy(pTypes, m_pTypes, m_nStackPointer);
        }

        if (m_pTypes != 0)
        {
            delete[] m_pTypes;
        }

        if (m_pValues != 0)
        {
            delete[] m_pValues;
        }

        m_pValues = pValues;
        m_pTypes = pTypes;
    }

    if (m_nStackPointer >= 0)
    {
        m_pTypes[m_nStackPointer] = (char)nType;
        if (nType == VM_STACK_STRING)
        {
            m_pValues[m_nStackPointer++].m_pString = 0;
        }
        else if (nType >= VM_STACK_ENGINE_STRUCTURE_FIRST &&
                 nType <= VM_STACK_ENGINE_STRUCTURE_LAST)
        {
            if (m_pVirtualMachine != 0)
            {
                m_pValues[m_nStackPointer++].m_pEngineStructure = 0;
            }
        }
        else if (nType == VM_STACK_INTEGER)
        {
            m_pValues[m_nStackPointer++].m_nInteger = 0;
        }
        else if (nType == VM_STACK_FLOAT)
        {
            CVirtualMachineStackValue cValue;
            cValue.m_fFloat = 0.0f;
            m_pValues[m_nStackPointer++] = cValue;
        }
        else if (nType == VM_STACK_OBJECT)
        {
            m_pValues[m_nStackPointer++].m_oidObject = OBJECT_INVALID;
        }
    }
}
// PC 0x005D16C0  Xbox 0x001581C0
void CVirtualMachineStack::ModifyIntegerAtLocation(int nLocation, int nValue)
{
    if (nLocation >= 0 && m_pVirtualMachine != 0 &&
        m_pTypes[nLocation] == VM_STACK_INTEGER)
    {
        m_pValues[nLocation].m_nInteger += nValue;
    }
}
// PC 0x005D16F0  Xbox 0x00158270
void CVirtualMachineInternal::InitializeScript(CVirtualMachineScript* pScript,
                                               char* pCode, int nCodeSize)
{
    char* pNewCode;

    pScript->m_bLoaded = 0;
    pScript->m_pStack = 0;
    pScript->m_nStackSize = 0;
    pScript->m_nInstructionPtr = 13;
    pScript->m_nSecondaryPtr = 0;
    pScript->m_nCodeSize = nCodeSize;
#ifdef _XBOX
    pScript->m_pCode = pCode;
#else
    pNewCode = new char[nCodeSize];
    pScript->m_pCode = pNewCode;
    memcpy(pNewCode, pCode, nCodeSize);
    pScript->m_nObjectId = 0;
#endif
}
// PC 0x005D1740  Xbox 0x00158540
inline CVirtualMachineScript::CVirtualMachineScript()
{
    m_sName = "";
    m_pStack = 0;
    m_nStackSize = 0;
    m_nInstructionPtr = 0;
    m_nSecondaryPtr = 0;
    m_pCode = 0;
    m_nCodeSize = 0;
    m_bLoaded = 0;
    m_nObjectId = 0;
}
// PC 0x005D17B0  Xbox 0x001582A0
int CVirtualMachineInternal::PopInstructionPtr(int* pnInstructionPtr)
{
    if (--m_nReturnStackPointer < 0)
    {
        m_nReturnStackPointer = 0;
        return 0;
    }

    *pnInstructionPtr = m_anReturnStack[m_nReturnStackPointer];
    return 1;
}
// PC 0x005D17F0  Xbox 0x001582E0
int CVirtualMachineInternal::PushInstructionPtr(int nInstructionPtr)
{
    m_anReturnStack[m_nReturnStackPointer] = nInstructionPtr;
    m_nReturnStackPointer++;
    return m_nReturnStackPointer < 128;
}
// PC 0x005D1820
int CVirtualMachineInternal::GetRunScriptReturnValue(int* pnType,
                                                     void** ppValue)
{
    if (m_nReturnType != VM_STACK_INTEGER)
    {
        return 0;
    }

    *pnType = VM_STACK_INTEGER;
    *ppValue = (void*)m_nReturnValue;
    return 1;
}
// PC 0x005D1850  Xbox 0x00159060
int CVirtualMachineInternal::StackPushInteger(int nValue)
{
    CVirtualMachineStack* pStack;

    pStack = &m_cStack;
    pStack->AddToTopOfStack(VM_STACK_INTEGER);
    m_cStack.m_pValues[m_cStack.m_nStackPointer - 1].m_nInteger = nValue;
    return 1;
}
// PC 0x005D1880  Xbox 0x001590E0
int CVirtualMachineInternal::StackPushFloat(float fValue)
{
    CVirtualMachineStack* pStack;
    int nStackPointer;

    pStack = &m_cStack;
    pStack->AddToTopOfStack(VM_STACK_FLOAT);
    nStackPointer = m_cStack.m_nStackPointer - 1;
    memcpy(&m_cStack.m_pValues[nStackPointer].m_nInteger, &fValue,
           sizeof(fValue));
    return 1;
}
// PC 0x005D18B0  Xbox 0x001591A0
int CVirtualMachineInternal::StackPushVector(Vector vValue)
{
    float fValue;

    fValue = vValue.m_fX;
    StackPushFloat(fValue);
    fValue = vValue.m_fY;
    StackPushFloat(fValue);
    fValue = vValue.m_fZ;
    StackPushFloat(fValue);
    return 1;
}
// PC 0x005D1930  Xbox 0x00159280
int CVirtualMachineInternal::StackPushString(const CExoString& sValue)
{
    CExoString* pString;

    pString = new CExoString(sValue.CStr());
    m_cStack.AddToTopOfStack(VM_STACK_STRING);
    m_cStack.m_pValues[m_cStack.m_nStackPointer - 1].m_pString = pString;
    return 1;
}
// PC 0x005D19C0  Xbox 0x00159380
int CVirtualMachineInternal::StackPushEngineStructure(int nType, void* pValue)
{
    void* pStructure;

    if (m_pCommandImplementer == 0)
    {
        return 0;
    }

    pStructure = m_pCommandImplementer->CopyGameDefinedStructure(nType, pValue);
    m_cStack.AddToTopOfStack(nType + VM_STACK_ENGINE_STRUCTURE_FIRST);
    m_cStack.m_pValues[m_cStack.m_nStackPointer - 1].m_pEngineStructure =
        pStructure;
    return 1;
}
// PC 0x005D1A10  Xbox 0x00159430
int CVirtualMachineInternal::StackPushObject(unsigned long nValue)
{
    CVirtualMachineStack* pStack;

    pStack = &m_cStack;
    pStack->AddToTopOfStack(VM_STACK_OBJECT);
    m_cStack.m_pValues[m_cStack.m_nStackPointer - 1].m_oidObject = nValue;
    return 1;
}
// PC 0x005D1A40  Xbox 0x00158340
void CVirtualMachineInternal::SetCommandImplementer(
    CVirtualMachineCmdImplementer* pImplementer)
{
    m_pCommandImplementer = pImplementer;
    if (pImplementer != 0)
    {
        pImplementer->InitializeCommands();
    }
}
// PC 0x005D21F0  Xbox 0x00158D70
void CVirtualMachineStack::SetStackPointer(int nStackPointer)
{
    VIRTUALMACHINE_TRACE();
    int nLocation;

    if (nStackPointer < m_nStackPointer)
    {
        for (nLocation = m_nStackPointer - 1; nLocation >= nStackPointer;
             nLocation--)
        {
            if (m_pTypes[nLocation] == VM_STACK_STRING)
            {
                if (m_pValues[nLocation].m_pString != 0)
                {
                    delete m_pValues[nLocation].m_pString;
                }
            }
            else if (m_pTypes[nLocation] >= VM_STACK_ENGINE_STRUCTURE_FIRST &&
                     m_pTypes[nLocation] <= VM_STACK_ENGINE_STRUCTURE_LAST)
            {
                m_pVirtualMachine->m_pCommandImplementer->DestroyGameDefinedStructure(
                    m_pTypes[nLocation] - VM_STACK_ENGINE_STRUCTURE_FIRST,
                    m_pValues[nLocation].m_pEngineStructure);
            }
        }

        m_nStackPointer = nStackPointer;
    }
}
#ifndef _XBOX
int CVirtualMachineInternal::ReadScriptFile(CExoString* psScript)  // PC 0x005D2260  Xbox 0x00158DE0
{
    char* pNCSData;
    unsigned long nNCSSize;
    char cVersion;
    char cRevision;
    int nVersion;

    m_nCurrentScript++;
    if (m_nCurrentScript >= 8)
    {
        m_nCurrentScript--;
        return -94;
    }

#ifdef _XBOX
    int nResult = m_aVMFile[m_nCurrentScript].LoadFile(psScript, &pNCSData,
                                                       &nNCSSize);
    if (nResult < 0)
    {
        m_nCurrentScript--;
        return nResult;
    }
#else
    if (psScript == 0)
    {
        CResRef cResRef("");

        SetResRef(cResRef, 0);
    }
    else
    {
        CResRef cResRef(*psScript);

        SetResRef(cResRef, 0);
    }
    if (m_pRes->Demand() == 0)
    {
        m_nCurrentScript--;
        return -95;
    }

    pNCSData = m_pRes->GetNCSDataPtr();
    nNCSSize = m_pRes->GetNCSSize();
#endif
    m_aScripts[m_nCurrentScript].m_sName = *psScript;
    if (pNCSData[0] != 'N' || pNCSData[1] != 'C' || pNCSData[2] != 'S' ||
        pNCSData[3] != ' ' || pNCSData[4] != 'V' || pNCSData[6] != '.')
    {
#ifdef _XBOX
        m_aVMFile[m_nCurrentScript].m_pRes->Release();
        m_nCurrentScript--;
#else
        m_nCurrentScript--;
        m_pRes->Release();
#endif
        return -96;
    }

    cVersion = pNCSData[5];
    nVersion = 0;
    if (cVersion >= '1' && cVersion <= '9')
    {
        nVersion = 10 * (cVersion - '0');
    }

    cRevision = pNCSData[7];
    if (cRevision >= '1' && cRevision <= '9')
    {
        nVersion += cRevision - '0';
    }

#ifdef _XBOX
    if (nVersion != 10)
    {
        m_aVMFile[m_nCurrentScript].m_pRes->Release();
        m_nCurrentScript--;
        return -96;
    }

    if (pNCSData[8] == 'B')
#else
    if (nVersion == 10 && pNCSData[8] == 'B')
#endif
    {
        InitializeScript(&m_aScripts[m_nCurrentScript], pNCSData + 13,
                         nNCSSize - 13);
#ifndef _XBOX
        m_pRes->Release();
        {
            CResRef cResRef("");

            SetResRef(cResRef, 0);
        }
#endif
        return 0;
    }

#ifdef _XBOX
    m_aVMFile[m_nCurrentScript].m_pRes->Release();
    m_nCurrentScript--;
#else
    m_nCurrentScript--;
    m_pRes->Release();
#endif
    return -96;
}
#endif
int CVirtualMachineInternal::SetUpScriptSituation(  // PC 0x005D23E0  Xbox 0x00158F10
    CVirtualMachineScript* pScript)
{
    int nCRC;
    int nResult;

    nCRC = pScript->m_bLoaded;
    if (pScript->m_pCode == 0)
    {
        nResult = ReadScriptFile(&pScript->m_sName);
        if (nResult != 0)
        {
            return nResult;
        }

        nCRC = 0;
    }
    else
    {
        m_nCurrentScript++;
        if (m_nCurrentScript >= 8)
        {
            m_nCurrentScript--;
            return -94;
        }
    }

    m_aScripts[m_nCurrentScript].m_bLoaded = nCRC;
    m_aScripts[m_nCurrentScript].m_sName = pScript->m_sName;
    m_aScripts[m_nCurrentScript].m_pStack = pScript->m_pStack;
    m_aScripts[m_nCurrentScript].m_nStackSize = pScript->m_nStackSize;
    m_aScripts[m_nCurrentScript].m_nInstructionPtr =
        pScript->m_nInstructionPtr;
    m_aScripts[m_nCurrentScript].m_nSecondaryPtr = pScript->m_nSecondaryPtr;
    m_aScripts[m_nCurrentScript].m_nObjectId = pScript->m_nObjectId;
    if (pScript->m_pCode != 0)
    {
        m_aScripts[m_nCurrentScript].m_pCode = pScript->m_pCode;
        m_aScripts[m_nCurrentScript].m_nCodeSize = pScript->m_nCodeSize;
    }

    pScript->m_pStack = 0;
    pScript->m_pCode = 0;
    return 0;
}
// PC 0x005D24C0  Xbox 0x00159010
int CVirtualMachineInternal::StackPopInteger(int* pnValue)
{
    int nStackPointer;

    nStackPointer = m_cStack.m_nStackPointer;
    if (nStackPointer <= 0)
    {
        return 0;
    }

    nStackPointer--;
    if (m_cStack.m_pTypes[nStackPointer] != VM_STACK_INTEGER)
    {
        return 0;
    }

    *pnValue = m_cStack.m_pValues[nStackPointer].m_nInteger;
    m_cStack.SetStackPointer(nStackPointer);
    return 1;
}
// PC 0x005D2510  Xbox 0x00159090
int CVirtualMachineInternal::StackPopFloat(float* pfValue)
{
    int nStackPointer;

    nStackPointer = m_cStack.m_nStackPointer;
    if (nStackPointer <= 0)
    {
        return 0;
    }

    nStackPointer--;
    if (m_cStack.m_pTypes[nStackPointer] != VM_STACK_FLOAT)
    {
        return 0;
    }

    *pfValue = m_cStack.m_pValues[nStackPointer].m_fFloat;
    m_cStack.SetStackPointer(nStackPointer);
    return 1;
}
// PC 0x005D2560  Xbox 0x00159110
int CVirtualMachineInternal::StackPopVector(Vector* pvValue)
{
    float fValue;
    int nResult;
    int nStackPointer;

    nStackPointer = m_cStack.m_nStackPointer;
    if (nStackPointer <= 0)
    {
        return 0;
    }

    nStackPointer--;
    if (m_cStack.m_pTypes[nStackPointer] != VM_STACK_FLOAT)
    {
        return 0;
    }

    fValue = m_cStack.m_pValues[nStackPointer].m_fFloat;
    m_cStack.SetStackPointer(nStackPointer);
    pvValue->m_fZ = fValue;
    nResult = StackPopFloat(&fValue);
    if (nResult == 1)
    {
        pvValue->m_fY = fValue;
        nResult = StackPopFloat(&fValue);
        if (nResult == 1)
        {
            pvValue->m_fX = fValue;
        }
    }

    return nResult;
}
// PC 0x005D25E0  Xbox 0x00159230
int CVirtualMachineInternal::StackPopString(CExoString* psValue)
{
    int nStackPointer;

    nStackPointer = m_cStack.m_nStackPointer;
    if (nStackPointer <= 0)
    {
        return 0;
    }

    nStackPointer--;
    if (m_cStack.m_pTypes[nStackPointer] != VM_STACK_STRING)
    {
        return 0;
    }

    *psValue = *m_cStack.m_pValues[nStackPointer].m_pString;
    m_cStack.SetStackPointer(nStackPointer);
    return 1;
}
// PC 0x005D2630  Xbox 0x00159310
int CVirtualMachineInternal::StackPopEngineStructure(int nType, void** ppValue)
{
    CVirtualMachineStack* pStack;
    void* pStructure;
    int nStackPointer;

    pStack = &m_cStack;
    nStackPointer = pStack->m_nStackPointer;
    if (nStackPointer <= 0)
    {
        return 0;
    }

    nStackPointer--;
    if (m_cStack.m_pTypes[nStackPointer] !=
        nType + VM_STACK_ENGINE_STRUCTURE_FIRST)
    {
        return 0;
    }

    pStructure = m_cStack.m_pValues[nStackPointer].m_pEngineStructure;
    if (m_pCommandImplementer == 0)
    {
        return 0;
    }

    *ppValue = m_pCommandImplementer->CopyGameDefinedStructure(nType, pStructure);
    pStack->SetStackPointer(nStackPointer);
    return 1;
}
// PC 0x005D26A0  Xbox 0x001593E0
int CVirtualMachineInternal::StackPopObject(unsigned long* pnValue)
{
    int nStackPointer;

    nStackPointer = m_cStack.m_nStackPointer;
    if (nStackPointer <= 0)
    {
        return 0;
    }

    nStackPointer--;
    if (m_cStack.m_pTypes[nStackPointer] != VM_STACK_OBJECT)
    {
        return 0;
    }

    *pnValue = m_cStack.m_pValues[nStackPointer].m_nInteger;
    m_cStack.SetStackPointer(nStackPointer);
    return 1;
}
#ifndef _XBOX
int CVirtualMachineInternal::LoadScriptSituation(  // PC 0x005D26F0  Xbox 0x00159460
    CVirtualMachineScript** ppScript, CResGFF* pGFF, CResStruct* pStruct)
{
    int bSuccess;
    CResStruct cStackStruct;

    *ppScript = new CVirtualMachineScript;
    (*ppScript)->m_nCodeSize =
        pGFF->ReadFieldINT(pStruct, "CodeSize", bSuccess, 0);
    if ((*ppScript)->m_nCodeSize != 0)
    {
        (*ppScript)->m_pCode = new char[(*ppScript)->m_nCodeSize];
    }
    else
    {
        (*ppScript)->m_pCode = 0;
    }

    pGFF->ReadFieldVOID(pStruct, (*ppScript)->m_pCode,
                       (*ppScript)->m_nCodeSize, "Code", bSuccess, 0);
    (*ppScript)->m_nObjectId =
        pGFF->ReadFieldDWORD(pStruct, "CRC", bSuccess, 0);
    if (!bSuccess)
    {
        (*ppScript)->m_nObjectId = 0;
    }

    (*ppScript)->m_bLoaded = 1;
    (*ppScript)->m_nInstructionPtr =
        pGFF->ReadFieldINT(pStruct, "InstructionPtr", bSuccess, 0);
    (*ppScript)->m_nSecondaryPtr =
        pGFF->ReadFieldINT(pStruct, "SecondaryPtr", bSuccess, 0);
    (*ppScript)->m_sName =
        pGFF->ReadFieldCExoString(pStruct, "Name", bSuccess, "");
    (*ppScript)->m_nStackSize =
        pGFF->ReadFieldINT(pStruct, "StackSize", bSuccess, 0);
    pGFF->GetStructFromStruct(&cStackStruct, pStruct, "Stack");
    (*ppScript)->m_pStack = new CVirtualMachineStack;
    (*ppScript)->m_pStack->m_pVirtualMachine = this;
    (*ppScript)->m_pStack->LoadStack(pGFF, &cStackStruct);
    return 1;
}
#endif
inline CVirtualMachineStack::~CVirtualMachineStack()  // PC 0x005D28E0  Xbox 0x00159620
{
    ClearStack();
}
// PC 0x005D28F0  Xbox 0x00159630
void CVirtualMachineStack::CopyFromStack(CVirtualMachineStack* pStack,
                                        int nStackSize, int nBaseSize)
{
    int nAllocated;
    int nLocation;
    int nSource;
    CExoString* pSourceString;
    CExoString* pStackSourceString;
    CExoString* pString;

    ClearStack();
    delete[] m_pTypes;
    delete[] m_pValues;
    m_pTypes = 0;
    m_pValues = 0;
    m_nAllocated = 0;
    m_nBasePointer = 0;
    m_nStackPointer = 0;

    nStackSize >>= 2;
    nBaseSize >>= 2;
    if (nStackSize == 0 && nBaseSize == 0)
    {
        nStackSize = pStack->m_nStackPointer - pStack->m_nBasePointer;
        nBaseSize = pStack->m_nBasePointer;
    }

    if (nBaseSize > VM_STACK_MAX_COPY_ENTRIES)
        nBaseSize = VM_STACK_MAX_COPY_ENTRIES;
    else if (nBaseSize < 0)
        nBaseSize = 0;
    if (pStack->m_nBasePointer < nBaseSize)
        nBaseSize = pStack->m_nBasePointer;

    if (nStackSize > VM_STACK_MAX_COPY_ENTRIES)
        nStackSize = VM_STACK_MAX_COPY_ENTRIES;
    else if (nStackSize < 0)
        nStackSize = 0;
    if (pStack->m_nBasePointer < nBaseSize)
        nBaseSize = pStack->m_nBasePointer;

    nAllocated = nStackSize + nBaseSize;
    if (nAllocated != 0)
    {
        nAllocated += VM_STACK_ALLOCATION_INCREMENT;
        m_pTypes = new char[nAllocated];
        m_pValues = new CVirtualMachineStackValue[nAllocated];
        m_nStackPointer = nStackSize + nBaseSize;
        m_nAllocated = nAllocated;
        m_nBasePointer = nBaseSize;
        memcpy(m_pTypes + nBaseSize,
               pStack->m_pTypes + pStack->m_nStackPointer - nStackSize,
               nStackSize);
        memcpy(m_pTypes,
               pStack->m_pTypes + pStack->m_nBasePointer - nBaseSize,
               nBaseSize);

        for (nLocation = 0; nLocation < nBaseSize; nLocation++)
        {
            nSource = pStack->m_nBasePointer - nBaseSize + nLocation;
            if (m_pTypes[nLocation] == VM_STACK_STRING)
            {
                pSourceString = pStack->m_pValues[nSource].m_pString;
                pString = new CExoString();
                *pString = *pSourceString;
                m_pValues[nLocation].m_pString = pString;
            }
            else if (m_pTypes[nLocation] >= VM_STACK_ENGINE_STRUCTURE_FIRST &&
                     m_pTypes[nLocation] <= VM_STACK_ENGINE_STRUCTURE_LAST)
            {
                m_pValues[nLocation].m_pEngineStructure =
                    m_pVirtualMachine->m_pCommandImplementer->CopyGameDefinedStructure(
                        pStack->m_pTypes[nSource] - VM_STACK_ENGINE_STRUCTURE_FIRST,
                        pStack->m_pValues[nSource].m_pEngineStructure);
            }
            else
            {
                m_pValues[nLocation] = pStack->m_pValues[nSource];
            }
        }

        for (nLocation = 0; nLocation < nStackSize; nLocation++)
        {
            nSource = pStack->m_nStackPointer - nStackSize + nLocation;
            if (m_pTypes[m_nBasePointer + nLocation] == VM_STACK_STRING)
            {
                pStackSourceString = pStack->m_pValues[nSource].m_pString;
                pString = new CExoString();
                *pString = *pStackSourceString;
                m_pValues[m_nBasePointer + nLocation].m_pString = pString;
            }
            else if (m_pTypes[m_nBasePointer + nLocation] >= VM_STACK_ENGINE_STRUCTURE_FIRST &&
                     m_pTypes[m_nBasePointer + nLocation] <= VM_STACK_ENGINE_STRUCTURE_LAST)
            {
                m_pValues[m_nBasePointer + nLocation].m_pEngineStructure =
                    m_pVirtualMachine->m_pCommandImplementer->CopyGameDefinedStructure(
                        pStack->m_pTypes[nSource] - VM_STACK_ENGINE_STRUCTURE_FIRST,
                        pStack->m_pValues[nSource].m_pEngineStructure);
            }
            else
            {
                m_pValues[m_nBasePointer + nLocation] = pStack->m_pValues[nSource];
            }
        }
    }
}
// PC 0x005D2BD0  Xbox 0x00159900
int CVirtualMachineInternal::ExecuteCode(int* pnInstructionPtr, char* pCode,
                                         int nCodeSize)
{
    for (;;)
    {
        if (*pnInstructionPtr == -1)
        {
            break;
        }
        if (*pnInstructionPtr + 1 >= nCodeSize)
        {
            return VM_ERROR_INVALID_CODE;
        }
        if (++m_nInstructionCount >= VM_MAX_EXECUTED_INSTRUCTIONS)
        {
            return VM_ERROR_TOO_MANY_INSTRUCTIONS;
        }
        char* pInstruction = pCode + *pnInstructionPtr;
        unsigned char nOpcode = pInstruction[0];
        unsigned char nType = pInstruction[1];

        switch (nOpcode)
        {
        case VM_CPDOWNSP:
        case VM_CPDOWNBP:
            {
                int nOffset = VM_CODE_INTEGER(pInstruction + 2);
                short nSize = VM_CODE_SHORT(pInstruction + 6);
                int nDestination;
                if (nOpcode == VM_CPDOWNSP)
                {
                    nDestination = m_cStack.m_nStackPointer - (-nOffset >> 2);
                }
                else if (nOpcode == VM_CPDOWNBP)
                {
                    nDestination = m_cStack.m_nBasePointer - (-nOffset >> 2);
                }
                nSize >>= 2;
                if (nDestination < 0)
                {
                    return VM_ERROR_STACK_POP;
                }
                for (int i = 0; i < nSize; ++i)
                {
                    m_cStack.AssignLocationToLocation(
                        m_cStack.m_nStackPointer + i - nSize, nDestination + i);
                }
                *pnInstructionPtr += 8;
            }
            break;
        case VM_RSADD:
            if (nType == VM_STACK_INTEGER)
            {
                if (!StackPushInteger(0)) return VM_ERROR_STACK_PUSH;
            }
            else if (nType == VM_STACK_FLOAT)
            {
                if (!StackPushFloat(0.0f)) return VM_ERROR_STACK_PUSH;
            }
            else if (nType == VM_STACK_STRING)
            {
                CExoString sValue("");
                if (!StackPushString(sValue)) return VM_ERROR_STACK_PUSH;
            }
            else if (nType == VM_STACK_OBJECT)
            {
                if (!StackPushObject(OBJECT_INVALID)) return VM_ERROR_STACK_PUSH;
            }
            else
            {
                if (nType < VM_STACK_ENGINE_STRUCTURE_FIRST || nType > VM_STACK_ENGINE_STRUCTURE_LAST)
                    return VM_ERROR_INVALID_OPERAND;
                if (!StackPushEngineStructure(nType - VM_STACK_ENGINE_STRUCTURE_FIRST, 0))
                    return VM_ERROR_STACK_PUSH;
            }
            *pnInstructionPtr += 2;
            break;
        case VM_CPTOPSP:
        case VM_CPTOPBP:
            {
                int nOffset = VM_CODE_INTEGER(pInstruction + 2);
                short nSize = VM_CODE_SHORT(pInstruction + 6);
                int nSource;
                if (nOpcode == VM_CPTOPSP)
                {
                    nSource = m_cStack.m_nStackPointer - (-nOffset >> 2);
                }
                else if (nOpcode == VM_CPTOPBP)
                {
                    nSource = m_cStack.m_nBasePointer - (-nOffset >> 2);
                }
                nSize >>= 2;
                if (nSource < 0) return VM_ERROR_STACK_POP;
                int nDestination = m_cStack.m_nStackPointer;
                for (int i = 0; i < nSize; ++i)
                {
                    m_cStack.AssignLocationToLocation(nSource + i, nDestination + i);
                }
                *pnInstructionPtr += 8;
            }
            break;
        case VM_CONST:
            if (nType == VM_STACK_INTEGER)
            {
                int nValue = VM_CODE_INTEGER(pInstruction + 2);
                if (!StackPushInteger(nValue)) return VM_ERROR_STACK_PUSH;
                *pnInstructionPtr += 6;
            }
            else if (nType == VM_STACK_OBJECT)
            {
                int nObject = VM_CODE_INTEGER(pInstruction + 2);
                unsigned long oidObject = OBJECT_INVALID;
                if (nObject == 1)
                {
                    oidObject = OBJECT_INVALID;
                }
                else if (nObject == 0 &&
                         m_abValidObjectRunScript[m_nCurrentScript] == 1)
                {
                    oidObject = m_aoidObjectRunScript[m_nCurrentScript];
                }
                if (!StackPushObject(oidObject)) return VM_ERROR_STACK_PUSH;
                *pnInstructionPtr += 6;
            }
            else if (nType == VM_STACK_FLOAT)
            {
                CVirtualMachineStackValue cValue;
                cValue.m_nInteger = VM_CODE_INTEGER(pInstruction + 2);
                if (!StackPushFloat(cValue.m_fFloat)) return VM_ERROR_STACK_PUSH;
                *pnInstructionPtr += 6;
            }
            else if (nType == VM_STACK_STRING)
            {
                int i = 0;
                CExoString sValue;
                short nSize = VM_CODE_SHORT(pCode + *pnInstructionPtr + 2);
                char* sText = new char[nSize + 1];
                for (i = 0; i < nSize; ++i)
                {
                    sText[i] = pCode[*pnInstructionPtr + i + 4];
                }
                sText[nSize] = 0;
                sValue.Format("%s", sText);
                delete[] sText;
                if (!StackPushString(sValue)) return VM_ERROR_STACK_PUSH;
                *pnInstructionPtr += i + 4;
            }
            else
            {
                return VM_ERROR_INVALID_OPERAND;
            }
            break;
        case VM_ACTION:
            {
                short nCommand = VM_CODE_SHORT(pInstruction + 2);
                unsigned char nArguments = pInstruction[4];
                if (!m_pCommandImplementer) return VM_ERROR_NO_COMMAND_IMPLEMENTER;
                int nResult = m_pCommandImplementer->ExecuteCommand(nCommand, nArguments);
                if (nResult < 0) return nResult;
                *pnInstructionPtr += 5;
            }
            break;
        case VM_LOGAND:
            {
                if (nType != VM_OPERAND_INTEGER_INTEGER) return VM_ERROR_INVALID_OPERAND;
                int nRight, nLeft;
                if (!StackPopInteger(&nRight) || !StackPopInteger(&nLeft)) return VM_ERROR_STACK_POP;
                if (nLeft && nRight) StackPushInteger(TRUE);
                else StackPushInteger(FALSE);
                *pnInstructionPtr += 2;
            }
            break;
        case VM_LOGOR:
            {
                if (nType != VM_OPERAND_INTEGER_INTEGER) return VM_ERROR_INVALID_OPERAND;
                int nRight, nLeft;
                if (!StackPopInteger(&nRight) || !StackPopInteger(&nLeft)) return VM_ERROR_STACK_POP;
                if (nLeft || nRight) StackPushInteger(TRUE);
                else StackPushInteger(FALSE);
                *pnInstructionPtr += 2;
            }
            break;
        case VM_INCOR:
            {
                if (nType != VM_OPERAND_INTEGER_INTEGER) return VM_ERROR_INVALID_OPERAND;
                int nRight, nLeft;
                if (!StackPopInteger(&nRight) || !StackPopInteger(&nLeft)) return VM_ERROR_STACK_POP;
                StackPushInteger(nRight | nLeft);
                *pnInstructionPtr += 2;
            }
            break;
        case VM_EXCOR:
            {
                if (nType != VM_OPERAND_INTEGER_INTEGER) return VM_ERROR_INVALID_OPERAND;
                int nRight, nLeft;
                if (!StackPopInteger(&nRight) || !StackPopInteger(&nLeft)) return VM_ERROR_STACK_POP;
                StackPushInteger(nRight ^ nLeft);
                *pnInstructionPtr += 2;
            }
            break;
        case VM_BOOLAND:
            {
                if (nType != VM_OPERAND_INTEGER_INTEGER) return VM_ERROR_INVALID_OPERAND;
                int nRight, nLeft;
                if (!StackPopInteger(&nRight) || !StackPopInteger(&nLeft)) return VM_ERROR_STACK_POP;
                StackPushInteger(nRight & nLeft);
                *pnInstructionPtr += 2;
            }
            break;
        case VM_EQUAL:
        case VM_NEQUAL:
            {
                if (nType != VM_OPERAND_INTEGER_INTEGER && nType != VM_OPERAND_FLOAT_FLOAT &&
                    nType != VM_OPERAND_OBJECT_OBJECT && nType != VM_OPERAND_STRING_STRING &&
                    nType != VM_OPERAND_STRUCTURE_STRUCTURE &&
                    (nType < VM_OPERAND_ENGINE_STRUCTURE_FIRST || nType > VM_OPERAND_ENGINE_STRUCTURE_LAST))
                    return VM_ERROR_INVALID_OPERAND;
                short nSize = 4;
                if (nType == VM_OPERAND_STRUCTURE_STRUCTURE)
                {
                    nSize = VM_CODE_SHORT(pInstruction + 2);
                }
                int nTop = m_cStack.m_nStackPointer;
                int bEqual = 1;
                if (nTop < (nSize >> 1)) return VM_ERROR_STACK_POP;
                nSize >>= 2;
                for (int i = 0; i < nSize; ++i)
                {
                    --nTop;
                    int nValueType = m_cStack.m_pTypes[nTop];
                    if (nValueType == VM_STACK_INTEGER || nValueType == VM_STACK_FLOAT ||
                        nValueType == VM_STACK_OBJECT)
                    {
                        if (m_cStack.m_pValues[nTop].m_nInteger !=
                            m_cStack.m_pValues[nTop - nSize].m_nInteger) bEqual = 0;
                    }
                    else if (nValueType == VM_STACK_STRING)
                    {
                        CExoString* pRight = m_cStack.m_pValues[nTop].m_pString;
                        CExoString* pLeft = m_cStack.m_pValues[nTop - nSize].m_pString;
                        if (!pRight->CompareNoCase(*pLeft)) bEqual = 0;
                    }
                    else
                    {
                        if (nValueType < VM_STACK_ENGINE_STRUCTURE_FIRST || nValueType > VM_STACK_ENGINE_STRUCTURE_LAST)
                            return VM_ERROR_INVALID_STACK_TYPE;
                        void* pRight = m_cStack.m_pValues[nTop].m_pEngineStructure;
                        void* pLeft = m_cStack.m_pValues[nTop - nSize].m_pEngineStructure;
                        if (!m_pCommandImplementer) return VM_ERROR_NO_COMMAND_IMPLEMENTER;
                        if (!m_pCommandImplementer->GetEqualGameDefinedStructure(
                            nValueType - VM_STACK_ENGINE_STRUCTURE_FIRST, pRight, pLeft)) bEqual = 0;
                    }
                }
                m_cStack.SetStackPointer(m_cStack.m_nStackPointer - 2 * nSize);
                StackPushInteger(nOpcode == VM_EQUAL ? bEqual : !bEqual);
                if (nType == VM_OPERAND_STRUCTURE_STRUCTURE) *pnInstructionPtr += 4;
                else *pnInstructionPtr += 2;
            }
            break;
        case VM_GEQ:
        case VM_GT:
        case VM_LT:
        case VM_LEQ:
            {
                if (nType == VM_OPERAND_INTEGER_INTEGER)
                {
                    int nRight, nLeft;
                    if (!StackPopInteger(&nRight) || !StackPopInteger(&nLeft)) return VM_ERROR_STACK_POP;
                    if ((nOpcode == VM_GEQ && nLeft >= nRight) ||
                        (nOpcode == VM_LEQ && nLeft <= nRight) ||
                        (nOpcode == VM_GT && nLeft > nRight) ||
                        (nOpcode == VM_LT && nLeft < nRight))
                    {
                        StackPushInteger(TRUE);
                    }
                    else
                    {
                        StackPushInteger(FALSE);
                    }
                }
                else if (nType == VM_OPERAND_FLOAT_FLOAT)
                {
                    float fRight, fLeft;
                    if (!StackPopFloat(&fRight) || !StackPopFloat(&fLeft)) return VM_ERROR_STACK_POP;
                    if ((nOpcode == VM_GEQ && fLeft >= fRight) ||
                        (nOpcode == VM_LEQ && fLeft <= fRight) ||
                        (nOpcode == VM_GT && fLeft > fRight) ||
                        (nOpcode == VM_LT && fLeft < fRight))
                    {
                        StackPushInteger(TRUE);
                    }
                    else
                    {
                        StackPushInteger(FALSE);
                    }
                }
                else return VM_ERROR_INVALID_OPERAND;
                *pnInstructionPtr += 2;
            }
            break;
        case VM_SHLEFT:
            {
                if (nType != VM_OPERAND_INTEGER_INTEGER) return VM_ERROR_INVALID_OPERAND;
                int nRight, nLeft;
                if (!StackPopInteger(&nRight) || !StackPopInteger(&nLeft)) return VM_ERROR_STACK_POP;
                StackPushInteger(nLeft << nRight);
                *pnInstructionPtr += 2;
            }
            break;
        case VM_SHRIGHT:
            {
                if (nType != VM_OPERAND_INTEGER_INTEGER) return VM_ERROR_INVALID_OPERAND;
                int nRight, nLeft;
                if (!StackPopInteger(&nRight) || !StackPopInteger(&nLeft)) return VM_ERROR_STACK_POP;
                if (nLeft < 0)
                    StackPushInteger(-(-nLeft >> nRight));
                else
                    StackPushInteger(nLeft >> nRight);
                *pnInstructionPtr += 2;
            }
            break;
        case VM_USHRIGHT:
            {
                if (nType != VM_OPERAND_INTEGER_INTEGER) return VM_ERROR_INVALID_OPERAND;
                int nRight, nLeft;
                if (!StackPopInteger(&nRight) || !StackPopInteger(&nLeft)) return VM_ERROR_STACK_POP;
                StackPushInteger(nLeft >> nRight);
                *pnInstructionPtr += 2;
            }
            break;
        case VM_ADD:
        case VM_SUB:
        case VM_MUL:
        case VM_DIV:
            if (nType == VM_OPERAND_INTEGER_INTEGER)
            {
                int nRight, nLeft;
                if (!StackPopInteger(&nRight) || !StackPopInteger(&nLeft)) return VM_ERROR_STACK_POP;
                if (nOpcode == VM_ADD)
                {
                    StackPushInteger(nRight + nLeft);
                }
                else if (nOpcode == VM_SUB)
                {
                    StackPushInteger(nLeft - nRight);
                }
                else if (nOpcode == VM_MUL)
                {
                    StackPushInteger(nRight * nLeft);
                }
                else
                {
                    if (nRight == 0) return VM_ERROR_DIVIDE_BY_ZERO;
                    StackPushInteger(nLeft / nRight);
                }
                *pnInstructionPtr += 2;
            }
            else if (nType == VM_OPERAND_INTEGER_FLOAT || nType == VM_OPERAND_FLOAT_INTEGER ||
                     nType == VM_OPERAND_FLOAT_FLOAT)
            {
                float fRight, fLeft;
                if (nType == VM_OPERAND_FLOAT_INTEGER)
                {
                    int nRight;
                    if (!StackPopInteger(&nRight)) return VM_ERROR_STACK_POP;
                    fRight = (float)nRight;
                }
                else if (!StackPopFloat(&fRight)) return VM_ERROR_STACK_POP;
                if (nType == VM_OPERAND_INTEGER_FLOAT)
                {
                    int nLeft;
                    if (!StackPopInteger(&nLeft)) return VM_ERROR_STACK_POP;
                    fLeft = (float)nLeft;
                }
                else if (!StackPopFloat(&fLeft)) return VM_ERROR_STACK_POP;
                if (nOpcode == VM_ADD)
                {
                    StackPushFloat(fLeft + fRight);
                }
                else if (nOpcode == VM_SUB)
                {
                    StackPushFloat(fLeft - fRight);
                }
                else if (nOpcode == VM_MUL)
                {
                    StackPushFloat(fLeft * fRight);
                }
                else
                {
                    if (fRight == 0.0) return VM_ERROR_DIVIDE_BY_ZERO;
                    StackPushFloat(fLeft / fRight);
                }
                *pnInstructionPtr += 2;
            }
            else if (nType == VM_OPERAND_STRING_STRING)
            {
                CExoString sRight;
                if (!StackPopString(&sRight)) return VM_ERROR_STACK_POP;
                CExoString sLeft;
                if (!StackPopString(&sLeft)) return VM_ERROR_STACK_POP;
                if (nOpcode == VM_ADD) StackPushString(sLeft + sRight);
                *pnInstructionPtr += 2;
            }
            else if (nType == VM_OPERAND_VECTOR_VECTOR)
            {
                Vector vRight;
                if (!StackPopVector(&vRight)) return VM_ERROR_STACK_POP;
                Vector vLeft;
                if (!StackPopVector(&vLeft)) return VM_ERROR_STACK_POP;
                if (nOpcode == VM_ADD) StackPushVector(vLeft + vRight);
                else if (nOpcode == VM_SUB) StackPushVector(vLeft - vRight);
                *pnInstructionPtr += 2;
            }
            else if (nType == VM_OPERAND_VECTOR_FLOAT || nType == VM_OPERAND_FLOAT_VECTOR)
            {
                Vector vValue;
                float fValue;
                if (nType == VM_OPERAND_VECTOR_FLOAT)
                {
                    if (!StackPopFloat(&fValue) || !StackPopVector(&vValue)) return VM_ERROR_STACK_POP;
                }
                else if (nType == VM_OPERAND_FLOAT_VECTOR)
                {
                    if (!StackPopVector(&vValue) || !StackPopFloat(&fValue)) return VM_ERROR_STACK_POP;
                }
                if (nOpcode == VM_MUL) StackPushVector(vValue * fValue);
                else if (nOpcode == VM_DIV)
                {
                    if (fValue == 0.0) return VM_ERROR_DIVIDE_BY_ZERO;
                    StackPushVector(vValue / fValue);
                }
                *pnInstructionPtr += 2;
            }
            else return VM_ERROR_INVALID_OPERAND;
            break;
        case VM_MOD:
            {
                if (nType != VM_OPERAND_INTEGER_INTEGER) return VM_ERROR_INVALID_OPERAND;
                int nRight, nLeft;
                if (!StackPopInteger(&nRight) || !StackPopInteger(&nLeft)) return VM_ERROR_STACK_POP;
                if (nRight == 0) return VM_ERROR_DIVIDE_BY_ZERO;
                StackPushInteger(nLeft % nRight);
                *pnInstructionPtr += 2;
            }
            break;
        case VM_NEG:
            if (nType == VM_STACK_INTEGER)
            {
                int nValue;
                if (!StackPopInteger(&nValue)) return VM_ERROR_STACK_POP;
                StackPushInteger(-nValue);
            }
            else if (nType == VM_STACK_FLOAT)
            {
                float fValue;
                if (!StackPopFloat(&fValue)) return VM_ERROR_STACK_POP;
                StackPushFloat(-fValue);
            }
            else return VM_ERROR_INVALID_OPERAND;
            *pnInstructionPtr += 2;
            break;
        case VM_COMP:
            {
                if (nType != VM_STACK_INTEGER) return VM_ERROR_INVALID_OPERAND;
                int nValue;
                if (!StackPopInteger(&nValue)) return VM_ERROR_STACK_POP;
                StackPushInteger(~nValue);
                *pnInstructionPtr += 2;
            }
            break;
        case VM_NOT:
            {
                if (nType != VM_STACK_INTEGER) return VM_ERROR_INVALID_OPERAND;
                int nValue;
                if (!StackPopInteger(&nValue)) return VM_ERROR_STACK_POP;
                StackPushInteger(nValue == 0);
                *pnInstructionPtr += 2;
            }
            break;
        case VM_MOVSP:
            {
                int nOffset = VM_CODE_INTEGER(pInstruction + 2);
                m_cStack.SetStackPointer(m_cStack.m_nStackPointer - (-nOffset >> 2));
                *pnInstructionPtr += 6;
            }
            break;
        case VM_STORE_STATEALL:
            m_nSavedInstructionPtr = *pnInstructionPtr + nType;
            m_nSavedStackSize = 0;
            m_nSavedStackPointer = 0;
            *pnInstructionPtr += 2;
            break;
        case VM_STORE_STATE:
            {
                m_nSavedInstructionPtr = *pnInstructionPtr + nType;
                char* pOperands = pCode + *pnInstructionPtr;
                int nLow = 0;
                int nStackPointer = VM_CODE_INTEGER(pOperands + 6);
                int nStackSize = ((pOperands[2] << 24) & 0xff000000) + ((pOperands[3] << 16) & 0x00ff0000) +
                    ((pOperands[4] << 8) & 0x0000ff00);
                nLow = pOperands[5];
                if (nLow < 0) nLow &= 0x000000ff;
                else nLow &= 0x000000ff;
                m_nSavedStackSize = nStackSize + nLow;
                m_nSavedStackPointer = nStackPointer;
                *pnInstructionPtr += 10;
            }
            break;
        case VM_JZ:
            {
                int nValue;
                if (!StackPopInteger(&nValue)) return VM_ERROR_STACK_POP;
                if (nValue == 0)
                {
                    *pnInstructionPtr += VM_CODE_INTEGER(pCode + *pnInstructionPtr + 2);
                }
                else *pnInstructionPtr += 6;
            }
            break;
        case VM_JNZ:
            {
                int nValue;
                if (!StackPopInteger(&nValue)) return VM_ERROR_STACK_POP;
                if (nValue != 0)
                {
                    *pnInstructionPtr += VM_CODE_INTEGER(pCode + *pnInstructionPtr + 2);
                }
                else *pnInstructionPtr += 6;
            }
            break;
        case VM_JMP:
            *pnInstructionPtr += VM_CODE_INTEGER(pInstruction + 2);
            break;
        case VM_JSR:
            {
                int nOffset = VM_CODE_INTEGER(pInstruction + 2);
                *pnInstructionPtr += 6;
                if (!PushInstructionPtr(*pnInstructionPtr)) return VM_ERROR_STACK_PUSH;
                *pnInstructionPtr += nOffset - 6;
            }
            break;
        case VM_RETN:
            if (!PopInstructionPtr(pnInstructionPtr)) return VM_ERROR_STACK_POP;
            break;
        case VM_DESTRUCT:
            {
                short nSize = VM_CODE_SHORT(pInstruction + 2) >> 2;
                short nOffset = VM_CODE_SHORT(pInstruction + 4) >> 2;
                short nSavedSize = VM_CODE_SHORT(pInstruction + 6) >> 2;
                int nTop = m_cStack.m_nStackPointer;
                if (nOffset + nSavedSize < nSize)
                {
                    nTop += nOffset - nSize + nSavedSize;
                    m_cStack.SetStackPointer(nTop);
                }
                for (int nLocation = nTop - nOffset - nSavedSize;
                     nLocation < nTop - nOffset; ++nLocation)
                {
                    m_cStack.AssignLocationToLocation(nLocation + nOffset, nLocation);
                }
                if (nOffset > 0)
                    m_cStack.SetStackPointer(m_cStack.m_nStackPointer - nOffset);
                *pnInstructionPtr += 8;
            }
            break;
        case VM_DECISP:
        case VM_INCISP:
        case VM_DECIBP:
        case VM_INCIBP:
            if (nType == VM_STACK_INTEGER)
            {
                int nOffset = VM_CODE_INTEGER(pInstruction + 2);
                int nLocation;
                if (nOpcode == VM_INCISP || nOpcode == VM_DECISP)
                {
                    nLocation = m_cStack.m_nStackPointer - (-nOffset >> 2);
                }
                else if (nOpcode == VM_INCIBP || nOpcode == VM_DECIBP)
                {
                    nLocation = m_cStack.m_nBasePointer - (-nOffset >> 2);
                }
                if (nLocation < 0) return VM_ERROR_STACK_POP;
                if (nOpcode == VM_INCISP || nOpcode == VM_INCIBP)
                {
                    m_cStack.ModifyIntegerAtLocation(nLocation, 1);
                }
                else
                {
                    m_cStack.ModifyIntegerAtLocation(nLocation, -1);
                }
            }
            *pnInstructionPtr += 6;
            break;
        case VM_SAVEBP:
            {
                int nStackPointer = m_cStack.m_nStackPointer;
                StackPushInteger(m_cStack.m_nBasePointer);
                m_cStack.m_nBasePointer = nStackPointer;
                *pnInstructionPtr += 2;
            }
            break;
        case VM_RESTOREBP:
            {
                int nBasePointer;
                if (!StackPopInteger(&nBasePointer)) return VM_ERROR_STACK_POP;
                m_cStack.m_nBasePointer = nBasePointer;
                *pnInstructionPtr += 2;
            }
            break;
        default:
            return VM_ERROR_INVALID_INSTRUCTION;
        }
    }
    return 0;
}
// PC 0x005D4270  Xbox 0x0015B000
int CVirtualMachineInternal::RunScriptFile(int nStartInstructionPtr)
{
    int nStackPointer;
    int nReturnStackPointer;
    int nReturn;

    nStackPointer = m_cStack.m_nStackPointer;
    nReturnStackPointer = m_nReturnStackPointer;
    m_nReturnType = 0;
    m_nReturnValue = 0;
    PushInstructionPtr(-1);

    {
        int nInstructionPtr = nStartInstructionPtr;

        nReturn = ExecuteCode(&nInstructionPtr,
                              m_aScripts[m_nCurrentScript].m_pCode,
                              m_aScripts[m_nCurrentScript].m_nCodeSize);
    }
    if (nReturn < 0 || nReturnStackPointer != m_nReturnStackPointer)
    {
        CExoString sScriptName(m_aScripts[m_nCurrentScript].m_sName.CStr());

        m_pCommandImplementer->ReportError(sScriptName, -nReturn);
        if (m_cStack.m_nStackPointer > nStackPointer)
        {
            m_cStack.SetStackPointer(nStackPointer);
        }
        else
        {
            m_cStack.m_nStackPointer = nStackPointer;
        }
        m_nReturnStackPointer = nReturnStackPointer;
    }

    return nReturn;
}
#ifndef _XBOX
int CVirtualMachineInternal::StackPopCommand(CVirtualMachineScript** ppCommand)  // PC 0x005D4380  Xbox 0x0015B120
{
    *ppCommand = new CVirtualMachineScript();
    if (m_aScripts[m_nCurrentScript].m_bLoaded == 0)
    {
        (*ppCommand)->m_nCodeSize = 0;
        (*ppCommand)->m_pCode = 0;
    }
    else
    {
        (*ppCommand)->m_nCodeSize = m_aScripts[m_nCurrentScript].m_nCodeSize;
        (*ppCommand)->m_pCode =
            new char[m_aScripts[m_nCurrentScript].m_nCodeSize];
        memcpy((*ppCommand)->m_pCode, m_aScripts[m_nCurrentScript].m_pCode,
               m_aScripts[m_nCurrentScript].m_nCodeSize);
    }

    (*ppCommand)->m_nObjectId = m_aScripts[m_nCurrentScript].m_nObjectId;
    (*ppCommand)->m_bLoaded = m_aScripts[m_nCurrentScript].m_bLoaded;
    (*ppCommand)->m_nSecondaryPtr = 0;
    (*ppCommand)->m_nInstructionPtr = m_nSavedInstructionPtr;
    (*ppCommand)->m_nStackSize = m_cStack.m_nStackPointer;
    (*ppCommand)->m_pStack = new CVirtualMachineStack;
    (*ppCommand)->m_pStack->m_pVirtualMachine = this;
    (*ppCommand)->m_pStack->CopyFromStack(&m_cStack, m_nSavedStackPointer,
                                          m_nSavedStackSize);
    (*ppCommand)->m_sName = m_aScripts[m_nCurrentScript].m_sName;
    return 1;
}
#endif
int CVirtualMachineInternal::RunScript(CExoString* psScript,  // PC 0x005D45D0  Xbox 0x0015B290
                                       unsigned long nObjectId,
                                       int bValidObjectRunScript)
{
    int nStackPointer;
    int nResult;

    if (psScript == 0 || psScript->IsEmpty() == 1)
    {
        return 0;
    }

    if (m_nCurrentScript == -1)
    {
        m_cStack.ClearStack();
        m_cStack.m_pVirtualMachine = this;
        m_nInstructionCount = 0;
        m_nReturnStackPointer = 0;
    }

    if (ReadScriptFile(psScript) == 0)
    {
#ifdef _XBOX
        m_aScripts[m_nCurrentScript].m_nObjectId = ComputeCRC(
            m_aScripts[m_nCurrentScript].m_pCode,
            m_aScripts[m_nCurrentScript].m_nCodeSize);
#else
        m_aScripts[m_nCurrentScript].m_nObjectId = 0;
#endif
        m_abValidObjectRunScript[m_nCurrentScript] = bValidObjectRunScript;
        m_aoidObjectRunScript[m_nCurrentScript] = nObjectId;
        if (m_pCommandImplementer != 0)
        {
            m_pCommandImplementer->RunScriptCallback(*psScript);
            m_pCommandImplementer->m_nCommandContext =
                m_abValidObjectRunScript[m_nCurrentScript];
            m_pCommandImplementer->m_oidObject =
                m_aoidObjectRunScript[m_nCurrentScript];
        }

        nStackPointer = m_cStack.m_nStackPointer;
        nResult = RunScriptFile(0);
        DeleteScript(&m_aScripts[m_nCurrentScript]);
#ifdef _XBOX
        m_aVMFile[m_nCurrentScript].m_pRes->Release();
#endif
        m_nCurrentScript--;
        if (m_nCurrentScript != -1 && m_pCommandImplementer != 0)
        {
            m_pCommandImplementer->m_nCommandContext =
                m_abValidObjectRunScript[m_nCurrentScript];
            m_pCommandImplementer->m_oidObject =
                m_aoidObjectRunScript[m_nCurrentScript];
        }

        if (nResult == 0)
        {
            if (m_nCurrentScript != -1)
            {
                psScript->CStr();
            }

            if (m_cStack.m_nStackPointer == nStackPointer + 1)
            {
                if (m_cStack.m_pTypes[nStackPointer] == VM_STACK_INTEGER)
                {
                    m_nReturnType = VM_STACK_INTEGER;
                    m_nReturnValue =
                        m_cStack.m_pValues[nStackPointer].m_nInteger;
                }

                m_cStack.SetStackPointer(nStackPointer);
                if (m_nCurrentScript == -1)
                {
                    m_cStack.ClearStack();
                }
                return 1;
            }
            else if (m_cStack.m_nStackPointer == nStackPointer)
            {
                if (m_nCurrentScript == -1)
                {
                    m_cStack.ClearStack();
                }
                return 1;
            }
            else
            {
                m_cStack.SetStackPointer(nStackPointer);
                if (m_nCurrentScript == -1)
                {
                    m_cStack.ClearStack();
                }
                return 0;
            }
        }

        if (m_nCurrentScript != -1)
        {
            psScript->CStr();
        }
        return 0;
    }

    if (m_nCurrentScript != -1)
    {
        psScript->CStr();
    }
    return 0;
}
#ifndef _XBOX
int CVirtualMachineInternal::SaveScriptSituation(  // PC 0x005D47A0  Xbox 0x0015B490
    CVirtualMachineScript* pScript, CResGFF* pGFF, CResStruct* pStruct)
{
    CResStruct cStack;

    if (pScript->m_pCode != 0 && pScript->m_nCodeSize != 0)
    {
        pGFF->WriteFieldINT(pStruct, pScript->m_nCodeSize, "CodeSize");
        pGFF->WriteFieldVOID(pStruct, pScript->m_pCode,
                             pScript->m_nCodeSize, "Code");
    }
    else
    {
        if (ReadScriptFile(&pScript->m_sName) == 0)
        {
            pGFF->WriteFieldINT(pStruct,
                m_aScripts[m_nCurrentScript].m_nCodeSize, "CodeSize");
            pGFF->WriteFieldVOID(pStruct,
                m_aScripts[m_nCurrentScript].m_pCode,
                m_aScripts[m_nCurrentScript].m_nCodeSize, "Code");
            DeleteScript(&m_aScripts[m_nCurrentScript]);
            m_nCurrentScript--;
        }
        else
        {
            pGFF->WriteFieldINT(pStruct, 0, "CodeSize");
        }
    }

    pScript->m_nObjectId = 0;
    pGFF->WriteFieldDWORD(pStruct, 0, "CRC");
    pGFF->WriteFieldINT(pStruct, pScript->m_nInstructionPtr,
                        "InstructionPtr");
    pGFF->WriteFieldINT(pStruct, pScript->m_nSecondaryPtr, "SecondaryPtr");
    pGFF->WriteFieldCExoString(pStruct, pScript->m_sName, "Name");
    pGFF->WriteFieldINT(pStruct, pScript->m_nStackSize, "StackSize");
    pGFF->AddStructToStruct(&cStack, pStruct, "Stack", 0);
    pScript->m_pStack->SaveStack(pGFF, &cStack);
    return 1;
}
#endif
int CVirtualMachineInternal::DeleteScript(CVirtualMachineScript* pScript)  // PC 0x005D44F0  Xbox 0x0015B250
{
    CVirtualMachineStack* pStack;

    pStack = pScript->m_pStack;
    if (pScript->m_pStack != 0)
    {
        delete pStack;
        pScript->m_pStack = 0;
    }

#ifndef _XBOX
    if (pScript->m_pCode != 0)
    {
        delete pScript->m_pCode;
        pScript->m_pCode = 0;
    }
#endif

    pScript->m_nStackSize = 0;
    pScript->m_nInstructionPtr = 13;
    pScript->m_nSecondaryPtr = 0;
    pScript->m_nCodeSize = 0;
    pScript->m_nObjectId = 0;

    return 1;
}
#ifndef _XBOX
int CVirtualMachineInternal::RunScriptSituation(  // PC 0x005D4AD0  Xbox 0x0015B840
    CVirtualMachineScript* pScript, unsigned long nObjectId,
    int bOneInstruction)
{
    int nResult;

    m_cStack.ClearStack();
    m_cStack.m_nStackPointer = pScript->m_pStack->m_nStackPointer;
    m_cStack.m_nAllocated = pScript->m_pStack->m_nAllocated;
    m_cStack.m_pVirtualMachine = this;
    m_cStack.CopyFromStack(pScript->m_pStack, 0, 0);
    if (pScript->m_pStack != 0)
    {
        delete pScript->m_pStack;
        pScript->m_pStack = 0;
    }

    m_nReturnStackPointer = 0;
    m_nInstructionCount = 0;
    if (SetUpScriptSituation(pScript) == 0)
    {
        if (m_aScripts[m_nCurrentScript].m_nObjectId == 0)
        {
            m_abValidObjectRunScript[m_nCurrentScript] = bOneInstruction;
            m_aoidObjectRunScript[m_nCurrentScript] = nObjectId;
            if (m_pCommandImplementer != 0)
            {
                m_pCommandImplementer->m_nCommandContext =
                    m_abValidObjectRunScript[m_nCurrentScript];
                m_pCommandImplementer->m_oidObject =
                    m_aoidObjectRunScript[m_nCurrentScript];
            }

            nResult = RunScriptFile(pScript->m_nInstructionPtr);
            DeleteScript(&m_aScripts[m_nCurrentScript]);
            m_nCurrentScript--;
            delete pScript;
            return nResult == 0;
        }

        DeleteScript(&m_aScripts[m_nCurrentScript]);
        m_nCurrentScript--;
    }

    delete pScript;
    return 0;
}
#endif
void CVirtualMachineInternal::DeleteScriptSituation(  // PC 0x005D48E0  Xbox 0x0015B820
    CVirtualMachineScript* pScript)
{
    delete pScript;
}
// PC 0x005D4900  Xbox 0x0015B690
CVirtualMachineInternal::CVirtualMachineInternal()
{
    m_nCurrentScript = -1;
    m_pCommandImplementer = 0;
    m_cStack.ClearStack();
    m_cStack.m_pVirtualMachine = this;
#ifdef _XBOX
    InitializeCRCTable();
#endif
}
// PC 0x005D49B0  Xbox 0x0015B740
CVirtualMachineInternal::~CVirtualMachineInternal()
{
    for (int nScript = 0; nScript < VM_MAX_SCRIPTS; nScript++)
    {
        DeleteScript(&m_aScripts[nScript]);
    }
}

static inline void VirtualMachineTrace()
{
}

#ifdef _XBOX
// Xbox 0x001581F0
void CVirtualMachineInternal::InitializeCRCTable()
{
    unsigned long nCRC;
    int nBit;

    for (unsigned long nValue = 0; nValue <= 255; nValue++)
    {
        nCRC = nValue;
        for (nBit = 8; nBit != 0; --nBit)
        {
            if (nCRC & 1)
            {
                nCRC = (nCRC >> 1) ^ 0xEDB88320;
            }
            else
            {
                nCRC >>= 1;
            }
        }
        m_aCRCTable[nValue] = nCRC;
    }
}

// Xbox 0x00158230
unsigned long CVirtualMachineInternal::ComputeCRC(char* pData, unsigned long nSize)
{
    unsigned long nCRC = 0;
    for (unsigned long nByte = 0; nByte < nSize; nByte++)
    {
        nCRC = m_aCRCTable[(pData[nByte] ^ nCRC) & 0xFF] ^ (nCRC >> 8);
    }
    return nCRC;
}

// Xbox 0x00158610
int CVirtualMachineFile::LoadFile(CExoString* psFileName, char** ppData, unsigned long* pnSize)
{
    if (psFileName == NULL)
    {
        CResRef cResRef("");
        SetResRef(cResRef, 0);
    }
    else
    {
        CResRef cResRef(*psFileName);
        SetResRef(cResRef, 0);
    }
    if (!m_pRes->Demand())
    {
        return -95;
    }
    *ppData = m_pRes->GetNCSDataPtr();
    *pnSize = m_pRes->GetNCSSize();
    return 0;
}
#endif

#ifdef _XBOX
void AurPostString(char* szString, int nColumn, int nRow, float fDuration);

// PC 0x005D2260  Xbox 0x00158DE0
int CVirtualMachineInternal::ReadScriptFile(CExoString* psScript)
{
    char* pNCSData;
    unsigned long nNCSSize;
    char cVersion;
    char cRevision;
    int nVersion;

    m_nCurrentScript++;
    if (m_nCurrentScript >= 8)
    {
        m_nCurrentScript--;
        return -94;
    }

    int nResult = m_aVMFile[m_nCurrentScript].LoadFile(psScript, &pNCSData,
                                                       &nNCSSize);
    if (nResult < 0)
    {
        m_nCurrentScript--;
        return nResult;
    }

    m_aScripts[m_nCurrentScript].m_sName = *psScript;
    if (pNCSData[0] == 'N' && pNCSData[1] == 'C' && pNCSData[2] == 'S' &&
        pNCSData[3] == ' ' && pNCSData[4] == 'V' && pNCSData[6] == '.')
    {
        cVersion = pNCSData[5];
        nVersion = 0;
        if (cVersion >= '1' && cVersion <= '9')
        {
            nVersion = 10 * (cVersion - '0');
        }

        cRevision = pNCSData[7];
        if (cRevision >= '1' && cRevision <= '9')
        {
            nVersion += cRevision - '0';
        }

        if (nVersion != 10)
        {
            m_aVMFile[m_nCurrentScript].m_pRes->Release();
            m_nCurrentScript--;
            return -96;
        }

        if (pNCSData[8] == 'B')
        {
            InitializeScript(&m_aScripts[m_nCurrentScript], pNCSData + 13,
                             nNCSSize - 13);
            return 0;
        }
    }

    m_aVMFile[m_nCurrentScript].m_pRes->Release();
    m_nCurrentScript--;
    return -96;
}

// PC 0x005D26F0  Xbox 0x00159460
int CVirtualMachineInternal::LoadScriptSituation(
    CVirtualMachineScript** ppScript, CResGFF* pGFF, CResStruct* pStruct)
{
    int bSuccess;
    CResStruct cStackStruct;

    *ppScript = new CVirtualMachineScript;
    (*ppScript)->m_nCodeSize = 0;
    (*ppScript)->m_pCode = 0;
    (*ppScript)->m_nObjectId =
        pGFF->ReadFieldDWORD(pStruct, "CRC", bSuccess, 0);
    (*ppScript)->m_bLoaded = 1;
    (*ppScript)->m_nInstructionPtr =
        pGFF->ReadFieldINT(pStruct, "InstructionPtr", bSuccess, 0);
    (*ppScript)->m_nSecondaryPtr =
        pGFF->ReadFieldINT(pStruct, "SecondaryPtr", bSuccess, 0);
    (*ppScript)->m_sName =
        pGFF->ReadFieldCExoString(pStruct, "Name", bSuccess, "");
    (*ppScript)->m_nStackSize =
        pGFF->ReadFieldINT(pStruct, "StackSize", bSuccess, 0);
    pGFF->GetStructFromStruct(&cStackStruct, pStruct, "Stack");
    (*ppScript)->m_pStack = new CVirtualMachineStack;
    (*ppScript)->m_pStack->m_pVirtualMachine = this;
    (*ppScript)->m_pStack->LoadStack(pGFF, &cStackStruct);
    return 1;
}

// PC 0x005D4380  Xbox 0x0015B120
int CVirtualMachineInternal::StackPopCommand(CVirtualMachineScript** ppCommand)
{
    *ppCommand = new CVirtualMachineScript();
    if (m_aScripts[m_nCurrentScript].m_bLoaded == 0)
    {
        (*ppCommand)->m_nCodeSize = 0;
        (*ppCommand)->m_pCode = 0;
    }
    else
    {
        (*ppCommand)->m_nCodeSize = 0;
        (*ppCommand)->m_pCode = 0;
    }

    (*ppCommand)->m_nObjectId = m_aScripts[m_nCurrentScript].m_nObjectId;
    (*ppCommand)->m_bLoaded = m_aScripts[m_nCurrentScript].m_bLoaded;
    (*ppCommand)->m_nSecondaryPtr = 0;
    (*ppCommand)->m_nInstructionPtr = m_nSavedInstructionPtr;
    (*ppCommand)->m_nStackSize = m_cStack.m_nStackPointer;
    (*ppCommand)->m_pStack = new CVirtualMachineStack;
    (*ppCommand)->m_pStack->m_pVirtualMachine = this;
    (*ppCommand)->m_pStack->CopyFromStack(&m_cStack, m_nSavedStackPointer,
                                          m_nSavedStackSize);
    (*ppCommand)->m_sName = m_aScripts[m_nCurrentScript].m_sName;
    return 1;
}

// PC 0x005D47A0  Xbox 0x0015B490
int CVirtualMachineInternal::SaveScriptSituation(
    CVirtualMachineScript* pScript, CResGFF* pGFF, CResStruct* pStruct)
{
    CResStruct cStack;

    pScript->m_nCodeSize = 0;
    pGFF->WriteFieldINT(pStruct, 0, "CodeSize");
    if (pScript->m_pCode != 0 && pScript->m_nCodeSize != 0)
    {
        pGFF->WriteFieldDWORD(pStruct, pScript->m_nObjectId, "CRC");
    }
    else
    {
        if (ReadScriptFile(&pScript->m_sName) == 0)
        {
            m_aScripts[m_nCurrentScript].m_nObjectId = ComputeCRC(
                m_aScripts[m_nCurrentScript].m_pCode,
                m_aScripts[m_nCurrentScript].m_nCodeSize);
            pGFF->WriteFieldDWORD(pStruct,
                m_aScripts[m_nCurrentScript].m_nObjectId, "CRC");
            DeleteScript(&m_aScripts[m_nCurrentScript]);
            m_aVMFile[m_nCurrentScript].m_pRes->Release();
            m_nCurrentScript--;
        }
        else
        {
            pGFF->WriteFieldDWORD(pStruct,
                m_aScripts[m_nCurrentScript].m_nObjectId, "CRC");
        }
    }

    pGFF->WriteFieldINT(pStruct, pScript->m_nInstructionPtr,
                        "InstructionPtr");
    pGFF->WriteFieldINT(pStruct, pScript->m_nSecondaryPtr, "SecondaryPtr");
    pGFF->WriteFieldCExoString(pStruct, pScript->m_sName, "Name");
    pGFF->WriteFieldINT(pStruct, pScript->m_nStackSize, "StackSize");
    pGFF->AddStructToStruct(&cStack, pStruct, "Stack", 0);
    pScript->m_pStack->SaveStack(pGFF, &cStack);
    return 1;
}


// PC 0x005D4AD0  Xbox 0x0015B840
int CVirtualMachineInternal::RunScriptSituation(
    CVirtualMachineScript* pScript, unsigned long nObjectId,
    int bOneInstruction)
{
    int nResult;

    m_cStack.ClearStack();
    m_cStack.m_nStackPointer = pScript->m_pStack->m_nStackPointer;
    m_cStack.m_nAllocated = pScript->m_pStack->m_nAllocated;
    m_cStack.m_pVirtualMachine = this;
    m_cStack.CopyFromStack(pScript->m_pStack, 0, 0);
    if (pScript->m_pStack != 0)
    {
        delete pScript->m_pStack;
        pScript->m_pStack = 0;
    }

    m_nReturnStackPointer = 0;
    m_nInstructionCount = 0;
    if (SetUpScriptSituation(pScript) == 0)
    {
        if (m_aScripts[m_nCurrentScript].m_nObjectId ==
            ComputeCRC(m_aScripts[m_nCurrentScript].m_pCode,
                       m_aScripts[m_nCurrentScript].m_nCodeSize))
        {
            m_abValidObjectRunScript[m_nCurrentScript] = bOneInstruction;
            m_aoidObjectRunScript[m_nCurrentScript] = nObjectId;
            if (m_pCommandImplementer != 0)
            {
                m_pCommandImplementer->m_nCommandContext =
                    m_abValidObjectRunScript[m_nCurrentScript];
                m_pCommandImplementer->m_oidObject =
                    m_aoidObjectRunScript[m_nCurrentScript];
            }

            nResult = RunScriptFile(pScript->m_nInstructionPtr);
            DeleteScript(&m_aScripts[m_nCurrentScript]);
            m_aVMFile[m_nCurrentScript].m_pRes->Release();
            m_nCurrentScript--;
            delete pScript;
            return nResult == 0;
        }

        AurPostString("ERROR: TRYING TO RUN WITH 64 MB CAP, BUT SCRIPTS HAVE CHANGED.", 2, 4, 15.0f);
        AurPostString("ERROR: PLOTS COULD BE BROKEN.", 2, 6, 15.0f);
        AurPostString("ERROR: REPORT THIS BUG IMMEDIATELY IF YOU THINK THIS IS IN ERROR.", 2, 8, 15.0f);
        AurPostString("ERROR: STOP TESTING IMMEDIATELY!", 2, 10, 15.0f);
        DeleteScript(&m_aScripts[m_nCurrentScript]);
        m_aVMFile[m_nCurrentScript].m_pRes->Release();
        m_nCurrentScript--;
    }

    delete pScript;
    return 0;
}
#endif
