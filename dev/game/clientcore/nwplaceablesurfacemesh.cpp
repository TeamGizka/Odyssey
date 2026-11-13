#include <stdio.h>
#include <string.h>

#include "SWCollisionMesh.h"
// PC 0x006161D0 GOG 0x006161B0
void CSWPlaceableSurfaceMesh::SetUsePoint(int nIndex, const Vector& vPosition)
{
    m_avUsePoints[nIndex] = WorldToLocal(vPosition);
}
// PC 0x00616BF0 GOG 0x00616BD0
CSWPlaceableSurfaceMesh::CSWPlaceableSurfaceMesh()
    : CSWCollisionMesh(RESTYPE_MDL, 0x805)
{
}
// PC 0x00616C10 GOG 0x00616BF0
CSWPlaceableSurfaceMesh::~CSWPlaceableSurfaceMesh()
{
}
// PC 0x00616C20 GOG 0x00616C00
int CSWPlaceableSurfaceMesh::LoadMeshText(unsigned char* pData,
                                          unsigned long nDataSize)
{
    unsigned char* pLine = new unsigned char[256];
    const char* pText;
    int nFaceIndex;
    int nVertexIndex;
    int nFace3;
    int nFace2;
    int nFace1;
    int nFace0;
    int nVertex1;
    int nVertex2;
    int nVertex0;
    float fOrientW;
    int bFoundTriMesh;
    char chTriangleCount;
    char chVertexCount;
    float fPositionZ;
    int nVertexCount;
    float fPositionX;
    unsigned char* pCurrentData;
    int nTriangleCount;
    float fPositionY;
    unsigned long nRemainingData;
    int bInTriMesh;
    unsigned char nUsePoint;

    if (pLine == NULL)
    {
        return 0;
    }

    pCurrentData = pData;
    nRemainingData = nDataSize;
    bInTriMesh = 0;
    bFoundTriMesh = 0;
    nUsePoint = 0;
    fPositionX = 0.0f;
    fPositionY = 0.0f;
    fPositionZ = 0.0f;
    Vector vVertex;

    while (nRemainingData > 0)
    {
        if (LoadMeshString(&pCurrentData, &nRemainingData, pLine, 256) != 1)
        {
            break;
        }

        for (pText = (char*)pLine; *pText == ' ' || *pText == '\t'; pText++)
        {
        }

        if (strncmp(pText, "node", 4) == 0)
        {
            pText += 4;
            bInTriMesh = 0;
            if (strncmp(pText, " trimesh ", 9) == 0)
            {
                bInTriMesh = 1;
                bFoundTriMesh = 1;
            }
            else if (strncmp(pText, " dummy ", 7) == 0)
            {
                pText += 11;
                if (strnicmp(pText, "pwk_use", 7) == 0)
                {
                    pText += 7;
                }
                else if (strnicmp(pText, "pwk_dp_use_", 11) == 0)
                {
                    pText += 11;
                }
                else
                {
                    continue;
                }

                if (strncmp(pText, "01", 2) == 0)
                {
                    nUsePoint = 1;
                }
                else if (strncmp(pText, "02", 2) == 0)
                {
                    nUsePoint = 2;
                }
            }
        }
        else if (strncmp(pText, "position", 8) == 0)
        {
            sscanf(pText + 8, "%f %f %f", &fPositionX, &fPositionY,
                   &fPositionZ);
            if (nUsePoint == 0)
            {
                if (bInTriMesh != 1)
                {
                    delete[] pLine;
                    return 0;
                }
                m_vPosition = Vector(fPositionX, fPositionY, fPositionZ);
            }
            else if (nUsePoint >= 1 && nUsePoint <= 2)
            {
                SetUsePoint(nUsePoint - 1,
                            Vector(fPositionX, fPositionY, fPositionZ));
                nUsePoint = 0;
            }
            else
            {
                delete[] pLine;
                return 0;
            }
        }
        else if (bInTriMesh == 1)
        {
            if (strncmp(pText, "endnode", 7) == 0)
            {
                bInTriMesh = 0;
                nUsePoint = 0;
            }
            else if (strncmp(pText, "orientation", 11) == 0)
            {
                Vector vOrientation;
                sscanf(pText + 11, "%f %f %f %f", &vOrientation.m_fX,
                       &vOrientation.m_fY, &vOrientation.m_fZ, &fOrientW);
                if (vOrientation.m_fX != 0.0f || vOrientation.m_fY != 0.0f ||
                    vOrientation.m_fZ != 0.0f)
                {
                    Quaternion qOrientation(vOrientation, fOrientW);
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
                sscanf(pText, "%d %c", &nVertexCount, &chVertexCount);
                SetVertexCount(nVertexCount);
                for (nVertexIndex = 0; nVertexIndex < nVertexCount;
                     nVertexIndex++)
                {
                    if (LoadMeshString(&pCurrentData, &nRemainingData, pLine,
                                       256) == 0)
                    {
                        return 0;
                    }
                    sscanf((char*)pLine, "%f %f %f", &vVertex.m_fX,
                           &vVertex.m_fY, &vVertex.m_fZ);
                    m_pVertices[nVertexIndex] = vVertex;
                }
            }
            else if (strncmp(pText, "faces", 5) == 0)
            {
                sscanf(pText + 5, "%d %c", &nTriangleCount, &chTriangleCount);
                SetTriangleCount(nTriangleCount);
                for (nFaceIndex = 0; nFaceIndex < nTriangleCount; nFaceIndex++)
                {
                    if (LoadMeshString(&pCurrentData, &nRemainingData, pLine,
                                       256) == 0)
                    {
                        delete[] pLine;
                        return 0;
                    }
                    sscanf((char*)pLine, "%d %d %d %d %d %d %d %d", &nVertex0,
                           &nVertex1, &nVertex2, &nFace0, &nFace3, &nFace2,
                           &nFace1, &m_pFaceMaterials[nFaceIndex]);
                    m_pFaces[nFaceIndex * 3] = nVertex0;
                    m_pFaces[nFaceIndex * 3 + 1] = nVertex1;
                    m_pFaces[nFaceIndex * 3 + 2] = nVertex2;
                }
            }
        }
    }

    delete[] pLine;
    if (bFoundTriMesh == 0)
    {
        m_cResRef.GetResRef();
    }
    return 1;
}
