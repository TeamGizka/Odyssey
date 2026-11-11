#include "exobase.h"
#include "exotlktable.h"
#include "nwscriptinternal.h"

int CScriptCompilerInternal::GenerateIdentifierList()
{
    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_INT ||
        m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_ACTION ||
        m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_OBJECT ||
        m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRING ||
        m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT ||
        m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_VECTOR ||
        m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE0 ||
        m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE1 ||
        m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE2 ||
        m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE3 ||
        m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE4 ||
        m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE5 ||
        m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE6 ||
        m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE7 ||
        m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE8 ||
        m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE9 ||
        m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_VOID)
    {
        if (m_nIdentifierListState == CSCRIPTCOMPILER_IDENT_STATE_START_OF_LINE)
        {
            m_nIdentifierListState = CSCRIPTCOMPILER_IDENT_STATE_AFTER_TYPE_DECLARATION;
            m_nIdentifierListReturnType = m_nTokenStatus;
            return 0;
        }
        else if (m_nIdentifierListState == CSCRIPTCOMPILER_IDENT_STATE_IN_PARAMETER_LIST)
        {

            int nValue = m_pcIdentifierList[m_nOccupiedIdentifiers - 1].m_nParameters;

            if (nValue == m_pcIdentifierList[m_nOccupiedIdentifiers - 1].m_nParameterSpace)
            {
                int nReturnValue = m_pcIdentifierList[m_nOccupiedIdentifiers - 1].ExpandParameterSpace();
                if (nReturnValue < 0)
                {
                    return nReturnValue;
                }
            }

            m_pcIdentifierList[m_nOccupiedIdentifiers - 1].m_pchParameters[nValue] = (char)m_nTokenStatus;

            if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_VECTOR)
            {
                m_pcIdentifierList[m_nOccupiedIdentifiers - 1].m_pchParameters[nValue] = (char)CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT;
                m_pcIdentifierList[m_nOccupiedIdentifiers - 1].m_psStructureParameterNames[nValue] = "vector";
            }

            m_pcIdentifierList[m_nOccupiedIdentifiers - 1].m_pbOptionalParameters[nValue] = FALSE;
            m_pcIdentifierList[m_nOccupiedIdentifiers - 1].m_nParameters = nValue + 1;
            return 0;
        }
        else
        {
            return STRREF_CSCRIPTCOMPILER_ERROR_PARSING_IDENTIFIER_LIST;
        }
    }

    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_DEFINE)
    {
        if (m_nIdentifierListState == CSCRIPTCOMPILER_IDENT_STATE_START_OF_LINE)
        {
            m_nIdentifierListState = CSCRIPTCOMPILER_IDENT_STATE_DEFINE_STATEMENT;
            return 0;
        }
        else
        {
            return STRREF_CSCRIPTCOMPILER_ERROR_PARSING_IDENTIFIER_LIST;
        }
    }

    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_NUM_STRUCTURES_DEFINITION)
    {
        if (m_nIdentifierListState == CSCRIPTCOMPILER_IDENT_STATE_DEFINE_STATEMENT)
        {
            m_nIdentifierListState = CSCRIPTCOMPILER_IDENT_STATE_DEFINE_NUM_ENGINE_STRUCTURE;
            return 0;
        }
        else
        {
            return STRREF_CSCRIPTCOMPILER_ERROR_PARSING_IDENTIFIER_LIST;
        }
    }

    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE_DEFINITION)
    {
        if (m_nIdentifierListState == CSCRIPTCOMPILER_IDENT_STATE_DEFINE_STATEMENT)
        {
            m_nIdentifierListState = CSCRIPTCOMPILER_IDENT_STATE_DEFINE_SINGLE_ENGINE_STRUCTURE;

            m_nIdentifierListEngineStructure = (int)(m_pchToken[17] - '0');

            if (m_nIdentifierListEngineStructure < 0 ||
                m_nIdentifierListEngineStructure >= CSCRIPTCOMPILER_MAX_ENGINE_STRUCTURES)
            {
                return STRREF_CSCRIPTCOMPILER_ERROR_PARSING_IDENTIFIER_LIST;
            }
            return 0;
        }
        else
        {
            return STRREF_CSCRIPTCOMPILER_ERROR_PARSING_IDENTIFIER_LIST;
        }
    }

    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_EQUAL)
    {
        if (m_nIdentifierListState == CSCRIPTCOMPILER_IDENT_STATE_AFTER_FUNCTION_IDENTIFIER)
        {
            m_nIdentifierListState = CSCRIPTCOMPILER_IDENT_STATE_BEFORE_CONSTANT_IDENTIFIER;
            m_pcIdentifierList[m_nOccupiedIdentifiers - 1].m_nIdentifierType = 0;
            return 0;
        }
        else if (m_nIdentifierListState == CSCRIPTCOMPILER_IDENT_STATE_IN_PARAMETER_LIST)
        {
            m_nIdentifierListState = CSCRIPTCOMPILER_IDENT_STATE_IN_PARAMETER_LIST_CONSTANT;
            return 0;
        }
        else
        {
            return STRREF_CSCRIPTCOMPILER_ERROR_PARSING_IDENTIFIER_LIST;
        }
    }

    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_VARIABLE)
    {
        if (m_nIdentifierListState == CSCRIPTCOMPILER_IDENT_STATE_DEFINE_SINGLE_ENGINE_STRUCTURE)
        {
            m_pbEngineDefinedStructureValid[m_nIdentifierListEngineStructure] = TRUE;
            m_pchToken[m_nTokenCharacters] = 0;
            (m_psEngineDefinedStructureName[m_nIdentifierListEngineStructure]).Format("%s", m_pchToken);

            m_nIdentifierListState = CSCRIPTCOMPILER_IDENT_STATE_START_OF_LINE;
            return 0;
        }

        if (m_nIdentifierListState == CSCRIPTCOMPILER_IDENT_STATE_AFTER_TYPE_DECLARATION)
        {
            m_nIdentifierListState = CSCRIPTCOMPILER_IDENT_STATE_AFTER_FUNCTION_IDENTIFIER;
            m_pcIdentifierList[m_nOccupiedIdentifiers].m_nIdentifierType = 1;

            if (m_nIdentifierListReturnType == CSCRIPTCOMPILER_TOKEN_KEYWORD_INT)
            {
                m_pcIdentifierList[m_nOccupiedIdentifiers].m_nReturnType = CSCRIPTCOMPILER_TOKEN_INTEGER_IDENTIFIER;
            }
            else if (m_nIdentifierListReturnType == CSCRIPTCOMPILER_TOKEN_KEYWORD_OBJECT)
            {
                m_pcIdentifierList[m_nOccupiedIdentifiers].m_nReturnType = CSCRIPTCOMPILER_TOKEN_OBJECT_IDENTIFIER;
            }
            else if (m_nIdentifierListReturnType == CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT)
            {
                m_pcIdentifierList[m_nOccupiedIdentifiers].m_nReturnType = CSCRIPTCOMPILER_TOKEN_FLOAT_IDENTIFIER;
            }
            else if (m_nIdentifierListReturnType == CSCRIPTCOMPILER_TOKEN_KEYWORD_STRING)
            {
                m_pcIdentifierList[m_nOccupiedIdentifiers].m_nReturnType = CSCRIPTCOMPILER_TOKEN_STRING_IDENTIFIER;
            }
            else if (m_nIdentifierListReturnType == CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE0)
            {
                m_pcIdentifierList[m_nOccupiedIdentifiers].m_nReturnType = CSCRIPTCOMPILER_TOKEN_ENGINE_STRUCTURE0_IDENTIFIER;
            }
            else if (m_nIdentifierListReturnType == CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE1)
            {
                m_pcIdentifierList[m_nOccupiedIdentifiers].m_nReturnType = CSCRIPTCOMPILER_TOKEN_ENGINE_STRUCTURE1_IDENTIFIER;
            }
            else if (m_nIdentifierListReturnType == CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE2)
            {
                m_pcIdentifierList[m_nOccupiedIdentifiers].m_nReturnType = CSCRIPTCOMPILER_TOKEN_ENGINE_STRUCTURE2_IDENTIFIER;
            }
            else if (m_nIdentifierListReturnType == CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE3)
            {
                m_pcIdentifierList[m_nOccupiedIdentifiers].m_nReturnType = CSCRIPTCOMPILER_TOKEN_ENGINE_STRUCTURE3_IDENTIFIER;
            }
            else if (m_nIdentifierListReturnType == CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE4)
            {
                m_pcIdentifierList[m_nOccupiedIdentifiers].m_nReturnType = CSCRIPTCOMPILER_TOKEN_ENGINE_STRUCTURE4_IDENTIFIER;
            }
            else if (m_nIdentifierListReturnType == CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE5)
            {
                m_pcIdentifierList[m_nOccupiedIdentifiers].m_nReturnType = CSCRIPTCOMPILER_TOKEN_ENGINE_STRUCTURE5_IDENTIFIER;
            }
            else if (m_nIdentifierListReturnType == CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE6)
            {
                m_pcIdentifierList[m_nOccupiedIdentifiers].m_nReturnType = CSCRIPTCOMPILER_TOKEN_ENGINE_STRUCTURE6_IDENTIFIER;
            }
            else if (m_nIdentifierListReturnType == CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE7)
            {
                m_pcIdentifierList[m_nOccupiedIdentifiers].m_nReturnType = CSCRIPTCOMPILER_TOKEN_ENGINE_STRUCTURE7_IDENTIFIER;
            }
            else if (m_nIdentifierListReturnType == CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE8)
            {
                m_pcIdentifierList[m_nOccupiedIdentifiers].m_nReturnType = CSCRIPTCOMPILER_TOKEN_ENGINE_STRUCTURE8_IDENTIFIER;
            }
            else if (m_nIdentifierListReturnType == CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE9)
            {
                m_pcIdentifierList[m_nOccupiedIdentifiers].m_nReturnType = CSCRIPTCOMPILER_TOKEN_ENGINE_STRUCTURE9_IDENTIFIER;
            }
            else if (m_nIdentifierListReturnType == CSCRIPTCOMPILER_TOKEN_KEYWORD_VECTOR)
            {
                m_pcIdentifierList[m_nOccupiedIdentifiers].m_nReturnType = CSCRIPTCOMPILER_TOKEN_STRUCTURE_IDENTIFIER;
                m_pcIdentifierList[m_nOccupiedIdentifiers].m_psStructureReturnName = "vector";
            }
            else if (m_nIdentifierListReturnType == CSCRIPTCOMPILER_TOKEN_KEYWORD_VOID)
            {
                m_pcIdentifierList[m_nOccupiedIdentifiers].m_nReturnType = CSCRIPTCOMPILER_TOKEN_VOID_IDENTIFIER;
            }
            else
            {
                return STRREF_CSCRIPTCOMPILER_ERROR_PARSING_IDENTIFIER_LIST;
            }

            m_pchToken[m_nTokenCharacters] = 0;
            (m_pcIdentifierList[m_nOccupiedIdentifiers].m_psIdentifier).Format("%s", m_pchToken);
            m_pcIdentifierList[m_nOccupiedIdentifiers].m_nIdentifierLength = m_nTokenCharacters;

            m_pcIdentifierList[m_nOccupiedIdentifiers].m_nBinarySourceStart = -1;
            m_pcIdentifierList[m_nOccupiedIdentifiers].m_nBinarySourceFinish = -1;
            m_pcIdentifierList[m_nOccupiedIdentifiers].m_nBinaryDestinationStart = -1;
            m_pcIdentifierList[m_nOccupiedIdentifiers].m_nBinaryDestinationFinish = -1;

            ++m_nOccupiedIdentifiers;
            if (m_nOccupiedIdentifiers >= CSCRIPTCOMPILER_MAX_IDENTIFIERS)
            {
                return STRREF_CSCRIPTCOMPILER_ERROR_IDENTIFIER_LIST_FULL;
            }

            return 0;
        }
        else if (m_nIdentifierListState != CSCRIPTCOMPILER_IDENT_STATE_IN_PARAMETER_LIST)
        {
            return STRREF_CSCRIPTCOMPILER_ERROR_PARSING_IDENTIFIER_LIST;
        }

        return 0;
    }

    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_EOF)
    {
        return 0;
    }

    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_MINUS)
    {
        if (m_nIdentifierListState == CSCRIPTCOMPILER_IDENT_STATE_BEFORE_CONSTANT_IDENTIFIER)
        {
            m_pcIdentifierList[m_nOccupiedIdentifiers - 1].m_nIntegerData = -1;
        }
        if (m_nIdentifierListState == CSCRIPTCOMPILER_IDENT_STATE_IN_PARAMETER_LIST_CONSTANT)
        {
            m_pcIdentifierList[m_nOccupiedIdentifiers - 1].m_nIntegerData = -1;
        }
        return 0;
    }

    int nCount;

    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_INTEGER)
    {
        if (m_nIdentifierListState == CSCRIPTCOMPILER_IDENT_STATE_DEFINE_NUM_ENGINE_STRUCTURE)
        {
            int nValue = 0;
            for (nCount = 0; nCount < m_nTokenCharacters; ++nCount)
            {
                nValue = nValue * 10 + (m_pchToken[nCount] - '0');
            }

            if (nValue >= CSCRIPTCOMPILER_MAX_ENGINE_STRUCTURES)
            {
                nValue = CSCRIPTCOMPILER_MAX_ENGINE_STRUCTURES;
            }

            if (nValue < 0)
            {
                nValue = 1;
            }

            m_nNumEngineDefinedStructures = nValue;

            if (m_pbEngineDefinedStructureValid != NULL)
            {
                delete[] m_pbEngineDefinedStructureValid;
            }

            m_pbEngineDefinedStructureValid = new BOOL[nValue];
            for (int nStructure = 0; nStructure < nValue; ++nStructure)
            {
                m_pbEngineDefinedStructureValid[nStructure] = FALSE;
            }

            if (m_psEngineDefinedStructureName != NULL)
            {
                delete[] m_psEngineDefinedStructureName;
            }
            m_psEngineDefinedStructureName = new CExoString[nValue];

            m_nIdentifierListState = CSCRIPTCOMPILER_IDENT_STATE_START_OF_LINE;
            return 0;
        }

        if (m_bCompileIdentifierConstants == FALSE &&
            m_nIdentifierListState == CSCRIPTCOMPILER_IDENT_STATE_BEFORE_CONSTANT_IDENTIFIER)
        {
            return 0;
        }

        int nLoc = m_nOccupiedIdentifiers - 1;

        if (m_nIdentifierListState == CSCRIPTCOMPILER_IDENT_STATE_BEFORE_CONSTANT_IDENTIFIER)
        {
            m_nIdentifierListState = CSCRIPTCOMPILER_IDENT_STATE_AFTER_CONSTANT_IDENTIFIER;

            if (m_pcIdentifierList[nLoc].m_nReturnType != CSCRIPTCOMPILER_TOKEN_INTEGER_IDENTIFIER ||
                m_pcIdentifierList[nLoc].m_nIdentifierType != 0)
            {
                return STRREF_CSCRIPTCOMPILER_ERROR_NON_INTEGER_ID_FOR_INTEGER_CONSTANT;
            }
        }
        else if (m_nIdentifierListState != CSCRIPTCOMPILER_IDENT_STATE_IN_PARAMETER_LIST_CONSTANT)
        {
            return STRREF_CSCRIPTCOMPILER_ERROR_PARSING_IDENTIFIER_LIST;
        }

        int nSign = 1;
        if (m_pcIdentifierList[nLoc].m_nIntegerData == -1)
        {
            m_pcIdentifierList[nLoc].m_nIntegerData = 0;
            nSign = -1;
        }

        nCount = 0;
        if (m_pchToken[nCount] == '-')
        {
            nSign *= -1;
            ++nCount;
        }

        int nValue = 0;
        for (; nCount < m_nTokenCharacters; ++nCount)
        {
            nValue = nValue * 10 + (m_pchToken[nCount] - '0');
        }

        if (m_nIdentifierListState == CSCRIPTCOMPILER_IDENT_STATE_AFTER_CONSTANT_IDENTIFIER)
        {
            m_pcIdentifierList[nLoc].m_nIntegerData = nSign * nValue;
            m_pchToken[m_nTokenCharacters] = 0;

            if (nSign == -1)
            {
                (m_pcIdentifierList[nLoc].m_psStringData).Format("-%s", m_pchToken);
            }
            else
            {
                (m_pcIdentifierList[nLoc].m_psStringData).Format("%s", m_pchToken);
            }
        }
        else if (m_nIdentifierListState == CSCRIPTCOMPILER_IDENT_STATE_IN_PARAMETER_LIST_CONSTANT)
        {
            m_nIdentifierListState = CSCRIPTCOMPILER_IDENT_STATE_IN_PARAMETER_LIST;
            int nLoc2 = m_pcIdentifierList[nLoc].m_nParameters - 1;
            m_pcIdentifierList[nLoc].m_pbOptionalParameters[nLoc2] = TRUE;
            m_pcIdentifierList[nLoc].m_pnOptionalParameterIntegerData[nLoc2] = nSign * nValue;
        }

        return 0;
    }

    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_FLOAT)
    {
        if (m_bCompileIdentifierConstants == FALSE &&
            m_nIdentifierListState == CSCRIPTCOMPILER_IDENT_STATE_BEFORE_CONSTANT_IDENTIFIER)
        {
            return 0;
        }

        int nLoc = m_nOccupiedIdentifiers - 1;

        if (m_nIdentifierListState == CSCRIPTCOMPILER_IDENT_STATE_BEFORE_CONSTANT_IDENTIFIER)
        {
            m_nIdentifierListState = CSCRIPTCOMPILER_IDENT_STATE_AFTER_CONSTANT_IDENTIFIER;

            if (m_pcIdentifierList[nLoc].m_nReturnType != CSCRIPTCOMPILER_TOKEN_FLOAT_IDENTIFIER ||
                m_pcIdentifierList[nLoc].m_nIdentifierType != 0)
            {
                return STRREF_CSCRIPTCOMPILER_ERROR_NON_FLOAT_ID_FOR_FLOAT_CONSTANT;
            }
        }
        else if (m_nIdentifierListState != CSCRIPTCOMPILER_IDENT_STATE_IN_VECTOR_PARAMETER &&
                 m_nIdentifierListState != CSCRIPTCOMPILER_IDENT_STATE_IN_VECTOR_CONSTANT &&
                 m_nIdentifierListState != CSCRIPTCOMPILER_IDENT_STATE_IN_PARAMETER_LIST_CONSTANT)
        {
            return STRREF_CSCRIPTCOMPILER_ERROR_PARSING_IDENTIFIER_LIST;
        }

        float fSign = 1.0f;
        if (m_pcIdentifierList[nLoc].m_nIntegerData == -1)
        {
            m_pcIdentifierList[nLoc].m_nIntegerData = 0;
            fSign = -1.0f;
        }

        float fValue = ParseFloatFromTokenString();

        if (m_nIdentifierListState == CSCRIPTCOMPILER_IDENT_STATE_AFTER_CONSTANT_IDENTIFIER)
        {
            m_pcIdentifierList[nLoc].m_fFloatData = fSign * fValue;
            m_pchToken[m_nTokenCharacters] = 0;

            if (fSign < 0.0f)
            {
                (m_pcIdentifierList[nLoc].m_psStringData).Format("-%s", m_pchToken);
            }
            else
            {
                (m_pcIdentifierList[nLoc].m_psStringData).Format("%s", m_pchToken);
            }
        }
        else if (m_nIdentifierListState == CSCRIPTCOMPILER_IDENT_STATE_IN_PARAMETER_LIST_CONSTANT)
        {
            m_nIdentifierListState = CSCRIPTCOMPILER_IDENT_STATE_IN_PARAMETER_LIST;
            int nLoc2 = m_pcIdentifierList[nLoc].m_nParameters - 1;
            m_pcIdentifierList[nLoc].m_pbOptionalParameters[nLoc2] = TRUE;
            m_pcIdentifierList[nLoc].m_pfOptionalParameterFloatData[nLoc2] = fSign * fValue;
        }
        else if (m_nIdentifierListState == CSCRIPTCOMPILER_IDENT_STATE_IN_VECTOR_CONSTANT)
        {
            m_pcIdentifierList[nLoc].m_fVectorData[m_nIdentifierListVector] = fSign * fValue;
        }
        else if (m_nIdentifierListState == CSCRIPTCOMPILER_IDENT_STATE_IN_VECTOR_PARAMETER)
        {
            int nLoc2 = m_pcIdentifierList[nLoc].m_nParameters - 1;
            m_pcIdentifierList[nLoc].m_pbOptionalParameters[nLoc2] = TRUE;
            m_pcIdentifierList[nLoc].m_pfOptionalParameterVectorData[nLoc2 * 3 + m_nIdentifierListVector] = fSign * fValue;
        }

        return 0;
    }

    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_STRING)
    {
        if (m_bCompileIdentifierConstants == FALSE &&
            m_nIdentifierListState == CSCRIPTCOMPILER_IDENT_STATE_BEFORE_CONSTANT_IDENTIFIER)
        {
            return 0;
        }

        int nLoc = m_nOccupiedIdentifiers - 1;

        if (m_nIdentifierListState == CSCRIPTCOMPILER_IDENT_STATE_BEFORE_CONSTANT_IDENTIFIER)
        {
            m_nIdentifierListState = CSCRIPTCOMPILER_IDENT_STATE_AFTER_CONSTANT_IDENTIFIER;

            if (m_pcIdentifierList[nLoc].m_nReturnType != CSCRIPTCOMPILER_TOKEN_STRING_IDENTIFIER ||
                m_pcIdentifierList[nLoc].m_nIdentifierType != 0)
            {
                return STRREF_CSCRIPTCOMPILER_ERROR_NON_STRING_ID_FOR_STRING_CONSTANT;
            }
        }
        else if (m_nIdentifierListState != CSCRIPTCOMPILER_IDENT_STATE_IN_PARAMETER_LIST_CONSTANT)
        {
            return STRREF_CSCRIPTCOMPILER_ERROR_PARSING_IDENTIFIER_LIST;
        }

        if (m_nIdentifierListState == CSCRIPTCOMPILER_IDENT_STATE_AFTER_CONSTANT_IDENTIFIER)
        {
            m_pchToken[m_nTokenCharacters] = 0;
            (m_pcIdentifierList[nLoc].m_psStringData).Format("%s", m_pchToken);
        }
        else if (m_nIdentifierListState == CSCRIPTCOMPILER_IDENT_STATE_IN_PARAMETER_LIST_CONSTANT)
        {
            m_nIdentifierListState = CSCRIPTCOMPILER_IDENT_STATE_IN_PARAMETER_LIST;
            int nLoc2 = m_pcIdentifierList[nLoc].m_nParameters - 1;
            m_pcIdentifierList[nLoc].m_pbOptionalParameters[nLoc2] = TRUE;
            m_pchToken[m_nTokenCharacters] = 0;
            (m_pcIdentifierList[nLoc].m_psOptionalParameterStringData[nLoc2]).Format("%s", m_pchToken);
        }
        return 0;
    }

    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_OBJECT_SELF)
    {
        if (m_nIdentifierListState != CSCRIPTCOMPILER_IDENT_STATE_IN_PARAMETER_LIST_CONSTANT)
        {
            return STRREF_CSCRIPTCOMPILER_ERROR_PARSING_IDENTIFIER_LIST;
        }

        m_nIdentifierListState = CSCRIPTCOMPILER_IDENT_STATE_IN_PARAMETER_LIST;
        int nLoc = m_nOccupiedIdentifiers - 1;
        int nLoc2 = m_pcIdentifierList[nLoc].m_nParameters - 1;
        m_pcIdentifierList[nLoc].m_pbOptionalParameters[nLoc2] = TRUE;
        m_pcIdentifierList[nLoc].m_poidOptionalParameterObjectData[nLoc2] = 0;
        return 0;
    }

    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_KEYWORD_OBJECT_INVALID)
    {
        if (m_nIdentifierListState != CSCRIPTCOMPILER_IDENT_STATE_IN_PARAMETER_LIST_CONSTANT)
        {
            return STRREF_CSCRIPTCOMPILER_ERROR_PARSING_IDENTIFIER_LIST;
        }

        m_nIdentifierListState = CSCRIPTCOMPILER_IDENT_STATE_IN_PARAMETER_LIST;
        int nLoc = m_nOccupiedIdentifiers - 1;
        int nLoc2 = m_pcIdentifierList[nLoc].m_nParameters - 1;
        m_pcIdentifierList[nLoc].m_pbOptionalParameters[nLoc2] = TRUE;
        m_pcIdentifierList[nLoc].m_poidOptionalParameterObjectData[nLoc2] = 1;
        return 0;
    }

    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_LEFT_SQUARE_BRACKET)
    {
        if (m_nIdentifierListState != CSCRIPTCOMPILER_IDENT_STATE_IN_PARAMETER_LIST_CONSTANT &&
            m_nIdentifierListState != CSCRIPTCOMPILER_IDENT_STATE_BEFORE_CONSTANT_IDENTIFIER)
        {
            return STRREF_CSCRIPTCOMPILER_ERROR_PARSING_IDENTIFIER_LIST;
        }

        m_nIdentifierListVector = 0;
        if (m_nIdentifierListState == CSCRIPTCOMPILER_IDENT_STATE_BEFORE_CONSTANT_IDENTIFIER)
        {
            m_nIdentifierListState = CSCRIPTCOMPILER_IDENT_STATE_IN_VECTOR_CONSTANT;
        }
        else
        {
            m_nIdentifierListState = CSCRIPTCOMPILER_IDENT_STATE_IN_VECTOR_PARAMETER;
        }

        int nLoc = m_nOccupiedIdentifiers - 1;
        int nLoc2 = m_pcIdentifierList[nLoc].m_nParameters - 1;
        m_pcIdentifierList[nLoc].m_pbOptionalParameters[nLoc2] = TRUE;
        for (int nLoc3 = 0; nLoc3 < 3; ++nLoc3)
        {
            m_pcIdentifierList[nLoc].m_pfOptionalParameterVectorData[nLoc2 * 3 + nLoc3] = 0.0f;
        }
        return 0;
    }

    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_RIGHT_SQUARE_BRACKET)
    {
        if (m_nIdentifierListState != CSCRIPTCOMPILER_IDENT_STATE_IN_VECTOR_PARAMETER &&
            m_nIdentifierListState != CSCRIPTCOMPILER_IDENT_STATE_IN_VECTOR_CONSTANT)
        {
            return STRREF_CSCRIPTCOMPILER_ERROR_PARSING_IDENTIFIER_LIST;
        }

        m_nIdentifierListVector = 0;
        if (m_nIdentifierListState == CSCRIPTCOMPILER_IDENT_STATE_IN_VECTOR_PARAMETER)
        {
            m_nIdentifierListState = CSCRIPTCOMPILER_IDENT_STATE_IN_PARAMETER_LIST;
        }
        else
        {
            m_nIdentifierListState = CSCRIPTCOMPILER_IDENT_STATE_AFTER_CONSTANT_IDENTIFIER;
        }

        return 0;
    }

    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_SEMICOLON)
    {

        if (m_nIdentifierListState == CSCRIPTCOMPILER_IDENT_STATE_IN_PARAMETER_LIST)
        {
            int nLoc = m_nOccupiedIdentifiers - 1;
            int nLoc2 = m_pcIdentifierList[nLoc].m_nParameters - 1;

            m_pcIdentifierList[nLoc].m_nIdIdentifier = m_nPredefinedIdentifierOrder;
            ++m_nPredefinedIdentifierOrder;
            m_pcIdentifierList[nLoc].m_bImplementationInPlace = TRUE;

            m_pcIdentifierList[nLoc].m_nNonOptionalParameters = 0;
            for (int nParameter = nLoc2;
                 nParameter >= 0 && m_pcIdentifierList[nLoc].m_nNonOptionalParameters == 0;
                 --nParameter)
            {
                if (m_pcIdentifierList[nLoc].m_pbOptionalParameters[nParameter] == FALSE)
                {
                    m_pcIdentifierList[nLoc].m_nNonOptionalParameters = nParameter + 1;
                }
            }
        }
        ++m_nMaxPredefinedIdentifierId;

        m_nIdentifierListState = CSCRIPTCOMPILER_IDENT_STATE_START_OF_LINE;
        return 0;
    }

    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_LEFT_BRACKET)
    {
        if (m_nIdentifierListState == CSCRIPTCOMPILER_IDENT_STATE_AFTER_FUNCTION_IDENTIFIER)
        {
            m_nIdentifierListState = CSCRIPTCOMPILER_IDENT_STATE_IN_PARAMETER_LIST;
            return 0;
        }
    }

    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_RIGHT_BRACKET ||
        m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_COMMA)
    {
        return 0;
    }

    return STRREF_CSCRIPTCOMPILER_ERROR_PARSING_IDENTIFIER_LIST;
}

