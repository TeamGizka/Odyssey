#include <stdio.h>
#include "exobase.h"
#include "exoresman.h"
#include "nwscriptinternal.h"

CScriptCompilerIdListEntry::CScriptCompilerIdListEntry()
{
    m_nIdentifierType = 0;
    m_nReturnType = 0;
    m_bImplementationInPlace = FALSE;

    m_nIntegerData = 0;
    m_fFloatData = 0.0f;
    m_fVectorData[0] = 0.0f;
    m_fVectorData[1] = 0.0f;
    m_fVectorData[2] = 0.0f;
    m_nParameters = 0;
    m_nNonOptionalParameters = 0;

    m_nParameterSpace = 0;
    m_pchParameters = NULL;
    m_psStructureParameterNames = NULL;
    m_pbOptionalParameters = NULL;
    m_pnOptionalParameterIntegerData = NULL;
    m_pfOptionalParameterFloatData = NULL;
    m_psOptionalParameterStringData = NULL;
    m_poidOptionalParameterObjectData = NULL;
    m_pfOptionalParameterVectorData = NULL;

    m_nBinarySourceStart = -1;
    m_nBinarySourceFinish = -1;
    m_nBinaryDestinationStart = -1;
    m_nBinaryDestinationFinish = -1;
}
// PC 0x0061CEB0
CScriptCompilerIdListEntry::~CScriptCompilerIdListEntry()
{
    if (m_pchParameters != NULL)
    {
        delete m_pchParameters;
    }
    if (m_psStructureParameterNames != NULL)
    {
        delete[] m_psStructureParameterNames;
    }
    if (m_pbOptionalParameters != NULL)
    {
        delete m_pbOptionalParameters;
    }
    if (m_pnOptionalParameterIntegerData != NULL)
    {
        delete m_pnOptionalParameterIntegerData;
    }
    if (m_pfOptionalParameterFloatData != NULL)
    {
        delete m_pfOptionalParameterFloatData;
    }
    if (m_psOptionalParameterStringData != NULL)
    {
        delete[] m_psOptionalParameterStringData;
    }
    if (m_poidOptionalParameterObjectData != NULL)
    {
        delete m_poidOptionalParameterObjectData;
    }
    if (m_pfOptionalParameterVectorData != NULL)
    {
        delete m_pfOptionalParameterVectorData;
    }
}

