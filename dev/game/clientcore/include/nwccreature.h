#ifndef __NWCCREATURE_H__
#define __NWCCREATURE_H__
extern float DriveWalkThreshold;
extern float fCombatTurnSpeed;

#include "exobase.h"
#include "math3d.h"
#include "gameobjectarray.h"
#include "SWCollisionMesh.h"

class CSWGuiInterfaceAction;
class CSWVisibilityNode;

class CSWCCombatAttackData
{
public:

    enum AnimationEvent
    {
        ANIMATION_EVENT_ATTACK = 1,
        ANIMATION_EVENT_SPELL = 4,
        ANIMATION_EVENT_SEQUENCE = 5,
        ANIMATION_EVENT_TIMED = 6
    };
    char m_bHit;
    char m_nAttackState;
    short m_nAttackAnimation;

    unsigned long m_oidCreature;

    union { short m_nAttackRoll; unsigned short m_nAttackDuration; };
    short m_nDamage;
    char m_nAttackResult;
    char m_aAttackPadding0D[3];
    int m_nTargetObjectId;

    unsigned long m_oidAttackObject;

    union { short m_nDamageType; unsigned short m_nAnimationLength;
            unsigned short m_nAnimationDelay; };
    union { short m_nDamageAmount; unsigned short m_nNextAnimation; };
    short m_nCriticalMultiplier;
    short m_nAttackFlags;
    int m_nImpactVisual;
    int m_nImpactX;
    char m_nImpactY;
    char m_aImpactPadding29[3];
    Vector m_vImpactPosition;
    int m_nAttackIndex;
    int m_nWeaponIndex;

    CSWCCombatAttackData* m_pNextAttack;

    union { int m_nAttackSequence; float m_fAnimationSpeed; };
    enum { EVENT_ANIMATION_TRANSITION = 5 };
    CSWCCombatAttackData();
    ~CSWCCombatAttackData();
    void Copy(CSWCCombatAttackData*);
};

class CSWCCreaturePartyStatus
{
public:
    int m_nUnknown00;
    int m_nUnknown04;
    int m_nUnknown08;
    int m_nUnknown0C;
    int m_nUnknown10;
    int m_nUnknown14;
    CExoString m_sUnknown18;
    CSWCCreaturePartyStatus();
};

#include "swgui.h"
#include "nwcobject.h"
#include "exoresman.h"
#include "math3d.h"
#include <aurpointer.h>
#include "aurmodel.h"
#include "nwccreatureappearance.h"

class CSWCLevelUpStats;
class CSWCCombatAttackData;
class CSWCSpellCastVisual;
class CExoSoundSource;
class CSoundSet;
class CSWVisibilityNode;
struct STR_RES;
class CAurScene;
class CSWGui3DSceneView;
class CSWSCreature;
class CCAppearanceInfo;
class CSWCAnimBase;
class CAurObject;
class CResRef;
class CSWCCreatureAppearance;
class CSWSItem;
class CSWCStats_SpellLikeAbility;
class CSWVisibilityNode;

void SwitchWeaponEvent(CAurObject* pObject, char* sEvent, void* pContext);
void FootstepEvent(CAurObject* pObject, char* sEvent, void* pData);
void HitEvent(CAurObject* pObject, char* sEvent, void* pData);
void HitGroundEvent(CAurObject* pObject, char* sEvent, void* pData);
void SwingShortEvent(CAurObject* pObject, char* sEvent, void* pData);
void SwingLongEvent(CAurObject* pObject, char* sEvent, void* pData);
void SwingTwirlEvent(CAurObject* pObject, char* sEvent, void* pData);
void HitClashEvent(CAurObject* pObject, char* sEvent, void* pData);
void HitParryEvent(CAurObject* pObject, char* sEvent, void* pData);
void Blur(CAurObject* pObject, char* sEvent, void* pData);
void Unblur(CAurObject* pObject, char* sEvent, void* pData);
void GetPersonalRadius(CAurObject* pObject, char* sEvent, void* pData);
void GetCreatureRadius(CAurObject* pObject, char* sEvent, void* pData);

#define SWCCREATURE_FOOTSTEP_CALLBACK_DELAY 50.0f

