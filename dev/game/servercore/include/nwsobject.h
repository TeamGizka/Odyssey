#ifndef __NWSOBJECT_H__
#define __NWSOBJECT_H__


#define SWSCREATURE_FACING_EPSILON_SQUARED 0.00001f



#define FEEDBACK_TRAP_TRIGGERED 82

#define ANIMATION_KNOCKDOWN_BACK 10005
#define ANIMATION_KNOCKDOWN_FRONT 10007

#include "gameobjectarray.h"

#define NWSCREATURE_RANDOM_WALK_WAIT 3.0f
#define NWSCREATURE_RANDOM_WALK_IDLE_WAIT 15.0f
#define NWSCREATURE_RANDOM_WALK_RANGE 15
#define NWSCREATURE_RANDOM_WALK_OFFSET 7.0f
#define NWSCREATURE_RANDOM_WALK_MINIMUM_FRACTION 0.3f
#define NWSCREATURE_RANDOM_WALK_SHORTEN_FACTOR 0.75f
#define NWSCREATURE_RANDOM_WALK_MINIMUM_DISTANCE 1.0f
#define NWSCREATURE_RANDOM_WALK_MAX_ATTEMPTS 20

#define NWSCREATURE_SNEAK_ATTACK_RANGE 10.0f
#define NWSCREATURE_SNEAK_ATTACK_BACK_ARC 0.707


#define NWSCREATURE_WALK_TARGET_REFRESH_DISTANCE_SQUARED 4.0f

#define OBJECT_UPDATE_DISTANCE_SQUARED 0.01f
#define OBJECT_UPDATE_ORIENTATION_COSINE 0.95f
#define OBJECT_UPDATE_RATE_TOLERANCE 10.0


#define NWSCREATURE_FORCE_FOLLOW_WAIT 0.25f
#define NWSCREATURE_FORCE_FOLLOW_PARTY_WAIT 0.025f
#define NWSCREATURE_FORCE_FOLLOW_POSITION_TOLERANCE 0.1

#define TAKE_ITEM_RUN_DISTANCE_SQUARED 25.0f

#define GIVE_ITEM_RUN_DISTANCE_SQUARED 25.0f

#define NWSCREATURE_DEATH_BROADCAST_DISTANCE_SQUARED 900.0f


#define NWSCREATURE_COMBAT_STATE_PARTY_RANGE 30.0f



#define NWSCREATURE_COMBAT_STEP_RANGE_MARGIN 0.25f
#define NWSCREATURE_COMBAT_STEP_SIDE_HALF_CHORD 0.4f
#define NWSCREATURE_COMBAT_STEP_MAX_DISTANCE 0.8f


#define SPELL_DATA_BROADCAST_RANGE_SQUARED 900.0f


#define SPELL_RANGED_MISS_LONG_DISTANCE_SQUARED 100.0f
#define SPELL_RANGED_MISS_SCATTER_DISTANCE_SQUARED 25.0f


#define SPELL_RANGED_MISS_DISTANCE_STEPS 2000
#define SPELL_RANGED_MISS_ANGLE_STEPS 2094


#define ATTACK_SUMMARY_RANGE_SQUARED 900.0f


#define BREAKDOWN_BROADCAST_RANGE_SQUARED 900.0f


#define RANGED_COUP_DE_GRACE_RANGE_SQUARED 100.0f



#define DIALOG_WAIT_FLAG_CAMERA 0x01
#define DIALOG_WAIT_FLAG_SOUND 0x02
#define DIALOG_WAIT_FLAG_ANIMATION 0x04
#define DIALOG_WAIT_FLAG_FADE 0x08


#define NWSCREATURE_FOLLOW_LINE_RADIUS_MARGIN 0.1f
#define NWSCREATURE_FOLLOW_PATH_SPEED_FACTOR 0.7f


#define NWSCREATURE_FOLLOW_PATH_TIMEOUT 30.0f
#define NWSCREATURE_FOLLOW_DESTINATION_RANGE 0.5f
#define NWSCREATURE_FOLLOW_SAFE_SEARCH_RADIUS 3.0f


#define NWSCREATURE_FOLLOW_PATH_WAIT_DISTANCE_SQUARED 49.0f


#define NWSCREATURE_FOLLOW_RADIUS_TOLERANCE 0.01f


#define NWSCREATURE_POINT_RADIUS_TOLERANCE 0.01f


#define NWSCREATURE_WALK_HEIGHT_MARGIN 0.1f


#define NWSCREATURE_BUMP_MIN_REPUTATION 10
class CExoLinkedListInternal;
class CResRef;
class CSWCCMessageData;
class CSWSDialog;
class CSWSpell;
class CSWPartyFollowDataArray;
class CSWPartyMemberData;
class Vector;
#include "exobase.h"
#include "math3d.h"
#include "nwscreaturestats.h"
#include "nwscreature.h"



#define OBJECT_DIALOG_UNSEEN_HEARING_DISTANCE_SQUARED 100.0f

class CResGFF;
class CResStruct;
class CSWItemProperty;
class CSWGuiInterfaceAction;
class CSWSCreature;
class CSWSDoor;
class CSWSModule;
class CSWSArea;
class CSWSAreaOfEffectObject;
class CSWSDoor;
class CSWSTrigger;
class CSWSPlaceable;
class CSWSWaypoint;
class CSWSEncounter;
class CSWSStore;
class CSWSSoundObject;
class CSWCModule;
class CSWCObject;
class CSWCArea;
class CSWCTrigger;
class CSWCProjectile;
class CSWCPlaceable;
class CSWCAreaOfEffectObject;
class CSWCSoundObject;
class CSWSCombatRound;
class CSWSCombatAttackData;
class CPathfindInformation;

class CSWSJournal;
extern int RenderCombat;
class CSWSCreatureStats;
class CSWSFaction;
class CSWSItem;
class CSWSTrigger;
class CSWSPlaceable;
class CSWSWaypoint;
class CSWSEncounter;
class CSWSStore;
class CSWSSoundObject;
class CSWCCreature;
class CSWCDoor;
class CSWCItem;
class CSWInventory;
class CItemRepository;
class CSWSForcedAction;

class CScriptLocation
{
public:
    CScriptLocation();
    ~CScriptLocation();
    void CopyScriptLocation(CScriptLocation* pLocation);
    int LoadLocation(CResGFF* pRes, CResStruct* pStruct);
    int SaveLocation(CResGFF* pRes, CResStruct* pStruct);
    int Equals(CScriptLocation* pLocation)  // PC 0x005AD880
    {
        return equal(m_vPosition, pLocation->m_vPosition) != 0 &&
               equal(m_vOrientation, pLocation->m_vOrientation) != 0;
    }

    Vector m_vPosition;
    Vector m_vOrientation;
};

#include "nwsvartable.h"
#include "nwsdialog.h"

class CGameEffect;
class CSWSExpression;

class CLoopingVisualEffect
{
public:
    static int GetIsBeam(unsigned short nVisualEffectId);
    int GetIsBeam();

    unsigned short m_nVisualEffectId;
    unsigned long m_oidCreator;
    unsigned char m_nBeamFlag;
};

class CScriptTalent
{
public:

    enum TalentType
    {
        TALENT_TYPE_SPELL = 0,
        TALENT_TYPE_FEAT = 1,
        TALENT_TYPE_SKILL = 2
    };

    CScriptTalent();
    ~CScriptTalent();
    void CopyScriptTalent(CScriptTalent* pTalent);
    int SaveTalent(CResGFF* pRes, CResStruct* pStruct);
    int LoadTalent(CResGFF* pRes, CResStruct* pStruct);
    int Equals(CScriptTalent* pTalent);

    int m_nType;
    int m_nID;
    unsigned char m_nMultiClass;
    unsigned long m_oidItem;
    int m_nItemPropertyIndex;
    unsigned char m_nCasterLevel;
    unsigned char m_nMetaType;
};

class CSWSObjectActionNode
{
public:
    CSWSObjectActionNode();
    ~CSWSObjectActionNode();



    enum { MAX_ACTION_PARAMETERS = 13 };




    enum
    {
        PHYSICAL_ATTACK_TARGET_PARAMETER = 0,
        SPELL_TARGET_PARAMETER = 5,
        ITEM_SPELL_TARGET_PARAMETER = 3,

        SPELL_ID_PARAMETER = 0,
        ITEM_SPELL_ITEM_PARAMETER = 0,


        EQUIP_ITEM_PARAMETER = 0,
        EQUIP_SLOT_PARAMETER = 1,
        EQUIP_FLAGS_PARAMETER = 2,

        UNEQUIP_ITEM_PARAMETER = 0,
        UNEQUIP_FLAGS_PARAMETER = 1
    };

    int m_nActionType;
    int m_anParamType[MAX_ACTION_PARAMETERS];
    void* m_apParam[MAX_ACTION_PARAMETERS];
    unsigned short m_nGroupId;
    unsigned short m_nNumParams;
    int m_nField70;
};



#define SWSOBJECT_COMBAT_ATTACK_MELEE 11
#define SWSOBJECT_COMBAT_ATTACK_RANGED 30
#define SWSOBJECT_SCRIPT_ATTACK_NONE 0
#define SWSOBJECT_SCRIPT_ATTACK_MELEE 9
#define SWSOBJECT_SCRIPT_ATTACK_RANGED 10





#define SWSOBJECT_COMBAT_MODE_PARRY 1
#define SWSOBJECT_COMBAT_MODE_POWER_ATTACK 2
#define SWSOBJECT_COMBAT_MODE_IMPROVED_POWER_ATTACK 3

#define SWSOBJECT_COMBAT_MODE_COUNTERSPELL 4
#define SWSOBJECT_COMBAT_MODE_FLURRY_OF_BLOWS 5
#define SWSOBJECT_COMBAT_MODE_RAPID_SHOT 6
#define SWSOBJECT_SCRIPT_COMBAT_MODE_INVALID 0
#define SWSOBJECT_SCRIPT_COMBAT_MODE_PARRY 1
#define SWSOBJECT_SCRIPT_COMBAT_MODE_POWER_ATTACK 2
#define SWSOBJECT_SCRIPT_COMBAT_MODE_IMPROVED_POWER_ATTACK 3
#define SWSOBJECT_SCRIPT_COMBAT_MODE_FLURRY_OF_BLOWS 4
#define SWSOBJECT_SCRIPT_COMBAT_MODE_RAPID_SHOT 5





#define SWSOBJECT_CAST_SPELL_FAKE_FLAG 0x80000000
#define SWSOBJECT_CAST_SPELL_INSTANT_FLAG 0x40000000


#define NWSCREATURE_NEAREST_VIEW_ORIGIN_OFFSET 4.0f
#define NWSCREATURE_NEAREST_FRONT_DOT 0.866

class CSWSObject : public CGameObject
{
public:
    static const int ACTION_COMPLETE;
    static const int ACTION_FAILED;
    static const int ACTION_IN_PROGRESS;
    static const int ACTION_IN_DELAY;
    static const unsigned long AI_TIME_SLICE;




    enum
    {
        ACTION_PICK_UP_ITEM = 7,

        ACTION_DROP_ITEM = 9,

        ACTION_TAKE_ITEM = 35,


        ACTION_GIVE_ITEM = 34,



        GIVE_ITEM_QUANTITY_ALL = -1,
        ACTION_CHANGE_FACING_POINT = 49,


        ACTION_CLOSE_DOOR = 21,
        ACTION_WAIT = 30,

        ACTION_APPEAR = 52,
        ACTION_DISAPPEAR = 53,


        ACTION_FOLLOW_LEADER = 61,


        ACTION_SURRENDER_TO_ENEMIES = 65,
        ACTION_CHECK_MOVE_TO_FOLLOW_RADIUS = 64,


        ACTION_BARK_STRING = 62,


        ACTION_SPEAK_STRING = 14,


        ACTION_START_CONVERSATION = 24,
        ACTION_PARAM_INTEGER = 1,


        ACTION_PARAM_FLOAT = 2,
        ACTION_PARAM_OBJECT = 3,
        ACTION_PARAM_STRING = 4,
        ACTION_PARAM_SCRIPT_SITUATION = 5,
        ACTION_GROUP_NEW = 0xFFFF,


        ACTION_GROUP_LAST = 0xFFFE
    };





    enum
    {
        ACTION_MOVE_TO_POINT = 1,
        ACTION_PLAY_ANIMATION = 6,
        ACTION_WAIT_FOR_END_OF_ROUND = 16,
        ACTION_ORIENT_CAMERA = 22,
        ACTION_PLAY_SOUND = 23,
        ACTION_PAUSE_DIALOG = 31,
        ACTION_RESUME_DIALOG = 32,
        ACTION_SPEAK_STR_REF = 33,
        ACTION_ENCOUNTER_CREATURE_DESTROY_SELF = 36,
        ACTION_DO_COMMAND = 37,
        ACTION_SET_COMMANDABLE = 47,
        ACTION_PROCESS_TIME_LIMIT = 1000,
        ACTION_QUEUE_WARNING_LIMIT = 75,
        ACTION_QUEUE_CLEAR_LIMIT = 500,
        ACTION_QUEUE_DELETE_LIMIT = 1000,
        EFFECT_WARNING_LIMIT = 500,
        EFFECT_REMOVE_LIMIT = 1000,


        ACTION_JUMP_TO_POINT = 5,
        ACTION_JUMP_TO_OBJECT = 48,

        ACTION_CHECK_INTER_AREA_PATHFINDING = 4,
        ACTION_AREA_WAIT = 60,


        ACTION_CHECK_MOVE_TO_OBJECT = 2,

        ACTION_CHECK_MOVE_TO_POINT = 10,



        ACTION_CHECK_MOVE_AWAY_FROM_OBJECT = 3,

        ACTION_CHECK_MOVE_TO_OBJECT_RADIUS = 17,

        ACTION_CHECK_MOVE_TO_POINT_RADIUS = 18,


        ACTION_CHANGE_FACING_OBJECT = 19,
        ACTION_UNEQUIP_ITEM = 11,

        ACTION_EQUIP_ITEM = 8,


        ACTION_TYPE_INVALID = 0xFFFF,
        ACTION_PHYSICAL_ATTACKS = 12,
        ACTION_CAST_SPELL = 15,
        ACTION_OPEN_DOOR = 20,
        ACTION_DIALOG_OBJECT = 24,
        ACTION_DISARM_MINE = 25,
        ACTION_RECOVER_MINE = 26,
        ACTION_FLAG_MINE = 27,
        ACTION_EXAMINE_MINE = 28,
        ACTION_SET_MINE = 29,
        ACTION_UNLOCK_OBJECT = 38,
        ACTION_LOCK_OBJECT = 39,
        ACTION_USE_OBJECT = 40,
        ACTION_ANIMAL_EMPATHY = 41,
        ACTION_REST = 42,
        ACTION_TAUNT = 43,


        ACTION_CHECK_MOVE_AWAY_FROM_LOCATION = 44,

        ACTION_RANDOM_WALK = 45,
        ACTION_ITEM_CAST_SPELL = 46,
        ACTION_COUNTER_SPELL = 50,


        ACTION_DRIVE = 51,
        ACTION_PICK_POCKET = 54,
        ACTION_FORCE_FOLLOW_OBJECT = 55,


        ACTION_CHECK_FORCE_FOLLOW_OBJECT = 58,
        ACTION_HEAL = 56,
        ACTION_COMBAT = 63,
        ACTION_DOOR_SABER = 67,
        ACTION_STANCE_TOTAL_DEFENSE = 68,
        ACTION_STANCE_MEDITATIVE = 69,
        ACTION_FOLLOW_OWNER = 70,
        ACTION_SWITCH_WEAPONS = 71
    };



