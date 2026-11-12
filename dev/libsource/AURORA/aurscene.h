#ifndef AURSCENE_H
#define AURSCENE_H

class Vector;
#include "math3d.h"
#include <aurlist.h>
#include <aurpointer.h>
#include "math3d.h"
#include "aurbehavior.h"
#include "aurmodel.h"

class CAurObject;
class CAurTexture;
class CAurRoom;
class CAurTriangleBin;
class CHitInfo;
class Gob;
class LightManager;
class Model;
class Part;
class BSPNode;
class PartLight;
class PartEmitter;
class TileData;
class TileData;
class Quaternion;
class WindManager;

#define AUR_FOG_TEST_START 0.1f
#define AUR_FOG_TEST_END 20.0f
#define AUR_FOG_TEST_DENSITY 0.1f

#define AUR_SHADOW_LIGHT_HEIGHT 1.75f

#define AUR_SHADOW_MAX_RADIUS 50.0f
#define AUR_SHADOW_GOB_RANGE_SQUARED 1100.0f

#define AUR_SHADOW_OPACITY 0.5f
#define AUR_SHADOW_FAN_ANGLE_STEP (40.0f * DEG_TO_RAD)

#define AUR_TILE_ROTATION_DEGREES 90.0f

#define AUR_ROOM_UNSET_BOUND 100000.0f
#define AUR_ROOM_AABB_UNSET_BOUND 1000000.0f

enum { AUR_ROOM_ANIM_RANDOM_STEPS = 100 };
#define AUR_ROOM_ANIM_SPEED_CENTER 50.0f
#define AUR_ROOM_ANIM_SPEED_STEP 0.001f
#define AUR_ROOM_ANIM_PHASE_STEP 0.01f

class CAurScene : public SafePointerList<CAurScene>
{
public:
    static const int ENABLE_FADE;
    static const int DISABLE_FADE;
    static const int ALWAYS_FADE;

    CAurScene();
    virtual ~CAurScene();
    virtual int SetFog(int bFog) = 0;
    virtual void SetFogColor(const Vector& vColor) = 0;
    virtual int GetObjectA(char* sName) = 0;
#ifdef _XBOX
    virtual bool SetObjectScale(CAurObject* pObject, float fScale) = 0;
#else
    virtual void SetObjectScale(CAurObject* pObject, float fScale) = 0;
#endif
    virtual float GetObjectScale(CAurObject* pObject) = 0;

    virtual void Render() = 0;
    virtual void Animate(float fTimeStep) = 0;
    virtual void InitBSP(int, int, int, bool, float) = 0;
    virtual CAurObject* SpawnTile(char* sName, const Vector& vPosition, int nRotation, int bMeshesOnly) = 0;
    virtual void SetTileLights(const Vector& vPosition, const Vector& vMainLight, const Vector& vSourceLight) = 0;
    virtual void SetTileFlames(const Vector& vPosition, CAurObject* pFlame1, CAurObject* pFlame2) = 0;
    virtual int ReplaceTileTexture(const Vector& vPosition, char* sOld, char* sNew) = 0;
    virtual void DeleteTile(const Vector& vPosition) = 0;
    virtual int HitCheck(Vector vStart, Vector vEnd, Vector* pHit,
                         CHitInfo* pHitInfo) = 0;
    virtual void ComputeStaticLighting();
    virtual void GetFogRange(float& fNear, float& fFar) = 0;
    virtual void SetFogRange(float fNear, float fFar) = 0;
    virtual void SetSceneFocus(Vector vFocus) = 0;
    virtual unsigned char SetGeomFadeProperties(int nProperties) = 0;
    virtual char IsGeomFadeEnabled() = 0;
    virtual int GetObjectCount() = 0;
    virtual void FinishSceneGeometry() = 0;
    virtual int GetNumRooms() = 0;
    virtual void EnableAnimations() = 0;
    virtual void DisableAnimations() = 0;
    virtual int SetBlindLights(CAurObject* pObject) = 0;
    virtual int SpawnRooms(char* sName) = 0;
    virtual Gob* SpawnRoom(char* sName, const Vector& vPosition,
                          const Quaternion& qOrientation) = 0;
    virtual CAurRoom* FindRoom(char* sName) = 0;
    virtual CAurRoom* GetRoom(int nRoom) = 0;
    virtual int SetCurrentRoom(CAurRoom* pRoom) = 0;
    virtual int SetCurrentRoom(int nRoom) = 0;
    virtual int GetIgnoreVisibilityGraph() = 0;
    virtual int EnableVisibilityGraph() = 0;
    virtual void DisableVisibilityGraph() = 0;
    virtual int SaveVisibility(char* sLayout, char* sVisibility) = 0;
    virtual int LoadVisibility(char* sName) = 0;
    virtual void RenderGobFirst(CAurObject* pObject) = 0;
    virtual void RenderGobLast(CAurObject* pObject) = 0;
    virtual void RenderGobAfter(CAurObject* pObject, CAurObject* pAfter) = 0;
    virtual void RenderGobBefore(CAurObject* pObject, CAurObject* pBefore) = 0;

