#ifndef AUREMITTER_H
#define AUREMITTER_H

class Gob;
class MaxTree;
class MdlNodeTriMesh;
class Quaternion;
class Vector;
#include "math3d.h"
#include <aurlist.h>
#include <aurpointer.h>

class Scene;
class CallBack;
class MdlNode;
class Part;
class WindManager;
class Material;
class CHitInfo;
class Model;
class PartTriMesh;
class TileData;
class PartLight;
class PartCamera;
class PartReference;
class PartEmitter;
class PartTrigger;
class PartAnimMesh;
class PartProjection;
class PartShadow;
class PartBeam;
class PartAABB;
class PartDanglyMesh;
class PartSkin;
class PartEmitterData;
class LightningEmitter;
class CAurObject;
struct RenderVertexVTC;

extern int totalparticles;
extern Scene* currentscene;
extern float birth;

extern int EmitterBounceType;
enum EmitterBounceMode
{
    EMITTER_BOUNCE_DAMPED = 1,
    EMITTER_BOUNCE_ELASTIC = 2
};

#define EMITTER_WIND_SAMPLE_THRESHOLD 50
#define EMITTER_WIND_SAMPLE_STRIDE 6

#define EMITTER_BOUNCE_REFERENCE_STEP 0.033f
#define EMITTER_BOUNCE_MIN_SCALE 0.5f
#define EMITTER_BOUNCE_SPEED_DAMPING 0.75f
#define EMITTER_BOUNCE_REST_SPEED_SQUARED 0.5
#define EMITTER_BOUNCE_REST_ANGLE 0.7853981633974483
#define EMITTER_BOUNCE_REST_SPEED_PER_SECOND 26.0f
#define EMITTER_BOUNCE_SURFACE_OFFSET 0.02f

#define EMITTER_GRAVITY -9.81f

#define EMITTER_GRAVITY_MAGNITUDE 9.81f
#define EMITTER_COLLISION_OFFSET 0.005f
#define EMITTER_COLLISION_PUSH_OFF EMITTER_COLLISION_OFFSET
#define EMITTER_SETTLED_OFFSET 0.02f
#define EMITTER_SETTLED_PUSH_OFF EMITTER_SETTLED_OFFSET
#define EMITTER_SETTLED_SPEED_SQUARED 0.5f
#define EMITTER_LINK_LENGTH_EPSILON 0.00001
#define EMITTER_EFFECT_RADIUS 10000.0f
#define EMITTER_RADIUS_PADDING 1.15f

#define EMITTER_UNSET_COLOR_KEY_TIME 255.0f

#define EMITTER_INITIAL_ELAPSED_TIME 10000.0f

#define EMITTER_DEADSPACE_ENABLED 1

extern float emitterfadeheight;

class CAurPart
{
public:
    virtual ~CAurPart();
    virtual char* Name() = 0;
    virtual void GetLocal(Vector& vPosition, Quaternion& qOrientation) = 0;
    virtual void GetWorld(Vector& vPosition, Quaternion& qOrientation) = 0;
    virtual List<Part*>* GetChildren() = 0;
    virtual Part* GetParent() = 0;
    virtual CAurObject* GetAurObject() = 0;
    virtual CAurPart* HitCheck(Vector vStart, Vector vEnd, Vector* pHit,
                         CHitInfo* pInfo, float fRadius) = 0;
};

class Part : public CAurPart
{
public:
    Part(MdlNode* pNode);
    virtual ~Part();
    virtual char* Name();
    virtual void GetLocal(Vector& vPosition, Quaternion& qOrientation);
    virtual void GetWorld(Vector& vPosition, Quaternion& qOrientation);
    virtual List<Part*>* GetChildren()  // PC 0x00453060
    {
        return &m_lstChildren;
    }
    virtual Part* GetParent()  // PC 0x00786830
    {
        return m_pParent;
    }
    virtual CAurObject* GetAurObject();
    virtual CAurPart* HitCheck(Vector vStart, Vector vEnd, Vector* pHit,
                         CHitInfo* pInfo, float fRadius);
    virtual int HitCheckGeom(const Vector& vStart, Vector& vEnd,
                             Vector* pHit, CHitInfo* pHitInfo, float fRadius);

