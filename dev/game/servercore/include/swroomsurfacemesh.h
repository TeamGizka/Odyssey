#ifndef __SWROOMSURFACEMESH_H__
#define __SWROOMSURFACEMESH_H__

#define SWROOMSURFACEMESH_ADJACENCY_TOLERANCE_SQUARED 0.01

#define SWROOMSURFACEMESH_MAX_WALK_TRIANGLES 512
#define SWROOMSURFACEMESH_HEIGHT_PROBE 1000.0f

#define SWROOMSURFACEMESH_ALL_MATERIALS_MASK 0xFFFFFFFF

#define SWROOMSURFACEMESH_LINE_LENGTH 256
#define SWROOMSURFACEMESH_VERTEX_GRID 512
#define SWROOMSURFACEMESH_AABB_MARGIN 0.01f

class CResBWM;
class Vector;
class CSWRoomSurfaceMeshAABBNode;
#include "SWCollisionMesh.h"
#include "exobase.h"

class CSWRoomSurfaceExternalEdge
{
public:
    CSWRoomSurfaceExternalEdge();

    CSWRoomSurfaceExternalEdge(int nEdge)
    {
        m_nEdge = nEdge;
        m_nAdjacentRoom = -1;
    }

    CSWRoomSurfaceExternalEdge& operator=(
        const CSWRoomSurfaceExternalEdge& other)
    {
        m_nEdge = other.m_nEdge;
        m_nAdjacentRoom = other.m_nAdjacentRoom;
        return *this;
    }

    int m_nEdge;
    int m_nAdjacentRoom;
};

class CSWRoomSurfaceMeshHitInfo
{
public:
    CSWRoomSurfaceMeshHitInfo(unsigned long nMask);
    int ClearOutput();
    void ClearAll(unsigned long nMask);

    int m_nField00;
    int m_nField04;
    int m_nField08;
    int m_nField0C;
    Vector m_vHitPosition;
    int m_nField1C;
};

class CSWRoomSurfaceMesh : public CSWCollisionMesh
{
public:

    enum SurfaceMaterial
    {
        SURFACE_MATERIAL_UNDEFINED = 0,
        SURFACE_MATERIAL_DIRT = 1,
        SURFACE_MATERIAL_GRASS = 3,
        SURFACE_MATERIAL_STONE = 4,
        SURFACE_MATERIAL_WOOD = 5,
        SURFACE_MATERIAL_WATER = 6,
        SURFACE_MATERIAL_CARPET = 9,
        SURFACE_MATERIAL_METAL = 10,
        SURFACE_MATERIAL_PUDDLES = 11,
        SURFACE_MATERIAL_SWAMP = 12,
        SURFACE_MATERIAL_MUD = 13,
        SURFACE_MATERIAL_LEAVES = 14,
        SURFACE_MATERIAL_LAVA = 15
    };

    CSWRoomSurfaceMesh();
    ~CSWRoomSurfaceMesh();

