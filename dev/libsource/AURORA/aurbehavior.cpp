#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "aurbehavior.h"
#include "swminigame.h"
#include "aurmodel.h"
#include "aurscene.h"
#include "../math3d/math3d.h"

extern int ParseInteger(char* sText, int* pnValue);
extern int ParseFloat(char* sText, float* pfValue);
extern int ParseVector(char* sText, float* pfValue);
extern int CompareInsensitive(const char* sLeft, const char* sRight);
char* firstword(char* sMessage);
int Parse(char* sText, int& nValue);
int Parse(char* sText, float& fValue);
int Parse(char* sText, Vector& vValue);
extern char sUpdateScenes[];
extern char sDying[];
extern char sUpdateDependentPosition[];
extern char sPropagateColorShifting[];
extern char sPropagateIllumination[];
extern char sPropagateRender[];
extern char sPropagateViewVolume[];
// PC 0x005EE1C0
void CAurBehavior::Message(char* sMessage)
{
}
// PC 0x004CBD30
void UpdateScenesAttach(CAurObject* pObject, char* sEvent, void* pContext)
{
    CAurBehaviorAttach* pBehavior = (CAurBehaviorAttach*)pContext;

    if (stricmp(sEvent, "UpdateScenes") == 0)
    {
        pBehavior->SetScene();
    }
    else if (pBehavior->m_nDieWithoutParent != 0 &&
             stricmp(sEvent, "Dying") == 0)
    {
        delete pBehavior->m_pObject;
    }
}
// PC 0x004CBD80
void UpdateDependentPosition(CAurObject* pObject, char* sEvent, void* pContext)
{
    if (stricmp(sEvent, "UpdateDependentPosition") == 0)
    {
        ((CAurBehaviorAttach*)pContext)->UpdateGobPosition();
    }
}
// PC 0x004CBDA0
void PropagateColorShifting(CAurObject* pObject, char* sEvent, void* pContext)
{
    if (stricmp(sEvent, "propagatecolorshift") == 0)
    {
        Gob* pSource = (Gob*)pObject;
        ((Gob*)pContext)
            ->SetColorShifting(pSource->m_vColor, pSource->m_fAlpha, 1);
    }
}
// PC 0x004CBDF0
void PropagateIllumination(CAurObject* pObject, char* sEvent, void* pContext)
{
    if (stricmp(sEvent, "propagateillumination") == 0)
    {
        Gob* pSource = (Gob*)pObject;
        ((Gob*)pContext)->SetIllumination(pSource->m_vIllumination, 1);
    }
}
// PC 0x004CBE40
void PropagateRender(CAurObject* pObject, char* sEvent, void* pContext)
{
    Gob* pGob;

    if (stricmp(sEvent, "propagaterender") == 0)
    {
        pGob = (Gob*)pContext;
        if (pGob->m_pBehavior != 0)
        {
            pGob->m_pBehavior->Control(0);
        }
        pGob->Render(1);
    }
}
// PC 0x004CBE80
void PropagateViewVolume(CAurObject* pObject, char* sEvent, void* pContext)
{
    if (stricmp(sEvent, "propagateviewvolume") == 0)
    {
        if (((Gob*)pObject)->IsInViewVolume())
        {
            ((Gob*)pContext)->EnableInViewVolume();
        }
    }
}
// PC 0x004CBEC0
void CAurBehaviorAttach::UpdateGobPosition()
{
    Gob* pGob = (Gob*)m_pObject;
    m_pPart->GetWorld(pGob->m_vPosition, pGob->m_qOrientation);
    pGob->DoEvent("UpdateDependentPosition", 0);
}
// PC 0x004CBEF0
void CAurBehaviorAttach::Message(char* sMessage)
{
    if (stricmp("diewithoutparent", firstword(sMessage)) == 0)
    {
        Parse(sMessage + 16, m_nDieWithoutParent);
    }
}
// PC 0x004CBF30
void CAurBehaviorAttachTranslate::Message(char* sMessage)
{
    CAurBehaviorAttach::Message(sMessage);
    if (stricmp("m_vTranslation", firstword(sMessage)) == 0)
    {
        Parse(sMessage + 14, m_vTranslation);
    }
}
// PC 0x004CBF90
void CAurBehaviorAttachRotate::Message(char* sMessage)
{
    CAurBehaviorAttach::Message(sMessage);
    if (stricmp("m_vRotation", firstword(sMessage)) == 0)
    {
        Parse(sMessage + 11, m_vRotation);
    }
}