    void SetAmbient(Vector vAmbient);

};

class Scene : public CAurScene
{
public:
    static const int FADING_OFF;
    static const int FADING_ON;

    Scene(char* sName);
    virtual ~Scene();
    virtual int SetFog(int bFog)  // PC 0x0046DC80
    {
        m_bFog = bFog;
        return bFog;
    }
    virtual void SetFogColor(const Vector& vColor);
    virtual int GetObjectA(char* sName);
#ifdef _XBOX
    virtual bool SetObjectScale(CAurObject* pObject, float fScale);
#else
    virtual void SetObjectScale(CAurObject* pObject, float fScale);
#endif
    virtual float GetObjectScale(CAurObject* pObject);

    virtual void Render();
    virtual void Animate(float fTimeStep);

    virtual void InitBSP(int, int, int, bool, float);
    virtual CAurObject* SpawnTile(char* sName, const Vector& vPosition, int nRotation, int bMeshesOnly);
    virtual void SetTileLights(const Vector& vPosition, const Vector& vMainLight, const Vector& vSourceLight);
    virtual void SetTileFlames(const Vector& vPosition, CAurObject* pFlame1, CAurObject* pFlame2);
    virtual int ReplaceTileTexture(const Vector& vPosition, char* sOld, char* sNew);
    virtual void DeleteTile(const Vector& vPosition);
    virtual int HitCheck(Vector vStart, Vector vEnd, Vector* pHit,
                         CHitInfo* pHitInfo);
    virtual void GetFogRange(float& fNear, float& fFar)  // PC 0x0046DC90
    {
        fNear = m_fFogNear;
        fFar = m_fFogFar;
    }
    virtual void SetFogRange(float fNear, float fFar)  // PC 0x0046DCB0
    {
        m_fFogNear = fNear;
        m_fFogFar = fFar;
    }
    virtual void SetSceneFocus(Vector vFocus)  // PC 0x0046DCD0
    {
        m_vSceneFocus = vFocus;
    }
    virtual unsigned char SetGeomFadeProperties(int nProperties);
    virtual char IsGeomFadeEnabled()  // PC 0x0046DCF0
    {
        return geomFadeEnabled == 1;
    }
    virtual int GetObjectCount()  // PC 0x0046DD00
    {
        return m_lstObjects.m_nCount;
    }
    virtual void FinishSceneGeometry();
    virtual int GetNumRooms();
    virtual void EnableAnimations();
    virtual void DisableAnimations();
    virtual int SetBlindLights(CAurObject* pObject);
    virtual int SpawnRooms(char* sName);
    virtual Gob* SpawnRoom(char* sName, const Vector& vPosition,
                          const Quaternion& qOrientation);
    virtual CAurRoom* FindRoom(char* sName);
    virtual CAurRoom* GetRoom(int nRoom);
    virtual int SetCurrentRoom(CAurRoom* pRoom);
    virtual int SetCurrentRoom(int nRoom);
    virtual int GetIgnoreVisibilityGraph();
    virtual int EnableVisibilityGraph();
    virtual void DisableVisibilityGraph();
    virtual int SaveVisibility(char* sLayout, char* sVisibility);
    virtual int LoadVisibility(char* sName);
#ifdef _XBOX

