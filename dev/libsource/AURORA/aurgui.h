#ifndef AURGUI_H
#define AURGUI_H

class Material;
class Vector;
#include "aurmaterial.h"
#include "aurrendergl.h"

enum
{
    VIEWPORT_X = 0,
    VIEWPORT_Y = 1,
    VIEWPORT_WIDTH = 2,
    VIEWPORT_HEIGHT = 3,
    VIEWPORT_SCISSOR = 4
};

enum
{
    ALIGNMENT_LEFT = 0x01,
    ALIGNMENT_HCENTER = 0x02,
    ALIGNMENT_RIGHT = 0x04,
    ALIGNMENT_HORIZONTAL = 0x07,
    ALIGNMENT_TOP = 0x08,
    ALIGNMENT_VCENTER = 0x10,
    ALIGNMENT_BOTTOM = 0x20,
    ALIGNMENT_VERTICAL = 0x38
};

class CAurGUIDraw
{
public:
    virtual void Draw() = 0;
#ifndef _XBOX
    CAurGUIDraw()
    {
    }
#endif
#ifdef _XBOX
    CAurGUIDraw(); ~CAurGUIDraw();
#else
    ~CAurGUIDraw()
    {
    }
#endif

    static short cm_nCurrentViewport;
    static float cm_fViewportAlpha[10];
    static short cm_nViewportStack[10][5];
};

struct CAurFontGlyph
{
    float m_fU;
    float m_fV;
    float m_fW;
};

class CAurGUIImage : public CAurGUIDraw
{
public:

    enum GUIImageMapping
    {
        GUI_IMAGE_MAPPING_NORMAL = 0,
        GUI_IMAGE_MAPPING_MIRRORED = 4,
        GUI_IMAGE_MAPPING_REVERSED = 8,
        GUI_IMAGE_MAPPING_MASK = 12
    };

#ifdef _XBOX
    CAurGUIImage();
#else
    CAurGUIImage()
    {
    }
#endif

    virtual ~CAurGUIImage();

    virtual void SetTexture(char* sTexture) = 0;
    virtual int GetImageHeight() = 0;
    virtual int GetImageWidth() = 0;
    virtual void Draw(float fAlpha) = 0;
    virtual void Draw(int nX, int nY, int nWidth, int nHeight,
                      unsigned char nFlags, float fRotation,
                      const Vector& vColor, float fAlpha) = 0;
    virtual void Draw(float fX, float fY, float fWidth, float fHeight,
                      unsigned char nFlags, float fRotation,
                      const Vector& vColor, float fAlpha) = 0;
    virtual void DrawBuffered(int nX, int nY, int nWidth, int nHeight,
                              unsigned char nFlags, float fRotation,
                              const Vector& vColor, float fAlpha) = 0;
    virtual void DrawBuffered(float fX, float fY, float fWidth, float fHeight,
                              unsigned char nFlags, float fRotation,
                              const Vector& vColor, float fAlpha) = 0;

    static void FlushBuffer(float fAlpha);

    static Material* cm_pMaterial;
    static float cm_fAlpha;
    static int cm_nGUITID;
    static int cm_nGUIBufferSizeUsed;
    static char cm_bIsVTC;
    static float cm_fGUIBuffer[];
};

class CAurGUIImageInternal : public CAurGUIImage
{
public:
    CAurGUIImageInternal();
    CAurGUIImageInternal(char* sTexture);
    virtual ~CAurGUIImageInternal();
    virtual void SetTexture(char* sTexture);
    virtual int GetImageHeight();
    virtual int GetImageWidth();
    virtual void Draw(float fAlpha);
    virtual void Draw(int nX, int nY, int nWidth, int nHeight,
                      unsigned char nFlags, float fRotation,
                      const Vector& vColor, float fAlpha);
    virtual void Draw(float fX, float fY, float fWidth, float fHeight,
                      unsigned char nFlags, float fRotation,
                      const Vector& vColor, float fAlpha);
    virtual void Draw();
    virtual void DrawBuffered(int nX, int nY, int nWidth, int nHeight,
                              unsigned char nFlags, float fRotation,
                              const Vector& vColor, float fAlpha);
    virtual void DrawBuffered(float fX, float fY, float fWidth, float fHeight,
                              unsigned char nFlags, float fRotation,
                              const Vector& vColor, float fAlpha);

    Material* m_pMaterial;
};

class CAurGUIString : public CAurGUIDraw
{
public:
    virtual ~CAurGUIString();

    virtual Vector GetColor() = 0;
    virtual void SetColor(const Vector& vColor) = 0;
    virtual int GetStringSize() = 0;
    virtual void SetStringSize(short nSize) = 0;
    virtual void SetString(char* sString) = 0;
    virtual void SetAlignment(unsigned char nAlignment) = 0;
    virtual unsigned char GetAlignment() = 0;
    virtual void SetAlignmentVertical(unsigned char nAlignment) = 0;
    virtual int GetAlignmentVertical() = 0;
    virtual void SetAlignmentHorizontal(unsigned char nAlignment) = 0;
    virtual int GetAlignmentHorizontal() = 0;
    virtual void SetCursorIndex(short nIndex) = 0;
    virtual short GetCursorIndex() = 0;
    virtual void DisableCursor() = 0;
    virtual char* GetFontTexture() = 0;
    virtual void SetFontTexture(char* sFont) = 0;
    virtual int GetFontPixelHeight() = 0;
    virtual void WrapStrings(int nWidth) = 0;
    virtual int GetIdealPixelHeight(int nWidth) = 0;
    virtual int GetNumLines() = 0;
    virtual void GetPositionBoundingRect(int& nX, int& nY) = 0;
    virtual void SetPositionBoundingRect(int nX, int nY) = 0;
    virtual void GetPositionBoundingRect(float& fX, float& fY) = 0;
    virtual void SetPositionBoundingRect(float fX, float fY) = 0;
    virtual void GetSizeBoundingRect(int& nWidth, int& nHeight) = 0;
    virtual void SetSizeBoundingRect(int nWidth, int nHeight) = 0;
    virtual void GetSizeBoundingRect(float& fWidth, float& fHeight) = 0;
    virtual void SetSizeBoundingRect(float fWidth, float fHeight) = 0;
    virtual void DrawBoundingRect(Vector vColor, float fAlpha) = 0;
    virtual void Draw(float fAlpha) = 0;
};