    enum HitGeomResult { HIT_GEOM_EXCLUDED = -1 };
    virtual PartLight* AsPartLight() { return 0; }  // PC 0x004065A0
    virtual PartCamera* AsPartCamera() { return 0; }  // PC 0x004065A0
    virtual PartTriMesh* AsPartTriMesh() { return 0; }  // PC 0x004065A0
    virtual PartAnimMesh* AsPartAnimMesh() { return 0; }  // PC 0x004065A0
    virtual PartEmitter* AsPartEmitter() { return 0; }  // PC 0x004065A0
    virtual PartReference* AsPartReference() { return 0; }  // PC 0x004065A0
    virtual PartProjection* AsPartProjection() { return 0; }  // PC 0x004065A0
    virtual PartShadow* AsPartShadow() { return 0; }  // PC 0x004065A0
    virtual PartBeam* AsPartBeam() { return 0; }  // PC 0x004065A0
    virtual PartAABB* AsPartAABB() { return 0; }  // PC 0x004065A0
    virtual PartDanglyMesh* AsPartDanglyMesh() { return 0; }  // PC 0x004065A0
    virtual PartSkin* AsPartSkin() { return 0; }  // PC 0x004065A0
    virtual PartTrigger* AsPartTrigger() { return 0; }  // PC 0x004065A0
    virtual Part* AsPartDummy() { return this; }  // PC 0x00698BD0
    virtual void Project(const Vector& vPoint, float fDistance,
                         const Plane& cPlane, class CAurShadowData* pShadowData);
    virtual void Mat();
    virtual void Draw(bool bRenderChildren);
    virtual void Animate(float fTimeStep);
    virtual void PostProcess(Part* pPart);
    virtual Vector SetPartPosition(Vector vPosition);
    virtual void GetMinimumBoundingBox(Vector& vMinimum, Vector& vMaximum,
                                       float fScale);
    virtual void GetMinimumSphere(Vector& vPosition, float& fRadius);

    MdlNode* m_pNode;
    Vector m_vPosition;
    Quaternion m_qOrientation;

    float m_fScale;

    int m_nNodeID;

    char m_bIncludeChildBounds;

    List<Part*> m_lstChildren;

    Part* m_pParent;

    Gob* m_pAurObject;
    Material* m_pMaterial;

    int m_nInstanceState;

};

class PartCamera : public Part
{
public:
    PartCamera(MdlNode* pNode) : Part(pNode) {}
    virtual PartCamera* AsPartCamera() { return this; }  // PC 0x00698BD0
    virtual PartCamera* AsPartDummy() { return 0; }  // PC 0x004065A0
};

class PartProjection : public Part
{
public:
    static int m_nNextProjectionID;

    int m_nReserved4C;
    int m_nReserved50;
    Vector m_vMinimum;
    Vector m_vMaximum;
};

class Model;
class MdlNodeEmitter;
class MdlNodeAnimMesh;
class MdlNodeCamera;
class MdlNodeDanglyMesh;
class MdlNodeLight;
class MdlNodeReference;
class MdlNodeSkin;
class MdlNodeAABB;
class MdlNodeLightsaber;
class MdlNodeTrigger;

enum MdlNodeType
{
    MDL_NODE_HEADER = 0x001,
    MDL_NODE_LIGHT = 0x002,
    MDL_NODE_EMITTER = 0x004,
    MDL_NODE_CAMERA = 0x008,
    MDL_NODE_REFERENCE = 0x010,
    MDL_NODE_TRIMESH = 0x020,
    MDL_NODE_SKIN = 0x040,
    MDL_NODE_ANIMMESH = 0x080,
    MDL_NODE_DANGLYMESH = 0x100,
    MDL_NODE_AABB = 0x200,

    MDL_NODE_TRIGGER = 0x400,

    MDL_NODE_LIGHTSABER = 0x800
};

#define MDL_SKIN_SEAM_TOLERANCE 0.001f

class NewController;

class MdlNode
{
public:
    MdlNode(char* sName);
    ~MdlNode();
    static const short MDLNODE_TRIGGER_ID;
    static const short MDLNODE_BASE_ID;
    static const short MDLNODE_LIGHT_ID;
    static const short MDLNODE_EMITTER_ID;
    static const short MDLNODE_CAMERA_ID;
    static const short MDLNODE_REFERENCE_ID;
    static const short MDLNODE_TRIMESH_ID;
    static const short MDLNODE_SKIN_ID;
    static const short MDLNODE_ANIM_ID;
    static const short MDLNODE_DANGLY_ID;
    static const short MDLNODE_AABB_ID;
    static const short MDLNODE_LIGHTSABER_ID;

    short m_nNodeType;

    short m_nNodeID;

    int m_nIndex;
    MaxTree* m_pModel;
    MdlNode* m_pParentNode;

