#include "math3d.h"
#include <math.h>
// PC 0x004ACEA0  Xbox 0x00310950
void BSPTraverse(BSPNode* pNode, BSPTraverseCallback pCallback, void* pData)
{
    while (pNode != 0)
    {
        pCallback(pNode, pData);
        BSPTraverse(pNode->m_pNegative, pCallback, pData);
        pNode = pNode->m_pPositive;
    }
}
// PC 0x004AD630  Xbox 0x003112A0
BSPNode::BSPNode(Vector vNormal, float fDistance)
    : m_Plane(vNormal, fDistance),
      m_vMinimum(0.0f, 0.0f, 0.0f),
      m_vMaximum(0.0f, 0.0f, 0.0f),
      m_lstNegativePlanes(0),
      m_lstPositivePlanes(0),
      m_lstVertices(0)
{
    m_pNegative = 0;
    m_pPositive = 0;
    m_bLeaf = 0;
    m_pData = 0;
}
// PC 0x004AD6D0  Xbox 0x00311350
void BSPTraverse(BSPNode* pNode, Vector vPoint, BSPPlaneCallback pCallback,
                 void* pData)
{
    if (pNode == 0)
    {
        return;
    }

    List<Plane> planes(0);
    pCallback(pNode, pData, planes);

    float fDistance =
        pNode->m_Plane.m_vNormal * vPoint + pNode->m_Plane.m_fDistance;
    BSPNode* pFar = fDistance > 0.0f ? pNode->m_pPositive : pNode->m_pNegative;
    BSPNode* pNear = fDistance > 0.0f ? pNode->m_pNegative : pNode->m_pPositive;

    BSPTraverse(pNear, vPoint, pCallback, pData);
    BSPTraverse(pFar, vPoint, pCallback, pData);
}
// PC 0x004ACED0  Xbox 0x00310980
BSPNode::~BSPNode()
{
    if (m_pNegative != 0)
    {
        delete m_pNegative;
    }
    if (m_pPositive != 0)
    {
        delete m_pPositive;
    }
}
// PC 0x004ACF60  Xbox 0x00310A10
void BSPTraverse(BSPNode* pNode, const Vector& vCenter, float fRadiusSquared,
                 BSPVolumeCallback pCallback)
{
    if (pNode != 0)
    {
        float fRadius = (float)sqrt(fRadiusSquared);
        do
        {
            pCallback(pNode, vCenter, fRadiusSquared);
            float fDistance =
                pNode->m_Plane.m_vNormal * vCenter + pNode->m_Plane.m_fDistance;
            if (fDistance > -fRadius)
            {
                BSPTraverse(pNode->m_pPositive, vCenter, fRadiusSquared,
                            pCallback);
            }
            if (!(fDistance < fRadius))
            {
                break;
            }
            pNode = pNode->m_pNegative;
        } while (pNode != 0);
    }
}
// PC 0x004ACFF0  Xbox 0x00310AB0
int RFaceSplitTest(List<Vector>& vertices, const Plane& plane)
{
    int nResult = 0;
    for (int i = 0; i < vertices.m_nCount; i++)
    {
        nResult |=
            vertices.m_pList[i] * plane.m_vNormal + plane.m_fDistance < 0.0f ? 1 : 2;
        if (nResult == 3)
        {
            break;
        }
    }
    return nResult;
}
// PC 0x004AD060  Xbox 0x00310C60
BSPNode* BSPFitSphere(BSPNode* pNode, const Vector& vCenter, float fRadius)
{
    while (!pNode->m_bLeaf)
    {
        float fDistance =
            pNode->m_Plane.m_vNormal * vCenter + pNode->m_Plane.m_fDistance;
        if (fDistance <= -fRadius)
        {
            pNode = pNode->m_pNegative;
        }
        else
        {
            if (!(fDistance >= fRadius))
            {
                return pNode;
            }
            pNode = pNode->m_pPositive;
        }
    }
    return pNode;
}
// PC 0x004AD0D0  Xbox 0x00310CD0
BSPNode* BSPFitBox(BSPNode* pNode, const Vector& vMinimum,
                   const Vector& vMaximum)
{
    if (pNode->m_bLeaf)
    {
        return pNode;
    }

    Vector vInverseNormal(-pNode->m_Plane.m_vNormal.m_fX,
                          -pNode->m_Plane.m_vNormal.m_fY,
                          -pNode->m_Plane.m_vNormal.m_fZ);
    Plane inverse(vInverseNormal, -pNode->m_Plane.m_fDistance);

    if (BoxAbovePlane(vMinimum, vMaximum, inverse) >= -0.02f)
    {
        return BSPFitBox(pNode->m_pNegative, vMinimum, vMaximum);
    }
    if (BoxAbovePlane(vMinimum, vMaximum, pNode->m_Plane) >= -0.02f)
    {
        return BSPFitBox(pNode->m_pPositive, vMinimum, vMaximum);
    }
    return pNode;
}
// PC 0x004AD1A0  Xbox 0x00310DB0
BSPNode* BSPSearch(BSPNode* pNode, Vector vPoint, BSPSearchCallback pCallback,
                   void* pData)
{
    if (pCallback(pNode, pData))
    {
        return pNode;
    }
    if (pNode->m_bLeaf)
    {
        return 0;
    }

    BSPNode* pFirst;
    BSPNode* pSecond;
    if (pNode->m_Plane.m_vNormal * vPoint + pNode->m_Plane.m_fDistance < 0.0f)
    {
        pFirst = pNode->m_pNegative;
        pSecond = pNode->m_pPositive;
    }
    else
    {
        pFirst = pNode->m_pPositive;
        pSecond = pNode->m_pNegative;
    }

    BSPNode* pResult = BSPSearch(pFirst, vPoint, pCallback, pData);
    if (pResult != 0)
    {
        return pResult;
    }
    return BSPSearch(pSecond, vPoint, pCallback, pData);
}
// PC 0x004AD260  Xbox 0x00310E90
int BSPHitCheck(BSPNode* pNode, Vector vStart, Vector vEnd, Vector* pHit,
                BSPHitCallback pCallback)
{
    int nHit = 0;
    if (pNode == 0)
    {
        return 0;
    }

    if (pCallback != 0 && (nHit = pCallback(pNode, vStart, vEnd, &vEnd)) != 0)
    {
        *pHit = vEnd;
        nHit = 1;
    }

    if (pNode->m_bLeaf)
    {
        if (pCallback == 0 && pNode->m_bLeaf == 1)
        {
            if (pHit != 0)
            {
                *pHit = vStart;
            }
            return 1;
        }
        return nHit;
    }

    int bStartPositive =
        pNode->m_Plane.m_vNormal * vStart + pNode->m_Plane.m_fDistance > 0.0f;
    int bEndPositive =
        pNode->m_Plane.m_vNormal * vEnd + pNode->m_Plane.m_fDistance > 0.0f;

    if (!bStartPositive && !bEndPositive)
    {
        return nHit |
               BSPHitCheck(pNode->m_pNegative, vStart, vEnd, pHit, pCallback);
    }
    if (bStartPositive == 1 && bEndPositive == 1)
    {
        return nHit |
               BSPHitCheck(pNode->m_pPositive, vStart, vEnd, pHit, pCallback);
    }

    Vector vIntersection = planelineintersection(
        pNode->m_Plane.m_vNormal, pNode->m_Plane.m_fDistance, vStart, vEnd);
    if (!bStartPositive)
    {
        if (BSPHitCheck(pNode->m_pNegative, vStart, vIntersection, pHit,
                        pCallback) != 0)
        {
            return 1;
        }
        return nHit | BSPHitCheck(pNode->m_pPositive, vIntersection, vEnd, pHit,
                                  pCallback);
    }
    if (BSPHitCheck(pNode->m_pPositive, vStart, vIntersection, pHit,
                    pCallback) != 0)
    {
        return 1;
    }
    return nHit | BSPHitCheck(pNode->m_pNegative, vIntersection, vEnd, pHit,
                              pCallback);
}
// PC 0x004AD560  Xbox 0x003111B0
void BSPSphereCheck(BSPNode* pNode, const Vector& vCenter, float fRadius,
                    List<BSPNode*>& nodes)
{
    while (!pNode->m_bLeaf)
    {
        float fDistance =
            pNode->m_Plane.m_vNormal * vCenter + pNode->m_Plane.m_fDistance;
        if (fDistance <= -fRadius)
        {
            pNode = pNode->m_pNegative;
        }
        else
        {
            if (!(fDistance >= fRadius))
            {
                BSPSphereCheck(pNode->m_pNegative, vCenter, fRadius, nodes);
            }
            pNode = pNode->m_pPositive;
        }
    }
    nodes.Add(pNode);
}
// PC 0x004AD7E0  Xbox 0x00311460
void BSPTraverseVolumeApprox(BSPNode* pNode, const Vector& vPoint,
                             List<Plane>& planes,
                             BSPPlaneCallback pCallback, void* pData)
{
    if (pNode == 0)
    {
        return;
    }

    pCallback(pNode, pData, planes);
    if (pNode->m_bLeaf)
    {
        return;
    }

    pNode->m_lstNegativePlanes.m_nCount = 0;
    pNode->m_lstPositivePlanes.m_nCount = 0;
    bool bNegativeRejected = false;
    bool bPositiveRejected = false;

    int i = planes.m_nCount;
    if (i != 0)
    {
        do
        {
            --i;
            int nSide = RFaceSplitTest(pNode->m_lstVertices, planes[i]);
            if (nSide == BSP_FACE_SPLIT)
            {
                pNode->m_lstPositivePlanes.Add(planes[i]);
                pNode->m_lstNegativePlanes.Add(planes[i]);
            }

            bool bSameNormal = planes[i].m_vNormal * pNode->m_Plane.m_vNormal > 0.0f;
            if (nSide == BSP_FACE_NEGATIVE)
            {
                if (bSameNormal)
                {
                    if (!bPositiveRejected)
                    {
                        pNode->m_lstPositivePlanes.Add(planes[i]);
                    }
                }
                else
                {
                    if (!bNegativeRejected)
                    {
                        pNode->m_lstNegativePlanes.Add(planes[i]);
                    }
                }
            }
            else if (nSide == BSP_FACE_POSITIVE)
            {
                if (bSameNormal)
                {
                    bPositiveRejected = true;
                    if (bNegativeRejected)
                    {
                        return;
                    }
                    pNode->m_lstNegativePlanes.Add(planes[i]);
                }
                else
                {
                    bNegativeRejected = true;
                    if (bPositiveRejected)
                    {
                        return;
                    }
                    pNode->m_lstPositivePlanes.Add(planes[i]);
                }
            }

            if (bPositiveRejected && bNegativeRejected)
            {
                return;
            }
        } while (i != 0);
    }

    float fDistance =
        pNode->m_Plane.m_vNormal * vPoint + pNode->m_Plane.m_fDistance;
    BSPNode* pFirst = fDistance > 0.0f ? pNode->m_pPositive : pNode->m_pNegative;
    BSPNode* pSecond = fDistance > 0.0f ? pNode->m_pNegative : pNode->m_pPositive;
    int nFirstPlanes = fDistance > 0.0f ? pNode->m_lstPositivePlanes.m_nCount
                                      : pNode->m_lstNegativePlanes.m_nCount;
    int nSecondPlanes = fDistance > 0.0f ? pNode->m_lstNegativePlanes.m_nCount
                                       : pNode->m_lstPositivePlanes.m_nCount;
    bool bFirstRejected = fDistance > 0.0f ? bPositiveRejected : bNegativeRejected;
    bool bSecondRejected = fDistance > 0.0f ? bNegativeRejected : bPositiveRejected;

    if (!bFirstRejected)
    {
        if (nFirstPlanes)
        {
            BSPTraverseVolumeApprox(pFirst, vPoint,
                fDistance > 0.0f ? pNode->m_lstPositivePlanes : pNode->m_lstNegativePlanes,
                pCallback, pData);
        }
        else
        {
            BSPTraverse(pFirst, vPoint, pCallback, pData);
        }
    }
    if (!bSecondRejected)
    {
        if (nSecondPlanes)
        {
            BSPTraverseVolumeApprox(pSecond, vPoint,
                fDistance > 0.0f ? pNode->m_lstNegativePlanes : pNode->m_lstPositivePlanes,
                pCallback, pData);
        }
        else
        {
            BSPTraverse(pSecond, vPoint, pCallback, pData);
        }
    }
}
