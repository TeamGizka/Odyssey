#ifndef __AURORA_H__
#define __AURORA_H__

#include "exoresman.h"
#include <aurpointer.h>

class CAurShadowData
{
public:
    int m_nEdges;
};

#define __CAUROBJECT_DEFINED__
class Vector;
class Quaternion;
class CAurCamera;
class CAurScene;
class CAurPart;
class CAurBehavior;
class OOAnimRun;
class CallBack;
class CHitInfo;
class Part;

class CAurObject : public SafePointerList<CAurObject>
{
public:
    CAurObject();
    virtual ~CAurObject();
    virtual CAurCamera* AsCAurCamera() = 0;
    virtual char* GetName() = 0;
    virtual char* GetModelName() = 0;
    virtual void Render(bool bRenderLast) = 0;
    virtual void Animate(float fTimeStep) = 0;
    virtual int PlayAnimation(char* sName, float fSpeed, int nFlags,
                              float fBlend) = 0;
    virtual OOAnimRun* GetAnimation(char* sName, float* pfTime, float* pfLength) = 0;
    virtual int LoadAddInAnimations(char* sName) = 0;
    virtual void RemoveAddInAnimations() = 0;
    virtual CallBack* RegisterCallBack(void (*pCallBack)(CAurObject*, char*, void*),
                                 char* sEvent, void* pData, float fDelay,
                                 bool bRepeat) = 0;
    virtual void DeregisterCallback(CallBack* pCallBack) = 0;
    virtual int ReplaceTexture(char* sOld, char* sNew) = 0;
    virtual int ReplaceTexture(char* sOld, char* sNew, int nCount,
                                unsigned short* pParts, int nStart) = 0;
    virtual int ReplaceTextureSubtree(char* sPart, char* sTexture, int nNumTXI,
                                      unsigned short* pTXI) = 0;
    virtual int RestoreTexture() = 0;
    virtual int SetCanDownSample(char bCanDownSample) = 0;
    virtual int SetTextureAttributes(char* sTexture, char* sAttributes) = 0;
    virtual int SetEnvironmentMap(char* sName) = 0;
    virtual int AttachToScene(CAurScene* pScene) = 0;
    virtual CAurBehavior* AttachToObject(CAurObject* pObject, char* sPart, int nFlags) = 0;
    virtual CAurBehavior* AttachToPart(CAurObject* pObject, CAurPart* pPart, int nFlags) = 0;
    virtual void SetBehavior(CAurBehavior* pBehavior) = 0;
    virtual Vector SetPosition(Vector vPosition) = 0;
    virtual Quaternion SetOrientation(Quaternion qOrientation) = 0;
    virtual Vector GetPosition() = 0;
    virtual Quaternion GetOrientation() = 0;
    virtual Vector GetPreviousPosition() = 0;
    virtual void* GetExternalData() = 0;
    virtual void SetExternalData(void* pData) = 0;
    virtual int GetExternalDataType() = 0;
    virtual void SetExternalDataType(int nType) = 0;
    virtual void InheritExternal(CAurObject* pSource) = 0;
    virtual CAurObject* HitCheckHack(Vector vStart, Vector vEnd, Vector* pHit) = 0;
    virtual CAurObject* HitCheckPart(CAurPart* pPart, Vector vStart, Vector vEnd,
                             Vector* pHit, CHitInfo* pInfo) = 0;
    virtual CAurObject* HitCheck(Vector vStart, Vector vEnd, Vector* pHit,
                                CHitInfo* pInfo) = 0;
    virtual void SetIgnoreHitCheck(bool bIgnore) = 0;
    virtual int SetPartPosition(char* sPart, Vector vPosition,
                                 Quaternion qOrientation) = 0;
    virtual int GetPartPosition(char* sPart, Vector* pPosition,
                                Quaternion* pOrientation) = 0;
    virtual int SetPartLocalPosition(char* sPart, Vector vPosition,
                                     Quaternion qOrientation) = 0;
    virtual int GetPartLocalPosition(char* sPart, Vector* pPosition,
                                     Quaternion* pOrientation) = 0;
    virtual void ReattachReference(CAurObject* pObject, char* sPart) = 0;
    virtual void BehaviorMessage(char* sMessage) = 0;
    virtual int Append(char* sModel, char* sPart) = 0;
    virtual int Amputate(char* sPart) = 0;
    virtual void SetColorShifting(Vector vColor, float fAlpha, int nFlags) = 0;
    virtual void SetIllumination(Vector vColor, int nFlags) = 0;
    virtual void TurnOffShadows() = 0;
    virtual void TurnOnShadows() = 0;
    virtual void SetLightPriority(int nPriority) = 0;
    virtual float GetMaximumLightRadius() = 0;
    virtual bool SetObjectScale(float fScale, bool bBaseScale) = 0;
    virtual void EnableRenderGobBBox(Vector const& vBoundingBox) = 0;
    virtual void DisableRenderGobBBox() = 0;
    virtual void SetWind(bool bWind, float fWindY, float fWindX, float fWindZ) = 0;
    virtual void EnableForceAnimations() = 0;
    virtual void DisableForceAnimations() = 0;
    virtual void DisableFog() = 0;
    virtual void EnableFog() = 0;
    virtual void GetBoundingBox(Vector& vMinimum, Vector& vMaximum) = 0;
    virtual void GetMinimumBoundingBox(Vector& vMinimum, Vector& vMaximum) = 0;
    virtual void GetMinimumBoundingSphere(Vector& vPosition, float& fRadius) = 0;
    virtual void EnableDistortion() = 0;
    virtual void DisableDistortion() = 0;
    virtual void SetAsCharacter() = 0;
    virtual CAurScene* GetScene() = 0;
    virtual CAurBehavior* GetBehavior(int nIndex) = 0;
    virtual Part* GetPart(char* sName) = 0;
    virtual void EnumParts(int (*pFilter)(CAurPart*), List<CAurPart*>* pParts) = 0;
    virtual void SetRenderPersonalSpace(bool bRender) = 0;
    virtual void BeginLookAtAnimate(CAurObject* pTarget, char* sTargetPart,
                                    char* sHeadPart, float fHorizontalLimit,
                                    float fVerticalLimit, float fDuration,
                                    float fMaxDelta) = 0;
    virtual void EndLookAtAnimate() = 0;
    virtual void PlayOutOfOrderAnimation(char* sName, float* pfWeights,
                                         float* pfTimes, int nCount,
                                         float fLength) = 0;
    virtual int StopOutOfOrderAnimation(char* sName) = 0;
    virtual void AddAnimationFlag(int nFlag) = 0;
    virtual void RemoveAnimationFlag(int nFlag) = 0;
    virtual void EnableAlwaysRender() = 0;
    virtual void DisableAlwaysRender() = 0;
    virtual void EnableInCutscene() = 0;
    virtual void DisableInCutscene() = 0;
    virtual Vector GetCutsceneDummyPosition() = 0;
    virtual void SetMotionBlurred(bool bBlurred) = 0;
    virtual bool GetMotionBlurred() = 0;
    virtual void RenderBlur() = 0;
    virtual void EnableRenderBumpedOut(char* sTexture, float fHeight) = 0;
    virtual void DisableRenderBumpedOut() = 0;
    virtual void SetMaxBlurLength(float fLength) = 0;
    virtual float GetMaxBlurLength() = 0;
    virtual void SetForcePoints(short nForcePoints)  // PC 0x005EE1C0
    {
    }
    virtual short GetForcePoints()  // PC 0x0045CCE0
    {
        return 0;
    }
    virtual void SetAlignmentValue(short nAlignmentValue)  // PC 0x005EE1C0
    {
    }
    virtual short GetAlignmentValue()  // PC 0x0045CCE0
    {
        return 0;
    }
    virtual void SetForceGlowColor(float fRed, float fGreen, float fBlue,
                                   float fAlpha)  // PC 0x00693D50
    {
    }
    virtual void GetForceGlowColor(float& fRed, float& fGreen, float& fBlue,
                                   float& fAlpha)  // PC 0x0045CCF0
    {
        fRed = fGreen = fBlue = fAlpha = 0.0f;
    }
    virtual void EnableForceGlow(bool bEnable)  // PC 0x005EE1C0
    {
    }
    virtual bool IsForceGlowEnabled()  // PC 0x00420990
    {
        return false;
    }
    virtual void SetNonForceBlending(bool bNonForceBlending)  // PC 0x005EE1C0
    {
    }
    virtual bool IsNonForceBlending()  // PC 0x00420990
    {
        return false;
    }
    virtual void SetEmitterAsWeather(bool bWeather) = 0;
    virtual void EnableHologram(bool bEnable) = 0;
    virtual bool IsHologramEnabled() = 0;
    virtual void EnableIllumination(bool bEnable, float fRed, float fGreen,
                                    float fBlue) = 0;
    virtual bool IsIlluminationEnable() = 0;
    virtual void GetIlluminationColor(float& fRed, float& fGreen, float& fBlue) = 0;
    virtual void EnableRendering(bool bEnable) = 0;
    virtual bool IsRenderingEnabled() = 0;
    void AnimateExternal(float);

};