int CScriptCompilerIdListEntry::ExpandParameterSpace()
{
    int nNewParameterSpace;

    if (m_nParameterSpace == CSCRIPTCOMPILERIDLISTENTRY_MAX_PARAMETERS)
    {
        return STRREF_CSCRIPTCOMPILER_ERROR_TOO_MANY_PARAMETERS_ON_FUNCTION;
    }

    if (m_nParameterSpace == 0)
    {
        nNewParameterSpace = 4;
    }
    else
    {
        nNewParameterSpace = m_nParameterSpace * 2;
    }

    char* pchNewParameters = new char[nNewParameterSpace];
    CExoString* psNewStructureParameterNames = new CExoString[nNewParameterSpace];
    BOOL* pbNewOptionalParameters = new BOOL[nNewParameterSpace];
    int* pnNewOptionalParameterIntegerData = new int[nNewParameterSpace];
    float* pfNewOptionalParameterFloatData = new float[nNewParameterSpace];
    CExoString* psNewOptionalParameterStringData = new CExoString[nNewParameterSpace];
    unsigned long* poidNewOptionalParameterObjectData = new unsigned long[nNewParameterSpace];
    float* pfNewOptionalParameterVectorData = new float[nNewParameterSpace * 3];

    int nCount;

    if (m_nParameterSpace != 0)
    {
        for (nCount = 0; nCount < m_nParameterSpace; ++nCount)
        {
            pchNewParameters[nCount] = m_pchParameters[nCount];
        }
        for (nCount = 0; nCount < m_nParameterSpace; ++nCount)
        {
            psNewStructureParameterNames[nCount] = m_psStructureParameterNames[nCount];
        }
        for (nCount = 0; nCount < m_nParameterSpace; ++nCount)
        {
            pbNewOptionalParameters[nCount] = m_pbOptionalParameters[nCount];
        }
        for (nCount = 0; nCount < m_nParameterSpace; ++nCount)
        {
            pnNewOptionalParameterIntegerData[nCount] = m_pnOptionalParameterIntegerData[nCount];
        }
        for (nCount = 0; nCount < m_nParameterSpace; ++nCount)
        {
            pfNewOptionalParameterFloatData[nCount] = m_pfOptionalParameterFloatData[nCount];
        }
        for (nCount = 0; nCount < m_nParameterSpace; ++nCount)
        {
            psNewOptionalParameterStringData[nCount] = m_psOptionalParameterStringData[nCount];
        }
        for (nCount = 0; nCount < m_nParameterSpace; ++nCount)
        {
            poidNewOptionalParameterObjectData[nCount] = m_poidOptionalParameterObjectData[nCount];
        }
        for (nCount = 0; nCount < m_nParameterSpace * 3; ++nCount)
        {
            pfNewOptionalParameterVectorData[nCount] = m_pfOptionalParameterVectorData[nCount];
        }
    }

    for (nCount = m_nParameterSpace; nCount < nNewParameterSpace; ++nCount)
    {
        pchNewParameters[nCount] = 0;
        psNewStructureParameterNames[nCount] = "";
        pbNewOptionalParameters[nCount] = FALSE;
        pnNewOptionalParameterIntegerData[nCount] = 0;
        pfNewOptionalParameterFloatData[nCount] = 0.0f;
        psNewOptionalParameterStringData[nCount] = "";
        poidNewOptionalParameterObjectData[nCount] = CSCRIPTCOMPILER_OBJECT_INVALID;
    }

    for (nCount = m_nParameterSpace * 3; nCount < nNewParameterSpace * 3; ++nCount)
    {
        pfNewOptionalParameterVectorData[nCount] = 0.0f;
    }

    m_nParameterSpace = nNewParameterSpace;

    if (m_pchParameters != NULL)
    {
        delete[] m_pchParameters;
    }
    if (m_psStructureParameterNames != NULL)
    {
        delete[] m_psStructureParameterNames;
    }
    if (m_pbOptionalParameters != NULL)
    {
        delete[] m_pbOptionalParameters;
    }
    if (m_pnOptionalParameterIntegerData != NULL)
    {
        delete[] m_pnOptionalParameterIntegerData;
    }
    if (m_pfOptionalParameterFloatData != NULL)
    {
        delete[] m_pfOptionalParameterFloatData;
    }
    if (m_psOptionalParameterStringData != NULL)
    {
        delete[] m_psOptionalParameterStringData;
    }
    if (m_poidOptionalParameterObjectData != NULL)
    {
        delete[] m_poidOptionalParameterObjectData;
    }
    if (m_pfOptionalParameterVectorData != NULL)
    {
        delete[] m_pfOptionalParameterVectorData;
    }

    m_pchParameters = pchNewParameters;
    m_psStructureParameterNames = psNewStructureParameterNames;
    m_pbOptionalParameters = pbNewOptionalParameters;
    m_pnOptionalParameterIntegerData = pnNewOptionalParameterIntegerData;
    m_pfOptionalParameterFloatData = pfNewOptionalParameterFloatData;
    m_psOptionalParameterStringData = psNewOptionalParameterStringData;
    m_poidOptionalParameterObjectData = poidNewOptionalParameterObjectData;
    m_pfOptionalParameterVectorData = pfNewOptionalParameterVectorData;

    return 0;
}

void CSourceScript::UnloadScript()
{
    m_pRes->Release();
}

