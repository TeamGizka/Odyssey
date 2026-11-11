#include "exobase.h"
#include "exotlktable.h"
#include "nwscriptinternal.h"

void CScriptCompilerInternal::TokenInitialize()
{
    m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_UNKNOWN;
    m_nTokenCharacters = 0;
}

void CScriptCompilerInternal::PushSRStack(int nState, int nRule, int nTerm,
                                          CScriptParseTreeNode* pCurrentTree)
{
    if (m_nSRStackStates + 1 >= m_nSRStackEntries)
    {
        CScriptCompilerStackEntry* pNewSRStack = new CScriptCompilerStackEntry[m_nSRStackEntries * 2];
        for (int nCount = 0; nCount < m_nSRStackEntries; ++nCount)
        {
            pNewSRStack[nCount].nState = m_pSRStack[nCount].nState;
            pNewSRStack[nCount].nRule = m_pSRStack[nCount].nRule;
            pNewSRStack[nCount].nTerm = m_pSRStack[nCount].nTerm;
            pNewSRStack[nCount].pCurrentTree = m_pSRStack[nCount].pCurrentTree;
            pNewSRStack[nCount].pReturnTree = m_pSRStack[nCount].pReturnTree;
        }
        m_nSRStackEntries *= 2;
        if (m_pSRStack != NULL)
        {
            delete[] m_pSRStack;
        }
        m_pSRStack = pNewSRStack;
    }

    ++m_nSRStackStates;
    m_pSRStack[m_nSRStackStates].nState = nState;
    m_pSRStack[m_nSRStackStates].nRule = nRule;
    m_pSRStack[m_nSRStackStates].nTerm = nTerm;
    m_pSRStack[m_nSRStackStates].pCurrentTree = pCurrentTree;
    m_pSRStack[m_nSRStackStates].pReturnTree = NULL;
}

int CScriptCompilerInternal::PopSRStack(int* pnState, int* pnRule, int* pnTerm,
                                        CScriptParseTreeNode** ppCurrentTree,
                                        CScriptParseTreeNode** ppReturnTree)
{
    if (m_nSRStackStates < 0)
    {
        return FALSE;
    }

    *pnState = m_pSRStack[m_nSRStackStates].nState;
    *pnRule = m_pSRStack[m_nSRStackStates].nRule;
    *pnTerm = m_pSRStack[m_nSRStackStates].nTerm;
    *ppCurrentTree = m_pSRStack[m_nSRStackStates].pCurrentTree;
    *ppReturnTree = m_pSRStack[m_nSRStackStates].pReturnTree;
    --m_nSRStackStates;
    return TRUE;
}

void CScriptCompilerInternal::ModifySRStackReturnTree(CScriptParseTreeNode* pReturnTree)
{
    if (m_nSRStackStates >= 0)
    {
        m_pSRStack[m_nSRStackStates].pReturnTree = pReturnTree;
    }
}

CScriptParseTreeNode* CScriptCompilerInternal::DuplicateScriptParseTree(CScriptParseTreeNode* pNode)
{
    CScriptParseTreeNode* pNewNode = NULL;

    if (pNode != NULL)
    {
        pNewNode = new CScriptParseTreeNode();
        pNewNode->nOperation = pNode->nOperation;
        pNewNode->m_psStringData = pNode->m_psStringData;
        pNewNode->nIntegerData = pNode->nIntegerData;
        pNewNode->nIntegerData2 = pNode->nIntegerData2;
        pNewNode->fFloatData = pNode->fFloatData;
        pNewNode->fVectorData[0] = pNode->fVectorData[0];
        pNewNode->fVectorData[1] = pNode->fVectorData[1];
        pNewNode->fVectorData[2] = pNode->fVectorData[2];
        pNewNode->nLine = pNode->nLine;
        pNewNode->nChar = pNode->nChar;
        pNewNode->nType = pNode->nType;
        pNewNode->m_psTypeName = pNode->m_psTypeName;
        pNewNode->m_nStackPointer = pNode->m_nStackPointer;
        pNewNode->pLeft = DuplicateScriptParseTree(pNode->pLeft);
        pNewNode->pRight = DuplicateScriptParseTree(pNode->pRight);
    }

    return pNewNode;
}

BOOL CScriptCompilerInternal::CheckForBadLValue(CScriptParseTreeNode* pNode)
{
    BOOL bBadLValue = TRUE;

    if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_VARIABLE)
    {
        bBadLValue = FALSE;
    }
    else if (pNode->nOperation == CSCRIPTCOMPILER_OPERATION_STRUCTURE_PART)
    {
        CScriptParseTreeNode* pTraceBranch = pNode->pLeft;
        while (pTraceBranch != NULL && bBadLValue == TRUE)
        {
            if (pTraceBranch->nOperation == CSCRIPTCOMPILER_OPERATION_VARIABLE)
            {
                bBadLValue = FALSE;
            }
            pTraceBranch = pTraceBranch->pLeft;
        }
    }

    return bBadLValue;
}

void CScriptCompilerInternal::DeleteCompileStack()
{
    for (int nCount = 0; nCount <= m_nSRStackStates; ++nCount)
    {
        if (m_pSRStack[nCount].pCurrentTree != NULL)
        {
            DeleteParseTree(TRUE, m_pSRStack[nCount].pCurrentTree);
        }
        if (m_pSRStack[nCount].pReturnTree != NULL)
        {
            DeleteParseTree(TRUE, m_pSRStack[nCount].pReturnTree);
        }
    }
}

int CScriptCompilerInternal::CleanUpDuringCompile(int nReturnValue)
{
    DeleteCompileStack();

    --m_nCompileFileLevel;
    if (m_nCompileFileLevel > 0)
    {
        ShutdownIncludeFile(m_nCompileFileLevel);
    }

    DeleteParseTree(FALSE, m_pGlobalVariableParseTree);
    m_pGlobalVariableParseTree = NULL;
    ClearUserDefinedIdentifiers();
    ClearAllSymbolLists();
    return nReturnValue;
}

int CScriptCompilerInternal::PrintParseSourceError(int nChar, int nError)
{
    STR_RES strRes;
    g_pTlkTable->Fetch(-nError, strRes, CTlkTable::GENDER_MALE);

    CExoString sFileName = m_pcIncludeFileStack[m_nCompileFileLevel - 1].m_sFileName;
    CExoString sErrorText;

    if (nError != STRREF_CSCRIPTCOMPILER_ERROR_UNDEFINED_IDENTIFIER)
    {
        sErrorText.Format("%s", strRes.m_sString.CStr());
    }
    else
    {
        sErrorText.Format("%s (%s)", strRes.m_sString.CStr(), m_sUndefinedIdentifier.CStr());
    }

    OutputError(nError, sFileName, m_nLines, sErrorText);
    return CleanUpDuringCompile(nError);
}

int CScriptCompilerInternal::ParseSource(char* pScript, int nScriptLength)
{
    int i;
    int ch;
    int chNext;
    int nParseCharacterReturn;

    if (m_nOccupiedIdentifiers == 0)
    {
        int nReturnValue = ParseIdentifierFile();
        if (nReturnValue < 0)
        {
            return nReturnValue;
        }
    }

    if (nScriptLength < 1)
    {
        ch = -1;
    }
    else
    {
        ch = pScript[0];
    }

    if (nScriptLength < 2)
    {
        chNext = -1;
    }
    else
    {
        chNext = pScript[1];
    }

    i = 2;

    while (ch != -1)
    {
        nParseCharacterReturn = ParseNextCharacter(ch, chNext);
        if (nParseCharacterReturn < 0)
        {
            return PrintParseSourceError(ch, nParseCharacterReturn);
        }

        while (nParseCharacterReturn >= 0)
        {
            if (ch == '\n')
            {
                ++m_nLines;
                m_nCharacterOnLine = 1;
            }
            else
            {
                ++m_nCharacterOnLine;
            }

            ch = chNext;

            if (i >= nScriptLength)
            {
                chNext = -1;
            }
            else
            {
                chNext = pScript[i];
            }

            --nParseCharacterReturn;
            ++i;
        }
    }

    nParseCharacterReturn = ParseNextCharacter(-1, -1);
    if (nParseCharacterReturn < 0)
    {
        return PrintParseSourceError(-1, nParseCharacterReturn);
    }

    return 0;
}

