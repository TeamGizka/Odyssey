#define WIN32_LEAN_AND_MEAN
#include "aurgui.h"
#include <windows.h>
#include <GL/gl.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "swmain.h"
#ifdef _XBOX

#define m_fScale 1.0f
#endif

static inline void AurguiTrace2();
static inline void AurguiTrace1();

#ifdef _XBOX

#define SetVertexData4f GLRender::SetVertexData4f
// PC 0x00459190
inline void AurGUIColor(unsigned int nColor)
{
    g_pD3DDevice->SetVertexDataColor(D3DVSDE_DIFFUSE, nColor);
}
#else
static void SetVertexData4f(Aurora::AuroraStreamMode nStream,
                            float fX, float fY, float fZ, float fW)
{
    switch (nStream)
    {
    case Aurora::STREAM_POSITION:
        glVertex4f(fX, fY, fZ, fW);
        break;
    case Aurora::STREAM_TEXCOORD:
        glTexCoord3f(fX, fY, fZ);
        break;
    case Aurora::STREAM_COLOR:
        glColor4f(fX, fY, fZ, fW);
        break;
    case Aurora::STREAM_NORMAL:
        glNormal3f(fX, fY, fZ);
        break;
    }
}
#endif

static inline CAurTexture* AsTexture(SafePointer<CAurTexture>& pPointer)
{
    return (CAurTexture*)pPointer.m_pObject;
}

int noguirender = 0;
int CAurGUIImage::cm_nGUIBufferSizeUsed = 0;
int CAurGUIImage::cm_nGUITID = 0;
Material* CAurGUIImage::cm_pMaterial = 0;
char CAurGUIImage::cm_bIsVTC = 0;
float CAurGUIImage::cm_fAlpha = 1.0f;

short CAurGUIDraw::cm_nCurrentViewport;

short CAurGUIDraw::cm_nViewportStack[10][5];

float CAurGUIImage::cm_fGUIBuffer[2048];
// PC 0x0045A2F0  Xbox 0x002C9B20
void CAurGUIStringInternal::WrapStrings(int nWidth)
{
    if (m_nPositionX != 0 || m_nWidth != 0)
    {
        CAurFontInfo* pInfo = AsTexture(m_pFont)->GetFontInfo();
        m_lstLineWidth.m_nCount = 0;
        m_lstLineLength.m_nCount = 0;

        unsigned char* pLineStart = (unsigned char*)m_sString;
        unsigned char* pChar = (unsigned char*)m_sString;

        short nBreakWidth = (short)((pInfo->m_lstLowerRight[*pChar].m_fX -
            pInfo->m_lstUpperLeft[*pChar].m_fX) * m_fScale *
            pInfo->m_fTextureWidth * 100.0f);
        int nLineWidth = nBreakWidth;
        short nWrapWidth = nWidth == -1 ? m_nWidth : nWidth;
        short nMinWidth = (short)((pInfo->m_lstLowerRight['o'].m_fX +
            pInfo->m_fSpacingR - pInfo->m_lstUpperLeft['o'].m_fX) *
            m_fScale * pInfo->m_fTextureWidth * 100.0f);

        if (strlen((char*)pLineStart) == 1 && nWrapWidth < nMinWidth)
            return;
        if (strlen((char*)pLineStart) <= 2 && nWrapWidth < 2 * nMinWidth)
            return;

        while (*pChar)
        {
            int nChar;
            unsigned char* pBreak = 0;
            while (*pChar == '\n')
            {
                pLineStart = ++pChar;
                m_lstLineWidth.Add(0);
                m_lstLineLength.Add(0);
            }

            if (*pChar == 0)
                nLineWidth = nWrapWidth;

            while (nLineWidth < nWrapWidth)
            {
                if (*pChar == '-' && isalpha(pChar[-1]) && isalpha(pChar[1]))
                {
                    pBreak = pChar;
                    nBreakWidth = nLineWidth;
                }
                ++pChar;
                if (*pChar == 0 || *pChar == '\n')
                    break;
                nChar = *pChar;
                nLineWidth += (int)(((pInfo->m_lstLowerRight[nChar].m_fX -
                    pInfo->m_lstUpperLeft[nChar].m_fX) * pInfo->m_fTextureWidth +
                    pInfo->m_fSpacingR) * m_fScale * 100.0f + 0.25f);
                if (*pChar == ' ')
                {
                    pBreak = pChar;
                    nBreakWidth = nLineWidth;
                }
            }

            if (*pChar != 0 && *pChar != '\n')
            {
                if (pBreak == 0)
                {
                    nBreakWidth = nLineWidth - (int)(
                        ((pInfo->m_lstLowerRight[*pChar].m_fX -
                        pInfo->m_lstUpperLeft[*pChar].m_fX) * pInfo->m_fTextureWidth +
                        pInfo->m_fSpacingR) * m_fScale * 100.0f);
                    --pChar;
                    if (pChar == (unsigned char*)m_sString)
                    {
                        m_lstLineWidth.m_nCount = 0;
                        m_lstLineLength.m_nCount = 0;
                        return;
                    }
                    nBreakWidth += (int)(
                        ((pInfo->m_lstLowerRight['-'].m_fX -
                        pInfo->m_lstUpperLeft['-'].m_fX) * pInfo->m_fTextureWidth +
                        pInfo->m_fSpacingR) * m_fScale * 100.0f) - (int)(
                        ((pInfo->m_lstLowerRight[*pChar].m_fX -
                        pInfo->m_lstUpperLeft[*pChar].m_fX) * pInfo->m_fTextureWidth +
                        pInfo->m_fSpacingR) * m_fScale * 100.0f);
                    m_lstLineLength.Add(pLineStart - pChar);
                    m_lstLineWidth.Add(nBreakWidth);
                }
                else
                {
                    if (*pBreak == ' ')
                    {
                        m_lstLineLength.Add(pBreak - pLineStart);
                        nBreakWidth += (int)(((pInfo->m_lstLowerRight[' '].m_fX -
                            pInfo->m_lstUpperLeft[' '].m_fX) * pInfo->m_fTextureWidth +
                            pInfo->m_fSpacingR) * m_fScale * -100.0f);
                    }
                    else
                    {
                        m_lstLineLength.Add(pBreak - pLineStart + 1);
                    }
                    m_lstLineWidth.Add(nBreakWidth);
                    pChar = pBreak + 1;
                }
            }
            else
            {
                m_lstLineWidth.Add(nLineWidth);
                m_lstLineLength.Add(pChar - pLineStart);
                if (*pChar == '\n')
                    ++pChar;
            }
            pLineStart = pChar;
            nChar = *pChar;
            nLineWidth = (int)((pInfo->m_lstLowerRight[nChar].m_fX -
                pInfo->m_lstUpperLeft[nChar].m_fX) * m_fScale *
                pInfo->m_fTextureWidth * 100.0f);
        }
    }
}

void AurGUIDumpWrappedLines(CAurGUIStringInternal* pString)
{
    List<int> lstLineStart;
    int nStart = 0;
    for (int i = 0; i < pString->m_lstLineLength.m_nCount; ++i)
    {
        lstLineStart.Add(nStart);
        nStart += pString->m_lstLineLength[i];
    }
    printf("\"%s\" in %s wrapped into %d lines\n", pString->m_sString,
           AsTexture(pString->m_pFont)->GetName(),
           pString->m_lstLineLength.m_nCount);
    for (i = 0; i < pString->m_lstLineLength.m_nCount; ++i)
    {
        printf("%3d %4d %5d %.*s\n", i, lstLineStart[i],
               pString->m_lstLineWidth[i], pString->m_lstLineLength[i],
               pString->m_sString + lstLineStart[i]);
    }
}

float GetViewportAlpha();