void CScriptCompilerInternal::InitializePreDefinedStructures()
{

    m_nMaxStructures = 1;

    m_pcStructList[0].m_nByteSize = 12;
    m_pcStructList[0].m_nFieldStart = 0;
    m_pcStructList[0].m_nFieldEnd = 2;
    m_pcStructList[0].m_psName = "vector";

    m_nMaxStructureFields = 3;

    m_pcStructFieldList[0].m_psVarName = "x";
    m_pcStructFieldList[0].m_nLocation = 0;
    m_pcStructFieldList[0].m_pchType = CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT;

    m_pcStructFieldList[1].m_psVarName = "y";
    m_pcStructFieldList[1].m_nLocation = 4;
    m_pcStructFieldList[1].m_pchType = CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT;

    m_pcStructFieldList[2].m_psVarName = "z";
    m_pcStructFieldList[2].m_nLocation = 8;
    m_pcStructFieldList[2].m_pchType = CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT;
}

CScriptCompilerInternal::CScriptCompilerInternal()
{
    m_nCompileSymbolicOutput = 0;
    m_nCompileDebugLevel = 0;

    m_sLanguageSource = "";
    m_sOutputAlias = "OVERRIDE";
    m_nIdentifierListState = 0;
    m_bOptimizeBinaryCodeLength = TRUE;

    m_pSRStack = NULL;
    m_pcIdentifierList = NULL;
    m_pcVarStackList = NULL;
    m_pcStructList = NULL;
    m_pcStructFieldList = NULL;
    m_pSymbolQueryList = NULL;
    m_pSymbolLabelList = NULL;

    m_nCompileFileLevel = 0;
    m_bCompileConditionalFile = FALSE;

    m_nNumEngineDefinedStructures = 0;
    m_pbEngineDefinedStructureValid = NULL;
    m_psEngineDefinedStructureName = NULL;
    m_nIdentifierListEngineStructure = 0;

    Initialize();
}

void CScriptCompilerInternal::Initialize()
{
    m_nLines = 1;
    m_nCharacterOnLine = 1;

    m_nSRStackStates = -1;
    m_nSRStackEntries = CSCRIPTCOMPILER_MAX_STACK_ENTRIES;
    if (m_pSRStack == NULL)
    {
        m_pSRStack = new CScriptCompilerStackEntry[m_nSRStackEntries];
    }
    PushSRStack(0, 0, 0, NULL);

    if (m_pcStructList == NULL)
    {
        m_pcStructList = new CScriptCompilerStructureEntry[CSCRIPTCOMPILER_MAX_STRUCTURES];
    }

    if (m_pcStructFieldList == NULL)
    {
        m_pcStructFieldList = new CScriptCompilerStructureFieldEntry[CSCRIPTCOMPILER_MAX_STRUCTURE_FIELDS];
    }

    m_nMaxStructures = 0;
    m_nMaxStructureFields = 0;
    m_nStructureDefinition = 0;
    m_bGlobalVariableDefinition = FALSE;
    m_nGlobalVariables = 0;
    m_nGlobalVariableSize = 0;

    m_nVarStackRecursionLevel = 0;

    m_nSwitchLevel = 0;
    m_nSwitchIdentifier = 0;
    m_nSwitchStackDepth = 0;
    m_bSwitchLabelDefault = FALSE;
    m_nSwitchLabelNumber = 0;
    m_nSwitchLabelArraySize = CSCRIPTCOMPILER_SWITCH_LABEL_ARRAY_SIZE;
    m_pnSwitchLabelStatements = NULL;

    m_nLoopIdentifier = 0;

    InitializePreDefinedStructures();

    m_bCompileIdentifierConstants = FALSE;
    m_bCompileIdentifierList = FALSE;

    if (m_pcVarStackList == NULL)
    {
        m_pcVarStackList = new CScriptCompilerVarStackEntry[CSCRIPTCOMPILER_MAX_VARIABLES];
    }

    m_nOccupiedVariables = -1;
    m_nVarStackVariableType = CSCRIPTCOMPILER_OPERATION_KEYWORD_DECLARATION;

    m_nStackCurrentDepth = 0;
    m_bAssignmentToVariable = FALSE;
    m_bInStructurePart = FALSE;
    m_nBinaryCodeLength = 0;
    m_bCompilingConditional = FALSE;

    ClearAllSymbolLists();

    m_bFunctionImp = FALSE;
    m_sFunctionImpName = "";
    m_nFunctionImpReturnType = 0;
    m_sFunctionImpReturnStructureName = "";
    m_nFunctionImpAbortStackPointer = 0;

    m_pGlobalVariableParseTree = NULL;
    m_pchOutputCode = NULL;

    TokenInitialize();
}

