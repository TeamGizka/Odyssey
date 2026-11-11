#ifndef AURSTRING_H
#define AURSTRING_H

#include "aurtexcontrol.h"
#ifndef _XBOX

class CAurString : public TextureController
{
public:
    CAurString(CAurTexture* pTexture);
    virtual void Control();
    virtual void Init();
    int LoadFont(char* szFontName);
    void DieWithoutParent();

    int m_nFont;
};

CAurString* NewCAurString(int nMaxLength);
#endif
#ifdef _XBOX

#include "aurmodel.h"

class CStringItem
{
public:
    virtual ~CStringItem();

    char* m_sString;
    int m_nLength;
    int m_nAllocated;
    short m_nState;
    short m_nColor;
    float m_fWidth;
    float m_fHeight;
};

struct CStringDisplayInfo
{
    int m_nFirstLine;
    int m_nLastLine;
    int m_nVisibleLines;
    float m_fMaxWidth;
    float m_fMaxHeight;
    float m_fTotalHeight;
    int m_nScrollLine;
    int m_nScrollPixel;
    bool m_bScrolled;
    int m_nReserved;
    float m_fFontScale;
};

class CAurString
{
public:
    CAurString() {}
    virtual ~CAurString() {}
    virtual CAurBehavior* AttachToObject(CAurObject* pObject, char* sPart, int nFlags) = 0;
    virtual CAurBehavior* AttachToPart(CAurObject* pObject, CAurPart* pPart, int nFlags) = 0;
    virtual CAurBehavior* AttachCornersToObjects(CAurObject* pObject, char* sPart1, char* sPart2) = 0;
    virtual int SetPositionOffset(Vector vOffset) = 0;
    virtual Vector SetPosition(Vector vPosition) = 0;
    virtual void AttachModel(CAurObject* pModel, int nIndex) = 0;
    virtual CAurObject* GetAttachedModel(int nIndex) = 0;
    virtual void* GetExternalData() = 0;
    virtual void SetExternalData(void* pData) = 0;
    virtual int SetFont(char* sFontName) = 0;
    virtual int SetFontPassword(char* sFontName, char cPassword) = 0;
    virtual int GetCurrentFontIndex() = 0;
    virtual void SetAlignment(float fAlignment) = 0;
    virtual void SetAlignmentVertical(float fAlignment) = 0;
    virtual void SetColorDefault(float fRed, float fGreen, float fBlue, float fAlpha) = 0;
    virtual void SetIndexedColor(int nIndex, float fRed, float fGreen, float fBlue, float fAlpha) = 0;
    virtual int ScrollToLine(int nLine) = 0;
    virtual int ScrollToPixel(int nPixel) = 0;
    virtual float GetLinePixelWidth(int nLine) = 0;
    virtual float GetLinePixelHeight(int nLine) = 0;
    virtual void SetLinePixelHeight(int nLine, float fHeight) = 0;
    virtual void RefreshDisplay() = 0;
    virtual CStringDisplayInfo* GetDisplayInfo() = 0;
    virtual void SetMaxWidth(float fWidth) = 0;
    virtual void SetMaxHeight(float fHeight) = 0;
    virtual int SetText(char* sText) = 0;
    virtual int AddText(char* sText) = 0;
    virtual char* GetTextLine(int nLine) = 0;
    virtual int GetTextLineCount() = 0;
    virtual int GetLineState(int nLine) = 0;
    virtual void SetLineState(int nLine, int nState) = 0;
    virtual void SetTextWrapping(bool bWrap) = 0;
    virtual void SetLineColor(int nLine, int nColor) = 0;
    virtual int GetLineColor(int nLine) = 0;
    virtual int GetNumColors() = 0;
    virtual void AddLine(char* sLine) = 0;
    virtual void InsertLine(char* sLine, int nLine) = 0;
    virtual void DeleteLine(int nLine) = 0;
    virtual void SetLine(int nLine, char* sLine) = 0;
    virtual bool InsertTextAtCaret(char* sText) = 0;
    virtual bool DeleteTextAtCaret(int nCount) = 0;
    virtual int GetCaretLinePos() = 0;
    virtual bool SetCaretLinePos(int nLine) = 0;
    virtual int GetCaretColumnPos() = 0;
    virtual bool SetCaretColumnPos(int nColumn) = 0;
    virtual int MoveCaretTo(Vector vPosition) = 0;
    virtual void SetCaretVisible(bool bVisible) = 0;
    virtual int SuggestDeleteCurrentFont() = 0;
    virtual CStringItem** GetStringsArray() = 0;
    virtual void SetColorSelected(float fRed, float fGreen, float fBlue, float fAlpha) = 0;
};

class StringGob : public Gob, public CAurString
{
public:
    StringGob(int nMaxLength);
    virtual ~StringGob();

    virtual CAurObject* HitCheck(Vector vStart, Vector vEnd, Vector* pHit, CHitInfo* pInfo) { return NULL; }
    virtual void GetMinimumBoundingBox(Vector& vMinimum, Vector& vMaximum) {}
    virtual void GetMinimumBoundingSphere(Vector& vPosition, float& fRadius) {}
    virtual CAurObject* AsCAurObject() { return NULL; }
    virtual StringGob* AsString() { return this; }

