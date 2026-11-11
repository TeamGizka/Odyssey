#include "exobase.h"
#include "nwscriptinternal.h"

#include "nwscriptinternal.h"

float CScriptCompilerInternal::ParseFloatFromTokenString()
{
    float fValue = 0.0f;
    float fSign = 1.0f;
    float fDecimalPlace = 1.0f;
    int bDecimal = FALSE;
    char* pchToken = m_pchToken;

    for (int nCount = 0; nCount < m_nTokenCharacters; ++nCount, ++pchToken)
    {
        char ch = *pchToken;

        if (nCount == 0 && ch == '-')
        {
            fSign = -1.0f;
        }

        if (ch >= '0' && ch <= '9')
        {
            if (bDecimal == FALSE)
            {
                fValue = fValue * 10.0f + (float)(ch - '0');
            }
            else if (bDecimal == TRUE)
            {
                fDecimalPlace /= 10.0f;
                fValue += (float)(ch - '0') * fDecimalPlace;
            }
        }

        if (ch == '.')
        {
            bDecimal = TRUE;
        }
    }

    if (fSign < 0.0f)
    {
        fValue *= fSign;
    }

    return fValue;
}

int CScriptCompilerInternal::ParseCharacterNumeric(int nChar)
{
    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_UNKNOWN)
    {
        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_INTEGER;
        m_nTokenCharacters = 0;
    }

    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_INTEGER ||
        m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_HEX_INTEGER ||
        m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_FLOAT ||
        m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_STRING ||
        m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_IDENTIFIER)
    {
        m_pchToken[m_nTokenCharacters] = (char)nChar;
        ++m_nTokenCharacters;
        if (m_nTokenCharacters > CSCRIPTCOMPILER_MAX_TOKEN_LENGTH)
        {
            return STRREF_CSCRIPTCOMPILER_ERROR_TOKEN_TOO_LONG;
        }
    }
    else
    {
        return STRREF_CSCRIPTCOMPILER_ERROR_UNEXPECTED_CHARACTER;
    }

    return 0;
}

int CScriptCompilerInternal::ParseCharacterAlphabet(int nChar)
{
    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_UNKNOWN)
    {
        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_IDENTIFIER;
        m_nTokenCharacters = 0;
    }

    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_INTEGER)
    {

        if ((nChar == 'x' || nChar == 'X') && m_nTokenCharacters == 1 && m_pchToken[0] == '0')
        {
            m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_HEX_INTEGER;
            m_pchToken[1] = (char)nChar;
            m_nTokenCharacters = 2;
            return 0;
        }
    }
    else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_HEX_INTEGER)
    {
        if ((nChar >= 'a' && nChar <= 'f') || (nChar >= 'A' && nChar <= 'F'))
        {
            if (nChar <= 'F')
            {
                m_pchToken[m_nTokenCharacters] = (char)(nChar + 32);
            }
            else
            {
                m_pchToken[m_nTokenCharacters] = (char)nChar;
            }
            ++m_nTokenCharacters;
            if (m_nTokenCharacters >= CSCRIPTCOMPILER_MAX_TOKEN_LENGTH)
            {
                return STRREF_CSCRIPTCOMPILER_ERROR_TOKEN_TOO_LONG;
            }
            return 0;
        }
    }

    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_IDENTIFIER || m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_STRING)
    {
        m_pchToken[m_nTokenCharacters] = (char)nChar;
        ++m_nTokenCharacters;
        if (m_nTokenCharacters >= CSCRIPTCOMPILER_MAX_TOKEN_LENGTH)
        {
            return STRREF_CSCRIPTCOMPILER_ERROR_TOKEN_TOO_LONG;
        }
        return 0;
    }

    return STRREF_CSCRIPTCOMPILER_ERROR_UNEXPECTED_CHARACTER;
}

