#include <stdio.h>
#include <string.h>
#include "exobase.h"
#include "exoresman.h"
#include "exotlktable.h"
#include "nwvmachineinternal.h"
#include "nwscriptinternal.h"

int CScriptCompilerInternal::WriteResolvedOutput()
{
    char* pchResolvedOutput = new char[m_nFinalBinarySize];
    memset(pchResolvedOutput, 0, m_nFinalBinarySize);

    memcpy(pchResolvedOutput, m_pchOutputCode, CSCRIPTCOMPILER_BINARY_SCRIPT_HEADER);

    for (int nCount = m_nMaxPredefinedIdentifierId; nCount < m_nOccupiedIdentifiers; ++nCount)
    {
        if (m_pcIdentifierList[nCount].m_nBinaryDestinationStart != -1)
        {
            memcpy(pchResolvedOutput + m_pcIdentifierList[nCount].m_nBinaryDestinationStart,
                   m_pchOutputCode + m_pcIdentifierList[nCount].m_nBinarySourceStart,
                   m_pcIdentifierList[nCount].m_nBinaryDestinationFinish -
                   m_pcIdentifierList[nCount].m_nBinaryDestinationStart);
        }
    }

    if (m_pchOutputCode != NULL)
    {
        delete[] m_pchOutputCode;
    }
    m_pchOutputCode = pchResolvedOutput;
    m_nOutputCodeLength = m_nFinalBinarySize;
    m_nOutputCodeSize = m_nFinalBinarySize;

    return 0;
}

void CScriptCompilerInternal::InitializeFinalCode()
{
    if (m_pchOutputCode != NULL)
    {
        delete[] m_pchOutputCode;
    }

    m_pchOutputCode = new char[CSCRIPTCOMPILER_OUTPUT_CODE_BLOCK_SIZE];
    m_nOutputCodeSize = CSCRIPTCOMPILER_OUTPUT_CODE_BLOCK_SIZE;

    sprintf(m_pchOutputCode, "NCS V1.0");
    m_nBinaryCodeLength = CSCRIPTCOMPILER_BINARY_SCRIPT_HEADER;

    if (m_nCompileSymbolicOutput == 0)
    {
        m_nOutputCodeLength = CSCRIPTCOMPILER_BINARY_SCRIPT_HEADER;
    }
    else
    {
        m_nOutputCodeLength = CSCRIPTCOMPILER_SYMBOLIC_SCRIPT_HEADER;
    }
}

void CScriptCompilerInternal::FinalizeFinalCode()
{
    if (m_nCompileSymbolicOutput == 0)
    {
        m_pchOutputCode[8] = 'B';
        m_pchOutputCode[9] = (char) ((m_nOutputCodeLength >> 24) & 0xff);
        m_pchOutputCode[10] = (char) ((m_nOutputCodeLength >> 16) & 0xff);
        m_pchOutputCode[11] = (char) ((m_nOutputCodeLength >> 8) & 0xff);
        m_pchOutputCode[12] = (char) (m_nOutputCodeLength & 0xff);
    }
    else
    {
        sprintf(m_pchOutputCode + 8, "T %08x\n", m_nOutputCodeLength);
        m_pchOutputCode[19] = '\n';
    }
}

int CScriptCompilerInternal::WriteFinalCodeToFile(const CExoString& sFileName)
{
    CExoString sModifiedFileName;
    sModifiedFileName.Format("%s:%s", m_sOutputAlias.CStr(), sFileName.CStr());

    CExoFile* pFile;
    if (m_nCompileSymbolicOutput == 0)
    {
        pFile = new CExoFile(sModifiedFileName, RESTYPE_NCS, "wb");
    }
    else
    {
        pFile = new CExoFile(sModifiedFileName, RESTYPE_NCS, "w");
    }

    if (pFile->FileOpened() == FALSE)
    {
        delete pFile;
        return STRREF_CSCRIPTCOMPILER_ERROR_UNABLE_TO_OPEN_FILE_FOR_WRITING;
    }

    pFile->Write(m_pchOutputCode, 1, m_nOutputCodeLength);
    delete pFile;

    CExoString sDirectoryFileName;
    sDirectoryFileName.Format("%s:", m_sOutputAlias.CStr());
    g_pExoResMan->UpdateResourceDirectory(sDirectoryFileName.CStr());

    if (m_pchOutputCode != NULL)
    {
        delete[] m_pchOutputCode;
    }
    m_pchOutputCode = NULL;
    m_nOutputCodeLength = 0;

    return 0;
}

void CScriptCompilerInternal::ClearUserDefinedIdentifiers()
{
    for (int nCount = m_nMaxPredefinedIdentifierId; nCount < m_nOccupiedIdentifiers; ++nCount)
    {
        m_pcIdentifierList[nCount].m_psIdentifier = "";
        m_pcIdentifierList[nCount].m_nIdentifierLength = 0;
        m_pcIdentifierList[nCount].m_nIdentifierType = 0;
        m_pcIdentifierList[nCount].m_nReturnType = 0;
        m_pcIdentifierList[nCount].m_bImplementationInPlace = FALSE;
        m_pcIdentifierList[nCount].m_psStructureReturnName = "";

        m_pcIdentifierList[nCount].m_psStringData = "";
        m_pcIdentifierList[nCount].m_nIntegerData = 0;
        m_pcIdentifierList[nCount].m_fFloatData = 0.0f;

        m_pcIdentifierList[nCount].m_nIdIdentifier = -1;
        m_pcIdentifierList[nCount].m_nParameters = 0;
        m_pcIdentifierList[nCount].m_nNonOptionalParameters = 0;
        m_pcIdentifierList[nCount].m_nParameterSpace = 0;

        if (m_pcIdentifierList[nCount].m_pchParameters != NULL)
        {
            delete[] m_pcIdentifierList[nCount].m_pchParameters;
            m_pcIdentifierList[nCount].m_pchParameters = NULL;
        }
        if (m_pcIdentifierList[nCount].m_psStructureParameterNames != NULL)
        {
            delete[] m_pcIdentifierList[nCount].m_psStructureParameterNames;
            m_pcIdentifierList[nCount].m_psStructureParameterNames = NULL;
        }
        if (m_pcIdentifierList[nCount].m_pbOptionalParameters != NULL)
        {
            delete[] m_pcIdentifierList[nCount].m_pbOptionalParameters;
            m_pcIdentifierList[nCount].m_pbOptionalParameters = NULL;
        }
        if (m_pcIdentifierList[nCount].m_pnOptionalParameterIntegerData != NULL)
        {
            delete[] m_pcIdentifierList[nCount].m_pnOptionalParameterIntegerData;
            m_pcIdentifierList[nCount].m_pnOptionalParameterIntegerData = NULL;
        }
        if (m_pcIdentifierList[nCount].m_pfOptionalParameterFloatData != NULL)
        {
            delete[] m_pcIdentifierList[nCount].m_pfOptionalParameterFloatData;
            m_pcIdentifierList[nCount].m_pfOptionalParameterFloatData = NULL;
        }
        if (m_pcIdentifierList[nCount].m_psOptionalParameterStringData != NULL)
        {
            delete[] m_pcIdentifierList[nCount].m_psOptionalParameterStringData;
            m_pcIdentifierList[nCount].m_psOptionalParameterStringData = NULL;
        }
        if (m_pcIdentifierList[nCount].m_poidOptionalParameterObjectData != NULL)
        {
            delete[] m_pcIdentifierList[nCount].m_poidOptionalParameterObjectData;
            m_pcIdentifierList[nCount].m_poidOptionalParameterObjectData = NULL;
        }
        if (m_pcIdentifierList[nCount].m_pfOptionalParameterVectorData != NULL)
        {
            delete[] m_pcIdentifierList[nCount].m_pfOptionalParameterVectorData;
            m_pcIdentifierList[nCount].m_pfOptionalParameterVectorData = NULL;
        }

        m_pcIdentifierList[nCount].m_nBinarySourceStart = -1;
        m_pcIdentifierList[nCount].m_nBinarySourceFinish = -1;
        m_pcIdentifierList[nCount].m_nBinaryDestinationStart = -1;
        m_pcIdentifierList[nCount].m_nBinaryDestinationFinish = -1;
    }

    m_nOccupiedIdentifiers = m_nMaxPredefinedIdentifierId;
}

int CScriptCompilerInternal::OutputIdentifierError(const CExoString& sFunctionName, int nError)
{
    STR_RES strRes;
    g_pTlkTable->Fetch(-nError, strRes, CTlkTable::GENDER_MALE);

    CExoString sFileName = m_pcIncludeFileStack[m_nCompileFileLevel].m_sFileName;
    CExoString sErrorText;
    sErrorText.Format("%s (%s)", strRes.m_sString.CStr(), sFunctionName.CStr());

    return OutputError(nError, m_pcIncludeFileStack[m_nCompileFileLevel].m_sFileName, 0, sErrorText);
}

int CScriptCompilerInternal::GetIdentifierByName(const CExoString& sIdentifierName)
{
    int nLength = sIdentifierName.GetLength();

    for (int nCount = 0; nCount <= m_nOccupiedIdentifiers; ++nCount)
    {
        if (m_pcIdentifierList[nCount].m_nIdentifierLength == nLength &&
            m_pcIdentifierList[nCount].m_psIdentifier == sIdentifierName)
        {
            return nCount;
        }
    }

    return STRREF_CSCRIPTCOMPILER_ERROR_UNDEFINED_IDENTIFIER;
}

int CScriptCompilerInternal::ValidateLocationOfIdentifier(const CExoString& sFunctionName)
{
    int nIdentifier = GetIdentifierByName(sFunctionName);
    if (nIdentifier < 0)
    {
        return OutputIdentifierError(sFunctionName, nIdentifier);
    }

    if (m_pcIdentifierList[nIdentifier].m_nBinaryDestinationStart != -1)
    {
        return 0;
    }

    int nFunctionSize = m_pcIdentifierList[nIdentifier].m_nBinarySourceFinish -
                        m_pcIdentifierList[nIdentifier].m_nBinarySourceStart;

    m_pcIdentifierList[nIdentifier].m_nBinaryDestinationStart = m_nFinalBinarySize;
    m_pcIdentifierList[nIdentifier].m_nBinaryDestinationFinish = m_nFinalBinarySize + nFunctionSize;
    m_nFinalBinarySize += nFunctionSize;

    CExoString sNewFunctionName;
    for (int nCount = 0; nCount < m_nSymbolQueryList; ++nCount)
    {
        if (m_pSymbolQueryList[nCount].m_nLocationPointer >= m_pcIdentifierList[nIdentifier].m_nBinarySourceStart &&
            m_pSymbolQueryList[nCount].m_nLocationPointer < m_pcIdentifierList[nIdentifier].m_nBinarySourceFinish)
        {
            if (m_pSymbolQueryList[nCount].m_sSymbolName.Left(2) == "FE")
            {
                sNewFunctionName = m_pSymbolQueryList[nCount].m_sSymbolName.Right(m_pSymbolQueryList[nCount].m_sSymbolName.GetLength() - 3);
                int nReturnValue = ValidateLocationOfIdentifier(sNewFunctionName);
                if (nReturnValue < 0)
                {
                    return nReturnValue;
                }
            }
        }
    }

    return 0;
}

int CScriptCompilerInternal::DetermineLocationOfCode()
{
    if (m_bOptimizeBinaryCodeLength == TRUE)
    {

        m_nFinalBinarySize = CSCRIPTCOMPILER_BINARY_SCRIPT_HEADER;
        return ValidateLocationOfIdentifier("#loader");
    }

    m_nFinalBinarySize = m_nOutputCodeLength;

    for (int nCount = m_nMaxPredefinedIdentifierId; nCount <= m_nOccupiedIdentifiers; ++nCount)
    {
        if (m_pcIdentifierList[nCount].m_nBinarySourceStart != -1)
        {
            m_pcIdentifierList[nCount].m_nBinaryDestinationStart = m_pcIdentifierList[nCount].m_nBinarySourceStart;
            m_pcIdentifierList[nCount].m_nBinaryDestinationFinish = m_pcIdentifierList[nCount].m_nBinarySourceFinish;
        }
    }

    return 0;
}

int CScriptCompilerInternal::ResolveLabels()
{
    CExoString sFunctionName;

    for (int nCount = 0; nCount < m_nSymbolQueryList; ++nCount)
    {
        int nResolvedLocationOfAddress = m_pSymbolQueryList[nCount].m_nLocationPointer;

        int nQueryIdentifier;
        BOOL bFoundIdentifier = FALSE;
        for (int nCount2 = m_nMaxPredefinedIdentifierId; nCount2 <= m_nOccupiedIdentifiers; ++nCount2)
        {
            if (nResolvedLocationOfAddress >= m_pcIdentifierList[nCount2].m_nBinarySourceStart &&
                nResolvedLocationOfAddress < m_pcIdentifierList[nCount2].m_nBinarySourceFinish)
            {
                nQueryIdentifier = nCount2;
                bFoundIdentifier = TRUE;
            }
        }

        if (bFoundIdentifier == FALSE)
        {
            return STRREF_CSCRIPTCOMPILER_ERROR_UNKNOWN_STATE_IN_COMPILER;
        }

        if (m_pcIdentifierList[nQueryIdentifier].m_nBinaryDestinationStart != -1)
        {
            nResolvedLocationOfAddress += m_pcIdentifierList[nQueryIdentifier].m_nBinaryDestinationStart -
                                          m_pcIdentifierList[nQueryIdentifier].m_nBinarySourceStart;

            BOOL bFoundLabel = FALSE;
            for (int nCount3 = 0; bFoundLabel == FALSE && nCount3 < m_nSymbolLabelList; ++nCount3)
            {
                if (m_pSymbolQueryList[nCount].m_sSymbolName == m_pSymbolLabelList[nCount3].m_sSymbolName)
                {

                    int nIdentifier = nQueryIdentifier;
                    if (m_pSymbolQueryList[nCount].m_sSymbolName.Left(2) == "FE" ||
                        m_pSymbolQueryList[nCount].m_sSymbolName.Left(2) == "FX")
                    {
                        sFunctionName = m_pSymbolQueryList[nCount].m_sSymbolName.Right(m_pSymbolQueryList[nCount].m_sSymbolName.GetLength() - 3);
                        nIdentifier = GetIdentifierByName(sFunctionName);
                        if (nIdentifier < 0)
                        {
                            return OutputIdentifierError(sFunctionName, nIdentifier);
                        }
                    }

                    int nResolvedAddress = m_pSymbolLabelList[nCount3].m_nLocationPointer +
                                           m_pcIdentifierList[nIdentifier].m_nBinaryDestinationStart -
                                           m_pcIdentifierList[nIdentifier].m_nBinarySourceStart;

                    int nJmpLength = nResolvedAddress - (nResolvedLocationOfAddress - 2);

                    m_pchOutputCode[m_pSymbolQueryList[nCount].m_nLocationPointer] = (char) ((nJmpLength >> 24) & 0xff);
                    m_pchOutputCode[m_pSymbolQueryList[nCount].m_nLocationPointer + 1] = (char) ((nJmpLength >> 16) & 0xff);
                    m_pchOutputCode[m_pSymbolQueryList[nCount].m_nLocationPointer + 2] = (char) ((nJmpLength >> 8) & 0xff);
                    m_pchOutputCode[m_pSymbolQueryList[nCount].m_nLocationPointer + 3] = (char) (nJmpLength & 0xff);

                    bFoundLabel = TRUE;
                }
            }

            if (bFoundLabel == FALSE)
            {
                sFunctionName = m_pSymbolQueryList[nCount].m_sSymbolName.Right(m_pSymbolQueryList[nCount].m_sSymbolName.GetLength() - 3);
                return OutputIdentifierError(sFunctionName, STRREF_CSCRIPTCOMPILER_ERROR_UNDEFINED_IDENTIFIER);
            }
        }
    }

    return 0;
}

int CScriptCompilerInternal::GetStructureField(const CExoString& sStructureName, const CExoString& sFieldName)
{
    for (int nCount = 0; nCount < m_nMaxStructures; ++nCount)
    {
        if (sStructureName == m_pcStructList[nCount].m_psName)
        {
            for (int nCount2 = m_pcStructList[nCount].m_nFieldStart; nCount2 <= m_pcStructList[nCount].m_nFieldEnd; ++nCount2)
            {
                if (sFieldName == m_pcStructFieldList[nCount2].m_psVarName)
                {
                    return nCount2;
                }
            }
            return STRREF_CSCRIPTCOMPILER_ERROR_UNDEFINED_FIELD_IN_STRUCTURE;
        }
    }

    return STRREF_CSCRIPTCOMPILER_ERROR_UNDEFINED_STRUCTURE;
}

int CScriptCompilerInternal::FoundReturnStatementOnAllBranches(CScriptParseTreeNode* pNode)
{
    if (pNode == NULL)
    {
        return FALSE;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_RETURN)
    {
        return TRUE;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_IF_CHOICE)
    {
        if (FoundReturnStatementOnAllBranches(pNode->pLeft) == TRUE &&
            FoundReturnStatementOnAllBranches(pNode->pRight) == TRUE)
        {
            return TRUE;
        }
        return FALSE;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_COMPOUND_STATEMENT ||
        pNode->nOperation == CSCRIPTCOMPILER_OPERATION_STATEMENT ||
        pNode->nOperation == CSCRIPTCOMPILER_OPERATION_STATEMENT_LIST ||
        pNode->nOperation == CSCRIPTCOMPILER_OPERATION_IF_BLOCK)
    {
        if (FoundReturnStatementOnAllBranches(pNode->pLeft) == TRUE)
        {
            return TRUE;
        }
        if (FoundReturnStatementOnAllBranches(pNode->pRight) == TRUE)
        {
            return TRUE;
        }
    }

    return FALSE;
}

void CScriptCompilerInternal::ClearAllSymbolLists()
{
    if (m_pSymbolQueryList != NULL)
    {
        delete[] m_pSymbolQueryList;
        m_pSymbolQueryList = NULL;
    }
    m_nSymbolQueryListSize = 0;
    m_nSymbolQueryList = 0;

    if (m_pSymbolLabelList != NULL)
    {
        delete[] m_pSymbolLabelList;
        m_pSymbolLabelList = NULL;
    }
    m_nSymbolLabelListSize = 0;
    m_nSymbolLabelList = 0;
}

int CScriptCompilerInternal::CleanUpAfterCompile(int nReturnValue, CScriptParseTreeNode* pReturnTree)
{
    DeleteParseTree(FALSE, pReturnTree);

    if (nReturnValue < 0)
    {
        if (m_pchOutputCode != NULL)
        {
            delete[] m_pchOutputCode;
            m_pchOutputCode = NULL;
        }
        m_nOutputCodeLength = 0;
    }

    DeleteParseTree(FALSE, m_pGlobalVariableParseTree);
    m_pGlobalVariableParseTree = NULL;

    ClearUserDefinedIdentifiers();
    ClearAllSymbolLists();

    return nReturnValue;
}

int CScriptCompilerInternal::AddSymbolToLabelList(const CExoString& sSymbolName, int nLocationPointer)
{
    if (m_nSymbolLabelListSize == m_nSymbolLabelList)
    {
        m_nSymbolLabelListSize += CSCRIPTCOMPILER_SYMBOL_LIST_GROW_SIZE;
        CScriptCompilerSymbolTableEntry* pNewSymbolLabelList = new CScriptCompilerSymbolTableEntry[m_nSymbolLabelListSize];

        if (m_pSymbolLabelList != NULL)
        {
            for (int nCount = 0; nCount < m_nSymbolLabelList; ++nCount)
            {
                pNewSymbolLabelList[nCount].m_sSymbolName = m_pSymbolLabelList[nCount].m_sSymbolName;
                pNewSymbolLabelList[nCount].m_nLocationPointer = m_pSymbolLabelList[nCount].m_nLocationPointer;
            }
            delete[] m_pSymbolLabelList;
        }
        m_pSymbolLabelList = pNewSymbolLabelList;
    }

    m_pSymbolLabelList[m_nSymbolLabelList].m_sSymbolName = sSymbolName;
    m_pSymbolLabelList[m_nSymbolLabelList].m_nLocationPointer = nLocationPointer;
    ++m_nSymbolLabelList;

    return 0;
}

int CScriptCompilerInternal::AddSymbolToQueryList(const CExoString& sSymbolName, int nLocationPointer)
{
    if (m_nSymbolQueryListSize == m_nSymbolQueryList)
    {
        m_nSymbolQueryListSize += CSCRIPTCOMPILER_SYMBOL_LIST_GROW_SIZE;
        CScriptCompilerSymbolTableEntry* pNewSymbolQueryList = new CScriptCompilerSymbolTableEntry[m_nSymbolQueryListSize];

        if (m_pSymbolQueryList != NULL)
        {
            for (int nCount = 0; nCount < m_nSymbolQueryList; ++nCount)
            {
                pNewSymbolQueryList[nCount].m_sSymbolName = m_pSymbolQueryList[nCount].m_sSymbolName;
                pNewSymbolQueryList[nCount].m_nLocationPointer = m_pSymbolQueryList[nCount].m_nLocationPointer;
            }
            delete[] m_pSymbolQueryList;
        }
        m_pSymbolQueryList = pNewSymbolQueryList;
    }

    m_pSymbolQueryList[m_nSymbolQueryList].m_sSymbolName = sSymbolName;
    m_pSymbolQueryList[m_nSymbolQueryList].m_nLocationPointer = nLocationPointer;
    ++m_nSymbolQueryList;

    return 0;
}

void CScriptCompilerInternal::PrintBinaryAddress()
{
    sprintf(m_pchOutputCode + m_nOutputCodeLength, "  %08x   ", m_nBinaryCodeLength);
}

int CScriptCompilerInternal::OutputWalkTreeError(int nError, CScriptParseTreeNode* pNode)
{
    STR_RES strRes;
    g_pTlkTable->Fetch(-nError, strRes, CTlkTable::GENDER_MALE);

    CExoString sFileName = m_pcIncludeFileStack[m_nCompileFileLevel].m_sFileName;

    int nLine = 0;
    if (pNode != NULL)
    {
        nLine = pNode->nLine;
    }

    return OutputError(nError, m_pcIncludeFileStack[m_nCompileFileLevel].m_sFileName, nLine, strRes.m_sString);
}

void CScriptCompilerInternal::InitializeSwitchLabelList()
{
    m_bSwitchLabelDefault = FALSE;
    m_nSwitchLabelNumber = 0;
    m_nSwitchLabelArraySize = CSCRIPTCOMPILER_SWITCH_LABEL_ARRAY_SIZE;
    m_pnSwitchLabelStatements = new int[m_nSwitchLabelArraySize];
}

