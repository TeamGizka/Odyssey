#ifndef __NWCOBJECT_H__
#define __NWCOBJECT_H__

#include "swgui.h"
#include "gameobjectarray.h"
class CExoLinkedListInternal;
class CResRef;
class CSWCArea;
class CSWCVisualEffectOnObject;
class Vector;
#include "exobase.h"
#include "exoresman.h"
#include "math3d.h"
#include "nwportrait.h"
#include "nwcanimbase.h"

class CSWCAnimBase;
class CAurScene;
class CAurObject;
class Gob;
class CallBack;
class CExoSoundSource;
class CExoStreamingSoundSource;

class CSWCAreaOfEffectObject;
class CSWCCreature;
class CSWCDoor;
class CSWCItem;
class CSWCModule;
class CSWCPlaceable;
class CSWCProjectile;
class CSWCSoundObject;
class CSWCTrigger;
class CSWSAreaOfEffectObject;
class CSWSArea;
class CSWSCreature;
class CSWSDoor;
class CSWSEncounter;
class CSWSItem;
class CSWSModule;
class CSWSObject;
class CSWSPlaceable;
class CSWSSoundObject;
class CSWSStore;
class CSWCStore;
class CSWSTrigger;
class CSWSWaypoint;

void QueuedCallbackFunction(CAurObject* pObject, char* sEvent, void* pUserData);

struct CSWCFireForgetAnimation
{

    enum { ANIMATION_NONE = 10000, DEFAULT_LENGTH = 500, MIN_REMAINING_LENGTH = 50 };

    CSWCFireForgetAnimation();

    int m_bActive;
    unsigned short m_nAnimation;
    unsigned long m_nEndDay;
    unsigned long m_nEndTime;
    unsigned long m_nLength;
    float m_fSpeed;
    void* m_pData;
};

class CSWCObject : public CGameObject
{
public:

    enum { MODEL_SLOT_DEFAULT = 0xff };

    enum TargetActionMenu
    {
        TARGET_ACTION_MENU_PRIMARY = 0,
        TARGET_ACTION_MENU_SKILLS = 1
    };

    enum { HILITE_CHANNEL_HOSTILE = 0 };

    enum { HILITE_CHANNEL_VISUAL_EFFECT = 3 };

    enum { LOAD_MODEL_FLAG_VISUAL_EFFECT = 8 };

    CSWCObject(unsigned char nObjectType);
    virtual ~CSWCObject();
    virtual void SetId(unsigned long nId)  // PC 0x00616FA0  Xbox 0x000475A0
    {
        m_nId = nId;
    }
    // PC 0x00641DB0
    virtual CSWCObject* AsSWCObject()
    {
        return this;
    }
    // PC 0x0063E730  Xbox 0x00098CC0
    virtual CExoString GetName()
    {
        return CExoString("");
    }
    virtual void AIUpdate() = 0;
    virtual int LoadModel(const CResRef& cModel, unsigned char nModel,
                          unsigned char nFlags) = 0;
    virtual void UnloadModel() = 0;
    virtual void UnloadModel(unsigned char nModel) = 0;
    virtual void SetArea(CSWCArea* pArea);
    virtual void SetOrientation(const Vector& vOrientation);
    virtual void SetPosition(const Vector& vPosition);
    // PC 0x004015A0
    virtual void UpdateWalkMesh()
    {
    }

    virtual void AttachToScene(CAurScene* pScene);
    virtual Gob* GetModel(unsigned char nSlot, int bFlag);
    virtual int SetAnimation(unsigned short nAnimation, int nFlags, int nParam);
    virtual void AddFireForgetAnimation(unsigned short nAnimation, void* pData);
    virtual int Hilite(const Vector& vColor, unsigned char nModel, float fSpeed,
                       int bPulse);
    virtual int UnHilite(unsigned char nModel);
    virtual int SetEnvMap(int nType, int nFlags, const CResRef& cEnvMap);
    virtual int SetEnvMap(unsigned char nModel, int nType, int nFlags,
                          const CResRef& cEnvMap);
    virtual int AddVisualEffect(unsigned short nEffect, int nNode,
                                unsigned long oidCreator,
                                unsigned long oidTarget,
                                unsigned char nBeamFlag, unsigned char nFlags,
                                const Vector& vOffset);
    virtual int DeleteVisualEffect(unsigned short nEffect);
    virtual void KillAllVisualEffects();
    virtual int RefreshVisualEffects(int bFlag);
    virtual void UpdateVisualEffectsList();
    virtual void StopAllVisualEffects();
    virtual int HasVisualEffectApplied(unsigned long nEffect);
    virtual CAurObject* GetCameraFollowPart()  // PC 0x0063E760  Xbox 0x00098CF0
    {
        return m_pAnimBase->GetModel(CSWCObject::MODEL_SLOT_DEFAULT);
    }
    // PC 0x0063E7A0
    virtual int GetFollowingBody()
    {
        return 1;
    }
    // PC 0x00616FE0
    virtual float GetHeadHeightOffset()
    {
        return 1.5f;
    }
    virtual void ActivateHardCodedVisualWithHighestPriority(
        unsigned char nEffectClass, CSWCVisualEffectOnObject* pEffect);
    virtual void DetachVisualEffects();
    virtual void ReAttachVisualEffects();

