#ifndef AURMODEL_H
#define AURMODEL_H

#include <stdio.h>

#include "../math3d/math3d.h"

class CAurPathInfo
{
public:
    enum { COORDINATES_PER_POINT = 3 };

    int m_nCoordinates;
    float* m_pfCoordinates;

    virtual ~CAurPathInfo();
    CAurPathInfo();
};

#include "math3d.h"
#include <aurlist.h>
#include <aurpointer.h>
#include "aurora.h"

class CAurScene;
class CAurPart;
class CAurBehavior;
class CAurCamera;
class Camera;
class StringGob;
class Scene;
class CHitInfo;
class Material;
class Part;
class Plane;
class CAurTexture;

void GetAllNamedTextures(Part* pPart, List<CAurTexture*>& lTextures, char* sName);

class GobModel
{
public:
    char m_abReserved00[8];
    char* m_sName;
    char m_abReserved0C[68];

    unsigned short m_bRenderFirst : 1;
    unsigned short : 1;
    unsigned short m_bCharacter : 1;
    unsigned short m_bDoor : 1;
    unsigned short m_bRenderEarly : 1;
    unsigned short : 11;
};

class CAurObject;
class CAurPart;
class CAurCamera;
class GobModel;
class Model;
class TileData;
class LookAtAnimation;
class AnimRun;
class OOAnimRun;
class CallBack;
class PartLight;
template <class T> class List;
class CAurObjectLightContainer;
class nodedata;

#define AUR_CALLBACK_LONG_DELAY 9999.0f

#define AUR_PERSONAL_SPACE_ORIGIN_RADIUS 0.001f

class Gob : public CAurObject
{
public:

    enum AttachmentMode
    {
        ATTACH_NORMAL = 0,
        ATTACH_TRANSLATE = 1,
        ATTACH_ROTATE = 2,
        ATTACH_NO_ORIENTATION = 3
    };

    enum { EXTERNAL_DATA_TYPE_CLIENT_ITEM = 6 };

    enum { LIGHT_PRIORITY_VISUAL_EFFECT = 3 };

    enum { LIGHT_PRIORITY_PLAYER_WEAPON = 2, LIGHT_PRIORITY_OTHER_WEAPON = 4 };

    static const int ANIMATION_FLAG_STOP = 0x80;

    static const int ANIMATION_FLAG_NO_LOOP = 1;

    static const int ANIMATION_FLAG_OUT_OF_PLACE = 2;

    static const int ANIMATION_FLAG_QUEUE = 0x20;

    static const int ANIMATION_FLAG_NO_BLEND = 2;
    static const int ANIMATION_FLAG_TRACK_PLAYBACK = 4;
    static const int ANIMATION_FLAG_REPORT_KEYS = 256;

    static const int ANIMATION_FLAG_STOP_OUT_OF_ORDER = 1024;

    static const int ANIMATION_FLAG_SYNCHRONIZE = 0x10;
    static const int ANIMATION_FLAG_RESTART = 0x40;

    enum { ANIMATION_LENGTH_NOT_FOUND = -1 };

    Gob(char* sName);
    virtual ~Gob();
    void Reset();
    void RenderBoundingBox();
    void CheckEvent(float fTime, bool bForce);
    void SetAnimationTime(char* sName, float fTime);
    void SetAnimationPercentage(char* sName, float fPercentage);
    int ShouldWeCheckVisibilityHACK();
    static int cm_nLastGobIndex;
    // PC 0x004065A0
    virtual CAurCamera* AsCAurCamera() { return 0; }
    virtual Camera* AsCamera() { return 0; }  // PC 0x004065A0
    virtual CAurObject* AsCAurObject() { return this; }  // PC 0x00698BD0
    virtual StringGob* AsString() { return 0; }  // PC 0x004065A0
    virtual char* GetName()  // PC 0x0045DCB0
    {
        return m_sName;
    }
    virtual char* GetModelName();
    virtual void Render(bool bRenderLast);
    virtual void Animate(float fTimeStep);
    virtual int PlayAnimation(char* sName, float fSpeed, int nFlags,
                              float fBlend);
    virtual OOAnimRun* GetAnimation(char* sName, float* pfTime, float* pfLength);
    virtual int LoadAddInAnimations(char* sName);
    virtual void RemoveAddInAnimations();
    virtual CallBack* RegisterCallBack(void (*pCallBack)(CAurObject*, char*, void*),
                                 char* sEvent, void* pData, float fDelay,
                                 bool bRepeat);
    virtual void DeregisterCallback(CallBack* pCallBack);
    virtual int ReplaceTexture(char* sOld, char* sNew);
    virtual int ReplaceTexture(char* sOld, char* sNew, int nCount,
                                unsigned short* pParts, int nStart);
    virtual int ReplaceTextureSubtree(char* sPart, char* sTexture, int nNumTXI,
                                      unsigned short* pTXI);
    virtual int RestoreTexture();
    virtual int SetCanDownSample(char bCanDownSample);
    virtual int SetTextureAttributes(char* sTexture, char* sAttributes);
    virtual int SetEnvironmentMap(char* sName);
    virtual int AttachToScene(CAurScene* pScene);
    virtual CAurBehavior* AttachToObject(CAurObject* pObject, char* sPart, int nFlags);
    virtual CAurBehavior* AttachToPart(CAurObject* pObject, CAurPart* pPart, int nFlags);
    virtual void SetBehavior(CAurBehavior* pBehavior);
    virtual Vector SetPosition(Vector vPosition);
    virtual Quaternion SetOrientation(Quaternion qOrientation);
    virtual Vector GetPosition()  // PC 0x0045DCC0
    {
        return m_vPosition;
    }
    virtual Quaternion GetOrientation()  // PC 0x0045DCF0
    {
        return m_qOrientation;
    }
    virtual Vector GetPreviousPosition()  // PC 0x0045DD20
    {
        return m_vPreviousPosition;
    }
    // PC 0x0045DD50
    virtual void* GetExternalData()
    {
        return m_pExternalData;
    }
    // PC 0x0045DE40
    virtual void SetExternalData(void* pData)
    {
        m_pExternalData = pData;
    }
    // PC 0x0045DD70
    virtual int GetExternalDataType()
    {
        return m_nExternalDataType;
    }
    // PC 0x0045DD80
    virtual void SetExternalDataType(int nType)
    {
        m_nExternalDataType = nType;
    }

    virtual void InheritExternal(CAurObject* pSource);
    virtual CAurObject* HitCheckHack(Vector vStart, Vector vEnd, Vector* pHit);
    virtual CAurObject* HitCheckPart(CAurPart* pPart, Vector vStart, Vector vEnd,
                             Vector* pHit, CHitInfo* pInfo);
    virtual CAurObject* HitCheck(Vector vStart, Vector vEnd, Vector* pHit,
                                CHitInfo* pInfo);
    // PC 0x0045DD90
    virtual void SetIgnoreHitCheck(bool bIgnore)
    {
        m_bIgnoreHitCheck = bIgnore;
    }