int CScriptCompilerInternal::ParseCommentedOutCharacter(int nChar)
{
    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_CPLUSCOMMENT && nChar == '\n')
    {
        TokenInitialize();
    }

    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_CCOMMENT)
    {

        if (nChar == '*')
        {
            if (m_nTokenCharacters == 0)
            {
                m_nTokenCharacters = 1;
            }
        }
        else if (nChar == '/')
        {
            if (m_nTokenCharacters == 1)
            {
                TokenInitialize();
            }
        }
        else
        {
            m_nTokenCharacters = 0;
        }
    }

    return 0;
}

int CScriptCompilerInternal::HandleToken()
{
    int nReturnValue;

    if (m_bCompileIdentifierList == TRUE)
    {
        nReturnValue = GenerateIdentifierList();
    }
    else
    {
        nReturnValue = GenerateParseTree();
    }

    if (nReturnValue < 0)
    {
        return nReturnValue;
    }

    TokenInitialize();
    return 0;
}

int CScriptCompilerInternal::ParseCharacterColon()
{
    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_UNKNOWN)
    {
        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_COLON;
        return HandleToken();
    }

    return STRREF_CSCRIPTCOMPILER_ERROR_UNEXPECTED_CHARACTER;
}

int CScriptCompilerInternal::ParseCharacterEllipsis()
{
    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_UNKNOWN)
    {
        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_IDENTIFIER;
        m_nTokenCharacters = 0;
        m_pchToken[m_nTokenCharacters] = '#';
        ++m_nTokenCharacters;
        if (m_nTokenCharacters >= CSCRIPTCOMPILER_MAX_TOKEN_LENGTH)
        {
            return STRREF_CSCRIPTCOMPILER_ERROR_TOKEN_TOO_LONG;
        }
        return 0;
    }

    return STRREF_CSCRIPTCOMPILER_ERROR_UNEXPECTED_CHARACTER;
}

int CScriptCompilerInternal::ParseCharacterComma()
{
    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_UNKNOWN)
    {
        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_COMMA;
        return HandleToken();
    }

    return STRREF_CSCRIPTCOMPILER_ERROR_UNEXPECTED_CHARACTER;
}

int CScriptCompilerInternal::ParseCharacterTilde()
{
    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_UNKNOWN)
    {
        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_TILDE;
        return HandleToken();
    }

    return STRREF_CSCRIPTCOMPILER_ERROR_UNEXPECTED_CHARACTER;
}

int CScriptCompilerInternal::ParseCharacterCarat(int nCharNext)
{
    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_UNKNOWN)
    {
        if (nCharNext == '=')
        {
            m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_XOR;
            int nReturnValue = HandleToken();
            if (nReturnValue < 0)
            {
                return nReturnValue;
            }
            return 1;
        }

        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_EXCLUSIVE_OR;
        return HandleToken();
    }

    return STRREF_CSCRIPTCOMPILER_ERROR_UNEXPECTED_CHARACTER;
}

int CScriptCompilerInternal::ParseCharacterSemicolon()
{
    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_UNKNOWN)
    {
        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_SEMICOLON;
        return HandleToken();
    }

    return STRREF_CSCRIPTCOMPILER_ERROR_UNEXPECTED_CHARACTER;
}

int CScriptCompilerInternal::ParseCharacterPercentSign(int nCharNext)
{
    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_UNKNOWN)
    {
        if (nCharNext == '=')
        {
            m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_MODULUS;
            int nReturnValue = HandleToken();
            if (nReturnValue < 0)
            {
                return nReturnValue;
            }
            return 1;
        }

        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_MODULUS;
        return HandleToken();
    }

    return STRREF_CSCRIPTCOMPILER_ERROR_UNEXPECTED_CHARACTER;
}

int CScriptCompilerInternal::ParseCharacterPlusSign(int nCharNext)
{
    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_UNKNOWN)
    {
        if (nCharNext == '=')
        {
            m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_PLUS;
            int nReturnValue = HandleToken();
            if (nReturnValue < 0)
            {
                return nReturnValue;
            }
            return 1;
        }
        else if (nCharNext == '+')
        {
            m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_INCREMENT;
            int nReturnValue = HandleToken();
            if (nReturnValue < 0)
            {
                return nReturnValue;
            }
            return 1;
        }

        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_PLUS;
        return HandleToken();
    }

    return STRREF_CSCRIPTCOMPILER_ERROR_UNEXPECTED_CHARACTER;
}

