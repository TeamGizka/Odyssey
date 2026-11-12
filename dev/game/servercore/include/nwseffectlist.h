#ifndef __NWSEFFECTLIST_H__
#define __NWSEFFECTLIST_H__



class CSWSObject;
class CGameEffect;



class CGameEffectApplierRemover
{
public:
    CGameEffectApplierRemover()
    {
    }

    virtual ~CGameEffectApplierRemover()
    {
    }

    virtual void InitializeEffects() = 0;
    virtual int OnEffectApplied(CSWSObject* pObject, CGameEffect* pEffect,
                                int bLoadingModule) = 0;
    virtual int OnEffectRemoved(CSWSObject* pObject, CGameEffect* pEffect) = 0;
};

class CSWSEffectListHandler : public CGameEffectApplierRemover
{
public:

    enum KnockdownPenalty { KNOCKDOWN_AI_STATE_MASK = -257 };




    enum SetStateSelector
    {
        STATE_CONFUSION = 1, STATE_FEAR = 2,

        STATE_DROID_STUN = 3, STATE_STUN = 4,
        STATE_HOLD = 5, STATE_SLEEP = 6,

        STATE_CHOKE = 7,

        STATE_HORRIFIED = 8,

        STATE_FORCE_PUSH = 9,

        STATE_WHIRLWIND = 10,

        STATE_VARIANT_12 = 12,


        STATE_FORCE_JUMP_TARGET = 13,

        STATE_KNOCKDOWN = 14, STATE_VARIANT_16 = 16, STATE_VARIANT_18 = 18,
        STATE_VARIANT_15 = 15, STATE_VARIANT_17 = 17, STATE_VARIANT_19 = 19
    };
    enum VisualEffectSelector
    {




        VFX_STUN = 2002, VFX_DROID_STUN = 1007,
        VFX_SLEEP_PRIMARY = 7,

        VFX_SLAY_IMPACT = 50,

        VFX_ABILITY_DAMAGE_IMPACT = 91,
        VFX_HOLD_SECONDARY = 82,
        VFX_SLEEP_IMPACT = 94,
        VFX_SLOW_IMPACT = 95,


        VFX_BLOOD_RED = 158, VFX_BLOOD_GREEN = 159, VFX_BLOOD_YELLOW = 160,
        VFX_ON_HIT_STATE = 207,
        VFX_CONFUSION_PRIMARY = 208,
        VFX_FEAR_PRIMARY = 218,
        VFX_HOLD_PRIMARY = 232,

        VFX_IMP_FORCE_JUMP_ADVANCED = 1002,



        VFX_IMP_MIRV = 6000, VFX_IMP_MIRV_IMPACT = 6001,
        VFX_PRO_AFFLICT = 1003, VFX_ARD_LIGHT_BLIND = 5002,

        VFX_DEATH_DESTRUCTION = 6003,

        VFX_DISEASE = 51,

        VFX_DISEASE_PAYLOAD = 84
    };


    enum VisualEffectTarget
    {
        VISUAL_EFFECT_TARGET_BODY = 1,
        VISUAL_EFFECT_TARGET_GROUND = 2
    };

    enum ForceJumpTiming { FORCE_JUMP_TIME_DELTA = 300 };
    enum EffectIconSelector
    {
        ICON_FEAR = 6, ICON_HOLD = 15, ICON_CONFUSION = 17,
        ICON_SLEEP = 27, ICON_SLOW = 28, ICON_STUN = 29,

        ICON_ABILITY_DECREASE = 28,

        ICON_PURE_EVIL_POWERS = 62,

        ICON_PURE_GOOD_POWERS = 60
    };


    enum { BLINDNESS_MISS_CHANCE_PERCENT = 50 };

    enum InvisibilityInteger
    {
        INVISIBILITY_INTEGER_TYPE = 0,
        INVISIBILITY_INTEGER_RACIAL_GROUP = 1,
        INVISIBILITY_INTEGER_ALIGNMENT = 3
    };
    enum SanctuaryInteger
    {
        SANCTUARY_INTEGER_RACIAL_GROUP = 1,
        SANCTUARY_INTEGER_ALIGNMENT = 3,
        SANCTUARY_INTEGER_SAVE_DC = 4
    };
    enum InvisibilityType
    {
        INVISIBILITY_TYPE_NORMAL = 1,
        INVISIBILITY_TYPE_DARKNESS = 2,
        INVISIBILITY_TYPE_IMPROVED = 4
    };
    enum EffectAlignment { ALIGNMENT_ALL = 0 };


    enum HealSelector { HEAL_FORCE_POINTS = 54 };
    enum HealIntegerParameter { HEAL_INTEGER_NO_FEEDBACK = 2 };
    enum DeathIntegerParameter { DEATH_INTEGER_NO_FADE_AWAY = 2 };
    static const float PLACEABLE_AUTO_OPEN_DISTANCE;
    int OnApplyHeal(CSWSObject* pObject, CGameEffect* pEffect,
                    int bLoadingModule);

    enum PureEvilBonus
    {
        PURE_EVIL_FORCE_POINT_BONUS = 50, PURE_EVIL_DAMAGE_BONUS = 8,
        PURE_EVIL_ABILITY_BONUS = 3
    };