#define SWCCREATURE_DRIVE_CORNER_OFFSET 0.06f
#define SWCCREATURE_WAYPOINT_LINE_DISTANCE 0.015625f
#define SWCCREATURE_WAYPOINT_LINE_WIDTH 0.015625f
#define SWCCREATURE_DRIVE_REVERSE_CORNER_OFFSET 0.03f
#define SWCCREATURE_TRIANGLE_RAY_HEIGHT 1.5f
#define SWCCREATURE_WALK_TRY_COLOR_STEP 0x1DE20000
#define SWCCREATURE_WALK_TRY_COLOR_MASK 0x01000000
#define WALK_TRY_SQUARE_COUNT 8
#define DRIVE_WALK_LAST_TRY 5

#define SWCCREATURE_POSITION_EPSILON_SQUARED 0.00001f
#define SWCCREATURE_POSITION_SNAP_DISTANCE_SQUARED 36.0f

#define SWCCREATURE_LOCK_DIRECTION_EPSILON_SQUARED 0.00001f

#define SWCCREATURE_SPEED_RAMP_TIME 1000.0f

#define SWCCREATURE_RUNNING_DRIVE_ACCELERATION 15.0f
#define SWCCREATURE_FAST_SPEED_RAMP_TIME 500.0f
#define SWCCREATURE_SLOWDOWN_SPEED_DIVISOR 2000000.0f
#define SWCCREATURE_MIN_SPEED_FACTOR 0.1f
#define SWCCREATURE_MIN_SLOWDOWN_RATIO 0.01f

#define SWCCREATURE_FOLLOW_SPEED_EPSILON 0.0001f

#define SWCCREATURE_WEAPON_OVERLAY_INTERVAL 2000
#define SWCCREATURE_CORPSE_GROUND_PITCH_MIN 65.0f
#define SWCCREATURE_CORPSE_GROUND_PITCH_MAX 80.0f
#define SWCCREATURE_GROUND_PITCH_MIN 40.0f

#define SWCCREATURE_SELF_FORCE_POWER_DISTANCE_SQUARED 0.0001f

#define SWCCREATURE_NASTY_ROOT_ROTATION (-HALF_PI)
#define SWCCREATURE_NASTY_EFFECT_HEIGHT 0.4f

#define __SWCCREATURE_CLASS_DEFINED__

class CSWCCreature : public CSWCObject
{
public:
    int LookAt(unsigned long oidTarget, float fTime);
    CSWCCreature();
    virtual ~CSWCCreature();
    int GetPauseReadyAnimation();
    void GetFriendlyInterfaceActions(CSWCObject* pTarget, int nType,
                                    CExoArrayList<CSWGuiInterfaceAction>& lstActions);
    void GetHostileInterfaceActions(CSWCObject* pTarget, int nType,
                                   CExoArrayList<CSWGuiInterfaceAction>& lstActions);

    void AnimationUpdate();
    void AnimateFireAndForget();

    enum FireForgetLength
    {
        FIRE_FORGET_DEFAULT_LENGTH = 500,
        FIRE_FORGET_SPELL_LENGTH = 1330,
        FIRE_FORGET_SEQUENCE_LENGTH = 1000,
        FIRE_FORGET_IDLE_LENGTH = 50
    };

    enum TargetInterfaceTargetType
    {
        TARGET_INTERFACE_INVALID = -1,
        TARGET_INTERFACE_DOOR_OR_CONTAINER = 0,
        TARGET_INTERFACE_TRIGGER = 1,
        TARGET_INTERFACE_FRIENDLY = 2,
        TARGET_INTERFACE_HOSTILE = 3
    };
    char GetTargetInterfaceTargetType(CSWCObject* pTarget);
    void GetTargetActions(CSWCObject* pTarget, int nMenu,
                          CExoArrayList<CSWGuiInterfaceAction>& lActions);

    enum InterfaceTargetType
    {
        INTERFACE_TARGET_NONE = 0,
        INTERFACE_TARGET_DOOR_OR_CONTAINER = 1,
        INTERFACE_TARGET_TRIGGER = 2,
        INTERFACE_TARGET_FRIENDLY = 3,
        INTERFACE_TARGET_HOSTILE = 4
    };
    int GetInterfaceTargetType(CSWCObject* pTarget);
    void UpdateMonkVisuals();

    void HandleConjuringCreature();
    // PC 0x00698BD0
    virtual CSWCCreature* AsSWCCreature()
    {
        return this;
    }