int CScriptCompilerInternal::ParseCharacterEqualSign(int nCharNext)
{
    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_UNKNOWN)
    {
        if (nCharNext == '=')
        {
            m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_COND_EQUAL;
            int nReturnValue = HandleToken();
            if (nReturnValue < 0)
            {
                return nReturnValue;
            }
            return 1;
        }

        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_EQUAL;
        return HandleToken();
    }

    return STRREF_CSCRIPTCOMPILER_ERROR_UNEXPECTED_CHARACTER;
}

int CScriptCompilerInternal::ParseCharacterExclamationPoint(int nCharNext)
{
    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_UNKNOWN)
    {
        if (nCharNext == '=')
        {
            m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_COND_NOT_EQUAL;
            int nReturnValue = HandleToken();
            if (nReturnValue < 0)
            {
                return nReturnValue;
            }
            return 1;
        }

        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_BOOLEAN_NOT;
        return HandleToken();
    }

    return STRREF_CSCRIPTCOMPILER_ERROR_UNEXPECTED_CHARACTER;
}

int CScriptCompilerInternal::ParseCharacterLeftAngle(int nCharNext)
{
    int nReturnValue = 0;

    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_UNKNOWN)
    {
        if (nCharNext == '=')
        {
            m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_COND_LESS_EQUAL;
            nReturnValue = HandleToken();
            if (nReturnValue < 0)
            {
                return nReturnValue;
            }
            nReturnValue = 1;
        }
        else if (nCharNext == '<')
        {

            m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_SHIFT_LEFT;
            return 0;
        }
        else
        {
            m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_COND_LESS_THAN;
            nReturnValue = HandleToken();
        }
    }
    else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_SHIFT_LEFT)
    {
        if (nCharNext == '=')
        {
            m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_SHIFT_LEFT;
            nReturnValue = HandleToken();
            if (nReturnValue < 0)
            {
                return nReturnValue;
            }
            nReturnValue = 1;
        }
        else
        {
            m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_SHIFT_LEFT;
            nReturnValue = HandleToken();
        }
    }
    else
    {
        nReturnValue = STRREF_CSCRIPTCOMPILER_ERROR_UNEXPECTED_CHARACTER;
    }

    return nReturnValue;
}

int CScriptCompilerInternal::ParseCharacterRightAngle(int nCharNext)
{
    int nReturnValue = 0;

    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_UNKNOWN)
    {
        if (nCharNext == '=')
        {
            m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_COND_GREATER_EQUAL;
            nReturnValue = HandleToken();
            if (nReturnValue < 0)
            {
                return nReturnValue;
            }
            nReturnValue = 1;
        }
        else if (nCharNext == '>')
        {

            m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_SHIFT_RIGHT;
            nReturnValue = 0;
        }
        else
        {
            m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_COND_GREATER_THAN;
            nReturnValue = HandleToken();
        }
    }
    else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_SHIFT_RIGHT)
    {
        if (nCharNext == '>')
        {

            m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_UNSIGNED_SHIFT_RIGHT;
            nReturnValue = 0;
        }
        else if (nCharNext == '=')
        {
            m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_SHIFT_RIGHT;
            nReturnValue = HandleToken();
            if (nReturnValue < 0)
            {
                return nReturnValue;
            }
            nReturnValue = 1;
        }
        else
        {
            m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_SHIFT_RIGHT;
            nReturnValue = HandleToken();
        }
    }
    else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_UNSIGNED_SHIFT_RIGHT)
    {
        if (nCharNext == '=')
        {
            m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_USHIFT_RIGHT;
            nReturnValue = HandleToken();
            if (nReturnValue < 0)
            {
                return nReturnValue;
            }
            nReturnValue = 1;
        }
        else
        {
            m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_UNSIGNED_SHIFT_RIGHT;
            nReturnValue = HandleToken();
        }
    }
    else
    {
        nReturnValue = STRREF_CSCRIPTCOMPILER_ERROR_UNEXPECTED_CHARACTER;
    }

    return nReturnValue;
}

