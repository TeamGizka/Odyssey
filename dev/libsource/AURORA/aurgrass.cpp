#include <windows.h>
#include <GL/gl.h>
#include "aurrendergl.h"
#include <string.h>
#include "aurtexman.h"
#include "aurwindmanager.h"
#include "aurscene.h"

void AurTextureRelease(CAurTexture* pTexture);
#include "aurgrass.h"
#include "aurora.h"

static inline void AurgrassTrace2();
static inline void AurgrassTrace1();
#define AURGRASS_TRACE() AurgrassTrace2()

float CAurTriangleBin::m_fNearRadiusSquared = 225.0f;
float CAurTriangleBin::m_fFarRadiusSquared = 900.0f;
int newgrass = 1;
int rendertexturedgrass = 1;

int enablegrasswind = 1;
float grassstiffness = 1.0f;

int enablepointwind = 1;
int dowindstuff = 1;

List<CAurGrassInfo> g_GrassInfo;
// PC 0x004D2EA0 GOG 0x004D2FE0
void AurGrassClearInfo()
{
    g_GrassInfo.m_nCount = 0;
}
// PC 0x004D2EB0 GOG 0x004D2FF0
void CAurTriangleBin::FinishGeometry()
{
    m_vMaximum = (m_vBoundingBoxMinimum + m_vBoundingBoxMaximum) / 2.0f;
    m_fRadius = magnitude(m_vBoundingBoxMinimum - m_vBoundingBoxMaximum) * 0.5f;
}
// PC 0x004D2F40 GOG 0x004D3080
char AurGrassMaterialInUse(int nMaterial)
{
    AurgrassTrace1();
    if (nMaterial > g_GrassInfo.m_nCount - 1)
    {
        return 0;
    }

    return g_GrassInfo.m_pList[nMaterial].m_bInUse;
}
// PC 0x004D2F70 GOG 0x004D30B0
CAurGrassInfo::CAurGrassInfo()
{
    m_szTextureName[0] = 0;
    m_fDensity = 0;
    m_fQuadSize = 0;
    m_bInUse = 0;
    for (int i = 0; i < NUM_GRASS_PROBABILITIES; i++)
    {
        m_afProbability[i] = 0.25f;
    }
}
// PC 0x004D2FB0 GOG 0x004D30F0
void CAurTriangleBin::UpdateGrassForWind(WindManager* pWindManager)
{
    if (!(~m_nFlags & GRASS_POLYS_BUILT))
    {
        if (enablegrasswind && currentscene->IsAnimating())
        {
            float fTime;
            if (DeltaT < AUR_GRASS_WIND_MAX_TIMESTEP)
                fTime = DeltaT;
            else
                fTime = AUR_GRASS_WIND_MAX_TIMESTEP;

            static Vector vSpringDist;
            static Vector vForceDirection;
            static Vector vWind;
            static Vector vPoint;
            static Vector vMiddle;

            float fWindMagnitude = magnitude(pWindManager->m_vGlobalWind);
            float fQuadSize = g_GrassInfo[m_nGrassInfo].m_fQuadSize;
            float fWindScale = 1.0f;
            if (fWindMagnitude > AUR_GRASS_MAX_WIND_MAGNITUDE)
                fWindScale = AUR_GRASS_MAX_WIND_MAGNITUDE / fWindMagnitude;
            float fStep = grassstiffness * fQuadSize * fTime;

            for (int i = 0; i < m_nTriangles; i++)
            {
                int nVertex = i * 4;
                float fBaseX = m_pCurrentVertices[nVertex * 3];
                float fBaseY = m_pCurrentVertices[nVertex * 3 + 1];
                vMiddle.m_fZ = m_pCurrentVertices[nVertex * 3 + 2];
                float fSideX = m_pCurrentVertices[nVertex * 3 + 3];
                float fSideY = m_pCurrentVertices[nVertex * 3 + 4];
                vMiddle.m_fX = (fSideX + fBaseX) * 0.5f;
                vMiddle.m_fY = (fSideY + fBaseY) * 0.5f;
                vWind = fWindScale * pWindManager->GetGlobalWindNoPoint(vMiddle, 1.0f);
                vPoint = pWindManager->GetGlobalPointWind(vMiddle, fTime);

                float fTipX = m_pCurrentVertices[nVertex * 3 + 9];
                float fTipY = m_pCurrentVertices[nVertex * 3 + 10];
                vSpringDist.m_fZ = 0.0f;
                vSpringDist.m_fX = fBaseX - fTipX;
                vSpringDist.m_fY = fBaseY - fTipY;
                vForceDirection = vWind + vSpringDist + vPoint;
                if (!vForceDirection.IsZero())
                {
                    float fNewTipX = fTipX + vForceDirection.m_fX * fStep;
                    float fNewTipY = fTipY + vForceDirection.m_fY * fStep;
                    m_pCurrentVertices[nVertex * 3 + 9] = fNewTipX;
                    m_pCurrentVertices[nVertex * 3 + 10] = fNewTipY;
                    m_pCurrentVertices[nVertex * 3 + 6] = fNewTipX - fBaseX + fSideX;
                    m_pCurrentVertices[nVertex * 3 + 7] = fNewTipY - fBaseY + fSideY;
                }
            }
        }
    }
}

bool AurNV1xLevelShadersAvailable();
extern unsigned int ATI_FRAGMENT_SHADER_BIT;

