#include "twodimarrays.h"
#include "twodimarray.h"
// PC 0x0060EB50
CTwoDimArrays::CTwoDimArrays()
{
    m_ppIPRPCostTables = 0;
    m_nIPRPCostTableCount = 0;
    m_ppIPRPParamTables = 0;
    m_nIPRPParamTableCount = 0;
    m_pAppearance = 0;
    m_pGender = 0;
    m_pSurfaceMaterial = 0;
    m_pVisualEffect = 0;
    m_pVisualEffectsPersistent = 0;
    m_pCreatureSpeed = 0;
    m_pDoorTypes = 0;
    m_pGenericDoors = 0;
    m_pPlaceables = 0;
    m_pAreaTransition = 0;
    m_pIPRPMelee = 0;
    m_pItemPropDef = 0;
    m_pPortrait = 0;
    m_pIPRPDamage = 0;
    m_pTraps = 0;
    m_pLightColor = 0;
    m_pCursor = 0;
    m_pAmbientMusic = 0;
    m_pAmbientSound = 0;
    m_pFootstepSounds = 0;
    m_pAppearanceSounds = 0;
    m_pWeaponSounds = 0;
    m_pAmmunitionTypes = 0;
#ifndef _XBOX
    m_pKeymap = 0;
    m_pBindableKey = 0;
#endif
    m_pPlaceableSounds = 0;
    m_pCameraStyle = 0;
    m_pTutorial = 0;
    m_pDifficultyOptions = 0;
    m_pGamma = 0;
    m_pStateScripts = 0;
    m_pPoison = 0;
    m_pDisease = 0;
    m_pRepAdjustments = 0;
    m_pFractionalCR = 0;
    m_pEncounterDifficulty = 0;
    m_pExcitedDuration = 0;
    m_pRegeneration = 0;
    m_pXpBase = 0;
    m_pIPRPMonsterDamage = 0;
    m_pIPRPBonusCost = 0;
    m_pIPRPSRCost = 0;
    m_pIPRPNeg5Cost = 0;
    m_pRanges = 0;
    m_pOnHit = 0;
    m_pOnHitDuration = 0;
    m_pDamageHitVisual = 0;
    m_pAnimations = 0;
    m_pBodyBag = 0;
    m_pForcePointAdjust = 0;
    m_pGameEffects = 0;
    m_pRepute = 0;
    m_pWeaponDischarge = 0;
    m_pDroidDischarge = 0;
    m_pPlotXP = 0;
    m_pCombatAnimations = 0;
    m_pDialogAnimations = 0;
    m_pNPC = 0;
    m_pSpells = 0;
    m_pFeat = 0;
    m_pFormations = 0;
    m_pGrenadeSound = 0;
    m_pJournal = 0;
    m_pFeedBackText = 0;
    m_pForceShields = 0;
    m_pHeads = 0;
#ifdef _XBOX
    m_pMinGlobalRim = 0;
#endif
    m_pItemValues = 0;
    m_pVideoEffects = 0;
    m_pMovies2DA = 0;
    m_pEffectIcon = 0;
    m_pRemoveFXOnDeath = 0;
    m_pTexPacks = 0;
    m_pSubrace = 0;
    m_pIPRPPC = 0;
    m_pInfluence = 0;
    m_pItemCreate = 0;
    m_pChemicalCreate = 0;
    m_pSwoopUpgrade = 0;
    m_pMusic = 0;
    m_pAlienVO = 0;
    m_pEmotion = 0;
    m_pAutoBalance = 0;
    m_pAIScripts = 0;
    return;
}

CTwoDimArrays::~CTwoDimArrays()
{
    if (m_pRemoveFXOnDeath != 0)
    {
        delete m_pRemoveFXOnDeath;
    }
    m_pRemoveFXOnDeath = 0;
    if (m_pEffectIcon != 0)
    {
        delete m_pEffectIcon;
    }
    m_pEffectIcon = 0;
    if (m_pAppearance != 0)
    {
        delete m_pAppearance;
    }
    m_pAppearance = 0;
    if (m_pGender != 0)
    {
        delete m_pGender;
    }
    m_pGender = 0;
    if (m_pSurfaceMaterial != 0)
    {
        delete m_pSurfaceMaterial;
    }
    m_pSurfaceMaterial = 0;
    if (m_pStateScripts != 0)
    {
        delete m_pStateScripts;
    }
    m_pStateScripts = 0;
    if (m_pVisualEffect != 0)
    {
        delete m_pVisualEffect;
    }
    m_pVisualEffect = 0;
    if (m_pAreaTransition != 0)
    {
        delete m_pAreaTransition;
    }
    m_pAreaTransition = 0;
    if (m_pTraps != 0)
    {
        delete m_pTraps;
    }
    m_pTraps = 0;
    if (m_pVisualEffectsPersistent != 0)
    {
        delete m_pVisualEffectsPersistent;
    }
    m_pVisualEffectsPersistent = 0;
    if (m_pCreatureSpeed != 0)
    {
        delete m_pCreatureSpeed;
    }
    m_pCreatureSpeed = 0;
    if (m_pDoorTypes != 0)
    {
        delete m_pDoorTypes;
    }
    m_pDoorTypes = 0;
    if (m_pPlaceables != 0)
    {
        delete m_pPlaceables;
    }
    m_pPlaceables = 0;
    if (m_pGenericDoors != 0)
    {
        delete m_pGenericDoors;
    }
    m_pGenericDoors = 0;
    if (m_pPoison != 0)
    {
        delete m_pPoison;
    }
    m_pPoison = 0;
    if (m_pDisease != 0)
    {
        delete m_pDisease;
    }
    m_pDisease = 0;
    if (m_pRepAdjustments != 0)
    {
        delete m_pRepAdjustments;
    }
    m_pRepAdjustments = 0;
    if (m_pFractionalCR != 0)
    {
        delete m_pFractionalCR;
    }
    m_pFractionalCR = 0;
    if (m_pEncounterDifficulty != 0)
    {
        delete m_pEncounterDifficulty;
    }
    m_pEncounterDifficulty = 0;
    if (m_pExcitedDuration != 0)
    {
        delete m_pExcitedDuration;
    }
    m_pExcitedDuration = 0;
    if (m_pRegeneration != 0)
    {
        delete m_pRegeneration;
    }
    m_pRegeneration = 0;
    if (m_pLightColor != 0)
    {
        delete m_pLightColor;
    }
    m_pLightColor = 0;
    if (m_pIPRPMelee != 0)
    {
        delete m_pIPRPMelee;
    }
    m_pIPRPMelee = 0;
    if (m_pXpBase != 0)
    {
        delete m_pXpBase;
    }
    m_pXpBase = 0;
    if (m_pCursor != 0)
    {
        delete m_pCursor;
    }
    m_pCursor = 0;
    if (m_pIPRPMonsterDamage != 0)
    {
        delete m_pIPRPMonsterDamage;
    }
    m_pIPRPMonsterDamage = 0;
    if (m_pIPRPDamage != 0)
    {
        delete m_pIPRPDamage;
    }
    m_pIPRPDamage = 0;
    if (m_pIPRPBonusCost != 0)
    {
        delete m_pIPRPBonusCost;
    }
    m_pIPRPBonusCost = 0;
    if (m_pIPRPSRCost != 0)
    {
        delete m_pIPRPSRCost;
    }
    m_pIPRPSRCost = 0;
    if (m_pIPRPNeg5Cost != 0)
    {
        delete m_pIPRPNeg5Cost;
    }
    m_pIPRPNeg5Cost = 0;
    if (m_pRanges != 0)
    {
        delete m_pRanges;
    }
    m_pRanges = 0;
    if (m_pItemPropDef != 0)
    {
        delete m_pItemPropDef;
    }
    m_pItemPropDef = 0;
    delete[] m_ppIPRPCostTables;
    m_ppIPRPCostTables = 0;
    m_nIPRPCostTableCount = 0;
    delete[] m_ppIPRPParamTables;
    m_ppIPRPParamTables = 0;
    m_nIPRPParamTableCount = 0;
    if (m_pAmbientMusic != 0)
    {
        delete m_pAmbientMusic;
    }
    m_pAmbientMusic = 0;
    if (m_pAmbientSound != 0)
    {
        delete m_pAmbientSound;
    }
    m_pAmbientSound = 0;
    if (m_pFootstepSounds != 0)
    {
        delete m_pFootstepSounds;
    }
    m_pFootstepSounds = 0;
    if (m_pAppearanceSounds != 0)
    {
        delete m_pAppearanceSounds;
    }
    m_pAppearanceSounds = 0;
    if (m_pWeaponSounds != 0)
    {
        delete m_pWeaponSounds;
    }
    m_pWeaponSounds = 0;
    if (m_pAmmunitionTypes != 0)
    {
        delete m_pAmmunitionTypes;
    }
    m_pAmmunitionTypes = 0;
#ifndef _XBOX
    if (m_pKeymap != 0)
    {
        delete m_pKeymap;
    }
    m_pKeymap = 0;
    if (m_pBindableKey != 0)
    {
        delete m_pBindableKey;
    }
    m_pBindableKey = 0;
#endif
    if (m_pOnHit != 0)
    {
        delete m_pOnHit;
    }
    m_pOnHit = 0;
    if (m_pOnHitDuration != 0)
    {
        delete m_pOnHitDuration;
    }
    m_pOnHitDuration = 0;
    if (m_pDamageHitVisual != 0)
    {
        delete m_pDamageHitVisual;
    }
    m_pDamageHitVisual = 0;
    if (m_pPlaceableSounds != 0)
    {
        delete m_pPlaceableSounds;
    }
    m_pPlaceableSounds = 0;
    if (m_pBodyBag != 0)
    {
        delete m_pBodyBag;
    }
    m_pBodyBag = 0;
    if (m_pPortrait != 0)
    {
        delete m_pPortrait;
    }
    m_pPortrait = 0;
    if (m_pForcePointAdjust != 0)
    {
        delete m_pForcePointAdjust;
    }
    m_pForcePointAdjust = 0;
    if (m_pGameEffects != 0)
    {
        delete m_pGameEffects;
    }
    m_pGameEffects = 0;
    if (m_pCameraStyle != 0)
    {
        delete m_pCameraStyle;
    }
    m_pCameraStyle = 0;
    if (m_pRepute != 0)
    {
        delete m_pRepute;
    }
    m_pRepute = 0;
    if (m_pWeaponDischarge != 0)
    {
        delete m_pWeaponDischarge;
    }
    m_pWeaponDischarge = 0;
    if (m_pDroidDischarge != 0)
    {
        delete m_pDroidDischarge;
    }
    m_pDroidDischarge = 0;
    if (m_pPlotXP != 0)
    {
        delete m_pPlotXP;
    }
    m_pPlotXP = 0;
    if (m_pAnimations != 0)
    {
        delete m_pAnimations;
    }
    m_pAnimations = 0;
    if (m_pCombatAnimations != 0)
    {
        delete m_pCombatAnimations;
    }
    m_pCombatAnimations = 0;
    if (m_pDialogAnimations != 0)
    {
        delete m_pDialogAnimations;
    }
    m_pDialogAnimations = 0;
    if (m_pNPC != 0)
    {
        delete m_pNPC;
    }
    m_pNPC = 0;
    if (m_pSpells != 0)
    {
        delete m_pSpells;
    }
    m_pSpells = 0;
    if (m_pFeat != 0)
    {
        delete m_pFeat;
    }
    m_pFeat = 0;
    if (m_pFormations != 0)
    {
        delete m_pFormations;
    }
    m_pFormations = 0;
    if (m_pGrenadeSound != 0)
    {
        delete m_pGrenadeSound;
    }
    m_pGrenadeSound = 0;
    if (m_pJournal != 0)
    {
        delete m_pJournal;
    }
    m_pJournal = 0;
    if (m_pFeedBackText != 0)
    {
        delete m_pFeedBackText;
    }
    m_pFeedBackText = 0;
    if (m_pForceShields != 0)
    {
        delete m_pForceShields;
    }
    m_pForceShields = 0;
    if (m_pHeads != 0)
    {
        delete m_pHeads;
    }
    m_pHeads = 0;
    if (m_pItemValues != 0)
    {
        delete m_pItemValues;
    }
    m_pItemValues = 0;
    if (m_pVideoEffects != 0)
    {
        delete m_pVideoEffects;
    }
    m_pVideoEffects = 0;
    if (m_pTutorial != 0)
    {
        delete m_pTutorial;
    }
    m_pTutorial = 0;
#ifndef _XBOX
    if (m_pDifficultyOptions != 0)
    {
        delete m_pDifficultyOptions;
    }
    m_pDifficultyOptions = 0;
    if (m_pGamma != 0)
    {
        delete m_pGamma;
    }
    m_pGamma = 0;
#endif
    if (m_pMovies2DA != 0)
    {
        delete m_pMovies2DA;
    }
    m_pMovies2DA = 0;
#ifndef _XBOX
    if (m_pTexPacks != 0)
    {
        delete m_pTexPacks;
    }
    m_pTexPacks = 0;
#else
    if (m_pMinGlobalRim != 0)
    {
        delete m_pMinGlobalRim;
    }
    m_pMinGlobalRim = 0;
#endif
}