    enum
    {
        AQ_ACTION_SPECIAL_WAIT = 24,
        AQ_ACTION_UNEQUIP_ITEM = 27,
        AQ_ACTION_COMBAT = 39
    };


    static const int ANIMATION_COMBAT_READY = 10001;

    static const int ANIMATION_IDLE = 10000;



    #define DIALOG_FOLLOWER_RELOCATE_DISTANCE_SQUARED 900.0f




    enum
    {
        DIRTY_POSITION_MASK = 2
    };




    enum { DIRTY_ANIMATION_INDEX = 0, DIRTY_ORIENTATION_INDEX = 2 };

    CSWSObject(unsigned char nObjectType, unsigned long oidObject, int bFlag);
    unsigned long GetNearestObjectByName(const CExoString& sName,
                                         float fDistance);

    virtual ~CSWSObject();
    virtual CSWSObject* AsSWSObject() { return this; }  // PC 0x00698BD0

    virtual void AIUpdate() = 0;
    virtual int ClearAction(CSWSObjectActionNode* pNode, int bRemove);
    virtual void EventHandler(unsigned long nEventId, unsigned long oidCaller,
                              void* pEventData, unsigned long nCalendarDay,
                              unsigned long nTimeOfDay) = 0;
    virtual void SetAnimation(int nAnimation);
    virtual CResRef GetDialogResref()  // PC 0x004FA0F0
    {
        return CResRef("");
    }
    virtual int GetDialogInterruptable()  // PC 0x0065D0F0
    {
        return 1;
    }
    virtual unsigned char GetGender();
    virtual CExoLocString* GetFirstName()  // PC 0x004FA110
    {
        return &m_locLastName;
    }
    // PC 0x004FA110
    virtual CExoLocString& GetLastName()
    {
        return m_locLastName;
    }
    virtual int GetDead();
    // PC 0x004FA120
    virtual short GetMaxHitPoints(int bBaseOnly) { return (short)m_nMaxHitPoints; }
    virtual short GetCurrentHitPoints(int bBaseOnly);
    virtual void DoDamage(int nDamage);
    virtual int DoDamageReduction(CSWSCreature* pAttacker, int nDamage,
                                  unsigned char nDamagePower,
                                  int bSuppressFeedback, int bAttackDamage);
    virtual int DoDamageResistance(CSWSCreature* pAttacker, int nDamage,
                                   unsigned short nDamageFlags,
                                   int bSuppressFeedback, int bAttackDamage,
                                   int bRemainingDamage);
    virtual int DoDamageImmunity(CSWSCreature* pAttacker, int nDamage,
                                 unsigned short nDamageFlags,
                                 int bSuppressFeedback, int bAttackDamage);
    virtual char GetDamageImmunity(unsigned char nType);
    virtual int GetDamageImmunityByFlags(unsigned short nFlags);
    virtual unsigned long GetEffectSpellId();
    virtual void SetEffectSpellId(unsigned long nSpellId)  // PC 0x005EE1C0
    {
    }
    // PC 0x005EE1C0
    virtual void SetLockOrientationToObject(unsigned long oidTarget)
    {
    }
    // PC 0x004FA130
    virtual unsigned long GetLockOrientationToObject()
    {
        return OBJECT_INVALID;
    }
    // PC 0x004FA140
    virtual CResRef GetPortrait()
    {
        return m_cPortrait;
    }
    // PC 0x004FA170
    virtual void SetPortrait(CResRef const& cResRef)
    {
        m_cPortrait = cResRef;
    }
    virtual unsigned short GetPortraitId()  // PC 0x004FA190
    {
        return m_nPortraitId;
    }
    virtual void SetPortraitId(unsigned short nPortraitId);
    virtual void UpdateEffectPtrs()  // PC 0x004015A0
    {
    }
    virtual void UpdateAttributesOnEffect(CGameEffect* pEffect)  // PC 0x005EE1C0
    {
    }



    enum CastingClassSelector
    {
        CASTING_CLASS_INNATE_FF = 0xFF,
        CASTING_CLASS_INNATE_FE = 0xFE
    };



    enum { SPELL_PROJECTILE_PATH_DEFAULT = 0 };

    enum DamageLevel
    {
        DAMAGE_LEVEL_UNINJURED = 0,
        DAMAGE_LEVEL_SLIGHTLY_INJURED = 1,
        DAMAGE_LEVEL_INJURED = 2,
        DAMAGE_LEVEL_BADLY_INJURED = 3,
        DAMAGE_LEVEL_NEAR_DEATH = 4,
        DAMAGE_LEVEL_DEAD = 5
    };
    unsigned long GetId() { return m_nId; }




    enum PortraitId
    {
        PORTRAIT_ID_RESREF_FIRST = 0xFFFE,
        PORTRAIT_ID_INVALID = 0xFFFF
    };
    int GetAnimation() { return m_nAnimation; }

    int GetPlotObject() { return m_bPlotObject; }
    int GetMin1HP() { return m_bMin1HP; }
    int GetNotBlastable() { return m_bNotBlastable; }
    int GetPartyInteract() { return m_bPartyInteract; }



    float GetAnimationSpeed() { return m_fSyncHeight; }
    float GetMovementRate() { return m_fMovementRate; }
    void SetSpellTargetPosition(const Vector& vPosition) { m_vSpellTargetPosition = vPosition; }  // PC 0x0069FDD0


    void SetLastDamageAmount(int nIndex, int nAmount);
    void SetLastAttackType(unsigned short nAttackType) { m_nAttackType = nAttackType; }
    void SetLastAttackMode(unsigned char nAttackMode) { m_nAttackMode = nAttackMode; }
    unsigned char GetDamageLevel();
    unsigned long SpawnBodyBag();

    enum { BODY_BAG_SPAWN_DELAY = 500 };
    int GetIsPCDying() { return 0; }
    void UpdateEffectList(unsigned long nDay, unsigned long nTime);
    void RunActions(unsigned long nDay, unsigned long nTime,
                   unsigned __int64 nTimer);
    int TerminateAISliceAfterAction(unsigned long nAction);

    CScriptLocation GetScriptLocation();

    void CopyScriptVars(CSWSScriptVarTable* pVarTable);
    CExoString& GetTag() { return m_sTag; }
    void SetTag(const CExoString& sTag)  // PC 0x0055B690
    {
        m_sTag = sTag.LowerCase();
    }
    void AddActionNodeParameter(CSWSObjectActionNode* pNode,
                                unsigned long nIndex, unsigned long nType,
                                void* pData);
    int AnimationStationary(unsigned short nAnimation);
    int AnimationWalking(unsigned short nAnimation);
    void SetDamageImmunity(unsigned short nFlags, char nValue);

    enum { LAST_DAMAGE_TYPE_COUNT = 15 };

    enum { DEAD_HIT_POINTS = -11 };
    int GetLastDamageAmountByFlags(int nFlags);
    void SetLastHostileActor(unsigned long oidActor, int bForce);
    CSWSArea* GetArea();
    unsigned long GetAreaId() { return m_oidArea; }
    void SetArea(CSWSArea* pArea);
    void SetDialogDelay(float fDelay);
    CSWSpell* GetLastSpell();
    void CalculateLastSpellProjectileTime(unsigned char nProjectileType);
    Vector CalculateSpellRangedMissTarget(unsigned long oidSource,
                                           unsigned long oidTarget);
    void BroadcastSpellData(CSWSpell* pSpell, CSWCCMessageData* pData);

    enum
    {
        SPELL_PROJECTILE_FIXED_TIME = 2000,
        SPELL_PROJECTILE_EXTRA_DELAY = 2500
    };
    int ReplyDialog(unsigned long nEntry, unsigned long nReply, int bEscape,
                    unsigned long oidSpeaker, int bSkip);
    int StopDialog();
    int RunDialogOneLiner(const CExoString& sDialogResRef,
                          unsigned long oidTokenTarget);
    int IsSoundPlayingInDialog();
    void StopSoundPlayingInDialog();
    int IsStuntAnimationPlayingInDialog();
    int IsFadingInDialog();
    void SetDialogOwner(unsigned long oidOwner);
    int RemoveObjectFromDialog(unsigned long oidObject);
    void SetDirtyFlag(unsigned long nFlag);
    void SetCurrentHitPoints(int nHitPoints);
    void AttachmentFromClientObject(CSWCObject* pObject);
    void DetachFromClientObject();
    void DeleteDialog();
    void TimeStampCameraAnimation();
    void ClearAllActions(int bClearAll);
    void ApplyEffect(CGameEffect* pEffect, int bLoadingGame, int bLoadingModule);
    int AddUseObjectAction(unsigned long oidObject);
    int AddUnlockObjectAction(unsigned long oidObject, unsigned long oidKey,
                              int bUnknown);
    int AddLockObjectAction(unsigned long oidObject);
    int AddGiveItemActions(unsigned long oidItem, unsigned long oidTarget,
                           int nQuantity, int bAddToQueue);
    int AddTakeItemActions(unsigned long oidItem, unsigned long oidTarget,
                           int bAddToQueue);

    void SaveEffectList(CResGFF* pRes, CResStruct* pStruct);
    void SaveListenData(CResGFF* pRes, CResStruct* pStruct);
    void LoadListenData(CResGFF* pRes, CResStruct* pStruct);
    void RemoveEffectTarget(unsigned long oidTarget);


    void AddEffectTarget(unsigned long oidTarget);
    int SendDialogReplies();
    int StartDialog(unsigned long oidObject, const CExoString& sDialogResRef,
                    int nConversationType);
    int IsDialogDelay();
    int SendDialogEntry(unsigned long nEntry);
    int UpdateDialog();
    void SaveVarTable(CResGFF* pRes, CResStruct* pStruct);
    void LoadVarTable(CResGFF* pRes, CResStruct* pStruct);
    int SaveObjectState(CResGFF* pRes, CResStruct* pStruct);
    unsigned char LoadObjectState(CResGFF* pRes, CResStruct* pStruct);
    void LoadEffectList(CResGFF* pRes, CResStruct* pStruct);
    void SaveActionQueue(CResGFF* pRes, CResStruct* pStruct);
    void LoadActionQueue(CResGFF* pRes, CResStruct* pStruct);
    void AddLoopingVisualEffect(unsigned short nVisualEffectId,
                                unsigned long oidCreator,
                                unsigned char nBeamFlag);
    int AddOpenDoorAction(unsigned long oidDoor, int bSuccess);
    int AddCloseDoorAction(unsigned long oidDoor, int bSuccess);
    int AddDoorSaberAction(unsigned long oidDoor);
    int AddSwitchWeaponsAction(int bInstant);
    void TestActionList();
    void AddAction(
        unsigned long nActionType, unsigned short nGroupId,
        unsigned long nParamType1, void* pParam1, unsigned long nParamType2,
        void* pParam2, unsigned long nParamType3, void* pParam3,
        unsigned long nParamType4, void* pParam4, unsigned long nParamType5,
        void* pParam5, unsigned long nParamType6, void* pParam6,
        unsigned long nParamType7, void* pParam7, unsigned long nParamType8,
        void* pParam8, unsigned long nParamType9, void* pParam9,
        unsigned long nParamType10, void* pParam10, unsigned long nParamType11,
        void* pParam11, unsigned long nParamType12, void* pParam12,
        unsigned long nParamType13, void* pParam13);
    void AddActionToFront(
        unsigned long nActionType, unsigned short nGroupId,
        unsigned long nParamType1, void* pParam1, unsigned long nParamType2,
        void* pParam2, unsigned long nParamType3, void* pParam3,
        unsigned long nParamType4, void* pParam4, unsigned long nParamType5,
        void* pParam5, unsigned long nParamType6, void* pParam6,
        unsigned long nParamType7, void* pParam7, unsigned long nParamType8,
        void* pParam8, unsigned long nParamType9, void* pParam9,
        unsigned long nParamType10, void* pParam10, unsigned long nParamType11,
        void* pParam11, unsigned long nParamType12, void* pParam12,
        unsigned long nParamType13, void* pParam13);
    void RemoveEffect(CGameEffect* pEffect);
    int RemoveEffectById(unsigned __int64 nEffectId);
    int DoSpellImmunity(CSWSObject* pCaster);

    enum SpellResistanceResult
    {
        SPELL_RESISTANCE_UNRESOLVED = -1,
        SPELL_RESISTANCE_IMMUNE = 2
    };
    void RemoveEffectByCreator(unsigned long oidCreator);
    void RemoveEffectByType(unsigned long nSpellId, int nType, int nInteger);

    unsigned long GetAcceptableAction(unsigned long nActionType);
    unsigned short GetAQActionIDByID(unsigned short nActionType, int bSpecialAction);


    unsigned short GetIDByAQActionID(unsigned short nAQActionID);
    int AIActionSpeakStrRef(CSWSObjectActionNode* pNode);
    int AIActionPlayAnimation(CSWSObjectActionNode* pNode);
    int AIActionUnlockObject(CSWSObjectActionNode* pNode);
    int AIActionDoorSaber(CSWSObjectActionNode* pNode);
    int AIActionLockObject(CSWSObjectActionNode* pNode);
    int DoSpellLevelAbsorption(CSWSObject* pCaster);
    enum { SPELL_RESISTANCE_RESISTED = 1 };
    int AIActionSpeak(CSWSObjectActionNode* pNode);
    int AddDoCommandAction(void* pScriptSituation);
    int AIActionBarkString(CSWSObjectActionNode* pNode);
    int AIActionPlaySound(CSWSObjectActionNode* pNode);
    int AIActionDoCommand(CSWSObjectActionNode* pNode);
    int AIActionSetCommandable(CSWSObjectActionNode* pNode);
    int AIActionPauseDialog(CSWSObjectActionNode* pNode);
    int AIActionDialogObject(CSWSObjectActionNode* pNode);
    int AIActionResumeDialog(CSWSObjectActionNode* pNode);
    int AIActionWait(CSWSObjectActionNode* pNode);
    int AIActionSwitchWeapons(CSWSObjectActionNode* pNode);
    int AIActionTakeItem(CSWSObjectActionNode* pNode);
    int AIActionGiveItem(CSWSObjectActionNode* pNode);
    int AIActionCloseDoor(CSWSObjectActionNode* pNode);
    int AIActionOpenDoor(CSWSObjectActionNode* pNode);
    int AIActionUseObject(CSWSObjectActionNode* pNode);

    static const int MAX_HOSTILE_REPUTATION = 10;

    static const int MIN_FRIENDLY_REPUTATION = 90;
    int GetReputation(unsigned long oidObject, int bUnknown);


    enum
    {
        REPUTATION_MIN = 0,
        REPUTATION_MAX = 100,
        REPUTATION_DEFAULT = 50,
        REPUTATION_NO_OVERRIDE = -1
    };


    enum
    {
        AI_STATE_REPUTATION_NEUTRAL = 0,
        AI_STATE_REPUTATION_FRIEND = 1,
        AI_STATE_REPUTATION_ENEMY = 2
    };
    unsigned char GetAIStateReputation(unsigned long oidObject);
    enum { AI_STATE_REPUTATION_DEFAULT = 2 };
    unsigned short GetActionByGroupId(unsigned short nGroupId,
                                      CSWSObjectActionNode** ppNode);
    CExoLinkedListNode* GetPositionByGroupIndex(int nIndex);
    CSWSObjectActionNode* GetNodeById(unsigned short nNodeId);
    int GetNumActionGroups();
    void RemoveGroup(unsigned short nGroupId);
    void SetGroupInterruptable(unsigned short nGroupId, int bInterruptable);
    void DeleteCurrentAIAction();
    void SetOrientation(const Vector& vOrientation);
    void SetPosition(const Vector& vPosition, int bUpdateArea, int bUnused,
                     int bNoDirty);
    void RemoveLoopingVisualEffect(unsigned short nVisualEffectId);