void CAurBehaviorNavigate::Message(char* sMessage)
{
    if (stricmp("turn", firstword(sMessage)) == 0)
    {
        Parse(sMessage + 4, m_fTurn);
    }
    if (stricmp("tilt", firstword(sMessage)) == 0)
    {
        Parse(sMessage + 4, m_fTilt);
    }
    if (stricmp("speed", firstword(sMessage)) == 0)
    {
        Parse(sMessage + 5, m_fSpeed);
    }
    if (stricmp("raise", firstword(sMessage)) == 0)
    {
        Parse(sMessage + 5, m_fRaise);
    }
    if (stricmp("strafe", firstword(sMessage)) == 0)
    {
        Parse(sMessage + 6, m_fStrafe);
    }
}

void CAurBehaviorCameraOnAStick::Message(char* sMessage)
{
    if (stricmp("m_fDist", firstword(sMessage)) == 0)
    {
        Parse(sMessage + 7, m_fDist);
    }
    if (stricmp("m_fSpeed", firstword(sMessage)) == 0)
    {
        Parse(sMessage + 8, m_fSpeed);
    }
    if (stricmp("m_fHeight", firstword(sMessage)) == 0)
    {
        Parse(sMessage + 9, m_fHeight);
    }
}
// PC 0x004CBFF0
void CAurBehaviorCameraFollow::Message(char* sMessage)
{
    if (stricmp("turn", firstword(sMessage)) == 0)
    {
        Parse(sMessage + 4, m_fTurn);
    }
    if (stricmp("spring", firstword(sMessage)) == 0)
    {
        Parse(sMessage + 6, m_fSpring);
    }
    if (stricmp("dist", firstword(sMessage)) == 0)
    {
        Parse(sMessage + 4, m_fDistance);
    }
    if (stricmp("offset", firstword(sMessage)) == 0)
    {
        Parse(sMessage + 6, m_vOffset);
    }
    if (stricmp("yaw", firstword(sMessage)) == 0)
    {
        Parse(sMessage + 3, m_fYaw);
    }
    if (stricmp("pitch", firstword(sMessage)) == 0)
    {
        Parse(sMessage + 5, m_fPitch);
    }
    if (stricmp("tilt", firstword(sMessage)) == 0)
    {
        Parse(sMessage + 4, m_fTilt);
    }
    if (stricmp("speed", firstword(sMessage)) == 0)
    {
        Parse(sMessage + 5, m_fSpeed);
    }
    if (stricmp("minyaw", firstword(sMessage)) == 0)
    {
        Parse(sMessage + 6, m_fMinYaw);
    }
    if (stricmp("maxyaw", firstword(sMessage)) == 0)
    {
        Parse(sMessage + 6, m_fMaxYaw);
    }
    if (stricmp("minpitch", firstword(sMessage)) == 0)
    {
        Parse(sMessage + 8, m_fMinPitch);
    }
    if (stricmp("maxpitch", firstword(sMessage)) == 0)
    {
        Parse(sMessage + 8, m_fMaxPitch);
    }
    if (stricmp("mindist", firstword(sMessage)) == 0)
    {
        Parse(sMessage + 7, m_fMinDistance);
    }
    if (stricmp("maxdist", firstword(sMessage)) == 0)
    {
        Parse(sMessage + 7, m_fMaxDistance);
    }
    if (stricmp("clampyaw", firstword(sMessage)) == 0)
    {
        Parse(sMessage + 8, m_fClampYaw);
    }
    if (stricmp("clamppitch", firstword(sMessage)) == 0)
    {
        Parse(sMessage + 10, m_fClampPitch);
    }
    if (stricmp("clampdist", firstword(sMessage)) == 0)
    {
        Parse(sMessage + 9, m_fClampDistance);
    }
}
// PC 0x004CC2B0
void CAurBehaviorAttach::SetScene()
{
    if (m_pParent != 0)
    {
        ((Gob*)m_pObject)->SetScene(((Gob*)(CAurObject*)m_pParent)->m_pScene);
    }
}
// PC 0x004CC2D0
Vector CAurBehaviorCameraFollow::GetTargetWorldPosition()
{
    Gob* pTarget;

    pTarget = (Gob*)(CAurObject*)m_pTarget;
    return pTarget->m_vPosition;
}
// PC 0x004CC300
CAurBehavior::CAurBehavior(CAurObject* pObject)
{
    m_pObject = pObject;
}

