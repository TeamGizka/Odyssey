#ifndef __NWCANIMBASE_H__
#define __NWCANIMBASE_H__

#define NWCANIMBASE_GROUND_NORMAL_UP_PITCH 90.0f
#define NWCANIMBASE_GROUND_TILT_BLEND 0.5f

class CAurObject;
class CAurScene;
class CResRef;
class Quaternion;
class Vector;
#include "exoresman.h"
#include "exobase.h"
class CSWAnimBase
{
public:

    enum ValueIndex { VALUE_FIRST = 0, VALUE_SECOND = 1,
                     VALUE_THIRD = 2, VALUE_FOURTH = 3 };

    float m_afValues[4];
    virtual ~CSWAnimBase();
    CSWAnimBase();
    int Set(int, float);
};

class CAurBehavior;
class CAurScene;
class CHitInfo;
class CResRef;
class Part;
class Plane;
class Quaternion;
class Scene;
class Vector;
#include "aurbehavior.h"
#include "math3d.h"
#include "exoresman.h"
#include "exobase.h"
#include "aurmodel.h"

class CSWBehaviorAttachSound : public CAurBehaviorAttach
{
public:
    CSWBehaviorAttachSound(CAurObject* pObject, CAurObject* pParent,
                           CAurPart* pPart, const CResRef& cResRef);
    virtual int Control(float fTimeStep);
    int PlaySoundA();
    virtual ~CSWBehaviorAttachSound();

    CResRef m_resSound;
    CExoStreamingSoundSource* m_pSoundSource;
};

class CSWGuiExtent;

extern float g_fMaxLean;

class CSWCAnimBase : public CSWAnimBase
{
public:

    enum
    {

        ANIMATION_TYPE_BASE = 0,

        ANIMATION_TYPE_HEAD = 1,
        ANIMATION_TYPE_WIELD = 2,
        ANIMATION_TYPE_BTYPE = 3,
        ANIMATION_TYPE_TAIL_WINGS = 11
    };

    static const unsigned char MODEL_SLOT_MAIN = 255;

    static const unsigned char MODEL_SLOT_HEAD = 254;

    static const unsigned char MODEL_SLOT_ITEM_VISUAL_EFFECT = 252;

    enum
    {
        MODEL_SLOT_RIGHT_ITEM = 251,
        MODEL_SLOT_LEFT_ITEM = 247
    };

    enum { MODEL_SLOT_RIGHTHAND = 247, MODEL_SLOT_LEFTHAND = 251 };

    enum WieldHand { WIELD_HAND_LEFT = 0, WIELD_HAND_RIGHT = 1 };

    enum
    {
        MODEL_WINGS = 252,
        MODEL_TAIL = 253
    };

    enum PoweredAnimationFlags
    {
        POWERED_ANIMATION_RIGHTHAND = 1,
        POWERED_ANIMATION_LEFTHAND = 2,
        POWERED_ANIMATION_OTHER = 4
    };

    static const unsigned char MODEL_SLOT_ITEM_IGNORE_MIN = 251;

    CSWCAnimBase();
    virtual ~CSWCAnimBase();