class Vector;

int AurMakeCurrent();

enum { AUR_DRAWABLE_COLOR_WHITE = 0xFFFFFFFF };
int AurAddDrawableSquare(float* pVertices, unsigned long nColor,
                         const char* sFile, int nLine);
#define AurAddDrawableSquare(pVertices, nColor) \
    AurAddDrawableSquare(pVertices, nColor, __FILE__, __LINE__)
void AurRemoveDrawableSquare(unsigned long nSquare);
void AurSnapshot(char* sFileName, bool bScreenSize);
int AurQueryMaximumAnisotropy();
void AurEnableShadows();
int AurDisableShadows();
int AurEnableSoftShadows();
void AurDisableSoftShadows();
int SetAnisotropicFilter(int nLevel);
void AurEnableFrameBufferEffects();
void AurDisableFrameBufferEffects();
void AurATIUnlockSoftShadowsForHotFix();
int AurQueryAntiAliasingMode(int nMode);
#ifdef _XBOX
void ReInitAurora(unsigned long nWidth, unsigned long nHeight,
                  unsigned char nBitsPerPixel, int nFlags);
#else
void ReInitAurora(unsigned long nWidth, unsigned long nHeight,
                  unsigned char nBitsPerPixel, int nFlags, int bReinit);
#endif
void AurDisableVertexBufferObjects();
void AurDisableVertexBufferObjectsWriteOnly();
void AurEnableVertexBufferObjectsWriteOnly();
bool AurCheckFrameBufferModificationsAvailable();
bool AurCheckSoftShadowsAvailable();
void AurSetSpeedBlurRatio(float fRatio);
void AurEnableSpeedBlur();
void AurDisableSpeedBlur();
void AurDisableSaturation();
void AurDisableScanNoise();
void AurEnableScanNoise();
void AurEnableSaturation();
void AurSetSaturationParams(Vector vModulation, float fSaturation);
void AurEnableForceSight();
void AurDisableForceSight();
bool AurIsSpeedBlurEnabled();
void AurEnableClairvoyance(bool bLetterbox);
void AurDisableClairvoyance();
void AurEnableForceFury(short nLevel);
void AurSetClearColor(float fRed, float fGreen, float fBlue, float fAlpha);
void AurGetClearColor(float* pColor);

