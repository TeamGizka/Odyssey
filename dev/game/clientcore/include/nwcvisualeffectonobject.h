#ifndef __NWCVISUALEFFECTONOBJECT_H__
#define __NWCVISUALEFFECTONOBJECT_H__

#include "exobase.h"
#include "math3d.h"

class CAurObject;
class CExoSoundSource;
class CSWCObject;

#define SPELL_VISUAL_DIRECTION_EPSILON_SQUARED 0.0001f

#define LIGHTSABER_THROW_TIME_SCALE 2000.0f

class CSWCVisualEffectOnObject
{
public:

    static const unsigned short VFX_NONE = 0xFFFF;

    enum StealthVisualEffect
    {
        VFX_STEALTH_FRAMEBUFFER = 8000,
        VFX_STEALTH_END = 8001,
        VFX_STEALTH_FALLBACK = 8002
    };

    enum { VFX_SINGLE_INSTANCE = 1020 };

    static const unsigned char HARDCODED_EFFECT_LIGHT = 4;

    static const unsigned char HARDCODED_EFFECT_BEAM = 7;

    static const unsigned char EFFECT_TYPE_SELFILLUMINATION = 20;
    enum SelfIlluminationEffect
    {
        SELFILLUMINATION_RED = 200,
        SELFILLUMINATION_GREEN = 201,
        SELFILLUMINATION_BLUE = 202,
        SELFILLUMINATION_PURPLE = 203,
        SELFILLUMINATION_YELLOW = 204,
        SELFILLUMINATION_WHITE = 205
    };

    static const unsigned char HARDCODED_EFFECT_ALPHA = 13;

    enum { SELFILLUMINATION_PULSE_PERIOD = 1000 };

    enum SelfIlluminationPulseEffect
    {
        SELFILLUMINATION_PULSE_RED = 500,
        SELFILLUMINATION_PULSE_BLUE = 501
    };

    enum { VFX_ALPHA_BEGIN = 400 };
    enum AlphaEffect
    {
        ALPHA_HALF = 0,
        ALPHA_FADE = 1,
        ALPHA_HALF_BLUE_HILITE = 2,
        ALPHA_HALF_PURPLE_HILITE = 3,
        ALPHA_FADE_PARTIAL = 4
    };

    enum { ALPHA_FADE_PERIOD = 6000, ALPHA_PARTIAL_FADE_PERIOD = 1000 };
    enum AlphaFadeState
    {
        ALPHA_FADING_OUT = 0,
        ALPHA_FADED_OUT = 1,
        ALPHA_FADING_IN = 2,
        ALPHA_FADED_IN = 3,
        ALPHA_FADE_STATE_COUNT = 4
    };
    enum { EFFECT_TYPE_STANDARD = 1 };

    enum BeamHook
    {
        BEAM_HOOK_HAND = 0,
        BEAM_HOOK_IMPACT = 1,
        BEAM_HOOK_HEAD = 2,
        BEAM_HOOK_LEFT_HAND = 3,
        BEAM_HOOK_RIGHT_HAND = 4
    };

    enum { CESSATION_ONLY_IMPACT_TIME = 1000 };

    enum
    {
        ATTACHED_MODEL_IMPACT_FALLBACK_TIME = 1000,
        HARDCODED_CESSATION_TIME = 1000,
        ATTACHED_MODEL_FADE_TIME = 3000
    };

    enum VisualEffectState
    {
        VISUAL_EFFECT_IMPACT = 1,
        VISUAL_EFFECT_DURATION = 2,
        VISUAL_EFFECT_CESSATION = 3
    };

    enum { FOV_ZOOM = 1500, FOV_RETURN_TIME = 4000 };

    enum { FOV_DURATION = 4750 };

    enum { VFX_SPELL_VISUAL_BEGIN = 1200 };
    enum SpellVisualEffect
    {
        SPELL_VISUAL_FIZZLE = 0,
        SPELL_VISUAL_FIZZLE_TIMED = 1,
        SPELL_VISUAL_RESIST = 2
    };