    virtual int SetPartPosition(char* sPart, Vector vPosition,
                                 Quaternion qOrientation);
    virtual int GetPartPosition(char* sPart, Vector* pPosition,
                                Quaternion* pOrientation);
    virtual int SetPartLocalPosition(char* sPart, Vector vPosition,
                                     Quaternion qOrientation);
    virtual int GetPartLocalPosition(char* sPart, Vector* pPosition,
                                     Quaternion* pOrientation);
    virtual void ReattachReference(CAurObject* pObject, char* sPart);
    virtual void BehaviorMessage(char* sMessage);
    virtual int Append(char* sModel, char* sPart);
    virtual int Amputate(char* sPart);
    virtual void SetColorShifting(Vector vColor, float fAlpha, int nFlags);
    virtual void SetIllumination(Vector vColor, int nFlags);
    // PC 0x0045DDA0
    virtual void TurnOffShadows()
    {
        m_bCastShadows = 0;
    }
    // PC 0x0045DDB0
    virtual void TurnOnShadows()
    {
        m_bCastShadows = 1;
    }

    virtual void DoEvent(char* sEvent, void* pData);
    virtual bool IsForceAnimated();
    // PC 0x0045DDC0
    virtual bool IsAnimating()
    {
        return m_bAnimating;
    }

    virtual void EnableInViewVolume();
    virtual void DisableInViewVolume();
    // PC 0x0045DDE0
    virtual bool IsInViewVolume()
    {
        return m_bInViewVolume;
    }

    virtual void SetLightPriority(int nPriority);
    virtual float GetMaximumLightRadius();
    virtual bool SetObjectScale(float fScale, bool bBaseScale);
    virtual void EnableRenderGobBBox(Vector const& vBoundingBox);
    virtual void DisableRenderGobBBox();
    virtual void SetWind(bool bWind, float fWindY, float fWindX, float fWindZ);
    virtual void EnableForceAnimations();
    virtual void DisableForceAnimations();
    virtual void DisableFog();
    virtual void EnableFog();
    virtual void GetBoundingBox(Vector& vMinimum, Vector& vMaximum);
    virtual void GetMinimumBoundingSphere(Vector& vPosition, float& fRadius);

    bool IsCharacter();

    bool IsRenderEarly();

    bool IsAllRooms();
    bool IsSceneHitExcluded();
    bool HasHitFilterOverride();
    bool IsVisibilityCheckDisabled();
    // PC 0x0045DDF0
    virtual void SetAsCharacter()
    {
        if (m_pModel)
        {
            ((GobModel*)m_pModel)->m_bCharacter = 1;
        }
    }
    // PC 0x00692D70
    virtual CAurScene* GetScene()
    {
        return (CAurScene*)m_pScene;
    }
    virtual CAurBehavior* GetBehavior(int nIndex);
    virtual Part* GetPart(char* sName);
    virtual void EnumParts(int (*pFilter)(CAurPart*), List<CAurPart*>* pParts);
    void EnumParts(Part* pPart, int (*pFilter)(CAurPart*), List<CAurPart*>* pParts);
    // PC 0x0045DE10
    virtual void SetRenderPersonalSpace(bool bRender)
    {
        m_bRenderPersonalSpace = bRender;
    }

    virtual void BeginLookAtAnimate(CAurObject* pTarget, char* sTargetPart,
                                    char* sHeadPart, float fHorizontalLimit,
                                    float fVerticalLimit, float fDuration,
                                    float fMaxDelta);
    virtual void EndLookAtAnimate();
    virtual void PlayOutOfOrderAnimation(char* sName, float* pfWeights,
                                         float* pfTimes, int nCount,
                                         float fLength);
    virtual int StopOutOfOrderAnimation(char* sName);
    virtual void AddAnimationFlag(int nFlag);
    virtual void RemoveAnimationFlag(int nFlag);
    virtual void EnableAlwaysRender();
    virtual void DisableAlwaysRender();
    virtual void EnableInCutscene();
    virtual void DisableInCutscene();
    virtual Vector GetCutsceneDummyPosition();
    // PC 0x0045DE20
    virtual void SetMotionBlurred(bool bBlurred)
    {
        m_bMotionBlurred = bBlurred;
    }
    // PC 0x0045DE30
    virtual bool GetMotionBlurred()
    {
        return m_bMotionBlurred;
    }

    virtual void RenderBlur();
    virtual void EnableRenderBumpedOut(char* sTexture, float fHeight);
    virtual void DisableRenderBumpedOut();

    virtual void SetMaxBlurLength(float fLength)
    {
        m_fMaxBlurLength = fLength;
    }
    // PC 0x0045DE50
    virtual float GetMaxBlurLength()
    {
        return m_fMaxBlurLength;
    }
    // PC 0x0045DE60
    virtual void EnableDistortion()
    {
        m_bDistortion = 1;
    }
    // PC 0x0045DE70
    virtual void DisableDistortion()
    {
        m_bDistortion = 0;
    }
    bool IsDistortionEnabled();
    // PC 0x0045DE80
    virtual void SetForcePoints(short nForcePoints)
    {
        m_nForcePoints = nForcePoints;
    }
    // PC 0x0045DE90
    virtual short GetForcePoints()
    {
        return m_nForcePoints;
    }
    // PC 0x0045DEA0
    virtual void SetAlignmentValue(short nAlignmentValue)
    {
        m_nAlignmentValue = nAlignmentValue;
    }
    // PC 0x0045DEB0
    virtual short GetAlignmentValue()
    {
        return m_nAlignmentValue;
    }
    // PC 0x0045DEC0
    virtual void SetForceGlowColor(float fRed, float fGreen, float fBlue, float fAlpha)
    {
        m_fForceGlowRed = fRed;
        m_fForceGlowGreen = fGreen;
        m_fForceGlowBlue = fBlue;
        m_fForceGlowAlpha = fAlpha;
    }
    // PC 0x0045DEE0
    virtual void GetForceGlowColor(float& fRed, float& fGreen, float& fBlue, float& fAlpha)
    {
        fRed = m_fForceGlowRed;
        fGreen = m_fForceGlowGreen;
        fBlue = m_fForceGlowBlue;
        fAlpha = m_fForceGlowAlpha;
    }
    // PC 0x0045DF10
    virtual void EnableForceGlow(bool bEnable)
    {
        m_bForceGlow = bEnable;
    }
    // PC 0x0045DF20
    virtual bool IsForceGlowEnabled()
    {
        return m_bForceGlow;
    }
    // PC 0x0045DF30
    virtual void SetNonForceBlending(bool bNonForceBlending)
    {
        m_bNonForceBlending = bNonForceBlending;
    }
    // PC 0x0045DF40
    virtual bool IsNonForceBlending()
    {
        return m_bNonForceBlending;
    }
    // PC 0x0045DF50
    virtual bool IsHologramEnabled()
    {
        return m_bHologram;
    }
    // PC 0x0045DF60
    virtual bool IsIlluminationEnable()
    {
        return m_bPerObjectIllumination;
    }
    // PC 0x0045DF70
    virtual void GetIlluminationColor(float& fRed, float& fGreen, float& fBlue)
    {
        fRed = m_fIlluminationRed;
        fGreen = m_fIlluminationGreen;
        fBlue = m_fIlluminationBlue;
    }
    // PC 0x0045DF90
    virtual void EnableRendering(bool bEnable)
    {
        m_bRenderingEnabled = bEnable;
    }
    // PC 0x0045DFA0
    virtual bool IsRenderingEnabled()
    {
        return m_bRenderingEnabled;
    }
    // PC 0x0045DFB0
    virtual void SetEmitterAsWeather(bool bWeather)
    {
        SetAllEmitterAsWeather(m_pRootPart, bWeather);
    }
    // PC 0x0045DFD0
    virtual void EnableHologram(bool bEnable)
    {
        EnableHologramRendering(m_pRootPart, bEnable);
    }
    // PC 0x0045DFF0
    virtual void EnableIllumination(bool bEnable, float fRed, float fGreen, float fBlue)
    {
        m_fIlluminationRed = fRed;
        m_fIlluminationGreen = fGreen;
        m_fIlluminationBlue = fBlue;
        EnablePerObjectIllumination(m_pRootPart, bEnable);
    }
    void SetAllEmitterAsWeather(Part* pPart, bool bWeather);
    void EnableHologramRendering(Part* pPart, bool bEnable);
    void EnablePerObjectIllumination(Part* pPart, bool bEnable);