extern unsigned char g_nBitsPerPixel;
extern int g_nDesiredFullScreenRefreshRate;

class CAuroraModel;

#pragma pack(push, 1)

struct SMDLDataTrailer
{
    unsigned short nType;
    CAuroraModel* pOwner;
};

#pragma pack(pop)

class CAuroraModel : public CResHelper<CResMDL, RESTYPE_MDL>
{
public:
    CAuroraModel(CResRef cResRef, int bRequest);
    virtual ~CAuroraModel();

    int Load();
    void Unload();
    int GetModelDataSize();
    unsigned char* GetModelDataPtr();
    unsigned char* GetPtrToResHelperInfo();
};

class CAurora16BitTexture;

#pragma pack(push, 1)

struct S16BitTextureDataTrailer
{
    unsigned short nType;
    CAurora16BitTexture* pOwner;
};

#pragma pack(pop)

struct S16BitTextureHeader
{
    int nWidth;
    int nHeight;
    float fAlpha;
    int nFormat;
};

class CAurora16BitTexture : public CResHelper<CRes4PC, RESTYPE_4PC>
{
public:
    CAurora16BitTexture(CResRef cResRef, int bRequest);
    virtual ~CAurora16BitTexture();

    int GetTextureAttrib(unsigned long* pnA, unsigned long* pnB, float* pfC,
                         int* pnD);
    unsigned char* GetTextureDataPtr();
    int Load();
    void Unload();
    int ReadTextureHeader(int* pnA, int* pnB, float* pfC, int* pnD);
    unsigned char* GetPtrToResHelperInfo();
};

