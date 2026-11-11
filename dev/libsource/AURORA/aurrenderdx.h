#ifndef AURRENDERDX_H
#define AURRENDERDX_H

extern LPDIRECT3DDEVICE8 g_pD3DDevice;

extern int g_nMatrixMode;
extern int g_nMatrixStackDepth;
extern LPD3DXMATRIXSTACK g_pProjectionMatrixStack;
extern LPD3DXMATRIXSTACK g_pViewMatrixStack;
extern LPD3DXMATRIXSTACK g_pWorldMatrixStack;
extern LPD3DXMATRIXSTACK g_pTextureMatrixStack;

struct XGBuffer;
struct DXVertexProgram
{
    DXVertexProgram();
    ~DXVertexProgram();
    XGBuffer* m_pConstants;
    XGBuffer* m_pCompiledShader;
    XGBuffer* m_pErrors;
    DWORD m_hShader;
};
extern List<DXVertexProgram*> g_listVertexPrograms;
extern int g_bVertexProgramBound;

struct DXTextureState
{
    DXTextureState()  // Xbox 0x0027F890
    {
        m_nUnknown0 = 0;
        m_nType = 0;
        m_nWrapR = 0;
        m_nWrapS = 3;
        m_nWrapT = 3;
        m_nMagFilter = 5;
        m_nMinFilter = 5;
        m_nMipFilter = 5;
    }
    int m_nUnknown0;
    int m_nType;
    int m_nWrapR;
    int m_nWrapS;
    int m_nWrapT;
    int m_nMagFilter;
    int m_nMinFilter;
    int m_nMipFilter;
};
extern int g_nCurrentTexture;
extern List<D3DBaseTexture*> g_listTextures;
extern List<DXTextureState*> g_listTextureStates;
extern DXTextureState* g_pCurrentTextureState;
extern D3DBaseTexture* g_pCurrentTexture;
extern DWORD g_hGrassVertexShader;
extern int g_bConvolutionSetUp;
extern DWORD g_hConvolutionVertexShader;
extern float g_fConvolutionAlpha;
extern float g_afConvolutionWeights[25];

extern int totalvertexpool;
extern int totalvertexpool4k;
extern int totalindexpool;
extern int totalindexpool4;

unsigned long TranslatePoolUsage(unsigned long nUsage);
unsigned long TranslatePoolFormat(unsigned long nFormat);
unsigned long TranslatePoolLockFlags(unsigned long nFlags);

extern const unsigned int streamTranslationTable[];

class GLRender
{

    friend class ImposterManager;
    friend class CAurTextureBasic;
    friend class CAurTriangleBin;
    friend void EndImposterRender(unsigned long nRecord);
    friend void RenderBumpMapped(VertexPrimitiveFlat* pPrimitive);
    friend void RenderDiffuseBumpMappedPass(VertexPrimitiveFlat* pPrimitive);
    friend void RenderEMBM(VertexPrimitiveFlat* pPrimitive);
    friend void RenderEMLM(VertexPrimitiveFlat* pPrimitive);
    friend void RenderEnvironmentMapped(VertexPrimitiveFlat* pPrimitive, bool, bool);
    friend void RenderFlatDecal(VertexPrimitiveFlat* pPrimitive);
    friend void RenderLightMapped(VertexPrimitiveFlat* pPrimitive);
    friend void RenderLightMappedBumpDiffuse(VertexPrimitiveFlat* pPrimitive);
    friend void RenderLightMappedBumpSpecular(VertexPrimitiveFlat* pPrimitive);
    friend void RenderNV1xEMLM(VertexPrimitiveFlat* pPrimitive);
    friend void RenderNormalMapped(VertexPrimitiveFlat* pPrimitive);
    friend void RenderSpecularBumpMappedPass(VertexPrimitiveFlat* pPrimitive);
    friend void ExecuteSkinTransforms(MdlNodeSkin* pNode, List<Vector>& tbones,
                                      List<Matrix>& rbones,
                                      VertexPrimitiveSkinned* pPrimitive,
                                      bool bBumpedOut);
    friend void RenderSkinnedVP(VertexPrimitiveSkinned* pPrimitive);
    friend void RenderSkinnedVPBlur(VertexPrimitiveSkinned* pPrimitive);
public:

    static void SetFogModef(Aurora::AuroraEnableEnums nMode, float fValue);

    static void SetFogModev(Aurora::AuroraEnableEnums nMode, Vector& vValue);

    static void Clear(unsigned long nFlags);

    static void SetVertexData2f(Aurora::AuroraStreamMode nStream,
                                float fX, float fY)
    {

        g_pD3DDevice->SetVertexData2f(streamTranslationTable[nStream], fX, fY);
    }

    static void SetVertexData4f(Aurora::AuroraStreamMode nStream, float fX, float fY, float fZ, float fW)
    {
        g_pD3DDevice->SetVertexData4f(streamTranslationTable[nStream], fX, fY, fZ, fW);
    }

    static void ApplyMatrix(int nMode);

    enum { MAX_SHADOW_VERTICES = 4096 };
    static unsigned short m_UsedVertices[];
    static float* cm_pUserShadowData;

    enum { SHADOW_VERTEX_USED = 1 };
    static int BuildShadowArray(const Vector& vLight, const float& fRadius,
                                const Plane& plane, const Vector& vNearCap,
                                const Vector& vFarCap, int nVertices,
                                RenderVertexV* pVertices);
    static void RenderShadowArray(int nVertices, int nEdges, int nPool);
    static void InitExtensions();
    static int InitializeAntiAliasing();
    static void Shutdown(int bReleaseRenderer);
    static void DrawFromVerts(int nMode, List<Vector>& lstVertices);
    static void DrawTriangles(List<MaxFace>& lFaces, RenderVertexV* pVertices,
                              RenderVertexC* pColors);
    static void RenderObjectsVTQuadUP(RenderVertexVT* pVertices, int nVertices);
    static void RenderObjectsVCUP(Aurora::AuroraPrimitiveTypes nPrimitive,
                                  RenderVertexVC* pVertices, int nVertices);
    static void RenderObjectsVTCQuadUP(RenderVertexVTC* pVertices, int nVertices);
    static void RenderObjectsVTCQuad(unsigned long nPool, int nVertices);
    static void RenderObjectsVTC(Aurora::AuroraPrimitiveTypes nPrimitive,
                                 unsigned long nPool, int nVertices);

    static void DrawSphereBound(float fRadius, Vector& vCenter,
                                const Vector& vColor, float fScale,
                                bool bBillboard);
    static void InitializeATIFragmentShaders();
    static void EnableFragmentShaders(int nShader, CAurTexture* pTexture0,
        CAurTexture* pTexture1, CAurTexture* pTexture2, CAurTexture* pTexture3);
    static void DrawElementsInShadow(unsigned int nProgram,
                                     unsigned long nLength,
                                     const char* sProgram,
                                     unsigned long* pnProgram);

    static void DrawAxisAlignedFrame(const Vector& vMinimum,
                                     const Vector& vMaximum,
                                     const Vector& vColor);

    static void TransformTextureMatrix(float fAngle)
    {

        if (fAngle != 0.0f)
        {
            int nMode = g_nMatrixMode;
            SetMatrixMode(3);
            PushCurrentMatrix();
            LoadMatrixIdentity();
            Rotate(fAngle, 0.0f, 0.0f, 1.0f);
            SetMatrixMode(nMode);
        }
    }
    static void RestoreTextureMatrix(float fAngle)
    {
        if (fAngle != 0.0f)
        {
            int nMode = g_nMatrixMode;
            SetMatrixMode(3);
            PopCurrentMatrix();
            SetMatrixMode(nMode);
        }
    }
    static void Transform(const Vector& vPosition,
                          const Quaternion& qOrientation, int nForce);

