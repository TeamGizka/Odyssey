#ifndef __NWVMACHINEINTERNAL_H__
#define __NWVMACHINEINTERNAL_H__

#include "exobase.h"
#include "gameobjectarray.h"
#include "exoresman.h"
#include "nwscript.h"

class CVirtualMachineInternal;
class CResGFF;
class CResStruct;

#define VM_STACK_MAX_COPY_ENTRIES 65536
#define VM_STACK_ALLOCATION_INCREMENT 16

enum VirtualMachineStackType
{
    VM_STACK_INTEGER = 3,
    VM_STACK_FLOAT = 4,
    VM_STACK_STRING = 5,
    VM_STACK_OBJECT = 6,
    VM_STACK_ENGINE_STRUCTURE_FIRST = 16,
    VM_STACK_ENGINE_STRUCTURE_LAST = 25
};

enum VirtualMachineOpcode
{
    VM_CPDOWNSP = 1, VM_RSADD = 2, VM_CPTOPSP = 3, VM_CONST = 4,
    VM_ACTION = 5, VM_LOGAND = 6, VM_LOGOR = 7, VM_INCOR = 8,
    VM_EXCOR = 9, VM_BOOLAND = 10, VM_EQUAL = 11, VM_NEQUAL = 12,
    VM_GEQ = 13, VM_GT = 14, VM_LT = 15, VM_LEQ = 16,
    VM_SHLEFT = 17, VM_SHRIGHT = 18, VM_USHRIGHT = 19,
    VM_ADD = 20, VM_SUB = 21, VM_MUL = 22, VM_DIV = 23,
    VM_MOD = 24, VM_NEG = 25, VM_COMP = 26, VM_MOVSP = 27,
    VM_STORE_STATEALL = 28, VM_JMP = 29, VM_JSR = 30, VM_JZ = 31,
    VM_RETN = 32, VM_DESTRUCT = 33, VM_NOT = 34, VM_DECISP = 35,
    VM_INCISP = 36, VM_JNZ = 37, VM_CPDOWNBP = 38, VM_CPTOPBP = 39,
    VM_DECIBP = 40, VM_INCIBP = 41, VM_SAVEBP = 42,
    VM_RESTOREBP = 43, VM_STORE_STATE = 44
};

enum VirtualMachineOperandType
{
    VM_OPERAND_INTEGER_INTEGER = 32,
    VM_OPERAND_FLOAT_FLOAT = 33,
    VM_OPERAND_OBJECT_OBJECT = 34,
    VM_OPERAND_STRING_STRING = 35,
    VM_OPERAND_STRUCTURE_STRUCTURE = 36,
    VM_OPERAND_INTEGER_FLOAT = 37,
    VM_OPERAND_FLOAT_INTEGER = 38,
    VM_OPERAND_ENGINE_STRUCTURE_FIRST = 48,
    VM_OPERAND_ENGINE_STRUCTURE_LAST = 57,
    VM_OPERAND_VECTOR_VECTOR = 58,
    VM_OPERAND_VECTOR_FLOAT = 59,
    VM_OPERAND_FLOAT_VECTOR = 60
};

enum VirtualMachineError
{
    VM_ERROR_TOO_MANY_INSTRUCTIONS = -93,
    VM_ERROR_INVALID_OPERAND = -97,
    VM_ERROR_STACK_PUSH = -99,
    VM_ERROR_STACK_POP = -100,
    VM_ERROR_INVALID_INSTRUCTION = -101,
    VM_ERROR_DIVIDE_BY_ZERO = -105,
    VM_ERROR_INVALID_CODE = -107,
    VM_ERROR_NO_COMMAND_IMPLEMENTER = -108,
    VM_ERROR_INVALID_STACK_TYPE = -109
};

#define VM_MAX_EXECUTED_INSTRUCTIONS 131072

union CVirtualMachineStackValue
{
    int m_nInteger;
    unsigned long m_oidObject;
    float m_fFloat;
    void* m_pEngineStructure;
    CExoString* m_pString;
};

class CVirtualMachineStack
{
public:
    CVirtualMachineStack()
    {
        m_nStackPointer = 0;
        m_nBasePointer = 0;
        m_nAllocated = 0;
        m_pTypes = 0;
        m_pValues = 0;
    }

    ~CVirtualMachineStack();

    int m_nStackPointer;
    int m_nBasePointer;
    int m_nAllocated;
    char* m_pTypes;
    CVirtualMachineStackValue* m_pValues;
    CVirtualMachineInternal* m_pVirtualMachine;