class CAuroraCompressedTexture;

#pragma pack(push, 1)

struct SCompressedTextureDataTrailer
{
    unsigned short nType;
    CAuroraCompressedTexture* pOwner;
};

#pragma pack(pop)

class CAuroraCompressedTexture : public CResHelper<CResDDS, RESTYPE_DDS>
{
public:
    CAuroraCompressedTexture(CResRef cResRef, int bRequest);
    virtual ~CAuroraCompressedTexture();
    int GetCompressedTextureAttrib(unsigned long*, unsigned long*,
                                   unsigned char*, unsigned long*, float*);
    int Load();
    void Unload();
    int ReadTextureHeader(int*, int*, int*, int*, float*);
    unsigned char* GetPtrToResHelperInfo();
    unsigned char* GetCompressedTextureDataPtr();
};

class CAuroraLayeredTexture;

#pragma pack(push, 1)

struct SPLTLayeredTrailer
{
    unsigned short nType;
    CAuroraLayeredTexture* pOwner;
};

#pragma pack(pop)

class CAuroraLayeredTexture : public CResHelper<CResPLT, RESTYPE_PLT>
{
public:
    CAuroraLayeredTexture(CResRef cResRef, int bRequest);
    virtual ~CAuroraLayeredTexture();

    int GetTextureAttrib(unsigned long* pnA, unsigned long* pnB,
                         unsigned long* pnC);
    int Load();
    void Unload();
    unsigned char* GetTextureDataPtr();
    unsigned char* GetPtrToResHelperInfo();
};

class CAuroraModelX;

#pragma pack(push, 1)

struct SMDXResPrefix
{
    unsigned short nType;
    CAuroraModelX* pOwner;
};

#pragma pack(pop)

class CAuroraModelX : public CResHelper<CResMDX, RESTYPE_MDX>
{
public:
    CAuroraModelX(CResRef cResRef, int bRequest);
    virtual ~CAuroraModelX();

    int Load();
    void Unload();
    int GetModelDataSize();
    unsigned char* GetModelDataPtr();
    unsigned char* GetPtrToResHelperInfo();
};

class CAuroraProcessedTexture;

#pragma pack(push, 1)

struct SProcessedTextureDataTrailer
{
    unsigned short nType;
    CAuroraProcessedTexture* pOwner;
};

#pragma pack(pop)

class CAuroraProcessedTexture : public CResHelper<CResTPC, RESTYPE_TPC>
{
public:
    CAuroraProcessedTexture(CResRef cResRef, int bRequest);
    virtual ~CAuroraProcessedTexture();

