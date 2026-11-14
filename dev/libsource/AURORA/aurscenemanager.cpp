#include "aurscenemanager.h"

#include "aurscene.h"
#include "math3d.h"
#include "nwcanimbase.h"
#include "aurmodel.h"
#include "aurgrass.h"
#include "aurtrigger.h"
#include "aurlightmanager.h"
#include "aurwindmanager.h"
#include "aurlist.h"
#include "aurcamera.h"
#include "aurrendergl.h"
#include "aurprimitive.h"
#include <math.h>
#include <string.h>
#include "aurconsole.h"
#include "aurdraw.h"

static inline void AurscenemanagerTrace3();
static inline void AurscenemanagerTrace2();
static inline void AurscenemanagerTrace1();
#define AURSCENEMANAGER_TRACE() AurscenemanagerTrace1()

float fadeToZeroTime = 0.25f;

float delayFactor = 0.1f;
int enablebboxtest = 0;

int enablespheretest = 0;
int countbinbucket = 0;
int countactivebins = 0;
int countmodels = 0;
int countculledpart = 0;
int countculledshadows = 0;
int countculledbeams = 0;
int countbackshadows = 0;
int countculledgobs = 0;
CHitInfo* CurrentHitInfo = 0;
int checkprimitive = 1;
int hitcheck_static_bsp_parts = 1;
int hitcheck_static_trimesh_parts = 0;
int hack_nobsp_hit_ground_zero = 0;
int enablenonbspgroundhit = 0;
int counthitcheckscenebspnode = 0;
int renderaabb = 0;
int shadowcull = 0;
int cullfoggeometry = 1;
int buildstaticshadows = 1;
float minshadowdim = 1.0f;
float minshadowarea = 1.0f;
float wedgeCloseFactor = 0.3333f;
float perpCloseFactor = 0.0f;
float fadeWidth = 1.0f;
float cameraepsilon = 0.3f;
int crap = 0;
int enablevisibilitygraph = 1;

int enablescenemanagement = 1;
int currentroomindex = -1;
int countlights;

#define AUR_MINIMUM_FADE_HORIZONTAL 0.4

#pragma inline_recursion(on)
#pragma inline_depth(10)

void LightNodeTile(BSPNode* pNode, void* pData);
void LightBins(BSPNode* pNode, void* pData);
int AurTextureGetMaxTexID();

bool g_bHasFadedGeometry;
extern int countbinbucket;
extern int countactivebins;
extern int countmodels;
extern int counthitcheckscenebspnode;
extern int hack_nobsp_hit_ground_zero;
extern int enablenonbspgroundhit;
extern int cullfoggeometry;
extern int countculledgobs;
extern int countculledshadows;
extern int countculledbeams;

char* GetToken(char** psLine)
{
#ifdef _XBOX
    char* sToken = *psLine;
    while (**psLine == ' ')
    {
        (*psLine)++;
    }
    if (**psLine == 0)
    {
        return NULL;
    }
    while (**psLine != ' ')
    {
        if (**psLine == 0)
            break;
        (*psLine)++;
    }
    if (**psLine == ' ')
    {
        **psLine = 0;
    }
    (*psLine)++;
    return sToken;
#else
    char* sLine = *psLine;
    while (*sLine == ' ')
    {
        *psLine = ++sLine;
    }
    if (*sLine == 0)
    {
        return NULL;
    }
    char* sToken = sLine;
    while (*sLine != 0 && *sLine != ' ')
    {
        *psLine = ++sLine;
    }
    if (*sLine == ' ')
    {
        *sLine = 0;
        ++*psLine;
    }
    return sToken;
#endif
}

#ifdef _XBOX
int checkprimitive = 1;
#endif

#ifdef _XBOX

void nodedata::ClearMovingObjects()
{
    m_lstMovingObjects.m_nCount = 0;
}

void nodedata::ClearMovingLights()
{
    m_lstMovingLights.m_nCount = 0;
}

void nodedata::ClearMovingEmitters()
{
    m_lstMovingEmitters.m_nCount = 0;
}

List<Gob*>& nodedata::GetMovingObjects()
{
    return m_lstMovingObjects;
}

List<PartLight*>& nodedata::GetMovingLights()
{
    return m_lstMovingLights;
}

List<PartEmitter*>& nodedata::GetMovingEmitters()
{
    return m_lstMovingEmitters;
}
#endif
// PC 0x00551580 GOG 0x005516A0
template <> inline List<CAurRoom*>& List<CAurRoom*>::operator=(List<CAurRoom*>& other)
{
    AURSCENEMANAGER_TRACE();
    m_nCount = 0;
    for (int i = 0; i < other.m_nCount; i++)
    {
        Add(other.m_pList[i]);
    }
    return *this;
}

#ifdef _XBOX

void nodedata::AddMovingLight(PartLight* pLight)
{
    m_lstMovingLights.Add(pLight);
}
#endif

static inline void AurscenemanagerTrace3()
{
    AurscenemanagerTrace2();
}

static inline void AurscenemanagerTrace2()
{
    AurscenemanagerTrace1();
}

static inline void AurscenemanagerTrace1()
{
}

#ifdef _XBOX

static void AddVisibleTriMeshes(nodedata* pNodeData, List<Plane>& planes)
{
    for (int i = 0; i < pNodeData->m_lstTriMeshes.m_nCount; i++)
    {
        PartTriMesh* pTriMesh = pNodeData->m_lstTriMeshes[i];
        if (!PartOutside(pTriMesh, planes))
        {
            AddPartToMeshBuckets(pTriMesh);
        }
    }
}
#endif

#ifdef _XBOX
Scene* FindScene(char* sName);

char* setbsphere(char* sArguments)
{
    char* sToken = GetToken(&sArguments);
    if (!sToken)
        return "0";
    Model* pModel = FindModel(sToken);
    sToken = GetToken(&sArguments);
    if (!sToken)
        return "0";
    pModel->m_fRadius = (float)atof(sToken);
    return "1";
}