    static void Scale(float fX, float fY, float fZ);
    static void SetViewPort(double fFieldOfView, double fAspect,
                            double fNearClip, double fFarClip);
    static void SetViewPort(int nX, int nY, int nWidth, int nHeight);

    static int cm_bEnablePausePlane;
    static int cm_nAnisotropy;
    static unsigned int cm_uiCurrentStage;
private:
    static unsigned int cm_nGLCurrentTID[4];
public:
    static void SetActiveTexture(unsigned long nStage);
    static void SetTextureCoordGenerationState(
        Aurora::AuroraTexCoordMappingModes nMode);
    static int SetCurrentStageTID(unsigned long nStage, unsigned int nTID);
    static bool cm_bVSync;
    static bool m_bSpeedBlurEnabled;
    static bool m_bSaturationEnabled;
    static bool m_bScanNoiseEnabled;

    static bool m_bFilmNoiseEnabled;
    static bool cm_bFirstFrameBufferFrame;
    static float m_fSaturation;

    static Vector m_vSaturationMultiplier;
    static float m_fAccumulationRatio;
    static HDC m_hDC;
    static HGLRC m_hGLRC;
    static HDC cm_hAccumulationHDC;

    static unsigned int cm_nAccumulationBlur;

    static PBUFFERARB* cm_hAccumulationPBuffer;

    static HGLRC cm_hAccumulationHGLRC;
    static CAurTexture* cm_pFilmNoiseTexture;
    static CAurTexture* cm_pForceDistortionTexture;
    static unsigned int cm_pHeatDistortionVertexProg;
    static unsigned int cm_nBufferRegionTexture;
    static unsigned int cm_tidAttenuationTexture;

    static void BindTexture(Aurora::AuroraEnableEnums nTarget,
                            unsigned long nTexture);
    static unsigned long RequestPool(Aurora::AuroraBufferRequestType nType,
                            unsigned long nSize, unsigned long nUsage,
                            unsigned long nPoolFlags, unsigned long* pnPool);
    static void* LockPool(unsigned long nPool, unsigned long nOffset,
                          unsigned long nSize, unsigned long nCount);
    static void UnlockPool(unsigned long nPool);
    static void DeletePool(unsigned long nPool);
    static void ResizePool(unsigned long nPool, unsigned long nSize);

    static unsigned long AddVertexPool(D3DVertexBuffer* pBuffer, unsigned long nSize);
    static unsigned long AddIndexPool(D3DIndexBuffer* pBuffer, unsigned long nSize);
    static D3DResource* GetPool(unsigned long nPool);
    static void ReleasePools();

    enum { SHADOW_CONSTANT_LIGHT_CLIP = 14, SHADOW_CONSTANT_LIGHT_VIEW = 16 };
    static void SetShadowLightConstants();

    static void GenTextures(unsigned int nCount, unsigned int* pnTextures);

    static void RenderObjectsVTCQuadUPVP(RenderVertexVTC* pVertices, int nVertices);

    enum { MAX_QUAD_VERTICES = 60000 };

    static void RenderObjectsVC(Aurora::AuroraPrimitiveTypes nPrimitive, unsigned long nPool, int nVertices);

    static void SetPoolBufferHeaders(unsigned long nPool, D3DVertexBuffer* pBuffer,
                                     D3DVertexBuffer* pOffsetBuffer, unsigned long nOffset);
    static void SetCombinerConstant0(Vector& vColor, float fAlpha);
    static void SetCombinerConstant1(Vector& vColor, float fAlpha);
    static void SetFragmentShaderConstant0(Vector& vColor, float fAlpha);
    static void SetFragmentShaderConstant1(Vector& vColor, float fAlpha);
private:
    static void (*pfuncSetPixelShaderAmbientColor)(Vector& vColor, float fAlpha);
    static void (*pfuncSetBumpLightColor)(Vector& vColor, float fAlpha);
    static void (*pfuncEnablePixelShaders)(int nShader, CAurTexture* pTexture0,
                                          CAurTexture* pTexture1,
                                          CAurTexture* pTexture2,
                                          CAurTexture* pTexture3);
public:
    static void DisableCombiners(CAurTexture* pTexture);

    static void EnablePixelShaders(int nShader, CAurTexture* pTexture0,
                               CAurTexture* pTexture1, CAurTexture* pTexture2,
                               CAurTexture* pTexture3);
    static PBUFFERARB* CreatePBuffer(int nWidth, int nHeight);

    static void MipmapFilter(HDC hDC, HGLRC hContext, float fWidth,
                             float fHeight, PBUFFERARB* pBuffer,
                             unsigned long nTexture, float fSourceSize);

    static void MipmapFilterATI(HDC hDC, HGLRC hContext, float fWidth,
                                float fHeight, PBUFFERARB* pBuffer,
                                unsigned long nTexture, float fSourceSize);
    static void InitAttenuationTexture();

    static void RenderOverlayFilmNoise(unsigned long nTexture, bool bScanLines);

    static void RenderOverlayFilmNoiseATI(unsigned long nTexture, bool bScanLines);

    static unsigned int cm_nFilmNoiseShaderATI;
    static void RenderOverlayOverbrighten(unsigned long nTexture);

    static void RenderForceDistortion(unsigned long nTexture);

    static void RenderForceDistortionATI(unsigned long nTexture);

    static unsigned int cm_nForceDistortionShaderATI;

    enum { DISTORTION_ENV_PARAMETER = 15 };

    static void RenderOverlayOverbrightenATI(unsigned long nTexture);
    static unsigned int cm_nOverbrightenRed;
    static unsigned int cm_nOverbrightenGreen;
    static unsigned int cm_nOverbrightenBlue;
    static void FrameBufferModificationPixelShaders(unsigned long nMode,
                                                    unsigned long nTexture0,
                                                    unsigned long nTexture1,
                                                    unsigned long nTexture2,
                                                    unsigned long nTexture3);

    enum
    {

        FRAMEBUFFER_SHADER_CONVOLUTION = 1,
        FRAMEBUFFER_SHADER_DISABLE = 2,
        FRAMEBUFFER_SHADER_COPY = 4,

        FRAMEBUFFER_SHADER_FOUR_DECALS = 8,
        FRAMEBUFFER_SHADER_SOFT_SHADOW = 0x256,

        FRAMEBUFFER_SHADER_SATURATION = 0x16,

        FRAMEBUFFER_SHADER_OVERBRIGHTEN = 0x512,

        FRAMEBUFFER_SHADER_FILM_NOISE = 0x32,

        FRAMEBUFFER_SHADER_ACCUMULATION_BLUR = 0x128,

        FRAMEBUFFER_SHADER_FORCE_DISTORTION = 0x64
    };
    static void FrameBufferModificationPixelShadersATI(unsigned long nMode,
                                                       unsigned long nTexture0,
                                                       unsigned long nTexture1,
                                                       unsigned long nTexture2,
                                                       unsigned long nTexture3);
    static void RenderFrameToTextureATI(unsigned long nTexture);

    static void RenderFrameToTexture(unsigned long nTexture);