int CScriptCompilerInternal::ParseCharacterQuestionMark()
{
    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_UNKNOWN)
    {
        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_QUESTION_MARK;
        return HandleToken();
    }

    return STRREF_CSCRIPTCOMPILER_ERROR_UNEXPECTED_CHARACTER;
}

int CScriptCompilerInternal::ParseCharacterRightSquareBracket()
{
    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_UNKNOWN)
    {
        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_RIGHT_SQUARE_BRACKET;
        return HandleToken();
    }

    return STRREF_CSCRIPTCOMPILER_ERROR_UNEXPECTED_CHARACTER;
}

int CScriptCompilerInternal::ParseCharacterLeftSquareBracket()
{
    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_UNKNOWN)
    {
        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_LEFT_SQUARE_BRACKET;
        return HandleToken();
    }

    return STRREF_CSCRIPTCOMPILER_ERROR_UNEXPECTED_CHARACTER;
}

int CScriptCompilerInternal::ParseCharacterRightBracket()
{
    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_UNKNOWN)
    {
        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_RIGHT_BRACKET;
        return HandleToken();
    }

    return STRREF_CSCRIPTCOMPILER_ERROR_UNEXPECTED_CHARACTER;
}

int CScriptCompilerInternal::ParseCharacterLeftBracket()
{
    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_UNKNOWN)
    {
        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_LEFT_BRACKET;
        return HandleToken();
    }

    return STRREF_CSCRIPTCOMPILER_ERROR_UNEXPECTED_CHARACTER;
}

int CScriptCompilerInternal::ParseCharacterRightBrace()
{
    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_UNKNOWN)
    {
        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_RIGHT_BRACE;
        return HandleToken();
    }

    return STRREF_CSCRIPTCOMPILER_ERROR_UNEXPECTED_CHARACTER;
}

int CScriptCompilerInternal::ParseCharacterLeftBrace()
{
    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_UNKNOWN)
    {
        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_LEFT_BRACE;
        return HandleToken();
    }

    return STRREF_CSCRIPTCOMPILER_ERROR_UNEXPECTED_CHARACTER;
}

int CScriptCompilerInternal::ParseCharacterHyphen(int nCharNext)
{
    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_UNKNOWN)
    {
        if (nCharNext == '=')
        {
            m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_MINUS;
            int nReturnValue = HandleToken();
            if (nReturnValue < 0)
            {
                return nReturnValue;
            }
            return 1;
        }
        else if (nCharNext == '-')
        {
            m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_DECREMENT;
            int nReturnValue = HandleToken();
            if (nReturnValue < 0)
            {
                return nReturnValue;
            }
            return 1;
        }

        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_MINUS;
        return HandleToken();
    }

    return STRREF_CSCRIPTCOMPILER_ERROR_UNEXPECTED_CHARACTER;
}

int CScriptCompilerInternal::ParseCharacterQuotationMark()
{
    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_UNKNOWN)
    {
        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_STRING;
        m_nTokenCharacters = 0;
        return 0;
    }
    else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_STRING)
    {

        return HandleToken();
    }

    return STRREF_CSCRIPTCOMPILER_ERROR_UNEXPECTED_CHARACTER;
}

int CScriptCompilerInternal::ParseCharacterAsterisk(int nCharNext)
{
    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_UNKNOWN)
    {
        if (nCharNext == '=')
        {
            m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_MULTIPLY;
            int nReturnValue = HandleToken();
            if (nReturnValue < 0)
            {
                return nReturnValue;
            }
            return 1;
        }

        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_MULTIPLY;
        return HandleToken();
    }

    return STRREF_CSCRIPTCOMPILER_ERROR_UNEXPECTED_CHARACTER;
}