    void AddMatchedExpressionString(const CExoString& sString);
    void ClearMatchedExpressionStrings();
    int TestListenExpression(const CExoString& sString);
    void BroadcastDialog(const CExoString& sString, float fRange,
                         int bCheckVisibility);
    CSWSExpression* GetListenExpressionObj(int nId);
    int HasSpellEffectApplied(unsigned long nSpellId);
    void BroadcastSafeProjectile(unsigned long oidSource, unsigned long oidTarget,
                                 const Vector& vStart, const Vector& vEnd,
                                 unsigned long nProjectileType,
                                 unsigned char nProjectileFlags, int nDelay,
                                 unsigned char nSpellType,
                                 unsigned char nSpellLevel, int nSpellId);
    void BroadcastSafeProjectile(unsigned long oidSource, unsigned long oidTarget,
                                 const Vector& vStart, const Vector& vEnd,
                                 unsigned long nProjectileType,
                                 unsigned char nSpellType,
                                 unsigned long nSpellId,
                                 unsigned char nSpellLevel,
                                 unsigned char nCasterLevel, int nDelay,
                                 int nVisualEffect);
    int GetHasFeatEffectApplied(unsigned short nFeat);

    CSWCObject* GetClientObject();



    enum SpellProjectileType
    {
        SPELL_PROJECTILE_TYPE_NORMAL = 6,
        SPELL_PROJECTILE_TYPE_CUSTOM_PATH = 7
    };
    void SpellCastAndImpact(unsigned long nSpellId, const Vector& vTargetPosition,
                            unsigned long oidTarget, unsigned char nMultiClass,
                            unsigned long oidItem, int bSpellCountered,
                            int bCounteringSpell,
                            unsigned char nProjectilePathType,
                            int bInstantSpell);
    int IsDirtyFlagSet(unsigned long nFlag);
    void ClearDirtyFlag(unsigned long nFlag);
    void ReportOverflow(int nType, int nValue, int nLimit, int nExtra);

    CExoLocString m_locLastName;
    unsigned short m_nNextGroupId;
    unsigned short m_nLastGroupId;
    CExoString m_sTag;
    CResRef m_cPortrait;
    unsigned short m_nPortraitId;
    char m_aUnmapped32[0x2];
    CSWSDialog* m_pDialog;
    unsigned long m_nDialogDelayDay;
    unsigned long m_nDialogDelayTime;
    unsigned long m_nDialogStartDay;
    unsigned long m_nDialogStartTime;
    int m_nDialogState;
    int m_nDialogReplyEntry;
    int m_nDialogPending;
    unsigned long m_oidDialogOwner;
    int m_n58;


    int m_nWaitFlags;
    int m_n60;
    int m_n64;
    unsigned long m_nCameraTimeStamp;
    char m_aUnmapped6C[0x4];
    int m_nCameraAnimState;
    int m_n74;
    int m_nAILevel;
    unsigned char m_c7C;
    char m_aUnmapped7D[0x3];
    char m_aUnmapped7C[0x4];
    unsigned short m_nCurrentAction;
    char m_aUnmapped82[0x2];
    unsigned long m_nUnknown84;
    int m_n88;
    unsigned long m_oidArea;
    Vector m_vPosition;
    Vector m_vOrientation;

    unsigned long m_nLastUpdateDay;
    unsigned long m_nLastUpdateTime;

    unsigned long m_nLastWalkDay;
    unsigned long m_nLastWalkTime;

    float m_fMovementRate;
    unsigned long m_nActionStartDay;
    unsigned long m_nActionStartTime;

    unsigned long m_nC4;
    unsigned long m_nC8;

    unsigned long m_nElapsedTime;
    unsigned long m_nWalkElapsedTime;
    int m_nAnimation;
    float m_fSyncHeight;
    int m_nCurrentHitPoints;
    int m_nMaxHitPoints;
    int m_nTemporaryHitPoints;
    int m_bCommandable;



    int m_nEC;
    int m_nF0;
    int m_nF4;
    int m_bPlotObject;
    CExoLinkedList<CSWSObjectActionNode> m_lstActions;
    CSWSScriptVarTable m_lScriptVars;
    CSWVarTable m_lVarTable;
    CExoArrayList<CGameEffect*> m_lstEffects;
    CExoArrayList<CLoopingVisualEffect*> m_lstLoopingVisualEffects;
    CExoArrayList<unsigned long> m_lstEffectTargets;

    int m_bApplyingLinkedEffect;


    int m_nSavingThrowSpellId;


    int m_nCustomScriptEventId;
    int m_n154;
    unsigned long m_oidLastHostileActor;
    unsigned long m_oidLastAttacker;
    unsigned long m_oidLastDamager;
    unsigned long m_oidLastDisturbed;
    int* m_pLastDamageAmounts;
    unsigned short m_nAttackType;
    unsigned char m_nAttackMode;
    char m_aUnmapped16F[0x1];
    unsigned long m_oidLastWeaponUsed;
    unsigned long m_oidInventoryDisturbItem;
    int m_n178;
    unsigned long m_oidLastSpeaker;

    int m_nConversationType;


    CExoString m_sLastListenText;
    CExoArrayList<CSWSExpression*> m_lstExpressions;
    int m_nMatchedStringsNum;
    int m_bListening;
    CExoArrayList<CExoString*> m_lstMatchedStrings;
    char* m_pDamageImmunity;
    Vector m_vSpellTargetPosition;
    unsigned long m_oidSpellTargetObject;
    int m_nLastSpellId;
    char m_c1C4;
    char m_aUnmapped1C5[0x3];
    unsigned long m_nLastSpellProjectileTime;


    int m_bSafeProjectileBroadcast;
    int m_n1D0;
    unsigned char m_c1D4;


    unsigned char m_c1D5;
    char m_aUnmapped1D6[0x2];
    int m_n1D8;
    int m_n1DC;
    int m_n1E0;
    int m_n208;

    CSWSObjectActionNode* m_pCurrentActionNode;

    unsigned short m_nEffectIterator;
    char m_aUnmapped1EA[0x2];
    int m_n1EC;

    int m_bOpenDoorStarted;
    int m_nDialogReplyState;
    int m_nClientState;
    unsigned long m_aDirtyFlags[1];
    int m_bNotBlastable;
    int m_bMin1HP;
    int m_bDeleteNoFade;
    int m_bKeepCorpse;
    unsigned long m_nFadeOutTime;
    char m_aUnmapped210[0x4];
    int m_bDialogEnded;
    int m_bPartyInteract;


    int m_bDialogFaceSpeaker;
    int m_n24C;
    unsigned char m_c250;
    unsigned char m_c251;
    char m_aUnmapped252[0x2];
    int m_n254;
    int m_n258;
    int m_bDialogSoundStopped;
    int m_n260;
    CSWCObject* m_pClientObject;
    void ClearAllEffects(int bKeepOwnEffects);
    void RemoveSomeEffectsOfDurationType(unsigned short nDurationType);
    void SetFadeOutTime(unsigned long nFadeOutTime);
    void SetDeleteNoFade(int bDeleteNoFade);
    void SetKeepCorpse(int bKeepCorpse);
    void SetListenExpression(const CExoString& sPattern, int nNumber);

public:

    int GetAILevel()
    {
        return m_nAILevel;
    }
    Vector* GetOrientation()
    {
        return &m_vOrientation;
    }
    Vector* GetPosition()
    {
        return &m_vPosition;
    }
    void SetAILevel(int nAILevel)
    {
        m_nAILevel = nAILevel;
    }
    unsigned long GetDialogOwner()
    {
        return m_oidDialogOwner;
    }
    int GetTemporaryHitPoints()
    {
        return m_nTemporaryHitPoints;
    }
    void SetTemporaryHitPoints(int nTemporaryHitPoints)
    {
        m_nTemporaryHitPoints = nTemporaryHitPoints;
    }
    int GetSavingThrowSpellId()
    {
        return m_nSavingThrowSpellId;
    }
    void SetSavingThrowSpellId(int nSavingThrowSpellId)
    {
        m_nSavingThrowSpellId = nSavingThrowSpellId;
    }
    CExoArrayList<unsigned long>* GetEffectTargets()
    {
        return &m_lstEffectTargets;
    }
    CExoArrayList<CLoopingVisualEffect*>* GetLoopingVisualEffects()
    {
        return &m_lstLoopingVisualEffects;
    }
    unsigned long GetLastSpeaker()
    {
        return m_oidLastSpeaker;
    }
    void SetLastSpeaker(unsigned long nLastSpeaker)
    {
        m_oidLastSpeaker = nLastSpeaker;
    }
    unsigned long GetLastAttacker()
    {
        return m_oidLastAttacker;
    }
    void SetLastAttacker(unsigned long nLastAttacker)
    {
        m_oidLastAttacker = nLastAttacker;
    }
    void SetLastDamager(unsigned long nLastDamager)
    {
        m_oidLastDamager = nLastDamager;
    }
    unsigned long GetLastDamager()
    {
        return m_oidLastDamager;
    }
    unsigned long GetLastHostileActor()
    {
        return m_oidLastHostileActor;
    }
    unsigned long GetLastDisturbed()
    {
        return m_oidLastDisturbed;
    }
    void SetLastDisturbed(unsigned long nLastDisturbed)
    {
        m_oidLastDisturbed = nLastDisturbed;
    }
    unsigned long GetInventoryDisturbItem()
    {
        return m_oidInventoryDisturbItem;
    }
    void SetInventoryDisturbItem(unsigned long nInventoryDisturbItem)
    {
        m_oidInventoryDisturbItem = nInventoryDisturbItem;
    }
    void SetListening(int nListening)
    {
        m_bListening = nListening;
    }
    int IsListening()
    {
        return m_bListening;
    }
    int GetCustomScriptEventId()
    {
        return m_nCustomScriptEventId;
    }
    void SetCustomScriptEventId(int nCustomScriptEventId)
    {
        m_nCustomScriptEventId = nCustomScriptEventId;
    }
    int GetLastSpellId()
    {
        return m_nLastSpellId;
    }
    void SetLastSpellId(int nLastSpellId)
    {
        m_nLastSpellId = nLastSpellId;
    }
};




inline void CSWSObject::SetLastDamageAmount(int nIndex, int nAmount)
{
    m_pLastDamageAmounts[nIndex] = nAmount;
}

class CSWSSpellScriptData
{
public:

    enum { GFF_STRUCT_ID = 0x6666 };

    int SaveData(CResGFF* pRes, CResStruct* pStruct);
    int LoadData(CResGFF* pRes, CResStruct* pStruct);

    int m_nSpellId;
    unsigned long m_oidCaster;
    unsigned long m_oidTarget;
    unsigned long m_oidItem;


    Vector m_vTargetPosition;
    CExoString m_sScript;
    unsigned long m_oidArea;
    int m_nUnknown28;
    int m_nFinalForceCost;
};



enum SWObjectType
{
    SW_OBJECT_TYPE_MODULE = 3,
    SW_OBJECT_TYPE_AREA = 4,
    SW_OBJECT_TYPE_CREATURE = 5,
    SW_OBJECT_TYPE_TRIGGER = 7,
    SW_OBJECT_TYPE_ITEM = 6,
    SW_OBJECT_TYPE_PLACEABLE = 9,
    SW_OBJECT_TYPE_DOOR = 10,
    SW_OBJECT_TYPE_AREA_OF_EFFECT = 11,

    SW_OBJECT_TYPE_WAYPOINT = 12,
    SW_OBJECT_TYPE_ENCOUNTER = 13,
    SW_OBJECT_TYPE_STORE = 14,
    SW_OBJECT_TYPE_SOUND = 16
};


class CSWVisibilityNode
{
public:
    CSWVisibilityNode()
        : m_oidObject(OBJECT_INVALID), m_bSeen(0), m_bHeard(0),
          m_nSanctuary(0), m_bInvisible(0)
    {
    }



    enum SanctuaryState
    {
        SANCTUARY_SAVE_NOT_ROLLED = 0,
        SANCTUARY_SAVE_FAILED = 1,
        SANCTUARY_SAVE_SUCCEEDED = 2
    };
    enum VisibilityMask
    {


        VISIBLE_OBJECT_MASK = 1,
        SANCTUARY_STATE_SHIFT = 2,
        SANCTUARY_STATE_MASK = 3,
        SANCTUARY_VISIBILITY_MASK = 0x0c
    };

    enum { SEEN = 0x01, HEARD = 0x02 };
    unsigned long m_oidObject;


    unsigned char m_bSeen : 1;
    unsigned char m_bHeard : 1;
    unsigned char m_nSanctuary : 2;
    unsigned char m_bInvisible : 1;
    unsigned char m_nUnusedFlags : 3;
    unsigned char m_aPadding[3];
};

class CSWSInvitationDetails
{
public:
    unsigned long m_oidCreature;
    unsigned long m_nDay;
    unsigned long m_nTime;
};





struct CEffectIconObject
{
    short m_nIcon;
    CResRef m_sIconResRef;
    int m_bGood;
    unsigned short m_nPriority;
    int m_nGood;
    int m_n20;
};


struct CSWSCreatureAppearanceInfo
{
    unsigned long m_oidAppearanceLeftHand;
    unsigned long m_oidAppearanceRightHand;
    unsigned long m_oidAppearanceArmour;
    unsigned long m_oidAppearanceHead;
    unsigned short m_nAppearance;
    unsigned char m_nAppearancePart0;
    unsigned char m_nAppearancePart1;
    unsigned char m_nAppearancePart2;
    unsigned char m_nAppearancePart3;
    unsigned char m_nAppearancePart4;
    unsigned char m_nAppearancePart5;
    unsigned char m_nAppearancePart6;
    unsigned char m_nAppearancePart7;
    unsigned char m_nAppearancePart8;
    unsigned long m_bUseBackupHead;
    unsigned char m_nAppearanceHead;
};

#pragma pack(push, 4)
class CSWSCreature : public CSWSObject
{
public:

    static const float MOVE_AWAY_RADIUS;
    static const int XP_CALCULATIONS_HENCHMEN_ALLOWED;
    static const float XP_CALCULATIONS_PC_BONUS_AMOUNT;
    static const float XP_CALCULATIONS_HENCHMEN_BONUS_AMOUNT;
    static const unsigned char XP_CALCULATIONS_MAX_XP_SCALE;

    enum
    {
        VISION_SEE_INVISIBLE = 0x01,
        VISION_ULTRAVISION = 0x02,
        VISION_TRUESEEING = 0x04,
        VISION_DARKNESS = 0x08,
        VISION_BLIND = 0x10
    };


    enum EffectBonusCategory
    {
        EFFECT_BONUS_ATTACK = 1,
        EFFECT_BONUS_DAMAGE = 2,
        EFFECT_BONUS_SAVING_THROW = 3,
        EFFECT_BONUS_ABILITY = 4,
        EFFECT_BONUS_SKILL = 5
    };
    enum SavingThrowSelector { SAVING_THROW_FORT = 1, SAVING_THROW_REFLEX = 2, SAVING_THROW_WILL = 3 };