    static void RenderAccumulationBlur(unsigned long nTexture);

    static void RenderAccumulationBlurATI(unsigned long nTexture);

    static unsigned int cm_nAccumulationBlurShaderATI;
    static void FrameBufferModificationsATI2(unsigned long nFlags);
    static void FrameBufferModifications2(unsigned long nFlags);

    static void FrameBufferModifications(unsigned long nFlags);
    static void FrameBufferModificationsATI(unsigned long nFlags);

    static void RenderOverlayConvolution(D3DTexture* pTexture);

    static void RenderOverlayConvolutionATI(unsigned long nTexture);

    static void BloomFilter(HDC hDC, HGLRC hContext, float fWidth,
                            float fHeight, PBUFFERARB* pBuffer,
                            unsigned long nTexture, float fSourceSize,
                            int bSoftShadows);

    static void BloomFilterATI(HDC hDC, HGLRC hContext, float fWidth,
                               float fHeight, PBUFFERARB* pBuffer,
                               unsigned long nTexture, float fSourceSize,
                               int bSoftShadows);

    static void BloomFilterFromTexture(HDC hDC, HGLRC hContext, float fWidth,
                                       float fHeight, unsigned int nTexture,
                                       float fSourceWidth, float fSourceHeight);
    static void RenderSaturationAdjustment(unsigned long nTexture);

    static void RenderBrightnessAdjustment(unsigned long nTexture);
    static bool cm_bPBuffersAllocated;
    static bool cm_bSetUpFrameBuffer;
    static void RenderSaturationAdjustmentATI(unsigned long nTexture);
    static unsigned int cm_nTargetTexture;

    static void InitializeFrameBufferModifications();
    static void InitializeFrameBufferModificationFragmentShadersATI();

    enum { CONVOLUTION_BUFFER_COUNT = 6 };
private:
    static unsigned int cm_nConvolutionRenderTexture[CONVOLUTION_BUFFER_COUNT];
    static HDC cm_hConvolutionHDC[CONVOLUTION_BUFFER_COUNT];
    static HGLRC cm_hConvolutionHGLRC[CONVOLUTION_BUFFER_COUNT];
    static PBUFFERARB* cm_hConvolutionPBuffer[CONVOLUTION_BUFFER_COUNT];
public:
    static unsigned int cm_nRedTexture;
    static unsigned int cm_nGreenTexture;
    static unsigned int cm_nBlueTexture;
    static unsigned int cm_nOverbrightenShaderATI;
    static unsigned int cm_nSaturationShaderATI;
    static unsigned int cm_nPerPixelFlareShaderATI;
    static unsigned int cm_nDecalShaderATI;
    static unsigned int cm_n4DecalShaderATI;

    enum
    {
        FRAMEBUFFER_LOOKUP_SIZE = 256,
        FRAMEBUFFER_RED_THRESHOLD = 234,
        FRAMEBUFFER_GREEN_THRESHOLD = 216,
        FRAMEBUFFER_BLUE_THRESHOLD = 216,
        FRAMEBUFFER_GREEN_SHIFT = 8,
        FRAMEBUFFER_BLUE_SHIFT = 16
    };

    enum
    {
        FRAMEBUFFER_OVERBRIGHTEN = 1,
        FRAMEBUFFER_CONVOLUTION = 2,
        FRAMEBUFFER_NOISE = 4,
        FRAMEBUFFER_SATURATION = 8,
        FRAMEBUFFER_SPEED_BLUR = 16,
        FRAMEBUFFER_FORCE_DISTORTION = 32
    };
    static void MakeLookUpTexture(float fGamma);

    static int GetAvailableTextureMemory();

    static int SetSpecularMode(int bSeparateSpecular);
    static void SetVertexBuffer(unsigned long nStride, unsigned long nOffset,
                                unsigned long nPool, int nUsage);

    static void SetInterleavedBuffer(unsigned long nStride, int nFlags,
                                    unsigned long nPool, int nVertexCount,
                                    MdlNodeTriMesh* pMesh, unsigned long nOffset);

    enum VertexAttributeIndex
    {
        VERTEX_ATTRIBUTE_SKIN_WEIGHT = 1,
        VERTEX_ATTRIBUTE_BONE_INDEX = 4,
        VERTEX_ATTRIBUTE_TANGENT_BASIS0 = 12,
        VERTEX_ATTRIBUTE_TANGENT_BASIS1 = 13,
        VERTEX_ATTRIBUTE_TANGENT_BASIS2 = 14
    };
    static void SetColorBuffer(unsigned long nStride, unsigned long nOffset,
                               unsigned long nPool, int nUsage);
    static void SetNormalBuffer(unsigned int nStride, unsigned int nOffset,
                                unsigned int nPool, int nUsage);
    static void SetTexCoordBuffer(unsigned long nStride, unsigned long nOffset,
                                  unsigned long nPool, int nUsage);

    static void SetLightmapTexCoordToStage0(unsigned long nStride, int nFlags,
                                            unsigned long nPool,
                                            int nVertexCount,
                                            MdlNodeTriMesh* pMesh,
                                            unsigned long nOffset);
    static void SetDecalTexCoordToStage0(unsigned long nStride, int nFlags,
                                         unsigned long nPool, int nVertexCount,
                                         MdlNodeTriMesh* pMesh,
                                         unsigned long nOffset);
    static void DrawElements(Aurora::AuroraPrimitiveTypes nPrimitive,
                             unsigned long nCount, unsigned long nOffset,
                             unsigned long nPool);
    static void RenderSquares(unsigned int nPool);
    static void RenderDrawableSquares();

    static void RenderBox(const Vector& vMinimum, const Vector& vMaximum, const Vector& vColor);
    // PC 0x00425250
    static void ActivateTextureStage(unsigned long nStage)
    {
        GLRender::cm_uiCurrentStage = nStage;
    }

    static void DrawLightmappedGrass(void* pVertices, int nVertexStride,
                                     void* pIndices, int nStride, int nFirst,
                                     int nQuads);

    static void RenderPausePlane();
    static List<unsigned long> cm_listSquaresToRender;
    static void AddSquareToRender(unsigned long nPool);
    static void RemoveSquareToRender(unsigned long nPool);
    static void MakeNormalizationCubeMap(int nSize);
    static void CreateCubeTexture(CAurTexture* pTexture, bool bMipMaps,
                                  int nIndex);
    static void CreateProcessedCubeTexture(CAurTexture* pTexture, bool bMipMaps,
                                           int nIndex);

    static void CreateTexture(CAurTexture* pTexture, bool bMipMaps, int nIndex, bool bLinear);
    static void CreateProcessedTexture(CAurTexture* pTexture, bool bMipMaps,
                                       int nIndex, bool bLinear);
    static void ConvertHeightFieldAndLoadNormalMapTexture(
        unsigned char* pHeightField, int nWidth, int nHeight, int nRowWidth,
        int nRowCount, float fScale, int bUpdate, bool bNormalMap);
    static int SetUpImposters(int nType, float* pfValues);
    static void SetFilmNoiseTexture(CAurTexture* pTexture);
    static void InitRenderSystem();
    static int GetFogArraysInfoRef();
    static void SetViewTransform(Vector& vPosition, Quaternion& qOrientation);
    static void SetUpSoftShadowsARB();
    static void SetUpSoftShadowsATI();
    static void InitializeImageSpaceSoftShadows();

