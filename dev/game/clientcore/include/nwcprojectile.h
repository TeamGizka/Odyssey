#ifndef __NWCPROJECTILE_H__
#define __NWCPROJECTILE_H__

#include "exobase.h"
#include "exoresman.h"
#include "math3d.h"
#include "nwcobject.h"
#include "nwsobject.h"

class CAurObject;
class CExoSoundSource;
class Gob;
class CSWCAnimBaseProjectile;

#define PROJECTILE_ORIENTATION_MIN_DIRECTION_SQUARED 0.00001f

#define PROJECTILE_POSITION_EPSILON_SQUARED 0.00001f
#define PROJECTILE_TARGET_HEIGHT 1.25f
#define PROJECTILE_TARGET_BACK_OFFSET 0.5f

#define PROJECTILE_SPIRAL_SPEED 7.5f

#define PROJECTILE_SPIRAL_MIN_DIRECTION_SQUARED 0.00001f
#define PROJECTILE_SPIRAL_RADIUS 2.0f

#define CSWCPROJECTILE_HIGH_BALLISTIC_HEIGHT 30.0f

#define PROJECTILE_DOUBLE_BALLISTIC_ANGLE_UNITS_PER_RADIAN 1000.0f
#define PROJECTILE_DOUBLE_BALLISTIC_MIN_ANGLE 0.78539819f
#define PROJECTILE_DOUBLE_BALLISTIC_MAX_ANGLE 2.3561945f
#define PROJECTILE_DOUBLE_BALLISTIC_OFFSET 3.0f
#define PROJECTILE_DOUBLE_BALLISTIC_GROUND_TRACE_HEIGHT 1000.0f
#define PROJECTILE_DOUBLE_BALLISTIC_RETURN_SPEED_SCALE 0.4f

#define PROJECTILE_BEHAVIOR_DISTANCE_UNSET (-1.0f)

class CProjectileBehavior
{
public:

    enum
    {
        BEHAVIOR_SPIRAL = 6,
        BEHAVIOR_SPIRAL_APPROACH = 10
    };

    enum BehaviorType
    {
        TYPE_HOMING = 1,
        TYPE_BALLISTIC = 2,

        TYPE_BURST_UP = 4,
        TYPE_ACCELERATING = 5,
        TYPE_LINKED = 7,
        TYPE_BURST = 9,
        TYPE_HIGH_BALLISTIC_TRANSITION = 11,
        TYPE_LINKED_BURST_UP = 12,

        TYPE_UNACCELERATED = 13
    };

    Vector m_vStartPosition;
    Vector m_vTargetPosition;
    unsigned long m_nTargetId;
    int m_nProjectileType;
    int m_nDuration;
    int m_nElapsedTime;
    float m_fDistance;
    unsigned char m_nSpawnCount;
    unsigned char m_nType;
    int m_nSoundRow;
};

class CSWCProjectile : public CSWCObject
{
public:
    static const int NUM_IMPACT_SOUNDS;
    static const int NUM_SHOT_SOUNDS;

    enum
    {
        PROJECTILE_ANIMATION_DEFAULT = 0,
        PROJECTILE_ANIMATION_TRAVEL = 1,
        PROJECTILE_ANIMATION_IMPACT = 2,
        PROJECTILE_ANIMATION_THROWOUT = 3,
        PROJECTILE_ANIMATION_THROWRETURN = 4,
        PROJECTILE_ANIMATION_COMBAT = 10000
    };

    enum { CREATURE_ANIMATION_PROJECTILE_PARRY_1 = 10300,
           CREATURE_ANIMATION_PROJECTILE_PARRY_2 = 10301 };
    enum { PROJECTILE_IMPACT_REACTION_TIME = 110 };
    enum ImpactType
    {
        IMPACT_TARGET = 0,
        IMPACT_UNATTACHED_1 = 1,
        IMPACT_SHIELD = 2,
        IMPACT_UNATTACHED_3 = 3,
        IMPACT_UNATTACHED_4 = 4
    };
    enum { PROJECTILE_IMPACT_NONE = 0 };

    enum { PROJECTILE_SPIRAL_DURATION = 2500 };

    enum { PROJECTILE_DOUBLE_BALLISTIC_ANGLE_STEPS = 1570 };

    enum
    {
        PROJECTILE_ORIENTATION_NONE = 0,
        PROJECTILE_ORIENTATION_TARGET = 1,
        PROJECTILE_ORIENTATION_MOVEMENT = 2
    };