    virtual void AttachToScene(CAurScene* pScene);
    virtual Gob* GetModel(unsigned char nSlot);
    virtual int LoadModel(const CResRef& sResRef, unsigned char nType);
    virtual int ReplaceTexture(unsigned char nModel, const CResRef& sOld,
                               const CResRef& sNew, int nFlags);
    virtual int ReplaceTexture(const CResRef& sOld, const CResRef& sNew,
                               int nFlags);
    virtual int ReplaceTexture(unsigned char nModel, const CResRef& sOld,
                               const CResRef& sNew, unsigned long nColor,
                               unsigned short* pnPalette, int nPaletteSize,
                               unsigned long nFlags);
    virtual int ReplaceTexture(const CResRef& sOld, const CResRef& sNew,
                               unsigned long nColor, unsigned short* pnPalette,
                               int nPaletteSize, unsigned long nFlags);
    virtual int RestoreTexture();
    virtual int RestoreTexture(unsigned char nModel);
    virtual int EnableBumpedOutShell(const CResRef& sShell, float fScale);
    virtual int DisableBumpedOutShell();
    virtual int SetMotionBlur(int bBlur);
    virtual int SetMaxMotionBlurLength(float fLength);
    virtual float GetAnimationLength(unsigned short nAnimation);
    virtual float GetAnimationTime(unsigned short nAnimation);
    virtual CExoString GetAnimationName(unsigned short nAnimation);
    virtual int SetOverlayAnimation(unsigned short nAnimation, int bLoop);
    virtual int GetOverlayAnimation(unsigned short nAnimation);
    virtual int GetOverlayAnimationPlaying(unsigned short nAnimation);
    virtual int AnimationExists(unsigned short nAnimation);
    virtual int SetAnimation(unsigned short nAnimation, float fSpeed,
                             int nFlags, float fBlend);
    virtual int SetAnimationOnSubObject(unsigned char nSubObject,
                                        unsigned short nAnimation, float fSpeed,
                                        int nFlags, float fBlend);
    virtual int SetAnimationVisemes(float* pfWeights, float* pfTimes,
                                    int nCount, float fLength);
    virtual int SetOrientation(const Vector& vFacing);
    virtual int SetOrientation(const Quaternion& qOrientation);
    virtual int SetPosition(const Vector& vPosition);
    virtual void SetAlwaysRender(int bRender);
    virtual void SetInCutscene(int bCutscene);
    virtual void UnloadModel();
    virtual void UnloadModel(unsigned char nModel);
    virtual int Wield(unsigned long nItemId, const CExoString& sAttach,  // PC 0x00401590
                            unsigned char nHand, float fScale)
    {
        return 0;
    }
    virtual void HideWieldedItems(const CResRef&, const CResRef&)  // PC 0x006049D0
    {
    }
    virtual void ShowWieldedItems()  // PC 0x004015A0
    {
    }
    virtual void HideHeadItem()  // PC 0x004015A0
    {
    }
    virtual void ShowHeadItem()  // PC 0x004015A0
    {
    }
    virtual void SetWieldLimited(int bLimited);
    virtual int SetAlpha(float fAlpha, unsigned char nModel);
    virtual int SetEnvMap(int nType, int nFlags, const CResRef& sMap);
    virtual int SetEnvMap(unsigned char nModel, int nType, int nFlags,
                          const CResRef& sMap);
    virtual int RestoreEnvMap();
    virtual int ApplyShadowBlob(const CExoString& sModel);
    virtual void RemoveShadowBlob();
    virtual void ShowShadowBlob(int bShow);
    virtual void HideShadowBlob();
    virtual void SetShadowBlobOrientation(const Quaternion& qOrientation);
    virtual void TurnOnShadows();
    virtual void TurnOffShadows();
    virtual Quaternion GetOrientation();
    virtual void UpdateSound(int)  // PC 0x005EE1C0
    {
    }
    virtual int AddAnimationFlag(int nFlag)  // PC 0x006F46C0
    {
        return AddAnimationFlagInternal(m_pModel, nFlag);
    }
    virtual int RemoveAnimationFlag(int nFlag)  // PC 0x006F46E0
    {
        return RemoveAnimationFlagInternal(m_pModel, nFlag);
    }
    virtual void SetLean(float fLean);
    virtual void Lean();
    virtual int GetBoundingRect(CSWGuiExtent& extent);

    enum CenterPointStatus
    {
        CENTER_POINT_UNAVAILABLE = 0,
        CENTER_POINT_IN_FRONT = 1,
        CENTER_POINT_BEHIND = 2
    };
    virtual unsigned char GetCenterPoint(Vector& vCenter);
    virtual unsigned short ServerToClientAnimation(unsigned short nAnimation);
    virtual void EnableDistortion();
    virtual void DisableDistortion();
    virtual void SetHeadItem(unsigned long)  // PC 0x005EE1C0
    {
    }
    virtual int SetAnimationInternal(CAurObject* pObject,
                                     unsigned short nAnimation, float fSpeed,
                                     int nFlags, float fBlend);
    virtual int SetAnimationOnSubObjectInternal(CAurObject* pObject,
                                                unsigned char nSubObject,
                                                unsigned short nAnimation,
                                                float fSpeed, int nFlags,
                                                float fBlend);
    int SetOverlayAnimation(CAurObject* pObject, unsigned short nAnimation,
                            int bLoop);
    int AddAnimationFlagInternal(CAurObject* pObject, int nFlag);
    int RemoveAnimationFlagInternal(CAurObject* pObject, int nFlag);
    int GetBoundingRect(CAurObject* pObject, CSWGuiExtent& extent);

    void RefreshHilite();
    int Hilite(const Vector& vColor, unsigned char nModel, int bPulse);
    int UnHilite(unsigned char nModel);