int CScriptCompilerInternal::AddUserDefinedIdentifier(CScriptParseTreeNode* pFunctionDeclaration,
                                                      BOOL bFunctionImplementation)
{

    m_pcIdentifierList[m_nOccupiedIdentifiers].m_psIdentifier = pFunctionDeclaration->pLeft->pLeft->m_psStringData;
    m_pcIdentifierList[m_nOccupiedIdentifiers].m_nIdentifierLength = pFunctionDeclaration->pLeft->pLeft->m_psStringData.GetLength();
    m_pcIdentifierList[m_nOccupiedIdentifiers].m_nIdentifierType = 1;

    int nOperationReturnType = pFunctionDeclaration->pLeft->pLeft->pLeft->nOperation;

    if (nOperationReturnType == CSCRIPTCOMPILER_OPERATION_KEYWORD_INT)
    {
        m_pcIdentifierList[m_nOccupiedIdentifiers].m_nReturnType = CSCRIPTCOMPILER_TOKEN_INTEGER_IDENTIFIER;
    }
    else if (nOperationReturnType == CSCRIPTCOMPILER_OPERATION_KEYWORD_OBJECT)
    {
        m_pcIdentifierList[m_nOccupiedIdentifiers].m_nReturnType = CSCRIPTCOMPILER_TOKEN_OBJECT_IDENTIFIER;
    }
    else if (nOperationReturnType == CSCRIPTCOMPILER_OPERATION_KEYWORD_FLOAT)
    {
        m_pcIdentifierList[m_nOccupiedIdentifiers].m_nReturnType = CSCRIPTCOMPILER_TOKEN_FLOAT_IDENTIFIER;
    }
    else if (nOperationReturnType == CSCRIPTCOMPILER_OPERATION_KEYWORD_STRING)
    {
        m_pcIdentifierList[m_nOccupiedIdentifiers].m_nReturnType = CSCRIPTCOMPILER_TOKEN_STRING_IDENTIFIER;
    }
    else if (nOperationReturnType == CSCRIPTCOMPILER_OPERATION_KEYWORD_VOID)
    {
        m_pcIdentifierList[m_nOccupiedIdentifiers].m_nReturnType = CSCRIPTCOMPILER_TOKEN_VOID_IDENTIFIER;
    }
    else if (nOperationReturnType == CSCRIPTCOMPILER_OPERATION_KEYWORD_STRUCT)
    {
        m_pcIdentifierList[m_nOccupiedIdentifiers].m_nReturnType = CSCRIPTCOMPILER_TOKEN_STRUCTURE_IDENTIFIER;
        m_pcIdentifierList[m_nOccupiedIdentifiers].m_psStructureReturnName = pFunctionDeclaration->pLeft->pLeft->pLeft->m_psStringData;
    }
    else if (nOperationReturnType >= CSCRIPTCOMPILER_OPERATION_KEYWORD_ENGINE_STRUCTURE0 &&
             nOperationReturnType <= CSCRIPTCOMPILER_OPERATION_KEYWORD_ENGINE_STRUCTURE9)
    {
        m_pcIdentifierList[m_nOccupiedIdentifiers].m_nReturnType = CSCRIPTCOMPILER_TOKEN_ENGINE_STRUCTURE0_IDENTIFIER +
                                                                   (nOperationReturnType - CSCRIPTCOMPILER_OPERATION_KEYWORD_ENGINE_STRUCTURE0);
    }
    else
    {
        return STRREF_CSCRIPTCOMPILER_ERROR_PARSING_FUNCTION_DECLARATION;
    }

    m_pcIdentifierList[m_nOccupiedIdentifiers].m_nIdIdentifier = -1;

    CScriptParseTreeNode* pNode = pFunctionDeclaration->pLeft->pRight;
    int nTotalParameters = 0;
    while (pNode != NULL)
    {
        ++nTotalParameters;
        pNode = pNode->pLeft;
    }

    while (nTotalParameters > m_pcIdentifierList[m_nOccupiedIdentifiers].m_nParameterSpace)
    {
        int nReturnValue = m_pcIdentifierList[m_nOccupiedIdentifiers].ExpandParameterSpace();
        if (nReturnValue < 0)
        {
            return nReturnValue;
        }
    }

    pNode = pFunctionDeclaration->pLeft->pRight;

    if (pNode == NULL)
    {
        m_pcIdentifierList[m_nOccupiedIdentifiers].m_nParameters = 0;
        m_pcIdentifierList[m_nOccupiedIdentifiers].m_nNonOptionalParameters = 0;
    }
    else
    {
        int nCurrentParameters = 0;
        BOOL bOptionalParametersStarted = FALSE;

        while (pNode != NULL)
        {
            int nParameterType = pNode->pRight->nOperation;
            char nNewParameterType;

            if (nParameterType == CSCRIPTCOMPILER_OPERATION_KEYWORD_INT)
            {
                nNewParameterType = CSCRIPTCOMPILER_TOKEN_KEYWORD_INT;
            }
            else if (nParameterType == CSCRIPTCOMPILER_OPERATION_KEYWORD_OBJECT)
            {
                nNewParameterType = CSCRIPTCOMPILER_TOKEN_KEYWORD_OBJECT;
            }
            else if (nParameterType == CSCRIPTCOMPILER_OPERATION_KEYWORD_FLOAT)
            {
                nNewParameterType = CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT;
            }
            else if (nParameterType == CSCRIPTCOMPILER_OPERATION_KEYWORD_STRING)
            {
                nNewParameterType = CSCRIPTCOMPILER_TOKEN_KEYWORD_STRING;
            }
            else if (nParameterType == CSCRIPTCOMPILER_OPERATION_KEYWORD_VOID)
            {
                nNewParameterType = CSCRIPTCOMPILER_TOKEN_KEYWORD_VOID;
            }
            else if (nParameterType == CSCRIPTCOMPILER_OPERATION_KEYWORD_STRUCT)
            {
                nNewParameterType = CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT;
            }
            else if (nParameterType >= CSCRIPTCOMPILER_OPERATION_KEYWORD_ENGINE_STRUCTURE0 &&
                     nParameterType <= CSCRIPTCOMPILER_OPERATION_KEYWORD_ENGINE_STRUCTURE9)
            {
                nNewParameterType = (char) (CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE0 +
                                            (nParameterType - CSCRIPTCOMPILER_OPERATION_KEYWORD_ENGINE_STRUCTURE0));
            }
            else
            {
                return STRREF_CSCRIPTCOMPILER_ERROR_PARSING_FUNCTION_DECLARATION;
            }

            m_pcIdentifierList[m_nOccupiedIdentifiers].m_pchParameters[nCurrentParameters] = nNewParameterType;
            m_pcIdentifierList[m_nOccupiedIdentifiers].m_pbOptionalParameters[nCurrentParameters] = FALSE;

            if (bOptionalParametersStarted == TRUE && pNode->pRight->pRight == NULL)
            {
                return STRREF_CSCRIPTCOMPILER_ERROR_NON_OPTIONAL_PARAMETER_CANNOT_FOLLOW_OPTIONAL_PARAMETER;
            }

            if (nNewParameterType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT)
            {
                m_pcIdentifierList[m_nOccupiedIdentifiers].m_psStructureParameterNames[nCurrentParameters] = pNode->pRight->m_psStringData;

                if (pNode->pRight->pRight != NULL && pNode->pRight->m_psStringData != "vector")
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_TYPE_DOES_NOT_HAVE_AN_OPTIONAL_PARAMETER;
                }

                if (pNode->pRight->pRight != NULL && pNode->pRight->m_psStringData == "vector")
                {
                    CScriptParseTreeNode* pNodeToDelete = pNode->pRight->pRight;

                    if (pNodeToDelete->nOperation != CSCRIPTCOMPILER_OPERATION_CONSTANT_VECTOR)
                    {
                        pNode->pRight->pRight = NULL;
                        delete pNodeToDelete;
                        return STRREF_CSCRIPTCOMPILER_ERROR_NON_CONSTANT_IN_FUNCTION_DECLARATION;
                    }

                    m_pcIdentifierList[m_nOccupiedIdentifiers].m_pbOptionalParameters[nCurrentParameters] = TRUE;
                    m_pcIdentifierList[m_nOccupiedIdentifiers].m_pfOptionalParameterVectorData[nCurrentParameters * 3] = pNode->pRight->pRight->fVectorData[0];
                    m_pcIdentifierList[m_nOccupiedIdentifiers].m_pfOptionalParameterVectorData[nCurrentParameters * 3 + 1] = pNode->pRight->pRight->fVectorData[1];
                    m_pcIdentifierList[m_nOccupiedIdentifiers].m_pfOptionalParameterVectorData[nCurrentParameters * 3 + 2] = pNode->pRight->pRight->fVectorData[2];
                    bOptionalParametersStarted = TRUE;
                    pNode->pRight->pRight = NULL;
                    delete pNodeToDelete;
                }
            }
            else if (nNewParameterType == CSCRIPTCOMPILER_TOKEN_KEYWORD_INT)
            {
                if (pNode->pRight->pRight != NULL)
                {
                    CScriptParseTreeNode* pNodeToDelete = pNode->pRight->pRight;

                    if (pNodeToDelete->nOperation != CSCRIPTCOMPILER_OPERATION_CONSTANT_INTEGER)
                    {
                        pNode->pRight->pRight = NULL;
                        delete pNodeToDelete;
                        return STRREF_CSCRIPTCOMPILER_ERROR_NON_CONSTANT_IN_FUNCTION_DECLARATION;
                    }

                    m_pcIdentifierList[m_nOccupiedIdentifiers].m_pbOptionalParameters[nCurrentParameters] = TRUE;
                    m_pcIdentifierList[m_nOccupiedIdentifiers].m_pnOptionalParameterIntegerData[nCurrentParameters] = pNode->pRight->pRight->nIntegerData;
                    bOptionalParametersStarted = TRUE;
                    pNode->pRight->pRight = NULL;
                    delete pNodeToDelete;
                }
            }
            else if (nNewParameterType == CSCRIPTCOMPILER_TOKEN_KEYWORD_OBJECT)
            {
                if (pNode->pRight->pRight != NULL)
                {
                    CScriptParseTreeNode* pNodeToDelete = pNode->pRight->pRight;

                    if (pNodeToDelete->nOperation != CSCRIPTCOMPILER_OPERATION_CONSTANT_OBJECT)
                    {
                        pNode->pRight->pRight = NULL;
                        delete pNodeToDelete;
                        return STRREF_CSCRIPTCOMPILER_ERROR_NON_CONSTANT_IN_FUNCTION_DECLARATION;
                    }

                    m_pcIdentifierList[m_nOccupiedIdentifiers].m_pbOptionalParameters[nCurrentParameters] = TRUE;
                    m_pcIdentifierList[m_nOccupiedIdentifiers].m_poidOptionalParameterObjectData[nCurrentParameters] = pNode->pRight->pRight->nIntegerData;
                    bOptionalParametersStarted = TRUE;
                    pNode->pRight->pRight = NULL;
                    delete pNodeToDelete;
                }
            }
            else if (nNewParameterType == CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT)
            {
                if (pNode->pRight->pRight != NULL)
                {
                    CScriptParseTreeNode* pNodeToDelete = pNode->pRight->pRight;

                    if (pNodeToDelete->nOperation != CSCRIPTCOMPILER_OPERATION_CONSTANT_FLOAT)
                    {
                        pNode->pRight->pRight = NULL;
                        delete pNodeToDelete;
                        return STRREF_CSCRIPTCOMPILER_ERROR_NON_CONSTANT_IN_FUNCTION_DECLARATION;
                    }

                    m_pcIdentifierList[m_nOccupiedIdentifiers].m_pbOptionalParameters[nCurrentParameters] = TRUE;
                    m_pcIdentifierList[m_nOccupiedIdentifiers].m_pfOptionalParameterFloatData[nCurrentParameters] = pNode->pRight->pRight->fFloatData;
                    bOptionalParametersStarted = TRUE;
                    pNode->pRight->pRight = NULL;
                    delete pNodeToDelete;
                }
            }
            else if (nNewParameterType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRING)
            {
                if (pNode->pRight->pRight != NULL)
                {
                    CScriptParseTreeNode* pNodeToDelete = pNode->pRight->pRight;

                    if (pNodeToDelete->nOperation != CSCRIPTCOMPILER_OPERATION_CONSTANT_STRING)
                    {
                        pNode->pRight->pRight = NULL;
                        delete pNodeToDelete;
                        return STRREF_CSCRIPTCOMPILER_ERROR_NON_CONSTANT_IN_FUNCTION_DECLARATION;
                    }

                    m_pcIdentifierList[m_nOccupiedIdentifiers].m_pbOptionalParameters[nCurrentParameters] = TRUE;
                    m_pcIdentifierList[m_nOccupiedIdentifiers].m_psOptionalParameterStringData[nCurrentParameters] = pNode->pRight->pRight->m_psStringData;
                    bOptionalParametersStarted = TRUE;
                    pNode->pRight->pRight = NULL;
                    delete pNodeToDelete;
                }
            }
            else if (nNewParameterType == CSCRIPTCOMPILER_TOKEN_KEYWORD_VOID ||
                     (nNewParameterType >= CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE0 &&
                      nNewParameterType <= CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE9))
            {
                if (pNode->pRight->pRight != NULL)
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_TYPE_DOES_NOT_HAVE_AN_OPTIONAL_PARAMETER;
                }
            }

            ++nCurrentParameters;
            m_pcIdentifierList[m_nOccupiedIdentifiers].m_nParameters = nCurrentParameters;
            if (bOptionalParametersStarted == FALSE)
            {
                m_pcIdentifierList[m_nOccupiedIdentifiers].m_nNonOptionalParameters = nCurrentParameters;
            }

            pNode = pNode->pLeft;
        }
    }

    BOOL bFoundIdenticalFunction = FALSE;

    for (int nCount = 0; nCount < m_nOccupiedIdentifiers; ++nCount)
    {
        if (m_pcIdentifierList[nCount].m_nIdentifierLength == m_pcIdentifierList[m_nOccupiedIdentifiers].m_nIdentifierLength &&
            m_pcIdentifierList[nCount].m_psIdentifier == m_pcIdentifierList[m_nOccupiedIdentifiers].m_psIdentifier)
        {
            if (m_pcIdentifierList[nCount].m_nParameters == m_pcIdentifierList[m_nOccupiedIdentifiers].m_nParameters)
            {
                BOOL bParameterListDifferent = FALSE;

                for (int nCount2 = 0; nCount2 < m_pcIdentifierList[nCount].m_nParameters; ++nCount2)
                {
                    if (m_pcIdentifierList[nCount].m_pchParameters[nCount2] != m_pcIdentifierList[m_nOccupiedIdentifiers].m_pchParameters[nCount2] ||
                        (m_pcIdentifierList[nCount].m_pchParameters[nCount2] == CSCRIPTCOMPILER_TOKEN_STRUCTURE_IDENTIFIER &&
                         m_pcIdentifierList[nCount].m_psStructureParameterNames[nCount2] != m_pcIdentifierList[m_nOccupiedIdentifiers].m_psStructureParameterNames[nCount2]))
                    {
                        bParameterListDifferent = TRUE;
                    }
                }

                if (bParameterListDifferent == FALSE)
                {
                    if (bFunctionImplementation == TRUE &&
                        m_pcIdentifierList[nCount].m_bImplementationInPlace == TRUE)
                    {
                        return STRREF_CSCRIPTCOMPILER_ERROR_DUPLICATE_FUNCTION_IMPLEMENTATION;
                    }

                    bFoundIdenticalFunction = TRUE;
                }
            }

            if (bFoundIdenticalFunction == FALSE)
            {
                return STRREF_CSCRIPTCOMPILER_ERROR_FUNCTION_IMPLEMENTATION_AND_DEFINITION_DIFFER;
            }
        }
    }

    if (bFoundIdenticalFunction == FALSE)
    {
        m_pcIdentifierList[m_nOccupiedIdentifiers].m_bImplementationInPlace = bFunctionImplementation;
        m_pcIdentifierList[m_nOccupiedIdentifiers].m_nBinarySourceStart = -1;
        m_pcIdentifierList[m_nOccupiedIdentifiers].m_nBinarySourceFinish = -1;
        m_pcIdentifierList[m_nOccupiedIdentifiers].m_nBinaryDestinationStart = -1;
        m_pcIdentifierList[m_nOccupiedIdentifiers].m_nBinaryDestinationFinish = -1;

        ++m_nOccupiedIdentifiers;
        if (m_nOccupiedIdentifiers >= CSCRIPTCOMPILER_MAX_IDENTIFIERS)
        {
            return STRREF_CSCRIPTCOMPILER_ERROR_IDENTIFIER_LIST_FULL;
        }
    }

    return 0;
}

CScriptParseTreeNode* CScriptCompilerInternal::CreateScriptParseTreeNode(int nNodeOperation,
                                                                         CScriptParseTreeNode* pNodeLeft,
                                                                         CScriptParseTreeNode* pNodeRight)
{
    CScriptParseTreeNode* pNewNode = new CScriptParseTreeNode();
    pNewNode->nOperation = nNodeOperation;
    pNewNode->pLeft = pNodeLeft;
    pNewNode->pRight = pNodeRight;
    pNewNode->nLine = m_nLines;
    pNewNode->nChar = m_nCharacterOnLine;
    return pNewNode;
}

int CScriptCompilerInternal::AddToGlobalVariableList(CScriptParseTreeNode* pGlobalVariableNode)
{
    if (m_pGlobalVariableParseTree == NULL)
    {
        m_pGlobalVariableParseTree = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_GLOBAL_VARIABLES, NULL, NULL);
    }

    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_STATEMENT, pGlobalVariableNode, NULL);

    CScriptParseTreeNode* pFindLastStatement = m_pGlobalVariableParseTree;
    while (pFindLastStatement->pRight != NULL)
    {
        pFindLastStatement = pFindLastStatement->pRight;
    }
    pFindLastStatement->pRight = pNewNode;

    return 0;
}