    static PBUFFERARB* cm_hSoftShadowPBuffer;
    static PBUFFERARB* cm_hSoftShadowTargetPBuffer;
    static unsigned int cm_nSoftShadowRenderTexture;
    static unsigned int cm_nSoftShadowTargetRenderTexture;
    static unsigned int cm_nSoftShadowShaderATI;

    static unsigned int cm_nPerPixelFlareShader;

    static unsigned long cm_nScreenWidthPow2;
    static unsigned long cm_nScreenHeightPow2;

    static HDC cm_hSoftShadowHDC;
    static HDC cm_hSoftShadowTargetHDC;
    static HGLRC cm_hSoftShadowHGLRC;
    static HGLRC cm_hSoftShadowTargetHGLRC;
    static void SetUpSoftShadows();
    static void FinishSoftShadows();
    static void FinishSoftShadowsATI();

    static bool cm_bSetUpImageSpaceShadows;

    enum { STENCIL_WRITE_MASK = 0xFF, SHADOW_STENCIL_MASK = 0xF };

    enum { DISTORTION_STENCIL_MASK = 1 };
    static void SetUpTextFormat();
    static void PrintStringAtScreenPos(char* szString, float fX, float fY);

    static void PushAttrib(unsigned long nMask)
    {
    }
    static void PopAttrib()
    {
    }

    static void BeginImmediatePrimitive(Aurora::AuroraPrimitiveTypes nType);
    static void EndImmediatePrimitive()
    {
        g_pD3DDevice->End();
    }

    static void Enable(Aurora::AuroraEnableEnums nCap);
    static void Disable(Aurora::AuroraEnableEnums nCap);
    static void SetBlendFunc(Aurora::AuroraBlendModes nSource, Aurora::AuroraBlendModes nDestination);

    static void SetTextureStageFlag(unsigned long nStage, int nValue);

    static D3DBaseTexture* cm_pCurrentTexture[4];

    static void PopCurrentMatrix();

    static void EnableClientState(int nArray);
    static void DisableClientState(int nArray);

    static void SetPixelShaderAmbientColor(Vector& vColor, float fAlpha);
    static void SetBumpLightColor(Vector& vColor);

    static void LoadMatrixIdentity();
    static void MultMatrix(const float* pMatrix);
    static void Translate(float fX, float fY, float fZ);
    static void Rotate(float fAngle, float fX, float fY, float fZ);

    static bool IsProgramNV(int nProgram);

    static void BindVertexProgram(int nProgram);

    static void TrackMatrix();

    static void CopyTextureToTextureMemory(void* pImage, D3DLOCKED_RECT* pLock, int nWidth, int nHeight, int nFormat);

    static void ReadPixelsFromBuffer(int nX, int nY, int nWidth, int nHeight, int nFormat, unsigned char* pPixels);

    static D3DTexture* cm_pRedTexture;
    static D3DTexture* cm_pGreenTexture;
    static D3DTexture* cm_pBlueTexture;
    static D3DTexture* cm_pConvolutionTextureSmall;
    static D3DTexture* cm_pConvolutionTextureMedium;
    static D3DTexture* cm_pConvolutionTextureLarge;

    static void SetMatrixMode(int nMode)
    {
        g_nMatrixMode = nMode;
    }

    static void PushCurrentMatrix();

    static void LoadViewIdentity();

    static void SetPolygonRenderMode(int nFace, int nFill)
    {
        g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, fillTranslationTable[nFill]);
    }

    static void GetViewPort(int* pViewport);

    static void SetPerspectiveTransform(double fFieldOfView, double fAspect, double fNearClip, double fFarClip);

    static void SetOrthoTransform(double fLeft, double fRight, double fBottom, double fTop, double fNear, double fFar);

    static void DeleteTextures(unsigned int nCount, const unsigned int* pnTextures);

    static void SetClearColor(float fRed, float fGreen, float fBlue, float fAlpha);

    static void SetTextureParameteri(int nTarget, int nParam, int nValue);

    static void SetColor4f(float fRed, float fGreen, float fBlue, float fAlpha);

    static void SetAlphaFunc(int nFunc, float fReference);
    static void SetStencilOp(int nFail, int nZFail, int nPass);
    static void SetStencilMask(unsigned int nMask);
    static void SetStencilFunc(int nFunc, int nReference, unsigned int nMask);
    static void SetColorMask(unsigned char bRed, unsigned char bGreen, unsigned char bBlue, unsigned char bAlpha);
    // Xbox 0x002AD920
    static bool IsEnabled(Aurora::AuroraEnableEnums nCap)
    {
        if (enableTranslationTable[nCap] == 0x7fffffff)
        {
            return true;
        }
        DWORD nValue;
        g_pD3DDevice->GetRenderState((D3DRENDERSTATETYPE)enableTranslationTable[nCap], &nValue);

        return nValue ? true : false;
    }

    static void SetDepthMask(unsigned char bWrite)
    {
        D3DDevice_SetRenderState(D3DRS_ZWRITEENABLE, bWrite);
    }
    static void SetFrontFace(int nWinding)
    {
        D3DDevice_SetRenderState(D3DRS_CULLMODE, cullTranslationTable[nWinding]);
    }

    static void GenPrograms(int nCount, unsigned int* pnPrograms);

    static void ValidatePool(unsigned long nPool);

    static void BackUpLights();
    static void RestoreLights();
    static void RenderToTexture(int nUnused, D3DTexture* pTarget, D3DBaseTexture* pSource, int nWidth,
                                int nHeight, int nTargetWidth, int nTargetHeight);
    static void CopyFrameBufferTexture(D3DBaseTexture* pSource, D3DTexture* pTarget);

    static void ResetViewPort(int nX, int nY, int nWidth, int nHeight);
    static void RemoveAllSquaresToRender();

    static void DrawTriangles(int nFaces, List<MaxFace>& lFaces, unsigned long nVertexPool, int nVertexStride,
                              int nPositionOffset, short nVertexCount, unsigned long nColorPool,
                              int nPoolOffset);
    static int BuildShadowArray(const Vector& vLight, const float& fRadius, const Plane& plane,
                                const Vector& vNearCap, const Vector& vFarCap, int nVertices,
                                unsigned long nPool, int nOffset, int nStride);

    static D3DTexture* cm_pAttenuationTexture;

    static void RenderOverlayConvolutionPyramid(D3DTexture* pTexture);
    static void RenderAccumulationBlur(D3DTexture* pFrame, D3DTexture* pAccumulation);

    static void RestoreVertexPrograms();
    static int InitializeVertexShaders();
    static int InitializePixelShaders();
    static void EnableLight(unsigned long nLight);

    static void SetLightModel(const float* pfColor);

    static void SetColorv(const float* pColor);
    static DWORD CreateClientStateVertexShader();

    static DWORD GetVertexShader(int bSeparateStreams);
    static unsigned int GetCurrentStageTID();
    static void SetForceDistortionTexture(CAurTexture* pTexture);
    static void ReleaseFrameTexture();
    static void SetFrameBufferMode(int nMode);
    static void SetShadowLightPosition(const Vector& vPosition);

    static void CreateTextureImageRaw(int nTarget, int nLevel, int nComponents, int nWidth, int nHeight, int nBorder,
                           int nFormat, void* pImage);

    static void CreateTexSubImage(int nTarget, int nLevel, int nWidth, int nHeight, int nFormat,
                              void* pImage);

    static void CreateCompressedTexImage(int nTarget, int bMipMaps, int nLevel, int nFormat, int nWidth,
                                     int nHeight, int nBorder, int nSize, void* pData);

    static void CreateMipMaps(int nTarget, int nComponents, int nWidth, int nHeight, int nFormat,
                               void* pImage);

    static void ClearStencilValue(int nStencil);

    static void EnableColorMaterial();
    static void DisableColorMaterial();
    static void SetColorMaterial(int nFace, int nMode);
    static void SetMaterialv(int nFace, int nMaterial, const float* pfParams);

    static void SetLightProperties(unsigned long nLight, int nParameter, const float* pfParams);

    static void DisableLight(unsigned long nLight);

    static void SetFogMode(int nFogMode);
    static void SetDepthFunc(int nFunc);
};

