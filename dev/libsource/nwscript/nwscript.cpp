#include "nwscript.h"
#include "nwvmachineinternal.h"
#include "nwscriptinternal.h"

CScriptCompiler::CScriptCompiler()
{
    m_pScriptCompilerInternal = new CScriptCompilerInternal;
}
// PC 0x005D0F40  Xbox 0x00157B50
CVirtualMachine::CVirtualMachine()
{
    m_pVirtualMachineInternal = new CVirtualMachineInternal;
}
// PC 0x005D0FB0
CVirtualMachine::~CVirtualMachine()
{
    if (m_pVirtualMachineInternal != NULL)
    {
        delete m_pVirtualMachineInternal;
    }
}
// PC 0x005D0FC0  Xbox 0x00157BC0
int CVirtualMachine::RunScript(CExoString* psScript, unsigned long nObjectId, int nInstructionLimit)
{
    return m_pVirtualMachineInternal->RunScript(psScript, nObjectId, nInstructionLimit);
}
// PC 0x005D0FD0  Xbox 0x00157BD0
int CVirtualMachine::RunScriptSituation(void* pScript, unsigned long nObjectId, int bOneInstruction)
{
    return m_pVirtualMachineInternal->RunScriptSituation((CVirtualMachineScript*)pScript, nObjectId, bOneInstruction);
}
// PC 0x005D0FE0  Xbox 0x00157BE0
int CVirtualMachine::GetRunScriptReturnValue(int* pnType, void** ppValue)
{
    return m_pVirtualMachineInternal->GetRunScriptReturnValue(pnType, ppValue);
}
// PC 0x005D0FF0  Xbox 0x00157BF0
void CVirtualMachine::SetCommandImplementer(CVirtualMachineCmdImplementer* pImplementer)
{
    m_pVirtualMachineInternal->SetCommandImplementer(pImplementer);
}
// PC 0x005D1000  Xbox 0x00157C00
int CVirtualMachine::StackPopInteger(int* pnValue)
{
    return m_pVirtualMachineInternal->StackPopInteger(pnValue);
}
// PC 0x005D1010  Xbox 0x00157C10
int CVirtualMachine::StackPushInteger(int nValue)
{
    return m_pVirtualMachineInternal->StackPushInteger(nValue);
}
// PC 0x005D1020  Xbox 0x00157C20
int CVirtualMachine::StackPopFloat(float* pfValue)
{
    return m_pVirtualMachineInternal->StackPopFloat(pfValue);
}
// PC 0x005D1030  Xbox 0x00157C30
int CVirtualMachine::StackPushFloat(float fValue)
{
    return m_pVirtualMachineInternal->StackPushFloat(fValue);
}
// PC 0x005D1040  Xbox 0x00157C40
int CVirtualMachine::StackPopVector(Vector* pvValue)
{
    return m_pVirtualMachineInternal->StackPopVector(pvValue);
}
// PC 0x005D1050  Xbox 0x00157C50
int CVirtualMachine::StackPushVector(Vector vValue)
{
    return m_pVirtualMachineInternal->StackPushVector(vValue);
}
// PC 0x005D1080  Xbox 0x00157C80
int CVirtualMachine::StackPopString(CExoString* psValue)
{
    return m_pVirtualMachineInternal->StackPopString(psValue);
}
// PC 0x005D1090  Xbox 0x00157C90
int CVirtualMachine::StackPushString(const CExoString& sValue)
{
    return m_pVirtualMachineInternal->StackPushString(sValue);
}
// PC 0x005D10A0  Xbox 0x00157CA0
int CVirtualMachine::StackPopEngineStructure(int nType, void** ppValue)
{
    return m_pVirtualMachineInternal->StackPopEngineStructure(nType, ppValue);
}
// PC 0x005D10B0  Xbox 0x00157CB0
int CVirtualMachine::StackPushEngineStructure(int nType, void* pValue)
{
    return m_pVirtualMachineInternal->StackPushEngineStructure(nType, pValue);
}
// PC 0x005D10C0  Xbox 0x00157CC0
int CVirtualMachine::StackPopObject(unsigned long* pnValue)
{
    return m_pVirtualMachineInternal->StackPopObject(pnValue);
}
// PC 0x005D10D0  Xbox 0x00157CD0
int CVirtualMachine::StackPushObject(unsigned long nValue)
{
    return m_pVirtualMachineInternal->StackPushObject(nValue);
}
// PC 0x005D10E0  Xbox 0x00157CE0
int CVirtualMachine::StackPopCommand(void** ppCommand)
{
    return m_pVirtualMachineInternal->StackPopCommand((CVirtualMachineScript**)ppCommand);
}
// PC 0x005D10F0  Xbox 0x00157CF0
int CVirtualMachine::SaveScriptSituation(void* pScript, CResGFF* pGFF, CResStruct* pStruct)
{
    return m_pVirtualMachineInternal->SaveScriptSituation((CVirtualMachineScript*)pScript, pGFF, pStruct);
}
// PC 0x005D1100  Xbox 0x00157D00
int CVirtualMachine::LoadScriptSituation(void** ppScript, CResGFF* pGFF, CResStruct* pStruct)
{
    return m_pVirtualMachineInternal->LoadScriptSituation((CVirtualMachineScript**)ppScript, pGFF, pStruct);
}
// PC 0x005D1110  Xbox 0x00157D10
void CVirtualMachine::DeleteScriptSituation(void* pScript)
{
    m_pVirtualMachineInternal->DeleteScriptSituation((CVirtualMachineScript*)pScript);
}
// PC 0x005D1120  Xbox 0x00157D20
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