    void ClearVisibility();
#endif

    virtual void RenderGobFirst(CAurObject* pObject);
    virtual void RenderGobLast(CAurObject* pObject);
    virtual void RenderGobAfter(CAurObject* pObject, CAurObject* pAfter);
    virtual void RenderGobBefore(CAurObject* pObject, CAurObject* pBefore);
    virtual char IsAnimating();
    virtual void AddObject(CAurObject* pObject)  // PC 0x0046DD10
    {
        ChildAdd((Gob*)pObject);
    }
    virtual void ChildAdd(Gob* pGob);
    virtual void ChildRemove(Gob* pGob);

    virtual void RenderSinglePass();

    virtual void RenderShadows(int, int, int, int);

    virtual void RenderDynamicGeometry();

    virtual void RenderStaticGeometry();

    virtual void RenderLensFlares();
    virtual void RenderMotionBlur();
    virtual Vector GetWindVector(Vector vPosition, float fScale);
    virtual void PostProcessLS();

    virtual void DoMeshBuckets();
    virtual void DoBlurBuckets();

    virtual void DoGobBuckets();
    virtual void DoHologramGobBuckets();
    virtual void DoDistortionBuckets();
    virtual void DoLightSaberBucket();
    virtual void DoCapBucket();
    virtual void DoBinBucket();
    virtual void DoFadeBuckets();

    virtual void DoGobShadows();
    virtual void DoGobShadowsForThisLight(PartLight& light);

    virtual void RenderStencilShadowsForThisLight(PartLight& light, float fAlpha,
                                                  int nStencilBits);
    virtual void DoStaticMeshShadows();
    virtual void DoStaticMeshBeams();

    virtual void DoDynamicMeshProjections();
    virtual void DoEmitterBucket();
    virtual void DoTriggerBucket();

    virtual void DoHighlightTriggers(bool bHighlight);
    virtual void SetSingleTileFlame(CAurObject* pObject, Gob** ppGob,
                                   Part* pPart);
    virtual int RenderOnlyGobs();
    virtual void DoFog();
    virtual void AddFacesToBins(PartAABB* pAABB, const Vector& vMinimum,
                               List<List<CAurTriangleBin*> >& lstBins);

    static void RemoveFromSceneManager(Part* pPart);
    static void RemoveFromSceneManager(Gob* pGob);
    void UnlistSpecialParts(Part* pPart);
    void ListSpecialPartsR(Part* pPart);
    void UpdateEmitters();
    CAurObject* HitCheckGob(Vector vStart, Vector vEnd, Vector* pHit,
                           CHitInfo* pHitInfo);

    enum { EMITTER_WARMUP_STEPS = 100 };

    enum { LENS_FLARE_POOL_FLAGS = 0x1011 };
    void SetVisibility(CAurRoom* pRoom, CAurRoom* pVisibleRoom, int bVisible);

    static int geomFadeEnabled;
    static float binsize;
    static float cellsize;
    static Vector m_vLastCameraPosition;
    static Quaternion m_qLastCameraOrientation;

    char m_sName[64];

    int m_nAnimationFrame;
    List<Gob*> m_lstObjects;
    int m_bFog;
    float m_fFogNear;
    float m_fFogFar;

    int m_nFogMode;
    float m_fFogDensity;
    Vector m_vFogColor;
    Vector m_v80;
    LightManager* m_pLightManager;
    WindManager* m_pWindManager;
    Vector m_vAmbient;

    List<PartEmitter*> m_lstEmitters;
    BSPNode* m_pBSPRoot;

    SafePointer<CAurTexture> m_pNormalizationCubeMap;
    Vector m_vSceneFocus;

    float m_fHighlightTime;

    int m_nCameraFadeState;

    List<void*> m_lstPendingObjects;
    List<CAurRoom*> m_lstRooms;
    CAurRoom* m_pCurrentRoom;
    short m_bIgnoreVisibilityGraph;
    char m_bAnimating;

