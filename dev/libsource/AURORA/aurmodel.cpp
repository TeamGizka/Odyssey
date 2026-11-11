#include "aurora.h"
#include "aurtexman.h"
#include "aurmodel.h"

#include "aurscene.h"
#include "aurmodel.h"
#include "aurrendergl.h"
#include "aurprimitive.h"
#include "aurtrigger.h"
#include "aurparse.h"
#include "aurconsole.h"

#include <string.h>
#include <stdlib.h>
#include <math.h>

static inline void AurModelTrace4();
static inline void AurModelTrace3();
static inline void AurModelTrace2();
static inline void AurModelTrace1();
#define AURMODEL_TRACE() AurModelTrace1()

int PartTriMesh::m_nNextPartID = 1;

int PartProjection::m_nNextProjectionID = 1;
int countmaxfaces = 0;
bool PartTriMesh::m_bMaterialHasChanged = true;

int exportmembers = 1;
int removedelref = 0;
int countnode = 0;
int countmodelnodes = 0;
int countpart = 0;
int counttrimesh = 0;
int g_nCountOfGobs = 0;

int g_nSpawnedGobs = 0;

char* hitpartname = 0;

static MdlNodeTriMesh* aabb_mesh;
static CHitInfo* aabb_hitinfo;
static int aabb_direction_heuristic = 0;
static int aabb_test = 0;
int aabb_testopposite = 0;

int countboxes = 0;

int staticratio = 0;
int alwaysdominspheres = 0;
float gobspherescaling = 1.35f;
FILE* spewpltfp = 0;

int bsphack;
char lastspawnedobject[64];
extern Vector defaultdiffuse;
extern Vector defaultambient;

unsigned short PartTriMesh::m_VertexIndices[PartTriMesh::MAX_SHADOW_INDICES];

const char MaxTree::MAXTREE_BASE_ID = 1;
const char MaxTree::MAXTREE_MODEL_ID = 2;
const char MaxTree::MAXTREE_ANIMATION_ID = 5;
const char Model::SCALABLE_MODEL = 1;
const char Model::NON_SCALABLE_MODEL = 0;

const short MdlNode::MDLNODE_BASE_ID = 0x0001;
const short MdlNode::MDLNODE_LIGHT_ID = 0x0003;
const short MdlNode::MDLNODE_EMITTER_ID = 0x0005;
const short MdlNode::MDLNODE_CAMERA_ID = 0x0009;
const short MdlNode::MDLNODE_REFERENCE_ID = 0x0011;
const short MdlNode::MDLNODE_TRIMESH_ID = 0x0021;
const short MdlNode::MDLNODE_SKIN_ID = 0x0061;
const short MdlNode::MDLNODE_ANIM_ID = 0x00a1;
const short MdlNode::MDLNODE_DANGLY_ID = 0x0121;
const short MdlNode::MDLNODE_AABB_ID = 0x0221;
const short MdlNode::MDLNODE_TRIGGER_ID = 0x0401;
const short MdlNode::MDLNODE_LIGHTSABER_ID = 0x0821;

#pragma inline_recursion(on)
#pragma inline_depth(10)

int UpdateAnimFootprint(MdlNode* pNode, int nMultiplier);
Part* FindPart(Part* pPart, char* sName);
int PartHasMaterial(CAurPart* pPart);

static int FindGobIdx(char* sName);
Part* CreateInstanceTreeR(MdlNode* pNode, Gob* pGob);
extern List<Gob*> gobs;
Gob* FindGob(char* sName);
Scene* FindScene(char* sName);

const short MdlNodeLightsaber::NUM_FRONT_SEGMENTS = 20;
const short MdlNodeLightsaber::NUM_BACK_SEGMENTS = 20;
const short MdlNodeLightsaber::NUM_PIECE_VERTS = 8;
const short MdlNodeLightsaber::NUM_SEGMENT_VERTS = 4;

const int MdlNodeTriMesh::BASE_CAP = 2;
const int MdlNodeTriMesh::NEIGHBOR_CAP = 4;
const int MdlNodeTriMesh::FADEABLE = 1;
const int MdlNodeTriMesh::DEFAULT_PROPERTIES = 0;

const int TileData::FADE = 1;
const int TileData::DO_NOT_FADE = 2;
const int TileData::FADE_NOT_CHECKED = 4;
// PC 0x0043E0D0  Xbox 0x00294810
MaxFace_PostProcess::MaxFace_PostProcess()
{
    m_pFaces = 0;
    m_pFaceIndices = 0;
    m_pFaceRemap = 0;
    m_nFaces = 0;
    m_nFaceIndices = 0;
}
// PC 0x0043E0F0  Xbox 0x00294830
MaxFace_PostProcess::~MaxFace_PostProcess()
{
    if (m_pFaces)
    {
        delete[] m_pFaces;
    }

    if (m_pFaceIndices)
    {
        delete[] m_pFaceIndices;
    }

    if (m_pFaceRemap)
    {
        delete[] m_pFaceRemap;
    }

    m_nFaces = 0;
    m_nFaceIndices = 0;
}
// PC 0x0043E130  Xbox 0x00294870
int MaxTree::GetType()
{
    return m_nType & TYPE_MASK;
}
// PC 0x0043E140  Xbox 0x00294880
MaxFace::~MaxFace()
{
    --countmaxfaces;
}
// PC 0x0043E150  Xbox 0x00294890
void CAurObject::AnimateExternal(float fTimeStep)
{
    Gob* pGob = (Gob*)this;

    if (pGob->m_pScene)
    {
        pGob->m_pScene->m_nAnimationFrame++;
        pGob->Animate(fTimeStep);
        pGob->m_pScene->m_nAnimationFrame++;
    }
    else
    {
        pGob->Animate(fTimeStep);
    }
}
// PC 0x0044AE60  Xbox 0x0029EE80
MdlNode* MdlNodeAnimMesh::InternalConJoin()
{
    return new MdlNodeAnimMesh(TranslateNodeName());
}
// PC 0x0043E190  Xbox 0x002948D0
int MaxTreeParseField(void* pTree, char* szField)
{
    return ((MaxTree*)pTree)->InternalParseField(szField);
}
// PC 0x0044AEC0  Xbox 0x0029EF10
void MaxTree::operator delete(void* pMemory)
{
    MaxTree* pTree = (MaxTree*)pMemory;
    if (pTree == 0)
        return;

    if (pTree->m_nType & BINARY_TYPE_FLAG)
    {
        pTree->FreeTheFreeList();
        Model* pModel = (Model*)pTree;
        if (pModel->m_pParentModel)
        {
            pModel->m_pParentModel->DelRef(pModel);
            pModel->m_pParentModel = 0;
        }
        pTree->m_pfnDestructor(pTree);
        if (pTree->AsModel()->m_pVertexResource)
            AurResFree(pTree->AsModel()->m_pVertexResource, 0);
        delete[] (unsigned char*)pTree;
    }
    else if (pTree->m_nType)
    {
        pTree->m_pfnDestructor(pTree);
        if (pTree->m_pRootNode)
        {
            if (pTree->m_pRootNode->AsMdlNodeLight())
                delete pTree->m_pRootNode->AsMdlNodeLight();
            else if (pTree->m_pRootNode->AsMdlNodeCamera())
                delete pTree->m_pRootNode->AsMdlNodeCamera();
            else if (pTree->m_pRootNode->AsMdlNodeDanglyMesh())
                delete pTree->m_pRootNode->AsMdlNodeDanglyMesh();
            else if (pTree->m_pRootNode->AsMdlNodeAnimMesh())
                delete pTree->m_pRootNode->AsMdlNodeAnimMesh();
            else if (pTree->m_pRootNode->AsMdlNodeEmitter())
                delete pTree->m_pRootNode->AsMdlNodeEmitter();
            else if (pTree->m_pRootNode->AsMdlNodeReference())
                delete pTree->m_pRootNode->AsMdlNodeReference();
            else if (pTree->m_pRootNode->AsMdlNodeAABB())
                delete pTree->m_pRootNode->AsMdlNodeAABB();
            else if (pTree->m_pRootNode->AsMdlNodeSkin())
                delete pTree->m_pRootNode->AsMdlNodeSkin();
            else if (pTree->m_pRootNode->AsMdlNodeTrigger())
                delete pTree->m_pRootNode->AsMdlNodeTrigger();
            else if (pTree->m_pRootNode->AsMdlNodeLightsaber())
                delete pTree->m_pRootNode->AsMdlNodeLightsaber();
            else if (pTree->m_pRootNode->AsMdlNodeTriMesh())
                delete pTree->m_pRootNode->AsMdlNodeTriMesh();
            else
                delete pTree->m_pRootNode;
        }
        if (pTree->AsModel())
            ::delete pTree->AsModel();
        else if (pTree->AsAnimation())
            ::delete pTree->AsAnimation();
    }
}
// PC 0x0043E1B0  Xbox 0x002948F0
void* MaxTree::operator new(size_t nSize)
{
    return ::operator new(nSize);
}
// PC 0x0043E1C0  Xbox 0x00294900
Model* MaxTree::AsModel()
{
#ifdef _XBOX
    return (unsigned char)GetType() == MAXTREE_MODEL_ID ? (Model*)this : 0;
#else
    return (m_nType & TYPE_MASK) == MAXTREE_MODEL_ID ? (Model*)this : 0;
#endif
}

void InsertModel(Model* pModel);
// PC 0x0043E1D0  Xbox 0x00294910
Animation* MaxTree::AsAnimation()
{
#ifdef _XBOX
    return (unsigned char)GetType() == MAXTREE_ANIMATION_ID ? (Animation*)this : 0;
#else
    return (m_nType & TYPE_MASK) == MAXTREE_ANIMATION_ID ? (Animation*)this : 0;
#endif
}
// PC 0x0043E1E0  Xbox 0x00294920
int ModelParseField(void* pModel, char* szField)
{
    return ((Model*)pModel)->InternalParseField(szField);
}
// PC 0x0043E1F0
int operator!=(const Vector& vFirst, const Vector& vSecond)
{
    return !equal(vFirst, vSecond);
}
// PC 0x0043E210  Xbox 0x00294930
int Model::DelRef(Model* pModel)
{
    if (removedelref)
        return m_nReferenceCount;

    int nReferenceCount = --m_nReferenceCount;
    if (m_nReferenceCount == 0)
        operator delete(this);
    return nReferenceCount;
}
// PC 0x0043E240  Xbox 0x00294960
int Model::AddRef(Gob* pGob)
{
    m_nReferenceCount++;
    return m_nReferenceCount;
}
// PC 0x0043E240
int Model::AddRef(Model* pModel)
{
    m_nReferenceCount++;
    return m_nReferenceCount;
}

#ifdef _XBOX
static char sFuncPrefix[64];
#endif
// PC 0x00449F60  Xbox 0x0029DA20
void Model::InternalDestructor()
{
    if (Models.Contains(this))
        Models.Remove(this);

    if (m_pParentModel)
    {
        m_pParentModel->DelRef(this);
        m_pParentModel = 0;
    }

    int i = m_lAnimations.m_nCount;
    animationfootprint -= m_lAnimations.m_nCount * sizeof(Animation*);
    animationfootprint -= m_lAnimations.m_nCount * sizeof(Animation);
    while (i--)
    {
        animationfootprint -= m_lAnimations[i]->m_lEvents.m_nCount * sizeof(AurKey<char[32]>);
        UpdateAnimFootprint(m_lAnimations[i]->m_pRootNode, -1);
        m_lAnimations[i]->m_pfnDestructor(m_lAnimations[i]);
    }

#ifdef _XBOX

    strcpy(sFuncPrefix, m_sName);
    strcat(sFuncPrefix, ".");
    DeleteFuncWithPrefix(sFuncPrefix);
    MaxTree::InternalDestructor();
#else
    if (m_pRootNode)
        m_pRootNode->DestructorDispatch();
#endif
}
// PC 0x00438A00  Xbox 0x0028FB30
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
// PC 0x0043F710
MaxFace::MaxFace()
{
    AURMODEL_TRACE();
    ++countmaxfaces;
    m_nSurfaceType = 0;
}
// PC 0x0043F730  Xbox 0x00295B30
MaxTree::~MaxTree()
{
}
// PC 0x0043F790  Xbox 0x00295B80
Model::~Model()
{
    if (m_nType)
    {
        m_nType = 0;
        int i = m_lAnimations.m_nCount;
        while (i)
        {
            operator delete(m_lAnimations[--i]);
            m_lAnimations.m_nCount = i;
        }
        int nPool = m_nVertexPool;
        GLRender::DeletePool(nPool);
        m_nVertexPool = RequestPoolManager::INVALID_POOL;
        for (int j = 0; j < m_lNames.m_nCount; ++j)
            delete[] m_lNames[j];
    }
}
// PC 0x0043F890  Xbox 0x00295C50
void MdlNode::ControlDispatch(Part* pPart, float fTime,
                              float fPreviousTime, float fBlend)
{
    switch (m_nNodeType)
    {
    case MDL_NODE_HEADER | MDL_NODE_LIGHT:
        AsMdlNodeLight()->InternalControl(pPart, fTime, fPreviousTime, fBlend);
        break;
    case MDL_NODE_HEADER | MDL_NODE_EMITTER:
        AsMdlNodeEmitter()->InternalControl(pPart, fTime, fPreviousTime, fBlend);
        break;
    case MDL_NODE_HEADER | MDL_NODE_CAMERA:
        AsMdlNodeCamera()->InternalControl(pPart, fTime, fPreviousTime, fBlend);
        break;
    case MDL_NODE_HEADER | MDL_NODE_REFERENCE:
        AsMdlNodeReference()->InternalControl(pPart, fTime, fPreviousTime, fBlend);
        break;
    case MDL_NODE_HEADER | MDL_NODE_TRIMESH:
        AsMdlNodeTriMesh()->InternalControl(pPart, fTime, fPreviousTime, fBlend);
        break;
    case MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_SKIN:
        AsMdlNodeSkin()->InternalControl(pPart, fTime, fPreviousTime, fBlend);
        break;
    case MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_ANIMMESH:
        AsMdlNodeAnimMesh()->InternalControl(pPart, fTime, fPreviousTime, fBlend);
        break;
    case MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_DANGLYMESH:
        AsMdlNodeDanglyMesh()->InternalControl(pPart, fTime, fPreviousTime, fBlend);
        break;
    case MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_AABB:
        AsMdlNodeAABB()->InternalControl(pPart, fTime, fPreviousTime, fBlend);
        break;
    case MDL_NODE_HEADER | MDL_NODE_TRIGGER:
        AsMdlNodeTrigger()->InternalControl(pPart, fTime, fPreviousTime, fBlend);
        break;
    case MDL_NODE_HEADER:
        InternalControl(pPart, fTime, fPreviousTime, fBlend);
        break;
    case MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_LIGHTSABER:
        AsMdlNodeLightsaber()->InternalControl(pPart, fTime, fPreviousTime, fBlend);
        break;
    }
}