    Vector m_vPosition;
    Quaternion m_qOrientation;

    List<MdlNode*> m_lChildren;
    List<NewController> m_lstControllers;
    List<float> m_lstControllerData;

    MdlNodeEmitter* AsMdlNodeEmitter();
    MdlNodeAnimMesh* AsMdlNodeAnimMesh();
    MdlNodeCamera* AsMdlNodeCamera();
    MdlNodeDanglyMesh* AsMdlNodeDanglyMesh();
    MdlNodeLight* AsMdlNodeLight();
    MdlNodeReference* AsMdlNodeReference();
    MdlNodeSkin* AsMdlNodeSkin();
    MdlNodeTriMesh* AsMdlNodeTriMesh();
    MdlNodeAABB* AsMdlNodeAABB();
    MdlNodeLightsaber* AsMdlNodeLightsaber();
#ifdef _XBOX
    MdlNodeTrigger* AsMdlNodeTrigger();
#else
    MdlNodeTrigger* AsMdlNodeTrigger()
    {
        if ((m_nNodeType & (MDL_NODE_HEADER | MDL_NODE_TRIGGER)) ==
            (MDL_NODE_HEADER | MDL_NODE_TRIGGER))
            return (MdlNodeTrigger*)this;
        return 0;
    }
#endif
    Part* InternalCreateInstance(Gob* pGob);
    Part* CreateInstanceDispatch(Gob* pGob);

    char* TranslateNodeName();
    void PostProcessDispatch();
#ifdef _XBOX
    void SortChildrenByTransparency();
#endif
    void ProcessSkinSeams();
    int CreateSkinNodeList(List<MdlNodeTriMesh*>& lstNodes);
    void InternalPostProcess();
    void InternalControl(Part* pPart, float fTime, float fPreviousTime,
                         float fBlend);
    void ControlDispatch(Part* pPart, float fTime, float fPreviousTime,
                        float fBlend);
    void InternalParseField(char* szField);
    void ParseFieldDispatch(char* szField);
    MdlNode* ConJoinDispatch();
    void InternalDestructor();
    void DestructorDispatch();
    void AddToFreeList(MdlNode* pNode);
    void AddToFreeTokenList(unsigned long nPool);
    MdlNode* InternalConJoin();
};

class MdlNodeEmitter : public MdlNode
{
public:

    enum RandomFlags { RANDOM_ENABLED = 1, RANDOM_ORDER = 2 };
    enum { POINT_TO_POINT_SELECTED = 1 };
    void InternalParseField(char* szField);
    MdlNodeEmitter(char* sName);
    Part* InternalCreateInstance(Gob* pGob);
    MdlNode* InternalConJoin();
    unsigned char IsAffectedByWind();
    unsigned char IsPointToPoint();
    unsigned char IsPointToPointSel();
    unsigned char IsInheritingVelocity();
    unsigned char IsRandom();
#ifdef _XBOX
    unsigned char IsSplat();
    unsigned char IsDepthTexture();
#endif

#ifdef _XBOX
    unsigned char IsInheriting();
#else
    unsigned char IsInheriting() { return (unsigned char)m_EmitterFlags.m_bInheriting; }
#endif
    unsigned char IsBouncing();
#ifdef _XBOX
    unsigned char IsInheritingLocal();
#else
    unsigned char IsInheritingLocal() { return (unsigned char)m_EmitterFlags.m_bInheritingLocal; }
#endif
#ifdef _XBOX
    unsigned char IsInheritingPart();
#else
    unsigned char IsInheritingPart() { return (unsigned char)m_EmitterFlags.m_bInheritingPart; }
#endif

#ifdef _XBOX
    unsigned char IsRandomOrder();
#else
    unsigned char IsRandomOrder() { return (unsigned char)m_EmitterFlags.m_bRandomOrder; }
#endif

    void EnablePointToPoint();
    void EnablePointToPointSel();
    void EnableAffectedByWind();
    void EnableTinted();
    void EnableBouncing();
    void EnableRandom();
    void EnableRandomOrder();
    void EnableInheriting();
    void EnableInheritingVelocity();
    void EnableInheritingLocal();
    void EnableSplat();
    void EnableInheritingPart();
    void EnableDepthTexture();
    void InternalPostProcess();
    void InternalControl(Part* pPart, float fTime, float fPreviousTime,
                         float fBlend);