    enum SavingThrowType
    {
        SAVING_THROW_TYPE_NONE = 0,
        SAVING_THROW_TYPE_DEATH = 4,
        SAVING_THROW_TYPE_DISEASE = 5,
        SAVING_THROW_TYPE_FEAR = 8,
        SAVING_THROW_TYPE_MIND_AFFECTING = 10,
        SAVING_THROW_TYPE_POISON = 12,
        SAVING_THROW_TYPE_TRAP = 14,
        SAVING_THROW_TYPE_PARALYSIS = 18
    };


    enum SavingThrowResult
    {
        SAVING_THROW_RESULT_FAILED = 0,
        SAVING_THROW_RESULT_SUCCESS = 1,
        SAVING_THROW_RESULT_IMMUNE = 2
    };
    enum EffectBonusFilter { EFFECT_BONUS_FILTER_ALL = 0xFF };


    enum FilteredEffectType
    {
        FILTERED_EFFECT_ATTACK = 1,
        FILTERED_EFFECT_DAMAGE = 2
    };
    CExoArrayList<CGameEffect*> GetFilteredEffectList(unsigned char nType);

    CSWSCreature(unsigned long oidCreature, int bFlag);
    ~CSWSCreature();
    short GetArmorClass();
    unsigned char GetBodyBagAppearance();
    virtual CSWSCreature* AsSWSCreature() { return this; }  // PC 0x00698BD0
    virtual void AIUpdate();
    void SpawnInHeartbeatPerception(int bForce);
    void ResetHeartbeatTimer();
    void HandleHPRegeneration();
    void HandleFPRegeneration();
    void HandleAlignmentInfluence();
    void HandleAutoBalancing();
    void SetIsPuppet(int bIsPuppet);
    void UpdateMineCheck();
    virtual int ClearAction(CSWSObjectActionNode* pNode, int bRemove);
    virtual void EventHandler(unsigned long nEventId, unsigned long oidCaller,
                              void* pEventData, unsigned long nCalendarDay,
                              unsigned long nTimeOfDay);
    virtual int GetDead();
    virtual void SetPortraitId(unsigned short nPortraitId);
    virtual void UpdateEffectPtrs();
    virtual void UpdateAttributesOnEffect(CGameEffect* pEffect);
    void RemoveFromArea(int bLoading);
    int AcquireItem(CSWSItem** ppItem, unsigned long oidCaller,
                    unsigned long oidRepository, int bFlag);
    void SetId(unsigned long oidCreature);

    enum ActivityMask
    {
        ACTIVITY_STEALTH = 0x01,

        ACTIVITY_DETECT = 0x02,

        ACTIVITY_DIALOG = 0x04,
        ACTIVITY_REST = 0x08,
        ACTIVITY_BIT_4 = 0x10,
        ACTIVITY_BIT_5 = 0x20,
        ACTIVITY_BIT_6 = 0x40,
        ACTIVITY_BIT_7 = 0x80,


        ACTIVITY_BIT_8 = 0x100,
        ACTIVITY_BIT_9 = 0x200,
        ACTIVITY_BIT_10 = 0x400,
        ACTIVITY_BIT_11 = 0x800,
        ACTIVITY_BIT_12 = 0x1000,
        ACTIVITY_BIT_13 = 0x2000,
        ACTIVITY_GROUP_LOW = 0xF8,
        ACTIVITY_GROUP_HIGH = 0x3F00,

        ACTIVITY_GROUP_BASIC = 0x0F,
        ACTIVITY_GROUP_ALL_LOW = 0xFF
    };
    void ClearActivities(int nActivity);
    int GetActivity(int nActivity);
    int GetLastInstantEquip(unsigned long nSlot);
    float GetMovementRateFactor(int bIncludeFeats = 1);
    void SetMovementRateFactor(float fMovementRateFactor);
    void PacifyCreature();
    void PacifyCreature(unsigned long oidCreature);


    enum
    {

        COMBAT_STATE_NOTIFY = 1,
        COMBAT_STATE_EQUIPMENT = 2
    };
    void SetCombatState(int bCombatState, unsigned char bFeedback,
                        int nArg3 = 0);
    void ClearHostileActionsVersus(CSWSCreature* pCreature);
    int AddDisappearActions(int bRemoveFromArea);
    int AddAppearActions();
    void ResetUpdateTimes(unsigned long nDay, unsigned long nTime);
    void UpdateExcitedStateTimer();
    void SetExcitedState(unsigned char nState);
    int IsInjured();
    void AddToVisibleList(unsigned long oidObject, int bSeen, int bHeard,
                          unsigned char nSanctuary, int bInvisible);
    void ClearVisibleList();
    void SetVisibilityList(CExoArrayList<CSWVisibilityNode*>* pList);
    void UpdateVisibleList(unsigned char bFlag);

    enum VisibleListUpdateType
    {
        VISIBLE_LIST_UPDATE_ALL = 0,
        VISIBLE_LIST_UPDATE_PARTY = 1,
        VISIBLE_LIST_UPDATE_NONPARTY = 2
    };
    void DoPerceptionUpdateOnCreature(CSWSCreature* pCreature);

    int AIActionCastSpell(CSWSObjectActionNode* pNode);
    int AIActionCheckMoveAwayFromObject(CSWSObjectActionNode* pNode);
    int AIActionCombat(CSWSObjectActionNode* pNode);
    int AIActionDisarmMine(CSWSObjectActionNode* pNode);
    int AIActionDrive(CSWSObjectActionNode* pNode);
    int DriveUpdateLocation(int bRun);
    int AIActionDropItem(CSWSObjectActionNode* pNode);
    int AIActionExamineMine(CSWSObjectActionNode* pNode);
    int AIActionFlagMine(CSWSObjectActionNode* pNode);
    int AIActionHeal(CSWSObjectActionNode* pNode);
    int AIActionItemCastSpell(CSWSObjectActionNode* pNode);
    int AIActionMoveToPoint(CSWSObjectActionNode* pNode);
    int AIActionPhysicalAttacks(CSWSObjectActionNode* pNode);
    int AIActionPickPocket(CSWSObjectActionNode* pNode);
    int AIActionPickUpItem(CSWSObjectActionNode* pNode);
    int AIActionRecoverMine(CSWSObjectActionNode* pNode);
    int AIActionSetMine(CSWSObjectActionNode* pNode);
    int AIActionTaunt(CSWSObjectActionNode* pNode);
    void BroadcastAttackOfOpportunity(unsigned long oidTarget);
    void BroadcastAttackDataToParty(CSWCCMessageData* pData,
        CSWSCombatAttackData* pAttack);

    enum { ACTION_MANAGER_DIALOG = 2, ACTION_MANAGER_REST = 8 };
    void RecomputeAmbientAnimationState();
    void RemoveFromVisibleList(unsigned long oidObject);
    int CheckVisibleList(unsigned long oidObject);
    int IsPartyVisible();
    CSWVisibilityNode* GetVisibleListElement(unsigned long oidObject);
    CSWSJournal* GetJournal();
    void LoadJournal(CResGFF* pRes, CResStruct* pStruct);
    int WalkUpdateLocation_QuickWalk(int bRun);
    int WalkUpdateLocation_QuickWalk_FollowLeader();
    int WalkUpdateLocation_QuickWalk_ForcePushed();
    int WalkUpdateLocation_QuickWalk_FollowLeader_Offset(CSWPartyMemberData*,
        CSWPartyFollowDataArray*, float&, int);
    int WalkUpdateLocation_QuickWalk_FollowLeader_Following_Path(CSWPartyMemberData*,
        CSWPartyFollowDataArray*, float&, int);
    int WalkUpdateLocation_QuickWalk_FollowLeader_Moving_To_Offset(CSWPartyMemberData*,
        CSWPartyFollowDataArray*, float&, int);
    int WalkUpdateLocationTestDistance(const Vector& vStart, const Vector& vEnd,
                                       Vector* pvHitPosition, unsigned long* poidHitCreature);
    int BumpFriends(CSWSCreature* pCreature, const Vector& vEnd, const Vector& vStart);
    int WalkUpdateLocation();
    int WalkUpdateLocationDistance(float fDistance, Vector* pPosition,
                                   Vector* pDirection,
                                   CExoArrayList<unsigned long>* pIntersectList);
    int WalkUpdateLocation_QuickWalk_FollowLeader_TeleportForward(
        CSWPartyMemberData* pMember, CSWPartyFollowDataArray* pPaths,
        float& fDistance, int bRun);
    int WalkUpdateLocation_QuickWalk_FollowLeader_OffsetWaitDest(
        CSWPartyMemberData* pMember, CSWPartyFollowDataArray* pPaths,
        float& fDistance, int bRun);
    int WalkUpdateLocation_QuickWalk_FollowLeader_PathWaitDest(
        CSWPartyMemberData* pMember, CSWPartyFollowDataArray* pPaths,
        float& fDistance, int bRun);
    int WalkUpdateLocation_QuickWalk_FollowLeader_ToOffsetWaitDest(
        CSWPartyMemberData* pMember, CSWPartyFollowDataArray* pPaths,
        float& fDistance, int bRun);
    int WalkUpdateLocation_QuickWalk_FollowLeader_ToPathWaitDest(
        CSWPartyMemberData* pMember, CSWPartyFollowDataArray* pPaths,
        float& fDistance, int bRun);
    int WalkUpdateLocation_QuickWalk_FollowLeader_Moving_To_Path(
        CSWPartyMemberData* pMember, CSWPartyFollowDataArray* pPaths,
        float& fDistance, int bRun);
    int WalkUpdateLocation_QuickWalk_FollowLeader_FindPath(
        CSWPartyMemberData* pMember, CSWPartyFollowDataArray* pPaths,
        float& fDistance, int bRun);
    int WalkUpdateLocation_QuickWalk_FollowLeader_WaitingForPath(
        CSWPartyMemberData* pMember, CSWPartyFollowDataArray* pPaths,
        float& fDistance, int bRun);
    unsigned char CanUnEquipWeapon(CSWSItem* pItem);
    unsigned char CanUnEquipWeapon2(CSWSItem* pItem);
    void SetLockOrientationToObject(unsigned long oidTarget, int bUnknown);
    void SetAnimation(int nAnimation);
    void SetCombatMode(unsigned char nMode, int bUpdate);
    void ActionManager(unsigned long nAction);
    int AddDriveAction(unsigned short nGroupId, const Vector& vPosition,
                       int nDirection, int nDriveMode, int nFlags,
                       int nUnknown);
    float ComputeSpellRange(CExoString sRange, unsigned long oidTarget);
    int AddPathfindingWaitActionToFront(unsigned short nGroupId);
    void SetSilentMove(int bSilentMove);
    void RemoveSpellActionFromRound();
    int UpdateSpecialAttacks();
    void ComputeAIState();
    float CalculateDeathExperience();
    void ApplyDeathExperience();


    void DoCombatStep(unsigned char nStep, int nTime, unsigned long oidTarget);


    enum CombatStep
    {
        COMBAT_STEP_ADJUST_RANGE = 0,
        COMBAT_STEP_AUTOMATIC = 1,
        COMBAT_STEP_FORWARD = 2,
        COMBAT_STEP_BACKWARD = 3,
        COMBAT_STEP_LEFT = 4,
        COMBAT_STEP_RIGHT = 5
    };
    void ComputeAIStateOnAction(int nAction, unsigned long oidTarget);
    CSWSFaction* GetFaction();




    CSWSCreatureStats* GetStats() { return m_pStats; }

    int GetFactionId() { return GetStats()->m_nFactionId; }
    void SetFactionId(int nFactionId) { m_pStats->m_nFactionId = nFactionId; }

    int ResolveDefensiveEffects(CSWSObject* pTarget, int bTouchAttack);
    int IsProjectileParryable(CSWSObject* pTarget, int bCheckWeapon,
                             int bCheckShield, int& nForceShield,
                             int& bCanReflect, int nWeaponHand);


    CPathfindInformation* GetPathfindInformation() { return m_pPathfindInformation; }
    void SetScript(int nScript, const CExoString& sScript) { m_sScripts[nScript] = sScript; }
    int GetCreatureReputation(unsigned long oidCreature, int nFaction);
    void BroadcastBreakdownMessage(unsigned char nMessageType,
                                  CSWCCMessageData* pMessageData);
    void SendAttackSummaryFeedback(unsigned long oidTarget);
    void SetAttackSummaryFeedbackEffectData(int nState, unsigned long oidTarget);
    void BroadcastAttackSummaryToParty(CSWCCMessageData* pMessageData);
    void BroadcastDeathDataToParty(CSWCCMessageData* pMessageData);
    void BroadcastAttackBreakdown(unsigned long oidTarget,
                                  CSWCCMessageData* pMessageData);
    void BroadcastThreatBreakdown(unsigned long oidTarget,
                                  CSWCCMessageData* pMessageData);
    void BroadcastDefenseBreakdown(unsigned long oidTarget,
                                   CSWCCMessageData* pMessageData);
    void BroadcastDamageBreakdown(unsigned long oidTarget,
                                  CSWCCMessageData* pMessageData);
    void BroadcastEffectApplicationBreakdown(unsigned long oidTarget,
                                             CSWCCMessageData* pMessageData);
    void BroadcastDeflectBreakdown(unsigned long oidTarget,
                                   CSWCCMessageData* pMessageData);
    unsigned long GetDamageFlags();
    int ReplyToInvitation(int nReply, unsigned long oidInviter);
    int CancelAction(unsigned short nGroupId, unsigned short nActionId);
    int RemoveAction(unsigned long nAction);
    void DumpToLog();
    int CalculateDamagePower(CSWSObject* pObject);
    int AddMoveToPointAction(unsigned short nGroupId, const Vector& vPosition,
                             unsigned long oidArea, unsigned long oidTarget,
                             int bRun, float fRange, float fTimeout,
                             int bUsePath, int nMovementRate, int bUnknown1,
                             int bClientMoving, int bUnknown2, int bUnknown3,
                             const Vector& vOrientation,
                             unsigned long nUnknown4, unsigned long nUnknown5,
                             int bUnknown6);
    int AddDropItemActions(unsigned long oidItem, const Vector& vPosition,
                           int bDropSingleItem);






    enum { SPELL_ACTION_OPTION10_UNSET = -1, SPELL_ACTION_OPTION11_UNSET = 255 };
    int AddCastSpellActions(unsigned long nSpellId, int nMultiClass,
                            int nSpellOption1, int nSpellOption2,
                            int nSpellOption3, Vector vTarget,
                            unsigned long oidTarget, int nSpellOption4,
                            int nSpellOption5, int nSpellOption6,
                            int nSpellOption7, unsigned char nSpellOption8,
                            int nSpellOption9, int nSpellOption10,
                            unsigned char nSpellOption11, int nSpellOption12);

    int AddUseTalentOnObjectActions(int nTalentType, int nTalentId,
                                    unsigned long oidTarget,
                                    unsigned char nMultiClass,
                                    unsigned long oidItem, int nItemProperty,
                                    unsigned char nSpellOption11,
                                    unsigned char nSpellOption2);
    int AddUseTalentAtLocationActions(int nTalentType, int nTalentId,
                                      const Vector& vTarget,
                                      unsigned char nMultiClass,
                                      unsigned long oidItem, int nItemProperty,
                                      unsigned char nSpellOption11,
                                      unsigned char nSpellOption2);
    int UseItem(unsigned long oidItem, unsigned char nItemProperty,
                 unsigned char nItemOption, unsigned long oidTarget,
                 const Vector& vTarget, unsigned long oidArea, int bItemOption);


    int AddItemCastSpellActions(unsigned long oidItem, int nItemProperty,
                                int nItemOption, Vector vTarget,
                                unsigned long oidTarget, int bItemOption,
                                int bTargetLocation, int bCombatAction,
                                int bInstant);
    int AddMineActions(unsigned long oidMine, unsigned char nAction,
                        unsigned long oidTarget, const Vector& vTarget,
                        unsigned long oidArea);


