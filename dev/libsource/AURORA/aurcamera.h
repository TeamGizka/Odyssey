#ifndef AURCAMERA_H
#define AURCAMERA_H

class CAurBehavior;
class CAurObject;
class CAurPart;
class CAurRoom;
class CAurScene;
class CHitInfo;
class LightManager;
class Part;
class PartLight;
class Plane;
class Quaternion;
class Vector;
class WindManager;
#include "aurbehavior.h"
#include "aurmodel.h"
#include "math3d.h"
#include "aurscene.h"

class CAurCamera
{
public:
    virtual ~CAurCamera();
    virtual CAurObject* AsCAurObject() = 0;
    virtual void RenderScene() = 0;
    virtual void Animate(float fTimeStep) = 0;

    virtual int PlayAnimation(const char* sName, float fSpeed, int nFlags,
                              float fBlend) = 0;

    virtual int AttachToScene(CAurScene* pScene) = 0;
    virtual Vector SetPosition(Vector vPosition) = 0;
    virtual Quaternion SetOrientation(Quaternion qOrientation) = 0;
    virtual Vector GetPosition() = 0;
    virtual Quaternion GetOrientation() = 0;
    virtual Vector MapToScreen(Vector vPoint, bool bNormalize) = 0;
    virtual Vector MouseDir(int nX, int nY) = 0;
    virtual int HitCheck(int nX, int nY, Vector* pHit, CHitInfo* pInfo) = 0;
    virtual void SetViewPort(int nX, int nY, int nWidth, int nHeight) = 0;
    virtual int GetViewPortHeight() = 0;
    virtual void GetClipDist(float& fNear, float& fFar) = 0;
    virtual void SetClipDist(float fNear, float fFar) = 0;
    virtual void SetViewAngle(float fViewAngle) = 0;
    virtual float GetViewAngle() = 0;
    virtual float HitCheckViewVolumeNearest(const Vector& vStart,
                                            const Vector& vEnd, Vector* pHit,
                                            int* pPlane) = 0;
    virtual int SceneBoundaryCheck(const Vector& vStart,
                                   const Vector& vEnd, const Plane& plane,
                                   Vector* pHit) = 0;
    virtual void BeginViewAngleAnimation(float fTarget, float fDuration,
                                         int nType) = 0;
    virtual void EndViewAngleAnimation(bool bReset) = 0;
    virtual void SetFollow(CAurObject* pObject) = 0;
    virtual void BehaviorMessage(char* sMessage) = 0;
    virtual float GetYaw() = 0;
    virtual float GetPitch() = 0;
    virtual float GetDist() = 0;
    virtual CAurCamera* AsAurCamera()  // PC 0x00698BD0
    {
        return this;
    }
    virtual CAurBehavior* AttachToObject(CAurObject* pObject, char* sPart,
                               int nFlags) = 0;
    virtual CAurBehavior* AttachToPart(CAurObject* pObject, CAurPart* pPart,
                             int nFlags) = 0;
    virtual void SetBehavior(CAurBehavior* pBehavior) = 0;
    virtual CAurBehavior* GetBehavior(int nIndex) = 0;
    virtual int ViewVolumeSet() = 0;
    virtual Plane GetViewVolume(int nIndex) = 0;
};

class ViewAngleAnimation
{
public:
    float m_fDuration;
    float m_fElapsed;
    float m_fStartAngle;
    float m_fEndAngle;
    int m_nType;
};

class Camera : public CAurCamera, public Gob
{
public:
    Camera(char* sName);
    virtual ~Camera();

