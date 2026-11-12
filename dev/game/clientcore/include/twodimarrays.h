#ifndef __TWODIMARRAYS_H__
#define __TWODIMARRAYS_H__

class C2DA;

extern char g_nClientServerMode;

enum ClientServerMode
{
    CLIENT_SERVER_MODE_SERVER = 2
};

class CTwoDimArrays
{
public:

    enum { AREA_TRANSITION_FIRST_IMAGE = 2 };

    enum { RANGES_PARTY_RETREAT_ROW = 16 };

    enum CombatBroadcastRows
    {
        RANGES_COMBAT_BROADCAST_ROW = 11,
        EXCITED_DURATION_COMBAT_ROW = 1
    };

    enum { PLACEABLE_SOUND_BODY_BAG = 53 };

    enum FootstepType
    {
        FOOTSTEP_TYPE_STEALTH = 0,
        FOOTSTEP_TYPE_NO_VISUAL_6 = 6,
        FOOTSTEP_TYPE_NO_VISUAL_7 = 7
    };

    enum BodyBagDefaults
    {
        BODY_BAG_DEFAULT_ROW = 0,
        BODY_BAG_DEFAULT_APPEARANCE = 3
    };

    enum
    {
        IPRP_COST_TABLE_DC = 25,

        IPRP_COST_TABLE_MELEE = 20
    };

    enum AppearanceModelType
    {
        APPEARANCE_MODELTYPE_S = 'S',
        APPEARANCE_MODELTYPE_L = 'L',
        APPEARANCE_MODELTYPE_LOWER_S = 's',
        APPEARANCE_MODELTYPE_LOWER_L = 'l'
    };

    enum VisualEffectShakeType
    {
        VISUALEFFECT_SHAKE_NONE = 0,
        VISUALEFFECT_SHAKE_BUMP = 1,
        VISUALEFFECT_SHAKE_CAMERA = 2
    };

    enum CameraStyleRow
    {
        ROW2DA_CAMERASTYLE_COMBAT = 8
    };