    enum { ITEM_PROPERTY_INVALID = -1 };
    int AddPickUpItemActions(unsigned long oidItem, unsigned long oidRepository,
                             unsigned char nPanel);
    int TransferGold(unsigned long oidTarget, const Vector& vPosition,
                     unsigned long nAmount);
    int GetTotalEffectBonus(unsigned char nEffectType, CSWSObject* pObject,
                            int& nCapped, int nA, int nB,
                            unsigned char nSavingThrow, unsigned char nC,
                            unsigned char nD, unsigned char nE, int nF,
                            int* pnDamageBonus = NULL);
    int IsCreature();
    CItemRepository* GetItemRepository(int bParty);
    int GetItemCount(int bDropableOnly, int bRepositoryOnly, int bNonPlotOnly);
    void SplitItem(CSWSItem* pItem, int nCount);
    int EvaluateLock(CSWSObject* pObject, int bLocked);

    void ActivityManager(unsigned long nAction);



    enum ActivityAction
    {

        ACTIVITY_ACTION_CLEAR_MODES = 1,
        ACTIVITY_ACTION_OBJECT_INTERACTION = 2,
        ACTIVITY_ACTION_COUNTERSPELL = 4
    };
    int GetUseRange(unsigned long oidObject, Vector& vPosition,
                    float& fRange, int bFlag, int nArg5 = 0);
    int GetIsInUseRange(unsigned long oidObject, float fRange, int bFlag,
                        int nArg4 = 0);
    int AddCounterSpellActions(unsigned long oidTarget);
    int AIActionCounterSpell(CSWSObjectActionNode* pAction);

    void BroadcastCombatStateToParty(unsigned long oidTarget);


    enum MoveFlags
    {
        MOVE_USE_PATH_SHIFT = 1,
        MOVE_TIMEOUT_MASK = 4,
        MOVE_OPTION1_INPUT_MASK = 7,
        MOVE_OPTION1_SHIFT = 4,
        MOVE_OPTION2_SHIFT = 3,
        MOVE_CLIENT_INPUT_MASK = 3,
        MOVE_CLIENT_SHIFT = 7,
        MOVE_OPTION3_INPUT_MASK = 1,
        MOVE_OPTION3_SHIFT = 9,
        MOVE_OPTION6_INPUT_MASK = 1,
        MOVE_OPTION6_SHIFT = 10
    };

    enum { ANIMATION_STATE_MOVE_PENDING = 2 };
    int AddMoveToPointActionToFront(
        unsigned short nGroupId, const Vector& vPosition,
        unsigned long oidArea, unsigned long oidTarget, int bRun,
        float fRange, float fTimeout, int bUsePath, int nMovementRate,
        int bUnknown1, int bClientMoving, int bUnknown2, int bUnknown3,
        const Vector& vOrientation, unsigned long nUnknown4,
        unsigned long nUnknown5, int bUnknown6);
    int IsLightsaberEquipped();
    int IsWieldingLightsaber();
    int ConvertGrenadeSpellID(int nSpellID);
    int ResolveMoveToForceJump(CSWSObject* pTarget, unsigned short& nFeat);
    int HasTargetBeenDetected(unsigned long oidTarget, int bCheckHidden);
    void InitializeNewAttackTarget(unsigned long oidTarget,
                                   CSWSObjectActionNode* pAction);
    int GetWeaponImpactHits(int nWeapon);
    int GetWeaponImpactTime(int nWeapon, int nImpact);
    int GetRelativeWeaponSize(CSWSItem* pItem);
    int ResolveProjectileMissImpact(const Vector& vImpact, int nDelay);
    CExoString GetWeaponDischargeRow(int nWeapon);
    int GetWeaponDischargeShots(int nWeapon);
    int GetWeaponHitShots(int nWeapon);
    int GetWeaponDischargeTime(int nWeapon, int nDischarge);
    int GetWeaponDischargeHand(int nWeapon, int nHand);
    void SetCombatReactionTime(int nAttack, int nTime, int nReactionDelay);
    int CalculateMaxElementalDamage(CSWSObject* pObject, int nUnknown);
    CGameEffect* CreateLinkedEffect(int nEffectType, int nEffectInteger,
                                    int nLinkedEffectType,
                                    int nLinkedEffectInteger);
    CGameEffect* CreateLinkedEffect(CGameEffect* pEffect, int nEffectType,
                                    int nEffectInteger);
    void ApplyOnHitStun(CSWSObject* pObject, int nDuration);
    void ApplyOnHitConfusion(CSWSObject* pObject, int nDuration);
    void ApplyOnHitFear(CSWSObject* pObject, int nDuration);
    void ApplyOnHitHold(CSWSObject* pObject, int nDuration);
    void ApplyOnHitSleep(CSWSObject* pObject, int nDuration);
    void ApplyOnHitSlow(CSWSObject* pObject, int nDuration);
    void ApplyOnHitPoison(CSWSObject* pObject, CSWItemProperty* pItemProperty);
    void ApplyOnHitAbilityDamage(CSWSObject* pObject,
                                 CSWItemProperty* pItemProperty);
    void ApplyOnHitSlayAlignmentGroup(CSWSObject* pObject,
                                      CSWItemProperty* pItemProperty);
    void ApplyOnHitSlayRacialGroup(CSWSObject* pObject,
                                   CSWItemProperty* pItemProperty);
    void ApplyOnHitInstantDeath(CSWSObject* pObject,
                                CSWItemProperty* pItemProperty);
    void ApplyOnHitKnockdown(CSWSObject* pObject,
                             CSWItemProperty* pItemProperty);
    Vector ResolveProjectileMissLocation(CSWSObject* pTarget);
    void ResolveRangedMiss(CSWSObject* pTarget);
    int CalculateProjectileTimeToTarget(const Vector& vTarget, int bFast);
    void ResolveRangedHit(CSWSObject* pTarget);

    void ResolveRangedSpecialAttack(CSWSObject* pTarget, int bUseAttackResult);
    void ResolveRangedAnimations(CSWSObject* pTarget, int nAnimationLength,
                                 int bForceReaction);
    void ResolveSafeProjectile(int nAttack, int nProjectileType,
                               int bUseAttackResult, int nWeaponHand);
    void ResolvePostMeleeDamage(CSWSObject* pTarget);

    void SignalMeleeDamage(unsigned long oidTarget, int nAttacks);

    void SignalDamage(int bRanged, unsigned long oidTarget);

    void SignalRangedDamage(unsigned long oidTarget);
    void ResolveOnHitVisuals(CSWSObject* pTarget);

    void ResolveOnHitEffect(CSWSObject* pTarget, int bOffHand, int bCritical);

    enum { DEFAULT_ON_HIT_SAVE_DC = 20 };
    void ResolvePostRangedDamage(CSWSObject* pTarget);

    void ResolveAttackRoll(CSWSObject* pTarget, int bUseAttackResult,
                           int nAttackResult);
    void ResolveSneakAttack(CSWSCreature* pTarget);
    int GetClientRangedAnimation(int nAttackType, int nOption1, int nOption2);
    void ResolveMeleeSpecialAttack(int nAttack, int nAttacks,
                                   CSWSObject* pTarget, int nImpactTime,
                                   int* pbStateApplied);
    void ResolveDamage(CSWSObject* pTarget, int bUseDamage, int nDamage);
    void ResolveMeleeAnimations(int nAttackNumber, int nTotalAttacks,
                                CSWSObject* pTarget, int nAnimationLength,
                                int bTimeAnimation);
    void ResolveSituationalModifiers(CSWSCreature* pTarget);
    void ResolveDamageShields(CSWSCreature* pAttacker);
    int GetClientMeleeAnimation(unsigned long oidTarget, unsigned short nAnimation,
                                int nAttackType, int bUseAttackResult,
                                int nAttackResult);
    void ResolveMeleeAttack(CSWSObject* pTarget, int nAttacks, int nAnimation,
                            int bUseAttackResult, int nAttackResult,
                            int nAttackRollResult, int nDamage);
    int AIActionAnimalEmpathy(CSWSObjectActionNode* pAction);
    int AIActionRest(CSWSObjectActionNode* pAction);
    int AIActionRandomWalk(CSWSObjectActionNode* pAction);
    int AIActionDisappear(CSWSObjectActionNode* pAction);
    int AIActionAppear(CSWSObjectActionNode* pAction);
    int AIActionAreaWait(CSWSObjectActionNode* pAction);
    int AIActionCheckMoveToObject(CSWSObjectActionNode* pAction);
    int AIActionCheckMoveToObjectRadius(CSWSObjectActionNode* pAction);
    int AIActionCheckMoveToPoint(CSWSObjectActionNode* pAction);
    int AIActionCheckMoveToPointRadius(CSWSObjectActionNode* pAction);
    int AIActionCheckMoveAwayFromLocation(CSWSObjectActionNode* pAction);
    int AIActionCheckMoveToFollowRadius(CSWSObjectActionNode* pAction);
    int AIActionPartyFollowLeader(CSWSObjectActionNode* pAction);
    int AIActionFollowOwner(CSWSObjectActionNode* pNode);
    int AIActionStanceTotalDefense(CSWSObjectActionNode* pNode);
    int AIActionStanceMeditative(CSWSObjectActionNode* pNode);


    enum { ENCOUNTER_CREATURE_DESTROY_DELAY = 5000 };
    int AIActionEncounterCreatureDestroySelf();
    int AIActionWaitForEndOfRound(CSWSObjectActionNode* pAction);
    int AIActionCheckForceFollowObject(CSWSObjectActionNode* pAction);
    int AIActionForceFollowObject(CSWSObjectActionNode* pAction);
    int AIActionCheckInterAreaPathfinding(CSWSObjectActionNode* pAction);
    int AIActionChangeFacingObject(CSWSObjectActionNode* pAction);
    int AIActionChangeFacingPoint(CSWSObjectActionNode* pAction);
    int AIActionOrientCamera(CSWSObjectActionNode* pAction);
    int ForceMoveToPoint(CSWSForcedAction* pAction);
    int AIActionJumpToPoint(CSWSObjectActionNode* pAction);
    int AIActionJumpToObject(CSWSObjectActionNode* pAction);

    enum { JUMP_TO_OBJECT_SEARCH_RADIUS = 20 };
    int AIActionUnequipItem(CSWSObjectActionNode* pAction);
    int AIActionEquipItem(CSWSObjectActionNode* pAction);
    void SetDetectMode(unsigned char nDetectMode);
    void SetStealthMode(unsigned char nStealthMode, int nValue);
    void TerminateClientSidePath(int nPathType);
    void ComputeModifiedMovementRate();
    float ComputeTotalWalkDistance();
    float ComputeSpeedFactor(float fDistance, int nTime);
    int SetAttackResult(int nAttackResult);
    int IsAttackResultHit(int nAttackResult);
    int IsAttackResultMiss(int nAttackResult);
    int IsAttackResultParried(int nAttackResult);
    unsigned long GetFirstAttacker();
    unsigned long GetNextAttacker();
    void ClearAttackerList();
    int IsDebilitated(int bCheckMeditation = 1);
    int IsInTotalDefense();
    void CancelAllCombatModes(int nUnknown);
    void CreateNewAttackActions(int bFindNewTarget, int bCheckCleaveRange,
                                unsigned long oidKilledCreature);
    unsigned long GetNearestEnemy(float fRange, unsigned long oidRelativeTo,
                                  unsigned long oidExclude);

    unsigned long GetAttemptedAttackTarget() { return m_oidAttemptedAttackTarget; }
    void SetAttemptedAttackTarget(unsigned long oidTarget);
    unsigned long GetAttemptedSpellTarget() { return m_oidAttemptedSpellTarget; }
    int GetDeadTemp()
    {
        if (m_bInParty != 0)
        {
            return GetCurrentHitPoints(0) <= 0;
        }
        return 0;
    }
    int ApplyPoisonDamage(int nPoison, CGameEffect* pEffect, int nSave, float fDamage);
    int ApplyDiseasePayload(CGameEffect* pEffect, unsigned long nDay,
                            unsigned long nTime);
    int GetIsCreatureBumpable(CSWSCreature* pCreature);

    void GetNearestObjects(float fHostileRange, float fRange,
                           CExoArrayList<unsigned long>& aForward,
                           CExoArrayList<unsigned long>& aNearest);
    int CheckProficiencies(CSWSItem* pItem, unsigned long nSlot);
    int CheckItemAlignmentRestrictions(CSWSItem* pItem);
    int CheckItemClassRestrictions(CSWSItem* pItem);
    int CheckItemRaceRestrictions(CSWSItem* pItem, int bEnforceRace);
    int CheckItemFeatRestrictions(CSWSItem* pItem);
    int CheckItemGenderRestrictions(CSWSItem* pItem);
    int CheckItemPCRestrictions(CSWSItem* pItem);
    int CheckItemAttributeRestrictions(CSWSItem* pItem);
    int CheckItemCNPCRestrictions(CSWSItem* pItem);
    int CheckUseMagicDeviceSkill(CSWSItem* pItem, int nRestrictionType);
    int RunEquip(unsigned long oidItem, unsigned long nInventorySlot, int nFlags);
    int RunUnequip(unsigned long oidItem, unsigned long oidTarget,
                   int bUnknown1, CSWSItem** ppItem, int bUnknown2,
                   int bUnknown3 = 0);
    int RemoveItem(CSWSItem* pItem, int bUnknown1, int bUnknown2,
                   CSWSItem** ppItem);
    unsigned char CanEquipMiscellaneous(CSWSItem* pItem, unsigned long* pnSlot,
                                        int bFeedback, int bUnknown);
    unsigned char CanEquipWeapon2(CSWSItem* pItem, unsigned long* pnSlot,
                                  int bFeedback, int bUnknown);
    unsigned char CanEquipWeapon(CSWSItem* pItem, unsigned long* pnSlot,
                                 int bFeedback, int bUnknown);
    int AIActionSurrenderToEnemies(CSWSObjectActionNode* pAction);
    void SurrenderToEnemies(int bKeepOwnEffects);
    int HasAction(unsigned long nAction);
    CSWSObjectActionNode* GetAction(unsigned long nAction);
    int GetNumInvited();
    void CleanInvitationLists();

    static const unsigned long INVITATION_OFFER_TIMEOUT = 30000;

    static const unsigned long INVITATION_IGNORE_TIMEOUT = 60000;


    static const unsigned long DISAPPEAR_ANIMATION_DURATION = 2000;

    static const unsigned long APPEAR_ANIMATION_DURATION = 2000;
    int GetIsInInvitationsOffered(unsigned long oidCreature);
    void RemoveFromInvitationsOffered(unsigned long oidCreature);
    int GetIsInInvitationsIgnored(unsigned long oidCreature);

    int GetInvitedToParty() { return m_bInvitedToParty; }


    int GetIsPC() { return m_nAreaWaitState; }
    void SetInvitedToParty(int bInvited);
    void AddToInvitationsOffered(unsigned long oidCreature);
    void AddToInvitationsIgnored(unsigned long oidCreature);
    short GetMaxHitPoints(int bBaseOnly);
    short GetMaxForcePoints();
    int GetUseMonkAbilities();
    int IsSpellCastingAnimation(unsigned short nAnimation);
    int UpdateSubareasOnJumpPosition(const Vector& vPosition,
                                     unsigned long oidArea, int bRunScripts);


