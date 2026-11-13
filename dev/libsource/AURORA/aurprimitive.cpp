#include <windows.h>
#include "aurrendergl.h"
#include <GL/gl.h>
#include "aurprimitive.h"
#include "aurmodel.h"
#include "aurrendergl.h"
#include "aurmodel.h"
#include "aurmaterial.h"
#include "aurlightmanager.h"

static inline void AurprimitiveTrace2();
static inline void AurprimitiveTrace1();
#define AURPRIMITIVE_TRACE() AurprimitiveTrace1()

const unsigned char NodeVertex::NODEVERTEX_ANIM_ID = 1;
const unsigned char NodeVertex::NODEVERTEX_DANGLY_ID = 2;
const unsigned char NodeVertex::NODEVERTEX_SKINNED_ID = 3;

int verticescount;
int enablevar;
int vertexsize;
int usevertexprograms = 1;

VertexPrimitive::VertexPrimitive(Part* pPart)
#ifdef _XBOX
    : m_nArrayPool(RequestPoolManager::INVALID_POOL), m_pPart(pPart), m_nArrayCount(0), m_bStatic(1),
      m_bRebuild(0)
#else
    : m_pPart(pPart), m_nArrayCount(0), m_bStatic(1), m_bRebuild(0)
#endif
{
}
// PC 0x00495060 GOG 0x00495120
NodeVertexAnim* NodeVertex::AsNodeVertexAnim()
{
    return m_nType == NODEVERTEX_ANIM_ID ? (NodeVertexAnim*)this : 0;
}

static inline void SetupColorState(bool bColors)
{
    bool bEnable = bColors;
    if (!bEnable)
    {
        GLRender_DisableClientState(CLIENT_STATE_COLOR);
    }
    else if (LightManager::m_pOnlyGob != 0)
    {
        GLRender_DisableClientState(CLIENT_STATE_COLOR);
    }
    else
    {
        GLRender_EnableClientState(CLIENT_STATE_COLOR);
    }
}
// PC 0x00495070 GOG 0x00495130
NodeVertexDangly* NodeVertex::AsNodeVertexDangly()
{
    return m_nType == NODEVERTEX_DANGLY_ID ? (NodeVertexDangly*)this : NULL;
}