int CTwoDimArrays::COL2DA_APPEARANCETABLE_ABORTONPARRY = -1;
int CTwoDimArrays::COL2DA_APPEARANCETABLE_BACKUPHEAD = -1;
int CTwoDimArrays::COL2DA_APPEARANCETABLE_CAMERAHEIGHTOFFSET = -1;
int CTwoDimArrays::COL2DA_APPEARANCETABLE_CAMERASPACE = -1;
int CTwoDimArrays::COL2DA_APPEARANCETABLE_CREPERSPACE = -1;
int CTwoDimArrays::COL2DA_APPEARANCETABLE_DISABLEINJUREDANIM = -1;
int CTwoDimArrays::COL2DA_APPEARANCETABLE_DRIVEACCl = -1;
int CTwoDimArrays::COL2DA_APPEARANCETABLE_DRIVEMAXSPEED = -1;
int CTwoDimArrays::COL2DA_APPEARANCETABLE_DRIVEANIMWALK = -1;
int CTwoDimArrays::COL2DA_APPEARANCETABLE_DRIVEANIMRUN = -1;
int CTwoDimArrays::COL2DA_APPEARANCETABLE_ENVMAP = -1;
int CTwoDimArrays::COL2DA_APPEARANCETABLE_FOOTSTEPTYPE = -1;
int CTwoDimArrays::COL2DA_APPEARANCETABLE_GROUNDTILT = -1;
int CTwoDimArrays::COL2DA_APPEARANCETABLE_HEAD_ARC_H = -1;
int CTwoDimArrays::COL2DA_APPEARANCETABLE_HEAD_ARC_V = -1;
int CTwoDimArrays::COL2DA_APPEARANCETABLE_HEADBONE = -1;
int CTwoDimArrays::COL2DA_APPEARANCETABLE_HEADTRACK = -1;
int CTwoDimArrays::COL2DA_APPEARANCETABLE_HEIGHT = -1;
int CTwoDimArrays::COL2DA_APPEARANCETABLE_HITDIST = -1;
int CTwoDimArrays::COL2DA_APPEARANCETABLE_HITRADIUS = -1;
int CTwoDimArrays::COL2DA_APPEARANCETABLE_MODELTYPE = -1;
int CTwoDimArrays::COL2DA_APPEARANCETABLE_NORMALHEAD = -1;
int CTwoDimArrays::COL2DA_APPEARANCETABLE_PERSPACE = -1;
int CTwoDimArrays::COL2DA_APPEARANCETABLE_RACE = -1;
int CTwoDimArrays::COL2DA_APPEARANCETABLE_RUNDIST = -1;
int CTwoDimArrays::COL2DA_APPEARANCETABLE_SIZECATEGORY = -1;
int CTwoDimArrays::COL2DA_APPEARANCETABLE_SOUNDAPPTYPE = -1;
int CTwoDimArrays::COL2DA_APPEARANCETABLE_WALKDIST = -1;
int CTwoDimArrays::COL2DA_APPEARANCESOUNDS_FALLDIRT = -1;
int CTwoDimArrays::COL2DA_APPEARANCESOUNDS_FALLMETAL = -1;
int CTwoDimArrays::COL2DA_APPEARANCESOUNDS_FALLHARD = -1;
int CTwoDimArrays::COL2DA_APPEARANCESOUNDS_FALLWATER = -1;
int CTwoDimArrays::COL2DA_APPEARANCESOUNDS_WEAPON = -1;
int CTwoDimArrays::COL2DA_APPEARANCESOUNDS_ARMORTYPE = -1;
int CTwoDimArrays::COL2DA_GENDER_GENDER = -1;
int CTwoDimArrays::COL2DA_ANIMATIONS_FIREFORGET = -1;
int CTwoDimArrays::COL2DA_ANIMATIONS_OVERLAY = -1;
int CTwoDimArrays::COL2DA_ANIMATIONS_LOOPING = -1;
int CTwoDimArrays::COL2DA_ANIMATIONS_STATIONARY = -1;
int CTwoDimArrays::COL2DA_ANIMATIONS_PAUSE = -1;
int CTwoDimArrays::COL2DA_ANIMATIONS_WALKING = -1;
int CTwoDimArrays::COL2DA_ANIMATIONS_RUNNING = -1;
int CTwoDimArrays::COL2DA_ANIMATIONS_DAMAGE = -1;
int CTwoDimArrays::COL2DA_ANIMATIONS_PARRY = -1;
int CTwoDimArrays::COL2DA_ANIMATIONS_DIALOG = -1;
int CTwoDimArrays::COL2DA_ANIMATIONS_NAME = -1;
int CTwoDimArrays::COL2DA_ANIMATIONS_DODGE = -1;
int CTwoDimArrays::COL2DA_ANIMATIONS_ATTACK = -1;
int CTwoDimArrays::COL2DA_ANIMATIONS_HIDEEQUIPPEDITEMS = -1;
int CTwoDimArrays::COL2DA_FORCEADJUST_GOODCOST = -1;
int CTwoDimArrays::COL2DA_FORCEADJUST_EVILCOST = -1;
int CTwoDimArrays::COL2DA_DIALOGANIMATIONS_FIREFORGET = -1;
int CTwoDimArrays::COL2DA_DIALOGANIMATIONS_DIALOG = -1;
int CTwoDimArrays::COL2DA_DIALOGANIMATIONS_LOOPING = -1;
int CTwoDimArrays::COL2DA_DIALOGANIMATIONS_OVERLAY = -1;
int CTwoDimArrays::COL2DA_FOOTSTEPSOUNDS_ROLLING = -1;
int CTwoDimArrays::COL2DA_FOOTSTEPSOUNDS_PITCHOFFSET = -1;
int CTwoDimArrays::COL2DA_WEAPONSOUNDS_PITCHOFFSET = -1;
int CTwoDimArrays::COL2DA_REGENERATION_HEALTHREGEN = -1;
int CTwoDimArrays::COL2DA_REGENERATION_FORCEREGEN = -1;
int CTwoDimArrays::COL2DA_SURFACEMATERIAL_NAME = -1;
int CTwoDimArrays::COL2DA_VISUALEFFECT_IMP_HEADCON_NODE = -1;
int CTwoDimArrays::COL2DA_VISUALEFFECT_IMP_IMPACT_NODE = -1;
int CTwoDimArrays::COL2DA_VISUALEFFECT_IMP_ROOT_M_NODE = -1;
int CTwoDimArrays::COL2DA_VISUALEFFECT_IMP_ROOT_S_NODE = -1;
int CTwoDimArrays::COL2DA_VISUALEFFECT_IMP_ROOT_L_NODE = -1;
int CTwoDimArrays::COL2DA_VISUALEFFECT_IMP_ROOT_H_NODE = -1;
int CTwoDimArrays::COL2DA_VISUALEFFECT_LOWQUALITY = -1;
int CTwoDimArrays::COL2DA_VISUALEFFECT_ORIENTWITHGROUND = -1;
int CTwoDimArrays::COL2DA_VISUALEFFECT_SHAKETYPE = -1;
int CTwoDimArrays::COL2DA_VISUALEFFECT_SHAKEDURATION = -1;
int CTwoDimArrays::COL2DA_VISUALEFFECT_SHAKEDELAY = -1;
int CTwoDimArrays::COL2DA_VISUALEFFECT_SOUNDIMPACT = -1;
int CTwoDimArrays::COL2DA_VISUALEFFECT_SOUNDDURATION = -1;
int CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_DURATIONVFX = -1;
int CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_ORIENTWITHGROUND = -1;
int CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_SHAPE = -1;
int CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_RADIUS = -1;
int CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_WIDTH = -1;
int CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_LENGTH = -1;
int CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_MODELMIN01 = -1;
int CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_MODEL01 = -1;
int CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_MODELMIN02 = -1;
int CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_MODEL02 = -1;
int CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_MODELMIN03 = -1;
int CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_MODEL03 = -1;
int CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_NUMACT01 = -1;
int CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_NUMACT02 = -1;
int CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_NUMACT03 = -1;
int CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_DURATION01 = -1;
int CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_DURATION02 = -1;
int CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_DURATION03 = -1;
int CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_EDGEWGHT01 = -1;
int CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_EDGEWGHT02 = -1;
int CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_EDGEWGHT03 = -1;
int CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_SOUNDONESHOT = -1;
int CTwoDimArrays::COL2DA_PERSISTENTVISUALEFFECT_SOUNDONESHOTPERCENTAGE = -1;
int CTwoDimArrays::COL2DA_CREATURESPEED_RUNRATE = -1;
int CTwoDimArrays::COL2DA_CREATURESPEED_WALKRATE = -1;
int CTwoDimArrays::COL2DA_DOORTYPES_SOUNDAPPTYPE = -1;
int CTwoDimArrays::COL2DA_GENERICDOORS_SOUNDAPPTYPE = -1;
int CTwoDimArrays::COL2DA_PLACEABLES_SOUNDAPPTYPE = -1;
int CTwoDimArrays::COL2DA_PLACEABLES_STRREF = -1;
int CTwoDimArrays::COL2DA_PLACEABLES_IGNORESTATICHITCHECK = -1;
int CTwoDimArrays::COL2DA_PLACEABLESOUNDS_ARMORTYPE = -1;
int CTwoDimArrays::COL2DA_PLACEABLESOUNDS_LOCKED = -1;
int CTwoDimArrays::COL2DA_PLACEABLESOUNDS_OPENED = -1;
int CTwoDimArrays::COL2DA_PLACEABLESOUNDS_CLOSED = -1;
int CTwoDimArrays::COL2DA_PLACEABLESOUNDS_DESTROYED = -1;
int CTwoDimArrays::Load2DArrays_Appearance()  // PC 0x0060ED00
{
    m_pAppearance = new C2DA("Appearance", 0);
    if (m_pAppearance == 0 || m_pAppearance->Load2DArray() == 0)
    {
        return 0;
    }

    COL2DA_APPEARANCETABLE_ABORTONPARRY =
        m_pAppearance->GetColumnIndex("ABORTONPARRY");
    {
        CExoString sColumn("BACKUPHEAD");
        C2DA* p2DA = m_pAppearance;
        COL2DA_APPEARANCETABLE_BACKUPHEAD = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("CameraHeightOffset");
        C2DA* p2DA = m_pAppearance;
        COL2DA_APPEARANCETABLE_CAMERAHEIGHTOFFSET = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("CAMERASPACE");
        C2DA* p2DA = m_pAppearance;
        COL2DA_APPEARANCETABLE_CAMERASPACE = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("CREPERSPACE");
        C2DA* p2DA = m_pAppearance;
        COL2DA_APPEARANCETABLE_CREPERSPACE = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("DisableInjuredAnim");
        C2DA* p2DA = m_pAppearance;
        COL2DA_APPEARANCETABLE_DISABLEINJUREDANIM = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("DRIVEACCl");
        C2DA* p2DA = m_pAppearance;
        COL2DA_APPEARANCETABLE_DRIVEACCl = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("DriveAnimRun");
        C2DA* p2DA = m_pAppearance;
        COL2DA_APPEARANCETABLE_DRIVEANIMRUN = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("DriveAnimWalk");
        C2DA* p2DA = m_pAppearance;
        COL2DA_APPEARANCETABLE_DRIVEANIMWALK = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("DriveMaxSpeed");
        C2DA* p2DA = m_pAppearance;
        COL2DA_APPEARANCETABLE_DRIVEMAXSPEED = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("ENVMAP");
        C2DA* p2DA = m_pAppearance;
        COL2DA_APPEARANCETABLE_ENVMAP = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("FootstepType");
        C2DA* p2DA = m_pAppearance;
        COL2DA_APPEARANCETABLE_FOOTSTEPTYPE = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("HEAD_ARC_H");
        C2DA* p2DA = m_pAppearance;
        COL2DA_APPEARANCETABLE_HEAD_ARC_H = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("HEAD_ARC_V");
        C2DA* p2DA = m_pAppearance;
        COL2DA_APPEARANCETABLE_HEAD_ARC_V = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("HEADBONE");
        C2DA* p2DA = m_pAppearance;
        COL2DA_APPEARANCETABLE_HEADBONE = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("HEADTRACK");
        C2DA* p2DA = m_pAppearance;
        COL2DA_APPEARANCETABLE_HEADTRACK = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("HEIGHT");
        C2DA* p2DA = m_pAppearance;
        COL2DA_APPEARANCETABLE_HEIGHT = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("HITRADIUS");
        C2DA* p2DA = m_pAppearance;
        COL2DA_APPEARANCETABLE_HITRADIUS = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("hitdist");
        C2DA* p2DA = m_pAppearance;
        COL2DA_APPEARANCETABLE_HITDIST = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("GROUNDTILT");
        C2DA* p2DA = m_pAppearance;
        COL2DA_APPEARANCETABLE_GROUNDTILT = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("MODELTYPE");
        C2DA* p2DA = m_pAppearance;
        COL2DA_APPEARANCETABLE_MODELTYPE = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("NORMALHEAD");
        C2DA* p2DA = m_pAppearance;
        COL2DA_APPEARANCETABLE_NORMALHEAD = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("PERSPACE");
        C2DA* p2DA = m_pAppearance;
        COL2DA_APPEARANCETABLE_PERSPACE = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("RACE");
        C2DA* p2DA = m_pAppearance;
        COL2DA_APPEARANCETABLE_RACE = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("RUNDIST");
        C2DA* p2DA = m_pAppearance;
        COL2DA_APPEARANCETABLE_RUNDIST = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("SIZECATEGORY");
        C2DA* p2DA = m_pAppearance;
        COL2DA_APPEARANCETABLE_SIZECATEGORY = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("SoundAppType");
        C2DA* p2DA = m_pAppearance;
        COL2DA_APPEARANCETABLE_SOUNDAPPTYPE = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("WALKDIST");
        C2DA* p2DA = m_pAppearance;
        COL2DA_APPEARANCETABLE_WALKDIST = p2DA->GetColumnIndex(sColumn);
    }
    return 1;
}
// PC 0x0060F380
int CTwoDimArrays::Load2DArrays_AppearanceSounds()
{
    m_pAppearanceSounds = new C2DA("AppearanceSndSet", 0);
    if (m_pAppearanceSounds == 0 || m_pAppearanceSounds->Load2DArray() == 0)
    {
        return 0;
    }

    COL2DA_APPEARANCESOUNDS_ARMORTYPE =
        m_pAppearanceSounds->GetColumnIndex("ArmorType");
    {
        CExoString sColumn("FallDirt");
        C2DA* p2DA = m_pAppearanceSounds;
        COL2DA_APPEARANCESOUNDS_FALLDIRT = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("FallHard");
        C2DA* p2DA = m_pAppearanceSounds;
        COL2DA_APPEARANCESOUNDS_FALLHARD = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("FallMetal");
        C2DA* p2DA = m_pAppearanceSounds;
        COL2DA_APPEARANCESOUNDS_FALLMETAL = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("FallWater");
        C2DA* p2DA = m_pAppearanceSounds;
        COL2DA_APPEARANCESOUNDS_FALLWATER = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("Weapon");
        C2DA* p2DA = m_pAppearanceSounds;
        COL2DA_APPEARANCESOUNDS_WEAPON = p2DA->GetColumnIndex(sColumn);
    }
    return 1;
}
// PC 0x0060F640
int CTwoDimArrays::Load2DArrays_Gender()
{
    m_pGender = new C2DA("GENDER", 0);
    if (m_pGender == 0 || m_pGender->Load2DArray() == 0)
    {
        return 0;
    }

    COL2DA_GENDER_GENDER = m_pGender->GetColumnIndex("GENDER");
    return 1;
}
// PC 0x0060F570
int CTwoDimArrays::Load2DArrays_SurfaceMaterial()
{
    m_pSurfaceMaterial = new C2DA("SURFACEMAT", 0);
    if (m_pSurfaceMaterial == 0 || m_pSurfaceMaterial->Load2DArray() == 0)
    {
        return 0;
    }

    COL2DA_SURFACEMATERIAL_NAME = m_pSurfaceMaterial->GetColumnIndex("Name");
    return 1;
}
// PC 0x0060F710
int CTwoDimArrays::Load2DArrays_VisualEffect()
{
    m_pVisualEffect = new C2DA("visualeffects", 0);
    if (m_pVisualEffect == 0 || m_pVisualEffect->Load2DArray() == 0)
    {
        return 0;
    }

    COL2DA_VISUALEFFECT_IMP_HEADCON_NODE =
        m_pVisualEffect->GetColumnIndex("Imp_HeadCon_Node");
    {
        CExoString sColumn("Imp_Impact_Node");
        C2DA* p2DA = m_pVisualEffect;
        COL2DA_VISUALEFFECT_IMP_IMPACT_NODE = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("Imp_Root_H_Node");
        C2DA* p2DA = m_pVisualEffect;
        COL2DA_VISUALEFFECT_IMP_ROOT_H_NODE = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("Imp_Root_L_Node");
        C2DA* p2DA = m_pVisualEffect;
        COL2DA_VISUALEFFECT_IMP_ROOT_L_NODE = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("Imp_Root_M_Node");
        C2DA* p2DA = m_pVisualEffect;
        COL2DA_VISUALEFFECT_IMP_ROOT_M_NODE = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("Imp_Root_S_Node");
        C2DA* p2DA = m_pVisualEffect;
        COL2DA_VISUALEFFECT_IMP_ROOT_S_NODE = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("LowQuality");
        C2DA* p2DA = m_pVisualEffect;
        COL2DA_VISUALEFFECT_LOWQUALITY = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("OrientWithGround");
        C2DA* p2DA = m_pVisualEffect;
        COL2DA_VISUALEFFECT_ORIENTWITHGROUND = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("ShakeDelay");
        C2DA* p2DA = m_pVisualEffect;
        COL2DA_VISUALEFFECT_SHAKEDELAY = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("ShakeDuration");
        C2DA* p2DA = m_pVisualEffect;
        COL2DA_VISUALEFFECT_SHAKEDURATION = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("ShakeType");
        C2DA* p2DA = m_pVisualEffect;
        COL2DA_VISUALEFFECT_SHAKETYPE = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("SoundDuration");
        C2DA* p2DA = m_pVisualEffect;
        COL2DA_VISUALEFFECT_SOUNDDURATION = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("SoundImpact");
        C2DA* p2DA = m_pVisualEffect;
        COL2DA_VISUALEFFECT_SOUNDIMPACT = p2DA->GetColumnIndex(sColumn);
    }
    return 1;
}
// PC 0x0060FA70
int CTwoDimArrays::Load2DArrays_PersistentVisualEffect()
{
    m_pVisualEffectsPersistent = new C2DA("VFX_Persistent", 0);
    if (m_pVisualEffectsPersistent == 0 ||
        m_pVisualEffectsPersistent->Load2DArray() == 0)
    {
        return 0;
    }

    COL2DA_PERSISTENTVISUALEFFECT_DURATIONVFX =
        m_pVisualEffectsPersistent->GetColumnIndex("DurationVFX");
    {
        CExoString sColumn("OrientWithGround");
        C2DA* p2DA = m_pVisualEffectsPersistent;
        COL2DA_PERSISTENTVISUALEFFECT_ORIENTWITHGROUND = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("SHAPE");
        C2DA* p2DA = m_pVisualEffectsPersistent;
        COL2DA_PERSISTENTVISUALEFFECT_SHAPE = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("RADIUS");
        C2DA* p2DA = m_pVisualEffectsPersistent;
        COL2DA_PERSISTENTVISUALEFFECT_RADIUS = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("WIDTH");
        C2DA* p2DA = m_pVisualEffectsPersistent;
        COL2DA_PERSISTENTVISUALEFFECT_WIDTH = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("LENGTH");
        C2DA* p2DA = m_pVisualEffectsPersistent;
        COL2DA_PERSISTENTVISUALEFFECT_LENGTH = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("MODELMIN01");
        C2DA* p2DA = m_pVisualEffectsPersistent;
        COL2DA_PERSISTENTVISUALEFFECT_MODELMIN01 = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("MODEL01");
        C2DA* p2DA = m_pVisualEffectsPersistent;
        COL2DA_PERSISTENTVISUALEFFECT_MODEL01 = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("MODELMIN02");
        C2DA* p2DA = m_pVisualEffectsPersistent;
        COL2DA_PERSISTENTVISUALEFFECT_MODELMIN02 = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("MODEL02");
        C2DA* p2DA = m_pVisualEffectsPersistent;
        COL2DA_PERSISTENTVISUALEFFECT_MODEL02 = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("MODELMIN03");
        C2DA* p2DA = m_pVisualEffectsPersistent;
        COL2DA_PERSISTENTVISUALEFFECT_MODELMIN03 = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("MODEL03");
        C2DA* p2DA = m_pVisualEffectsPersistent;
        COL2DA_PERSISTENTVISUALEFFECT_MODEL03 = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("NUMACT01");
        C2DA* p2DA = m_pVisualEffectsPersistent;
        COL2DA_PERSISTENTVISUALEFFECT_NUMACT01 = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("NUMACT02");
        C2DA* p2DA = m_pVisualEffectsPersistent;
        COL2DA_PERSISTENTVISUALEFFECT_NUMACT02 = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("NUMACT03");
        C2DA* p2DA = m_pVisualEffectsPersistent;
        COL2DA_PERSISTENTVISUALEFFECT_NUMACT03 = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("DURATION01");
        C2DA* p2DA = m_pVisualEffectsPersistent;
        COL2DA_PERSISTENTVISUALEFFECT_DURATION01 = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("DURATION02");
        C2DA* p2DA = m_pVisualEffectsPersistent;
        COL2DA_PERSISTENTVISUALEFFECT_DURATION02 = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("DURATION03");
        C2DA* p2DA = m_pVisualEffectsPersistent;
        COL2DA_PERSISTENTVISUALEFFECT_DURATION03 = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("EDGEWGHT01");
        C2DA* p2DA = m_pVisualEffectsPersistent;
        COL2DA_PERSISTENTVISUALEFFECT_EDGEWGHT01 = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("EDGEWGHT02");
        C2DA* p2DA = m_pVisualEffectsPersistent;
        COL2DA_PERSISTENTVISUALEFFECT_EDGEWGHT02 = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("EDGEWGHT03");
        C2DA* p2DA = m_pVisualEffectsPersistent;
        COL2DA_PERSISTENTVISUALEFFECT_EDGEWGHT03 = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("SoundOneShot");
        C2DA* p2DA = m_pVisualEffectsPersistent;
        COL2DA_PERSISTENTVISUALEFFECT_SOUNDONESHOT = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("SoundOneShotPercentage");
        C2DA* p2DA = m_pVisualEffectsPersistent;
        COL2DA_PERSISTENTVISUALEFFECT_SOUNDONESHOTPERCENTAGE =
            p2DA->GetColumnIndex(sColumn);
    }
    return 1;
}
// PC 0x0060FFE0
int CTwoDimArrays::Load2DArrays_CreatureSpeed()
{
    m_pCreatureSpeed = new C2DA("CreatureSpeed", 0);
    if (m_pCreatureSpeed == 0 || m_pCreatureSpeed->Load2DArray() == 0)
    {
        return 0;
    }

    COL2DA_CREATURESPEED_RUNRATE = m_pCreatureSpeed->GetColumnIndex("RUNRATE");
    {
        CExoString sColumn("WALKRATE");
        C2DA* p2DA = m_pCreatureSpeed;
        COL2DA_CREATURESPEED_WALKRATE = p2DA->GetColumnIndex(sColumn);
    }
    return 1;
}
// PC 0x006100F0
int CTwoDimArrays::Load2DArrays_DoorTypes()
{
    m_pDoorTypes = new C2DA("DoorTypes", 0);
    if (m_pDoorTypes == 0 || m_pDoorTypes->Load2DArray() == 0)
    {
        return 0;
    }

    COL2DA_DOORTYPES_SOUNDAPPTYPE =
        m_pDoorTypes->GetColumnIndex("SoundAppType");
    return 1;
}
// PC 0x006101C0
int CTwoDimArrays::Load2DArrays_GenericDoors()
{
    m_pGenericDoors = new C2DA("GenericDoors", 0);
    if (m_pGenericDoors == 0 || m_pGenericDoors->Load2DArray() == 0)
    {
        return 0;
    }

    COL2DA_GENERICDOORS_SOUNDAPPTYPE =
        m_pGenericDoors->GetColumnIndex("SoundAppType");
    return 1;
}
// PC 0x00610290
int CTwoDimArrays::Load2DArrays_Placeables()
{
    m_pPlaceables = new C2DA("Placeables", 0);
    if (m_pPlaceables == 0 || m_pPlaceables->Load2DArray() == 0)
    {
        return 0;
    }

    {
        CExoString sColumn("SoundAppType");
        C2DA* p2DA = m_pPlaceables;
        COL2DA_PLACEABLES_SOUNDAPPTYPE = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("StrRef");
        C2DA* p2DA = m_pPlaceables;
        COL2DA_PLACEABLES_STRREF = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("IgnoreStaticHitcheck");
        C2DA* p2DA = m_pPlaceables;
        COL2DA_PLACEABLES_IGNORESTATICHITCHECK = p2DA->GetColumnIndex(sColumn);
    }
    return 1;
}
// PC 0x006103E0
int CTwoDimArrays::Load2DArrays_PlaceableSounds()
{
    m_pPlaceableSounds = new C2DA("placeableobjsnds", 0);
    if (m_pPlaceableSounds == 0 || m_pPlaceableSounds->Load2DArray() == 0)
    {
        return 0;
    }

    COL2DA_PLACEABLESOUNDS_ARMORTYPE =
        m_pPlaceableSounds->GetColumnIndex("ArmorType");
    {
        CExoString sColumn("Locked");
        C2DA* p2DA = m_pPlaceableSounds;
        COL2DA_PLACEABLESOUNDS_LOCKED = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("Opened");
        C2DA* p2DA = m_pPlaceableSounds;
        COL2DA_PLACEABLESOUNDS_OPENED = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("Closed");
        C2DA* p2DA = m_pPlaceableSounds;
        COL2DA_PLACEABLESOUNDS_CLOSED = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("Destroyed");
        C2DA* p2DA = m_pPlaceableSounds;
        COL2DA_PLACEABLESOUNDS_DESTROYED = p2DA->GetColumnIndex(sColumn);
    }
    return 1;
}
// PC 0x006106E0
int CTwoDimArrays::Load2DArrays_AreaTransition()
{
    m_pAreaTransition = new C2DA("Loadscreens", 0);
    return m_pAreaTransition != 0 && m_pAreaTransition->Load2DArray() != 0;
}
// PC 0x00610780
int CTwoDimArrays::Load2DArrays_IPRPMelee()
{
    m_pIPRPMelee = new C2DA("IPRP_MeleeCost", 0);
    return m_pIPRPMelee != 0 && m_pIPRPMelee->Load2DArray() != 0;
}
// PC 0x006105A0
int CTwoDimArrays::Load2DArrays_ItemPropDef()
{
    m_pItemPropDef = new C2DA("ItemPropDef", 0);
    return m_pItemPropDef != 0 && m_pItemPropDef->Load2DArray() != 0;
}
// PC 0x00610640
int CTwoDimArrays::Load2DArrays_Portrait()
{
    m_pPortrait = new C2DA("Portraits", 0);
    return m_pPortrait != 0 && m_pPortrait->Load2DArray() != 0;
}
// PC 0x00611860
int CTwoDimArrays::Load2DArrays_IPRPDamage()
{
    m_pIPRPDamage = new C2DA("IPRP_DAMAGECOST", 0);
    return m_pIPRPDamage != 0 && m_pIPRPDamage->Load2DArray() != 0;
}
// PC 0x006112C0
int CTwoDimArrays::Load2DArrays_Traps()
{
    m_pTraps = new C2DA("Traps", 0);
    return m_pTraps != 0 && m_pTraps->Load2DArray() != 0;
}
// PC 0x006108C0
int CTwoDimArrays::Load2DArrays_LightColor()
{
    m_pLightColor = new C2DA("LightColor", 0);
    return m_pLightColor != 0 && m_pLightColor->Load2DArray() != 0;
}
// PC 0x00610960
int CTwoDimArrays::Load2DArrays_Cursor()
{
    m_pCursor = new C2DA("cursors", 0);
    return m_pCursor != 0 && m_pCursor->Load2DArray() != 0;
}
// PC 0x00610A00
int CTwoDimArrays::Load2DArrays_AmbientMusic()
{
    m_pAmbientMusic = new C2DA("AmbientMusic", 0);
    return m_pAmbientMusic != 0 && m_pAmbientMusic->Load2DArray() != 0;
}
// PC 0x00610AA0
int CTwoDimArrays::Load2DArrays_AmbientSound()
{
    m_pAmbientSound = new C2DA("AmbientSound", 0);
    return m_pAmbientSound != 0 && m_pAmbientSound->Load2DArray() != 0;
}
// PC 0x00610BE0
int CTwoDimArrays::Load2DArrays_FootstepSounds()
{
    m_pFootstepSounds = new C2DA("FootstepSounds", 0);
    if (m_pFootstepSounds == 0 || m_pFootstepSounds->Load2DArray() == 0)
    {
        return 0;
    }

    COL2DA_FOOTSTEPSOUNDS_PITCHOFFSET =
        m_pFootstepSounds->GetColumnIndex("PitchOffset");
    {
        CExoString sColumn("Rolling");
        C2DA* p2DA = m_pFootstepSounds;
        COL2DA_FOOTSTEPSOUNDS_ROLLING = p2DA->GetColumnIndex(sColumn);
    }
    return 1;
}
// PC 0x00610CF0
int CTwoDimArrays::Load2DArrays_WeaponSounds()
{
    m_pWeaponSounds = new C2DA("WeaponSounds", 0);
    if (m_pWeaponSounds == 0 || m_pWeaponSounds->Load2DArray() == 0)
    {
        return 0;
    }

    COL2DA_WEAPONSOUNDS_PITCHOFFSET =
        m_pWeaponSounds->GetColumnIndex("PitchOffset");
    return 1;
}

