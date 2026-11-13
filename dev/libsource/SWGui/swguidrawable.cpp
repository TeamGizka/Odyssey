#include "swguidrawable.h"
#include "aurscene.h"
#include "aurcamera.h"
#include "aurmodel.h"
#include "resgff.h"
#include "swgui.h"

Vector CSWGuiDrawable::NO_COLOURING(-1.0f, -1.0f, -1.0f);

float CSWGuiDrawable::m_fAlphaPulseTimer = 0.75f;
float CSWGuiDrawable::m_fPulsingAlpha = 1.0f;
int CSWGuiDrawable::m_bAlphaPulsingUp;
// PC 0x00414270 GOG 0x00414220
void CSWGuiDrawable::UpdatePulsing(float fElapsed)
{
    float fStartAlpha;
    float fEndAlpha;

    m_fAlphaPulseTimer -= fElapsed;
    if (m_fAlphaPulseTimer <= 0.0f)
    {
        m_fAlphaPulseTimer = 0.75f;
        m_bAlphaPulsingUp = !m_bAlphaPulsingUp;
    }
    if (m_bAlphaPulsingUp)
    {
        fStartAlpha = 1.0f;
        fEndAlpha = 0.4f;
    }
    else
    {
        fStartAlpha = 0.4f;
        fEndAlpha = 1.0f;
    }
    m_fPulsingAlpha = fEndAlpha + (fStartAlpha - fEndAlpha) *
                                      ((0.75f - m_fAlphaPulseTimer) * 1.3333334f);
}

#include "aurgui.h"
// PC 0x00414300 GOG 0x004142B0
void CSWGuiBorderParams::SetCornerImage(const CResRef& cImage, int bForceReload)
{
    if (cImage != m_cCornerImage || bForceReload != 0)
    {
        m_cCornerImage = cImage;
        if (m_pBorder != NULL)
        {
            if (m_pBorder->m_pCornerImage != NULL)
            {
                delete m_pBorder->m_pCornerImage;
                m_pBorder->m_pCornerImage = NULL;
            }
            if (m_cCornerImage.IsValid())
            {
                char szImage[17];

                m_cCornerImage.CopyToString(szImage);
                m_pBorder->m_pCornerImage = NewCAurGUIImage(szImage);
            }
        }
    }
}
// PC 0x00414390 GOG 0x00414340
void CSWGuiBorderParams::SetEdgeImage(const CResRef& cImage, int bForceReload)
{
    if (cImage != m_cEdgeImage || bForceReload != 0)
    {
        m_cEdgeImage = cImage;
        if (m_pBorder != NULL)
        {
            if (m_pBorder->m_pEdgeImage != NULL)
            {
                delete m_pBorder->m_pEdgeImage;
                m_pBorder->m_pEdgeImage = NULL;
            }
            if (m_cEdgeImage.IsValid())
            {
                char szImage[17];

                m_cEdgeImage.CopyToString(szImage);
                m_pBorder->m_pEdgeImage = NewCAurGUIImage(szImage);
            }
        }
    }
}
// PC 0x00414420 GOG 0x004143D0
void CSWGuiBorderParams::SetFillImage(const CResRef& cImage, int bForceReload)
{
    if (cImage != m_cFillImage || bForceReload != 0)
    {
        m_cFillImage = cImage;
        if (m_pBorder != NULL)
        {
            if (m_pBorder->m_pFillImage != NULL)
            {
                delete m_pBorder->m_pFillImage;
                m_pBorder->m_pFillImage = NULL;
            }
            if (m_cFillImage.IsValid())
            {
                char szImage[17];

                m_cFillImage.CopyToString(szImage);
                m_pBorder->m_pFillImage = NewCAurGUIImage(szImage);
            }
        }
    }
}
// PC 0x004144B0 GOG 0x00414460
void CSWGuiBorderParams::SetBorder(CSWGuiBorder* pBorder)
{
    m_pBorder = pBorder;
    if (pBorder != NULL)
    {
        SetCornerImage(m_cCornerImage, 1);
        SetEdgeImage(m_cEdgeImage, 1);
        SetFillImage(m_cFillImage, 1);
    }
}
// PC 0x004144F0 GOG 0x004144A0
int CSWGuiBorderParams::GetBorderDim() const
{
    int nBorderDim = 0;

    if (m_nBorderDim != 0)
    {
        nBorderDim = m_nBorderDim;
    }
    else if (m_pBorder != NULL && m_pBorder->m_pCornerImage != NULL &&
             m_pBorder->m_pEdgeImage != NULL)
    {
        nBorderDim = m_pBorder->m_pCornerImage->GetImageHeight();
    }
    return nBorderDim;
}
// PC 0x00414520 GOG 0x004144D0
void CSWGuiBorderParams::SetPulsingAlpha(int bPulsing, int bLowAlpha,
                                         int bHighAlpha)
{
    m_bPulsing = bPulsing;
    m_bLowAlpha = bLowAlpha;
    m_bHighAlpha = bHighAlpha;
}
// PC 0x00414550 GOG 0x00414500
CSWGuiImageParams::CSWGuiImageParams(const CResRef& cImage, int bTiled,
                                   int nDrawStyle, float fRotation,
                                   int nAlignment, float fAlpha,
                                   const Vector& vColor)
{
    m_cImage = cImage;
    m_fRotation = fRotation;
    m_fAlpha = fAlpha;
    m_bPulsing = 0;
    m_bTiled = bTiled;
    m_nDrawStyle = nDrawStyle;
    m_nAlignment = nAlignment;
    m_vColor = vColor;
    m_pImage = NULL;
}