int CScriptCompilerInternal::ParseCharacterVerticalBar(int nCharNext)
{
    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_UNKNOWN)
    {
        if (nCharNext == '|')
        {
            m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_LOGICAL_OR;
            int nReturnValue = HandleToken();
            if (nReturnValue < 0)
            {
                return nReturnValue;
            }
            return 1;
        }
        else if (nCharNext == '=')
        {
            m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_OR;
            int nReturnValue = HandleToken();
            if (nReturnValue < 0)
            {
                return nReturnValue;
            }
            return 1;
        }

        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_INCLUSIVE_OR;
        return HandleToken();
    }

    return STRREF_CSCRIPTCOMPILER_ERROR_UNEXPECTED_CHARACTER;
}

int CScriptCompilerInternal::ParseCharacterAmpersand(int nCharNext)
{
    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_UNKNOWN)
    {
        if (nCharNext == '&')
        {
            m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_LOGICAL_AND;
            int nReturnValue = HandleToken();
            if (nReturnValue < 0)
            {
                return nReturnValue;
            }
            return 1;
        }
        else if (nCharNext == '=')
        {
            m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_AND;
            int nReturnValue = HandleToken();
            if (nReturnValue < 0)
            {
                return nReturnValue;
            }
            return 1;
        }

        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_BOOLEAN_AND;
        return HandleToken();
    }

    return STRREF_CSCRIPTCOMPILER_ERROR_UNEXPECTED_CHARACTER;
}

int CScriptCompilerInternal::ParseCharacterSlash(int nCharNext)
{
    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_UNKNOWN)
    {
        if (nCharNext == '*')
        {
            m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_CCOMMENT;
            return 1;
        }
        else if (nCharNext == '/')
        {
            m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_CPLUSCOMMENT;
            return 1;
        }
        else if (nCharNext == '=')
        {
            m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_ASSIGNMENT_DIVIDE;
            int nReturnValue = HandleToken();
            if (nReturnValue < 0)
            {
                return nReturnValue;
            }
            return 1;
        }

        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_DIVIDE;
        return HandleToken();
    }

    return STRREF_CSCRIPTCOMPILER_ERROR_UNEXPECTED_CHARACTER;
}

