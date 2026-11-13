#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <GL/gl.h>
#include <string.h>
#include "aurlist.h"
#include "../math3d/math3d.h"
#include "aurmodel.h"
#include "auremitter.h"
#include "aurprimitive.h"
#include "aurmaterial.h"
#include "aurtristripper.h"
#include "aurrendergl.h"

#pragma inline_recursion(on)
#pragma inline_depth(10)

#include <new>

static inline void AurpostprocessTrace2();
static inline void AurpostprocessTrace1();
#define AURPOSTPROCESS_TRACE() AurpostprocessTrace1()

int animationfootprint = 0;

int countanimnodes = 0;
int countprunedanimnodes = 0;
int countcrappyfaces1 = 0;
int countcrappyfaces2 = 0;
int countcrappyfaces3 = 0;
// PC 0x004015A0
void MdlNode::InternalPostProcess()
{
}
// PC 0x004015A0
void MdlNodeAnimMesh::InternalPostProcess()
{
}

static inline void AurpostprocessTrace2()
{
    AurpostprocessTrace1();
}

static inline void AurpostprocessTrace1()
{
}

void AurPostProcessArrayListDefiners()
{
    List<unsigned int> lstIndices;
    void (List<NewController>::*pfnAllocate)(int) = &List<NewController>::allocate;
}

#ifdef _XBOX

MdlNode* findnode(MdlNode* pNode, char* sName);

void Animation::PruneNodes(MdlNode* pNode)
{
    countanimnodes++;
    for (int i = pNode->m_lChildren.m_nCount; i != 0; )
    {
        i--;
        PruneNodes(pNode->m_lChildren[i]);
    }
    if (pNode != m_pRootNode && pNode->m_lChildren.m_nCount == 0 && pNode->m_lstControllers.m_nCount == 0)
    {
        pNode->m_pParentNode->m_lChildren.Remove(pNode);
        delete pNode;
        countprunedanimnodes++;
    }
}

void Animation::PruneToAnimRoot()
{
    if (strcmp(m_sAnimRoot, "") != 0 && stricmp(m_sAnimRoot, "NULL") != 0)
    {
        MdlNode* pNode = findnode(m_pRootNode, m_sAnimRoot);
        if (pNode != 0)
        {
            while (pNode->m_pParentNode != 0)
            {
                MdlNode* pParent = pNode->m_pParentNode;
                pParent->m_lstControllers.SetSize(0);
                for (int i = pParent->m_lChildren.m_nCount; i != 0; )
                {
                    i--;
                    MdlNode* pChild = pParent->m_lChildren[i];
                    if (pChild != pNode)
                    {
                        pParent->m_lChildren.DelIndex(i);
                        delete pChild;
                    }
                }
                pNode = pParent;
            }
            if (pruneanimations)
                PruneNodes(m_pRootNode);
            if (m_fLength < 0.0f)
                m_fLength = 0.0f;
        }
    }
}
#endif

#ifdef _XBOX

int __cdecl CompareTransparencyHint(const void* pFirst, const void* pSecond);

void MdlNode::SortChildrenByTransparency()
{
    int i;

    for (i = 0; i < m_lChildren.m_nCount; i++)
    {
        if (m_lChildren[i]->AsMdlNodeTriMesh() && m_lChildren[i]->AsMdlNodeTriMesh()->m_nTransparencyHint > 0)
        {
            qsort(m_lChildren.m_pList, m_lChildren.m_nCount, sizeof(MdlNode*), CompareTransparencyHint);
            break;
        }
    }
    for (i = 0; i < m_lChildren.m_nCount; i++)
    {
        m_lChildren[i]->SortChildrenByTransparency();
    }
}
#endif
// PC 0x0044E030
void MdlNodeAnimMesh::InternalGenVertices()
{

    m_nAnimationDataCount1 = m_lAnimationVectors1.m_nCount /
                            m_pPostProcess->m_lPositions.m_nCount;
    m_nAnimationDataCount2 = m_lAnimationVectors2.m_nCount /
                            m_pPostProcess->m_lTextureCoordinates0.m_nCount;

    int nFace;
    int nCorner;
    int nVertex;
    int nSample;
    for (nFace = 0; nFace < m_lFaces.m_nCount; nFace++)
    {
        for (nCorner = 0; nCorner < 3; nCorner++)
        {
            List<Vector> lPositions;
            List<Vector> lTexCoords;
            List<Vector> lNormals;
            int nPosition = m_pPostProcess->m_lFaces[nFace].m_pFaces[nCorner];
            int nTexCoord = m_pPostProcess->m_lTextureCoordinates0.m_nCount != 0
                ? m_pPostProcess->m_lFaces[nFace].m_pFaceIndices[nCorner] : 0;
            int nNormal = m_pPostProcess->m_lFaces[nFace].m_pFaceRemap[nCorner];
            for (nSample = 0; nSample < m_nAnimationDataCount1; nSample++)
            {
                lPositions.Add(m_lAnimationVectors1[nPosition + nSample *
                    m_pPostProcess->m_lPositions.m_nCount]);
                lNormals.Add(m_lAnimationVectors3[nNormal + nSample *
                    m_pPostProcess->m_lNormals.m_nCount]);
            }
            for (nSample = 0; nSample < m_nAnimationDataCount2; nSample++)
            {
                lTexCoords.Add(m_lAnimationVectors2[nTexCoord + nSample *
                    m_pPostProcess->m_lTextureCoordinates0.m_nCount]);
            }

            Vector* pPosition = &m_pPostProcess->m_lPositions[nPosition];
            Vector* pTexCoord0 = m_pPostProcess->m_lTextureCoordinates0.m_nCount != 0
                ? &m_pPostProcess->m_lTextureCoordinates0[nTexCoord] : 0;
            Vector* pTexCoord1 = m_pPostProcess->m_lTextureCoordinates1.m_nCount != 0
                ? &m_pPostProcess->m_lTextureCoordinates1[m_pPostProcess->m_lFaces[nFace].m_pFaceIndices[nCorner + 3]] : 0;
            Vector* pTexCoord2 = m_pPostProcess->m_lTextureCoordinates2.m_nCount != 0
                ? &m_pPostProcess->m_lTextureCoordinates2[m_pPostProcess->m_lFaces[nFace].m_pFaceIndices[nCorner + 6]] : 0;
            Vector* pTexCoord3 = m_pPostProcess->m_lTextureCoordinates3.m_nCount != 0
                ? &m_pPostProcess->m_lTextureCoordinates3[m_pPostProcess->m_lFaces[nFace].m_pFaceIndices[nCorner + 9]] : 0;
            Vector* pNormal = &m_pPostProcess->m_lNormals[nNormal];
            Vector* pColor = 0;
            if (m_pPostProcess->m_lColors.m_nCount > 0)
                pColor = &m_pPostProcess->m_lColors[m_pPostProcess->m_lFaces[nFace].m_pFaces[nCorner]];

            NodeVertexAnim* pVertex = new NodeVertexAnim(pPosition, pTexCoord0,
                pTexCoord1, pTexCoord2, pTexCoord3, pNormal, pColor,
                lPositions, lTexCoords, lNormals);
            bool bFound = false;
            for (nVertex = 0; nVertex < m_pPostProcess->m_lstVertices.m_nCount; nVertex++)
            {
                NodeVertexAnim* pExisting = ((NodeVertex*)m_pPostProcess->m_lstVertices[nVertex])->AsNodeVertexAnim();
                if (*pExisting == *pVertex)
                {
                    delete pVertex;
                    m_lFaces[nFace].m_anVertexIndices[nCorner] = nVertex;
                    bFound = true;
                    break;
                }
            }
            if (!bFound)
            {
                m_lFaces[nFace].m_anVertexIndices[nCorner] =
                    m_pPostProcess->m_lstVertices.m_nCount;
                m_pPostProcess->m_lstVertices.Add(pVertex);
            }
        }
    }
}
// PC 0x00448F30
inline char* MdlNode::TranslateNodeName()
{
    if (m_pModel->AsModel())
    {
        return m_pModel->AsModel()->m_lNames[m_nIndex];
    }
    else
    {
        return ((Model*)m_pModel->AsAnimation()->m_pBinaryData)->m_lNames[m_nIndex];
    }
}
// PC 0x004490F0
int UpdateAnimFootprint(MdlNode* pNode, int nMultiplier)
{
    animationfootprint += nMultiplier * pNode->m_lstControllerData.m_nCount * sizeof(float);
    animationfootprint += nMultiplier * pNode->m_lstControllers.m_nCount * sizeof(NewController);

    if (pNode->AsMdlNodeAABB() != 0)
        animationfootprint += nMultiplier * sizeof(MdlNodeAABB);
    else if (pNode->AsMdlNodeAnimMesh() != 0)
        animationfootprint += nMultiplier * sizeof(MdlNodeAnimMesh);
    else if (pNode->AsMdlNodeCamera() != 0)
        animationfootprint += nMultiplier * sizeof(MdlNodeCamera);
    else if (pNode->AsMdlNodeDanglyMesh() != 0)
        animationfootprint += nMultiplier * sizeof(MdlNodeDanglyMesh);
    else if (pNode->AsMdlNodeEmitter() != 0)
        animationfootprint += nMultiplier * sizeof(MdlNodeEmitter);
    else if (pNode->AsMdlNodeLight() != 0)
        animationfootprint += nMultiplier * sizeof(MdlNodeLight);
    else if (pNode->AsMdlNodeLightsaber() != 0)
        animationfootprint += nMultiplier * sizeof(MdlNodeLightsaber);
    else if (pNode->AsMdlNodeReference() != 0)
        animationfootprint += nMultiplier * sizeof(MdlNodeReference);
    else if (pNode->AsMdlNodeSkin() != 0)
        animationfootprint += nMultiplier * sizeof(MdlNodeSkin);
    else if (pNode->AsMdlNodeTriMesh() != 0)
        animationfootprint += nMultiplier * sizeof(MdlNodeTriMesh);

    animationfootprint += nMultiplier * pNode->m_lChildren.m_nCount * sizeof(MdlNode*);
    for (int i = 0; i < pNode->m_lChildren.m_nCount; i++)
        UpdateAnimFootprint(pNode->m_lChildren[i], nMultiplier);
    return animationfootprint;
}
// PC 0x00449760
void SynchronizeNodes(MdlNode* pDestination, MdlNode* pSource, int& nNodeID)
{
    AurpostprocessTrace2();
    pDestination->m_nNodeID = pSource->m_nNodeID;
    for (int i = 0; i < pDestination->m_lChildren.m_nCount; i++)
    {
        MdlNode* pChild = pDestination->m_lChildren[i];
        bool bMatched = false;
        for (int j = 0; j < pSource->m_lChildren.m_nCount; j++)
        {
            MdlNode* pSourceChild = pSource->m_lChildren[j];
            if (stricmp(pChild->TranslateNodeName(), pSourceChild->TranslateNodeName()) == 0)
            {
                SynchronizeNodes(pChild, pSourceChild, nNodeID);
                bMatched = true;
                break;
            }
        }
        if (!bMatched)
        {
            pChild->m_nNodeID = nNodeID;
            nNodeID++;
        }
    }
}
// PC 0x00449AC0
int GetMaxID(MdlNode* pNode, int nMaximumID)
{
    if (pNode->m_nNodeID > nMaximumID)
        nMaximumID = pNode->m_nNodeID;
    for (int i = 0; i < pNode->m_lChildren.m_nCount; i++)
        nMaximumID = GetMaxID(pNode->m_lChildren[i], nMaximumID);
    return nMaximumID;
}
// PC 0x00449D70
void MaxTree::SynchronizeTree(MaxTree* pTree)
{
    if (pTree != 0)
    {
        MdlNode* pNode = pTree->m_pRootNode;
        int nNodeID = GetMaxID(pNode, -1) + 1;

        SynchronizeNodes(m_pRootNode, pNode, nNodeID);

        if (AsAnimation() != 0)
        {
            animationfootprint += 140;
            animationfootprint += 36 * AsAnimation()->m_lEvents.m_nCount;
            UpdateAnimFootprint(m_pRootNode, 1);
        }
    }
}