    enum
    {
        PROJECTILE_PATH_TYPE_HOMING = 1,
        PROJECTILE_PATH_TYPE_BALLISTIC = 2,
        PROJECTILE_PATH_TYPE_HIGH_BALLISTIC = 3,
        PROJECTILE_PATH_TYPE_BURST_UP = 4,
        PROJECTILE_PATH_TYPE_ACCELERATING = 5,
        PROJECTILE_PATH_TYPE_SPIRAL = 6,
        PROJECTILE_PATH_TYPE_LINKED = 7,
        PROJECTILE_PATH_TYPE_BOUNCE = 8,
        PROJECTILE_PATH_TYPE_BURST = 9,
        PROJECTILE_PATH_TYPE_LINKED_BURST_UP = 10,
        PROJECTILE_PATH_TYPE_TRIPLE_BALLISTIC_HIT = 11,
        PROJECTILE_PATH_TYPE_TRIPLE_BALLISTIC_MISS = 12,
        PROJECTILE_PATH_TYPE_DOUBLE_BALLISTIC = 13,
        PROJECTILE_PATH_TYPE_PARRIED = 14,
        PROJECTILE_PATH_TYPE_DEFLECTED = 15,
        PROJECTILE_PATH_TYPE_REFLECTED = 16,
        PROJECTILE_PATH_TYPE_TEST = 50
    };

    unsigned char m_bSpawnCombatVisualOnImpact;
    Vector m_vImpactPosition;
    short m_nCombatVisualEffect;
    int m_nCombatVisualSpellId;
    Vector m_vDesiredPosition;
    unsigned long m_nDesiredPositionTime;
    unsigned long m_nDesiredTargetId;
    CExoString m_sImpactHook;

    CResRef m_cFlightSound;
    CResRef m_cProjectileSound;
    CExoSoundSource* m_pSound;

    int m_bApplyImpactToTarget;
    unsigned char m_nImpactType;
    int m_bHasImpactSound;
    int m_bImpactSoundStarted;
    int m_bProjectileFinished;
    int m_bUseModuleOrientation;
    unsigned long m_nProjectileType;
    int m_nProjectileDuration;
    int m_nSpellId;
    Vector m_vVelocity;
    Vector m_vAcceleration;
    int m_nDesiredPositionTimer;
    CExoLinkedList<CProjectileBehavior> m_lstBehaviors;
    CProjectileBehavior* m_pCurrentBehavior;
    Gob* m_pReferenceObject;

    unsigned char m_bSpiralCounterClockwise;
    unsigned char m_nOrientationType;

    int m_bImpactAnimationStarted;

    int m_nUnused;
    Gob* m_pMuzzleFlash;
    Vector m_vSourcePosition;
    int m_nSourceDuration;
    float m_fSourceDistance;
    unsigned long m_nSourceTargetId;

    CSWCProjectile();
    virtual ~CSWCProjectile();
    // PC 0x00698BD0 GOG 0x00698980
    virtual CSWCProjectile* AsSWCProjectile()
    {
        return this;
    }

    virtual void AIUpdate();
    virtual int LoadModel(const CResRef& cModel, unsigned char nModel,
                          unsigned char nFlags);
    virtual void UnloadModel();
    virtual void UnloadModel(unsigned char nModel);

    int SetDesiredPositionTimer(int nTime);
    void SetDesiredPosition(const Vector& vPosition);
    void SetSourcePosition(const Vector& vPosition);
    void MoveProjectileAccelerating();
    void MoveProjectileSpiral();
    void SetProjectileVelAndAccel(const Vector& vDirection);
    void SetProjectileSpellId(unsigned long nSpellId);
    int SetAnimation(unsigned short nAnimation);
    void SetSpawnCombatVisualOnImpact(unsigned char bSpawn, int nSpellId,
                                      short nEffect);
    void SetDesiredTargetObject(unsigned long nTargetId,
                                const CExoString& sImpactHook);
    void CreateMuzzleFlash(const CExoString& sModel, CAurObject* pObject,
                           const CExoString& sHook);
    void ClearBehaviorList();
    void SetBehaviorListHoming();
    void SetBehaviorListBallistic();
    void SetBehaviorListHighBallistic();
    void SetBehaviorListBurstUp();
    void SetBehaviorListBurst();
    void SetBehaviorListAccelerating();
    void SetBehaviorListSpiral();
    void SetBehaviorListLinked();
    void SetBehaviorListParried();
    void SetBehaviorListDeflected();
    void SetBehaviorListReflected();
    void SetBehaviorListLinkedBurstUp();
    void SetBehaviorListTest();
    void SetBehaviorListTripleBallistic(int bHit);
    void SetBehaviorListBounce();
    void SetBehaviorListDoubleBallistic();
    void SetProjectileType(unsigned long nType);
    int UpdateCurrentBehavior();
    void MoveProjectileToPosition();
};

#endif