    enum DispelDifficulty { DISPEL_DIFFICULTY_BASE = 11 };
    enum BlindnessSelector { BLINDNESS_REMOVABLE_BY_TRUESEEING = 8 };
    enum TrueSeeingScanLimit { TRUESEEING_LINKED_EFFECT_LIMIT = 75 };

    enum PureGoodBonus { PURE_GOOD_ABILITY_BONUS = 3 };
    enum PoisonResistedSpell { SPELL_ID_7 = 7, SPELL_ID_38 = 38 };


    enum EntanglePenalty
    {
        ENTANGLE_ATTACK_PENALTY = 2, ENTANGLE_DEXTERITY_PENALTY = 4,
        ENTANGLE_MOVEMENT_DECREASE = -3
    };




    enum SetStateAIStateMask
    {
        SETSTATE_AI_NO_MOVEMENT = -3,
        SETSTATE_AI_HOLD = -15,
        SETSTATE_AI_SLEEP = -351,

        SETSTATE_AI_STATE_MASK = -257,
        SILENCE_AI_STATE_MASK = -9,
        DEAF_AI_STATE_MASK = -17
    };

    enum SetStatePenalty { SETSTATE_SAVING_THROW_PENALTY = 2 };

    enum { STATE_MIND_TRICK_18 = 18, STATE_MIND_TRICK_19 = 19 };


    enum ArcaneSpellFailurePenalty
    {
        DEAF_ARCANE_SPELL_FAILURE = 20,
        TAUNT_ARCANE_SPELL_FAILURE = 30
    };

    enum DamageEffectWeaponSelector
    {
        DAMAGE_EFFECT_WEAPON_FIRST = 1, DAMAGE_EFFECT_WEAPON_SECOND = 2
    };
    int OnApplyPureEvilPowers(CSWSObject* pObject, CGameEffect* pEffect,
                             int bLoadingModule);
    int OnApplyForceBody(CSWSObject* pObject, CGameEffect* pEffect,
                         int bLoadingModule);
    int OnApplyFury(CSWSObject* pObject, CGameEffect* pEffect,
                    int bLoadingModule);
    int OnApplyFPRegenModifier(CSWSObject* pObject, CGameEffect* pEffect,
                               int bLoadingModule);
    int OnApplyVPRegenModifier(CSWSObject* pObject, CGameEffect* pEffect,
                               int bLoadingModule);
    int OnApplyForceSight(CSWSObject* pObject, CGameEffect* pEffect,
                          int bLoadingModule);
    int OnApplyFactionModifier(CSWSObject* pObject, CGameEffect* pEffect,
                               int bLoadingModule);
    int OnApplyDestroyShields(CSWSObject* pObject, CGameEffect* pEffect,
                              int bLoadingModule);
    int OnApplyAssassinate(CSWSObject* pObject, CGameEffect* pEffect,
                           int bLoadingModule);
    int OnApplyPureGoodPowers(CSWSObject* pObject, CGameEffect* pEffect,
                             int bLoadingModule);
    int IsEffectToIgnoreOnDeath(CGameEffect* pEffect);


    enum ImmunityIntegerParameter { IMMUNITY_INTEGER_TYPE = 0 };



    enum
    {
        VFX_COM_BLASTER_DEFLECTION = 4023,
        VFX_COM_BLASTER_IMPACT_GROUND = 4032
    };

    static const int STORE_EFFECT;
    static const int DELETE_EFFECT;
    static const int EFFECT_PUREGOODPOWERS;


    static const int EFFECT_RACIAL_TYPE;
    static const int EFFECT_PUREEVILPOWERS;
    static const int EFFECT_FORCEBODY;
    static const int EFFECT_FURY;
    static const int EFFECT_FPREGENMODIFIER;
    static const int EFFECT_VPREGENMODIFIER;
    static const int EFFECT_FORCESIGHT;
    static const int EFFECT_FACTIONMODIFIER;
    static const int EFFECT_DESTROYSHIELDS;
    static const int EFFECT_ASSASSINATE;
    static const int EFFECT_SETSTATE_INTERNAL;

    typedef int (CSWSEffectListHandler::*EffectAppliedHandler)(
        CSWSObject* pObject, CGameEffect* pEffect, int bLoadingModule);
    typedef int (CSWSEffectListHandler::*EffectRemovedHandler)(
        CSWSObject* pObject, CGameEffect* pEffect);

    virtual ~CSWSEffectListHandler();

    virtual void InitializeEffects();
    virtual int OnEffectApplied(CSWSObject* pObject, CGameEffect* pEffect,
                                int bLoadingModule);
    virtual int OnEffectRemoved(CSWSObject* pObject, CGameEffect* pEffect);

