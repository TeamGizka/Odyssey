#ifndef __SWGUIDRAWABLE_H__
#define __SWGUIDRAWABLE_H__

class CSWGuiExtent;
class CSWGuiTextParams;
class CSWGuiImageParams;
class CSWGuiBorderParams;
class CSWGuiDrawable;
class CSWGuiText;
class CSWGuiImage;
class CSWGuiBorder;
class CSWGuiObject;
class CSWGuiControl;
class CSWGuiNavigable;
class CSWGuiLabel;
class CSWGuiLabelHilight;
class CSWGuiButton;
class CSWGuiButtonToggle;
class CSWGuiListBox;
class CSWGuiSlider;
class CSWGuiProgressBar;
class CSWGuiScrollBar;
class CSWGuiCheckBox;
class CSWGuiEditbox;
class CSWGuiPanel;
class CSWGuiManager;
class CSWGuiMessageBox;
class CSWGuiTimedControl;
class CSWGuiInterfaceAction;
class CResGFF;
class CResStruct;

class CSWGuiExtent
{
public:

    CSWGuiExtent()
    {
        m_nLeft = m_nTop = m_nWidth = m_nHeight = 0;
    }

    CSWGuiExtent(int nLeft, int nTop, int nWidth, int nHeight)
    {
        m_nLeft = nLeft;
        m_nTop = nTop;
        m_nWidth = nWidth;
        m_nHeight = nHeight;
    }

    int Load(CResGFF* pGFF, CResStruct& cStruct);

    static CSWGuiExtent Intersection(const CSWGuiExtent& extentFirst,
                                     const CSWGuiExtent& extentSecond);
    static CSWGuiExtent Union(const CSWGuiExtent& extentFirst,
                              const CSWGuiExtent& extentSecond);

    int GetRight() const { return m_nLeft + m_nWidth; }
    int GetBottom() const { return m_nTop + m_nHeight; }

    int m_nLeft;
    int m_nTop;
    int m_nWidth;
    int m_nHeight;
};

#include "math3d.h"
#include "exoresman.h"
#include "exobase.h"

class CAurScene;
class CAurObject;
class CAurGUIStringInternal;
class CAurGUIImage;
class CAurGUIImageInternal;

#define SWGUI_TEXT_ALIGNMENT_DEFAULT 9

class CSWGuiTextParams
{
public:

    enum { ALIGNMENT_CENTER = 0x12, ALIGNMENT_DIALOG_REPLY = 34,
           ALIGNMENT_ITEM_COUNT = 0x22 };

    CSWGuiTextParams(const CExoString& sText, const CResRef& cFont,
                     const Vector& vColor, int nAlignment, float fAlpha);
    CSWGuiTextParams& operator=(const CSWGuiTextParams& cParams);
    void SetText(const CExoString& sText); void SetStrRef(unsigned long nStrRef);
    void RestoreDefaultColor(); void ReSetFont();
    void SetAlignment(int nAlignment); void SetColor(const Vector& vColor);
    void SetDefaultColor(const Vector& vColor);
    const Vector& GetColor();
    void SetBaseFont(const CResRef& cFont);
    void SetFont(const CResRef& cFont);
    void SetFontRename();
    void SetTextObject(CSWGuiText* pText);

    CExoString m_sText;
    int m_nStrRef;
    CResRef m_cFont;
    Vector m_vColor;
    float m_fAlpha;
#ifndef _XBOX
    Vector m_vOriginalColor;
#endif
    unsigned long m_nAlignment : 6;
    unsigned long m_bHilightText : 1;
    unsigned long m_nTextFlagsUnused : 25;
#ifndef _XBOX
    char m_szBaseFont[20];
#endif
    CSWGuiText* m_pText;

    void SetColor(const Vector* pColor);
    CSWGuiTextParams();
#ifdef _XBOX
    static CResRef s_cDefaultFont; static CResRef s_cConsoleFont; CSWGuiTextParams(unsigned long nStrRef, const CResRef& cFont, const Vector& vColor, int nAlignment, float fAlpha);
#endif
};