int MdlNodeAffectedByWind(MdlNode* pNode)
{
    MdlNodeEmitter* pEmitter = pNode->AsMdlNodeEmitter();
    return pEmitter != 0 && pEmitter->IsAffectedByWind();
}
// PC 0x0043FA10  Xbox 0x00295E70
void MdlNode::ParseFieldDispatch(char* szField)
{
    switch (m_nNodeType)
    {
    case MDL_NODE_HEADER:
        InternalParseField(szField);
        break;
    case MDL_NODE_HEADER | MDL_NODE_LIGHT:
        AsMdlNodeLight()->InternalParseField(szField);
        break;
    case MDL_NODE_HEADER | MDL_NODE_EMITTER:
        AsMdlNodeEmitter()->InternalParseField(szField);
        break;
    case MDL_NODE_HEADER | MDL_NODE_CAMERA:
        AsMdlNodeCamera()->InternalParseField(szField);
        break;
    case MDL_NODE_HEADER | MDL_NODE_REFERENCE:
        AsMdlNodeReference()->InternalParseField(szField);
        break;
    case MDL_NODE_HEADER | MDL_NODE_TRIMESH:
        AsMdlNodeTriMesh()->InternalParseField(szField);
        break;
    case MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_SKIN:
        AsMdlNodeSkin()->InternalParseField(szField);
        break;
    case MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_ANIMMESH:
        AsMdlNodeAnimMesh()->InternalParseField(szField);
        break;
    case MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_DANGLYMESH:
        AsMdlNodeDanglyMesh()->InternalParseField(szField);
        break;
    case MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_AABB:
        AsMdlNodeAABB()->InternalParseField(szField);
        break;
    case MDL_NODE_HEADER | MDL_NODE_TRIGGER:
        AsMdlNodeTrigger()->InternalParseField(szField);
        break;
    case MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_LIGHTSABER:
        AsMdlNodeLightsaber()->InternalParseField(szField);
        break;
    }
}
// PC 0x0043FB90  Xbox 0x00295FE0
void MdlNode::PostProcessDispatch()
{
    switch (m_nNodeType)
    {
    case MDL_NODE_HEADER | MDL_NODE_LIGHT:
        AsMdlNodeLight()->InternalPostProcess();
        break;
    case MDL_NODE_HEADER | MDL_NODE_EMITTER:
        AsMdlNodeEmitter()->InternalPostProcess();
        break;
    case MDL_NODE_HEADER | MDL_NODE_CAMERA:
        AsMdlNodeCamera()->InternalPostProcess();
        break;
    case MDL_NODE_HEADER | MDL_NODE_REFERENCE:
        AsMdlNodeReference()->InternalPostProcess();
        break;
    case MDL_NODE_HEADER | MDL_NODE_TRIMESH:
        AsMdlNodeTriMesh()->InternalPostProcess();
        break;
    case MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_SKIN:
        AsMdlNodeSkin()->InternalPostProcess();
        break;
    case MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_ANIMMESH:
        AsMdlNodeAnimMesh()->InternalPostProcess();
        break;
    case MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_DANGLYMESH:
        AsMdlNodeDanglyMesh()->InternalPostProcess();
        break;
    case MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_AABB:
        AsMdlNodeAABB()->InternalPostProcess();
        break;
    case MDL_NODE_HEADER | MDL_NODE_TRIGGER:
        AsMdlNodeTrigger()->InternalPostProcess();
        break;
    case MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_LIGHTSABER:
        AsMdlNodeLightsaber()->InternalPostProcess();
        break;
    case MDL_NODE_HEADER:
        InternalPostProcess();
        break;
    }
}
// PC 0x0043FD10
void MdlNodeLightsaber::InternalDestructor()
{
    GLRender::DeletePool(m_nBladePool);
    m_nBladePool = RequestPoolManager::INVALID_POOL;
    GLRender::DeletePool(m_nGlowPool);
    m_nGlowPool = RequestPoolManager::INVALID_POOL;
    int i = m_lChildren.m_nCount;
    while (i != 0)
        m_lChildren[--i]->DestructorDispatch();
    --countnode;
}
// PC 0x0043FD70
inline MdlNode* MdlNodeTrigger::InternalConJoin()
{
    return new MdlNodeTrigger(TranslateNodeName());
}
// PC 0x0043FDD0  Xbox 0x002960F0
void MdlNode::InternalDestructor()
{
    int i = m_lChildren.m_nCount;
    while (i != 0)
        m_lChildren[--i]->DestructorDispatch();
    --countnode;
}
// PC 0x00445190  Xbox 0x0029A110
void MaxTree::AddToFreeTokenList(unsigned long nPool)
{
    m_lFreeTokens.Add(nPool);
}
// PC 0x004451D0  Xbox 0x0029A120
void MaxTree::ResetFreeLists()
{
    m_lFreeNodes.SetSize(0);
    m_lFreeTokens.SetSize(0);
}
// PC 0x00445210  Xbox 0x0029A1A0
MdlNode::MdlNode(char* sName)
    : m_lChildren(0), m_lstControllers(0), m_lstControllerData(0)
{
#ifdef _XBOX
    m_nNodeType = MDL_NODE_HEADER;
    m_pParentNode = 0;
    m_nNodeID = 0;
#else
    m_pParentNode = 0;
    m_nNodeID = 0;
    m_nNodeType = MDL_NODE_HEADER;
#endif
    ++countnode;
    m_vPosition = Vector(0.0f, 0.0f, 0.0f);
    m_qOrientation = Quaternion();
    ++countmodelnodes;
}
// PC 0x004452D0
void MdlNode::AddToFreeList(MdlNode* pNode)
{
    m_pModel->m_lFreeNodes.Add(pNode);
}
// PC 0x00445310  Xbox 0x0029A2C0
void MdlNode::AddToFreeTokenList(unsigned long nPool)
{
#ifdef _XBOX
    m_pModel->AddToFreeTokenList(nPool);
#else
    m_pModel->m_lFreeTokens.Add(nPool);
#endif
}
// PC 0x00445350  Xbox 0x0029A2D0
MdlNode* MdlNode::InternalConJoin()
{
    return new MdlNode(TranslateNodeName());
}
// PC 0x004453B0  Xbox 0x0029A360
MdlNodeTriMesh_PostProcess::~MdlNodeTriMesh_PostProcess()
{
    m_lPositions.SetSize(0);
    m_lNormals.SetSize(0);
    m_lTextureCoordinates0.SetSize(0);
    m_lTextureCoordinates1.SetSize(0);
    m_lTextureCoordinates2.SetSize(0);
    m_lTextureCoordinates3.SetSize(0);
    m_lColors.SetSize(0);
    m_lFaces.SetSize(0);

    for (int i = 0; i < m_lstVertices.m_nCount; i++)
    {
        delete m_lstVertices[i];
    }
    m_lstVertices.SetSize(0);
}
// PC 0x004488C0  Xbox 0x0029C550
inline CAurObject::~CAurObject()
{
}
// PC 0x004488F0  Xbox 0x0029C580
void MdlNode::DestructorDispatch()
{
    switch (m_nNodeType)
    {
    case MDL_NODE_HEADER:
        InternalDestructor();
        break;
    case MDL_NODE_HEADER | MDL_NODE_LIGHT:
        AsMdlNodeLight()->InternalDestructor();
        break;
    case MDL_NODE_HEADER | MDL_NODE_EMITTER:
        AsMdlNodeEmitter()->InternalDestructor();
        break;
    case MDL_NODE_HEADER | MDL_NODE_CAMERA:
        AsMdlNodeCamera()->InternalDestructor();
        break;
    case MDL_NODE_HEADER | MDL_NODE_REFERENCE:
        AsMdlNodeReference()->InternalDestructor();
        break;
    case MDL_NODE_HEADER | MDL_NODE_TRIMESH:
        AsMdlNodeTriMesh()->InternalDestructor();
        break;
    case MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_SKIN:
        AsMdlNodeSkin()->InternalDestructor();
        break;
    case MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_ANIMMESH:
        AsMdlNodeAnimMesh()->InternalDestructor();
        break;
    case MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_DANGLYMESH:
        AsMdlNodeDanglyMesh()->InternalDestructor();
        break;
    case MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_AABB:
        AsMdlNodeAABB()->InternalDestructor();
        break;
    case MDL_NODE_HEADER | MDL_NODE_TRIGGER:
        AsMdlNodeTrigger()->InternalDestructor();
        break;
    case MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_LIGHTSABER:
        AsMdlNodeLightsaber()->InternalDestructor();
        break;
    }
}
// PC 0x00448000
void MdlNodeLight::InternalDestructor()
{
    for (int i = m_lstTextures.m_nCount; i != 0; --m_lstTextures.m_nCount)
    {
        CAurTexture* pTexture = m_lstTextures[--i];
        m_lstTextures[i] = 0;
        AurTextureRelease(pTexture);
    }
    MdlNode::InternalDestructor();
}
// PC 0x00448A70  Xbox 0x0029C690
Part* MdlNode::CreateInstanceDispatch(Gob* pGob)
{
    switch (m_nNodeType)
    {
    case MDL_NODE_HEADER:
        return InternalCreateInstance(pGob);
    case MDL_NODE_HEADER | MDL_NODE_LIGHT:
        return AsMdlNodeLight()->InternalCreateInstance(pGob);
    case MDL_NODE_HEADER | MDL_NODE_EMITTER:
        return AsMdlNodeEmitter()->InternalCreateInstance(pGob);
    case MDL_NODE_HEADER | MDL_NODE_CAMERA:
        return AsMdlNodeCamera()->InternalCreateInstance(pGob);
    case MDL_NODE_HEADER | MDL_NODE_REFERENCE:
        return AsMdlNodeReference()->InternalCreateInstance(pGob);
    case MDL_NODE_HEADER | MDL_NODE_TRIMESH:
        return AsMdlNodeTriMesh()->InternalCreateInstance(pGob);
    case MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_SKIN:
        return AsMdlNodeSkin()->InternalCreateInstance(pGob);
    case MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_ANIMMESH:
        return AsMdlNodeAnimMesh()->InternalCreateInstance(pGob);
    case MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_DANGLYMESH:
        return AsMdlNodeDanglyMesh()->InternalCreateInstance(pGob);
    case MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_AABB:
        return AsMdlNodeAABB()->InternalCreateInstance(pGob);
    case MDL_NODE_HEADER | MDL_NODE_TRIGGER:
        return AsMdlNodeTrigger()->InternalCreateInstance(pGob);
    case MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_LIGHTSABER:
        return AsMdlNodeLightsaber()->InternalCreateInstance(pGob);
    default:
        return 0;
    }
}
static void findallmaterials(Part* pPart, List<Material*>& lMaterials);
// PC 0x004480C0  Xbox 0x0029BEC0
Part* MdlNodeLightsaber::InternalCreateInstance(Gob*)
{
    AurModelTrace4();
    return new PartLightsaber(this);
}
// PC 0x00448130  Xbox 0x0029BF30
void Gob::EnableRenderBumpedOut(char* sTexture, float fHeight)
{
    m_bRenderBumpedOut = 1;
    List<CAurPart*> lstParts;
    EnumParts(PartHasMaterial, &lstParts);
    int nSize = 0;
    char sBlending[32];
    sBlending[0] = 0;
    CAurTexture* pTexture = AurTextureGetReference(sTexture, 0);
    Aurora::AuroraBlendModes nSource = BLEND_SRC_ALPHA;
    Aurora::AuroraBlendModes nDestination = BLEND_ONE_MINUS_SRC_ALPHA;
    char sLine[8192];
    if (!pTexture->IsTxiLoaded())
    {
        void* pResource = AurResGet(sTexture, ".txi", &nSize, true);
        if (pResource == 0)
        {
            pTexture->SetTxiLoaded(true);
            return;
        }
        int nBytes = nSize < 0 ? -1 - nSize : nSize;
        if (nBytes > 0)
        {
            pTexture->SetTxiData(AurResGetDataBytes(0, pResource), nBytes);
            for (char* pLine = AurResGetNextLine(); pLine != 0;
                 pLine = AurResGetNextLine())
            {
                while (*pLine == ' ' || *pLine == '\t')
                    ++pLine;
                if (stricmp("blending", firstword(pLine)) == 0)
                    Parse(pLine + 8, sBlending);
                if (sBlending[0] != 0)
                {
                    if (strcmp(sBlending, "additive") == 0)
                    {
                        nSource = BLEND_SRC_ALPHA;
                        nDestination = BLEND_ONE;
                    }
                    else if (strcmp(sBlending, "punchthrough") == 0)
                    {
                        nSource = BLEND_ONE;
                        nDestination = BLEND_ZERO;
                    }
                }
            }
        }
        AurResFree(pResource, nSize);
        pTexture->SetTxiLoaded(true);
    }
    else
    {
        char* pData = pTexture->GetTxiData();
        int nBytes = pTexture->GetTxiSize();
        if (pData != 0 && nBytes > 0)
        {
            do
            {
                int nLength = 0;
                do
                    sLine[nLength++] = *pData++;
                while (nLength < nBytes && sLine[nLength - 1] != '\n');
                nBytes -= nLength;
                if (nBytes < 0)
                    break;
                sLine[nLength] = 0;
                char* pLine = sLine;
                while (*pLine == ' ' || *pLine == '\t')
                    ++pLine;
                if (stricmp("blending", firstword(pLine)) == 0)
                    Parse(pLine + 8, sBlending);
                if (sBlending[0] != 0)
                {
                    if (strcmp(sBlending, "additive") == 0)
                    {
                        nSource = BLEND_SRC_ALPHA;
                        nDestination = BLEND_ONE;
                    }
                    else if (strcmp(sBlending, "punchthrough") == 0)
                    {
                        nSource = BLEND_ONE;
                        nDestination = BLEND_ZERO;
                    }
                }
            } while (nBytes > 0);
        }
    }
    if (pTexture)
    {
        for (int i = 0; i < lstParts.m_nCount; i++)
        {
            if (((Part*)lstParts[i])->m_pMaterial->m_pTexture3)
            {
                CAurTexture* pOldTexture = ((Part*)lstParts[i])->m_pMaterial->m_pTexture3;
                ((Part*)lstParts[i])->m_pMaterial->m_pTexture3 = 0;
                AurTextureRelease(pOldTexture);
            }
            ((Part*)lstParts[i])->m_pMaterial->m_pTexture3 = pTexture;
            nSize = 0;
            ((Part*)lstParts[i])->m_pMaterial->m_lstBlendSrc.SetSize(2);
            ((Part*)lstParts[i])->m_pMaterial->m_lstBlendDst.SetSize(2);
            ((Part*)lstParts[i])->m_pMaterial->m_lstBlendSrc[1] = nSource;
            ((Part*)lstParts[i])->m_pMaterial->m_lstBlendDst[1] = nDestination;
        }
    }
    m_fBumpedOutShellHeight = fHeight;
}
// PC 0x004485C0  Xbox 0x0029C2C0
void Gob::DisableRenderBumpedOut()
{
    if (m_bRenderBumpedOut)
    {
        m_bRenderBumpedOut = 0;
        List<CAurPart*> parts;
        EnumParts(PartHasMaterial, &parts);
        for (int i = 0; i < parts.m_nCount; i++)
        {
            Material* pMaterial = ((Part*)parts[i])->m_pMaterial;
            CAurTexture* pTexture = (CAurTexture*)pMaterial->m_pTexture3.m_pObject;
            pMaterial->m_pTexture3 = 0;
            AurTextureRelease(pTexture);
        }
        m_fBumpedOutShellHeight = 0;
    }
}
// PC 0x00448690  Xbox 0x0029C380
int Gob::SetEnvironmentMap(char* sName)
{
    char sTexture[64];
    if (sName && sName[0])
        strcpy(sTexture, sName);
    else
        strcpy(sTexture, "NULL");

    CAurTexture* pTexture = AurTextureGetReference(sTexture, 0);
    if (pTexture->IsInvalid())
    {
        AurTextureRelease(pTexture);
        return 0;
    }

    CAurTexture* pOldTexture = m_pEnvironmentMap;
    m_pEnvironmentMap = pTexture;
    AurTextureRelease(pOldTexture);
    if (m_pEnvironmentMap)
    {
        List<Material*> lMaterials;
        findallmaterials(m_pRootPart, lMaterials);
        for (int i = 0; i < lMaterials.m_nCount; i++)
            ((CAurTexture*)lMaterials[i]->m_pTexture0.m_pObject)->SetIsEnvironmentMapped(true);
    }
    return 1;
}
// PC 0x00448BF0  Xbox 0x0029C810
MdlNodeTriMesh::~MdlNodeTriMesh()
{
    if (m_nNodeType != 0)
        DestructorDispatch();
    m_nNodeType = 0;
    delete m_pPostProcess;
    if (m_nAuxIndexPool != RequestPoolManager::INVALID_POOL)
    {
        GLRender::DeletePool(m_nAuxIndexPool);
        m_nAuxIndexPool = RequestPoolManager::INVALID_POOL;
    }
    for (int i = 0; i < m_lstIndexPools.m_nCount; i++)
    {
        unsigned int nPool = m_lstIndexPools[i];
        GLRender::DeletePool(nPool);
        m_lstIndexPools[i] = RequestPoolManager::INVALID_POOL;
    }
    for (int j = 0; j < m_lstIndexArrays.m_nCount; j++)
        delete[] m_lstIndexArrays[j];
#ifdef _XBOX
    m_lstIndexArrays.m_nCount = 0;
#else
    m_lstIndexArrays.m_nCount = 0;
    delete[] m_pVertexPositions;
#endif
}
// PC 0x0043FE00  Xbox 0x00296180
Part* FindPart(Part* pPart, char* sName)
{
    if (pPart == 0)
        return 0;
    if (sName == 0)
        return 0;

    if (stricmp(pPart->m_pNode->TranslateNodeName(), sName) == 0)
        return pPart;

    for (int i = 0; i < pPart->m_lstChildren.m_nCount; i++)
    {
#ifdef _XBOX
        Part* pFound = FindPart(pPart->m_lstChildren[i], sName);
#else
        Part* pChild = pPart->m_lstChildren[i];
        Part* pFound = FindPart(pChild, sName);
#endif
        if (pFound)
            return pFound;
    }
    return 0;
}
// PC 0x00440110  Xbox 0x002961F0
void Part::PostProcess(Part* pPart)
{
    for (int i = 0; i < m_lstChildren.m_nCount; i++)
    {
        m_lstChildren[i]->PostProcess(pPart);
    }
}
// PC 0x00448DA0  Xbox 0x0029C970
MdlNodeAnimMesh::~MdlNodeAnimMesh()
{
    if (m_nNodeType != 0)
        DestructorDispatch();
    m_nNodeType = 0;
    DestructorDispatch();
    delete[] m_pAnimationData1;
    delete[] m_pAnimationData2;
}
// PC 0x00448E60  Xbox 0x0029CA20
MdlNodeSkin::~MdlNodeSkin()
{
    if (m_nNodeType != 0)
        DestructorDispatch();
    m_nNodeType = 0;
    if (m_pfBoneMap != 0)
    {
        delete[] m_pfBoneMap;
        m_pfBoneMap = 0;
    }
    m_lstTBoneRefInv.SetSize(0);
    m_lstQBoneRefInv.SetSize(0);
}
// PC 0x00448F80  Xbox 0x0029CAF0
MdlNodeLight::~MdlNodeLight()
{
    if (m_nNodeType != 0)
        DestructorDispatch();
    m_nNodeType = 0;
    for (int i = m_lstTextureNames.m_nCount; i != 0; )
    {
        delete[] m_lstTextureNames[--i];
        m_lstTextureNames.m_nCount--;
    }
}
// PC 0x00449090  Xbox 0x0029CBC0
MdlNodeLightsaber::~MdlNodeLightsaber()
{
    if (m_nNodeType != 0)
        DestructorDispatch();
    m_nNodeType = 0;
}
// PC 0x004490F0  Xbox 0x0029CC20
MdlNodeDanglyMesh::~MdlNodeDanglyMesh()
{
    if (m_nNodeType != 0)
        DestructorDispatch();
    m_nNodeType = 0;
    delete[] m_pVertexPositions;
}
// PC 0x00449170  Xbox 0x0029CCA0
MdlNodeAABB::~MdlNodeAABB()
{
    if (m_nNodeType != 0)
        DestructorDispatch();
    m_nNodeType = 0;
    delete m_pAABBData;
}
// PC 0x00449200  Xbox 0x0029CD30
Part* CreateInstanceTreeR(MdlNode* pNode, Gob* pGob)
{
    if (!pNode)
        return 0;

    Part* pPart = pNode->CreateInstanceDispatch(pGob);
    pPart->m_pAurObject = pGob;
    position_scale_factor = 1.0f;
    pNode->ControlDispatch(pPart, -1.0f, 0.0f, 1.0f);
    pPart->m_lstChildren.SetSize(pNode->m_lChildren.m_nCount);
    for (int i = 0; i < pNode->m_lChildren.m_nCount; i++)
    {
        pPart->m_lstChildren[i] = CreateInstanceTreeR(pNode->m_lChildren[i], pGob);
        pPart->m_lstChildren[i]->m_pParent = pPart;
    }
    pPart->m_nNodeID = pNode->m_nNodeID;
    return pPart;
}
// PC 0x00449410  Xbox 0x0029CDF0
Part* CreateInstanceTree(MdlNode* pNode, Gob* pGob)
{
    Part* pPart = CreateInstanceTreeR(pNode, pGob);
    pPart->PostProcess(pPart);
    return pPart;
}

