#ifndef AURTEXMAN_H
#define AURTEXMAN_H

#include "math3d.h"
#include <aurlist.h>
#include <aurpointer.h>
#include "aurora.h"

namespace Aurora
{
enum AuroraTextureFormats
{
    TEXTURE_FORMAT_RGBA = 0,
    TEXTURE_FORMAT_RGB = 1,
    TEXTURE_FORMAT_LUMINANCE_ALPHA = 2,
    TEXTURE_FORMAT_LUMINANCE = 3,
    TEXTURE_FORMAT_DXT1 = 7,
    TEXTURE_FORMAT_DXT5 = 8
};
}

class CAurTextureBasic;
class CAurTexture;
int AurTextureSetAttributes(CAurTexture* pTexture, char* sField);

enum AuroraTextureClampFlags
{
    TEXTURE_CLAMP_S = 1,
    TEXTURE_CLAMP_T = 2
};

typedef struct
{
    unsigned char nz, ny, nx;
    unsigned char mag;
} Normal;
Normal* convertHeightFieldToNormalMap(unsigned char* pHeightField,
    int nWidth, int nHeight, int nRowWidth, int nRowCount, float fScale);
Normal* downSampleNormalMap(Normal* pSrc,
    int nWidth, int nHeight, int nDestWidth, int nDestHeight);

int BuildTextureNormalMap(unsigned char*, float, int&, int&, int, int, bool);
void AurTextureSetModeStatic();
void AurTextureSetModeDynamic();
int AurTextureSetDim(int nWidth, int nHeight);
int AurTextureMakeCurrent();
extern int usedtexturememory;
void AurTextureSuspendUpdates();
void AurTextureResumeUpdates();

void AurTextureEnableLowEndEnvMapping(Vector vColor);
void AurTextureDisableLowEndEnvMapping();
void AurTextureForceControllerUpdate();

CAurTexture* AurTextureGet(char* sName, CAurTexture* pReference, char* sExtra,
                           int* pTXI, int nNumTXI);
char* AurTextureFind(char* sName, char* sExtra, int* pTXI, int nNumTXI);
void AurTextureBindTID(unsigned int nTextureID, bool bCubeMap, bool b3D);
short AurTextureCalculateHashID(char* sName, short nNumTXI, int* pTXI);
void InsertionSortTexture(CAurTextureBasic** ppTextures, int nCount);

void AurSaveGameSnapshot(char* sFileName, int nSize, bool bSquare);
class TextureController;

void AurTextureClearAll();

void AurTextureFreeAll();

int AurTextureResetAll();
void AurTextureSetDynamicMemoryRatio(float fRatio);
#ifdef _XBOX
int AurTextureSetMem(int nMemory);
#else
bool AurTextureSetMem(int nMemory);
#endif
List<int>* AurTextureGetOrdering();

int AurTextureGetMaxTexID();
bool AurTextureEnvMapDisabled();
void AurTextureRefreshAll();
int AurTextureReplaceTileReferences(CAurScene* pScene, const Vector& vPos, char* sOldName, char* sNewName);

struct CAurFontInfo
{
    CAurFontInfo();
    ~CAurFontInfo();
    void ParseField(char* sField);

    int m_nNumChars;
    float m_fHeight;
    float m_fBaselineHeight;
    float m_fTextureWidth;
    float m_fSpacingR;
    float m_fLineSpacing;
    List<Vector> m_lstUpperLeft;
    List<Vector> m_lstLowerRight;
};

struct TEXTUREMEMORYSTATS
{
    int m_nTotal;
    int m_nInterface;
    int m_nStatic;
    int m_nCubeMap;
    int m_nBumpMap;
};

void AurTextureMemCheck(List<CAurTextureBasic*>& lstTextures, int nBudget,
                        TEXTUREMEMORYSTATS* pStats, float fRatio);
void AurTextureBuildAndStoreAll();
void AurTextureCalculateNewTexMem(List<CAurTextureBasic*>& lstTextures,
                                  TEXTUREMEMORYSTATS* pStats);

unsigned char* AurGetImageWrapper(char* sFileName, int* pWidth, int* pHeight, int* pBPP,
                                  int* pCompressedSize, int* pReserved6, int* pReserved7,
                                  Aurora::AuroraTextureFormats* pFormat, float* pAlpha,
                                  bool* pProcessed, short* pNumMipMaps,
                                  int bAllowCompression);