int CScriptCompilerInternal::TestIdentifierToken()
{
    if (m_nTokenCharacters == 2 && strncmp(m_pchToken, "if", 2) == 0)
    {
        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_KEYWORD_IF;
    }
    if (m_nTokenCharacters == 2 && strncmp(m_pchToken, "do", 2) == 0)
    {
        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_KEYWORD_DO;
    }
    if (m_nTokenCharacters == 4 && strncmp(m_pchToken, "else", 4) == 0)
    {
        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_KEYWORD_ELSE;
    }
    if (m_nTokenCharacters == 3 && strncmp(m_pchToken, "int", 3) == 0)
    {
        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_KEYWORD_INT;
    }
    if (m_nTokenCharacters == 5 && strncmp(m_pchToken, "float", 5) == 0)
    {
        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_KEYWORD_FLOAT;
    }
    if (m_nTokenCharacters == 6 && strncmp(m_pchToken, "string", 6) == 0)
    {
        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_KEYWORD_STRING;
    }
    if (m_nTokenCharacters == 6 && strncmp(m_pchToken, "object", 6) == 0)
    {
        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_KEYWORD_OBJECT;
    }
    if (m_nTokenCharacters == 6 && strncmp(m_pchToken, "return", 6) == 0)
    {
        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_KEYWORD_RETURN;
    }
    if (m_nTokenCharacters == 5 && strncmp(m_pchToken, "while", 5) == 0)
    {
        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_KEYWORD_WHILE;
    }
    if (m_nTokenCharacters == 3 && strncmp(m_pchToken, "for", 3) == 0)
    {
        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_KEYWORD_FOR;
    }
    if (m_nTokenCharacters == 4 && strncmp(m_pchToken, "void", 4) == 0)
    {
        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_KEYWORD_VOID;
    }
    if (m_nTokenCharacters == 4 && strncmp(m_pchToken, "case", 4) == 0)
    {
        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_KEYWORD_CASE;
    }
    if (m_nTokenCharacters == 5 && strncmp(m_pchToken, "break", 5) == 0)
    {
        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_KEYWORD_BREAK;
    }
    if (m_nTokenCharacters == 6 && strncmp(m_pchToken, "struct", 6) == 0)
    {
        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_KEYWORD_STRUCT;
    }
    if (m_nTokenCharacters == 6 && strncmp(m_pchToken, "action", 6) == 0)
    {
        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_KEYWORD_ACTION;
    }
    if (m_nTokenCharacters == 6 && strncmp(m_pchToken, "switch", 6) == 0)
    {
        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_KEYWORD_SWITCH;
    }
    if (m_nTokenCharacters == 7 && strncmp(m_pchToken, "default", 7) == 0)
    {
        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_KEYWORD_DEFAULT;
    }
    if (m_nTokenCharacters == 8 && strncmp(m_pchToken, "#include", 8) == 0)
    {
        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_KEYWORD_INCLUDE;
    }
    if (m_nTokenCharacters == 8 && strncmp(m_pchToken, "continue", 8) == 0)
    {
        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_KEYWORD_CONTINUE;
    }
    if (m_nTokenCharacters == 6 && strncmp(m_pchToken, "vector", 6) == 0)
    {
        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_KEYWORD_VECTOR;
    }
    if (m_nTokenCharacters == 7 && strncmp(m_pchToken, "#define", 7) == 0)
    {
        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_KEYWORD_DEFINE;
    }
    if (m_nTokenCharacters == 21 && strncmp(m_pchToken, "ENGINE_NUM_STRUCTURES", 21) == 0)
    {
        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_NUM_STRUCTURES_DEFINITION;
    }
    if (m_nTokenCharacters == 11 && strncmp(m_pchToken, "OBJECT_SELF", 11) == 0)
    {
        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_KEYWORD_OBJECT_SELF;
    }
    if (m_nTokenCharacters == 14 && strncmp(m_pchToken, "OBJECT_INVALID", 14) == 0)
    {
        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_KEYWORD_OBJECT_INVALID;
    }
    if (m_nTokenCharacters == 18 && strncmp(m_pchToken, "ENGINE_STRUCTURE_", 17) == 0 &&
        m_pchToken[17] >= '0' && m_pchToken[17] <= '9')
    {
        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE_DEFINITION;
    }

    int nCount;

    for (nCount = 0; nCount < m_nNumEngineDefinedStructures; ++nCount)
    {
        if (m_pbEngineDefinedStructureValid[nCount] == TRUE &&
            m_nTokenCharacters == m_psEngineDefinedStructureName[nCount].GetLength() &&
            strncmp(m_pchToken, m_psEngineDefinedStructureName[nCount].CStr(), m_nTokenCharacters) == 0)
        {
            m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_KEYWORD_ENGINE_STRUCTURE0 + nCount;
        }
    }

    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_IDENTIFIER && m_pchToken[0] == '#')
    {
        return STRREF_CSCRIPTCOMPILER_ERROR_ELLIPSIS_IN_IDENTIFIER;
    }

    BOOL bFound = FALSE;

    for (nCount = 0; nCount < m_nOccupiedIdentifiers && bFound == FALSE; ++nCount)
    {
        int nLength = m_pcIdentifierList[nCount].m_nIdentifierLength;
        bFound = (nLength == m_nTokenCharacters);

        for (int nCount2 = 0; bFound == TRUE && nCount2 < nLength; ++nCount2)
        {
            if (m_pcIdentifierList[nCount].m_psIdentifier[nCount2] != m_pchToken[nCount2])
            {
                bFound = FALSE;
            }
        }

        if (bFound == TRUE)
        {
            if (m_pcIdentifierList[nCount].m_nIdentifierType == 0)
            {

                if (m_pcIdentifierList[nCount].m_nReturnType == CSCRIPTCOMPILER_TOKEN_INTEGER_IDENTIFIER)
                {
                    m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_INTEGER;
                }
                else if (m_pcIdentifierList[nCount].m_nReturnType == CSCRIPTCOMPILER_TOKEN_FLOAT_IDENTIFIER)
                {
                    m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_FLOAT;
                }
                else if (m_pcIdentifierList[nCount].m_nReturnType == CSCRIPTCOMPILER_TOKEN_STRING_IDENTIFIER)
                {
                    m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_STRING;
                }

                int nSize = m_pcIdentifierList[nCount].m_psStringData.GetLength();
                for (int nCount3 = 0; nCount3 < nSize; ++nCount3)
                {
                    m_pchToken[nCount3] = (m_pcIdentifierList[nCount].m_psStringData.CStr())[nCount3];
                }
                m_nTokenCharacters = nSize;
            }
            else
            {
                m_nTokenStatus = m_pcIdentifierList[nCount].m_nReturnType;
            }
        }
    }

    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_IDENTIFIER)
    {
        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_VARIABLE;
    }

    return 0;
}

