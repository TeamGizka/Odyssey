#ifndef __AURSCENEMANAGER_H__
#define __AURSCENEMANAGER_H__

#include <aurlist.h>

class CAurTriangleBin;
class Gob;
class PartEmitter;
class PartLight;
class PartProjection;
class PartShadow;
class PartBeam;
class PartTriMesh;
class PartTrigger;
class TileData;
class Scene;
class CAurTexture;
class Part;
class Vector;
class BSPNode;
class CHitInfo;
class CAurObject;

extern CHitInfo* CurrentHitInfo;
int HitPartOnBSPNode(BSPNode* pNode, Vector vStart, Vector vEnd, Vector* pHit);

void AddLightToRooms(Scene* pScene, PartLight* pLight);
void UpdateStaticLighting(Scene* pScene, PartLight* pLight);
void AnimateStaticLight(CAurObject* pObject, char* sEvent, void* pData);
void ManageSceneBSP(Scene* pScene);
TileData* SceneBSPGetTileData(Scene* pScene, const Vector& vPosition);
void SceneBSPAddGob(Scene* pScene, Gob* pGob);
void SceneBSPRemoveGob(Scene* pScene, Gob* pGob);
void LightPartTriMesh(PartTriMesh* pPart, List<PartLight*> lstLights, Scene* pScene);
void SceneBSPGetObjectTextures(Part* pPart, List<CAurTexture*>& lstTextures);
void SceneBSPGetTileTextures(Scene* pScene, const Vector& vPosition,
                             List<CAurTexture*>& lstTextures);

class nodedata
{
public:
    List<PartLight*> m_lstLights;
    List<Gob*> m_lstGobs;
    List<PartTriMesh*> m_lstTriMeshes;
    List<PartEmitter*> m_lstEmitters;
    List<PartShadow*> m_lstShadowProjections;
    List<PartBeam*> m_lstBeamProjections;
    List<Part*> m_lstOthers;
    List<PartTrigger*> m_lstTriggers;
    List<CAurTriangleBin*> m_lstBins;
    PartTriMesh* m_pAABB;
    TileData* m_pTileData;
    List<Gob*> m_lstMovingObjects;

    List<PartLight*> m_lstMovingLights;
    List<PartEmitter*> m_lstMovingEmitters;

    void AddMovingObject(Gob* pGob);

    void AddMovingEmitter(PartEmitter* pEmitter);
#ifdef _XBOX

    void ClearMovingObjects();
    void ClearMovingLights();
    void ClearMovingEmitters();
    List<Gob*>& GetMovingObjects();
    List<PartLight*>& GetMovingLights();
    List<PartEmitter*>& GetMovingEmitters();
    void AddMovingLight(PartLight* pLight);
#endif
    ~nodedata();

    nodedata();
};

class Bucket : public List<PartTriMesh*>
{
};

#define AUR_MESH_BUCKET_COUNT 5000
extern Bucket meshbuckets[AUR_MESH_BUCKET_COUNT];
extern Bucket fadebuckets[AUR_MESH_BUCKET_COUNT];
extern Bucket capbuckets[AUR_MESH_BUCKET_COUNT];
extern Bucket g_listBackgroundBucket;
int AurClearActiveBins();
void DeleteNonGobParts(class BSPNode* pNode, void* pData);

extern List<CAurTriangleBin*> g_pOldActiveBins;
extern List<CAurTriangleBin*> g_pActiveBins;

extern List<Gob*> blurbucket;
extern List<Gob*> g_listForcePushBucket;
extern List<Gob*> g_listLightsaberBucket;
extern List<Gob*> g_listHologramBucket;
extern List<Part*> staticbeambucket;
extern List<CAurTriangleBin*> binbucket;
extern List<Part*> staticshadowbucket;
extern List<Gob*> gobbucket;
extern List<PartTriMesh*> meshshadowbucket;
extern List<PartTrigger*> triggerbucket;
extern List<PartEmitter*> emitterbucket;

#endif
