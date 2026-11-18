#include "nwscript.h"
#include "nwvmachineinternal.h"
#include "nwscriptinternal.h"

// Dead code: not in the PC executable
CScriptCompiler::CScriptCompiler()
{
    m_pScriptCompilerInternal = new CScriptCompilerInternal;
}
// PC 0x0061CAB0 GOG 0x0061CA90 Xbox 0x0016A940
CVirtualMachine::CVirtualMachine()
{
    m_pVirtualMachineInternal = new CVirtualMachineInternal;
}

// PC 0x0061CB30 GOG 0x0061CB10 Xbox 0x0016A980
CVirtualMachine::~CVirtualMachine()
{
    if (m_pVirtualMachineInternal != NULL)
    {
        delete m_pVirtualMachineInternal;
    }
}
// PC 0x0061CB90 GOG 0x0061CB70 Xbox 0x0016A780
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
// PC 0x0061CC00 GOG 0x0061CBE0 Xbox 0x0016A7C0
int CVirtualMachine::RunScript(CExoString* psScript, unsigned long nObjectId, int nInstructionLimit)
{
    return m_pVirtualMachineInternal->RunScript(psScript, nObjectId, nInstructionLimit);
}
// PC 0x0061CC10 GOG 0x0061CBF0 Xbox 0x0016A7D0
int CVirtualMachine::RunScriptSituation(void* pScript, unsigned long nObjectId, int bOneInstruction)
{
    return m_pVirtualMachineInternal->RunScriptSituation((CVirtualMachineScript*)pScript, nObjectId, bOneInstruction);
}
// PC 0x0061CC20 GOG 0x0061CC00 Xbox 0x0016A7E0
int CVirtualMachine::GetRunScriptReturnValue(int* pnType, void** ppValue)
{
    return m_pVirtualMachineInternal->GetRunScriptReturnValue(pnType, ppValue);
}
// PC 0x0061CC30 GOG 0x0061CC10 Xbox 0x0016A7F0
void CVirtualMachine::SetCommandImplementer(CVirtualMachineCmdImplementer* pImplementer)
{
    m_pVirtualMachineInternal->SetCommandImplementer(pImplementer);
}
// PC 0x0061CC40 GOG 0x0061CC20 Xbox 0x0016A800
int CVirtualMachine::StackPopInteger(int* pnValue)
{
    return m_pVirtualMachineInternal->StackPopInteger(pnValue);
}
// PC 0x0061CC50 GOG 0x0061CC30 Xbox 0x0016A810
int CVirtualMachine::StackPushInteger(int nValue)
{
    return m_pVirtualMachineInternal->StackPushInteger(nValue);
}
// PC 0x0061CC60 GOG 0x0061CC40 Xbox 0x0016A820
int CVirtualMachine::StackPopFloat(float* pfValue)
{
    return m_pVirtualMachineInternal->StackPopFloat(pfValue);
}
// PC 0x0061CC70 GOG 0x0061CC50 Xbox 0x0016A830
int CVirtualMachine::StackPushFloat(float fValue)
{
    return m_pVirtualMachineInternal->StackPushFloat(fValue);
}
// PC 0x0061CC80 GOG 0x0061CC60 Xbox 0x0016A840
int CVirtualMachine::StackPopVector(Vector* pvValue)
{
    return m_pVirtualMachineInternal->StackPopVector(pvValue);
}
// PC 0x0061CC90 GOG 0x0061CC70 Xbox 0x0016A850
int CVirtualMachine::StackPushVector(Vector vValue)
{
    return m_pVirtualMachineInternal->StackPushVector(vValue);
}
// PC 0x0061CCC0 GOG 0x0061CCA0 Xbox 0x0016A880
int CVirtualMachine::StackPopString(CExoString* psValue)
{
    return m_pVirtualMachineInternal->StackPopString(psValue);
}
// PC 0x0061CCD0 GOG 0x0061CCB0 Xbox 0x0016A890
int CVirtualMachine::StackPushString(const CExoString& sValue)
{
    return m_pVirtualMachineInternal->StackPushString(sValue);
}
// PC 0x0061CCE0 GOG 0x0061CCC0 Xbox 0x0016A8A0
int CVirtualMachine::StackPopEngineStructure(int nType, void** ppValue)
{
    return m_pVirtualMachineInternal->StackPopEngineStructure(nType, ppValue);
}
// PC 0x0061CCF0 GOG 0x0061CCD0 Xbox 0x0016A8B0
int CVirtualMachine::StackPushEngineStructure(int nType, void* pValue)
{
    return m_pVirtualMachineInternal->StackPushEngineStructure(nType, pValue);
}
// PC 0x0061CD00 GOG 0x0061CCE0 Xbox 0x0016A8C0
int CVirtualMachine::StackPopObject(unsigned long* pnValue)
{
    return m_pVirtualMachineInternal->StackPopObject(pnValue);
}
// PC 0x0061CD10 GOG 0x0061CCF0 Xbox 0x0016A8D0
int CVirtualMachine::StackPushObject(unsigned long nValue)
{
    return m_pVirtualMachineInternal->StackPushObject(nValue);
}
// PC 0x0061CD20 GOG 0x0061CD00 Xbox 0x0016A8E0
int CVirtualMachine::StackPopCommand(void** ppCommand)
{
    return m_pVirtualMachineInternal->StackPopCommand((CVirtualMachineScript**)ppCommand);
}
// PC 0x0061CD30 GOG 0x0061CD10 Xbox 0x0016A8F0
int CVirtualMachine::SaveScriptSituation(void* pScript, CResGFF* pGFF, CResStruct* pStruct)
{
    return m_pVirtualMachineInternal->SaveScriptSituation((CVirtualMachineScript*)pScript, pGFF, pStruct);
}
// PC 0x0061CD40 GOG 0x0061CD20 Xbox 0x0016A900
int CVirtualMachine::LoadScriptSituation(void** ppScript, CResGFF* pGFF, CResStruct* pStruct)
{
    return m_pVirtualMachineInternal->LoadScriptSituation((CVirtualMachineScript**)ppScript, pGFF, pStruct);
}
// PC 0x0061CD50 GOG 0x0061CD30 Xbox 0x0016A910
void CVirtualMachine::DeleteScriptSituation(void* pScript)
{
    m_pVirtualMachineInternal->DeleteScriptSituation((CVirtualMachineScript*)pScript);
}
// PC 0x0061CD60 GOG 0x0061CD40 Xbox 0x0016A920
CScriptCompiler::~CScriptCompiler()
{
    if (m_pScriptCompilerInternal != NULL)
    {
        delete m_pScriptCompilerInternal;
    }
}