int AurTextureGetAvailableTextureMemory();

class CAurTexture : public SafePointerList<CAurTexture>
{
public:
    CAurTexture();
    virtual ~CAurTexture();
    virtual CAurTextureBasic* AsCAurTextureBasic()  // PC 0x004065A0 GOG 0x00406570
    {
        return 0;
    }
    virtual CAurTexture* GetReferencedTexture();
    virtual bool IsCubeMap();
    virtual unsigned char IsInvalid();
    virtual bool IsDiffuseBumpMap();
    virtual bool IsSpecularBumpMap();
    virtual bool IsBumpMap();
    virtual bool IsNormalMap();
    virtual unsigned char IsUsingGlobalAlpha();
    virtual unsigned char IsFullyLoaded();
    virtual bool IsEnvironmentMapped();
    virtual unsigned char IsProcessed();
    virtual unsigned char IsTxiLoaded();
    virtual CAurFontInfo* GetFontInfo();
    virtual unsigned char* GetImage();
    virtual char* GetName();
    virtual int GetFileRange();
    virtual int GetWidth();
    virtual int GetHeight();
    virtual int GetDepth();
    virtual int GetBPP();
    virtual int GetNumMipMaps();
    virtual int GetNumX();
    virtual int GetNumY();
    virtual int GetClamp();
    virtual int GetCompressedSize();
    virtual float GetEnvMapAlpha();
    virtual unsigned char GetMipMap();
    virtual unsigned char GetFilter();
    virtual unsigned char GetMapTexelsToPixels();
    virtual float GetBumpMapScaling();
    virtual float GetBumpIntensity();
    virtual float GetSpecularBumpIntensity();
    virtual float GetDiffuseBumpIntensity();
    virtual Vector GetSpecularColor();
    virtual int GetCompressionFormat();
    virtual int GetTxiSize();
    virtual char* GetTxiData();
    virtual int GetTextureFormat();
    virtual void SetTextureFormat(Aurora::AuroraTextureFormats nFormat);
    virtual void SetTxiData(char* pData, int nSize);
    virtual void SetTxiLoaded(bool bLoaded);
    virtual void IncrementTIDIndex(int nIndex);
    virtual int glImage(bool bMipMaps, unsigned char* pImage);
    virtual void FreeImage();
    virtual void Bind();
    virtual TextureController* GetController();

    unsigned char IsNull();
    void SetIsEnvironmentMapped(bool bMapped);
    void SetWidth(int nWidth);
    void SetHeight(int nHeight);
    void SetDepth(int nDepth);
#ifdef _XBOX
    void SetReferencedTexture(CAurTexture* pTexture);
#endif

    SafePointer<CAurTexture> m_pReferenced;
    int m_bEnvironmentMapped;
};

class CAurTextureBasic : public CAurTexture
{
public:
    CAurTextureBasic(char* sName, char* sExtra);
    virtual ~CAurTextureBasic();
    virtual CAurTextureBasic* AsCAurTextureBasic()  // PC 0x00698BD0 GOG 0x00698980
    {
        return this;
    }
    virtual void FreeImage();
    virtual TextureController* GetController()
    {
        return m_pController;
    }

    void Reset();
    int glImage(bool bMipMaps);
    void SetScheme(int* pScheme, int nCount);
    void AddToOrderedLists();
    void RemoveFromOrderedLists();
    void BuildAndStoreTexture();

    static List<CAurTextureBasic*> m_OrderedTextureList;
    static List<CAurTextureBasic*> m_StaticTextureList;
    static List<CAurTextureBasic*> m_DynamicTextureList;
    static bool m_bLowEndEnvMapEnabled;
    static Vector m_vLowEndEnvMapColor;
    static SafePointer<CAurTexture> m_pNullTexture;