void AurGUIColor(unsigned int nColor);
// PC 0x0045A850  Xbox 0x002C9E20
void CAurGUIStringInternal::Draw(float fAlpha)
{
    int nStringLength = strlen(m_sString);
    if (nStringLength == 0 || noguirender != 0)
        return;
#ifdef _XBOX
    GLRender::LoadViewIdentity();
#endif
    GLRender_Disable(ENABLE_LIGHTING);
    GLRender::cm_bEnablePausePlane = 0;
    if (m_nPositionX == 0 && m_nWidth == 0)
        return;

    CAurFontInfo* pInfo = AsTexture(m_pFont)->GetFontInfo();
    float fTextureWidth = pInfo->m_fTextureWidth;
    float fScaleX = fTextureWidth * m_fScale * 100.0f /
        CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport][VIEWPORT_WIDTH];
    float fScaleY = fTextureWidth * m_fScale * 100.0f /
        CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport][VIEWPORT_HEIGHT];
    float fTop = (float)m_nPositionY / (float)CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport][VIEWPORT_HEIGHT];
    unsigned char* pChar = (unsigned char*)m_sString;
    int nLine = 0;
    unsigned char nAlignment = m_nAlignmentBits & ALIGNMENT_VERTICAL;
    float fLineHeight;
    if (nAlignment == ALIGNMENT_VCENTER)
    {
        fLineHeight = pInfo->m_fHeight * m_fScale * 100.0f;
        float fOffset = (m_nHeight - m_lstLineLength.m_nCount * fLineHeight) /
            ((float)CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport][VIEWPORT_HEIGHT] + (float)CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport][VIEWPORT_HEIGHT]);
        while (fOffset < 0.0f)
        {
            fOffset += fLineHeight / (float)CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport][VIEWPORT_HEIGHT];
            int nLength = m_lstLineLength[nLine];
            if (nLength < 0)
                pChar -= nLength;
            else
                pChar += nLength;
            if (*pChar == '\n' || *pChar == ' ')
                ++pChar;
            ++nLine;
        }
        fTop += fOffset;
    }
    else if (nAlignment == ALIGNMENT_BOTTOM)
    {
        fLineHeight = pInfo->m_fHeight * m_fScale * 100.0f;
        float fTextHeight = m_lstLineLength.m_nCount * fLineHeight;
        while (fTextHeight > m_nHeight)
        {
            fTextHeight -= fLineHeight;
            int nLength = m_lstLineLength[nLine];
            if (nLength < 0)
                pChar -= nLength;
            else
                pChar += nLength;
            if (*pChar == '\n' || *pChar == ' ')
                ++pChar;
            ++nLine;
        }
        fTop += (m_nHeight - fTextHeight) / (float)CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport][VIEWPORT_HEIGHT];
    }

    AsTexture(m_pFont)->Bind();
    GLRender_BeginImmediatePrimitive(Aurora::PRIMITIVE_QUADS);
    for (; nLine < m_lstLineLength.m_nCount; ++nLine)
    {
        unsigned char nChar;
        while (m_lstLineLength[nLine] == 0)
        {
            nChar = *pChar;
            fTop += (pInfo->m_fLineSpacing * m_fScale * 100.0f /
                CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport][VIEWPORT_HEIGHT] +
                pInfo->m_lstUpperLeft[nChar].m_fY -
                pInfo->m_lstLowerRight[nChar].m_fY) * fScaleY;
            ++pChar;
            if (++nLine == m_lstLineLength.m_nCount)
                break;
        }
        if (nLine == m_lstLineLength.m_nCount)
            break;

        float fSpacing = pInfo->m_fLineSpacing * m_fScale * 100.0f /
            CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport][VIEWPORT_HEIGHT];
        nChar = *pChar;
        if ((fSpacing + pInfo->m_lstUpperLeft[nChar].m_fY -
             pInfo->m_lstLowerRight[nChar].m_fY) * fScaleY + fTop < 0.0f)
        {
            pChar += m_lstLineLength[nLine];
            if (*pChar == '\n' || *pChar == ' ')
                ++pChar;
            nChar = *pChar;
            fTop += (fSpacing + pInfo->m_lstUpperLeft[nChar].m_fY -
                     pInfo->m_lstLowerRight[nChar].m_fY) * fScaleY;
            continue;
        }

        int nLength = m_lstLineLength[nLine];
        char bHyphen;
        if (nLength < 0)
        {
            bHyphen = 1;
            nLength = -nLength;
        }
        else
        {
            bHyphen = 0;
        }
        float fLeft = (float)m_nPositionX /
            CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport][VIEWPORT_WIDTH];
        if (m_lstLineWidth.m_nCount == 0)
        {
            int nWidth = 0;
            char* pText = m_sString;
            while (*pText)
            {
                nWidth -= (int)(((pInfo->m_lstLowerRight[nChar].m_fX -
                    pInfo->m_lstUpperLeft[nChar].m_fX) * pInfo->m_fTextureWidth +
                    pInfo->m_fSpacingR) * m_fScale * -100.0f);
                ++pText;
            }
            m_lstLineWidth.Add((short)(nWidth -
                (int)(pInfo->m_fSpacingR * m_fScale * 100.0f)));
        }
        nAlignment = m_nAlignmentBits & ALIGNMENT_HORIZONTAL;
        if (nAlignment == ALIGNMENT_HCENTER)
        {
            float fWidth = CAurGUIDraw::cm_nViewportStack[
                CAurGUIDraw::cm_nCurrentViewport][VIEWPORT_WIDTH];
            fLeft += (float)(m_nWidth - m_lstLineWidth[nLine]) / (fWidth + fWidth);
        }
        else if (nAlignment == ALIGNMENT_RIGHT)
        {
            fLeft += (float)(m_nWidth - m_lstLineWidth[nLine]) /
                CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport][VIEWPORT_WIDTH];
        }
        if ((float)(m_nPositionY + m_nHeight) /
            CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport][VIEWPORT_HEIGHT] < fTop)
        {
            nLine = m_lstLineLength.m_nCount;
            continue;
        }

        int nRed, nGreen, nBlue, nAlpha;
        roundToInt(nRed, m_vColor.m_fX * 255.0f);
        roundToInt(nGreen, m_vColor.m_fY * 255.0f);
        roundToInt(nBlue, m_vColor.m_fZ * 255.0f);
        roundToInt(nAlpha, GetViewportAlpha() * fAlpha * 255.0f);
#ifdef _XBOX
        unsigned int nColor = nBlue | ((nGreen | ((nRed | (nAlpha << 8)) << 8)) << 8);
#else
        unsigned int nColor = nRed | ((nGreen | ((nBlue | (nAlpha << 8)) << 8)) << 8);