    void SetGroundTilt(const Vector& vGroundNormal, const Vector& vFacing);

    unsigned short m_anPalette[10];
    unsigned char m_nPaletteCount;
    unsigned char m_nPaletteOffset;
    unsigned char m_aUnmapped2A[2];
    CResRef m_resLoadedModel;
    int m_bPaletteReplaced;
    CResRef m_resEnvMap;
    int m_bEnvMap;
    Vector m_avHilite[5];
    int m_abHiliteActive[5];
    int m_abHilitePulse[5];
    Gob* m_pModel;
    Gob* m_pShadowBlob;
    float m_fAlpha;
    unsigned char m_nAnimationType;

    enum WieldClass
    {
        WIELD_CLASS_NONE = 0,
        WIELD_CLASS_1 = 1,
        WIELD_CLASS_2 = 2,
        WIELD_CLASS_3 = 3,
        WIELD_CLASS_DUAL_2 = 4,
        WIELD_CLASS_4 = 5,
        WIELD_CLASS_DUAL_4 = 6,
        WIELD_CLASS_5 = 7,
        WIELD_CLASS_UNARMED = 8,
        WIELD_CLASS_6 = 9,
        WIELD_CLASS_UNARMED_207 = 10
    };
    enum { OFF_HAND_TYPE_UNARMED = 2 };
    unsigned char m_nWieldClass;
    unsigned char m_nAnimSet;
    unsigned char m_nOffHandType;
    unsigned char m_nSpecialWalkAnimation;
    unsigned char m_aUnmappedC9[3];
    int m_bRunLooksAwful;
    unsigned char m_nD0;
    unsigned char m_aUnmappedD1[3];
    int m_nD4;
    int m_nD8;
    int m_bUseable;
    int m_bShadowBlobShown;
    float m_fLean;
    int m_nPoweredAnimations;
    Quaternion m_qGroundTilt;
};

class CAurObject;
class CResRef;

class CSWCAnimBaseDoor : public CSWCAnimBase
{
public:
    CSWCAnimBaseDoor();
    ~CSWCAnimBaseDoor();
    virtual int LoadModel(CResRef const&, unsigned char);
    virtual int SetAnimationInternal(CAurObject*, unsigned short, float, int,
                                     float);
    virtual CExoString GetAnimationName(unsigned short);
    virtual unsigned short ServerToClientAnimation(unsigned short);
};

class CResRef;

class CSWCAnimBasePlaceable : public CSWCAnimBase
{
public:
    CSWCAnimBasePlaceable();
    ~CSWCAnimBasePlaceable();
    virtual int LoadModel(CResRef const&, unsigned char);
    virtual int SetAnimation(unsigned short, float, int, float);
    virtual CExoString GetAnimationName(unsigned short);
    virtual unsigned short ServerToClientAnimation(unsigned short);
};

class CResRef;
class Vector;
class CSWCAnimBaseProjectile : public CSWCAnimBase
{
public:
    CSWCAnimBaseProjectile();
    ~CSWCAnimBaseProjectile();
    virtual int LoadModel(CResRef const&, unsigned char);
    virtual int SetAnimation(unsigned short, float, int, float);
    virtual int SetOrientation(Vector const&);
};

class CResRef;

class CSWCAnimBaseTrigger : public CSWCAnimBase
{
public:
    CSWCAnimBaseTrigger();
    ~CSWCAnimBaseTrigger();
    virtual int LoadModel(CResRef const&, unsigned char);
    virtual CExoString GetAnimationName(unsigned short);
    virtual unsigned short ServerToClientAnimation(unsigned short);
};

class CAurObject;
class CResRef;

class CSWCAnimBaseTW : public CSWCAnimBase
{
public:
    CSWCAnimBaseTW();
    virtual ~CSWCAnimBaseTW();

