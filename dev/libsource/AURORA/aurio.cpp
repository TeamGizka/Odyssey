#include "aurlist.h"
#include "aurmodel.h"
#include "aurlightmanager.h"
#include "aurrendergl.h"
#include "aurconsole.h"
#include <string.h>
#ifdef _XBOX
#include <xgraphics.h>
#endif

int UpdateAnimFootprint(MdlNode* pNode, int nMultiplier);
Model* FindModel(char* sName);
void InsertModel(Model* pModel);

namespace
{
class Input
{
public:
    class InputBinary
    {
    public:
        MaxTree* ResetLite(unsigned char* pModelData,
                           unsigned char* pVertexData, void* pResource);
        MaxTree* Read(void* pModelResource, void* pVertexResource, bool bLite);
        MaxTree* Reset(unsigned char* pModelData,
                       unsigned char* pVertexData, void* pResource);

        unsigned long m_nModelDataSize;
        unsigned long m_nVertexDataSize;
        unsigned char m_aUnmapped[4];
        void ResetMdlNodePartsLite(MdlNode* pNode, unsigned char* pModelData,
                                   unsigned char* pVertexData);
        void ResetMdlNodeParts(MdlNode* pNode, unsigned char* pModelData,
                               unsigned char* pVertexData);
        void ResetMdlNodeLite(MdlNode* pNode, unsigned char* pModelData,
                              unsigned char* pVertexData);
        void ResetMdlNode(MdlNode* pNode, unsigned char* pModelData,
                          unsigned char* pVertexData);
        void ResetAnim(MdlNodeAnimMesh* pNode, unsigned char* pModelData,
                       unsigned char* pVertexData);
        void ResetLight(MdlNodeLight* pNode, unsigned char* pModelData,
                        unsigned char* pVertexData);
        void ResetAnimation(Animation* pAnimation, unsigned char* pModelData,
                            unsigned char* pVertexData);
        void ResetAABB(MdlNodeAABB* pNode, unsigned char* pModelData,
                       unsigned char* pVertexData);
        void ResetDangly(MdlNodeDanglyMesh* pNode,
                         unsigned char* pModelData,
                         unsigned char* pVertexData);
        void ResetSkin(MdlNodeSkin* pNode, unsigned char* pModelData,
                       unsigned char* pVertexData);
        void ResetTriMeshParts(MdlNodeTriMesh* pNode, unsigned char* pModelData,
                               unsigned char* pVertexData);
        void ResetTriMeshPartsLite(MdlNodeTriMesh* pNode,
                                   unsigned char* pModelData,
                                   unsigned char* pVertexData);
        void ResetLightLite(MdlNodeLight* pNode, unsigned char* pModelData,
                            unsigned char* pVertexData);
        void ResetBaseLite(MdlNode* pNode, unsigned char* pModelData,
                           unsigned char* pVertexData);
        void ResetCameraLite(MdlNode* pNode, unsigned char* pModelData,
                             unsigned char* pVertexData);
        void ResetEmitterLite(MdlNode* pNode, unsigned char* pModelData,
                              unsigned char* pVertexData);
        void ResetReferenceLite(MdlNode* pNode, unsigned char* pModelData,
                                unsigned char* pVertexData);
        void ResetTriggerLite(MdlNode* pNode, unsigned char* pModelData,
                              unsigned char* pVertexData);
        void ResetLightsaberLite(MdlNodeLightsaber* pNode,
                                 unsigned char* pModelData,
                                 unsigned char* pVertexData);
        void ResetLightsaber(MdlNodeLightsaber* pNode,
                             unsigned char* pModelData,
                             unsigned char* pVertexData);
        void ResetAABBTree(AABB* pNode, unsigned char* pModelData,
                           unsigned char* pVertexData);
#ifdef _XBOX

        void ResetBase(MdlNode* pNode, unsigned char* pModelData, unsigned char* pVertexData);
        void ResetCamera(MdlNode* pNode, unsigned char* pModelData, unsigned char* pVertexData);
        void ResetEmitter(MdlNode* pNode, unsigned char* pModelData, unsigned char* pVertexData);
        void ResetReference(MdlNode* pNode, unsigned char* pModelData, unsigned char* pVertexData);
        void ResetTrigger(MdlNode* pNode, unsigned char* pModelData, unsigned char* pVertexData);
        void ResetTriMesh(MdlNodeTriMesh* pNode, unsigned char* pModelData,
                          unsigned char* pVertexData);
        void ResetAABBLite(MdlNodeAABB* pNode, unsigned char* pModelData,
                           unsigned char* pVertexData);
        void ResetAnimLite(MdlNodeAnimMesh* pNode, unsigned char* pModelData,
                           unsigned char* pVertexData);
        void ResetDanglyLite(MdlNodeDanglyMesh* pNode, unsigned char* pModelData,
                             unsigned char* pVertexData);
        void ResetSkinLite(MdlNodeSkin* pNode, unsigned char* pModelData,
                           unsigned char* pVertexData);
        void ResetTriMeshLite(MdlNodeTriMesh* pNode, unsigned char* pModelData,
                              unsigned char* pVertexData);
        void ResetAnimationLite(Animation* pAnimation, unsigned char* pModelData,
                                unsigned char* pVertexData);
#endif
    };

    MaxTree* Read(char* sFileName);
    MaxTree* Read(FILE* pModelFile, FILE* pVertexFile);