inline CAurObjectLightContainer::CAurObjectLightContainer()
    : m_lstLights(0), m_lstFadeIntensities(0)
{
    m_pShadowLight = 0;
    m_fShadowFade = 0.0f;
}
// PC 0x00445600
inline CAurObjectLightContainer::~CAurObjectLightContainer()
{
}
// PC 0x004478D0  Xbox 0x0029B860
PartLightsaber::PartLightsaber(MdlNode* pNode)
    : PartTriMesh(pNode)
{
    m_nSegments = 0;
    m_nFirstSegment = 0;
    m_nNextSegment = 0;
}
// PC 0x00445670  Xbox 0x0029A490
PartLightsaber::~PartLightsaber()
{
    m_nSegments = 0;
    m_nFirstSegment = 0;
    m_nNextSegment = 0;
}
// PC 0x00447980  Xbox 0x0029B920
PartDanglyMesh::PartDanglyMesh(MdlNode* pNode)
    : PartTriMesh(pNode),
      m_pVertexPositions(0)
{
    m_pVertexPositions = (Vector*)new char[pNode->AsMdlNodeTriMesh()->m_nVertexCount * sizeof(Vector)];
    memcpy(m_pVertexPositions, pNode->AsMdlNodeDanglyMesh()->m_pVertexPositions,
           pNode->AsMdlNodeTriMesh()->m_nVertexCount * sizeof(Vector));
#ifndef _XBOX
    GLRender::RequestPool(Aurora::POOL_VERTEX,
                          pNode->AsMdlNodeTriMesh()->m_nVertexCount * sizeof(Vector),
                          RequestPoolManager::POOL_STATIC, 0, &m_nVertexPool);
    int nVertexPool = m_nVertexPool;
    void* pVertices = GLRender::LockPool(nVertexPool, 0, 0,
                                         RequestPoolManager::POOL_LOCK_WRITE);
    memcpy(pVertices, m_pVertexPositions,
           pNode->AsMdlNodeTriMesh()->m_nVertexCount * sizeof(Vector));
    GLRender::UnlockPool(m_nVertexPool);
#endif
}
// PC 0x004456A0  Xbox 0x0029A4C0
PartDanglyMesh::~PartDanglyMesh()
{
    delete m_pVertexPositions;
#ifndef _XBOX
    GLRender::DeletePool(m_nVertexPool);
    m_nVertexPool = RequestPoolManager::INVALID_POOL;
#endif
}
// PC 0x00445710  Xbox 0x0029A520
PartSkin::~PartSkin()
{
}
// PC 0x00447B20  Xbox 0x0029BA30
Part* MdlNode::InternalCreateInstance(Gob*)
{
    return new Part(this);
}
// PC 0x00447B90  Xbox 0x0029BAA0
Part* MdlNodeLight::InternalCreateInstance(Gob* pGob)
{
    PartLight* pLight = new PartLight(this);
    pLight->m_bShadow = m_bShadow;
    pLight->m_bFlare = m_bAffectDynamic;
    pLight->m_nLightType = m_nDynamicType;
    pLight->m_bAmbientOnly = m_bAmbientOnly;
    pLight->m_nInstanceState = 0;
    if (pGob)
    {
        if (m_nDynamicType || m_bAffectDynamic)
            pGob->m_bHasDynamicLights = 1;
        if (!m_nDynamicType)
            pGob->m_lstLights.Add(pLight);
    }
    return pLight;
}
// PC 0x00447C80  Xbox 0x0029BB70
Part* MdlNodeCamera::InternalCreateInstance(Gob*)
{
    return new PartCamera(this);
}
// PC 0x00447CE0  Xbox 0x0029BBD0
Part* MdlNodeReference::InternalCreateInstance(Gob*)
{
    return new PartReference(this);
}
// PC 0x00447D60  Xbox 0x0029BC50
PartTriMesh* MdlNodeTriMesh::InternalCreateInstance(Gob*)
{
    return new PartTriMesh(this);
}
// PC 0x00447DD0  Xbox 0x0029BCC0
Part* MdlNodeAnimMesh::InternalCreateInstance(Gob*)
{
    return new PartAnimMesh(this);
}
// PC 0x00447E30  Xbox 0x0029BD20
Part* MdlNodeAABB::InternalCreateInstance(Gob*)
{
    return new PartAABB(this);
}
// PC 0x00447E90  Xbox 0x0029BD80
Part* MdlNodeDanglyMesh::InternalCreateInstance(Gob*)
{
    return new PartDanglyMesh(this);
}
// PC 0x00445720  Xbox 0x0029A530
MdlNodeTriMesh_PostProcess::MdlNodeTriMesh_PostProcess()
    : m_lPositions(0),
      m_lTextureCoordinates0(0),
      m_lTextureCoordinates1(0),
      m_lTextureCoordinates2(0),
      m_lTextureCoordinates3(0),
      m_lColors(0),
      m_lFaces(0),
      m_lNormals(0),
      m_lstVertices(0)
{
}
// PC 0x0043E2E0  Xbox 0x002949B0
inline CAurPart::~CAurPart()
{
}
// PC 0x0043E310  Xbox 0x002949E0
inline CAurPathInfo::~CAurPathInfo()
{
}
// PC 0x004457A0  Xbox 0x0029A5F0
Part::Part(MdlNode* pNode)
    : m_lstChildren(0)
{
    m_fScale = 1.0f;
    m_pNode = pNode;
    m_pParent = 0;
    m_pAurObject = 0;
    m_nInstanceState = 0;
    m_pMaterial = 0;
    countpart++;
    m_nNodeID = 0;
    m_bIncludeChildBounds = 0;
}
// PC 0x00440140  Xbox 0x00296220
Part::~Part()
{
    for (int i = m_lstChildren.m_nCount; i != 0; m_lstChildren.m_nCount = i)
    {
        delete m_lstChildren[--i];
    }
    delete m_pMaterial;
    --countpart;
}
// PC 0x00445840  Xbox 0x0029A6C0
PartTriMesh::PartTriMesh(MdlNode* pNode)
    : Part(pNode)
{
    m_pTileData = 0;
    m_fAlpha = 1.0f;
    m_fTextureAngle = 0.0f;
    m_pVertexPrimitive = 0;
    m_nColorPool = -1;
    m_nTexCoordPool = -1;
    m_pMaterial = new Material;
    m_bColors = 0;
    m_bFog = 1;
    m_nPartID = m_nNextPartID++;
    Model* pModel = pNode->m_pModel->AsModel();
    if (pModel)
        m_bFog = pModel->m_bModelOptionEnabled;
    counttrimesh++;

    MdlNodeTriMesh* pMesh = pNode->AsMdlNodeTriMesh();
    if (pMesh->m_bAnimateUV)
    {
        m_pOriginalTexCoords = new float[2 * pMesh->m_nVertexCount];
#ifdef _XBOX
        unsigned long nPool = pMesh->m_pModel->AsModel()->m_nVertexPool;
        int nSource = (int)GLRender::LockPool(nPool, 0, 0,
            RequestPoolManager::POOL_LOCK_WRITE);
#else
        int nSource = (int)GLRender::LockPool(
            pMesh->m_pModel->AsModel()->m_nVertexPool, 0, 0,
            RequestPoolManager::POOL_LOCK_READ_ONLY);
#endif
        for (int i = 0; i < pMesh->m_nVertexCount; i++)
        {
            m_pOriginalTexCoords[2 * i] =
                *(float*)(nSource + pMesh->m_nVertexDataOffset +
                        i * pMesh->m_nVertexStride + pMesh->m_nTexCoord0Offset);
            m_pOriginalTexCoords[2 * i + 1] =
                *(float*)(nSource + pMesh->m_nVertexDataOffset +
                        i * pMesh->m_nVertexStride + pMesh->m_nTexCoord0Offset + 4);
        }
        GLRender::UnlockPool(pMesh->m_pModel->AsModel()->m_nVertexPool);
    }
    else
        m_pOriginalTexCoords = 0;
}
// PC 0x00440230  Xbox 0x00296310
PartTriMesh::~PartTriMesh()
{
    if (m_pVertexPrimitive)
    {
        delete m_pVertexPrimitive;
    }

    if (m_bColors == 1)
    {
        unsigned int nPool = m_nColorPool;
        m_bColors = 0;
        GLRender::DeletePool(nPool);
        m_nColorPool = RequestPoolManager::INVALID_POOL;
    }

    if (m_nTexCoordPool != RequestPoolManager::INVALID_POOL)
    {
        GLRender::DeletePool(m_nTexCoordPool);
        m_nTexCoordPool = RequestPoolManager::INVALID_POOL;
    }

    --counttrimesh;

    if (m_pOriginalTexCoords)
    {
        delete m_pOriginalTexCoords;
        m_pOriginalTexCoords = 0;
    }
}
// PC 0x004402F0  Xbox 0x002963D0
Part* Gob::GetPart(char* sName)
{
    if (stricmp("root", sName) != 0)
    {
        return FindPart(m_pRootPart, sName);
    }

    return m_pRootPart;
}
// PC 0x00445A40  Xbox 0x0029A8B0
PartLight::PartLight(MdlNode* pNode)
    : Part(pNode)
{
#ifdef _XBOX
    m_fMultiplier = 1.0f;
    m_fColorMultiplier = 1.0f;
    m_bShadow = 0;
    m_bAmbientOnly = 0;
    m_bFlare = 0;
    m_nLightType = PartLight::LIGHTTYPE_STATIC;
    m_nPriority = 0;
    m_fRadius = 0.0f;
    m_fDynamicMultiplier = 0.0f;
    m_fPriorityValue = 0.0f;
    m_fLightState = 0.0f;
    m_fFlareFade = 0.0f;
    m_fShadowFade = 0.0f;
    m_fShadowRadius = -1.0f;
    m_fShadowHeight = 0.0f;
#else
    m_fMultiplier = 1.0f;
    m_fColorMultiplier = 1.0f;
    m_bShadow = 0;
    m_bAmbientOnly = 0;
    m_bFlare = 0;
    m_nLightType = PartLight::LIGHTTYPE_STATIC;
    m_fRadius = 0.0f;
    m_fDynamicMultiplier = 0.0f;
    m_fPriorityValue = 0.0f;
    m_nLightState = 0;
    m_fFlareFade = 0.0f;
    m_nPriority = 0;
    m_fShadowFade = 0.0f;
    m_fShadowHeight = 0.0f;
    m_fShadowRadius = -1.0f;
#endif
}
// PC 0x00440320  Xbox 0x00296400
PartLight::~PartLight()
{
    for (int i = 0; i < gobs.m_nCount; i++)
    {
        CAurObjectLightContainer* pLights = gobs[i]->m_pLightContainer;
        if (pLights && pLights->m_lstLights.Contains(this))
        {
            int nIndex = pLights->m_lstLights.IndexOf(this);
            if (nIndex >= 0)
            {
                pLights->m_lstLights.DelIndex(nIndex);
                pLights->m_lstFadeIntensities.DelIndex(nIndex);
            }
        }
        if (pLights->m_pShadowLight == this)
        {
            pLights->m_pShadowLight = 0;
            pLights->m_fShadowFade = 0;
        }
    }
}
static int FindGobIdx(char* sName)  // PC 0x00440410  Xbox 0x002964A0
{
    int nLow = 0;
    int nHigh = gobs.m_nCount - 1;

    while (nLow <= nHigh)
    {
        int nMiddle = (nLow + nHigh) >> 1;
        int nCompare = stricmp(gobs[nMiddle]->m_sName, sName);
        if (nCompare > 0)
            nHigh = nMiddle - 1;
        else if (nCompare < 0)
            nLow = nMiddle + 1;
        else
            return nMiddle;
    }

    return nLow;
}
// PC 0x00440460  Xbox 0x002964F0
Gob* FindGob(char* sName)
{
    int nIndex = FindGobIdx(sName);

    if (nIndex < gobs.m_nCount &&
        stricmp(gobs.m_pList[nIndex]->m_sName, sName) == 0)
    {
        return gobs.m_pList[nIndex];
    }

    return 0;
}