extern bool g_bColorMaterial;
extern int g_nColorMaterialMode;
extern unsigned short g_nClientState;
extern int g_bCustomVertexShader;
extern float g_fBrightness;
extern float g_fContrast;
extern D3DTexture* g_pFrameTexture;
extern bool g_bFrameTextureReleased;
extern int g_bFrameBufferModeChanged;
extern int g_bFrameBufferChanged;
extern D3DXVECTOR4 g_vShadowLightClip;
extern D3DXVECTOR4 g_vShadowLightView;
extern DWORD g_hGrassVertexShader;

int GetPrimitiveCount(int nPrimitive, int nVertices);

class DXBox
{
public:
    DXBox(D3DDevice* pDevice);
    ~DXBox()
    {
        m_pVertexBuffer->Release();
    }
    void Render(D3DDevice* pDevice);
    D3DVertexBuffer* m_pVertexBuffer;
};
class DXCircle
{
public:
    DXCircle(D3DDevice* pDevice);
    ~DXCircle();
    void Render(D3DDevice* pDevice);
    D3DVertexBuffer* m_pVertexBuffer;
};

enum DXPixelShader
{
    PIXEL_SHADER_FOUR_TEXTURE_WEIGHTED = 0,
    PIXEL_SHADER_TEXTURE = 1,
    PIXEL_SHADER_FOUR_TEXTURE_ADD = 2,
    PIXEL_SHADER_THREE_TEXTURE_ADD = 3,
    PIXEL_SHADER_FOG = 4,
    PIXEL_SHADER_ENVIRONMENT = 5,
    PIXEL_SHADER_ENVIRONMENT_LIGHT_MAP = 6,
    PIXEL_SHADER_LIGHT_MAP = 7,
    PIXEL_SHADER_LIGHT_MAP_ONLY = 8,
    PIXEL_SHADER_BUMP_SELF_SHADOW = 9,
    PIXEL_SHADER_BUMP_DIFFUSE = 10,
    PIXEL_SHADER_BUMP_ENVIRONMENT = 11,
    PIXEL_SHADER_BUMP_SPECULAR = 12,
    PIXEL_SHADER_BUMP_SPECULAR_POWER = 13,
    PIXEL_SHADER_BUMP_LIGHT_MAP = 14,
    PIXEL_SHADER_NORMAL_MAP_LIGHT = 15,
    PIXEL_SHADER_NORMAL_MAP_MODULATE = 16,
    PIXEL_SHADER_DEPTH_SPRITE = 17,
    PIXEL_SHADER_COLOR_LOOKUP = 18,
    PIXEL_SHADER_AVERAGE = 19,
    PIXEL_SHADER_ADD = 20,
    PIXEL_SHADER_BLEND = 21,
    PIXEL_SHADER_COLOR_MATRIX = 22,
    PIXEL_SHADER_COLOR_LOOKUP_ALPHA = 23,
    PIXEL_SHADER_TEXTURE_MODULATE = 24,
    PIXEL_SHADER_DEPENDENT_LOOKUP = 25,
    PIXEL_SHADER_DISTORTION = 26,
    PIXEL_SHADER_WHITE = 27
};
extern unsigned long g_ahPixelShader[];
extern int g_nFrameBufferMode;
extern bool g_bRendererShutDown;
extern int g_nLightBackUpDepth;
extern List<D3DLIGHT8> g_lstLightBackUp;
extern List<unsigned long> g_lstVisibilityTests;
extern int g_nStreamBufferIndex;
extern __int64 g_nRenderStartTime;
extern List<unsigned long> g_lstLights;
extern int g_bInitializeLights;
extern D3DVIEWPORT8 g_ViewPort;
extern float g_fFrameBufferTime;

extern int visualizeregular;
extern int dooldflares;
extern Vector distortionadd;
extern List<char*> g_lstShaderSources;
extern List<char*> g_lstShaderBuffers;
extern LPDIRECT3D8 g_pD3D;

#define AUR_ENABLE_SPECIAL 0x7fffffff

extern bool g_bVertexProgramEnabled;
extern int g_nTexturingDisabled;
extern int g_nCurrentBlendSource;
extern int g_nCurrentBlendDestination;
// Xbox 0x00011EA0
inline void GLRender::Enable(Aurora::AuroraEnableEnums nCap)
{
    unsigned long nState = enableTranslationTable[nCap];
    if (nState == AUR_ENABLE_SPECIAL)
    {
        if (nCap == Aurora::ENABLE_VERTEX_PROGRAM)
        {
            g_bVertexProgramEnabled = true;
        }
        else if (nCap == Aurora::ENABLE_TEXTURE_2D)
        {
            g_nTexturingDisabled = 0;
            for (int i = 0; i < 4; i++)
            {
                g_pD3DDevice->SetTexture(i, cm_pCurrentTexture[i]);
                SetTextureStageFlag(i, 0);
            }
        }
        else if (nCap == Aurora::ENABLE_CULL_FACE)
        {
            g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
        }
    }
    else
    {
        g_pD3DDevice->SetRenderState((D3DRENDERSTATETYPE)nState, TRUE);
    }
}
// Xbox 0x0027F8C0
inline void GLRender::Disable(Aurora::AuroraEnableEnums nCap)
{
    unsigned long nState = enableTranslationTable[nCap];
    if (nState == AUR_ENABLE_SPECIAL)
    {
        if (nCap == Aurora::ENABLE_VERTEX_PROGRAM)
        {
            g_bVertexProgramEnabled = false;
        }
        else if (nCap == Aurora::ENABLE_TEXTURE_2D)
        {
            g_pD3DDevice->SetTexture(cm_uiCurrentStage, NULL);
            SetTextureStageFlag(cm_uiCurrentStage, 0);
            cm_pCurrentTexture[cm_uiCurrentStage] = NULL;
        }
        else if (nCap == Aurora::ENABLE_CULL_FACE)
        {
            g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
        }
    }
    else
    {
        g_pD3DDevice->SetRenderState((D3DRENDERSTATETYPE)nState, FALSE);
    }
}
// Xbox 0x00012210
inline void GLRender::SetBlendFunc(Aurora::AuroraBlendModes nSource, Aurora::AuroraBlendModes nDestination)
{
    if (nSource != g_nCurrentBlendSource || nDestination != g_nCurrentBlendDestination)
    {
        g_nCurrentBlendSource = nSource;
        g_nCurrentBlendDestination = nDestination;
        g_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, blendTranslationTable[nSource]);
        g_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, blendTranslationTable[nDestination]);
    }
}

extern int enablecolor;
// Xbox 0x002A3BB0
inline void GLRender::SetAlphaFunc(int nFunc, float fReference)
{
    unsigned long nReference = (unsigned long)(fReference * 255.0f);
    g_pD3DDevice->SetRenderState(D3DRS_ALPHAFUNC, cmpTranslationTable[nFunc]);
    g_pD3DDevice->SetRenderState(D3DRS_ALPHAREF, nReference);
}