int CScriptCompilerInternal::TraverseTreeForSwitchLabels(CScriptParseTreeNode* pNode)
{

    if (pNode == NULL || pNode->nOperation == CSCRIPTCOMPILER_OPERATION_SWITCH_BLOCK)
    {
        return 0;
    }

    int nReturnValue = TraverseTreeForSwitchLabels(pNode->pLeft);
    if (nReturnValue < 0)
    {
        return nReturnValue;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_DEFAULT)
    {
        if (m_bSwitchLabelDefault == TRUE)
        {
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_MULTIPLE_DEFAULT_STATEMENTS_WITHIN_SWITCH, pNode);
        }
        m_bSwitchLabelDefault = TRUE;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_CASE)
    {
        int nCaseValue;

        if (pNode->pLeft != NULL &&
            pNode->pLeft->nOperation == CSCRIPTCOMPILER_OPERATION_NEGATION &&
            pNode->pLeft->pLeft != NULL &&
            pNode->pLeft->pLeft->nOperation == CSCRIPTCOMPILER_OPERATION_CONSTANT_INTEGER)
        {
            nCaseValue = -pNode->pLeft->pLeft->nIntegerData;
        }
        else if (pNode->pLeft != NULL &&
                 pNode->pLeft->nOperation == CSCRIPTCOMPILER_OPERATION_CONSTANT_INTEGER)
        {
            nCaseValue = pNode->pLeft->nIntegerData;
        }
        else
        {
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_CASE_PARAMETER_NOT_A_CONSTANT_INTEGER, pNode);
        }

        int nCount;
        for (nCount = 0; nCount < m_nSwitchLabelNumber; ++nCount)
        {
            if (m_pnSwitchLabelStatements[nCount] == nCaseValue)
            {
                return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_MULTIPLE_CASE_CONSTANT_STATEMENTS_WITHIN_SWITCH, pNode);
            }
        }

        if (m_nSwitchLabelNumber >= m_nSwitchLabelArraySize)
        {
            int* pnNewSwitchLabelStatements = new int[m_nSwitchLabelArraySize * 2];
            for (nCount = 0; nCount < m_nSwitchLabelNumber; ++nCount)
            {
                pnNewSwitchLabelStatements[nCount] = m_pnSwitchLabelStatements[nCount];
            }
            m_nSwitchLabelArraySize *= 2;
            if (m_pnSwitchLabelStatements != NULL)
            {
                delete[] m_pnSwitchLabelStatements;
            }
            m_pnSwitchLabelStatements = pnNewSwitchLabelStatements;
        }

        m_pnSwitchLabelStatements[m_nSwitchLabelNumber] = nCaseValue;
        ++m_nSwitchLabelNumber;

        int nStackElementsDown = -4;
        int nSize = 4;
        if (m_nCompileSymbolicOutput == 0)
        {
            m_pchOutputCode[m_nOutputCodeLength] = VM_CPTOPSP;
            m_pchOutputCode[m_nOutputCodeLength + 1] = CSCRIPTCOMPILER_AUXCODE_TYPE_VOID;
            m_pchOutputCode[m_nOutputCodeLength + 2] = (char) ((nStackElementsDown >> 24) & 0xff);
            m_pchOutputCode[m_nOutputCodeLength + 3] = (char) ((nStackElementsDown >> 16) & 0xff);
            m_pchOutputCode[m_nOutputCodeLength + 4] = (char) ((nStackElementsDown >> 8) & 0xff);
            m_pchOutputCode[m_nOutputCodeLength + 5] = (char) (nStackElementsDown & 0xff);
            m_pchOutputCode[m_nOutputCodeLength + 6] = (char) ((nSize >> 8) & 0xff);
            m_pchOutputCode[m_nOutputCodeLength + 7] = (char) (nSize & 0xff);
            m_nOutputCodeLength += 8;
        }
        else
        {
            PrintBinaryAddress();
            sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                    "CPTOPSP %08x,%04x\n", nStackElementsDown, nSize);
            m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 22;
        }
        m_nBinaryCodeLength += 8;

        if (m_nCompileSymbolicOutput == 0)
        {
            m_pchOutputCode[m_nOutputCodeLength] = VM_CONST;
            m_pchOutputCode[m_nOutputCodeLength + 1] = VM_STACK_INTEGER;
            m_pchOutputCode[m_nOutputCodeLength + 2] = (char) ((nCaseValue >> 24) & 0xff);
            m_pchOutputCode[m_nOutputCodeLength + 3] = (char) ((nCaseValue >> 16) & 0xff);
            m_pchOutputCode[m_nOutputCodeLength + 4] = (char) ((nCaseValue >> 8) & 0xff);
            m_pchOutputCode[m_nOutputCodeLength + 5] = (char) (nCaseValue & 0xff);
            m_nOutputCodeLength += 6;
        }
        else
        {
            PrintBinaryAddress();
            sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                    "CONSTI %08x\n", nCaseValue);
            m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 16;
        }
        m_nBinaryCodeLength += 6;

        if (m_nCompileSymbolicOutput == 0)
        {
            m_pchOutputCode[m_nOutputCodeLength] = VM_EQUAL;
            m_pchOutputCode[m_nOutputCodeLength + 1] = VM_OPERAND_INTEGER_INTEGER;
            m_nOutputCodeLength += 2;
        }
        else
        {
            PrintBinaryAddress();
            sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH, "EQUALII\n");
            m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 8;
        }
        m_nBinaryCodeLength += 2;

        if (m_nCompileSymbolicOutput == 0)
        {
            m_pchOutputCode[m_nOutputCodeLength] = VM_JNZ;
            m_pchOutputCode[m_nOutputCodeLength + 1] = 0;

            CExoString sSymbolName;
            sSymbolName.Format("_SC_%08x_%08x", nCaseValue, m_nSwitchIdentifier);
            AddSymbolToQueryList(sSymbolName, m_nOutputCodeLength + 2);
            m_nOutputCodeLength += 6;
        }
        else
        {
            PrintBinaryAddress();
            sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                    "JNZ _SC_%08x_%08x\n", nCaseValue, m_nSwitchIdentifier);
            m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 26;
        }
        m_nBinaryCodeLength += 6;
    }

    nReturnValue = TraverseTreeForSwitchLabels(pNode->pRight);
    if (nReturnValue < 0)
    {
        return nReturnValue;
    }

    return 0;
}

void CScriptCompilerInternal::ClearSwitchLabelList()
{

    if (m_bSwitchLabelDefault == TRUE)
    {
        if (m_nCompileSymbolicOutput == 0)
        {
            m_pchOutputCode[m_nOutputCodeLength] = VM_JMP;
            m_pchOutputCode[m_nOutputCodeLength + 1] = 0;

            CExoString sSymbolName;
            sSymbolName.Format("_SC_DEFAULT_%08x", m_nSwitchIdentifier);
            AddSymbolToQueryList(sSymbolName, m_nOutputCodeLength + 2);
            m_nOutputCodeLength += 6;
        }
        else
        {
            PrintBinaryAddress();
            sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                    "JMP _SC_DEFAULT_%08x\n", m_nSwitchIdentifier);
            m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 25;
        }
    }
    else
    {
        if (m_nCompileSymbolicOutput == 0)
        {
            m_pchOutputCode[m_nOutputCodeLength] = VM_JMP;
            m_pchOutputCode[m_nOutputCodeLength + 1] = 0;

            CExoString sSymbolName;
            sSymbolName.Format("_BR_%08x", m_nSwitchIdentifier);
            AddSymbolToQueryList(sSymbolName, m_nOutputCodeLength + 2);
            m_nOutputCodeLength += 6;
        }
        else
        {
            PrintBinaryAddress();
            sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                    "JMP _BR_%08x\n", m_nSwitchIdentifier);
            m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 17;
        }
    }
    m_nBinaryCodeLength += 6;

    m_bSwitchLabelDefault = FALSE;
    m_nSwitchLabelNumber = 0;
    m_nSwitchLabelArraySize = CSCRIPTCOMPILER_SWITCH_LABEL_ARRAY_SIZE;
    if (m_pnSwitchLabelStatements != NULL)
    {
        delete[] m_pnSwitchLabelStatements;
    }
    m_pnSwitchLabelStatements = NULL;
}

int CScriptCompilerInternal::GenerateCodeForSwitchLabels(CScriptParseTreeNode* pNode)
{
    int nReturnValue = 0;

    InitializeSwitchLabelList();
    if (pNode != NULL)
    {
        nReturnValue = TraverseTreeForSwitchLabels(pNode->pRight);
    }
    if (nReturnValue < 0)
    {
        return nReturnValue;
    }
    ClearSwitchLabelList();

    return 0;
}

void CScriptCompilerInternal::AddStructureToStack(const CExoString& sStructureName, BOOL bGenerateCode)
{
    for (int nCount = 0; nCount < m_nMaxStructures; ++nCount)
    {
        if (sStructureName == m_pcStructList[nCount].m_psName)
        {
            for (int nCount2 = m_pcStructList[nCount].m_nFieldStart; nCount2 <= m_pcStructList[nCount].m_nFieldEnd; ++nCount2)
            {
                char chType = m_pcStructFieldList[nCount2].m_pchType;

                if (chType == CSCRIPTCOMPILER_TOKEN_KEYWORD_INT ||
                    chType == CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT ||
                    chType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRING ||
                    chType == CSCRIPTCOMPILER_TOKEN_KEYWORD_OBJECT ||
                    (chType >= CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE0 &&
                     chType <= CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE9))
                {
                    int nStackType;
                    if (chType == CSCRIPTCOMPILER_TOKEN_KEYWORD_INT)
                    {
                        nStackType = VM_STACK_INTEGER;
                    }
                    else if (chType == CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT)
                    {
                        nStackType = VM_STACK_FLOAT;
                    }
                    else if (chType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRING)
                    {
                        nStackType = VM_STACK_STRING;
                    }
                    else if (chType == CSCRIPTCOMPILER_TOKEN_KEYWORD_OBJECT)
                    {
                        nStackType = VM_STACK_OBJECT;
                    }
                    else if (chType >= CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE0 &&
                             chType <= CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE9)
                    {
                        nStackType = VM_STACK_ENGINE_STRUCTURE_FIRST + (chType - CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE0);
                    }

                    m_pchStackTypes[m_nStackCurrentDepth] = (char) nStackType;
                    ++m_nStackCurrentDepth;

                    if (bGenerateCode == TRUE)
                    {
                        if (m_nCompileSymbolicOutput == 0)
                        {
                            m_pchOutputCode[m_nOutputCodeLength] = VM_RSADD;
                            m_pchOutputCode[m_nOutputCodeLength + 1] = (char) nStackType;
                            m_nOutputCodeLength += 2;
                        }
                        else
                        {
                            PrintBinaryAddress();
                            if (chType == CSCRIPTCOMPILER_TOKEN_KEYWORD_INT)
                            {
                                sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH, "RSADDI\n");
                                m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 7;
                            }
                            else if (chType == CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT)
                            {
                                sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH, "RSADDF\n");
                                m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 7;
                            }
                            else if (chType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRING)
                            {
                                sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH, "RSADDS\n");
                                m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 7;
                            }
                            else if (chType == CSCRIPTCOMPILER_TOKEN_KEYWORD_OBJECT)
                            {
                                sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH, "RSADDO\n");
                                m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 7;
                            }
                            else if (chType >= CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE0 &&
                                     chType <= CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE9)
                            {
                                sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                                        "RSADDP%1d\n", chType - CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE0);
                                m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 8;
                            }
                        }
                        m_nBinaryCodeLength += 2;
                    }
                }
                else if (chType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT)
                {
                    AddStructureToStack(m_pcStructFieldList[nCount2].m_psStructureName, bGenerateCode);
                }
            }
        }
    }
}

void CScriptCompilerInternal::AddVariableToStack(int nVariableType, const CExoString& sVariableTypeName, BOOL bGenerateCode)
{
    if (nVariableType == CSCRIPTCOMPILER_TOKEN_KEYWORD_INT ||
        nVariableType == CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT ||
        nVariableType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRING ||
        nVariableType == CSCRIPTCOMPILER_TOKEN_KEYWORD_OBJECT ||
        (nVariableType >= CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE0 &&
         nVariableType <= CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE9))
    {
        int nStackType;
        if (nVariableType == CSCRIPTCOMPILER_TOKEN_KEYWORD_INT)
        {
            nStackType = VM_STACK_INTEGER;
        }
        else if (nVariableType == CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT)
        {
            nStackType = VM_STACK_FLOAT;
        }
        else if (nVariableType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRING)
        {
            nStackType = VM_STACK_STRING;
        }
        else if (nVariableType == CSCRIPTCOMPILER_TOKEN_KEYWORD_OBJECT)
        {
            nStackType = VM_STACK_OBJECT;
        }
        else if (nVariableType >= CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE0 &&
                 nVariableType <= CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE9)
        {
            nStackType = VM_STACK_ENGINE_STRUCTURE_FIRST + (nVariableType - CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE0);
        }

        m_pchStackTypes[m_nStackCurrentDepth] = (char) nStackType;
        ++m_nStackCurrentDepth;

        if (bGenerateCode == TRUE)
        {
            if (m_nCompileSymbolicOutput == 0)
            {
                m_pchOutputCode[m_nOutputCodeLength] = VM_RSADD;
                m_pchOutputCode[m_nOutputCodeLength + 1] = (char) nStackType;
                m_nOutputCodeLength += 2;
            }
            else
            {
                PrintBinaryAddress();
                if (nVariableType == CSCRIPTCOMPILER_TOKEN_KEYWORD_INT)
                {
                    sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH, "RSADDI\n");
                    m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 7;
                }
                else if (nVariableType == CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT)
                {
                    sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH, "RSADDF\n");
                    m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 7;
                }
                else if (nVariableType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRING)
                {
                    sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH, "RSADDS\n");
                    m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 7;
                }
                else if (nVariableType == CSCRIPTCOMPILER_TOKEN_KEYWORD_OBJECT)
                {
                    sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH, "RSADDO\n");
                    m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 7;
                }
                else if (nVariableType >= CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE0 &&
                         nVariableType <= CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE9)
                {
                    sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                            "RSADDP%1d\n", nVariableType - CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE0);
                    m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 8;
                }
            }
            m_nBinaryCodeLength += 2;
        }
    }
    else if (nVariableType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT)
    {
        AddStructureToStack(sVariableTypeName, bGenerateCode);
    }
}

int CScriptCompilerInternal::GetStructureSize(const CExoString& sStructureName)
{
    for (int nCount = 0; nCount < m_nMaxStructures; ++nCount)
    {
        if (sStructureName == m_pcStructList[nCount].m_psName)
        {
            return m_pcStructList[nCount].m_nByteSize;
        }
    }

    return 0;
}