    virtual void GetMinimumBoundingBox(Vector& vMinimum, Vector& vMaximum);
    virtual void GetMinimumBoundingBox();
    virtual void GetMinimumSphere();
    virtual void SetUpImposters(int nImposterType, float* pfImposterValues);
    virtual bool IsAlwaysRendering();

    bool IsInCutscene();
    void SetScene(Scene* pScene);
#ifdef _XBOX
    void ResetAnimationScales();
#endif
    void CollectLights(List<PartLight*>& lstLights);
    void SetProcessFlag();
    void ProcessGob();

    static const int TEXTURE_SCHEME_BUFFER_SIZE = 256;

    enum { MAX_ANIMATION_DEPTH = 100 };
    void AddAttachment(Gob* pAttachment);
    void RemoveAttachment(Gob* pAttachment);
#ifndef _XBOX
    bool VisibilityCheck();
#endif
    void PartDraw(Part* pPart, bool bRenderLast);
    void PartVolumeProject(Part* pPart, Vector vPosition, float fRadius,
                          Plane plane);
    void ProxyPartDraw(Part* pPart, bool bRenderLast);

public:
    bool m_bForceAnimated;
    bool m_bInViewVolume;
    bool m_bAlwaysRendering;

    char m_bRendered;
    bool m_bInCutscene;

    char m_bRenderBumpedOut;

    bool m_bHierarchyAnimated;

    bool m_bFirstAnimate;

#ifndef _XBOX
    bool m_bVisible;
    char m_abReserved19[3];
    float m_fVisibilityCheckTime;
#endif

    short m_nForcePoints;
    short m_nAlignmentValue;
    bool m_bNonForceBlending;
    bool m_bForceGlow;
    float m_fForceGlowRed;
    float m_fForceGlowGreen;
    float m_fForceGlowBlue;
    float m_fForceGlowAlpha;
    bool m_bHologram;
    bool m_bPerObjectIllumination;
    float m_fIlluminationRed;
    float m_fIlluminationGreen;
    float m_fIlluminationBlue;
    bool m_bRenderingEnabled;

    List<nodedata*> m_lstRoomData;

    float m_fCameraDistance;
    int m_nImposters;
    int m_nImposterType;
    char m_sName[32];
    Model* m_pModel;

    Model* m_pModelX;
    Model* m_pModelZ;
    Model* m_pAnimationModel;
    Scene* m_pScene;
    Part* m_pRootPart;

    Part* m_pRootPartX;
    Part* m_pRootPartZ;
    Vector m_vPosition;
    Quaternion m_qOrientation;

    Vector m_vPreviousPosition;
    Quaternion m_qPreviousOrientation;

    List<AnimRun*> m_lOutOfOrderAnimations;

    int m_bHasDynamicLights;
    char m_bCanDownSample;
    char m_bReservedC1;
    short m_bCastShadows;

    short m_bColorShifting;
    short m_bIllumination;
    short m_bRenderGobBBox;
    short m_nReservedCA;
    Vector m_vColor;
    float m_fAlpha;

    float m_fOcclusionAlpha;
    Vector m_vIllumination;

    List<Model*> m_lstAppendModels;
    List<Part*> m_lstAppendParts;

    float m_fZOrder;

    float m_fPersonalSpaceRadius;
    float m_fPersonalSpaceRadius2;

    CAurPathInfo m_PathInfo;

    float m_fBumpedOutShellHeight;
    Vector m_vBoundingBoxMinimum;
    Vector m_vBoundingBoxMaximum;
    float m_fMinimumBoundingSphereRadius;
    Vector m_vMinimumBoundingSphere;
    Vector m_vRenderGobBBox;
    bool m_bRenderPersonalSpace;
    bool m_bMotionBlurred;
    bool m_bWind;
    char m_bReserved157;
    float m_fWindX;
    float m_fWindY;
    float m_fWindZ;

    char m_bSingleTexture;

    char m_bUnused;
    char m_bHasGeometry;
    char m_bProcessFlag;
    Material* m_pSingleTexture;

    int m_nUnused1;
    TileData* m_pTileData;
    LookAtAnimation* m_pLookAtAnimation;

    CAurObjectLightContainer* m_pLightContainer;
    char m_bDistortion;
    bool m_bIgnoreHitCheck;
    bool m_bAnimating;

    char m_bHasTransparency;
    float m_fMaxBlurLength;

    char m_bAnimateUV;
    char m_abReserved185[3];
    CAurBehavior* m_pBehavior;

    int m_nLastAnimationFrame;
    List<CallBack*> m_lCallBacks;

    List<PartLight*> m_lstLights;

    SafePointer<CAurTexture> m_pEnvironmentMap;
    void* m_pExternalData;
    int m_nExternalDataType;
    float m_fScale;
    float m_fBaseScale;

    List<Gob*> m_lstAttachments;

#ifdef _XBOX

    int m_nVisiblePixels;
    bool IsVisibilityTested();
    void RenderVisibilityBounds();
    bool IsAlternateShadowCaster();
    bool IsEmitterBounds(); bool CheckEventCallBack(float fDelay, bool bRepeat);
#else
    int m_nUnused2;
#endif
    int m_n1F8;
    int m_n1FC;
};