#include "clientexoappinternal.h"
// PC 0x00414630 GOG 0x004145E0
void CSWGuiTextParams::SetColor(const Vector& vColor)
{
    m_vColor = vColor;
    if (m_pText != NULL && m_pText->m_pString != NULL)
    {
        m_pText->m_pString->SetColor(m_vColor);
    }
}
// PC 0x00414670 GOG 0x00414620
void CSWGuiTextParams::SetDefaultColor(const Vector& vColor)
{
#ifndef _XBOX
    m_vOriginalColor = vColor;
#endif
    SetColor(vColor);
}
// PC 0x004146A0 GOG 0x00414650
void CSWGuiTextParams::RestoreDefaultColor()
{
#ifndef _XBOX
    SetColor(m_vOriginalColor);
#endif
}
// PC 0x004146B0 GOG 0x00414660
void CSWGuiText::WrapString()
{
    if (m_pString != NULL && m_cExtent.m_nHeight != 0 &&
        m_cExtent.m_nWidth != 0)
    {
        m_pString->WrapStrings(-1);
    }
}
#ifndef _XBOX
int CSWGuiText::GetIdealHeight()  // PC 0x004146D0 GOG 0x00414680
{
    int nHeight;

    if (m_pString != NULL)
    {
        nHeight = m_pString->GetFontPixelHeight();
        if (nHeight < 16)
        {
            return m_pString->GetIdealPixelHeight(-1) + 1;
        }
        return m_pString->GetIdealPixelHeight(-1);
    }
    return 0;
}
// PC 0x00414700 GOG 0x004146B0
int CSWGuiText::GetFontHeight()
{
    int nHeight;

    if (m_pString != NULL)
    {
        nHeight = m_pString->GetFontPixelHeight();
        if (nHeight < 16)
        {
            return m_pString->GetFontPixelHeight() + 1;
        }
        return m_pString->GetFontPixelHeight();
    }
    return 0;
}
#endif
CSWGuiExtent CSWGuiText::GetIdealWidthAndHeight()  // PC 0x00414730 GOG 0x004146E0
{
    CSWGuiExtent cExtent;
    int nHeight;
    int nWidth;

#ifndef _XBOX
    nHeight = m_pString->GetIdealPixelHeight(m_nWrapWidth);
    nWidth = m_nWrapWidth;
#else
    nHeight = m_pString->GetIdealPixelHeight(m_cExtent.m_nWidth);
    nWidth = m_cExtent.m_nWidth;
#endif
    while (nHeight == m_pString->GetIdealPixelHeight(nWidth))
    {
        nWidth -= 10;
        if (nWidth <= 0)
        {
#ifndef _XBOX
            nWidth = m_nWrapWidth;
#else
            nWidth = m_cExtent.m_nWidth;
#endif
            break;
        }
    }
    nWidth += 10;
    if (m_pString->GetFontPixelHeight() < 16)
    {
        nHeight++;
    }
    cExtent.m_nWidth = nWidth;
    cExtent.m_nHeight = nHeight;
    return cExtent;
}
// PC 0x004147A0 GOG 0x00414750
inline CSWGuiDrawable::~CSWGuiDrawable()
{
}
// PC 0x00416CC0 GOG 0x00416CF0
CSWGuiScene::CSWGuiScene() : m_lModels(0)
{
    m_pScene = NewCAurScene("scene");
    m_pCamera = NewCAurCamera("camera");
    if (m_pScene != NULL && m_pCamera != NULL)
    {
        m_pScene->InitBSP(10, 10, 5, true, 10.0f);
        m_pScene->DisableVisibilityGraph();
        m_pScene->SetFog(0);
        m_pCamera->SetClipDist(0.1f, 10000.0f);
        m_pCamera->AttachToScene(m_pScene);
        m_vColor = CSWGuiDrawable::NO_COLOURING;
    }
}
// PC 0x004147D0 GOG 0x00414780
void CSWGuiScene::Draw(float fElapsed)
{
    m_pScene->Animate(fElapsed);
    if (AurGUISetupViewport(m_cExtent.m_nLeft, m_cExtent.m_nTop,
                            m_cExtent.m_nWidth, m_cExtent.m_nHeight, m_vColor,
                            true, 1.0f) != 0)
    {
        m_pCamera->RenderScene();
        AurGUICloseViewport();
    }
}
// PC 0x00415E70 GOG 0x00415E20
CSWGuiScene::~CSWGuiScene()
{
    for (int nModel = 0; nModel < m_lModels.num; ++nModel)
    {
        delete m_lModels.element[nModel];
    }
    m_lModels.num = 0;
    if (m_pCamera != NULL)
    {
        delete m_pCamera;
        m_pCamera = NULL;
    }
    if (m_pScene != NULL)
    {
        delete m_pScene;
        m_pScene = NULL;
    }
#ifndef _XBOX
    if (g_pExoResMan->m_bMainMenuImageLoaded) {
        g_pExoResMan->RemoveResourceImageFile("RIMS:MAINMENU");
        g_pExoResMan->m_bMainMenuImageLoaded = 0;
    }
    if (g_pExoResMan->m_bCharGenImageLoaded) {
        g_pExoResMan->RemoveResourceImageFile("RIMS:CHARGEN");
        g_pExoResMan->m_bCharGenImageLoaded = 0;
    }
#endif
}
// PC 0x00415FA0 GOG 0x00415F50
int CSWGuiScene::RemoveModel(int nModel, int bDestroy)
{
    CAurObject* pModel;

    if (nModel < 0 || nModel >= m_lModels.num)
    {
        return 0;
    }
    pModel = m_lModels.element[nModel];
    m_lModels.DelIndex(nModel);
    if (bDestroy != 0 && pModel != NULL)
    {
        delete pModel;
    }
    return 1;
}
// PC 0x00415FE0 GOG 0x00415F90
CAurObject* CSWGuiScene::GetModel(int nModel)
{
    if (nModel >= 0 && nModel < m_lModels.num)
    {
        return m_lModels[nModel];
    }
    return NULL;
}
// PC 0x00416DD0 GOG 0x00416E00
int CSWGuiScene::AddModel(CAurObject* pModel, int nModel, int bAttach)
{
    if (pModel == NULL)
    {
        return 0;
    }

    if (nModel < 0 || nModel >= m_lModels.num)
    {
        m_lModels.Add(pModel);
    }
    else
    {
        m_lModels.Insert(pModel, nModel);
    }

    if (bAttach != 0)
    {
        ((Gob*)pModel)->AttachToScene((Scene*)m_pScene);
    }

    return 1;
}
// PC 0x00416E30 GOG 0x00416E60
CAurObject* CSWGuiScene::AddModel(const CExoString& sModel, int nFlags)
{
    CAurObject* pModel;

    pModel = NewCAurObject((char*)sModel.CStr(), "", NULL, NULL);
    if (AddModel(pModel, nFlags, 1) != 0)
    {
        return pModel;
    }
    return NULL;
}
// PC 0x00416000 GOG 0x00416030
CSWGuiBorder::CSWGuiBorder()
    : m_cParams(CResRef(), CResRef(), CResRef(), 0, 0, 1.0f,
                Vector(1.0f, 1.0f, 1.0f), 0)
{
    m_pCornerImage = NULL;
    m_pEdgeImage = NULL;
    m_pFillImage = NULL;
    m_cParams.SetBorder(this);
}
// PC 0x00414820 GOG 0x004147D0
CSWGuiBorderParams::CSWGuiBorderParams(const CResRef& cCorner,
                                       const CResRef& cEdge,
                                       const CResRef& cFill, int nFillMode,
                                       int nBorderDim, float fAlpha,
                                       const Vector& vColor, int nInnerOffset, int n8)
{
    m_cCornerImage = cCorner;
    m_cEdgeImage = cEdge;
    m_cFillImage = cFill;
    m_nBorderDim = nBorderDim;
    m_nFillMode = nFillMode;
    m_fHilightAlpha = fAlpha;
    m_bPulsing = 0;
    m_vColor = vColor;
    m_bLowAlpha = 0;
    m_bHighAlpha = 0;
    m_pBorder = NULL;
    m_fRotation = 0.0f;
    m_nInnerOffset = nInnerOffset;
    m_n8 = n8;
}