const char* setmirrorimage(char* sCommand)
{
    char sName[64];
    int nImposters;

    if (sscanf(sCommand, "%s %d", sName, &nImposters) != 2)
    {
        return "FAILURE";
    }
    Gob* pGob = FindGob(sName);
    if (pGob == 0)
    {
        return "FAILURE";
    }
    float* pfValues = new float[3 * nImposters];
    if (nImposters == 0)
    {
        nImposters = 1;
    }
    for (int i = 0; i < nImposters; i++)
    {
        float fAngle = TWO_PI * i / nImposters;
        pfValues[3 * i] = 3.0f * (float)sin(fAngle);
        pfValues[3 * i + 1] = 3.0f * (float)cos(fAngle);
        pfValues[3 * i + 2] = 0.0f;
    }
    pGob->SetUpImposters(nImposters, pfValues);
    return "SUCCESS";
}
// PC 0x004404B0  Xbox 0x00296620
void Gob::SetUpImposters(int nImposterType, float* pfImposterValues)
{
    AURMODEL_TRACE();
    m_nImposterType = nImposterType;
#ifdef _XBOX
    m_nImposters = 0;
#else
    m_nImposters = GLRender::SetUpImposters(nImposterType, pfImposterValues);
#endif
}
// PC 0x0043E340  Xbox 0x00294A10
MdlNodeAABB* MdlNode::AsMdlNodeAABB()
{
    if ((m_nNodeType & (MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_AABB)) == (MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_AABB))
        return (MdlNodeAABB*)this;
    else
        return 0;
}
// PC 0x0043E360  Xbox 0x00294A30
MdlNodeAnimMesh* MdlNode::AsMdlNodeAnimMesh()
{
    if ((m_nNodeType &
            (MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_ANIMMESH)) ==
                   (MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_ANIMMESH))
        return (MdlNodeAnimMesh*)this;
    else
        return 0;
}
// PC 0x0043E370  Xbox 0x00294A40
MdlNodeCamera* MdlNode::AsMdlNodeCamera()
{
    if ((m_nNodeType & (MDL_NODE_HEADER | MDL_NODE_CAMERA)) ==
                   (MDL_NODE_HEADER | MDL_NODE_CAMERA))
        return (MdlNodeCamera*)this;
    else
        return 0;
}
// PC 0x0043E380  Xbox 0x00294A50
MdlNodeDanglyMesh* MdlNode::AsMdlNodeDanglyMesh()
{
    if ((m_nNodeType & (MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_DANGLYMESH)) ==
        (MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_DANGLYMESH))
        return (MdlNodeDanglyMesh*)this;
    else
        return 0;
}
// PC 0x0043E3A0  Xbox 0x00294A70
MdlNodeLightsaber* MdlNode::AsMdlNodeLightsaber()
{
    if ((m_nNodeType & (MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_LIGHTSABER)) ==
        (MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_LIGHTSABER))
        return (MdlNodeLightsaber*)this;
    else
        return 0;
}
// PC 0x0043E3C0  Xbox 0x00294A90
MdlNodeEmitter* MdlNode::AsMdlNodeEmitter()
{
    if ((m_nNodeType & (MDL_NODE_HEADER | MDL_NODE_EMITTER)) ==
                   (MDL_NODE_HEADER | MDL_NODE_EMITTER))
        return (MdlNodeEmitter*)this;
    else
        return 0;
}
// PC 0x0043E3D0  Xbox 0x00294AA0
MdlNodeLight* MdlNode::AsMdlNodeLight()
{
    if ((m_nNodeType & (MDL_NODE_HEADER | MDL_NODE_LIGHT)) ==
                   (MDL_NODE_HEADER | MDL_NODE_LIGHT))
        return (MdlNodeLight*)this;
    else
        return 0;
}
// PC 0x0043E3E0  Xbox 0x00294AB0
MdlNodeReference* MdlNode::AsMdlNodeReference()
{
    if ((m_nNodeType & (MDL_NODE_HEADER | MDL_NODE_REFERENCE)) ==
                   (MDL_NODE_HEADER | MDL_NODE_REFERENCE))
        return (MdlNodeReference*)this;
    else
        return 0;
}
// PC 0x0043E3F0  Xbox 0x00294AC0
MdlNodeSkin* MdlNode::AsMdlNodeSkin()
{
    if ((m_nNodeType &
            (MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_SKIN)) ==
                   (MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_SKIN))
        return (MdlNodeSkin*)this;
    else
        return 0;
}
// PC 0x004404D0  Xbox 0x00296640
Vector Gob::GetCutsceneDummyPosition()
{
    Part* pPart = FindPart(m_pRootPart, "cutscenedummy");
    if (pPart)
    {
        Vector vPosition;
        Quaternion qOrientation;
        pPart->GetWorld(vPosition, qOrientation);
        return vPosition;
    }
    return Vector();
}
// PC 0x00440550  Xbox 0x002966E0
void PartSetFog(Part* pPart, bool bFog)
{
    PartTriMesh* pMesh = pPart->AsPartTriMesh();
    if (pMesh)
        pMesh->m_bFog = bFog;

    for (int i = 0; i < pPart->m_lstChildren.m_nCount; i++)
        PartSetFog(pPart->m_lstChildren[i], bFog);
}
// PC 0x00440870  Xbox 0x00296720
void Gob::DisableFog()
{
    PartSetFog(m_pRootPart, false);
}
// PC 0x00440880  Xbox 0x00296730
void Gob::EnableFog()
{
    PartSetFog(m_pRootPart, true);
}
// Xbox 0x002967A0
const char* loadaddin(char* sCommand)
{
    char sName[64];
    char sModel[64];

    if (sscanf(sCommand, "%s %s", sName, sModel) != 2)
    {
        return "Incorrect number of params";
    }
    Gob* pGob = FindGob(sName);
    if (pGob == 0)
    {
        return "Couldn't find Gob";
    }
    pGob->LoadAddInAnimations(sModel);
    pGob->PlayAnimation("ca1slashl", 1.0f, 0, 0);
    return "Playing ca1slashl";
}
// PC 0x00440890  Xbox 0x002968B0
int Gob::LoadAddInAnimations(char* sName)
{
    m_pAnimationModel = FindModel(sName);
    if (m_pAnimationModel == 0)
    {
        char sFileName[64];
        strcpy(sFileName, sName);
        strcat(sFileName, ".mdl");
        FILE* pFile = fopen(sFileName, "r");
        if (pFile != 0)
        {
            m_pAnimationModel = LoadModel(pFile, 0);
            m_pAnimationModel->SynchronizeTree(m_pModel);
            for (int i = 0; i < m_pModel->m_lAnimations.m_nCount; ++i)
            {
                ((MaxTree*)m_pModel->m_lAnimations[i])->SynchronizeTree(m_pModel);
            }
        }
        else
            return 0;
    }
    ++m_pAnimationModel->m_nReferenceCount;
    return 1;
}
// PC 0x00440960  Xbox 0x00296FB0
void Part::GetMinimumBoundingBox(Vector& vMinimum, Vector& vMaximum, float fScale)
{
    PartTriMesh* pPart = AsPartTriMesh();
    if (pPart && (pPart->m_pNode->AsMdlNodeLightsaber() ||
                  pPart->m_pNode->AsMdlNodeTriMesh()->m_nVertexCount == 0))
        pPart = 0;

    float fLocalScale = fScale * m_fScale;
    if (pPart)
    {
        Vector vPosition;
        Quaternion qOrientation;
        pPart->GetWorld(vPosition, qOrientation);
        MdlNodeTriMesh* pNode = pPart->m_pNode->AsMdlNodeTriMesh();
        int nVertexPool = pNode->m_pModel->AsModel()->m_nVertexPool;
        int nBuffer = (int)GLRender::LockPool(
            nVertexPool, 0, 0,
            RequestPoolManager::POOL_LOCK_READ_ONLY);
        for (int i = 0; i < pNode->m_lFaces.m_nCount; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                int nVertex = pNode->m_lFaces[i].m_anVertexIndices[j];
                Vector vVertex;
                float* pVertex = (float*)(nBuffer + pNode->m_nVertexDataOffset +
                    pNode->m_nVertexStride * nVertex);
                vVertex.m_fX = pVertex[0];
                vVertex.m_fY = pVertex[1];
                vVertex.m_fZ = pVertex[2];
                vVertex = vVertex * fLocalScale;
                vVertex = vPosition + qOrientation * vVertex;
                vVertex = -pPart->m_pAurObject->m_qOrientation *
                    (vVertex - pPart->m_pAurObject->m_vPosition);
                vMinimum = VectorMin(vMinimum, vVertex);
                vMaximum = VectorMax(vMaximum, vVertex);
            }
        }
        GLRender::UnlockPool(pNode->m_pModel->AsModel()->m_nVertexPool);
    }
    for (int i = 0; i < m_lstChildren.m_nCount; i++)
        m_lstChildren[i]->GetMinimumBoundingBox(vMinimum, vMaximum, fLocalScale);
}
// Xbox 0x00297440
const char* setillumination(char* sCommand)
{
    char sName[64];
    float fRed;
    float fGreen;
    float fBlue;

    sName[0] = 0;
    int nCount = sscanf(sCommand, "%s%f%f%f", sName, &fRed, &fGreen, &fBlue);
    if (nCount != 4)
    {
        return "Usage:  setillumination <gobname> <red> <green> <blue> <optional alpha>";
    }
    Gob* pGob = FindGob(sName);
    if (pGob == 0)
    {
        return "Error:  No gob by that name";
    }
    pGob->SetIllumination(Vector(fRed, fGreen, fBlue), 1);
    return "Success";
}
// PC 0x00440CF0  Xbox 0x00297510
int AnimMeshHitCheck(PartAnimMesh* pPart, MdlNodeAnimMesh* pNode,
                     Vector vStart, Vector vEnd, Vector* pHit, float fScale)
{
    if (pNode == 0 || pPart == 0)
        return 0;
    int nHit = 0;

    int nVertexPool = pPart->m_pVertexPrimitive->AsVertexPrimitiveAnim()->m_nVertexPool;
    float* pVertices = (float*)GLRender::LockPool(
        nVertexPool,
        0, 0, RequestPoolManager::POOL_LOCK_READ_ONLY);
    if (fScale == 1.0f)
    {
        for (int i = 0; i < pNode->m_lFaces.m_nCount; i++)
        {
            Vector vTriangle[3];
            vTriangle[0] = Vector(pVertices[pNode->m_lFaces[i].m_anVertexIndices[0] * 3],
                                  pVertices[pNode->m_lFaces[i].m_anVertexIndices[0] * 3 + 1],
                                  pVertices[pNode->m_lFaces[i].m_anVertexIndices[0] * 3 + 2]);
            vTriangle[1] = Vector(pVertices[pNode->m_lFaces[i].m_anVertexIndices[1] * 3],
                                  pVertices[pNode->m_lFaces[i].m_anVertexIndices[1] * 3 + 1],
                                  pVertices[pNode->m_lFaces[i].m_anVertexIndices[1] * 3 + 2]);
            vTriangle[2] = Vector(pVertices[pNode->m_lFaces[i].m_anVertexIndices[2] * 3],
                                  pVertices[pNode->m_lFaces[i].m_anVertexIndices[2] * 3 + 1],
                                  pVertices[pNode->m_lFaces[i].m_anVertexIndices[2] * 3 + 2]);
            nHit |= polyhit(vTriangle, 3, vStart, vEnd, &vEnd, 0);
        }
    }
    else
    {
        for (int i = 0; i < pNode->m_lFaces.m_nCount; i++)
        {
            Vector vTriangle[3];
            MaxFace& face = pNode->m_lFaces[i];
            vTriangle[0] = Vector(fScale * pVertices[face.m_anVertexIndices[0] * 3],
                                  fScale * pVertices[face.m_anVertexIndices[0] * 3 + 1],
                                  fScale * pVertices[face.m_anVertexIndices[0] * 3 + 2]);
            vTriangle[1] = Vector(fScale * pVertices[face.m_anVertexIndices[1] * 3],
                                  fScale * pVertices[face.m_anVertexIndices[1] * 3 + 1],
                                  fScale * pVertices[face.m_anVertexIndices[1] * 3 + 2]);
            vTriangle[2] = Vector(fScale * pVertices[face.m_anVertexIndices[2] * 3],
                                  fScale * pVertices[face.m_anVertexIndices[2] * 3 + 1],
                                  fScale * pVertices[face.m_anVertexIndices[2] * 3 + 2]);
            nHit |= polyhit(vTriangle, 3, vStart, vEnd, &vEnd, 0);
        }
    }
    nVertexPool = pPart->m_pVertexPrimitive->AsVertexPrimitiveAnim()->m_nVertexPool;
    GLRender::UnlockPool(nVertexPool);
    if (nHit && pHit)
        *pHit = vEnd;
    return nHit;
}
// PC 0x00440FF0  Xbox 0x002978A0
int PartTriMesh::HitCheckGeom(const Vector& vStart, Vector& vEnd,
                              Vector* pHit, CHitInfo* pInfo, float fScale)
{
    if (AsPartSkin())
        return 0;

    MdlNodeTriMesh* pMesh = m_pVertexPrimitive->m_pNode;
    if (pMesh->AsMdlNodeLightsaber() || pMesh->m_nVertexCount == 0)
        return 0;

    int nHit = 0;
    if (pInfo->m_nMask)
    {
        static Vector old;
        old = vEnd;
        if (fScale == 1.0f)
            nHit = BoxIntersect(vStart, vEnd,
                pMesh->m_vMinimum - g_HitCheckBoxExpansion,
                pMesh->m_vMaximum + g_HitCheckBoxExpansion, pHit);
        else
            nHit = BoxIntersect(vStart, vEnd,
                pMesh->m_vMinimum * fScale - g_HitCheckBoxExpansion,
                pMesh->m_vMaximum * fScale + g_HitCheckBoxExpansion, pHit);
        if (nHit)
        {
            if (nHit != -1)
            {
                pInfo->ClearOutput();
                ((float*)&pInfo->m_vOutputNormal)[(nHit - 1) / 2] =
                    (nHit - 1) % 2 ? 1.0f : -1.0f;
            }
            else if (pInfo->m_nResult)
                vEnd = old;
            else
                pInfo->ClearOutput();
            pInfo->m_vOutputPosition = *pHit;
        }
        return nHit;
    }

    if (fScale == 1.0f)
    {
        if (!BoxIntersect(vStart, vEnd, pMesh->m_vMinimum, pMesh->m_vMaximum))
            return 0;
    }
    else
    {
        if (!BoxIntersect(vStart, vEnd, pMesh->m_vMinimum * fScale,
                          pMesh->m_vMaximum * fScale))
            return 0;
    }

#ifdef _XBOX

    int nVertexPool = pMesh->m_pModel->AsModel()->m_nVertexPool;
    unsigned char* pVertices = (unsigned char*)GLRender::LockPool(
        nVertexPool, 0, 0, RequestPoolManager::POOL_LOCK_READ_ONLY);
    Vector vNormal;
#else
    Vector* pVertices = pMesh->m_pVertexPositions;
    Vector vNormal;
#endif
    if (fScale == 1.0f)
    {
        for (int i = 0; i < pMesh->m_lFaces.m_nCount; i++)
        {
            Vector vTriangle[3];
#ifdef _XBOX
            Vector& vVertex0 = *(Vector*)(pVertices + pMesh->m_nVertexDataOffset +
                pMesh->m_nVertexStride * pMesh->m_lFaces[i].m_anVertexIndices[0]);
            vTriangle[0].m_fX = vVertex0.m_fX;
            vTriangle[0].m_fY = vVertex0.m_fY;
            vTriangle[0].m_fZ = vVertex0.m_fZ;
            Vector& vVertex1 = *(Vector*)(pVertices + pMesh->m_nVertexDataOffset +
                pMesh->m_nVertexStride * pMesh->m_lFaces[i].m_anVertexIndices[1]);
            vTriangle[1].m_fX = vVertex1.m_fX;
            vTriangle[1].m_fY = vVertex1.m_fY;
            vTriangle[1].m_fZ = vVertex1.m_fZ;
            Vector& vVertex2 = *(Vector*)(pVertices + pMesh->m_nVertexDataOffset +
                pMesh->m_nVertexStride * pMesh->m_lFaces[i].m_anVertexIndices[2]);
            vTriangle[2].m_fX = vVertex2.m_fX;
            vTriangle[2].m_fY = vVertex2.m_fY;
            vTriangle[2].m_fZ = vVertex2.m_fZ;
#else
            int nVertex0 = pMesh->m_lFaces[i].m_anVertexIndices[0];
            Vector& vVertex0 = pVertices[nVertex0];
            vTriangle[0].m_fX = vVertex0.m_fX;
            vTriangle[0].m_fY = vVertex0.m_fY;
            vTriangle[0].m_fZ = vVertex0.m_fZ;
            int nVertex1 = pMesh->m_lFaces[i].m_anVertexIndices[1];
            Vector& vVertex1 = pVertices[nVertex1];
            vTriangle[1].m_fX = vVertex1.m_fX;
            vTriangle[1].m_fY = vVertex1.m_fY;
            vTriangle[1].m_fZ = vVertex1.m_fZ;
            int nVertex2 = pMesh->m_lFaces[i].m_anVertexIndices[2];
            Vector& vVertex2 = pVertices[nVertex2];
            vTriangle[2].m_fX = vVertex2.m_fX;
            vTriangle[2].m_fY = vVertex2.m_fY;
            vTriangle[2].m_fZ = vVertex2.m_fZ;
#endif
            nHit |= polyhit(vTriangle, 3, pMesh->m_lFaces[i].m_vNormal,
                pMesh->m_lFaces[i].m_fDistance, vStart, vEnd, &vEnd);
        }
    }
    else
    {
        for (int i = 0; i < pMesh->m_lFaces.m_nCount; i++)
        {
            Vector vTriangle[3];
#ifdef _XBOX
            Vector& vVertex0 = *(Vector*)(pVertices + pMesh->m_nVertexDataOffset +
                pMesh->m_nVertexStride * pMesh->m_lFaces[i].m_anVertexIndices[0]);
            vTriangle[0].m_fX = fScale * vVertex0.m_fX;
            vTriangle[0].m_fY = fScale * vVertex0.m_fY;
            vTriangle[0].m_fZ = fScale * vVertex0.m_fZ;
            Vector& vVertex1 = *(Vector*)(pVertices + pMesh->m_nVertexDataOffset +
                pMesh->m_nVertexStride * pMesh->m_lFaces[i].m_anVertexIndices[1]);
            vTriangle[1].m_fX = fScale * vVertex1.m_fX;
            vTriangle[1].m_fY = fScale * vVertex1.m_fY;
            vTriangle[1].m_fZ = fScale * vVertex1.m_fZ;
            Vector& vVertex2 = *(Vector*)(pVertices + pMesh->m_nVertexDataOffset +
                pMesh->m_nVertexStride * pMesh->m_lFaces[i].m_anVertexIndices[2]);
            vTriangle[2].m_fX = fScale * vVertex2.m_fX;
            vTriangle[2].m_fY = fScale * vVertex2.m_fY;
            vTriangle[2].m_fZ = fScale * vVertex2.m_fZ;
#else
            int nVertex0 = pMesh->m_lFaces[i].m_anVertexIndices[0];
            Vector& vVertex0 = pVertices[nVertex0];
            vTriangle[0].m_fX = fScale * vVertex0.m_fX;
            vTriangle[0].m_fY = fScale * vVertex0.m_fY;
            vTriangle[0].m_fZ = fScale * vVertex0.m_fZ;
            int nVertex1 = pMesh->m_lFaces[i].m_anVertexIndices[1];
            Vector& vVertex1 = pVertices[nVertex1];
            vTriangle[1].m_fX = fScale * vVertex1.m_fX;
            vTriangle[1].m_fY = fScale * vVertex1.m_fY;
            vTriangle[1].m_fZ = fScale * vVertex1.m_fZ;
            int nVertex2 = pMesh->m_lFaces[i].m_anVertexIndices[2];
            Vector& vVertex2 = pVertices[nVertex2];
            vTriangle[2].m_fX = fScale * vVertex2.m_fX;
            vTriangle[2].m_fY = fScale * vVertex2.m_fY;
            vTriangle[2].m_fZ = fScale * vVertex2.m_fZ;
#endif
            nHit |= polyhit(vTriangle, 3, pMesh->m_lFaces[i].m_vNormal,
                pMesh->m_lFaces[i].m_fDistance, vStart, vEnd, &vEnd);
        }
    }
    if (nHit && pHit)
    {
        pInfo->ClearOutput();
        pInfo->m_vOutputNormal = vNormal;
        pInfo->m_vOutputPosition = vEnd;
        pInfo->m_vOutputVector = vEnd;
        *pHit = vEnd;
    }
#ifdef _XBOX
    GLRender::UnlockPool(pMesh->m_pModel->AsModel()->m_nVertexPool);
#endif
    return nHit;
}
// PC 0x00441510  Xbox 0x00297ED0
int PartAnimMesh::HitCheckGeom(Vector const& vStart, Vector& vEnd,
                               Vector* pHit, CHitInfo*, float fScale)
{
    MdlNodeAnimMesh* pNode = m_pNode->AsMdlNodeAnimMesh();
    return AnimMeshHitCheck(this, pNode, vStart, vEnd, pHit, fScale);
}
inline void DebugValidatePartMaterial(Part* pPart, char* sName);
// PC 0x00441570  Xbox 0x00297F30
int HitCheckAABBnode(AABB* pNode, const Vector& vStart, Vector& vEnd, float fScale)
{
    ++countboxes;
    if (fScale == 1.0f)
    {
        if (!BoxIntersect(vStart, vEnd, pNode->m_vMinimum, pNode->m_vMaximum))
            return 0;
    }
    else
    {
        if (!BoxIntersect(vStart, vEnd, pNode->m_vMinimum * fScale,
                          pNode->m_vMaximum * fScale))
            return 0;
    }

    int nFace = pNode->m_nFaceIndex;
    if (nFace != AABB::INTERNAL_NODE)
    {
        int nHit = 0;
        MaxFace& face = aabb_mesh->m_lFaces[nFace];
        if (!(aabb_hitinfo->m_nFlags & (1 << face.m_nSurfaceType)))
            return 0;

        Vector vTriangle[3];
#ifdef _XBOX
        int nVertexPool = aabb_mesh->m_pModel->AsModel()->m_nVertexPool;
        unsigned char* pVertices = (unsigned char*)GLRender::LockPool(
            nVertexPool, 0, 0, RequestPoolManager::POOL_LOCK_READ_ONLY);
#else
        Vector* pVertices = aabb_mesh->m_pVertexPositions;
#endif

        if (fScale == 1.0f)
        {
#ifdef _XBOX
            Vector& vVertex0 = *(Vector*)(pVertices + aabb_mesh->m_nVertexDataOffset +
                aabb_mesh->m_nVertexStride * face.m_anVertexIndices[0]);
            vTriangle[0].m_fX = vVertex0.m_fX;
            vTriangle[0].m_fY = vVertex0.m_fY;
            vTriangle[0].m_fZ = vVertex0.m_fZ;
            Vector& vVertex1 = *(Vector*)(pVertices + aabb_mesh->m_nVertexDataOffset +
                aabb_mesh->m_nVertexStride * face.m_anVertexIndices[1]);
            vTriangle[1].m_fX = vVertex1.m_fX;
            vTriangle[1].m_fY = vVertex1.m_fY;
            vTriangle[1].m_fZ = vVertex1.m_fZ;
            Vector& vVertex2 = *(Vector*)(pVertices + aabb_mesh->m_nVertexDataOffset +
                aabb_mesh->m_nVertexStride * face.m_anVertexIndices[2]);
            vTriangle[2].m_fX = vVertex2.m_fX;
            vTriangle[2].m_fY = vVertex2.m_fY;
            vTriangle[2].m_fZ = vVertex2.m_fZ;
#else
            int nVertex0 = face.m_anVertexIndices[0];
            Vector& vVertex0 = pVertices[nVertex0];
            vTriangle[0].m_fX = vVertex0.m_fX;
            vTriangle[0].m_fY = vVertex0.m_fY;
            vTriangle[0].m_fZ = vVertex0.m_fZ;
            int nVertex1 = face.m_anVertexIndices[1];
            Vector& vVertex1 = pVertices[nVertex1];
            vTriangle[1].m_fX = vVertex1.m_fX;
            vTriangle[1].m_fY = vVertex1.m_fY;
            vTriangle[1].m_fZ = vVertex1.m_fZ;
            int nVertex2 = face.m_anVertexIndices[2];
            Vector& vVertex2 = pVertices[nVertex2];
            vTriangle[2].m_fX = vVertex2.m_fX;
            vTriangle[2].m_fY = vVertex2.m_fY;
            vTriangle[2].m_fZ = vVertex2.m_fZ;
#endif
        }
        else
        {
#ifdef _XBOX
            Vector& vVertex0 = *(Vector*)(pVertices + aabb_mesh->m_nVertexDataOffset +
                aabb_mesh->m_nVertexStride * face.m_anVertexIndices[0]);
            vTriangle[0].m_fX = vVertex0.m_fX;
            vTriangle[0].m_fY = vVertex0.m_fY;
            vTriangle[0].m_fZ = vVertex0.m_fZ;
            Vector& vVertex1 = *(Vector*)(pVertices + aabb_mesh->m_nVertexDataOffset +
                aabb_mesh->m_nVertexStride * face.m_anVertexIndices[1]);
            vTriangle[1].m_fX = vVertex1.m_fX;
            vTriangle[1].m_fY = vVertex1.m_fY;
            vTriangle[1].m_fZ = vVertex1.m_fZ;
            Vector& vVertex2 = *(Vector*)(pVertices + aabb_mesh->m_nVertexDataOffset +
                aabb_mesh->m_nVertexStride * face.m_anVertexIndices[2]);
            vTriangle[2].m_fX = vVertex2.m_fX;
            vTriangle[2].m_fY = vVertex2.m_fY;
            vTriangle[2].m_fZ = vVertex2.m_fZ;
#else
            int nVertex0 = face.m_anVertexIndices[0];
            Vector& vVertex0 = pVertices[nVertex0];
            vTriangle[0].m_fX = vVertex0.m_fX;
            vTriangle[0].m_fY = vVertex0.m_fY;
            vTriangle[0].m_fZ = vVertex0.m_fZ;
            int nVertex1 = face.m_anVertexIndices[1];
            Vector& vVertex1 = pVertices[nVertex1];
            vTriangle[1].m_fX = vVertex1.m_fX;
            vTriangle[1].m_fY = vVertex1.m_fY;
            vTriangle[1].m_fZ = vVertex1.m_fZ;
            int nVertex2 = face.m_anVertexIndices[2];
            Vector& vVertex2 = pVertices[nVertex2];
            vTriangle[2].m_fX = vVertex2.m_fX;
            vTriangle[2].m_fY = vVertex2.m_fY;
            vTriangle[2].m_fZ = vVertex2.m_fZ;
#endif
        }
#ifdef _XBOX
        GLRender::UnlockPool(aabb_mesh->m_pModel->AsModel()->m_nVertexPool);
#endif
        if (polyhit(vTriangle, 3, face.m_vNormal, face.m_fDistance,
                    vStart, vEnd, &vEnd))
        {
            nHit = 1;
            aabb_hitinfo->ClearOutput();
            aabb_hitinfo->m_vOutputNormal = face.m_vNormal;
            aabb_hitinfo->m_nOutputIndex = 1 << face.m_nSurfaceType;
            aabb_hitinfo->m_nOutputType = nFace;
            aabb_hitinfo->m_vOutputVector = vEnd;
            aabb_hitinfo->m_vOutputPosition = vEnd;
            aabb_test = nFace;
        }
        return nHit;
    }

    if (aabb_testopposite)
    {
        if (aabb_direction_heuristic & pNode->m_nSplitMask)
            return HitCheckAABBnode(pNode->m_pRightChild, vStart, vEnd, fScale) +
                   HitCheckAABBnode(pNode->m_pLeftChild, vStart, vEnd, fScale);
        else
            return HitCheckAABBnode(pNode->m_pLeftChild, vStart, vEnd, fScale) +
                   HitCheckAABBnode(pNode->m_pRightChild, vStart, vEnd, fScale);
    }
    else
    {
        if (aabb_direction_heuristic & pNode->m_nSplitMask)
            return HitCheckAABBnode(pNode->m_pLeftChild, vStart, vEnd, fScale) +
                   HitCheckAABBnode(pNode->m_pRightChild, vStart, vEnd, fScale);
        else
            return HitCheckAABBnode(pNode->m_pRightChild, vStart, vEnd, fScale) +
                   HitCheckAABBnode(pNode->m_pLeftChild, vStart, vEnd, fScale);
    }

}
// PC 0x004428F0  Xbox 0x002982B0
int PartAABB::HitCheckGeom(const Vector& vStart, Vector& vEnd,
                           Vector* pHit, CHitInfo* pInfo, float fScale)
{
    aabb_direction_heuristic = 0;
    aabb_mesh = m_pNode->AsMdlNodeTriMesh();
    aabb_hitinfo = pInfo;
    aabb_direction_heuristic |= (1 << (vStart.m_fX < vEnd.m_fX ? AABB::POSITIVE_X : AABB::NEGATIVE_X));
    aabb_direction_heuristic |= (1 << (vStart.m_fY < vEnd.m_fY ? AABB::POSITIVE_Y : AABB::NEGATIVE_Y));
    aabb_direction_heuristic |= (1 << (vStart.m_fZ < vEnd.m_fZ ? AABB::POSITIVE_Z : AABB::NEGATIVE_Z));
    int nHit = HitCheckAABBnode(m_pNode->AsMdlNodeAABB()->m_pAABBData,
                               vStart, vEnd, fScale);
    if (nHit && pHit)
        *pHit = vEnd;
    return nHit;
}
// PC 0x004429D0  Xbox 0x00298370
CAurPart* Part::HitCheck(Vector vStart, Vector vEnd, Vector* pHit,
                        CHitInfo* pInfo, float fScale)
{
    CAurPart* pHitPart = 0;
    fScale *= m_fScale;
    static Vector vTemp;
    if (fScale == 1.0f)
    {
        vTemp.m_fX = vStart.m_fX -  m_vPosition.m_fX;
        vTemp.m_fY = vStart.m_fY -  m_vPosition.m_fY;
        vTemp.m_fZ = vStart.m_fZ -  m_vPosition.m_fZ;
        vStart = -m_qOrientation * vTemp;
        vTemp.m_fX = vEnd.m_fX -  m_vPosition.m_fX;
        vTemp.m_fY = vEnd.m_fY -  m_vPosition.m_fY;
        vTemp.m_fZ = vEnd.m_fZ -  m_vPosition.m_fZ;
        vEnd = -m_qOrientation * vTemp;
    }
    else
    {
        vTemp.m_fX = vStart.m_fX - fScale * m_vPosition.m_fX;
        vTemp.m_fY = vStart.m_fY - fScale * m_vPosition.m_fY;
        vTemp.m_fZ = vStart.m_fZ - fScale * m_vPosition.m_fZ;
        vStart = -m_qOrientation * vTemp;
        vTemp.m_fX = vEnd.m_fX - fScale * m_vPosition.m_fX;
        vTemp.m_fY = vEnd.m_fY - fScale * m_vPosition.m_fY;
        vTemp.m_fZ = vEnd.m_fZ - fScale * m_vPosition.m_fZ;
        vEnd = -m_qOrientation * vTemp;
    }

    int nHit = HitCheckGeom(vStart, vEnd, &vEnd, pInfo, fScale);
    if (nHit && (!pInfo->m_nResult || nHit != HIT_GEOM_EXCLUDED))
    {
        pHitPart = this;
        hitpartname = Name();
        pInfo->m_sOutputPartName = Name();
    }
    for (int i = 0; i < m_lstChildren.m_nCount; i++)
    {
        CAurPart* pChildHit = m_lstChildren[i]->HitCheck(vStart, vEnd, &vEnd,
                                                       pInfo, fScale);
        if (pChildHit)
            pHitPart = pChildHit;
    }
    if (pHitPart)
    {
        if (fScale == 1.0f)
        {
            vTemp = m_qOrientation * vEnd;
            vTemp.m_fX += m_vPosition.m_fX;
            vTemp.m_fY += m_vPosition.m_fY;
            vTemp.m_fZ += m_vPosition.m_fZ;
            if (pInfo)
            {
                pInfo->m_vOutputNormal = m_qOrientation * pInfo->m_vOutputNormal;
                pInfo->m_vOutputPosition = vTemp;
            }
            if (pHit)
                *pHit = vTemp;
        }
        else
        {
            vTemp = m_qOrientation * vEnd;
            vTemp.m_fX += fScale * m_vPosition.m_fX;
            vTemp.m_fY += fScale * m_vPosition.m_fY;
            vTemp.m_fZ += fScale * m_vPosition.m_fZ;
            if (pInfo)
            {
                pInfo->m_vOutputNormal = m_qOrientation * pInfo->m_vOutputNormal;
                pInfo->m_vOutputPosition = vTemp;
            }
            if (pHit)
                *pHit = vTemp;
        }
    }
    return pHitPart;
}
// PC 0x00442D70  Xbox 0x002987A0
void PartInitMaterial(Gob* pGob, Part* pPart, char* sName)
{
    PartTriMesh* pTriMesh = pPart->AsPartTriMesh();
    if (pTriMesh)
    {
        if (pTriMesh->m_pNode->AsMdlNodeTriMesh()->m_bAnimateUV &&
            pTriMesh->m_pNode->AsMdlNodeTriMesh()->m_nTexCoord0Offset !=
                MdlNodeTriMesh::NO_VERTEX_ARRAY_OFFSET && pGob)
        {
            pGob->m_bAnimateUV = 1;
        }
        if (pTriMesh->m_pMaterial)
            pTriMesh->m_pMaterial->LoadTextures(pPart);
        if (pTriMesh->AsPartSkin())
            pTriMesh->m_pVertexPrimitive =
                new VertexPrimitiveSkinned(pTriMesh->AsPartSkin());
        else if (pTriMesh->AsPartAnimMesh())
            pTriMesh->m_pVertexPrimitive = new VertexPrimitiveAnim(pTriMesh);
        else if (pTriMesh->AsPartDanglyMesh())
            pTriMesh->m_pVertexPrimitive = new VertexPrimitiveDangly(pTriMesh);
        else
            pTriMesh->m_pVertexPrimitive = new VertexPrimitiveFlat(pTriMesh);
    }
    for (int i = 0; i < pPart->m_lstChildren.m_nCount; i++)
    {
        PartInitMaterial(pGob, pPart->m_lstChildren[i], sName);
    }
    DebugValidatePartMaterial(pPart, sName);
}
// PC 0x00443080  Xbox 0x00298920
void Part::GetMinimumSphere(Vector& vPosition, float& fRadius)
{
    Vector vCenter;
    float fSphereRadius = 0.0f;
    PartEmitter* pEmitter = AsPartEmitter();
    if (pEmitter && (m_pAurObject->m_pModel->m_Classification.m_bEmitterBounds))
        fSphereRadius = pEmitter->m_fRadius;

    if (m_bIncludeChildBounds || alwaysdominspheres)
    {
        for (int i = 0; i < m_lstChildren.m_nCount; i++)
        {
            Vector vChildCenter;
            float fChildRadius = 0.0f;
            m_lstChildren[i]->GetMinimumSphere(vChildCenter, fChildRadius);
            vChildCenter = m_lstChildren[i]->m_qOrientation * vChildCenter + m_lstChildren[i]->m_vPosition;

            float fLargeRadius = fSphereRadius < fChildRadius ? fChildRadius : fSphereRadius;
            float fSmallRadius = fSphereRadius < fChildRadius ? fSphereRadius : fChildRadius;
            Vector& vLargeCenter = fSphereRadius < fChildRadius ? vChildCenter : vCenter;
            float fDistance = magnitude(vCenter - vChildCenter);
            if (fDistance + fSmallRadius <= fLargeRadius || fDistance == 0.0f)
            {
                fSphereRadius = fLargeRadius;
                vCenter = vLargeCenter;
            }
            else
            {
                float fRadiusRatio = fSphereRadius / fDistance;
                float fChildRatio = -fChildRadius / fDistance;
                float fBlend = (fChildRatio + fRadiusRatio + 1.0f) * 0.5f;
                fSphereRadius = (fRadiusRatio + 1.0f - fBlend) * fDistance;
                vCenter = (vCenter - vChildCenter) * fBlend + vChildCenter;
            }
        }
    }
    vPosition = vCenter;
    fRadius = fSphereRadius;
}
// PC 0x00443330  Xbox 0x00298BE0
void PartTriMesh::GetMinimumSphere(Vector& vPosition, float& fRadius)
{
    Part::GetMinimumSphere(vPosition, fRadius);

    MdlNodeTriMesh* pNode = m_pNode->AsMdlNodeTriMesh();
    float fMeshRadius = pNode->m_fSphereRadius;
    Vector vMeshCenter = pNode->m_vSphereCenter;
#ifdef _XBOX

    float fLargeRadius = fRadius > fMeshRadius ? fRadius : fMeshRadius;
    float fSmallRadius = fRadius > fMeshRadius ? fMeshRadius : fRadius;
    Vector& vLargeCenter = fRadius > fMeshRadius ? vPosition : vMeshCenter;
    float fDistance;
    if ((fDistance = magnitude(vMeshCenter - vPosition)) + fSmallRadius <= fLargeRadius ||
        fDistance == 0.0f)
    {
        fRadius = fLargeRadius;
        vPosition = vLargeCenter;
    }
    else
    {
        float fFraction = ((-1.0f / fDistance) * fRadius + fMeshRadius / fDistance + 1.0f) * 0.5f;
        fRadius = (fMeshRadius / fDistance + 1.0f - fFraction) * fDistance;
        vPosition = vPosition + (vMeshCenter - vPosition) * fFraction;
    }
    return;
#else
    float fSmallRadius;
    float fLargeRadius;
    if (fMeshRadius < fRadius)
    {
        fSmallRadius = fMeshRadius;
        fLargeRadius = fRadius;
    }
    else
    {
        fSmallRadius = fRadius;
        fLargeRadius = fMeshRadius;
        vPosition = vMeshCenter;
    }

    float fDistance = magnitude(vMeshCenter - vPosition);
    if (fDistance + fSmallRadius <= fLargeRadius || fDistance == 0.0f)
    {
        fRadius = fLargeRadius;
    }
    else
    {
        float fOffset = (fDistance - fRadius + fMeshRadius) * 0.5f;
        fRadius = fDistance + fRadius - fOffset;
        vPosition = (vMeshCenter - vPosition) * (fOffset / fDistance) + vPosition;
    }
#endif
}
// PC 0x004434A0  Xbox 0x00298E10
int PartSetCanDownSample(Part* pPart, char bCanDownSample)
{
    int nChanged = 0;
    MdlNodeTriMesh* pMesh = pPart->m_pNode->AsMdlNodeTriMesh();
    if (pMesh != 0 && pMesh->m_sTexture0[0] != 0 &&
        strcmp("NULL", pMesh->m_sTexture0) != 0)
    {
        pPart->m_pMaterial->SetDownSampling(bCanDownSample);
        nChanged = 1;
    }
    for (int i = 0; i < pPart->m_lstChildren.m_nCount; i++)
        nChanged += PartSetCanDownSample(pPart->m_lstChildren[i], bCanDownSample);
    return nChanged;
}
// PC 0x0043E400  Xbox 0x00294AF0
MdlNodeTriMesh* MdlNode::AsMdlNodeTriMesh()
{
    if ((m_nNodeType & (MDL_NODE_HEADER | MDL_NODE_TRIMESH)) ==
                   (MDL_NODE_HEADER | MDL_NODE_TRIMESH))
        return (MdlNodeTriMesh*)this;
    else
        return 0;
}
// PC 0x0043E410  Xbox 0x00294B00
unsigned char MdlNodeTriMesh::HasTangentBases()
{
    return m_TangentBases != -1;
}
// PC 0x0043E420  Xbox 0x00294B10
void MdlNodeAnimMeshGenVertices(void* pNode)
{
    return ((MdlNodeAnimMesh*)pNode)->InternalGenVertices();
}
// PC 0x0043E430  Xbox 0x00294B20
void MdlNodeAnimMeshRemoveTemporaryArrays(void* pNode)
{
    return ((MdlNodeAnimMesh*)pNode)->InternalRemoveTemporaryArrays();
}
// PC 0x0043E440  Xbox 0x00294B30
void MdlNodeSkinGenVertices(void* pNode)
{
    return ((MdlNodeSkin*)pNode)->InternalGenVertices();
}
// PC 0x0043E450  Xbox 0x00294B40
void MdlNodeSkinRemoveTemporaryArrays(void* pNode)
{
    return ((MdlNodeSkin*)pNode)->InternalRemoveTemporaryArrays();
}
// PC 0x0043E460  Xbox 0x00294B50
void MdlNodeDanglyMeshRemoveTemporaryArrays(void* pNode)
{
    return ((MdlNodeDanglyMesh*)pNode)->InternalRemoveTemporaryArrays();
}
// PC 0x0043E470  Xbox 0x00294B60
void MdlNodeDanglyMeshGenVertices(void* pNode)
{
    return ((MdlNodeDanglyMesh*)pNode)->InternalGenVertices();
}
// PC 0x0043E480  Xbox 0x00294B70
void MdlNodeTriMeshGenVertices(void* pNode)
{
    return ((MdlNodeTriMesh*)pNode)->InternalGenVertices();
}
// PC 0x0043E490  Xbox 0x00294B80
void MdlNodeTriMeshRemoveTemporaryArrays(void* pNode)
{
    return ((MdlNodeTriMesh*)pNode)->InternalRemoveTemporaryArrays();
}
// PC 0x0043E4A0  Xbox 0x00294B90
void Part::GetWorld(Vector& vPosition, Quaternion& qOrientation)
{
    Part* pPart = this;
    vPosition = Vector(0.0f, 0.0f, 0.0f);
    qOrientation = Quaternion(1.0f, 0.0f, 0.0f, 0.0f);

    Gob* pGob = m_pAurObject;
    if (pGob == 0)
    {
        vPosition = m_vPosition;
        qOrientation = m_qOrientation;
        return;
    }

    do
    {
        vPosition *= pPart->m_fScale;
        vPosition = pPart->m_qOrientation * vPosition;
        vPosition = pPart->m_vPosition + vPosition;
        qOrientation = pPart->m_qOrientation * qOrientation;
        pPart = pPart->m_pParent;
    } while (pPart != 0);

    if (pGob->m_fScale != 1.0f)
    {
        vPosition *= pGob->m_fScale;
    }
    vPosition = pGob->m_qOrientation * vPosition;
    vPosition = pGob->m_vPosition + vPosition;
    qOrientation = pGob->m_qOrientation * qOrientation;
}
// PC 0x0043E6A0  Xbox 0x00294DB0
void Part::GetLocal(Vector& vPosition, Quaternion& qOrientation)
{
    float fScale = 1.0f;
    if (m_pParent)
    {
        m_pParent->GetLocal(vPosition, qOrientation);
        fScale = m_pParent->m_fScale;
    }
    else if (m_pAurObject)
    {
        qOrientation = -m_pAurObject->m_qOrientation * qOrientation;
        vPosition = -m_pAurObject->m_qOrientation * (vPosition - m_pAurObject->m_vPosition);
        fScale = m_pAurObject->m_fScale;
    }

    qOrientation = -m_qOrientation * qOrientation;
    vPosition = -m_qOrientation * (vPosition - m_vPosition);
    vPosition *= 1.0f / fScale;
}
// PC 0x0043E830  Xbox 0x00294F70
void Gob::EnableForceAnimations()
{
    m_bForceAnimated = 1;
}
// PC 0x0043E840  Xbox 0x00294F80
void Gob::DisableForceAnimations()
{
    m_bForceAnimated = 0;
}
// PC 0x0043E850  Xbox 0x00294F90
bool Gob::IsForceAnimated()
{
    return m_bForceAnimated;
}
// PC 0x0043E860  Xbox 0x00294FA0
void Gob::EnableAlwaysRender()
{
    m_bAlwaysRendering = 1;
}
// PC 0x0043E870  Xbox 0x00294FB0
void Gob::DisableAlwaysRender()
{
    m_bAlwaysRendering = 0;
}
// PC 0x0043E880  Xbox 0x00294FC0
void Gob::EnableInCutscene()
{
    m_bInCutscene = 1;
}
// PC 0x0043E890  Xbox 0x00294FD0
void Gob::DisableInCutscene()
{
    m_bInCutscene = 0;
}
// PC 0x0043E8A0  Xbox 0x00294FE0
bool Gob::IsInCutscene()
{
    return m_bInCutscene;
}
// PC 0x0043E8B0  Xbox 0x00294FF0
bool Gob::IsAlwaysRendering()
{
    return m_bAlwaysRendering;
}
// PC 0x0043E8C0  Xbox 0x00295000
void Gob::GetBoundingBox(Vector& vMinimum, Vector& vMaximum)
{
    vMinimum = m_vBoundingBoxMinimum;
    vMaximum = m_vBoundingBoxMaximum;
}
// PC 0x0043E900  Xbox 0x00295040
void Gob::EnableInViewVolume()
{
    m_bInViewVolume = 1;
    DoEvent("propagateviewvolume", 0);
}
// PC 0x0043E920  Xbox 0x00295060
void Gob::DisableInViewVolume()
{
    m_bInViewVolume = 0;
}
// PC 0x0043E930  Xbox 0x00296980
CAurObject* Gob::HitCheckPart(CAurPart* pPart, Vector vStart, Vector vEnd,
                            Vector* pHit, CHitInfo* pInfo)
{
    if (!pInfo)
        pInfo = &DefaultHitInfo;
    if (!m_pModel || IsAllRooms() ||
        (pInfo->m_bResult2 && m_bIgnoreHitCheck))
        return 0;
    if (pInfo->m_bResult1)
    {
        if (bool(m_pModel->m_Classification.m_bDoor))
        {
            if (m_fOcclusionAlpha <= mindooralpha)
                return 0;
        }
        else if (m_fAlpha <= mindooralpha)
            return 0;
    }

    int nHit = 0;
    vStart = -m_qOrientation * (vStart - m_vPosition);
    vEnd = -m_qOrientation * (vEnd - m_vPosition);
    if (!SphereIntersect(vStart, vEnd, m_vMinimumBoundingSphere,
                         m_fMinimumBoundingSphereRadius))
        return 0;
    if (pPart && pPart->HitCheck(vStart, vEnd, &vEnd, pInfo, 1.0f))
    {
        nHit = 1;
        vEnd = m_vPosition + m_qOrientation * vEnd;
        pInfo->m_vOutputPosition = vEnd;
        pInfo->m_vOutputNormal = m_qOrientation * pInfo->m_vOutputNormal;
        if (pHit)
            *pHit = vEnd;
    }
    return nHit ? this : 0;
}
// PC 0x0043EB80  Xbox 0x00296C10
CAurObject* Gob::HitCheck(Vector vStart, Vector vEnd, Vector* pHit,
                           CHitInfo* pInfo)
{
    if (!pInfo)
        pInfo = &DefaultHitInfo;
    if (!m_pModel || IsAllRooms() ||
        (pInfo->m_bResult2 && m_bIgnoreHitCheck))
        return 0;

    if (pInfo->m_bResult1)
    {
        if (bool(m_pModel->m_Classification.m_bDoor))
        {
            if (m_fOcclusionAlpha <= mindooralpha)
                return 0;
        }
        else if (m_fAlpha <= mindooralpha)
            return 0;
    }

    int bHit = 0;
    vStart = -m_qOrientation * (vStart - m_vPosition);
    vEnd = -m_qOrientation * (vEnd - m_vPosition);
    if (!SphereIntersect(vStart, vEnd, m_vMinimumBoundingSphere,
                         m_fMinimumBoundingSphereRadius))
        return 0;

    if (m_pRootPart &&
        m_pRootPart->HitCheck(vStart, vEnd, &vEnd, pInfo, m_fScale))
    {
        bHit = 1;
        vEnd = m_vPosition + m_qOrientation * vEnd;
        pInfo->m_vOutputPosition = vEnd;
        pInfo->m_vOutputNormal = m_qOrientation * pInfo->m_vOutputNormal;
        if (pHit)
            *pHit = vEnd;
    }
    return bHit ? this : 0;
}
// PC 0x0043EDD0  Xbox 0x00295070
void Gob::SetWind(bool bWind, float fWindY, float fWindX, float fWindZ)
{
    m_bWind = bWind;
    m_fWindY = fWindY;
    m_fWindX = fWindX;
    m_fWindZ = fWindZ;
}
// PC 0x0043EE00  Xbox 0x002950B0
void Gob::EnableRenderGobBBox(Vector const& vBoundingBox)
{
    m_bRenderGobBBox = 1;
    m_vRenderGobBBox = vBoundingBox;
}
// PC 0x0043EE30  Xbox 0x002950E0
void Gob::DisableRenderGobBBox()
{
    m_bRenderGobBBox = 0;
}
// PC 0x0043EE40  Xbox 0x002950F0
int PartHasMaterial(CAurPart* pPart)
{
    return ((Part*)pPart)->m_pMaterial != 0;
}
// PC 0x0043EE50  Xbox 0x00295100
void Gob::SetColorShifting(Vector vColor, float fAlpha, int nFlags)
{
    m_vColor = vColor;
#ifdef _XBOX
    m_bColorShifting = 1;
    m_fAlpha = fAlpha;
#else
    m_fAlpha = fAlpha;
    m_bColorShifting = 1;
#endif
    if (nFlags)
    {
        DoEvent("propagatecolorshift", 0);
    }
}
// PC 0x0043EEA0  Xbox 0x00295150
void Gob::SetIllumination(Vector vColor, int nFlags)
{
    m_vIllumination = vColor;
    m_bIllumination = 1;
    if (nFlags)
    {
        DoEvent("propagateillumination", 0);
    }
}
// PC 0x0043EEE0  Xbox 0x00295190
void Gob::GetMinimumBoundingBox()
{
    m_vBoundingBoxMinimum = Vector(10000.0f, 10000.0f, 10000.0f);
    m_vBoundingBoxMaximum = Vector(-10000.0f, -10000.0f, -10000.0f);
    m_pRootPart->GetMinimumBoundingBox(m_vBoundingBoxMinimum,
                                       m_vBoundingBoxMaximum, m_fScale);
}