char* setfadetoalways(char* sArguments)
{
    FindScene("mainscene");
    Scene::geomFadeEnabled = CAurScene::ALWAYS_FADE;
    return "SUCCESS";
}
#endif
// PC 0x004808F0 GOG 0x00480A70
int AurClearActiveBins()
{
    g_pActiveBins.m_nCount = 0;
    g_pOldActiveBins.m_nCount = 0;
    return 0;
}
// PC 0x00480940 GOG 0x00480AC0
static int NodeHasAABB(BSPNode* pNode, void* pData)
{
    if (pNode == 0)
    {
        return 0;
    }
    return pNode->m_pData->m_pAABB == pData;
}
// PC 0x00480960 GOG 0x00480AE0
TileData* SceneBSPGetTileData(Scene* pScene, const Vector& vPosition)
{
    if (pScene->m_pBSPRoot == 0)
    {
        return 0;
    }
    return BSPFitSphere(pScene->m_pBSPRoot, vPosition, 0.0f)
        ->m_pData->m_pTileData;
}
// PC 0x00480990 GOG 0x00480B10
int PartOutside(PartTriMesh* pTriMesh, List<Plane>& lstPlanes)
{
    static int lastClipped = 0;

    for (int i = 0; i < lstPlanes.m_nCount; i++)
    {
        int nPlane = (i + lastClipped) % lstPlanes.m_nCount;
        if (BoxAbovePlane(pTriMesh->m_vMinimum, pTriMesh->m_vMaximum,
                          lstPlanes[nPlane]) > 0.0f)
        {
            lastClipped = (lastClipped + i) % lstPlanes.m_nCount;
            return 1;
        }
    }

    return 0;
}
// PC 0x00480A10 GOG 0x00480B90
int PartOutside(PartEmitter* pEmitter, List<Plane>& lstPlanes)
{
    Vector vPosition;
    Quaternion qOrientation;

    pEmitter->GetWorld(vPosition, qOrientation);

    for (int i = 0; i < lstPlanes.m_nCount; i++)
    {
        if (SphereAbovePlane(vPosition, pEmitter->m_fRadius,
                             lstPlanes[i]) > 0.0f)
        {
            return 1;
        }
    }

    return 0;
}
// PC 0x00480AC0 GOG 0x00480C40
int PartOutside(PartProjection* pProjection, List<Plane>& lstPlanes)
{
    static int lastClipped = 0;

    for (int i = 0; i < lstPlanes.m_nCount; i++)
    {
        int nPlane = (i + lastClipped) % lstPlanes.m_nCount;
        if (BoxAbovePlane(pProjection->m_vMinimum, pProjection->m_vMaximum,
                          lstPlanes[nPlane]) > 0.0f)
        {
            lastClipped = i;
            return 1;
        }
    }

    return 0;
}
// PC 0x00480B30 GOG 0x00480CB0
int PartOutside(PartTrigger* pTrigger, List<Plane>& lstPlanes)
{
    static int lastClipped = 0;
    for (int i = 0; i < lstPlanes.m_nCount; i++)
    {
        int nPlane = (i + lastClipped) % lstPlanes.m_nCount;
        if (BoxAbovePlane(pTrigger->m_vMinimum, pTrigger->m_vMaximum,
                          lstPlanes[nPlane]) > 0.0f)
        {
            lastClipped = i;
            return 1;
        }
    }
    return 0;
}
// PC 0x00480BA0 GOG 0x00480D20
int GobOutside(Gob* pGob, List<Plane>& lstPlanes)
{
    if (pGob->IsInViewVolume())
    {
        return 1;
    }

    for (int i = 0; i < lstPlanes.m_nCount; i++)
    {
        if (enablebboxtest == 0)
        {
            Vector vPosition =
                pGob->m_vPosition +
                pGob->m_qOrientation * pGob->m_vMinimumBoundingSphere;
            if (SphereAbovePlane(vPosition,
                                 pGob->m_fMinimumBoundingSphereRadius,
                                 lstPlanes[i]) > 0.0f)
            {
                return 1;
            }
        }
    }

    return 0;
}
// PC 0x00480DC0 GOG 0x00480F40
int BinOutside(CAurTriangleBin* pBin, List<Plane>& lstPlanes)
{
    for (int i = 0; i < lstPlanes.m_nCount; i++)
    {
        if (SphereAbovePlane(pBin->m_vMaximum, pBin->m_fRadius,
                             lstPlanes[i]) > 0.0f)
        {
            return 1;
        }
    }
    return 0;
}
// PC 0x00480E20 GOG 0x00480FA0
void LightPartTriMesh(PartTriMesh* pPart, List<PartLight*> lstLights, Scene* pScene)
{
    if (pPart == 0)
        return;
    if (pPart->m_pTileData != 0 && pPart->m_pTileData->m_bSuppliesMaterial != 0)
        return;

    Vector vPosition;
    Quaternion qOrientation;
    Matrix mTransform;
    MdlNodeTriMesh* pMesh = (MdlNodeTriMesh*)pPart->m_pVertexPrimitive->m_pNode;
    if (pMesh->m_nVertexCount == 0)
        return;

    if (lstLights.m_nCount == 0 && equal(pPart->m_vSelfIllumColor, Vector()))
    {
        if (pPart->m_bColors == 1)
        {
            unsigned long nOldColorPool = pPart->m_nColorPool;
            GLRender::DeletePool(nOldColorPool);
            pPart->m_nColorPool = RequestPoolManager::INVALID_POOL;
            pPart->m_bColors = 0;
            pPart->m_pVertexPrimitive->Rebuild();
        }
        return;
    }
    if (pPart->m_bColors == 0)
    {
        if (pMesh->m_nVertexCount != 0)
        {
            int nColorBytes = (pMesh->m_nVertexCount + 1) * sizeof(unsigned long);
            GLRender::RequestPool(Aurora::POOL_VERTEX,
                                  nColorBytes,
                                  RequestPoolManager::POOL_STATIC | RequestPoolManager::POOL_COLOR,
                                  0, &pPart->m_nColorPool);
            pPart->m_bColors = 1;
            pPart->m_pVertexPrimitive->Rebuild();
        }
        else
            return;
    }

    pPart->GetWorld(vPosition, qOrientation);
    AurDrawLine(vPosition, vPosition, 0);
    unsigned long nColorPool = pPart->m_nColorPool;
    unsigned long* pColors = (unsigned long*)GLRender::LockPool(
        nColorPool, 0, 0, RequestPoolManager::POOL_LOCK_WRITE);
    Model* pModel = pMesh->m_pModel->AsModel();
    unsigned long nVertexPool = pModel->m_nVertexPool;
#ifdef _XBOX
    unsigned char* pVertices = (unsigned char*)GLRender::LockPool(
        nVertexPool, 0, 0,
        RequestPoolManager::POOL_LOCK_WRITE);
#else
    unsigned char* pVertices = (unsigned char*)GLRender::LockPool(
        nVertexPool, 0, 0,
        RequestPoolManager::POOL_LOCK_READ_WRITE);
#endif
    unsigned long nVertexDataOffset = pMesh->m_nVertexDataOffset;
    for (int i = 0; i < pMesh->m_nVertexCount; i++)
    {
        float* pPosition = (float*)(pVertices + nVertexDataOffset +
                                   i * pMesh->m_nVertexStride + pMesh->m_nPositionOffset);
        Vector vVertex(pPosition[0], pPosition[1], pPosition[2]);
        float* pNormal = (float*)(pVertices + nVertexDataOffset +
                                 i * pMesh->m_nVertexStride + pMesh->m_nNormalOffset);
        Vector vNormal(pNormal[0], pNormal[1], pNormal[2]);
        Vector vWorldVertex = vPosition + qOrientation * vVertex;
        Vector vWorldNormal = qOrientation * vNormal;
        static Vector accumulator;
        accumulator = Vector();
        if (pPart->m_pMaterial->m_bDecal)
            accumulator = Vector(1.0f, 1.0f, 1.0f);
        else
            accumulator = pPart->m_vSelfIllumColor;

        for (int j = 0; j < lstLights.m_nCount; j++)
        {
            PartLight* pLight = lstLights.m_pList[j];
            MdlNodeLight* pLightNode = (MdlNodeLight*)pLight->m_pNode;
            if (pLight->m_fRadius == 0.0f)
                continue;
            float fDistance = magnitude(pLight->m_vWorldPosition - vWorldVertex);
            if (fDistance == 0.0f)
                fDistance = 0.000001f;
            float fAttenuation = 1.0f - fDistance / pLight->m_fRadius;
            if (fAttenuation < 0.0f)
                continue;
            Vector vDirection = (pLight->m_vWorldPosition - vWorldVertex) * (1.0f / fDistance);
            float fIntensity = 1.0f;
            if (!pLightNode->m_bAmbientOnly)
            {
                fIntensity = vDirection * vWorldNormal;
                if (fIntensity < 0.0f)
                    continue;
            }
            Vector vMaterialColor = pLightNode->m_bAmbientOnly ? pMesh->m_vAmbient : pMesh->m_vDiffuse;
            Vector vLightColor = pLight->m_vColor * (fIntensity * fAttenuation);
            Vector vContribution = vLightColor;
            vContribution.m_fX *= vMaterialColor.m_fX;
            vContribution.m_fY *= vMaterialColor.m_fY;
            vContribution.m_fZ *= vMaterialColor.m_fZ;
            accumulator = vContribution + accumulator;
        }
        if (pMesh->m_nColorOffset != -1)
        {
            unsigned char* pColor = pVertices + nVertexDataOffset +
                i * pMesh->m_nVertexStride + pMesh->m_nColorOffset;
            accumulator.m_fX *= pColor[0] / 255.0f;
            accumulator.m_fY *= pColor[1] / 255.0f;
            accumulator.m_fZ *= pColor[2] / 255.0f;
        }
        accumulator.m_fX = min(max(accumulator.m_fX, 0.0f), 1.0f);
        accumulator.m_fY = min(max(accumulator.m_fY, 0.0f), 1.0f);
        accumulator.m_fZ = min(max(accumulator.m_fZ, 0.0f), 1.0f);
        pColors[i] = AUR_PACKED_COLOR_OPAQUE_ALPHA |
                    ((unsigned char)(accumulator.m_fZ * 255.0f + 0.5f) << 16) |
                    ((unsigned char)(accumulator.m_fY * 255.0f + 0.5f) << 8) |
                    (unsigned char)(accumulator.m_fX * 255.0f + 0.5f);
    }
    nColorPool = pPart->m_nColorPool;
    GLRender::UnlockPool(nColorPool);
    GLRender::UnlockPool(pMesh->m_pModel->AsModel()->m_nVertexPool);
#ifdef _XBOX

    if (checkprimitive)
    {
        VertexPrimitive* pPrimitive = pPart->m_pVertexPrimitive;
        if (pPrimitive)
            pPrimitive->DeleteArrayPool();
    }

    else
        pPart->m_pVertexPrimitive->DeleteArrayPool();
#endif
}

Bucket meshbuckets[AUR_MESH_BUCKET_COUNT];
Bucket fadebuckets[AUR_MESH_BUCKET_COUNT];
Bucket capbuckets[AUR_MESH_BUCKET_COUNT];
Bucket g_listBackgroundBucket;
List<Part*> staticshadowbucket;
List<Part*> staticbeambucket;
List<PartTriMesh*> meshshadowbucket;
List<PartEmitter*> emitterbucket;
List<Gob*> gobbucket;
List<Gob*> g_listForcePushBucket;
List<Gob*> g_listLightsaberBucket;
List<Gob*> g_listHologramBucket;
List<BSPNode*> staticlightbucket;
List<PartTrigger*> triggerbucket;
List<CAurTriangleBin*> binbucket;
List<Gob*> blurbucket;