#endif
        unsigned int nWhite = (nAlpha << 24) | 0x00ffffff;
        float fRight;
        float fTopVertex;
        float fBottomVertex;
        for (int i = nLength; i > 0; --i)
        {
            fRight = (pInfo->m_lstLowerRight[*pChar].m_fX -
                pInfo->m_lstUpperLeft[*pChar].m_fX) * fScaleX + fLeft;
            fTopVertex = 1.0f - fTop;
            fBottomVertex = 1.0f - (pInfo->m_fHeight * m_fScale * 100.0f /
                CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport][VIEWPORT_HEIGHT] + fTop);
            if (*pChar < ' ')
                AurGUIColor(nWhite);
            else
                AurGUIColor(nColor);
            GLRender::SetVertexData2f(Aurora::STREAM_TEXCOORD, pInfo->m_lstUpperLeft[*pChar].m_fX - 0.0001f,
                         pInfo->m_lstUpperLeft[*pChar].m_fY - 0.0001f);
            SetVertexData4f(Aurora::STREAM_POSITION, fLeft, fTopVertex, 0.0f, 1.0f);
            GLRender::SetVertexData2f(Aurora::STREAM_TEXCOORD, pInfo->m_lstUpperLeft[*pChar].m_fX - 0.0001f,
                         pInfo->m_lstLowerRight[*pChar].m_fY - 0.0001f);
            SetVertexData4f(Aurora::STREAM_POSITION, fLeft, fBottomVertex, 0.0f, 1.0f);
            GLRender::SetVertexData2f(Aurora::STREAM_TEXCOORD, pInfo->m_lstLowerRight[*pChar].m_fX - 0.0001f,
                         pInfo->m_lstLowerRight[*pChar].m_fY - 0.0001f);
            SetVertexData4f(Aurora::STREAM_POSITION, fRight, fBottomVertex, 0.0f, 1.0f);
            GLRender::SetVertexData2f(Aurora::STREAM_TEXCOORD, pInfo->m_lstLowerRight[*pChar].m_fX - 0.0001f,
                         pInfo->m_lstUpperLeft[*pChar].m_fY - 0.0001f);
            SetVertexData4f(Aurora::STREAM_POSITION, fRight, fTopVertex, 0.0f, 1.0f);
            fLeft += (pInfo->m_fSpacingR * m_fScale * 100.0f /
                CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport][VIEWPORT_WIDTH] +
                pInfo->m_lstLowerRight[*pChar].m_fX - pInfo->m_lstUpperLeft[*pChar].m_fX) * fScaleX;
            ++pChar;
        }
        if (*pChar == '\n' || *pChar == ' ')
            ++pChar;
        if (bHyphen)
        {
            fRight = ((pInfo->m_lstLowerRight['-'].m_fX -
                pInfo->m_lstUpperLeft['-'].m_fX) * fScaleX + fLeft);
            fTopVertex = 1.0f - fTop;
            fBottomVertex = 1.0f - (pInfo->m_fHeight * m_fScale * 100.0f /
                CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport][VIEWPORT_HEIGHT] + fTop);
            AurGUIColor(nColor);
            GLRender::SetVertexData2f(Aurora::STREAM_TEXCOORD, pInfo->m_lstUpperLeft['-'].m_fX - 0.0001f,
                         pInfo->m_lstUpperLeft['-'].m_fY - 0.0001f);
            SetVertexData4f(Aurora::STREAM_POSITION, fLeft, fTopVertex, 0.0f, 1.0f);
            GLRender::SetVertexData2f(Aurora::STREAM_TEXCOORD, pInfo->m_lstUpperLeft['-'].m_fX - 0.0001f,
                         pInfo->m_lstLowerRight['-'].m_fY - 0.0001f);
            SetVertexData4f(Aurora::STREAM_POSITION, fLeft, fBottomVertex, 0.0f, 1.0f);
            GLRender::SetVertexData2f(Aurora::STREAM_TEXCOORD, pInfo->m_lstLowerRight['-'].m_fX - 0.0001f,
                         pInfo->m_lstLowerRight['-'].m_fY - 0.0001f);
            SetVertexData4f(Aurora::STREAM_POSITION, fRight, fBottomVertex, 0.0f, 1.0f);
            GLRender::SetVertexData2f(Aurora::STREAM_TEXCOORD, pInfo->m_lstLowerRight['-'].m_fX - 0.0001f,
                         pInfo->m_lstUpperLeft['-'].m_fY - 0.0001f);
            SetVertexData4f(Aurora::STREAM_POSITION, fRight, fTopVertex, 0.0f, 1.0f);
        }
        fTop += (pInfo->m_fLineSpacing * m_fScale * 100.0f /
            CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport][VIEWPORT_HEIGHT] +
            pInfo->m_lstUpperLeft[*pChar].m_fY - pInfo->m_lstLowerRight[*pChar].m_fY) * fScaleY;
        if (fTop > 1.0f)
            break;
    }
    GLRender_EndImmediatePrimitive();
    GLRender_Enable(ENABLE_LIGHTING);
}
float GetViewportAlpha();
void AurGUIColor(unsigned int nColor);
// PC 0x0045B7A0  Xbox 0x002CAA50
int CAurGUIStringInternal::GetIdealPixelHeight(int nWidth)
{
    int nHeight;

    if (nWidth == -1)
    {
        if (m_lstLineLength.m_nCount != 0)
        {
            CAurFontInfo* pInfo = AsTexture(m_pFont)->GetFontInfo();

            float fLineHeight =
                (pInfo->m_fLineSpacing + pInfo->m_fHeight) * m_fScale * 100.0f;

            return (int)(fLineHeight * m_lstLineLength.m_nCount + 0.5f);
        }

        List<int> lstSavedWidth(m_lstLineWidth);
        List<int> lstSavedLength(m_lstLineLength);

        WrapStrings(-1);
        short nLines = m_lstLineLength.m_nCount;
        m_lstLineLength = lstSavedLength;
        m_lstLineWidth = lstSavedWidth;

        CAurFontInfo* pInfo = AsTexture(m_pFont)->GetFontInfo();

        float fLineHeight = (pInfo->m_fLineSpacing + pInfo->m_fHeight) * m_fScale * 100.0f;
        nHeight = (int)(fLineHeight * nLines + 0.5f);
    }
    else
    {
        List<int> lstSavedWidth(m_lstLineWidth);
        List<int> lstSavedLength(m_lstLineLength);

        WrapStrings(nWidth);
        short nLines = m_lstLineLength.m_nCount;
        m_lstLineLength = lstSavedLength;
        m_lstLineWidth = lstSavedWidth;

        CAurFontInfo* pInfo = AsTexture(m_pFont)->GetFontInfo();

        int nLineHeight = (int)((int)((pInfo->m_fLineSpacing + pInfo->m_fHeight) * 100.0f + 0.5f) * m_fScale + 0.5f);
        nHeight = nLines * nLineHeight;
    }

    return nHeight;
}
// PC 0x0045A170  Xbox 0x002CA950
void CAurGUIImage::FlushBuffer(float fAlpha)
{
    if (cm_nGUIBufferSizeUsed > 0)
    {
#ifdef _XBOX
        GLRender::LoadViewIdentity();
#endif
        GLRender_Disable(ENABLE_LIGHTING);
        GLRender::cm_bEnablePausePlane = 0;
        AurTextureMakeCurrent();
        if (cm_pMaterial != 0)
        {
            cm_pMaterial->BindTexture0();
            cm_pMaterial->SetGUIBlendingMode();
        }
        if (cm_bIsVTC == 0)
        {
            float fViewportAlpha = GetViewportAlpha();
            GLfloat fFinalAlpha = fViewportAlpha * fAlpha;
            GLRender_SetColor4f(1.0f, 1.0f, 1.0f, fFinalAlpha);
            GLRender::RenderObjectsVTQuadUP((RenderVertexVT*)cm_fGUIBuffer,
                                            cm_nGUIBufferSizeUsed / 5);
        }
        else
        {
            GLRender::RenderObjectsVTCQuadUP((RenderVertexVTC*)cm_fGUIBuffer,
                                             cm_nGUIBufferSizeUsed / 6);
        }
        GLRender_Enable(ENABLE_LIGHTING);
    }
    cm_nGUIBufferSizeUsed = 0;
}
// PC 0x0045A260  Xbox 0x002C9A90
CAurGUIImageInternal* NewCAurGUIImage(char* sTexture)
{
    if (sTexture == 0)
    {
        return new CAurGUIImageInternal;
    }
    else
    {
        return new CAurGUIImageInternal(sTexture);
    }
}
// PC 0x00458EF0  Xbox 0x002C8450
float GetViewportAlpha()
{
    float fAlpha = 1.0f;
    for (int i = 0; i <= CAurGUIDraw::cm_nCurrentViewport; i++)
        fAlpha *= CAurGUIDraw::cm_fViewportAlpha[i];
    return fAlpha;
}
// PC 0x00458F10  Xbox 0x002C84D0
void CAurGUIStringInternal::SetStringSize(short nSize)
{
    if (nSize > (short)strlen(m_sString))
    {
        char* sNew = new char[nSize + 1];
        strcpy(sNew, m_sString);
        delete[] m_sString;
        m_sString = sNew;
    }
    else
    {
        m_sString[nSize] = 0;
    }
}
// PC 0x00458F70  Xbox 0x002C8530
void CAurGUIStringInternal::SetString(char* sString)
{
    int nLength = strlen(sString);

    if (nLength > strlen(m_sString))
    {
        delete[] m_sString;
        m_sString = new char[strlen(sString) + 1];
    }

    strcpy(m_sString, sString);
}
// PC 0x00458FE0  Xbox 0x002C85A0
void CAurGUIStringInternal::GetPositionBoundingRect(float& fX, float& fY)
{
    fX = (float)m_nPositionX /
         CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport]
                                       [VIEWPORT_WIDTH];
    fY = (float)m_nPositionY /
         CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport]
                                       [VIEWPORT_HEIGHT];
}
// PC 0x00459030  Xbox 0x002C8600
void CAurGUIStringInternal::SetPositionBoundingRect(float fX, float fY)
{
    m_nPositionX =
        (int)(CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport]
                                            [VIEWPORT_WIDTH] *
              fX);
    m_nPositionY =
        (int)(CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport]
                                            [VIEWPORT_HEIGHT] *
              fY);
}
// PC 0x00459090  Xbox 0x002C8650
void CAurGUIStringInternal::GetSizeBoundingRect(float& fWidth, float& fHeight)
{
    fWidth = (float)m_nWidth /
             CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport]
                                           [VIEWPORT_WIDTH];
    fHeight = (float)m_nHeight /
              CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport]
                                            [VIEWPORT_HEIGHT];
}
// PC 0x004590E0  Xbox 0x002C86B0
void CAurGUIStringInternal::SetSizeBoundingRect(float fWidth, float fHeight)
{
    m_nWidth =
        (int)(CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport]
                                            [VIEWPORT_WIDTH] *
              fWidth);
    m_nHeight =
        (int)(CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport]
                                            [VIEWPORT_HEIGHT] *
              fHeight);
}

#ifdef _XBOX
CAurGUIImage::CAurGUIImage()
{
}
// PC 0x00459140  Xbox 0x002C8750
inline CAurGUIImage::~CAurGUIImage()
{
}
// PC 0x00459140  Xbox 0x002C8720
inline CAurGUIString::~CAurGUIString()
{
}
#else
inline CAurGUIImage::~CAurGUIImage()  // PC 0x00459140  Xbox 0x002C8750
{
}
// PC 0x00459140  Xbox 0x002C8720
inline CAurGUIString::~CAurGUIString()
{
}
#endif