int CScriptCompilerInternal::PostVisitGenerateCode(CScriptParseTreeNode* pNode)
{
    int nCount;

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_ASSIGNMENT)
    {
        if (pNode->pRight != NULL && pNode->pLeft != NULL &&
            pNode->pRight->nType == pNode->pLeft->nType &&
            (pNode->pRight->nType != CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT ||
             pNode->pRight->m_psTypeName == pNode->pLeft->m_psTypeName))
        {
            pNode->nType = pNode->pRight->nType;
            if (pNode->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT)
            {
                pNode->m_psTypeName = pNode->pLeft->m_psTypeName;
            }

            int nStackElementsDown = pNode->pRight->nIntegerData - (m_nStackCurrentDepth * 4);
            int nSize = 4;
            if (pNode->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT)
            {
                nSize = GetStructureSize(pNode->m_psTypeName);
            }

            BOOL bOperateOnStackPointer = TRUE;
            if (pNode->pRight->nIntegerData < m_nGlobalVariableSize && m_bGlobalVariableDefinition == FALSE)
            {
                bOperateOnStackPointer = FALSE;
                nStackElementsDown = pNode->pRight->nIntegerData - m_nGlobalVariableSize;
            }

            if (m_nCompileSymbolicOutput == 0)
            {
                if (bOperateOnStackPointer == TRUE)
                {
                    m_pchOutputCode[m_nOutputCodeLength] = VM_CPDOWNSP;
                }
                else
                {
                    m_pchOutputCode[m_nOutputCodeLength] = VM_CPDOWNBP;
                }
                m_pchOutputCode[m_nOutputCodeLength + 1] = CSCRIPTCOMPILER_AUXCODE_TYPE_VOID;
                m_pchOutputCode[m_nOutputCodeLength + 2] = (char) ((nStackElementsDown >> 24) & 0xff);
                m_pchOutputCode[m_nOutputCodeLength + 3] = (char) ((nStackElementsDown >> 16) & 0xff);
                m_pchOutputCode[m_nOutputCodeLength + 4] = (char) ((nStackElementsDown >> 8) & 0xff);
                m_pchOutputCode[m_nOutputCodeLength + 5] = (char) (nStackElementsDown & 0xff);
                m_pchOutputCode[m_nOutputCodeLength + 6] = (char) ((nSize >> 8) & 0xff);
                m_pchOutputCode[m_nOutputCodeLength + 7] = (char) (nSize & 0xff);
                m_nOutputCodeLength += 8;
            }
            else
            {
                PrintBinaryAddress();
                if (bOperateOnStackPointer == TRUE)
                {
                    sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                            "CPDOWNSP %08x,%04x\n", nStackElementsDown, nSize);
                }
                else
                {
                    sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                            "CPDOWNBP %08x,%04x\n", nStackElementsDown, nSize);
                }
                m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 23;
            }
            m_nBinaryCodeLength += 8;

            m_bAssignmentToVariable = FALSE;
            return 0;
        }

        return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_MISMATCHED_TYPES, pNode);
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_KEYWORD_VOID)
    {
        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_KEYWORD_INT)
    {
        m_nVarStackVariableType = CSCRIPTCOMPILER_TOKEN_KEYWORD_INT;
        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_KEYWORD_FLOAT)
    {
        m_nVarStackVariableType = CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT;
        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_KEYWORD_STRING)
    {
        m_nVarStackVariableType = CSCRIPTCOMPILER_TOKEN_KEYWORD_STRING;
        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_KEYWORD_OBJECT)
    {
        m_nVarStackVariableType = CSCRIPTCOMPILER_TOKEN_KEYWORD_OBJECT;
        return 0;
    }

    if (pNode->nOperation >= CSCRIPTCOMPILER_OPERATION_KEYWORD_ENGINE_STRUCTURE0 &&
        pNode->nOperation <= CSCRIPTCOMPILER_OPERATION_KEYWORD_ENGINE_STRUCTURE9)
    {
        m_nVarStackVariableType = CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE0 +
                                  (pNode->nOperation - CSCRIPTCOMPILER_OPERATION_KEYWORD_ENGINE_STRUCTURE0);
        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_KEYWORD_STRUCT)
    {

        if (m_nStructureDefinition != 1)
        {
            m_nVarStackVariableType = CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT;
            m_sVarStackVariableTypeName = pNode->m_psStringData;
        }
        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_VARIABLE)
    {

        if (m_nVarStackVariableType != CSCRIPTCOMPILER_OPERATION_KEYWORD_DECLARATION)
        {
            if (m_nStructureDefinition == 0)
            {
                for (nCount = m_nOccupiedVariables; nCount >= 0; --nCount)
                {
                    if (m_pcVarStackList[nCount].m_psVarName == pNode->m_psStringData &&
                        m_pcVarStackList[nCount].m_nVarLevel == m_nVarStackRecursionLevel)
                    {
                        return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_VARIABLE_ALREADY_USED_WITHIN_SCOPE, pNode);
                    }
                }

                ++m_nOccupiedVariables;
                if (m_bGlobalVariableDefinition != FALSE)
                {
                    ++m_nGlobalVariables;
                    if (m_nVarStackVariableType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT)
                    {
                        m_nGlobalVariableSize += GetStructureSize(m_sVarStackVariableTypeName);
                    }
                    else
                    {
                        m_nGlobalVariableSize += 4;
                    }
                }

                m_pcVarStackList[m_nOccupiedVariables].m_psVarName = pNode->m_psStringData;
                m_pcVarStackList[m_nOccupiedVariables].m_nVarType = m_nVarStackVariableType;
                if (m_nVarStackVariableType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT)
                {
                    m_pcVarStackList[m_nOccupiedVariables].m_sVarStructureName = m_sVarStackVariableTypeName;

                    BOOL bFoundStructure = FALSE;
                    for (int nCount2 = 0; nCount2 < m_nMaxStructures; ++nCount2)
                    {
                        if (m_sVarStackVariableTypeName == m_pcStructList[nCount2].m_psName)
                        {
                            bFoundStructure = TRUE;
                        }
                    }
                    if (bFoundStructure == FALSE)
                    {
                        return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_UNDEFINED_STRUCTURE, pNode);
                    }
                }
                else
                {
                    m_sVarStackVariableTypeName = "";
                }

                m_pcVarStackList[m_nOccupiedVariables].m_nVarLevel = m_nVarStackRecursionLevel;
                m_pcVarStackList[m_nOccupiedVariables].m_nVarRunTimeLocation = m_nStackCurrentDepth * 4;

                AddVariableToStack(m_nVarStackVariableType, m_sVarStackVariableTypeName, TRUE);
            }
            else
            {

                for (nCount = m_nStructureDefinitionFieldStart; nCount < m_nMaxStructureFields; ++nCount)
                {
                    if (m_pcStructFieldList[nCount].m_psVarName == pNode->m_psStringData)
                    {
                        return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_VARIABLE_USED_TWICE_IN_SAME_STRUCTURE, pNode);
                    }
                }

                m_pcStructFieldList[m_nMaxStructureFields].m_pchType = (char) m_nVarStackVariableType;
                if (m_nVarStackVariableType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT)
                {
                    m_pcStructFieldList[m_nMaxStructureFields].m_psStructureName = m_sVarStackVariableTypeName;
                }
                m_pcStructFieldList[m_nMaxStructureFields].m_psVarName = pNode->m_psStringData;
                m_pcStructFieldList[m_nMaxStructureFields].m_nLocation = 0;
                ++m_nMaxStructureFields;
            }
        }
        else
        {

            if (m_bInStructurePart == TRUE)
            {
                return 0;
            }

            for (nCount = m_nOccupiedVariables; nCount >= 0; --nCount)
            {
                if (m_pcVarStackList[nCount].m_psVarName == pNode->m_psStringData)
                {
                    pNode->m_psStringData = "";

                    pNode->nType = m_pcVarStackList[nCount].m_nVarType;
                    if (pNode->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT)
                    {
                        pNode->m_psTypeName = m_pcVarStackList[nCount].m_sVarStructureName;
                    }
                    else
                    {
                        pNode->m_psTypeName = "";
                    }

                    pNode->nIntegerData = m_pcVarStackList[nCount].m_nVarRunTimeLocation;

                    if (m_bAssignmentToVariable == FALSE)
                    {
                        int nStackElementsDown = pNode->nIntegerData - (m_nStackCurrentDepth * 4);
                        int nSize = 4;
                        if (pNode->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT)
                        {
                            nSize = GetStructureSize(pNode->m_psTypeName);
                        }

                        BOOL bOperateOnStackPointer = TRUE;
                        if (pNode->nIntegerData < m_nGlobalVariableSize && m_bGlobalVariableDefinition == FALSE)
                        {
                            bOperateOnStackPointer = FALSE;
                            nStackElementsDown = pNode->nIntegerData - m_nGlobalVariableSize;
                        }

                        if (m_nCompileSymbolicOutput == 0)
                        {
                            if (bOperateOnStackPointer == TRUE)
                            {
                                m_pchOutputCode[m_nOutputCodeLength] = VM_CPTOPSP;
                            }
                            else
                            {
                                m_pchOutputCode[m_nOutputCodeLength] = VM_CPTOPBP;
                            }
                            m_pchOutputCode[m_nOutputCodeLength + 1] = CSCRIPTCOMPILER_AUXCODE_TYPE_VOID;
                            m_pchOutputCode[m_nOutputCodeLength + 2] = (char) ((nStackElementsDown >> 24) & 0xff);
                            m_pchOutputCode[m_nOutputCodeLength + 3] = (char) ((nStackElementsDown >> 16) & 0xff);
                            m_pchOutputCode[m_nOutputCodeLength + 4] = (char) ((nStackElementsDown >> 8) & 0xff);
                            m_pchOutputCode[m_nOutputCodeLength + 5] = (char) (nStackElementsDown & 0xff);
                            m_pchOutputCode[m_nOutputCodeLength + 6] = (char) ((nSize >> 8) & 0xff);
                            m_pchOutputCode[m_nOutputCodeLength + 7] = (char) (nSize & 0xff);
                            m_nOutputCodeLength += 8;
                        }
                        else
                        {
                            PrintBinaryAddress();
                            if (bOperateOnStackPointer == TRUE)
                            {
                                sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                                        "CPTOPSP %08x,%04x\n", nStackElementsDown, nSize);
                            }
                            else
                            {
                                sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                                        "CPTOPBP %08x,%04x\n", nStackElementsDown, nSize);
                            }
                            m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 22;
                        }
                        m_nBinaryCodeLength += 8;

                        AddVariableToStack(pNode->nType, pNode->m_psTypeName, FALSE);
                    }

                    return 0;
                }
            }

            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_VARIABLE_DEFINED_WITHOUT_TYPE, pNode);
        }

        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_CONSTANT_INTEGER)
    {
        if (m_nCompileSymbolicOutput == 0)
        {
            m_pchOutputCode[m_nOutputCodeLength] = VM_CONST;
            m_pchOutputCode[m_nOutputCodeLength + 1] = VM_STACK_INTEGER;
            m_pchOutputCode[m_nOutputCodeLength + 2] = (char) ((pNode->nIntegerData >> 24) & 0xff);
            m_pchOutputCode[m_nOutputCodeLength + 3] = (char) ((pNode->nIntegerData >> 16) & 0xff);
            m_pchOutputCode[m_nOutputCodeLength + 4] = (char) ((pNode->nIntegerData >> 8) & 0xff);
            m_pchOutputCode[m_nOutputCodeLength + 5] = (char) (pNode->nIntegerData & 0xff);
            m_nOutputCodeLength += 6;
        }
        else
        {
            PrintBinaryAddress();
            sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                    "CONSTI %08x\n", pNode->nIntegerData);
            m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 16;
        }
        m_nBinaryCodeLength += 6;

        pNode->nType = CSCRIPTCOMPILER_TOKEN_KEYWORD_INT;
        m_pchStackTypes[m_nStackCurrentDepth] = VM_STACK_INTEGER;
        ++m_nStackCurrentDepth;
        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_CONSTANT_OBJECT)
    {
        if (m_nCompileSymbolicOutput == 0)
        {
            m_pchOutputCode[m_nOutputCodeLength] = VM_CONST;
            m_pchOutputCode[m_nOutputCodeLength + 1] = VM_STACK_OBJECT;
            m_pchOutputCode[m_nOutputCodeLength + 2] = (char) ((pNode->nIntegerData >> 24) & 0xff);
            m_pchOutputCode[m_nOutputCodeLength + 3] = (char) ((pNode->nIntegerData >> 16) & 0xff);
            m_pchOutputCode[m_nOutputCodeLength + 4] = (char) ((pNode->nIntegerData >> 8) & 0xff);
            m_pchOutputCode[m_nOutputCodeLength + 5] = (char) (pNode->nIntegerData & 0xff);
            m_nOutputCodeLength += 6;
        }
        else
        {
            PrintBinaryAddress();
            sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                    "CONSTO %08x\n", pNode->nIntegerData);
            m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 16;
        }
        m_nBinaryCodeLength += 6;

        pNode->nType = CSCRIPTCOMPILER_TOKEN_KEYWORD_OBJECT;
        m_pchStackTypes[m_nStackCurrentDepth] = VM_STACK_OBJECT;
        ++m_nStackCurrentDepth;
        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_CONSTANT_FLOAT)
    {
        if (m_nCompileSymbolicOutput == 0)
        {
            m_pchOutputCode[m_nOutputCodeLength] = VM_CONST;
            m_pchOutputCode[m_nOutputCodeLength + 1] = VM_STACK_FLOAT;
            m_pchOutputCode[m_nOutputCodeLength + 2] = (char) ((*((int*) &pNode->fFloatData) >> 24) & 0xff);
            m_pchOutputCode[m_nOutputCodeLength + 3] = (char) ((*((int*) &pNode->fFloatData) >> 16) & 0xff);
            m_pchOutputCode[m_nOutputCodeLength + 4] = (char) ((*((int*) &pNode->fFloatData) >> 8) & 0xff);
            m_pchOutputCode[m_nOutputCodeLength + 5] = (char) (*((int*) &pNode->fFloatData) & 0xff);
            m_nOutputCodeLength += 6;
        }
        else
        {
            PrintBinaryAddress();
            sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                    "CONSTF %19.9f\n", pNode->fFloatData);
            m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 27;
        }
        m_nBinaryCodeLength += 6;

        pNode->nType = CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT;
        m_pchStackTypes[m_nStackCurrentDepth] = VM_STACK_FLOAT;
        ++m_nStackCurrentDepth;
        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_CONSTANT_VECTOR)
    {

        for (nCount = 0; nCount < 3; ++nCount)
        {
            if (m_nCompileSymbolicOutput == 0)
            {
                m_pchOutputCode[m_nOutputCodeLength] = VM_CONST;
                m_pchOutputCode[m_nOutputCodeLength + 1] = VM_STACK_FLOAT;
                m_pchOutputCode[m_nOutputCodeLength + 2] = (char) ((*((int*) &pNode->fVectorData[nCount]) >> 24) & 0xff);
                m_pchOutputCode[m_nOutputCodeLength + 3] = (char) ((*((int*) &pNode->fVectorData[nCount]) >> 16) & 0xff);
                m_pchOutputCode[m_nOutputCodeLength + 4] = (char) ((*((int*) &pNode->fVectorData[nCount]) >> 8) & 0xff);
                m_pchOutputCode[m_nOutputCodeLength + 5] = (char) (*((int*) &pNode->fVectorData[nCount]) & 0xff);
                m_nOutputCodeLength += 6;
            }
            else
            {
                PrintBinaryAddress();
                sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                        "CONSTF %19.9f\n", pNode->fFloatData);
                m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 27;
            }
            m_nBinaryCodeLength += 6;

            m_pchStackTypes[m_nStackCurrentDepth] = VM_STACK_FLOAT;
            ++m_nStackCurrentDepth;
        }

        pNode->nType = CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT;
        pNode->m_psTypeName = "vector";
        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_CONSTANT_STRING)
    {
        int nLength = pNode->m_psStringData.GetLength();

        if (m_nCompileSymbolicOutput == 0)
        {
            m_pchOutputCode[m_nOutputCodeLength] = VM_CONST;
            m_pchOutputCode[m_nOutputCodeLength + 1] = VM_STACK_STRING;
            m_pchOutputCode[m_nOutputCodeLength + 2] = (char) ((nLength >> 8) & 0xff);
            m_pchOutputCode[m_nOutputCodeLength + 3] = (char) (nLength & 0xff);
            for (nCount = 0; nCount < nLength; ++nCount)
            {
                m_pchOutputCode[m_nOutputCodeLength + 4 + nCount] = pNode->m_psStringData.CStr()[nCount];
            }
            m_nOutputCodeLength += nLength + 4;
        }
        else
        {
            PrintBinaryAddress();
            sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                    "CONSTS %04x %s\n", nLength, pNode->m_psStringData.CStr());
            m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + nLength + 13;
        }
        m_nBinaryCodeLength += nLength + 4;

        pNode->nType = CSCRIPTCOMPILER_TOKEN_KEYWORD_STRING;
        m_pchStackTypes[m_nStackCurrentDepth] = VM_STACK_STRING;
        ++m_nStackCurrentDepth;
        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_KEYWORD_DECLARATION)
    {
        m_nVarStackVariableType = CSCRIPTCOMPILER_OPERATION_KEYWORD_DECLARATION;
        pNode->nType = 0;
        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_VARIABLE_LIST ||
        pNode->nOperation == CSCRIPTCOMPILER_OPERATION_STATEMENT_LIST)
    {
        pNode->nType = 0;
        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_ACTION_ARG_LIST)
    {

        if (pNode->pRight != NULL)
        {
            pNode->nType = pNode->pRight->nType;
            if (pNode->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT)
            {
                pNode->m_psTypeName = pNode->pRight->m_psTypeName;
            }
        }
        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_ACTION_PARAMETER)
    {

        if (m_nCompileSymbolicOutput == 0)
        {
            m_pchOutputCode[m_nOutputCodeLength] = VM_RETN;
            m_pchOutputCode[m_nOutputCodeLength + 1] = 0;
            m_nOutputCodeLength += 2;

            int nJmpLength = m_nOutputCodeLength - pNode->nIntegerData;
            m_pchOutputCode[pNode->nIntegerData + 2] = (char) ((nJmpLength >> 24) & 0xff);
            m_pchOutputCode[pNode->nIntegerData + 3] = (char) ((nJmpLength >> 16) & 0xff);
            m_pchOutputCode[pNode->nIntegerData + 4] = (char) ((nJmpLength >> 8) & 0xff);
            m_pchOutputCode[pNode->nIntegerData + 5] = (char) (nJmpLength & 0xff);
        }
        else
        {
            PrintBinaryAddress();
            sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH, "RETN\n");
            m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 5;
            sprintf(m_pchOutputCode + m_nOutputCodeLength, "_X%08x:\n", pNode->nIntegerData);
            m_nOutputCodeLength += 12;
        }
        m_nBinaryCodeLength += 2;

        if (pNode->pLeft != NULL)
        {
            pNode->nType = CSCRIPTCOMPILER_TOKEN_KEYWORD_ACTION;
        }
        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_ACTION_ID)
    {
        pNode->nIntegerData = GetIdentifierByName(pNode->m_psStringData);
        if (pNode->nIntegerData < 0)
        {
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_UNKNOWN_STATE_IN_COMPILER, pNode);
        }
        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_ACTION)
    {
        if (pNode->pRight == NULL)
        {
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_UNKNOWN_STATE_IN_COMPILER, pNode);
        }

        CScriptParseTreeNode* pTracePtr = pNode->pLeft;
        int nParameters = 0;

        while (pTracePtr != NULL)
        {
            if (pTracePtr->nType != CSCRIPTCOMPILER_TOKEN_KEYWORD_INT &&
                pTracePtr->nType != CSCRIPTCOMPILER_TOKEN_KEYWORD_ACTION &&
                pTracePtr->nType != CSCRIPTCOMPILER_TOKEN_KEYWORD_OBJECT &&
                pTracePtr->nType != CSCRIPTCOMPILER_TOKEN_KEYWORD_STRING &&
                pTracePtr->nType != CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT &&
                pTracePtr->nType != CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT &&
                pTracePtr->nType != CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE0 &&
                pTracePtr->nType != CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE1 &&
                pTracePtr->nType != CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE2 &&
                pTracePtr->nType != CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE3 &&
                pTracePtr->nType != CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE4 &&
                pTracePtr->nType != CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE5 &&
                pTracePtr->nType != CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE6 &&
                pTracePtr->nType != CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE7 &&
                pTracePtr->nType != CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE8 &&
                pTracePtr->nType != CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE9)
            {
                return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_UNKNOWN_STATE_IN_COMPILER, pNode);
            }

            m_pchActionParameters[nParameters] = (char) pTracePtr->nType;
            if (pTracePtr->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT)
            {
                m_pchActionParameterStructureNames[nParameters] = pTracePtr->m_psTypeName;
                m_nStackCurrentDepth -= GetStructureSize(pTracePtr->m_psTypeName) >> 2;
            }
            else if (pTracePtr->nType != CSCRIPTCOMPILER_TOKEN_KEYWORD_ACTION)
            {
                --m_nStackCurrentDepth;
            }

            ++nParameters;
            pTracePtr = pTracePtr->pLeft;
        }

        int nIdentifier = pNode->pRight->nIntegerData;
        BOOL bMatch = TRUE;

        if (nParameters > m_pcIdentifierList[nIdentifier].m_nParameters)
        {
            bMatch = FALSE;
        }
        if (nParameters < m_pcIdentifierList[nIdentifier].m_nNonOptionalParameters)
        {
            bMatch = FALSE;
        }

        for (nCount = 0; nCount < nParameters; ++nCount)
        {
            if (m_pchActionParameters[nCount] != m_pcIdentifierList[nIdentifier].m_pchParameters[nCount] ||
                (m_pchActionParameters[nCount] == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT &&
                 m_pchActionParameterStructureNames[nCount] != m_pcIdentifierList[nIdentifier].m_psStructureParameterNames[nCount]))
            {
                bMatch = FALSE;
            }
        }

        if (bMatch != TRUE)
        {
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_DECLARATION_DOES_NOT_MATCH_PARAMETERS, pNode);
        }

        for (nCount = nParameters; nCount < m_pcIdentifierList[nIdentifier].m_nParameters; ++nCount)
        {
            if (m_pcIdentifierList[nIdentifier].m_pchParameters[nCount] == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT)
            {
                m_pchActionParameterStructureNames[nParameters] = m_pcIdentifierList[nIdentifier].m_psStructureParameterNames[nCount];
                m_nStackCurrentDepth -= GetStructureSize(m_pcIdentifierList[nIdentifier].m_psStructureParameterNames[nCount]) >> 2;
            }
            else if (m_pcIdentifierList[nIdentifier].m_pchParameters[nCount] != CSCRIPTCOMPILER_TOKEN_KEYWORD_ACTION)
            {
                --m_nStackCurrentDepth;
            }
        }

        if (m_pcIdentifierList[nIdentifier].m_nReturnType != CSCRIPTCOMPILER_TOKEN_VOID_IDENTIFIER)
        {
            pNode->m_psTypeName = "";

            if (m_pcIdentifierList[nIdentifier].m_nReturnType == CSCRIPTCOMPILER_TOKEN_INTEGER_IDENTIFIER)
            {
                pNode->nType = CSCRIPTCOMPILER_TOKEN_KEYWORD_INT;
            }
            else if (m_pcIdentifierList[nIdentifier].m_nReturnType == CSCRIPTCOMPILER_TOKEN_FLOAT_IDENTIFIER)
            {
                pNode->nType = CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT;
            }
            else if (m_pcIdentifierList[nIdentifier].m_nReturnType == CSCRIPTCOMPILER_TOKEN_OBJECT_IDENTIFIER)
            {
                pNode->nType = CSCRIPTCOMPILER_TOKEN_KEYWORD_OBJECT;
            }
            else if (m_pcIdentifierList[nIdentifier].m_nReturnType == CSCRIPTCOMPILER_TOKEN_STRING_IDENTIFIER)
            {
                pNode->nType = CSCRIPTCOMPILER_TOKEN_KEYWORD_STRING;
            }
            else if (m_pcIdentifierList[nIdentifier].m_nReturnType >= CSCRIPTCOMPILER_TOKEN_ENGINE_STRUCTURE0_IDENTIFIER &&
                     m_pcIdentifierList[nIdentifier].m_nReturnType <= CSCRIPTCOMPILER_TOKEN_ENGINE_STRUCTURE9_IDENTIFIER)
            {
                pNode->nType = CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE0 +
                               (m_pcIdentifierList[nIdentifier].m_nReturnType - CSCRIPTCOMPILER_TOKEN_ENGINE_STRUCTURE0_IDENTIFIER);
            }
            else if (m_pcIdentifierList[nIdentifier].m_nReturnType == CSCRIPTCOMPILER_TOKEN_STRUCTURE_IDENTIFIER)
            {
                pNode->nType = CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT;
                pNode->m_psTypeName = m_pcIdentifierList[nIdentifier].m_psStructureReturnName;
            }

            if (pNode->nIntegerData == 0)
            {
                AddVariableToStack(pNode->nType, pNode->m_psTypeName, FALSE);
            }
        }

        if (pNode->nIntegerData == 0)
        {

            int nIdentifierOrder = m_pcIdentifierList[nIdentifier].m_nIdIdentifier;

            if (m_nCompileSymbolicOutput == 0)
            {
                m_pchOutputCode[m_nOutputCodeLength] = VM_ACTION;
                m_pchOutputCode[m_nOutputCodeLength + 1] = 0;
                m_pchOutputCode[m_nOutputCodeLength + 2] = (char) ((nIdentifierOrder >> 8) & 0xff);
                m_pchOutputCode[m_nOutputCodeLength + 3] = (char) (nIdentifierOrder & 0xff);
                m_pchOutputCode[m_nOutputCodeLength + 4] = (char) (m_pcIdentifierList[nIdentifier].m_nParameters & 0xff);
                m_nOutputCodeLength += 5;
            }
            else
            {
                PrintBinaryAddress();
                sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                        "ACTION %04x %02x\n", nIdentifierOrder & 0xffff,
                        m_pcIdentifierList[nIdentifier].m_nParameters & 0xffff);
                m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 15;
            }
            m_nBinaryCodeLength += 5;
        }
        else
        {

            if (m_nCompileSymbolicOutput == 0)
            {
                m_pchOutputCode[m_nOutputCodeLength] = VM_JSR;
                m_pchOutputCode[m_nOutputCodeLength + 1] = 0;

                CExoString sSymbolName;
                sSymbolName.Format("FE_%s", m_pcIdentifierList[nIdentifier].m_psIdentifier.CStr());
                AddSymbolToQueryList(sSymbolName, m_nOutputCodeLength + 2);
                m_nOutputCodeLength += 6;
            }
            else
            {
                PrintBinaryAddress();
                sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                        "JSR FE_%s\n", m_pcIdentifierList[nIdentifier].m_psIdentifier.CStr());
                m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH +
                                       m_pcIdentifierList[nIdentifier].m_psIdentifier.GetLength() + 8;
            }
            m_nBinaryCodeLength += 6;
        }

        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_STATEMENT)
    {

        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_COMPOUND_STATEMENT)
    {

        int nStackAtStart = m_nStackCurrentDepth;

        while (m_nOccupiedVariables >= 0 &&
               m_pcVarStackList[m_nOccupiedVariables].m_nVarLevel == m_nVarStackRecursionLevel)
        {
            if (m_pcVarStackList[m_nOccupiedVariables].m_nVarType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT)
            {
                m_nStackCurrentDepth -= GetStructureSize(m_pcVarStackList[m_nOccupiedVariables].m_sVarStructureName) >> 2;
            }
            else
            {
                --m_nStackCurrentDepth;
            }
            --m_nOccupiedVariables;
        }

        --m_nVarStackRecursionLevel;

        int nStackModifier = (m_nStackCurrentDepth - nStackAtStart) * 4;
        if (nStackModifier != 0)
        {
            if (m_nCompileSymbolicOutput == 0)
            {
                m_pchOutputCode[m_nOutputCodeLength] = VM_MOVSP;
                m_pchOutputCode[m_nOutputCodeLength + 1] = 0;
                m_pchOutputCode[m_nOutputCodeLength + 2] = (char) ((nStackModifier >> 24) & 0xff);
                m_pchOutputCode[m_nOutputCodeLength + 3] = (char) ((nStackModifier >> 16) & 0xff);
                m_pchOutputCode[m_nOutputCodeLength + 4] = (char) ((nStackModifier >> 8) & 0xff);
                m_pchOutputCode[m_nOutputCodeLength + 5] = (char) (nStackModifier & 0xff);
                m_nOutputCodeLength += 6;
            }
            else
            {
                PrintBinaryAddress();
                sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                        "MOVSP %08x\n", nStackModifier);
                m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 15;
            }
            m_nBinaryCodeLength += 6;
        }

        if (pNode->m_nStackPointer != m_nStackCurrentDepth)
        {
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_INCORRECT_VARIABLE_STATE_LEFT_ON_STACK, pNode);
        }
        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_IF_CONDITION ||
        pNode->nOperation == CSCRIPTCOMPILER_OPERATION_COND_CONDITION ||
        pNode->nOperation == CSCRIPTCOMPILER_OPERATION_WHILE_CONDITION ||
        pNode->nOperation == CSCRIPTCOMPILER_OPERATION_DOWHILE_CONDITION ||
        pNode->nOperation == CSCRIPTCOMPILER_OPERATION_SWITCH_CONDITION)
    {
        if (pNode->pLeft != NULL)
        {
            pNode->nType = pNode->pLeft->nType;
            if (pNode->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT)
            {
                pNode->m_psTypeName = pNode->pLeft->m_psTypeName;
            }
        }

        if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_SWITCH_CONDITION &&
            m_pchStackTypes[m_nStackCurrentDepth - 1] != VM_STACK_INTEGER)
        {
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_SWITCH_MUST_EVALUATE_TO_AN_INTEGER, pNode);
        }
        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_SWITCH_BLOCK)
    {

        if (m_nCompileSymbolicOutput == 0)
        {
            CExoString sSymbolName;
            sSymbolName.Format("_BR_%08x", m_nSwitchIdentifier);
            AddSymbolToLabelList(sSymbolName, m_nOutputCodeLength);
        }
        else
        {
            sprintf(m_pchOutputCode + m_nOutputCodeLength, "_BR_%08x:\n", m_nSwitchIdentifier);
            m_nOutputCodeLength += 14;
        }

        --m_nSwitchLevel;
        m_nSwitchIdentifier = pNode->nIntegerData2;
        m_nSwitchStackDepth = pNode->nIntegerData3;
        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_WHILE_BLOCK)
    {

        if (m_nCompileSymbolicOutput == 0)
        {
            m_pchOutputCode[m_nOutputCodeLength] = VM_JMP;
            m_pchOutputCode[m_nOutputCodeLength + 1] = 0;

            int nJmpLength = pNode->nIntegerData - m_nOutputCodeLength;
            m_pchOutputCode[m_nOutputCodeLength + 2] = (char) ((nJmpLength >> 24) & 0xff);
            m_pchOutputCode[m_nOutputCodeLength + 3] = (char) ((nJmpLength >> 16) & 0xff);
            m_pchOutputCode[m_nOutputCodeLength + 4] = (char) ((nJmpLength >> 8) & 0xff);
            m_pchOutputCode[m_nOutputCodeLength + 5] = (char) (nJmpLength & 0xff);
            m_nOutputCodeLength += 6;

            nJmpLength = m_nOutputCodeLength - pNode->nIntegerData2;
            m_pchOutputCode[pNode->nIntegerData2 + 2] = (char) ((nJmpLength >> 24) & 0xff);
            m_pchOutputCode[pNode->nIntegerData2 + 3] = (char) ((nJmpLength >> 16) & 0xff);
            m_pchOutputCode[pNode->nIntegerData2 + 4] = (char) ((nJmpLength >> 8) & 0xff);
            m_pchOutputCode[pNode->nIntegerData2 + 5] = (char) (nJmpLength & 0xff);

            CExoString sSymbolName;
            sSymbolName.Format("_BR_%08x", m_nLoopIdentifier);
            AddSymbolToLabelList(sSymbolName, m_nOutputCodeLength);
        }
        else
        {
            PrintBinaryAddress();
            sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                    "JMP _W1_%08x\n", pNode->nIntegerData);
            m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 17;
            sprintf(m_pchOutputCode + m_nOutputCodeLength, "_W2_%08x:\n", pNode->nIntegerData);
            m_nOutputCodeLength += 14;
            sprintf(m_pchOutputCode + m_nOutputCodeLength, "_BR_%08x:\n", m_nLoopIdentifier);
            m_nOutputCodeLength += 14;
        }
        m_nBinaryCodeLength += 6;

        pNode->nType = pNode->pLeft->nType;
        if (pNode->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT)
        {
            pNode->m_psTypeName = pNode->pLeft->m_psTypeName;
        }

        m_nLoopIdentifier = pNode->nIntegerData3;
        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_WHILE_CHOICE)
    {

        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_DOWHILE_BLOCK)
    {

        if (m_pchStackTypes[m_nStackCurrentDepth - 1] != VM_STACK_INTEGER)
        {
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_INTEGER_NOT_AT_TOP_OF_STACK, pNode);
        }
        --m_nStackCurrentDepth;

        if (m_nCompileSymbolicOutput == 0)
        {

            int nJmpLength = 12;
            m_pchOutputCode[m_nOutputCodeLength] = VM_JZ;
            m_pchOutputCode[m_nOutputCodeLength + 1] = 0;
            m_pchOutputCode[m_nOutputCodeLength + 2] = (char) ((nJmpLength >> 24) & 0xff);
            m_pchOutputCode[m_nOutputCodeLength + 3] = (char) ((nJmpLength >> 16) & 0xff);
            m_pchOutputCode[m_nOutputCodeLength + 4] = (char) ((nJmpLength >> 8) & 0xff);
            m_pchOutputCode[m_nOutputCodeLength + 5] = (char) (nJmpLength & 0xff);
            m_nOutputCodeLength += 6;
        }
        else
        {
            PrintBinaryAddress();
            sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                    "JZ _DW2_%08x\n", pNode->nIntegerData);
            m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 17;
        }
        m_nBinaryCodeLength += 6;

        if (m_nCompileSymbolicOutput == 0)
        {
            m_pchOutputCode[m_nOutputCodeLength] = VM_JMP;
            m_pchOutputCode[m_nOutputCodeLength + 1] = 0;

            int nJmpLength = pNode->nIntegerData - m_nOutputCodeLength;
            m_pchOutputCode[m_nOutputCodeLength + 2] = (char) ((nJmpLength >> 24) & 0xff);
            m_pchOutputCode[m_nOutputCodeLength + 3] = (char) ((nJmpLength >> 16) & 0xff);
            m_pchOutputCode[m_nOutputCodeLength + 4] = (char) ((nJmpLength >> 8) & 0xff);
            m_pchOutputCode[m_nOutputCodeLength + 5] = (char) (nJmpLength & 0xff);
            m_nOutputCodeLength += 6;

            CExoString sSymbolName;
            sSymbolName.Format("_BR_%08x", m_nLoopIdentifier);
            AddSymbolToLabelList(sSymbolName, m_nOutputCodeLength);
        }
        else
        {
            PrintBinaryAddress();
            sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                    "JMP _DW1_%08x\n", pNode->nIntegerData);
            m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 18;
            sprintf(m_pchOutputCode + m_nOutputCodeLength, "_DW2_%08x:\n", pNode->nIntegerData);
            m_nOutputCodeLength += 15;
            sprintf(m_pchOutputCode + m_nOutputCodeLength, "_BR_%08x:\n", m_nLoopIdentifier);
            m_nOutputCodeLength += 14;
        }
        m_nBinaryCodeLength += 6;

        pNode->nType = pNode->pRight->nType;
        if (pNode->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT)
        {
            pNode->m_psTypeName = pNode->pRight->m_psTypeName;
        }

        m_nLoopIdentifier = pNode->nIntegerData3;
        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_IF_CHOICE)
    {

        if (m_nCompileSymbolicOutput == 0)
        {
            int nJmpLength = m_nOutputCodeLength - pNode->nIntegerData2;
            m_pchOutputCode[pNode->nIntegerData2 + 2] = (char) ((nJmpLength >> 24) & 0xff);
            m_pchOutputCode[pNode->nIntegerData2 + 3] = (char) ((nJmpLength >> 16) & 0xff);
            m_pchOutputCode[pNode->nIntegerData2 + 4] = (char) ((nJmpLength >> 8) & 0xff);
            m_pchOutputCode[pNode->nIntegerData2 + 5] = (char) (nJmpLength & 0xff);
        }
        else
        {
            sprintf(m_pchOutputCode + m_nOutputCodeLength, "_I2_%08x:\n", pNode->nIntegerData);
            m_nOutputCodeLength += 14;
        }
        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_IF_BLOCK)
    {
        pNode->nType = pNode->pLeft->nType;
        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_COND_CHOICE)
    {

        if (m_nCompileSymbolicOutput == 0)
        {
            int nJmpLength = m_nOutputCodeLength - pNode->nIntegerData2;
            m_pchOutputCode[pNode->nIntegerData2 + 2] = (char) ((nJmpLength >> 24) & 0xff);
            m_pchOutputCode[pNode->nIntegerData2 + 3] = (char) ((nJmpLength >> 16) & 0xff);
            m_pchOutputCode[pNode->nIntegerData2 + 4] = (char) ((nJmpLength >> 8) & 0xff);
            m_pchOutputCode[pNode->nIntegerData2 + 5] = (char) (nJmpLength & 0xff);
        }
        else
        {
            sprintf(m_pchOutputCode + m_nOutputCodeLength, "_CH2_%08x:\n", pNode->nIntegerData);
            m_nOutputCodeLength += 15;
        }

        if (pNode->pLeft->nType != pNode->pRight->nType ||
            (pNode->pLeft->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT &&
             pNode->pLeft->m_psTypeName != pNode->pRight->m_psTypeName))
        {
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_CONDITIONAL_MUST_HAVE_MATCHING_RETURN_TYPES, pNode);
        }

        pNode->nType = pNode->pLeft->nType;
        if (pNode->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT)
        {
            pNode->m_psTypeName = pNode->pLeft->m_psTypeName;
        }
        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_COND_BLOCK)
    {

        pNode->nType = pNode->pRight->nType;
        if (pNode->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT)
        {
            pNode->m_psTypeName = pNode->pRight->m_psTypeName;
        }
        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_NON_VOID_EXPRESSION ||
        pNode->nOperation == CSCRIPTCOMPILER_OPERATION_INTEGER_EXPRESSION)
    {

        int nRemovedTokens = m_nStackCurrentDepth - pNode->m_nStackPointer;

        if (nRemovedTokens == 0)
        {
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_VOID_EXPRESSION_WHERE_NON_VOID_REQUIRED, pNode);
        }
        if (nRemovedTokens > 1)
        {
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_INCORRECT_VARIABLE_STATE_LEFT_ON_STACK, pNode);
        }
        if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_INTEGER_EXPRESSION &&
            m_pchStackTypes[m_nStackCurrentDepth - 1] != VM_STACK_INTEGER)
        {
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_NON_INTEGER_EXPRESSION_WHERE_INTEGER_REQUIRED, pNode);
        }

        pNode->nType = pNode->pLeft->nType;
        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_LOGICAL_AND)
    {
        if (pNode->pLeft != NULL && pNode->pRight != NULL)
        {
            if (pNode->pLeft->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_INT &&
                pNode->pRight->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_INT)
            {
                if (m_nCompileSymbolicOutput == 0)
                {
                    m_pchOutputCode[m_nOutputCodeLength] = VM_LOGAND;
                    m_pchOutputCode[m_nOutputCodeLength + 1] = VM_OPERAND_INTEGER_INTEGER;
                    m_nOutputCodeLength += 2;
                }
                else
                {
                    PrintBinaryAddress();
                    sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH, "LOGANDII\n");
                    m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 9;
                }
                m_nBinaryCodeLength += 2;

                --m_nStackCurrentDepth;
                pNode->nType = CSCRIPTCOMPILER_TOKEN_KEYWORD_INT;

                if (m_nCompileSymbolicOutput == 0)
                {
                    int nJmpLength = m_nOutputCodeLength - pNode->nIntegerData;
                    m_pchOutputCode[pNode->nIntegerData + 2] = (char) ((nJmpLength >> 24) & 0xff);
                    m_pchOutputCode[pNode->nIntegerData + 3] = (char) ((nJmpLength >> 16) & 0xff);
                    m_pchOutputCode[pNode->nIntegerData + 4] = (char) ((nJmpLength >> 8) & 0xff);
                    m_pchOutputCode[pNode->nIntegerData + 5] = (char) (nJmpLength & 0xff);
                }
                else
                {
                    sprintf(m_pchOutputCode + m_nOutputCodeLength, "_ILA_%08x:\n", pNode->nIntegerData);
                    m_nOutputCodeLength += 15;
                }
                return 0;
            }
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_LOGICAL_OPERATION_HAS_INVALID_OPERANDS, pNode);
        }
        return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_UNKNOWN_STATE_IN_COMPILER, pNode);
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_LOGICAL_OR)
    {
        if (pNode->pLeft != NULL && pNode->pRight != NULL)
        {
            if (pNode->pLeft->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_INT &&
                pNode->pRight->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_INT)
            {
                pNode->nType = CSCRIPTCOMPILER_TOKEN_KEYWORD_INT;

                if (m_nCompileSymbolicOutput == 0)
                {
                    int nJmpLength = m_nOutputCodeLength - pNode->nIntegerData;
                    m_pchOutputCode[pNode->nIntegerData + 2] = (char) ((nJmpLength >> 24) & 0xff);
                    m_pchOutputCode[pNode->nIntegerData + 3] = (char) ((nJmpLength >> 16) & 0xff);
                    m_pchOutputCode[pNode->nIntegerData + 4] = (char) ((nJmpLength >> 8) & 0xff);
                    m_pchOutputCode[pNode->nIntegerData + 5] = (char) (nJmpLength & 0xff);
                }
                else
                {
                    sprintf(m_pchOutputCode + m_nOutputCodeLength, "_ILO2_%08x:\n", pNode->nIntegerData);
                    m_nOutputCodeLength += 16;
                }

                if (m_nCompileSymbolicOutput == 0)
                {
                    m_pchOutputCode[m_nOutputCodeLength] = VM_LOGOR;
                    m_pchOutputCode[m_nOutputCodeLength + 1] = VM_OPERAND_INTEGER_INTEGER;
                    m_nOutputCodeLength += 2;
                }
                else
                {
                    PrintBinaryAddress();
                    sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH, "LOGORII\n");
                    m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 8;
                }
                m_nBinaryCodeLength += 2;

                --m_nStackCurrentDepth;
                pNode->nType = CSCRIPTCOMPILER_TOKEN_KEYWORD_INT;
                return 0;
            }
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_LOGICAL_OPERATION_HAS_INVALID_OPERANDS, pNode);
        }
        return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_UNKNOWN_STATE_IN_COMPILER, pNode);
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_INCLUSIVE_OR)
    {
        if (pNode->pLeft != NULL && pNode->pRight != NULL)
        {
            if (pNode->pLeft->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_INT &&
                pNode->pRight->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_INT)
            {
                if (m_nCompileSymbolicOutput == 0)
                {
                    m_pchOutputCode[m_nOutputCodeLength] = VM_INCOR;
                    m_pchOutputCode[m_nOutputCodeLength + 1] = VM_OPERAND_INTEGER_INTEGER;
                    m_nOutputCodeLength += 2;
                }
                else
                {
                    PrintBinaryAddress();
                    sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH, "INCORII\n");
                    m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 8;
                }
                m_nBinaryCodeLength += 2;

                --m_nStackCurrentDepth;
                pNode->nType = CSCRIPTCOMPILER_TOKEN_KEYWORD_INT;
                return 0;
            }
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_LOGICAL_OPERATION_HAS_INVALID_OPERANDS, pNode);
        }
        return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_UNKNOWN_STATE_IN_COMPILER, pNode);
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_EXCLUSIVE_OR)
    {
        if (pNode->pLeft != NULL && pNode->pRight != NULL)
        {
            if (pNode->pLeft->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_INT &&
                pNode->pRight->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_INT)
            {
                if (m_nCompileSymbolicOutput == 0)
                {
                    m_pchOutputCode[m_nOutputCodeLength] = VM_EXCOR;
                    m_pchOutputCode[m_nOutputCodeLength + 1] = VM_OPERAND_INTEGER_INTEGER;
                    m_nOutputCodeLength += 2;
                }
                else
                {
                    PrintBinaryAddress();
                    sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH, "EXCORII\n");
                    m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 8;
                }
                m_nBinaryCodeLength += 2;

                --m_nStackCurrentDepth;
                pNode->nType = CSCRIPTCOMPILER_TOKEN_KEYWORD_INT;
                return 0;
            }
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_LOGICAL_OPERATION_HAS_INVALID_OPERANDS, pNode);
        }
        return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_UNKNOWN_STATE_IN_COMPILER, pNode);
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_BOOLEAN_AND)
    {
        if (pNode->pLeft != NULL && pNode->pRight != NULL)
        {
            if (pNode->pLeft->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_INT &&
                pNode->pRight->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_INT)
            {
                if (m_nCompileSymbolicOutput == 0)
                {
                    m_pchOutputCode[m_nOutputCodeLength] = VM_BOOLAND;
                    m_pchOutputCode[m_nOutputCodeLength + 1] = VM_OPERAND_INTEGER_INTEGER;
                    m_nOutputCodeLength += 2;
                }
                else
                {
                    PrintBinaryAddress();
                    sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH, "BOOLANDII\n");
                    m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 10;
                }
                m_nBinaryCodeLength += 2;

                --m_nStackCurrentDepth;
                pNode->nType = CSCRIPTCOMPILER_TOKEN_KEYWORD_INT;
                return 0;
            }
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_LOGICAL_OPERATION_HAS_INVALID_OPERANDS, pNode);
        }
        return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_UNKNOWN_STATE_IN_COMPILER, pNode);
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_CONDITION_EQUAL ||
        pNode->nOperation == CSCRIPTCOMPILER_OPERATION_CONDITION_NOT_EQUAL)
    {
        if (pNode->pLeft != NULL && pNode->pRight != NULL)
        {
            if (pNode->pLeft->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_INT &&
                pNode->pRight->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_INT)
            {
                if (m_nCompileSymbolicOutput == 0)
                {
                    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_CONDITION_EQUAL)
                    {
                        m_pchOutputCode[m_nOutputCodeLength] = VM_EQUAL;
                    }
                    else
                    {
                        m_pchOutputCode[m_nOutputCodeLength] = VM_NEQUAL;
                    }
                    m_pchOutputCode[m_nOutputCodeLength + 1] = VM_OPERAND_INTEGER_INTEGER;
                    m_nOutputCodeLength += 2;
                }
                else
                {
                    PrintBinaryAddress();
                    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_CONDITION_EQUAL)
                    {
                        sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH, "EQUALII\n");
                        m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 8;
                    }
                    else
                    {
                        sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH, "NEQUALII\n");
                        m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 9;
                    }
                }
                m_nBinaryCodeLength += 2;

                --m_nStackCurrentDepth;
                pNode->nType = CSCRIPTCOMPILER_TOKEN_KEYWORD_INT;
                return 0;
            }

            if (pNode->pLeft->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT &&
                pNode->pRight->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT)
            {
                if (m_nCompileSymbolicOutput == 0)
                {
                    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_CONDITION_EQUAL)
                    {
                        m_pchOutputCode[m_nOutputCodeLength] = VM_EQUAL;
                    }
                    else
                    {
                        m_pchOutputCode[m_nOutputCodeLength] = VM_NEQUAL;
                    }
                    m_pchOutputCode[m_nOutputCodeLength + 1] = VM_OPERAND_FLOAT_FLOAT;
                    m_nOutputCodeLength += 2;
                }
                else
                {
                    PrintBinaryAddress();
                    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_CONDITION_EQUAL)
                    {
                        sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH, "EQUALFF\n");
                        m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 8;
                    }
                    else
                    {
                        sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH, "NEQUALFF\n");
                        m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 9;
                    }
                }
                m_nBinaryCodeLength += 2;

                m_nStackCurrentDepth -= 2;
                AddVariableToStack(CSCRIPTCOMPILER_TOKEN_KEYWORD_INT, "", FALSE);
                pNode->nType = CSCRIPTCOMPILER_TOKEN_KEYWORD_INT;
                return 0;
            }

            if (pNode->pLeft->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_OBJECT &&
                pNode->pRight->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_OBJECT)
            {
                if (m_nCompileSymbolicOutput == 0)
                {
                    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_CONDITION_EQUAL)
                    {
                        m_pchOutputCode[m_nOutputCodeLength] = VM_EQUAL;
                    }
                    else
                    {
                        m_pchOutputCode[m_nOutputCodeLength] = VM_NEQUAL;
                    }
                    m_pchOutputCode[m_nOutputCodeLength + 1] = VM_OPERAND_OBJECT_OBJECT;
                    m_nOutputCodeLength += 2;
                }
                else
                {
                    PrintBinaryAddress();
                    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_CONDITION_EQUAL)
                    {
                        sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH, "EQUALOO\n");
                        m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 8;
                    }
                    else
                    {
                        sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH, "NEQUALOO\n");
                        m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 9;
                    }
                }
                m_nBinaryCodeLength += 2;

                m_nStackCurrentDepth -= 2;
                AddVariableToStack(CSCRIPTCOMPILER_TOKEN_KEYWORD_INT, "", FALSE);
                pNode->nType = CSCRIPTCOMPILER_TOKEN_KEYWORD_INT;
                return 0;
            }

            if (pNode->pLeft->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRING &&
                pNode->pRight->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRING)
            {
                if (m_nCompileSymbolicOutput == 0)
                {
                    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_CONDITION_EQUAL)
                    {
                        m_pchOutputCode[m_nOutputCodeLength] = VM_EQUAL;
                    }
                    else
                    {
                        m_pchOutputCode[m_nOutputCodeLength] = VM_NEQUAL;
                    }
                    m_pchOutputCode[m_nOutputCodeLength + 1] = VM_OPERAND_STRING_STRING;
                    m_nOutputCodeLength += 2;
                }
                else
                {
                    PrintBinaryAddress();
                    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_CONDITION_EQUAL)
                    {
                        sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH, "EQUALSS\n");
                        m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 8;
                    }
                    else
                    {
                        sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH, "NEQUALSS\n");
                        m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 9;
                    }
                }
                m_nBinaryCodeLength += 2;

                m_nStackCurrentDepth -= 2;
                AddVariableToStack(CSCRIPTCOMPILER_TOKEN_KEYWORD_INT, "", FALSE);
                pNode->nType = CSCRIPTCOMPILER_TOKEN_KEYWORD_INT;
                return 0;
            }

            if (pNode->pLeft->nType >= CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE0 &&
                pNode->pLeft->nType <= CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE9 &&
                pNode->pLeft->nType == pNode->pRight->nType)
            {
                int nEngineStructure = pNode->pLeft->nType - CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE0;

                if (m_nCompileSymbolicOutput == 0)
                {
                    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_CONDITION_EQUAL)
                    {
                        m_pchOutputCode[m_nOutputCodeLength] = VM_EQUAL;
                    }
                    else
                    {
                        m_pchOutputCode[m_nOutputCodeLength] = VM_NEQUAL;
                    }
                    m_pchOutputCode[m_nOutputCodeLength + 1] = (char) (VM_OPERAND_ENGINE_STRUCTURE_FIRST + nEngineStructure);
                    m_nOutputCodeLength += 2;
                }
                else
                {
                    PrintBinaryAddress();
                    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_CONDITION_EQUAL)
                    {
                        sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                                "EQUALP%1dP%1d\n", nEngineStructure, nEngineStructure);
                        m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 10;
                    }
                    else
                    {
                        sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                                "NEQUALP%1dP%1d\n", nEngineStructure, nEngineStructure);
                        m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 11;
                    }
                }
                m_nBinaryCodeLength += 2;

                m_nStackCurrentDepth -= 2;
                AddVariableToStack(CSCRIPTCOMPILER_TOKEN_KEYWORD_INT, "", FALSE);
                pNode->nType = CSCRIPTCOMPILER_TOKEN_KEYWORD_INT;
                return 0;
            }

            if (pNode->pLeft->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT &&
                pNode->pRight->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT &&
                pNode->pLeft->m_psTypeName == pNode->pRight->m_psTypeName)
            {

                int nSize = GetStructureSize(pNode->pLeft->m_psTypeName);

                if (m_nCompileSymbolicOutput == 0)
                {
                    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_CONDITION_EQUAL)
                    {
                        m_pchOutputCode[m_nOutputCodeLength] = VM_EQUAL;
                    }
                    else
                    {
                        m_pchOutputCode[m_nOutputCodeLength] = VM_NEQUAL;
                    }
                    m_pchOutputCode[m_nOutputCodeLength + 1] = VM_OPERAND_STRUCTURE_STRUCTURE;
                    m_pchOutputCode[m_nOutputCodeLength + 2] = (char) ((nSize >> 8) & 0xff);
                    m_pchOutputCode[m_nOutputCodeLength + 3] = (char) (nSize & 0xff);
                    m_nOutputCodeLength += 4;
                }
                else
                {
                    PrintBinaryAddress();
                    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_CONDITION_EQUAL)
                    {
                        sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                                "EQUALTT %04x\n", nSize);
                        m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 13;
                    }
                    else
                    {
                        sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                                "NEQUALTT %04x\n", nSize);
                        m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 14;
                    }
                }
                m_nBinaryCodeLength += 4;

                m_nStackCurrentDepth -= nSize >> 1;
                AddVariableToStack(CSCRIPTCOMPILER_TOKEN_KEYWORD_INT, "", FALSE);
                pNode->nType = CSCRIPTCOMPILER_TOKEN_KEYWORD_INT;
                return 0;
            }

            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_EQUALITY_TEST_HAS_INVALID_OPERANDS, pNode);
        }
        return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_UNKNOWN_STATE_IN_COMPILER, pNode);
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_CONDITION_GEQ ||
        pNode->nOperation == CSCRIPTCOMPILER_OPERATION_CONDITION_GT ||
        pNode->nOperation == CSCRIPTCOMPILER_OPERATION_CONDITION_LT ||
        pNode->nOperation == CSCRIPTCOMPILER_OPERATION_CONDITION_LEQ)
    {

        if (m_nCompileSymbolicOutput == 0)
        {
            if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_CONDITION_GEQ)
            {
                m_pchOutputCode[m_nOutputCodeLength] = VM_GEQ;
            }
            else if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_CONDITION_GT)
            {
                m_pchOutputCode[m_nOutputCodeLength] = VM_GT;
            }
            else if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_CONDITION_LT)
            {
                m_pchOutputCode[m_nOutputCodeLength] = VM_LT;
            }
            else
            {
                m_pchOutputCode[m_nOutputCodeLength] = VM_LEQ;
            }
        }
        else
        {
            PrintBinaryAddress();
            m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH;
            if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_CONDITION_GEQ)
            {
                sprintf(m_pchOutputCode + m_nOutputCodeLength, "GEQ");
                m_nOutputCodeLength += 3;
            }
            else if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_CONDITION_GT)
            {
                sprintf(m_pchOutputCode + m_nOutputCodeLength, "GT");
                m_nOutputCodeLength += 2;
            }
            else if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_CONDITION_LT)
            {
                sprintf(m_pchOutputCode + m_nOutputCodeLength, "LT");
                m_nOutputCodeLength += 2;
            }
            else
            {
                sprintf(m_pchOutputCode + m_nOutputCodeLength, "LEQ");
                m_nOutputCodeLength += 3;
            }
        }

        if (pNode->pLeft != NULL && pNode->pRight != NULL)
        {
            if (pNode->pLeft->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_INT &&
                pNode->pRight->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_INT)
            {
                if (m_nCompileSymbolicOutput == 0)
                {
                    m_pchOutputCode[m_nOutputCodeLength + 1] = VM_OPERAND_INTEGER_INTEGER;
                    m_nOutputCodeLength += 2;
                }
                else
                {
                    sprintf(m_pchOutputCode + m_nOutputCodeLength, "II\n");
                    m_nOutputCodeLength += 3;
                }
                m_nBinaryCodeLength += 2;

                --m_nStackCurrentDepth;
                pNode->nType = CSCRIPTCOMPILER_TOKEN_KEYWORD_INT;
                return 0;
            }

            if (pNode->pLeft->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT &&
                pNode->pRight->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT)
            {
                if (m_nCompileSymbolicOutput == 0)
                {
                    m_pchOutputCode[m_nOutputCodeLength + 1] = VM_OPERAND_FLOAT_FLOAT;
                    m_nOutputCodeLength += 2;
                }
                else
                {
                    sprintf(m_pchOutputCode + m_nOutputCodeLength, "FF\n");
                    m_nOutputCodeLength += 3;
                }
                m_nBinaryCodeLength += 2;

                m_nStackCurrentDepth -= 2;
                AddVariableToStack(CSCRIPTCOMPILER_TOKEN_KEYWORD_INT, "", FALSE);
                pNode->nType = CSCRIPTCOMPILER_TOKEN_KEYWORD_INT;
                return 0;
            }

            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_COMPARISON_TEST_HAS_INVALID_OPERANDS, pNode);
        }
        return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_UNKNOWN_STATE_IN_COMPILER, pNode);
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_SHIFT_LEFT ||
        pNode->nOperation == CSCRIPTCOMPILER_OPERATION_SHIFT_RIGHT ||
        pNode->nOperation == CSCRIPTCOMPILER_OPERATION_UNSIGNED_SHIFT_RIGHT)
    {
        if (pNode->pLeft != NULL && pNode->pRight != NULL)
        {
            if (pNode->pLeft->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_INT &&
                pNode->pRight->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_INT)
            {
                if (m_nCompileSymbolicOutput == 0)
                {
                    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_SHIFT_LEFT)
                    {
                        m_pchOutputCode[m_nOutputCodeLength] = VM_SHLEFT;
                    }
                    else if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_SHIFT_RIGHT)
                    {
                        m_pchOutputCode[m_nOutputCodeLength] = VM_SHRIGHT;
                    }
                    else
                    {
                        m_pchOutputCode[m_nOutputCodeLength] = VM_USHRIGHT;
                    }
                    m_pchOutputCode[m_nOutputCodeLength + 1] = VM_OPERAND_INTEGER_INTEGER;
                    m_nOutputCodeLength += 2;
                }
                else
                {
                    PrintBinaryAddress();
                    m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH;

                    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_SHIFT_LEFT)
                    {
                        sprintf(m_pchOutputCode + m_nOutputCodeLength, "SHLEFT\n");
                        m_nOutputCodeLength += 7;
                    }
                    else if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_CONDITION_GT)
                    {
                        sprintf(m_pchOutputCode + m_nOutputCodeLength, "SHRIGHT\n");
                        m_nOutputCodeLength += 8;
                    }
                    else
                    {
                        sprintf(m_pchOutputCode + m_nOutputCodeLength, "USHRIGHTII\n");
                        m_nOutputCodeLength += 11;
                    }
                }
                m_nBinaryCodeLength += 2;

                m_nStackCurrentDepth -= 2;
                AddVariableToStack(CSCRIPTCOMPILER_TOKEN_KEYWORD_INT, "", FALSE);
                pNode->nType = CSCRIPTCOMPILER_TOKEN_KEYWORD_INT;
                return 0;
            }
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_SHIFT_OPERATION_HAS_INVALID_OPERANDS, pNode);
        }
        return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_UNKNOWN_STATE_IN_COMPILER, pNode);
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_ADD ||
        pNode->nOperation == CSCRIPTCOMPILER_OPERATION_SUBTRACT ||
        pNode->nOperation == CSCRIPTCOMPILER_OPERATION_MULTIPLY ||
        pNode->nOperation == CSCRIPTCOMPILER_OPERATION_DIVIDE)
    {

        if (m_nCompileSymbolicOutput == 0)
        {
            if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_ADD)
            {
                m_pchOutputCode[m_nOutputCodeLength] = VM_ADD;
            }
            else if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_SUBTRACT)
            {
                m_pchOutputCode[m_nOutputCodeLength] = VM_SUB;
            }
            else if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_MULTIPLY)
            {
                m_pchOutputCode[m_nOutputCodeLength] = VM_MUL;
            }
            else
            {
                m_pchOutputCode[m_nOutputCodeLength] = VM_DIV;
            }
        }
        else
        {
            PrintBinaryAddress();
            m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH;
            if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_ADD)
            {
                sprintf(m_pchOutputCode + m_nOutputCodeLength, "ADD");
                m_nOutputCodeLength += 3;
            }
            else if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_SUBTRACT)
            {
                sprintf(m_pchOutputCode + m_nOutputCodeLength, "SUB");
                m_nOutputCodeLength += 3;
            }
            else if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_MULTIPLY)
            {
                sprintf(m_pchOutputCode + m_nOutputCodeLength, "MUL");
                m_nOutputCodeLength += 3;
            }
            else
            {
                sprintf(m_pchOutputCode + m_nOutputCodeLength, "DIV");
                m_nOutputCodeLength += 3;
            }
        }

        if (pNode->pLeft != NULL && pNode->pRight != NULL)
        {
            if (pNode->pLeft->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_INT &&
                pNode->pRight->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_INT)
            {
                if (m_nCompileSymbolicOutput == 0)
                {
                    m_pchOutputCode[m_nOutputCodeLength + 1] = VM_OPERAND_INTEGER_INTEGER;
                    m_nOutputCodeLength += 2;
                }
                else
                {
                    sprintf(m_pchOutputCode + m_nOutputCodeLength, "II\n");
                    m_nOutputCodeLength += 3;
                }
                m_nBinaryCodeLength += 2;

                m_nStackCurrentDepth -= 2;
                AddVariableToStack(CSCRIPTCOMPILER_TOKEN_KEYWORD_INT, "", FALSE);
                pNode->nType = CSCRIPTCOMPILER_TOKEN_KEYWORD_INT;
                return 0;
            }

            if (pNode->pLeft->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_INT &&
                pNode->pRight->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT)
            {
                if (m_nCompileSymbolicOutput == 0)
                {
                    m_pchOutputCode[m_nOutputCodeLength + 1] = VM_OPERAND_INTEGER_FLOAT;
                    m_nOutputCodeLength += 2;
                }
                else
                {
                    sprintf(m_pchOutputCode + m_nOutputCodeLength, "IF\n");
                    m_nOutputCodeLength += 3;
                }
                m_nBinaryCodeLength += 2;

                m_nStackCurrentDepth -= 2;
                AddVariableToStack(CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT, "", FALSE);
                pNode->nType = CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT;
                return 0;
            }

            if (pNode->pLeft->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT &&
                pNode->pRight->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_INT)
            {
                if (m_nCompileSymbolicOutput == 0)
                {
                    m_pchOutputCode[m_nOutputCodeLength + 1] = VM_OPERAND_FLOAT_INTEGER;
                    m_nOutputCodeLength += 2;
                }
                else
                {
                    sprintf(m_pchOutputCode + m_nOutputCodeLength, "FI\n");
                    m_nOutputCodeLength += 3;
                }
                m_nBinaryCodeLength += 2;

                m_nStackCurrentDepth -= 2;
                AddVariableToStack(CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT, "", FALSE);
                pNode->nType = CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT;
                return 0;
            }

            if (pNode->pLeft->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT &&
                pNode->pRight->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT)
            {
                if (m_nCompileSymbolicOutput == 0)
                {
                    m_pchOutputCode[m_nOutputCodeLength + 1] = VM_OPERAND_FLOAT_FLOAT;
                    m_nOutputCodeLength += 2;
                }
                else
                {
                    sprintf(m_pchOutputCode + m_nOutputCodeLength, "FF\n");
                    m_nOutputCodeLength += 3;
                }
                m_nBinaryCodeLength += 2;

                m_nStackCurrentDepth -= 2;
                AddVariableToStack(CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT, "", FALSE);
                pNode->nType = CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT;
                return 0;
            }

            if (pNode->pLeft->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRING &&
                pNode->pRight->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRING)
            {

                if (pNode->nOperation != CSCRIPTCOMPILER_OPERATION_ADD)
                {
                    return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_ARITHMETIC_OPERATION_HAS_INVALID_OPERANDS, pNode);
                }

                if (m_nCompileSymbolicOutput == 0)
                {
                    m_pchOutputCode[m_nOutputCodeLength + 1] = VM_OPERAND_STRING_STRING;
                    m_nOutputCodeLength += 2;
                }
                else
                {
                    sprintf(m_pchOutputCode + m_nOutputCodeLength, "SS\n");
                    m_nOutputCodeLength += 3;
                }
                m_nBinaryCodeLength += 2;

                m_nStackCurrentDepth -= 2;
                AddVariableToStack(CSCRIPTCOMPILER_TOKEN_KEYWORD_STRING, "", FALSE);
                pNode->nType = CSCRIPTCOMPILER_TOKEN_KEYWORD_STRING;
                return 0;
            }

            if (pNode->pLeft->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT &&
                pNode->pRight->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT)
            {
                if (pNode->pLeft->m_psTypeName == "vector" &&
                    pNode->pRight->m_psTypeName == pNode->pLeft->m_psTypeName)
                {

                    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_MULTIPLY ||
                        pNode->nOperation == CSCRIPTCOMPILER_OPERATION_DIVIDE)
                    {
                        return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_ARITHMETIC_OPERATION_HAS_INVALID_OPERANDS, pNode);
                    }

                    if (m_nCompileSymbolicOutput == 0)
                    {
                        m_pchOutputCode[m_nOutputCodeLength + 1] = VM_OPERAND_VECTOR_VECTOR;
                        m_nOutputCodeLength += 2;
                    }
                    else
                    {
                        sprintf(m_pchOutputCode + m_nOutputCodeLength, "VV\n");
                        m_nOutputCodeLength += 3;
                    }
                    m_nBinaryCodeLength += 2;

                    m_nStackCurrentDepth -= 6;
                    AddVariableToStack(CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT, "vector", FALSE);
                    pNode->nType = CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT;
                    pNode->m_psTypeName = pNode->pLeft->m_psTypeName;
                    return 0;
                }
            }

            if (pNode->pLeft->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT &&
                pNode->pRight->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT)
            {
                if (pNode->pLeft->m_psTypeName == "vector")
                {

                    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_ADD ||
                        pNode->nOperation == CSCRIPTCOMPILER_OPERATION_SUBTRACT)
                    {
                        return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_ARITHMETIC_OPERATION_HAS_INVALID_OPERANDS, pNode);
                    }

                    if (m_nCompileSymbolicOutput == 0)
                    {
                        m_pchOutputCode[m_nOutputCodeLength + 1] = VM_OPERAND_VECTOR_FLOAT;
                        m_nOutputCodeLength += 2;
                    }
                    else
                    {
                        sprintf(m_pchOutputCode + m_nOutputCodeLength, "VF\n");
                        m_nOutputCodeLength += 3;
                    }
                    m_nBinaryCodeLength += 2;

                    m_nStackCurrentDepth -= 4;
                    AddVariableToStack(CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT, "vector", FALSE);
                    pNode->nType = CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT;
                    pNode->m_psTypeName = pNode->pLeft->m_psTypeName;
                    return 0;
                }
            }

            if (pNode->pLeft->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT &&
                pNode->pRight->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT)
            {
                if (pNode->pRight->m_psTypeName == "vector")
                {

                    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_ADD ||
                        pNode->nOperation == CSCRIPTCOMPILER_OPERATION_SUBTRACT ||
                        pNode->nOperation == CSCRIPTCOMPILER_OPERATION_DIVIDE)
                    {
                        return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_ARITHMETIC_OPERATION_HAS_INVALID_OPERANDS, pNode);
                    }

                    if (m_nCompileSymbolicOutput == 0)
                    {
                        m_pchOutputCode[m_nOutputCodeLength + 1] = VM_OPERAND_FLOAT_VECTOR;
                        m_nOutputCodeLength += 2;
                    }
                    else
                    {
                        sprintf(m_pchOutputCode + m_nOutputCodeLength, "FV\n");
                        m_nOutputCodeLength += 3;
                    }
                    m_nBinaryCodeLength += 2;

                    m_nStackCurrentDepth -= 4;
                    AddVariableToStack(CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT, "vector", FALSE);
                    pNode->nType = CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT;
                    pNode->m_psTypeName = pNode->pRight->m_psTypeName;
                    return 0;
                }
            }

            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_ARITHMETIC_OPERATION_HAS_INVALID_OPERANDS, pNode);
        }
        return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_UNKNOWN_STATE_IN_COMPILER, pNode);
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_MODULUS)
    {
        if (pNode->pLeft != NULL && pNode->pRight != NULL)
        {
            if (pNode->pLeft->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_INT &&
                pNode->pRight->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_INT)
            {
                if (m_nCompileSymbolicOutput == 0)
                {
                    m_pchOutputCode[m_nOutputCodeLength] = VM_MOD;
                    m_pchOutputCode[m_nOutputCodeLength + 1] = VM_OPERAND_INTEGER_INTEGER;
                    m_nOutputCodeLength += 2;
                }
                else
                {
                    PrintBinaryAddress();
                    sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH, "MODII\n");
                    m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 6;
                }
                m_nBinaryCodeLength += 2;

                m_nStackCurrentDepth -= 2;
                AddVariableToStack(CSCRIPTCOMPILER_TOKEN_KEYWORD_INT, "", FALSE);
                pNode->nType = CSCRIPTCOMPILER_TOKEN_KEYWORD_INT;
                return 0;
            }
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_ARITHMETIC_OPERATION_HAS_INVALID_OPERANDS, pNode);
        }
        return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_UNKNOWN_STATE_IN_COMPILER, pNode);
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_NEGATION)
    {
        if (pNode->pLeft != NULL)
        {
            if (pNode->pLeft->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_INT)
            {
                if (m_nCompileSymbolicOutput == 0)
                {
                    m_pchOutputCode[m_nOutputCodeLength] = VM_NEG;
                    m_pchOutputCode[m_nOutputCodeLength + 1] = VM_STACK_INTEGER;
                    m_nOutputCodeLength += 2;
                }
                else
                {
                    PrintBinaryAddress();
                    sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH, "NEGI\n");
                    m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 5;
                }
                m_nBinaryCodeLength += 2;

                pNode->nType = CSCRIPTCOMPILER_TOKEN_KEYWORD_INT;
                return 0;
            }

            if (pNode->pLeft->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT)
            {
                if (m_nCompileSymbolicOutput == 0)
                {
                    m_pchOutputCode[m_nOutputCodeLength] = VM_NEG;
                    m_pchOutputCode[m_nOutputCodeLength + 1] = VM_STACK_FLOAT;
                    m_nOutputCodeLength += 2;
                }
                else
                {
                    PrintBinaryAddress();
                    sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH, "NEGF\n");
                    m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 5;
                }
                m_nBinaryCodeLength += 2;

                pNode->nType = CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT;
                return 0;
            }

            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_ARITHMETIC_OPERATION_HAS_INVALID_OPERANDS, pNode);
        }
        return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_UNKNOWN_STATE_IN_COMPILER, pNode);
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_ONES_COMPLEMENT)
    {
        if (pNode->pLeft != NULL)
        {
            if (pNode->pLeft->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_INT)
            {
                if (m_nCompileSymbolicOutput == 0)
                {
                    m_pchOutputCode[m_nOutputCodeLength] = VM_COMP;
                    m_pchOutputCode[m_nOutputCodeLength + 1] = VM_STACK_INTEGER;
                    m_nOutputCodeLength += 2;
                }
                else
                {
                    PrintBinaryAddress();
                    sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH, "COMPI\n");
                    m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 6;
                }
                m_nBinaryCodeLength += 2;

                pNode->nType = CSCRIPTCOMPILER_TOKEN_KEYWORD_INT;
                return 0;
            }
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_ARITHMETIC_OPERATION_HAS_INVALID_OPERANDS, pNode);
        }
        return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_UNKNOWN_STATE_IN_COMPILER, pNode);
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_BOOLEAN_NOT)
    {
        if (pNode->pLeft != NULL)
        {
            if (pNode->pLeft->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_INT)
            {
                if (m_nCompileSymbolicOutput == 0)
                {
                    m_pchOutputCode[m_nOutputCodeLength] = VM_NOT;
                    m_pchOutputCode[m_nOutputCodeLength + 1] = VM_STACK_INTEGER;
                    m_nOutputCodeLength += 2;
                }
                else
                {
                    PrintBinaryAddress();
                    sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH, "NOTI\n");
                    m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 5;
                }
                m_nBinaryCodeLength += 2;

                pNode->nType = CSCRIPTCOMPILER_TOKEN_KEYWORD_INT;
                return 0;
            }
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_ARITHMETIC_OPERATION_HAS_INVALID_OPERANDS, pNode);
        }
        return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_UNKNOWN_STATE_IN_COMPILER, pNode);
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_STRUCTURE_DEFINITION)
    {

        if (m_nStructureDefinition == 2)
        {
            m_pcStructList[m_nMaxStructures].m_nFieldEnd = m_nMaxStructureFields - 1;

            int nTotalSize = 0;
            for (nCount = m_pcStructList[m_nMaxStructures].m_nFieldStart;
                 nCount <= m_pcStructList[m_nMaxStructures].m_nFieldEnd;
                 ++nCount)
            {
                m_pcStructFieldList[nCount].m_nLocation = nTotalSize;

                if (m_pcStructFieldList[nCount].m_pchType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT)
                {
                    for (int nCount2 = 0; nCount2 < m_nMaxStructures; ++nCount2)
                    {
                        if (m_pcStructFieldList[nCount].m_psStructureName == m_pcStructList[nCount2].m_psName)
                        {
                            nTotalSize += m_pcStructList[nCount2].m_nByteSize;
                        }
                    }
                }
                else
                {
                    nTotalSize += 4;
                }
            }

            m_pcStructList[m_nMaxStructures].m_nByteSize = nTotalSize;
            if (nTotalSize != 0)
            {
                ++m_nMaxStructures;
                m_nStructureDefinition = 0;
                return 0;
            }
        }
        return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_UNKNOWN_STATE_IN_COMPILER, pNode);
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_STRUCTURE_PART)
    {
        m_bInStructurePart = FALSE;

        if (pNode->pLeft != NULL && pNode->pRight != NULL)
        {
            if (pNode->pLeft->nType != CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT)
            {
                return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_LEFT_OF_STRUCTURE_PART_NOT_STRUCTURE, pNode);
            }

            int nField = GetStructureField(pNode->pLeft->m_psTypeName, pNode->pRight->m_psStringData);
            if (nField < 0)
            {
                return OutputWalkTreeError(nField, pNode);
            }

            pNode->nType = m_pcStructFieldList[nField].m_pchType;
            if (pNode->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT)
            {
                pNode->m_psTypeName = m_pcStructFieldList[nField].m_psStructureName;
            }

            pNode->nIntegerData = pNode->pLeft->nIntegerData + m_pcStructFieldList[nField].m_nLocation;

            if (m_bAssignmentToVariable == FALSE)
            {

                int nSizeOriginal = GetStructureSize(pNode->pLeft->m_psTypeName);
                int nSize = 4;
                if (m_pcStructFieldList[nField].m_pchType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT)
                {

                    nSize = GetStructureSize(pNode->pRight->m_psTypeName);
                }
                int nStartComponent = m_pcStructFieldList[nField].m_nLocation;

                if (m_nCompileSymbolicOutput == 0)
                {
                    m_pchOutputCode[m_nOutputCodeLength] = VM_DESTRUCT;
                    m_pchOutputCode[m_nOutputCodeLength + 1] = CSCRIPTCOMPILER_AUXCODE_TYPE_VOID;
                    m_pchOutputCode[m_nOutputCodeLength + 2] = (char) ((nSizeOriginal >> 8) & 0xff);
                    m_pchOutputCode[m_nOutputCodeLength + 3] = (char) (nSizeOriginal & 0xff);
                    m_pchOutputCode[m_nOutputCodeLength + 4] = (char) ((nStartComponent >> 8) & 0xff);
                    m_pchOutputCode[m_nOutputCodeLength + 5] = (char) (nStartComponent & 0xff);
                    m_pchOutputCode[m_nOutputCodeLength + 6] = (char) ((nSize >> 8) & 0xff);
                    m_pchOutputCode[m_nOutputCodeLength + 7] = (char) (nSize & 0xff);
                    m_nOutputCodeLength += 8;
                }
                else
                {
                    PrintBinaryAddress();
                    sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                            "DESTRUCT %04x,%04x,%04x\n", nSizeOriginal, nStartComponent, nSize);
                    m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 24;
                }
                m_nBinaryCodeLength += 8;

                int nCutSizeOriginal = nSizeOriginal >> 2;
                int nCutSize = nSize >> 2;
                int nCutStartComponent = nStartComponent >> 2;

                if (nCutSize + nCutStartComponent < nCutSizeOriginal)
                {
                    m_nStackCurrentDepth -= nCutSizeOriginal;
                    m_nStackCurrentDepth += nCutSize + nCutStartComponent;
                }

                for (nCount = m_nStackCurrentDepth - nCutSize - nCutStartComponent;
                     nCount < m_nStackCurrentDepth - nCutStartComponent;
                     ++nCount)
                {
                    m_pchStackTypes[nCount] = m_pchStackTypes[nCount + nCutStartComponent];
                }

                m_nStackCurrentDepth -= nCutStartComponent;
            }
            return 0;
        }
        return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_UNKNOWN_STATE_IN_COMPILER, pNode);
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_FUNCTIONAL_UNIT)
    {
        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_FUNCTION)
    {

        if (m_bFunctionImp == FALSE)
        {
            return 0;
        }

        int nReturnValueLevel = m_nGlobalVariables;
        if (m_nFunctionImpReturnType != CSCRIPTCOMPILER_TOKEN_KEYWORD_VOID)
        {
            ++nReturnValueLevel;
        }

        while (m_nOccupiedVariables >= nReturnValueLevel)
        {
            if (m_pcVarStackList[m_nOccupiedVariables].m_nVarType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT)
            {
                m_nStackCurrentDepth -= GetStructureSize(m_pcVarStackList[m_nOccupiedVariables].m_sVarStructureName) >> 2;
            }
            else
            {
                --m_nStackCurrentDepth;
            }
            --m_nOccupiedVariables;
        }

        if (m_nCompileSymbolicOutput == 0)
        {
            CExoString sSymbolName;
            sSymbolName.Format("FX_%s", m_sFunctionImpName.CStr());
            AddSymbolToLabelList(sSymbolName, m_nOutputCodeLength);
        }
        else
        {
            sprintf(m_pchOutputCode + m_nOutputCodeLength, "FX_%s:\n", m_sFunctionImpName.CStr());
            m_nOutputCodeLength += m_sFunctionImpName.GetLength() + 5;
        }

        int nStackModifier = (m_nStackCurrentDepth - pNode->m_nStackPointer) * 4;
        if (nStackModifier != 0)
        {
            if (m_nCompileSymbolicOutput == 0)
            {
                m_pchOutputCode[m_nOutputCodeLength] = VM_MOVSP;
                m_pchOutputCode[m_nOutputCodeLength + 1] = 0;
                m_pchOutputCode[m_nOutputCodeLength + 2] = (char) ((nStackModifier >> 24) & 0xff);
                m_pchOutputCode[m_nOutputCodeLength + 3] = (char) ((nStackModifier >> 16) & 0xff);
                m_pchOutputCode[m_nOutputCodeLength + 4] = (char) ((nStackModifier >> 8) & 0xff);
                m_pchOutputCode[m_nOutputCodeLength + 5] = (char) (nStackModifier & 0xff);
                m_nOutputCodeLength += 6;
            }
            else
            {
                PrintBinaryAddress();
                sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                        "MOVSP %08x\n", nStackModifier);
                m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 15;
            }
            m_nBinaryCodeLength += 6;
        }

        if (m_nCompileSymbolicOutput == 0)
        {
            m_pchOutputCode[m_nOutputCodeLength] = VM_RETN;
            m_pchOutputCode[m_nOutputCodeLength + 1] = 0;
            m_nOutputCodeLength += 2;
        }
        else
        {
            PrintBinaryAddress();
            sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH, "RET\n");
            m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 4;
        }
        m_nBinaryCodeLength += 2;

        if (m_nFunctionImpReturnType != CSCRIPTCOMPILER_TOKEN_KEYWORD_VOID)
        {
            if (FoundReturnStatementOnAllBranches(pNode->pRight) == FALSE)
            {
                return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_NOT_ALL_CONTROL_PATHS_RETURN_A_VALUE, pNode);
            }
        }

        if (nReturnValueLevel != m_nGlobalVariables)
        {
            if (m_pcVarStackList[m_nOccupiedVariables].m_nVarType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT)
            {
                m_nStackCurrentDepth -= GetStructureSize(m_pcVarStackList[m_nOccupiedVariables].m_sVarStructureName) >> 2;
            }
            else
            {
                --m_nStackCurrentDepth;
            }
            --m_nOccupiedVariables;
        }

        --m_nVarStackRecursionLevel;

        int nIdentifier = GetIdentifierByName(m_sFunctionImpName);
        if (nIdentifier < 0)
        {
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_UNKNOWN_STATE_IN_COMPILER, pNode);
        }
        m_pcIdentifierList[nIdentifier].m_nBinarySourceFinish = m_nBinaryCodeLength;

        m_bFunctionImp = FALSE;
        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_GLOBAL_VARIABLES)
    {

        m_bGlobalVariableDefinition = FALSE;

        if (m_nCompileSymbolicOutput == 0)
        {
            m_pchOutputCode[m_nOutputCodeLength] = VM_SAVEBP;
            m_pchOutputCode[m_nOutputCodeLength + 1] = 0;
            m_nOutputCodeLength += 2;
        }
        else
        {
            PrintBinaryAddress();
            sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH, "SAVEBP\n");
            m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 7;
        }
        m_nBinaryCodeLength += 2;

        if (m_bCompileConditionalFile == TRUE)
        {
            if (m_nCompileSymbolicOutput == 0)
            {
                m_pchOutputCode[m_nOutputCodeLength] = VM_RSADD;
                m_pchOutputCode[m_nOutputCodeLength + 1] = VM_STACK_INTEGER;
                m_nOutputCodeLength += 2;
            }
            else
            {
                PrintBinaryAddress();
                sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH, "RSADDI\n");
                m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 7;
            }
            m_nBinaryCodeLength += 2;
        }

        if (m_nCompileSymbolicOutput == 0)
        {
            m_pchOutputCode[m_nOutputCodeLength] = VM_JSR;
            m_pchOutputCode[m_nOutputCodeLength + 1] = 0;

            CExoString sSymbolName;
            if (m_bCompileConditionalFile != FALSE)
            {
                sSymbolName.Format("FE_StartingConditional");
            }
            else
            {
                sSymbolName.Format("FE_main");
            }
            AddSymbolToQueryList(sSymbolName, m_nOutputCodeLength + 2);
            m_nOutputCodeLength += 6;
        }
        else
        {
            PrintBinaryAddress();
            if (m_bCompileConditionalFile != FALSE)
            {
                sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH, "JSR FE_StartingConditional\n");
                m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 27;
            }
            else
            {
                sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH, "JSR FE_main\n");
                m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 12;
            }
        }
        m_nBinaryCodeLength += 6;

        if (m_bCompileConditionalFile == TRUE)
        {
            int nStackElementsDown = -(m_nGlobalVariableSize + 12);
            int nSize = 4;
            if (m_nFunctionImpReturnType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT)
            {
                nSize = GetStructureSize(m_sFunctionImpReturnStructureName);
            }

            if (m_nCompileSymbolicOutput == 0)
            {
                m_pchOutputCode[m_nOutputCodeLength] = VM_CPDOWNSP;
                m_pchOutputCode[m_nOutputCodeLength + 1] = CSCRIPTCOMPILER_AUXCODE_TYPE_VOID;
                m_pchOutputCode[m_nOutputCodeLength + 2] = (char) ((nStackElementsDown >> 24) & 0xff);
                m_pchOutputCode[m_nOutputCodeLength + 3] = (char) ((nStackElementsDown >> 16) & 0xff);
                m_pchOutputCode[m_nOutputCodeLength + 4] = (char) ((nStackElementsDown >> 8) & 0xff);
                m_pchOutputCode[m_nOutputCodeLength + 5] = (char) (nStackElementsDown & 0xff);
                m_pchOutputCode[m_nOutputCodeLength + 6] = (char) ((nSize >> 8) & 0xff);
                m_pchOutputCode[m_nOutputCodeLength + 7] = (char) (nSize & 0xff);
                m_nOutputCodeLength += 8;
            }
            else
            {
                PrintBinaryAddress();
                sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                        "CPDOWNSP %08x,%04x\n", nStackElementsDown, nSize);
                m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 23;
            }
            m_nBinaryCodeLength += 8;

            int nStackModifier = -4;
            if (m_nCompileSymbolicOutput == 0)
            {
                m_pchOutputCode[m_nOutputCodeLength] = VM_MOVSP;
                m_pchOutputCode[m_nOutputCodeLength + 1] = 0;
                m_pchOutputCode[m_nOutputCodeLength + 2] = (char) ((nStackModifier >> 24) & 0xff);
                m_pchOutputCode[m_nOutputCodeLength + 3] = (char) ((nStackModifier >> 16) & 0xff);
                m_pchOutputCode[m_nOutputCodeLength + 4] = (char) ((nStackModifier >> 8) & 0xff);
                m_pchOutputCode[m_nOutputCodeLength + 5] = (char) (nStackModifier & 0xff);
                m_nOutputCodeLength += 6;
            }
            else
            {
                PrintBinaryAddress();
                sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                        "MOVSP %08x\n", nStackModifier);
                m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 15;
            }
            m_nBinaryCodeLength += 6;
        }

        if (m_nCompileSymbolicOutput == 0)
        {
            m_pchOutputCode[m_nOutputCodeLength] = VM_RESTOREBP;
            m_pchOutputCode[m_nOutputCodeLength + 1] = 0;
            m_nOutputCodeLength += 2;
        }
        else
        {
            PrintBinaryAddress();
            sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH, "RESTOREBP\n");
            m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 10;
        }
        m_nBinaryCodeLength += 2;

        int nStackPointer = -m_nGlobalVariableSize;
        if (nStackPointer != 0)
        {
            if (m_nCompileSymbolicOutput == 0)
            {
                m_pchOutputCode[m_nOutputCodeLength] = VM_MOVSP;
                m_pchOutputCode[m_nOutputCodeLength + 1] = 0;
                m_pchOutputCode[m_nOutputCodeLength + 2] = (char) ((nStackPointer >> 24) & 0xff);
                m_pchOutputCode[m_nOutputCodeLength + 3] = (char) ((nStackPointer >> 16) & 0xff);
                m_pchOutputCode[m_nOutputCodeLength + 4] = (char) ((nStackPointer >> 8) & 0xff);
                m_pchOutputCode[m_nOutputCodeLength + 5] = (char) (nStackPointer & 0xff);
                m_nOutputCodeLength += 6;
            }
            else
            {
                PrintBinaryAddress();
                sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                        "MOVSP %08x\n", nStackPointer);
                m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 15;
            }
            m_nBinaryCodeLength += 6;
        }

        if (m_nCompileSymbolicOutput == 0)
        {
            m_pchOutputCode[m_nOutputCodeLength] = VM_RETN;
            m_pchOutputCode[m_nOutputCodeLength + 1] = 0;
            m_nOutputCodeLength += 2;
        }
        else
        {
            PrintBinaryAddress();
            sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH, "RET\n");
            m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 4;
        }
        m_nBinaryCodeLength += 2;

        m_pcIdentifierList[m_nOccupiedIdentifiers].m_nBinarySourceFinish = m_nBinaryCodeLength;
        ++m_nOccupiedIdentifiers;
        if (m_nOccupiedIdentifiers >= CSCRIPTCOMPILER_MAX_IDENTIFIERS)
        {
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_IDENTIFIER_LIST_FULL, pNode);
        }
        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_FUNCTION_IDENTIFIER)
    {
        m_nVarStackVariableType = CSCRIPTCOMPILER_OPERATION_KEYWORD_DECLARATION;
        if (m_bFunctionImp == FALSE)
        {
            return 0;
        }

        if (pNode->pLeft == NULL)
        {
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_UNKNOWN_STATE_IN_COMPILER, pNode);
        }

        int nTokenType;
        CExoString sTokenStructureName = "";
        if (pNode->pLeft->nOperation == CSCRIPTCOMPILER_OPERATION_KEYWORD_STRUCT)
        {
            nTokenType = CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT;
            sTokenStructureName = pNode->pLeft->m_psStringData;
        }
        else if (pNode->pLeft->nOperation == CSCRIPTCOMPILER_OPERATION_KEYWORD_INT)
        {
            nTokenType = CSCRIPTCOMPILER_TOKEN_KEYWORD_INT;
        }
        else if (pNode->pLeft->nOperation == CSCRIPTCOMPILER_OPERATION_KEYWORD_FLOAT)
        {
            nTokenType = CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT;
        }
        else if (pNode->pLeft->nOperation == CSCRIPTCOMPILER_OPERATION_KEYWORD_STRING)
        {
            nTokenType = CSCRIPTCOMPILER_TOKEN_KEYWORD_STRING;
        }
        else if (pNode->pLeft->nOperation == CSCRIPTCOMPILER_OPERATION_KEYWORD_OBJECT)
        {
            nTokenType = CSCRIPTCOMPILER_TOKEN_KEYWORD_OBJECT;
        }
        else if (pNode->pLeft->nOperation == CSCRIPTCOMPILER_OPERATION_KEYWORD_VOID)
        {
            nTokenType = CSCRIPTCOMPILER_TOKEN_KEYWORD_VOID;
        }
        else if (pNode->pLeft->nOperation >= CSCRIPTCOMPILER_OPERATION_KEYWORD_ENGINE_STRUCTURE0 &&
                 pNode->pLeft->nOperation <= CSCRIPTCOMPILER_OPERATION_KEYWORD_ENGINE_STRUCTURE9)
        {
            nTokenType = CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE0 +
                         (pNode->pLeft->nOperation - CSCRIPTCOMPILER_OPERATION_KEYWORD_ENGINE_STRUCTURE0);
        }
        else
        {
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_UNKNOWN_STATE_IN_COMPILER, pNode);
        }

        if (nTokenType != CSCRIPTCOMPILER_TOKEN_KEYWORD_VOID)
        {
            ++m_nOccupiedVariables;
            if (m_bGlobalVariableDefinition != FALSE)
            {
                ++m_nGlobalVariables;
                if (nTokenType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT)
                {
                    m_nGlobalVariableSize += GetStructureSize(sTokenStructureName);
                }
                else
                {
                    m_nGlobalVariableSize += 4;
                }
            }

            m_pcVarStackList[m_nOccupiedVariables].m_psVarName = "#retval";
            m_pcVarStackList[m_nOccupiedVariables].m_nVarType = nTokenType;
            if (nTokenType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT)
            {
                m_pcVarStackList[m_nOccupiedVariables].m_sVarStructureName = sTokenStructureName;
            }
            m_pcVarStackList[m_nOccupiedVariables].m_nVarLevel = m_nVarStackRecursionLevel;
            m_pcVarStackList[m_nOccupiedVariables].m_nVarRunTimeLocation = m_nStackCurrentDepth * 4;

            AddVariableToStack(nTokenType, sTokenStructureName, FALSE);
        }

        m_nFunctionImpReturnType = nTokenType;
        m_sFunctionImpReturnStructureName = sTokenStructureName;
        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_FUNCTION_PARAM_NAME)
    {
        m_nVarStackVariableType = CSCRIPTCOMPILER_OPERATION_KEYWORD_DECLARATION;
        if (m_bFunctionImp == FALSE)
        {
            return 0;
        }

        if (pNode->pRight == NULL)
        {
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_UNKNOWN_STATE_IN_COMPILER, pNode);
        }

        for (nCount = m_nOccupiedVariables; nCount >= 0; --nCount)
        {
            if (m_pcVarStackList[nCount].m_psVarName == pNode->m_psStringData &&
                m_pcVarStackList[nCount].m_nVarLevel == m_nVarStackRecursionLevel)
            {
                return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_VARIABLE_ALREADY_USED_WITHIN_SCOPE, pNode);
            }
        }

        int nTokenType;
        CExoString sTokenStructureName = "";
        if (pNode->pRight->nOperation == CSCRIPTCOMPILER_OPERATION_KEYWORD_STRUCT)
        {
            nTokenType = CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT;
            sTokenStructureName = pNode->pRight->m_psStringData;
        }
        else if (pNode->pRight->nOperation == CSCRIPTCOMPILER_OPERATION_KEYWORD_INT)
        {
            nTokenType = CSCRIPTCOMPILER_TOKEN_KEYWORD_INT;
        }
        else if (pNode->pRight->nOperation == CSCRIPTCOMPILER_OPERATION_KEYWORD_FLOAT)
        {
            nTokenType = CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT;
        }
        else if (pNode->pRight->nOperation == CSCRIPTCOMPILER_OPERATION_KEYWORD_STRING)
        {
            nTokenType = CSCRIPTCOMPILER_TOKEN_KEYWORD_STRING;
        }
        else if (pNode->pRight->nOperation == CSCRIPTCOMPILER_OPERATION_KEYWORD_OBJECT)
        {
            nTokenType = CSCRIPTCOMPILER_TOKEN_KEYWORD_OBJECT;
        }
        else if (pNode->pRight->nOperation == CSCRIPTCOMPILER_OPERATION_KEYWORD_VOID)
        {
            nTokenType = CSCRIPTCOMPILER_TOKEN_KEYWORD_VOID;
        }
        else if (pNode->pRight->nOperation >= CSCRIPTCOMPILER_OPERATION_KEYWORD_ENGINE_STRUCTURE0 &&
                 pNode->pRight->nOperation <= CSCRIPTCOMPILER_OPERATION_KEYWORD_ENGINE_STRUCTURE9)
        {
            nTokenType = CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE0 +
                         (pNode->pRight->nOperation - CSCRIPTCOMPILER_OPERATION_KEYWORD_ENGINE_STRUCTURE0);
        }
        else
        {
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_UNKNOWN_STATE_IN_COMPILER, pNode);
        }

        if (nTokenType != CSCRIPTCOMPILER_TOKEN_KEYWORD_VOID)
        {
            ++m_nOccupiedVariables;
            if (m_bGlobalVariableDefinition != FALSE)
            {
                ++m_nGlobalVariables;
                if (nTokenType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT)
                {
                    m_nGlobalVariableSize += GetStructureSize(sTokenStructureName);
                }
                else
                {
                    m_nGlobalVariableSize += 4;
                }
            }

            m_pcVarStackList[m_nOccupiedVariables].m_psVarName = pNode->m_psStringData;
            m_pcVarStackList[m_nOccupiedVariables].m_nVarType = nTokenType;
            if (nTokenType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT)
            {
                m_pcVarStackList[m_nOccupiedVariables].m_sVarStructureName = sTokenStructureName;
            }
            m_pcVarStackList[m_nOccupiedVariables].m_nVarLevel = m_nVarStackRecursionLevel;
            m_pcVarStackList[m_nOccupiedVariables].m_nVarRunTimeLocation = m_nStackCurrentDepth * 4;

            AddVariableToStack(nTokenType, sTokenStructureName, FALSE);
        }
        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_FUNCTION_DECLARATION)
    {
        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_RETURN)
    {
        if (m_bFunctionImp == FALSE)
        {
            return 0;
        }

        if ((m_nFunctionImpReturnType == CSCRIPTCOMPILER_TOKEN_KEYWORD_VOID && pNode->pRight != NULL) ||
            (m_nFunctionImpReturnType != CSCRIPTCOMPILER_TOKEN_KEYWORD_VOID && pNode->pRight == NULL))
        {
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_RETURN_TYPE_AND_FUNCTION_TYPE_MISMATCHED, pNode);
        }

        if ((m_nFunctionImpReturnType == CSCRIPTCOMPILER_TOKEN_KEYWORD_INT && pNode->pRight->nType != CSCRIPTCOMPILER_TOKEN_KEYWORD_INT) ||
            (m_nFunctionImpReturnType == CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT && pNode->pRight->nType != CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT) ||
            (m_nFunctionImpReturnType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRING && pNode->pRight->nType != CSCRIPTCOMPILER_TOKEN_KEYWORD_STRING) ||
            (m_nFunctionImpReturnType == CSCRIPTCOMPILER_TOKEN_KEYWORD_OBJECT && pNode->pRight->nType != CSCRIPTCOMPILER_TOKEN_KEYWORD_OBJECT) ||
            (m_nFunctionImpReturnType == CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE0 && pNode->pRight->nType != CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE0) ||
            (m_nFunctionImpReturnType == CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE1 && pNode->pRight->nType != CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE1) ||
            (m_nFunctionImpReturnType == CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE2 && pNode->pRight->nType != CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE2) ||
            (m_nFunctionImpReturnType == CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE3 && pNode->pRight->nType != CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE3) ||
            (m_nFunctionImpReturnType == CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE4 && pNode->pRight->nType != CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE4) ||
            (m_nFunctionImpReturnType == CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE5 && pNode->pRight->nType != CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE5) ||
            (m_nFunctionImpReturnType == CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE6 && pNode->pRight->nType != CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE6) ||
            (m_nFunctionImpReturnType == CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE7 && pNode->pRight->nType != CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE7) ||
            (m_nFunctionImpReturnType == CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE8 && pNode->pRight->nType != CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE8) ||
            (m_nFunctionImpReturnType == CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE9 && pNode->pRight->nType != CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE9))
        {
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_RETURN_TYPE_AND_FUNCTION_TYPE_MISMATCHED, pNode);
        }

        if ((m_nFunctionImpReturnType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT && pNode->pRight->nType != CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT) ||
            (m_nFunctionImpReturnType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT && m_sFunctionImpReturnStructureName != pNode->pRight->m_psTypeName))
        {
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_RETURN_TYPE_AND_FUNCTION_TYPE_MISMATCHED, pNode);
        }

        if (m_nFunctionImpReturnType != CSCRIPTCOMPILER_TOKEN_KEYWORD_VOID)
        {
            int nStackElementsDown = m_nGlobalVariableSize - (m_nStackCurrentDepth * 4);
            int nSize = 4;
            if (m_nFunctionImpReturnType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT)
            {
                nSize = GetStructureSize(m_sFunctionImpReturnStructureName);
            }

            if (m_nCompileSymbolicOutput == 0)
            {
                m_pchOutputCode[m_nOutputCodeLength] = VM_CPDOWNSP;
                m_pchOutputCode[m_nOutputCodeLength + 1] = CSCRIPTCOMPILER_AUXCODE_TYPE_VOID;
                m_pchOutputCode[m_nOutputCodeLength + 2] = (char) ((nStackElementsDown >> 24) & 0xff);
                m_pchOutputCode[m_nOutputCodeLength + 3] = (char) ((nStackElementsDown >> 16) & 0xff);
                m_pchOutputCode[m_nOutputCodeLength + 4] = (char) ((nStackElementsDown >> 8) & 0xff);
                m_pchOutputCode[m_nOutputCodeLength + 5] = (char) (nStackElementsDown & 0xff);
                m_pchOutputCode[m_nOutputCodeLength + 6] = (char) ((nSize >> 8) & 0xff);
                m_pchOutputCode[m_nOutputCodeLength + 7] = (char) (nSize & 0xff);
                m_nOutputCodeLength += 8;
            }
            else
            {
                PrintBinaryAddress();
                sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                        "CPDOWNSP %08x,%04x\n", nStackElementsDown, nSize);
                m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 23;
            }
            m_nBinaryCodeLength += 8;
        }

        pNode->nIntegerData = (m_nFunctionImpAbortStackPointer - m_nStackCurrentDepth) * 4;
        if (pNode->nIntegerData != 0)
        {
            if (m_nCompileSymbolicOutput == 0)
            {
                m_pchOutputCode[m_nOutputCodeLength] = VM_MOVSP;
                m_pchOutputCode[m_nOutputCodeLength + 1] = 0;
                m_pchOutputCode[m_nOutputCodeLength + 2] = (char) ((pNode->nIntegerData >> 24) & 0xff);
                m_pchOutputCode[m_nOutputCodeLength + 3] = (char) ((pNode->nIntegerData >> 16) & 0xff);
                m_pchOutputCode[m_nOutputCodeLength + 4] = (char) ((pNode->nIntegerData >> 8) & 0xff);
                m_pchOutputCode[m_nOutputCodeLength + 5] = (char) (pNode->nIntegerData & 0xff);
                m_nOutputCodeLength += 6;
            }
            else
            {
                PrintBinaryAddress();
                sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                        "MOVSP %08x\n", pNode->nIntegerData);
                m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 15;
            }
            m_nBinaryCodeLength += 6;
        }

        if (m_nCompileSymbolicOutput == 0)
        {
            m_pchOutputCode[m_nOutputCodeLength] = VM_JMP;
            m_pchOutputCode[m_nOutputCodeLength + 1] = 0;

            CExoString sSymbolName;
            sSymbolName.Format("FX_%s", m_sFunctionImpName.CStr());
            AddSymbolToQueryList(sSymbolName, m_nOutputCodeLength + 2);
            m_nOutputCodeLength += 6;
        }
        else
        {
            PrintBinaryAddress();
            sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                    "JMP FX_%s\n", m_sFunctionImpName.CStr());
            m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + m_sFunctionImpName.GetLength() + 8;
        }
        m_nBinaryCodeLength += 6;
        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_PRE_INCREMENT ||
        pNode->nOperation == CSCRIPTCOMPILER_OPERATION_PRE_DECREMENT)
    {
        if (pNode->pLeft != NULL)
        {
            if (pNode->pLeft->nType != CSCRIPTCOMPILER_TOKEN_KEYWORD_INT)
            {
                return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_OPERAND_MUST_BE_AN_INTEGER_LVALUE, pNode);
            }
            if (pNode->pLeft->nOperation != CSCRIPTCOMPILER_OPERATION_VARIABLE &&
                pNode->pLeft->nOperation != CSCRIPTCOMPILER_OPERATION_STRUCTURE_PART)
            {
                return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_OPERAND_MUST_BE_AN_INTEGER_LVALUE, pNode->pLeft);
            }

            pNode->nType = pNode->pLeft->nType;

            int nStackElementsDown = pNode->pLeft->nIntegerData - (m_nStackCurrentDepth * 4);
            BOOL bOperateOnStackPointer;
            if (pNode->pLeft->nIntegerData < m_nGlobalVariableSize && m_bGlobalVariableDefinition == FALSE)
            {
                bOperateOnStackPointer = FALSE;
                nStackElementsDown = pNode->pLeft->nIntegerData - m_nGlobalVariableSize;
            }
            else
            {
                bOperateOnStackPointer = TRUE;
                nStackElementsDown += 4;
            }

            if (m_nCompileSymbolicOutput == 0)
            {
                if (bOperateOnStackPointer == FALSE)
                {
                    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_PRE_INCREMENT)
                    {
                        m_pchOutputCode[pNode->nIntegerData2] = VM_INCIBP;
                    }
                    else
                    {
                        m_pchOutputCode[pNode->nIntegerData2] = VM_DECIBP;
                    }
                }
                m_pchOutputCode[pNode->nIntegerData2 + 2] = (char) ((nStackElementsDown >> 24) & 0xff);
                m_pchOutputCode[pNode->nIntegerData2 + 3] = (char) ((nStackElementsDown >> 16) & 0xff);
                m_pchOutputCode[pNode->nIntegerData2 + 4] = (char) ((nStackElementsDown >> 8) & 0xff);
                m_pchOutputCode[pNode->nIntegerData2 + 5] = (char) (nStackElementsDown & 0xff);
            }
            else
            {

                if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_PRE_INCREMENT)
                {
                    sprintf(m_pchOutputCode + pNode->nIntegerData2, "INCI");
                }
                else
                {
                    sprintf(m_pchOutputCode + pNode->nIntegerData2, "DECI");
                }
                if (bOperateOnStackPointer == TRUE)
                {
                    sprintf(m_pchOutputCode + pNode->nIntegerData2 + 4, "SP ");
                }
                else
                {
                    sprintf(m_pchOutputCode + pNode->nIntegerData2 + 4, "BP ");
                }
                sprintf(m_pchOutputCode + pNode->nIntegerData2 + 7, "%08x\n", nStackElementsDown);
                m_pchOutputCode[pNode->nIntegerData2 + 16] = ' ';
            }
            return 0;
        }
        return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_UNKNOWN_STATE_IN_COMPILER, pNode);
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_POST_INCREMENT ||
        pNode->nOperation == CSCRIPTCOMPILER_OPERATION_POST_DECREMENT)
    {
        if (pNode->pLeft != NULL)
        {
            if (pNode->pLeft->nType != CSCRIPTCOMPILER_TOKEN_KEYWORD_INT)
            {
                return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_OPERAND_MUST_BE_AN_INTEGER_LVALUE, pNode);
            }
            if (pNode->pLeft->nOperation != CSCRIPTCOMPILER_OPERATION_VARIABLE &&
                pNode->pLeft->nOperation != CSCRIPTCOMPILER_OPERATION_STRUCTURE_PART)
            {
                return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_OPERAND_MUST_BE_AN_INTEGER_LVALUE, pNode->pLeft);
            }

            pNode->nType = pNode->pLeft->nType;

            int nStackElementsDown = pNode->pLeft->nIntegerData - (m_nStackCurrentDepth * 4);
            BOOL bOperateOnStackPointer = TRUE;
            if (pNode->pLeft->nIntegerData < m_nGlobalVariableSize && m_bGlobalVariableDefinition == FALSE)
            {
                bOperateOnStackPointer = FALSE;
                nStackElementsDown = pNode->pLeft->nIntegerData - m_nGlobalVariableSize;
            }

            if (m_nCompileSymbolicOutput == 0)
            {
                if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_POST_INCREMENT)
                {
                    if (bOperateOnStackPointer == TRUE)
                    {
                        m_pchOutputCode[m_nOutputCodeLength] = VM_INCISP;
                    }
                    else
                    {
                        m_pchOutputCode[m_nOutputCodeLength] = VM_INCIBP;
                    }
                }
                else
                {
                    if (bOperateOnStackPointer == TRUE)
                    {
                        m_pchOutputCode[m_nOutputCodeLength] = VM_DECISP;
                    }
                    else
                    {
                        m_pchOutputCode[m_nOutputCodeLength] = VM_DECIBP;
                    }
                }
                m_pchOutputCode[m_nOutputCodeLength + 1] = VM_STACK_INTEGER;
                m_pchOutputCode[m_nOutputCodeLength + 2] = (char) ((nStackElementsDown >> 24) & 0xff);
                m_pchOutputCode[m_nOutputCodeLength + 3] = (char) ((nStackElementsDown >> 16) & 0xff);
                m_pchOutputCode[m_nOutputCodeLength + 4] = (char) ((nStackElementsDown >> 8) & 0xff);
                m_pchOutputCode[m_nOutputCodeLength + 5] = (char) (nStackElementsDown & 0xff);
                m_nOutputCodeLength += 6;
            }
            else
            {
                PrintBinaryAddress();
                if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_POST_INCREMENT)
                {
                    sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH, "INCI");
                }
                else
                {
                    sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH, "DECI");
                }
                if (bOperateOnStackPointer == TRUE)
                {
                    sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 4, "SP ");
                }
                else
                {
                    sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 4, "BP ");
                }
                sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 7,
                        "%08x\n", nStackElementsDown);
                m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 16;
            }
            m_nBinaryCodeLength += 6;
            return 0;
        }
        return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_UNKNOWN_STATE_IN_COMPILER, pNode);
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_CASE)
    {
        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_DEFAULT)
    {
        if (m_nCompileSymbolicOutput == 0)
        {
            CExoString sSymbolName;
            sSymbolName.Format("_SC_DEFAULT_%08x", m_nSwitchIdentifier);
            AddSymbolToLabelList(sSymbolName, m_nOutputCodeLength);
        }
        else
        {
            sprintf(m_pchOutputCode + m_nOutputCodeLength, "_SC_DEFAULT_%08x:\n", m_nSwitchIdentifier);
            m_nOutputCodeLength += 22;
        }

        if (m_nSwitchStackDepth + 1 != m_nStackCurrentDepth)
        {
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_JUMPING_OVER_DECLARATION_STATEMENTS_DEFAULT_DISALLOWED, pNode);
        }
        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_BREAK)
    {

        int nIdentifierBreak;
        if (m_nSwitchIdentifier >= m_nLoopIdentifier)
        {
            nIdentifierBreak = m_nSwitchIdentifier;
        }
        else
        {
            nIdentifierBreak = m_nLoopIdentifier;
        }

        if (nIdentifierBreak == 0)
        {
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_BREAK_OUTSIDE_OF_LOOP_OR_CASE_STATEMENT, pNode);
        }

        if (m_nCompileSymbolicOutput == 0)
        {
            m_pchOutputCode[m_nOutputCodeLength] = VM_JMP;
            m_pchOutputCode[m_nOutputCodeLength + 1] = 0;

            CExoString sSymbolName;
            sSymbolName.Format("_BR_%08x", nIdentifierBreak);
            AddSymbolToQueryList(sSymbolName, m_nOutputCodeLength + 2);
            m_nOutputCodeLength += 6;
        }
        else
        {
            PrintBinaryAddress();
            sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                    "JMP _BR_%08x\n", nIdentifierBreak);
            m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 17;
        }
        m_nBinaryCodeLength += 6;
        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_CONTINUE)
    {
        if (m_nCompileSymbolicOutput == 0)
        {
            m_pchOutputCode[m_nOutputCodeLength] = VM_JMP;
            m_pchOutputCode[m_nOutputCodeLength + 1] = 0;

            CExoString sSymbolName;
            sSymbolName.Format("_CN_%08x", m_nLoopIdentifier);
            AddSymbolToQueryList(sSymbolName, m_nOutputCodeLength + 2);
            m_nOutputCodeLength += 6;
        }
        else
        {
            PrintBinaryAddress();
            sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                    "JMP _CN_%08x\n", m_nLoopIdentifier);
            m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 17;
        }
        m_nBinaryCodeLength += 6;
        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_WHILE_CONTINUE)
    {

        if (m_nCompileSymbolicOutput == 0)
        {
            CExoString sSymbolName;
            sSymbolName.Format("_CN_%08x", m_nLoopIdentifier);
            AddSymbolToLabelList(sSymbolName, m_nOutputCodeLength);
        }
        else
        {
            sprintf(m_pchOutputCode + m_nOutputCodeLength, "_CN_%08x:\n", m_nLoopIdentifier);
            m_nOutputCodeLength += 14;
        }
        return 0;
    }

    return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_UNKNOWN_OPERATION_IN_SEMANTIC_CHECK, pNode);
}