// Dead code: not in the PC executable
void CScriptCompiler::SetCompileSymbolicOutput(int nValue)
{
    m_pScriptCompilerInternal->SetCompileSymbolicOutput(nValue);
}

// Dead code: not in the PC executable
void CScriptCompiler::SetCompileDebugLevel(int nValue)
{
    m_pScriptCompilerInternal->SetCompileDebugLevel(nValue);
}

// Dead code: not in the PC executable
void CScriptCompiler::SetIdentifierSpecification(const CExoString& sLanguageSource)
{
    m_pScriptCompilerInternal->SetIdentifierSpecification(sLanguageSource);
}

// Dead code: not in the PC executable
void CScriptCompiler::SetOutputAlias(const CExoString& sAlias)
{
    m_pScriptCompilerInternal->SetOutputAlias(sAlias);
}

// Dead code: not in the PC executable
void CScriptCompiler::SetOptimizeBinaryCodeLength(int bValue)
{
    m_pScriptCompilerInternal->SetOptimizeBinaryCodeLength(bValue);
}

// Dead code: not in the PC executable
void CScriptCompiler::SetCompileConditionalFile(int bValue)
{
    m_pScriptCompilerInternal->SetCompileConditionalFile(bValue);
}

// Dead code: not in the PC executable
int CScriptCompiler::CompileFile(const CExoString& sFileName)
{
    return m_pScriptCompilerInternal->CompileFile(sFileName);
}

// Dead code: not in the PC executable
int CScriptCompiler::CompileScriptChunk(const CExoString& sScriptChunk)
{
    return m_pScriptCompilerInternal->CompileScriptChunk(sScriptChunk);
}

// Dead code: not in the PC executable
int CScriptCompiler::CompileScriptConditional(const CExoString& sScriptConditional)
{
    return m_pScriptCompilerInternal->CompileScriptConditional(sScriptConditional);
}

// Dead code: not in the PC executable
int CScriptCompiler::GetCompiledScriptCode(char** ppnCode, int* pnCodeSize)
{
    return m_pScriptCompilerInternal->GetCompiledScriptCode(ppnCode, pnCodeSize);
}

// Dead code: not in the PC executable
void CScriptCompiler::ClearCompiledScriptCode()
{
    m_pScriptCompilerInternal->ClearCompiledScriptCode();
}