    InputBinary m_InputBinary;
    void* m_pResource;
};

void Input::InputBinary::ResetAnimation(Animation* pAnimation,
                                        unsigned char* pModelData,
                                        unsigned char* pVertexData)
{
    pAnimation->m_pBinaryData = pModelData;
    if (pAnimation->m_lEvents.m_nCount != 0)
    {
        pAnimation->m_lEvents.m_pList =
            (AurKey<char[32]>*)(pModelData + (unsigned int)pAnimation->m_lEvents.m_pList);
    }
    else
    {
        pAnimation->m_lEvents.m_pList = 0;
    }

    pAnimation->m_pRootNode =
        (MdlNode*)(pModelData + (unsigned int)pAnimation->m_pRootNode);
    ResetMdlNode(pAnimation->m_pRootNode, pModelData, pVertexData);
    UpdateAnimFootprint(pAnimation->m_pRootNode, 1);
    animationfootprint += 140;
    animationfootprint += 36 * pAnimation->m_lEvents.m_nCount;
    pAnimation->m_pfnDestructor = MaxTreeDestructor;
    pAnimation->m_pfnParseField = MaxTreeParseField;
}

MaxTree* Input::InputBinary::Reset(unsigned char* pModelData,
                                  unsigned char* pVertexData, void* pResource)
{
    Model* pModel = (Model*)pModelData;
    pModel->m_pModelResource = pResource;
    pModel->m_nType = pModel->GetType() | MaxTree::BINARY_TYPE_FLAG;
    pModel->m_pParentModel = FindModel(pModel->m_sSuperModel);
    if (pModel->m_pParentModel)
    {
        pModel->m_pParentModel->AddRef(pModel);
    }
    pModel->ResetFreeLists();

#ifndef _XBOX
    int nVertexOffset = pModel->m_nVertexPool;
    GLRender::RequestPool(Aurora::POOL_VERTEX, pModel->m_nVertexDataSize, 2, 0,
                          &pModel->m_nVertexPool);
    void* pVertices = GLRender::LockPool(pModel->m_nVertexPool, 0, 0, RequestPoolManager::POOL_LOCK_WRITE);
    memcpy(pVertices, pVertexData + nVertexOffset, pModel->m_nVertexDataSize);
    GLRender::UnlockPool(pModel->m_nVertexPool);
    pModel->AddToFreeTokenList(pModel->m_nVertexPool);
#endif

    if (pModel->m_lNames.m_nCount != 0)
    {
        pModel->m_lNames.m_pList =
            (char**)(pModelData + (unsigned int)pModel->m_lNames.m_pList);
    }
    else
    {
        pModel->m_lNames.m_pList = 0;
    }
    for (int i = 0; i < pModel->m_lNames.m_nCount; ++i)
    {
        pModel->m_lNames.m_pList[i] =
            (char*)(pModelData + (unsigned int)pModel->m_lNames.m_pList[i]);
    }

    if (pModel->m_lAnimations.m_nCount != 0)
    {
        pModel->m_lAnimations.m_pList =
            (Animation**)(pModelData + (unsigned int)pModel->m_lAnimations.m_pList);
    }
    else
    {
        pModel->m_lAnimations.m_pList = 0;
    }
    for (int i = 0; i < pModel->m_lAnimations.m_nCount; ++i)
    {
        pModel->m_lAnimations.m_pList[i] =
            (Animation*)(pModelData + (unsigned int)pModel->m_lAnimations.m_pList[i]);
        ResetAnimation(pModel->m_lAnimations.m_pList[i], pModelData, pVertexData);
    }

    if (pModel->m_pBinaryData != 0)
    {
        pModel->m_pBinaryData = pModelData + (unsigned int)pModel->m_pBinaryData;
    }
    pModel->m_pRootNode =
        (MdlNode*)(pModelData + (unsigned int)pModel->m_pRootNode);
    ResetMdlNode(pModel->m_pRootNode, pModelData, pVertexData);
    pModel->m_pfnDestructor = ModelDestructor;
    pModel->m_pfnParseField = ModelParseField;
    InsertModel(pModel);
    return pModel;
}

#ifdef _XBOX

MaxTree* Input::InputBinary::Read(void* pModelResource,
                                 void* pVertexResource, bool bLite)
{
    unsigned char* pData = (unsigned char*)AurResGetDataBytes(4, pModelResource);
    if (pData == 0)
    {
        return 0;
    }
    m_nModelDataSize = *(unsigned long*)pData;
    AurResFreeDataBytes(pModelResource, pData);
    pData = (unsigned char*)AurResGetDataBytes(4, pModelResource);
    m_nVertexDataSize = *(unsigned long*)pData;
    AurResFreeDataBytes(pModelResource, pData);

    unsigned char* pModelData = (unsigned char*)AurResGetDataBytes(m_nModelDataSize, pModelResource);
    AurResFreeDataBytes(pModelResource, pModelData);

    if (m_nVertexDataSize > 0)
    {
        pData = (unsigned char*)AurResGetDataBytes(m_nVertexDataSize, pVertexResource);
    }
    else
    {
        pData = 0;
    }
    unsigned char* pVertexData = 0;
    unsigned long nVertexPool = RequestPoolManager::INVALID_POOL;
    if (pData != 0)
    {
        D3DVertexBuffer* pBuffer = new D3DVertexBuffer;
        XGSetVertexBufferHeader(0, 0, 0, 0, pBuffer, 0);
        pBuffer->Register(pData);
        nVertexPool = GLRender::AddVertexPool(pBuffer, m_nVertexDataSize);
        pVertexData = pData;
    }

    MaxTree* pTree;
    if (!bLite)
    {
        pTree = Reset(pModelData, pVertexData, pModelResource);
    }
    else
    {
        pTree = ResetLite(pModelData, pVertexData, pModelResource);
    }
    pTree->AsModel()->m_nVertexPool = nVertexPool;
    if (nVertexPool != RequestPoolManager::INVALID_POOL)
    {
        pTree->AsModel()->AddToFreeTokenList(pTree->AsModel()->m_nVertexPool);
    }
    pTree->AsModel()->m_pVertexResource = pVertexResource;
    pTree->AsModel()->m_pModelResource = pModelResource;
    AurResRemove(pModelResource);
    AurResRemove(pVertexResource);
    return pTree;
}

void Input::InputBinary::ResetAnimationLite(Animation* pAnimation, unsigned char* pModelData,
                                            unsigned char* pVertexData)
{
    ResetMdlNodeLite(pAnimation->m_pRootNode, pModelData, pVertexData);
    UpdateAnimFootprint(pAnimation->m_pRootNode, 1);
    animationfootprint += 140;
    animationfootprint += 36 * pAnimation->m_lEvents.m_nCount;
}

MaxTree* Input::InputBinary::ResetLite(unsigned char* pModelData,
                                      unsigned char* pVertexData,
                                      void* pResource)
{
    Model* pModel = (Model*)pModelData;
    pModel->m_pParentModel = FindModel(pModel->m_sSuperModel);
    if (pModel->m_pParentModel)
    {
        pModel->m_pParentModel->AddRef(pModel);
    }
    pModel->ResetFreeLists();

    for (int i = 0; i < pModel->m_lAnimations.m_nCount; ++i)
    {
        ResetAnimationLite(pModel->m_lAnimations.m_pList[i], pModelData, pVertexData);
    }

    ResetMdlNodeLite(pModel->m_pRootNode, pModelData, pVertexData);
    InsertModel(pModel);
    return pModel;
}
#else
MaxTree* Input::InputBinary::Read(void* pModelResource,
                                 void* pVertexResource, bool bLite)
{
    unsigned char* pData = (unsigned char*)AurResGetDataBytes(4, pModelResource);
    if (pData == 0)
    {
        return 0;
    }
    m_nModelDataSize = *(unsigned long*)pData;
    AurResFreeDataBytes(pModelResource, pData);
    pData = (unsigned char*)AurResGetDataBytes(4, pModelResource);
    m_nVertexDataSize = *(unsigned long*)pData;
    AurResFreeDataBytes(pModelResource, pData);

    pData = (unsigned char*)AurResGetDataBytes(m_nModelDataSize, pModelResource);
    unsigned char* pModelData = new unsigned char[m_nModelDataSize];
    memcpy(pModelData, pData, m_nModelDataSize);
    AurResFreeDataBytes(pModelResource, pData);

    unsigned char* pVertexData = 0;
    if (m_nVertexDataSize > 0)
    {
        pData = (unsigned char*)AurResGetDataBytes(m_nVertexDataSize, pVertexResource);
    }
    else
    {
        pData = 0;
    }
    if (m_nVertexDataSize != 0)
    {
        pVertexData = new unsigned char[m_nVertexDataSize];
        memcpy(pVertexData, pData, m_nVertexDataSize);
    }
    AurResFreeDataBytes(pModelResource, pData);

    MaxTree* pTree;
    if (!bLite)
    {
        pTree = Reset(pModelData, pVertexData, pModelResource);
    }
    else
    {
        pTree = ResetLite(pModelData, pVertexData, pModelResource);
    }
    pTree->AsModel()->m_pVertexResource = pVertexResource;
    pTree->AsModel()->m_pModelResource = pModelResource;
    AurResFree(pModelResource, 0);
    AurResRemove(pVertexResource);
    delete[] pVertexData;
    return pTree;
}

MaxTree* Input::InputBinary::ResetLite(unsigned char* pModelData,
                                      unsigned char* pVertexData,
                                      void* pResource)
{
    Model* pModel = (Model*)pModelData;
    pModel->m_pParentModel = FindModel(pModel->m_sSuperModel);
    if (pModel->m_pParentModel)
    {
        pModel->m_pParentModel->AddRef(pModel);
    }
    pModel->ResetFreeLists();

    for (int i = 0; i < pModel->m_lAnimations.m_nCount; ++i)
    {
        Animation* pAnimation = pModel->m_lAnimations.m_pList[i];
        ResetMdlNodeLite(pAnimation->m_pRootNode, pModelData, pVertexData);
        UpdateAnimFootprint(pAnimation->m_pRootNode, 1);
        animationfootprint += 140;
        animationfootprint += 36 * pAnimation->m_lEvents.m_nCount;
    }

    ResetMdlNodeLite(pModel->m_pRootNode, pModelData, pVertexData);
    InsertModel(pModel);
    return pModel;
}
#endif

void Input::InputBinary::ResetAnim(MdlNodeAnimMesh* pNode,
                                   unsigned char* pModelData,
                                   unsigned char* pVertexData)
{
    if (pNode->m_lAnimationVectors1.m_nCount != 0)
    {
        pNode->m_lAnimationVectors1.m_pList =
            (Vector*)(pModelData +
                      (unsigned int)pNode->m_lAnimationVectors1.m_pList);
    }
    else
    {
        pNode->m_lAnimationVectors1.m_pList = 0;
    }
    if (pNode->m_lAnimationVectors2.m_nCount != 0)
    {
        pNode->m_lAnimationVectors2.m_pList =
            (Vector*)(pModelData +
                      (unsigned int)pNode->m_lAnimationVectors2.m_pList);
    }
    else
    {
        pNode->m_lAnimationVectors2.m_pList = 0;
    }
    if (pNode->m_lAnimationVectors3.m_nCount != 0)
    {
        pNode->m_lAnimationVectors3.m_pList =
            (Vector*)(pModelData +
                      (unsigned int)pNode->m_lAnimationVectors3.m_pList);
    }
    else
    {
        pNode->m_lAnimationVectors3.m_pList = 0;
    }
    if (pNode->m_nAnimationDataCount1 != 0)
    {
        pNode->m_pAnimationData1 =
            pModelData + (unsigned int)pNode->m_pAnimationData1;
    }
    else
    {
        pNode->m_pAnimationData1 = 0;
    }
    if (pNode->m_nAnimationDataCount2 != 0)
    {
        pNode->m_pAnimationData2 =
            pModelData + (unsigned int)pNode->m_pAnimationData2;
    }
    else
    {
        pNode->m_pAnimationData2 = 0;
    }
    pNode->m_pfnGenVertices = MdlNodeAnimMeshGenVertices;
    pNode->m_pfnRemoveTemporaryArrays = MdlNodeAnimMeshRemoveTemporaryArrays;
    ResetTriMeshParts(pNode, pModelData, pVertexData);
}

void Input::InputBinary::ResetDangly(MdlNodeDanglyMesh* pNode,
                                     unsigned char* pModelData,
                                     unsigned char* pVertexData)
{
    ResetTriMeshParts(pNode, pModelData, pVertexData);
    pNode->m_pfnGenVertices = MdlNodeDanglyMeshGenVertices;
    pNode->m_pfnRemoveTemporaryArrays = MdlNodeDanglyMeshRemoveTemporaryArrays;

    if (pNode->m_lConstraints.m_nCount != 0)
    {
        pNode->m_lConstraints.m_pList =
            (float*)(pModelData +
                     (unsigned int)pNode->m_lConstraints.m_pList);
    }
    else
    {
        pNode->m_lConstraints.m_pList = 0;
    }

    if (pNode->m_nVertexCount > 0)
    {
        pNode->m_pVertexPositions =
            (Vector*)(pModelData + (unsigned int)pNode->m_pVertexPositions);
    }
}

void Input::InputBinary::ResetBaseLite(MdlNode* pNode,
                                    unsigned char* pModelData,
                                    unsigned char* pVertexData)
#ifdef _XBOX
{
    ResetMdlNodePartsLite(pNode, pModelData, pVertexData);
}
#else
{
    int nChildren = pNode->m_lChildren.m_nCount;
    if (nChildren != 0)
    {
        for (int i = 0; i < nChildren; ++i)
        {
            ResetMdlNodeLite(pNode->m_lChildren.m_pList[i], pModelData, pVertexData);
            nChildren = pNode->m_lChildren.m_nCount;
        }
    }
}
#endif

void Input::InputBinary::ResetSkin(MdlNodeSkin* pNode,
                                   unsigned char* pModelData,
                                   unsigned char* pVertexData)
{
    ResetTriMeshParts(pNode, pModelData, pVertexData);
    pNode->m_pfnGenVertices = MdlNodeSkinGenVertices;
    pNode->m_pfnRemoveTemporaryArrays = MdlNodeSkinRemoveTemporaryArrays;

    if (pNode->m_nBoneMap != 0)
    {
#ifdef _XBOX
        pNode->m_pfBoneMap =
            (short*)(pModelData + (unsigned int)pNode->m_pfBoneMap);
#else
        pNode->m_pfBoneMap =
            (float*)(pModelData + (unsigned int)pNode->m_pfBoneMap);
#endif
    }
    else
    {
        pNode->m_pfBoneMap = 0;
    }

    if (pNode->m_lstQBoneRefInv.m_nCount != 0)
    {
        pNode->m_lstQBoneRefInv.m_pList =
            (Quaternion*)(pModelData +
                          (unsigned int)pNode->m_lstQBoneRefInv.m_pList);
    }
    else
    {
        pNode->m_lstQBoneRefInv.m_pList = 0;
    }

    if (pNode->m_lstTBoneRefInv.m_nCount != 0)
    {
        pNode->m_lstTBoneRefInv.m_pList =
            (Vector*)(pModelData +
                      (unsigned int)pNode->m_lstTBoneRefInv.m_pList);
    }
    else
    {
        pNode->m_lstTBoneRefInv.m_pList = 0;
    }

    if (pNode->m_lstBoneConstantIndices.m_nCount != 0)
    {
        pNode->m_lstBoneConstantIndices.m_pList =
            (int*)(pModelData +
                   (unsigned int)pNode->m_lstBoneConstantIndices.m_pList);
    }
    else
    {
        pNode->m_lstBoneConstantIndices.m_pList = 0;
    }
}

void Input::InputBinary::ResetAABB(MdlNodeAABB* pNode,
                                   unsigned char* pModelData,
                                   unsigned char* pVertexData)
{
#ifdef _XBOX
    pNode->m_pAABBData =
        (AABB*)(pModelData + (unsigned int)pNode->m_pAABBData);
    ResetAABBTree(pNode->m_pAABBData, pModelData, pVertexData);
#endif
    pNode->m_pfnGenVertices = MdlNodeTriMeshGenVertices;
    pNode->m_pfnRemoveTemporaryArrays = MdlNodeTriMeshRemoveTemporaryArrays;
    ResetTriMeshParts(pNode, pModelData, pVertexData);
}

void Input::InputBinary::ResetAABBTree(AABB* pNode, unsigned char* pModelData,
                                       unsigned char* pVertexData)
#ifdef _XBOX
{
    if (pNode->m_pLeftChild)
    {
        pNode->m_pLeftChild = (AABB*)(pModelData + (unsigned int)pNode->m_pLeftChild);
        ResetAABBTree(pNode->m_pLeftChild, pModelData, pVertexData);
    }
    if (pNode->m_pRightChild)
    {
        pNode->m_pRightChild = (AABB*)(pModelData + (unsigned int)pNode->m_pRightChild);
        ResetAABBTree(pNode->m_pRightChild, pModelData, pVertexData);
    }
}
#else
{
    if (pNode->m_pLeftChild)
    {
        AABB* pLeft = (AABB*)(pModelData + (unsigned int)pNode->m_pLeftChild);
        pNode->m_pLeftChild = pLeft;
        if (pLeft->m_pLeftChild)
        {
            AABB* pLeftLeft =
                (AABB*)(pModelData + (unsigned int)pLeft->m_pLeftChild);
            pLeft->m_pLeftChild = pLeftLeft;
            if (pLeftLeft->m_pLeftChild)
            {
                AABB* pBranch =
                    (AABB*)(pModelData + (unsigned int)pLeftLeft->m_pLeftChild);
                pLeftLeft->m_pLeftChild = pBranch;
                if (pBranch->m_pLeftChild)
                {
                    pBranch->m_pLeftChild =
                        (AABB*)(pModelData +
                                (unsigned int)pBranch->m_pLeftChild);
                    ResetAABBTree(pBranch->m_pLeftChild, pModelData,
                                  pVertexData);
                }
                if (pBranch->m_pRightChild)
                {
                    pBranch->m_pRightChild =
                        (AABB*)(pModelData +
                                (unsigned int)pBranch->m_pRightChild);
                    ResetAABBTree(pBranch->m_pRightChild, pModelData,
                                  pVertexData);
                }
            }
            if (pLeftLeft->m_pRightChild)
            {
                AABB* pBranch = (AABB*)(pModelData +
                                        (unsigned int)pLeftLeft->m_pRightChild);
                pLeftLeft->m_pRightChild = pBranch;
                if (pBranch->m_pLeftChild)
                {
                    pBranch->m_pLeftChild =
                        (AABB*)(pModelData +
                                (unsigned int)pBranch->m_pLeftChild);
                    ResetAABBTree(pBranch->m_pLeftChild, pModelData,
                                  pVertexData);
                }
                if (pBranch->m_pRightChild)
                {
                    pBranch->m_pRightChild =
                        (AABB*)(pModelData +
                                (unsigned int)pBranch->m_pRightChild);
                    ResetAABBTree(pBranch->m_pRightChild, pModelData,
                                  pVertexData);
                }
            }
        }
        if (pLeft->m_pRightChild)
        {
            AABB* pBranch =
                (AABB*)(pModelData + (unsigned int)pLeft->m_pRightChild);
            pLeft->m_pRightChild = pBranch;
            if (pBranch->m_pLeftChild)
            {
                pBranch->m_pLeftChild =
                    (AABB*)(pModelData + (unsigned int)pBranch->m_pLeftChild);
                ResetAABBTree(pBranch->m_pLeftChild, pModelData, pVertexData);
            }
            if (pBranch->m_pRightChild)
            {
                pBranch->m_pRightChild =
                    (AABB*)(pModelData + (unsigned int)pBranch->m_pRightChild);
                ResetAABBTree(pBranch->m_pRightChild, pModelData, pVertexData);
            }
        }
    }
    if (pNode->m_pRightChild)
    {
        AABB* pRight = (AABB*)(pModelData + (unsigned int)pNode->m_pRightChild);
        pNode->m_pRightChild = pRight;
        if (pRight->m_pLeftChild)
        {
            AABB* pRightLeft =
                (AABB*)(pModelData + (unsigned int)pRight->m_pLeftChild);
            pRight->m_pLeftChild = pRightLeft;
            if (pRightLeft->m_pLeftChild)
            {
                AABB* pBranch = (AABB*)(pModelData +
                                        (unsigned int)pRightLeft->m_pLeftChild);
                pRightLeft->m_pLeftChild = pBranch;
                if (pBranch->m_pLeftChild)
                {
                    pBranch->m_pLeftChild =
                        (AABB*)(pModelData +
                                (unsigned int)pBranch->m_pLeftChild);
                    ResetAABBTree(pBranch->m_pLeftChild, pModelData,
                                  pVertexData);
                }
                if (pBranch->m_pRightChild)
                {
                    pBranch->m_pRightChild =
                        (AABB*)(pModelData +
                                (unsigned int)pBranch->m_pRightChild);
                    ResetAABBTree(pBranch->m_pRightChild, pModelData,
                                  pVertexData);
                }
            }
            if (pRightLeft->m_pRightChild)
            {
                AABB* pBranch =
                    (AABB*)(pModelData +
                            (unsigned int)pRightLeft->m_pRightChild);
                pRightLeft->m_pRightChild = pBranch;
                if (pBranch->m_pLeftChild)
                {
                    pBranch->m_pLeftChild =
                        (AABB*)(pModelData +
                                (unsigned int)pBranch->m_pLeftChild);
                    ResetAABBTree(pBranch->m_pLeftChild, pModelData,
                                  pVertexData);
                }
                if (pBranch->m_pRightChild)
                {
                    pBranch->m_pRightChild =
                        (AABB*)(pModelData +
                                (unsigned int)pBranch->m_pRightChild);
                    ResetAABBTree(pBranch->m_pRightChild, pModelData,
                                  pVertexData);
                }
            }
        }
        if (pRight->m_pRightChild)
        {
            AABB* pBranch =
                (AABB*)(pModelData + (unsigned int)pRight->m_pRightChild);
            pRight->m_pRightChild = pBranch;
            if (pBranch->m_pLeftChild)
            {
                pBranch->m_pLeftChild =
                    (AABB*)(pModelData + (unsigned int)pBranch->m_pLeftChild);
                ResetAABBTree(pBranch->m_pLeftChild, pModelData, pVertexData);
            }
            if (pBranch->m_pRightChild)
            {
                pBranch->m_pRightChild =
                    (AABB*)(pModelData + (unsigned int)pBranch->m_pRightChild);
                ResetAABBTree(pBranch->m_pRightChild, pModelData, pVertexData);
            }
        }
    }
}
#endif

void Input::InputBinary::ResetLightsaber(MdlNodeLightsaber* pNode,
                                        unsigned char* pModelData,
                                        unsigned char* pVertexData)
{
    ResetTriMeshParts(pNode, pModelData, pVertexData);
    pNode->m_pfnGenVertices = MdlNodeTriMeshGenVertices;
    pNode->m_pfnRemoveTemporaryArrays = MdlNodeTriMeshRemoveTemporaryArrays;
    GLRender::RequestPool(Aurora::POOL_VERTEX, MdlNodeLightsaber::NUM_PIECE_VERTS * 32, 2,
                          MdlNodeLightsaber::BLADE_POOL_FLAGS,
                          &pNode->m_nBladePool);
    GLRender::RequestPool(Aurora::POOL_VERTEX, MdlNodeLightsaber::NUM_SEGMENT_VERTS *
                                 MdlNodeLightsaber::NUM_BACK_SEGMENTS * 20,
                          2, MdlNodeLightsaber::GLOW_POOL_FLAGS,
                          &pNode->m_nGlowPool);
    pNode->m_pVertexPositions =
        (Vector*)(pModelData + (unsigned int)pNode->m_pVertexPositions);
    pNode->m_pTextureCoordinates =
        (float*)(pModelData + (unsigned int)pNode->m_pTextureCoordinates);
    pNode->m_pVertexNormals =
        (Vector*)(pModelData + (unsigned int)pNode->m_pVertexNormals);
}

void Input::InputBinary::ResetLightsaberLite(MdlNodeLightsaber* pNode,
                                             unsigned char* pModelData,
                                             unsigned char* pVertexData)
{
    ResetTriMeshPartsLite(pNode, pModelData, pVertexData);
    GLRender::RequestPool(Aurora::POOL_VERTEX, MdlNodeLightsaber::NUM_PIECE_VERTS * 32, 2,
                          4114, &pNode->m_nBladePool);
    GLRender::RequestPool(Aurora::POOL_VERTEX, MdlNodeLightsaber::NUM_SEGMENT_VERTS *
                                 MdlNodeLightsaber::NUM_BACK_SEGMENTS *
                                 20,
                          2, 4112, &pNode->m_nGlowPool);
}

}