    char m_bBinsPopulated;

    int m_nSceneFlags;
    float m_fSceneScale;

    CAurTexture* m_pFilmNoiseTexture;
    CAurTexture* m_pDistortionTexture;
    CAurTexture* m_pDirtyTexture;
    CAurTexture* m_pDirtyTexture2;
    CAurTexture* m_pDirtyTexture3;
    CAurTexture* m_pClairTexture;
    CAurTexture* m_pHoloTexture;
    List<Gob*> m_lstAnimatedObjects;

    Gob m_SceneRoot;
};

CAurScene* NewCAurScene(char* sName);
void ProjectDynamicShadow(Gob* pGob, PartLight* pLight);

inline void AurSceneTrace(const void*)
{
}

class CHitInfo
{
public:

    enum HitFlags
    {
        HIT_TRIGGER = 0x40000000,

        HIT_ALL_NON_TRIGGER = ~HIT_TRIGGER
    };

    enum HitType
    {
        HIT_TYPE_VISUAL_EFFECT_GROUND = 1
    };

    enum HitCategory
    {
        HIT_STATIC = 1,
        HIT_GOB = 2
    };

    CHitInfo(int nType, int nFlags, int nMask);
    // PC 0x0046B430
    ~CHitInfo()
    {
        AurSceneTrace(this);
    }

    void ClearOutput();
    void Reset(int nType, int nFlags, int nMask);

    int m_nType;
    int m_nFlags;

    List<CAurObject*> m_lstHits;
    int m_nMask;
    int m_nOutputFlags;
    Vector m_vOutputPosition;
    Vector m_vOutputVector;
    int m_nOutputIndex;
    SafePointer<CAurObject> m_pOutputObject;
    int m_nOutputType;
    Vector m_vOutputNormal;

    char* m_sOutputPartName;

    void* m_pOutputHitData;

    CAurRoom* m_pOutputRoom;
    int m_nResult;
    unsigned char m_bResult0;
    unsigned char m_bResult1;
    unsigned char m_bResult2;
    unsigned char m_bResult3;
#ifndef _XBOX
    unsigned char m_bResult4;
#endif
};

extern CHitInfo DefaultHitInfo;

class CAurRoom : public BSPNode
{
public:
    List<CAurRoom*> m_lstVisibleRooms;

    List<Gob*> m_lstAnimatedObjects;
    Gob* m_pRoomObject;
    char m_sName[64];
    char m_bB8;
    ~CAurRoom();
    CAurRoom(char*);
};

void SceneBSPRemovePart(Scene* pScene, Part* pPart);
MdlNode* findnode(class MdlNode* pNode, char* sName);

#define TILE_ROTATION_DEGREES 90.0f
void SetPartTextureRotate(Part* pPart, int nRotation);

void ExpandReferenceParts(Part* pPart);
void CollapsePartTree(Gob* pGob, Part* pPart, List<Part*>& lstParts,
                      Vector vPosition, Quaternion qOrientation,
                      Scene* pScene, CAurRoom* pRoom);
Gob* SpawnPartsForTile(char* sName, const Vector& vPosition,
                       const Quaternion& qOrientation, int nRotation,
                       int bMeshesOnly, List<Part*>& lstParts);
void SceneBSPRemoveTiledata(Scene* pScene, const Vector& vPosition);
void SceneBSPAddTileData(Scene* pScene, TileData* pTileData, Vector vPosition);
void SceneRemovePartFromRooms(Scene* pScene, Part* pPart);
void SceneAddPartToRooms(Scene* pScene, Part* pPart);
int SceneBSPAddPart(Scene* pScene, Part* pPart, Model* pModel);

extern int g_bInScreenshotMode;
void AurEnableInterleavedAnimations();
void AurDisableInterleavedAnimations();
char AurSetGlobalWind(Vector vWind);
bool AurSetGlobalWindVariation(float fMaximumDeviation, float fVariation);
extern float shadowalpha;
extern int enablegetnearestlights;
extern int skippauseplaneforscreenshot;

#endif