    static int COL2DA_APPEARANCETABLE_ABORTONPARRY;
    static int COL2DA_APPEARANCETABLE_BACKUPHEAD;
    static int COL2DA_APPEARANCETABLE_CAMERAHEIGHTOFFSET;
    static int COL2DA_APPEARANCETABLE_CAMERASPACE;
    static int COL2DA_APPEARANCETABLE_CREPERSPACE;
    static int COL2DA_APPEARANCETABLE_DISABLEINJUREDANIM;
    static int COL2DA_APPEARANCETABLE_DRIVEACCl;
    static int COL2DA_APPEARANCETABLE_DRIVEMAXSPEED;
    static int COL2DA_APPEARANCETABLE_DRIVEANIMWALK;
    static int COL2DA_APPEARANCETABLE_DRIVEANIMRUN;
    static int COL2DA_APPEARANCETABLE_ENVMAP;
    static int COL2DA_APPEARANCETABLE_FOOTSTEPTYPE;
    static int COL2DA_APPEARANCETABLE_GROUNDTILT;
    static int COL2DA_APPEARANCETABLE_HEAD_ARC_H;
    static int COL2DA_APPEARANCETABLE_HEAD_ARC_V;
    static int COL2DA_APPEARANCETABLE_HEADBONE;
    static int COL2DA_APPEARANCETABLE_HEADTRACK;
    static int COL2DA_APPEARANCETABLE_HEIGHT;
    static int COL2DA_APPEARANCETABLE_HITDIST;
    static int COL2DA_APPEARANCETABLE_HITRADIUS;
    static int COL2DA_APPEARANCETABLE_MODELTYPE;
    static int COL2DA_APPEARANCETABLE_NORMALHEAD;
    static int COL2DA_APPEARANCETABLE_PERSPACE;
    static int COL2DA_APPEARANCETABLE_RACE;
    static int COL2DA_APPEARANCETABLE_RUNDIST;
    static int COL2DA_APPEARANCETABLE_SIZECATEGORY;
    static int COL2DA_APPEARANCETABLE_SOUNDAPPTYPE;
    static int COL2DA_APPEARANCETABLE_WALKDIST;
    static int COL2DA_APPEARANCESOUNDS_ARMORTYPE;
    static int COL2DA_APPEARANCESOUNDS_FALLDIRT;
    static int COL2DA_APPEARANCESOUNDS_FALLHARD;
    static int COL2DA_APPEARANCESOUNDS_FALLMETAL;
    static int COL2DA_APPEARANCESOUNDS_FALLWATER;
    static int COL2DA_APPEARANCESOUNDS_WEAPON;
    static int COL2DA_VISUALEFFECT_IMP_HEADCON_NODE;
    static int COL2DA_VISUALEFFECT_IMP_IMPACT_NODE;
    static int COL2DA_VISUALEFFECT_IMP_ROOT_H_NODE;
    static int COL2DA_VISUALEFFECT_IMP_ROOT_L_NODE;
    static int COL2DA_VISUALEFFECT_IMP_ROOT_M_NODE;
    static int COL2DA_VISUALEFFECT_IMP_ROOT_S_NODE;
    static int COL2DA_VISUALEFFECT_LOWQUALITY;
    static int COL2DA_VISUALEFFECT_ORIENTWITHGROUND;
    static int COL2DA_VISUALEFFECT_SHAKEDELAY;
    static int COL2DA_VISUALEFFECT_SHAKEDURATION;
    static int COL2DA_VISUALEFFECT_SHAKETYPE;
    static int COL2DA_VISUALEFFECT_SOUNDDURATION;
    static int COL2DA_VISUALEFFECT_SOUNDIMPACT;
    static int COL2DA_PERSISTENTVISUALEFFECT_DURATIONVFX;
    static int COL2DA_PERSISTENTVISUALEFFECT_ORIENTWITHGROUND;
    static int COL2DA_PERSISTENTVISUALEFFECT_SHAPE;
    static int COL2DA_PERSISTENTVISUALEFFECT_RADIUS;
    static int COL2DA_PERSISTENTVISUALEFFECT_WIDTH;
    static int COL2DA_PERSISTENTVISUALEFFECT_LENGTH;
    static int COL2DA_PERSISTENTVISUALEFFECT_MODELMIN01;
    static int COL2DA_PERSISTENTVISUALEFFECT_MODEL01;
    static int COL2DA_PERSISTENTVISUALEFFECT_MODELMIN02;
    static int COL2DA_PERSISTENTVISUALEFFECT_MODEL02;
    static int COL2DA_PERSISTENTVISUALEFFECT_MODELMIN03;
    static int COL2DA_PERSISTENTVISUALEFFECT_MODEL03;
    static int COL2DA_PERSISTENTVISUALEFFECT_NUMACT01;
    static int COL2DA_PERSISTENTVISUALEFFECT_NUMACT02;
    static int COL2DA_PERSISTENTVISUALEFFECT_NUMACT03;
    static int COL2DA_PERSISTENTVISUALEFFECT_DURATION01;
    static int COL2DA_PERSISTENTVISUALEFFECT_DURATION02;
    static int COL2DA_PERSISTENTVISUALEFFECT_DURATION03;
    static int COL2DA_PERSISTENTVISUALEFFECT_EDGEWGHT01;
    static int COL2DA_PERSISTENTVISUALEFFECT_EDGEWGHT02;
    static int COL2DA_PERSISTENTVISUALEFFECT_EDGEWGHT03;
    static int COL2DA_PERSISTENTVISUALEFFECT_SOUNDONESHOT;
    static int COL2DA_PERSISTENTVISUALEFFECT_SOUNDONESHOTPERCENTAGE;
    static int COL2DA_PLACEABLESOUNDS_ARMORTYPE;
    static int COL2DA_PLACEABLESOUNDS_LOCKED;
    static int COL2DA_PLACEABLESOUNDS_OPENED;
    static int COL2DA_PLACEABLESOUNDS_CLOSED;
    static int COL2DA_PLACEABLESOUNDS_DESTROYED;
    static int COL2DA_FOOTSTEPSOUNDS_PITCHOFFSET;
    static int COL2DA_FOOTSTEPSOUNDS_ROLLING;
    static int COL2DA_ANIMATIONS_ATTACK;
    static int COL2DA_ANIMATIONS_DAMAGE;
    static int COL2DA_ANIMATIONS_DIALOG;
    static int COL2DA_ANIMATIONS_DODGE;
    static int COL2DA_ANIMATIONS_FIREFORGET;
    static int COL2DA_ANIMATIONS_HIDEEQUIPPEDITEMS;
    static int COL2DA_ANIMATIONS_LOOPING;
    static int COL2DA_ANIMATIONS_NAME;
    static int COL2DA_ANIMATIONS_OVERLAY;
    static int COL2DA_ANIMATIONS_PARRY;
    static int COL2DA_ANIMATIONS_PAUSE;
    static int COL2DA_ANIMATIONS_RUNNING;
    static int COL2DA_ANIMATIONS_STATIONARY;
    static int COL2DA_ANIMATIONS_WALKING;
    static int COL2DA_DIALOGANIMATIONS_DIALOG;
    static int COL2DA_DIALOGANIMATIONS_FIREFORGET;
    static int COL2DA_DIALOGANIMATIONS_LOOPING;
    static int COL2DA_DIALOGANIMATIONS_OVERLAY;
    static int COL2DA_FORCEADJUST_GOODCOST;
    static int COL2DA_FORCEADJUST_EVILCOST;
    static int COL2DA_GENDER_GENDER;
    static int COL2DA_SURFACEMATERIAL_NAME;
    static int COL2DA_DOORTYPES_SOUNDAPPTYPE;
    static int COL2DA_GENERICDOORS_SOUNDAPPTYPE;
    static int COL2DA_PLACEABLES_SOUNDAPPTYPE;
    static int COL2DA_PLACEABLES_STRREF;
    static int COL2DA_PLACEABLES_IGNORESTATICHITCHECK;
    static int COL2DA_WEAPONSOUNDS_PITCHOFFSET;
    static int COL2DA_CREATURESPEED_RUNRATE;
    static int COL2DA_CREATURESPEED_WALKRATE;