    virtual Gob* GetModel(unsigned char nSlot);
    virtual int LoadModel(const CResRef& sResRef, unsigned char nType);
    virtual void UnloadModel();
    virtual int RestoreEnvMap();
    virtual int ReplaceTexture(unsigned char nModel, const CResRef& sOld,
                               const CResRef& sNew, int nFlags);
    virtual int ReplaceTexture(const CResRef& sOld, const CResRef& sNew,
                               int nFlags);
    virtual int ReplaceTexture(unsigned char nModel, const CResRef& sOld,
                               const CResRef& sNew, unsigned long nColor,
                               unsigned short* pnPalette, int nPaletteSize,
                               unsigned long nFlags);
    virtual int ReplaceTexture(const CResRef& sOld, const CResRef& sNew,
                               unsigned long nColor, unsigned short* pnPalette,
                               int nPaletteSize, unsigned long nFlags);
    virtual int RestoreTexture();
    virtual int RestoreTexture(unsigned char nModel);
    virtual void UnloadModel(unsigned char nModel);
    virtual int SetEnvMap(int nType, int nFlags, const CResRef& sMap);
    virtual int SetEnvMap(unsigned char nModel, int nType, int nFlags,
                          const CResRef& sMap);
    virtual int SetAnimationInternal(CAurObject* pObject,
                                     unsigned short nAnimation, float fSpeed,
                                     int nFlags, float fBlend);
    virtual int SetAnimationOnSubObjectInternal(CAurObject* pObject,
                                                unsigned char nSubObject,
                                                unsigned short nAnimation,
                                                float fSpeed, int nFlags,
                                                float fBlend);

    unsigned short m_aTailPalette[10];
    unsigned char m_nTailColor;
    unsigned char m_nTailPaletteStart;
    unsigned short m_aWingsPalette[10];
    unsigned char m_nWingsColor;
    unsigned char m_nWingsPaletteStart;
    CResRef m_sTailTexture;
    int m_bTailTextureChanged;
    CResRef m_sWingsTexture;
    int m_bWingsTextureChanged;
    CResRef m_sTailEnvMap;
    int m_bTailEnvMap;
    CResRef m_sWingsEnvMap;
    int m_bWingsEnvMap;
    Gob* m_pTail;
    Gob* m_pWings;
};

class CAurObject;
class CResRef;
class CSWCAnimBaseVisualEffect : public CSWCAnimBase
{
public:
    CSWCAnimBaseVisualEffect();
    ~CSWCAnimBaseVisualEffect();
    virtual int LoadModel(CResRef const&, unsigned char);
    virtual CExoString GetAnimationName(unsigned short);
    virtual int SetAnimationInternal(CAurObject*, unsigned short, float, int, float);
};

enum RangedAttackAnimation
{
    RANGED_ANIMATION_NONE = 0,
    RANGED_ANIMATION_POWER_BLAST_CREATURE = 361,
    RANGED_ANIMATION_POWER_BLAST_WIELD_4 = 362,
    RANGED_ANIMATION_POWER_BLAST_DUAL_4 = 363,
    RANGED_ANIMATION_POWER_BLAST_WIELD_5 = 364,
    RANGED_ANIMATION_POWER_BLAST_WIELD_6 = 365,
    RANGED_ANIMATION_RAPID_SHOT_CREATURE = 289,
    RANGED_ANIMATION_RAPID_SHOT_WIELD_4 = 218,
    RANGED_ANIMATION_RAPID_SHOT_DUAL_4 = 232,
    RANGED_ANIMATION_RAPID_SHOT_WIELD_5 = 240,
    RANGED_ANIMATION_RAPID_SHOT_WIELD_6 = 353,
    RANGED_ANIMATION_SNIPER_SHOT_CREATURE = 351,
    RANGED_ANIMATION_SNIPER_SHOT_WIELD_4 = 219,
    RANGED_ANIMATION_SNIPER_SHOT_DUAL_4 = 233,
    RANGED_ANIMATION_SNIPER_SHOT_WIELD_5 = 241,
    RANGED_ANIMATION_SNIPER_SHOT_WIELD_6 = 354,
    RANGED_ANIMATION_NORMAL_CREATURE = 288,
    RANGED_ANIMATION_NORMAL_WIELD_4 = 217,
    RANGED_ANIMATION_NORMAL_DUAL_4 = 231,
    RANGED_ANIMATION_NORMAL_WIELD_5 = 239,
    RANGED_ANIMATION_NORMAL_WIELD_6 = 352
};

enum CreatureAnimation
{

    CREATURE_ANIMATION_LOCKPICK_DOOR = 10128,
    CREATURE_ANIMATION_LOCKPICK_PLACEABLE = 10131,
    CREATURE_ANIMATION_MINE_DISARM_1 = 10132,
    CREATURE_ANIMATION_MINE_DISARM_2 = 10134,
    CREATURE_ANIMATION_MINE_DISARM_3 = 10135,
    CREATURE_ANIMATION_MINE_ARM = 10140,
    CREATURE_ANIMATION_MINE_DISARM_4 = 10141,

