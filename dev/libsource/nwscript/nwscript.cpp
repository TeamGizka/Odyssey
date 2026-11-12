#include "nwscript.h"
#include "nwvmachineinternal.h"
#include "nwscriptinternal.h"

CScriptCompiler::CScriptCompiler()
{
    m_pScriptCompilerInternal = new CScriptCompilerInternal;
}
// PC 0x0061CAB0
CVirtualMachine::CVirtualMachine()
{
    m_pVirtualMachineInternal = new CVirtualMachineInternal;
}

// PC 0x0061CB30
CVirtualMachine::~CVirtualMachine()
{
    if (m_pVirtualMachineInternal != NULL)
    {
        delete m_pVirtualMachineInternal;
    }
}
// PC 0x0061CB90
void CVirtualMachine::SetScriptParams(int nParam1, int nParam2, int nParam3,
                                      int nParam4, int nParam5,
                                      CExoString sParam)
{
    m_nScriptParameters[0] = nParam1;
    m_nScriptParameters[1] = nParam2;
    m_nScriptParameters[2] = nParam3;
    m_nScriptParameters[3] = nParam4;
    m_nScriptParameters[4] = nParam5;
    m_sScriptStringParameter[0] = sParam;
}
// PC 0x0061CC00
int CVirtualMachine::RunScript(CExoString* psScript, unsigned long nObjectId, int nInstructionLimit)
{
    return m_pVirtualMachineInternal->RunScript(psScript, nObjectId, nInstructionLimit);
}
// PC 0x0061CC10
int CVirtualMachine::RunScriptSituation(void* pScript, unsigned long nObjectId, int bOneInstruction)
{
    return m_pVirtualMachineInternal->RunScriptSituation((CVirtualMachineScript*)pScript, nObjectId, bOneInstruction);
}
// PC 0x0061CC20
int CVirtualMachine::GetRunScriptReturnValue(int* pnType, void** ppValue)
{
    return m_pVirtualMachineInternal->GetRunScriptReturnValue(pnType, ppValue);
}
// PC 0x0061CC30
void CVirtualMachine::SetCommandImplementer(CVirtualMachineCmdImplementer* pImplementer)
{
    m_pVirtualMachineInternal->SetCommandImplementer(pImplementer);
}
// PC 0x0061CC40
int CVirtualMachine::StackPopInteger(int* pnValue)
{
    return m_pVirtualMachineInternal->StackPopInteger(pnValue);
}
// PC 0x0061CC50
int CVirtualMachine::StackPushInteger(int nValue)
{
    return m_pVirtualMachineInternal->StackPushInteger(nValue);
}
// PC 0x0061CC60
int CVirtualMachine::StackPopFloat(float* pfValue)
{
    return m_pVirtualMachineInternal->StackPopFloat(pfValue);
}
// PC 0x0061CC70
int CVirtualMachine::StackPushFloat(float fValue)
{
    return m_pVirtualMachineInternal->StackPushFloat(fValue);
}
// PC 0x0061CC80
int CVirtualMachine::StackPopVector(Vector* pvValue)
{
    return m_pVirtualMachineInternal->StackPopVector(pvValue);
}
// PC 0x0061CC90
int CVirtualMachine::StackPushVector(Vector vValue)
{
    return m_pVirtualMachineInternal->StackPushVector(vValue);
}
// PC 0x0061CCC0
int CVirtualMachine::StackPopString(CExoString* psValue)
{
    return m_pVirtualMachineInternal->StackPopString(psValue);
}
// PC 0x0061CCD0
int CVirtualMachine::StackPushString(const CExoString& sValue)
{
    return m_pVirtualMachineInternal->StackPushString(sValue);
}
// PC 0x0061CCE0
int CVirtualMachine::StackPopEngineStructure(int nType, void** ppValue)
{
    return m_pVirtualMachineInternal->StackPopEngineStructure(nType, ppValue);
}
// PC 0x0061CCF0
int CVirtualMachine::StackPushEngineStructure(int nType, void* pValue)
{
    return m_pVirtualMachineInternal->StackPushEngineStructure(nType, pValue);
}
// PC 0x0061CD00
int CVirtualMachine::StackPopObject(unsigned long* pnValue)
{
    return m_pVirtualMachineInternal->StackPopObject(pnValue);
}
// PC 0x0061CD10
int CVirtualMachine::StackPushObject(unsigned long nValue)
{
    return m_pVirtualMachineInternal->StackPushObject(nValue);
}
// PC 0x0061CD20
int CVirtualMachine::StackPopCommand(void** ppCommand)
{
    return m_pVirtualMachineInternal->StackPopCommand((CVirtualMachineScript**)ppCommand);
}
// PC 0x0061CD30
int CVirtualMachine::SaveScriptSituation(void* pScript, CResGFF* pGFF, CResStruct* pStruct)
{
    return m_pVirtualMachineInternal->SaveScriptSituation((CVirtualMachineScript*)pScript, pGFF, pStruct);
}
// PC 0x0061CD40
int CVirtualMachine::LoadScriptSituation(void** ppScript, CResGFF* pGFF, CResStruct* pStruct)
{
    return m_pVirtualMachineInternal->LoadScriptSituation((CVirtualMachineScript**)ppScript, pGFF, pStruct);
}
// PC 0x0061CD50
void CVirtualMachine::DeleteScriptSituation(void* pScript)
{
    m_pVirtualMachineInternal->DeleteScriptSituation((CVirtualMachineScript*)pScript);
}
// PC 0x0061CD60
CScriptCompiler::~CScriptCompiler()
{
    if (m_pScriptCompilerInternal != NULL)
    {
        delete m_pScriptCompilerInternal;
    }
}

void CScriptCompiler::SetCompileSymbolicOutput(int nValue)
{
    m_pScriptCompilerInternal->SetCompileSymbolicOutput(nValue);
}

void CScriptCompiler::SetCompileDebugLevel(int nValue)
{
    m_pScriptCompilerInternal->SetCompileDebugLevel(nValue);
}

void CScriptCompiler::SetIdentifierSpecification(const CExoString& sLanguageSource)
{
    m_pScriptCompilerInternal->SetIdentifierSpecification(sLanguageSource);
}

void CScriptCompiler::SetOutputAlias(const CExoString& sAlias)
{
    m_pScriptCompilerInternal->SetOutputAlias(sAlias);
}

void CScriptCompiler::SetOptimizeBinaryCodeLength(int bValue)
{
    m_pScriptCompilerInternal->SetOptimizeBinaryCodeLength(bValue);
}

void CScriptCompiler::SetCompileConditionalFile(int bValue)
{
    m_pScriptCompilerInternal->SetCompileConditionalFile(bValue);
}

int CScriptCompiler::CompileFile(const CExoString& sFileName)
{
    return m_pScriptCompilerInternal->CompileFile(sFileName);
}

int CScriptCompiler::CompileScriptChunk(const CExoString& sScriptChunk)
{
    return m_pScriptCompilerInternal->CompileScriptChunk(sScriptChunk);
}

int CScriptCompiler::CompileScriptConditional(const CExoString& sScriptConditional)
{
    return m_pScriptCompilerInternal->CompileScriptConditional(sScriptConditional);
}

int CScriptCompiler::GetCompiledScriptCode(char** ppnCode, int* pnCodeSize)
{
    return m_pScriptCompilerInternal->GetCompiledScriptCode(ppnCode, pnCodeSize);
}

void CScriptCompiler::ClearCompiledScriptCode()
{
    m_pScriptCompilerInternal->ClearCompiledScriptCode();
}
