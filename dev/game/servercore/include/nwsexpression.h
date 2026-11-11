#ifndef __NWSEXPRESSION_H__
#define __NWSEXPRESSION_H__

enum
{
    EXPRESSION_NODE_LITERAL = 0,
    EXPRESSION_NODE_ANYTHING = 1,
    EXPRESSION_NODE_ALTERNATIVES = 2,
    EXPRESSION_NODE_ALTERNATIVE_END = 3,
    EXPRESSION_NODE_WHITESPACE = 4,
    EXPRESSION_NODE_PUNCTUATION = 5,
    EXPRESSION_NODE_LETTER = 6,
    EXPRESSION_NODE_DIGIT = 7,
    EXPRESSION_NODE_ALPHANUMERIC = 8,
    EXPRESSION_NODE_END = 9
};

class CSWSExpressionNode
{
public:
    CSWSExpressionNode()
    {
        m_pNext = NULL;
        m_nVisits = 0;
    }

    CSWSExpressionNode* GetNext() { return m_pNext; }
    void SetNext(CSWSExpressionNode* pNext) { m_pNext = pNext; }
    int GetEnd() { return m_nEnd; }

    int m_nVisits;
    int m_nType;
    int m_nEnd;
    CSWSExpressionNode* m_pNext;
    CExoString m_sText;
    CSWSExpressionNode* m_pAlternate;
};

class CSWSExpressionList
{
public:
    CSWSExpressionList()
    {
        m_pTail = NULL;
        m_pHead = NULL;
        m_pCurrent = NULL;
    }

    void DeleteAlternate(CSWSExpressionNode* pNode);
    int AddNode(CSWSExpressionNode* pNode);
    void DeleteList();

    CSWSExpressionNode* GetHead() { return m_pHead; }
    CSWSExpressionNode* GetCurrent() { return m_pCurrent; }
    CSWSExpressionNode* GetTail() { return m_pTail; }
    void SetHead(CSWSExpressionNode* pNode) { m_pHead = pNode; }
    void SetCurrent(CSWSExpressionNode* pNode) { m_pCurrent = pNode; }
    void SetTail(CSWSExpressionNode* pNode) { m_pTail = pNode; }

    CSWSExpressionNode* m_pHead;
    CSWSExpressionNode* m_pCurrent;
    CSWSExpressionNode* m_pTail;
};

class CSWSExpression
{
public:
    CSWSExpression();
    ~CSWSExpression();

    CSWSExpressionNode* NewNode(int nType);
    void ClearAlternate(CSWSExpressionNode* pNode);
    void ClearGraph();
    int ParseString(CExoString sPattern);
    int TestString(const CExoString& sString, int bCapture);

    int m_nReserved;
    CExoArrayList<CExoString*> m_lstMatches;
    int m_bCaseSensitive;
    CSWSExpressionList* m_pList;
    CExoString m_sPattern;
};

#endif