#ifdef _XBOX
void CAurBehavior::AddDependency(CAurObject* pObject)
{
    m_lstDependencies.Add(new SafePointer<CAurObject>(pObject));
}
#else
int CAurBehavior::AddDependency(CAurObject* pObject)  // PC 0x004CC320
{
    SafePointer<CAurObject>* pDependency = new SafePointer<CAurObject>(pObject);
    List<SafePointer<CAurObject>*>& lstDependencies = m_lstDependencies;
    lstDependencies.Add(pDependency);
    return lstDependencies.m_nCount;
}
#endif
// PC 0x004CCDC0
CAurBehavior::~CAurBehavior()
{
    int i;
    int nCount;
    SafePointer<CAurObject>* pDependency;

    nCount = m_lstDependencies.m_nCount;
    for (i = 0; i < nCount; i++)
    {
        pDependency = m_lstDependencies.m_pList[i];
        delete pDependency;
    }

    m_lstDependencies.SetSize(0);
}
// PC 0x004CC3B0
void CAurBehavior::RemoveDependency(CAurObject* pObject)
{
    int nIndex;
    int nCount = m_lstDependencies.m_nCount;

    for (nIndex = 0; nIndex < nCount; ++nIndex)
    {
        if (m_lstDependencies.m_pList[nIndex]->m_pObject == pObject)
        {
            delete m_lstDependencies.m_pList[nIndex];
#ifdef _XBOX
            m_lstDependencies.DelIndex(nIndex);
#else
            int i = nIndex;
            m_lstDependencies.m_nCount--;
            for (; i < m_lstDependencies.m_nCount; ++i)
            {
                m_lstDependencies.m_pList[i] =
                    m_lstDependencies.m_pList[i + 1];
            }
#endif
            break;
        }
    }
}
// PC 0x004CC430
int CAurBehavior::Control(float fTimeStep)
{
    SafePointer<CAurObject> pObject(m_pObject);

    for (int i = m_lstDependencies.m_nCount - 1; i >= 0; i--)
    {
#ifdef _XBOX
        Gob* pDependent = (Gob*)m_lstDependencies.m_pList[i]->m_pObject;
        if (pDependent)
        {
            pDependent->Animate(fTimeStep);
        }
        else
        {
            delete m_lstDependencies.m_pList[i];
            m_lstDependencies.DelIndex(i);
        }
#else
        if (m_lstDependencies.m_pList[i]->m_pObject)
        {
            ((Gob*)m_lstDependencies.m_pList[i]->m_pObject)->Animate(fTimeStep);
        }
        else
        {
            delete m_lstDependencies.m_pList[i];
            m_lstDependencies.DelIndex(i);
        }
#endif
    }

    if (!pObject.m_pObject)
    {
        return 0;
    }
    if ((CAurBehavior*)((Gob*)pObject.m_pObject)->GetBehavior(-1) != this)
    {
        return 0;
    }
    return 1;
}
// PC 0x004CC580
int CAurBehaviorAttach::Control(float fTimeStep)
{
    Gob* pGob;
    bool bAnimate = true;
    if (currentscene)
    {
        if (!currentscene->IsAnimating())
        {
            bAnimate = false;
        }
    }

    pGob = (Gob*)m_pObject;
    SafePointer<CAurObject> pObject(m_pObject);

    if (bAnimate)
    {
        if (CAurBehavior::Control(fTimeStep) == 0)
        {
            return 0;
        }
    }

    if ((CAurBehavior*)((Gob*)pObject.m_pObject)->GetBehavior(-1) != this)
    {
        return 1;
    }

    if (m_pParent == 0)
    {
        if (m_nDieWithoutParent)
        {
            delete m_pObject;
            return 1;
        }
        pGob->SetBehavior(0);
        return 1;
    }

    m_pPart->GetWorld(pGob->m_vPosition, pGob->m_qOrientation);
    return 1;
}
// PC 0x004CC6D0
int CAurBehaviorAttachTranslate::Control(float fTimeStep)
{
    int nResult = CAurBehaviorAttach::Control(fTimeStep);
    if (nResult)
    {
        if (m_nWorldSpace)
        {
            ((Gob*)m_pObject)->SetPosition(((Gob*)m_pObject)->GetPosition() +
                m_vTranslation);
        }
        else
        {
            ((Gob*)m_pObject)->SetPosition(((Gob*)m_pObject)->GetPosition() +
                ((Gob*)m_pObject)->GetOrientation() * m_vTranslation);
        }
    }
    return nResult;
}
// PC 0x004CC8C0
int CAurBehaviorAttachRotate::Control(float fTimeStep)
{
    int nResult = CAurBehaviorAttach::Control(fTimeStep);
    if (nResult)
    {
        ((Gob*)m_pObject)->SetOrientation(((Gob*)m_pObject)->GetOrientation() *
            YawPitchRoll(m_vRotation.m_fZ, m_vRotation.m_fX, m_vRotation.m_fY));
    }
    return nResult;
}
// PC 0x004CC9C0
int CAurBehaviorAttachNoOrientation::Control(float fTimeStep)
{
    Quaternion qOrientation = ((Gob*)m_pObject)->GetOrientation();
    int nResult = CAurBehaviorAttach::Control(fTimeStep);
    if (nResult)
    {
        ((Gob*)m_pObject)->SetOrientation(qOrientation);
    }
    return nResult;
}
// PC 0x004CCA20
int CAurBehaviorCameraFollow::Control(float fTimeStep)
{
    if (currentscene)
    {
        currentscene->IsAnimating();
    }

    Quaternion qOrientation;
    if (CAurBehavior::Control(fTimeStep))
    {
        Gob* pTarget = (Gob*)m_pTarget.m_pObject;
        if (!pTarget)
        {
            ((Gob*)m_pObject)->SetBehavior(0);
            return 1;
        }

        m_fYaw += fTimeStep * m_fTurn;
        m_fTurn = 0.0f;
        if (m_fClampYaw != 0.0f)
        {
            m_fYaw = max(m_fMinYaw, min(m_fYaw, m_fMaxYaw));
        }
        m_fPitch += fTimeStep * m_fTilt;
        m_fTilt = 0.0f;
        if (m_fClampPitch != 0.0f)
        {
            m_fPitch = max(m_fMinPitch, min(m_fPitch, m_fMaxPitch));
        }
        m_fDistance *= (float)pow(2.0f, fTimeStep * m_fSpeed * -0.5f);
        m_fSpeed = 0.0f;
        if (m_fClampDistance != 0.0f)
        {
            m_fDistance = max(m_fMinDistance, min(m_fDistance, m_fMaxDistance));
        }

        float fAmount = 1.0f - (float)pow(0.5f, fTimeStep * m_fSpring);
        Vector vPosition;
        vPosition = (1.0f - fAmount) * m_vPosition +
                           fAmount * (pTarget->GetPosition() + m_vOffset);
        qOrientation = YawPitchRoll(m_fYaw, m_fPitch, 0.0f);
        ((Gob*)m_pObject)->SetOrientation(normalize(slerp(
            ((Gob*)m_pObject)->GetOrientation(), qOrientation, fAmount)));
        ((Gob*)m_pObject)->SetPosition(vPosition +
            ((Gob*)m_pObject)->GetOrientation().zdir() * m_fDistance);
        m_vPosition = vPosition;
    }
    return 1;
}