int CTwoDimArrays::Load2DArrays_AmmunitionTypes()
{
    m_pAmmunitionTypes = new C2DA("AmmunitionTypes", 0);
    return m_pAmmunitionTypes != 0 && m_pAmmunitionTypes->Load2DArray() != 0;
}

#ifndef _XBOX
int CTwoDimArrays::Load2DArrays_Keymap()  // PC 0x00610DC0
{
    m_pKeymap = new C2DA("keymap", 0);
    return m_pKeymap != 0 && m_pKeymap->Load2DArray() != 0;
}
// PC 0x00610E60
int CTwoDimArrays::Load2DArrays_BindableKey()
{
    m_pBindableKey = new C2DA("BindableKeys", 0);
    return m_pBindableKey != 0 && m_pBindableKey->Load2DArray() != 0;
}
#endif
// PC 0x00610F00
int CTwoDimArrays::Load2DArrays_CameraStyle()
{
    m_pCameraStyle = new C2DA("CameraStyle", 0);
    return m_pCameraStyle != 0 && m_pCameraStyle->Load2DArray() != 0;
}
// PC 0x00612D30
int CTwoDimArrays::Load2DArrays_Tutorial()
{
    m_pTutorial = new C2DA("tutorial", 0);
    return m_pTutorial != 0 && m_pTutorial->Load2DArray() != 0;
}
// PC 0x00611040
int CTwoDimArrays::Load2DArrays_DifficultyOptions()
{
    m_pDifficultyOptions = new C2DA("difficultyopt", 0);
    return m_pDifficultyOptions != 0 &&
           m_pDifficultyOptions->Load2DArray() != 0;
}
// PC 0x006110E0
int CTwoDimArrays::Load2DArrays_Gamma()
{
    m_pGamma = new C2DA("gamma", 0);
    return m_pGamma != 0 && m_pGamma->Load2DArray() != 0;
}
// PC 0x00611180
int CTwoDimArrays::Load2DArrays_StateScripts()
{
    m_pStateScripts = new C2DA("StateScripts", 0);
    return m_pStateScripts != 0 && m_pStateScripts->Load2DArray() != 0;
}
// PC 0x00611360
int CTwoDimArrays::Load2DArrays_Poison()
{
    m_pPoison = new C2DA("poison", 0);
    return m_pPoison != 0 && m_pPoison->Load2DArray() != 0;
}
// PC 0x00611400
int CTwoDimArrays::Load2DArrays_Disease()
{
    m_pDisease = new C2DA("disease", 0);
    return m_pDisease != 0 && m_pDisease->Load2DArray() != 0;
}
// PC 0x006114A0
int CTwoDimArrays::Load2DArrays_RepAdjustments()
{
    m_pRepAdjustments = new C2DA("RepAdjust", 0);
    return m_pRepAdjustments != 0 && m_pRepAdjustments->Load2DArray() != 0;
}
// PC 0x00611540
int CTwoDimArrays::Load2DArrays_FractionalCR()
{
    m_pFractionalCR = new C2DA("FractionalCR", 0);
    return m_pFractionalCR != 0 && m_pFractionalCR->Load2DArray() != 0;
}
// PC 0x00611720
int CTwoDimArrays::Load2DArrays_EncounterDifficulty()
{
    m_pEncounterDifficulty = new C2DA("EncDifficulty", 0);
    return m_pEncounterDifficulty != 0 &&
           m_pEncounterDifficulty->Load2DArray() != 0;
}
// PC 0x006115E0
int CTwoDimArrays::Load2DArrays_ExcitedDuration()
{
    m_pExcitedDuration = new C2DA("ExcitedDuration", 0);
    return m_pExcitedDuration != 0 && m_pExcitedDuration->Load2DArray() != 0;
}
// PC 0x00611680
int CTwoDimArrays::Load2DArrays_Regeneration()
{
    m_pRegeneration = new C2DA("Regeneration", 0);
    if (m_pRegeneration == 0 || m_pRegeneration->Load2DArray() == 0)
    {
        return 0;
    }

    COL2DA_REGENERATION_FORCEREGEN =
        m_pRegeneration->GetColumnIndex("ForceRegen");
    {
        CExoString sColumn("HealthRegen");
        C2DA* p2DA = m_pRegeneration;
        COL2DA_REGENERATION_HEALTHREGEN = p2DA->GetColumnIndex(sColumn);
    }
    return 1;
}
// PC 0x00611AE0
int CTwoDimArrays::Load2DArrays_XpBase()
{
    m_pXpBase = new C2DA("XPTable", 0);
    return m_pXpBase != 0 && m_pXpBase->Load2DArray() != 0;
}
// PC 0x006117C0
int CTwoDimArrays::Load2DArrays_IPRPMonsterDamage()
{
    m_pIPRPMonsterDamage = new C2DA("IPRP_MONSTCOST", 0);
    return m_pIPRPMonsterDamage != 0 &&
           m_pIPRPMonsterDamage->Load2DArray() != 0;
}
// PC 0x00611900
int CTwoDimArrays::Load2DArrays_IPRPBonusCost()
{
    m_pIPRPBonusCost = new C2DA("IPRP_BONUSCOST", 0);
    return m_pIPRPBonusCost != 0 && m_pIPRPBonusCost->Load2DArray() != 0;
}
// PC 0x006119A0
int CTwoDimArrays::Load2DArrays_IPRPSRCost()
{
    m_pIPRPSRCost = new C2DA("IPRP_SRCOST", 0);
    return m_pIPRPSRCost != 0 && m_pIPRPSRCost->Load2DArray() != 0;
}
// PC 0x00611A40
int CTwoDimArrays::Load2DArrays_IPRPNeg5Cost()
{
    m_pIPRPNeg5Cost = new C2DA("IPRP_NEG5COST", 0);
    return m_pIPRPNeg5Cost != 0 && m_pIPRPNeg5Cost->Load2DArray() != 0;
}
// PC 0x00611B80
int CTwoDimArrays::Load2DArrays_Ranges()
{
    m_pRanges = new C2DA("Ranges", 0);
    return m_pRanges != 0 && m_pRanges->Load2DArray() != 0;
}
// PC 0x00611C20
int CTwoDimArrays::Load2DArrays_OnHit()
{
    m_pOnHit = new C2DA("IPRP_ONHIT", 0);
    return m_pOnHit != 0 && m_pOnHit->Load2DArray() != 0;
}
// PC 0x00612240
int CTwoDimArrays::Load2DArrays_OnHitDuration()
{
    m_pOnHitDuration = new C2DA("IPRP_ONHITDUR", 0);
    return m_pOnHitDuration != 0 && m_pOnHitDuration->Load2DArray() != 0;
}
// PC 0x00612080
int CTwoDimArrays::Load2DArrays_DamageHitVisual()
{
    m_pDamageHitVisual = new C2DA("DamageHitVisual", 0);
    return m_pDamageHitVisual != 0 && m_pDamageHitVisual->Load2DArray() != 0;
}
// PC 0x00611CC0
int CTwoDimArrays::Load2DArrays_Animations()
{
    m_pAnimations = new C2DA("Animations", 0);
    if (m_pAnimations == 0 || m_pAnimations->Load2DArray() == 0)
    {
        return 0;
    }

    COL2DA_ANIMATIONS_ATTACK = m_pAnimations->GetColumnIndex("Attack");
    {
        CExoString sColumn("Damage");
        C2DA* p2DA = m_pAnimations;
        COL2DA_ANIMATIONS_DAMAGE = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("Dialog");
        C2DA* p2DA = m_pAnimations;
        COL2DA_ANIMATIONS_DIALOG = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("Dodge");
        C2DA* p2DA = m_pAnimations;
        COL2DA_ANIMATIONS_DODGE = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("FireForget");
        C2DA* p2DA = m_pAnimations;
        COL2DA_ANIMATIONS_FIREFORGET = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("HideEquippedItems");
        C2DA* p2DA = m_pAnimations;
        COL2DA_ANIMATIONS_HIDEEQUIPPEDITEMS = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("Looping");
        C2DA* p2DA = m_pAnimations;
        COL2DA_ANIMATIONS_LOOPING = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("Name");
        C2DA* p2DA = m_pAnimations;
        COL2DA_ANIMATIONS_NAME = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("Overlay");
        C2DA* p2DA = m_pAnimations;
        COL2DA_ANIMATIONS_OVERLAY = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("Parry");
        C2DA* p2DA = m_pAnimations;
        COL2DA_ANIMATIONS_PARRY = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("Pause");
        C2DA* p2DA = m_pAnimations;
        COL2DA_ANIMATIONS_PAUSE = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("Running");
        C2DA* p2DA = m_pAnimations;
        COL2DA_ANIMATIONS_RUNNING = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("Stationary");
        C2DA* p2DA = m_pAnimations;
        COL2DA_ANIMATIONS_STATIONARY = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("Walking");
        C2DA* p2DA = m_pAnimations;
        COL2DA_ANIMATIONS_WALKING = p2DA->GetColumnIndex(sColumn);
    }
    return 1;
}