List<Gob*> fadinginbucket;
List<Gob*> fadingoutbucket;
List<CAurTriangleBin*> g_pActiveBins, g_pOldActiveBins;
// PC 0x00481940 GOG 0x00481AC0
nodedata::~nodedata()
{
    if (m_pTileData != 0)
    {
        delete m_pTileData;
    }
}
// PC 0x00481AA0 GOG 0x00481C20
static int NodeHasLight(BSPNode* pNode, void* pData)
{
    if (pNode == 0)
    {
        return 0;
    }
    return pNode->m_pData->m_lstLights.Contains((PartLight*)pData);
}
// PC 0x00481AD0 GOG 0x00481C50
static int NodeHasEmitter(BSPNode* pNode, void* pData)
{
    if (pNode == 0)
    {
        return 0;
    }
    return pNode->m_pData->m_lstEmitters.Contains((PartEmitter*)pData);
}
// PC 0x00481B00 GOG 0x00481C80
static int NodeHasProjection(BSPNode* pNode, void* pData)
{
    if (pNode == 0)
    {
        return 0;
    }
    nodedata* pNodeData = pNode->m_pData;
    return pNodeData->m_lstShadowProjections.Contains((PartShadow*)pData) ||
           pNodeData->m_lstBeamProjections.Contains((PartBeam*)pData);
}
// PC 0x00481B60 GOG 0x00481CE0
static int NodeHasTriMesh(BSPNode* pNode, void* pData)
{
    if (pNode == 0)
    {
        return 0;
    }
    return pNode->m_pData->m_lstTriMeshes.Contains((PartTriMesh*)pData);
}
// PC 0x00481B90 GOG 0x00481D10
static int NodeHasTrigger(BSPNode* pNode, void* pData)
{
    if (pNode == 0)
    {
        return 0;
    }
    return pNode->m_pData->m_lstTriggers.Contains((PartTrigger*)pData);
}
// PC 0x00481BC0 GOG 0x00481D40
static int NodeHasOther(BSPNode* pNode, void* pData)
{
    if (pNode == 0)
    {
        return 0;
    }
    return pNode->m_pData->m_lstOthers.Contains((Part*)pData);
}
// PC 0x00481BF0 GOG 0x00481D70
static int NodeHasGob(BSPNode* pNode, void* pData)
{
    if (pNode == 0)
    {
        return 0;
    }
    return pNode->m_pData->m_lstGobs.Contains((Gob*)pData);
}
// PC 0x00481C20 GOG 0x00481DA0
int HitPartOnBSPNode(BSPNode* pNode, Vector vStart, Vector vEnd, Vector* pHit)
{
    counthitcheckscenebspnode++;
    nodedata* pNodeData = pNode->m_pData;
    int bHit = 0;

    if (pNodeData == 0)
    {
        return 0;
    }

    for (int i = 0; i < pNodeData->m_lstTriggers.m_nCount; i++)
    {
        PartTrigger* pTrigger = pNodeData->m_lstTriggers[i];
        if (BoxIntersect(vStart, vEnd, pTrigger->m_vMinimum, pTrigger->m_vMaximum))
        {
            if (pTrigger->HitCheck(vStart, vEnd, &vEnd, CurrentHitInfo, 1.0f))
            {
                bHit = 1;
                if (pHit)
                {
                    *pHit = vEnd;
                }
            }
        }
    }

    if (hitcheck_static_trimesh_parts)
    {
        for (int i = 0; i < pNodeData->m_lstTriMeshes.m_nCount; i++)
        {
            if (BoxIntersect(vStart, vEnd,
                             pNodeData->m_lstTriMeshes[i]->m_vMinimum,
                             pNodeData->m_lstTriMeshes[i]->m_vMaximum))
            {
                if (pNodeData->m_lstTriMeshes[i]->HitCheck(
                        vStart, vEnd, &vEnd, CurrentHitInfo, 1.0f))
                {
                    bHit = 1;
                    if (pHit)
                    {
                        *pHit = vEnd;
                    }
                }
            }
        }
    }

    hack_nobsp_hit_ground_zero = 0;
    if (hitcheck_static_bsp_parts)
    {
        if (pNodeData->m_pAABB)
        {
            bHit |= pNodeData->m_pAABB->HitCheck(
                vStart, vEnd, pHit, CurrentHitInfo, 1.0f) != 0;
            return bHit;
        }
        else if (enablenonbspgroundhit && pNode->m_pPositive == 0 &&
                 pNode->m_pNegative == 0 && vEnd.m_fZ < 0.0f && vStart.m_fZ > 0.0f)
        {
            if (pHit)
            {
                *pHit = planelineintersection(Vector(0.0f, 0.0f, 1.0f),
                                              0.0f, vStart, vEnd);
            }
            hack_nobsp_hit_ground_zero = 1;
            return 1;
        }
    }
    return bHit;
}
// PC 0x00481EF0 GOG 0x00482070
void LightBins(BSPNode* pNode, void* pData)
{
    nodedata* pNodeData = pNode->m_pData;

    for (int i = 0; i < pNodeData->m_lstBins.m_nCount; i++)
    {
        pNodeData->m_lstBins[i]->ReloadColor();
    }
}
// PC 0x00481F20 GOG 0x004820A0
static void ConstructCameraFadePlane(Scene* pScene)
{
    AurscenemanagerTrace2();
    CAurBehaviorCameraFollow* pFollow = 0;
    if (CurrentCamera && CurrentCamera->m_pBehavior)
    {
        pFollow = CurrentCamera->m_pBehavior->AsBehaviorCameraFollow();
    }
    if (pFollow == 0)
    {
        pScene->m_nCameraFadeState = Scene::FADING_OFF;
        return;
    }

    Vector vTarget = pFollow->GetTargetWorldPosition();
    Vector vCamera = CurrentCamera->GetPosition();
    Vector vForward = vTarget - vCamera;
    Vector vDirection = normalize(vForward);
    float fHorizontal = sqrt(vDirection.m_fX * vDirection.m_fX +
                             vDirection.m_fY * vDirection.m_fY);
    if (fHorizontal < AUR_MINIMUM_FADE_HORIZONTAL)
    {
        pScene->m_nCameraFadeState = Scene::FADING_OFF;
    }
    else
    {
        vDirection.m_fZ = 0.0f;
        vForward.m_fZ = 0.0f;
        static Vector vertZ(0.0f, 0.0f, 1.0f);
        Vector vRight = Vector(-vDirection.m_fY, vDirection.m_fX, 0.0f) * fadeWidth;
        Vector vLeft = -vRight;
        Vector vEdge = vForward * (1.0f - wedgeCloseFactor) - vRight;
        Vector vNormalRight = normalize(cross(vertZ, vEdge));
        Plane planeRight(vNormalRight, -(vNormalRight * (vRight + vCamera)));
        vEdge = vForward * (1.0f - wedgeCloseFactor) - vLeft;
        Vector vNormalLeft = normalize(cross(vEdge, vertZ));
        Plane planeLeft(vNormalLeft, -(vNormalLeft * (vLeft + vCamera)));
        vDirection = vDirection * (1.0f / fHorizontal);
        pScene->m_nCameraFadeState = Scene::FADING_ON;
        CurrentCamera->m_planeFadePerpendicular = Plane(vDirection,
            -((vTarget - vForward * perpCloseFactor) * vDirection));
        CurrentCamera->SetFadePlanes(planeRight, planeLeft);
    }
}
// PC 0x00482330 GOG 0x004824B0
void AurProcessActiveBins()
{
    for (int i = 0; i < g_pActiveBins.m_nCount; i++)
    {
        g_pActiveBins[i]->BuildGrassPolys();
    }

    for (int i = 0; i < g_pOldActiveBins.m_nCount; i++)
    {
        CAurTriangleBin* pBin = g_pOldActiveBins[i];
        bool bActive = false;
        for (int j = 0; j < g_pActiveBins.m_nCount; j++)
        {
            if (g_pActiveBins[j] == pBin)
            {
                bActive = true;
                break;
            }
        }
        if (!bActive)
        {
            pBin->DestroyGrassPolys();
        }
    }
}
// PC 0x004823B0 GOG 0x00482530
void ClearSceneRooms(Scene* pScene)
{
    for (int i = 0; i < pScene->m_lstRooms.m_nCount; i++)
    {
        nodedata* pNodeData = pScene->m_lstRooms[i]->m_pData;
#ifdef _XBOX
        pNodeData->ClearMovingLights();
        pNodeData->ClearMovingObjects();
        pNodeData->ClearMovingEmitters();
#else
        pNodeData->m_lstMovingLights.m_nCount = 0;
        pNodeData->m_lstMovingObjects.m_nCount = 0;
        pNodeData->m_lstMovingEmitters.m_nCount = 0;
#endif
    }
}
// PC 0x00482450 GOG 0x004825B0
void AddPartToMeshBuckets(PartTriMesh* pTriMesh)
{
    PartDanglyMesh* pDanglyMesh = pTriMesh->AsPartDanglyMesh();
    if (pDanglyMesh != 0)
    {
        pDanglyMesh->Animate(DeltaT);
    }

    if (pTriMesh->m_pNode->AsMdlNodeTriMesh()->m_bBackground)
    {
        g_listBackgroundBucket.Add(pTriMesh);
    }
    else
    {
        meshbuckets[pTriMesh->m_pMaterial->GetTextureTID()].Add(pTriMesh);

        meshshadowbucket.Add(pTriMesh);
    }
}
// PC 0x00482550 GOG 0x004826B0
void ClearBuckets(Scene* pScene)
{
    int nMaxTexID = AurTextureGetMaxTexID();

    gobbucket.m_nCount = 0;
    g_listForcePushBucket.m_nCount = 0;
    g_listLightsaberBucket.m_nCount = 0;
    blurbucket.m_nCount = 0;
    emitterbucket.m_nCount = 0;
    g_listHologramBucket.m_nCount = 0;

    for (int i = 0; i <= nMaxTexID; i++)
    {
        meshbuckets[i].m_nCount = 0;
        fadebuckets[i].m_nCount = 0;
        capbuckets[i].m_nCount = 0;
    }

    g_listBackgroundBucket.m_nCount = 0;
    staticshadowbucket.m_nCount = 0;
    staticbeambucket.m_nCount = 0;
    meshshadowbucket.m_nCount = 0;
    triggerbucket.m_nCount = 0;
    staticlightbucket.m_nCount = 0;
    binbucket.m_nCount = 0;
    fadingoutbucket.m_nCount = 0;
    fadinginbucket.m_nCount = 0;
    g_pOldActiveBins = g_pActiveBins;
    g_pActiveBins.m_nCount = 0;

    countbinbucket = 0;
    countactivebins = 0;
    countmodels = Models.m_nCount;
}
// PC 0x00482600 GOG 0x00482760
nodedata::nodedata()
    : m_lstLights(0),
      m_lstGobs(0),
      m_lstTriMeshes(0),
      m_lstEmitters(0),
      m_lstShadowProjections(0),
      m_lstBeamProjections(0),
      m_lstOthers(0),
      m_lstTriggers(0),
      m_lstBins(0),
      m_lstMovingObjects(0),
      m_lstMovingLights(0),
      m_lstMovingEmitters(0)
{

    TileData* pTileData = 0;
    m_pAABB = 0;
    m_pTileData = pTileData;
}
// PC 0x004826B0 GOG 0x00482810
void SceneBSPGetObjectTextures(Part* pPart, List<CAurTexture*>& lstTextures)
{
    if (pPart != 0)
    {
        PartTriMesh* pTriMesh = pPart->AsPartTriMesh();
        if (pTriMesh != 0)
        {
            lstTextures.Add((CAurTexture*)pTriMesh->m_pMaterial->m_pTexture0.m_pObject);
        }
        for (int i = 0; i < pPart->m_lstChildren.m_nCount; i++)
        {
            SceneBSPGetObjectTextures(pPart->m_lstChildren[i], lstTextures);
        }
    }
}
// PC 0x004829D0 GOG 0x00482B30
void SceneBSPGetTileTextures(Scene* pScene, const Vector& vPosition,
                             List<CAurTexture*>& lstTextures)
{
    TileData* pTileData = SceneBSPGetTileData(pScene, vPosition);
    lstTextures.m_nCount = 0;
    if (pTileData != 0)
    {
        for (int i = 0; i < pTileData->m_lstParts.m_nCount; i++)
        {
            PartTriMesh* pTriMesh = pTileData->m_lstParts[i]->AsPartTriMesh();
            if (pTriMesh != 0)
            {
                lstTextures.Add((CAurTexture*)pTriMesh->m_pMaterial->m_pTexture0.m_pObject);
            }
        }
        if (pTileData->m_pStaticObject != 0)
        {
            SceneBSPGetObjectTextures(pTileData->m_pStaticObject->m_pRootPart, lstTextures);
        }
    }
}
// PC 0x00482A70 GOG 0x00482BD0
void SceneBSPAddGob(Scene* pScene, Gob* pGob)
{
    if (pScene->m_pBSPRoot == 0)
    {
        return;
    }

    BSPNode* pNode = BSPFitSphere(pScene->m_pBSPRoot, pGob->m_vPosition,
                                  pGob->m_pModel->m_fRadius);
    if (!pNode->m_pData->m_lstGobs.Contains(pGob))
    {
        pNode->m_pData->m_lstGobs.Add(pGob);
    }
}
// PC 0x00482AF0 GOG 0x00482C50
void SceneBSPRemoveGob(Scene* pScene, Gob* pGob)
{
    if (pScene->m_pBSPRoot == 0)
    {
        return;
    }

    BSPNode* pNode = BSPSearch(pScene->m_pBSPRoot, pGob->m_vPosition, NodeHasGob, pGob);
    if (pNode == 0)
    {
        return;
    }
    if (pNode->m_pData->m_lstGobs.Contains(pGob))
    {
        pNode->m_pData->m_lstGobs.Remove(pGob);
    }
}
// PC 0x00482B60 GOG 0x00482CC0
void SceneBSPRemovePart(Scene* pScene, Part* pPart)
{
    if (pScene->m_pBSPRoot == 0)
    {
        return;
    }

    Vector vPosition;
    Quaternion qOrientation;
    pPart->GetWorld(vPosition, qOrientation);

    PartLight* pLight = pPart->AsPartLight();
    if (pLight != 0)
    {
        BSPNode* pNode = BSPSearch(pScene->m_pBSPRoot, vPosition,
                                    NodeHasLight, pLight);
        if (pNode->m_pData->m_lstLights.Contains(pLight))
        {
            pNode->m_pData->m_lstLights.Remove(pLight);
        }
        pScene->m_pLightManager->RemoveLight(pLight, false);
        return;
    }

    PartEmitter* pEmitter = pPart->AsPartEmitter();
    if (pEmitter != 0)
    {
        nodedata* pNodeData = BSPSearch(pScene->m_pBSPRoot, vPosition,
                                      NodeHasEmitter, pEmitter)->m_pData;
        if (pNodeData->m_lstEmitters.Contains(pEmitter))
        {
            pNodeData->m_lstEmitters.Remove(pEmitter);
        }
        return;
    }

    PartProjection* pProjection = pPart->AsPartProjection();
    if (pProjection != 0)
    {
        nodedata* pNodeData = BSPSearch(pScene->m_pBSPRoot, vPosition,
                                      NodeHasProjection, pProjection)->m_pData;
        PartShadow* pShadow = pProjection->AsPartShadow();
        if (pShadow != 0)
        {
            if (pNodeData->m_lstShadowProjections.Contains(pShadow))
            {
                pNodeData->m_lstShadowProjections.Remove(pShadow);
            }
        }
        else
        {
            PartBeam* pBeam = pProjection->AsPartBeam();
            if (pBeam != 0)
            {
                if (pNodeData->m_lstBeamProjections.Contains(pBeam))
                {
                    pNodeData->m_lstBeamProjections.Remove(pBeam);
                }
            }
        }
        return;
    }

    PartAABB* pAABB = pPart->AsPartAABB();
    if (pAABB != 0)
    {
        nodedata* pNodeData = BSPSearch(pScene->m_pBSPRoot, vPosition,
                                      NodeHasAABB, pAABB)->m_pData;
        if (pNodeData->m_pAABB == pAABB)
        {
            pNodeData->m_pAABB = 0;
        }
        return;
    }

    PartTriMesh* pTriMesh = pPart->AsPartTriMesh();
    if (pTriMesh != 0)
    {
        nodedata* pNodeData = BSPSearch(pScene->m_pBSPRoot, vPosition,
                                      NodeHasTriMesh, pTriMesh)->m_pData;
        if (pNodeData->m_lstTriMeshes.Contains(pTriMesh))
        {
            pNodeData->m_lstTriMeshes.Remove(pTriMesh);
        }
        return;
    }

    PartTrigger* pTrigger = pPart->AsPartTrigger();
    if (pTrigger != 0)
    {
        nodedata* pNodeData = BSPSearch(pScene->m_pBSPRoot, vPosition,
                                      NodeHasTrigger, pTrigger)->m_pData;
        if (pNodeData->m_lstTriggers.Contains(pTrigger))
        {
            pNodeData->m_lstTriggers.Remove(pTrigger);
        }
        return;
    }

    nodedata* pNodeData = BSPSearch(pScene->m_pBSPRoot, vPosition,
                                  NodeHasOther, pPart)->m_pData;
    if (pNodeData->m_lstOthers.Contains(pPart))
    {
        pNodeData->m_lstOthers.Remove(pPart);
    }
}
// PC 0x00482EA0 GOG 0x00483000
void DeleteNonGobParts(BSPNode* pNode, void* pData)
{
    nodedata* pNodeData = pNode->m_pData;

    for (int i = pNodeData->m_lstShadowProjections.m_nCount; i != 0; )
    {
        if (((Part*)pNodeData->m_lstShadowProjections.m_pList[--i])->m_pAurObject == 0)
        {
            delete (Part*)pNodeData->m_lstShadowProjections.m_pList[i];
        }
        pNodeData->m_lstShadowProjections.m_nCount--;
    }

    for (int i = pNodeData->m_lstBeamProjections.m_nCount; i != 0; )
    {
        if (((Part*)pNodeData->m_lstBeamProjections.m_pList[--i])->m_pAurObject == 0)
        {
            delete (Part*)pNodeData->m_lstBeamProjections.m_pList[i];
        }
        pNodeData->m_lstBeamProjections.m_nCount--;
    }

    for (int i = pNodeData->m_lstLights.m_nCount; i != 0; )
    {
        if (pNodeData->m_lstLights.m_pList[--i]->m_pAurObject == 0)
        {
            Model* pModel = pNodeData->m_lstLights.m_pList[i]->m_pNode->m_pModel->AsModel();
            if (pModel != 0)
            {
                pModel->DelRef(pModel);
            }
            delete pNodeData->m_lstLights.m_pList[i];
        }
        pNodeData->m_lstLights.m_nCount--;
    }

    for (int i = pNodeData->m_lstTriMeshes.m_nCount; i != 0; )
    {
        if (pNodeData->m_lstTriMeshes.m_pList[--i]->m_pAurObject == 0)
        {
            Model* pModel = pNodeData->m_lstTriMeshes.m_pList[i]->m_pNode->m_pModel->AsModel();
            if (pModel != 0)
            {
                pModel->DelRef(pModel);
            }
            delete pNodeData->m_lstTriMeshes.m_pList[i];
        }
        pNodeData->m_lstTriMeshes.m_nCount--;
    }

    for (int i = pNodeData->m_lstEmitters.m_nCount; i != 0; )
    {
        if (pNodeData->m_lstEmitters.m_pList[--i]->m_pAurObject == 0)
        {
            Model* pModel = pNodeData->m_lstEmitters.m_pList[i]->m_pNode->m_pModel->AsModel();
            if (pModel != 0)
            {
                pModel->DelRef(pModel);
            }
            delete pNodeData->m_lstEmitters.m_pList[i];
        }
        pNodeData->m_lstEmitters.m_nCount--;
    }

    for (int i = pNodeData->m_lstOthers.m_nCount; i != 0; )
    {
        if (((Part*)pNodeData->m_lstOthers.m_pList[--i])->m_pAurObject == 0)
        {
            Model* pModel = ((Part*)pNodeData->m_lstOthers.m_pList[i])->m_pNode->m_pModel->AsModel();
            if (pModel != 0)
            {
                pModel->DelRef(pModel);
            }
            delete (Part*)pNodeData->m_lstOthers.m_pList[i];
        }
        pNodeData->m_lstOthers.m_nCount--;
    }

    for (int i = pNodeData->m_lstBins.m_nCount; i != 0; )
    {
        delete pNodeData->m_lstBins.m_pList[--i];
        pNodeData->m_lstBins.m_nCount--;
    }

    delete pNodeData;
}
// PC 0x00483080 GOG 0x004831E0
void UpdateStaticLighting(Scene* pScene, PartLight* pLight)
{
    if (pLight->m_nLightType == PartLight::LIGHTTYPE_STATIC)
    {
        List<BSPNode*> lstNodes;
        if (pScene->m_pBSPRoot != 0)
        {
            BSPSphereCheck(pScene->m_pBSPRoot, pLight->m_vWorldPosition,
                           pLight->m_fRadius, lstNodes);
            for (int i = 0; i < lstNodes.m_nCount; i++)
            {
                staticlightbucket.AddUnique(lstNodes.m_pList[i]);
            }
        }
    }
}
// PC 0x00483130 GOG 0x00483290
void CollectBins(BSPNode* pNode, const Vector& vPosition, float fRangeSquared)
{
    nodedata* pNodeData = pNode->m_pData;
    int nBins = pNodeData->m_lstBins.m_nCount;

    for (int i = 0; i < nBins; i++)
    {
        CAurTriangleBin* pBin = pNodeData->m_lstBins.m_pList[i];
        Vector vDelta;
        vDelta = vPosition - pBin->m_vMaximum;
        float fDistance = magnitude(vDelta) - pBin->m_fRadius;
        if (fDistance < 0.0f)
        {
            fDistance = 0.0f;
        }
#ifdef _XBOX
        fDistance = fDistance * fDistance;
        if (fDistance < fRangeSquared)
        {
            countactivebins++;
            g_pActiveBins.Add(pBin);
            pBin->SetMinDistanceSquared(fDistance);
        }
#else
        float fDistanceSquared = fDistance * fDistance;
        if (fDistanceSquared < fRangeSquared)
        {
            countactivebins++;
            g_pActiveBins.Add(pBin);
            pBin->SetMinDistanceSquared(fDistanceSquared);
        }
#endif
    }
}
// PC 0x00483250 GOG 0x004833B0
void LightManager::CollectLightsForGivenRoom(CAurRoom* pRoom)
{
    m_lstNearbyLights.m_nCount = 0;
    m_lstLensFlareLights.m_nCount = 0;
    List<CAurRoom*> lstRooms;
    if (pRoom != 0)
    {
        lstRooms = pRoom->m_lstVisibleRooms;
        lstRooms.Add(pRoom);
    }
    else
    {
        lstRooms = currentscene->m_lstRooms;
    }

    for (int i = 0; i < lstRooms.m_nCount; i++)
    {
        CAurRoom* pCurrentRoom = lstRooms[i];
        nodedata* pNodeData = pCurrentRoom->m_pData;
        for (int j = 0; j < pNodeData->m_lstLights.m_nCount; j++)
        {
            PartLight* pLight = pNodeData->m_lstLights[j];
            if (((MdlNodeLight*)pLight->m_pNode)->m_lstFlareColorShifts.m_nCount > 0)
            {
                m_lstLensFlareLights.AddUnique(pLight);
            }
            if (!pLight->m_vColor.IsZero())
            {
                m_lstNearbyLights.Add(pLight);
            }
        }

        for (int k = 0; k < pNodeData->m_lstMovingLights.m_nCount; k++)
        {
            PartLight* pLight = pNodeData->m_lstMovingLights[k];
            if (((MdlNodeLight*)pLight->m_pNode)->m_lstFlareColorShifts.m_nCount > 0)
            {
                m_lstLensFlareLights.AddUnique(pLight);
            }
            if (!pLight->m_vColor.IsZero())
            {
                m_lstNearbyLights.AddUnique(pLight);
            }
        }
    }
}
// PC 0x00483670 GOG 0x004837D0
void CollectActiveRooms(Scene* pScene, List<CAurRoom*>& lstRooms)
{
    AurscenemanagerTrace2();
    if (pScene->m_pCurrentRoom == 0)
    {
        lstRooms = pScene->m_lstRooms;
        return;
    }

    lstRooms.m_nCount = 0;

    if (pScene->m_bIgnoreVisibilityGraph == 0 && enablevisibilitygraph != 0)
    {
        lstRooms.Add(pScene->m_pCurrentRoom);
        List<Plane>& lstPlanes = CurrentCamera->m_lstViewVolume;
        for (int i = 0;
             i < pScene->m_pCurrentRoom->m_lstVisibleRooms.m_nCount; i++)
        {
            CAurRoom* pRoom =
                pScene->m_pCurrentRoom->m_lstVisibleRooms.m_pList[i];
            bool bVisible = true;
            if (!pRoom->m_bB8)
            {
                for (int j = 0; j < lstPlanes.m_nCount; j++)
                {
                    if (BoxAbovePlane(pRoom->m_vMinimum, pRoom->m_vMaximum,
                                      lstPlanes.m_pList[j]) > 0.0f)
                    {
                        bVisible = false;
                        break;
                    }
                }
            }
            if (bVisible)
                lstRooms.Add(pRoom);

        }
    }
    else
    {
        lstRooms = pScene->m_lstRooms;
    }
}
// PC 0x004837C0 GOG 0x00483920
void SceneAddPartToRooms(Scene* pScene, Part* pPart)
{
    PartTrigger* pTrigger = pPart->AsPartTrigger();
    for (int i = 0; i < pScene->m_lstRooms.m_nCount; i++)
    {
        CAurRoom* pRoom = pScene->m_lstRooms.m_pList[i];
        if (pTrigger != 0)
        {
            Vector vCenter = (pTrigger->m_vMinimum + pTrigger->m_vMaximum) / 2.0f;
            if (vCenter.m_fX >= pRoom->m_vMinimum.m_fX &&
                vCenter.m_fY >= pRoom->m_vMinimum.m_fY &&
                vCenter.m_fX <= pRoom->m_vMaximum.m_fX &&
                vCenter.m_fY <= pRoom->m_vMaximum.m_fY)
            {
                pRoom->m_pData->m_lstTriggers.Add(pTrigger);
            }
        }
    }
}
// PC 0x00483870 GOG 0x004839D0
void SceneRemovePartFromRooms(Scene* pScene, Part* pPart)
{
    PartTrigger* pTrigger = pPart->AsPartTrigger();
    for (int i = 0; i < pScene->m_lstRooms.m_nCount; i++)
    {
        nodedata* pNodeData = pScene->m_lstRooms.m_pList[i]->m_pData;
        if (pTrigger != 0)
        {
            if (pNodeData->m_lstTriggers.Contains(pTrigger))
            {
                pNodeData->m_lstTriggers.Remove(pTrigger);
            }
        }
    }
}
// PC 0x004838E0 GOG 0x00483A40
void nodedata::AddMovingObject(Gob* pGob)
{
    m_lstMovingObjects.Add(pGob);
}
// PC 0x00483920 GOG 0x00483A80
void nodedata::AddMovingEmitter(PartEmitter* pEmitter)
{
    m_lstMovingEmitters.Add(pEmitter);
}
// PC 0x00483A00 GOG 0x00483AC0
void SceneBSPRemoveTiledata(Scene* pScene, const Vector& vPosition)
{
    AurscenemanagerTrace3();
    if (pScene->m_pBSPRoot == 0)
    {
        return;
    }

    nodedata* pNodeData = BSPFitSphere(pScene->m_pBSPRoot, vPosition, 0.0f)->m_pData;
    if (pNodeData->m_pTileData == 0)
    {
        return;
    }

    for (int i = pNodeData->m_pTileData->m_lstParts.m_nCount; i != 0; )
    {
        Part* pPart = pNodeData->m_pTileData->m_lstParts.m_pList[--i];
        SceneBSPRemovePart(pScene, pPart);
        Model* pModel = pPart->m_pNode->m_pModel->AsModel();
        if (pModel != 0)
        {
            pModel->DelRef(pModel);
        }
        delete pPart;
        pNodeData->m_pTileData->m_lstParts.m_nCount--;
    }

    if (pNodeData->m_pTileData->m_pStaticObject != 0)
    {
        delete pNodeData->m_pTileData->m_pStaticObject;
        pNodeData->m_pTileData->m_pStaticObject = 0;
    }
    delete pNodeData->m_pTileData;
    pNodeData->m_pTileData = 0;
}