    void AddToAreaOfEffectObjects(unsigned long oidObject);
    int HandleSubAreaStateChanges(CExoArrayList<unsigned long>* pNewSubAreas,
                                  CExoArrayList<unsigned long>* pOldSubAreas,
                                  int bFlag, int bRunScripts);
    int UpdateSubareasOnMoveTo(const Vector& vFrom, const Vector& vTo,
                               int bUseIntersectList,
                               CExoArrayList<unsigned long>* pIntersectList,
                               int bFlag);


    unsigned long GetId() { return m_nId; }



#ifdef _XBOX





    float m_fMoveToPointTime;
    int m_nMoveToPointCalls;
    int m_nMoveToPointElapsed;
#endif
    unsigned short m_nUnknown228;
    int m_nUnknown22C;


    enum ScriptIndex
    {
        SCRIPT_HEARTBEAT = 0,
        SCRIPT_ON_NOTICE = 1,
        SCRIPT_SPELL_AT = 2,
        SCRIPT_ATTACKED = 3,
        SCRIPT_DAMAGED = 4,
        SCRIPT_DISTURBED = 5,
        SCRIPT_END_ROUND = 6,
        SCRIPT_DIALOGUE = 7,

        SCRIPT_SPAWN = 8,
        SCRIPT_RESTED = 9,
        SCRIPT_DEATH = 10,
        SCRIPT_USER_DEFINED = 11,
        SCRIPT_ON_BLOCKED = 12,
        SCRIPT_END_DIALOGUE = 13,
        NUM_SCRIPTS = 14
    };
    CExoString m_sScripts[NUM_SCRIPTS];

    CExoString m_sScriptNames[NUM_SCRIPTS];
    unsigned long m_oidDisappearTarget;
    Vector m_vDisappearPosition;


    int m_bWorkRemainingCompleteSent;


    CExoArrayList<unsigned long> m_lstAreaOfEffectObjects;
    CExoArrayList<unsigned long> m_lstSubAreas;
    unsigned long m_oidBlockingDoor;
    CPathfindInformation* m_pPathfindInformation;
    int m_nUnknown344;

    int m_nCharacterSheetDisplayCount;

    int m_bCreationScriptFired;
    unsigned long m_nLastHeartbeatDay;
    unsigned long m_nLastHeartbeatTime;

    unsigned long m_nHeartbeatInterval;
    unsigned long m_nVisibilityInterval;
    unsigned long m_nLastVisibilityDay;
    unsigned long m_nLastVisibilityTime;
    int m_nUnknown368;
    int m_nUnknown36C;
    unsigned long m_oidLastMineDetected;
    unsigned long m_nLastMineCheckDay;
    unsigned long m_nLastMineCheckTime;

    unsigned long m_nLastStealthRollDay;
    unsigned long m_nLastStealthRollTime;


    unsigned long m_nExcitedStateTimer;
    unsigned long m_nLastPerceptionDay;
    unsigned long m_nLastPerceptionTime;
    unsigned long m_nPerceptionUpdateCount;
    unsigned long m_nHeartbeatUpdateCount;
    unsigned long m_nMovementSpeedDebugTimer;


    int m_nUnknown39C;
    unsigned long m_nActionQueueUpdateDay;
    unsigned long m_nActionQueueUpdateTime;
    CSWActionNode m_aActionQueue[10];

    CSWSCreaturePartyFollowInfo* m_pFollowInfo;
    unsigned char m_nAmbientAnimationState;

    enum AmbientAnimationState
    {
        AMBIENT_STATE_NORMAL = 0,
        AMBIENT_STATE_2 = 2, AMBIENT_STATE_3 = 3, AMBIENT_STATE_4 = 4,
        AMBIENT_STATE_5 = 5, AMBIENT_STATE_6 = 6, AMBIENT_STATE_7 = 7,
        AMBIENT_STATE_8 = 8, AMBIENT_STATE_9 = 9, AMBIENT_STATE_10 = 10,
        AMBIENT_STATE_11 = 11, AMBIENT_STATE_12 = 12,
        AMBIENT_STATE_13 = 13, AMBIENT_STATE_14 = 14,
        AMBIENT_STATE_15 = 15, AMBIENT_STATE_16 = 16, AMBIENT_STATE_17 = 17
    };
    char m_aUnmapped4C5[3];

    CExoString m_sModelType;
    unsigned char m_nDetectMode;
    unsigned char m_nStealthMode;




    unsigned char m_nActiveCombatMode;
    unsigned char m_nDesiredCombatMode;


    unsigned long m_oidCounterSpellTarget;
    unsigned char m_nUnknown4D8;
    char m_aUnmapped4D9[3];
    int m_bResolveInitiative;
    int m_bCombatState;

    int GetCombatState() { return m_bCombatState; }


    int m_nCombatStateTimer;
    int m_n528;
    int m_bNewAttackTarget;


    int m_bHasArms;
    int m_bHasLegs;


    int m_nDisarmable;
    int GetDisarmable() { return m_nDisarmable; }
    int m_nCreatureSize;

    float m_fPreferredAttackDistance;
    float m_fWeaponScale;
    unsigned long m_oidAttackTarget;
    unsigned long m_oidAttemptedMovementTarget;
    unsigned long m_oidAttemptedAttackTarget;




    unsigned long m_oidAttemptedMoveToTarget;

    Vector m_vLastAttackPosition;
    unsigned long m_oidGoingToBeAttackedBy;
    unsigned long m_oidAttemptedSpellTarget;
    unsigned long m_oidSpellTarget;
    int m_nAttackMovementTimer;
    int m_nInjuredAnimationTimer;
    int m_bTimedAnimation;
    int m_nTimedAnimationRemaining;
    unsigned long m_oidLastHostileTarget;
    int m_nCombatMode;
    int m_nLastAttackAction;
    int m_nCombatSpellId;
    int m_nLastForcePowerUsed;

    int m_nLastSpecialAttack;
    int m_nLastCombatFeatUsed;
    int m_nAttackResult;
    int m_nLastAttackResult;
    int m_nUnknown560;
    int m_bLastForcePowerSuccessful;
    int m_n5AC;


    enum { EFFECT_STACK_COUNT = 108, EFFECT_BONUS_LIMIT = 108,
           EFFECT_ATTACK_LIMIT = 60, EFFECT_SAVING_THROW_LIMIT = 60,
           EFFECT_ABILITY_BONUS_LIMIT = 60, EFFECT_ABILITY_PENALTY_LIMIT = 90,
           EFFECT_SKILL_LIMIT = 90, EFFECT_SKILL_ALL = 255 };
    int m_nEffectBonus[EFFECT_STACK_COUNT];
    int m_nEffectPenalty[EFFECT_STACK_COUNT];
    int m_nEffectBonusSpell[EFFECT_STACK_COUNT];
    int m_nEffectPenaltySpell[EFFECT_STACK_COUNT];
    unsigned long m_oidEffectBonusCreator[EFFECT_STACK_COUNT];
    unsigned long m_oidEffectPenaltyCreator[EFFECT_STACK_COUNT];
    int m_bSilenced;
    int m_bHasted;
    int m_bSlowed;
    int m_bAssuredHit;
    int m_bAssuredDeflection;
    int m_bHasAssuredDeflection;

    unsigned short m_nGuardStanceFeat;
    char m_aUnmapped8E2[0x8E4 - 0x8E2];
    int m_bTaunted;
    int m_nUnknown8E8;
    unsigned char m_nVisionFlags;
    unsigned char m_bDebilitated;
    char m_aUnmapped8EE[0x8F0 - 0x8EE];
    unsigned long m_nEffectSpellId;


    CExoArrayList<CEffectIconObject*> m_lstEffectIcons;
    unsigned long m_oidLastPerceived;


    int m_bLastPerceptionHeard;
    int m_bLastPerceptionSeen;
    int m_bLastPerceptionInaudible;
    int m_bLastPerceptionVanished;
    float m_fSpotCheckDistance;
    float m_fListenCheckDistance;

    unsigned char m_nHideCheckRoll;
    unsigned char m_nMoveSilentlyCheckRoll;
    unsigned char m_nSpotCheckRoll;
    unsigned char m_nListenCheckRoll;
    CExoArrayList<CSWVisibilityNode*> m_lstVisibilityList;
    unsigned long m_oidInvitedBy;
    int m_bInvitedToParty;
    unsigned long m_nInvitedToPartyDay;
    unsigned long m_nInvitedToPartyTime;
    CExoArrayList<CSWSInvitationDetails>* m_pInvitationsOffered;
    CExoArrayList<CSWSInvitationDetails>* m_pInvitationsIgnored;
    unsigned long m_oidLockOrientationToObject;
    int m_nUnknown948;
    unsigned char m_nUnknown94C;
    unsigned char m_nUnknown94D;
    char m_aUnmapped94E[2];

    CExoArrayList<unsigned long> m_lstCreatureRequests;
    unsigned long m_oidSpellCasterItem;
    int m_bIgnoreSpellReadyCount;


    int m_bCasterLevelOverride;
    int m_nCasterLevelOverride;
    int GetCasterLevelOverrideEnabled();
    int m_bItemCastStarted;
    int m_bPickPocketAnimationPlayed;
    int m_bPickPocketTransferStarted;

    int m_bHealAnimationPlayed;
    int m_bMineActionStarted;

    int m_bUnlockObjectAnimationPlayed;
    int m_bLockActionStarted;
    int m_bDropAnimationPlayed;
    int m_bPickupAnimationPlayed;
    int m_nUnknown990;
    int m_nUnknown994;
    int m_nUnknown998;
    int m_nUnknown99C;
    int m_nUnknown9A0;
    int m_bUnlockObjectFacingSet;
    int m_n10B0;
    int m_n10B4;
    int m_n10B8;
    unsigned long m_anSkillCooldowns[8];


    CSWSCombatRound* GetCombatRound() { return m_pcCombatRound; }
    CSWSCombatRound* m_pcCombatRound;
    CSWSJournal* m_pJournal;
    int m_nGold;
    int m_nAreaWaitState;


    short m_nSoundSetFile;
    short GetSoundSetFile() { return m_nSoundSetFile; }

    unsigned char m_nBodyBag;
    char m_aUnmapped9DB[1];


    int m_bRestorePlotOnDownload;
    int m_nPoisonState;

    int GetPoisoned() { return m_nPoisonState; }
    void SetPoisoned(int nState, unsigned __int64 nEffectID)
    {
        m_nPoisonState = nState;
        m_nPoisonEffectID = nEffectID;
    }
    int m_bDiseased;
    unsigned __int64 m_nPoisonEffectID;
    int m_n1104;
    int m_n1108;
    unsigned char m_c110C;
    char m_aUnmapped110D[0x3];
    int m_n1110;




    enum AIStateFlag
    {
        AI_STATE_CAN_MOVE = 0x0002,
        AI_STATE_CAN_USE_HANDS = 0x0004,
        AI_STATE_CAN_SPEAK = 0x0008,
        AI_STATE_CAN_ATTACK = 0x0080,


        AI_STATE_ALL = 0xFFFF,


        AI_STATE_FLAG_0040 = 0x0040
    };

    enum { AI_STATE_ALL_FLAGS = 0xFFFF };
    unsigned short m_nAIStateFlags;
    unsigned char m_nAIState;
    char m_aUnmapped9F3[1];
    unsigned long m_oidAITarget;
    unsigned char m_nUnknown9F8;
    char m_aUnmapped9F9[3];


    unsigned long m_nActivity;

    enum DialogStateMask { DIALOG_PAUSED = 4 };
    unsigned long m_bInDialog;
    int m_bLightsaberThrow;
    float m_fMovementRateFactor;

    float m_fClientPathTime;
    char m_nSpecialWalkAnimation;
    char m_aUnmappedA11[0xA14 - 0xA11];

    int m_bUpdateMovingOrientation;

    int m_bJumped;
    char m_aUnmappedA1C[0xA20 - 0xA1C];

    CExoArrayList<unsigned long>* m_pCreatureList;
    unsigned long m_oidSpawnedBy;


    int m_bEncounterCountRemoved;

    CSWInventory* GetInventory() { return m_pInventory; }
    CSWInventory* m_pInventory;
    CItemRepository* m_pcItemRepository;



    unsigned short m_nInventoryIndex;
    unsigned short m_nInventoryContainerIndex;
    unsigned long m_oidInventoryContainer;


    unsigned short GetInventoryIndex() { return m_nInventoryIndex; }
    void SetInventoryIndex(unsigned short nIndex) { m_nInventoryIndex = nIndex; }
    unsigned short GetInventoryContainerIndex() { return m_nInventoryContainerIndex; }
    void SetInventoryContainerIndex(unsigned short nIndex) { m_nInventoryContainerIndex = nIndex; }
    unsigned long GetInventoryContainer() { return m_oidInventoryContainer; }
    void SetInventoryContainer(unsigned long oidContainer) { m_oidInventoryContainer = oidContainer; }

    int m_nEquippedWeight;


    int m_bComputeWeightCarried;
    int m_nEncumbranceState;
    int GetEncumbranceState() { return m_nEncumbranceState; }
    int m_bPMIsDisguised;
    unsigned short m_nPMAppearance;
    char m_aUnmappedA4E[2];
    CSWSCreatureAppearanceInfo m_cAppearanceInfo;
    CSWSCreatureStats* m_pStats;
    int m_nUnknownA78;
    int m_nUnknownA7C;
    CExoArrayList<unsigned long>* m_pAttackerList;
    int m_nNextAttacker;
    int m_bInParty;




    enum
    {
        ANIMATION_STATE_WALK = 1,
        ANIMATION_STATE_FORCE_PUSH = 4,
        ANIMATION_STATE_FORCE_JUMP = 5,
        ANIMATION_STATE_FORCE_JUMP_TARGET = 6
    };
    int m_nAnimationState;



    float m_fForcedMovementDistance;
    float m_fForcedMovementHeight;
    int m_bRun;
    int m_n11C0;
    int m_bBodyFuel;

    int m_bVisibleListUpdatePending;

    int m_nAttackedPartyLeaderState;
    int m_nLastInstantEquipLeft;
    int m_nLastInstantEquipRight;
    int m_nUseItemTimer;


    float m_fLightsaberThrowRate;


    int m_bForceEquipClothing;
    int m_nStealthModeValue;



    unsigned char m_nItemUseState;

    unsigned char GetItemUseState() { return m_nItemUseState; }

    int m_bDisableInjuredAnim;
    int m_n11F0;
    int m_n11F4;
    int m_n11F8;
    int m_n11FC;
    int m_bPlayerCreated;
    int m_n1204;
    Vector m_v1208;
    Vector m_v1214;
    void AddPazaakCard(CSWSItem* pItem);
    int RemovePazaakCard(CSWSItem* pItem);
    CSWCCreature* GetClientCreature();
    int CanUseItem(CSWSItem* pItem, int bUnknown, int bCheckFullHealth = 1);

    static const int MAX_GOLD = 999999999;
    enum FeedbackMessage
    {


        FEEDBACK_SKILL_COOLDOWN = 1,



        FEEDBACK_OBJECT_ITEM_TEXT = 4,
        FEEDBACK_OBJECT_COUNT = 5,

        FEEDBACK_OBJECT_LOCKED = 13,
        FEEDBACK_ATTACK_LINE_OF_SIGHT_BLOCKED = 218,
        FEEDBACK_OBJECT_ALREADY_UNLOCKED = 14,
        FEEDBACK_OBJECT_KEY_REQUIRED = 15,

        FEEDBACK_REST_IN_COMBAT = 17,
        FEEDBACK_REST_STARTED = 18,
        FEEDBACK_REST_NOT_ALLOWED = 54,
        FEEDBACK_REST_ENEMIES_NEARBY = 186,