int CScriptCompilerInternal::PrintParseIdentifierFileError(int nChar, int nError)
{
    STR_RES strRes;
    g_pTlkTable->Fetch(-nError, strRes, CTlkTable::GENDER_MALE);

    CExoString sFileName = m_pcIncludeFileStack[0].m_sFileName;
    OutputError(nError, sFileName, m_nLines, strRes.m_sString);
    m_pcIncludeFileStack[0].m_cSourceScript.UnloadScript();
    return nError;
}

int CScriptCompilerInternal::ParseIdentifierFile()
{
    char* pScript;
    unsigned long nScriptLength;
    unsigned long i;
    int ch;
    int chNext;
    int nParseCharacterReturn;

    m_nPredefinedIdentifierOrder = 0;

    nParseCharacterReturn = m_pcIncludeFileStack[0].m_cSourceScript.LoadScript(m_sLanguageSource.CStr(), &pScript, &nScriptLength);
    if (nParseCharacterReturn < 0)
    {
        return PrintParseIdentifierFileError(0x41, nParseCharacterReturn);
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
            m_pcIncludeFileStack[0].m_cSourceScript.UnloadScript();
            return PrintParseIdentifierFileError(ch, nParseCharacterReturn);
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

    m_pcIncludeFileStack[0].m_cSourceScript.UnloadScript();

    if (nParseCharacterReturn < 0)
    {
        return PrintParseIdentifierFileError(-1, nParseCharacterReturn);
    }

    return 0;
}