#ifdef _XBOX
// PC 0x00459170  Xbox 0x002C8780
void AurGUIViewport(int nX, int nY, int nWidth, int nHeight)
{
    D3DVIEWPORT8 viewport;
    viewport.X = nX;
    viewport.Y = nY;
    viewport.Width = nWidth;
    viewport.Height = nHeight;
    viewport.MinZ = 0.0f;
    viewport.MaxZ = 1.0f;
    g_pD3DDevice->SetViewport(&viewport);
    GLRender::SetMatrixMode(1);
    GLRender::LoadMatrixIdentity();
}
#else
inline void GLRender::Scale(float fX, float fY, float fZ)  // PC 0x00459150  Xbox 0x00284A90
{
    glScalef(fX, fY, fZ);
}
// PC 0x00459170  Xbox 0x002C8780
void AurGUIViewport(int nX, int nY, int nWidth, int nHeight)
{
    glViewport(nX, nY, nWidth, nHeight);
}
// PC 0x00459190
void AurGUIColor(unsigned int nColor)
{
    unsigned int nGUIColor = nColor;
    glColor4ubv((GLubyte*)&nGUIColor);
}
#endif
// PC 0x004591B0  Xbox 0x002C87E0
void AurGUIStartLayer()
{
    GLint aViewport[4];

    GLRender_GetViewPort(aViewport);
    CAurGUIDraw::cm_nViewportStack[0][VIEWPORT_X] = aViewport[0];
    CAurGUIDraw::cm_nViewportStack[0][VIEWPORT_Y] = aViewport[1];
    CAurGUIDraw::cm_nViewportStack[0][VIEWPORT_WIDTH] = aViewport[2];
    CAurGUIDraw::cm_nViewportStack[0][VIEWPORT_HEIGHT] = aViewport[3];
    CAurGUIDraw::cm_nViewportStack[0][VIEWPORT_SCISSOR] = 0;
    CAurGUIDraw::cm_fViewportAlpha[0] = 1.0f;
    CAurGUIDraw::cm_nCurrentViewport = 0;
    GLRender_Disable(ENABLE_LIGHTING);
    GLRender_Disable(ENABLE_DEPTH_TEST);
    GLRender_SetMatrixMode(0);
    GLRender_PushCurrentMatrix();
    GLRender_LoadMatrixIdentity();
    GLRender_SetOrthoTransform(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
    GLRender_SetMatrixMode(2);
    GLRender_LoadMatrixIdentity();
    GLRender_Enable(ENABLE_BLEND);
    GLRender_SetBlendFunc(Aurora::BLEND_SRC_ALPHA, Aurora::BLEND_ONE_MINUS_SRC_ALPHA);
}
// PC 0x004592B0  Xbox 0x002C88E0
void AurGUIStopLayer()
{
    CAurGUIDraw::cm_nCurrentViewport = -1;
    GLRender_SetMatrixMode(0);
    GLRender_PopCurrentMatrix();
    GLRender_SetMatrixMode(2);
    GLRender_Enable(ENABLE_LIGHTING);
    GLRender_Enable(ENABLE_DEPTH_TEST);
}
// PC 0x004592F0  Xbox 0x002C8920
int AurGUISetupViewport(int nX, int nY, int nWidth, int nHeight,
                        const Vector& vColor, bool bScissor, float fAlpha)
{
    if (noguirender != 0)
    {
        return 0;
    }

    if (nX + nWidth < 0 || nY + nHeight < 0 || nWidth <= 0 || nHeight <= 0 ||
        nX >= g_nScreenWidth || nY >= g_nScreenHeight)
    {
        return 0;
    }

    short nViewportX = nX + CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport][VIEWPORT_X];
    short nViewportY = nY + CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport][VIEWPORT_Y];
    CAurGUIDraw::cm_nCurrentViewport++;
    CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport][VIEWPORT_X] = nViewportX;
    CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport][VIEWPORT_Y] = nViewportY;
    CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport][VIEWPORT_WIDTH] = nWidth;
    CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport][VIEWPORT_HEIGHT] = nHeight;
    CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport][VIEWPORT_SCISSOR] = bScissor;
    CAurGUIDraw::cm_fViewportAlpha[CAurGUIDraw::cm_nCurrentViewport] = fAlpha;
#ifdef _XBOX

    if (nViewportX < 0)
    {
        nWidth += nViewportX;
        nViewportX = 0;
    }
    else if (nViewportX + nWidth >= g_nScreenWidth)
    {
        nWidth = g_nScreenWidth - nViewportX;
    }
    if (nViewportY < 0)
    {
        nHeight += nViewportY;
        nViewportY = 0;
    }
    else if (nViewportY + nHeight >= g_nScreenHeight)
    {
        nHeight = g_nScreenHeight - nViewportY;
    }
    AurGUIViewport(nViewportX, nViewportY, nWidth, nHeight);
#else
    AurGUIViewport(nViewportX,
                   CAurGUIDraw::cm_nViewportStack[0][VIEWPORT_HEIGHT] -
                       nViewportY - nHeight,
                   nWidth, nHeight);
#endif

    int nRed;
    int nGreen;
    int nBlue;
    int nAlpha;
    roundToInt(nRed, vColor.m_fX * 255.0f);
    roundToInt(nGreen, vColor.m_fY * 255.0f);
    roundToInt(nBlue, vColor.m_fZ * 255.0f);
    roundToInt(nAlpha, fAlpha * 255.0f);
#ifdef _XBOX
    int nColor = nBlue | ((nGreen | ((nRed | (nAlpha << 8)) << 8)) << 8);
#else
    int nColor = nRed | ((nGreen | ((nBlue | (nAlpha << 8)) << 8)) << 8);