    int GetProcessedTextureAttrib(int* pnWidth, int* pnHeight, char* pnEncoding,
                                  int* pnCompressedSize, int* pnDataSize,
                                  int* pnTXISize, float* pfAlphaBlending,
                                  char* pnMipMapCount);
    unsigned char* GetProcessedTextureDataPtr();
    int GetProcessedTextureDataSize();
    unsigned char* GetProcessedTextureTXIPtr();
    int GetProcessedTextureTXISize();
    int Load();
    int ReadProcessedTextureHeader(int* pnWidth, int* pnHeight,
                                   char* pnEncoding, int* pnCompressedSize,
                                   int* pnDataSize, float* pfAlphaBlending,
                                   char* pnMipMapCount);
    int GetTextureSize();
    unsigned char* GetPtrToResHelperInfo();
    void Unload();
};

class CAuroraTexture;

#pragma pack(push, 1)

struct STGATextureTrailer
{
    unsigned short nType;
    CAuroraTexture* pOwner;
};

#pragma pack(pop)

class CAuroraTexture : public CResHelper<CResTGA, RESTYPE_TGA>
{
public:
    CAuroraTexture(CResRef cResRef, int bRequest);
    virtual ~CAuroraTexture();

    int GetTextureAttrib(unsigned long* pnWidth, unsigned long* pnHeight,
                         unsigned char* pnDepth);
    int Load();
    void Unload();
    int ReadTextureHeader(int* pnWidth, int* pnHeight, int* pnDepth);
    unsigned char* GetPtrToResHelperInfo();
    int GetTextureDataSize();
    unsigned char* GetTextureDataPtr();
};

class CAuroraTXI;

#pragma pack(push, 1)

struct STXIResourcePrefix
{
    unsigned short nType;
    CAuroraTXI* pOwner;
};

#pragma pack(pop)

class CAuroraTXI : public CResHelper<CResTXI, RESTYPE_TXI>
{
public:
    CAuroraTXI(CResRef cResRef, int bRequest);
    virtual ~CAuroraTXI();

    int Load();
    void Unload();
    int GetTXIDataSize();
    unsigned char* GetTXIDataPtr();
    unsigned char* GetPtrToResHelperInfo();
};

class CAuroraVisibility;

#pragma pack(push, 1)

struct SVISDataTrailer
{
    unsigned short nType;
    CAuroraVisibility* pOwner;
};

#pragma pack(pop)

class CAuroraVisibility : public CResHelper<CResVIS, RESTYPE_VIS>
{
public:
    CAuroraVisibility(CResRef cResRef, int bRequest);
    virtual ~CAuroraVisibility();

    int Load();
    void Unload();
    int GetVisibilityDataSize();
    unsigned char* GetVisibilityDataPtr();
    unsigned char* GetPtrToResHelperInfo();
};

class CAurora16BitTexture;
class CAuroraCompressedTexture;
class CAuroraLayeredTexture;
class CAuroraModel;
class CAuroraModelX;
class CAuroraProcessedTexture;
class CAuroraTXI;
class CAuroraTexture;
class CAuroraVisibility;
class CExoLinkedListInternal;
class CExoLinkedListNode;
class CResRef;

extern "C" void* (*AurGetResource)(char*, char*, int*);
extern "C" void (*AurReleaseResource)(void*, int);

class CAuroraFontTexture;

class CAuroraInterface
{
public:

    enum TexturePackSlot
    {
        TEXTURE_PACK_TEXTURE = 1,
        TEXTURE_PACK_GUI = 2
    };

    enum VideoRefreshRate
    {
        VIDEO_REFRESH_DEFAULT = 0,
        VIDEO_REFRESH_DEFAULT_ALTERNATE = 1
    };