CAurObject* NewCAurObject(char* sModel, char* sName, FILE* pModelFile, FILE* pVertexFile);

#include "aurmaterial.h"
#include "auremitter.h"

class Gob;
class MaxTree;
class MaxFace;
class MdlNodeTriMesh_PostProcess;
class Part;

class MaxFace_PostProcess;
class NodeVertex;

class MdlNodeTriMesh_PostProcess
{
public:

    List<Vector> m_lPositions;

    List<Vector> m_lTextureCoordinates0;
    List<Vector> m_lTextureCoordinates1;
    List<Vector> m_lTextureCoordinates2;
    List<Vector> m_lTextureCoordinates3;
    List<Vector> m_lColors;
    List<MaxFace_PostProcess> m_lFaces;
    List<Vector> m_lNormals;
    List<NodeVertex*> m_lstVertices;
    ~MdlNodeTriMesh_PostProcess();
    MdlNodeTriMesh_PostProcess();
};

class MdlNodeTriMesh : public MdlNode
{
public:
    static const int BASE_CAP;
    static const int NEIGHBOR_CAP;
    static const int FADEABLE;
    static const int DEFAULT_PROPERTIES;
    void (*m_pfnGenVertices)(void*);
    void (*m_pfnRemoveTemporaryArrays)(void*);
    List<MaxFace> m_lFaces;

    Vector m_vMinimum;
    Vector m_vMaximum;

    float m_fSphereRadius;
    Vector m_vSphereCenter;

    Vector m_vDiffuse;
    Vector m_vAmbient;

    int m_nTransparencyHint;

    enum { TEXTURE_NAME_LENGTH = 32 };
    char m_sTexture0[TEXTURE_NAME_LENGTH];
    char m_sTexture1[TEXTURE_NAME_LENGTH];

    List<unsigned int*> m_lstIndexArrays;

    List<unsigned int> m_lstTriangleIndices;

    List<int> m_lstIndexCounts;
    List<unsigned long> m_lstIndexOffsets;
    List<unsigned long> m_lstIndexPools;
    int m_nAuxIndexOffset;
    unsigned long m_nAuxIndexPool;
    unsigned long GetAuxIndexPool() { return m_nAuxIndexPool; }
    unsigned int m_nAuxIndexCount;

    int m_nPrimitiveType;
    MdlNodeTriMesh_PostProcess* m_pPostProcess;

    int m_bAnimateUV;

    float m_fUVDirectionX;
    float m_fUVDirectionY;
    float m_fUVJitter;
    float m_fUVJitterSpeed;

    int m_nVertexStride;

    enum VertexArrayFlags
    {
        VERTEX_ARRAY_POSITION = 0x01,
        VERTEX_ARRAY_TEXCOORD0 = 0x02,
        VERTEX_ARRAY_TEXCOORD1 = 0x04,
        VERTEX_ARRAY_TEXCOORD2 = 0x08,
        VERTEX_ARRAY_TEXCOORD3 = 0x10,
        VERTEX_ARRAY_NORMAL = 0x20,
        VERTEX_ARRAY_COLOR = 0x40,
        VERTEX_ARRAY_TANGENT_BASIS = 0x80
    };
    int m_nVertexArrayFlags;
    int m_nPositionOffset;
    int m_nNormalOffset;
    int m_nColorOffset;
    int m_nTexCoord0Offset;

    enum { NO_VERTEX_ARRAY_OFFSET = -1 };
    int m_nTexCoord1Offset;
    int m_nTexCoord2Offset;
    int m_nTexCoord3Offset;
    int m_TangentBases;

    int m_nUnusedVertexOffset1;
    int m_nUnusedVertexOffset2;
    int m_nUnusedVertexOffset3;
    short m_nVertexCount;

    short m_nTextureCount;

    bool m_bLightMapped;

    bool m_bRotateTexture;

    bool m_bBackground;

    bool m_bShadow;
    bool m_bBeaming;

    bool m_bRender;
    bool m_bDirtEnabled;
    short m_nDirtTexture;
    short m_nDirtCoordSpace;
    bool m_bHideInHolograms;

    float m_fSurfaceArea;
#ifdef _XBOX

    float m_fXboxUnmapped190;
#else

    int m_nUnusedWord;
#endif

    unsigned long m_nVertexDataOffset;
#ifndef _XBOX
    Vector* m_pVertexPositions;
#else
    void InternalDestructor();
#endif
    void BuildNeighborLists();
    void InternalGenVertices();
    void ProcessVerts();
    void CalculateTangentSpaceBasis();
    void InternalRemoveTemporaryArrays();
    void BuildVertexIndices();
    void InternalPostProcess();
    void ComputeLocalSurfaceArea(List<MaxFace>& lFaces, float& fArea);
    unsigned char HasTangentBases();
    PartTriMesh* InternalCreateInstance(Gob*);
    ~MdlNodeTriMesh(); MdlNodeTriMesh(char*);
    MdlNode* InternalConJoin();
    void InternalParseField(char*);
    void InternalControl(Part*, float, float, float);
};

#define MESH_UNUSED_VERTEX_COORDINATE 10000000.0f

class MdlNodeDanglyMesh : public MdlNodeTriMesh
{
public:
    List<float> m_lConstraints;
    float m_fDisplacement;
    float m_fTightness;
    float m_fPeriod;
    Vector* m_pVertexPositions;
#ifdef _XBOX
    void InternalDestructor();
#endif
    void InternalGenVertices();
    void InternalRemoveTemporaryArrays();
    void InternalPostProcess();
    Part* InternalCreateInstance(Gob*);
    ~MdlNodeDanglyMesh(); MdlNodeDanglyMesh(char*);
    void InternalParseField(char*);
};

class CHitInfo;
class GLRender;
class Gob;
class Animation;
class MdlNode;
class Model;
class NodeVertex;
class NodeVertexAnim;
class SkinVertexWeight;
class Part;
class Vector;
void MaxTreeDestructor(void* pTree);
int MaxTreeParseField(void* pTree, char* szField);
void ModelDestructor(void* pModel);
int ModelParseField(void* pModel, char* szField);
void MdlNodeTriMeshGenVertices(void* pNode);
void MdlNodeTriMeshRemoveTemporaryArrays(void* pNode);
void MdlNodeDanglyMeshGenVertices(void* pNode);
void MdlNodeDanglyMeshRemoveTemporaryArrays(void* pNode);
void MdlNodeSkinGenVertices(void* pNode);
void MdlNodeSkinRemoveTemporaryArrays(void* pNode);
void MdlNodeAnimMeshGenVertices(void* pNode);
void MdlNodeAnimMeshRemoveTemporaryArrays(void* pNode);
char TreeHasSingleTexture(Part* pPart);
char TreeHasGeometry(Part* pPart);
char TreeSetGeometryFlags(Part* pPart);
char TreeHasTransparency(Part* pPart);
#include "../math3d/math3d.h"