    enum { VFX_BEAM_BEGIN = 600 };
    enum BeamEffect
    {
        BEAM_LIGHTNING_SINGLE = 8,
        BEAM_LIGHTNING_MULTIPLE = 9,
        BEAM_DROID_DISABLE = 10,
        BEAM_DROID_KILL = 11,
        BEAM_DEATH_FIELD = 12,
        BEAM_DRAIN = 13,
        BEAM_FLAME = 14,
        BEAM_STUN_RAY = 15,
        BEAM_COLD_RAY = 16,
        BEAM_ION_RAY_1 = 17,
        BEAM_ION_RAY_2 = 18,
        BEAM_FORCE_STORM = 19,
        BEAM_DROID_STUN = 20,
        BEAM_FORCE_SHOCK = 21
    };

    enum { VFX_MODEL_ATTACHMENT_BEGIN = 1700 };
    enum ModelAttachmentEffect
    {
        MODEL_ATTACHMENT_MEDAL = 0,
        MODEL_ATTACHMENT_REVAN_MASK_1 = 1,
        MODEL_ATTACHMENT_REVAN_MASK_2 = 2
    };

    enum HardCodedEffectRange
    {
        VFX_ENVIRONMENT_MAP_BEGIN = 100,
        VFX_ENVIRONMENT_MAP_END = 200,
        VFX_SELFILLUMINATION_BEGIN = 200,
        VFX_SELFILLUMINATION_END = 300,
        VFX_LIGHT_SOURCE_BEGIN = 300,
        VFX_LIGHT_SOURCE_END = 400,
        VFX_ALPHA_END = 500,
        VFX_SELFILLUMINATION_PULSE_BEGIN = 500,
        VFX_SELFILLUMINATION_PULSE_END = 600,
        VFX_BEAM_END = 700,
        VFX_DELETE_MODEL_END = 800,
        VFX_CHUNK_END = 900,
        VFX_MIRV_BEGIN = 900,
        VFX_MIRV_END = 1000,
        VFX_TEXTURE_REPLACE_BEGIN = 1000,
        VFX_TEXTURE_REPLACE_END = 1100,
        VFX_CHEAT_END = 1200,
        VFX_SPELL_VISUAL_END = 1300,
        VFX_LIGHTSABER_THROW_END = 1400,
        VFX_BUMPED_OUT_SHELL_BEGIN = 1400,
        VFX_BUMPED_OUT_SHELL_END = 1500,
        VFX_FOV_BEGIN = 1500,
        VFX_FOV_END = 1600,
        VFX_MOTION_BLUR_BEGIN = 1600,
        VFX_MOTION_BLUR_END = 1700,
        VFX_MODEL_ATTACHMENT_END = 1800,
        VFX_DISTORTION_BEGIN = 1800,
        VFX_DISTORTION_END = 1900
    };

    enum MirvEffect
    {
        MIRV_MAGIC_MISSILE = 0,
        MIRV_FIRE_ARROW = 1,
        MIRV_ACID_ARROW = 2
    };

    enum CheatEffect
    {
        CHEAT_KUNG_POW = 1100
    };

