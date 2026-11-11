#include "aurtexman.h"
#include "aurrendergl.h"
#include "aurtexman.h"

#pragma inline_recursion(on)

List<CAurFont*> CAurFont::s_LoadedFontList;
void AurTextureRelease(CAurTexture* pTexture);
int rint(float fValue);

int CAurFont::ProcessTextMarkup(char* sText, CAurFont** ppFont,
                                CStringColor& cColor)
{
    int nLength;
    char chMarkup;

    cColor = g_cSavedCol;
    *ppFont = this;
    nLength = 0;

    switch (sText[1])
    {
    case '/':
        chMarkup = sText[2];
        if (chMarkup != 'c')
        {
            if (chMarkup == 'f')
            {
                *ppFont = 0;
                nLength = 4;
            }
        }
        else
        {
            cColor = g_cSavedCol;
            nLength = 4;
        }
        break;

    case 'c':
        cColor.m_fRed = (unsigned char)sText[2] * 0.0039215689f;
        cColor.m_fGreen = (unsigned char)sText[3] * 0.0039215689f;
        cColor.m_fBlue = (unsigned char)sText[4] * 0.0039215689f;
        cColor.m_fAlpha = 1.0f;
        nLength = 6;
        break;

    case 'f':
        *ppFont = s_LoadedFontList.m_pList[(unsigned char)sText[2] - '0'];
        nLength = 4;
        break;

    case '<':
        return 1;

    default:
        break;
    }

    return nLength;
}

#ifndef _XBOX

static inline unsigned long RequestFontPool(unsigned long nSize, unsigned long* pnPool)
{
    return GLRender::RequestPool(Aurora::POOL_VERTEX, nSize,
                                 RequestPoolManager::POOL_STATIC | RequestPoolManager::POOL_COLOR,
                                 RequestPoolManager::POOL_FORMAT_VTC, pnPool);
}