    float m_fDeadSpace;
    float m_fBlastRadius;
    float m_fBlastLength;
    int m_nNumBranches;
    int m_nControlPointSmoothing;
    int m_nXGrid;
    int m_nYGrid;
    int m_nSpawnType;
    char m_sUpdate[32];
    char m_sRender[32];
    char m_sBlend[32];
    char m_sTexture[32];
    char m_sChunkName[16];
    int m_nTwoSidedTex;
    int m_nLoop;
    short m_nRenderOrder;
    bool m_bFrameBlending;
    char m_sDepthTextureName[33];

    struct EmitterFlags
    {
        unsigned long m_bPointToPoint : 1;
        unsigned long m_bPointToPointSel : 1;
        unsigned long m_bAffectedByWind : 1;
        unsigned long m_bTinted : 1;
        unsigned long m_bBouncing : 1;
        unsigned long m_bRandom : 1;
        unsigned long m_bInheriting : 1;
        unsigned long m_bInheritingVelocity : 1;
        unsigned long m_bInheritingLocal : 1;
        unsigned long m_bSplat : 1;
        unsigned long m_bInheritingPart : 1;
        unsigned long m_bDepthTexture : 1;
        unsigned long m_bRandomOrder : 1;
        unsigned long m_nFlagsUnused : 19;
    } m_EmitterFlags;
};

class PartEmitter;
class Particle;
class ChunkyParticle;

class Particle
{
public:
    Particle(PartEmitter* pEmitter, float fTime);
    virtual ~Particle();
    // PC 0x004065A0
    virtual ChunkyParticle* AsChunkyParticle() { return 0; }
    virtual void initialize(float fTime, float fScale);
    void randomPosition(int nWidth, int nHeight);
    void randomDirection(int nWidth, int nHeight);
    void inheritVelocity(float fTime);

    PartEmitter* m_pEmitter;
    Vector m_vPosition;

    float m_fRotation;
    float m_fSpeed;
    Vector m_vVelocity;
    char m_abReserved28[4];
    Quaternion m_qOrientation;
    int m_nFrame;
    float m_fFrameStartAge;
    float m_fFrameAge;
    Vector m_vHeading;
    Vector m_vDrift;
    Vector m_vOrigin;
    char m_bReserved6C;
    float m_fAge;
};

class ChunkyParticle : public Particle
{
public:
    ChunkyParticle(PartEmitter* pEmitter, float fTime);
    virtual ~ChunkyParticle();
    virtual ChunkyParticle* AsChunkyParticle() { return this; }  // PC 0x00698BD0
    virtual void initialize(float fTime, float fScale);

    Gob* m_pGob;
};

#define EMITTER_MAX_TIME_STEP 0.1f

class PartEmitter : public Part
{
public:
    PartEmitter(MdlNode* pNode);
    virtual ~PartEmitter();
    virtual void Initialize();

    virtual void Update(float fTimeStep) = 0;
    // PC 0x004065A0
    virtual LightningEmitter* AsLightningEmitter() { return 0; }
    virtual PartEmitter* AsPartEmitter() { return this; }  // PC 0x00698BD0
    void AnimateParticles(float fTimeStep);
    void RenderFunc();
    void chkRandom(Particle* pParticle);
    void chkParticleLife(float fTimeStep);
    void updateParticleFrame(float fTimeStep);
    void RenderBoundingSphere();
    void RenderNothing();
    void RenderStandard();
    void RenderMotionBlurred();
    void RenderLinked();
    void MoveFunc(float fTimeStep);

    void moveParticle(Particle* pParticle, float fTimeStep, int bInheriting);
    void moveParticleStandard(float fTimeStep);
    void moveParticleP2PBezier(float fTimeStep);
    void moveParticleP2PGrav(float fTimeStep);

    float GetRadius();
    int GetNeighborFrame(int nFrame);
#ifdef _XBOX
    int GetNextFrame(int nFrame);
    float GetObjectAlpha();
#endif

    static unsigned int particlePoolToken;

    static void StartEmitterLayer();
    static void StopEmitterLayer();
    virtual void Animate(float fTimeStep);

    enum BlendMode
    {
        BLEND_NORMAL = 0,
        BLEND_PUNCH_THROUGH = 1,
        BLEND_OTHER = 2
    };
    int m_nBlendMode;

    float m_fAlphaEnd;
    float m_fAlphaStart;

    float m_fBirthRate;

    float m_fBounceCoefficient;
    float m_fCombineTime;
    float m_fDrag;
    float m_fFramesPerSecond;
    float m_fFrameEnd;
    float m_fFrameStart;

    float m_fGravity;
    float m_fLifeTime;

