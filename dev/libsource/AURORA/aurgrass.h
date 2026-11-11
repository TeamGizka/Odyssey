#ifndef __AURGRASS_H__
#define __AURGRASS_H__

#include <aurlist.h>
#include <aurpointer.h>
#include "../math3d/math3d.h"
#include "exobase.h"

class CAurTexture;
class CAurObject;
class CAurTriangleListElem;
class MaxFace;
class MdlNodeTriMesh;
class PartTriMesh;
class WindManager;

#define AUR_GRASS_MAX_WIND_MAGNITUDE 5.0f
#define AUR_GRASS_WIND_MAX_TIMESTEP 0.1f

class CAurGrassInfo
{
public:

    enum { NUM_GRASS_PROBABILITIES = 4 };
    char m_szTextureName[32];
    float m_fDensity;
    float m_fQuadSize;
    Vector m_vDiffuse;
    Vector m_vAmbient;
    float m_afProbability[NUM_GRASS_PROBABILITIES];
    char m_bInUse;
    CAurGrassInfo();
};

extern List<CAurGrassInfo> g_GrassInfo;
void AurGrassAddInfo(int nMaterial, const CAurGrassInfo& info);

class CAurTriangleBin
{
public:
    unsigned short m_nVertexSize;

    enum { HAS_VERTEX_DATA = 1, HAS_BOUNDING_BOX = 2, GRASS_POLYS_BUILT = 4,
           MULTI_TEXTURE = 8, FOUR_COMPONENTS = 16 };
    unsigned char m_nFlags;
    char m_bUnmapped03;
    float m_fDistance;
    float m_fRadius;
    int m_nTriangles;
    int m_nGrassInfo;
    Vector m_vBoundingBoxMinimum;
    Vector m_vBoundingBoxMaximum;
    Vector m_vMaximum;

    float* m_pVertices;
    float* m_pCurrentVertices;
    int m_nVertexOffset;
    int m_nIndexOffset;
    List<CAurTriangleListElem> m_lstTriangles;
    SafePointer<CAurTexture> m_pTexture;
    List<int> m_lstTextureIndices;
    List<SafePointer<CAurTexture> > m_lstTextures;
    void FinishGeometry();
    void UpdateGrassForWind(WindManager*);
    void RenderGrassPolys(int nGrassInfo, const Vector& vPosition,
                         WindManager* pWindManager);

    static float m_fNearRadiusSquared;
    static float m_fFarRadiusSquared;
    void AddTriangle(const MaxFace& face, PartTriMesh* pTriMesh,
                    MdlNodeTriMesh* pNode);
    CAurTriangleBin();
    ~CAurTriangleBin();
    void DestroyGrassPolys();

    void SortTriangles();
    void CreateArrays();
    void BuildGrassPolys();
    void ReloadColor();
    void SetMinDistanceSquared(float fDistanceSquared);
};

class CAurTriangleListElem
{
public:
    List<float> m_lstVertices;
    MaxFace* m_pFace;
    PartTriMesh* m_pTriMesh;
    int m_nTriangle;
    CAurTriangleListElem();
    CAurTriangleListElem(CAurTriangleListElem& other)
        : m_lstVertices(other.m_lstVertices), m_pFace(other.m_pFace),
          m_pTriMesh(other.m_pTriMesh), m_nTriangle(other.m_nTriangle)
    {
    }
    CAurTriangleListElem(List<float> lstVertices, MaxFace* pFace,
                         PartTriMesh* pTriMesh, int nTriangle);
    ~CAurTriangleListElem();
};

#endif