#ifdef _XBOX
static void AddVisibleTriMeshes(nodedata* pNodeData, List<Plane>& planes);
#endif
// PC 0x00483AC0 GOG 0x00483B80
static void ProcessParts(BSPNode* pNode, void* pData, List<Plane>& planes)
{
    if (pNode == 0)
    {
        return;
    }
    nodedata* pNodeData = pNode->m_pData;
    if (pNodeData == 0)
    {
        return;
    }
    if (bool(renderaabb && pNodeData->m_pAABB))
    {
        AddPartToMeshBuckets(pNodeData->m_pAABB);
    }
    if (bool(bool(!cullfoggeometry && pNodeData->m_pTileData) &&
        pNodeData->m_pTileData->m_bFade))
    {
        return;
    }

    for (int i = 0; i < pNodeData->m_lstGobs.m_nCount; i++)
    {
        Gob* pGob = pNodeData->m_lstGobs[i];
        if (!GobOutside(pGob, planes))
        {
            pGob->EnableInViewVolume();
            Vector vCenter = pGob->m_vPosition +
                pGob->m_qOrientation * pGob->m_vMinimumBoundingSphere;
            float fDistance = magnitude(CurrentCamera->m_vPosition - vCenter);
            pGob->m_fZOrder = fDistance;
        }
        else
        {
            countculledgobs++;
        }
    }

#ifdef _XBOX
    List<Gob*> lstStaticGobs(pNodeData->GetMovingObjects());
#else
    List<Gob*> lstStaticGobs(pNodeData->m_lstMovingObjects);
#endif
    for (int i = 0; i < lstStaticGobs.m_nCount; i++)
    {
        Gob* pGob = lstStaticGobs[i];
        if (pGob->IsInViewVolume())
        {
            pGob->m_fZOrder = magnitude(CurrentCamera->m_vPosition -
                (pGob->m_vPosition + pGob->m_qOrientation *
                 pGob->m_vMinimumBoundingSphere));
        }
        if (!GobOutside(pGob, planes))

        {
            pGob->EnableInViewVolume();
            pGob->m_fZOrder = magnitude(CurrentCamera->m_vPosition -
                (pGob->m_vPosition + pGob->m_qOrientation *
                 pGob->m_vMinimumBoundingSphere));
        }
        else
        {
            countculledgobs++;
        }
    }

    for (int i = 0; i < pNodeData->m_lstEmitters.m_nCount; i++)
    {
        PartEmitter* pEmitter = pNodeData->m_lstEmitters[i];
        if (!PartOutside(pEmitter, planes))
        {
            emitterbucket.Add(pEmitter);
        }
    }

#ifdef _XBOX
    List<PartEmitter*> lstStaticEmitters(pNodeData->GetMovingEmitters());
#else
    List<PartEmitter*> lstStaticEmitters(pNodeData->m_lstMovingEmitters);
#endif
    for (int i = 0; i < lstStaticEmitters.m_nCount; i++)
    {
        PartEmitter* pEmitter = lstStaticEmitters[i];
        if (!PartOutside(pEmitter, planes) ||
            strstr(strupr(pEmitter->m_pNode->m_pModel->m_sName), "BAODUR"))
        {
            emitterbucket.AddUnique(pEmitter);
        }
    }

    if (pNodeData->m_pTileData && pNodeData->m_pTileData->m_fFadeAlpha < 1.0f &&
        !pNodeData->m_pTileData->m_bFade)
    {
        pNodeData->m_pTileData->m_fFadeAlpha += DeltaT / fadeToZeroTime;
        if (pNodeData->m_pTileData->m_fFadeAlpha > 1.0f)
        {
            pNodeData->m_pTileData->m_fFadeAlpha = 1.0f;
        }
    }

    if (!renderaabb)
    {
#ifdef _XBOX
        AddVisibleTriMeshes(pNodeData, planes);
#else
        for (int i = 0; i < pNodeData->m_lstTriMeshes.m_nCount; i++)
        {
            PartTriMesh* pTriMesh = pNodeData->m_lstTriMeshes[i];
            if (!PartOutside(pTriMesh, planes))
            {
                AddPartToMeshBuckets(pTriMesh);
            }
        }
#endif
    }

    for (int i = 0; i < pNodeData->m_lstShadowProjections.m_nCount; i++)
    {
        PartProjection* pShadow = (PartProjection*)pNodeData->m_lstShadowProjections[i];
        if (!PartOutside(pShadow, planes))
        {
            staticshadowbucket.Add(pShadow);
        }
        else
        {
            countculledshadows++;
        }
    }

    for (int i = 0; i < pNodeData->m_lstTriggers.m_nCount; i++)
    {
        PartTrigger* pTrigger = pNodeData->m_lstTriggers[i];
        if (!PartOutside(pTrigger, planes))
        {
            triggerbucket.AddUnique(pTrigger);
        }
    }

    for (int i = 0; i < pNodeData->m_lstBeamProjections.m_nCount; i++)
    {
        PartProjection* pBeam = (PartProjection*)pNodeData->m_lstBeamProjections[i];
        if (!PartOutside(pBeam, planes))
        {
            staticbeambucket.Add(pBeam);
        }
        else
        {
            countculledbeams++;
        }
    }

    for (int i = 0; i < pNodeData->m_lstBins.m_nCount; i++)
    {
        CAurTriangleBin* pBin = pNodeData->m_lstBins[i];
        if (!BinOutside(pBin, planes))
        {
            binbucket.Add(pBin);
            countbinbucket++;
        }
    }
}
// PC 0x004847D0 GOG 0x00484890
void LightNodeTile(BSPNode* pNode, void* pData)
{
    nodedata* pNodeData = pNode->m_pData;
    if (pNodeData->m_pTileData != 0)
    {
        List<PartLight*> lstLights;
        List<PartLight*> lstSceneLights;
        lstSceneLights = ((Scene*)pData)->m_pLightManager->m_lstStaticLights;
        for (int i = 0; i < pNodeData->m_pTileData->m_lstParts.m_nCount; i++)
        {
            PartTriMesh* pPart = pNodeData->m_pTileData->m_lstParts[i]->AsPartTriMesh();
            if (pPart != 0)
            {
                lstLights.m_nCount = 0;
                MdlNodeTriMesh* pMesh = pPart->m_pNode->AsMdlNodeTriMesh();
                Vector vCenter;
                vCenter = pPart->m_vPosition + pPart->m_qOrientation * pMesh->m_vSphereCenter;
                for (int j = 0; j < lstSceneLights.m_nCount; j++)
                {
                    PartLight* pLight = lstSceneLights[j];
                    if (pLight->m_nLightType == PartLight::LIGHTTYPE_STATIC &&
                        !equal(pLight->m_vColor, Vector(0.0f, 0.0f, 0.0f)))
                    {
                        float fDistance = magnitude(pLight->m_vWorldPosition - vCenter);
                        if (fDistance < pLight->m_fRadius + pMesh->m_fSphereRadius)
                        {
                            lstLights.Add(pLight);
                        }
                    }
                }
                LightPartTriMesh(pPart, lstLights, (Scene*)pData);
            }
        }
    }
}
// PC 0x00484B50 GOG 0x00484C10
void DoStaticLightBuckets(Scene* pScene)
{
    for (int i = 0; i < staticlightbucket.m_nCount; i++)
    {
        LightNodeTile(staticlightbucket.m_pList[i], pScene);
    }

    if (staticlightbucket.m_nCount != 0 && pScene->m_pBSPRoot != 0)
    {
        BSPTraverse(pScene->m_pBSPRoot, LightBins, pScene);
    }
}
// PC 0x00484BA0 GOG 0x00484C60
void AnimateStaticLight(CAurObject* pObject, char* sEvent, void* pData)
{
    AurscenemanagerTrace3();
    UpdateStaticLighting(((Gob*)pObject)->m_pScene, (PartLight*)pData);
}
// PC 0x00484BC0 GOG 0x00484C80
void AddLightToRooms(Scene* pScene, PartLight* pLight)
{
    int nRooms = 0;
    for (int i = 0; i < pScene->m_lstRooms.m_nCount; i++)
    {
        CAurRoom* pRoom = pScene->m_lstRooms.m_pList[i];
        if (pLight->m_vWorldPosition.m_fX >= pRoom->m_vMinimum.m_fX &&
            pLight->m_vWorldPosition.m_fY >= pRoom->m_vMinimum.m_fY &&
            pLight->m_vWorldPosition.m_fX <= pRoom->m_vMaximum.m_fX &&
            pLight->m_vWorldPosition.m_fY <= pRoom->m_vMaximum.m_fY)
        {
#ifdef _XBOX
            pRoom->m_pData->AddMovingLight(pLight);
#else
            pRoom->m_pData->m_lstMovingLights.Add(pLight);
#endif
            nRooms++;
        }
    }
    if (nRooms == 0)
    {
        for (int i = 0; i < pScene->m_lstRooms.m_nCount; i++)
        {
#ifdef _XBOX
            pScene->m_lstRooms.m_pList[i]->m_pData->AddMovingLight(pLight);
#else
            pScene->m_lstRooms.m_pList[i]->m_pData->m_lstMovingLights.Add(pLight);
#endif
        }
    }
}
// PC 0x00484D30 GOG 0x00484DF0
void AddEmitterToRooms(Scene* pScene, PartEmitter* pEmitter)
{
    if (pEmitter->m_pAurObject == 0)
    {
        return;
    }
    Vector& vPrevious = pEmitter->m_pAurObject->m_vPreviousPosition;
    Vector& vCurrent = pEmitter->m_pAurObject->m_vPosition;
    if (equal(vCurrent, vPrevious) &&
        pEmitter->m_pAurObject->m_lstRoomData.m_nCount > 0)
    {
        pEmitter->m_pAurObject->m_lstRoomData[0]->AddMovingEmitter(pEmitter);
        return;
    }

    Vector& vPosition = pEmitter->m_pAurObject->m_vPosition;
    int nRooms = 0;
    int bAddToAll = TRUE;
    int i = 0;
    while (i < pScene->m_lstRooms.m_nCount)
    {
        CAurRoom* pRoom = pScene->m_lstRooms.m_pList[i];
        if (vPosition.m_fX >= pRoom->m_vMinimum.m_fX &&
            vPosition.m_fY >= pRoom->m_vMinimum.m_fY &&
            vPosition.m_fX <= pRoom->m_vMaximum.m_fX &&
            vPosition.m_fY <= pRoom->m_vMaximum.m_fY)
        {
            pRoom->m_pData->AddMovingEmitter(pEmitter);
            nRooms++;
        }
        i++;
    }
    if (nRooms == 0 && bAddToAll)
    {
        for (i = 0; i < pScene->m_lstRooms.m_nCount && bAddToAll; i++)
        {
            pScene->m_lstRooms.m_pList[i]->m_pData->AddMovingEmitter(pEmitter);
        }
    }
}