class AABB
{
public:

    enum
    {
        INTERNAL_NODE = -1,
        POSITIVE_X = 0, POSITIVE_Y = 1, POSITIVE_Z = 2,
        NEGATIVE_X = 3, NEGATIVE_Y = 4, NEGATIVE_Z = 5
    };

    AABB()
    {
        m_pRightChild = 0;
        m_pLeftChild = 0;
        m_nFaceIndex = INTERNAL_NODE;
        m_nSplitMask = 0;
    }

    Vector m_vMinimum;
    Vector m_vMaximum;
    AABB* m_pLeftChild;
    AABB* m_pRightChild;
    int m_nFaceIndex;
    int m_nSplitMask;
    ~AABB()
    {
        delete m_pLeftChild;
        delete m_pRightChild;
    }
};

#define AABB_BOUNDING_TOLERANCE 0.01f

void Parse(char* sText, AABB*& pAABB);
int HitCheckAABBnode(AABB* pNode, const Vector& vStart, Vector& vEnd, float fScale);

class MaxFace
{
public:

    enum { NO_ADJACENT_FACE = -1 };

    Vector m_vNormal;
    float m_fDistance;

    int m_nSurfaceType;

    short m_anAdjacentFaces[3];
    short m_anVertexIndices[3];
    MaxFace();
    ~MaxFace();
};

class MaxFace_PostProcess
{
public:
    static List<Vector> texindices1;
    static List<Vector> texindices2;
    static List<Vector> texindices3;

    enum { INVALID_NORMAL_INDEX = -1 };
    int* m_pFaces;
    int* m_pFaceIndices;

    int m_nFaces;
    int m_nFaceIndices;
    int* m_pFaceRemap;
    ~MaxFace_PostProcess();
    MaxFace_PostProcess();
};

class AurGenericKey
{
public:

    static const int AXIS_ANGLE_FIELD_COUNT = 5;

    static const int SINGLE_AXIS_ANGLE_FIELD_COUNT = 4;

    enum
    {
        COMPRESSED_QUATERNION_FIELD_COUNT = 2,
        QUATERNION_XY_BITS = 11,
        QUATERNION_XY_SCALE = 1023,
        QUATERNION_XY_MAX = 2046,
        QUATERNION_Z_SCALE = 511,
        QUATERNION_Z_MAX = 1022
    };
    float m_fTime;
    float m_afValues[9];
    int m_nValues;
};

template <class T> class AurKey
{
public:
    AurKey()
    {
        m_fTime = 0.0f;
    }

    float m_fTime;
    T m_Value;
};

class MaxTree
{
public:

    enum { TYPE_MASK = 0x7f, BINARY_TYPE_FLAG = 0x80 };
    void (*m_pfnDestructor)(void*);
    int (*m_pfnParseField)(void*, char*);

    char m_sName[32];
    MdlNode* m_pRootNode;

    int m_nNodeCount;

    List<MdlNode*> m_lFreeNodes;
    List<unsigned long> m_lFreeTokens;

    void* m_pModelResource;
    unsigned char m_nType;
    void SynchronizeTree(MaxTree*);
    int GetType();
    static void* operator new(size_t);
    Model* AsModel();
    Animation* AsAnimation();
    ~MaxTree();
    void InternalDestructor();
    void AddToFreeTokenList(unsigned long);
    void AddToFreeList(MdlNode*);
    void ResetFreeLists();
    MaxTree(char*);
    void FreeTheFreeList();
    void foobar(void*);
    int InternalParseField(char*);
    static const char MAXTREE_BASE_ID;
    static const char MAXTREE_MODEL_ID;
    static const char MAXTREE_ANIMATION_ID;
};

class Model : public MaxTree
{
public:

    static const char SCALABLE_MODEL;
    static const char NON_SCALABLE_MODEL;

    Model(char* sName);
    ~Model();
    int InternalParseField(char* szField);
    int AddRef(Gob* pGob);
    int AddRef(Model* pModel);
    int DelRef(Gob* pGob);
    int DelRef(Model* pModel);
    void InternalDestructor();

    struct Classification
    {
        unsigned short m_bEmitterBounds : 1;
        unsigned short m_bSceneHitDisabled : 1;
        unsigned short m_bCharacter : 1;
        unsigned short m_bDoor : 1;
        unsigned short m_bRenderEarly : 1;
        unsigned short m_bAlternateShadowCaster : 1;

        unsigned short m_bSkipVisibilityCheck : 1;
        unsigned short m_bSceneHitExcluded : 1;
        unsigned short m_bAllRooms : 1;

        unsigned short m_bNoQuaternionCompression : 1;
        unsigned short m_bHitFilterOverride : 1;
        unsigned short : 5;
    } m_Classification;

    unsigned char m_nModelOptions;
    unsigned char m_bModelOptionEnabled;

    bool IsQuaternionCompressionDisabled() const
    {
        return m_Classification.m_bNoQuaternionCompression;
    }
    int m_nReferenceCount;
    List<Animation*> m_lAnimations;
    Model* m_pParentModel;

    Vector m_vBoundingMin;
    Vector m_vBoundingMax;
    float m_fRadius;
    float m_fAnimationScale;

    char m_sSuperModel[32];

    void* m_pBinaryData;
    unsigned long m_nVertexPool;
    unsigned long GetVertexPool() { return m_nVertexPool; }
    int m_nVertexDataSize;

    void* m_pVertexResource;
    List<char*> m_lNames;
};

inline bool Gob::IsAllRooms()
{
    if (m_pModel)
    {
        char bFlag = m_pModel->m_Classification.m_bAllRooms;
        return bFlag;
    }
    return false;
}
#ifdef _XBOX

inline bool Gob::IsAlternateShadowCaster()
{
    if (m_pModel)
    {
        char bFlag = m_pModel->m_Classification.m_bAlternateShadowCaster;
        return bFlag;
    }
    return false;
}

inline bool Gob::IsEmitterBounds()
{
    if (m_pModel)
    {
        char bFlag = m_pModel->m_Classification.m_bEmitterBounds;
        return bFlag;
    }
    return false;
}
#endif

inline bool Gob::IsSceneHitExcluded()
{
    if (m_pModel)
    {
        char bFlag = m_pModel->m_Classification.m_bSceneHitExcluded;
        return bFlag;
    }
    return false;
}

inline bool Gob::IsVisibilityCheckDisabled()
{
    if (m_pModel)
    {
        return m_pModel->m_Classification.m_bSkipVisibilityCheck;
    }
    return false;
}

inline bool Gob::HasHitFilterOverride()
{
    if (m_pModel)
    {
        char bFlag = m_pModel->m_Classification.m_bHitFilterOverride;
        return bFlag;
    }
    return false;
}