#endif

    if (vColor != Vector(-1.0f, -1.0f, -1.0f))
    {
        GLRender_Disable(ENABLE_TEXTURE_2D);
        GLRender_Disable(ENABLE_LIGHTING);
        GLRender_BeginImmediatePrimitive(Aurora::PRIMITIVE_QUADS);
        AurGUIColor(nColor);
        SetVertexData4f(Aurora::STREAM_POSITION, 0.0f, 0.0f, 0.0f, 1.0f);
        SetVertexData4f(Aurora::STREAM_POSITION, 1.0f, 0.0f, 0.0f, 1.0f);
        SetVertexData4f(Aurora::STREAM_POSITION, 1.0f, 1.0f, 0.0f, 1.0f);
        SetVertexData4f(Aurora::STREAM_POSITION, 0.0f, 1.0f, 0.0f, 1.0f);
        GLRender_EndImmediatePrimitive();
        GLRender_Enable(ENABLE_TEXTURE_2D);
        GLRender_Enable(ENABLE_LIGHTING);
    }

    if (bScissor)
    {
        GLRender_Clear(2);
        GLRender_Enable(ENABLE_DEPTH_TEST);
    }

    return 1;
}
// PC 0x00459580  Xbox 0x002C8C10
void AurGUICloseViewport()
{
    if (noguirender == 0)
    {
        int nIndex = --CAurGUIDraw::cm_nCurrentViewport;

#ifdef _XBOX

        D3DVIEWPORT8 viewport;
        viewport.X = CAurGUIDraw::cm_nViewportStack[nIndex][VIEWPORT_X];
        viewport.Y = CAurGUIDraw::cm_nViewportStack[nIndex][VIEWPORT_Y];
        viewport.Width = CAurGUIDraw::cm_nViewportStack[nIndex][VIEWPORT_WIDTH];
        viewport.Height = CAurGUIDraw::cm_nViewportStack[nIndex][VIEWPORT_HEIGHT];
        viewport.MinZ = 0.0f;
        viewport.MaxZ = 1.0f;
        g_pD3DDevice->SetViewport(&viewport);
        GLRender::SetMatrixMode(1);
        GLRender::LoadMatrixIdentity();
#else
        glViewport(CAurGUIDraw::cm_nViewportStack[nIndex][VIEWPORT_X],
                   CAurGUIDraw::cm_nViewportStack[0][VIEWPORT_HEIGHT] -
                       CAurGUIDraw::cm_nViewportStack[nIndex][VIEWPORT_Y] -
                       CAurGUIDraw::cm_nViewportStack[nIndex][VIEWPORT_HEIGHT],
                   CAurGUIDraw::cm_nViewportStack[nIndex][VIEWPORT_WIDTH],
                   CAurGUIDraw::cm_nViewportStack[nIndex][VIEWPORT_HEIGHT]);
#endif
        if (CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport]
                                          [VIEWPORT_SCISSOR] != 0)
        {
            GLRender_Enable(ENABLE_DEPTH_TEST);
        }
        else
        {
            GLRender_Disable(ENABLE_DEPTH_TEST);
        }
    }
}
// PC 0x00459610  Xbox 0x002C8CC0
int CAurGUIStringInternal::GetFontPixelHeight()
{
    return (int)(AsTexture(m_pFont)->GetFontInfo()->m_fHeight * 100.0f + 0.5f);
}
// PC 0x00459630  Xbox 0x002C8CF0
void CAurGUIStringInternal::DrawBoundingRect(Vector vColor, float fAlpha)
{
    if (noguirender == 0)
    {
        GLRender_Disable(ENABLE_LIGHTING);
        GLRender_Disable(ENABLE_TEXTURE_2D);
        GLRender_Disable(ENABLE_BLEND);

        int nBottom = m_nPositionY + m_nHeight;
        int nRight = m_nPositionX + m_nWidth;
        GLfloat fTop =
            1.0f -
            (float)m_nPositionY /
                CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport]
                                              [VIEWPORT_HEIGHT];
        GLfloat fBottom =
            1.0f -
            (float)nBottom /
                CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport]
                                              [VIEWPORT_HEIGHT];
        GLfloat fLeft =
            (float)m_nPositionX /
            CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport]
                                          [VIEWPORT_WIDTH];
        GLfloat fRight =
            (float)nRight /
            CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport]
                                          [VIEWPORT_WIDTH];

        GLRender_BeginImmediatePrimitive(Aurora::PRIMITIVE_LINE_LOOP);
        {
#ifdef _XBOX
            unsigned int nColor = 0xFF66A5E5;
#else
            unsigned int nColor = 0xFFE5A566;
#endif
#ifdef _XBOX
            AurGUIColor(nColor);
#else
            glColor4ubv((GLubyte*)&nColor);
#endif
        }
        SetVertexData4f(Aurora::STREAM_POSITION, fLeft, fTop, 0.0f, 1.0f);
        SetVertexData4f(Aurora::STREAM_POSITION, fLeft, fBottom, 0.0f, 1.0f);
        SetVertexData4f(Aurora::STREAM_POSITION, fRight, fBottom, 0.0f, 1.0f);
        SetVertexData4f(Aurora::STREAM_POSITION, fRight, fTop, 0.0f, 1.0f);
        SetVertexData4f(Aurora::STREAM_POSITION, fLeft, fBottom, 0.0f, 1.0f);
        GLRender_EndImmediatePrimitive();
        GLRender_Enable(ENABLE_LIGHTING);
        GLRender_Enable(ENABLE_BLEND);
        GLRender_Enable(ENABLE_TEXTURE_2D);
    }
}
// PC 0x00459790  Xbox 0x002C8E50
CAurGUIImageInternal::CAurGUIImageInternal()
{
    m_pMaterial = new Material;
}
// PC 0x0045B200  Xbox 0x002CAC00
CAurGUIImageInternal::~CAurGUIImageInternal()
{
    AurguiTrace2();
    if (CAurGUIImage::cm_nGUITID == m_pMaterial->GetTextureTID())
    {
        CAurGUIImage::FlushBuffer(CAurGUIImage::cm_fAlpha);
        CAurGUIImage::cm_nGUITID = 0;
        CAurGUIImage::cm_pMaterial = 0;
    }

    delete m_pMaterial;
}
// PC 0x0045B290  Xbox 0x002CACA0
void CAurGUIImageInternal::DrawBuffered(float fX, float fY, float fWidth,
                                        float fHeight, unsigned char nFlags,
                                        float fRotation, const Vector& vColor,
                                        float fAlpha)
{
    AurguiTrace2();
    if (noguirender != 0)
        return;

    char bColored;
    if (vColor.m_fX != -1.0f ||
        vColor.m_fY != -1.0f ||
        vColor.m_fZ != -1.0f)
        bColored = 1;
    else
        bColored = 0;

    int nTextureTID = m_pMaterial->GetTextureTID();
    if (nTextureTID != cm_nGUITID || bColored != cm_bIsVTC ||
        fAlpha != cm_fAlpha ||
        (bColored && cm_nGUIBufferSizeUsed + 24 >= 2048) ||
        (!bColored && cm_nGUIBufferSizeUsed + 20 >= 2048))
    {
        FlushBuffer(cm_fAlpha);
        cm_nGUITID = nTextureTID;
        cm_pMaterial = m_pMaterial;
        cm_bIsVTC = bColored;
        cm_fAlpha = fAlpha;
    }

#ifdef _XBOX
    unsigned char nCorner0, nCorner1, nCorner2, nCorner3;
    unsigned char nRotated[4];
    switch (nFlags & GUI_IMAGE_MAPPING_MASK)
    {
    case GUI_IMAGE_MAPPING_MIRRORED:
        nCorner0 = 1; nCorner1 = 0; nCorner2 = 3; nCorner3 = 2;
        break;
    case GUI_IMAGE_MAPPING_REVERSED:
        nCorner0 = 3; nCorner1 = 2; nCorner2 = 1; nCorner3 = 0;
        break;
    case GUI_IMAGE_MAPPING_NORMAL:
        nCorner0 = 0; nCorner1 = 1; nCorner2 = 2; nCorner3 = 3;
        break;
    }

    if (fRotation < 0.0f)
        fRotation += 360.0f;
    switch ((int)((fRotation + 45.0f) / 90.0f))
    {
    case 0:
        nRotated[0] = nCorner0; nRotated[1] = nCorner1;
        nRotated[2] = nCorner2; nRotated[3] = nCorner3;
        break;
    case 1:
        nRotated[nCorner0] = 1; nRotated[nCorner1] = 2;
        nRotated[nCorner2] = 3; nRotated[nCorner3] = 0;
        break;
    case 2:
        nRotated[nCorner0] = 2; nRotated[nCorner1] = 3;
        nRotated[nCorner2] = 0; nRotated[nCorner3] = 1;
        break;
    case 3:
        nRotated[nCorner0] = 3; nRotated[nCorner1] = 0;
        nRotated[nCorner2] = 1; nRotated[nCorner3] = 2;
        break;
    }
#else
    unsigned char nCorner[4];
    unsigned char nRotated[4];
    switch (nFlags & GUI_IMAGE_MAPPING_MASK)
    {
    case GUI_IMAGE_MAPPING_MIRRORED:
        nCorner[0] = 1; nCorner[1] = 0; nCorner[2] = 3; nCorner[3] = 2;
        break;
    case GUI_IMAGE_MAPPING_REVERSED:
        nCorner[0] = 3; nCorner[1] = 2; nCorner[2] = 1; nCorner[3] = 0;
        break;
    case GUI_IMAGE_MAPPING_NORMAL:
        nCorner[0] = 0; nCorner[1] = 1; nCorner[2] = 2; nCorner[3] = 3;
        break;
    }

    if (fRotation < 0.0f)
        fRotation += 360.0f;
    switch ((unsigned int)((fRotation + 45.0f) / 90.0f))
    {
    case 0:
        nRotated[0] = nCorner[0]; nRotated[1] = nCorner[1];
        nRotated[2] = nCorner[2]; nRotated[3] = nCorner[3];
        break;
    case 1:
        nRotated[nCorner[0]] = 1; nRotated[nCorner[1]] = 2;
        nRotated[nCorner[2]] = 3; nRotated[nCorner[3]] = 0;
        break;
    case 2:
        nRotated[nCorner[0]] = 2; nRotated[nCorner[1]] = 3;
        nRotated[nCorner[2]] = 0; nRotated[nCorner[3]] = 1;
        break;
    case 3:
        nRotated[nCorner[0]] = 3; nRotated[nCorner[1]] = 0;
        nRotated[nCorner[2]] = 1; nRotated[nCorner[3]] = 2;
        break;
    }
#endif

#ifdef _XBOX
    float fHalfU = 0.5f / AsTexture(m_pMaterial->m_pTexture0)->GetWidth();
    float fHalfV = 0.5f / AsTexture(m_pMaterial->m_pTexture0)->GetHeight();
#endif
    if (bColored == 0)
    {
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed] = fX;
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + 1] = 1.0f - fY;
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + 2] = 0.0f;
#ifdef _XBOX
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + nRotated[0] * 5 + 3] = fHalfU;
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + nRotated[0] * 5 + 4] = 1.0f - fHalfV;
#else
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + nRotated[0] * 5 + 3] = 0.0f;
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + nRotated[0] * 5 + 4] = 1.0f;
#endif
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + 5] = fX;
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + 6] = 1.0f - (fY + fHeight);
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + 7] = 0.0f;
#ifdef _XBOX
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + nRotated[1] * 5 + 3] = fHalfU;
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + nRotated[1] * 5 + 4] = 0.0f - fHalfV;
#else
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + nRotated[1] * 5 + 3] = 0.0f;
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + nRotated[1] * 5 + 4] = 0.0f;
#endif
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + 10] = fX + fWidth;
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + 11] = 1.0f - (fY + fHeight);
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + 12] = 0.0f;
#ifdef _XBOX
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + nRotated[2] * 5 + 3] = fHalfU + 1.0f;
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + nRotated[2] * 5 + 4] = 0.0f - fHalfV;
#else
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + nRotated[2] * 5 + 3] = 1.0f;
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + nRotated[2] * 5 + 4] = 0.0f;
#endif
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + 15] = fX + fWidth;
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + 16] = 1.0f - fY;
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + 17] = 0.0f;
#ifdef _XBOX
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + nRotated[3] * 5 + 3] = fHalfU + 1.0f;
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + nRotated[3] * 5 + 4] = 1.0f - fHalfV;
#else
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + nRotated[3] * 5 + 3] = 1.0f;
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + nRotated[3] * 5 + 4] = 1.0f;
#endif
        cm_nGUIBufferSizeUsed = cm_nGUIBufferSizeUsed + 20;
    }
    else
    {
        int nRed;
        int nGreen;
        int nBlue;
        int nAlpha;
        roundToInt(nRed, vColor.m_fX * 255.0f);
        roundToInt(nGreen, vColor.m_fY * 255.0f);
        roundToInt(nBlue, vColor.m_fZ * 255.0f);
#ifdef _XBOX
        roundToInt(nAlpha, GetViewportAlpha() * fAlpha * 255.0f);
#else
        float fViewportAlpha = 1.0f;
        for (int i = 0; i <= CAurGUIDraw::cm_nCurrentViewport; i++)
            fViewportAlpha *= CAurGUIDraw::cm_fViewportAlpha[i];
        roundToInt(nAlpha, fViewportAlpha * fAlpha * 255.0f);
#endif
#ifdef _XBOX
        int nColor = nBlue | ((nGreen | ((nRed | (nAlpha << 8)) << 8)) << 8);
#else
        int nColor = nRed | ((nGreen | ((nBlue | (nAlpha << 8)) << 8)) << 8);
#endif
        union { int nColor; float fColor; } color;
        color.nColor = nColor;
        float fColor = color.fColor;
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed] = fX;
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + 1] = 1.0f - fY;
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + 2] = 0.0f;
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + 3] = fColor;
#ifdef _XBOX
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + nRotated[0] * 6 + 4] = fHalfU;
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + nRotated[0] * 6 + 5] = 1.0f - fHalfV;
#else
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + nRotated[0] * 6 + 4] = 0.0f;
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + nRotated[0] * 6 + 5] = 1.0f;
#endif
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + 6] = fX;
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + 7] = 1.0f - (fY + fHeight);
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + 8] = 0.0f;
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + 9] = fColor;
#ifdef _XBOX
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + nRotated[1] * 6 + 4] = fHalfU;
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + nRotated[1] * 6 + 5] = fHalfV;
#else
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + nRotated[1] * 6 + 4] = 0.0f;
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + nRotated[1] * 6 + 5] = 0.0f;
#endif
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + 12] = fX + fWidth;
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + 13] = 1.0f - (fY + fHeight);
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + 14] = 0.0f;
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + 15] = fColor;
#ifdef _XBOX
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + nRotated[2] * 6 + 4] = 1.0f - fHalfU;
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + nRotated[2] * 6 + 5] = fHalfV;
#else
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + nRotated[2] * 6 + 4] = 1.0f;
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + nRotated[2] * 6 + 5] = 0.0f;
#endif
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + 18] = fX + fWidth;
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + 19] = 1.0f - fY;
#ifdef _XBOX
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + 20] = fHalfV;
#else
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + 20] = 0.0f;
#endif
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + 21] = fColor;
#ifdef _XBOX
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + nRotated[3] * 6 + 4] = 1.0f - fHalfU;
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + nRotated[3] * 6 + 5] = 1.0f - fHalfV;
#else
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + nRotated[3] * 6 + 4] = 1.0f;
        cm_fGUIBuffer[cm_nGUIBufferSizeUsed + nRotated[3] * 6 + 5] = 1.0f;