int pruneanimations = 1;
int enabletristrips = 0;
int minstripsize = 5;
// PC 0x00449DF0
void MdlNodeTriMesh::BuildNeighborLists()
{
    for (int nFace = 0; nFace < m_lFaces.m_nCount; nFace++)
    {
        m_lFaces[nFace].m_vNormal = NormalOf(
            m_pPostProcess->m_lPositions[m_pPostProcess->m_lFaces[nFace].m_pFaces[0]],
            m_pPostProcess->m_lPositions[m_pPostProcess->m_lFaces[nFace].m_pFaces[1]],
            m_pPostProcess->m_lPositions[m_pPostProcess->m_lFaces[nFace].m_pFaces[2]]);
        m_lFaces[nFace].m_fDistance =
            -(m_lFaces[nFace].m_vNormal *
              m_pPostProcess->m_lPositions[m_pPostProcess->m_lFaces[nFace].m_pFaces[0]]);

        for (int nEdge = 0; nEdge < 3; nEdge++)
        {
            int nFirst = m_pPostProcess->m_lFaces[nFace].m_pFaces[nEdge];
            int nSecond = m_pPostProcess->m_lFaces[nFace].m_pFaces[(nEdge + 1) % 3];

            m_lFaces[nFace].m_anAdjacentFaces[nEdge] = -1;
            for (int nOther = 0; nOther < m_lFaces.m_nCount; nOther++)
            {
                if (nFace != nOther)
                {
                    for (int nCorner = 0; nCorner < 3; nCorner++)
                    {
                        m_lFaces[nFace].m_anAdjacentFaces[nEdge] = -1;
                        if (m_pPostProcess->m_lFaces[nOther].m_pFaces[nCorner] == nFirst &&
                            (m_pPostProcess->m_lFaces[nOther].m_pFaces[(nCorner + 1) % 3] == nSecond ||
                             m_pPostProcess->m_lFaces[nOther].m_pFaces[(nCorner + 2) % 3] == nSecond))
                        {
                            m_lFaces[nFace].m_anAdjacentFaces[nEdge] = nOther;
                            break;
                        }
                    }
                }
                if (m_lFaces[nFace].m_anAdjacentFaces[nEdge] >= 0)
                {
                    break;
                }
            }
        }
        if (stricmp(m_pModel->m_sName, "P_HK47") == 0)
        {
            int nOpenEdges = 0;
            for (int nSide = 0; nSide < 3; nSide++)
            {
                if (m_lFaces[nFace].m_anAdjacentFaces[nSide] == -1)
                    nOpenEdges++;
            }
            if (nOpenEdges == 1)
                countcrappyfaces1++;
            else if (nOpenEdges == 2)
                countcrappyfaces2++;
            else if (nOpenEdges == 3)
                countcrappyfaces3++;
        }
    }
}
// PC 0x0044A060
void MdlNodeTriMesh::ComputeLocalSurfaceArea(List<MaxFace>& lFaces, float& fArea)
{
    int i = 0;
    fArea = 0.0f;
    unsigned long nPool = m_pModel->AsModel()->m_nVertexPool;
    unsigned char* pData = (unsigned char*)GLRender::LockPool(
        nPool, 0, 0, RequestPoolManager::POOL_LOCK_READ_ONLY);
    unsigned int nOffset = m_nVertexDataOffset;
    for (; i < lFaces.m_nCount; i++)
    {
        float* pVertex0 = (float*)(pData + nOffset + m_nVertexStride * lFaces[i].m_anVertexIndices[0]);
        Vector vVertex0(pVertex0[0], pVertex0[1], pVertex0[2]);
        float* pVertex1 = (float*)(pData + nOffset + m_nVertexStride * lFaces[i].m_anVertexIndices[1]);
        Vector vVertex1(pVertex1[0], pVertex1[1], pVertex1[2]);
        float* pVertex2 = (float*)(pData + nOffset + m_nVertexStride * lFaces[i].m_anVertexIndices[2]);
        Vector vVertex2(pVertex2[0], pVertex2[1], pVertex2[2]);
        Vector vEdge1 = vVertex1 - vVertex0;
        Vector vEdge2 = vVertex2 - vVertex0;
        Vector vNormal = cross(vEdge2, vEdge1);
        float fFaceArea = magnitude(vNormal);
        fFaceArea *= 0.5f;
        fArea += fFaceArea;
    }
    nPool = m_pModel->AsModel()->m_nVertexPool;
    GLRender::UnlockPool(nPool);
}
// PC 0x00448EF0
void MdlNodeEmitter::InternalPostProcess()
{
    Model* pModel = m_pModel->AsModel();
    if (pModel != 0 && pModel->m_fRadius < 40.0f)
        pModel->m_fRadius = 40.0f;
}
// PC 0x0044A340
inline NodeVertexAnim::~NodeVertexAnim()
{
    m_lstPositions.SetSize(0);
    m_lstNormals.SetSize(0);
    m_lstTexCoords.SetSize(0);
}
// PC 0x0044A480
int MdlNode::CreateSkinNodeList(List<MdlNodeTriMesh*>& lstNodes)
{
    int nSkinNodes = 0;
    for (int i = 0; i < m_lChildren.m_nCount; i++)
    {
        if (m_lChildren[i]->AsMdlNodeLightsaber() == 0)
        {
            if (m_lChildren[i]->AsMdlNodeTriMesh() != 0 &&
                m_lChildren[i]->AsMdlNodeTriMesh()->m_bRender != 0)
            {
                lstNodes.Add(m_lChildren[i]->AsMdlNodeTriMesh());
                if (m_lChildren[i]->AsMdlNodeSkin() != 0)
                    nSkinNodes++;
            }
            nSkinNodes += m_lChildren[i]->CreateSkinNodeList(lstNodes);
        }
    }
    return nSkinNodes;
}
// PC 0x0044A7E0
void MdlNode::ProcessSkinSeams()
{
    if (m_pModel->m_nType & MaxTree::BINARY_TYPE_FLAG)
        return;

    List<MdlNodeTriMesh*> lstNodes;
    if (CreateSkinNodeList(lstNodes) == 0)
        return;

    unsigned char* pBuffer = 0;
    unsigned long nPool;
    List<RenderVertexN*> lstNormals;
    List<Vector> lstPositions;
    List<Quaternion> lstOrientations;
    List<int> lstNormalNodes;
    Vector vPosition;
    Vector vOtherPosition;
    Vector vLocalNormal;
    Vector vNormal;
    Vector vDifference;

    int nNode;
    for (nNode = 0; nNode < lstNodes.m_nCount; nNode++)
    {
        MdlNode* pNode = lstNodes[nNode];
        Vector vTranslation;
        Quaternion qOrientation;
        while (pNode != 0)
        {
            vTranslation = pNode->m_qOrientation * vTranslation + pNode->m_vPosition;
            qOrientation = pNode->m_qOrientation * qOrientation;
            pNode = pNode->m_pParentNode;
        }
        lstPositions.Add(vTranslation);
        lstOrientations.Add(qOrientation);
    }

    if (lstNodes.m_nCount >= 1)
    {
        nPool = lstNodes[0]->m_pModel->AsModel()->m_nVertexPool;
#ifdef _XBOX
        pBuffer = (unsigned char*)GLRender::LockPool(
            nPool, 0, 0, RequestPoolManager::POOL_LOCK_WRITE);
#else
        pBuffer = (unsigned char*)GLRender::LockPool(
            nPool, 0, 0, RequestPoolManager::POOL_LOCK_READ_WRITE);
#endif
    }

    for (nNode = 0; nNode < lstNodes.m_nCount - 1; nNode++)
    {
        for (int nVertex = 0; nVertex < lstNodes[nNode]->m_nVertexCount; nVertex++)
        {
            RenderVertexV* pPosition = (RenderVertexV*)(pBuffer +
                lstNodes[nNode]->m_nVertexDataOffset + lstNodes[nNode]->m_nPositionOffset +
                nVertex * lstNodes[nNode]->m_nVertexStride);
            vPosition.m_fX = pPosition->position.m_fX;
            vPosition.m_fY = pPosition->position.m_fY;
            vPosition.m_fZ = pPosition->position.m_fZ;
            vPosition = lstOrientations[nNode] * vPosition + lstPositions[nNode];
            lstNormals.SetSize(0);
            lstNormalNodes.SetSize(0);
            RenderVertexN* pNormal = (RenderVertexN*)(pBuffer +
                lstNodes[nNode]->m_nVertexDataOffset + lstNodes[nNode]->m_nNormalOffset +
                nVertex * lstNodes[nNode]->m_nVertexStride);
            lstNormals.Add(pNormal);
            lstNormalNodes.Add(nNode);
#ifdef _XBOX

            long nPackedNormal = *(long*)pNormal;
            vLocalNormal.m_fX = (float)((nPackedNormal << 21) >> 21) * (1.0f / 1023.0f);
            vLocalNormal.m_fY = (float)((nPackedNormal << 10) >> 21) * (1.0f / 1023.0f);
            vLocalNormal.m_fZ = (float)(nPackedNormal >> 22) * (1.0f / 511.0f);
#else
            vLocalNormal.m_fX = pNormal->normal.m_fX;
            vLocalNormal.m_fY = pNormal->normal.m_fY;
            vLocalNormal.m_fZ = pNormal->normal.m_fZ;
#endif
            vNormal = lstOrientations[nNode] * vLocalNormal;

            for (int nOther = 0; nOther < lstNodes.m_nCount; nOther++)
            {
                if (nNode != nOther)
                {

                    for (int nOtherVertex = 0;
                         nOtherVertex < lstNodes[nOther]->m_nVertexCount + 1;
                         nOtherVertex++)
                    {
                        RenderVertexV* pOtherPosition = (RenderVertexV*)(pBuffer +
                            lstNodes[nOther]->m_nVertexDataOffset +
                            lstNodes[nOther]->m_nPositionOffset +
                            nOtherVertex * lstNodes[nOther]->m_nVertexStride);
                        vOtherPosition.m_fX = pOtherPosition->position.m_fX;
                        vOtherPosition.m_fY = pOtherPosition->position.m_fY;
                        vOtherPosition.m_fZ = pOtherPosition->position.m_fZ;
                        vOtherPosition = lstOrientations[nOther] * vOtherPosition + lstPositions[nOther];
                        vDifference = vOtherPosition - vPosition;
                        if ((vDifference.m_fX < 0.0f ? -vDifference.m_fX : vDifference.m_fX) < MDL_SKIN_SEAM_TOLERANCE &&
                            (vDifference.m_fY < 0.0f ? -vDifference.m_fY : vDifference.m_fY) < MDL_SKIN_SEAM_TOLERANCE &&
                            (vDifference.m_fZ < 0.0f ? -vDifference.m_fZ : vDifference.m_fZ) < MDL_SKIN_SEAM_TOLERANCE)
                        {
                            pNormal = (RenderVertexN*)(pBuffer +
                                lstNodes[nOther]->m_nVertexDataOffset +
                                lstNodes[nOther]->m_nNormalOffset +
                                nOtherVertex * lstNodes[nOther]->m_nVertexStride);
                            lstNormals.Add(pNormal);
                            lstNormalNodes.Add(nOther);
#ifdef _XBOX

                            long nPackedNormal = *(long*)pNormal;
                            vLocalNormal.m_fX = (float)((nPackedNormal << 21) >> 21) * (1.0f / 1023.0f);
                            vLocalNormal.m_fY = (float)((nPackedNormal << 10) >> 21) * (1.0f / 1023.0f);
                            vLocalNormal.m_fZ = (float)(nPackedNormal >> 22) * (1.0f / 511.0f);
#else
                            vLocalNormal.m_fX = pNormal->normal.m_fX;
                            vLocalNormal.m_fY = pNormal->normal.m_fY;
                            vLocalNormal.m_fZ = pNormal->normal.m_fZ;
#endif
                            vNormal += lstOrientations[nOther] * vLocalNormal;
                        }
                    }
                }
            }

            if (lstNormals.m_nCount != 1)
            {
                vNormal = vNormal / (float)lstNormals.m_nCount;
                vNormal.Normalize();
                for (int nNormal = 0; nNormal < lstNormals.m_nCount; nNormal++)
                {
                    vLocalNormal = -lstOrientations[lstNormalNodes[nNormal]] * vNormal;
#ifdef _XBOX
                    *(unsigned long*)lstNormals[nNormal] =
                        ((((unsigned long)(vLocalNormal.m_fY * 1023.0f) & 0x7ff) |
                        ((unsigned long)(vLocalNormal.m_fZ * 511.0f) << 11)) << 11) |
                        ((unsigned long)(vLocalNormal.m_fX * 1023.0f) & 0x7ff);
#else
                    lstNormals[nNormal]->normal.m_fX = vLocalNormal.m_fX;
                    lstNormals[nNormal]->normal.m_fY = vLocalNormal.m_fY;
                    lstNormals[nNormal]->normal.m_fZ = vLocalNormal.m_fZ;
#endif
                }
            }
        }
    }

    if (lstNodes.m_nCount >= 1)
    {
        nPool = lstNodes[0]->m_pModel->AsModel()->m_nVertexPool;
        GLRender::UnlockPool(nPool);
    }
}
void SynchronizeNodes(MdlNode* pDestination, MdlNode* pSource, int& nNodeID);
// PC 0x0044BA40
void MdlNodeTriMesh::InternalGenVertices()
{
    int nFace;
    int nCorner;
    int nVertex;

    for (nFace = 0; nFace < m_pPostProcess->m_lFaces.m_nCount; nFace++)
    {
        for (nCorner = 0; nCorner < 3; nCorner++)
        {
#ifdef _XBOX
            Vector* pPosition = &m_pPostProcess->m_lPositions[m_pPostProcess->m_lFaces[nFace].m_pFaces[nCorner]];
            Vector* pTexCoord0 = m_pPostProcess->m_lTextureCoordinates0.m_nCount != 0
                ? &m_pPostProcess->m_lTextureCoordinates0[m_pPostProcess->m_lFaces[nFace].m_pFaceIndices[nCorner]] : 0;
            Vector* pTexCoord1 = m_pPostProcess->m_lTextureCoordinates1.m_nCount != 0
                ? &m_pPostProcess->m_lTextureCoordinates1[m_pPostProcess->m_lFaces[nFace].m_pFaceIndices[nCorner + 3]] : 0;
            Vector* pTexCoord2 = m_pPostProcess->m_lTextureCoordinates2.m_nCount != 0
                ? &m_pPostProcess->m_lTextureCoordinates2[m_pPostProcess->m_lFaces[nFace].m_pFaceIndices[nCorner + 6]] : 0;
            Vector* pTexCoord3 = m_pPostProcess->m_lTextureCoordinates3.m_nCount != 0
                ? &m_pPostProcess->m_lTextureCoordinates3[m_pPostProcess->m_lFaces[nFace].m_pFaceIndices[nCorner + 9]] : 0;
            Vector* pNormal = &m_pPostProcess->m_lNormals[m_pPostProcess->m_lFaces[nFace].m_pFaceRemap[nCorner]];
            Vector* pColor = 0;
            if (m_pPostProcess->m_lColors.m_nCount > 0)
                pColor = &m_pPostProcess->m_lColors[m_pPostProcess->m_lFaces[nFace].m_pFaces[nCorner]];

#else
            MaxFace_PostProcess* pFace = &m_pPostProcess->m_lFaces[nFace];
            Vector* pPosition = &m_pPostProcess->m_lPositions[pFace->m_pFaces[nCorner]];
            Vector* pTexCoord0 = m_pPostProcess->m_lTextureCoordinates0.m_nCount != 0
                ? &m_pPostProcess->m_lTextureCoordinates0[pFace->m_pFaceIndices[nCorner]] : 0;
            Vector* pTexCoord1 = m_pPostProcess->m_lTextureCoordinates1.m_nCount != 0
                ? &m_pPostProcess->m_lTextureCoordinates1[pFace->m_pFaceIndices[nCorner + 3]] : 0;
            Vector* pTexCoord2 = m_pPostProcess->m_lTextureCoordinates2.m_nCount != 0
                ? &m_pPostProcess->m_lTextureCoordinates2[pFace->m_pFaceIndices[nCorner + 6]] : 0;
            Vector* pTexCoord3 = m_pPostProcess->m_lTextureCoordinates3.m_nCount != 0
                ? &m_pPostProcess->m_lTextureCoordinates3[pFace->m_pFaceIndices[nCorner + 9]] : 0;
            Vector* pNormal = &m_pPostProcess->m_lNormals[pFace->m_pFaceRemap[nCorner]];
            Vector* pColor = 0;
            if (m_pPostProcess->m_lColors.m_nCount > 0)
                pColor = &m_pPostProcess->m_lColors[pFace->m_pFaces[nCorner]];

#endif
            NodeVertex* pVertex = new NodeVertex(pPosition, pTexCoord0, pTexCoord1,
                                               pTexCoord2, pTexCoord3, pNormal, pColor);
            bool bFound = false;
            for (nVertex = 0; nVertex < m_pPostProcess->m_lstVertices.m_nCount; nVertex++)
            {
                if (*(NodeVertex*)m_pPostProcess->m_lstVertices[nVertex] == *pVertex)
                {
                    delete pVertex;
                    m_lFaces[nFace].m_anVertexIndices[nCorner] = nVertex;
                    bFound = true;
                    break;
                }
            }
            if (!bFound)
            {
                m_lFaces[nFace].m_anVertexIndices[nCorner] =
                    m_pPostProcess->m_lstVertices.m_nCount;
                m_pPostProcess->m_lstVertices.Add(pVertex);
            }
        }
    }

    if (AsMdlNodeAABB() == 0)
    {
        for (nFace = m_lFaces.m_nCount; nFace != 0; )
        {
            nFace--;
            if (m_lFaces[nFace].m_anVertexIndices[0] == m_lFaces[nFace].m_anVertexIndices[1] ||
                m_lFaces[nFace].m_anVertexIndices[0] == m_lFaces[nFace].m_anVertexIndices[2] ||
                m_lFaces[nFace].m_anVertexIndices[1] == m_lFaces[nFace].m_anVertexIndices[2])
            {
                m_lFaces.DelIndex(nFace);
            }
        }
    }
}