    virtual CExoString GetName();
    virtual void AIUpdate();
    virtual int LoadModel(const CResRef& cModel, unsigned char nModel,
                          unsigned char nFlags);
    virtual void UnloadModel();
    virtual void UnloadModel(unsigned char nModel);
    virtual void SetArea(CSWCArea* pArea);
    virtual void SetPosition(const Vector& vPosition);
    int SetPrecalculatedPosition(const Vector& vPosition);
    void EvaluateNewWayPoints(int nCount, Vector* pWayPoints, int nPathType,
                              const Vector& vPosition);

    virtual void AddFireForgetAnimation(unsigned short nAnimation, void* pData);
    virtual float GetHeadHeightOffset();
    virtual unsigned short GetPortraitId();

    enum { PORTRAIT_INVALID = 0xffff };
    virtual void RemoveShadowBlob();
    virtual int LipSync(const CResRef& cLip);
    virtual void ClientServerSyncHandler_Position(const Vector& vPosition,
                                                  int nFlags, float fTime);
    virtual Gob* GetModel(unsigned char nModel, int bForceAppearanceUpdate);
    void ResetUpdateTimes(unsigned long nDay, unsigned long nTime);
    void UpdateAreaId();
    virtual void UpdatePopUpBubble();
    void SetPopUpBubbleDecayTime(float fDecayTime);
    void SetItemUnEquippedByMe(unsigned long oidItem);
    void ResetDesiredPositionTimer();
    float ComputeSpeedFactor(float fSpeed);

    enum SpeedRampMode { SPEED_RAMP_FAST = 1 };

    void MoveCreatureToPosition(const Vector& vPosition);
    void UpdateRelativePerception(unsigned long oidObject);

    void UpdateRelativePerception(CSWVisibilityNode* pNode);

    void SetMoveOrientation(const Vector& vOrientation);

    void SetDesiredOrientation(const Vector& vOrientation);
    void SetDesiredOrientationForLock();
    void SetLockOrientationToObject(unsigned long oidTarget, int bUnknown);
    void StripEquippedItems();
    void PlaySoundSetSound(unsigned short nSound, int bFlag);
    int GetSoundSetStrres(unsigned short nSound, STR_RES& strRes);
    virtual void SetPortrait(const CResRef& cPortrait);
    void SetSoundSet(unsigned short nSoundSet);
    virtual CResRef GetPortrait(int bAlternate);
    virtual CResRef GetPortrait(unsigned char nSize);
    virtual Vector* GetCurrentWayPoint();
    virtual Vector* GetEndWayPoint();
    virtual Vector* GetStartWayPoint();
    int GetWayPoints(unsigned short* pnWayPointCount, Vector** ppWayPoints,
                     unsigned short* pnCurrentWayPoint);
    void SetWayPoints(unsigned short nWayPointCount, Vector* pWayPoints);
    virtual int RequestModel(const CResRef& cModel, const CResRef& cTexture,
                              unsigned char nAnimType);
    virtual int MoveToNextWayPoint();

    CSWSCreature* GetServerCreature();
    unsigned char GetMenuPoint(Vector& vPoint);

    enum MenuPointResult
    {
        MENU_POINT_IN_FRONT = 1,
        MENU_POINT_BEHIND = 2
    };
    float GetLightSaberThrowRate();
    int GetForceShield();

    enum PersonalActionMenu
    {
        PERSONAL_ACTION_FORCE_POWERS = 0,
        PERSONAL_ACTION_USABLE_ITEMS_1 = 1,
        PERSONAL_ACTION_USABLE_ITEMS_2 = 2,
        PERSONAL_ACTION_USABLE_ITEMS_3 = 3
    };
    enum UsableItemActionMask
    {
        USABLE_ITEM_ACTION_MASK_1 = 1,
        USABLE_ITEM_ACTION_MASK_2 = 2,
        USABLE_ITEM_ACTION_MASK_4 = 4
    };
    void GetPersonalActions(int nMenu, CExoArrayList<CSWGuiInterfaceAction>& lstActions);
    void GetSelfForcePowers(CExoArrayList<CSWGuiInterfaceAction>& lstActions);
    void GetHostileForcePowers(CSWCObject* pTarget,
                              CExoArrayList<CSWGuiInterfaceAction>& lstActions);
    void GetEquipmentForcePowers(CExoArrayList<CSWGuiInterfaceAction>& lstActions,
                                 unsigned long nType, int nExclusion,
                                 unsigned long nItemMask);
    void GetUsableItems(CExoArrayList<CSWGuiInterfaceAction>& lstActions,
                       unsigned char nActionMask);