enum { VERTEX_NORMAL };
// PC 0x004953A0 GOG 0x00495460
VertexPrimitiveFlat::~VertexPrimitiveFlat()
{
    if (m_nPool != RequestPoolManager::INVALID_POOL)
    {
        GLRender::DeletePool(m_nPool);
        m_nPool = RequestPoolManager::INVALID_POOL;
    }
}
// PC 0x00495080 GOG 0x00495140
VertexPrimitive::~VertexPrimitive()
{
#ifdef _XBOX
    GLRender::DeletePool(m_nArrayPool);
    m_nArrayPool = RequestPoolManager::INVALID_POOL;
#endif
}
// PC 0x00495090 GOG 0x00495150
NodeVertex::NodeVertex(Vector* pPosition, Vector* pTexCoord0, Vector* pTexCoord1,
                       Vector* pTexCoord2, Vector* pTexCoord3,
                       Vector* pNormal, Vector* pColor)
{
    m_nType = VERTEX_NORMAL;
    m_nFlags = 0;
    m_vColor = Vector(1.0f, 1.0f, 1.0f);

    m_vPosition = *pPosition;
    m_vNormal = *pNormal;

    if (pTexCoord0 != 0)
    {
        m_vTexCoord0 = *pTexCoord0;
        m_nFlags = VERTEX_HAS_TEXCOORD0;
    }
    if (pTexCoord1 != 0)
    {
        m_vTexCoord1 = *pTexCoord1;
        m_nFlags |= VERTEX_HAS_TEXCOORD1;
    }
    if (pTexCoord2 != 0)
    {
        m_vTexCoord2 = *pTexCoord2;
        m_nFlags |= VERTEX_HAS_TEXCOORD2;
    }
    if (pTexCoord3 != 0)
    {
        m_vTexCoord3 = *pTexCoord3;
        m_nFlags |= VERTEX_HAS_TEXCOORD3;
    }
    if (pColor != 0)
    {
        m_vColor = *pColor;
    }

    vertexsize += 8;
}
// PC 0x004951F0 GOG 0x004952B0
NodeVertexDangly::NodeVertexDangly(Vector* pPosition, Vector* pTexCoord0,
                                   Vector* pTexCoord1, Vector* pTexCoord2,
                                   Vector* pTexCoord3, Vector* pNormal,
                                   Vector* pColor, float fValue)
    : NodeVertex(pPosition, pTexCoord0, pTexCoord1, pTexCoord2, pTexCoord3,
                 pNormal, pColor)
{
    m_nType = NODEVERTEX_DANGLY_ID;
    m_fConstraint = fValue;
}
// PC 0x00495230 GOG 0x004952F0
unsigned char NodeVertex::operator==(NodeVertex const& other)
{
    return (equal(other.m_vPosition, m_vPosition) != 0) &&
           (equal(other.m_vTexCoord0, m_vTexCoord0) != 0) &&
           (equal(other.m_vTexCoord1, m_vTexCoord1) != 0) &&
           (equal(other.m_vTexCoord2, m_vTexCoord2) != 0) &&
           (equal(other.m_vTexCoord3, m_vTexCoord3) != 0) &&
           (equal(other.m_vNormal, m_vNormal) != 0) &&
           (equal(other.m_vColor, m_vColor) != 0);
}
// PC 0x00495230 GOG 0x004952F0
unsigned char NodeVertexAnim::operator==(NodeVertexAnim const& other)
{

    return (equal(other.m_vPosition, m_vPosition) != 0) &&
           (equal(other.m_vTexCoord0, m_vTexCoord0) != 0) &&
           (equal(other.m_vTexCoord1, m_vTexCoord1) != 0) &&
           (equal(other.m_vTexCoord2, m_vTexCoord2) != 0) &&
           (equal(other.m_vTexCoord3, m_vTexCoord3) != 0) &&
           (equal(other.m_vNormal, m_vNormal) != 0) &&
           (equal(other.m_vColor, m_vColor) != 0);
}
// PC 0x004952D0 GOG 0x00495390
unsigned char NodeVertexDangly::operator==(NodeVertexDangly const& other)
{
    return (equal(other.m_vPosition, m_vPosition) != 0) &&
           (equal(other.m_vTexCoord0, m_vTexCoord0) != 0) &&
           (equal(other.m_vTexCoord1, m_vTexCoord1) != 0) &&
           (equal(other.m_vTexCoord2, m_vTexCoord2) != 0) &&
           (equal(other.m_vTexCoord3, m_vTexCoord3) != 0) &&
           (equal(other.m_vNormal, m_vNormal) != 0) &&
           (equal(other.m_vColor, m_vColor) != 0) &&
           (other.m_fConstraint == m_fConstraint);
}
// PC 0x00495420 GOG 0x004954E0
VertexPrimitiveSkinned::~VertexPrimitiveSkinned()
{
}
// PC 0x00495450 GOG 0x00495510
VertexPrimitiveDangly::~VertexPrimitiveDangly()
{
    AurprimitiveTrace2();
    delete[] m_pOffsets;
}
// PC 0x004954D0 GOG 0x00495590
VertexPrimitiveAnim::~VertexPrimitiveAnim()
{
    AurprimitiveTrace2();
    int nVertexPool = m_nVertexPool;
    GLRender::DeletePool(nVertexPool);
    m_nVertexPool = RequestPoolManager::INVALID_POOL;
    GLRender::DeletePool(m_nTexCoordPool);
    m_nTexCoordPool = RequestPoolManager::INVALID_POOL;
}
// PC 0x004015A0 GOG 0x004015A0
void VertexPrimitiveAnim::BuildVertexArrays()
{
}
// PC 0x004015A0 GOG 0x004015A0
void VertexPrimitiveDangly::BuildVertexArrays()
{
}
// PC 0x004015A0 GOG 0x004015A0
void VertexPrimitiveSkinned::BuildVertexArrays()
{
}
// PC 0x005EE1C0 GOG 0x005EE1A0
void VertexPrimitiveFlat::UpdateLightVectors(const Vector&)
{
}
// PC 0x006049D0 GOG 0x006049B0
void VertexPrimitiveFlat::UpdateHalfAngleVectors(const Vector&, const Vector&)
{
}
// PC 0x006049D0 GOG 0x006049B0
void VertexPrimitiveFlat::UpdateFogValues(const Vector&, float)
{
}
// PC 0x00495BF0 GOG 0x00495CB0
NodeVertexAnim::NodeVertexAnim(Vector* pPosition, Vector* pTexCoord0,
                               Vector* pTexCoord1, Vector* pTexCoord2,
                               Vector* pTexCoord3, Vector* pNormal,
                               Vector* pColor, List<Vector>& lstPositions,
                               List<Vector>& lstNormals,
                               List<Vector>& lstTexCoords)
    : NodeVertex(pPosition, pTexCoord0, pTexCoord1, pTexCoord2, pTexCoord3,
                 pNormal, pColor)
{
    m_nType = NODEVERTEX_ANIM_ID;
    m_lstPositions = lstPositions;
    m_lstNormals = lstNormals;
    m_lstTexCoords = lstTexCoords;
}
// PC 0x00495CB0 GOG 0x00495D70
VertexPrimitiveFlat::VertexPrimitiveFlat(PartTriMesh* pPart)
    : VertexPrimitive(pPart)
{
    AurprimitiveTrace2();
#ifdef _XBOX
    m_pNode = (MdlNodeTriMesh*)m_pPart->m_pNode;
#else
    m_pNode = (MdlNodeTriMesh*)pPart->m_pNode;
#endif
    short nVertices = m_pNode->m_nVertexCount;
    if (nVertices == 0)
    {
        m_nArrayCount = 1;
    }
    else
    {
        if (m_pNode->m_bAnimateUV != 0)
        {
            GLRender::RequestPool(Aurora::POOL_VERTEX, 8 * nVertices,
                                  RequestPoolManager::POOL_STATIC, 0,
                                  &pPart->m_nTexCoordPool);
            m_nArrayCount += 2;
        }
        BuildVertexArrays();
        m_nArrayCount += 6;
        m_nArrayCount += 2 * m_pNode->m_nTextureCount;
        if (pPart->m_pMaterial->HasBumpMap() != 0 &&
            m_pNode->HasTangentBases() && AurBumpMapAvailable() != 0)
        {
#ifdef _XBOX
            m_nArrayCount += 9;
#else
            m_nPool = GLRender::GetFogArraysInfoRef();
            m_nArrayCount += 9;
            return;
#endif
        }
    }
    m_nPool = RequestPoolManager::INVALID_POOL;
}
// PC 0x00495560 GOG 0x00495620
void VertexPrimitiveFlat::BuildVertexArrays()
{
    AurprimitiveTrace2();
    MdlNodeTriMesh* pNode = m_pNode;

    if (pNode->m_bAnimateUV != 0)
    {
        PartTriMesh* pMesh = GetPartTriMesh();
        int nDest = (int)GLRender::LockPool(pMesh->m_nTexCoordPool, 0, 0,
                                              RequestPoolManager::POOL_LOCK_WRITE);
        pNode = m_pNode;
        Model* pModel = pNode->m_pModel->AsModel();
        unsigned long nPool = pModel->m_nVertexPool;
        int nSource = (int)GLRender::LockPool(
            nPool, 0, 0,
            RequestPoolManager::POOL_LOCK_READ_ONLY);

        for (int i = 0; i < m_pNode->m_nVertexCount; i++)
        {
            int nOffset = m_pNode->m_nTexCoord0Offset +
                          (m_pNode->m_nVertexDataOffset + i * m_pNode->m_nVertexStride);
            int* pTexCoord = (int*)(nSource + nOffset);
            ((int*)nDest)[2 * i] = pTexCoord[0];
            ((int*)nDest)[2 * i + 1] = pTexCoord[1];
        }

        GLRender::UnlockPool(pMesh->m_nTexCoordPool);
        nPool = m_pNode->m_pModel->AsModel()->m_nVertexPool;
        GLRender::UnlockPool(nPool);
    }
}
// PC 0x00495630 GOG 0x004956F0
unsigned char* VertexPrimitiveFlat::ReconnectArraysVAR(unsigned char* pData)
{
    AurprimitiveTrace2();
#ifdef _XBOX
    PartTriMesh* pMesh = GetPartTriMesh();
    pMesh->m_pNode->AsMdlNodeTriMesh();
    return pData;
#else
    int nVertices = m_pNode->m_nVertexCount;
    PartTriMesh* pMesh = GetPartTriMesh();
    MdlNodeTriMesh* pNode =
        pMesh->m_pNode->AsMdlNodeTriMesh();

    if (pMesh->m_bColors != 0)
    {
        glColorPointer(4, GL_UNSIGNED_BYTE, 0, pData);
        pData += 4 * nVertices;
        SetupColorState(true);
    }
    else
    {
        SetupColorState(false);
    }

    switch (m_pNode->m_nTextureCount)
    {
    case 4:
        GLRender::ActivateTextureStage(3);
        glTexCoordPointer(2, GL_FLOAT, pNode->m_nVertexStride,
                          pData + pNode->m_nTexCoord3Offset);
    case 3:
        GLRender::ActivateTextureStage(2);
        glTexCoordPointer(2, GL_FLOAT, pNode->m_nVertexStride,
                          pData + pNode->m_nTexCoord2Offset);
    case 2:
        GLRender::ActivateTextureStage(1);
        glTexCoordPointer(2, GL_FLOAT, pNode->m_nVertexStride,
                          pData + pNode->m_nTexCoord1Offset);
    case 1:
        GLRender::ActivateTextureStage(0);
        glTexCoordPointer(2, GL_FLOAT, pNode->m_nVertexStride,
                          pData + pNode->m_nTexCoord0Offset);
        break;
    default:
        break;
    }

    if (m_pNode->m_nTextureCount < 1)
    {
        GLRender_DisableClientState(
            CLIENT_STATE_TEXTURE_COORD);
    }
    else
    {
        GLRender_EnableClientState(
            CLIENT_STATE_TEXTURE_COORD);
    }

    glNormalPointer(GL_FLOAT, pNode->m_nVertexStride, pData + pNode->m_nNormalOffset);
    glVertexPointer(3, GL_FLOAT, pNode->m_nVertexStride,
                    pData + pNode->m_nPositionOffset);

    if (pMesh->m_pMaterial->HasBumpMap() != 0 && m_pNode->HasTangentBases() &&
        AurBumpMapAvailable() != 0)
    {
        int nStride = pNode->m_nVertexStride;
        unsigned char* pTangent = pData + pNode->m_TangentBases;
        if (usearbvertexprograms != 0)
        {
            glVertexAttribPointerARB(GLRender::VERTEX_ATTRIBUTE_TANGENT_BASIS0, 3, GL_FLOAT, 0, nStride, pTangent);
        }
        else if (usevertexprograms != 0 && AurVertexProgramsAvailable() != 0)
        {
            glVertexAttribPointerNV(GLRender::VERTEX_ATTRIBUTE_TANGENT_BASIS0, 3, GL_FLOAT, nStride, pTangent);
        }

        int nTangentBases = pNode->m_TangentBases;
        unsigned char* pBinormal = &pData[nTangentBases + 12];
        int nBinormalStride = pNode->m_nVertexStride;
        if (usearbvertexprograms != 0)
        {
            glVertexAttribPointerARB(GLRender::VERTEX_ATTRIBUTE_TANGENT_BASIS1, 3, GL_FLOAT, 0, nBinormalStride,
                                     pBinormal);
        }
        else if (usevertexprograms != 0 && AurVertexProgramsAvailable() != 0)
        {
            glVertexAttribPointerNV(GLRender::VERTEX_ATTRIBUTE_TANGENT_BASIS1, 3, GL_FLOAT, nBinormalStride,
                                    pBinormal);
        }

        unsigned char* pNormal = &pData[pNode->m_TangentBases + 24];
        int nNormalStride = pNode->m_nVertexStride;
        if (usearbvertexprograms != 0)
        {
            glVertexAttribPointerARB(GLRender::VERTEX_ATTRIBUTE_TANGENT_BASIS2, 3, GL_FLOAT, 0, nNormalStride,
                                     pNormal);
        }
        else if (usevertexprograms != 0 && AurVertexProgramsAvailable() != 0)
        {
            glVertexAttribPointerNV(GLRender::VERTEX_ATTRIBUTE_TANGENT_BASIS2, 3, GL_FLOAT, nNormalStride, pNormal);
        }
        GLRender::ActivateTextureStage(0);
    }

    return pData + nVertices * pNode->m_nVertexStride;
#endif
}
// PC 0x00495960 GOG 0x00495A20
unsigned char* VertexPrimitiveSkinned::ReconnectArraysVAR(unsigned char* pData)
{
    AurprimitiveTrace2();
    unsigned char* pNext = VertexPrimitiveFlat::ReconnectArraysVAR(pData);
#ifdef _XBOX
    int nVertices = m_pNode->m_nVertexCount;
    pNext += 16 * nVertices;
    return pNext + 4 * nVertices;
#else
    if (AurVertexProgramsAvailable() == 0)
    {
        return pNext;
    }

    int nVertices = m_pNode->m_nVertexCount;
    if (usearbvertexprograms != 0)
    {
        glVertexAttribPointerARB(GLRender::VERTEX_ATTRIBUTE_SKIN_WEIGHT,
                                 4, GL_FLOAT, 0, 0, pNext);
    }
    else if (usevertexprograms != 0 && AurVertexProgramsAvailable() != 0)
    {
        glVertexAttribPointerNV(GLRender::VERTEX_ATTRIBUTE_SKIN_WEIGHT,
                                4, GL_FLOAT, 0, pNext);
    }
    pNext += 16 * nVertices;
    if (usearbvertexprograms != 0)
    {
        glVertexAttribPointerARB(GLRender::VERTEX_ATTRIBUTE_BONE_INDEX,
                                 4, GL_SHORT, 0, 0, pNext);
    }
    else if (usevertexprograms != 0 && AurVertexProgramsAvailable() != 0)
    {
        glVertexAttribPointerNV(GLRender::VERTEX_ATTRIBUTE_BONE_INDEX,
                                4, GL_SHORT, 0, pNext);
    }
    return pNext + 4 * nVertices;
#endif
}
// PC 0x00495A30 GOG 0x00495AF0
void VertexPrimitiveFlat::SetupArrays()
{
    AurprimitiveTrace2();
    VertexPrimitiveAnim* pAnim = AsVertexPrimitiveAnim();
    AsVertexPrimitiveDangly();
    PartTriMesh* pMesh = GetPartTriMesh();
    MdlNodeTriMesh* pNode = m_pNode;

    if (pMesh->m_bColors == 1)
    {
        GLRender::SetColorBuffer(4, 0, pMesh->m_nColorPool,
                                 pNode->m_nVertexCount + 1);
    }

    unsigned long nOffset = pNode->m_nVertexDataOffset;
    int nCount = pNode->m_nVertexCount + 1;
    unsigned long nPool = m_pNode->m_pModel->AsModel()->m_nVertexPool;
    GLRender::SetInterleavedBuffer(
        m_pNode->m_nVertexStride, m_pNode->m_nVertexArrayFlags,
        nPool, nCount, pNode, nOffset);

    if (pAnim != 0)
    {
        GLRender::SetTexCoordBuffer(8, 0, pAnim->m_nTexCoordPool,
                                    pNode->m_nVertexCount);
        GLRender::SetVertexBuffer(12, 0, pAnim->m_nVertexPool,
                                  m_pNode->m_nVertexCount);
    }
}
// PC 0x00495AF0 GOG 0x00495BB0
void VertexPrimitive::SetupVertexArraysStandard()
{
    AurprimitiveTrace2();
    SetupColorState(GetPartTriMesh()->m_bColors != 0);
    SetupArrays();
    m_bRebuild = 0;
}