// PC 0x004148F0 GOG 0x004148A0
CSWGuiBorderParams& CSWGuiBorderParams::operator=(const CSWGuiBorderParams& cParams)
{
    m_cCornerImage = cParams.m_cCornerImage;
    m_cEdgeImage = cParams.m_cEdgeImage;
    m_cFillImage = cParams.m_cFillImage;
    m_nFillMode = cParams.m_nFillMode;
    float fRotation = cParams.m_fRotation;
    m_fRotation = fRotation;
    m_nBorderDim = cParams.GetBorderDim();
    float fAlpha = cParams.m_fHilightAlpha;
    m_fHilightAlpha = fAlpha;
    m_bPulsing = cParams.m_bPulsing;
    m_vColor = cParams.m_vColor;
    m_bLowAlpha = cParams.m_bLowAlpha;
    m_bHighAlpha = cParams.m_bHighAlpha;
    m_nInnerOffset = cParams.m_nInnerOffset;
    m_n8 = cParams.GetVerticalInnerOffset();
    SetBorder(m_pBorder);
    return *this;
}
// PC 0x004149D0 GOG 0x00414980
void CSWGuiBorder::Initialize(const CSWGuiExtent& cExtent,
                              const CSWGuiBorderParams& cParams)
{
    m_cExtent = cExtent;
    m_cParams = cParams;
}
// PC 0x00414A00 GOG 0x004149B0
CSWGuiBorder::~CSWGuiBorder()
{
    if (m_pCornerImage != NULL)
    {
        delete m_pCornerImage;
        m_pCornerImage = NULL;
    }
    if (m_pEdgeImage != NULL)
    {
        delete m_pEdgeImage;
        m_pEdgeImage = NULL;
    }
    if (m_pFillImage != NULL)
    {
        delete m_pFillImage;
        m_pFillImage = NULL;
    }
}
// PC 0x00414A80 GOG 0x00414A30
CSWGuiExtent CSWGuiBorder::GetInnerExtent()
{
    int nCornerHeight;
    int nCornerWidth;
    int nEdgeHeight;
    int nEdgeWidth;

    if (m_cParams.GetBorderDim() != 0)
    {
        nCornerHeight = nCornerWidth = nEdgeHeight = nEdgeWidth =
            m_cParams.GetBorderDim();
    }
    else if (m_pCornerImage != NULL && m_pEdgeImage != NULL &&
             m_cExtent.m_nHeight != 0 && m_cExtent.m_nWidth != 0)
    {
        nCornerHeight = m_pCornerImage->GetImageHeight();
        nCornerWidth = m_pCornerImage->GetImageWidth();
        nEdgeHeight = m_pEdgeImage->GetImageHeight();
        nEdgeWidth = m_pEdgeImage->GetImageWidth();
    }
    else
    {
        nCornerHeight = nCornerWidth = nEdgeHeight = nEdgeWidth = 0;
    }

    CSWGuiExtent cInner;
    int nWidth = m_cExtent.m_nWidth - 2 * nCornerWidth;
    int nColumns = nEdgeWidth != 0 ? nWidth / nEdgeWidth : 0;
    if (nWidth > 0 && (nColumns != 0 || nEdgeWidth / 2 <= nWidth))
    {
        cInner.m_nLeft = m_cExtent.m_nLeft + nCornerWidth;
        cInner.m_nWidth = m_cExtent.m_nWidth - 2 * nCornerWidth < 0 ? 0 :
            m_cExtent.m_nWidth - 2 * nCornerWidth;
    }
    else
    {
        cInner.m_nLeft = m_cExtent.m_nLeft + m_cExtent.m_nWidth / 2;
        cInner.m_nWidth = 0;
    }

    int nHeight = m_cExtent.m_nHeight - 2 * nCornerHeight;
    int nRows = nEdgeWidth != 0 ? nHeight / nEdgeWidth : 0;
    if (nHeight > 0 && (nRows != 0 || nEdgeWidth / 2 <= nHeight))
    {
        cInner.m_nTop = m_cExtent.m_nTop + nCornerHeight;
        cInner.m_nHeight = m_cExtent.m_nHeight - 2 * nCornerHeight < 0 ? 0 :
            m_cExtent.m_nHeight - 2 * nCornerHeight;
    }
    else
    {
        cInner.m_nTop = m_cExtent.m_nTop + m_cExtent.m_nHeight / 2;
        cInner.m_nHeight = 0;
    }

    int nInnerOffset = m_cParams.GetInnerOffset();
    int nVerticalOffset = m_cParams.GetVerticalInnerOffset();
    cInner.m_nTop -= nVerticalOffset;
    cInner.m_nLeft -= nInnerOffset < m_cParams.GetBorderDim() ?
        nInnerOffset : m_cParams.GetBorderDim();
    cInner.m_nWidth += 2 * (nInnerOffset < m_cParams.GetBorderDim() ?
        nInnerOffset : m_cParams.GetBorderDim());
    cInner.m_nHeight += 2 * nVerticalOffset;
    return cInner;
}