    CREATURE_ANIMATION_ELECTRICAL_DARK_SIDE_DAMAGE = 10023,
    CREATURE_ANIMATION_NONMELEE_DAMAGE = 10302,

    CREATURE_ANIMATION_AMBIENT_PRESERVING_1 = 10053,
    CREATURE_ANIMATION_AMBIENT_PRESERVING_2 = 10054,
    CREATURE_ANIMATION_AMBIENT_PRESERVING_3 = 10055,
    CREATURE_ANIMATION_AMBIENT_PRESERVING_4 = 10147,
    CREATURE_ANIMATION_AMBIENT_PRESERVING_5 = 10056,

    CREATURE_ANIMATION_SHORT_IDLE = 6,

    CREATURE_ANIMATION_FIXED_SEQUENCE_1 = 10221,
    CREATURE_ANIMATION_FIXED_SEQUENCE_2 = 10222,
    CREATURE_ANIMATION_FIXED_SEQUENCE_3 = 10219,
    CREATURE_ANIMATION_FIXED_SEQUENCE_4 = 10220,

    CREATURE_ANIMATION_RANGED_REACTION = 10011,
    CREATURE_ANIMATION_CURRENT = 0xffff,

    CREATURE_ANIMATION_FIREFORGET_ACTIVATE = 10127,
    CREATURE_ANIMATION_FIREFORGET_BOW = 10035,
    CREATURE_ANIMATION_FIREFORGET_CHOKE = 10150,
    CREATURE_ANIMATION_FIREFORGET_CUSTOM01 = 10142,
    CREATURE_ANIMATION_FIREFORGET_GREETING = 10029,
    CREATURE_ANIMATION_FIREFORGET_HEAD_TURN_LEFT = 10053,
    CREATURE_ANIMATION_FIREFORGET_HEAD_TURN_RIGHT = 10054,
    CREATURE_ANIMATION_FIREFORGET_INJECT = 10070,
    CREATURE_ANIMATION_FIREFORGET_PAUSE_BORED = 10056,
    CREATURE_ANIMATION_FIREFORGET_PAUSE_SCRATCH_HEAD = 10055,
    CREATURE_ANIMATION_FIREFORGET_PERSUADE = 10126,
    CREATURE_ANIMATION_FIREFORGET_SALUTE = 10034,
    CREATURE_ANIMATION_FIREFORGET_TAUNT = 10028,
    CREATURE_ANIMATION_FIREFORGET_THROW_HIGH = 10129,
    CREATURE_ANIMATION_FIREFORGET_THROW_LOW = 10130,
    CREATURE_ANIMATION_FIREFORGET_TREAT_INJURED = 10159,
    CREATURE_ANIMATION_FIREFORGET_USE_COMPUTER = 10125,
    CREATURE_ANIMATION_FIREFORGET_VICTORY = 10044,
    CREATURE_ANIMATION_LOOPING_DANCE = 10122,
    CREATURE_ANIMATION_LOOPING_DANCE1 = 10123,
    CREATURE_ANIMATION_LOOPING_DEACTIVATE = 10118,
    CREATURE_ANIMATION_LOOPING_DEAD = 10006,
    CREATURE_ANIMATION_LOOPING_DEAD_PRONE = 10156,
    CREATURE_ANIMATION_OVERLAY_DRAW = 10157,
    CREATURE_ANIMATION_LOOPING_FLIRT = 10120,
    CREATURE_ANIMATION_LOOPING_GET_LOW = 10059,
    CREATURE_ANIMATION_LOOPING_GET_MID = 10060,
    CREATURE_ANIMATION_LOOPING_HORROR = 10124,
    CREATURE_ANIMATION_LOOPING_KNEEL_TALK_ANGRY = 10163,
    CREATURE_ANIMATION_LOOPING_KNEEL_TALK_SAD = 10164,
    CREATURE_ANIMATION_LOOPING_CHECK_BODY = 10165,
    CREATURE_ANIMATION_LOOPING_SIT_AND_MEDITATE = 10418,
    CREATURE_ANIMATION_LOOPING_SIT_CHAIR = 10424,
    CREATURE_ANIMATION_LOOPING_SIT_CHAIR_DRINK = 10425,
    CREATURE_ANIMATION_LOOPING_SIT_CHAIR_PAZAK = 10426,
    CREATURE_ANIMATION_LOOPING_SIT_CHAIR_COMP1 = 10427,
    CREATURE_ANIMATION_LOOPING_SIT_CHAIR_COMP2 = 10428,
    CREATURE_ANIMATION_DOOR_SABER = 10429,
    CREATURE_ANIMATION_LOOPING_RAGE = 10414,
    CREATURE_ANIMATION_LOOPING_CLOSED = 10022,
    CREATURE_ANIMATION_LOOPING_MEDITATE_STAND = 10419,
    CREATURE_ANIMATION_FIREFORGET_OPEN = 10021,
    CREATURE_ANIMATION_LOOPING_LISTEN = 10030,
    CREATURE_ANIMATION_LOOPING_LISTEN_INJURED = 10155,
    CREATURE_ANIMATION_LOOPING_MEDITATE = 10032,
    CREATURE_ANIMATION_LOOPING_PAUSE2 = 10052,
    CREATURE_ANIMATION_LOOPING_PAUSE3 = 10151,
    CREATURE_ANIMATION_LOOPING_PAUSE_DRUNK = 10058,
    CREATURE_ANIMATION_LOOPING_PAUSE_TIRED = 10057,
    CREATURE_ANIMATION_LOOPING_PRONE = 10139,
    CREATURE_ANIMATION_LOOPING_SLEEP = 10137,
    CREATURE_ANIMATION_LOOPING_SPASM = 10023,
    CREATURE_ANIMATION_LOOPING_TALK_FORCEFUL = 10040,
    CREATURE_ANIMATION_LOOPING_TALK_INJURED = 10154,
    CREATURE_ANIMATION_LOOPING_TALK_LAUGHING = 10041,
    CREATURE_ANIMATION_LOOPING_TALK_NORMAL = 10038,
    CREATURE_ANIMATION_LOOPING_TALK_PLEADING = 10039,
    CREATURE_ANIMATION_LOOPING_TALK_SAD = 10042,
    CREATURE_ANIMATION_LOOPING_TREAT_INJURED = 10160,
    CREATURE_ANIMATION_LOOPING_USE_COMPUTER = 10121,
    CREATURE_ANIMATION_LOOPING_WELD = 10152,
    CREATURE_ANIMATION_LOOPING_WORSHIP = 10033,
    CREATURE_ANIMATION_COMBAT_IDLE = 10000,