Vector defaultspecular;
List<Vector> MaxFace_PostProcess::texindices1;
List<Vector> MaxFace_PostProcess::texindices2;
List<Vector> MaxFace_PostProcess::texindices3;
List<Model*> Models;
List<Gob*> gobs;
Vector g_HitCheckBoxExpansion;
// PC 0x0043EF70  Xbox 0x00295230
int Part::HitCheckGeom(const Vector&, Vector&, Vector*, CHitInfo*, float)
{
    return 0;
}
// PC 0x0043EF80  Xbox 0x00295240
void Gob::GetMinimumBoundingBox(Vector& vMinimum, Vector& vMaximum)
{
    GetMinimumBoundingBox();
    vMinimum = m_vBoundingBoxMinimum;
    vMaximum = m_vBoundingBoxMaximum;
}

inline void DebugValidatePartMaterial(Part* pPart, char* sName)
{
}
// PC 0x0043EFD0  Xbox 0x00295290
void Gob::GetMinimumBoundingSphere(Vector& vPosition, float& fRadius)
{
    GetMinimumSphere();
    vPosition = m_vMinimumBoundingSphere;
    fRadius = m_fMinimumBoundingSphereRadius;
}
// PC 0x0043F010  Xbox 0x002952D0
void Gob::GetMinimumSphere()
{
    if (m_pRootPart)
    {
        m_pRootPart->GetMinimumSphere(m_vMinimumBoundingSphere,
                                      m_fMinimumBoundingSphereRadius);
        m_vMinimumBoundingSphere = m_vMinimumBoundingSphere + m_pRootPart->m_vPosition;
        m_vMinimumBoundingSphere = m_vMinimumBoundingSphere * m_fScale;
        m_fMinimumBoundingSphereRadius = m_fScale * m_fMinimumBoundingSphereRadius * gobspherescaling;
    }
}
// PC 0x0043F0C0  Xbox 0x002953F0
Vector Gob::SetPosition(Vector vPosition)
{
    m_vPosition = vPosition;
    return m_vPosition;
}
// PC 0x0043F0F0  Xbox 0x00295420
Quaternion Gob::SetOrientation(Quaternion qOrientation)
{
    m_qOrientation = qOrientation;
    return m_qOrientation;
}
// PC 0x0043F130  Xbox 0x00295460
CAurBehavior* Gob::GetBehavior(int nIndex)
{
    if (nIndex != -1)
    {
#ifdef _XBOX
        if (!m_pBehavior || m_pBehavior->GetType() != nIndex)
        {
            return 0;
        }
#else
        CAurBehavior* pBehavior = m_pBehavior;
        if (!pBehavior || pBehavior->GetType() != nIndex)
        {
            return 0;
        }
#endif
    }

    return m_pBehavior;
}
// PC 0x0043F170  Xbox 0x002954A0
CAurObject* Gob::HitCheckHack(Vector vStart, Vector vEnd, Vector* pHit)
{
    return HitCheck(vStart, vEnd, pHit, 0);
}
// PC 0x0043F1C0  Xbox 0x002954F0
char* Gob::GetModelName()
{
    if (m_pModel == NULL)
        return NULL;
    return m_pModel->m_sName;
}
// PC 0x0043F1D0  Xbox 0x00295500
void Gob::SetBehavior(CAurBehavior* pBehavior)
{
    CAurBehavior* pOldBehavior = m_pBehavior;
    m_pBehavior = pBehavior;

    if (pOldBehavior)
    {
        delete pOldBehavior;
    }
}
// PC 0x004437F0  Xbox 0x00298E90
int Gob::SetCanDownSample(char bCanDownSample)
{
    if (m_bCanDownSample == bCanDownSample)
    {
        return 0;
    }

    m_bCanDownSample = bCanDownSample;
    return PartSetCanDownSample(m_pRootPart, bCanDownSample);
}
// PC 0x00443820  Xbox 0x00298F40
CAurBehavior* Gob::AttachToObject(CAurObject* pObject, char* sPart, int nFlags)
{
    CAurBehavior* pBehavior = 0;
    Gob* pParent = (Gob*)pObject;
    CAurPart* pPart = pParent ? pParent->GetPart(sPart) : 0;
    if (pPart)
    {
        pPart->GetWorld(m_vPosition, m_qOrientation);
        switch (nFlags)
        {
        case ATTACH_NORMAL:
            pBehavior = new CAurBehaviorAttach(this, pObject, pPart);
            break;
        case ATTACH_TRANSLATE:
            pBehavior = new CAurBehaviorAttachTranslate(this, pObject, pPart);
            break;
        case ATTACH_ROTATE:
            pBehavior = new CAurBehaviorAttachRotate(this, pObject, pPart);
            break;
        case ATTACH_NO_ORIENTATION:
            pBehavior = new CAurBehaviorAttachNoOrientation(this, pObject, pPart);
            break;
        }
        SetBehavior(pBehavior);
        SetScene(pParent->m_pScene);
    }
    else
    {
        SetBehavior(0);
    }
    return pBehavior;
}
// PC 0x004439A0  Xbox 0x00299090
CAurBehavior* Gob::AttachToPart(CAurObject* pObject, CAurPart* pPart, int nFlags)
{
    pPart->GetWorld(m_vPosition, m_qOrientation);

    CAurBehavior* pBehavior;
    switch (nFlags)
    {
    case ATTACH_NORMAL:
        pBehavior = new CAurBehaviorAttach(this, pObject, pPart);
        break;
    case ATTACH_TRANSLATE:
        pBehavior = new CAurBehaviorAttachTranslate(this, pObject, pPart);
        break;
    case ATTACH_ROTATE:
        pBehavior = new CAurBehaviorAttachRotate(this, pObject, pPart);
        break;
    case ATTACH_NO_ORIENTATION:
        pBehavior = new CAurBehaviorAttachNoOrientation(this, pObject, pPart);
        break;
    }

    SetBehavior(pBehavior);
    SetScene(((Gob*)pObject)->m_pScene);
    return pBehavior;
}
// PC 0x0043F200  Xbox 0x00295530
void Gob::SetScene(Scene* pScene)
{
    if (m_pScene != pScene)
    {
        if (m_pScene)
        {
            m_pScene->ChildRemove(this);
        }

        m_pScene = pScene;
        if (m_pScene)
        {
            m_pScene->ChildAdd(this);
        }

        DoEvent("UpdateScenes", 0);
    }
}
// PC 0x0043F250  Xbox 0x00295580
void Gob::InheritExternal(CAurObject* pSource)
{
    Gob* pGob = (Gob*)pSource;

    m_pExternalData = pGob->m_pExternalData;
    m_nExternalDataType = pGob->m_nExternalDataType;
}
// PC 0x0043F270  Xbox 0x002955A0
void Gob::SetProcessFlag()
{
    m_bProcessFlag = 1;
}
// PC 0x0043F280  Xbox 0x002955B0
int Gob::SetPartPosition(char* sPart, Vector vPosition,
                         Quaternion qOrientation)
{
    Part* pPart = GetPart(sPart);
    if (pPart == 0)
    {
        return 0;
    }

    pPart->m_vPosition = Vector(0.0f, 0.0f, 0.0f);
    pPart->m_qOrientation = Quaternion(1.0f, 0.0f, 0.0f, 0.0f);

    Vector vWorldPosition;
    Quaternion qWorldOrientation;
    pPart->GetWorld(vWorldPosition, qWorldOrientation);

    pPart->m_vPosition = -qWorldOrientation * (vPosition - vWorldPosition);
    pPart->m_qOrientation = normalize(-qWorldOrientation * qOrientation);
    pPart->m_nInstanceState = 1;
    return 1;
}
// PC 0x0043F430  Xbox 0x00295760
int Gob::GetPartPosition(char* sPart, Vector* pPosition,
                         Quaternion* pOrientation)
{
    Part* pPart = GetPart(sPart);
    if (pPart == 0)
    {
        return 0;
    }

    Vector vPosition = pPart->m_vPosition;
    Quaternion qOrientation = pPart->m_qOrientation;
    pPart->GetWorld(vPosition, qOrientation);

    if (pPosition)
    {
        *pPosition = vPosition;
    }
    if (pOrientation)
    {
        *pOrientation = qOrientation;
    }
    return 1;
}
// PC 0x0043F4E0  Xbox 0x00295810
int Gob::SetPartLocalPosition(char* sPart, Vector vPosition,
                              Quaternion qOrientation)
{
    Part* pPart = GetPart(sPart);
    if (pPart == 0)
    {
        return 0;
    }
    pPart->m_vPosition = vPosition;
    pPart->m_qOrientation = qOrientation;
    pPart->m_nInstanceState = 1;
    return 1;
}
// PC 0x0043F540  Xbox 0x00295870
int Gob::GetPartLocalPosition(char* sPart, Vector* pPosition,
                              Quaternion* pOrientation)
{
    Part* pPart = GetPart(sPart);
    if (pPart == 0)
    {
        return 0;
    }
    if (pPosition)
    {
        *pPosition = pPart->m_vPosition;
    }
    if (pOrientation)
    {
        *pOrientation = pPart->m_qOrientation;
    }
    return 1;
}
// PC 0x0043F5A0  Xbox 0x002958D0
void Gob::BehaviorMessage(char* sMessage)
{
    if (m_pBehavior)
    {
        m_pBehavior->Message(sMessage);
    }
}
// PC 0x0043F5C0  Xbox 0x002958F0
Vector Part::SetPartPosition(Vector vPosition)
{
    return Vector();
}
// PC 0x00443B30  Xbox 0x00299390
char TreeHasSingleTexture(Part* pPart)
{
    PartTriMesh* pMesh = pPart->AsPartTriMesh();
    Gob* pGob = pPart->m_pAurObject;
    if (!pGob)
        return 0;

    if (pMesh)
    {
        Material* pSingleTexture = pGob->m_pSingleTexture;
        Material* pMaterial = pMesh->m_pMaterial;
        if (!pSingleTexture)
            pGob->m_pSingleTexture = pMaterial;
        else if (pMaterial->m_pTexture0)
        {
            if (pSingleTexture->m_pTexture0 != pMesh->m_pMaterial->m_pTexture0)
                return 0;
        }
        else if (pSingleTexture->m_pTexture0)
            return 0;
    }

    for (int i = 0; i < pPart->m_lstChildren.m_nCount; i++)
    {
        char bSingleTexture = TreeHasSingleTexture(pPart->m_lstChildren[i]);
        if (!bSingleTexture)
            return 0;
    }
    return 1;
}
// PC 0x00443D70  Xbox 0x00299410
char TreeHasTransparency(Part* pPart)
{
    PartTriMesh* pMesh = pPart->AsPartTriMesh();
    if (!pPart->m_pAurObject)
        return false;
    if (pMesh)
    {
        if (pMesh->m_fAlpha < 1.0f)
            return true;
    }

    for (int i = 0; i < pPart->m_lstChildren.m_nCount; i++)
    {
        char bHasTransparency = TreeHasTransparency(pPart->m_lstChildren[i]);
        if (bHasTransparency)
            return true;
    }
    return false;
}
// PC 0x004440D0  Xbox 0x00299480
char TreeHasGeometry(Part* pPart)
{
    PartTriMesh* pMesh = pPart->AsPartTriMesh();
    if (!pPart->m_pAurObject)
        return false;
    if (pMesh)
        return true;

    for (int i = 0; i < pPart->m_lstChildren.m_nCount; i++)
    {
        char bHasGeometry = TreeHasGeometry(pPart->m_lstChildren[i]);
        if (bHasGeometry)
            return true;
    }
    return false;
}
// PC 0x004443E0  Xbox 0x002994E0
char TreeSetGeometryFlags(Part* pPart)
{
    pPart->m_bIncludeChildBounds = 0;
    for (int i = 0; i < pPart->m_lstChildren.m_nCount; i++)
    {
        if (TreeSetGeometryFlags(pPart->m_lstChildren[i]))
            pPart->m_bIncludeChildBounds = 1;
    }
    if (pPart->AsPartTriMesh())
        pPart->m_bIncludeChildBounds = 1;
    return pPart->m_bIncludeChildBounds;
}
// PC 0x00444840  Xbox 0x00299530
void Gob::ProcessGob()
{
    if (m_bProcessFlag)
    {
        m_pSingleTexture = 0;
        m_bSingleTexture = TreeHasSingleTexture(m_pRootPart);
        if (!m_bSingleTexture)
        {
            m_pSingleTexture = 0;
        }
        if (!m_pSingleTexture)
        {
            m_bSingleTexture = 0;
        }
        m_bHasGeometry = TreeHasGeometry(m_pRootPart);
        TreeSetGeometryFlags(m_pRootPart);
        m_bHasTransparency = TreeHasTransparency(m_pRootPart);
        GetMinimumSphere();
        m_bProcessFlag = 0;
    }
}
// PC 0x004448C0  Xbox 0x002995B0
void CreateReferenceObjects(Gob* pGob, Part* pPart)
{
    PartReference* pReference = pPart->AsPartReference();
    if (pReference)
    {
        pReference->m_pReferenceObject = (Gob*)NewCAurObject(
            pReference->m_pNode->AsMdlNodeReference()->m_sRefModel, 0, 0, 0);
        if (pReference->m_pReferenceObject)
        {
            pReference->m_pReferenceObject->AttachToObject(pGob, pReference->Name(), Gob::ATTACH_NORMAL);
            float fSpeed = 1.0f + ((rand() % 100) - 50.0f) / 1000.0f;
            float fPhase = (rand() % 100) / 100.0f;
            pReference->m_pReferenceObject->PlayAnimation("default", fSpeed, 0, fPhase);
        }
    }
    for (int i = 0; i < pPart->m_lstChildren.m_nCount; i++)
    {
        Part* pChild = pPart->m_lstChildren[i];
        CreateReferenceObjects(pGob, pChild);
    }
}
// Xbox 0x00299690
const char* append(char* sCommand)
{
    char sName[64];
    char sPart[64];
    char sModel[64];

    sName[0] = 0;
    sPart[0] = 0;
    sModel[0] = 0;
    sscanf(sCommand, "%s%s%s", sName, sPart, sModel);
    if (sName[0] == 0 || sPart[0] == 0 || sModel[0] == 0)
    {
        return "Usage:  append <gobname> <partname> <modelname>";
    }
    Gob* pGob = FindGob(sName);
    if (pGob == 0)
    {
        return "Error:  No gob by that name";
    }
    if (pGob->GetPart(sPart) == 0)
    {
        return "Error:  Object does not have part with that name";
    }
    pGob->Append(sModel, sPart);
    return "OK";
}
// Xbox 0x00299760
const char* amputate(char* sCommand)
{
    char sName[64];
    char sPart[64];

    sName[0] = 0;
    sPart[0] = 0;
    sscanf(sCommand, "%s%s", sName, sPart);
    if (sName[0] == 0 || sPart[0] == 0)
    {
        return "Usage:  ampuatate <gobname> <partname>";
    }
    Gob* pGob = FindGob(sName);
    if (pGob == 0)
    {
        return "Error:  No gob by that name";
    }
    if (pGob->GetPart(sPart) == 0)
    {
        return "Error:  Object does not have part with that name";
    }
    pGob->Amputate(sPart);
    return "Amputation Sucessful";
}
// PC 0x00444D90  Xbox 0x00299810
bool Gob::SetObjectScale(float fScale, bool bBaseScale)
{
    if (!m_pModel)
    {
        return false;
    }

    if (bBaseScale == true)
    {
        m_fScale /= m_fBaseScale;
        m_fBaseScale = fScale;
        m_fScale = fScale * m_fScale;
    }
    else
    {
        m_fScale = fScale * m_fBaseScale;
    }

    for (int i = 0; i < m_lstAttachments.m_nCount; ++i)
    {
        m_lstAttachments[i]->SetObjectScale(m_fScale, false);
    }
    return true;
}
// PC 0x00444E20  Xbox 0x00299900
TileData::~TileData()
{
    delete m_pStaticObject;
}
// Xbox 0x00299D40
const char* setgobwind(char* sCommand)
{
    char sName[64];
    float fIntensity;

    if (sscanf(sCommand, "%s %f", sName, &fIntensity) != 2)
    {
        return "Usage: setgobwind gobname intensity";
    }
    Gob* pGob = FindGob(sName);
    if (pGob == 0)
    {
        return "That gob cannot be found!";
    }
    pGob->SetWind(true, fIntensity, 0.0f, 0.0f);
    return "Success.";
}
// PC 0x00445AE0  Xbox 0x0029A980
static void findalllightparts(Part* pPart, List<PartLight*>& lLights)
{
    PartLight* pLight = pPart->AsPartLight();
    if (pLight)
        lLights.Add(pLight);
    for (int i = 0; i < pPart->m_lstChildren.m_nCount; i++)
        findalllightparts(pPart->m_lstChildren[i], lLights);
}
// PC 0x00445E30  Xbox 0x0029A9D0
void Gob::SetLightPriority(int nPriority)
{
    List<PartLight*> lLights;
    findalllightparts(m_pRootPart, lLights);
    for (int i = 0; i < lLights.m_nCount; i++)
    {
        lLights[i]->m_nPriority = nPriority;
    }
}
// PC 0x00445EC0  Xbox 0x0029AA60
static void findallmaterials(Part* pPart, List<Material*>& lMaterials)
{
    if (pPart->m_pMaterial)
        lMaterials.AddUnique(pPart->m_pMaterial);
    for (int i = 0; i < pPart->m_lstChildren.m_nCount; i++)
        findallmaterials(pPart->m_lstChildren[i], lMaterials);
}
// PC 0x00447F00  Xbox 0x0029BDF0
float Gob::GetMaximumLightRadius()
{
    float fMaximumRadius = 0.0f;
    List<PartLight*> lstLights;
    CollectLights(lstLights);
    for (int i = 0; i < lstLights.m_nCount; i++)
    {
        fMaximumRadius = max(lstLights[i]->m_fRadius, fMaximumRadius);
    }
    return fMaximumRadius;
}
// PC 0x00447FA0  Xbox 0x00064C20
template <> inline SafePointer<CAurObject>& SafePointer<CAurObject>::operator=(CAurObject* pObject)
{
    AurModelTrace3();
    SetPointer(pObject);
    return *this;
}
// PC 0x0044B2C0  Xbox 0x0029F2B0
void Gob::RemoveAddInAnimations()
{
    if (m_pAnimationModel != NULL)
        m_pAnimationModel->DelRef(this);
    m_pAnimationModel = NULL;
}
// PC 0x004461D0  Xbox 0x0029AAA0
static void findalltrimeshparts(Part* pPart, List<PartTriMesh*>& lParts)
{
    PartTriMesh* pTriMesh = pPart->AsPartTriMesh();
    if (pTriMesh)
        lParts.Add(pTriMesh);
    for (int i = 0; i < pPart->m_lstChildren.m_nCount; i++)
        findalltrimeshparts(pPart->m_lstChildren[i], lParts);
}
// PC 0x00446520  Xbox 0x0029AAF0
int Gob::ReplaceTexture(char* sOld, char* sNew)
{
    if (sOld[0] == 0)
    {
        List<Material*> lMaterials;
        findallmaterials(m_pRootPart, lMaterials);
        for (int i = 0; i < lMaterials.m_nCount; i++)
            lMaterials[i]->ReplaceTexture(0, sNew);
        return 0;
    }

    List<Material*> lMaterials;
    findallmaterials(m_pRootPart, lMaterials);
    for (int i = 0; i < lMaterials.m_nCount; i++)
    {
        lMaterials[i]->ReplaceTexture(sOld, sNew);
        if (m_pEnvironmentMap)
            ((CAurTexture*)lMaterials[i]->m_pTexture0.m_pObject)->SetIsEnvironmentMapped(true);
    }
#ifdef _XBOX
    SetProcessFlag();
#else
    m_bProcessFlag = 1;
#endif
    return 1;
}
// PC 0x00446660  Xbox 0x0029AC20
int Gob::RestoreTexture()
{
    List<PartTriMesh*> lParts;
    findalltrimeshparts(m_pRootPart, lParts);
    for (int i = 0; i < lParts.m_nCount; i++)
    {
        PartTriMesh* pPart = lParts[i];
        MdlNodeTriMesh* pNode = pPart->m_pNode->AsMdlNodeTriMesh();
        pPart->m_pMaterial->ReplaceTexture(0, pNode->m_sTexture0);
    }
    return 1;
}
int Gob::ReplaceTexture(char* sOld, char* sNew, int nCount,  // PC 0x00446720  Xbox 0x0029ACC0
                        unsigned short* pTXI, int nFlags)
{
    if (spewpltfp)
    {
        fprintf(spewpltfp, "%s\n", sNew);
        for (int i = 0; i < nCount; i++)
            fprintf(spewpltfp, "%i ", pTXI[i]);
        fprintf(spewpltfp, "\n");
    }

    int anTXI[TEXTURE_SCHEME_BUFFER_SIZE];
    for (int i = 0; i < nCount; i++)
        anTXI[i] = pTXI[i];

    char* sOldTexture = 0;
    if (sOld && sOld[0])
        sOldTexture = sOld;

    List<PartTriMesh*> lParts;
    findalltrimeshparts(m_pRootPart, lParts);
    for (int i = 0; i < lParts.m_nCount; i++)
    {
        PartTriMesh* pPart = lParts[i];
        pPart->m_pMaterial->ReplaceTexture(sOldTexture, sNew,
            pPart->m_pNode->TranslateNodeName(), anTXI, nCount);
        if (m_pEnvironmentMap)
            ((CAurTexture*)lParts[i]->m_pMaterial->m_pTexture0.m_pObject)->SetIsEnvironmentMapped(true);
    }
#ifdef _XBOX
    SetProcessFlag();
#else
    m_bProcessFlag = 1;
#endif
    return 1;
}
// PC 0x00446910  Xbox 0x0029AE60
int Gob::ReplaceTextureSubtree(char* sPart, char* sTexture, int nNumTXI,
                               unsigned short* pTXI)
{
    if (spewpltfp)
    {
        fprintf(spewpltfp, "%s\n", sTexture);
        for (int i = 0; i < nNumTXI; i++)
            fprintf(spewpltfp, "%i ", pTXI[i]);
        fprintf(spewpltfp, "\n");
    }

    int anTXI[TEXTURE_SCHEME_BUFFER_SIZE];
    for (int i = 0; i < nNumTXI; i++)
        anTXI[i] = pTXI[i];

    List<Material*> lMaterials;
    Part* pPart = FindPart(m_pRootPart, sPart);
    if (!pPart)
        return 0;

    findallmaterials(pPart, lMaterials);
    for (int i = 0; i < lMaterials.m_nCount; i++)
    {
        lMaterials[i]->ReplaceTexture(0, sTexture, sPart, anTXI, nNumTXI);
        if (m_pEnvironmentMap)
            ((CAurTexture*)lMaterials[i]->m_pTexture0.m_pObject)->SetIsEnvironmentMapped(true);
    }
#ifdef _XBOX
    SetProcessFlag();
#else
    m_bProcessFlag = 1;
#endif
    return 1;
}
// PC 0x00446AB0  Xbox 0x0029AFE0
void GetAllNamedTextures(Part* pPart, List<CAurTexture*>& lTextures, char* sName)
{
    PartTriMesh* pTriMesh = pPart->AsPartTriMesh();
    if (pTriMesh)
    {
        CAurTexture* pTexture =
            ((CAurTexture*)pTriMesh->m_pMaterial->m_pTexture0.m_pObject)->GetReferencedTexture();
        if (pTexture->AsCAurTextureBasic())
        {
            if (stricmp(pTexture->GetName(), sName) == 0)
                lTextures.AddUnique(pTexture);
        }
    }
    for (int i = 0; i < pPart->m_lstChildren.m_nCount; i++)
        GetAllNamedTextures(pPart->m_lstChildren[i], lTextures, sName);
}
// PC 0x00446E10  Xbox 0x0029B060
int Gob::SetTextureAttributes(char* sTexture, char* sAttributes)
{
    List<CAurTexture*> lTextures;
    GetAllNamedTextures(m_pRootPart, lTextures, sTexture);
    if (lTextures.m_nCount > 0)
    {
        for (int i = 0; i < lTextures.m_nCount; i++)
        {
            AurTextureSetAttributes(lTextures[i], sAttributes);
        }
        return 1;
    }
    return 0;
}
// PC 0x00446EE0  Xbox 0x0029B120
void Gob::ReattachReference(CAurObject* pObject, char* sPart)
{
    List<Part*> lstParts;
    lstParts.Add(m_pRootPart);
    while (lstParts.m_nCount)
    {
        Part* pPart = lstParts[0];
        PartReference* pReference = pPart->AsPartReference();
        if (pReference &&
            (pReference->m_pNode->m_nNodeType & (MDL_NODE_HEADER | MDL_NODE_REFERENCE)) ==
                (MDL_NODE_HEADER | MDL_NODE_REFERENCE) &&
            ((MdlNodeReference*)pReference->m_pNode)->m_bReattachable &&
            pReference->m_pReferenceObject)
        {
            if (pObject)
                pReference->m_pReferenceObject->AttachToObject(pObject, sPart, ATTACH_NORMAL);
            if (!pObject)
                pReference->m_pReferenceObject->AttachToObject(this, pReference->Name(), ATTACH_NORMAL);
            m_qOrientation = RotationArc(m_qOrientation.zdir(),
                pReference->m_pReferenceObject->GetPosition() - m_vPosition) * m_qOrientation;
        }
        lstParts.DelIndex(0);
        for (int i = 0; i < pPart->m_lstChildren.m_nCount; i++)
        {
            lstParts.Add(pPart->m_lstChildren[i]);
        }
    }
}
// PC 0x00447180  Xbox 0x0029B360
void Gob::CollectLights(List<PartLight*>& lstLights)
{
    List<Part*> lstParts;
    lstLights.m_nCount = 0;
    lstParts.Add(m_pRootPart);
    for (int i = 0; i < lstParts.m_nCount; i++)
    {
        Part* pPart = lstParts[i];
        PartLight* pLight = pPart->AsPartLight();
        if (pLight)
        {
            lstLights.Add(pLight);
        }
        for (int j = 0; j < pPart->m_lstChildren.m_nCount; j++)
        {
            lstParts.Add(pPart->m_lstChildren[j]);
        }
    }
}
// PC 0x00447310  Xbox 0x0029B420
void Gob::AddAttachment(Gob* pAttachment)
{
    if (!m_lstAttachments.Contains(pAttachment))
    {
        m_lstAttachments.Add(pAttachment);
    }
}
// PC 0x00447370  Xbox 0x0029B450
void Gob::RemoveAttachment(Gob* pAttachment)
{
    if (m_lstAttachments.Contains(pAttachment))
    {
        m_lstAttachments.Remove(pAttachment);
    }
}
// PC 0x004473B0  Xbox 0x0029B480
TileData::TileData()
    : m_lstParts(0),
      m_lstAdjacentTiles(0),
      m_lstReferences(0),
      m_lstAdjacentReferences(0),
      m_lstLocalReferences(0)
{
    m_pStaticObject = 0;
    m_pMainLight = 0;
    m_pSourceLight = 0;
    m_pFlamePart1 = 0;
    m_pFlamePart2 = 0;
    m_pFlame1 = 0;
    m_pFlame2 = 0;
    m_nFadeState = FADE_NOT_CHECKED;
    m_bSuppliesMaterial = 0;
    m_fHeightFadeAlpha = 1.0f;
    m_fBaseHeightFadeAlpha = 1.0f;
    m_fFadeAlpha = 1.0f;
    m_nFadeControl = 0;
    m_bFade = 0;
    m_vMinimum = Vector(AUR_TILE_UNSET_BOUND, AUR_TILE_UNSET_BOUND,
                        AUR_TILE_UNSET_BOUND);
    m_vMaximum = Vector(AUR_TILE_UNSET_BOUND, AUR_TILE_UNSET_BOUND,
                        AUR_TILE_UNSET_BOUND);
}
// PC 0x004474D0  Xbox 0x0029B5D0
void Gob::EnumParts(Part* pPart, int (*pFilter)(CAurPart*), List<CAurPart*>* pParts)
{
    if (pPart)
    {
        if (pFilter == 0)
            pParts->Add(pPart);
        else if (pFilter(pPart))
            pParts->Add(pPart);

        for (int i = 0; i < pPart->m_lstChildren.m_nCount; i++)
            EnumParts(pPart->m_lstChildren[i], pFilter, pParts);
    }
}
// PC 0x004477B0  Xbox 0x0029B630
void Gob::EnumParts(int (*pFilter)(CAurPart*), List<CAurPart*>* pParts)
{
    EnumParts(m_pRootPart, pFilter, pParts);
}