inline void CSWGuiImageParams::SetImage(const CResRef& cImage)
{
    SetImage(cImage, 0);
}
// PC 0x00414C20 GOG 0x00414BD0
void CSWGuiBorder::Load(CResGFF* pGFF, CResStruct& cStruct,
                       const CExoString& sName)
{
    CResStruct cBorder;

    if (pGFF->GetStructFromStruct(&cBorder, &cStruct, (char*)sName.CStr()))
    {
        int bSuccess;

        m_cParams.SetCornerImage(pGFF->ReadFieldCResRef(&cBorder, "CORNER",
                                  bSuccess, ""));
        CSWGuiBorderParams& cParams = m_cParams;
        cParams.SetEdgeImage(pGFF->ReadFieldCResRef(&cBorder, "EDGE",
                                bSuccess, ""));
        cParams.SetFillImage(pGFF->ReadFieldCResRef(&cBorder, "FILL",
                                bSuccess, ""));
        cParams.m_nFillMode = pGFF->ReadFieldINT(&cBorder, "FILLSTYLE",
                                                 bSuccess, 0);
        cParams.m_nBorderDim = pGFF->ReadFieldINT(&cBorder, "DIMENSION",
                                                  bSuccess, 0);
        m_cParams.m_nInnerOffset = pGFF->ReadFieldINT(&cBorder, "INNEROFFSET",
                                                    bSuccess, 0);
        m_cParams.m_n8 = pGFF->ReadFieldINT(&cBorder, "INNEROFFSETY",
                                          bSuccess, 0);
        m_cParams.m_vColor = pGFF->ReadFieldVector(&cBorder, "COLOR", bSuccess,
                                                 Vector(1.0f, 1.0f, 1.0f));
        unsigned char nPulsing = pGFF->ReadFieldBYTE(&cBorder, "PULSING",
                                                    bSuccess, 0);
        cParams.SetPulsingAlpha(nPulsing > 0, nPulsing & 1,
                                  (nPulsing >> 1) & 1);
    }
}
// PC 0x00414E00 GOG 0x00414DB0
void CSWGuiBorder::FillTile(int nHeight, int nWidth, int nX, int nY,
                            float fAlpha, const Vector& vColor)
{
    int nImageWidth = m_pFillImage->GetImageWidth();
    int nImageHeight = m_pFillImage->GetImageHeight();

    if (nImageWidth > 0 && nImageHeight > 0)
    {
        int nColumns = nWidth / nImageWidth;
        int nRows = nHeight / nImageHeight;
        float fRotation = m_cParams.m_fRotation;
        int nHeightExtra;
        int nWidthExtra;

        nHeight -= nRows * nImageHeight;
        nWidth -= nColumns * nImageWidth;
        if (nRows != 0)
        {
            nHeightExtra = nHeight / nRows;
            nHeight -= nHeightExtra * nRows;
        }
        if (nColumns != 0)
        {
            nWidthExtra = nWidth / nColumns;
            nWidth -= nWidthExtra * nColumns;
            int nHeightRemainder = nHeight;
            int nDrawX = nX;
            for (int nColumn = 0; nColumn < nColumns; ++nColumn)
            {
                int nTileWidth = nImageWidth + nWidthExtra;
                if (nWidth != 0)
                {
                    --nWidth;
                    ++nTileWidth;
                }
                if (nRows != 0)
                {
                    nHeight = nHeightRemainder;
                    int nDrawY = nY;
                    for (int nRow = 0; nRow < nRows; ++nRow)
                    {
                        int nTileHeight = nImageHeight + nHeightExtra;
                        if (nHeight != 0)
                        {
                            --nHeight;
                            ++nTileHeight;
                        }
                        m_pFillImage->Draw(nDrawX, nDrawY, nTileWidth,
                                          nTileHeight, 0, fRotation, vColor,
                                          fAlpha);
                        nDrawY += nTileHeight;
                    }
                }
                else
                {
                    m_pFillImage->Draw(nDrawX, nY, nTileWidth, nHeight,
                                      0, fRotation, vColor, fAlpha);
                }
                nDrawX += nTileWidth;
            }
        }
        else if (nRows != 0)
        {
            int nDrawY = nY;
            for (int nRow = 0; nRow < nRows; ++nRow)
            {
                int nTileHeight = nImageHeight + nHeightExtra;
                if (nHeight != 0)
                {
                    --nHeight;
                    ++nTileHeight;
                }
                m_pFillImage->Draw(nX, nDrawY, nWidth, nTileHeight,
                                  0, fRotation, vColor, fAlpha);
                nDrawY += nTileHeight;
            }
        }
        else
        {
            m_pFillImage->Draw(nX, nY, nWidth, nHeight,
                              0, fRotation, vColor, fAlpha);
        }
    }
}
// PC 0x004160D0 GOG 0x00416100
void CSWGuiBorder::Draw(float fElapsed)
{
    if (m_cExtent.m_nWidth == 0 || m_cExtent.m_nHeight == 0)
        return;

    float fBorderAlpha = m_cParams.m_fHilightAlpha;
    float fFillAlpha = m_cParams.m_fHilightAlpha;
    if (m_cParams.m_bPulsing)
    {
        if (m_cParams.m_bLowAlpha)
            fBorderAlpha = m_fPulsingAlpha;
        if (m_cParams.m_bHighAlpha)
            fFillAlpha = m_fPulsingAlpha;
    }

    int nX = m_cExtent.m_nLeft;
    int nY = m_cExtent.m_nTop;
#ifdef _XBOX
    int nCornerHeight = 0;
    int nBottomHeight = 0;
    int nCornerWidth = 0;
    int nRightWidth = 0;
#else
    int nCornerHeight = 0;
    int nCornerWidth = 0;
    int nBottomHeight = 0;
    int nRightWidth = 0;
#endif
    if (m_pCornerImage != NULL)
    {
        int nImageCornerWidth;
        int nImageCornerHeight;
        int nEdgeWidth;
        int nEdgeHeight;
        if (m_cParams.GetBorderDim() == 0)
        {
            nImageCornerWidth = m_pCornerImage->GetImageWidth();
            nImageCornerHeight = m_pCornerImage->GetImageHeight();
            if (m_pEdgeImage != NULL)
            {
                nEdgeWidth = m_pEdgeImage->GetImageWidth();
                nEdgeHeight = m_pEdgeImage->GetImageHeight();
            }
            else
            {
                nEdgeWidth = nImageCornerWidth;
                nEdgeHeight = nImageCornerHeight;
            }
        }
        else
        {
            nImageCornerHeight = nImageCornerWidth = nEdgeHeight = nEdgeWidth =
                m_cParams.GetBorderDim();
        }

        int nWidth = m_cExtent.m_nWidth - 2 * nImageCornerWidth;
        int nColumns = 0;
        if (nWidth > 0)
        {
            nColumns = nEdgeWidth != 0 ? nWidth / nEdgeWidth : 0;
            if (nColumns == 0 && nEdgeWidth / 2 <= nWidth)
                nColumns = 1;
            nWidth -= nEdgeWidth * nColumns;
        }
        int nWidthExtra = nColumns != 0 ? nWidth / nColumns : 0;
        nWidth -= nColumns * nWidthExtra;

        int nHeight = m_cExtent.m_nHeight - 2 * nImageCornerHeight;
        int nRows = 0;
        if (nHeight > 0)
        {

            nRows = nEdgeWidth != 0 ? nHeight / nEdgeWidth : 0;
            if (nRows == 0 && nEdgeWidth / 2 <= nHeight)
                nRows = 1;
            nHeight -= nEdgeHeight * nRows;
        }
        int nHeightExtra = nRows != 0 ? nHeight / nRows : 0;
        nHeight -= nRows * nHeightExtra;

        if (nColumns != 0 && nRows != 0)
        {
            nX += nImageCornerWidth;
            nY += nImageCornerHeight;
            nCornerHeight = nImageCornerHeight;
            nCornerWidth = nImageCornerWidth;
        }
        else if (nColumns != 0)
        {
            nX += nImageCornerWidth;
            nCornerHeight = m_cExtent.m_nHeight / 2;
            nCornerWidth = nImageCornerWidth;
        }
        else if (nRows != 0)
        {
            nY += nImageCornerHeight;
            nCornerHeight = nImageCornerHeight;
            nCornerWidth = m_cExtent.m_nWidth / 2;
        }
        else
        {
            nCornerHeight = m_cExtent.m_nHeight / 2;
            nCornerWidth = m_cExtent.m_nWidth / 2;
        }

        int nRight = m_cExtent.m_nWidth + m_cExtent.m_nLeft - nCornerWidth;
        int nBottom = m_cExtent.m_nTop + m_cExtent.m_nHeight - nCornerHeight;
        if (nRows == 0 && m_cExtent.m_nHeight % 2 != 0)
            --nBottom;
        if (nColumns == 0 && m_cExtent.m_nWidth % 2 != 0)
            --nRight;
        nBottomHeight = m_cExtent.m_nTop + m_cExtent.m_nHeight - nBottom;
        nRightWidth = m_cExtent.m_nWidth + m_cExtent.m_nLeft - nRight;

        m_pCornerImage->DrawBuffered(m_cExtent.m_nLeft, m_cExtent.m_nTop,
            nCornerWidth, nCornerHeight, 0, 0.0f, m_cParams.m_vColor, fBorderAlpha);
        m_pCornerImage->DrawBuffered(m_cExtent.m_nLeft, nBottom,
            nCornerWidth, nBottomHeight, CAurGUIImage::GUI_IMAGE_MAPPING_MIRRORED, 0.0f, m_cParams.m_vColor,
            fBorderAlpha);
        m_pCornerImage->DrawBuffered(nRight, nBottom,
            nRightWidth, nBottomHeight, 0, 180.0f, m_cParams.m_vColor, fBorderAlpha);
        m_pCornerImage->DrawBuffered(nRight, m_cExtent.m_nTop,
            nRightWidth, nCornerHeight, CAurGUIImage::GUI_IMAGE_MAPPING_REVERSED, 0.0f, m_cParams.m_vColor,
            fBorderAlpha);

        if (m_pEdgeImage != NULL)
        {
            for (int nColumn = 0; nColumn < nColumns; ++nColumn)
            {
                int nTileWidth = nEdgeWidth + nWidthExtra;
                if (nWidth != 0)
                {
                    ++nTileWidth;
                    --nWidth;
                }
                m_pEdgeImage->DrawBuffered(nX, m_cExtent.m_nTop,
                    nTileWidth, nCornerHeight, 0, 0.0f, m_cParams.m_vColor, fBorderAlpha);
                m_pEdgeImage->DrawBuffered(nX, nBottom,
                    nTileWidth, nBottomHeight, 0, 180.0f, m_cParams.m_vColor, fBorderAlpha);
                nX += nTileWidth;
            }
            for (int nRow = 0; nRow < nRows; ++nRow)
            {
                int nTileHeight = nEdgeWidth + nHeightExtra;
                if (nHeight != 0)
                {
                    ++nTileHeight;
                    --nHeight;
                }
                m_pEdgeImage->DrawBuffered(m_cExtent.m_nLeft, nY,
                    nCornerWidth, nTileHeight, 0, 90.0f, m_cParams.m_vColor, fBorderAlpha);
                m_pEdgeImage->DrawBuffered(nRight, nY,
                    nRightWidth, nTileHeight, 0, 270.0f, m_cParams.m_vColor, fBorderAlpha);
                nY += nTileHeight;
            }
        }
    }
    CAurGUIImage::FlushBuffer(CAurGUIImage::cm_fAlpha);
    if (m_pFillImage != NULL)
    {
        int nHeight = m_cExtent.m_nHeight - nBottomHeight - nCornerHeight;
        int nWidth = m_cExtent.m_nWidth - nRightWidth - nCornerWidth;
        int nLeft = nCornerWidth + m_cExtent.m_nLeft;
        int nTop = m_cExtent.m_nTop + nCornerHeight;
        switch (m_cParams.m_nFillMode)
        {
        case SWGUI_BORDER_FILL_MODE_TILE:
            FillTile(nHeight, nWidth, nLeft, nTop, fFillAlpha, m_cParams.m_vColor);
            break;
        case SWGUI_BORDER_FILL_MODE_CENTER:
            FillCenter(nHeight, nWidth, nLeft, nTop, fFillAlpha, m_cParams.m_vColor);
            break;
        case CSWGuiBorderParams::FILL_MODE_BACKGROUND:
            {
                float fRotation = m_cParams.m_fRotation;
                m_pFillImage->Draw(nLeft, nTop, nWidth, nHeight, 0,
                    fRotation, m_cParams.m_vColor, fFillAlpha);
            }
            break;
        }
    }
    CAurGUIImage::FlushBuffer(CAurGUIImage::cm_fAlpha);
}
// PC 0x00415040 GOG 0x00414FF0
void CSWGuiBorder::FillCenter(int nHeight, int nWidth, int nX, int nY,
                              float fAlpha, const Vector& vColor)
{
    int nImageWidth = m_pFillImage->GetImageWidth();
    int nImageHeight = m_pFillImage->GetImageHeight();
    float fRotation = m_cParams.m_fRotation;

    if (nImageWidth < nWidth && nImageHeight < nHeight)
    {
        m_pFillImage->Draw(nX + (nWidth - nImageWidth) / 2,
                          nY + (nHeight - nImageHeight) / 2,
                          nImageWidth, nImageHeight, 0, fRotation,
                          vColor, fAlpha);
    }
    else if (nImageWidth < nWidth)
    {
        m_pFillImage->Draw(nX + (nWidth - nImageWidth) / 2, nY,
                          nImageWidth, nHeight, 0, fRotation, vColor, fAlpha);
    }
    else if (nImageHeight < nHeight)
    {
        m_pFillImage->Draw(nX, nY + (nHeight - nImageHeight) / 2,
                          nWidth, nImageHeight, 0, fRotation, vColor, fAlpha);
    }
    else
    {
        m_pFillImage->Draw(nX, nY, nWidth, nHeight,
                          0, fRotation, vColor, fAlpha);
    }
}
// PC 0x00415150 GOG 0x00415100
void CSWGuiImageParams::SetImage(const CResRef& cImage, int bForceReload)
{
    if (cImage != m_cImage || bForceReload != 0)
    {
        m_cImage = cImage;
        if (m_pImage != NULL)
        {
            if (m_pImage->m_pImageObject != NULL)
            {
                delete m_pImage->m_pImageObject;
                m_pImage->m_pImageObject = NULL;
            }
            if (m_cImage.IsValid())
            {
                char szImage[17];

                m_cImage.CopyToString(szImage);
                m_pImage->m_pImageObject = NewCAurGUIImage(szImage);
                if (m_pImage->m_pImageObject != NULL)
                {
                    m_pImage->SetExtent(m_pImage->m_cExtent);
                }
            }
        }
    }
}
// PC 0x004151E0 GOG 0x00415190
void CSWGuiImageParams::SetAlignment(int nAlignment)
{
    m_nAlignment = nAlignment;
    if (m_pImage != NULL)
    {
        m_pImage->SetExtent(m_pImage->m_cExtent);
    }
}
// PC 0x004165F0 GOG 0x00416620
CSWGuiImageParams& CSWGuiImageParams::operator=(const CSWGuiImageParams& cParams)
{
    m_cImage = cParams.m_cImage;
    m_bTiled = cParams.m_bTiled;
    m_nDrawStyle = cParams.m_nDrawStyle;
    float fRotation = cParams.m_fRotation;
    m_fRotation = fRotation;
    m_nAlignment = cParams.m_nAlignment;
    float fAlpha = cParams.m_fAlpha;
    m_fAlpha = fAlpha;
    m_bPulsing = cParams.m_bPulsing;
    m_vColor = cParams.m_vColor;
    SetImageObject(m_pImage);
    return *this;
}
// PC 0x00415210 GOG 0x004151C0
void CSWGuiImageParams::SetDrawStyle(int nDrawStyle)
{
    m_bTiled = nDrawStyle;
    if (m_pImage != NULL)
    {
        m_pImage->SetExtent(m_pImage->m_cExtent);
    }
}
// PC 0x00415240 GOG 0x004151F0
void CSWGuiImageParams::SetImageObject(CSWGuiImage* pImage)
{
    m_pImage = pImage;
    if (pImage != NULL)
    {
        SetImage(m_cImage, 1);
        SetAlignment(m_nAlignment);
        SetDrawStyle(m_bTiled);
    }
}
// PC 0x00415280 GOG 0x00415230
CSWGuiImage::CSWGuiImage()
    : m_cParams(CResRef(), 0, 0, 0.0f,
                CSWGuiImageParams::ALIGNMENT_DEFAULT, 1.0f,
                Vector(1.0f, 1.0f, 1.0f))
{
    m_pImageObject = NULL;
    m_cParams.SetImageObject(this);
}
// PC 0x004166A0 GOG 0x004166D0
void CSWGuiImage::Initialize(const CSWGuiExtent& cExtent,
                            const CSWGuiImageParams& cParams)
{
    m_cExtent = cExtent;
    m_cParams = cParams;
}
// PC 0x004166D0 GOG 0x00416700
void CSWGuiImage::Load(CResGFF* pGFF, CResStruct& cStruct,
                      const CExoString& sName)
{
    CResStruct cImage;

    if (pGFF->GetStructFromStruct(&cImage, &cStruct, (char*)sName.CStr()))
    {
        int bSuccess;
        m_cParams.SetImage(pGFF->ReadFieldCResRef(&cImage, "IMAGE", bSuccess,
                                                ""));
        CSWGuiImageParams& cParams = m_cParams;
        cParams.SetDrawStyle(pGFF->ReadFieldINT(&cImage, "DRAWSTYLE",
                                                bSuccess, 0));
        cParams.m_nDrawStyle = pGFF->ReadFieldINT(&cImage, "FLIPSTYLE",
                                                  bSuccess, 0);
        cParams.SetAlignment(pGFF->ReadFieldINT(&cImage, "ALIGNMENT", bSuccess,
                                                CSWGuiImageParams::ALIGNMENT_DEFAULT));
        float fRotation = pGFF->ReadFieldFLOAT(&cImage, "ROTATE", bSuccess, 0.0f);
        if (!bSuccess)
        {
            switch (pGFF->ReadFieldINT(&cImage, "ROTATESTYLE", bSuccess, 0))
            {
            case 0:
                fRotation = 0.0f;
                break;
            case 1:
                fRotation = 90.0f;
                break;
            case 2:
                fRotation = 180.0f;
                break;
            case 3:
                fRotation = 270.0f;
                break;
            default:
                break;
            }
        }
        m_cParams.m_fRotation = fRotation;
    }
}
// PC 0x00415320 GOG 0x004152D0
CSWGuiImage::~CSWGuiImage()
{
    if (m_pImageObject != NULL)
    {
        delete m_pImageObject;
        m_pImageObject = NULL;
    }
}
// PC 0x00415380 GOG 0x00415330
CSWGuiExtent CSWGuiImage::GetImageExtent()
{
    CSWGuiExtent cExtent;

    if (m_pImageObject != NULL)
    {
        cExtent.m_nWidth = m_pImageObject->GetImageWidth();
        cExtent.m_nHeight = m_pImageObject->GetImageHeight();
    }

    return cExtent;
}
// PC 0x004153C0 GOG 0x00415370
void CSWGuiImage::SetExtent(const CSWGuiExtent& cExtent)
{
    m_cExtent = cExtent;
    if (m_pImageObject != NULL)
    {
        switch (m_cParams.m_bTiled)
        {
        case 0:
            m_cImageExtent = cExtent;
            break;
        case 1:
            m_cImageExtent.m_nHeight = m_pImageObject->GetImageHeight();
            m_cImageExtent.m_nWidth = m_pImageObject->GetImageWidth();

            if (m_cImageExtent.m_nWidth >= m_cExtent.m_nWidth)
            {
                m_cImageExtent.m_nWidth = m_cExtent.m_nWidth;
                m_cImageExtent.m_nLeft = m_cExtent.m_nLeft;
            }
            else
            {
                switch (m_cParams.m_nAlignment & ALIGNMENT_HORIZONTAL)
                {
                case ALIGNMENT_LEFT:
                    m_cImageExtent.m_nLeft = m_cExtent.m_nLeft;
                    break;
                case ALIGNMENT_HCENTER:
                    m_cImageExtent.m_nLeft = m_cExtent.m_nLeft +
                        (m_cExtent.m_nWidth - m_cImageExtent.m_nWidth) / 2;
                    break;
                case ALIGNMENT_RIGHT:
                    m_cImageExtent.m_nLeft = m_cExtent.GetRight() - m_cImageExtent.m_nWidth;
                    break;
                }
            }

            if (m_cImageExtent.m_nHeight >= m_cExtent.m_nHeight)
            {
                m_cImageExtent.m_nHeight = m_cExtent.m_nHeight;
                m_cImageExtent.m_nTop = m_cExtent.m_nTop;
            }
            else
            {
                switch (m_cParams.m_nAlignment & ALIGNMENT_VERTICAL)
                {
                case ALIGNMENT_TOP:
                    m_cImageExtent.m_nTop = m_cExtent.m_nTop;
                    break;
                case ALIGNMENT_VCENTER:
                    m_cImageExtent.m_nTop = m_cExtent.m_nTop +
                        (m_cExtent.m_nHeight - m_cImageExtent.m_nHeight) / 2;
                    break;
                case ALIGNMENT_BOTTOM:
                    m_cImageExtent.m_nTop = m_cExtent.GetBottom() - m_cImageExtent.m_nHeight;
                    break;
                }
            }
            break;
        }
    }
}
// PC 0x004154E0 GOG 0x00415490
void CSWGuiImage::Draw(float fElapsed)
{
    float fAlpha;

    if (m_pImageObject != NULL && m_cExtent.m_nWidth != 0 &&
        m_cExtent.m_nHeight != 0)
    {
        fAlpha = m_cParams.m_fAlpha;
        if (m_cParams.m_bPulsing != 0)
        {
            fAlpha = CSWGuiDrawable::m_fPulsingAlpha;
        }
        float fRotation = m_cParams.m_fRotation;
        m_pImageObject->Draw(m_cImageExtent.m_nLeft, m_cImageExtent.m_nTop,
                             m_cImageExtent.m_nWidth, m_cImageExtent.m_nHeight,
                             m_cParams.m_nDrawStyle, fRotation,
                             m_cParams.m_vColor, fAlpha);
    }
}
#ifndef _XBOX
void CSWGuiTextParams::SetFont(const CResRef& cFont)  // PC 0x00415550 GOG 0x00415500
{
    m_cFont = cFont;
    if (m_pText != NULL && m_pText->m_pString != NULL && m_cFont.IsValid())
    {
        char szFont[17];

        m_cFont.CopyToString(szFont);
        m_pText->m_pString->SetFontTexture(szFont);
        m_pText->WrapString();
    }
}
#endif
// PC 0x004155B0 GOG 0x00415560
void CSWGuiTextParams::SetFontRename()
{
    char szFont[20];
    char szCurrentFont[20];

#ifndef _XBOX
    strcpy(szFont, m_szBaseFont);
    g_pGuiMan->GetUpdatedFontName(szFont);
    m_cFont.CopyToString(szCurrentFont);
    if (strcmpi(szFont, szCurrentFont) != 0)
    {
        CResRef cFont(szFont);
        SetFont(cFont);
    }
#endif
}
#ifndef _XBOX
CSWGuiTextParams::CSWGuiTextParams()  // PC 0x00416860 GOG 0x00416890
{
#ifndef _XBOX
    m_vColor = m_vOriginalColor = Vector(1.0f, 1.0f, 1.0f);
#else
    m_vColor = Vector(1.0f, 1.0f, 1.0f);
#endif
    m_pText = NULL;
    m_nStrRef = -1;
    m_fAlpha = 1.0f;
    m_nAlignment = SWGUI_TEXT_ALIGNMENT_DEFAULT;
    m_bHilightText = 0;
    SetBaseFont("dialogfont16x16");
}
#endif
// PC 0x00416930 GOG 0x00416960
CSWGuiTextParams::CSWGuiTextParams(const CExoString& sText,
                                   const CResRef& cFont,
                                   const Vector& vColor, int nAlignment,
                                   float fAlpha)
{
    m_pText = NULL;
    m_sText = sText;
#ifndef _XBOX
    m_vOriginalColor = vColor;
#endif
    SetBaseFont(cFont);
#ifndef _XBOX
    m_nStrRef = -1;
    m_vColor = m_vOriginalColor;
#else
    m_vColor = vColor; m_nStrRef = -1;
#endif
    m_fAlpha = fAlpha;
    m_nAlignment = nAlignment;
    m_bHilightText = 0;
}
// PC 0x00415620 GOG 0x004155D0
void CSWGuiTextParams::SetBaseFont(const CResRef& cFont)
{
#ifndef _XBOX
    cFont.CopyToString(m_szBaseFont);
    SetFontRename();
#else
    SetFont(cFont);
#endif
}
// PC 0x00415640 GOG 0x004155F0
void CSWGuiTextParams::ReSetFont()
{
    SetFontRename();
}
// PC 0x00415650 GOG 0x00415600
void CSWGuiTextParams::SetText(const CExoString& sText)
{
    m_sText = sText;
    m_nStrRef = -1;
    if (m_pText != NULL && m_pText->m_pString != NULL)
    {
        m_pText->m_pString->SetString((char*)m_sText.CStr());
        m_pText->WrapString();
    }
}
// PC 0x004156A0 GOG 0x00415650
void CSWGuiTextParams::SetStrRef(unsigned long nStrRef)
{
    m_nStrRef = nStrRef;
    if (m_nStrRef != -1)
    {
        STR_RES strRes;

        g_pTlkTable->Fetch(m_nStrRef, strRes, CTlkTable::GENDER_MALE);
        m_sText = strRes.m_sString;
        if (m_pText != NULL && m_pText->m_pString != NULL)
        {
            m_pText->m_pString->SetString((char*)m_sText.CStr());
            m_pText->WrapString();
        }
    }
}
// PC 0x00415770 GOG 0x00415720
void CSWGuiTextParams::SetAlignment(int nAlignment)
{
    m_nAlignment = nAlignment;
    if (m_pText != NULL && m_pText->m_pString != NULL)
    {
        m_pText->m_pString->SetAlignment(m_nAlignment);
        m_pText->WrapString();
    }
}
#ifndef _XBOX
// PC 0x004169F0 GOG 0x00416A20
CSWGuiTextParams& CSWGuiTextParams::operator=(const CSWGuiTextParams& cParams)
{
    m_sText = cParams.m_sText;
    m_nStrRef = cParams.m_nStrRef;
#ifndef _XBOX
    strcpy(m_szBaseFont, cParams.m_szBaseFont);
#endif
#ifndef _XBOX
    m_vOriginalColor = cParams.m_vColor;
#endif
    m_vColor = cParams.m_vColor;
    m_nAlignment = cParams.m_nAlignment;
    float fAlpha = cParams.m_fAlpha;
    m_fAlpha = fAlpha;
    m_bHilightText = cParams.m_bHilightText;
    SetTextObject(m_pText);
    return *this;
}
// PC 0x004157C0 GOG 0x00415770
void CSWGuiTextParams::SetTextObject(CSWGuiText* pText)
{
    m_pText = pText;
    if (pText != NULL)
    {
        if (pText->m_pString == NULL)
        {
            char szFont[20];
            m_cFont.CopyToString(szFont);
#ifdef _XBOX
            m_pText->m_pString = NewCAurGUIString((char*)m_sText.CStr(),
                                                 szFont, m_vColor);
#else
            m_pText->m_pString = NewCAurGUIString((char*)m_sText.CStr(),
                                                 szFont, m_vColor, 1.0f);
#endif
        }
        int nStrRef = m_nStrRef;
        SetFontRename();
        SetText(m_sText);
        SetAlignment(m_nAlignment);
        SetColor(m_vColor);
        m_nStrRef = nStrRef;
    }
}
#endif
CSWGuiText::CSWGuiText()  // PC 0x00416A90 GOG 0x00416AC0
{
    m_pString = NULL;
    m_cParams.SetTextObject(this);
#ifndef _XBOX
    m_nWrapWidth = 400;
#endif
}
// PC 0x00415840 GOG 0x004157F0
CSWGuiText::~CSWGuiText()
{
    if (m_pString != NULL)
    {
        delete m_pString;
        m_pString = NULL;
    }
}
// PC 0x004158B0 GOG 0x00415860
void CSWGuiText::Load(CResGFF* pGFF, CResStruct& cStruct,
                     const CExoString& sName)
{
    CResStruct cText;
    if (pGFF->GetStructFromStruct(&cText, &cStruct, (char*)sName.CStr()))
    {
        int bSuccess;
#ifndef _XBOX
        m_cParams.SetBaseFont(pGFF->ReadFieldCResRef(&cText, "FONT", bSuccess, "dialogfont16x16"));
#else
        m_cParams.SetBaseFont(pGFF->ReadFieldCResRef(&cText, "FONT", bSuccess, CSWGuiTextParams::s_cDefaultFont));
#endif
        CSWGuiTextParams& cParams = m_cParams;
        cParams.SetDefaultColor(pGFF->ReadFieldVector(&cText, "COLOR", bSuccess, Vector(1.0f, 1.0f, 1.0f)));
        cParams.SetText(pGFF->ReadFieldCExoString(&cText, "TEXT", bSuccess, ""));
        cParams.SetStrRef(pGFF->ReadFieldDWORD(&cText, "STRREF", bSuccess, -1));
        cParams.SetAlignment(pGFF->ReadFieldINT(&cText, "ALIGNMENT", bSuccess, SWGUI_TEXT_ALIGNMENT_DEFAULT));
        int bPulsing = pGFF->ReadFieldBYTE(&cText, "PULSING", bSuccess, 0);
        cParams.m_bHilightText = bPulsing;
        if (m_pString != NULL)
        {
            delete m_pString;
            m_pString = NULL;
        }
        char szFont[20];
        m_cParams.m_cFont.CopyToString(szFont);
#ifdef _XBOX
        m_pString = NewCAurGUIString((char*)cParams.m_sText.CStr(), szFont,
                                     m_cParams.m_vColor);
#else
        m_pString = NewCAurGUIString((char*)cParams.m_sText.CStr(), szFont,
                                     m_cParams.m_vColor, 1.0f);
#endif
        cParams.SetTextObject(this);
    }
}
// PC 0x00415AA0 GOG 0x00415A50
void CSWGuiText::Draw(float fElapsed)
{
    float fAlpha;

    if (m_pString != NULL)
    {
        fAlpha = m_cParams.m_fAlpha;
        if (m_cParams.m_bHilightText != 0)
        {
            fAlpha = CSWGuiDrawable::m_fPulsingAlpha;
        }
        m_pString->Draw(fAlpha);
    }
}
// PC 0x00415AE0 GOG 0x00415A90
void CSWGuiText::SetExtent(const CSWGuiExtent& cExtent)
{
    m_cExtent = cExtent;
    if (m_pString != NULL)
    {
        m_pString->SetPositionBoundingRect(cExtent.m_nLeft, cExtent.m_nTop);
        m_pString->SetSizeBoundingRect(cExtent.m_nWidth, cExtent.m_nHeight);
        WrapString();
    }
}
// PC 0x00415B40 GOG 0x00415AF0
void CSWGuiEditText::RemoveLastChar()
{
    CExoString sDisplayText;

    if (m_sText.GetLength() > 0)
    {
        m_sText = m_sText.SubString(0, m_sText.GetLength() - 1);
        if (m_bCaretVisible)
        {
            sDisplayText.Format("%s_", m_sText.CStr());
            m_cParams.SetText(sDisplayText);
        }
        else
        {
            m_cParams.SetText(m_sText);
        }
    }
}
// PC 0x00415C20 GOG 0x00415BD0
void CSWGuiEditText::AddNewChar(char* szChar)
{
    CExoString sDisplayText;

    if (m_nMaxLength == SWGUI_EDITTEXT_MAX_LENGTH_UNLIMITED || m_sText.GetLength() < m_nMaxLength)
    {
        m_sText = m_sText + CExoString(szChar);
        if (m_bCaretVisible)
        {
            sDisplayText.Format("%s_", m_sText.CStr());
            m_cParams.SetText(sDisplayText);
        }
        else
        {
            m_cParams.SetText(m_sText);
        }
    }
}
// PC 0x00415D20 GOG 0x00415CD0
void CSWGuiEditText::SetText(const CExoString& sText)
{
    CExoString sDisplayText;

    m_sText = sText;
    if (m_bCaretVisible)
    {
        sDisplayText.Format("%s_", sText.CStr());
        m_cParams.SetText(sDisplayText);
    }
    else
    {
        m_cParams.SetText(m_sText);
    }
}
// PC 0x00415DC0 GOG 0x00415D70
void CSWGuiEditText::SetCaretVisible(int bVisible)
{
    m_bCaretVisible = bVisible;
    if (bVisible)
    {
        CExoString sDisplayText;

        sDisplayText.Format("%s_", m_sText.CStr());
        m_cParams.SetText(sDisplayText);
    }
    else
    {
        m_cParams.SetText(m_sText);
    }
}
// PC 0x00416B20 GOG 0x00416B50
#ifdef _XBOX
void CSWGuiText::Initialize(const CSWGuiExtent& cExtent,
                            const CSWGuiTextParams& cParams)
