#include <string.h>
#include <windows.h>
#include <GL/gl.h>

#include "../math3d/math3d.h"
#include "aurmodel.h"
#include "aurprimitive.h"
#include "aurmodel.h"
#include "aurrendergl.h"
#include "aurscene.h"
#include "aurcamera.h"
#include "aurlightmanager.h"
#include "swmain.h"
#include "aurmaterialinl.h"

#pragma inline_recursion(on)
#pragma inline_depth(10)

static VertexProgram* pSkinVP = 0;
static VertexProgram* pHoloSkinVP = 0;
static VertexProgram* pSkinBumpyVP = 0;
static VertexProgram* pSkinSphereMapVP = 0;
static VertexProgram* pSkinCubeMapVP = 0;
float g_DesatureFactor = 0.65f;
int usebumpdiffuse = 1;
int usebumpspecular = 1;
int use3boneskinning = 0;
int usenewbumpyskin = 1;
int bumpyskinpass = 1;

int lightskinpass = 1;
int fogskinpass = 1;
int blendmodeone = 1;
int modecolor = 1;
VertexProgram* g_pBumpyShinySkinPass1 = 0;
VertexProgram* g_pBumpyShinySkinPass2 = 0;
VertexProgram* g_pBumpyShinySkinPass2_3bone = 0;
VertexProgram* g_pSkinBumpedOutVP = NULL;
VertexProgram* g_pSkinBumpedOutWithTangentVP = NULL;
int renderskin = 1;
extern int enablebumpyshiny;
extern float diffuseintensity;
extern float specularintensity;
int AurBumpyShinyAvailable();
unsigned char AurBumpMapCardValidate();
void SetBumpLightColor(Vector& vColor);
void SetupForceSightShader(Gob* pGob, int nForceShader, int nShader,
                           CAurTexture* pTexture0, CAurTexture* pTexture1,
                           CAurTexture* pTexture2, CAurTexture* pTexture3,
                           bool bKeepBlend);

#ifdef _XBOX
VertexProgram* GetSkinVP(bool)
{
    static VertexProgram* pSkinVP = new VertexProgram(cSkinVP, cSkinVPDeclaration);

    return pSkinVP;
}
#else
// PC 0x004A06B0 GOG 0x004A0770
VertexProgram* GetSkinVP(bool bHologram)
{
    if (!pSkinVP)
    {
        pSkinVP = new VertexProgram(
            usearbvertexprograms ?
            szSkinVPARB
            :
            cSkinVP,
            cSkinVPDeclaration);
    }
    if (!pHoloSkinVP)
    {
        pHoloSkinVP = new VertexProgram(
            usearbvertexprograms ?
            cHoloSkinVPARB
            :
            cSkinVP,
            cSkinVPDeclaration);
    }

    if (!bHologram)
        return pSkinVP;
    return pHoloSkinVP;
}
#endif

VertexProgram* GetSkinBumpyVP()
{
    if (!pSkinBumpyVP)
    {
        pSkinBumpyVP = new VertexProgram(
            cSkinBumpyVP,
            cSkinBumpyVPDeclaration);
    }

    return pSkinBumpyVP;
}
// PC 0x004A0790 GOG 0x004A0850
VertexProgram* GetSkinSphereMapVP()
{
    if (!pSkinSphereMapVP)
    {
        pSkinSphereMapVP = new VertexProgram(
            cSkinSphereMapVP,
            cSkinSphereMapVPDeclaration);
    }

    return pSkinSphereMapVP;
}
// PC 0x004A0810 GOG 0x004A08D0
VertexProgram* GetSkinCubeMapVP()
{
    if (!pSkinCubeMapVP)
    {
        pSkinCubeMapVP = new VertexProgram(
            usearbvertexprograms ?
            szSkinCubeMapVPARB
            :
            cSkinCubeMapVP,
            cSkinCubeMapVPDeclaration);
    }

    return pSkinCubeMapVP;
}

bool AurNV2xLevelShadersAvailable();
// PC 0x004A08A0 GOG 0x004A0960
Part* MdlNodeSkin::InternalCreateInstance(Gob*)
{
    return new PartSkin(this);
}

static inline unsigned long RequestSkinPool(unsigned long nSize, unsigned long* pnPool)
{
    return GLRender::RequestPool(Aurora::POOL_VERTEX, nSize,
                                 RequestPoolManager::POOL_COLOR | RequestPoolManager::POOL_STATIC,
                                 0, pnPool);
}

inline MdlNodeTriMesh* VertexPrimitive::GetNode() { return m_pNode; }

#define DEBUG_SKINPOOLS 0
// PC 0x004A0900 GOG 0x004A09C0
#ifdef _XBOX
void ExecuteSkinTransforms(MdlNodeSkin* pNode, List<Vector>& tbones,
                           List<Matrix>& rbones,
                           VertexPrimitiveSkinned* pPrimitive)
#else
void ExecuteSkinTransforms(MdlNodeSkin* pNode, List<Vector>& tbones,
                           List<Matrix>& rbones,
                           VertexPrimitiveSkinned* pPrimitive, bool bBumpedOut)