    int OnRemoveAssuredHit(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveAssuredDeflection(CSWSObject* pObject, CGameEffect* pEffect);
    int OnApplyAbilityIncrease(CSWSObject* pObject, CGameEffect* pEffect,
                               int bLoadingModule);
    int OnApplyAbilityDecrease(CSWSObject* pObject, CGameEffect* pEffect,
                               int bLoadingModule);
    int OnApplyCurse(CSWSObject* pObject, CGameEffect* pEffect,
                     int bLoadingModule);
    int OnRemoveAbilityIncrease(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveAbilityDecrease(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveSummonCreature(CSWSObject* pObject, CGameEffect* pEffect);
    int OnApplySummonParty(CSWSObject* pObject, CGameEffect* pEffect,
                            int bLoadingModule);
    int OnApplySavingThrowIncrease(CSWSObject* pObject, CGameEffect* pEffect,
                                   int bLoadingModule);
    int OnApplyAttackIncrease(CSWSObject* pObject, CGameEffect* pEffect,
                              int bLoadingModule);
    int OnApplyDamageIncrease(CSWSObject* pObject, CGameEffect* pEffect,
                              int bLoadingModule);
    int OnApplySavingThrowDecrease(CSWSObject* pObject, CGameEffect* pEffect,
                                   int bLoadingModule);
    int OnApplyAttackDecrease(CSWSObject* pObject, CGameEffect* pEffect,
                              int bLoadingModule);
    int OnApplyDamageDecrease(CSWSObject* pObject, CGameEffect* pEffect,
                              int bLoadingModule);
    int OnApplyMovementSpeedIncrease(CSWSObject* pObject, CGameEffect* pEffect,
                                     int bLoadingModule);
    int OnApplyMovementSpeedDecrease(CSWSObject* pObject, CGameEffect* pEffect,
                                     int bLoadingModule);
    int OnApplyTemporaryHitpoints(CSWSObject* pObject, CGameEffect* pEffect,
                                  int bLoadingModule);
    int OnApplyEntangled(CSWSObject* pObject, CGameEffect* pEffect,
                         int bLoadingModule);
    int OnApplyDeaf(CSWSObject* pObject, CGameEffect* pEffect,
                    int bLoadingModule);
    int OnApplyDisarm(CSWSObject* pObject, CGameEffect* pEffect,
                      int bLoadingModule);
    int OnRemoveTemporaryHitpoints(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveEntangled(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveKnockdown(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveSetAIState(CSWSObject* pObject, CGameEffect* pEffect);
    int OnApplySetAIState(CSWSObject* pObject, CGameEffect* pEffect,
                          int bLoadingModule);
    int OnApplyEnemyAttackBonus(CSWSObject* pObject, CGameEffect* pEffect,
                                int bLoadingModule);
    int OnApplyModifyNumAttacks(CSWSObject* pObject, CGameEffect* pEffect,
                                int bLoadingModule);
    int OnRemoveModifyNumAttacks(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveVisualEffect(CSWSObject* pObject, CGameEffect* pEffect);
    int OnApplyLink(CSWSObject* pObject, CGameEffect* pEffect,
                    int bLoadingModule);
    int OnApplyTimestop(CSWSObject* pObject, CGameEffect* pEffect,
                        int bLoadingModule);
    int OnRemoveTimestop(CSWSObject* pObject, CGameEffect* pEffect);
    int OnApplyMissChance(CSWSObject* pObject, CGameEffect* pEffect,
                          int bLoadingModule);

    int OnRemovePoison(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveDisease(CSWSObject* pObject, CGameEffect* pEffect);
    int OnApplyHasteOrSlow(CSWSObject* pObject, CGameEffect* pEffect,
                          int bLoadingModule);
    int OnRemoveHasteInternal(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveHasteOrSlow(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveSlowInternal(CSWSObject* pObject, CGameEffect* pEffect);
    int OnApplySetState(CSWSObject* pObject, CGameEffect* pEffect,
                         int bLoadingModule);
    int OnRemoveSetState(CSWSObject* pObject, CGameEffect* pEffect);
    int OnApplySetStateInternal(CSWSObject* pObject, CGameEffect* pEffect,
                                int bLoadingModule);
    int OnRemoveSetStateInternal(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveSilence(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveTaunt(CSWSObject* pObject, CGameEffect* pEffect);
    int OnApplyLimitMovementSpeed(CSWSObject* pObject, CGameEffect* pEffect,
                                  int bLoadingModule);
    int OnApplySpecialWalkAnimation(CSWSObject* pObject, CGameEffect* pEffect,
                                    int bLoadingModule);
    int OnApplyHitPointChangeWhenDying(CSWSObject* pObject, CGameEffect* pEffect,
                                      int bLoadingModule);
    int OnApplySeeInvisible(CSWSObject* pObject, CGameEffect* pEffect,
                            int bLoadingModule);
    int OnApplyDisappearAppear(CSWSObject* pObject, CGameEffect* pEffect,
                               int bLoadingModule);
    int OnRemoveDisappearAppear(CSWSObject* pObject, CGameEffect* pEffect);
    int OnApplyDisappear(CSWSObject* pObject, CGameEffect* pEffect,
                         int bLoadingModule);
    int OnApplyAppear(CSWSObject* pObject, CGameEffect* pEffect,
                      int bLoadingModule);
    int OnApplyTurnResistance(CSWSObject* pObject, CGameEffect* pEffect,
                              int bLoadingModule);
    int OnApplyTemporaryForce(CSWSObject* pObject, CGameEffect* pEffect,
                              int bLoadingModule);
    int OnRemoveTemporaryForce(CSWSObject* pObject, CGameEffect* pEffect);
    int OnApplyBodyFuel(CSWSObject* pObject, CGameEffect* pEffect,
                        int bLoadingModule);
    int OnRemoveBodyFuel(CSWSObject* pObject, CGameEffect* pEffect);
    int OnApplyEffectImmunity(CSWSObject* pObject, CGameEffect* pEffect,
                              int bLoadingModule);
    int OnApplyLightsaberThrow(CSWSObject* pObject, CGameEffect* pEffect,
                               int bLoadingModule);

    enum { LIGHTSABER_THROW_TIME_SCALE = 2000 };

    enum
    {
        LIGHTSABER_THROW_TARGET_COUNT = 3, LIGHTSABER_THROW_ROUTE_LENGTH = 4
    };
    int OnRemoveLightsaberThrow(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveSavingThrowIncrease(CSWSObject* pObject, CGameEffect* pEffect);
    int OnApplySkillIncrease(CSWSObject* pObject, CGameEffect* pEffect,
                             int bLoadingModule);
    int OnApplySkillDecrease(CSWSObject* pObject, CGameEffect* pEffect,
                             int bLoadingModule);
    int OnRemoveEnemyAttackBonus(CSWSObject* pObject, CGameEffect* pEffect);
    int OnApplyArcaneSpellFailure(CSWSObject* pObject, CGameEffect* pEffect,
                                  int bLoadingModule);
    int OnRemoveArcaneSpellFailure(CSWSObject* pObject, CGameEffect* pEffect);
    int OnApplyForceResistanceIncrease(CSWSObject* pObject,
                                       CGameEffect* pEffect,
                                       int bLoadingModule);
    int OnApplyForceResistanceDecrease(CSWSObject* pObject,
                                       CGameEffect* pEffect,
                                       int bLoadingModule);
    int OnRemoveLimitMovementSpeed(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveSpecialWalkAnimation(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveMovementSpeedIncrease(CSWSObject* pObject,
                                      CGameEffect* pEffect);
    int OnApplyDamageImmunityIncrease(CSWSObject* pObject, CGameEffect* pEffect,
                                      int bLoadingModule);
    int OnApplyDamageImmunityDecrease(CSWSObject* pObject, CGameEffect* pEffect,
                                      int bLoadingModule);
    int OnRemoveDamageImmunityIncrease(CSWSObject* pObject,
                                       CGameEffect* pEffect);
    int OnRemoveDamageImmunityDecrease(CSWSObject* pObject,
                                       CGameEffect* pEffect);
    int OnApplyDamageForcePoints(CSWSObject* pObject, CGameEffect* pEffect,
                                 int bLoadingModule);
    int OnApplyHealForcePoints(CSWSObject* pObject, CGameEffect* pEffect,
                               int bLoadingModule);
    int OnApplyForceDrain(CSWSObject* pObject, CGameEffect* pEffect,
                          int bLoadingModule);


    enum NegativeLevelParameter
    {
        NEGATIVE_LEVEL_MAXIMUM = 100,
        NEGATIVE_LEVEL_ALL_SKILLS = 255,
        NEGATIVE_LEVEL_HITPOINTS = 5
    };
    int OnApplyNegativeLevel(CSWSObject* pObject, CGameEffect* pEffect,
                              int bLoadingModule);
    int OnRemoveNegativeLevel(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveSeeInvisible(CSWSObject* pObject, CGameEffect* pEffect);
    int OnApplyUltraVision(CSWSObject* pObject, CGameEffect* pEffect,
                           int bLoadingModule);
    int OnRemoveTrueSeeing(CSWSObject* pObject, CGameEffect* pEffect);
    int OnApplyTrueSeeing(CSWSObject* pObject, CGameEffect* pEffect,
                          int bLoadingModule);
    int OnRemoveBlindness(CSWSObject* pObject, CGameEffect* pEffect);
    int OnApplyAssuredHit(CSWSObject* pObject, CGameEffect* pEffect,
                          int bLoadingModule);
    int OnApplyAssuredDeflection(CSWSObject* pObject, CGameEffect* pEffect,
                                 int bLoadingModule);
    int OnApplyRacialType(CSWSObject* pObject, CGameEffect* pEffect,
                          int bLoadingModule);
    int OnRemoveForceResistanceDecrease(CSWSObject* pObject,
                                        CGameEffect* pEffect);
    int OnRemoveForceResistanceIncrease(CSWSObject* pObject,
                                        CGameEffect* pEffect);
    int OnApplyACDecrease(CSWSObject* pObject, CGameEffect* pEffect,
                          int bLoadingModule);
    int OnRemoveACIncrease(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveACDecrease(CSWSObject* pObject, CGameEffect* pEffect);
    int OnApplyRegenerate(CSWSObject* pObject, CGameEffect* pEffect,
                          int bLoadingModule);
    int OnApplyForceFizzle(CSWSObject* pObject, CGameEffect* pEffect,
                           int bLoadingModule);
    int OnApplyForceShield(CSWSObject* pObject, CGameEffect* pEffect,
                           int bLoadingModule);
    int OnRemoveNothing(CSWSObject* pObject, CGameEffect* pEffect);
    int OnApplyResurrection(CSWSObject* pObject, CGameEffect* pEffect,
                            int bLoadingModule);
    int OnRemoveHitPointChangeWhenDying(CSWSObject* pObject,
                                       CGameEffect* pEffect);
    int OnApplyWounding(CSWSObject* pObject, CGameEffect* pEffect,
                        int bLoadingModule);
    int OnRemoveWounding(CSWSObject* pObject, CGameEffect* pEffect);
    int OnApplyLight(CSWSObject* pObject, CGameEffect* pEffect,
                     int bLoadingModule);
    int OnApplyDarkness(CSWSObject* pObject, CGameEffect* pEffect,
                        int bLoadingModule);
    int OnApplyForceResisted(CSWSObject* pObject, CGameEffect* pEffect,
                             int bLoadingModule);
    int OnRemoveEffectImmunity(CSWSObject* pObject, CGameEffect* pEffect);

    int OnRemoveDroidStun(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveForcePush(CSWSObject* pObject, CGameEffect* pEffect);
    int OnApplyForcePush(CSWSObject* pObject, CGameEffect* pEffect,
                         int bLoadingModule);
    int OnApplyVampiricRegeneration(CSWSObject* pObject, CGameEffect* pEffect,
                                    int bLoadingModule);
    int OnApplyHasteInternal(CSWSObject* pObject, CGameEffect* pEffect,
                             int bLoadingModule);
    int OnApplyBeam(CSWSObject* pObject, CGameEffect* pEffect,
                    int bLoadingModule);
    int OnApplySlowInternal(CSWSObject* pObject, CGameEffect* pEffect,
                            int bLoadingModule);
    int OnApplyDisguise(CSWSObject* pObject, CGameEffect* pEffect,
                        int bLoadingModule);
    int OnApplyVision(CSWSObject* pObject, CGameEffect* pEffect,
                      int bLoadingModule);
    int OnRemoveAreaOfEffect(CSWSObject* pObject, CGameEffect* pEffect);
    int OnApplyAreaOfEffect(CSWSObject* pObject, CGameEffect* pEffect,
                            int bLoadingModule);
    int OnApplyDispelAllMagic(CSWSObject* pObject, CGameEffect* pEffect,
                             int bLoadingModule);
    int OnApplyDispelBestMagic(CSWSObject* pObject, CGameEffect* pEffect,
                              int bLoadingModule);
    int OnApplyBonusFeat(CSWSObject* pObject, CGameEffect* pEffect,
                         int bLoadingModule);
    int OnApplyInvisibility(CSWSObject* pObject, CGameEffect* pGameEffect,
                            int nValue);
    int OnRemoveInvisibility(CSWSObject* pObject, CGameEffect* pEffect);
    int OnApplySanctuary(CSWSObject* pObject, CGameEffect* pEffect,
                         int bLoadingModule);
    int OnRemoveSanctuary(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveBonusFeat(CSWSObject* pObject, CGameEffect* pGameEffect);
    int OnApplyEffectIcon(CSWSObject* pObject, CGameEffect* pEffect,
                          int bLoadingModule);




    int OnApplyACIncrease(CSWSObject* pObject, CGameEffect* pEffect, int bLoadingModule);
    int OnApplyBlasterDeflectionDecrease(CSWSObject* pObject, CGameEffect* pEffect, int bLoadingModule);
    int OnApplyBlasterDeflectionIncrease(CSWSObject* pObject, CGameEffect* pEffect, int bLoadingModule);
    int OnApplyBlindness(CSWSObject* pObject, CGameEffect* pEffect, int bLoadingModule);
    int OnApplyBonusSpellOfLevel(CSWSObject* pObject, CGameEffect* pEffect, int bLoadingModule);
    int OnApplyChoke(CSWSObject* pObject, CGameEffect* pEffect, int bLoadingModule);
    int OnApplyConcealment(CSWSObject* pObject, CGameEffect* pEffect, int bLoadingModule);
    int OnApplyDamage(CSWSObject* pObject, CGameEffect* pEffect, int bLoadingModule);
    int OnApplyDamageReduction(CSWSObject* pObject, CGameEffect* pEffect, int bLoadingModule);
    int OnApplyDamageResistance(CSWSObject* pObject, CGameEffect* pEffect, int bLoadingModule);
    int OnApplyDamageShield(CSWSObject* pObject, CGameEffect* pEffect, int bLoadingModule);
    int OnApplyDeath(CSWSObject* pObject, CGameEffect* pEffect, int bLoadingModule);
    int OnApplyDisease(CSWSObject* pObject, CGameEffect* pEffect, int bLoadingModule);
    int OnApplyDroidStun(CSWSObject* pObject, CGameEffect* pEffect, int bLoadingModule);
    int OnApplyForceJump(CSWSObject* pObject, CGameEffect* pEffect, int bLoadingModule);
    int OnApplyForceJumpDelayed(CSWSObject* pObject, CGameEffect* pEffect, int bLoadingModule);
    int OnApplyHorrified(CSWSObject* pObject, CGameEffect* pEffect, int bLoadingModule);
    int OnApplyKnockdown(CSWSObject* pObject, CGameEffect* pEffect, int bLoadingModule);
    int OnApplyPoison(CSWSObject* pObject, CGameEffect* pEffect, int bLoadingModule);
    int OnApplyPsychicStatic(CSWSObject* pObject, CGameEffect* pEffect, int bLoadingModule);
    int OnApplySilence(CSWSObject* pObject, CGameEffect* pEffect, int bLoadingModule);
    int OnApplySpellImmunity(CSWSObject* pObject, CGameEffect* pEffect, int bLoadingModule);
    int OnApplySpellLevelAbsorption(CSWSObject* pObject, CGameEffect* pEffect, int bLoadingModule);
    int OnApplySummonCreature(CSWSObject* pObject, CGameEffect* pEffect, int bLoadingModule);
    int OnApplyTaunt(CSWSObject* pObject, CGameEffect* pEffect, int bLoadingModule);
    int OnApplyVisualEffect(CSWSObject* pObject, CGameEffect* pEffect, int bLoadingModule);
    int OnApplyWhirlWind(CSWSObject* pObject, CGameEffect* pEffect, int bLoadingModule);
    int OnRemoveAttackDecrease(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveAttackIncrease(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveBeam(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveBlasterDeflectionDecrease(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveBlasterDeflectionIncrease(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveBonusSpellOfLevel(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveChoke(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveConcealment(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveCurse(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveDamageDecrease(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveDamageIncrease(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveDamageShield(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveDarkness(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveDisarm(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveDisguise(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveEffectIcon(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveForceJump(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveForceJumpDelayed(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveForceShield(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveHorrified(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveLight(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveMissChance(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveMovementSpeedDecrease(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemovePsychicStatic(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemovePureEvilPowers(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveForceBody(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveFury(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveFPRegenModifier(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveVPRegenModifier(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveForceSight(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveFactionModifier(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemovePureGoodPowers(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveRacialType(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveSavingThrowDecrease(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveSkillDecrease(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveSkillIncrease(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveSpellImmunity(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveSpellLevelAbsorption(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveSummonParty(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveTurnResistance(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveUltraVision(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveVision(CSWSObject* pObject, CGameEffect* pEffect);
    int OnRemoveWhirlWind(CSWSObject* pObject, CGameEffect* pEffect);

    EffectAppliedHandler* m_pEffectApplied;
    EffectRemovedHandler* m_pEffectRemoved;



    static const int EFFECT_HASTE_INTERNAL;
    static const int EFFECT_SLOW_INTERNAL;
    static const int EFFECT_DROIDSTUN;
    static const int EFFECT_TAUNT;
    static const int EFFECT_WHIRLWIND;
    static const int EFFECT_VISION;
    static const int EFFECT_BONUS_SPELL_OF_LEVEL;
    static const int EFFECT_DISAPPEARAPPEAR;
    static const int EFFECT_DISAPPEAR;
    static const int EFFECT_WOUNDING;
    static const int EFFECT_VAMPIRIC_REGENERATION;
    static const int EFFECT_DISARM;
    static const int EFFECT_HORRIFIED;
    static const int EFFECT_CHOKE;
    static const int EFFECT_FORCEJUMPDELAYED;
    static const int DAMAGE_POWER_PLUS_ONE;
    static const int DAMAGE_POWER_PLUS_TWO;
    static const int DAMAGE_POWER_PLUS_THREE;
    static const int DAMAGE_POWER_PLUS_FOUR;
    static const int DAMAGE_POWER_PLUS_FIVE;
    static const int IMMUNITY_TYPE_ATTACK_DECREASE;
    static const int IMMUNITY_TYPE_DAMAGE_DECREASE;
    static const int IMMUNITY_TYPE_DAMAGE_IMMUNITY_DECREASE;
    static const int IMMUNITY_TYPE_AC_DECREASE;
    static const int IMMUNITY_TYPE_SAVING_THROW_DECREASE;
    static const int IMMUNITY_TYPE_FORCE_RESISTANCE_DECREASE;
    static const int IMMUNITY_TYPE_SKILL_DECREASE;


    static const int EFFECT_TURN_RESISTANCE_INCREASE;
    static const int EFFECT_TURN_RESISTANCE_DECREASE;
    static const int EFFECT_ABILITY_INVALID;
    static const int VISION_EFFECT_TYPE_BLINDVISION;
    static const int VISION_EFFECT_TYPE_LOWLIGHTVISION;
    static const int VISION_EFFECT_TYPE_DARKVISION;
    static const int VISION_EFFECT_TYPE_ULTRAVISION;
    static const int VISION_EFFECT_TYPE_BLIND;
    static const int EFFECT_ABILITY_DECREASE;
    static const int EFFECT_ABILITY_INCREASE;



    enum ACIntegerParameter
    {
        AC_INTEGER_TYPE = 0,
        AC_INTEGER_BONUS = 1,
        AC_INTEGER_RACIAL_GROUP = 2,
        AC_INTEGER_ALIGNMENT_GROUP1 = 3,
        AC_INTEGER_ALIGNMENT_GROUP2 = 4,


        AC_INTEGER_DAMAGE_FLAGS = 5
    };

    enum { AC_VS_DAMAGE_TYPE_ALL = 0x2007 };


    enum ACTypeSelector
    {
        AC_DODGE_BONUS = 0,
        AC_NATURAL_BONUS = 1,
        AC_ARMOUR_ENCHANTMENT_BONUS = 2,
        AC_SHIELD_ENCHANTMENT_BONUS = 3,
        AC_DEFLECTION_BONUS = 4
    };

    enum AttackIntegerParameter
    {
        ATTACK_INTEGER_BONUS = 0,
        ATTACK_INTEGER_WEAPON = 1,
        ATTACK_INTEGER_RACIAL_GROUP = 2,
        ATTACK_INTEGER_ALIGNMENT_GROUP1 = 3,
        ATTACK_INTEGER_ALIGNMENT_GROUP2 = 4
    };

    enum DamageIntegerParameter
    {
        DAMAGE_INTEGER_BONUS = 0,
        DAMAGE_INTEGER_TYPE = 1,
        DAMAGE_INTEGER_RACIAL_GROUP = 2,
        DAMAGE_INTEGER_ALIGNMENT_GROUP1 = 3,
        DAMAGE_INTEGER_ALIGNMENT_GROUP2 = 4,
        DAMAGE_INTEGER_WEAPON = 5
    };


    enum ResistanceIntegerParameter
    {
        RESISTANCE_INTEGER_TYPE = 0,
        RESISTANCE_INTEGER_AMOUNT = 1,
        RESISTANCE_INTEGER_LIMIT = 2,
        RESISTANCE_INTEGER_VULNERABILITY_TYPE = 3,
        RESISTANCE_INTEGER_POWER_ATTACK_VULNERABLE = 4
    };



    enum AppliedDamageIntegerParameter
    {


        APPLIED_DAMAGE_BLASTER = 12, APPLIED_DAMAGE_BASE = 14,
        APPLIED_DAMAGE_TYPE_COUNT = 15,
        APPLIED_DAMAGE_FROM_ATTACK = 15,
        APPLIED_DAMAGE_ANIMATION_DELAY = 16,
        APPLIED_DAMAGE_FLAGS = 17, APPLIED_DAMAGE_POWER = 18,
        APPLIED_DAMAGE_MELEE = 19,


        APPLIED_DAMAGE_RANGED = 20,
        APPLIED_DAMAGE_COMBAT = 21,
        APPLIED_DAMAGE_INTEGER_COUNT = 22
    };

    static const int EFFECT_AC_DECREASE;
    static const int EFFECT_AC_INCREASE;
    static const int EFFECT_AREA_OF_EFFECT;


    static const int EFFECT_APPEAR;
    static const int EFFECT_ATTACK_DECREASE;
    static const int EFFECT_ATTACK_INCREASE;
    static const int EFFECT_BEAM;
    static const int EFFECT_CONCEALMENT;
    static const int EFFECT_DAMAGE;
    static const int EFFECT_DEATH;


    static const int EFFECT_KNOCKDOWN;
    static const int EFFECT_FORCERESISTED;
    static const int EFFECT_FORCEFIZZLE;
    static const int EFFECT_FORCESHIELD;
    static const int EFFECT_FORCEDRAIN;
    static const int EFFECT_HEAL;
    static const int EFFECT_HITPOINTCHANGEWHENDYING;
    static const int MISS_CHANCE_TYPE_NORMAL;
    static const int EFFECT_MODIFYNUMATTACKS;
    static const int EFFECT_TEMPORARY_FORCE;
    static const int EFFECT_FORCEPUSHED;
    static const int EFFECT_BLASTERDEFLECTIONINCREASE;
    static const int EFFECT_BLASTERDEFLECTIONDECREASE;
    static const int EFFECT_DAMAGEFORCEPOINTS;
    static const int EFFECT_HEALFORCEPOINTS;
    static const int EFFECT_BODYFUEL;
    static const int EFFECT_PSYCHICSTATIC;
    static const int EFFECT_EFFECT_ICON;
    static const int EFFECT_ABILITY_STRENGTH;
    static const int EFFECT_ABILITY_DEXTERITY;
    static const int EFFECT_ABILITY_CONSTITUTION;
    static const int EFFECT_ABILITY_INTELLIGENCE;
    static const int EFFECT_ABILITY_WISDOM;
    static const int EFFECT_ABILITY_CHARISMA;
    static const int DAMAGE_FLAG_BLUDGEONING;
    static const int DAMAGE_FLAG_PIERCING;
    static const int DAMAGE_FLAG_SLASHING;
    static const int DAMAGE_FLAG_ACID;
    static const int DAMAGE_FLAG_COLD;
    static const int DAMAGE_FLAG_LIGHT_SIDE;
    static const int DAMAGE_FLAG_FIRE;
    static const int DAMAGE_FLAG_ELECTRICAL;
    static const int DAMAGE_FLAG_DARK_SIDE;
    static const int DAMAGE_FLAG_ION;
    static const int DAMAGE_FLAG_BLASTER;
    static const int DAMAGE_FLAG_UNIVERSAL;
    static const int DAMAGE_FLAG_SONIC;
    static const int DAMAGE_FLAG_POISON;
    static const int DAMAGE_FLAG_PHYSICAL;


    static const int DAMAGE_FLAG_BASE_DAMAGE;
    static const int DAMAGE_FLAG_ALL;
    static const int DAMAGE_POWER_NORMAL;
    static const int DAMAGE_POWER_ENERGY;
    static const int AC_DEFLECTION;


    static const int AC_DODGE;
    static const int AC_NATURAL;
    static const int AC_ARMOUR_BONUS;
    static const int AC_SHIELD_BONUS;
    static const int ATTACK_BONUS_MISC;
    static const int ATTACK_BONUS_ONHAND;
    static const int ATTACK_BONUS_OFFHAND;
    static const int ATTACK_BONUS_CWEAPON1;
    static const int ATTACK_BONUS_CWEAPON2;
    static const int ATTACK_BONUS_CWEAPON3;
    static const int ATTACK_BONUS_UNARMED;
    static const int DAMAGE_BONUS_MISC;
    static const int DAMAGE_BONUS_ONHAND;
    static const int DAMAGE_BONUS_OFFHAND;
    static const int DAMAGE_BONUS_CWEAPON1;
    static const int DAMAGE_BONUS_CWEAPON2;
    static const int DAMAGE_BONUS_CWEAPON3;
    static const int DAMAGE_BONUS_UNARMED;
    static const int IMMUNITY_TYPE_ALL;
    static const int IMMUNITY_TYPE_MIND_AFFECTING;
    static const int IMMUNITY_TYPE_POISON;
    static const int IMMUNITY_TYPE_DISEASE;
    static const int IMMUNITY_TYPE_ABILITY_DECREASE;
    static const int IMMUNITY_TYPE_KNOCKDOWN;
    static const int IMMUNITY_TYPE_NEGATIVE_LEVEL;
    static const int IMMUNITY_TYPE_BACKSTAB;
    static const int IMMUNITY_TYPE_DEATH_MAGIC;
    static const int IMMUNITY_TYPE_FEAR;
    static const int IMMUNITY_TYPE_MINE;
    static const int IMMUNITY_TYPE_PARALYSIS;


    static const int IMMUNITY_TYPE_BLINDNESS;
    static const int IMMUNITY_TYPE_DEAFNESS;
    static const int IMMUNITY_TYPE_SILENCE;
    static const int IMMUNITY_TYPE_CURSED;
    static const int IMMUNITY_TYPE_DAZED;

    static const int IMMUNITY_TYPE_STUN;
    static const int IMMUNITY_TYPE_SLEEP;
    static const int IMMUNITY_TYPE_CONFUSED;
    static const int IMMUNITY_TYPE_SLOW;
    static const int IMMUNITY_TYPE_ENTANGLE;
    static const int IMMUNITY_TYPE_MOVEMENT_SPEED_DECREASE;
    static const int IMMUNITY_TYPE_CRITICAL_HITS;
    static const int IMMUNITY_TYPE_MAX;
    static const int EFFECT_DAMAGE_DECREASE;
    static const int EFFECT_DAMAGE_IMMUNITY_DECREASE;
    static const int EFFECT_DAMAGE_IMMUNITY_INCREASE;
    static const int EFFECT_DAMAGE_INCREASE;
    static const int EFFECT_DAMAGE_REDUCTION;
    static const int EFFECT_DAMAGE_RESISTANCE;
    static const int EFFECT_DAMAGE_SHIELD;
    static const int EFFECT_DISEASE;
    static const int EFFECT_DISGUISE;
    static const int EFFECT_FORCEJUMP;
    static const int EFFECT_FORCE_RESISTANCE_DECREASE;
    static const int EFFECT_FORCE_RESISTANCE_INCREASE;
    static const int EFFECT_HASTE;
    static const int EFFECT_IMMUNITY;

    static const int MAX_EFFECTS;
    static const int EFFECT_SET_AI_STATE;
    static const int EFFECT_INVALIDEFFECT;
    static const int EFFECT_INVISIBILITY;
    static const int EFFECT_LINK;
    static const int EFFECT_LIGHTSABERTHROW;
    static const int EFFECT_MOVEMENT_SPEED_DECREASE;
    static const int EFFECT_MOVEMENT_SPEED_INCREASE;
    static const int EFFECT_POISON;
    static const int EFFECT_REGENERATE;
    static const int EFFECT_SAVING_THROW_DECREASE;
    static const int EFFECT_SAVING_THROW_INCREASE;
    static const int EFFECT_SEEINVISIBLE;
    static const int EFFECT_SETSTATE;
    static const int EFFECT_SKILL_DECREASE;
    static const int EFFECT_SKILL_INCREASE;
    static const int EFFECT_SLOW;
    static const int EFFECT_SPELL_IMMUNITY;
    static const int EFFECT_SPELL_LEVEL_ABSORPTION;
    static const int EFFECT_SUMMON_CREATURE;
    static const int EFFECT_SUMMON_PARTY;
    static const int EFFECT_TEMPORARY_HITPOINTS;
    static const int EFFECT_TIMESTOP;
    static const int EFFECT_TRUESEEING;
    static const int EFFECT_ASSUREDHIT;
    static const int EFFECT_ASSUREDDEFLECTION;
    static const int EFFECT_ENTANGLE;
    static const int EFFECT_DEAF;
    static const int EFFECT_ENEMY_ATTACK_BONUS;
    static const int EFFECT_ARCANE_SPELL_FAILURE;
    static const int EFFECT_CURSE;
    static const int EFFECT_SILENCE;
    static const int EFFECT_DISPEL_ALL_MAGIC;
    static const int EFFECT_DISPEL_BEST_MAGIC;
    static const int EFFECT_SANCTUARY;
    static const int EFFECT_ULTRAVISION;
    static const int EFFECT_BLINDNESS;
    static const int EFFECT_DARKNESS;
    static const int EFFECT_MISS_CHANCE;
    static const int EFFECT_NEGATIVE_LEVEL;
    static const int EFFECT_VISUALEFFECT;
    static const int EFFECT_RESURRECTION;
    static const int EFFECT_BONUSFEAT;
    static const int EFFECT_SETWALKANIMATION;
    static const int EFFECT_LIMIT_MOVEMENT_SPEED;
    static const int EFFECT_LIGHT;
    static const int MISS_CHANCE_TYPE_DARKNESS;
    static const int VFX_NONE;
};

#endif