    void AddToTopOfStack(int nType);
    void ModifyIntegerAtLocation(int nLocation, int nValue);
    void AssignLocationToLocation(int nSource, int nDestination);
    int SaveStack(CResGFF* pGFF, CResStruct* pStruct);
    void ClearStack();
    void SetStackPointer(int nStackPointer);
    int LoadStack(CResGFF* pGFF, CResStruct* pStruct);
    void CopyFromStack(CVirtualMachineStack* pStack, int nStackSize,
                       int nBaseSize);
};

class CVirtualMachineScript
{
public:
    CVirtualMachineScript();
    ~CVirtualMachineScript()  // PC 0x006201A0
    {
        CVirtualMachineStack* pStack;
        char* pCode;

        pStack = m_pStack;
        if (m_pStack != 0)
        {
            delete pStack;
        }

#ifndef _XBOX
        pCode = m_pCode;
#endif
        m_pStack = 0;
#ifndef _XBOX
        if (pCode != 0)
        {
            delete pCode;
            m_pCode = 0;
        }
#endif
    }

    CVirtualMachineStack* m_pStack;
    int m_nStackSize;
    int m_nInstructionPtr;
    int m_nSecondaryPtr;
    char* m_pCode;
    int m_nCodeSize;
    CExoString m_sName;
    int m_bLoaded;
    unsigned long m_nObjectId;
};

#define VM_MAX_SCRIPTS 8

#ifdef _XBOX

class CVirtualMachineFile : public CResHelper<CResNCS, RESTYPE_NCS>
{
public:
    int LoadFile(CExoString* psFileName, char** ppData,
                 unsigned long* pnSize);
};

class CVirtualMachineInternal
#else
class CVirtualMachineInternal : public CResHelper<CResNCS, RESTYPE_NCS>
#endif
{
public:
    CVirtualMachineInternal();
#ifdef _XBOX
    ~CVirtualMachineInternal();
    void InitializeCRCTable();
    unsigned long ComputeCRC(char* pData, unsigned long nSize);
#else
    virtual ~CVirtualMachineInternal();
#endif

    void InitializeScript(CVirtualMachineScript* pScript, char* pCode,
                          int nCodeSize);
    int DeleteScript(CVirtualMachineScript* pScript);
    int PopInstructionPtr(int* pnInstructionPtr);
    int PushInstructionPtr(int nInstructionPtr);
    int ReadScriptFile(CExoString* psScript);
    int SetUpScriptSituation(CVirtualMachineScript* pScript);

    int RunScript(CExoString* psScript, unsigned long nObjectId,
                  int bValidObjectRunScript);
    int RunScriptSituation(CVirtualMachineScript* pScript,
                           unsigned long nObjectId, int bOneInstruction);
    int GetRunScriptReturnValue(int* pnType, void** ppValue);
    void SetCommandImplementer(CVirtualMachineCmdImplementer* pImplementer);
    int StackPopInteger(int* pnValue);
    int StackPushInteger(int nValue);
    int StackPopFloat(float* pfValue);
    int StackPushFloat(float fValue);
    int StackPopVector(Vector* pvValue);
    int StackPushVector(Vector vValue);
    int StackPopString(CExoString* psValue);
    int StackPushString(const CExoString& sValue);
    int StackPopEngineStructure(int nType, void** ppValue);
    int StackPushEngineStructure(int nType, void* pValue);
    int StackPopObject(unsigned long* pnValue);
    int StackPushObject(unsigned long nValue);
    int StackPopCommand(CVirtualMachineScript** ppCommand);
    int SaveScriptSituation(CVirtualMachineScript* pScript, CResGFF* pGFF,
                            CResStruct* pStruct);
    int LoadScriptSituation(CVirtualMachineScript** ppScript, CResGFF* pGFF,
                            CResStruct* pStruct);
    void DeleteScriptSituation(CVirtualMachineScript* pScript);
    int ExecuteCode(int* pnInstructionPtr, char* pCode, int nCodeSize);
    int RunScriptFile(int nStartInstructionPtr);

    int m_nReturnType;
    int m_nReturnValue;
    int m_nInstructionCount;
    int m_nCurrentScript;
#ifdef _XBOX
    unsigned long m_aCRCTable[256];
    CVirtualMachineFile m_aVMFile[VM_MAX_SCRIPTS];
#endif
    CVirtualMachineScript m_aScripts[VM_MAX_SCRIPTS];
    int m_abValidObjectRunScript[VM_MAX_SCRIPTS];
    unsigned long m_aoidObjectRunScript[VM_MAX_SCRIPTS];
    CVirtualMachineStack m_cStack;
    int m_nReturnStackPointer;
    int m_anReturnStack[128];
    int m_nSavedInstructionPtr;
    int m_nSavedStackPointer;
    int m_nSavedStackSize;
    CVirtualMachineCmdImplementer* m_pCommandImplementer;
};

#endif