void CScriptCompilerInternal::InitializeIncludeFile(int nCompileFileLevel)
{

    if (nCompileFileLevel >= 1)
    {
        m_pcIncludeFileStack[nCompileFileLevel - 1].m_nLine = m_nLines;
        m_pcIncludeFileStack[nCompileFileLevel - 1].m_nCharacterOnLine = m_nCharacterOnLine;
        m_pcIncludeFileStack[nCompileFileLevel - 1].m_nTokenStatus = m_nTokenStatus;
        m_pcIncludeFileStack[nCompileFileLevel - 1].m_nTokenCharacters = m_nTokenCharacters;

        m_nLines = 1;
        m_nCharacterOnLine = 1;
        TokenInitialize();
    }
}

void CScriptCompilerInternal::ShutdownIncludeFile(int nCompileFileLevel)
{

    if (nCompileFileLevel >= 1)
    {
        m_nLines = m_pcIncludeFileStack[nCompileFileLevel - 1].m_nLine;
        m_nCharacterOnLine = m_pcIncludeFileStack[nCompileFileLevel - 1].m_nCharacterOnLine;
        m_nTokenStatus = m_pcIncludeFileStack[nCompileFileLevel - 1].m_nTokenStatus;
        m_nTokenCharacters = m_pcIncludeFileStack[nCompileFileLevel - 1].m_nTokenCharacters;
    }
}

void CScriptCompilerInternal::DeleteParseTree(BOOL bStack, CScriptParseTreeNode* pNode)
{
    if (pNode != NULL)
    {
        DeleteParseTree(bStack, pNode->pLeft);
        DeleteParseTree(bStack, pNode->pRight);

        if (bStack == TRUE)
        {

            for (int nCount = 0; nCount <= m_nSRStackStates; ++nCount)
            {
                if (m_pSRStack[nCount].pCurrentTree != NULL)
                {
                    m_pSRStack[nCount].pCurrentTree = NULL;
                }
                if (m_pSRStack[nCount].pReturnTree != NULL)
                {
                    m_pSRStack[nCount].pReturnTree = NULL;
                }
            }
        }

        delete pNode;
    }
}

int CScriptCompilerInternal::OutputError(int nError, const CExoString& sFileName,
                                         int nLineNumber,
                                         const CExoString& sErrorText)
{
    CExoString sFullErrorString;

    if (sFileName.Left(1) == "!")
    {
        if (nLineNumber > 0)
        {
            sFullErrorString.Format("%s(%d): %s\n",
                                    sFileName.Right(sFileName.GetLength() - 1).CStr(),
                                    nLineNumber, sErrorText.CStr());
        }
        else
        {
            sFullErrorString.Format("%s: %s\n",
                                    sFileName.Right(sFileName.GetLength() - 1).CStr(),
                                    sErrorText.CStr());
        }
    }
    else
    {
        if (nLineNumber > 0)
        {
            sFullErrorString.Format("%s.nss(%d): %s\n", sFileName.CStr(),
                                    nLineNumber, sErrorText.CStr());
        }
        else
        {
            sFullErrorString.Format("%s.nss: %s\n", sFileName.CStr(),
                                    sErrorText.CStr());
        }
    }

    return nError;
}