int CScriptCompilerInternal::InVisitGenerateCode(CScriptParseTreeNode* pNode)
{
    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_STRUCTURE_DEFINITION)
    {

        if (m_nStructureDefinition != 1)
        {
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_UNKNOWN_STATE_IN_COMPILER, pNode);
        }
        m_nStructureDefinition = 2;

        if (pNode->pLeft == NULL || pNode->pLeft->nOperation != CSCRIPTCOMPILER_OPERATION_KEYWORD_STRUCT)
        {
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_UNKNOWN_STATE_IN_COMPILER, pNode);
        }

        for (int nCount = 0; nCount < m_nMaxStructures; ++nCount)
        {
            if (m_pcStructList[nCount].m_psName == pNode->pLeft->m_psStringData)
            {
                return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_STRUCTURE_REDEFINED, pNode);
            }
        }

        m_pcStructList[m_nMaxStructures].m_psName = pNode->pLeft->m_psStringData;
        m_pcStructList[m_nMaxStructures].m_nByteSize = 0;
        m_pcStructList[m_nMaxStructures].m_nFieldStart = m_nMaxStructureFields;
        m_pcStructList[m_nMaxStructures].m_nFieldEnd = -1;
        m_nStructureDefinitionFieldStart = m_nMaxStructureFields;
        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_ASSIGNMENT)
    {

        if (pNode->pRight == NULL || pNode->pLeft == NULL ||
            (pNode->pRight->nOperation != CSCRIPTCOMPILER_OPERATION_STRUCTURE_PART &&
             pNode->pRight->nOperation != CSCRIPTCOMPILER_OPERATION_VARIABLE))
        {
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_INVALID_PARAMETERS_FOR_ASSIGNMENT, pNode);
        }
        m_bAssignmentToVariable = TRUE;
        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_STATEMENT)
    {

        if (pNode->pLeft == NULL ||
            (pNode->pLeft->nOperation != CSCRIPTCOMPILER_OPERATION_KEYWORD_DECLARATION &&
             (pNode->pLeft->nOperation != CSCRIPTCOMPILER_OPERATION_STATEMENT_LIST ||
              pNode->pLeft->pLeft == NULL ||
              pNode->pLeft->pLeft->nOperation != CSCRIPTCOMPILER_OPERATION_STATEMENT ||
              pNode->pLeft->pLeft->pLeft == NULL ||
              pNode->pLeft->pLeft->pLeft->nOperation != CSCRIPTCOMPILER_OPERATION_KEYWORD_DECLARATION)))
        {
            pNode->nIntegerData = (pNode->m_nStackPointer - m_nStackCurrentDepth) * 4;
            m_nStackCurrentDepth = pNode->m_nStackPointer;

            if (pNode->nIntegerData != 0)
            {
                if (m_nCompileSymbolicOutput == 0)
                {
                    m_pchOutputCode[m_nOutputCodeLength] = VM_MOVSP;
                    m_pchOutputCode[m_nOutputCodeLength + 1] = 0;
                    m_pchOutputCode[m_nOutputCodeLength + 2] = (char) ((pNode->nIntegerData >> 24) & 0xff);
                    m_pchOutputCode[m_nOutputCodeLength + 3] = (char) ((pNode->nIntegerData >> 16) & 0xff);
                    m_pchOutputCode[m_nOutputCodeLength + 4] = (char) ((pNode->nIntegerData >> 8) & 0xff);
                    m_pchOutputCode[m_nOutputCodeLength + 5] = (char) (pNode->nIntegerData & 0xff);
                    m_nOutputCodeLength += 6;
                }
                else
                {
                    PrintBinaryAddress();
                    sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                            "MOVSP %08x\n", pNode->nIntegerData);
                    m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 15;
                }
                m_nBinaryCodeLength += 6;
            }
        }
        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_SWITCH_BLOCK)
    {
        return GenerateCodeForSwitchLabels(pNode);
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_WHILE_BLOCK)
    {

        pNode->nIntegerData2 = m_nOutputCodeLength;

        if (m_pchStackTypes[m_nStackCurrentDepth - 1] != VM_STACK_INTEGER)
        {
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_INTEGER_NOT_AT_TOP_OF_STACK, pNode);
        }
        --m_nStackCurrentDepth;

        if (m_nCompileSymbolicOutput == 0)
        {
            m_pchOutputCode[m_nOutputCodeLength] = VM_JZ;
            m_pchOutputCode[m_nOutputCodeLength + 1] = 0;
            m_nOutputCodeLength += 6;
        }
        else
        {
            PrintBinaryAddress();
            sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                    "JZ _W2_%08x\n", pNode->nIntegerData);
            m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 16;
        }
        m_nBinaryCodeLength += 6;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_IF_CHOICE)
    {

        pNode->nIntegerData2 = m_nOutputCodeLength;

        if (m_nCompileSymbolicOutput == 0)
        {
            m_pchOutputCode[m_nOutputCodeLength] = VM_JMP;
            m_pchOutputCode[m_nOutputCodeLength + 1] = 0;
            m_nOutputCodeLength += 6;

            int nJmpLength = m_nOutputCodeLength - pNode->nIntegerData;
            m_pchOutputCode[pNode->nIntegerData + 2] = (char) ((nJmpLength >> 24) & 0xff);
            m_pchOutputCode[pNode->nIntegerData + 3] = (char) ((nJmpLength >> 16) & 0xff);
            m_pchOutputCode[pNode->nIntegerData + 4] = (char) ((nJmpLength >> 8) & 0xff);
            m_pchOutputCode[pNode->nIntegerData + 5] = (char) (nJmpLength & 0xff);
        }
        else
        {
            PrintBinaryAddress();
            sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                    "JMP _I2_%08x\n", pNode->nIntegerData);
            m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 17;
            sprintf(m_pchOutputCode + m_nOutputCodeLength, "_I1_%08x:\n", pNode->nIntegerData);
            m_nOutputCodeLength += 14;
        }
        m_nBinaryCodeLength += 6;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_COND_CHOICE)
    {

        pNode->nIntegerData2 = m_nOutputCodeLength;

        if (m_nCompileSymbolicOutput == 0)
        {
            m_pchOutputCode[m_nOutputCodeLength] = VM_JMP;
            m_pchOutputCode[m_nOutputCodeLength + 1] = 0;
            m_nOutputCodeLength += 6;

            int nJmpLength = m_nOutputCodeLength - pNode->nIntegerData;
            m_pchOutputCode[pNode->nIntegerData + 2] = (char) ((nJmpLength >> 24) & 0xff);
            m_pchOutputCode[pNode->nIntegerData + 3] = (char) ((nJmpLength >> 16) & 0xff);
            m_pchOutputCode[pNode->nIntegerData + 4] = (char) ((nJmpLength >> 8) & 0xff);
            m_pchOutputCode[pNode->nIntegerData + 5] = (char) (nJmpLength & 0xff);
        }
        else
        {
            PrintBinaryAddress();
            sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                    "JMP _CH2_%08x\n", pNode->nIntegerData);
            m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 18;
            sprintf(m_pchOutputCode + m_nOutputCodeLength, "_CH1_%08x:\n", pNode->nIntegerData);
            m_nOutputCodeLength += 15;
        }
        m_nBinaryCodeLength += 6;

        int nSize = 4;
        if (pNode->nType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT)
        {
            nSize = GetStructureSize(pNode->m_psTypeName);
        }
        m_nStackCurrentDepth -= nSize >> 2;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_FUNCTION && m_bFunctionImp == TRUE)
    {

        pNode->m_nStackPointer = m_nStackCurrentDepth;
        m_nFunctionImpAbortStackPointer = m_nStackCurrentDepth;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_LOGICAL_AND && pNode->pLeft != NULL)
    {

        if (m_nCompileSymbolicOutput == 0)
        {
            m_pchOutputCode[m_nOutputCodeLength] = VM_CPTOPSP;
            m_pchOutputCode[m_nOutputCodeLength + 1] = CSCRIPTCOMPILER_AUXCODE_TYPE_VOID;
            m_pchOutputCode[m_nOutputCodeLength + 2] = (char) 0xff;
            m_pchOutputCode[m_nOutputCodeLength + 3] = (char) 0xff;
            m_pchOutputCode[m_nOutputCodeLength + 4] = (char) 0xff;
            m_pchOutputCode[m_nOutputCodeLength + 5] = (char) 0xfc;
            m_pchOutputCode[m_nOutputCodeLength + 6] = 0;
            m_pchOutputCode[m_nOutputCodeLength + 7] = 4;
            m_nOutputCodeLength += 8;
        }
        else
        {
            PrintBinaryAddress();
            sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                    "CPTOPSP %08x,%04x\n", -4, 4);
            m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 22;
        }
        m_nBinaryCodeLength += 8;
        AddVariableToStack(CSCRIPTCOMPILER_TOKEN_KEYWORD_INT, "", FALSE);

        pNode->nIntegerData = m_nOutputCodeLength;

        if (m_pchStackTypes[m_nStackCurrentDepth - 1] != VM_STACK_INTEGER)
        {
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_INTEGER_NOT_AT_TOP_OF_STACK, pNode);
        }
        --m_nStackCurrentDepth;

        if (m_nCompileSymbolicOutput == 0)
        {
            m_pchOutputCode[m_nOutputCodeLength] = VM_JZ;
            m_pchOutputCode[m_nOutputCodeLength + 1] = 0;
            m_nOutputCodeLength += 6;
        }
        else
        {
            PrintBinaryAddress();
            sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                    "JZ _ILA_%08x\n", pNode->nIntegerData);
            m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 17;
        }
        m_nBinaryCodeLength += 6;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_LOGICAL_OR && pNode->pLeft != NULL)
    {

        if (m_nCompileSymbolicOutput == 0)
        {
            m_pchOutputCode[m_nOutputCodeLength] = VM_CPTOPSP;
            m_pchOutputCode[m_nOutputCodeLength + 1] = CSCRIPTCOMPILER_AUXCODE_TYPE_VOID;
            m_pchOutputCode[m_nOutputCodeLength + 2] = (char) 0xff;
            m_pchOutputCode[m_nOutputCodeLength + 3] = (char) 0xff;
            m_pchOutputCode[m_nOutputCodeLength + 4] = (char) 0xff;
            m_pchOutputCode[m_nOutputCodeLength + 5] = (char) 0xfc;
            m_pchOutputCode[m_nOutputCodeLength + 6] = 0;
            m_pchOutputCode[m_nOutputCodeLength + 7] = 4;
            m_nOutputCodeLength += 8;
        }
        else
        {
            PrintBinaryAddress();
            sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                    "CPTOPSP %08x,%04x\n", -4, 4);
            m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 22;
        }
        m_nBinaryCodeLength += 8;
        AddVariableToStack(CSCRIPTCOMPILER_TOKEN_KEYWORD_INT, "", FALSE);

        if (m_pchStackTypes[m_nStackCurrentDepth - 1] != VM_STACK_INTEGER)
        {
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_INTEGER_NOT_AT_TOP_OF_STACK, pNode);
        }
        --m_nStackCurrentDepth;

        if (m_nCompileSymbolicOutput == 0)
        {
            m_pchOutputCode[m_nOutputCodeLength] = VM_JZ;
            m_pchOutputCode[m_nOutputCodeLength + 1] = 0;
            m_pchOutputCode[m_nOutputCodeLength + 2] = 0;
            m_pchOutputCode[m_nOutputCodeLength + 3] = 0;
            m_pchOutputCode[m_nOutputCodeLength + 4] = 0;
            m_pchOutputCode[m_nOutputCodeLength + 5] = 20;
            m_nOutputCodeLength += 6;
        }
        else
        {
            PrintBinaryAddress();
            sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                    "JZ _ILO1_%08x\n", m_nOutputCodeLength + 66);
            m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 18;
        }
        m_nBinaryCodeLength += 6;

        if (m_nCompileSymbolicOutput == 0)
        {
            m_pchOutputCode[m_nOutputCodeLength] = VM_CPTOPSP;
            m_pchOutputCode[m_nOutputCodeLength + 1] = CSCRIPTCOMPILER_AUXCODE_TYPE_VOID;
            m_pchOutputCode[m_nOutputCodeLength + 2] = (char) 0xff;
            m_pchOutputCode[m_nOutputCodeLength + 3] = (char) 0xff;
            m_pchOutputCode[m_nOutputCodeLength + 4] = (char) 0xff;
            m_pchOutputCode[m_nOutputCodeLength + 5] = (char) 0xfc;
            m_pchOutputCode[m_nOutputCodeLength + 6] = 0;
            m_pchOutputCode[m_nOutputCodeLength + 7] = 4;
            m_nOutputCodeLength += 8;
        }
        else
        {
            PrintBinaryAddress();
            sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                    "CPTOPSP %08x,%04x\n", -4, 4);
            m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 22;
        }
        m_nBinaryCodeLength += 8;

        pNode->nIntegerData = m_nOutputCodeLength;

        if (m_nCompileSymbolicOutput == 0)
        {
            m_pchOutputCode[m_nOutputCodeLength] = VM_JZ;
            m_pchOutputCode[m_nOutputCodeLength + 1] = 0;
            m_nOutputCodeLength += 6;
        }
        else
        {
            PrintBinaryAddress();
            sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                    "JMP _ILO2_%08x\n", pNode->nIntegerData);
            m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 19;
            sprintf(m_pchOutputCode + m_nOutputCodeLength, "_ILO1_%08x:\n", pNode->nIntegerData);
            m_nOutputCodeLength += 16;
        }
        m_nBinaryCodeLength += 6;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_STRUCTURE_PART)
    {
        m_bInStructurePart = TRUE;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_DOWHILE_BLOCK)
    {

        if (m_nCompileSymbolicOutput == 0)
        {
            CExoString sSymbolName;
            sSymbolName.Format("_CN_%08x", m_nLoopIdentifier);
            AddSymbolToLabelList(sSymbolName, m_nOutputCodeLength);
        }
        else
        {
            sprintf(m_pchOutputCode + m_nOutputCodeLength, "_CN_%08x:\n", m_nLoopIdentifier);
            m_nOutputCodeLength += 14;
        }
    }

    return 0;
}