    float m_fMass;
    float m_fPointToPointBezier2;
    float m_fPointToPointBezier3;
    float m_fParticleRotation;
    float m_fVelocityRandom;
    float m_fXSizeStart;
    float m_fXSizeEnd;
    float m_fYSizeStart;
    float m_fYSizeEnd;
    float m_fSpread;
    float m_fThreshold;
    float m_fVelocity;
    float m_fXSize;
    float m_fYSize;

    float m_fBlurLength;

    float m_fLightningDelay;
    float m_fLightningRadius;

    float m_fLightningScale;
    float m_fLightningSubDivisions;

    float m_fLightningZigZag;

    float m_fBirthElapsed;
    float m_fRadius;
    float m_fGravityAcceleration;
    float m_fAlphaMid;
    float m_fPercentStart;
    float m_fPercentMid;
    float m_fPercentEnd;
    float m_fXSizeMid;
    float m_fYSizeMid;

    float m_fBirthRateRandom;

    float m_fReservedF4;
    float m_fReservedF8;
    float m_fTargetSize;
    float m_fNumControlPoints;
    float m_fControlPointRadius;

    float m_fControlPointDelay;
    float m_fTangentSpread;
    float m_fTangentLength;
    int m_nReserved114;
    char m_abReserved118[4];

    Vector m_vColorMid;
    Vector m_vXAxis;
    Vector m_vYAxis;
    Vector m_vDirection;
    Vector m_vReserved14C;
    Vector m_vReserved158;
    Vector m_vOrigin;
    Vector GetOrigin();
    Vector m_vLastOrigin;
    void SetLastOrigin(Vector v);
    Vector m_vColorEnd;

    Vector m_vColorStart;
    Quaternion m_qReserved194;
    Quaternion m_qOrientation;
    Quaternion m_qWorldOrientation;

    SafePointer<WindManager> m_pWindManager;
    List<Particle*> m_lstActive;
    List<Particle*> m_lstFree;
    Material* m_pEmitterMaterial;
    SafePointer<CAurObject> m_pTarget;
    CHitInfo* m_pHitInfo;
    TileData* m_pTileData;
    CallBack* m_pEmitterTargetCallback;
    short m_pReserved1F4;
    char m_bDetonated;
    char m_pReserved1F7;

    void RecursiveFractal(int nIndex, int nStep, int nCount,
                          Vector vDirection, Vector vOffset, float fRange);
    void CalculateRadius();
    float GetTileAlpha();
};

class CAurObject;

void Detonate(CAurObject* pObject, char* sEvent, void* pData);
void SetEmitterTarget(CAurObject* pObject, char* sEvent, void* pData);

class FountainEmitter : public PartEmitter
{
public:
#ifdef _XBOX
    FountainEmitter(MdlNode* pNode);
#else
    FountainEmitter(MdlNode* pNode) : PartEmitter(pNode) {}
#endif
    virtual void Update(float);
};

class ExplosionEmitter : public PartEmitter
{
public:
#ifdef _XBOX
    ExplosionEmitter(MdlNode* pNode);
#else
    ExplosionEmitter(MdlNode* pNode) : PartEmitter(pNode) {}
#endif
    virtual void Update(float);
};

class LightningEmitter : public PartEmitter
{
public:

    Vector m_vReserved1F8;

    void SetReserved1F8(Vector v);
    Vector m_vReserved204;
    List<Vector> m_lstReserved210;
    List<Vector> m_lstReserved21C;
    List<Vector> m_lstReserved228;
    List<Vector> m_lstReserved234;
    List<Vector> m_lstReserved240;
    List<Vector> m_lstReserved24C;
    List<LightningEmitter*> m_lstBranches;
    List<int> m_lstBranchIndices;

    List<Vector> m_lstReserved270;
    Vector m_vReserved27C;
    int m_nBranches;
    char m_bBranch;
    char gap_28D[3];
    virtual LightningEmitter* AsLightningEmitter() { return this; }  // PC 0x00698BD0
    virtual void UpdateControlPoints();

    virtual void UpdateBranches();
    virtual void Update(float);
    void InitializeBranch();
    LightningEmitter(MdlNode*);
    ~LightningEmitter();
    virtual void Initialize();
};

class SingleEmitter : public PartEmitter
{
public:
#ifdef _XBOX
    SingleEmitter(MdlNode* pNode);
#else
    SingleEmitter(MdlNode* pNode) : PartEmitter(pNode) {}
#endif
    virtual void Update(float);
};

#endif