#ifdef _XBOX
int __fastcall FindFaceCorner(MaxFace_PostProcess& face, int nPosition);
#endif

void MdlNodeTriMesh::ProcessVerts()
{
    if (MaxFace_PostProcess::texindices1.m_nCount == 0 && m_bLightMapped)
    {
        for (int nFace = 0; nFace < m_pPostProcess->m_lFaces.m_nCount; nFace++)
        {
            MaxFace_PostProcess::texindices1.Add(Vector(
                (float)m_pPostProcess->m_lFaces[nFace].m_pFaceIndices[0],
                (float)m_pPostProcess->m_lFaces[nFace].m_pFaceIndices[1],
                (float)m_pPostProcess->m_lFaces[nFace].m_pFaceIndices[2]));
        }
    }

    for (int nFace = 0; nFace < m_pPostProcess->m_lFaces.m_nCount; nFace++)
    {
        int nSmoothingGroup = m_pPostProcess->m_lFaces[nFace].m_nFaces;
        for (int nCorner = 0; nCorner < 3; nCorner++)
        {
            if (MaxFace_PostProcess::texindices1.m_nCount != 0)
            {
                m_pPostProcess->m_lFaces[nFace].m_pFaceIndices[3] = (int)MaxFace_PostProcess::texindices1[nFace].m_fX;
                m_pPostProcess->m_lFaces[nFace].m_pFaceIndices[4] = (int)MaxFace_PostProcess::texindices1[nFace].m_fY;
                m_pPostProcess->m_lFaces[nFace].m_pFaceIndices[5] = (int)MaxFace_PostProcess::texindices1[nFace].m_fZ;
            }
            if (MaxFace_PostProcess::texindices2.m_nCount != 0)
            {
                m_pPostProcess->m_lFaces[nFace].m_pFaceIndices[6] = (int)MaxFace_PostProcess::texindices2[nFace].m_fX;
                m_pPostProcess->m_lFaces[nFace].m_pFaceIndices[7] = (int)MaxFace_PostProcess::texindices2[nFace].m_fY;
                m_pPostProcess->m_lFaces[nFace].m_pFaceIndices[8] = (int)MaxFace_PostProcess::texindices2[nFace].m_fZ;
            }
            if (MaxFace_PostProcess::texindices3.m_nCount != 0)
            {
                m_pPostProcess->m_lFaces[nFace].m_pFaceIndices[9] = (int)MaxFace_PostProcess::texindices3[nFace].m_fX;
                m_pPostProcess->m_lFaces[nFace].m_pFaceIndices[10] = (int)MaxFace_PostProcess::texindices3[nFace].m_fY;
                m_pPostProcess->m_lFaces[nFace].m_pFaceIndices[11] = (int)MaxFace_PostProcess::texindices3[nFace].m_fZ;
            }
            m_pPostProcess->m_lFaces[nFace].m_pFaceRemap[nCorner] = MaxFace_PostProcess::INVALID_NORMAL_INDEX;
            int nPosition = m_pPostProcess->m_lFaces[nFace].m_pFaces[nCorner];
            Vector vNormal;
            for (int nOtherFace = 0; nOtherFace < m_pPostProcess->m_lFaces.m_nCount; nOtherFace++)
            {
#ifdef _XBOX
                MdlNodeTriMesh_PostProcess* pPostProcess = m_pPostProcess;
                MaxFace_PostProcess& otherFace = pPostProcess->m_lFaces[nOtherFace];
                int nOtherCorner = FindFaceCorner(otherFace, nPosition);
                if (nOtherCorner >= 0 &&
                    (nOtherFace == nFace || (nSmoothingGroup & otherFace.m_nFaces) != 0))
                {
                    Vector vFirst = pPostProcess->m_lPositions[otherFace.m_pFaces[0]];
                    Vector vSecond = pPostProcess->m_lPositions[otherFace.m_pFaces[1]];
                    Vector vThird = pPostProcess->m_lPositions[otherFace.m_pFaces[2]];
#else
                int nOtherCorner;
                for (nOtherCorner = 3; nOtherCorner--;)
                {
                    if (m_pPostProcess->m_lFaces[nOtherFace].m_pFaces[nOtherCorner] == nPosition)
                        break;
                }
                if (nOtherCorner >= 0 &&
                    (nOtherFace == nFace || (nSmoothingGroup & m_pPostProcess->m_lFaces[nOtherFace].m_nFaces) != 0))
                {
                    Vector vFirst = m_pPostProcess->m_lPositions[m_pPostProcess->m_lFaces[nOtherFace].m_pFaces[0]];
                    Vector vSecond = m_pPostProcess->m_lPositions[m_pPostProcess->m_lFaces[nOtherFace].m_pFaces[1]];
                    Vector vThird = m_pPostProcess->m_lPositions[m_pPostProcess->m_lFaces[nOtherFace].m_pFaces[2]];
#endif
                    vNormal = vNormal + cross(vSecond - vFirst, vThird - vSecond);
                    if (nFace > nOtherFace && m_pPostProcess->m_lFaces[nOtherFace].m_nFaces == nSmoothingGroup)
                    {
                        m_pPostProcess->m_lFaces[nFace].m_pFaceRemap[nCorner] =
                            m_pPostProcess->m_lFaces[nOtherFace].m_pFaceRemap[nOtherCorner];
                        break;
                    }
                }
            }
            if (magnitude(vNormal) != 0.0f)
                vNormal.Normalize();
            if (m_pPostProcess->m_lFaces[nFace].m_pFaceRemap[nCorner] == MaxFace_PostProcess::INVALID_NORMAL_INDEX)
            {
                m_pPostProcess->m_lFaces[nFace].m_pFaceRemap[nCorner] = m_pPostProcess->m_lNormals.m_nCount;
                m_pPostProcess->m_lNormals.Add(vNormal);
            }
        }
    }

    Vector vCenter;
    for (int nVertex = 0; nVertex < m_pPostProcess->m_lPositions.m_nCount; nVertex++)
    {
        Vector& vPosition = m_pPostProcess->m_lPositions[nVertex];
        m_vMinimum.m_fX = min(m_vMinimum.m_fX, vPosition.m_fX);
        m_vMinimum.m_fY = min(m_vMinimum.m_fY, vPosition.m_fY);
        m_vMinimum.m_fZ = min(m_vMinimum.m_fZ, vPosition.m_fZ);
        m_vMaximum.m_fX = max(m_vMaximum.m_fX, vPosition.m_fX);
        m_vMaximum.m_fY = max(m_vMaximum.m_fY, vPosition.m_fY);
        m_vMaximum.m_fZ = max(m_vMaximum.m_fZ, vPosition.m_fZ);
        vCenter += vPosition;
    }
    m_vSphereCenter = vCenter / (float)m_pPostProcess->m_lPositions.m_nCount;
    for (nVertex = 0; nVertex < m_pPostProcess->m_lPositions.m_nCount; nVertex++)
    {
        Vector& vPosition = m_pPostProcess->m_lPositions[nVertex];
        float fRadius = magnitude(vPosition - m_vSphereCenter);
        if (fRadius > m_fSphereRadius)
            m_fSphereRadius = fRadius;
    }
}
// PC 0x0044C490
void MdlNodeTriMesh::CalculateTangentSpaceBasis()
{
    unsigned long nPool = m_pModel->AsModel()->m_nVertexPool;
    unsigned char* pVertexData = (unsigned char*)GLRender::LockPool(
        nPool, 0, 0, RequestPoolManager::POOL_LOCK_WRITE);
    Vector vS;
    Vector vT;
    Matrix matBasis;
    Vector vTangent;
    Vector vBinormal;
    Matrix* pFaceBases = new Matrix[m_lFaces.m_nCount];

    for (int nFace = 0; nFace < m_lFaces.m_nCount; nFace++)
    {
        List<MaxFace>& lFaces = m_lFaces;
        Vector vPosition0 = ((NodeVertex*)m_pPostProcess->m_lstVertices[
            lFaces[nFace].m_anVertexIndices[0]])->m_vPosition;
        Vector vPosition1 = ((NodeVertex*)m_pPostProcess->m_lstVertices[
            lFaces[nFace].m_anVertexIndices[1]])->m_vPosition;
        Vector vPosition2 = ((NodeVertex*)m_pPostProcess->m_lstVertices[
            lFaces[nFace].m_anVertexIndices[2]])->m_vPosition;
        Vector vTexCoord0 = ((NodeVertex*)m_pPostProcess->m_lstVertices[
            lFaces[nFace].m_anVertexIndices[0]])->m_vTexCoord0;
        Vector vTexCoord1 = ((NodeVertex*)m_pPostProcess->m_lstVertices[
            lFaces[nFace].m_anVertexIndices[1]])->m_vTexCoord0;
        Vector vTexCoord2 = ((NodeVertex*)m_pPostProcess->m_lstVertices[
            lFaces[nFace].m_anVertexIndices[2]])->m_vTexCoord0;
        Vector vEdge1 = vPosition1 - vPosition0;
        Vector vEdge2 = vPosition2 - vPosition0;
        float fS1 = vTexCoord1.m_fX - vTexCoord0.m_fX;
        float fS2 = vTexCoord2.m_fX - vTexCoord0.m_fX;
        float fT1 = vTexCoord1.m_fY - vTexCoord0.m_fY;
        float fT2 = vTexCoord2.m_fY - vTexCoord0.m_fY;

        vT = fS1 * vEdge2 - fS2 * vEdge1;
        vS = fT1 * vEdge2 - fT2 * vEdge1;
        vTangent = normalize(vT);
        vBinormal = normalize(vS);
        Vector vNormal;
        vNormal = cross(vTangent, vBinormal);
        Vector vFaceNormal = m_pPostProcess->m_lNormals[
            m_pPostProcess->m_lFaces[nFace].m_pFaceRemap[0]];
        if (vFaceNormal * vNormal < 0.0f)
        {
            vBinormal = -vBinormal;
            vNormal = -vNormal;
        }
        matBasis.m_vFirst = vTangent;
        matBasis.m_vSecond = vBinormal;
        matBasis.m_vThird = vNormal;
        pFaceBases[nFace] = matBasis;
    }

    for (nFace = 0; nFace < m_lFaces.m_nCount; nFace++)
    {
        for (int nCorner = 0; nCorner < 3; nCorner++)
        {
            Vector vPosition = ((NodeVertex*)m_pPostProcess->m_lstVertices[
                m_lFaces[nFace].m_anVertexIndices[nCorner]])->m_vPosition;
            List<int> lstFaces;
            lstFaces.Add(nFace);
            int nSmoothingGroups = m_pPostProcess->m_lFaces[nFace].m_nFaces;
            for (int nOtherFace = 0; nOtherFace < m_lFaces.m_nCount; nOtherFace++)
            {
                if (nOtherFace != nFace)
                {
                    for (int nOtherCorner = 0; nOtherCorner < 3; nOtherCorner++)
                    {
                        Vector vOtherPosition = ((NodeVertex*)m_pPostProcess->m_lstVertices[
                            m_lFaces[nOtherFace].m_anVertexIndices[nOtherCorner]])->m_vPosition;
                        if (equal(vPosition, vOtherPosition) &&
                            (nSmoothingGroups & m_pPostProcess->m_lFaces[nOtherFace].m_nFaces))
                            lstFaces.Add(nOtherFace);
                    }
                }
            }

            Matrix matAverage(Vector(0.0f, 0.0f, 0.0f),
                              Vector(0.0f, 0.0f, 0.0f),
                              Vector(0.0f, 0.0f, 0.0f));
            for (int nIndex = 0; nIndex < lstFaces.m_nCount; nIndex++)
            {
                matAverage.m_vFirst += pFaceBases[lstFaces[nIndex]].m_vFirst;
                matAverage.m_vSecond += pFaceBases[lstFaces[nIndex]].m_vSecond;
                matAverage.m_vThird += pFaceBases[lstFaces[nIndex]].m_vThird;
            }
            matAverage.m_vFirst.Normalize();
            matAverage.m_vSecond.Normalize();
            matAverage.m_vThird.Normalize();
            matAverage = Transpose(matAverage);
            int nVertex = m_lFaces[nFace].m_anVertexIndices[nCorner];
#ifdef _XBOX
            *(unsigned long*)(pVertexData + m_TangentBases + m_nVertexDataOffset + nVertex * m_nVertexStride) =
                ((((unsigned long)(matAverage.m_vSecond.m_fX * 1023.0f) & 0x7ff) |
                ((unsigned long)(matAverage.m_vThird.m_fX * 511.0f) << 11)) << 11) |
                ((unsigned long)(matAverage.m_vFirst.m_fX * 1023.0f) & 0x7ff);
            *(unsigned long*)(pVertexData + m_TangentBases + m_nVertexDataOffset + nVertex * m_nVertexStride + 4) =
                ((((unsigned long)(matAverage.m_vSecond.m_fY * 1023.0f) & 0x7ff) |
                ((unsigned long)(matAverage.m_vThird.m_fY * 511.0f) << 11)) << 11) |
                ((unsigned long)(matAverage.m_vFirst.m_fY * 1023.0f) & 0x7ff);
            *(unsigned long*)(pVertexData + m_TangentBases + m_nVertexDataOffset + nVertex * m_nVertexStride + 8) =
                ((((unsigned long)(matAverage.m_vSecond.m_fZ * 1023.0f) & 0x7ff) |
                ((unsigned long)(matAverage.m_vThird.m_fZ * 511.0f) << 11)) << 11) |
                ((unsigned long)(matAverage.m_vFirst.m_fZ * 1023.0f) & 0x7ff);
#else
            *(float*)(pVertexData + m_TangentBases + m_nVertexDataOffset + nVertex * m_nVertexStride) = matAverage.m_vFirst.m_fX;
            *(float*)(pVertexData + m_TangentBases + m_nVertexDataOffset + nVertex * m_nVertexStride + 4) = matAverage.m_vSecond.m_fX;
            *(float*)(pVertexData + m_TangentBases + m_nVertexDataOffset + nVertex * m_nVertexStride + 8) = matAverage.m_vThird.m_fX;
            *(float*)(pVertexData + m_TangentBases + m_nVertexDataOffset + nVertex * m_nVertexStride + 12) = matAverage.m_vFirst.m_fY;
            *(float*)(pVertexData + m_TangentBases + m_nVertexDataOffset + nVertex * m_nVertexStride + 16) = matAverage.m_vSecond.m_fY;
            *(float*)(pVertexData + m_TangentBases + m_nVertexDataOffset + nVertex * m_nVertexStride + 20) = matAverage.m_vThird.m_fY;
            *(float*)(pVertexData + m_TangentBases + m_nVertexDataOffset + nVertex * m_nVertexStride + 24) = matAverage.m_vFirst.m_fZ;
            *(float*)(pVertexData + m_TangentBases + m_nVertexDataOffset + nVertex * m_nVertexStride + 28) = matAverage.m_vSecond.m_fZ;
            *(float*)(pVertexData + m_TangentBases + m_nVertexDataOffset + nVertex * m_nVertexStride + 32) = matAverage.m_vThird.m_fZ;
#endif
        }
    }
    delete[] pFaceBases;
    nPool = m_pModel->AsModel()->m_nVertexPool;
    GLRender::UnlockPool(nPool);
}
// PC 0x0044D030
void MdlNodeSkin::InternalPostProcess()
{
    if (m_nVertexCount <= 0)
    {
        ProcessVerts();
#ifndef _XBOX
        MaxFace_PostProcess::texindices1.m_nCount = 0;
        MaxFace_PostProcess::texindices2.m_nCount = 0;
        MaxFace_PostProcess::texindices3.m_nCount = 0;
#endif
    }
}
// PC 0x0044D060
void MdlNodeLightsaber::InternalPostProcess()
{
    if (m_nVertexCount <= 0)
    {
        ProcessVerts();
        m_pfnGenVertices(this);
        m_nVertexCount = (NUM_FRONT_SEGMENTS + NUM_BACK_SEGMENTS) * NUM_SEGMENT_VERTS + 2 * NUM_PIECE_VERTS;
        m_pVertexPositions = (Vector*)new float[m_nVertexCount * 3];
        m_pVertexNormals = (Vector*)new float[m_nVertexCount * 3];
#ifndef _XBOX
        MdlNodeTriMesh::m_pVertexPositions = (Vector*)new float[(m_nVertexCount + 1) * 3];
#endif
        m_nTextureCount++;
        m_pTextureCoordinates = new float[m_nVertexCount * 2];
        int nVertex = 0;
        int nFirst, nSecond, nThird, nFourth;
        int nLoop;
        for (nLoop = 0; nLoop < NUM_PIECE_VERTS; nLoop++)
        {
            int nSource = 0;
            while ((int)(m_pPostProcess->m_lColors[nSource].m_fX * 255.0f + 0.5f) != nLoop)
                nSource++;
            if (nLoop == 0)
                nFirst = nVertex;
            else if (nLoop == 1)
                nSecond = nVertex;
            else if (nLoop == 2)
                nThird = nVertex;
            else if (nLoop == 3)
                nFourth = nVertex;
            m_pTextureCoordinates[2 * nVertex] = m_pPostProcess->m_lTextureCoordinates0[nSource].m_fX;
            m_pTextureCoordinates[2 * nVertex + 1] = m_pPostProcess->m_lTextureCoordinates0[nSource].m_fY;
            m_pVertexNormals[nVertex].m_fX = m_pPostProcess->m_lNormals[nSource].m_fX;
            m_pVertexNormals[nVertex].m_fY = m_pPostProcess->m_lNormals[nSource].m_fY;
            m_pVertexNormals[nVertex].m_fZ = m_pPostProcess->m_lNormals[nSource].m_fZ;
            m_pVertexPositions[nVertex].m_fX = m_pPostProcess->m_lPositions[nSource].m_fX;
            m_pVertexPositions[nVertex].m_fY = m_pPostProcess->m_lPositions[nSource].m_fY;
            m_pVertexPositions[nVertex].m_fZ = m_pPostProcess->m_lPositions[nSource].m_fZ;
#ifndef _XBOX
            MdlNodeTriMesh::m_pVertexPositions[nVertex] = m_pVertexPositions[nVertex];
#endif
            nVertex++;
        }
        for (nLoop = 0; nLoop < NUM_FRONT_SEGMENTS; nLoop++)
        {
            m_pTextureCoordinates[2 * nVertex] = m_pTextureCoordinates[2 * nFirst];
            m_pTextureCoordinates[2 * nVertex + 1] = m_pTextureCoordinates[2 * nFirst + 1];
            m_pVertexNormals[nVertex].m_fX = m_pVertexNormals[nFirst].m_fX;
            m_pVertexNormals[nVertex].m_fY = m_pVertexNormals[nFirst].m_fY;
            m_pVertexNormals[nVertex].m_fZ = m_pVertexNormals[nFirst].m_fZ;
            m_pVertexPositions[nVertex].m_fX = m_pVertexPositions[nFirst].m_fX;
            m_pVertexPositions[nVertex].m_fY = m_pVertexPositions[nFirst].m_fY;
            m_pVertexPositions[nVertex].m_fZ = m_pVertexPositions[nFirst].m_fZ;
#ifndef _XBOX
            MdlNodeTriMesh::m_pVertexPositions[nVertex] = m_pVertexPositions[nVertex];
#endif
            nVertex++;
            m_pTextureCoordinates[2 * nVertex] = m_pTextureCoordinates[2 * nSecond];
            m_pTextureCoordinates[2 * nVertex + 1] = m_pTextureCoordinates[2 * nSecond + 1];
            m_pVertexNormals[nVertex].m_fX = m_pVertexNormals[nSecond].m_fX;
            m_pVertexNormals[nVertex].m_fY = m_pVertexNormals[nSecond].m_fY;
            m_pVertexNormals[nVertex].m_fZ = m_pVertexNormals[nSecond].m_fZ;
            m_pVertexPositions[nVertex].m_fX = m_pVertexPositions[nSecond].m_fX;
            m_pVertexPositions[nVertex].m_fY = m_pVertexPositions[nSecond].m_fY;
            m_pVertexPositions[nVertex].m_fZ = m_pVertexPositions[nSecond].m_fZ;
#ifndef _XBOX
            MdlNodeTriMesh::m_pVertexPositions[nVertex] = m_pVertexPositions[nVertex];
#endif
            nVertex++;
            m_pTextureCoordinates[2 * nVertex] = m_pTextureCoordinates[2 * nThird];
            m_pTextureCoordinates[2 * nVertex + 1] = m_pTextureCoordinates[2 * nThird + 1];
            m_pVertexNormals[nVertex].m_fX = m_pVertexNormals[nThird].m_fX;
            m_pVertexNormals[nVertex].m_fY = m_pVertexNormals[nThird].m_fY;
            m_pVertexNormals[nVertex].m_fZ = m_pVertexNormals[nThird].m_fZ;
            m_pVertexPositions[nVertex].m_fX = m_pVertexPositions[nThird].m_fX;
            m_pVertexPositions[nVertex].m_fY = m_pVertexPositions[nThird].m_fY;
            m_pVertexPositions[nVertex].m_fZ = m_pVertexPositions[nThird].m_fZ;
#ifndef _XBOX
            MdlNodeTriMesh::m_pVertexPositions[nVertex] = m_pVertexPositions[nVertex];
#endif
            nVertex++;
            m_pTextureCoordinates[2 * nVertex] = m_pTextureCoordinates[2 * nFourth];
            m_pTextureCoordinates[2 * nVertex + 1] = m_pTextureCoordinates[2 * nFourth + 1];
            m_pVertexNormals[nVertex].m_fX = m_pVertexNormals[nFourth].m_fX;
            m_pVertexNormals[nVertex].m_fY = m_pVertexNormals[nFourth].m_fY;
            m_pVertexNormals[nVertex].m_fZ = m_pVertexNormals[nFourth].m_fZ;
            m_pVertexPositions[nVertex].m_fX = m_pVertexPositions[nFourth].m_fX;
            m_pVertexPositions[nVertex].m_fY = m_pVertexPositions[nFourth].m_fY;
            m_pVertexPositions[nVertex].m_fZ = m_pVertexPositions[nFourth].m_fZ;
#ifndef _XBOX
            MdlNodeTriMesh::m_pVertexPositions[nVertex] = m_pVertexPositions[nVertex];
#endif
            nVertex++;
        }
        for (nLoop = 0; nLoop < NUM_PIECE_VERTS; nLoop++)
        {
            int nSource = 0;
            while ((int)(m_pPostProcess->m_lColors[nSource].m_fZ * 255.0f + 0.5f) != nLoop)
                nSource++;
            if (nLoop == 0)
                nFirst = nVertex;
            else if (nLoop == 1)
                nSecond = nVertex;
            else if (nLoop == 2)
                nThird = nVertex;
            else if (nLoop == 3)
                nFourth = nVertex;
            m_pTextureCoordinates[2 * nVertex] = m_pPostProcess->m_lTextureCoordinates0[nSource].m_fX;
            m_pTextureCoordinates[2 * nVertex + 1] = m_pPostProcess->m_lTextureCoordinates0[nSource].m_fY;
            m_pVertexNormals[nVertex].m_fX = m_pPostProcess->m_lNormals[nSource].m_fX;
            m_pVertexNormals[nVertex].m_fY = m_pPostProcess->m_lNormals[nSource].m_fY;
            m_pVertexNormals[nVertex].m_fZ = m_pPostProcess->m_lNormals[nSource].m_fZ;
            m_pVertexPositions[nVertex].m_fX = m_pPostProcess->m_lPositions[nSource].m_fX;
            m_pVertexPositions[nVertex].m_fY = m_pPostProcess->m_lPositions[nSource].m_fY;
            m_pVertexPositions[nVertex].m_fZ = m_pPostProcess->m_lPositions[nSource].m_fZ;
#ifndef _XBOX
            MdlNodeTriMesh::m_pVertexPositions[nVertex] = m_pVertexPositions[nVertex];
#endif
            nVertex++;
        }
        for (nLoop = 0; nLoop < NUM_BACK_SEGMENTS; nLoop++)
        {
            m_pTextureCoordinates[2 * nVertex] = m_pTextureCoordinates[2 * nFirst];
            m_pTextureCoordinates[2 * nVertex + 1] = m_pTextureCoordinates[2 * nFirst + 1];
            m_pVertexNormals[nVertex].m_fX = m_pVertexNormals[nFirst].m_fX;
            m_pVertexNormals[nVertex].m_fY = m_pVertexNormals[nFirst].m_fY;
            m_pVertexNormals[nVertex].m_fZ = m_pVertexNormals[nFirst].m_fZ;
            m_pVertexPositions[nVertex].m_fX = m_pVertexPositions[nFirst].m_fX;
            m_pVertexPositions[nVertex].m_fY = m_pVertexPositions[nFirst].m_fY;
            m_pVertexPositions[nVertex].m_fZ = m_pVertexPositions[nFirst].m_fZ;
#ifndef _XBOX
            MdlNodeTriMesh::m_pVertexPositions[nVertex] = m_pVertexPositions[nVertex];
#endif
            nVertex++;
            m_pTextureCoordinates[2 * nVertex] = m_pTextureCoordinates[2 * nSecond];
            m_pTextureCoordinates[2 * nVertex + 1] = m_pTextureCoordinates[2 * nSecond + 1];
            m_pVertexNormals[nVertex].m_fX = m_pVertexNormals[nSecond].m_fX;
            m_pVertexNormals[nVertex].m_fY = m_pVertexNormals[nSecond].m_fY;
            m_pVertexNormals[nVertex].m_fZ = m_pVertexNormals[nSecond].m_fZ;
            m_pVertexPositions[nVertex].m_fX = m_pVertexPositions[nSecond].m_fX;
            m_pVertexPositions[nVertex].m_fY = m_pVertexPositions[nSecond].m_fY;
            m_pVertexPositions[nVertex].m_fZ = m_pVertexPositions[nSecond].m_fZ;
#ifndef _XBOX
            MdlNodeTriMesh::m_pVertexPositions[nVertex] = m_pVertexPositions[nVertex];
#endif
            nVertex++;
            m_pTextureCoordinates[2 * nVertex] = m_pTextureCoordinates[2 * nThird];
            m_pTextureCoordinates[2 * nVertex + 1] = m_pTextureCoordinates[2 * nThird + 1];
            m_pVertexNormals[nVertex].m_fX = m_pVertexNormals[nThird].m_fX;
            m_pVertexNormals[nVertex].m_fY = m_pVertexNormals[nThird].m_fY;
            m_pVertexNormals[nVertex].m_fZ = m_pVertexNormals[nThird].m_fZ;
            m_pVertexPositions[nVertex].m_fX = m_pVertexPositions[nThird].m_fX;
            m_pVertexPositions[nVertex].m_fY = m_pVertexPositions[nThird].m_fY;
            m_pVertexPositions[nVertex].m_fZ = m_pVertexPositions[nThird].m_fZ;
#ifndef _XBOX
            MdlNodeTriMesh::m_pVertexPositions[nVertex] = m_pVertexPositions[nVertex];
#endif
            nVertex++;
            m_pTextureCoordinates[2 * nVertex] = m_pTextureCoordinates[2 * nFourth];
            m_pTextureCoordinates[2 * nVertex + 1] = m_pTextureCoordinates[2 * nFourth + 1];
            m_pVertexNormals[nVertex].m_fX = m_pVertexNormals[nFourth].m_fX;
            m_pVertexNormals[nVertex].m_fY = m_pVertexNormals[nFourth].m_fY;
            m_pVertexNormals[nVertex].m_fZ = m_pVertexNormals[nFourth].m_fZ;
            m_pVertexPositions[nVertex].m_fX = m_pVertexPositions[nFourth].m_fX;
            m_pVertexPositions[nVertex].m_fY = m_pVertexPositions[nFourth].m_fY;
            m_pVertexPositions[nVertex].m_fZ = m_pVertexPositions[nFourth].m_fZ;
#ifndef _XBOX
            MdlNodeTriMesh::m_pVertexPositions[nVertex] = m_pVertexPositions[nVertex];
#endif
            nVertex++;
        }
        m_nVertexStride = 0;
    }
}
// PC 0x0044DA20
void MdlNodeDanglyMesh::InternalGenVertices()
{

    List<float> lConstraints;
    int nFace;
    int nCorner;
    int nVertex;

    for (nFace = 0; nFace < m_lFaces.m_nCount; nFace++)
    {
        for (nCorner = 0; nCorner < 3; nCorner++)
        {
            int nPosition = m_pPostProcess->m_lFaces[nFace].m_pFaces[nCorner];
            Vector* pPosition = &m_pPostProcess->m_lPositions[nPosition];
            Vector* pTexCoord0 = m_pPostProcess->m_lTextureCoordinates0.m_nCount != 0
                ? &m_pPostProcess->m_lTextureCoordinates0[m_pPostProcess->m_lFaces[nFace].m_pFaceIndices[nCorner]] : 0;
            Vector* pTexCoord1 = m_pPostProcess->m_lTextureCoordinates1.m_nCount != 0
                ? &m_pPostProcess->m_lTextureCoordinates1[m_pPostProcess->m_lFaces[nFace].m_pFaceIndices[nCorner + 3]] : 0;
            Vector* pTexCoord2 = m_pPostProcess->m_lTextureCoordinates2.m_nCount != 0
                ? &m_pPostProcess->m_lTextureCoordinates2[m_pPostProcess->m_lFaces[nFace].m_pFaceIndices[nCorner + 6]] : 0;
            Vector* pTexCoord3 = m_pPostProcess->m_lTextureCoordinates3.m_nCount != 0
                ? &m_pPostProcess->m_lTextureCoordinates3[m_pPostProcess->m_lFaces[nFace].m_pFaceIndices[nCorner + 9]] : 0;
            Vector* pNormal = &m_pPostProcess->m_lNormals[m_pPostProcess->m_lFaces[nFace].m_pFaceRemap[nCorner]];

            NodeVertexDangly* pVertex = new NodeVertexDangly(pPosition, pTexCoord0,
                pTexCoord1, pTexCoord2, pTexCoord3, pNormal, 0, m_lConstraints[nPosition]);
            bool bFound = false;
            for (nVertex = 0; nVertex < m_pPostProcess->m_lstVertices.m_nCount; nVertex++)
            {
                NodeVertexDangly* pExisting = ((NodeVertex*)m_pPostProcess->m_lstVertices[nVertex])->AsNodeVertexDangly();
                if (*pExisting == *pVertex)
                {
                    delete pVertex;
                    m_lFaces[nFace].m_anVertexIndices[nCorner] = nVertex;
                    bFound = true;
                    break;
                }
            }
            if (!bFound)
            {
                m_lFaces[nFace].m_anVertexIndices[nCorner] =
                    m_pPostProcess->m_lstVertices.m_nCount;
                m_pPostProcess->m_lstVertices.Add(pVertex);
                lConstraints.Add(m_lConstraints[nPosition]);
            }
        }
    }

    m_lConstraints.SetSize(0);
    m_lConstraints = lConstraints;
}
// PC 0x0044DD70
void MdlNodeTriMesh::InternalRemoveTemporaryArrays()
{
    if (m_pPostProcess != 0)
    {
        for (int i = 0; i < m_pPostProcess->m_lstVertices.m_nCount; i++)
        {
            delete m_pPostProcess->m_lstVertices.m_pList[i];
        }

        m_pPostProcess->m_lstVertices.SetSize(0);
    }

    MaxFace_PostProcess::texindices1.m_nCount = 0;
    MaxFace_PostProcess::texindices2.m_nCount = 0;
    MaxFace_PostProcess::texindices3.m_nCount = 0;

    delete m_pPostProcess;
    m_pPostProcess = 0;
}
// PC 0x0044DE10
void MdlNodeAnimMesh::InternalRemoveTemporaryArrays()
{
    for (int i = 0; i < m_pPostProcess->m_lstVertices.m_nCount; i++)
    {
        delete (NodeVertexAnim*)m_pPostProcess->m_lstVertices.m_pList[i];
    }

    m_pPostProcess->m_lstVertices.SetSize(0);
    m_lAnimationVectors1.SetSize(0);
    m_lAnimationVectors2.SetSize(0);
    m_lAnimationVectors3.SetSize(0);

    MdlNodeTriMesh::InternalRemoveTemporaryArrays();
}
// PC 0x0044DEF0
void MdlNodeDanglyMesh::InternalRemoveTemporaryArrays()
{
    List<NodeVertex*>* pVertexList;
    int i;

    for (i = 0; i < m_pPostProcess->m_lstVertices.m_nCount; i++)
    {
        delete m_pPostProcess->m_lstVertices.m_pList[i];
    }

#ifdef _XBOX
    m_pPostProcess->m_lstVertices.SetSize(0);
#else
    pVertexList = &m_pPostProcess->m_lstVertices;
    if (pVertexList->m_pList != 0)
    {
        delete[] pVertexList->m_pList;
        pVertexList->m_pList = 0;
    }
    pVertexList->m_nAllocated = 0;
    pVertexList->m_nCount = 0;
#endif

    return ((MdlNodeTriMesh*)this)->InternalRemoveTemporaryArrays();
}
// PC 0x0044DF60
void MdlNodeSkin::InternalRemoveTemporaryArrays()
{
    MdlNodeTriMesh::InternalRemoveTemporaryArrays();
}
// PC 0x0044E570
void MdlNodeTriMesh::BuildVertexIndices()
{
    int nCorner;
    int nStrip;
    int nIndex;
    unsigned long nPool;

    if (enabletristrips)
    {
        List<int> lstIndices;
        List<List<int> > lstStrips(0);
        for (nStrip = 0; nStrip < m_lFaces.m_nCount; nStrip++)
        {
            for (nCorner = 0; nCorner < 3; nCorner++)
                lstIndices.Add(m_lFaces[nStrip].m_anVertexIndices[nCorner]);
        }
        AurStripify(lstIndices, lstStrips);

        for (nStrip = 0; nStrip < lstStrips.m_nCount - 1; nStrip++)
        {
            int nSize = lstStrips[nStrip].m_nCount;
            if (nSize < minstripsize)
            {
                for (nIndex = 0; nIndex < 3; nIndex++)
                    m_lstTriangleIndices.Add(lstStrips[nStrip][nIndex]);
                for (nIndex = 3; nIndex < lstStrips[nStrip].m_nCount; nIndex++)
                {
                    int nWinding = nIndex % 2;
                    if (nWinding)
                    {
                        for (nCorner = nIndex; nCorner >= nIndex - 2; nCorner--)
                            m_lstTriangleIndices.Add(lstStrips[nStrip][nCorner]);
                    }
                    else
                    {
                        for (nCorner = nIndex - 2; nCorner <= nIndex; nCorner++)
                            m_lstTriangleIndices.Add(lstStrips[nStrip][nCorner]);
                    }
                }
            }
            else
            {
                m_lstIndexCounts.Add(nSize);
                unsigned int* pIndices = new unsigned int[lstStrips[nStrip].m_nCount];
                for (nIndex = 0; nIndex < lstStrips[nStrip].m_nCount; nIndex++)
                    pIndices[nIndex] = lstStrips[nStrip][nIndex];
                m_lstIndexArrays.Add(pIndices);
            }
        }
        for (nIndex = 0; nIndex < lstStrips[lstStrips.m_nCount - 1].m_nCount; nIndex++)
            m_lstTriangleIndices.Add(lstStrips[lstStrips.m_nCount - 1][nIndex]);
        m_nPrimitiveType = Aurora::PRIMITIVE_TRIANGLE_STRIP;
    }
    else
    {
        int nSize = m_lFaces.m_nCount * 3;
        unsigned int* pIndices = new unsigned int[nSize];
        for (nStrip = 0; nStrip < m_lFaces.m_nCount; nStrip++)
        {
            for (nCorner = 0; nCorner < 3; nCorner++)
                pIndices[nStrip * 3 + nCorner] = m_lFaces[nStrip].m_anVertexIndices[nCorner];
        }
        m_lstIndexArrays.Add(pIndices);
        m_lstIndexCounts.Add(nSize);
        m_nPrimitiveType = Aurora::PRIMITIVE_TRIANGLES;
    }

    if (enabletristrips)
    {
        for (nStrip = 0; nStrip < m_lstIndexArrays.m_nCount; nStrip++)
        {
            GLRender::RequestPool(Aurora::POOL_INDEX,
                m_lstIndexCounts[nStrip] * sizeof(unsigned short),
                RequestPoolManager::POOL_STATIC, 0, &nPool);
            m_lstIndexPools.Add(nPool);
            unsigned short* pIndices = (unsigned short*)GLRender::LockPool(
                nPool, 0, 0, RequestPoolManager::POOL_LOCK_WRITE);
            for (nIndex = 0; nIndex < m_lstIndexCounts[nStrip]; nIndex++)
                pIndices[nIndex] = m_lstIndexArrays[nStrip][nIndex];
            GLRender::UnlockPool(nPool);
            delete[] m_lstIndexArrays[nStrip];
        }
        GLRender::RequestPool(Aurora::POOL_INDEX,
            m_lstTriangleIndices.m_nCount * sizeof(unsigned short),
            RequestPoolManager::POOL_STATIC, 0, &m_nAuxIndexPool);
        unsigned long nTrianglePool = m_nAuxIndexPool;
        unsigned short* pIndices = (unsigned short*)GLRender::LockPool(
            nTrianglePool, 0, 0, RequestPoolManager::POOL_LOCK_WRITE);
        for (nIndex = 0; nIndex < m_lstTriangleIndices.m_nCount; nIndex++)
            pIndices[nIndex] = m_lstTriangleIndices[nIndex];
        GLRender::UnlockPool(m_nAuxIndexPool);
        m_nAuxIndexCount = m_lstTriangleIndices.m_nCount;
        m_lstIndexArrays.SetSize(0);
        m_lstTriangleIndices.SetSize(0);
    }
    else
    {
        for (nStrip = 0; nStrip < m_lstIndexArrays.m_nCount; nStrip++)
        {
            GLRender::RequestPool(Aurora::POOL_INDEX,
                m_lstIndexCounts[nStrip] * sizeof(unsigned short),
                RequestPoolManager::POOL_STATIC, 0, &nPool);
            m_lstIndexPools.Add(nPool);
            unsigned short* pIndices = (unsigned short*)GLRender::LockPool(
                nPool, 0, 0, RequestPoolManager::POOL_LOCK_WRITE);
            for (nIndex = 0; nIndex < m_lstIndexCounts[nStrip]; nIndex++)
                pIndices[nIndex] = m_lstIndexArrays[nStrip][nIndex];
            GLRender::UnlockPool(nPool);
            GLRender::UnlockPool(nPool);
            delete[] m_lstIndexArrays[nStrip];
        }
        m_lstIndexArrays.SetSize(0);
#ifdef _XBOX
        m_nAuxIndexPool = RequestPoolManager::INVALID_POOL;
        m_nAuxIndexCount = 0;
#else
        m_nAuxIndexCount = 0;
        m_nAuxIndexPool = RequestPoolManager::INVALID_POOL;
#endif
    }
}
// PC 0x0044F070
void MdlNodeTriMesh::InternalPostProcess()
{
    if (m_nVertexCount <= 0)
    {
        if (!m_sTexture0[0])
            strcpy(m_sTexture0, "NULL");
        if (strlen(m_sTexture1) > 16)
            m_sTexture1[16] = 0;
        ProcessVerts();
        int bCalculateTangents = 0;
        if (m_lFaces.m_nCount > 0)
        {
            m_pfnGenVertices(this);
            m_nVertexCount = m_pPostProcess->m_lstVertices.m_nCount;
            BuildNeighborLists();
            BuildVertexIndices();

            NodeVertex* pFirstVertex = 0;
            if (m_nVertexCount > 0)
                pFirstVertex = m_pPostProcess->m_lstVertices[0];
            m_nVertexArrayFlags |= VERTEX_ARRAY_POSITION | VERTEX_ARRAY_NORMAL;
#ifndef _XBOX
            int nVertexSize = 2 * sizeof(Vector);
#endif
            m_vMinimum = m_vMinimum;
            m_nPositionOffset = 0;
            m_vMaximum = m_vMaximum;
            m_vSphereCenter = m_vSphereCenter;
            m_vDiffuse = m_vDiffuse;
            m_nNormalOffset = sizeof(Vector);
#ifdef _XBOX
            unsigned int nStride = m_nNormalOffset + sizeof(unsigned long);
            int nVertexSize = sizeof(Vector) + sizeof(unsigned long);
#else
            unsigned int nStride = m_nNormalOffset + sizeof(Vector);
#endif
            if (pFirstVertex->m_nFlags & NodeVertex::VERTEX_HAS_TEXCOORD0)
            {
                m_nVertexArrayFlags |= VERTEX_ARRAY_TEXCOORD0;
                m_nTextureCount++;
                nStride += 2 * sizeof(float);
                m_nTexCoord0Offset = nVertexSize;
                nVertexSize += 2 * sizeof(float);
            }
            if (pFirstVertex->m_nFlags & NodeVertex::VERTEX_HAS_TEXCOORD1)
            {
                m_nVertexArrayFlags |= VERTEX_ARRAY_TEXCOORD1;
                nStride += 2 * sizeof(float);
                m_nTextureCount++;
                m_nTexCoord1Offset = nVertexSize;
                nVertexSize += 2 * sizeof(float);
            }
            if (pFirstVertex->m_nFlags & NodeVertex::VERTEX_HAS_TEXCOORD2)
            {
                m_nVertexArrayFlags |= VERTEX_ARRAY_TEXCOORD2;
                nStride += 2 * sizeof(float);
                m_nTextureCount++;
                m_nTexCoord2Offset = nVertexSize;
                nVertexSize += 2 * sizeof(float);
            }
            if (pFirstVertex->m_nFlags & NodeVertex::VERTEX_HAS_TEXCOORD3)
            {
                m_nVertexArrayFlags |= VERTEX_ARRAY_TEXCOORD3;
                nStride += 2 * sizeof(float);
                m_nTextureCount++;
                m_nTexCoord3Offset = nVertexSize;
                nVertexSize += 2 * sizeof(float);
            }
            Material* pMaterial = new Material;
            pMaterial->InitializeTextures(m_sTexture0, 0, 0, 0);
#ifdef _XBOX
            if (pMaterial->HasBumpMap())
            {
                bCalculateTangents = 1;
                m_nVertexArrayFlags |= VERTEX_ARRAY_TANGENT_BASIS;
                nStride += 3 * sizeof(unsigned long);
                m_TangentBases = nVertexSize;
            }
#else
            if (pMaterial->HasBumpMap() && !pMaterial->HasNormalMap())
            {
                bCalculateTangents = 1;
                m_nVertexArrayFlags |= VERTEX_ARRAY_TANGENT_BASIS;
                nStride += 3 * sizeof(Vector);
                m_TangentBases = nVertexSize;
            }
#endif
            m_nVertexStride = nStride;
            int nAlignment = m_pModel->AsModel()->m_nVertexDataSize & 0x0f;
            unsigned int nPoolSize = m_pModel->AsModel()->m_nVertexDataSize +
                nStride * (m_nVertexCount + 1) + nAlignment;
            unsigned long nPool = m_pModel->AsModel()->m_nVertexPool;
            GLRender::ResizePool(nPool, nPoolSize);
            unsigned int nOffset = m_pModel->AsModel()->m_nVertexDataSize + nAlignment;
            m_nVertexDataOffset = nOffset;
            m_pModel->AsModel()->m_nVertexDataSize += nStride * (m_nVertexCount + 1) + nAlignment;
            nPool = m_pModel->AsModel()->m_nVertexPool;
            unsigned char* pData = (unsigned char*)GLRender::LockPool(
                nPool, 0, 0, RequestPoolManager::POOL_LOCK_WRITE);
#ifndef _XBOX
            m_pVertexPositions = (Vector*)new char[(m_nVertexCount + 1) * sizeof(Vector)];
#endif
            for (int i = 0; i < m_nVertexCount; i++)
            {
                NodeVertex* pVertex = m_pPostProcess->m_lstVertices[i];
                if (pFirstVertex->m_nFlags & NodeVertex::VERTEX_HAS_TEXCOORD0)
                {
                    ((float*)(pData + i * nStride + m_nTexCoord0Offset + nOffset))[0] = pVertex->m_vTexCoord0.m_fX;
                    ((float*)(pData + i * nStride + m_nTexCoord0Offset + nOffset))[1] = pVertex->m_vTexCoord0.m_fY;
                }
                if (pFirstVertex->m_nFlags & NodeVertex::VERTEX_HAS_TEXCOORD1)
                {
                    ((float*)(pData + i * nStride + m_nTexCoord1Offset + nOffset))[0] = pVertex->m_vTexCoord1.m_fX;
                    ((float*)(pData + i * nStride + m_nTexCoord1Offset + nOffset))[1] = pVertex->m_vTexCoord1.m_fY;
                }
                if (pFirstVertex->m_nFlags & NodeVertex::VERTEX_HAS_TEXCOORD2)
                {
                    ((float*)(pData + i * nStride + m_nTexCoord2Offset + nOffset))[0] = pVertex->m_vTexCoord2.m_fX;
                    ((float*)(pData + i * nStride + m_nTexCoord2Offset + nOffset))[1] = pVertex->m_vTexCoord2.m_fY;
                }
                if (pFirstVertex->m_nFlags & NodeVertex::VERTEX_HAS_TEXCOORD3)
                {
                    ((float*)(pData + i * nStride + m_nTexCoord3Offset + nOffset))[0] = pVertex->m_vTexCoord3.m_fX;
                    ((float*)(pData + i * nStride + m_nTexCoord3Offset + nOffset))[1] = pVertex->m_vTexCoord3.m_fY;
                }
#ifdef _XBOX
                *(unsigned long*)(pData + i * nStride + m_nNormalOffset + nOffset) =
                    ((((unsigned long)(pVertex->m_vNormal.m_fY * 1023.0f) & 0x7ff) |
                    ((unsigned long)(pVertex->m_vNormal.m_fZ * 511.0f) << 11)) << 11) |
                    ((unsigned long)(pVertex->m_vNormal.m_fX * 1023.0f) & 0x7ff);
#else
                ((float*)(pData + i * nStride + m_nNormalOffset + nOffset))[0] = pVertex->m_vNormal.m_fX;
                ((float*)(pData + i * nStride + m_nNormalOffset + nOffset))[1] = pVertex->m_vNormal.m_fY;
                ((float*)(pData + i * nStride + m_nNormalOffset + nOffset))[2] = pVertex->m_vNormal.m_fZ;
#endif
                ((float*)(pData + i * nStride + m_nPositionOffset + nOffset))[0] = pVertex->m_vPosition.m_fX;
                ((float*)(pData + i * nStride + m_nPositionOffset + nOffset))[1] = pVertex->m_vPosition.m_fY;
                ((float*)(pData + i * nStride + m_nPositionOffset + nOffset))[2] = pVertex->m_vPosition.m_fZ;
#ifndef _XBOX
                m_pVertexPositions[i] = pVertex->m_vPosition;
#endif
            }
            memset(pData + nOffset + m_nPositionOffset + nStride * m_nVertexCount, 0, nStride);
            ((float*)(pData + nStride * m_nVertexCount + m_nPositionOffset + nOffset))[0] = MESH_UNUSED_VERTEX_COORDINATE;
            ((float*)(pData + nStride * m_nVertexCount + m_nPositionOffset + nOffset))[1] = MESH_UNUSED_VERTEX_COORDINATE;
            ((float*)(pData + nStride * m_nVertexCount + m_nPositionOffset + nOffset))[2] = MESH_UNUSED_VERTEX_COORDINATE;
            nPool = m_pModel->AsModel()->m_nVertexPool;
            GLRender::UnlockPool(nPool);
            delete pMaterial;
            if (bCalculateTangents)
                CalculateTangentSpaceBasis();
        }
        ComputeLocalSurfaceArea(m_lFaces, m_fSurfaceArea);
        m_pfnRemoveTemporaryArrays(this);
    }
}
// PC 0x0044F5C0
void MdlNodeDanglyMesh::InternalPostProcess()
{
    if (m_nVertexCount <= 0)
    {
        ProcessVerts();
        if (m_lFaces.m_nCount > 0)
        {
            m_pfnGenVertices(this);
            m_nVertexCount = m_pPostProcess->m_lstVertices.m_nCount;
            BuildNeighborLists();
            BuildVertexIndices();

            NodeVertex* pFirstVertex = 0;
            if (m_nVertexCount > 0)
                pFirstVertex = (NodeVertex*)m_pPostProcess->m_lstVertices[0];

            m_nVertexStride = 0;
            m_nPositionOffset = 0;
            m_pVertexPositions = (Vector*)new char[m_nVertexCount * sizeof(Vector)];
#ifdef _XBOX
            int nVertexSize = sizeof(Vector) + sizeof(unsigned long);
            m_nNormalOffset = sizeof(Vector);
#else
            int nVertexSize = 2 * sizeof(Vector);
            m_nNormalOffset = nVertexSize / 2;
#endif
            m_nVertexStride = nVertexSize;
            m_nVertexArrayFlags |= VERTEX_ARRAY_POSITION | VERTEX_ARRAY_NORMAL;

            if (pFirstVertex->m_nFlags & NodeVertex::VERTEX_HAS_TEXCOORD0)
            {
                m_nTextureCount++;
                m_nTexCoord0Offset = nVertexSize;
                m_nVertexStride = nVertexSize += 2 * sizeof(float);
                m_nVertexArrayFlags |= VERTEX_ARRAY_TEXCOORD0;
            }
            if (pFirstVertex->m_nFlags & NodeVertex::VERTEX_HAS_TEXCOORD1)
            {
                m_nTextureCount++;
                m_nTexCoord1Offset = nVertexSize;
                m_nVertexStride = nVertexSize += 2 * sizeof(float);
                m_nVertexArrayFlags |= VERTEX_ARRAY_TEXCOORD1;
            }
            if (pFirstVertex->m_nFlags & NodeVertex::VERTEX_HAS_TEXCOORD2)
            {
                m_nTextureCount++;
                m_nTexCoord2Offset = nVertexSize;
                m_nVertexStride = nVertexSize += 2 * sizeof(float);
                m_nVertexArrayFlags |= VERTEX_ARRAY_TEXCOORD2;
            }
            if (pFirstVertex->m_nFlags & NodeVertex::VERTEX_HAS_TEXCOORD3)
            {
                m_nTextureCount++;
                m_nTexCoord3Offset = nVertexSize;
                m_nVertexStride = nVertexSize += 2 * sizeof(float);
                m_nVertexArrayFlags |= VERTEX_ARRAY_TEXCOORD3;
            }

            int nAlignment = m_pModel->AsModel()->m_nVertexDataSize & 0x0f;
            unsigned int nStride = m_nVertexStride;
            unsigned int nPoolSize = m_pModel->AsModel()->m_nVertexDataSize +
                nStride * (m_nVertexCount + 1) + nAlignment;
            unsigned long nPool = m_pModel->AsModel()->m_nVertexPool;
            GLRender::ResizePool(nPool, nPoolSize);
            unsigned int nOffset = m_pModel->AsModel()->m_nVertexDataSize + nAlignment;
            m_nVertexDataOffset = nOffset;
            m_pModel->AsModel()->m_nVertexDataSize += nStride * (m_nVertexCount + 1) + nAlignment;
            nPool = m_pModel->AsModel()->m_nVertexPool;
            unsigned char* pData = (unsigned char*)GLRender::LockPool(
                nPool, 0, 0, RequestPoolManager::POOL_LOCK_WRITE);
#ifndef _XBOX
            MdlNodeTriMesh::m_pVertexPositions = (Vector*)new char[(m_nVertexCount + 1) * sizeof(Vector)];
#endif

            for (int i = 0; i < m_nVertexCount; i++)
            {
                NodeVertex* pVertex = (NodeVertex*)m_pPostProcess->m_lstVertices[i];
                if (pFirstVertex->m_nFlags & NodeVertex::VERTEX_HAS_TEXCOORD0)
                {
                    ((float*)(pData + i * nStride + m_nTexCoord0Offset + nOffset))[0] = pVertex->m_vTexCoord0.m_fX;
                    ((float*)(pData + i * nStride + m_nTexCoord0Offset + nOffset))[1] = pVertex->m_vTexCoord0.m_fY;
                }
                if (pFirstVertex->m_nFlags & NodeVertex::VERTEX_HAS_TEXCOORD1)
                {
                    ((float*)(pData + i * nStride + m_nTexCoord1Offset + nOffset))[0] = pVertex->m_vTexCoord1.m_fX;
                    ((float*)(pData + i * nStride + m_nTexCoord1Offset + nOffset))[1] = pVertex->m_vTexCoord1.m_fY;
                }
                if (pFirstVertex->m_nFlags & NodeVertex::VERTEX_HAS_TEXCOORD2)
                {
                    ((float*)(pData + i * nStride + m_nTexCoord2Offset + nOffset))[0] = pVertex->m_vTexCoord2.m_fX;
                    ((float*)(pData + i * nStride + m_nTexCoord2Offset + nOffset))[1] = pVertex->m_vTexCoord2.m_fY;
                }
                if (pFirstVertex->m_nFlags & NodeVertex::VERTEX_HAS_TEXCOORD3)
                {
                    ((float*)(pData + i * nStride + m_nTexCoord3Offset + nOffset))[0] = pVertex->m_vTexCoord3.m_fX;
                    ((float*)(pData + i * nStride + m_nTexCoord3Offset + nOffset))[1] = pVertex->m_vTexCoord3.m_fY;
                }
#ifdef _XBOX
                *(unsigned long*)(pData + i * nStride + nOffset + m_nNormalOffset) =
                    ((((unsigned long)(pVertex->m_vNormal.m_fY * 1023.0f) & 0x7ff) |
                    ((unsigned long)(pVertex->m_vNormal.m_fZ * 511.0f) << 11)) << 11) |
                    ((unsigned long)(pVertex->m_vNormal.m_fX * 1023.0f) & 0x7ff);
#else
                ((float*)(pData + i * nStride + nOffset + m_nNormalOffset))[0] = pVertex->m_vNormal.m_fX;
                ((float*)(pData + i * nStride + nOffset + m_nNormalOffset))[1] = pVertex->m_vNormal.m_fY;
                ((float*)(pData + i * nStride + nOffset + m_nNormalOffset))[2] = pVertex->m_vNormal.m_fZ;
#endif
                ((float*)(pData + i * nStride + m_nPositionOffset + nOffset))[0] = pVertex->m_vPosition.m_fX;
                ((float*)(pData + i * nStride + m_nPositionOffset + nOffset))[1] = pVertex->m_vPosition.m_fY;
                ((float*)(pData + i * nStride + m_nPositionOffset + nOffset))[2] = pVertex->m_vPosition.m_fZ;
#ifndef _XBOX
                MdlNodeTriMesh::m_pVertexPositions[i] = pVertex->m_vPosition;
#endif
                m_pVertexPositions[i].m_fX = pVertex->m_vPosition.m_fX;
                m_pVertexPositions[i].m_fY = pVertex->m_vPosition.m_fY;
                m_pVertexPositions[i].m_fZ = pVertex->m_vPosition.m_fZ;
            }

            memset(pData + nOffset + m_nPositionOffset + nStride * m_nVertexCount, 0, nStride);
            ((float*)(pData + nStride * m_nVertexCount + m_nPositionOffset + nOffset))[0] = MESH_UNUSED_VERTEX_COORDINATE;
            ((float*)(pData + nStride * m_nVertexCount + m_nPositionOffset + nOffset))[1] = MESH_UNUSED_VERTEX_COORDINATE;
            ((float*)(pData + nStride * m_nVertexCount + m_nPositionOffset + nOffset))[2] = MESH_UNUSED_VERTEX_COORDINATE;
            GLRender::UnlockPool(m_pModel->AsModel()->m_nVertexPool);
        }
        m_pfnRemoveTemporaryArrays(this);
    }
}
// PC 0x0044FC00
void MdlNodeLight::InternalPostProcess()
{
    m_lstTextures.SetSize(m_lstTextureNames.m_nCount);
    for (int i = m_lstTextureNames.m_nCount; i != 0; )
    {
        i--;
        m_lstTextures[i] = AurTextureGetReference(m_lstTextureNames.m_pList[i], 0);
    }
}