int CScriptCompilerInternal::PreVisitGenerateCode(CScriptParseTreeNode* pNode)
{
    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_COMPOUND_STATEMENT ||
        pNode->nOperation == CSCRIPTCOMPILER_OPERATION_STATEMENT ||
        pNode->nOperation == CSCRIPTCOMPILER_OPERATION_NON_VOID_EXPRESSION ||
        pNode->nOperation == CSCRIPTCOMPILER_OPERATION_INTEGER_EXPRESSION)
    {
        if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_COMPOUND_STATEMENT)
        {
            ++m_nVarStackRecursionLevel;
        }

        pNode->m_nStackPointer = m_nStackCurrentDepth;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_FUNCTION)
    {
        m_bFunctionImp = FALSE;
        if (pNode->pRight != NULL)
        {
            m_bFunctionImp = TRUE;
        }

        if (m_bFunctionImp == TRUE)
        {
            ++m_nVarStackRecursionLevel;

            m_sFunctionImpName = pNode->pLeft->pLeft->m_psStringData;

            int nIdentifier = GetIdentifierByName(m_sFunctionImpName);
            if (nIdentifier < 0)
            {
                return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_UNKNOWN_STATE_IN_COMPILER, pNode);
            }

            m_pcIdentifierList[nIdentifier].m_nBinarySourceStart = m_nBinaryCodeLength;
            m_pcIdentifierList[nIdentifier].m_nBinarySourceFinish = -1;
            m_pcIdentifierList[nIdentifier].m_nBinaryDestinationStart = -1;
            m_pcIdentifierList[nIdentifier].m_nBinaryDestinationFinish = -1;

            if (m_nCompileSymbolicOutput == 0)
            {
                CExoString sSymbolName;
                sSymbolName.Format("FE_%s", m_sFunctionImpName.CStr());
                AddSymbolToLabelList(sSymbolName, m_nOutputCodeLength);
            }
            else
            {
                sprintf(m_pchOutputCode + m_nOutputCodeLength, "FE_%s:\n", m_sFunctionImpName.CStr());
                m_nOutputCodeLength += m_sFunctionImpName.GetLength() + 5;
            }
        }
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_GLOBAL_VARIABLES)
    {
        m_bFunctionImp = FALSE;
        if (pNode->pRight != NULL)
        {
            m_bFunctionImp = TRUE;
        }

        if (m_bFunctionImp == TRUE)
        {
            ++m_nVarStackRecursionLevel;

            if (m_bGlobalVariableDefinition != FALSE)
            {
                return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_UNKNOWN_STATE_IN_COMPILER, pNode);
            }
            m_bGlobalVariableDefinition = TRUE;

            m_pcIdentifierList[m_nOccupiedIdentifiers].m_psIdentifier = "#globals";
            m_pcIdentifierList[m_nOccupiedIdentifiers].m_nIdentifierLength = 8;
            m_pcIdentifierList[m_nOccupiedIdentifiers].m_nBinarySourceStart = m_nBinaryCodeLength;
            m_pcIdentifierList[m_nOccupiedIdentifiers].m_nBinaryDestinationStart = -1;
            m_pcIdentifierList[m_nOccupiedIdentifiers].m_nBinaryDestinationFinish = -1;

            if (m_nCompileSymbolicOutput == 0)
            {
                CExoString sSymbolName;
                sSymbolName.Format("FE_#globals");
                AddSymbolToLabelList(sSymbolName, m_nOutputCodeLength);
            }
            else
            {
                sprintf(m_pchOutputCode + m_nOutputCodeLength, "FE_#globals:\n");
                m_nOutputCodeLength += 13;
            }
        }
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_CASE)
    {

        int nCaseValue;
        if (pNode->pLeft == NULL)
        {
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_CASE_PARAMETER_NOT_A_CONSTANT_INTEGER, pNode);
        }

        if (pNode->pLeft->nOperation == CSCRIPTCOMPILER_OPERATION_NEGATION)
        {
            if (pNode->pLeft->pLeft == NULL ||
                pNode->pLeft->pLeft->nOperation != CSCRIPTCOMPILER_OPERATION_CONSTANT_INTEGER)
            {
                return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_CASE_PARAMETER_NOT_A_CONSTANT_INTEGER, pNode);
            }
            nCaseValue = -pNode->pLeft->pLeft->nIntegerData;
        }
        else if (pNode->pLeft->nOperation == CSCRIPTCOMPILER_OPERATION_CONSTANT_INTEGER)
        {
            nCaseValue = pNode->pLeft->nIntegerData;
        }
        else
        {
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_CASE_PARAMETER_NOT_A_CONSTANT_INTEGER, pNode);
        }

        if (m_nCompileSymbolicOutput == 0)
        {
            CExoString sSymbolName;
            sSymbolName.Format("_SC_%08x_%08x", nCaseValue, m_nSwitchIdentifier);
            AddSymbolToLabelList(sSymbolName, m_nOutputCodeLength);
        }
        else
        {
            sprintf(m_pchOutputCode + m_nOutputCodeLength, "_SC_%08x_%08x:\n", nCaseValue, m_nSwitchIdentifier);
            m_nOutputCodeLength += 23;
        }

        if (m_nSwitchStackDepth + 1 != m_nStackCurrentDepth)
        {
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_JUMPING_OVER_DECLARATION_STATEMENTS_CASE_DISALLOWED, pNode);
        }

        return 1;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_STRUCTURE_DEFINITION)
    {
        if (m_nStructureDefinition != 0)
        {
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_UNKNOWN_STATE_IN_COMPILER, pNode);
        }
        m_nStructureDefinition = 1;
        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_ACTION_PARAMETER)
    {

        if (m_nCompileSymbolicOutput == 0)
        {
            m_pchOutputCode[m_nOutputCodeLength] = VM_STORE_STATE;
            m_pchOutputCode[m_nOutputCodeLength + 1] = CSCRIPTCOMPILER_STORE_STATE_RESUME_OFFSET;
            m_pchOutputCode[m_nOutputCodeLength + 2] = (char) ((m_nGlobalVariableSize >> 24) & 0xff);
            m_pchOutputCode[m_nOutputCodeLength + 3] = (char) ((m_nGlobalVariableSize >> 16) & 0xff);
            m_pchOutputCode[m_nOutputCodeLength + 4] = (char) ((m_nGlobalVariableSize >> 8) & 0xff);
            m_pchOutputCode[m_nOutputCodeLength + 5] = (char) (m_nGlobalVariableSize & 0xff);

            int nStackSize = m_nStackCurrentDepth * 4 - m_nGlobalVariableSize;
            m_pchOutputCode[m_nOutputCodeLength + 6] = (char) ((nStackSize >> 24) & 0xff);
            m_pchOutputCode[m_nOutputCodeLength + 7] = (char) ((nStackSize >> 16) & 0xff);
            m_pchOutputCode[m_nOutputCodeLength + 8] = (char) ((nStackSize >> 8) & 0xff);
            m_pchOutputCode[m_nOutputCodeLength + 9] = (char) (nStackSize & 0xff);
            m_nOutputCodeLength += 10;
        }
        else
        {
            PrintBinaryAddress();
            sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                    "STORE_STATE %08x %08x %08x\n", CSCRIPTCOMPILER_STORE_STATE_RESUME_OFFSET,
                    m_nGlobalVariableSize, m_nStackCurrentDepth * 4 - m_nGlobalVariableSize);
            m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 39;
        }
        m_nBinaryCodeLength += 10;

        if (m_nCompileSymbolicOutput == 0)
        {
            m_pchOutputCode[m_nOutputCodeLength] = VM_JMP;
            m_pchOutputCode[m_nOutputCodeLength + 1] = 0;
            pNode->nIntegerData = m_nOutputCodeLength;
            m_nOutputCodeLength += 6;
        }
        else
        {
            PrintBinaryAddress();
            sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                    "JMP _X%08x\n", m_nOutputCodeLength);
            pNode->nIntegerData = m_nOutputCodeLength;
            m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 15;
        }
        m_nBinaryCodeLength += 6;
        return 0;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_ACTION)
    {
        if (pNode->pRight != NULL)
        {
            for (int nCount = 0; nCount < m_nOccupiedIdentifiers; ++nCount)
            {
                if (m_pcIdentifierList[nCount].m_psIdentifier == pNode->pRight->m_psStringData)
                {
                    pNode->pRight->nIntegerData = nCount;

                    if (nCount >= m_nMaxPredefinedIdentifierId)
                    {

                        pNode->nIntegerData = 1;

                        int nReturnType;
                        CExoString sStructureName = "";
                        if (m_pcIdentifierList[nCount].m_nReturnType != CSCRIPTCOMPILER_TOKEN_VOID_IDENTIFIER)
                        {
                            if (m_pcIdentifierList[nCount].m_nReturnType == CSCRIPTCOMPILER_TOKEN_INTEGER_IDENTIFIER)
                            {
                                nReturnType = CSCRIPTCOMPILER_TOKEN_KEYWORD_INT;
                            }
                            else if (m_pcIdentifierList[nCount].m_nReturnType == CSCRIPTCOMPILER_TOKEN_FLOAT_IDENTIFIER)
                            {
                                nReturnType = CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT;
                            }
                            else if (m_pcIdentifierList[nCount].m_nReturnType == CSCRIPTCOMPILER_TOKEN_STRING_IDENTIFIER)
                            {
                                nReturnType = CSCRIPTCOMPILER_TOKEN_KEYWORD_STRING;
                            }
                            else if (m_pcIdentifierList[nCount].m_nReturnType == CSCRIPTCOMPILER_TOKEN_OBJECT_IDENTIFIER)
                            {
                                nReturnType = CSCRIPTCOMPILER_TOKEN_KEYWORD_OBJECT;
                            }
                            else if (m_pcIdentifierList[nCount].m_nReturnType == CSCRIPTCOMPILER_TOKEN_STRUCTURE_IDENTIFIER)
                            {
                                nReturnType = CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT;
                                sStructureName = m_pcIdentifierList[nCount].m_psStructureReturnName;
                            }
                            else if (m_pcIdentifierList[nCount].m_nReturnType >= CSCRIPTCOMPILER_TOKEN_ENGINE_STRUCTURE0_IDENTIFIER &&
                                     m_pcIdentifierList[nCount].m_nReturnType <= CSCRIPTCOMPILER_TOKEN_ENGINE_STRUCTURE9_IDENTIFIER)
                            {
                                nReturnType = CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE0 +
                                              (m_pcIdentifierList[nCount].m_nReturnType - CSCRIPTCOMPILER_TOKEN_ENGINE_STRUCTURE0_IDENTIFIER);
                            }
                            else
                            {
                                return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_UNKNOWN_STATE_IN_COMPILER, pNode);
                            }

                            AddVariableToStack(nReturnType, sStructureName, TRUE);
                        }
                    }
                    else
                    {
                        pNode->nIntegerData = 0;
                    }

                    int nParameters = 0;
                    for (CScriptParseTreeNode* pTracePtr = pNode->pLeft; pTracePtr != NULL; pTracePtr = pTracePtr->pLeft)
                    {
                        ++nParameters;
                    }

                    int nIdentifier = pNode->pRight->nIntegerData;
                    if (m_pcIdentifierList[nIdentifier].m_nNonOptionalParameters > nParameters ||
                        m_pcIdentifierList[nIdentifier].m_nParameters < nParameters)
                    {
                        return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_DECLARATION_DOES_NOT_MATCH_PARAMETERS, pNode);
                    }

                    if (nParameters != m_pcIdentifierList[nIdentifier].m_nParameters)
                    {
                        for (int nCount2 = m_pcIdentifierList[nIdentifier].m_nParameters - 1; nCount2 >= nParameters; --nCount2)
                        {
                            if (m_pcIdentifierList[nIdentifier].m_pbOptionalParameters[nCount2] != TRUE)
                            {
                                return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_DECLARATION_DOES_NOT_MATCH_PARAMETERS, pNode);
                            }

                            if (m_pcIdentifierList[nIdentifier].m_pchParameters[nCount2] != CSCRIPTCOMPILER_TOKEN_KEYWORD_INT &&
                                m_pcIdentifierList[nIdentifier].m_pchParameters[nCount2] != CSCRIPTCOMPILER_TOKEN_KEYWORD_OBJECT &&
                                m_pcIdentifierList[nIdentifier].m_pchParameters[nCount2] != CSCRIPTCOMPILER_TOKEN_KEYWORD_STRING &&
                                m_pcIdentifierList[nIdentifier].m_pchParameters[nCount2] != CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT &&
                                (m_pcIdentifierList[nIdentifier].m_pchParameters[nCount2] != CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT ||
                                 m_pcIdentifierList[nIdentifier].m_psStructureParameterNames[nCount2] != "vector"))
                            {
                                return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_TYPE_DOES_NOT_HAVE_AN_OPTIONAL_PARAMETER, pNode);
                            }

                            if (m_pcIdentifierList[nIdentifier].m_pchParameters[nCount2] == CSCRIPTCOMPILER_TOKEN_KEYWORD_INT)
                            {
                                int nIntegerData = m_pcIdentifierList[nIdentifier].m_pnOptionalParameterIntegerData[nCount2];

                                if (m_nCompileSymbolicOutput == 0)
                                {
                                    m_pchOutputCode[m_nOutputCodeLength] = VM_CONST;
                                    m_pchOutputCode[m_nOutputCodeLength + 1] = VM_STACK_INTEGER;
                                    m_pchOutputCode[m_nOutputCodeLength + 2] = (char) ((nIntegerData >> 24) & 0xff);
                                    m_pchOutputCode[m_nOutputCodeLength + 3] = (char) ((nIntegerData >> 16) & 0xff);
                                    m_pchOutputCode[m_nOutputCodeLength + 4] = (char) ((nIntegerData >> 8) & 0xff);
                                    m_pchOutputCode[m_nOutputCodeLength + 5] = (char) (nIntegerData & 0xff);
                                    m_nOutputCodeLength += 6;
                                }
                                else
                                {
                                    PrintBinaryAddress();
                                    sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                                            "CONSTI %08x\n", nIntegerData);
                                    m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 16;
                                }
                                m_nBinaryCodeLength += 6;

                                m_pchStackTypes[m_nStackCurrentDepth] = VM_STACK_INTEGER;
                                ++m_nStackCurrentDepth;
                            }

                            if (m_pcIdentifierList[nIdentifier].m_pchParameters[nCount2] == CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT)
                            {
                                float fFloatData = m_pcIdentifierList[nIdentifier].m_pfOptionalParameterFloatData[nCount2];

                                if (m_nCompileSymbolicOutput == 0)
                                {
                                    int* pFloatAsInt = (int*) &fFloatData;
                                    m_pchOutputCode[m_nOutputCodeLength] = VM_CONST;
                                    m_pchOutputCode[m_nOutputCodeLength + 1] = VM_STACK_FLOAT;
                                    m_pchOutputCode[m_nOutputCodeLength + 2] = (char) ((*pFloatAsInt >> 24) & 0xff);
                                    m_pchOutputCode[m_nOutputCodeLength + 3] = (char) ((*pFloatAsInt >> 16) & 0xff);
                                    m_pchOutputCode[m_nOutputCodeLength + 4] = (char) ((*pFloatAsInt >> 8) & 0xff);
                                    m_pchOutputCode[m_nOutputCodeLength + 5] = (char) (*pFloatAsInt & 0xff);
                                    m_nOutputCodeLength += 6;
                                }
                                else
                                {
                                    PrintBinaryAddress();
                                    sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                                            "CONSTF %19.9f\n", fFloatData);
                                    m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 27;
                                }
                                m_nBinaryCodeLength += 6;

                                m_pchStackTypes[m_nStackCurrentDepth] = VM_STACK_FLOAT;
                                ++m_nStackCurrentDepth;
                            }

                            if (m_pcIdentifierList[nIdentifier].m_pchParameters[nCount2] == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT)
                            {
                                if (m_pcIdentifierList[nIdentifier].m_psStructureParameterNames[nCount2] == "vector")
                                {

                                    for (int nCount3 = 0; nCount3 < 3; ++nCount3)
                                    {
                                        float fFloatData = m_pcIdentifierList[nIdentifier].m_pfOptionalParameterVectorData[nCount2 * 3 + nCount3];

                                        if (m_nCompileSymbolicOutput == 0)
                                        {
                                            int* pFloatAsInt = (int*) &fFloatData;
                                            m_pchOutputCode[m_nOutputCodeLength] = VM_CONST;
                                            m_pchOutputCode[m_nOutputCodeLength + 1] = VM_STACK_FLOAT;
                                            m_pchOutputCode[m_nOutputCodeLength + 2] = (char) ((*pFloatAsInt >> 24) & 0xff);
                                            m_pchOutputCode[m_nOutputCodeLength + 3] = (char) ((*pFloatAsInt >> 16) & 0xff);
                                            m_pchOutputCode[m_nOutputCodeLength + 4] = (char) ((*pFloatAsInt >> 8) & 0xff);
                                            m_pchOutputCode[m_nOutputCodeLength + 5] = (char) (*pFloatAsInt & 0xff);
                                            m_nOutputCodeLength += 6;
                                        }
                                        else
                                        {
                                            PrintBinaryAddress();
                                            sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                                                    "CONSTF %19.9f\n", fFloatData);
                                            m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 27;
                                        }
                                        m_nBinaryCodeLength += 6;

                                        m_pchStackTypes[m_nStackCurrentDepth] = VM_STACK_FLOAT;
                                        ++m_nStackCurrentDepth;
                                    }
                                }
                            }

                            if (m_pcIdentifierList[nIdentifier].m_pchParameters[nCount2] == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRING)
                            {
                                CExoString sStringData = m_pcIdentifierList[nIdentifier].m_psOptionalParameterStringData[nCount2];
                                int nLength = sStringData.GetLength();

                                if (m_nCompileSymbolicOutput == 0)
                                {
                                    m_pchOutputCode[m_nOutputCodeLength] = VM_CONST;
                                    m_pchOutputCode[m_nOutputCodeLength + 1] = VM_STACK_STRING;
                                    m_pchOutputCode[m_nOutputCodeLength + 2] = (char) ((nLength >> 8) & 0xff);
                                    m_pchOutputCode[m_nOutputCodeLength + 3] = (char) (nLength & 0xff);
                                    for (int nCount3 = 0; nCount3 < nLength; ++nCount3)
                                    {
                                        m_pchOutputCode[m_nOutputCodeLength + 4 + nCount3] = (sStringData.CStr())[nCount3];
                                    }
                                    m_nOutputCodeLength += nLength + 4;
                                }
                                else
                                {
                                    PrintBinaryAddress();
                                    sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                                            "CONSTS %04x %s\n", nLength, sStringData.CStr());
                                    m_nOutputCodeLength += nLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 13;
                                }
                                m_nBinaryCodeLength += nLength + 4;

                                m_pchStackTypes[m_nStackCurrentDepth] = VM_STACK_STRING;
                                ++m_nStackCurrentDepth;
                            }

                            if (m_pcIdentifierList[nIdentifier].m_pchParameters[nCount2] == CSCRIPTCOMPILER_TOKEN_KEYWORD_OBJECT)
                            {
                                unsigned long oidObjectData = m_pcIdentifierList[nIdentifier].m_poidOptionalParameterObjectData[nCount2];

                                if (m_nCompileSymbolicOutput == 0)
                                {
                                    m_pchOutputCode[m_nOutputCodeLength] = VM_CONST;
                                    m_pchOutputCode[m_nOutputCodeLength + 1] = VM_STACK_OBJECT;
                                    m_pchOutputCode[m_nOutputCodeLength + 2] = (char) ((oidObjectData >> 24) & 0xff);
                                    m_pchOutputCode[m_nOutputCodeLength + 3] = (char) ((oidObjectData >> 16) & 0xff);
                                    m_pchOutputCode[m_nOutputCodeLength + 4] = (char) ((oidObjectData >> 8) & 0xff);
                                    m_pchOutputCode[m_nOutputCodeLength + 5] = (char) (oidObjectData & 0xff);
                                    m_nOutputCodeLength += 6;
                                }
                                else
                                {
                                    PrintBinaryAddress();
                                    sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                                            "CONSTO %08x\n", oidObjectData);
                                    m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 16;
                                }
                                m_nBinaryCodeLength += 6;

                                m_pchStackTypes[m_nStackCurrentDepth] = VM_STACK_OBJECT;
                                ++m_nStackCurrentDepth;
                            }
                        }
                    }
                    return 0;
                }
            }
        }
        return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_UNKNOWN_STATE_IN_COMPILER, pNode);
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_WHILE_BLOCK)
    {
        pNode->nIntegerData3 = m_nLoopIdentifier;
        m_nLoopIdentifier = m_nOutputCodeLength;
        pNode->nIntegerData = m_nOutputCodeLength;

        if (m_nCompileSymbolicOutput != 0)
        {
            sprintf(m_pchOutputCode + m_nOutputCodeLength, "_W1_%08x:\n", m_nOutputCodeLength);
            m_nOutputCodeLength += 14;
        }
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_SWITCH_BLOCK)
    {
        ++m_nSwitchLevel;
        pNode->nIntegerData2 = m_nSwitchIdentifier;
        pNode->nIntegerData3 = m_nSwitchStackDepth;
        m_nSwitchIdentifier = m_nOutputCodeLength;
        m_nSwitchStackDepth = m_nStackCurrentDepth;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_DOWHILE_BLOCK)
    {
        pNode->nIntegerData3 = m_nLoopIdentifier;
        m_nLoopIdentifier = m_nOutputCodeLength;
        pNode->nIntegerData = m_nOutputCodeLength;

        if (m_nCompileSymbolicOutput != 0)
        {
            sprintf(m_pchOutputCode + m_nOutputCodeLength, "_DW1_%08x:\n", m_nOutputCodeLength);
            m_nOutputCodeLength += 15;
        }
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_IF_CHOICE)
    {

        pNode->nIntegerData = m_nOutputCodeLength;

        if (m_pchStackTypes[m_nStackCurrentDepth - 1] != VM_STACK_INTEGER)
        {
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_INTEGER_NOT_AT_TOP_OF_STACK, pNode);
        }
        --m_nStackCurrentDepth;

        if (m_nCompileSymbolicOutput == 0)
        {
            m_pchOutputCode[m_nOutputCodeLength] = VM_JZ;
            m_pchOutputCode[m_nOutputCodeLength + 1] = 0;
            m_nOutputCodeLength += 6;
        }
        else
        {
            PrintBinaryAddress();
            sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                    "JZ _I1_%08x\n", pNode->nIntegerData);
            m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 16;
        }
        m_nBinaryCodeLength += 6;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_COND_CHOICE)
    {
        pNode->nIntegerData = m_nOutputCodeLength;

        if (m_pchStackTypes[m_nStackCurrentDepth - 1] != VM_STACK_INTEGER)
        {
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_INTEGER_NOT_AT_TOP_OF_STACK, pNode);
        }
        --m_nStackCurrentDepth;

        if (m_nCompileSymbolicOutput == 0)
        {
            m_pchOutputCode[m_nOutputCodeLength] = VM_JZ;
            m_pchOutputCode[m_nOutputCodeLength + 1] = 0;
            m_nOutputCodeLength += 6;
        }
        else
        {
            PrintBinaryAddress();
            sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH,
                    "JZ _CH1_%08x\n", pNode->nIntegerData);
            m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 17;
        }
        m_nBinaryCodeLength += 6;
    }

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_PRE_INCREMENT ||
        pNode->nOperation == CSCRIPTCOMPILER_OPERATION_PRE_DECREMENT)
    {

        if (m_nCompileSymbolicOutput == 0)
        {
            if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_PRE_INCREMENT)
            {
                m_pchOutputCode[m_nOutputCodeLength] = VM_INCISP;
            }
            else
            {
                m_pchOutputCode[m_nOutputCodeLength] = VM_DECISP;
            }
            m_pchOutputCode[m_nOutputCodeLength + 1] = VM_STACK_INTEGER;
            m_pchOutputCode[m_nOutputCodeLength + 2] = 0;
            m_pchOutputCode[m_nOutputCodeLength + 3] = 0;
            m_pchOutputCode[m_nOutputCodeLength + 4] = 0;
            m_pchOutputCode[m_nOutputCodeLength + 5] = 0;
            pNode->nIntegerData2 = m_nOutputCodeLength;
            m_nOutputCodeLength += 6;
        }
        else
        {
            PrintBinaryAddress();
            if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_PRE_INCREMENT)
            {
                sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH, "INCISP %08x\n", 0);
            }
            else
            {
                sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH, "DECISP %08x\n", 0);
            }
            pNode->nIntegerData2 = m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH;
            m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 16;
        }
        m_nBinaryCodeLength += 6;
    }

    return 0;
}