#else
void CSWGuiText::Initialize(const CSWGuiExtent& cExtent,
                            const CSWGuiTextParams& cParams, float fScale)
#endif
{
    char szFont[17];

    if (m_pString != NULL)
    {
        delete m_pString;
    }
    cParams.m_cFont.CopyToString(szFont);
#ifdef _XBOX
    m_pString = NewCAurGUIString((char*)cParams.m_sText.CStr(), szFont,
                                cParams.m_vColor);
#else
    m_pString = NewCAurGUIString((char*)cParams.m_sText.CStr(), szFont,
                                cParams.m_vColor, fScale);
#endif
    m_cParams = cParams;
    m_cExtent = cExtent;
    if (m_pString != NULL)
    {
        m_pString->SetPositionBoundingRect(m_cExtent.m_nLeft, m_cExtent.m_nTop);
        m_pString->SetSizeBoundingRect(m_cExtent.m_nWidth, m_cExtent.m_nHeight);
        m_pString->SetAlignment(cParams.m_nAlignment);
        WrapString();
    }
}
// PC 0x00416BE0 GOG 0x00416C10
CSWGuiEditText::CSWGuiEditText()
{
    m_bCaretVisible = 0;
    m_nMaxLength = -1;
    m_sText = "";
}

#ifdef _XBOX
int CSWGuiText::GetIdealHeight()
{
    if (m_pString != NULL)
    {
        return m_pString->GetIdealPixelHeight(-1);
    }
    return 0;
}