inline CAurObject::CAurObject()
{
}
// PC 0x00449440  Xbox 0x0029CE20
Gob::Gob(char* sName)
    : m_lstRoomData(0), m_lOutOfOrderAnimations(0),
      m_lstAppendModels(0), m_lstAppendParts(0), m_lCallBacks(0),
      m_lstLights(0), m_lstAttachments(0)
{
    m_bFirstAnimate = 1;
    m_bAnimateUV = 0;
    ++g_nSpawnedGobs;
    m_bRenderPersonalSpace = false;
#ifndef _XBOX
    m_bVisible = true;
    m_fVisibilityCheckTime = 0.0f;
#endif
    ++g_nCountOfGobs;
    m_sName[0] = 0;
    if (sName)
    {
        char sBuffer[64];
        sprintf(sBuffer, "%s", sName);
        int i = 0;
        while (FindScene(sBuffer) || FindGob(sBuffer))
            sprintf(sBuffer, "%s%d", sName, ++i);
        strcpy(m_sName, sBuffer);
    }

    m_fCameraDistance = 0.0f;
    m_pRootPart = 0;
    m_pRootPartX = 0;
    m_pRootPartZ = 0;
    m_pScene = 0;
    m_pBehavior = 0;
    m_pEnvironmentMap = 0;
    m_bHasDynamicLights = 0;
    m_bCanDownSample = 1;
    m_nLastAnimationFrame = -1;
    m_pModel = 0;
    m_pModelX = 0;
    m_pModelZ = 0;
    m_pAnimationModel = 0;
    m_bColorShifting = 0;
    m_bIllumination = 0;
    m_vColor = Vector(1.0f, 1.0f, 1.0f);
    m_fAlpha = 1.0f;
    m_fOcclusionAlpha = 1.0f;
    m_bCastShadows = 1;
    m_bRenderGobBBox = 0;
    m_fZOrder = 0.0f;
    m_bMotionBlurred = false;
    m_fMaxBlurLength = 1.5f;
    m_fBumpedOutShellHeight = 0.0f;
    m_fPersonalSpaceRadius = 0.0f;
    m_fPersonalSpaceRadius2 = 0.0f;
    m_bSingleTexture = 0;
    m_bUnused = 0;
    m_bHasGeometry = 0;
    m_bDistortion = 0;
    m_pSingleTexture = 0;
    m_nUnused1 = 0;
    m_bRenderBumpedOut = 0;
    m_bHasTransparency = 0;
    m_bIgnoreHitCheck = false;
    m_bWind = false;
    m_fWindX = 0.0f;
    m_fWindY = 0.0f;
    m_fWindZ = 0.0f;
    m_vBoundingBoxMinimum = Vector(0.0f, 0.0f, 0.0f);
    m_vBoundingBoxMaximum = Vector(1.0f, 1.0f, 1.0f);
    m_fMinimumBoundingSphereRadius = 1.0f;
    m_vMinimumBoundingSphere = Vector(0.0f, 0.0f, 0.0f);
    m_vRenderGobBBox = Vector(0.0f, 0.0f, 1.0f);
    m_pExternalData = 0;
    m_pTileData = 0;
    m_nExternalDataType = 0;
    m_bProcessFlag = 0;
    m_bForceAnimated = false;
    m_bInViewVolume = true;
    m_bAnimating = false;
    m_pLookAtAnimation = 0;
    m_nImposters = 0;
    m_fScale = 1.0f;
    m_fBaseScale = 1.0f;
    m_nImposterType = 0;
    m_bAlwaysRendering = false;
    m_bInCutscene = false;
    m_bHierarchyAnimated = false;
    m_pLightContainer = new CAurObjectLightContainer;
    m_nUnused2 = 1;
    gobs.Insert(this, FindGobIdx(m_sName));
}
// PC 0x0044B2F0  Xbox 0x0029F2D0
Gob::~Gob()
{
    --g_nCountOfGobs;
    DoEvent("Dying", 0);
    PlayAnimation(0, 1.0f, 0, 0.0f);
    SetScene(0);
    SetBehavior(0);

    if (m_pRootPart)
    {
        delete m_pRootPart;
        m_pRootPart = 0;
    }

    for (int i = 0; i < m_lstAppendModels.m_nCount; i++)
    {
        m_lstAppendModels[i]->DelRef(this);
    }
    m_lstAppendModels.m_nCount = 0;
    m_lstAppendParts.m_nCount = 0;

    if (m_pModel)
        m_pModel->DelRef(this);
    if (m_pModelX)
        m_pModelX->DelRef(this);
    if (m_pModelZ)
        m_pModelZ->DelRef(this);

    for (int j = 0; j < m_lCallBacks.m_nCount; j++)
    {
        delete m_lCallBacks[j];
        m_lCallBacks[j] = 0;
    }
    m_lCallBacks.m_nCount = 0;

    CAurTexture* pEnvironmentMap = m_pEnvironmentMap;
    m_pEnvironmentMap = 0;
    AurTextureRelease(pEnvironmentMap);
    gobs.Remove(this);
    Scene::RemoveFromSceneManager(this);

    if (m_pLookAtAnimation)
    {
        delete m_pLookAtAnimation;
        m_pLookAtAnimation = 0;
    }
    if (m_pLightContainer)
    {
        delete m_pLightContainer;
        m_pLightContainer = 0;
    }
#ifdef _XBOX
    strcpy(sFuncPrefix, m_sName);
    strcat(sFuncPrefix, ".");
    DeleteFuncWithPrefix(sFuncPrefix);
#endif
}
// PC 0x00449B10  Xbox 0x0029D4D0
int Gob::Append(char* sModel, char* sPart)
{
    Part* pParent = FindPart(m_pRootPart, sPart);
    if (pParent == 0)
        return 0;
    Model* pModel = FindModel(sModel);
    if (pModel == 0)
        return 0;
    ++pModel->m_nReferenceCount;
    Part* pPart = CreateInstanceTree(pModel->m_pRootNode, this);
    pPart->m_vPosition = Vector(0.0f, 0.0f, 0.0f);
    pPart->m_qOrientation = Quaternion(1.0f, 0.0f, 0.0f, 0.0f);
    m_lstAppendModels.Add(pModel);
    m_lstAppendParts.Add(pPart);
    pParent->m_lstChildren.Add(pPart);
    pPart->m_pParent = pParent;
    PartInitMaterial(this, pPart, "");
#ifdef _XBOX
    SetProcessFlag();
#else
    m_bProcessFlag = 1;
#endif
    CreateReferenceObjects(this, pPart);
    DoEvent("_EmitterTarget", 0);
    return 1;
}
// Xbox 0x0029DB00
const char* spinmodels(char* sCommand)
{
    for (int i = 0; i < 1000; i++)
    {
        CAurObject* pRat = NewCAurObject("c_wererat", "", 0, 0);
        CAurObject* pSun = NewCAurObject("gidy_sun", "", 0, 0);
        CAurObject* pRat2 = NewCAurObject("c_wererat", "", 0, 0);
        CAurObject* pRat3 = NewCAurObject("c_wererat", "", 0, 0);
        CAurObject* pRat4 = NewCAurObject("c_wererat", "", 0, 0);
        CAurObject* pRat5 = NewCAurObject("c_wererat", "", 0, 0);
        CAurObject* pRat6 = NewCAurObject("c_wererat", "", 0, 0);
        pSun->AttachToObject(pRat, "wrat_handl", 0);
        pSun->AttachToObject(pRat, "wrat_handr", 0);
        pSun->AttachToObject(pRat, "wrat_t4", 0);
        delete pRat;
        delete pSun;
        delete pRat2;
        delete pRat3;
        delete pRat4;
        delete pRat5;
        delete pRat6;
    }
    return "SUCK-CESS";
}
// PC 0x00449CC0  Xbox 0x0029D600
CAurObject* NewCAurObject(char* sModel, char* sName, FILE* pModelFile, FILE* pVertexFile)
{
    bool bKeepHistory = g_bKeepHistory;
    g_bKeepHistory = false;
    Model* pModel;
    Model* pModelX = 0;
    Model* pModelZ = 0;
    if (!pModelFile)
    {
        pModel = FindModel(sModel);
        if (cullwithLOD)
        {
            char sVariant[64];
            strcpy(sVariant, sModel);
            int nLength = strlen(sModel);
            sVariant[nLength] = '_';
            sVariant[nLength + 1] = 'x';
            sVariant[nLength + 2] = 0;
            pModelX = FindModel(sVariant);
            strcpy(sVariant, sModel);
            nLength = strlen(sModel);
            sVariant[nLength] = '_';
            sVariant[nLength + 1] = 'z';
            sVariant[nLength + 2] = 0;
            pModelZ = FindModel(sVariant);
        }
    }
    else
        pModel = LoadModel(pModelFile, pVertexFile);
    if (!pModel)
    {
        g_bKeepHistory = bKeepHistory;
        return 0;
    }
    if (!sName || !sName[0])
        sName = pModel->m_sName;
    Gob* pGob = new Gob(sName);
    pGob->m_pModel = pModel;
    ++pModel->m_nReferenceCount;
    pGob->m_pModelX = pModelX;
    if (pModelX)
        ++pModelX->m_nReferenceCount;
    pGob->m_pModelZ = pModelZ;
    if (pModelZ)
        ++pModelZ->m_nReferenceCount;
    pGob->m_pRootPart = CreateInstanceTree(pModel->m_pRootNode, pGob);
    pGob->m_pRootPart->m_pNode->ProcessSkinSeams();
    pGob->m_pRootPart->m_pAurObject = pGob;
    if (pModelX)
    {
        pGob->m_pRootPartX = CreateInstanceTree(pModelX->m_pRootNode, pGob);
        pGob->m_pRootPartX->m_pAurObject = pGob;
    }
    if (pModelZ)
    {
        pGob->m_pRootPartZ = CreateInstanceTree(pModelZ->m_pRootNode, pGob);
        pGob->m_pRootPartZ->m_pAurObject = pGob;
    }
    CreateReferenceObjects(pGob, pGob->m_pRootPart);
    if (pGob->m_pRootPartX)
        CreateReferenceObjects(pGob, pGob->m_pRootPartX);
    if (pGob->m_pRootPartZ)
        CreateReferenceObjects(pGob, pGob->m_pRootPartZ);
    pGob->DoEvent("_EmitterTarget", 0);
    PartInitMaterial(pGob, pGob->m_pRootPart, "");
    if (pGob->m_pRootPartX)
        PartInitMaterial(pGob, pGob->m_pRootPartX, "");
    if (pGob->m_pRootPartZ)
        PartInitMaterial(pGob, pGob->m_pRootPartZ, "");
    g_bKeepHistory = bKeepHistory;
    pGob->GetMinimumBoundingBox();
#ifdef _XBOX
    pGob->SetProcessFlag();
#else
    pGob->m_bProcessFlag = 1;
#endif
    pGob->Animate(0.0f);
    return pGob;
}
// PC 0x0044A040  Xbox 0x0029DF70
MdlNode::~MdlNode()
{
    if (m_nNodeType != 0)
        DestructorDispatch();
    m_nNodeType = 0;
    for (int i = m_lChildren.m_nCount; i != 0; )
    {
        --i;
        if (m_lChildren[i]->AsMdlNodeLight())
            delete m_lChildren[i]->AsMdlNodeLight();
        else if (m_lChildren[i]->AsMdlNodeCamera())
            delete m_lChildren[i]->AsMdlNodeCamera();
        else if (m_lChildren[i]->AsMdlNodeDanglyMesh())
            delete m_lChildren[i]->AsMdlNodeDanglyMesh();
        else if (m_lChildren[i]->AsMdlNodeAnimMesh())
            delete m_lChildren[i]->AsMdlNodeAnimMesh();
        else if (m_lChildren[i]->AsMdlNodeEmitter())
            delete m_lChildren[i]->AsMdlNodeEmitter();
        else if (m_lChildren[i]->AsMdlNodeReference())
            delete m_lChildren[i]->AsMdlNodeReference();
        else if (m_lChildren[i]->AsMdlNodeAABB())
            delete m_lChildren[i]->AsMdlNodeAABB();
        else if (m_lChildren[i]->AsMdlNodeSkin())
            delete m_lChildren[i]->AsMdlNodeSkin();
        else if (m_lChildren[i]->AsMdlNodeTrigger())
            delete m_lChildren[i]->AsMdlNodeTrigger();
        else if (m_lChildren[i]->AsMdlNodeLightsaber())
            delete m_lChildren[i]->AsMdlNodeLightsaber();
        else if (m_lChildren[i]->AsMdlNodeTriMesh())
            delete m_lChildren[i]->AsMdlNodeTriMesh();
        else
            delete m_lChildren[i];
    }
    countmodelnodes--;
}
// PC 0x0044A280  Xbox 0x0029E170
MdlNodeCamera::MdlNodeCamera(char* sName)
    : MdlNode(sName)
{
    m_nNodeType = MDL_NODE_HEADER | MDL_NODE_CAMERA;
}
// PC 0x0044A2A0  Xbox 0x0029E190
MdlNode* MdlNodeCamera::InternalConJoin()
{
    return new MdlNodeCamera(TranslateNodeName());
}
// PC 0x0044A300  Xbox 0x0029E220
MdlNodeEmitter::MdlNodeEmitter(char* sName)
    : MdlNode(sName)
{
    m_nNodeType = MDL_NODE_HEADER | MDL_NODE_EMITTER;
    m_nLoop = 0;
    memset(m_sChunkName, 0, sizeof(m_sChunkName));
    memset(m_sDepthTextureName, 0, 32);
    m_nSpawnType = 0;
    m_fDeadSpace = 0.0f;
    m_nTwoSidedTex = 0;
    m_fBlastRadius = 0.0f;
    m_fBlastLength = 0.0f;
    m_bFrameBlending = 0;
    m_nNumBranches = 0;
    EmitterFlags flags = { 0 };
    m_EmitterFlags = flags;
    m_nRenderOrder = 0;
    m_nControlPointSmoothing = 0;
    m_nXGrid = 0;
    m_nYGrid = 0;
    m_sUpdate[0] = 0;
    m_sRender[0] = 0;
    m_sBlend[0] = 0;
    m_sTexture[0] = 0;
    m_sChunkName[0] = 0;
}
// PC 0x0044A390  Xbox 0x0029E2D0
MdlNode* MdlNodeEmitter::InternalConJoin()
{
    return new MdlNodeEmitter(TranslateNodeName());
}
// PC 0x0044A3F0  Xbox 0x0029E360
MdlNodeLight::MdlNodeLight(char* sName)
    : MdlNode(sName),
      m_lstTextures(0),
      m_lstFlareSizes(0),
      m_lstFlarePositions(0),
      m_lstFlareColorShifts(0),
      m_lstTextureNames(0)
{
    m_nNodeType = MDL_NODE_HEADER | MDL_NODE_LIGHT;
    m_bAmbientOnly = 0;
    m_nDynamicType = 1;
    m_bAffectDynamic = 1;
    m_bShadow = 1;
    m_bGenerateFlare = 0;
    m_fFlareRadius = 0.0f;
    m_bFadingLight = 1;
    m_nPriority = DEFAULT_LIGHT_PRIORITY;
}
// PC 0x0044A4A0  Xbox 0x0029E440
MdlNode* MdlNodeLight::InternalConJoin()
{
    return new MdlNodeLight(TranslateNodeName());
}
// PC 0x0044A500  Xbox 0x0029E4D0
void MdlNodeLight::BinaryInternalDestructor()
{
    for (int i = m_lstTextures.m_nCount; i != 0; )
    {
        i--;
        CAurTexture* pTexture = m_lstTextures[i];
        m_lstTextures[i] = 0;
        AurTextureRelease(pTexture);
        m_lstTextures.m_nCount--;
    }
    m_lstTextures.SetSize(0);
}
// PC 0x0044A5C0  Xbox 0x0029E520
MdlNodeReference::MdlNodeReference(char* sName)
    : MdlNode(sName)
{
    m_nNodeType = MDL_NODE_HEADER | MDL_NODE_REFERENCE;
}
// PC 0x0044A5E0  Xbox 0x0029E540
MdlNode* MdlNodeReference::InternalConJoin()
{
    return new MdlNodeReference(TranslateNodeName());
}
// PC 0x0044A640  Xbox 0x0029E5D0
MdlNodeTriMesh::MdlNodeTriMesh(char* sName)
    : MdlNode(sName), m_lFaces(0), m_lstIndexArrays(0),
      m_lstTriangleIndices(0), m_lstIndexCounts(0), m_lstIndexOffsets(0),
      m_lstIndexPools(0)
{
    m_pfnGenVertices = MdlNodeTriMeshGenVertices;
    m_pfnRemoveTemporaryArrays = MdlNodeTriMeshRemoveTemporaryArrays;
    m_nVertexStride = -1;
    m_nVertexArrayFlags = 0;
    m_nPositionOffset = -1;
    m_nNormalOffset = -1;
    m_nColorOffset = -1;
    m_nTexCoord0Offset = -1;
    m_nTexCoord1Offset = -1;
    m_nTexCoord2Offset = -1;
    m_nTexCoord3Offset = -1;
    m_TangentBases = -1;
    m_nUnusedVertexOffset1 = -1;
    m_nUnusedVertexOffset2 = -1;
    m_nUnusedVertexOffset3 = -1;
    m_fSurfaceArea = 0.0f;
#ifdef _XBOX
    m_fXboxUnmapped190 = 0.0f;
#else
    m_nUnusedWord = 0;
#endif
    m_nTransparencyHint = 0;
    m_bBeaming = false;
    m_bShadow = true;
    m_bRender = true;
    m_bLightMapped = false;
    m_bBackground = false;
    m_bRotateTexture = false;
    m_vDiffuse = defaultdiffuse;
    m_vAmbient = defaultambient;
    m_sTexture0[0] = 0;
    m_sTexture1[0] = 0;
    m_nVertexCount = 0;
    m_nTextureCount = 0;
    m_nNodeType = MDL_NODE_HEADER | MDL_NODE_TRIMESH;
    m_fSphereRadius = 0.0f;
    m_nAuxIndexPool = -1;
    m_nAuxIndexCount = 0;
    m_pPostProcess = 0;
    m_bAnimateUV = 0;
#ifdef _XBOX
    m_nVertexDataOffset = 0;
}
#else
    m_nVertexDataOffset = 0;
    m_pVertexPositions = 0;
}
#endif
// PC 0x0044A850  Xbox 0x0029E810
MdlNode* MdlNodeTriMesh::InternalConJoin()
{
    return new MdlNodeTriMesh(TranslateNodeName());
}
// PC 0x0044A8B0  Xbox 0x0029E8A0
MdlNodeAABB::MdlNodeAABB(char* sName)
    : MdlNodeTriMesh(sName)
{
    m_pAABBData = 0;
    m_bRender = 0;
    m_bShadow = 0;
    m_nNodeType = MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_AABB;
}
// PC 0x0044A8E0  Xbox 0x0029E8D0
void MaxTreeDestructor(void* pTree)
{
    ((MaxTree*)pTree)->InternalDestructor();
}
// Xbox 0x0029DA10
void MaxTree::InternalDestructor()
{
    MdlNode* pRootNode = m_pRootNode;
    if (pRootNode != NULL)
        pRootNode->DestructorDispatch();
}
// PC 0x0044A900  Xbox 0x0029E8E0
MaxTree::MaxTree(char* sName)
{
    strcpy(m_sName, sName);
    m_pRootNode = 0;
    m_nType = MAXTREE_BASE_ID;
    m_pModelResource = 0;
    m_nNodeCount = 0;
    m_lFreeTokens.SetSize(0);
    m_lFreeNodes.SetSize(0);
    m_pfnDestructor = MaxTreeDestructor;
    m_pfnParseField = MaxTreeParseField;
}
// PC 0x0044A9C0  Xbox 0x0029E980
void MaxTree::FreeTheFreeList()
{
    for (int i = 0; i < m_lFreeNodes.m_nCount; ++i)
    {
        m_lFreeNodes[i]->AsMdlNodeLight()->BinaryInternalDestructor();
    }
    m_lFreeNodes.SetSize(0);

    for (int j = 0; j < m_lFreeTokens.m_nCount; ++j)
    {
        GLRender::DeletePool(m_lFreeTokens[j]);
        m_lFreeTokens[j] = RequestPoolManager::INVALID_POOL;
    }
    m_lFreeTokens.SetSize(0);
}
// PC 0x0044AA60  Xbox 0x0029E9F0
void ModelDestructor(void* pModel)
{
    ((Model*)pModel)->InternalDestructor();
}
// PC 0x0044AA70  Xbox 0x0029EA00
Model::Model(char* sName) : MaxTree(sName)
{
    m_nType = MAXTREE_MODEL_ID;
    m_pParentModel = 0;
    Classification classification = { 0 };
    m_Classification = classification;
    m_nModelOptions = 0;
    m_bModelOptionEnabled = 1;
    m_fRadius = 7.0f;
    m_nReferenceCount = 0;
    m_fAnimationScale = 1.0f;
    m_vBoundingMin = Vector(-5.0f, -5.0f, -1.0f);
    m_vBoundingMax = Vector(5.0f, 5.0f, 10.0f);
    InsertModel(this);
    m_sSuperModel[0] = 0;
    m_pfnDestructor = ModelDestructor;
    m_pfnParseField = ModelParseField;
    m_nVertexPool = RequestPoolManager::INVALID_POOL;
    m_nVertexPool = GLRender::RequestPool(Aurora::POOL_VERTEX, 1,
        RequestPoolManager::POOL_STATIC, 0, &m_nVertexPool);
    m_nVertexDataSize = 0;
    m_pVertexResource = 0;
    m_lNames.m_nCount = 0;
}
// PC 0x0044ABC0  Xbox 0x0029EB80
MdlNodeAnimMesh::MdlNodeAnimMesh(char* sName)
    : MdlNodeTriMesh(sName),
      m_lAnimationVectors1(0),
      m_lAnimationVectors2(0),
      m_lAnimationVectors3(0)
{
    m_fSamplePeriod = 0.0f;
    m_sTexture0[0] = 0;
    m_sTexture1[0] = 0;
    m_nAnimationDataCount1 = 0;
    m_nAnimationDataCount2 = 0;
    m_pAnimationData1 = 0;
    m_pAnimationData2 = 0;
    m_pfnGenVertices = MdlNodeAnimMeshGenVertices;
    m_pfnRemoveTemporaryArrays = MdlNodeAnimMeshRemoveTemporaryArrays;
    m_nNodeType = MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_ANIMMESH;
}
// PC 0x0044AC70  Xbox 0x0029EC40
MdlNodeSkin::MdlNodeSkin(char* sName)
    : MdlNodeTriMesh(sName),
      m_lstSkinWeights(0),
      m_lstQBoneRefInv(0),
      m_lstTBoneRefInv(0),
      m_lstBoneConstantIndices(0)
{
    m_nNodeType = MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_SKIN;
    m_pfnGenVertices = MdlNodeSkinGenVertices;
    m_pfnRemoveTemporaryArrays = MdlNodeSkinRemoveTemporaryArrays;
    m_pfBoneMap = 0;
    m_nSkinWeightOffset = -1;
    m_nBoneIndexOffset = -1;
}
// PC 0x0044AD20  Xbox 0x0029ED00
MdlNode* MdlNodeSkin::InternalConJoin()
{
    return new MdlNodeSkin(TranslateNodeName());
}
// PC 0x0044AD80  Xbox 0x0029ED90
MdlNodeLightsaber::MdlNodeLightsaber(char* sName)
    : MdlNodeTriMesh(sName)
{
    m_nNodeType = MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_LIGHTSABER;
    GLRender::RequestPool(Aurora::POOL_VERTEX, 256, 2, 4114, &m_nBladePool);
    GLRender::RequestPool(Aurora::POOL_VERTEX, 1600, 2, 4112, &m_nGlowPool);
}
// PC 0x0044AE00  Xbox 0x0029EE10
MdlNodeDanglyMesh::MdlNodeDanglyMesh(char* sName)
    : MdlNodeTriMesh(sName),
      m_lConstraints(0)
{
    m_nNodeType = MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_DANGLYMESH;
    m_pfnRemoveTemporaryArrays = MdlNodeDanglyMeshRemoveTemporaryArrays;
    m_pfnGenVertices = MdlNodeDanglyMeshGenVertices;
}
// PC 0x0044B670  Xbox 0x0029F520
int Gob::Amputate(char* sPart)
{
    Part* pPart = FindPart(m_pRootPart, sPart);
    if (pPart == 0)
        return 0;

    List<Part*> lParts;
    lParts.Add(pPart);
    while (lParts.m_nCount)
    {
        Part* pCurrent = lParts[lParts.m_nCount - 1];
        lParts.m_nCount--;
        for (int i = 0; i < pCurrent->m_lstChildren.m_nCount; ++i)
            lParts.Add(pCurrent->m_lstChildren[i]);

        if (m_lstAppendParts.Contains(pCurrent))
        {
            int nIndex = m_lstAppendParts.IndexOf(pCurrent);
            m_lstAppendModels[nIndex]->DelRef(this);
            m_lstAppendParts.DelIndex(nIndex);
            m_lstAppendModels.DelIndex(nIndex);
        }
    }

    pPart->m_pParent->m_lstChildren.Remove(pPart);
    pPart->m_pParent = 0;
    delete pPart;
#ifdef _XBOX
    SetProcessFlag();
#else
    m_bProcessFlag = 1;
#endif
    return 1;
}