#endif
        cm_nGUIBufferSizeUsed = cm_nGUIBufferSizeUsed + 24;
    }
}
// PC 0x00459800  Xbox 0x002C8ED0
void CAurGUIImageInternal::DrawBuffered(int nX, int nY, int nWidth, int nHeight,
                                        unsigned char nFlags, float fRotation,
                                        const Vector& vColor, float fAlpha)
{
    if (noguirender == 0)
    {
        DrawBuffered(
            (float)nX /
                CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport]
                                              [VIEWPORT_WIDTH],
            (float)nY /
                CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport]
                                              [VIEWPORT_HEIGHT],
            (float)nWidth /
                CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport]
                                              [VIEWPORT_WIDTH],
            (float)nHeight /
                CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport]
                                              [VIEWPORT_HEIGHT],
            nFlags, fRotation, vColor, fAlpha);
    }
}
// PC 0x00459880  Xbox 0x002C8F60
inline void CAurGUIImageInternal::SetTexture(char* sTexture)
{
    m_pMaterial->InitializeTextures(sTexture, 0, 0, 0);
}
// PC 0x004598A0  Xbox 0x002C9060
CAurGUIImageInternal::CAurGUIImageInternal(char* sTexture)
{
    m_pMaterial = new Material;
    SetTexture(sTexture);
}
// PC 0x00459920  Xbox 0x002CA860
void CAurGUIImageInternal::Draw(int nX, int nY, int nWidth, int nHeight,
                                unsigned char nFlags, float fRotation,
                                const Vector& vColor, float fAlpha)
{
#ifdef _XBOX

    if (noguirender == 0)
    {
        float fX = (float)nX / CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport][VIEWPORT_WIDTH];
        float fY = (float)nY / CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport][VIEWPORT_HEIGHT];
        float fWidth =
            (float)nWidth / CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport][VIEWPORT_WIDTH];
        float fHeight =
            (float)nHeight / CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport][VIEWPORT_HEIGHT];
        GLRender::LoadViewIdentity();
        GLRender::cm_bEnablePausePlane = 0;
        Draw(fX, fY, fWidth, fHeight, nFlags, fRotation, vColor, fAlpha);
    }
#else
    if (noguirender == 0)
    {
        GLRender::cm_bEnablePausePlane = 0;
        Draw(
            (float)nX /
                CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport]
                                              [VIEWPORT_WIDTH],
            (float)nY /
                CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport]
                                              [VIEWPORT_HEIGHT],
            (float)nWidth /
                CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport]
                                              [VIEWPORT_WIDTH],
            (float)nHeight /
                CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport]
                                              [VIEWPORT_HEIGHT],
            nFlags, fRotation, vColor, fAlpha);
    }
#endif
}

#ifdef _XBOX
static inline int TextureCellWidth(CAurTexture* pTexture)
{
    int nWidth = pTexture->GetWidth();
    return nWidth / pTexture->GetNumX();
}

