#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include <string.h>
#include "aurcamera.h"
#ifdef _XBOX
#include "aurconsole.h"
#endif
#include "aurrendergl.h"
#include "swmain.h"

static inline void AurcameraTrace2();
static inline void AurcameraTrace1();
#define AURCAMERA_TRACE() AurcameraTrace1()

Camera* CurrentCamera = 0;

Vector frustumn;
float frustumd;
int frustumpick = 0;
static int wireframe = 0;
int overridecamera = 0;
Vector cameraposition;
Quaternion cameraorientation;
// PC 0x004710C0
void Camera::Render(bool bRenderLast)
{
    if (CurrentCamera != this && CurrentCamera != 0)
    {
        Gob::Render(bRenderLast);
    }
}
// PC 0x00401590
int Camera::PlayAnimation(const char* sName, float fSpeed, int nFlags,
                          float fBlend)
{
    return 0;
}
// PC 0x004710E0
void Camera::SetClipDist(float fNear, float fFar)
{
    m_fNearClip = fNear;
    m_fFarClip = fFar;
}
// PC 0x00471100
void Camera::SetViewAngle(float fViewAngle)
{
    m_fViewAngle = fViewAngle;
}
// PC 0x00471110
void Camera::BeginViewAngleAnimation(float fTarget, float fDuration, int nType)
{
    if (m_pViewAngleAnimation != 0)
    {
        delete m_pViewAngleAnimation;
        m_fViewAngle = 45.0f;
    }

    m_pViewAngleAnimation = new ViewAngleAnimation;
    m_pViewAngleAnimation->m_fDuration = fDuration;
    m_pViewAngleAnimation->m_fElapsed = 0.0f;
    m_pViewAngleAnimation->m_fEndAngle = fTarget;
    m_pViewAngleAnimation->m_fStartAngle = m_fViewAngle;
    m_pViewAngleAnimation->m_nType = nType;
}
// PC 0x00471180
void Camera::EndViewAngleAnimation(bool bReset)
{
    delete m_pViewAngleAnimation;
    m_pViewAngleAnimation = 0;

    if (bReset)
    {
        m_fViewAngle = 45.0f;
    }
}
// PC 0x004711C0
void Camera::SetViewPort(int nX, int nY, int nWidth, int nHeight)
{
    m_nViewPortX = nX;
    m_nViewPortY = nY;

    if (nWidth <= 0)
    {
        nWidth = 1;
    }

    m_nViewPortWidth = nWidth;

    if (nHeight <= 0)
    {
        nHeight = 1;
    }

    m_nViewPortHeight = nHeight;
}
// PC 0x00471200
CAurObject* Camera::HitCheck(Vector vStart, Vector vEnd, Vector* pHit)
{
    return Gob::HitCheck(vStart, vEnd, pHit, 0);
}
// PC 0x00471250
Vector Camera::SetPosition(Vector vPosition)
{
    m_vPosition = vPosition;
    return vPosition;
}
// PC 0x00471290
Quaternion Camera::SetOrientation(Quaternion qOrientation)
{
    m_qOrientation = qOrientation;
    return qOrientation;
}
// PC 0x004712D0
int Camera::AttachToScene(CAurScene* pScene)
{
    SetScene((Scene*)pScene);
    return 1;
}
// PC 0x004712F0
void Camera::SetFollow(CAurObject* pObject)
{
    if (pObject != 0)
    {
        SetBehavior(new CAurBehaviorCameraFollow(this, pObject));
    }
    else
    {
        SetBehavior(0);
    }
}
// PC 0x00471390
void Camera::BehaviorMessage(char* sMessage)
{
    Gob::BehaviorMessage(sMessage);
}
// PC 0x004713A0
float Camera::GetYaw()
{
    if (m_pBehavior != 0 && m_pBehavior->AsBehaviorCameraFollow() != 0)
    {
        return ((CAurBehaviorCameraFollow*)m_pBehavior)->m_fYaw;
    }

    return 0.0f;
}
// PC 0x004713D0
float Camera::GetPitch()
{
    if (m_pBehavior != 0 && m_pBehavior->AsBehaviorCameraFollow() != 0)
    {
        return ((CAurBehaviorCameraFollow*)m_pBehavior)->m_fPitch;
    }

    return 0.0f;
}
// PC 0x00471400
float Camera::GetDist()
{
    if (m_pBehavior != 0 && m_pBehavior->AsBehaviorCameraFollow() != 0)
    {
        return ((CAurBehaviorCameraFollow*)m_pBehavior)->m_fDistance;
    }

    return 0.0f;
}
// PC 0x00471430
CAurBehavior* Camera::AttachToObject(CAurObject* pObject, char* sPart, int nFlags)
{
    return Gob::AttachToObject(pObject, sPart, nFlags);
}
// PC 0x00471440
CAurBehavior* Camera::AttachToPart(CAurObject* pObject, CAurPart* pPart, int nFlags)
{
    return Gob::AttachToPart(pObject, pPart, nFlags);
}
// PC 0x00471450
CAurBehavior* Camera::GetBehavior(int nIndex)
{
    return Gob::GetBehavior(nIndex);
}
// PC 0x00471460
void Camera::SetBehavior(CAurBehavior* pBehavior)
{
    Gob::SetBehavior(pBehavior);
}
// PC 0x00471500
inline CAurCamera::~CAurCamera()
{
}
// PC 0x00471530
inline void GLRender::SetViewPort(double fFieldOfView, double fAspect,
                           double fNearClip, double fFarClip)
{
    AURCAMERA_TRACE();
    GLRender_SetPerspectiveTransform(fFieldOfView, fAspect, fNearClip, fFarClip);
}
// PC 0x00473530
Camera::Camera(char* sName)
    : Gob(sName), m_lstViewVolume(0)
{
    m_fViewAngle = 45.0f;
    m_fNearClip = 0.1f;
#ifdef _XBOX
    m_pViewAngleAnimation = 0;
#endif
    m_fFarClip = 100.0f;
    m_fViewPlaneScale = 1.0f;
#ifdef _XBOX
    AUR_OBJECT_CONSOLE_VAR(m_sName, "viewangle", &m_fViewAngle);
    AUR_OBJECT_CONSOLE_VAR(m_sName, "viewsize", &m_fViewPlaneScale);
    AUR_OBJECT_CONSOLE_VAR(m_sName, "clipnear", &m_fNearClip);
    AUR_OBJECT_CONSOLE_VAR(m_sName, "clipfar", &m_fFarClip);
    m_nViewPortHeight = 0;
    m_nViewPortWidth = 0;
    m_nViewPortY = 0;
    m_nViewPortX = 0;
    m_lstViewVolume.SetSize(6);
#else
    m_pViewAngleAnimation = 0;
    m_nViewPortHeight = 0;
    m_nViewPortWidth = 0;
    m_nViewPortY = 0;
    m_nViewPortX = 0;

    List<Plane>& lstViewVolume = m_lstViewVolume;
    lstViewVolume.allocate(6);
    lstViewVolume.m_nCount = 6;
#endif
}
// PC 0x00471560
Camera::~Camera()
{
#ifdef _XBOX
    strcpy(g_sObjectFuncName, m_sName);
    strcat(g_sObjectFuncName, ".");
    DeleteFuncWithPrefix(g_sObjectFuncName);
#endif
}
// PC 0x00471780
void Camera::RenderScene()
{

    if (m_pScene != 0)
    {
        int nFillMode = wireframe ? FILL_LINE : FILL_SOLID;
        GLRender_SetPolygonRenderMode(FACING_FRONT_AND_BACK, nFillMode);
        GLRender_SetMatrixMode(TRANSFORM_MODELVIEW);
        GLRender_PushCurrentMatrix();
        GLRender_SetMatrixMode(0);
        GLRender_PushCurrentMatrix();
#ifdef _XBOX

        GLRender::LoadViewIdentity();
#endif

        GLint aViewport[4];
        GLRender_GetViewPort(aViewport);
        float fAspect;
        if (m_nViewPortWidth != 0 || m_nViewPortHeight != 0)
        {
            if (m_nViewPortX >= g_nScreenWidth ||
                m_nViewPortX + m_nViewPortWidth < 0 ||
                m_nViewPortY >= g_nScreenHeight ||
                m_nViewPortY + m_nViewPortHeight < 0)
            {
                ++m_pScene->m_nAnimationFrame;
                return;
            }
            GLRender::SetViewPort(m_nViewPortX, m_nViewPortY,
                                  m_nViewPortWidth, m_nViewPortHeight);
            float fWidth = (float)m_nViewPortWidth;
            float fHeight = (float)m_nViewPortHeight;
            fAspect = fWidth / fHeight;
            m_fViewPlaneScale = m_nViewPortHeight * 0.01f;
        }
        else
        {
            float fWidth = (float)aViewport[2];
            float fHeight = (float)aViewport[3];
            fAspect = fWidth / fHeight;
            m_fViewPlaneScale = aViewport[3] * 0.01f;
        }

        GLRender_SetMatrixMode(0);
        GLRender_LoadMatrixIdentity();
        m_lstViewVolume[VIEW_PLANE_NEAR] =
            Plane(Vector(0.0f, 0.0f, 1.0f), m_fNearClip);
        m_lstViewVolume[VIEW_PLANE_FAR] =
            Plane(Vector(0.0f, 0.0f, -1.0f), -m_fFarClip);

        if (m_fViewAngle > 0.0f)
        {
            GLRender::SetViewPort((double)(m_fViewAngle), (double)(fAspect),
                                  (double)(m_fNearClip), (double)(m_fFarClip));
            float fLeft = -tan(DegToRad(m_fViewAngle * 0.5f)) * fAspect;
            float fRight = tan(DegToRad(m_fViewAngle * 0.5f)) * fAspect;
            float fBottom = -tan(DegToRad(m_fViewAngle * 0.5f));
            float fTop = tan(DegToRad(m_fViewAngle * 0.5f));
            m_lstViewVolume[VIEW_PLANE_LEFT] = Plane(
                NormalOf(Vector(), Vector(fLeft, fTop, -1.0f),
                         Vector(fLeft, fBottom, -1.0f)), 0.0f);
            m_lstViewVolume[VIEW_PLANE_RIGHT] = Plane(
                NormalOf(Vector(), Vector(fRight, fBottom, -1.0f),
                         Vector(fRight, fTop, -1.0f)), 0.0f);
            m_lstViewVolume[VIEW_PLANE_TOP] = Plane(
                NormalOf(Vector(), Vector(fRight, fTop, -1.0f),
                         Vector(fLeft, fTop, -1.0f)), 0.0f);
            m_lstViewVolume[VIEW_PLANE_BOTTOM] = Plane(
                NormalOf(Vector(), Vector(fLeft, fBottom, -1.0f),
                         Vector(fRight, fBottom, -1.0f)), 0.0f);
        }
        else
        {
            AurOrtho(0.0, (double)(fAspect * m_fViewPlaneScale), 0.0,
                     (double)m_fViewPlaneScale, (double)m_fNearClip, (double)m_fFarClip);
            m_lstViewVolume[VIEW_PLANE_LEFT] =
                Plane(Vector(-1.0f, 0.0f, 0.0f), 0.0f);
            m_lstViewVolume[VIEW_PLANE_RIGHT] =
                Plane(Vector(1.0f, 0.0f, 0.0f), -fAspect * m_fViewPlaneScale);
            m_lstViewVolume[VIEW_PLANE_TOP] =
                Plane(Vector(0.0f, -1.0f, 0.0f), 0.0f);
            m_lstViewVolume[VIEW_PLANE_BOTTOM] =
                Plane(Vector(0.0f, 1.0f, 0.0f), -(m_fViewPlaneScale));
        }

        if (overridecamera)
        {
            GLRender::SetViewTransform(cameraposition, cameraorientation);
        }
        else
        {
            GLRender::SetViewTransform(m_vPosition, m_qOrientation);
        }
        for (int i = 0; i < VIEW_PLANE_COUNT; ++i)
        {
            m_lstViewVolume[i].m_vNormal =
                m_qOrientation * m_lstViewVolume[i].m_vNormal;
            float fDistance =
                (m_lstViewVolume[i].m_fDistance) -
                (m_lstViewVolume[i].m_vNormal.m_fZ * m_vPosition.m_fZ) -
                (m_lstViewVolume[i].m_vNormal.m_fY * m_vPosition.m_fY) -
                (m_lstViewVolume[i].m_vNormal.m_fX * m_vPosition.m_fX);
            m_lstViewVolume[i].m_fDistance = fDistance;
        }
        frustumn = m_lstViewVolume[frustumpick].m_vNormal;
        frustumd = m_lstViewVolume[frustumpick].m_fDistance;
        m_bViewVolumeSet = 1;
        CurrentCamera = this;
        GLRender_SetMatrixMode(TRANSFORM_MODELVIEW);
        m_pScene->Render();
        GLRender_SetMatrixMode(0);
        GLRender_PopCurrentMatrix();
        GLRender_SetMatrixMode(TRANSFORM_MODELVIEW);
        GLRender_PopCurrentMatrix();
        CurrentCamera = 0;
        if (m_nViewPortWidth != 0 || m_nViewPortHeight != 0)
        {
            GLRender_SetViewPort(aViewport[0], aViewport[1], aViewport[2], aViewport[3]);
        }
    }
}
// PC 0x00472730
int Camera::HitCheck(int nX, int nY, Vector* pHit, CHitInfo* pInfo)
{

    if (m_pScene == 0)
    {
        return 0;
    }

    if (m_fViewAngle > 0.0f)
    {
        Vector vPoint = MouseDir(nX, nY);
        return m_pScene->HitCheck(vPoint * m_fNearClip + m_vPosition,
                                 m_fFarClip * vPoint + m_vPosition, pHit, pInfo);
    }

    GLint aViewport[4];
    for (int i = 0; i < 4; ++i)
    {
        aViewport[i] = (&m_nViewPortX)[i];
    }
    if (aViewport[2] == 0 || aViewport[3] == 0)
    {
        GLRender_GetViewPort(aViewport);
    }

    nX -= aViewport[0];
    nY -= aViewport[1];
    if (nX < 0 || nY < 0 || nX >= aViewport[2] || nY >= aViewport[3])
    {
        return 0;
    }

    Vector vNear((nX + 0.5f) / aViewport[3] * m_fViewPlaneScale,
                 (nY + 0.5f) / aViewport[3] * m_fViewPlaneScale,
                 -m_fNearClip);
    Vector vFar = vNear;
    vFar.m_fZ -= m_fFarClip;
    return m_pScene->HitCheck(vNear + m_vPosition, vFar + m_vPosition,
                             pHit, pInfo);
}
// PC 0x004729E0
Vector Camera::MouseDir(int nX, int nY)
{
    AurcameraTrace1();
    GLint aViewport[4];
    for (int i = 0; i < 4; ++i)
    {
        aViewport[i] = (&m_nViewPortX)[i];
    }

    if (aViewport[2] == 0 || aViewport[3] == 0)
    {
        GLRender_GetViewPort(aViewport);
    }

    float fTan = tan(m_fViewAngle * 0.5f * DEG_TO_RAD);
    float fX = ((nX - aViewport[0]) - aViewport[2] * 0.5f) * fTan /
               (aViewport[3] * 0.5f);
    float fY = ((nY - aViewport[1]) - aViewport[3] * 0.5f) * fTan /
               (aViewport[3] * 0.5f);
    Vector vDirection(fX, fY, -1.0f);
    vDirection = m_qOrientation * vDirection;
    vDirection.Normalize();
    return vDirection;
}
// PC 0x00472C20
Vector Camera::MapToScreen(Vector vPoint, bool bNormalize)
{
    AurcameraTrace1();

    GLint aViewport[4];
    for (int i = 0; i < 4; ++i)
    {
        aViewport[i] = (&m_nViewPortX)[i];
    }

    if (aViewport[2] == 0 || aViewport[3] == 0)
    {
        GLRender_GetViewPort(aViewport);
    }

    if (m_fViewAngle > 0.0f)
    {
        vPoint = vPoint - m_vPosition;
        vPoint = -m_qOrientation * vPoint;
        Vector vScreen = planelineintersection(
            Vector(0.0f, 0.0f, 1.0f), 1.0f, Vector(), vPoint);
        float fTan = tan(m_fViewAngle * 0.5f * DEG_TO_RAD);
        float fHalfHeight = aViewport[3] * 0.5f;
        float fX = (fHalfHeight / fTan) * vScreen.m_fX + aViewport[2] * 0.5f + aViewport[0];
        float fY = (fHalfHeight / fTan) * vScreen.m_fY + aViewport[1] + fHalfHeight;
        if (bNormalize)
        {
            fX /= aViewport[2];
            fY = 1.0f - fY / aViewport[3];
        }
        return Vector(fX, fY, 0.0f);
    }
    return Vector();
}
// PC 0x00472F20
void Camera::Animate(float fTimeStep)
{
    AurcameraTrace1();
    if (m_pViewAngleAnimation != 0)
    {
        m_pViewAngleAnimation->m_fElapsed =
            fTimeStep + m_pViewAngleAnimation->m_fElapsed;

        if (m_pViewAngleAnimation->m_fElapsed >
            m_pViewAngleAnimation->m_fDuration)
        {
            delete m_pViewAngleAnimation;
            m_pViewAngleAnimation = 0;
        }
        else
        {
            int nType = m_pViewAngleAnimation->m_nType;
            float fPosition = m_pViewAngleAnimation->m_fElapsed /
                              m_pViewAngleAnimation->m_fDuration;

            if (nType == 0)
            {
                m_fViewAngle =
                    (1.0f - fPosition) * m_pViewAngleAnimation->m_fStartAngle +
                    fPosition * m_pViewAngleAnimation->m_fEndAngle;
            }
            else if (nType == 1)
            {
                fPosition = 1.0f - (cosf(fPosition * PI) + 1.0f) * 0.5f;
                m_fViewAngle =
                    (1.0f - fPosition) * m_pViewAngleAnimation->m_fStartAngle +
                    fPosition * m_pViewAngleAnimation->m_fEndAngle;
            }
            else if (nType == 2)
            {
                fPosition = 1.0f - cosf(fPosition * HALF_PI);
                m_fViewAngle =
                    (1.0f - fPosition) * m_pViewAngleAnimation->m_fStartAngle +
                    fPosition * m_pViewAngleAnimation->m_fEndAngle;
            }
            else if (nType == 3)
            {
                fPosition = sinf(fPosition * HALF_PI);
                m_fViewAngle =
                    (1.0f - fPosition) * m_pViewAngleAnimation->m_fStartAngle +
                    fPosition * m_pViewAngleAnimation->m_fEndAngle;
            }
        }
    }

    Gob::Animate(fTimeStep);
}
// PC 0x006049D0
void Camera::GetMinimumBoundingBox(Vector& vMinimum, Vector& vMaximum)
{
}
// PC 0x00473000
float Camera::HitCheckViewVolumeNearest(const Vector& vStart,
                                        const Vector& vEnd, Vector* pHit,
                                        int* pPlane)
{
    float fNearest = m_fFarClip;
    Vector vHit;
    Vector vZero;

    if (pPlane != NULL)
    {
        *pPlane = 0;
    }

    for (int nPlane = 2; nPlane < 6; nPlane++)
    {
        vHit = planelineintersection(
            m_lstViewVolume[nPlane].m_vNormal,
            m_lstViewVolume[nPlane].m_fDistance, vStart, vEnd);
        if (!equal(vHit, vZero))
        {
            int nOther = 0;
            while (nOther < 6)
            {
                if (nOther != nPlane &&
                    vHit.m_fY * m_lstViewVolume[nOther].m_vNormal.m_fY +
                            vHit.m_fZ *
                                m_lstViewVolume[nOther].m_vNormal.m_fZ +
                            vHit.m_fX *
                                m_lstViewVolume[nOther].m_vNormal.m_fX +
                            m_lstViewVolume[nOther].m_fDistance >
                        0.0f)
                {
                    break;
                }
                nOther++;
                if (nOther >= 6)
                {
                    vHit -= m_vPosition;
                    Plane* pFarPlane = &m_lstViewVolume[1];
                    float fDistance = vHit.m_fY * pFarPlane->m_vNormal.m_fY +
                                      vHit.m_fZ * pFarPlane->m_vNormal.m_fZ +
                                      vHit.m_fX * pFarPlane->m_vNormal.m_fX;
                    if (fDistance < fNearest && fDistance > 0.0f)
                    {
                        fNearest = fDistance;
                        if (pHit != NULL)
                        {
                            *pHit = vHit;
                        }
                        if (pPlane != NULL)
                        {
                            *pPlane = 1;
                        }
                    }
                    break;
                }
            }
        }
    }

    return fNearest;
}
// PC 0x004731B0
int Camera::SceneBoundaryCheck(const Vector& vStart, const Vector& vEnd,
                               const Plane& plane, Vector* pHit)
{

    Plane planeHorizontal(Vector(0.0f, 0.0f, 1.0f), 0.0f);
    int aAdjacent[VIEW_SIDE_PLANE_COUNT] =
    {
        VIEW_PLANE_TOP, VIEW_PLANE_BOTTOM, VIEW_PLANE_RIGHT, VIEW_PLANE_LEFT
    };
    Vector aIntersections[VIEW_SIDE_PLANE_COUNT];
    int nHighest = 0;
    float fHighest = 0.0f;

    for (int i = 0; i < VIEW_SIDE_PLANE_COUNT; ++i)
    {
        Vector vDirection;
        vDirection = cross(
            m_lstViewVolume.m_pList[i + VIEW_PLANE_LEFT].m_vNormal,
            m_lstViewVolume.m_pList[aAdjacent[i]].m_vNormal);
        Vector& vIntersection = aIntersections[i];
        vIntersection = planelineintersection(
            planeHorizontal.m_vNormal, planeHorizontal.m_fDistance,
            m_vPosition, vDirection + m_vPosition);
        float fHeight = aIntersections[i] * plane.m_vNormal + plane.m_fDistance;
        if (fHeight > fHighest)
        {
            fHighest = fHeight;
            nHighest = i;
        }
    }

    if (fHighest == 0.0f)
    {
        return 0;
    }

    Vector vOffset = m_vPosition - aIntersections[nHighest];
    Vector vNormal;
    vNormal = normalize(cross(vOffset, vEnd - vStart));
    Plane planeHit;
    planeHit.m_vNormal = vNormal;
    planeHit.m_fDistance = (0.0f - vNormal * vStart);
    *pHit = planelineintersection(planeHit.m_vNormal, planeHit.m_fDistance,
                                 m_vPosition,
                                 m_vPosition +
                                     m_lstViewVolume.m_pList[VIEW_PLANE_FAR].m_vNormal);
    return 1;
}
// PC 0x00473490
Plane Camera::GetViewVolume(int nIndex)
{
    return m_lstViewVolume[nIndex];
}
// PC 0x00473660
CAurCamera* NewCAurCamera(char* sName)
{
    return new Camera(sName);
}

static inline void AurcameraTrace2()
{
    AurcameraTrace1();
}

static inline void AurcameraTrace1()
{
}

#ifdef _XBOX
Gob* FindGob(char* sName);

char* beginviewangleanim(char* sArguments)
{
    float fAngle;
    float fLength;
    int nInterpType;
    if (sscanf(sArguments, "%f%f%d", &fAngle, &fLength, &nInterpType) != 3)
        return "beginviewangleanim [angle] [length] [interpType]";
    Camera* pCamera = (Camera*)FindGob("camera");
    pCamera->BeginViewAngleAnimation(fAngle, fLength, nInterpType);
    return "success";
}

char* endviewangleanim(char* sArguments)
{
    Camera* pCamera = (Camera*)FindGob("camera");
    pCamera->EndViewAngleAnimation(true);
    return "success";
}
#endif