int maxgrass = 0;
int newstuff = 0;
// PC 0x004D3430 GOG 0x004D3570
void CAurTriangleBin::RenderGrassPolys(int nGrassInfo, const Vector& vPosition,
                                      WindManager* pWindManager)
{
    AurgrassTrace2();
#ifndef _XBOX
    if (!AurNV1xLevelShadersAvailable() && !AurMultiTextureAvailable())
        return;
#endif
    if (~m_nFlags & GRASS_POLYS_BUILT)
        return;
    if (m_nTriangles == 0 || nGrassInfo != m_nGrassInfo)
        return;

    float fAlpha = 0.0f;
    float fNear = m_fNearRadiusSquared;
    float fFar = m_fFarRadiusSquared;
    if (maxgrass)
    {
        fNear = 1000000.0f;
        fFar = 10000000.0f;
    }
    if (m_fDistance < fNear)
        fAlpha = 1.0f;
    else if (m_fDistance < fFar)
        fAlpha = 1.0f - (m_fDistance - fNear) / (fFar - fNear);
    if (fAlpha == 0.0f)
        return;

#ifdef _XBOX

    GLRender_SetMaterialv(FACING_FRONT, MATERIAL_AMBIENT,
                          Homogenize(g_GrassInfo[nGrassInfo].m_vAmbient, 1.0f));
    GLRender_SetMaterialv(FACING_FRONT, MATERIAL_DIFFUSE,
                          Homogenize(g_GrassInfo[nGrassInfo].m_vDiffuse, fAlpha));
    int nVertices = m_nTriangles * 4;
    if (newgrass && m_fDistance < m_fNearRadiusSquared)
    {
        UpdateGrassForWind(pWindManager);
        void* pPool = GLRender::LockPool((unsigned long)m_pVertices, 0, 0, 1);
        memcpy(pPool, m_pCurrentVertices, nVertices * sizeof(Vector));
        GLRender::UnlockPool((unsigned long)m_pVertices);
    }
    if (rendertexturedgrass)
        m_pTexture->Bind();
    GLRender_EnableClientState(CLIENT_STATE_VERTEX);
    GLRender_EnableClientState(CLIENT_STATE_NORMAL);
    GLRender_EnableClientState(CLIENT_STATE_TEXTURE_COORD);
    int nStride = m_nVertexSize - 12;
    if ((m_nFlags & MULTI_TEXTURE) && rendertexturedgrass)
    {
        GLRender::ActivateTextureStage(1);
        GLRender_EnableClientState(CLIENT_STATE_TEXTURE_COORD);
        GLRender::ActivateTextureStage(0);
        int nFirst = 0;
        for (int i = 0; i < m_lstTextureIndices.m_nCount; i++)
        {
            GLRender_EnablePixelShaders(MODEL_SHADER_EMLM,
                m_pTexture.m_pObject, m_lstTextures[i].m_pObject, 0, 0);
            GLRender::DrawLightmappedGrass((void*)m_nVertexOffset, 12, (void*)m_nIndexOffset,
                nStride, nFirst, m_lstTextureIndices[i]);
            nFirst += m_lstTextureIndices[i] * 4;
        }
        GLRender_EnablePixelShaders(MODEL_SHADER_DISABLE, 0, 0, 0, 0);
        GLRender::ActivateTextureStage(1);
        GLRender_DisableClientState(CLIENT_STATE_TEXTURE_COORD);
        GLRender::ActivateTextureStage(0);
    }
    else
    {
        GLRender::DrawLightmappedGrass((void*)m_nVertexOffset, 12, (void*)m_nIndexOffset,
                                      nStride, 0, m_nTriangles);
    }
    countfaces += m_nTriangles;
#else
    float afAmbient[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    float afDiffuse[4] = {0.0f, 0.0f, 0.0f, fAlpha};
    SetMaterialParameter(FACING_FRONT, MATERIAL_AMBIENT, afAmbient);
    SetMaterialParameter(FACING_FRONT, MATERIAL_DIFFUSE, afDiffuse);
    float afEmissive[4] = {g_GrassInfo[nGrassInfo].m_vEmissive.m_fX,
                           g_GrassInfo[nGrassInfo].m_vEmissive.m_fY,
                           g_GrassInfo[nGrassInfo].m_vEmissive.m_fZ, fAlpha};
    SetMaterialParameter(FACING_FRONT, MATERIAL_EMISSION, afEmissive);
    if (newgrass && m_fDistance < m_fNearRadiusSquared)
    {
        m_pCurrentVertices = m_pVertices;
        UpdateGrassForWind(pWindManager);
        m_pCurrentVertices = 0;
    }
    if (rendertexturedgrass)
        m_pTexture->Bind();
    GLRender_EnableClientState(CLIENT_STATE_VERTEX);
    GLRender_EnableClientState(CLIENT_STATE_NORMAL);
    GLRender_EnableClientState(CLIENT_STATE_TEXTURE_COORD);
    int nStride = m_nVertexSize - 12;
    if ((m_nFlags & MULTI_TEXTURE) && rendertexturedgrass)
    {
        if (!AurNonRadeon8500Validate())
        {
            GLRender_Enable(ENABLE_VERTEX_PROGRAM);
            static VertexProgram* standardvp =
                new VertexProgram(usearbvertexprograms ? szStandardVPARB
                                                       : cStandardVP,
                                  cStandardVPDeclaration);
            standardvp->Enable(0, true);
            AurSetVertexProgramParameter4f(VertexProgram::PARAM_GRASS_DIFFUSE,
                g_GrassInfo[nGrassInfo].m_vDiffuse.m_fX,
                g_GrassInfo[nGrassInfo].m_vDiffuse.m_fY,
                g_GrassInfo[nGrassInfo].m_vDiffuse.m_fZ, fAlpha);
            AurSetVertexProgramParameter4f(VertexProgram::PARAM_GRASS_AMBIENT,
                g_GrassInfo[nGrassInfo].m_vAmbient.m_fX,
                g_GrassInfo[nGrassInfo].m_vAmbient.m_fY,
                g_GrassInfo[nGrassInfo].m_vAmbient.m_fZ, 1.0f);
        }
        if (AurNV1xLevelShadersAvailable())
        {
            GLRender::ActivateTextureStage(1);
            GLRender_EnableClientState(CLIENT_STATE_TEXTURE_COORD);
            GLRender::ActivateTextureStage(0);
            int nFirst = 0;
            for (int i = 0; i < m_lstTextureIndices.m_nCount; i++)
            {
                if (g_ForceSight)
                {
                    GLRender_EnablePixelShaders(MODEL_SHADER_DESAT_LM,
                        m_pTexture.m_pObject, m_lstTextures[i].m_pObject, 0, 0);
                    float afColor[4];
                    afColor[0] = 0.0f;
                    afColor[1] = 0.0f;
                    afColor[2] = 0.0f;
                    afColor[3] = 0.75f;
                    if ((existingextensions & ATI_FRAGMENT_SHADER_BIT) == ATI_FRAGMENT_SHADER_BIT)
                        glSetFragmentShaderConstantATI(GL_CON_1_ATI, afColor);
                    else
                        glCombinerParameterfvNV(GL_CONSTANT_COLOR1_NV, afColor);
                }
                else
                {
                    GLRender_EnablePixelShaders(MODEL_SHADER_NV1X_LIGHTMAP,
                        m_pTexture.m_pObject, m_lstTextures[i].m_pObject, 0, 0);
                }
                GLRender::DrawLightmappedGrass(m_pVertices, 12, &m_nIndexOffset,
                    nStride, nFirst, m_lstTextureIndices[i]);
                nFirst += m_lstTextureIndices[i] * 4;
            }
            if (g_ForceSight)
                GLRender_EnablePixelShaders(MODEL_SHADER_NOISE_DISABLE, 0, 0, 0, 0);
            else
                GLRender_EnablePixelShaders(MODEL_SHADER_DISABLE, 0, 0, 0, 0);
            GLRender::ActivateTextureStage(1);
            GLRender_DisableClientState(CLIENT_STATE_TEXTURE_COORD);
            GLRender::ActivateTextureStage(0);
        }
        else if (AurMultiTextureAvailable())
        {
            GLRender::ActivateTextureStage(1);
            GLRender_EnableClientState(CLIENT_STATE_TEXTURE_COORD);
            GLRender::ActivateTextureStage(0);
            int nFirst = 0;
            GLRender_Disable(ENABLE_LIGHTING);
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            GLRender::SetActiveTexture(0);
            GLRender_Enable(ENABLE_TEXTURE_2D);
            m_pTexture->Bind();
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
            for (int i = 0; i < m_lstTextureIndices.m_nCount; i++)
            {
                GLRender::SetActiveTexture(1);
                GLRender_Enable(ENABLE_TEXTURE_2D);
                m_lstTextures[i]->Bind();
                glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                GLRender::SetActiveTexture(0);
                GLRender::DrawLightmappedGrass(&m_nVertexOffset, 12, &m_nIndexOffset,
                    nStride, nFirst, m_lstTextureIndices[i]);
                nFirst += m_lstTextureIndices[i] * 4;
            }
            GLRender::SetActiveTexture(0);
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            GLRender::SetActiveTexture(1);
            GLRender_Disable(ENABLE_TEXTURE_2D);
            GLRender::SetActiveTexture(0);
            glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
            GLRender_Enable(ENABLE_LIGHTING);
            GLRender::ActivateTextureStage(1);
            GLRender_DisableClientState(CLIENT_STATE_TEXTURE_COORD);
            GLRender::ActivateTextureStage(0);
        }
        if (!AurNonRadeon8500Validate())
            GLRender_Disable(ENABLE_VERTEX_PROGRAM);
    }
    else
    {
        if (g_ForceSight)
        {
            GLRender_EnablePixelShaders(MODEL_SHADER_DESAT, m_pTexture.m_pObject, 0, 0, 0);
            float afColor[4];
            afColor[0] = 0.0f;
            afColor[1] = 0.0f;
            afColor[2] = 0.0f;
            afColor[3] = 1.0f;
            if ((existingextensions & ATI_FRAGMENT_SHADER_BIT) == ATI_FRAGMENT_SHADER_BIT)
                glSetFragmentShaderConstantATI(GL_CON_1_ATI, afColor);
            else
                glCombinerParameterfvNV(GL_CONSTANT_COLOR1_NV, afColor);
        }
        GLRender::DrawLightmappedGrass(m_pVertices, 12, &m_nIndexOffset,
                                      nStride, 0, m_nTriangles);
        if (g_ForceSight)
            GLRender_EnablePixelShaders(MODEL_SHADER_NOISE_DISABLE, 0, 0, 0, 0);
    }
    countfaces += m_nTriangles;
#endif
}
// PC 0x004D3AA0 GOG 0x004D3BE0
CAurTriangleListElem::CAurTriangleListElem()
{
    AurgrassTrace2();
    m_pFace = 0;
    m_pTriMesh = 0;
    m_nTriangle = 0;
}
// PC 0x004D3AC0 GOG 0x004D3C00
CAurTriangleListElem::CAurTriangleListElem(List<float> lstVertices,
                                           MaxFace* pFace,
                                           PartTriMesh* pTriMesh, int nTriangle)
{
    m_lstVertices = lstVertices;
    m_pFace = pFace;
    m_pTriMesh = pTriMesh;
    m_nTriangle = nTriangle;
}
// PC 0x004D3B50 GOG 0x004D3C90
CAurTriangleListElem::~CAurTriangleListElem()
{
    AURGRASS_TRACE();
#ifdef _XBOX
    m_lstVertices.SetSize(0);
#else
    if (m_lstVertices.m_pList != 0) {
        delete[] m_lstVertices.m_pList; m_lstVertices.m_pList = 0; }
    m_lstVertices.m_nAllocated = 0; m_lstVertices.m_nCount = 0;
#endif
    m_pFace = 0;
    m_pTriMesh = 0;
    m_nTriangle = 0;
}
// PC 0x004015A0 GOG 0x004015A0
void CAurTriangleBin::ReloadColor()
{
}
// PC 0x004D3DD0 GOG 0x004D3F10
void AurGrassAddInfo(int nMaterial, const CAurGrassInfo& info)
{
    while (nMaterial >= g_GrassInfo.m_nCount)
    {
        g_GrassInfo.Add(CAurGrassInfo());
    }

    CAurGrassInfo& material = g_GrassInfo[nMaterial];
    material.m_fDensity = info.m_fDensity;
    material.m_fQuadSize = info.m_fQuadSize;
    material.m_vAmbient = info.m_vAmbient;
    material.m_vDiffuse = info.m_vDiffuse;
    material.m_vEmissive = info.m_vEmissive;
    float fTotal = 0.0f;
    for (int i = 0; i < CAurGrassInfo::NUM_GRASS_PROBABILITIES; i++)
    {
        fTotal += info.m_afProbability[i];
        material.m_afProbability[i] = info.m_afProbability[i];
    }
    if (fTotal != 1.0f)
    {
        for (int i = 0; i < CAurGrassInfo::NUM_GRASS_PROBABILITIES; i++)
        {
            material.m_afProbability[i] /= fTotal;
        }
    }
    strcpy(g_GrassInfo[nMaterial].m_szTextureName, info.m_szTextureName);
    g_GrassInfo[nMaterial].m_bInUse = 1;
}
// PC 0x004D3FA0 GOG 0x004D40E0
int GrassSortTris(const void* pFirst, const void* pSecond)
{
    CAurTriangleListElem first(*(CAurTriangleListElem*)pFirst);
    CAurTriangleListElem second(*(CAurTriangleListElem*)pSecond);

    if (first.m_pTriMesh < second.m_pTriMesh)
        return -1;
    if (first.m_pTriMesh > second.m_pTriMesh)
        return 1;
    return 0;
}

#include "aurmaterial.h"

#include "aurmodel.h"

#include "aurrendergl.h"

#include <stdlib.h>

extern int enablelightmap;
// PC 0x004D4170 GOG 0x004D42B0
void CAurTriangleBin::AddTriangle(const MaxFace& face, PartTriMesh* pTriMesh,
                                  MdlNodeTriMesh* pNode)
{

    static Vector v0;
    static Vector v1;
    static Vector v2;
#ifdef _XBOX

    int nVertexPool = pNode->m_pModel->AsModel()->m_nVertexPool;
    unsigned char* pVertices = (unsigned char*)GLRender::LockPool(
        nVertexPool, 0, 0, RequestPoolManager::POOL_LOCK_READ_ONLY);
#else
    Vector* pVertices = pNode->m_pVertexPositions;
#endif
    if (m_nGrassInfo == -1)
    {
        m_nGrassInfo = face.m_nSurfaceType;
        AurTextureSetModeStatic();
        m_pTexture = AurTextureGetReference(g_GrassInfo[m_nGrassInfo].m_szTextureName, 0);
        AurTextureSetModeDynamic();
    }
#ifdef _XBOX
    float* pSource0 = (float*)(pVertices + pNode->m_nVertexStride * face.m_anVertexIndices[0] +
        pNode->m_nVertexDataOffset + pNode->m_nPositionOffset);
    v0.m_fX = pSource0[0];
    v0.m_fY = pSource0[1];
    v0.m_fZ = pSource0[2];
    float* pSource1 = (float*)(pVertices + pNode->m_nVertexStride * face.m_anVertexIndices[1] +
        pNode->m_nVertexDataOffset + pNode->m_nPositionOffset);
    v1.m_fX = pSource1[0];
    v1.m_fY = pSource1[1];
    v1.m_fZ = pSource1[2];
    float* pSource2 = (float*)(pVertices + pNode->m_nVertexStride * face.m_anVertexIndices[2] +
        pNode->m_nVertexDataOffset + pNode->m_nPositionOffset);
    v2.m_fX = pSource2[0];
    v2.m_fY = pSource2[1];
    v2.m_fZ = pSource2[2];
    GLRender::UnlockPool(pNode->m_pModel->AsModel()->m_nVertexPool);
#else
    const Vector& vSource0 = pVertices[face.m_anVertexIndices[0]];
    v0 = vSource0;
    const Vector& vSource1 = pVertices[face.m_anVertexIndices[1]];
    v1 = vSource1;
    const Vector& vSource2 = pVertices[face.m_anVertexIndices[2]];
    v2 = vSource2;
#endif

    Vector vPosition;
    Quaternion qOrientation;
    pTriMesh->GetWorld(vPosition, qOrientation);
    v0 = vPosition + qOrientation * v0;
    v1 = vPosition + qOrientation * v1;
    v2 = vPosition + qOrientation * v2;

    float fArea = magnitude(cross(v1 - v0, v2 - v1)) * 0.5f;
    int nTriangles;
    float fTriangles = fArea * g_GrassInfo[face.m_nSurfaceType].m_fDensity;
    roundToInt(nTriangles, fTriangles);
    if (nTriangles != 0)
    {
        if (pNode->m_bLightMapped && enablelightmap)
        {
            m_nFlags |= MULTI_TEXTURE;
        }
        else if (pTriMesh->m_bColors == 1)
        {
            if (~m_nFlags & MULTI_TEXTURE)
                m_nFlags |= FOUR_COMPONENTS;
        }
        List<float> lstVertices;
        lstVertices.Add(v0.m_fX);
        lstVertices.Add(v0.m_fY);
        lstVertices.Add(v0.m_fZ);
        lstVertices.Add(v1.m_fX);
        lstVertices.Add(v1.m_fY);
        lstVertices.Add(v1.m_fZ);
        lstVertices.Add(v2.m_fX);
        lstVertices.Add(v2.m_fY);
        lstVertices.Add(v2.m_fZ);
        m_lstTriangles.Add(CAurTriangleListElem(lstVertices, (MaxFace*)&face,
                                              pTriMesh, nTriangles));
        m_nTriangles += nTriangles;
        if (!(m_nFlags & HAS_BOUNDING_BOX))
        {
            m_vBoundingBoxMinimum = v0;
            m_vBoundingBoxMaximum = v0;
            m_nFlags |= HAS_BOUNDING_BOX;
        }
        else
        {
            m_vBoundingBoxMinimum = VectorMin(m_vBoundingBoxMinimum, v0);
            m_vBoundingBoxMaximum = VectorMax(m_vBoundingBoxMaximum, v0);
        }
        m_vBoundingBoxMinimum = VectorMin(m_vBoundingBoxMinimum, v1);
        m_vBoundingBoxMaximum = VectorMax(m_vBoundingBoxMaximum, v1);
        m_vBoundingBoxMinimum = VectorMin(m_vBoundingBoxMinimum, v2);
        m_vBoundingBoxMaximum = VectorMax(m_vBoundingBoxMaximum, v2);
        m_vMaximum = (m_vBoundingBoxMinimum + m_vBoundingBoxMaximum) * 0.5f;
    }
}
// PC 0x004D5060 GOG 0x004D51A0
CAurTriangleBin::CAurTriangleBin()
{
    m_nTriangles = 0;
    m_nFlags = 0;
    m_pTexture = 0;
#ifdef _XBOX
    m_pVertices = (float*)RequestPoolManager::INVALID_POOL;
#else
    m_pVertices = 0;
#endif
    m_pCurrentVertices = 0;
    m_nGrassInfo = -1;
    m_nVertexOffset = 0;
    m_nIndexOffset = 0;
}
// PC 0x004D5100 GOG 0x004D5240
CAurTriangleBin::~CAurTriangleBin()
{
    if (m_nFlags & HAS_VERTEX_DATA)
    {
#ifdef _XBOX
        delete[] m_pCurrentVertices;
        m_pCurrentVertices = 0;
        GLRender::DeletePool((unsigned long)m_pVertices);
        m_pVertices = (float*)RequestPoolManager::INVALID_POOL;
        delete (void*)m_nVertexOffset;
        m_nVertexOffset = 0;
        delete (void*)m_nIndexOffset;
        m_nIndexOffset = 0;
#else
        delete[] m_pCurrentVertices;
        m_pCurrentVertices = 0;
        delete[] m_pVertices;
        m_pVertices = 0;
#endif
        m_nFlags ^= HAS_VERTEX_DATA;
    }

    CAurTexture* pTexture = m_pTexture;
    m_pTexture = 0;
    AurTextureRelease(pTexture);
}
// PC 0x004D5210 GOG 0x004D5350
void CAurTriangleBin::DestroyGrassPolys()
{
    if (!(~m_nFlags & GRASS_POLYS_BUILT))
    {
        if (m_nFlags & HAS_VERTEX_DATA)
        {
#ifdef _XBOX
            delete[] m_pCurrentVertices;
            m_pCurrentVertices = 0;
            delete (void*)m_nVertexOffset;
            m_nVertexOffset = 0;
            delete (void*)m_nIndexOffset;
            m_nIndexOffset = 0;
            GLRender::DeletePool((unsigned long)m_pVertices);
            m_pVertices = (float*)RequestPoolManager::INVALID_POOL;
#else
            delete[] m_pCurrentVertices;
            m_pCurrentVertices = 0;
            delete[] m_pVertices;
            m_pVertices = 0;
#endif
            m_nFlags ^= HAS_VERTEX_DATA;
        }
        if (m_nFlags & MULTI_TEXTURE)
        {
            m_lstTextureIndices.SetSize(0);
            m_lstTextures.SetSize(0);
        }
        m_nFlags ^= GRASS_POLYS_BUILT;
    }
}
// PC 0x00664830 GOG 0x006645E0
void CAurTriangleBin::SetMinDistanceSquared(float fDistanceSquared)
{
    m_fDistance = fDistanceSquared;
}
// PC 0x004D52A0 GOG 0x004D53E0
void CAurTriangleBin::SortTriangles()
{
    if (m_lstTriangles.m_nCount == 1)
    {
        m_lstTextureIndices.Add(m_lstTriangles[0].m_nTriangle);
        m_lstTextures.Add(m_lstTriangles[0].m_pTriMesh->m_pMaterial->m_pTexture1);
        return;
    }

    List<CAurTriangleListElem> lstTriangles(m_lstTriangles);
    qsort(lstTriangles.m_pList, lstTriangles.m_nCount,
          sizeof(CAurTriangleListElem), GrassSortTris);
    int nTriangles = 0;
    int i;
    for (i = 1; i < lstTriangles.m_nCount; i++)
    {
        if (lstTriangles[i - 1].m_pTriMesh == lstTriangles[i].m_pTriMesh)
        {
            nTriangles += lstTriangles[i - 1].m_nTriangle;
        }
        else
        {
            m_lstTextures.Add(m_lstTriangles[i - 1].m_pTriMesh->m_pMaterial->m_pTexture1);
            m_lstTextureIndices.Add(nTriangles + lstTriangles[i - 1].m_nTriangle);
            nTriangles = 0;
        }
    }

    if (lstTriangles[i - 1].m_pTriMesh == lstTriangles[i - 2].m_pTriMesh) nTriangles += lstTriangles[i - 1].m_nTriangle;
    else
        nTriangles = lstTriangles[i - 1].m_nTriangle;
    m_lstTextures.Add(m_lstTriangles[i - 1].m_pTriMesh->m_pMaterial->m_pTexture1);
    m_lstTextureIndices.Add(nTriangles);
}
#ifndef _XBOX
// PC 0x004D5560 GOG 0x004D56A0
void CAurTriangleBin::CreateArrays()
{
    int nVertices = 4 * m_nTriangles;
    m_pVertices = (float*)new unsigned char[nVertices * m_nVertexSize];
    int nIndexOffset = 3 * sizeof(float) * nVertices;
    m_nIndexOffset = nIndexOffset;
    float* pVertices = m_pVertices;
    m_pCurrentVertices = pVertices;
    if (m_nFlags & MULTI_TEXTURE)
        SortTriangles();
    int nTriangles = m_lstTriangles.m_nCount;
    Vector vUp(0.0f, 0.0f, 1.0f);
    m_nFlags |= HAS_VERTEX_DATA;
    int nAttribute = nIndexOffset / sizeof(float) - 1;
    int nVertex = -1;
    Vector vWeight;
    Quaternion qRotation;
    for (int i = 0; i < nTriangles; ++i)
    {
        static Vector v0;
        static Vector v1;
        static Vector v2;
        v0.m_fX = m_lstTriangles[i].m_lstVertices[0];
        v0.m_fY = m_lstTriangles[i].m_lstVertices[1];
        v0.m_fZ = m_lstTriangles[i].m_lstVertices[2];
        v1.m_fX = m_lstTriangles[i].m_lstVertices[3];
        v1.m_fY = m_lstTriangles[i].m_lstVertices[4];
        v1.m_fZ = m_lstTriangles[i].m_lstVertices[5];
        v2.m_fX = m_lstTriangles[i].m_lstVertices[6];
        v2.m_fY = m_lstTriangles[i].m_lstVertices[7];
        v2.m_fZ = m_lstTriangles[i].m_lstVertices[8];
        int nGrass = m_lstTriangles[i].m_nTriangle;
        if (nGrass != 0)
        {
            int nSeed;
            roundToInt(nSeed, v2.m_fZ + v1.m_fY + v0.m_fX);
            srand(nSeed);
            float fQuadSize = g_GrassInfo[m_nGrassInfo].m_fQuadSize;
            Vector vBottomLeft(-0.5f * fQuadSize, 0.0f, 0.0f);
            Vector vBottomRight(0.5f * fQuadSize, 0.0f, 0.0f);
            Vector vTopRight(0.5f * fQuadSize, 0.0f, fQuadSize);
            Vector vTopLeft(-0.5f * fQuadSize, 0.0f, fQuadSize);
            MaxFace* pFace = m_lstTriangles[i].m_pFace;
            PartTriMesh* pTriMesh = m_lstTriangles[i].m_pTriMesh;
            MdlNodeTriMesh* pNode = pTriMesh->m_pNode->AsMdlNodeTriMesh();
            Vector vLight;
            unsigned char nRed = 0, nGreen = 0, nBlue = 0, nAlpha = 127;
            for (int j = 0; j < nGrass; ++j)
            {
                vWeight.m_fX = (rand() % 10000) * 0.0001f;
                vWeight.m_fY = (rand() % 10000) * 0.0001f;
                vWeight.m_fZ = (rand() % 10000) * 0.0001f;
                Vector& vNormalized = vWeight;
                vNormalized = vWeight / (vWeight.m_fX + vWeight.m_fY + vWeight.m_fZ);
                if (m_nFlags & MULTI_TEXTURE)
                {
                    int nVertexPool = pNode->m_pModel->AsModel()->m_nVertexPool;
                    unsigned char* pVertexData = (unsigned char*)GLRender::LockPool(
                        nVertexPool, 0, 0, RequestPoolManager::POOL_LOCK_READ_ONLY);
                    float* pUV0 = (float*)(pVertexData + pNode->m_nVertexDataOffset +
                        pNode->m_nTexCoord1Offset +
                        pNode->m_nVertexStride * pFace->m_anVertexIndices[0]);
                    Vector vUV0;
                    vUV0.m_fX = pUV0[0];
                    vUV0.m_fY = pUV0[1];
                    float* pUV1 = (float*)(pVertexData + pNode->m_nVertexDataOffset +
                        pNode->m_nTexCoord1Offset +
                        pNode->m_nVertexStride * pFace->m_anVertexIndices[1]);
                    Vector vUV1;
                    vUV1.m_fX = pUV1[0];
                    vUV1.m_fY = pUV1[1];
                    float* pUV2 = (float*)(pVertexData + pNode->m_nVertexDataOffset +
                        pNode->m_nTexCoord1Offset +
                        pNode->m_nVertexStride * pFace->m_anVertexIndices[2]);
                    Vector vUV2;
                    vUV2.m_fX = pUV2[0];
                    vUV2.m_fY = pUV2[1];
                    vLight = vWeight.m_fX * vUV0 + vWeight.m_fY * vUV1 + vWeight.m_fZ * vUV2;
                    nVertexPool = pNode->m_pModel->AsModel()->m_nVertexPool;
                    GLRender::UnlockPool(nVertexPool);
                }
                else if (m_nFlags & FOUR_COMPONENTS)
                {
                    int nColorPool = pTriMesh->m_nColorPool;
                    unsigned long* pColors = (unsigned long*)GLRender::LockPool(
                        nColorPool, 0, 0, RequestPoolManager::POOL_LOCK_READ_ONLY);
                    float fRed = (pColors[pFace->m_anVertexIndices[0]] & 0xff) * vNormalized.m_fX +
                                 (pColors[pFace->m_anVertexIndices[1]] & 0xff) * vNormalized.m_fY +
                                 (pColors[pFace->m_anVertexIndices[2]] & 0xff) * vNormalized.m_fZ;
                    float fGreen = ((pColors[pFace->m_anVertexIndices[0]] & 0xff00) >> 8) * vNormalized.m_fX +
                                   ((pColors[pFace->m_anVertexIndices[1]] & 0xff00) >> 8) * vNormalized.m_fY +
                                   ((pColors[pFace->m_anVertexIndices[2]] & 0xff00) >> 8) * vNormalized.m_fZ;
                    float fBlue = ((pColors[pFace->m_anVertexIndices[0]] & 0xff0000) >> 16) * vNormalized.m_fX +
                                  ((pColors[pFace->m_anVertexIndices[1]] & 0xff0000) >> 16) * vNormalized.m_fY +
                                  ((pColors[pFace->m_anVertexIndices[2]] & 0xff0000) >> 16) * vNormalized.m_fZ;
                    float fAlpha = ((pColors[pFace->m_anVertexIndices[0]] & 0xff000000) >> 24) * vNormalized.m_fX +
                                   ((pColors[pFace->m_anVertexIndices[1]] & 0xff000000) >> 24) * vNormalized.m_fY +
                                   ((pColors[pFace->m_anVertexIndices[2]] & 0xff000000) >> 24) * vNormalized.m_fZ;
                    float fMinimum = 0.0f;
                    float fMaximum = 255.0f;
                    nRed = (unsigned char)max(fMinimum, min(fMaximum, fRed));
                    nGreen = (unsigned char)max(fMinimum, min(fMaximum, fGreen));
                    nBlue = (unsigned char)max(fMinimum, min(fMaximum, fBlue));
                    nAlpha = (unsigned char)max(fMinimum, min(fMaximum, fAlpha));
                    nColorPool = pTriMesh->m_nColorPool;
                    GLRender::UnlockPool(nColorPool);
                }
                Vector vPosition = vWeight.m_fX * v0 + vWeight.m_fY * v1 + vWeight.m_fZ * v2;
                qRotation = Quaternion(vUp, ((rand() % 10000) * 0.0001f) * TWO_PI);
                float fProbability = (rand() % 10000) * 0.0001f;
                int nCell = 0;
                for (int nIndex = 0; nIndex < CAurGrassInfo::NUM_GRASS_PROBABILITIES; ++nIndex)
                {
                    if (fProbability < g_GrassInfo[m_nGrassInfo].m_afProbability[nIndex])
                    {
                        nCell = nIndex;
                        break;
                    }
                    fProbability -= g_GrassInfo[m_nGrassInfo].m_afProbability[nIndex];
                }
                float fU = nCell % 2 == 1 ? 0.5f : 0.0f;
                float fV = (nCell / 2) % 2 == 1 ? 0.5f : 0.0f;
                float fHeightScale = ((rand() % 10000) * 0.0001f - 0.5f) *
                                     0.4f + 1.0f;
                Vector vBottomLeftPosition = vPosition + (qRotation * vBottomLeft);
                ++nVertex;
                m_pCurrentVertices[nVertex * 3] = vBottomLeftPosition.m_fX;
                m_pCurrentVertices[nVertex * 3 + 1] = vBottomLeftPosition.m_fY;
                m_pCurrentVertices[nVertex * 3 + 2] = vBottomLeftPosition.m_fZ;
                pVertices[++nAttribute] = pFace->m_vNormal.m_fX;
                pVertices[++nAttribute] = pFace->m_vNormal.m_fY;
                pVertices[++nAttribute] = pFace->m_vNormal.m_fZ;
                pVertices[++nAttribute] = fU;
                pVertices[++nAttribute] = fV;
                if (m_nFlags & MULTI_TEXTURE)
                {
                    pVertices[++nAttribute] = vLight.m_fX;
                    pVertices[++nAttribute] = vLight.m_fY;
                }
                else if (m_nFlags & FOUR_COMPONENTS)
                {

                    pVertices[++nAttribute] = (float)(unsigned int)(nRed |
                        ((nGreen | ((nBlue | (nAlpha << 8)) << 8)) << 8));
                }
                Vector vBottomRightPosition = vPosition + (qRotation * vBottomRight);
                ++nVertex;
                m_pCurrentVertices[nVertex * 3] = vBottomRightPosition.m_fX;
                m_pCurrentVertices[nVertex * 3 + 1] = vBottomRightPosition.m_fY;
                m_pCurrentVertices[nVertex * 3 + 2] = vBottomRightPosition.m_fZ;
                pVertices[++nAttribute] = pFace->m_vNormal.m_fX;
                pVertices[++nAttribute] = pFace->m_vNormal.m_fY;
                pVertices[++nAttribute] = pFace->m_vNormal.m_fZ;
                pVertices[++nAttribute] = fU + 0.5f;
                pVertices[++nAttribute] = fV;
                if (m_nFlags & MULTI_TEXTURE)
                {
                    pVertices[++nAttribute] = vLight.m_fX;
                    pVertices[++nAttribute] = vLight.m_fY;
                }
                else if (m_nFlags & FOUR_COMPONENTS)
                {

                    pVertices[++nAttribute] = (float)(unsigned int)(nRed |
                        ((nGreen | ((nBlue | (nAlpha << 8)) << 8)) << 8));
                }
                Vector vTopRightPosition = vPosition + fHeightScale * (qRotation * vTopRight);
                ++nVertex;
                m_pCurrentVertices[nVertex * 3] = vTopRightPosition.m_fX;
                m_pCurrentVertices[nVertex * 3 + 1] = vTopRightPosition.m_fY;
                m_pCurrentVertices[nVertex * 3 + 2] = vTopRightPosition.m_fZ;
                pVertices[++nAttribute] = pFace->m_vNormal.m_fX;
                pVertices[++nAttribute] = pFace->m_vNormal.m_fY;
                pVertices[++nAttribute] = pFace->m_vNormal.m_fZ;
                pVertices[++nAttribute] = fU + 0.5f;
                pVertices[++nAttribute] = fV + 0.5f;
                if (m_nFlags & MULTI_TEXTURE)
                {
                    pVertices[++nAttribute] = vLight.m_fX;
                    pVertices[++nAttribute] = vLight.m_fY;
                }
                else if (m_nFlags & FOUR_COMPONENTS)
                {

                    pVertices[++nAttribute] = (float)(unsigned int)(nRed |
                        ((nGreen | ((nBlue | (nAlpha << 8)) << 8)) << 8));
                }
                Vector vTopLeftPosition = vPosition + fHeightScale * (qRotation * vTopLeft);
                ++nVertex;
                m_pCurrentVertices[nVertex * 3] = vTopLeftPosition.m_fX;
                m_pCurrentVertices[nVertex * 3 + 1] = vTopLeftPosition.m_fY;
                m_pCurrentVertices[nVertex * 3 + 2] = vTopLeftPosition.m_fZ;
                pVertices[++nAttribute] = pFace->m_vNormal.m_fX;
                pVertices[++nAttribute] = pFace->m_vNormal.m_fY;
                pVertices[++nAttribute] = pFace->m_vNormal.m_fZ;
                pVertices[++nAttribute] = fU;
                pVertices[++nAttribute] = fV + 0.5f;
                if (m_nFlags & MULTI_TEXTURE)
                {
                    pVertices[++nAttribute] = vLight.m_fX;
                    pVertices[++nAttribute] = vLight.m_fY;
                }
                else if (m_nFlags & FOUR_COMPONENTS)
                {

                    pVertices[++nAttribute] = (float)(unsigned int)(nRed |
                        ((nGreen | ((nBlue | (nAlpha << 8)) << 8)) << 8));
                }
            }
        }
    }
    m_pCurrentVertices = 0;
    m_nFlags |= GRASS_POLYS_BUILT;
}
#endif
void CAurTriangleBin::BuildGrassPolys()  // PC 0x004D68C0 GOG 0x004D6A00
{
    if (!(m_nFlags & GRASS_POLYS_BUILT) && m_nTriangles != 0)
    {
        bool bUseFourComponents = false;
        char bUseMultiTexture = 0;

        if (m_nFlags & MULTI_TEXTURE)
        {
            bUseMultiTexture = 1;
        }
        else if (m_nFlags & FOUR_COMPONENTS)
        {
            bUseFourComponents = true;
        }

#ifdef _XBOX
        m_nVertexSize = 24;
        if (bUseMultiTexture)
        {
            m_nVertexSize = 32;
        }
        else if (bUseFourComponents)
        {
            m_nVertexSize = 28;
        }
#else
        m_nVertexSize = 32;
        if (bUseMultiTexture)
        {
            m_nVertexSize = 40;
        }
        else if (bUseFourComponents)
        {
            m_nVertexSize = 36;
        }
#endif
        CreateArrays();
    }
}

static inline void AurgrassTrace2()
{
    AurgrassTrace1();
}

static inline void AurgrassTrace1()
{
}

#ifdef _XBOX

void CAurTriangleBin::CreateArrays()
{
    int nVertices = 4 * m_nTriangles;
    GLRender::RequestPool(Aurora::POOL_VERTEX, nVertices * m_nVertexSize,
                          RequestPoolManager::POOL_COLOR | RequestPoolManager::POOL_STATIC, 0,
                          (unsigned long*)&m_pVertices);
    float* pVertices = (float*)GLRender::LockPool((unsigned long)m_pVertices, 0, 0,
                                                  RequestPoolManager::POOL_LOCK_WRITE);
    int nIndexOffset = 3 * sizeof(float) * nVertices;
    m_pCurrentVertices = (float*)new unsigned char[nIndexOffset];
    m_nVertexOffset = (int)new D3DVertexBuffer;
    memset((void*)m_nVertexOffset, 0, sizeof(D3DVertexBuffer));
    m_nIndexOffset = (int)new D3DVertexBuffer;
    memset((void*)m_nIndexOffset, 0, sizeof(D3DVertexBuffer));
    GLRender::SetPoolBufferHeaders((unsigned long)m_pVertices, (D3DVertexBuffer*)m_nVertexOffset,
                                   (D3DVertexBuffer*)m_nIndexOffset, nIndexOffset);
    if (m_nFlags & MULTI_TEXTURE)
        SortTriangles();
    int nTriangles = m_lstTriangles.m_nCount;
    m_nFlags |= HAS_VERTEX_DATA;
    int nAttribute = nIndexOffset / sizeof(float) - 1;
    Vector vUp(0.0f, 0.0f, 1.0f);
    int nVertex = -1;
    Vector vWeight;
    Quaternion qRotation;
    Vector vPosition;
    Vector vVertex;
    for (int i = 0; i < nTriangles; ++i)
    {
        static Vector v0;
        static Vector v1;
        static Vector v2;
        v0.m_fX = m_lstTriangles[i].m_lstVertices[0];
        v0.m_fY = m_lstTriangles[i].m_lstVertices[1];
        v0.m_fZ = m_lstTriangles[i].m_lstVertices[2];
        v1.m_fX = m_lstTriangles[i].m_lstVertices[3];
        v1.m_fY = m_lstTriangles[i].m_lstVertices[4];
        v1.m_fZ = m_lstTriangles[i].m_lstVertices[5];
        v2.m_fX = m_lstTriangles[i].m_lstVertices[6];
        v2.m_fY = m_lstTriangles[i].m_lstVertices[7];
        v2.m_fZ = m_lstTriangles[i].m_lstVertices[8];
        int nGrass = m_lstTriangles[i].m_nTriangle;
        if (nGrass != 0)
        {
            int nSeed;
            roundToInt(nSeed, v2.m_fZ + v1.m_fY + v0.m_fX);
            srand(nSeed);
            float fQuadSize = g_GrassInfo[m_nGrassInfo].m_fQuadSize;
            Vector vBottomLeft(-0.5f * fQuadSize, 0.0f, 0.0f);
            Vector vBottomRight(0.5f * fQuadSize, 0.0f, 0.0f);
            Vector vTopRight(0.5f * fQuadSize, 0.0f, fQuadSize);
            Vector vTopLeft(-0.5f * fQuadSize, 0.0f, fQuadSize);
            MaxFace* pFace = m_lstTriangles[i].m_pFace;
            PartTriMesh* pTriMesh = m_lstTriangles[i].m_pTriMesh;

            unsigned long nPackedNormal =
                ((((int)(pFace->m_vNormal.m_fZ * 511.0f) << 11) |
                  ((int)(pFace->m_vNormal.m_fY * 1023.0f) & 0x7ff)) << 11) |
                ((int)(pFace->m_vNormal.m_fX * 1023.0f) & 0x7ff);
            MdlNodeTriMesh* pNode = pTriMesh->m_pNode->AsMdlNodeTriMesh();
            Vector vLight(0.0f, 0.0f, 0.0f);
            unsigned char nRed = 0, nGreen = 0, nBlue = 0, nAlpha = 127;
            for (int j = 0; j < nGrass; ++j)
            {
                vWeight.m_fX = (rand() % 10000) * 0.0001f;
                vWeight.m_fY = (rand() % 10000) * 0.0001f;
                vWeight.m_fZ = (rand() % 10000) * 0.0001f;
                vWeight = vWeight / (vWeight.m_fX + vWeight.m_fY + vWeight.m_fZ);
                if (m_nFlags & MULTI_TEXTURE)
                {
                    int nVertexPool = pNode->m_pModel->AsModel()->m_nVertexPool;
                    unsigned char* pVertexData = (unsigned char*)GLRender::LockPool(
                        nVertexPool, 0, 0, RequestPoolManager::POOL_LOCK_READ_ONLY);
                    float* pUV0 = (float*)(pVertexData + pNode->m_nVertexDataOffset +
                        pNode->m_nTexCoord1Offset +
                        pNode->m_nVertexStride * pFace->m_anVertexIndices[0]);
                    Vector vUV0;
                    vUV0.m_fX = pUV0[0];
                    vUV0.m_fY = pUV0[1];
                    float* pUV1 = (float*)(pVertexData + pNode->m_nVertexDataOffset +
                        pNode->m_nTexCoord1Offset +
                        pNode->m_nVertexStride * pFace->m_anVertexIndices[1]);
                    Vector vUV1;
                    vUV1.m_fX = pUV1[0];
                    vUV1.m_fY = pUV1[1];
                    float* pUV2 = (float*)(pVertexData + pNode->m_nVertexDataOffset +
                        pNode->m_nTexCoord1Offset +
                        pNode->m_nVertexStride * pFace->m_anVertexIndices[2]);
                    Vector vUV2;
                    vUV2.m_fX = pUV2[0];
                    vUV2.m_fY = pUV2[1];
                    vLight = vWeight.m_fX * vUV0 + vWeight.m_fY * vUV1 + vWeight.m_fZ * vUV2;
                    nVertexPool = pNode->m_pModel->AsModel()->m_nVertexPool;
                    GLRender::UnlockPool(nVertexPool);
                }
                else if (m_nFlags & FOUR_COMPONENTS)
                {
                    int nColorPool = pTriMesh->m_nColorPool;
                    unsigned long* pColors = (unsigned long*)GLRender::LockPool(
                        nColorPool, 0, 0, RequestPoolManager::POOL_LOCK_READ_ONLY);
                    float fWeightA = vWeight.m_fX;
                    float fWeightB = vWeight.m_fY;
                    float fWeightC = vWeight.m_fZ;
                    float fRed = (pColors[pFace->m_anVertexIndices[0]] & 0xff) * fWeightA +
                                 (pColors[pFace->m_anVertexIndices[1]] & 0xff) * fWeightB +
                                 (pColors[pFace->m_anVertexIndices[2]] & 0xff) * fWeightC;
                    float fGreen = ((pColors[pFace->m_anVertexIndices[0]] & 0xff00) >> 8) * vWeight.m_fX +
                                   ((pColors[pFace->m_anVertexIndices[1]] & 0xff00) >> 8) * vWeight.m_fY +
                                   ((pColors[pFace->m_anVertexIndices[2]] & 0xff00) >> 8) * vWeight.m_fZ;
                    float fBlue = ((pColors[pFace->m_anVertexIndices[0]] & 0xff0000) >> 16) * vWeight.m_fX +
                                  ((pColors[pFace->m_anVertexIndices[1]] & 0xff0000) >> 16) * vWeight.m_fY +
                                  ((pColors[pFace->m_anVertexIndices[2]] & 0xff0000) >> 16) * vWeight.m_fZ;
                    float fAlpha = ((pColors[pFace->m_anVertexIndices[0]] & 0xff000000) >> 24) * vWeight.m_fX +
                                   ((pColors[pFace->m_anVertexIndices[1]] & 0xff000000) >> 24) * vWeight.m_fY +
                                   ((pColors[pFace->m_anVertexIndices[2]] & 0xff000000) >> 24) * vWeight.m_fZ;
                    nRed = (unsigned char)max(0.0f, min(255.0f, fRed));
                    nGreen = (unsigned char)max(0.0f, min(255.0f, fGreen));
                    nBlue = (unsigned char)max(0.0f, min(255.0f, fBlue));
                    nAlpha = (unsigned char)max(0.0f, min(255.0f, fAlpha));
                    nColorPool = pTriMesh->m_nColorPool;
                    GLRender::UnlockPool(nColorPool);
                }
                vPosition = vWeight.m_fX * v0 + vWeight.m_fY * v1 + vWeight.m_fZ * v2;
                qRotation = Quaternion(vUp, ((rand() % 10000) * 0.0001f) * TWO_PI);
                float fProbability = (rand() % 10000) * 0.0001f;
                int nCell = 0;
                for (int nIndex = 0; nIndex < CAurGrassInfo::NUM_GRASS_PROBABILITIES; ++nIndex)
                {
                    if (fProbability < g_GrassInfo[m_nGrassInfo].m_afProbability[nIndex])
                    {
                        nCell = nIndex;
                        break;
                    }
                    fProbability -= g_GrassInfo[m_nGrassInfo].m_afProbability[nIndex];
                }
                float fU = nCell % 2 == 1 ? 0.5f : 0.0f;
                float fV = (nCell / 2) % 2 == 1 ? 0.5f : 0.0f;
                float fHeightScale = ((rand() % 10000) * 0.0001f - 0.5f) *
                                     0.4f + 1.0f;
                vVertex = (qRotation * vBottomLeft) + vPosition;
                ++nVertex;
                m_pCurrentVertices[nVertex * 3] = vVertex.m_fX;
                m_pCurrentVertices[nVertex * 3 + 1] = vVertex.m_fY;
                m_pCurrentVertices[nVertex * 3 + 2] = vVertex.m_fZ;
                pVertices[nVertex * 3] = vVertex.m_fX;
                pVertices[nVertex * 3 + 1] = vVertex.m_fY;
                pVertices[nVertex * 3 + 2] = vVertex.m_fZ;
                ((unsigned long*)pVertices)[++nAttribute] = nPackedNormal;
                pVertices[++nAttribute] = fU;
                pVertices[++nAttribute] = fV;
                if (m_nFlags & MULTI_TEXTURE)
                {
                    pVertices[++nAttribute] = vLight.m_fX;
                    pVertices[++nAttribute] = vLight.m_fY;
                }
                else if (m_nFlags & FOUR_COMPONENTS)
                {

                    pVertices[++nAttribute] = (float)(unsigned int)(nRed |
                        ((nGreen | ((nBlue | (nAlpha << 8)) << 8)) << 8));
                }
                vVertex = vPosition + (qRotation * vBottomRight);
                ++nVertex;
                m_pCurrentVertices[nVertex * 3] = vVertex.m_fX;
                m_pCurrentVertices[nVertex * 3 + 1] = vVertex.m_fY;
                m_pCurrentVertices[nVertex * 3 + 2] = vVertex.m_fZ;
                pVertices[nVertex * 3] = vVertex.m_fX;
                pVertices[nVertex * 3 + 1] = vVertex.m_fY;
                pVertices[nVertex * 3 + 2] = vVertex.m_fZ;
                ((unsigned long*)pVertices)[++nAttribute] = nPackedNormal;
                pVertices[++nAttribute] = fU + 0.5f;
                pVertices[++nAttribute] = fV;
                if (m_nFlags & MULTI_TEXTURE)
                {
                    pVertices[++nAttribute] = vLight.m_fX;
                    pVertices[++nAttribute] = vLight.m_fY;
                }
                else if (m_nFlags & FOUR_COMPONENTS)
                {

                    pVertices[++nAttribute] = (float)(unsigned int)(nRed |
                        ((nGreen | ((nBlue | (nAlpha << 8)) << 8)) << 8));
                }
                vVertex = vPosition + fHeightScale * (qRotation * vTopRight);
                ++nVertex;
                m_pCurrentVertices[nVertex * 3] = vVertex.m_fX;
                m_pCurrentVertices[nVertex * 3 + 1] = vVertex.m_fY;
                m_pCurrentVertices[nVertex * 3 + 2] = vVertex.m_fZ;
                pVertices[nVertex * 3] = vVertex.m_fX;
                pVertices[nVertex * 3 + 1] = vVertex.m_fY;
                pVertices[nVertex * 3 + 2] = vVertex.m_fZ;
                ((unsigned long*)pVertices)[++nAttribute] = nPackedNormal;
                pVertices[++nAttribute] = fU + 0.5f;
                pVertices[++nAttribute] = fV + 0.5f;
                if (m_nFlags & MULTI_TEXTURE)
                {
                    pVertices[++nAttribute] = vLight.m_fX;
                    pVertices[++nAttribute] = vLight.m_fY;
                }
                else if (m_nFlags & FOUR_COMPONENTS)
                {

                    pVertices[++nAttribute] = (float)(unsigned int)(nRed |
                        ((nGreen | ((nBlue | (nAlpha << 8)) << 8)) << 8));
                }
                vVertex = vPosition + fHeightScale * (qRotation * vTopLeft);
                ++nVertex;
                m_pCurrentVertices[nVertex * 3] = vVertex.m_fX;
                m_pCurrentVertices[nVertex * 3 + 1] = vVertex.m_fY;
                m_pCurrentVertices[nVertex * 3 + 2] = vVertex.m_fZ;
                pVertices[nVertex * 3] = vVertex.m_fX;
                pVertices[nVertex * 3 + 1] = vVertex.m_fY;
                pVertices[nVertex * 3 + 2] = vVertex.m_fZ;
                ((unsigned long*)pVertices)[++nAttribute] = nPackedNormal;
                pVertices[++nAttribute] = fU;
                pVertices[++nAttribute] = fV + 0.5f;
                if (m_nFlags & MULTI_TEXTURE)
                {
                    pVertices[++nAttribute] = vLight.m_fX;
                    pVertices[++nAttribute] = vLight.m_fY;
                }
                else if (m_nFlags & FOUR_COMPONENTS)
                {

                    pVertices[++nAttribute] = (float)(unsigned int)(nRed |
                        ((nGreen | ((nBlue | (nAlpha << 8)) << 8)) << 8));
                }
            }
        }
    }
    GLRender::UnlockPool((unsigned long)m_pVertices);
    m_nFlags |= GRASS_POLYS_BUILT;
}
#endif