        FEEDBACK_INVITATION_EXPIRED = 43,
        FEEDBACK_OBJECT_ITEM_NOTICE = 47,


        FEEDBACK_ITEM_RECEIVED = 50,
        FEEDBACK_ITEM_LOST = 51,

        FEEDBACK_ITEM_LEVEL_RESTRICTION = 0x62,

        FEEDBACK_LEVEL_UP = 11,
        FEEDBACK_ITEM_ALIGNMENT_RESTRICTION = 0xCF,
        FEEDBACK_ITEM_CLASS_RESTRICTION = 0xD0,
        FEEDBACK_ITEM_RACE_RESTRICTION = 0xD1,
        FEEDBACK_ITEM_FEAT_RESTRICTION = 0x6B,

        FEEDBACK_DAMAGE_IMMUNITY = 62,
        FEEDBACK_DAMAGE_RESISTED = 63,
        FEEDBACK_DAMAGE_REDUCTION = 64,
        FEEDBACK_DAMAGE_RESISTANCE_REMAINING = 66,
        FEEDBACK_DAMAGE_REDUCTION_REMAINING = 67,



        FEEDBACK_OBJECT_COUNTER = 69,
        FEEDBACK_OBJECT_COUNTER_REMAINING = 70,
        FEEDBACK_COUNTER_PAIR = 71,
        FEEDBACK_ALIGNMENT_SHIFT = 108,
        FEEDBACK_ATTRIBUTE_REDUCED = 142,
        FEEDBACK_HEALED = 151,
        FEEDBACK_SCRIPT_REPORT = 160,
        FEEDBACK_ACTION_LIST_OVERFLOW = 161,
        FEEDBACK_EFFECT_LIST_OVERFLOW = 162,
        FEEDBACK_ACTION_LIST_ERASED = 164,
        FEEDBACK_EFFECT_LIST_ERASED = 165,
        FEEDBACK_EXPERIENCE_POINTS_GAINED = 182,
        FEEDBACK_EXPERIENCE_POINTS_LOST = 183,


        FEEDBACK_AC_NATURAL_BONUS_ACTIVE = 189,
        FEEDBACK_AC_ARMOUR_BONUS_ACTIVE = 190,
        FEEDBACK_AC_SHIELD_BONUS_ACTIVE = 191,
        FEEDBACK_AC_DEFLECTION_BONUS_ACTIVE = 192,
        FEEDBACK_CUSTOM_STRING = 204,
        FEEDBACK_FORCE_POWER_NAME = 222,
        FEEDBACK_FORCE_POINTS_REGAINED = 228,


        FEEDBACK_WEAPON_INEFFECTIVE = 117,


        FEEDBACK_FLOATY_DATA = 93,


        FEEDBACK_FLOATY_STRING = 94,

        FEEDBACK_ENTANGLE_IMMUNITY = 144,


        FEEDBACK_MIND_SPELLS_IMMUNITY = 132,
        FEEDBACK_POISON_IMMUNITY = 133,
        FEEDBACK_SNEAK_ATTACK_IMMUNITY = 134,
        FEEDBACK_DISEASE_IMMUNITY = 125,
        FEEDBACK_FEAR_IMMUNITY = 128,
        FEEDBACK_PARALYSIS_IMMUNITY = 130,
        FEEDBACK_BLINDNESS_IMMUNITY = 139,
        FEEDBACK_DEAFNESS_IMMUNITY = 140,
        FEEDBACK_SLOW_IMMUNITY = 146,
        FEEDBACK_SILENCE_IMMUNITY = 145,
        FEEDBACK_STUN_IMMUNITY = 138,
        FEEDBACK_SLEEP_IMMUNITY = 135,
        FEEDBACK_CONFUSED_IMMUNITY = 137,

        FEEDBACK_HORRIFIED_IMMUNITY = 227,
        FEEDBACK_CURSED_IMMUNITY = 141,
        FEEDBACK_DAZED_IMMUNITY = 136,
        FEEDBACK_KNOCKDOWN_IMMUNITY = 129,
        FEEDBACK_NEGATIVE_LEVEL_IMMUNITY = 131,
        FEEDBACK_CRITICAL_HITS_IMMUNITY = 126,
        FEEDBACK_DEATH_IMMUNITY = 127,
        FEEDBACK_SPELL_IMMUNITY = 0x44,
        FEEDBACK_GOLD_ADDED = 0x94,
        FEEDBACK_GOLD_REMOVED = 0x95,

        FEEDBACK_CANNOT_DROP_ARMOUR_IN_COMBAT = 0xC2,
        FEEDBACK_CANNOT_UNEQUIP_ARMOUR_IN_COMBAT = 0xC2,
        FEEDBACK_CANNOT_ATTACK_FRIEND = 0xBB,

        FEEDBACK_COMBAT_MODE_WEAPON_INVALID = 188,

        FEEDBACK_CANNOT_STEALTH_IN_COMBAT = 60,


        FEEDBACK_AI_UPDATE_TIME_OVERFLOW = 163,

        FEEDBACK_ON_HIT_ABILITY_DAMAGE = 142,

        FEEDBACK_CANNOT_MOVE = 7,



        FEEDBACK_CANNOT_USE_SKILL = 0,
        FEEDBACK_UNKNOWN_002 = 2,
        FEEDBACK_UNKNOWN_003 = 3,
        FEEDBACK_TARGET_MUST_BE_AWARE = 6,
        FEEDBACK_UNKNOWN_008 = 8,
        FEEDBACK_UNKNOWN_009 = 9,
        FEEDBACK_UNKNOWN_012 = 12,
        FEEDBACK_KEY_USED = 16,
        FEEDBACK_UNKNOWN_017 = 17,
        FEEDBACK_UNKNOWN_018 = 18,
        FEEDBACK_UNKNOWN_019 = 19,
        FEEDBACK_UNKNOWN_020 = 20,
        FEEDBACK_TARGET_NOT_VISIBLE = 21,
        FEEDBACK_UNKNOWN_022 = 22,
        FEEDBACK_ITEM_NO_USES = 23,
        FEEDBACK_PARTY_LEADER_CHANGED = 31,
        FEEDBACK_UNKNOWN_032 = 32,
        FEEDBACK_UNKNOWN_033 = 33,
        FEEDBACK_UNKNOWN_034 = 34,
        FEEDBACK_UNKNOWN_035 = 35,
        FEEDBACK_UNKNOWN_036 = 36,
        FEEDBACK_UNKNOWN_037 = 37,
        FEEDBACK_PARTY_MEMBER_JOINED = 38,
        FEEDBACK_UNKNOWN_039 = 39,
        FEEDBACK_UNKNOWN_040 = 40,
        FEEDBACK_UNKNOWN_041 = 41,
        FEEDBACK_UNKNOWN_042 = 42,
        FEEDBACK_PARTY_MEMBER_LEFT = 44,
        FEEDBACK_UNKNOWN_045 = 45,
        FEEDBACK_UNKNOWN_046 = 46,
        FEEDBACK_UNKNOWN_048 = 48,
        FEEDBACK_UNKNOWN_052 = 52,
        FEEDBACK_UNKNOWN_054 = 54,
        FEEDBACK_HEAL_NO_AFFLICTION = 55,
        FEEDBACK_HEAL_ALREADY_FULL = 56,
        FEEDBACK_UNKNOWN_057 = 57,
        FEEDBACK_DIALOG_TARGET_TOO_FAR = 58,
        FEEDBACK_DIALOG_TARGET_BUSY = 59,
        FEEDBACK_UNKNOWN_061 = 61,
        FEEDBACK_UNKNOWN_065 = 65,
        FEEDBACK_UNKNOWN_072 = 72,
        FEEDBACK_UNKNOWN_074 = 74,
        FEEDBACK_UNKNOWN_075 = 75,
        FEEDBACK_UNKNOWN_076 = 76,
        FEEDBACK_UNKNOWN_077 = 77,
        FEEDBACK_UNKNOWN_078 = 78,
        FEEDBACK_UNKNOWN_079 = 79,
        FEEDBACK_UNKNOWN_080 = 80,
        FEEDBACK_UNKNOWN_081 = 81,
        FEEDBACK_INVENTORY_ITEM_DROPPED = 96,
        FEEDBACK_UNKNOWN_097 = 97,
        FEEDBACK_UNKNOWN_099 = 99,
        FEEDBACK_UNKNOWN_100 = 100,
        FEEDBACK_UNKNOWN_101 = 101,
        FEEDBACK_UNKNOWN_102 = 102,
        FEEDBACK_UNKNOWN_103 = 103,
        FEEDBACK_UNKNOWN_106 = 106,
        FEEDBACK_UNKNOWN_118 = 118,
        FEEDBACK_ITEM_PROFICIENCY_REQUIRED = 119,
        FEEDBACK_UNKNOWN_120 = 120,
        FEEDBACK_WEAPON_EQUIPPED_ONE_HANDED = 121,
        FEEDBACK_WEAPON_EQUIPPED_TWO_HANDED = 122,
        FEEDBACK_EQUIPPED_ITEM_SWAPPED = 123,
        FEEDBACK_UNKNOWN_124 = 124,
        FEEDBACK_EXPERIENCE_POINTS_RECEIVED = 143,
        FEEDBACK_UNKNOWN_147 = 147,
        FEEDBACK_UNKNOWN_150 = 150,
        FEEDBACK_UNKNOWN_152 = 152,
        FEEDBACK_UNKNOWN_186 = 186,
        FEEDBACK_CANNOT_EQUIP_ARMOUR_IN_COMBAT = 193,
        FEEDBACK_UNKNOWN_196 = 196,
        FEEDBACK_UNKNOWN_197 = 197,
        FEEDBACK_FEAT_USES_EXHAUSTED = 199,
        FEEDBACK_UNKNOWN_200 = 200,
        FEEDBACK_UNKNOWN_201 = 201,
        FEEDBACK_UNKNOWN_202 = 202,
        FEEDBACK_PAUSED = 205,
        FEEDBACK_UNPAUSED = 206,
        FEEDBACK_UNKNOWN_210 = 210,
        FEEDBACK_UNKNOWN_211 = 211,
        FEEDBACK_CONTAINER_FULL = 212,
        FEEDBACK_UNKNOWN_213 = 213,
        FEEDBACK_UNKNOWN_214 = 214,
        FEEDBACK_UNKNOWN_215 = 215,
        FEEDBACK_UNKNOWN_219 = 219,
        FEEDBACK_UNKNOWN_220 = 220,
        FEEDBACK_UNKNOWN_221 = 221,
        FEEDBACK_UNKNOWN_223 = 223,
        FEEDBACK_UNKNOWN_224 = 224,
        FEEDBACK_UNKNOWN_225 = 225,
        FEEDBACK_UNKNOWN_226 = 226,
        FEEDBACK_ITEM_GENDER_RESTRICTION = 229,
        FEEDBACK_ITEM_PC_RESTRICTION = 230,
        FEEDBACK_UNKNOWN_231 = 231,
        FEEDBACK_ITEM_ATTRIBUTE_RESTRICTION = 232,
        FEEDBACK_COMPONENTS_ADDED = 233,
        FEEDBACK_CHEMICALS_ADDED = 234,
        FEEDBACK_ITEM_CNPC_RESTRICTION = 235,
    };

    int GetGold();
    int GetComponent();
    void SetComponent(int nComponent);
    int GetChemical();
    void SetChemical(int nChemical);

    enum
    {
        VOICE_CHAT_PICK_LOCK = 23, VOICE_CHAT_PICK_LOCK_FAILED = 24,
        VOICE_CHAT_PICK_LOCK_SUCCESS = 25,
        GUI_TIMING_UNLOCK_OBJECT = 7, GUI_TIMING_LOCK = 8,
        UNLOCK_OBJECT_DURATION = 1500, LOCK_ACTION_TIME_MS = 1500,
        UNLOCK_OBJECT_IMPOSSIBLE_DC = 60, ATTACK_MOVEMENT_TIMEOUT = 3000
    };
    enum
    {
        GUI_TIMING_DOOR_SABER = 9, DOOR_SABER_DURATION = 2000,
        ANIMATION_DOOR_SABER = 10429
    };
    void SetGold(int nGold);
    int DecrementSpellReadyCount(unsigned long nSpell, unsigned char nClass,
                                 unsigned char nSpellLevel,
                                 unsigned char nMetamagic,
                                 unsigned char nSpellLikeClass);
    int GetTotalACEffectBonus();


    int GetEffectIconCount() { return m_lstEffectIcons.num; }
    const CResRef& GetEffectIcon(int nIndex);
    void SendFeedbackMessage(unsigned short nMessage,
                             CSWCCMessageData* pMessageData);
    void DisplayFloatyDataToSelf(unsigned long nData);


    enum { ACTIVITY_INDEX_USE_OBJECT = 4 };
    void SetActivity(int nActivity, int bFlag, int nArg);
    void ReadScriptsFromGff(CResGFF* pRes, CResStruct* pStruct);
    void ReadItemsFromGff(CResGFF* pRes, CResStruct* pStruct,
                         int bFlag1, int bFlag2, int bFlag3);
    void PostProcess();

    void SetOriginalFactionId(int nFactionId)
    {
        GetStats()->m_nOriginalFactionId = nFactionId;
    }

    virtual void BroadcastFloatyData(CSWCCMessageData* pMessageData);
    void BroadcastSkillData(CSWCCMessageData* pMessageData);
    void BroadcastPoisoned(CSWCCMessageData* pMessageData);

    enum PoisonBroadcastRangeLimit { POISON_BROADCAST_RANGE_SQUARED = 900 };



    enum FloatyDataRangeLimit { FLOATY_DATA_RANGE_SQUARED = 900 };

    enum VoiceChatRangeLimit { VOICE_CHAT_RANGE_SQUARED = 900 };
    enum { VOICE_CHAT_DEATH = 16 };
    void UpdateActionQueue();


    enum BattleCryLimits
    {
        BATTLE_CRY_RANGE_SQUARED = 900,
        BATTLE_CRY_COUNT = 5
    };


    enum BattleCryVoice
    {
        BATTLE_CRY_VOICE_1 = 1,
        BATTLE_CRY_VOICE_2 = 2,
        BATTLE_CRY_VOICE_3 = 3,
        BATTLE_CRY_VOICE_4 = 4,
        BATTLE_CRY_VOICE_5 = 5
    };
    void BroadcastBattleCry();
    void BroadcastVoiceChat(unsigned char nVoiceChat);

    enum { VOICE_CHAT_CRITICAL_HIT = 17 };
    int SaveCreature(CResGFF* pRes, CResStruct* pStruct);
    void SaveFollowInfo(CResGFF* pRes, CResStruct* pStruct);



    int GetInParty() { return m_bInParty; }
    int GetForm() { return m_n1110; }
    void SetInParty(int bInParty, int bFlag);
    int LoadCreature(CResGFF* pRes, CResStruct* pStruct, int nAreaId);
    int LoadFromTemplate(const CResRef& cResRef, int bFlag);
    int ResolveInitiative();
    void RemoveEffect(CGameEffect* pEffect);
    void AddToArea(CSWSArea* pArea, float fX, float fY, float fZ, int bFlag1,
                   int bFlag2);
    void ReceiveExperience(int nExperience);
    int EquipMostEffectiveArmor();
    int EquipMostDamagingRangedWeapon(unsigned long oidTarget);
    int EquipMostDamagingMeleeWeapon(unsigned long oidTarget, int nSlot,
                                     int bOffHand);
    float DesiredAttackRange(unsigned long oidTarget, int nAttackType);
    float MaxAttackRange(unsigned long oidTarget, int nAttackType,
                         int bOffHand);
    float MaxCleaveRange(unsigned long oidTarget, int nAttackType,
                         int bOffHand);
    void ForceEquipClothing();
    void UnsummonMyself();