int CScriptCompilerInternal::HandleIdentifierToken()
{
    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_IDENTIFIER)
    {
        int nReturnValue = TestIdentifierToken();
        if (nReturnValue != 0)
        {
            return nReturnValue;
        }
    }

    return HandleToken();
}

int CScriptCompilerInternal::ParseCharacterPeriod()
{
    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_FLOAT)
    {
        return STRREF_CSCRIPTCOMPILER_ERROR_UNEXPECTED_CHARACTER;
    }
    else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_INTEGER)
    {
        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_FLOAT;
        m_pchToken[m_nTokenCharacters] = '.';
        ++m_nTokenCharacters;
        if (m_nTokenCharacters >= CSCRIPTCOMPILER_MAX_TOKEN_LENGTH)
        {
            return STRREF_CSCRIPTCOMPILER_ERROR_TOKEN_TOO_LONG;
        }
    }
    else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_IDENTIFIER)
    {

        int nReturnValue = HandleIdentifierToken();
        if (nReturnValue >= 0)
        {
            m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_STRUCTURE_PART_SPECIFY;
            nReturnValue = HandleToken();
        }
        return nReturnValue;
    }
    else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_UNKNOWN)
    {
        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_STRUCTURE_PART_SPECIFY;
        return HandleToken();
    }
    else
    {
        return STRREF_CSCRIPTCOMPILER_ERROR_UNEXPECTED_CHARACTER;
    }

    return 0;
}

int CScriptCompilerInternal::ParseStringCharacter(int nChar, int nCharNext)
{
    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_STRING)
    {
        if (nChar == '\n')
        {
            return STRREF_CSCRIPTCOMPILER_ERROR_UNTERMINATED_STRING_CONSTANT;
        }

        if (nChar == '"')
        {
            return HandleToken();
        }

        if (nChar == '\\')
        {

            if (nCharNext == 'n')
            {
                m_pchToken[m_nTokenCharacters] = '\n';
                ++m_nTokenCharacters;
                if (m_nTokenCharacters >= CSCRIPTCOMPILER_MAX_TOKEN_LENGTH)
                {
                    return STRREF_CSCRIPTCOMPILER_ERROR_TOKEN_TOO_LONG;
                }
                return 1;
            }
            return 0;
        }

        m_pchToken[m_nTokenCharacters] = (char)nChar;
        ++m_nTokenCharacters;
        if (m_nTokenCharacters >= CSCRIPTCOMPILER_MAX_TOKEN_LENGTH)
        {
            return STRREF_CSCRIPTCOMPILER_ERROR_TOKEN_TOO_LONG;
        }
        return 0;
    }

    return STRREF_CSCRIPTCOMPILER_ERROR_UNEXPECTED_CHARACTER;
}