int CScriptCompilerInternal::WalkParseTree(CScriptParseTreeNode* pNode)
{
    int nReturnCode = 0;

    if (pNode != NULL)
    {
        nReturnCode = PreVisitGenerateCode(pNode);
        if (nReturnCode == 0)
        {
            nReturnCode = WalkParseTree(pNode->pLeft);
            if (nReturnCode == 0)
            {
                nReturnCode = InVisitGenerateCode(pNode);
                if (nReturnCode == 0)
                {
                    nReturnCode = WalkParseTree(pNode->pRight);
                    if (nReturnCode == 0)
                    {
                        nReturnCode = PostVisitGenerateCode(pNode);
                    }
                }
            }
        }

        if (nReturnCode > 0)
        {
            nReturnCode = 0;
        }
        else if (nReturnCode < 0)
        {
            return nReturnCode;
        }

        if (m_nOutputCodeLength >= m_nOutputCodeSize - CSCRIPTCOMPILER_OUTPUT_CODE_SLACK)
        {
            m_nOutputCodeSize += CSCRIPTCOMPILER_OUTPUT_CODE_BLOCK_SIZE;
            char* pNewArray = new char[m_nOutputCodeSize];
            memcpy(pNewArray, m_pchOutputCode, m_nOutputCodeLength);
            if (m_pchOutputCode != NULL)
            {
                delete[] m_pchOutputCode;
            }
            m_pchOutputCode = pNewArray;
            return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_SCRIPT_TOO_LARGE, pNode);
        }
    }

    return nReturnCode;
}