static const int AUR_ROOM_INDEX_ALL = 999;
// PC 0x00484E30 GOG 0x00484EF0
void AddObjectToRooms(Scene* pScene, Gob* pGob)
{
    int nRooms = 0;
    Vector vPosition;
    Model* pModel = pGob->m_pModel;
    if (pModel && char(pModel->m_Classification.m_bCharacter))
    {
        vPosition = pGob->m_vPosition;
    }
    else
    {
        Quaternion& qOrientation = pGob->m_qOrientation;
        vPosition = pGob->m_vPosition + qOrientation *
            ((pGob->m_vBoundingBoxMinimum + pGob->m_vBoundingBoxMaximum) / 2.0f);
    }

    if (!(pModel ? char(pModel->m_Classification.m_bAllRooms) : char(0)))
    {
        for (int i = 0; i < pScene->m_lstRooms.m_nCount; i++)
        {
            CAurRoom* pRoom = pScene->m_lstRooms.m_pList[i];
            if (vPosition.m_fX >= pRoom->m_vMinimum.m_fX &&
                vPosition.m_fY >= pRoom->m_vMinimum.m_fY &&
                vPosition.m_fX <= pRoom->m_vMaximum.m_fX &&
                vPosition.m_fY <= pRoom->m_vMaximum.m_fY)
            {
                pRoom->m_pData->AddMovingObject(pGob);
                nRooms++;
                crap = i;
            }
        }
    }
    if (nRooms == 0)
    {
        for (int i = 0; i < pScene->m_lstRooms.m_nCount; i++)
        {
#ifdef _XBOX
            pScene->m_lstRooms.m_pList[i]->m_pData->AddMovingObject(pGob);
#else
            pScene->m_lstRooms.m_pList[i]->m_pData->m_lstMovingObjects.Add(pGob);
#endif
        }
        crap = AUR_ROOM_INDEX_ALL;
    }
}
// PC 0x004851C0 GOG 0x00485280
int SceneBSPAddPart(Scene* pScene, Part* pPart, Model* pModel)
{
    if (pScene->m_pBSPRoot == 0)
    {
        return 0;
    }

    Vector vPosition;
    Quaternion qOrientation;
    pPart->GetWorld(vPosition, qOrientation);

    PartLight* pLight = pPart->AsPartLight();
    PartAABB* pAABB;
    PartTriMesh* pTriMesh;
    PartProjection* pProjection;
    PartEmitter* pEmitter;
    PartTrigger* pTrigger;

    if (pLight != 0)
    {
        BSPNode* pNode = BSPFitSphere(pScene->m_pBSPRoot, vPosition,
                                          pLight->m_fRadius);
        if (!pNode->m_pData->m_lstLights.Contains(pLight))
        {
            pNode->m_pData->m_lstLights.Add(pLight);
        }
        pScene->m_pLightManager->AddLight(pLight);
    }
    else if ((pAABB = pPart->AsPartAABB()) != 0)
    {
        BSPFitSphere(pScene->m_pBSPRoot, vPosition, 0.0f)->m_pData->m_pAABB = pAABB;
    }
    else if ((pTriMesh = pPart->AsPartTriMesh()) != 0)
    {
        Vector vMinimum = pTriMesh->m_vMinimum;
        Vector vMaximum = pTriMesh->m_vMaximum;
        BSPNode* pNode = BSPFitBox(pScene->m_pBSPRoot, vMinimum, vMaximum);
        pNode->m_pData->m_lstTriMeshes.Add(pTriMesh);
    }
    else if ((pProjection = pPart->AsPartProjection()) != 0)
    {
        Vector vMinimum = pProjection->m_vMinimum;
        Vector vMaximum = pProjection->m_vMaximum;
        nodedata* pNodeData = BSPFitBox(pScene->m_pBSPRoot, vMinimum, vMaximum)->m_pData;
        if (pProjection->AsPartShadow())
        {
            pNodeData->m_lstShadowProjections.Add(pProjection->AsPartShadow());
        }
        else
        {
            pNodeData->m_lstBeamProjections.Add(pProjection->AsPartBeam());
        }
    }
    else if ((pEmitter = pPart->AsPartEmitter()) != 0)
    {
        nodedata* pNodeData = BSPFitSphere(pScene->m_pBSPRoot, vPosition,
                                          pEmitter->m_fRadius)->m_pData;
        if (!pNodeData->m_lstEmitters.Contains(pEmitter))
        {
            pNodeData->m_lstEmitters.Add(pEmitter);
        }
        if (pEmitter->m_pNode->AsMdlNodeEmitter()->IsAffectedByWind() &&
            pScene->m_pWindManager)
        {
            pEmitter->m_pWindManager = pScene->m_pWindManager;
            pScene->m_pWindManager->RegisterWindyEmitter(pEmitter);
        }
    }
    else if ((pTrigger = pPart->AsPartTrigger()) != 0)
    {
        Vector vMinimum = pTrigger->m_vMinimum;
        Vector vMaximum = pTrigger->m_vMaximum;
        nodedata* pNodeData = BSPFitBox(pScene->m_pBSPRoot, vMinimum, vMaximum)->m_pData;
        if (!pNodeData->m_lstTriggers.Contains(pTrigger))
        {
            pNodeData->m_lstTriggers.Add(pTrigger);
        }
    }
    else
    {
        nodedata* pNodeData = BSPFitSphere(pScene->m_pBSPRoot, vPosition, 0.0f)->m_pData;
        if (!pNodeData->m_lstOthers.Contains(pPart))
        {
            pNodeData->m_lstOthers.Add(pPart);
        }
    }

    if (pModel)
    {
        pModel->AddRef(pModel);
    }
    return 1;
}
// PC 0x004854F0 GOG 0x004855B0
void ManageSceneBSP(Scene* pScene)
{
    LightManager::cm_bLightManagerUpdated = 1;
    if (!enablescenemanagement)
    {
        return;
    }
    if (currentroomindex >= 0)
    {
        pScene->m_pCurrentRoom = pScene->m_lstRooms[currentroomindex];
        pScene->m_bIgnoreVisibilityGraph = 0;
        enablevisibilitygraph = 1;
    }
    if (pScene->m_pBSPRoot == 0 && pScene->m_lstRooms.m_nCount == 0)
    {
        return;
    }
    for (int i = 0; i < pScene->m_lstObjects.m_nCount; i++)
    {
        pScene->m_lstObjects[i]->DisableInViewVolume();
    }

    List<PartLight*>& lstLights = pScene->m_pLightManager->m_lstShadowLights;
    static Vector lastcamerapos;
    static Quaternion lastcameraorientation;
    static float lastcameraclipnear;
    static float lastcameraclipfar;
    static int firsttime = 1;

    ConstructCameraFadePlane(pScene);
    DoStaticLightBuckets(pScene);
    ClearBuckets(pScene);
    ClearSceneRooms(pScene);
    pScene->m_pLightManager->Reset();
    pScene->m_pLightManager->UpdateLights();
    for (int i = 0; i < lstLights.m_nCount; i++)
    {
        PartLight* pLight = lstLights[i];
        AddLightToRooms(pScene, pLight);
        if (pScene->m_lstRooms.m_nCount == 0)
        {
            SceneBSPRemovePart(pScene, pLight);
            SceneBSPAddPart(pScene, pLight, 0);
        }
    }
    if (!pScene->m_bIgnoreVisibilityGraph && enablevisibilitygraph)
    {
        pScene->m_pLightManager->CollectLightsForGivenRoom(pScene->m_pCurrentRoom);
    }
    else if (pScene->m_lstRooms.m_nCount)
    {
        pScene->m_pLightManager->CollectLightsForGivenRoom(0);
    }
    if (pScene->m_lstRooms.m_nCount)
    {
        pScene->m_pLightManager->SetupLightsForThisRoom(
            pScene->m_vSceneFocus, CurrentCamera->m_fFarClip);
    }
    else
    {
        pScene->m_pLightManager->SetupLightsForThisPositionAndRadius(
            pScene->m_vSceneFocus, CurrentCamera->m_fFarClip);
    }
    for (int i = 0; i < pScene->m_lstEmitters.m_nCount; i++)
    {
        PartEmitter* pEmitter = pScene->m_lstEmitters[i];
        AddEmitterToRooms(pScene, pEmitter);
        if (pScene->m_lstRooms.m_nCount == 0 && pEmitter->m_pAurObject)
        {
            SceneBSPRemovePart(pScene, pEmitter);
            SceneBSPAddPart(pScene, pEmitter, 0);
        }
    }
    for (int i = 0; i < pScene->m_lstObjects.m_nCount; i++)
    {
        if (pScene->m_lstObjects[i]->m_pModel)
        {
            AddObjectToRooms(pScene, pScene->m_lstObjects[i]);
            if (pScene->m_lstRooms.m_nCount == 0 &&
                strcmp(pScene->m_lstObjects[i]->m_sName, "c_FocusGob"))
            {
                SceneBSPRemoveGob(pScene, pScene->m_lstObjects[i]);
                SceneBSPAddGob(pScene, pScene->m_lstObjects[i]);
            }
        }
    }
    countculledpart = 0;
    countculledshadows = 0;
    countculledbeams = 0;
    countbackshadows = 0;
    countculledgobs = 0;

    List<CAurRoom*> lstRooms;
    CollectActiveRooms(pScene, lstRooms);
    if (pScene->m_lstRooms.m_nCount)
    {
        int nRooms = lstRooms.m_nCount;
        if (pScene->m_bBinsPopulated)
        {
            for (int i = 0; i < nRooms; i++)
            {
                BSPTraverse(lstRooms.m_pList[i], CurrentCamera->m_vPosition,
                            CAurTriangleBin::m_fFarRadiusSquared, CollectBins);
            }
            AurProcessActiveBins();
        }
        for (int i = 0; i < nRooms; i++)
        {
            BSPTraverseVolumeApprox(lstRooms.m_pList[i], CurrentCamera->m_vPosition,
                CurrentCamera->m_lstViewVolume, ProcessParts, pScene);
        }
    }
    else
    {
        BSPTraverseVolumeApprox(pScene->m_pBSPRoot, CurrentCamera->m_vPosition,
            CurrentCamera->m_lstViewVolume, ProcessParts, pScene);
    }
    pScene->m_pLightManager->PrioritizeLights(pScene->m_vSceneFocus, true);
    countlights = 0;
    for (int i = 0; i < pScene->m_pLightManager->m_lstRenderLights.m_nCount; i++)
    {
        countlights +=
            !pScene->m_pLightManager->m_lstRenderLights[i]->m_vColor.IsZero();
    }
    for (int i = 0; i < pScene->m_lstObjects.m_nCount; i++)
    {
        Gob* pGob = pScene->m_lstObjects[i];
        if (pGob->m_pModel)
        {
            if (pScene->m_lstRooms.m_nCount)
            {
                if (pGob->IsInViewVolume() ||
                    char(pGob->m_pModel->m_Classification.m_bSkipVisibilityCheck) ||
                    pGob->IsAlwaysRendering())
                {
                    if (pGob->IsHologramEnabled())
                    {
                        g_listHologramBucket.Add(pGob);
                    }
                    else if (pGob->m_pModel && (pGob->m_bDistortion ||
                        char(pGob->m_pModel->m_Classification.m_bSceneHitDisabled)))
                    {
                        g_listForcePushBucket.Add(pGob);
                    }
                    else if (char(pGob->m_pModel->m_Classification.m_bRenderEarly))
                    {
                        g_listLightsaberBucket.Add(pGob);
                    }
                    else
                    {
                        gobbucket.Add(pGob);
                    }
                }
                else
                {
#ifndef _XBOX
                    pGob->m_bVisible = true;
#endif
                }
            }
            else
            {
                gobbucket.Add(pGob);
            }
        }
    }
    CurrentCamera->EnableInViewVolume();
    lastcamerapos = CurrentCamera->m_vPosition;
    lastcameraorientation = CurrentCamera->m_qOrientation;
    lastcameraclipnear = CurrentCamera->m_fNearClip;
    lastcameraclipfar = CurrentCamera->m_fFarClip;
    firsttime = 0;
}
// PC 0x00485D00 GOG 0x00485DC0
void SceneBSPAddTileData(Scene* pScene, TileData* pTileData, Vector vPosition)
{
    if (pScene->m_pBSPRoot == 0)
    {
        return;
    }

    nodedata* pNodeData = BSPFitSphere(pScene->m_pBSPRoot, vPosition, 0.0f)->m_pData;
    pNodeData->m_pTileData = pTileData;
    for (int i = 0; i < pTileData->m_lstParts.m_nCount; i++)
    {
        Part* pPart = pNodeData->m_pTileData->m_lstParts.m_pList[i];
        SceneBSPAddPart(pScene, pPart, pPart->m_pNode->m_pModel->AsModel());
    }
}