    enum VideoModeLimits
    {
        VIDEO_MIN_WIDTH = 640,
        VIDEO_MIN_HEIGHT = 480,
        VIDEO_MIN_BITS_PER_PIXEL = 16,
        VIDEO_MIN_REFRESH_RATE = 60
    };
    char gap_0[1];
    static unsigned char* GetLayeredRaster(char*, int*, int*, int*);
    static unsigned char* GetRaster(char*, int*, int*, int*);
    static void ReleaseLayeredRaster(void*, int, int);
    static void ReleaseRaster(void*, int, int, int, int, int, int, bool);
    static void* GetResource(char*, char*, int*);
    static void ReleaseResource(void*, int);
    static int ReadRasterHeader(char*, int*, int*, int*);
    static int ReadProcessedRasterHeader(char*, int*, int*, char*, int*, int*,
                                         float*, char*);
    static int ReadCompressedRasterHeader(char*, int*, int*, int*, int*,
                                          float*);
    static int Get16BitRasterHeader(char*, int*, int*, float*, int*);
    static int LoadTexturePack(unsigned char, CExoString const&);
    static int UnloadTexturePack(unsigned char);
    static CExoString GetCurrentTexturePack(unsigned char);
    static int GetNearestVideoMode(unsigned long, unsigned long, unsigned char,
                                   unsigned long);
    static int GetExactVideoMode(unsigned long, unsigned long, unsigned char,
                                 unsigned long);
    ~CAuroraInterface();
    static unsigned char* GetCompressedRaster(char*, int*, int*, int*, int*, float*);
    static unsigned char* GetProcessedRaster(char*, int*, int*, char*, int*, int*, int*,
                                  float*, char*);

    static int GetHighestPriorityTextureFormat(CResRef resRef, int bDDS,
                                               int b4PC, int bTGA,
                                               int bTPC);

    static unsigned char* Get16BitRaster(char*, int*, int*, float*, int*);
    static int SetTexturePack(unsigned char, CExoString const&);
    static void RequestModel(char*);
    static int IsResident(char*);
    CAuroraInterface();
    static CAuroraModel* GetModelResHelper(CExoString const&, int, int*,
                                           CExoLinkedListNode**);
    static CAuroraModel* GetModelInternal(CExoString const&, int, int*,
                                          CExoLinkedListNode**);
    static void ReleaseModelInternal(CAuroraModel*);
    static CAuroraTexture* GetTextureInternal(CExoString const&);
    static CAurora16BitTexture* Get16BitTextureInternal(CExoString const&);

    static CAuroraFontTexture* GetFontTextureInternal(CExoString const&);
    static CAuroraLayeredTexture* GetLayeredTextureInternal(CExoString const&);
    static CAuroraCompressedTexture* GetCompressedTextureInternal(
        CExoString const&);
    static CAuroraProcessedTexture* GetProcessedTextureInternal(
        CExoString const&);
    static CAuroraTXI* GetTXIInternal(CExoString const&);
    static CAuroraVisibility* GetVisibilityInternal(CExoString const&);
    static CAuroraModelX* GetModelXInternal(CExoString const&);
    static void ReleaseTextureInternal(CAuroraTexture*);
    static void Release16BitTextureInternal(CAurora16BitTexture*);
    static void ReleaseFontTextureInternal(CAuroraFontTexture*);
    static void ReleaseLayeredTextureInternal(CAuroraLayeredTexture*);
    static void ReleaseCompressedTextureInternal(CAuroraCompressedTexture*);
    static void ReleaseProcessedTextureInternal(CAuroraProcessedTexture*);
    static void ReleaseTXIInternal(CAuroraTXI*);
    static void ReleaseVisibilityInternal(CAuroraVisibility*);

    static void ReleaseModelXInternal(CAuroraModelX*);
    static CAuroraTexture* GetTexture(CExoString const&);
    static CAurora16BitTexture* Get16BitTexture(CExoString const&);
    static CAuroraLayeredTexture* GetLayeredTexture(CExoString const&);
    static CAuroraCompressedTexture* GetCompressedTexture(CExoString const&);
    static CAuroraProcessedTexture* GetProcessedTexture(CExoString const&);
    static CAuroraTXI* GetTXI(CExoString const&);
    static CAuroraVisibility* GetVisibility(CExoString const&);
    static CAuroraModelX* GetModelX(CExoString const&);
    static int ReleaseTexture(CAuroraTexture*);
    static int Release16BitTexture(CAurora16BitTexture*);
    static int ReleaseLayeredTexture(CAuroraLayeredTexture*);
    static int ReleaseCompressedTexture(CAuroraCompressedTexture*);
    static int ReleaseProcessedTexture(CAuroraProcessedTexture*);
    static int ReleaseTXI(CAuroraTXI*);
    static int ReleaseVisibility(CAuroraVisibility*);
    static int ReleaseModelX(CAuroraModelX*);
    static CExoLinkedList<CAuroraModel> m_lstRequestedModel;
    static CExoString m_pCurrentTexturePack[4];
};