int CAurBehaviorNavigate::Control(float fTimeStep)
{
    if (currentscene)
    {
        currentscene->IsAnimating();
    }

    if (CAurBehavior::Control(fTimeStep))
    {
        Gob* pGob = (Gob*)m_pObject;
        Quaternion qOrientation = pGob->GetOrientation();
        Vector vPosition = pGob->GetPosition();
        vPosition = vPosition + qOrientation.zdir() * (fTimeStep * m_fSpeed);
        m_fSpeed = 0.0f;
        vPosition = vPosition + qOrientation.xdir() * (fTimeStep * m_fStrafe);
        m_fStrafe = 0.0f;
        vPosition.m_fZ += fTimeStep * m_fRaise;
        m_fRaise = 0.0f;
        pGob->SetPosition(vPosition);
        pGob->SetOrientation(qOrientation *
            YawPitchRoll(fTimeStep * m_fTurn, fTimeStep * m_fTilt, 0.0f));
        m_fTurn = 0.0f;
        m_fTilt = 0.0f;
    }
    return 1;
}

int CAurBehaviorCameraOnAStick::Control(float fTimeStep)
{
    if (currentscene)
    {
        currentscene->IsAnimating();
    }

    if (CAurBehavior::Control(fTimeStep))
    {
        Gob* pTarget = (Gob*)m_pTarget.m_pObject;
        if (!pTarget)
        {
            ((Gob*)m_pObject)->SetBehavior(0);
            return 1;
        }

        Vector vPosition = ((Gob*)m_pObject)->GetPosition();
        Vector vTarget = pTarget->GetPosition();
        Vector vDelta(vTarget.m_fX - vPosition.m_fX,
                      vTarget.m_fY - vPosition.m_fY, 0.0f);
        float fLength = magnitude(vDelta);
        float fMove = (fLength - m_fDist) / m_fDist * fTimeStep * m_fSpeed;
        vPosition.m_fX += vDelta.m_fX / fLength * fMove;
        vPosition.m_fY += vDelta.m_fY / fLength * fMove;
        vPosition.m_fZ = m_fHeight;
        float fYaw = (float)(atan2(vDelta.m_fY, vDelta.m_fX) * 57.29577951308232);
        ((Gob*)m_pObject)->SetOrientation(YawPitchRoll(fYaw, 0.0f, 0.0f));
        ((Gob*)m_pObject)->SetPosition(vPosition);
        m_vPosition = vPosition;
    }
    return 1;
}
// PC 0x004CCE60
CAurBehaviorAttach::CAurBehaviorAttach(CAurObject* pObject, CAurObject* pParent,
                                       CAurPart* pPart)
    : CAurBehavior(pObject)
{
    AddDependency(pParent);
    m_pParent = pParent;
    m_pPart = pPart;
    m_nUpdateScenesCallback = 0;
    m_nUpdatePositionCallback = 0;
    m_nColorCallback = 0;
    m_nIlluminationCallback = 0;
    m_nRenderCallback = 0;
    m_nViewVolumeCallback = 0;
    m_nReserved1C = 0;
    m_nDieWithoutParent = 0;

    Gob* pGob = (Gob*)m_pObject;
    pGob->SetScene(0);
    Gob* pParentGob = (Gob*)(CAurObject*)m_pParent;
    pGob->SetScene(pParentGob->m_pScene);
    m_pPart->GetWorld(pGob->m_vPosition, pGob->m_qOrientation);
    pGob->DoEvent("UpdateDependentPosition", 0);
#ifdef _XBOX
    pGob->DoEvent("_EmitterTarget", 0);
    pParentGob->AddAttachment(pGob);
    if (pParentGob->m_fScale != pGob->m_fScale / pGob->m_fBaseScale)
    {
        pGob->SetObjectScale(pParentGob->m_fScale * pGob->m_fBaseScale, 0);
    }
#else
    pParentGob->AddAttachment(pGob);
    if (pGob->m_fScale / pGob->m_fBaseScale != pParentGob->m_fScale)
    {
        pGob->SetObjectScale(pGob->m_fBaseScale * pParentGob->m_fScale, 0);
    }
#endif
    m_nUpdateScenesCallback = ((Gob*)(CAurObject*)m_pParent)->RegisterCallBack(
        UpdateScenesAttach, "*", this, AUR_CALLBACK_LONG_DELAY, 0);
    m_nUpdatePositionCallback = ((Gob*)(CAurObject*)m_pParent)->RegisterCallBack(
        UpdateDependentPosition, "UpdateDependentPosition", this, AUR_CALLBACK_LONG_DELAY, 0);
    m_nColorCallback = ((Gob*)(CAurObject*)m_pParent)->RegisterCallBack(
        PropagateColorShifting,
        "propagatecolorshift", m_pObject, AUR_CALLBACK_LONG_DELAY, 0);
    m_nIlluminationCallback = ((Gob*)(CAurObject*)m_pParent)->RegisterCallBack(
        PropagateIllumination,
        "propagateillumination", m_pObject, AUR_CALLBACK_LONG_DELAY, 0);
    m_nRenderCallback = ((Gob*)(CAurObject*)m_pParent)->RegisterCallBack(
        PropagateRender, "propagaterender", m_pObject, AUR_CALLBACK_LONG_DELAY, 0);
    m_nViewVolumeCallback = ((Gob*)(CAurObject*)m_pParent)->RegisterCallBack(
        PropagateViewVolume, "propagateviewvolume", m_pObject, AUR_CALLBACK_LONG_DELAY, 0);
}
// PC 0x004CD050
CAurBehaviorAttach::~CAurBehaviorAttach()
{
    Gob* pParent = (Gob*)m_pParent.m_pObject;
    if (pParent)
    {
        Gob* pObject = (Gob*)m_pObject;
        if (pObject)
        {
            CAurBehavior* pBehavior = pObject->m_pBehavior;
            CAurBehaviorAttach* pAttach;
            if (pBehavior && (pAttach = pBehavior->AsBehaviorAttach()))
            {
                if (pAttach->m_pParent != m_pParent)
                {
                    pParent->RemoveAttachment((Gob*)m_pObject);
                }
            }
            else
            {
                pParent->RemoveAttachment((Gob*)m_pObject);
            }
        }
        pParent->DeregisterCallback(m_nUpdateScenesCallback);
        pParent->DeregisterCallback(m_nUpdatePositionCallback);
        pParent->DeregisterCallback(m_nColorCallback);
        pParent->DeregisterCallback(m_nIlluminationCallback);
        pParent->DeregisterCallback(m_nRenderCallback);
        pParent->DeregisterCallback(m_nViewVolumeCallback);
    }
}
// PC 0x004CD130
CAurBehaviorAttachTranslate::CAurBehaviorAttachTranslate(CAurObject* pObject,
                                                         CAurObject* pParent,
                                                         CAurPart* pPart)
    : CAurBehaviorAttach(pObject, pParent, pPart)
{
}
// PC 0x004CD170
CAurBehaviorAttachRotate::CAurBehaviorAttachRotate(CAurObject* pObject,
                                                   CAurObject* pParent,
                                                   CAurPart* pPart)
    : CAurBehaviorAttach(pObject, pParent, pPart)
{
}
// PC 0x004CD1B0
CAurBehaviorAttachNoOrientation::CAurBehaviorAttachNoOrientation(
    CAurObject* pObject, CAurObject* pParent, CAurPart* pPart)
    : CAurBehaviorAttach(pObject, pParent, pPart)
{
}