    void CreateUsableItemEntry(CExoArrayList<CSWGuiInterfaceAction>& lstActions,
                             CSWSItem* pItem, int bEquipped,
                             unsigned char nActionMask);
    void GetHostileItems(CExoArrayList<CSWGuiInterfaceAction>& lstActions,
                         CSWCCreature* pTarget);
    int IsHostileUsableItem(CSWSItem* pItem);
    void CreateHostileItemEntry(CExoArrayList<CSWGuiInterfaceAction>& lstActions,
                               CSWSItem* pItem, CSWCCreature* pTarget);
    int CheckFeat(int nFeat1, int nFeat2, int nFeat3);
    void GetCombatFeatsSkills(CExoArrayList<CSWGuiInterfaceAction>& lstActions,
                             CSWCCreature* pTarget);
    void EnableFeatForMenu(int nFeat,
                           CExoArrayList<CSWGuiInterfaceAction>& lstActions,
                           PFNMENUACTION pfnAction, CSWCCreature* pTarget);
    void ActionMenuUseAttackFeat(unsigned long nFeat, CSWCCreature* pTarget);
    void SaveCharGenCreature();
    CExoString GetSaveCreatureFilename(const CExoString& sFirstName,
                                      const CExoString& sLastName);
    int CanSee(CSWCObject* pObject);
    int IsInjured();
    int GetFreeLookVideoEffect();
    float GetCameraOffset();
    void SetAlignmentGoodEvil(short nAlignment);
    void UpdateShadowBlob();
    virtual int ApplyShadowBlob();
    CSWCCreatureAppearance* GetAppearance() { return m_pAppearance; }
    unsigned long GetEquippedItemID(unsigned long nSlot);
    void BlurWeapon(int bBlur);

    int ResetAnimationState();
    void EquipToInventorySlot(unsigned long nSlot, CSWCItem* pItem);
    void SetEquippedItemID(unsigned long nSlot, unsigned long oidItem);
    void SetLastInstant(unsigned long nSlot, int nLastInstant);
    int GetLastInstant(unsigned long nSlot);
    unsigned long GetAttackingWeapon(int nAttackType);
    void SetAppearance(CSWCCreatureAppearance* pAppearance, unsigned char nMode,
                       int bUpdate);
    const Quaternion& GetOrientation();
    virtual unsigned char GetReticlePoint(Vector& vPoint);
    void RotateCreatureToOrientation(const Vector& vOrientation);

    void FollowAnimationUpdate();

    void DriveAnimationUpdate();
    int IsLeaderUsableItem(CSWSItem* pItem, int nSlot, unsigned char nMask);
    void SwitchMode(int nMode);
    float GetDriveAcceleration();
    int IsDriveAnimationBlocked();
    static const int MODE_DEFAULT;
    static const int MODE_SWDRIVE;
    static const int MODE_SWCOMBAT;

    static const unsigned short CCREATURE_EMOTE_ANIMATION[13];
    static const unsigned short CCREATURE_EMOTE_SOUNDID[13];
    static const unsigned long CCREATURE_EMOTE_STRREF[13];
    static const char* CCREATURE_EMOTE_ICON[13];

    enum { EMOTE_SOUND_INVALID = 0xFFFF };

    enum AppearancePartMask
    {
        APPEARANCE_PART_BODY = 1,
        APPEARANCE_PART_HEAD = 2,
        APPEARANCE_PART_TAIL = 4,
        APPEARANCE_PART_WING = 8
    };
    int SetAppearance(CCAppearanceInfo* pAppearance, unsigned char nMode,
                       int bUpdate);

    enum CGPauseAnimation
    {
        CG_PAUSE_1 = 0,
        CG_PAUSE_2 = 1,
        CG_PAUSE_COUNT = 2
    };
    enum CGGestureAnimation
    {
        CG_GESTURE_GREETING = 0,
        CG_GESTURE_HEAD_TURN_LEFT = 1,
        CG_GESTURE_HEAD_TURN_RIGHT = 2,
        CG_GESTURE_PAUSE_BORED = 3,
        CG_GESTURE_PAUSE_SHORT = 4,
        CG_GESTURE_PAUSE_3 = 5,
        CG_GESTURE_COUNT = 6
    };
    void CGPauseCycle(float fTimeStep, int bInitialize);
    void SetInParty(int bInParty, int bFlag);
    int SetInParty(int bInParty);
    void SetOverrideLightsaberState(int bOverride, int bPowered,
                                    int bShowTransition);
    void SetName(const CExoString& sFirstName, const CExoString& sLastName);
    int SetEngagedState(int nState);
    int IsHostile();
    void DoFootstepAudio(unsigned char nFootstep);