    C2DA** m_ppIPRPCostTables;
    unsigned char m_nIPRPCostTableCount;
    unsigned char m_aUnknown05[3];
    C2DA** m_ppIPRPParamTables;
    unsigned char m_nIPRPParamTableCount;
    unsigned char m_aUnknown0D[3];
    C2DA* m_pAppearance;
    C2DA* m_pGender;
    C2DA* m_pSurfaceMaterial;
    C2DA* m_pVisualEffect;
    C2DA* m_pVisualEffectsPersistent;
    C2DA* m_pCreatureSpeed;
    C2DA* m_pDoorTypes;
    C2DA* m_pGenericDoors;
    C2DA* m_pPlaceables;
    C2DA* m_pAreaTransition;
    C2DA* m_pIPRPMelee;
    C2DA* m_pItemPropDef;
    C2DA* m_pPortrait;
    C2DA* m_pIPRPDamage;
    C2DA* m_pTraps;
    C2DA* m_pLightColor;
    C2DA* m_pCursor;
    C2DA* m_pAmbientMusic;
    C2DA* m_pAmbientSound;
    C2DA* m_pFootstepSounds;
    C2DA* m_pAppearanceSounds;
    C2DA* m_pWeaponSounds;
    C2DA* m_pAmmunitionTypes;
#ifndef _XBOX
    C2DA* m_pKeymap;
    C2DA* m_pBindableKey;
#endif
    C2DA* m_pPlaceableSounds;
    C2DA* m_pCameraStyle;
    C2DA* m_pTutorial;
    C2DA* m_pDifficultyOptions;
    C2DA* m_pGamma;
    C2DA* m_pStateScripts;
    C2DA* m_pPoison;
    C2DA* m_pDisease;
    C2DA* m_pRepAdjustments;
    C2DA* m_pFractionalCR;
    C2DA* m_pEncounterDifficulty;
    C2DA* m_pExcitedDuration;
    C2DA* m_pRegeneration;
    C2DA* m_pXpBase;
    C2DA* m_pIPRPMonsterDamage;
    C2DA* m_pIPRPBonusCost;
    C2DA* m_pIPRPSRCost;
    C2DA* m_pIPRPNeg5Cost;
    C2DA* m_pRanges;
    C2DA* m_pOnHit;
    C2DA* m_pOnHitDuration;
    C2DA* m_pDamageHitVisual;
    C2DA* m_pAnimations;
    C2DA* m_pBodyBag;
    C2DA* m_pForcePointAdjust;
    C2DA* m_pGameEffects;
    C2DA* m_pRepute;
    C2DA* m_pWeaponDischarge;
    C2DA* m_pDroidDischarge;
    C2DA* m_pPlotXP;
    C2DA* m_pCombatAnimations;
    C2DA* m_pDialogAnimations;
    C2DA* m_pNPC;
    C2DA* m_pSpells;
    C2DA* m_pFeat;
    C2DA* m_pFormations;
    C2DA* m_pGrenadeSound;
    C2DA* m_pJournal;
    C2DA* m_pFeedBackText;
    C2DA* m_pForceShields;
    C2DA* m_pHeads;
#ifdef _XBOX
    C2DA* m_pMinGlobalRim;
#endif
    C2DA* m_pItemValues;
    C2DA* m_pVideoEffects;
    C2DA* m_pMovies2DA;
    C2DA* m_pEffectIcon;
    C2DA* m_pRemoveFXOnDeath;
    C2DA* m_pTexPacks;
    C2DA* m_pSubrace;
    C2DA* m_pIPRPPC;
    C2DA* m_pInfluence;
    C2DA* m_pItemCreate;
    C2DA* m_pChemicalCreate;
    C2DA* m_pSwoopUpgrade;
    C2DA* m_pMusic;
    C2DA* m_pAlienVO;
    C2DA* m_pEmotion;
    C2DA* m_pAutoBalance;
    C2DA* m_pAIScripts;