    CREATURE_ANIMATION_DIALOG_SPEAK = 10030,
    CREATURE_ANIMATION_DIALOG_SPEAK_INJURED = 10155,
    CREATURE_ANIMATION_DIALOG_LISTEN = 10038,
    CREATURE_ANIMATION_DIALOG_LISTEN_INJURED = 10154,

    CREATURE_ANIMATION_MELEE_SEQUENCE = 10109,

    CREATURE_ANIMATION_WALK = 10002,

    CREATURE_ANIMATION_WALK_BACKWARD = 10003,

    CREATURE_ANIMATION_RUN = 10004,

    CREATURE_ANIMATION_FOOTSTEP_MOVE_10078 = 10078,
    CREATURE_ANIMATION_FOOTSTEP_MOVE_10079 = 10079,
    CREATURE_ANIMATION_FOOTSTEP_MOVE_10084 = 10084,
    CREATURE_ANIMATION_FOOTSTEP_MOVE_10085 = 10085,
    CREATURE_ANIMATION_FOOTSTEP_MOVE_10086 = 10086,
    CREATURE_ANIMATION_FOOTSTEP_MOVE_10087 = 10087,
    CREATURE_ANIMATION_INJURED_WALK = 10093,
    CREATURE_ANIMATION_INJURED_RUN = 10094,
    CREATURE_ANIMATION_SPECIAL_WALK = 10133,

    CREATURE_ANIMATION_DRIVE_DIAGONAL = 10102,
    CREATURE_ANIMATION_DRIVE_DIAGONAL_REVERSE = 10103,
    CREATURE_ANIMATION_DRIVE_SIDEWAYS = 10104,
    CREATURE_ANIMATION_DRIVE_SIDEWAYS_REVERSE = 10105,

    CREATURE_ANIMATION_DRIVE_NO_TRANSITION = 10095,

    CREATURE_ANIMATION_DRAW_WEAPON = 10246,

    CREATURE_ANIMATION_COMBAT_READY = 10001,

    CREATURE_ANIMATION_PAUSE_READY = 10001,