    void DoFootstepVisual(unsigned char nFootstep, int bUpdateGroundHeight);

    enum FootstepVisual
    {
        FOOTSTEP_VISUAL_SPLASH = 0,
        FOOTSTEP_VISUAL_DUST = 1,
        FOOTSTEP_VISUAL_LEAF = 2
    };
    void DoFootstepRumble(int nFootstepType);
    void PlayFootstep();
    int PlayHitSound(CSWCCombatAttackData* pAttackData, int bQueued);
    int PlaySwingSound(const CExoString& sSound,
                       CSWCCombatAttackData* pAttackData, int nSoundType);
    int LoadRollingFootstepSound();
    void PlayRollingFootstepSound(int bRunning);
    void UpdateRollingFootstepSound();
    int ShowLightSaberContactVisual(CSWCCombatAttackData* pAttackData);

    enum { HIT_VISUAL_IMPACT = 1 };
    int SpawnHitVisuals(unsigned char nMode, CSWCCombatAttackData* pAttackData,
                       const Vector& vDirection, const CExoString& sHook);
    int UnHilite(unsigned char nMode);
    virtual int Hilite(const Vector& vColor, unsigned char nModel, float fSpeed,
                       int bPulse);
    int GetCreatureLight(int bRequireEnabled);
    float GetHeadTurnHorizontal();
    float GetHeadTurnVertical();
    void SetNewLoopingCreatureSound(const CExoString& sSound);
    int SelectSpecialAttackAnimation(int nAttackType, int nType2Animation,
                                     int nType3Animation, int nType4Animation);
    int StopAmbientPauseAnim();
    void DeleteLoopingCreatureSound();
    int ResolveAttackIndex(unsigned short nAnimation, int nMode,
                           int nPreviousIndex);
    int IsCreature();
    int InCreatureCombat(CSWCObject* pTarget);
    int UpdateMeleeAttackData(unsigned short nAnimation, int nAttackType, int bInCombat,
                              int nWieldClass);
    int UpdateRangedAttackData(int nA, int nWieldClass);
    int UpdateAttackData(unsigned short* pAnimation, CSWCCombatAttackData* pAttackData);
    unsigned short ResolveReactionAnimation(unsigned short nAnimation, unsigned short nReaction,
                                 int nWieldClass);
    void ResolveSpecialAttackVisuals(int nAttackType, int nWieldClass, int bRanged);

    enum { ATTACK_ANIMATION_INVALID = 0xffff };

    enum { COMBAT_ANIMATION_OVERRIDE_OBJECT_ID = -1 };
    void AttachToScene(CAurScene* pScene);
    void Setup3DScene(CSWGui3DSceneView& cView, CSWCCreature* pCreature,
                      CSWSCreature* pServerCreature, int bFlag);

    unsigned long GetGold() { return m_nGold; }

    unsigned short GetSoundSet() { return m_nSoundSet; }
    CSWCCreaturePartyStatus* GetPartyStatus() { return m_pPartyStatus; }

    CSWCLevelUpStats* GetStats()
    {
        return m_pStats;
    }

    unsigned long GetObjectId()
    {
        return m_nId;
    }

    void PerformCombatFeat(unsigned short nFeat, CSWCCreature* pTarget);
    void PerformUseHostileItem(unsigned long nItem, CSWCCreature* pTarget);
    void PartyMenuUseHostileItem(unsigned long nItem, CSWCCreature* pTarget);
    void ActionMenuAttack(unsigned long nA, CSWCCreature* pInstigator);
    void ActionInitiateDialog(unsigned long nAction, CSWCCreature* pInstigator);
    void ForceMenuAction(unsigned long nAction, CSWCCreature* pTarget);
    void ForceMenuHostileAction(unsigned long nAction, CSWCCreature* pTarget);
    void PartyMenuUseItem(unsigned long nItem, CSWCCreature* pTarget);
    void SetCombatMode(int nMode);
    int IsStealthCapable();