#endif
{

    static unsigned long currSizePoolCoords = pNode->m_nVertexCount;
    static unsigned long vertexToken = RequestSkinPool(
        currSizePoolCoords * sizeof(Vector), &vertexToken);
    static unsigned long normalToken = RequestSkinPool(
        currSizePoolCoords * sizeof(Vector), &normalToken);

    unsigned long nVertexCount = pNode->m_nVertexCount;
    if (nVertexCount > currSizePoolCoords)
    {
        currSizePoolCoords = nVertexCount;
        GLRender::ResizePool(vertexToken, 12 * currSizePoolCoords);
        GLRender::ResizePool(normalToken, 12 * currSizePoolCoords);
    }
    Vector* pCoords = (Vector*)GLRender::LockPool(vertexToken, 0, 0,
        RequestPoolManager::POOL_LOCK_WRITE);
    Vector* pNormals = (Vector*)GLRender::LockPool(normalToken, 0, 0,
        RequestPoolManager::POOL_LOCK_WRITE);
    unsigned long nPool = pNode->m_pModel->AsModel()->GetVertexPool();
#ifdef _XBOX
    unsigned char* pVertices = (unsigned char*)GLRender::LockPool(nPool, 0, 0,
        RequestPoolManager::POOL_LOCK_WRITE);
#else
    unsigned char* pVertices = (unsigned char*)GLRender::LockPool(nPool, 0, 0,
        RequestPoolManager::POOL_LOCK_READ_WRITE);
#endif
    if (DEBUG_SKINPOOLS)
    {
        for (unsigned long n = 0; n < nVertexCount; n++)
        {
            pCoords[n].m_fX = pNormals[n].m_fX;
            pCoords[n].m_fY = pNormals[n].m_fY;
            pCoords[n].m_fZ = pNormals[n].m_fZ;
            pNormals[n].m_fX = 0.0f;
            pNormals[n].m_fY = 0.0f;
            pNormals[n].m_fZ = 1.0f;
            pCoords[n].m_fX = pNormals[n].m_fX;
            pCoords[n].m_fY = pNormals[n].m_fY;
            pCoords[n].m_fZ = pNormals[n].m_fZ;
            pNormals[n].m_fX = 0.0f;
            pNormals[n].m_fY = 0.0f;
            pNormals[n].m_fZ = 1.0f;
            pCoords[n].m_fX = pNormals[n].m_fX;
            pCoords[n].m_fY = pNormals[n].m_fY;
            pCoords[n].m_fZ = pNormals[n].m_fZ;
            pNormals[n].m_fX = 0.0f;
            pNormals[n].m_fY = 0.0f;
            pNormals[n].m_fZ = 1.0f;
            pCoords[n].m_fX = pNormals[n].m_fX;
            pCoords[n].m_fY = pNormals[n].m_fY;
            pCoords[n].m_fZ = pNormals[n].m_fZ;
            pNormals[n].m_fX = 0.0f;
            pNormals[n].m_fY = 0.0f;
            pNormals[n].m_fZ = 1.0f;
            pCoords[n].m_fX = pNormals[n].m_fX;
            pCoords[n].m_fY = pNormals[n].m_fY;
            pCoords[n].m_fZ = pNormals[n].m_fZ;
            pNormals[n].m_fX = 0.0f;
            pNormals[n].m_fY = 0.0f;
            pNormals[n].m_fZ = 1.0f;
            pCoords[n].m_fX = pNormals[n].m_fX;
            pCoords[n].m_fY = pNormals[n].m_fY;
        }
    }

    Vector coord;
    Vector normal;
    Vector newcoord;
    Vector newnormal;
    unsigned short* pIndices;
    for (int i = 0; i < pPrimitive->GetNode()->m_lstIndexPools.m_nCount; i++)
    {
        nPool = pPrimitive->m_pNode->m_lstIndexPools[i];
        pIndices = (unsigned short*)GLRender::LockPool(
            nPool, 0, 0,
            RequestPoolManager::POOL_LOCK_READ_ONLY);
        for (int j = 0; j < pPrimitive->m_pNode->m_lstIndexCounts[i]; j++)
        {
            int nVertex = pIndices[j];
            Vector* pPosition = (Vector*)(pVertices + pNode->m_nVertexDataOffset +
                nVertex * pNode->m_nVertexStride + pNode->m_nPositionOffset);
            Vector* pNormal = (Vector*)(pVertices + pNode->m_nVertexDataOffset +
                nVertex * pNode->m_nVertexStride + pNode->m_nNormalOffset);
#ifdef _XBOX
            coord.m_fX = pPosition->m_fX;
            coord.m_fY = pPosition->m_fY;
            coord.m_fZ = pPosition->m_fZ;
            normal.m_fX = pNormal->m_fX;
            normal.m_fY = pNormal->m_fY;
            normal.m_fZ = pNormal->m_fZ;
#else
            coord = *pPosition;
            normal = *pNormal;
#endif
            newcoord.m_fX = newcoord.m_fY = newcoord.m_fZ = 0.0f;
            newnormal.m_fX = newnormal.m_fY = newnormal.m_fZ = 0.0f;
            for (int k = 0; k < 4; k++)
            {
                float* pWeights = (float*)(pVertices +
                    pNode->m_nVertexDataOffset + nVertex * pNode->m_nVertexStride +
                    pNode->m_nSkinWeightOffset);
                float fWeight = pWeights[k];
                if (fWeight != 0.0f)
                {
#ifdef _XBOX
                    int nBone = pNode->m_anBoneRemap[((unsigned short*)(pVertices +
                        pNode->m_nVertexDataOffset + nVertex * pNode->m_nVertexStride +
                        pNode->m_nBoneIndexOffset))[k]];
#else
                    int nBone = pNode->m_anBoneRemap[(int)((float*)(pVertices +
                        pNode->m_nVertexDataOffset + nVertex * pNode->m_nVertexStride +
                        pNode->m_nBoneIndexOffset))[k]];
#endif
                    newcoord += fWeight * (tbones[nBone] + rbones[nBone] * coord);
                    newnormal += fWeight * (rbones[nBone] * normal);
                }
            }
            pCoords[nVertex].m_fX = newcoord.m_fX;
            pCoords[nVertex].m_fY = newcoord.m_fY;
            pCoords[nVertex].m_fZ = newcoord.m_fZ;
            pNormals[nVertex].m_fX = newnormal.m_fX;
            pNormals[nVertex].m_fY = newnormal.m_fY;
            pNormals[nVertex].m_fZ = newnormal.m_fZ;
        }
        GLRender::UnlockPool(pPrimitive->m_pNode->m_lstIndexPools[i]);
    }
    nPool = pPrimitive->m_pNode->GetAuxIndexPool();
    pIndices = (unsigned short*)GLRender::LockPool(
        nPool, 0, 0,
        RequestPoolManager::POOL_LOCK_READ_ONLY);
    for (unsigned int i = 0; i < pPrimitive->m_pNode->m_nAuxIndexCount; i++)
    {
        int nVertex = pIndices[i];
        Vector* pPosition = (Vector*)(pVertices + pNode->m_nVertexDataOffset +
            nVertex * pNode->m_nVertexStride + pNode->m_nPositionOffset);
        Vector* pNormal = (Vector*)(pVertices + pNode->m_nVertexDataOffset +
            nVertex * pNode->m_nVertexStride + pNode->m_nNormalOffset);
#ifdef _XBOX
        coord.m_fX = pPosition->m_fX;
        coord.m_fY = pPosition->m_fY;
        coord.m_fZ = pPosition->m_fZ;
        normal.m_fX = pNormal->m_fX;
        normal.m_fY = pNormal->m_fY;
        normal.m_fZ = pNormal->m_fZ;
#else
        coord = *pPosition;
        normal = *pNormal;
#endif
        newcoord.m_fX = newcoord.m_fY = newcoord.m_fZ = 0.0f;
        newnormal.m_fX = newnormal.m_fY = newnormal.m_fZ = 0.0f;
        for (int k = 0; k < 4; k++)
        {
            float fWeight = ((float*)(pVertices +
                pNode->m_nVertexDataOffset + nVertex * pNode->m_nVertexStride +
                pNode->m_nSkinWeightOffset))[k];
            if (fWeight != 0.0f)
            {
                int nBone = pNode->m_anBoneRemap[((unsigned short*)(pVertices +
                    pNode->m_nVertexDataOffset + nVertex * pNode->m_nVertexStride +
                    pNode->m_nBoneIndexOffset))[k]];
                newcoord += fWeight * (rbones[nBone] * coord + tbones[nBone]);
                newnormal += fWeight * (rbones[nBone] * normal);
            }
        }
#ifndef _XBOX
        if (bBumpedOut)
        {
            newcoord += newnormal * pPrimitive->m_pPart->m_pAurObject->m_fBumpedOutShellHeight;
        }
#endif
        pCoords[nVertex].m_fX = newcoord.m_fX;
        pCoords[nVertex].m_fY = newcoord.m_fY;
        pCoords[nVertex].m_fZ = newcoord.m_fZ;
        pNormals[nVertex].m_fX = newnormal.m_fX;
        pNormals[nVertex].m_fY = newnormal.m_fY;
        pNormals[nVertex].m_fZ = newnormal.m_fZ;
    }
    GLRender::UnlockPool(pPrimitive->m_pNode->GetAuxIndexPool());
    GLRender::UnlockPool(vertexToken);
    GLRender::UnlockPool(normalToken);
    nPool = pNode->m_pModel->AsModel()->m_nVertexPool;
    GLRender::UnlockPool(nPool);

    GLRender::ActivateTextureStage(0);
    if (pNode->m_nTextureCount > 0)
        GLRender_EnableClientState(CLIENT_STATE_TEXTURE_COORD);
    else
        GLRender_DisableClientState(CLIENT_STATE_TEXTURE_COORD);
    int nCount;
    switch (pNode->m_nTextureCount)
    {
    case 4:
        GLRender::ActivateTextureStage(3);
        nCount = pNode->m_nVertexCount;
        nPool = pNode->m_pModel->AsModel()->m_nVertexPool;
        GLRender::SetTexCoordBuffer(pNode->m_nVertexStride,
            pNode->m_nVertexDataOffset + pNode->m_nTexCoord3Offset, nPool, nCount);
    case 3:
        GLRender::ActivateTextureStage(2);
        nCount = pNode->m_nVertexCount;
        nPool = pNode->m_pModel->AsModel()->m_nVertexPool;
        GLRender::SetTexCoordBuffer(pNode->m_nVertexStride,
            pNode->m_nVertexDataOffset + pNode->m_nTexCoord2Offset, nPool, nCount);
    case 2:
        GLRender::ActivateTextureStage(1);
        nCount = pNode->m_nVertexCount;
        nPool = pNode->m_pModel->AsModel()->m_nVertexPool;
        GLRender::SetTexCoordBuffer(pNode->m_nVertexStride,
            pNode->m_nVertexDataOffset + pNode->m_nTexCoord1Offset, nPool, nCount);
    case 1:
        GLRender::ActivateTextureStage(0);
        nCount = pNode->m_nVertexCount;
        nPool = pNode->m_pModel->AsModel()->m_nVertexPool;
        GLRender::SetTexCoordBuffer(pNode->m_nVertexStride,
            pNode->m_nVertexDataOffset + pNode->m_nTexCoord0Offset, nPool, nCount);
    }
    GLRender::SetNormalBuffer(12, 0, normalToken, pNode->m_nVertexCount);
    GLRender::SetVertexBuffer(12, 0, vertexToken, pNode->m_nVertexCount);
    PartSkin* pPart = pPrimitive->m_pPart->AsPartSkin();
#ifdef _XBOX
    if ((pPart->m_pAurObject && pPart->m_pAurObject->m_pEnvironmentMap.m_pObject) ||
        pPart->m_pMaterial->HasEnvMap())
    {
        RenderEnvironmentMapped(pPrimitive, false);
    }
    else
    {
        RenderFlat(pPrimitive);
    }
#else
    if (bBumpedOut)
    {
        GLRender::SetActiveTexture(0);
        ((CAurTexture*)pPart->m_pMaterial->m_pTexture3.m_pObject)->Bind();
        if (pPart->m_pMaterial->m_lstBlendSrc.m_nCount > 1)
            pPart->m_pMaterial->SetBlendingMode(1);
        RenderFlat(pPrimitive);
        pPart->m_pMaterial->BindTexture0();
        GLRender_SetBlendFunc(BLEND_SRC_ALPHA,
            BLEND_ONE_MINUS_SRC_ALPHA);
    }
    else if ((pPart->m_pAurObject && pPart->m_pAurObject->m_pEnvironmentMap.m_pObject) ||
             pPart->m_pMaterial->HasEnvMap())
    {
        RenderEnvironmentMapped(pPrimitive, false, false);
    }
    else if (pPart->m_pMaterial->HasBumpMap() && pNode->HasTangentBases())
    {
        if (AurNV1xLevelShadersAvailable())
        {
            GLRender_EnablePixelShaders(MODEL_SHADER_MODULATE, 0, 0, 0, 0);
            RenderFlat(pPrimitive);
            GLRender_EnablePixelShaders(MODEL_SHADER_DISABLE, 0, 0, 0, 0);
        }
        else
        {
            GLRender_Disable(ENABLE_ALPHA_TEST);
            GLRender_SetBlendFunc(BLEND_ONE,
                BLEND_ZERO);
            RenderFlat(pPrimitive);
            GLRender_SetBlendFunc(BLEND_SRC_ALPHA,
            BLEND_ONE_MINUS_SRC_ALPHA);
            GLRender_Enable(ENABLE_ALPHA_TEST);
        }
    }
    else
    {
        if (!AurNV1xLevelShadersAvailable() && !AurNV2xLevelShadersAvailable() &&
            AurMultiTextureAvailable() && AurVertexProgramsNVAvailable())
        {
            GLRender::SetActiveTexture(1);
            GLRender_Enable(ENABLE_TEXTURE_2D);
            GLRender::BindTexture(ENABLE_TEXTURE_2D, 0);
            GLRender::SetActiveTexture(0);
        }
        RenderFlat(pPrimitive);
    }
#endif
}
// PC 0x004A1590 GOG 0x004A1650
void MakeInlinePartList(Part* pPart, List<Part*>& lstParts)
{
    lstParts.Add(pPart);

    for (int i = 0; i < pPart->m_lstChildren.m_nCount; i++)
    {
        MakeInlinePartList(pPart->m_lstChildren[i], lstParts);
    }
}
// PC 0x004A1870 GOG 0x004A1930
int FindPartIndexDFS(Part* pPart, char* sPartName)
{
    List<Part*> lstParts;
    int i;

    if (stricmp("root", sPartName) == 0)
    {
        return 0;
    }

    MakeInlinePartList(pPart, lstParts);

    for (i = 0; i < lstParts.m_nCount; i++)
    {
        if (stricmp(lstParts.m_pList[i]->Name(), sPartName) == 0)
        {
            return i;
        }
    }

    return -1;
}
// PC 0x004A1980 GOG 0x004A1A40
void MdlNodeSkin::InternalGenVertices()
{
    List<SkinVertexWeight> lstSkinWeights;
    int nFace;
    int nCorner;
    int nVertex;

    for (nFace = 0; nFace < m_lFaces.m_nCount; nFace++)
    {
        for (nCorner = 0; nCorner < 3; nCorner++)
        {
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

            NodeVertex* pVertex = new NodeVertex(pPosition, pTexCoord0, pTexCoord1,
                                               pTexCoord2, pTexCoord3, pNormal, 0);
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
                lstSkinWeights.Add(m_lstSkinWeights[
                    m_pPostProcess->m_lFaces[nFace].m_pFaces[nCorner]]);
            }
        }
    }

    m_lstSkinWeights = lstSkinWeights;
}
// PC 0x004A1CD0 GOG 0x004A1D90
void GatherBones(Part* pPart, List<Vector>& tbones, List<Quaternion>& qbones,
                 Vector vTranslation, Quaternion qRotation)
{
    vTranslation = vTranslation + qRotation * pPart->m_vPosition;
    qRotation = qRotation * pPart->m_qOrientation;
    tbones.Add(vTranslation);
    qbones.Add(qRotation);

    for (int i = 0; i < pPart->m_lstChildren.m_nCount; i++)
    {
        GatherBones(pPart->m_lstChildren.m_pList[i], tbones, qbones,
                    vTranslation, qRotation);
    }
}
// PC 0x004A31D0 GOG 0x004A3290
void PartSkin::PostProcess(Part* pRoot)
{
    MdlNodeSkin* pNode = (MdlNodeSkin*)m_pNode;
    int bTangentSpace = 0;
    List<Part*> lstParts;

    if (pNode->m_nVertexCount == 0)
    {
        {
            List<Quaternion> qbones;
            List<Vector> tbones;

            if (pNode->m_lstTBoneRefInv.m_nCount != 0)
            {
                return;
            }

            Part* pPart = this;
            Vector vTranslation = -pPart->m_qOrientation * -pPart->m_vPosition;
            Quaternion qRotation = -pPart->m_qOrientation;
            while (pPart->m_pParent != 0)
            {
                pPart = pPart->m_pParent;
                vTranslation = vTranslation + qRotation *
                    (-pPart->m_qOrientation * -pPart->m_vPosition);
                qRotation = qRotation * -pPart->m_qOrientation;
            }
            GatherBones(pPart, tbones, qbones, vTranslation, qRotation);

            int k;
            for (k = 0; k < tbones.m_nCount; k++)
            {
                pNode->m_lstTBoneRefInv.Add(-qbones[k] * -tbones[k]);
                pNode->m_lstQBoneRefInv.Add(-qbones[k]);
            }

            for (k = 0; k < pNode->m_lstSkinWeights.m_nCount; k++)
            {
                if (pNode->m_lstSkinWeights[k].m_anBoneIndices[0] == -1)
                {
                    pNode->m_lstSkinWeights[k].m_anBoneIndices[0] =
                        FindPartIndexDFS(pPart, pNode->m_lstSkinWeights[k].m_apBones[0]);
                    pNode->m_lstSkinWeights[k].m_anBoneIndices[1] =
                        FindPartIndexDFS(pPart, pNode->m_lstSkinWeights[k].m_apBones[1]);
                    pNode->m_lstSkinWeights[k].m_anBoneIndices[2] =
                        FindPartIndexDFS(pPart, pNode->m_lstSkinWeights[k].m_apBones[2]);
                    pNode->m_lstSkinWeights[k].m_anBoneIndices[3] =
                        FindPartIndexDFS(pPart, pNode->m_lstSkinWeights[k].m_apBones[3]);
                }
                delete[] pNode->m_lstSkinWeights[k].m_apBones[0];
                delete[] pNode->m_lstSkinWeights[k].m_apBones[1];
                delete[] pNode->m_lstSkinWeights[k].m_apBones[2];
                delete[] pNode->m_lstSkinWeights[k].m_apBones[3];
                pNode->m_lstSkinWeights[k].m_apBones[0] = 0;
                pNode->m_lstSkinWeights[k].m_apBones[1] = 0;
                pNode->m_lstSkinWeights[k].m_apBones[2] = 0;
                pNode->m_lstSkinWeights[k].m_apBones[3] = 0;
            }

            pNode->m_pfnGenVertices(pNode);
            pNode->m_nVertexCount =
                (short)pNode->m_pPostProcess->m_lstVertices.m_nCount;
            pNode->BuildVertexIndices();
            pNode->BuildNeighborLists();

            Material* pMaterial = 0;
            NodeVertex* pVertex = 0;
            if (pNode->m_nVertexCount > 0)
            {
                pVertex = pNode->m_pPostProcess->m_lstVertices[0];
            }

#ifdef _XBOX

            pNode->m_nPositionOffset = 0;
            pNode->m_nVertexArrayFlags |= MdlNodeTriMesh::VERTEX_ARRAY_POSITION |
                                          MdlNodeTriMesh::VERTEX_ARRAY_NORMAL;
            int nOffset = 16;
            pNode->m_nNormalOffset = 12;
            pNode->m_nVertexStride = 16;
#else
            pNode->m_nVertexArrayFlags |= MdlNodeTriMesh::VERTEX_ARRAY_POSITION |
                                          MdlNodeTriMesh::VERTEX_ARRAY_NORMAL;
            int nOffset = 24;
            pNode->m_nPositionOffset = 0;
            pNode->m_nNormalOffset = 12;
            pNode->m_nVertexStride = 24;
#endif
            if (pVertex->m_nFlags & NodeVertex::VERTEX_HAS_TEXCOORD0)
            {
                pNode->m_nTextureCount++;
                pNode->m_nVertexArrayFlags |= MdlNodeTriMesh::VERTEX_ARRAY_TEXCOORD0;
                pNode->m_nTexCoord0Offset = nOffset;
                nOffset += 8;
                pNode->m_nVertexStride = nOffset;
            }
            if (pVertex->m_nFlags & NodeVertex::VERTEX_HAS_TEXCOORD1)
            {
                pNode->m_nTextureCount++;
                pNode->m_nVertexArrayFlags |= MdlNodeTriMesh::VERTEX_ARRAY_TEXCOORD1;
                pNode->m_nTexCoord1Offset = nOffset;
                nOffset += 8;
                pNode->m_nVertexStride = nOffset;
            }
            if (pVertex->m_nFlags & NodeVertex::VERTEX_HAS_TEXCOORD2)
            {
                pNode->m_nTextureCount++;
                pNode->m_nVertexArrayFlags |= MdlNodeTriMesh::VERTEX_ARRAY_TEXCOORD2;
                pNode->m_nTexCoord2Offset = nOffset;
                nOffset += 8;
                pNode->m_nVertexStride = nOffset;
            }
            if (pVertex->m_nFlags & NodeVertex::VERTEX_HAS_TEXCOORD3)
            {
                pNode->m_nTextureCount++;
                pNode->m_nVertexArrayFlags |= MdlNodeTriMesh::VERTEX_ARRAY_TEXCOORD3;
                pNode->m_nTexCoord3Offset = nOffset;
                nOffset += 8;
                pNode->m_nVertexStride = nOffset;
            }

            pMaterial = new Material;
            pMaterial->InitializeTextures(pNode->m_sTexture0, 0, 0, 0);
            if (pMaterial->HasBumpMap())
            {
                pNode->m_TangentBases = nOffset;
                bTangentSpace = 1;
                pNode->m_nVertexArrayFlags |= MdlNodeTriMesh::VERTEX_ARRAY_TANGENT_BASIS;
#ifdef _XBOX
                pNode->m_nVertexStride += 12;
                nOffset += 12;
#else
                pNode->m_nVertexStride += 36;
                nOffset += 36;
#endif
            }
            delete pMaterial;

            pNode->m_nSkinWeightOffset = nOffset;
#ifdef _XBOX
            pNode->m_nVertexStride += 24;
#else
            pNode->m_nVertexStride += 32;
#endif
            nOffset += 16;
            m_vPosition = m_vPosition;
            pNode->m_nBoneIndexOffset = nOffset;

            Model* pModel = pNode->m_pModel->AsModel();
            int nPadding = pModel->m_nVertexDataSize & 15;
            int nSize = pNode->m_pModel->AsModel()->m_nVertexDataSize +
                (pNode->m_nVertexCount + 1) * pNode->m_nVertexStride + nPadding;
            unsigned long nPool = pNode->m_pModel->AsModel()->m_nVertexPool;
            GLRender::ResizePool(nPool, nSize);
            nPool = pNode->m_pModel->AsModel()->m_nVertexPool;
            char* pBuffer = (char*)GLRender::LockPool(nPool, 0, 0,
                RequestPoolManager::POOL_LOCK_WRITE);
            int nStart = pNode->m_pModel->AsModel()->m_nVertexDataSize + nPadding;
            m_qOrientation = m_qOrientation;
            m_vMinimum = m_vMinimum;
            m_vMaximum = m_vMaximum;
            pNode->m_nVertexDataOffset = nStart;
            pNode->m_pModel->AsModel()->m_nVertexDataSize +=
                (pNode->m_nVertexCount + 1) * pNode->m_nVertexStride + nPadding;
#ifndef _XBOX
            pNode->m_pVertexPositions =
                (Vector*)new float[(pNode->m_nVertexCount + 1) * 3];
#endif

            int nLastVertex = 0;
            int i;
            for (i = 0; i < pNode->m_nVertexCount; i++)
            {
                pVertex = pNode->m_pPostProcess->m_lstVertices[i];
                if (pNode->m_nTexCoord0Offset != MdlNodeTriMesh::NO_VERTEX_ARRAY_OFFSET)
                {
                    float* pTexCoord = (float*)(pBuffer + nStart +
                        pNode->m_nTexCoord0Offset + i * pNode->m_nVertexStride);
                    pTexCoord[0] = pVertex->m_vTexCoord0.m_fX;
                    pTexCoord[1] = pVertex->m_vTexCoord0.m_fY;
                }
                if (pNode->m_nTexCoord1Offset != MdlNodeTriMesh::NO_VERTEX_ARRAY_OFFSET)
                {
                    float* pTexCoord = (float*)(pBuffer + nStart +
                        pNode->m_nTexCoord1Offset + i * pNode->m_nVertexStride);
                    pTexCoord[0] = pVertex->m_vTexCoord1.m_fX;
                    pTexCoord[1] = pVertex->m_vTexCoord1.m_fY;
                }
                if (pNode->m_nTexCoord2Offset != MdlNodeTriMesh::NO_VERTEX_ARRAY_OFFSET)
                {
                    float* pTexCoord = (float*)(pBuffer + nStart +
                        pNode->m_nTexCoord2Offset + i * pNode->m_nVertexStride);
                    pTexCoord[0] = pVertex->m_vTexCoord2.m_fX;
                    pTexCoord[1] = pVertex->m_vTexCoord2.m_fY;
                }
                if (pNode->m_nTexCoord3Offset != MdlNodeTriMesh::NO_VERTEX_ARRAY_OFFSET)
                {
                    float* pTexCoord = (float*)(pBuffer + nStart +
                        pNode->m_nTexCoord3Offset + i * pNode->m_nVertexStride);
                    pTexCoord[0] = pVertex->m_vTexCoord3.m_fX;
                    pTexCoord[1] = pVertex->m_vTexCoord3.m_fY;
                }
                float* pPosition = (float*)(pBuffer + nStart +
                    pNode->m_nPositionOffset + i * pNode->m_nVertexStride);
                pPosition[0] = pVertex->m_vPosition.m_fX;
                pPosition[1] = pVertex->m_vPosition.m_fY;
                pPosition[2] = pVertex->m_vPosition.m_fZ;
#ifndef _XBOX
                pNode->m_pVertexPositions[i] = pVertex->m_vPosition;
#endif
#ifdef _XBOX
                unsigned long* pNormal = (unsigned long*)(pBuffer + nStart +
                    pNode->m_nNormalOffset + i * pNode->m_nVertexStride);
                *pNormal = ((((unsigned long)(pVertex->m_vNormal.m_fY * 1023.0f) & 0x7ff) |
                    ((unsigned long)(pVertex->m_vNormal.m_fZ * 511.0f) << 11)) << 11) |
                    ((unsigned long)(pVertex->m_vNormal.m_fX * 1023.0f) & 0x7ff);
#else
                float* pNormal = (float*)(pBuffer + nStart +
                    pNode->m_nNormalOffset + i * pNode->m_nVertexStride);
                pNormal[0] = pVertex->m_vNormal.m_fX;
                pNormal[1] = pVertex->m_vNormal.m_fY;
                pNormal[2] = pVertex->m_vNormal.m_fZ;
#endif
            }

            memset((void*)(pBuffer + nStart + pNode->m_nPositionOffset +
                pNode->m_nVertexCount * pNode->m_nVertexStride), 0,
                pNode->m_nVertexStride);
            *(float*)(pBuffer + nStart + pNode->m_nPositionOffset +
                pNode->m_nVertexCount * pNode->m_nVertexStride) = 1000000.0f;
            *(float*)(pBuffer + nStart + pNode->m_nPositionOffset +
                pNode->m_nVertexCount * pNode->m_nVertexStride + 4) = 1000000.0f;
            *(float*)(pBuffer + nStart + pNode->m_nPositionOffset +
                pNode->m_nVertexCount * pNode->m_nVertexStride + 8) = 1000000.0f;

            pPart = this;
            while (pPart->m_pParent != 0)
            {
                pPart = pPart->m_pParent;
            }
            MakeInlinePartList(pPart, lstParts);

            pNode->m_lstBoneConstantIndices.SetSize(lstParts.m_nCount);
            pNode->m_nBoneMap = lstParts.m_nCount;
#ifdef _XBOX
            pNode->m_pfBoneMap = new short[lstParts.m_nCount];
            for (i = 0; i < lstParts.m_nCount; i++)
            {
                pNode->m_pfBoneMap[i] = -1;
            }
#else
            pNode->m_pfBoneMap = new float[lstParts.m_nCount];
            for (i = 0; i < lstParts.m_nCount; i++)
            {
                pNode->m_pfBoneMap[i] = -1.0f;
            }
#endif

            int nNextBone = 0;
            float fWeight;
            int nBone;
            for (i = 0; i < pNode->m_nVertexCount; i++)
            {
                SkinVertexWeight* pWeight = &pNode->m_lstSkinWeights[i];
                nLastVertex = i;
                unsigned short nSlot = 0;
                for (int j = 0; j < 4; nSlot++, j++)
                {
                    const unsigned long& nDataOffset = pNode->m_nVertexDataOffset;
#ifdef _XBOX
                    ((short*)(pBuffer + (nDataOffset + i * pNode->m_nVertexStride) +
                        pNode->m_nBoneIndexOffset))[nSlot] = -1;
#else
                    ((float*)(pBuffer + (nDataOffset + i * pNode->m_nVertexStride) +
                        pNode->m_nBoneIndexOffset))[nSlot] = -1.0f;
#endif
                    ((float*)(pBuffer + (nDataOffset + i * pNode->m_nVertexStride) +
                        pNode->m_nSkinWeightOffset))[nSlot] = -1.0f;
                    if (j == 0)
                        fWeight = pWeight->m_afWeights[0];
                    else if (j == 1)
                        fWeight = pWeight->m_afWeights[1];
                    else if (j == 2)
                        fWeight = pWeight->m_afWeights[2];
                    else if (j == 3)
                        fWeight = pWeight->m_afWeights[3];
                    ((float*)(pBuffer + (nDataOffset + i * pNode->m_nVertexStride) +
                        pNode->m_nSkinWeightOffset))[nSlot] = fWeight;
                    if (fWeight != 0.0f)
                    {
                        if (j == 0)
                            nBone = pWeight->m_anBoneIndices[0];
                        else if (j == 1)
                            nBone = pWeight->m_anBoneIndices[1];
                        else if (j == 2)
                            nBone = pWeight->m_anBoneIndices[2];
                        else if (j == 3)
                            nBone = pWeight->m_anBoneIndices[3];
#ifdef _XBOX
                        if (pNode->m_pfBoneMap[nBone] == -1)
                        {
                            pNode->m_pfBoneMap[nBone] = nNextBone++;
                        }
                        ((short*)(pBuffer + (nDataOffset + i * pNode->m_nVertexStride) +
                            pNode->m_nBoneIndexOffset))[nSlot] =
                            pNode->m_pfBoneMap[nBone];
                        pNode->m_anBoneRemap[((unsigned short*)(pBuffer +
                            (nDataOffset + i * pNode->m_nVertexStride) +
                            pNode->m_nBoneIndexOffset))[nSlot]] = nBone;
#else
                        if (pNode->m_pfBoneMap[nBone] < 0.0f)
                        {
                            pNode->m_pfBoneMap[nBone] = (float)nNextBone++;
                        }
                        ((float*)(pBuffer + (nDataOffset + i * pNode->m_nVertexStride) +
                            pNode->m_nBoneIndexOffset))[nSlot] =
                            pNode->m_pfBoneMap[nBone];
                        pNode->m_anBoneRemap[(int)((float*)(pBuffer +
                            (nDataOffset + i * pNode->m_nVertexStride) +
                            pNode->m_nBoneIndexOffset))[nSlot]] = nBone;
#endif
                    }
                }
            }

            const unsigned long& nVertexDataOffset = pNode->m_nVertexDataOffset;
#ifdef _XBOX
            *(short*)(pBuffer + (nVertexDataOffset + (nLastVertex + 1) * pNode->m_nVertexStride) +
                pNode->m_nBoneIndexOffset) = 0;
            *(short*)(pBuffer + (nVertexDataOffset + (nLastVertex + 1) * pNode->m_nVertexStride) +
                pNode->m_nBoneIndexOffset + 2) = 0;
            *(short*)(pBuffer + (nVertexDataOffset + (nLastVertex + 1) * pNode->m_nVertexStride) +
                pNode->m_nBoneIndexOffset + 4) = 0;
            *(short*)(pBuffer + (nVertexDataOffset + (nLastVertex + 1) * pNode->m_nVertexStride) +
                pNode->m_nBoneIndexOffset + 6) = 0;
#else
            *(float*)(pBuffer + (nVertexDataOffset + (nLastVertex + 1) * pNode->m_nVertexStride) +
                pNode->m_nBoneIndexOffset) = 0.0f;
            *(float*)(pBuffer + (nVertexDataOffset + (nLastVertex + 1) * pNode->m_nVertexStride) +
                pNode->m_nBoneIndexOffset + 4) = 0.0f;
            *(float*)(pBuffer + (nVertexDataOffset + (nLastVertex + 1) * pNode->m_nVertexStride) +
                pNode->m_nBoneIndexOffset + 8) = 0.0f;
            *(float*)(pBuffer + (nVertexDataOffset + (nLastVertex + 1) * pNode->m_nVertexStride) +
                pNode->m_nBoneIndexOffset + 12) = 0.0f;
#endif
            *(float*)(pBuffer + (nVertexDataOffset + (nLastVertex + 1) * pNode->m_nVertexStride) +
                pNode->m_nSkinWeightOffset) = 1.0f;
            *(float*)(pBuffer + (nVertexDataOffset + (nLastVertex + 1) * pNode->m_nVertexStride) +
                pNode->m_nSkinWeightOffset + 4) = 0.0f;
            *(float*)(pBuffer + (nVertexDataOffset + (nLastVertex + 1) * pNode->m_nVertexStride) +
                pNode->m_nSkinWeightOffset + 8) = 0.0f;
            *(float*)(pBuffer + (nVertexDataOffset + (nLastVertex + 1) * pNode->m_nVertexStride) +
                pNode->m_nSkinWeightOffset + 12) = 0.0f;

            pNode->m_lstSkinWeights.SetSize(0);
            nPool = pNode->m_pModel->AsModel()->m_nVertexPool;
            GLRender::UnlockPool(nPool);
        }

        if (bTangentSpace)
        {
            pNode->CalculateTangentSpaceBasis();
        }
    }

    pNode->m_pfnRemoveTemporaryArrays(pNode);
}
// PC 0x004A4680 GOG 0x004A4740
#ifdef _XBOX
void RenderSkinned(VertexPrimitiveSkinned* pPrimitive)
#else
void RenderSkinned(VertexPrimitiveSkinned* pPrimitive, bool bBumpedOut)
#endif
{
    Part* pPart = pPrimitive->m_pPart;
    MdlNodeSkin* pNode = (MdlNodeSkin*)pPart->m_pNode;

    static List<Quaternion> qbones(0);
    static List<Matrix> rbones(0);
    static List<Vector> tbones(0);

    qbones.m_nCount = 0;
    rbones.m_nCount = 0;
    tbones.m_nCount = 0;

    Part* pRoot = pPart;
    Vector vTranslation = -pPart->m_qOrientation * -pPart->m_vPosition;
    Quaternion qRotation = -pPart->m_qOrientation;
    while (pRoot->m_pParent != 0)
    {
        pRoot = pRoot->m_pParent;
        vTranslation = vTranslation + qRotation *
            (-pRoot->m_qOrientation * -pRoot->m_vPosition);
        qRotation = qRotation * -pRoot->m_qOrientation;
    }

    GatherBones(pRoot, tbones, qbones, vTranslation, qRotation);
    for (int i = 0; i < tbones.m_nCount; i++)
    {
        tbones[i] = tbones[i] + qbones[i] * pNode->m_lstTBoneRefInv[i];
        qbones[i] = qbones[i] * pNode->m_lstQBoneRefInv[i];
        rbones.Add(qbones[i].getmatrix());
    }

    static Vector coord;
    static Vector normal;
#ifdef _XBOX
    return ExecuteSkinTransforms(pNode, tbones, rbones, pPrimitive);
#else
    return ExecuteSkinTransforms(pNode, tbones, rbones, pPrimitive, bBumpedOut);
#endif
}
// PC 0x004A50A0 GOG 0x004A5160
void RenderSkinnedVP(VertexPrimitiveSkinned* pPrimitive)
{
    PartSkin* pSkin = pPrimitive->m_pPart->AsPartSkin();
    MdlNodeSkin* pNode = (MdlNodeSkin*)pSkin->m_pNode;
    if (pNode->m_nTextureCount != 0)
    {
        Gob* pObject = 0;
        PartTriMesh* pMesh = pSkin->AsPartTriMesh();
        MdlNodeTriMesh* pTriMesh = 0;
        if (pMesh != 0)
        {
            pTriMesh = (MdlNodeTriMesh*)pSkin->m_pNode;
            pObject = pMesh->m_pAurObject;
        }
        bool bHologram = pSkin->m_bHologram;
        if (bHologram && pTriMesh->m_bHideInHolograms)
            return;
        pPrimitive->SetupVertexArraysStandard();
        Matrix mRotation;
        static List<Quaternion> qbones(0);
        static List<Vector> tbones(0);

#ifndef _XBOX
        if (usearbvertexprograms)
            glEnable(GL_VERTEX_PROGRAM_ARB);
        else
#endif
            GLRender_Enable(ENABLE_VERTEX_PROGRAM);
        AurSetVertexProgramParameter4f(PartSkin::SKIN_VP_CONSTANTS,
            0.0f, 2.0f, 3.0f, 1.0f);
        AurSetVertexProgramParameter4f(PartSkin::SKIN_VP_FOG,
            currentscene->m_fFogNear, currentscene->m_fFogFar,
            currentscene->m_bFog ? 1.0f : 0.0f, 0.5f);

        Part* pPart = pSkin;
        char bRendered = pPrimitive->m_pPart->m_pAurObject->m_bRendered;
        if (bRendered)
        {
            while (pPart->m_pParent != 0)
                pPart = pPart->m_pParent;
            qbones.m_nCount = 0;
            tbones.m_nCount = 0;
            GatherBones(pPart, tbones, qbones, Vector(), Quaternion());
            Gob* pGob = pPrimitive->m_pPart->m_pAurObject;
            pGob->m_bRendered = 0;
        }

        pPart = pSkin;
        Vector vTranslation = -pPart->m_qOrientation * -pPart->m_vPosition;
        Quaternion qRotation = -pPart->m_qOrientation;
        while (pPart->m_pParent != 0)
        {
            pPart = pPart->m_pParent;
            vTranslation = vTranslation + qRotation *
                (-pPart->m_qOrientation * -pPart->m_vPosition);
            qRotation = qRotation * -pPart->m_qOrientation;
        }
        for (int i = 0; i < pNode->m_nBoneMap; i++)
        {
            int nBone = (int)pNode->m_pfBoneMap[i];
            if (nBone == -1)
                continue;
            {
                Vector vBone = vTranslation + qRotation * tbones[i] + qRotation *
                    (qbones[i] * pNode->m_lstTBoneRefInv[i]);
                Quaternion qBone = qRotation * qbones[i] * pNode->m_lstQBoneRefInv[i];
                mRotation = qBone.getmatrix();
                int nRegister = PartSkin::SKIN_VP_BONES + 3 * nBone;
                const Vector& vFirst = mRotation.m_vFirst;
                const Vector& vSecond = mRotation.m_vSecond;
                const Vector& vThird = mRotation.m_vThird;
                GLRender_SetVertexProgramParameter4f(nRegister,
                    vFirst.m_fX, vSecond.m_fX,
                    vThird.m_fX, vBone.m_fX);
                GLRender_SetVertexProgramParameter4f(++nRegister,
                    vFirst.m_fY, vSecond.m_fY,
                    vThird.m_fY, vBone.m_fY);
                GLRender_SetVertexProgramParameter4f(nRegister + 1,
                    vFirst.m_fZ, vSecond.m_fZ,
                    vThird.m_fZ, vBone.m_fZ);
            }
        }

#ifdef _XBOX

        if (enablebumpyshiny && pSkin->m_pMaterial->HasBumpMap() &&
            pSkin->m_pMaterial->HasBumpyShinyMap() && pTriMesh->HasTangentBases())
        {
            GLRender_Enable(ENABLE_BLEND);
            GLRender_Disable(ENABLE_ALPHA_TEST);
            if (bumpyskinpass)
            {
                GLRender_SetBlendFunc(BLEND_ONE, BLEND_ZERO);
                GLRender_EnableClientState(CLIENT_STATE_TANGENT_BASIS0);
                GLRender_EnableClientState(CLIENT_STATE_TANGENT_BASIS1);
                GLRender_EnableClientState(CLIENT_STATE_TANGENT_BASIS2);
                GLRender_DisableClientState(CLIENT_STATE_NORMAL);
                AurSetVertexProgramParameter4f(LightManager::LIGHT_PROGRAM_COLOR_SCALE,
                    1.0f, 0.0f, 1.0f, 0.0f);
                g_pBumpyShinySkinPass1->Enable(pSkin, false);
                GLRender_EnablePixelShaders(MODEL_SHADER_TEXTURE_SHADER_BUMP,
                    pSkin->m_pMaterial->m_pBumpMap, 0, 0,
                    pSkin->m_pMaterial->m_pEnvMap);
                GLRender_EnablePixelShaders(MODEL_SHADER_ENVIRONMENT_ONLY,
                    0, 0, 0, 0);
                RenderFlat(pPrimitive);
                GLRender_EnablePixelShaders(MODEL_SHADER_DISABLE, 0, 0, 0, 0);
                GLRender_EnablePixelShaders(MODEL_SHADER_TEXTURE_SHADER_DISABLE,
                    0, 0, 0, 0);
                GLRender_EnablePixelShaders(MODEL_SHADER_BUMP_DISABLE, 0, 0, 0, 0);
                GLRender_DisableClientState(CLIENT_STATE_TANGENT_BASIS0);
                GLRender_DisableClientState(CLIENT_STATE_TANGENT_BASIS1);
                GLRender_DisableClientState(CLIENT_STATE_TANGENT_BASIS2);
                GLRender_EnableClientState(CLIENT_STATE_NORMAL);
            }
            AurSetVertexProgramParameter4f(LightManager::LIGHT_PROGRAM_COLOR_SCALE,
                1.0f, 1.0f, 1.0f, 1.0f);
            if (use3boneskinning)
                g_pBumpyShinySkinPass2_3bone->Enable(pSkin, true);
            else
                g_pBumpyShinySkinPass2->Enable(pSkin, true);
            if (enabledecal)
            {
                GLRender::ActivateTextureStage(0);
                pSkin->m_pMaterial->BindTexture0();
                GLRender_SetBlendFunc(BLEND_ONE, BLEND_ONE_MINUS_SRC_ALPHA);
                RenderFlat(pPrimitive);
            }
            GLRender_Enable(ENABLE_ALPHA_TEST);
        }
        else if (pSkin->m_pMaterial->HasBumpMap() && pTriMesh->HasTangentBases() &&
            usenewbumpyskin)
        {
            GLRender_Disable(ENABLE_ALPHA_TEST);
            static VertexProgram* skinvp = new VertexProgram(
                cNewSkinBumpyVP, cSkinBumpyVPDeclaration);
            Vector vDiffuse;
            Vector vSpecular;
            if (pTriMesh != 0)
            {
                vSpecular = pSkin->m_pMaterial->GetBumpMapSpecularColor();
                vDiffuse = pTriMesh->m_vDiffuse;
            }
            List<PartLight*> lstLights(*currentscene->m_pLightManager->GetBumpMapLights());
            GLRender_EnableClientState(CLIENT_STATE_TANGENT_BASIS0);
            GLRender_EnableClientState(CLIENT_STATE_TANGENT_BASIS1);
            GLRender_EnableClientState(CLIENT_STATE_TANGENT_BASIS2);
            GLRender_EnablePixelShaders(MODEL_SHADER_BUMP,
                pSkin->m_pMaterial->m_pBumpMap,
                currentscene->m_pNormalizationCubeMap,
                currentscene->m_pNormalizationCubeMap,
                pSkin->m_pMaterial->m_pTexture0);
            Vector& vAmbient = currentscene->m_vAmbient;
            GLRender_SetPixelShaderAmbientColor(vAmbient, 1.0f);
            Quaternion qOrientation;
            Vector vPosition;
            pSkin->GetWorld(vPosition, qOrientation);
            Vector vEye = -qOrientation * (CurrentCamera->m_vPosition - vPosition);
            GLRender_Enable(ENABLE_VERTEX_PROGRAM);
            GLRender_SetBlendFunc(BLEND_ONE, BLEND_ZERO);
            currentscene->m_pLightManager->DisableBumpMapLights();
            for (int nLight = 0; nLight < lstLights.m_nCount; nLight++)
            {
                skinvp->Enable(pSkin, false);
                PartLight* pLight = lstLights[nLight];
                Vector vLight = lstLights[nLight]->m_vWorldPosition;
                vLight = -qOrientation * (vLight - vPosition);
                Vector vLightColor = cmul(pLight->m_vColor, vDiffuse * diffuseintensity);
                if (usebumpdiffuse)
                    GLRender_SetBumpLightColor(vLightColor, 1.0f);
                else
                {
                    Vector vBlack;
                    GLRender_SetBumpLightColor(vBlack, 1.0f);
                }
                vLightColor = cmul(lstLights[nLight]->m_vColor,
                    (vSpecular * specularintensity) *
                    pSkin->m_pMaterial->GetBumpMapIntensity());
                if (usebumpspecular)
                    GLRender_SetPixelShaderAmbientColor(vLightColor, 1.0f);
                else
                {
                    Vector vBlack;
                    GLRender_SetPixelShaderAmbientColor(vBlack, 1.0f);
                }
                AurSetVertexProgramParameter4f(VertexProgram::PARAM_NORMAL_MAP_LIGHT_POSITION,
                    vLight.m_fX, vLight.m_fY, vLight.m_fZ, 1.0f);
                AurSetVertexProgramParameter4f(VertexProgram::PARAM_NORMAL_MAP_EYE_POSITION,
                    vEye.m_fX, vEye.m_fY, vEye.m_fZ, 1.0f);
                float fRadius = lstLights[nLight]->m_fRadius;
                AurSetVertexProgramParameter4f(VertexProgram::PARAM_NORMAL_MAP_ATTENUATION,
                    1.0f, 0.0f, AUR_LIGHT_ATTENUATION_SCALE / (fRadius * fRadius + fRadius * fRadius), 1.0f);
                RenderFlat(pPrimitive);
            }
            if (lstLights.m_nCount == 0)
            {
                skinvp->Enable(pSkin, false);
                GLRender_SetBumpLightColor(Vector(), 1.0f);
                GLRender_SetPixelShaderAmbientColor(Vector(), 1.0f);
                RenderFlat(pPrimitive);
            }
            GLRender_DisableClientState(CLIENT_STATE_TANGENT_BASIS0);
            GLRender_DisableClientState(CLIENT_STATE_TANGENT_BASIS1);
            GLRender_DisableClientState(CLIENT_STATE_TANGENT_BASIS2);
            currentscene->DoFog();
            GLRender_SetBlendFunc(BLEND_ONE, BLEND_ONE);
            GLRender_EnablePixelShaders(MODEL_SHADER_BUMP_DISABLE, 0, 0, 0, 0);
            GLRender_EnablePixelShaders(MODEL_SHADER_DISABLE, 0, 0, 0, 0);
            AurSetVertexProgramParameter4f(LightManager::LIGHT_PROGRAM_COLOR_SCALE,
                1.0f, 1.0f, 1.0f, 1.0f);
            static VertexProgram* skinDecalPassvp = new VertexProgram(
                cSkinVP, cSkinBumpyVPDeclaration);
            skinDecalPassvp->Enable(pSkin, true);
            if (enabledecal)
            {
                GLRender::ActivateTextureStage(0);
                pSkin->m_pMaterial->BindTexture0();
                RenderFlat(pPrimitive);
            }
            GLRender_Enable(ENABLE_ALPHA_TEST);
            currentscene->m_pLightManager->EnableBumpMapLights();
            GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
        }
        else if (pSkin->m_pMaterial->HasBumpMap() && pTriMesh->HasTangentBases())
        {
            static VertexProgram* skinvp = new VertexProgram(
                usearbvertexprograms ? szSkinBumpyVPARB : cSkinBumpyVP,
                cSkinBumpyVPDeclaration);
            Vector vDiffuse;
            Vector vSpecular;
            if (pTriMesh != 0)
            {
                vSpecular = pSkin->m_pMaterial->GetBumpMapSpecularColor();
                vDiffuse = pTriMesh->m_vDiffuse;
            }
            List<PartLight*> lstLights(*currentscene->m_pLightManager->GetBumpMapLights());
            GLRender_EnableClientState(CLIENT_STATE_TANGENT_BASIS0);
            GLRender_EnableClientState(CLIENT_STATE_TANGENT_BASIS1);
            GLRender_EnableClientState(CLIENT_STATE_TANGENT_BASIS2);
            GLRender_EnablePixelShaders(MODEL_SHADER_BUMP,
                pSkin->m_pMaterial->m_pBumpMap,
                currentscene->m_pNormalizationCubeMap,
                currentscene->m_pNormalizationCubeMap,
                pSkin->m_pMaterial->m_pTexture0);
            Vector& vAmbient = currentscene->m_vAmbient;
            GLRender_SetPixelShaderAmbientColor(vAmbient, 1.0f);
            Quaternion qOrientation;
            Vector vPosition;
            pSkin->GetWorld(vPosition, qOrientation);
            Vector vEye = -qOrientation * (CurrentCamera->m_vPosition - vPosition);
            GLRender_Enable(ENABLE_VERTEX_PROGRAM);
            GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
            for (int nLight = 0; nLight < lstLights.m_nCount; nLight++)
            {
                currentscene->m_pLightManager->DisableBumpMapLights();
                skinvp->Enable(pSkin, true);
                PartLight* pLight = lstLights[nLight];
                Vector vLight = lstLights[nLight]->m_vWorldPosition;
                vLight = -qOrientation * (vLight - vPosition);
                Vector vLightColor = cmul(pLight->m_vColor, diffuseintensity * vDiffuse);
                if (usebumpdiffuse)
                    GLRender_SetBumpLightColor(vLightColor, 1.0f);
                else
                {
                    Vector vBlack;
                    GLRender_SetBumpLightColor(vBlack, 1.0f);
                }
                vLightColor = cmul(lstLights[nLight]->m_vColor,
                    (specularintensity * vSpecular) *
                    pSkin->m_pMaterial->GetBumpMapIntensity());
                if (usebumpspecular)
                    GLRender_SetPixelShaderAmbientColor(vLightColor, 1.0f);
                else
                {
                    Vector vBlack;
                    GLRender_SetPixelShaderAmbientColor(vBlack, 1.0f);
                }
                AurSetVertexProgramParameter4f(VertexProgram::PARAM_NORMAL_MAP_LIGHT_POSITION,
                    vLight.m_fX, vLight.m_fY, vLight.m_fZ, 1.0f);
                AurSetVertexProgramParameter4f(VertexProgram::PARAM_NORMAL_MAP_EYE_POSITION,
                    vEye.m_fX, vEye.m_fY, vEye.m_fZ, 1.0f);
                float fRadius = lstLights[nLight]->m_fRadius;
                AurSetVertexProgramParameter4f(VertexProgram::PARAM_NORMAL_MAP_ATTENUATION,
                    1.0f, 0.0f, AUR_LIGHT_ATTENUATION_SCALE / (fRadius * fRadius + fRadius * fRadius), 1.0f);
                RenderFlat(pPrimitive);
                currentscene->m_pLightManager->EnableBumpMapLights();
            }
            if (lstLights.m_nCount == 0)
            {
                skinvp->Enable(pSkin, true);
                GLRender_SetBumpLightColor(Vector(), 1.0f);
                GLRender_SetPixelShaderAmbientColor(Vector(), 1.0f);
                RenderFlat(pPrimitive);
            }
            GLRender_DisableClientState(CLIENT_STATE_TANGENT_BASIS0);
            GLRender_DisableClientState(CLIENT_STATE_TANGENT_BASIS1);
            GLRender_DisableClientState(CLIENT_STATE_TANGENT_BASIS2);
            currentscene->DoFog();
            GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
            GLRender_EnablePixelShaders(MODEL_SHADER_BUMP_DISABLE, 0, 0, 0, 0);
            GLRender_EnablePixelShaders(MODEL_SHADER_DISABLE, 0, 0, 0, 0);
        }
        else
        {
            if ((pSkin->m_pAurObject != 0 && pSkin->m_pAurObject->m_pEnvironmentMap.m_pObject != 0) ||
                pSkin->m_pMaterial->HasEnvMap())
            {
                CAurTexture* pEnvMap;
                if (pSkin->m_pAurObject != 0 && pSkin->m_pAurObject->m_pEnvironmentMap.m_pObject != 0)
                    pEnvMap = pSkin->m_pAurObject->m_pEnvironmentMap;
                else
                    pEnvMap = pSkin->m_pMaterial->m_pEnvMap;
                VertexProgram* pProgram;
                if (pEnvMap->IsCubeMap())
                    pProgram = GetSkinCubeMapVP();
                else
                    pProgram = GetSkinSphereMapVP();
                pProgram->Enable(pSkin, true);
                RenderEnvironmentMapped(pPrimitive, true);
            }
            else
            {
                GLRender::ActivateTextureStage(0);
                pSkin->m_pMaterial->BindTexture0();
                VertexProgram* pProgram;
                if (use3boneskinning)
                {
                    static VertexProgram* skin3bonevp = new VertexProgram(
                        cSkin3BoneVP, cSkin3BoneVPDeclaration);
                    pProgram = skin3bonevp;
                    pProgram->Enable(pSkin, true);
                }
                else
                {
                    pProgram = GetSkinVP(pSkin->m_bHologram);
                    pProgram->Enable(pSkin, true);
                }
                RenderFlat(pPrimitive);
            }
        }
        GLRender_Disable(ENABLE_VERTEX_PROGRAM);
#else
        if (pSkin->m_pMaterial->HasBumpMap() &&
            pSkin->m_pMaterial->HasBumpyShinyMap() &&
            pTriMesh->HasTangentBases() && AurBumpyShinyAvailable() &&
            enablebumpyshiny)
        {
            GLRender_PushAttrib(GL_COLOR_BUFFER_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT);
            GLRender_Enable(ENABLE_BLEND);
            GLRender_Disable(ENABLE_ALPHA_TEST);
            if (bHologram)
            {
                static VertexProgram* pHoloBumpyShinySkinVP = new VertexProgram(
                    cHoloSkinVPARB, cBumpyShinySkinVPDeclaration);
                GLRender_DisableClientState(CLIENT_STATE_NORMAL);
                pHoloBumpyShinySkinVP->Enable(pSkin, false);
                if (g_ForceSight)
                {
                    GLRender_EnablePixelShaders(MODEL_SHADER_HOLO_DESAT, 0, 0, 0, 0);
                    SetBlendMode(BLEND_SRC_ALPHA, BLEND_ONE);
                }
                else
                {
                    GLRender_EnablePixelShaders(MODEL_SHADER_HOLO, 0, 0, 0, 0);
                    SetBlendMode(BLEND_ONE, BLEND_SRC_ALPHA);
                }
                RenderFlat(pPrimitive);
                GLRender_EnablePixelShaders(MODEL_SHADER_HOLO_DISABLE, 0, 0, 0, 0);
                SetBlendMode(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
                if (usearbvertexprograms)
                {
                    glDisableVertexAttribArrayARB(GLRender::VERTEX_ATTRIBUTE_TANGENT_BASIS0);
                    glDisableVertexAttribArrayARB(GLRender::VERTEX_ATTRIBUTE_TANGENT_BASIS1);
                    glDisableVertexAttribArrayARB(GLRender::VERTEX_ATTRIBUTE_TANGENT_BASIS2);
                }
                GLRender_EnableClientState(CLIENT_STATE_NORMAL);
                GLRender::PopAttrib();
            }
            else
            {
                if (bumpyskinpass && pObject->m_fAlpha >= 1.0f)
                {
                    SetBlendMode(BLEND_ONE, BLEND_ZERO);
                    GLRender_DisableClientState(CLIENT_STATE_NORMAL);
                    AurSetVertexProgramParameter4f(LightManager::LIGHT_PROGRAM_COLOR_SCALE,
                        1.0f, 0.0f, 1.0f, 0.0f);
                    g_pBumpyShinySkinPass1->Enable(pSkin, false);
                    if (!g_ForceSight)
                    {
                        GLRender_EnablePixelShaders(MODEL_SHADER_TEXTURE_SHADER_BUMP,
                            pSkin->m_pMaterial->m_pBumpMap, 0, 0,
                            pSkin->m_pMaterial->m_pEnvMap);
                        GLRender_EnablePixelShaders(MODEL_SHADER_ENVIRONMENT_ONLY,
                            0, 0, 0, 0);
                        RenderFlat(pPrimitive);
                        GLRender_EnablePixelShaders(MODEL_SHADER_DISABLE, 0, 0, 0, 0);
                        GLRender_EnablePixelShaders(MODEL_SHADER_TEXTURE_SHADER_DISABLE,
                            0, 0, 0, 0);
                        GLRender_EnablePixelShaders(MODEL_SHADER_BUMP_DISABLE, 0, 0, 0, 0);
                    }
                    if (usearbvertexprograms)
                    {
                        glDisableVertexAttribArrayARB(GLRender::VERTEX_ATTRIBUTE_TANGENT_BASIS0);
                        glDisableVertexAttribArrayARB(GLRender::VERTEX_ATTRIBUTE_TANGENT_BASIS1);
                        glDisableVertexAttribArrayARB(GLRender::VERTEX_ATTRIBUTE_TANGENT_BASIS2);
                    }
                    else
                    {
                        GLRender_DisableClientState(CLIENT_STATE_TANGENT_BASIS0);
                        GLRender_DisableClientState(CLIENT_STATE_TANGENT_BASIS1);
                        GLRender_DisableClientState(CLIENT_STATE_TANGENT_BASIS2);
                    }
                    GLRender_EnableClientState(CLIENT_STATE_NORMAL);
                }
                GLRender::PopAttrib();
                AurSetVertexProgramParameter4f(LightManager::LIGHT_PROGRAM_COLOR_SCALE,
                    1.0f, 1.0f, 1.0f, 1.0f);
                if (use3boneskinning)
                    g_pBumpyShinySkinPass2_3bone->Enable(pSkin, true);
                else
                    g_pBumpyShinySkinPass2->Enable(pSkin, true);
                if (enabledecal)
                {
                    GLRender::ActivateTextureStage(0);
                    pSkin->m_pMaterial->BindTexture0();
                    SetBlendMode(BLEND_ONE, BLEND_ONE_MINUS_SRC_ALPHA);
                    if (g_ForceSight)
                        SetupForceSightShader(pObject, MODEL_SHADER_FGLOW, MODEL_SHADER_DESAT,
                            0, 0, 0, 0, false);
                    RenderFlat(pPrimitive);
                    if (g_ForceSight)
                    {
                        GLRender_EnablePixelShaders(MODEL_SHADER_NOISE_DISABLE, 0, 0, 0, 0);
                        SetBlendMode(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
                        glDepthMask(GL_TRUE);
                    }
                }
                GLRender_Enable(ENABLE_ALPHA_TEST);
            }
        }
        else if (pSkin->m_pMaterial->HasBumpMap() && AurBumpMapCardValidate() &&
            pTriMesh->HasTangentBases() && AurNV2xLevelShadersAvailable() &&
            enablebumpmap && usenewbumpyskin)
        {
            GLRender_Disable(ENABLE_ALPHA_TEST);
            static VertexProgram* skinvp = new VertexProgram(
                usearbvertexprograms ? szNewSkinBumpyVPARB : cNewSkinBumpyVP,
                cSkinBumpyVPDeclaration);
            if (bHologram)
            {
                static VertexProgram* pHoloSkinBumpyVP = new VertexProgram(
                    cHoloSkinVPARB, cSkinBumpyVPDeclaration);
                pHoloSkinBumpyVP->Enable(pSkin, false);
                if (g_ForceSight)
                {
                    GLRender_EnablePixelShaders(MODEL_SHADER_HOLO_DESAT, 0, 0, 0, 0);
                    SetBlendMode(BLEND_SRC_ALPHA, BLEND_ONE);
                }
                else
                {
                    GLRender_EnablePixelShaders(MODEL_SHADER_HOLO, 0, 0, 0, 0);
                    SetBlendMode(BLEND_ONE, BLEND_SRC_ALPHA);
                }
                RenderFlat(pPrimitive);
                GLRender_EnablePixelShaders(MODEL_SHADER_HOLO_DISABLE, 0, 0, 0, 0);
                SetBlendMode(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
            }
            else
            {
                Vector vDiffuse;
                Vector vSpecular;
                if (pTriMesh != 0)
                {
                    vSpecular = pSkin->m_pMaterial->GetBumpMapSpecularColor();
                    vDiffuse = pTriMesh->m_vDiffuse;
                }
                List<PartLight*> lstLights(*currentscene->m_pLightManager->GetBumpMapLights());
                if (!g_ForceSight)
                {
                    GLRender_EnablePixelShaders(MODEL_SHADER_BUMP,
                        pSkin->m_pMaterial->m_pBumpMap,
                        currentscene->m_pNormalizationCubeMap,
                        currentscene->m_pNormalizationCubeMap,
                        pSkin->m_pMaterial->m_pTexture0);
                    Vector& vAmbient = currentscene->m_vAmbient;
                    GLRender_SetPixelShaderAmbientColor(vAmbient, 1.0f);
                    Quaternion qOrientation;
                    Vector vPosition;
                    pSkin->GetWorld(vPosition, qOrientation);
                    Vector vEye = -qOrientation * (CurrentCamera->m_vPosition - vPosition);
                    GLRender_Enable(ENABLE_VERTEX_PROGRAM);
                    SetBlendMode(BLEND_ONE, BLEND_ZERO);
                    currentscene->m_pLightManager->DisableBumpMapLights();
                    GLRender_Disable(ENABLE_FOG);
                    for (int nLight = 0; nLight < lstLights.m_nCount; nLight++)
                    {
                        skinvp->Enable(pSkin, false);
                        Vector vLight = lstLights[nLight]->m_vWorldPosition;
                        Vector& vColor = lstLights[nLight]->m_vColor;
                        vLight = -qOrientation * (vLight - vPosition);
                        Vector vLightColor = cmul(vColor, vDiffuse * diffuseintensity);
                        if (usebumpdiffuse)
                            SetBumpLightColor(vLightColor);
                        else
                        {
                            Vector vBlack;
                            SetBumpLightColor(vBlack);
                        }
                        vLightColor = cmul(lstLights[nLight]->m_vColor,
                            (vSpecular * specularintensity) *
                            pSkin->m_pMaterial->GetBumpMapIntensity());
                        if (usebumpspecular)
                            GLRender_SetPixelShaderAmbientColor(vLightColor, 1.0f);
                        else
                        {
                            Vector vBlack;
                            GLRender_SetPixelShaderAmbientColor(vBlack, 1.0f);
                        }
                        AurSetVertexProgramParameter4f(VertexProgram::PARAM_NORMAL_MAP_LIGHT_POSITION,
                            vLight.m_fX, vLight.m_fY, vLight.m_fZ, 1.0f);
                        AurSetVertexProgramParameter4f(VertexProgram::PARAM_NORMAL_MAP_EYE_POSITION,
                            vEye.m_fX, vEye.m_fY, vEye.m_fZ, 1.0f);
                        float fRadius = lstLights[nLight]->m_fRadius;
                        AurSetVertexProgramParameter4f(VertexProgram::PARAM_NORMAL_MAP_ATTENUATION,
                            1.0f, 0.0f, AUR_LIGHT_ATTENUATION_SCALE / (fRadius * fRadius + fRadius * fRadius), 1.0f);
                        RenderFlat(pPrimitive);
                    }
                    if (lstLights.m_nCount == 0)
                    {
                        skinvp->Enable(pSkin, false);
                        SetBumpLightColor(Vector());
                        GLRender_SetPixelShaderAmbientColor(Vector(), 1.0f);
                        RenderFlat(pPrimitive);
                    }
                }
                if (usearbvertexprograms)
                {
                    glDisableVertexAttribArrayARB(GLRender::VERTEX_ATTRIBUTE_TANGENT_BASIS0);
                    glDisableVertexAttribArrayARB(GLRender::VERTEX_ATTRIBUTE_TANGENT_BASIS1);
                    glDisableVertexAttribArrayARB(GLRender::VERTEX_ATTRIBUTE_TANGENT_BASIS2);
                }
                else
                {
                    GLRender_DisableClientState(CLIENT_STATE_TANGENT_BASIS0);
                    GLRender_DisableClientState(CLIENT_STATE_TANGENT_BASIS1);
                    GLRender_DisableClientState(CLIENT_STATE_TANGENT_BASIS2);
                }
                SetBlendMode(BLEND_ONE, BLEND_ONE);
                GLRender_EnablePixelShaders(MODEL_SHADER_BUMP_DISABLE, 0, 0, 0, 0);
                GLRender_EnablePixelShaders(MODEL_SHADER_DISABLE, 0, 0, 0, 0);
                AurSetVertexProgramParameter4f(LightManager::LIGHT_PROGRAM_COLOR_SCALE,
                    1.0f, 1.0f, 1.0f, 1.0f);
                static VertexProgram* skinDecalPassvp = new VertexProgram(
                    usearbvertexprograms ? szSkinVPARB : cSkinVP,
                    cSkinBumpyVPDeclaration);
                skinDecalPassvp->Enable(pSkin, true);
                if (enabledecal)
                {
                    GLRender::ActivateTextureStage(0);
                    pSkin->m_pMaterial->BindTexture0();
                    if (g_ForceSight)
                        SetupForceSightShader(pObject, MODEL_SHADER_FGLOW, MODEL_SHADER_DESAT,
                            0, 0, 0, 0, false);
                    RenderFlat(pPrimitive);
                    if (g_ForceSight)
                    {
                        GLRender_EnablePixelShaders(MODEL_SHADER_BUMP_DISABLE, 0, 0, 0, 0);
                        GLRender_EnablePixelShaders(MODEL_SHADER_DISABLE, 0, 0, 0, 0);
                    }
                }
                GLRender_Enable(ENABLE_ALPHA_TEST);
                currentscene->m_pLightManager->EnableBumpMapLights();
                SetBlendMode(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
                currentscene->DoFog();
                if (currentscene->m_bFog)
                {
                    static VertexProgram* skinFogVP = new VertexProgram(
                        usearbvertexprograms ? szSkinFogVPARB : 0,
                        cSkinBumpyVPDeclaration);
                    skinFogVP->Enable(pSkin, false);
                    GLRender_EnablePixelShaders(MODEL_SHADER_FOG, 0, 0, 0, 0);
                    RenderFlat(pPrimitive);
                    GLRender_EnablePixelShaders(MODEL_SHADER_DISABLE, 0, 0, 0, 0);
                }
            }
        }
        else if (pSkin->m_pMaterial->HasNormalMap() && AurBumpMapCardValidate() &&
            AurNV2xLevelShadersAvailable() && enablebumpmap && usenewbumpyskin)
        {
            GLRender_Disable(ENABLE_ALPHA_TEST);
            static VertexProgram* skinvp = new VertexProgram(
                szSkinBumpyWSVPARB, szNormalmapVPARBDeclaration);
            if (bHologram)
            {
                static VertexProgram* pHoloSkinNormalVP = new VertexProgram(
                    cHoloSkinVPARB, szNormalmapVPARBDeclaration);
                pHoloSkinNormalVP->Enable(pSkin, false);
                if (g_ForceSight)
                {
                    GLRender_EnablePixelShaders(MODEL_SHADER_HOLO_DESAT, 0, 0, 0, 0);
                    SetBlendMode(BLEND_SRC_ALPHA, BLEND_ONE);
                }
                else
                {
                    GLRender_EnablePixelShaders(MODEL_SHADER_HOLO, 0, 0, 0, 0);
                    SetBlendMode(BLEND_ONE, BLEND_SRC_ALPHA);
                }
                RenderFlat(pPrimitive);
                GLRender_EnablePixelShaders(MODEL_SHADER_HOLO_DISABLE, 0, 0, 0, 0);
                SetBlendMode(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
            }
            else
            {
                Vector vDiffuse;
                Vector vSpecular;
                if (pTriMesh != 0)
                {
                    vSpecular = pSkin->m_pMaterial->GetBumpMapSpecularColor();
                    vDiffuse = pTriMesh->m_vDiffuse;
                }
                List<PartLight*> lstLights(*currentscene->m_pLightManager->GetBumpMapLights());
                GLRender_EnablePixelShaders(MODEL_SHADER_NORMAL,
                    pSkin->m_pMaterial->m_pTexture0, pSkin->m_pMaterial->m_pBumpMap,
                    currentscene->m_pNormalizationCubeMap,
                    currentscene->m_pNormalizationCubeMap);
                Vector& vAmbient = currentscene->m_vAmbient;
                GLRender_SetPixelShaderAmbientColor(vAmbient, 1.0f);
                GLRender::ActivateTextureStage(1);
                GLRender_EnableClientState(CLIENT_STATE_TEXTURE_COORD);
                GLRender::ActivateTextureStage(0);
                Quaternion qOrientation;
                Vector vPosition;
                pSkin->GetWorld(vPosition, qOrientation);
                Vector vEye = -qOrientation * (CurrentCamera->m_vPosition - vPosition);
                GLRender_Enable(ENABLE_VERTEX_PROGRAM);
                SetBlendMode(BLEND_ONE, BLEND_ZERO);
                currentscene->m_pLightManager->DisableBumpMapLights();
                for (int nLight = 0; nLight < lstLights.m_nCount; nLight++)
                {
                    skinvp->Enable(pSkin, false);
                    Vector vLight = lstLights[nLight]->m_vWorldPosition;
                    Vector& vColor = lstLights[nLight]->m_vColor;
                    vLight = -qOrientation * (vLight - vPosition);
                    Vector vLightColor = cmul(vColor, vDiffuse * diffuseintensity);
                    if (usebumpdiffuse)
                        GLRender_SetBumpLightColor(vLightColor, 1.0f);
                    else
                    {
                        Vector vBlack;
                        GLRender_SetBumpLightColor(vBlack, 1.0f);
                    }
                    vLightColor = cmul(lstLights[nLight]->m_vColor,
                        (vSpecular * specularintensity) *
                        pSkin->m_pMaterial->GetBumpMapIntensity());
                    if (usebumpspecular)
                        GLRender_SetPixelShaderAmbientColor(vLightColor, 1.0f);
                    else
                    {
                        Vector vBlack;
                        GLRender_SetPixelShaderAmbientColor(vBlack, 1.0f);
                    }
                    AurSetVertexProgramParameter4f(VertexProgram::PARAM_NORMAL_MAP_LIGHT_POSITION,
                        vLight.m_fX, vLight.m_fY, vLight.m_fZ, 1.0f);
                    AurSetVertexProgramParameter4f(VertexProgram::PARAM_NORMAL_MAP_EYE_POSITION,
                        vEye.m_fX, vEye.m_fY, vEye.m_fZ, 1.0f);
                    float fRadius = lstLights[nLight]->m_fRadius;
                    AurSetVertexProgramParameter4f(VertexProgram::PARAM_NORMAL_MAP_ATTENUATION,
                        1.0f, 0.0f, AUR_LIGHT_ATTENUATION_SCALE / (fRadius * fRadius + fRadius * fRadius), 1.0f);
                    RenderFlat(pPrimitive);
                }
                if (lstLights.m_nCount == 0)
                {
                    skinvp->Enable(pSkin, false);
                    GLRender_SetBumpLightColor(Vector(), 1.0f);
                    GLRender_SetPixelShaderAmbientColor(Vector(), 1.0f);
                    RenderFlat(pPrimitive);
                }
                GLRender::ActivateTextureStage(1);
                GLRender_DisableClientState(CLIENT_STATE_TEXTURE_COORD);
                GLRender::ActivateTextureStage(0);
                currentscene->DoFog();
                GLRender_SetBlendFunc(BLEND_ONE, BLEND_ONE);
                GLRender_EnablePixelShaders(MODEL_SHADER_BUMP_DISABLE, 0, 0, 0, 0);
                GLRender_EnablePixelShaders(MODEL_SHADER_DISABLE, 0, 0, 0, 0);
                AurSetVertexProgramParameter4f(LightManager::LIGHT_PROGRAM_COLOR_SCALE,
                    1.0f, 1.0f, 1.0f, 1.0f);
                static VertexProgram* skinDecalPassvp = new VertexProgram(
                    usearbvertexprograms ? szSkinVPARB : cSkinVP,
                    szNormalmapVPARBDeclaration);
                skinDecalPassvp->Enable(pSkin, true);
                if (enabledecal)
                {
                    GLRender::ActivateTextureStage(0);
                    pSkin->m_pMaterial->BindTexture0();
                    RenderFlat(pPrimitive);
                }
                GLRender_Enable(ENABLE_ALPHA_TEST);
                currentscene->m_pLightManager->EnableBumpMapLights();
                GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
            }
        }
        else if (pSkin->m_pMaterial->HasBumpMap() && AurBumpMapCardValidate() &&
            pTriMesh->HasTangentBases() && AurNV2xLevelShadersAvailable() &&
            enablebumpmap)
        {
            static VertexProgram* skinvp = new VertexProgram(
                usearbvertexprograms ? szSkinBumpyVPARB : cSkinBumpyVP,
                cSkinBumpyVPDeclaration);
            if (bHologram)
            {
                static VertexProgram* pHoloSkinBumpyOldVP = new VertexProgram(
                    cHoloSkinVPARB, cSkinBumpyVPDeclaration);
                pHoloSkinBumpyOldVP->Enable(pSkin, false);
                if (g_ForceSight)
                {
                    GLRender_EnablePixelShaders(MODEL_SHADER_HOLO_DESAT, 0, 0, 0, 0);
                    SetBlendMode(BLEND_SRC_ALPHA, BLEND_ONE);
                }
                else
                {
                    GLRender_EnablePixelShaders(MODEL_SHADER_HOLO, 0, 0, 0, 0);
                    SetBlendMode(BLEND_ONE, BLEND_SRC_ALPHA);
                }
                RenderFlat(pPrimitive);
                GLRender_EnablePixelShaders(MODEL_SHADER_HOLO_DISABLE, 0, 0, 0, 0);
                SetBlendMode(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
            }
            else
            {
                Vector vDiffuse;
                Vector vSpecular;
                if (pTriMesh != 0)
                {
                    vSpecular = pSkin->m_pMaterial->GetBumpMapSpecularColor();
                    vDiffuse = pTriMesh->m_vDiffuse;
                }
                List<PartLight*> lstLights(*currentscene->m_pLightManager->GetBumpMapLights());
                GLRender_EnablePixelShaders(MODEL_SHADER_BUMP,
                    pSkin->m_pMaterial->m_pBumpMap,
                    currentscene->m_pNormalizationCubeMap,
                    currentscene->m_pNormalizationCubeMap,
                    pSkin->m_pMaterial->m_pTexture0);
                Vector& vAmbient = currentscene->m_vAmbient;
                GLRender_SetPixelShaderAmbientColor(vAmbient, 1.0f);
                Quaternion qOrientation;
                Vector vPosition;
                pSkin->GetWorld(vPosition, qOrientation);
                Vector vEye = -qOrientation * (CurrentCamera->m_vPosition - vPosition);
                GLRender_Enable(ENABLE_VERTEX_PROGRAM);
                SetBlendMode(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
                for (int nLight = 0; nLight < lstLights.m_nCount; nLight++)
                {
                    currentscene->m_pLightManager->DisableBumpMapLights();
                    skinvp->Enable(pSkin, true);
                    Vector vLight = lstLights[nLight]->m_vWorldPosition;
                    Vector& vColor = lstLights[nLight]->m_vColor;
                    vLight = -qOrientation * (vLight - vPosition);
                    Vector vLightColor = cmul(vColor, vDiffuse * diffuseintensity);
                    if (usebumpdiffuse)
                        GLRender_SetBumpLightColor(vLightColor, 1.0f);
                    else
                    {
                        Vector vBlack;
                        GLRender_SetBumpLightColor(vBlack, 1.0f);
                    }
                    vLightColor = cmul(lstLights[nLight]->m_vColor,
                        (vSpecular * specularintensity) *
                        pSkin->m_pMaterial->GetBumpMapIntensity());
                    if (usebumpspecular)
                        GLRender_SetPixelShaderAmbientColor(vLightColor, 1.0f);
                    else
                    {
                        Vector vBlack;
                        GLRender_SetPixelShaderAmbientColor(vBlack, 1.0f);
                    }
                    AurSetVertexProgramParameter4f(VertexProgram::PARAM_NORMAL_MAP_LIGHT_POSITION,
                        vLight.m_fX, vLight.m_fY, vLight.m_fZ, 1.0f);
                    AurSetVertexProgramParameter4f(VertexProgram::PARAM_NORMAL_MAP_EYE_POSITION,
                        vEye.m_fX, vEye.m_fY, vEye.m_fZ, 1.0f);
                    float fRadius = lstLights[nLight]->m_fRadius;
                    AurSetVertexProgramParameter4f(VertexProgram::PARAM_NORMAL_MAP_ATTENUATION,
                        1.0f, 0.0f, AUR_LIGHT_ATTENUATION_SCALE / (fRadius * fRadius + fRadius * fRadius), 1.0f);
                    RenderFlat(pPrimitive);
                    currentscene->m_pLightManager->EnableBumpMapLights();
                }
                if (lstLights.m_nCount == 0)
                {
                    skinvp->Enable(pSkin, true);
                    GLRender_SetBumpLightColor(Vector(), 1.0f);
                    GLRender_SetPixelShaderAmbientColor(Vector(), 1.0f);
                    RenderFlat(pPrimitive);
                }
                if (usearbvertexprograms)
                {
                    glDisableVertexAttribArrayARB(GLRender::VERTEX_ATTRIBUTE_TANGENT_BASIS0);
                    glDisableVertexAttribArrayARB(GLRender::VERTEX_ATTRIBUTE_TANGENT_BASIS1);
                    glDisableVertexAttribArrayARB(GLRender::VERTEX_ATTRIBUTE_TANGENT_BASIS2);
                }
                else
                {
                    GLRender_DisableClientState(CLIENT_STATE_TANGENT_BASIS0);
                    GLRender_DisableClientState(CLIENT_STATE_TANGENT_BASIS1);
                    GLRender_DisableClientState(CLIENT_STATE_TANGENT_BASIS2);
                }
                currentscene->DoFog();
                GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
                GLRender_EnablePixelShaders(MODEL_SHADER_BUMP_DISABLE, 0, 0, 0, 0);
                GLRender_EnablePixelShaders(MODEL_SHADER_DISABLE, 0, 0, 0, 0);
            }
        }
        else
        {
            if ((pSkin->m_pAurObject != 0 && pSkin->m_pAurObject->m_pEnvironmentMap.m_pObject != 0) ||
                pSkin->m_pMaterial->HasEnvMap())
            {
                CAurTexture* pEnvMap;
                if (pSkin->m_pAurObject != 0 && pSkin->m_pAurObject->m_pEnvironmentMap.m_pObject != 0)
                    pEnvMap = pSkin->m_pAurObject->m_pEnvironmentMap;
                else
                    pEnvMap = pSkin->m_pMaterial->m_pEnvMap;
                if (bHologram)
                {
                    static VertexProgram* pHoloSkinCubeMapVP = new VertexProgram(
                        cHoloSkinVPARB, cSkinCubeMapVPDeclaration);
                    static VertexProgram* pHoloSkinSphereMapVP = new VertexProgram(
                        cHoloSkinVPARB, cSkinSphereMapVPDeclaration);
                    if (pEnvMap->IsCubeMap())
                        pHoloSkinCubeMapVP->Enable(pSkin, false);
                    else
                        pHoloSkinSphereMapVP->Enable(pSkin, false);
                    if (g_ForceSight)
                    {
                        GLRender_EnablePixelShaders(MODEL_SHADER_HOLO_DESAT, 0, 0, 0, 0);
                        GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE);
                    }
                    else
                    {
                        GLRender_EnablePixelShaders(MODEL_SHADER_HOLO, 0, 0, 0, 0);
                        GLRender_SetBlendFunc(BLEND_ONE, BLEND_SRC_ALPHA);
                    }
                    RenderFlat(pPrimitive);
                    GLRender_EnablePixelShaders(MODEL_SHADER_HOLO_DISABLE, 0, 0, 0, 0);
                    GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
                }
                else
                {
                    VertexProgram* pProgram;
                    if (pEnvMap->IsCubeMap())
                        pProgram = GetSkinCubeMapVP();
                    else
                        pProgram = GetSkinSphereMapVP();
                    if (AurATIValidate())
                    {
                        if (!currentscene->m_bFog)
                        {
                            float afBlack[3] = {0.0f, 0.0f, 0.0f};
                            glFogfv(enableTranslationTable[FOG_COLOR], afBlack);
                            GLRender_SetFogModef(FOG_START, 1000.0f);
                            GLRender_SetFogModef(FOG_END, 2000.0f);
                        }
                        GLRender_Disable(ENABLE_FOG);
                        float afFog[4] = {
                            pSkin->m_pAurObject->m_pScene->m_vFogColor.m_fX,
                            pSkin->m_pAurObject->m_pScene->m_vFogColor.m_fY,
                            pSkin->m_pAurObject->m_pScene->m_vFogColor.m_fZ, 1.0f};
                        glSetFragmentShaderConstantATI(GL_CON_1_ATI, afFog);
                    }
                    pProgram->Enable(pSkin, true);
                    if (AurATIValidate())
                        RenderEnvironmentMapped(pPrimitive, true, true);
                    else
                        RenderEnvironmentMapped(pPrimitive, true, false);
                    if (AurATIValidate())
                    {
                        GLRender_EnablePixelShaders(MODEL_SHADER_DISABLE,
                            pSkin->m_pMaterial->m_pTexture0, 0, 0, 0);
                        currentscene->DoFog();
                    }
                }
            }
            else
            {
                GLRender::ActivateTextureStage(0);
                pSkin->m_pMaterial->BindTexture0();
                if (AurATIValidate() && currentscene->m_bFog)
                {
                    GLRender_Disable(ENABLE_FOG);
                    GLRender_EnablePixelShaders(MODEL_SHADER_STANDARD_FOG,
                        pSkin->m_pMaterial->m_pTexture0, 0, 0, 0);
                    float afFog[4] = {
                        pSkin->m_pAurObject->m_pScene->m_vFogColor.m_fX,
                        pSkin->m_pAurObject->m_pScene->m_vFogColor.m_fY,
                        pSkin->m_pAurObject->m_pScene->m_vFogColor.m_fZ, 1.0f};
                    glSetFragmentShaderConstantATI(GL_CON_1_ATI, afFog);
                }
                VertexProgram* pProgram;
                if (use3boneskinning)
                {
                    static VertexProgram* skin3bonevp = new VertexProgram(
                        cSkin3BoneVP, cSkin3BoneVPDeclaration);
                    pProgram = skin3bonevp;
                    pProgram->Enable(pSkin, true);
                }
                else
                {
                    pProgram = GetSkinVP(bHologram);
                    pProgram->Enable(pSkin, true);
                }
                if (pSkin->m_pMaterial->HasBumpMap() && pTriMesh->HasTangentBases() &&
                        !AurNV2xLevelShadersAvailable())
                {
                    GLRender_SetBlendFunc(BLEND_ONE, BLEND_ZERO);
                    RenderFlat(pPrimitive);
                    GLRender_SetBlendFunc(BLEND_SRC_ALPHA,
                        BLEND_ONE_MINUS_SRC_ALPHA);
                }
                else if (pSkin->m_pMaterial->HasBumpMap() && pTriMesh->HasTangentBases() &&
                        !AurBumpMapCardValidate())
                {
                    GLRender_SetBlendFunc(BLEND_ONE, BLEND_ZERO);
                    if (g_ForceSight)
                        SetupForceSightShader(pObject, MODEL_SHADER_FGLOW, MODEL_SHADER_DESAT,
                            0, 0, 0, 0, false);
                    RenderFlat(pPrimitive);
                    if (g_ForceSight)
                    {
                        GLRender_EnablePixelShaders(MODEL_SHADER_NOISE_DISABLE, 0, 0, 0, 0);
                        SetBlendMode(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
                        glDepthMask(GL_TRUE);
                    }
                    GLRender_SetBlendFunc(BLEND_SRC_ALPHA,
                        BLEND_ONE_MINUS_SRC_ALPHA);
                }
                else
                {
                    if (bHologram)
                    {
                        if (g_ForceSight)
                        {
                            GLRender_EnablePixelShaders(MODEL_SHADER_HOLO_DESAT, 0, 0, 0, 0);
                            GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE);
                        }
                        else
                        {
                            GLRender_EnablePixelShaders(MODEL_SHADER_HOLO, 0, 0, 0, 0);
                            GLRender_SetBlendFunc(BLEND_ONE, BLEND_SRC_ALPHA);
                        }
                    }
                    else if (g_ForceSight)
                    {
                        SetupForceSightShader(pObject, MODEL_SHADER_FGLOW, MODEL_SHADER_DESAT,
                            0, 0, 0, 0, false);
                    }
                    RenderFlat(pPrimitive);
                    if (bHologram)
                    {
                        GLRender_EnablePixelShaders(MODEL_SHADER_HOLO_DISABLE, 0, 0, 0, 0);
                        GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
                    }
                    else if (g_ForceSight)
                    {
                        GLRender_EnablePixelShaders(MODEL_SHADER_NOISE_DISABLE, 0, 0, 0, 0);
                        GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
                        glDepthMask(GL_TRUE);
                    }
                }
                if (AurATIValidate())
                {
                    GLRender_EnablePixelShaders(MODEL_SHADER_DISABLE,
                        pSkin->m_pMaterial->m_pTexture0, 0, 0, 0);
                    currentscene->DoFog();
                }
            }
        }
        if (usearbvertexprograms)
            glDisable(GL_VERTEX_PROGRAM_ARB);
        else
            GLRender_Disable(ENABLE_VERTEX_PROGRAM);
#endif
    }
}
// PC 0x004A8DE0 GOG 0x004A8EA0
void RenderSkinnedVPBlur(VertexPrimitiveSkinned* pPrimitive)
{
    PartSkin* pSkin = pPrimitive->m_pPart->AsPartSkin();
    MdlNodeSkin* pNode = (MdlNodeSkin*)pSkin->m_pNode;
    if (pNode->m_nTextureCount != 0)
    {
        pSkin->AsPartTriMesh();
        Vector vDisplacement = pSkin->m_pAurObject->m_vPosition -
            pSkin->m_pAurObject->m_vPreviousPosition;
        float fBlurLength = magnitude(vDisplacement);
        if (fBlurLength > 0.0f)
        {
            pPrimitive->SetupVertexArraysStandard();
            Matrix mRotation;
            static List<Quaternion> qbones(0);
            static List<Vector> tbones(0);
            qbones.m_nCount = 0;
            tbones.m_nCount = 0;
            GLRender_SetDepthMask(GL_FALSE);
            GLRender_Enable(ENABLE_VERTEX_PROGRAM);

            Part* pPart = pSkin;
            Vector vTranslation = -pPart->m_qOrientation * -pPart->m_vPosition;
            Quaternion qRotation = -pPart->m_qOrientation;
            while (pPart->m_pParent != 0)
            {
                pPart = pPart->m_pParent;
                vTranslation = vTranslation + qRotation *
                    (-pPart->m_qOrientation * -pPart->m_vPosition);
                qRotation = qRotation * -pPart->m_qOrientation;
            }
            GatherBones(pPart, tbones, qbones, vTranslation, qRotation);
            for (int i = 0; i < pNode->m_nBoneMap; i++)
            {
                int nBone = (int)pNode->m_pfBoneMap[i];
                if (nBone != -1)
                {
                    tbones[i] = tbones[i] + qbones[i] * pNode->m_lstTBoneRefInv[i];
                    qbones[i] = qbones[i] * pNode->m_lstQBoneRefInv[i];
                    mRotation = qbones[i].getmatrix();
                    Vector& vBoneTranslation = tbones[i];
                    nBone = PartSkin::SKIN_VP_BONES + 3 * nBone;
                    AurSetVertexProgramParameter4f(nBone,
                        mRotation.m_vFirst.m_fX, mRotation.m_vSecond.m_fX,
                        mRotation.m_vThird.m_fX, vBoneTranslation.m_fX);
                    AurSetVertexProgramParameter4f(++nBone,
                        mRotation.m_vFirst.m_fY, mRotation.m_vSecond.m_fY,
                        mRotation.m_vThird.m_fY, vBoneTranslation.m_fY);
#ifdef _XBOX
                    AurSetVertexProgramParameter4f(nBone + 1,
                        mRotation.m_vFirst.m_fZ, mRotation.m_vSecond.m_fZ,
                        mRotation.m_vThird.m_fZ, vBoneTranslation.m_fZ);
#else
                    AurSetVertexProgramParameter4f(++nBone,
                        mRotation.m_vFirst.m_fZ, mRotation.m_vSecond.m_fZ,
                        mRotation.m_vThird.m_fZ, vBoneTranslation.m_fZ);
#endif
                }
            }

            static VertexProgram* skinmotionblurvp = new VertexProgram(
                cSkinMotionBlurVP, cSkinMotionBlurVPDeclaration);

            Quaternion qOrientation;
            Vector vPosition;
            pSkin->GetWorld(vPosition, qOrientation);
            Vector vCamera = -qOrientation * (CurrentCamera->m_vPosition - vPosition);
            Quaternion& qObject = pSkin->m_pAurObject->m_qOrientation;
            Vector vSide = cross(vDisplacement, qObject.zdir());
            vDisplacement = -qOrientation * vDisplacement;
            vSide = -qOrientation * vSide;
            fBlurLength = fBlurLength / (2.0f * DeltaT);
            Vector vOrigin;
            Vector& vObjectPosition = pSkin->m_pAurObject->m_vPosition;
            vOrigin = -qOrientation * (vObjectPosition - vPosition);

            AurSetVertexProgramParameter4f(PartSkin::SKIN_VP_CONSTANTS,
                0.0f, 2.0f, 3.0f, 1.0f);
            AurSetVertexProgramParameter4f(PartSkin::SKIN_VP_BLUR_REFERENCE,
                vOrigin.m_fX, vOrigin.m_fY, vOrigin.m_fZ, SKIN_BLUR_SIDE_SCALE);
            vDisplacement = normalize(vDisplacement);
            AurSetVertexProgramParameter4f(PartSkin::SKIN_VP_BLUR_DIRECTION,
                vDisplacement.m_fX, vDisplacement.m_fY, vDisplacement.m_fZ, 1.0f);
            float fMaxBlurLength = pSkin->m_pAurObject->m_fMaxBlurLength;
            AurSetVertexProgramParameter4f(PartSkin::SKIN_VP_BLUR_LIMITS,
                fBlurLength, fMaxBlurLength, SKIN_BLUR_NORMAL_OFFSET, SKIN_BLUR_LENGTH_SCALE);
            AurSetVertexProgramParameter4f(PartSkin::SKIN_VP_BLUR_SIDE,
                vSide.m_fX, vSide.m_fY, vSide.m_fZ, SKIN_BLUR_ALPHA);
            AurSetVertexProgramParameter4f(PartSkin::SKIN_VP_BLUR_REVERSE,
                0.0f, 0.0f, 0.0f, 0.0f);

            Vector vCameraToOrigin = vOrigin - vCamera;
            Vector vProjection = vDisplacement * (vCameraToOrigin * vDisplacement);
            if (vProjection.m_fX * vProjection.m_fX +
                vProjection.m_fY * vProjection.m_fY +
                vProjection.m_fZ * vProjection.m_fZ >
                (vCameraToOrigin * vCameraToOrigin) * SKIN_BLUR_CULL_COSINE_SQUARED)
            {
                if (magnitude(vCameraToOrigin + vDisplacement) >
                    magnitude(vCameraToOrigin - vDisplacement))
                {
                    AurSetVertexProgramParameter4f(PartSkin::SKIN_VP_BLUR_REVERSE,
                        1.0f, 0.0f, 0.0f, 0.0f);
                    GLRender_SetFrontFace(CULL_CLOCKWISE);
                }
            }
            GLRender_EnablePixelShaders(MODEL_SHADER_SKIN_BLUR,
                pSkin->m_pMaterial->m_pTexture0.m_pObject, 0, 0, 0);
            skinmotionblurvp->Enable(pSkin, true);
            RenderFlat(pPrimitive);
            GLRender_EnablePixelShaders(MODEL_SHADER_DISABLE, 0, 0, 0, 0);
        }
        GLRender_SetFrontFace(CULL_COUNTERCLOCKWISE);
        GLRender_Disable(ENABLE_VERTEX_PROGRAM);
    }
}

#ifndef _XBOX
void PartSkin::DrawBumpedOutShellVP()  // PC 0x004AA490 GOG 0x004AA550
{
    GLRender::SetActiveTexture(0);
    ((CAurTexture*)m_pMaterial->m_pTexture3.m_pObject)->Bind();
    if (m_pMaterial->m_lstBlendSrc.m_nCount > 1)
    {
        m_pMaterial->SetBlendingMode(1);
    }

    VertexPrimitiveSkinned* pPrimitive =
        m_pVertexPrimitive->AsVertexPrimitiveSkinned();
    PartSkin* pSkin = pPrimitive->m_pPart->AsPartSkin();
    MdlNodeSkin* pNode = (MdlNodeSkin*)pSkin->m_pNode;
    if (pNode->m_nTextureCount != 0)
    {
        PartTriMesh* pMeshPart = pSkin->AsPartTriMesh();
        MdlNodeTriMesh* pTriMesh = 0;
        if (pMeshPart != 0)
        {
            pTriMesh = (MdlNodeTriMesh*)pSkin->m_pNode;
        }
        pPrimitive->SetupVertexArraysStandard();
        static List<Quaternion> qbones(0);
        static List<Vector> tbones(0);
        qbones.m_nCount = 0;
        tbones.m_nCount = 0;
        Matrix mRotation;

        GLRender_Enable(ENABLE_VERTEX_PROGRAM);
        AurSetVertexProgramParameter4f(SKIN_VP_CONSTANTS, 0.0f, 2.0f, 3.0f, 1.0f);
        AurSetVertexProgramParameter4f(SKIN_VP_FOG, currentscene->m_fFogNear,
            currentscene->m_fFogFar, currentscene->m_bFog ? 1.0f : 0.0f, 0.5f);

        Part* pPart = pSkin;
        Vector vTranslation = -pPart->m_qOrientation * -pPart->m_vPosition;
        Quaternion qRotation = -pPart->m_qOrientation;
        while (pPart->m_pParent != 0)
        {
            pPart = pPart->m_pParent;
            Vector vLocal = -pPart->m_qOrientation * -pPart->m_vPosition;
            vTranslation = vTranslation + qRotation * vLocal;
            qRotation = qRotation * -pPart->m_qOrientation;
        }
        GatherBones(pPart, tbones, qbones, vTranslation, qRotation);
        for (int i = 0; i < pNode->m_nBoneMap; i++)
        {
            int nBone = (int)pNode->m_pfBoneMap[i];
            if (nBone != -1)
            {
                tbones[i] = tbones[i] + qbones[i] * pNode->m_lstTBoneRefInv[i];
                qbones[i] = qbones[i] * pNode->m_lstQBoneRefInv[i];
                mRotation = qbones[i].getmatrix();
                Vector& vBoneTranslation = tbones[i];
                int nRegister = SKIN_VP_BONES + 3 * nBone;
                AurSetVertexProgramParameter4f(nRegister,
                    mRotation.m_vFirst.m_fX, mRotation.m_vSecond.m_fX,
                    mRotation.m_vThird.m_fX, vBoneTranslation.m_fX);
                AurSetVertexProgramParameter4f(++nRegister,
                    mRotation.m_vFirst.m_fY, mRotation.m_vSecond.m_fY,
                    mRotation.m_vThird.m_fY, vBoneTranslation.m_fY);
                AurSetVertexProgramParameter4f(++nRegister,
                    mRotation.m_vFirst.m_fZ, mRotation.m_vSecond.m_fZ,
                    mRotation.m_vThird.m_fZ, vBoneTranslation.m_fZ);
            }
        }
        AurSetVertexProgramParameter4f(SKIN_VP_SHELL_HEIGHT,
            m_pAurObject->m_fBumpedOutShellHeight, 0.0f, 0.0f, 1.0f);
        VertexProgram* pProgram;
        if (pSkin->m_pMaterial->HasBumpMap() && pTriMesh->HasTangentBases() &&
            AurNV2xLevelShadersAvailable())
        {
            pProgram = g_pSkinBumpedOutWithTangentVP;
        }
        else
        {
            pProgram = g_pSkinBumpedOutVP;
        }
        pProgram->Enable(pSkin, false);
        if (g_ForceSight)
            SetupForceSightShader(m_pAurObject, MODEL_SHADER_FGLOW, MODEL_SHADER_DESAT,
                0, 0, 0, 0, false);
        RenderFlat(pPrimitive);
        if (g_ForceSight)
        {
            GLRender_EnablePixelShaders(MODEL_SHADER_NOISE_DISABLE, 0, 0, 0, 0);
            GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
            glDepthMask(GL_TRUE);
        }
#ifndef _XBOX
        if (usearbvertexprograms != 0)
        {
            glDisableVertexAttribArrayARB(GLRender::VERTEX_ATTRIBUTE_SKIN_WEIGHT);
            glDisableVertexAttribArrayARB(GLRender::VERTEX_ATTRIBUTE_BONE_INDEX);
        }
        else
#endif
        {
            GLRender_DisableClientState(CLIENT_STATE_SKIN_WEIGHT);
            GLRender_DisableClientState(CLIENT_STATE_BONE_INDEX);
        }
        GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_TRUE);
        GLRender_Disable(ENABLE_VERTEX_PROGRAM);
    }
}

#endif

#ifndef _XBOX
void PartSkin::DrawBumpedOutShell()  // PC 0x004AB160 GOG 0x004AB220
{
    if (AurNV2xLevelShadersAvailable())
    {
        DrawBumpedOutShellVP();
        return;
    }

    RenderSkinned(
        (VertexPrimitiveSkinned*)m_pVertexPrimitive->AsVertexPrimitiveSkinned(),
        true);
}
#endif
// PC 0x004AB190 GOG 0x004AB250
void PartSkin::Draw(bool bRenderLast)
{
#ifdef _XBOX

    if (renderskin != 0)
    {
        if (usevertexprograms != 0)
        {
            GLRender_EnableClientState(CLIENT_STATE_SKIN_WEIGHT);
            GLRender_EnableClientState(CLIENT_STATE_BONE_INDEX);
            if (!bRenderLast)
            {
                RenderSkinnedVP((VertexPrimitiveSkinned*)
                    m_pVertexPrimitive->AsVertexPrimitiveSkinned());
            }
            else
            {
                RenderSkinnedVPBlur((VertexPrimitiveSkinned*)
                    m_pVertexPrimitive->AsVertexPrimitiveSkinned());
            }
            GLRender_DisableClientState(CLIENT_STATE_SKIN_WEIGHT);
            GLRender_DisableClientState(CLIENT_STATE_BONE_INDEX);
        }
        else
        {
            GLRender_DisableClientState(CLIENT_STATE_COLOR);
            RenderSkinned((VertexPrimitiveSkinned*)
                m_pVertexPrimitive->AsVertexPrimitiveSkinned());
        }

        if (m_pAurObject != 0 && m_pAurObject->m_fBumpedOutShellHeight != 0.0f &&
            currentscene != 0)
        {

            GLRender::SetActiveTexture(0);
            ((CAurTexture*)m_pMaterial->m_pTexture3.m_pObject)->Bind();
            if (m_pMaterial->m_lstBlendSrc.m_nCount > 1)
            {
                m_pMaterial->SetBlendingMode(1);
            }

            VertexPrimitiveSkinned* pPrimitive =
                m_pVertexPrimitive->AsVertexPrimitiveSkinned();
            PartSkin* pSkin = pPrimitive->m_pPart->AsPartSkin();
            MdlNodeSkin* pNode = (MdlNodeSkin*)pSkin->m_pNode;
            if (pNode->m_nTextureCount != 0)
            {
                PartTriMesh* pMeshPart = pSkin->AsPartTriMesh();
                MdlNodeTriMesh* pTriMesh = 0;
                if (pMeshPart != 0)
                {
                    pTriMesh = (MdlNodeTriMesh*)pSkin->m_pNode;
                }
                pPrimitive->SetupVertexArraysStandard();
                static List<Quaternion> qbones(0);
                static List<Vector> tbones(0);
                qbones.m_nCount = 0;
                tbones.m_nCount = 0;
                Matrix mRotation;

                GLRender_Enable(ENABLE_VERTEX_PROGRAM);
                GLRender_EnableClientState(CLIENT_STATE_SKIN_WEIGHT);
                GLRender_EnableClientState(CLIENT_STATE_BONE_INDEX);
                AurSetVertexProgramParameter4f(SKIN_VP_CONSTANTS, 0.0f, 2.0f, 3.0f, 1.0f);
                AurSetVertexProgramParameter4f(SKIN_VP_FOG, currentscene->m_fFogNear,
                    currentscene->m_fFogFar, currentscene->m_bFog ? 1.0f : 0.0f, 0.5f);

                Part* pPart = pSkin;
                Vector vTranslation = -pPart->m_qOrientation * -pPart->m_vPosition;
                Quaternion qRotation = -pPart->m_qOrientation;
                while (pPart->m_pParent != 0)
                {
                    pPart = pPart->m_pParent;
                    vTranslation = vTranslation + qRotation *
                        (-pPart->m_qOrientation * -pPart->m_vPosition);
                    qRotation = qRotation * -pPart->m_qOrientation;
                }
                GatherBones(pPart, tbones, qbones, vTranslation, qRotation);
                for (int i = 0; i < pNode->m_nBoneMap; i++)
                {
                    int nBone = (int)pNode->m_pfBoneMap[i];
                    if (nBone != -1)
                    {
                        tbones[i] = tbones[i] + qbones[i] * pNode->m_lstTBoneRefInv[i];
                        qbones[i] = qbones[i] * pNode->m_lstQBoneRefInv[i];
                        mRotation = qbones[i].getmatrix();
                        Vector& vBoneTranslation = tbones[i];
                        int nRegister = SKIN_VP_BONES + 3 * nBone;
                        AurSetVertexProgramParameter4f(nRegister,
                            mRotation.m_vFirst.m_fX, mRotation.m_vSecond.m_fX,
                            mRotation.m_vThird.m_fX, vBoneTranslation.m_fX);
                        AurSetVertexProgramParameter4f(++nRegister,
                            mRotation.m_vFirst.m_fY, mRotation.m_vSecond.m_fY,
                            mRotation.m_vThird.m_fY, vBoneTranslation.m_fY);
                        AurSetVertexProgramParameter4f(++nRegister,
                            mRotation.m_vFirst.m_fZ, mRotation.m_vSecond.m_fZ,
                            mRotation.m_vThird.m_fZ, vBoneTranslation.m_fZ);
                    }
                }
                AurSetVertexProgramParameter4f(SKIN_VP_SHELL_HEIGHT,
                    m_pAurObject->m_fBumpedOutShellHeight, 0.0f, 0.0f, 1.0f);

                static VertexProgram* skinBumpedOutVP = new VertexProgram(
                    usearbvertexprograms ? szSkinBumpedOutVPARB : cSkinBumpedOutVP,
                    cSkinBumpedOutVPDeclaration);
                static VertexProgram* skinBumpedOutWithTangentVP = new VertexProgram(
                    usearbvertexprograms ? szSkinBumpedOutVPARB : cSkinBumpedOutVP,
                    cBumpyShinySkinVPDeclaration);
                VertexProgram* pProgram;
                if (pSkin->m_pMaterial->HasBumpMap() && pTriMesh->HasTangentBases())
                {
                    pProgram = skinBumpedOutWithTangentVP;
                }
                else
                {
                    pProgram = skinBumpedOutVP;
                }
                pProgram->Enable(pSkin, false);
                RenderFlat(pPrimitive);
                GLRender_DisableClientState(CLIENT_STATE_SKIN_WEIGHT);
                GLRender_DisableClientState(CLIENT_STATE_BONE_INDEX);
                GLRender_Disable(ENABLE_VERTEX_PROGRAM);
            }
        }
    }
#else
    if (renderskin != 0)
    {
        if (usevertexprograms != 0 && AurNV2xLevelShadersAvailable() &&
            currentscene != 0)
        {
            if (!bRenderLast)
            {
                RenderSkinnedVP((VertexPrimitiveSkinned*)
                    m_pVertexPrimitive->AsVertexPrimitiveSkinned());
            }
            else
            {
                RenderSkinnedVPBlur((VertexPrimitiveSkinned*)
                    m_pVertexPrimitive->AsVertexPrimitiveSkinned());
            }
        }
        else
        {
            GLRender_DisableClientState(CLIENT_STATE_COLOR);
            RenderSkinned((VertexPrimitiveSkinned*)
                m_pVertexPrimitive->AsVertexPrimitiveSkinned(), false);
        }

        if (m_pAurObject != 0 && m_pAurObject->m_fBumpedOutShellHeight != 0.0f &&
            currentscene != 0)
        {
            DrawBumpedOutShell();
        }
    }
#endif
}

#ifdef _XBOX

char* usebumpdiffuseonly(char* sArguments)
{
    usebumpdiffuse = 1;
    usebumpspecular = 0;
    return "Displaying only diffuse highlights on bumpmapped objects.";
}

char* usebumpspecularonly(char* sArguments)
{
    usebumpdiffuse = 0;
    usebumpspecular = 1;
    return "Displaying only specular highlights on bumpmapped objects.";
}

char* usebumpspecularanddiffuse(char* sArguments)
{
    usebumpdiffuse = 1;
    usebumpspecular = 1;
    return "Displaying both specular and diffuse highlights on bumpmapped objects.";
}
#endif
