#ifndef __SWSROOM_H__
#define __SWSROOM_H__

class CSWRoomSurfaceMesh;
class CSWRoomSurfaceMeshHitInfo;
class Vector;
#include "swroom.h"
#include "swroomsurfacemesh.h"

class CWalkHitInfo;

class CSWSRoom : public CSWRoom
{
public:
    CSWSRoom();
    virtual ~CSWSRoom();
    void DestroyWalkMesh();

    int GetAdjacentRoom(int nEdge);
    float ComputeHeight(Vector vPosition, int nFace);
    float ComputeHeight(int nFace, Vector vPosition);
    int LoadWalkMesh(int nType);
    int CheckAABBWalkable(const Vector& vMin, const Vector& vMax,
                          CSWRoomSurfaceMeshHitInfo& hitInfo);
    int CheckAABBAll(Vector vMin, Vector vMax,
                     CSWRoomSurfaceMeshHitInfo& hitInfo);
    int ClippedLineSegmentWalkable(const Vector& vStart, const Vector& vEnd,
                                   Vector& vClippedStart, Vector& vClippedEnd,
                                   float& fStartT, float& fEndT, int& nFace);
    int NoNonWalkPolysOnRoom(const Vector& vStart, const Vector& vEnd,
                             const float& fA, const float& fB, const float& fC,
                             CWalkHitInfo* pHitInfo);
    void TransformToWorld();
    int WeldRoom(CSWSRoom& other, float fTolerance);
    void ComputeMeshNormals();
    int SetRoomAdjacency(CSWSRoom& other, int nEdge, int nOtherEdge);
    int ClearLineOfSight(Vector vStart, Vector vEnd, Vector* pvHit,
                         unsigned long* pnFace);
    Vector GetNormalOfPoint(int nFace);

    CSWRoomSurfaceMesh* m_pWalkMesh;
    CSWRoomSurfaceMesh* GetWalkMesh();
    char m_aUnmapped40[0xC];
};

#endif