int CAurFont::TextOutA(char* sText, int nLength)
{

    static int poolSize = 100;
    static unsigned long stringToken = RequestFontPool(poolSize * sizeof(FontVertex), &stringToken);
    static float fCurrentIndent = 0.0f;

    if (nLength * 6 > poolSize)
    {
        GLRender::ResizePool(stringToken, nLength * 6 * sizeof(FontVertex));
        poolSize = nLength * 6;
    }

    Vector vBottomLeft;
    Vector vBottomRight;
    Vector vTopLeft;
    Vector vTopRight;
    vBottomLeft.m_fY = m_pFontInfo->m_fBaselineHeight - m_pFontInfo->m_fHeight;
    vBottomRight.m_fY = m_pFontInfo->m_fBaselineHeight - m_pFontInfo->m_fHeight;
    vTopLeft.m_fY = m_pFontInfo->m_fBaselineHeight;
    vTopRight.m_fY = m_pFontInfo->m_fBaselineHeight;

    int nVertices = 0;
    CStringColor cColor = g_cSavedCol;
    FontVertex* pVertices = (FontVertex*)GLRender::LockPool(
        stringToken, 0, 0, RequestPoolManager::POOL_LOCK_WRITE);
    bool bColorChanged = true;
    unsigned int nColor = 0;
    for (int i = 0; i < nLength; i++)
    {
        unsigned char ch = sText[i];
        if (ch == '<')
        {
            CAurFont* pFont;
            int nMarkupLength = ProcessTextMarkup(sText + i, &pFont, cColor);
            bColorChanged = true;
            if (nMarkupLength > 1)
            {
                i += nMarkupLength;
                if (pFont == this)
                {
                    i--;
                    continue;
                }
                if (pFont == NULL)
                {
                    GLRender::UnlockPool(stringToken);
                    GLRender::RenderObjectsVTC(Aurora::PRIMITIVE_TRIANGLES,
                                               stringToken, nVertices);
                    return i;
                }
                GLRender::UnlockPool(stringToken);
                GLRender::RenderObjectsVTC(Aurora::PRIMITIVE_TRIANGLES,
                                           stringToken, nVertices);
                nVertices = 0;
                GLRender::LockPool(stringToken, 0, 0,
                                   RequestPoolManager::POOL_LOCK_WRITE);
                pFont->m_pTexture->Bind();
                i += pFont->TextOutA(sText + i, nLength - i);
                pVertices = (FontVertex*)GLRender::LockPool(
                    stringToken, 0, 0, RequestPoolManager::POOL_LOCK_WRITE);
                m_pTexture->Bind();
                i--;
                continue;
            }
        }

        fCurrentIndent += m_pFontInfo->m_fSpacingR;
        vBottomLeft.m_fX = fCurrentIndent;
        vTopLeft.m_fX = fCurrentIndent;
        if (ch >= m_pFontInfo->m_lstLowerRight.m_nCount)
        {
            ch = m_pFontInfo->m_lstLowerRight.m_nCount - 1;
        }
        if (ch >= m_pFontInfo->m_lstUpperLeft.m_nCount)
        {
            ch = m_pFontInfo->m_lstUpperLeft.m_nCount - 1;
        }
        fCurrentIndent +=
            (m_pFontInfo->m_lstLowerRight[ch].m_fX -
             m_pFontInfo->m_lstUpperLeft[ch].m_fX) * m_pFontInfo->m_fTextureWidth;
        vBottomRight.m_fX = fCurrentIndent;
        vTopRight.m_fX = fCurrentIndent;
        if (bColorChanged)
        {
            int nRed, nGreen, nBlue, nAlpha;
            roundToInt(nRed, cColor.m_fRed * 255.0f);
            roundToInt(nGreen, cColor.m_fGreen * 255.0f);
            roundToInt(nBlue, cColor.m_fBlue * 255.0f);
            roundToInt(nAlpha, cColor.m_fAlpha * 255.0f);
            nColor = nRed | ((nGreen | ((nBlue | (nAlpha << 8)) << 8)) << 8);
            bColorChanged = false;
        }
        for (int j = 0; j < 6; j++)
        {
            pVertices[nVertices + j].m_nColor = nColor;
        }

        pVertices[nVertices].m_fU = m_pFontInfo->m_lstUpperLeft[ch].m_fX;
        pVertices[nVertices].m_fV = m_pFontInfo->m_lstLowerRight[ch].m_fY;
        pVertices[nVertices].m_fZ = 0.0f;
        pVertices[nVertices].m_fX = vBottomLeft.m_fX;
        pVertices[nVertices].m_fY = vBottomLeft.m_fY;
        nVertices++;
        pVertices[nVertices].m_fU = m_pFontInfo->m_lstLowerRight[ch].m_fX;
        pVertices[nVertices].m_fV = m_pFontInfo->m_lstLowerRight[ch].m_fY;
        pVertices[nVertices].m_fZ = 0.0f;
        pVertices[nVertices].m_fX = vBottomRight.m_fX;
        pVertices[nVertices].m_fY = vBottomRight.m_fY;
        nVertices++;
        pVertices[nVertices].m_fU = m_pFontInfo->m_lstUpperLeft[ch].m_fX;
        pVertices[nVertices].m_fV = m_pFontInfo->m_lstUpperLeft[ch].m_fY;
        pVertices[nVertices].m_fX = vTopLeft.m_fX;
        pVertices[nVertices].m_fY = vTopLeft.m_fY;
        pVertices[nVertices].m_fZ = 0.0f;
        nVertices++;
        pVertices[nVertices].m_fU = m_pFontInfo->m_lstUpperLeft[ch].m_fX;
        pVertices[nVertices].m_fV = m_pFontInfo->m_lstUpperLeft[ch].m_fY;
        pVertices[nVertices].m_fX = vTopLeft.m_fX;
        pVertices[nVertices].m_fY = vTopLeft.m_fY;
        pVertices[nVertices].m_fZ = 0.0f;
        nVertices++;
        pVertices[nVertices].m_fU = m_pFontInfo->m_lstLowerRight[ch].m_fX;
        pVertices[nVertices].m_fV = m_pFontInfo->m_lstLowerRight[ch].m_fY;
        pVertices[nVertices].m_fX = vBottomRight.m_fX;
        pVertices[nVertices].m_fY = vBottomRight.m_fY;
        pVertices[nVertices].m_fZ = 0.0f;
        nVertices++;
        pVertices[nVertices].m_fU = m_pFontInfo->m_lstLowerRight[ch].m_fX;
        pVertices[nVertices].m_fV = m_pFontInfo->m_lstUpperLeft[ch].m_fY;
        pVertices[nVertices].m_fX = vTopRight.m_fX;
        pVertices[nVertices].m_fY = vTopRight.m_fY;
        pVertices[nVertices].m_fZ = 0.0f;
        nVertices++;
    }
    GLRender::UnlockPool(stringToken);
    GLRender::RenderObjectsVTC(Aurora::PRIMITIVE_TRIANGLES, stringToken, nVertices);
    fCurrentIndent = 0.0f;
    return nLength;
}
#endif

CStringColor g_cSavedCol(1.0f, 1.0f, 1.0f, 1.0f);
// PC 0x004CE9E0
CAurFont::~CAurFont()
{
    CAurTexture* pTexture = m_pTexture.m_pObject;

    m_pTexture = NULL;
    AurTextureRelease(pTexture);
    m_pTexture = NULL;

    if (m_nLoadedFontIndex >= 0)
    {
        for (int i = m_nLoadedFontIndex + 1; i < s_LoadedFontList.m_nCount; i++)
        {
            s_LoadedFontList.m_pList[i]->m_nLoadedFontIndex--;
        }
        s_LoadedFontList.DelIndex(m_nLoadedFontIndex);
    }
}
// PC 0x004CEAF0
void AurDeleteAllFonts()
{
    int i;

    for (i = CAurFont::s_LoadedFontList.m_nCount - 1; i >= 0; i--)
    {
        CAurFont* pFont = CAurFont::s_LoadedFontList.m_pList[i];

        if (pFont != 0)
        {
            delete pFont;
        }
    }
}