int CTwoDimArrays::Load2DArrays_BodyBag()
{
    m_pBodyBag = new C2DA("bodybag", 0);
    return m_pBodyBag != 0 && m_pBodyBag->Load2DArray() != 0;
}
// PC 0x00612120
int CTwoDimArrays::Load2DArrays_ForceAdjust()
{
    m_pForcePointAdjust = new C2DA("ForceAdjust", 0);
    if (m_pForcePointAdjust == 0 || m_pForcePointAdjust->Load2DArray() == 0)
    {
        return 0;
    }

    COL2DA_FORCEADJUST_EVILCOST = m_pForcePointAdjust->GetColumnIndex("EvilCost");
    {
        CExoString sColumn("GoodCost");
        C2DA* p2DA = m_pForcePointAdjust;
        COL2DA_FORCEADJUST_GOODCOST = p2DA->GetColumnIndex(sColumn);
    }
    return 1;
}
// PC 0x006122E0
int CTwoDimArrays::Load2DArrays_GameEffects()
{
    m_pGameEffects = new C2DA("GameEffects", 0);
    return m_pGameEffects != 0 && m_pGameEffects->Load2DArray() != 0;
}
// PC 0x00612380
int CTwoDimArrays::Load2DArrays_Repute()
{
    m_pRepute = new C2DA("Repute", 0);
    return m_pRepute != 0 && m_pRepute->Load2DArray() != 0;
}
// PC 0x00612420
int CTwoDimArrays::Load2DArrays_WeaponDischarge()
{
    m_pWeaponDischarge = new C2DA("WeaponDischarge", 0);
    return m_pWeaponDischarge != 0 && m_pWeaponDischarge->Load2DArray() != 0;
}
// PC 0x006124C0
int CTwoDimArrays::Load2DArrays_DroidDischarge()
{
    m_pDroidDischarge = new C2DA("DroidDischarge", 0);
    return m_pDroidDischarge != 0 && m_pDroidDischarge->Load2DArray() != 0;
}
// PC 0x00612560
int CTwoDimArrays::Load2DArrays_PlotXP()
{
    m_pPlotXP = new C2DA("Plot", 0);
    return m_pPlotXP != 0 && m_pPlotXP->Load2DArray() != 0;
}
// PC 0x00610FA0
int CTwoDimArrays::Load2DArrays_CombatAnimations()
{
    m_pCombatAnimations = new C2DA("CombatAnimations", 0);
    return m_pCombatAnimations != 0 && m_pCombatAnimations->Load2DArray() != 0;
}
// PC 0x00612600
int CTwoDimArrays::Load2DArrays_DialogAnimations()
{
    m_pDialogAnimations = new C2DA("DialogAnimations", 0);
    if (m_pDialogAnimations == 0 || m_pDialogAnimations->Load2DArray() == 0)
    {
        return 0;
    }

    COL2DA_DIALOGANIMATIONS_DIALOG =
        m_pDialogAnimations->GetColumnIndex("Dialog");
    {
        CExoString sColumn("FireForget");
        C2DA* p2DA = m_pDialogAnimations;
        COL2DA_DIALOGANIMATIONS_FIREFORGET = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("Looping");
        C2DA* p2DA = m_pDialogAnimations;
        COL2DA_DIALOGANIMATIONS_LOOPING = p2DA->GetColumnIndex(sColumn);
    }
    {
        CExoString sColumn("Overlay");
        C2DA* p2DA = m_pDialogAnimations;
        COL2DA_DIALOGANIMATIONS_OVERLAY = p2DA->GetColumnIndex(sColumn);
    }
    return 1;
}