    void SetStealthState(int nState, int bSuppressAnimation);
    void PartyMenuStealth(unsigned long oidTarget, CSWCCreature* pTarget);
    void SetThrownLightsaber(unsigned long oidLightsaber);
    void PerformGiveItem(unsigned long oidItem, unsigned long oidTarget,
                         int nStack);
    void PerformAttack(unsigned long oidTarget);
    void PerformSkill(unsigned char nSkill, unsigned char nUseType,
                      unsigned long oidTarget);
    void PerformCastSpell(unsigned long nSpell, unsigned long nTarget,
                          const Vector& vTargetPos, unsigned char nA,
                          unsigned char nB, unsigned char nC, int nD);
    void ComputeZValue(CSWCArea* pArea, Vector* pPosition, float fUnused,
                       Vector* pUnused);
    void PerformUseItem(unsigned long nItem, int nA, unsigned long nB,
                        const Vector& vPosition);

    virtual void ClientServerSyncHandler_Orientation(const Vector& vOrientation);
    virtual void ClientServerSyncHandler_Animation(int nAnimation,
                                                   float fSpeed);

    enum
    {
        CREATURE_TYPE_UNLIMITED_WIELD = 1,
        CREATURE_TYPE_LIMITED_WIELD = 2
    };
    void SetCreatureType(unsigned char bFlag);

    void UpdateWeaponClass();

    enum
    {
        MONK_VISUAL_APPEARANCE_1 = 4,
        MONK_VISUAL_APPEARANCE_2 = 6,
        ALIGNMENT_SIMPLE_EVIL = 3
    };
    enum
    {
        SPECIAL_ATTACK_VISUAL_KI_1 = 0,
        SPECIAL_ATTACK_VISUAL_KI_2 = 1,
        SPECIAL_ATTACK_VISUAL_STUN = 2,
        SPECIAL_ATTACK_VISUAL_QUIVER = 3
    };
    enum
    {
        SPECIAL_ATTACK_ANIMATION_HAND_1 = 10202,
        SPECIAL_ATTACK_ANIMATION_HAND_2 = 10203,
        SPECIAL_ATTACK_ANIMATION_RIGHT_FOOT_1 = 10204,
        SPECIAL_ATTACK_ANIMATION_LEFT_FOOT_1 = 10205,
        SPECIAL_ATTACK_ANIMATION_HAND_OVERLAY = 10254,
        SPECIAL_ATTACK_ANIMATION_RIGHT_FOOT_2 = 10263,
        SPECIAL_ATTACK_ANIMATION_LEFT_FOOT_2 = 10264,
        SPECIAL_ATTACK_ANIMATION_LEFT_FOOT_3 = 10265
    };
    void LoadMonkVisuals();
    void UnloadMonkVisuals();
    void LoadSpecialAttacks(unsigned char nType);
    void StopSpecialAttacks(unsigned char nType);

    void UpdateVisibleWeapons(unsigned long nSlot, CSWCItem* pItem, int bInstant);
    int RequestAppearance(CCAppearanceInfo* pCAppearanceInfo,
                          unsigned char bFlag);
    virtual void SetPortraitId(unsigned short nPortraitId);
    CSWCAnimBase* SetSpecialWalkAnimation(unsigned char bFlag);
    CSWCAnimBase* SetRunLooksAwful(int nValue);
    void HideEquippedHeadItem();
    void ShowEquippedHeadItem();
    void ShowEquippedItems();
    virtual void TurnOnShadows();
    virtual void TurnOffShadows();
    void ReAttachCamera();
    void ResetCameraToFollow();
    virtual CAurObject* GetCameraFollowPart();
    int GetFollowingBody();
    int GetArmourClass();
    float GetSWAnimationSpeed() { return m_fSWAnimationSpeed; }
    void SetSWAnimationSpeed(float fSpeed, float fTargetSpeed);
    virtual void SetQuededAnimationCallback();
    int ClearQuededAnimationCallback();
    int BarkString(unsigned long nValue, CExoString const& sString,
                   CResRef const& cResRef, int nValue2);
    int StopLipSync();
    int BarkBubble(unsigned long nStrRef, const CExoString& sText,
                   const CResRef& cSound, int nFlags);