#define SWGUI_IMAGE_DRAW_STYLE_NORMAL 0
#define SWGUI_IMAGE_DRAW_STYLE_FLIP_VERTICAL 4
#define SWGUI_IMAGE_DRAW_STYLE_FLIP_HORIZONTAL 8

class CSWGuiImageParams
{
public:

    static const int ALIGNMENT_DEFAULT = 18;

    CResRef m_cImage;
    float m_fRotation;
    float m_fAlpha;
    Vector m_vColor;
    unsigned long m_bPulsing : 1;
    unsigned long m_bTiled : 1;
    unsigned long m_nDrawStyle : 4;
    unsigned long m_nAlignment : 6;
    unsigned long m_nImageFlagsUnused : 20;
    CSWGuiImage* m_pImage;
    CSWGuiImageParams(const CResRef& cImage, int bTiled,
                      int nDrawStyle, float fRotation, int nAlignment,
                      float fAlpha, const Vector& vColor);
    void SetImage(const CResRef& cImage);
    void SetImage(const CResRef& cImage, int bForceReload);
    void SetAlignment(int);
    void SetDrawStyle(int);
    void SetImageObject(CSWGuiImage*);
    CSWGuiImageParams& operator=(const CSWGuiImageParams& cParams);
};

#define SWGUI_BORDER_FILL_MODE_TILE 0
#define SWGUI_BORDER_FILL_MODE_CENTER 1

class CSWGuiBorderParams
{
public:

    static const unsigned long FILL_MODE_BACKGROUND = 2;

    static const unsigned long FILL_MODE_MAP = 0;

    CSWGuiBorderParams(const CResRef& cCorner, const CResRef& cEdge,
                       const CResRef& cFill, int nDimension, int nUnknown,
                       float fAlpha, const Vector& vColor, int bPulsing, int n8 = 0);
    void SetPulsingAlpha(int bPulsing, int nLow, int nHigh);
    void SetCornerImage(const CResRef& cImage, int bTile);
    void SetCornerImage(const CResRef& cImage);
    void SetEdgeImage(const CResRef& cImage, int bTile);
    void SetEdgeImage(const CResRef& cImage);
    CSWGuiBorderParams& operator=(const CSWGuiBorderParams& cParams);
    void SetBorder(CSWGuiBorder* pBorder);
    void SetFillImage(const CResRef& cImage, int bTile);
    void SetFillImage(const CResRef& cImage);
    int GetBorderDim() const;

    int GetInnerOffset() const { return m_nInnerOffset; }
    int GetVerticalInnerOffset() const { return m_n8 != 0 ? m_n8 : m_nInnerOffset; }
    void SetColor(const Vector& vColor) { m_vColor = vColor; }

    int m_nBorderDim;
    int m_nInnerOffset;
    int m_n8;

    float m_fRotation;
    float m_fHilightAlpha;
    Vector m_vColor;
    unsigned long m_nFillMode : 2;
    unsigned long m_bPulsing : 1;
    unsigned long m_bLowAlpha : 1;
    unsigned long m_bHighAlpha : 1;
    unsigned long m_nBorderFlagsUnused : 27;
    CResRef m_cCornerImage;
    CResRef m_cEdgeImage;
    CResRef m_cFillImage;
    CSWGuiBorder* m_pBorder;
};

class CSWGuiDrawable
{
public:

    virtual ~CSWGuiDrawable();
    virtual void SetExtent(const CSWGuiExtent& cExtent)  // PC 0x0040A2B0
    {
        m_cExtent = cExtent;
    }
    virtual int GetIdealHeight()  // PC 0x004065B0
    {
        return m_cExtent.m_nHeight;
    }

    virtual void Draw(float fElapsed) = 0;
    virtual void Load(CResGFF* pGFF, CResStruct& cStruct,
                      const CExoString& sName) = 0;

    static void UpdatePulsing(float fElapsed);
    static float m_fPulsingAlpha;
    static float m_fAlphaPulseTimer;
    static int m_bAlphaPulsingUp;
    static Vector NO_COLOURING;

    CSWGuiExtent m_cExtent;
};