inline void GLRender::SetStencilMask(unsigned int nMask)
{
    g_pD3DDevice->SetRenderState(D3DRS_STENCILWRITEMASK, nMask);
}
// Xbox 0x00284C40
inline void GLRender::SetStencilFunc(int nFunc, int nReference, unsigned int nMask)
{
    g_pD3DDevice->SetRenderState(D3DRS_STENCILFUNC, cmpTranslationTable[nFunc]);
    g_pD3DDevice->SetRenderState(D3DRS_STENCILREF, nReference);
    g_pD3DDevice->SetRenderState(D3DRS_STENCILMASK, nMask);
}
// PC 0x0044F820  Xbox 0x00284BF0
inline void AurSetVertexProgramParameter4f(unsigned int nIndex, float x, float y, float z, float w)
{
    float fParameter[4];
    fParameter[0] = x;
    fParameter[1] = y;
    fParameter[2] = z;
    fParameter[3] = w;
    g_pD3DDevice->SetVertexShaderConstant(nIndex, fParameter, 1);
}

inline void GLRender::SetColorMask(unsigned char bRed, unsigned char bGreen, unsigned char bBlue,
                                   unsigned char bAlpha)
{
    if (!enablecolor)
    {
        g_pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE,
                                     (bRed ? D3DCOLORWRITEENABLE_RED : 0) |
                                     (bGreen ? D3DCOLORWRITEENABLE_GREEN : 0) |
                                     (bBlue ? D3DCOLORWRITEENABLE_BLUE : 0) |
                                     (bAlpha ? D3DCOLORWRITEENABLE_ALPHA : 0));
    }
}
// Xbox 0x002A35C0
inline void GLRender::SetFogModef(Aurora::AuroraEnableEnums nMode, float fValue)
{
    fValue = 0.0f - fValue;
    g_pD3DDevice->SetRenderState((D3DRENDERSTATETYPE)enableTranslationTable[nMode], *(unsigned long*)&fValue);
}
// Xbox 0x002A3880
inline void GLRender::SetFogModev(Aurora::AuroraEnableEnums nMode, Vector& vValue)
{
    g_pD3DDevice->SetRenderState((D3DRENDERSTATETYPE)enableTranslationTable[nMode],
                                 D3DCOLOR_ARGB(0, (int)(vValue.m_fX * 255.0f), (int)(vValue.m_fY * 255.0f),
                                               (int)(vValue.m_fZ * 255.0f)));
}
// Xbox 0x002A3B30
inline void GLRender::SetLightModel(const float* pfColor)
{
    g_pD3DDevice->SetRenderState(D3DRS_AMBIENT,
                                 D3DCOLOR_ARGB((int)(pfColor[3] * 255.0f), (int)(pfColor[0] * 255.0f),
                                               (int)(pfColor[1] * 255.0f), (int)(pfColor[2] * 255.0f)));
}

extern D3DCOLOR g_nClearColor;
extern int g_nClearStencil;

extern D3DMATERIAL8 g_Material;
extern int g_bMaterialDirty;
extern bool g_bColorMaterial;
extern int g_nColorMaterialMode;

inline void GLRender::ClearStencilValue(int nStencil)
{
    g_nClearStencil = nStencil;
}

inline void GLRender::EnableColorMaterial()
{
    g_bColorMaterial = true;
}

inline void GLRender::DisableColorMaterial()
{
    g_bColorMaterial = false;
}

inline void GLRender::SetColorMaterial(int nFace, int nMode)
{
    g_nColorMaterialMode = nMode;
}

inline void GLRender::SetMaterialv(int nFace, int nMaterial, const float* pfParams)
{
    g_bMaterialDirty = 1;
    switch (nMaterial)
    {
    case MATERIAL_AMBIENT:
        g_Material.Ambient.r = pfParams[0];
        g_Material.Ambient.g = pfParams[1];
        g_Material.Ambient.b = pfParams[2];
        g_Material.Ambient.a = pfParams[3];
        break;
    case MATERIAL_DIFFUSE:
        g_Material.Diffuse.r = pfParams[0];
        g_Material.Diffuse.g = pfParams[1];
        g_Material.Diffuse.b = pfParams[2];
        g_Material.Diffuse.a = pfParams[3];
        break;
    case MATERIAL_EMISSION:
        g_Material.Emissive.r = pfParams[0];
        g_Material.Emissive.g = pfParams[1];
        g_Material.Emissive.b = pfParams[2];
        g_Material.Emissive.a = pfParams[3];
        break;
    }
}
// Xbox 0x002F3860
inline void GLRender::SetLightProperties(unsigned long nLight, int nParameter, const float* pfParams)
{
    D3DLIGHT8 light;
    if (!g_lstLights.Contains(nLight))
    {
        g_lstLights.Add(nLight);
        memset(&light, 0, sizeof(light));
        light.Type = D3DLIGHT_POINT;
        light.Range = 100000.0f;
        light.Attenuation0 = 1.0f;
        light.Direction = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
        D3DXCOLOR color;
        if (nLight == 0)
        {
            color.a = 1.0f;
            color.r = 1.0f;
            color.g = 1.0f;
            color.b = 1.0f;
        }
        else
        {
            color.a = 1.0f;
            color.r = 0.0f;
            color.g = 0.0f;
            color.b = 0.0f;
        }
        light.Diffuse = color;
        light.Ambient = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
        g_pD3DDevice->SetLight(nLight, &light);
    }
    else
    {
        g_pD3DDevice->GetLight(nLight, &light);
    }
    switch (nParameter)
    {
    case LIGHT_AMBIENT:
        light.Ambient = D3DXCOLOR(pfParams[0], pfParams[1], pfParams[2], pfParams[3]);
        break;
    case LIGHT_DIFFUSE:
        light.Diffuse = D3DXCOLOR(pfParams[0], pfParams[1], pfParams[2], pfParams[3]);
        break;
    case LIGHT_SPECULAR:
        light.Specular = D3DXCOLOR(pfParams[0], pfParams[1], pfParams[2], pfParams[3]);
        break;
    case LIGHT_POSITION:
        light.Position = D3DXVECTOR3(pfParams[0], pfParams[1], pfParams[2]);
        break;
    case LIGHT_QUADRATIC_ATTENUATION:
        light.Attenuation2 = *pfParams;
        break;
    }
    g_pD3DDevice->SetLight(nLight, &light);
}

inline void GLRender::DisableLight(unsigned long nLight)
{
    g_pD3DDevice->LightEnable(nLight, FALSE);
}

inline void GLRender::SetFogMode(int nFogMode)
{
    g_pD3DDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
    g_pD3DDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
}

inline void GLRender::SetDepthFunc(int nFunc)
{
    g_pD3DDevice->SetRenderState(D3DRS_ZFUNC, cmpTranslationTable[nFunc]);
}
// PC 0x0044F880
inline void SetMaterialParameter(int nFace, int nMaterial, float* pfParams)
{
    GLRender::SetMaterialv(nFace, nMaterial, pfParams);
}
// PC 0x0045FD00
inline int AurGetAvailableStencilBits()
{
    return 8;
}
// PC 0x0045FD20
inline int AurGetStencilClearValue()
{
    return (1 << (AurGetAvailableStencilBits() - 1)) - 1;
}
// PC 0x0045FBD0
inline bool AurNV2xLevelShadersAvailable()
{
    return true;
}
// PC 0x0045F7B0
inline int AurSoftShadowsAvailable()
{
    return 0;
}
// PC 0x0045F7E0
inline int AurATISoftShadowsAvailable()
{
    return 0;
}
// PC 0x0045FB10
inline int AurBumpMapAvailable()
{
    return 1;
}