    float m_fBumpMapScaling;
    float m_fBumpIntensity;
    float m_fSortKey;
    float m_fGamma;
    float m_fEnvMapAlpha;
    float m_fDiffuseBumpIntensity;
    float m_fSpecularBumpIntensity;
    TextureController* m_pController;
    CAurFontInfo* m_pFontInfo;
    int* m_pScheme;
    unsigned int* m_pTextureIDs;
    unsigned char* m_pImage;
    char* m_pTxiData;
    Vector m_vSpecularColor;
    int m_nTIDIndex;
    int m_nCompressedSize;
    int m_nDataSize;
#ifdef _XBOX

    int m_nCycleCompressedSize;
#endif
    int m_nTxiSize;
    int m_nWidth;
    int m_nHeight;
    int m_nDepth;
    int m_nBPP;
    int m_nTextureFormat;
    char m_sName[32];
    char m_sFileName[32];
    short m_nMaxLevel;
    short m_nBaseLevel;
    short m_nLevel;
    short m_nUploadedLevels;
    short m_nClamp;
    short m_nNumX;
    short m_nNumY;
    short m_nFileRange;
    short m_nDefaultWidth;
    short m_nDefaultHeight;
    short m_nSchemeSize;
    short m_nNumMipMaps;
    unsigned short m_nOrder;
    char m_bTemporary;
    char m_bDynamic;
    int m_nBumpType;
    char m_bDiffuseBumpMap;
    char m_bSpecularBumpMap;
    bool m_bEnvMapped;
    bool m_bCubeMap;
    char m_bMipMap;
    char m_bFilter;
    char m_bUsingGlobalAlpha;
    char m_bMapTexelsToPixels;
    char m_bFullyLoaded;
    char m_bInvalid;
    char m_bSpecial;
    char m_bRasterAllocated;
    char m_bMemoryDirty;
    bool m_bProcessed;
    char m_bTxiLoaded;
    char m_bControllerDirty;
    int m_nMemorySize;
    bool m_b3D;

#ifdef _XBOX

    void ReduceToLevel();
    void ClampMissingImageSize();
    void CalculateHashID();
    int CanReduceLevel();
    void RaiseLevel(int nLevels);
#endif
    void GetMemoryUsage(TEXTUREMEMORYSTATS* pStats);
    void AsNormalizeVectorCubeMap();
    void ConstructImage();
    unsigned char* LoadImageA();
    unsigned char* LoadImagePLT();
    void ParseField(char* sField);
    void Init(char* sName);
};

extern List<CAurTextureBasic*> RefreshTextures;
CAurTextureBasic* AurTextureGetNormalizationCubeMap();
TextureController* AurTextureGetController(CAurTexture* pTexture);
#include "exobase.h"
#include <aurpointer.h>
struct CAurFontInfo;
class CAurTexture;
struct CStringColor
{
#ifdef _XBOX
    CStringColor() { m_fRed = 1.0f; m_fGreen = 1.0f; m_fBlue = 1.0f; m_fAlpha = 1.0f; }
    CStringColor(float fRed, float fGreen, float fBlue, float fAlpha) { m_fRed = fRed; m_fGreen = fGreen; m_fBlue = fBlue; m_fAlpha = fAlpha; }
#else
    CStringColor() {}
    CStringColor(float fRed, float fGreen, float fBlue, float fAlpha)
    {
        m_fRed = fRed;
        m_fGreen = fGreen;
        m_fBlue = fBlue;
        m_fAlpha = fAlpha;
    }
#endif
    float m_fRed;
    float m_fGreen;
    float m_fBlue;
    float m_fAlpha;
};

struct FontVertex
{
    float m_fX;
    float m_fY;
    float m_fZ;
    unsigned int m_nColor;
    float m_fU;
    float m_fV;
};

class CAurFont
{
public:
    int ProcessTextMarkup(char* sText, CAurFont** ppFont, CStringColor& cColor);
    int TextOutA(char* sText, int nLength);
    ~CAurFont();

    static List<CAurFont*> s_LoadedFontList;

    int m_nLoadedFontIndex;
    char m_abReserved[20];
    CAurFontInfo* m_pFontInfo;
    SafePointer<CAurTexture> m_pTexture;
};

extern CStringColor g_cSavedCol;
extern int countbinds;
extern int currenttextime;
extern int texcurrentframe;

#ifdef _XBOX

inline int AurTextureCompressionAvailable()
{
    return 1;
}

void AurSaveGameSnapshot(char* sFileName, int nSize, bool bSquare);
#endif

#endif