int CScriptCompilerInternal::GenerateParseTree()
{
    int nTopStackState;
    int nTopStackRule;
    int nTopStackTerm;
    CScriptParseTreeNode* pTopStackCurrentNode;
    CScriptParseTreeNode* pTopStackReturnNode;

    BOOL bAlwaysTrue = TRUE;

    while (bAlwaysTrue)
    {
        if (PopSRStack(&nTopStackState, &nTopStackRule, &nTopStackTerm,
                       &pTopStackCurrentNode, &pTopStackReturnNode) == FALSE)
        {
            return STRREF_CSCRIPTCOMPILER_ERROR_FATAL_COMPILER_ERROR;
        }

        switch (nTopStackState)
        {

        case CSCRIPTCOMPILER_GRAMMAR_CONSTANT:
            if (nTopStackRule == 0 && nTopStackTerm == 0)
            {
                int nCount;

                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_LEFT_SQUARE_BRACKET)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_CONSTANT_VECTOR, NULL, NULL);
                    pNewNode->fVectorData[0] = 0.0f;
                    pNewNode->fVectorData[1] = 0.0f;
                    pNewNode->fVectorData[2] = 0.0f;
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_CONSTANT, 6, 2, pNewNode);
                    return 0;
                }
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_INTEGER)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_CONSTANT_INTEGER, NULL, NULL);
                    pNewNode->nIntegerData = 0;
                    int nSign = 1;
                    for (nCount = 0; nCount < m_nTokenCharacters; ++nCount)
                    {

                        if (m_pchToken[nCount] == '-' && nCount == 0)
                        {
                            nSign = -1;
                        }
                        else
                        {
                            pNewNode->nIntegerData *= 10;
                            pNewNode->nIntegerData += (m_pchToken[nCount] - '0');
                        }
                    }
                    if (nSign == -1)
                    {
                        pNewNode->nIntegerData *= nSign;
                    }
                    ModifySRStackReturnTree(pNewNode);
                    return 0;
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_HEX_INTEGER)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_CONSTANT_INTEGER, NULL, NULL);
                    m_pchToken[m_nTokenCharacters] = 0;
                    pNewNode->nIntegerData = 0;

                    for (nCount = 2; nCount < m_nTokenCharacters; ++nCount)
                    {
                        if (m_pchToken[nCount] >= '0' && m_pchToken[nCount] <= '9')
                        {
                            pNewNode->nIntegerData *= 16;
                            pNewNode->nIntegerData += (m_pchToken[nCount] - '0');
                        }
                        else
                        {
                            pNewNode->nIntegerData *= 16;
                            pNewNode->nIntegerData += 10 + (m_pchToken[nCount] - 'a');
                        }
                    }
                    ModifySRStackReturnTree(pNewNode);
                    return 0;
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_FLOAT)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_CONSTANT_FLOAT, NULL, NULL);
                    pNewNode->fFloatData = ParseFloatFromTokenString();
                    ModifySRStackReturnTree(pNewNode);
                    return 0;
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_STRING)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_CONSTANT_STRING, NULL, NULL);
                    m_pchToken[m_nTokenCharacters] = 0;
                    pNewNode->m_psStringData.Format("%s", m_pchToken);
                    ModifySRStackReturnTree(pNewNode);
                    return 0;
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_OBJECT_SELF)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_CONSTANT_OBJECT, NULL, NULL);
                    pNewNode->nIntegerData = 0;
                    ModifySRStackReturnTree(pNewNode);
                    return 0;
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_OBJECT_INVALID)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_CONSTANT_OBJECT, NULL, NULL);
                    pNewNode->nIntegerData = 1;
                    ModifySRStackReturnTree(pNewNode);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_UNKNOWN_STATE_IN_COMPILER;
                }
            }
            if (nTopStackRule == 6 && nTopStackTerm == 2)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_RIGHT_SQUARE_BRACKET)
                {
                    ModifySRStackReturnTree(pTopStackCurrentNode);
                    return 0;
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_FLOAT)
                {
                    pTopStackCurrentNode->fVectorData[0] = ParseFloatFromTokenString();
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_CONSTANT, 6, 3, pTopStackCurrentNode);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_PARSING_CONSTANT_VECTOR;
                }
            }
            if (nTopStackRule == 6 && nTopStackTerm == 3)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_RIGHT_SQUARE_BRACKET)
                {
                    ModifySRStackReturnTree(pTopStackCurrentNode);
                    return 0;
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_COMMA)
                {
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_CONSTANT, 6, 4, pTopStackCurrentNode);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_PARSING_CONSTANT_VECTOR;
                }
            }
            if (nTopStackRule == 6 && nTopStackTerm == 4)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_FLOAT)
                {
                    pTopStackCurrentNode->fVectorData[1] = ParseFloatFromTokenString();
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_CONSTANT, 6, 5, pTopStackCurrentNode);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_PARSING_CONSTANT_VECTOR;
                }
            }
            if (nTopStackRule == 6 && nTopStackTerm == 5)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_RIGHT_SQUARE_BRACKET)
                {
                    ModifySRStackReturnTree(pTopStackCurrentNode);
                    return 0;
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_COMMA)
                {
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_CONSTANT, 6, 6, pTopStackCurrentNode);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_PARSING_CONSTANT_VECTOR;
                }
            }
            if (nTopStackRule == 6 && nTopStackTerm == 6)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_FLOAT)
                {
                    pTopStackCurrentNode->fVectorData[2] = ParseFloatFromTokenString();
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_CONSTANT, 6, 7, pTopStackCurrentNode);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_PARSING_CONSTANT_VECTOR;
                }
            }
            if (nTopStackRule == 6 && nTopStackTerm == 7)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_RIGHT_SQUARE_BRACKET)
                {
                    ModifySRStackReturnTree(pTopStackCurrentNode);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_PARSING_CONSTANT_VECTOR;
                }
            }
            break;

        case CSCRIPTCOMPILER_GRAMMAR_PRIMARY_EXPRESSION:
            if (nTopStackRule == 0 && nTopStackTerm == 0)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_INTEGER ||
                    m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_HEX_INTEGER ||
                    m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_FLOAT ||
                    m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_STRING ||
                    m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_OBJECT_SELF ||
                    m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_OBJECT_INVALID ||
                    m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_LEFT_SQUARE_BRACKET)
                {
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_PRIMARY_EXPRESSION, 2, 1, NULL);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_CONSTANT, 0, 0, NULL);
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_VARIABLE)
                {
                    CScriptParseTreeNode* pNewNode2 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_VARIABLE, NULL, NULL);
                    m_pchToken[m_nTokenCharacters] = 0;
                    pNewNode2->m_psStringData.Format("%s", m_pchToken);
                    ModifySRStackReturnTree(pNewNode2);
                    return 0;
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_LEFT_BRACKET)
                {
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_PRIMARY_EXPRESSION, 3, 2, NULL);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_EXPRESSION, 0, 0, NULL);
                    return 0;
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_INTEGER_IDENTIFIER ||
                         m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_FLOAT_IDENTIFIER ||
                         m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_STRING_IDENTIFIER ||
                         m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_VOID_IDENTIFIER ||
                         m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_OBJECT_IDENTIFIER ||
                         m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_STRUCTURE_IDENTIFIER ||
                         (m_nTokenStatus >= CSCRIPTCOMPILER_TOKEN_ENGINE_STRUCTURE0_IDENTIFIER &&
                          m_nTokenStatus <= CSCRIPTCOMPILER_TOKEN_ENGINE_STRUCTURE9_IDENTIFIER))
                {
                    CScriptParseTreeNode* pNewNode2 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_ACTION_ID, NULL, NULL);
                    m_pchToken[m_nTokenCharacters] = 0;
                    pNewNode2->m_psStringData.Format("%s", m_pchToken);
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_ACTION, NULL, pNewNode2);

                    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_VOID_IDENTIFIER)
                    {
                        CScriptParseTreeNode* pNewNode3 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_ACTION_PARAMETER, pNewNode, NULL);
                        PushSRStack(CSCRIPTCOMPILER_GRAMMAR_PRIMARY_EXPRESSION, 5, 2, pNewNode3);
                    }
                    else
                    {
                        PushSRStack(CSCRIPTCOMPILER_GRAMMAR_PRIMARY_EXPRESSION, 1, 2, pNewNode);
                    }
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_UNKNOWN_STATE_IN_COMPILER;
                }
            }
            if (nTopStackRule == 1 && nTopStackTerm == 2)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_LEFT_BRACKET)
                {
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_PRIMARY_EXPRESSION, 1, 3, pTopStackCurrentNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_ARGUMENT_EXPRESSION_LIST, 0, 0, NULL);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_NO_LEFT_BRACKET_ON_ARG_LIST;
                }
            }
            if (nTopStackRule == 1 && nTopStackTerm == 3)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_RIGHT_BRACKET)
                {
                    pTopStackCurrentNode->pLeft = pTopStackReturnNode;
                    ModifySRStackReturnTree(pTopStackCurrentNode);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_NO_RIGHT_BRACKET_ON_ARG_LIST;
                }
            }
            if (nTopStackRule == 5 && nTopStackTerm == 2)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_LEFT_BRACKET)
                {
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_PRIMARY_EXPRESSION, 5, 3, pTopStackCurrentNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_ARGUMENT_EXPRESSION_LIST, 0, 0, NULL);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_NO_LEFT_BRACKET_ON_ARG_LIST;
                }
            }
            if (nTopStackRule == 5 && nTopStackTerm == 3)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_RIGHT_BRACKET)
                {
                    pTopStackCurrentNode->pLeft->pLeft = pTopStackReturnNode;
                    ModifySRStackReturnTree(pTopStackCurrentNode);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_NO_RIGHT_BRACKET_ON_ARG_LIST;
                }
            }
            if (nTopStackRule == 2 && nTopStackTerm == 1)
            {
                ModifySRStackReturnTree(pTopStackReturnNode);
            }
            if (nTopStackRule == 3 && nTopStackTerm == 2)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_RIGHT_BRACKET)
                {
                    ModifySRStackReturnTree(pTopStackReturnNode);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_UNKNOWN_STATE_IN_COMPILER;
                }
            }
            break;

        case CSCRIPTCOMPILER_GRAMMAR_POST_EXPRESSION:
            if (nTopStackRule == 0 && nTopStackTerm == 0)
            {
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_POST_EXPRESSION, 1, 2, NULL);
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_PRIMARY_EXPRESSION, 0, 0, NULL);
            }
            if ((nTopStackRule == 1 && nTopStackTerm == 2) || nTopStackTerm == 4)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_STRUCTURE_PART_SPECIFY)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_STRUCTURE_PART, NULL, NULL);
                    if (nTopStackTerm == 4)
                    {
                        pNewNode->pLeft = pTopStackCurrentNode;
                        pNewNode->pLeft->pRight = pTopStackReturnNode;
                    }
                    else if (pTopStackCurrentNode != NULL && pTopStackReturnNode == NULL)
                    {

                        pNewNode->pLeft = pTopStackCurrentNode;
                    }
                    else
                    {
                        pNewNode->pLeft = pTopStackReturnNode;
                    }

                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_POST_EXPRESSION, 2, 4, pNewNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_POST_EXPRESSION, 2, 3, NULL);
                    return 0;
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_INCREMENT)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_POST_INCREMENT, NULL, NULL);
                    if (nTopStackTerm == 4)
                    {
                        pNewNode->pLeft = pTopStackCurrentNode;
                        pNewNode->pLeft->pRight = pTopStackReturnNode;
                    }
                    else
                    {
                        pNewNode->pLeft = pTopStackReturnNode;
                    }
                    ModifySRStackReturnTree(pNewNode);
                    return 0;
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_DECREMENT)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_POST_DECREMENT, NULL, NULL);
                    if (nTopStackTerm == 4)
                    {
                        pNewNode->pLeft = pTopStackCurrentNode;
                        pNewNode->pLeft->pRight = pTopStackReturnNode;
                    }
                    else
                    {
                        pNewNode->pLeft = pTopStackReturnNode;
                    }
                    ModifySRStackReturnTree(pNewNode);
                    return 0;
                }
                else
                {

                    if (nTopStackTerm == 4)
                    {
                        pTopStackCurrentNode->pRight = pTopStackReturnNode;
                        ModifySRStackReturnTree(pTopStackCurrentNode);
                    }
                    else
                    {
                        ModifySRStackReturnTree(pTopStackReturnNode);
                    }
                }
            }
            if (nTopStackRule == 2 && nTopStackTerm == 3)
            {
                CScriptParseTreeNode* pNewNode2 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_VARIABLE, NULL, NULL);
                m_pchToken[m_nTokenCharacters] = 0;
                pNewNode2->m_psStringData.Format("%s", m_pchToken);
                ModifySRStackReturnTree(pNewNode2);
                return 0;
            }
            break;

        case CSCRIPTCOMPILER_GRAMMAR_UNARY_EXPRESSION:
            if (nTopStackRule == 0 && nTopStackTerm == 0)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_MINUS)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_NEGATION, NULL, NULL);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_UNARY_EXPRESSION, 1, 1, pNewNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_POST_EXPRESSION, 0, 0, NULL);
                    return 0;
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_TILDE)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_ONES_COMPLEMENT, NULL, NULL);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_UNARY_EXPRESSION, 2, 1, pNewNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_POST_EXPRESSION, 0, 0, NULL);
                    return 0;
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_BOOLEAN_NOT)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_BOOLEAN_NOT, NULL, NULL);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_UNARY_EXPRESSION, 3, 1, pNewNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_POST_EXPRESSION, 0, 0, NULL);
                    return 0;
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_INCREMENT)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_PRE_INCREMENT, NULL, NULL);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_UNARY_EXPRESSION, 4, 1, pNewNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_POST_EXPRESSION, 0, 0, NULL);
                    return 0;
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_DECREMENT)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_PRE_DECREMENT, NULL, NULL);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_UNARY_EXPRESSION, 5, 1, pNewNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_POST_EXPRESSION, 0, 0, NULL);
                    return 0;
                }
                else
                {
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_UNARY_EXPRESSION, 6, 1, NULL);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_POST_EXPRESSION, 0, 0, NULL);
                }
            }
            if ((nTopStackRule >= 1 && nTopStackRule <= 5) && nTopStackTerm == 1)
            {
                pTopStackCurrentNode->pLeft = pTopStackReturnNode;
                ModifySRStackReturnTree(pTopStackCurrentNode);
            }
            if (nTopStackRule == 6 && nTopStackTerm == 1)
            {
                ModifySRStackReturnTree(pTopStackReturnNode);
            }
            break;

        case CSCRIPTCOMPILER_GRAMMAR_MULTIPLICATIVE_EXPRESSION:
            if (nTopStackRule == 0 && nTopStackTerm == 0)
            {
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_MULTIPLICATIVE_EXPRESSION, 1, 1, NULL);
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_UNARY_EXPRESSION, 0, 0, NULL);
            }
            if ((nTopStackRule == 1 && nTopStackTerm == 1) || nTopStackTerm == 3)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_MULTIPLY)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_MULTIPLY, pTopStackReturnNode, NULL);
                    if (nTopStackTerm == 3)
                    {
                        pNewNode->pLeft = pTopStackCurrentNode;
                        pNewNode->pLeft->pRight = pTopStackReturnNode;
                    }
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_MULTIPLICATIVE_EXPRESSION, 2, 3, pNewNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_UNARY_EXPRESSION, 0, 0, NULL);
                    return 0;
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_DIVIDE)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_DIVIDE, pTopStackReturnNode, NULL);
                    if (nTopStackTerm == 3)
                    {
                        pNewNode->pLeft = pTopStackCurrentNode;
                        pNewNode->pLeft->pRight = pTopStackReturnNode;
                    }
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_MULTIPLICATIVE_EXPRESSION, 3, 3, pNewNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_UNARY_EXPRESSION, 0, 0, NULL);
                    return 0;
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_MODULUS)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_MODULUS, pTopStackReturnNode, NULL);
                    if (nTopStackTerm == 3)
                    {
                        pNewNode->pLeft = pTopStackCurrentNode;
                        pNewNode->pLeft->pRight = pTopStackReturnNode;
                    }
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_MULTIPLICATIVE_EXPRESSION, 4, 3, pNewNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_UNARY_EXPRESSION, 0, 0, NULL);
                    return 0;
                }
                else
                {
                    if (nTopStackTerm == 3)
                    {
                        pTopStackCurrentNode->pRight = pTopStackReturnNode;
                        ModifySRStackReturnTree(pTopStackCurrentNode);
                    }
                    else
                    {
                        ModifySRStackReturnTree(pTopStackReturnNode);
                    }
                }
            }
            break;

        case CSCRIPTCOMPILER_GRAMMAR_ADDITIVE_EXPRESSION:
            if (nTopStackRule == 0 && nTopStackTerm == 0)
            {
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_ADDITIVE_EXPRESSION, 1, 1, NULL);
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_MULTIPLICATIVE_EXPRESSION, 0, 0, NULL);
            }
            if ((nTopStackRule == 1 && nTopStackTerm == 1) || nTopStackTerm == 3)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_PLUS)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_ADD, pTopStackReturnNode, NULL);
                    if (nTopStackTerm == 3)
                    {
                        pNewNode->pLeft = pTopStackCurrentNode;
                        pNewNode->pLeft->pRight = pTopStackReturnNode;
                    }
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_ADDITIVE_EXPRESSION, 2, 3, pNewNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_MULTIPLICATIVE_EXPRESSION, 0, 0, NULL);
                    return 0;
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_MINUS)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_SUBTRACT, pTopStackReturnNode, NULL);
                    if (nTopStackTerm == 3)
                    {
                        pNewNode->pLeft = pTopStackCurrentNode;
                        pNewNode->pLeft->pRight = pTopStackReturnNode;
                    }
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_ADDITIVE_EXPRESSION, 3, 3, pNewNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_MULTIPLICATIVE_EXPRESSION, 0, 0, NULL);
                    return 0;
                }
                else
                {
                    if (nTopStackTerm == 3)
                    {
                        pTopStackCurrentNode->pRight = pTopStackReturnNode;
                        ModifySRStackReturnTree(pTopStackCurrentNode);
                    }
                    else
                    {
                        ModifySRStackReturnTree(pTopStackReturnNode);
                    }
                }
            }
            break;

        case CSCRIPTCOMPILER_GRAMMAR_SHIFT_EXPRESSION:
            if (nTopStackRule == 0 && nTopStackTerm == 0)
            {
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_SHIFT_EXPRESSION, 1, 1, NULL);
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_ADDITIVE_EXPRESSION, 0, 0, NULL);
            }
            if ((nTopStackRule == 1 && nTopStackTerm == 1) || nTopStackTerm == 3)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_SHIFT_LEFT)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_SHIFT_LEFT, pTopStackReturnNode, NULL);
                    if (nTopStackTerm == 3)
                    {
                        pNewNode->pLeft = pTopStackCurrentNode;
                        pNewNode->pLeft->pRight = pTopStackReturnNode;
                    }
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_SHIFT_EXPRESSION, 2, 3, pNewNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_ADDITIVE_EXPRESSION, 0, 0, NULL);
                    return 0;
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_SHIFT_RIGHT)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_SHIFT_RIGHT, pTopStackReturnNode, NULL);
                    if (nTopStackTerm == 3)
                    {
                        pNewNode->pLeft = pTopStackCurrentNode;
                        pNewNode->pLeft->pRight = pTopStackReturnNode;
                    }
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_SHIFT_EXPRESSION, 3, 3, pNewNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_ADDITIVE_EXPRESSION, 0, 0, NULL);
                    return 0;
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_UNSIGNED_SHIFT_RIGHT)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_UNSIGNED_SHIFT_RIGHT, pTopStackReturnNode, NULL);
                    if (nTopStackTerm == 3)
                    {
                        pNewNode->pLeft = pTopStackCurrentNode;
                        pNewNode->pLeft->pRight = pTopStackReturnNode;
                    }
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_SHIFT_EXPRESSION, 4, 3, pNewNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_ADDITIVE_EXPRESSION, 0, 0, NULL);
                    return 0;
                }
                else
                {
                    if (nTopStackTerm == 3)
                    {
                        pTopStackCurrentNode->pRight = pTopStackReturnNode;
                        ModifySRStackReturnTree(pTopStackCurrentNode);
                    }
                    else
                    {
                        ModifySRStackReturnTree(pTopStackReturnNode);
                    }
                }
            }
            break;

        case CSCRIPTCOMPILER_GRAMMAR_RELATIONAL_EXPRESSION:
            if (nTopStackRule == 0 && nTopStackTerm == 0)
            {
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_RELATIONAL_EXPRESSION, 1, 1, NULL);
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_SHIFT_EXPRESSION, 0, 0, NULL);
            }
            if ((nTopStackRule == 1 && nTopStackTerm == 1) || nTopStackTerm == 3)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_COND_GREATER_EQUAL)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_CONDITION_GEQ, pTopStackReturnNode, NULL);
                    if (nTopStackTerm == 3)
                    {
                        pNewNode->pLeft = pTopStackCurrentNode;
                        pNewNode->pLeft->pRight = pTopStackReturnNode;
                    }
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_RELATIONAL_EXPRESSION, 2, 3, pNewNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_SHIFT_EXPRESSION, 0, 0, NULL);
                    return 0;
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_COND_LESS_EQUAL)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_CONDITION_LEQ, pTopStackReturnNode, NULL);
                    if (nTopStackTerm == 3)
                    {
                        pNewNode->pLeft = pTopStackCurrentNode;
                        pNewNode->pLeft->pRight = pTopStackReturnNode;
                    }
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_RELATIONAL_EXPRESSION, 3, 3, pNewNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_SHIFT_EXPRESSION, 0, 0, NULL);
                    return 0;
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_COND_LESS_THAN)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_CONDITION_LT, pTopStackReturnNode, NULL);
                    if (nTopStackTerm == 3)
                    {
                        pNewNode->pLeft = pTopStackCurrentNode;
                        pNewNode->pLeft->pRight = pTopStackReturnNode;
                    }
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_RELATIONAL_EXPRESSION, 4, 3, pNewNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_SHIFT_EXPRESSION, 0, 0, NULL);
                    return 0;
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_COND_GREATER_THAN)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_CONDITION_GT, pTopStackReturnNode, NULL);
                    if (nTopStackTerm == 3)
                    {
                        pNewNode->pLeft = pTopStackCurrentNode;
                        pNewNode->pLeft->pRight = pTopStackReturnNode;
                    }
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_RELATIONAL_EXPRESSION, 5, 3, pNewNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_SHIFT_EXPRESSION, 0, 0, NULL);
                    return 0;
                }
                else
                {
                    if (nTopStackTerm == 3)
                    {
                        pTopStackCurrentNode->pRight = pTopStackReturnNode;
                        ModifySRStackReturnTree(pTopStackCurrentNode);
                    }
                    else
                    {
                        ModifySRStackReturnTree(pTopStackReturnNode);
                    }
                }
            }
            break;

        case CSCRIPTCOMPILER_GRAMMAR_EQUALITY_EXPRESSION:
            if (nTopStackRule == 0 && nTopStackTerm == 0)
            {
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_EQUALITY_EXPRESSION, 1, 1, NULL);
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_RELATIONAL_EXPRESSION, 0, 0, NULL);
            }
            if ((nTopStackRule == 1 && nTopStackTerm == 1) || nTopStackTerm == 3)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_COND_NOT_EQUAL)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_CONDITION_NOT_EQUAL, pTopStackReturnNode, NULL);
                    if (nTopStackTerm == 3)
                    {
                        pNewNode->pLeft = pTopStackCurrentNode;
                        pNewNode->pLeft->pRight = pTopStackReturnNode;
                    }
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_EQUALITY_EXPRESSION, 2, 3, pNewNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_RELATIONAL_EXPRESSION, 0, 0, NULL);
                    return 0;
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_COND_EQUAL)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_CONDITION_EQUAL, pTopStackReturnNode, NULL);
                    if (nTopStackTerm == 3)
                    {
                        pNewNode->pLeft = pTopStackCurrentNode;
                        pNewNode->pLeft->pRight = pTopStackReturnNode;
                    }
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_EQUALITY_EXPRESSION, 3, 3, pNewNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_RELATIONAL_EXPRESSION, 0, 0, NULL);
                    return 0;
                }
                else
                {
                    if (nTopStackTerm == 3)
                    {
                        pTopStackCurrentNode->pRight = pTopStackReturnNode;
                        ModifySRStackReturnTree(pTopStackCurrentNode);
                    }
                    else
                    {
                        ModifySRStackReturnTree(pTopStackReturnNode);
                    }
                }
            }
            break;

        case CSCRIPTCOMPILER_GRAMMAR_BOOLEAN_AND_EXPRESSION:
            if (nTopStackRule == 0 && nTopStackTerm == 0)
            {
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_BOOLEAN_AND_EXPRESSION, 1, 1, NULL);
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_EQUALITY_EXPRESSION, 0, 0, NULL);
            }
            if ((nTopStackRule == 1 && nTopStackTerm == 1) || nTopStackTerm == 3)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_BOOLEAN_AND)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_BOOLEAN_AND, pTopStackReturnNode, NULL);
                    if (nTopStackTerm == 3)
                    {
                        pNewNode->pLeft = pTopStackCurrentNode;
                        pNewNode->pLeft->pRight = pTopStackReturnNode;
                    }
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_BOOLEAN_AND_EXPRESSION, 2, 3, pNewNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_EQUALITY_EXPRESSION, 0, 0, NULL);
                    return 0;
                }
                else
                {
                    if (nTopStackTerm == 3)
                    {
                        pTopStackCurrentNode->pRight = pTopStackReturnNode;
                        ModifySRStackReturnTree(pTopStackCurrentNode);
                    }
                    else
                    {
                        ModifySRStackReturnTree(pTopStackReturnNode);
                    }
                }
            }
            break;

        case CSCRIPTCOMPILER_GRAMMAR_EXCLUSIVE_OR_EXPRESSION:
            if (nTopStackRule == 0 && nTopStackTerm == 0)
            {
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_EXCLUSIVE_OR_EXPRESSION, 1, 1, NULL);
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_BOOLEAN_AND_EXPRESSION, 0, 0, NULL);
            }
            if ((nTopStackRule == 1 && nTopStackTerm == 1) || nTopStackTerm == 3)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_EXCLUSIVE_OR)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_EXCLUSIVE_OR, pTopStackReturnNode, NULL);
                    if (nTopStackTerm == 3)
                    {
                        pNewNode->pLeft = pTopStackCurrentNode;
                        pNewNode->pLeft->pRight = pTopStackReturnNode;
                    }
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_EXCLUSIVE_OR_EXPRESSION, 2, 3, pNewNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_BOOLEAN_AND_EXPRESSION, 0, 0, NULL);
                    return 0;
                }
                else
                {
                    if (nTopStackTerm == 3)
                    {
                        pTopStackCurrentNode->pRight = pTopStackReturnNode;
                        ModifySRStackReturnTree(pTopStackCurrentNode);
                    }
                    else
                    {
                        ModifySRStackReturnTree(pTopStackReturnNode);
                    }
                }
            }
            break;

        case CSCRIPTCOMPILER_GRAMMAR_INCLUSIVE_OR_EXPRESSION:
            if (nTopStackRule == 0 && nTopStackTerm == 0)
            {
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_INCLUSIVE_OR_EXPRESSION, 1, 1, NULL);
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_EXCLUSIVE_OR_EXPRESSION, 0, 0, NULL);
            }
            if ((nTopStackRule == 1 && nTopStackTerm == 1) || nTopStackTerm == 3)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_INCLUSIVE_OR)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_INCLUSIVE_OR, pTopStackReturnNode, NULL);
                    if (nTopStackTerm == 3)
                    {
                        pNewNode->pLeft = pTopStackCurrentNode;
                        pNewNode->pLeft->pRight = pTopStackReturnNode;
                    }
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_INCLUSIVE_OR_EXPRESSION, 2, 3, pNewNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_EXCLUSIVE_OR_EXPRESSION, 0, 0, NULL);
                    return 0;
                }
                else
                {
                    if (nTopStackTerm == 3)
                    {
                        pTopStackCurrentNode->pRight = pTopStackReturnNode;
                        ModifySRStackReturnTree(pTopStackCurrentNode);
                    }
                    else
                    {
                        ModifySRStackReturnTree(pTopStackReturnNode);
                    }
                }
            }
            break;

        case CSCRIPTCOMPILER_GRAMMAR_LOGICAL_AND_EXPRESSION:
            if (nTopStackRule == 0 && nTopStackTerm == 0)
            {
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_LOGICAL_AND_EXPRESSION, 1, 1, NULL);
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_INCLUSIVE_OR_EXPRESSION, 0, 0, NULL);
            }
            if ((nTopStackRule == 1 && nTopStackTerm == 1) || nTopStackTerm == 3)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_LOGICAL_AND)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_LOGICAL_AND, pTopStackReturnNode, NULL);
                    if (nTopStackTerm == 3)
                    {
                        pNewNode->pLeft = pTopStackCurrentNode;
                        pNewNode->pLeft->pRight = pTopStackReturnNode;
                    }
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_LOGICAL_AND_EXPRESSION, 2, 3, pNewNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_INCLUSIVE_OR_EXPRESSION, 0, 0, NULL);
                    return 0;
                }
                else
                {
                    if (nTopStackTerm == 3)
                    {
                        pTopStackCurrentNode->pRight = pTopStackReturnNode;
                        ModifySRStackReturnTree(pTopStackCurrentNode);
                    }
                    else
                    {
                        ModifySRStackReturnTree(pTopStackReturnNode);
                    }
                }
            }
            break;

        case CSCRIPTCOMPILER_GRAMMAR_LOGICAL_OR_EXPRESSION:
            if (nTopStackRule == 0 && nTopStackTerm == 0)
            {
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_LOGICAL_OR_EXPRESSION, 1, 1, NULL);
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_LOGICAL_AND_EXPRESSION, 0, 0, NULL);
            }
            if ((nTopStackRule == 1 && nTopStackTerm == 1) || nTopStackTerm == 3)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_LOGICAL_OR)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_LOGICAL_OR, pTopStackReturnNode, NULL);
                    if (nTopStackTerm == 3)
                    {
                        pNewNode->pLeft = pTopStackCurrentNode;
                        pNewNode->pLeft->pRight = pTopStackReturnNode;
                    }
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_LOGICAL_OR_EXPRESSION, 2, 3, pNewNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_LOGICAL_AND_EXPRESSION, 0, 0, NULL);
                    return 0;
                }
                else
                {
                    if (nTopStackTerm == 3)
                    {
                        pTopStackCurrentNode->pRight = pTopStackReturnNode;
                        ModifySRStackReturnTree(pTopStackCurrentNode);
                    }
                    else
                    {
                        ModifySRStackReturnTree(pTopStackReturnNode);
                    }
                }
            }
            break;

        case CSCRIPTCOMPILER_GRAMMAR_CONDITIONAL_EXPRESSION:
            if (nTopStackRule == 0 && nTopStackTerm == 0)
            {
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_CONDITIONAL_EXPRESSION, 1, 1, NULL);
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_LOGICAL_OR_EXPRESSION, 0, 0, NULL);
            }
            if (nTopStackRule == 1 && nTopStackTerm == 1)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_QUESTION_MARK)
                {
                    CScriptParseTreeNode* pNewNode2 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_COND_CONDITION, pTopStackReturnNode, NULL);
                    CScriptParseTreeNode* pNewNode3 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_COND_CHOICE, NULL, NULL);
                    CScriptParseTreeNode* pNewNode1 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_COND_BLOCK, pNewNode2, pNewNode3);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_CONDITIONAL_EXPRESSION, 2, 3, pNewNode1);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_CONDITIONAL_EXPRESSION, 0, 0, NULL);
                    return 0;
                }
                else
                {
                    ModifySRStackReturnTree(pTopStackReturnNode);
                }
            }
            if (nTopStackRule == 2 && nTopStackTerm == 3)
            {
                if (m_nTokenStatus != CSCRIPTCOMPILER_TOKEN_COLON)
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_CONDITIONAL_REQUIRES_SECOND_EXPRESSION;
                }
                pTopStackCurrentNode->pRight->pLeft = pTopStackReturnNode;
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_CONDITIONAL_EXPRESSION, 2, 5, pTopStackCurrentNode);
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_CONDITIONAL_EXPRESSION, 0, 0, NULL);
                return 0;
            }
            if (nTopStackRule == 2 && nTopStackTerm == 5)
            {
                pTopStackCurrentNode->pRight->pRight = pTopStackReturnNode;
                ModifySRStackReturnTree(pTopStackCurrentNode);
            }
            break;

        case CSCRIPTCOMPILER_GRAMMAR_ASSIGNMENT_EXPRESSION:
            if (nTopStackRule == 0 && nTopStackTerm == 0)
            {
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_ASSIGNMENT_EXPRESSION, 1, 1, NULL);
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_CONDITIONAL_EXPRESSION, 0, 0, NULL);
            }
            if (nTopStackRule == 1 && nTopStackTerm == 1)
            {
                if (m_nTokenStatus != CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_EQUAL &&
                    m_nTokenStatus != CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_MINUS &&
                    m_nTokenStatus != CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_PLUS &&
                    m_nTokenStatus != CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_MULTIPLY &&
                    m_nTokenStatus != CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_DIVIDE &&
                    m_nTokenStatus != CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_MODULUS &&
                    m_nTokenStatus != CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_AND &&
                    m_nTokenStatus != CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_XOR &&
                    m_nTokenStatus != CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_OR &&
                    m_nTokenStatus != CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_SHIFT_LEFT &&
                    m_nTokenStatus != CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_SHIFT_RIGHT &&
                    m_nTokenStatus != CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_USHIFT_RIGHT)
                {
                    ModifySRStackReturnTree(pTopStackReturnNode);
                }
                else
                {
                    if (CheckForBadLValue(pTopStackReturnNode) == TRUE)
                    {
                        return STRREF_CSCRIPTCOMPILER_ERROR_BAD_LVALUE;
                    }

                    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_EQUAL)
                    {
                        CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_ASSIGNMENT, NULL, pTopStackReturnNode);
                        PushSRStack(CSCRIPTCOMPILER_GRAMMAR_ASSIGNMENT_EXPRESSION, 2, 3, pNewNode);
                        PushSRStack(CSCRIPTCOMPILER_GRAMMAR_CONDITIONAL_EXPRESSION, 0, 0, NULL);
                    }
                    else
                    {

                        CScriptParseTreeNode* pDupTree = DuplicateScriptParseTree(pTopStackReturnNode);
                        int nNewTerm = 0;
                        CScriptParseTreeNode* pNewNode2 = NULL;

                        if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_MINUS)
                        {
                            pNewNode2 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_SUBTRACT, pDupTree, NULL);
                            nNewTerm = 3;
                        }
                        if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_PLUS)
                        {
                            pNewNode2 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_ADD, pDupTree, NULL);
                            nNewTerm = 4;
                        }
                        if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_MULTIPLY)
                        {
                            pNewNode2 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_MULTIPLY, pDupTree, NULL);
                            nNewTerm = 5;
                        }
                        if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_DIVIDE)
                        {
                            pNewNode2 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_DIVIDE, pDupTree, NULL);
                            nNewTerm = 6;
                        }
                        if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_MODULUS)
                        {
                            pNewNode2 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_MODULUS, pDupTree, NULL);
                            nNewTerm = 7;
                        }
                        if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_AND)
                        {
                            pNewNode2 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_BOOLEAN_AND, pDupTree, NULL);
                            nNewTerm = 8;
                        }
                        if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_XOR)
                        {
                            pNewNode2 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_EXCLUSIVE_OR, pDupTree, NULL);
                            nNewTerm = 9;
                        }
                        if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_OR)
                        {
                            pNewNode2 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_INCLUSIVE_OR, pDupTree, NULL);
                            nNewTerm = 10;
                        }
                        if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_SHIFT_LEFT)
                        {
                            pNewNode2 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_SHIFT_LEFT, pDupTree, NULL);
                            nNewTerm = 11;
                        }
                        if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_SHIFT_RIGHT)
                        {
                            pNewNode2 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_SHIFT_RIGHT, pDupTree, NULL);
                            nNewTerm = 12;
                        }
                        if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_USHIFT_RIGHT)
                        {
                            pNewNode2 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_UNSIGNED_SHIFT_RIGHT, pDupTree, NULL);
                            nNewTerm = 13;
                        }

                        CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_ASSIGNMENT, pNewNode2, pTopStackReturnNode);
                        PushSRStack(CSCRIPTCOMPILER_GRAMMAR_ASSIGNMENT_EXPRESSION, nNewTerm, 3, pNewNode);
                        PushSRStack(CSCRIPTCOMPILER_GRAMMAR_CONDITIONAL_EXPRESSION, 0, 0, NULL);
                    }
                    return 0;
                }
            }
            if (nTopStackTerm == 3)
            {

                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_LEFT_BRACKET)
                {
                    if (pTopStackReturnNode != NULL)
                    {
                        if (pTopStackReturnNode->nOperation == CSCRIPTCOMPILER_OPERATION_VARIABLE)
                        {
                            m_sUndefinedIdentifier = pTopStackReturnNode->m_psStringData;
                            return STRREF_CSCRIPTCOMPILER_ERROR_UNDEFINED_IDENTIFIER;
                        }
                    }
                }

                if (nTopStackRule == 2)
                {
                    pTopStackCurrentNode->pLeft = pTopStackReturnNode;
                }
                else
                {
                    pTopStackCurrentNode->pLeft->pRight = pTopStackReturnNode;
                }
                ModifySRStackReturnTree(pTopStackCurrentNode);
            }
            break;

        case CSCRIPTCOMPILER_GRAMMAR_EXPRESSION:
            if (nTopStackRule == 0 && nTopStackTerm == 0)
            {
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_EXPRESSION, 1, 1, NULL);
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_ASSIGNMENT_EXPRESSION, 0, 0, NULL);
            }
            if (nTopStackRule == 1 && nTopStackTerm == 1)
            {
                ModifySRStackReturnTree(pTopStackReturnNode);
            }
            break;

        case CSCRIPTCOMPILER_GRAMMAR_NON_VOID_EXPRESSION:
            if (nTopStackRule == 0 && nTopStackTerm == 0)
            {
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_NON_VOID_EXPRESSION, 1, 1, NULL);
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_EXPRESSION, 0, 0, NULL);
            }
            if (nTopStackRule == 1 && nTopStackTerm == 1)
            {

                CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_NON_VOID_EXPRESSION, pTopStackReturnNode, NULL);
                ModifySRStackReturnTree(pTopStackReturnNode);
            }
            break;

        case CSCRIPTCOMPILER_GRAMMAR_BOOLEAN_EXPRESSION:
            if (nTopStackRule == 0 && nTopStackTerm == 0)
            {
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_BOOLEAN_EXPRESSION, 1, 1, NULL);
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_NON_VOID_EXPRESSION, 0, 0, NULL);
            }
            if (nTopStackRule == 1 && nTopStackTerm == 1)
            {
                CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_INTEGER_EXPRESSION, pTopStackReturnNode, NULL);
                ModifySRStackReturnTree(pNewNode);
            }
            break;

        case CSCRIPTCOMPILER_GRAMMAR_AFTER_ARGUMENT_EXPRESSION:
            if (nTopStackRule == 0 && nTopStackTerm == 0)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_RIGHT_BRACKET)
                {
                    ModifySRStackReturnTree(NULL);
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_COMMA)
                {
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_AFTER_ARGUMENT_EXPRESSION, 2, 2, NULL);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_NON_VOID_EXPRESSION, 0, 0, NULL);
                    return 0;
                }
                else
                {
                    if (m_pSRStack[m_nSRStackStates].pCurrentTree != NULL)
                    {
                        if (m_pSRStack[m_nSRStackStates].pCurrentTree->pRight != NULL)
                        {
                            if (m_pSRStack[m_nSRStackStates].pCurrentTree->pRight->nOperation == CSCRIPTCOMPILER_OPERATION_VARIABLE)
                            {
                                m_sUndefinedIdentifier = m_pSRStack[m_nSRStackStates].pCurrentTree->pRight->m_psStringData;
                                return STRREF_CSCRIPTCOMPILER_ERROR_UNDEFINED_IDENTIFIER;
                            }
                        }
                    }
                    return STRREF_CSCRIPTCOMPILER_ERROR_UNKNOWN_STATE_IN_COMPILER;
                }
            }
            if (nTopStackRule == 2 && nTopStackTerm == 2)
            {
                CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_ACTION_ARG_LIST, NULL, pTopStackReturnNode);
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_AFTER_ARGUMENT_EXPRESSION, 2, 3, pNewNode);
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_AFTER_ARGUMENT_EXPRESSION, 0, 0, NULL);
            }
            if (nTopStackRule == 2 && nTopStackTerm == 3)
            {
                pTopStackCurrentNode->pLeft = pTopStackReturnNode;
                ModifySRStackReturnTree(pTopStackCurrentNode);
            }
            break;

        case CSCRIPTCOMPILER_GRAMMAR_ARGUMENT_EXPRESSION_LIST:
            if (nTopStackRule == 0 && nTopStackTerm == 0)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_RIGHT_BRACE)
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_UNKNOWN_STATE_IN_COMPILER;
                }
                else if (m_nTokenStatus != CSCRIPTCOMPILER_TOKEN_RIGHT_BRACKET)
                {
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_ARGUMENT_EXPRESSION_LIST, 2, 1, NULL);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_NON_VOID_EXPRESSION, 0, 0, NULL);
                }
            }
            if (nTopStackRule == 2 && nTopStackTerm == 1)
            {
                CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_ACTION_ARG_LIST, NULL, pTopStackReturnNode);
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_ARGUMENT_EXPRESSION_LIST, 2, 2, pNewNode);
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_AFTER_ARGUMENT_EXPRESSION, 0, 0, NULL);
            }
            if (nTopStackRule == 2 && nTopStackTerm == 2)
            {
                pTopStackCurrentNode->pLeft = pTopStackReturnNode;
                ModifySRStackReturnTree(pTopStackCurrentNode);
            }
            break;

        case CSCRIPTCOMPILER_GRAMMAR_DECL_VARLIST_SEPARATOR:
            if (nTopStackRule == 0 && nTopStackTerm == 0)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_SEMICOLON)
                {
                    ModifySRStackReturnTree(pTopStackCurrentNode);
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_COMMA)
                {

                    CScriptParseTreeNode* pNewNode3 = CreateScriptParseTreeNode(0, NULL, NULL);
                    if (pTopStackCurrentNode != NULL &&
                        pTopStackCurrentNode->pLeft != NULL &&
                        pTopStackCurrentNode->pLeft->pLeft != NULL)
                    {
                        pNewNode3->nOperation = pTopStackCurrentNode->pLeft->pLeft->nOperation;
                        if (pNewNode3->nOperation == CSCRIPTCOMPILER_OPERATION_KEYWORD_STRUCT)
                        {
                            pNewNode3->m_psStringData = pTopStackCurrentNode->pLeft->pLeft->m_psStringData;
                        }
                    }

                    CScriptParseTreeNode* pNewNode2 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_KEYWORD_DECLARATION, pNewNode3, NULL);
                    CScriptParseTreeNode* pNewNode1 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_STATEMENT, pNewNode2, NULL);
                    pTopStackCurrentNode->pRight = pNewNode1;
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_DECL_VARLIST, 0, 0, pNewNode1);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_PARSING_VARIABLE_LIST;
                }
            }
            else
            {
                return STRREF_CSCRIPTCOMPILER_ERROR_PARSING_VARIABLE_LIST;
            }
            break;

        case CSCRIPTCOMPILER_GRAMMAR_DECL_VARLIST:
            if (nTopStackRule == 0 && nTopStackTerm == 0)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_VARIABLE)
                {
                    CScriptParseTreeNode* pNewNode2 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_VARIABLE, NULL, NULL);
                    m_pchToken[m_nTokenCharacters] = 0;
                    pNewNode2->m_psStringData.Format("%s", m_pchToken);

                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_VARIABLE_LIST, pNewNode2, NULL);

                    if (pTopStackCurrentNode != NULL &&
                        pTopStackCurrentNode->pLeft != NULL)
                    {
                        pTopStackCurrentNode->pLeft->pRight = pNewNode;
                    }
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_DECL_VARLIST, 1, 2, pTopStackCurrentNode);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_PARSING_VARIABLE_LIST;
                }
            }
            else if (nTopStackRule == 1 && nTopStackTerm == 2)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_EQUAL)
                {

                    CScriptParseTreeNode* pNewNode0 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_VARIABLE, NULL, NULL);
                    if (pTopStackCurrentNode != NULL &&
                        pTopStackCurrentNode->pLeft != NULL &&
                        pTopStackCurrentNode->pLeft->pRight != NULL &&
                        pTopStackCurrentNode->pLeft->pRight->pLeft != NULL)
                    {
                        pNewNode0->m_psStringData.Format("%s", pTopStackCurrentNode->pLeft->pRight->pLeft->m_psStringData.CStr());
                    }

                    CScriptParseTreeNode* pNewNode1 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_ASSIGNMENT, NULL, pNewNode0);
                    CScriptParseTreeNode* pNewNode2 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_STATEMENT, pNewNode1, NULL);
                    pTopStackCurrentNode->pRight = pNewNode2;

                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_DECL_VARLIST, 2, 2, pNewNode2);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_NON_VOID_EXPRESSION, 0, 0, NULL);
                    return 0;
                }
                else
                {
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_DECL_VARLIST_SEPARATOR, 0, 0, pTopStackCurrentNode);
                }
            }
            else if (nTopStackRule == 2 && nTopStackTerm == 2)
            {
                pTopStackCurrentNode->pLeft->pLeft = pTopStackReturnNode;
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_DECL_VARLIST_SEPARATOR, 0, 0, pTopStackCurrentNode);
            }
            break;

        case CSCRIPTCOMPILER_GRAMMAR_NON_VOID_TYPE_SPECIFIER:
            if (nTopStackRule == 0 && nTopStackTerm == 0)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_VECTOR)
                {

                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_KEYWORD_STRUCT, NULL, NULL);
                    m_pchToken[m_nTokenCharacters] = 0;
                    pNewNode->m_psStringData.Format("%s", m_pchToken);
                    ModifySRStackReturnTree(pNewNode);
                    return 0;
                }
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_KEYWORD_STRUCT, NULL, NULL);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_NON_VOID_TYPE_SPECIFIER, 5, 1, pNewNode);
                    return 0;
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_INT ||
                         m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT ||
                         m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRING ||
                         m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_OBJECT ||
                         (m_nTokenStatus >= CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE0 &&
                          m_nTokenStatus <= CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE9))
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(0, NULL, NULL);
                    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_INT)
                    {
                        pNewNode->nOperation = CSCRIPTCOMPILER_OPERATION_KEYWORD_INT;
                    }
                    else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT)
                    {
                        pNewNode->nOperation = CSCRIPTCOMPILER_OPERATION_KEYWORD_FLOAT;
                    }
                    else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRING)
                    {
                        pNewNode->nOperation = CSCRIPTCOMPILER_OPERATION_KEYWORD_STRING;
                    }
                    else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_OBJECT)
                    {
                        pNewNode->nOperation = CSCRIPTCOMPILER_OPERATION_KEYWORD_OBJECT;
                    }
                    else if (m_nTokenStatus >= CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE0 &&
                             m_nTokenStatus <= CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE9)
                    {
                        pNewNode->nOperation = CSCRIPTCOMPILER_OPERATION_KEYWORD_ENGINE_STRUCTURE0 +
                                               (m_nTokenStatus - CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE0);
                    }
                    ModifySRStackReturnTree(pNewNode);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_INVALID_DECLARATION_TYPE;
                }
            }
            if (nTopStackRule == 5 && nTopStackTerm == 1)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_VARIABLE)
                {
                    m_pchToken[m_nTokenCharacters] = 0;
                    pTopStackCurrentNode->m_psStringData.Format("%s", m_pchToken);
                    ModifySRStackReturnTree(pTopStackCurrentNode);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_INVALID_DECLARATION_TYPE;
                }
            }
            break;

        case CSCRIPTCOMPILER_GRAMMAR_ANY_TYPE_SPECIFIER:
            if (nTopStackRule == 0 && nTopStackTerm == 0)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_VOID)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_KEYWORD_VOID, NULL, NULL);
                    ModifySRStackReturnTree(pNewNode);
                    return 0;
                }
                else
                {
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_NON_VOID_TYPE_SPECIFIER, 0, 0, NULL);
                }
            }
            break;

        case CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT:
            if (nTopStackRule == 0 && nTopStackTerm == 0)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_LEFT_BRACE)
                {
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 1, 2, NULL);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_COMPOUND_STATEMENT, 0, 0, NULL);
                    return 0;
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_RETURN)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_RETURN, NULL, NULL);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 6, 2, pNewNode);
                    return 0;
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_SWITCH)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_SWITCH_BLOCK, NULL, NULL);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 5, 2, pNewNode);
                    return 0;
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_DEFAULT)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_DEFAULT, NULL, NULL);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 12, 2, pNewNode);
                    return 0;
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_CASE)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_CASE, NULL, NULL);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 13, 3, pNewNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_CONDITIONAL_EXPRESSION, 0, 0, NULL);
                    return 0;
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_BREAK)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_BREAK, NULL, NULL);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 14, 2, pNewNode);
                    return 0;
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_CONTINUE)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_CONTINUE, NULL, NULL);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 15, 2, pNewNode);
                    return 0;
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_VOID_IDENTIFIER)
                {
                    CScriptParseTreeNode* pNewNode2 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_ACTION_ID, NULL, NULL);
                    m_pchToken[m_nTokenCharacters] = 0;
                    pNewNode2->m_psStringData.Format("%s", m_pchToken);

                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_ACTION, NULL, pNewNode2);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 2, 2, pNewNode);
                    return 0;
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_IF)
                {
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 3, 2, NULL);
                    return 0;
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_ELSE)
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_ELSE_WITHOUT_CORRESPONDING_IF;
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_WHILE)
                {
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 7, 2, NULL);
                    return 0;
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_DO)
                {
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 8, 2, NULL);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 0, 0, NULL);
                    return 0;
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_FOR)
                {
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 9, 2, NULL);
                    return 0;
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_INT ||
                         m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT ||
                         m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRING ||
                         m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_OBJECT ||
                         m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT ||
                         m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_VECTOR ||
                         (m_nTokenStatus >= CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE0 &&
                          m_nTokenStatus <= CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE9))
                {
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 10, 2, NULL);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_NON_VOID_TYPE_SPECIFIER, 0, 0, NULL);
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_SEMICOLON)
                {
                    ModifySRStackReturnTree(pTopStackCurrentNode);
                    return 0;
                }
                else
                {
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 11, 2, NULL);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_EXPRESSION, 0, 0, NULL);
                }
            }
            if (nTopStackRule == 1 && nTopStackTerm == 2)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_RIGHT_BRACE)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_COMPOUND_STATEMENT, pTopStackReturnNode, NULL);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 1, 3, pNewNode);
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_UNEXPECTED_END_COMPOUND_STATEMENT;
                }
            }
            if (nTopStackRule == 1 && nTopStackTerm == 3)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_RIGHT_BRACE)
                {
                    ModifySRStackReturnTree(pTopStackCurrentNode);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_UNEXPECTED_END_COMPOUND_STATEMENT;
                }
            }
            if (nTopStackRule == 2 && nTopStackTerm == 2)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_LEFT_BRACKET)
                {
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 2, 3, pTopStackCurrentNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_ARGUMENT_EXPRESSION_LIST, 0, 0, NULL);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_NO_LEFT_BRACKET_ON_ARG_LIST;
                }
            }
            if (nTopStackRule == 2 && nTopStackTerm == 3)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_RIGHT_BRACKET)
                {
                    pTopStackCurrentNode->pLeft = pTopStackReturnNode;
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 2, 4, pTopStackCurrentNode);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_NO_RIGHT_BRACKET_ON_ARG_LIST;
                }
            }
            if (nTopStackRule == 2 && nTopStackTerm == 4)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_SEMICOLON)
                {
                    ModifySRStackReturnTree(pTopStackCurrentNode);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_NO_SEMICOLON_AFTER_EXPRESSION;
                }
            }
            if (nTopStackRule == 3 && nTopStackTerm == 2)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_LEFT_BRACKET)
                {
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 3, 3, NULL);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_BOOLEAN_EXPRESSION, 0, 0, NULL);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_NO_LEFT_BRACKET_ON_EXPRESSION;
                }
            }
            if (nTopStackRule == 3 && nTopStackTerm == 3)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_RIGHT_BRACKET)
                {
                    CScriptParseTreeNode* pNewNode2 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_IF_CONDITION, pTopStackReturnNode, NULL);
                    CScriptParseTreeNode* pNewNode3 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_IF_CHOICE, NULL, NULL);
                    CScriptParseTreeNode* pNewNode1 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_IF_BLOCK, pNewNode2, pNewNode3);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 3, 5, pNewNode1);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 0, 0, NULL);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_NO_RIGHT_BRACKET_ON_EXPRESSION;
                }
            }
            if (nTopStackRule == 3 && nTopStackTerm == 5)
            {

                if (pTopStackReturnNode == NULL)
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_UNEXPECTED_CHARACTER;
                }

                CScriptParseTreeNode* pNewNode2 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_STATEMENT, pTopStackReturnNode, NULL);
                pTopStackCurrentNode->pRight->pLeft = pNewNode2;

                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_ELSE)
                {
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 3, 7, pTopStackCurrentNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 0, 0, NULL);
                    return 0;
                }
                else
                {
                    ModifySRStackReturnTree(pTopStackCurrentNode);
                }
            }
            if (nTopStackRule == 3 && nTopStackTerm == 7)
            {
                CScriptParseTreeNode* pNewNode2 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_STATEMENT, pTopStackReturnNode, NULL);
                pTopStackCurrentNode->pRight->pRight = pNewNode2;
                ModifySRStackReturnTree(pTopStackCurrentNode);
            }
            if (nTopStackRule == 5 && nTopStackTerm == 2)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_LEFT_BRACKET)
                {
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 5, 4, pTopStackCurrentNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_BOOLEAN_EXPRESSION, 0, 0, NULL);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_NO_LEFT_BRACKET_ON_EXPRESSION;
                }
            }
            if (nTopStackRule == 5 && nTopStackTerm == 4)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_RIGHT_BRACKET)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_SWITCH_CONDITION, pTopStackReturnNode, NULL);
                    pTopStackCurrentNode->pLeft = pNewNode;
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 5, 5, pTopStackCurrentNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 0, 0, NULL);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_NO_RIGHT_BRACKET_ON_EXPRESSION;
                }
            }
            if (nTopStackRule == 5 && nTopStackTerm == 5)
            {
                CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_STATEMENT, pTopStackReturnNode, NULL);
                pTopStackCurrentNode->pRight = pNewNode;
                ModifySRStackReturnTree(pTopStackCurrentNode);
            }
            if (nTopStackRule == 6 && nTopStackTerm == 2)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_SEMICOLON)
                {
                    ModifySRStackReturnTree(pTopStackCurrentNode);
                    return 0;
                }
                else
                {
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 6, 3, pTopStackCurrentNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_NON_VOID_EXPRESSION, 0, 0, NULL);
                }
            }
            if (nTopStackRule == 6 && nTopStackTerm == 3)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_SEMICOLON)
                {
                    pTopStackCurrentNode->pRight = pTopStackReturnNode;
                    ModifySRStackReturnTree(pTopStackCurrentNode);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_PARSING_RETURN_STATEMENT;
                }
            }
            if (nTopStackRule == 7 && nTopStackTerm == 2)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_LEFT_BRACKET)
                {
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 7, 3, NULL);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_BOOLEAN_EXPRESSION, 0, 0, NULL);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_NO_LEFT_BRACKET_ON_EXPRESSION;
                }
            }
            if (nTopStackRule == 7 && nTopStackTerm == 3)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_RIGHT_BRACKET)
                {
                    CScriptParseTreeNode* pNewNode2 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_WHILE_CONDITION, pTopStackReturnNode, NULL);
                    CScriptParseTreeNode* pNewNode3 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_WHILE_CHOICE, NULL, NULL);
                    CScriptParseTreeNode* pNewNode1 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_WHILE_BLOCK, pNewNode2, pNewNode3);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 7, 5, pNewNode1);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 0, 0, NULL);
                    return 0;
                }
                else
                {

                    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_LEFT_BRACKET)
                    {
                        if (pTopStackReturnNode != NULL && pTopStackReturnNode->pLeft != NULL)
                        {
                            if (pTopStackReturnNode->pLeft->nOperation == CSCRIPTCOMPILER_OPERATION_VARIABLE)
                            {
                                m_sUndefinedIdentifier = pTopStackReturnNode->pLeft->m_psStringData;
                                return STRREF_CSCRIPTCOMPILER_ERROR_UNDEFINED_IDENTIFIER;
                            }
                        }
                    }
                    return STRREF_CSCRIPTCOMPILER_ERROR_NO_RIGHT_BRACKET_ON_EXPRESSION;
                }
            }
            if (nTopStackRule == 7 && nTopStackTerm == 5)
            {

                CScriptParseTreeNode* pNewNode4 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_WHILE_CONTINUE, NULL, NULL);
                CScriptParseTreeNode* pNewNode3 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_STATEMENT, pNewNode4, NULL);
                CScriptParseTreeNode* pNewNode2 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_STATEMENT, pTopStackReturnNode, pNewNode3);
                pTopStackCurrentNode->pRight->pLeft = pNewNode2;
                ModifySRStackReturnTree(pTopStackCurrentNode);
            }
            if (nTopStackRule == 8 && nTopStackTerm == 2)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_WHILE)
                {
                    CScriptParseTreeNode* pNewNode2 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_STATEMENT, pTopStackReturnNode, NULL);
                    CScriptParseTreeNode* pNewNode1 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_DOWHILE_CONDITION, NULL, NULL);
                    CScriptParseTreeNode* pNewNode3 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_DOWHILE_BLOCK, pNewNode2, pNewNode1);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 8, 3, pNewNode3);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_NO_WHILE_AFTER_DO_KEYWORD;
                }
            }
            if (nTopStackRule == 8 && nTopStackTerm == 3)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_LEFT_BRACKET)
                {
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 8, 5, pTopStackCurrentNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_BOOLEAN_EXPRESSION, 0, 0, NULL);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_NO_LEFT_BRACKET_ON_EXPRESSION;
                }
            }
            if (nTopStackRule == 8 && nTopStackTerm == 5)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_RIGHT_BRACKET)
                {
                    pTopStackCurrentNode->pRight->pLeft = pTopStackReturnNode;
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 8, 6, pTopStackCurrentNode);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_NO_RIGHT_BRACKET_ON_EXPRESSION;
                }
            }
            if (nTopStackRule == 8 && nTopStackTerm == 6)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_SEMICOLON)
                {
                    ModifySRStackReturnTree(pTopStackCurrentNode);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_NO_SEMICOLON_AFTER_EXPRESSION;
                }
            }
            if (nTopStackRule == 9 && nTopStackTerm == 2)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_LEFT_BRACKET)
                {

                    CScriptParseTreeNode* pNewNode8 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_WHILE_CONTINUE, NULL, NULL);
                    CScriptParseTreeNode* pNewNode9 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_STATEMENT, NULL, NULL);
                    CScriptParseTreeNode* pNewNode1 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_STATEMENT, NULL, NULL);
                    CScriptParseTreeNode* pNewNode2 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_STATEMENT, pNewNode8, pNewNode9);
                    CScriptParseTreeNode* pNewNode3 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_WHILE_CHOICE, pNewNode1, pNewNode2);
                    CScriptParseTreeNode* pNewNode4 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_WHILE_CONDITION, NULL, NULL);
                    CScriptParseTreeNode* pNewNode5 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_WHILE_BLOCK, pNewNode4, pNewNode3);
                    CScriptParseTreeNode* pNewNode6 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_STATEMENT, pNewNode5, NULL);
                    CScriptParseTreeNode* pNewNode7 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_STATEMENT, NULL, pNewNode6);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 9, 3, pNewNode7);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_NO_LEFT_BRACKET_ON_EXPRESSION;
                }
            }
            if (nTopStackRule == 9 && nTopStackTerm == 3)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_SEMICOLON)
                {
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 9, 5, pTopStackCurrentNode);
                    return 0;
                }
                else
                {
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 9, 4, pTopStackCurrentNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_BOOLEAN_EXPRESSION, 0, 0, NULL);
                }
            }
            if (nTopStackRule == 9 && nTopStackTerm == 4)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_SEMICOLON)
                {
                    pTopStackCurrentNode->pLeft = pTopStackReturnNode;
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 9, 5, pTopStackCurrentNode);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_NO_SEMICOLON_AFTER_EXPRESSION;
                }
            }
            if (nTopStackRule == 9 && nTopStackTerm == 5)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_SEMICOLON)
                {

                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_CONSTANT_INTEGER, NULL, NULL);
                    pNewNode->nIntegerData = 1;
                    CScriptParseTreeNode* pNewNode2 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_NON_VOID_EXPRESSION, pNewNode, NULL);
                    CScriptParseTreeNode* pNewNode3 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_INTEGER_EXPRESSION, pNewNode2, NULL);
                    pTopStackCurrentNode->pRight->pLeft->pLeft->pLeft = pNewNode3;
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 9, 7, pTopStackCurrentNode);
                    return 0;
                }
                else
                {
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 9, 6, pTopStackCurrentNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_BOOLEAN_EXPRESSION, 0, 0, NULL);
                }
            }
            if (nTopStackRule == 9 && nTopStackTerm == 6)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_SEMICOLON)
                {
                    pTopStackCurrentNode->pRight->pLeft->pLeft->pLeft = pTopStackReturnNode;
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 9, 7, pTopStackCurrentNode);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_NO_SEMICOLON_AFTER_EXPRESSION;
                }
            }
            if (nTopStackRule == 9 && nTopStackTerm == 7)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_RIGHT_BRACKET)
                {
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 9, 9, pTopStackCurrentNode);
                    return 0;
                }
                else
                {
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 9, 8, pTopStackCurrentNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_BOOLEAN_EXPRESSION, 0, 0, NULL);
                }
            }
            if (nTopStackRule == 9 && nTopStackTerm == 8)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_RIGHT_BRACKET)
                {
                    pTopStackCurrentNode->pRight->pLeft->pRight->pRight->pRight->pLeft = pTopStackReturnNode;
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 9, 9, pTopStackCurrentNode);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_NO_SEMICOLON_AFTER_EXPRESSION;
                }
            }
            if (nTopStackRule == 9 && nTopStackTerm == 9)
            {
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 9, 10, pTopStackCurrentNode);
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 0, 0, NULL);
            }
            if (nTopStackRule == 9 && nTopStackTerm == 10)
            {
                pTopStackCurrentNode->pRight->pLeft->pRight->pLeft->pLeft = pTopStackReturnNode;
                ModifySRStackReturnTree(pTopStackCurrentNode);
            }
            if (nTopStackRule == 10 && nTopStackTerm == 2)
            {
                CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_KEYWORD_DECLARATION, pTopStackReturnNode, NULL);
                CScriptParseTreeNode* pNewNode2 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_STATEMENT, pNewNode, NULL);
                CScriptParseTreeNode* pNewNode3 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_STATEMENT_LIST, pNewNode2, NULL);
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 10, 3, pNewNode3);
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_DECL_VARLIST, 0, 0, pNewNode2);
            }
            if (nTopStackRule == 10 && nTopStackTerm == 3)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_SEMICOLON)
                {
                    ModifySRStackReturnTree(pTopStackCurrentNode);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_NO_SEMICOLON_AFTER_EXPRESSION;
                }
            }
            if (nTopStackRule == 11 && nTopStackTerm == 2)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_SEMICOLON)
                {
                    ModifySRStackReturnTree(pTopStackReturnNode);
                    return 0;
                }
                else
                {
                    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_LEFT_BRACKET)
                    {
                        if (pTopStackReturnNode != NULL)
                        {
                            if (pTopStackReturnNode->nOperation == CSCRIPTCOMPILER_OPERATION_VARIABLE)
                            {
                                m_sUndefinedIdentifier = pTopStackReturnNode->m_psStringData;
                                return STRREF_CSCRIPTCOMPILER_ERROR_UNDEFINED_IDENTIFIER;
                            }
                        }
                    }
                    return STRREF_CSCRIPTCOMPILER_ERROR_NO_SEMICOLON_AFTER_EXPRESSION;
                }
            }
            if (nTopStackRule == 12 && nTopStackTerm == 2)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_COLON)
                {
                    ModifySRStackReturnTree(pTopStackCurrentNode);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_NO_COLON_AFTER_DEFAULT_LABEL;
                }
            }
            if (nTopStackRule == 13 && nTopStackTerm == 3)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_COLON)
                {
                    pTopStackCurrentNode->pLeft = pTopStackReturnNode;
                    ModifySRStackReturnTree(pTopStackCurrentNode);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_NO_COLON_AFTER_CASE_LABEL;
                }
            }
            if ((nTopStackRule == 14 && nTopStackTerm == 2) ||
                (nTopStackRule == 15 && nTopStackTerm == 2))
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_SEMICOLON)
                {
                    ModifySRStackReturnTree(pTopStackCurrentNode);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_NO_SEMICOLON_AFTER_STATEMENT;
                }
            }
            break;

        case CSCRIPTCOMPILER_GRAMMAR_WITHIN_STATEMENT_LIST:
            if (nTopStackRule == 0 && nTopStackTerm == 0)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_RIGHT_BRACE)
                {
                    ModifySRStackReturnTree(NULL);
                }
                else
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_STATEMENT, NULL, NULL);
                    if (pTopStackCurrentNode != NULL)
                    {
                        pTopStackCurrentNode->pRight = pNewNode;
                    }
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_STATEMENT_LIST, 1, 1, pNewNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 0, 0, NULL);
                }
            }
            if (nTopStackRule == 1 && nTopStackTerm == 1)
            {
                pTopStackCurrentNode->pLeft = pTopStackReturnNode;
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_STATEMENT_LIST, 1, 2, pTopStackCurrentNode);
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_STATEMENT_LIST, 0, 0, pTopStackCurrentNode);
            }
            if (nTopStackRule == 1 && nTopStackTerm == 2)
            {
                ModifySRStackReturnTree(pTopStackCurrentNode);
            }
            break;

        case CSCRIPTCOMPILER_GRAMMAR_WITHIN_COMPOUND_STATEMENT:
            if (nTopStackRule == 0 && nTopStackTerm == 0)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_RIGHT_BRACE)
                {
                    ModifySRStackReturnTree(pTopStackCurrentNode);
                }
                else
                {
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_COMPOUND_STATEMENT, 2, 1, NULL);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_STATEMENT_LIST, 0, 0, NULL);
                }
            }
            if (nTopStackRule == 2 && nTopStackTerm == 1)
            {
                CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_STATEMENT_LIST, pTopStackReturnNode, NULL);
                ModifySRStackReturnTree(pNewNode);
            }
            break;

        case CSCRIPTCOMPILER_GRAMMAR_NON_INIT_DECL_VARLIST_SEPARATOR:
            if (nTopStackRule == 0 && nTopStackTerm == 0)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_SEMICOLON)
                {
                    ModifySRStackReturnTree(pTopStackCurrentNode);
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_COMMA)
                {
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_NON_INIT_DECL_VARLIST, 0, 0, pTopStackCurrentNode);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_PARSING_VARIABLE_LIST;
                }
            }
            else
            {
                return STRREF_CSCRIPTCOMPILER_ERROR_PARSING_VARIABLE_LIST;
            }
            break;

        case CSCRIPTCOMPILER_GRAMMAR_NON_INIT_DECL_VARLIST:
            if (nTopStackRule == 0 && nTopStackTerm == 0)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_VARIABLE)
                {
                    CScriptParseTreeNode* pNewNode2 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_VARIABLE, NULL, NULL);
                    m_pchToken[m_nTokenCharacters] = 0;
                    pNewNode2->m_psStringData.Format("%s", m_pchToken);

                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_VARIABLE_LIST, pNewNode2, NULL);

                    if (pTopStackCurrentNode != NULL)
                    {
                        pTopStackCurrentNode->pRight = pNewNode;
                    }
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_NON_INIT_DECL_VARLIST, 1, 2, pNewNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_NON_INIT_DECL_VARLIST_SEPARATOR, 0, 0, pNewNode);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_PARSING_VARIABLE_LIST;
                }
            }
            else if (nTopStackRule == 1 && nTopStackTerm == 2)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_SEMICOLON)
                {
                    ModifySRStackReturnTree(pTopStackCurrentNode);
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_PARSING_VARIABLE_LIST;
                }
            }
            break;

        case CSCRIPTCOMPILER_GRAMMAR_NON_INIT_DECL_LIST:
            if (nTopStackRule == 0 && nTopStackTerm == 0)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_INT ||
                    m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT ||
                    m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRING ||
                    m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_OBJECT ||
                    m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT ||
                    m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_VECTOR ||
                    (m_nTokenStatus >= CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE0 &&
                     m_nTokenStatus <= CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE9))
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_STATEMENT, NULL, NULL);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_NON_INIT_DECL_LIST, 2, 2, pNewNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_NON_VOID_TYPE_SPECIFIER, 0, 0, NULL);
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_RIGHT_BRACE)
                {
                    ModifySRStackReturnTree(pTopStackCurrentNode);
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_BAD_TYPE_SPECIFIER;
                }
            }
            if (nTopStackRule == 2 && nTopStackTerm == 2)
            {
                CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_KEYWORD_DECLARATION, pTopStackReturnNode, NULL);
                pTopStackCurrentNode->pLeft = pNewNode;
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_NON_INIT_DECL_LIST, 2, 3, pTopStackCurrentNode);
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_NON_INIT_DECL_VARLIST, 0, 0, NULL);
            }
            if (nTopStackRule == 2 && nTopStackTerm == 3)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_SEMICOLON)
                {
                    pTopStackCurrentNode->pLeft->pRight = pTopStackReturnNode;
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_NON_INIT_DECL_LIST, 2, 4, pTopStackCurrentNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_NON_INIT_DECL_LIST, 0, 0, NULL);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_NO_SEMICOLON_AFTER_EXPRESSION;
                }
            }
            if (nTopStackRule == 2 && nTopStackTerm == 4)
            {
                pTopStackCurrentNode->pRight = pTopStackReturnNode;
                ModifySRStackReturnTree(pTopStackCurrentNode);
            }
            break;

        case CSCRIPTCOMPILER_GRAMMAR_AFTER_FUNCTION_PARAM:
            if (nTopStackRule == 0 && nTopStackTerm == 0)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_RIGHT_BRACKET)
                {
                    ModifySRStackReturnTree(NULL);
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_COMMA)
                {
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_FUNCTION_PARAM_LIST, 0, 0, NULL);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_UNKNOWN_STATE_IN_COMPILER;
                }
            }
            break;

        case CSCRIPTCOMPILER_GRAMMAR_FUNCTION_PARAM_LIST:
            if (nTopStackRule == 0 && nTopStackTerm == 0)
            {
                if (m_nTokenStatus != CSCRIPTCOMPILER_TOKEN_RIGHT_BRACKET)
                {
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_FUNCTION_PARAM_LIST, 1, 2, NULL);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_NON_VOID_TYPE_SPECIFIER, 0, 0, NULL);
                }
            }
            if (nTopStackRule == 1 && nTopStackTerm == 2)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_VARIABLE)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_FUNCTION_PARAM_NAME, NULL, pTopStackReturnNode);
                    m_pchToken[m_nTokenCharacters] = 0;
                    pNewNode->m_psStringData.Format("%s", m_pchToken);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_FUNCTION_PARAM_LIST, 1, 3, pNewNode);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_BAD_VARIABLE_NAME;
                }
            }
            else if (nTopStackRule == 1 && nTopStackTerm == 3)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_EQUAL)
                {
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_FUNCTION_PARAM_LIST, 2, 4, pTopStackCurrentNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_PRIMARY_EXPRESSION, 0, 0, NULL);
                    return 0;
                }
                else
                {
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_FUNCTION_PARAM_LIST, 0, 1, pTopStackCurrentNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_AFTER_FUNCTION_PARAM, 0, 0, NULL);
                }
            }
            else if (nTopStackRule == 2 && nTopStackTerm == 4)
            {

                pTopStackCurrentNode->pRight->pRight = pTopStackReturnNode;
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_FUNCTION_PARAM_LIST, 0, 1, pTopStackCurrentNode);
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_AFTER_FUNCTION_PARAM, 0, 0, NULL);
            }

            if (nTopStackRule == 0 && nTopStackTerm == 1)
            {
                pTopStackCurrentNode->pLeft = pTopStackReturnNode;
                ModifySRStackReturnTree(pTopStackCurrentNode);
            }
            break;

        case CSCRIPTCOMPILER_GRAMMAR_AFTER_PROGRAM:
            if (nTopStackRule == 0 && nTopStackTerm == 0)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_EOF)
                {
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_AFTER_PROGRAM, 1, 1, pTopStackReturnNode);
                    ModifySRStackReturnTree(pTopStackReturnNode);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_AFTER_COMPOUND_STATEMENT_AT_END;
                }
            }
            if (nTopStackRule == 1 && nTopStackTerm == 1)
            {
                ModifySRStackReturnTree(pTopStackReturnNode);
            }
            break;

        case CSCRIPTCOMPILER_GRAMMAR_FUNCTIONAL_UNIT:
            if (nTopStackRule == 0 && nTopStackTerm == 0)
            {
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_FUNCTIONAL_UNIT, 0, 2, NULL);
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_ANY_TYPE_SPECIFIER, 0, 0, NULL);
            }
            if (nTopStackRule == 0 && nTopStackTerm == 2)
            {
                if (pTopStackReturnNode->nOperation == CSCRIPTCOMPILER_OPERATION_KEYWORD_STRUCT &&
                    m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_LEFT_BRACE)
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_STRUCTURE_DEFINITION, pTopStackReturnNode, NULL);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_FUNCTIONAL_UNIT, 3, 4, pNewNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_NON_INIT_DECL_LIST, 0, 0, NULL);
                    return 0;
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_VARIABLE ||
                         m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_INTEGER_IDENTIFIER ||
                         m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_FLOAT_IDENTIFIER ||
                         m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_STRING_IDENTIFIER ||
                         m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_OBJECT_IDENTIFIER ||
                         m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_STRUCTURE_IDENTIFIER ||
                         m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_VOID_IDENTIFIER ||
                         (m_nTokenStatus >= CSCRIPTCOMPILER_TOKEN_ENGINE_STRUCTURE0_IDENTIFIER &&
                          m_nTokenStatus <= CSCRIPTCOMPILER_TOKEN_ENGINE_STRUCTURE9_IDENTIFIER))
                {
                    CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_FUNCTION_IDENTIFIER, pTopStackReturnNode, NULL);
                    m_pchToken[m_nTokenCharacters] = 0;
                    pNewNode->m_psStringData.Format("%s", m_pchToken);

                    CScriptParseTreeNode* pNewNode2 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_FUNCTION_DECLARATION, pNewNode, NULL);
                    CScriptParseTreeNode* pNewNode3 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_FUNCTION, pNewNode2, NULL);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_FUNCTIONAL_UNIT, 0, 3, pNewNode3);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_FUNCTION_DEFINITION_MISSING_NAME;
                }
            }
            if (nTopStackRule == 0 && nTopStackTerm == 3)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_LEFT_BRACKET)
                {
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_FUNCTIONAL_UNIT, 0, 5, pTopStackCurrentNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_FUNCTION_PARAM_LIST, 0, 0, NULL);
                    return 0;
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_SEMICOLON ||
                         m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_COMMA ||
                         m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_EQUAL)
                {

                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_FUNCTIONAL_UNIT, 4, 0, pTopStackCurrentNode);
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_FUNCTION_DEFINITION_MISSING_PARAMETER_LIST;
                }
            }
            if (nTopStackRule == 0 && nTopStackTerm == 5)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_RIGHT_BRACKET)
                {
                    pTopStackCurrentNode->pLeft->pRight = pTopStackReturnNode;
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_FUNCTIONAL_UNIT, 0, 6, pTopStackCurrentNode);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_MALFORMED_PARAMETER_LIST;
                }
            }
            if (nTopStackRule == 0 && nTopStackTerm == 6)
            {

                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_SEMICOLON)
                {
                    int nReturnValue = AddUserDefinedIdentifier(pTopStackCurrentNode, FALSE);
                    ModifySRStackReturnTree(pTopStackCurrentNode);
                    return nReturnValue;
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_LEFT_BRACE)
                {
                    int nReturnValue = AddUserDefinedIdentifier(pTopStackCurrentNode, TRUE);
                    if (nReturnValue < 0)
                    {
                        return nReturnValue;
                    }

                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_FUNCTIONAL_UNIT, 2, 8, pTopStackCurrentNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_A_STATEMENT, 1, 2, NULL);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_WITHIN_COMPOUND_STATEMENT, 0, 0, NULL);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_UNKNOWN_STATE_IN_COMPILER;
                }
            }
            if (nTopStackRule == 2 && nTopStackTerm == 8)
            {
                pTopStackCurrentNode->pRight = pTopStackReturnNode;
                ModifySRStackReturnTree(pTopStackCurrentNode);
            }
            if (nTopStackRule == 3 && nTopStackTerm == 4)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_RIGHT_BRACE)
                {
                    pTopStackCurrentNode->pRight = pTopStackReturnNode;
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_FUNCTIONAL_UNIT, 3, 5, pTopStackCurrentNode);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_BAD_TYPE_SPECIFIER;
                }
            }
            if (nTopStackRule == 3 && nTopStackTerm == 5)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_SEMICOLON)
                {
                    AddToGlobalVariableList(pTopStackCurrentNode);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_NO_SEMICOLON_AFTER_STRUCTURE;
                }
            }
            if (nTopStackRule == 4 && nTopStackTerm == 0)
            {

                CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_KEYWORD_DECLARATION, NULL, NULL);
                CScriptParseTreeNode* pNewNode2 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_STATEMENT, pNewNode, NULL);
                CScriptParseTreeNode* pNewNode3 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_STATEMENT_LIST, pNewNode2, NULL);

                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_FUNCTIONAL_UNIT, 4, 3, pNewNode3);

                pNewNode->pLeft = pTopStackCurrentNode->pLeft->pLeft->pLeft;
                pTopStackCurrentNode->pLeft->pLeft->pLeft = NULL;

                if (pNewNode->pLeft->nOperation == CSCRIPTCOMPILER_OPERATION_KEYWORD_VOID)
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_INVALID_DECLARATION_TYPE;
                }

                pNewNode->m_psStringData = pTopStackCurrentNode->pLeft->pLeft->m_psStringData;

                int m_nCurrentTokenStatus = m_nTokenStatus;

                m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_IDENTIFIER;
                m_nTokenCharacters = pNewNode->m_psStringData.GetLength();
                memcpy(m_pchToken, pNewNode->m_psStringData.CStr(), m_nTokenCharacters);
                int nReturnValue = TestIdentifierToken();
                if (nReturnValue != 0)
                {
                    return nReturnValue;
                }

                if (m_nTokenStatus != CSCRIPTCOMPILER_TOKEN_VARIABLE)
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_PARSING_VARIABLE_LIST;
                }

                CScriptParseTreeNode* pNewNode5 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_VARIABLE, NULL, NULL);
                m_pchToken[m_nTokenCharacters] = 0;
                pNewNode5->m_psStringData.Format("%s", m_pchToken);

                CScriptParseTreeNode* pNewNode6 = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_VARIABLE_LIST, pNewNode5, NULL);
                pNewNode->pRight = pNewNode6;

                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_DECL_VARLIST, 1, 2, pNewNode2);

                m_nTokenStatus = m_nCurrentTokenStatus;

                delete pTopStackCurrentNode->pLeft->pLeft;
                delete pTopStackCurrentNode->pLeft;
                delete pTopStackCurrentNode;
            }
            if (nTopStackRule == 4 && nTopStackTerm == 3)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_SEMICOLON)
                {
                    AddToGlobalVariableList(pTopStackCurrentNode);
                    return 0;
                }
                else
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_NO_SEMICOLON_AFTER_EXPRESSION;
                }
            }
            break;

        case CSCRIPTCOMPILER_GRAMMAR_PROGRAM:
            if (nTopStackRule == 0 && nTopStackTerm == 0)
            {
                if (pTopStackCurrentNode == NULL && m_nCompileFileLevel <= 1)
                {
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_AFTER_PROGRAM, 0, 0, NULL);
                }

                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_EOF)
                {
                    ModifySRStackReturnTree(NULL);

                    if (m_nCompileFileLevel > 1)
                    {
                        return 0;
                    }
                }
                else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_INCLUDE)
                {
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_PROGRAM, 3, 1, pTopStackCurrentNode);
                    return 0;
                }
                else
                {
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_PROGRAM, 1, 2, NULL);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_FUNCTIONAL_UNIT, 0, 0, NULL);
                }
            }
            if (nTopStackRule == 1 && nTopStackTerm == 2)
            {
                CScriptParseTreeNode* pNewNode = CreateScriptParseTreeNode(CSCRIPTCOMPILER_OPERATION_FUNCTIONAL_UNIT, pTopStackReturnNode, NULL);
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_PROGRAM, 1, 3, pNewNode);
                PushSRStack(CSCRIPTCOMPILER_GRAMMAR_PROGRAM, 0, 0, pNewNode);
            }
            if (nTopStackRule == 1 && nTopStackTerm == 3)
            {
                pTopStackCurrentNode->pRight = pTopStackReturnNode;
                ModifySRStackReturnTree(pTopStackCurrentNode);
            }
            if (nTopStackRule == 3 && nTopStackTerm == 1)
            {
                if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_STRING)
                {
                    CExoString sStringToCompile;

                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_PROGRAM, 3, 2, pTopStackCurrentNode);
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_PROGRAM, 0, 0, pTopStackCurrentNode);

                    m_pchToken[m_nTokenCharacters] = 0;
                    sStringToCompile.Format("%s", m_pchToken);

                    TokenInitialize();
                    return CompileFile(sStringToCompile);
                }
            }
            if (nTopStackRule == 3 && nTopStackTerm == 2)
            {

                CScriptParseTreeNode* pRunnerNode;
                CScriptParseTreeNode* pNextNode;
                CScriptParseTreeNode* pLastNode;

                pLastNode = NULL;
                pRunnerNode = pTopStackReturnNode;

                while (pRunnerNode != NULL)
                {
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_PROGRAM, 1, 3, pRunnerNode);
                    pLastNode = pRunnerNode;
                    pNextNode = pRunnerNode->pRight;
                    pRunnerNode->pRight = NULL;
                    pRunnerNode = pNextNode;
                }

                if (pLastNode == NULL)
                {
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_PROGRAM, 0, 0, pTopStackCurrentNode);
                }
                else
                {
                    PushSRStack(CSCRIPTCOMPILER_GRAMMAR_PROGRAM, 0, 0, pLastNode);
                }
            }
            break;

        default:
            return STRREF_CSCRIPTCOMPILER_ERROR_UNKNOWN_STATE_IN_COMPILER;
        }
    }

    return 0;
}
