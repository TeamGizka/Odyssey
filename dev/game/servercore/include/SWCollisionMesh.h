#ifndef __SWCOLLISIONMESH_H__
#define __SWCOLLISIONMESH_H__

#include "math3d.h"
#include "gameobjectarray.h"

class CWalkHitInfo
{
public:

    enum { FAILURE_BLOCKED = 1, FAILURE_INSIDE_TRIANGLE = 2 };
    enum { INTERSECTION_LINE = 1, INTERSECTION_CORNER = 2,
           INTERSECTION_TRIANGLE = 3 };

    int m_nFailureReason;

    Vector m_vHitPosition;
    Vector m_vNormal;
    Vector m_vVertex1;
    Vector m_vVertex2;
    Vector m_vDirection;
    unsigned long m_oidBlocker;
    int m_nIntersectionType;
    // PC 0x004DF7E0 GOG 0x004DF900
    CWalkHitInfo()
    {
        m_nFailureReason = 0;
        m_nIntersectionType = 0;
        m_oidBlocker = OBJECT_INVALID;
    }
    // PC 0x004FC0F0 GOG 0x004F41D0
    void SetHitInfo(int nFailureReason, const Vector& vHitPosition,
                    const Vector& vNormal, const Vector& vVertex1,
                    const Vector& vVertex2, int nIntersectionType,
                    const Vector& vDirection)
    {
        m_nFailureReason = nFailureReason;
        m_vHitPosition = vHitPosition;
        m_vNormal = vNormal;
        m_vVertex1 = vVertex1;
        m_vVertex2 = vVertex2;
        m_nIntersectionType = nIntersectionType;
        m_vDirection = vDirection;
    }
};

class CResBWM;
class CResRef;
class Vector;
#include "exoresman.h"

class CRes;
class CWalkHitInfo;

class CSWCollisionMesh
{
public:

    enum RenderColor
    {
        RENDER_COLOR_RED = 0xFFFF0000,
        RENDER_COLOR_GREEN = 0xFF00FF00
    };

    CSWCollisionMesh(unsigned short nTextResType,
                     unsigned short nBinaryResType);
    ~CSWCollisionMesh();
    virtual int LoadDefaultMesh();

    virtual int SaveBinaryMesh(const CExoString& sFileName);
    virtual void TransformToWorld(const Vector& vPosition);
    virtual void GetResourceForBinaryWalkMesh(CResRef& cResRef)  // PC 0x00557090 GOG 0x00557A20
    {
        cResRef = m_cResRef;
    }
    virtual void GetResourceForTextWalkMesh(CResRef& cResRef)  // PC 0x00557090 GOG 0x00557A20
    {
        cResRef = m_cResRef;
    }
    // PC 0x00406590 GOG 0x00406560
    virtual int GetWalkCheckMaterialMask()
    {
        return -1;
    }
    // PC 0x00406590 GOG 0x00406560
    virtual int GetLOSMaterialMask()
    {
        return -1;
    }
    // PC 0x00406590 GOG 0x00406560
    virtual int GetWalkableMaterialMask()
    {
        return -1;
    }
    // PC 0x00406590 GOG 0x00406560
    virtual int GetAllMaterialsMask()
    {
        return -1;
    }

    virtual int LoadMeshBinary(CResBWM* pMesh);
    virtual int LoadMeshText(unsigned char* pData, unsigned long nDataSize);

    int LoadMesh(int nType);
    int LoadMeshString(unsigned char** ppData, unsigned long* pnDataSize,
                       unsigned char* pBuffer, unsigned long nBufferSize);
    void SetTriangleCount(int nCount);
    void SetVertexCount(int nCount);
    Vector WorldToLocal(const Vector& vPosition);
    float ZWorldToZLocal(float fZ);
    Vector LocalToWorld(const Vector& vPosition);

    Vector GetUsePoint(int nUsePoint)
    {
        return LocalToWorld(m_avUsePoints[nUsePoint]);
    }
    Vector NormalLocalToWorld(const Vector& vPosition);
    Vector NormalWorldToLocal(const Vector& vPosition);
    void ComputeMeshNormals();
    int GetMeshBoundingBox(Vector& vMin, Vector& vMax);
    int GetMeshOutline(Vector& vFirst, Vector& vSecond, Vector& vThird,
                       Vector& vFourth);
    void GetTriangleVertices(int nTriangle, int* pnFirst, int* pnSecond,
                             int* pnThird);
    void GetVertex(int nVertex, float* pfX, float* pfY, float* pfZ);
    void GetVertex(int nVertex, Vector* pvVertex);
    void GetFaceVertex(int nIndex, int nCorner, Vector& vVertex);
    int RenderMesh(int nFlag, unsigned long nColor, unsigned long nMask);
    int NoNonWalkPolysOnRoom(const Vector& vStart, const Vector& vEnd, float fA,
                             float fB, float fC, CWalkHitInfo* pHitInfo);
    int NoNonWalkPolys(const Vector& vStart, const Vector& vEnd, float fA,
                       float fB, float fC, CWalkHitInfo* pHitInfo);

    int m_nType;
    int m_nField08;
    CResRef m_cResRef;
    CRes* m_pRes;
    Vector m_vWorldPosition;
    Vector m_vPosition;
    Quaternion m_qOrientation;
    unsigned short m_nBinaryResType;
    unsigned short m_nTextResType;
    int m_bBinary;
    int m_nVertexCount;
    Vector* m_pVertices;
    int m_nFaceCount;
    int m_nAdjacencyCount;
    int* m_pFaces;
    int* m_pFaceMaterials;
    Vector* m_pFaceNormals;
    float* m_pFaceDistances;
    Vector m_avUsePoints[2];
};

#include "math3d.h"

class CSWPlaceableSurfaceMesh : public CSWCollisionMesh
{
public:
    virtual int LoadMeshText(unsigned char*, unsigned long);
    CSWPlaceableSurfaceMesh();
    ~CSWPlaceableSurfaceMesh();
    void SetUsePoint(int nIndex, const Vector& vPosition);
};

#endif