    enum { LIGHT_SOURCE_REVERSE_BEGIN = 50 };
    enum LightSourceEffect
    {
        LIGHT_SOURCE_WHITE_5M = 0,
        LIGHT_SOURCE_WHITE_10M = 1,
        LIGHT_SOURCE_WHITE_15M = 2,
        LIGHT_SOURCE_WHITE_20M = 3,
        LIGHT_SOURCE_BLUE_5M = 4,
        LIGHT_SOURCE_BLUE_10M = 5,
        LIGHT_SOURCE_BLUE_15M = 6,
        LIGHT_SOURCE_BLUE_20M = 7,
        LIGHT_SOURCE_YELLOW_5M = 8,
        LIGHT_SOURCE_YELLOW_10M = 9,
        LIGHT_SOURCE_YELLOW_15M = 10,
        LIGHT_SOURCE_YELLOW_20M = 11,
        LIGHT_SOURCE_RED_5M = 12,
        LIGHT_SOURCE_RED_10M = 13,
        LIGHT_SOURCE_RED_15M = 14,
        LIGHT_SOURCE_RED_20M = 15,
        LIGHT_SOURCE_ORANGE_5M = 16,
        LIGHT_SOURCE_ORANGE_10M = 17,
        LIGHT_SOURCE_ORANGE_15M = 18,
        LIGHT_SOURCE_ORANGE_20M = 19,
        LIGHT_SOURCE_PURPLE_5M = 20,
        LIGHT_SOURCE_PURPLE_10M = 21,
        LIGHT_SOURCE_PURPLE_15M = 22,
        LIGHT_SOURCE_PURPLE_20M = 23,
        LIGHT_SOURCE_GREEN_5M = 24,
        LIGHT_SOURCE_GREEN_10M = 25,
        LIGHT_SOURCE_GREEN_15M = 26,
        LIGHT_SOURCE_GREEN_20M = 27,
        LIGHT_SOURCE_DARKVISION = 28,
        LIGHT_SOURCE_BLINDVISION = 29,
        LIGHT_SOURCE_LOWLIGHTVISION = 30,
        LIGHT_SOURCE_ULTRAVISION = 31,
        LIGHT_SOURCE_DARKNESS = 32,
        LIGHT_SOURCE_BLINDNESS = 33
    };

    enum LightSourceType
    {
        LIGHT_SOURCE_TYPE_NORMAL = 20,
        LIGHT_SOURCE_TYPE_ULTRAVISION = 49,
        LIGHT_SOURCE_TYPE_DARKNESS = 50,
        LIGHT_SOURCE_TYPE_DARKVISION = 51,
        LIGHT_SOURCE_TYPE_LOWLIGHTVISION = 52,
        LIGHT_SOURCE_TYPE_BLINDVISION = 53,
        LIGHT_SOURCE_TYPE_BLINDNESS = 13
    };

    enum { LIGHT_PRIORITY_PLAYER = 2, LIGHT_PRIORITY_OTHER_OBJECT = 4 };

    Vector m_vSelfIlluminationPulseStart;
    Vector m_vSelfIlluminationPulseEnd;
    Vector m_vSelfIlluminationPulseCurrent;
    CExoString m_sBeamTargetNode;
    CExoString m_sBeamSourceNode;
    Vector m_vOffset;
    unsigned long m_oidTargetObject;
    unsigned long m_oidCreator;
    unsigned long m_oidTarget;
    unsigned long m_anAttachedModelAnimationTime[3];
    unsigned short m_anAttachedModelAnimationState[3];
    unsigned long m_anVisualEffectModelAnimationTime[3];
    unsigned short m_anVisualEffectModelAnimationState[3];
    CAurObject* m_apAttachedModels[3];
    CAurObject* m_apVisualEffectModels[3];
    CAurObject* m_pEffectModel;
    int m_anAttachedModelFadeTime[3];
    unsigned long m_nHardCodedEffectTime;
    unsigned short m_anVisualEffect[3];
    int m_bVisualEffectShown;
    int m_bSelfIlluminationPulseDone;
    int m_bAlphaDone;
    int m_bFOVDone;
    int m_bCessationDone;
    int m_bVisualEffectStopped;
    int m_bVisualEffectDone;
    unsigned short m_nVisualEffect;
    unsigned char m_nVisualEffectState;
    unsigned char m_nAlphaState;
    char m_bPlayDurationAnimation;
    float m_fAlpha;
    float m_fAlphaTarget;
    int m_bLightSourceDynamic;
    unsigned char m_nEffectType;
    char m_HardCodedEffectClass;
    int m_bLightSourceShadows;
    int m_bLightSourceApplied;
    float m_fSavedFOV;
    float m_fSavedCameraDistance;
    CExoSoundSource* m_pSoundSource;
    int m_bRumblePerformed;
    int m_bHardCodedEffectApplied;