int CScriptCompilerInternal::InstallLoader()
{

    int nMainIdentifier;
    if (m_bCompileConditionalFile != FALSE)
    {
        nMainIdentifier = GetIdentifierByName("StartingConditional");
        if (nMainIdentifier < 0)
        {
            return STRREF_CSCRIPTCOMPILER_ERROR_NO_FUNCTION_INTSC_IN_SCRIPT;
        }
        if (m_pcIdentifierList[nMainIdentifier].m_nReturnType != CSCRIPTCOMPILER_TOKEN_INTEGER_IDENTIFIER)
        {
            return STRREF_CSCRIPTCOMPILER_ERROR_FUNCTION_INTSC_MUST_HAVE_VOID_RETURN_VALUE;
        }
        if (m_pcIdentifierList[nMainIdentifier].m_nParameters != 0)
        {
            return STRREF_CSCRIPTCOMPILER_ERROR_FUNCTION_INTSC_MUST_HAVE_NO_PARAMETERS;
        }
    }
    else
    {
        nMainIdentifier = GetIdentifierByName("main");
        if (nMainIdentifier < 0)
        {
            return STRREF_CSCRIPTCOMPILER_ERROR_NO_FUNCTION_MAIN_IN_SCRIPT;
        }
        if (m_pcIdentifierList[nMainIdentifier].m_nReturnType != CSCRIPTCOMPILER_TOKEN_VOID_IDENTIFIER &&
            (m_pcIdentifierList[nMainIdentifier].m_nReturnType != CSCRIPTCOMPILER_TOKEN_INTEGER_IDENTIFIER ||
             m_bCompilingConditional != TRUE))
        {
            return STRREF_CSCRIPTCOMPILER_ERROR_FUNCTION_MAIN_MUST_HAVE_VOID_RETURN_VALUE;
        }
        if (m_pcIdentifierList[nMainIdentifier].m_nParameters != 0)
        {
            return STRREF_CSCRIPTCOMPILER_ERROR_FUNCTION_MAIN_MUST_HAVE_NO_PARAMETERS;
        }
    }

    BOOL bGlobalVariablesPresent = FALSE;
    if (m_pGlobalVariableParseTree != NULL)
    {
        bGlobalVariablesPresent = TRUE;
    }

    m_pcIdentifierList[m_nOccupiedIdentifiers].m_psIdentifier = "#loader";
    m_pcIdentifierList[m_nOccupiedIdentifiers].m_nIdentifierLength = 7;
    m_pcIdentifierList[m_nOccupiedIdentifiers].m_nBinarySourceStart = m_nBinaryCodeLength;
    m_pcIdentifierList[m_nOccupiedIdentifiers].m_nBinaryDestinationStart = -1;
    m_pcIdentifierList[m_nOccupiedIdentifiers].m_nBinaryDestinationFinish = -1;

    if (m_pcIdentifierList[nMainIdentifier].m_nReturnType == CSCRIPTCOMPILER_TOKEN_INTEGER_IDENTIFIER)
    {
        ++m_nOccupiedVariables;
        ++m_nVarStackRecursionLevel;
        ++m_nGlobalVariables;

        m_pcVarStackList[m_nOccupiedVariables].m_psVarName = "#retval";
        m_pcVarStackList[m_nOccupiedVariables].m_nVarType = CSCRIPTCOMPILER_TOKEN_KEYWORD_INT;
        m_pcVarStackList[m_nOccupiedVariables].m_nVarLevel = m_nVarStackRecursionLevel;
        m_pcVarStackList[m_nOccupiedVariables].m_nVarRunTimeLocation = m_nStackCurrentDepth * 4;

        AddVariableToStack(CSCRIPTCOMPILER_TOKEN_KEYWORD_INT, "", TRUE);
        --m_nStackCurrentDepth;
    }

    if (m_nCompileSymbolicOutput == 0)
    {
        m_pchOutputCode[m_nOutputCodeLength] = VM_JSR;
        m_pchOutputCode[m_nOutputCodeLength + 1] = 0;

        CExoString sSymbolName;
        if (bGlobalVariablesPresent)
        {
            sSymbolName.Format("FE_#globals");
        }
        else if (m_bCompileConditionalFile == TRUE)
        {
            sSymbolName.Format("FE_StartingConditional");
        }
        else
        {
            sSymbolName.Format("FE_main");
        }
        AddSymbolToQueryList(sSymbolName, m_nOutputCodeLength + 2);
        m_nOutputCodeLength += 6;
    }
    else
    {
        PrintBinaryAddress();
        if (bGlobalVariablesPresent)
        {
            sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH, "JSR FE_#globals\n");
            m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 16;
        }
        else if (m_bCompileConditionalFile != FALSE)
        {
            sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH, "JSR FE_StartingConditional\n");
            m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 27;
        }
        else
        {
            sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH, "JSR FE_main\n");
            m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 12;
        }
    }
    m_nBinaryCodeLength += 6;

    if (m_nCompileSymbolicOutput == 0)
    {
        m_pchOutputCode[m_nOutputCodeLength] = VM_RETN;
        m_pchOutputCode[m_nOutputCodeLength + 1] = 0;
        m_nOutputCodeLength += 2;
    }
    else
    {
        PrintBinaryAddress();
        sprintf(m_pchOutputCode + m_nOutputCodeLength + CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH, "RETN\n");
        m_nOutputCodeLength += CSCRIPTCOMPILER_SYMBOLIC_ADDRESS_LENGTH + 5;
    }
    m_nBinaryCodeLength += 2;

    m_pcIdentifierList[m_nOccupiedIdentifiers].m_nBinarySourceFinish = m_nBinaryCodeLength;

    ++m_nOccupiedIdentifiers;
    if (m_nOccupiedIdentifiers >= CSCRIPTCOMPILER_MAX_IDENTIFIERS)
    {
        return OutputWalkTreeError(STRREF_CSCRIPTCOMPILER_ERROR_IDENTIFIER_LIST_FULL, NULL);
    }

    return 0;
}