int CTwoDimArrays::Load2DArrays_NPC()
{
    m_pNPC = new C2DA("NPC", 0);
    return m_pNPC != 0 && m_pNPC->Load2DArray() != 0;
}
// PC 0x00612790
int CTwoDimArrays::Load2DArrays_Spells()
{
    m_pSpells = new C2DA("Spells", 0);
    return m_pSpells != 0 && m_pSpells->Load2DArray() != 0;
}
// PC 0x00612830
int CTwoDimArrays::Load2DArrays_Feat()
{
    m_pFeat = new C2DA("Feat", 0);
    return m_pFeat != 0 && m_pFeat->Load2DArray() != 0;
}
// PC 0x006128D0
int CTwoDimArrays::Load2DArrays_Formations()
{
    m_pFormations = new C2DA("Formations", 0);
    return m_pFormations != 0 && m_pFormations->Load2DArray() != 0;
}
// PC 0x00612970
int CTwoDimArrays::Load2DArrays_GrenadeSound()
{
    m_pGrenadeSound = new C2DA("GrenadeSND", 0);
    return m_pGrenadeSound != 0 && m_pGrenadeSound->Load2DArray() != 0;
}
// PC 0x00612A10
int CTwoDimArrays::Load2DArrays_Planetary()
{
    m_pJournal = new C2DA("Planetary", 0);
    return m_pJournal != 0 && m_pJournal->Load2DArray() != 0;
}
// PC 0x00612AB0
int CTwoDimArrays::Load2DArrays_FeedBackText()
{
    m_pFeedBackText = new C2DA("FeedBackText", 0);
    return m_pFeedBackText != 0 && m_pFeedBackText->Load2DArray() != 0;
}
// PC 0x00612B50
int CTwoDimArrays::Load2DArrays_ForceShields()
{
    m_pForceShields = new C2DA("ForceShields", 0);
    return m_pForceShields != 0 && m_pForceShields->Load2DArray() != 0;
}
// PC 0x00610820
int CTwoDimArrays::Load2DArrays_Heads()
{
    m_pHeads = new C2DA("Heads", 0);
    return m_pHeads != 0 && m_pHeads->Load2DArray() != 0;
}
// PC 0x00612C90
int CTwoDimArrays::Load2DArrays_ItemValues()
{
    m_pItemValues = new C2DA("ItemValue", 0);
    return m_pItemValues != 0 && m_pItemValues->Load2DArray() != 0;
}
// PC 0x00612BF0
int CTwoDimArrays::Load2DArrays_VideoEffects()
{
    m_pVideoEffects = new C2DA("VideoEffects", 0);
    return m_pVideoEffects != 0 && m_pVideoEffects->Load2DArray() != 0;
}
// PC 0x00612DD0
int CTwoDimArrays::Load2DArrays_Movies()
{
    m_pMovies2DA = new C2DA("Movies", 0);
    return m_pMovies2DA != 0 && m_pMovies2DA->Load2DArray() != 0;
}
// PC 0x00613090
int CTwoDimArrays::Load2DArrays_EffectIcon()
{
    m_pEffectIcon = new C2DA("EffectIcon", 0);
    if (m_pEffectIcon == 0)
    {
        return 0;
    }
    if (m_pEffectIcon->Load2DArray() == 0)
    {
        delete m_pEffectIcon;
        m_pEffectIcon = 0;
        return 0;
    }
    return 1;
}
// PC 0x00612F10
int CTwoDimArrays::Load2DArrays_RemoveFXOnDeath()
{
    m_pRemoveFXOnDeath = new C2DA("removefxondeath", 0);
    if (m_pRemoveFXOnDeath == 0)
    {
        return 0;
    }
    if (m_pRemoveFXOnDeath->Load2DArray() == 0)
    {
        delete m_pRemoveFXOnDeath;
        m_pRemoveFXOnDeath = 0;
        return 0;
    }
    return 1;
}