class MdlNodeAABB : public MdlNodeTriMesh
{
public:
    AABB* m_pAABBData; Part* InternalCreateInstance(Gob*);
    ~MdlNodeAABB(); MdlNodeAABB(char*);
    void InternalParseField(char*);
#ifdef _XBOX
    void InternalDestructor();
#endif
};
class MdlNodeAnimMesh : public MdlNodeTriMesh
{
public:
    float m_fSamplePeriod;

    List<Vector> m_lAnimationVectors1;
    List<Vector> m_lAnimationVectors2;
    List<Vector> m_lAnimationVectors3;
    void* m_pAnimationData1;
    void* m_pAnimationData2;
    int m_nAnimationDataCount1;
    int m_nAnimationDataCount2;
    void InternalGenVertices(); void InternalRemoveTemporaryArrays();
    Part* InternalCreateInstance(Gob*);
    ~MdlNodeAnimMesh(); MdlNodeAnimMesh(char*);
#ifdef _XBOX
    void InternalDestructor();
#endif
    MdlNode* InternalConJoin();
    void InternalParseField(char*);
    void InternalControl(Part*, float, float, float);
    void InternalPostProcess();
};

class MdlNodeCamera : public MdlNode
{
public:
    void InternalParseField(char* szField);
    Part* InternalCreateInstance(Gob*);
    MdlNodeCamera(char*);
    MdlNode* InternalConJoin();
    void InternalControl(Part*, float, float, float);
};

class MdlNodeLight : public MdlNode
{
public:

    enum { DEFAULT_LIGHT_PRIORITY = 5 };
    float m_fFlareRadius;
    List<SafePointer<CAurTexture> > m_lstTextures;
    List<float> m_lstFlareSizes;
    List<float> m_lstFlarePositions;
    List<Vector> m_lstFlareColorShifts;
    List<char*> m_lstTextureNames;
    int m_nPriority;
    int m_bAmbientOnly;
    int m_nDynamicType;
    int m_bAffectDynamic;
    int m_bShadow;
    int m_bGenerateFlare;
    int m_bFadingLight;
    void InternalPostProcess();
    Part* InternalCreateInstance(Gob*);
    void InternalDestructor();
    ~MdlNodeLight();
    MdlNodeLight(char*);
    MdlNode* InternalConJoin();
    void BinaryInternalDestructor();
    void InternalParseField(char*);
    void InternalControl(Part*, float, float, float);
};

class MdlNodeLightsaber : public MdlNodeTriMesh
{
public:
    void InternalParseField(char* szField);

    Vector* m_pVertexPositions;
    float* m_pTextureCoordinates;
    Vector* m_pVertexNormals;

    enum { BLADE_POOL_FLAGS = 0x1012, GLOW_POOL_FLAGS = 0x1010 };
    unsigned long m_nBladePool;
    unsigned long m_nGlowPool;
    void InternalDestructor();
    void InternalPostProcess();
    Part* InternalCreateInstance(Gob*);
    ~MdlNodeLightsaber();
    MdlNodeLightsaber(char*);

    static const short NUM_FRONT_SEGMENTS;
    static const short NUM_BACK_SEGMENTS;
    static const short NUM_PIECE_VERTS;
    static const short NUM_SEGMENT_VERTS;
};

class MdlNodeReference : public MdlNode
{
public:
    char m_sRefModel[32];
    int m_bReattachable;
    Part* InternalCreateInstance(Gob*);
    MdlNodeReference(char*);
    MdlNode* InternalConJoin();
    void InternalParseField(char*);
    void InternalControl(Part*, float, float, float);
};

class MdlNodeSkin : public MdlNodeTriMesh
{
public:

    enum { UNMAPPED_BONE = -1 };

    List<SkinVertexWeight> m_lstSkinWeights;

    int m_nSkinWeightOffset;
    int m_nBoneIndexOffset;

#ifdef _XBOX
    void InternalDestructor();
    short* m_pfBoneMap;
#else
    float* m_pfBoneMap;
#endif
    int m_nBoneMap;

    List<Quaternion> m_lstQBoneRefInv;
    List<Vector> m_lstTBoneRefInv;
    List<int> m_lstBoneConstantIndices;

    short m_anBoneRemap[18];
    void InternalPostProcess();
    ~MdlNodeSkin();
    MdlNodeSkin(char*);
    MdlNode* InternalConJoin();
    void InternalRemoveTemporaryArrays();
    void InternalParseField(char*);
    Part* InternalCreateInstance(Gob*);
    void InternalGenVertices();
};

class PartReference : public Part
{
public:
    PartReference(MdlNode* pNode) : Part(pNode), m_pReferenceObject(0) {}
    virtual ~PartReference()  // PC 0x0045BAB0
    {
        delete m_pReferenceObject;
    }
    virtual PartReference* AsPartReference() { return this; }  // PC 0x00698BD0
    virtual PartReference* AsPartDummy() { return 0; }  // PC 0x004065A0

    Gob* m_pReferenceObject;

    int m_nReserved50;
};

class PartSkin : public PartTriMesh
{
public:
    PartSkin(MdlNode* pNode) : PartTriMesh(pNode) {}
    ~PartSkin();
    virtual PartSkin* AsPartSkin() { return this; }  // PC 0x00698BD0
    virtual Part* AsPartDummy() { return 0; }  // PC 0x004065A0
    virtual void Project(const Vector&, float, const Plane&, CAurShadowData*);
    virtual void PostProcess(Part*);

    void DrawBumpedOutShellVP();
    void DrawBumpedOutShell();

    enum
    {
        SKIN_VP_SHELL_HEIGHT = 12,

        SKIN_VP_BLUR_DIRECTION = 12,
        SKIN_VP_BLUR_REVERSE = 13,
        SKIN_VP_BLUR_LIMITS = 14,
        SKIN_VP_BLUR_REFERENCE = 17,
        SKIN_VP_BLUR_SIDE = 69,
        SKIN_VP_CONSTANTS = 16,
        SKIN_VP_FOG = 17,
        SKIN_VP_BONES = 18
    };
    virtual void Draw(bool);
};

class PartAnimMesh : public PartTriMesh
{
public:
    PartAnimMesh(MdlNode* pNode) : PartTriMesh(pNode) {}
    virtual PartAnimMesh* AsPartAnimMesh() { return this; }  // PC 0x00698BD0
    virtual Part* AsPartDummy() { return 0; }  // PC 0x004065A0
    virtual int HitCheckGeom(Vector const&, Vector&, Vector*, CHitInfo*, float);
    virtual void Draw(bool);
};

class PartAABB : public PartTriMesh
{
public:
    PartAABB(MdlNode* pNode) : PartTriMesh(pNode) {}
    virtual PartAABB* AsPartAABB() { return this; }  // PC 0x00698BD0
    virtual Part* AsPartDummy() { return 0; }  // PC 0x004065A0
    virtual int HitCheckGeom(Vector const&, Vector&, Vector*, CHitInfo*, float);
    virtual void Draw(bool);
};