    virtual int SaveBinaryMesh(const CExoString& sFileName);
    virtual int LoadMeshBinary(CResBWM* pMesh);
    virtual int LoadMeshText(unsigned char* pData, unsigned long nDataSize);
    void ComputeTriangleAdjacency();
    void ComputeRoomBorder();
    int GetAdjacentRoom(int nEdge);
    void GetTriangleAdjacency(int nUnused, int nTriangle, int* pnFirst,
                              int* pnSecond, int* pnThird);
    void GetTriangleVertices(int nUnused, int nTriangle, int* pnFirst,
                             int* pnSecond, int* pnThird);
    int CheckAABBNode(int nNode, const Vector& vMinimum, Vector& vMaximum,
                      CSWRoomSurfaceMeshHitInfo& hitInfo);
    void TransformAABBToWorld(int nNode, const Vector& vOffset);
    int GetEdgeVertex(int nEdge, Vector& vVertex);
    int SetEdgeVertex(int nEdge, Vector& vVertex);
    int GetEdgeVertices(int nEdge, Vector& vFirst, Vector& vSecond);
    int FindTriangle(const Vector& vPosition, int bWalkable, Vector vDirection);
    int GetSurfaceMaterial(float fX, float fY, float fZ);
    int GetSurfaceMaterial(Vector vPosition);
    int GetSurfaceMaterialWalkCheckOnly(Vector vPosition);
    void SetOrientation(const Quaternion& qOrientation);
    virtual void TransformToWorld(const Vector& vPosition);
    virtual int GetWalkCheckMaterialMask()  // PC 0x00576C10 GOG 0x00576C90
    {
        return m_nWalkCheckMaterialMask;
    }
    virtual int GetLOSMaterialMask()  // PC 0x00576C20 GOG 0x00576CA0
    {
        return m_nLOSMaterialMask;
    }
    virtual int GetWalkableMaterialMask()  // PC 0x00576C30 GOG 0x00576CB0
    {
        return m_nWalkableMaterialMask;
    }
    virtual int GetAllMaterialsMask()  // PC 0x00576C40 GOG 0x00576CC0
    {
        return m_nAllMaterialsMask;
    }
    float ComputeHeight(Vector vPosition, int bWalkable);
    float ComputeHeight(int nFace, Vector vPosition);
    int CheckAABBWalkable(Vector vMin, Vector vMax,
                          CSWRoomSurfaceMeshHitInfo& hitInfo);
    int CheckAABBAll(Vector vMin, Vector vMax,
                     CSWRoomSurfaceMeshHitInfo& hitInfo);
    int CheckAABBLineOfSight(Vector vStart, Vector vEnd,
                             CSWRoomSurfaceMeshHitInfo& hitInfo);
    int ClippedLineSegmentWalkable(const Vector& vStart, const Vector& vEnd,
                                   Vector& vClippedStart, Vector& vClippedEnd,
                                   float& fMinHeight, float& fMaxHeight,
                                   int& nFace);
    int WeldEdgeVertices(CSWRoomSurfaceMesh* pOther, float fTolerance,
                         Vector* pvPosition, Vector* pvOtherPosition);
    int SetRoomAdjacency(CSWRoomSurfaceMesh* pOther, int nRoom, int nOtherRoom,
                         const Vector& vPosition, const Vector& vOtherPosition);
    void AddCrossing(const Vector& vStart, const Vector& vDirection,
                     const Vector& vCrossing, int& nCrossings, int nFace);
    int IsPointInPolygon(const Vector& vStart, const Vector& vEnd, int& nFace);
    int ComputeClippedLineSegment(Vector vStart, Vector vEnd,
                                  Vector& vClippedStart, Vector& vClippedEnd,
                                  int& nFace);

    int* m_pTriangleAdjacency;
    CExoStaticArrayList<CSWRoomSurfaceExternalEdge> m_lstExternalEdges;
    CExoStaticArrayList<int> m_lstPerimeters;
    CExoStaticArrayList<CSWRoomSurfaceMeshAABBNode> m_lstAABBNodes;
    CExoArrayList<Vector> m_lstCrossings;
    CExoArrayList<int> m_lstCrossingFaces;
    int m_nAABBRootNode;
    int m_nLOSMaterialMask;
    int m_nWalkableMaterialMask;
    int m_nWalkCheckMaterialMask;
    int m_nAllMaterialsMask;
};

class CSWRoomSurfaceMeshAABBNode
{
public:

    enum TraversalMask
    {
        TRAVERSAL_POSITIVE_X = 1,
        TRAVERSAL_POSITIVE_Y = 2,
        TRAVERSAL_POSITIVE_Z = 4,
        TRAVERSAL_NEGATIVE_X = 8,
        TRAVERSAL_NEGATIVE_Y = 16,
        TRAVERSAL_NEGATIVE_Z = 32
    };
    Vector m_vMin;
    Vector m_vMax;
    int m_nTriangle;
    int m_nNodeType;
    int m_nTraversalMask;
    int m_nLeftChild;
    int m_nRightChild;
    CSWRoomSurfaceMeshAABBNode();
    ~CSWRoomSurfaceMeshAABBNode()  // PC 0x004015A0 GOG 0x004015A0
    {
    }

    CSWRoomSurfaceMeshAABBNode& operator=(
        const CSWRoomSurfaceMeshAABBNode& other)
    {
        m_vMin = other.m_vMin;
        m_vMax = other.m_vMax;
        m_nTriangle = other.m_nTriangle;
        m_nNodeType = other.m_nNodeType;
        m_nTraversalMask = other.m_nTraversalMask;
        m_nLeftChild = other.m_nLeftChild;
        m_nRightChild = other.m_nRightChild;
        return *this;
    }
};

#endif