CAurBehaviorNavigate::CAurBehaviorNavigate(CAurObject* pObject)
    : CAurBehavior(pObject)
{
    m_fTurn = 0.0f;
    m_fTilt = 0.0f;
    m_fSpeed = 0.0f;
    m_fRaise = 0.0f;
    m_fStrafe = 0.0f;
}

CAurBehaviorCameraOnAStick::CAurBehaviorCameraOnAStick(CAurObject* pCamera,
                                                       CAurObject* pTarget)
    : CAurBehavior(pCamera)
{
    AddDependency(pTarget);
    m_pTarget = pTarget;
    m_vPosition = ((Gob*)pTarget)->GetPosition();
    m_fDist = 1.0f;
    m_fSpeed = 1.0f;
    m_fHeight = 1.0f;
}
// PC 0x004CD1F0
CAurBehaviorCameraFollow::CAurBehaviorCameraFollow(CAurObject* pCamera,
                                                   CAurObject* pTarget)
    : CAurBehavior(pCamera)
{
    AddDependency(pTarget);
    m_pTarget = pTarget;
    m_vPosition = ((Gob*)pTarget)->GetPosition();
    m_vOffset = Vector(0.0f, 0.0f, 1.0f);
    m_fSpring = 1.0f;
    m_fDistance = 8.0f;
    m_fTurn = m_fYaw = 0.0f;
    m_fPitch = 45.0f;
    m_fTilt = 0.0f;
    m_fSpeed = 0.0f;
    m_fClampYaw = m_fMinYaw = 0.0f;
    m_fClampPitch = m_fMinPitch = 0.0f;
    m_fClampDistance = m_fMinDistance = 0.0f;
    m_fMaxPitch = m_fMaxYaw = 360.0f;
    m_fMaxDistance = 10.0f;
}
// PC 0x004CD310
CAurBehaviorCameraFollow::~CAurBehaviorCameraFollow()
{
}