class PostedString
{
public:
    char m_szString[1024];
    int m_nLength;
    float m_fTimeRemaining;
    float m_fDuration;
    float m_fStartRed;
    float m_fStartGreen;
    float m_fStartBlue;
    float m_fStartAlpha;
    float m_fEndRed;
    float m_fEndGreen;
    float m_fEndBlue;
    float m_fEndAlpha;
    int m_nColumn;
    int m_nRow;

    void Post();
    PostedString(char* szString, int nColumn, int nRow, float fDuration);
#ifdef _XBOX
    ~PostedString();
#endif
};

void AurPrintString(char* szString, int nColumn, int nRow);

extern CAuroraInterface* g_pAuroraInterface;
void AurInitFont();
void AurFreeFont();
void AurDeleteAllFonts();
void PrintStringScreenPos_UseTextureFont(char* szString, float fX, float fY);
void AurStringsFree();
void AurPostString(char* szString, int nColumn, int nRow, float fDuration);
void AurPrintf(const char* szFormat, ...);
char* untrace(char* szPrefix);
void PostTraceStrings();
void AurRenderStrings();

extern CAuroraInterface* g_pAuroraInterface;

void DestroyCAurora();

class Model;
extern int RyanTotalLevelVertices;
extern int RyanTotalLevelVertSize;
extern bool g_bKeepHistory;
extern int g_bEnableAurPostString;
void AurGrassClearInfo();
extern float DeltaT;
char* firstword(char* sMessage);
extern Model* currentmodel;
extern float projectionalpha;
extern "C" int AurSetAntiAliasingMode(int nMode);

int AurSetAnisotropy(int nAnisotropy);
const char* AurGetVersionString();
void AurSetPauseOverlayColor(float fRed, float fGreen, float fBlue, float fAlpha);
extern float memoryfree;
void AurGetGLError();
extern bool g_bVertexBufferObjectsWriteOnly;
extern bool g_bDisableVertexBufferObjects;
extern char disablepostprocessing;
extern int hitcheck_static_bsp_parts;
extern int hitcheck_static_trimesh_parts;
extern "C" unsigned char* (*AurGetRaster)(char*, int*, int*, int*);
extern "C" unsigned char* (*AurGetS3TCImage)(char*, int*, int*, int*, int*, float*);
extern "C" unsigned char* (*AurGet16BitRaster)(char*, int*, int*, float*, int*);
extern "C" unsigned char* (*AurGetProcessedRaster)(char*, int*, int*, char*, int*, int*, int*, float*, char*);
extern "C" void (*AurReleaseRaster)(void* pImage, int nWidth, int nHeight, int nBPP, int nSize, int nReserved60, int nTxiSize, bool bProcessed);
extern "C" void (*AurReleaseLayeredRaster)(void*, int, int);
extern "C" unsigned char* (*AurGetLayeredRaster)(char* sFileName, int* pWidth, int* pHeight, int* pReserved);
extern "C" int (*AurGetRasterHeader)(char*, int*, int*, int*);
extern "C" int (*AurGetS3TCHeader)(char*, int*, int*, int*, int*, float*);
extern "C" int (*AurGet16BitRasterHeader)(char*, int*, int*, float*, int*);
extern "C" int (*AurGetProcessedRasterHeader)(char*, int*, int*, char*, int*, int*, float*, char*);
#ifdef _XBOX
int AurTextureSetMem(int);

void AurSnapshot(char* sFileName);
#else
bool AurTextureSetMem(int);
#endif
void AurClearPalette();

#endif