class CAurGUIStringInternal : public CAurGUIString
{
public:
#ifdef _XBOX
    CAurGUIStringInternal(char* sString, char* sFont, Vector vColor);
#else
    CAurGUIStringInternal(char* sString, char* sFont, Vector vColor,
                          float fScale);
#endif
    virtual ~CAurGUIStringInternal();
    virtual Vector GetColor()  // PC 0x00470CF0 GOG 0x00470E30
    {
        return m_vColor;
    }
    virtual void SetColor(const Vector& vColor)  // PC 0x00470D10 GOG 0x00470E50
    {
        m_vColor = vColor;
    }
    virtual int GetStringSize()  // PC 0x00470D30 GOG 0x00470E70
    {
        return strlen(m_sString);
    }
    virtual void SetStringSize(short nSize);
    virtual void SetString(char* sString);
    virtual void SetAlignment(unsigned char nAlignment)  // PC 0x00470D40 GOG 0x00470E80
    {
        m_nAlignmentBits = nAlignment;
    }
    virtual unsigned char GetAlignment()  // PC 0x00470D50 GOG 0x00470E90
    {
        return m_nAlignmentBits;
    }
    virtual void SetAlignmentVertical(unsigned char nAlignment)  // PC 0x00470D60 GOG 0x00470EA0
    {
        m_nAlignmentBits = nAlignment | (m_nAlignmentBits & ALIGNMENT_HORIZONTAL);
    }
    virtual int GetAlignmentVertical()  // PC 0x00470D80 GOG 0x00470EC0
    {
        return m_nAlignmentBits & ALIGNMENT_VERTICAL;
    }
    virtual void SetAlignmentHorizontal(unsigned char nAlignment)  // PC 0x00470D90 GOG 0x00470ED0
    {
        m_nAlignmentBits = nAlignment | (m_nAlignmentBits & ALIGNMENT_VERTICAL);
    }
    virtual int GetAlignmentHorizontal()  // PC 0x00470DB0 GOG 0x00470EF0
    {
        return m_nAlignmentBits & ALIGNMENT_HORIZONTAL;
    }
    virtual void SetCursorIndex(short nIndex)  // PC 0x00470DC0 GOG 0x00470F00
    {
        m_nCursorIndex = nIndex;
    }
    virtual short GetCursorIndex()  // PC 0x00470DD0 GOG 0x00470F10
    {
        return m_nCursorIndex;
    }
    virtual void DisableCursor()  // PC 0x00470DE0 GOG 0x00470F20
    {
        m_nCursorIndex = -1;
    }
    virtual char* GetFontTexture();
    virtual void SetFontTexture(char* sFont);
    virtual int GetFontPixelHeight();
    virtual void WrapStrings(int nWidth);
    virtual int GetIdealPixelHeight(int nWidth);
    virtual int GetNumLines()  // PC 0x00786860 GOG 0x00786630
    {
        return m_lstLineLength.m_nCount;
    }
    virtual void GetPositionBoundingRect(int& nX, int& nY);
    virtual void SetPositionBoundingRect(int nX, int nY);
    virtual void GetPositionBoundingRect(float& fX, float& fY);
    virtual void SetPositionBoundingRect(float fX, float fY);
    virtual void GetSizeBoundingRect(int& nWidth, int& nHeight);
    virtual void SetSizeBoundingRect(int nWidth, int nHeight);
    virtual void GetSizeBoundingRect(float& fWidth, float& fHeight);
    virtual void SetSizeBoundingRect(float fWidth, float fHeight);
    virtual void DrawBoundingRect(Vector vColor, float fAlpha);
    virtual void Draw(float fAlpha);
    virtual void Draw()  // PC 0x00470E00 GOG 0x00470F40
    {
        Draw(1.0f);
    }

    int m_nPositionX;
    int m_nPositionY;
    int m_nWidth;
    int m_nHeight;
    char* m_sString;
    SafePointer<CAurTexture> m_pFont;
    Vector m_vColor;
    List<int> m_lstLineWidth;
    List<int> m_lstLineLength;
#ifndef _XBOX
    float m_fScale;
#endif
    short m_nCursorIndex;
    unsigned char m_nAlignmentBits : 8;

};

CAurGUIImageInternal* NewCAurGUIImage(char* sTexture);
#ifdef _XBOX
CAurGUIStringInternal* NewCAurGUIString(char* sString, char* sFont,
                                        const Vector& vColor);
#else
CAurGUIStringInternal* NewCAurGUIString(char* sString, char* sFont,
                                        const Vector& vColor, float fScale);
#endif
void AurGUIStartLayer();
void AurGUIStopLayer();
int AurGUISetupViewport(int nX, int nY, int nWidth, int nHeight,
                        const Vector& vColor, bool bScissor, float fAlpha);
void AurGUICloseViewport();

#endif