int Model::DelRef(Gob* pGob)
{
    if (removedelref)
        return m_nReferenceCount;

    int nReferenceCount = --m_nReferenceCount;
    if (m_nReferenceCount == 0)
        operator delete(this);
    return nReferenceCount;
}
// PC 0x0044B900  Xbox 0x0029F770
bool AurControlPart(char* sCommand)
{
    char sGob[64];
    char sPart[64];
    int nChars;
    Gob* pGob;
    Part* pPart;

    sPart[0] = 0;
    sGob[0] = 0;
    if (sscanf(sCommand, "%s%s%n", sGob, sPart, &nChars) >= 2 &&
        sGob[0] && sPart[0] &&
        (pGob = FindGob(sGob)) != 0 &&
        (pPart = pGob->GetPart(sPart)) != 0)
    {
        MdlNode* pNode = pPart->m_pNode->ConJoinDispatch();
        pNode->ParseFieldDispatch(skipspace(sCommand + nChars));
        pNode->ControlDispatch(pPart, 0.0f, 0.0f, 1.0f);
        delete pNode;
        return true;
    }
    return false;
}
// PC 0x0044B140  Xbox 0x0029F190
MdlNode* MdlNode::ConJoinDispatch()
{
    switch (m_nNodeType)
    {
    case MDL_NODE_HEADER:
        return InternalConJoin();
    case MDL_NODE_HEADER | MDL_NODE_LIGHT:
        return AsMdlNodeLight()->InternalConJoin();
    case MDL_NODE_HEADER | MDL_NODE_EMITTER:
        return AsMdlNodeEmitter()->InternalConJoin();
    case MDL_NODE_HEADER | MDL_NODE_CAMERA:
        return AsMdlNodeCamera()->InternalConJoin();
    case MDL_NODE_HEADER | MDL_NODE_REFERENCE:
        return AsMdlNodeReference()->InternalConJoin();
    case MDL_NODE_HEADER | MDL_NODE_TRIMESH:
        return AsMdlNodeTriMesh()->InternalConJoin();
    case MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_SKIN:
        return AsMdlNodeSkin()->InternalConJoin();
    case MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_ANIMMESH:
        return AsMdlNodeAnimMesh()->InternalConJoin();
    case MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_DANGLYMESH:
        return AsMdlNodeDanglyMesh()->InternalConJoin();
    case MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_AABB:
        return AsMdlNodeAABB()->InternalConJoin();
    case MDL_NODE_HEADER | MDL_NODE_TRIGGER:
        return AsMdlNodeTrigger()->InternalConJoin();
    case MDL_NODE_HEADER | MDL_NODE_TRIMESH | MDL_NODE_LIGHTSABER:
        return AsMdlNodeLightsaber()->InternalConJoin();
    }
    return 0;
}
// Xbox 0x0029D880
const char* spawn(char* sCommand)
{
    char sModel[128];
    char sName[128];

    sModel[0] = 0;
    sName[0] = 0;
    sscanf(sCommand, "%s %s", sModel, sName);
    CAurObject* pObject = NewCAurObject(sModel, sName, 0, 0);
    strcpy(lastspawnedobject, pObject ? pObject->GetName() : "NULL");
    FuncInterp("attach [lastspawned] scene");
    if (pObject == 0)
    {
        return "failure";
    }
    return "success";
}

Vector defaultdiffuse(0.8f, 0.8f, 0.8f);
Vector defaultambient(0.2f, 0.2f, 0.2f);

static inline void AurModelTrace4()
{
    AurModelTrace3();
}

static inline void AurModelTrace3()
{
    AurModelTrace2();
}

static inline void AurModelTrace2()
{
    AurModelTrace1();
}

static inline void AurModelTrace1()
{
}