int CScriptCompilerInternal::ParseNextCharacter(int nChar, int nCharNext)
{
    if (nChar == -1)
    {
        m_nTokenStatus = CSCRIPTCOMPILER_TOKEN_EOF;
        int nReturnValue = HandleToken();
        if (nReturnValue < 0)
        {
            return nReturnValue;
        }
        return 0;
    }

    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_CPLUSCOMMENT ||
        m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_CCOMMENT)
    {
        return ParseCommentedOutCharacter(nChar);
    }

    if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_STRING)
    {
        return ParseStringCharacter(nChar, nCharNext);
    }

    if (nChar >= '0' && nChar <= '9')
    {
        return ParseCharacterNumeric(nChar);
    }
    else if (nChar == '.')
    {
        return ParseCharacterPeriod();
    }
    else if ((nChar == 'x' || nChar == 'X') &&
             m_nTokenCharacters == 1 &&
             m_pchToken[0] == '0')
    {
        return ParseCharacterAlphabet(nChar);
    }
    else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_INTEGER ||
             m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_FLOAT)
    {

        int nCompiledCharacters = 0;
        if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_FLOAT && nChar == 'f')
        {
            nCompiledCharacters = 1;
        }

        int nReturnValue = HandleToken();
        if (nReturnValue < 0)
        {
            return nReturnValue;
        }
        else if (nCompiledCharacters > 0)
        {
            return nCompiledCharacters - 1;
        }
    }

    if ((nChar >= 'a' && nChar <= 'z') || (nChar >= 'A' && nChar <= 'Z') || nChar == '_')
    {
        return ParseCharacterAlphabet(nChar);
    }
    else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_IDENTIFIER)
    {
        int nReturnValue = HandleIdentifierToken();
        if (nReturnValue < 0)
        {
            return nReturnValue;
        }
    }
    else if (m_nTokenStatus == CSCRIPTCOMPILER_TOKEN_HEX_INTEGER)
    {
        int nReturnValue = HandleToken();
        if (nReturnValue < 0)
        {
            return nReturnValue;
        }
    }

    if (nChar == '/')
    {
        return ParseCharacterSlash(nCharNext);
    }
    else if (nChar == '*')
    {
        return ParseCharacterAsterisk(nCharNext);
    }
    else if (nChar == '&')
    {
        return ParseCharacterAmpersand(nCharNext);
    }
    else if (nChar == '|')
    {
        return ParseCharacterVerticalBar(nCharNext);
    }

    if (nChar == ' ' || nChar == '\n' || nChar == '\t')
    {
        return 0;
    }

    if (nChar == '"')
    {
        return ParseCharacterQuotationMark();
    }
    if (nChar == '-')
    {
        return ParseCharacterHyphen(nCharNext);
    }
    if (nChar == '{')
    {
        return ParseCharacterLeftBrace();
    }
    if (nChar == '}')
    {
        return ParseCharacterRightBrace();
    }
    if (nChar == '(')
    {
        return ParseCharacterLeftBracket();
    }
    if (nChar == ')')
    {
        return ParseCharacterRightBracket();
    }
    if (nChar == '[')
    {
        return ParseCharacterLeftSquareBracket();
    }
    if (nChar == ']')
    {
        return ParseCharacterRightSquareBracket();
    }
    if (nChar == '<')
    {
        return ParseCharacterLeftAngle(nCharNext);
    }
    if (nChar == '>')
    {
        return ParseCharacterRightAngle(nCharNext);
    }
    if (nChar == '!')
    {
        return ParseCharacterExclamationPoint(nCharNext);
    }
    if (nChar == '=')
    {
        return ParseCharacterEqualSign(nCharNext);
    }
    if (nChar == '+')
    {
        return ParseCharacterPlusSign(nCharNext);
    }
    if (nChar == '%')
    {
        return ParseCharacterPercentSign(nCharNext);
    }
    if (nChar == ';')
    {
        return ParseCharacterSemicolon();
    }
    if (nChar == ',')
    {
        return ParseCharacterComma();
    }
    if (nChar == '^')
    {
        return ParseCharacterCarat(nCharNext);
    }
    if (nChar == '~')
    {
        return ParseCharacterTilde();
    }
    if (nChar == '#')
    {
        return ParseCharacterEllipsis();
    }
    if (nChar == '?')
    {
        return ParseCharacterQuestionMark();
    }
    if (nChar == ':')
    {
        return ParseCharacterColon();
    }

    return 0;
}