static inline int TextureCellHeight(CAurTexture* pTexture)
{
    int nHeight = pTexture->GetHeight();
    return nHeight / pTexture->GetNumY();
}
#endif
// PC 0x004599B0  Xbox 0x002C90F0
void CAurGUIImageInternal::Draw(float fX, float fY, float fWidth,
                                float fHeight, unsigned char nFlags,
                                float fRotation, const Vector& vColor,
                                float fAlpha)
{
    if (noguirender != 0)
        return;

#ifdef _XBOX
    int nTexelsX = TextureCellWidth(AsTexture(m_pMaterial->m_pTexture0));
    int nTexelsY = TextureCellHeight(AsTexture(m_pMaterial->m_pTexture0));
#else

    float fTextureScaleX = (float)AsTexture(m_pMaterial->m_pTexture0)->GetNumX() /
        AsTexture(m_pMaterial->m_pTexture0)->GetWidth();
    float fTextureScaleY = (float)AsTexture(m_pMaterial->m_pTexture0)->GetNumY() /
        AsTexture(m_pMaterial->m_pTexture0)->GetHeight();
#endif
    AurTextureMakeCurrent();
    m_pMaterial->BindTexture0();

    unsigned char nCorner[4];
    unsigned char nRotated[4];
    switch (nFlags & GUI_IMAGE_MAPPING_MASK)
    {
    case GUI_IMAGE_MAPPING_MIRRORED:
        nCorner[0] = 1; nCorner[1] = 0; nCorner[2] = 3; nCorner[3] = 2;
        break;
    case GUI_IMAGE_MAPPING_REVERSED:
        nCorner[0] = 3; nCorner[1] = 2; nCorner[2] = 1; nCorner[3] = 0;
        break;
    case GUI_IMAGE_MAPPING_NORMAL:
        nCorner[0] = 0; nCorner[1] = 1; nCorner[2] = 2; nCorner[3] = 3;
        break;
    }
    if (fRotation < 0.0f)
        fRotation += 360.0f;

    GLRender_PushCurrentMatrix();
    if (fRotation == 0.0f)
    {
        nRotated[0] = nCorner[0]; nRotated[1] = nCorner[1];
        nRotated[2] = nCorner[2]; nRotated[3] = nCorner[3];
    }
    else if (fRotation == 90.0f)
    {
        nRotated[nCorner[0]] = 1; nRotated[nCorner[1]] = 2;
        nRotated[nCorner[2]] = 3; nRotated[nCorner[3]] = 0;
    }
    else if (fRotation == 180.0f)
    {
        nRotated[nCorner[0]] = 2; nRotated[nCorner[1]] = 3;
        nRotated[nCorner[2]] = 0; nRotated[nCorner[3]] = 1;
    }
    else if (fRotation == 270.0f)
    {
        nRotated[nCorner[0]] = 3; nRotated[nCorner[1]] = 0;
        nRotated[nCorner[2]] = 1; nRotated[nCorner[3]] = 2;
    }
    else
    {
        nRotated[0] = nCorner[0]; nRotated[1] = nCorner[1];
        nRotated[2] = nCorner[2]; nRotated[3] = nCorner[3];
        float fCenterX = -fX - fWidth * 0.5f;
        float fCenterY = fWidth * 0.5f - (1.0f - fY);
        GLRender_LoadMatrixIdentity();
        GLRender_Translate(-fCenterX, -fCenterY, 0.0f);
#ifdef _XBOX
        GLRender_Rotate(fRotation * 0.0174533f, 0.0f, 0.0f, 1.0f);
#else
        GLRender_Rotate(fRotation, 0.0f, 0.0f, 1.0f);
#endif
        float fScaleX = 1.0f;
        float fScaleY = 1.0f;
        float fRotationScale = (int)fRotation % 180 * (1.0f / 90.0f);
        fRotationScale += fRotationScale > 1.0f ? 1.0f - fRotationScale : 0.0f;
        short nWidth = CAurGUIDraw::cm_nViewportStack[
            CAurGUIDraw::cm_nCurrentViewport][VIEWPORT_WIDTH];
        short nHeight = CAurGUIDraw::cm_nViewportStack[
            CAurGUIDraw::cm_nCurrentViewport][VIEWPORT_HEIGHT];
        if (nHeight < nWidth)
        {
            fScaleY = (float)nHeight / nWidth;
            fScaleX = 1.0f / fScaleY;
        }
        else if (nHeight > nWidth)
        {
            fScaleX = (float)nWidth / nHeight;
            fScaleY = 1.0f / fScaleX;
        }
        GLRender::Scale(fRotationScale * fScaleX + (1.0f - fRotationScale),
                 fRotationScale * fScaleY + (1.0f - fRotationScale), 1.0f);
        GLRender_Translate(fCenterX, fCenterY, 0.0f);
    }

#ifdef _XBOX

    float fTexelsX = (float)nTexelsX;
    float fHalfU = 0.5f / fTexelsX;
    float fTexelsY = (float)nTexelsY;
    float fHalfV = 0.5f / fTexelsY;
    GLRender_Disable(ENABLE_LIGHTING);
    m_pMaterial->SetGUIBlendingMode();
    float afTexCoords[4][2];
    float& fU0 = afTexCoords[nRotated[0]][0];
    float& fV0 = afTexCoords[nRotated[0]][1];
    fU0 = fHalfU;
    fV0 = 1.0f - fHalfV;
    float& fU1 = afTexCoords[nRotated[1]][0];
    float& fV1 = afTexCoords[nRotated[1]][1];
    fU1 = fHalfU;
    fV1 = 0.0f - fHalfV;
    float& fU2 = afTexCoords[nRotated[2]][0];
    float& fV2 = afTexCoords[nRotated[2]][1];
    fU2 = fHalfU + 1.0f;
    fV2 = 0.0f - fHalfV;
    float& fU3 = afTexCoords[nRotated[3]][0];
    float& fV3 = afTexCoords[nRotated[3]][1];
    fU3 = fHalfU + 1.0f;
    fV3 = 1.0f - fHalfV;
    int nPixelX, nPixelWidth, nPixelY, nPixelHeight;
    roundToInt(nPixelX, CAurGUIDraw::cm_nViewportStack[
        CAurGUIDraw::cm_nCurrentViewport][VIEWPORT_WIDTH] * fX);
    roundToInt(nPixelY, CAurGUIDraw::cm_nViewportStack[
        CAurGUIDraw::cm_nCurrentViewport][VIEWPORT_HEIGHT] * fY);
    roundToInt(nPixelWidth, CAurGUIDraw::cm_nViewportStack[
        CAurGUIDraw::cm_nCurrentViewport][VIEWPORT_WIDTH] * fWidth);
    roundToInt(nPixelHeight, CAurGUIDraw::cm_nViewportStack[
        CAurGUIDraw::cm_nCurrentViewport][VIEWPORT_HEIGHT] * fHeight);
    if (nPixelX + CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport][VIEWPORT_X] < 0)
    {
        float fShift = (float)((CAurGUIDraw::cm_nViewportStack[
            CAurGUIDraw::cm_nCurrentViewport][VIEWPORT_X] + nPixelX) / nPixelWidth);
        fU0 -= fShift;
        fU1 -= fShift;
    }
    else if (CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport][VIEWPORT_X] +
             nPixelWidth + nPixelX > g_nScreenWidth)
    {
        float fShift = (float)((CAurGUIDraw::cm_nViewportStack[
            CAurGUIDraw::cm_nCurrentViewport][VIEWPORT_X] - g_nScreenWidth + nPixelWidth + nPixelX) / nPixelWidth);
        fU2 -= fShift;
        fU3 -= fShift;
    }
    if (CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport][VIEWPORT_Y] + nPixelY < 0)
    {
        float fShift = (float)((CAurGUIDraw::cm_nViewportStack[
            CAurGUIDraw::cm_nCurrentViewport][VIEWPORT_Y] + nPixelY) / nPixelY);
        fV0 -= fShift;
        fV3 -= fShift;
    }
    else if (CAurGUIDraw::cm_nViewportStack[CAurGUIDraw::cm_nCurrentViewport][VIEWPORT_Y] +
             nPixelHeight + nPixelY > g_nScreenHeight)
    {
        float fShift = (float)((CAurGUIDraw::cm_nViewportStack[
            CAurGUIDraw::cm_nCurrentViewport][VIEWPORT_Y] - g_nScreenHeight + nPixelHeight + nPixelY) / nPixelHeight);
        fV1 -= fShift;
        fV2 -= fShift;
    }
    if ((nTexelsX & (nTexelsX - 1)) != 0 || (nTexelsY & (nTexelsY - 1)) != 0)
    {
        fU0 *= fTexelsX;
        fV0 *= fTexelsY;
        fU1 *= fTexelsX;
        fV1 *= fTexelsY;
        fU2 *= fTexelsX;
        fV2 *= fTexelsY;
        fU3 *= fTexelsX;
        fV3 *= fTexelsY;
    }
    GLRender_BeginImmediatePrimitive(Aurora::PRIMITIVE_QUADS);
    unsigned int nColor;
    if (equal(vColor, Vector(-1.0f, -1.0f, -1.0f)))
    {
        GLRender_SetColor4f(1.0f, 1.0f, 1.0f, GetViewportAlpha() * fAlpha);
        nColor = ((unsigned int)(GetViewportAlpha() * fAlpha * 255.0f) << 24) | 0x00ffffff;
    }
    else
    {
        int nRed, nGreen, nBlue, nAlpha;
        roundToInt(nRed, vColor.m_fX * 255.0f);
        roundToInt(nGreen, vColor.m_fY * 255.0f);
        roundToInt(nBlue, vColor.m_fZ * 255.0f);
        roundToInt(nAlpha, GetViewportAlpha() * fAlpha * 255.0f);
        nColor = nBlue | ((nGreen | ((nRed | (nAlpha << 8)) << 8)) << 8);
    }
    AurGUIColor(nColor);
    GLRender::SetVertexData2f(Aurora::STREAM_TEXCOORD, afTexCoords[0][0], afTexCoords[0][1]);
    SetVertexData4f(Aurora::STREAM_POSITION, fX, 1.0f - fY, 0.0f, 1.0f);
    GLRender::SetVertexData2f(Aurora::STREAM_TEXCOORD, afTexCoords[1][0], afTexCoords[1][1]);
    SetVertexData4f(Aurora::STREAM_POSITION, fX, 1.0f - (fY + fHeight), 0.0f, 1.0f);
    GLRender::SetVertexData2f(Aurora::STREAM_TEXCOORD, afTexCoords[2][0], afTexCoords[2][1]);
    SetVertexData4f(Aurora::STREAM_POSITION, fX + fWidth, 1.0f - (fY + fHeight), 0.0f, 1.0f);
    GLRender::SetVertexData2f(Aurora::STREAM_TEXCOORD, afTexCoords[3][0], afTexCoords[3][1]);
    SetVertexData4f(Aurora::STREAM_POSITION, fX + fWidth, 1.0f - fY, 0.0f, 1.0f);