inline PartTriMesh* VertexPrimitive::GetPartTriMesh()
{
    return m_pPart->AsPartTriMesh();
}
// PC 0x00495DB0 GOG 0x00495E70
VertexPrimitiveAnim::VertexPrimitiveAnim(PartTriMesh* pPart)
    : VertexPrimitiveFlat(pPart)
{
    GLRender::RequestPool(Aurora::POOL_VERTEX, m_pNode->m_nVertexCount * 12, 2, 0,
                          &m_nVertexPool);
    GLRender::RequestPool(Aurora::POOL_VERTEX, m_pNode->m_nVertexCount * 8, 2, 0,
                          &m_nTexCoordPool);
    m_bStatic = 0;
}
// PC 0x00495E40 GOG 0x00495F00
VertexPrimitiveDangly::VertexPrimitiveDangly(PartTriMesh* pPart)
    : VertexPrimitiveFlat(pPart)
{
    int nVertices = m_pNode->m_nVertexCount;
    m_pOffsets = new Vector[nVertices];
    m_bStatic = 0;
}
// PC 0x00495ED0 GOG 0x00495F90
VertexPrimitiveSkinned::VertexPrimitiveSkinned(PartSkin* pPart)
    : VertexPrimitiveFlat(pPart)
{
    m_nArrayCount += 6;
}

static inline void AurprimitiveTrace2()
{
    AurprimitiveTrace1();
}

static inline void AurprimitiveTrace1()
{
}

#ifdef _XBOX

void VertexPrimitive::DeleteArrayPool()
{
    GLRender::DeletePool(m_nArrayPool);
    m_nArrayPool = RequestPoolManager::INVALID_POOL;
}
#endif