int CSourceScript::LoadScript(const CExoString& sFileName, char** ppScript,
                              unsigned long* pnScriptLength)
{
    SetResRef(CResRef(sFileName), FALSE);

    if (!m_pRes->Demand())
    {
        return STRREF_CSCRIPTCOMPILER_ERROR_FILE_NOT_FOUND;
    }

    *ppScript = m_pRes->GetNSSDataPtr();
    *pnScriptLength = m_pRes->GetNSSSize();
    return 0;
}
// PC 0x0061D010
void CScriptCompilerInternal::ShutDown()
{
    if (m_pSRStack != NULL)
    {
        delete m_pSRStack;
    }
    if (m_pcIdentifierList != NULL)
    {
        delete[] m_pcIdentifierList;
    }
    if (m_pcVarStackList != NULL)
    {
        delete[] m_pcVarStackList;
    }
    if (m_pcStructList != NULL)
    {
        delete[] m_pcStructList;
        m_pcStructList = NULL;
    }
    if (m_pcStructFieldList != NULL)
    {
        delete[] m_pcStructFieldList;
        m_pcStructFieldList = NULL;
    }
    if (m_psEngineDefinedStructureName != NULL)
    {
        delete[] m_psEngineDefinedStructureName;
        m_psEngineDefinedStructureName = NULL;
    }
    if (m_pbEngineDefinedStructureValid != NULL)
    {
        delete m_pbEngineDefinedStructureValid;
        m_pbEngineDefinedStructureValid = NULL;
    }
}
// PC 0x0061D120
CScriptCompilerInternal::~CScriptCompilerInternal()
{
    ShutDown();
}

void CScriptCompilerInternal::SetCompileDebugLevel(int nValue)
{
    m_nCompileDebugLevel = nValue;
}

void CScriptCompilerInternal::SetCompileSymbolicOutput(int nValue)
{
    m_nCompileSymbolicOutput = nValue;
}

void CScriptCompilerInternal::SetIdentifierSpecification(const CExoString& sLanguageSource)
{
    if (m_sLanguageSource != sLanguageSource)
    {
        m_sLanguageSource = sLanguageSource;

        if (m_pcIdentifierList != NULL)
        {
            delete[] m_pcIdentifierList;
        }

        if (m_pcIdentifierList == NULL)
        {
            m_pcIdentifierList = new CScriptCompilerIdListEntry[CSCRIPTCOMPILER_MAX_IDENTIFIERS];
            m_bCompileIdentifierList = TRUE;
            m_bCompileIdentifierConstants = TRUE;
            m_nOccupiedIdentifiers = 0;
            m_nMaxPredefinedIdentifierId = 0;
            ParseIdentifierFile();

            m_nLines = 1;
            m_nCharacterOnLine = 1;
            m_nSRStackStates = -1;

            m_bCompileIdentifierList = FALSE;
            m_bCompileIdentifierConstants = FALSE;
        }
    }
}

void CScriptCompilerInternal::SetOutputAlias(const CExoString& sAlias)
{
    m_sOutputAlias = sAlias;
}

void CScriptCompilerInternal::SetOptimizeBinaryCodeLength(BOOL bValue)
{
    m_bOptimizeBinaryCodeLength = bValue;
}

void CScriptCompilerInternal::SetCompileConditionalFile(BOOL bValue)
{
    m_bCompileConditionalFile = bValue;
}

int CScriptCompilerInternal::CompileFile(const CExoString& sFileName)
{
    char* pScript;
    unsigned long nScriptLength;

    if (m_nCompileFileLevel == 0)
    {
        Initialize();
    }

    if (m_nCompileFileLevel >= CSCRIPTCOMPILER_INCLUDE_LEVELS)
    {
        return STRREF_CSCRIPTCOMPILER_ERROR_INCLUDE_TOO_MANY_LEVELS;
    }

    if (m_nCompileFileLevel > 0)
    {
        for (int nCount = 0; nCount < m_nCompileFileLevel; ++nCount)
        {
            if (m_pcIncludeFileStack[nCount].m_sFileName == sFileName)
            {
                EXOLOG("Recursive #include of %s.\n", sFileName.CStr());
                for (int nCount2 = m_nCompileFileLevel - 1; nCount2 >= 0; --nCount2)
                {
                    EXOLOG("  included from %s\n", m_pcIncludeFileStack[nCount2].m_sFileName.CStr());
                }
                return STRREF_CSCRIPTCOMPILER_ERROR_INCLUDE_RECURSIVE;
            }
        }

        InitializeIncludeFile(m_nCompileFileLevel);
    }

    m_pcIncludeFileStack[m_nCompileFileLevel].m_sFileName = sFileName;
    EXOLOG("Compiling %s\n", sFileName.CStr());

    int nReturnValue = m_pcIncludeFileStack[m_nCompileFileLevel].m_cSourceScript.LoadScript(sFileName, &pScript, &nScriptLength);
    if (nReturnValue < 0)
    {
        if (m_nCompileFileLevel > 0)
        {
            ShutdownIncludeFile(m_nCompileFileLevel);
        }
        return nReturnValue;
    }

    ++m_nCompileFileLevel;

    nReturnValue = ParseSource(pScript, nScriptLength);
    if (nReturnValue < 0)
    {
        m_pcIncludeFileStack[m_nCompileFileLevel].m_cSourceScript.UnloadScript();
        return nReturnValue;
    }

    m_pcIncludeFileStack[m_nCompileFileLevel - 1].m_cSourceScript.UnloadScript();
    --m_nCompileFileLevel;

    if (m_nCompileFileLevel > 0)
    {
        ShutdownIncludeFile(m_nCompileFileLevel);
        return 0;
    }

    InitializeFinalCode();

    if (m_bCompileConditionalFile == TRUE)
    {
        m_bCompilingConditional = TRUE;
    }

    nReturnValue = GenerateFinalCodeFromParseTree();

    if (m_bCompileConditionalFile == TRUE)
    {
        m_bCompilingConditional = FALSE;
    }

    if (nReturnValue < 0)
    {
        return nReturnValue;
    }

    FinalizeFinalCode();

    return WriteFinalCodeToFile(sFileName);
}