    CREATURE_ANIMATION_DODGE = 10011,
    CREATURE_ANIMATION_PARRY = 10012,
    CREATURE_ANIMATION_DAMAGE = 10014,

    CREATURE_ANIMATION_FOOTSTEP_1 = 10024,
    CREATURE_ANIMATION_FOOTSTEP_2 = 10025,
    CREATURE_ANIMATION_FOOTSTEP_3 = 10026,
    CREATURE_ANIMATION_FOOTSTEP_4 = 10027,

    CREATURE_ANIMATION_CONJURE_1 = 10015,
    CREATURE_ANIMATION_CONJURE_2 = 10016,
    CREATURE_ANIMATION_CONJURE_3 = 11000,
    CREATURE_ANIMATION_SPELLCAST_1 = 10017,
    CREATURE_ANIMATION_SPELLCAST_2 = 10018,
    CREATURE_ANIMATION_SPELLCAST_3 = 10019,
    CREATURE_ANIMATION_SPELLCAST_4 = 10020,
    CREATURE_ANIMATION_SPELLCAST_5 = 10061,

    CREATURE_ANIMATION_DISAPPEAR = 10062,

    CREATURE_ANIMATION_APPEAR = 10063,
    CREATURE_ANIMATION_SPELLCAST_6 = 10250,
    CREATURE_ANIMATION_SPELLCAST_7 = 10251,
    CREATURE_ANIMATION_SPELLCAST_8 = 10252,
    CREATURE_ANIMATION_SPELLCAST_9 = 10253,
    CREATURE_ANIMATION_SPELLCAST_10 = 10261,
    CREATURE_ANIMATION_SPELLCAST_11 = 10401,
    CREATURE_ANIMATION_INJURED_IDLE = 10092,

    CREATURE_ANIMATION_INJURED_IDLE_EXCLUDED = 10150,

    CREATURE_ANIMATION_STEALTH_TRANSITION = 10136,

    CREATURE_ANIMATION_NO_IDLE_FALLBACK = 10098,

    CREATURE_ANIMATION_DEFAULT = -1,
    CREATURE_ANIMATION_STEALTH_WALK = CREATURE_ANIMATION_SPECIAL_WALK,
    CREATURE_ANIMATION_AMBIENT_2 = 10058,
    CREATURE_ANIMATION_AMBIENT_3 = 10008,
    CREATURE_ANIMATION_AMBIENT_4 = 10006,
    CREATURE_ANIMATION_AMBIENT_5 = CREATURE_ANIMATION_INJURED_IDLE_EXCLUDED,
    CREATURE_ANIMATION_AMBIENT_6 = 10117,
    CREATURE_ANIMATION_AMBIENT_7 = 10124,
    CREATURE_ANIMATION_AMBIENT_8 = 10118,
    CREATURE_ANIMATION_AMBIENT_9 = 10400,
    CREATURE_ANIMATION_AMBIENT_10 = CREATURE_ANIMATION_SPELLCAST_11,
    CREATURE_ANIMATION_AMBIENT_11 = 10137,
    CREATURE_ANIMATION_AMBIENT_12 = 10138,
    CREATURE_ANIMATION_AMBIENT_13 = 10402,
    CREATURE_ANIMATION_AMBIENT_14 = 10156
};

enum CutsceneAnimation
{
    CUTSCENE_ANIMATION_FIRST = 1000,
    CUTSCENE_ANIMATION_LAST = 1127,
    CUTSCENE_ANIMATION_W_FIRST = 1200,
    CUTSCENE_ANIMATION_W_LAST = 1327,
    CUTSCENE_ANIMATION_L_FIRST = 1400,
    CUTSCENE_ANIMATION_L_LAST = 1527,
    CUTSCENE_ANIMATION_WL_FIRST = 1600,
    CUTSCENE_ANIMATION_WL_LAST = 1727
};