CAurBehaviorCameraOnAStick::~CAurBehaviorCameraOnAStick()
{
}

CSWMGBehaviorFire::~CSWMGBehaviorFire()
{
}

#ifdef _XBOX
Gob* FindGob(char* sName);
Scene* FindScene(char* sName);

CAurBehaviorNavigate::~CAurBehaviorNavigate()
{
}

const char* nullscene(char* sCommand)
{
    Gob* pGob = FindGob(sCommand);
    if (pGob == 0)
    {
        return "hey, no Gob by that name";
    }
    pGob->SetScene(0);
    return "OK";
}

const char* setscene(char* sCommand)
{
    char sGob[64];
    char sScene[64];

    if (sscanf(sCommand, "%s %s", sGob, sScene) != 2)
    {
        return "Must specify gobname and scenename!";
    }
    Gob* pGob = FindGob(sGob);
    if (pGob == 0)
    {
        return "hey, no Gob by that name";
    }
    Scene* pScene = FindScene(sScene);
    if (pScene == 0)
    {
        return "hey, no Scene by that name";
    }
    pGob->SetScene(pScene);
    return "OK";
}

const char* navigate(char* sCommand)
{
    Gob* pGob = FindGob(sCommand);
    if (pGob == 0)
    {
        return "hey, no Gob by that name";
    }
    Scene* pScene = pGob->m_pScene;
    pGob->SetBehavior(new CAurBehaviorNavigate(pGob));
    pGob->SetScene(pScene);
    return "OK";
}
#endif
