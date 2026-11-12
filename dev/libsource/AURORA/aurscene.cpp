#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef _XBOX
void AurPushClientState()
{
    glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
}

void AurPopClientState()
{
    glPopClientAttrib();
}
#endif

class CHitInfo;
static inline void AurSceneTrace(CHitInfo* = 0);
#include "aurscene.h"
#include "aurora.h"
#include <aurlist.h>
#include <aurpointer.h>
#include "aurrendergl.h"

static inline void AurSceneTrace4();
static inline void AurSceneTrace3();
static inline void AurSceneTrace2();
static inline void AurSceneTrace1();
#define AURSCENE_TRACE() AurSceneTrace2()

const int Scene::FADING_OFF = 1;
const int Scene::FADING_ON = 2;
const int CAurScene::ENABLE_FADE = 1;
const int CAurScene::DISABLE_FADE = 2;
const int CAurScene::ALWAYS_FADE = 4;

Scene* currentscene = 0;

int countcull = 0;

int countdisplaycreates = 0;
int g_bInScreenshotMode = 0;
static void NOP();

void (*AurRenderCallback)() = NOP;
int Scene::geomFadeEnabled = ENABLE_FADE;

int checkstasis = 1;

int g_FocusModelCount = 0;
int enablegetnearestlights = 0;
int count_render = 0;
int enableshadows = 1;
int enablerendertriggers = 0;
int enablegrass = 1;
float alphatest = 0.0f;
float grassalphatest = 0.4f;

int newsorting = 1;
int numgobs = 0;
int rendertransparentcharacters = 0;
#ifdef _XBOX
int interleaveanimations;
#else
int interleaveanimations = 1;
#endif
char g_bEnableInterleavedAnimations = 0;
float visibilitycheckdelay = 0.33f;
float visibilitycutoffratio = 0.04f;
int enablevisibilitytest = 0;
int countvisibilityculls = 0;
int dogobrender = 1;
int showlightpriorityinfo = 0;

int partcheckplanes = 0;
int renderspeedtest = 0;
float maxshadowdist = 35.0f;
int enablefocusshadowing = 1;

int enablebeams = 1;
float shadowlength = 5.0f;
int enableemitters = 1;
float exp2fogdensity = 0.01f;
int forcefogtest = 0;
int countvertices = 0;
float lensflaresize = 0.1f;
int hitcheckflare = 1;
float flarespeed = 3.0f;
int reverseflarehit = 1;

int renderlensflares = 1;
int renderpersonalspace = 0;
int renderpaths = 0;

int renderbins = 0;
int countbins = 0;
int enablehiccups = 0;
int passes = 0;
int g_nModelsRead = 0;
int g_nImagesDownSampled = 0;
int g_nImagesDecoded = 0;
int maxmodelloadtime = 0;
int maxappendtime = 0;
int modelloadtime = 0;
int appendtime = 0;
int maxtotaltime = 0;
int renderpauseplane = 0;
int enablescenerender = 1;
int skippauseplaneforscreenshot = 0;

int showvisibilityinfo = 0;
int doprofile = 0;
int g_nDanglyMeshAnimationByMod3Counter = 2;

Scene* g_pLastScene = NULL;
unsigned long g_SceneRender_CurrentTime_ms;

float mintopshadowalpha = 0.0f;
float shadowcutoffangle = 0.0f;
float shadowdarkradius = 0.0f;
float shadowdarkratio = 0.0f;
int shadowspeedtest = 0;
int enablerenderdynamic = 1;
int enablerenderstatic = 1;
int noframebuffermodification = 1;
float shadowalpha = 0.5f;
int shadowstaticgeom = 1;
int shadowdynamicgeom = 1;
float solidcutoffangle = 90.0f;
int modulateshadows = 1;
int newshadows = 1;
int systemmemoryshadows = 0;

int countanimatedgobs = 0;
int doanimateprofile = 0;
int bspXsize = 0;
int bspYsize = 0;
int parttrigger = 0;
int enablesoftshadows = 1;
float Scene::cellsize = 10.0f;
float Scene::binsize = 2.6f;

#pragma inline_recursion(on)
#pragma inline_depth(10)

class CAurTriangleBin;
extern int enablefocusshadowing;
extern float maxshadowdist;
extern float shadowlength;

void SetShadowStencilBuffer()
{
    int nStencilBits = AurGetAvailableStencilBits();
    if (nStencilBits)
    {
        GLRender_ClearStencilValue((1 << (nStencilBits - 1)) - 1);
    }
    GLRender_SetStencilMask(GLRender::STENCIL_WRITE_MASK);
    GLRender::Clear(4);
}
// PC 0x0056FB90
void Scene::InitBSP(int, int, int, bool, float)
{
}

extern char g_bEnableInterleavedAnimations;
// PC 0x004015A0
void Scene::FinishSceneGeometry()
{
}
// PC 0x004015A0
void Scene::PostProcessLS()
{
}
static void NOP()
{
}

inline __int64 QueryTimerFrequency()
{
    LARGE_INTEGER nFrequency;
    QueryPerformanceFrequency(&nFrequency);
    return nFrequency.QuadPart;
}

static __int64 QueryTimer()
{
    LARGE_INTEGER nPerformanceCount;
    static __int64 performanceFrequency = QueryTimerFrequency();

    QueryPerformanceCounter(&nPerformanceCount);
    return 1000000 * nPerformanceCount.QuadPart / performanceFrequency;
}

#ifdef _XBOX

static void ClearStencilBuffer()
{
    GLRender_ClearStencilValue(AurGetStencilClearValue());
    GLRender_SetStencilMask(GLRender::STENCIL_WRITE_MASK);
    GLRender_Clear(4);
}
#endif
// PC 0x004634B0
void Scene::EnableAnimations()
{
    m_bAnimating = 1;
}
// PC 0x004634C0
void Scene::DisableAnimations()
{
    m_bAnimating = 0;
}
// PC 0x004634D0
char Scene::IsAnimating()
{
    return m_bAnimating;
}

#include "nwcanimbase.h"
#include "aurwindmanager.h"
#include "aurmodel.h"
#include "aurcamera.h"
#include "aurmaterial.h"
#include "aurgrass.h"
#include "aurtrigger.h"
#include "aurscenemanager.h"
#include "aurconsole.h"
#include <string.h>
#include <math.h>
#include "aurtexman.h"
#include "aurprimitive.h"
// PC 0x004636B0
int EmitterOrderCmp(const void* pFirst, const void* pSecond)
{
    PartEmitter* pEmitter1 = *(PartEmitter**)pFirst;
    PartEmitter* pEmitter2 = *(PartEmitter**)pSecond;

    if (pEmitter1->m_pAurObject && pEmitter2->m_pAurObject &&
        pEmitter1->m_pAurObject == pEmitter2->m_pAurObject)
    {
        MdlNodeEmitter* pNode1 = pEmitter1->m_pNode->AsMdlNodeEmitter();
        MdlNodeEmitter* pNode2 = pEmitter2->m_pNode->AsMdlNodeEmitter();
        return pNode1->m_nRenderOrder - pNode2->m_nRenderOrder;
    }

    if (pEmitter1->m_nBlendMode == PartEmitter::BLEND_OTHER &&
        pEmitter2->m_nBlendMode != PartEmitter::BLEND_OTHER)
        return 1;
    if (pEmitter2->m_nBlendMode == PartEmitter::BLEND_OTHER &&
        pEmitter1->m_nBlendMode != PartEmitter::BLEND_OTHER)
        return -1;

    Vector vPosition1;
    Vector vPosition2;
    if (pEmitter1->m_pAurObject)
        vPosition1 = pEmitter1->m_pAurObject->m_vPosition;
    else
        vPosition1 = pEmitter1->m_vPosition;
    if (pEmitter2->m_pAurObject)
        vPosition2 = pEmitter2->m_pAurObject->m_vPosition;
    else
        vPosition2 = pEmitter2->m_vPosition;

    float fDistance1 = magnitude(CurrentCamera->m_vPosition - vPosition1);
    float fDistance2 = magnitude(CurrentCamera->m_vPosition - vPosition2);
    if (max(fDistance1 - fDistance2, fDistance2 - fDistance1) < 0.01f)
        return pEmitter1 > pEmitter2 ? -1 : 1;
    return fDistance1 > fDistance2 ? -1 : 1;
}
// PC 0x00463880
void AurEnableInterleavedAnimations()
{
    g_bEnableInterleavedAnimations = 1;
}

#include "aurlightmanager.h"
// PC 0x00463890
void AurDisableInterleavedAnimations()
{
    g_bEnableInterleavedAnimations = 0;
}
// PC 0x004638A0
bool Gob::IsDistortionEnabled()
{
    if (m_pModel)
        return m_bDistortion || bool(m_pModel->m_Classification.m_bSceneHitDisabled);
    return false;
}

int SceneGetNearestLights(Scene* pScene, Vector vPosition, float fRadius)
{
    return pScene->m_pLightManager->GetNearestLights(pScene, vPosition, fRadius, 0);
}
// PC 0x004638D0
Vector Scene::GetWindVector(Vector vPosition, float fScale)
{
    return m_pWindManager->GetGlobalWind(vPosition, fScale);
}

char* firstword(char* sMessage);
int Parse(char* sText, int& nValue);
// PC 0x00463910
int Gob::AttachToScene(CAurScene* pScene)
{
    SetScene((Scene*)pScene);
    return 1;
}
// PC 0x00463930
int Scene::EnableVisibilityGraph()
{
    CAurRoom* pCurrentRoom = m_pCurrentRoom;
    m_bIgnoreVisibilityGraph = 0;
    return pCurrentRoom != 0;
}
// PC 0x00463950
void Scene::DisableVisibilityGraph()
{
    m_bIgnoreVisibilityGraph = 1;
}
// PC 0x00463960
int Scene::GetIgnoreVisibilityGraph()
{
    return m_bIgnoreVisibilityGraph;
}

int AurTextureReplaceTileReferences(CAurScene* pScene, const Vector& vPosition,
                                    char* sOldTexture, char* sNewTexture);
// PC 0x00466110
MdlNode* findnode(MdlNode* pNode, char* sName)
{
    if (strcmp(pNode->TranslateNodeName(), sName) == 0)
        return pNode;

    MdlNode* pFound = 0;
    for (int i = 0; i < pNode->m_lChildren.m_nCount; i++)
    {
        pFound = findnode(pNode->m_lChildren.m_pList[i], sName);
        if (pFound)
            break;
    }
    return pFound;
}
// PC 0x00466610
void SetPartTextureRotate(Part* pPart, int nRotation)
{
    PartTriMesh* pTriMesh = pPart->AsPartTriMesh();
    if (pTriMesh)
    {
        if (((MdlNodeTriMesh*)pTriMesh->m_pNode)->m_bRotateTexture)
            pTriMesh->RotateTexture(nRotation * TILE_ROTATION_DEGREES);
    }
    for (int i = 0; i < pPart->m_lstChildren.m_nCount; i++)
        SetPartTextureRotate(pPart->m_lstChildren[i], nRotation);
}
// PC 0x00469EA0
void Scene::ChildAdd(Gob* pGob)
{
    if (pGob->m_pModel != 0)
    {
        SceneBSPAddGob(this, pGob);
        ListSpecialPartsR(pGob->m_pRootPart);
    }

    m_lstObjects.Add(pGob);

    if (pGob->AsCamera() != 0)
    {
        m_lstAnimatedObjects.Add(pGob);
    }
}
// PC 0x00467430
void Scene::ListSpecialPartsR(Part* pPart)
{
    if (pPart)
    {
        PartLight* pLight = pPart->AsPartLight();
        if (pLight)
        {
            m_pLightManager->AddLight(pLight);
            SceneBSPAddPart(this, pLight, 0);
        }

        PartEmitter* pEmitter = pPart->AsPartEmitter();
        if (pEmitter)
        {
            SceneBSPAddPart(this, pEmitter, 0);
            m_lstEmitters.AddUnique(pEmitter);
        }

        for (int i = 0; i < pPart->m_lstChildren.m_nCount; i++)
        {
            ListSpecialPartsR(pPart->m_lstChildren.m_pList[i]);
        }
    }
}

extern int interleaveanimations;

extern int rendertransparentcharacters;
extern int showlightpriorityinfo;
extern int dogobrender;
extern int countvisibilityculls;
// PC 0x004677E0
void Scene::UnlistSpecialParts(Part* pPart)
{
    if (pPart != 0)
    {
        PartLight* pLight = pPart->AsPartLight();
        if (pLight != 0)
        {
            m_pLightManager->RemoveLight(pLight, false);
            SceneBSPRemovePart(this, pLight);
        }

        PartEmitter* pEmitter = pPart->AsPartEmitter();
        if (pEmitter != 0)
        {
            m_lstEmitters.Remove(pEmitter);
            SceneBSPRemovePart(this, pEmitter);
        }

        for (int i = 0; i < pPart->m_lstChildren.m_nCount; i++)
        {
            UnlistSpecialParts(pPart->m_lstChildren[i]);
        }
    }
}
// PC 0x00467B90
void Scene::ChildRemove(Gob* pGob)
{
    if (pGob->m_pModel != 0)
    {
        SceneBSPRemoveGob(this, pGob);
        UnlistSpecialParts(pGob->m_pRootPart);
    }

    m_lstObjects.Remove(pGob);
    if (pGob->AsCamera() != 0)
    {
        m_lstAnimatedObjects.Remove(pGob);
    }
}
// PC 0x00467BF0
void Scene::DoGobShadows()
{
    List<PartLight*> lstShadowLights;
    lstShadowLights = *m_pLightManager->GetShadowLights(LightManager::SHADOWLIGHTS_ALL);
    if (!lstShadowLights.m_nCount)
        return;

    int i = gobbucket.m_nCount;
    while (i--)
    {
        if (interleaveanimations && g_bEnableInterleavedAnimations)
        {
            if (i >= gobbucket.m_nCount)
                continue;
            if (gobbucket[i]->m_pBehavior)
                gobbucket[i]->Animate(DeltaT);
            if (i >= gobbucket.m_nCount)
                continue;
        }

        if (!gobbucket[i]->m_bCastShadows || !gobbucket[i]->m_bHasGeometry)
            continue;
        if (enablefocusshadowing &&
            magnitude(currentscene->m_vSceneFocus - gobbucket[i]->m_vPosition) > maxshadowdist)
            continue;
        if (!gobbucket[i]->m_pModel || bool(gobbucket[i]->m_pModel->m_Classification.m_bCharacter) ||
            (gobbucket[i]->m_pBehavior && gobbucket[i]->m_pBehavior->AsBehaviorAttach()))
        {
            for (int j = 0; j < lstShadowLights.m_nCount; j++)
            {
                float fRadius = lstShadowLights[j]->m_fRadius;
                if (lstShadowLights[j]->m_fShadowRadius > 0.0f)
                    fRadius = lstShadowLights[j]->m_fShadowRadius;
                if (magnitude(lstShadowLights[j]->m_vWorldPosition -
                              gobbucket[i]->m_vPosition) < fRadius)
                    ProjectDynamicShadow(gobbucket[i], lstShadowLights[j]);
            }
        }
    }
}
// PC 0x00467E60
void Scene::DoDynamicMeshProjections()
{
    List<PartLight*> lstShadowLights;
    Plane& farPlane = CurrentCamera->m_lstViewVolume[Camera::VIEW_PLANE_FAR];
    Plane plane(farPlane.m_vNormal, farPlane.m_fDistance + 0.05f);
    lstShadowLights = *m_pLightManager->GetShadowLights(LightManager::SHADOWLIGHTS_ALL);

    for (int i = 0; i < lstShadowLights.m_nCount; i++)
    {
        PartLight* pLight = lstShadowLights[i];
        float fRadius = pLight->m_fRadius;
        if (pLight->m_fShadowRadius > 0.0f)
            fRadius = pLight->m_fShadowRadius;
        fRadius = max(fRadius, shadowlength);

        for (int j = 0; j < meshshadowbucket.m_nCount; j++)
        {
            if (meshshadowbucket[j]->WithinRange(pLight->m_vWorldPosition, fRadius))
            {
                GLRender_PushCurrentMatrix();
                float afMatrix[16];
                MakeMatrix4x4(afMatrix, meshshadowbucket[j]->m_vPosition,
                              meshshadowbucket[j]->m_qOrientation);
                GLRender_MultMatrix(afMatrix);
                Plane localPlane(plane.m_vNormal, plane.m_fDistance);
                localPlane.Transform(meshshadowbucket[j]->m_vPosition,
                                     meshshadowbucket[j]->m_qOrientation);
                Vector vPosition = -meshshadowbucket[j]->m_qOrientation *
                    (pLight->m_vWorldPosition - meshshadowbucket[j]->m_vPosition);
                meshshadowbucket[j]->Project(vPosition, fRadius, localPlane, 0);
                GLRender_PopCurrentMatrix();
            }
        }
    }
}

extern int enableemitters;

inline void SetFogMode(int nFogMode)
{
    GLRender_SetFogMode(nFogMode);
}
// PC 0x00468240
void Scene::RenderGobFirst(CAurObject* pObject)
{
    Gob* pGob = (Gob*)pObject;
    m_lstObjects.Remove(pGob);
    m_lstObjects.Insert(pGob, 0);
}
// PC 0x00468260
void Scene::RenderGobLast(CAurObject* pObject)
{
    Gob* pGob = (Gob*)pObject;
    m_lstObjects.Remove(pGob);
    m_lstObjects.Add(pGob);
}
// PC 0x004682B0
void Scene::RenderGobAfter(CAurObject* pObject, CAurObject* pAfter)
{
    List<Gob*>& lstObjects = m_lstObjects;
    int nPosition = lstObjects.IndexOf((Gob*)pAfter);

    Gob* pGob = (Gob*)pObject;
    lstObjects.Remove(pGob);
    return lstObjects.Insert(pGob, nPosition + 1);
}
// PC 0x00468300
void Scene::RenderGobBefore(CAurObject* pObject, CAurObject* pBefore)
{
    List<Gob*>& lstObjects = m_lstObjects;
    int nPosition = lstObjects.IndexOf((Gob*)pBefore);

    Gob* pGob = (Gob*)pObject;
    lstObjects.Remove(pGob);
    return lstObjects.Insert(pGob, nPosition);
}

int AurGetStencilClearValue();

// PC 0x00468350
void CollapsePartTree(Gob* pGob, Part* pPart, List<Part*>& lstParts,
                      Vector vPosition, Quaternion qOrientation,
                      Scene* pScene, CAurRoom* pRoom)
{
    Vector vOriginalPosition;
    Quaternion qOriginalOrientation;
    Part* pAnimatedPart = 0;
    char sAnimatedPart[64];
    if (pGob)
    {
        strcpy(sAnimatedPart, pGob->m_pModel->m_sName);
        strcat(sAnimatedPart, "a");
        vOriginalPosition = pPart->m_vPosition;
        qOriginalOrientation = pPart->m_qOrientation;
    }

    pPart->m_vPosition = vPosition + qOrientation * pPart->m_vPosition;
    pPart->m_qOrientation = qOrientation * pPart->m_qOrientation;
    lstParts.Add(pPart);
    for (int i = 0; i < lstParts.m_nCount; i++)
    {
        Part* pCurrentPart = lstParts[i];
        PartReference* pReference = pCurrentPart->AsPartReference();
        if (pReference && pRoom && pScene)
        {
            char* sRefModel = ((MdlNodeReference*)pReference->m_pNode)->m_sRefModel;
            Gob* pReferenceObject = (Gob*)NewCAurObject(sRefModel, "", 0, 0);
            if (pReferenceObject)
            {
                pReferenceObject->SetPosition(pCurrentPart->m_vPosition);
                pReferenceObject->SetOrientation(pCurrentPart->m_qOrientation);
                float fSpeed = 1.0f + (rand() % 100 - 50.0f) / 1000.0f;
                float fStartTime = (rand() % 100) / 100.0f;
                pReferenceObject->PlayAnimation("default", fSpeed, 0, fStartTime);
                pReferenceObject->SetScene(pScene);
                pRoom->m_lstAnimatedObjects.Add(pReferenceObject);
            }
        }
        for (int j = 0; j < pCurrentPart->m_lstChildren.m_nCount; j++)
        {
            Part* pChild = pCurrentPart->m_lstChildren[j];
            if (pGob && stricmp(pChild->m_pNode->TranslateNodeName(), sAnimatedPart) == 0)
            {
                pAnimatedPart = pChild;
            }
            else
            {
                pChild->m_vPosition = pCurrentPart->m_vPosition +
                    pCurrentPart->m_qOrientation * pChild->m_vPosition;
                pChild->m_qOrientation = pCurrentPart->m_qOrientation * pChild->m_qOrientation;
                lstParts.Add(pChild);
            }
        }
        pCurrentPart->m_lstChildren.m_nCount = 0;
        pCurrentPart->m_pParent = 0;
        if (pCurrentPart->m_pAurObject &&
            pCurrentPart->m_pAurObject->m_pRootPart == pCurrentPart)
            pCurrentPart->m_pAurObject->m_pRootPart = 0;
        pCurrentPart->m_pAurObject = 0;
    }

    if (pGob)
    {
        lstParts.Remove(pPart);
        pPart->m_lstChildren.Add(pAnimatedPart);
        pPart->m_pAurObject = pGob;
        pGob->m_pRootPart = pPart;
        pPart->m_vPosition = vOriginalPosition;
        pPart->m_qOrientation = qOriginalOrientation;
        pGob->GetMinimumBoundingBox();
        pGob->SetProcessFlag();
    }
}

inline void SetColorMask(unsigned char bRed, unsigned char bGreen, unsigned char bBlue,
                         unsigned char bAlpha)
{
    GLRender_SetColorMask(bRed, bGreen, bBlue, bAlpha);
}

inline void SetColor4fv(const float* pColor)
{
    GLRender_SetColorv(pColor);
}

#ifndef _XBOX
void SetMaterialParameter(int nFace, int nMaterial, GLfloat* pfParams)  // PC 0x00463D30
{
    GLRender_SetMaterialv(nFace, nMaterial, pfParams);
}
#endif