enum MeleeAnimation
{
    MELEE_ANIMATION_NONE = 0,
    MELEE_ANIMATION_CRITICAL_WIELD_1 = 87,
    MELEE_ANIMATION_CRITICAL_WIELD_2 = 113,
    MELEE_ANIMATION_CRITICAL_WIELD_3 = 154,
    MELEE_ANIMATION_CRITICAL_DUAL_2 = 195,
    MELEE_ANIMATION_FLURRY_WIELD_1 = 88,
    MELEE_ANIMATION_FLURRY_WIELD_2 = 114,
    MELEE_ANIMATION_FLURRY_WIELD_3 = 155,
    MELEE_ANIMATION_FLURRY_DUAL_2 = 196,
    MELEE_ANIMATION_POWER_WIELD_1 = 87,
    MELEE_ANIMATION_POWER_WIELD_2 = 115,
    MELEE_ANIMATION_POWER_WIELD_3 = 156,
    MELEE_ANIMATION_POWER_DUAL_2 = 197,
    MELEE_ANIMATION_JUMP_WIELD_2 = 386,
    MELEE_ANIMATION_JUMP_WIELD_3 = 387,
    MELEE_ANIMATION_JUMP_DUAL_2 = 388,
    MELEE_ANIMATION_NORMAL_WIELD_1 = 87,
    MELEE_ANIMATION_NORMAL_WIELD_2 = 122,
    MELEE_ANIMATION_NORMAL_WIELD_3 = 163,
    MELEE_ANIMATION_NORMAL_DUAL_2 = 204,
    MELEE_ANIMATION_NORMAL_UNARMED = 247,
    MELEE_ANIMATION_COMBAT_WIELD_1 = 87,
    MELEE_ANIMATION_COMBAT_WIELD_2 = 125,
    MELEE_ANIMATION_COMBAT_WIELD_3 = 166,
    MELEE_ANIMATION_COMBAT_DUAL_2 = 207,
    MELEE_ANIMATION_COMBAT_UNARMED = 247,
    MELEE_ANIMATION_NONCOMBAT_WIELD_1 = 87,
    MELEE_ANIMATION_NONCOMBAT_WIELD_2 = 94,
    MELEE_ANIMATION_NONCOMBAT_WIELD_3 = 135,
    MELEE_ANIMATION_NONCOMBAT_DUAL_2 = 176,
    MELEE_ANIMATION_NONCOMBAT_UNARMED = 247,
    MELEE_ANIMATION_CREATURE = 276,
    MELEE_ANIMATION_CREATURE_COMBAT = 282
};

enum DoorAnimation
{
    DOOR_ANIMATION_CLOSED = 10022,
    DOOR_ANIMATION_OPEN1 = 10050,
    DOOR_ANIMATION_OPEN2 = 10051,

    DOOR_ANIMATION_DESTROYED = 10072,
    DOOR_ANIMATION_BUSTED = 10153
};

enum VisualEffectAnimation
{
    VFX_ANIMATION_START = 1,
    VFX_ANIMATION_LOOP = 2,
    VFX_ANIMATION_END = 3
};

#define SERVER_ANIMATION_FIRST 10000

enum PlaceableAnimation
{

    PLACEABLE_ANIMATION_CLIENT_DESTROYED = 306,
    PLACEABLE_ANIMATION_CLIENT_OPEN = 312,
    PLACEABLE_ANIMATION_CLIENT_CLOSED = 313,
    PLACEABLE_ANIMATION_CLIENT_USED = 314,
    PLACEABLE_ANIMATION_CLIENT_ACTIVATE = 315,

    PLACEABLE_ANIMATION_ANIMLOOP01 = 10106,
    PLACEABLE_ANIMATION_ANIMLOOP02 = 10107,
    PLACEABLE_ANIMATION_ANIMLOOP03 = 10108,
    PLACEABLE_ANIMATION_ANIMLOOP04 = 10110,
    PLACEABLE_ANIMATION_ANIMLOOP05 = 10111,
    PLACEABLE_ANIMATION_ANIMLOOP06 = 10112,
    PLACEABLE_ANIMATION_ANIMLOOP07 = 10113,
    PLACEABLE_ANIMATION_ANIMLOOP08 = 10114,
    PLACEABLE_ANIMATION_ANIMLOOP09 = 10115,
    PLACEABLE_ANIMATION_ANIMLOOP10 = 10116,
    PLACEABLE_ANIMATION_BASE = 10000,

    PLACEABLE_ANIMATION_DESTROYED = 10072,
    PLACEABLE_ANIMATION_ACTIVATE = 10073,
    PLACEABLE_ANIMATION_DEACTIVATE = 10074,

    PLACEABLE_ANIMATION_OPEN = 10075,
    PLACEABLE_ANIMATION_CLOSED = 10076
};

enum ItemAnimation
{
    ITEM_ANIMATION_POWER_UP = 296,
    ITEM_ANIMATION_POWERED = 297,
    ITEM_ANIMATION_POWER_DOWN = 298,
    ITEM_ANIMATION_UNPOWERED = 356
};

#endif