#else
    GLRender_Disable(ENABLE_LIGHTING);
    m_pMaterial->SetGUIBlendingMode();
    float afTexCoords[4][2];
    afTexCoords[nRotated[0]][0] = 0.0f;
    afTexCoords[nRotated[0]][1] = 1.0f;
    afTexCoords[nRotated[1]][0] = 0.0f;
    afTexCoords[nRotated[1]][1] = 0.0f;
    afTexCoords[nRotated[2]][0] = 1.0f;
    afTexCoords[nRotated[2]][1] = 0.0f;
    afTexCoords[nRotated[3]][0] = 1.0f;
    afTexCoords[nRotated[3]][1] = 1.0f;
    GLRender_BeginImmediatePrimitive(Aurora::PRIMITIVE_QUADS);
    if (equal(vColor, Vector(-1.0f, -1.0f, -1.0f)))
    {
        GLRender_SetColor4f(1.0f, 1.0f, 1.0f, GetViewportAlpha() * fAlpha);
        AurGUIColor(((unsigned int)(GetViewportAlpha() * fAlpha * 255.0f) << 24) |
                    0x00ffffff);
        GLRender::SetVertexData2f(Aurora::STREAM_TEXCOORD, afTexCoords[0][0], afTexCoords[0][1]);
        SetVertexData4f(Aurora::STREAM_POSITION, fX, 1.0f - fY, 0.0f, 1.0f);
        GLRender::SetVertexData2f(Aurora::STREAM_TEXCOORD, afTexCoords[1][0], afTexCoords[1][1]);
        SetVertexData4f(Aurora::STREAM_POSITION, fX, 1.0f - (fY + fHeight), 0.0f, 1.0f);
        GLRender::SetVertexData2f(Aurora::STREAM_TEXCOORD, afTexCoords[2][0], afTexCoords[2][1]);
        SetVertexData4f(Aurora::STREAM_POSITION, fX + fWidth, 1.0f - (fY + fHeight), 0.0f, 1.0f);
        GLRender::SetVertexData2f(Aurora::STREAM_TEXCOORD, afTexCoords[3][0], afTexCoords[3][1]);
        SetVertexData4f(Aurora::STREAM_POSITION, fX + fWidth, 1.0f - fY, 0.0f, 1.0f);
    }
    else
    {
        int nRed, nGreen, nBlue, nAlpha;
        roundToInt(nRed, vColor.m_fX * 255.0f);
        roundToInt(nGreen, vColor.m_fY * 255.0f);
        roundToInt(nBlue, vColor.m_fZ * 255.0f);
        roundToInt(nAlpha, GetViewportAlpha() * fAlpha * 255.0f);
#ifdef _XBOX
        AurGUIColor(nBlue | ((nGreen | ((nRed | (nAlpha << 8)) << 8)) << 8));
#else
        AurGUIColor(nRed | ((nGreen | ((nBlue | (nAlpha << 8)) << 8)) << 8));
#endif
        GLRender::SetVertexData2f(Aurora::STREAM_TEXCOORD, afTexCoords[0][0], afTexCoords[0][1]);
        SetVertexData4f(Aurora::STREAM_POSITION, fX, 1.0f - fY, 0.0f, 1.0f);
        GLRender::SetVertexData2f(Aurora::STREAM_TEXCOORD, afTexCoords[1][0], afTexCoords[1][1]);
        SetVertexData4f(Aurora::STREAM_POSITION, fX, 1.0f - (fY + fHeight), 0.0f, 1.0f);
        GLRender::SetVertexData2f(Aurora::STREAM_TEXCOORD, afTexCoords[2][0], afTexCoords[2][1]);
        SetVertexData4f(Aurora::STREAM_POSITION, fX + fWidth, 1.0f - (fY + fHeight), 0.0f, 1.0f);
        GLRender::SetVertexData2f(Aurora::STREAM_TEXCOORD, afTexCoords[3][0], afTexCoords[3][1]);
        SetVertexData4f(Aurora::STREAM_POSITION, fX + fWidth, 1.0f - fY, 0.0f, 1.0f);
    }
#endif
    GLRender_EndImmediatePrimitive();
    GLRender_PopCurrentMatrix();
    GLRender_Enable(ENABLE_LIGHTING);
}
// PC 0x0045A090  Xbox 0x002C8F80
int CAurGUIImageInternal::GetImageHeight()
{
    return AsTexture(m_pMaterial->m_pTexture0)->GetHeight();
}
// PC 0x0045A0A0  Xbox 0x002C8F90
int CAurGUIImageInternal::GetImageWidth()
{
    return AsTexture(m_pMaterial->m_pTexture0)->GetWidth();
}
// PC 0x0045A0B0  Xbox 0x002C8FA0
void CAurGUIImageInternal::Draw()
{
    Draw(0, 0, AsTexture(m_pMaterial->m_pTexture0)->GetWidth(),
         AsTexture(m_pMaterial->m_pTexture0)->GetHeight(), 0, 0.0f,
         Vector(-1.0f, -1.0f, -1.0f), 1.0f);
}
// PC 0x0045A110  Xbox 0x002C9000
void CAurGUIImageInternal::Draw(float fAlpha)
{
    Draw(0, 0, AsTexture(m_pMaterial->m_pTexture0)->GetWidth(),
         AsTexture(m_pMaterial->m_pTexture0)->GetHeight(), 0, 0.0f,
         Vector(-1.0f, -1.0f, -1.0f), fAlpha);
}
// PC 0x0045B990  Xbox 0x002CB240
#ifdef _XBOX
CAurGUIStringInternal::CAurGUIStringInternal(char* sString, char* sFont,
                                             Vector vColor)
#else
CAurGUIStringInternal::CAurGUIStringInternal(char* sString, char* sFont,
                                             Vector vColor, float fScale)
#endif
{
    if (sFont == 0)
    {
        m_pFont = 0;
    }
    else
    {
        m_pFont = AurTextureGetReference(sFont, 0);
    }

    m_sString = new char[strlen(sString) + 1];
    strcpy(m_sString, sString);
    m_lstLineLength.Add(strlen(sString));
    m_vColor = vColor;
    m_nPositionX = 0;
    m_nPositionY = 0;
    m_nWidth = 0;
    m_nHeight = 0;
    m_nCursorIndex = -1;
    m_nAlignmentBits = 9;
#ifndef _XBOX
    m_fScale = fScale;
#endif
}
// PC 0x0045BBE0  Xbox 0x002CB480
inline void CAurGUIStringInternal::GetPositionBoundingRect(int& nX, int& nY)
{
    nX = m_nPositionX;
    nY = m_nPositionY;
}
// PC 0x0045BC00  Xbox 0x002CB4A0
inline void CAurGUIStringInternal::SetPositionBoundingRect(int nX, int nY)
{
    m_nPositionX = nX;
    m_nPositionY = nY;
}
// PC 0x0045BC20  Xbox 0x002CB4C0
inline void CAurGUIStringInternal::GetSizeBoundingRect(int& nWidth, int& nHeight)
{
    nWidth = m_nWidth;
    nHeight = m_nHeight;
}
// PC 0x0045BC40  Xbox 0x002CB4E0
inline void CAurGUIStringInternal::SetSizeBoundingRect(int nWidth, int nHeight)
{
    m_nWidth = nWidth;
    m_nHeight = nHeight;
}
// PC 0x0045BC60  Xbox 0x002CB460
inline char* CAurGUIStringInternal::GetFontTexture()
{
    CAurTexture* pFont = AsTexture(m_pFont);

    if (pFont != 0)
    {
        return pFont->GetName();
    }
    else
    {
        return 0;
    }
}
// PC 0x0045BC70  Xbox 0x002CB500
CAurGUIStringInternal::~CAurGUIStringInternal()
{
    CAurTexture* pFont = AsTexture(m_pFont);

    m_pFont = 0;
    AurTextureRelease(pFont);
    m_pFont = 0;

    delete[] m_sString;
    m_sString = 0;
#ifdef _XBOX
    m_lstLineLength.SetSize(0);
    m_lstLineWidth.SetSize(0);
#else
    List<int>& lstLength = m_lstLineLength;
    if (lstLength.m_pList != 0)
    {
        delete[] lstLength.m_pList;
        lstLength.m_pList = 0;
    }
    lstLength.m_nAllocated = 0;
    lstLength.m_nCount = 0;
    List<int>& lstWidth = m_lstLineWidth;
    if (lstWidth.m_pList != 0)
    {
        delete[] lstWidth.m_pList;
        lstWidth.m_pList = 0;
    }
    lstWidth.m_nAllocated = 0;
    lstWidth.m_nCount = 0;
#endif
    m_nPositionX = 0;
    m_nPositionY = 0;
    m_nWidth = 0;
    m_nHeight = 0;
}
// PC 0x0045BD70  Xbox 0x002CB5D0
void CAurGUIStringInternal::SetFontTexture(char* sFont)
{
    CAurTexture* pOld = AsTexture(m_pFont);

    if (pOld != 0)
    {
        m_pFont = 0;
        AurTextureRelease(pOld);
    }

    if (sFont == 0)
    {
        m_pFont = 0;
        m_lstLineWidth.m_nCount = 0;
    }
    else
    {
        m_pFont = AurTextureGetReference(sFont, 0);
        m_lstLineWidth.m_nCount = 0;
    }
}

#ifdef _XBOX
CAurGUIStringInternal* NewCAurGUIString(char* sString, char* sFont,  // Xbox 0x002CB640
                                        const Vector& vColor)
{
    return new CAurGUIStringInternal(sString, sFont, vColor);
}
#else
CAurGUIStringInternal* NewCAurGUIString(char* sString, char* sFont,  // PC 0x0045BDF0
                                        const Vector& vColor, float fScale)
{
    return new CAurGUIStringInternal(sString, sFont, vColor, fScale);
}
#endif

static inline void AurguiTrace2()
{
    AurguiTrace1();
}

static inline void AurguiTrace1()
{
}

#ifdef _XBOX
CAurGUIDraw::~CAurGUIDraw()  // Xbox 0x002C8710
{
}
// Xbox 0x002C8700
CAurGUIDraw::CAurGUIDraw()
{
}
#endif