    virtual void UpdatePopUpBubble();

    virtual void SetPortrait(const CResRef& cPortrait);
    virtual CResRef GetPortrait(unsigned char nSize)  // PC 0x0063E770  Xbox 0x00098D00
    {
        return m_cPortrait.GetPortrait(nSize);
    }
    virtual void SetPortraitId(unsigned short nPortraitId);
    // PC 0x0063E790  Xbox 0x00098D20
    virtual unsigned short GetPortraitId()
    {
        return m_nPortraitId;
    }
    // PC 0x0063E7A0
    virtual int ApplyShadowBlob()
    {
        return 1;
    }

    virtual void RemoveShadowBlob();
    // PC 0x00616FB0  Xbox 0x000475B0
    virtual void ShowShadowBlob()
    {
        if (m_pAnimBase != NULL)
        {
            m_pAnimBase->ShowShadowBlob(0);
        }
    }
    // PC 0x00616FD0  Xbox 0x000475D0
    virtual void HideShadowBlob()
    {
        if (m_pAnimBase != NULL)
        {
            m_pAnimBase->HideShadowBlob();
        }
    }

    virtual void SetGroundNormal(const Vector& vNormal);
    // PC 0x0060B310  Xbox 0x0003B100
    virtual void TurnOnShadows()
    {
        if (m_pAnimBase != NULL)
        {
            m_pAnimBase->TurnOnShadows();
        }
    }
    // PC 0x0060B320  Xbox 0x0003B120
    virtual void TurnOffShadows()
    {
        if (m_pAnimBase != NULL)
        {
            m_pAnimBase->TurnOffShadows();
        }
    }
    // PC 0x004015A0
    virtual void SetQuededAnimationCallback()
    {
    }
    // PC 0x0063E7F0
    virtual int ClearQuededAnimationCallback()
    {
        return 0;
    }
    // PC 0x0063E7B0  Xbox 0x00098D30
    virtual int BarkString(unsigned long nStrRef, const CExoString& sText,
                               const CResRef& cSound, int bFlag)
    {
        return BarkBubble(nStrRef, sText, cSound, bFlag);
    }
    virtual int BarkBubble(unsigned long nStrRef, const CExoString& sText,
                           const CResRef& cSound, int bFlag);
    // PC 0x0063E7C0
    virtual int LipSync(const CResRef& cLip)
    {
        return 0;
    }
    // PC 0x0063E7D0  Xbox 0x00098D40
    virtual void ClientServerSyncHandler_Position(const Vector& vPosition,
                                                      int nFlags, float fTime)
    {
        SetPosition(vPosition);
    }
    virtual void ClientServerSyncHandler_Orientation(const Vector& vOrientation)  // PC 0x0063E7E0  Xbox 0x00098D50
    {
        SetOrientation(vOrientation);
    }
    virtual void ClientServerSyncHandler_Animation(int nAnimation,
                                                   float fSpeed);
    // PC 0x0063E7F0
    virtual int IsHostile()
    {
        return 0;
    }

    enum ReticlePointResult
    {
        RETICLE_POINT_UNAVAILABLE = 0,
        RETICLE_POINT_IN_FRONT = 1,
        RETICLE_POINT_BEHIND = 2
    };
    virtual unsigned char GetReticlePoint(Vector& vPoint);
    virtual float GetCanSeeHeight()  // PC 0x00616FE0  Xbox 0x000475F0
    {
        return 1.5f;
    }
#ifndef _XBOX
    virtual float GetStickyMouseThreshhold();
#endif

    float GetAnimationLength(unsigned short nAnimation);
    void InsertFireForgetAnimation(unsigned short nAnimation,
                                  unsigned long nLength, float fSpeed,
                                  void* pData, unsigned long nDelay);
    int GetTopFireForgetAnimation(unsigned short* pnAnimation, float* pfSpeed,
                                  unsigned long* pnLength, void** ppData);
    void RemoveTopFireForgetAnimation();
    void RemoveAllFireForgetAnimations();
    void AttachmentFromServerObject(CSWSObject* pObject);
    CSWSObject* GetServerObject();
    void ClearAllActions();
    void ClearAllQueuedCombatActions();

    int IsModelLoaded();

    enum FadeState
    {
        FADE_STATE_HIDDEN = 0,
        FADE_STATE_VISIBLE = 1,

        FADE_STATE_FADING_IN = 2,
        FADE_STATE_FADING_OUT = 3
    };
    void SetFadeState(unsigned char nState);
    void SetDesiredFadeState(unsigned char nState, int bFade,
                             unsigned long nDelay, unsigned long nDuration);

    int m_bStoneTexture;
    int m_bBarkTexture;
    int m_bShadeTexture;