int Scene::GetNumRooms()
{
    return m_lstRooms.m_nCount;
}
// PC 0x00463970
int Scene::SetCurrentRoom(CAurRoom* pRoom)
{
    m_pCurrentRoom = pRoom;
    return (int)pRoom;
}
// PC 0x00463980
int Scene::SpawnRooms(char* sName)
{
    void* pLayout = AurResGet(sName, ".lyt", 0, true);
    int nRoomCount;
    int nTrackCount;
    char sObjectName[128];
    char* sLine;

    if (pLayout == 0)
    {
        return 0;
    }

    nRoomCount = 0;
    nTrackCount = 0;
    Vector vPosition;
    for (sLine = AurResGetNextLine(); sLine != 0; sLine = AurResGetNextLine())
    {
        while (*sLine == ' ' || *sLine == '\t')
        {
            sLine++;
        }

        if (nRoomCount > 0)
        {
            sscanf(sLine, "%s %f%f%f", sObjectName, &vPosition.m_fX,
                   &vPosition.m_fY, &vPosition.m_fZ);
            SpawnRoom(sObjectName, vPosition, Quaternion());
            nRoomCount--;
        }

        if (stricmp("roomcount", firstword(sLine)) == 0)
        {
            Parse(sLine + 9, nRoomCount);
        }

        if (nTrackCount > 0)
        {
            Gob* pGob;

            sscanf(sLine, "%s %f%f%f", sObjectName, &vPosition.m_fX,
                   &vPosition.m_fY, &vPosition.m_fZ);
            pGob = (Gob*)NewCAurObject(sObjectName, sObjectName, 0, 0);
            if (pGob != 0)
            {
                pGob->AttachToScene((Scene*)this);
                pGob->SetPosition(vPosition);
            }
            nTrackCount--;
        }

        if (stricmp("trackcount", firstword(sLine)) == 0)
        {
            Parse(sLine + 10, nTrackCount);
        }
    }

    AurResFree(pLayout, 0);
    LoadVisibility(sName);
    return m_lstRooms.m_nCount;
}
// PC 0x00463B70
CallBack* ProcessStaticObject(Gob* pGob, Scene* pScene, TileData* pTile,
                        const Vector& vPosition, const Quaternion& qOrientation)
{
    pTile->m_pStaticObject = pGob;
    pGob->m_vPosition = vPosition;
    pGob->m_qOrientation = qOrientation;
    pGob->m_pTileData = pTile;
    pGob->SetScene(pScene);
    return pGob->RegisterCallBack(
        AnimateStaticLight,
        "AnimateStaticLight", 0, 9999.0f, 0);
}
// PC 0x00463C50
void Scene::SetTileLights(const Vector& vPosition, const Vector& vMainLight,
                          const Vector& vSourceLight)
{
    TileData* pTile = SceneBSPGetTileData(this, vPosition);

    if (pTile->m_pMainLight != 0 &&
        !equal(pTile->m_pMainLight->m_vColor, vMainLight))
    {
        pTile->m_pMainLight->m_vColor = vMainLight;
        UpdateStaticLighting(this, pTile->m_pMainLight);
        pTile->m_pMainLight->m_bShadow = 1;
        pTile->m_pMainLight->m_fShadowRadius = 12.0f;
        pTile->m_pMainLight->m_bFlare = 1;
        pTile->m_pMainLight->m_nPriority = 5;
    }

    if (pTile->m_pSourceLight != 0 &&
        !equal(pTile->m_pSourceLight->m_vColor, vSourceLight))
    {
        pTile->m_pSourceLight->m_vColor = vSourceLight;
        UpdateStaticLighting(this, pTile->m_pSourceLight);
        pTile->m_pSourceLight->m_bShadow = 1;
        pTile->m_pSourceLight->m_fShadowRadius = 8.0f;
        pTile->m_pSourceLight->m_bFlare = 1;
        pTile->m_pSourceLight->m_nPriority = 5;
    }
}
// PC 0x00463D60
void Scene::SetTileFlames(const Vector& vPosition, CAurObject* pFlame1,
                         CAurObject* pFlame2)
{
    TileData* pTile = SceneBSPGetTileData(this, vPosition);
    SetSingleTileFlame(pFlame1, &pTile->m_pFlame1, pTile->m_pFlamePart1);
    if (pFlame1 != 0)
    {
        ((Gob*)pFlame1)->m_pTileData = pTile;
    }

    SetSingleTileFlame(pFlame2, &pTile->m_pFlame2, pTile->m_pFlamePart2);
    if (pFlame2 != 0)
    {
        ((Gob*)pFlame2)->m_pTileData = pTile;
    }

}
// PC 0x00463DC0
int Scene::ReplaceTileTexture(const Vector& vPosition, char* sOldTexture,
                              char* sNewTexture)
{
    return AurTextureReplaceTileReferences(
        this, vPosition, sOldTexture, sNewTexture);
}
// PC 0x00463DE0
void Scene::DeleteTile(const Vector& vPosition)
{
    SceneBSPRemoveTiledata(this, vPosition);
}
// PC 0x00463E00
void CAurScene::ComputeStaticLighting()
{
    if (this != 0)
    {
        ((Scene*)this)->PostProcessLS();
    }
}
// PC 0x00463E10
void CAurScene::SetAmbient(Vector vAmbient)
{
    ((Scene*)this)->m_vAmbient = vAmbient;
}
#ifndef _XBOX
void Scene::SetObjectScale(CAurObject* pObject, float fScale)  // PC 0x00463E30
{
    ((Gob*)pObject)->SetObjectScale(fScale, 1);
}
#endif
extern List<Scene*> Scenes;
// PC 0x00463E50
unsigned char Scene::SetGeomFadeProperties(int nProperties)
{
    if (nProperties != ENABLE_FADE && nProperties != DISABLE_FADE &&
        nProperties != ALWAYS_FADE)
    {
        return 0;
    }

    geomFadeEnabled = nProperties;
    return 1;
}
// PC 0x00463E80
float Scene::GetObjectScale(CAurObject* pObject)
{
    return ((Gob*)pObject)->m_fScale;
}
// PC 0x00463E90
void Scene::SetFogColor(const Vector& vColor)
{
    m_vFogColor = vColor;
    Vector& vFog = m_vFogColor;
    m_v80.m_fX = m_v80.m_fY = m_v80.m_fZ =
        (vFog.m_fX * 0.3f + vFog.m_fY * 0.59f + vFog.m_fZ * 0.11f) * g_DesatureFactor;
}

extern int numgobs;
// PC 0x00463EF0
int Scene::SetBlindLights(CAurObject* pObject)
{
    LightManager::m_pOnlyGob = pObject;
    return (int)pObject;
}

extern float grassalphatest;
extern float alphatest;
char AurGrassMaterialInUse(int nMaterial);
// PC 0x00468C70
void ExpandReferenceParts(Part* pPart)
{
    PartReference* pReference = pPart->AsPartReference();
    if (pReference)
    {
        MdlNodeReference* pNode = (MdlNodeReference*)pReference->m_pNode;
        Model* pModel = FindModel(pNode->m_sRefModel);
        pReference->m_lstChildren.Add(
            CreateInstanceTree(pModel->m_pRootNode, 0));
        char sName[32];
        sprintf(sName, "%s", pNode->TranslateNodeName());
        PartInitMaterial(0, pPart, sName);
    }
    for (int i = 0; i < pPart->m_lstChildren.m_nCount; i++)
    {
        ExpandReferenceParts(pPart->m_lstChildren[i]);
    }
}
// PC 0x004690A0
CAurRoom::CAurRoom(char* sName)
    : BSPNode(Vector(), 0.0f),
      m_lstVisibleRooms(0),
      m_lstAnimatedObjects(0)
{
    strcpy(m_sName, sName);
    m_pRoomObject = 0;
    m_bB8 = 0;
}
// PC 0x0046C210
CHitInfo::CHitInfo(int nType, int nFlags, int nMask)
    : m_pOutputObject()
{
    Reset(nType, nFlags, nMask);
}
// PC 0x0046B3F0
void CHitInfo::Reset(int nType, int nFlags, int nMask)
{
    m_nType = nType;
    m_nFlags = nFlags;
    m_nMask = nMask;
    m_lstHits.m_nCount = 0;
    m_nResult = 0;
    m_bResult0 = 0;
    m_bResult1 = 1;
    m_bResult2 = 0;
    m_bResult3 = 0;
#ifndef _XBOX
    m_bResult4 = 0;
#endif
    ClearOutput();
}

inline void SetColorMaterial(int nFace, int nMode)
{
    GLRender_SetColorMaterial(nFace, nMode);
}

inline void SetDepthFunc(int nFunc)
{
    GLRender_SetDepthFunc(nFunc);
}
// PC 0x00469F50
void Scene::DoMeshBuckets()
{
    int i;
    int j;
    List<TileData*> lstTiles;

    for (i = 0; i < g_listBackgroundBucket.m_nCount; i++)
    {
        PartTriMesh* pTriMesh = g_listBackgroundBucket.m_pList[i];
        Material* pMaterial = pTriMesh->m_pMaterial;
        pMaterial->BindTexture0();
        pMaterial->SetBlendingMode(0);
        pTriMesh->Mat();
        pTriMesh->Draw(false);
    }

    List<int> lstOrdering(*AurTextureGetOrdering());

    for (i = 0; i < lstOrdering.m_nCount; i++)
    {
        unsigned int nTexture = lstOrdering.m_pList[i];
        if (meshbuckets[nTexture].m_nCount != 0)
        {
            Material* pMaterial = meshbuckets[nTexture].m_pList[0]->m_pMaterial;
            pMaterial->BindTexture0();
            pMaterial->SetBlendingMode(0);

            for (j = 0; j < meshbuckets[nTexture].m_nCount; j++)
            {
                PartTriMesh* pTriMesh = meshbuckets[nTexture].m_pList[j];
                pTriMesh->Mat();
                pTriMesh->Draw(false);
                TileData* pTile = pTriMesh->m_pTileData;
                if (pTile != 0 && !lstTiles.Contains(pTile))
                {
                    lstTiles.Add(pTile);
                }
            }
        }
    }

    for (j = 0; j < meshbuckets[0].m_nCount; j++)
    {
        PartTriMesh* pTriMesh = meshbuckets[0].m_pList[j];
        if (j == 0)
        {
            pTriMesh->m_pMaterial->BindTexture0();
        }
        pTriMesh->Mat();
        pTriMesh->Draw(false);
        if (!lstTiles.Contains(pTriMesh->m_pTileData))
        {
            lstTiles.Add(pTriMesh->m_pTileData);
        }
    }

    GLRender_EnableClientState(CLIENT_STATE_TEXTURE_COORD);
    for (i = 0; i < lstTiles.m_nCount; i++)
    {
        if (lstTiles.m_pList[i] != 0)
        {
            lstTiles.m_pList[i]->m_fHeightFadeAlpha =
                lstTiles.m_pList[i]->m_fBaseHeightFadeAlpha;
            lstTiles.m_pList[i]->m_nFadeState = TileData::FADE_NOT_CHECKED;
        }
    }
}

extern int interleaveanimations;

extern int renderpersonalspace;

void Transform(const Vector& vPosition, const Quaternion& qOrientation,
               int nForce);

extern int forcefogtest;
// PC 0x0046A280
void Scene::DoCapBucket()
{
    List<int> lstOrdering(*AurTextureGetOrdering());

    for (int i = 0; i < lstOrdering.m_nCount; i++)
    {
        unsigned int nTexture = lstOrdering.m_pList[i];
        if (capbuckets[nTexture].m_nCount != 0)
        {
            Material* pMaterial = capbuckets[nTexture].m_pList[0]->m_pMaterial;
            pMaterial->BindTexture0();
            pMaterial->SetBlendingMode(0);

            for (int j = 0; j < capbuckets[nTexture].m_nCount; j++)
            {
                Part* pPart = capbuckets[nTexture].m_pList[j];
                pPart->Mat();
                pPart->Draw(false);
            }
        }
    }

    for (int i = 0; i < capbuckets[0].m_nCount; i++)
    {
        Part* pPart = capbuckets[0].m_pList[i];
        if (i == 0)
        {
            pPart->m_pMaterial->BindTexture0();
        }
        pPart->Mat();
        pPart->Draw(false);
    }
}

