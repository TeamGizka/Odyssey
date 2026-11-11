#include "exobase.h"
#include "nwsexpression.h"

static inline void NwsexpressionTrace2();
static inline void NwsexpressionTrace1();
#define NWSEXPRESSION_TRACE() NwsexpressionTrace2()
// PC 0x005E45F0
void CSWSExpressionList::DeleteAlternate(CSWSExpressionNode* pNode)
{
    while (pNode != NULL)
    {
        if (pNode->m_nType == EXPRESSION_NODE_ALTERNATIVE_END)
        {
            break;
        }

        CSWSExpressionNode* pNext = pNode->m_pNext;

        if (pNode->m_nType == EXPRESSION_NODE_ALTERNATIVES)
        {
            DeleteAlternate(pNode->m_pAlternate);
        }

        delete pNode;
        pNode = pNext;
    }
}
// PC 0x005E4640
int CSWSExpressionList::AddNode(CSWSExpressionNode* pNode)
{
    NwsexpressionTrace2();
    CSWSExpressionNode* pLast;

    if (m_pHead != NULL || m_pTail != NULL)
    {
        pLast = m_pCurrent;
        if (pLast == m_pTail)
        {
            m_pTail = pNode;
        }
        pNode->SetNext(pLast->GetNext());
        m_pCurrent->SetNext(pNode);
        m_pCurrent = pNode;
    }
    else
    {
        m_pHead = pNode;
        m_pTail = pNode;
        m_pCurrent = pNode;
    }
    return (int)pNode;
}
// PC 0x005E4680
CSWSExpressionNode* CSWSExpression::NewNode(int nType)
{
    NwsexpressionTrace2();
    CSWSExpressionNode* pNode = new CSWSExpressionNode;
    pNode->m_nType = nType;
    return pNode;
}
// PC 0x005E4700
void CSWSExpression::ClearAlternate(CSWSExpressionNode* pNode)
{
    NWSEXPRESSION_TRACE();
    for (; pNode != NULL; pNode = pNode->m_pNext)
    {
        if (pNode->m_nType == EXPRESSION_NODE_ALTERNATIVE_END)
        {
            break;
        }

        if (pNode->m_nType == EXPRESSION_NODE_ALTERNATIVES)
        {
            CSWSExpressionNode* pAlternate = pNode->m_pAlternate;
            ClearAlternate(pAlternate);
        }

        pNode->m_nVisits = 0;
    }
}
// PC 0x005E4740
void CSWSExpressionList::DeleteList()
{
    CSWSExpressionNode* pNode = m_pHead;

    while (pNode != NULL)
    {
        CSWSExpressionNode* pNext = pNode->m_pNext;

        if (pNode->m_nType == EXPRESSION_NODE_ALTERNATIVES)
        {
            DeleteAlternate(pNode->m_pAlternate);
        }

        delete pNode;
        pNode = pNext;
    }
}
// PC 0x005E4780
void CSWSExpression::ClearGraph()
{
    CSWSExpressionNode* pNode = m_pList->m_pHead;

    while (pNode != NULL)
    {
        if (pNode->m_nType == EXPRESSION_NODE_ALTERNATIVES)
        {
            ClearAlternate(pNode->m_pAlternate);
        }

        pNode->m_nVisits = 0;
        pNode = pNode->m_pNext;
    }
}
// PC 0x005E47B0
CSWSExpression::CSWSExpression()
{
    m_pList = NULL;
    m_bCaseSensitive = 0;
    m_nReserved = 0;
}
// PC 0x005E4800
CSWSExpression::~CSWSExpression()
{
    if (m_pList != NULL)
    {
        m_pList->DeleteList();
        if (m_pList != NULL)
        {
            delete m_pList;
        }
    }

    for (int i = 0; i < m_lstMatches.num; i++)
    {
        if (m_lstMatches[i] != NULL)
        {
            delete m_lstMatches[i];
        }
    }

    m_lstMatches.SetSize(0);
}
// PC 0x005E48D0
int CSWSExpression::ParseString(CExoString sPattern)
{
    int nDepth = 0;

    if (m_bCaseSensitive == 0)
    {
        sPattern = sPattern.LowerCase();
    }

    if (m_sPattern == sPattern)
    {
        return 1;
    }

    if (m_pList != NULL)
    {
        m_pList->DeleteList();
        if (m_pList != NULL)
        {
            delete m_pList;
        }
    }

    int nPos = 0;
    CExoArrayList<CSWSExpressionList*> lstLists;

    lstLists.Add(new CSWSExpressionList);

    CExoArrayList<char> lstText;

    int nLength = sPattern.GetLength();
    while (nPos < nLength)
    {
        char cChar = sPattern[nPos];

        switch (cChar)
        {
        case '*':
            nPos++;
            if (nPos < nLength)
            {
                switch (sPattern[nPos])
                {
                case '*':
                    lstLists[nDepth]->AddNode(
                        NewNode(EXPRESSION_NODE_ANYTHING));
                    break;
                case 'w':
                    lstLists[nDepth]->AddNode(
                        NewNode(EXPRESSION_NODE_WHITESPACE));
                    break;
                case 'p':
                    lstLists[nDepth]->AddNode(
                        NewNode(EXPRESSION_NODE_PUNCTUATION));
                    break;
                case 'a':
                    lstLists[nDepth]->AddNode(
                        NewNode(EXPRESSION_NODE_LETTER));
                    break;
                case 'n':
                    lstLists[nDepth]->AddNode(
                        NewNode(EXPRESSION_NODE_DIGIT));
                    break;
                case 'A':
                    lstLists[nDepth]->AddNode(
                        NewNode(EXPRESSION_NODE_ALPHANUMERIC));
                    break;
                default:
                    return -1004;
                }
                break;
            }
            return -1003;

        case '(':
            nDepth++;
            lstLists.Add(new CSWSExpressionList);
            break;

        case '|':
        {
            if (lstLists[nDepth]->GetHead() == NULL)
            {
                return -1002;
            }

            CSWSExpressionNode* pEnd = NewNode(EXPRESSION_NODE_ALTERNATIVE_END);
            CSWSExpressionNode* pAlternatives =
                NewNode(EXPRESSION_NODE_ALTERNATIVES);

            lstLists[nDepth]->AddNode(pEnd);
            pAlternatives->m_pAlternate = lstLists[nDepth]->GetHead();
            pAlternatives->m_pNext = pEnd;
            lstLists[nDepth]->SetHead(pAlternatives);
            lstLists[nDepth]->SetCurrent(pAlternatives);
            break;
        }

        case ')':
        {
            if (nDepth < 1)
            {
                return -1000;
            }

            if (lstLists[nDepth - 1]->GetHead() == NULL)
            {
                lstLists[nDepth - 1]->SetHead(lstLists[nDepth]->GetHead());
                lstLists[nDepth - 1]->SetTail(lstLists[nDepth]->GetTail());
                lstLists[nDepth - 1]->SetCurrent(lstLists[nDepth]->GetTail());
            }
            else
            {
                lstLists[nDepth]->GetTail()->m_pNext =
                    lstLists[nDepth - 1]->GetCurrent()->m_pNext;
                if (lstLists[nDepth - 1]->GetCurrent() ==
                    lstLists[nDepth - 1]->GetTail())
                {
                    lstLists[nDepth - 1]->SetTail(lstLists[nDepth]->GetTail());
                }
                lstLists[nDepth - 1]->GetCurrent()->m_pNext = lstLists[nDepth]->GetHead();
                lstLists[nDepth - 1]->SetCurrent(lstLists[nDepth]->GetTail());
            }

            if (lstLists[nDepth] != NULL)
            {
                delete lstLists[nDepth];
            }
            lstLists.DelIndex(nDepth);
            nDepth--;
            break;
        }

        default:
            lstText.Add(cChar);

            char cNext;
            if (nPos + 1 >= sPattern.GetLength() ||
                (cNext = sPattern[nPos + 1]) == '*' || cNext == '|' ||
                cNext == '(' || cNext == ')')
            {
                CSWSExpressionNode* pNode = NewNode(EXPRESSION_NODE_LITERAL);
                CExoString sText(lstText.element, lstText.num);
                pNode->m_sText = sText;
                lstText.num = 0;
                lstLists[nDepth]->AddNode(pNode);
            }
            break;
        }
        nPos++;
    }

    if (nDepth > 0)
    {
        return -1001;
    }

    CSWSExpressionNode* pTail = lstLists[0]->GetTail();
    if (pTail != NULL)
    {
        pTail->m_pNext = NewNode(EXPRESSION_NODE_END);
    }
    else
    {
        lstLists[0]->AddNode(NewNode(EXPRESSION_NODE_END));
    }

    m_pList = lstLists[0];
    m_sPattern = sPattern;
    return 0;
}
// PC 0x005E4FC0
int CSWSExpression::TestString(const CExoString& sString, int bCapture)
{
    CExoString sText;

    if (m_bCaseSensitive == 0)
    {
        sText = sString.LowerCase();
    }
    else
    {
        sText = sString;
    }

    if (m_pList == NULL)
    {
        return 0;
    }

    CSWSExpressionNode* pNode = m_pList->m_pHead;
    CExoArrayList<CSWSExpressionNode*> lstStack;

    lstStack.Add(NewNode(EXPRESSION_NODE_ALTERNATIVE_END));

    int nPos = 0;
    int nLength = sText.GetLength();
    int nTop = 0;
    int bBacktrack = FALSE;

    while (pNode != NULL)
    {
        pNode->m_nVisits++;

        switch (pNode->m_nType)
        {
        case EXPRESSION_NODE_END:
            bBacktrack = TRUE;
            if (nPos == nLength)
            {
                if (bCapture != 0)
                {
                    for (int j = 0; j < m_lstMatches.num; j++)
                    {
                        if (m_lstMatches[j] != NULL)
                        {
                            delete m_lstMatches[j];
                        }
                    }
                    m_lstMatches.SetSize(0);

                    int nStart = 0;

                    m_lstMatches.Add(new CExoString(sString));

                    for (int k = 1; k < lstStack.num; k++)
                    {
                        CSWSExpressionNode* pStacked = lstStack[k];
                        if (pStacked->m_nType != EXPRESSION_NODE_ALTERNATIVES)
                        {
                            int nCount = pStacked->GetEnd() - nStart;

                            m_lstMatches.Add(new CExoString(
                                sString.CStr() + nStart, nCount));
                            nStart = lstStack[k]->m_nEnd;
                        }
                    }
                }

                delete lstStack[0];
                ClearGraph();
                return 1;
            }
            break;

        case EXPRESSION_NODE_LITERAL:
        {
            lstStack.Add(pNode);
            nTop++;

            int nTextLength = pNode->m_sText.GetLength();
            for (int i = 0; i < nTextLength; i++)
            {
                if (sText[nPos + i] != pNode->m_sText[i])
                {
                    bBacktrack = TRUE;
                    break;
                }
            }
            if (bBacktrack)
            {
                break;
            }

            nPos += nTextLength;
            pNode->m_nEnd = nPos;
            pNode = pNode->m_pNext;
            break;
        }

        case EXPRESSION_NODE_ANYTHING:
            lstStack.Add(pNode);
            nTop++;

            if (nPos == nLength)
            {
                if (pNode->m_nVisits > 1)
                {
                    delete lstStack[0];
                    ClearGraph();
                    return 0;
                }
            }
            else if (pNode->m_nVisits > 1)
            {
                nPos++;
            }
            pNode->m_nEnd = nPos;
            pNode = pNode->m_pNext;
            break;

        case EXPRESSION_NODE_ALTERNATIVES:
            pNode->m_nEnd = nPos;
            lstStack.Add(pNode);
            nTop++;

            if (pNode->m_nVisits == 1)
            {
                pNode = pNode->m_pNext;
                break;
            }

            if (pNode->m_nVisits == 2)
            {
                pNode = pNode->m_pAlternate;
                break;
            }

            lstStack.DelIndex(nTop);
            nTop--;
            bBacktrack = TRUE;
            break;

        case EXPRESSION_NODE_WHITESPACE:
        case EXPRESSION_NODE_PUNCTUATION:
        case EXPRESSION_NODE_LETTER:
        case EXPRESSION_NODE_DIGIT:
        case EXPRESSION_NODE_ALPHANUMERIC:
        {
            if (nPos == nLength)
            {
                delete lstStack[0];
                ClearGraph();
                return 0;
            }

            lstStack.Add(pNode);
            nTop++;

            char cChar = sText[nPos];

            if ((pNode->m_nType == EXPRESSION_NODE_LETTER && cChar >= 'a' &&
                 cChar <= 'z') ||
                (pNode->m_nType == EXPRESSION_NODE_DIGIT && cChar >= '0' &&
                 cChar <= '9') ||
                (pNode->m_nType == EXPRESSION_NODE_ALPHANUMERIC &&
                 ((cChar >= 'a' && cChar <= 'z') ||
                  (cChar >= '0' && cChar <= '9'))) ||
                (pNode->m_nType == EXPRESSION_NODE_WHITESPACE &&
                 cChar == ' ') ||
                (pNode->m_nType == EXPRESSION_NODE_PUNCTUATION &&
                 (cChar == '.' || cChar == '?' || cChar == '!')))
            {
                nPos++;
                pNode->m_nEnd = nPos;
                pNode = pNode->m_pNext;
                break;
            }

            lstStack.DelIndex(nTop);
            nTop--;
            bBacktrack = TRUE;
            break;
        }

        default:
            pNode = pNode->m_pNext;
            break;
        }

        if (bBacktrack)
        {
            bBacktrack = FALSE;
            do
            {
                pNode = lstStack[nTop];
                nPos = pNode->m_nEnd;
                lstStack.DelIndex(nTop);
                nTop--;

                if (nTop < 0)
                {
                    delete pNode;
                    ClearGraph();
                    return 0;
                }
            } while (pNode->m_nType == EXPRESSION_NODE_LITERAL);
        }
        else
        {
            pNode->m_nVisits = 0;
        }
    }

    delete lstStack[0];
    ClearGraph();
    return 1;
}

static inline void NwsexpressionTrace2()
{
    NwsexpressionTrace1();
}

static inline void NwsexpressionTrace1()
{
}