#define DANGLY_RESET_DISTANCE 5.0f
#define DANGLY_MAX_COORDINATE_DELTA 1.0e10f
#define DANGLY_MAX_CONSTRAINT 255.0f

class PartDanglyMesh : public PartTriMesh
{
public:

    Vector m_vLastPosition;
    Quaternion m_qLastOrientation;

    Vector* m_pVertexPositions;
#ifndef _XBOX
    unsigned long m_nVertexPool;
#endif
    ~PartDanglyMesh();
    PartDanglyMesh(MdlNode*);
    virtual void Draw(bool);
    virtual void Animate(float);
    virtual PartDanglyMesh* AsPartDanglyMesh() { return this; }  // PC 0x00698BD0
    virtual Part* AsPartDummy() { return 0; }  // PC 0x004065A0
};

class PartLight : public Part
{
public:

    enum { LIGHTTYPE_STATIC = 0, LIGHTTYPE_DYNAMIC = 1, LIGHTTYPE_SECONDARY = 2 };
    Vector m_vColor;
    float m_fRadius;
    int m_bShadow;
    float m_fShadowRadius;

    float m_fShadowHeight;
    int m_bAmbientOnly;
    int m_bFlare;
    int m_nLightType;
    int m_nPriority;
    float m_fMultiplier;

    float m_fDynamicMultiplier;
    float m_fPriorityValue;

#ifdef _XBOX
    float m_fLightState;
#else
    int m_nLightState;
#endif

    float m_fFlareFade;

    float m_fColorMultiplier;

    float m_fShadowFade;
    Vector m_vWorldPosition;
    virtual ~PartLight();
    // PC 0x00698BD0
    virtual PartLight* AsPartLight() { return this; }
    virtual Part* AsPartDummy() { return 0; }  // PC 0x004065A0
    virtual void Draw(bool) {}  // PC 0x005EE1C0
    virtual void Mat();
    PartLight(MdlNode*);
};

class CAurObjectLightContainer
{
public:
    CAurObjectLightContainer();
    virtual ~CAurObjectLightContainer();
    List<PartLight*> m_lstLights;
    List<float> m_lstFadeIntensities;
    PartLight* m_pShadowLight;
    float m_fShadowFade;
};

class PartLightsaber : public PartTriMesh
{
public:

    enum { SEGMENT_CAPACITY = 25 };
    float m_afSegmentAge[SEGMENT_CAPACITY];
    Vector m_avSegmentPosition[SEGMENT_CAPACITY];
    Quaternion m_aqSegmentOrientation[SEGMENT_CAPACITY];
    short m_nNextSegment;
    short m_nFirstSegment;
    short m_nSegments;
    Vector m_vLastPosition;
    ~PartLightsaber();
    PartLightsaber(MdlNode*);
    virtual void Draw(bool);
    virtual void Animate(float);
    virtual Part* AsPartDummy() { return 0; }  // PC 0x004065A0
};

#define AUR_TILE_UNSET_BOUND 1000000.0f

class TileData
{
public:
    PartLight* m_pMainLight;
    PartLight* m_pSourceLight;
    Part* m_pFlamePart1;
    Part* m_pFlamePart2;
    Gob* m_pFlame1;
    Gob* m_pFlame2;
    List<Part*> m_lstParts;
    Gob* m_pStaticObject;

    Vector m_vPosition;

    int m_bSuppliesMaterial;

    Vector m_vMinimum;
    Vector m_vMaximum;

    float m_fHeightFadeAlpha;

    float m_fBaseHeightFadeAlpha;

    int m_nFadeControl;

    List<TileData*> m_lstAdjacentTiles;

    List<void*> m_lstReferences;

    List<PartTriMesh*> m_lstAdjacentReferences;

    List<PartTriMesh*> m_lstLocalReferences;
    int m_nFadeState;
    char m_bFade;
    char gap_91[3];

    float m_fFadeAlpha;
    ~TileData();
    TileData();
    static const int FADE;
    static const int DO_NOT_FADE;
    static const int FADE_NOT_CHECKED;
};

void PartSetFog(Part* pPart, bool bFog);

void ClipProjection(Vector& vPosition, const Vector& vStart, const Plane& plane);
Model* FindModel(char* sName);
Model* LoadModel(FILE* pModelFile, FILE* pVertexFile);
Part* CreateInstanceTree(MdlNode* pNode, Gob* pGob);

void PartInitMaterial(Gob* pGob, Part* pPart, char* sName);

void CreateReferenceObjects(Gob* pGob, Part* pPart);
Part* FindPart(Part* pPart, char* sName);
int PartSetCanDownSample(Part* pPart, char bCanDownSample);

#include "aurmaterial.h"
class Part;
class VertexProgram;
class Plane;
class Vector;
class CAurShadowData;
class VertexPrimitiveFlat;

class SkinVertexWeight
{
public:
    char* m_apBones[4];
    float m_afWeights[4];
    int m_anBoneIndices[4];
    int m_nWeights;
};

void GatherBones(Part*, List<Vector>&, List<Quaternion>&, Vector, Quaternion);

extern VertexProgram* g_pSkinBumpedOutVP;
extern VertexProgram* g_pSkinBumpedOutWithTangentVP;

extern VertexProgram* g_pBumpyShinySkinPass1;
extern VertexProgram* g_pBumpyShinySkinPass2;
extern VertexProgram* g_pBumpyShinySkinPass2_3bone;

#define SKIN_BLUR_SIDE_SCALE 0.15f
#define SKIN_BLUR_NORMAL_OFFSET 0.01f
#define SKIN_BLUR_LENGTH_SCALE 0.85f
#define SKIN_BLUR_ALPHA 0.5f
#define SKIN_BLUR_CULL_COSINE_SQUARED 0.5625f

#include <aurpointer.h>

class Animation;
class CAurObject;
class Gob;
class MaxTree;
class MdlNode;
class Part;
class Quaternion;
class Vector;
class OOAnimRun;

#define ANIMATION_TRANSITION_TIME_LIMIT (-95.0f)
#define ANIMATION_TRANSITION_TIME_ORIGIN (-100.0f)

#define ANIMATION_TIME_UNAVAILABLE (-1.0f)

#define ANIMATION_EVENT_TIME_EPSILON 0.0000001f

#define ANIMATION_EVENT_TIME_LIMIT 1000000000.0f

#define LOOKAT_MAX_ANGLE_DEGREES 89.0f

#define LOOKAT_KEY_END_TIME 0.25f

class CallBack
{
public:
    char m_sEvent[32];
    void (*m_pfnCallBack)(CAurObject*, char*, void*);
    void* m_pData;
    float m_fDelay;
    bool m_bRepeat;
    char m_abReserved2D[3];

    CallBack(void (*pfnCallBack)(CAurObject*, char*, void*), char* sEvent,
             void* pData, float fDelay, bool bRepeat);
};