    virtual CAurBehavior* AttachToObject(CAurObject* pObject, char* sPart, int nFlags) { return NULL; }
    virtual CAurBehavior* AttachToPart(CAurObject* pObject, CAurPart* pPart, int nFlags) { return NULL; }
    virtual CAurBehavior* AttachCornersToObjects(CAurObject* pObject, char* sPart1, char* sPart2) { return NULL; }
    virtual int SetPositionOffset(Vector vOffset) { return 0; }
    virtual Vector SetPosition(Vector vPosition) { return Vector(0.0f, 0.0f, 0.0f); }
    virtual void AttachModel(CAurObject* pModel, int nIndex) {}
    virtual CAurObject* GetAttachedModel(int nIndex) { return NULL; }
    virtual void* GetExternalData() { return m_pExternalData; }
    virtual void SetExternalData(void* pData) { m_pExternalData = pData; }
    virtual int SetFont(char* sFontName) { return 0; }
    virtual int SetFontPassword(char* sFontName, char cPassword) { return 0; }
    virtual int GetCurrentFontIndex() { return m_pFont->m_nLoadedFontIndex; }
    virtual void SetAlignment(float fAlignment) { m_fAlignment = fAlignment; }
    virtual void SetAlignmentVertical(float fAlignment) { m_fAlignmentVertical = fAlignment; }
    virtual void SetColorDefault(float fRed, float fGreen, float fBlue, float fAlpha) {}
    virtual void SetIndexedColor(int nIndex, float fRed, float fGreen, float fBlue, float fAlpha) {}
    virtual int ScrollToLine(int nLine) { return 0; }
    virtual int ScrollToPixel(int nPixel) { return -1; }
    virtual float GetLinePixelWidth(int nLine) { return 0.0f; }
    virtual float GetLinePixelHeight(int nLine) { return 0.0f; }
    virtual void SetLinePixelHeight(int nLine, float fHeight) {}
    virtual void RefreshDisplay() {}
    virtual CStringDisplayInfo* GetDisplayInfo() { return &m_DisplayInfo; }
    virtual void SetMaxWidth(float fWidth) { m_DisplayInfo.m_fMaxWidth = fWidth; }
    virtual void SetMaxHeight(float fHeight) { m_DisplayInfo.m_fMaxHeight = fHeight; }
    virtual int SetText(char* sText) { return 1; }
    virtual int AddText(char* sText) { return 1; }
    virtual char* GetTextLine(int nLine) { return NULL; }
    virtual int GetTextLineCount() { return m_lstStrings.m_nCount; }
    virtual int GetLineState(int nLine) { return m_lstStrings[nLine]->m_nState; }
    virtual void SetLineState(int nLine, int nState) { m_lstStrings[nLine]->m_nState = nState; }
    virtual void SetTextWrapping(bool bWrap) { m_bTextWrapping = bWrap; }
    virtual void SetLineColor(int nLine, int nColor) { m_lstStrings[nLine]->m_nColor = nColor; }
    virtual int GetLineColor(int nLine) { return m_lstStrings[nLine]->m_nColor; }
    virtual int GetNumColors() { return m_lstColors.m_nCount; }
    virtual void AddLine(char* sLine) {}
    virtual void InsertLine(char* sLine, int nLine) {}
    virtual void DeleteLine(int nLine) {}
    virtual void SetLine(int nLine, char* sLine) {}
    virtual bool InsertTextAtCaret(char* sText) { return false; }
    virtual bool DeleteTextAtCaret(int nCount) { return false; }
    virtual int GetCaretLinePos() { return m_nCaretLinePos; }
    virtual bool SetCaretLinePos(int nLine) { return false; }
    virtual int GetCaretColumnPos() { return m_nCaretColumnPos; }
    virtual bool SetCaretColumnPos(int nColumn) { return true; }
    virtual int MoveCaretTo(Vector vPosition) { return 0; }
    virtual void SetCaretVisible(bool bVisible) { m_bCaretVisible = bVisible; }
    virtual int SuggestDeleteCurrentFont() { return 0; }
    virtual CStringItem** GetStringsArray() { return m_lstStrings.m_pList; }
    virtual void SetColorSelected(float fRed, float fGreen, float fBlue, float fAlpha)
    {
        SetIndexedColor(1, fRed, fGreen, fBlue, fAlpha);
    }

    virtual float GetAlignment() { return 0.0f; }
    virtual float GetAlignmentVertical() { return 0.0f; }
    virtual char* GetFontName() { return NULL; }
    virtual void GetColorDefault(float& fRed, float& fGreen, float& fBlue, float& fAlpha) {}

    bool m_bTextWrapping;
    CAurFont* m_pFont;
    char* m_sCaret;
    int m_nCaretLinePos;
    int m_nCaretColumnPos;
    float m_fCaretTime;
    float m_fAlignment;
    float m_fAlignmentVertical;
    List<CStringItem*> m_lstStrings;
    int m_nMaxLength;
    bool m_bCaretVisible;
    List<CStringColor> m_lstColors;
    CStringDisplayInfo m_DisplayInfo;
};
#endif
#endif