#ifndef _XBOX
int CTwoDimArrays::Load2DArrays_TexPacks()  // PC 0x00612FD0
{
    m_pTexPacks = new C2DA("TexPacks", 0);
    if (m_pTexPacks == 0)
    {
        return 0;
    }
    if (m_pTexPacks->Load2DArray() == 0)
    {
        delete m_pTexPacks;
        m_pTexPacks = 0;
        return 0;
    }
    return 1;
}
#endif
// PC 0x00613830
int CTwoDimArrays::LoadIPRPCostTables()
{
    int bClientLoad;
    C2DA* pTable = new C2DA("IPRP_COSTTABLE", 0);

    if (pTable != 0 && pTable->Load2DArray() != 0)
    {
        int nTableCount = pTable->m_nNumRows;
        m_nIPRPCostTableCount = nTableCount;

        CExoString sResRef;
        m_ppIPRPCostTables = new C2DA*[m_nIPRPCostTableCount];

        for (int i = 0; i < m_nIPRPCostTableCount; i++)
        {
            m_ppIPRPCostTables[i] = 0;
            bClientLoad = 1;
            if (g_nClientServerMode == 1)
            {
                pTable->GetINTEntry(i, "ClientLoad", &bClientLoad);
                if (bClientLoad == 0)
                {
                    continue;
                }
            }
            {
                CExoString sColumn("Name");
                pTable->GetCExoStringEntry(i, sColumn, &sResRef);
            }

            m_ppIPRPCostTables[i] = new C2DA((char*)sResRef.CStr(), 0);
            if (m_ppIPRPCostTables[i]->Load2DArray() == 0)
            {
                delete pTable;
                return 0;
            }
        }

        delete pTable;
        return 1;
    }
    return 0;
}
// PC 0x00613A40
int CTwoDimArrays::UnLoadIPRPCostTables()
{
    if (m_ppIPRPCostTables != 0)
    {
        for (int i = 0; i < m_nIPRPCostTableCount; i++)
        {
            if (m_ppIPRPCostTables[i] != 0)
            {
                m_ppIPRPCostTables[i]->Unload2DArray();
                delete m_ppIPRPCostTables[i];
                m_ppIPRPCostTables[i] = 0;
            }
        }
        delete[] m_ppIPRPCostTables;
        m_ppIPRPCostTables = 0;
    }
    return 1;
}
// PC 0x00613AA0
C2DA* CTwoDimArrays::GetIPRPCostTable(unsigned char nTable)
{
    C2DA* pResult = 0;

    if (nTable < m_nIPRPCostTableCount)
    {
        if (m_ppIPRPCostTables != 0)
        {
            pResult = m_ppIPRPCostTables[nTable];
        EXOXBOXASSERT(pResult);
        }
    }
    return pResult;
}
// PC 0x00613AC0
int CTwoDimArrays::LoadIPRPParamTables()
{
    C2DA* pTable = new C2DA("IPRP_PARAMTABLE", 0);

    if (pTable != 0 && pTable->Load2DArray() != 0)
    {
        int nTableCount = pTable->m_nNumRows;
        m_nIPRPParamTableCount = nTableCount;

        CExoString sResRef;
        m_ppIPRPParamTables = new C2DA*[m_nIPRPParamTableCount];

        for (int i = 0; i < m_nIPRPParamTableCount; i++)
        {
            m_ppIPRPParamTables[i] = 0;

            pTable->GetCExoStringEntry(i, "TableResRef", &sResRef);

            m_ppIPRPParamTables[i] = new C2DA((char*)sResRef.CStr(), 0);
            if (m_ppIPRPParamTables[i]->Load2DArray() == 0)
            {
                delete pTable;
                return 0;
            }
        }

        delete pTable;
        return 1;
    }
    return 0;
}
// PC 0x00613C90
int CTwoDimArrays::UnLoadIPRPParamTables()
{
    if (m_ppIPRPParamTables != 0)
    {
        for (int i = 0; i < m_nIPRPParamTableCount; i++)
        {
            if (m_ppIPRPParamTables[i] != 0)
            {
                m_ppIPRPParamTables[i]->Unload2DArray();
                delete m_ppIPRPParamTables[i];
                m_ppIPRPParamTables[i] = 0;
            }
        }
        delete[] m_ppIPRPParamTables;
        m_ppIPRPParamTables = 0;
    }
    return 1;
}
// PC 0x00613D00
C2DA* CTwoDimArrays::GetIPRPParamTable(unsigned char nTable)
{
    C2DA* pResult = 0;

    if (nTable < m_nIPRPParamTableCount)
    {
        if (m_ppIPRPParamTables != 0)
        {
            pResult = m_ppIPRPParamTables[nTable];
        EXOXBOXASSERT(pResult);
        }
    }
    return pResult;
}
// PC 0x00613D20
int CTwoDimArrays::Load2DArrays()
{
    LoadIPRPCostTables();
    LoadIPRPParamTables();
    if (Load2DArrays_Appearance() == 0)
        return 0;
    if (Load2DArrays_Gender() == 0)
        return 0;
    if (Load2DArrays_SurfaceMaterial() == 0)
        return 0;
    if (Load2DArrays_VisualEffect() == 0)
        return 0;
    if (Load2DArrays_PersistentVisualEffect() == 0)
        return 0;
    if (Load2DArrays_CreatureSpeed() == 0)
        return 0;
    if (Load2DArrays_DoorTypes() == 0)
        return 0;
    if (Load2DArrays_GenericDoors() == 0)
        return 0;
    if (Load2DArrays_Placeables() == 0)
        return 0;
    if (Load2DArrays_AreaTransition() == 0)
        return 0;
    if (Load2DArrays_IPRPMelee() == 0)
        return 0;
    if (Load2DArrays_ItemPropDef() == 0)
        return 0;
    if (Load2DArrays_Portrait() == 0)
        return 0;
    if (Load2DArrays_IPRPDamage() == 0)
        return 0;
    if (Load2DArrays_Traps() == 0)
        return 0;
    if (Load2DArrays_LightColor() == 0)
        return 0;

    if (g_nClientServerMode != 2)
    {
        if (Load2DArrays_Cursor() == 0)
            return 0;
        if (Load2DArrays_AmbientMusic() == 0)
            return 0;
        if (Load2DArrays_AmbientSound() == 0)
            return 0;
        if (Load2DArrays_FootstepSounds() == 0)
            return 0;
        if (Load2DArrays_AppearanceSounds() == 0)
            return 0;
        if (Load2DArrays_WeaponSounds() == 0)
            return 0;
        if (Load2DArrays_AmmunitionTypes() == 0)
            return 0;
#ifndef _XBOX
        if (Load2DArrays_Keymap() == 0)
            return 0;
        if (Load2DArrays_BindableKey() == 0)
            return 0;
#endif
        if (Load2DArrays_PlaceableSounds() == 0)
            return 0;
        if (Load2DArrays_CameraStyle() == 0)
            return 0;
        if (Load2DArrays_Tutorial() == 0)
            return 0;
        if (Load2DArrays_DifficultyOptions() == 0)
            return 0;
        if (Load2DArrays_Gamma() == 0)
            return 0;
    }

    if (g_nClientServerMode != 1)
    {
        if (Load2DArrays_StateScripts() == 0)
            return 0;
        if (Load2DArrays_Poison() == 0)
            return 0;
        if (Load2DArrays_Disease() == 0)
            return 0;
        if (Load2DArrays_RepAdjustments() == 0)
            return 0;
        if (Load2DArrays_FractionalCR() == 0)
            return 0;
        if (Load2DArrays_EncounterDifficulty() == 0)
            return 0;
        if (Load2DArrays_ExcitedDuration() == 0)
            return 0;
        if (Load2DArrays_Regeneration() == 0)
            return 0;
        if (Load2DArrays_XpBase() == 0)
            return 0;
        if (Load2DArrays_IPRPMonsterDamage() == 0)
            return 0;
        if (Load2DArrays_IPRPBonusCost() == 0)
            return 0;
        if (Load2DArrays_IPRPSRCost() == 0)
            return 0;
        if (Load2DArrays_IPRPNeg5Cost() == 0)
            return 0;
        if (Load2DArrays_Ranges() == 0)
            return 0;
        if (Load2DArrays_OnHit() == 0)
            return 0;
        if (Load2DArrays_OnHitDuration() == 0)
            return 0;
        if (Load2DArrays_DamageHitVisual() == 0)
            return 0;
        if (Load2DArrays_Animations() == 0)
            return 0;
        if (Load2DArrays_BodyBag() == 0)
            return 0;
        if (Load2DArrays_ForceAdjust() == 0)
            return 0;
        if (Load2DArrays_GameEffects() == 0)
            return 0;
        if (Load2DArrays_GameEffects() == 0)
            return 0;
        if (Load2DArrays_Repute() == 0)
            return 0;
        if (Load2DArrays_WeaponDischarge() == 0)
            return 0;
        if (Load2DArrays_DroidDischarge() == 0)
            return 0;
        if (Load2DArrays_PlotXP() == 0)
            return 0;
        if (Load2DArrays_CombatAnimations() == 0)
            return 0;
        if (Load2DArrays_DialogAnimations() == 0)
            return 0;
        if (Load2DArrays_NPC() == 0)
            return 0;
        if (Load2DArrays_Spells() == 0)
            return 0;
        if (Load2DArrays_Feat() == 0)
            return 0;
        if (Load2DArrays_Formations() == 0)
            return 0;
        if (Load2DArrays_GrenadeSound() == 0)
            return 0;
        if (Load2DArrays_Planetary() == 0)
            return 0;
        if (Load2DArrays_FeedBackText() == 0)
            return 0;
        if (Load2DArrays_ForceShields() == 0)
            return 0;
        if (Load2DArrays_Heads() == 0)
            return 0;
        if (Load2DArrays_ItemValues() == 0)
            return 0;
        if (Load2DArrays_VideoEffects() == 0)
            return 0;
        if (Load2DArrays_Movies() == 0)
            return 0;
        if (Load2DArrays_EffectIcon() == 0)
            return 0;
        if (Load2DArrays_RemoveFXOnDeath() == 0)
            return 0;
#ifndef _XBOX
        if (Load2DArrays_TexPacks() == 0)
            return 0;
#endif
    }

    return 1;
}
// PC 0x00614210
int CTwoDimArrays::UnLoad2DArrays()
{
    UnLoadIPRPCostTables();
    UnLoadIPRPParamTables();
#ifdef _XBOX
    if (m_pMinGlobalRim != 0)
        m_pMinGlobalRim->Unload2DArray();
#endif
    if (m_pRemoveFXOnDeath != 0)
        m_pRemoveFXOnDeath->Unload2DArray();
    if (m_pEffectIcon != 0)
        m_pEffectIcon->Unload2DArray();
    if (m_pAppearance != 0)
        m_pAppearance->Unload2DArray();
    if (m_pGender != 0)
        m_pGender->Unload2DArray();
    if (m_pSurfaceMaterial != 0)
        m_pSurfaceMaterial->Unload2DArray();
    if (m_pStateScripts != 0)
        m_pStateScripts->Unload2DArray();
    if (m_pVisualEffect != 0)
        m_pVisualEffect->Unload2DArray();
    if (m_pAreaTransition != 0)
        m_pAreaTransition->Unload2DArray();
    if (m_pTraps != 0)
        m_pTraps->Unload2DArray();
    if (m_pVisualEffectsPersistent != 0)
        m_pVisualEffectsPersistent->Unload2DArray();
    if (m_pCreatureSpeed != 0)
        m_pCreatureSpeed->Unload2DArray();
    if (m_pDoorTypes != 0)
        m_pDoorTypes->Unload2DArray();
    if (m_pPlaceables != 0)
        m_pPlaceables->Unload2DArray();
    if (m_pGenericDoors != 0)
        m_pGenericDoors->Unload2DArray();
    if (m_pPoison != 0)
        m_pPoison->Unload2DArray();
    if (m_pDisease != 0)
        m_pDisease->Unload2DArray();
    if (m_pRepAdjustments != 0)
        m_pRepAdjustments->Unload2DArray();
    if (m_pFractionalCR != 0)
        m_pFractionalCR->Unload2DArray();
    if (m_pEncounterDifficulty != 0)
        m_pEncounterDifficulty->Unload2DArray();
    if (m_pExcitedDuration != 0)
        m_pExcitedDuration->Unload2DArray();
    if (m_pRegeneration != 0)
        m_pRegeneration->Unload2DArray();
    if (m_pLightColor != 0)
        m_pLightColor->Unload2DArray();
    if (m_pIPRPMelee != 0)
        m_pIPRPMelee->Unload2DArray();
    if (m_pItemPropDef != 0)
        m_pItemPropDef->Unload2DArray();
    if (m_pXpBase != 0)
        m_pXpBase->Unload2DArray();
    if (m_pCursor != 0)
        m_pCursor->Unload2DArray();
    if (m_pIPRPMonsterDamage != 0)
        m_pIPRPMonsterDamage->Unload2DArray();
    if (m_pIPRPDamage != 0)
        m_pIPRPDamage->Unload2DArray();
    if (m_pIPRPBonusCost != 0)
        m_pIPRPBonusCost->Unload2DArray();
    if (m_pIPRPSRCost != 0)
        m_pIPRPSRCost->Unload2DArray();
    if (m_pIPRPNeg5Cost != 0)
        m_pIPRPNeg5Cost->Unload2DArray();
    if (m_pAmbientMusic != 0)
        m_pAmbientMusic->Unload2DArray();
    if (m_pAmbientSound != 0)
        m_pAmbientMusic->Unload2DArray();
    if (m_pFootstepSounds != 0)
        m_pFootstepSounds->Unload2DArray();
    if (m_pAppearanceSounds != 0)
        m_pAppearanceSounds->Unload2DArray();
    if (m_pWeaponSounds != 0)
        m_pWeaponSounds->Unload2DArray();
    if (m_pAmmunitionTypes != 0)
        m_pAmmunitionTypes->Unload2DArray();
#ifndef _XBOX
    if (m_pKeymap != 0)
        m_pKeymap->Unload2DArray();
    if (m_pBindableKey != 0)
        m_pBindableKey->Unload2DArray();
#endif
    if (m_pOnHit != 0)
        m_pOnHit->Unload2DArray();
    if (m_pOnHitDuration != 0)
        m_pOnHitDuration->Unload2DArray();
    if (m_pDamageHitVisual != 0)
        m_pDamageHitVisual->Unload2DArray();
    if (m_pPlaceableSounds != 0)
        m_pPlaceableSounds->Unload2DArray();
    if (m_pBodyBag != 0)
        m_pBodyBag->Unload2DArray();
    if (m_pPortrait != 0)
        m_pPortrait->Unload2DArray();
    if (m_pCameraStyle != 0)
        m_pCameraStyle->Unload2DArray();
    if (m_pRepute != 0)
        m_pRepute->Unload2DArray();
    if (m_pPlotXP != 0)
        m_pPlotXP->Unload2DArray();
    if (m_pNPC != 0)
        m_pNPC->Unload2DArray();
    if (m_pSpells != 0)
        m_pSpells->Unload2DArray();
    if (m_pFeat != 0)
        m_pFeat->Unload2DArray();
    if (m_pFormations != 0)
        m_pFormations->Unload2DArray();
    if (m_pGrenadeSound != 0)
        m_pGrenadeSound->Unload2DArray();
    if (m_pJournal != 0)
        m_pJournal->Unload2DArray();
    if (m_pFeedBackText != 0)
        m_pFeedBackText->Unload2DArray();
    if (m_pItemValues != 0)
        m_pItemValues->Unload2DArray();
    if (m_pVideoEffects != 0)
        m_pVideoEffects->Unload2DArray();
    if (m_pTutorial != 0)
        m_pTutorial->Unload2DArray();
    if (m_pDifficultyOptions != 0)
        m_pDifficultyOptions->Unload2DArray();
    if (m_pGamma != 0)
        m_pGamma->Unload2DArray();
#ifndef _XBOX
    if (m_pTexPacks != 0)
        m_pTexPacks->Unload2DArray();
#endif
    return 1;
}