class CustomAnimation
{
public:
    char m_bDeactivated;
    MaxTree* m_pTree;
    OOAnimRun* m_pOutOfOrderRun;
#ifndef _XBOX
    CustomAnimation()
    { m_pTree = 0; m_pOutOfOrderRun = 0; m_bDeactivated = 0;
    }
#else
    CustomAnimation();
#endif
    virtual ~CustomAnimation();
    virtual unsigned char IsActive() = 0;
    virtual void Deactivate() = 0;
    virtual void Control() = 0;
};

class AnimRun
{
public:
    static AnimRun* m_pTraceRun;

    static float m_AnimLength;
    static int m_AnimFlags;

    static const int FLAG_SUPPRESS_KEY_REPORTS = 1;
    Animation* m_pAnimation;
    float m_fSpeed;
    int m_nFlags;
    static const int FLAG_CURRENT_ANIMATION_PENDING = 0x8;
    static const int FLAG_CURRENT_ANIMATION_SUPPRESSED = 0x20;

    static const int FLAG_OUT_OF_ORDER = 512;

    enum
    {
        FLAG_STOP_AT_END = 1,
        FLAG_REPORT_LOOP_EVENTS = 4,
        FLAG_RETAIN_OLDER_RUNS = 8,
        FLAG_REPORT_KEYS = 256,
        FLAG_BLEND_OUT = 1024
    };
    float m_fPreviousTime;
    float m_fTime;
    float m_fBlendWeight;
    float m_fTransitionTime;
    float m_fScale;
    int m_bDone;

    AnimRun(Animation* pAnimation);
};

class OOAnimRun : public AnimRun
{
public:
    float m_fBlendTime;
    float* m_pDeltaValues;
    float* m_pStartValues;
    int m_nNumValues;

    OOAnimRun(Animation*, float*, float*, int, float);
    ~OOAnimRun();
};

struct SWMGReportKeyData
{
    float m_fTime;
    Vector* m_pPosition;
    Quaternion* m_pOrientation;
    unsigned char m_bPositionKey;
};

class NewController
{
public:

    int m_nOffset;
    short m_nNumKeys;
    short m_nRows;
    short m_nKeyIndex;
    short m_nColumnIndex;

    char m_nValueFormat;

    enum
    {
        VALUE_TYPE_MASK = 0x0f,
        VALUE_TYPE_FLOAT = 1,
        VALUE_TYPE_COMPRESSED_QUATERNION = 2,
        VALUE_TYPE_VECTOR = 3,
        VALUE_TYPE_QUATERNION = 4
    };

    enum
    {
        INTERPOLATION_MASK = 0xf0,
        INTERPOLATION_BEZIER = 0x10
    };

    enum
    {
        OFFSET_POSITION = 0x08,
        OFFSET_ORIENTATION = 0x14,
        OFFSET_SCALE = 0x24
    };

    enum { NODE_OFFSET_NONE = -1 };

    enum QuaternionPacking
    {
        QUATERNION_COMPONENT_MASK = 0x7ff,
        QUATERNION_COMPONENT_BITS = 11,
        QUATERNION_Z_SHIFT = 22,
        QUATERNION_Z_MAX = 0x3ff
    };

    enum
    {
        QUATERNION_COMPONENT_RANGE = 2046,
        QUATERNION_Z_RANGE = 1022
    };

    void GetKeyIndexAndAlpha(float, MdlNode*, int&, float&);
    int GetFloatValue(float, MdlNode*, float&);
    int GetVectorValue(float, MdlNode*, Vector&);
    void GetQuaternionFromIndexLocation(int, MdlNode*, Quaternion&);
    int GetQuaternionValue(float, MdlNode*, Quaternion&);
    void Control(Part*, MdlNode*, float, float, float);
    void ReportKey(Part*, MdlNode*, int, float);
    void ReportKeys(Part*, MdlNode*, float, float);
};

class LookAtAnimation : public CustomAnimation
{
public:
    SafePointer<CAurObject> m_pTarget;
    Gob* m_pGob;
    Part* m_pPart;
    Part* m_pHeadPart;

    float m_fYawLimit;
    float m_fPitchLimit;
    float m_fDuration;
    float m_fMaxDistance;

    float* m_pEndOrientation;
    float* m_pStartOrientation;
    char m_bRestoringOrientation;

    LookAtAnimation(CAurObject*, Part*, Gob*, Part*, float, float, float,
                    float);
    virtual ~LookAtAnimation();
    virtual unsigned char IsActive();
    virtual void Deactivate();
    virtual void Control();
    virtual void BuildAnimation();
};

#include "aurmodel.h"

class Animation : public MaxTree
{
public:

    enum { EVENT_REVERSE = 1, EVENT_ALL = 2 };
    float m_fLength;

    float m_fTransitionTime;

    char m_sAnimRoot[32];
    List<AurKey<char[32]> > m_lEvents;
    unsigned char* m_pBinaryData;

    Animation(char* sName);
    ~Animation();
    int InternalParseField(char*);
#ifdef _XBOX
    void PruneNodes(MdlNode* pNode);
    void PruneToAnimRoot();
#endif
};

Animation* FindAnimation(Model* pModel, char* sName);

#include <stdio.h>

#include <aurlist.h>

class AABB;
class Animation;
class MdlNode;
class MdlNodeAABB;
class MdlNodeAnimMesh;
class MdlNodeDanglyMesh;
class MdlNodeLight;
class MdlNodeLightsaber;
class MdlNodeSkin;
class MdlNodeTriMesh;
class MaxTree;

class IODispatcher
{
public:

    IODispatcher();

    static IODispatcher* GetRef();
    MaxTree* ReadSync(char* sFileName);
    MaxTree* ReadSync(FILE* pModelFile, FILE* pVertexFile);
    bool WriteSync(MaxTree* pTree, char* sFileName);

    List<int> m_lDispatchEntries;
};

extern Vector g_HitCheckBoxExpansion;
bool AurControlPart(char* sCommand);
extern List<Model*> Models;
// PC 0x00453070
inline CAurObject* Part::GetAurObject()
{
    return m_pAurObject;
}
// PC 0x00453080
inline char* Part::Name()
{
#ifdef _XBOX
    return m_pNode->m_pModel->AsModel()->m_lNames.m_pList[m_pNode->m_nIndex];
#else
    MaxTree* pTree = m_pNode->m_pModel; Model* pModel = (pTree->m_nType & MaxTree::TYPE_MASK) == 2 ? static_cast<Model*>(pTree) : 0; return pModel->m_lNames.m_pList[m_pNode->m_nIndex];
#endif
}

#ifdef _XBOX
#define AUR_MODEL_ADDREF(pModel, pGob) (pModel)->AddRef(pGob)
#else
#define AUR_MODEL_ADDREF(pModel, pGob) ++(pModel)->m_nReferenceCount
#endif

#endif