inline unsigned char AurBumpMapCardValidate()
{
    return 1;
}

inline int AurBumpyShinyAvailable()
{
    return 1;
}
// PC 0x0045F560
inline int AurATIFragmentShaderAvailable()
{
    return 0;
}

inline void GLRender::Clear(unsigned long nFlags)
{
    g_pD3DDevice->Clear(0, NULL,
                        ((nFlags & 1) ? D3DCLEAR_TARGET : 0) |
                        ((nFlags & 2) ? D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL : 0) |
                        ((nFlags & 4) ? D3DCLEAR_STENCIL : 0),
                        g_nClearColor, 1.0f, g_nClearStencil);
}

extern int g_nScreenWidth;
extern int g_nScreenHeight;
extern float g_fViewPortScaleX;
extern float g_fViewPortScaleY;
extern List<D3DVIEWPORT8> g_lstViewPorts;
// PC 0x00459170  Xbox 0x002BDC10
inline void GLRender::SetViewPort(int nX, int nY, int nWidth, int nHeight)
{
    if (g_pD3DDevice)
    {
        D3DVIEWPORT8 viewport;
        memset(&viewport, 0, sizeof(viewport));
        viewport.MinZ = 0.0f;
        viewport.MaxZ = 1.0f;
        if (nY + nHeight > g_nScreenHeight)
        {
            g_fViewPortScaleY = (float)nHeight / (float)(g_nScreenHeight - nY);
        }
        else
        {
            g_fViewPortScaleY = 1.0f;
        }
        if (nX + nWidth > g_nScreenWidth)
        {
            g_fViewPortScaleX = (float)nWidth / (float)(g_nScreenWidth - nX);
        }
        else
        {
            g_fViewPortScaleX = 1.0f;
        }
        int nOffsetX = 0;
        int nOffsetY = 0;
        if (nX != 0 || nY != 0)
        {
            nOffsetY = g_lstViewPorts[g_lstViewPorts.m_nCount - 1].Y;
            nOffsetX = g_lstViewPorts[g_lstViewPorts.m_nCount - 1].X;
        }
        int nVisibleHeight;
        if (nOffsetY + nY + nHeight > g_nScreenHeight)
        {
            nVisibleHeight = g_nScreenHeight - nOffsetY - nY;
        }
        else
        {
            nVisibleHeight = nHeight;
        }
        int nVisibleWidth;
        if (nOffsetX + nX + nWidth > g_nScreenWidth)
        {
            nVisibleWidth = g_nScreenWidth - nOffsetX - nX;
        }
        else
        {
            nVisibleWidth = nWidth;
        }
        viewport.X = nOffsetX + nX;
        viewport.Y = g_nScreenHeight - nVisibleHeight - nOffsetY - nY;
        viewport.Width = nVisibleWidth;
        viewport.Height = nVisibleHeight;
        if (nX == 0 && nY == 0)
        {

            g_lstViewPorts.SetSize(nX);
            g_lstViewPorts.Add(viewport);
        }
        else
        {
            g_lstViewPorts.Add(viewport);
        }
        g_pD3DDevice->SetViewport(&viewport);
    }
}

inline void GLRender::GetViewPort(int* pViewport)
{
    D3DVIEWPORT8 viewport;
    g_pD3DDevice->GetViewport(&viewport);
    int nHeight = viewport.Height;
    int nY = g_nScreenHeight - viewport.Y - nHeight;
    pViewport[0] = viewport.X;
    pViewport[1] = nY;
    pViewport[2] = viewport.Width;
    pViewport[3] = nHeight;
}
// Xbox 0x00012150
inline void GLRender::LoadMatrixIdentity()
{
    switch (g_nMatrixMode)
    {
    case 2:
        g_pWorldMatrixStack->LoadIdentity();
        break;
    case 1:
        g_pViewMatrixStack->LoadIdentity();
        if (g_fViewPortScaleX != 1.0f || g_fViewPortScaleY != 1.0f)
        {
            D3DXMATRIX mScale;
            D3DXMatrixScaling(&mScale, g_fViewPortScaleX, g_fViewPortScaleY, 1.0f);
            g_pViewMatrixStack->MultMatrixLocal(&mScale);
        }
        break;
    case 3:
        g_pTextureMatrixStack->LoadIdentity();
        break;
    case 0:
        g_pProjectionMatrixStack->LoadIdentity();
        break;
    }
}
// Xbox 0x000128E0
inline void GLRender::LoadViewIdentity()
{
    g_nMatrixMode = 1;
    LoadMatrixIdentity();
    g_nMatrixMode = 1;
    g_pD3DDevice->SetTransform(D3DTS_VIEW, g_pViewMatrixStack->GetTop());
    g_nMatrixMode = 2;
}
// PC 0x0045F8B0
inline int AurCubeMapAvailable()
{
    return 1;
}

extern int g_bHighResolutionTimer;
extern int g_bTimerScaled;
extern float g_fTimerScale;
inline __int64 AurGetHighResolutionTime()  // Xbox 0x00012270
{
    if (g_bHighResolutionTimer == 1)
    {
        LARGE_INTEGER nCounter;
        QueryPerformanceCounter(&nCounter);
        if (g_bTimerScaled)
        {
            return (__int64)((double)g_fTimerScale * nCounter.QuadPart);
        }
        return (__int64)((double)g_fTimerScale * nCounter.QuadPart);
    }
    return (unsigned __int64)GetTickCount() * 1000;
}
// Xbox 0x00011E30
inline void GLRender::SetClearColor(float fRed, float fGreen, float fBlue, float fAlpha)
{
    g_nClearColor = D3DCOLOR_ARGB((int)(fAlpha * 255.0f), (int)(fRed * 255.0f), (int)(fGreen * 255.0f),
                                  (int)(fBlue * 255.0f));
}

inline void GLRender::ApplyMatrix(int nMode)
{
    g_nMatrixMode = nMode;
    switch (nMode)
    {
    case 1:
        g_pD3DDevice->SetTransform(D3DTS_VIEW, g_pViewMatrixStack->GetTop());
        break;
    case 2:
        g_pD3DDevice->SetTransform(D3DTS_WORLD, g_pWorldMatrixStack->GetTop());
        break;
    case 3:
        if (cm_pCurrentTexture[0])
        {
            g_pD3DDevice->SetTransform(D3DTS_TEXTURE0, g_pTextureMatrixStack->GetTop());
        }
        if (cm_pCurrentTexture[1])
        {
            g_pD3DDevice->SetTransform(D3DTS_TEXTURE1, g_pTextureMatrixStack->GetTop());
        }
        if (cm_pCurrentTexture[2])
        {
            g_pD3DDevice->SetTransform(D3DTS_TEXTURE2, g_pTextureMatrixStack->GetTop());
        }
        if (cm_pCurrentTexture[3])
        {
            g_pD3DDevice->SetTransform(D3DTS_TEXTURE3, g_pTextureMatrixStack->GetTop());
        }
        break;
    }
}
// Xbox 0x002AD8C0
inline void GLRender::BeginImmediatePrimitive(Aurora::AuroraPrimitiveTypes nType)
{
    int nMode = g_nMatrixMode;
    ApplyMatrix(1);
    ApplyMatrix(2);
    SetMatrixMode(nMode);
    g_pD3DDevice->Begin((D3DPRIMITIVETYPE)primTypeTranslationTable[nType]);
}