    int IsDone();
    void ApplySelfIlluminationPulse(unsigned short);
    void RemoveBeam(unsigned short);
    void SetEffectDone(CAurObject* pEffectModel);
    int GetHardCodedImpactLength(unsigned short nVisualEffect);

    int GetHardCodedCessationLength(unsigned short nVisualEffect);
    int LoadBeam(unsigned short);
    int LoadSpellVisual(unsigned short);
    int LoadModelAttachment(unsigned short);
    void UnloadHardCodedVisualEffect();
    char GetHardCodedEffectClass();
    char GetHardCodedEffectClass(unsigned short);
    CSWCVisualEffectOnObject();
    void UnloadVisualEffect();
    void DetachVisualEffect();
    void AttachBeam();
    void AttachLight();
    void ApplyBumpedOutShell(unsigned short);
    void RemoveBumpedOutShell();
    void ApplyMotionBlur(unsigned short);
    void RemoveMotionBlur();
    void ApplyDistortion(unsigned short);
    void RemoveDistortion(unsigned short);

    CSWCObject* GetTargetCSWCObject();
    CSWCObject* GetSourceCSWCObject();
    CSWCObject* GetOwnerCSWCObject();
    void AttachVFXModelToTargetModel(CAurObject*, CAurObject*,
                                    CExoString const&);
    ~CSWCVisualEffectOnObject();
    int LoadVisualEffect();
    int LoadVisualEffect(unsigned short nEffect, int nNode,
                         unsigned long oidObject, unsigned long oidCreator,
                         unsigned long oidTarget, unsigned char nBeamFlag,
                         unsigned char nFlags, Vector vOffset);
    void ApplyCheat(unsigned short);
    void ApplySpellVisual(unsigned short);
    void ApplyLightsaberThrow(unsigned short);
    void ApplyTextureReplace(unsigned short);
    void ApplyEnvironmentMap(unsigned short);
    void ApplySelfIllumination(unsigned short);
    void ApplyLightSource(unsigned short);
    void ApplyAlpha(unsigned short);
    int ApplyBeam(unsigned short);
    void ApplyDeleteModel(unsigned short);
    void ApplyChunk(unsigned short);
    void ApplyMirv(unsigned short);
    void ApplyFOV(unsigned short);
    void ApplyModelAttachment(unsigned short);
    void RemoveTextureReplace(unsigned short);
    void RemoveEnvironmentMap(unsigned short);
    void RemoveSelfIllumination(unsigned short);
    void RemoveSelfIlluminationPulse(unsigned short);
    void RemoveLightSource(unsigned short);
    void RemoveAlpha(unsigned short);
    void RemoveFOV(unsigned short, int);
    void UpdateSelfIlluminationPulse(unsigned long);
    void UpdateAlpha(unsigned long);
    void UpdateFOV(unsigned short, unsigned long);
    int LoadLightSource(unsigned short);
    void ReAttachVisualEffect();
    void ApplyHardCodedVisualEffect(unsigned short);
    void RemoveHardCodedVisualEffect(unsigned short);
    int LoadHardCodedVisualEffect(unsigned short);
    void UpdateMotionBlur(unsigned long);
    int ShowVisualEffect(int);
    void HideVisualEffect();
    void StartVisualEffect();
    void StopVisualEffect(int);

    void DoVFXRumble();
    void KillVisualEffect();
    void UpdateHardCodedVisualEffect(unsigned short, unsigned long);
    void Update(unsigned long);
    static short m_nMotionBlurVFXCount;
};

#endif