CScriptParseTreeNode* CScriptCompilerInternal::InsertGlobalVariablesInParseTree(CScriptParseTreeNode* pOldTree)
{

    if (m_pGlobalVariableParseTree == NULL)
    {
        return pOldTree;
    }

    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_FUNCTIONAL_UNIT,
                                                               m_pGlobalVariableParseTree, pOldTree);
    m_pGlobalVariableParseTree = NULL;

    return pNewNode;
}

int CScriptCompilerInternal::GenerateFinalCodeFromParseTree()
{
    int nState;
    int nRule;
    int nTerm;
    CScriptParseTreeNode* pCurrentTree;
    CScriptParseTreeNode* pReturnTree;

    PopSRStack(&nState, &nRule, &nTerm, &pCurrentTree, &pReturnTree);

    m_nTotalCompileNodes = 1;

    int nReturnValue = InstallLoader();
    CScriptParseTreeNode* pNewReturnTree = InsertGlobalVariablesInParseTree(pReturnTree);

    if (nReturnValue < 0)
    {
        OutputWalkTreeError(nReturnValue, NULL);
        return CleanUpAfterCompile(nReturnValue, pNewReturnTree);
    }

    nReturnValue = WalkParseTree(pNewReturnTree);
    if (nReturnValue < 0)
    {
        return CleanUpAfterCompile(nReturnValue, pNewReturnTree);
    }

    if (m_nCompileSymbolicOutput != 0)
    {
        return CleanUpAfterCompile(0, pNewReturnTree);
    }

    nReturnValue = DetermineLocationOfCode();
    if (nReturnValue >= 0)
    {
        nReturnValue = ResolveLabels();
    }
    if (nReturnValue >= 0)
    {
        nReturnValue = WriteResolvedOutput();
    }
    if (nReturnValue < 0)
    {
        return CleanUpAfterCompile(nReturnValue, pNewReturnTree);
    }
    return CleanUpAfterCompile(0, pNewReturnTree);
}