int CSWGuiText::GetFontHeight()
{
    if (m_pString != NULL)
    {
        return m_pString->GetFontPixelHeight();
    }
    return 0;
}
#endif

#ifdef _XBOX

CResRef CSWGuiTextParams::s_cDefaultFont("dialogfont16x16");
CResRef CSWGuiTextParams::s_cConsoleFont("fnt_console");

void CSWGuiTextParams::SetFont(const CResRef& cFont)
{
    if (cFont == s_cConsoleFont)
    {
        m_cFont = s_cDefaultFont;
    }
    else
    {
        m_cFont = cFont;
    }
    if (m_pText != NULL && m_pText->m_pString != NULL && m_cFont.IsValid())
    {
        char szFont[17];

        m_cFont.CopyToString(szFont);
        m_pText->m_pString->SetFontTexture(szFont);
        m_pText->WrapString();
    }
}

CSWGuiTextParams::CSWGuiTextParams()
{
    m_cFont = s_cDefaultFont;
    m_vColor = Vector(1.0f, 1.0f, 1.0f);
    m_nAlignment = SWGUI_TEXT_ALIGNMENT_DEFAULT;
    m_bHilightText = 0;
    m_pText = NULL;
    m_nStrRef = -1;
    m_fAlpha = 1.0f;
}