    int m_bOtherTexture;
    CExoLinkedListNode* m_pAIListPos;
    CSWCArea* m_pArea;
    CSWCArea* GetArea();
    Vector m_vPosition;
    Vector m_vOrientation;
    unsigned long m_nLastUpdateDay;
    unsigned long m_nLastUpdateTime;
    unsigned long m_nTimeDelta;
    unsigned short m_nCurrentAnimation;
    unsigned short m_nQueuedAnimation;
    float m_fAnimationSpeed;
    int m_bOutOfPlace;
    int m_bPlayedOutOfPlace;
    unsigned short m_nLoopingAnimation;
    unsigned short m_nFireForgetCapacity;
    CSWCFireForgetAnimation* m_pFireForgetAnimations;
    unsigned char m_nFireForgetCount;
    int m_nAnimationTimer;
    CSWCAnimBase* m_pAnimBase;
    CExoLinkedList<CSWCVisualEffectOnObject> m_lstVisualEffects;

    CExoArrayList<unsigned long> m_aObjectIds;

    unsigned long m_nObjectIdState;
    unsigned long m_nFadeStartDay;
    unsigned long m_nFadeStartTime;
    unsigned long m_nFadeDelay;
    unsigned long m_nFadeDuration;
    unsigned char m_nFadeState;
    unsigned char m_nDesiredFadeState;
    int m_bFadeFlag;
    float m_fFadeAlpha;

    int m_bVisibilityUpdated;

    int m_bFadeHold;

    float m_fHeadHitHeight;
    CSWPortrait m_cPortrait;
    unsigned short m_nPortraitId;

    unsigned short m_nIdleAnimation;

    unsigned long m_oidIdleTarget;
    unsigned long m_nIdleDay;
    unsigned long m_nIdleTime;
    CExoSoundSource* m_pVocalization;
    CExoStreamingSoundSource* m_pStreamingSound;
    int m_nVocalizationFlag;
    int m_bShadowsVisible;
    Vector m_vGroundNormal;
    int m_bFadeLocked;
    CExoString m_sFeedbackInfo;
    int m_nFeedbackTime;
    CallBack* m_pQueuedAnimationCallback;
    CSWSObject* m_pServerObject;

    CSWCAnimBase* GetAnimationBase();
    void HidePopUpBubble();
    void KillPopUpBubble();
    void SetPopUpBubbleText(const CExoString& sText, int bShow,
                            float fScale, float fDuration);
    void SetPopUpBubblePortrait(const CResRef& cPortrait);
    void SetPopUpBubbleDefaultText(const CExoString& sText);
    void SetPopUpBubbleDefaultCreatureID(unsigned long oidCreature);
    unsigned short GetCurrentAnimation();
    unsigned short GetLoopingAnimation();
    int EnableBumpedOutShell(CResRef cShell, float fScale);
    int DisableBumpedOutShell();
    void EnableDistortion();
    void DisableDistortion();
    int RestoreEnvMap();
    int RestoreTexture();
    int RestoreTexture(unsigned char nModel);
    int AnimationStationary(unsigned short nAnimation);
    int AnimationPause(unsigned short nAnimation);
    int AnimationWalking(unsigned short nAnimation);
    int AnimationRunning(unsigned short nAnimation);
    int AnimationLooping(unsigned short nAnimation);
    int AnimationPlayOutOfPlace(unsigned short nAnimation);
    int AnimationFireAndForget(unsigned short nAnimation);
    int AnimationOverlay(unsigned short nAnimation);
    int AnimationDialog(unsigned short nAnimation);
    int AnimationParry(unsigned short nAnimation);
    int AnimationAttack(unsigned short nAnimation);
    int AnimationHideEquippedItems(unsigned short nAnimation);
    int AbortFireForgetAnimation(int bAbort);
    void SetFeedbackInfo(const CExoString& sFeedback);
    void GetFeedbackInfo(CExoString& sFeedback, unsigned long& nTime);
    void DetachFromServerObject();
    int SetLoopingAnimation(unsigned short nAnimation);
    int SetAlpha(float fAlpha, unsigned char nFlags);
    int ReplaceTexture(unsigned char nModel, const CResRef& cOld,
                       const CResRef& cNew, int nFlags);
    int ReplaceTexture(const CResRef& cOld, const CResRef& cNew, int nFlags);
    void SetWind(int bEnabled, float fStrength);

    void UpdateFloatyText();
    void NewVocalization(const CResRef& cVocalization, int nParam);
    int CalculateProjectileTimeToTarget(const Vector& vTarget, int bFlag);
    void QueuedCallback(CAurObject* pObject, char* sEvent);
    void AcquireAnimationBase(CSWCObject* pSource);
    void TestAndReEquipweapons(int nFlag, unsigned short nAnimation);
    void DetachVisualEffectsMatchingHardCodedClass(
        CSWCVisualEffectOnObject* pEffect);
    float GetVisualEffectAlpha();
    float GetFinalFadeAlpha();
    int FadeObject();
    void SetObjectVisibilityOnSetArea(CSWCArea* pArea);
#ifdef _XBOX

    void MenuActionUseSecuritySpike(unsigned long oidItem, CSWCCreature* pCreature);
#endif
};

#endif