inline void GLRender::SetTextureCoordGenerationState(
    Aurora::AuroraTexCoordMappingModes nMode)
{

    if (nMode == Aurora::TEXCOORD_MAPPING_REFLECTION)
    {
        g_pD3DDevice->SetTextureStageState(cm_uiCurrentStage, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3);
        g_pD3DDevice->SetTextureStageState(cm_uiCurrentStage, D3DTSS_TEXCOORDINDEX,
            cm_uiCurrentStage | D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR);
        Enable(ENABLE_TEXTURE_GEN_S);
        Enable(ENABLE_TEXTURE_GEN_T);
        Enable(ENABLE_TEXTURE_GEN_R);
    }
    else if (nMode == Aurora::TEXCOORD_MAPPING_SPHERE)
    {
        g_pD3DDevice->SetTextureStageState(cm_uiCurrentStage, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
        g_pD3DDevice->SetTextureStageState(cm_uiCurrentStage, D3DTSS_TEXCOORDINDEX,
            cm_uiCurrentStage | D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR);
        Enable(ENABLE_TEXTURE_GEN_S);
        Enable(ENABLE_TEXTURE_GEN_T);
    }
}
// Xbox 0x002AD950
inline void GLRender::SetPixelShaderAmbientColor(Vector& vColor, float fAlpha)
{
    float fW = fAlpha;
    g_pD3DDevice->SetPixelShaderConstant(0, Homogenize(vColor, fW), 1);
}
// Xbox 0x002AD980
inline void GLRender::SetBumpLightColor(Vector& vColor)
{
    g_pD3DDevice->SetPixelShaderConstant(1, Homogenize(vColor, 1.0f), 1);
}
// PC 0x00425900
inline void GLRender::SetVertexBuffer(unsigned long nStride, unsigned long nOffset, unsigned long nPool,
                                      int nUsage)
{
    ValidatePool(nPool);
    g_pD3DDevice->SetStreamSource(0, (D3DVertexBuffer*)GetPool(nPool), nStride);
}
// PC 0x004259E0
inline void GLRender::SetNormalBuffer(unsigned int nStride, unsigned int nOffset, unsigned int nPool, int nUsage)
{
    ValidatePool(nPool);
    g_pD3DDevice->SetStreamSource(1, (D3DVertexBuffer*)GetPool(nPool), nStride);
}
// PC 0x00425970
inline void GLRender::SetColorBuffer(unsigned long nStride, unsigned long nOffset, unsigned long nPool,
                                     int nUsage)
{
    ValidatePool(nPool);
    g_pD3DDevice->SetStreamSource(2, (D3DVertexBuffer*)GetPool(nPool), nStride);
}
// PC 0x00425A40
inline void GLRender::SetTexCoordBuffer(unsigned long nStride, unsigned long nOffset, unsigned long nPool,
                                        int nUsage)
{
    ValidatePool(nPool);
    g_pD3DDevice->SetStreamSource(cm_uiCurrentStage + 3, (D3DVertexBuffer*)GetPool(nPool), nStride);
}

#define GLRender_Enable GLRender::Enable
#define GLRender_Disable GLRender::Disable
#define GLRender_IsEnabled GLRender::IsEnabled

#define GLRender_SetBlendFunc(nSource, nDestination)     GLRender::SetBlendFunc((Aurora::AuroraBlendModes)(nSource), (Aurora::AuroraBlendModes)(nDestination))
#define GLRender_EnableClientState GLRender::EnableClientState
#define GLRender_DisableClientState GLRender::DisableClientState
#define GLRender_PushCurrentMatrix GLRender::PushCurrentMatrix
#define GLRender_PopCurrentMatrix GLRender::PopCurrentMatrix
#define GLRender_EnablePixelShaders GLRender::EnablePixelShaders
#define GLRender_SetPixelShaderAmbientColor GLRender::SetPixelShaderAmbientColor
#define GLRender_SetBumpLightColor(vColor, fAlpha) GLRender::SetBumpLightColor(vColor)
#define GLRender_SetDepthMask GLRender::SetDepthMask
#define GLRender_SetFrontFace GLRender::SetFrontFace

#define GLRender_QueryIntegerv(nName, pnValue) g_pD3DDevice->GetRenderState((D3DRENDERSTATETYPE)(nName), (DWORD*)(pnValue))
#define GLRender_BeginImmediatePrimitive GLRender::BeginImmediatePrimitive
#define GLRender_EndImmediatePrimitive GLRender::EndImmediatePrimitive

#define GLRender_SetMatrixMode(nMode) GLRender::SetMatrixMode(nMode)
#define GLRender_LoadMatrixIdentity() GLRender::LoadMatrixIdentity()
#define GLRender_SetPolygonRenderMode(nFace, nFill) GLRender::SetPolygonRenderMode(nFace, nFill)
#define GLRender_GetViewPort(pViewport) GLRender::GetViewPort(pViewport)
#define GLRender_SetPerspectiveTransform GLRender::SetPerspectiveTransform
#define GLRender_SetOrthoTransform GLRender::SetOrthoTransform
#define GLRender_SetViewPort GLRender::SetViewPort

#define GLRender_DeleteTextures GLRender::DeleteTextures
#define GLRender_GenTextures GLRender::GenTextures
#define GLRender_SetClearColor GLRender::SetClearColor
#define GLRender_Rotate GLRender::Rotate

#define GLRender_SetFogModef GLRender::SetFogModef

#define GLRender_SetColor4f GLRender::SetColor4f
#define GLRender_SetAlphaFunc GLRender::SetAlphaFunc
#define GLRender_SetStencilOp GLRender::SetStencilOp
#define GLRender_SetStencilMask GLRender::SetStencilMask
#define GLRender_SetStencilFunc GLRender::SetStencilFunc
#define GLRender_SetColorMask GLRender::SetColorMask
#define GLRender_ClearStencilValue GLRender::ClearStencilValue
#define GLRender_Clear GLRender::Clear
#define GLRender_EnableColorMaterial GLRender::EnableColorMaterial
#define GLRender_DisableColorMaterial GLRender::DisableColorMaterial
#define GLRender_SetColorMaterial GLRender::SetColorMaterial
#define GLRender_SetMaterialv GLRender::SetMaterialv
#define GLRender_SetLightModel GLRender::SetLightModel
#define GLRender_PushAttrib GLRender::PushAttrib
#define GLRender_PopAttrib GLRender::PopAttrib
#define GLRender_SetLightProperties GLRender::SetLightProperties
#define GLRender_EnableLight GLRender::EnableLight
#define GLRender_DisableLight GLRender::DisableLight
#define GLRender_SetFogMode GLRender::SetFogMode
#define GLRender_SetFogModev GLRender::SetFogModev
#define GLRender_SetDepthFunc GLRender::SetDepthFunc
#define GLRender_SetColorv GLRender::SetColorv
#define GLRender_SetVertexProgramParameter4f AurSetVertexProgramParameter4f
#define GLRender_Translate GLRender::Translate
#define GLRender_MultMatrix GLRender::MultMatrix

#endif
