#ifndef __AURBEHAVIOR_H__
#define __AURBEHAVIOR_H__

#include "math3d.h"
#include <aurpointer.h>

class CAurObject;
class CAurPart;
class Gob;
class CallBack;
class CAurBehaviorAttach;
class CAurBehaviorCameraFollow;
class CAurBehaviorNavigate;

class CAurBehavior
{
public:

    enum BehaviorType
    {
        TYPE_ATTACH_TRANSLATE = 1002,
        TYPE_ATTACH_ROTATE = 1003
    };

    CAurBehavior(CAurObject* pObject);
    virtual ~CAurBehavior();
    virtual void SetScene() {}  // PC 0x004015A0
    virtual int Control(float fTimeStep);
    virtual void Message(char* sMessage);
    virtual int GetType() { return 0; }  // PC 0x0063E7F0
    virtual CAurBehaviorAttach* AsBehaviorAttach()  // PC 0x0063E7F0
    {
        return 0;
    }
    virtual CAurBehaviorNavigate* AsBehaviorNavigate()  // PC 0x0063E7F0
    {
        return 0;
    }
    virtual CAurBehaviorCameraFollow* AsBehaviorCameraFollow()  // PC 0x0063E7F0
    {
        return 0;
    }

#ifdef _XBOX
    void AddDependency(CAurObject* pObject);
#else
    int AddDependency(CAurObject* pObject);
#endif
    void RemoveDependency(CAurObject* pObject);

    List<SafePointer<CAurObject>*> m_lstDependencies;
    CAurObject* m_pObject;
};

class CAurBehaviorAttach : public CAurBehavior
{
public:
    CAurBehaviorAttach(CAurObject* pObject, CAurObject* pParent,
                       CAurPart* pPart);
    virtual ~CAurBehaviorAttach();
    virtual void SetScene();
    virtual int Control(float fTimeStep);
    virtual void Message(char* sMessage);
    virtual int GetType()  // PC 0x0049FCD0  Xbox 0x000B7130
    {
        return 1001;
    }
    virtual CAurBehaviorAttach* AsBehaviorAttach()  // PC 0x00641DB0
    {
        return this;
    }
    virtual void UpdateGobPosition();

    SafePointer<CAurObject> m_pParent;
    CAurPart* m_pPart;
    int m_nReserved1C;
    int m_nDieWithoutParent;
    CallBack* m_nUpdateScenesCallback;
    CallBack* m_nUpdatePositionCallback;
    CallBack* m_nColorCallback;
    CallBack* m_nIlluminationCallback;
    CallBack* m_nRenderCallback;
    CallBack* m_nViewVolumeCallback;
};

class CAurBehaviorAttachTranslate : public CAurBehaviorAttach
{
public:
    CAurBehaviorAttachTranslate(CAurObject* pObject, CAurObject* pParent,
                                CAurPart* pPart);
    virtual int Control(float fTimeStep);
    virtual void Message(char* sMessage);
    virtual int GetType()  // PC 0x0049FDF0  Xbox 0x002C0120
    {
        return 1002;
    }

    Vector m_vTranslation;
    int m_nWorldSpace;
};

class CAurBehaviorAttachRotate : public CAurBehaviorAttach
{
public:
    CAurBehaviorAttachRotate(CAurObject* pObject, CAurObject* pParent,
                             CAurPart* pPart);
    virtual int Control(float fTimeStep);
    virtual void Message(char* sMessage);
    virtual int GetType()  // PC 0x0049FE30  Xbox 0x002C0170
    {
        return 1003;
    }

    Vector m_vRotation;
};

class CAurBehaviorAttachNoOrientation : public CAurBehaviorAttach
{
public:
    CAurBehaviorAttachNoOrientation(CAurObject* pObject, CAurObject* pParent,
                                    CAurPart* pPart);
    virtual int Control(float fTimeStep);
    virtual int GetType()  // PC 0x0049FE70  Xbox 0x002C01B0
    {
        return 1008;
    }
};

class CAurBehaviorNavigate : public CAurBehavior
{
public:
    CAurBehaviorNavigate(CAurObject* pObject);
#ifdef _XBOX
    virtual ~CAurBehaviorNavigate();
#endif
    virtual int Control(float fTimeStep);
    virtual void Message(char* sMessage);
    virtual int GetType()  // Xbox 0x002C0200
    {
        return 1004;
    }
    virtual CAurBehaviorNavigate* AsBehaviorNavigate()
    {
        return this;
    }

    float m_fTurn;
    float m_fTilt;
    float m_fSpeed;
    float m_fRaise;
    float m_fStrafe;
};

class CAurBehaviorCameraOnAStick : public CAurBehavior
{
public:
    CAurBehaviorCameraOnAStick(CAurObject* pCamera, CAurObject* pTarget);
    virtual ~CAurBehaviorCameraOnAStick();
    virtual int Control(float fTimeStep);
    virtual void Message(char* sMessage);
    virtual int GetType()
    {
        return 1007;
    }

    SafePointer<CAurObject> m_pTarget;
    Vector m_vPosition;
    float m_fDist;
    float m_fSpeed;
    float m_fHeight;
};

class CAurBehaviorCameraFollow : public CAurBehavior
{
public:
    CAurBehaviorCameraFollow(CAurObject* pCamera, CAurObject* pTarget);
    virtual ~CAurBehaviorCameraFollow();
    virtual int Control(float fTimeStep);
    virtual void Message(char* sMessage);
    virtual int GetType()  // PC 0x0049FF90  Xbox 0x002C03A0
    {
        return 1005;
    }
    // PC 0x00641DB0
    virtual CAurBehaviorCameraFollow* AsBehaviorCameraFollow()
    {
        return this;
    }

    Vector GetTargetWorldPosition();

    SafePointer<CAurObject> m_pTarget;

    Vector m_vPosition;
    Vector m_vOffset;
    float m_fDistance;
    float m_fSpeed;
    float m_fSpring;
    float m_fYaw;
    float m_fTurn;
    float m_fPitch;
    float m_fTilt;
    float m_fClampYaw;
    float m_fMinYaw;
    float m_fMaxYaw;
    float m_fClampPitch;
    float m_fMinPitch;
    float m_fMaxPitch;
    float m_fClampDistance;
    float m_fMinDistance;
    float m_fMaxDistance;
};

class CAurObject;
class CAurPart;
class Gob;
class Vector;

void UpdateScenesAttach(CAurObject* pObject, char* sEvent, void* pContext);
void UpdateDependentPosition(CAurObject* pObject, char* sEvent, void* pContext);
void PropagateColorShifting(CAurObject* pObject, char* sEvent, void* pContext);
void PropagateIllumination(CAurObject* pObject, char* sEvent, void* pContext);
void PropagateRender(CAurObject* pObject, char* sEvent, void* pContext);
void PropagateViewVolume(CAurObject* pObject, char* sEvent, void* pContext);

#endif