    CTwoDimArrays();
    ~CTwoDimArrays();
    int Load2DArrays_Appearance();
    int Load2DArrays_AppearanceSounds();
    int Load2DArrays_Gender();
    int Load2DArrays_SurfaceMaterial();
    int Load2DArrays_VisualEffect();
    int Load2DArrays_PersistentVisualEffect();
    int Load2DArrays_CreatureSpeed();
    int Load2DArrays_DoorTypes();
    int Load2DArrays_GenericDoors();
    int Load2DArrays_Placeables();
    int Load2DArrays_PlaceableSounds();
    int Load2DArrays_AreaTransition();
    int Load2DArrays_IPRPMelee();
    int Load2DArrays_ItemPropDef();
    int Load2DArrays_Portrait();
    int Load2DArrays_IPRPDamage();
    int Load2DArrays_Traps();
    int Load2DArrays_LightColor();
    int Load2DArrays_Cursor();
    int Load2DArrays_AmbientMusic();
    int Load2DArrays_AmbientSound();
    int Load2DArrays_FootstepSounds();
    int Load2DArrays_WeaponSounds();
    int Load2DArrays_AmmunitionTypes();
    int Load2DArrays_Keymap();
    int Load2DArrays_BindableKey();
    int Load2DArrays_CameraStyle();
    int Load2DArrays_Tutorial();
    int Load2DArrays_DifficultyOptions();
    int Load2DArrays_Gamma();
    int Load2DArrays_StateScripts();
    int Load2DArrays_Poison();
    int Load2DArrays_Disease();
    int Load2DArrays_RepAdjustments();
    int Load2DArrays_FractionalCR();
    int Load2DArrays_EncounterDifficulty();
    int Load2DArrays_ExcitedDuration();
    int Load2DArrays_Regeneration();
    int Load2DArrays_XpBase();
    int Load2DArrays_IPRPMonsterDamage();
    int Load2DArrays_IPRPBonusCost();
    int Load2DArrays_IPRPSRCost();
    int Load2DArrays_IPRPNeg5Cost();
    int Load2DArrays_Ranges();
    int Load2DArrays_OnHit();
    int Load2DArrays_OnHitDuration();
    int Load2DArrays_DamageHitVisual();
    int Load2DArrays_Animations();
    int Load2DArrays_BodyBag();
    int Load2DArrays_ForceAdjust();
    int Load2DArrays_GameEffects();
    int Load2DArrays_Repute();
    int Load2DArrays_WeaponDischarge();
    int Load2DArrays_DroidDischarge();
    int Load2DArrays_PlotXP();
    int Load2DArrays_CombatAnimations();
    int Load2DArrays_DialogAnimations();
    int Load2DArrays_NPC();
    int Load2DArrays_Spells();
    int Load2DArrays_Feat();
    int Load2DArrays_Formations();
    int Load2DArrays_GrenadeSound();
    int Load2DArrays_Planetary();
    int Load2DArrays_FeedBackText();
    int Load2DArrays_ForceShields();
    int Load2DArrays_Heads();
    int Load2DArrays_ItemValues();
    int Load2DArrays_VideoEffects();
    int Load2DArrays_Movies();
    int Load2DArrays_EffectIcon();
    int Load2DArrays_RemoveFXOnDeath();
    int Load2DArrays_TexPacks();
    int Load2DArrays_Subrace();
    int Load2DArrays_IPRPPC();
    int Load2DArrays_Influence();
    int Load2DArrays_ItemCreate();
    int Load2DArrays_ChemicalCreate();
    int Load2DArrays_SwoopUpgrade();
    int Load2DArrays_Music();
    int Load2DArrays_AlienVO();
    int Load2DArrays_Emotion();
    int Load2DArrays_AutoBalance();
    int Load2DArrays_AIScripts();
    int LoadIPRPCostTables();
    int UnLoadIPRPCostTables();

