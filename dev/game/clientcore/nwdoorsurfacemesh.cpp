#include "nwdoorsurfacemesh.h"
#include "ResBWM.h"
#include <stdio.h>
#include <string.h>

const int CSWDoorSurfaceMesh::DOOR_STATE_CLOSED = 0;
const int CSWDoorSurfaceMesh::DOOR_STATE_OPEN1 = 1;
const int CSWDoorSurfaceMesh::DOOR_STATE_OPEN2 = 2;
// PC 0x00616210 GOG 0x006161F0
void CSWDoorSurfaceMesh::SetOpenPosition(unsigned char nIndex,
                                         Vector vPosition)
{
    EXOXBOXASSERT(nIndex <= 1); m_avUsePoints[nIndex] = WorldToLocal(vPosition);
}
// PC 0x00616250 GOG 0x00616230
CSWDoorSurfaceMesh::CSWDoorSurfaceMesh()
    : CSWCollisionMesh(RESTYPE_MDL, 0x804)
{
}
// PC 0x00616270 GOG 0x00616250
CSWDoorSurfaceMesh::~CSWDoorSurfaceMesh()
{
}
// PC 0x00616280 GOG 0x00616260
void CSWDoorSurfaceMesh::SetDoorType(int nType)
{
    m_nDoorType = nType;
    switch (nType)
    {
    case 0:
        strcpy(m_szDoorState, "closed");
        break;
    case 1:
        strcpy(m_szDoorState, "open1");
        break;
    case 2:
        strcpy(m_szDoorState, "open2");
        break;
    default:
        strcpy(m_szDoorState, " ");
        break;
    }
}
// PC 0x00616300 GOG 0x006162E0
int CSWDoorSurfaceMesh::LoadMeshText(unsigned char* pData, unsigned long nSize)
{
    char* pLine = new char[256];
    int bInTriMesh = 0;

    if (pLine == NULL)
    {
        return 0;
    }

    unsigned char* pCurrentData = pData;
    unsigned long nRemainingData = nSize;
    float fPositionX = 0.0f;
    float fPositionY = 0.0f;
    float fPositionZ = 0.0f;
    Vector vVertex;
    int nNodeDoorType = -1;
    int nUsePoint = -1;
    int bInNode = 0;
    int bFoundMesh = 0;
    bInTriMesh = 0;

    while (nRemainingData > 0 &&
           LoadMeshString(&pCurrentData, &nRemainingData,
                          (unsigned char*)pLine, 256) == 1)
    {
        const char* pText;
        for (pText = pLine; *pText == ' ' || *pText == '	'; pText++)
        {
        }

        if (strncmp(pText, "node", 4) == 0)
        {
            pText += 4;
            bInNode = 0;
            if (strncmp(pText, " trimesh ", 9) == 0)
            {
                pText += 11;
                if (strnicmp(pText, "_DWK_wg_", 8) == 0)
                {
                    if (strncmp(pText + 8, m_szDoorState, 5) == 0)
                    {
                        nNodeDoorType = m_nDoorType;
                    }
                    else
                    {
                        nNodeDoorType = -1;
                    }
                    bInNode = 1;
                    bFoundMesh = 1;
                    bInTriMesh = 1;
                    continue;
                }
            }
            if (strncmp(pText, " dummy ", 7) == 0)
            {
                pText += 9;
                if (strnicmp(pText, "_DWK_dp_", 8) == 0)
                {
                    bInTriMesh = 0;
                    bInNode = 1;
                    bFoundMesh = 1;
                    pText += 8;
                    if (strncmp(pText, m_szDoorState, 5) == 0)
                    {
                        nNodeDoorType = m_nDoorType;
                        pText += strlen(m_szDoorState);
                    }
                    if (strncmp(pText, "_01", 3) == 0)
                    {
                        nUsePoint = 0;
                    }
                    else if (strncmp(pText, "_02", 3) == 0)
                    {
                        nUsePoint = 1;
                    }
                    continue;
                }
            }
        }

        if (strncmp(pText, "endnode", 7) == 0)
        {
            bInNode = 0;
            nNodeDoorType = -1;
        }
        else if (bInNode == 1 && nNodeDoorType != -1)
        {
            if (strncmp(pText, "position", 8) == 0)
            {
                sscanf(pText + 8, "%f %f %f", &fPositionX, &fPositionY,
                       &fPositionZ);
                if (bInTriMesh)
                {
                    m_vPosition = Vector(fPositionX, fPositionY, fPositionZ);
                }
                else
                {
                    SetOpenPosition(nUsePoint, Vector(fPositionX, fPositionY,
                                                      fPositionZ));
                }
            }
            else if (bInTriMesh)
            {
                if (strncmp(pText, "orientation", 11) == 0)
                {
                    Vector vAxis;
                    float fAngle;
                    sscanf(pText + 11, "%f %f %f %f", &vAxis.m_fX,
                           &vAxis.m_fY, &vAxis.m_fZ, &fAngle);
                    if (vAxis.m_fX != 0.0f || vAxis.m_fY != 0.0f ||
                        vAxis.m_fZ != 0.0f)
                    {
                        Quaternion qOrientation(vAxis, fAngle);
                    }
                }
                else if (strncmp(pText, "verts", 5) == 0 ||
                         strncmp(pText, "vertices", 8) == 0)
                {
                    if (strncmp(pText, "verts", 5) == 0)
                    {
                        pText += 5;
                    }
                    if (strncmp(pText, "vertices", 8) == 0)
                    {
                        pText += 8;
                    }
                    int nVertexCount;
                    char chVertexCount;
                    sscanf(pText, "%d %c", &nVertexCount, &chVertexCount);
                    SetVertexCount(nVertexCount);
                    for (int nVertex = 0; nVertex < nVertexCount; nVertex++)
                    {
                        if (LoadMeshString(&pCurrentData, &nRemainingData,
                                           (unsigned char*)pLine, 256) == 0)
                        {
                            LoadDefaultMesh();
                            delete pLine;
                            return 1;
                        }
                        sscanf(pLine, "%f %f %f", &vVertex.m_fX,
                               &vVertex.m_fY, &vVertex.m_fZ);
                        m_pVertices[nVertex] = vVertex;
                    }
                }
                else if (strncmp(pText, "faces", 5) == 0)
                {
                    int nFaceCount;
                    char chFaceCount;
                    sscanf(pText + 5, "%d %c", &nFaceCount, &chFaceCount);
                    SetTriangleCount(nFaceCount);
                    for (int nFace = 0; nFace < nFaceCount; nFace++)
                    {
                        if (LoadMeshString(&pCurrentData, &nRemainingData,
                                           (unsigned char*)pLine, 256) == 0)
                        {
                            delete pLine;
                            return 1;
                        }
                        int nVertex0;
                        int nVertex1;
                        int nVertex2;
                        int nSmoothGroup;
                        int nTexture0;
                        int nTexture1;
                        int nTexture2;
                        int nMaterial;
                        sscanf(pLine, "%d %d %d %d %d %d %d %d", &nVertex0,
                               &nVertex1, &nVertex2, &nSmoothGroup,
                               &nTexture0, &nTexture1, &nTexture2,
                               &nMaterial);
                        m_pFaceMaterials[nFace] = nMaterial;
                        m_pFaces[3 * nFace] = nVertex0;
                        m_pFaces[3 * nFace + 1] = nVertex1;
                        m_pFaces[3 * nFace + 2] = nVertex2;
                    }
                }
            }
        }
    }

    delete pLine;
    if (bFoundMesh == 0)
    {
        LoadDefaultMesh();
    }
    return 1;
}
// PC 0x00616910 GOG 0x006168F0
int CSWDoorSurfaceMesh::LoadMeshBinary(CResBWM* pResBWM)
{
    CSWCollisionMesh::LoadMeshBinary(pResBWM);

    if (!pResBWM->GetAbsoluteUsePoint(0).IsZero())
    {
        m_avUsePoints[0] = pResBWM->GetAbsoluteUsePoint(0);
    }
    if (!pResBWM->GetAbsoluteUsePoint(1).IsZero())
    {
        m_avUsePoints[1] = pResBWM->GetAbsoluteUsePoint(1);
    }

    return 1;
}
// PC 0x00616A50 GOG 0x00616A30
inline void CSWDoorSurfaceMesh::GetResourceForBinaryWalkMesh(CResRef& cResRef)
{
    CExoString sResRef;
    CExoString sSuffix;
    m_cResRef.CopyToString(sResRef);
    sSuffix.Format("%s%d", sResRef.CStr(), m_nDoorType);
    cResRef = sSuffix;
}
// PC 0x00616B00 GOG 0x00616AE0
int CSWDoorSurfaceMesh::SaveBinaryMesh(const CExoString& sFileName)
{
    int nResult = CSWCollisionMesh::SaveBinaryMesh(sFileName);

    if (nResult == 1)
    {
        FILE* pFile = fopen(sFileName.CStr(), "rb+");
        _CSWWalkMeshHeader header;

        fread(&header, sizeof(header), 1, pFile);
        fseek(pFile, 0, SEEK_END);
        {
            Vector vUsePoint = LocalToWorld(m_avUsePoints[0]);
            header.m_avAbsoluteUsePoints[0] = vUsePoint;
        }
        {
            Vector vUsePoint = LocalToWorld(m_avUsePoints[1]);
            header.m_avAbsoluteUsePoints[1] = vUsePoint;
        }
        fseek(pFile, 0, SEEK_SET);
        fwrite(&header, sizeof(header), 1, pFile);
        fclose(pFile);
        return 1;
    }

    return nResult;
}
