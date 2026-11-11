#ifndef __NWDOORSURFACEMESH_H__
#define __NWDOORSURFACEMESH_H__

#include "SWCollisionMesh.h"

class CResBWM;
class CResRef;

class CSWDoorSurfaceMesh : public CSWCollisionMesh
{
public:
    static const int DOOR_STATE_CLOSED;
    static const int DOOR_STATE_OPEN1;
    static const int DOOR_STATE_OPEN2;

    CSWDoorSurfaceMesh();
    ~CSWDoorSurfaceMesh();

    virtual int SaveBinaryMesh(const CExoString& sFileName);
    virtual void GetResourceForBinaryWalkMesh(CResRef& cResRef);
    virtual int LoadMeshBinary(CResBWM* pResBWM);
    virtual int LoadMeshText(unsigned char* pBuffer, unsigned long nSize);

    void SetDoorType(int nType);

    void SetOpenPosition(unsigned char nIndex, Vector vPosition);

    int m_nDoorType;
    char m_szDoorState[8];
};

#endif