    enum AnimationCallbackSlot
    {
        CALLBACK_FOOTSTEP = 0,
        CALLBACK_HIT = 1,
        CALLBACK_HIT_GROUND = 3,
        CALLBACK_SWING_SHORT = 4,
        CALLBACK_SWING_LONG = 5,
        CALLBACK_SWING_TWIRL = 6,
        CALLBACK_HIT_CLASH = 7,
        CALLBACK_HIT_CONTACT = 8,
        CALLBACK_HIT_PARRY = 9,
        CALLBACK_BLUR_START = 10,
        CALLBACK_BLUR_END = 11,
        CALLBACK_DONE_ATTACK_01 = 12,
        CALLBACK_DONE_ATTACK_02 = 13,
        CALLBACK_GET_PERSONAL_RADIUS = 14,
        CALLBACK_GET_CREATURE_RADIUS = 15,
        CALLBACK_GET_PATH = 16
    };
    void RegisterCallbacks();
    void UnRegisterCallbacks();
    void PlayDrawAnimation(unsigned short nValue);
    int ComputeWeaponOverlays(int bCheckAnimations, int bUnused);
    void ClearVisibilityList();
    void HideEquippedItems(CResRef const& cResRef, CResRef const& cResRef2);
    void ForceAppearanceUpdate();
    void CreateNewSpellLikeAbilityList();
    void DeleteSpellLikeAbilityList();
    void SetCombatState(int nValue);

    enum { COMBAT_STATE_ACTIVE = 1 };

    CAurObject* m_apOwnedObjects[12];

    Gob* m_pBloodEffect;

    CAurObject* m_pOwnedObject130;
    CAurObject* m_pOwnedObject134;
    int m_nCombatState;
    int m_nEngagedState;

    int m_bCombatReady;
    unsigned long m_nGold;
    int m_n148;
    int m_n14C;
#ifdef _XBOX
    float m_fUnknown148;
#else
    int m_nUnknown148;
#endif

    int m_bWeaponDrawBlocked;
    int m_nUnknown150;

    int m_nWeaponCheckTimer;
    int m_nWeaponOverlayTimer;
    CAurObject* m_pOwnedObject15C;

    Gob* m_apAttachedModels[4];
    Gob* m_pMonkVisual;
    int m_nMonkVisualState;
    int m_nMonkVisualTimer;
    CExoSoundSource* m_pLoopingSound;

    int m_bLoopingSoundPlaying;
    unsigned short m_nLoopingSoundId;
    unsigned char m_aLoopingSoundPadding[2];

    int m_bDeadSelectable;
    int m_bLoopingSoundEnabled;
    int m_nUnknown190;

    int m_nStealthState;
    int m_nUnknown198;
    int m_nUnknown19C;
    int m_nUnknown1A0;
    int m_nUnknown1A4;
    int m_nUnknown1A8;
    int m_nUnknown1AC;

    int m_bInvisibilityVisualApplied;
    int m_nUnknown1B4;
    int m_nUnknown1B8;
    Vector* m_pWayPoints;
    Vector* m_pCurrentWayPoint;

    Vector m_vMoveOrientation;
    Vector m_vDesiredOrientation;
    Vector m_vDesiredPosition;
    unsigned long m_nDesiredPositionTimer;
    unsigned short m_nWayPointCount;
    unsigned char m_nUnknown1EE;
    unsigned char m_nUnknown1EF;
    unsigned char m_nUnknown1F0;
    unsigned char m_aWayPointStatePadding[3];

    int m_nSpeedRampMode;
    float m_fSpeedFactor;
    float m_fUnknown1FC;
    Vector m_vUnknown200;

    float m_fWalkSpeed;

    float m_fDriveSpeed;
    int m_nUnknown214;
    unsigned char m_nCreatureType;
    unsigned char m_aUnmapped219[0x3];
    CSWCCreatureAppearance* m_pAppearance;

    enum EquippedItemIndex
    {
        EQUIPPED_ITEM_RIGHTHAND = 4,
        EQUIPPED_ITEM_LEFTHAND = 5,
        EQUIPPED_ITEM_RIGHTRING = 7,
        EQUIPPED_ITEM_NECK = 8,
        EQUIPPED_ITEM_ARMS = 10
    };

    enum { EQUIPPED_ITEM_COUNT = 20 };
    enum { EQUIPPED_MENU_ITEM_COUNT = 15 };
    unsigned long m_aoidEquippedItems[EQUIPPED_ITEM_COUNT];

    unsigned long m_oidVisibleRightWeapon;
    unsigned long m_oidVisibleLeftWeapon;
    int m_anLastInstant[EQUIPPED_ITEM_COUNT];
    CSWCCombatAttackData* m_apAttackData[2];

    int m_bConjuringCreature;
    int m_nConjuringSpell;
    unsigned long m_oidUnknown2C8;
    int m_bBlurWeapon;