IODispatcher::IODispatcher() : m_lDispatchEntries(0)
{
}
// PC 0x004CD870 GOG 0x004CDA00
IODispatcher* IODispatcher::GetRef()
{
    static IODispatcher ioDispatch;

    return &ioDispatch;
}
// PC 0x004CE990 GOG 0x004CEAD0
MaxTree* IODispatcher::ReadSync(char* sFileName)
{
    Input input;

    input.m_pResource = 0;
    return input.Read(sFileName);
}
// PC 0x004CE9B0 GOG 0x004CEAF0
MaxTree* IODispatcher::ReadSync(FILE* pModelFile, FILE* pVertexFile)
{
    Input input;

    input.m_pResource = 0;
    return input.Read(pModelFile, pVertexFile);
}

static const int WRITE_PATH_LENGTH = 256;

#ifndef _XBOX
bool IODispatcher::WriteSync(MaxTree* pTree, char* sFileName)
{
    char sPath[WRITE_PATH_LENGTH];

    sprintf(sPath, sFileName ? "d:\\override\\%s" : "d:\\override\\%s.mdl",
            sFileName ? sFileName : pTree->m_sName);
    fclose(fopen(sPath, "wb"));
    return false;
}
#endif

namespace
{

void Input::InputBinary::ResetLight(MdlNodeLight* pNode,
                                   unsigned char* pModelData,
                                   unsigned char* pVertexData)
{
    ResetMdlNodeParts(pNode, pModelData, pVertexData);
    if (pNode->m_lstTextureNames.m_nCount != 0)
    {
        pNode->m_lstTextureNames.m_pList =
            (char**)(pModelData + (unsigned int)pNode->m_lstTextureNames.m_pList);
        for (int i = 0; i < pNode->m_lstTextureNames.m_nCount; ++i)
        {
            pNode->m_lstTextureNames.m_pList[i] =
                (char*)(pModelData + (unsigned int)pNode->m_lstTextureNames.m_pList[i]);
        }
    }
    else
    {
        pNode->m_lstTextureNames.m_pList = 0;
    }
    if (pNode->m_lstFlareSizes.m_nCount != 0)
    {
        pNode->m_lstFlareSizes.m_pList =
            (float*)(pModelData + (unsigned int)pNode->m_lstFlareSizes.m_pList);
    }
    else
    {
        pNode->m_lstFlareSizes.m_pList = 0;
    }
    if (pNode->m_lstFlarePositions.m_nCount != 0)
    {
        pNode->m_lstFlarePositions.m_pList =
            (float*)(pModelData + (unsigned int)pNode->m_lstFlarePositions.m_pList);
    }
    else
    {
        pNode->m_lstFlarePositions.m_pList = 0;
    }
    if (pNode->m_lstFlareColorShifts.m_nCount != 0)
    {
        pNode->m_lstFlareColorShifts.m_pList =
            (Vector*)(pModelData + (unsigned int)pNode->m_lstFlareColorShifts.m_pList);
    }
    else
    {
        pNode->m_lstFlareColorShifts.m_pList = 0;
    }
    pNode->m_lstTextures.SetSize(pNode->m_lstTextureNames.m_nCount);
    int nTextures = pNode->m_lstTextureNames.m_nCount;
    while (nTextures != 0)
    {
        --nTextures;
        pNode->m_lstTextures[nTextures] =
            AurTextureGetReference(pNode->m_lstTextureNames.m_pList[nTextures], 0);
    }
    pNode->AddToFreeList(pNode);
}

void Input::InputBinary::ResetLightLite(MdlNodeLight* pNode,
                                      unsigned char* pModelData,
                                      unsigned char* pVertexData)
{
    ResetMdlNodePartsLite(pNode, pModelData, pVertexData);
    pNode->m_lstTextures.SetSize(pNode->m_lstTextureNames.m_nCount);
    int nTextures = pNode->m_lstTextureNames.m_nCount;
    while (nTextures != 0)
    {
        --nTextures;
        pNode->m_lstTextures[nTextures] =
            AurTextureGetReference(pNode->m_lstTextureNames.m_pList[nTextures], 0);
    }
    pNode->AddToFreeList(pNode);
}

#ifdef _XBOX
void Input::InputBinary::ResetMdlNode(MdlNode* pNode,
                                     unsigned char* pModelData,
                                     unsigned char* pVertexData)
{
    if (pNode->m_nNodeType == MdlNode::MDLNODE_AABB_ID)
        return ResetAABB((MdlNodeAABB*)pNode, pModelData, pVertexData);
    if (pNode->m_nNodeType == MdlNode::MDLNODE_ANIM_ID)
        return ResetAnim((MdlNodeAnimMesh*)pNode, pModelData, pVertexData);
    if (pNode->m_nNodeType == MdlNode::MDLNODE_BASE_ID)
        return ResetBase(pNode, pModelData, pVertexData);
    if (pNode->m_nNodeType == MdlNode::MDLNODE_CAMERA_ID)
        return ResetCamera(pNode, pModelData, pVertexData);
    if (pNode->m_nNodeType == MdlNode::MDLNODE_DANGLY_ID)
        return ResetDangly((MdlNodeDanglyMesh*)pNode, pModelData, pVertexData);
    if (pNode->m_nNodeType == MdlNode::MDLNODE_EMITTER_ID)
        return ResetEmitter(pNode, pModelData, pVertexData);
    if (pNode->m_nNodeType == MdlNode::MDLNODE_LIGHT_ID)
        return ResetLight((MdlNodeLight*)pNode, pModelData, pVertexData);
    if (pNode->m_nNodeType == MdlNode::MDLNODE_REFERENCE_ID)
        return ResetReference(pNode, pModelData, pVertexData);
    if (pNode->m_nNodeType == MdlNode::MDLNODE_SKIN_ID)
        return ResetSkin((MdlNodeSkin*)pNode, pModelData, pVertexData);
    if (pNode->m_nNodeType == MdlNode::MDLNODE_TRIGGER_ID)
        return ResetTrigger(pNode, pModelData, pVertexData);
    if (pNode->m_nNodeType == MdlNode::MDLNODE_LIGHTSABER_ID)
        return ResetLightsaber((MdlNodeLightsaber*)pNode, pModelData, pVertexData);
    if (pNode->m_nNodeType == MdlNode::MDLNODE_TRIMESH_ID)
        return ResetTriMesh((MdlNodeTriMesh*)pNode, pModelData, pVertexData);
}

void Input::InputBinary::ResetMdlNodeLite(MdlNode* pNode,
                                       unsigned char* pModelData,
                                       unsigned char* pVertexData)
{
    if (pNode->m_nNodeType == MdlNode::MDLNODE_AABB_ID)
        return ResetAABBLite((MdlNodeAABB*)pNode, pModelData, pVertexData);
    if (pNode->m_nNodeType == MdlNode::MDLNODE_ANIM_ID)
        return ResetAnimLite((MdlNodeAnimMesh*)pNode, pModelData, pVertexData);
    if (pNode->m_nNodeType == MdlNode::MDLNODE_BASE_ID)
        return ResetBaseLite(pNode, pModelData, pVertexData);
    if (pNode->m_nNodeType == MdlNode::MDLNODE_CAMERA_ID)
        return ResetCameraLite(pNode, pModelData, pVertexData);
    if (pNode->m_nNodeType == MdlNode::MDLNODE_DANGLY_ID)
        return ResetDanglyLite((MdlNodeDanglyMesh*)pNode, pModelData, pVertexData);
    if (pNode->m_nNodeType == MdlNode::MDLNODE_EMITTER_ID)
        return ResetEmitterLite(pNode, pModelData, pVertexData);
    if (pNode->m_nNodeType == MdlNode::MDLNODE_LIGHT_ID)
        return ResetLightLite((MdlNodeLight*)pNode, pModelData, pVertexData);
    if (pNode->m_nNodeType == MdlNode::MDLNODE_REFERENCE_ID)
        return ResetReferenceLite(pNode, pModelData, pVertexData);
    if (pNode->m_nNodeType == MdlNode::MDLNODE_SKIN_ID)
        return ResetSkinLite((MdlNodeSkin*)pNode, pModelData, pVertexData);
    if (pNode->m_nNodeType == MdlNode::MDLNODE_TRIGGER_ID)
        return ResetTriggerLite(pNode, pModelData, pVertexData);
    if (pNode->m_nNodeType == MdlNode::MDLNODE_LIGHTSABER_ID)
        return ResetLightsaberLite((MdlNodeLightsaber*)pNode, pModelData, pVertexData);
    if (pNode->m_nNodeType == MdlNode::MDLNODE_TRIMESH_ID)
        return ResetTriMeshLite((MdlNodeTriMesh*)pNode, pModelData, pVertexData);
}
#else
void Input::InputBinary::ResetMdlNode(MdlNode* pNode,
                                     unsigned char* pModelData,
                                     unsigned char* pVertexData)
{
    if (pNode->m_nNodeType == MdlNode::MDLNODE_AABB_ID)
    {
        MdlNodeAABB* pAABB = (MdlNodeAABB*)pNode;
        pAABB->m_pAABBData =
            (AABB*)(pModelData + (unsigned int)pAABB->m_pAABBData);
        ResetAABBTree(pAABB->m_pAABBData, pModelData, pVertexData);
        pAABB->m_pfnGenVertices = MdlNodeTriMeshGenVertices;
        pAABB->m_pfnRemoveTemporaryArrays = MdlNodeTriMeshRemoveTemporaryArrays;
        ResetTriMeshParts(pAABB, pModelData, pVertexData);
    }
    else if (pNode->m_nNodeType == MdlNode::MDLNODE_ANIM_ID)
    {
        ResetAnim((MdlNodeAnimMesh*)pNode, pModelData, pVertexData);
    }
    else if (pNode->m_nNodeType == MdlNode::MDLNODE_BASE_ID)
    {
        ResetMdlNodeParts(pNode, pModelData, pVertexData);
    }
    else if (pNode->m_nNodeType == MdlNode::MDLNODE_CAMERA_ID)
    {
        ResetMdlNodeParts(pNode, pModelData, pVertexData);
    }
    else if (pNode->m_nNodeType == MdlNode::MDLNODE_DANGLY_ID)
    {
        ResetDangly((MdlNodeDanglyMesh*)pNode, pModelData, pVertexData);
    }
    else if (pNode->m_nNodeType == MdlNode::MDLNODE_EMITTER_ID)
    {
        ResetMdlNodeParts(pNode, pModelData, pVertexData);
    }
    else if (pNode->m_nNodeType == MdlNode::MDLNODE_LIGHT_ID)
    {
        ResetLight((MdlNodeLight*)pNode, pModelData, pVertexData);
    }
    else if (pNode->m_nNodeType == MdlNode::MDLNODE_REFERENCE_ID)
    {
        ResetMdlNodeParts(pNode, pModelData, pVertexData);
    }
    else if (pNode->m_nNodeType == MdlNode::MDLNODE_SKIN_ID)
    {
        ResetSkin((MdlNodeSkin*)pNode, pModelData, pVertexData);
    }
    else if (pNode->m_nNodeType == MdlNode::MDLNODE_TRIGGER_ID)
    {
        ResetMdlNodeParts(pNode, pModelData, pVertexData);
    }
    else if (pNode->m_nNodeType == MdlNode::MDLNODE_LIGHTSABER_ID)
    {
        ResetLightsaber((MdlNodeLightsaber*)pNode, pModelData, pVertexData);
    }
    else if (pNode->m_nNodeType == MdlNode::MDLNODE_TRIMESH_ID)
    {
        ResetAABB((MdlNodeAABB*)pNode, pModelData, pVertexData);
    }
}

void Input::InputBinary::ResetMdlNodeLite(MdlNode* pNode,
                                       unsigned char* pModelData,
                                       unsigned char* pVertexData)
{
    if (pNode->m_nNodeType == MdlNode::MDLNODE_AABB_ID ||
        pNode->m_nNodeType == MdlNode::MDLNODE_ANIM_ID)
        return ResetTriMeshPartsLite((MdlNodeTriMesh*)pNode, pModelData, pVertexData);
    if (pNode->m_nNodeType == MdlNode::MDLNODE_BASE_ID)
        return ResetBaseLite(pNode, pModelData, pVertexData);
    if (pNode->m_nNodeType == MdlNode::MDLNODE_CAMERA_ID)
        return ResetCameraLite(pNode, pModelData, pVertexData);
    if (pNode->m_nNodeType == MdlNode::MDLNODE_DANGLY_ID)
        return ResetTriMeshPartsLite((MdlNodeTriMesh*)pNode, pModelData, pVertexData);
    if (pNode->m_nNodeType == MdlNode::MDLNODE_EMITTER_ID)
        return ResetEmitterLite(pNode, pModelData, pVertexData);
    if (pNode->m_nNodeType == MdlNode::MDLNODE_LIGHT_ID)
        return ResetLightLite((MdlNodeLight*)pNode, pModelData, pVertexData);
    if (pNode->m_nNodeType == MdlNode::MDLNODE_REFERENCE_ID)
        return ResetReferenceLite(pNode, pModelData, pVertexData);
    if (pNode->m_nNodeType == MdlNode::MDLNODE_SKIN_ID)
        return ResetTriMeshPartsLite((MdlNodeTriMesh*)pNode, pModelData, pVertexData);
    if (pNode->m_nNodeType == MdlNode::MDLNODE_TRIGGER_ID)
        return ResetTriggerLite(pNode, pModelData, pVertexData);
    if (pNode->m_nNodeType == MdlNode::MDLNODE_LIGHTSABER_ID)
        return ResetLightsaberLite((MdlNodeLightsaber*)pNode, pModelData, pVertexData);
    if (pNode->m_nNodeType == MdlNode::MDLNODE_TRIMESH_ID)
        return ResetTriMeshPartsLite((MdlNodeTriMesh*)pNode, pModelData, pVertexData);
}
#endif

void Input::InputBinary::ResetCameraLite(MdlNode* pNode,
                                    unsigned char* pModelData,
                                    unsigned char* pVertexData)
#ifdef _XBOX
{
    ResetMdlNodePartsLite(pNode, pModelData, pVertexData);
}
#else
{
    int nChildren = pNode->m_lChildren.m_nCount;
    if (nChildren != 0)
    {
        for (int i = 0; i < nChildren; ++i)
        {
            ResetMdlNodeLite(pNode->m_lChildren.m_pList[i], pModelData, pVertexData);
            nChildren = pNode->m_lChildren.m_nCount;
        }
    }
}
#endif

void Input::InputBinary::ResetEmitterLite(MdlNode* pNode,
                                    unsigned char* pModelData,
                                    unsigned char* pVertexData)
#ifdef _XBOX
{
    ResetMdlNodePartsLite(pNode, pModelData, pVertexData);
}
#else
{
    int nChildren = pNode->m_lChildren.m_nCount;
    if (nChildren != 0)
    {
        for (int i = 0; i < nChildren; ++i)
        {
            ResetMdlNodeLite(pNode->m_lChildren.m_pList[i], pModelData, pVertexData);
            nChildren = pNode->m_lChildren.m_nCount;
        }
    }
}
#endif

void Input::InputBinary::ResetReferenceLite(MdlNode* pNode,
                                    unsigned char* pModelData,
                                    unsigned char* pVertexData)
#ifdef _XBOX
{
    ResetMdlNodePartsLite(pNode, pModelData, pVertexData);
}
#else
{
    int nChildren = pNode->m_lChildren.m_nCount;
    if (nChildren != 0)
    {
        for (int i = 0; i < nChildren; ++i)
        {
            ResetMdlNodeLite(pNode->m_lChildren.m_pList[i], pModelData, pVertexData);
            nChildren = pNode->m_lChildren.m_nCount;
        }
    }
}
#endif

void Input::InputBinary::ResetTriggerLite(MdlNode* pNode,
                                    unsigned char* pModelData,
                                    unsigned char* pVertexData)
#ifdef _XBOX
{
    ResetMdlNodePartsLite(pNode, pModelData, pVertexData);
}
#else
{
    int nChildren = pNode->m_lChildren.m_nCount;
    if (nChildren != 0)
    {
        for (int i = 0; i < nChildren; ++i)
        {
            ResetMdlNodeLite(pNode->m_lChildren.m_pList[i], pModelData, pVertexData);
            nChildren = pNode->m_lChildren.m_nCount;
        }
    }
}
#endif

void Input::InputBinary::ResetMdlNodePartsLite(MdlNode* pNode,
                                    unsigned char* pModelData,
                                    unsigned char* pVertexData)
{
    int nChildren = pNode->m_lChildren.m_nCount;
    if (nChildren != 0)
    {
        for (int i = 0; i < nChildren; ++i)
        {
            ResetMdlNodeLite(pNode->m_lChildren.m_pList[i], pModelData, pVertexData);
            nChildren = pNode->m_lChildren.m_nCount;
        }
    }
}

void Input::InputBinary::ResetMdlNodeParts(MdlNode* pNode,
                                           unsigned char* pModelData,
                                           unsigned char* pVertexData)
{
    MdlNode* pParentNode = pNode->m_pParentNode;
    int i = 0;
    pNode->m_pModel = (MaxTree*)(pModelData + (unsigned int)pNode->m_pModel);
    if (pParentNode != 0)
    {
        pNode->m_pParentNode =
            (MdlNode*)((unsigned char*)pParentNode + (unsigned int)pModelData);
    }
    else
    {
        pNode->m_pParentNode = 0;
    }

    if (pNode->m_lChildren.m_nCount != 0)
    {
        pNode->m_lChildren.m_pList =
            (MdlNode**)(pModelData + (unsigned int)pNode->m_lChildren.m_pList);
        for (i = 0; i < pNode->m_lChildren.m_nCount; ++i)
        {
            pNode->m_lChildren.m_pList[i] =
                (MdlNode*)(pModelData + (unsigned int)pNode->m_lChildren.m_pList[i]);
            ResetMdlNode(pNode->m_lChildren.m_pList[i], pModelData, pVertexData);
        }
    }
    else
    {
        pNode->m_lChildren.m_pList = 0;
    }

    if (pNode->m_lstControllers.m_nCount != 0)
    {
        pNode->m_lstControllers.m_pList =
            (NewController*)(pModelData + (unsigned int)pNode->m_lstControllers.m_pList);
    }
    else
    {
        pNode->m_lstControllers.m_pList = 0;
    }

    if (pNode->m_lstControllerData.m_nCount != 0)
    {
        pNode->m_lstControllerData.m_pList =
            (float*)(pModelData + (unsigned int)pNode->m_lstControllerData.m_pList);
    }
    else
    {
        pNode->m_lstControllerData.m_pList = 0;
    }
}

MaxTree* Input::Read(char* sFileName)
{
    ++g_nModelsRead;
    void* pModelResource = AurResGet(sFileName, ".mdl", 0, true);
    void* pVertexResource = AurResGet(sFileName, ".mdx", 0, true);
    if (pModelResource == 0)
    {
        return 0;
    }
    unsigned char* pData = (unsigned char*)AurResGetDataBytes(4, pModelResource);
    if (pData == 0)
    {
        return 0;
    }
    if (*pData == 0)
    {
#ifdef _XBOX
        bool bLite = false;
        if (pData[3] & 1)
        {
            bLite = true;
        }
        pData[3] |= 1;
        AurResFreeDataBytes(pModelResource, pData);
        MaxTree* pTree = m_InputBinary.Read(pModelResource, pVertexResource, bLite);
#else
        AurResFreeDataBytes(pModelResource, pData);
        MaxTree* pTree = m_InputBinary.Read(pModelResource, pVertexResource, false);
#endif
        return pTree->AsModel();
    }
    if (pVertexResource != 0)
    {
        AurResFree(pVertexResource, 0);
    }
    AurResFreeDataBytes(pModelResource, pData);
    AurResFree(pModelResource, 0);
    char sPath[128];
    sprintf(sPath, "%s%s", sFileName, ".mdl");
    Model* pPreviousModel = currentmodel;
    dofile(sPath);
    Model* pModel = currentmodel;
    currentmodel = pPreviousModel;
    return pModel;
}

MaxTree* Input::Read(FILE* pModelFile, FILE* pVertexFile)
{
    void* pModelResource = AurResGet(pModelFile);
    if (pModelResource == 0)
    {
        return 0;
    }
    unsigned char* pData = (unsigned char*)AurResGetDataBytes(4, pModelResource);
    if (pData == 0)
    {
        return 0;
    }
    if (*pData == 0)
    {
        AurResFreeDataBytes(pModelResource, pData);
        void* pVertexResource = AurResGet(pVertexFile);
        MaxTree* pTree = m_InputBinary.Read(pModelResource, pVertexResource, false);
        return pTree->AsModel();
    }
    else
    {
        AurResFreeDataBytes(pModelResource, pData);
        Model* pPreviousModel = currentmodel;
        for (char* pLine = AurResGetNextLine(); pLine != 0; pLine = AurResGetNextLine())
        {
            while (*pLine == ' ' || *pLine == '\t')
            {
                ++pLine;
            }
            if (*pLine != 0 && *pLine != '#')
            {
                for (char* pEnd = pLine + strlen(pLine) - 1; pEnd >= pLine; *pEnd-- = 0)
                {
                    char ch = *pEnd;
                    if (*pEnd != '\n' && ch != '\r' && ch != '\t' && ch != ' ')
                    {
                        break;
                    }
                }
                FuncInterp(pLine);
            }
        }
        AurResFree(pModelResource, 0);
        Model* pModel = currentmodel;
        currentmodel = pPreviousModel;
        return pModel;
    }
}

void Input::InputBinary::ResetTriMeshParts(MdlNodeTriMesh* pNode,
                                          unsigned char* pModelData,
                                          unsigned char* pVertexData)
{
    ResetMdlNodeParts(pNode, pModelData, pVertexData);
#ifndef _XBOX
    if (pNode->m_nVertexCount > 0)
    {
        pNode->m_pVertexPositions =
            (Vector*)(pModelData + (unsigned int)pNode->m_pVertexPositions);
    }
    else
    {
        pNode->m_pVertexPositions = 0;
    }
#endif
    if (pNode->m_lFaces.m_nCount != 0)
    {
        pNode->m_lFaces.m_pList =
            (MaxFace*)(pModelData + (unsigned int)pNode->m_lFaces.m_pList);
    }
    else
    {
        pNode->m_lFaces.m_pList = 0;
    }
    if (pNode->m_lstIndexArrays.m_nCount != 0)
    {
        pNode->m_lstIndexArrays.m_pList =
            (unsigned int**)(pModelData + (unsigned int)pNode->m_lstIndexArrays.m_pList);
    }
    else
    {
        pNode->m_lstIndexArrays.m_pList = 0;
    }
    if (pNode->m_lstIndexCounts.m_nCount != 0)
    {
        pNode->m_lstIndexCounts.m_pList =
            (int*)(pModelData + (unsigned int)pNode->m_lstIndexCounts.m_pList);
    }
    else
    {
        pNode->m_lstIndexCounts.m_pList = 0;
    }
    for (int i = 0; i < pNode->m_lstIndexArrays.m_nCount; ++i)
    {
        pNode->m_lstIndexArrays.m_pList[i] =
            (unsigned int*)(pModelData + (unsigned int)pNode->m_lstIndexArrays.m_pList[i]);
    }
    if (pNode->m_lstTriangleIndices.m_nCount != 0)
    {
        pNode->m_lstTriangleIndices.m_pList =
            (unsigned int*)(pModelData + (unsigned int)pNode->m_lstTriangleIndices.m_pList);
    }
    else
    {
        pNode->m_lstTriangleIndices.m_pList = 0;
    }
    if (pNode->m_lstIndexPools.m_nCount != 0)
    {
        pNode->m_lstIndexPools.m_pList =
            (unsigned long*)(pModelData + (unsigned int)pNode->m_lstIndexPools.m_pList);
    }
    else
    {
        pNode->m_lstIndexPools.m_pList = 0;
    }
    if (pNode->m_lstIndexOffsets.m_nCount != 0)
    {
        pNode->m_lstIndexOffsets.m_pList =
            (unsigned long*)(pModelData + (unsigned int)pNode->m_lstIndexOffsets.m_pList);
    }
    else
    {
        pNode->m_lstIndexOffsets.m_pList = 0;
    }
    for (int j = 0; j < pNode->m_lstIndexPools.m_nCount; ++j)
    {
        int nOffset = pNode->m_lstIndexOffsets.m_pList[j];
#ifdef _XBOX
        if (pNode->m_lstIndexCounts.m_pList[j] != 0)
        {
            D3DIndexBuffer* pBuffer = new D3DIndexBuffer;
            memset(pBuffer, 0, sizeof(D3DIndexBuffer));
            XGSetIndexBufferHeader(pNode->m_lstIndexCounts.m_pList[j] * sizeof(unsigned short),
                                   D3DUSAGE_DYNAMIC, D3DFMT_INDEX16, D3DPOOL_SYSTEMMEM, pBuffer,
                                   (UINT)(pModelData + nOffset));
            unsigned long* pPool = &pNode->m_lstIndexPools.m_pList[j];
            *pPool = GLRender::AddIndexPool(
                pBuffer, pNode->m_lstIndexCounts.m_pList[j] * sizeof(unsigned short));
            pNode->AddToFreeTokenList(pNode->m_lstIndexPools.m_pList[j]);
        }
        else
        {
            pNode->m_lstIndexPools.m_pList[j] = RequestPoolManager::INVALID_POOL;
        }
#else
        GLRender::RequestPool(Aurora::POOL_INDEX,
                              pNode->m_lstIndexCounts.m_pList[j] * sizeof(unsigned short),
                              RequestPoolManager::POOL_STATIC, 0,
                              &pNode->m_lstIndexPools.m_pList[j]);
        int nPool = pNode->m_lstIndexPools.m_pList[j];
        void* pIndices = GLRender::LockPool(nPool, 0, 0, RequestPoolManager::POOL_LOCK_WRITE);
        memcpy(pIndices, pModelData + nOffset,
               pNode->m_lstIndexCounts.m_pList[j] * sizeof(unsigned short));
        GLRender::UnlockPool(pNode->m_lstIndexPools.m_pList[j]);
        pNode->AddToFreeTokenList(pNode->m_lstIndexPools.m_pList[j]);
#endif
    }
    if (pNode->m_nAuxIndexPool != RequestPoolManager::INVALID_POOL)
    {
        int nOffset = pNode->m_nAuxIndexOffset;
        GLRender::RequestPool(Aurora::POOL_INDEX,
                              pNode->m_nAuxIndexCount * sizeof(unsigned short),
                              RequestPoolManager::POOL_STATIC, 0,
                              &pNode->m_nAuxIndexPool);
        int nPool = pNode->m_nAuxIndexPool;
        void* pIndices = GLRender::LockPool(nPool, 0, 0, RequestPoolManager::POOL_LOCK_WRITE);
        memcpy(pIndices, pModelData + nOffset,
               pNode->m_nAuxIndexCount * sizeof(unsigned short));
        GLRender::UnlockPool(pNode->m_nAuxIndexPool);
        pNode->AddToFreeTokenList(pNode->m_nAuxIndexPool);
    }
    RyanTotalLevelVertices += pNode->m_nVertexCount;
    RyanTotalLevelVertSize += pNode->m_nVertexCount * pNode->m_nVertexStride;
}

void Input::InputBinary::ResetTriMeshPartsLite(MdlNodeTriMesh* pNode,
                                              unsigned char* pModelData,
                                              unsigned char* pVertexData)
{
    ResetMdlNodePartsLite(pNode, pModelData, pVertexData);
    for (int i = 0; i < pNode->m_lstIndexPools.m_nCount; ++i)
    {
        int nOffset = pNode->m_lstIndexOffsets.m_pList[i];
#ifdef _XBOX
        if (pNode->m_lstIndexCounts.m_pList[i] != 0)
        {
            D3DIndexBuffer* pBuffer = new D3DIndexBuffer;
            memset(pBuffer, 0, sizeof(D3DIndexBuffer));
            XGSetIndexBufferHeader(pNode->m_lstIndexCounts.m_pList[i] * sizeof(short),
                                   D3DUSAGE_DYNAMIC, D3DFMT_INDEX16, D3DPOOL_SYSTEMMEM, pBuffer,
                                   (UINT)(pModelData + nOffset));
            unsigned long* pPool = &pNode->m_lstIndexPools.m_pList[i];
            *pPool = GLRender::AddIndexPool(
                pBuffer, pNode->m_lstIndexCounts.m_pList[i] * sizeof(short));
            pNode->AddToFreeTokenList(pNode->m_lstIndexPools.m_pList[i]);
        }
        else
        {
            pNode->m_lstIndexPools.m_pList[i] = RequestPoolManager::INVALID_POOL;
        }
#else
        GLRender::RequestPool(Aurora::POOL_INDEX,
                              pNode->m_lstIndexCounts.m_pList[i] * sizeof(short),
                              RequestPoolManager::POOL_STATIC, 0,
                              &pNode->m_lstIndexPools.m_pList[i]);
        int nPool = pNode->m_lstIndexPools.m_pList[i];
        void* pIndices = GLRender::LockPool(nPool,
                                           0, 0, 1);
        memcpy(pIndices, pModelData + nOffset,
               pNode->m_lstIndexCounts.m_pList[i] * sizeof(short));
        GLRender::UnlockPool(pNode->m_lstIndexPools.m_pList[i]);
        pNode->AddToFreeTokenList(pNode->m_lstIndexPools.m_pList[i]);
#endif
    }
    if (pNode->m_nAuxIndexPool != RequestPoolManager::INVALID_POOL)
    {
        int nOffset = pNode->m_nAuxIndexOffset;
        GLRender::RequestPool(Aurora::POOL_INDEX,
                              pNode->m_nAuxIndexCount * sizeof(short),
                              RequestPoolManager::POOL_STATIC, 0,
                              &pNode->m_nAuxIndexPool);
        void* pIndices = GLRender::LockPool(pNode->m_nAuxIndexPool, 0, 0, RequestPoolManager::POOL_LOCK_WRITE);
        memcpy(pIndices, pModelData + nOffset,
               pNode->m_nAuxIndexCount * sizeof(short));
        GLRender::UnlockPool(pNode->m_nAuxIndexPool);
        pNode->AddToFreeTokenList(pNode->m_nAuxIndexPool);
    }
    RyanTotalLevelVertices += pNode->m_nVertexCount;
    RyanTotalLevelVertSize += pNode->m_nVertexCount * pNode->m_nVertexStride;
}

#ifdef _XBOX
void Input::InputBinary::ResetBase(MdlNode* pNode, unsigned char* pModelData, unsigned char* pVertexData)
{
    ResetMdlNodeParts(pNode, pModelData, pVertexData);
}

void Input::InputBinary::ResetCamera(MdlNode* pNode, unsigned char* pModelData, unsigned char* pVertexData)
{
    ResetMdlNodeParts(pNode, pModelData, pVertexData);
}

void Input::InputBinary::ResetEmitter(MdlNode* pNode, unsigned char* pModelData, unsigned char* pVertexData)
{
    ResetMdlNodeParts(pNode, pModelData, pVertexData);
}

void Input::InputBinary::ResetReference(MdlNode* pNode, unsigned char* pModelData, unsigned char* pVertexData)
{
    ResetMdlNodeParts(pNode, pModelData, pVertexData);
}

void Input::InputBinary::ResetTrigger(MdlNode* pNode, unsigned char* pModelData, unsigned char* pVertexData)
{
    ResetMdlNodeParts(pNode, pModelData, pVertexData);
}

void Input::InputBinary::ResetTriMesh(MdlNodeTriMesh* pNode, unsigned char* pModelData,
                                      unsigned char* pVertexData)
{
    pNode->m_pfnGenVertices = MdlNodeTriMeshGenVertices;
    pNode->m_pfnRemoveTemporaryArrays = MdlNodeTriMeshRemoveTemporaryArrays;
    ResetTriMeshParts(pNode, pModelData, pVertexData);
}

void Input::InputBinary::ResetAABBLite(MdlNodeAABB* pNode, unsigned char* pModelData,
                                       unsigned char* pVertexData)
{
    ResetTriMeshPartsLite(pNode, pModelData, pVertexData);
}

void Input::InputBinary::ResetAnimLite(MdlNodeAnimMesh* pNode, unsigned char* pModelData,
                                       unsigned char* pVertexData)
{
    ResetTriMeshPartsLite(pNode, pModelData, pVertexData);
}

void Input::InputBinary::ResetDanglyLite(MdlNodeDanglyMesh* pNode, unsigned char* pModelData,
                                         unsigned char* pVertexData)
{
    ResetTriMeshPartsLite(pNode, pModelData, pVertexData);
}

void Input::InputBinary::ResetSkinLite(MdlNodeSkin* pNode, unsigned char* pModelData,
                                       unsigned char* pVertexData)
{
    ResetTriMeshPartsLite(pNode, pModelData, pVertexData);
}

void Input::InputBinary::ResetTriMeshLite(MdlNodeTriMesh* pNode, unsigned char* pModelData,
                                          unsigned char* pVertexData)
{
    ResetTriMeshPartsLite(pNode, pModelData, pVertexData);
}
#endif

}

#ifdef _XBOX

namespace
{
class Output
{
public:
    Output() : m_lstAllocations(0)
    {
        m_nUnknown0c = 0;
        m_nUnknown10 = 0;
    }
    ~Output();
    bool Write(MaxTree* pTree, char* sFileName);

    List<char*> m_lstAllocations;
    int m_nUnknown0c;
    int m_nUnknown10;
};

char sOutputPath[WRITE_PATH_LENGTH];

bool Output::Write(MaxTree* pTree, char* sFileName)
{
    if (sFileName)
    {
        sprintf(sOutputPath, "d:\override\%s", sFileName);
    }
    else
    {
        sprintf(sOutputPath, "d:\override\%s.mdl", pTree->m_sName);
    }
    fclose(fopen(sOutputPath, "wb"));
    return false;
}

Output::~Output()
{
    for (int i = 0; i < m_lstAllocations.m_nCount; ++i)
    {
        delete m_lstAllocations[i];
    }
}
}

bool IODispatcher::WriteSync(MaxTree* pTree, char* sFileName)
{
    Output output;

    return output.Write(pTree, sFileName);
}
#endif
