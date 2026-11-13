#ifndef AURMATERIAL_H
#define AURMATERIAL_H

class CAurTexture;
class CAurShadowData;
class CHitInfo;
class MdlNode;
class Part;
class Plane;
class Vector;
class VertexPrimitiveFlat;
#include "auremitter.h"
#include "math3d.h"
#include <aurlist.h>
#include <aurpointer.h>
#include "aurtexman.h"
#include "aurrendergl.h"

class Material;
struct CAurFontInfo;

class PartTriMeshData
{
public:
    char m_abReserved[168];
    char m_sTexture0[32];
    char m_sTexture1[8];
};

class PartEmitterData
{
public:
    char m_abReserved[208];
    char m_sTexture[32];
};

class PartTriMesh : public Part
{
public:
    PartTriMesh(MdlNode* pNode);
    virtual ~PartTriMesh();
    virtual int HitCheckGeom(const Vector& vStart, Vector& vEnd,
                             Vector* pHit, CHitInfo* pHitInfo, float fRadius);
    virtual PartTriMesh* AsPartTriMesh()  // PC 0x00698BD0 GOG 0x00698980
    {
        return this;
    }
    virtual Part* AsPartDummy() { return 0; }  // PC 0x004065A0 GOG 0x00406570
    virtual void Project(const Vector& vPoint, float fDistance,
                         const Plane& cPlane, CAurShadowData* pShadowData);
    virtual void Mat();
    virtual void GetMinimumSphere(Vector& vPosition, float& fRadius);
    virtual void RotateTexture(float fAngle);
    virtual int WithinRange(Vector vPosition, float fRange);
    virtual float GetTextureAngle()  // PC 0x00453160 GOG 0x004532A0
    {
        return m_fTextureAngle;
    }
    virtual void TransformTextureMatrix();
    virtual void RestoreTextureMatrix();
    virtual void TransformModelviewMatrix();
    virtual void RestoreModelviewMatrix();
    void Draw(bool bRenderChildren);
    int GetRenderPath();

    enum RenderPath
    {
        RENDER_FLAT = 0,
        RENDER_FLAT_VP = 1,
        RENDER_EMBM = 2,
        RENDER_LIGHTMAPPED_BUMP_SPECULAR = 3,
        RENDER_LIGHTMAPPED_BUMP_DIFFUSE = 4,
        RENDER_BUMPMAPPED = 5,
        RENDER_BUMPMAP_FALLBACK = 6,
        RENDER_EMLM = 7,
        RENDER_EMLM_VP = 8,
        RENDER_NV1X_EMLM = 9,
        RENDER_ENVIRONMENTMAPPED = 10,
        RENDER_ENVIRONMENTMAPPED_VP = 11,
        RENDER_LIGHTMAPPED = 12,
        RENDER_LIGHTMAPPED_VP = 13,
        RENDER_LIGHTMAPPED_NO_SHADERS = 14,
        RENDER_NORMALMAPPED = 15,
        RENDER_NOISEMAPPED = 16
    };

    Vector m_vMinimum;
    Vector m_vMaximum;

    Vector m_vSelfIllumColor;
    VertexPrimitiveFlat* m_pVertexPrimitive;

    unsigned long m_nColorPool;

    unsigned long m_nTexCoordPool;

    char m_bColors;

    char m_bFog;
    TileData* m_pTileData;
    float m_fAlpha;
    int m_nReserved88;

    int m_nPartID;

    float* m_pOriginalTexCoords;
    float m_fTextureAngle;

    static bool m_bMaterialHasChanged;

    enum { MAX_SHADOW_INDICES = 4096 };
    static unsigned short m_VertexIndices[];
    static int m_nNextPartID;
};

class Material
{
public:
    Material();
    ~Material();

    void ParseField(char* sField);
    void PurgeBadTexture(SafePointer<CAurTexture>& pTexture);
    int BindTexture0();
    int BindTexture1();
    int BindEnvMap();
    int GetTextureTID();
    int SetDownSampling(int nMax);
    int BumpMapIsSpecular();
    Vector GetBumpMapSpecularColor();

    float GetBumpMapIntensity();
    void SetBlendingMode(int nIndex);
    void SetGUIBlendingMode();
    void SetMaterialColor(Vector vColor, float fAlpha);
    unsigned char HasBumpyShinyMap();
    unsigned char HasEnvMap();
    void Init(char* sName);
    void ClearTextures();
    void InitializeTextures(char* sTexture0, char* sTexture1, char* sTexture2,
                            char* sTexture3);
    void LoadTextures(Part* pPart);
    void ReplaceTexture(char* sOld, char* sNew, char* sLayer, int* pTXI,
                        int nFlags);
    void ReplaceTexture(char* sOld, char* sNew);
    unsigned char HasBumpMap();
    unsigned char HasNormalMap();

    SafePointer<CAurTexture> m_pTexture0;
    SafePointer<CAurTexture> m_pTexture1;
    SafePointer<CAurTexture> m_pTexture2;
    SafePointer<CAurTexture> m_pTexture3;
    SafePointer<CAurTexture> m_pBumpMap;
    SafePointer<CAurTexture> m_pEnvMap;
    SafePointer<CAurTexture> m_pAltTexture;
    char* m_sBumpMapName;
    char* m_sBumpyShinyName;
    char* m_sEnvMapName;
    List<Aurora::AuroraBlendModes> m_lstBlendSrc;
    List<Aurora::AuroraBlendModes> m_lstBlendDst;
    bool m_bDecal;
    int m_nRenderType;
    float m_fWaterAlpha;
};

inline float Material::GetBumpMapIntensity()
{
    return ((CAurTexture*)m_pBumpMap.m_pObject)->GetBumpIntensity();
}

extern List<Material*> materials;
extern int enablebumpmap;
extern float g_fGlobalAlpha;

CAurTexture* AurTextureGetNULL();
CAurTexture* AurTextureGetReference(char* sName, char* sExtra);
void AurTextureRelease(CAurTexture* pTexture);
unsigned int AurTextureGetTID(CAurTexture* pTexture);
int AurTextureMessage(CAurTexture* pTexture, char* sCommand);
int AurTextureReplaceSingleReference(CAurTexture* pTexture, char* sOld,
                                     char* sNew, char* sLayer, int* pTXI,
                                     int nFlags);
void AurTextureSetLayered(CAurTexture* pTexture, char* sName, int* pTXI,
                          int nFlags);

#include "aurmodel.h"

#endif