class CSWGuiText : public CSWGuiDrawable
{
public:
    CSWGuiText();
    virtual ~CSWGuiText();
    virtual void SetExtent(const CSWGuiExtent& cExtent);
    virtual int GetIdealHeight();
    virtual void Draw(float fElapsed);
    virtual void Load(CResGFF* pGFF, CResStruct& cStruct,
                      const CExoString& sName);

    int GetHeight();
    int GetFontHeight();

    void WrapString();
    CSWGuiExtent GetExtent();
    CSWGuiExtent GetIdealWidthAndHeight();
#ifdef _XBOX

    void Initialize(const CSWGuiExtent& cExtent, const CSWGuiTextParams& cParams);
    void Initialize(const CSWGuiExtent& cExtent, const CSWGuiTextParams& cParams, float)
    {
        Initialize(cExtent, cParams);
    }
#else
    void Initialize(const CSWGuiExtent& cExtent, const CSWGuiTextParams& cParams,
                   float fScale);
#endif
    void SetText(const CExoString& sText);

    CAurGUIStringInternal* m_pString;
    CSWGuiTextParams m_cParams;
#ifndef _XBOX
    int m_nWrapWidth;
#endif
};

class CSWGuiImage : public CSWGuiDrawable
{
public:
    virtual ~CSWGuiImage();
    virtual void SetExtent(const CSWGuiExtent& cExtent);
    virtual void Draw(float fElapsed);
    virtual void Load(CResGFF* pGFF, CResStruct& cStruct,
                     const CExoString& sName);

    CSWGuiImage();
    CSWGuiExtent GetImageExtent();
    void Initialize(const CSWGuiExtent& cExtent,
                   const CSWGuiImageParams& cParams);

    CSWGuiExtent m_cImageExtent;
    CAurGUIImageInternal* m_pImageObject;
    CSWGuiImageParams m_cParams;
};

class CSWGuiBorder : public CSWGuiDrawable
{
public:
    CSWGuiBorder();
    virtual ~CSWGuiBorder();
    virtual void Draw(float fElapsed);
    virtual void Load(CResGFF* pGFF, CResStruct& cStruct,
                      const CExoString& sName);

    CSWGuiExtent GetInnerExtent();
    int SetPulsingAlpha(int bPulsing, int nLow, int nHigh);
    void Initialize(const CSWGuiExtent& cExtent,
                   const CSWGuiBorderParams& cParams);
    void FillTile(int nHeight, int nWidth, int nX, int nY,
                  float fAlpha, const Vector& vColor);
    void FillCenter(int nHeight, int nWidth, int nX, int nY,
                    float fAlpha, const Vector& vColor);

    CSWGuiBorderParams m_cParams;
    CAurGUIImageInternal* m_pCornerImage;
    CAurGUIImageInternal* m_pEdgeImage;
    CAurGUIImageInternal* m_pFillImage;
};

class CAurCamera;

class CSWGuiScene : public CSWGuiDrawable
{
public:
    CSWGuiScene();
    virtual ~CSWGuiScene();
    virtual void Draw(float fElapsed);
    virtual void Load(CResGFF* pGFF, CResStruct& cStruct,  // PC 0x006009E0
                           const CExoString& sName)
    {
    }

    int RemoveModel(int nModel, int bDestroy);
    CAurObject* GetModel(int nModel);
    int AddModel(CAurObject* pModel, int nModel, int bAttach);
    CAurObject* AddModel(const CExoString& sModel, int nFlags);

    CAurCamera* GetCamera();

    CAurScene* m_pScene;
    CAurCamera* m_pCamera;
    CExoArrayList<CAurObject*> m_lModels;
    Vector m_vColor;
};

#define SWGUI_EDITTEXT_MAX_LENGTH_UNLIMITED (-1)

class CSWGuiEditText : public CSWGuiText
{
public:
    CSWGuiEditText();
    void RemoveLastChar();
    void SetText(const CExoString& sText);
    void SetCaretVisible(int bVisible);
    void AddNewChar(char* szChar);

    short m_nMaxLength;
    unsigned char m_aUnknown72[2];
    unsigned long m_bCaretVisible : 1;
    unsigned long m_nEditFlagsUnused : 31;
    CExoString m_sText;
};

#endif