    C2DA* GetCreatureSpeedTable() { return m_pCreatureSpeed; }
    C2DA* GetIPRPCostTable(unsigned char nTable);
    int LoadIPRPParamTables();
    int UnLoadIPRPParamTables();
    C2DA* GetIPRPParamTable(unsigned char nTable);

    C2DA* GetSurfaceMaterial() { return m_pSurfaceMaterial; }
    C2DA* GetAreaTransition() { return m_pAreaTransition; }

    C2DA* GetVisualEffect() { return m_pVisualEffect; }

    C2DA* GetFootstepSounds() { EXOXBOXASSERT(m_pFootstepSounds); return m_pFootstepSounds; }

    C2DA* GetAppearanceSounds() { return m_pAppearanceSounds; }

    C2DA* GetDialogAnimations() { return m_pDialogAnimations; }

    C2DA* GetAppearance() { return m_pAppearance; }

    C2DA* GetEffectIcon() { EXOXBOXASSERT(m_pEffectIcon); return m_pEffectIcon; }
    C2DA* GetStateScripts() { EXOXBOXASSERT(m_pStateScripts); return m_pStateScripts; }
    C2DA* GetFormations() { EXOXBOXASSERT(m_pFormations); return m_pFormations; }
    C2DA* GetTraps() { return m_pTraps; }
    C2DA* GetIPRPPC() { return m_pIPRPPC; }

    C2DA* GetSpells() { EXOXBOXASSERT(m_pSpells); return m_pSpells; }

    C2DA* GetRanges() { EXOXBOXASSERT(m_pRanges); return m_pRanges; }
    int Load2DArrays();
    int UnLoad2DArrays();
#ifdef _XBOX

    C2DA* GetIPRPMonsterDamage() { EXOXBOXASSERT(m_pIPRPMonsterDamage); return m_pIPRPMonsterDamage; }

    C2DA* GetPoison() { EXOXBOXASSERT(m_pPoison); return m_pPoison; }
    C2DA* GetDisease() { EXOXBOXASSERT(m_pDisease); return m_pDisease; }
#endif
};

#endif