    virtual void Render(bool bRenderLast);
    virtual int PlayAnimation(const char* sName, float fSpeed, int nFlags,
                              float fBlend);
    // PC 0x00471660
    virtual CAurCamera* AsCAurCamera()
    {
        return this;
    }
    virtual Camera* AsCamera()  // PC 0x00471660
    {
        return this;
    }
    virtual CAurObject* AsCAurObject()  // PC 0x00471670
    {
        return this;
    }
    virtual void GetMinimumBoundingBox(Vector& vMinimum, Vector& vMaximum);
    virtual void GetMinimumBoundingSphere(Vector& vPosition, float& fRadius)  // PC 0x006049D0
    {
    }
    virtual int AttachToScene(CAurScene* pScene);
    virtual CAurBehavior* AttachToObject(CAurObject* pObject, char* sPart, int nFlags);
    virtual CAurBehavior* AttachToPart(CAurObject* pObject, CAurPart* pPart, int nFlags);
    virtual void SetBehavior(CAurBehavior* pBehavior);
    virtual Vector SetPosition(Vector vPosition);
    virtual Quaternion SetOrientation(Quaternion qOrientation);
    virtual Vector GetPosition()  // PC 0x00471680
    {
        return m_vPosition;
    }
    virtual Quaternion GetOrientation()  // PC 0x004716B0
    {
        return m_qOrientation;
    }
    virtual void BehaviorMessage(char* sMessage);
    virtual CAurBehavior* GetBehavior(int nIndex);
    virtual void Animate(float fTimeStep);

    Vector MouseDir(int nX, int nY);
    int GetViewPortHeight()  // PC 0x004716E0
    {
        return m_nViewPortHeight;
    }
    void SetClipDist(float fNear, float fFar);
    void GetClipDist(float& fNear, float& fFar)  // PC 0x004716F0
    {
        fNear = m_fNearClip;
        fFar = m_fFarClip;
    }
    void SetViewAngle(float fViewAngle);
    float GetViewAngle()  // PC 0x00471710
    {
        return m_fViewAngle;
    }
    void BeginViewAngleAnimation(float fTarget, float fDuration, int nType);
    void EndViewAngleAnimation(bool bReset);
    void SetViewPort(int nX, int nY, int nWidth, int nHeight);
    virtual CAurObject* HitCheck(Vector vStart, Vector vEnd, Vector* pHit);
    int HitCheck(int nX, int nY, Vector* pHit, CHitInfo* pInfo);
    void SetFollow(CAurObject* pObject);
    float GetYaw();
    float GetPitch();
    float GetDist();
    virtual void SetFadePlanes(Plane& planeNear, Plane& planeFar)  // PC 0x00471720
    {
        m_planeFadeNear = planeNear;
        m_planeFadeFar = planeFar;
    }
    int ViewVolumeSet()  // PC 0x00471770
    {
        return m_bViewVolumeSet;
    }
    void RenderScene();
    Vector MapToScreen(Vector vPoint, bool bNormalize);
    float HitCheckViewVolumeNearest(const Vector& vStart, const Vector& vEnd,
                                    Vector* pHit, int* pPlane);
    int SceneBoundaryCheck(const Vector& vStart, const Vector& vEnd,
                           const Plane& plane, Vector* pHit);
    Plane GetViewVolume(int nIndex);

public:

    enum ViewVolumePlane
    {
        VIEW_PLANE_NEAR = 0,
        VIEW_PLANE_FAR = 1,
        VIEW_PLANE_LEFT = 2,
        VIEW_PLANE_RIGHT = 3,
        VIEW_PLANE_TOP = 4,
        VIEW_PLANE_BOTTOM = 5,
        VIEW_PLANE_COUNT = 6,
        VIEW_SIDE_PLANE_COUNT = 4
    };

    float m_fViewAngle;
    ViewAngleAnimation* m_pViewAngleAnimation;
    float m_fViewPlaneScale;
    float m_fNearClip;
    float m_fFarClip;
    int m_nViewPortX;
    int m_nViewPortY;
    int m_nViewPortWidth;
    int m_nViewPortHeight;
    int m_bViewVolumeSet;
    List<Plane> m_lstViewVolume;

    Plane m_planeFadePerpendicular;
    Plane m_planeFadeNear;
    Plane m_planeFadeFar;
};

extern Camera* CurrentCamera;

extern int overridecamera;
extern int frustumpick;
extern Vector frustumn;
extern float frustumd;
extern Vector cameraposition;
extern Quaternion cameraorientation;

CAurCamera* NewCAurCamera(char* sName);

#endif