CSWGuiTextParams& CSWGuiTextParams::operator=(const CSWGuiTextParams& cParams)
{
    m_sText = cParams.m_sText;
    m_nStrRef = cParams.m_nStrRef;
    m_cFont = cParams.m_cFont;
    m_vColor = cParams.m_vColor;
    m_nAlignment = cParams.m_nAlignment;
    m_fAlpha = cParams.m_fAlpha;
    m_bHilightText = cParams.m_bHilightText;
    SetTextObject(m_pText);
    return *this;
}

void CSWGuiTextParams::SetTextObject(CSWGuiText* pText)
{
    m_pText = pText;
    if (pText != NULL)
    {
        if (pText->m_pString == NULL)
        {
            char szFont[20];
            m_cFont.CopyToString(szFont);
            m_pText->m_pString = NewCAurGUIString((char*)m_sText.CStr(), szFont, m_vColor);
        }
        int nStrRef = m_nStrRef;
        SetFont(m_cFont);
        SetText(m_sText);
        SetAlignment(m_nAlignment);
        SetColor(m_vColor);
        m_nStrRef = nStrRef;
    }
}

CSWGuiTextParams::CSWGuiTextParams(unsigned long nStrRef, const CResRef& cFont, const Vector& vColor,
                                   int nAlignment, float fAlpha)
{
    m_pText = NULL;
    SetStrRef(nStrRef);
    SetFont(cFont);
    m_vColor = vColor;
    m_nAlignment = nAlignment;
    m_bHilightText = 0;
    m_fAlpha = fAlpha;
}
#endif