int CScriptCompilerInternal::CompileScriptChunk(const CExoString& sScriptChunk)
{
    Initialize();

    if (m_nCompileFileLevel != 0)
    {
        return STRREF_CSCRIPTCOMPILER_ERROR_INCLUDE_TOO_MANY_LEVELS;
    }

    m_pcIncludeFileStack[m_nCompileFileLevel].m_sFileName = "!Chunk";

    int nScriptLength = sScriptChunk.GetLength() + 13;
    char* pScript = new char[nScriptLength + 13];
    sprintf(pScript, "void main(){%s}", sScriptChunk.CStr());

    ++m_nCompileFileLevel;

    int nReturnValue = ParseSource(pScript, nScriptLength);
    if (nReturnValue < 0)
    {
        return nReturnValue;
    }

    --m_nCompileFileLevel;

    InitializeFinalCode();

    nReturnValue = GenerateFinalCodeFromParseTree();
    if (nReturnValue < 0)
    {
        return nReturnValue;
    }

    FinalizeFinalCode();
    return 0;
}

int CScriptCompilerInternal::CompileScriptConditional(const CExoString& sScriptConditional)
{
    Initialize();

    if (m_nCompileFileLevel != 0)
    {
        return STRREF_CSCRIPTCOMPILER_ERROR_INCLUDE_TOO_MANY_LEVELS;
    }

    m_pcIncludeFileStack[m_nCompileFileLevel].m_sFileName = "!Conditional";

    int nScriptLength = sScriptConditional.GetLength() + 22;
    char* pScript = new char[nScriptLength + 22];
    sprintf(pScript, "int main(){ return(%s);}", sScriptConditional.CStr());

    ++m_nCompileFileLevel;

    int nReturnValue = ParseSource(pScript, nScriptLength);
    if (nReturnValue < 0)
    {
        return nReturnValue;
    }

    --m_nCompileFileLevel;

    InitializeFinalCode();

    m_bCompilingConditional = TRUE;
    nReturnValue = GenerateFinalCodeFromParseTree();
    m_bCompilingConditional = FALSE;

    if (nReturnValue < 0)
    {
        return nReturnValue;
    }

    FinalizeFinalCode();
    return 0;
}

int CScriptCompilerInternal::GetCompiledScriptCode(char** ppnCode, int* pnCodeSize)
{
    *pnCodeSize = m_nOutputCodeSize;
    *ppnCode = m_pchOutputCode;
    return 0;
}

void CScriptCompilerInternal::ClearCompiledScriptCode()
{
    m_nOutputCodeSize = 0;
    if (m_pchOutputCode != NULL)
    {
        delete[] m_pchOutputCode;
    }
}