    int GetIsWeaponEffective(unsigned long oidTarget, int bOffHand);
    int UseLoreOnItem(unsigned long oidItem);
    int LearnScroll(unsigned long oidItem);

    unsigned char SavingThrowRoll(unsigned char nSaveType, unsigned short nDC,
                                  unsigned char nSaveVsType,
                                  unsigned long oidSaveVersus,
                                  int nArg5, unsigned short nArg6, int nArg7);
    int AddEquipItemActions(CSWSItem* pItem, unsigned long nSlot, int nArg3,
                            int nArg4, int nArg5, int nArg6, int nArg7,
                            int nArg8 = 0);
    void ProcessPendingCombatActions();
    int ReplaceEquipItemActions(CSWSItem* pItem, unsigned long nSlot,
                               int bInstant, int nEquipFlags);
    int AddUnequipActions(CSWSItem* pArg1, unsigned long nArg2, int nArg3,
                          int nArg4, int nArg5, int nArg6 = 0,
                          int nArg7 = 0);
    void AddGold(int nGold, int bFeedback);
    void AddComponent(int nComponent, int bFeedback);
    void AddChemical(int nChemical, int bFeedback);
    void RemoveGold(int nGold, int bFeedback);


    void UpdateAppearanceForEquippedItems();
    int RequestBuy(unsigned long oidItem, unsigned long oidStore);
    int RequestSell(unsigned long oidItem, unsigned long oidStore);
    void CutWaypointPath();
    int AddCutsceneAttackActions(unsigned long nArg1, int nArg2, int nArg3,
                                 int nArg4);
    int AddCutsceneMoveActions(unsigned long nArg1, const Vector& vArg2,
                               int nArg3);
    void CancelCombat(int nArg1);
    int AddAttackActions(unsigned long nArg1, unsigned short nArg2, int nArg3,
                         int nArg4, int nArg5, int nArg6, int nArg7, int nArg8,
                         int nArg9, int nArg10, int nArg11, int nArg12,
                         int nArg13, int nArg14, int nArg15);
    int AddStanceActions(int nStance, unsigned long oidTarget, int nArg3,
                         int nArg4, int nArg5, int nArg6);
    void AdjustReputation(int nFaction, int nAdjustment);
    void SetFormation(unsigned long oidLeader, int nFormation, int nPosition);
    int GetUserActionsPending();

    int GetBlind();
    int Disguise(int nAppearance, CGameEffect* pEffect, int bKeepPosition,
                 int bPermanent = 0);
    int UnDisguise(CGameEffect* pEffect);
    void UpdatePersonalSpace();
    void UpdateAppearanceDependantInfo(int bUpdateModel);
    void BroadcastStealthDetectionData(CSWCCMessageData* pMessageData);
    void BroadcastMineDetectionData(CSWCCMessageData* pMessageData);
    float GetWalkRate();
    float GetRunRate();
    void RemovePureGoodPowers();
    void RemovePureEvilPowers();
    void AddPureGoodPowers();
    void AddPureEvilPowers();
    CResRef GetPortraitBasedOnAlignment();
    void RemoveMindTrickEffects();
    int HasEffect(int nEffectType);
    void RemoveCombatInvisibilityEffects();
    int GetForceItemMask();
    void InstantUseItem(unsigned long oidItem);
    void AutoCloseGUIPanels(int bForce);
    void SetAmbientAnimationState(unsigned char nState);
    int GetInvisible(CSWSObject* pObject);
    void MergeItem(CSWSItem* pItem, CSWSItem* pMergeItem);
    int ValidNearestObjectType(CGameObject* pObject, int& bHostile);
    void DoDamage(int nDamage, int bSuppressFeedback);
    void BroadcastSpellCast(unsigned long nSpell, int nCastResult,
                            unsigned char nClass, unsigned short nFeat,
                            unsigned long oidTarget);
    float GetSpotCheckDistance();
    float GetListenCheckDistance();
    void ComputeArmourClass(CSWSItem* pItem, int bEquipping, int bLoading);
    int UseFeat(unsigned short nFeat, unsigned short nAction,
                unsigned long oidTarget, unsigned long nTime);
    int UseSkill(unsigned char nSkill, unsigned char nSubSkill,
                 unsigned long oidTarget, const Vector& vTarget,
                 unsigned long oidArea, unsigned long oidItemUsed,
                 int bInstant);
    int CalculateSpellSaveDC(int nSpellId);
    unsigned char GetDetectMode();
    unsigned char GetStealthMode();
    void BroadcastSavingThrowData(CSWCCMessageData* pMessageData);
    int GetCurrentlyFollowingLeader();
    int RangeWeaponEquipped();
    void StartGuiTimingBar(unsigned long nValue, unsigned char bFlag);
    void StopGuiTimingBar();
    enum { REST_CANCEL_ACTION = 20 };
    void CancelRest(unsigned short nFeedback);
    void Rest(int bFlag);
    void OnEndDialog(CSWSObject* pObject);
    void GetSpikeDetails(CExoArrayList<CSWGuiInterfaceAction>& lstActions,
                         unsigned char nSpikeType);
    void SetLastInstantEquip(unsigned long nSlot, int nValue);
    int AddHealActions(unsigned long nValue, unsigned long nValue2,
                       int nValue3);
    int GetPauseReadyAnimation();
    void UpdateCombatRoundTimer();
    int ResetCombatAnimations(int nElapsedTime);
    int ReplaceUnequipItemActions(CSWSItem* pItem, unsigned long nSlot,
                                  int bUseActionQueue, int nFlags);
    void BroadcastDamageDataToParty(CSWCCMessageData* pMessageData, int bHealing);

    enum DamageBroadcastRangeLimit { DAMAGE_BROADCAST_RANGE_SQUARED = 900 };
    unsigned long GetLockOrientationToObject()  // PC 0x0051A2F0
    {
        return m_oidLockOrientationToObject;
    }
    CResRef GetDialogResref()  // PC 0x0051A300
    {
        if (m_pStats == NULL)
        {
            return CResRef("");
        }
        return m_pStats->m_cConversation;
    }
    int GetDialogInterruptable()  // PC 0x0051A350
    {
        if (m_pStats == NULL)
        {
            return 0;
        }
        return m_pStats->m_bInterruptable;
    }
    unsigned char GetGender()  // PC 0x0051A360
    {
        if (m_pStats == NULL)
        {
            return ((CSWSObject*)this)->CSWSObject::GetGender();
        }
        return m_pStats->m_nGender;
    }
    CExoLocString* GetFirstName()  // PC 0x0051A380
    {
        if (m_pStats == NULL)
        {
            return &m_locLastName;
        }
        return &m_pStats->m_lsFirstName;
    }
    CExoLocString& GetLastName()  // PC 0x0051A3A0
    {
        if (m_pStats == NULL)
        {
            return m_locLastName;
        }
        return m_pStats->m_lsLastName;
    }
    unsigned long GetEffectSpellId()  // PC 0x0051A3C0
    {
        return m_nEffectSpellId;
    }
    void SetEffectSpellId(unsigned long nSpellId)  // PC 0x0051A3D0
    {
        m_nEffectSpellId = nSpellId;
    }
    CResRef GetPortrait()  // PC 0x0051A3E0
    {
        if (m_pStats != NULL)
        {
            return m_pStats->m_cPortrait;
        }
        return CResRef("");
    }
    void SetPortrait(const CResRef& cResRef)  // PC 0x0051A430
    {
        if (m_pStats != NULL)
        {
            m_pStats->m_cPortrait = CResRef(cResRef);
        }
    }
    int UnequipItem(CSWSItem* pItem);
    int ToggleMode(unsigned char nMode);

    enum ModeToggle
    {
        MODE_STEALTH = 1,


        MODE_PARRY = 2,
        MODE_POWER_ATTACK = 3,
        MODE_IMPROVED_POWER_ATTACK = 4,
        MODE_COUNTERSPELL = 5,
        MODE_FLURRY_OF_BLOWS = 6,
        MODE_RAPID_SHOT = 7
    };

    enum EquipItemResult { EQUIP_ITEM_OK = 1, EQUIP_ITEM_REPLACE = 2,
                           EQUIP_ITEM_REPLACE_HANDS = 3 };
    unsigned char CanEquipItem(CSWSItem* pItem, unsigned long* pnSlot,
                              int bFlag1, int bFlag2, int bFlag3);
    int EquipItem(unsigned long nSlot, CSWSItem* pItem,
                  int bFlag1, int bFlag2, int bFlag3);
    int LoadFollowInfo(CResGFF* pResGFF, CResStruct* pResStruct);
    int DoListenDetection(CSWSCreature* pCreature, int bFlag);
    int DoSpotDetection(CSWSCreature* pCreature, int bFlag);
    int DoStealthDetection(CSWSCreature* pCreature, int bCheckSpot,
                           int* pnSpotDetected, int* pnListenDetected,
                           int bFlag);
    void UpdatePureGoodEvilPowers();

    int ResolveRangedMissToParry(CSWSObject* pTarget, int bCheckWeapon,
                                 int nWeaponHand);
    void ResolveRangedAttack(CSWSObject* pTarget, int nA, int nB, int nC,
                             int nD, int nE, int nF);
    void ResolveAttack(unsigned long oidTarget, int nA, int nB, int nC,
                       int nD, int nE, int nF);

public:

    void SetHealAnimationPlayed(int nHealAnimationPlayed)
    {
        m_bHealAnimationPlayed = nHealAnimationPlayed;
    }
    int GetHealAnimationPlayed()
    {
        return m_bHealAnimationPlayed;
    }
    unsigned char GetAIState()
    {
        return m_nAIState;
    }
    void SetAIState(unsigned char nAIState)
    {
        m_nAIState = nAIState;
    }
    void SetDisarmable(int nDisarmable)
    {
        m_nDisarmable = nDisarmable;
    }
    int GetCombatMode()
    {
        return m_nCombatMode;
    }
    unsigned char GetDesiredCombatMode()
    {
        return m_nDesiredCombatMode;
    }
    void SetDesiredCombatMode(unsigned char nDesiredCombatMode)
    {
        m_nDesiredCombatMode = nDesiredCombatMode;
    }
    void SetHasArms(int nHasArms)
    {
        m_bHasArms = nHasArms;
    }
    int GetHasArms()
    {
        return m_bHasArms;
    }
    void SetHasLegs(int nHasLegs)
    {
        m_bHasLegs = nHasLegs;
    }
    int GetHasLegs()
    {
        return m_bHasLegs;
    }
    void SetCreatureSize(int nCreatureSize)
    {
        m_nCreatureSize = nCreatureSize;
    }
    int GetCreatureSize()
    {
        return m_nCreatureSize;
    }
    void SetModelType(const CExoString& sModelType)
    {
        m_sModelType = sModelType;
    }
    CExoString GetModelType()
    {
        return m_sModelType;
    }
    void SetWeaponScale(float nWeaponScale)
    {
        m_fWeaponScale = nWeaponScale;
    }
    float GetWeaponScale()
    {
        return m_fWeaponScale;
    }
    void SetPreferredAttackDistance(float nPreferredAttackDistance)
    {
        m_fPreferredAttackDistance = nPreferredAttackDistance;
    }
    float GetPreferredAttackDistance()
    {
        return m_fPreferredAttackDistance;
    }
    CExoArrayList<unsigned long>* GetAreaOfEffectObjects()
    {
        return &m_lstAreaOfEffectObjects;
    }
    CExoArrayList<CEffectIconObject*>* GetEffectIcons()
    {
        return &m_lstEffectIcons;
    }
    void SetHasted(int nHasted)
    {
        m_bHasted = nHasted;
    }
    int GetHasted()
    {
        return m_bHasted;
    }
    void SetSlowed(int nSlowed)
    {
        m_bSlowed = nSlowed;
    }
    int GetSlowed()
    {
        return m_bSlowed;
    }
    int GetCombatStateTimer()
    {
        return m_nCombatStateTimer;
    }
    void SetCounterSpellTarget(unsigned long nCounterSpellTarget)
    {
        m_oidCounterSpellTarget = nCounterSpellTarget;
    }
    unsigned long GetCounterSpellTarget()
    {
        return m_oidCounterSpellTarget;
    }
    unsigned char GetAmbientAnimationState()
    {
        return m_nAmbientAnimationState;
    }
    CExoArrayList<CSWVisibilityNode*>* GetVisibilityList()
    {
        return &m_lstVisibilityList;
    }
    void SetEncumbranceState(int nEncumbranceState)
    {
        m_nEncumbranceState = nEncumbranceState;
    }
    unsigned long GetLastPerceived()
    {
        return m_oidLastPerceived;
    }
    void SetLastPerceived(unsigned long nLastPerceived)
    {
        m_oidLastPerceived = nLastPerceived;
    }
    int GetLastPerceptionHeard()
    {
        return m_bLastPerceptionHeard;
    }
    void SetLastPerceptionHeard(int nLastPerceptionHeard)
    {
        m_bLastPerceptionHeard = nLastPerceptionHeard;
    }
    int GetLastPerceptionSeen()
    {
        return m_bLastPerceptionSeen;
    }
    void SetLastPerceptionSeen(int nLastPerceptionSeen)
    {
        m_bLastPerceptionSeen = nLastPerceptionSeen;
    }
    int GetLastPerceptionInaudible()
    {
        return m_bLastPerceptionInaudible;
    }
    void SetLastPerceptionInaudible(int nLastPerceptionInaudible)
    {
        m_bLastPerceptionInaudible = nLastPerceptionInaudible;
    }
    int GetLastPerceptionVanished()
    {
        return m_bLastPerceptionVanished;
    }
    void SetLastPerceptionVanished(int nLastPerceptionVanished)
    {
        m_bLastPerceptionVanished = nLastPerceptionVanished;
    }
    unsigned long GetAttackTarget()
    {
        return m_oidAttackTarget;
    }
    void SetAttackTarget(unsigned long nAttackTarget)
    {
        m_oidAttackTarget = nAttackTarget;
    }
    unsigned long GetSpellTarget()
    {
        return m_oidSpellTarget;
    }
    void SetSpellTarget(unsigned long nSpellTarget)
    {
        m_oidSpellTarget = nSpellTarget;
    }
    unsigned long GetGoingToBeAttackedBy()
    {
        return m_oidGoingToBeAttackedBy;
    }
    void SetGoingToBeAttackedBy(unsigned long nGoingToBeAttackedBy)
    {
        m_oidGoingToBeAttackedBy = nGoingToBeAttackedBy;
    }
    void SetSpotCheckDistance(float nSpotCheckDistance)
    {
        m_fSpotCheckDistance = nSpotCheckDistance;
    }
    void SetListenCheckDistance(float nListenCheckDistance)
    {
        m_fListenCheckDistance = nListenCheckDistance;
    }
    int GetEquippedWeight()
    {
        return m_nEquippedWeight;
    }
    void SetEquippedWeight(int nEquippedWeight)
    {
        m_nEquippedWeight = nEquippedWeight;
    }
    unsigned char GetBodyBag()
    {
        return m_nBodyBag;
    }
    void SetBodyBag(unsigned char nBodyBag)
    {
        m_nBodyBag = nBodyBag;
    }
#ifdef _XBOX



    void AddPrerequisiteSpells(unsigned long nSpell);
#endif
};
#pragma pack(pop)

#endif