    unsigned short m_nWeaponOverlayState;

    enum ReadyAnimationState
    {
        READY_ANIMATION_STATE_2 = 2,
        READY_ANIMATION_STATE_3 = 3,
        READY_ANIMATION_STATE_14 = 14
    };
    unsigned char m_nReadyAnimationState;
    unsigned char m_aUnmapped2D3;

    union
    {
        unsigned short m_nMovementFlags;
        struct { unsigned short m_bRunning : 1; unsigned short : 15; };
    };
    unsigned char m_aUnmapped2D6[2];
    unsigned long m_oidCombatTarget;

    enum { HOSTILE_STATE_HOSTILE = 2 };
    unsigned char m_nHostile;
    CResRef m_cUnknown2DD;
    unsigned char m_aResourcePadding[3];
    CSWCSpellCastVisual* m_pSpellCastVisual;

    unsigned long m_oidLockTarget;
    CSWCLevelUpStats* m_pStats;
    unsigned short m_nSoundSet;
    unsigned char m_aSoundSetPadding[2];

    int m_bWetFootstep;

    float m_fFootstepGroundHeight;
    unsigned long m_nUnknown308;
    CSWCCreaturePartyStatus* m_pPartyStatus;
    unsigned long m_nCameraFollowDelay;
    int m_bCameraFollowPending;
    int m_bCameraFollowSnap;
    unsigned char m_aUnmapped31C[0x3C];

    CSWCAnimBase* m_pPendingAnimBase;
    float m_fPendingSpeedFactor;

    CExoArrayList<CSWVisibilityNode*> m_lstVisibilityList;
    CSoundSet* m_pSoundSet;
    unsigned long m_nUnknown370;
    unsigned long m_nUnknown374;
    unsigned long m_nUnknown378;

    unsigned long m_aoidUnknown37C[4];

    unsigned short m_nAppearance;
    unsigned char m_aUnmapped38E[2];
    unsigned long m_nAmbientPauseTimer;
    unsigned long m_nAmbientPauseDelay;
    int m_bAmbientPause;
    unsigned long m_oidArea;
    CExoArrayList<CSWCStats_SpellLikeAbility> *m_pSpellLikeAbilities;
    int m_bInParty;
    int m_nMode;
    int m_nUnknown3AC;
    float m_fSWAnimationSpeed;
    float m_fSWTargetAnimationSpeed;
    float m_fDriveTurnAngle;

    int m_nFollowAnimation;
    int m_nCurrentAttackIndex;
    int m_nPreviousAttackIndex;
    unsigned long m_oidThrownLightsaber;
    int m_bEquippedItemsHidden;
    int m_bEquippedHeadItemHidden;

    unsigned short m_nServerUpdateValue;
    unsigned char m_aUnmapped3D6[2];
    int m_bOverrideLightsaberState;
    unsigned long m_oidParentCorpse;

    unsigned char m_bDriveRunning;
    unsigned char m_aLookAtStatePadding[3];
    unsigned long m_oidLookAtTarget;
    int m_nAttackAnimationOverride;
    CallBack* m_apAnimationCallbacks[17];
    CExoSoundSource* m_pRollingFootstepSound;

    float m_fRollingFootstepPitchOffset;
    int m_bRollingFootstepPlaying;
    int m_bRollingFootstepRunning;
    unsigned long m_bCombatMode : 1;
    unsigned long m_bLevelUpSent : 1;
    unsigned long : 30;

    unsigned short m_nDrawAnimation;
    unsigned char m_aUnmapped446[2];
#ifndef _XBOX
    float m_fFreeLookTurn;
#endif
    int m_n464;
    int m_n468;
    int m_n46C;
#ifdef _XBOX

    int IsLeaderUsableItem(CSWSItem* pItem, int nSlot);

    void CreateUsableItemEntry(CExoArrayList<CSWGuiInterfaceAction>& lstActions,
                               CSWSItem* pItem, int bEquipped);
    void GetUsableItems(CExoArrayList<CSWGuiInterfaceAction>& lstActions);
    void GetNonCombatFeatsSkills(CExoArrayList<CSWGuiInterfaceAction>& lstActions);
    void GetMines(CExoArrayList<CSWGuiInterfaceAction>& lstActions);
    void ActionMenuSetMine(unsigned long oidItem, CSWCCreature* pTarget);
#endif
};

#endif