#define DEBUG_FADEBUCKETS 0
// PC 0x0046A3A0
void Scene::DoFadeBuckets()
{
    int i;
    int j;
    float fAlpha;
    List<TileData*> lstTiles;
    List<int> lstOrdering(*AurTextureGetOrdering());

    m_pLightManager->EnableLightsDynamicOnly();
    GLRender_Enable(ENABLE_BLEND);
    SetColorMaterial(FACING_FRONT, 0);
    GLRender_EnableColorMaterial();
    if (LightManager::m_pOnlyGob != 0)
        GLRender_EnableClientState(CLIENT_STATE_COLOR);

    GLfloat afWhite[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat afBlack[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    GLRender_SetMaterialv(FACING_FRONT, MATERIAL_AMBIENT, afWhite);
    GLRender_SetMaterialv(FACING_FRONT, MATERIAL_DIFFUSE, afWhite);
    GLRender_SetMaterialv(FACING_FRONT, MATERIAL_EMISSION, afBlack);

    if (DEBUG_FADEBUCKETS)
    {
        for (i = 0; i < 256; i++)
        {
            for (j = 0; j < fadebuckets[i].m_nCount; j++)
            {
                fadebuckets[i].m_pList[j]->m_pTileData->m_fHeightFadeAlpha = 1.0f;
                fadebuckets[i].m_pList[j]->m_pTileData->m_fBaseHeightFadeAlpha = 1.0f;
                fadebuckets[i].m_pList[j]->m_pTileData->m_fFadeAlpha = 1.0f;
                fadebuckets[i].m_pList[j]->m_pTileData->m_nFadeState = TileData::FADE_NOT_CHECKED;
                fadebuckets[i].m_pList[j]->m_pMaterial->BindTexture0();
                fadebuckets[i].m_pList[j]->m_pMaterial->SetBlendingMode(0);
                fadebuckets[i].m_pList[j]->Mat();
                fadebuckets[i].m_pList[j]->Draw(false);
            }
        }
        return;
    }

    for (i = 0; i < lstOrdering.m_nCount; i++)
    {
        unsigned int nTexture = lstOrdering.m_pList[i];
        if (fadebuckets[nTexture].m_nCount != 0)
        {
            Material* pMaterial = fadebuckets[nTexture].m_pList[0]->m_pMaterial;
            pMaterial->BindTexture0();
            pMaterial->SetBlendingMode(0);
            for (j = 0; j < fadebuckets[nTexture].m_nCount; j++)
            {
                PartTriMesh* pTriMesh = fadebuckets[nTexture].m_pList[j];
                fAlpha = pTriMesh->m_fAlpha;
                pTriMesh->m_fAlpha *= pTriMesh->m_pTileData->m_fHeightFadeAlpha;
                if (pTriMesh->m_fAlpha > 0.0f)
                {
                    pTriMesh->Mat();
                    if (pTriMesh->m_pTileData != 0 &&
                        pTriMesh->m_pTileData->m_fFadeAlpha < 1.0f)
                    {
                        GLRender_Enable(ENABLE_FOG);
                        float fFade = pTriMesh->m_pTileData->m_fFadeAlpha *
                            pTriMesh->m_pTileData->m_fFadeAlpha;
                        Vector vFogColor = fFade * m_vFogColor;
                        GLRender::SetFogModev(FOG_COLOR, vFogColor);
                        GLRender::SetFogModef(FOG_DENSITY, (m_fFogDensity + m_fFogDensity));
                        GLRender::SetFogModef(FOG_START,
                            (pTriMesh->m_pTileData->m_fFadeAlpha * m_fFogNear));
                        GLRender::SetFogModef(FOG_END,
                            (pTriMesh->m_pTileData->m_fFadeAlpha * m_fFogFar) + 0.1f);
                        pTriMesh->Draw(false);
                        if (!m_bFog)
                            GLRender_Disable(ENABLE_FOG);
                        GLRender::SetFogModef(FOG_DENSITY, m_fFogDensity);
                        GLRender::SetFogModef(FOG_START, m_fFogNear);
                        GLRender::SetFogModef(FOG_END, m_fFogFar);
                        GLRender::SetFogModev(FOG_COLOR, m_vFogColor);
                    }
                    else
                        pTriMesh->Draw(false);
                }
                pTriMesh->m_fAlpha = fAlpha;
                if (!lstTiles.Contains(pTriMesh->m_pTileData))
                    lstTiles.Add(pTriMesh->m_pTileData);
            }
        }
    }

    for (j = 0; j < fadebuckets[0].m_nCount; j++)
    {
        PartTriMesh* pTriMesh = fadebuckets[0].m_pList[j];
        if (j == 0)
            pTriMesh->m_pMaterial->BindTexture0();
        fAlpha = pTriMesh->m_fAlpha;
        pTriMesh->m_fAlpha *= pTriMesh->m_pTileData->m_fHeightFadeAlpha;
        if (pTriMesh->m_fAlpha > 0.0f)
        {
            pTriMesh->Mat();
            if (pTriMesh->m_pTileData != 0 &&
                pTriMesh->m_pTileData->m_fFadeAlpha < 1.0f)
            {
                GLRender_Enable(ENABLE_FOG);
                float fFade = pTriMesh->m_pTileData->m_fFadeAlpha *
                    pTriMesh->m_pTileData->m_fFadeAlpha;
                Vector vFogColor = fFade * m_vFogColor;
                GLRender::SetFogModev(FOG_COLOR, vFogColor);
                GLRender::SetFogModef(FOG_DENSITY, (m_fFogDensity + m_fFogDensity));
                GLRender::SetFogModef(FOG_START,
                    (pTriMesh->m_pTileData->m_fFadeAlpha * m_fFogNear));
                GLRender::SetFogModef(FOG_END,
                    (pTriMesh->m_pTileData->m_fFadeAlpha * m_fFogFar) + 0.1f);
                pTriMesh->Draw(false);
                if (!m_bFog)
                    GLRender_Disable(ENABLE_FOG);
                GLRender::SetFogModef(FOG_DENSITY, m_fFogDensity);
                GLRender::SetFogModef(FOG_START, m_fFogNear);
                GLRender::SetFogModef(FOG_END, m_fFogFar);
                GLRender::SetFogModev(FOG_COLOR, m_vFogColor);
            }
            else
                pTriMesh->Draw(false);
        }
        pTriMesh->m_fAlpha = fAlpha;
        if (!lstTiles.Contains(pTriMesh->m_pTileData))
            lstTiles.Add(pTriMesh->m_pTileData);
    }

    for (i = 0; i < lstTiles.m_nCount; i++)
    {
        lstTiles.m_pList[i]->m_fHeightFadeAlpha =
            lstTiles.m_pList[i]->m_fBaseHeightFadeAlpha;
        lstTiles.m_pList[i]->m_nFadeState = TileData::FADE_NOT_CHECKED;
    }
    GLRender_DisableClientState(CLIENT_STATE_COLOR);
    GLRender_DisableColorMaterial();
    GLRender_Disable(ENABLE_BLEND);
}
// PC 0x0046AA90
void Scene::DoStaticMeshShadows()
{
    PartLight* pShadowLight = m_pLightManager->m_pShadowLight;
    List<PartLight*> lstShadowLights(
        *m_pLightManager->GetShadowLights(LightManager::SHADOWLIGHTS_STATIC));

    if (lstShadowLights.m_nCount == 1 &&
        pShadowLight != lstShadowLights.m_pList[0])
    {
        return;
    }

    for (int i = 0; i < staticshadowbucket.m_nCount; i++)
    {
        staticshadowbucket.m_pList[i]->Draw(false);
    }
}
// PC 0x0046AB40
void Scene::DoHighlightTriggers(bool bHighlight)
{
    List<PartTrigger*> lstTriggers;
    int nStencilBits = AurGetAvailableStencilBits();
    if (nStencilBits == 0)
        return;

    m_fHighlightTime += DeltaT * TRIGGER_HIGHLIGHT_PHASE_RATE;
    if (enablerender)
    {
        GLRender_Disable(ENABLE_BLEND);
        GLRender_Disable(ENABLE_TEXTURE_2D);
        GLRender_Disable(ENABLE_LIGHTING);
        GLRender_Disable(ENABLE_ALPHA_TEST);
        for (int i = 0; i < triggerbucket.m_nCount; ++i)
        {
            PartTrigger* pTrigger = triggerbucket[i];
            bool bHighlighted = pTrigger->m_bHighlighted;
            if (bHighlighted && bHighlight == pTrigger->m_bHighlightReverse)
                lstTriggers.Add(pTrigger);
        }

        for (int i = 0; i < lstTriggers.m_nCount; ++i)
        {
#ifdef _XBOX
            ClearStencilBuffer();
#else
            int nAvailableBits = AurGetAvailableStencilBits();
            if (nAvailableBits)
            {
                int nClearValue = (1 << (nAvailableBits - 1)) - 1;
                GLRender_ClearStencilValue(nClearValue);
            }
            GLRender_SetStencilMask(GLRender::STENCIL_WRITE_MASK);
            GLRender_Clear(4);
#endif
            GLRender_SetStencilMask(GLRender::STENCIL_WRITE_MASK);
            GLRender_Enable(ENABLE_STENCIL_TEST);
            GLRender_SetDepthMask(GL_FALSE);
            GLRender_SetColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
            GLRender_SetStencilFunc(CMP_ALWAYS, TRIGGER_HIGHLIGHT_STENCIL_MASK, TRIGGER_HIGHLIGHT_STENCIL_MASK);
            GLRender_SetStencilOp(STENCIL_KEEP, STENCIL_INCREMENT, STENCIL_KEEP);
            GLRender_SetFrontFace(CULL_CLOCKWISE);
            lstTriggers[i]->DrawVolume();
            GLRender_SetStencilOp(STENCIL_KEEP, STENCIL_DECREMENT, STENCIL_KEEP);
            GLRender_SetFrontFace(CULL_COUNTERCLOCKWISE);
            lstTriggers[i]->DrawVolume();
            GLRender_SetFrontFace(CULL_COUNTERCLOCKWISE);
            GLRender_SetColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

            List<Vector> lstVertices;
            lstVertices.Add(normalize(cross(
                CurrentCamera->m_lstViewVolume[Camera::VIEW_PLANE_BOTTOM].m_vNormal,
                CurrentCamera->m_lstViewVolume[Camera::VIEW_PLANE_LEFT].m_vNormal)));
            lstVertices.Add(normalize(cross(
                CurrentCamera->m_lstViewVolume[Camera::VIEW_PLANE_RIGHT].m_vNormal,
                CurrentCamera->m_lstViewVolume[Camera::VIEW_PLANE_BOTTOM].m_vNormal)));
            lstVertices.Add(normalize(cross(
                CurrentCamera->m_lstViewVolume[Camera::VIEW_PLANE_TOP].m_vNormal,
                CurrentCamera->m_lstViewVolume[Camera::VIEW_PLANE_RIGHT].m_vNormal)));
            lstVertices.Add(normalize(cross(
                CurrentCamera->m_lstViewVolume[Camera::VIEW_PLANE_LEFT].m_vNormal,
                CurrentCamera->m_lstViewVolume[Camera::VIEW_PLANE_TOP].m_vNormal)));
            GLRender_PushCurrentMatrix();
            float fZ = CurrentCamera->m_vPosition.m_fZ;
            float fY = CurrentCamera->m_vPosition.m_fY;
            float fX = CurrentCamera->m_vPosition.m_fX;
            GLRender_Translate(fX, fY, fZ);
            GLRender_Enable(ENABLE_BLEND);
            GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
            int nStencilReference = 1 << (nStencilBits - 1);
            GLRender_SetStencilFunc(CMP_LEQUAL, nStencilReference, 0xFFFFFFFF);
            GLRender_SetStencilOp(STENCIL_KEEP, STENCIL_KEEP, STENCIL_KEEP);
            PartTrigger* pTrigger = lstTriggers[i];
            if (pTrigger->m_bHighlightColor)
            {
                float fAlpha = (sinf(pTrigger->m_fHighlightBlend * m_fHighlightTime) + 1.0f) *
                    pTrigger->m_fHighlightAlpha * 0.5f;
                float fBlue = pTrigger->m_fHighlightColorBlue;
                float fGreen = pTrigger->m_fHighlightColorGreen;
                float fRed = pTrigger->m_fHighlightColorRed;
                GLRender_SetColor4f(fRed, fGreen, fBlue, fAlpha);
            }
            else
                SetColor4fv(&pTrigger->m_fHighlightColorRed);
            GLRender::DrawFromVerts(Aurora::PRIMITIVE_QUADS, lstVertices);
            GLRender_PopCurrentMatrix();
            GLRender_Disable(ENABLE_STENCIL_TEST);
            GLRender_SetDepthMask(GL_TRUE);
        }
        GLRender_Enable(ENABLE_ALPHA_TEST);
        GLRender_Enable(ENABLE_BLEND);
        GLRender_Enable(ENABLE_TEXTURE_2D);
        GLRender_Enable(ENABLE_LIGHTING);
    }
}
// PC 0x0046B1D0
int Scene::LoadVisibility(char* sName)
{
    int i;
    int nVisibleRooms;
    void* pVisibility;
    char sRoomName[64];
    char sVisibleRoomName[64];
    char* sLine;
    CAurRoom* pRoom;
    CAurRoom* pVisibleRoom;

#ifdef _XBOX
    ClearVisibility();
#else
    for (i = 0; i < m_lstRooms.m_nCount; i++)
    {
        m_lstRooms.m_pList[i]->m_lstVisibleRooms.m_nCount = 0;
    }
#endif

    pVisibility = AurResGet(sName, ".vis", 0, true);
    if (pVisibility == 0)
    {
        return 0;
    }

    for (sLine = AurResGetNextLine(); sLine != 0; sLine = AurResGetNextLine())
    {
        sRoomName[0] = 0;
        nVisibleRooms = 0;
        sscanf(sLine, "%s%d", sRoomName, &nVisibleRooms);
        pRoom = FindRoom(sRoomName);
        if (pRoom != 0 && nVisibleRooms != 0)
        {
            for (int j = 0; j < nVisibleRooms; j++)
            {
                sVisibleRoomName[0] = 0;
                sLine = AurResGetNextLine();
                sscanf(sLine, "%s", sVisibleRoomName);
                pVisibleRoom = FindRoom(sVisibleRoomName);
                if (pVisibleRoom != 0)
                {
                    SetVisibility(pRoom, pVisibleRoom, 1);
                }
            }
        }
    }

    AurResFree(pVisibility, 0);
    return 1;
}
// PC 0x00469140
void Scene::SetVisibility(CAurRoom* pRoom, CAurRoom* pVisibleRoom,
                          int bVisible)
{
    if (bVisible)
    {
        if (!pRoom->m_lstVisibleRooms.Contains(pVisibleRoom))
        {
            pRoom->m_lstVisibleRooms.Add(pVisibleRoom);
            pVisibleRoom->m_lstVisibleRooms.Add(pRoom);
        }
    }
    else
    {
        if (pRoom->m_lstVisibleRooms.Contains(pVisibleRoom))
        {
            pRoom->m_lstVisibleRooms.Remove(pVisibleRoom);
            pVisibleRoom->m_lstVisibleRooms.Remove(pRoom);
        }
    }
}
// PC 0x00469220
Gob* SpawnPartsForTile(char* sName, const Vector& vPosition,
                       const Quaternion& qOrientation, int nRotation,
                       int bMeshesOnly, List<Part*>& lstParts)
{
    Model* pModel = FindModel(sName);
    if (!pModel)
        return 0;

    else
    {
        AurTextureSetModeStatic();
        Gob* pGob = 0;
        Part* pPart;
        char sPartName[64];
        if (!bMeshesOnly)
        {
            strcpy(sPartName, pModel->m_sName);
            strcat(sPartName, "a");
        }
        if (!bMeshesOnly && findnode(pModel->m_pRootNode, sPartName))
        {
            pGob = (Gob*)NewCAurObject(sName, "", 0, 0);
            pPart = pGob->m_pRootPart;
        }
        else
        {
            pPart = CreateInstanceTree(pModel->m_pRootNode, 0);
            sprintf(sPartName, "(%d,%d)", (int)floor(vPosition.m_fX),
                    (int)floor(vPosition.m_fY));
            PartInitMaterial(0, pPart, sPartName);
        }
        SetPartTextureRotate(pPart, nRotation);
        ExpandReferenceParts(pPart);
        CollapsePartTree(pGob, pPart, lstParts, vPosition, qOrientation, 0, 0);
        AurTextureSetModeDynamic();
        return pGob;
    }
}
bool ProcessTilePart(TileData* pTile, Part* pPart, char* sName);
void ComputePartBBox(PartTriMesh* pTriMesh);
#ifdef _XBOX
Vector Round(const Vector& vValue, float fRoundTo);
#endif
// PC 0x00469380
void ProcessStaticParts(List<Part*>& lstParts, Scene* pScene,
                        TileData* pTile, char* sName, int bMeshesOnly)
{
    for (int i = 0; i < lstParts.m_nCount; i++)
    {
        Part* pPart = lstParts[i];
        PartTriMesh* pTriMesh = pPart->AsPartTriMesh();
        bool bKeep = ProcessTilePart(pTile, pPart, sName);

        bool bDelete = false;
        if (pTriMesh)
        {
            pTriMesh->m_pTileData = pTile;
            if (!pTriMesh->AsPartAnimMesh())
            {
                ComputePartBBox(pTriMesh);
            }
            if (bMeshesOnly && pTriMesh->AsPartAABB())
            {
                bDelete = true;
            }
        }
        else
        {
            PartEmitter* pEmitter = pPart->AsPartEmitter();
            if (bMeshesOnly || (!bKeep && !pEmitter && !pPart->AsPartLight()))
            {
                bDelete = true;
            }
            else if (pEmitter)
            {
                pEmitter->m_pTileData = pTile;
            }
        }

        if (!bDelete)
        {
            pTile->m_lstParts.Add(pPart);
        }
        else
        {
            delete pPart;
        }
    }
}
// PC 0x004669C0
bool ProcessTilePart(TileData* pTile, Part* pPart, char* sName)
{
    char sPartName[64];
    strcpy(sPartName, sName);
    strcat(sPartName, "ml1");
    if (stricmp(pPart->m_pNode->TranslateNodeName(), sPartName) == 0)
    {
        pTile->m_pMainLight = pPart->AsPartLight();
        return true;
    }
    strcpy(sPartName, sName);
    strcat(sPartName, "ml2");
    if (stricmp(pPart->m_pNode->TranslateNodeName(), sPartName) == 0)
    {
        pTile->m_pSourceLight = pPart->AsPartLight();
        return true;
    }
    strcpy(sPartName, sName);
    strcat(sPartName, "sl1");
    if (stricmp(pPart->m_pNode->TranslateNodeName(), sPartName) == 0)
    {
        pTile->m_pFlamePart1 = pPart;
        return true;
    }
    strcpy(sPartName, sName);
    strcat(sPartName, "sl2");
    if (stricmp(pPart->m_pNode->TranslateNodeName(), sPartName) == 0)
    {
        pTile->m_pFlamePart2 = pPart;
        return true;
    }
    return false;
}
// PC 0x00466B70
CAurRoom::~CAurRoom()
{
    delete m_pRoomObject;

    for (int i = 0; i < m_lstAnimatedObjects.m_nCount; i++)
    {
        delete m_lstAnimatedObjects[i];
    }
}
// PC 0x00466C20
CAurRoom* Scene::FindRoom(char* sName)
{
    int i;

    for (i = 0; i < m_lstRooms.m_nCount; i++)
    {
        if (stricmp(sName, m_lstRooms[i]->m_sName) == 0)
        {
            return m_lstRooms[i];
        }
    }

    return 0;
}
// PC 0x00466C80
int Scene::SaveVisibility(char* sPath, char* sName)
{
    char sFileName[256];

    if (sPath == 0 || *sPath == 0)
    {
        sPath = ".";
    }

    sprintf(sFileName, "%s/%s.VIS", sPath, sName);
    FILE* pFile = fopen(sFileName, "w");
    if (pFile == 0)
    {
        return pFile != 0;
    }

    for (int i = 0; i < m_lstRooms.m_nCount; i++)
    {
        fprintf(pFile, "%s %d\n", m_lstRooms[i]->m_sName,
                m_lstRooms[i]->m_lstVisibleRooms.m_nCount);
        for (int j = 0; j < m_lstRooms[i]->m_lstVisibleRooms.m_nCount; j++)
        {
            fprintf(pFile, "  %s\n",
                    m_lstRooms[i]->m_lstVisibleRooms[j]->m_sName);
        }
    }

    fclose(pFile);
    return 1;
}
// PC 0x00466D70
CAurRoom* Scene::GetRoom(int nRoom)
{
    if (nRoom >= 0 && nRoom < m_lstRooms.m_nCount)
    {
        return m_lstRooms[nRoom];
    }

    return 0;
}
// PC 0x00466DF0
int Scene::SetCurrentRoom(int nRoom)
{
    int nResult = nRoom;

    if (nRoom >= 0 && nRoom < m_lstRooms.m_nCount)
    {
        nResult = (int)m_lstRooms[nRoom];
        m_pCurrentRoom = (CAurRoom*)nResult;
    }
    else
    {
        m_pCurrentRoom = 0;
    }

    return nResult;
}
// PC 0x00466E20
void Scene::AddFacesToBins(PartAABB* pAABB, const Vector& vMinimum,
                           List<List<CAurTriangleBin*> >& lstBins)
{
    MdlNodeTriMesh* pNode = pAABB->m_pNode->AsMdlNodeTriMesh();
    Vector vPosition;
    Quaternion qOrientation;
    pAABB->GetWorld(vPosition, qOrientation);

#ifdef _XBOX

    unsigned long nVertexPool = pNode->m_pModel->AsModel()->GetVertexPool();
    void* pBuffer = GLRender::LockPool(nVertexPool, 0, 0,
        RequestPoolManager::POOL_LOCK_READ_ONLY);
    unsigned long nOffset = pNode->m_nVertexDataOffset;
#endif
    for (int i = 0; i < pNode->m_lFaces.m_nCount; i++)
    {
        MaxFace& face = pNode->m_lFaces.m_pList[i];
        if (!AurGrassMaterialInUse(face.m_nSurfaceType))
            continue;

#ifndef _XBOX
        unsigned long nVertexPool = pNode->m_pModel->AsModel()->GetVertexPool();
        void* pBuffer = GLRender::LockPool(nVertexPool, 0, 0,
            RequestPoolManager::POOL_LOCK_READ_ONLY);
        unsigned long nOffset = pNode->m_nVertexDataOffset;
#endif
        Vector vCenter;
        Vector* pVertex = (Vector*)((char*)pBuffer + nOffset + pNode->m_nPositionOffset +
            pNode->m_nVertexStride * face.m_anVertexIndices[0]);
        vCenter.m_fX = pVertex->m_fX;
        vCenter.m_fY = pVertex->m_fY;
        vCenter.m_fZ = pVertex->m_fZ;
        pVertex = (Vector*)((char*)pBuffer + nOffset + pNode->m_nPositionOffset +
            pNode->m_nVertexStride * face.m_anVertexIndices[1]);
        vCenter.m_fX += pVertex->m_fX;
        vCenter.m_fY += pVertex->m_fY;
        vCenter.m_fZ += pVertex->m_fZ;
        int nIndex = face.m_anVertexIndices[2];
        pVertex = (Vector*)((char*)pBuffer + nOffset + pNode->m_nPositionOffset +
            pNode->m_nVertexStride * nIndex);
        vCenter.m_fX += pVertex->m_fX;
        vCenter.m_fY += pVertex->m_fY;
        vCenter.m_fZ += pVertex->m_fZ;
        vCenter = vCenter / 3.0f;
        vCenter = vPosition + qOrientation * vCenter;
#ifdef _XBOX
        vCenter = Round(vCenter, 0.001f);
#else
        vCenter = Vector(Round(vCenter.m_fX, 0.001f),
                         Round(vCenter.m_fY, 0.001f),
                         Round(vCenter.m_fZ, 0.001f));
#endif

        int nX;
        int nY;
        roundToInt(nX, (float)floor((vCenter.m_fX - vMinimum.m_fX) / binsize));
        roundToInt(nY, (float)floor((vCenter.m_fY - vMinimum.m_fY) / binsize));
        nX = max(0, min(nX, lstBins.m_nCount - 1));
        nY = max(0, min(nY, lstBins[nX].m_nCount - 1));
        if (!lstBins[nX][nY])
            lstBins[nX][nY] = new CAurTriangleBin;
#ifndef _XBOX
        nVertexPool = pNode->m_pModel->AsModel()->m_nVertexPool;
        GLRender::UnlockPool(nVertexPool);
#endif
        lstBins[nX][nY]->AddTriangle(face, pAABB, pNode);
        m_bBinsPopulated = 1;
    }
#ifdef _XBOX
    nVertexPool = pNode->m_pModel->AsModel()->m_nVertexPool;
    GLRender::UnlockPool(nVertexPool);
#endif
}

Scene* FindScene(char* sName);
// PC 0x00467310
char AurSetGlobalWind(Vector vWind)
{
    Scene* pScene = FindScene("mainscene");
    if (pScene == 0)
    {
        pScene = FindScene("scene");
    }

    if (pScene != 0)
    {
        Vector vGlobalWind(vWind.m_fX, vWind.m_fY, vWind.m_fZ);
        pScene->m_pWindManager->SetGlobalWind(vGlobalWind);
        return 1;
    }

    return 0;
}
// PC 0x00469480
CAurObject* Scene::SpawnTile(char* sName, const Vector& vPosition,
                            int nRotation, int bMeshesOnly)
{
    Quaternion qOrientation = YawPitchRoll(nRotation * AUR_TILE_ROTATION_DEGREES, 0.0f, 0.0f);
    List<Part*> lstParts;
    Gob* pGob = SpawnPartsForTile(sName, vPosition, qOrientation,
                                 nRotation, bMeshesOnly, lstParts);
    TileData* pTile = new TileData;
    pTile->m_vPosition = vPosition;
    pTile->m_bSuppliesMaterial = bMeshesOnly;
    if (pGob)
        ProcessStaticObject(pGob, this, pTile, vPosition, qOrientation);
    ProcessStaticParts(lstParts, this, pTile, sName, bMeshesOnly);

    int i = 0;
    if (pTile->m_lstParts.m_nCount > 0)
    {
        while (i < pTile->m_lstParts.m_nCount && !pTile->m_lstParts[i]->AsPartTriMesh())
            i++;
        if (pTile->m_lstParts[i]->AsPartTriMesh())
        {
            pTile->m_vMinimum = pTile->m_lstParts[i]->AsPartTriMesh()->m_vMinimum;
            pTile->m_vMaximum = pTile->m_lstParts[i]->AsPartTriMesh()->m_vMaximum;
            for (i++; i < pTile->m_lstParts.m_nCount; i++)
            {
                PartTriMesh* pTriMesh = pTile->m_lstParts[i]->AsPartTriMesh();
                if (pTriMesh)
                {
                    pTile->m_vMinimum = VectorMin(pTile->m_vMinimum, pTriMesh->m_vMinimum);
                    pTile->m_vMaximum = VectorMax(pTile->m_vMaximum, pTriMesh->m_vMaximum);
                }
            }
        }
    }
    SceneBSPAddTileData(this, pTile, vPosition);

    for (i = -1; i <= 1; i += 2)
    {
        TileData* pAdjacent = SceneBSPGetTileData(this, vPosition + Vector(i * cellsize, 0.0f, 0.0f));
        if (pAdjacent && pAdjacent != pTile)
        {
            pTile->m_lstAdjacentTiles.Add(pAdjacent);
            pAdjacent->m_lstAdjacentTiles.Add(pTile);
            for (int j = 0; j < pTile->m_lstLocalReferences.m_nCount; j++)
                pAdjacent->m_lstAdjacentReferences.Add(pTile->m_lstLocalReferences[j]);
            for (int j = 0; j < pAdjacent->m_lstLocalReferences.m_nCount; j++)
                pTile->m_lstAdjacentReferences.Add(pAdjacent->m_lstLocalReferences[j]);
        }
    }
    for (i = -1; i <= 1; i += 2)
    {
        TileData* pAdjacent = SceneBSPGetTileData(this, vPosition + Vector(0.0f, i * cellsize, 0.0f));
        if (pAdjacent && pAdjacent != pTile)
        {
            pTile->m_lstAdjacentTiles.Add(pAdjacent);
            pAdjacent->m_lstAdjacentTiles.Add(pTile);
            for (int j = 0; j < pTile->m_lstLocalReferences.m_nCount; j++)
                pAdjacent->m_lstAdjacentReferences.Add(pTile->m_lstLocalReferences[j]);
            for (int j = 0; j < pAdjacent->m_lstLocalReferences.m_nCount; j++)
                pTile->m_lstAdjacentReferences.Add(pAdjacent->m_lstLocalReferences[j]);
        }
    }
    return pGob;
}
// PC 0x00467380
bool AurSetGlobalWindVariation(float fMaximumDeviation, float fVariation)
{
    Scene* pScene = FindScene("mainscene");
    if (pScene == 0)
    {
        pScene = FindScene("scene");
    }

    if (pScene != 0)
    {
        float fVariationRadians = fVariation * DEG_TO_RAD;
        float fDeviationRadians = fMaximumDeviation * DEG_TO_RAD;
        pScene->m_pWindManager->SetMaximumDeviation(fDeviationRadians,
                                                    fVariationRadians);
        return true;
    }

    return false;
}
// PC 0x00463F00
Scene* FindScene(char* sName)
{
    for (int i = 0; i < Scenes.m_nCount; i++)
    {
        if (stricmp(sName, Scenes.m_pList[i]->m_sName) == 0)
        {
            return Scenes.m_pList[i];
        }
    }

    return 0;
}
// PC 0x00463F50
int Scene::GetObjectA(char* sName)
{
    for (int i = 0; i < m_lstObjects.m_nCount; i++)
    {
        if (stricmp(sName, m_lstObjects.m_pList[i]->m_sName) == 0)
        {
            return (int)m_lstObjects.m_pList[i];
        }
    }

    return 0;
}
// PC 0x00463FA0
CAurObject* Scene::HitCheckGob(Vector vStart, Vector vEnd, Vector* pHit,
                              CHitInfo* pHitInfo)
{
    CAurObject* pHitObject = 0;
    for (int i = 0; i < m_lstObjects.m_nCount; i++)
    {
        Gob* pGob = m_lstObjects.m_pList[i];
        if (pGob->m_pModel &&
#ifdef _XBOX
            (bool(pGob->m_pModel->m_Classification.m_bEmitterBounds) ||
             pGob->m_bDistortion ||
#else
            ((bool(pGob->m_pModel->m_Classification.m_bEmitterBounds) &&
              !pHitInfo->m_bResult4) || pGob->m_bDistortion ||
#endif
             bool(pGob->m_pModel->m_Classification.m_bSceneHitDisabled) ||
             pGob->IsSceneHitExcluded()))
            continue;

        if (pHitInfo->m_bResult0 && pGob->m_pModel &&
            (bool(pGob->m_pModel->m_Classification.m_bCharacter) ||
             bool(pGob->m_pModel->m_Classification.m_bRenderEarly) ||
             (bool(pGob->m_pModel->m_Classification.m_bAlternateShadowCaster) &&
              !pGob->HasHitFilterOverride())))
            continue;

#ifdef _XBOX
        if (!pHitInfo->m_lstHits.Contains(pGob))
#else
        int nFound = 0;
        for (int j = 0; j < pHitInfo->m_lstHits.m_nCount; j++)
        {
            if (pHitInfo->m_lstHits.m_pList[j] == pGob)
                nFound++;
        }
        if (!nFound)
#endif
        {
            CAurObject* pObject = pGob->HitCheck(vStart, vEnd, &vEnd, pHitInfo);

            if (pObject)
                pHitObject = pObject;
        }
    }

    if (pHitObject && pHit)
        *pHit = vEnd;
    return pHitObject;
}
// PC 0x004640F0
void ProjectDynamicShadow(Gob* pGob, PartLight* pLight)
{
    if (!systemmemoryshadows)
    {
        Plane* pPlane;
        if (pLight->m_fRadius > AUR_PRIMARY_SHADOW_RADIUS)
            pPlane = new Plane(Vector(0.0f, 0.0f, -1.0f), -0.1f);
        else
        {
            Plane& planeFar = CurrentCamera->m_lstViewVolume.m_pList[Camera::VIEW_PLANE_FAR];
            pPlane = new Plane(planeFar.m_vNormal, planeFar.m_fDistance + 0.05f);
        }

        float fRadius = pLight->m_fRadius;
        Vector vPosition = pLight->m_vWorldPosition + Vector(0.0f, 0.0f, pLight->m_fShadowHeight);
        if (pLight->m_fShadowRadius > 0.0f)
            fRadius = pLight->m_fShadowRadius;
        pGob->PartVolumeProject(pGob->m_pRootPart, vPosition, fRadius, *pPlane);
        delete pPlane;
    }
    else
    {
        float fRadius = pLight->m_fRadius;
        Vector vPosition = pLight->m_vWorldPosition + Vector(0.0f, 0.0f, pLight->m_fShadowHeight);
        if (pLight->m_fShadowRadius > 0.0f)
            fRadius = pLight->m_fShadowRadius;
        pGob->PartVolumeProject(pGob->m_pRootPart, vPosition, fRadius, Plane());
    }
}
// PC 0x004642F0
void Scene::DoBinBucket()
{
    if (enablegrass == 0)
    {
        return;
    }

    int bAlphaTestEnabled = GLRender_IsEnabled(ENABLE_ALPHA_TEST);
    if (!bAlphaTestEnabled)
    {
        GLRender_Enable(ENABLE_ALPHA_TEST);
    }
    GLRender_Enable(ENABLE_BLEND);
    GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
    GLRender_Disable(ENABLE_CULL_FACE);
#ifndef _XBOX
    if (AurMultiTextureAvailable() != 0)
    {
        glClientActiveTextureARB(GL_TEXTURE0_ARB);
    }
#endif
    GLRender::cm_uiCurrentStage = 0;
    if (LightManager::m_pOnlyGob != 0)
    {
        GLRender_EnableClientState(CLIENT_STATE_COLOR);
    }
    GLRender_DisableClientState(CLIENT_STATE_COLOR);
    GLRender_SetColor4f(0.0f, 0.0f, 0.0f, 1.0f);
    GLRender_SetAlphaFunc(CMP_GREATER, grassalphatest);

    for (int i = 0; i < g_GrassInfo.m_nCount; i++)
    {
        if (AurGrassMaterialInUse(i))
        {
            for (int j = 0; j < binbucket.m_nCount; j++)
            {
                binbucket.m_pList[j]->RenderGrassPolys(
                    i, CurrentCamera->m_vPosition, m_pWindManager);
            }
        }
    }

    GLRender_SetAlphaFunc(CMP_GREATER, alphatest);
    PartTriMesh::m_bMaterialHasChanged = true;
    GLRender_Enable(ENABLE_CULL_FACE);
    if (!bAlphaTestEnabled)
    {
        GLRender_Disable(ENABLE_ALPHA_TEST);
    }
}
// PC 0x00464460
int __cdecl GobOrderCmp(const void* pGob1, const void* pGob2)
{
    Gob* pGobA = *(Gob**)pGob1;
    Gob* pGobB = *(Gob**)pGob2;

    if (pGobA->m_fAlpha < pGobB->m_fAlpha)
        return -1;
    if (pGobA->m_fAlpha > pGobB->m_fAlpha)
        return 1;

    GobModel* pModelA = (GobModel*)pGobA->m_pModel;
    GobModel* pModelB = (GobModel*)pGobB->m_pModel;

    char bRenderFirstA = pModelA->m_bRenderFirst;
    char bRenderFirstB = pModelB->m_bRenderFirst;

    if (bRenderFirstA && !bRenderFirstB)
        return -1;
    if (!bRenderFirstA && bRenderFirstB)
        return 1;

    if (newsorting != 0)
    {
        if (pGobA->m_bHasTransparency)
        {
            if (!pGobB->m_bHasTransparency)
                return -1;
        }
        if (!pGobA->m_bHasTransparency && pGobB->m_bHasTransparency)
        {
            return 1;
        }
    }

    char bCharacterA = pModelA->m_bCharacter;
    char bCharacterB = pModelB->m_bCharacter;
    char bDoorA = pModelA->m_bDoor;
    char bDoorB = pModelB->m_bDoor;

    if (!g_ForceSight)
    {
        if (bDoorA && !bDoorB)
            return 1;
        if (!bDoorA && bDoorB)
            return -1;
        if (bCharacterA && !bCharacterB)
            return -1;
        if (!bCharacterA && bCharacterB)
            return 1;
    }
    else
    {
        if (bDoorA && !bDoorB)
            return -1;
        if (!bDoorA && bDoorB)
            return 1;
        if (bCharacterA && !bCharacterB)
            return 1;
        if (!bCharacterA && bCharacterB)
            return -1;
    }

    if (pGobA->m_fZOrder > pGobB->m_fZOrder)
        return 1;
    return -1;
}
// PC 0x004645B0
void Scene::DoDistortionBuckets()
{
    GLRender_Enable(ENABLE_STENCIL_TEST);
    SetColorMask(0, 0, 0, 0);
    GLRender_SetStencilOp(STENCIL_KEEP, STENCIL_KEEP, STENCIL_REPLACE);
    GLRender_SetStencilMask(GLRender::DISTORTION_STENCIL_MASK);
    GLRender_SetStencilFunc(CMP_ALWAYS, 1, 0xFF);

    for (int i = 0; i < g_listForcePushBucket.m_nCount; i++)
    {
        g_listForcePushBucket.m_pList[i]->Render(false);
    }

    SetColorMask(1, 1, 1, 1);
    GLRender_SetStencilMask(GLRender::STENCIL_WRITE_MASK);
    GLRender_Disable(ENABLE_STENCIL_TEST);
}
// PC 0x00464650
void Scene::DoLightSaberBucket()
{
    GLRender_Enable(ENABLE_BLEND);

    for (int i = 0; i < g_listLightsaberBucket.m_nCount; i++)
    {
        Gob* pGob = g_listLightsaberBucket.m_pList[i];
        m_pLightManager->SetupLightsForThisGob(
            pGob->m_vPosition, pGob->m_fMinimumBoundingSphereRadius);
        m_pLightManager->PrioritizeLights(pGob->m_vPosition, false);
        if (pGob->m_pLightContainer != 0)
        {
            m_pLightManager->EnableHighestPriorityLightsFade(
                3, pGob->m_pLightContainer);
        }
        else
        {
            m_pLightManager->EnableHighestPriorityLights(3);
        }
        pGob->Render(false);
    }
}
// PC 0x004646E0
void Scene::DoBlurBuckets()
{
    numgobs = blurbucket.m_nCount;

    for (int i = 0; i < blurbucket.m_nCount; i++)
    {
        AurRenderCallback();

        m_pLightManager->SetupLightsForThisGob(
            blurbucket.m_pList[i]->m_vPosition,
            blurbucket.m_pList[i]->m_fMinimumBoundingSphereRadius);
        m_pLightManager->PrioritizeLights(
            blurbucket.m_pList[i]->m_vPosition, false);
        m_pLightManager->EnableHighestPriorityLights(3);
        blurbucket.m_pList[i]->RenderBlur();
    }
}
// PC 0x00464770
void Scene::DoGobShadowsForThisLight(PartLight& light)
{
    float fRadius = light.m_fRadius;
    if (light.m_fShadowRadius > 0.0f)
        fRadius = light.m_fShadowRadius;
    Vector vFocus = m_SceneRoot.GetPosition();

    int i = gobbucket.m_nCount;
    while (i--)
    {
        if (interleaveanimations && g_bEnableInterleavedAnimations)
        {
            if (i >= gobbucket.m_nCount)
                continue;
#ifdef _XBOX
            Gob* pAnimated = gobbucket.m_pList[i];
            if (pAnimated->m_pBehavior)
                pAnimated->Animate(DeltaT);
#else
            if (gobbucket.m_pList[i]->m_pBehavior)
                gobbucket.m_pList[i]->Animate(DeltaT);
#endif
            if (i >= gobbucket.m_nCount)
                continue;
        }

        Gob* pGob = gobbucket.m_pList[i];
#ifdef _XBOX

        if (!pGob->m_bCastShadows || !pGob->m_bHasGeometry ||
            !bool(pGob->m_pModel->m_Classification.m_bCharacter))
            continue;
#else

        if (!pGob->m_bCastShadows || !pGob->m_bHasGeometry ||
            !(bool(pGob->m_pModel->m_Classification.m_bCharacter) ||
              bool(pGob->m_pModel->m_Classification.m_bAlternateShadowCaster)))
            continue;
#endif

        Vector vPosition = pGob->m_vPosition;
        Vector vDelta = vPosition - vFocus;
        if (vDelta.m_fX * vDelta.m_fX + vDelta.m_fY * vDelta.m_fY +
                vDelta.m_fZ * vDelta.m_fZ >
            AUR_SHADOW_GOB_RANGE_SQUARED)
            continue;
        if (pGob->IsInCutscene())

        {
            Part* pDummy = FindPart(pGob->m_pRootPart, "cutscenedummy");
            Quaternion qOrientation;
            if (pDummy)
                pDummy->GetWorld(vPosition, qOrientation);
        }

        if (enablefocusshadowing &&
            magnitude(currentscene->m_vSceneFocus - vPosition) > maxshadowdist)
            continue;
        if (magnitude(light.m_vWorldPosition - vPosition) < fRadius)
            ProjectDynamicShadow(pGob, &light);
    }
}
// PC 0x004649B0
void Scene::RenderStencilShadowsForThisLight(PartLight& light, float fAlpha,
                                            int nStencilBits)
{
    float fRadius = light.m_fRadius;
    if (light.m_fShadowRadius > 0.0f)
        fRadius = light.m_fShadowRadius;
    if (fRadius == 0.0f)
        return;

#ifdef _XBOX

    static unsigned long nVertexPool = GLRender::RequestPool(
        Aurora::POOL_VERTEX, 11 * sizeof(RenderVertexVC),
        RequestPoolManager::POOL_FLARE_USAGE, RequestPoolManager::POOL_FORMAT_VC, &nVertexPool);
    RenderVertexVC* pVertexArray = (RenderVertexVC*)GLRender::LockPool(
        nVertexPool, 0, 0, RequestPoolManager::POOL_LOCK_WRITE);
#else
    static RenderVertexVC* pVertexArray = new RenderVertexVC[11];
#endif
    if (fRadius > AUR_SHADOW_MAX_RADIUS)
    {
        fRadius = AUR_SHADOW_MAX_RADIUS;
        pVertexArray[0].position[0] = m_vSceneFocus.m_fX;
        pVertexArray[0].position[1] = m_vSceneFocus.m_fY;
        pVertexArray[0].position[2] = m_vSceneFocus.m_fZ;
    }
    else
    {
#ifdef _XBOX
        float fRadiusSquared = fRadius * fRadius;
        fRadius = (float)fabs(light.m_vWorldPosition.m_fZ - m_vSceneFocus.m_fZ);
        fRadius = fRadiusSquared - fRadius * fRadius;
#else
        float fHeight = (float)fabs(light.m_vWorldPosition.m_fZ - m_vSceneFocus.m_fZ);
        fRadius = fRadius * fRadius - fHeight * fHeight;
#endif
        if (fRadius < 0.0f)
            fRadius = 0.0f;
        fRadius = (float)sqrt(fRadius);
        pVertexArray[0].position[0] = light.m_vWorldPosition.m_fX;

        pVertexArray[0].position[1] = light.m_vWorldPosition.m_fY;
        pVertexArray[0].position[2] = m_vSceneFocus.m_fZ;
    }

    GLRender_SetStencilFunc(CMP_LEQUAL, 1 << (nStencilBits - 1), -1);
    GLRender_SetStencilOp(STENCIL_KEEP, STENCIL_KEEP, STENCIL_KEEP);
    GLRender_Disable(ENABLE_DEPTH_TEST);
    GLRender_Enable(ENABLE_BLEND);
    if (enablesoftshadows && AurNV2xLevelShadersAvailable() &&
        (AurSoftShadowsAvailable() || AurATISoftShadowsAvailable()) &&
        GLRender::cm_bSetUpImageSpaceShadows)
        SetBlendMode(BLEND_ONE, BLEND_ZERO);
    else
        GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);

    fAlpha *= AUR_SHADOW_OPACITY;
    *(unsigned int*)pVertexArray[0].color = (unsigned int)(fAlpha * 255.0f) << 24;
    for (int i = 0; i < 10; i++)
    {
        float fAngle = i * AUR_SHADOW_FAN_ANGLE_STEP;
        pVertexArray[i + 1].position[0] = pVertexArray[0].position[0] + (float)cos(fAngle) * fRadius;
        pVertexArray[i + 1].position[1] = pVertexArray[0].position[1] + (float)sin(fAngle) * fRadius;
        pVertexArray[i + 1].position[2] = m_vSceneFocus.m_fZ;
        *(unsigned int*)pVertexArray[i + 1].color = 0;
    }
#ifdef _XBOX
    GLRender::UnlockPool(nVertexPool);
    GLRender::RenderObjectsVC(Aurora::PRIMITIVE_TRIANGLE_FAN, nVertexPool, 11);
#else
    GLRender::RenderObjectsVCUP(Aurora::PRIMITIVE_TRIANGLE_FAN, pVertexArray, 11);
#endif
    GLRender_Enable(ENABLE_DEPTH_TEST);
}
// PC 0x00464BF0
void Scene::DoStaticMeshBeams()
{
    if (enablebeams != 0)
    {
        SetBeaming();
        for (int i = 0; i < staticbeambucket.m_nCount; i++)
        {
            staticbeambucket.m_pList[i]->Draw(false);
        }
    }
}
// PC 0x00464C30
void Scene::DoEmitterBucket()
{
    GLRender_ClearStencilValue(0);
    GLRender_SetStencilMask(GLRender::STENCIL_WRITE_MASK);
    GLRender::Clear(4);

    if (enableemitters != 0)
    {
        GLRender_Enable(ENABLE_STENCIL_TEST);
        GLRender_Disable(ENABLE_STENCIL_TEST);
        PartEmitter::StartEmitterLayer();
        qsort(emitterbucket.m_pList, emitterbucket.m_nCount,
              sizeof(PartEmitter*), EmitterOrderCmp);

        char bAnimating = IsAnimating();
        if (enablerender != 0)
        {
            for (int i = 0; i < emitterbucket.m_nCount; i++)
            {
                Gob* pObject = emitterbucket.m_pList[i]->m_pAurObject;
                if (bAnimating || (pObject != 0 && pObject->IsForceAnimated()))
                {
                    emitterbucket.m_pList[i]->AnimateParticles(
                        DeltaT < EMITTER_MAX_TIME_STEP ? DeltaT : EMITTER_MAX_TIME_STEP);
                }
                emitterbucket.m_pList[i]->RenderFunc();
            }
        }
        PartEmitter::StopEmitterLayer();
    }
}
// PC 0x00464D40
void Scene::DoTriggerBucket()
{
    if (enablerender != 0)
    {
        GLRender_Disable(ENABLE_BLEND);
        GLRender_Disable(ENABLE_TEXTURE_2D);
        GLRender_Disable(ENABLE_LIGHTING);

        for (int i = 0; i < triggerbucket.m_nCount; i++)
        {
            PartTrigger* pTrigger = triggerbucket.m_pList[i];
            if (!pTrigger->m_bHighlighted)
            {
                pTrigger->Draw(false);
            }
        }

        GLRender_Enable(ENABLE_BLEND);
        GLRender_Enable(ENABLE_TEXTURE_2D);
        GLRender_Enable(ENABLE_LIGHTING);
    }
}
// PC 0x00464DD0
void Scene::RenderDynamicGeometry()
{
    if (enableshadows)
    {
        int nClearValue = AurGetStencilClearValue();
        GLRender_Enable(ENABLE_STENCIL_TEST);
        GLRender_SetStencilMask(GLRender::STENCIL_WRITE_MASK);
        GLRender_SetStencilFunc(CMP_ALWAYS, nClearValue, 0xFFFFFFFF);
        GLRender_SetStencilOp(STENCIL_KEEP, STENCIL_KEEP, STENCIL_REPLACE);
    }

    AurVertexProgramMakeCurrent();
    DoGobBuckets();
}
// PC 0x00464E30
void Scene::RenderMotionBlur()
{
    GLRender_EnableClientState(CLIENT_STATE_TEXTURE_COORD);
    GLRender_EnableClientState(CLIENT_STATE_VERTEX);
    GLRender_EnableClientState(CLIENT_STATE_NORMAL);
    GLRender_EnableClientState(CLIENT_STATE_COLOR);
    GLRender_Enable(ENABLE_BLEND);
    GLRender_Enable(ENABLE_DEPTH_TEST);
    GLRender_SetDepthMask(0);
    GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
    GLRender_Enable(ENABLE_LIGHTING);
    DoBlurBuckets();
    GLRender_SetDepthMask(1);
}
// PC 0x00464EB0
void Scene::RenderStaticGeometry()
{
    AurVertexProgramMakeCurrent();
    m_pLightManager->EnableLightsDynamicOnly();

    float afWhite[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    float afBlack[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    SetMaterialParameter(FACING_FRONT, MATERIAL_AMBIENT, afWhite);
    SetMaterialParameter(FACING_FRONT, MATERIAL_DIFFUSE, afWhite);
    SetMaterialParameter(FACING_FRONT, MATERIAL_EMISSION, afBlack);

    DoMeshBuckets();
    DoBinBucket();
}
// PC 0x00464F70
int Scene::RenderOnlyGobs()
{
    int i;
    int nResult;

    m_pLightManager->EnableLights();
    GLRender_Disable(ENABLE_ALPHA_TEST);
    AurVertexProgramMakeCurrent();

    nResult = m_lstObjects.m_nCount;
    for (i = 0; i < nResult; i++)
    {
        m_pLightManager->GetNearestLights(
            this, m_lstObjects.m_pList[i]->m_vPosition, 2.0f, 0);
        m_lstObjects.m_pList[i]->Render(false);
        nResult = m_lstObjects.m_nCount;
    }

    return nResult;
}
// PC 0x00464FF0
void Scene::DoFog()
{
    if (!forcefogtest)
    {
        if (m_bFog)
        {
            GLRender_Enable(ENABLE_FOG);
            SetFogMode(
                m_nFogMode == FOG_MODE_LINEAR ? FOG_MODE_LINEAR :
                m_nFogMode == FOG_MODE_EXP ? FOG_MODE_EXP : FOG_MODE_EXP2);
            if (LightManager::m_pOnlyGob)
            {
                Vector vBlack(0.0f, 0.0f, 0.0f);
                GLRender_SetFogModev(FOG_COLOR, vBlack);
            }
            else if (g_ForceSight)
            {
                GLRender_SetFogModev(FOG_COLOR, m_v80);
            }
            else
            {
                GLRender_SetFogModev(FOG_COLOR, m_vFogColor);
            }
            float fNear = m_fFogNear;
            GLRender_SetFogModef(FOG_START, fNear);
            float fFar = m_fFogFar;
            GLRender_SetFogModef(FOG_END, fFar);
            float fDensity = m_fFogDensity;
            GLRender_SetFogModef(FOG_DENSITY, fDensity);
        }
        else
        {
            GLRender_Disable(ENABLE_FOG);
        }
    }
    else
    {
#ifndef _XBOX

        m_bFog = 1;
#endif
        GLRender_Enable(ENABLE_FOG);
        SetFogMode(
            m_nFogMode == FOG_MODE_LINEAR ? FOG_MODE_LINEAR :
            m_nFogMode == FOG_MODE_EXP ? FOG_MODE_EXP : FOG_MODE_EXP2);
        {
            Vector vRed(1.0f, 0.0f, 0.0f);
            GLRender_SetFogModev(FOG_COLOR, vRed);
        }
        m_vFogColor = Vector(1.0f, 0.0f, 0.0f);
        GLRender_SetFogModef(FOG_START, AUR_FOG_TEST_START);
        m_fFogNear = 0.0f;
        m_fFogFar = AUR_FOG_TEST_END;
        m_fFogDensity = AUR_FOG_TEST_DENSITY;
        GLRender_SetFogModef(FOG_END, AUR_FOG_TEST_END);
        GLRender_SetFogModef(FOG_DENSITY, AUR_FOG_TEST_DENSITY);
    }
}
// PC 0x004651C0
void RenderPersonalSpace(Scene* pScene)
{
    if (!renderpersonalspace)
    {
        return;
    }
    GLRender_Disable(ENABLE_DEPTH_TEST);
    for (int i = 0; i < pScene->m_lstObjects.m_nCount; i++)
    {
        Gob* pGob = pScene->m_lstObjects.m_pList[i];
        if ((renderpersonalspace || pGob->m_bRenderPersonalSpace) &&
            pGob->m_fPersonalSpaceRadius > 0.0f)
        {
            GLRender_PushCurrentMatrix();
            Transform(pGob->m_vPosition, pGob->m_qOrientation, 1);

            Vector vPosition;
            Vector vColor(1.0f, 0.6f, 1.0f);
            GLRender::DrawSphereBound(AUR_PERSONAL_SPACE_ORIGIN_RADIUS,
                                      vPosition, vColor, 1.0f, false);
            GLRender::DrawSphereBound(pGob->m_fPersonalSpaceRadius,
                                      vPosition, vColor, 1.0f, false);
            float fRadius = pGob->m_fPersonalSpaceRadius2;
            vColor.m_fY = 0.0f;
            GLRender::DrawSphereBound(fRadius,
                                      vPosition, vColor, 1.0f, false);
            GLRender_PopCurrentMatrix();
        }
    }
    GLRender_Enable(ENABLE_DEPTH_TEST);
}

#define DEBUG_RENDERPATHS 0
// PC 0x00465310
void RenderPaths(Scene* pScene)
{
    if (renderpaths)
    {
        GLRender_Disable(ENABLE_DEPTH_TEST);
        GLRender_Disable(ENABLE_LIGHTING);
        GLRender_Disable(ENABLE_TEXTURE_2D);
#ifdef _XBOX

        for (int i = 0; i < pScene->m_lstObjects.m_nCount; i++)
        {
            Gob* pGob = pScene->m_lstObjects.m_pList[i];
            g_pD3DDevice->Begin(D3DPT_LINESTRIP);
            for (int j = 0; j < pGob->m_PathInfo.m_nCoordinates /
                    CAurPathInfo::COORDINATES_PER_POINT; j++)
            {
                g_pD3DDevice->SetVertexDataColor(D3DVSDE_DIFFUSE, D3DCOLOR_XRGB(153, 255, 255));
                g_pD3DDevice->SetVertexData4f(D3DVSDE_POSITION,
                    pGob->m_PathInfo.m_pfCoordinates[j * CAurPathInfo::COORDINATES_PER_POINT],
                    pGob->m_PathInfo.m_pfCoordinates[j * CAurPathInfo::COORDINATES_PER_POINT + 1],
                    pGob->m_PathInfo.m_pfCoordinates[j * CAurPathInfo::COORDINATES_PER_POINT + 2], 1.0f);
            }
            g_pD3DDevice->End();
        }
#else
        if (DEBUG_RENDERPATHS)
        {
            glPointSize(4.0f);
            for (int i = 0; i < pScene->m_lstObjects.m_nCount; i++)
            {
                Gob* pGob = pScene->m_lstObjects.m_pList[i];
                glBegin(GL_POINTS);
                for (int j = 0; j < pGob->m_PathInfo.m_nCoordinates /
                        CAurPathInfo::COORDINATES_PER_POINT; j++)
                {
                    glVertex3f(pGob->m_PathInfo.m_pfCoordinates[j * CAurPathInfo::COORDINATES_PER_POINT],
                        pGob->m_PathInfo.m_pfCoordinates[j * CAurPathInfo::COORDINATES_PER_POINT + 1],
                        pGob->m_PathInfo.m_pfCoordinates[j * CAurPathInfo::COORDINATES_PER_POINT + 2]);
                }
                glEnd();
                if (pGob->m_PathInfo.m_nCoordinates != 0)
                {
                    glColor3f(1.0f, 0.0f, 0.0f);
                    glBegin(GL_POINTS);
                    glVertex3f(pGob->m_PathInfo.m_pfCoordinates[0],
                        pGob->m_PathInfo.m_pfCoordinates[1],
                        pGob->m_PathInfo.m_pfCoordinates[2]);
                    glEnd();
                }
            }
        }
        glColor3f(0.6f, 1.0f, 1.0f);
        for (int i = 0; i < pScene->m_lstObjects.m_nCount; i++)
        {
            Gob* pGob = pScene->m_lstObjects.m_pList[i];
            glBegin(GL_LINE_STRIP);
            for (int j = 0; j < pGob->m_PathInfo.m_nCoordinates /
                    CAurPathInfo::COORDINATES_PER_POINT; j++)
            {
                glVertex3f(pGob->m_PathInfo.m_pfCoordinates[j * CAurPathInfo::COORDINATES_PER_POINT],
                    pGob->m_PathInfo.m_pfCoordinates[j * CAurPathInfo::COORDINATES_PER_POINT + 1],
                    pGob->m_PathInfo.m_pfCoordinates[j * CAurPathInfo::COORDINATES_PER_POINT + 2]);
            }
            glEnd();
        }
#endif
        GLRender_Enable(ENABLE_LIGHTING);
        GLRender_Enable(ENABLE_TEXTURE_2D);
        GLRender_Enable(ENABLE_DEPTH_TEST);
    }
}
// PC 0x00465420
void Scene::UpdateEmitters()
{
    currentscene = this;

    for (int i = 0; i < m_lstRooms.m_nCount; i++)
    {
        nodedata* pData = m_lstRooms.m_pList[i]->m_pData;
        for (int j = 0; j < pData->m_lstEmitters.m_nCount; j++)
        {
            PartEmitter* pEmitter = pData->m_lstEmitters.m_pList[j];
            for (int k = EMITTER_WARMUP_STEPS; k != 0; k--)
            {
                pEmitter->AnimateParticles(0.1f);
            }
        }
    }

    for (int i = 0; i < m_lstEmitters.m_nCount; i++)
    {
        PartEmitter* pEmitter = m_lstEmitters.m_pList[i];
        for (int j = EMITTER_WARMUP_STEPS; j != 0; j--)
        {
            pEmitter->AnimateParticles(0.1f);
        }
    }

    currentscene = 0;
}

#ifdef _XBOX

char g_bNoSceneRendered = 1;
extern int g_nSpawnedGobs;

void Scene::Render()
{
    char sDownSampled[32];
    char sDecoded[32];
    char sRead[32];
    char sGobs[32];
    char sRoom[128];

    g_bNoSceneRendered = 0;
    if (m_nAnimationFrame == 0)
        UpdateEmitters();
    if (!interleaveanimations || !g_bEnableInterleavedAnimations)
        m_nAnimationFrame++;
    g_nDanglyMeshAnimationByMod3Counter =
        (g_nDanglyMeshAnimationByMod3Counter + 1) % 3;

    if (enablescenerender)
    {
        count_render = 0;
        countvertices = 0;
        m_SceneRoot.SetPosition(m_vSceneFocus);
        PartTriMesh::m_bMaterialHasChanged = true;
        AurTextureMakeCurrent();
        AurTextureForceControllerUpdate();
        currentscene = this;
        ManageSceneBSP(this);
        if (showvisibilityinfo)
        {
            if (m_bIgnoreVisibilityGraph)
            {
                AurPostString("ERROR: ignoreVisibilityGraph flag set", 10, 6, 0.5f);
            }
            else if (!m_pCurrentRoom)
            {
                AurPostString("ERROR: currentRoom hasn't been set", 10, 6, 0.5f);
            }
            else if (!m_lstRooms.m_nCount)
            {
                AurPostString("ERROR: No rooms in scene", 10, 6, 0.5f);
            }
            else
            {
                sprintf(sRoom, "Current room:%s", m_pCurrentRoom->m_sName);
                AurPostString(sRoom, 10, 6, 1.0f);
                if (!m_pCurrentRoom->m_lstVisibleRooms.m_nCount)
                {
                    AurPostString("     Visible:< none visible >", 10, 7, 0.5f);
                }
                else
                {
                    for (int i = 0; i < m_pCurrentRoom->m_lstVisibleRooms.m_nCount; i++)
                    {
                        if (i == 19 && m_pCurrentRoom->m_lstVisibleRooms.m_nCount > 20)
                        {
                            sprintf(sRoom, "     Visible:< %d more rooms >",
                                    m_pCurrentRoom->m_lstVisibleRooms.m_nCount - 19);
                        }
                        else if (i > 19)

                        {
                            continue;
                        }
                        else
                        {
                            sprintf(sRoom, "     Visible:%s",
                                    m_pCurrentRoom->m_lstVisibleRooms[i]->m_sName);
                        }
                        AurPostString(sRoom, 10, i + 7, 0.5f);
                    }
                }
            }
        }
        SetDepthFunc(CMP_LEQUAL);
        GLRender_EnableClientState(CLIENT_STATE_NORMAL);
        GLRender_EnableClientState(CLIENT_STATE_TEXTURE_COORD);
        GLRender_EnableClientState(CLIENT_STATE_VERTEX);
        GLRender_Enable(ENABLE_ALPHA_TEST);
        GLRender_SetAlphaFunc(CMP_GREATER, alphatest);
        GLRender_Enable(ENABLE_LIGHTING);
        if (!m_pBSPRoot && !m_lstRooms.m_nCount)
        {
            RenderOnlyGobs();
            currentscene = 0;
        }
        else
        {
            m_pWindManager->updateTimer(DeltaT);
            RenderSinglePass();
            currentscene = 0;
            if (maxtotaltime < currenttextime + modelloadtime + appendtime)
                maxtotaltime = currenttextime + modelloadtime + appendtime;
            if (maxmodelloadtime < modelloadtime)
                maxmodelloadtime = modelloadtime;
            modelloadtime = 0;
            if (maxappendtime < appendtime)
                maxappendtime = appendtime;
            appendtime = 0;
            if (enablehiccups)
            {
                sprintf(sDownSampled, "Number down sampled: %i\n", g_nImagesDownSampled);
                sprintf(sDecoded, "Number decoded: %i\n", g_nImagesDecoded);
                sprintf(sRead, "Number read in: %i\n", g_nModelsRead);
                sprintf(sGobs, "Number Gobs Made: %i\n", g_nSpawnedGobs);
                if (g_nSpawnedGobs)
                    AurPostString(sGobs, 0, 14, 1.0f);
                if (g_nImagesDownSampled)
                    AurPostString(sDownSampled, 0, 12, 1.0f);
                if (g_nImagesDecoded)
                    AurPostString(sDecoded, 0, 10, 1.0f);
                if (g_nModelsRead)
                    AurPostString(sRead, 0, 8, 1.0f);
                g_nImagesDownSampled = 0;
                g_nImagesDecoded = 0;
                g_nModelsRead = 0;
                g_nSpawnedGobs = 0;
            }
            IsAnimating();
        }
    }
    if (interleaveanimations && g_bEnableInterleavedAnimations)
        m_nAnimationFrame++;
}
#else
void Scene::Render()  // PC 0x00465500
{
#ifndef _XBOX
    AurATIValidate();
#endif
    g_pLastScene = this;
    g_SceneRender_CurrentTime_ms = timeGetTime();
    if (m_nAnimationFrame == 0)
        UpdateEmitters();
    if (!interleaveanimations || !g_bEnableInterleavedAnimations)
        m_nAnimationFrame++;
    g_nDanglyMeshAnimationByMod3Counter =
        (g_nDanglyMeshAnimationByMod3Counter + 1) % 3;

    if (enablescenerender)
    {
        count_render = 0;
        countvertices = 0;
        m_SceneRoot.SetPosition(m_vSceneFocus);
        PartTriMesh::m_bMaterialHasChanged = true;
        AurTextureMakeCurrent();
        AurTextureForceControllerUpdate();
        currentscene = this;
        ManageSceneBSP(this);
        GLRender_PushAttrib(GL_FOG_BIT | GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
        SetDepthFunc(CMP_LEQUAL);
        GLRender_EnableClientState(CLIENT_STATE_NORMAL);
        GLRender_EnableClientState(CLIENT_STATE_TEXTURE_COORD);
        GLRender_EnableClientState(CLIENT_STATE_VERTEX);
        GLRender_Enable(ENABLE_ALPHA_TEST);
        GLRender_SetAlphaFunc(CMP_GREATER, alphatest);
        GLRender_Enable(ENABLE_LIGHTING);
        if (!m_pBSPRoot && !m_lstRooms.m_nCount)
        {
            RenderOnlyGobs();
            currentscene = 0;
            GLRender_PopAttrib();
        }
        else
        {
            m_pWindManager->updateTimer(DeltaT);
            RenderSinglePass();
            GLRender_PopAttrib();
            currentscene = 0;
            if (maxtotaltime < currenttextime + modelloadtime + appendtime)
                maxtotaltime = currenttextime + modelloadtime + appendtime;
            if (maxmodelloadtime < modelloadtime)
                maxmodelloadtime = modelloadtime;
            modelloadtime = 0;
            if (maxappendtime < appendtime)
                maxappendtime = appendtime;
            appendtime = 0;
            IsAnimating();
        }
    }
    if (interleaveanimations && g_bEnableInterleavedAnimations)
        m_nAnimationFrame++;
}
#endif
// PC 0x004656F0
void Scene::RenderSinglePass()

{
    GLRender::SetDirtyTexture(0, m_pDirtyTexture);
    GLRender::SetDirtyTexture(1, m_pDirtyTexture2);
    GLRender::SetDirtyTexture(2, m_pDirtyTexture3);
    GLRender::m_pHologramFilterTexture = m_pHoloTexture;
    if (AurNV2xLevelShadersAvailable())
    {
        Matrix mCamera = CurrentCamera->m_qOrientation.getmatrix();
        Vector vCamera = CurrentCamera->m_vPosition;
        AurSetVertexProgramParameter4f(LightManager::LIGHT_PROGRAM_CAMERA_X,
            mCamera.m_vFirst.m_fX, mCamera.m_vSecond.m_fX,
            mCamera.m_vThird.m_fX, vCamera.m_fX);
        AurSetVertexProgramParameter4f(LightManager::LIGHT_PROGRAM_CAMERA_Y,
            mCamera.m_vFirst.m_fY, mCamera.m_vSecond.m_fY,
            mCamera.m_vThird.m_fY, vCamera.m_fY);
        AurSetVertexProgramParameter4f(LightManager::LIGHT_PROGRAM_CAMERA_Z,
            mCamera.m_vFirst.m_fZ, mCamera.m_vSecond.m_fZ,
            mCamera.m_vThird.m_fZ, vCamera.m_fZ);
        GLRender_SetVertexProgramParameter4f(LightManager::LIGHT_PROGRAM_ZERO_ONE, 0.0f, 0.0f, 0.0f, 1.0f);
    }

    DoFog();
    GLRender_Enable(ENABLE_BLEND);
    GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
    GLRender_Disable(ENABLE_STENCIL_TEST);
    GLRender_DisableClientState(CLIENT_STATE_COLOR);
    GLRender_DisableColorMaterial();
    GLRender_SetLightModel(Homogenize(Vector(), 1.0f));
    GLRender_SetDepthMask(GL_TRUE);
    if (enablerenderstatic)
        RenderStaticGeometry();
    GLRender_SetLightModel(Homogenize(m_vAmbient, 1.0f));
    GLRender_DisableClientState(CLIENT_STATE_COLOR);
    GLRender_DisableColorMaterial();
    if (enablerendertriggers)
    {
#ifdef _XBOX
        ClearStencilBuffer();
#else
        int nStencilBits = AurGetAvailableStencilBits();
        if (nStencilBits)
            GLRender_ClearStencilValue((1 << (nStencilBits - 1)) - 1);
        GLRender_SetStencilMask(GLRender::STENCIL_WRITE_MASK);
        GLRender_Clear(4);
#endif
        DoTriggerBucket();
        DoHighlightTriggers(false);
    }

#ifdef _XBOX
    ClearStencilBuffer();
#else
    int nStencilBits = AurGetAvailableStencilBits();
    if (nStencilBits)
        GLRender_ClearStencilValue((1 << (nStencilBits - 1)) - 1);
    GLRender_SetStencilMask(GLRender::STENCIL_WRITE_MASK);
    GLRender_Clear(4);
#endif
#ifdef _XBOX
    if (enableshadows)
        RenderShadows(0, 1, 1, 0);
    Sleep(1);
#else
    if (m_pCurrentRoom && enableshadows)
        RenderShadows(0, 1, 1, 0);
#endif
    if (enablerenderdynamic)
    {
        RenderDynamicGeometry();
        DoHologramGobBuckets();
    }
    RenderPersonalSpace(this);
    RenderPaths(this);
    if (enablerendertriggers)
    {
#ifdef _XBOX
        ClearStencilBuffer();
#else
        int nStencilBits = AurGetAvailableStencilBits();
        if (nStencilBits)
            GLRender_ClearStencilValue((1 << (nStencilBits - 1)) - 1);
        GLRender_SetStencilMask(GLRender::STENCIL_WRITE_MASK);
        GLRender_Clear(4);
#endif
        DoHighlightTriggers(true);
    }
    DoEmitterBucket();
    DoLightSaberBucket();
    GLRender::RenderDrawableSquares();
    RenderMotionBlur();
    DoDistortionBuckets();
    GLRender::SetFilmNoiseTexture(m_pFilmNoiseTexture);
#ifdef _XBOX

    GLRender::SetForceDistortionTexture(m_pDistortionTexture);
#else
    GLRender::cm_pForceDistortionTexture = m_pDistortionTexture;
#endif
    GLRender::m_pDreamDistortTexture = m_pClairTexture;
    GLRender_SetAlphaFunc(CMP_GREATER, 0.0f);
    unsigned long nFrameBufferFlags = GLRender::FRAMEBUFFER_OVERBRIGHTEN |
        GLRender::FRAMEBUFFER_CONVOLUTION | GLRender::FRAMEBUFFER_NOISE |
        GLRender::FRAMEBUFFER_SATURATION | GLRender::FRAMEBUFFER_SPEED_BLUR;
    if (m_pCurrentRoom)
    {
        unsigned long nFlags = nFrameBufferFlags;
        if (g_listForcePushBucket.m_nCount > 0)
            nFlags |= GLRender::FRAMEBUFFER_FORCE_DISTORTION;
        GLRender::FrameBufferModifications(nFlags);
    }
    RenderLensFlares();
    if (m_pCurrentRoom)
    {
        GLRender_Disable(ENABLE_DEPTH_TEST);
        GLRender_Disable(ENABLE_LIGHTING);
        GLRender::FrameBufferModifications2(nFrameBufferFlags);
    }
    GLRender_Disable(ENABLE_STENCIL_TEST);
    GLRender_Disable(ENABLE_LIGHTING);
    GLRender_Enable(ENABLE_ALPHA_TEST);
    GLRender_Enable(ENABLE_DEPTH_TEST);
    GLRender_Enable(ENABLE_BLEND);
    GLRender_Disable(ENABLE_FOG);
    GLRender_DisableClientState(CLIENT_STATE_NORMAL);
    GLRender_DisableClientState(CLIENT_STATE_TEXTURE_COORD);
    GLRender_DisableClientState(CLIENT_STATE_VERTEX);
    GLRender_DisableClientState(CLIENT_STATE_COLOR);
    GLRender_DisableColorMaterial();
}

#ifdef _XBOX

static unsigned long cShadowVolumeVPDeclaration[] =
{
    D3DVSD_STREAM(0),
    D3DVSD_REG(0, D3DVSDT_FLOAT3),
    D3DVSD_REG(3, D3DVSDT_NORMPACKED3),
    D3DVSD_END()
};

static char cShadowVolumeVP[] =
    "xvs.1.1;\n"
    "sge  r9.w, v0.z, c[12].w;\n"
    "mov r11, c[16];\n"
    "dp4   r10.x, c[4], r11;\n"
    "dp4   r10.y, c[5], r11;\n"
    "dp4   r10.z, c[6], r11;\n"
    "dp3 r6.w, v3, -r10;\n"
    "sge r9.y, r6.w, c[12].y;\n"
    "add r9.w, r9.y, r9.w;\n"
    "sge r9.w, r9.w, c[12].x;\n"
    "add r9.z, c[12].x, -r9.w;\n"
    "mov r7, c[14];\n"
    "dp4   r3.x, c[0], v0;\n"
    "dp4   r3.y, c[1], v0;\n"
    "dp4   r3.z, c[2], v0;\n"
    "dp4   r3.w, c[3], v0;\n"
    "mul r7, r7, r9.w;\n"
    "mul r3, r3, r9.z;\n"
    "add r4, r3, r3;\n"
    "sub r7, r4, r7;\n"
    "add oPos, r3, r7;\n";

void Scene::RenderShadows(int nReserved1, int nReserved2,
                          int bRenderGobs, int bClearStencil)
{
    if (systemmemoryshadows)
    {
        static VertexProgram* pShadowVolumeProgram =
            new VertexProgram(cShadowVolumeVP, cShadowVolumeVPDeclaration);
        pShadowVolumeProgram->Enable(NULL, false);
        GLRender_Enable(ENABLE_VERTEX_PROGRAM);
        AurSetVertexProgramParameter4f(12, 1.0f, 0.0f, 0.0f, 1000000.0f);
        if (systemmemoryshadows)
            g_pD3DDevice->SetRenderState(D3DRS_DEPTHCLIPCONTROL, D3DDCC_CLAMP);
    }
    GLRender_Disable(ENABLE_TEXTURE_2D);
    GLRender_Disable(ENABLE_ALPHA_TEST);
    GLRender_Disable(ENABLE_LIGHTING);
    GLRender_SetStencilMask(GLRender::STENCIL_WRITE_MASK);
    GLRender_Enable(ENABLE_STENCIL_TEST);
    if (bClearStencil)
        GLRender_Clear(4);
    if (bRenderGobs)
    {
        PartLight* pLight = m_SceneRoot.m_pLightContainer->m_pShadowLight;
        if (pLight && m_vSceneFocus.m_fZ + AUR_SHADOW_LIGHT_HEIGHT <
                      pLight->m_vWorldPosition.m_fZ)
        {
            if (systemmemoryshadows)
            {
                GLRender::SetShadowLightPosition(pLight->m_vWorldPosition);
                GLRender::SetShadowLightConstants();
            }
            if (newshadows)
            {
                g_pD3DDevice->SetRenderState(D3DRS_MULTISAMPLEMODE, D3DMULTISAMPLEMODE_4X);
                g_pD3DDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
            }
            GLRender_SetStencilMask(GLRender::STENCIL_WRITE_MASK);
            GLRender_Enable(ENABLE_STENCIL_TEST);
            GLRender_SetDepthMask(GL_FALSE);
            GLRender_SetColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
            GLRender_SetStencilFunc(CMP_ALWAYS, GLRender::SHADOW_STENCIL_MASK,
                          GLRender::SHADOW_STENCIL_MASK);
            GLRender_DisableClientState(CLIENT_STATE_NORMAL);
            GLRender_DisableClientState(CLIENT_STATE_TEXTURE_COORD);
            GLRender_DisableClientState(CLIENT_STATE_COLOR);
            if (systemmemoryshadows)
                GLRender::EnablePixelShaders(31, NULL, NULL, NULL, NULL);
            DoGobShadowsForThisLight(*pLight);
            if (systemmemoryshadows)
                GLRender::EnablePixelShaders(32, NULL, NULL, NULL, NULL);
            GLRender_EnableClientState(CLIENT_STATE_NORMAL);
            GLRender_EnableClientState(CLIENT_STATE_TEXTURE_COORD);
            GLRender_EnableClientState(CLIENT_STATE_COLOR);
            GLRender_SetColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            GLRender_SetStencilMask(0);
            RenderStencilShadowsForThisLight(*pLight,
                m_SceneRoot.m_pLightContainer->m_fShadowFade, 8);
            GLRender_SetStencilMask(GLRender::STENCIL_WRITE_MASK);
        }
        if (systemmemoryshadows)
        {
            g_pD3DDevice->SetRenderState(D3DRS_DEPTHCLIPCONTROL, D3DDCC_CULLPRIMITIVE);
            GLRender_Disable(ENABLE_VERTEX_PROGRAM);
        }
        if (newshadows)
            g_pD3DDevice->SetRenderState(D3DRS_MULTISAMPLEMODE, D3DMULTISAMPLEMODE_1X);
    }
    GLRender_EnableClientState(CLIENT_STATE_NORMAL);
    GLRender_EnableClientState(CLIENT_STATE_TEXTURE_COORD);
    if (LightManager::m_pOnlyGob)
        GLRender_EnableClientState(CLIENT_STATE_COLOR);
    GLRender_SetDepthMask(GL_TRUE);
    GLRender_Enable(ENABLE_TEXTURE_2D);
    GLRender_Enable(ENABLE_ALPHA_TEST);
    GLRender_Enable(ENABLE_LIGHTING);
    GLRender_Disable(ENABLE_STENCIL_TEST);
}
#else
// PC 0x00465AF0
void Scene::RenderShadows(int nReserved1, int nReserved2,
                          int bRenderGobs, int bClearStencil)
{
    int nStencilBits = AurGetAvailableStencilBits();
    if (nStencilBits)
    {
        GLRender_Disable(ENABLE_TEXTURE_2D);
        GLRender_Disable(ENABLE_ALPHA_TEST);
        GLRender_Disable(ENABLE_LIGHTING);
        GLRender_SetStencilMask(GLRender::STENCIL_WRITE_MASK);
        GLRender_Enable(ENABLE_STENCIL_TEST);
        if (bClearStencil)
            GLRender_Clear(4);
        if (bRenderGobs)
        {
            PartLight* pLight = m_SceneRoot.m_pLightContainer->m_pShadowLight;
            if (pLight && m_vSceneFocus.m_fZ + AUR_SHADOW_LIGHT_HEIGHT <
                          pLight->m_vWorldPosition.m_fZ)
            {
                if (enablesoftshadows && AurNV2xLevelShadersAvailable() &&
                    (AurSoftShadowsAvailable() || AurATISoftShadowsAvailable()) &&
                    GLRender::cm_bSetUpImageSpaceShadows)
                    GLRender::SetUpSoftShadows();
                GLRender_SetStencilMask(GLRender::STENCIL_WRITE_MASK);
                GLRender_Enable(ENABLE_STENCIL_TEST);
                GLRender_SetDepthMask(GL_FALSE);
                GLRender_SetColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
                GLRender_SetStencilFunc(CMP_ALWAYS, GLRender::SHADOW_STENCIL_MASK,
                              GLRender::SHADOW_STENCIL_MASK);
                GLRender_DisableClientState(CLIENT_STATE_NORMAL);
                GLRender_DisableClientState(CLIENT_STATE_TEXTURE_COORD);
                GLRender_DisableClientState(CLIENT_STATE_COLOR);
                DoGobShadowsForThisLight(*pLight);
                GLRender_EnableClientState(CLIENT_STATE_NORMAL);
                GLRender_EnableClientState(CLIENT_STATE_TEXTURE_COORD);
                GLRender_EnableClientState(CLIENT_STATE_COLOR);
                GLRender_SetColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
                GLRender_SetStencilMask(0);
                RenderStencilShadowsForThisLight(*pLight,
                    m_SceneRoot.m_pLightContainer->m_fShadowFade, nStencilBits);
                if (enablesoftshadows && AurNV2xLevelShadersAvailable() &&
                    (AurSoftShadowsAvailable() || AurATISoftShadowsAvailable()) &&
                    GLRender::cm_bSetUpImageSpaceShadows)
                    GLRender::FinishSoftShadows();
                GLRender_SetStencilMask(GLRender::STENCIL_WRITE_MASK);
            }
        }
        GLRender_EnableClientState(CLIENT_STATE_NORMAL);
        GLRender_EnableClientState(CLIENT_STATE_TEXTURE_COORD);
        if (LightManager::m_pOnlyGob)
            GLRender_EnableClientState(CLIENT_STATE_COLOR);
        GLRender_SetDepthMask(GL_TRUE);
        GLRender_Enable(ENABLE_TEXTURE_2D);
        GLRender_Enable(ENABLE_ALPHA_TEST);
        GLRender_Enable(ENABLE_LIGHTING);
        GLRender_Disable(ENABLE_STENCIL_TEST);
    }
}
#endif
// PC 0x00465D20
void Scene::Animate(float fTimeStep)
{
    currentscene = this;

    if (interleaveanimations && g_bEnableInterleavedAnimations)
    {
        int i = m_lstAnimatedObjects.m_nCount;
        while (i--)
        {
            if (i < m_lstAnimatedObjects.m_nCount)
            {
                m_lstAnimatedObjects.m_pList[i]->Animate(fTimeStep);
            }
        }

        i = m_lstObjects.m_nCount;
        while (i--)
        {
            if (i < m_lstObjects.m_nCount)
            {
                Gob* pGob = m_lstObjects.m_pList[i];
                if (!pGob->m_bHierarchyAnimated)
                {
                    pGob->Animate(fTimeStep);
                }
            }
        }
    }
    else
    {
        int i = m_lstObjects.m_nCount;
        while (i--)
        {
            if (i < m_lstObjects.m_nCount)
            {
                m_lstObjects.m_pList[i]->Animate(fTimeStep);
            }
        }
    }

    currentscene = 0;
}

#ifndef _XBOX
void GLRender::DrawFromVerts(int nMode, List<Vector>& lstVertices)  // PC 0x00466DA0
{
    glBegin(primTypeTranslationTable[nMode]);
    for (int i = 0; i < lstVertices.m_nCount; i++)
    {
        glVertex3fv((GLfloat*)&lstVertices.m_pList[i]);
    }
    glEnd();
}
#endif
// PC 0x00465DE0
void ComputePartBBox(PartTriMesh* pTriMesh)
{
    MdlNodeTriMesh* pNode = pTriMesh->m_pVertexPrimitive->m_pNode;
    Vector vMax(-100000.0f, -100000.0f, -100000.0f);
    Vector vMin(100000.0f, 100000.0f, 100000.0f);
    int nVertexPool = pNode->m_pModel->AsModel()->m_nVertexPool;
    void* pVertexBuffer = GLRender::LockPool(nVertexPool, 0, 0, RequestPoolManager::POOL_LOCK_READ_ONLY);

    unsigned long nVertexDataOffset = pNode->m_nVertexDataOffset;
    for (int i = 0; i < pNode->m_nVertexCount; i++)
    {
        float* pVertex = (float*)((char*)pVertexBuffer + nVertexDataOffset
                                 + i * pNode->m_nVertexStride + pNode->m_nPositionOffset);
        Vector vLocal(pVertex[0], pVertex[1], pVertex[2]);
        Vector vTransformed = pTriMesh->m_vPosition + pTriMesh->m_qOrientation * vLocal;
#ifdef _XBOX
        Vector vRounded = Round(vTransformed, 0.001f);
#else
        Vector vRounded(Round(vTransformed.m_fX, 0.001f),
                        Round(vTransformed.m_fY, 0.001f),
                        Round(vTransformed.m_fZ, 0.001f));
#endif
        vMin = VectorMin(vMin, vRounded);
        vMax = VectorMax(vMax, vRounded);
    }

    nVertexPool = pNode->m_pModel->AsModel()->m_nVertexPool;
    GLRender::UnlockPool(nVertexPool);
    pTriMesh->m_vMinimum = vMin;
    pTriMesh->m_vMaximum = vMax;
}
// PC 0x00469B70
void Scene::SetSingleTileFlame(CAurObject* pObject, Gob** ppGob, Part* pPart)
{
    AURSCENE_TRACE();
    List<PartLight*> lstLights;
    Gob* pGob = (Gob*)pObject;
    if (*ppGob == pGob)
    {
        return;
    }

    if (*ppGob != 0)
    {
        (*ppGob)->CollectLights(lstLights);
        for (int i = 0; i < lstLights.m_nCount; i++)
        {
            UpdateStaticLighting(this, lstLights.m_pList[i]);
        }
        if (*ppGob)
        {
            delete *ppGob;
            *ppGob = 0;
        }
    }

    if (pGob != 0 && pPart != 0)
    {
        pGob->m_vPosition = pPart->m_vPosition;
        pGob->m_qOrientation = pPart->m_qOrientation;
        pGob->SetScene(this);
        pGob->CollectLights(lstLights);
        for (int i = 0; i < lstLights.m_nCount; i++)
        {
            UpdateStaticLighting(this, lstLights.m_pList[i]);
        }
        *ppGob = pGob;
    }
}
// PC 0x00469CB0
void Scene::RemoveFromSceneManager(Part* pPart)
{
    PartEmitter* pEmitter = pPart->AsPartEmitter();
    PartTrigger* pTrigger = pPart->AsPartTrigger();

    if (pEmitter != 0 && emitterbucket.Contains(pEmitter))
    {
        emitterbucket.Remove(pEmitter);
    }

    if (pTrigger != 0 && triggerbucket.Contains(pTrigger))
    {
        triggerbucket.Remove(pTrigger);
    }
}

Gob* FindGob(char* sName);
int AurATIFragmentShaderAvailable();

void Scene::RemoveFromSceneManager(Gob* pGob)
{
    if (gobbucket.Contains(pGob))
    {
        gobbucket.Remove(pGob);
        return;
    }
    if (g_listHologramBucket.Contains(pGob))
    {
        g_listHologramBucket.Remove(pGob);
    }
}
// PC 0x0046B460
int Scene::HitCheck(Vector vStart, Vector vEnd, Vector* pHit, CHitInfo* pHitInfo)
{
    if (!pHitInfo)
    {
        pHitInfo = &DefaultHitInfo;
        pHitInfo->ClearOutput();
    }

    CurrentHitInfo = pHitInfo;
    CAurObject* pHitObject = 0;

    if (!pHitInfo->m_bResult3)
    {
        if (!m_lstRooms.m_nCount)
        {
            if (BSPHitCheck(m_pBSPRoot, vStart, vEnd, &vEnd, HitPartOnBSPNode))
            {
                pHitInfo->m_nOutputFlags = CHitInfo::HIT_STATIC;
                pHitInfo->m_vOutputPosition = vEnd;
                if (pHit)
                    *pHit = vEnd;
            }
        }

        for (int i = 0; i < m_lstRooms.m_nCount; i++)
        {
            CAurRoom* pRoom = m_lstRooms[i];
            if (BoxIntersect(vStart, vEnd, pRoom->m_vMinimum, pRoom->m_vMaximum))
            {
                if (HitPartOnBSPNode(pRoom, vStart, vEnd, &vEnd))
                {
                    pHitInfo->m_nOutputFlags = CHitInfo::HIT_STATIC;
                    pHitInfo->m_vOutputPosition = vEnd;
                    if (pHit)
                        *pHit = vEnd;
                    pHitInfo->m_pOutputRoom = pRoom;
                }
            }
        }
    }

    CurrentHitInfo = 0;
    if (!pHitInfo->m_nType)
    {
        pHitObject = HitCheckGob(vStart, vEnd, &vEnd, pHitInfo);
        if (pHitObject)
        {
            pHitInfo->m_nOutputFlags = CHitInfo::HIT_GOB;
            pHitInfo->m_vOutputPosition = vEnd;
            if (pHit)
                *pHit = vEnd;
        }
    }

    pHitInfo->m_pOutputObject = pHitObject;
    return pHitInfo->m_nOutputFlags;
}
// PC 0x0046B710
void InitGrassBins(Vector vMin, Vector vMax,
                   List<List<CAurTriangleBin*> >& lstBins)
{
    AurSceneTrace();
    lstBins.m_nCount = 0;
    int nBinsX;
    int nBinsY;
    roundToInt(nBinsX, (float)ceil((vMax.m_fX - vMin.m_fX) / Scene::binsize));
    roundToInt(nBinsY, (float)ceil((vMax.m_fY - vMin.m_fY) / Scene::binsize));
    if (nBinsX >= 0 && nBinsY >= 0)
    {
        lstBins.SetSize(nBinsX);
        for (int x = 0; x < nBinsX; x++)
        {
            lstBins[x].m_nCount = 0;
            lstBins[x].SetSize(nBinsY);
            for (int y = 0; y < nBinsY; y++)
                lstBins[x][y] = 0;
        }
    }
}
// PC 0x0046B840
Gob* Scene::SpawnRoom(char* sName, const Vector& vPosition,
                      const Quaternion& qOrientation)
{
    AurTextureSetModeStatic();
    Model* pModel = FindModel(sName);
    if (!pModel)
        return 0;

    pModel->m_Classification.m_bAllRooms = 1;
    Gob* pGob = 0;
    CAurRoom* pRoom = new CAurRoom(sName);
    nodedata* pData = new nodedata;
    bool bHasAABB = false;
    Vector vMin(AUR_ROOM_AABB_UNSET_BOUND, AUR_ROOM_AABB_UNSET_BOUND,
                AUR_ROOM_AABB_UNSET_BOUND);
    Vector vMax(-AUR_ROOM_AABB_UNSET_BOUND, -AUR_ROOM_AABB_UNSET_BOUND,
                -AUR_ROOM_AABB_UNSET_BOUND);
    char sPartName[64];
    char sMaterialName[32];
    Part* pRootPart;

    strcpy(sPartName, pModel->m_sName);
    strcat(sPartName, "a");
    if (findnode(pModel->m_pRootNode, sPartName))
    {
        pGob = (Gob*)NewCAurObject(sName, "lbl_cardstand", 0, 0);
        pRootPart = pGob->m_pRootPart;
        pGob->m_vPosition = vPosition;
        pGob->m_qOrientation = qOrientation;
        pGob->SetScene(this);
        pRoom->m_pRoomObject = pGob;
    }
    else
    {
        pRootPart = CreateInstanceTree(pModel->m_pRootNode, 0);
        sprintf(sMaterialName, "%s", sName);
        PartInitMaterial(0, pRootPart, sMaterialName);
    }

    List<Part*> lstParts;
    List<PartAABB*> lstAABBs;
    CollapsePartTree(pGob, pRootPart, lstParts, vPosition, qOrientation, this, pRoom);
    pRoom->m_vMaximum = Vector(-AUR_ROOM_UNSET_BOUND, -AUR_ROOM_UNSET_BOUND,
                               -AUR_ROOM_UNSET_BOUND);
    pRoom->m_vMinimum = Vector(AUR_ROOM_UNSET_BOUND, AUR_ROOM_UNSET_BOUND,
                               AUR_ROOM_UNSET_BOUND);
    for (int i = 0; i < lstParts.m_nCount; i++)
    {
        PartTriMesh* pTriMesh = lstParts[i]->AsPartTriMesh();
        PartLight* pLight = lstParts[i]->AsPartLight();
        PartEmitter* pEmitter = lstParts[i]->AsPartEmitter();
        PartAABB* pAABB = lstParts[i]->AsPartAABB();
        if (pTriMesh)
        {
            if (!pTriMesh->AsPartAnimMesh())
            {
                ComputePartBBox(pTriMesh);
                if (!pTriMesh->m_pNode->AsMdlNodeTriMesh()->m_bBackground)
                {
                    pRoom->m_vMaximum = VectorMax(pRoom->m_vMaximum, pTriMesh->m_vMaximum);
                    pRoom->m_vMinimum = VectorMin(pRoom->m_vMinimum, pTriMesh->m_vMinimum);
                }
                else
                {
                    pRoom->m_bB8 = 1;
                }
            }
            pModel->AddRef((Gob*)0);
            pData->m_lstTriMeshes.Add(pTriMesh);
            if (pAABB)
            {
                pData->m_pAABB = pAABB;
                lstAABBs.Add(pAABB);
                vMax = VectorMax(vMax, pAABB->m_vMaximum);
                vMin = VectorMin(vMin, pAABB->m_vMinimum);
                bHasAABB = true;
            }
        }
        else if (pLight)
        {
            m_pLightManager->AddLight(pLight);
            pData->m_lstLights.Add(pLight);
            if (pLight->m_nLightType)
                SceneBSPAddPart(this, pLight, 0);
            pModel->AddRef((Gob*)0);
        }
        else if (pEmitter)
        {
            pData->m_lstEmitters.Add(pEmitter);
            pModel->AddRef((Gob*)0);
        }
        else
            delete lstParts[i];
    }
    if (lstAABBs.m_nCount > 0)
    {
        List<List<CAurTriangleBin*> > lstBins;
        InitGrassBins(vMin, vMax, lstBins);
        for (int i = 0; i < lstAABBs.m_nCount; i++)
            AddFacesToBins(lstAABBs[i], vMin, lstBins);
        for (int i = 0; i < lstBins.m_nCount; i++)
        {
            for (int j = 0; j < lstBins[i].m_nCount; j++)
            {
                if (lstBins[i][j])
                {
                    lstBins[i][j]->FinishGeometry();
                    pData->m_lstBins.Add(lstBins[i][j]);
                }
            }
        }
    }
    pRoom->m_pData = pData;
    m_lstRooms.Add(pRoom);
    AurTextureSetModeDynamic();
    if (!bHasAABB)
    {
        pRoom->m_vMaximum = Vector(-AUR_ROOM_UNSET_BOUND, -AUR_ROOM_UNSET_BOUND,
                                   -AUR_ROOM_UNSET_BOUND);
        pRoom->m_vMinimum = Vector(AUR_ROOM_UNSET_BOUND, AUR_ROOM_UNSET_BOUND,
                                   AUR_ROOM_UNSET_BOUND);
    }
    return pGob;
}
// PC 0x0046D820
Scene::Scene(char* sName)
    : m_SceneRoot("c_FocusGob")
{
    char sFocusModelName[32];
    char Buffer[64];

    m_sName[0] = 0;
    if (sName)
    {
        sprintf(Buffer, "%s", sName);
        int i = 0;
        while (FindScene(Buffer) || FindGob(Buffer))
            sprintf(Buffer, "%s%d", sName, ++i);
        strcpy(m_sName, Buffer);
    }

    sprintf(sFocusModelName, "c_FocusGobDummyModel%d", g_FocusModelCount);
    g_FocusModelCount++;
    m_SceneRoot.m_pModel = new Model(sFocusModelName);
    m_SceneRoot.m_pModel->AddRef(&m_SceneRoot);

    m_vAmbient = Vector(0.0f, 0.0f, 0.0f);
    m_bFog = 0;
    m_nFogMode = FOG_MODE_LINEAR;
    m_vFogColor = Vector(1.0f, 1.0f, 1.0f);
    m_v80 = Vector(g_DesatureFactor * 0.3f, g_DesatureFactor * 0.59f,
                   g_DesatureFactor * 0.11f);
    m_fFogNear = 5.0f;
    m_fFogFar = 40.0f;
    m_fFogDensity = 0.1f;
    m_nAnimationFrame = 0;

    m_pLightManager = new LightManager;
    m_pWindManager = new WindManager;
    m_pNormalizationCubeMap = AurTextureGetNormalizationCubeMap();

    m_bIgnoreVisibilityGraph = 1;
    m_fHighlightTime = 0.0f;
#ifdef _XBOX

    AUR_OBJECT_CONSOLE_VAR(m_sName, "ambient", &m_vAmbient)
    AUR_OBJECT_CONSOLE_VAR(m_sName, "fog", &m_bFog)
    AUR_OBJECT_CONSOLE_VAR(m_sName, "fogcolor", &m_vFogColor)
    AUR_OBJECT_CONSOLE_VAR(m_sName, "fogstart", &m_fFogNear)
    AUR_OBJECT_CONSOLE_VAR(m_sName, "fogend", &m_fFogFar)
    AUR_OBJECT_CONSOLE_VAR(m_sName, "fogmode", &m_nFogMode)
    AUR_OBJECT_CONSOLE_VAR(m_sName, "fogdensity", &m_fFogDensity)
#endif
    Scenes.Add(this);

    m_pBSPRoot = 0;

    m_bAnimating = 1;
    m_bBinsPopulated = 0;
    m_nSceneFlags = 0;
    m_fSceneScale = 1.0f;
    m_lstPendingObjects.m_nCount = 0;
    m_SceneRoot.SetScene(this);
    m_pCurrentRoom = 0;

    m_pDirtyTexture = AurTextureGetReference("dirtytex", 0);
    m_pDirtyTexture2 = AurTextureGetReference("dirtytex2", 0);
    m_pDirtyTexture3 = AurTextureGetReference("dirtytex3", 0);
    m_pFilmNoiseTexture = AurTextureGetReference("filmnoisetex", 0);
    m_pDistortionTexture = AurTextureGetReference("distortiontex", 0);
    m_pClairTexture = AurTextureGetReference("fx_Clair01", 0);
    m_pHoloTexture = AurTextureGetReference("Holotex", 0);

    if (AurATIFragmentShaderAvailable())
        VertexProgramManager::ResetVertexPrograms();
}
// PC 0x0046DD20
Scene::~Scene()
{
    while (m_lstObjects.m_nCount > 0)
        m_lstObjects[m_lstObjects.m_nCount - 1]->SetScene(0);
    m_SceneRoot.SetScene(0);
#ifdef _XBOX
    strcpy(g_sObjectFuncName, m_sName);
    strcat(g_sObjectFuncName, ".");
    DeleteFuncWithPrefix(g_sObjectFuncName);
#endif
    Scenes.Remove(this);

    delete m_pLightManager;
    delete m_pBSPRoot;
    AurClearActiveBins();
    while (m_lstRooms.m_nCount > 0)
    {
        BSPTraverse(m_lstRooms[m_lstRooms.m_nCount - 1], DeleteNonGobParts, 0);
        delete m_lstRooms[m_lstRooms.m_nCount - 1];
        m_lstRooms.m_nCount--;
    }
    delete m_pWindManager;

    if (m_pNormalizationCubeMap)
    {
        CAurTexture* pTexture = m_pNormalizationCubeMap;
        m_pNormalizationCubeMap = 0;
        AurTextureRelease(pTexture);
    }
    if (m_pFilmNoiseTexture)
    {
        CAurTexture* pTexture = m_pFilmNoiseTexture;
        m_pFilmNoiseTexture = 0;
        AurTextureRelease(pTexture);
    }
    if (m_pDistortionTexture)
    {
        CAurTexture* pTexture = m_pDistortionTexture;
        m_pDistortionTexture = 0;
        AurTextureRelease(pTexture);
    }
    if (m_pDirtyTexture)
    {
        CAurTexture* pTexture = m_pDirtyTexture;
        m_pDirtyTexture = 0;
        AurTextureRelease(pTexture);
    }
    if (m_pDirtyTexture2)
    {
        CAurTexture* pTexture = m_pDirtyTexture2;
        m_pDirtyTexture2 = 0;
        AurTextureRelease(pTexture);
    }
    if (m_pDirtyTexture3)
    {
        CAurTexture* pTexture = m_pDirtyTexture3;
        m_pDirtyTexture3 = 0;
        AurTextureRelease(pTexture);
    }
    if (m_pClairTexture)
    {
        CAurTexture* pTexture = m_pClairTexture;
        m_pClairTexture = 0;
        AurTextureRelease(pTexture);
    }
    if (m_pHoloTexture)
    {
        CAurTexture* pTexture = m_pHoloTexture;
        m_pHoloTexture = 0;
        AurTextureRelease(pTexture);
    }
    for (int i = 0; i < AUR_MESH_BUCKET_COUNT; i++)
        meshbuckets[i].m_nCount = 0;
}
// PC 0x0046B360
void CHitInfo::ClearOutput()
{
    Vector vZero;

    m_nOutputFlags = 0;
    m_pOutputObject = 0;
    m_vOutputNormal = vZero;
    m_vOutputPosition = vZero;
    m_vOutputVector = vZero;
    m_nOutputIndex = 0;
    m_sOutputPartName = 0;
    m_pOutputHitData = 0;
    m_pOutputRoom = 0;
    m_nOutputType = -1;
}

#include "aurlist.h"

#include <stdlib.h>

#include "aurpointer.h"

Vector Scene::m_vLastCameraPosition;
Quaternion Scene::m_qLastCameraOrientation;
List<Scene*> Scenes;
CHitInfo DefaultHitInfo(0, ~CHitInfo::HIT_TRIGGER, 0);

extern int enablevisibilitytest;
extern float visibilitycutoffratio;
extern float visibilitycheckdelay;
extern int countvisibilityculls;
// PC 0x0046C2B0
inline CAurScene::~CAurScene()
{
}

inline CAurScene::CAurScene()
{
}

#ifndef _XBOX
bool Gob::VisibilityCheck()  // PC 0x0046C2E0
{
    if (!enablevisibilitytest)
        return true;
    if (!IsVisibilityCheckDisabled() && !IsAlwaysRendering())
    {
        Camera* pCamera = CurrentCamera;
        float fDistance = magnitude(pCamera->m_vPosition - m_vPosition);
        if (2.0f * m_fMinimumBoundingSphereRadius / fDistance > visibilitycutoffratio)
        {
            m_fVisibilityCheckTime = 0.0f;
            m_bVisible = true;
        }
        else
        {
            m_fVisibilityCheckTime += DeltaT;
            if (m_fVisibilityCheckTime > visibilitycheckdelay)
            {
                m_fVisibilityCheckTime = 0.0f;
                CHitInfo hitInfo(0, CHitInfo::HIT_ALL_NON_TRIGGER, 0);
                hitInfo.m_nType = CHitInfo::HIT_TYPE_VISUAL_EFFECT_GROUND;
                Vector vHit(0.0f, 0.0f, 0.0f);
                Vector vCenter = m_vPosition + m_qOrientation * m_vMinimumBoundingSphere;
                int nHit = m_pScene->HitCheck(CurrentCamera->m_vPosition, vCenter,
                                              &vHit, &hitInfo);
                m_bVisible = !nHit;
            }

            if (!m_bVisible)
                countvisibilityculls++;
            return m_bVisible;
        }
    }
    return true;
}
#endif
// PC 0x0046C630
void Scene::DoGobBuckets()
{
    countvisibilityculls = 0;
    Vector vPosition;
    qsort(gobbucket.m_pList, gobbucket.m_nCount, sizeof(Gob*), GobOrderCmp);
    numgobs = gobbucket.m_nCount;
    LightManager* pLightManager = m_pLightManager;
    pLightManager->m_lstPreviousFadeLights.m_nCount = 0;
    pLightManager->m_lstPreviousFadeIntensities.m_nCount = 0;

    int nPasses = 1;
    if (g_ForceSight)
        nPasses = 2;
    for (int nPass = 0; nPass < nPasses; nPass++)
    {
        int i = gobbucket.m_nCount;
        while (i--)
        {
            AurRenderCallback();
            Gob* pGob = gobbucket.m_pList[i];
            if (g_ForceSight)
            {
                Model* pModel = pGob->m_pModel;
                if (nPass == 0)
                {
                    if (bool(pModel->m_Classification.m_bCharacter) || bool(pModel->m_Classification.m_bDoor))
                        continue;
                }
                else
                {
                    if (!bool(pModel->m_Classification.m_bCharacter) && !bool(pModel->m_Classification.m_bDoor))
                        continue;
                }
            }
            if (interleaveanimations && g_bEnableInterleavedAnimations)
            {
                if (i >= gobbucket.m_nCount)
                    continue;
                pGob->Animate(DeltaT);
                if (i >= gobbucket.m_nCount)
                    continue;
            }
            if (!pGob->VisibilityCheck())
                continue;

            if (rendertransparentcharacters && pGob->m_pModel &&
                bool(pGob->m_pModel->m_Classification.m_bCharacter))
                GLRender_Disable(ENABLE_DEPTH_TEST);

            vPosition = pGob->m_vPosition;
            if (pGob->IsInCutscene())
            {
                Part* pDummy = FindPart(pGob->m_pRootPart, "cutscenedummy");
                if (!pDummy)
                    m_pLightManager->SetupLightsForThisGob(
                        pGob->m_vPosition, pGob->m_fMinimumBoundingSphereRadius);
                else
                {
                    static Vector worldPos;
                    static Quaternion worldOrient;
                    pDummy->GetWorld(worldPos, worldOrient);
                    m_pLightManager->SetupLightsForThisGob(
                        worldPos, pGob->m_fMinimumBoundingSphereRadius);
                    vPosition = worldPos;
                }
            }
            else
                m_pLightManager->SetupLightsForThisGob(
                    pGob->m_vPosition, pGob->m_fMinimumBoundingSphereRadius);

            m_pLightManager->PrioritizeLights(vPosition, false);
            if (pGob->m_pLightContainer)
                m_pLightManager->EnableHighestPriorityLightsFade(
                    LightManager::MAX_ENABLED_LIGHTS, pGob->m_pLightContainer);
            else
                m_pLightManager->EnableHighestPriorityLights(
                    LightManager::MAX_ENABLED_LIGHTS);
            if (showlightpriorityinfo &&
                strnicmp(pGob->m_sName, "c_focusgob", 10) == 0)
                m_pLightManager->DebugPrintLightInfo(LightManager::MAX_ENABLED_LIGHTS,
                                            pGob->m_vPosition);

            GLRender_Enable(ENABLE_BLEND);
            GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
            if (dogobrender && pGob->IsRenderingEnabled())
                pGob->Render(false);
            if (rendertransparentcharacters &&
                bool(pGob->m_pModel->m_Classification.m_bCharacter))
                GLRender_Enable(ENABLE_DEPTH_TEST);
        }
    }

    if (interleaveanimations && g_bEnableInterleavedAnimations)
    {
        int i = m_lstObjects.m_nCount;
        while (i--)
        {
            if (i < m_lstObjects.m_nCount)
                m_lstObjects.m_pList[i]->Animate(DeltaT);
        }
    }
}
// PC 0x0046CA00
void Scene::DoHologramGobBuckets()
{
    if (g_listHologramBucket.m_nCount == 0)
    {
        return;
    }

    countvisibilityculls = 0;
    Vector vPosition;
    qsort(g_listHologramBucket.m_pList, g_listHologramBucket.m_nCount, sizeof(Gob*),
          GobOrderCmp);
    numgobs = g_listHologramBucket.m_nCount;
    LightManager* pLightManager = m_pLightManager;
    pLightManager->m_lstPreviousFadeLights.m_nCount = 0;
    pLightManager->m_lstPreviousFadeIntensities.m_nCount = 0;

    int i = g_listHologramBucket.m_nCount;
    while (i--)
    {
        AurRenderCallback();
        Gob* pGob = g_listHologramBucket.m_pList[i];
        if (interleaveanimations && g_bEnableInterleavedAnimations)
        {
            if (i >= g_listHologramBucket.m_nCount)
                continue;
            pGob->Animate(DeltaT);
            if (i >= g_listHologramBucket.m_nCount)
                continue;
        }
        if (!pGob->VisibilityCheck())
            continue;

        if (rendertransparentcharacters && pGob->m_pModel &&
            bool(pGob->m_pModel->m_Classification.m_bCharacter))
            GLRender_Disable(ENABLE_DEPTH_TEST);

        vPosition = pGob->m_vPosition;
        if (pGob->IsInCutscene())
        {
            Part* pDummy = FindPart(pGob->m_pRootPart, "cutscenedummy");
            if (!pDummy)
                m_pLightManager->SetupLightsForThisGob(
                    pGob->m_vPosition, pGob->m_fMinimumBoundingSphereRadius);
            else
            {
                static Vector worldPos;
                static Quaternion worldOrient;
                pDummy->GetWorld(worldPos, worldOrient);
                m_pLightManager->SetupLightsForThisGob(
                    worldPos, pGob->m_fMinimumBoundingSphereRadius);
                vPosition = worldPos;
            }
        }
        else
            m_pLightManager->SetupLightsForThisGob(
                pGob->m_vPosition, pGob->m_fMinimumBoundingSphereRadius);

        m_pLightManager->PrioritizeLights(vPosition, false);
        if (pGob->m_pLightContainer)
            m_pLightManager->EnableHighestPriorityLightsFade(
                LightManager::MAX_ENABLED_LIGHTS, pGob->m_pLightContainer);
        else
            m_pLightManager->EnableHighestPriorityLights(
                LightManager::MAX_ENABLED_LIGHTS);
        if (showlightpriorityinfo &&
            strnicmp(pGob->m_sName, "c_focusgob", 10) == 0)
            m_pLightManager->DebugPrintLightInfo(LightManager::MAX_ENABLED_LIGHTS,
                                        pGob->m_vPosition);

        GLRender_Enable(ENABLE_BLEND);
        GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
        if (dogobrender)
            pGob->Render(false);
        if (rendertransparentcharacters &&
            bool(pGob->m_pModel->m_Classification.m_bCharacter))
            GLRender_Enable(ENABLE_DEPTH_TEST);
    }

}

extern unsigned int ATI_FRAGMENT_SHADER_BIT;
// PC 0x0046CCF0
void Scene::RenderLensFlares()
{
#ifndef _XBOX
    if (renderlensflares)
#endif
    {
        List<PartLight*> lights(*m_pLightManager->GetLensFlareLights());
#ifdef _XBOX

        static RenderVertexVTC* pVertices = new RenderVertexVTC[4];
#else
        static unsigned long renderListToken = GLRender::RequestPool(
            Aurora::POOL_VERTEX, 4 * sizeof(RenderVertexVTC),
            RequestPoolManager::POOL_FLARE_USAGE, LENS_FLARE_POOL_FLAGS, &renderListToken);
#endif

        GLRender_Disable(ENABLE_LIGHTING);
        GLRender_Disable(ENABLE_DEPTH_TEST);
        GLRender_Enable(ENABLE_BLEND);
        GLRender_SetBlendFunc(BLEND_ONE, BLEND_ONE);
#ifndef _XBOX
        GLRender_Enable(ENABLE_TEXTURE_2D);
#endif
        if (m_bFog)
            GLRender_Disable(ENABLE_FOG);
        GLRender_SetMatrixMode(TRANSFORM_PROJECTION);
        GLRender_PushCurrentMatrix();
        GLRender_LoadMatrixIdentity();
        GLRender_SetMatrixMode(TRANSFORM_MODELVIEW);
        GLRender_PushCurrentMatrix();
        GLRender_LoadMatrixIdentity();
        GLRender_Translate(0.0f, 0.0f, -2.0f);

        if (g_ForceSight)
        {
            GLRender_EnablePixelShaders(MODEL_SHADER_DESAT, 0, 0, 0, 0);
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

        for (int i = 0; i < lights.m_nCount; i++)
        {
            PartLight* pLight = lights.m_pList[i];
            Vector vToLight = pLight->m_vWorldPosition - CurrentCamera->m_vPosition;
            if (!(magnitude(vToLight) > ((MdlNodeLight*)pLight->m_pNode)->m_fFlareRadius))
            {
                if (hitcheckflare)
                {
                    static Vector impact;
                    CHitInfo* pHitInfo = new CHitInfo(0, CHitInfo::HIT_ALL_NON_TRIGGER, 0);
                    pHitInfo->m_nMask = 0;
                    pHitInfo->m_nResult = 1;
                    int nHit = reverseflarehit ?
                        HitCheck(CurrentCamera->m_vPosition, pLight->m_vWorldPosition,
                                 &impact, pHitInfo) :
                        HitCheck(pLight->m_vWorldPosition, CurrentCamera->m_vPosition,
                                 &impact, pHitInfo);
                    delete pHitInfo;
#ifdef _XBOX
                    if (nHit)
                    {
                        pLight->m_fFlareFade = max(pLight->m_fFlareFade - flarespeed * DeltaT, 0.0f);
                        if (pLight->m_fFlareFade == 0.0f)
                            continue;
                    }
                    else
                        pLight->m_fFlareFade = min(pLight->m_fFlareFade + flarespeed * DeltaT, 1.0f);
#else
                    float fStep = DeltaT * flarespeed;
                    if (nHit)
                    {
                        pLight->m_fFlareFade = max(pLight->m_fFlareFade - fStep, 0.0f);
                        if (pLight->m_fFlareFade == 0.0f)
                            continue;
                    }
                    else
                        pLight->m_fFlareFade = min(pLight->m_fFlareFade + fStep, 1.0f);
#endif

                }

                MdlNodeLight* pNode = (MdlNodeLight*)pLight->m_pNode;
                Vector vDirection = -CurrentCamera->m_qOrientation *
                                    (pLight->m_vWorldPosition - CurrentCamera->m_vPosition);
                float fDistance = magnitude(vDirection);
                vDirection = vDirection / fDistance;
                float fFacing = max(vDirection * Vector(0.0f, 0.0f, -1.0f), 0.0f);
                fFacing *= fFacing;
                vDirection = planelineintersection(Vector(0.0f, 0.0f, -1.0f),
                                                   2.0f, Vector(), vDirection);
                vDirection = -vDirection;

                if (pNode->m_lstTextures.m_nCount)
                {
                    for (int j = 0; j < pNode->m_lstTextures.m_nCount; j++)
                    {
                        pNode->m_lstTextures.m_pList[j]->Bind();
                        float fPosition = pNode->m_lstFlarePositions.m_pList[j];
                        Vector vPosition = vDirection * fPosition;
                        Vector vColor = (pNode->m_lstFlareColorShifts.m_pList[j] + pLight->m_vColor) *
                                        pLight->m_fFlareFade * fFacing *
                                        (pNode->m_fFlareRadius - fDistance) / pNode->m_fFlareRadius;
                        if (vColor.m_fX < 0.0f)
                            vColor.m_fX = 0.0f;
                        else if (vColor.m_fX > 1.0f)
                            vColor.m_fX = 1.0f;
                        if (vColor.m_fY < 0.0f)
                            vColor.m_fY = 0.0f;
                        else if (vColor.m_fY > 1.0f)
                            vColor.m_fY = 1.0f;
                        if (vColor.m_fZ < 0.0f)
                            vColor.m_fZ = 0.0f;
                        else if (vColor.m_fZ > 1.0f)
                            vColor.m_fZ = 1.0f;
                        float fSize = pNode->m_lstFlareSizes.m_pList[j] /
                                      pNode->m_fFlareRadius * (pNode->m_fFlareRadius - fDistance) *
                                      lensflaresize;
                        GLRender_PushCurrentMatrix();
                        GLRender_Translate(vPosition.m_fX, vPosition.m_fY, 0.0f);
#ifndef _XBOX
                        static unsigned long renderListToken = GLRender::RequestPool(
                            Aurora::POOL_VERTEX, 4 * sizeof(RenderVertexVTC),
                            RequestPoolManager::POOL_FLARE_USAGE, LENS_FLARE_POOL_FLAGS, &renderListToken);
                        RenderVertexVTC* pVertices = (RenderVertexVTC*)GLRender::LockPool(
                            renderListToken, 0, 0, RequestPoolManager::POOL_LOCK_WRITE);
#endif
                        int nRed, nGreen, nBlue;
                        roundToInt(nRed, vColor.m_fX * 255.0f);
                        roundToInt(nGreen, vColor.m_fY * 255.0f);
                        roundToInt(nBlue, vColor.m_fZ * 255.0f);
                        unsigned long nColor = nRed | ((nGreen | ((nBlue | (0xFF << 8)) << 8)) << 8);
                        for (int k = 0; k < 4; k++)
                        {
                            *(unsigned long*)pVertices[k].color = nColor;
                            pVertices[k].u = (k == 0 || k == 3) ? 0.0f : 1.0f;
                            pVertices[k].v = k < 2 ? 0.0f : 1.0f;
                            pVertices[k].position[0] = (k == 0 || k == 3) ? -fSize : fSize;
                            pVertices[k].position[1] = k < 2 ? -fSize : fSize;
                            pVertices[k].position[2] = 0.0f;
                        }
#ifdef _XBOX
                        GLRender::RenderObjectsVTCQuadUP(pVertices, 4);
#else
                        GLRender::UnlockPool(renderListToken);
                        GLRender::RenderObjectsVTCQuad(renderListToken, 4);
#endif
                        GLRender_PopCurrentMatrix();
                    }
                }
            }
        }
        if (g_ForceSight)
            GLRender_EnablePixelShaders(MODEL_SHADER_NOISE_DISABLE, 0, 0, 0, 0);
        GLRender_SetBlendFunc(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
        GLRender_Enable(ENABLE_DEPTH_TEST);
        GLRender_Enable(ENABLE_LIGHTING);
        if (m_bFog)
            GLRender_Enable(ENABLE_FOG);
        GLRender_SetMatrixMode(TRANSFORM_MODELVIEW);
        GLRender_PopCurrentMatrix();
        GLRender_SetMatrixMode(TRANSFORM_PROJECTION);
        GLRender_PopCurrentMatrix();
    }
}
// PC 0x0046E020
CAurScene* NewCAurScene(char* sName)
{
    return (CAurScene*)new Scene(sName);
}

static inline void AurSceneTrace(CHitInfo*)
{
    AurSceneTrace4();
}

static inline void AurSceneTrace4()
{
    AurSceneTrace3();
}

static inline void AurSceneTrace3()
{
    AurSceneTrace2();
}

static inline void AurSceneTrace2()
{
    AurSceneTrace1();
}

static inline void AurSceneTrace1()
{
}

#ifdef _XBOX

void AurDisableRoomFog(CAurRoom* pRoom)
{
    nodedata* pData = pRoom->m_pData;
    for (int i = 0; i < pData->m_lstTriMeshes.m_nCount; i++)
    {
        pData->m_lstTriMeshes.m_pList[i]->m_bFog = 0;
    }
}

char* disablefog(char* sArguments)
{
    CAurRoom* pRoom = Scenes.m_pList[0]->FindRoom(sArguments);
    if (!pRoom)
    {
        return "Cannot find room with that name";
    }
    AurDisableRoomFog(pRoom);
    return "Success.";
}

void Scene::ClearVisibility()
{
    for (int i = 0; i < m_lstRooms.m_nCount; i++)
    {
        m_lstRooms.m_pList[i]->m_lstVisibleRooms.m_nCount = 0;
    }
}

Vector Round(const Vector& vValue, float fRoundTo)
{
    return Vector(Round(vValue.m_fX, fRoundTo), Round(vValue.m_fY, fRoundTo), Round(vValue.m_fZ, fRoundTo));
}
#endif

#ifdef _XBOX

int g_bConsoleLineVisible;
char g_sConsoleLine[512];

void AurPostConsoleLine()
{
    char sLine[1024];
    sLine[0] = 0;
    if (g_bConsoleLineVisible)
    {
        sprintf(sLine, "> %s", g_sConsoleLine);
        AurPostString(sLine, 0, 0, -5.0f);
    }
}

void AurFlushPostedStrings()
{
    g_nMatrixMode = 1;
    GLRender::LoadMatrixIdentity();
    g_nMatrixMode = 1;
    g_pD3DDevice->SetTransform(D3DTS_VIEW, g_pViewMatrixStack->GetTop());
    g_nMatrixMode = 2;
    AurPostConsoleLine();
    AurRenderStrings();
    g_pD3DDevice->Present(NULL, NULL, NULL, NULL);
}
#endif

#ifdef _XBOX

PartTrigger* g_pConsoleTrigger;
extern List<Gob*> gobs;

char* enableforceanimatins(char* sArguments)
{
    Gob* pGob = FindGob(sArguments);
    if (!pGob)
        return "Cannot find gob.";
    pGob->EnableForceAnimations();
    return "Success.";
}

char* disableforceanimatins(char* sArguments)
{
    Gob* pGob = FindGob(sArguments);
    if (!pGob)
        return "Cannot find gob.";
    pGob->DisableForceAnimations();
    return "Success.";
}

char* enablesaturation(char* sArguments)
{
    AurEnableSaturation();
    return "SUCCESS";
}

char* disablesaturation(char* sArguments)
{
    AurDisableSaturation();
    return "SUCCESS";
}

char* highlighttrigger(char* sArguments)
{
    if (!g_pConsoleTrigger)
        return "Trigger does not exist";
    int bAffectDynamic = 0;
    float fHeight = 10.0f;
    int bHighlight;
    int nRead = sscanf(sArguments, "%d %f %d", &bHighlight, &fHeight, &bAffectDynamic);
    if (nRead != 1 && nRead != 2 && nRead != 3)
        return "Usage: highlighttrigger [0-1] [height] [affect dynamic]";
    AurPartTriggerSetHighlight(g_pConsoleTrigger, bHighlight != 0, fHeight, bAffectDynamic != 0);
    return "Success.";
}

char* highlightparams(char* sArguments)
{
    if (!g_pConsoleTrigger)
        return "Trigger does not exist";
    float fFrequency;
    int bPulse;
    float fAlpha;
    Vector vColor(0.0f, 0.0f, 0.0f);
    if (sscanf(sArguments, "%f%f%f %f %d %f", &vColor.m_fX, &vColor.m_fY, &vColor.m_fZ, &fAlpha, &bPulse,
               &fFrequency) != 6)
        return "Usage: highlightparams color alpha pulse frequency";
    AurPartTriggerSetHighlightParams(g_pConsoleTrigger, vColor, fAlpha, bPulse != 0, fFrequency, false);
    return "Success.";
}

char* getgobpos(char* sArguments)
{
    Gob* pGob = FindGob(sArguments);
    if (!pGob)
        return "NULL";
    char* sResult = new char[64];
    sprintf(sResult, "x %f y %f z %f", pGob->m_vPosition.m_fX, pGob->m_vPosition.m_fY, pGob->m_vPosition.m_fZ);
    return sResult;
}

char* enabletransparentcharacters(char* sArguments)
{
    float fAlpha = 0.5f;
    sscanf(sArguments, "%f", &fAlpha);
    for (int i = 0; i < gobs.m_nCount; i++)
    {
        Gob* pGob = gobs.m_pList[i];
        char bCharacter = ((GobModel*)pGob->m_pModel)->m_bCharacter;
        if (bCharacter)
            pGob->SetColorShifting(Vector(1.0f, 1.0f, 1.0f), fAlpha, 1);
    }
    rendertransparentcharacters = 1;
    return "Done.";
}

char* disabletransparentcharacters(char* sArguments)
{
    for (int i = 0; i < gobs.m_nCount; i++)
    {
        Gob* pGob = gobs.m_pList[i];
        char bCharacter = ((GobModel*)pGob->m_pModel)->m_bCharacter;
        if (bCharacter)
            pGob->SetColorShifting(Vector(1.0f, 1.0f, 1.0f), 1.0f, 1);
    }
    rendertransparentcharacters = 0;
    return "Done.";
}

char* disableanimations(char* sArguments)
{
    Scene* pScene = FindScene("scene");
    if (!pScene)
    {
        pScene = FindScene("mainscene");
        if (!pScene)
            return "Failed: couldn't find the main scene.";
    }
    pScene->DisableAnimations();
    return "Success.";
}

char* enableanimations(char* sArguments)
{
    Scene* pScene = FindScene("scene");
    if (!pScene)
    {
        pScene = FindScene("mainscene");
        if (!pScene)
            return "Failed: couldn't find the main scene.";
    }
    pScene->EnableAnimations();
    return "Success.";
}

char* setcurrentroom(char* sArguments)
{
    int nRoom;
    sscanf(sArguments, "%d", &nRoom);
    if (nRoom >= Scenes.m_pList[0]->GetNumRooms())
        return "eh? That room doesn't exist.";
    Scene* pScene = Scenes.m_pList[0];
    pScene->SetCurrentRoom(pScene->GetRoom(nRoom));
    return "Done.";
}

char* visibilityload(char* sArguments)
{
    char sName[16];
    sscanf(sArguments, "%s", sName);
    if (Scenes.m_pList[0]->LoadVisibility(sName))
        return "ok";
    return "nope";
}

char* visibilitysave(char* sArguments)
{
    char sName[16];
    sscanf(sArguments, "%s", sName);
    if (Scenes.m_pList[0]->SaveVisibility("", sName))
        return "ok";
    return "nope";
}

char* replacetilereferences(char* sArguments)
{
    char sNew[128];
    char sOld[128];
    Vector vPosition(0.0f, 0.0f, 0.0f);
    if (sscanf(sArguments, "%f%f%f %s %s", &vPosition.m_fX, &vPosition.m_fY, &vPosition.m_fZ, sOld, sNew) != 5)
        return "Usage: replacetilereferences <position> <oldtexname> <newtexname>";
    if (!AurTextureReplaceTileReferences(Scenes.m_pList[0], vPosition, sOld, sNew))
        return "Failure";
    return "Success";
}

char* finishscenegeometry(char* sArguments)
{
    Scene* pScene = FindScene("mainscene");
    if (!pScene)
    {
        pScene = FindScene("scene");
        if (!pScene)
            return "Failure - could not find scene.";
    }
    pScene->FinishSceneGeometry();
    return "Success.";
}

char* spawnrooms(char* sArguments)
{
    char sName[128];
    if (sscanf(sArguments, "%s", sName) != 1)
        return "USAGE: spawnrooms <layout filename>";
    if (!Scenes.m_pList[0]->SpawnRooms(sName))
        return "Could not open layout file...";
    return "Success, attempted to spawn the rooms";
}

void lightstaticmesh()
{
    for (int i = 0; i < Scenes.m_nCount; i++)
        Scenes.m_pList[i]->ComputeStaticLighting();
    staticshadowbucket.m_nCount = 0;
}

char* modifytrigger(char* sArguments)
{
    if (!g_pConsoleTrigger)
        return "Trigger does not exist";
    float pfColor[4];
    if (sscanf(sArguments, "%f %f %f %f", &pfColor[0], &pfColor[1], &pfColor[2], &pfColor[3]) != 4)
        return "Usage: modifytrigger r g b a";
    AurPartTriggerSetColor(g_pConsoleTrigger, pfColor);
    return "OK, set the trigger's color";
}

char* deletetrigger(char* sArguments)
{
    if (!g_pConsoleTrigger)
        return "Trigger does not exist";
    AurPartTriggerDelete(Scenes.m_pList[0], g_pConsoleTrigger);
    g_pConsoleTrigger = 0;
    return "OK, deleted the trigger from the scene";
}

char* setonlygob(char* sArguments)
{
    Gob* pGob = FindGob(sArguments);
    Scene* pScene = FindScene("mainscene");
    if (!pScene)
    {
        pScene = FindScene("scene");
        if (!pScene)
            return "Failure - cannot find scene.";
    }
    pScene->SetBlindLights(pGob);
    return "success";
}

void AuroraSetTileFoggedByWar(Vector& vPosition, bool bFogged);
char* bload(char* sArguments);

char* setfogofwar(char* sArguments)
{
    float pfPosition[3];
    pfPosition[0] = (float)atof(GetToken(&sArguments));
    pfPosition[1] = (float)atof(GetToken(&sArguments));
    pfPosition[2] = (float)atof(GetToken(&sArguments));
    int nFogged = atoi(GetToken(&sArguments));
    AuroraSetTileFoggedByWar(*(Vector*)pfPosition, nFogged == 1);
    return "FOGGED";
}

void SetPartLightShadowing(Part* pPart, int bShadow)
{
    PartLight* pLight = pPart->AsPartLight();
    if (pLight)
    {
        MdlNodeLight* pNode = (MdlNodeLight*)pLight->m_pNode;
        if (pNode->m_bAffectDynamic || pNode->m_nDynamicType)
            pLight->m_bShadow = bShadow;
    }
    for (int i = 0; i < pPart->m_lstChildren.m_nCount; i++)
        SetPartLightShadowing(pPart->m_lstChildren.m_pList[i], bShadow);
}

char* SetGobLightShadowing(char* sArguments, int bShadow)
{
    Gob* pGob = FindGob(sArguments);
    if (!pGob)
        return "no gob by that name";
    if (!pGob->m_bHasDynamicLights)
        return "no dynamic lights on that gob - nothing done";
    SetPartLightShadowing(pGob->m_pRootPart, bShadow);
    return "success";
}

char* enableshadowing(char* sArguments)
{
    return SetGobLightShadowing(sArguments, 1);
}

char* disableshadowing(char* sArguments)
{
    return SetGobLightShadowing(sArguments, 0);
}

char* initscenebsp(char* sArguments)
{
    int nWidth = 0;
    int nHeight = 0;
    int nBorderSize = 0;
    int bHasBins = 0;
    sscanf(sArguments, "%d%d%d%d", &nWidth, &nHeight, &nBorderSize, &bHasBins);
    if (!nWidth || !nHeight)
        return "USAGE: initscenebsp width height bordersize hasbins";
    Scenes.m_pList[0]->InitBSP(nWidth, nHeight, nBorderSize, bHasBins != 0, 10.0f);
    return "OK";
}

char* spawntile(char* sArguments)
{
    char sName[128];
    Vector vRead(0.0f, 0.0f, 0.0f);
    int nRotation;
    int bMeshesOnly;
    sName[0] = 0;
    nRotation = 0;
    bMeshesOnly = 0;
    sscanf(sArguments, "%s %f%f%f %d %d", sName, &vRead.m_fX, &vRead.m_fY, &vRead.m_fZ, &nRotation, &bMeshesOnly);
    Vector vPosition = vRead;
    bMeshesOnly = 0;
    Scene* pScene = FindScene("mainscene");
    if (!pScene)
        pScene = FindScene("scene");
    pScene->SpawnTile(sName, vPosition, nRotation, bMeshesOnly);
    return "OK";
}

char* spawnbinarytile(char* sArguments)
{
    char sName[128];
    char sModel[128];
    Vector vRead(0.0f, 0.0f, 0.0f);
    int nRotation;
    int bMeshesOnly;
    sName[0] = 0;
    nRotation = 0;
    bMeshesOnly = 0;
    sscanf(sArguments, "%s %f%f%f %d %d", sName, &vRead.m_fX, &vRead.m_fY, &vRead.m_fZ, &nRotation, &bMeshesOnly);
    Vector vPosition = vRead;
    bMeshesOnly = 0;
    sprintf(sModel, "%s.mdl", sName);
    bload(sModel);
    Scene* pScene = FindScene("mainscene");
    if (!pScene)
        FindScene("scene");
    pScene->SpawnTile(sName, vPosition, nRotation, bMeshesOnly);
    return "OK";
}

char* spawnbordertile(char* sArguments)
{
    char sName[128];
    Vector vRead(0.0f, 0.0f, 0.0f);
    int nRotation;
    sName[0] = 0;
    nRotation = 0;
    sscanf(sArguments, "%s %f%f%f %d", sName, &vRead.m_fX, &vRead.m_fY, &vRead.m_fZ, &nRotation);
    Vector vPosition = vRead;
    Scenes.m_pList[0]->SpawnTile(sName, vPosition, nRotation, 1);
    return "OK";
}

char* deletetile(char* sArguments)
{
    Vector vPosition(0.0f, 0.0f, 0.0f);
    if (sscanf(sArguments, "%f %f %f", &vPosition.m_fX, &vPosition.m_fY, &vPosition.m_fZ) != 3)
        return "Usage: deletetile <tile position>";
    Scene* pScene;
    if (Scenes.m_nCount == 1)
    {
        pScene = Scenes.m_pList[0];
    }
    else
    {
        pScene = FindScene("mainscene");
        if (!pScene)
            pScene = FindScene("scene");
    }
    pScene->DeleteTile(vPosition);
    return "OK, deleted any information at the given tile location";
}

char* settilelights(char* sArguments)
{
    Vector vPosition(0.0f, 0.0f, 0.0f);
    Vector vMainLight(0.0f, 0.0f, 0.0f);
    Vector vSecondaryLight(0.0f, 0.0f, 0.0f);
    if (sscanf(sArguments, "%f%f%f %f%f%f %f%f%f", &vPosition.m_fX, &vPosition.m_fY, &vPosition.m_fZ,
               &vMainLight.m_fX, &vMainLight.m_fY, &vMainLight.m_fZ,
               &vSecondaryLight.m_fX, &vSecondaryLight.m_fY, &vSecondaryLight.m_fZ) != 9)
        return "Usage: settilelights <tile position> <light#1 color> <light#2 color>";
    Scene* pScene;
    if (Scenes.m_nCount == 1)
    {
        pScene = Scenes.m_pList[0];
    }
    else
    {
        pScene = FindScene("mainscene");
        if (!pScene)
            pScene = FindScene("scene");
    }
    pScene->SetTileLights(vPosition, vMainLight, vSecondaryLight);
    return "OK, set tile lighting";
}

char* settileflames(char* sArguments)
{
    Vector vPosition(0.0f, 0.0f, 0.0f);
    char sFlame1[64];
    char sFlame2[64];
    sFlame1[0] = 0;
    sFlame2[0] = 0;
    int nRead = sscanf(sArguments, "%f%f%f %s %s", &vPosition.m_fX, &vPosition.m_fY, &vPosition.m_fZ, sFlame1,
                       sFlame2);
    if (nRead != 3 && nRead != 4 && nRead != 5)
        return "Usage: settileflames <tile position> [flame1] [flame2]";
    Scene* pScene;
    if (Scenes.m_nCount == 1)
    {
        pScene = Scenes.m_pList[0];
    }
    else
    {
        pScene = FindScene("mainscene");
        if (!pScene)
            pScene = FindScene("scene");
    }
    CAurObject* pFlame1 = NewCAurObject(sFlame1, "", 0, 0);
    CAurObject* pFlame2 = NewCAurObject(sFlame2, "", 0, 0);
    pScene->SetTileFlames(vPosition, pFlame1, pFlame2);
    return "OK, set tile lighting";
}

char* setglobalwind(char* sArguments)
{
    float fX;
    float fY;
    float fZ;
    if (sscanf(sArguments, "%f %f %f", &fX, &fY, &fZ) != 3)
        return "eh? Need 3 floats as input....";
    Scene* pScene = FindScene("mainscene");
    if (!pScene)
        pScene = FindScene("scene");
    if (pScene)
    {
        Vector vGlobalWind(fX, fY, fZ);
        pScene->m_pWindManager->SetGlobalWind(vGlobalWind);
        return "Done.";
    }
    return "eh? Couldn't find scene...";
}

char* setglobalwindvariation(char* sArguments)
{
    float fMaximumDeviation = 0.0f;
    float fVariation = 0.0f;
    int nRead = sscanf(sArguments, "%f %f", &fMaximumDeviation, &fVariation);
    if (nRead >= 1 && nRead <= 2)
    {
        Scene* pScene = FindScene("mainscene");
        if (!pScene)
            pScene = FindScene("scene");
        if (pScene)
        {
            pScene->m_pWindManager->SetMaximumDeviation(fMaximumDeviation * DEG_TO_RAD, fVariation * DEG_TO_RAD);
            return "Done.";
        }
        return "eh? Couldn't find scene...";
    }
    return "eh? Need 1 or 2 floats as input...";
}

char* setpointwind(char* sArguments)
{
    float fIntensity;
    float fDuration;
    float fRadius;
    Vector vPosition(0.0f, 0.0f, 0.0f);
    int nRead = sscanf(sArguments, "%f %f %f %f %f %f", &vPosition.m_fX, &vPosition.m_fY, &vPosition.m_fZ,
                       &fRadius, &fDuration, &fIntensity);
    if (nRead != 5 && nRead != 6)
        return "Usage: setpointwind position radius duration intensity";
    Scene* pScene = FindScene("mainscene");
    if (!pScene)
        pScene = FindScene("scene");
    if (pScene)
    {
        pScene->m_pWindManager->AddPointSourceWind(vPosition, fRadius, fDuration, fIntensity);
        return "Done.";
    }
    return "eh? Couldn't find scene...";
}

char* setgeomfade(char* sArguments)
{
    int nProperties = atoi(GetToken(&sArguments));
    Scene* pScene = FindScene("mainscene");
    if (!pScene)
    {
        FindScene("scene");
        return "NULL";
    }
    if (!pScene->SetGeomFadeProperties(nProperties))
        return "NULL";
    return "SUCCESS";
}

char* visibilityset(char* sArguments)
{
    int bVisible = 1;
    char sRoom[64];
    char sVisibleRoom[64];
    sVisibleRoom[0] = 0;
    sRoom[0] = 0;
    sscanf(sArguments, "%s %s %d", sRoom, sVisibleRoom, &bVisible);
    CAurRoom* pRoom = Scenes.m_pList[0]->FindRoom(sRoom);
    CAurRoom* pVisibleRoom = Scenes.m_pList[0]->FindRoom(sVisibleRoom);
    if (!pRoom || !pVisibleRoom)
        return " unable to find room by that name";
    Scenes.m_pList[0]->SetVisibility(pRoom, pVisibleRoom, bVisible);
    if (bVisible)
        return "ok set visibility";
    return "ok, cleared visibility";
}

char* hittrigger(char* sArguments)
{
    static char sResult[128];
    Scene* pScene = Scenes.m_pList[0];
    CHitInfo* pHitInfo = new CHitInfo(0, CHitInfo::HIT_TRIGGER, 0);
    Vector vStart(0.5f, 0.5f, 60.0f);
    Vector vHit(0.5f, 0.5f, -10.0f);
    if (pScene->HitCheck(vStart, vHit, &vHit, pHitInfo) && (pHitInfo->m_nOutputIndex & CHitInfo::HIT_TRIGGER))
    {
        sprintf(sResult, "OK, hit the trigger at (%.2f, %.2f, %.2f)", vHit.m_fX, vHit.m_fY, vHit.m_fZ);
        return sResult;
    }
    return "No hit on a trigger!";
}
#endif

#ifdef _XBOX
bool Scene::SetObjectScale(CAurObject* pObject, float fScale)
{
    return ((Gob*)pObject)->SetObjectScale(fScale, 1);
}
#endif
#ifdef _XBOX

extern int texturegettime;
extern int maxdecodetime;
extern int maxgltime;
extern int maxtexloadtime;
extern int maxtextime;
extern float maxdeltat;

char* dogantest(char* sArguments)
{
    maxappendtime = 0;
    maxmodelloadtime = 0;
    maxtotaltime = 0;
    texturegettime = 0;
    maxdecodetime = 0;
    maxgltime = 0;
    maxtexloadtime = 0;
    maxtextime = 0;
    maxdeltat = 0;
    return "done.";
}

char* setscale(char* sArguments)
{
    Gob* pGob = FindGob(GetToken(&sArguments));
    if (!pGob)
        return "NULL";
    float fScale = (float)atof(GetToken(&sArguments));
    if (pGob->m_pScene && !pGob->m_pScene->SetObjectScale(pGob, fScale))
        return "NULL";
    return "SUCCESS";
}

char* addtrigger(char* sArguments)
{
    if (g_pConsoleTrigger)
        return "Test trigger already exists";
    Scene* pScene = Scenes.m_pList[0];
    int bHitCheck = 1;
    if (sscanf(sArguments, "%d", &bHitCheck) > 1)
        return "Usage: addtrigger [geometry is 3d]";
    List<Vector> lstVertices(0);
    float afColor[4] = { 0.0f, 1.0f, 0.0f, 0.25f };
    lstVertices.Add(Vector(9.5f, 11.0f, 1.0f));
    lstVertices.Add(Vector(5.0f, 11.0f, 1.0f));
    lstVertices.Add(Vector(8.0f, 14.0f, 1.0f));
    lstVertices.Add(Vector(0.0f, 10.0f, 1.0f));
    lstVertices.Add(Vector(8.0f, 6.0f, 1.0f));
    lstVertices.Add(Vector(8.2f, 6.2f, 1.0f));
    lstVertices.Add(Vector(4.9f, 9.0f, 1.0f));
    lstVertices.Add(Vector(9.5f, 9.0f, 1.0f));
    g_pConsoleTrigger = AurPartTriggerCreate(pScene, lstVertices.m_pList, lstVertices.m_nCount, bHitCheck, afColor, 0);
    lstVertices.m_nCount = 0;
    lstVertices.Add(Vector(0.0f, 0.0f, 1.0f));
    lstVertices.Add(Vector(1.0f, 0.0f, 4.0f));
    lstVertices.Add(Vector(6.0f, 7.0f, 5.0f));
    lstVertices.Add(Vector(0.0f, 1.0f, 10.0f));
    g_pConsoleTrigger = AurPartTriggerCreate(pScene, lstVertices.m_pList, lstVertices.m_nCount, bHitCheck, afColor, 0);
    return "OK, added the trigger to the scene";
}

extern float ViewAngle;
extern float clipnear;
extern float clipfar;
extern int g_nScreenWidth;
extern int g_nScreenHeight;

void AurReshape(int nWidth, int nHeight)
{
    g_nScreenWidth = nWidth;
    g_nScreenHeight = nHeight;
    if (nWidth < 1)
        nWidth = 1;
    if (nHeight < 1)
        nHeight = 1;
    if (nWidth == 1 || nHeight == 1)
        return;
    GLRender::ResetViewPort(0, 0, nWidth, nHeight);
    g_nMatrixMode = 0;
    GLRender::LoadMatrixIdentity();
    GLRender::SetPerspectiveTransform(ViewAngle, (double)nWidth / nHeight, clipnear, clipfar);
    g_nMatrixMode = 2;
    GLRender::LoadMatrixIdentity();
}

char* reshape(char* sArguments)
{
    static char sResult[256];
    AurReshape(g_nScreenWidth, g_nScreenHeight);
    sprintf(sResult, "win=%dx%d fov=%2.0f clip=%4.3f,%4.1f", g_nScreenWidth, g_nScreenHeight, ViewAngle, clipnear, clipfar);
    return sResult;
}
#endif
